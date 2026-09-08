/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#ifndef SDIO_DMA_COPY_H
#define SDIO_DMA_COPY_H

#include <stdint.h>

/*
 * DEBUG 2026-07-23: gated SDIO->WiFi TX-inject copy experiment.
 *
 * The inject worker copies each host frame (~1514B) from the SMID RX buffer
 * (uncached OCRAM alias 0x2102_xxxx) into a TX-pool pbuf payload in uncached
 * WRAM (0x2105_xxxx). The legacy path is a single arch_memcpy_fast, measured
 * ~46us/frame on the worker's critical path.
 *
 * Two alternative copy modes are measured against legacy, selected by
 * g_sdio_dmacpy_mode (0 legacy / 1 DMA-overlap / 2 cached-read):
 *   1  DMA0 (PL081, base 0x30081000, DRV_DMA0_ID ch7) moves the bulk while the
 *      CPU proceeds into the fhost inject (~228us); finish() waits (bounded)
 *      for DMA idle before any consumer reads the payload.
 *   2  CPU copy that READS through the OCRAM cached alias (src - 0x10000000)
 *      after a dcache invalidate-by-range; dst unchanged (arch_memcpy_fast).
 *
 * Everything is a strict no-behavior-change when g_sdio_dmacpy_mode == 0.
 */

extern volatile uint32_t g_sdio_dmacpy_mode;

/* Idempotent DMA channel bring-up; returns 0 ok, <0 on failure. */
int sdio_dmacpy_init(void);

/* Start the overlapped DMA copy of [src,src+len) -> [dst,dst+len).
 * Returns bytes handed to the DMA engine (the caller must call finish()
 * before reading dst); 0 means the whole copy was done on the CPU already
 * (small frame / alignment fallback / not inited) and no finish() is needed. */
uint32_t sdio_dmacpy_start(void *dst, const void *src, uint32_t len);

/* Wait (bounded) for the in-flight DMA to go idle; CPU-rescue + latch-off on
 * timeout so the frame is always correct. No-op if nothing is in flight. */
void sdio_dmacpy_finish(void);

/* mode 2 helper: invalidate the cached alias range then arch_memcpy_fast
 * reading through it (src - 0x10000000), dst unchanged. */
void sdio_dmacpy_cached_copy(void *dst, const void *src, uint32_t len);

/* Self-test: legacy vs cached-read vs DMA, byte-compare + per-mode timing. */
void sdio_dmacpy_selftest(void);

/* Dump the experiment counters; reset != 0 clears them. */
void sdio_dmacpy_stats(int reset);

/* Bus-politeness knobs (re-init the channel): burst 0..3 = 1/4/8/16 beats per
 * arbitration grant; width 1 = 16-bit, 2 = 32-bit transfer units. */
void sdio_dmacpy_set_burst(uint32_t b);
void sdio_dmacpy_set_width(uint32_t w);

#endif /* SDIO_DMA_COPY_H */
