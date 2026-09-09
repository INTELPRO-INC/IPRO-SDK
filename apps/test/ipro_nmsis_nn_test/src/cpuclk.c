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
 * @file cpuclk.c
 * @brief Measure the actual core clock, and cross-check three sources for it.
 *
 * Every cycles-to-microseconds figure in this benchmark rests on the core
 * clock, and nothing in the app's .config states it - the 192 MHz used in the
 * printouts is just the datasheet maximum. This resolves it against a known
 * time base instead of assuming.
 *
 * Three readings, because they can disagree and the disagreement is itself
 * informative:
 *   1. SystemCoreClockGet()       - what the SDK believes
 *   2. mcycle vs mtime            - measured, mtime rate from SOC_TIMER_TICK_PER_US
 *   3. FreeRTOS ticks vs mtime    - whether the OS tick really is 1 kHz
 *
 * (3) is worth having: ipro7.h sets SOC_TIMER_TICK_PER_US = 4, i.e. a 4 MHz
 * mtime, while portable_ipro7/FreeRTOSConfig.h declares
 * configCPU_CLOCK_HZ = 1000000. If the port takes that literally as the CLINT
 * rate, every vTaskDelay() on this chip is 4x shorter than it reads.
 */

#include <stdio.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ipro7.h"          /* SOC_TIMER_TICK_PER_US */
#include "drv_clock.h"      /* SystemCoreClockGet */

#include "cpuclk.h"

static inline uint32_t rd_mcycle(void)
{
    uint32_t v;
    __asm__ volatile("csrr %0, mcycle" : "=r"(v));
    return v;
}

void cpuclk_report(void)
{
    printf("\r\nCore clock cross-check\r\n");

    const uint32_t sdk_hz = SystemCoreClockGet();
    printf("  SystemCoreClockGet()   : %lu Hz (%lu.%02lu MHz)\r\n",
           (unsigned long)sdk_hz,
           (unsigned long)(sdk_hz / 1000000u),
           (unsigned long)((sdk_hz % 1000000u) / 10000u));

    printf("  SOC_TIMER_TICK_PER_US  : %lu  -> mtime = %lu Hz\r\n",
           (unsigned long)SOC_TIMER_TICK_PER_US,
           (unsigned long)(SOC_TIMER_TICK_PER_US * 1000000UL));

    /* Measure over ~200 ms of mtime. Interrupts stay enabled on purpose: the
     * benchmark cycles were measured with them enabled too, so a clock derived
     * the same way keeps the conversion self-consistent. */
    const uint64_t mt_window = (uint64_t)SOC_TIMER_TICK_PER_US * 1000ULL * 200ULL;

    const uint64_t mt0 = SysTimer_GetLoadValue();
    const uint32_t mc0 = rd_mcycle();
    const TickType_t os0 = xTaskGetTickCount();

    uint64_t mt1;
    do {
        mt1 = SysTimer_GetLoadValue();
    } while ((mt1 - mt0) < mt_window);

    const uint32_t mc1 = rd_mcycle();
    const TickType_t os1 = xTaskGetTickCount();

    const uint32_t cycles = mc1 - mc0;
    const uint64_t mt_delta = mt1 - mt0;
    const uint32_t us = (uint32_t)(mt_delta / SOC_TIMER_TICK_PER_US);

    /* Hz = cycles / seconds = cycles * 1e6 / us; keep it in 64-bit. */
    const uint64_t measured_hz = ((uint64_t)cycles * 1000000ULL) / us;

    printf("  measured (mcycle/mtime): %lu Hz (%lu.%02lu MHz)  over %lu us\r\n",
           (unsigned long)measured_hz,
           (unsigned long)(measured_hz / 1000000u),
           (unsigned long)((measured_hz % 1000000u) / 10000u),
           (unsigned long)us);

    /* FreeRTOS tick sanity: how many OS ticks elapsed in that same window? */
    const uint32_t os_ticks = (uint32_t)(os1 - os0);
    const uint32_t os_expected = us / 1000u;   /* at a true 1 kHz */
    printf("  FreeRTOS ticks         : %lu in %lu ms (expected %lu at 1 kHz)\r\n",
           (unsigned long)os_ticks, (unsigned long)(us / 1000u),
           (unsigned long)os_expected);

    if (os_expected > 0) {
        const uint32_t ratio_x100 = (os_ticks * 100u) / os_expected;
        if (ratio_x100 < 90u || ratio_x100 > 110u) {
            printf("  WARNING: OS tick rate is %lu.%02lux nominal - every\r\n"
                   "           vTaskDelay(pdMS_TO_TICKS(n)) is off by that factor\r\n",
                   (unsigned long)(ratio_x100 / 100u),
                   (unsigned long)(ratio_x100 % 100u));
        }
    }

    if (sdk_hz > 0) {
        const uint32_t d = (measured_hz > sdk_hz)
                         ? (uint32_t)(measured_hz - sdk_hz)
                         : (uint32_t)(sdk_hz - measured_hz);
        if ((uint64_t)d * 100ULL > (uint64_t)sdk_hz * 2ULL) {
            printf("  WARNING: measured differs from SystemCoreClockGet() by >2%%\r\n");
        }
    }
}
