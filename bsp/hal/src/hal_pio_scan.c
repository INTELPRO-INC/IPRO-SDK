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
 * hal_pio - PIO scan-shift HAL (Phase 2a, CPU-paced). See inc/hal_pio_scan.h.
 *
 * Host-tested: hal_pio_scan_setup_sm (pioasm + SM config via drv_pio) and the CPU
 * shift loop run under -DHAL_PIO_SCAN_HOSTTEST. The GLB clock / pinmux / capture are
 * target-only (compiled out on host) and verified at first light.
 */
#include "hal_pio_scan.h"
#ifndef HAL_PIO_SCAN_HOSTTEST
#include "drv_glb.h"       /* GLB_GPIO_Init, GLB_GPIO_Cfg_Type */
#include "drv_gpio.h"      /* GPIO_FUN_PIO, GPIO_MODE_* */
#include "glb_reg.h"       /* GLB clock / reset fields */
#include "ipro7.h"         /* GLB_BASE */
#endif
#ifdef HAL_PIO_SCAN_USE_DMA     /* DMA-paced path: opt-in (Phase 2b); CPU-paced needs no drv_dma */
#include "hal_dma.h"       /* dma_device_t, dma_setConfig, dma_reload */
#include "drv_dma.h"       /* DMA_Channel_Is_Busy */
#include "system_soc.h"    /* L1C_DCACHE_CLEAN_RANGE / INVALID_RANGE (DMA cache coherency) */
#endif
#include "drv_pio.h"

#define PIO_SM 0u          /* scan uses SM0 */

/*
 * scan player PIO program (Approach A) — an OUT-DRIVEN pin player, NO side-set.
 * gen0-7 (SCAN_IN[4:0] + SCAN_CLK + SCAN_EN + OCC_SCAN_CLK) are ALL driven from
 * the TX word, so the capture clock pulse is the SAME 1-slot width as the shift
 * pulse and SCAN_EN is PIO-timed. 4-tick wrap (3 `out` + 1 `in`); clkdiv sets the
 * rate. The `in` at tick1 samples SCAN_OUT while the pins still hold tick0's drive
 * (clk LOW) = PRE-edge — the LA-proven sample point (post-edge sampled the EDT
 * chains one cell AHEAD). See hal_pio_scan_build_prog.
 */
uint8_t hal_pio_scan_build_prog(uint16_t period_ns, uint16_t clk_rise_ns,
                                uint16_t clk_fall_ns, uint16_t strobe_ns,
                                uint8_t out_cnt, uint8_t in_cnt,
                                uint16_t prog[4], uint8_t *wrap_top)
{
    /* OUT-DRIVEN pin player (Approach A): gen0-7 (SCAN_IN[4:0] + SCAN_CLK + SCAN_EN +
     * OCC) are ALL driven from the TX word — no side-set, so the full PIO delay field
     * [12:8] (0..31) is free. The 4 instructions keep the event order, and each one's
     * DELAY = its WGL-timeplate phase length (in SM cycles == WGL ns, since we run
     * ~100x under the 25ns WFT so 1 SM cycle/ns is plenty fine). This reproduces the
     * timeplate's DUTY and EDGE positions (not just the order), with NO extra TX words:
     *   prog0 out : clk LOW + drive,   length = strobe_ns      (0 -> strobe)
     *   prog1 in  : sample PRE-edge,   length = rise - strobe  (strobe -> clk rise)
     *   prog2 out : clk HIGH pulse,    length = fall - rise    (rise -> clk fall)
     *   prog3 out : clk LOW hold,      length = period - fall  (fall -> cell end)
     * period==0 (old IVF) -> the proven uniform 1-slot fallback. clkdiv sets the rate;
     * total SM cycles = period_ns, so clkdiv is sized for that in the adapter cfg. */
    uint16_t ph[4];
    if (period_ns && clk_rise_ns > strobe_ns && clk_fall_ns > clk_rise_ns
                  && period_ns > clk_fall_ns) {
        ph[0] = strobe_ns ? strobe_ns : 1;
        ph[1] = clk_rise_ns - strobe_ns;
        ph[2] = clk_fall_ns - clk_rise_ns;
        ph[3] = period_ns - clk_fall_ns;
    } else {
        ph[0] = ph[1] = ph[2] = ph[3] = 1;   /* uniform fallback */
    }
    for (int i = 0; i < 4; i++) {            /* clamp to [1,32] SM cycles (delay 0..31) */
        if (ph[i] < 1) ph[i] = 1;
        if (ph[i] > 32) ph[i] = 32;
    }
    prog[0] = (uint16_t)(0x6000u | ((ph[0]-1u)<<8) | out_cnt);  /* out: clk LOW + drive */
    prog[1] = (uint16_t)(0x4000u | ((ph[1]-1u)<<8) | in_cnt);   /* in : sample PRE-edge */
    prog[2] = (uint16_t)(0x6000u | ((ph[2]-1u)<<8) | out_cnt);  /* out: clk HIGH */
    prog[3] = (uint16_t)(0x6000u | ((ph[3]-1u)<<8) | out_cnt);  /* out: clk LOW hold */
    *wrap_top = 3;
    return 4;
}

/*
 * A1 capture-doublet program — COUNTER-BRANCH (v3). De-risk proved an extra `jmp` (0 TX
 * words) is FREE but an extra `out pins` (1 TX word) re-breaks shift sampling, so EVERY
 * normal cell stays the EXACT proven A0 4-tick (3 `out pins,8` + `in`, 3 TX words); only
 * the boundary cell — selected by a PIO down-counter X — diverts to a compressed
 * launch->capture. Layout (PC seats at P0 = prime, runs ONCE; wrap target is P1):
 *   P0  out  x,32             ; PRIME X from the first TX word (segment0 cell-count-1)
 *   P1  out  pins,8 [d0-1]    ; t0 clk LOW + drive            <- cell start (every cell)
 *   P2  in   pins,3 [d1-2]    ; sample SCAN_OUT PRE-edge
 *   P3  jmp  x--, P_NORM      ; X!=0: normal (X--). X==0: fall through to boundary
 *   P4  out  pins,8 [0]       ; LAUNCH pulse (clk HIGH, last-shift si, EN high) 1 cyc
 *   P5  out  pins,8 [0]       ; gap (clk LOW) -> launch->capture = 2 cyc = 20.83ns
 *   P6  out  pins,8 [cw-1]    ; CAPTURE pulse (EN low, cap PIs, cap clk)
 *   P7  out  pins,8 [d3-1]    ; hold / settle
 *   P8  out  x,32             ; RELOAD X for the next segment (boundary-only TX word)
 *   P9  jmp  P1
 *   P10 out  pins,8 [d2-1]    ; P_NORM: normal pulse (wide, = A0)
 *   P11 out  pins,8 [d3-2]    ; normal low hold
 *   P12 jmp  P1
 * Normal cell waveform == A0 exactly: the two `jmp`s (P3,P12) each steal 1 cyc, folded
 * out of P2/P11 so the period stays 75 and the sample/pulse phases are unchanged. The
 * boundary cell is 5 `out pins` + 1 `out x` (6 TX words) but occurs only ~once/pattern.
 * Feed (pio_replay run_doublet_dma): [prime] then per cell 3 words (normal) / 6 words
 * (boundary = t0,launch,gap,capture,hold,next-reload). RX = 1 `in`/cell (prime/reload
 * take none). Returns the instruction count (13).
 */
#ifndef HAL_PIO_DOUBLET_CAPW
#define HAL_PIO_DOUBLET_CAPW 4u    /* CAPTURE pulse width (SM cycles); edge-triggered, a few cyc */
#endif
#ifndef HAL_PIO_DOUBLET_GAP
#define HAL_PIO_DOUBLET_GAP 1u     /* P5 gap width -> launch->capture = 1(launch)+GAP cyc. 1=20.8ns.
                                    * SWEEP knob: wide (~48) mimics A0's slow capture (sanity). */
#endif
uint8_t hal_pio_scan_build_doublet_prog(uint16_t period_ns, uint16_t clk_rise_ns,
                                        uint16_t clk_fall_ns, uint16_t strobe_ns,
                                        uint8_t out_cnt, uint8_t in_cnt,
                                        uint16_t prog[16])
{
    uint16_t d0, d1, d2, d3;
    if (period_ns && clk_rise_ns > strobe_ns && clk_fall_ns > clk_rise_ns
                  && period_ns > clk_fall_ns) {
        d0 = strobe_ns ? strobe_ns : 1u;            /* t0 LOW length = WFT strobe (sample @) */
        d1 = (uint16_t)(clk_rise_ns - strobe_ns);   /* strobe -> clk rise */
        d2 = (uint16_t)(clk_fall_ns - clk_rise_ns); /* clk HIGH width (normal pulse) */
        d3 = (uint16_t)(period_ns - clk_fall_ns);   /* clk LOW hold */
    } else {
        d0 = d1 = d2 = d3 = 4u;                      /* default: keep >=2 so the jmp-steal stays >=1 */
    }
    if (d0 < 1u) d0 = 1u;
    if (d0 > 32u) d0 = 32u;
    if (d1 < 2u) d1 = 2u;                  /* P2 steals 1 cyc for the P3 jmp -> need >=2 */
    if (d1 > 32u) d1 = 32u;
    if (d2 < 1u) d2 = 1u;
    if (d2 > 32u) d2 = 32u;
    if (d3 < 2u) d3 = 2u;                  /* P11 steals 1 cyc for the P12 jmp -> need >=2 */
    if (d3 > 32u) d3 = 32u;
    uint16_t cw = HAL_PIO_DOUBLET_CAPW;
    if (cw < 1u) cw = 1u;
    if (cw > 32u) cw = 32u;
    uint16_t gap = HAL_PIO_DOUBLET_GAP;
    if (gap < 1u) gap = 1u;
    if (gap > 32u) gap = 32u;
    #define O_PINS(d) ((uint16_t)(0x6000u | (((d)-1u)<<8) | out_cnt))
    #define I_PINS(d) ((uint16_t)(0x4000u | (((d)-1u)<<8) | in_cnt))
    #define O_X32     ((uint16_t)(0x6020u))                    /* out x, 32 (dest=X, count 32->0) */
    #define JMP_XDEC(a) ((uint16_t)(0x0040u | (a)))            /* jmp x--, a (cond 010 = X post-dec) */
    #define JMP(a)      ((uint16_t)(0x0000u | (a)))            /* jmp always, a */
    prog[0]  = O_X32;                 /* P0  prime X (runs once; PC seats here) */
    prog[1]  = O_PINS(d0);            /* P1  t0 clk LOW + drive */
    prog[2]  = I_PINS((uint16_t)(d1 - 1u)); /* P2  sample PRE-edge (steal 1 cyc for P3 jmp) */
    prog[3]  = JMP_XDEC(10u);         /* P3  branch: ->P_NORM (10) normal, fall through = boundary */
    prog[4]  = O_PINS(1u);            /* P4  LAUNCH pulse (clk HIGH) 1 cyc */
    prog[5]  = O_PINS(gap);           /* P5  gap (clk LOW) -> launch->capture = 1+gap cyc */
    prog[6]  = O_PINS(cw);            /* P6  CAPTURE pulse */
    prog[7]  = O_PINS(d3);            /* P7  hold / settle */
    prog[8]  = O_X32;                 /* P8  reload X for next segment */
    prog[9]  = JMP(1u);               /* P9  -> P1 */
    prog[10] = O_PINS(d2);            /* P10 P_NORM: normal pulse (wide) */
    prog[11] = O_PINS((uint16_t)(d3 - 1u)); /* P11 normal low hold (steal 1 cyc for P12 jmp) */
    prog[12] = JMP(1u);              /* P12 -> P1 */
    #undef O_PINS
    #undef I_PINS
    #undef O_X32
    #undef JMP_XDEC
    #undef JMP
    return 13;
}

static hal_pio_scan_cfg_t s_cfg;
static uint16_t s_prog[16];         /* the out-driven player (4-tick) or 14-instr doublet */

void hal_pio_scan_setup_sm(const hal_pio_scan_cfg_t *cfg)
{
    uint8_t top = 3;
    /* uniform_phases -> pass period_ns=0 so build_prog takes the uniform 1-slot path
     * (4 SM cycles/cell); otherwise the WFT-proportional dilation, with every phase
     * length scaled by delay_scale (A0: x3 so 96MHz cells keep the 32MHz wall-time). */
    uint16_t sc = cfg->delay_scale ? cfg->delay_scale : 1u;
    uint16_t per = cfg->uniform_phases ? 0u : (uint16_t)(cfg->period_ns * sc);
    uint8_t n;
    if (cfg->capture_doublet) {        /* A1: 2-path program (control-byte selected) */
        n = hal_pio_scan_build_doublet_prog(per,
                                            (uint16_t)(cfg->clk_rise_ns * sc),
                                            (uint16_t)(cfg->clk_fall_ns * sc),
                                            (uint16_t)(cfg->strobe_ns * sc),
                                            cfg->out_cnt, cfg->in_cnt, s_prog);
        top = (uint8_t)(n - 1u);       /* all paths end in `jmp 0`; wrap_top never reached */
    } else {
        n = hal_pio_scan_build_prog(per,
                                    (uint16_t)(cfg->clk_rise_ns * sc),
                                    (uint16_t)(cfg->clk_fall_ns * sc),
                                    (uint16_t)(cfg->strobe_ns * sc),
                                    cfg->out_cnt, cfg->in_cnt, s_prog, &top);
        if (cfg->branch_probe) {     /* de-risk: +1 `jmp 0`/cell, period unchanged (steal 1 cyc
                                      * from the low-hold delay). 5 instr, still 3 TX words. */
            uint16_t d = (uint16_t)((s_prog[3] >> 8) & 0x1Fu);   /* prog[3] delay field */
            if (d) s_prog[3] = (uint16_t)((s_prog[3] & ~(0x1Fu << 8)) | ((d - 1u) << 8));
            s_prog[n] = 0x0000u;     /* jmp always -> addr 0 (wrap target) */
            top = n;                 /* wrap at the jmp (never reached; the jmp loops) */
            n = (uint8_t)(n + 1u);
        }
    }
    pio_load_program(s_prog, n, 0);
    pio_sm_set_pins(PIO_SM, cfg->out_base, cfg->out_cnt, cfg->in_base, cfg->in_cnt,
                    cfg->ss_base, cfg->ss_cnt);
    /* out shift RIGHT: `out pins,N` emits OSR[N-1:0] -> gen(out_base+i)=SCAN_IN bit i.
     * in shift LEFT: `in pins,N` lands gen(in_base+i) in ISR bit i so the pushed word
     * has SCAN_OUT[i] in bit i (gather reads rx & mask). The two dirs are OPPOSITE:
     * in-RIGHT would put the captured bits in the HIGH word -> gather reads 0. */
    pio_sm_set_shift(PIO_SM, /*out_shdir*/true,  /*autopull*/true, cfg->out_cnt,
                             /*in_shdir*/false, /*autopush*/true, cfg->in_cnt);
    pio_sm_set_clkdiv(PIO_SM, cfg->clkdiv_int, cfg->clkdiv_frac);
    pio_sm_set_wrap(PIO_SM, /*top*/top, /*bottom*/0, /*ss_en*/false);
    pio_input_sync_bypass(cfg->input_sync_bypass);
    /* CRITICAL: the whole out group (gen0-7 = SCAN_IN[4:0] + SCAN_CLK + SCAN_EN +
     * OCC) must be PIO OUTPUTS — they are all driven by `out pins`. (out_cnt=8 now;
     * the old code capped the mask at 5 bits, leaving the clocks/SCAN_EN Hi-Z.) */
    pio_sm_set_pindirs(PIO_SM, cfg->out_base, cfg->out_cnt,
                       (cfg->out_cnt >= 32) ? 0xFFFFFFFFu
                                            : (uint32_t)((1u << cfg->out_cnt) - 1u));
    pio_sm_exec(PIO_SM, 0x0000u);    /* JMP 0: seat PC at wrap_target */
}

void hal_pio_scan_start(void)
{
    pio_sm_enable(PIO_SM, false);
    pio_sm_restart(PIO_SM);
    pio_sm_enable(PIO_SM, true);
}

void hal_pio_scan_stop(void)
{
    pio_sm_enable(PIO_SM, false);
}

/* Feed tx_words and drain rx_words on the ALREADY-RUNNING SM (no enable/restart).
 * Returns when rx_words have been collected; the SM then stalls on the empty TX
 * FIFO holding the side-set clocks LOW (the inter-pattern idle state). */
int hal_pio_scan_feed(const uint32_t *tx, uint32_t tx_words, uint32_t *rx, uint32_t rx_words)
{
    if ((tx_words && !tx) || (rx_words && !rx)) return -1;

    uint32_t ti = 0, ri = 0;
    /* generous progress guard: any iteration must move a word, else the SM stalled
     * unexpectedly (e.g. tx underfed) - bail rather than hang. */
    uint32_t guard = (tx_words + rx_words) * 16u + 1024u;
    while (ri < rx_words) {
        while (ti < tx_words && pio_txlevel(PIO_SM) < PIO_FIFO_DEPTH)
            pio_sm_put(PIO_SM, tx[ti++]);
        while (pio_rxlevel(PIO_SM) > 0 && ri < rx_words)
            rx[ri++] = pio_sm_get(PIO_SM);
        if (--guard == 0) return -1;
    }
    return 0;
}

int hal_pio_scan_shift(const uint32_t *tx, uint32_t tx_words, uint32_t *rx, uint32_t rx_words)
{
    if ((tx_words && !tx) || (rx_words && !rx)) return -1;
    hal_pio_scan_start();
    int rc = hal_pio_scan_feed(tx, tx_words, rx, rx_words);
    hal_pio_scan_stop();
    return rc;
}

int hal_pio_scan_feed_log(const uint32_t *tx, uint32_t n,
                          uint8_t *caplog, uint32_t log_max, uint32_t in_mask)
{
    if (n && (!tx || !caplog)) return -1;
    uint32_t ti = 0, ri = 0;
    uint32_t guard = n * 32u + 1024u;
    while (ri < n) {
        /* top the TX FIFO every pass so the SM never drains it (continuous clock) */
        while (ti < n && pio_txlevel(PIO_SM) < PIO_FIFO_DEPTH)
            pio_sm_put(PIO_SM, tx[ti++]);
        while (pio_rxlevel(PIO_SM) > 0 && ri < n) {
            uint32_t w = pio_sm_get(PIO_SM);
            if (ri < log_max) caplog[ri] = (uint8_t)(w & in_mask);
            ri++;
        }
        if (--guard == 0) return -1;
    }
    return 0;
}


#ifndef HAL_PIO_SCAN_HOSTTEST
/* ---- target-only: GLB clock + pinmux (verified at first light) ------------- */
static inline uint32_t glb_rd(uint32_t off) { return *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off); }
static inline void     glb_wr(uint32_t off, uint32_t v) { *(volatile uint32_t *)(uintptr_t)(GLB_BASE + off) = v; }

static void hal_pio_glb_clock(const hal_pio_scan_cfg_t *cfg)
{
    uint32_t v;
    /* pulse the PIO block reset (SWRST_CFG1.bit24); polarity to confirm at bring-up */
    v = glb_rd(GLB_SWRST_CFG1_OFFSET); glb_wr(GLB_SWRST_CFG1_OFFSET, v | GLB_SWRST_PIO_MSK);
    v = glb_rd(GLB_SWRST_CFG1_OFFSET); glb_wr(GLB_SWRST_CFG1_OFFSET, v & ~GLB_SWRST_PIO_MSK);
    /* ungate the PIO clock FIRST (else the SM never ticks) */
    v = glb_rd(GLB_CGEN_CFG0_OFFSET); glb_wr(GLB_CGEN_CFG0_OFFSET, v | GLB_CGEN_PIO_MSK);
    /* PIO_CLK_EN=1 + source select + GLB pre-divider */
    v = glb_rd(GLB_CLK_CFG6_OFFSET);
    v &= ~(GLB_PIO_CLK_SEL_MSK | GLB_PIO_CLK_DIV_MSK);
    v |= GLB_PIO_CLK_EN_MSK;
    v |= ((uint32_t)cfg->pio_clk_sel << GLB_PIO_CLK_SEL_POS) & GLB_PIO_CLK_SEL_MSK;
    v |= ((uint32_t)cfg->pio_clk_div << GLB_PIO_CLK_DIV_POS) & GLB_PIO_CLK_DIV_MSK;
    glb_wr(GLB_CLK_CFG6_OFFSET, v);
}

static void pin_func(uint8_t pin, uint32_t fun, uint32_t mode)
{
    GLB_GPIO_Cfg_Type c = {
        .gpioPin = pin, .gpioFun = fun, .gpioMode = mode,
        .pullType = GPIO_PULL_NONE, .drive = 1, .smtCtrl = 1,
    };
    GLB_GPIO_Init(&c);
}

static void hal_pio_pinmux(const hal_pio_scan_cfg_t *cfg)
{
    /* Approach A: the whole out group gen0-7 (SCAN_IN[4:0] + SCAN_CLK + SCAN_EN +
     * OCC_SCAN_CLK) is PIO-func OUTPUT — all driven by `out pins`. No SWGPIO clocks,
     * no SWGPIO SCAN_EN; the capture clock is a TX-stream cell like any shift cell. */
    for (uint8_t i = 0; i < cfg->out_cnt; i++)
        pin_func(cfg->out_base + i, GPIO_FUN_PIO, GPIO_MODE_OUTPUT);
    for (uint8_t i = 0; i < cfg->in_cnt; i++)           /* SCAN_OUT: PIO inputs (func=PIO to route to `in pins`) */
        pin_func(cfg->in_base + i, GPIO_FUN_PIO, GPIO_MODE_INPUT);
}
#endif

int hal_pio_scan_init(const hal_pio_scan_cfg_t *cfg)
{
    if (!cfg) return -1;
    s_cfg = *cfg;
#ifndef HAL_PIO_SCAN_HOSTTEST
    hal_pio_glb_clock(cfg);     /* (1) ungate PIO clock before any SM access */
#endif
    hal_pio_scan_setup_sm(cfg);      /* (2) program + configure SM0 */
#ifndef HAL_PIO_SCAN_HOSTTEST
    hal_pio_pinmux(cfg);        /* (3) gen0-7 -> PIO func outputs, gen22-24 -> PIO inputs */
#endif
    return 0;
}

/* --- FIFO MMIO addresses (pure; DMA src/dst) ------------------------------ */
uint32_t hal_pio_scan_tx_fifo_addr(void) { return PIO_BASE + PIO_TXF0_OFFSET; }
uint32_t hal_pio_scan_rx_fifo_addr(void) { return PIO_BASE + PIO_RXF0_OFFSET; }

#ifdef HAL_PIO_SCAN_USE_DMA
/* --- DMA-paced burst (Phase 2b; opt-in via HAL_PIO_SCAN_USE_DMA, first-light verified) -- */
static void dma_setup(const hal_pio_scan_cfg_t *c, uint8_t ch, uint8_t dir,
                      uint32_t src_req, uint32_t dst_req,
                      uint8_t src_inc, uint8_t dst_inc)
{
    dma_device_t d = { 0 };
    d.id = c->dma_id; d.ch = ch;
    d.transfer_mode = DMA_LLI_ONCE_MODE;
    d.direction = dir;
    d.src_req = src_req; d.dst_req = dst_req;
    d.src_addr_inc = src_inc; d.dst_addr_inc = dst_inc;
    d.src_width = DMA_TRNS_WIDTH_32BITS; d.dst_width = DMA_TRNS_WIDTH_32BITS;
    d.src_burst_size = 0; d.dst_burst_size = 0;     /* burst 1 word; tune via FIFO thr later */
    dma_setConfig(&d);
}

int hal_pio_scan_shift_dma(const uint32_t *tx, uint32_t tx_words, uint32_t *rx, uint32_t rx_words)
{
    if ((tx_words && !tx) || (rx_words && !rx)) return -1;
    uint32_t tx_fifo = hal_pio_scan_tx_fifo_addr();
    uint32_t rx_fifo = hal_pio_scan_rx_fifo_addr();
    dma_lli_ctrl_t *lli = 0;

    /* Cache coherency: TX was just written by the CPU (encode) into cacheable PSRAM --
     * flush it so the DMA reads the fresh bytes, not stale memory. */
    if (tx_words) L1C_DCACHE_CLEAN_RANGE(tx, tx_words * 4u);
    /* Drop any DIRTY RX cache lines BEFORE the DMA writes: on the first run after boot the
     * RX buffer (.bss) has dirty zero lines, and the post-DMA flush+invalidate (the IPRO7
     * INVALID macro is a flush+inval) would otherwise write those zeros back OVER the DMA's
     * just-captured data -> first-run-only corruption. Pre-invalidating leaves nothing dirty. */
    if (rx_words) L1C_DCACHE_INVALID_RANGE(rx, rx_words * 4u);
    /* Barrier: make the cache flush's PSRAM writes COMPLETE before the DMA reads TX -- at a
     * high clkdiv the DMA can start before a buffered PSRAM write drains (-> stale TX). */
    __asm__ volatile("fence rw, rw" ::: "memory");

    pio_sm_enable(PIO_SM, false);
    pio_sm_restart(PIO_SM);
    /* Seat the PC at the program start (wrap_target). pio_sm_restart clears ISR/OSR but
     * NOT the PC; on the first run after a fresh load the SM would otherwise start mid-
     * program and emit a few spurious `in` words, offsetting the whole RX capture (3-word
     * shift -> global first-run corruption). The CPU feed path tolerates this; the DMA
     * one-shot does not. */
    pio_sm_exec(PIO_SM, 0x0000u);   /* JMP 0 */
    /* Flush any stale RX FIFO words BEFORE enabling DMA mode (once the RX FIFO is owned
     * by the DMA, a CPU drain is ineffective). A leftover word would offset the RX DMA
     * vs the SM output -> misalignment or both-FIFOs-full hang. */
    while (pio_rxlevel(PIO_SM)) (void)pio_sm_get(PIO_SM);
    pio_sm_dma_enable(PIO_SM, true, true);

    /* RX first (P2M, RXF0->mem, no src inc), then TX (M2P, mem->TXF0, no dst inc).
     * dma_reload's transfer_size is in BYTES (it divides by the 32-bit width and rejects
     * non-multiples of 4), so pass words*4 -- NOT the word count. */
    int rrc, trc;
    dma_init(s_cfg.dma_id, s_cfg.dma_rx_ch);
    dma_setup(&s_cfg, s_cfg.dma_rx_ch, DMA_PERIPH_TO_MEMORY, HAL_PIO_SCAN_DMA_REQ_RX, 0, 0, 1);
    rrc = dma_reload(s_cfg.dma_id, s_cfg.dma_rx_ch, rx_fifo, (uint32_t)(uintptr_t)rx, rx_words * 4u, &lli);

    dma_init(s_cfg.dma_id, s_cfg.dma_tx_ch);
    dma_setup(&s_cfg, s_cfg.dma_tx_ch, DMA_MEMORY_TO_PERIPH, 0, HAL_PIO_SCAN_DMA_REQ_TX, 1, 0);
    trc = dma_reload(s_cfg.dma_id, s_cfg.dma_tx_ch, (uint32_t)(uintptr_t)tx, tx_fifo, tx_words * 4u, &lli);
    if (rrc < 0 || trc < 0) {                  /* reload rejected -> don't run a no-op DMA */
        pio_sm_dma_enable(PIO_SM, false, false);
        return -1;
    }

    /* ARM both channels (dma_reload only configures the LLI; it does NOT start the
     * channel) so they are waiting on the PIO DREQs before the SM produces them. */
    dma_channel_start(s_cfg.dma_id, s_cfg.dma_rx_ch);
    dma_channel_start(s_cfg.dma_id, s_cfg.dma_tx_ch);

    pio_sm_enable(PIO_SM, true);

    /* wait for both channels to drain */
    uint32_t guard = (tx_words + rx_words) * 64u + 100000u;
    while ((DMA_Channel_Is_Busy(s_cfg.dma_id, s_cfg.dma_rx_ch) ||
            DMA_Channel_Is_Busy(s_cfg.dma_id, s_cfg.dma_tx_ch)) && --guard) { }

    pio_sm_enable(PIO_SM, false);
    pio_sm_dma_enable(PIO_SM, false, false);
    dma_close(s_cfg.dma_id, s_cfg.dma_tx_ch);
    dma_close(s_cfg.dma_id, s_cfg.dma_rx_ch);

    /* A clean run leaves the RX FIFO empty: the SM produced exactly rx_words and the DMA
     * captured all of them. A residual means the SM out-ran the DMA -- the first run after
     * a fresh load emits a few spurious leading `in` words, so the DMA's first captures are
     * garbage and the whole rxbuf is shifted (global corruption). Detect it here and fail;
     * the caller re-runs (a warmed SM leaves no residual). */
    uint8_t rx_residual = pio_rxlevel(PIO_SM);

    /* RX was just written by the DMA into cacheable PSRAM -- invalidate (flush+inval)
     * so the CPU's subsequent reads see the captured data, not a stale cache line. */
    if (rx_words) L1C_DCACHE_INVALID_RANGE(rx, rx_words * 4u);
    return (guard && rx_residual == 0u) ? 0 : -1;
}
#endif /* HAL_PIO_SCAN_USE_DMA */

void hal_pio_scan_deinit(void)
{
    pio_sm_enable(PIO_SM, false);
    pio_sm_dma_enable(PIO_SM, false, false);
}
