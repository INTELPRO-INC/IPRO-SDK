/**************************************************************************************************************
 * altobeam RTOS WSM host interface (HI) implementation
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "atbm_hal.h"


static void atbm_ThreadCallback(void *arg){
	atbm_thread_internal_t *thread = (atbm_thread_internal_t *)arg;
	int ret;

	wifi_printk(WIFI_ALWAYS, "Thread %s %d start\n", thread->name, thread->wake_period);
	if(thread->wake_period){
		while (!thread->stop)
		{	
			do{
				if(atbm_atomic_read(&thread->count) != 0)
					break;
				ret = atbm_os_wait_event_timeout(&thread->wq, thread->wake_period);
			}while(0);
			atbm_atomic_set(&thread->count, 0);
			if(thread->cb){
				thread->cb(thread->p_arg, ret);
			}
		}
	}else{
		if(thread->cb){
			thread->cb(thread->p_arg, 0);
		}
	}
	atbm_os_wakeup_event(&thread->stopEv);
	wifi_printk(WIFI_ALWAYS, "Thread %s stop\n", thread->name);
}

atbm_thread_internal_t *atbm_createThreadInternal(char *name,
							thread_callback task,
							atbm_void *p_arg,
							int prio,
							int wake_period){
	atbm_thread_internal_t *thread = atbm_kmalloc(sizeof(atbm_thread_internal_t), GFP_KERNEL);
	if(!thread)
		return ATBM_NULL;
	
	thread->cb = task;
	thread->p_arg = p_arg;
	thread->stop = 0;
	thread->wake_period = wake_period;
	thread->name = name;
	atbm_atomic_set(&thread->count, 0);

	if(wake_period){
		atbm_os_init_waitevent(&thread->wq);
	}

	atbm_os_init_waitevent(&thread->stopEv);	

	thread->thread = atbm_createThread(name,atbm_ThreadCallback,(atbm_void*)thread,prio);

	if(!thread->thread){
		atbm_kfree(thread);
		return ATBM_NULL;
	}
	return thread;
}

int atbm_stopThreadInternal(atbm_thread_internal_t *thread){
	int ret;

	thread->stop = 1;
	if(thread->wake_period){
		atbm_os_wakeup_event(&thread->wq);
	}
	atbm_os_wait_event_timeout(&thread->stopEv, 10*ATBM_HZ);

	ret = atbm_stopThread(thread->thread);
	atbm_os_delete_waitevent(&thread->stopEv);
	if(thread->wake_period){
		atbm_os_delete_waitevent(&thread->wq);
	}

	atbm_kfree(thread);
	return ret;
};


void atbm_wakeupThreadInternal(atbm_thread_internal_t *thread){
	if(thread->wake_period == 0){
		wifi_printk(WIFI_ALWAYS, "wake up %d error\n", thread->name);
		return;
	}
	if(atbm_atomic_add_return(1, &thread->count) == 1){
		atbm_os_wakeup_event(&thread->wq);
	}
};



