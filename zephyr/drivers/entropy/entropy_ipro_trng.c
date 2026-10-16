/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 TRNG entropy driver: Zephyr entropy_driver_api -> SDK hal_sec_trng.c
 * (hal_rand_stream over the SEC_ENG TRNG, 32-byte hardware batches).
 */

#define DT_DRV_COMPAT intelpro_ipro_trng

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/drivers/clock_control.h>
#include <soc.h>

#include "hal_sec_common.h"

int hal_rand_stream(uint8_t *buf, int len);
int hal_sec_trng_init(void);

struct entropy_ipro_config {
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
};

static int entropy_ipro_get_entropy(const struct device *dev, uint8_t *buf, uint16_t len)
{
	ARG_UNUSED(dev);
	return hal_rand_stream(buf, len) == 0 ? 0 : -EIO;
}

static int entropy_ipro_get_entropy_isr(const struct device *dev, uint8_t *buf, uint16_t len,
					uint32_t flags)
{
	ARG_UNUSED(flags);
	/* The HAL path is polled and interrupt-safe (it masks interrupts
	 * around the FIFO refill), so it can serve ISR callers directly. */
	return entropy_ipro_get_entropy(dev, buf, len) == 0 ? len : -EIO;
}

static int entropy_ipro_init(const struct device *dev)
{
	const struct entropy_ipro_config *cfg = dev->config;
	int ret = clock_control_on(cfg->clock_dev, cfg->clock_id);

	if (ret != 0) {
		return ret;
	}
	return hal_sec_trng_init() == 0 ? 0 : -EIO;
}

static DEVICE_API(entropy, entropy_ipro_api) = {
	.get_entropy = entropy_ipro_get_entropy,
	.get_entropy_isr = entropy_ipro_get_entropy_isr,
};

static const struct entropy_ipro_config entropy_ipro_cfg_0 = {
	.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(0)),
	.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(0, id),
};

DEVICE_DT_INST_DEFINE(0, entropy_ipro_init, NULL, NULL, &entropy_ipro_cfg_0, PRE_KERNEL_1,
		      CONFIG_ENTROPY_INIT_PRIORITY, &entropy_ipro_api);
