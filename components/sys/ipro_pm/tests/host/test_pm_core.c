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

/* Host unit tests for ipro_pm core policy, holds, vetoes, devices and model.
 * Build and run: make -C components/sys/ipro_pm/tests/host */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ipro_pm.h"
#include "pm_internal.h"
#include "timers.h"

TickType_t host_tick;
uint64_t host_rtc;
uint64_t (*host_rtc_fn)(void);
static int s_failures;
static int s_pended;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                  \
            s_failures++;                                                           \
        }                                                                           \
    } while (0)

/* stubs for the port, backend and shell the core initialises */
void pm_port_init(void) {}
void pm_arbiter_init(void) {}
void pm_shell_init(void) {}
BaseType_t xTimerPendFunctionCall(PendedFunction_t fn, void *p1, uint32_t p2, TickType_t wait)
{
    (void)wait;
    s_pended++;
    fn(p1, p2);
    return pdPASS;
}

static void advance_ms(uint32_t ms)
{
    host_tick += ms;
    host_rtc += ((uint64_t)ms * 32768ULL) / 1000ULL;
}

static int s_evt_count[8];
static uint32_t s_evt_arg;
static void notifier(ipro_pm_event_t evt, uint32_t arg, void *ctx)
{
    (void)ctx;
    s_evt_count[evt]++;
    s_evt_arg = arg;
}

static bool s_veto_allow = true;
static bool veto(uint32_t sleep_us, void *ctx)
{
    (void)sleep_us;
    (void)ctx;
    return s_veto_allow;
}

static int s_calls[3][4]; /* [device][suspend, early, late] */
static int dev_suspend(void *ctx) { int i = (int)(long)ctx; s_calls[i][0]++; return (i == 2) ? -1 : 0; }
static void dev_early(void *ctx) { s_calls[(int)(long)ctx][1]++; }
static void dev_late(void *ctx) { s_calls[(int)(long)ctx][2]++; }

static void test_policy_and_holds(void)
{
    ipro_pm_reason_t r_wfi = ipro_pm_reason_register("radio", IPRO_PM_MODE_WFI);
    ipro_pm_reason_t r_act = ipro_pm_reason_register("audio", IPRO_PM_MODE_ACTIVE);

    CHECK(ipro_pm_policy_get() == IPRO_PM_MODE_WFI); /* CONFIG_IPRO_PM_MAX_MODE=1 */
    ipro_pm_policy_set(IPRO_PM_MODE_PDS);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);

    ipro_pm_hold(r_wfi);
    ipro_pm_hold(r_wfi);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    ipro_pm_release(r_wfi);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI); /* still one hold */
    ipro_pm_release(r_wfi);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
    ipro_pm_release(r_wfi); /* extra release is harmless */
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);

    ipro_pm_hold_isr(r_act);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_ACTIVE);
    ipro_pm_release_isr(r_act);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);

    ipro_pm_hold_for_ms(r_wfi, 50);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    advance_ms(49);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    advance_ms(1);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
    ipro_pm_hold_for_ms(r_wfi, 100);
    ipro_pm_hold_for_ms(r_wfi, 10); /* shorter request does not cut the longer one */
    advance_ms(50);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    advance_ms(50);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);

    CHECK(ipro_pm_reason_register("bad", IPRO_PM_MODE_COUNT) < 0);
    ipro_pm_hold(99); /* invalid handle ignored */
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
}

/* An expired timed hold must stay expired: the half-range tick compare
 * would read it as pending again 2^31 ticks later (24.8 days at 1 kHz). */
static void test_timed_hold_wrap(void)
{
    ipro_pm_reason_t r = ipro_pm_reason_register("burst", IPRO_PM_MODE_WFI);

    ipro_pm_policy_set(IPRO_PM_MODE_PDS);
    ipro_pm_hold_for_ms(r, 10);
    advance_ms(10);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
    advance_ms(0x80000000UL);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
    advance_ms(0x7ffffff0UL);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);

    /* expired (the idle hook observes it) long before a new request */
    ipro_pm_hold_for_ms(r, 10);
    advance_ms(10);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
    advance_ms(0x90000000UL);
    ipro_pm_hold_for_ms(r, 20);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    advance_ms(20);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
}

static void test_auto_ceiling(void)
{
    CHECK(ipro_pm_notifier_register(notifier, NULL) == 0);
    pm_core_auto_max_set(IPRO_PM_MODE_WFI);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_WFI);
    CHECK(s_evt_count[IPRO_PM_EVT_DEMOTED] == 1);
    CHECK(s_evt_arg == IPRO_PM_MODE_WFI);
    pm_core_auto_max_set(IPRO_PM_MODE_WFI); /* no change, no event */
    CHECK(s_evt_count[IPRO_PM_EVT_DEMOTED] == 1);
    pm_core_auto_max_set(IPRO_PM_MODE_PDS);
    CHECK(s_evt_count[IPRO_PM_EVT_PROMOTED] == 1);
    CHECK(ipro_pm_allowed_mode() == IPRO_PM_MODE_PDS);
}

static void test_vetoes(void)
{
    CHECK(pm_core_vetoes_allow(20000));
    CHECK(ipro_pm_veto_register(veto, NULL) == 0);
    s_veto_allow = false;
    CHECK(!pm_core_vetoes_allow(20000));
    s_veto_allow = true;
    CHECK(pm_core_vetoes_allow(20000));
    CHECK(ipro_pm_veto_register(NULL, NULL) < 0);
}

static void test_devices(void)
{
    ipro_pm_device_t d0 = { "d0", dev_suspend, dev_early, dev_late, (void *)0L };
    ipro_pm_device_t d1 = { "d1", dev_suspend, dev_early, dev_late, (void *)1L };

    CHECK(ipro_pm_device_register(&d0) == 0);
    CHECK(ipro_pm_device_register(&d1) == 0);
    CHECK(pm_core_devices_suspend());
    CHECK((s_calls[0][0] == 1) && (s_calls[1][0] == 1));
    pm_core_devices_resume_early();
    CHECK((s_calls[0][1] == 1) && (s_calls[1][1] == 1));
    pm_core_devices_resume_late_defer();
    CHECK((s_pended == 1) && (s_calls[0][2] == 1) && (s_calls[1][2] == 1));

    /* a device that vetoes: the ones already suspended are resumed */
    ipro_pm_device_t d2 = { "d2", dev_suspend, dev_early, NULL, (void *)2L };
    CHECK(ipro_pm_device_register(&d2) == 0);
    CHECK(!pm_core_devices_suspend());
    CHECK((s_calls[0][1] == 2) && (s_calls[1][1] == 2));
    CHECK(s_calls[2][1] == 0);
}

static void test_stats_and_model(void)
{
    ipro_pm_stats_t st;

    ipro_pm_stats_reset();
    advance_ms(1000);
    pm_core_account(IPRO_PM_MODE_WFI, 500000);
    pm_core_account(IPRO_PM_MODE_PDS, 250000);
    pm_core_pds_failed(3);
    pm_core_pds_rejected();
    ipro_pm_stats_get(&st);
    CHECK(st.window_us >= 999000 && st.window_us <= 1001000);
    CHECK(st.residency_us[IPRO_PM_MODE_WFI] == 500000);
    CHECK(st.residency_us[IPRO_PM_MODE_PDS] == 250000);
    CHECK(st.residency_us[IPRO_PM_MODE_ACTIVE] + 750000 == st.window_us);
    CHECK(st.entries[IPRO_PM_MODE_PDS] == 1);
    CHECK(st.pds_failures == 1 && st.pds_rejects == 1);
    CHECK(s_evt_count[IPRO_PM_EVT_PDS_FAILED] == 1);
    /* model: 0.25 s * 3500 + 0.5 s * 600 + 0.25 s * 80 + 1 cycle * 34.2 uA*s over 1 s */
    {
        uint32_t expect = (uint32_t)((st.residency_us[0] * 3500ULL + 500000ULL * 600ULL + 250000ULL * 80ULL +
                                      34200ULL * 1000ULL) / st.window_us);
        CHECK(st.est_avg_ua == expect);
        CHECK(st.est_avg_ua > 1200 && st.est_avg_ua < 1300);
    }
}

int main(void)
{
    host_rtc = 12345;
    CHECK(ipro_pm_init() == 0);
    CHECK(ipro_pm_init() == 0); /* idempotent */
    test_policy_and_holds();
    test_timed_hold_wrap();
    test_auto_ceiling();
    test_vetoes();
    test_devices();
    test_stats_and_model();
    if (s_failures != 0) {
        printf("%d check(s) failed\n", s_failures);
        return 1;
    }
    printf("ipro_pm host tests: all passed\n");
    return 0;
}
