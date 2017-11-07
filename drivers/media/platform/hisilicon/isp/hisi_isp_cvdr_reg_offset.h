/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef __CVDR_REG_OFFSET_H__
#define __CVDR_REG_OFFSET_H__

#define CVDR_CVDR_CVDR_CFG_REG           0x0   /* CVDR config register */
#define CVDR_CVDR_CVDR_DEBUG_EN_REG      0x4   /* CVDR debug register enable */
#define CVDR_CVDR_CVDR_DEBUG_REG         0x8   /* CVDR debug register */
#define CVDR_CVDR_CVDR_WR_QOS_CFG_REG    0xC   /* AXI Write QOS/Pressure configuration */
#define CVDR_CVDR_CVDR_RD_QOS_CFG_REG    0x10  /* AXI Read QOS/Pressure configuration */
#define CVDR_CVDR_OTHER_RO_REG           0x14  /* Spare Other RO */
#define CVDR_CVDR_OTHER_RW_REG           0x18  /* Spare Other RW */
#define CVDR_CVDR_VP_WR_CFG_0_REG        0x1C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_0_REG     0x20  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_0_REG   0x24  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_0_REG   0x28  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_1_REG        0x2C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_1_REG     0x30  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_1_REG   0x34  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_1_REG   0x38  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_2_REG        0x3C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_2_REG     0x40  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_2_REG   0x44  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_2_REG   0x48  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_3_REG        0x4C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_3_REG     0x50  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_3_REG   0x54  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_3_REG   0x58  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_4_REG        0x5C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_4_REG     0x60  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_4_REG   0x64  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_4_REG   0x68  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_5_REG        0x6C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_5_REG     0x70  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_5_REG   0x74  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_5_REG   0x78  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_8_REG        0x9C  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_8_REG     0xA0  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_8_REG   0xA4  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_8_REG   0xA8  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_9_REG        0xAC  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_9_REG     0xB0  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_9_REG   0xB4  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_9_REG   0xB8  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_10_REG       0xBC  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_10_REG    0xC0  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_10_REG  0xC4  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_10_REG  0xC8  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_11_REG       0xCC  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_11_REG    0xD0  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_11_REG  0xD4  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_11_REG  0xD8  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_12_REG       0xDC  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_12_REG    0xE0  /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_12_REG  0xE4  /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_12_REG  0xE8  /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_14_REG       0xFC  /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_14_REG    0x100 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_14_REG  0x104 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_14_REG  0x108 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_15_REG       0x10C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_15_REG    0x110 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_15_REG  0x114 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_15_REG  0x118 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_16_REG       0x11C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_16_REG    0x120 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_16_REG  0x124 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_16_REG  0x128 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_17_REG       0x12C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_17_REG    0x130 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_17_REG  0x134 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_17_REG  0x138 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_18_REG       0x13C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_18_REG    0x140 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_18_REG  0x144 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_18_REG  0x148 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_19_REG       0x14C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_19_REG    0x150 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_19_REG  0x154 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_19_REG  0x158 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_20_REG       0x15C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_20_REG    0x160 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_20_REG  0x164 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_20_REG  0x168 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_21_REG       0x16C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_21_REG    0x170 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_21_REG  0x174 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_21_REG  0x178 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_22_REG       0x17C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_22_REG    0x180 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_22_REG  0x184 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_22_REG  0x188 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_23_REG       0x18C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_23_REG    0x190 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_23_REG  0x194 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_23_REG  0x198 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_24_REG       0x19C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_24_REG    0x1A0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_24_REG  0x1A4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_24_REG  0x1A8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_25_REG       0x1AC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_25_REG    0x1B0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_25_REG  0x1B4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_25_REG  0x1B8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_26_REG       0x1BC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_26_REG    0x1C0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_26_REG  0x1C4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_26_REG  0x1C8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_27_REG       0x1CC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_27_REG    0x1D0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_27_REG  0x1D4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_27_REG  0x1D8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_28_REG       0x1DC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_28_REG    0x1E0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_28_REG  0x1E4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_28_REG  0x1E8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_29_REG       0x1EC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_29_REG    0x1F0 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_29_REG  0x1F4 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_29_REG  0x1F8 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_30_REG       0x1FC /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_30_REG    0x200 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_30_REG  0x204 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_30_REG  0x208 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_31_REG       0x20C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_31_REG    0x210 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_31_REG  0x214 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_31_REG  0x218 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_32_REG       0x21C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_32_REG    0x220 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_32_REG  0x224 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_32_REG  0x228 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_33_REG       0x22C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_33_REG    0x230 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_33_REG  0x234 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_33_REG  0x238 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_34_REG       0x23C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_34_REG    0x240 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_34_REG  0x244 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_34_REG  0x248 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_35_REG       0x24C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_35_REG    0x250 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_35_REG  0x254 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_35_REG  0x258 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_36_REG       0x25C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_36_REG    0x260 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_36_REG  0x264 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_36_REG  0x268 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_VP_WR_CFG_37_REG       0x26C /* Video port write Configuration */
#define CVDR_CVDR_VP_WR_AXI_FS_37_REG    0x270 /* AXI address Frame start */
#define CVDR_CVDR_VP_WR_AXI_LINE_37_REG  0x274 /* AXI line wrap and line stride */
#define CVDR_CVDR_VP_WR_IF_CFG_37_REG  0x278 /* Configure the IF_CFG mechanism and must be aligned with the SMMU configuration. */
#define CVDR_CVDR_LIMITER_VP_WR_0_REG    0x400 /* Video port write Access limiter */

#endif /* __CVDR_REG_OFFSET_H__ */
