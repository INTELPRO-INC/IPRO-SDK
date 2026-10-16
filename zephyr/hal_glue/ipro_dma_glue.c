/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * DMA facade over the SDK HAL (hal_dma.c). Pure SDK side: hal_dma.h's
 * dma_reload() shares its name with Zephyr's DMA API.
 */

#include <ipro_zephyr/ipro_hal_glue.h>

#include "hal_dma.h"
#include "drv_dma.h"
/* The SoC header must come first: it defines __DCACHE_PRESENT/__CCM_PRESENT
 * (from CFG_HAS_DCACHE/CFG_HAS_CCM), and core_feature_cache.h compiles the
 * flush helpers to empty inlines when they are not set yet (seen on IPRO6S:
 * DMA read stale RAM because the source never left the D-cache). */
#include "ipro_soc_regs.h"
#include "core_feature_cache.h"

static void (*chan_cb[DMA_MAX_INDEX])(uint8_t channel, bool error);
static dma_lli_ctrl_t *chan_lli[DMA_MAX_INDEX];
static uint32_t chan_dst[DMA_MAX_INDEX];
static size_t chan_size[DMA_MAX_INDEX];

/*
 * OCRAM is cacheable under Zephyr (no PMA split, see known_issues.md) and
 * the D-cache is on (CONFIG_IPRO7_DCACHE). hal_dma.c only cleans its LLI
 * descriptors; SDK apps clean/invalidate the data buffers themselves. Zephyr
 * callers expect the driver to keep DMA buffers coherent, so do it here:
 * clean the source and clean+invalidate the destination before the
 * transfer, and drop the destination lines again when it completes.
 * Observed without this: periph_smoke "dma mem2mem" completes but the
 * destination compares unequal.
 */
static inline void dma_cache_before(uint32_t src, uint32_t dst, size_t size)
{
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
	MFlushDCacheRange((uintptr_t)src, size);
	MFlushInvalDCacheRange((uintptr_t)dst, size);
#endif
}

static inline void dma_cache_after(uint32_t dst, size_t size)
{
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
	MFlushInvalDCacheRange((uintptr_t)dst, size);
#endif
}

static void hal_cb(void *dev, uint32_t event)
{
	dma_device_t *d = dev;

	if (d->ch < DMA_MAX_INDEX) {
		if (event == HAL_DMA_INT_COMPLETE) {
			dma_cache_after(chan_dst[d->ch], chan_size[d->ch]);
		}
		if (chan_cb[d->ch] != NULL) {
			chan_cb[d->ch](d->ch, event != HAL_DMA_INT_COMPLETE);
		}
	}
}

static uint8_t width_code(uint8_t bytes)
{
	switch (bytes) {
	case 2:
		return DMA_TRANSFER_WIDTH_16BIT;
	case 4:
		return DMA_TRANSFER_WIDTH_32BIT;
	default:
		return DMA_TRANSFER_WIDTH_8BIT;
	}
}

static uint8_t burst_code(uint8_t beats)
{
	if (beats >= 16) {
		return DMA_BURST_INCR16;
	} else if (beats >= 8) {
		return DMA_BURST_INCR8;
	} else if (beats >= 4) {
		return DMA_BURST_INCR4;
	}
	return DMA_BURST_INCR1;
}

int ipro_hal_dma_controller_init(void)
{
	dma_controller_init(DMA0_ID);
	return 0;
}

int ipro_hal_dma_channel_configure(const struct ipro_hal_dma_cfg *cfg)
{
	dma_device_t hal = {
		.id = DMA0_ID,
		.ch = cfg->channel,
		.transfer_mode = cfg->pingpong ? DMA_LLI_PINGPONG_MODE
				: cfg->cyclic ? DMA_LLI_CYCLE_MODE : DMA_LLI_ONCE_MODE,
		.src_req = cfg->src_req,
		.dst_req = cfg->dst_req,
		.src_addr_inc = cfg->src_inc ? DMA_ADDR_INCREMENT_ENABLE : DMA_ADDR_INCREMENT_DISABLE,
		.dst_addr_inc = cfg->dst_inc ? DMA_ADDR_INCREMENT_ENABLE : DMA_ADDR_INCREMENT_DISABLE,
		.src_burst_size = burst_code(cfg->src_burst_beats),
		.dst_burst_size = burst_code(cfg->dst_burst_beats),
		.src_width = width_code(cfg->src_width_bytes),
		.dst_width = width_code(cfg->dst_width_bytes),
		.lli_count = 0,
		.callback = hal_cb,
	};

	if (cfg->channel >= DMA_MAX_INDEX) {
		return -1;
	}
	switch (cfg->dir) {
	case IPRO_HAL_DMA_M2M:
		hal.direction = DMA_MEMORY_TO_MEMORY;
		break;
	case IPRO_HAL_DMA_M2P:
		hal.direction = DMA_MEMORY_TO_PERIPH;
		break;
	case IPRO_HAL_DMA_P2M:
		hal.direction = DMA_PERIPH_TO_MEMORY;
		break;
	default:
		return -1;
	}

	chan_cb[cfg->channel] = cfg->callback;
	dma_setConfig(&hal);
	if (dma_channel_attach(DMA0_ID, cfg->channel) != 0) {
		return -1;
	}
	dma_ioctl(DMA0_ID, cfg->channel, DMA_CTRL_SET_CALLBACK, hal_cb);
	dma_ioctl(DMA0_ID, cfg->channel, DMA_CTRL_SET_INT, NULL);
	return 0;
}

int ipro_hal_dma_channel_reload(uint8_t channel, uint32_t src, uint32_t dst, size_t size)
{
	if (channel >= DMA_MAX_INDEX) {
		return -1;
	}
	if (chan_lli[channel] != NULL) {
		/* dma_reload() would leak the previous chain: hand it back so
		 * the HAL's realloc() reuses it. */
		dma_device_t *d = dma_getDevice(DMA0_ID, channel);

		d->lli_cfg = chan_lli[channel];
		chan_lli[channel] = NULL;
	}
	chan_dst[channel] = dst;
	chan_size[channel] = size;
	dma_cache_before(src, dst, size);
	return dma_reload(DMA0_ID, channel, src, dst, size, &chan_lli[channel]);
}

int ipro_hal_dma_channel_start(uint8_t channel)
{
	if (channel >= DMA_MAX_INDEX || chan_lli[channel] == NULL) {
		return -1;
	}
	dma_channel_lli_update(DMA0_ID, channel, chan_lli[channel]);
	dma_channel_start(DMA0_ID, channel);
	return 0;
}

void ipro_hal_dma_channel_stop(uint8_t channel)
{
	if (channel < DMA_MAX_INDEX) {
		dma_channel_stop(DMA0_ID, channel);
	}
}

bool ipro_hal_dma_channel_busy(uint8_t channel)
{
	return channel < DMA_MAX_INDEX && dma_channel_check_busy(DMA0_ID, channel) != 0;
}

int ipro_hal_dma_channel_restart(uint8_t channel, uint32_t src, uint32_t dst, size_t size)
{
	dma_lli_ctrl_t *lli;
	dma_device_t *d;
	uint32_t shift;

	if (channel >= DMA_MAX_INDEX || size == 0U || size > DMA_MAX_TRANSFER_PER_LLI) {
		return -1;
	}
	/* The descriptor chain is owned by the last reload: one descriptor for
	 * a <= DMA_MAX_TRANSFER_PER_LLI single block. Patch it in place - no
	 * allocation, so this is safe from an ISR (a DMA completion handler
	 * re-arming the channel with the next buffer). */
	lli = chan_lli[channel];
	if (lli == NULL) {
		d = dma_getDevice(DMA0_ID, channel);
		lli = d != NULL ? d->lli_cfg : NULL;
	}
	if (lli == NULL || lli->nextlli != 0U) {
		return -1;
	}
	shift = (lli->cfg.bits.SWidth == DMA_TRANSFER_WIDTH_16BIT) ? 1U
		: (lli->cfg.bits.SWidth == DMA_TRANSFER_WIDTH_32BIT) ? 2U : 0U;
	if ((size >> shift) > DMA_MAX_TRANSFER_PER_LLI || (size & ((1U << shift) - 1U)) != 0U) {
		return -1;
	}

	DMA_Channel_Disable(DMA0_ID, channel);
	lli->src_addr = src;
	lli->dst_addr = dst;
	lli->cfg.bits.TransferSize = size >> shift;
	lli->cfg.bits.I = 1;
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
	MFlushDCacheRange((uintptr_t)lli, sizeof(*lli));
#endif
	chan_dst[channel] = dst;
	chan_size[channel] = size;
	dma_cache_before(src, dst, size);
	DMA_LLI_Update(DMA0_ID, channel, (uint32_t)lli);
	DMA_Channel_Enable(DMA0_ID, channel);
	return 0;
}

size_t ipro_hal_dma_channel_remaining(uint8_t channel)
{
	if (channel >= DMA_MAX_INDEX) {
		return 0;
	}
	if (!ipro_hal_dma_channel_busy(channel)) {
		return 0;
	}
	/* CxCONTROL.TransferSize counts down while the channel runs (PL08x
	 * semantics), in source-width units. */
	return DMA_Channel_TranferSize(DMA0_ID, channel);
}

void ipro_hal_dma_cache_invalidate(uint32_t addr, size_t size)
{
	dma_cache_after(addr, size);
}
