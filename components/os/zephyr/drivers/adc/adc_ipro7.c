/*
 * Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ipro_ipro7_adc

#include <zephyr/drivers/adc.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

#include "drv_adc.h"

LOG_MODULE_REGISTER(adc_ipro7, CONFIG_ADC_LOG_LEVEL);

#define ADC_IPRO7_CHANNELS		8
#define ADC_IPRO7_RESOLUTION		12

struct adc_ipro7_config {
	uint32_t base;
	uint8_t channels;
};

struct adc_ipro7_data {
	struct k_mutex mutex;
};

static int adc_ipro7_channel_setup(const struct device *dev,
				   const struct adc_channel_cfg *channel_cfg)
{
	if (channel_cfg->channel_id >= ADC_IPRO7_CHANNELS) {
		return -EINVAL;
	}
	
	if (channel_cfg->acquisition_time != ADC_ACQ_TIME_DEFAULT) {
		return -ENOTSUP;
	}
	
	if (channel_cfg->reference != ADC_REF_INTERNAL) {
		return -ENOTSUP;
	}
	
	if (channel_cfg->gain != ADC_GAIN_1) {
		return -ENOTSUP;
	}
	
	return 0;
}

static int adc_ipro7_read(const struct device *dev,
			  const struct adc_sequence *sequence)
{
	struct adc_ipro7_data *data = dev->data;
	ADC_CFG_Type adcCfg;
	ADC_FIFO_CFG_Type adcFifoCfg;
	uint32_t channel_mask = 0;
	uint16_t *buf = (uint16_t *)sequence->buffer;
	int ret = 0;
	
	if (sequence->resolution != ADC_IPRO7_RESOLUTION) {
		return -ENOTSUP;
	}
	
	k_mutex_lock(&data->mutex, K_FOREVER);
	
	/* Configure ADC */
	adcCfg.v18Sel = ADC_V18_SEL_1P82V;
	adcCfg.v11Sel = ADC_V11_SEL_1P1V;
	adcCfg.clkDiv = ADC_CLK_DIV_32;
	adcCfg.gainSel1 = ADC_PGA_GAIN_1;
	adcCfg.gainSel2 = ADC_PGA_GAIN_1;
	adcCfg.chopMode = ADC_CHOP_MOD_AZ_PGA_ON;
	adcCfg.biasSel = ADC_BIAS_SEL_MAIN_BANDGAP;
	adcCfg.vcm = ADC_PGA_VCM_1P2V;
	adcCfg.vref = ADC_VREF_2P0V;
	adcCfg.inputMode = ADC_INPUT_SINGLE_END;
	adcCfg.resWidth = ADC_DATA_WIDTH_12_WITH_256_AVERAGE;
	adcCfg.offsetCalibEn = 0;
	adcCfg.offsetCalibVal = 0;
	
	ADC_Init(ADC_ID_0, &adcCfg);
	ADC_Enable(ADC_ID_0);
	
	/* Configure FIFO */
	adcFifoCfg.fifoThreshold = ADC_FIFO_THRESHOLD_1;
	adcFifoCfg.dmaEn = DISABLE;
	ADC_FIFO_Cfg(&adcFifoCfg);
	
	/* Convert each channel */
	for (int i = 0; i < ADC_IPRO7_CHANNELS; i++) {
		if (sequence->channels & BIT(i)) {
			/* Configure channel */
			ADC_Channel_Config(ADC_ID_0, i, ADC_CHAN_SINGLE_END);
			
			/* Start conversion */
			ADC_Start(ADC_ID_0);
			
			/* Wait for conversion complete */
			while (ADC_Get_FIFO_Count() == 0) {
				k_yield();
			}
			
			/* Read result */
			uint32_t result = ADC_Read_FIFO();
			if (buf) {
				*buf++ = (uint16_t)result;
			}
			
			ADC_Stop(ADC_ID_0);
		}
	}
	
	ADC_Disable(ADC_ID_0);
	k_mutex_unlock(&data->mutex);
	
	return ret;
}

static int adc_ipro7_init(const struct device *dev)
{
	struct adc_ipro7_data *data = dev->data;
	
	LOG_DBG("Initializing ADC IPRO7 device %s", dev->name);
	
	k_mutex_init(&data->mutex);
	
	/* Initialize ADC hardware */
	GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_96M, 7);
	
	return 0;
}

static const struct adc_driver_api adc_ipro7_driver_api = {
	.channel_setup = adc_ipro7_channel_setup,
	.read = adc_ipro7_read,
};

#define ADC_IPRO7_INIT(n)						\
	static const struct adc_ipro7_config adc_ipro7_config_##n = {	\
		.base = DT_INST_REG_ADDR(n),				\
		.channels = ADC_IPRO7_CHANNELS,				\
	};								\
									\
	static struct adc_ipro7_data adc_ipro7_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n, adc_ipro7_init,			\
			      NULL,					\
			      &adc_ipro7_data_##n,			\
			      &adc_ipro7_config_##n,			\
			      POST_KERNEL,				\
			      CONFIG_ADC_INIT_PRIORITY,			\
			      &adc_ipro7_driver_api);

DT_INST_FOREACH_STATUS_OKAY(ADC_IPRO7_INIT)
