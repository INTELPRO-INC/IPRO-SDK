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

#pragma once

#include <stdbool.h>

typedef void *(*wifi_pkt_eth_input_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern wifi_pkt_eth_input_hook_cb_t wifi_pkt_eth_input_hook;
extern void *wifi_pkt_eth_input_hook_arg;

/**
 * @brief  Register a callback that is called before an eth packet is passed to TCP/IP stack.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                The first argument passed to cb is a struct pbuf *, second being cb_arg.
 *                cb should return NULL if cb drops the packet(cb might need to free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be passed to TCP/IP stack.
 * @param cb_arg  Callback argument.
 */
void wifi_pkt_eth_input_hook_register(wifi_pkt_eth_input_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void wifi_pkt_eth_input_hook_unregister(void);

typedef void *(*wifi_pkt_eth_output_hook_cb_t)(bool is_sta, void *pkt, void *arg);

extern wifi_pkt_eth_output_hook_cb_t wifi_pkt_eth_output_hook;
extern void *wifi_pkt_eth_output_hook_arg;


/**
 * @brief  Register a callback that is called before an eth packet from TCP/IP stack is sent by Wi-Fi.
 *
 * @param cb      Callback. cb may inspect, manipulate or even duplicate the packet.
 *                cb should return NULL if it drops the packet(cb should NOT free it if so).
 *                If non-NULL is returned by cb, the returned pbuf will be sent by Wi-Fi.
 * @param cb_arg  Callback argument.
 */
void wifi_pkt_eth_output_hook_register(wifi_pkt_eth_output_hook_cb_t cb, void *cb_arg);

/**
 * @brief  Unregister the callback previously registered.
 */
void wifi_pkt_eth_output_hook_unregister(void);
