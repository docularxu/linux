/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2024 Troy Mitchell <troymitchell988@gmail.com>
 */
#ifndef __SPACEMIT_P1_H__
#define __SPACEMIT_P1_H__

enum p1_reg {
	P1_ID_DCDC1,
	P1_ID_DCDC2,
	P1_ID_DCDC3,
	P1_ID_DCDC4,
	P1_ID_DCDC5,
	P1_ID_DCDC6,
	P1_ID_LDO1,
	P1_ID_LDO2,
	P1_ID_LDO3,
	P1_ID_LDO4,
	P1_ID_LDO5,
	P1_ID_LDO6,
	P1_ID_LDO7,
	P1_ID_LDO8,
	P1_ID_LDO9,
	P1_ID_LDO10,
	P1_ID_LDO11,
	P1_ID_SWITCH1,
};

enum p1_irq_line {
	/* reg: 0x91 */
	P1_E_GPI0,
	P1_E_GPI1,
	P1_E_GPI2,
	P1_E_GPI3,
	P1_E_GPI4,
	P1_E_GPI5,

	/* reg: 0x92 */
	P1_E_ADC_TEMP,
	P1_E_ADC_EOC,
	P1_E_ADC_EOS,
	P1_E_WDT_TO,
	P1_E_ALARM,
	P1_E_TICK,

	/* reg: 0x93 */
	P1_E_LDO_OV,
	P1_E_LDO_UV,
	P1_E_LDO_SC,
	P1_E_SW_SC,
	P1_E_TEMP_WARN,
	P1_E_TEMP_SEVERE,
	P1_E_TEMP_CRIT,

	/* reg: 0x94 */
	P1_E_BUCK1_OV,
	P1_E_BUCK2_OV,
	P1_E_BUCK3_OV,
	P1_E_BUCK4_OV,
	P1_E_BUCK5_OV,
	P1_E_BUCK6_OV,

	/* reg: 0x95 */
	P1_E_BUCK1_UV,
	P1_E_BUCK2_UV,
	P1_E_BUCK3_UV,
	P1_E_BUCK4_UV,
	P1_E_BUCK5_UV,
	P1_E_BUCK6_UV,

	/* reg: 0x96 */
	P1_E_BUCK1_SC,
	P1_E_BUCK2_SC,
	P1_E_BUCK3_SC,
	P1_E_BUCK4_SC,
	P1_E_BUCK5_SC,
	P1_E_BUCK6_SC,

	/* reg: 0x97 */
	P1_E_PWRON_RINTR,
	P1_E_PWRON_FINTR,
	P1_E_PWRON_SINTR,
	P1_E_PWRON_LINTR,
	P1_E_PWRON_SDINTR,
	P1_E_VSYS_OV,
};

#define P1_MAX_REG			0xA8

#define P1_BUCK_VSEL_MASK		0xff
#define P1_BUCK_EN_MASK			0x1

#define P1_BUCK1_CTRL_REG		0x47
#define P1_BUCK2_CTRL_REG		0x4a
#define P1_BUCK3_CTRL_REG		0x4d
#define P1_BUCK4_CTRL_REG		0x50
#define P1_BUCK5_CTRL_REG		0x53
#define P1_BUCK6_CTRL_REG		0x56

#define P1_BUCK1_VSEL_REG		0x48
#define P1_BUCK2_VSEL_REG		0x4b
#define P1_BUCK3_VSEL_REG		0x4e
#define P1_BUCK4_VSEL_REG		0x51
#define P1_BUCK5_VSEL_REG		0x54
#define P1_BUCK6_VSEL_REG		0x57

#define P1_ALDO1_CTRL_REG		0x5b
#define P1_ALDO2_CTRL_REG		0x5e
#define P1_ALDO3_CTRL_REG		0x61
#define P1_ALDO4_CTRL_REG		0x64

#define P1_ALDO1_VOLT_REG		0x5c
#define P1_ALDO2_VOLT_REG		0x5f
#define P1_ALDO3_VOLT_REG		0x62
#define P1_ALDO4_VOLT_REG		0x65

#define P1_ALDO_EN_MASK			0x1
#define P1_ALDO_VSEL_MASK		0x7f

#define P1_DLDO1_CTRL_REG		0x67
#define P1_DLDO2_CTRL_REG		0x6a
#define P1_DLDO3_CTRL_REG		0x6d
#define P1_DLDO4_CTRL_REG		0x70
#define P1_DLDO5_CTRL_REG		0x73
#define P1_DLDO6_CTRL_REG		0x76
#define P1_DLDO7_CTRL_REG		0x79

#define P1_DLDO1_VOLT_REG		0x68
#define P1_DLDO2_VOLT_REG		0x6b
#define P1_DLDO3_VOLT_REG		0x6e
#define P1_DLDO4_VOLT_REG		0x71
#define P1_DLDO5_VOLT_REG		0x74
#define P1_DLDO6_VOLT_REG		0x77
#define P1_DLDO7_VOLT_REG		0x7a

#define P1_DLDO_EN_MASK			0x1
#define P1_DLDO_VSEL_MASK		0x7f

#define P1_SWITCH_CTRL_REG		0x59
#define P1_SWTICH_EN_MASK		0x1

#define P1_PWR_CTRL2			0x7e
#define P1_SW_SHUTDOWN_BIT_MSK		0x4
#define P1_SW_RESET_BIT_MSK		0x2

#define P1_E_GPI0_MSK			BIT(0)
#define P1_E_GPI1_MSK			BIT(1)
#define P1_E_GPI2_MSK			BIT(2)
#define P1_E_GPI3_MSK			BIT(3)
#define P1_E_GPI4_MSK			BIT(4)
#define P1_E_GPI5_MSK			BIT(5)

#define P1_E_ADC_TEMP_MSK		BIT(0)
#define P1_E_ADC_EOC_MSK		BIT(1)
#define P1_E_ADC_EOS_MSK		BIT(2)
#define P1_E_WDT_TO_MSK			BIT(3)
#define P1_E_ALARM_MSK			BIT(4)
#define P1_E_TICK_MSK			BIT(5)

#define P1_E_LDO_OV_MSK			BIT(0)
#define P1_E_LDO_UV_MSK			BIT(1)
#define P1_E_LDO_SC_MSK			BIT(2)
#define P1_E_SW_SC_MSK			BIT(3)
#define P1_E_TEMP_WARN_MSK		BIT(4)
#define P1_E_TEMP_SEVERE_MSK		BIT(5)
#define P1_E_TEMP_CRIT_MSK		BIT(6)

#define P1_E_BUCK1_OV_MSK		BIT(0)
#define P1_E_BUCK2_OV_MSK		BIT(1)
#define P1_E_BUCK3_OV_MSK		BIT(2)
#define P1_E_BUCK4_OV_MSK		BIT(3)
#define P1_E_BUCK5_OV_MSK		BIT(4)
#define P1_E_BUCK6_OV_MSK		BIT(5)

#define P1_E_BUCK1_UV_MSK		BIT(0)
#define P1_E_BUCK2_UV_MSK		BIT(1)
#define P1_E_BUCK3_UV_MSK		BIT(2)
#define P1_E_BUCK4_UV_MSK		BIT(3)
#define P1_E_BUCK5_UV_MSK		BIT(4)
#define P1_E_BUCK6_UV_MSK		BIT(5)

#define P1_E_BUCK1_SC_MSK		BIT(0)
#define P1_E_BUCK2_SC_MSK		BIT(1)
#define P1_E_BUCK3_SC_MSK		BIT(2)
#define P1_E_BUCK4_SC_MSK		BIT(3)
#define P1_E_BUCK5_SC_MSK		BIT(4)
#define P1_E_BUCK6_SC_MSK		BIT(5)

#define P1_E_PWRON_RINTR_MSK		BIT(0)
#define P1_E_PWRON_FINTR_MSK		BIT(1)
#define P1_E_PWRON_SINTR_MSK		BIT(2)
#define P1_E_PWRON_LINTR_MSK		BIT(3)
#define P1_E_PWRON_SDINTR_MSK		BIT(4)
#define P1_E_VSYS_OV_MSK		BIT(5)

#define P1_E_STATUS_REG_BASE		0x91
#define P1_E_EN_REG_BASE		0x98

#define P1_REGMAP_CONFIG	\
	static const struct regmap_config p1_regmap_config = {	\
		.reg_bits = 8,					\
		.val_bits = 8,					\
		.max_register = P1_MAX_REG,			\
		.cache_type = REGCACHE_RBTREE,			\
	}

#define P1_IRQS_DESC					\
static const struct regmap_irq p1_irqs[] = {		\
	[P1_E_GPI0] = {					\
		.mask = P1_E_GPI0_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_GPI1] = {					\
		.mask = P1_E_GPI1_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_GPI2] = {					\
		.mask = P1_E_GPI2_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_GPI3] = {					\
		.mask = P1_E_GPI3_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_GPI4] = {					\
		.mask = P1_E_GPI4_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_GPI5] = {					\
		.mask = P1_E_GPI5_MSK,			\
		.reg_offset = 0,			\
	},						\
							\
	[P1_E_ADC_TEMP] = {				\
		.mask = P1_E_ADC_TEMP_MSK,		\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_ADC_EOC] = {				\
		.mask = P1_E_ADC_EOC_MSK,		\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_ADC_EOS] = {				\
		.mask = P1_E_ADC_EOS_MSK,		\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_WDT_TO] = {				\
		.mask = P1_E_WDT_TO_MSK,		\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_ALARM] = {				\
		.mask = P1_E_ALARM_MSK,			\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_TICK] = {					\
		.mask = P1_E_TICK_MSK,			\
		.reg_offset = 1,			\
	},						\
							\
	[P1_E_LDO_OV] = {				\
		.mask = P1_E_LDO_OV_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_LDO_UV] = {				\
		.mask = P1_E_LDO_UV_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_LDO_SC] = {				\
		.mask = P1_E_LDO_SC_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_SW_SC] = {				\
		.mask = P1_E_SW_SC_MSK,			\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_TEMP_WARN] = {				\
		.mask = P1_E_TEMP_WARN_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_TEMP_SEVERE] = {				\
		.mask = P1_E_TEMP_SEVERE_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_TEMP_CRIT] = {				\
		.mask = P1_E_TEMP_CRIT_MSK,		\
		.reg_offset = 2,			\
	},						\
							\
	[P1_E_BUCK1_OV] = {				\
		.mask = P1_E_BUCK1_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK2_OV] = {				\
		.mask = P1_E_BUCK2_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK3_OV] = {				\
		.mask = P1_E_BUCK3_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK4_OV] = {				\
		.mask = P1_E_BUCK4_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK5_OV] = {				\
		.mask = P1_E_BUCK5_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK6_OV] = {				\
		.mask = P1_E_BUCK6_OV_MSK,		\
		.reg_offset = 3,			\
	},						\
							\
	[P1_E_BUCK1_UV] = {				\
		.mask = P1_E_BUCK1_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK2_UV] = {				\
		.mask = P1_E_BUCK2_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK3_UV] = {				\
		.mask = P1_E_BUCK3_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK4_UV] = {				\
		.mask = P1_E_BUCK4_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK5_UV] = {				\
		.mask = P1_E_BUCK5_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK6_UV] = {				\
		.mask = P1_E_BUCK6_UV_MSK,		\
		.reg_offset = 4,			\
	},						\
							\
	[P1_E_BUCK1_SC] = {				\
		.mask = P1_E_BUCK1_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_BUCK2_SC] = {				\
		.mask = P1_E_BUCK2_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_BUCK3_SC] = {				\
		.mask = P1_E_BUCK3_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_BUCK4_SC] = {				\
		.mask = P1_E_BUCK4_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_BUCK5_SC] = {				\
		.mask = P1_E_BUCK5_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_BUCK6_SC] = {				\
		.mask = P1_E_BUCK6_SC_MSK,		\
		.reg_offset = 5,			\
	},						\
							\
	[P1_E_PWRON_RINTR] = {				\
		.mask = P1_E_PWRON_RINTR_MSK,		\
		.reg_offset = 6,			\
	},						\
							\
	[P1_E_PWRON_FINTR] = {				\
		.mask = P1_E_PWRON_FINTR_MSK,		\
		.reg_offset = 6,			\
	},						\
							\
	[P1_E_PWRON_SINTR] = {				\
		.mask = P1_E_PWRON_SINTR_MSK,		\
		.reg_offset = 6,			\
	},						\
							\
	[P1_E_PWRON_LINTR] = {				\
		.mask = P1_E_PWRON_LINTR_MSK,		\
		.reg_offset = 6,			\
	},						\
							\
	[P1_E_PWRON_SDINTR] = {				\
		.mask = P1_E_PWRON_SDINTR_MSK,		\
		.reg_offset = 6,			\
	},						\
							\
	[P1_E_VSYS_OV] = {				\
		.mask = P1_E_VSYS_OV_MSK,		\
		.reg_offset = 6,			\
	},						\
}

#define P1_IRQ_CHIP_DESC				\
static const struct regmap_irq_chip p1_irq_chip = {	\
	.name			= "p1",			\
	.irqs			= p1_irqs,		\
	.num_irqs		= ARRAY_SIZE(p1_irqs),	\
	.num_regs		= 7,			\
	.status_base		= P1_E_STATUS_REG_BASE,	\
	.mask_base		= P1_E_EN_REG_BASE,	\
	.ack_base		= P1_E_STATUS_REG_BASE,	\
	.init_ack_masked	= true,			\
}

#define P1_POWER_KEY_RESOURCES_DESC			\
static const struct resource p1_pwrkey_resources[] = {	\
	DEFINE_RES_IRQ(P1_E_PWRON_RINTR),		\
	DEFINE_RES_IRQ(P1_E_PWRON_FINTR),		\
	DEFINE_RES_IRQ(P1_E_PWRON_SINTR),		\
	DEFINE_RES_IRQ(P1_E_PWRON_LINTR),		\
}

#define P1_RTC_RESOURCES_DESC				\
static const struct resource p1_rtc_resources[] = {	\
	DEFINE_RES_IRQ(P1_E_ALARM),			\
}

#define P1_MFD_CELL	\
	static const struct mfd_cell p1[] = {					\
		{								\
			.name = "spacemit-regulator@p1",			\
			.of_compatible = "pmic,p1-regulator",			\
		},								\
		{								\
			.name = "spacemit-pinctrl@p1",				\
			.of_compatible = "pmic,p1-pinctrl",			\
		},								\
		{								\
			.name = "spacemit-pwrkey@p1",				\
			.of_compatible = "pmic,p1-pwrkey",			\
			.num_resources = ARRAY_SIZE(p1_pwrkey_resources),	\
			.resources = &p1_pwrkey_resources[0],			\
		},								\
		{								\
			.name = "spacemit-rtc@p1",				\
			.of_compatible = "pmic,p1-rtc",				\
			.num_resources = ARRAY_SIZE(p1_rtc_resources),		\
			.resources = &p1_rtc_resources[0],			\
		},								\
	}

#define P1_MFD_MATCH_DATA							\
static struct spacemit_pmic_match_data pmic_p1_match_data = {			\
	.regmap_cfg = &p1_regmap_config,					\
	.regmap_irq_chip = &p1_irq_chip,					\
	.mfd_cells = p1,							\
	.nr_cells = ARRAY_SIZE(p1),						\
	.name = "p1",								\
	.shutdown = {								\
		.reg = P1_PWR_CTRL2,						\
		.bit = P1_SW_SHUTDOWN_BIT_MSK,					\
	},									\
	.reboot = {								\
		.reg = P1_PWR_CTRL2,						\
		.bit = P1_SW_RESET_BIT_MSK,					\
	},									\
}

#endif // __SPACEMIT_P1_H__
