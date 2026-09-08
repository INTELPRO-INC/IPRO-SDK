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
 * Clock-tree register dump.
 *
 * Shell command `clock_dump` prints every register that the offline
 * `clock_tool` (tools/clock_tree) needs to decode the IPRO7 clock tree.
 * The output format matches the existing `p` command, so users can:
 *
 *   1. Run `clock_dump` on the DUT shell and save the log
 *   2. Feed the log to `clock_tool --yaml chips/ipro7.yaml --input dump.txt`
 *
 * If the YAML clock DB grows to cover more registers, append them to
 * CLOCK_DUMP_REGS[] below. The tool's `--list-regs` output is the
 * reference list.
 */

#include "shell.h"

#include <stdio.h>
#include <stdint.h>

static const struct {
    uint32_t    addr;
    const char *desc;
} CLOCK_DUMP_REGS[] = {
    /* CCI — CLKPLL */
    { 0x30009418, "CCI.CLKPLL_SDM       (sdmin -> VCO freq)" },
    { 0x30009404, "CCI.CLKPLL_TOP_CTRL  (postdiv, refdiv)" },

    /* HBN — xclk / root / f32k / uart mirrors */
    { 0x3008F030, "HBN.HBN_GLB          (uart/f32k/root mirror)" },

    /* GLB — main clock control */
    { 0x30000000, "GLB.CLK_CFG0         (xclk, pll_sel, root, h/bclk gates)" },
    { 0x30000004, "GLB.CLK_CFG1         (hclk_div, bclk_div, fclk_en)" },
    { 0x30000008, "GLB.CLK_CFG2         (qdec, audio, usb_en)" },
    { 0x3000000C, "GLB.CLK_CFG3         (uart, flash, ir)" },
    { 0x30000010, "GLB.CLK_CFG4         (spi, i2c)" },
    { 0x30000014, "GLB.CLK_CFG5         (cam_ref, spi2, psram)" },
    { 0x30000018, "GLB.CLK_CFG6         (i2s_ref)" },
    { 0x30000028, "GLB.SWRST_CFG2       (pka_clk_sel)" },
    { 0x300000B4, "GLB.GPADC_32M_SRC    (gpadc clock)" },
    { 0x300000B8, "GLB.DIG32K_WAKEUP    (gpdac, f32k_div)" },
    { 0x30000430, "GLB.SDH_CFG0         (sdio master clock)" },

    /* PWM */
    { 0x3000F040, "PWM.MC0_CONFIG0      (pwm clock)" },

    /* TIMER */
    { 0x30010000, "TIMER.TCCR           (timer0/1/wdt sel)" },
    { 0x300100BC, "TIMER.TCDR           (timer0/1 div)" },
};

#define CLOCK_DUMP_N (sizeof(CLOCK_DUMP_REGS) / sizeof(CLOCK_DUMP_REGS[0]))

static int shell_clock_dump(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("# clock_dump: %u registers\r\n", (unsigned)CLOCK_DUMP_N);
    for (unsigned i = 0; i < CLOCK_DUMP_N; i++) {
        uint32_t addr = CLOCK_DUMP_REGS[i].addr;
        uint32_t val  = *(volatile uint32_t *)(uintptr_t)addr;
        printf("0x%08x: %08x  # %s\r\n",
               (unsigned)addr, (unsigned)val, CLOCK_DUMP_REGS[i].desc);
    }
    printf("# clock_dump end\r\n");
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(shell_clock_dump, clock_dump,
                       Dump all clock-tree registers for tools/clock_tree);
