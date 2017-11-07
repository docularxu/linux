/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_ISP_SR_DEF_H
#define HISI_ISP_SR_DEF_H

#include "hisi_isp_core.h"

int isp_sr_config(struct isp_device *dev);
void isp_sr_go(struct isp_device *dev, unsigned int go_bit);
#endif /* HISI_ISP_SR_DEF_H */
