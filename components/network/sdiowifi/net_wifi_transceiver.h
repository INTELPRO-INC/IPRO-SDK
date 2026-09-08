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

#ifndef __NET_WIFI_TRANSCEIVER_H__
#define __NET_WIFI_TRANSCEIVER_H__
#include <stdint.h>
#include <stdbool.h>
#include <sdiowifi_platform_adapt.h>
#include <lwip/netif.h>
#include <lwip/pbuf.h>

#define IPRO_MSG_TYPE_ETH_WIFI_FRAME 0xabcd
#define IPRO_MSG_TYPE_CMD            0xef01

/*
 * 0  -> tx credit lo byte (was: tx desc cnt, uint8)
 * 1..6-> mac
 * 7  -> boot src info
 * 1  -> tty0 rcv buf rem (reuse, after MAC handshake done)
 * 2  -> tty1 rcv buf rem (reuse, after MAC handshake done)
 * 16 -> tx credit hi byte (added to widen credit to uint16; pairs with byte 0)
 *
 * Host (RPi ipronet) must mirror CREDIT_HI_SCRATCH_OFFSET in its
 * FREE_TX_PBUF_CNT_HIGH_OFFSET (= IPRO_FUNC_SCRATCH_BASE + 16).
 */
#define WIFI_MAC_ADDR_SCRATCH_OFFSET   1
#define BOOT_SRC_INFO_SCRATCH_OFFSET   (1  + 6)
#define TTY_RCV_BUF_REM_OFFSET(tty_id) (1  + (tty_id))
#define CREDIT_HI_SCRATCH_OFFSET       16
/* una_seq reuses the boot-src slot after the boot handshake completes
 * (ipro6le wrote it raw at SDU_BASE+0x167 = scratch offset 7). */
#define UNA_SEQ_SCRATCH_OFFSET         7
#define IPRO_BOOT_SRC_PING_VALUE         0xEE
#define IPRO_BOOT_SRC_PONG_VALUE         0xEF
#define BOOT_SRC_PONG_TIMEOUT_MS       (10 * 1000)

enum {
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_TO_WIFI_TX = 0x2000,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_STA_FROM_WIFI_RX,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_TO_WIFI_TX,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_AP_FROM_WIFI_RX,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_TO_WIFI_TX,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_SNIFFER_FROM_WIFI_RX,
    IPRO_MSG_ETH_WIFI_FRAME_SUBTYPE_EXT, // UART, etc
};

typedef struct {
    uint16_t type;
    uint16_t len; // type + len + payload
    uint8_t payload[];
} __attribute__((packed)) bl602_sdio_msg_t;

typedef struct {
    void *arg;

    bool host_present;

    sdiowifi_mutex tx_lock;
} net_wifi_trcver_t;

#define IPRO_MSG_ERR_HOST_NOT_READY 2
#define IPRO_MSG_ERR_TIMEOUT        3
#define IPRO_MSG_ERR_DESC_USED      4

int ipro_net_wifi_trcver_init(net_wifi_trcver_t *trcver, void *arg);
int ipro_net_wifi_trcver_reinit(net_wifi_trcver_t *trcver);
int ipro_net_wifi_trcver_set_present(net_wifi_trcver_t *trcver, bool present);
int ipro_msg_send(net_wifi_trcver_t *trcver, uint16_t type, const void *payload, uint16_t payload_len);
int ipro_msg_send_pbuf(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, struct pbuf *p, bool is_amsdu, void *cb, void *cb_arg);
int ipro_msg_send_frame_ext(net_wifi_trcver_t *trcver, uint16_t type, uint16_t subtype, const void *headroom, uint16_t headroom_len, const void *tailroom, uint16_t tailroom_len);
uint8_t ipro_read_s_reg(net_wifi_trcver_t *trcver, uint32_t offset);
void ipro_write_s_reg(net_wifi_trcver_t *trcver, uint32_t offset, uint8_t val);

#endif
