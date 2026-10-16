/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 SPI driver: Zephyr spi_driver_api -> SDK hal_spi.c -> drv_spi.c.
 *
 * The HAL device descriptor (spi_device_t) is filled from the Zephyr
 * spi_config on every transceive that changes it and applied with
 * spi_init(); transfers use spi_transmit_receive() (polled, full duplex).
 * Frame size follows SPI_WORD_SET(): 8/16/24/32 bits, buffers hold one
 * frame per byte/half-word/word.
 */

#define DT_DRV_COMPAT intelpro_ipro_spi

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/spi/rtio.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/logging/log.h>
#include <soc.h>

LOG_MODULE_REGISTER(spi_ipro, CONFIG_SPI_LOG_LEVEL);

#include "spi_context.h"

#include "ipro_soc_regs.h"
#include "hal_spi.h"
#include "drv_spi.h"
#include "spi_reg.h"
#include <zephyr/drivers/dma.h>
#include <ipro_zephyr/ipro_hal_glue.h>

#define SPI_IPRO_MAX_FREQ 48000000U

struct spi_ipro_config {
	uint8_t id;
	uint32_t base;
	const struct pinctrl_dev_config *pincfg;
	const struct device *clock_dev;
	clock_control_subsys_t clock_id;
	bool swap_mosi_miso;
	uint8_t deglitch;
	/* Optional TX DMA (dmas = <&dma0 ch IPRO7_DMA_REQ_SPIn_TX>, dma-names "tx"):
	 * transmit-only chunks of CONFIG_SPI_IPRO_DMA_MIN_BYTES or more go by DMA. */
	const struct device *dma_dev;
	uint8_t tx_dma_ch;
	uint8_t tx_dma_slot;
};

struct spi_ipro_data {
	struct spi_context ctx;
#ifdef CONFIG_SPI_ASYNC
	struct k_work async_work;
	const struct device *dev;
#endif
	uint8_t dfs; /* bytes per frame */
	struct k_sem dma_done;
	int dma_status;
};

static void spi_ipro_dma_cb(const struct device *dma, void *user, uint32_t ch, int status)
{
	struct spi_ipro_data *data = user;

	ARG_UNUSED(dma); ARG_UNUSED(ch);
	data->dma_status = status;
	k_sem_give(&data->dma_done);
}

/*
 * Transmit-only chunk by DMA into the TX FIFO. The SPI keeps clocking data
 * into the RX FIFO meanwhile; it overflows harmlessly and is cleared after.
 * Returns -ENOTSUP when the chunk does not qualify (caller falls back to the
 * polled HAL transfer).
 */
static int spi_ipro_tx_dma(const struct device *dev, const void *tx, size_t len)
{
	const struct spi_ipro_config *config = dev->config;
	struct spi_ipro_data *data = dev->data;
	size_t bytes = len * data->dfs;
	struct dma_block_config blk = {
		.source_address = (uint32_t)tx,
		.dest_address = config->base + SPI_FIFO_WDATA_OFFSET,
		.block_size = bytes,
		.source_addr_adj = DMA_ADDR_ADJ_INCREMENT,
		.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE,
	};
	struct dma_config cfg = {
		.channel_direction = MEMORY_TO_PERIPHERAL,
		.dma_slot = config->tx_dma_slot,
		.source_data_size = data->dfs,
		.dest_data_size = data->dfs,
		.source_burst_length = data->dfs,
		.dest_burst_length = data->dfs,
		.block_count = 1,
		.head_block = &blk,
		.dma_callback = spi_ipro_dma_cb,
		.user_data = data,
	};
	uint32_t fifo0;
	int ret;

	if (config->dma_dev == NULL || bytes < CONFIG_SPI_IPRO_DMA_MIN_BYTES ||
	    !device_is_ready(config->dma_dev)) {
		return -ENOTSUP;
	}
	ipro_hal_cache_clean((uint32_t)tx, bytes);
	k_sem_reset(&data->dma_done);
	data->dma_status = 0;

	/* TX FIFO DMA request on (SPI_FIFO_CONFIG_0 bit 0). */
	fifo0 = sys_read32(config->base + SPI_FIFO_CONFIG_0_OFFSET);
	sys_write32(fifo0 | SPI_DMA_TX_EN_MSK, config->base + SPI_FIFO_CONFIG_0_OFFSET);

	ret = dma_config(config->dma_dev, config->tx_dma_ch, &cfg);
	if (ret == 0) {
		ret = dma_start(config->dma_dev, config->tx_dma_ch);
	}
	if (ret == 0 && k_sem_take(&data->dma_done, K_MSEC(1000 + bytes / 100)) != 0) {
		ret = -ETIMEDOUT;
	}
	if (ret == 0 && data->dma_status < 0) {
		ret = data->dma_status;
	}
	if (ret != 0) {
		(void)dma_stop(config->dma_dev, config->tx_dma_ch);
	}
	/* Drain: FIFO empty, then the shifter idle. */
	for (int i = 0; i < 100000 && SPI_GetTxFifoCount(config->id) < SPI_TX_FIFO_SIZE; i++) {
		k_busy_wait(1);
	}
	for (int i = 0; i < 100000 && SPI_GetBusyStatus(config->id); i++) {
		k_busy_wait(1);
	}
	sys_write32(fifo0 & ~SPI_DMA_TX_EN_MSK, config->base + SPI_FIFO_CONFIG_0_OFFSET);
	(void)SPI_ClrRxFifo(config->id);
	return ret;
}

static int spi_ipro_configure(const struct device *dev, const struct spi_config *cfg)
{
	const struct spi_ipro_config *config = dev->config;
	struct spi_ipro_data *data = dev->data;
	spi_device_t hal = {
		.id = config->id,
		.mode = SPI_MASTER_MODE,
		.fifo_threshold = 0,
		.pin_swap_enable = config->swap_mosi_miso,
		.delitch_cnt = config->deglitch,
		.ignore_cs = 0,
	};
	uint8_t word = SPI_WORD_SIZE_GET(cfg->operation);

	if (spi_context_configured(&data->ctx, cfg)) {
		return 0;
	}

	if (SPI_OP_MODE_GET(cfg->operation) != SPI_OP_MODE_MASTER) {
		LOG_ERR("slave mode not supported");
		return -ENOTSUP;
	}
	if (cfg->operation & SPI_HALF_DUPLEX) {
		return -ENOTSUP;
	}
	if (cfg->operation & SPI_MODE_LOOP) {
		return -ENOTSUP;
	}

	switch (word) {
	case 8:
		hal.datasize = SPI_DATASIZE_8BIT;
		data->dfs = 1;
		break;
	case 16:
		hal.datasize = SPI_DATASIZE_16BIT;
		data->dfs = 2;
		break;
	case 24:
		hal.datasize = SPI_DATASIZE_24BIT;
		data->dfs = 4;
		break;
	case 32:
		hal.datasize = SPI_DATASIZE_32BIT;
		data->dfs = 4;
		break;
	default:
		LOG_ERR("unsupported word size %u", word);
		return -ENOTSUP;
	}

	hal.direction = (cfg->operation & SPI_TRANSFER_LSB) ? SPI_LSB_BYTE0_DIRECTION_FIRST
							     : SPI_MSB_BYTE0_DIRECTION_FIRST;
	hal.clk_polaraity = (cfg->operation & SPI_MODE_CPOL) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
	hal.clk_phase = (cfg->operation & SPI_MODE_CPHA) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;

	if (cfg->frequency == 0 || cfg->frequency > SPI_IPRO_MAX_FREQ) {
		LOG_ERR("frequency %u out of range", cfg->frequency);
		return -EINVAL;
	}
	hal.clk = cfg->frequency;

	spi_setConfig(&hal);
	if (spi_init(config->id) != 0) {
		return -EIO;
	}

	data->ctx.config = cfg;
	return 0;
}

/* One full transfer over the buffer sets already installed in the context;
 * used by the synchronous path directly and by the async work item. */
static int spi_ipro_run(const struct device *dev)
{
	const struct spi_ipro_config *config = dev->config;
	struct spi_ipro_data *data = dev->data;
	struct spi_context *ctx = &data->ctx;
	int ret = 0;

	spi_context_cs_control(ctx, true);
	spi_enable(config->id, 1);

	/*
	 * Walk both buffer sets in lock step; the HAL call is one full-duplex
	 * chunk of frames. A missing side is fed from / dropped into a small
	 * scratch buffer so every chunk stays full duplex.
	 */
	while (spi_context_tx_on(ctx) || spi_context_rx_on(ctx)) {
		size_t tx_len = spi_context_tx_on(ctx) ? ctx->tx_len : 0;
		size_t rx_len = spi_context_rx_on(ctx) ? ctx->rx_len : 0;
		size_t len = (tx_len && rx_len) ? MIN(tx_len, rx_len) : MAX(tx_len, rx_len);
		uint8_t scratch[32] __aligned(4);

		if (len * data->dfs > sizeof(scratch) && (tx_len == 0 || rx_len == 0)) {
			len = sizeof(scratch) / data->dfs;
		}

		if (tx_len != 0 && rx_len == 0) {
			int r = spi_ipro_tx_dma(dev, ctx->tx_buf, tx_len);

			if (r == 0) {
				spi_context_update_tx(ctx, data->dfs, tx_len);
				continue;
			}
			if (r != -ENOTSUP) {
				ret = r;
				break;
			}
		}

		const void *tx = tx_len ? ctx->tx_buf : scratch;
		void *rx = rx_len ? ctx->rx_buf : scratch;

		if (tx_len == 0) {
			memset(scratch, 0xff, sizeof(scratch));
		}

		if (spi_transmit_receive(config->id, tx, rx, len) != 0) {
			ret = -EIO;
			break;
		}

		spi_context_update_tx(ctx, data->dfs, len);
		spi_context_update_rx(ctx, data->dfs, len);
	}

	spi_enable(config->id, 0);
	spi_context_cs_control(ctx, false);
	return ret;
}

static int spi_ipro_transceive_common(const struct device *dev, const struct spi_config *cfg,
				      const struct spi_buf_set *tx_bufs,
				      const struct spi_buf_set *rx_bufs, bool asynchronous,
				      spi_callback_t cb, void *userdata)
{
	struct spi_ipro_data *data = dev->data;
	struct spi_context *ctx = &data->ctx;
	int ret;

	spi_context_lock(ctx, asynchronous, cb, userdata, cfg);

	ret = spi_ipro_configure(dev, cfg);
	if (ret != 0) {
		goto out;
	}

	spi_context_buffers_setup(ctx, tx_bufs, rx_bufs, data->dfs);

#ifdef CONFIG_SPI_ASYNC
	if (asynchronous) {
		/* The HAL transfer is blocking: run it on the system work queue
		 * and complete the context (callback) from there. The buffer
		 * sets must stay valid until the callback, as the API requires. */
		k_work_submit(&data->async_work);
		ret = spi_context_wait_for_completion(ctx);
		goto out;
	}
#endif
	ret = spi_ipro_run(dev);

out:
	spi_context_release(ctx, ret);
	return ret;
}

static int spi_ipro_transceive(const struct device *dev, const struct spi_config *cfg,
			       const struct spi_buf_set *tx_bufs, const struct spi_buf_set *rx_bufs)
{
	return spi_ipro_transceive_common(dev, cfg, tx_bufs, rx_bufs, false, NULL, NULL);
}

#ifdef CONFIG_SPI_ASYNC
static void spi_ipro_async_work(struct k_work *work)
{
	struct spi_ipro_data *data = CONTAINER_OF(work, struct spi_ipro_data, async_work);
	int ret = spi_ipro_run(data->dev);

	spi_context_complete(&data->ctx, data->dev, ret);
}

static int spi_ipro_transceive_async(const struct device *dev, const struct spi_config *cfg,
				     const struct spi_buf_set *tx_bufs,
				     const struct spi_buf_set *rx_bufs, spi_callback_t cb,
				     void *userdata)
{
	return spi_ipro_transceive_common(dev, cfg, tx_bufs, rx_bufs, true, cb, userdata);
}
#endif

static int spi_ipro_release(const struct device *dev, const struct spi_config *cfg)
{
	struct spi_ipro_data *data = dev->data;

	ARG_UNUSED(cfg);
	spi_context_unlock_unconditionally(&data->ctx);
	return 0;
}

static int spi_ipro_init(const struct device *dev)
{
	const struct spi_ipro_config *config = dev->config;
	struct spi_ipro_data *data = dev->data;
	int ret;

	k_sem_init(&data->dma_done, 0, 1);
	ret = clock_control_on(config->clock_dev, config->clock_id);
	if (ret != 0) {
		return ret;
	}
	ret = pinctrl_apply_state(config->pincfg, PINCTRL_STATE_DEFAULT);
	if (ret != 0) {
		return ret;
	}
	ret = spi_context_cs_configure_all(&data->ctx);
	if (ret != 0) {
		return ret;
	}
#ifdef CONFIG_SPI_ASYNC
	data->dev = dev;
	k_work_init(&data->async_work, spi_ipro_async_work);
#endif
	spi_context_unlock_unconditionally(&data->ctx);
	return 0;
}

static DEVICE_API(spi, spi_ipro_api) = {
	.transceive = spi_ipro_transceive,
#ifdef CONFIG_SPI_ASYNC
	.transceive_async = spi_ipro_transceive_async,
#endif
#ifdef CONFIG_SPI_RTIO
	.iodev_submit = spi_rtio_iodev_default_submit,
#endif
	.release = spi_ipro_release,
};

#define SPI_IPRO_INIT(n)                                                                           \
	PINCTRL_DT_INST_DEFINE(n);                                                                 \
	static const struct spi_ipro_config spi_ipro_cfg_##n = {                                   \
		.id = DT_INST_REG_ADDR(n) == SPI_BASE ? 0 : 1,                                     \
		.base = DT_INST_REG_ADDR(n),                                                       \
		.pincfg = PINCTRL_DT_INST_DEV_CONFIG_GET(n),                                       \
		.clock_dev = DEVICE_DT_GET(DT_INST_CLOCKS_CTLR(n)),                                \
		.clock_id = (clock_control_subsys_t)DT_INST_CLOCKS_CELL(n, id),                    \
		.swap_mosi_miso = DT_INST_PROP(n, swap_mosi_miso),                                 \
		.deglitch = DT_INST_PROP(n, deglitch_count),                                       \
		.dma_dev = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                             \
				       (DEVICE_DT_GET(DT_INST_DMAS_CTLR_BY_NAME(n, tx))), (NULL)), \
		.tx_dma_ch = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                           \
					 (DT_INST_DMAS_CELL_BY_NAME(n, tx, channel)), (0)),        \
		.tx_dma_slot = COND_CODE_1(DT_INST_NODE_HAS_PROP(n, dmas),                         \
					   (DT_INST_DMAS_CELL_BY_NAME(n, tx, slot)), (0)),         \
	};                                                                                         \
	static struct spi_ipro_data spi_ipro_data_##n = {                                          \
		SPI_CONTEXT_INIT_LOCK(spi_ipro_data_##n, ctx),                                     \
		SPI_CONTEXT_INIT_SYNC(spi_ipro_data_##n, ctx),                                     \
		SPI_CONTEXT_CS_GPIOS_INITIALIZE(DT_DRV_INST(n), ctx)};                             \
	SPI_DEVICE_DT_INST_DEFINE(n, spi_ipro_init, NULL, &spi_ipro_data_##n, &spi_ipro_cfg_##n,   \
				  POST_KERNEL, CONFIG_SPI_INIT_PRIORITY, &spi_ipro_api);

DT_INST_FOREACH_STATUS_OKAY(SPI_IPRO_INIT)
