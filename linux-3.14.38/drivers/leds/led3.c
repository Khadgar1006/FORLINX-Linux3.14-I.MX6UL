/*
 * LEDs test driver for GPIOs
 *
 * Copyright (C) 2007 8D Technologies inc.
 * Raphael Assenat <raph@8d.com>
 * Copyright (C) 2008 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/leds.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <linux/err.h>

struct gpio_led_data {
	struct led_classdev cdev;
	unsigned gpio;
	struct work_struct work;
	u8 new_level;
	u8 can_sleep;
	u8 active_low;
	u8 blinking;
	int (*platform_gpio_blink_set)(unsigned gpio, int state,
			unsigned long *delay_on, unsigned long *delay_off);
};

static int gpio_led_probe(struct platform_device *pdev)
{

}

static int gpio_led_remove(struct platform_device *pdev)
{

}


static const struct of_device_id of_gpio_leds_match[] = {
	{ .compatible = "led3", },
	{},
};

static struct platform_driver gpio_led3_driver = {
	.probe		= led3_probe,
	.remove		= led3_remove,
	.driver		= {
		.name	= "led3",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(of_gpio_leds_match),
	},
};

module_platform_driver(gpio_led3_driver);

MODULE_AUTHOR("Khadgar");
MODULE_DESCRIPTION("GPIO LED Test driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:led3");
