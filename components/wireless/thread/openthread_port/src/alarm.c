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
 * @file alarm.c  
 * @brief OpenThread alarm/timer platform implementation for IPRO7
 * 
 * Implements OpenThread's alarm API using FreeRTOS software timers.
 * Provides both millisecond and microsecond (if needed) timer support.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "openthread-core-config.h"
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/time.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "ipro_log.h"
#include "openthread_port.h"  // For ot_system_event_t type
#include "ot_pm.h"
/* Hardware microsecond alarm (TIMER0 ch1): RCP software timing and the CSL
 * receiver, whose sample windows the stack times with it. */
#if defined(CONFIG_THREAD_RCP_SOFTWARE_TIMING) || defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE)
#define OT_PORT_USEC_ALARM 1
#else
#define OT_PORT_USEC_ALARM 0
#endif
#if OT_PORT_USEC_ALARM
#include <drv_clock.h>
#endif
#include "hal_mtimer.h"
#if OT_PORT_USEC_ALARM
#include "hal_timer.h"
#include "drv_timer.h"
#endif

/* FreeRTOS timer handle for millisecond alarm */
static TimerHandle_t sMilliTimer = NULL;

/* Static timer structures for timer creation */
static StaticTimer_t sMilliTimerBuffer;

/* Store OpenThread instance for alarm callbacks */
static otInstance *sInstance = NULL;

/* Alarm fire time in milliseconds */
static uint32_t sAlarmT0 = 0;
static uint32_t sAlarmDt = 0;
static bool sIsRunning = false;

/* Diagnostic counter — number of times OT's millisecond alarm has
 * fired since boot. Useful for debugging "why does OT housekeeping
 * run every N seconds even though there's no traffic" — typically
 * means OT has an internal periodic timer (MLE keep-alive, child
 * supervisor, etc.) scheduled at that cadence. */
volatile uint32_t g_ot_alarm_fire_count;

/**
 * FreeRTOS timer callback - fires when alarm expires
 */
static void alarmCallback(TimerHandle_t xTimer)
{
    (void)xTimer;

    // Verify we're in Timer Service task context (not ISR)
    if (xPortIsInsideInterrupt()) {
        return;
    }

    g_ot_alarm_fire_count++;

    // Send notification to OT task instead of calling otPlatAlarmMilliFired directly
    // This avoids stack overflow in Timer Service task when crypto operations are performed
    OT_NOTIFY(OT_SYSTEM_EVENT_ALARM_MS_EXPIRED);
}

void otPlatAlarmMilliInit(void)
{
    // Create FreeRTOS software timer using static allocation
    // Note: This works whether called before or after scheduler starts
    sMilliTimer = xTimerCreateStatic(
        "OT_Alarm",                    // Timer name
        pdMS_TO_TICKS(1000),           // Initial period (1 second, will be updated)
        pdFALSE,                       // One-shot (not auto-reload)
        NULL,                          // Timer ID (not used)
        alarmCallback,                 // Callback function
        &sMilliTimerBuffer             // Static timer buffer
    );
    
    sIsRunning = false;
}

uint32_t otPlatAlarmMilliGetNow(void)
{
    // Return current time in milliseconds
    // FreeRTOS tick count provides millisecond resolution
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

void otPlatAlarmMilliStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
    // Store instance for alarm callback
    sInstance = aInstance;

    // Check if timer was created successfully
    if (sMilliTimer == NULL)
    {
        return;
    }

    uint32_t now = otPlatAlarmMilliGetNow();
    uint32_t fireTime = aT0 + aDt;
    int32_t remaining;
    
    // Store alarm parameters
    sAlarmT0 = aT0;
    sAlarmDt = aDt;
    
    // Calculate remaining time (handle wraparound)
    remaining = (int32_t)(fireTime - now);
    
    if (remaining <= 0)
    {
        // Alarm should fire immediately
        remaining = 1;  // Fire on next tick
    }
    
    // Convert to FreeRTOS ticks
    TickType_t ticks = pdMS_TO_TICKS(remaining);
    if (ticks == 0) ticks = 1;  // Ensure at least 1 tick
    
    BaseType_t ret;
    
    // Stop existing timer if running
    if (sIsRunning)
    {
        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            ret = xTimerStopFromISR(sMilliTimer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            ret = xTimerStop(sMilliTimer, pdMS_TO_TICKS(100));
        }
    }
    
    // Update timer period and start it
    if (xPortIsInsideInterrupt()) {
        // Called from ISR context - use FromISR APIs
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerChangePeriodFromISR(sMilliTimer, ticks, &xHigherPriorityTaskWoken);
        if (ret == pdPASS) {
            ret = xTimerStartFromISR(sMilliTimer, &xHigherPriorityTaskWoken);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        // Called from task context
        ret = xTimerChangePeriod(sMilliTimer, ticks, pdMS_TO_TICKS(100));
        if (ret == pdPASS) {
            ret = xTimerStart(sMilliTimer, pdMS_TO_TICKS(100));
        }
    }
    
    if (ret != pdPASS) {
        return;
    }
    
    sIsRunning = true;
}

void otPlatAlarmMilliStop(otInstance *aInstance)
{
    (void)aInstance;
    
    // Check if timer was created successfully
    if (sMilliTimer == NULL)
    {
        return;
    }
    
    if (sIsRunning)
    {
        BaseType_t ret;
        if (xPortIsInsideInterrupt()) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            ret = xTimerStopFromISR(sMilliTimer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        } else {
            ret = xTimerStop(sMilliTimer, pdMS_TO_TICKS(100));
        }
        
        if (ret == pdPASS) {
            sIsRunning = false;
        }
    }
}

#if defined(OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE) && OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
/**
 * Microsecond timer support - optional, needed for CSL
 * Currently not implemented as CSL is disabled in config
 */

void otPlatAlarmMicroInit(void)
{
    // Not implemented - CSL disabled
}

uint32_t otPlatAlarmMicroGetNow(void)
{
    return (uint32_t)mtimer_get_time_us();
}

#if OT_PORT_USEC_ALARM
static otInstance *sMicroInstance;
static bool sMicroRunning;
static uint32_t sMicroDeadline;

/* Diagnostics for `radio_stats`: timer IRQs, timer_init failures, and how
 * late the alarm reached the task (now - deadline). */
struct ot_usalarm_dbg { uint32_t irq, init_fail, fired, starts, clk_restores; int32_t late_last, late_max; };
struct ot_usalarm_dbg g_ot_usalarm_dbg;

void ot_usalarm_dbg_dump(void)
{
    printf("[ot ] us-alarm: starts=%lu init_fail=%lu irq=%lu fired=%lu clk-restores=%lu late last/max=%ld/%ld us"
           " timer0 clk=%lu\r\n",
           (unsigned long)g_ot_usalarm_dbg.starts, (unsigned long)g_ot_usalarm_dbg.init_fail,
           (unsigned long)g_ot_usalarm_dbg.irq, (unsigned long)g_ot_usalarm_dbg.fired,
           (unsigned long)g_ot_usalarm_dbg.clk_restores, (long)g_ot_usalarm_dbg.late_last,
           (long)g_ot_usalarm_dbg.late_max,
           (unsigned long)Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER0));
}

void ot_usalarm_dbg_reset(void)
{
    g_ot_usalarm_dbg.late_last = 0;
    g_ot_usalarm_dbg.late_max = 0;
}

static void microTimerIrq(int id, int ch, uint32_t event)
{
    (void)event;
    g_ot_usalarm_dbg.irq++;
    timer_stop(id, ch);
    OT_NOTIFY_ISR(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
}
#endif

void otPlatAlarmMicroStartAt(otInstance *aInstance, uint32_t aT0, uint32_t aDt)
{
#if OT_PORT_USEC_ALARM
    timer_stop(TIMER0_INDEX, TIMER_CH1_INDEX);
    sMicroInstance = aInstance;
    sMicroDeadline = aT0 + aDt;
    if (aDt > 11000000u) {
        /* no sub-MAC wait is that long: the longest is a CSL window
         * (period up to 65535 x 160 us = 10.5 s) */
        IPRO_LOGW("OT-Alarm", "us alarm t0=%lu dt=%lu now=%lu",
                  (unsigned long)aT0, (unsigned long)aDt, (unsigned long)otPlatAlarmMicroGetNow());
    }
    sMicroRunning = true;
    int32_t remaining = (int32_t)(sMicroDeadline - otPlatAlarmMicroGetNow());
    if (remaining <= 0) {
        OT_NOTIFY(OT_SYSTEM_EVENT_ALARM_US_EXPIRED);
    } else {
        hal_timer_config_t cfg = {
            .cnt_mode = TIMER_CNT_PRELOAD,
            .trigger = TIMER_PRELOAD_TRIGGER_COMP0,
            .reload = 0,
            /* Bound hardware multiplication; long waits are rearmed. */
            .timeout1 = (remaining > 1000000) ? 1000000 : (uint32_t)remaining,
        };
        if (cfg.timeout1 < 2) cfg.timeout1 = 2;
        g_ot_usalarm_dbg.starts++;
        if (timer_init(TIMER0_INDEX, TIMER_CH1_INDEX, &cfg) != 0) {
            g_ot_usalarm_dbg.init_fail++;
        }
        timer_setCallback(TIMER0_INDEX, TIMER_CH1_INDEX, microTimerIrq);
        timer_start(TIMER0_INDEX, TIMER_CH1_INDEX);
    }
#else
    (void)aInstance;
    (void)aT0;
    (void)aDt;
#endif
}

void otPlatAlarmMicroStop(otInstance *aInstance)
{
    (void)aInstance;
#if OT_PORT_USEC_ALARM
    timer_stop(TIMER0_INDEX, TIMER_CH1_INDEX);
    sMicroRunning = false;
#endif
}

#endif // OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE

/* ---------------------------------------------------------------------------
 * Platform PDS (ipro_pm).  The microsecond alarm (a CSL sample window) runs on
 * TIMER0, which neither the RTOS nor the PDS wake logic sees: its deadline is
 * handed to the power manager, the timer is stopped across the sleep and
 * re-armed from the absolute deadline on the (rebased) microsecond clock.
 * A PDS with the SoC reset also resets the timer block's clock select and
 * divider (TIMER0 then counts at 32 MHz, not 1 MHz): they are kept and
 * written back.  IRQs masked in all three.
 * -------------------------------------------------------------------------*/
#if OT_PORT_USEC_ALARM && defined(OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE) && \
    OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
static uint32_t s_tmr_tccr;
static uint32_t s_tmr_tcdr;
#else
/* no microsecond alarm in this build: nothing to report (ot_pm shell) */
void ot_usalarm_dbg_dump(void)
{
}

void ot_usalarm_dbg_reset(void)
{
}
#endif
bool ot_alarm_micro_deadline(uint32_t *deadline)
{
#if OT_PORT_USEC_ALARM && defined(OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE) && \
    OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if (sMicroRunning) {
        *deadline = sMicroDeadline;
        return true;
    }
#else
    (void)deadline;
#endif
    return false;
}

void ot_alarm_micro_suspend(void)
{
#if OT_PORT_USEC_ALARM && defined(OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE) && \
    OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    s_tmr_tccr = readl(TIMER_BASE + TIMER_TCCR_OFFSET);
    s_tmr_tcdr = readl(TIMER_BASE + TIMER_TCDR_OFFSET);
    if (sMicroRunning) {
        timer_stop(TIMER0_INDEX, TIMER_CH1_INDEX);
    }
#endif
}

void ot_alarm_micro_resume(void)
{
#if OT_PORT_USEC_ALARM && defined(OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE) && \
    OPENTHREAD_CONFIG_PLATFORM_USEC_TIMER_ENABLE
    if ((readl(TIMER_BASE + TIMER_TCCR_OFFSET) != s_tmr_tccr) ||
        (readl(TIMER_BASE + TIMER_TCDR_OFFSET) != s_tmr_tcdr)) {
        writel(s_tmr_tccr, TIMER_BASE + TIMER_TCCR_OFFSET);
        writel(s_tmr_tcdr, TIMER_BASE + TIMER_TCDR_OFFSET);
        g_ot_usalarm_dbg.clk_restores++;
    }
    if (sMicroRunning) {
        int32_t remaining = (int32_t)(sMicroDeadline - otPlatAlarmMicroGetNow());
        hal_timer_config_t cfg = {
            .cnt_mode = TIMER_CNT_PRELOAD,
            .trigger = TIMER_PRELOAD_TRIGGER_COMP0,
            .reload = 0,
            /* already due: fire right after interrupts come back (no task
             * API here, the scheduler is suspended) */
            .timeout1 = (remaining > 1000000) ? 1000000u : ((remaining < 2) ? 2u : (uint32_t)remaining),
        };

        g_ot_usalarm_dbg.starts++;
        if (timer_init(TIMER0_INDEX, TIMER_CH1_INDEX, &cfg) != 0) {
            g_ot_usalarm_dbg.init_fail++;
        }
        timer_setCallback(TIMER0_INDEX, TIMER_CH1_INDEX, microTimerIrq);
        timer_start(TIMER0_INDEX, TIMER_CH1_INDEX);
    }
#endif
}

/**
 * @brief Get current platform time in microseconds
 * 
 * This provides a continuous monotonic clock for OpenThread timing.
 * Uses FreeRTOS tick count as the time base.
 * 
 * @return Current time in microseconds
 */
uint64_t otPlatTimeGet(void)
{
    // Convert FreeRTOS ticks to microseconds
    // Use 64-bit to avoid overflow
    return mtimer_get_time_us();
}

/**
 * @brief Get crystal oscillator accuracy
 * 
 * @return Accuracy in PPM (parts per million)
 */
uint16_t otPlatTimeGetXtalAccuracy(void)
{
    // Return a conservative estimate
    // Typical crystal accuracy is ±20-50 PPM
    return 50; // 50 PPM = ±0.005% accuracy
}

/**
 * Task processing and initialization stubs for FreeRTOS event loop
 * These are called from ot_freertos.c
 */

// Alarm task processing - called from OT task when alarm event is received
void ot_alarmTask(ot_system_event_t sevent)
{
#if OT_PORT_USEC_ALARM
    if ((sevent & OT_SYSTEM_EVENT_ALARM_US_EXPIRED) && sMicroRunning) {
        int32_t late = (int32_t)(otPlatAlarmMicroGetNow() - sMicroDeadline);
        if (late >= 0) {
            g_ot_usalarm_dbg.fired++;
            g_ot_usalarm_dbg.late_last = late;
            if (late > g_ot_usalarm_dbg.late_max) g_ot_usalarm_dbg.late_max = late;
            sMicroRunning = false;
            otPlatAlarmMicroFired(sMicroInstance);
        } else {
            otPlatAlarmMicroStartAt(sMicroInstance, sMicroDeadline, 0);
        }
    }
#endif
    // Check if alarm event bit is set
    if (sevent & OT_SYSTEM_EVENT_ALARM_MS_EXPIRED) {
        // Now in OT task context with adequate stack - safe to process
        if (sInstance != NULL) {
            otPlatAlarmMilliFired(sInstance);
        } 
    }
}

// Alarm initialization - calls the actual platform init
void ot_alarmInit(void)
{
    // Initialize the alarm platform layer
    otPlatAlarmMilliInit();
}

#ifndef CONFIG_OPENTHREAD_RADIO
// CLI builds keep using the console directly; only RCP has a UART task.
void ot_uartTask(ot_system_event_t sevent)
{
    (void)sevent;
}
#endif
