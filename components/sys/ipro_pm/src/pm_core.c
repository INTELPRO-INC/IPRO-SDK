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
 * @file pm_core.c
 * @brief ipro_pm core: hold reasons, vetoes, devices, notifiers, statistics.
 *
 * Everything here is policy bookkeeping.  The FreeRTOS port (pm_freertos.c)
 * asks ipro_pm_allowed_mode() for every idle window and reports what it did
 * through pm_core_account(); the radio participants report PDS outcomes.
 */
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "drv_hbn.h"
#include "ipro_pm.h"
#include "pm_internal.h"

#ifndef CONFIG_IPRO_PM_MAX_REASONS
#define CONFIG_IPRO_PM_MAX_REASONS 16
#endif
#ifndef CONFIG_IPRO_PM_MAX_VETOES
#define CONFIG_IPRO_PM_MAX_VETOES 4
#endif
#ifndef CONFIG_IPRO_PM_MAX_DEVICES
#define CONFIG_IPRO_PM_MAX_DEVICES 8
#endif
#ifndef CONFIG_IPRO_PM_MAX_NOTIFIERS
#define CONFIG_IPRO_PM_MAX_NOTIFIERS 4
#endif
#ifndef CONFIG_IPRO_PM_MAX_MODE
#define CONFIG_IPRO_PM_MAX_MODE 1 /* WFI */
#endif

/* Energy model (uA while in each mode; PDS cycle overhead in uA*ms).  The
 * defaults are the measured IPRO7 figures where one exists: PDS floor with the
 * connected KEEP_RF policy ~80 uA, snapshot/restore ~11.4 ms at ~3 mA.  WFI and
 * active are estimates until measured; see Kconfig. */
#ifndef CONFIG_IPRO_PM_MODEL_ACTIVE_UA
#define CONFIG_IPRO_PM_MODEL_ACTIVE_UA 3500
#endif
#ifndef CONFIG_IPRO_PM_MODEL_WFI_UA
#define CONFIG_IPRO_PM_MODEL_WFI_UA 600
#endif
#ifndef CONFIG_IPRO_PM_MODEL_NATIVE_UA
#define CONFIG_IPRO_PM_MODEL_NATIVE_UA 550
#endif
#ifndef CONFIG_IPRO_PM_MODEL_PDS_UA
#define CONFIG_IPRO_PM_MODEL_PDS_UA 80
#endif
#ifndef CONFIG_IPRO_PM_MODEL_PDS_CYCLE_UAMS
#define CONFIG_IPRO_PM_MODEL_PDS_CYCLE_UAMS 34200
#endif

typedef struct {
    const char *name;
    uint8_t max_mode;
    volatile uint16_t count;
    volatile TickType_t until; /* hold_for_ms expiry, 0 = none */
} pm_reason_t;

static pm_reason_t s_reasons[CONFIG_IPRO_PM_MAX_REASONS];
static uint8_t s_nb_reasons;

static struct {
    ipro_pm_veto_t fn;
    void *ctx;
} s_vetoes[CONFIG_IPRO_PM_MAX_VETOES];
static uint8_t s_nb_vetoes;

static ipro_pm_device_t s_devices[CONFIG_IPRO_PM_MAX_DEVICES];
static uint8_t s_nb_devices;
static uint8_t s_suspended; /* number of devices suspended by the current PDS */
static volatile uint32_t s_late_mask; /* devices resumed early, resume_late pending */

static struct {
    ipro_pm_notifier_t fn;
    void *ctx;
} s_notifiers[CONFIG_IPRO_PM_MAX_NOTIFIERS];
static uint8_t s_nb_notifiers;

static volatile ipro_pm_mode_t s_policy = (ipro_pm_mode_t)CONFIG_IPRO_PM_MAX_MODE;
static volatile ipro_pm_mode_t s_auto_max = IPRO_PM_MODE_PDS;

static struct {
    uint64_t residency_us[IPRO_PM_MODE_COUNT];
    uint32_t entries[IPRO_PM_MODE_COUNT];
    uint32_t pds_failures;
    uint32_t pds_rejects;
    uint32_t pds_aborts;
    uint32_t demotions;
    uint64_t start_us;
} s_stats;

static uint64_t s_uptime_epoch;
static bool s_initialised;
static pm_txn_t s_txn;

/* -----------------------------------------------------------------------
 * Time
 * -----------------------------------------------------------------------*/
static uint64_t pm_rtc_now(void)
{
    uint32_t lo;
    uint32_t hi;

    HBN_Get_RTC_Timer_Val(&lo, &hi);
    return ((uint64_t)hi << 32) | lo;
}

uint64_t pm_core_rtc_to_us(uint64_t ticks)
{
    return (ticks * 15625ULL) / 512ULL; /* 1e6 / 32768 */
}

uint64_t ipro_pm_uptime_us(void)
{
    return pm_core_rtc_to_us(pm_rtc_now() - s_uptime_epoch);
}

/* -----------------------------------------------------------------------
 * Reasons
 * -----------------------------------------------------------------------*/
ipro_pm_reason_t ipro_pm_reason_register(const char *name, ipro_pm_mode_t max_mode)
{
    ipro_pm_reason_t id = -1;

    if (max_mode >= IPRO_PM_MODE_COUNT) {
        return -1;
    }
    taskENTER_CRITICAL();
    if (s_nb_reasons < CONFIG_IPRO_PM_MAX_REASONS) {
        id = (ipro_pm_reason_t)s_nb_reasons;
        s_reasons[id].name = name;
        s_reasons[id].max_mode = (uint8_t)max_mode;
        s_reasons[id].count = 0U;
        s_reasons[id].until = 0U;
        s_nb_reasons++;
    }
    taskEXIT_CRITICAL();
    return id;
}

static bool pm_reason_valid(ipro_pm_reason_t reason)
{
    return (reason >= 0) && (reason < (ipro_pm_reason_t)s_nb_reasons);
}

void ipro_pm_hold(ipro_pm_reason_t reason)
{
    if (!pm_reason_valid(reason)) {
        return;
    }
    taskENTER_CRITICAL();
    if (s_reasons[reason].count != UINT16_MAX) {
        s_reasons[reason].count++;
    }
    taskEXIT_CRITICAL();
}

void ipro_pm_release(ipro_pm_reason_t reason)
{
    if (!pm_reason_valid(reason)) {
        return;
    }
    taskENTER_CRITICAL();
    if (s_reasons[reason].count != 0U) {
        s_reasons[reason].count--;
    }
    taskEXIT_CRITICAL();
}

void ipro_pm_hold_isr(ipro_pm_reason_t reason)
{
    UBaseType_t mask;

    if (!pm_reason_valid(reason)) {
        return;
    }
    mask = taskENTER_CRITICAL_FROM_ISR();
    if (s_reasons[reason].count != UINT16_MAX) {
        s_reasons[reason].count++;
    }
    taskEXIT_CRITICAL_FROM_ISR(mask);
}

void ipro_pm_release_isr(ipro_pm_reason_t reason)
{
    UBaseType_t mask;

    if (!pm_reason_valid(reason)) {
        return;
    }
    mask = taskENTER_CRITICAL_FROM_ISR();
    if (s_reasons[reason].count != 0U) {
        s_reasons[reason].count--;
    }
    taskEXIT_CRITICAL_FROM_ISR(mask);
}

/* A timed hold whose expiry tick has passed (compared in the half-range, so
 * it must be cleared before 2^31 ticks or it would read as pending again). */
static bool pm_reason_expired(TickType_t until, TickType_t now)
{
    return ((TickType_t)(until - now) >= (portMAX_DELAY / 2U)) || (until == now);
}

void ipro_pm_hold_for_ms(ipro_pm_reason_t reason, uint32_t ms)
{
    TickType_t until;

    if (!pm_reason_valid(reason)) {
        return;
    }
    until = xTaskGetTickCount() + pdMS_TO_TICKS(ms);
    if (until == 0U) {
        until = 1U; /* 0 means "no timed hold" */
    }
    taskENTER_CRITICAL();
    if ((s_reasons[reason].until == 0U) || pm_reason_expired(s_reasons[reason].until, xTaskGetTickCount()) ||
        ((TickType_t)(until - s_reasons[reason].until) < (portMAX_DELAY / 2U))) {
        s_reasons[reason].until = until;
    }
    taskEXIT_CRITICAL();
}

void ipro_pm_hold_for_ms_isr(ipro_pm_reason_t reason, uint32_t ms)
{
    TickType_t now;
    TickType_t until;
    UBaseType_t mask;

    if (!pm_reason_valid(reason)) {
        return;
    }
    now = xTaskGetTickCountFromISR();
#if defined(CONFIG_IPRO_PM_IDLE_PDS) && defined(CONFIG_IPRO7)
    /* the interrupt that ended a tickless WFI runs before the port steps the
     * slept ticks: count from where the kernel is heading, not from there */
    now = pm_exec_idle_wall_ticks(now);
#endif
    until = now + pdMS_TO_TICKS(ms);
    if (until == 0U) {
        until = 1U;
    }
    mask = taskENTER_CRITICAL_FROM_ISR();
    if ((s_reasons[reason].until == 0U) || pm_reason_expired(s_reasons[reason].until, now) ||
        ((TickType_t)(until - s_reasons[reason].until) < (portMAX_DELAY / 2U))) {
        s_reasons[reason].until = until;
    }
    taskEXIT_CRITICAL_FROM_ISR(mask);
}

/* Lock-free: called from the idle hooks with interrupts masked. */
ipro_pm_mode_t ipro_pm_allowed_mode(void)
{
    ipro_pm_mode_t mode = s_policy;
    TickType_t now = xTaskGetTickCount();

    if (s_auto_max < mode) {
        mode = s_auto_max;
    }
    for (uint8_t i = 0U; i < s_nb_reasons; i++) {
        bool held = (s_reasons[i].count != 0U);

        if (!held && (s_reasons[i].until != 0U)) {
            held = ((TickType_t)(s_reasons[i].until - now) < (portMAX_DELAY / 2U)) &&
                   (s_reasons[i].until != now);
            if (!held) {
                /* expired: clear it unless a hold_for_ms() just renewed it */
                TickType_t seen = s_reasons[i].until;
                UBaseType_t mask = taskENTER_CRITICAL_FROM_ISR();

                if ((s_reasons[i].until == seen) && pm_reason_expired(seen, now)) {
                    s_reasons[i].until = 0U;
                }
                taskEXIT_CRITICAL_FROM_ISR(mask);
            }
        }
        if (held && ((ipro_pm_mode_t)s_reasons[i].max_mode < mode)) {
            mode = (ipro_pm_mode_t)s_reasons[i].max_mode;
        }
    }
    return mode;
}

uint32_t pm_core_reasons_dump(pm_reason_view_t *out, uint32_t max)
{
    uint32_t n = 0U;

    for (uint8_t i = 0U; (i < s_nb_reasons) && (n < max); i++, n++) {
        out[n].name = s_reasons[i].name;
        out[n].max_mode = (ipro_pm_mode_t)s_reasons[i].max_mode;
        out[n].count = s_reasons[i].count;
        out[n].timed = (s_reasons[i].until != 0U);
    }
    return n;
}

/* -----------------------------------------------------------------------
 * Policy and automatic ceiling
 * -----------------------------------------------------------------------*/
void ipro_pm_policy_set(ipro_pm_mode_t max_mode)
{
    if (max_mode < IPRO_PM_MODE_COUNT) {
        s_policy = max_mode;
    }
}

ipro_pm_mode_t ipro_pm_policy_get(void)
{
    return s_policy;
}

void pm_core_auto_max_set(ipro_pm_mode_t mode)
{
    ipro_pm_mode_t old = s_auto_max;

    if ((mode >= IPRO_PM_MODE_COUNT) || (mode == old)) {
        return;
    }
    s_auto_max = mode;
    if (mode < old) {
        s_stats.demotions++;
        pm_core_notify(IPRO_PM_EVT_DEMOTED, (uint32_t)mode);
    } else {
        pm_core_notify(IPRO_PM_EVT_PROMOTED, (uint32_t)mode);
    }
}

ipro_pm_mode_t pm_core_auto_max_get(void)
{
    return s_auto_max;
}

/* -----------------------------------------------------------------------
 * Vetoes, devices, notifiers
 * -----------------------------------------------------------------------*/
int ipro_pm_veto_register(ipro_pm_veto_t veto, void *ctx)
{
    int rc = -1;

    if (veto == NULL) {
        return -1;
    }
    taskENTER_CRITICAL();
    if (s_nb_vetoes < CONFIG_IPRO_PM_MAX_VETOES) {
        s_vetoes[s_nb_vetoes].fn = veto;
        s_vetoes[s_nb_vetoes].ctx = ctx;
        s_nb_vetoes++;
        rc = 0;
    }
    taskEXIT_CRITICAL();
    return rc;
}

bool pm_core_vetoes_allow(uint32_t sleep_us)
{
    for (uint8_t i = 0U; i < s_nb_vetoes; i++) {
        if (!s_vetoes[i].fn(sleep_us, s_vetoes[i].ctx)) {
            return false;
        }
    }
    return true;
}

int ipro_pm_device_register(const ipro_pm_device_t *dev)
{
    int rc = -1;

    if (dev == NULL) {
        return -1;
    }
    taskENTER_CRITICAL();
    if (s_nb_devices < CONFIG_IPRO_PM_MAX_DEVICES) {
        s_devices[s_nb_devices] = *dev;
        s_nb_devices++;
        rc = 0;
    }
    taskEXIT_CRITICAL();
    return rc;
}

/* Called with interrupts masked right before PDS.  On a veto, the devices
 * already suspended are resumed in reverse order and false is returned. */
bool pm_core_devices_suspend(void)
{
    s_suspended = 0U;
    for (uint8_t i = 0U; i < s_nb_devices; i++) {
        if ((s_devices[i].suspend != NULL) && (s_devices[i].suspend(s_devices[i].ctx) != 0)) {
            pm_core_devices_resume_early();
            return false;
        }
        s_suspended = (uint8_t)(i + 1U);
    }
    return true;
}

void pm_core_devices_resume_early(void)
{
    while (s_suspended != 0U) {
        s_suspended--;
        s_late_mask |= UINT32_C(1) << s_suspended;
        if (s_devices[s_suspended].resume_early != NULL) {
            s_devices[s_suspended].resume_early(s_devices[s_suspended].ctx);
        }
    }
}

static void pm_core_resume_late_cb(void *arg1, uint32_t arg2)
{
    uint32_t mask;

    (void)arg1;
    (void)arg2;
    taskENTER_CRITICAL();
    mask = s_late_mask;
    s_late_mask = 0U;
    taskEXIT_CRITICAL();
    for (uint8_t i = 0U; i < s_nb_devices; i++) {
        if (((mask & (UINT32_C(1) << i)) != 0U) && (s_devices[i].resume_late != NULL)) {
            s_devices[i].resume_late(s_devices[i].ctx);
        }
    }
}

/* Task context.  Queues resume_late for the devices resumed early; a pend
 * that fails (full timer queue) leaves the mask set for the next call. */
void pm_core_devices_resume_late_defer(void)
{
    if (s_late_mask != 0U) {
        (void)xTimerPendFunctionCall(pm_core_resume_late_cb, NULL, 0U, 0U);
    }
}

int ipro_pm_notifier_register(ipro_pm_notifier_t cb, void *ctx)
{
    int rc = -1;

    if (cb == NULL) {
        return -1;
    }
    taskENTER_CRITICAL();
    if (s_nb_notifiers < CONFIG_IPRO_PM_MAX_NOTIFIERS) {
        s_notifiers[s_nb_notifiers].fn = cb;
        s_notifiers[s_nb_notifiers].ctx = ctx;
        s_nb_notifiers++;
        rc = 0;
    }
    taskEXIT_CRITICAL();
    return rc;
}

void pm_core_notify(ipro_pm_event_t evt, uint32_t arg)
{
    for (uint8_t i = 0U; i < s_nb_notifiers; i++) {
        s_notifiers[i].fn(evt, arg, s_notifiers[i].ctx);
    }
}

/* -----------------------------------------------------------------------
 * Statistics
 * -----------------------------------------------------------------------*/
void pm_core_account(ipro_pm_mode_t mode, uint64_t us)
{
    if (mode >= IPRO_PM_MODE_COUNT) {
        return;
    }
    s_stats.residency_us[mode] += us;
    s_stats.entries[mode]++;
}

void pm_core_pds_failed(uint32_t reason)
{
    s_stats.pds_failures++;
    pm_core_notify(IPRO_PM_EVT_PDS_FAILED, reason);
}

void pm_core_pds_rejected(void)
{
    s_stats.pds_rejects++;
}

void pm_core_pds_aborted(int32_t rc)
{
    s_stats.pds_aborts++;
    pm_core_notify(IPRO_PM_EVT_PDS_ABORTED, (uint32_t)rc);
}

/* -----------------------------------------------------------------------
 * Transaction record (device callbacks read it)
 * -----------------------------------------------------------------------*/
pm_txn_t *pm_core_txn_begin(pm_exec_id_t exec, uint32_t sleep_us)
{
    s_txn.generation++;
    s_txn.executor = exec;
    s_txn.sleep_us = sleep_us;
    s_txn.elapsed_us = 0U;
    s_txn.wake_reason = 0U;
    s_txn.domain_policy = 0U;
    s_txn.slept = false;
    return &s_txn;
}

const pm_txn_t *pm_txn_current(void)
{
    return &s_txn;
}

void ipro_pm_stats_reset(void)
{
    taskENTER_CRITICAL();
    memset(&s_stats, 0, sizeof(s_stats));
    s_stats.start_us = ipro_pm_uptime_us();
    taskEXIT_CRITICAL();
}

void ipro_pm_stats_get(ipro_pm_stats_t *out)
{
    static const uint32_t model_ua[IPRO_PM_MODE_COUNT] = {
        CONFIG_IPRO_PM_MODEL_ACTIVE_UA,
        CONFIG_IPRO_PM_MODEL_WFI_UA,
        CONFIG_IPRO_PM_MODEL_NATIVE_UA,
        CONFIG_IPRO_PM_MODEL_PDS_UA,
    };
    uint64_t sleep_us = 0U;
    uint64_t charge = 0U; /* uA * us */

    if (out == NULL) {
        return;
    }
    memset(out, 0, sizeof(*out));
    taskENTER_CRITICAL();
    out->window_us = ipro_pm_uptime_us() - s_stats.start_us;
    for (uint32_t m = IPRO_PM_MODE_WFI; m < IPRO_PM_MODE_COUNT; m++) {
        out->residency_us[m] = s_stats.residency_us[m];
        out->entries[m] = s_stats.entries[m];
        sleep_us += s_stats.residency_us[m];
    }
    out->pds_failures = s_stats.pds_failures;
    out->pds_rejects = s_stats.pds_rejects;
    out->pds_aborts = s_stats.pds_aborts;
    out->demotions = s_stats.demotions;
    taskEXIT_CRITICAL();

    /* Everything not accounted to a sleep mode was spent awake. */
    out->residency_us[IPRO_PM_MODE_ACTIVE] =
        (out->window_us > sleep_us) ? (out->window_us - sleep_us) : 0U;
    out->entries[IPRO_PM_MODE_ACTIVE] = 0U;
    for (uint32_t m = 0U; m < IPRO_PM_MODE_COUNT; m++) {
        charge += out->residency_us[m] * model_ua[m];
    }
    charge += (uint64_t)out->entries[IPRO_PM_MODE_PDS] * CONFIG_IPRO_PM_MODEL_PDS_CYCLE_UAMS * 1000ULL;
    out->est_avg_ua = (out->window_us != 0U) ? (uint32_t)(charge / out->window_us) : 0U;
    out->auto_max = s_auto_max;
}

/* -----------------------------------------------------------------------
 * Init
 * -----------------------------------------------------------------------*/
#if defined(CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS) && (CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS > 0)
/* Console input keeps the part at WFI for a while: the UART cannot receive in
 * PDS, so a command typed right after a wake would lose its later bytes, and
 * the next one would not be seen at all.  Type (or have the bench send) one
 * character, wait for the echo, then the command. */
static ipro_pm_reason_t s_console_rx_reason = -1;

void shell_rx_activity_hook(void)
{
    ipro_pm_hold_for_ms_isr(s_console_rx_reason, CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS);
}
#endif

#if defined(CONFIG_IPRO_PM_CONSOLE_VETO)
#include "drv_uart.h"

/* Built-in veto: the console UART is still shifting out a log line, which a
 * PDS would cut (the rest of the line is lost).  IRQs masked: a register read. */
static bool pm_console_veto(uint32_t sleep_us, void *ctx)
{
    (void)sleep_us;
    (void)ctx;
    return UART_GetTxBusBusyStatus(UART0_ID) != SET;
}
#endif

int ipro_pm_init(void)
{
    if (s_initialised) {
        return 0;
    }
    HBN_Enable_RTC_Counter();
    s_uptime_epoch = pm_rtc_now();
    memset(&s_stats, 0, sizeof(s_stats));
    s_initialised = true;
    pm_port_init();
    pm_arbiter_init();
#if defined(PM_HAVE_TIMEBASE)
    pm_lpclk_init();
#endif
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
    pm_exec_idle_init();
#endif
#if defined(IPRO_PM_HAS_BLE)
    pm_ble_init();
#endif
#if defined(CONFIG_IPRO_PM_CONSOLE_VETO)
    (void)ipro_pm_veto_register(pm_console_veto, NULL);
#endif
#if defined(CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS) && (CONFIG_IPRO_PM_CONSOLE_RX_HOLD_MS > 0)
    s_console_rx_reason = ipro_pm_reason_register("console", IPRO_PM_MODE_WFI);
#endif
#if defined(CONFIG_IPRO_PM_SHELL)
    pm_shell_init();
#endif
    return 0;
}
