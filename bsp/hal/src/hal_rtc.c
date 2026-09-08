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

#include "hal_rtc.h"
#include "drv_hbn.h"

static uint64_t current_timestamp = 0;

void rtc_init(uint64_t sleep_time)
{
    uint32_t comp_l, comp_h;

    HBN_Clear_RTC_Counter();

    HBN_Get_RTC_Timer_Val(&comp_l, &comp_h);

    /* calculate RTC Comp time */
    comp_l += (uint32_t)((sleep_time * 32768) & 0xFFFFFFFF);
    comp_h += (uint32_t)(((sleep_time * 32768) >> 32) & 0xFFFFFFFF);

    HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, comp_l, comp_h, HBN_RTC_COMP_BIT0_39);

    HBN_Enable_RTC_Counter();
}

void rtc_set_timestamp(uint64_t time_stamp)
{
    current_timestamp = time_stamp;
}

uint64_t rtc_get_timestamp(void)
{
    uint32_t time_l;
    uint32_t time_h;

    HBN_Get_RTC_Timer_Val(&time_l, &time_h);

    return ((uint64_t) (((uint64_t)time_h << 32 | time_l) >> 15) + current_timestamp);
}
