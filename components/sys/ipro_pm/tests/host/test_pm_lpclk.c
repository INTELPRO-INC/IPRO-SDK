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

/* Host unit tests for the ipro_pm 32 kHz clock calibration, the MTIME rebase
 * and the idle-executor sleep sizing.
 * Build and run: make -C components/sys/ipro_pm/tests/host */
#include <stdio.h>
#include <stdlib.h>

#include "pm_internal.h"

TickType_t host_tick;
uint64_t host_rtc;
uint64_t (*host_rtc_fn)(void);
uint64_t host_mtime;
int host_mtime_stopped;
uint32_t host_mtime_autostep;
static int s_failures;

/* Model: real time in MTIME counts = host_mtime + s_off (s_off moves when a
 * PDS resets MTIME); the RTC runs +300 ppm against it. */
static long double s_off;
static uint64_t model_rtc(void)
{
    long double real = (long double)host_mtime + s_off;

    return (uint64_t)(real * 32768.0L * 1.0003L / 4000000.0L) + 12345U;
}

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                  \
            s_failures++;                                                           \
        }                                                                           \
    } while (0)

#define RTC_MASK ((UINT64_C(1) << 45) - 1U)

static void test_conversions(void)
{
    /* one second of 32 kHz ticks is 4,000,000 MTIME counts */
    CHECK(pm_lpclk_ticks_to_counts_ppm(32768U, 0) == 4000000U);
    /* an RTC running 100 ppm fast overcounts: fewer real counts */
    CHECK(pm_lpclk_ticks_to_counts_ppm(32768U, 100) == 3999600U);
    CHECK(pm_lpclk_ticks_to_counts_ppm(32768U, -250) == 4001000U);
    CHECK(pm_lpclk_us_to_ticks_ppm(1000000U, 0) == 32768U);
    CHECK(pm_lpclk_us_to_ticks_ppm(1000000U, 100) == 32771U);
    CHECK(pm_lpclk_us_to_ticks_ppm(0U, 500) == 0U);

    /* window: 32800 ticks against 4 s... 1 s of MTIME = +976 ppm */
    CHECK(pm_lpclk_window_ppm(32800U, 4000000U) == 976);
    CHECK(pm_lpclk_window_ppm(32768U, 4000000U) == 0);
    CHECK(pm_lpclk_window_ppm(32768U, 4002000U) == -499);
    CHECK(pm_lpclk_window_ppm(1U, 0U) == 0);
}

static void test_rebase_target(void)
{
    pm_clkpair_t before = { .rtc = 1000U, .mtime = 5000000U };

    /* MTIME reset by the PDS: target is before + 1 s */
    CHECK(pm_lpclk_rebase_target(&before, 1000U + 32768U, 100U, 0) == 9000000U);
    /* MTIME kept running and is already further: never backwards */
    CHECK(pm_lpclk_rebase_target(&before, 1000U + 32768U, 9500000U, 0) == 9500000U);
    /* RTC 45-bit wrap */
    before.rtc = RTC_MASK - 10U;
    CHECK(pm_lpclk_rebase_target(&before, 22U, 0U, 0) == 5000000U + ((33U * 15625U) / 128U));
    /* with a +200 ppm RTC: the sleep was a bit shorter in MTIME */
    before.rtc = 0U;
    CHECK(pm_lpclk_rebase_target(&before, 32768U * 10U, 0U, 200) == 5000000U + 40000000U - 8000U);
}

static void awake(uint64_t us)
{
    host_mtime += us * 4U;
}

static void test_calibration_and_rebase(void)
{
    pm_clkpair_t p;

    host_mtime = 777U;
    host_mtime_autostep = 3U; /* ~1 us per read while spinning for an edge */
    host_rtc_fn = model_rtc;
    pm_lpclk_init();
    /* the 50 ms boot measurement already sees the +300 ppm RTC, but 50 ms
     * is not a calibration yet */
    CHECK(abs(pm_lpclk_ppm() - 300) <= 60);
    CHECK(pm_lpclk_bound_ppm() == 1000U);

    /* short spans are ignored */
    {
        int32_t before = pm_lpclk_ppm();

        awake(1000U);
        pm_lpclk_pair_now(&p);
        pm_lpclk_span_end(&p);
        CHECK(pm_lpclk_ppm() == before);
    }

    /* three awake seconds in 50 ms spans, as between PDS cycles */
    for (int i = 0; i < 60; i++) {
        pm_lpclk_span_start(&p);
        awake(50000U);
        pm_lpclk_pair_now(&p);
        pm_lpclk_span_end(&p);
    }
    CHECK(abs(pm_lpclk_ppm() - 300) <= 40);
    CHECK(pm_lpclk_bound_ppm() == 200U); /* calibrated after 2 s */

    /* a PDS of 500 ms that resets MTIME to 0 */
    {
        pm_clkpair_t before;
        uint64_t expect;
        uint32_t slept;

        pm_lpclk_span_start(&p);
        awake(20000U);
        pm_lpclk_pair_now(&before);
        pm_lpclk_span_end(&before);
        /* 500 ms of real time pass; MTIME comes back reset and stopped */
        expect = before.mtime + 2000000U;
        s_off += (long double)host_mtime + 2000000.0L - 40.0L;
        host_mtime = 40U;
        host_mtime_stopped = 1;
        slept = pm_lpclk_rebase(&before);
        CHECK(host_mtime_stopped == 0);
        CHECK(llabs((long long)host_mtime - (long long)expect) <= 800); /* < 200 us */
        CHECK(abs((int)slept - 500000) <= 200);
    }
    host_rtc_fn = NULL;
    host_mtime_autostep = 0U;
}

static void test_idle_plan(void)
{
    /* 485 ms to the CSL window: minus wake 1.5 ms, guard 0.3 ms, 200 ppm */
    CHECK(pm_exec_idle_plan(485000U, 1500U, 300U, 200U) == 485000U - 1500U - 300U - 97U);
    /* before calibration the bound costs more */
    CHECK(pm_exec_idle_plan(485000U, 1500U, 300U, 1000U) == 485000U - 1500U - 300U - 485U);
    /* nothing left */
    CHECK(pm_exec_idle_plan(1700U, 1500U, 300U, 200U) == 0U);
    CHECK(pm_exec_idle_plan(0U, 1500U, 300U, 200U) == 0U);
    /* saturates */
    CHECK(pm_exec_idle_plan(UINT64_C(1) << 40, 0U, 0U, 0U) == UINT32_MAX);
}

int main(void)
{
    test_conversions();
    test_rebase_target();
    test_calibration_and_rebase();
    test_idle_plan();
    if (s_failures != 0) {
        printf("ipro_pm lpclk tests: %d failure(s)\n", s_failures);
        return 1;
    }
    printf("ipro_pm lpclk tests: all passed\n");
    return 0;
}
