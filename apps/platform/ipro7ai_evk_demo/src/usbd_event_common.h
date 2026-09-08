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

#ifndef USBD_EVENT_COMMON_H
#define USBD_EVENT_COMMON_H

#include <stdint.h>

/**
 * @brief USB device event handler callback type
 * @param event USB event code
 */
typedef void (*usbd_event_callback_t)(uint8_t event);

/**
 * @brief Register USB device event handler callback
 * @param callback Event handler callback function
 * @note Only one callback can be registered at a time.
 *       The newer registration will overwrite the previous one.
 */
void usbd_event_register_callback(usbd_event_callback_t callback);

#endif /* USBD_EVENT_COMMON_H */
