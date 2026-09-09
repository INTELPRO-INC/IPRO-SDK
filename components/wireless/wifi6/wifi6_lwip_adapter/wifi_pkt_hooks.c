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

#include <wifi_pkt_hooks.h>

#include <stdio.h>

wifi_pkt_eth_input_hook_cb_t wifi_pkt_eth_input_hook = NULL;
void *wifi_pkt_eth_input_hook_arg = NULL;

void wifi_pkt_eth_input_hook_register(wifi_pkt_eth_input_hook_cb_t cb, void *cb_arg)
{
    wifi_pkt_eth_input_hook = cb;
    wifi_pkt_eth_input_hook_arg = cb_arg;
}

void wifi_pkt_eth_input_hook_unregister(void)
{
    wifi_pkt_eth_input_hook = NULL;
    wifi_pkt_eth_input_hook_arg = NULL;
}

wifi_pkt_eth_output_hook_cb_t wifi_pkt_eth_output_hook = NULL;
void *wifi_pkt_eth_output_hook_arg = NULL;

void wifi_pkt_eth_output_hook_register(wifi_pkt_eth_output_hook_cb_t cb, void *cb_arg)
{
    wifi_pkt_eth_output_hook = cb;
    wifi_pkt_eth_output_hook_arg = cb_arg;
}

void wifi_pkt_eth_output_hook_unregister(void)
{
    wifi_pkt_eth_output_hook = NULL;
    wifi_pkt_eth_output_hook_arg = NULL;
}
