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
 * @file ipro_pm.h
 * @brief IPRO power management: automatic idle-mode selection for FreeRTOS.
 *
 * The framework owns the FreeRTOS tickless-idle hooks and picks, for every
 * idle window, the deepest sleep mode that the window, the application and
 * the radio allow:
 *
 *   IPRO_PM_MODE_ACTIVE  - never sleep (busy idle)
 *   IPRO_PM_MODE_WFI     - core clock gated, every peripheral and clock live
 *   IPRO_PM_MODE_NATIVE  - WFI while the BLE core sleeps on its own 32 kHz timer
 *   IPRO_PM_MODE_PDS     - platform PDS31 (connected BLE kept alive by the
 *                          controller's snapshot/restore)
 *
 * Applications do not call a "sleep" function.  They describe what must not
 * happen right now - a reason that limits the depth, a veto for one window,
 * a device that needs suspend/resume around PDS - and the idle task does the
 * rest.  Every call here is safe from any task; the functions marked _isr
 * are also safe from interrupt context.
 */
#ifndef IPRO_PM_H_
#define IPRO_PM_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Sleep depth, ordered from shallowest to deepest. */
typedef enum {
    IPRO_PM_MODE_ACTIVE = 0,
    IPRO_PM_MODE_WFI,
    IPRO_PM_MODE_NATIVE,
    IPRO_PM_MODE_PDS,
    IPRO_PM_MODE_COUNT
} ipro_pm_mode_t;

/** Handle of a registered hold reason (negative on registration failure). */
typedef int ipro_pm_reason_t;

/** Events reported to notifiers. */
typedef enum {
    IPRO_PM_EVT_PDS_ENTER = 0, /**< about to enter PDS (executor context, IRQs masked, must not block); arg = sleep us */
    IPRO_PM_EVT_PDS_EXIT,      /**< PDS returned (executor context, IRQs masked, must not block); arg = elapsed us */
    IPRO_PM_EVT_PDS_FAILED,    /**< a PDS attempt failed or its wake did not recover; arg = reason */
    IPRO_PM_EVT_DEMOTED,       /**< the framework lowered the automatic depth; arg = new ipro_pm_mode_t */
    IPRO_PM_EVT_PROMOTED,      /**< the framework raised it again; arg = new ipro_pm_mode_t */
    IPRO_PM_EVT_PDS_ABORTED,   /**< a PDS entry never powered down (nothing lost); arg = HAL error */
} ipro_pm_event_t;

typedef void (*ipro_pm_notifier_t)(ipro_pm_event_t evt, uint32_t arg, void *ctx);

/**
 * Veto for one PDS window.  Called with interrupts masked just before a PDS
 * grant; must be short and lock-free.  Return false to keep this window at
 * WFI/native depth.
 */
typedef bool (*ipro_pm_veto_t)(uint32_t sleep_us, void *ctx);

/** Device callbacks around platform PDS (all optional).
 *
 *  They run in the context of whoever executes the PDS: the BLE controller
 *  task (connected BLE), or the idle task (ipro_pm's own executor, e.g. for
 *  Thread).  pm_txn_current() (ipro_pm_radio.h) describes the transaction. */
typedef struct {
    const char *name;
    /** Before PDS, at the last point before the sleep commits (IRQs masked,
     *  must not block).  Return non-zero to veto this PDS window; devices
     *  already suspended are then resumed. */
    int (*suspend)(void *ctx);
    /** Right after PDS returns, IRQs masked: re-init registers.  Also called
     *  when a suspended window is abandoned before PDS. */
    void (*resume_early)(void *ctx);
    /** After the system is back, from the timer task: anything that may block.
     *  Called only for devices that went through resume_early. */
    void (*resume_late)(void *ctx);
    void *ctx;
} ipro_pm_device_t;

/** Residency and event statistics since boot or the last ipro_pm_stats_reset(). */
typedef struct {
    uint64_t residency_us[IPRO_PM_MODE_COUNT]; /**< time spent per mode */
    uint64_t window_us;                        /**< wall time covered by the counters */
    uint32_t entries[IPRO_PM_MODE_COUNT];      /**< idle windows entered per mode */
    uint32_t pds_failures;                     /**< PDS attempts that failed or did not recover */
    uint32_t pds_rejects;                      /**< PDS windows refused by a gate, veto or device */
    uint32_t pds_aborts;                       /**< PDS entries that never powered down */
    uint32_t demotions;                        /**< automatic depth reductions */
    uint32_t est_avg_ua;                       /**< model estimate of the average current (uA) */
    ipro_pm_mode_t auto_max;                   /**< current automatic ceiling */
} ipro_pm_stats_t;

/** Initialise the framework.  Called from ipro_platform_init() when CONFIG_IPRO_PM=y. */
int ipro_pm_init(void);

/**
 * Register a hold reason.  While at least one hold on it is active, idle never
 * goes deeper than @p max_mode.  @p name must stay valid for the program's life.
 */
ipro_pm_reason_t ipro_pm_reason_register(const char *name, ipro_pm_mode_t max_mode);
void ipro_pm_hold(ipro_pm_reason_t reason);
void ipro_pm_release(ipro_pm_reason_t reason);
void ipro_pm_hold_isr(ipro_pm_reason_t reason);
void ipro_pm_release_isr(ipro_pm_reason_t reason);
/** Hold for @p ms from now (re-arming extends it); released by the framework. */
void ipro_pm_hold_for_ms(ipro_pm_reason_t reason, uint32_t ms);
void ipro_pm_hold_for_ms_isr(ipro_pm_reason_t reason, uint32_t ms);

int ipro_pm_veto_register(ipro_pm_veto_t veto, void *ctx);
int ipro_pm_device_register(const ipro_pm_device_t *dev);
int ipro_pm_notifier_register(ipro_pm_notifier_t cb, void *ctx);

/** Application ceiling for the automatic selection (default: CONFIG_IPRO_PM_MAX_MODE). */
void ipro_pm_policy_set(ipro_pm_mode_t max_mode);
ipro_pm_mode_t ipro_pm_policy_get(void);

/** Deepest mode currently allowed by policy, holds and the automatic ceiling. */
ipro_pm_mode_t ipro_pm_allowed_mode(void);

void ipro_pm_stats_get(ipro_pm_stats_t *out);
void ipro_pm_stats_reset(void);

/** Microseconds since ipro_pm_init() on the always-on 32 kHz clock (survives PDS). */
uint64_t ipro_pm_uptime_us(void);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_PM_H_ */
