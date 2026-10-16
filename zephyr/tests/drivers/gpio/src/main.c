/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * gpio_ipro: pin range, unsupported drive/trigger modes, output read-back
 * through the pad, and interrupt (un)configuration on the shared INT0 line.
 * Runs on any pad that is not wired to something that minds being toggled:
 * the led0 alias.
 */
#include <zephyr/ztest.h>
#include <zephyr/ztest_error_hook.h>
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct device *const port1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));

ZTEST(ipro_gpio, test_devices_ready)
{
	zassert_true(gpio_is_ready_dt(&led));
	zassert_true(device_is_ready(port1));
}

ZTEST(ipro_gpio, test_pin_out_of_range)
{
	/* gpio0 owns pads 0..31, gpio1 owns 32..41 */
#ifdef CONFIG_ASSERT
	/* With asserts on, gpio.h rejects the pin against port_pin_mask before
	 * the driver sees it; the expected outcome is that assertion. */
	ztest_set_assert_valid(true);
	(void)gpio_pin_configure(port1, 10, GPIO_OUTPUT);
	zassert_unreachable("out-of-range pin must assert");
#else
	zassert_equal(gpio_pin_configure(led.port, 32, GPIO_OUTPUT), -EINVAL);
	zassert_equal(gpio_pin_configure(port1, 10, GPIO_OUTPUT), -EINVAL);
#endif
}

ZTEST(ipro_gpio, test_open_drain_not_supported)
{
	zassert_equal(gpio_pin_configure_dt(&led, GPIO_OUTPUT | GPIO_OPEN_DRAIN), -ENOTSUP);
	zassert_equal(gpio_pin_configure_dt(&led, GPIO_OUTPUT | GPIO_OPEN_SOURCE), -ENOTSUP);
}

ZTEST(ipro_gpio, test_output_readback)
{
	zassert_ok(gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE));
	zassert_ok(gpio_pin_set_dt(&led, 1));
	zassert_equal(gpio_pin_get_dt(&led), 1, "output level must read back through the pad");
	zassert_ok(gpio_pin_set_dt(&led, 0));
	zassert_equal(gpio_pin_get_dt(&led), 0);
	zassert_ok(gpio_pin_toggle_dt(&led));
	zassert_equal(gpio_pin_get_dt(&led), 1);
	zassert_ok(gpio_pin_set_dt(&led, 0));
}

ZTEST(ipro_gpio, test_input_pulls)
{
	zassert_ok(gpio_pin_configure_dt(&led, GPIO_INPUT | GPIO_PULL_UP));
	k_busy_wait(100);
	zassert_equal(gpio_pin_get_raw(led.port, led.pin), 1, "pull-up must read high");
	zassert_ok(gpio_pin_configure_dt(&led, GPIO_INPUT | GPIO_PULL_DOWN));
	k_busy_wait(100);
	zassert_equal(gpio_pin_get_raw(led.port, led.pin), 0, "pull-down must read low");
	zassert_ok(gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE));
}

ZTEST(ipro_gpio, test_interrupt_modes)
{
	zassert_ok(gpio_pin_configure_dt(&led, GPIO_INPUT | GPIO_PULL_UP));

	zassert_ok(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_EDGE_FALLING));
	zassert_ok(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_EDGE_RISING));
	zassert_ok(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_LEVEL_LOW));
	zassert_ok(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_LEVEL_HIGH));
	/* GLB has no both-edge trigger: documented limitation */
	zassert_equal(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_EDGE_BOTH), -ENOTSUP);
	zassert_ok(gpio_pin_interrupt_configure_dt(&led, GPIO_INT_DISABLE));

	zassert_ok(gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE));
}

static struct gpio_callback cb;
static volatile int cb_hits;

static void on_edge(const struct device *port, struct gpio_callback *c, gpio_port_pins_t pins)
{
	ARG_UNUSED(port); ARG_UNUSED(c); ARG_UNUSED(pins);
	cb_hits++;
}

ZTEST(ipro_gpio, test_callback_add_remove)
{
	gpio_init_callback(&cb, on_edge, BIT(led.pin));
	zassert_ok(gpio_add_callback(led.port, &cb));
	zassert_ok(gpio_remove_callback(led.port, &cb));
}

ZTEST_SUITE(ipro_gpio, NULL, NULL, NULL, NULL, NULL);
