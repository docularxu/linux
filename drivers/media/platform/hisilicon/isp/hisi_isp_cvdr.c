/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "hisi_isp_cvdr.h"
#include "hisi_isp_cvdr_reg_offset.h"
#include "hisi_isp_cvdr_priv.h"

#define CVDR_VP_RD_NBR    (22)
#define CVDR_VP_WR_NBR    (38)
#define CVDR_NR_WR_NBR    (4)
#define CVDR_NR_RD_NBR    (8)

#define ONE_REG_OFFSET   (0x4)

#define VP_WR_REG_OFFSET (0x10)

#define ISP_CLK   (480)
#define DERATE    (1.2)

#define CVDR_ALIGN_BYTES            (16)

typedef struct _cvdr_smmu_cfg_t {
	unsigned char   to_use;

	unsigned int   num;
	unsigned int   smr_nscfg;
} cvdr_smmu_cfg_t;



typedef struct _cvdr_vp_wr_cfg_t {
	unsigned char           to_use;
	unsigned char           id;
	cvdr_wr_fmt_desc_t      fmt;
	cvdr_bw_cfg_t           bw;
} cvdr_vp_wr_cfg_t;

typedef struct _cvdr_rd_fmt_desc_t {
	unsigned int        fs_addr;
	unsigned int        last_page;
	cvdr_pix_fmt_e      pix_fmt;
	unsigned char       pix_expan;
	unsigned short      allocated_du;
	unsigned short      line_size;
	unsigned short      hblank;
	unsigned short      frame_size;
	unsigned short      vblank;
	unsigned short      line_stride;
	unsigned short      line_wrap;
} cvdr_rd_fmt_desc_t;

typedef struct _cvdr_vp_rd_cfg_t {
	unsigned char           to_use;
	unsigned char           id;
	cvdr_rd_fmt_desc_t      fmt;
	cvdr_bw_cfg_t           bw;
} cvdr_vp_rd_cfg_t;

typedef struct _cvdr_nr_wr_cfg_t {
	unsigned char       to_use;
	unsigned char       nr_wr_stop_en_du_thr;
	unsigned char       nr_wr_stop_en_flux_ctrl;
	unsigned char       nr_wr_stop_en_pressure;
	unsigned char       nr_wr_stop_ok;
	unsigned char       nr_wr_stop;
	unsigned char       en;
	cvdr_bw_cfg_t       bw;
} cvdr_nr_wr_cfg_t;

typedef struct _cvdr_nr_rd_cfg_t {
	unsigned char       to_use;
	unsigned short      allocated_du;
	unsigned char       nr_rd_stop_en_du_thr;
	unsigned char       nr_rd_stop_en_flux_ctrl;
	unsigned char       nr_rd_stop_en_pressure;
	unsigned char       nr_rd_stop_ok;
	unsigned char       nr_rd_stop;
	unsigned char       en;
	cvdr_bw_cfg_t       bw;
} cvdr_nr_rd_cfg_t;

typedef struct _cfg_tab_cvdr_t {
	cvdr_smmu_cfg_t      smmu_nr_rd_cfg[CVDR_NR_RD_NBR];
	cvdr_smmu_cfg_t      smmu_vp_wr_cfg[CVDR_VP_WR_NBR];
	cvdr_smmu_cfg_t      smmu_vp_rd_cfg[CVDR_VP_RD_NBR];
	cvdr_vp_wr_cfg_t      vp_wr_cfg[CVDR_VP_WR_NBR];
	cvdr_vp_rd_cfg_t      vp_rd_cfg[CVDR_VP_RD_NBR];
	cvdr_nr_wr_cfg_t      nr_wr_cfg[CVDR_NR_WR_NBR];
	cvdr_nr_rd_cfg_t      nr_rd_cfg[CVDR_NR_RD_NBR];
} cfg_tab_cvdr_t;

typedef struct _cvdr_opt_bw_t {
	unsigned int       srt;
	unsigned int       pclk;
	unsigned int       throughput;
} cvdr_opt_bw_t;

typedef enum _cvdr_dev_e {
	CVDR_RT		= 0,
	CVDR_SRT	= 1,
} cvdr_dev_e;

typedef enum _vp_rd_id_e {
	R3_1        = 0,
	R11_1       = 4,
	R12_1       = 5,
	R11_2       = 6,
	R12_2       = 7,
	R10_1       = 8,
	R10_2       = 9,
	R2_1        = 10,
	R8_1_1      = 11,
	R8_2_1      = 12,
	R8_1_2      = 13,
	R8_2_2      = 14,
	R13_1       = 15,
	R13_2       = 16,
	R5_1_4      = 19,
	R5_1_5      = 20,
	R5_1_6      = 21,
} vp_rd_id_e;

static cvdr_opt_bw_t cvdr_vp_wr_bw[CVDR_VP_WR_NBR] = {
	[W12_1]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[W12_2]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[W6_1]   = {CVDR_SRT, 480, (float)DERATE*480*2},
	[W6_2]   = {CVDR_SRT, 480, (float)DERATE*480*2},
	[W5_1_1] = {CVDR_SRT, 480, (float)DERATE*480},
	[W5_1_2] = {CVDR_SRT, 480, (float)DERATE*480},
	[W4_1_1] = {CVDR_SRT, 480, (float)DERATE*480},
	[W4_1_2] = {CVDR_SRT, 480, (float)DERATE*480},
	[W4_2_1] = {CVDR_SRT, 480, (float)DERATE*480},
	[W4_2_2] = {CVDR_SRT, 480, (float)DERATE*480},
	[W7_1]   = {CVDR_SRT, 480, (float)DERATE*480},
	[W13_1]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[W13_2]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[W1_1]   = {CVDR_RT,  720, 720*2},
	[W1_2]   = {CVDR_RT,  720, 720*2},
	[W14_1]  = {CVDR_RT,  720, 720*2},
	[W14_2]  = {CVDR_RT,  720, 720*2},
	[W11_1]  = {CVDR_RT,  585, ISP_CLK},
	[W11_2]  = {CVDR_RT,  585, ISP_CLK},
	[W2_3]   = {CVDR_RT,  720, 720*2},
	[W2_4]   = {CVDR_RT,  720, 720*2},
	[W2_5]   = {CVDR_RT,  720, 720*2},
	[W8_1]   = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[W3_1]   = {CVDR_RT,  480/4, 4*480/29},
	[W2_2]   = {CVDR_RT,  720, 720*2},
	[W2_1]   = {CVDR_RT,  720, 720*2},
	[W15_1]  = {CVDR_RT,  585, 374},
	[W16_1]  = {CVDR_RT,  585, 187},
	[W16_2]  = {CVDR_RT,  585, 187},
	[W17_1]  = {CVDR_RT,  585, ISP_CLK},
	[W17_2]  = {CVDR_RT,  585, ISP_CLK},
	[W19_1]  = {CVDR_RT,  ISP_CLK, (float)DERATE*ISP_CLK*16/8},
	[W19_2]  = {CVDR_RT,  ISP_CLK, (float)DERATE*ISP_CLK*16/8},
	[W20_1]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
	[W20_2]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
};

static cvdr_opt_bw_t cvdr_vp_rd_bw[CVDR_VP_RD_NBR] = {
	[R3_1]   = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[R11_1]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[R12_1]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[R11_2]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[R12_2]  = {CVDR_SRT, ISP_CLK, ISP_CLK},
	[R10_1]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
	[R10_2]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
	[R2_1]   = {CVDR_RT,  ISP_CLK, ISP_CLK*2},
	[R8_1_1] = {CVDR_SRT, ISP_CLK, ISP_CLK*2*DERATE},
	[R8_2_1] = {CVDR_SRT, ISP_CLK, ISP_CLK*2*DERATE},
	[R8_1_2] = {CVDR_SRT, ISP_CLK, ISP_CLK*2*DERATE},
	[R8_2_2] = {CVDR_SRT, ISP_CLK, ISP_CLK*2*DERATE},
	[R13_1]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
	[R13_2]  = {CVDR_RT,  ISP_CLK, ISP_CLK},
	[R5_1_4] = {CVDR_SRT, 240, (float)DERATE*2*240},
	[R5_1_5] = {CVDR_SRT, 240, (float)DERATE*1.5*240},
	[R5_1_6] = {CVDR_SRT, 240, (float)DERATE*1.5*240},
};

#define CVDR_SRT_CVDR_CFG_REG          0x0   /* CVDR config register. */
#define CVDR_RT_CVDR_CFG_REG           0x0   /* CVDR config register. */
#define CVDR_SRT_VP_WR_IF_CFG_0_REG    0x28  /* Video port write interface configuration: prefetch or reset or stall capability. */
#define CVDR_SRT_VP_RD_IF_CFG_0_REG    0x514 /* Video port read interface configuration: prefetch or reset or stall capability. */

int cvdr_set_vp_wr_ready(char __iomem *base, unsigned char port, cvdr_wr_fmt_desc_t *desc, cvdr_bw_cfg_t *bw)
{
	U_VP_WR_CFG tmp_cfg;
	U_VP_WR_AXI_FS tmp_fs;
	U_VP_WR_AXI_LINE tmp_line;
	U_LIMITER_VP_WR tmp_limiter;

	loge_if_ret(desc->fs_addr & 0xF);

	tmp_cfg.u32  = 0;
	tmp_fs.u32   = 0;
	tmp_line.u32 = 0;
	tmp_limiter.u32 = 0;

	if (NULL == bw) {
		ISP_ERR("vdr_bw_cfg_t* bw NULL!\n");
		return -1;
	}

	tmp_cfg.bits.vpwr_pixel_format       = desc->pix_fmt;
	tmp_cfg.bits.vpwr_pixel_expansion    = desc->pix_expan;
	tmp_cfg.bits.vpwr_last_page          = desc->last_page;

	tmp_fs.bits.vpwr_address_frame_start = desc->fs_addr >> 4;

	tmp_line.bits.vpwr_line_stride       = desc->line_stride;
	tmp_line.bits.vpwr_line_wrap         = desc->line_wrap;

	tmp_limiter.bits.vpwr_access_limiter_0 = bw->bw_limiter0;
	tmp_limiter.bits.vpwr_access_limiter_1 = bw->bw_limiter1;
	tmp_limiter.bits.vpwr_access_limiter_2 = bw->bw_limiter2;
	tmp_limiter.bits.vpwr_access_limiter_3 = bw->bw_limiter3;
	tmp_limiter.bits.vpwr_access_limiter_reload = bw->bw_limiter_reload;

	switch (port) {
		/*VP_WR @CVDR_SRT&CMDLST TO CONFIG */
	case W4_1_1:
	case W4_1_2:
	case W4_2_1:
	case W4_2_2:
	case W5_1_1:
	case W5_1_2:
	case W19_1:
	case W19_2:
	case W6_1:
	case W6_2:
	case W7_1:
	case W3_1:
		ISP_ERR("not support yet\n");
		break;

	default:
		REG_SET(base + CVDR_CVDR_LIMITER_VP_WR_0_REG + ONE_REG_OFFSET * port, tmp_limiter.u32);
		REG_SET(base + CVDR_CVDR_VP_WR_CFG_0_REG + VP_WR_REG_OFFSET * port, tmp_cfg.u32);
		REG_SET(base + CVDR_CVDR_VP_WR_AXI_LINE_0_REG + VP_WR_REG_OFFSET * port, tmp_line.u32);
		REG_SET(base + CVDR_CVDR_VP_WR_AXI_FS_0_REG + VP_WR_REG_OFFSET * port, tmp_fs.u32);
		break;
	}

	return 0;
}

int isp_cvdr_config(struct isp_device *dev, dma_addr_t hw_addr)
{
	cvdr_wr_fmt_desc_t cvdr_wr_fmt = {
		.fs_addr = hw_addr,
		.pix_fmt = dev->pf,
		.pix_expan = 0,
		.last_page = (dev->w * dev->h * 2 + hw_addr) >> 15,
		.line_stride = dev->w*2/CVDR_ALIGN_BYTES - 1,
		.line_wrap = 8000,
	};
	cvdr_bw_cfg_t cvdr_bw = {0xF, 0xF, 0xF, 0xF, 0xF};

	return cvdr_set_vp_wr_ready(dev->base + ISP_BASE_ADDR_CVDR_RT, W2_4, &cvdr_wr_fmt, &cvdr_bw);
}

int isp_cvdr_init(struct isp_device *dev)
{
	unsigned int i;
	unsigned int prefetch_bypass;
	char __iomem *base = dev->base;
	char __iomem *reg_addr;
	unsigned int reg_val;

	prefetch_bypass = 1;

	for (i = 0; i < CVDR_VP_WR_NBR; ++i) {
		reg_addr = ((cvdr_vp_wr_bw[i].srt == CVDR_SRT) ?
				(base + ISP_BASE_ADDR_CVDR_SRT) : (base + ISP_BASE_ADDR_CVDR_RT))
			+ CVDR_SRT_VP_WR_IF_CFG_0_REG + 0x10 * i;
		reg_val = REG_GET(reg_addr);
		reg_val = (reg_val & 0x7FFFFFFF) | (prefetch_bypass << 31);
		REG_SET(reg_addr, reg_val);
	}

	for (i = 0; i < CVDR_VP_RD_NBR; ++i) {
		reg_addr = ((cvdr_vp_rd_bw[i].srt == CVDR_SRT) ?
				(base + ISP_BASE_ADDR_CVDR_SRT) : (base + ISP_BASE_ADDR_CVDR_RT))
			+ CVDR_SRT_VP_RD_IF_CFG_0_REG + 0x20 * i;
		reg_val = REG_GET(reg_addr);
		reg_val = (reg_val & 0x7FFFFFFF) | (prefetch_bypass << 31);
		REG_SET(reg_addr, reg_val);
	}

	/* CVDR RT*/
	REG_SET(base + ISP_BASE_ADDR_CVDR_RT + CVDR_RT_CVDR_CFG_REG,
			(1 << 0)
			| (64 << 8)
			| (11 << 16)
			| (11 << 24)
		   );

	/* CVDR SRT*/
	REG_SET(base + ISP_BASE_ADDR_CVDR_SRT + CVDR_SRT_CVDR_CFG_REG,
			(1 << 0)
			| (34 << 8)
			| (19 << 16)
			| (11 << 24)
		   );

	REG_SET(base + 0x22000 + 0x00C, 0xf8765432);
	REG_SET(base + 0x22000 + 0x010, 0xf8122334);

	REG_SET(base + 0x2E000 + 0x00C, 0xd0765432);
	REG_SET(base + 0x2E000 + 0x010, 0xd0122334);


	REG_SET(base + SUB_CTRL_BASE_ADDR + 0x190, 0x00026e10);
	REG_SET(base + SUB_CTRL_BASE_ADDR + 0x194, 0x0000021f);

	REG_SET(base + SUB_CTRL_BASE_ADDR + 0x198, 0x00027210);
	REG_SET(base + SUB_CTRL_BASE_ADDR + 0x19C, 0x0000024e);

	return 0;
}
