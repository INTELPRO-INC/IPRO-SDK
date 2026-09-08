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

#include "atbm_os_timer.h"
#include "atbm_hal.h"
//#include "fh_os.h"
static inline unsigned long os_msec_to_ticks(unsigned long msecs)
{
	return ((msecs) * (RT_TICK_PER_SECOND)) / 1000;
}
static inline unsigned long os_ticks_to_msec(unsigned long ticks)
{
	return ((ticks) * 1000) / (RT_TICK_PER_SECOND);
}

atbm_uint32  atbm_InitTimer(OS_TIMER *pTimer, TIMER_CALLBACK pCallback, atbm_void * CallRef)
{
	atbm_uint8 flag;
	flag=RT_TIMER_FLAG_DEACTIVATED;
	flag |= RT_TIMER_FLAG_ONE_SHOT;
	flag|=RT_TIMER_FLAG_SOFT_TIMER;
	//os_msec_to_ticks(10) timer is right???
    *pTimer = rt_timer_create("AtbmTimer", pCallback, CallRef, os_msec_to_ticks(10), flag);
    if(*pTimer == (OS_TIMER)NULL){
	    return -1;
    }else{
	    return 0;
    }
}
/**************************************************************************/
atbm_uint32  atbm_StartTimer(OS_TIMER *pTimer, atbm_uint32 Interval)
{
	atbm_uint32 result;
	atbm_uint32 ticks;
    ticks = os_msec_to_ticks(Interval);
	//change time
    result=rt_timer_control(*pTimer, RT_TIMER_CTRL_SET_TIME, &ticks);
    if (result == RT_EOK){
    }else{
        return -1;
    }
	//start time
    result = rt_timer_start(*pTimer);
    if (result == RT_EOK){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint32 atbm_CancelTimer(OS_TIMER *pTimer)
{
	atbm_uint32 result;
	if((atbm_void *)(*pTimer) == ATBM_NULL)
		return -1;

    result = rt_timer_stop(*pTimer);
	
    if (result == RT_EOK){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint32 atbm_FreeTimer(OS_TIMER *pTimer)
{
	atbm_uint32 result;

	if((atbm_void *)(*pTimer) == ATBM_NULL)
		return -1;

	result = rt_timer_delete(*pTimer);
    if (result == RT_EOK){
        return 0;
    }else{
        return -1;
    }
}
atbm_uint64 atbm_GetOsTimeMs()
{

	unsigned int tick,time;
	tick= rt_tick_get();
	time=os_ticks_to_msec(tick);
	return time;
}
atbm_uint32 atbm_GetOsTime(void)
{
	unsigned int time = rt_tick_get();
	return time;
}

atbm_uint32 atbm_TimerTickGet(atbm_void)
{
	return rt_tick_get();
}

atbm_uint32 atbm_TimerMsToTick(atbm_uint32 ms)
{
	return os_msec_to_ticks(ms);
}

atbm_uint32 atbm_TimerTickToMs(atbm_uint32 tick){
	return os_ticks_to_msec(tick);
}

atbm_uint8 atbm_TimerIsActive(OS_TIMER *pTimer)
{
	/* IPRO7 port: FreeRTOS active query instead of rt_timer->parent.flag. */
	return xTimerIsTimerActive((*pTimer)->h) ? 1 : 0;
}

atbm_uint32 atbm_TimerGetExpiry(OS_TIMER *pTimer)
{
	if(atbm_TimerIsActive(pTimer))
		/* IPRO7 port: ticks until expiry via FreeRTOS instead of ->timeout_tick. */
		return (atbm_uint32)(xTimerGetExpiryTime((*pTimer)->h) - xTaskGetTickCount());
	return 0;
}


/*Avoid timer wrap*/
ATBM_BOOL atbm_TimeAfter(atbm_uint32 tickMs, atbm_uint32 intvMs)
{
	atbm_uint32 current_time = atbm_GetOsTimeMs();
	return ((((int)(tickMs + intvMs - current_time)) >= 0) && (current_time >= tickMs));
}
atbm_void atbm_wifi_ticks_timer_init(atbm_void)
{
}

atbm_void atbm_SleepMs(atbm_uint32 ms){
	rt_thread_delay(os_msec_to_ticks(ms));
}
