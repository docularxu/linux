/*
 * Device driver for regulators in hi655x IC
 *
 * Copyright (c) 2015 Hisilicon.
 *
 * Chen Feng <puck.chen@hisilicon.com>
 * Fei  Wang <w.f@huawei.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_address.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/regulator/hi655x-regulator.h>
#include <linux/mfd/hi655x-pmic.h>
#include <linux/regmap.h>
#include <linux/bitops.h>
#include <linux/string.h>

static int hi655x_is_enabled(struct regulator_dev *rdev)
{
	unsigned int value = 0;

	struct hi655x_regulator *regulator = rdev_get_drvdata(rdev);
	struct hi655x_regulator_ctrl_regs *ctrl_regs = &regulator->ctrl_regs;

	regmap_read(rdev->regmap, ctrl_regs->status_reg, &value);
	return (value & BIT(regulator->ctrl_mask));
}

static int hi655x_disable(struct regulator_dev *rdev)
{
	int ret = 0;

	struct hi655x_regulator *regulator = rdev_get_drvdata(rdev);
	struct hi655x_regulator_ctrl_regs *ctrl_regs = &regulator->ctrl_regs;

	ret = regmap_write(rdev->regmap, ctrl_regs->disable_reg,
			   BIT(regulator->ctrl_mask));
	return ret;
}

static int hi655x_get_voltage(struct regulator_dev *rdev)
{
	unsigned int value = 0;

	struct hi655x_regulator *regulator = rdev_get_drvdata(rdev);

	regmap_read(rdev->regmap, regulator->vset_reg, &value);
	value &= regulator->vset_mask;

	return regulator->rdesc.volt_table[value];
}

static int hi655x_set_voltage(struct regulator_dev *rdev,
				int min_uV, int max_uV, unsigned *selector)
{
	int i = 0;
	int vol = 0;

	struct hi655x_regulator *regulator = rdev_get_drvdata(rdev);

	/**
	 * search the matched vol and get its index
	 */
	for (i = 0; i < regulator->rdesc.n_voltages; i++) {
		vol = regulator->rdesc.volt_table[i];
		if ((vol >= min_uV) && (vol <= max_uV))
			break;
	}

	if (i == regulator->rdesc.n_voltages)
		return -EINVAL;

	regmap_update_bits(rdev->regmap, regulator->vset_reg,
			   regulator->vset_mask, i);

	*selector = i;

	return 0;
}

static struct regulator_ops hi655x_regulator_ops = {
	.enable = regulator_enable_regmap,
	.disable = hi655x_disable,
	.is_enabled = hi655x_is_enabled,
	.list_voltage = regulator_list_voltage_table,
	.get_voltage = hi655x_get_voltage,
	.set_voltage = hi655x_set_voltage,
};

static const struct of_device_id of_hi655x_regulator_match_tbl[] = {
	{
		.compatible = "hisilicon,hi655x-regulator-pmic",
	},
};
MODULE_DEVICE_TABLE(of, of_hi655x_regulator_match_tbl);

/**
 * get the hi655x specific data from dt node.
 */
static int of_get_hi655x_ctr(struct hi655x_regulator *regulator,
			     struct device *dev, struct device_node *np)
{
	int ret;
	u32 *vset_table;
	u32 vset_reg;
	u32 ctrl_mask;
	u32 vset_mask;
	u32 vol_numb;
	struct hi655x_regulator_ctrl_regs ctrl_reg;
	struct hi655x_regulator_ctrl_regs *t_ctrl;

	ret = of_property_read_u32_array(np, "regulator-ctrl-regs",
					 (u32 *)&ctrl_reg, 0x3);
	if (!ret) {
		t_ctrl = &regulator->ctrl_regs;
		t_ctrl->enable_reg = HI655X_BUS_ADDR(ctrl_reg.enable_reg);
		t_ctrl->disable_reg = HI655X_BUS_ADDR(ctrl_reg.disable_reg);
		t_ctrl->status_reg = HI655X_BUS_ADDR(ctrl_reg.status_reg);
	} else
		goto error;

	ret = of_property_read_u32(np, "regulator-ctrl-mask", &ctrl_mask);
	if (!ret)
		regulator->ctrl_mask = ctrl_mask;
	else
		goto error;

	regulator->rdesc.enable_reg = t_ctrl->enable_reg;
	regulator->rdesc.enable_val = ctrl_mask;
	regulator->rdesc.enable_mask = ctrl_mask;

	ret = of_property_read_u32(np, "regulator-vset-regs", &vset_reg);
	if (!ret)
		regulator->vset_reg = HI655X_BUS_ADDR(vset_reg);
	else
		goto error;

	ret = of_property_read_u32(np, "regulator-vset-mask", &vset_mask);
	if (!ret)
		regulator->vset_mask = vset_mask;
	else
		goto error;

	ret = of_property_read_u32(np, "regulator-n-vol", &vol_numb);
	if (!ret)
		regulator->rdesc.n_voltages = vol_numb;
	else
		goto error;

	vset_table = devm_kzalloc(dev, vol_numb * sizeof(int), GFP_KERNEL);
	if (!vset_table)
		return -ENOMEM;

	ret = of_property_read_u32_array(np, "regulator-vset-table",
					 vset_table,
					 vol_numb);

	if (!ret)
		regulator->rdesc.volt_table = vset_table;

	return 0;
error:
	dev_err(dev, "get from dts node error!\n");
	return -ENODEV;
}

static int hi655x_regulator_probe(struct platform_device *pdev)
{
	int ret;
	struct hi655x_regulator *regulator;
	struct hi655x_pmic *pmic;
	struct regulator_init_data *init_data;
	struct regulator_config config = { };
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;

	pmic = dev_get_drvdata(pdev->dev.parent);
	if (!pmic) {
		dev_err(dev, "no pmic in the regulator parent node\n");
		return -ENODEV;
	}

	regulator = devm_kzalloc(dev, sizeof(*regulator), GFP_KERNEL);
	if (!regulator)
		return -ENOMEM;

	regulator->rdesc.type = REGULATOR_VOLTAGE;
	regulator->rdesc.owner = THIS_MODULE;
	regulator->rdesc.ops = &hi655x_regulator_ops;
	init_data = of_get_regulator_init_data(dev, dev->of_node,
					       &regulator->rdesc);
	if (!init_data)
		return -EINVAL;

	config.dev = &pdev->dev;
	config.init_data = init_data;
	config.driver_data = regulator;
	config.regmap = pmic->regmap;
	config.of_node = pdev->dev.of_node;
	regulator->rdesc.name = init_data->constraints.name;

	ret = of_get_hi655x_ctr(regulator, dev, np);
	if (ret) {
		dev_err(dev, "get param from dts error!\n");
		return ret;
	}

	regulator->regdev = devm_regulator_register(dev,
						    &regulator->rdesc,
						    &config);
	if (IS_ERR(regulator->regdev))
		return PTR_ERR(regulator->regdev);

	platform_set_drvdata(pdev, regulator);

	return 0;
}

static struct platform_driver hi655x_regulator_driver = {
	.driver = {
		.name	= "hi655x_regulator",
		.of_match_table = of_hi655x_regulator_match_tbl,
	},
	.probe	= hi655x_regulator_probe,
};
module_platform_driver(hi655x_regulator_driver);

MODULE_AUTHOR("Chen Feng <puck.chen@hisilicon.com>");
MODULE_DESCRIPTION("Hisi hi655x PMIC driver");
MODULE_LICENSE("GPL v2");
