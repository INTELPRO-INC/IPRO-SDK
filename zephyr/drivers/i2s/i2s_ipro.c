/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 I2S as a Zephyr I2S device (SDK HAL hal_i2s.c for the block
 * configuration, this file for the streaming): one direction each for TX and
 * RX, each fed by one DMA channel from the "dmas" property. Blocks come from
 * the application's memory slab; the DMA completion callback re-arms the
 * channel with the next block in place (dma_ipro's ISR-safe reload), so a
 * queue of blocks streams gap-free as long as the application keeps it
 * filled (TX) or drained (RX).
 *
 * UNVERIFIED ON HARDWARE (2026-09-21): the bench EVB has no codec or I2S
 * peripheral; this driver builds and follows the SDK's hal_i2s DMA usage.
 */

#define DT_DRV_COMPAT intelpro_ipro_i2s

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/logging/log.h>
#include <soc.h>

#include "ipro_soc_regs.h"
#include "hal_i2s.h"
#include "drv_i2s.h"
#include "drv_glb.h"

LOG_MODULE_REGISTER(i2s_ipro, CONFIG_I2S_LOG_LEVEL);

#define I2S_IPRO_QUEUE_LEN CONFIG_I2S_IPRO_QUEUE_LEN

struct i2s_ipro_stream {
	struct i2s_config cfg;
	enum i2s_state state;
	struct k_msgq queue; /* TX: blocks to send; RX: filled blocks */
	void *queue_buf[I2S_IPRO_QUEUE_LEN];
	void *active;     /* block the DMA is working on */
	uint8_t dma_ch;
	uint8_t dma_slot;
	bool stop_requested;
	bool drain_requested;
	bool configured;
};

struct i2s_ipro_config {
	const struct pinctrl_dev_config *pincfg;
	const struct device *dma_dev;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	uint8_t tx_dma_ch, tx_dma_slot, rx_dma_ch, rx_dma_slot;
	bool has_tx_dma, has_rx_dma;
};

struct i2s_ipro_data {
	const struct device *dev;
	struct i2s_ipro_stream tx, rx;
	bool hw_configured;
	uint16_t caps;
};

static int i2s_ipro_apply(const struct device *dev, const struct i2s_config *cfg)
{
	struct i2s_ipro_data *data = dev->data;
	i2s_device_t hal = {.id = 0, .fifo_threshold = 8};
	uint8_t fmt = cfg->format & I2S_FMT_DATA_FORMAT_MASK;

	switch (fmt) {
	case I2S_FMT_DATA_FORMAT_I2S:
		hal.interface_mode = I2S_MODE_STD;
		break;
	case I2S_FMT_DATA_FORMAT_LEFT_JUSTIFIED:
		hal.interface_mode = I2S_MODE_LEFT;
		break;
	case I2S_FMT_DATA_FORMAT_RIGHT_JUSTIFIED:
		hal.interface_mode = I2S_MODE_RIGHT;
		break;
	case I2S_FMT_DATA_FORMAT_PCM_SHORT:
		hal.interface_mode = I2S_MODE_DSP_A;
		break;
	case I2S_FMT_DATA_FORMAT_PCM_LONG:
		hal.interface_mode = I2S_MODE_DSP_B;
		break;
	default:
		return -EINVAL;
	}
	if (cfg->format & (I2S_FMT_DATA_ORDER_LSB | I2S_FMT_BIT_CLK_INV | I2S_FMT_FRAME_CLK_INV)) {
		return -ENOTSUP; /* the HAL fixes endian/polarity at build time */
	}

	switch (cfg->word_size) {
	case 8:
		hal.data_size = I2S_DATA_LEN_8;
		hal.frame_size = I2S_FRAME_LEN_16;
		break;
	case 16:
		hal.data_size = I2S_DATA_LEN_16;
		hal.frame_size = I2S_FRAME_LEN_16;
		break;
	case 24:
		hal.data_size = I2S_DATA_LEN_24;
		hal.frame_size = I2S_FRAME_LEN_32;
		break;
	case 32:
		hal.data_size = I2S_DATA_LEN_32;
		hal.frame_size = I2S_FRAME_LEN_32;
		break;
	default:
		return -EINVAL;
	}
	if (cfg->channels < 1 || cfg->channels > 4) {
		return -EINVAL;
	}
	hal.channel_num = cfg->channels;
	hal.sampl_freq_hz = cfg->frame_clk_freq;
	/* Both directions share the block: master unless either asks slave. */
	hal.iis_mode = (cfg->options & (I2S_OPT_FRAME_CLK_TARGET | I2S_OPT_BIT_CLK_TARGET))
			       ? I2S_MODE_SLAVE
			       : I2S_MODE_MASTER;

	hal_i2s_setConfig(&hal);
	if (hal_i2s_init(0, data->caps) != 0) {
		return -EIO;
	}
	data->hw_configured = true;
	return 0;
}

static struct i2s_ipro_stream *stream_of(struct i2s_ipro_data *data, enum i2s_dir dir)
{
	return dir == I2S_DIR_TX ? &data->tx : &data->rx;
}

static int i2s_ipro_configure(const struct device *dev, enum i2s_dir dir,
			      const struct i2s_config *cfg)
{
	const struct i2s_ipro_config *config = dev->config;
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s;
	int ret;

	if (dir == I2S_DIR_BOTH) {
		ret = i2s_ipro_configure(dev, I2S_DIR_TX, cfg);
		return ret != 0 ? ret : i2s_ipro_configure(dev, I2S_DIR_RX, cfg);
	}
	s = stream_of(data, dir);
	if (s->state != I2S_STATE_NOT_READY && s->state != I2S_STATE_READY) {
		return -EBUSY;
	}
	if ((dir == I2S_DIR_TX && !config->has_tx_dma) || (dir == I2S_DIR_RX && !config->has_rx_dma)) {
		return -ENOTSUP;
	}
	if (cfg->frame_clk_freq == 0U) {
		s->state = I2S_STATE_NOT_READY;
		s->configured = false;
		return 0;
	}
	if (cfg->mem_slab == NULL || cfg->block_size == 0U || (cfg->block_size & 3U) != 0U) {
		return -EINVAL;
	}
	ret = i2s_ipro_apply(dev, cfg);
	if (ret != 0) {
		return ret;
	}
	s->cfg = *cfg;
	s->configured = true;
	s->state = I2S_STATE_READY;
	return 0;
}

static const struct i2s_config *i2s_ipro_config_get(const struct device *dev, enum i2s_dir dir)
{
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = stream_of(data, dir);

	return s->configured ? &s->cfg : NULL;
}

static uint8_t width_bytes(const struct i2s_config *cfg)
{
	return cfg->word_size <= 8 ? 1 : cfg->word_size <= 16 ? 2 : 4;
}

static int stream_dma_arm(const struct device *dev, enum i2s_dir dir, void *block, bool first)
{
	const struct i2s_ipro_config *config = dev->config;
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = stream_of(data, dir);
	uint32_t fifo = (dir == I2S_DIR_TX) ? (I2S_BASE + I2S_FIFO_WDATA_OFFSET)
					      : (I2S_BASE + I2S_FIFO_RDATA_OFFSET);
	uint32_t src = dir == I2S_DIR_TX ? (uint32_t)block : fifo;
	uint32_t dst = dir == I2S_DIR_TX ? fifo : (uint32_t)block;
	extern void i2s_ipro_dma_cb(const struct device *, void *, uint32_t, int);
	struct dma_block_config blk = {
		.source_address = src,
		.dest_address = dst,
		.block_size = s->cfg.block_size,
		.source_addr_adj = dir == I2S_DIR_TX ? DMA_ADDR_ADJ_INCREMENT : DMA_ADDR_ADJ_NO_CHANGE,
		.dest_addr_adj = dir == I2S_DIR_TX ? DMA_ADDR_ADJ_NO_CHANGE : DMA_ADDR_ADJ_INCREMENT,
	};
	struct dma_config cfg = {
		.dma_slot = s->dma_slot,
		.channel_direction = dir == I2S_DIR_TX ? MEMORY_TO_PERIPHERAL : PERIPHERAL_TO_MEMORY,
		.source_data_size = width_bytes(&s->cfg),
		.dest_data_size = width_bytes(&s->cfg),
		.source_burst_length = width_bytes(&s->cfg),
		.dest_burst_length = width_bytes(&s->cfg),
		.block_count = 1,
		.head_block = &blk,
		.dma_callback = i2s_ipro_dma_cb,
		.user_data = (void *)(uintptr_t)((dir == I2S_DIR_TX ? 1U : 0U) | ((uintptr_t)dev << 1)),
	};
	int ret;

	s->active = block;
	if (!first) {
		return dma_reload(config->dma_dev, s->dma_ch, src, dst, s->cfg.block_size);
	}
	ret = dma_config(config->dma_dev, s->dma_ch, &cfg);
	if (ret == 0) {
		ret = dma_start(config->dma_dev, s->dma_ch);
	}
	return ret;
}

static void stream_stop(const struct device *dev, enum i2s_dir dir)
{
	const struct i2s_ipro_config *config = dev->config;
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = stream_of(data, dir);

	(void)dma_stop(config->dma_dev, s->dma_ch);
	if (s->active != NULL) {
		k_mem_slab_free(s->cfg.mem_slab, s->active);
		s->active = NULL;
	}
}

/* DMA completion: hand the finished block on and re-arm with the next one. */
void i2s_ipro_dma_cb(const struct device *dma, void *user, uint32_t ch, int status)
{
	const struct device *dev = (const struct device *)((uintptr_t)user >> 1);
	enum i2s_dir dir = ((uintptr_t)user & 1U) ? I2S_DIR_TX : I2S_DIR_RX;
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = stream_of(data, dir);
	void *next = NULL;

	ARG_UNUSED(dma);
	ARG_UNUSED(ch);
	if (status != 0) {
		s->state = I2S_STATE_ERROR;
		stream_stop(dev, dir);
		return;
	}

	if (dir == I2S_DIR_TX) {
		k_mem_slab_free(s->cfg.mem_slab, s->active);
		s->active = NULL;
		if (s->stop_requested || k_msgq_get(&s->queue, &next, K_NO_WAIT) != 0) {
			/* Nothing queued: DRAIN ends here, otherwise underrun. */
			s->state = (s->stop_requested || s->drain_requested) ? I2S_STATE_READY
									    : I2S_STATE_ERROR;
			s->stop_requested = false;
			s->drain_requested = false;
			return;
		}
	} else {
		if (k_msgq_put(&s->queue, &s->active, K_NO_WAIT) != 0) {
			k_mem_slab_free(s->cfg.mem_slab, s->active); /* overrun: drop */
			s->state = I2S_STATE_ERROR;
		}
		s->active = NULL;
		if (s->stop_requested || s->drain_requested ||
		    k_mem_slab_alloc(s->cfg.mem_slab, &next, K_NO_WAIT) != 0) {
			s->state = s->state == I2S_STATE_ERROR ? I2S_STATE_ERROR : I2S_STATE_READY;
			s->stop_requested = false;
			s->drain_requested = false;
			return;
		}
	}
	if (stream_dma_arm(dev, dir, next, false) != 0) {
		s->state = I2S_STATE_ERROR;
	}
}

static int i2s_ipro_trigger(const struct device *dev, enum i2s_dir dir, enum i2s_trigger_cmd cmd)
{
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s;
	void *block;
	int ret;

	if (dir == I2S_DIR_BOTH) {
		ret = i2s_ipro_trigger(dev, I2S_DIR_TX, cmd);
		return ret != 0 ? ret : i2s_ipro_trigger(dev, I2S_DIR_RX, cmd);
	}
	s = stream_of(data, dir);

	switch (cmd) {
	case I2S_TRIGGER_START:
		if (s->state != I2S_STATE_READY) {
			return -EIO;
		}
		if (dir == I2S_DIR_TX) {
			if (k_msgq_get(&s->queue, &block, K_NO_WAIT) != 0) {
				return -EIO; /* queue at least one block first */
			}
		} else if (k_mem_slab_alloc(s->cfg.mem_slab, &block, K_NO_WAIT) != 0) {
			return -ENOMEM;
		}
		s->stop_requested = false;
		s->drain_requested = false;
		ret = stream_dma_arm(dev, dir, block, true);
		if (ret != 0) {
			k_mem_slab_free(s->cfg.mem_slab, block);
			return ret;
		}
		s->state = I2S_STATE_RUNNING;
		return 0;
	case I2S_TRIGGER_STOP:
		if (s->state != I2S_STATE_RUNNING) {
			return -EIO;
		}
		s->stop_requested = true; /* current block finishes, then idle */
		s->state = I2S_STATE_STOPPING;
		return 0;
	case I2S_TRIGGER_DRAIN:
		if (s->state != I2S_STATE_RUNNING) {
			return -EIO;
		}
		s->drain_requested = true;
		s->state = I2S_STATE_STOPPING;
		return 0;
	case I2S_TRIGGER_DROP:
		if (s->state == I2S_STATE_NOT_READY) {
			return -EIO;
		}
		stream_stop(dev, dir);
		while (k_msgq_get(&s->queue, &block, K_NO_WAIT) == 0) {
			k_mem_slab_free(s->cfg.mem_slab, block);
		}
		s->state = I2S_STATE_READY;
		return 0;
	case I2S_TRIGGER_PREPARE:
		if (s->state != I2S_STATE_ERROR) {
			return -EIO;
		}
		stream_stop(dev, dir);
		while (k_msgq_get(&s->queue, &block, K_NO_WAIT) == 0) {
			k_mem_slab_free(s->cfg.mem_slab, block);
		}
		s->state = I2S_STATE_READY;
		return 0;
	default:
		return -EINVAL;
	}
}

static int i2s_ipro_read(const struct device *dev, void **mem_block, size_t *size)
{
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = &data->rx;

	if (s->state == I2S_STATE_NOT_READY) {
		return -EIO;
	}
	if (k_msgq_get(&s->queue, mem_block, SYS_TIMEOUT_MS(s->cfg.timeout)) != 0) {
		return s->state == I2S_STATE_ERROR ? -EIO : -EAGAIN;
	}
	*size = s->cfg.block_size;
	return 0;
}

static int i2s_ipro_write(const struct device *dev, void *mem_block, size_t size)
{
	struct i2s_ipro_data *data = dev->data;
	struct i2s_ipro_stream *s = &data->tx;

	if (s->state != I2S_STATE_RUNNING && s->state != I2S_STATE_READY) {
		return -EIO;
	}
	if (size != s->cfg.block_size) {
		return -EINVAL;
	}
	return k_msgq_put(&s->queue, &mem_block, SYS_TIMEOUT_MS(s->cfg.timeout)) == 0 ? 0 : -EAGAIN;
}

static int i2s_ipro_init(const struct device *dev)
{
	const struct i2s_ipro_config *config = dev->config;
	struct i2s_ipro_data *data = dev->data;
	int ret;

	data->dev = dev;
	ret = clock_control_on(config->clock_dev, config->clock_id);
	if (ret != 0) {
		return ret;
	}
	ret = pinctrl_apply_state(config->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret != 0) {
		return ret;
	}
	k_msgq_init(&data->tx.queue, (char *)data->tx.queue_buf, sizeof(void *), I2S_IPRO_QUEUE_LEN);
	k_msgq_init(&data->rx.queue, (char *)data->rx.queue_buf, sizeof(void *), I2S_IPRO_QUEUE_LEN);
	data->tx.dma_ch = config->tx_dma_ch;
	data->tx.dma_slot = config->tx_dma_slot;
	data->rx.dma_ch = config->rx_dma_ch;
	data->rx.dma_slot = config->rx_dma_slot;
	data->caps = (config->has_tx_dma ? COMMON_CAPS_DMA_TX : 0) |
		     (config->has_rx_dma ? COMMON_CAPS_DMA_RX : 0);
	data->tx.state = I2S_STATE_NOT_READY;
	data->rx.state = I2S_STATE_NOT_READY;
	return 0;
}

static DEVICE_API(i2s, i2s_ipro_api) = {
	.configure = i2s_ipro_configure,
	.config_get = i2s_ipro_config_get,
	.read = i2s_ipro_read,
	.write = i2s_ipro_write,
	.trigger = i2s_ipro_trigger,
};

#define I2S_DMA(n, name, cell)                                                                     \
	COND_CODE_1(DT_INST_DMAS_HAS_NAME(n, name), (DT_INST_DMAS_CELL_BY_NAME(n, name, cell)), (0))

#define I2S_IPRO_INIT(n)                                                                           \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	static const struct i2s_ipro_config i2s_ipro_cfg_##n = {                                   \
		.pincfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                       \
		.dma_dev = DEVICE_DT_GET(DT_INST_DMAS_CTLR_BY_IDX(n, 0)),                          \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, id),                    \
		.tx_dma_ch = I2S_DMA(n, tx, channel),                                              \
		.tx_dma_slot = I2S_DMA(n, tx, slot),                                               \
		.rx_dma_ch = I2S_DMA(n, rx, channel),                                              \
		.rx_dma_slot = I2S_DMA(n, rx, slot),                                               \
		.has_tx_dma = DT_INST_DMAS_HAS_NAME(n, tx),                                        \
		.has_rx_dma = DT_INST_DMAS_HAS_NAME(n, rx),                                        \
	};                                                                                         \
	static struct i2s_ipro_data i2s_ipro_data_##n;                                             \
	DEVICE_DT_INST_DEFINE(n, i2s_ipro_init, NULL, &i2s_ipro_data_##n, &i2s_ipro_cfg_##n,      \
			      POST_KERNEL, CONFIG_I2S_INIT_PRIORITY, &i2s_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(I2S_IPRO_INIT)
