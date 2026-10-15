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

/* Host unit tests for the ipro_pm arbiter and the BLE sleep sizing.
 * Build and run: make -C components/sys/ipro_pm/tests/host */
#include <stdio.h>
#include <string.h>

#include "pm_internal.h"

TickType_t host_tick;
static int s_failures;

#define CHECK(cond)                                                                 \
    do {                                                                            \
        if (!(cond)) {                                                              \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                  \
            s_failures++;                                                           \
        }                                                                           \
    } while (0)

/* Two fake radios: "ble" (may execute) and "thread" (participates). */
typedef struct {
    pm_part_state_t state;
    uint32_t deadline;
    TickType_t projected;
    uint32_t wfi_deadline;
    pm_offer_t offer_result;
    int offers;
    uint32_t last_rtos;
    uint32_t last_part;
    int skipped;
} fake_t;

static fake_t s_ble;
static fake_t s_thr;

static pm_part_state_t ble_state(void) { return s_ble.state; }
static uint32_t ble_deadline(void) { return s_ble.deadline; }
static TickType_t ble_projected(void) { return s_ble.projected; }
static uint32_t ble_wfi(void) { return s_ble.wfi_deadline; }
static void ble_skipped(void) { s_ble.skipped++; }
static pm_offer_t ble_offer(uint32_t rtos, uint32_t part)
{
    s_ble.offers++;
    s_ble.last_rtos = rtos;
    s_ble.last_part = part;
    return s_ble.offer_result;
}

static pm_part_state_t thr_state(void) { return s_thr.state; }
static uint32_t thr_deadline(void) { return s_thr.deadline; }
static uint32_t thr_wfi(void) { return s_thr.wfi_deadline; }
static void thr_skipped(void) { s_thr.skipped++; }

static const pm_participant_t s_ble_part = {
    .name = "ble",
    .projected_idle_ticks = ble_projected,
    .wfi_deadline_hus = ble_wfi,
    .pds_state = ble_state,
    .pds_deadline_hus = ble_deadline,
    .offer = ble_offer,
    .window_skipped = ble_skipped,
};

static const pm_participant_t s_thr_part = {
    .name = "thread",
    .projected_idle_ticks = NULL,
    .wfi_deadline_hus = thr_wfi,
    .pds_state = thr_state,
    .pds_deadline_hus = thr_deadline,
    .offer = NULL,
    .window_skipped = thr_skipped,
};

static void reset_fakes(void)
{
    memset(&s_ble, 0, sizeof(s_ble));
    memset(&s_thr, 0, sizeof(s_thr));
    s_ble.deadline = PM_DEADLINE_NONE;
    s_thr.deadline = PM_DEADLINE_NONE;
    s_ble.wfi_deadline = PM_DEADLINE_NONE;
    s_thr.wfi_deadline = PM_DEADLINE_NONE;
    s_ble.offer_result = PM_OFFER_GRANTED;
}

static void test_ble_only(void)
{
    reset_fakes();
    CHECK(pm_participant_register(&s_ble_part) == 0);
    CHECK(pm_participant_register(&s_ble_part) == 0); /* idempotent */
    CHECK(pm_arbiter_count() == 1U);

    s_ble.state = PM_PART_EXECUTOR;
    CHECK(pm_arbiter_offer(100U, true) == PM_OFFER_GRANTED);
    CHECK(s_ble.offers == 1);
    CHECK(s_ble.last_rtos == 200000U);          /* 100 ticks * 2000 hus */
    CHECK(s_ble.last_part == PM_DEADLINE_NONE); /* nobody else constrains it */

    s_ble.offer_result = PM_OFFER_RETRY;
    CHECK(pm_arbiter_offer(100U, true) == PM_OFFER_RETRY);
    s_ble.offer_result = PM_OFFER_REJECTED;
    CHECK(pm_arbiter_offer(100U, false) == PM_OFFER_REJECTED);

    /* controller up but not in the PDS profile, or not running: nothing offered */
    s_ble.offers = 0;
    s_ble.state = PM_PART_BUSY;
    CHECK(pm_arbiter_offer(100U, false) == PM_OFFER_NONE);
    s_ble.state = PM_PART_ABSENT;
    CHECK(pm_arbiter_offer(100U, false) == PM_OFFER_NONE);
    CHECK(s_ble.offers == 0);

    /* RTOS budget saturates */
    s_ble.state = PM_PART_EXECUTOR;
    s_ble.offer_result = PM_OFFER_GRANTED;
    CHECK(pm_arbiter_offer(0xFFFFFFF0U, true) == PM_OFFER_GRANTED);
    CHECK(s_ble.last_rtos == UINT32_MAX);
}

static void test_ble_and_thread(void)
{
    reset_fakes();
    CHECK(pm_participant_register(&s_thr_part) == 0);
    CHECK(pm_arbiter_count() == 2U);

    /* BLE executes, Thread ready with a CSL window 300 ms away */
    s_ble.state = PM_PART_EXECUTOR;
    s_thr.state = PM_PART_READY;
    s_thr.deadline = 600000U;
    CHECK(pm_arbiter_offer(1000U, true) == PM_OFFER_GRANTED);
    CHECK(s_ble.last_part == 600000U);

    /* Thread has a frame in flight: keep the window, BLE not asked */
    s_ble.offers = 0;
    s_thr.state = PM_PART_RETRY;
    CHECK(pm_arbiter_offer(1000U, true) == PM_OFFER_RETRY);
    CHECK(s_ble.offers == 0);

    /* Thread cannot sleep (rx-on-when-idle): the BLE window is dropped */
    s_thr.state = PM_PART_BUSY;
    CHECK(pm_arbiter_offer(1000U, true) == PM_OFFER_REJECTED);
    CHECK(s_ble.offers == 0);

    /* BLE advertising (busy), Thread ready: no executor for this build */
    s_ble.state = PM_PART_BUSY;
    s_thr.state = PM_PART_READY;
    CHECK(pm_arbiter_offer(1000U, false) == PM_OFFER_NONE);

    /* an absent Thread does not constrain BLE */
    s_ble.state = PM_PART_EXECUTOR;
    s_thr.state = PM_PART_ABSENT;
    s_thr.deadline = 1000U;
    CHECK(pm_arbiter_offer(1000U, true) == PM_OFFER_GRANTED);
    CHECK(s_ble.last_part == PM_DEADLINE_NONE);

    /* earliest WFI deadline and projection */
    s_ble.wfi_deadline = 90000U;
    s_thr.wfi_deadline = 40000U;
    CHECK(pm_arbiter_wfi_deadline_hus() == 40000U);
    s_ble.projected = 70U;
    CHECK(pm_arbiter_projected_idle_ticks() == 70U);

    /* window skipped reaches everyone */
    pm_arbiter_window_skipped();
    CHECK((s_ble.skipped == 1) && (s_thr.skipped == 1));
}

static const pm_participant_t s_exec2 = {
    .name = "exec2",
    .pds_state = ble_state, /* shares the BLE state: EXECUTOR together with it */
    .offer = ble_offer,
};

static void test_conflict_and_limits(void)
{
    reset_fakes();
    CHECK(pm_participant_register(&s_exec2) == 0);
    s_ble.state = PM_PART_EXECUTOR;
    s_thr.state = PM_PART_READY;
    CHECK(pm_arbiter_offer(100U, true) == PM_OFFER_REJECTED); /* two executors */
    CHECK(s_ble.offers == 0);
    CHECK(pm_participant_register(NULL) == -1);
    CHECK(pm_arbiter_get(99U) == NULL);
}

static void test_ble_sizing(void)
{
    /* no other deadline: the previous formula exactly */
    for (uint32_t rtos = 0U; rtos < 4000000U; rtos += 99991U) {
        for (uint32_t fresh = 0U; fresh < 4000000U; fresh += 123457U) {
            uint32_t reserve = 29000U;
            uint32_t joint = (rtos < fresh) ? rtos : fresh;
            uint32_t old = ((joint / 2U) > reserve) ? ((joint / 2U) - reserve) : 0U;

            CHECK(pm_ble_size_sleep(rtos, fresh, PM_DEADLINE_NONE, reserve, 12000U) == old);
        }
    }
    /* a Thread deadline 300 ms out caps a 1 s window at 300 - 12 ms */
    CHECK(pm_ble_size_sleep(2000000U, 2000000U, 600000U, 29000U, 12000U) == 288000U);
    /* a later deadline does not lengthen it */
    CHECK(pm_ble_size_sleep(200000U, 2000000U, 600000U, 29000U, 12000U) == 71000U);
    /* a deadline inside the wake budget leaves nothing */
    CHECK(pm_ble_size_sleep(2000000U, 2000000U, 20000U, 29000U, 12000U) == 0U);
}

int main(void)
{
    test_ble_only();
    test_ble_and_thread();
    test_conflict_and_limits();
    test_ble_sizing();
    if (s_failures != 0) {
        printf("ipro_pm arbiter tests: %d failure(s)\n", s_failures);
        return 1;
    }
    printf("ipro_pm arbiter tests: all passed\n");
    return 0;
}
