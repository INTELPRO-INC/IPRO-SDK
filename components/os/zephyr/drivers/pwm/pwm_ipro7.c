/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ipro_ipro7_pwm

#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "drv_pwm.h"

LOG_MODULE_REGISTER(pwm_ipro7, CONFIG_PWM_LOG_LEVEL);

struct pwm_ipro7_config {
	uint32_t base;
	uint8_t channels;
};

struct pwm_ipro7_data {
	/* Driver data */
};

static int pwm_ipro7_set_cycles(const struct device *dev, uint32_t channel,
				uint32_t period_cycles, uint32_t pulse_cycles,
				pwm_flags_t flags)
{
	const struct pwm_ipro7_config *config = dev->config;
	PWM_CH_CFG_Type pwmCfg;

	if (channel >= config->channels) {
		return -EINVAL;
	}

	/* Configure PWM channel */
	pwmCfg.ch = (PWM_CH_ID_Type)channel;
	pwmCfg.clk = PWM_CLK_XCLK;
	pwmCfg.clkDiv = 1;
	pwmCfg.stopMode = PWM_STOP_ABRUPT;
	pwmCfg.pol = (flags & PWM_POLARITY_INVERTED) ? PWM_POL_INVERT : PWM_POL_NORMAL;
	pwmCfg.period = period_cycles;
	pwmCfg.threshold1 = pulse_cycles;
	pwmCfg.threshold2 = 0;
	pwmCfg.intPulseCnt = 0;

	PWM_Channel_Init(&pwmCfg);
	
	if (pulse_cycles > 0) {
		PWM_Channel_Enable(channel);
	} else {
		PWM_Channel_Disable(channel);
	}

	return 0;
}static int pwm_ipro7_get_cycles_per_sec(const struct device *dev,
					uint32_t channel, uint64_t *cycles)
{
	/* Assuming 100MHz clock */
	*cycles = 100000000ULL;
	return 0;
}

static int pwm_ipro7_init(const struct device *dev)
{
	LOG_DBG("Initializing PWM IPRO7 device %s", dev->name);
	
	/* PWM hardware initialization is done per channel in set_cycles */
	
	return 0;
}

static const struct pwm_driver_api pwm_ipro7_driver_api = {
	.set_cycles = pwm_ipro7_set_cycles,
	.get_cycles_per_sec = pwm_ipro7_get_cycles_per_sec,
};

#define PWM_IPRO7_INIT(n)						\
	static const struct pwm_ipro7_config pwm_ipro7_config_##n = {	\
		.base = DT_INST_REG_ADDR(n),				\
		.channels = 5,					\
	};								\
									\
	static struct pwm_ipro7_data pwm_ipro7_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n, pwm_ipro7_init,			\
			      NULL,					\
			      &pwm_ipro7_data_##n,			\
			      &pwm_ipro7_config_##n,			\
			      POST_KERNEL,				\
			      CONFIG_PWM_INIT_PRIORITY,			\
			      &pwm_ipro7_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PWM_IPRO7_INIT)
