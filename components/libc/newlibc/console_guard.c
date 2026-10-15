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

#include "console_guard.h"
#include <stddef.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#if defined(CONFIG_IPRO7)
/* Nuclei core CSR helpers and ECLIC (IPRO7 has no ipro_irq.h shim) */
#include "ipro7.h"
#include <system_soc.h>
static inline uintptr_t ipro_irq_save(void)
{
    return (uintptr_t)__RV_CSR_READ_CLEAR(CSR_MSTATUS, MSTATUS_MIE);
}

static inline void ipro_irq_restore(uintptr_t flags)
{
    __RV_CSR_WRITE(CSR_MSTATUS, (unsigned long)flags);
}
#ifndef __get_MSTATUS
#define __get_MSTATUS() __RV_CSR_READ(CSR_MSTATUS)
#endif
#else
#include "ipro_irq.h"
#endif

static StaticSemaphore_t sMutexStorage;
static SemaphoreHandle_t sMutex;
static bool sBusy;
static uint32_t sDrops;

bool newlibc_console_init(void)
{
    if (sMutex) return true;
    if (xPortIsInsideInterrupt() || xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        return false;
    sMutex = xSemaphoreCreateMutexStatic(&sMutexStorage);
    return sMutex != NULL;
}

static enum console_guard_token reject(void)
{
    uintptr_t flags = ipro_irq_save();
    sDrops++;
    ipro_irq_restore(flags);
    return CONSOLE_GUARD_DROP;
}

enum console_guard_token newlibc_console_enter(void)
{
    enum console_guard_token token = CONSOLE_GUARD_NONBLOCK;
    /* Inspect the caller's context before masking IRQs ourselves. Raw masking
     * does not necessarily increment the FreeRTOS critical nesting count. */
    bool can_wait = sMutex && !xPortIsInsideInterrupt() &&
                    xTaskGetSchedulerState() == taskSCHEDULER_RUNNING &&
                    portGET_CRITICAL_NESTING_COUNT() == 0 &&
                    (__get_MSTATUS() & MSTATUS_MIE) && ECLIC_GetMth() == 0;
    if (can_wait) {
        if (xSemaphoreGetMutexHolder(sMutex) == xTaskGetCurrentTaskHandle())
            return reject();  /* Recursive printf must not deadlock itself. */
        if (xSemaphoreTake(sMutex, portMAX_DELAY) != pdTRUE) return reject();
        token = CONSOLE_GUARD_LOCKED;
    }
    uintptr_t flags = ipro_irq_save();
    bool claimed = !sBusy;
    if (claimed) sBusy = true;
    else sDrops++;
    ipro_irq_restore(flags);
    if (!claimed) {
        if (token == CONSOLE_GUARD_LOCKED) xSemaphoreGive(sMutex);
        return CONSOLE_GUARD_DROP;
    }
    return token;
}

void newlibc_console_leave(enum console_guard_token token)
{
    if (token == CONSOLE_GUARD_DROP) return;
    uintptr_t flags = ipro_irq_save();
    sBusy = false;
    ipro_irq_restore(flags);
    if (token == CONSOLE_GUARD_LOCKED) xSemaphoreGive(sMutex);
}

uint32_t newlibc_console_drops(void)
{
    uintptr_t flags = ipro_irq_save();
    uint32_t drops = sDrops;
    ipro_irq_restore(flags);
    return drops;
}
