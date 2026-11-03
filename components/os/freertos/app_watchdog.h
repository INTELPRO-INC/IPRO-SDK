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

#ifndef APP_WATCHDOG_H
#define APP_WATCHDOG_H

#include <stdint.h>

/*
 * The AON watchdog for an application: restart the SoC when it stops.
 *
 * app_watchdog_start() creates a feeder task at configMAX_PRIORITIES - 1 that
 * arms the always-on watchdog when it first runs and feeds it every second of
 * hardware-timer time (it wakes every 10 ticks: ticks are lost while
 * interrupts are off, so tick-counted waits stretch during flash erases).
 * It fires when the CPU as a whole stops - a bus stall (an NPU running a
 * malformed program stops the SoC; docs/release/03_runtime_reliability.md),
 * interrupts left off, an interrupt storm - and not because a busy task
 * starved a lower one.
 *
 * Timeout: CONFIG_APP_WATCHDOG_TIMEOUT_S rounded up to 2, 4, 8, 16, 32 or
 * 64 s of the 32 kHz RC clock, which runs fast: 8 s fired after 5.6 s on the
 * Reef. It must cover the longest stretch with interrupts off; flash_erase()
 * holds them off for the whole range it erases.
 *
 * With CONFIG_APP_WATCHDOG, platform_init() arms the watchdog with the same
 * timeout early in every boot, so a boot that hangs before the feeder runs
 * restarts too; an application that sets CONFIG_APP_WATCHDOG must therefore
 * call app_watchdog_start(). hal_reboot_download() disables the watchdog so
 * the boot ROM's download mode is never cut short.
 *
 * Without CONFIG_APP_WATCHDOG every call is a no-op.
 */

typedef struct {
    uint8_t  armed;          /* 1 while the watchdog runs */
    uint8_t  feeding;        /* 0 after app_watchdog_test() */
    uint32_t timeout_ms;
    uint32_t feed_ms;
    uint32_t feeds;
    uint32_t hw_ms;          /* hardware-timer ms since armed */
    uint32_t ticks;          /* RTOS ticks since armed: fewer than hw_ms after interrupts were off */
} app_watchdog_info_t;

/* Create the feeder task (once). 0, or -1 if it could not be created: the
 * watchdog is then not armed. Call before or after the scheduler starts. */
int app_watchdog_start(void);

/* Disable the watchdog and stop feeding. */
void app_watchdog_stop(void);

/* Stop feeding, leave the watchdog armed: the SoC restarts within the timeout. */
void app_watchdog_test(void);

void app_watchdog_info(app_watchdog_info_t *out);

#endif /* APP_WATCHDOG_H */
