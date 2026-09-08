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

#ifndef __HAL_CACHE_ALLOC_H__
#define __HAL_CACHE_ALLOC_H__

#include <stdint.h>
#include <stddef.h>

#if defined(CONFIG_IPRO6LE)
#define HAL_CACHE_LINE_SIZE 32
#else
#include <core_feature_cache.h>
#define HAL_CACHE_LINE_SIZE CCM_CACHELINE_SIZE
#endif

/*
 * Cache-line aligned heap allocator.
 *
 * pvPortMalloc guarantees only portBYTE_ALIGNMENT (8 bytes on IPRO7).
 * Buffers shared with DMA or hardware accelerators require cache-line
 * alignment so that cache clean/invalidate operations do not corrupt
 * adjacent heap objects sharing the same cache line.
 *
 * Layout: [raw_ptr | padding ... | aligned_payload ...]
 *                                 ^ returned pointer
 *         stored at ((void**)aligned)[-1]
 *
 * Requires FreeRTOS (pvPortMalloc/vPortFree).
 */

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>

static inline void *pvPortMallocCacheAligned(size_t size)
{
    void *raw = pvPortMalloc(size + HAL_CACHE_LINE_SIZE + sizeof(void *));
    if (!raw)
        return NULL;

    uintptr_t aligned = ((uintptr_t)raw + sizeof(void *) + HAL_CACHE_LINE_SIZE - 1)
                        & ~(HAL_CACHE_LINE_SIZE - 1);
    ((void **)aligned)[-1] = raw;
    return (void *)aligned;
}

static inline void vPortFreeCacheAligned(void *ptr)
{
    if (ptr)
        vPortFree(((void **)ptr)[-1]);
}

#endif /* CONFIG_FREERTOS */

#endif /* __HAL_CACHE_ALLOC_H__ */
