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

#include "br_wifi_storage.h"
#include <FreeRTOS.h>
#include <task.h>
#include <easyflash.h>
#include <stdbool.h>

static bool closed, active;
static const void *storage_owner;

static bool task_context(void)
{
    return !xPortIsInsideInterrupt() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING;
}

static bool begin_access(const void *owner)
{
    if (!task_context()) return false;
    taskENTER_CRITICAL();
    const bool admitted = !closed && !active && storage_owner == owner;
    if (admitted) active = true;
    taskEXIT_CRITICAL();
    return admitted;
}

static void end_access(void)
{
    taskENTER_CRITICAL();
    active = false;
    taskEXIT_CRITICAL();
}

int br_wifi_storage_claim(const void *owner)
{
    if (!owner || !task_context()) return -1;
    taskENTER_CRITICAL();
    const bool admitted = !closed && !active && (!storage_owner || storage_owner == owner);
    if (admitted) storage_owner = owner;
    taskEXIT_CRITICAL();
    return admitted ? 0 : -1;
}

int br_wifi_storage_save_owned(const void *owner, const void *profile, size_t size)
{
    if (!profile || !size || !begin_access(owner)) return -1;
    const int result = ef_env_set_checked(BR_WIFI_KEY, profile, size);
    end_access();
    return result;
}

int br_wifi_storage_clear_owned(const void *owner)
{
    if (!begin_access(owner)) return -1;
    const int result = ef_env_del_checked(BR_WIFI_KEY);
    end_access();
    return result == EF_ENV_NOT_FOUND ? EF_NO_ERR : result;
}

int br_wifi_storage_load_owned(const void *owner, struct br_wifi_profile *profile, bool *present)
{
    if (!profile || !present) return -1;
    *present = false;
    br_wifi_profile_wipe(profile, sizeof(*profile));
    if (!begin_access(owner)) return -1;
    size_t copied = 0, total = 0;
    int error = ef_env_get_checked(BR_WIFI_KEY, 0, profile, sizeof(*profile), &copied, &total);
    if (error == EF_NO_ERR) {
        if (copied != sizeof(*profile) || total != sizeof(*profile) ||
            !br_wifi_profile_valid(profile, total)) error = EF_ENV_CORRUPT;
        else *present = true;
    } else if (error == EF_ENV_NOT_FOUND) error = EF_NO_ERR;
    if (!*present) br_wifi_profile_wipe(profile, sizeof(*profile));
    end_access();
    return error;
}

int br_wifi_storage_save(const void *profile, size_t size)
{
    return br_wifi_storage_save_owned(NULL, profile, size);
}

int br_wifi_storage_clear(void) { return br_wifi_storage_clear_owned(NULL); }

int br_wifi_storage_load(struct br_wifi_profile *profile, bool *present)
{
    return br_wifi_storage_load_owned(NULL, profile, present);
}

int br_wifi_storage_fence(void)
{
    if (!task_context()) return -1;
    taskENTER_CRITICAL();
    closed = true;
    const int result = active ? 1 : 0;
    taskEXIT_CRITICAL();
    return result;
}
