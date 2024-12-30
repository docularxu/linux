/* SPDX-License-Identifier: GPL-2.0-only
 *
 * Copyright (C) 2024 Troy Mitchell <troymitchell988@gmail.com>
 */
#ifndef __SPACEMIT_PMIC_H__
#define __SPACEMIT_PMIC_H__

#include <linux/regulator/machine.h>
#include <linux/regmap.h>
#include <linux/mfd/spacemit/spacemit-p1.h>

struct spacemit_pmic_match_data {
	const struct regmap_config *regmap_cfg;
	const struct regmap_irq_chip *regmap_irq_chip;
	const struct mfd_cell *mfd_cells;
	int nr_cells;
	const char *name;

	struct {
		unsigned char reg;
		unsigned char bit;
	} shutdown;

	struct {
		unsigned int reg;
		unsigned char bit;
	} reboot;
};

struct spacemit_pmic {
	struct i2c_client		*i2c;
	struct regmap_irq_chip_data	*irq_data;
	struct regmap			*regmap;
	const struct regmap_config	*regmap_cfg;
	const struct regmap_irq_chip	*regmap_irq_chip;
	const struct spacemit_pmic_match_data *match_data;
};

#endif // __SPACEMIT_PMIC_H__
