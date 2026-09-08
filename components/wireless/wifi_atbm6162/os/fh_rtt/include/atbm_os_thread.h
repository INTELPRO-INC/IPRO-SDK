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

#ifndef ATBM_OS_THREAD_H
#define ATBM_OS_THREAD_H

#define WIFI_TASK_TICK 5
#define WIFI_TASK_NUM  10
#define WIFI_TASK_NAME_LEN 16
#define WIFI_THREAD_PRIO_START 80
#define BH_TASK_PRIO (WIFI_THREAD_PRIO_START)	 /** High **/
#define HIF_TASK_PRIO (WIFI_THREAD_PRIO_START + 3)
#define WORK_TASK_PRIO (WIFI_THREAD_PRIO_START + 4)
#define ELOOP_TASK_PRIO (WIFI_THREAD_PRIO_START + 5)
#define TEST_TASK_PRIO (WIFI_THREAD_PRIO_START + 6)
#define TXURB_TASK_PRIO (WIFI_THREAD_PRIO_START + 1)
#define RXURB_TASK_PRIO (WIFI_THREAD_PRIO_START + 2)
#define TX_BH_TASK_PRIO (WIFI_THREAD_PRIO_START + 1)
#define RX_BH_TASK_PRIO (WIFI_THREAD_PRIO_START + 2)

#define BLE_XMIT_PRIO (WIFI_THREAD_PRIO_START + 5)
#define BLE_RECV_PRIO (WIFI_THREAD_PRIO_START + 5)

#if ATBM_SUPPORT_SMARTCONFIG
#define SMARTCONFIG_MONNITOR_TASK_PRIO (WIFI_THREAD_PRIO_START + 4)
#endif
#define MAIL_BOX_BH_PRIO     (WIFI_THREAD_PRIO_START -1)

typedef struct rt_thread *pAtbm_thread_t;

//typedef atbm_void * pAtbm_thread_t ;

/*priotry */
enum  ATBM_THREAD_PRIO
{
	//WORK_TASK_PRIO,
	//BH_TASK_PRIO,
	//ELOOP_TASK_PRIO,
	HIF_TASK_PRIO1,
};

pAtbm_thread_t atbm_createThread(char *name, atbm_void(*task)(atbm_void *p_arg),atbm_void *p_arg,int prio);
int atbm_stopThread(pAtbm_thread_t thread_id);
int atbm_ThreadStopEvent(pAtbm_thread_t thread_id);
atbm_uint32 atbm_getCurThreadId(void);


#endif /* ATBM_OS_THREAD_H */
