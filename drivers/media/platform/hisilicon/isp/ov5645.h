/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#ifndef HISI_OV5645_H
#define HISI_OV5645_H

#include "hisi_isp_core.h"
#include "i2c.h"

#define OV5645_SLAVE_ID        0x78

int ov5645_power(i2c_client_t *client, int on);
int ov5645_get_chip_id(i2c_client_t *client, unsigned int *chip_id);
int ov5645_stream_on_off(i2c_client_t *client, int enable);
int ov5645_set_fmt(i2c_client_t *client, unsigned int w, unsigned int h);
#endif /* HISI_OV5645_H */
