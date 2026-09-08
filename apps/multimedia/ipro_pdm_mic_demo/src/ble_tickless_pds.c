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
 * PDM demo BLE + FreeRTOS tickless-PDS bridge.
 *
 * Policy stays in the application: FreeRTOS owns the next task deadline, the
 * controller publishes BLE radio-safe sleep windows, and this hook grants one
 * controller-owned platform-PDS transaction only when both are still valid.
 */

#include <stdint.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>
#include <generated/autoconf.h>

#include "drv_hbn.h"
#include "ipro_ble_ctlr_pds_snapshot.h"
#include "ipro_ble_controller.h"

#ifndef CONFIG_IPRO_BLE_PDS_WAKE_BUDGET_US
#define CONFIG_IPRO_BLE_PDS_WAKE_BUDGET_US 20000U
#endif

#ifndef CONFIG_IPRO_BLE_PDS_DEADLINE_GUARD_US
#define CONFIG_IPRO_BLE_PDS_DEADLINE_GUARD_US 2000U
#endif

#define IPRO_BLE_CTLR_PDS_MIN_SLEEP_US 10000U

volatile uint32_t ipro_ble_ctlr_tickless_calls;
volatile uint32_t ipro_ble_ctlr_tickless_rtos_ticks;
volatile uint32_t ipro_ble_ctlr_tickless_ble_hus = UINT32_MAX;
volatile uint32_t ipro_ble_ctlr_tickless_selected_ticks;
volatile uint32_t ipro_ble_ctlr_tickless_completed_ticks;
volatile uint32_t ipro_ble_ctlr_tickless_completed_total;
volatile uint32_t ipro_ble_ctlr_tickless_timer_wakes;
volatile uint32_t ipro_ble_ctlr_tickless_external_wakes;
volatile uint32_t ipro_ble_ctlr_tickless_aborts;
volatile uint32_t ipro_ble_ctlr_tickless_clamps;
volatile uint32_t ipro_ble_ctlr_tickless_pds_grant;
volatile uint32_t ipro_ble_ctlr_tickless_pds_grant_reject;
volatile uint32_t ipro_ble_ctlr_tickless_pds_grant_sleep_us;
volatile uint32_t ipro_ble_ctlr_tickless_pds_skip_wfi;
volatile uint32_t ipro_ble_ctlr_tickless_window_grant;
volatile uint32_t ipro_ble_ctlr_tickless_window_reject_no_budget;
volatile uint32_t ipro_ble_ctlr_tickless_window_reject_expired;
volatile uint32_t ipro_ble_ctlr_tickless_window_reject_repeat;
volatile uint32_t ipro_ble_ctlr_tickless_window_reject_timing;
volatile uint32_t ipro_ble_ctlr_tickless_window_reject_policy;
volatile uint32_t ipro_ble_ctlr_tickless_deferred_grant;
volatile uint32_t ipro_ble_ctlr_tickless_deferred_reject_policy;
volatile uint32_t ipro_ble_ctlr_tickless_deferred_reject_timing;
volatile uint32_t ipro_ble_ctlr_tickless_correct_expected;
volatile uint32_t ipro_ble_ctlr_tickless_correct_port;
volatile uint32_t ipro_ble_ctlr_tickless_correct_rtc;
volatile uint32_t ipro_ble_ctlr_native_tick_corrections;
volatile uint32_t ipro_ble_ctlr_native_tick_corrected_total;
volatile uint32_t ipro_ble_ctlr_native_tick_corrected_last;
volatile uint32_t ipro_ble_ctlr_app_diag_pds_sleep_us;

static volatile BaseType_t s_skip_wfi_request;
static volatile uint32_t s_grant_outstanding;
static volatile uint32_t s_rtos_budget_valid;
static volatile TickType_t s_rtos_budget_expire_tick;
static volatile uint32_t s_last_committed_generation;
static uint64_t s_native_rtc_epoch;
static TickType_t s_native_tick_epoch;
static uint8_t s_native_epoch_valid;
static volatile uint64_t s_tickless_rtc_start;
static volatile uint64_t s_tickless_rtc_elapsed;

__attribute__((weak)) bool ipro_ble_ctlr_app_pds_tickless_allowed(void)
{
    return true;
}

static uint32_t pds_reserve_hus(void)
{
    return (CONFIG_IPRO_BLE_PDS_WAKE_BUDGET_US +
            CONFIG_IPRO_BLE_PDS_DEADLINE_GUARD_US) * 2U;
}

static uint32_t ticks_to_hus(TickType_t ticks)
{
    uint64_t hus = (uint64_t)ticks * 2000ULL *
                   (uint64_t)portTICK_PERIOD_MS;

    return (hus > UINT32_MAX) ? UINT32_MAX : (uint32_t)hus;
}

static uint32_t cap_sleep_us(uint32_t sleep_us)
{
    uint32_t app_max = ipro_ble_ctlr_app_diag_pds_sleep_us;

    if ((app_max != 0U) && (sleep_us > app_max)) {
        sleep_us = app_max;
    }
    return sleep_us;
}

void ipro_ble_ctlr_tickless_stats_reset(void)
{
    ipro_ble_ctlr_tickless_calls = 0U;
    ipro_ble_ctlr_tickless_rtos_ticks = 0U;
    ipro_ble_ctlr_tickless_ble_hus = UINT32_MAX;
    ipro_ble_ctlr_tickless_selected_ticks = 0U;
    ipro_ble_ctlr_tickless_completed_ticks = 0U;
    ipro_ble_ctlr_tickless_completed_total = 0U;
    ipro_ble_ctlr_tickless_timer_wakes = 0U;
    ipro_ble_ctlr_tickless_external_wakes = 0U;
    ipro_ble_ctlr_tickless_aborts = 0U;
    ipro_ble_ctlr_tickless_clamps = 0U;
    ipro_ble_ctlr_tickless_pds_grant = 0U;
    ipro_ble_ctlr_tickless_pds_grant_reject = 0U;
    ipro_ble_ctlr_tickless_pds_grant_sleep_us = 0U;
    ipro_ble_ctlr_tickless_pds_skip_wfi = 0U;
    ipro_ble_ctlr_tickless_window_grant = 0U;
    ipro_ble_ctlr_tickless_window_reject_no_budget = 0U;
    ipro_ble_ctlr_tickless_window_reject_expired = 0U;
    ipro_ble_ctlr_tickless_window_reject_repeat = 0U;
    ipro_ble_ctlr_tickless_window_reject_timing = 0U;
    ipro_ble_ctlr_tickless_window_reject_policy = 0U;
    ipro_ble_ctlr_tickless_deferred_grant = 0U;
    ipro_ble_ctlr_tickless_deferred_reject_policy = 0U;
    ipro_ble_ctlr_tickless_deferred_reject_timing = 0U;
    ipro_ble_ctlr_tickless_correct_expected = 0U;
    ipro_ble_ctlr_tickless_correct_port = 0U;
    ipro_ble_ctlr_tickless_correct_rtc = 0U;
    s_skip_wfi_request = pdFALSE;
    s_grant_outstanding = 0U;
    s_rtos_budget_valid = 0U;
    s_rtos_budget_expire_tick = 0U;
    s_last_committed_generation = 0U;
}

static bool grant_pds_from_windows(uint32_t ble_hus, uint32_t rtos_hus,
                                   uint32_t generation)
{
    uint32_t reserve = pds_reserve_hus();
    uint32_t joint;
    uint32_t sleep_us;

    if ((rtos_hus <= reserve) || (ble_hus <= reserve)) {
        ipro_ble_ctlr_tickless_window_reject_timing++;
        return false;
    }
    if ((generation == 0U) || (generation == s_last_committed_generation)) {
        ipro_ble_ctlr_tickless_window_reject_repeat++;
        return false;
    }
    if (s_grant_outstanding != 0U) {
        ipro_ble_ctlr_tickless_window_reject_repeat++;
        return false;
    }
    if (!ipro_ble_ctlr_app_pds_tickless_allowed()) {
        ipro_ble_ctlr_tickless_window_reject_policy++;
        return false;
    }

    joint = (rtos_hus < ble_hus) ? rtos_hus : ble_hus;
    sleep_us = (joint / 2U) -
               CONFIG_IPRO_BLE_PDS_WAKE_BUDGET_US -
               CONFIG_IPRO_BLE_PDS_DEADLINE_GUARD_US;
    sleep_us = cap_sleep_us(sleep_us);
    ipro_ble_ctlr_tickless_pds_grant_sleep_us = sleep_us;

    if ((sleep_us < IPRO_BLE_CTLR_PDS_MIN_SLEEP_US) ||
        (ipro_ble_ctlr_pds_tickless_grant(sleep_us) != 0)) {
        ipro_ble_ctlr_tickless_pds_grant_reject++;
        return false;
    }

    s_last_committed_generation = generation;
    s_grant_outstanding = 1U;
    ipro_ble_ctlr_tickless_pds_grant++;
    ipro_ble_ctlr_tickless_window_grant++;
    return true;
}

void ipro_ble_ctlr_app_pds_cycle_consumed(void)
{
    s_grant_outstanding = 0U;
}

static void native_time_sync(void)
{
    uint32_t lo, hi;
    uint64_t now, elapsed_us;
    TickType_t wall_ticks, target_tick, current_tick, missing;

    HBN_Enable_RTC_Counter();
    HBN_Get_RTC_Timer_Val(&lo, &hi);
    now = ((uint64_t)hi << 32) | lo;
    current_tick = xTaskGetTickCount();
    if (s_native_epoch_valid == 0U) {
        s_native_rtc_epoch = now;
        s_native_tick_epoch = current_tick;
        s_native_epoch_valid = 1U;
        return;
    }

    elapsed_us = ((now - s_native_rtc_epoch) * 15625ULL) / 512ULL;
    wall_ticks = (TickType_t)
        (elapsed_us / (1000ULL * (uint64_t)portTICK_PERIOD_MS));
    target_tick = s_native_tick_epoch + wall_ticks;
    missing = (target_tick > current_tick) ? (target_tick - current_tick) : 0U;
    if (missing != 0U) {
        (void)xTaskCatchUpTicks(missing);
        ipro_ble_ctlr_native_tick_corrections++;
        ipro_ble_ctlr_native_tick_corrected_total += (uint32_t)missing;
    }
    ipro_ble_ctlr_native_tick_corrected_last = (uint32_t)missing;
}

void ipro_ble_ctlr_native_wait_begin(void)
{
    native_time_sync();
}

void ipro_ble_ctlr_native_wait_end(void)
{
    native_time_sync();
}

TickType_t xPortApplicationClampIdleTime(TickType_t expected_ticks)
{
    uint32_t ble_hus = ipro_ble_ctlr_tickless_slack_hus;
    uint32_t pds_hus = ipro_ble_ctlr_pds_candidate_slack_hus;
    TickType_t selected = expected_ticks;

    ipro_ble_ctlr_tickless_calls++;
    ipro_ble_ctlr_tickless_rtos_ticks = (uint32_t)expected_ticks;
    ipro_ble_ctlr_tickless_ble_hus = ble_hus;

    /* The RTOS budget belongs to this idle attempt only.  If the next
     * scheduler decision is too short, any previous long-idle grant must not
     * remain available for a later controller BLE-window callback. */
    s_rtos_budget_valid = 0U;
    s_rtos_budget_expire_tick = 0U;

    if (ticks_to_hus(expected_ticks) > pds_reserve_hus()) {
        s_rtos_budget_expire_tick = xTaskGetTickCount() + expected_ticks;
        s_rtos_budget_valid = 1U;
    }

    if (ipro_ble_ctlr_tickless_connection_active_get() &&
        (ble_hus == UINT32_MAX) && (pds_hus != 0U) && (pds_hus != UINT32_MAX)) {
        ble_hus = pds_hus;
    }

    if (ble_hus != UINT32_MAX) {
        const uint32_t guard_hus = 4000U;
        const uint32_t hus_per_tick = 2000U * (uint32_t)portTICK_PERIOD_MS;
        TickType_t ble_ticks = 0U;

        if (ble_hus > guard_hus) {
            ble_ticks = (TickType_t)((ble_hus - guard_hus) / hus_per_tick);
        }
        if (ble_ticks < selected) {
            selected = ble_ticks;
            ipro_ble_ctlr_tickless_clamps++;
        }
    }

    ipro_ble_ctlr_tickless_selected_ticks = (uint32_t)selected;
    return selected;
}

bool ipro_ble_ctlr_tickless_full_window_grant(uint32_t fresh_slack_hus)
{
    TickType_t now_tick;
    TickType_t rtos_ticks;

    if (s_rtos_budget_valid == 0U) {
        ipro_ble_ctlr_tickless_window_reject_no_budget++;
        return false;
    }

    now_tick = xTaskGetTickCount();
    rtos_ticks = s_rtos_budget_expire_tick - now_tick;
    if (rtos_ticks < 2U) {
        s_rtos_budget_valid = 0U;
        ipro_ble_ctlr_tickless_window_reject_expired++;
        return false;
    }

    return grant_pds_from_windows(fresh_slack_hus, ticks_to_hus(rtos_ticks),
                                  ipro_ble_ctlr_pds_full_generation);
}

void vPortApplicationPreSleepRevalidate(TickType_t selected_ticks)
{
    uint32_t token = ipro_ble_ctlr_pds_candidate_token;
    uint32_t generation = ipro_ble_ctlr_pds_full_generation;
    uint32_t fresh_ble_hus = 0U;
    uint32_t rtc_lo, rtc_hi;

    HBN_Enable_RTC_Counter();
    HBN_Get_RTC_Timer_Val(&rtc_lo, &rtc_hi);
    s_tickless_rtc_start = ((uint64_t)rtc_hi << 32) | rtc_lo;

    if ((token == 0U) ||
        !ipro_ble_ctlr_pds_candidate_revalidate(token, &fresh_ble_hus)) {
        return;
    }

    if (grant_pds_from_windows(fresh_ble_hus, ticks_to_hus(selected_ticks),
                               generation)) {
        ipro_ble_ctlr_tickless_pds_skip_wfi++;
        s_skip_wfi_request = pdTRUE;
    }
}

BaseType_t xPortApplicationPreSleepSkipWfi(void)
{
    if (s_skip_wfi_request == pdFALSE) {
        return pdFALSE;
    }
    s_skip_wfi_request = pdFALSE;
    return pdTRUE;
}

void vPortApplicationPostSleepMasked(void)
{
    uint32_t lo, hi;
    uint64_t now;

    HBN_Get_RTC_Timer_Val(&lo, &hi);
    now = ((uint64_t)hi << 32) | lo;
    s_tickless_rtc_elapsed = now - s_tickless_rtc_start;
}

TickType_t xPortApplicationCorrectElapsedTicks(TickType_t expected_ticks,
                                               TickType_t port_ticks)
{
    uint64_t elapsed_us = (s_tickless_rtc_elapsed * 15625ULL) / 512ULL;
    TickType_t rtc_ticks = (TickType_t)
        (elapsed_us / (1000ULL * (uint64_t)portTICK_PERIOD_MS));

    ipro_ble_ctlr_tickless_correct_expected = (uint32_t)expected_ticks;
    ipro_ble_ctlr_tickless_correct_port = (uint32_t)port_ticks;
    ipro_ble_ctlr_tickless_correct_rtc = (uint32_t)rtc_ticks;
    if (rtc_ticks >= expected_ticks) {
        rtc_ticks = expected_ticks - 1U;
    }
    return rtc_ticks;
}

void vPortApplicationTicklessObservation(TickType_t selected_ticks,
                                         TickType_t completed_ticks,
                                         BaseType_t wake_by_timer,
                                         BaseType_t aborted)
{
    (void)selected_ticks;
    ipro_ble_ctlr_tickless_completed_ticks = (uint32_t)completed_ticks;
    ipro_ble_ctlr_tickless_completed_total += (uint32_t)completed_ticks;
    if (aborted != pdFALSE) {
        ipro_ble_ctlr_tickless_aborts++;
    } else if (wake_by_timer != pdFALSE) {
        ipro_ble_ctlr_tickless_timer_wakes++;
    } else {
        ipro_ble_ctlr_tickless_external_wakes++;
    }
}
