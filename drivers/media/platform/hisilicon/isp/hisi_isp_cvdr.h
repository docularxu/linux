/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_ISP_CVDR_DEF_H
#define HISI_ISP_CVDR_DEF_H

#include "hisi_isp_core.h"

typedef enum _cvdr_pix_fmt_e {
	DF_1PF8  = 0x0,
	DF_1PF10 = 0x1,
	DF_1PF12 = 0x2,
	DF_1PF14 = 0x3,
	DF_2PF8  = 0x4,
	DF_2PF10 = 0x5,
	DF_2PF12 = 0x6,
	DF_2PF14 = 0x7,
	DF_3PF8  = 0x8,
	DF_3PF10 = 0x9,
	DF_3PF12 = 0xA,
	DF_3PF14 = 0xB,
	DF_D32   = 0xC,
	DF_D48   = 0xD,
	DF_D64   = 0xE,
	DF_FMT_INVALID,
} cvdr_pix_fmt_e;

typedef enum _vp_wr_id_e {
	W12_1       = 0,
	W12_2       = 1,
	W6_1        = 2,
	W6_2        = 3,
	W5_1_1      = 4,
	W5_1_2      = 5,
	W4_1_1      = 8,
	W4_1_2      = 9,
	W4_2_1      = 10,
	W4_2_2      = 11,
	W7_1        = 12,
	W13_1       = 14,
	W13_2       = 15,
	W1_1        = 16,
	W1_2        = 17,
	W14_1       = 18,
	W14_2       = 19,
	W11_1       = 20,
	W11_2       = 21,
	W2_3        = 22,
	W2_4        = 23,
	W2_5        = 24,
	W8_1        = 25,
	W3_1        = 26,
	W2_2        = 27,
	W2_1        = 28,
	W15_1       = 29,
	W16_1       = 30,
	W16_2       = 31,
	W17_1       = 32,
	W17_2       = 33,
	W19_1       = 34,
	W19_2       = 35,
	W20_1       = 36,
	W20_2       = 37,
} vp_wr_id_e;

typedef struct _cvdr_wr_fmt_desc_t {
	unsigned int        fs_addr;
	unsigned int        last_page;

	cvdr_pix_fmt_e      pix_fmt;
	unsigned char       pix_expan;
	unsigned short      line_stride;
	unsigned short      line_wrap;
} cvdr_wr_fmt_desc_t;

typedef struct _cvdr_bw_cfg_t {
	unsigned char       bw_limiter0;
	unsigned char       bw_limiter1;
	unsigned char       bw_limiter2;
	unsigned char       bw_limiter3;
	unsigned char       bw_limiter_reload;
} cvdr_bw_cfg_t;

int isp_cvdr_config(struct isp_device *dev, dma_addr_t hw_addr);
int isp_cvdr_init(struct isp_device *dev);

#endif /* HISI_ISP_CVDR_DEF_H */
