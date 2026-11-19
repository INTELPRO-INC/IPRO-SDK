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

#include <ipro_matter_wifi_scan.h>
#include <fhost_scan_result.h>
#include "wifi_mgmr_ext.h"
#include "cfgrwnx.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <string.h>

_Static_assert(sizeof(TickType_t) >= 4, "scan deadlines require 32-bit ticks");
static struct {
    uint32_t sequence, active, result;
    TickType_t started, budget;
    bool worker, pending, ready, delivered, closed;
    wifi_mgmr_scan_params_t config;
    struct cfgrwnx_scan command;
    ipro_wifi_scan_snapshot observations;
} scan;

static bool task_context(void)
{
    return !xPortIsInsideInterrupt() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
}
static bool expired(void) { return (TickType_t)(xTaskGetTickCount() - scan.started) >= scan.budget; }
/* All helpers touching state run inside a short critical section. */
static void publish(uint32_t result)
{
    if (!scan.closed && !scan.delivered && !scan.ready) {
        scan.result = expired() ? IPRO_WIFI_SCAN_TIMEOUT : result;
        scan.ready = true;
    }
}

void fhost_scan_request_observed(int vif, uint32_t id, const struct fhost_scan_observation *entry)
{
    if (!entry || entry->ssid_length > 32 || entry->channel < 1 || entry->channel > 14) return;
    taskENTER_CRITICAL();
    if (id && vif == MGMR_VIF_STA && scan.active == id && scan.pending &&
        !scan.closed && !scan.ready && !scan.delivered && !expired()) {
        unsigned i;
        for (i = 0; i < scan.observations.count; ++i)
            if (memcmp(scan.observations.entries[i].bssid, entry->bssid, 6) == 0) break;
        if (i < IPRO_WIFI_SCAN_CAPACITY) {
            ipro_wifi_scan_entry *out = &scan.observations.entries[i];
            memset(out, 0, sizeof(*out));
            memcpy(out->ssid, entry->ssid, entry->ssid_length);
            memcpy(out->bssid, entry->bssid, sizeof(out->bssid));
            out->ssid_length = entry->ssid_length;
            out->channel = entry->channel;
            out->rssi = entry->rssi;
            out->auth = entry->auth <= 7 ? entry->auth : 255;
            if (i == scan.observations.count) ++scan.observations.count;
        }
    }
    taskEXIT_CRITICAL();
}

void fhost_scan_request_completed(int vif, uint32_t id, uint32_t result, uint32_t count)
{
    (void)count;
    taskENTER_CRITICAL();
    if (id && vif == MGMR_VIF_STA && scan.active == id && scan.pending) {
        scan.pending = false;
        publish(result == WIFI_SCAN_RESULT_SUCCESS ? IPRO_WIFI_SCAN_SUCCESS : IPRO_WIFI_SCAN_FAILED);
        if (!scan.worker) memset(&scan.config, 0, sizeof(scan.config));
    }
    taskEXIT_CRITICAL();
}

static void scan_worker(void *context)
{
    (void)context;
    taskENTER_CRITICAL();
    const bool cancel = scan.closed || scan.delivered || expired();
    if (cancel) {
        scan.pending = false;
        publish(IPRO_WIFI_SCAN_TIMEOUT);
    }
    taskEXIT_CRITICAL();
    if (!cancel) {
        /* Borrowed static config remains alive even if command transport stalls. */
        const int error = wifi_mgmr_sta_scan_with_id(&scan.config, scan.active, &scan.command);
        taskENTER_CRITICAL();
        if (error) {
            if (error == -1) scan.pending = false; /* Explicit rejection. */
            publish(IPRO_WIFI_SCAN_FAILED);
        }
        taskEXIT_CRITICAL();
    }
    taskENTER_CRITICAL();
    if (!scan.pending) memset(&scan.config, 0, sizeof(scan.config));
    scan.worker = false;
    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

int ipro_wifi_scan_start(const uint8_t *ssid, uint32_t length, uint32_t timeout_ms, uint32_t *operation)
{
    if (operation) *operation = 0;
    if (!operation || (length && !ssid) || length > 32 || !timeout_ms || timeout_ms > 60000 || !task_context())
        return -1;
    taskENTER_CRITICAL();
    if (scan.closed || scan.sequence == UINT32_MAX) { taskEXIT_CRITICAL(); return -1; }
    if (scan.worker || scan.pending || scan.ready) { taskEXIT_CRITICAL(); return 1; }
    const uint32_t id = ++scan.sequence;
    scan.active = id;
    scan.worker = scan.pending = true;
    scan.delivered = false;
    memset(&scan.observations, 0, sizeof(scan.observations));
    scan.started = xTaskGetTickCount();
    scan.budget = pdMS_TO_TICKS(timeout_ms);
    if (!scan.budget) scan.budget = 1;
    memset(&scan.config, 0, sizeof(scan.config));
    scan.config.ssid_length = (uint8_t)length;
    if (length) memcpy(scan.config.ssid_array, ssid, length);
    taskEXIT_CRITICAL();
    if (xTaskCreate(scan_worker, "br_wifi_scan", 1024, NULL, tskIDLE_PRIORITY + 2, NULL) != pdPASS) {
        taskENTER_CRITICAL();
        scan.worker = scan.pending = false;
        scan.active = 0;
        memset(&scan.config, 0, sizeof(scan.config));
        taskEXIT_CRITICAL();
        return -1;
    }
    *operation = id;
    return 0;
}

static int take(uint32_t operation, uint32_t *result, ipro_wifi_scan_snapshot *out)
{
    if (result) *result = 0;
    if (!operation || !result || !task_context()) return -1;
    taskENTER_CRITICAL();
    if (scan.closed || scan.active != operation || scan.delivered) { taskEXIT_CRITICAL(); return -1; }
    if (!scan.ready && expired()) publish(IPRO_WIFI_SCAN_TIMEOUT);
    if (!scan.ready) { taskEXIT_CRITICAL(); return 1; }
    *result = scan.result;
    if (out && scan.result == IPRO_WIFI_SCAN_SUCCESS) *out = scan.observations;
    scan.ready = false;
    scan.delivered = true;
    taskEXIT_CRITICAL();
    return 0;
}

int ipro_wifi_scan_take(uint32_t operation, uint32_t *result)
{
    return take(operation, result, NULL);
}

int ipro_wifi_scan_take_results(uint32_t operation, uint32_t *result,
                                ipro_wifi_scan_snapshot *out, uint32_t size)
{
    if (!out || size != sizeof(*out)) return -1;
    memset(out, 0, sizeof(*out));
    return take(operation, result, out);
}

int ipro_wifi_scan_close(void)
{
    if (!task_context()) return -1;
    taskENTER_CRITICAL();
    scan.closed = scan.delivered = true;
    scan.ready = false;
    const bool busy = scan.worker || scan.pending;
    taskEXIT_CRITICAL();
    return busy ? 1 : 0;
}

int ipro_wifi_scan_quiesce(void)
{
    if (!task_context()) return -1;
    taskENTER_CRITICAL();
    if (scan.closed) { taskEXIT_CRITICAL(); return -1; }
    /* Discard the response, not buffers borrowed by the command transport.
     * A queued worker skips submission; an admitted request needs its real CFM. */
    scan.delivered = true;
    scan.ready = false;
    const bool busy = scan.worker || scan.pending;
    taskEXIT_CRITICAL();
    return busy ? 1 : 0;
}
