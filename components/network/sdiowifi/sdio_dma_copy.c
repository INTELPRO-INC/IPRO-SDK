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

/*
 * DEBUG 2026-07-23: gated SDIO->WiFi TX-inject copy experiment.
 * See sdio_dma_copy.h for the design summary. Strict no-behavior-change when
 * g_sdio_dmacpy_mode == 0.
 *
 * IPRO6-only: modes 1/2 use the ipro6_soc DMA0 (PL081) driver and the OCRAM
 * cached/uncached alias trick, neither of which exist on IPRO6LE. The gate
 * symbol and legacy-equivalent stubs are still provided on IPRO6LE so
 * net_wifi_transceiver.c links unchanged there.
 */

#include <stdint.h>
#include <string.h>

#include "sdio_dma_copy.h"

/* Gate: 0 legacy / 1 DMA-overlap / 2 cached-read. Volatile — poked live from
 * the CLI while traffic runs. */
volatile uint32_t g_sdio_dmacpy_mode = 0;

/* Lib/ROM word-copy already used on this inject path (net_wifi_transceiver.c). */
extern void *arch_memcpy_fast(void *dst, const void *src, uint32_t n);

#ifdef IPRO6

#include "drv_dma.h"
#include "drv_glb.h"
#include "drv_l1c.h"
#include "hostrouter_log.h"      /* HR_LOGI / HR_LOGE */
#include <hal_mtimer.h>          /* mtimer_get_time_us (64-bit us) */

/* Register-probed 2026-07-23 on IPRO6 DUT: DMA0 CH0-CH3 register blocks are
 * alive, CH4-CH7 read back 0 after any write (per-channel register clock stays
 * gated; GLB_Set_DMA_CLK sets GLB_DMA_CFG0[24+ch] but that does not ungate the
 * upper four on this chip). Use CH3 — top of the working range, clear of the
 * peripheral_demo UART test (CH0/CH1). */
#define DMACPY_CH                 3            /* DMA0 channel 3 */
#define DMACPY_CACHED_ALIAS_OFFS  0x10000000u  /* uncached OCRAM alias - this = cached alias */
#define DMACPY_MIN_LEN            256u         /* below this the DMA setup is not worth it */
#define DMACPY_HEAD               64u          /* CPU-copied head (covers eth hdr + DSCP) */
#define DMACPY_FINISH_MAX_LOOPS   50000u       /* bounded idle poll */
#define DMACPY_PREV_MAX_LOOPS     2000u        /* bounded stale-DMA drain */

/* ------------------------------------------------------------------------- */
/* State                                                                     */
/* ------------------------------------------------------------------------- */
/* Blind-write fast start (2026-07-23 bench finding): DMA_Channel_Update_* +
 * DMA_Channel_Enable each do an uncached APB read-modify-write at 0x3008xxxx;
 * the two reads alone cost ~15us/frame (D34 bucket 25us with them, ~46us
 * legacy). CONTROL/CONFIG are constant except TRANSFERSIZE and the enable bit,
 * so init captures both words once and the per-frame path is 4 posted writes:
 * SRC, DST, CONTROL|size, CONFIG|E. Channel register block (probed): ch base =
 * DMA_BASE + 0x100 + ch*0x100; SRC +0x0, DST +0x4, CONTROL +0xC, CONFIG +0x10;
 * CONFIG.E = bit0, CONTROL.TRANSFERSIZE = bits[11:0]. */
#define DMACPY_CH_BASE   (0x30081000u + 0x100u + DMACPY_CH * 0x100u)
#define DMACPY_REG(off)  (*(volatile uint32_t *)(DMACPY_CH_BASE + (off)))
static uint32_t          s_ctrl_tmpl       = 0;   /* CONTROL with size field 0 */
static uint32_t          s_cfg_en          = 0;   /* CONFIG with E set         */

/* Bus-politeness knobs (2026-07-23 A/B finding: DMA mode LOWERS served
 * throughput while cutting inject cost — suspected WRAM/OCRAM port contention
 * with the MAC HW MPDU fetch during the air phase and with SDHCI ADMA. The DMA
 * latency is fully hidden behind the fhost inject (finish waits 0 loops), so a
 * slower, finer-grained DMA costs nothing — burst/width are runtime-tunable to
 * find the polite setting: `dmacpy b 0-3` (burst 1/4/8/16), `dmacpy w 1|2`
 * (16/32-bit). Width 32 applies to every production frame observed so far
 * (frame at buff+8 -> m==0; ctr_cpu_misalign 0 across ~30k frames). */
/* Defaults = the bench winner (2026-07-23 sweep): burst4 + 32-bit. burst16
 * LOST throughput 4/4 (-10%) despite the cheaper inject; burst4 flipped it to
 * +5..11%; burst1 over-fragments (loses again). 32-bit + burst4 = 16B grants,
 * fast enough to stay hidden (wait_loops ~0/40k frames) and polite enough not
 * to starve the MAC HW MPDU fetch / SDHCI ADMA on the same fabric. */
static uint32_t          s_burst           = DMA_BURST_SIZE_4;
static uint32_t          s_width           = DMA_TRNS_WIDTH_32BITS;
static uint32_t          s_unit            = 4;   /* bytes per transfer unit  */

static uint32_t          s_inited          = 0;
static volatile uint32_t s_dma_failed      = 0;   /* latched hard-fault; forces mode 0 */
static volatile uint32_t s_inflight        = 0;
static void             *s_fly_dst         = 0;   /* rescue: DMA middle destination */
static const void       *s_fly_src         = 0;   /* rescue: DMA middle source */
static volatile uint32_t s_fly_len         = 0;   /* rescue: DMA middle byte count */
static volatile uint32_t s_fly_off         = 0;   /* rescue: head offset of the middle */
static volatile uint32_t s_last_wait_loops = 0;   /* last finish() poll count (selftest) */

static volatile uint32_t ctr_dma_starts    = 0;
static volatile uint32_t ctr_cpu_small     = 0;   /* small/uninit -> full CPU copy */
static volatile uint32_t ctr_cpu_misalign  = 0;   /* odd dst alignment -> full CPU copy */
static volatile uint32_t ctr_timeout       = 0;
static volatile uint32_t ctr_finish_n      = 0;
static volatile uint32_t ctr_wait_loops_sum = 0;
static volatile uint32_t ctr_wait_prev     = 0;

/* ------------------------------------------------------------------------- */
int sdio_dmacpy_init(void)
{
    DMA_Channel_Cfg_Type cfg;

    /* Idempotent. Re-init clears the hard-fault latch so a CLI `dmacpy 1`
     * after a rescued timeout can re-arm the experiment. The burst/width knobs
     * clear s_inited to force a re-init through here. */
    s_dma_failed = 0;
    if (s_inited)
        return 0;

    GLB_Set_DMA_CLK(1, (uint8_t)(GLB_DMA0_CLK_CH0 + DMACPY_CH));
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_DMA_0);
    DMA_Enable(DRV_DMA0_ID);
    DMA_IntMask(DRV_DMA0_ID, DMACPY_CH, DMA_INT_ALL, MASK);   /* poll, never IRQ */
    DMA_Channel_Disable(DRV_DMA0_ID, DMACPY_CH);

    memset(&cfg, 0, sizeof(cfg));
    cfg.ch          = (DMA_Chan_Type)DMACPY_CH;
    cfg.dir         = DMA_TRNS_M2M;
    cfg.srcPeriph   = DMA_REQ_NONE;
    cfg.dstPeriph   = DMA_REQ_NONE;
    cfg.srcAddrInc  = 1;
    cfg.destAddrInc = 1;
    cfg.srcBurstSize = s_burst;
    cfg.dstBurstSize = s_burst;
    cfg.srcTransfWidth = s_width;
    cfg.dstTransfWidth = s_width;
    s_unit = (s_width == DMA_TRNS_WIDTH_32BITS) ? 4u : 2u;
    cfg.transfLength   = 0;
    cfg.srcDmaAddr     = 0;
    cfg.destDmaAddr    = 0;
    DMA_Channel_Init(DRV_DMA0_ID, &cfg);

    /* Capture the blind-write templates: CONTROL as programmed by Channel_Init
     * (transfLength 0), CONFIG in its disabled state + the enable bit. */
    s_ctrl_tmpl = DMACPY_REG(0x0C) & ~0xFFFu;   /* clear TRANSFERSIZE[11:0] */
    s_cfg_en    = (DMACPY_REG(0x10) & ~1u) | 1u;

    s_inflight = 0;
    s_fly_len  = 0;
    s_inited   = 1;
    return 0;
}

/* ------------------------------------------------------------------------- */
uint32_t sdio_dmacpy_start(void *dst, const void *src, uint32_t len)
{
    uintptr_t s = (uintptr_t)src;
    uintptr_t d = (uintptr_t)dst;
    uint32_t n_head, n_tail, mid, m;

    if (!s_inited || s_dma_failed || len < DMACPY_MIN_LEN) {
        arch_memcpy_fast(dst, src, len);
        ctr_cpu_small++;
        return 0;
    }

    /* Grow the CPU-copied head until src+n_head is word-aligned, so the DMA
     * middle is a clean halfword stream from a word-aligned source. */
    n_head = DMACPY_HEAD;
    n_head += (4u - ((uint32_t)(s + n_head) & 3u)) & 3u;

    /* Width alignment: src+n_head is word-aligned; the dst offset decides.
     * 16-bit width serves any even offset; 32-bit width needs offset 0 (every
     * production frame observed: frame at buff+8 -> m==0). Unserved -> CPU. */
    m = (uint32_t)((d + n_head) & 3u);
    if ((m & 1u) || (s_unit == 4u && m != 0u)) {
        arch_memcpy_fast(dst, src, len);
        ctr_cpu_misalign++;
        return 0;
    }

    mid    = len - n_head;
    n_tail = mid % s_unit;        /* trailing sub-unit bytes, if any */
    mid   -= n_tail;

    /* A previous DMA should already be idle (finish() runs before this worker
     * yields). Guard anyway: bounded drain + CPU rescue of the stale middle. */
    if (s_inflight) {
        uint32_t g = 0;
        while (DMA_Channel_Is_Busy(DRV_DMA0_ID, DMACPY_CH) == SET) {
            if (++g >= DMACPY_PREV_MAX_LOOPS) {
                DMA_Channel_Disable(DRV_DMA0_ID, DMACPY_CH);
                arch_memcpy_fast((uint8_t *)s_fly_dst + s_fly_off,
                                 (const uint8_t *)s_fly_src + s_fly_off, s_fly_len);
                break;
            }
        }
        ctr_wait_prev++;
        s_inflight = 0;
    }

    /* CPU does the head now (and any odd tail byte); DMA does the middle. */
    arch_memcpy_fast(dst, src, n_head);
    if (n_tail)
        memcpy((uint8_t *)dst + n_head + mid,
               (const uint8_t *)src + n_head + mid, n_tail);

    s_fly_dst  = dst;
    s_fly_src  = src;
    s_fly_len  = mid;
    s_fly_off  = n_head;
    s_inflight = 1;

    /* Blind-write fast start: 4 posted APB writes, no reads (see template
     * capture in init). */
    DMACPY_REG(0x00) = (uint32_t)(s + n_head);                    /* SRCADDR  */
    DMACPY_REG(0x04) = (uint32_t)(d + n_head);                    /* DSTADDR  */
    DMACPY_REG(0x0C) = s_ctrl_tmpl | (mid / s_unit);              /* CONTROL  */
    DMACPY_REG(0x10) = s_cfg_en;                                  /* CONFIG.E */
    ctr_dma_starts++;

    return mid;
}

/* ------------------------------------------------------------------------- */
void sdio_dmacpy_finish(void)
{
    uint32_t loops = 0;

    if (!s_inflight)
        return;

    /* DMA_Channel_Is_Busy returns SET while the channel-enable bit is set (the
     * PL08x self-clears it on completion); poll until RESET, bounded. Only a
     * cheap loop counter is accumulated here — no timer calls on this path. */
    while (DMA_Channel_Is_Busy(DRV_DMA0_ID, DMACPY_CH) == SET) {
        if (++loops >= DMACPY_FINISH_MAX_LOOPS) {
            DMA_Channel_Disable(DRV_DMA0_ID, DMACPY_CH);
            /* Rescue: redo the DMA middle on the CPU so the frame is correct,
             * then latch the experiment off (one error line). */
            arch_memcpy_fast((uint8_t *)s_fly_dst + s_fly_off,
                             (const uint8_t *)s_fly_src + s_fly_off, s_fly_len);
            ctr_timeout++;
            s_dma_failed = 1;
            g_sdio_dmacpy_mode = 0;
            HR_LOGE("sdio_dmacpy: DMA idle timeout -> CPU-rescued, latched off\r\n");
            break;
        }
    }

    ctr_wait_loops_sum += loops;
    ctr_finish_n++;
    s_last_wait_loops = loops;
    s_inflight = 0;
    s_fly_len  = 0;
}

/* ------------------------------------------------------------------------- */
void sdio_dmacpy_set_burst(uint32_t b)
{
    if (b > DMA_BURST_SIZE_16)
        b = DMA_BURST_SIZE_16;
    s_burst  = b;
    s_inited = 0;
    sdio_dmacpy_init();
    HR_LOGI("dmacpy burst=%lu (0/1/2/3 = 1/4/8/16 beats)\r\n", (unsigned long)b);
}

void sdio_dmacpy_set_width(uint32_t w)
{
    s_width  = (w == 2u) ? DMA_TRNS_WIDTH_32BITS : DMA_TRNS_WIDTH_16BITS;
    s_inited = 0;
    sdio_dmacpy_init();
    HR_LOGI("dmacpy width=%s\r\n", (s_width == DMA_TRNS_WIDTH_32BITS) ? "32" : "16");
}

/* ------------------------------------------------------------------------- */
void sdio_dmacpy_cached_copy(void *dst, const void *src, uint32_t len)
{
    /* SMID RX buffers are only ever written by SDIO ADMA and read by the CPU,
     * never CPU-written through the cached alias, so invalidate-by-range can
     * never discard dirty data. The 32B align-out stays inside the (32B-aligned)
     * RX buffer, so it cannot touch a neighbour's dirty line either. */
    uintptr_t c  = (uintptr_t)src - DMACPY_CACHED_ALIAS_OFFS;
    uintptr_t a0 = c & ~31u;
    uint32_t  inv_len = (uint32_t)(((c + len + 31u) & ~31u) - a0);

    csi_dcache_invalid_range((void *)a0, inv_len);
    arch_memcpy_fast(dst, (const void *)c, len);
}

/* ------------------------------------------------------------------------- */
/* Self-test: needs a WRAM destination (a real TX-pool pbuf payload) and an
 * uncached OCRAM source (.noncacheable collects into the .nocache_ram output
 * section -> 0x2102/0x2103_xxxx alias on IPRO6, same region as smid_rx_buf;
 * map-verified 2026-07-23). tx_buf.h must
 * precede mgmr_type.h: the latter embeds struct sdiowifi_tx_buf by value. */
#include "sdiowifi_tx_buf.h"
#include "sdiowifi_mgmr_type.h"
#include <lwip/pbuf.h>

/* OFF BY DEFAULT since the mem-32K campaign: st_src is 1.6 KB of nocache OCRAM
 * held for the lifetime of the image so that a bring-up self-test can run on
 * demand. Build it with -DSDIO_DMACPY_SELFTEST; the `dmacpy selftest` shell
 * command stays present either way and says so. */
#ifndef SDIO_DMACPY_SELFTEST
void sdio_dmacpy_selftest(void)
{
    HR_LOGI("dmacpy selftest not built (rebuild with -DSDIO_DMACPY_SELFTEST)\r\n");
}
#else
void sdio_dmacpy_selftest(void)
{
    static uint8_t __attribute__((section(".noncacheable"), aligned(32))) st_src[1600];
    static uint32_t s_run = 0;
    const uint32_t TEST_LEN = 1514;
    struct pbuf *p;
    uint8_t *dst;
    uint64_t t0, t1;
    uint32_t i, bad, dma_bytes;

    extern struct pbuf *sdiowifi_tx_buf_alloc(struct sdiowifi_tx_buf *tb);
    extern sdiowifi_mgmr_t g_sdiowifi_mgmr;

    sdio_dmacpy_init();
    s_run++;

    p = sdiowifi_tx_buf_alloc(&g_sdiowifi_mgmr.tx_desc);
    if (!p) {
        HR_LOGE("sdio_dmacpy selftest: no tx pbuf\r\n");
        return;
    }
    /* Pool pbufs are exactly TX_PBUF_FRAME_LEN (1514) — require TEST_LEN only. */
    if (p->tot_len < TEST_LEN) {
        HR_LOGE("sdio_dmacpy selftest: pbuf too small (%u)\r\n", (unsigned)p->tot_len);
        pbuf_free(p);
        return;
    }
    dst = (uint8_t *)p->payload;

    /* 1. counting pattern, offset by the run counter so repeated runs differ */
    for (i = 0; i < sizeof(st_src); i++)
        st_src[i] = (uint8_t)(i + s_run);

    /* 2. legacy arch_memcpy_fast baseline */
    t0 = mtimer_get_time_us();
    arch_memcpy_fast(dst, st_src, TEST_LEN);
    t1 = mtimer_get_time_us();
    HR_LOGI("selftest legacy : %llu us\r\n", (unsigned long long)(t1 - t0));

    /* 3. mode-2 cached-read + byte-compare */
    memset(dst, 0, TEST_LEN);
    t0 = mtimer_get_time_us();
    sdio_dmacpy_cached_copy(dst, st_src, TEST_LEN);
    t1 = mtimer_get_time_us();
    for (bad = 0, i = 0; i < TEST_LEN; i++)
        if (dst[i] != st_src[i]) bad++;
    HR_LOGI("selftest cached : %llu us  %s (bad=%lu)\r\n",
            (unsigned long long)(t1 - t0), bad ? "FAIL" : "PASS", (unsigned long)bad);

    /* 4. mode-1 DMA with a fresh pattern: start + immediate finish */
    for (i = 0; i < sizeof(st_src); i++)
        st_src[i] = (uint8_t)((i * 3u) + s_run + 0x5A);
    memset(dst, 0, TEST_LEN);
    t0 = mtimer_get_time_us();
    dma_bytes = sdio_dmacpy_start(dst, st_src, TEST_LEN);
    sdio_dmacpy_finish();
    t1 = mtimer_get_time_us();
    for (bad = 0, i = 0; i < TEST_LEN; i++)
        if (dst[i] != st_src[i]) bad++;
    HR_LOGI("selftest dma    : %llu us  dma_bytes=%lu wait_loops=%lu  %s (bad=%lu)\r\n",
            (unsigned long long)(t1 - t0), (unsigned long)dma_bytes,
            (unsigned long)s_last_wait_loops, bad ? "FAIL" : "PASS", (unsigned long)bad);
    if (bad) { s_dma_failed = 1; g_sdio_dmacpy_mode = 0; }

    /* 5. head-align path: src at +2 (real frames land 2-byte off word) */
    for (i = 0; i < sizeof(st_src); i++)
        st_src[i] = (uint8_t)((i * 7u) + s_run + 0xA5);
    memset(dst, 0, TEST_LEN);
    dma_bytes = sdio_dmacpy_start(dst, st_src + 2, TEST_LEN);
    sdio_dmacpy_finish();
    for (bad = 0, i = 0; i < TEST_LEN; i++)
        if (dst[i] != st_src[i + 2]) bad++;
    HR_LOGI("selftest dma+2  : dma_bytes=%lu wait_loops=%lu  %s (bad=%lu)\r\n",
            (unsigned long)dma_bytes, (unsigned long)s_last_wait_loops,
            bad ? "FAIL" : "PASS", (unsigned long)bad);
    if (bad) { s_dma_failed = 1; g_sdio_dmacpy_mode = 0; }

    pbuf_free(p);
    sdio_dmacpy_stats(0);
}
#endif /* SDIO_DMACPY_SELFTEST */

/* ------------------------------------------------------------------------- */
void sdio_dmacpy_stats(int reset)
{
    HR_LOGI("dmacpy mode=%lu burst=%lu width=%lu starts=%lu cpu_small=%lu cpu_misalign=%lu timeout=%lu\r\n",
            (unsigned long)g_sdio_dmacpy_mode, (unsigned long)s_burst,
            (unsigned long)((s_unit == 4u) ? 32 : 16), (unsigned long)ctr_dma_starts,
            (unsigned long)ctr_cpu_small, (unsigned long)ctr_cpu_misalign,
            (unsigned long)ctr_timeout);
    HR_LOGI("dmacpy finish_n=%lu wait_loops_sum=%lu wait_prev=%lu inited=%lu failed=%lu\r\n",
            (unsigned long)ctr_finish_n, (unsigned long)ctr_wait_loops_sum,
            (unsigned long)ctr_wait_prev, (unsigned long)s_inited,
            (unsigned long)s_dma_failed);
    if (reset) {
        ctr_dma_starts = ctr_cpu_small = ctr_cpu_misalign = 0;
        ctr_timeout = ctr_finish_n = ctr_wait_loops_sum = ctr_wait_prev = 0;
        s_last_wait_loops = 0;
    }
}

#else /* !IPRO6 : legacy-equivalent stubs so the gate links unchanged */

int sdio_dmacpy_init(void)
{
    return -1;
}

uint32_t sdio_dmacpy_start(void *dst, const void *src, uint32_t len)
{
    arch_memcpy_fast(dst, src, len);
    return 0;
}

void sdio_dmacpy_finish(void)
{
}

void sdio_dmacpy_cached_copy(void *dst, const void *src, uint32_t len)
{
    arch_memcpy_fast(dst, src, len);
}

void sdio_dmacpy_selftest(void)
{
}

void sdio_dmacpy_stats(int reset)
{
    (void)reset;
}

void sdio_dmacpy_set_burst(uint32_t b)
{
    (void)b;
}

void sdio_dmacpy_set_width(uint32_t w)
{
    (void)w;
}

#endif /* IPRO6 */
