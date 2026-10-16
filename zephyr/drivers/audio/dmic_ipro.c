/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 PDM microphone input (the AUADC block's PDM path, SDK HAL
 * hal_auadc.c) as a Zephyr DMIC device. The HAL delivers 16-bit PCM frames
 * into a fixed ping-pong pair from its DMA completion; this driver keeps
 * that pair (allocated from the stream's memory slab at configure time),
 * copies each finished frame into a fresh slab block and queues it for
 * dmic_read(). One channel, 8/16/32/48 kHz nominal (the real rate is
 * hal_auadc_real_rate(): the audio PLL divides to 15875 Hz for "16 kHz").
 *
 * Verified on the IPRO7AI EVK's IM69D129 (GPIO18/19) on 2026-09-21.
 */

#define DT_DRV_COMPAT intelpro_ipro_pdm

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/audio/dmic.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_hal_glue.h>
#include <string.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "hal_auadc.h"

LOG_MODULE_REGISTER(dmic_ipro, CONFIG_AUDIO_DMIC_LOG_LEVEL);

#define DMIC_IPRO_QUEUE_LEN CONFIG_DMIC_IPRO_QUEUE_LEN

struct dmic_ipro_config {
	uint8_t clk_pin;  /* HAL index: 0, 2, 8 */
	uint8_t data_pin; /* HAL index: 1, 3, 7 */
	uint8_t channel;  /* HAL_AUADC_PDM_CH_LEFT / RIGHT */
};

struct dmic_ipro_data {
	enum dmic_state state;
	struct k_mem_slab *slab;
	size_t block_size;
	int16_t *frames[2];
	struct k_msgq queue;
	void *queue_buf[DMIC_IPRO_QUEUE_LEN];
	uint32_t dropped;
	volatile uint32_t discard;
	hal_auadc_pdm_cfg_t hal;
};

/* Frames dropped after START while the decimator settles (SDK: 5). */
#define DMIC_IPRO_STARTUP_DISCARD 5

static struct dmic_ipro_data *dmic_ipro_singleton;
static uint8_t dmic_ipro_dma_frames[2 * CONFIG_DMIC_IPRO_MAX_BLOCK_SIZE] __aligned(32);

/* HAL DMA completion: frame idx is full. */
static void dmic_ipro_frame_event(int idx)
{
	struct dmic_ipro_data *data = dmic_ipro_singleton;
	void *block;

	if (data == NULL || data->state != DMIC_STATE_ACTIVE) {
		return;
	}
	if (data->discard != 0U) {
		/* The decimator's start-up transient (the SDK drops the first
		 * frames too). */
		data->discard--;
		return;
	}
	if (k_mem_slab_alloc(data->slab, &block, K_NO_WAIT) != 0) {
		data->dropped++;
		return;
	}
	/* The AUADC DMA wrote the frame behind the D-cache. */
	ipro_hal_cache_invalidate((uint32_t)data->frames[idx], data->block_size);
	memcpy(block, data->frames[idx], data->block_size);
	if (k_msgq_put(&data->queue, &block, K_NO_WAIT) != 0) {
		k_mem_slab_free(data->slab, block);
		data->dropped++;
	}
}

static int dmic_ipro_configure(const struct device *dev, struct dmic_cfg *cfg)
{
	const struct dmic_ipro_config *config = dev->config;
	struct dmic_ipro_data *data = dev->data;
	struct pcm_stream_cfg *stream = &cfg->streams[0];

	if (data->state == DMIC_STATE_ACTIVE) {
		return -EBUSY;
	}
	if (cfg->channel.req_num_streams != 1 || cfg->channel.req_num_chan != 1) {
		return -EINVAL;
	}
	if (stream->pcm_width != 16) {
		return -ENOTSUP;
	}
	switch (stream->pcm_rate) {
	case 8000:
	case 16000:
	case 32000:
	case 48000:
		break;
	default:
		return -ENOTSUP;
	}
	if (stream->mem_slab == NULL || stream->block_size == 0U ||
	    stream->block_size > CONFIG_DMIC_IPRO_MAX_BLOCK_SIZE ||
	    (stream->block_size & 1U) != 0U) {
		return -EINVAL;
	}

	/*
	 * The HAL's ping-pong pair: pcm_frame_size is in BYTES and the DMA
	 * chain writes frame 1 right after frame 0, so the two frames must be
	 * one contiguous buffer (as the SDK's audio apps lay them out). A
	 * static, cache-line aligned pair; the slab only carries the copies
	 * handed to dmic_read().
	 */
	data->slab = stream->mem_slab;
	data->block_size = stream->block_size;
	data->frames[0] = (int16_t *)&dmic_ipro_dma_frames[0];
	data->frames[1] = (int16_t *)&dmic_ipro_dma_frames[stream->block_size];

	data->hal = (hal_auadc_pdm_cfg_t){
		.pdm_clk_pin = config->clk_pin,
		.pdm_in_pin = config->data_pin,
		.pdm_ch = config->channel,
		.pcm_frame_size = stream->block_size,
		.pcm_frame_buf = {data->frames[0], data->frames[1]},
		.pcm_frame_event = dmic_ipro_frame_event,
		.sample_rate = stream->pcm_rate,
	};
	cfg->channel.act_num_streams = 1;
	cfg->channel.act_num_chan = 1;
	cfg->channel.act_chan_map_lo = dmic_build_channel_map(0, 0, PDM_CHAN_LEFT);
	if (hal_auadc_pdm_init(&data->hal) != 0) {
		return -EIO;
	}
	hal_auadc_set_digital_gain((float)CONFIG_DMIC_IPRO_DIGITAL_GAIN_DB);
	data->state = DMIC_STATE_CONFIGURED;
	return 0;
}

static int dmic_ipro_trigger(const struct device *dev, enum dmic_trigger cmd)
{
	struct dmic_ipro_data *data = dev->data;
	void *block;

	switch (cmd) {
	case DMIC_TRIGGER_START:
		if (data->state != DMIC_STATE_CONFIGURED && data->state != DMIC_STATE_PAUSED) {
			return -EIO;
		}
		data->dropped = 0;
		data->discard = DMIC_IPRO_STARTUP_DISCARD;
		if (hal_auadc_start() != 0) {
			return -EIO;
		}
		data->state = DMIC_STATE_ACTIVE;
		return 0;
	case DMIC_TRIGGER_PAUSE:
		if (data->state != DMIC_STATE_ACTIVE) {
			return -EIO;
		}
		(void)hal_auadc_stop();
		data->state = DMIC_STATE_PAUSED;
		return 0;
	case DMIC_TRIGGER_STOP:
		if (data->state == DMIC_STATE_ACTIVE) {
			(void)hal_auadc_stop();
		}
		while (k_msgq_get(&data->queue, &block, K_NO_WAIT) == 0) {
			k_mem_slab_free(data->slab, block);
		}
		data->state = DMIC_STATE_CONFIGURED;
		return 0;
	case DMIC_TRIGGER_RELEASE:
		if (data->state == DMIC_STATE_ACTIVE) {
			(void)hal_auadc_stop();
		}
		hal_auadc_deinit();
		data->frames[0] = NULL;
		data->frames[1] = NULL;
		data->state = DMIC_STATE_INITIALIZED;
		return 0;
	case DMIC_TRIGGER_RESET:
		return dmic_ipro_trigger(dev, DMIC_TRIGGER_RELEASE);
	default:
		return -EINVAL;
	}
}

static int dmic_ipro_read(const struct device *dev, uint8_t stream, void **buffer, size_t *size,
			  int32_t timeout)
{
	struct dmic_ipro_data *data = dev->data;

	if (stream != 0U) {
		return -EINVAL;
	}
	if (data->state != DMIC_STATE_ACTIVE && k_msgq_num_used_get(&data->queue) == 0U) {
		return -EIO;
	}
	if (k_msgq_get(&data->queue, buffer, SYS_TIMEOUT_MS(timeout)) != 0) {
		return -EAGAIN;
	}
	*size = data->block_size;
	return 0;
}

static int dmic_ipro_init(const struct device *dev)
{
	struct dmic_ipro_data *data = dev->data;

	dmic_ipro_singleton = data;
	k_msgq_init(&data->queue, (char *)data->queue_buf, sizeof(void *), DMIC_IPRO_QUEUE_LEN);
	data->state = DMIC_STATE_INITIALIZED;
	return 0;
}

static const struct _dmic_ops dmic_ipro_api = {
	.configure = dmic_ipro_configure,
	.trigger = dmic_ipro_trigger,
	.read = dmic_ipro_read,
};

#define DMIC_IPRO_INIT(n)                                                                          \
	static const struct dmic_ipro_config dmic_ipro_cfg_##n = {                                 \
		.clk_pin = DT_INST_PROP(n, clk_pin),                                               \
		.data_pin = DT_INST_PROP(n, data_pin),                                             \
		.channel = DT_INST_ENUM_IDX(n, channel),                                           \
	};                                                                                         \
	static struct dmic_ipro_data dmic_ipro_data_##n;                                           \
	DEVICE_DT_INST_DEFINE(n, dmic_ipro_init, NULL, &dmic_ipro_data_##n, &dmic_ipro_cfg_##n,    \
			      POST_KERNEL, CONFIG_AUDIO_DMIC_INIT_PRIORITY, &dmic_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(DMIC_IPRO_INIT)
