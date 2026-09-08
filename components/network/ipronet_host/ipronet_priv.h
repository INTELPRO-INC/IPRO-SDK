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

/* ipronet_priv.h - internals shared by the ipronet_host sources. Not an API. */
#ifndef IPRONET_PRIV_H
#define IPRONET_PRIV_H

#include <stdint.h>
#include <stdbool.h>
#include "ipronet_host.h"
#include "ipronet_host_diag.h"
#include "ipronet_ctrl.h"

/* On-wire framing - verbatim from Linux sdio_msgs.h. */
#define IPRONET_MSG_TYPE_DATA      0xABCDU
#define IPRONET_MSG_TYPE_CMD       0xEF01U
#define IPRONET_MSG_SUBTYPE_STA_RX 0x2001U   /* STA_FROM_WIFI_RX */
#define IPRONET_MSG_SUBTYPE_STA_TX 0x2000U   /* STA_TO_WIFI_TX   */

/* Zero-copy stride geometry, shared by the transport (packer / walker) and the
 * ZCOK grant parser in ipronet_ctrl.c. ZCEN2 = host->device strides, ZCEN3 =
 * device->host strides; both are 2048 B, at most four per transfer. */
#define IPRO6S_ZC2_STRIDE          2048U
#define IPRO6S_ZC2_MAX_SLOT        4U
#define IPRO6S_ZC3_STRIDE          2048U
#define IPRO6S_ZC3_MAX_SLOT        4U

/* ---- ipronet_sdio.c ------------------------------------------------------ */
int  ipronet_sdio_init(void);                       /* IPRONET_ERR_SETUP */
int  ipronet_sdio_enum(void);                       /* IPRONET_ERR_NO_DEVICE */
int  ipronet_sdio_setup(uint32_t bus_hz);           /* IPRONET_ERR_SETUP */
bool ipronet_sdio_is_setup(void);                   /* F1 enabled */
int  ipronet_sdio_rx_start(void);                   /* IPRONET_ERR_NOMEM */
void ipronet_sdio_rx_stop(void);                    /* blocks until the task exits (<= 100 ms) */
void ipronet_sdio_teardown(void);                   /* stop rx, disable F1, reset agg/zc state, controller back to 1-bit ID mode */
int  ipronet_sdio_send_pdu(uint16_t msg_type, uint16_t subtype,
                           const void *payload, uint16_t payload_len);   /* IPRONET_ERR_IO */
int  ipronet_sdio_eth_tx(const uint8_t *frame, uint16_t len);
void ipronet_sdio_tx_agg_flush(void);
void ipronet_sdio_tx_agg_accept_grant(uint32_t nslot, uint32_t stride);
void ipronet_sdio_tx_single_frame_lock(void);       /* ZCEN: drain batch, cap=0, keep the lock */
void ipronet_sdio_tx_single_frame_unlock(void);
void ipronet_sdio_set_zc3_uslots(uint32_t uslots);
void ipronet_sdio_set_pace(uint32_t kbps);
int  ipronet_sdio_power(int on);                    /* IPRONET_ERR_STATE if unconfigured */

/* ---- ipronet_ctrl.c ------------------------------------------------------ */
void ipronet_ctrl_reset_session(void);              /* MAC/link/IP/reconnect state -> idle */
void ipronet_ctrl_set_callbacks(const ipronet_callbacks_t *cb);   /* NULL = none */
void ipronet_ctrl_set_auto_reconnect(uint8_t en);
void ipronet_ctrl_handle(const uint8_t *pld, uint16_t len);        /* RX task -> here */
int  ipronet_ctrl_send_hello(void);
int  ipronet_ctrl_get_mac(void);                    /* blocks <= 1 s; IPRONET_ERR_NO_MAC */
int  ipronet_ctrl_zcen(int gen);
int  ipronet_ctrl_link_status(void);
bool ipronet_ctrl_mac_learned(void);
const uint8_t *ipronet_ctrl_sta_mac(void);
const uint8_t *ipronet_ctrl_ap_mac(void);

/* ---- ipronet_netif.c ----------------------------------------------------- */
void ipronet_netif_up(void);
void ipronet_netif_down(void);
bool ipronet_netif_is_up(void);
void ipronet_netif_set_ip(const uint8_t ip4[4], const uint8_t mask4[4], const uint8_t gw4[4]);
void ipronet_netif_set_mac(const uint8_t mac[6]);
void ipronet_netif_input(const uint8_t *eth, uint16_t len);
uint32_t ipronet_netif_pool_drops(void);
uint32_t ipronet_netif_mbox_drops(void);

#endif /* IPRONET_PRIV_H */
