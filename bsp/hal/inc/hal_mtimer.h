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

#ifndef __HAL_MTIMER__H__
#define __HAL_MTIMER__H__

#include "stdint.h"

#ifdef __cplusplus
extern "C"{
#endif

uint64_t mtimer_get_time_ms();
uint64_t mtimer_get_time_us();
void mtimer_delay_ms(uint32_t time);
void mtimer_delay_us(uint32_t time);
void mtimer_clear_time();
#ifdef __cplusplus
}
#endif

#endif