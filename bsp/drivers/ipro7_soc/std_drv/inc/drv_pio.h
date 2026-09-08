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

#ifndef DRV_PIO_H
#define DRV_PIO_H
/*
 * drv_pio - low-level IPRO7 PIO peripheral driver (RP2040-compatible).
 *
 * Pure SM / FIFO / instruction-memory / clkdiv / sync primitives on top of the
 * in-tree register map (regs/pio_reg.h). No DMA, no scan knowledge - that lives
 * in hal_pio / the scan backend. The PIO block clock MUST be ungated in GLB
 * (GLB_CGEN_PIO + pio_clk_en/sel/div) before any access here; the HAL does that.
 *
 * Host-testable: compile the .c with -DDRV_PIO_HOSTTEST to route MMIO through
 * drv_pio_test_mmio_read/write (a software register model), mirroring the
 * ivf_replay -DIVF_REPLAY_HOSTTEST approach. See tests/drv_pio/.
 */
#include <stdint.h>
#include <stdbool.h>
#include "pio_reg.h"        /* field POS/LEN/MSK + (via ipro7.h) PIO_BASE */

#define PIO_NUM_SM      2
#define PIO_IMEM_SIZE   32
#define PIO_FIFO_DEPTH  8

/* --- instruction memory ------------------------------------------------- */
/* Write n 16-bit opcodes into INSTR_MEM[origin .. origin+n-1]. */
void     pio_load_program(const uint16_t *ops, uint8_t n, uint8_t origin);

/* --- per-SM static config (program with the SM disabled) ---------------- */
/* PINCTRL: out/in/sideset each have an independent base + count. */
void     pio_sm_set_pins(uint8_t sm, uint8_t out_base, uint8_t out_cnt,
                         uint8_t in_base, uint8_t in_cnt,
                         uint8_t ss_base, uint8_t ss_cnt);
/* SHIFTCTRL: shift dirs (true = shift right) + autopull/autopush + thresholds.
 * A threshold of 0 means 32 in hardware; pass 32 -> encoded as 0. */
void     pio_sm_set_shift(uint8_t sm, bool out_shdir, bool autopull, uint8_t pull_thr,
                          bool in_shdir,  bool autopush, uint8_t push_thr);
/* FIFO DREQ thresholds for burst DMA (0 = default single-word request). Call
 * AFTER pio_sm_set_shift, which zeroes these fields. */
void     pio_sm_set_fifo_thr(uint8_t sm, uint8_t txf_thr, uint8_t rxf_thr);
/* CLKDIV: rate = pio_clk / (intg + frac/256). intg==0 means 65536 in hw. */
void     pio_sm_set_clkdiv(uint8_t sm, uint16_t intg, uint8_t frac);
/* EXECCTRL: wrap addresses (absolute imem slots) + EXECCTRL.SIDE_EN
 * (true => the top side-set bit is an optional-sideset enable, not data). */
void     pio_sm_set_wrap(uint8_t sm, uint8_t top, uint8_t bottom, bool ss_en);

/* --- run control -------------------------------------------------------- */
void     pio_sm_enable(uint8_t sm, bool en);    /* CTRL.SM_EN bit; others preserved */
void     pio_sm_restart(uint8_t sm);            /* CTRL.SM_RESET pulse              */
void     pio_sm_exec(uint8_t sm, uint16_t op);  /* execute one instr now (set PC via jmp) */
/* Set PIO pin DIRECTIONS for `count` pins (<=5) from `base` (bit=1 -> output) via
 * the SET group + an immediate `set pindirs`. A func=PIO pad only DRIVES when its
 * PIO pindir is output; out/side-set pads otherwise default to input (Hi-Z). */
void     pio_sm_set_pindirs(uint8_t sm, uint8_t base, uint8_t count, uint32_t dirs);
/* PINCTRL SET group (base+count) for a program's own `set pins`/`set pindirs`.
 * RMW so it composes after pio_sm_set_pins (which clears the SET fields). */
void     pio_sm_set_set_base(uint8_t sm, uint8_t base, uint8_t count);
/* EXECCTRL JMP_PIN: the GPIO tested by `jmp pin`. RMW (call after pio_sm_set_wrap). */
void     pio_sm_set_jmp_pin(uint8_t sm, uint8_t pin);
/* EXECCTRL mov_status source for `mov x/y, STATUS`: rx_not_tx=false -> STATUS is
 * true when TX FIFO level < n; true -> RX FIFO level < n. RMW (after set_wrap). */
void     pio_sm_set_mov_status(uint8_t sm, bool rx_not_tx, uint8_t n);
/* DMA_CONFIG: gate the per-SM DMA tx/rx req-ack lines (IPRO7 addition @0x158).
 * IPRO7 DMA peripheral IDs: pio_tx[0]=20 pio_rx[0]=16 (tx[1]=21 rx[1]=17). */
void     pio_sm_dma_enable(uint8_t sm, bool tx_en, bool rx_en);

/* --- FIFO --------------------------------------------------------------- */
void     pio_sm_put(uint8_t sm, uint32_t w);
uint32_t pio_sm_get(uint8_t sm);
uint8_t  pio_txlevel(uint8_t sm);
uint8_t  pio_rxlevel(uint8_t sm);

/* --- misc --------------------------------------------------------------- */
void     pio_input_sync_bypass(uint32_t mask);  /* 2-FF input synchronizer bypass */
/* DBG_CFGINFO: report the hardware's free parameters (NULL args skipped). */
void     pio_cfginfo(uint8_t *sm_count, uint8_t *fifo_depth, uint8_t *imem_size);

#endif /* DRV_PIO_H */
