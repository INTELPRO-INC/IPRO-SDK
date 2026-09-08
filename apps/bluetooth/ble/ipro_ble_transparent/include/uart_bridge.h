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

#ifndef __UART_BRIDGE_H__
#define __UART_BRIDGE_H__

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BRIDGE_MODE_COMMAND = 0,
    BRIDGE_MODE_DATA,
} bridge_mode_t;

/* Transport-agnostic bridge API */
int uart_bridge_init(void);
int uart_bridge_send(const uint8_t *data, uint16_t len);
int uart_bridge_send_str(const char *str);
bridge_mode_t uart_bridge_get_mode(void);
int uart_bridge_set_mode(bridge_mode_t mode);

/* UART-specific (no-op when CDC) */
int uart_bridge_set_baudrate(uint32_t baudrate);
uint32_t uart_bridge_get_baudrate(void);

/* Transport name for status display */
const char *uart_bridge_get_transport_name(void);

#endif /* __UART_BRIDGE_H__ */
