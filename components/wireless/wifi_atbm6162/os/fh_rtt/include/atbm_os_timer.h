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

#ifndef ATBM_OS_TIMER_H
#define ATBM_OS_TIMER_H
#include "atbm_type.h"
#include <rtdef.h>
#include <rtthread.h>

typedef atbm_void (*TIMER_CALLBACK)(atbm_void * CallRef);
#define TIMER_1ST_PARAM
/*** Timer Management ***/
typedef void *os_timer_arg_t;
typedef rt_timer_t OS_TIMER;
typedef enum os_timer_reload {
	OS_TIMER_ONE_SHOT,
	OS_TIMER_PERIODIC,
} os_timer_reload_t;

typedef enum os_timer_activate {
	OS_TIMER_AUTO_ACTIVATE,
	OS_TIMER_NO_ACTIVATE,
} os_timer_activate_t;

atbm_uint32   atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef);
atbm_uint32  atbm_StartTimer(OS_TIMER *pTimer, atbm_uint32 Interval/*ms*/);
atbm_uint32 atbm_CancelTimer(OS_TIMER *pTimer);
atbm_uint32 atbm_FreeTimer(OS_TIMER *pTimer);
atbm_uint32  atbm_GetOsTime(atbm_void);
ATBM_BOOL atbm_TimeAfter(atbm_uint32 tickMs, atbm_uint32 intvMs);
atbm_uint64 atbm_GetOsTimeMs();
atbm_uint32 atbm_TimerTickGet(atbm_void);
atbm_uint32 atbm_TimerTickToMs(atbm_uint32 tick);
atbm_uint8 atbm_TimerIsActive(OS_TIMER *pTimer);
atbm_uint32 atbm_TimerGetExpiry(OS_TIMER *pTimer);

atbm_uint32 msecs_to_jiffies(atbm_uint32 a);
#define atbm_mdelay(x) atbm_SleepMs(x)
#endif /* ATBM_OS_TIMER_H */
