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

#include <string.h>
#include "hal_sec_common_v2.h"
#include "hal_sec_trng_v2.h"
#include "hal_sec_v2_priv.h"

#ifndef SEC_V2_HOST
#include "hal_common.h"
#include "drv_glb.h"
#ifdef CONFIG_IPRO7
#include "glb_reg.h"
#endif

#define SEC_V2_REG(off) (*(volatile uint32_t *)(uintptr_t)(SEC_ENG_BASE + (off)))

uint32_t sec_v2_rd(uint32_t off)
{
    return SEC_V2_REG(off);
}

void sec_v2_wr(uint32_t off, uint32_t v)
{
    SEC_V2_REG(off) = v;
}

uint32_t sec_v2_dma(const void *p)
{
    return (uint32_t)(uintptr_t)hal_get_noncacheable_addr(p);
}

void sec_v2_flush(const void *p, size_t n)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1)
    uintptr_t a = (uintptr_t)p & ~(uintptr_t)(CCM_CACHELINE_SIZE - 1);
    uintptr_t end = (uintptr_t)p + n;

    if (n && hal_is_cacheable_addr(p)) {
        MFlushInvalDCacheLines(a, (end - a + CCM_CACHELINE_SIZE - 1) /
                                      CCM_CACHELINE_SIZE);
    }
#else
    (void)p;
    (void)n;
#endif
}

/* word-aligned, inside the cacheable OCRAM alias 0x11000000-0x11FFFFFF */
int sec_v2_direct(const void *p, size_t n)
{
    uintptr_t a = (uintptr_t)p;

    return (a & 3u) == 0 && (a >> 24) == 0x11u &&
           n <= 0x01000000u - (a & 0x00FFFFFFu);
}

#ifndef CCM_CACHELINE_SIZE
#define CCM_CACHELINE_SIZE 32
#endif

size_t sec_v2_direct_out(const void *p, size_t n)
{
    size_t m = n & ~(size_t)(CCM_CACHELINE_SIZE - 1);

    if ((uintptr_t)p % CCM_CACHELINE_SIZE || !m || !sec_v2_direct(p, m)) {
        return 0;
    }
    return m;
}

void sec_v2_soc_clock_on(void)
{
#ifdef CONFIG_IPRO7
    volatile uint32_t *cgen =
        (volatile uint32_t *)(uintptr_t)(GLB_BASE + GLB_CGEN_CFG0_OFFSET);

    *cgen |= 1u << GLB_CGEN_SEC_POS;
#else
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SEC);
#endif
}

void sec_v2_soc_reset(void)
{
#ifdef CONFIG_IPRO7
    volatile uint32_t *rst =
        (volatile uint32_t *)(uintptr_t)(GLB_BASE + GLB_SWRST_CFG1_OFFSET);

    *rst |= 1u << GLB_SWRST_SEC_ENG_POS;
    *rst &= ~(1u << GLB_SWRST_SEC_ENG_POS);
#else
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SEC_ENG);
#endif
}
#else
void sec_v2_soc_clock_on(void);
void sec_v2_soc_reset(void);
#endif

static struct hal_sec_v2_lock_ops sec_v2_lock_ops;
static uint8_t sec_v2_want_claim;

void hal_sec_v2_set_lock_ops(const struct hal_sec_v2_lock_ops *ops)
{
    if (ops) {
        sec_v2_lock_ops = *ops;
    } else {
        memset(&sec_v2_lock_ops, 0, sizeof(sec_v2_lock_ops));
    }
}

int sec_v2_lock(unsigned int block)
{
    if (!sec_v2_lock_ops.lock) {
        return HAL_SEC_V2_OK;
    }
    return sec_v2_lock_ops.lock(sec_v2_lock_ops.arg, block) == 0 ?
               HAL_SEC_V2_OK : HAL_SEC_V2_ERR_TIMEOUT;
}

void sec_v2_unlock(unsigned int block)
{
    if (sec_v2_lock_ops.unlock) {
        sec_v2_lock_ops.unlock(sec_v2_lock_ops.arg, block);
    }
}

void hal_sec_v2_zeroize(void *p, size_t n)
{
    if (p && n) {
        memset(p, 0, n);
        __asm__ __volatile__("" : : "r"(p) : "memory");
    }
}

static int sec_v2_wait(uint32_t ctrl)
{
    for (uint32_t i = 0; i < SEC_V2_SPIN; i++) {
        if (!(sec_v2_rd(ctrl) & SEC_V2_BUSY)) {
            return HAL_SEC_V2_OK;
        }
    }
    return HAL_SEC_V2_ERR_TIMEOUT;
}

int sec_v2_job(uint32_t ctrl, uint32_t link, uint32_t set, uint32_t clr,
               uint32_t cfg_dma)
{
    uint32_t v;

    if (sec_v2_wait(ctrl) != HAL_SEC_V2_OK) {
        hal_sec_v2_recover();
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    sec_v2_wr(link, cfg_dma);
    v = (sec_v2_rd(ctrl) & ~clr) | set;
    sec_v2_wr(ctrl, v);
    sec_v2_wr(ctrl, v | SEC_V2_TRIG);
    if (sec_v2_wait(ctrl) != HAL_SEC_V2_OK) {
        hal_sec_v2_recover();
        return HAL_SEC_V2_ERR_TIMEOUT;
    }
    return HAL_SEC_V2_OK;
}

int hal_sec_v2_init(void)
{
    sec_v2_soc_clock_on();
    return hal_sec_trng_v2_init();
}

int hal_sec_v2_claimed(void)
{
    uint32_t rd = sec_v2_rd(SEC_V2_PROT_RD);

    for (unsigned int b = 0; b < HAL_SEC_V2_BLOCKS; b++) {
        if (((rd >> (2u * b)) & 3u) != 1u) {
            return 0;
        }
    }
    return 1;
}

int hal_sec_v2_claim(void)
{
    sec_v2_want_claim = 1;
    for (unsigned int b = 0; b < HAL_SEC_V2_BLOCKS; b++) {
        if (((sec_v2_rd(SEC_V2_PROT_RD) >> (2u * b)) & 3u) == 3u) {
            sec_v2_wr(SEC_V2_PROT(b), 0x02u);
        }
    }
    return hal_sec_v2_claimed() ? HAL_SEC_V2_OK : HAL_SEC_V2_ERR_ACCESS;
}

int hal_sec_v2_recover(void)
{
    int ret;

    sec_v2_soc_reset();
    ret = hal_sec_trng_v2_init();
    if (sec_v2_want_claim && hal_sec_v2_claim() != HAL_SEC_V2_OK) {
        return HAL_SEC_V2_ERR_ACCESS;
    }
    return ret;
}
