/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ipro_ipro7_gpio

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "drv_gpio.h"
#include "drv_glb.h"

LOG_MODULE_REGISTER(gpio_ipro7, CONFIG_GPIO_LOG_LEVEL);

struct gpio_ipro7_config {
	/* gpio_driver_config needs to be first */
	struct gpio_driver_config common;
	uint32_t base;
	uint8_t ngpios;
};

struct gpio_ipro7_data {
	/* gpio_driver_data needs to be first */
	struct gpio_driver_data common;
	/* Driver data */
};

static int gpio_ipro7_pin_configure(const struct device *dev,
				    gpio_pin_t pin,
				    gpio_flags_t flags)
{
	const struct gpio_ipro7_config *config = dev->config;
	GLB_GPIO_Cfg_Type gpioCfg;
	
	if (pin >= config->ngpios) {
		return -EINVAL;
	}

	/* Initialize GPIO configuration structure */
	gpioCfg.gpioPin = pin;
	gpioCfg.gpioFun = 11; // GPIO_FUN_GPIO from BSP
	gpioCfg.drive = 0;
	gpioCfg.smtCtrl = 1;

	if (flags & GPIO_OUTPUT) {
		/* Configure as output */
		gpioCfg.gpioMode = 0x00000001U; // GPIO_MODE_OUTPUT from BSP
		gpioCfg.pullType = 0x00000002U; // GPIO_PULL_NONE from BSP
		
		GLB_GPIO_Init(&gpioCfg);
		
		if (flags & GPIO_OUTPUT_INIT_HIGH) {
			GLB_GPIO_Write(pin, 1);
		} else if (flags & GPIO_OUTPUT_INIT_LOW) {
			GLB_GPIO_Write(pin, 0);
		}
	} else if (flags & GPIO_INPUT) {
		/* Configure as input */
		gpioCfg.gpioMode = 0x00000000U; // GPIO_MODE_INPUT from BSP
		
		if (flags & GPIO_PULL_UP) {
			gpioCfg.pullType = 0x00000000U; // GPIO_PULL_UP from BSP
		} else if (flags & GPIO_PULL_DOWN) {
			gpioCfg.pullType = 0x00000001U; // GPIO_PULL_DOWN from BSP
		} else {
			gpioCfg.pullType = 0x00000002U; // GPIO_PULL_NONE from BSP
		}
		
		GLB_GPIO_Init(&gpioCfg);
	}

	return 0;
}

static int gpio_ipro7_port_get_raw(const struct device *dev,
				   gpio_port_value_t *value)
{
	const struct gpio_ipro7_config *config = dev->config;
	uint32_t port_value = 0;
	
	for (int i = 0; i < config->ngpios; i++) {
		if (GLB_GPIO_Read(i)) {
			port_value |= BIT(i);
		}
	}
	
	*value = port_value;
	return 0;
}

static int gpio_ipro7_port_set_masked_raw(const struct device *dev,
					  gpio_port_pins_t mask,
					  gpio_port_value_t value)
{
	const struct gpio_ipro7_config *config = dev->config;
	
	for (int i = 0; i < config->ngpios; i++) {
		if (mask & BIT(i)) {
			GLB_GPIO_Write(i, (value & BIT(i)) ? 1 : 0);
		}
	}
	
	return 0;
}

static int gpio_ipro7_port_set_bits_raw(const struct device *dev,
					gpio_port_pins_t pins)
{
	const struct gpio_ipro7_config *config = dev->config;
	
	for (int i = 0; i < config->ngpios; i++) {
		if (pins & BIT(i)) {
			GLB_GPIO_Write(i, 1);
		}
	}
	
	return 0;
}

static int gpio_ipro7_port_clear_bits_raw(const struct device *dev,
					  gpio_port_pins_t pins)
{
	const struct gpio_ipro7_config *config = dev->config;
	
	for (int i = 0; i < config->ngpios; i++) {
		if (pins & BIT(i)) {
			GLB_GPIO_Write(i, 0);
		}
	}
	
	return 0;
}

static int gpio_ipro7_port_toggle_bits(const struct device *dev,
				       gpio_port_pins_t pins)
{
	const struct gpio_ipro7_config *config = dev->config;
	
	for (int i = 0; i < config->ngpios; i++) {
		if (pins & BIT(i)) {
			uint8_t current = GLB_GPIO_Read(i);
			GLB_GPIO_Write(i, !current);
		}
	}
	
	return 0;
}

static int gpio_ipro7_init(const struct device *dev)
{
	LOG_DBG("Initializing GPIO IPRO7 device %s", dev->name);
	
	/* GPIO hardware initialization is done per pin in pin_configure */
	
	return 0;
}

static const struct gpio_driver_api gpio_ipro7_driver_api = {
	.pin_configure = gpio_ipro7_pin_configure,
	.port_get_raw = gpio_ipro7_port_get_raw,
	.port_set_masked_raw = gpio_ipro7_port_set_masked_raw,
	.port_set_bits_raw = gpio_ipro7_port_set_bits_raw,
	.port_clear_bits_raw = gpio_ipro7_port_clear_bits_raw,
	.port_toggle_bits = gpio_ipro7_port_toggle_bits,
};

#define GPIO_IPRO7_INIT(n)						\
	static const struct gpio_ipro7_config gpio_ipro7_config_##n = {	\
		.common = {						\
			.port_pin_mask = GPIO_PORT_PIN_MASK_FROM_DT_INST(n), \
		},							\
		.base = DT_INST_REG_ADDR(n),				\
		.ngpios = DT_INST_PROP(n, ngpios),			\
	};								\
									\
	static struct gpio_ipro7_data gpio_ipro7_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n, gpio_ipro7_init,			\
			      NULL,					\
			      &gpio_ipro7_data_##n,			\
			      &gpio_ipro7_config_##n,			\
			      PRE_KERNEL_1,				\
			      CONFIG_GPIO_INIT_PRIORITY,		\
			      &gpio_ipro7_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_IPRO7_INIT)
