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
 * Minimal USB device event dispatcher for ota_usb.
 *
 * cherryusb calls a single `usbd_event_handler(event)` hook per binary; this
 * routes it to a registered callback. ota_usb registers its callback here.
 *
 * Only needed by apps that DON'T already provide usbd_event_handler. Apps that
 * do (e.g. anything pulling in usb_demo's usbd_event_common.c) must NOT also
 * compile ota_usb_event.c -- it would duplicate the usbd_event_handler symbol.
 * ota_usb.c only needs this header (the declaration); the definition comes from
 * whichever dispatcher the app links.
 */
#ifndef OTA_USB_EVENT_H
#define OTA_USB_EVENT_H

#include <stdint.h>

typedef void (*usbd_event_callback_t)(uint8_t event);

void usbd_event_register_callback(usbd_event_callback_t callback);

#endif /* OTA_USB_EVENT_H */
