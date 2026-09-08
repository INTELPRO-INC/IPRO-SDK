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

#ifndef HAL_PIO_CMSIS_DAP_H
#define HAL_PIO_CMSIS_DAP_H
/*
 * hal_pio_cmsis_dap - PIO debug-probe HAL (peer to hal_pio_scan, both on drv_pio).
 * Owns the JTAG PIO program, GLB clock, pinmux, SM config, and a dumb per-TCK
 * shift primitive. No CMSIS-DAP knowledge (that is jtag_pio.c). Host-testable:
 * the SM/program setup + shift loop run under -DHAL_PIO_CMSIS_DAP_HOSTTEST.
 */
#include <stdint.h>
#include <stdbool.h>

typedef enum { HAL_DAP_PORT_OFF, HAL_DAP_PORT_JTAG, HAL_DAP_PORT_SWD } hal_dap_port_t;

typedef struct {
    uint8_t  pin_tck;     /* shared SWCLK; TCK = side-set (any GPIO) */
    uint8_t  pin_tdi;     /* TDI; out-group bit0 unless out_swap (then bit1) */
    uint8_t  pin_tms;     /* TMS; out-group bit1 unless out_swap (then bit0/base) */
    uint8_t  pin_tdo;     /* TDO; in pin (any GPIO, not in the out-group) */
    uint8_t  pin_nreset;  /* nSRST - plain gpio open-drain; 0xFF = none */
    uint8_t  pin_ntrst;   /* nTRST - plain gpio; 0xFF = none */
    uint8_t  pio_sm;      /* SM index (default 0) */
    uint8_t  out_swap;    /* 0: out-group base=TDI, TMS=TDI+1 (normal).
                           * 1: base=TMS, TDI=TMS+1 -- the caller's encoder must
                           *    pack bit0=TMS,bit1=TDI to match (REPIN-to-IPRO6). */
    uint8_t  tms_gpio;    /* 1: TMS is a plain GPIO driven by the CPU (quasi-static
                           * per JTAG_Sequence entry), NOT a PIO out-group member. The
                           * cell shifts TDI only (`out pins,1`); TDI/TMS need no
                           * adjacency. out_swap is ignored. See JTAG_TMS_GPIO profiles. */
    uint8_t  pio_clk_div, pio_clk_sel;  /* GLB pre-divider + source (as hal_pio_scan) */
    uint16_t clkdiv_int; uint8_t clkdiv_frac;  /* SM clkdiv -> TCK rate */
} hal_pio_cmsis_dap_cfg_t;

void     hal_pio_cmsis_dap_board_init(void);
int      hal_pio_cmsis_dap_init(const hal_pio_cmsis_dap_cfg_t *cfg);
void     hal_pio_cmsis_dap_deinit(void);
int      hal_pio_cmsis_dap_set_port(hal_dap_port_t port);
/* SWRST + re-enable the shared PIO block, wiping all SM state. The REPIN time-mux calls
 * this before re-initialising the active mode's SM so a disabled SM leaves no stale
 * pin-direction claim on a shared pad. */
void     hal_pio_cmsis_dap_pio_reclock(void);
uint32_t hal_pio_cmsis_dap_set_clock(uint32_t hz);

/* Dumb shift: feed n out-words (bit0=TDI, bit1=TMS), one TCK each; capture TDO
 * (bit0) into rx[i] if rx != NULL. The only hot-path primitive. In the tms_gpio
 * cell each word is TDI-only (bit0), TMS being driven separately. */
void     hal_pio_cmsis_dap_shift(const uint32_t *tx, uint32_t n, uint32_t *rx);

/* tms_gpio cell only: drive the CPU-owned TMS pin (no-op unless cfg.tms_gpio). Call
 * before shifting a constant-TMS run (JTAG) or per bit (SWJ). */
void     hal_pio_cmsis_dap_set_tms(uint32_t level);

/* Cold path: DAP_SWJ_Pins. Bit layout = DAP_SWJ_* (bit0 TCK,1 TMS,2 TDI,3 TDO,5 nTRST,7 nRESET). */
void     hal_pio_cmsis_dap_set_pins(uint8_t value, uint8_t select);
uint8_t  hal_pio_cmsis_dap_get_pins(void);

/* host-test seam: build the JTAG PIO program into prog[], return instr count */
uint8_t  hal_pio_cmsis_dap_build_jtag_prog(uint16_t prog[8], uint8_t *wrap_top);
/* Same cell but `out pins,1` (TDI only) -- for the tms_gpio non-adjacent profile. */
uint8_t  hal_pio_cmsis_dap_build_jtag_prog_tdi(uint16_t prog[8], uint8_t *wrap_top);

#endif /* HAL_PIO_CMSIS_DAP_H */
