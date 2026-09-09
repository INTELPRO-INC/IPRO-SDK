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

#pragma once

#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <utils_list.h>

#ifndef container_of
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define PTR_SZ_MSK (sizeof(void *) - 1)
#define ALIGN_PTR_PADD(x) ((sizeof(void *) - ((uintptr_t)(x) & PTR_SZ_MSK)) & PTR_SZ_MSK)
#define ALIGN_PTR_HIGH(x) (((uintptr_t)(x) + PTR_SZ_MSK) & ~PTR_SZ_MSK)

#define ALIGN_LOW(align, addr) ((uintptr_t)(addr) & ~((uintptr_t)(align) - 1))

#define ENTER_CRITICAL taskENTER_CRITICAL
#define EXIT_CRITICAL  taskEXIT_CRITICAL

static inline struct utils_list_hdr *atomic_list_pop_front(struct utils_list *list)
{
    struct utils_list_hdr *node;

    ENTER_CRITICAL();
    node = utils_list_pop_front(list);
    EXIT_CRITICAL();
    return node;
}

#if defined(IPRO6LE) || defined(IPRO6)
#define MAWOC(addr) ((((uintptr_t)(addr) & ~0xF0000000) | 0x20000000))
#endif
