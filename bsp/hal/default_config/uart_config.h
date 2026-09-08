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

#ifndef _UART_CONFIG_H
#define _UART_CONFIG_H

#define UART_CTS_FLOWCONTROL_ENABLE  (0)
#define UART_RTS_FLOWCONTROL_ENABLE  (0)
#define UART_RX_DEGLITCH_ENABLE      (0)
#define UART_MSB_FIRST_ENABLE        (0)
#define UART_TX_SWCONTROL_ENABLE     (0)
#define UART_TX_LINMODE_ENABLE       (0)
#define UART_RX_LINMODE_ENABLE       (0)
#define UART_TX_BREAKBIT_CNT         (0)

#define UART_FIFO_MAX_LEN        128
#define UART_DEFAULT_RTO_TIMEOUT 100

#endif