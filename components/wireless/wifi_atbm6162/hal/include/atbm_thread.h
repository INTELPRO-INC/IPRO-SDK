/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBMWIFI_THREAD_H
#define ATBMWIFI_THREAD_H
#include "atbm_config.h"

typedef int (*thread_callback)(atbm_void *p_arg, int ret);
typedef struct{
	char *name;
	thread_callback cb;
	atbm_void *p_arg;
	pAtbm_thread_t thread;
	ATBM_BOOL stop;
	int wake_period;
	atbm_atomic_t count;
	atbm_os_wait_queue_head_t wq;
	atbm_os_wait_queue_head_t stopEv;
}atbm_thread_internal_t;

atbm_thread_internal_t *atbm_createThreadInternal(char *name, thread_callback task,atbm_void *p_arg,int prio, int wake_period);
int atbm_stopThreadInternal(atbm_thread_internal_t *thread);
void atbm_wakeupThreadInternal(atbm_thread_internal_t *thread);
#endif //ATBMWIFI_THREAD_H
