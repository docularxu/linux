/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "hisi_isp_isr.h"
#include "hisi_isp_common.h"
#include "hisi_isp_cvdr.h"
#include "hisi_isp_sr.h"

static unsigned int isp_clear_irq(struct isp_device *d, IRQ_MERGER_TYPE irq)
{
	unsigned int val;

	val = REG_GET(d->base + ISP_BASE_ADDR_IRQ_MERGER2+irq+IRQ_MERGER_FUNC_RIS);
	REG_SET(d->base + ISP_BASE_ADDR_IRQ_MERGER2+irq+IRQ_MERGER_FUNC_ICR, val);

	return val;
}

void isp_enable_irq(struct isp_device *dev)
{
	REG_SET(dev->base + IRQ_MERGER_IMSC_DEBUG1, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_IMSC_FRPROC0, 0xFFFFFFFF);
}

void isp_clear_irq_state(struct isp_device *dev)
{
	REG_SET(dev->base + IRQ_MERGER_ICR_DEBUG0, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_DEBUG1, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_DEBUG2, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_DEBUG3, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_ERROR0, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_ERROR1, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_FRPROC0, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_FRPROC1, 0xFFFFFFFF);
	REG_SET(dev->base + IRQ_MERGER_ICR_FRPROC2, 0xFFFFFFFF);

	return;
}

irqreturn_t isp_vic1_handler(int irq, void *dev_id)
{
	struct isp_device *d = (struct isp_device *)dev_id;

	unsigned int val = isp_clear_irq(d, IRQ_MERGER_DEBUG_1);

	if (val & (1 << IRQ_MERGER_STREAM_ROUTER_4__CVDR_RT__SOF_VPWR_23_OFFSET)) {
		isp_cvdr_config(d, d->hw_addr + frame_num2Offset(d, d->isp_frame_num + 1));
	}

	return IRQ_HANDLED;
}

irqreturn_t isp_frproc0_handler(int irq, void *dev_id)
{
	struct isp_device *d = (struct isp_device *)dev_id;

	unsigned int val = isp_clear_irq(d, IRQ_MERGER_FRPROC_0);

	if (val & (1 << IRQ_MERGER_STREAM_ROUTER_4__CVDR_RT__EOF_VPWR_23_OFFSET)) {
		isp_sr_go(d, (1 << (4*d->csi_index)));
		d->isp_frame_num++;
		complete_all(&d->frame_comp);
	}

	return IRQ_HANDLED;
}
