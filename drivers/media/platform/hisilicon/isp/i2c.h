/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_ISP_I2C_H
#define HISI_ISP_I2C_H

#include "hisi_isp_core.h"

typedef enum i2c_addr_e {
	I2C_ADDR_7BIT  = 0x0,
	I2C_ADDR_10BIT = 0x10,
} i2c_addr_e;

typedef enum i2c_speed_e {
	I2C_SPEED_STD       = 0x1,
	I2C_SPEED_FAST      = 0x2,
	I2C_SPEED_FAST_PLUS = 0x3,
	I2C_SPEED_HIGH      = 0x4,
} i2c_speed_e;

typedef enum _state_e {
	OFF,
	ON,
	STATE_END,
} state_e;

typedef struct i2c_msg_t {
	unsigned short  addr;
	unsigned short  flags;
	unsigned short  len;
	unsigned char   *buf;
} i2c_msg_t;

typedef struct i2c_dev_t {
	char __iomem    *isp_base;
	char __iomem    *base;
	i2c_speed_e     speed_cfg;
	unsigned int    irq;
	unsigned int    ic_clk;
	unsigned int    master_cfg;
	unsigned int    cmd_err;
	i2c_msg_t       *msgs;
	int             msgs_num;
	int             msg_write_idx;
	int             msg_read_idx;
	unsigned int    tx_buf_len;
	unsigned int    rx_buf_len;
	unsigned char   *tx_buf;
	unsigned char   *rx_buf;
	int             msg_err;
	unsigned int    status;
	unsigned int    abort_source;
	unsigned int    tx_fifo_depth;
	unsigned int    rx_fifo_depth;
	int             rx_outstanding;
} i2c_dev_t;

typedef struct i2c_client_t {
	struct isp_device   *isp_dev;
	i2c_dev_t           *i2c_dev;
	struct i2c_dev_ops  *ops;
	int                 csi_index;
	unsigned short      addr;
	unsigned int        flag;
	i2c_speed_e         speed_cfg;
	int                 mode;
} i2c_client_t;

struct i2c_dev_ops {
	int (*open)(i2c_client_t *client);
	int (*close)(i2c_client_t *client);
	int (*read)(i2c_client_t *client, unsigned short reg, unsigned char *val);
	int (*write)(i2c_client_t *client, unsigned short reg, unsigned char val);
};

i2c_client_t *create_i2c_client(struct isp_device *isp_dev);
void destory_i2c_client(i2c_client_t *dev);

#endif /* HISI_ISP_I2C_H */
