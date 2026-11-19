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

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <openthread/platform/memory.h>


#include "FreeRTOS.h"
#include "task.h"

void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    if (aNum == 0 || aSize == 0 || aNum > SIZE_MAX / aSize) return NULL;
    size_t total_size = aNum * aSize;
    void *ptr = pvPortMalloc(total_size);
    if (ptr != NULL) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void otPlatFree(void *aPtr)
{
    if (aPtr != NULL) {
        vPortFree(aPtr);
    }
}
