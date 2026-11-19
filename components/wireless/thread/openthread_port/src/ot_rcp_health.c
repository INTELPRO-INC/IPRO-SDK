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

/* Fatal-error handling for the OpenThread Spinel host; see ot_rcp_health.h. */
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "ipro_log.h"
#include "openthread_port.h"
#include "ot_rcp_health.h"

#define TAG "RCP"

static ot_rcp_fault_t s_fault;
static bool s_failed;

__attribute__((weak)) void ot_rcp_on_fatal(const ot_rcp_fault_t *fault)
{
    (void)fault;
}

bool ot_rcp_get_fault(ot_rcp_fault_t *out)
{
    bool failed;

    taskENTER_CRITICAL();
    failed = s_failed;
    if (failed && out != NULL) {
        *out = s_fault;
    }
    taskEXIT_CRITICAL();
    return failed;
}

void ot_rcp_fatal(int exit_code, const char *cause, const char *file, int line)
{
    const bool in_ot_task = otrIsCurrentThread();
    bool first;

    taskENTER_CRITICAL();
    first = !s_failed;
    if (first) {
        s_failed = true;
        s_fault.exit_code = exit_code;
        s_fault.cause = cause;
        s_fault.file = file;
        s_fault.line = line;
        s_fault.uptime_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        s_fault.in_ot_task = in_ot_task;
        strncpy(s_fault.task, pcTaskGetName(NULL), sizeof(s_fault.task) - 1);
    }
    taskEXIT_CRITICAL();
    otrMarkStopped();

    if (first) {
        const char *where = s_fault.file ? strrchr(s_fault.file, '/') : NULL;

        IPRO_LOGE(TAG, "OpenThread stopped: exit=%d cause=%s at %s:%d task=%s uptime=%lu ms", exit_code, cause,
                  where ? where + 1 : (s_fault.file ? s_fault.file : "-"), line, s_fault.task,
                  (unsigned long)s_fault.uptime_ms);
        ot_radio_spinel_stop();
        ot_rcp_on_fatal(&s_fault);
    }
    /* Keep the OpenThread mutex if this task holds it: the stack must not run again. */
    for (;;) {
        vTaskSuspend(NULL);
    }
}

#define RCP_FLAP_WINDOW_MS 300000u
#ifdef CONFIG_THREAD_RCP_FLAP_MAX_RESETS
#define RCP_FLAP_MAX_RESETS ((uint32_t)CONFIG_THREAD_RCP_FLAP_MAX_RESETS)
#else
#define RCP_FLAP_MAX_RESETS 10u
#endif

/* Both touched only by the OpenThread task (Spinel send path, radio callbacks). */
static uint32_t s_flap_start;
static uint32_t s_flap_resets;

void ot_rcp_note_progress(void)
{
    s_flap_resets = 0u;
}

void ot_rcp_note_reset(void)
{
    const uint32_t now = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    if (s_flap_resets == 0u || now - s_flap_start > RCP_FLAP_WINDOW_MS) {
        s_flap_start = now;
        s_flap_resets = 0u;
    }
    if (++s_flap_resets > RCP_FLAP_MAX_RESETS) {
        IPRO_LOGE(TAG, "RCP reset %lu times in %lu s without doing any radio work", (unsigned long)s_flap_resets,
                  (unsigned long)((now - s_flap_start) / 1000u));
        ot_rcp_fatal(1 /* OT_EXIT_FAILURE */, "rcp-flapping", __FILE__, __LINE__);
    }
}

/* exit() (wrapped by the application so this runs before atexit handlers),
 * abort(), assert(): take over when the task was running OpenThread code -
 * its own task, or a caller holding the OpenThread mutex. */
void newlibc_fatal_hook(const char *cause, int status, const char *file, int line)
{
    if (xPortIsInsideInterrupt()) {
        return;
    }
    if (otrIsCurrentThread() || otrIsLockOwner()) {
        ot_rcp_fatal(status, cause, file, line);
    }
}
