// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2024 Troy Mitchell <troymitchell988@gmail.com>
 */

#include <linux/i2c.h>
#include <linux/mfd/core.h>
#include <linux/mfd/spacemit/spacemit-pmic.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/pm_wakeirq.h>
#include <linux/reboot.h>

P1_REGMAP_CONFIG;
P1_IRQS_DESC;
P1_IRQ_CHIP_DESC;
P1_POWER_KEY_RESOURCES_DESC;
P1_RTC_RESOURCES_DESC;
P1_MFD_CELL;
P1_MFD_MATCH_DATA;

static int spacemit_pmic_shutdown(struct sys_off_data *data)
{
	int ret;
	struct spacemit_pmic *pmic = data->cb_data;

	ret = regmap_update_bits(pmic->regmap,
				 pmic->match_data->shutdown.reg,
				 pmic->match_data->shutdown.bit,
				 pmic->match_data->shutdown.bit);
	if (ret)
		dev_err(data->dev, "failed to reboot device!");

	return NOTIFY_DONE;
}

static int spacemit_pmic_restart(struct sys_off_data *data)
{
	int ret;
	struct spacemit_pmic *pmic = data->cb_data;

	ret = regmap_update_bits(pmic->regmap,
				 pmic->match_data->reboot.reg,
				 pmic->match_data->reboot.bit,
				 pmic->match_data->reboot.bit);
	if (ret)
		dev_err(data->dev, "failed to reboot device!");

	return NOTIFY_DONE;
}

static int spacemit_pmic_probe(struct i2c_client *client)
{
	const struct spacemit_pmic_match_data *match_data;
	const struct mfd_cell *cells;
	struct spacemit_pmic *pmic;
	int nr_cells, ret;

	if (!client->irq)
		return dev_err_probe(&client->dev, -ENXIO, "no interrupt supported");

	match_data = of_device_get_match_data(&client->dev);
	if (WARN_ON(!match_data))
		return -EINVAL;

	pmic = devm_kzalloc(&client->dev, sizeof(*pmic), GFP_KERNEL);
	if (!pmic)
		return -ENOMEM;

	cells = match_data->mfd_cells;
	nr_cells = match_data->nr_cells;

	pmic->regmap_cfg = match_data->regmap_cfg;
	pmic->regmap_irq_chip = match_data->regmap_irq_chip;
	pmic->i2c = client;
	pmic->match_data = match_data;
	pmic->regmap = devm_regmap_init_i2c(client, pmic->regmap_cfg);
	if (IS_ERR(pmic->regmap))
		return dev_err_probe(&client->dev,
				     PTR_ERR(pmic->regmap),
				     "regmap initialization failed");

	regcache_cache_bypass(pmic->regmap, true);

	i2c_set_clientdata(client, pmic);

	if (pmic->regmap_irq_chip) {
		ret = regmap_add_irq_chip(pmic->regmap, client->irq, IRQF_ONESHOT, -1,
						pmic->regmap_irq_chip, &pmic->irq_data);
		if (ret)
			return dev_err_probe(&client->dev, ret, "failed to add irqchip");
	}

	dev_pm_set_wake_irq(&client->dev, client->irq);
	device_init_wakeup(&client->dev, true);

	ret = devm_mfd_add_devices(&client->dev, PLATFORM_DEVID_NONE,
				   cells, nr_cells, NULL, 0,
				   regmap_irq_get_domain(pmic->irq_data));
	if (ret)
		return dev_err_probe(&client->dev, ret, "failed to add MFD devices");

	if (match_data->shutdown.reg) {
		ret = devm_register_sys_off_handler(&client->dev,
						    SYS_OFF_MODE_POWER_OFF_PREPARE,
						    SYS_OFF_PRIO_HIGH,
						    &spacemit_pmic_shutdown,
						    pmic);
		if (ret)
			return dev_err_probe(&client->dev,
					     ret,
					     "failed to register restart handler");

	}

	if (match_data->reboot.reg) {
		ret = devm_register_sys_off_handler(&client->dev,
						    SYS_OFF_MODE_RESTART,
						    SYS_OFF_PRIO_HIGH,
						    &spacemit_pmic_restart,
						    pmic);
		if (ret)
			return dev_err_probe(&client->dev,
					     ret,
					     "failed to register restart handler");
	}

	return 0;
}

static const struct of_device_id spacemit_pmic_of_match[] = {
	{ .compatible = "spacemit,p1", .data = &pmic_p1_match_data },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, spacemit_pmic_of_match);

static struct i2c_driver spacemit_pmic_i2c_driver = {
	.driver = {
		.name = "spacemit-pmic",
		.of_match_table = spacemit_pmic_of_match,
	},
	.probe    = spacemit_pmic_probe,
};

static int __init spacemit_pmic_init(void)
{
	return i2c_add_driver(&spacemit_pmic_i2c_driver);
}

static void __exit spacemit_pmic_exit(void)
{
	i2c_del_driver(&spacemit_pmic_i2c_driver);
}

module_init(spacemit_pmic_init);
module_exit(spacemit_pmic_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("mfd core driver for the SpacemiT PMIC");
