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
#include "drv_clock.h"
#include "hal_mtimer.h"

uint64_t ATTR_TCM_SECTION mtimer_get_time_ms()
{
    /* Avoid 64-bit division (__udivdi3 is in XIP/flash)
     * x / 1000 ≈ (x * 274877907) >> 38 (error < 0.0001%)
     * But 274877907 * large us value could overflow 64-bit
     * Use simpler: x >> 10 = x / 1024 (2.4% error, acceptable for LP debug)
     */
    return mtimer_get_time_us() >> 10;
}

uint64_t ATTR_TCM_SECTION mtimer_get_time_us()
{
#if defined(CONFIG_IPRO6LE)
    return CPU_Get_MTimer_Counter();
#elif defined(CONFIG_IPRO7)
    return SysTimer_GetLoadValue() >> 2;
#elif defined(CONFIG_IPRO6)
    return SysTimer_GetLoadValue();
#else
    #error "Unknown platform"
#endif
}

void ATTR_TCM_SECTION mtimer_delay_ms(uint32_t time)
{
    arch_delay_ms(time);
}

void ATTR_TCM_SECTION mtimer_delay_us(uint32_t time)
{
    arch_delay_us(time);
}

void ATTR_TCM_SECTION mtimer_clear_time()
{
#if defined(CONFIG_IPRO7) || defined(CONFIG_IPRO6)
    SysTimer_SetLoadValue(0);
#endif
}
