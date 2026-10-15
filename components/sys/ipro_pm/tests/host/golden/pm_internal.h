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

/* ---- radio backend (pm_ble_ipro7.c, or the no-radio stub) ----
 *
 * The backend tells the port about radio deadlines and turns an idle window
 * into a controller-owned PDS transaction.  All functions except init run from
 * the idle task's tickless hooks, most of them with interrupts masked, so they
 * must be short and lock-free. */
void pm_backend_init(void);
/** Idle ticks hidden behind the radio driver's own short safety poll (0 if none). */
TickType_t pm_backend_projected_idle_ticks(void);
/** Radio deadline to clamp a WFI window to, in half-microseconds (UINT32_MAX: none). */
uint32_t pm_backend_wfi_deadline_hus(void);
/**
 * Offer this idle window (rtos_hus of RTOS idle time, in half-microseconds)
 * for PDS, IRQs masked.  GRANTED: the radio driver runs the PDS itself and the
 * port returns to the scheduler without WFI.  RETRY: the window stays valid
 * but not now; the port waits for the next interrupt and offers it again.
 */
typedef enum {
    PM_OFFER_REJECTED = 0,  /* window not usable: do not offer it again */
    PM_OFFER_GRANTED,       /* the radio driver runs the PDS */
    PM_OFFER_RETRY,         /* transient (e.g. HCI bytes in flight): retry later */
} pm_offer_t;
pm_offer_t pm_backend_offer_pds(uint32_t rtos_hus);
/** Forget per-window state after a window was not offered (projection rejected). */
void pm_backend_window_skipped(void);
/** Whether the radio driver is currently in a PDS-capable connected profile. */
bool pm_backend_pds_capable(void);

/* ---- shell (pm_shell.c) ---- */
void pm_shell_init(void);

#endif /* IPRO_PM_INTERNAL_H_ */
