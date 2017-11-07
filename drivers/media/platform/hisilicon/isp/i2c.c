/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "i2c.h"
#include "hisi_isp_common.h"

/* Registers offset */
#define I2C_CON                 0x0
#define I2C_TAR                 0x4
#define I2C_DATA_CMD            0x10
#define I2C_SS_SCL_HCNT         0x14
#define I2C_SS_SCL_LCNT         0x18
#define I2C_FS_SCL_HCNT         0x1c
#define I2C_FS_SCL_LCNT         0x20
#define I2C_HS_SCL_HCNT         0x24
#define I2C_HS_SCL_LCNT         0x28
#define I2C_SDA_HOLD            0x7C
#define I2C_FS_SPKLEN           0xA0
#define I2C_HS_SPKLEN           0xA4

#define I2C_INTR_STAT           0x2c
#define I2C_INTR_MASK           0x30
#define I2C_RAW_INTR_STAT       0x34
#define I2C_RX_TL               0x38
#define I2C_TX_TL               0x3c
#define I2C_CLR_INTR            0x40
#define I2C_CLR_RX_UNDER        0x44
#define I2C_CLR_RX_OVER         0x48
#define I2C_CLR_TX_OVER         0x4c
#define I2C_CLR_RD_REQ          0x50
#define I2C_CLR_TX_ABRT         0x54
#define I2C_CLR_RX_DONE         0x58
#define I2C_CLR_ACTIVITY        0x5c
#define I2C_CLR_STOP_DET        0x60
#define I2C_CLR_START_DET       0x64
#define I2C_CLR_GEN_CALL        0x68
#define I2C_ENABLE              0x6c
#define I2C_STATUS              0x70
#define I2C_TXFLR               0x74
#define I2C_RXFLR               0x78
#define I2C_TX_ABRT_SOURCE      0x80
#define I2C_ENABLE_STATUS       0x9c
#define I2C_COMP_TYPE           0xfc
#define I2C_COMP_TYPE_VALUE     0x44570140

#define I2C_INTR_RX_UNDER       0x001
#define I2C_INTR_RX_OVER        0x002
#define I2C_INTR_RX_FULL        0x004
#define I2C_INTR_TX_OVER        0x008
#define I2C_INTR_TX_EMPTY       0x010
#define I2C_INTR_RD_REQ         0x020
#define I2C_INTR_TX_ABRT        0x040
#define I2C_INTR_RX_DONE        0x080
#define I2C_INTR_ACTIVITY       0x100
#define I2C_INTR_STOP_DET       0x200
#define I2C_INTR_START_DET      0x400
#define I2C_INTR_GEN_CALL       0x800

#define I2C_INTR_DEFAULT_MASK   (I2C_INTR_RX_FULL | \
		I2C_INTR_TX_EMPTY | \
		I2C_INTR_TX_ABRT  | \
		I2C_INTR_STOP_DET)

#define I2C_STATUS_ACTIVITY    0x1
#define I2C_ERR_TX_ABRT        0x1
#define I2C_IC_EN              0x1

#define I2C_M_RD    0x01

#define I2C_CON_MASTER             0x1
#define I2C_CON_SPEED_STD          0x2
#define I2C_CON_SPEED_FAST         0x4
#define I2C_CON_SPEED_HIGH         0x6
#define I2C_CON_10BITADDR_MASTER   0x10
#define I2C_CON_RESTART_EN         0x20
#define I2C_CON_SLAVE_DISABLE      0x40

#define I2C_COMP_PARAM_1    0xf4

/* status codes */
#define STATUS_IDLE                0x0
#define STATUS_WRITE_IN_PROGRESS   0x1
#define STATUS_READ_IN_PROGRESS    0x2

#define ISPSS_CTRL_MODULE_RESET_TOP_REG     0x60

static unsigned int i2c_reg_read(struct i2c_dev_t *dev, int offset)
{
	unsigned int value;

	value = readl(dev->base + offset);
	return value;
}

static void i2c_reg_write(struct i2c_dev_t *dev, int offset, unsigned int b)
{
	writel(b, dev->base + offset);
}

static int i2c_wait_bus_not_busy(struct i2c_dev_t *dev)
{
	unsigned int timeout = 20;/* ms*/

	while (i2c_reg_read(dev, I2C_STATUS) & I2C_STATUS_ACTIVITY) {
		if (timeout <= 0) {
			ISP_ERR("timeout waiting for bus ready\n");
			return -1;
		}
		timeout--;
		mdelay(1);
	}
	return 0;
}

static unsigned int i2c_read_clear_intrbits(struct i2c_dev_t *dev)
{
	unsigned int stat;

	stat = i2c_reg_read(dev, I2C_INTR_STAT);

	if (stat & I2C_INTR_RX_UNDER) {
		i2c_reg_read(dev, I2C_CLR_RX_UNDER);
	}
	if (stat & I2C_INTR_RX_OVER) {
		i2c_reg_read(dev, I2C_CLR_RX_OVER);
	}
	if (stat & I2C_INTR_TX_OVER) {
		i2c_reg_read(dev, I2C_CLR_TX_OVER);
	}
	if (stat & I2C_INTR_RD_REQ) {
		i2c_reg_read(dev, I2C_CLR_RD_REQ);
	}
	if (stat & I2C_INTR_TX_ABRT) {
		/*
		 *		* The IC_TX_ABRT_SOURCE
		 *		register is cleared whenever
		 *				* the IC_CLR_TX_ABRT
		 *				is read, preserve it
		 *				beforehand.
		 *						*/
		ISP_ERR("ABRT_SOURCE 0x%x\n", i2c_reg_read(dev, I2C_TX_ABRT_SOURCE));
		i2c_reg_read(dev, I2C_TX_ABRT_SOURCE);
		i2c_reg_read(dev, I2C_CLR_TX_ABRT);
	}
	if (stat & I2C_INTR_RX_DONE) {
		i2c_reg_read(dev, I2C_CLR_RX_DONE);
	}
	if (stat & I2C_INTR_ACTIVITY) {
		i2c_reg_read(dev, I2C_CLR_ACTIVITY);
	}
	if (stat & I2C_INTR_STOP_DET) {
		i2c_reg_read(dev, I2C_CLR_STOP_DET);
	}
	if (stat & I2C_INTR_START_DET) {
		i2c_reg_read(dev, I2C_CLR_START_DET);
	}
	if (stat & I2C_INTR_GEN_CALL) {
		i2c_reg_read(dev, I2C_CLR_GEN_CALL);
	}

	return stat;
}

static void i2c_read(struct i2c_dev_t *dev)
{
	struct i2c_msg_t *msgs = dev->msgs;
	int rx_valid;

	for (; dev->msg_read_idx < dev->msgs_num; dev->msg_read_idx++) {
		unsigned int len;
		unsigned char *buf;

		if (!(msgs[dev->msg_read_idx].flags & I2C_M_RD)) {
			continue;
		}

		if (!(dev->status & STATUS_READ_IN_PROGRESS)) {
			len = msgs[dev->msg_read_idx].len;
			buf = msgs[dev->msg_read_idx].buf;
		} else {
			len = dev->rx_buf_len;
			buf = dev->rx_buf;
		}

		rx_valid = i2c_reg_read(dev, I2C_RXFLR);

		for (; len > 0 && rx_valid > 0; len--, rx_valid--) {
			*buf++ = i2c_reg_read(dev, I2C_DATA_CMD);
			dev->rx_outstanding--;
		}

		if (len > 0) {
			dev->status |= STATUS_READ_IN_PROGRESS;
			dev->rx_buf_len = len;
			dev->rx_buf = buf;
			return;
		} else {
			dev->status &= ~STATUS_READ_IN_PROGRESS;
		}
	}
}

static void i2c_xfer_msg_loop(i2c_dev_t *dev, i2c_msg_t *msgs, int tx_limit, int rx_limit)
{
	while (dev->tx_buf_len > 0 && tx_limit > 0 && rx_limit > 0) {
		unsigned int cmd = 0;

		if (dev->msg_write_idx == dev->msgs_num - 1 && dev->tx_buf_len == 1) {
			cmd |= (0x1 << 9);
		}

		if (msgs[dev->msg_write_idx].flags & I2C_M_RD) {
			/* avoid rx buffer overrun */
			if (rx_limit - dev->rx_outstanding <= 0) {
				break;
			}

			i2c_reg_write(dev, I2C_DATA_CMD, cmd | 0x100);
			rx_limit--;
			dev->rx_outstanding++;
		} else {
			i2c_reg_write(dev, I2C_DATA_CMD, cmd | *(dev->tx_buf)++);
		}

		tx_limit--;
		dev->tx_buf_len--;
	}
}

static int i2c_xfer_msg(struct i2c_dev_t *dev)
{
	int tx_limit;
	int rx_limit;
	unsigned int intr_mask;

	struct i2c_msg_t *msgs = dev->msgs;
	unsigned int addr = msgs[dev->msg_write_idx].addr;

	intr_mask = I2C_INTR_DEFAULT_MASK;

	for (; dev->msg_write_idx < dev->msgs_num; dev->msg_write_idx++) {
		/*
		 *		* if target address has changed, we need to
		 *				* reprogram the target address in the i2c
		 *						* adapter when we are done with this
		 *						transfer
		 *								*/
		if (msgs[dev->msg_write_idx].addr != addr) {
			ISP_ERR(" invalid target address\n");
			dev->msg_err = -1;
			break;
		}

		if (msgs[dev->msg_write_idx].len == 0) {
			ISP_ERR(" invalid message length\n");
			dev->msg_err = -1;
			break;
		}

		if (!(dev->status & STATUS_WRITE_IN_PROGRESS)) {
			/* new i2c_msg_t */
			dev->tx_buf = msgs[dev->msg_write_idx].buf;
			dev->tx_buf_len = msgs[dev->msg_write_idx].len;
		}

		tx_limit = dev->tx_fifo_depth - i2c_reg_read(dev, I2C_TXFLR);
		rx_limit = dev->rx_fifo_depth - i2c_reg_read(dev, I2C_RXFLR);

		i2c_xfer_msg_loop(dev, msgs, tx_limit, rx_limit);

		if (dev->tx_buf_len > 0) {
			/* more bytes to be written */
			dev->status |= STATUS_WRITE_IN_PROGRESS;
			break;
		} else {
			dev->status &= ~STATUS_WRITE_IN_PROGRESS;
		}
	}

	/*
	 *	* If i2c_msg_t index search is completed, we don't need TX_EMPTY
	 *		* interrupt any more.
	 *			*/
	if (dev->msg_write_idx == dev->msgs_num) {
		intr_mask &= ~I2C_INTR_TX_EMPTY;
	}
	if (dev->msg_err) {
		intr_mask = 0;
	}

	i2c_reg_write(dev, I2C_INTR_MASK, intr_mask);

	return 0;
}

int i2c_polling(struct i2c_dev_t *dev)
{
	int stat;

	stat = i2c_read_clear_intrbits(dev);

	if (stat & I2C_INTR_TX_ABRT) {
		dev->cmd_err |= I2C_ERR_TX_ABRT;
		dev->status = STATUS_IDLE;
		i2c_reg_write(dev, I2C_INTR_MASK, 0);
		goto i2c_isr_err1;
	}

	if (stat & I2C_INTR_RX_FULL) {
		i2c_read(dev);
	}

	if (stat & I2C_INTR_TX_EMPTY) {
		i2c_xfer_msg(dev);
	}

i2c_isr_err1:
	if ((stat & (I2C_INTR_TX_ABRT | I2C_INTR_STOP_DET)) || dev->msg_err) {
		return 0;
	}

	return -1;
}

static void i2c_enable(struct i2c_dev_t *dev)
{
	int timeout = 100;

	do {
		i2c_reg_write(dev, I2C_ENABLE, 1);
		if ((i2c_reg_read(dev, I2C_ENABLE_STATUS) & 1) == 1) {
			return;
		}

		mdelay(1);
	} while (timeout--);

	ISP_WARNING("i2c dev enable timeout\n");
}

static void i2c_disable(struct i2c_dev_t *dev)
{
	int timeout = 100;

	do {
		i2c_reg_write(dev, I2C_ENABLE, 0);
		if ((i2c_reg_read(dev, I2C_ENABLE_STATUS) & 1) == 0) {
			return;
		}

		mdelay(1);
	} while (timeout--);

	ISP_WARNING("i2c dev disable timeout\n");
}

static void i2c_clear_int(struct i2c_dev_t *dev)
{
	i2c_reg_read(dev, I2C_CLR_INTR);
}

static unsigned int i2c_scl_hcnt(unsigned int ic_clk, unsigned int tSYMBOL,
		unsigned int tf, int cond, int offset)
{
	if (cond) {
		return (ic_clk * tSYMBOL + 5000) / 10000 - 8 + offset;
	} else {
		return (ic_clk * (tSYMBOL + tf) + 5000) / 10000 - 3 + offset;
	}
}

static unsigned int i2c_scl_lcnt(unsigned int ic_clk, unsigned int tLOW,
		unsigned int tf, int offset)
{
	return ((ic_clk * (tLOW + tf) + 5000) / 10000) - 1 + offset;
}

static void i2c_xfer_init(struct i2c_dev_t *dev, i2c_speed_e speed_cfg)
{
	struct i2c_msg_t *msgs = dev->msgs;
	unsigned int retry_num = 100;
	unsigned int ic_con = 0;
	unsigned int hcnt;
	unsigned int lcnt;

	i2c_reg_write(dev, I2C_ENABLE, 0);

	while ((--retry_num != 0) && (i2c_reg_read(dev, I2C_ENABLE_STATUS) & I2C_IC_EN)) {
		mdelay(1);
	}

	if (retry_num <= 0) {
		ISP_ERR(" disable i2c controller error.\n");
	}

	/* set the slave (target) address */
	i2c_reg_write(dev, I2C_TAR, msgs[dev->msg_write_idx].addr);

	/* configure the i2c master speed */
	if (speed_cfg > dev->speed_cfg) {
		ISP_ERR(" i2c driver error: %d is beyond adapter speed_cfg\n", speed_cfg);
	}

	switch (speed_cfg) {
	case I2C_SPEED_STD:
		ic_con = I2C_CON_SPEED_STD;
		break;
	case I2C_SPEED_FAST:
		ic_con = I2C_CON_SPEED_FAST;
		hcnt = i2c_scl_hcnt(dev->ic_clk,
				6,  /* tHD;STA = tHIGH = 0.6 us */
				3,  /* tf = 0.3 us              */
				0,  /* 0: default, 1: Ideal     */
				0); /* No offset                */
		lcnt = i2c_scl_lcnt(dev->ic_clk,
				13, /* tLOW = 1.3 us            */
				3,  /* tf = 0.3 us              */
				0); /* No offset                */
		i2c_reg_write(dev, I2C_FS_SCL_HCNT, hcnt);
		i2c_reg_write(dev, I2C_FS_SCL_LCNT, lcnt);
		break;
	case I2C_SPEED_FAST_PLUS:
		ic_con = I2C_CON_SPEED_FAST;
		hcnt = i2c_scl_hcnt(dev->ic_clk,
				2,  /* tHD;STA = tHIGH = 0.26 us */
				1,  /* tf = 0.12 us              */
				0,  /* 0: default, 1: Ideal      */
				0); /* No offset                 */
		lcnt = i2c_scl_lcnt(dev->ic_clk,
				5,  /* tLOW = 0.5 us             */
				1,  /* tf = 0.12 us              */
				0); /* No offset                 */
		i2c_reg_write(dev, I2C_FS_SCL_HCNT, hcnt);
		i2c_reg_write(dev, I2C_FS_SCL_LCNT, lcnt);
		break;
	case I2C_SPEED_HIGH:
		ic_con = I2C_CON_SPEED_HIGH;
		break;
	}

	i2c_reg_write(dev, I2C_CON, dev->master_cfg | ic_con);
	/* if the slave address is ten bit address, enable 10BITADDR */
	ic_con = i2c_reg_read(dev, I2C_CON);
	if (msgs[dev->msg_write_idx].flags & I2C_ADDR_10BIT) {
		ic_con |= I2C_CON_10BITADDR_MASTER;
	} else {
		ic_con &= ~I2C_CON_10BITADDR_MASTER;
	}
	i2c_reg_write(dev, I2C_CON, ic_con);

	i2c_enable(dev);
	i2c_clear_int(dev);
	/* Enable interrupts */
	i2c_reg_write(dev, I2C_INTR_MASK, I2C_INTR_DEFAULT_MASK);
}

int i2c_xfer(struct i2c_dev_t *dev, struct i2c_msg_t msgs[], int num, int burst_flag, i2c_speed_e speed_cfg)
{
	int totallen;
	int i;
	int ret;
	unsigned int timeout = 100;

	dev->msgs = msgs;
	dev->msgs_num = num;
	dev->cmd_err = 0;
	dev->msg_write_idx = 0;
	dev->msg_read_idx = 0;
	dev->msg_err = 0;
	dev->status = STATUS_IDLE;
	dev->abort_source = 0;
	dev->rx_outstanding = 0;

	ret = i2c_wait_bus_not_busy(dev);
	if (ret < 0)
		return -EBUSY;
	/* start the transfers */
	i2c_xfer_init(dev, speed_cfg);

	/* calculate total length */
	totallen = 0;
	for (i = 0; i < num; i++) {
		totallen += msgs[i].len;
	}

	/* wait for tx to complete */
	while (true) {
		if (timeout <= 0) {
			ISP_ERR("i2c timeout\n");
			return -EIO;
		}

		if (i2c_polling(dev) == 0) {
			break;
		}

		mdelay(1);
		timeout--;
	}

	if (dev->msg_err) {
		ISP_ERR("I2C msg_err\n");
		return -EIO;
	}

	if (dev->cmd_err == I2C_ERR_TX_ABRT) {
		ISP_ERR("I2C abort\n");
		return -EIO;
	}

	i2c_disable(dev);

	return 0;
}

int i2c_read_reg(i2c_client_t *client, unsigned short reg, unsigned char *val)
{
	struct i2c_msg_t msg[2];
	int ret;
	unsigned char addr[2];
	unsigned char data;

	if ((client == NULL) || (client->i2c_dev == NULL)) {
		ISP_ERR("device is null\n");
		return -1;
	}

	addr[0] = (reg>>8) & 0xff;
	addr[1] =  reg & 0xff;

	msg[0].addr = client->addr;
	msg[0].len = 2;
	msg[0].buf = addr;
	msg[0].flags = client->flag & I2C_ADDR_10BIT;

	msg[1].addr = client->addr;
	msg[1].len = 1;
	msg[1].buf = &data;
	msg[1].flags = client->flag & I2C_ADDR_10BIT;
	msg[1].flags |= I2C_M_RD;

	ISP_ERR("read[0x%x]\n", reg);

	ret = i2c_xfer(client->i2c_dev, msg, 2, 0, client->speed_cfg);
	if (ret != 0) {
		ISP_ERR("read reg fail");
		return ret;
	}

	ISP_ERR("read[0x%x] = 0x%x\n", reg, data);

	*val = data;

	return ret;
}

int i2c_write_reg(i2c_client_t *client, unsigned short reg, unsigned char val)
{
	struct i2c_msg_t msg[1];
	int ret;
	unsigned char data[3];

	if ((client == NULL) || (client->i2c_dev == NULL)) {
		ISP_ERR("device is null\n");
		return -1;
	}

	data[0] = (reg>>8) & 0xff;
	data[1] = reg & 0xff;
	data[2] = val;

	msg[0].addr = client->addr;
	msg[0].len = 3;
	msg[0].buf = data;
	msg[0].flags = client->flag & I2C_ADDR_10BIT;

	ret = i2c_xfer(client->i2c_dev, msg, 1, 0, client->speed_cfg);
	if (ret != 0) {
		ISP_ERR("write reg fail\n");
		return ret;
	}

	return 0;
}

static void i2c_ispss_reset(struct i2c_dev_t *dev)
{
	int offset;

	switch (dev->base - dev->isp_base) {
	case ISP_BASE_ADDR_I2C_1:
		offset = 11;
		break;
	case ISP_BASE_ADDR_I2C_2:
		offset = 10;
		break;
	case ISP_BASE_ADDR_I2C_3:
		offset = 12;
		break;
	default:
		ISP_ERR("invalid i2c device\n");
		return;
	}

	REG_SET(dev->isp_base + ISP_BASE_ADDR_ISPSS_CTRL + ISPSS_CTRL_MODULE_RESET_TOP_REG, 1 << offset);
	mdelay(1);
	REG_SET(dev->isp_base + ISP_BASE_ADDR_ISPSS_CTRL + ISPSS_CTRL_MODULE_RESET_TOP_REG, 0x00000000);
	mdelay(1);

	return;
}

static int i2c_init(struct i2c_dev_t *dev)
{
	unsigned int hcnt;
	unsigned int lcnt;
	unsigned int sda_delay_count;

	/* I2C internal clock */
	dev->ic_clk = 480000;

	/* workaround: reset for a poweron sequence */
	i2c_ispss_reset(dev);

	i2c_disable(dev);

	/* Standard-mode */
	hcnt = i2c_scl_hcnt(dev->ic_clk,
			40, /* tHD;STA = tHIGH = 4.0 us */
			3,  /* tf = 0.3 us              */
			0,  /* 0: default, 1: Ideal     */
			0); /* No offset                */

	lcnt = i2c_scl_lcnt(dev->ic_clk,
			47, /* tLOW = 4.7 us            */
			3,  /* tf = 0.3 us              */
			0); /* No offset                */

	i2c_reg_write(dev, I2C_SS_SCL_HCNT, hcnt);
	i2c_reg_write(dev, I2C_SS_SCL_LCNT, lcnt);

	/* Fast-mode */
	hcnt = i2c_scl_hcnt(dev->ic_clk,
			6,  /* tHD;STA = tHIGH = 0.6 us */
			3,  /* tf = 0.3 us              */
			0,  /* 0: default, 1: Ideal     */
			0); /* No offset                */

	lcnt = i2c_scl_lcnt(dev->ic_clk,
			13, /* tLOW = 1.3 us            */
			3,  /* tf = 0.3 us              */
			0); /* No offset                */

	i2c_reg_write(dev, I2C_FS_SCL_HCNT, hcnt);
	i2c_reg_write(dev, I2C_FS_SCL_LCNT, lcnt);

	/* High-mode */
	hcnt = i2c_scl_hcnt(dev->ic_clk,
			1,  /* tHD;STA = tHIGH = 0.1 us */
			3,  /* tf = 0.3 us              */
			0,  /* 0: default, 1: Ideal     */
			0); /* No offset                */

	lcnt = i2c_scl_lcnt(dev->ic_clk,
			2,  /* tLOW = 0.2 us            */
			3,  /* tf = 0.3 us              */
			0); /* No offset                */

	i2c_reg_write(dev, I2C_HS_SCL_HCNT, hcnt);
	i2c_reg_write(dev, I2C_HS_SCL_LCNT, lcnt);

	/* Spike Suppression (FS 50ns or HS 10ns) */
	i2c_reg_write(dev, I2C_FS_SPKLEN, (dev->ic_clk * 50) / 1000000);
	i2c_reg_write(dev, I2C_HS_SPKLEN, (dev->ic_clk * 10) / 1000000);

	/* SDA HOLD TIME (300ns or 70ns)*/
	sda_delay_count = (dev->ic_clk * 300) / 1000000;
	i2c_reg_write(dev, I2C_SDA_HOLD, sda_delay_count);

	/* Configure Tx/Rx FIFO threshold levels */
	i2c_reg_write(dev, I2C_TX_TL, dev->tx_fifo_depth / 2);
	i2c_reg_write(dev, I2C_RX_TL, 0);

	/* configure the i2c master */
	i2c_reg_write(dev, I2C_CON, dev->master_cfg);

	return 0;
}

static void i2c_disable_int(struct i2c_dev_t *dev)
{
	i2c_reg_write(dev, I2C_INTR_MASK, 0);
}

int i2c_open(i2c_client_t *client)
{
	i2c_dev_t *i2c_dev = client->i2c_dev;
	int param;
	int ret;

	i2c_dev->master_cfg = I2C_CON_MASTER |
		I2C_CON_SLAVE_DISABLE |
		I2C_CON_RESTART_EN;

	param = i2c_reg_read(i2c_dev, I2C_COMP_PARAM_1);
	i2c_dev->tx_fifo_depth = ((param >> 16) & 0xff) + 1;
	i2c_dev->rx_fifo_depth = ((param >> 8)  & 0xff) + 1;

	ret = i2c_init(i2c_dev);
	if (0 != ret) {
		ISP_ERR(" i2c init failed :%x\n", ret);
		return -1;
	}

	i2c_disable_int(i2c_dev);

	i2c_dev->speed_cfg = I2C_SPEED_HIGH;

	return 0;
}

int i2c_close(i2c_client_t *client)
{
	return 0;
}

static struct i2c_dev_ops i2c_dops = {
	.open = i2c_open,
	.close = i2c_close,
	.read = i2c_read_reg,
	.write = i2c_write_reg,
};

int i2c_get_offset(int csi_index)
{
	switch (csi_index) {
	case 0:
		return ISP_BASE_ADDR_I2C_1;
	case 1:
		return ISP_BASE_ADDR_I2C_2;
	case 2:
		return ISP_BASE_ADDR_I2C_3;
	default:
		ISP_ERR("csi_index=%d error\n", csi_index);
		return -1;
	}
}

i2c_client_t *create_i2c_client(struct isp_device *isp_dev)
{
	i2c_client_t *client = kzalloc(sizeof(i2c_client_t), GFP_KERNEL);
	if (client) {
		client->i2c_dev = kzalloc(sizeof(i2c_dev_t), GFP_KERNEL);
		if (client->i2c_dev) {
			client->i2c_dev->base = isp_dev->base + i2c_get_offset(isp_dev->csi_index);
			client->i2c_dev->isp_base = isp_dev->base;
			client->csi_index = isp_dev->csi_index;
			if (isp_dev->addr > 0x77)
				client->addr = isp_dev->addr >> 1;
			else
				client->addr = isp_dev->addr;
			client->flag = isp_dev->flag;
			client->speed_cfg = isp_dev->speed_cfg;
			client->ops = &i2c_dops;
			client->isp_dev = isp_dev;
		} else {
			kfree(client);
			client = NULL;
		}
	}

	return client;
}

void destory_i2c_client(i2c_client_t *dev)
{
	kfree(dev->i2c_dev);
	kfree(dev);
}
