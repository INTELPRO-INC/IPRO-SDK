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
 * @file ipro_pm_ble.h
 * @brief ipro_pm BLE controller integration (CONFIG_IPRO_PM_BLE).
 *
 * An application brings the controller up with ipro_pm_ble_controller_init()
 * instead of ipro_ble_ctlr_controller_init().  After that the framework
 * follows every connection on its own (through the host's connection
 * callbacks) and puts it in the PDS profile when its parameters leave room
 * for a PDS cycle between connection events; otherwise the link stays on
 * WFI, which the idle hooks already clamp to the next radio event.
 */
#ifndef IPRO_PM_BLE_H_
#define IPRO_PM_BLE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Connection parameters, in the units of the LE Connection Update procedure. */
typedef struct {
    uint16_t interval_min; /**< 1.25 ms units */
    uint16_t interval_max; /**< 1.25 ms units */
    uint16_t latency;      /**< connection events */
    uint16_t timeout;      /**< 10 ms units */
} ipro_pm_ble_conn_param_t;

/**
 * Initialise the BLE controller with the platform-PDS policy of this board
 * and register the framework's connection tracking.  Replaces
 * ipro_ble_ctlr_controller_init(); returns its result.
 */
int ipro_pm_ble_controller_init(unsigned int task_priority);

/**
 * Connection parameters that give PDS room on this board while staying
 * inside the limits Apple accessories accept (interval 15 ms multiples,
 * interval * (latency + 1) <= 2 s, timeout > 3 * that and <= 6 s).
 * @p idle_ms is how often the application needs a connection event
 * (e.g. 1000 for a sensor that reports every second).
 */
void ipro_pm_ble_conn_param_hint(uint32_t idle_ms, ipro_pm_ble_conn_param_t *out);

/**
 * Ask the central of the tracked link for the parameters of
 * ipro_pm_ble_conn_param_hint(@p idle_ms) (L2CAP/LL connection parameter
 * update).  With CONFIG_IPRO_PM_BLE_IDLE_INTERVAL_MS set the framework does
 * this by itself CONFIG_IPRO_PM_BLE_IDLE_DELAY_MS after each connection;
 * call it again after a burst that asked for a short interval.
 * @return 0, -ENOTCONN without a link, or the host's error.
 */
int ipro_pm_ble_params_request(uint32_t idle_ms);

/**
 * Where the PDS windows of a connection went, counted since boot (monotonic:
 * subtract two reads for a window).  ipro_pm_stats_t.pds_rejects is
 * reject_budget + reject_veto + reject_controller; the other rejects also cost
 * a window but are not in it.
 */
typedef struct {
    uint32_t grants;            /**< windows granted to the controller */
    uint32_t cycles;            /**< grants the controller ran as a PDS cycle */
    uint32_t recovered;         /**< cycles whose first post-PDS event re-anchored the link */
    uint32_t failures;          /**< cycles that did not recover */
    uint32_t reject_budget;     /**< too short once the wake reserve is paid (e.g. an RTOS deadline inside it) */
    uint32_t reject_veto;       /**< refused by a veto (console UART, application) when offered */
    uint32_t reject_controller; /**< refused by the controller's grant */
    uint32_t reject_prepare;    /**< refused at the last moment by a hold, veto or device */
    uint32_t reject_stale;      /**< the offered window no longer matched the controller's */
    uint32_t retry_hci;         /**< offers deferred while HCI bytes were in flight (per idle pass) */
    uint32_t backoffs;          /**< demotions to WFI after repeated failures */
} ipro_pm_ble_diag_t;

/** Snapshot of the BLE backend counters. */
void ipro_pm_ble_diag_get(ipro_pm_ble_diag_t *out);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_PM_BLE_H_ */
