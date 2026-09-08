/**
 * @file hal_emac.h
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

#ifndef __HAL_EMAC_H__
#define __HAL_EMAC_H__

#include "hal_common.h"
#include "drv_emac.h"
#include "ethernet_phy.h"

#define SPEED_SEL_MODE  1   // 1: 10/100Mbps,  0: force 10Mbps
#define CTX_TYPE 1

// MSG is already defined in platform_common.h
// #define MSG_DBG(a,...)          printf(a,##__VA_ARGS__)
// #define MSG_WAR(a,...)          printf(a,##__VA_ARGS__)
// #define MSG_ERR(a,...)          printf(a,##__VA_ARGS__)

typedef struct emac_device {
    uint8_t mac_addr[6]; /*!< mac address */
} emac_device_t;

#define EMAC_NORMAL_PACKET    (uint32_t)(0)
#define EMAC_FRAGMENT_PACKET  (uint32_t)(0x01)
#define EMAC_NOCOPY_PACKET    (uint32_t)(0x02)

#ifndef ETH_TX_BUFFER_SIZE
#define ETH_TX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif

#ifndef ETH_RX_BUFFER_SIZE
#define ETH_RX_BUFFER_SIZE (ETH_MAX_PACKET_SIZE)
#endif

#define EMAC_TX_COMMON_FLAGS (EMAC_BD_FIELD_MSK(TX_RD) |  \
                              EMAC_BD_FIELD_MSK(TX_IRQ) | \
                              EMAC_BD_FIELD_MSK(TX_PAD) | \
                              EMAC_BD_FIELD_MSK(TX_CRC) | \
                              EMAC_BD_FIELD_MSK(TX_EOF))

#define EMAC_RX_COMMON_FLAGS    (ETH_MAX_PACKET_SIZE << 16) | \
    EMAC_BD_FIELD_MSK(RX_IRQ) )

typedef enum _BD_TYPE_ {
    EMAC_BD_TYPE_INVLAID,
    EMAC_BD_TYPE_TX,
    EMAC_BD_TYPE_RX,
    EMAC_BD_TYPE_NONE,
    EMAC_BD_TYPE_MAX = 0x7FFFFFFF
} EMAC_BD_TYPE_e;

int emac_init(emac_device_t *emac_cfg);
int emac_bd_init(uint8_t *eth_tx_buff, uint8_t tx_buf_count, uint8_t *eth_rx_buff, uint8_t rx_buf_count);
int emac_bd_fragment_support(void);
int emac_bd_tx_enqueue(uint32_t flags, uint32_t len, const uint8_t *data_in);
int emac_bd_rx_dequeue(uint32_t flags, uint32_t *len, uint8_t *data_out);
__WEAK void emac_rx_done_callback_app(void);
__WEAK void emac_rx_error_callback_app(void);
__WEAK void emac_rx_busy_callback_app(void);
__WEAK void emac_tx_error_callback_app(void);
__WEAK void emac_tx_done_callback_app(void);
int emac_phy_set_address(uint16_t phyAddress);
int emac_phy_config_full_duplex(uint8_t fullDuplex);
int emac_phy_reg_read(uint16_t phyReg, uint16_t *regValue);
int emac_phy_reg_write(uint16_t phyReg, uint16_t regValue);
int emac_stop(void);
int emac_start(void);
int emac_start_tx(void);
int emac_stop_tx(void);
int emac_start_rx(void);
int emac_stop_rx(void);
int emac_intmask(EMAC_INT_Type intType, int intMask);
int emac_get_fram_len(uint16_t *max, uint16_t *min);
int EMAC_GetFramLen(EMAC_ID_Type emacId, uint16_t * max, uint16_t *min);
int EMAC_GetBD(EMAC_ID_Type emacId, uint32_t *bd);
#endif
