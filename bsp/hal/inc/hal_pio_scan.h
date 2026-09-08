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

#ifndef HAL_PIO_SCAN_H
#define HAL_PIO_SCAN_H
/*
 * hal_pio - PIO scan-shift HAL (Phase 2a, CPU-paced transport).
 *
 * Binds drv_pio to the scan use case: ungate the GLB PIO clock, load the 4-tick
 * out-driven player program, configure SM0 (pins / shift / clkdiv / wrap), set
 * gen0-7 (SCAN_IN + SCAN_CLK + SCAN_EN + OCC) to PIO func outputs + gen22-24 to
 * PIO inputs, and move bursts via a CPU FIFO loop. DMA-paced transport (Phase 2b)
 * is deferred: the IPRO7 DMA req-source for PIO is not in the SDK headers.
 *
 * The pioasm program + SM-config path (hal_pio_scan_setup_sm) and the CPU shift loop
 * are host-testable under -DHAL_PIO_SCAN_HOSTTEST (with -DDRV_PIO_HOSTTEST); the GLB
 * clock / pinmux / capture are target-only and verified at first light.
 */
#include <stdint.h>
#include <stdbool.h>

/*
 * IPRO7 DMA peripheral-request IDs for SM0's FIFOs, from the IPRO7 memory map
 * (IPRO7_MemMap_20250407_YK.xlsx). NOTE: the SDK drv_dma.h DMA_Periph_Req_Type
 * enum is incomplete/wrong for ipro7 (no PIO; its I2S_RX=20 collides) - use these.
 */
#define HAL_PIO_SCAN_DMA_REQ_TX  20u   /* pio_tx[0] */
#define HAL_PIO_SCAN_DMA_REQ_RX  16u   /* pio_rx[0] */

typedef struct {
    uint8_t  out_base, out_cnt;   /* SCAN_IN   gen0..4   -> 0, 5 */
    uint8_t  in_base,  in_cnt;    /* SCAN_OUT  gen22..24 -> 22, 3 */
    uint8_t  ss_base,  ss_cnt;    /* side-set  gen5..7   -> 5, 3 (SCAN_CLK..OCC) */
    uint32_t ss_pio_mask;         /* pads to switch to PIO func: 0xA0 = gen5|gen7 (NOT gen6) */
    uint16_t clkdiv_int;          /* SM clock divider integer part */
    uint8_t  clkdiv_frac;         /* SM clock divider fractional part (/256) */
    uint8_t  pio_clk_div;         /* GLB pre-divider (GLB_CLK_CFG6.PIO_CLK_DIV) */
    uint8_t  pio_clk_sel;         /* GLB PIO clock source select */
    uint32_t input_sync_bypass;   /* 2-FF input synchronizer bypass (sample-edge knob; start 0) */
    uint8_t  dma_id;              /* DMA controller id for the DMA-paced path */
    uint8_t  dma_tx_ch, dma_rx_ch;/* DMA channels for TX (M2P) / RX (P2M) */
    /* shift-WFT timing (ns) from the IVF header (parsed from the WGL timeplate).
     * The scan_shift PIO program (pulse position, pre/post-edge strobe, hold) is
     * BUILT from these instead of hardcoded. All 0 -> use the built-in default. */
    uint16_t period_ns, clk_rise_ns, clk_fall_ns, strobe_ns;
    /* Force the uniform 1-cycle-per-phase player (4 SM cycles/cell) instead of the
     * WFT-proportional dilation. With a high SM clock this compresses launch->capture
     * toward the WGL's at-speed 25ns gap (experiment B for the OCC/SSA residual). */
    uint8_t  uniform_phases;
    /* Multiply every WFT phase length by this before building the player (0/1 = x1).
     * Approach A "A0": run the SM at 96MHz (clkdiv=1) with delay_scale=3 so each cell
     * is 75 SM cycles = the SAME 781ns wall-time as the proven 32MHz/25-cyc config (the
     * sample tick lands at the same wall position -> shift sampling stays valid), while
     * the finer 10.42ns SM grid lets a later capture doublet hit ~20.83ns. */
    uint8_t  delay_scale;
    /* A1: use the 2-path "capture doublet" PIO program. Normal shift cells keep the
     * proven 4-tick player (delay_scale); a control byte per cell selects a merged
     * boundary cell that does the last-shift LAUNCH then the CAPTURE pulse 2 SM cycles
     * later (=20.83ns @96MHz) — the at-speed launch->capture the WGL needs. Requires the
     * doublet TX encoding (control word + 3/5 pin words per cell) in pio_replay. */
    uint8_t  capture_doublet;
    /* De-risk probe for branched-PIO: append ONE unconditional `jmp 0` per cell to the
     * proven 4-tick player (period kept = A0's by stealing 1 cyc from the low-hold), so
     * the cell is 5 instructions but still 3 TX words + identical waveform. If chain_test
     * stays 2341 the shift tolerates an extra (non-`out`, 0-TX-word) instruction -> a
     * counter `jmp` branch to a compressed-capture boundary is viable. */
    uint8_t  branch_probe;
} hal_pio_scan_cfg_t;

int  hal_pio_scan_init(const hal_pio_scan_cfg_t *cfg);
/* one CPU-paced burst: feed tx_words then collect rx_words. 0 ok, <0 on timeout.
 * = hal_pio_scan_start() + hal_pio_scan_feed() + hal_pio_scan_stop() (one-shot). */
int  hal_pio_scan_shift(const uint32_t *tx, uint32_t tx_words,
                        uint32_t *rx, uint32_t rx_words);

/*
 * Streaming transport (continuity across patterns). The EDT compressor is one
 * continuous pipeline over the whole chain_test shift; restarting the SM between
 * patterns resets it (corrupts each pattern's first ~30 EDT unload cells). Use:
 *   hal_pio_scan_start();                     // restart the SM exactly ONCE
 *   for each shift run: hal_pio_scan_feed(...) // SM keeps running between runs
 *   hal_pio_scan_stop();                       // disable at the end
 * Between feeds the SM stalls on the empty TX FIFO (autopull), holding the last
 * tick's pin state. SCAN_EN and both capture clocks are PIO-driven from the TX
 * word (Approach A), so the inter-pattern capture cycle is just another cell in
 * the continuous TX stream — no separate CPU capture, no SM restart.
 */
void hal_pio_scan_start(void);
int  hal_pio_scan_feed(const uint32_t *tx, uint32_t tx_words,
                       uint32_t *rx, uint32_t rx_words);
void hal_pio_scan_stop(void);

/*
 * Continuous feed that tops the TX FIFO for the WHOLE burst (n words) while
 * draining each captured word straight into caplog[i] = (rx_i & in_mask). The
 * point is that one big call keeps the FIFO non-empty across pattern boundaries
 * so SCAN_CLK NEVER stalls (the per-pattern feed left a ~70us clock-stop at each
 * boundary that corrupted the DUT's EDT compressor). Only the first log_max words
 * are written to caplog (the rest are still clocked, just not logged). 0 ok, <0 timeout.
 */
int  hal_pio_scan_feed_log(const uint32_t *tx, uint32_t n,
                           uint8_t *caplog, uint32_t log_max, uint32_t in_mask);

/* DMA-paced burst (Phase 2b): same semantics as hal_pio_scan_shift, but the
 * FIFOs are fed/drained by drv_dma (TX = M2P req 20, RX = P2M req 16). Falls
 * back to CPU-paced if DMA is unavailable. Target-only. */
int  hal_pio_scan_shift_dma(const uint32_t *tx, uint32_t tx_words,
                            uint32_t *rx, uint32_t rx_words);
void hal_pio_scan_deinit(void);

/* absolute MMIO addresses of SM0's FIFOs (DMA src/dst); pure, host-testable */
uint32_t hal_pio_scan_tx_fifo_addr(void);
uint32_t hal_pio_scan_rx_fifo_addr(void);

/* --- exposed for host test (program + SM config via drv_pio only) ---------- */
void hal_pio_scan_setup_sm(const hal_pio_scan_cfg_t *cfg);

/*
 * Build the scan_shift PIO program from the WGL shift-WFT timing (ns). Emits the
 * clock as a pulse shaped LIKE the WFT: LOW (drive SCAN_IN) -> sample SCAN_OUT at
 * the strobe -> HIGH pulse [clk_rise,clk_fall) -> LOW hold to the period end. The
 * `in` lands where the WFT strobe sits relative to clk_rise (pre-edge here). The
 * inter-event durations set each instruction's side-set delay (scaled to PIO's
 * <=3). period_ns==0 -> the proven default {0x6805,0x4803,0xBC42,0xA842}.
 * Returns the instruction count (<=4); fills prog[4] and *wrap_top.
 */
uint8_t hal_pio_scan_build_prog(uint16_t period_ns, uint16_t clk_rise_ns,
                                uint16_t clk_fall_ns, uint16_t strobe_ns,
                                uint8_t out_cnt, uint8_t in_cnt,
                                uint16_t prog[4], uint8_t *wrap_top);

/* A1 capture-doublet program (14 instr, 2 control-byte-selected paths): normal 4-tick
 * shift + a merged boundary cell whose LAUNCH and CAPTURE pulses are 2 SM cycles apart
 * (=20.83ns @96MHz). Fills prog[14], returns the instruction count. See the .c body. */
uint8_t hal_pio_scan_build_doublet_prog(uint16_t period_ns, uint16_t clk_rise_ns,
                                        uint16_t clk_fall_ns, uint16_t strobe_ns,
                                        uint8_t out_cnt, uint8_t in_cnt,
                                        uint16_t prog[16]);

#endif /* HAL_PIO_SCAN_H */
