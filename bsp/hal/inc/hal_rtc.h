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

#ifndef __HAL_RTC__H__
#define __HAL_RTC__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"

void rtc_init(uint64_t sleep_time);
void rtc_set_timestamp(uint64_t time_stamp);
uint64_t rtc_get_timestamp(void);

#ifdef __cplusplus
}
#endif
#endif
