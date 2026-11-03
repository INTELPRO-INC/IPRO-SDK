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

/* Private variable to store the registered callback */
static usbd_event_callback_t g_usbd_event_callback = NULL;

/**
 * @brief Register USB device event handler callback
 * @param callback Event handler callback function
 */
void usbd_event_register_callback(usbd_event_callback_t callback)
{
    g_usbd_event_callback = callback;
}

/**
 * @brief Common USB device event handler
 * @param event USB event code
 * @note This function is called by the USB stack and dispatches to the registered callback
 */
void usbd_event_handler(uint8_t event)
{
    if (g_usbd_event_callback != NULL) {
        g_usbd_event_callback(event);
    }
}
