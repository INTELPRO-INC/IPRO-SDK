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

#include "misc.h"

void *ATTR_TCM_SECTION arch_memcpy(void *dst, const void *src, uint32_t n)
{
    const uint8_t *p = src;
    uint8_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

uint32_t *ATTR_TCM_SECTION arch_memcpy4(uint32_t *dst, const uint32_t *src, uint32_t n)
{
    const uint32_t *p = src;
    uint32_t *q = dst;

    while (n--) {
        *q++ = *p++;
    }

    return dst;
}

void *ATTR_TCM_SECTION arch_memcpy_fast(void *pdst, const void *psrc, uint32_t n)
{
    uint32_t left, done, i = 0;
    uint8_t *dst = (uint8_t *)pdst;
    uint8_t *src = (uint8_t *)psrc;

    if (((uint32_t)(uintptr_t)dst & 0x3) == 0 && ((uint32_t)(uintptr_t)src & 0x3) == 0) {
        arch_memcpy4((uint32_t *)dst, (const uint32_t *)src, n >> 2);
        left = n % 4;
        done = n - left;

        while (i < left) {
            dst[done + i] = src[done + i];
            i++;
        }
    } else {
        arch_memcpy(dst, src, n);
    }

    return dst;
}

uint32_t *ATTR_TCM_SECTION arch_memset4(uint32_t *dst, const uint32_t val, uint32_t n)
{
    uint32_t *q = dst;

    while (n--) {
        *q++ = val;
    }

    return dst;
}

int ATTR_TCM_SECTION arch_memcmp(const void *s1, const void *s2, uint32_t n)
{
    const unsigned char *c1 = s1, *c2 = s2;
    int d = 0;

    while (n--) {
        d = (int)*c1++ - (int)*c2++;

        if (d) {
            break;
        }
    }

    return d;
}

void memcopy_to_fifo(void *fifo_addr, uint8_t *data, uint32_t length)
{
    uint8_t *p = (uint8_t *)fifo_addr;
    uint8_t *q = data;

    while (length--) {
        *p = *q++;
    }
}

void fifocopy_to_mem(void *fifo_addr, uint8_t *data, uint32_t length)
{
    uint8_t *p = (uint8_t *)fifo_addr;
    uint8_t *q = data;

    while (length--) {
        *q++ = *p;
    }
}

/* arch_memset is NOT in ROM, always compile it */
void *ATTR_TCM_SECTION __attribute__((optimize("-O1"))) arch_memset(void *s, uint8_t c, uint32_t n)
{
    uint8_t *p = (uint8_t *)s;

    while (n > 0) {
        *p++ = (uint8_t)c;
        --n;
    }

    return s;
}

/****************************************************************************/ /**
 * @brief  get u64 first number 1 from right to left
 *
 * @param  val: target value
 * @param  bit: first 1 in bit
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
int arch_ffsll(uint64_t *val, uint32_t *bit)
{
    if (!*val) {
        return ERROR;
    }

    *bit = __builtin_ffsll(*val) - 1;
    *val &= ~((1ULL) << (*bit));
    return 0;
}

int arch_ctzll(uint64_t *val, uint32_t *bit)
{
    if (!*val)
        return -1;

    *bit = __builtin_ctzll(*val);
    *val &= ~((1ULL) << (*bit));
    return 0;
}

int arch_clzll(uint64_t *val, uint32_t *bit)
{
    if (!*val)
        return -1;

    *bit = __builtin_clzll(*val);
    *val &= ~((1ULL) << (*bit));
    return 0;
}

#ifdef DEBUG
/*******************************************************************************
* @brief  Reports the name of the source file and the source line number
*         where the CHECK_PARAM error has occurred.

* @param  file: Pointer to the source file name
* @param  line: assert_param error line source number

* @return None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
    /* Infinite loop */
    while (1)
        ;
}
#endif /* DEBUG */






#if defined(_REG_WRITE_DEBUG_)

#define _REG_WRITE_DEBUG_MAX_    (16384)

static volatile uint32_t g_udLogRegAVPairIdx = 0;
static volatile uint32_t g_audLogRegAVPair[_REG_WRITE_DEBUG_MAX_]={0};  /*reserve a array for storing (register, value) pairs*/

uint32_t writel_dump(uint32_t v, uint32_t *c)
{
    //printf("{Writel Reg Log] set *0x%08x=0x%08x\r\n",(unsigned int)(c),(unsigned int)(v));
    g_audLogRegAVPair[g_udLogRegAVPairIdx++] = (uint32_t)c;  \
    g_audLogRegAVPair[g_udLogRegAVPairIdx++] = v; \
    g_udLogRegAVPairIdx = g_udLogRegAVPairIdx&(_REG_WRITE_DEBUG_MAX_-1);

    *(volatile uint32_t *)(uintptr_t)(c) = (v);
    return v;
}
#endif


/*@} end of group DRIVER_Public_Functions */

/*@} end of group DRIVER_COMMON */

/*@} end of group IPRO_Periph_Driver */
