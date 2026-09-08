/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBM_OS_MEM_H
#define ATBM_OS_MEM_H
//#include "sys_MsWrapper_cus_os_mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "command.h"
//#include "os_malloc.h"
/* IPRO7 port: back the ATBM heap with the FreeRTOS managed heap (heap_5) rather
 * than newlib malloc/free, so Wi-Fi allocations share the SDK heap regions. */
#include "rtthread.h"

extern void *atbm_ak_wifi_zalloc(unsigned int size);
extern void *atbm_realloc(void *ptr, atbm_size_t size);

#define atbm_kmalloc(x,y) pvPortMalloc(x)
#define atbm_kzalloc(x,y) atbm_ak_wifi_zalloc(x)
#define atbm_kfree(x)   vPortFree(x)
#define atbm_memcpy         memcpy 
#define atbm_memset         memset 
#define atbm_memmove  		memmove
#define atbm_memcmp 		memcmp
#define atbm_calloc			calloc

static inline void * atbm_realloc_array(void *ptr, size_t nmemb, size_t size)
{
	if (size && nmemb > (~(size_t) 0) / size)
		return NULL;
	return atbm_realloc(ptr, nmemb * size);
}


#endif /* ATBM_OS_MEM_H */
