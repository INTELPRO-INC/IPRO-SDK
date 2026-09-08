/**
 * @file ethernetif.h
 * @brief
 *
 * Copyright (c) 2021 IntelPro team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "hal_emac.h"
#if CTX_TYPE
#include <utils_list.h>
#endif
#include "lwip/err.h"
#include "lwip/netif.h"
#include "ethernet_phy.h"

#define DHCP_OFF              (uint8_t)0
#define DHCP_START            (uint8_t)1
#define DHCP_WAIT_ADDRESS     (uint8_t)2
#define DHCP_ADDRESS_ASSIGNED (uint8_t)3
#define DHCP_TIMEOUT          (uint8_t)4
#define DHCP_LINK_DOWN        (uint8_t)5

#if CTX_TYPE
#define ETH_MAX_BUFFER_SIZE        (ETH_MAX_PACKET_SIZE)

struct unsent_item{
    struct utils_list_hdr hdr;
    struct pbuf *p;
};

typedef struct {
    struct utils_list unsent;
    uint8_t Tx_free_bd_num;
    uint8_t Rx_free_bd_num;
    volatile uint16_t unsent_num;
    uint32_t tx_pkt_cnt;
    uint32_t rx_pkt_cnt;
    uint32_t out_tmr;
    uint32_t done_tmr;
}eth_context;
#endif
/* Exported types ------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
void ethernet_link_check_state(struct netif *netif);
void ethernet_link_status_updated(struct netif *netif);

#endif
