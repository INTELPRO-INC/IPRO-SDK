/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 DMA driver: Zephyr dma_driver_api -> ipro_hal_dma_* facade -> SDK
 * hal_dma.c -> drv_dma.c.
 *
 *   dma_config()  -> ipro_hal_dma_channel_configure() + _reload()
 *   dma_start()   -> ipro_hal_dma_channel_start() (LLI update + enable)
 *   dma_stop()    -> ipro_hal_dma_channel_stop()
 *   dma_reload()  -> ipro_hal_dma_channel_reload()
 *
 * One block per configuration (head_block only), or, for a cyclic
 * transfer, two equal contiguous blocks: that is the HAL's ping-pong mode
 * (DMA_LLI_PINGPONG_MODE), the descriptors loop over both halves by
 * themselves and the callback fires at the end of each half with
 * DMA_STATUS_BLOCK / DMA_STATUS_COMPLETE alternating. Other multi-block
 * lists are refused so callers use Zephyr's chained helpers. A cyclic
 * single block maps to DMA_LLI_CYCLE_MODE. The HAL's DMA0 ISR (registered
 * through the SDK interrupt seam) reports completion/error per channel.
 *
 * Cache: the HAL descriptors and the buffers handed to the controller live
 * in cacheable OCRAM; callers flush/invalidate (sys_cache_data_*) or use
 * the non-cacheable alias, as the SDK drivers do with L1C_DCACHE_*.
 */

#define DT_DRV_COMPAT intelpro_ipro_dma

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_hal_glue.h>

LOG_MODULE_REGISTER(dma_ipro, CONFIG_DMA_LOG_LEVEL);

#define DMA_IPRO_CHANNELS DT_INST_PROP(0, dma_channels)

struct dma_ipro_config {
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
};

struct dma_ipro_chan {
	dma_callback_t cb;
	void *user_data;
	uint32_t block_size;
	bool cyclic;
	bool pingpong;
	uint8_t half; /* ping-pong: half that completes next */
	bool configured;
	bool busy;
};

struct dma_ipro_data {
	struct dma_context ctx;
	ATOMIC_DEFINE(channels_atomic, DMA_IPRO_CHANNELS);
	struct dma_ipro_chan chan[DMA_IPRO_CHANNELS];
};

static const struct device *dma_ipro_dev;

static void dma_ipro_hal_cb(uint8_t channel, bool error)
{
	struct dma_ipro_data *data = dma_ipro_dev->data;
	struct dma_ipro_chan *c = &data->chan[channel];

	int status = DMA_STATUS_COMPLETE;

	if (error || !c->cyclic) {
		c->busy = false;
	}
	if (error) {
		status = -EIO;
	} else if (c->pingpong) {
		status = c->half == 0 ? DMA_STATUS_BLOCK : DMA_STATUS_COMPLETE;
		c->half ^= 1U;
	}
	if (c->cb != NULL) {
		c->cb(dma_ipro_dev, c->user_data, channel, status);
	}
}

static int dma_ipro_configure(const struct device *dev, uint32_t channel, struct dma_config *cfg)
{
	struct dma_ipro_data *data = dev->data;
	struct dma_ipro_chan *c;
	struct ipro_hal_dma_cfg hal = {
		.channel = channel,
		.callback = dma_ipro_hal_cb,
	};

	if (channel >= DMA_IPRO_CHANNELS || cfg == NULL || cfg->head_block == NULL) {
		return -EINVAL;
	}
	if (cfg->block_count == 2 && cfg->cyclic) {
		const struct dma_block_config *b0 = cfg->head_block, *b1 = b0->next_block;

		if (b1 == NULL || b1->block_size != b0->block_size ||
		    b1->source_address != b0->source_address + b0->block_size ||
		    b1->dest_address != b0->dest_address) {
			LOG_ERR("cyclic two-block transfers must be contiguous ping-pong halves");
			return -ENOTSUP;
		}
		hal.pingpong = true;
	} else if (cfg->block_count != 1) {
		LOG_ERR("only single-block (or cyclic ping-pong) transfers are supported");
		return -ENOTSUP;
	}
	c = &data->chan[channel];
	if (c->busy) {
		return -EBUSY;
	}
	if ((cfg->source_data_size != 1 && cfg->source_data_size != 2 && cfg->source_data_size != 4) ||
	    (cfg->dest_data_size != 1 && cfg->dest_data_size != 2 && cfg->dest_data_size != 4)) {
		return -EINVAL;
	}

	switch (cfg->channel_direction) {
	case MEMORY_TO_MEMORY:
		hal.dir = IPRO_HAL_DMA_M2M;
		break;
	case MEMORY_TO_PERIPHERAL:
		hal.dir = IPRO_HAL_DMA_M2P;
		hal.dst_req = cfg->dma_slot;
		break;
	case PERIPHERAL_TO_MEMORY:
		hal.dir = IPRO_HAL_DMA_P2M;
		hal.src_req = cfg->dma_slot;
		break;
	default:
		return -ENOTSUP;
	}

	hal.src_inc = cfg->head_block->source_addr_adj != DMA_ADDR_ADJ_NO_CHANGE &&
		      cfg->channel_direction != PERIPHERAL_TO_MEMORY;
	hal.dst_inc = cfg->head_block->dest_addr_adj != DMA_ADDR_ADJ_NO_CHANGE &&
		      cfg->channel_direction != MEMORY_TO_PERIPHERAL;
	hal.src_width_bytes = cfg->source_data_size;
	hal.dst_width_bytes = cfg->dest_data_size;
	hal.src_burst_beats = cfg->source_burst_length / cfg->source_data_size;
	hal.dst_burst_beats = cfg->dest_burst_length / cfg->dest_data_size;
	hal.cyclic = cfg->cyclic;

	c->cb = cfg->dma_callback;
	c->user_data = cfg->user_data;
	c->block_size = cfg->head_block->block_size;
	c->cyclic = cfg->cyclic;
	c->pingpong = hal.pingpong;
	c->half = 0;

	if (ipro_hal_dma_channel_configure(&hal) != 0) {
		return -EIO;
	}
	if (ipro_hal_dma_channel_reload(channel, cfg->head_block->source_address,
					cfg->head_block->dest_address,
					cfg->head_block->block_size) != 0) {
		return -EINVAL;
	}
	c->configured = true;
	return 0;
}

static int dma_ipro_reload(const struct device *dev, uint32_t channel, uint32_t src, uint32_t dst,
			   size_t size)
{
	struct dma_ipro_data *data = dev->data;

	if (channel >= DMA_IPRO_CHANNELS || !data->chan[channel].configured) {
		return -EINVAL;
	}
	if (data->chan[channel].busy) {
		return -EBUSY;
	}
	data->chan[channel].block_size = size;
	if (k_is_in_isr()) {
		/* The HAL reload allocates its descriptor chain (malloc lock);
		 * from an ISR only the in-place re-arm of a single-descriptor
		 * channel is possible. The channel is left started. */
		if (ipro_hal_dma_channel_restart(channel, src, dst, size) != 0) {
			return -EINVAL;
		}
		data->chan[channel].busy = true;
		return 0;
	}
	return ipro_hal_dma_channel_reload(channel, src, dst, size) == 0 ? 0 : -EINVAL;
}

static int dma_ipro_start(const struct device *dev, uint32_t channel)
{
	struct dma_ipro_data *data = dev->data;

	if (channel >= DMA_IPRO_CHANNELS || !data->chan[channel].configured) {
		return -EINVAL;
	}
	data->chan[channel].busy = true;
	if (ipro_hal_dma_channel_start(channel) != 0) {
		data->chan[channel].busy = false;
		return -EIO;
	}
	return 0;
}

static int dma_ipro_stop(const struct device *dev, uint32_t channel)
{
	struct dma_ipro_data *data = dev->data;

	if (channel >= DMA_IPRO_CHANNELS) {
		return -EINVAL;
	}
	ipro_hal_dma_channel_stop(channel);
	data->chan[channel].busy = false;
	return 0;
}

static int dma_ipro_get_status(const struct device *dev, uint32_t channel, struct dma_status *stat)
{
	ARG_UNUSED(dev);
	if (channel >= DMA_IPRO_CHANNELS) {
		return -EINVAL;
	}
	stat->busy = ipro_hal_dma_channel_busy(channel);
	stat->dir = 0;
	stat->pending_length = stat->busy ? ipro_hal_dma_channel_remaining(channel) : 0;
	return 0;
}

static bool dma_ipro_chan_filter(const struct device *dev, int channel, void *filter_param)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(filter_param);
	return channel >= 0 && channel < DMA_IPRO_CHANNELS;
}

static int dma_ipro_init(const struct device *dev)
{
	const struct dma_ipro_config *cfg = dev->config;
	struct dma_ipro_data *data = dev->data;
	int ret;

	dma_ipro_dev = dev;
	data->ctx.magic = DMA_MAGIC;
	data->ctx.dma_channels = DMA_IPRO_CHANNELS;
	data->ctx.atomic = data->channels_atomic;

	ret = clock_control_on(cfg->clock_dev, cfg->clock_id);
	if (ret != 0) {
		return ret;
	}
	return ipro_hal_dma_controller_init() == 0 ? 0 : -EIO;
}

static DEVICE_API(dma, dma_ipro_api) = {
	.config = dma_ipro_configure,
	.reload = dma_ipro_reload,
	.start = dma_ipro_start,
	.stop = dma_ipro_stop,
	.get_status = dma_ipro_get_status,
	.chan_filter = dma_ipro_chan_filter,
};

static const struct dma_ipro_config dma_ipro_cfg_0 = {
	.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(0)),
	.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(0, id),
};
static struct dma_ipro_data dma_ipro_data_0;

DEVICE_DT_INST_DEFINE(0, dma_ipro_init, NULL, &dma_ipro_data_0, &dma_ipro_cfg_0, PRE_KERNEL_1,
		      CONFIG_DMA_INIT_PRIORITY, &dma_ipro_api);
