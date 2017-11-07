/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_ISP_COMMON_DEF_H
#define HISI_ISP_COMMON_DEF_H

#include "hisi_isp_core.h"
#include "i2c.h"

typedef enum _yuv_data_type_e {
    YUV_DT_420_8BITS  = 0x18,
    YUV_DT_420_10BITS  = 0x19,
    YUV_DT_LEGACY_420_8BITS  = 0x1A,
    YUV_DT_420_8BITS_C  = 0x1C,
    YUV_DT_420_10BITS_C  = 0x1D,
    YUV_DT_422_8BITS  = 0x1E,
    YUV_DT_422_10BITS  = 0x1F,
} yuv_data_type_e;

typedef enum _raw_data_type_e {
    RAW_DT_RAW6  = 0x28,
    RAW_DT_RAW7  = 0x29,
    RAW_DT_RAW8  = 0x2A,
    RAW_DT_RAW10 = 0x2B,
    RAW_DT_RAW12 = 0x2C,
    RAW_DT_RAW14 = 0x2D,
} raw_data_type_e;

int frame_num2Offset(struct isp_device *dev, int frame_num);

int isp_start_streaming(struct isp_device *dev);
int isp_stop_streaming(struct isp_device *dev);

#endif /* HISI_ISP_COMMON_DEF_H */
