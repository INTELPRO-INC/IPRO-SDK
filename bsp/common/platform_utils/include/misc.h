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

#ifndef _MISC_H
#define _MISC_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include "compiler/gcc.h"
#include "compiler/common.h"

#ifdef BIT
#undef BIT
#define BIT(n) (1UL << (n))
#else
#define BIT(n) (1UL << (n))
#endif

#ifndef NULL
#define NULL 0
#endif

typedef enum {
    MISC_SUCCESS = 0,
    MISC_ERROR = 1,
    MISC_TIMEOUT = 2,
    MISC_INVALID = 3, /* invalid arguments */
    MISC_NORESC = 4,  /* no resource or resource temperary unavailable */
}Err_Type;

/*
 * Legacy defines for backward compatibility with BSP code.
 * Using numeric values to avoid conflicts with other libraries
 * (wpa_supplicant, LwIP) that use these names as enum identifiers.
 */
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef ERROR
#define ERROR 1
#endif
#ifndef TIMEOUT
#define TIMEOUT 2
#endif
#ifndef INVALID
#define INVALID 3
#endif
#ifndef NORESC
#define NORESC 4
#endif

typedef enum {
    DISABLE = 0,
    ENABLE = 1,
}Fun_Type;

typedef enum {
    RESET = 0,
    SET = 1,
}Sts_Type;

typedef enum {
    UNMASK = 0,
    MASK = 1
}Mask_Type;

#define IS_MASK_TYPE(type)          (((type) == MASK) || ((type) == UNMASK))

typedef void(intCallback_Type)(void);
typedef void (*pFunc)(void);

#define arch_memcpy      arch_memcpy
#define arch_memset      arch_memset
#define ARCH_MemCmp      arch_memcmp
#define arch_memcpy4     arch_memcpy4
#define arch_memcpy_fast arch_memcpy_fast
#define arch_memset4     arch_memset4

void *arch_memcpy(void *dst, const void *src, uint32_t n);
void *arch_memset(void *s, uint8_t c, uint32_t n);
int arch_memcmp(const void *s1, const void *s2, uint32_t n);
uint32_t *arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n);
void *arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n);
uint32_t *arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n);
void memcopy_to_fifo(void *fifo_addr, uint8_t *data, uint32_t length);
void fifocopy_to_mem(void *fifo_addr, uint8_t *data, uint32_t length);
int arch_ctzll(uint64_t *val, uint32_t *bit);
int arch_clzll(uint64_t *val, uint32_t *bit);
int arch_ffsll(uint64_t *val, uint32_t *bit);

#ifdef DEBUG
void check_failed(uint8_t *file, uint32_t line);
#define CHECK_PARAM(expr) ((expr) ? (void)0 : check_failed((uint8_t *)__FILE__, __LINE__))
#else
#define CHECK_PARAM(expr) ((void)0)
#endif /* DEBUG */
#endif

#define CHECK_PARAM_RANGE(value, min, max)  \
    CHECK_PARAM((value) >= min && (value) <= max)
#define CHECK_PARAM_MAX(value, max)  \
    CHECK_PARAM((value) <= max)
#define CHECK_PARAM_MIN(value, min)  \
    CHECK_PARAM((value) >= min)
