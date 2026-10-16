/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 GPADC (SDK LL drv_adc.c) as a Zephyr ADC.
 *
 * 12/14/16-bit single-ended conversions, one sample per selected channel per
 * sequence, scan mode for multi-channel sequences. The hardware's plain
 * 12-bit conversion is not usable on this silicon (GND, Vref and VBAT/2 all
 * read the same ~0.8 V on the EVB; the SDK's own tools only ever use the
 * averaging modes), so every resolution runs through hardware averaging:
 * 12-bit = 16-sample average scaled down, 14-bit = 16 or 64 samples, 16-bit
 * = 128 or 256 samples (adc_sequence.oversampling 4/6 and 7/8 select the
 * longer ones). Channels 0-11 are the analog pads (the pad
 * must be muxed to IPRO7_FUN_ANALOG through pinctrl); 12/13 read the two
 * DAC outputs, 14/15 the temperature sensor, 16 Vref, 18 VBAT/2, 23 GND.
 * VBAT and TSEN are powered on when such a channel is set up. Reference is
 * the internal 3.2 V or 2.0 V (adc_channel_cfg.reference = ADC_REF_INTERNAL,
 * "vref-mv" on the node selects which), PGA gain 1 only.
 *
 * Conversions are interrupt driven: the block runs in continuous mode over
 * the scan list, the data-ready interrupt drains the FIFO, the first
 * CONFIG_ADC_IPRO_DISCARD_ROUNDS rounds are dropped (the GPADC's first
 * conversions after a start carry the previous input: single 12-bit reads
 * of GND, Vref and VBAT/2 all returned the same ~0.8 V on the EVB, while the
 * SDK's own apps read 16 continuous samples and average the later ones),
 * the next round goes into the caller's buffer and the block is stopped. The
 * adc_context helper completes the read (synchronous, or asynchronous with
 * CONFIG_ADC_ASYNC).
 */

#define DT_DRV_COMPAT intelpro_ipro_adc

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/irq.h>
#include <zephyr/logging/log.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "drv_adc.h"
#include "drv_glb.h"

#define ADC_CONTEXT_USES_KERNEL_TIMER
#include "adc_context.h"

LOG_MODULE_REGISTER(adc_ipro, CONFIG_ADC_LOG_LEVEL);

#define ADC_IPRO_NUM_CHANNELS 24
#define ADC_IPRO_MAX_SCAN     12 /* GLB_GPADC_SCAN_LENGTH: up to 12 entries */
#define ADC_IPRO_FIFO_WORDS   32

struct adc_ipro_config {
	uint32_t vref_mv;
	uint8_t clk_div;
	uint32_t irq;
};

struct adc_ipro_data {
	struct adc_context ctx;
	const struct device *dev;
	uint32_t channels_configured;
	/* Current sequence */
	uint8_t scan_list[ADC_IPRO_MAX_SCAN];
	uint8_t scan_len;
	uint8_t collected;
	uint16_t seen; /* FIFO words consumed in this sampling */
	uint8_t width; /* ADC_Data_Width_Type */
	uint8_t resolution; /* requested by the sequence */
	uint8_t hw_width; /* what the block is programmed for */
	uint16_t *buffer;
	uint16_t *repeat_buffer;
	uint32_t channels;
};

static void adc_ipro_apply_width(const struct device *dev, uint8_t width)
{
	const struct adc_ipro_config *cfg = dev->config;
	struct adc_ipro_data *data = dev->data;
	ADC_CFG_Type acfg = {
		.v18Sel = ADC_V18_SEL_1P82V,
		.v11Sel = ADC_V11_SEL_1P1V,
		.clkDiv = cfg->clk_div,
		.gain1 = ADC_PGA_GAIN_1,
		.gain2 = ADC_PGA_GAIN_1,
		.chopMode = ADC_CHOP_MOD_AZ_PGA_ON,
		.biasSel = ADC_BIAS_SEL_MAIN_BANDGAP,
		.vcm = ADC_PGA_VCM_1P2V,
		.vref = cfg->vref_mv == 2000 ? ADC_VREF_2P0V : ADC_VREF_3P2V,
		.inputMode = ADC_INPUT_SINGLE_END,
		.resWidth = width,
		.offsetCalibEn = 0,
		.offsetCalibVal = 0,
	};
	ADC_FIFO_Cfg_Type fifo = {.fifoThreshold = ADC_FIFO_THRESHOLD_1, .dmaEn = 0};

	ADC_Disable();
	ADC_Enable();
	ADC_Reset();
	ADC_Init(&acfg);
	ADC_FIFO_Cfg(&fifo);
	ADC_IntClr(ADC_INT_ALL);
	data->hw_width = width;
}

static int adc_ipro_channel_setup(const struct device *dev, const struct adc_channel_cfg *ch)
{
	struct adc_ipro_data *data = dev->data;

	if (ch->channel_id >= ADC_IPRO_NUM_CHANNELS) {
		return -EINVAL;
	}
	if (ch->differential) {
		LOG_ERR("differential input not supported");
		return -ENOTSUP;
	}
	if (ch->gain != ADC_GAIN_1) {
		LOG_ERR("only gain 1");
		return -ENOTSUP;
	}
	if (ch->reference != ADC_REF_INTERNAL) {
		LOG_ERR("only the internal reference");
		return -ENOTSUP;
	}
	if (ch->acquisition_time != ADC_ACQ_TIME_DEFAULT) {
		return -ENOTSUP;
	}

	switch (ch->channel_id) {
	case ADC_CHAN_VABT_HALF:
		ADC_Vbat_Enable();
		break;
	case ADC_CHAN_TSEN_P:
	case ADC_CHAN_TSEN_N:
		ADC_Tsen_Init(ADC_TSEN_MOD_INTERNAL_DIODE);
		ADC_Tsen_Enable();
		break;
	default:
		break;
	}
	data->channels_configured |= BIT(ch->channel_id);
	return 0;
}

static int adc_ipro_width_for(uint8_t resolution, uint8_t oversampling, uint8_t *width)
{
	switch (resolution) {
	case 12:
		if (oversampling != 0) {
			return -EINVAL;
		}
		*width = ADC_DATA_WIDTH_14_WITH_16_AVERAGE; /* scaled to 12 bits on read */
		return 0;
	case 14:
		*width = (oversampling == 6) ? ADC_DATA_WIDTH_14_WITH_64_AVERAGE
					     : ADC_DATA_WIDTH_14_WITH_16_AVERAGE;
		return (oversampling == 0 || oversampling == 4 || oversampling == 6) ? 0 : -EINVAL;
	case 16:
		*width = (oversampling == 8) ? ADC_DATA_WIDTH_16_WITH_256_AVERAGE
					     : ADC_DATA_WIDTH_16_WITH_128_AVERAGE;
		return (oversampling == 0 || oversampling == 7 || oversampling == 8) ? 0 : -EINVAL;
	default:
		return -EINVAL;
	}
}

static int adc_ipro_start_read(const struct device *dev, const struct adc_sequence *seq)
{
	struct adc_ipro_data *data = dev->data;
	uint32_t ch = seq->channels;
	size_t needed;
	uint8_t width;

	if (ch == 0U || (ch & ~data->channels_configured) != 0U) {
		LOG_ERR("channels 0x%x not configured", ch);
		return -EINVAL;
	}
	if (adc_ipro_width_for(seq->resolution, seq->oversampling, &width) != 0) {
		LOG_ERR("resolution %u / oversampling %u", seq->resolution, seq->oversampling);
		return -EINVAL;
	}

	data->scan_len = 0;
	for (uint8_t i = 0; i < ADC_IPRO_NUM_CHANNELS; i++) {
		if (ch & BIT(i)) {
			if (data->scan_len == ADC_IPRO_MAX_SCAN) {
				return -EINVAL;
			}
			data->scan_list[data->scan_len++] = i;
		}
	}

	needed = data->scan_len * sizeof(uint16_t);
	if (seq->options != NULL) {
		needed *= (seq->options->extra_samplings + 1U);
	}
	if (seq->buffer_size < needed) {
		LOG_ERR("buffer %u < %u", (unsigned)seq->buffer_size, (unsigned)needed);
		return -ENOMEM;
	}

	data->width = width;
	data->resolution = seq->resolution;
	data->channels = ch;
	data->buffer = seq->buffer;
	if (data->hw_width != width) {
		adc_ipro_apply_width(dev, width);
	}

	adc_context_start_read(&data->ctx, seq);
	return adc_context_wait_for_completion(&data->ctx);
}

static int adc_ipro_read(const struct device *dev, const struct adc_sequence *seq)
{
	struct adc_ipro_data *data = dev->data;
	int ret;

	adc_context_lock(&data->ctx, false, NULL);
	ret = adc_ipro_start_read(dev, seq);
	adc_context_release(&data->ctx, ret);
	return ret;
}

#ifdef CONFIG_ADC_ASYNC
static int adc_ipro_read_async(const struct device *dev, const struct adc_sequence *seq,
			       struct k_poll_signal *async)
{
	struct adc_ipro_data *data = dev->data;
	int ret;

	adc_context_lock(&data->ctx, true, async);
	ret = adc_ipro_start_read(dev, seq);
	adc_context_release(&data->ctx, ret);
	return ret;
}
#endif

static void adc_context_start_sampling(struct adc_context *ctx)
{
	struct adc_ipro_data *data = CONTAINER_OF(ctx, struct adc_ipro_data, ctx);
	uint8_t neg[ADC_IPRO_MAX_SCAN];

	data->repeat_buffer = data->buffer;
	data->collected = 0;
	data->seen = 0;
	for (uint8_t i = 0; i < data->scan_len; i++) {
		neg[i] = ADC_CHAN_GND;
	}

	ADC_Stop();
	ADC_FIFO_Clear();
	ADC_IntClr(ADC_INT_ALL);
	if (data->scan_len == 1U) {
		ADC_Channel_Config(data->scan_list[0], ADC_CHAN_GND, 1);
	} else {
		ADC_Scan_Channel_Config(data->scan_list, neg, data->scan_len, 1);
	}
	ADC_IntMask(ADC_INT_ADC_READY, 0);
	ADC_Start();
}

static void adc_context_update_buffer_pointer(struct adc_context *ctx, bool repeat)
{
	struct adc_ipro_data *data = CONTAINER_OF(ctx, struct adc_ipro_data, ctx);

	if (repeat) {
		data->buffer = data->repeat_buffer;
	}
}

static uint16_t adc_ipro_raw_to_sample(uint32_t raw, uint8_t width, uint8_t resolution)
{
	uint16_t v;

	switch (width) {
	case ADC_DATA_WIDTH_12:
		v = (uint16_t)((raw & 0xFFFFU) >> 4);
		break;
	case ADC_DATA_WIDTH_14_WITH_16_AVERAGE:
	case ADC_DATA_WIDTH_14_WITH_64_AVERAGE:
		v = (uint16_t)((raw & 0xFFFFU) >> 2);
		break;
	default:
		v = (uint16_t)(raw & 0xFFFFU);
		break;
	}
	return (resolution == 12U && width != ADC_DATA_WIDTH_12) ? (v >> 2) : v;
}

static void adc_ipro_isr(const void *arg)
{
	const struct device *dev = arg;
	struct adc_ipro_data *data = dev->data;

	if (!ADC_GetIntStatus(ADC_INT_ADC_READY)) {
		ADC_IntClr(ADC_INT_ALL);
		return;
	}
	ADC_IntClr(ADC_INT_ADC_READY);

	while (!ADC_FIFO_Is_Empty() && data->collected < data->scan_len) {
		uint32_t raw = ADC_Read_FIFO();
		uint8_t pos = (uint8_t)(raw >> 21);
		uint8_t idx;


		/* Place the sample by its channel, in sequence order. */
		for (idx = 0; idx < data->scan_len; idx++) {
			if (data->scan_list[idx] == pos) {
				break;
			}
		}
		if (idx == data->scan_len) {
			continue; /* stale FIFO word from a previous run */
		}
		data->buffer[idx] = adc_ipro_raw_to_sample(raw, data->width, data->resolution);
		data->collected++;
	}

	if (data->collected >= data->scan_len) {
		ADC_IntMask(ADC_INT_ADC_READY, 1);
		ADC_Stop();
		data->buffer += data->scan_len;
		adc_context_on_sampling_done(&data->ctx, dev);
	}
}

static int adc_ipro_init(const struct device *dev)
{
	const struct adc_ipro_config *cfg = dev->config;
	struct adc_ipro_data *data = dev->data;

	data->dev = dev;
	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_GPIP);
	GLB_Set_ADC_CLK(1, GLB_ADC_CLK_XCLK, 0);
	ADC_IntMask(ADC_INT_ALL, 1);
	data->hw_width = 0xFF;
	adc_ipro_apply_width(dev, ADC_DATA_WIDTH_12);

	IRQ_CONNECT(DT_INST_IRQN(0), DT_INST_IRQ(0, priority), adc_ipro_isr, DEVICE_DT_INST_GET(0),
		    0);
	irq_enable(cfg->irq);
	adc_context_unlock_unconditionally(&data->ctx);
	return 0;
}

static DEVICE_API(adc, adc_ipro_api) = {
	.channel_setup = adc_ipro_channel_setup,
	.read = adc_ipro_read,
#ifdef CONFIG_ADC_ASYNC
	.read_async = adc_ipro_read_async,
#endif
	.ref_internal = DT_INST_PROP(0, vref_mv),
};

#define ADC_IPRO_INIT(n)                                                                           \
	static const struct adc_ipro_config adc_ipro_cfg_##n = {                                   \
		.vref_mv = DT_INST_PROP(n, vref_mv),                                               \
		.clk_div = DT_INST_PROP(n, clock_divider),                                         \
		.irq = DT_INST_IRQN(n),                                                            \
	};                                                                                         \
	static struct adc_ipro_data adc_ipro_data_##n = {                                          \
		ADC_CONTEXT_INIT_TIMER(adc_ipro_data_##n, ctx),                                    \
		ADC_CONTEXT_INIT_LOCK(adc_ipro_data_##n, ctx),                                     \
		ADC_CONTEXT_INIT_SYNC(adc_ipro_data_##n, ctx),                                     \
	};                                                                                         \
	DEVICE_DT_INST_DEFINE(n, adc_ipro_init, NULL, &adc_ipro_data_##n, &adc_ipro_cfg_##n,      \
			      POST_KERNEL, CONFIG_ADC_INIT_PRIORITY, &adc_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(ADC_IPRO_INIT)
