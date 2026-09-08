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

#ifndef _ATBM_TIMER_H
#define _ATBM_TIMER_H
#define ATBM_TX_WAIT_FOREVER 0xffffffff
#define ATBM_TX_NO_WAIT 0
#define ATBM_TX_SUCCESS 0
struct wpa_timer
{
	struct atbm_list_head list;
	atbm_void (*fun) (atbm_void *data1,atbm_void *data2);
	atbm_void *data1;
	atbm_void *data2;
	atbm_uint32 cancled;
//	struct atbm_timer_list *timer;
	OS_TIMER  timer;
};
#define  ELOOP_TIMER_OUT			(ATBM_HZ)		
#define  ELOOP_TIMER_TABLE_SIZE		(16)

#define atbm_timer_lock() 		atbm_spin_lock_bh(&atbm_timer_mutex)
#define atbm_timer_unlock()		atbm_spin_unlock_bh(&atbm_timer_mutex)
#define atbm_timer_lock_irqsave(flags) 		atbm_spin_lock_irqsave(&atbm_timer_mutex, flags)
#define atbm_timer_unlock_irqrestore(flags)		atbm_spin_unlock_irqrestore(&atbm_timer_mutex, flags)

#define ATBM_CANCLE_TIMER_FROM_LIST(_list,_func,_data1,_data2,_found)										\
	do{																										\
		struct wpa_timer *timer_cancle,*temp;																\
		atbm_list_for_each_entry_safe(timer_cancle,temp,(_list),list,struct wpa_timer){						\
			if(timer_cancle->fun != _func)																	\
				continue;																					\
			if(timer_cancle->data1 != _data1)																\
				continue;																					\
			if(timer_cancle->data2 != _data2)																\
				continue;																					\
			_found = timer_cancle;																						\
			timer_cancle->cancled = 1;																		\
		}																									\
	}while(0)

#define ATBM_FREE_TIMER_FORM_LIST(_list)																	\
	do{																										\
		struct wpa_timer *free_timer = ATBM_NULL;															\
		while(!atbm_list_empty((_list))){																	\
			free_timer = atbm_list_first_entry((_list),struct wpa_timer, list);								\
			free_timer->cancled = 1;																		\
			atbm_list_move_tail(&free_timer->list,&atbm_timeout_free_list);									\
			atbm_CancelTimer(&free_timer->timer);                                                          \
		}																									\
	}while(0)
	
int atbm_timer_init(struct atbmwifi_common * hw_priv);
atbm_void atbm_timer_free(struct atbmwifi_common * hw_priv);
int atbmwifi_eloop_cancel_timeout(atbm_void (*handler)(atbm_void *eloop_ctx, atbm_void *sock_ctx),
			 atbm_void *eloop_data, atbm_void *user_data);
int atbmwifi_eloop_register_timeout(unsigned int secs, unsigned int msecs,
			   atbm_void (*handler)(atbm_void *eloop_ctx, atbm_void *timeout_ctx),
			   atbm_void *eloop_data, atbm_void *user_data);
#endif //_ATBM_TIMER_H
