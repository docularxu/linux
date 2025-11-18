// SPDX-License-Identifier: GPL-2.0

/*
 * SpacemiT K1 SPI controller driver
 *
 * Copyright (C) 2025 by RISCstar Solutions Corporation.  All rights reserved.
 * Copyright (c) 2023, spacemit Corporation.
 */

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

/* Default speed used if spi-max-frequency is invalid or not present */
#define K1_SPI_DEFAULT_MAX_SPEED_HZ	25600000

/* DMA constraints */
#define K1_SPI_DMA_ALIGNMENT	64
#define K1_SPI_MAX_DMA_LEN	SZ_512K

/* SpacemiT K1 SPI Registers */

/* SSP Top Control Register */
#define SSP_TOP_CTRL		0x00
#define TOP_SSE				BIT(0)		/* Enable port */
#define TOP_FRF_MASK			GENMASK(2, 1)	/* Frame format */
#define TOP_FRF_MOTOROLA			0	/* Motorola SPI */
#define TOP_DSS_MASK			GENMASK(9, 5)	/* Data size (1-32) */
#define TOP_SPO				BIT(10)		/* Polarity: 0=low */
#define TOP_SPH				BIT(11)		/* Half-cycle phase */
#define TOP_LBM				BIT(12)		/* Loopback mode */
#define TOP_TRAIL			BIT(13)		/* Trailing bytes */
#define TOP_HOLD_FRAME_LOW		BIT(14)		/* Chip select */

/* SSP FIFO Control Register */
#define SSP_FIFO_CTRL		0x04
#define FIFO_TFT_MASK			GENMASK(4, 0)	/* TX FIFO threshold */
#define FIFO_RFT_MASK			GENMASK(9, 5)	/* RX FIFO threshold */
#define FIFO_TSRE			BIT(10)		/* TX service request */
#define FIFO_RSRE			BIT(11)		/* RX service request */

/* SSP Interrupt Enable Register */
#define SSP_INT_EN		0x08
#define SSP_INT_EN_TINTE		BIT(1)		/* RX timeout */
#define SSP_INT_EN_RIE			BIT(2)		/* RX FIFO */
#define SSP_INT_EN_TIE			BIT(3)		/* TX FIFO */
#define SSP_INT_EN_RIM			BIT(4)		/* RX FIFO overrun */
#define SSP_INT_EN_TIM			BIT(5)		/* TX FIFO underrun */

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

/* The FIFO sizes and thresholds are the same for RX and TX */
#define K1_SPI_FIFO_SIZE	32
#define K1_SPI_THRESH		(K1_SPI_FIFO_SIZE / 2)

struct k1_spi_io {
	void *buf;
	unsigned int resid;
};

struct k1_spi_driver_data {
	struct spi_controller *host;
	struct device *dev;
	void __iomem *base;
	unsigned long bus_rate;
	struct clk *clk;
	unsigned long rate;
	u32 rx_timeout;
	int irq;

	struct k1_spi_io rx;
	struct k1_spi_io tx;

	struct spi_message *message;	/* Current message */

	/* Current transfer information; not valid if message is null */
	unsigned int len;
	u32 bytes;			/* Bytes used for bits_per_word */
	struct completion completion;	/* Transfer completion */
};

/* Discard any data in the RX FIFO */
static void k1_spi_flush(struct k1_spi_driver_data *drv_data)
{
	u32 val = readl(drv_data->base + SSP_STATUS);
	u32 count;

	/* If there's nothing in the FIFO, we're done */
	if (!(val & SSP_STATUS_RNE))
		return;

	/* Read and discard what's there (one more than what the field says) */
	count = FIELD_GET(SSP_STATUS_RFL, val) + 1;
	do
		(void)readl(drv_data->base + SSP_DATAR);
	while (--count);
}

/* Set the transfer speed; the SPI core code ensures it is supported */
static int k1_spi_set_speed(struct k1_spi_driver_data *drv_data, u32 rate)
{
	struct clk *clk = drv_data->clk;
	u64 nsec_per_word;
	u64 bus_ticks;
	int ret;

	ret = clk_set_rate(clk, rate);
	if (ret)
		return ret;

	drv_data->rate = clk_get_rate(clk);

	/*
	 * Compute the RX FIFO inactivity timeout value that should be used.
	 * The inactivity timer restarts with each word that lands in the
	 * FIFO.  If two or more "word transfer times" pass without any new
	 * data in the RX FIFO, we might as well read what's there.
	 *
	 * The rate at which words land in the FIFO is determined by the
	 * word size and the transfer rate.  One bit is transferred per
	 * clock tick, and 8 (or 16 or 32) bits are transferred per word.
	 *
	 * So we can get word transfer time (in nanoseconds) from:
	 *   nsec_per_tick = NANOHZ_PER_HZ / drv_data->rate;
	 *   ticks_per_word = BITS_PER_BYTE * drv_data->bytes;
	 * We do the divide last for better accuracy.
	 */
	nsec_per_word = NANOHZ_PER_HZ * BITS_PER_BYTE * drv_data->bytes;
	nsec_per_word = DIV_ROUND_UP_ULL(nsec_per_word, drv_data->rate);

	/*
	 * The timeout (which we'll set to three word transfer times) is
	 * expressed as a number of APB clock ticks.
	 *   bus_ticks = 3 * nsec * (drv_data->bus_rate / NANOHZ_PER_HZ)
	 */
	bus_ticks = 3 * nsec_per_word * drv_data->bus_rate;
	drv_data->rx_timeout = DIV_ROUND_UP_ULL(bus_ticks, NANOHZ_PER_HZ);

	return 0;
}

static void k1_spi_set_cs(struct spi_device *spi, bool enable)
{
	struct k1_spi_driver_data *drv_data;
	u32 val;

	drv_data = spi_controller_get_devdata(spi->controller);

	val = readl(drv_data->base + SSP_TOP_CTRL);
	if (enable)
		val |= TOP_HOLD_FRAME_LOW;
	else
		val &= ~TOP_HOLD_FRAME_LOW;
	writel(val, drv_data->base + SSP_TOP_CTRL);
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
	u32 val;

	drv_data = spi_controller_get_devdata(spi->controller);

	val = readl(drv_data->base + SSP_TOP_CTRL);
	val &= TOP_FRF_MASK | TOP_SPO | TOP_SPH | TOP_LBM;
	writel(val, drv_data->base + SSP_TOP_CTRL);
}

static void k1_spi_read_word(struct k1_spi_driver_data *drv_data)
{
	struct k1_spi_io *rx = &drv_data->rx;
	u32 bytes = drv_data->bytes;
	u32 val;

	val = readl(drv_data->base + SSP_DATAR);
	rx->resid -= bytes;

	if (!rx->buf)
		return;	/* Null reader: discard the data */

	if (bytes == 1)
		*(u8 *)rx->buf = val;
	else if (bytes == 2)
		*(u16 *)rx->buf = val;
	else	/* bytes == 4 */
		*(u32 *)rx->buf = val;

	rx->buf += bytes;
}

static bool k1_spi_read(struct k1_spi_driver_data *drv_data)
{
	struct k1_spi_io *rx = &drv_data->rx;
	unsigned int count;
	u32 val;

	if (!rx->resid)
		return true;	/* Nothing more to receive */

	/* We'll read as many slots in the FIFO as there are available */
	val = readl(drv_data->base + SSP_STATUS);
	/* The number of open slots is one more than what's in the field */
	count = FIELD_GET(SSP_STATUS_RFL, val) + 1;

	/* A full FIFO count means the FIFO is either full or empty */

	if (count == K1_SPI_FIFO_SIZE)
		if (!(val & SSP_STATUS_RNE))
			return false;	/* Nothing available to read */

	count = min(count, rx->resid);
	while (count--)
		k1_spi_read_word(drv_data);

	return !rx->resid;
}

static void k1_spi_write_word(struct k1_spi_driver_data *drv_data)
{
	struct k1_spi_io *tx = &drv_data->tx;
	u32 val = 0;
	u32 bytes;

	bytes = drv_data->bytes;
	if (tx->buf) {
		if (bytes == 1)
			val = *(u8 *)tx->buf;
		else if (bytes == 2)
			val = *(u16 *)tx->buf;
		else	/* bytes == 4 */
			val = *(u32 *)tx->buf;
		tx->buf += bytes;
	} /* Otherwise null writer; write 1, 2, or 4 zero bytes */

	tx->resid -= bytes;
	writel(val, drv_data->base + SSP_DATAR);
}

static bool k1_spi_write(struct k1_spi_driver_data *drv_data)
{
	struct k1_spi_io *tx = &drv_data->tx;
	unsigned int count;
	u32 val;

	if (!tx->resid)
		return true;	/* Nothing more to send */

	/* See how many slots in the TX FIFO are available */
	val = readl(drv_data->base + SSP_STATUS);
	count = FIELD_GET(SSP_STATUS_TFL, val);

	/* A zero count means the FIFO is either full or empty */
	if (!count) {
		if (val & SSP_STATUS_TNF)
			count = K1_SPI_FIFO_SIZE;
		else
			return false;	/* No room in the FIFO */
	}

	/*
	 * Limit how much we try to send at a time, to reduce the
	 * chance the other side can overrun our RX FIFO.
	 */
	count = min3(count, K1_SPI_THRESH, tx->resid);
	while (count--)
		k1_spi_write_word(drv_data);

	return !tx->resid;
}

static void k1_spi_transfer_start(struct k1_spi_driver_data *drv_data,
				  struct spi_transfer *transfer)
{
	u32 val;

	/* Record the current transfer information */
	drv_data->rx.buf = transfer->rx_buf;
	drv_data->rx.resid = transfer->len;
	drv_data->tx.buf = (void *)transfer->tx_buf;
	drv_data->tx.resid = transfer->len;
	drv_data->len = transfer->len;

	/* Set the RX timeout period (required for both DMA and PIO) */
	val = FIELD_PREP(SSP_TIMEOUT_MASK, drv_data->rx_timeout);
	writel(val, drv_data->base + SSP_TIMEOUT);

	/* Clear any existing interrupt conditions */
	val = readl(drv_data->base + SSP_STATUS);
	writel(val, drv_data->base + SSP_STATUS);

	/* Set the data size and enable the hardware */
	val = readl(drv_data->base + SSP_TOP_CTRL);
	val |= FIELD_PREP(TOP_DSS_MASK, transfer->bits_per_word - 1);
	val |= TOP_SSE;
	writel(val, drv_data->base + SSP_TOP_CTRL);

	/*
	 * For PIO transfers, interrupts will cause words to get
	 * transferred.  The interrupts will get disabled as the
	 * transfer completes.  We'll write what we can to get
	 * things started.
	 */
	(void)k1_spi_write(drv_data);

	val = SSP_INT_EN_RIM | SSP_INT_EN_TIM;
	val |= SSP_INT_EN_TINTE | SSP_INT_EN_RIE | SSP_INT_EN_TIE;
	writel(val, drv_data->base + SSP_INT_EN);
}

static int
k1_spi_transfer_one(struct spi_controller *host, struct spi_device *spi,
		    struct spi_transfer *transfer)
{
	struct k1_spi_driver_data *drv_data = spi_controller_get_devdata(host);
	int ret;

	/* Bits per word can change on a per-transfer basis */
	drv_data->bytes = spi_bpw_to_bytes(transfer->bits_per_word);

	/* Each transfer can also specify a different rate */
	ret = k1_spi_set_speed(drv_data, transfer->speed_hz);
	if (ret)
		dev_err(drv_data->dev,
			"failed to set transfer speed: %d\n", ret);
	else
		k1_spi_transfer_start(drv_data, transfer);

	return ret;
}

static void k1_spi_transfer_wait(struct k1_spi_driver_data *drv_data)
{
	struct completion *completion = &drv_data->completion;
	struct spi_message *message = drv_data->message;
	unsigned long timeout;
	int ret;

	/* Length in bits to be transferred */
	timeout = BITS_PER_BYTE * drv_data->bytes * drv_data->len;
	/* Time (usec) to transfer that many bits at the current bit rate */
	timeout = DIV_ROUND_UP(timeout * MICROHZ_PER_HZ, drv_data->rate);
	/* Convert that (+ 25%) to jiffies for the wait call */
	timeout = usecs_to_jiffies(5 * timeout / 4);

	ret = wait_for_completion_interruptible_timeout(completion, timeout);
	if (ret > 0)
		return;

	message->status = -EIO;
}

static void k1_spi_transfer_end(struct k1_spi_driver_data *drv_data,
				struct spi_transfer *transfer)
{
	struct spi_message *message = drv_data->message;
	u32 val;

	val = readl(drv_data->base + SSP_TOP_CTRL);
	val &= ~TOP_SSE;
	val &= ~TOP_DSS_MASK;
	writel(val, drv_data->base + SSP_TOP_CTRL);

	writel(0, drv_data->base + SSP_TIMEOUT);

	spi_transfer_delay_exec(transfer);

	if (!message->status)
		message->actual_length += drv_data->len;
}

static int k1_spi_transfer_one_message(struct spi_controller *host,
					   struct spi_message *message)
{
	struct k1_spi_driver_data *drv_data = spi_controller_get_devdata(host);
	struct completion *completion = &drv_data->completion;
	struct spi_transfer *transfer;

	/* Message status starts out successful; set to -EIO on error */
	message->status = 0;

	k1_spi_set_cs(message->spi, true);

	list_for_each_entry(transfer, &message->transfers, transfer_list) {
		reinit_completion(completion);

		/* Issue the next transfer */
		if (k1_spi_transfer_one(host, message->spi, transfer)) {
			message->status = -EIO;
			break;
		}

		k1_spi_transfer_wait(drv_data);

		k1_spi_transfer_end(drv_data, transfer);

		/* If an error has occurred, we're done */
		if (message->status)
			break;
	}

	drv_data->message = NULL;

	spi_finalize_current_message(drv_data->host);

	k1_spi_set_cs(message->spi, false);

	return 0;
}

static int k1_spi_prepare_message(struct spi_controller *host,
				  struct spi_message *message)
{
	struct k1_spi_driver_data *drv_data = spi_controller_get_devdata(host);

	drv_data->message = message;

	k1_spi_flush(drv_data);

	return 0;
}

static int k1_spi_unprepare_message(struct spi_controller *host,
				    struct spi_message *message)
{
	return 0;
}

static const struct of_device_id k1_spi_dt_ids[] = {
	{ .compatible = "spacemit,k1-spi", },
	{}
};
MODULE_DEVICE_TABLE(of, k1_spi_dt_ids);

static void k1_spi_host_init(struct k1_spi_driver_data *drv_data)
{
	struct device_node *np = dev_of_node(drv_data->dev);
	struct spi_controller *host = drv_data->host;
	struct device *dev = drv_data->dev;
	u32 max_speed_hz;
	int ret;

	host->dev.of_node = np;
	host->dev.parent = drv_data->dev;
	host->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LOOP;
	host->bits_per_word_mask = SPI_BPW_RANGE_MASK(4, 32);
	host->num_chipselect = 1;

	host->setup = k1_spi_setup;
	host->cleanup = k1_spi_cleanup;
	host->transfer_one_message = k1_spi_transfer_one_message;
	host->prepare_message = k1_spi_prepare_message;
	host->unprepare_message = k1_spi_unprepare_message;

	ret = of_property_read_u32(np, "spi-max-frequency", &max_speed_hz);
	if (!ret) {
		host->max_speed_hz = clamp(max_speed_hz, K1_SPI_MIN_SPEED_HZ,
					   K1_SPI_MAX_SPEED_HZ);
		if (host->max_speed_hz != max_speed_hz)
			dev_warn(dev, "spi-max-frequency %u out of range, using %u\n",
				max_speed_hz, host->max_speed_hz);
	} else {
		if (ret != -EINVAL)
			dev_warn(dev, "bad spi-max-frequency, using %u\n",
				 K1_SPI_DEFAULT_MAX_SPEED_HZ);
		host->max_speed_hz = K1_SPI_DEFAULT_MAX_SPEED_HZ;
	}
}

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
	val = readl(drv_data->base + SSP_STATUS);
	writel(val, drv_data->base + SSP_STATUS);
}

static irqreturn_t k1_spi_ssp_isr(int irq, void *dev_id)
{
	struct k1_spi_driver_data *drv_data = dev_id;
	bool rx_done;
	bool tx_done;
	u32 val;

	/* Get status and clear pending interrupts */
	val = readl(drv_data->base + SSP_STATUS);
	writel(val, drv_data->base + SSP_STATUS);

	if (!drv_data->message)
		return IRQ_NONE;

	/* Check for an RX overrun or TX underrun first */
	if (val & (SSP_STATUS_TUR | SSP_STATUS_ROR)) {
		/* Disable all interrupts on error */
		writel(0, drv_data->base + SSP_INT_EN);

		drv_data->message->status = -EIO;
		complete(&drv_data->completion);

		return IRQ_HANDLED;
	}

	/* Drain the RX FIFO first, then transmit what we can */
	rx_done = k1_spi_read(drv_data);
	tx_done = k1_spi_write(drv_data);

	/* Disable interrupts if we're done transferring either direction */
	if (rx_done || tx_done) {
		/* If both are done, disable all interrupts */
		if (rx_done && tx_done) {
			val = 0;
		} else {
			val = readl(drv_data->base + SSP_INT_EN);
			if (rx_done)
				val &= ~(SSP_INT_EN_TINTE | SSP_INT_EN_RIE);
			if (tx_done)
				val &= ~SSP_INT_EN_TIE;
		}
		writel(val, drv_data->base + SSP_INT_EN);
	}

	if (rx_done && tx_done)
		complete(&drv_data->completion);

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
	drv_data->dev = dev;
	init_completion(&drv_data->completion);

	drv_data->base = devm_platform_get_and_ioremap_resource(pdev, 0,
								&iores);
	if (IS_ERR(drv_data->base))
		return dev_err_probe(dev, PTR_ERR(drv_data->base),
				     "error mapping memory\n");

	k1_spi_host_init(drv_data);

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

	k1_spi_register_reset(drv_data, true);

	drv_data->irq = platform_get_irq(pdev, 0);
	if (drv_data->irq < 0)
		return dev_err_probe(dev, drv_data->irq, "error getting IRQ\n");

	ret = devm_request_irq(dev, drv_data->irq, k1_spi_ssp_isr,
			       IRQF_SHARED, dev_name(dev), drv_data);
	if (ret < 0)
		return dev_err_probe(dev, ret, "error requesting IRQ\n");

	ret = devm_spi_register_controller(dev, host);
	if (ret)
		dev_err(dev, "error registering controller\n");

	return ret;
}

static void k1_spi_remove(struct platform_device *pdev)
{
	struct k1_spi_driver_data *drv_data = platform_get_drvdata(pdev);

	k1_spi_register_reset(drv_data, false);
}

static struct platform_driver k1_spi_driver = {
	.driver = {
		.name		= "k1-spi",
		.of_match_table	= k1_spi_dt_ids,
	},
	.probe			= k1_spi_probe,
	.remove			= k1_spi_remove,
};

module_platform_driver(k1_spi_driver);

MODULE_DESCRIPTION("SpacemiT K1 SPI controller driver");
MODULE_LICENSE("GPL");
