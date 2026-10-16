/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * wdt_ipro: single-stage timeout install rules, feed keeps the board alive
 * across the timeout, and the callback (interrupt) mode fires.
 */
#include <zephyr/ztest.h>
#include <zephyr/drivers/watchdog.h>

static const struct device *const wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));
static volatile int fired;

static void on_wdt(const struct device *dev, int ch)
{
	ARG_UNUSED(dev); ARG_UNUSED(ch);
	fired++;
}

ZTEST(ipro_wdt, test_install_rules)
{
	struct wdt_timeout_cfg cfg = {.window = {.min = 0, .max = 1000}, .flags = WDT_FLAG_RESET_SOC};

	zassert_true(device_is_ready(wdt));
	zassert_equal(wdt_setup(wdt, 0), -EINVAL, "setup before install");

	cfg.window.min = 10;
	zassert_equal(wdt_install_timeout(wdt, &cfg), -EINVAL, "window min unsupported");
	cfg.window.min = 0;
	cfg.window.max = 0x10000;
	zassert_equal(wdt_install_timeout(wdt, &cfg), -EINVAL, "17-bit timeout");
	cfg.window.max = 1000;
	cfg.flags = WDT_FLAG_RESET_NONE;
	zassert_equal(wdt_install_timeout(wdt, &cfg), -EINVAL, "no reset and no callback");

	cfg.flags = WDT_FLAG_RESET_SOC;
	zassert_true(wdt_install_timeout(wdt, &cfg) >= 0);
	zassert_equal(wdt_install_timeout(wdt, &cfg), -ENOMEM, "single stage");
	zassert_equal(wdt_setup(wdt, WDT_OPT_PAUSE_IN_SLEEP), -ENOTSUP);
	zassert_ok(wdt_disable(wdt));
}

ZTEST(ipro_wdt, test_feed_keeps_alive)
{
	struct wdt_timeout_cfg cfg = {.window = {.max = 200}, .flags = WDT_FLAG_RESET_SOC};
	int ch = wdt_install_timeout(wdt, &cfg);

	zassert_true(ch >= 0);
	zassert_ok(wdt_setup(wdt, 0));
	for (int i = 0; i < 6; i++) {          /* 600 ms > 200 ms timeout */
		k_msleep(100);
		zassert_ok(wdt_feed(wdt, ch));
	}
	zassert_equal(wdt_feed(wdt, 1), -EINVAL);
	zassert_ok(wdt_disable(wdt));
}

ZTEST(ipro_wdt, test_callback_mode_fires)
{
	struct wdt_timeout_cfg cfg = {
		.window = {.max = 100}, .flags = WDT_FLAG_RESET_NONE, .callback = on_wdt,
	};

	fired = 0;
	zassert_true(wdt_install_timeout(wdt, &cfg) >= 0);
	zassert_ok(wdt_setup(wdt, 0));
	k_msleep(350);
	zassert_ok(wdt_disable(wdt));
	zassert_true(fired >= 1, "watchdog interrupt did not fire (%d)", fired);
}

ZTEST_SUITE(ipro_wdt, NULL, NULL, NULL, NULL, NULL);
