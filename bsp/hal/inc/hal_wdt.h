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

#ifndef __HAL_WDT__H__
#define __HAL_WDT__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"

void wdt_init(int int_mode, uint32_t wdt_timeout);
void wdt_disable(void);
uint32_t wdt_get_counter(void);
void wdt_feed(void);
void wdt_isWDTReset(void);
void wdt_clearWDTReset(void);
void wdt_setTimeout(uint32_t wdt_timeout);

#ifdef __cplusplus
}
#endif
#endif
