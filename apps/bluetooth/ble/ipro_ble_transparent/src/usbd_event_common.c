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

#include "usbd_event_common.h"
#include "usbd_core.h"

static usbd_event_callback_t g_usbd_event_callback = NULL;

void usbd_event_register_callback(usbd_event_callback_t callback)
{
    g_usbd_event_callback = callback;
}

void usbd_event_handler(uint8_t event)
{
    if (g_usbd_event_callback != NULL) {
        g_usbd_event_callback(event);
    }
}
