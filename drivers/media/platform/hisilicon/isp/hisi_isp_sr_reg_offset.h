/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __STREAM_ROUTER_REG_OFFSET_H__
#define __STREAM_ROUTER_REG_OFFSET_H__

#define STREAM_ROUTER_CSIFILTER_A_REG             0x0   /* CSI2 VC/DT filtering for CSIA interface */
#define STREAM_ROUTER_CSIFILTER_B_REG             0x4   /* CSI2 VC/DT filtering for CSIB interface */
#define STREAM_ROUTER_CSIFILTER_C_REG             0x8   /* CSI2 VC/DT filtering for CSIC interface */
#define STREAM_ROUTER_CSIFILTER_GO_REG            0xC   /* CSI2 stream go bits */
#define STREAM_ROUTER_PRESSURE_START_REG          0x10  /* Pressure start configuration register */
#define STREAM_ROUTER_PRESSURE_STOP_REG           0x14  /* Pressure stop configuration register */
#define STREAM_ROUTER_ID_ROUTER_0_REG             0x18  /* ID router configuration (field 0) */
#define STREAM_ROUTER_ID_ROUTER_1_REG             0x1C  /* ID router configuration (field 1) */
#define STREAM_ROUTER_REFORMAT_0_REG              0x20  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_1_REG              0x28  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_2_REG              0x30  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_3_REG              0x38  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_4_REG              0x40  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_5_REG              0x48  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_6_REG              0x50  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_7_REG              0x58  /* Reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_0_REG     0x24  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_1_REG     0x2C  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_2_REG     0x34  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_3_REG     0x3C  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_4_REG     0x44  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_5_REG     0x4C  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_6_REG     0x54  /* Min space reformatter configuration */
#define STREAM_ROUTER_REFORMAT_MINSPACE_7_REG     0x5C  /* Min space reformatter configuration */
#define STREAM_ROUTER_EOL_CNT_REG                 0x60  /* End Of Line counter interrupt event configuration */
#define STREAM_ROUTER_DPCM_0_REG                  0x78  /* DPCM config register */
#define STREAM_ROUTER_DPCM_1_REG                  0x7C  /* DPCM config register */
#define STREAM_ROUTER_DPCM_2_REG                  0x80  /* DPCM config register */
#define STREAM_ROUTER_VP_ROUTER_0_REG             0x90  /* Video port router configuration */
#define STREAM_ROUTER_VP_ROUTER_1_REG             0x94  /* Video port router configuration */
#define STREAM_ROUTER_VP_ROUTER_2_REG             0x98  /* Video port router configuration */
#define STREAM_ROUTER_VP_ROUTER_3_REG             0x9C  /* Video port router configuration */
#define STREAM_ROUTER_PIXFRAG_CONVERT_REG         0xA0  /* pixfrag conversion before sending the data out of stream router */

#endif
