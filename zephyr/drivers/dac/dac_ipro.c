/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 GPDAC (SDK LL drv_dac.c) as a Zephyr DAC: two 10-bit channels
 * (A = channel 0 on GPIO23, B = channel 1 on GPIO22), register-sourced
 * conversions at XCLK/(div+1). The pad output is enabled unless the channel
 * is set up with `internal` only (the DAC outputs are also readable through
 * the ADC's channels 12/13, which is how the board test checks them). The
 * pad must be muxed to IPRO7_FUN_ANALOG through pinctrl for an external
 * output; DMA streaming (GPIP DAC DMA) is not exposed here.
 */

#define DT_DRV_COMPAT intelpro_ipro_dac

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/logging/log.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "drv_dac.h"
#include "drv_gpio.h"
#include "drv_glb.h"

LOG_MODULE_REGISTER(dac_ipro, CONFIG_DAC_LOG_LEVEL);

#define DAC_IPRO_CHANNELS   2
#define DAC_IPRO_RESOLUTION 10

struct dac_ipro_config {
	uint8_t clock_div; /* XCLK divider - 1 for GLB_Set_GPDAC_CLK_Sel */
};

struct dac_ipro_data {
	bool inited;
	bool chan_en[DAC_IPRO_CHANNELS];
	bool pad_out[DAC_IPRO_CHANNELS];
};

static void dac_ipro_apply(const struct device *dev)
{
	struct dac_ipro_data *data = dev->data;
	GLB_GPIP_DAC_Cfg_Type cfg = {
		.refSel = GLB_DAC_REF_SEL_INTERNAL,
		.resetChanA = 1,
		.resetChanB = 1,
		.div = DAC_CLK_DIV_1,
		.dmaEn = 0,
		.dmaFmt = GPIP_DAC_DMA_FORMAT_0,
	};
	GLB_GPIP_DAC_ChanA_Cfg_Type a = {
		.chanCovtEn = data->chan_en[0],
		.outputEn = data->chan_en[0] && data->pad_out[0],
		.chanEn = data->chan_en[0],
		.src = GPIP_DAC_ChanA_SRC_REG,
	};
	GLB_GPIP_DAC_ChanB_Cfg_Type b = {
		.chanCovtEn = data->chan_en[1],
		.outputEn = data->chan_en[1] && data->pad_out[1],
		.chanEn = data->chan_en[1],
		.src = GPIP_DAC_ChanB_SRC_REG,
	};

	if (!data->inited) {
		(void)GLB_GPIP_DAC_Init(&cfg);
		data->inited = true;
	}
	GLB_GPIP_DAC_Set_ChanA_Config(&a);
	GLB_GPIP_DAC_Set_ChanB_Config(&b);
}

static int dac_ipro_channel_setup(const struct device *dev, const struct dac_channel_cfg *ch)
{
	struct dac_ipro_data *data = dev->data;

	if (ch->channel_id >= DAC_IPRO_CHANNELS) {
		return -EINVAL;
	}
	if (ch->resolution != DAC_IPRO_RESOLUTION) {
		LOG_ERR("resolution %u, only %u", ch->resolution, DAC_IPRO_RESOLUTION);
		return -ENOTSUP;
	}
	data->chan_en[ch->channel_id] = true;
	data->pad_out[ch->channel_id] = !ch->internal;
	if (!ch->internal) {
		/* The output pad (A = GPIO23, B = GPIO22) must sit on the analogue
		 * function for the GPDAC to drive it (hal_dac.c checks the same). */
		GLB_GPIO_Cfg_Type pad = {
			.gpioMode = GPIO_MODE_ANALOG,
			.pullType = GPIO_PULL_NONE,
			.drive = 0,
			.smtCtrl = 0,
			.gpioPin = ch->channel_id == 0 ? 23 : 22,
			.gpioFun = GPIO_FUN_ANALOG,
		};

		(void)GLB_GPIO_Init(&pad);
	}
	dac_ipro_apply(dev);
	return 0;
}

static int dac_ipro_write_value(const struct device *dev, uint8_t channel, uint32_t value)
{
	struct dac_ipro_data *data = dev->data;

	if (channel >= DAC_IPRO_CHANNELS || !data->chan_en[channel]) {
		return -EINVAL;
	}
	if (value >= BIT(DAC_IPRO_RESOLUTION)) {
		return -EINVAL;
	}
	if (channel == 0U) {
		GLB_DAC_Set_ChanA_Value((uint16_t)value);
	} else {
		GLB_DAC_Set_ChanB_Value((uint16_t)value);
	}
	return 0;
}

static int dac_ipro_init(const struct device *dev)
{
	const struct dac_ipro_config *cfg = dev->config;

	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_GPIP);
	(void)GLB_Set_GPDAC_CLK_Sel(GLB_DAC_CLK_XCLK_32M, cfg->clock_div, 1);
	return 0;
}

static DEVICE_API(dac, dac_ipro_api) = {
	.channel_setup = dac_ipro_channel_setup,
	.write_value = dac_ipro_write_value,
};

#define DAC_IPRO_INIT(n)                                                                           \
	static const struct dac_ipro_config dac_ipro_cfg_##n = {                                   \
		.clock_div = DT_INST_PROP(n, clock_divider),                                       \
	};                                                                                         \
	static struct dac_ipro_data dac_ipro_data_##n;                                             \
	DEVICE_DT_INST_DEFINE(n, dac_ipro_init, NULL, &dac_ipro_data_##n, &dac_ipro_cfg_##n,      \
			      POST_KERNEL, CONFIG_DAC_INIT_PRIORITY, &dac_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(DAC_IPRO_INIT)
