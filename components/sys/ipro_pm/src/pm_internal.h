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
 * @file pm_internal.h
 * @brief Interfaces between the ipro_pm core, the FreeRTOS port, the radio
 *        backend and the shell.  Not part of the public API.
 */
#ifndef IPRO_PM_INTERNAL_H_
#define IPRO_PM_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include <FreeRTOS.h>

#include "ipro_pm.h"
#include "ipro_pm_radio.h"

/* the idle executor needs the timebase (raw .config builds do not resolve
 * Kconfig select) */
#if defined(CONFIG_IPRO_PM_TIMEBASE) || defined(CONFIG_IPRO_PM_IDLE_PDS)
#define PM_HAVE_TIMEBASE 1
#endif

/* ---- core (pm_core.c) ---- */
typedef struct {
    const char *name;
    ipro_pm_mode_t max_mode;
    uint16_t count;
    bool timed;
} pm_reason_view_t;

uint64_t pm_core_rtc_to_us(uint64_t ticks);
void pm_core_account(ipro_pm_mode_t mode, uint64_t us);
void pm_core_pds_failed(uint32_t reason);
void pm_core_pds_rejected(void);
void pm_core_pds_aborted(int32_t rc);
void pm_core_notify(ipro_pm_event_t evt, uint32_t arg);
bool pm_core_vetoes_allow(uint32_t sleep_us);
bool pm_core_devices_suspend(void);
void pm_core_devices_resume_early(void);
void pm_core_devices_resume_late_defer(void);
void pm_core_auto_max_set(ipro_pm_mode_t mode);
ipro_pm_mode_t pm_core_auto_max_get(void);
uint32_t pm_core_reasons_dump(pm_reason_view_t *out, uint32_t max);

/* ---- FreeRTOS port (pm_freertos.c) ---- */
void pm_port_init(void);

/* ---- arbiter (pm_arbiter.c) ----
 *
 * Collects the registered radio participants (ipro_pm_radio.h) and decides,
 * per idle window, who executes a PDS.  Called from the idle task's tickless
 * hooks. */
void pm_arbiter_init(void);
/** Largest idle time hidden behind a participant's safety poll (0 if none). IRQs on. */
TickType_t pm_arbiter_projected_idle_ticks(void);
/** Earliest WFI deadline of all participants, half-microseconds (PM_DEADLINE_NONE: none). */
uint32_t pm_arbiter_wfi_deadline_hus(void);
/**
 * Offer this window for PDS, IRQs masked and the scheduler suspended.
 * @p selected_ticks is the RTOS budget the port selected; @p projected is set
 * when that budget came from a participant's projection (never slept on by
 * the idle executor).
 */
pm_offer_t pm_arbiter_offer(TickType_t selected_ticks, bool projected);
/** A projected window was not offered: forget per-window state. */
void pm_arbiter_window_skipped(void);
/** Number of registered participants and one of them, for the shell. */
uint32_t pm_arbiter_count(void);
const pm_participant_t *pm_arbiter_get(uint32_t i);

/* ---- transaction (pm_core.c) ---- */
/** Start a PDS transaction record (new generation), executor-side. */
pm_txn_t *pm_core_txn_begin(pm_exec_id_t exec, uint32_t sleep_us);

/* ---- 32 kHz clock and MTIME rebase (pm_lpclk.c) ---- */
typedef struct {
    uint64_t rtc;   /* HBN RTC, 45 bits */
    uint64_t mtime; /* machine timer counts */
} pm_clkpair_t;

void pm_lpclk_init(void);
/** (RTC, MTIME) taken on an RTC edge.  IRQs masked. */
void pm_lpclk_pair_now(pm_clkpair_t *out);
/** An awake span starts / ends (feeds the calibration). */
void pm_lpclk_span_start(const pm_clkpair_t *now);
void pm_lpclk_span_end(const pm_clkpair_t *now);
/** After a PDS that started at @p before: restart MTIME and write it forward
 *  by the RTC-measured sleep.  Returns the sleep, microseconds. */
uint32_t pm_lpclk_rebase(const pm_clkpair_t *before);
/* pure helpers */
uint64_t pm_lpclk_ticks_to_counts_ppm(uint64_t ticks, int32_t ppm);
uint64_t pm_lpclk_us_to_ticks_ppm(uint64_t us, int32_t ppm);
int32_t pm_lpclk_window_ppm(uint64_t rtc_ticks, uint64_t mtime_counts);
uint64_t pm_lpclk_rebase_target(const pm_clkpair_t *before, uint64_t after_rtc, uint64_t after_edge_mtime,
                                int32_t ppm);

/* ---- shell dumps ---- */
void pm_port_dump(void);
void pm_ble_dump(void);

/* ---- idle-task executor (pm_exec_idle.c) ---- */
#if defined(CONFIG_IPRO_PM_IDLE_PDS)
void pm_exec_idle_init(void);
pm_offer_t pm_exec_idle_run(TickType_t selected_ticks, uint32_t part_deadline_hus);
/** Task context (idle hook, scheduler running): ticks owed, backoff timer. */
void pm_exec_idle_housekeeping(void);
/** Task context, scheduler running (the BLE controller's native waits): bring
 *  the kernel to the MTIME epoch's wall ticks, owed ticks included. */
void pm_exec_idle_tick_task_sync(void);
/** Any context: the tick the kernel is heading for (the MTIME epoch's wall
 *  ticks), never behind @p kernel_now. */
TickType_t pm_exec_idle_wall_ticks(TickType_t kernel_now);
#endif
/**
 * Idle-executor sleep sizing (pure): the CPU must be back @p back_us from now;
 * the wake path takes @p wake_us, the 32 kHz clock may be @p bound_ppm off.
 * Returns the sleep, microseconds (0: nothing left).
 */
static inline uint32_t pm_exec_idle_plan(uint64_t back_us, uint32_t wake_us, uint32_t guard_us, uint32_t bound_ppm)
{
    uint64_t margin = (uint64_t)wake_us + guard_us + ((back_us * bound_ppm) / 1000000U);

    if (back_us <= margin) {
        return 0U;
    }
    back_us -= margin;
    return (back_us > UINT32_MAX) ? UINT32_MAX : (uint32_t)back_us;
}

/* ---- built-in participants ---- */
#if defined(IPRO_PM_HAS_BLE)
void pm_ble_init(void);
#endif

/**
 * BLE sleep sizing: the joint RTOS/radio window (half-microseconds) minus the
 * controller's wake reserve, shortened so the CPU is back @p exec_wake_us
 * before another participant's deadline (@p part_hus, PM_DEADLINE_NONE:
 * none).  Returns microseconds, 0 when nothing is left.
 */
static inline uint32_t pm_ble_size_sleep(uint32_t rtos_hus, uint32_t fresh_hus, uint32_t part_hus,
                                         uint32_t reserve_us, uint32_t exec_wake_us)
{
    uint32_t joint_hus = (rtos_hus < fresh_hus) ? rtos_hus : fresh_hus;
    uint32_t sleep_us = 0U;

    if ((joint_hus / 2U) > reserve_us) {
        sleep_us = (joint_hus / 2U) - reserve_us;
    }
    if (part_hus != PM_DEADLINE_NONE) {
        uint32_t cap_us = ((part_hus / 2U) > exec_wake_us) ? ((part_hus / 2U) - exec_wake_us) : 0U;

        if (cap_us < sleep_us) {
            sleep_us = cap_us;
        }
    }
    return sleep_us;
}

/* ---- shell (pm_shell.c) ---- */
void pm_shell_init(void);

#endif /* IPRO_PM_INTERNAL_H_ */
