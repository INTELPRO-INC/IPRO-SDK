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
 * The AON watchdog for an application. See app_watchdog.h.
 */
#include "app_watchdog.h"

#include <generated/autoconf.h>
#include <string.h>

#if defined(CONFIG_APP_WATCHDOG)

#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "hal_aon_wdt.h"
#include "hal_mtimer.h"

#ifndef CONFIG_APP_WATCHDOG_TIMEOUT_S
#define CONFIG_APP_WATCHDOG_TIMEOUT_S 8
#endif
#define FEED_MS      1000u
#define WAKE_TICKS   10u
#define FEEDER_WORDS 256u

static volatile uint8_t  s_armed, s_feeding;
static volatile uint32_t s_feeds;
static TaskHandle_t      s_task;
static uint64_t          s_armed_ms;      /* hardware time and tick count when armed */
static TickType_t        s_armed_tick;

static void feeder(void *arg)
{
    (void)arg;
    uint32_t seconds;
    const uint8_t mask = aon_wdt_mask_for_seconds(CONFIG_APP_WATCHDOG_TIMEOUT_S, &seconds);

    aon_wdt_init(mask);
    s_feeding = 1;
    s_armed = 1;
    printf("[WDT] AON watchdog armed: %lu s, fed every %lu ms\r\n",
           (unsigned long)seconds, (unsigned long)FEED_MS);
    /* Paced by the hardware timer, not by vTaskDelay(FEED_MS): the tick is
     * reloaded from the current time, so ticks are lost while interrupts are
     * off, and a flash erase keeps them off for each sector it erases. During
     * a 437 KB deploy on the Reef the hardware timer advanced 3,694 ms and the
     * tick count 196; a feeder waiting 1000 ticks let the watchdog fire. */
    s_armed_ms = mtimer_get_time_ms();
    s_armed_tick = xTaskGetTickCount();
    uint64_t last = s_armed_ms - FEED_MS;
    for (;;) {
        const uint64_t now = mtimer_get_time_ms();
        if (s_armed && s_feeding && now - last >= FEED_MS) {
            aon_wdt_feed();
            s_feeds++;
            last = now;
        }
        vTaskDelay(WAKE_TICKS);
    }
}

int app_watchdog_start(void)
{
    if (s_task) {
        return 0;
    }
    if (xTaskCreate(feeder, "wdt", FEEDER_WORDS, NULL, configMAX_PRIORITIES - 1, &s_task) != pdPASS) {
        s_task = NULL;
        printf("[WDT] could not create the feeder task: the watchdog is not armed\r\n");
        return -1;
    }
    return 0;
}

void app_watchdog_stop(void)
{
    s_feeding = 0;
    s_armed = 0;
    aon_wdt_disable();
}

void app_watchdog_test(void)
{
    s_feeding = 0;
}

void app_watchdog_info(app_watchdog_info_t *out)
{
    uint32_t seconds;
    (void)aon_wdt_mask_for_seconds(CONFIG_APP_WATCHDOG_TIMEOUT_S, &seconds);
    memset(out, 0, sizeof(*out));
    out->armed = s_armed;
    out->feeding = s_feeding;
    out->timeout_ms = seconds * 1000u;
    out->feed_ms = FEED_MS;
    out->feeds = s_feeds;
    if (s_armed) {
        out->hw_ms = (uint32_t)(mtimer_get_time_ms() - s_armed_ms);
        out->ticks = (uint32_t)(xTaskGetTickCount() - s_armed_tick);
    }
}

#ifdef CONFIG_SHELL
#include "shell.h"

static int watchdog_cmd(int argc, char **argv)
{
    app_watchdog_info_t w;
    app_watchdog_info(&w);
    if (argc >= 2 && strcmp(argv[1], "test") == 0) {
        if (!w.armed) {
            printf("watchdog: not armed\r\n");
            return -1;
        }
        printf("watchdog: feeding stopped, the board restarts within %lu s\r\n",
               (unsigned long)(w.timeout_ms / 1000u));
        app_watchdog_test();
        return 0;
    }
    if (argc >= 2) {
        printf("usage: watchdog [test]\r\n");
        return -1;
    }
    printf("watchdog: %s, timeout %lu ms, fed every %lu ms, %lu feeds%s; since armed %lu ms, %lu ticks\r\n",
           w.armed ? "armed" : "off", (unsigned long)w.timeout_ms, (unsigned long)w.feed_ms,
           (unsigned long)w.feeds, w.armed && !w.feeding ? " (feeding stopped)" : "",
           (unsigned long)w.hw_ms, (unsigned long)w.ticks);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(watchdog_cmd, watchdog, AON watchdog status; `watchdog test` stops feeding);
#endif /* CONFIG_SHELL */

#else /* !CONFIG_APP_WATCHDOG */

int app_watchdog_start(void)
{
    return 0;
}

void app_watchdog_stop(void)
{
}

void app_watchdog_test(void)
{
}

void app_watchdog_info(app_watchdog_info_t *out)
{
    memset(out, 0, sizeof(*out));
}

#endif /* CONFIG_APP_WATCHDOG */
