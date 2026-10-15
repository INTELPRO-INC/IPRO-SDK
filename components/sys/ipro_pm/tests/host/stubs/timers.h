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

#ifndef HOST_TIMERS_H
#define HOST_TIMERS_H
#include "FreeRTOS.h"
typedef void (*PendedFunction_t)(void *, uint32_t);
BaseType_t xTimerPendFunctionCall(PendedFunction_t fn, void *p1, uint32_t p2, TickType_t wait);
#endif
