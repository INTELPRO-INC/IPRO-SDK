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

#ifndef HOST_DRV_HBN_H
#define HOST_DRV_HBN_H
#include <stdint.h>
extern uint64_t host_rtc;
/* a test can model the RTC as a function of time instead */
extern uint64_t (*host_rtc_fn)(void);
static inline int HBN_Get_RTC_Timer_Val(uint32_t *lo, uint32_t *hi)
{
    uint64_t v = (host_rtc_fn != 0) ? host_rtc_fn() : host_rtc;

    *lo = (uint32_t)v;
    *hi = (uint32_t)(v >> 32);
    return 0;
}
static inline int HBN_Enable_RTC_Counter(void) { return 0; }
#endif
