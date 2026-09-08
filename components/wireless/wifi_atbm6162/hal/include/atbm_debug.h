/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBMWIFI_DEBUG_H
#define ATBMWIFI_DEBUG_H
#include "atbm_config.h"
#define ATBM_DEBUG(enable, tagged, ...)						\
	do{									\
		if( enable){							\
			if( tagged)						\
				iot_printf("[ %s() ] ", __FUNCTION__);	\
			iot_printf(__VA_ARGS__);				\
		}							        \
	} while( 0)

#define ATBM_ERROR(...)	ATBM_DEBUG(1, 1, "ERROR:"__VA_ARGS__)
#define ATBM_WARN(...)	ATBM_DEBUG(1, 1, "WARNING:"__VA_ARGS__)
#define ATBM_ASSERT(cond)								\
{										\
	if (!(cond)){								\
		ATBM_ERROR("Failed assert in %s\n:"				\
				"line %d\n",					\
				__FUNCTION__,					\
				__LINE__);	\
		while (1);							\
	}									\
}
/*cond==TRUE will printf WARNING*/
#define ATBM_WARN_ON_FUNC(cond)  						\
{											\
	if ((cond)){							\
		ATBM_WARN(" in %s: line:%d\n",			\
				__FUNCTION__,	\
				__LINE__);	\
	}				\
}
#define ATBM_KPANIC( ...)		\
{					\
	ATBM_ERROR(__VA_ARGS__);	\
	while (1);	\
}
#define ATBM_BUG_ON(cond)  	ATBM_ASSERT(!(cond))
/*cond ATBM_TRUE will error*/
#if ATBM_PLATFORM == UIS8310_THREADX
#define ATBM_WARN_ON(cond)  		(cond < 0) 
#else
#define ATBM_WARN_ON(cond)  		(cond) 
#endif
atbm_void dump_mem(const atbm_void *mem, int count);

/*******************************************/
#define WIFI_TX 		ATBM_BIT(0)
#define WIFI_RX 		ATBM_BIT(1)
#define WIFI_WSM 		ATBM_BIT(2)
#define WIFI_PS 		ATBM_BIT(3)
#define WIFI_QUEUE 		ATBM_BIT(4)
#define WIFI_IF 		ATBM_BIT(5)
#define WIFI_DBG_MSG 	ATBM_BIT(6)
#define WIFI_DBG_ANY 	ATBM_BIT(7)
#define WIFI_DBG_INIT 	ATBM_BIT(8)
#define WIFI_DBG_ERROR 	ATBM_BIT(9)
#define WIFI_WARN_CODE 	ATBM_BIT(10)
#define WIFI_OS 		ATBM_BIT(11)
#define WIFI_CONNECT	ATBM_BIT(12)
#define WIFI_SCAN		ATBM_BIT(13)
#define WIFI_RATE		ATBM_BIT(14)
#define WIFI_ATCMD 		ATBM_BIT(15)
#define WIFI_WPA 		ATBM_BIT(16)
#define WIFI_BH 		ATBM_BIT(17)
#define WIFI_TASK 		ATBM_BIT(18)
#define WIFI_DPLL		ATBM_BIT(19)
#define WIFI_ALWAYS		ATBM_BIT(20)
#define WIFI_SDIO		ATBM_BIT(21)
#define WIFI_WPS		ATBM_BIT(22)
#define WIFI_SAE		ATBM_BIT(23)
#define WIFI_BASETUP	ATBM_BIT(24)
#define WIFI_BLE		ATBM_BIT(25)
#define WIFI_CSA		ATBM_BIT(26)

#define WIFI_DEBUG_LEVEL  (WIFI_BASETUP|WIFI_ATCMD|WIFI_DBG_INIT|WIFI_DBG_ERROR|WIFI_DBG_ANY|WIFI_WARN_CODE|WIFI_CONNECT|WIFI_SCAN|WIFI_WPS|WIFI_WPA)
#define wifi_printk(_level,...) do {if(((_level)==WIFI_ALWAYS) || ((_level)&WIFI_DEBUG_LEVEL)) iot_printf("[atbm_log]:"__VA_ARGS__);} while (0)
#endif //ATBMWIFI_DEBUG_H
