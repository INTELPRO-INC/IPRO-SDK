/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 watchdog: Zephyr wdt_driver_api -> ipro_hal_wdt_* facade -> SDK
 * hal_wdt.c -> drv_timer.c.
 *
 * The counter runs at 32 kHz / 32 = 1 kHz (WDT_Set_Clock in the clock
 * controller), so the 16-bit compare value is the timeout in milliseconds.
 * WDT_FLAG_RESET_* -> hardware reset on expiry; a callback selects the
 * interrupt-mode watchdog instead (the SDK's wdt_init(int_mode=1)).
 */

#define DT_DRV_COMPAT intelpro_ipro_wdt

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/drivers/clock_control.h>
#include <ipro_zephyr/ipro_hal_glue.h>

#define WDT_IPRO_MAX_MS 0xFFFFU

struct wdt_ipro_config {
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
};

struct wdt_ipro_data {
	wdt_callback_t cb;
	uint32_t timeout_ms;
	bool installed;
	bool irq_mode;
};

static void wdt_ipro_isr(void)
{
	const struct device *dev = DEVICE_DT_INST_GET(0);
	struct wdt_ipro_data *data = dev->data;

	if (data->cb != NULL) {
		data->cb(dev, 0);
	}
}

static int wdt_ipro_setup(const struct device *dev, uint8_t options)
{
	struct wdt_ipro_data *data = dev->data;

	if (!data->installed) {
		return -EINVAL;
	}
	if (options & WDT_OPT_PAUSE_IN_SLEEP) {
		return -ENOTSUP;
	}
	ipro_hal_wdt_start(data->irq_mode, data->timeout_ms);
	return 0;
}

static int wdt_ipro_disable(const struct device *dev)
{
	struct wdt_ipro_data *data = dev->data;

	ipro_hal_wdt_stop();
	data->installed = false;
	return 0;
}

static int wdt_ipro_install_timeout(const struct device *dev, const struct wdt_timeout_cfg *cfg)
{
	struct wdt_ipro_data *data = dev->data;

	if (data->installed) {
		return -ENOMEM; /* single stage */
	}
	if (cfg->window.min != 0 || cfg->window.max == 0 || cfg->window.max > WDT_IPRO_MAX_MS) {
		return -EINVAL;
	}
	if ((cfg->flags & WDT_FLAG_RESET_MASK) == WDT_FLAG_RESET_NONE && cfg->callback == NULL) {
		return -EINVAL;
	}

	data->cb = cfg->callback;
	data->timeout_ms = cfg->window.max;
	data->irq_mode = cfg->callback != NULL;
	data->installed = true;
	return 0;
}

static int wdt_ipro_feed(const struct device *dev, int channel_id)
{
	ARG_UNUSED(dev);
	if (channel_id != 0) {
		return -EINVAL;
	}
	ipro_hal_wdt_feed();
	return 0;
}

static int wdt_ipro_init(const struct device *dev)
{
	const struct wdt_ipro_config *cfg = dev->config;

	(void)clock_control_on(cfg->clock_dev, cfg->clock_id);
	ipro_hal_wdt_set_isr(wdt_ipro_isr);
	/* boot2 may leave the watchdog armed; the SDK disables it in
	 * platform_init(). Stay disarmed until the application sets it up. */
	ipro_hal_wdt_stop();
	return 0;
}

static DEVICE_API(wdt, wdt_ipro_api) = {
	.setup = wdt_ipro_setup,
	.disable = wdt_ipro_disable,
	.install_timeout = wdt_ipro_install_timeout,
	.feed = wdt_ipro_feed,
};

static const struct wdt_ipro_config wdt_ipro_cfg_0 = {
	.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(0)),
	.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(0, id),
};
static struct wdt_ipro_data wdt_ipro_data_0;

DEVICE_DT_INST_DEFINE(0, wdt_ipro_init, NULL, &wdt_ipro_data_0, &wdt_ipro_cfg_0, PRE_KERNEL_1,
		      CONFIG_KERNEL_INIT_PRIORITY_DEVICE, &wdt_ipro_api);
