/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 GPIO driver.
 *
 * Two device-tree instances cover the 42 pads (gpio0: pads 0-31, gpio1:
 * pads 32-41, "pin-offset"). All hardware access goes through the SDK LL
 * via the ipro_hal_gpio_* facade (zephyr/hal_glue/ipro_gpio_glue.c):
 * GLB_GPIO_Init for configuration - the same call the SDK's gpio_set_mode()
 * makes - the GLB level registers for port access, and the GLB per-pin
 * interrupt block (sync mode, pulse/level trigger) behind the single
 * GPIO_INT0 line. Both ports share one ISR that walks the pending pads.
 */

#define DT_DRV_COMPAT intelpro_ipro_gpio

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/irq.h>
#include <zephyr/sys/util.h>
#include <ipro_zephyr/ipro_hal_glue.h>

struct gpio_ipro_config {
	struct gpio_driver_config common;
	uint8_t port;       /* 0: pads 0-31, 1: pads 32-41 */
	uint8_t pin_offset;
	uint8_t ngpios;
};

struct gpio_ipro_data {
	struct gpio_driver_data common;
	sys_slist_t callbacks;
	uint32_t int_enabled;
};

static const struct device *ipro_gpio_ports[2];

static inline uint8_t pad_of(const struct device *dev, gpio_pin_t pin)
{
	const struct gpio_ipro_config *cfg = dev->config;

	return cfg->pin_offset + pin;
}

static int gpio_ipro_pin_configure(const struct device *dev, gpio_pin_t pin, gpio_flags_t flags)
{
	const struct gpio_ipro_config *cfg = dev->config;
	enum ipro_hal_gpio_dir dir = IPRO_HAL_GPIO_DISCONNECTED;
	enum ipro_hal_gpio_pull pull = IPRO_HAL_GPIO_PULL_NONE;
	int init = -1;

	if (pin >= cfg->ngpios) {
		return -EINVAL;
	}
	/* Open-drain / open-source are not offered by the GLB pad cell. */
	if (flags & (GPIO_SINGLE_ENDED | GPIO_LINE_OPEN_DRAIN)) {
		return -ENOTSUP;
	}

	if (flags & GPIO_PULL_UP) {
		pull = IPRO_HAL_GPIO_PULL_UP;
	} else if (flags & GPIO_PULL_DOWN) {
		pull = IPRO_HAL_GPIO_PULL_DOWN;
	}

	if (flags & GPIO_OUTPUT) {
		dir = IPRO_HAL_GPIO_OUTPUT;
		if (flags & GPIO_OUTPUT_INIT_HIGH) {
			init = 1;
		} else if (flags & GPIO_OUTPUT_INIT_LOW) {
			init = 0;
		}
	} else if (flags & GPIO_INPUT) {
		dir = IPRO_HAL_GPIO_INPUT;
	}

	return ipro_hal_gpio_configure(pad_of(dev, pin), dir, pull, init) == 0 ? 0 : -EIO;
}

static int gpio_ipro_port_get_raw(const struct device *dev, gpio_port_value_t *value)
{
	const struct gpio_ipro_config *cfg = dev->config;

	/* port_pin_mask, not BIT_MASK(ngpios): BIT_MASK(32) overflows on RV32. */
	*value = ipro_hal_gpio_in_reg(cfg->port) & cfg->common.port_pin_mask;
	return 0;
}

static int gpio_ipro_port_set_masked_raw(const struct device *dev, gpio_port_pins_t mask,
					 gpio_port_value_t value)
{
	const struct gpio_ipro_config *cfg = dev->config;
	unsigned int key = irq_lock();
	uint32_t reg = ipro_hal_gpio_out_reg(cfg->port);

	reg = (reg & ~mask) | (value & mask);
	ipro_hal_gpio_out_reg_write(cfg->port, reg);
	irq_unlock(key);
	return 0;
}

static int gpio_ipro_port_set_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	return gpio_ipro_port_set_masked_raw(dev, pins, pins);
}

static int gpio_ipro_port_clear_bits_raw(const struct device *dev, gpio_port_pins_t pins)
{
	return gpio_ipro_port_set_masked_raw(dev, pins, 0);
}

static int gpio_ipro_port_toggle_bits(const struct device *dev, gpio_port_pins_t pins)
{
	const struct gpio_ipro_config *cfg = dev->config;
	unsigned int key = irq_lock();

	ipro_hal_gpio_out_reg_write(cfg->port, ipro_hal_gpio_out_reg(cfg->port) ^ pins);
	irq_unlock(key);
	return 0;
}

static int gpio_ipro_pin_interrupt_configure(const struct device *dev, gpio_pin_t pin,
					     enum gpio_int_mode mode, enum gpio_int_trig trig)
{
	const struct gpio_ipro_config *cfg = dev->config;
	struct gpio_ipro_data *data = dev->data;
	uint8_t pad = pad_of(dev, pin);
	enum ipro_hal_gpio_trig t;

	if (pin >= cfg->ngpios) {
		return -EINVAL;
	}

	if (mode == GPIO_INT_MODE_DISABLED) {
		ipro_hal_gpio_int_disable(pad);
		data->int_enabled &= ~BIT(pin);
		return 0;
	}

	if (mode == GPIO_INT_MODE_EDGE) {
		switch (trig) {
		case GPIO_INT_TRIG_LOW:
			t = IPRO_HAL_GPIO_TRIG_FALLING_EDGE;
			break;
		case GPIO_INT_TRIG_HIGH:
			t = IPRO_HAL_GPIO_TRIG_RISING_EDGE;
			break;
		default:
			/* The GLB block has no both-edge trigger. */
			return -ENOTSUP;
		}
	} else {
		switch (trig) {
		case GPIO_INT_TRIG_LOW:
			t = IPRO_HAL_GPIO_TRIG_LOW_LEVEL;
			break;
		case GPIO_INT_TRIG_HIGH:
			t = IPRO_HAL_GPIO_TRIG_HIGH_LEVEL;
			break;
		default:
			return -ENOTSUP;
		}
	}

	data->int_enabled |= BIT(pin);
	ipro_hal_gpio_int_enable(pad, t);
	return 0;
}

static int gpio_ipro_manage_callback(const struct device *dev, struct gpio_callback *callback,
				     bool set)
{
	struct gpio_ipro_data *data = dev->data;

	return gpio_manage_callback(&data->callbacks, callback, set);
}

/*
 * Every pad with its interrupt status set, whether or not this driver armed
 * it. INT0 is a single level line shared by all 42 pads: a status left set on
 * a pad nobody asked for (the ROM leaves the boot strap unmasked; a pad
 * re-muxed to an alternate function that toggles, e.g. the camera MCLK on
 * GPIO41, raises it too) would otherwise re-enter the ISR forever.
 */
static uint32_t gpio_ipro_get_pending_int(const struct device *dev)
{
	const struct gpio_ipro_config *cfg = dev->config;
	uint32_t pending = 0;

	for (uint8_t pin = 0; pin < cfg->ngpios; pin++) {
		if (ipro_hal_gpio_int_pending(cfg->pin_offset + pin)) {
			pending |= BIT(pin);
		}
	}
	return pending;
}

/* Shared GPIO_INT0 line: acknowledge every pending pad, then dispatch. */
static void gpio_ipro_isr(const void *arg)
{
	ARG_UNUSED(arg);

	for (int p = 0; p < ARRAY_SIZE(ipro_gpio_ports); p++) {
		const struct device *dev = ipro_gpio_ports[p];

		if (dev == NULL) {
			continue;
		}

		const struct gpio_ipro_config *cfg = dev->config;
		struct gpio_ipro_data *data = dev->data;
		uint32_t pending = gpio_ipro_get_pending_int(dev);
		uint32_t owned = pending & data->int_enabled;

		for (uint8_t pin = 0; pin < cfg->ngpios; pin++) {
			if (!(pending & BIT(pin))) {
				continue;
			}
			if (owned & BIT(pin)) {
				ipro_hal_gpio_int_clear(cfg->pin_offset + pin);
			} else {
				/* Not ours: mask it so it cannot come back. */
				ipro_hal_gpio_int_disable(cfg->pin_offset + pin);
			}
		}
		if (owned != 0) {
			gpio_fire_callbacks(&data->callbacks, dev, owned);
		}
	}
}

static int gpio_ipro_init(const struct device *dev)
{
	const struct gpio_ipro_config *cfg = dev->config;
	static bool irq_connected;

	ipro_gpio_ports[cfg->port] = dev;

	/* Start from a known state: every pad masked, no stale status (the
	 * ROM / boot2 leave the boot strap and wake pads as they were). */
	for (uint8_t pin = 0; pin < cfg->ngpios; pin++) {
		ipro_hal_gpio_int_disable(cfg->pin_offset + pin);
	}

	if (!irq_connected) {
		irq_connected = true;
		IRQ_CONNECT(DT_INST_IRQN(0), DT_INST_IRQ(0, priority), gpio_ipro_isr, NULL, 0);
		irq_enable(DT_INST_IRQN(0));
	}
	return 0;
}

static DEVICE_API(gpio, gpio_ipro_api) = {
	.pin_configure = gpio_ipro_pin_configure,
	.port_get_raw = gpio_ipro_port_get_raw,
	.port_set_masked_raw = gpio_ipro_port_set_masked_raw,
	.port_set_bits_raw = gpio_ipro_port_set_bits_raw,
	.port_clear_bits_raw = gpio_ipro_port_clear_bits_raw,
	.port_toggle_bits = gpio_ipro_port_toggle_bits,
	.pin_interrupt_configure = gpio_ipro_pin_interrupt_configure,
	.manage_callback = gpio_ipro_manage_callback,
	.get_pending_int = gpio_ipro_get_pending_int,
};

#define GPIO_IPRO_INIT(n)                                                                          \
	static const struct gpio_ipro_config gpio_ipro_cfg_##n = {                                 \
		.common = {.port_pin_mask = GPIO_PORT_PIN_MASK_FROM_DT_INST(n)},                   \
		.port = DT_INST_PROP(n, pin_offset) == 0 ? 0 : 1,                                  \
		.pin_offset = DT_INST_PROP(n, pin_offset),                                         \
		.ngpios = DT_INST_PROP(n, ngpios),                                                 \
	};                                                                                         \
	static struct gpio_ipro_data gpio_ipro_data_##n;                                           \
	DEVICE_DT_INST_DEFINE(n, gpio_ipro_init, NULL, &gpio_ipro_data_##n, &gpio_ipro_cfg_##n,    \
			      PRE_KERNEL_1, CONFIG_GPIO_INIT_PRIORITY, &gpio_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_IPRO_INIT)
