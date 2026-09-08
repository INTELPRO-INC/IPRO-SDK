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

/**
 * @file rc32k_demo.c
 * @brief RC32K shell commands.
 *
 *   rc32k_status               Show current 32K source.
 *   rc32k_check  [window_ms]   Measure RC32K vs MTimer (XTAL32M reference).
 *   rc32k_force_cal            Run full cal flow + apply (no efuse needed).
 *   rc32k_sdm_compare          Force cal then compare byp=0 vs byp=1.
 *
 * Reference clock: CPU MTimer is XTAL32M-derived (precision ≪ 100 ppm), so
 * we treat it as ground truth and measure RC32K drift against it.
 *
 * History: this file used to host 18 experimental rc32k commands (sweep,
 * sw_cal, hw_cal, hw_cal_v2, hw_cal_tune, vendor_cal, vendor_cal_rmw,
 * compare, check_xtal, bench, cdet_*) accumulated during the 2026-05 cal
 * algorithm refactor. After that work converged on a single production
 * cal flow (commits fdd42db9d + b37a08eb4), the 14 dev/debug variants
 * were removed (2026-05-06) — all their useful behavior is now inside
 * hal_rc32k_force_cal. Post-boot CDET commands were also non-functional
 * on this silicon (see hal_sys.c CDET comment) and were dropped.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "shell.h"
#include "drv_hbn.h"
#include "hal_sys.h"

static const char *src_name(HBN_32K_CLK_Type s)
{
    switch (s) {
    case HBN_32K_RC:   return "RC32K";
    case HBN_32K_XTAL: return "XTAL32K";
    case HBN_32K_DIG:  return "DIG32K";
    default:           return "?";
    }
}

static int cmd_rc32k_status(int argc, char **argv)
{
    (void)argc; (void)argv;
    HBN_32K_CLK_Type src = HBN_Get_32K_Sel();
    printf("32K source: %s\r\n", src_name(src));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rc32k_status, rc32k_status, Show 32K source);

/* rc32k_check — measure current RC32K freq vs MTimer over an N-ms window.
 * No re-cal; reports the drift of whatever cap_sel/code is currently
 * applied (e.g., from boot-time efuse trim). Default window 200 ms,
 * range [10, 2000] ms. Use 1000 ms for ~30 ppm RTC quantization. */
static int cmd_rc32k_check(int argc, char **argv)
{
    uint32_t window_ms = (argc >= 2) ? (uint32_t)atoi(argv[1]) : 200U;
    if (window_ms < 10U)   window_ms = 10U;
    if (window_ms > 2000U) window_ms = 2000U;

    HBN_32K_CLK_Type orig = HBN_Get_32K_Sel();
    if (orig != HBN_32K_RC) {
        HBN_32K_Sel(HBN_32K_RC);
    }

    int err = hal_rc32k_measure_error_ppm(window_ms * 1000U);
    if (err == INT32_MAX) {
        printf("rc32k_check: measurement failed\r\n");
    } else {
        printf("rc32k_check: window=%ums  err=%+d ppm  (target 32768 Hz)\r\n",
               (unsigned)window_ms, err);
    }

    if (orig != HBN_32K_RC) {
        HBN_32K_Sel(orig);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rc32k_check, rc32k_check,
    Measure RC32K vs XTAL32M MTimer [window_ms]);

/* rc32k_force_cal — manually trigger full cal (HW FSM hint + cap walk +
 * R 10-bit binary search + verify) and apply the result, regardless of
 * efuse trim state. Use to recalibrate at runtime when efuse trim is
 * missing or stale. SDM stage was dropped 2026-05-05; SDM=0,
 * sdm_bypass=1 always (see hal_sys.c). */
static int cmd_rc32k_force_cal(int argc, char **argv)
{
    (void)argc; (void)argv;
    int cap = hal_rc32k_force_cal();
    if (cap < 0) {
        printf("rc32k_force_cal: failed\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rc32k_force_cal, rc32k_force_cal,
    Force full calibration and apply result);

/* rc32k_sdm_compare — diagnostic for evaluating whether the SDM modulator
 * is worth re-enabling on a given silicon revision. After force_cal,
 * measures the same 13-bit code with sdm_bypass=0 then sdm_bypass=1 and
 * reports both. Diff reveals how much the 3-bit SDM actually moves
 * frequency on this DUT.
 *
 * On the IC family used during the 2026-05 cal refactor, byp=1 produced
 * 30% better post-cal stability than byp=0 (1σ 287 vs 447 ppm) and the
 * SDM modulator with input=0 was not a no-op (+861 ppm bias). Production
 * cal flow therefore locks SDM=0 + sdm_bypass=1. Re-run this command on
 * future silicon to re-evaluate that decision. */
static int cmd_rc32k_sdm_compare(int argc, char **argv)
{
    (void)argc; (void)argv;

    int cap = hal_rc32k_force_cal();
    if (cap < 0) {
        printf("rc32k_sdm_compare: cal failed\r\n");
        return -1;
    }

    /* HBN_RC32K_CTRL1 register at HBN_BASE + 0x208 (HBN_BASE = 0x3008F000
     * per ipro7.h). Direct register access here so we can toggle just the
     * sdm_bypass bit without going through hal_sys.c's static helpers. */
    const uint32_t HBN_RC32K_CTRL1 = 0x3008F208U;
    uint32_t reg = *(volatile uint32_t *)HBN_RC32K_CTRL1;
    uint16_t code13 = (uint16_t)(reg & 0x1FFFU);
    int sdm = code13 & 0x7;
    int r10 = (code13 >> 3) & 0x3FF;

    /* Apply same code with sdm_bypass=0 (SDM enabled), 1 ms settle, measure. */
    uint32_t reg_byp0 = (0xBU << 28) | (0x0U << 20) | (0x1U << 16) | code13;
    *(volatile uint32_t *)HBN_RC32K_CTRL1 = reg_byp0;
    arch_delay_us(1000);
    int e_byp0 = hal_rc32k_measure_error_ppm(1000000U);

    /* Apply same code with sdm_bypass=1 (SDM disabled, HW default), measure. */
    uint32_t reg_byp1 = (0xBU << 28) | (0x1U << 20) | (0x1U << 16) | code13;
    *(volatile uint32_t *)HBN_RC32K_CTRL1 = reg_byp1;
    arch_delay_us(1000);
    int e_byp1 = hal_rc32k_measure_error_ppm(1000000U);

    /* Restore the state cal originally chose: bypass=1 if SDM=0 else 0. */
    int byp_final = (sdm == 0) ? 1 : 0;
    uint32_t reg_final = (0xBU << 28) | ((uint32_t)byp_final << 20) | (0x1U << 16) | code13;
    *(volatile uint32_t *)HBN_RC32K_CTRL1 = reg_final;
    arch_delay_us(2);

    printf("[RC32K_SDMCMP] code=0x%04x r=0x%03x sdm=%d byp0=%+d byp1=%+d diff=%+d\r\n",
           (unsigned)code13, (unsigned)r10, sdm,
           e_byp0, e_byp1, e_byp0 - e_byp1);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_rc32k_sdm_compare, rc32k_sdm_compare,
    Force cal then compare ppm with sdm_bypass=0 vs =1);
