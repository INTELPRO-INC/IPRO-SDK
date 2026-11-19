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

#include "br_wifi_connect_operation.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stddef.h>
#include <fhost_wpa.h>
#include <net_al_ext.h>

static struct {
    uint32_t sequence, active, result;
    bool submitting, pending, ready, closed, disconnect;
} operation_state;

static enum { DISABLE_IDLE, DISABLE_PENDING, DISABLE_SUCCEEDED, DISABLE_FAILED } disable_state;

static void operation_disable_task(void *context)
{
    (void)context;
    /* The legacy WPA command ignores its timeout argument. Keep its request
     * lifetime in this worker; never abandon/free an outstanding eloop request. */
    const int result = wifi_mgmr_sta_autoconnect_disable();
    taskENTER_CRITICAL();
    disable_state = result == 0 ? DISABLE_SUCCEEDED : DISABLE_FAILED;
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

static bool operation_task_context(void)
{
    return !xPortIsInsideInterrupt() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
}

static void operation_complete(void *context, int vif, uint32_t result)
{
    /* Opaque integer cookie, never dereferenced; fits both 32/64-bit targets.
     * No pointer to caller storage survives submission. */
    const uint32_t id = (uint32_t)(uintptr_t)context;
    taskENTER_CRITICAL();
    if (vif == MGMR_VIF_STA && operation_state.active == id && operation_state.pending &&
        !operation_state.disconnect) {
        operation_state.pending = false;
        if (!operation_state.closed) {
            operation_state.result = result;
            operation_state.ready = true;
        }
    }
    taskEXIT_CRITICAL();
}

int br_wifi_connect_operation_start(const wifi_mgmr_sta_connect_params_t *config,
                                  uint32_t *operation)
{
    if (operation) *operation = 0;
    if (!config || !operation || !operation_task_context()) return -1;
    taskENTER_CRITICAL();
    if (operation_state.closed || operation_state.sequence == UINT32_MAX) {
        taskEXIT_CRITICAL();
        return -1;
    }
    if (operation_state.submitting || operation_state.pending || operation_state.ready) {
        taskEXIT_CRITICAL();
        return 1;
    }
    const uint32_t id = ++operation_state.sequence;
    operation_state.active = id;
    operation_state.disconnect = false;
    operation_state.submitting = operation_state.pending = true;
    taskEXIT_CRITICAL();

    const int error = wifi_mgmr_sta_connect_managed(config, operation_complete,
                                                           (void *)(uintptr_t)id);
    taskENTER_CRITICAL();
    operation_state.submitting = false;
    if (error) {
        operation_state.pending = operation_state.ready = false;
        operation_state.active = operation_state.result = 0;
    } else {
        *operation = id;
    }
    taskEXIT_CRITICAL();
    return error ? -1 : 0;
}

static int operation_take(uint32_t operation, uint32_t *result, bool disconnect)
{
    if (result) *result = 0;
    if (!operation || !result || !operation_task_context()) return -1;
    taskENTER_CRITICAL();
    if (operation_state.closed || operation_state.active != operation ||
        operation_state.disconnect != disconnect) {
        taskEXIT_CRITICAL();
        return -1;
    }
    if (operation_state.submitting || operation_state.pending) {
        taskEXIT_CRITICAL();
        return 1;
    }
    if (!operation_state.ready) {
        taskEXIT_CRITICAL();
        return -1;
    }
    *result = operation_state.result;
    operation_state.ready = false;
    operation_state.active = operation_state.result = 0;
    taskEXIT_CRITICAL();
    return 0;
}

int br_wifi_connect_operation_take(uint32_t operation, uint32_t *result)
{
    return operation_take(operation, result, false);
}

int br_wifi_disconnect_operation_take(uint32_t operation, uint32_t *result)
{
    return operation_take(operation, result, true);
}

static bool operation_closed(void)
{
    taskENTER_CRITICAL();
    const bool closed = operation_state.closed;
    taskEXIT_CRITICAL();
    return closed;
}

static void operation_disconnect_task(void *context)
{
    (void)context;
    uint32_t lease = 0;
    uint32_t result = 1;
    while (!operation_closed()) {
        const int busy = wifi_mgmr_sta_connect_pause(&lease);
        if (busy < 0) break;
        if (busy) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        /* Keep all potentially blocking WPA commands off the CHIP task. Do
         * not remove/reset the VIF: an unrelated scan may still own firmware
         * requests. This disables the network, not the interface or storage. */
        if (wifi_mgmr_sta_autoconnect_disable() == 0) {
            net_al_ext_sta_reconnect_suppress();
            if (fhost_wpa_get_state(MGMR_VIF_STA) == FHOST_WPA_STATE_STOPPED ||
                fhost_wpa_disable_network(MGMR_VIF_STA) == 0)
                result = 0;
        }
        break;
    }
    /* A lease belongs to this worker until release. Never publish completion
     * while a pause is stranded; reset must also wait for this cleanup. */
    while (lease && wifi_mgmr_sta_connect_resume(lease) != 0)
        vTaskDelay(pdMS_TO_TICKS(100));

    taskENTER_CRITICAL();
    operation_state.pending = false;
    if (!operation_state.closed) {
        operation_state.result = result;
        operation_state.ready = true;
    }
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

int br_wifi_disconnect_operation_start(uint32_t *operation)
{
    if (operation) *operation = 0;
    if (!operation || !operation_task_context()) return -1;
    taskENTER_CRITICAL();
    if (operation_state.closed || operation_state.sequence == UINT32_MAX) {
        taskEXIT_CRITICAL();
        return -1;
    }
    if (operation_state.submitting || operation_state.pending || operation_state.ready) {
        taskEXIT_CRITICAL();
        return 1;
    }
    const uint32_t id = ++operation_state.sequence;
    operation_state.active = id;
    operation_state.disconnect = true;
    operation_state.submitting = operation_state.pending = true;
    taskEXIT_CRITICAL();
    const int created = xTaskCreate(operation_disconnect_task, "br_wifi_disconnect", 1024,
                                    NULL, tskIDLE_PRIORITY + 2, NULL);
    taskENTER_CRITICAL();
    operation_state.submitting = false;
    if (created != pdPASS) {
        operation_state.pending = operation_state.ready = false;
        operation_state.active = operation_state.result = 0;
    } else {
        *operation = id;
    }
    taskEXIT_CRITICAL();
    return created == pdPASS ? 0 : -1;
}

int br_wifi_connect_operation_close(void)
{
    if (!operation_task_context()) return -1;
    taskENTER_CRITICAL();
    operation_state.closed = true;
    operation_state.ready = false;
    operation_state.result = 0;
    const bool busy = operation_state.submitting || operation_state.pending;
    taskEXIT_CRITICAL();
    return busy ? 1 : 0;
}

int br_wifi_connect_operation_drain(void)
{
    const int mailbox = br_wifi_connect_operation_close();
    if (mailbox < 0) return -1;
    const int worker = wifi_mgmr_sta_connect_close();
    if (mailbox || worker) return 1;
    /* Manager enable admission is permanently closed. A dedicated worker owns
     * the possibly unbounded legacy command; the reset caller only polls. */
    taskENTER_CRITICAL();
    if (disable_state == DISABLE_SUCCEEDED) {
        taskEXIT_CRITICAL();
        return 0;
    }
    if (disable_state == DISABLE_PENDING) {
        taskEXIT_CRITICAL();
        return 1;
    }
    if (disable_state == DISABLE_FAILED) {
        disable_state = DISABLE_IDLE;
        taskEXIT_CRITICAL();
        return -1;
    }
    disable_state = DISABLE_PENDING;
    taskEXIT_CRITICAL();
    if (xTaskCreate(operation_disable_task, "br_wifi_disable", 1024, NULL,
                    tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        taskENTER_CRITICAL();
        disable_state = DISABLE_IDLE;
        taskEXIT_CRITICAL();
        return -1;
    }
    return 1;
}
