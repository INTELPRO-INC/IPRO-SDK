/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 PWM driver over the SDK LL (drv_pwm.c).
 *
 * The block has ONE period register and ONE clock divider shared by its
 * four channels. pwm_set_cycles() therefore programs the block period the
 * first time a channel is used and rejects a different period while another
 * channel is active. Per channel: thresholdL = 0, thresholdH = pulse, so the
 * output is asserted for the first `pulse` counts of every period; the
 * polarity flag flips the active level in hardware (PWM_Channelx_Polarity_Set).
 */

#define DT_DRV_COMPAT intelpro_ipro_pwm

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/logging/log.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "drv_pwm.h"
#include "pwm_reg.h"
#include "drv_clock.h"

LOG_MODULE_REGISTER(pwm_ipro, CONFIG_PWM_LOG_LEVEL);

#define PWM_IPRO_CHANNELS   4
#define PWM_IPRO_MAX_PERIOD 0xFFFFU

struct pwm_ipro_config {
	const struct pinctrl_dev_config *pincfg;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	PWM_Clk_Type clk_src;
	uint16_t clk_div;
};

struct pwm_ipro_data {
	uint32_t period;                  /* cycles, 0 = block not started */
	uint8_t active;                   /* bitmask of running channels */
};

static uint32_t pwm_ipro_src_hz(const struct pwm_ipro_config *cfg)
{
	switch (cfg->clk_src) {
	case PWM_CLK_BCLK:
		return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK);
	case PWM_CLK_32K:
		return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_F32K);
	case PWM_CLK_XCLK:
	default:
		return Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK);
	}
}

static int pwm_ipro_get_cycles_per_sec(const struct device *dev, uint32_t channel, uint64_t *cycles)
{
	const struct pwm_ipro_config *cfg = dev->config;

	if (channel >= PWM_IPRO_CHANNELS) {
		return -EINVAL;
	}
	*cycles = pwm_ipro_src_hz(cfg) / cfg->clk_div;
	return 0;
}

static int pwm_ipro_set_cycles(const struct device *dev, uint32_t channel, uint32_t period_cycles,
			       uint32_t pulse_cycles, pwm_flags_t flags)
{
	const struct pwm_ipro_config *cfg = dev->config;
	struct pwm_ipro_data *data = dev->data;
	PWM_CHx_CFG_Type ch = {
		.modP = PWM_MODE_ENABLE,
		.modN = PWM_MODE_DISABLE,
		.polP = (flags & PWM_POLARITY_INVERTED) ? PWM_POL_ACTIVE_LOW : PWM_POL_ACTIVE_HIGH,
		.polN = PWM_POL_ACTIVE_HIGH,
		.idlP = PWM_IDLE_STATE_INACTIVE,
		.idlN = PWM_IDLE_STATE_INACTIVE,
		.brkP = PWM_BREAK_STATE_INACTIVE,
		.brkN = PWM_BREAK_STATE_INACTIVE,
		.thresholdL = 0,
		.dtg = 0,
	};

	if (channel >= PWM_IPRO_CHANNELS) {
		return -EINVAL;
	}
	if (period_cycles == 0 || period_cycles > PWM_IPRO_MAX_PERIOD) {
		LOG_ERR("period %u cycles out of range (1..%u)", period_cycles, PWM_IPRO_MAX_PERIOD);
		return -ENOTSUP;
	}
	if (pulse_cycles > period_cycles) {
		return -EINVAL;
	}

	/* Shared period: only the first active channel may (re)define it. */
	uint8_t others = data->active & ~BIT(channel);

	if (others != 0 && data->period != period_cycles) {
		LOG_ERR("period %u conflicts with active channels (period %u)", period_cycles,
			data->period);
		return -EBUSY;
	}

	if (data->period != period_cycles) {
		/*
		 * PWMx_Init() polls PWM_STS_STOP with a short loop and returns
		 * TIMEOUT if the block is still finishing a graceful stop (the
		 * previous period may be tens of ms). Wait for the stop here,
		 * bounded, before re-programming the period.
		 */
		for (int i = 0; i < 5000; i++) {
			uint32_t st = *(volatile uint32_t *)(PWM_BASE + PWM_MC0_CONFIG0_OFFSET);

			if (reg_is_bit_set(st, PWM_STS_STOP)) {
				break;
			}
			k_busy_wait(10);
		}

		PWMx_CFG_Type blk = {
			.clk = cfg->clk_src,
			.clkDiv = cfg->clk_div,
			.period = (uint16_t)period_cycles,
			.stopMode = PWM_STOP_GRACEFUL,
			.extPol = PWM_BREAK_Polarity_LOW,
			.adcSrc = PWM_TRIGADC_SOURCE_NONE,
			.stpRept = DISABLE,
			.intPulseCnt = 0,
		};

		if (PWMx_Init(PWM0_ID, &blk) != 0) {
			return -EIO;
		}
		data->period = period_cycles;
	}

	if (pulse_cycles == 0) {
		/* Constant inactive level. */
		ch.modP = PWM_MODE_DISABLE;
		PWM_Channelx_Init(PWM0_ID, (PWM_CHx_Type)channel, &ch);
		data->active &= ~BIT(channel);
	} else {
		/* thresholdH == period keeps the output asserted for the whole
		 * period (100 % duty). */
		ch.thresholdH = (uint16_t)pulse_cycles;
		PWM_Channelx_Init(PWM0_ID, (PWM_CHx_Type)channel, &ch);
		data->active |= BIT(channel);
	}

	if (data->active != 0) {
		PWMx_Enable(PWM0_ID);
	} else {
		PWMx_Disable(PWM0_ID);
		data->period = 0;
	}
	return 0;
}

static int pwm_ipro_init(const struct device *dev)
{
	const struct pwm_ipro_config *cfg = dev->config;
	int ret;

	ret = clock_control_on(cfg->clock_dev, cfg->clock_id);
	if (ret != 0) {
		return ret;
	}
	return pinctrl_apply_state(cfg->pincfg, PINCTRL_STATE_DEFAULT);
}

static DEVICE_API(pwm, pwm_ipro_api) = {
	.set_cycles = pwm_ipro_set_cycles,
	.get_cycles_per_sec = pwm_ipro_get_cycles_per_sec,
};

#define PWM_IPRO_CLK_SRC(n)                                                                        \
	(DT_INST_ENUM_IDX(n, clock_source) == 1   ? PWM_CLK_BCLK                                  \
	 : DT_INST_ENUM_IDX(n, clock_source) == 2 ? PWM_CLK_32K                                   \
						  : PWM_CLK_XCLK)

#define PWM_IPRO_INIT(n)                                                                           \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	static const struct pwm_ipro_config pwm_ipro_cfg_##n = {                                   \
		.pincfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                       \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, id),                    \
		.clk_src = PWM_IPRO_CLK_SRC(n),                                                    \
		.clk_div = DT_INST_PROP(n, clock_divider),                                         \
	};                                                                                         \
	static struct pwm_ipro_data pwm_ipro_data_##n;                                             \
	DEVICE_DT_INST_DEFINE(n, pwm_ipro_init, NULL, &pwm_ipro_data_##n, &pwm_ipro_cfg_##n,       \
			      POST_KERNEL, CONFIG_PWM_INIT_PRIORITY, &pwm_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(PWM_IPRO_INIT)
