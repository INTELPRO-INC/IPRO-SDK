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

#include <ipro_matter_br_reset.h>
#include <ipro_matter_wifi_scan.h>
#include "br_wifi_storage.h"
#include "br_wifi_connect_operation.h"
#include <FreeRTOS.h>
#include <task.h>
#include "openthread_port.h"

static TaskHandle_t reset_owner;
static bool ot_fenced;

bool ipro_matter_br_is_network_worker(void)
{
    return otrIsCurrentThread();
}

otError ipro_matter_br_reset_fence(void)
{
    if (xPortIsInsideInterrupt() || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING ||
        otrIsCurrentThread() || !otrGetInstanceIfReady()) return OT_ERROR_INVALID_STATE;

    const TaskHandle_t caller = xTaskGetCurrentTaskHandle();
    taskENTER_CRITICAL();
    if (!reset_owner) reset_owner = caller;
    const bool owner = reset_owner == caller;
    taskEXIT_CRITICAL();
    if (!owner) return OT_ERROR_INVALID_STATE;
    if (ot_fenced) return OT_ERROR_NONE;

    /* CHIP shutdown/fail-safe handling is complete before this final fence.
     * Drain tracked SCAN and CONNECT before taking the retained OT mutex: a worker completion
     * must not be stranded behind a lock the reset task will never release. */
    const int scan = ipro_wifi_scan_close();
    if (scan < 0) return OT_ERROR_INVALID_STATE;
    if (scan != 0) return OT_ERROR_BUSY;

    const int connect = br_wifi_connect_operation_drain();
    if (connect < 0) return OT_ERROR_INVALID_STATE;
    if (connect != 0) return OT_ERROR_BUSY;

    const int wifi = br_wifi_storage_fence();
    if (wifi < 0) return OT_ERROR_INVALID_STATE;
    if (wifi != 0 || !otrTryLock()) return OT_ERROR_BUSY;
    /* All SDK OT mutations (including queued CLI work) run under this mutex.
     * Keep its owner alive through deletion failures. No second OT instance,
     * task suspension or live Wi-Fi/lwIP ownership transfer is involved. */
    ot_fenced = true;
    return OT_ERROR_NONE;
}
