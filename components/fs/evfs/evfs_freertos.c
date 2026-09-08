/* SPDX-License-Identifier: MIT
Copyright 2020 Kevin Thibedeau
FreeRTOS port Copyright 2025 IPRO Technology Co., Ltd.

See LICENSE in the EVFS project root for details
*/

/*
------------------------------------------------------------------------------
Embedded Virtual Filesystem

  Thread wrappers for FreeRTOS
------------------------------------------------------------------------------
*/
#include "evfs.h"
#include "evfs_internal.h"

#if defined EVFS_USE_THREADING && defined USE_FREERTOS

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

static volatile bool s_evfs_initialized = false;

void evfs__init_once(void) {
    if (!s_evfs_initialized) {
        taskENTER_CRITICAL();
        if (!s_evfs_initialized) {
            evfs__lib_init();
            s_evfs_initialized = true;
        }
        taskEXIT_CRITICAL();
    }
}

// ******************** Locking API ********************

int evfs__lock_init(EvfsLock *lock) {
    if (!lock) {
        return EVFS_ERR;
    }

    *lock = xSemaphoreCreateMutex();
    return (*lock != NULL) ? EVFS_OK : EVFS_ERR;
}

int evfs__lock_destroy(EvfsLock *lock) {
    if (!lock || !*lock) {
        return EVFS_ERR;
    }

    vSemaphoreDelete(*lock);
    *lock = NULL;
    return EVFS_OK;
}

int evfs__lock(EvfsLock *lock) {
    if (!lock || !*lock) {
        return EVFS_ERR;
    }

    if (xSemaphoreTake(*lock, portMAX_DELAY) == pdTRUE) {
        return EVFS_OK;
    }
    return EVFS_ERR;
}

int evfs__unlock(EvfsLock *lock) {
    if (!lock || !*lock) {
        return EVFS_ERR;
    }

    if (xSemaphoreGive(*lock) == pdTRUE) {
        return EVFS_OK;
    }
    return EVFS_ERR;
}

#endif /* EVFS_USE_THREADING && USE_FREERTOS */
