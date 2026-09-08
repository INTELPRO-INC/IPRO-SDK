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

/*
 * ipronet_ctrl.h — host-side control-plane wire format for the ipronet
 * SDIO-WiFi bridge (host commands the ipro6s device to scan/connect; device
 * reports connect/IP events back). Ported + renamed from the device/Linux-host
 * RNM protocol — wire values are byte-identical to the contract:
 *   device: ipro6s_wifi_dev/components/network/remote_net_manager/inc/rnm_msg.h
 *   linux host ref: ~/work/ext/ipro_iotnet/rnm_msg.h
 * Renames (host port): device/linux IPRO_CMD_ becomes IPRONET_CMD_;
 * rnm_ structs become ipronet_.
 * Protocol is little-endian, packed; IPRO7 is LE so no byte-swap.
 */
#ifndef IPRONET_CTRL_H
#define IPRONET_CTRL_H

#include <stdint.h>

/* Command IDs — VALUES ARE THE WIRE CONTRACT, do not renumber.
 * (== device and linux host ipro_cmd_t enum ordering.) */
typedef enum {
    IPRONET_CMD_REBOOT = 0,
    IPRONET_CMD_RESET,
    IPRONET_CMD_HELLO,                  /* 2 */
    IPRONET_CMD_PING,                   /* 3 */
    IPRONET_CMD_GET_MAC_ADDR,           /* 4 */
    IPRONET_CMD_SCAN,                   /* 5 */
    IPRONET_CMD_SCAN_RESULTS,           /* 6 */
    IPRONET_CMD_STA_CONNECT,            /* 7 */
    IPRONET_CMD_STA_DISCONNECT,         /* 8 */
    IPRONET_CMD_STA_CONNECTED_IND,      /* 9  (async event) */
    IPRONET_CMD_STA_DISCONNECTED_IND,   /* 10 (async event) */
    IPRONET_CMD_STA_IP_UPDATE_IND,      /* 11 (async event, carries IP) */
    IPRONET_CMD_STA_SET_AUTO_RECONNECT, /* 12 */
    IPRONET_CMD_STA_GET_LINK_STATUS,    /* 13 */
    IPRONET_CMD_AP_START,               /* 14 */
    IPRONET_CMD_AP_STOP,
    IPRONET_CMD_AP_STARTED_IND,
    IPRONET_CMD_AP_STOPPED_IND,
    IPRONET_CMD_AP_GET_STA_LIST,        /* 18 */
    IPRONET_CMD_MONITOR_START,          /* 19 */
    IPRONET_CMD_MONITOR_STOP,
    IPRONET_CMD_MONITOR_SET_CHANNEL,
    IPRONET_CMD_MONITOR_GET_CHANNEL,
    IPRONET_CMD_SET_LPM_MODE,           /* 23 */
    IPRONET_CMD_GET_DEV_VERSION,        /* 24 */
    IPRONET_CMD_OTA,                    /* 25 */
    IPRONET_CMD_EXT,                    /* 26 */
    IPRONET_CMD_USER_EXT,               /* 27 — carries the "ZCEN<n>" opt-in */
    IPRONET_CMD_MAX,
} ipronet_cmd_t;

/* hdr.flags bits (was RNM_MSG_FLAG_*) */
#define IPRONET_FLAG_ACK         (1u << 0)  /* device->host: this is a response */
#define IPRONET_FLAG_TRANSPARENT (1u << 1)  /* local-only, never on the wire */
#define IPRONET_FLAG_ASYNC       (1u << 2)  /* device->host: unsolicited event */

/* hdr.status (was cmd_status_t) */
enum {
    IPRONET_STATUS_OK = 0,
    IPRONET_STATUS_NOMEM = 128,
    IPRONET_STATUS_INVALID_INPUT,
    IPRONET_STATUS_INVALID_MODE,
    IPRONET_STATUS_ERR_UNSPECIFIED,
    IPRONET_STATUS_NOT_IMPLEMENTED,
};

/* auth modes reported in scan records (was rnm_wifi_auth_mode_t): the
 * IPRONET_AUTH_* values live in the public ipronet_host.h, same numbering. */

#pragma pack(push, 1)

/* common control header (was rnm_base_msg_t), 12 bytes */
typedef struct {
    uint16_t cmd;
    uint16_t flags;
    uint16_t status;
    uint16_t msg_id;
    uint16_t session_id;
    uint16_t msg_id_replying;
} ipronet_ctrl_hdr_t;

/* body that follows the hdr for STA_CONNECT (total msg = hdr + this = 110 B) */
typedef struct {
    uint16_t ssid_len;
    uint8_t  ssid[32];
    uint8_t  password[64];
} ipronet_sta_connect_body_t;

/* STA_IP_UPDATE_IND payload (hdr + this); ip bytes are octets a.b.c.d */
typedef struct {
    ipronet_ctrl_hdr_t hdr;
    uint8_t ip4_addr[4];
    uint8_t ip4_mask[4];
    uint8_t ip4_gw[4];
    uint8_t ip4_dns1[4];
    uint8_t ip4_dns2[4];
    uint8_t gw_mac[6];
} ipronet_sta_ip_update_ind_msg_t;

/* one scan result record, 44 bytes */
typedef struct {
    uint8_t  bssid[6];
    uint8_t  ssid[32 + 1];
    uint16_t channel;
    int8_t   rssi;
    uint8_t  auth_mode;
    uint8_t  cipher;
} ipronet_scan_record_t;

/* SCAN_RESULTS payload (hdr + num + records[]) */
typedef struct {
    ipronet_ctrl_hdr_t hdr;
    uint16_t num;
    ipronet_scan_record_t records[];
} ipronet_scan_ind_msg_t;

/* GET_MAC_ADDR response */
typedef struct {
    ipronet_ctrl_hdr_t hdr;
    uint8_t sta_mac[6];
    uint8_t ap_mac[6];
} ipronet_mac_addr_ind_msg_t;

#pragma pack(pop)

#endif /* IPRONET_CTRL_H */
