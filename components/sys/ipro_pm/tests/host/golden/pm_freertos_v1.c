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
 * @file pm_freertos.c
 * @brief ipro_pm FreeRTOS port: the only owner of the tickless-idle hooks.
 *
 * Every idle window is one of:
 *   - short (< 2 ticks): WFI from the idle hook, nothing else;
 *   - tickless: the port sleeps on WFI for the selected ticks, clamped to the
 *     radio deadline, or - when the allowed depth reaches PDS and the radio
 *     backend accepts the window - returns without WFI and lets the radio
 *     driver run the PDS transaction from its own task.
 *
 * A window can also be "projected": the BLE controller polls every tick while
 * a connection is up, so the kernel alone never sees two idle ticks.  The
 * backend then reports the idle time excluding that poll.  A projected window
 * is only ever offered for PDS, never slept on with WFI: vTaskStepTick() must
 * not step past the controller's own unblock time.
 */
#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "core_feature_base.h"
#include "core_feature_timer.h"
#include "ipro_pm.h"
#include "pm_internal.h"

#if (configUSE_TICKLESS_IDLE != 1)
#error "ipro_pm needs CONFIG_FREERTOS_USE_TICKLESS_IDLE=1 (port tickless mode 1)"
#endif
#if (configUSE_IDLE_HOOK != 1)
#error "ipro_pm needs CONFIG_FREERTOS_IDLE_HOOK=y"
#endif

/* half-microseconds per RTOS tick */
#define PM_HUS_PER_TICK (2000UL * (uint32_t)portTICK_PERIOD_MS)
/* WFI clamp guard before a radio deadline: controller ISR entry */
#define PM_WFI_DEADLINE_GUARD_HUS 4000UL
/* SysTimer counts per microsecond */
#define PM_SYSTIMER_PER_US (configSYSTICK_CLOCK_HZ / 1000000UL)

static volatile BaseType_t s_projected;
static volatile BaseType_t s_offered;
static volatile BaseType_t s_retry;
/* a transient reject is pending: the next idle hook pass waits for one
 * interrupt (tick running) before the window is offered again */
static volatile BaseType_t s_retry_wfi;
static uint64_t s_wfi_start;

volatile uint32_t ipro_pm_port_projected;
volatile uint32_t ipro_pm_port_offered;
volatile uint32_t ipro_pm_port_offer_rejected;
volatile uint32_t ipro_pm_port_short_wfi;
volatile uint32_t ipro_pm_port_tickless_wfi;
volatile uint32_t ipro_pm_port_aborts;
volatile uint32_t ipro_pm_port_retry_wfi;

void pm_port_init(void)
{
    s_projected = pdFALSE;
    s_offered = pdFALSE;
}

static uint64_t pm_systimer_us(uint64_t counts)
{
    return counts / PM_SYSTIMER_PER_US;
}

TickType_t xPortApplicationProjectIdleTime(TickType_t kernel_expected_ticks)
{
    TickType_t projected;

    /* called twice per idle pass (preliminary, then with the scheduler
     * suspended): only the latest call decides what the port sleeps on */
    s_projected = pdFALSE;
    if (ipro_pm_allowed_mode() < IPRO_PM_MODE_PDS) {
        return kernel_expected_ticks;
    }
    projected = pm_backend_projected_idle_ticks();
    if ((projected < 2U) || (projected <= kernel_expected_ticks)) {
        return kernel_expected_ticks;
    }
    s_projected = pdTRUE;
    ipro_pm_port_projected++;
    return projected;
}

/* Short idle: wait for the next interrupt instead of spinning.  Longer idle
 * is left to tickless, and so is a window the projection is about to offer. */
void vApplicationIdleHook(void)
{
    uint64_t start;

    /* apps that do not use ipro_platform_init() get initialised here, the
     * first time the scheduler goes idle (idempotent) */
    (void)ipro_pm_init();
    if (ipro_pm_allowed_mode() == IPRO_PM_MODE_ACTIVE) {
        return;
    }
    if (s_retry_wfi != pdFALSE) {
        /* a projected window was refused for a transient reason: sleep until
         * the next interrupt with the tick running (the controller's 1 ms
         * poll at the latest), then the next pass offers it again */
        s_retry_wfi = pdFALSE;
    } else if ((xTaskGetExpectedIdleTimeExcludingTask(NULL) >= 2U) ||
               ((ipro_pm_allowed_mode() >= IPRO_PM_MODE_PDS) &&
                (pm_backend_projected_idle_ticks() >= 2U))) {
        return;
    }
    start = SysTimer_GetLoadValue();
    __WFI();
    ipro_pm_port_short_wfi++;
    pm_core_account(IPRO_PM_MODE_WFI, pm_systimer_us(SysTimer_GetLoadValue() - start));
}

TickType_t xPortApplicationClampIdleTime(TickType_t expected_ticks)
{
    uint32_t deadline_hus;

    if (ipro_pm_allowed_mode() == IPRO_PM_MODE_ACTIVE) {
        return 0U;
    }
    deadline_hus = pm_backend_wfi_deadline_hus();
    if (deadline_hus != UINT32_MAX) {
        TickType_t radio_ticks = 0U;

        if (deadline_hus > PM_WFI_DEADLINE_GUARD_HUS) {
            radio_ticks = (TickType_t)((deadline_hus - PM_WFI_DEADLINE_GUARD_HUS) / PM_HUS_PER_TICK);
        }
        if (radio_ticks < expected_ticks) {
            expected_ticks = radio_ticks;
        }
    }
    return expected_ticks;
}

/* Interrupts are masked and the scheduler has confirmed sleep. */
void vPortApplicationPreSleepRevalidate(TickType_t selected_ticks)
{
    s_offered = pdFALSE;
    s_retry = pdFALSE;
    if ((ipro_pm_allowed_mode() >= IPRO_PM_MODE_PDS) && pm_backend_pds_capable()) {
        uint64_t rtos_hus64 = (uint64_t)selected_ticks * PM_HUS_PER_TICK;
        uint32_t rtos_hus = (rtos_hus64 > UINT32_MAX) ? UINT32_MAX : (uint32_t)rtos_hus64;
        pm_offer_t offer = pm_backend_offer_pds(rtos_hus);

        if (offer == PM_OFFER_GRANTED) {
            s_offered = pdTRUE;
            ipro_pm_port_offered++;
            return;
        }
        ipro_pm_port_offer_rejected++;
        if ((offer == PM_OFFER_RETRY) && (s_projected != pdFALSE)) {
            s_retry = pdTRUE;   /* keep the window for a later pass */
            return;
        }
    }
    if (s_projected != pdFALSE) {
        pm_backend_window_skipped();
    }
    s_wfi_start = SysTimer_GetLoadValue();
}

BaseType_t xPortApplicationPreSleepSkipWfi(void)
{
    BaseType_t skip = ((s_offered != pdFALSE) || (s_projected != pdFALSE)) ? pdTRUE : pdFALSE;

    if (s_retry != pdFALSE) {
        /* transient reject of a projected window: no WFI here (the port has
         * already moved the tick compare to the projected deadline); the
         * idle hook's next pass does a short WFI with the tick running */
        s_retry = pdFALSE;
        s_retry_wfi = pdTRUE;
        ipro_pm_port_retry_wfi++;
    }

    /* offered: the radio driver runs the PDS; projected but not offered: the
     * budget is not the kernel's to sleep on */
    s_projected = pdFALSE;
    s_offered = pdFALSE;
    return skip;
}

void vPortApplicationPostSleepMasked(void)
{
    ipro_pm_port_tickless_wfi++;
    pm_core_account(IPRO_PM_MODE_WFI, pm_systimer_us(SysTimer_GetLoadValue() - s_wfi_start));
}

void vPortApplicationTicklessObservation(TickType_t selected_ticks, TickType_t completed_ticks,
                                         BaseType_t wake_by_timer, BaseType_t aborted)
{
    (void)selected_ticks;
    (void)completed_ticks;
    (void)wake_by_timer;
    if (aborted != pdFALSE) {
        ipro_pm_port_aborts++;
    }
}
