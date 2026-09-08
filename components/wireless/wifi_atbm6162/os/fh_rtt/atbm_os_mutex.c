/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/

#include "rtdef.h"
#include "atbm_hal.h"
//#include "fh_os.h"

#define AK_MUTEX_DEBUG_LEVEL	WIFI_DBG_MSG
#define AK_MUTEX_ERR_LEVEL		WIFI_DBG_ERROR
//------------------------------------------------------------------------------
//  Function    : atbm_os_CreateSem
//  Description :
//------------------------------------------------------------------------------
/** @brief This function creates a semaphore.

@param[in] ubSemValue : is the initial value for the semaphore.  If the value is 0, no resource is
                        available.  You initialize the semaphore to a non-zero value to specify how many resources are available.
@retval 0xFF for create semaphore internal error from OS
		0xFE the system maximum semaphore counts exceed.
		others, the ID to access the semaphore
*/
static inline unsigned long os_msec_to_ticks(unsigned long msecs)
{
	return ((msecs) * (RT_TICK_PER_SECOND)) / 1000;
}

atbm_uint32 atbm_os_CreateSem(atbm_uint8 ubSemValue)
{
	//rsem = mutex_init("");
	const char *name = NULL;
	rt_sem_t ret = rt_sem_create( name, ubSemValue, RT_IPC_FLAG_FIFO);
 
	wifi_printk(WIFI_OS,"-- %x--\n", ret);
    return (rt_sem_t)ret;
}
atbm_int32 atbm_os_init_waitevent(atbm_os_wait_queue_head_t* pulSem)
{	 
	*pulSem = rt_sem_create("waitQueue", 0, RT_IPC_FLAG_PRIO);
	if (*pulSem){
		wifi_printk(WIFI_OS,"OS: Semaphore Init: handle %p\r\n", *pulSem);
		return 0;
	}else{
		ATBM_BUG_ON(1);
		return -1;
	}
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_DeleteSem
//  Description :
//------------------------------------------------------------------------------
/** @brief This function deletes a semaphore and readies all tasks pending on the semaphore.

@param[in] ulSemId : The semaphore ID that return by @ref atbm_os_CreateSem 
@retval 0xFF for delete semaphore internal error from OS
		0, return delete success.
*/

atbm_uint8 atbm_os_DeleteSem(atbm_os_wait_queue_head_t* pulSem)
{
	//iot_printf("%s %d\n",__func__,__LINE__);
	int status = RT_EOK;
	
	//status = AK_Delete_Semaphore(*pulSem);
	status = rt_sem_delete(*pulSem);//rt_mutex_delete(*pulSem);
	
	if(RT_EOK != status)
		return -1;
    return 0;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_wait_event_timeout
//  Description :
//------------------------------------------------------------------------------
/** @brief This function waits for a semaphore.

@param[in] ulSemId : The semaphore ID that return by @ref atbm_os_CreateSem 
@param[in] ulTimeout : is an optional timeout period (in clock ticks).  If non-zero, your task will
                            wait for the resource up to the amount of time specified by this argument.
                            If you specify 0, however, your task will wait forever at the specified
                            semaphore or, until the resource becomes available.
@retval 0xFE for bad input semaphore id,
		0xFF for acquire semaphore internal error from OS
		0 for getting the resource.
		1 for time out happens
*/

atbm_uint8 atbm_os_wait_event_timeout(atbm_os_wait_queue_head_t * pSem, atbm_uint32 ulTimeout)
{
	rt_err_t ret;
	if(*pSem==ATBM_NULL)
		ATBM_BUG_ON(1);
	if(0 == ulTimeout){
	    ret = rt_sem_trytake(*pSem);
	}else if(OS_WAIT_FOREVER == ulTimeout){
	    ret = rt_sem_take(*pSem, RT_WAITING_FOREVER);
	}else{
	    ret = rt_sem_take(*pSem, os_msec_to_ticks(ulTimeout));
	}
	//wifi_printk(WIFI_ALWAYS,"OS: Semaphore take:handle %p ulTimeout/5 is %dms\r\n",*pSem, ulTimeout/5);
	if(ret == RT_EOK)//OS_SUCCESS;
		return 1;
	else if (-RT_ETIMEOUT == ret)//RT_ETIMEOUT;
		return 0;
	else //fail
		return -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_wakeup_event
//  Description :
//------------------------------------------------------------------------------
/** @brief This function signals a semaphore

@param[in] ulSemId : The semaphore ID that return by @ref atbm_os_CreateSem 
@retval 0xFE for bad input semaphore id,
		0xFF for release semaphore internal error from OS
		0 for getting the resource.
		1 If the semaphore count exceeded its limit.
*/
atbm_uint8 atbm_os_wakeup_event(atbm_os_wait_queue_head_t * pSem)
{
	int ret = RT_EOK; 
	
	//ret = AK_Release_Semaphore(*pSem);
	ret = rt_sem_release(*pSem);

	if(RT_EOK != ret){		
		wifi_printk(AK_MUTEX_ERR_LEVEL,"atbm_os_wakeup_event err\n");
		return -1;
	}

	return 0;
}


int atbm_os_wait_event_count_get(atbm_os_wait_queue_head_t *pSem){
	/* IPRO7 port: current count via FreeRTOS instead of rt_semaphore->value. */
	return (int)uxSemaphoreGetCount((*pSem)->h);
}


//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLockInit
//  Description :
//------------------------------------------------------------------------------

atbm_uint32 atbm_os_mutexLockInit(atbm_mutex * pmutex)
{
	*pmutex= rt_mutex_create("AtbMutex", RT_IPC_FLAG_PRIO);

	if(*pmutex){
		return 0;
	}else{
		return -1;
	}
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_DeleteMutex
//  Description :
//------------------------------------------------------------------------------
/** @brief This function deletes a mutual exclusion semaphore and readies all tasks pending on the it.

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@retval 0xFF for delete mutex internal error from OS
		0, return delete success.
*/

atbm_uint8 atbm_os_DeleteMutex(atbm_mutex * pmutex)
{
   // return ak_thread_mutex_destroy(pmutex_id);
   int status = RT_EOK;
	
	//status = AK_Delete_Semaphore(*pmutex_id);
	rt_mutex_delete(*pmutex);
	
	if(RT_EOK != status)
		return -1;
    return 0;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function waits for a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_mutexLockInit 
@param[in] ulTimeout : is an optional timeout period (in clock ticks).  If non-zero, your task will
                            wait for the resource up to the amount of time specified by this argument.
                            If you specify 0, however, your task will wait forever at the specified
                            semaphore or, until the resource becomes available.
@retval 0xFE for bad input mutex id,
		0xFF for acquire mutex internal error from OS
		0 for getting the resource.
		1 for time out happens
*/

atbm_uint8 atbm_os_mutexLock(atbm_mutex * pmutex,atbm_uint32 ulTimeout)
{
	atbm_uint8 ret = -1;
	ulTimeout=RT_WAITING_FOREVER;

	if(0 == ulTimeout)
		ret = rt_mutex_take(*pmutex, 0);
	else if(RT_WAITING_FOREVER == ulTimeout)
		ret = rt_mutex_take(*pmutex, RT_WAITING_FOREVER);
	else
		ret = rt_mutex_take(*pmutex, ulTimeout);

	return ret == RT_EOK ? 0 : -1;
}

//------------------------------------------------------------------------------
//  Function    : atbm_os_mutexUnLock
//  Description :
//------------------------------------------------------------------------------
/** @brief This function signals a mutual exclusion semaphore

@param[in] ulMutexId : The mutex ID that return by @ref atbm_os_CreateSem 
@retval 0xFE for bad input mutex id,
		0xFF for release mutex internal error from OS
		0 for getting the resource.
*/
atbm_uint8 atbm_os_mutexUnLock(atbm_mutex * pmutex_id)
{
	atbm_uint8 ret;

	ret = rt_mutex_release(*pmutex_id);
	return ret == RT_EOK ? 0 : -1;
}

