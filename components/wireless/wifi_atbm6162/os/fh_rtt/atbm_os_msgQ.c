
/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/
#include <stdio.h>
#include "atbm_hal.h"
#include "atbm_os_msgQ.h"

atbm_int8 atbm_os_MsgQ_Create(atbm_os_msgq *pmsgQ, atbm_uint32 *pstack, atbm_uint32 item_size, atbm_uint32 item_num){
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	status = rt_mq_init(pmsgQ,"wifi_msgq",pstack,4,item_size*item_num,RT_IPC_FLAG_FIFO);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Create create failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Create create null \n");
		ret = WIFI_ERROR;
		goto error;
	}
	
error:
	return ret;
}

atbm_int8 atbm_os_MsgQ_Delete(atbm_os_msgq *pmsgQ)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Delete error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	
	//Delete MessageQ
	status = rt_mq_detach(pmsgQ);//rt_mq_delete(pmsgQ);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Delete delete failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

atbm_int8 atbm_os_MsgQ_Recv(atbm_os_msgq *pmsgQ, atbm_void *pbuf, atbm_uint32 val, int timeout)
{
	atbm_int8 ret = WIFI_OK;
	atbm_uint32 status;
	atbm_uint32 data;

	val = val;//No used

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Get error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	//Receive MessageQ
	status = rt_mq_recv (pmsgQ, &data, val,timeout);
	//wifi_printk(WIFI_DBG_MSG,"%s %d 0x%x\n",__func__,__LINE__,data);
	if(status != 0){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Get recv failed 0x%x\n", status);
		ret = WIFI_ERROR;
		goto error;
	}	

	atbm_memcpy(pbuf, &data, 4);
error:
	return ret;

}

atbm_int8 atbm_os_MsgQ_Send(atbm_os_msgq *pmsgQ, atbm_void *pbuf, atbm_uint32 val, int timeout)
{
	atbm_int8 ret = WIFI_OK;
	int status;

	val = val; //No used
	timeout = timeout; //No used

	if(pmsgQ == ATBM_NULL){
		wifi_printk(WIFI_DBG_MSG,"atbm_os_MsgQ_Put error\n");
		ret = WIFI_ERROR;
		goto error;
	}
	//Send MessageQ, and no wait
	status = rt_mq_send(pmsgQ, pbuf,val);
	if (0 != status){
		wifi_printk(WIFI_ALWAYS, "atbm_os_MsgQ_Put send failed %d\n", status);
		ret = WIFI_ERROR;
		goto error;
	}

error:
	return ret;

}

////

