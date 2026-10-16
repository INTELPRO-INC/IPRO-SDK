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

/*
 * CherryUSB configuration for ipro_agora_audio (NCM only)
 */
#ifndef CHERRYUSB_CONFIG_H
#define CHERRYUSB_CONFIG_H

#define CHERRYUSB_VERSION 0x001000

/* ================ USB common Configuration ================ */

#define CONFIG_USB_PRINTF(...)  /* disabled: ISR-context printf causes timing issues */

#define usb_malloc(size) malloc(size)
#define usb_free(ptr)    free(ptr)

#ifndef CONFIG_USB_DBG_LEVEL
#define CONFIG_USB_DBG_LEVEL USB_DBG_LOG
#endif

/* #define CONFIG_USB_PRINTF_COLOR_ENABLE */

#ifndef CONFIG_USB_ALIGN_SIZE
#define CONFIG_USB_ALIGN_SIZE 4
#endif

#define USB_NOCACHE_RAM_SECTION __attribute__((section(".noncacheable")))

/* DMA cache operations for PSRAM */
#include "ipro7.h"
#include "core_feature_cache.h"
#define USB_DCACHE_FLUSH(addr, size)  MFlushDCacheRange((unsigned long)(addr), (unsigned long)(size))
#define USB_DCACHE_INVAL(addr, size)  MInvalDCacheRange((unsigned long)(addr), (unsigned long)(size))

/* ================= USB Device Stack Configuration ================ */

#define CONFIG_USBDEV_REQUEST_BUFFER_LEN 512

#define CONFIG_USBDEV_CDC_NCM_USING_LWIP

#endif
