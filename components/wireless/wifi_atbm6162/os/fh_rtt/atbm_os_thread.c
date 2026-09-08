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

#include "atbm_hal.h"
#include "atbm_os_thread.h"
#include "rtdef.h"

typedef void *os_thread_arg_t;
typedef void (*os_pthread) (os_thread_arg_t argument); 
#define WIFI_TASK_TICK 5
#define WIFI_TASK_NAME_LEN 16
#define WIFI_TASK_NUM  8
typedef struct os_thread_def  {
	const char name[WIFI_TASK_NAME_LEN];
	os_pthread entry;
	atbm_uint32 stack_size;
	atbm_uint8 priority;
	atbm_uint32 tick;
} AtbmThreadDef_t;

AtbmThreadDef_t ATBMTHREAD[WIFI_TASK_NUM]={0};
static atbm_uint32 TaskNum=0;


#define MAX_WIFI_TASK 6
#define ATBM_BH_PRO_BASE       (10)
#define WIFI_GENERAL_STACK_SIZE 1024*6

//static int b_first_init =0;
pAtbm_thread_t atbm_createThread(char *name, atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio)
{
	pAtbm_thread_t thread;	

	thread = rt_thread_create(name, task, p_arg, 8192, prio, WIFI_TASK_TICK);
	 if (thread != RT_NULL)
		 rt_thread_startup(thread);

	 return thread;
}


int atbm_stopThread(pAtbm_thread_t thread_id)
{	
	atbm_uint32 result;
	result = rt_thread_delete(thread_id);
	if (result == RT_EOK)
		return 0;
	else
		return -1;
}

atbm_uint32 atbm_getThreadStackFreesize(){
	/* IPRO7 port: current task's minimum free stack (bytes) via the FreeRTOS
	 * high-water mark, instead of RT-Thread rt_thread struct internals. */
	return (atbm_uint32)(uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t));
}

int atbm_ThreadStopEvent(pAtbm_thread_t thread_id)
{
	//Not used
	return 0;
}

atbm_uint32 atbm_getCurThreadId(void)
{
	return (atbm_uint32)rt_thread_self();
}


