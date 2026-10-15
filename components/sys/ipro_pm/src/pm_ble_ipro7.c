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
 * @file pm_ble_ipro7.c
 * @brief ipro_pm radio participant for the IPRO7 BLE controller.
 *
 * The controller owns its radio deadline and the whole PDS transaction
 * (snapshot, native sleep hand-off, platform PDS, restore): while a link is in
 * the PDS profile it is the executor (PM_PART_EXECUTOR); with the controller
 * up in any other state it is busy (only it can keep the BLE core across a
 * PDS).  This participant decides, per idle window, whether to hand the
 * window to the controller:
 *
 *   1. While a connection is in the tickless PDS profile the controller
 *      publishes a "full window" after each connection event: a token, a
 *      generation and the slack to the next radio target.
 *   2. The FreeRTOS port offers the window here with the RTOS idle budget.
 *      The token is revalidated against the live arbiter, the joint budget
 *      (RTOS, radio) is sized with the controller's wake reserve, the
 *      application's holds/vetoes are consulted, and one grant is issued.
 *   3. The controller consumes the grant at its own quiescent boundary and
 *      reports the outcome through the weak ipro_ble_ctlr_app_* hooks below.
 *
 * Failures back off: after IPRO_PM_BLE_FAIL_LIMIT consecutive failed or
 * unrecovered PDS cycles the automatic ceiling drops to WFI for a cool-down
 * that doubles on every repeat (capped), then PDS is tried again.
 */
#include <generated/autoconf.h>
#include <errno.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

#include "drv_hbn.h"
#include "hal_lp.h"
#include "hal_pm.h"
#include "core_feature_timer.h"
#include "ipro_ble_ctlr_arch.h"
#include "ipro_ble_ctlr_hcitl.h"
#include "ipro_ble_ctlr_pds_snapshot.h"
#include "ipro_pm.h"
#include "ipro_pm_ble.h"
#include "pm_internal.h"

#ifndef CONFIG_IPRO_PM_BLE_PDS_MIN_INTERVAL_MS
#define CONFIG_IPRO_PM_BLE_PDS_MIN_INTERVAL_MS 200
#endif
#ifndef CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS
#define CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS 0
#endif
#ifndef CONFIG_IPRO_PM_BLE_IDLE_DELAY_MS
#define CONFIG_IPRO_PM_BLE_IDLE_DELAY_MS 5000
#endif
/* Another participant's deadline: the CPU is back this long before it (BLE
 * restore <= 8.7 ms, post-PDS tick proof ~1 ms, grant-to-sleep latency). */
#ifndef CONFIG_IPRO_PM_BLE_EXEC_WAKE_US
#define CONFIG_IPRO_PM_BLE_EXEC_WAKE_US 12000
#endif


/* HCI hold-off after host activity while a link is in the PDS profile: the
 * host shares this CPU, so only bytes already queued can be caught by a PDS
 * (checked separately); the default 100 ms cost every 1 s-interval window of
 * an application that notifies once per second. */
#define IPRO_PM_BLE_HCI_GUARD_MS 5U

/* Shorter PDS than this never pays for its snapshot/restore. */
#define IPRO_PM_BLE_MIN_SLEEP_US 10000U
/* Consecutive failed PDS cycles before the automatic ceiling drops to WFI. */
#define IPRO_PM_BLE_FAIL_LIMIT 2U
#define IPRO_PM_BLE_BACKOFF_MIN_MS 10000U
#define IPRO_PM_BLE_BACKOFF_MAX_MS 600000U

/* controller interface (plf/ipro7) not in its public headers */
extern volatile uint32_t ipro_ble_ctlr_tickless_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_candidate_slack_hus;
extern volatile uint32_t ipro_ble_ctlr_pds_candidate_token;
extern volatile uint32_t ipro_ble_ctlr_pds_full_generation;
extern bool ipro_ble_ctlr_pds_candidate_revalidate(uint32_t token, uint32_t *fresh_slack_hus);
extern TaskHandle_t ipro_ble_ctlr_controller_task_handle(void);
extern int ipro_ble_ctlr_pds_tickless_grant(uint32_t sleep_us);

static volatile uint32_t s_last_committed_generation;
static volatile uint32_t s_last_rejected_generation;
static volatile uint32_t s_consecutive_failures;
static volatile uint32_t s_backoff_ms;
static TimerHandle_t s_backoff_timer;

volatile uint32_t ipro_pm_ble_grants;
volatile uint32_t ipro_pm_ble_reject_stale;
volatile uint32_t ipro_pm_ble_reject_budget;
volatile uint32_t ipro_pm_ble_reject_veto;
volatile uint32_t ipro_pm_ble_reject_controller;
volatile uint32_t ipro_pm_ble_cycles;
volatile uint32_t ipro_pm_ble_failures;
volatile uint32_t ipro_pm_ble_last_sleep_us;
volatile uint32_t ipro_pm_ble_recovered;
volatile uint32_t ipro_pm_ble_reject_hci;
volatile uint32_t ipro_pm_ble_reject_prepare;
volatile uint32_t ipro_pm_ble_backoffs;


static pm_part_state_t pm_ble_pds_state(void)
{
    if (ipro_ble_ctlr_tickless_connection_active_get()) {
        return PM_PART_EXECUTOR;
    }
    /* up but not in the PDS profile (advertising, short interval, idle):
     * nobody else can keep the BLE core across a PDS */
    return (ipro_ble_ctlr_controller_task_handle() != NULL) ? PM_PART_BUSY : PM_PART_ABSENT;
}

/* Idle time the controller's own safety poll hides: projected only for a fresh
 * full-window generation not yet granted or rejected. */
static TickType_t pm_ble_projected_idle_ticks(void)
{
    uint32_t generation = ipro_ble_ctlr_pds_full_generation;
    uint32_t slack_hus = ipro_ble_ctlr_pds_candidate_slack_hus;
    TaskHandle_t ctlr_task;

    if (!ipro_ble_ctlr_tickless_connection_active_get() || (ipro_ble_ctlr_pds_candidate_token == 0U) ||
        (slack_hus == 0U) || (slack_hus == UINT32_MAX) || (generation == 0U) ||
        (generation == s_last_committed_generation) || (generation == s_last_rejected_generation)) {
        return 0U;
    }
    ctlr_task = ipro_ble_ctlr_controller_task_handle();
    if (ctlr_task == NULL) {
        return 0U;
    }
    return xTaskGetExpectedIdleTimeExcludingTask(ctlr_task);
}

static uint32_t pm_ble_wfi_deadline_hus(void)
{
    uint32_t hus = ipro_ble_ctlr_tickless_slack_hus;
    uint32_t pds_hus = ipro_ble_ctlr_pds_candidate_slack_hus;

    /* A live BLE IP wakes the CPU from WFI by itself; only a connection in
     * the PDS profile publishes a finite deadline worth clamping to. */
    if (ipro_ble_ctlr_tickless_connection_active_get() && (hus == UINT32_MAX) && (pds_hus != 0U) &&
        (pds_hus != UINT32_MAX)) {
        hus = pds_hus;
    }
    return hus;
}

static void pm_ble_window_skipped(void)
{
    /* projected but not offered: do not project this generation again */
    s_last_rejected_generation = ipro_ble_ctlr_pds_full_generation;
}

/* Timer task: the cool-down after repeated failures is over. */
static void pm_ble_backoff_end(TimerHandle_t timer)
{
    (void)timer;
    pm_core_auto_max_set(IPRO_PM_MODE_PDS);
}

/* When the controller published its latest full window (the first offer of
 * each generation): the connection event had just ended.  MTIME us, the
 * clock of OpenThread's microsecond time. */
static volatile uint32_t s_window_us;
static uint32_t s_window_gen;

uint32_t ipro_pm_ble_last_window_us(void)
{
    return s_window_us;
}

static pm_offer_t pm_ble_offer(uint32_t rtos_hus, uint32_t part_hus)
{
    uint32_t token = ipro_ble_ctlr_pds_candidate_token;
    uint32_t generation = ipro_ble_ctlr_pds_full_generation;
    uint32_t fresh_hus = 0U;
    uint32_t sleep_us;

    if ((token == 0U) || (generation == 0U) || (generation == s_last_committed_generation)) {
        ipro_pm_ble_reject_stale++;
        return PM_OFFER_REJECTED;
    }
    if (generation != s_window_gen) {
        s_window_gen = generation;
        s_window_us = (uint32_t)(SysTimer_GetLoadValue() / (configSYSTICK_CLOCK_HZ / 1000000UL));
    }
    if (!ipro_ble_ctlr_pds_candidate_revalidate(token, &fresh_hus)) {
        if (ipro_ble_ctlr_pds_reval_last_reason == IPRO_BLE_CTLR_PDS_REVAL_HCI) {
            /* host/controller bytes still in flight: the window stays valid,
             * retry on a later idle pass (paced by the controller's 1 ms poll) */
            ipro_pm_ble_reject_hci++;
            return PM_OFFER_RETRY;
        }
        ipro_pm_ble_reject_stale++;
        s_last_rejected_generation = generation;
        return PM_OFFER_REJECTED;
    }

    /* joint window minus the wake and restore budget the controller spends
     * after the sleep; another participant's deadline (a Thread CSL window, a
     * data poll) shortens it */
    sleep_us = pm_ble_size_sleep(rtos_hus, fresh_hus, part_hus, ipro_ble_ctlr_pds_reserve_us(),
                                 CONFIG_IPRO_PM_BLE_EXEC_WAKE_US);
    if (sleep_us < IPRO_PM_BLE_MIN_SLEEP_US) {
        /* (not RETRY behind another radio's imminent window: the controller
         * refuses or releases an entry granted that late after the event -
         * measured, SSED CSL window right after it: 56 grants, 10 PDS) */
        ipro_pm_ble_reject_budget++;
        pm_core_pds_rejected();
        s_last_rejected_generation = generation;
        return PM_OFFER_REJECTED;
    }
    if (!pm_core_vetoes_allow(sleep_us)) {
        ipro_pm_ble_reject_veto++;
        pm_core_pds_rejected();
        s_last_rejected_generation = generation;
        return PM_OFFER_REJECTED;
    }
    if (ipro_ble_ctlr_pds_tickless_grant(sleep_us) != 0) {
        ipro_pm_ble_reject_controller++;
        pm_core_pds_rejected();
        s_last_rejected_generation = generation;
        return PM_OFFER_REJECTED;
    }
    s_last_committed_generation = generation;
    ipro_pm_ble_grants++;
    ipro_pm_ble_last_sleep_us = sleep_us;
    return PM_OFFER_GRANTED;
}

static const pm_participant_t s_ble_participant = {
    .name = "ble",
    .projected_idle_ticks = pm_ble_projected_idle_ticks,
    .wfi_deadline_hus = pm_ble_wfi_deadline_hus,
    .pds_state = pm_ble_pds_state,
    .pds_deadline_hus = NULL,
    .offer = pm_ble_offer,
    .window_skipped = pm_ble_window_skipped,
};

void pm_ble_init(void)
{
    s_last_committed_generation = 0U;
    s_last_rejected_generation = 0U;
    s_consecutive_failures = 0U;
    s_backoff_ms = IPRO_PM_BLE_BACKOFF_MIN_MS;
    (void)pm_participant_register(&s_ble_participant);
}

/* ---------------------------------------------------------------------------
 * Shared PDS: another radio participant (Thread) sleeps in the PDS the
 * controller runs.  Its devices resume against an ipro_pm transaction, and
 * MTIME - OpenThread's clock and the RTOS tick authority of timebase builds,
 * reset by PDS31 - is carried across the sleep as the idle executor does.
 * BLE-only builds (no timebase): none of this (the controller keeps its own
 * time on the 32 kHz clock).
 * -------------------------------------------------------------------------*/
#if defined(PM_HAVE_TIMEBASE)
static pm_txn_t *s_ble_txn;
static pm_clkpair_t s_ble_before;
volatile uint32_t ipro_pm_ble_txn_rebases;
volatile uint32_t ipro_pm_ble_txn_no_pds;

/* before the devices suspend: they see this transaction, not the last one */
static void pm_ble_txn_begin(uint32_t sleep_us)
{
    s_ble_txn = pm_core_txn_begin(PM_EXEC_BLE, sleep_us);
    s_ble_txn->domain_policy = pm_pds_ble_ctlr_domain_policy_get();
    pm_lpclk_pair_now(&s_ble_before);
    pm_lpclk_span_end(&s_ble_before);
}

/* the grant ended without a PDS: the awake span goes on */
static void pm_ble_txn_abandon(void)
{
    pm_clkpair_t now;

    if (s_ble_txn == NULL) {
        return;
    }
    s_ble_txn = NULL;
    pm_lpclk_pair_now(&now);
    pm_lpclk_span_start(&now);
}

/* after hal_lp_enter(), IRQs masked: before the controller re-arms the RTOS
 * tick on MTIME.  An entry that never powered down (rc < 0) left MTIME
 * running: no rebase, the devices see slept = false. */
static void pm_ble_txn_wake(void)
{
    if (s_ble_txn == NULL) {
        return;
    }
    if (ipro_ble_ctlr_pds_last_enter_rc < 0) {
        ipro_pm_ble_txn_no_pds++;
        pm_ble_txn_abandon();
        return;
    }
    s_ble_txn->elapsed_us = pm_lpclk_rebase(&s_ble_before);
    s_ble_txn->slept = true;
    s_ble_txn = NULL;
    ipro_pm_ble_txn_rebases++;
}
#else
#define pm_ble_txn_begin(sleep_us) ((void)(sleep_us))
#define pm_ble_txn_wake()          ((void)0)
#define pm_ble_txn_abandon()       ((void)0)
#endif

/* ---------------------------------------------------------------------------
 * Controller callbacks (weak hooks declared by the controller platform layer)
 * -------------------------------------------------------------------------*/

/* The controller consumed a grant and ran a platform PDS cycle. */
void ipro_ble_ctlr_app_pds_cycle_consumed(void)
{
    uint32_t elapsed_us = ipro_ble_ctlr_pds_life_last_elapsed_us;

    pm_core_account(IPRO_PM_MODE_PDS, elapsed_us);
    pm_core_notify(IPRO_PM_EVT_PDS_EXIT, elapsed_us);
    ipro_pm_ble_cycles++;
    /* safety net: devices are normally resumed from app_pds_resumed() */
    pm_ble_txn_abandon();
    pm_core_devices_resume_early();
    pm_core_devices_resume_late_defer();
}

/* BLE ISR: the first post-PDS event re-anchored the link.  Only this proves
 * the cycle; cycle_consumed() runs before the verdict. */
void ipro_ble_ctlr_app_pds_recovered(void)
{
    ipro_pm_ble_recovered++;
    s_consecutive_failures = 0U;
    s_backoff_ms = IPRO_PM_BLE_BACKOFF_MIN_MS;
}

/* Controller task, last point before the native sleep of a connected PDS
 * commits: holds, vetoes and devices get the final say. */
bool ipro_ble_ctlr_app_pds_prepare(uint32_t sleep_us)
{
    if ((ipro_pm_allowed_mode() < IPRO_PM_MODE_PDS) || !pm_core_vetoes_allow(sleep_us)) {
        ipro_pm_ble_reject_prepare++;
        return false;
    }
    pm_ble_txn_begin(sleep_us);
    if (!pm_core_devices_suspend()) {
        pm_ble_txn_abandon();
        ipro_pm_ble_reject_prepare++;
        return false;
    }
    pm_core_notify(IPRO_PM_EVT_PDS_ENTER, sleep_us);
    return true;
}

/* Right after hal_lp_enter() returns, IRQs masked. */
void ipro_ble_ctlr_app_pds_resumed(void)
{
    pm_ble_txn_wake();
    pm_core_devices_resume_early();
}

/* A recovery disconnect never completed; the host got HCI Hardware Error. */
void ipro_ble_ctlr_app_pds_link_stuck(uint32_t links)
{
    pm_core_pds_failed(0x100U | links);
    pm_core_auto_max_set(IPRO_PM_MODE_WFI);
}

/* The wake after a PDS cycle did not recover the link (or the attempt failed
 * after the grant was consumed). */
void ipro_ble_ctlr_app_pds_recovery_failed(uint32_t reason)
{
    ipro_pm_ble_failures++;
    pm_core_pds_failed(reason);
    pm_ble_txn_abandon();
    pm_core_devices_resume_early();
    pm_core_devices_resume_late_defer();
    s_consecutive_failures++;
    if (s_consecutive_failures >= IPRO_PM_BLE_FAIL_LIMIT) {
        s_consecutive_failures = 0U;
        if (s_backoff_timer == NULL) {
            s_backoff_timer = xTimerCreate("pm_ble_bo", pdMS_TO_TICKS(s_backoff_ms), pdFALSE, NULL,
                                           pm_ble_backoff_end);
        }
        if ((s_backoff_timer != NULL) &&
            (xTimerChangePeriod(s_backoff_timer, pdMS_TO_TICKS(s_backoff_ms), 0U) == pdPASS)) {
            pm_core_auto_max_set(IPRO_PM_MODE_WFI);
            ipro_pm_ble_backoffs++;
        }
        s_backoff_ms = (s_backoff_ms >= (IPRO_PM_BLE_BACKOFF_MAX_MS / 2U)) ? IPRO_PM_BLE_BACKOFF_MAX_MS
                                                                           : (s_backoff_ms * 2U);
    }
}

/* The controller released a grant without running PDS (admission or gate
 * reject): only the window is lost. */
void ipro_ble_ctlr_app_pds_grant_released(void)
{
    /* released after prepare() had suspended the devices (controller task) */
    pm_ble_txn_abandon();
    pm_core_devices_resume_early();
    pm_core_devices_resume_late_defer();
}

/* The controller's native wait can outlast RTOS ticks it did not see (the tick
 * is parked across a PDS cycle): catch the kernel up with the 32 kHz clock. */
#if !defined(CONFIG_IPRO_PM_IDLE_PDS)
static uint64_t s_time_rtc_epoch;
static TickType_t s_time_tick_epoch;
static bool s_time_epoch_valid;
#endif

static void pm_ble_time_sync(void)
{
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
    /* one tick authority: the idle executor's MTIME epoch (MTIME is carried
     * across the controller's PDS by the txn above); a second, RTC-nominal
     * view would pull the kernel ahead of it and stall tickless crediting */
    pm_exec_idle_tick_task_sync();
#else
    uint32_t lo;
    uint32_t hi;
    uint64_t now;
    TickType_t wall_ticks;
    TickType_t current;

    HBN_Get_RTC_Timer_Val(&lo, &hi);
    now = ((uint64_t)hi << 32) | lo;
    current = xTaskGetTickCount();
    if (!s_time_epoch_valid) {
        s_time_rtc_epoch = now;
        s_time_tick_epoch = current;
        s_time_epoch_valid = true;
        return;
    }
    wall_ticks = (TickType_t)(pm_core_rtc_to_us(now - s_time_rtc_epoch) / (1000ULL * portTICK_PERIOD_MS));
    if ((TickType_t)(s_time_tick_epoch + wall_ticks - current) < (portMAX_DELAY / 2U)) {
        TickType_t missing = s_time_tick_epoch + wall_ticks - current;

        if (missing != 0U) {
            /* unlike vTaskStepTick(), this walks past task deadlines safely */
            (void)xTaskCatchUpTicks(missing);
        }
    } else {
        /* the kernel is ahead of the 32 kHz clock (tick quantisation, SysTimer
         * vs RC32K drift): re-base instead of charging the lead against the
         * next PDS gap */
        s_time_tick_epoch = current - wall_ticks;
    }
#endif
}

void ipro_ble_ctlr_native_wait_begin(void)
{
    pm_ble_time_sync();
}

void ipro_ble_ctlr_native_wait_end(void)
{
    pm_ble_time_sync();
}

/* ---------------------------------------------------------------------------
 * Controller bring-up and connection profile
 * -------------------------------------------------------------------------*/
static bool s_pds_available;
static struct bt_conn *s_conn;
static TimerHandle_t s_param_timer;
/* settle requests sent on this link: a central that picks a short interval
 * again (or rejects the request) is asked a bounded number of times */
#define PM_BLE_SETTLE_ATTEMPTS 3U
static uint32_t s_settle_attempts;
volatile uint32_t ipro_pm_ble_param_requests;
volatile int32_t ipro_pm_ble_param_last_err;
volatile uint32_t ipro_pm_ble_profile_pds;   /* 1 while the link is in the PDS profile */
volatile uint32_t ipro_pm_ble_profile_evals;

static void pm_ble_profile_set(bool pds)
{
    if (pds == (ipro_pm_ble_profile_pds != 0U)) {
        return;
    }
    if (pds) {
        /* the controller's 1 ms connected safety poll and its full-window
         * publication run only in this profile */
        ipro_ble_ctlr_tickless_connection_active_set(true);
        (void)ipro_ble_ctlr_pds_auto_config(true, 0U);
        ipro_ble_ctlr_hcitl_awake_guard_ms = IPRO_PM_BLE_HCI_GUARD_MS;
    } else {
        (void)ipro_ble_ctlr_pds_auto_config(false, 100000U);
        ipro_ble_ctlr_tickless_connection_active_set(false);
        ipro_ble_ctlr_hcitl_awake_guard_ms = 100U;
    }
    ipro_pm_ble_profile_pds = pds ? 1U : 0U;
}

/* Slow loop: a link goes to the PDS profile when its connection interval
 * leaves room for a PDS cycle that pays for itself. */
static volatile uint32_t s_link_interval_us;

uint32_t ipro_pm_ble_link_interval_us(void)
{
    return s_link_interval_us;
}

static void pm_ble_profile_eval(uint16_t interval, uint16_t latency, uint16_t timeout)
{
    uint32_t interval_ms = ((uint32_t)interval * 5U) / 4U;
    bool pds = s_pds_available && (interval_ms >= CONFIG_IPRO_PM_BLE_PDS_MIN_INTERVAL_MS) &&
               (((uint32_t)timeout * 10U) >= (3U * interval_ms * ((uint32_t)latency + 1U)));

    ipro_pm_ble_profile_evals++;
    /* with slave latency the controller wakes every (latency + 1) intervals */
    s_link_interval_us = pds ? ((uint32_t)interval * 1250U * ((uint32_t)latency + 1U)) : 0U;
    pm_ble_profile_set(pds);
}

static int pm_ble_params_request_conn(struct bt_conn *conn, uint32_t idle_ms)
{
    ipro_pm_ble_conn_param_t hint;
    struct bt_le_conn_param param;
    int err;

    ipro_pm_ble_conn_param_hint(idle_ms, &hint);
    param.interval_min = hint.interval_min;
    param.interval_max = hint.interval_max;
    param.latency = hint.latency;
    param.timeout = hint.timeout;
    err = bt_conn_le_param_update(conn, &param);
    ipro_pm_ble_param_requests++;
    ipro_pm_ble_param_last_err = err;
    return err;
}

/* A reference to the tracked link, safe against a concurrent disconnect in
 * the host thread (the scheduler is held while the pointer is sampled). */
static struct bt_conn *pm_ble_conn_get(void)
{
    struct bt_conn *conn;

    vTaskSuspendAll();
    conn = (s_conn != NULL) ? bt_conn_ref(s_conn) : NULL;
    (void)xTaskResumeAll();
    return conn;
}

#if (CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS > 0)
/* Below the window the settle request asks for (the hint rounds the idle
 * interval to a 15 ms multiple: 1000 ms asks for 990..1005 ms). */
static bool pm_ble_below_idle_window(uint16_t interval)
{
    ipro_pm_ble_conn_param_t hint;

    ipro_pm_ble_conn_param_hint(CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS, &hint);
    return interval < hint.interval_min;
}

/* Timer task: the link has settled, ask the central for the idle profile. */
static void pm_ble_param_timer_cb(TimerHandle_t timer)
{
    struct bt_conn *conn = pm_ble_conn_get();
    struct bt_conn_info info;

    (void)timer;
    if (conn == NULL) {
        return;
    }
    if ((bt_conn_get_info(conn, &info) == 0) && pm_ble_below_idle_window(info.le.interval) &&
        (s_settle_attempts < PM_BLE_SETTLE_ATTEMPTS)) {
        s_settle_attempts++;
        if (pm_ble_params_request_conn(conn, CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS) != 0) {
            /* a parameter procedure still under way (the application's own
             * request right after the connection): ask again later */
            (void)xTimerReset(timer, 0U);
        }
    }
    bt_conn_unref(conn);
}
#endif

int ipro_pm_ble_params_request(uint32_t idle_ms)
{
    struct bt_conn *conn = pm_ble_conn_get();
    int err;

    if (conn == NULL) {
        return -ENOTCONN;
    }
    err = pm_ble_params_request_conn(conn, idle_ms);
    bt_conn_unref(conn);
    return err;
}

void ipro_pm_ble_diag_get(ipro_pm_ble_diag_t *out)
{
    if (out == NULL) {
        return;
    }
    /* one consistent set against the controller task (the recovery count
     * comes from the BLE ISR and may land either side) */
    taskENTER_CRITICAL();
    out->grants = ipro_pm_ble_grants;
    out->cycles = ipro_pm_ble_cycles;
    out->recovered = ipro_pm_ble_recovered;
    out->failures = ipro_pm_ble_failures;
    out->reject_budget = ipro_pm_ble_reject_budget;
    out->reject_veto = ipro_pm_ble_reject_veto;
    out->reject_controller = ipro_pm_ble_reject_controller;
    out->reject_prepare = ipro_pm_ble_reject_prepare;
    out->reject_stale = ipro_pm_ble_reject_stale;
    out->retry_hci = ipro_pm_ble_reject_hci;
    out->backoffs = ipro_pm_ble_backoffs;
    taskEXIT_CRITICAL();
}

static void pm_ble_param_timer_start(void)
{
#if (CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS > 0)
    if (s_param_timer == NULL) {
        s_param_timer = xTimerCreate("pm_ble_prm", pdMS_TO_TICKS(CONFIG_IPRO_PM_BLE_IDLE_DELAY_MS), pdFALSE,
                                     NULL, pm_ble_param_timer_cb);
    }
    if (s_param_timer != NULL) {
        (void)xTimerReset(s_param_timer, 0U);
    }
#endif
}

static void pm_ble_param_timer_stop(void)
{
    if (s_param_timer != NULL) {
        (void)xTimerStop(s_param_timer, 0U);
    }
}

static void pm_ble_connected(struct bt_conn *conn, uint8_t err)
{
    struct bt_conn_info info;

    if ((err != 0U) || (s_conn != NULL)) {
        return; /* one link at a time in the PDS profile */
    }
    if ((bt_conn_get_info(conn, &info) != 0) || (info.type != BT_CONN_TYPE_LE)) {
        return;
    }
    /* no reference is held: the object stays valid until every disconnected
     * callback has run, and a held reference kept the link's slot busy while
     * the application restarted advertising from its own callback (-ENOMEM) */
    s_conn = conn;
    s_settle_attempts = 0U;
    /* a connection proves the host finished HCI initialization; until the
     * transport is marked ready the controller never sleeps (apps that did
     * not call ipro_ble_ctlr_hcitl_host_ready() themselves) */
    ipro_ble_ctlr_hcitl_host_ready();
    (void)ipro_ble_ctlr_pds_auto_config(false, 100000U);
    ipro_ble_ctlr_pds_auto_connected(true);
    pm_ble_profile_eval(info.le.interval, info.le.latency, info.le.timeout);
    if (s_pds_available) {
        pm_ble_param_timer_start();
    }
}

static void pm_ble_disconnected(struct bt_conn *conn, uint8_t reason)
{
    (void)reason;
    if (conn != s_conn) {
        return;
    }
    pm_ble_param_timer_stop();
    s_link_interval_us = 0U;
    pm_ble_profile_set(false);
    ipro_ble_ctlr_pds_auto_connected(false);
    vTaskSuspendAll();
    s_conn = NULL;
    (void)xTaskResumeAll();
}

static void pm_ble_param_updated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
    if (conn == s_conn) {
        pm_ble_profile_eval(interval, latency, timeout);
#if (CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS > 0)
        /* the application's own short interval right after the connection
         * overtook the settle request: settle again from here (bounded -
         * a central that insists on a short interval, or rejects, wins) */
        if (s_pds_available && pm_ble_below_idle_window(interval) && (s_settle_attempts < PM_BLE_SETTLE_ATTEMPTS)) {
            pm_ble_param_timer_start();
        }
#endif
    }
}

static struct bt_conn_cb s_conn_cb = {
    .connected = pm_ble_connected,
    .disconnected = pm_ble_disconnected,
    .le_param_updated = pm_ble_param_updated,
};

int ipro_pm_ble_controller_init(unsigned int task_priority)
{
    int err;

    /* connected-BLE PDS domain profile and measured RTC wake timing; only
     * consulted when the controller actually enters PDS */
    pm_pds_ble_ctlr_domain_policy_set(PM_PDS_BLE_CTLR_DOMAIN_POLICY_CONNECTED_BASELINE |
                                      PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE);
    hal_lp_set_timing_policy(HAL_LP_TIMING_POLICY_EXACT_US_TO_RTC | HAL_LP_TIMING_POLICY_PDS_WAKE_DELAY_VALID,
                             1021U);
    err = ipro_ble_ctlr_controller_init(task_priority);
    if (err != 0) {
        return err;
    }
#if defined(CONFIG_IPRO_PM_PDS_CONNECTED)
    s_pds_available = (ipro_ble_ctlr_pds_enable(true) == 0) &&
                      (ipro_ble_ctlr_pds_auto_config(false, 100000U) == 0);
    if (s_pds_available) {
        /* size post-PDS recovery windows with the 32 kHz sleep-clock bound */
        ipro_ble_ctlr_platform_sleep_drift_enable(true);
    }
#else
    (void)ipro_ble_ctlr_pds_enable(false);
    s_pds_available = false;
#endif
    if (!s_pds_available) {
        pm_core_auto_max_set(IPRO_PM_MODE_WFI);
    }
    (void)bt_conn_cb_register(&s_conn_cb);
    return 0;
}

void ipro_pm_ble_conn_param_hint(uint32_t idle_ms, ipro_pm_ble_conn_param_t *out)
{
    uint32_t interval_ms;
    uint32_t timeout_ms;

    if (out == NULL) {
        return;
    }
    /* Interval window [interval_ms, interval_ms + 15]: multiples of 15 ms,
     * at least the PDS profile threshold, the maximum at most 1995 ms (Apple:
     * interval_max * (latency + 1) <= 2 s).  Timeout: the largest Apple
     * accepts (6 s) as long as it stays above 3x the maximum interval, so a
     * short burst of lost events (RF, a missed post-PDS anchor) does not end
     * the link; never below 2 s. */
    interval_ms = (idle_ms / 15U) * 15U;
    if (interval_ms < CONFIG_IPRO_PM_BLE_PDS_MIN_INTERVAL_MS) {
        interval_ms = ((CONFIG_IPRO_PM_BLE_PDS_MIN_INTERVAL_MS + 14U) / 15U) * 15U;
    }
    if (interval_ms > 1980U) {
        interval_ms = 1980U;
    }
    timeout_ms = 6000U;
    if (timeout_ms <= (3U * (interval_ms + 15U))) {
        timeout_ms = (((3U * (interval_ms + 15U)) + 10U + 9U) / 10U) * 10U;
    }
    out->interval_min = (uint16_t)((interval_ms * 4U) / 5U);
    out->interval_max = (uint16_t)(((interval_ms + 15U) * 4U) / 5U);
    out->latency = 0U;
    out->timeout = (uint16_t)(timeout_ms / 10U);
}

/* `pm ble`: why the connected PDS is (not) running */
void pm_ble_dump(void)
{
    printf("pm ble: profile %lu link %lu us | ctlr token %lu gen %lu slack %lu hus | committed %lu rejected %lu\r\n",
           (unsigned long)ipro_pm_ble_profile_pds, (unsigned long)s_link_interval_us,
           (unsigned long)ipro_ble_ctlr_pds_candidate_token, (unsigned long)ipro_ble_ctlr_pds_full_generation,
           (unsigned long)ipro_ble_ctlr_pds_candidate_slack_hus, (unsigned long)s_last_committed_generation,
           (unsigned long)s_last_rejected_generation);
    printf("  grants %lu cycles %lu recovered %lu failures %lu (in a row %lu) backoffs %lu (%lu ms)\r\n",
           (unsigned long)ipro_pm_ble_grants, (unsigned long)ipro_pm_ble_cycles, (unsigned long)ipro_pm_ble_recovered,
           (unsigned long)ipro_pm_ble_failures, (unsigned long)s_consecutive_failures,
           (unsigned long)ipro_pm_ble_backoffs, (unsigned long)s_backoff_ms);
    printf("  last window %lu us | ctlr released: admit %lu gate %lu stale %lu rw-declined %lu after-commit %lu"
           " | us timer rescued %lu\r\n",
           (unsigned long)s_window_us, (unsigned long)ipro_ble_ctlr_pds_release_cnt[IPRO_BLE_CTLR_PDS_REL_ADMIT],
           (unsigned long)ipro_ble_ctlr_pds_release_cnt[IPRO_BLE_CTLR_PDS_REL_GATE],
           (unsigned long)ipro_ble_ctlr_pds_release_cnt[IPRO_BLE_CTLR_PDS_REL_STALE],
           (unsigned long)ipro_ble_ctlr_pds_release_cnt[IPRO_BLE_CTLR_PDS_REL_RW_DECLINED],
           (unsigned long)ipro_ble_ctlr_pds_release_cnt[IPRO_BLE_CTLR_PDS_REL_AFTER_COMMIT],
           (unsigned long)ipro_ble_ctlr_pds_us_timer_rescued);
    printf("  rejects: stale %lu hci %lu budget %lu veto %lu controller %lu prepare %lu | reval reason %lu"
           " | param requests %lu\r\n",
           (unsigned long)ipro_pm_ble_reject_stale, (unsigned long)ipro_pm_ble_reject_hci,
           (unsigned long)ipro_pm_ble_reject_budget, (unsigned long)ipro_pm_ble_reject_veto,
           (unsigned long)ipro_pm_ble_reject_controller, (unsigned long)ipro_pm_ble_reject_prepare,
           (unsigned long)ipro_ble_ctlr_pds_reval_last_reason, (unsigned long)ipro_pm_ble_param_requests);
}
