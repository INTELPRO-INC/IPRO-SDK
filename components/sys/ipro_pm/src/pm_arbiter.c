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
 * @file pm_arbiter.c
 * @brief ipro_pm arbiter: which radio participant, if any, executes the PDS
 *        of an idle window.
 *
 * Per window (IRQs masked, scheduler suspended):
 *   - two executors at once: refused (counted, should never happen);
 *   - a busy participant: no PDS (REJECTED when an executor had the window,
 *     NONE otherwise, so a plain WFI window is not counted as a reject);
 *   - a participant with work in flight: RETRY, nothing is offered, so an
 *     executor's window survives until the next pass;
 *   - an executor: offered, with the earliest deadline of the others;
 *   - otherwise the idle task executes (CONFIG_IPRO_PM_IDLE_PDS), never on a
 *     projected window (that budget belongs to the executor's own poll).
 *
 * With only the BLE participant registered this is exactly the previous
 * single-backend behaviour: offer while the controller is in the connected
 * PDS profile, nothing otherwise.
 */
#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "ipro_pm_radio.h"
#include "pm_internal.h"

#define PM_MAX_PARTICIPANTS 4U
/* half-microseconds per RTOS tick */
#define PM_HUS_PER_TICK (2000UL * (uint32_t)portTICK_PERIOD_MS)

static const pm_participant_t *s_parts[PM_MAX_PARTICIPANTS];
static volatile uint32_t s_nb_parts;

volatile uint32_t ipro_pm_arb_conflicts;
volatile uint32_t ipro_pm_arb_busy;
volatile uint32_t ipro_pm_arb_retry;

void pm_arbiter_init(void)
{
}

int pm_participant_register(const pm_participant_t *p)
{
    int rc = -1;

    if ((p == NULL) || (p->pds_state == NULL)) {
        return -1;
    }
    taskENTER_CRITICAL();
    for (uint32_t i = 0U; i < s_nb_parts; i++) {
        if (s_parts[i] == p) {
            taskEXIT_CRITICAL();
            return 0; /* already registered */
        }
    }
    if (s_nb_parts < PM_MAX_PARTICIPANTS) {
        s_parts[s_nb_parts] = p;
        /* readers take the count first, with interrupts masked */
        __asm__ volatile("" ::: "memory");
        s_nb_parts++;
        rc = 0;
    }
    taskEXIT_CRITICAL();
    return rc;
}

uint32_t pm_arbiter_count(void)
{
    return s_nb_parts;
}

const pm_participant_t *pm_arbiter_get(uint32_t i)
{
    return (i < s_nb_parts) ? s_parts[i] : NULL;
}

TickType_t pm_arbiter_projected_idle_ticks(void)
{
    TickType_t best = 0U;
    uint32_t n = s_nb_parts;

    for (uint32_t i = 0U; i < n; i++) {
        TickType_t t;

        if (s_parts[i]->projected_idle_ticks == NULL) {
            continue;
        }
        t = s_parts[i]->projected_idle_ticks();
        if ((t != 0U) && ((best == 0U) || (t < best))) {
            best = t;
        }
    }
    return best;
}

uint32_t pm_arbiter_wfi_deadline_hus(void)
{
    uint32_t best = PM_DEADLINE_NONE;
    uint32_t n = s_nb_parts;

    for (uint32_t i = 0U; i < n; i++) {
        if (s_parts[i]->wfi_deadline_hus != NULL) {
            uint32_t d = s_parts[i]->wfi_deadline_hus();

            if (d < best) {
                best = d;
            }
        }
    }
    return best;
}

void pm_arbiter_window_skipped(void)
{
    uint32_t n = s_nb_parts;

    for (uint32_t i = 0U; i < n; i++) {
        if (s_parts[i]->window_skipped != NULL) {
            s_parts[i]->window_skipped();
        }
    }
}

pm_offer_t pm_arbiter_offer(TickType_t selected_ticks, bool projected)
{
    const pm_participant_t *exec = NULL;
    uint32_t deadline = PM_DEADLINE_NONE;
    uint32_t n = s_nb_parts;
    uint32_t ready = 0U;
    bool conflict = false;
    bool busy = false;
    bool retry = false;
    uint64_t rtos_hus64 = (uint64_t)selected_ticks * PM_HUS_PER_TICK;
    uint32_t rtos_hus = (rtos_hus64 > UINT32_MAX) ? UINT32_MAX : (uint32_t)rtos_hus64;

    for (uint32_t i = 0U; i < n; i++) {
        const pm_participant_t *p = s_parts[i];
        pm_part_state_t st = p->pds_state();

        switch (st) {
        case PM_PART_ABSENT:
            continue;
        case PM_PART_READY:
            ready++;
            break;
        case PM_PART_EXECUTOR:
            if ((exec != NULL) || (p->offer == NULL)) {
                conflict = true;
            } else {
                exec = p;
            }
            continue; /* its deadline is its own business */
        case PM_PART_RETRY:
            retry = true;
            break;
        case PM_PART_BUSY:
        default:
            busy = true;
            break;
        }
        if (p->pds_deadline_hus != NULL) {
            uint32_t d = p->pds_deadline_hus();

            if (d < deadline) {
                deadline = d;
            }
        }
    }
    if (conflict) {
        ipro_pm_arb_conflicts++;
        return PM_OFFER_REJECTED;
    }
    if (busy) {
        if (exec != NULL) {
            ipro_pm_arb_busy++;
            return PM_OFFER_REJECTED;
        }
        return PM_OFFER_NONE;
    }
    if (retry) {
        ipro_pm_arb_retry++;
        return PM_OFFER_RETRY;
    }
    if (exec != NULL) {
        return exec->offer(rtos_hus, deadline);
    }
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
    if (projected) {
        return PM_OFFER_REJECTED;
    }
#if !defined(CONFIG_IPRO_PM_IDLE_PDS_NO_RADIO)
    if (ready == 0U) {
#if defined(CONFIG_IPRO_PM_FAULT_INJECT)
        extern volatile uint32_t ipro_pm_idle_no_radio;

        if (ipro_pm_idle_no_radio == 0U)
#endif
        {
            return PM_OFFER_NONE;
        }
    }
#endif
    return pm_exec_idle_run(selected_ticks, deadline);
#else
    (void)projected;
    (void)ready;
    return PM_OFFER_NONE;
#endif
}

/* builds without the BLE participant (pm_ble_ipro7.c defines the real one) */
__attribute__((weak)) uint32_t ipro_pm_ble_link_interval_us(void)
{
    return 0U;
}

__attribute__((weak)) uint32_t ipro_pm_ble_last_window_us(void)
{
    return 0U;
}
