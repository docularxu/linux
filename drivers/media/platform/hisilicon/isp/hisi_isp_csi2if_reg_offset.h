/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __CSI2IF_REG_OFFSET_H__
#define __CSI2IF_REG_OFFSET_H__

#define CSI2IF_VERSION_REG               0x0   /* Version of the CSI-2 host controller. */
#define CSI2IF_N_LANES_REG               0x4   /* Number of active data lanes. */
#define CSI2IF_CSI2_RESETN_REG           0x8   /* CSI-2 controller reset. */
#define CSI2IF_INT_ST_MAIN_REG           0xC   /* Clear in read register. */
#define CSI2IF_DATA_IDS_1_REG            0x10  /* Data IDs for which IDI reports line boundary matching errors. */
#define CSI2IF_PHY_SHUTDOWNZ_REG         0x40  /* D-PHY shutdown control. */
#define CSI2IF_DPHY_RSTZ_REG             0x44  /* D-PHY reset control. */
#define CSI2IF_PHY_RX_REG                0x48  /* General settings for all blocks. */
#define CSI2IF_PHY_STOPSTATE_REG         0x4C  /* contains the stopstate signal status from the synopsys D-PHY. */
#define CSI2IF_PHY_TEST_CTRL0_REG        0x50  /* D-PHY Test interface control 0. */
#define CSI2IF_PHY_TEST_CTRL1_REG        0x54  /* D-PHY Test interface control 1. */
#define CSI2IF_PHY_CAL_REG               0xCC  /* contains the calibration signal status from DPHY. */
#define CSI2IF_INT_ST_PHY_FATAL_REG      0xE0  /* Groups the fatal interruptions caused by PHY Packet discarded. */
#define CSI2IF_INT_MSK_PHY_FATAL_REG     0xE4  /* Interrupt Mask for INT_ST_PHY_FATAL. */
#define CSI2IF_INT_FORCE_PHY_FATAL_REG   0xE8  /* Interrupt Set for INT_ST_PHY_FATAL. */
#define CSI2IF_INT_ST_PKT_FATAL_REG      0xF0  /* Groups the fatal interrupt related with packet construction. */
#define CSI2IF_INT_MSK_PKT_FATAL_REG     0xF4  /* interrupr mask for INT_ST_PKT_FATAL. */
#define CSI2IF_INT_FORCE_PKT_FATAL_REG   0xF8  /* interrupr mask for INT_ST_PKT_FATAL. */
#define CSI2IF_INT_ST_FRAME_FATAL_REG    0x100 /* groups interrupt related with frame construction. */
#define CSI2IF_INT_MSK_FRAME_FATAL_REG   0x104 /* interrupt mask for INT_ST_FRAME_FATAL. */
#define CSI2IF_INT_FORCE_FRAME_FATAL_REG 0x108 /* interrupt set for INT_ST_FRAME_FATAL. */
#define CSI2IF_INT_ST_PHY_REG            0x110 /* groups and notifies which interruption bits caused the interruption. */
#define CSI2IF_INT_MSK_PHY_REG           0x114 /* interrupt mask for INT_ST_PHY. */
#define CSI2IF_INT_FORCE_PHY_REG         0x118 /* interrupt set for INT_ST_PHY. */
#define CSI2IF_INT_ST_PKT_REG            0x120 /* groups and notifies  interruption related with Packet construction. */
#define CSI2IF_INT_MSK_PKT_REG           0x124 /* interrupt mask for INT_ST_PKT. */
#define CSI2IF_INT_FORCE_PKT_REG         0x128 /* interrupt set for INT_ST_PKT. */
#define CSI2IF_INT_ST_LINE_REG           0x130 /* interrupt related with Line construction. */
#define CSI2IF_INT_MSK_LINE_REG          0x134 /* interrupt mask for INT_ST_LINE. */
#define CSI2IF_INT_FORCE_LINE_REG        0x138 /* interrupt set  for INT_ST_LINE. */

#endif
