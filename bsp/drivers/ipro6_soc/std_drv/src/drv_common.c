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

#include "drv_common.h"
#include <string.h>
#include "drv_glb.h"
#include "drv_dma.h"
#include "drv_i2s.h"
#include "drv_i2c.h"
#include "drv_uart.h"

void ATTR_TCM_SECTION arch_delay_us(uint32_t tick_us)
{
    volatile uint64_t start_mtime, delta_mtime;
    volatile uint64_t delay_ticks = (SOC_TIMER_TICK_PER_US * (uint64_t)tick_us);

    start_mtime = SysTimer_GetLoadValue();

    do {
        delta_mtime = SysTimer_GetLoadValue() - start_mtime;
    } while (delta_mtime < delay_ticks);
}

void ATTR_TCM_SECTION arch_delay_ms(uint32_t cnt)
{
    uint32_t i = 0;
    uint32_t count = 0;

    if (cnt >= 1024) {
        /* delay (n*1024) ms */
        count = 1024;

        for (i = 0; i < (cnt / 1024); i++) {
            arch_delay_us(1024 * 1000);
        }
    }

    if (cnt & 0x3FF) {
        /* delay (1-1023)ms */
        count = cnt & 0x3FF;
        arch_delay_us(count * 1000);
    }
}

#ifdef DEBUG

void ATTR_TCM_SECTION check_failed(uint8_t *file, uint32_t line)
{
    /* Infinite loop */
    while (1)
        ;
}
#endif /* DEBUG */

bool drv_is_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if ((a & 0xFFFF0000UL) >= 0x62FC0000UL) {
        return true;
    }
    return false;
}

void *drv_get_no_cache_addr(const void *addr)
{
    uintptr_t a = (uintptr_t)addr;

    if (!drv_is_cache_addr(addr)) {
        return (void *)addr;
    }

    // OCRAM
    if ((a & 0xF0000000UL) == 0x60000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x20000000UL);
    }
    // pSRAM
    if ((a & 0xF0000000UL) == 0xA0000000UL) {
        return (void *)((a & ~0xF0000000UL) | 0x10000000UL);
    }

    return NULL;
}
