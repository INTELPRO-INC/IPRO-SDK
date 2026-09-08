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
 * USB device event dispatcher shim for usb_console.
 *
 * cherryusb calls a single weak usbd_event_handler(event) per binary; this shim
 * provides a STRONG one that routes to a registered callback, so usb_console
 * doesn't hard-own the handler symbol. Identical API to the OTA shim
 * (apps/platform/common/ota/ota_usb_event.h) -- the symbols are interchangeable.
 *
 * Compile usb_console_event.c ONLY in apps that don't already provide a strong
 * usbd_event_handler + usbd_event_register_callback (e.g. usb_demo's
 * usbd_event_common.c, or the OTA shim). usb_console.c only needs this header;
 * the definition comes from whichever shim the app links.
 */
#ifndef USB_CONSOLE_EVENT_H
#define USB_CONSOLE_EVENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*usbd_event_callback_t)(uint8_t event);

void usbd_event_register_callback(usbd_event_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* USB_CONSOLE_EVENT_H */
