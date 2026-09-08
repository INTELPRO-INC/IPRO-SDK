/**************************************************************************************************************
 * altobeam RTOS wifi hmac source code 
 *
 * Copyright (c) 2018, altobeam.inc   All rights reserved.
 *
 *  The source code contains proprietary information of AltoBeam, and shall not be distributed, 
 *  copied, reproduced, or disclosed in whole or in part without prior written permission of AltoBeam.
*****************************************************************************************************************/


#ifndef ATBM_OS_API_H
#define ATBM_OS_API_H

//#include "drv_api.h"
//#include "platform_devices.h"
//#include "dev_drv.h"
//#include "drv_gpio.h"

//#include <drvErrno.h>
#include "atbm_type.h"
#if ATBM_SDIO_BUS
#include "atbm_os_sdio.h"
#endif
#if ATBM_USB_BUS
#include "atbm_os_usb.h"
#endif
#define HZ  1000
typedef enum
{
    INTERFACE_NOT_SD,
    INTERFACE_SDMMC4,
    INTERFACE_SDMMC8,
    INTERFACE_SDIO,
    INTERFACE_SDIO2
}
T_eCARD_INTERFACE;
typedef enum _BUS_MODE
{
    USE_ONE_BUS,
    USE_FOUR_BUS,
    USE_EIGHT_BUS
}T_eBUS_MODE;
/* IPRO7 port: lwIP (CONFIG_LWIP) provides struct sockaddr (lwip/sockets.h) with the
 * same layout; suppress the vendor's to avoid a redefinition clash. Unused in the
 * compiled STA sources. */
#ifndef CONFIG_LWIP
struct sockaddr {
  u8_t		  sa_len;
  u8_t		 sa_family;
  char		  sa_data[14];
};
#endif
#ifndef atbm_packed
#define atbm_packed __attribute__ ((packed))
#endif //__packed

struct __una_u16 { atbm_uint16 x; } atbm_packed ;
struct __una_u32  { atbm_uint32 x; } atbm_packed ;

static __INLINE atbm_uint16 __get_unaligned_cpu16(const atbm_void *p)
{
 	const struct __una_u16 *ptr = (const struct __una_u16 *)p;
 	return ptr->x;
}
 
static __INLINE  atbm_uint32 __get_unaligned_cpu32 (const atbm_void *p)
{
	 const struct __una_u32  *ptr = (const struct __una_u32  *)p;
	 return ptr->x;
}

//static __INLINE uint16 get_unaligned_le16(const void *p)
//{
// 	return __get_unaligned_cpu16((const atbm_uint8 *)p);
//}
 
static __INLINE atbm_uint32 get_unaligned_le32(const atbm_void *p)
{
	return __get_unaligned_cpu32 ((const atbm_uint8 *)p);
}

int atbmwifi_event_OsCallback(atbm_void *prv,int eventid,atbm_void *param);
#if ATBM_USB_BUS
int atbm_usb_register_init();
int atbm_usb_register_deinit();
#endif

#endif //ATBM_OS_API_H
