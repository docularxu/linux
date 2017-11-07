/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "hisi_isp_sr.h"
#include "hisi_isp_common.h"
#include "hisi_isp_sr_reg_offset.h"

/* Define the union U_ID_ROUTER_1 */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reserved_0            : 16  ; /* [15..0]  */
		unsigned int    idr_input_stream_6    : 4   ; /* [19..16]  */
		unsigned int    idr_enable_6          : 1   ; /* [20]  */
		unsigned int    reserved_1            : 3   ; /* [23..21]  */
		unsigned int    idr_input_stream_7    : 4   ; /* [27..24]  */
		unsigned int    idr_enable_7          : 1   ; /* [28]  */
		unsigned int    reserved_2            : 3   ; /* [31..29]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_ID_ROUTER_1;

/* Define the union U_REFORMAT */
typedef union {
	/* Define the struct bits */
	struct {
		unsigned int    reformat_num_lines    : 13  ; /* [12..0]  */
		unsigned int    reformat_pixel_reorder : 3   ; /* [15..13]  */
		unsigned int    reformat_num_pixels   : 13  ; /* [28..16]  */
		unsigned int    reserved_0            : 2   ; /* [30..29]  */
		unsigned int    reformat_enable       : 1   ; /* [31]  */
	} bits;

	/* Define an unsigned member */
	unsigned int    u32;

} U_REFORMAT;

#define STREAM_ROUTER_REFORMAT_MINSPACE_REG(x)  \
	(STREAM_ROUTER_REFORMAT_MINSPACE_0_REG + (x)*(STREAM_ROUTER_REFORMAT_MINSPACE_1_REG - STREAM_ROUTER_REFORMAT_MINSPACE_0_REG))

#define STREAM_ROUTER_REFORMAT_REG(x)   \
	(STREAM_ROUTER_REFORMAT_0_REG + (x)*(STREAM_ROUTER_REFORMAT_1_REG - STREAM_ROUTER_REFORMAT_0_REG))

int isp_sr_config(struct isp_device *dev)
{
	int data_type = 1;
	U_ID_ROUTER_1 reg1;
	U_REFORMAT reg;

	REG_SET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_CSIFILTER_A_REG + 0x04 * dev->csi_index,
			(0 << 6) | (dev->dt << 0));

	reg1.u32 = REG_GET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_ID_ROUTER_1_REG);
	reg1.bits.idr_enable_6 = 1;
	reg1.bits.idr_input_stream_6 = (dev->csi_index << 2) | (0 << 0);
	REG_SET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_ID_ROUTER_1_REG, reg1.u32);

	reg.u32 = 0;
	reg.bits.reformat_enable = 1;
	reg.bits.reformat_num_pixels = dev->w * 2 - 1;
	reg.bits.reformat_num_lines  = dev->h - 1;
	reg.bits.reformat_pixel_reorder = data_type;
	REG_SET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_REFORMAT_REG(6), reg.u32);

	REG_SET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_REFORMAT_MINSPACE_REG(6),
			(0x1F << 0));            /* REFORMAT_MINSPACE0 */

	return 0;
}

void isp_sr_go(struct isp_device *dev, unsigned int go_bit)
{
	REG_SET(dev->base + ISP_BASE_ADDR_STREAM_ROUTER + STREAM_ROUTER_CSIFILTER_GO_REG, go_bit);
}
