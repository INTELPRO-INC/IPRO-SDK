/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * GPDAC DMA streaming (see ipro_dac_stream.h). Mirrors bsp/hal/src/hal_dac.c
 * + components/audio/src/audio_output_dac.c: GPDAC clock XCLK/63 with the
 * 1/16/32/64 divider for 32/16/8 kHz, DMA format 0 ({A0},{A1},...), 16-bit
 * DMA writes into GPIP_GPDAC_DMA_WDATA. Two contiguous segments play as a
 * hardware ping-pong loop (cyclic two-block transfer -> HAL LLI ring): the
 * controller moves on to the other segment by itself, the end-of-segment
 * callback only hands the finished one to the refill thread. No ISR
 * latency ever reaches the DAC FIFO (a synchronous printk line, ~0.7 ms
 * with interrupts locked, used to be an audible hiccup).
 */
#include <zephyr/kernel.h>
#include <zephyr/drivers/dma.h>
#include <ipro_zephyr/ipro_hal_glue.h>
#include <zephyr/logging/log.h>
#include <ipro_zephyr/ipro_dac_stream.h>
#include <soc.h>
#include "ipro_soc_regs.h"
#include "drv_glb.h"
#include "drv_dac.h"

LOG_MODULE_REGISTER(dac_ipro_stream, CONFIG_DAC_LOG_LEVEL);

#define GPDAC_DMA_WDATA (GPIP_BASE + 0x48)
#define DAC_SILENCE     512U
#define MAX_SEG         CONFIG_DAC_IPRO_STREAM_MAX_SEG_SAMPLES

static struct {
	struct ipro_dac_stream_cfg cfg;
	uint16_t seg[2 * MAX_SEG] __aligned(32); /* two contiguous halves */
	int16_t pcm[MAX_SEG];
	volatile uint8_t playing; /* segment the DMA is on */
	volatile bool running;
	volatile uint32_t segments;
	volatile uint32_t refills;
	struct k_sem refill_sem;
	struct k_thread refill_thread;
	k_tid_t refill_tid;
	volatile uint8_t to_fill;
} s;

K_THREAD_STACK_DEFINE(dac_refill_stack, CONFIG_DAC_IPRO_STREAM_THREAD_STACK);

static void seg_convert(uint8_t idx)
{
	/* signed 16-bit -> 10-bit unsigned around mid-scale, as the SDK does */
	uint16_t *seg = &s.seg[idx * s.cfg.seg_samples];

	s.cfg.fill(s.pcm, s.cfg.seg_samples, s.cfg.user);
	for (size_t i = 0; i < s.cfg.seg_samples; i++) {
		seg[i] = (uint16_t)((s.pcm[i] >> 6) + (int)DAC_SILENCE);
	}
	ipro_hal_cache_clean((uint32_t)seg, s.cfg.seg_samples * sizeof(uint16_t));
}

/*
 * Refill runs on its own high-priority thread, not the system work queue:
 * under a Bluetooth LE Audio load (host RX + LC3 decode) the work queue was
 * starved for seconds and the DMA kept replaying the initial segments.
 */
static void refill_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);
	while (true) {
		k_sem_take(&s.refill_sem, K_FOREVER);
		if (s.running) {
			seg_convert(s.to_fill);
			s.refills++;
		}
	}
}

static void dma_done(const struct device *dev, void *user, uint32_t ch, int status)
{
	ARG_UNUSED(user);
	if (!s.running) {
		return;
	}
	ARG_UNUSED(dev); ARG_UNUSED(ch);
	uint8_t done = s.playing;

	s.playing = done ^ 1U;
	s.segments++;
	s.to_fill = done;
	k_sem_give(&s.refill_sem);
	if (status < 0) {
		LOG_WRN("DMA error %d", status);
	}
}

int ipro_dac_stream_start(const struct ipro_dac_stream_cfg *cfg)
{
	DAC_CLK_Type div;

	if (cfg == NULL || cfg->fill == NULL || cfg->dma == NULL ||
	    cfg->seg_samples == 0 || cfg->seg_samples > MAX_SEG) {
		return -EINVAL;
	}
	switch (cfg->sample_rate) {
	case 8000: div = DAC_CLK_DIV_64; break;
	case 16000: div = DAC_CLK_DIV_32; break;
	case 32000: div = DAC_CLK_DIV_16; break;
	default: return -ENOTSUP;
	}
	if (s.running) {
		return -EBUSY;
	}
	s.cfg = *cfg;
	if (s.refill_tid == NULL) {
		k_sem_init(&s.refill_sem, 0, 1);
		s.refill_tid = k_thread_create(&s.refill_thread, dac_refill_stack,
					       K_THREAD_STACK_SIZEOF(dac_refill_stack), refill_thread,
					       NULL, NULL, NULL, CONFIG_DAC_IPRO_STREAM_THREAD_PRIO, 0,
					       K_NO_WAIT);
		k_thread_name_set(s.refill_tid, "dac_refill");
	}

	GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_GPIP);
	(void)GLB_Set_GPDAC_CLK_Sel(GLB_DAC_CLK_XCLK_32M, 62, 1);
	{
		GLB_GPIP_DAC_Cfg_Type d = {
			.refSel = GLB_DAC_REF_SEL_INTERNAL, .resetChanA = 1, .resetChanB = 1,
			.div = div, .dmaEn = 1, .dmaFmt = GPIP_DAC_DMA_FORMAT_0,
		};
		GLB_GPIP_DAC_ChanA_Cfg_Type a = {
			.chanCovtEn = 1, .outputEn = 1, .chanEn = 1, .src = GPIP_DAC_ChanA_SRC_DMA,
		};
		GLB_GPIP_DAC_ChanB_Cfg_Type b = {0};

		(void)GLB_GPIP_DAC_Init(&d);
		GLB_GPIP_DAC_Set_ChanA_Config(&a);
		GLB_GPIP_DAC_Set_ChanB_Config(&b);
	}

	seg_convert(0);
	seg_convert(1);
	s.playing = 0;
	s.segments = 0;
	s.refills = 0;
	k_sem_reset(&s.refill_sem);
	s.running = true;

	const uint32_t seg_bytes = cfg->seg_samples * sizeof(uint16_t);
	struct dma_block_config blk1 = {
		.source_address = (uint32_t)s.seg + seg_bytes,
		.dest_address = GPDAC_DMA_WDATA,
		.block_size = seg_bytes,
		.source_addr_adj = DMA_ADDR_ADJ_INCREMENT,
		.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE,
	};
	struct dma_block_config blk = {
		.source_address = (uint32_t)s.seg,
		.dest_address = GPDAC_DMA_WDATA,
		.block_size = seg_bytes,
		.source_addr_adj = DMA_ADDR_ADJ_INCREMENT,
		.dest_addr_adj = DMA_ADDR_ADJ_NO_CHANGE,
		.next_block = &blk1,
	};
	struct dma_config dcfg = {
		.cyclic = 1,
		.channel_direction = MEMORY_TO_PERIPHERAL,
		.dma_slot = CONFIG_DAC_IPRO_STREAM_DMA_REQ,
		.source_data_size = 2,
		.dest_data_size = 2,
		.source_burst_length = 16, /* INCR8 x 16-bit */
		.dest_burst_length = 2,
		.block_count = 2,
		.head_block = &blk,
		.dma_callback = dma_done,
	};
	int ret = dma_config(cfg->dma, cfg->dma_channel, &dcfg);

	if (ret == 0) {
		ret = dma_start(cfg->dma, cfg->dma_channel);
	}
	if (ret != 0) {
		s.running = false;
		LOG_ERR("DMA setup failed: %d", ret);
	}
	return ret;
}

void ipro_dac_stream_stop(void)
{
	if (!s.running) {
		return;
	}
	s.running = false;
	(void)dma_stop(s.cfg.dma, s.cfg.dma_channel);
	/* Park the output at mid-scale on the register source. */
	GLB_GPIP_DAC_ChanA_Cfg_Type a = {
		.chanCovtEn = 1, .outputEn = 1, .chanEn = 1, .src = GPIP_DAC_ChanA_SRC_REG,
	};
	GLB_GPIP_DAC_Set_ChanA_Config(&a);
	GLB_DAC_Set_ChanA_Value(DAC_SILENCE);
}

bool ipro_dac_stream_running(void)
{
	return s.running;
}

uint32_t ipro_dac_stream_segments(void)
{
	return s.segments;
}

uint32_t ipro_dac_stream_refills(void)
{
	return s.refills;
}
