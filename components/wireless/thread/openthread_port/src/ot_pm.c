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
 * @file ot_pm.c
 * @brief OpenThread / 802.15.4 as an ipro_pm radio participant.
 *
 * The radio cannot wake the chip from PDS31 (only the RTC and GPIOs can), so
 * a PDS may only span the time the radio has nothing to do:
 *
 *   state     READY when nothing is in flight or timed and the receiver is
 *             off; RETRY for a short activity (a frame, a CSL window, a data
 *             poll's wait); BUSY with the receiver on while idle (MED,
 *             router): such a device never sleeps deeper than WFI
 *   deadline  the microsecond alarm (the next CSL sample window) minus a
 *             lead; OpenThread's millisecond alarm is an RTOS timer that the
 *             power manager sees by itself
 *   suspend   re-check, stop the microsecond timer, remember the MAC setup
 *   resume    MAC configuration written back when the PDS reset it (the SoC
 *             reset the PDS needs to wake reliably takes the MAC with it),
 *             the RF warmed up from the OT task when the PDS powered it
 *             down, the microsecond timer re-armed on the rebased clock
 */
#include <stdio.h>
#include <string.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include <openthread/link.h>
#include <openthread/platform/time.h>
#include <openthread/thread.h>

#include "ieee802154.h"
#include "hal_pm.h"
#include "ieee802154_hal.h"
#include "ipro_pm.h"
#include "ipro_pm_radio.h"
#include "openthread_port.h"
#include "ot_pm.h"
#if defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE)
#include "ot_radio_csl.h"
#endif
#if defined(CONFIG_COEX_BZ)
#include "coex_bz.h"
#endif

#if defined(CONFIG_THREAD_LP_SLEEP)
#error "CONFIG_IPRO_PM_THREAD replaces the legacy CONFIG_THREAD_LP_SLEEP (both own PDS entry)"
#endif

/* the CPU is back and the radio restored this long before the microsecond
 * alarm (it fires ahead of the CSL sample time by OpenThread's own margin) */
#ifndef CONFIG_IPRO_PM_THREAD_WAKE_LEAD_US
#define CONFIG_IPRO_PM_THREAD_WAKE_LEAD_US 1000
#endif

static otInstance *s_instance;
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
/* a BLE-run PDS just ended: line the next data poll up (OT task) */
static volatile bool s_align_pending;
static uint32_t s_align_wake_us; /* when that PDS woke (OpenThread us clock) */
#endif

/* the microsecond clock of the alarm (MTIME), also in builds without the
 * microsecond alarm (a SED without CSL) */
static inline uint32_t ot_pm_now_us(void)
{
    return (uint32_t)otPlatTimeGet();
}

static struct {
    uint32_t ready;
    uint32_t retry;
    uint32_t busy_rx_idle;
    uint32_t why_hal[8];   /* ieee802154_hal_pds_busy() bits */
    uint32_t why_radio[4]; /* ot_radio_pm_busy() bits */
    uint32_t why_events;
    uint32_t suspends;
    uint32_t suspend_vetoes;
    uint32_t resumes_slept;
    uint32_t mac_lost;
    uint32_t mac_restored;
    uint32_t rf_requests;
    uint32_t deadline_min_us;
    uint32_t restore_n;
    uint32_t restore_max_us;
    uint64_t restore_sum_us;
} s_st;

/* longest radio restore seen: the wake lead grows with it */
static volatile uint32_t s_restore_lead_us;

static void ot_pm_count_bits(uint32_t bits, uint32_t *tab, uint32_t n)
{
    for (uint32_t i = 0U; i < n; i++) {
        if ((bits & (1U << i)) != 0U) {
            tab[i]++;
        }
    }
}

/* IRQs masked, lock-free: register and flag reads only. */
static pm_part_state_t ot_pm_state(void)
{
    uint32_t hal;
    uint32_t radio;

    if ((s_instance == NULL) || !ot_radio_pm_enabled()) {
        return PM_PART_ABSENT;
    }
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
        /* the idle pre-sleep pass only: `pm status` calls this from the
         * shell task, which can preempt the OT task mid-transmit */
        ieee802154_hal_rx_stray_check();
    }
    hal = ieee802154_hal_pds_busy();
    /* the link mode (not otPlatRadioSetRxOnWhenIdle: the radio does not
     * advertise OT_RADIO_CAPS_RX_ON_WHEN_IDLE, so OpenThread never calls it);
     * a flag read */
    if (((hal & IEEE802154_HAL_PDS_BUSY_RX_ON) != 0U) && otThreadGetLinkMode(s_instance).mRxOnWhenIdle) {
        /* a MED or a router listens all the time: WFI only */
        s_st.busy_rx_idle++;
        return PM_PART_BUSY;
    }
    radio = ot_radio_pm_busy();
    if ((hal | radio) != 0U) {
        ot_pm_count_bits(hal, s_st.why_hal, 8U);
        ot_pm_count_bits(radio, s_st.why_radio, 4U);
        s_st.retry++;
        return PM_PART_RETRY;
    }
    if (ot_system_event_var != OT_SYSTEM_EVENT_NONE) {
        /* the OT task has work it has not taken yet */
        s_st.why_events++;
        s_st.retry++;
        return PM_PART_RETRY;
    }
    s_st.ready++;
    return PM_PART_READY;
}

static uint32_t ot_pm_deadline_hus(void)
{
    uint32_t dl;
    int32_t rem;

    if (!ot_alarm_micro_deadline(&dl)) {
        return PM_DEADLINE_NONE;
    }
    rem = (int32_t)(dl - ot_pm_now_us()) - (int32_t)(CONFIG_IPRO_PM_THREAD_WAKE_LEAD_US + s_restore_lead_us);
    if (rem <= 0) {
        return 0U;
    }
    if (((uint32_t)rem < s_st.deadline_min_us) || (s_st.deadline_min_us == 0U)) {
        s_st.deadline_min_us = (uint32_t)rem;
    }
    return ((uint32_t)rem >= (PM_DEADLINE_NONE / 2U)) ? (PM_DEADLINE_NONE - 1U) : ((uint32_t)rem * 2U);
}

/* Radio states that end without an interrupt of their own - the FP window
 * (120 ms), a frame or an Enh-ACK under way, a MAC interrupt not taken yet -
 * keep the radio RETRY; the WFI in their place stops this soon, so the
 * window is offered again once they have passed rather than at the next RTOS
 * deadline.  Everything else ends on TIMER0, the MAC or the OT task. */
#define OT_PM_RETRY_WFI_HUS (2U * 10000U)
#define OT_PM_HAL_TIME_BOUNDED                                                                                  \
    (IEEE802154_HAL_PDS_BUSY_FP_WINDOW | IEEE802154_HAL_PDS_BUSY_RX_FRAME | IEEE802154_HAL_PDS_BUSY_ACK_WINDOW | \
     IEEE802154_HAL_PDS_BUSY_IRQ | IEEE802154_HAL_PDS_BUSY_MAC_HW)

static uint32_t ot_pm_wfi_deadline_hus(void)
{
    if ((s_instance == NULL) || !ot_radio_pm_enabled()) {
        return PM_DEADLINE_NONE;
    }
    return ((ieee802154_hal_pds_busy() & OT_PM_HAL_TIME_BOUNDED) != 0U) ? OT_PM_RETRY_WFI_HUS : PM_DEADLINE_NONE;
}

static const pm_participant_t s_participant = {
    .name = "thread",
    .projected_idle_ticks = NULL,
    .wfi_deadline_hus = ot_pm_wfi_deadline_hus,
    .pds_state = ot_pm_state,
    .pds_deadline_hus = ot_pm_deadline_hus,
    .offer = NULL,
    .window_skipped = NULL,
};

/* ---- device callbacks (executor context, IRQs masked) ---- */
static int ot_pm_suspend(void *ctx)
{
    (void)ctx;
    /* the window was offered a moment ago; a frame may have started since */
    if ((ieee802154_hal_pds_busy() | ot_radio_pm_busy()) != 0U) {
        s_st.suspend_vetoes++;
        return -1;
    }
    ot_alarm_micro_suspend();
    ieee802154_hal_pds_suspend();
    s_st.suspends++;
    return 0;
}

static void ot_pm_resume_early(void *ctx)
{
    const pm_txn_t *txn = pm_txn_current();

    (void)ctx;
    if (txn->slept) {
        uint32_t t0 = ot_pm_now_us();
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
        if (txn->executor == PM_EXEC_BLE) {
            s_align_wake_us = t0;
            s_align_pending = true;
        }
#endif
        uint32_t dt;
        int mac;

        s_st.resumes_slept++;
        mac = ieee802154_hal_pds_resume();
#if defined(CONFIG_COEX_BZ)
        /* the arbiter enable lives in GLB, outside the MAC restore */
        (void)coex_bz_resume();
#endif
        if (mac == IEEE802154_HAL_PDS_LOST) {
            /* the register restore did not take: MAC reset from the OT task,
             * the RF left alone when the BLE controller ran the PDS (its
             * policy keeps the RF, and BLE must not see an rf_init) */
            s_st.mac_lost++;
            if (txn->executor == PM_EXEC_IDLE) {
                (void)ot_radio_reinit_for_lp_wake();
            } else {
                (void)ot_radio_reinit_for_lp_wake_keep_rf();
            }
        } else {
            if (mac == IEEE802154_HAL_PDS_RESTORED) {
                s_st.mac_restored++;
            }
#if defined(CONFIG_IPRO_PM_THREAD_RF_RESTORE)
            /* the RF powered down or the BZ PHY reset by the PDS (idle
             * executor without KEEP_RF / KEEP_BZ) is warmed up and
             * reconfigured from the OT task; the BLE controller's policy
             * keeps both, and BLE must not see an rf_init */
            if ((txn->executor == PM_EXEC_IDLE) &&
                ((txn->domain_policy & (PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_RF | PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ)) !=
                 (PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_RF | PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ))) {
                s_st.rf_requests++;
                ot_radio_pm_request_rf_restore();
            }
#endif
        }
        dt = ot_pm_now_us() - t0;
        s_st.restore_n++;
        s_st.restore_sum_us += dt;
        if (dt > s_st.restore_max_us) {
            s_st.restore_max_us = dt;
        }
        if (dt > s_restore_lead_us) {
            s_restore_lead_us = dt;
        }
    }
    ot_alarm_micro_resume();
}

/* Timer task: have the OT task run its restores now rather than at the next
 * radio event (the CSL window itself). */
static void ot_pm_resume_late(void *ctx)
{
    (void)ctx;
    if (((ot_radio_pm_busy() & OT_RADIO_PM_BUSY_RESTORE) != 0U)
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
        || s_align_pending
#endif
    ) {
        OT_NOTIFY(OT_SYSTEM_EVENT_PM_RESUMED);
    }
}

/* ---------------------------------------------------------------------------
 * Lining a SED's data polls up with the BLE link (CONFIG_IPRO_PM_THREAD_ALIGN)
 *
 * The BLE controller runs one PDS per connection interval and wakes ahead of
 * its anchor anyway; a data poll due in the middle of the interval cuts that
 * PDS short (the rest of the interval is spent at WFI).  So, with the link in
 * the PDS profile, the poll goes out at a BLE wake: every k-th wake, k =
 * round(poll period / interval), and OpenThread's own poll timer is pushed
 * half an interval past that as the fallback.  The poll period the
 * application set comes back when the link goes (or it sets another one).
 * -------------------------------------------------------------------------*/
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
/* the last data poll sent (OpenThread's own counter, sampled every OT task
 * pass - the task runs right after each TX done) */
static uint32_t s_poll_count;
static uint32_t s_last_poll_us;
static bool s_last_poll_valid;

static void ot_pm_note_polls(void)
{
    uint32_t n = otLinkGetCounters(s_instance)->mTxDataPoll;

    if (n != s_poll_count) {
        s_poll_count = n;
        s_last_poll_us = ot_pm_now_us();
        s_last_poll_valid = true;
    }
}

/* `ot_pm align on|off` (bench A/B) */
static bool s_align_enabled = true;

static struct {
    bool active;
    uint32_t base_ms;     /* the application's poll period */
    uint32_t fallback_ms; /* what we set in its place */
    uint32_t polls;       /* polls sent at a BLE wake */
    uint32_t starts;
    uint32_t csl_base_us; /* the application's CSL period (0: not changed) */
    uint32_t csl_set_us;  /* what we set in its place */
    uint32_t csl_shifts;  /* sample phase moved onto the BLE wake */
    int32_t csl_last_err_us;
} s_align;

#if defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE)
/* sample this long after the BLE connection event ended: the receive window
 * opens up to a few ms ahead of it and must not overlap the event */
#ifndef CONFIG_IPRO_PM_THREAD_ALIGN_CSL_OFFSET_US
#define CONFIG_IPRO_PM_THREAD_ALIGN_CSL_OFFSET_US 6000
#endif
/* phase error left alone (the parent tracks the drift itself) */
#define OT_PM_ALIGN_CSL_TOL_US 3000
/* OpenThread vendor extension (core/api/link_api.cpp) */
extern void otLinkCslShiftSampleTime(otInstance *aInstance, int32_t aDeltaUs);

static void ot_pm_align_csl_restore(void)
{
    if ((s_align.csl_base_us != 0U) && (otLinkGetCslPeriod(s_instance) == s_align.csl_set_us)) {
        (void)otLinkSetCslPeriod(s_instance, s_align.csl_base_us);
    }
    s_align.csl_base_us = 0U;
}

/* An SSED: the CSL period an integer fraction of the connection interval
 * (n = round(interval / period), 160 us units) so the phase holds, and a
 * sample right after the BLE wake; the parent learns a moved phase from the
 * data poll that follows.  OT task, OT lock held. */
static void ot_pm_align_ssed(uint32_t ci_us, uint32_t csl_us)
{
    uint32_t n;
    uint32_t want_us;
    uint32_t next;
    uint32_t e;
    int32_t err;

    if (s_align.csl_base_us != 0U) {
        if (csl_us != s_align.csl_set_us) {
            s_align.csl_base_us = 0U; /* the application set another period */
        } else {
            csl_us = s_align.csl_base_us;
        }
    }
    n = (ci_us + csl_us / 2U) / csl_us;
    if (n == 0U) {
        n = 1U;
    }
    want_us = ((ci_us / n + 80U) / 160U) * 160U;
    if (otLinkGetCslPeriod(s_instance) != want_us) {
        if (s_align.csl_base_us == 0U) {
            s_align.csl_base_us = csl_us;
        }
        if (otLinkSetCslPeriod(s_instance, want_us) == OT_ERROR_NONE) {
            s_align.csl_set_us = want_us;
            s_align.starts++;
        }
        return; /* the phase restarts from now: line it up at the next wake */
    }
    /* right after the connection event (not after the PDS wake: the
     * controller spends that on its restore and the anchor); the BLE offer
     * that comes first waits for the window (PM_OFFER_RETRY), then the PDS
     * runs to the next interval */
    if (!ot_radio_csl_next_sample_us(&next) || (ipro_pm_ble_last_window_us() == 0U)) {
        return;
    }
    e = (next - (ipro_pm_ble_last_window_us() + CONFIG_IPRO_PM_THREAD_ALIGN_CSL_OFFSET_US)) % want_us;
    err = (e > want_us / 2U) ? -(int32_t)(want_us - e) : (int32_t)e;
    s_align.csl_last_err_us = err;
    if ((err > OT_PM_ALIGN_CSL_TOL_US) || (err < -OT_PM_ALIGN_CSL_TOL_US)) {
        otLinkCslShiftSampleTime(s_instance, -err);
        (void)otLinkSendDataRequest(s_instance);
        s_align.csl_shifts++;
    }
}
#endif

static void ot_pm_align_stop(void)
{
    if (s_align.active) {
        s_align.active = false;
        if (otLinkGetPollPeriod(s_instance) == s_align.fallback_ms) {
            (void)otLinkSetPollPeriod(s_instance, s_align.base_ms);
        }
    }
}

/* OT task, OT lock held */
static void ot_pm_align_sed(void)
{
    uint32_t ci_us = ipro_pm_ble_link_interval_us();
    uint32_t cur_ms = otLinkGetPollPeriod(s_instance);
    uint32_t base_ms;
    uint32_t k;

    if (!s_align_enabled || (ci_us == 0U) || otThreadGetLinkMode(s_instance).mRxOnWhenIdle ||
        (otThreadGetDeviceRole(s_instance) != OT_DEVICE_ROLE_CHILD)) {
        ot_pm_align_stop();
#if defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE)
        ot_pm_align_csl_restore();
#endif
        return;
    }
#if defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE)
    if (otLinkGetCslPeriod(s_instance) != 0U) {
        /* an SSED: the parent reaches it in its CSL windows, polls are rare */
        ot_pm_align_stop();
        ot_pm_align_ssed(ci_us, otLinkGetCslPeriod(s_instance));
        return;
    }
#endif
    if (s_align.active && (cur_ms != s_align.fallback_ms)) {
        s_align.active = false; /* the application set a new poll period */
    }
#if !defined(CONFIG_IPRO_PM_THREAD_ALIGN_SED)
    /* measured (SED 1 s poll, BLE 1005 ms, 2026-09-28): OpenThread's own poll
     * schedule already gave 71-75 % PDS; polls moved onto BLE wakes, with the
     * fallback timer firing between them, gave 34-37 % */
    ot_pm_align_stop();
    (void)cur_ms;
    return;
#endif
    base_ms = s_align.active ? s_align.base_ms : cur_ms;
    if (base_ms == 0U) {
        return; /* OpenThread's own (long, timeout-derived) period: leave it */
    }
    k = (base_ms * 1000U + ci_us / 2U) / ci_us;
    if (k == 0U) {
        k = 1U;
    }
    if (!s_align.active) {
        s_align.base_ms = base_ms;
        s_align.fallback_ms = (k * ci_us + ci_us / 2U) / 1000U;
        if (otLinkSetPollPeriod(s_instance, s_align.fallback_ms) != OT_ERROR_NONE) {
            return;
        }
        s_align.active = true;
        s_align.starts++;
    }
    /* poll now when waiting for the next wake would overshoot k intervals */
    if (!s_last_poll_valid || ((ot_pm_now_us() - s_last_poll_us) + ci_us / 2U >= k * ci_us)) {
        if (otLinkSendDataRequest(s_instance) == OT_ERROR_NONE) {
            s_align.polls++;
        }
    }
}
#endif

void ot_pm_process(void)
{
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
    if (s_instance != NULL) {
        ot_pm_note_polls();
    }
    if ((s_instance != NULL) && s_align_pending) {
        s_align_pending = false;
        ot_pm_align_sed();
    }
#endif
}

void ot_pm_init(otInstance *instance)
{
    static const ipro_pm_device_t dev = {
        .name = "thread",
        .suspend = ot_pm_suspend,
        .resume_early = ot_pm_resume_early,
        .resume_late = ot_pm_resume_late,
        .ctx = NULL,
    };

    if (s_instance != NULL) {
        return;
    }
    s_instance = instance;
    if ((ipro_pm_device_register(&dev) != 0) || (pm_participant_register(&s_participant) != 0)) {
        printf("[ot_pm] registration with ipro_pm failed\r\n");
    }
}

#if defined(CONFIG_SHELL)
#include "shell.h"

static const char *const s_hal_bits[8] = { "tx", "fp-win", "rx-frame", "rx-on", "mac-hw", "ack-win", "irq", "?" };
static const char *const s_radio_bits[4] = { "ot-tx", "rx-queue", "scan", "restore" };

static int cmd_ot_pm(int argc, char **argv)
{
    extern volatile uint32_t ieee802154_hal_pds_suspends;
    extern volatile uint32_t ieee802154_hal_pds_retained;
    extern volatile uint32_t ieee802154_hal_pds_lost;
    extern volatile uint32_t ot_radio_rf_restores;

#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
    if ((argc >= 3) && (strcmp(argv[1], "align") == 0)) {
        s_align_enabled = (strcmp(argv[2], "on") == 0);
        s_align_pending = true; /* on: from the next BLE wake; off: restores at once */
        OT_NOTIFY(OT_SYSTEM_EVENT_PM_RESUMED);
        printf("ot_pm align %s\r\n", s_align_enabled ? "on" : "off");
        return 0;
    }
#endif
    if ((argc >= 2) && (argv[1][0] == 'r')) {
        extern void ot_usalarm_dbg_reset(void);

        memset(&s_st, 0, sizeof(s_st));
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
        s_align.polls = 0U;
        s_align.starts = 0U;
        s_align.csl_shifts = 0U;
#endif
        ot_usalarm_dbg_reset();
        printf("ot_pm stats reset\r\n");
        return 0;
    }
#if defined(CONFIG_IPRO_PM_THREAD_ALIGN)
    printf("ot_pm align: %s k-poll base %lu ms fallback %lu ms | polls at a BLE wake %lu starts %lu | ble ci %lu us"
           " | csl base %lu set %lu us shifts %lu last err %ld us\r\n",
           s_align.active ? "on" : "off", (unsigned long)s_align.base_ms, (unsigned long)s_align.fallback_ms,
           (unsigned long)s_align.polls, (unsigned long)s_align.starts,
           (unsigned long)ipro_pm_ble_link_interval_us(), (unsigned long)s_align.csl_base_us,
           (unsigned long)s_align.csl_set_us, (unsigned long)s_align.csl_shifts, (long)s_align.csl_last_err_us);
#endif
    printf("ot_pm state: ready %lu retry %lu busy(rx-on-idle) %lu | suspends %lu vetoes %lu | slept %lu"
           " mac restored %lu lost %lu rf-restores %lu/%lu | deadline min %lu us\r\n",
           (unsigned long)s_st.ready, (unsigned long)s_st.retry, (unsigned long)s_st.busy_rx_idle,
           (unsigned long)s_st.suspends, (unsigned long)s_st.suspend_vetoes, (unsigned long)s_st.resumes_slept,
           (unsigned long)s_st.mac_restored, (unsigned long)s_st.mac_lost, (unsigned long)ot_radio_rf_restores, (unsigned long)s_st.rf_requests,
           (unsigned long)s_st.deadline_min_us);
    {
        extern void ot_usalarm_dbg_dump(void);
        uint32_t dl = 0U;
        bool running = ot_alarm_micro_deadline(&dl);

        printf("  us-alarm %s deadline %lu now %lu | ", running ? "running" : "stopped", (unsigned long)dl,
               (unsigned long)ot_pm_now_us());
        ot_usalarm_dbg_dump();
    }
    printf("  restore: n %lu avg %lu max %lu us (lead now %lu us)\r\n", (unsigned long)s_st.restore_n,
           (unsigned long)(s_st.restore_n ? (uint32_t)(s_st.restore_sum_us / s_st.restore_n) : 0U),
           (unsigned long)s_st.restore_max_us,
           (unsigned long)(CONFIG_IPRO_PM_THREAD_WAKE_LEAD_US + s_restore_lead_us));
    printf("  retry why:");
    for (uint32_t i = 0U; i < 7U; i++) {
        printf(" %s %lu", s_hal_bits[i], (unsigned long)s_st.why_hal[i]);
    }
    for (uint32_t i = 0U; i < 4U; i++) {
        printf(" %s %lu", s_radio_bits[i], (unsigned long)s_st.why_radio[i]);
    }
    printf(" events %lu\r\n", (unsigned long)s_st.why_events);
    {
        extern volatile uint32_t ieee802154_hal_rx_on_count;
        extern uint64_t ieee802154_hal_rx_on_time_us(void);

        printf("  rx on: %lu times, %lu ms total | uptime %lu ms\r\n", (unsigned long)ieee802154_hal_rx_on_count,
               (unsigned long)(ieee802154_hal_rx_on_time_us() / 1000U),
               (unsigned long)(otPlatTimeGet() / 1000U));
    }
    {
        extern volatile uint32_t ieee802154_hal_pds_lost_reg[3];
        extern volatile uint32_t ieee802154_hal_pds_restored;
        extern volatile uint32_t ieee802154_hal_pds_restore_us_max;
        extern volatile uint32_t ieee802154_hal_pds_hw_busy;
        extern volatile uint32_t ieee802154_hal_pds_hw_busy_regs[4];

        printf("  hal pds: suspends %lu retained %lu restored %lu (max %lu us) lost %lu"
               " (last: reg %03lx %08lx -> %08lx) | sym_cnt %lu\r\n",
               (unsigned long)ieee802154_hal_pds_suspends, (unsigned long)ieee802154_hal_pds_retained,
               (unsigned long)ieee802154_hal_pds_restored, (unsigned long)ieee802154_hal_pds_restore_us_max,
               (unsigned long)ieee802154_hal_pds_lost, (unsigned long)ieee802154_hal_pds_lost_reg[0],
               (unsigned long)ieee802154_hal_pds_lost_reg[1], (unsigned long)ieee802154_hal_pds_lost_reg[2],
               (unsigned long)ieee802154_hal_get_symbol_counter());
        printf("  mac hw busy vetoes %lu | stray receiver turned off %lu (last TX_0 %08lx TX_3 %08lx RX_00 %08lx"
               " RX_09 %08lx)\r\n",
               (unsigned long)ieee802154_hal_pds_hw_busy, (unsigned long)ieee802154_hal_rx_stray_off, (unsigned long)ieee802154_hal_pds_hw_busy_regs[0],
               (unsigned long)ieee802154_hal_pds_hw_busy_regs[1], (unsigned long)ieee802154_hal_pds_hw_busy_regs[2],
               (unsigned long)ieee802154_hal_pds_hw_busy_regs[3]);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ot_pm, ot_pm, OpenThread radio under ipro_pm: stats [reset]);
#endif
