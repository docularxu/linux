/*
 * Device driver for PMIC DRIVER in HI655X IC
 *
 * Copyright (c) 2015 Hisilicon Co. Ltd
 *
 * Chen Feng <puck.chen@hisilicon.com>
 * Fei Wang  <w.f@huawei.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/err.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of_gpio.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/mfd/hi655x-pmic.h>
#include <linux/regmap.h>

static const struct of_device_id of_hi655x_pmic_child_match_tbl[] = {
	{ .compatible = "hisilicon,hi655x-regulator-pmic", },
	{ .compatible = "hisilicon,hi655x-powerkey", },
	{ .compatible = "hisilicon,hi655x-usbvbus", },
	{ .compatible = "hisilicon,hi655x-coul", },
	{ .compatible = "hisilicon,hi655x-pmu-rtc", },
	{},
};

static const struct of_device_id of_hi655x_pmic_match_tbl[] = {
	{ .compatible = "hisilicon,hi655x-pmic-driver", },
	{},
};

static const struct regmap_irq hi655x_irqs[] = {
	{ .reg_offset = 0, .mask = OTMP_D1R_INT },
	{ .reg_offset = 0, .mask = VSYS_2P5_R_INT },
	{ .reg_offset = 0, .mask = VSYS_UV_D3R_INT },
	{ .reg_offset = 0, .mask = VSYS_6P0_D200UR_INT },
	{ .reg_offset = 0, .mask = PWRON_D4SR_INT },
	{ .reg_offset = 0, .mask = PWRON_D20F_INT },
	{ .reg_offset = 0, .mask = PWRON_D20R_INT },
	{ .reg_offset = 0, .mask = RESERVE_INT },
};

static const struct regmap_irq_chip hi655x_irq_chip = {
	.name = "hi655x-pmic",
	.irqs = hi655x_irqs,
	.num_regs = 1,
	.num_irqs = ARRAY_SIZE(hi655x_irqs),
	.status_base = HI655X_IRQ_STAT_BASE,
	.mask_base = HI655X_IRQ_MASK_BASE,
};

static unsigned int hi655x_pmic_get_version(struct hi655x_pmic *pmic)
{
	u32 val;

	regmap_read(pmic->regmap,
		    HI655X_BUS_ADDR(HI655X_VER_REG), &val);

	return val;
}

static struct regmap_config hi655x_regmap_config = {
	.reg_bits = 32,
	.reg_stride = HI655X_STRIDE,
	.val_bits = 8,
	.max_register = HI655X_BUS_ADDR(0xFFF),
};

static void hi655x_local_irq_clear(struct regmap *map)
{
	unsigned int i;

	regmap_write(map, HI655X_ANA_IRQM_BASE, HI655X_IRQ_CLR);
	for (i = 0; i < HI655X_IRQ_ARRAY; i++) {
		regmap_write(map, HI655X_IRQ_STAT_BASE + i * HI655X_STRIDE,
			     HI655X_IRQ_CLR);
	}
}

static int hi655x_pmic_probe(struct platform_device *pdev)
{
	int ret;
	struct hi655x_pmic *pmic;
	struct device_node *gpio_np;

	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	void __iomem *base;
	enum of_gpio_flags gpio_flags;

	pmic = devm_kzalloc(dev, sizeof(*pmic), GFP_KERNEL);
	pmic->dev = dev;

	pmic->res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!pmic->res) {
		dev_err(dev, "platform_get_resource err\n");
		return -ENOENT;
	}
	base = devm_ioremap_resource(&pdev->dev, pmic->res);
	if (!base) {
		dev_err(dev, "cannot map register memory\n");
		return -ENOMEM;
	}
	pmic->regmap = devm_regmap_init_mmio_clk(dev, NULL, base,
							&hi655x_regmap_config);

	pmic->ver = hi655x_pmic_get_version(pmic);
	if ((pmic->ver < PMU_VER_START) || (pmic->ver > PMU_VER_END)) {
		dev_warn(dev, "it is wrong pmu version\n");
		return -EINVAL;
	}

	hi655x_local_irq_clear(pmic->regmap);

	gpio_np = of_parse_phandle(np, "pmic-gpios", 0);
	if (!gpio_np) {
		dev_err(dev, "can't parse property\n");
		return -ENOENT;
	}

	pmic->gpio = of_get_gpio_flags(gpio_np, 0, &gpio_flags);
	if (!gpio_is_valid(pmic->gpio)) {
		dev_err(dev, "it is invalid gpio %d\n", pmic->gpio);
		return -EINVAL;
	}

	ret = devm_gpio_request_one(&pdev->dev,
			pmic->gpio, GPIOF_IN, "hi655x_pmic_irq");
	if (ret) {
		dev_err(dev, "failed to request gpio %d  ret = %d\n",
			pmic->gpio, ret);
		return ret;
	}
	pmic->irq = gpio_to_irq(pmic->gpio);

	ret = regmap_add_irq_chip(pmic->regmap, pmic->irq,
				  IRQF_TRIGGER_LOW | IRQF_NO_SUSPEND, 0,
				  &hi655x_irq_chip, &pmic->irq_data);
	if (ret) {
		gpio_free(pmic->gpio);
		return ret;
	}

	platform_set_drvdata(pdev, pmic);

	/**
	 * populate sub nodes
	 */
	ret = of_platform_populate(np, of_hi655x_pmic_child_match_tbl,
				   NULL, dev);
	if (ret) {
		gpio_free(pmic->gpio);
		regmap_del_irq_chip(pmic->irq, pmic->irq_data);
		return ret;
	}

	return 0;
}

static struct platform_driver hi655x_pmic_driver = {
	.driver	= {
		.name =	"hi655x-pmic",
		.owner = THIS_MODULE,
		.of_match_table = of_hi655x_pmic_match_tbl,
	},
	.probe  = hi655x_pmic_probe,
};
module_platform_driver(hi655x_pmic_driver);

MODULE_AUTHOR("Chen Feng <puck.chen@hisilicon.com>");
MODULE_DESCRIPTION("Hisilicon hi655x pmic driver");
MODULE_LICENSE("GPL v2");
