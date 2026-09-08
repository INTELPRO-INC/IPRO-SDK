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
 * Strong usbd_event_handler dispatcher for usb_console (see usb_console_event.h).
 * Compile ONLY in apps that don't already provide one (usb_demo / OTA shim).
 */
#include <generated/autoconf.h>

#if defined(CONFIG_DEBUG_CONSOLE_USB)

#include "usb_console_event.h"
#include "usbd_core.h"

static usbd_event_callback_t g_usb_console_event_cb;

void usbd_event_register_callback(usbd_event_callback_t callback)
{
    g_usb_console_event_cb = callback;
}

/* Overrides cherryusb's __WEAK usbd_event_handler; dispatch to the registered cb. */
void usbd_event_handler(uint8_t event)
{
    if (g_usb_console_event_cb != NULL)
        g_usb_console_event_cb(event);
}

#endif /* CONFIG_DEBUG_CONSOLE_USB */
