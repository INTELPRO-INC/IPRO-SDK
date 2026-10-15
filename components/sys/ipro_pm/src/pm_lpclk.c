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
 * @file pm_lpclk.c
 * @brief ipro_pm 32 kHz sleep clock: calibration against the machine timer
 *        and the machine-timer (MTIME) rebase after a PDS.
 *
 * MTIME (4 MHz, from the crystal) is the time base of the RTOS tick and of
 * OpenThread (otPlatTimeGet() = MTIME / 4).  PDS31 stops or resets it, so
 * after every PDS it is written forward to where it would be had it kept
 * running: the sleep is measured on the always-on HBN RTC (32 kHz, RC32K on
 * the EVB) and converted with the RTC's frequency error learned while awake.
 *
 * Both ends of a measurement are taken on an RTC edge (spin until the counter
 * moves, then read MTIME), so the 30.5 us RTC quantisation does not enter the
 * calibration: two edge-synced pairs over an awake span give the RTC's rate
 * against MTIME to about a microsecond per span.
 *
 * Everything runs with interrupts masked (the PDS executors) except init.
 */
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>

#include "core_feature_timer.h"
#include "drv_hbn.h"
#include "pm_internal.h"

#ifndef CONFIG_IPRO_PM_LPCLK_BOUND_PPM
#define CONFIG_IPRO_PM_LPCLK_BOUND_PPM 200
#endif
#ifndef CONFIG_IPRO_PM_LPCLK_UNCAL_PPM
#define CONFIG_IPRO_PM_LPCLK_UNCAL_PPM 1000
#endif

#define PM_RTC_MASK ((UINT64_C(1) << 45) - UINT64_C(1))
/* MTIME counts per microsecond (configSYSTICK_CLOCK_HZ) */
#define PM_MTIME_PER_US (configSYSTICK_CLOCK_HZ / 1000000UL)
/* an RTC edge comes every 30.5 us: give up after 64 us of MTIME */
#define PM_EDGE_SPIN_COUNTS (64U * PM_MTIME_PER_US)
#ifndef CONFIG_IPRO_PM_LPCLK_CAL_WINDOW_MS
#define CONFIG_IPRO_PM_LPCLK_CAL_WINDOW_MS 250
#endif
/* calibration: ignore spans shorter than this, fold one in per window */
#define PM_CAL_MIN_SPAN_US 2000U
#define PM_CAL_WINDOW_US ((uint32_t)CONFIG_IPRO_PM_LPCLK_CAL_WINDOW_MS * 1000U)
/* calibrated after this much measured time */
#define PM_CAL_VALID_US 2000000U

static struct {
    int32_t ppm;            /* RTC frequency error vs MTIME (positive: RTC fast) */
    uint64_t cal_us;        /* measured time folded into ppm */
    uint64_t acc_rtc;       /* current window, RTC ticks */
    uint64_t acc_mtime;     /* current window, MTIME counts */
    pm_clkpair_t span;      /* start of the awake span being measured */
    bool span_valid;
} s_clk;

/* last rebase, for the bench (`pm time`): MTIME and MTIMECTL as the wake
 * found them, MTIME before the sleep, where it was written to */
volatile uint64_t ipro_pm_lpclk_last_mtime_before;
volatile uint64_t ipro_pm_lpclk_last_mtime_woke;
volatile uint64_t ipro_pm_lpclk_last_mtime_target;
volatile uint32_t ipro_pm_lpclk_last_mtimectl_woke;
volatile uint32_t ipro_pm_lpclk_rebases;
volatile uint32_t ipro_pm_lpclk_windows;
volatile int32_t ipro_pm_lpclk_last_window_ppm;
volatile uint32_t ipro_pm_lpclk_edge_timeouts;

/* -------------------------------------------------------------------------
 * Pure conversions (host-tested)
 * -----------------------------------------------------------------------*/

/* MTIME counts for RTC ticks at a given RTC error: nominal
 * ticks * 4e6 / 32768 = ticks * 15625 / 128, slowed by ppm. */
uint64_t pm_lpclk_ticks_to_counts_ppm(uint64_t ticks, int32_t ppm)
{
    uint64_t nominal = (ticks * 15625U) / 128U;
    int64_t adj = ((int64_t)nominal * (int64_t)ppm) / 1000000;

    return (uint64_t)((int64_t)nominal - adj);
}

uint64_t pm_lpclk_us_to_ticks_ppm(uint64_t us, int32_t ppm)
{
    uint64_t nominal = (us * 512U) / 15625U;
    int64_t adj = ((int64_t)nominal * (int64_t)ppm) / 1000000;

    return (uint64_t)((int64_t)nominal + adj);
}

/* RTC frequency error from a window: RTC-implied counts vs MTIME counts. */
int32_t pm_lpclk_window_ppm(uint64_t rtc_ticks, uint64_t mtime_counts)
{
    int64_t rtc_counts = (int64_t)((rtc_ticks * 15625U) / 128U);

    if (mtime_counts == 0U) {
        return 0;
    }
    return (int32_t)(((rtc_counts - (int64_t)mtime_counts) * 1000000) / (int64_t)mtime_counts);
}

/* Where MTIME should be at @p after_edge_mtime (read at the post-wake RTC edge,
 * possibly from a reset or stopped counter): the pre-sleep MTIME plus the
 * RTC-measured sleep.  Never moves MTIME backwards. */
uint64_t pm_lpclk_rebase_target(const pm_clkpair_t *before, uint64_t after_rtc, uint64_t after_edge_mtime,
                                int32_t ppm)
{
    uint64_t dr = (after_rtc - before->rtc) & PM_RTC_MASK;
    uint64_t target = before->mtime + pm_lpclk_ticks_to_counts_ppm(dr, ppm);

    return (target > after_edge_mtime) ? target : after_edge_mtime;
}

/* -------------------------------------------------------------------------
 * Calibration
 * -----------------------------------------------------------------------*/
static void pm_lpclk_fold(uint64_t rtc_ticks, uint64_t mtime_counts)
{
    int32_t w;

    s_clk.acc_rtc += rtc_ticks;
    s_clk.acc_mtime += mtime_counts;
    if (s_clk.acc_mtime < ((uint64_t)PM_CAL_WINDOW_US * PM_MTIME_PER_US)) {
        return;
    }
    w = pm_lpclk_window_ppm(s_clk.acc_rtc, s_clk.acc_mtime);
    ipro_pm_lpclk_last_window_ppm = w;
    ipro_pm_lpclk_windows++;
    /* first window taken as is, then an exponential average (1/4): RC32K
     * wanders by ~100 ppm per second and drifts with the die temperature;
     * the average follows within a few windows */
    if (s_clk.cal_us == 0U) {
        s_clk.ppm = w;
    } else {
        s_clk.ppm += (w - s_clk.ppm) / 4;
    }
    s_clk.cal_us += s_clk.acc_mtime / PM_MTIME_PER_US;
    s_clk.acc_rtc = 0U;
    s_clk.acc_mtime = 0U;
}

/* The awake span that started at the last wake (or at init) ends at @p now:
 * fold it into the calibration. */
void pm_lpclk_span_end(const pm_clkpair_t *now)
{
    if (s_clk.span_valid) {
        uint64_t dr = (now->rtc - s_clk.span.rtc) & PM_RTC_MASK;
        uint64_t dm = now->mtime - s_clk.span.mtime;

        if ((now->mtime > s_clk.span.mtime) && (dm >= ((uint64_t)PM_CAL_MIN_SPAN_US * PM_MTIME_PER_US))) {
            pm_lpclk_fold(dr, dm);
        }
    }
    s_clk.span_valid = false;
}

void pm_lpclk_span_start(const pm_clkpair_t *now)
{
    s_clk.span = *now;
    s_clk.span_valid = true;
}

int32_t pm_lpclk_ppm(void)
{
    return s_clk.ppm;
}

uint32_t pm_lpclk_bound_ppm(void)
{
    return (s_clk.cal_us >= PM_CAL_VALID_US) ? (uint32_t)CONFIG_IPRO_PM_LPCLK_BOUND_PPM
                                             : (uint32_t)CONFIG_IPRO_PM_LPCLK_UNCAL_PPM;
}

uint64_t pm_lpclk_us_to_ticks(uint64_t us)
{
    return pm_lpclk_us_to_ticks_ppm(us, s_clk.ppm);
}

uint64_t pm_lpclk_ticks_to_us(uint64_t ticks)
{
    return pm_lpclk_ticks_to_counts_ppm(ticks, s_clk.ppm) / PM_MTIME_PER_US;
}

void pm_lpclk_feedback(int32_t err_ppm, uint32_t span_us)
{
    /* same averaging as a calibration window; a span too short to mean much
     * (quantisation of the measurement) is ignored */
    if (span_us < 250000U) {
        return;
    }
    s_clk.ppm += (err_ppm - s_clk.ppm) / 4;
}

/* -------------------------------------------------------------------------
 * Hardware
 * -----------------------------------------------------------------------*/
static uint64_t pm_rtc_read(void)
{
    uint32_t lo;
    uint32_t hi;

    HBN_Get_RTC_Timer_Val(&lo, &hi);
    return (((uint64_t)hi << 32) | lo) & PM_RTC_MASK;
}

/* bench: 0 takes the pair without waiting for an RTC edge (`pm xedge`) */
volatile uint32_t ipro_pm_lpclk_edge_sync = 1U;

void pm_lpclk_pair_now(pm_clkpair_t *out)
{
    uint64_t r0 = pm_rtc_read();
    uint64_t start = SysTimer_GetLoadValue();
    uint64_t r;

    if (ipro_pm_lpclk_edge_sync == 0U) {
        out->mtime = start;
        out->rtc = r0;
        return;
    }

    do {
        r = pm_rtc_read();
        if ((SysTimer_GetLoadValue() - start) > PM_EDGE_SPIN_COUNTS) {
            /* MTIME not running (just woken) or the RTC stalled: take the
             * pair unsynchronised rather than spin */
            ipro_pm_lpclk_edge_timeouts++;
            break;
        }
    } while (r == r0);
    out->mtime = SysTimer_GetLoadValue();
    out->rtc = r;
}

uint32_t pm_lpclk_rebase(const pm_clkpair_t *before)
{
    pm_clkpair_t after;
    uint64_t target;
    uint64_t cur;
    uint64_t dr;

    ipro_pm_lpclk_last_mtimectl_woke = SysTimer_GetControlValue();
    ipro_pm_lpclk_last_mtime_woke = SysTimer_GetLoadValue();
    ipro_pm_lpclk_last_mtime_before = before->mtime;
    /* PDS can bring MTIMECTL back with TIMESTOP set */
    SysTimer_Start();
    pm_lpclk_pair_now(&after);
    target = pm_lpclk_rebase_target(before, after.rtc, after.mtime, s_clk.ppm);
    SysTimer_Stop();
    cur = SysTimer_GetLoadValue();
    /* keep the time elapsed since the edge */
    if (target > after.mtime) {
        uint64_t moved = target + (cur - after.mtime);

        if (moved > cur) {
            SysTimer_SetLoadValue(moved);
        }
    }
    SysTimer_Start();
    ipro_pm_lpclk_last_mtime_target = target;
    ipro_pm_lpclk_rebases++;
    after.mtime = target;
    pm_lpclk_span_start(&after);
    dr = (after.rtc - before->rtc) & PM_RTC_MASK;
    return (uint32_t)pm_lpclk_ticks_to_us(dr);
}

/* A first estimate before any PDS: the PDS regime leaves only short awake
 * spans, so a calibration built from them alone takes tens of seconds, and
 * RC32K can be ~1500 ppm off (0.7 ms per 0.5 s sleep: a CSL window missed).
 * Edge-synced over this span: ~20 ppm. */
#define PM_CAL_BOOT_US 50000U

#if defined(CONFIG_IPRO_PM_RC32K_CAL)
#include "hal_pm.h"
#include "hal_sys.h"
#endif

/* bench: what the boot 32 kHz setup found (`pm time`) */
volatile int32_t ipro_pm_lpclk_rc32k_cal = -1;

void pm_lpclk_init(void)
{
    pm_clkpair_t a;
    pm_clkpair_t b;
    uint64_t start;

#if defined(CONFIG_IPRO_PM_RC32K_CAL)
    /* The HBN RTC is the only timed PDS wake: as the BLE controller does,
     * select RC32K explicitly as its 32 kHz source and calibrate it against
     * the crystal (~2 s, once).  Nothing may call HBN_32K_Sel() afterwards:
     * it reapplies the efuse trim. */
    pm_pds_set_32k_src(0U, 0U);
    ipro_pm_lpclk_rc32k_cal = hal_rc32k_force_cal();
#endif
    memset(&s_clk, 0, sizeof(s_clk));
    pm_lpclk_pair_now(&a);
    start = SysTimer_GetLoadValue();
    while ((SysTimer_GetLoadValue() - start) < ((uint64_t)PM_CAL_BOOT_US * PM_MTIME_PER_US)) {
    }
    pm_lpclk_pair_now(&b);
    if (b.mtime > a.mtime) {
        s_clk.ppm = pm_lpclk_window_ppm((b.rtc - a.rtc) & PM_RTC_MASK, b.mtime - a.mtime);
        s_clk.cal_us = (b.mtime - a.mtime) / PM_MTIME_PER_US;
        ipro_pm_lpclk_last_window_ppm = s_clk.ppm;
        ipro_pm_lpclk_windows++;
    }
    pm_lpclk_span_start(&b);
}
