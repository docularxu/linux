/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_ISP_CORE_DEF_H
#define HISI_ISP_CORE_DEF_H

#include <linux/sched.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <linux/clk.h>
#include <linux/version.h>
#include <linux/list.h>
#include <linux/file.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/completion.h>
#include <linux/io.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/compat.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/of_address.h>
#include <linux/regulator/consumer.h>

#include "hisi_isp_reg_offset.h"
#include "i2c.h"

#define COMP_HISI_ISP_NAME      "hisilicon,hisi-isp"
#define DEV_NAME_ISP            "hisi_isp"

#define USED        0x01
#define UNUSED      0x00

//#define HISP_DEBUG

#define array_count(x) (sizeof(x) / sizeof(x[0]))

#if defined(HISP_DEBUG)
#define ISP_ERR(msg, ...) \
	printk(KERN_ERR "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_WARNING(msg, ...) \
	printk(KERN_WARNING "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_INFO(msg, ...) \
	printk(KERN_INFO "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_DEBUG(msg, ...) \
	printk(KERN_DEBUG "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#else
#define ISP_ERR(msg, ...) \
	printk(KERN_ERR "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_WARNING(msg, ...) \
	printk(KERN_WARNING "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_INFO(msg, ...) \
	printk(KERN_INFO "[ISP] %s: "msg, __func__, ## __VA_ARGS__)
#define ISP_DEBUG(msg, ...)
#endif

#define loge_if_ret(x) \
	{\
		if (x) {\
			ISP_ERR("'%s' failed\n", #x); \
			return 1; \
		} \
	}

#define REG_SET(addr, val) writel(val, addr)
#define REG_GET(addr) readl(addr)

#define MASK0(name)  (((unsigned int)1<<(name##_LEN))-1)
#define MASK1(name)  ((((unsigned int)1<<(name##_LEN))-1) << (name##_OFFSET))

/* operation on the field of a variable */
#define REG_GET_FIELD(reg, name) \
	(((reg) >> (name##_OFFSET)) & MASK0(name))

#define REG_SET_FIELD(reg, name, val) \
	(reg = ((reg) & ~MASK1(name)) | (((val) & MASK0(name)) << (name##_OFFSET)))

struct stream_info {
	int w;
	int h;
	int format;
	int pool_size;
};

struct frame_info {
	int frame_num;
	int offset;
};

#define ISP_IOC_MAGIC       'I'

#define ISP_IOC_STREAM_ON    _IOWR(ISP_IOC_MAGIC, 0, struct stream_info)

#define ISP_IOC_FRAME_REQUEST    _IOWR(ISP_IOC_MAGIC, 1, struct frame_info)

#define ISP_IOC_STREAM_OFF    _IO(ISP_IOC_MAGIC, 2)

struct isp_device {
	struct miscdevice dev;
	char __iomem *base;
	char __iomem *base_gpio;
	uint32_t isp_irq_vic1;
	uint32_t isp_irq_frproc0;
	struct clk *xclk;
	int xclk_freq;
	struct regulator *io_regulator;

	void *virt_addr;
	dma_addr_t hw_addr;
	unsigned int pool_size;
	size_t frame_size;
	size_t frame_count;
	volatile size_t isp_frame_num;
	volatile size_t hal_frame_num;
	struct completion frame_comp;
	bool streaming;

	unsigned short addr;
	unsigned int flag;
	unsigned int speed_cfg;

	int csi_index;
	int dt;
	int pf;
	int w;
	int h;

	int csi_lane;

	int pwdn_gpio;
	int rst_gpio;
	struct i2c_client_t *client;
};
#endif /* HISI_ISP_CORE_DEF_H */
