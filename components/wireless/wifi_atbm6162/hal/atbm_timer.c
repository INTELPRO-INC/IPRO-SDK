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
#include "atbm_os_timer.h"
#include "atbm_timer.h"
static struct wpa_timer timer_table[ELOOP_TIMER_TABLE_SIZE];
static struct atbm_list_head atbm_timeout_free_list;
static struct atbm_list_head atbm_timeout_pennding_list;
static struct atbm_list_head atbm_timeout_ready_list;
static atbm_spinlock_t atbm_timer_mutex;
atbm_os_msgq atbm_timer_msgq;
static atbm_uint32 atbm_timer_msgq_buf[ELOOP_TIMER_TABLE_SIZE];
atbm_void eloop_timeout_func(atbm_void *data1);

static int atbmwifi_wpa_timer_int(struct atbmwifi_common *hw_priv)
{
	atbm_uint8 index = 0;
	atbm_memset(timer_table,0,sizeof(timer_table));
	atbm_spin_lock_init(&atbm_timer_mutex);
	
	ATBM_INIT_LIST_HEAD(&atbm_timeout_free_list);
	ATBM_INIT_LIST_HEAD(&atbm_timeout_pennding_list);
	ATBM_INIT_LIST_HEAD(&atbm_timeout_ready_list);
	
	for(index = 0;index<ELOOP_TIMER_TABLE_SIZE;index++)
	{
		atbm_InitTimer(&timer_table[index].timer, (TIMER_CALLBACK)eloop_timeout_func,(atbm_void*)(&timer_table[index]));		
		atbm_list_add_tail(&(timer_table[index].list), &atbm_timeout_free_list);
	}

	return 0;
}

int atbm_timer_init(struct atbmwifi_common *hw_priv)
{
	return atbmwifi_wpa_timer_int(hw_priv);
}

static int atbm_timer_task(atbm_void *arg, int wakeup_state)
{
	atbm_uint32 pdata;
	struct wpa_timer *ready_timer = NULL;
	struct atbmwifi_common *hw_priv=(struct atbmwifi_common*)arg;

	while(1){
		if(atbm_os_MsgQ_Recv(&atbm_timer_msgq, &pdata, sizeof(atbm_void *), ATBM_TX_WAIT_FOREVER)){
			if (hw_priv->timer_term){
				break;
			}
			continue;
		}
		ready_timer = (struct wpa_timer *)pdata;
		
		if (hw_priv->timer_term){
			wifi_printk(WIFI_ALWAYS,"exit %s()\n", __func__);
			break;
		}
		
		if(ready_timer == ATBM_NULL)
			continue;

		atbm_timer_lock();
		if(ready_timer->cancled){
			atbm_list_move_tail(&ready_timer->list,&atbm_timeout_free_list);
			atbm_timer_unlock();
			continue;
		}
		atbm_timer_unlock();
		ready_timer->fun(ready_timer->data1,ready_timer->data2);
		atbm_timer_lock();
		atbm_list_move_tail(&ready_timer->list,&atbm_timeout_free_list);
		atbm_timer_unlock();
	}

	return 0;
}

int atbm_create_timerTask(struct atbmwifi_common *hw_priv)
{
	int ret,status;

	status=atbm_os_MsgQ_Create(&atbm_timer_msgq,&atbm_timer_msgq_buf[0],sizeof(atbm_uint32),ELOOP_TIMER_TABLE_SIZE);
	if(status != ATBM_TX_SUCCESS){
		wifi_printk(WIFI_ALWAYS, "atbm_timer_msgq create failed 0x%x\n", status);
		return WIFI_ERROR;
	}

	hw_priv->timer_term = 0;
	hw_priv->timer_thread=atbm_createThreadInternal("atbm_timer",
												atbm_timer_task,
												(atbm_void*)hw_priv,
												ELOOP_TASK_PRIO,
												0);
	if (!hw_priv->timer_thread){
		wifi_printk(WIFI_IF,"timer_thread Failed\n");
		ret = WIFI_ERROR;
	}else{
		ret = WIFI_OK;
	}

	return ret;
}

atbm_void eloop_timeout_func(atbm_void * userdata1)
{
	atbm_int8 ret;
	struct wpa_timer *pending_timer = (struct wpa_timer *)userdata1;

	if(pending_timer == ATBM_NULL){
		wifi_printk(WIFI_DBG_ERROR,"%s %d error\n",__func__,__LINE__);
		return;
	}

	//atbm_timer_lock();
	atbm_list_move_tail(&pending_timer->list, &atbm_timeout_ready_list);
	//atbm_timer_unlock();

	ret = atbm_os_MsgQ_Send(&atbm_timer_msgq, &userdata1, sizeof(void *), ATBM_TX_NO_WAIT);
	if(ret){
		wifi_printk(WIFI_DBG_ERROR,"%s Atbm SendMsg Error\n", __func__);
	}

	return;
}

int atbmwifi_eloop_register_timeout(unsigned int secs, unsigned int msecs,
			   atbm_void (*handler)(atbm_void *eloop_ctx, atbm_void *timeout_ctx),
			   atbm_void *eloop_data, atbm_void *user_data)
{
	struct wpa_timer *timer_free = ATBM_NULL;
	//atbm_uint8 start_timer = 1;
	atbm_int32 ret = 0;

	if(ATBM_NULL == handler)
	{
		return 0;
	}
	
	atbm_timer_lock();
	if (!(atbm_list_empty(&atbm_timeout_free_list))){
		timer_free = atbm_list_first_entry(
			&atbm_timeout_free_list, struct wpa_timer, list);
		
		timer_free->data1 = eloop_data;
		timer_free->data2 = user_data;
		timer_free->fun = handler;
		timer_free->cancled = 0;
		atbm_list_move_tail(&timer_free->list,&atbm_timeout_pennding_list);	
		if(timer_free->data1==ATBM_NULL){
		}else{
		}
		ret = 0;
	}else{
		ret = -1;
	}
	atbm_timer_unlock();
	if(ret == 0){	
		atbm_StartTimer(&timer_free->timer,(/*ELOOP_TIMER_OUT*/1000*secs+msecs));
	}else{
		wifi_printk(WIFI_WPA,"atbmwifi_eloop_register_timeout:err\n");
	}

	return ret;
}

int atbmwifi_eloop_cancel_timeout(atbm_void (*handler)(atbm_void *eloop_ctx, atbm_void *sock_ctx),
			 atbm_void *eloop_data, atbm_void *user_data)
{
	struct wpa_timer *found = ATBM_NULL;
	unsigned long flags;

	atbm_timer_lock_irqsave(&flags);
	ATBM_CANCLE_TIMER_FROM_LIST(&atbm_timeout_pennding_list,handler,eloop_data,user_data,found);
	if(found){
		atbm_CancelTimer(&found->timer);
		atbm_list_move_tail(&found->list,&atbm_timeout_free_list);
	}else{
		ATBM_CANCLE_TIMER_FROM_LIST(&atbm_timeout_ready_list,handler,eloop_data,user_data,found);
	}
	atbm_timer_unlock_irqrestore(flags);

	if(found == 0){
		wifi_printk(WIFI_DBG_MSG,"atbmwifi_eloop_cancel_timeout: not found\n");
	}
	return 0;
}
atbm_void atbm_timer_free(struct atbmwifi_common *hw_priv)
{
	atbm_uint32 index;

	hw_priv->timer_term = 1;
	atbm_os_MsgQ_Send(&atbm_timer_msgq, &index, sizeof(atbm_void *), ATBM_TX_WAIT_FOREVER);
	atbm_stopThreadInternal(hw_priv->timer_thread);
	atbm_os_MsgQ_Delete(&atbm_timer_msgq);
	ATBM_FREE_TIMER_FORM_LIST(&atbm_timeout_pennding_list);
	ATBM_FREE_TIMER_FORM_LIST(&atbm_timeout_ready_list);
	for(index = 0;index<ELOOP_TIMER_TABLE_SIZE;index++){
		atbm_FreeTimer(&timer_table[index].timer); 
	}

}

