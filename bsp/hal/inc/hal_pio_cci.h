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

#ifndef HAL_PIO_CCI_H
#define HAL_PIO_CCI_H
#include <stdint.h>

/* CCI master over a dedicated PIO state machine (SM1; the cmsis-dap JTAG uses
 * SM0). Reuses the JTAG PIO cell structure: side-set = CLK, out-group
 * {DI = bit0, CS = bit1} (CS MUST be DI+1), input DO sampled on the CLK-HIGH
 * phase (= sample just after the rising edge, which the CCI slave needs).
 *
 * Requires the PIO block to be already clocked + the shared cell program loaded
 * at origin 0 -- the cmsis-dap board_init does both when it brings up JTAG. */
typedef struct {
    uint8_t  pin_clk;       /* side-set (CLK) */
    uint8_t  pin_di;        /* out-group: DI (master -> DUT) */
    uint8_t  pin_cs;        /* out-group: CS */
    uint8_t  pin_do;        /* in (DO, DUT -> master) */
    uint8_t  pio_sm;        /* SM index (1) */
    uint8_t  out_swap;      /* 0: out-group base=DI, CS=DI+1 (encoder bit0=DI,bit1=CS).
                             * 1: base=CS, DI=CS+1 -- the caller's encoder must pack
                             *    bit0=CS,bit1=DI to match (co-pin where CS is the lower pad). */
    uint16_t clkdiv_int;    /* PIO clock divisor; CLK = 96 MHz / 12 / clkdiv */
    uint8_t  clkdiv_frac;
} hal_pio_cci_cfg_t;

/* Configure, pinmux, and enable the CCI SM. */
int  hal_pio_cci_init(const hal_pio_cci_cfg_t *cfg);

/* Shift n bit-cells. tx[i] bit0 = DI level, bit1 = CS level for clock i.
 * rx[i] bit0 = DO sampled on clock i's high phase (rx may be NULL). */
void hal_pio_cci_shift(const uint32_t *tx, uint32_t n, uint32_t *rx);

#endif /* HAL_PIO_CCI_H */
