/*
 * Copyright (c) 2016-2017 Linaro Ltd.
 * Copyright (c) 2016-2017 HiSilicon Technologies Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "hisi_isp_csi.h"
#include "hisi_isp_csi2if_reg_offset.h"

#define CSI2IF_PHY_STOPSTATEDATA_0_OFFSET 0
#define CSI2IF_PHY_STOPSTATEDATA_0_LEN    1
#define CSI2IF_PHY_STOPSTATECLK_OFFSET    16
#define CSI2IF_PHY_STOPSTATECLK_LEN       1
#define CSI2IF_PHY_RXCLKACTIVEHS_OFFSET 17
#define CSI2IF_PHY_RXCLKACTIVEHS_LEN    1

#define CSI_INDEX_CNT    (3)

#define CFG_CLK_ATTR   (0x01)
#define CFG_CLK_DETECT (0x0A)
#define CFG_DESKEW_1   (0x0B)
#define CFG_DESKEW_2   (0x0F)
#define LANE0_SETTLE   (0x30)
#define LANE0_MISC     (0x31)
#define LANE0_ADDITION (0x32)
#define LANE0_DESKEW_1 (0x3A)
#define LANE0_DESKEW_2 (0x3B)
#define LANE0_DESKEW_3 (0x3C)
#define LANE1_SETTLE   (0x40)
#define LANE1_MISC     (0x41)
#define LANE1_ADDITION (0x42)
#define LANE1_DESKEW_1 (0x4A)
#define LANE1_DESKEW_2 (0x4B)
#define LANE1_DESKEW_3 (0x4C)
#define LANE2_SETTLE   (0x50)
#define LANE2_MISC     (0x51)
#define LANE2_ADDITION (0x52)
#define LANE2_DESKEW_1 (0x5A)
#define LANE2_DESKEW_2 (0x5B)
#define LANE2_DESKEW_3 (0x5C)
#define LANE3_SETTLE   (0x60)
#define LANE3_MISC     (0x61)
#define LANE3_ADDITION (0x62)
#define LANE3_DESKEW_1 (0x6A)
#define LANE3_DESKEW_2 (0x6B)
#define LANE3_DESKEW_3 (0x6C)

#define TSTCODE_SETREG8(reg_base, addr, value) \
	do { \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL1_REG), (1 << 16) | addr);           \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 2);          \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 0);          \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL1_REG), value);                  \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 2);          \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 0);          \
	} while (0)

#define TSTCODE_GETREG8(reg_base, addr, value)\
	do { \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL1_REG), (1 << 16) | addr);         \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 2);          \
		REG_SET((reg_base+CSI2IF_PHY_TEST_CTRL0_REG), 0);          \
		(value) = ((REG_GET(reg_base+CSI2IF_PHY_TEST_CTRL1_REG) >> 8) & (0x000000ff)); \
	} while (0)

static int csi2if_dphy_init(char __iomem *base, unsigned char settle_time)
{
	unsigned char value = 0;

	if (settle_time > 0) { /* configure settle time mannually */
		TSTCODE_SETREG8(base, LANE0_SETTLE, settle_time);
		TSTCODE_SETREG8(base, LANE1_SETTLE, settle_time);
		TSTCODE_SETREG8(base, LANE2_SETTLE, settle_time);
		TSTCODE_SETREG8(base, LANE3_SETTLE, settle_time);
		TSTCODE_SETREG8(base, CFG_CLK_DETECT, 0x01);
	} else { /* enable clock detect */
		TSTCODE_SETREG8(base, LANE0_SETTLE, 0x01);
		TSTCODE_SETREG8(base, LANE0_ADDITION, 0x05);
		TSTCODE_SETREG8(base, LANE1_SETTLE, 0x01);
		TSTCODE_SETREG8(base, LANE1_ADDITION, 0x05);
		TSTCODE_SETREG8(base, LANE2_SETTLE, 0x01);
		TSTCODE_SETREG8(base, LANE2_ADDITION, 0x05);
		TSTCODE_SETREG8(base, LANE3_SETTLE, 0x01);
		TSTCODE_SETREG8(base, LANE3_ADDITION, 0x05);
	}

	/* setup time and hold time for dphy v1.2 */
	TSTCODE_SETREG8(base, LANE0_DESKEW_1, 0x0D);
	TSTCODE_SETREG8(base, LANE1_DESKEW_1, 0x0D);
	TSTCODE_SETREG8(base, LANE2_DESKEW_1, 0x0D);
	TSTCODE_SETREG8(base, LANE3_DESKEW_1, 0x0D);
	TSTCODE_SETREG8(base, LANE0_DESKEW_3, 0x03);
	TSTCODE_SETREG8(base, LANE1_DESKEW_3, 0x03);
	TSTCODE_SETREG8(base, LANE2_DESKEW_3, 0x03);
	TSTCODE_SETREG8(base, LANE3_DESKEW_3, 0x03);

	TSTCODE_SETREG8(base, CFG_CLK_ATTR, 0x50);

	TSTCODE_GETREG8(base, LANE0_SETTLE, value);
	ISP_DEBUG("### LANE0_SETTLE = %d", value);

	TSTCODE_GETREG8(base, LANE1_SETTLE, value);
	ISP_DEBUG("### LANE1_SETTLE = %d", value);

	TSTCODE_GETREG8(base, LANE2_SETTLE, value);
	ISP_DEBUG("### LANE2_SETTLE = %d", value);

	TSTCODE_GETREG8(base, LANE3_SETTLE, value);
	ISP_DEBUG("### LANE3_SETTLE = %d", value);

	TSTCODE_GETREG8(base, CFG_CLK_DETECT, value);
	ISP_DEBUG("### CFG_CLK_DETECT = 0x%x", value);

	TSTCODE_GETREG8(base, CFG_CLK_ATTR, value);
	ISP_DEBUG("### CFG_CLK_ATTR = 0x%x", value);

	return 0;
}

int csi2if_enable(char __iomem *base, unsigned char num_lanes, unsigned char settle_time)
{
	unsigned int   phy_rx;
	unsigned int   phy_state;
	char __iomem *base_addr = base;

	if (num_lanes > 4 || 0 == num_lanes) {
		ISP_ERR("number of lanes %d out of range!!\n", num_lanes);
		return -1;
	}

	/* de-assert the shutdown signal*/
	REG_SET(base_addr + CSI2IF_PHY_SHUTDOWNZ_REG, 0);
	REG_SET(base_addr + CSI2IF_DPHY_RSTZ_REG, 0);
	REG_SET(base_addr + CSI2IF_CSI2_RESETN_REG, 0);

	REG_SET(base_addr + CSI2IF_PHY_TEST_CTRL0_REG, 1);
	REG_SET(base_addr + CSI2IF_PHY_TEST_CTRL0_REG, 0);
	mdelay(1);

	REG_SET(base_addr + CSI2IF_PHY_SHUTDOWNZ_REG, 1);
	REG_SET(base_addr + CSI2IF_N_LANES_REG, (num_lanes-1));

	REG_SET(base_addr + CSI2IF_DPHY_RSTZ_REG, 1);
	REG_SET(base_addr + CSI2IF_CSI2_RESETN_REG, 1);

	/* Configure
	 * HUAWEI D-PHY
	 * */
	if (csi2if_dphy_init(base_addr, settle_time) < 0) {
		return -1;
	}

	/* *confirm
	 * the D-PHY
	 * is in
	 * right
	 * state*/
	phy_rx    = REG_GET(base_addr + CSI2IF_PHY_RX_REG);
	phy_state = REG_GET(base_addr + CSI2IF_PHY_STOPSTATE_REG);

	if ((0 == REG_GET_FIELD(phy_state, CSI2IF_PHY_STOPSTATEDATA_0))
			&& (0 == REG_GET_FIELD(phy_state, CSI2IF_PHY_STOPSTATECLK))) {
		ISP_INFO("### not all data and clock lanes in stop state! phy_rx = 0x%x, phy_state = %x\n", phy_rx, phy_state);
	}

	if (0 == REG_GET_FIELD(phy_state, CSI2IF_PHY_RXCLKACTIVEHS)) {
		ISP_INFO("### D-PHY is not receiving a clock!\n");
	}

	mdelay(1);

	phy_rx    = REG_GET(base_addr + CSI2IF_PHY_RX_REG);
	phy_state = REG_GET(base_addr + CSI2IF_PHY_STOPSTATE_REG);
	ISP_INFO("### D-PHY state: phy_rx = 0x%x, phy_state = 0x%x\n", phy_rx, phy_state);

	return 0;
}

int csi2if_disable(char __iomem *base)
{
	REG_SET(base + CSI2IF_CSI2_RESETN_REG, 0);
	REG_SET(base + CSI2IF_PHY_SHUTDOWNZ_REG, 0);

	return 0;
}

int isp_csi_get_reg_offset(int csi_index)
{
	switch (csi_index) {
	case 0:
		return ISP_BASE_ADDR_CSI2IF_A;
	case 1:
		return ISP_BASE_ADDR_CSI2IF_B;
	case 2:
		return ISP_BASE_ADDR_CSI2IF_C;
	default:
		ISP_ERR("csi_index=%d error\n", csi_index);
		return -1;
	}
}

int isp_csi_enable(struct isp_device *dev, unsigned char num_lanes, unsigned char settle_time)
{
	csi2if_enable(dev->base + isp_csi_get_reg_offset(dev->csi_index), num_lanes, settle_time);
	return 0;
}

int isp_csi_disable(struct isp_device *dev)
{
	csi2if_disable(dev->base + isp_csi_get_reg_offset(dev->csi_index));
	return 0;
}
