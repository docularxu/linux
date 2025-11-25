// SPDX-License-Identifier: GPL-2.0
//
// SpacemiT K1 SPI controller driver
//
// Copyright (C) 2025 by RISCstar Solutions Corporation.  All rights reserved.
// Copyright (c) 2023, spacemit Corporation.

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/dmaengine.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/scatterlist.h>
#include <linux/sizes.h>
#include <linux/spi/spi.h>
#include <linux/units.h>

#include "internals.h"

/* This is the range of transfer rates supported by the K1 SoC */
#define K1_SPI_MIN_SPEED_HZ		6250
#define K1_SPI_MAX_SPEED_HZ		51200000

/* DMA constraints */
#define K1_SPI_DMA_ALIGNMENT		64
#define K1_SPI_MAX_DMA_LEN	SZ_512K

/* SSP Top Control Register */
#define SSP_TOP_CTRL		0x00
#define TOP_SSE				BIT(0)		/* Enable port */
#define TOP_FRF_MASK			GENMASK(2, 1)	/* Frame format */
#define TOP_FRF_MOTOROLA			0	/* Motorola SPI */
#define TOP_DSS_MASK			GENMASK(9, 5)	/* Data size (1-32) */
#define TOP_SPO				BIT(10)		/* Polarity: 0=low */
#define TOP_SPH				BIT(11)		/* Half-cycle phase */
#define TOP_LBM				BIT(12)		/* Loopback mode */
#define TOP_HOLD_FRAME_LOW		BIT(14)		/* Chip select */

/* SSP FIFO Control Register */
#define SSP_FIFO_CTRL		0x04
#define FIFO_TFT_MASK			GENMASK(4, 0)	/* TX FIFO threshold */
#define FIFO_RFT_MASK			GENMASK(9, 5)	/* RX FIFO threshold */

/* SSP Interrupt Enable Register */
#define SSP_INT_EN		0x08
#define SSP_INT_EN_TINTE		BIT(1)		/* RX timeout */
#define SSP_INT_EN_RIE			BIT(2)		/* RX FIFO */
#define SSP_INT_EN_TIE			BIT(3)		/* TX FIFO */
#define SSP_INT_EN_RIM			BIT(4)		/* RX FIFO overrun */
#define SSP_INT_EN_TIM			BIT(5)		/* TX FIFO underrun */
#define SSP_INT_EN_EBCEI		BIT(6)		/* Bit count error */

/* TX interrupts, RX interrupts, and error interrupts */
#define SSP_INT_EN_TX		SSP_INT_EN_TIE
#define SSP_INT_EN_RX \
		(SSP_INT_EN_TINTE | SSP_INT_EN_RIE)
#define SSP_INT_EN_ERROR \
		(SSP_INT_EN_RIM | SSP_INT_EN_TIM | SSP_INT_EN_EBCEI)

/* SSP Time Out Register */
#define SSP_TIMEOUT		0x0c
#define SSP_TIMEOUT_MASK		GENMASK(23, 0)

/* SSP Data Register */
#define SSP_DATAR		0x10

/* SSP Status Register */
#define SSP_STATUS		0x14
#define SSP_STATUS_BSY			BIT(0)		/* SPI/I2S busy */
#define SSP_STATUS_TNF			BIT(6)		/* TX FIFO not full */
#define SSP_STATUS_TFL			GENMASK(11, 7)	/* TX FIFO level */
#define SSP_STATUS_TUR			BIT(12)		/* TX FIFO underrun */
#define SSP_STATUS_RNE			BIT(14)		/* RX FIFO not empty */
#define SSP_STATUS_RFL			GENMASK(19, 15)	/* RX FIFO level */
#define SSP_STATUS_ROR			BIT(20)		/* RX FIFO overrun */
#define SSP_STATUS_BCE			BIT(21)		/* Bit count error */

/* Error status mask */
#define SSP_STATUS_ERROR \
		(SSP_STATUS_TUR | SSP_STATUS_ROR |SSP_STATUS_BCE)

/* The FIFO sizes and thresholds are the same for RX and TX */
#define K1_SPI_FIFO_SIZE	32
#define K1_SPI_THRESH		(K1_SPI_FIFO_SIZE / 2)

struct k1_spi_driver_data {
	struct spi_controller *host;
	void __iomem *base;
	unsigned long bus_rate;
	struct clk *clk;
	unsigned long rate;
	int irq;

	/* Current transfer information; not valid if message is null */
	u32 bytes;			/* Bytes used for bits_per_word */
	unsigned int rx_resid;		/* RX bytes left in transfer */
	unsigned int tx_resid;		/* TX bytes left in transfer */
	struct spi_transfer *transfer;	/* Current transfer */

	bool dma_enabled;
};

/* Set our registers to a known initial state */
static void
k1_spi_register_reset(struct k1_spi_driver_data *drv_data, bool initial)
{
	u32 val = 0;

	writel(0, drv_data->base + SSP_TOP_CTRL);

	if (initial) {
		/*
		 * The TX and RX FIFO thresholds are the same no matter
		 * what the speed or bits per word, so we can just set
		 * them once.  The thresholds are one more than the values
		 * in the register.
		 */
		val = FIELD_PREP(FIFO_RFT_MASK, K1_SPI_THRESH - 1);
		val |= FIELD_PREP(FIFO_TFT_MASK, K1_SPI_THRESH - 1);
	}
	writel(val, drv_data->base + SSP_FIFO_CTRL);

	writel(0, drv_data->base + SSP_INT_EN);
	writel(0, drv_data->base + SSP_TIMEOUT);

	/* Clear any pending interrupt conditions */
	writel(~0, drv_data->base + SSP_STATUS);
}

/*
 * The client can call the setup function multiple times, and each call
 * can specify a different SPI mode (and transfer speed).  Each transfer
 * can specify its own speed though, and the core code ensures each
 * transfer's speed is set to something nonzero and supported by both
 * the controller and the device.  We just set the speed for each transfer.
 */
static int k1_spi_setup(struct spi_device *spi)
{
	struct k1_spi_driver_data *drv_data;
	u32 val;

	drv_data = spi_controller_get_devdata(spi->controller);

	/*
	 * Configure the message format for this device.  We only
	 * support Motorola SPI format in master mode.
	 */
	val = FIELD_PREP(TOP_FRF_MASK, TOP_FRF_MOTOROLA);

	/* Translate the mode into the value used to program the hardware. */
	if (spi->mode & SPI_CPHA)
		val |= TOP_SPH;		/* 1/2 cycle */
	if (spi->mode & SPI_CPOL)
		val |= TOP_SPO;		/* active low */
	if (spi->mode & SPI_LOOP)
		val |= TOP_LBM;		/* enable loopback */
	writel(val, drv_data->base + SSP_TOP_CTRL);

	return 0;
}

static void k1_spi_cleanup(struct spi_device *spi)
{
	struct k1_spi_driver_data *drv_data;

	drv_data = spi_controller_get_devdata(spi->controller);
	k1_spi_register_reset(drv_data, false);
}

static bool k1_spi_can_dma(struct spi_controller *host, struct spi_device *spi,
			   struct spi_transfer *transfer)
{
	return false;
}

/* Flush the RX FIFO of any leftover data before processing a message */
static int k1_spi_prepare_message(struct spi_controller *host,
				  struct spi_message *message)
{
	struct k1_spi_driver_data *drv_data = spi_controller_get_devdata(host);
	u32 val = readl(drv_data->base + SSP_STATUS);
	u32 count;

	/* If there's nothing in the FIFO, we're done */
	if (!(val & SSP_STATUS_RNE))
		return 0;

	/* Read and discard what's there (one more than what the field says) */
	count = FIELD_GET(SSP_STATUS_RFL, val) + 1;
	do
		(void)readl(drv_data->base + SSP_DATAR);
	while (--count);

	return 0;
}

/* Set logic level of chip select line (high=true means CS deasserted) */
static void k1_spi_set_cs(struct spi_device *spi, bool high)
{
	struct k1_spi_driver_data *drv_data;
	u32 val;

	drv_data = spi_controller_get_devdata(spi->controller);

	val = readl(drv_data->base + SSP_TOP_CTRL);
	if (high)
		val &= ~TOP_HOLD_FRAME_LOW;
	else
		val |= TOP_HOLD_FRAME_LOW;
	writel(val, drv_data->base + SSP_TOP_CTRL);
}

/* Set the transfer speed; the SPI core code ensures it is supported */
static int k1_spi_set_speed(struct k1_spi_driver_data *drv_data,
			    struct spi_transfer *transfer)
{
	struct clk *clk = drv_data->clk;
	u64 nsec_per_word;
	u64 bus_ticks;
	u32 timeout;
	u32 val;
	int ret;

	ret = clk_set_rate(clk, transfer->speed_hz);
	if (ret)
		return ret;

	drv_data->rate = clk_get_rate(clk);

	/* No need for RX FIFO timeout if we're not receiving anything */
	if (!transfer->rx_buf)
		return 0;

	/*
	 * Compute the RX FIFO inactivity timeout value that should be used.
	 * The inactivity timer restarts with each word that lands in the
	 * FIFO.  If several "word transfer times" pass without any new data
	 * in the RX FIFO, we might as well read what's there.
	 *
	 * The rate at which words land in the FIFO is determined by the
	 * word size and the transfer rate.  One bit is transferred per
	 * clock tick, and 8 (or 16 or 32) bits are transferred per word.
	 *
	 * So we can get word transfer time (in nanoseconds) from:
	 *   nsec_per_tick = NSEC_PER_SEC / drv_data->rate;
	 *   ticks_per_word = BITS_PER_BYTE * drv_data->bytes;
	 * We do the divide last for better accuracy.
	 */
	nsec_per_word = NSEC_PER_SEC * BITS_PER_BYTE * drv_data->bytes;
	nsec_per_word = DIV_ROUND_UP_ULL(nsec_per_word, drv_data->rate);

	/*
	 * The timeout (which we'll set to three word transfer times) is
	 * expressed as a number of APB clock ticks.
	 *   bus_ticks = 3 * nsec * (drv_data->bus_rate / NSEC_PER_SEC)
	 */
	bus_ticks = 3 * nsec_per_word * drv_data->bus_rate;
	timeout = DIV_ROUND_UP_ULL(bus_ticks, NSEC_PER_SEC);

	/* Set the RX timeout period (required for both DMA and PIO) */
	val = FIELD_PREP(SSP_TIMEOUT_MASK, timeout);
	writel(val, drv_data->base + SSP_TIMEOUT);

	return 0;
}

static int k1_spi_transfer_one(struct spi_controller *host,
			       struct spi_device *spi,
			       struct spi_transfer *transfer)
{
	struct k1_spi_driver_data *drv_data = spi_controller_get_devdata(host);
	u32 count;
	u32 ctrl;
	u32 val;
	int ret;

	/* Bits per word can change on a per-transfer basis */
	drv_data->bytes = spi_bpw_to_bytes(transfer->bits_per_word);

	/* Each transfer can also specify a different rate */
	ret = k1_spi_set_speed(drv_data, transfer);
	if (ret) {
		dev_err(&host->dev,
			"failed to set transfer speed: %d\n", ret);
		return ret;
	}

	/* Record how many words the len bytes represent */
	count = transfer->len / drv_data->bytes;
	drv_data->rx_resid = count;
	drv_data->tx_resid = count;

	drv_data->transfer = transfer;

	/* Set the data (word) size, and enable the port */
	ctrl = readl(drv_data->base + SSP_TOP_CTRL);
	ctrl &= ~TOP_DSS_MASK;
	ctrl |= FIELD_PREP(TOP_DSS_MASK, transfer->bits_per_word - 1);
	ctrl |= TOP_SSE;
	writel(ctrl, drv_data->base + SSP_TOP_CTRL);

	/* Clear any existing interrupt conditions */
	writel(~0, drv_data->base + SSP_STATUS);

	/* An interrupt will initiate the transfer */
	val = SSP_INT_EN_TX | SSP_INT_EN_RX | SSP_INT_EN_ERROR;
	writel(val, drv_data->base + SSP_INT_EN);

	return 1;	/* We will call spi_finalize_current_transfer() */
}

static void
k1_spi_handle_err(struct spi_controller *host, struct spi_message *message)
{
}

static int k1_spi_unprepare_message(struct spi_controller *ctlr,
				    struct spi_message *message)
{
	return 0;
}

static void k1_spi_write_word(struct k1_spi_driver_data *drv_data)
{
	struct spi_transfer *transfer = drv_data->transfer;
	u32 bytes = drv_data->bytes;
	u32 val;

	if (transfer->tx_buf) {
		const void *buf;

		buf = transfer->tx_buf + (transfer->len - drv_data->tx_resid);
		if (bytes == 1)
			val = *(u8 *)buf;
		else if (bytes == 2)
			val = *(u16 *)buf;
		else	/* bytes == 4 */
			val = *(u32 *)buf;
	} else {
		val = 0;	/* Null writer; write 1, 2, or 4 zero bytes */
	}
	/* Fill the next TX FIFO entry */
	writel(val, drv_data->base + SSP_DATAR);

	drv_data->tx_resid -= bytes;
}

static void k1_spi_write(struct k1_spi_driver_data *drv_data)
{
	unsigned int count;
	u32 val;

	val = readl(drv_data->base + SSP_STATUS);

	/* Nothing to do if there's no room in the FIFO */
	if (!(val & SSP_STATUS_TNF))
		return;

	/* Get the number of open slots in the FIFO; zero means all */
	count = FIELD_GET(SSP_STATUS_TFL, val) ? : K1_SPI_FIFO_SIZE;

	/*
	 * Limit how much we try to send at a time, to reduce the
	 * chance the other side can overrun our RX FIFO.
	 */
	count = min3(count, K1_SPI_THRESH, drv_data->tx_resid);
	do
		k1_spi_write_word(drv_data);
	while (--count);
}

static void k1_spi_read_word(struct k1_spi_driver_data *drv_data)
{
	struct spi_transfer *transfer = drv_data->transfer;
	u32 bytes = drv_data->bytes;
	u32 val;

	/* Consume the next RX FIFO entry */
	val = readl(drv_data->base + SSP_DATAR);
	if (transfer->rx_buf) {
		void *buf;

		buf = transfer->rx_buf + (transfer->len - drv_data->rx_resid);

		if (bytes == 1)
			*(u8 *)buf = val;
		else if (bytes == 2)
			*(u16 *)buf = val;
		else	/* bytes == 4 */
			*(u32 *)buf = val;
	}	/* Otherwise null reader: discard the data */

	drv_data->rx_resid -= bytes;
}

static void k1_spi_read(struct k1_spi_driver_data *drv_data)
{
	do {
		u32 val = readl(drv_data->base + SSP_STATUS);
		unsigned int count;

		/* Nothing to do if the FIFO is empty */
		if (!(val & SSP_STATUS_RNE))
			return;

		/* Get the number of filled FIFO entries */
		count = FIELD_GET(SSP_STATUS_RFL, val) + 1;
		/* Only read what we need */
		count = min(count, drv_data->rx_resid);
		do
			k1_spi_read_word(drv_data);
		while (--count);
	} while (drv_data->rx_resid);
}

static irqreturn_t k1_spi_ssp_isr(int irq, void *dev_id)
{
	struct k1_spi_driver_data *drv_data = dev_id;
	u32 val;

	/* Return immediately if we're not expecting any interrupts */
	if (!drv_data->transfer)
		return IRQ_NONE;

	/* Get status and clear pending interrupts; all are handled below */
	val = readl(drv_data->base + SSP_STATUS);
	writel(val, drv_data->base + SSP_STATUS);

	/* Check for any error conditions first */
	if (val & SSP_STATUS_ERROR) {
		drv_data->transfer->error |= SPI_TRANS_FAIL_IO;
		goto done;
	}

	/*
	 * For SPI, bytes are transferred in both directions equally, and
	 * RX always follows TX.  Start by writing if there is anything to
	 * write, then read.  Once there's no more to read, we're done.
	 */
	if (drv_data->tx_resid) {
		k1_spi_write(drv_data);

		/* If we're done writing, disable TX interrupts */
		if (!drv_data->tx_resid) {
			val = SSP_INT_EN_RX | SSP_INT_EN_ERROR;
			writel(val, drv_data->base + SSP_INT_EN);
		}
	}

	/*
	 * Read more if there's more to read, and if we didn't get it all
	 * we'll continue at the next interrupt.
	 */
	if (drv_data->rx_resid) {
		k1_spi_read(drv_data);
		if (drv_data->rx_resid)
			return IRQ_HANDLED;
	}
done:
	/* Disable the port */
	val = readl(drv_data->base + SSP_TOP_CTRL);
	val &= ~TOP_SSE;
	writel(val, drv_data->base + SSP_TOP_CTRL);

	/* Disable all interrupts */
	writel(0, drv_data->base + SSP_INT_EN);

	drv_data->transfer = NULL;

	spi_finalize_current_transfer(drv_data->host);

	return IRQ_HANDLED;
}

static int k1_spi_probe(struct platform_device *pdev)
{
	struct k1_spi_driver_data *drv_data;
	struct device *dev = &pdev->dev;
	struct reset_control *reset;
	struct spi_controller *host;
	struct resource *iores;
	struct clk *clk_bus;
	int ret;

	host = devm_spi_alloc_host(dev, sizeof(*drv_data));
	if (!host)
		return -ENOMEM;
	drv_data = spi_controller_get_devdata(host);
	drv_data->host = host;
	platform_set_drvdata(pdev, drv_data);

	drv_data->base = devm_platform_get_and_ioremap_resource(pdev, 0,
								&iores);
	if (IS_ERR(drv_data->base))
		return dev_err_probe(dev, PTR_ERR(drv_data->base),
				     "error mapping memory\n");

	/* Reset registers to a known initial state */
	k1_spi_register_reset(drv_data, true);

	clk_bus = devm_clk_get_enabled(dev, "bus");
	if (IS_ERR(clk_bus))
		return dev_err_probe(dev, PTR_ERR(clk_bus),
				     "error getting/enabling bus clock\n");
	drv_data->bus_rate = clk_get_rate(clk_bus);

	drv_data->clk = devm_clk_get_enabled(dev, "core");
	if (IS_ERR(drv_data->clk))
		return dev_err_probe(dev, PTR_ERR(drv_data->clk),
				     "error getting/enabling core clock\n");

	reset = devm_reset_control_get_exclusive_deasserted(dev, NULL);
	if (IS_ERR(reset))
		return dev_err_probe(dev, PTR_ERR(reset),
				     "error getting/deasserting reset\n");

	drv_data->irq = platform_get_irq(pdev, 0);
	if (drv_data->irq < 0)
		return dev_err_probe(dev, drv_data->irq, "error getting IRQ\n");

	ret = devm_request_irq(dev, drv_data->irq, k1_spi_ssp_isr,
			       IRQF_SHARED, dev_name(dev), drv_data);
	if (ret < 0)
		return dev_err_probe(dev, ret, "error requesting IRQ\n");

	/* Initialize the host structure, then register it */
	host->dev.of_node = dev_of_node(dev);
	host->dev.parent = dev;
	host->num_chipselect = 1;
	if (drv_data->dma_enabled)
		host->dma_alignment = K1_SPI_DMA_ALIGNMENT;
	host->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LOOP;
	host->bits_per_word_mask = SPI_BPW_RANGE_MASK(4, 32);
	host->min_speed_hz = K1_SPI_MIN_SPEED_HZ;
	host->max_speed_hz = K1_SPI_MAX_SPEED_HZ;
	host->flags = SPI_CONTROLLER_MUST_RX | SPI_CONTROLLER_MUST_TX;
	host->max_dma_len = K1_SPI_MAX_DMA_LEN;

	host->setup = k1_spi_setup;
	host->cleanup = k1_spi_cleanup;
	host->can_dma = k1_spi_can_dma;
	host->prepare_message = k1_spi_prepare_message;
	host->unprepare_message = k1_spi_unprepare_message;
	host->set_cs = k1_spi_set_cs;
	host->transfer_one = k1_spi_transfer_one;
	host->handle_err = k1_spi_handle_err;

	ret = devm_spi_register_controller(dev, host);
	if (ret)
		dev_err(dev, "error registering controller\n");

	return ret;
}

static const struct of_device_id k1_spi_dt_ids[] = {
	{ .compatible = "spacemit,k1-spi", },
	{}
};
MODULE_DEVICE_TABLE(of, k1_spi_dt_ids);

static struct platform_driver k1_spi_driver = {
	.driver = {
		.name		= "k1-spi",
		.of_match_table	= k1_spi_dt_ids,
	},
};
module_platform_driver_probe(k1_spi_driver, k1_spi_probe);

MODULE_DESCRIPTION("SpacemiT K1 SPI controller driver");
MODULE_LICENSE("GPL");
