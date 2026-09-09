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

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    IPRO_CMD_REBOOT = 0,
    IPRO_CMD_RESET,
    IPRO_CMD_HELLO,
    IPRO_CMD_PING,

    IPRO_CMD_GET_MAC_ADDR,

    // Scan
    IPRO_CMD_SCAN,
    IPRO_CMD_SCAN_RESULTS,

    // STA
    IPRO_CMD_STA_CONNECT,
    IPRO_CMD_STA_DISCONNECT,
    IPRO_CMD_STA_CONNECTED_IND,
    IPRO_CMD_STA_DISCONNECTED_IND,
    IPRO_CMD_STA_IP_UPDATE_IND,
    IPRO_CMD_STA_SET_AUTO_RECONNECT,
    IPRO_CMD_STA_GET_LINK_STATUS,

    // AP
    IPRO_CMD_AP_START,
    IPRO_CMD_AP_STOP,
    IPRO_CMD_AP_STARTED_IND,
    IPRO_CMD_AP_STOPPED_IND,
    IPRO_CMD_AP_GET_STA_LIST,

    // Monitor
    IPRO_CMD_MONITOR_START,
    IPRO_CMD_MONITOR_STOP,
    IPRO_CMD_MONITOR_SET_CHANNEL,
    IPRO_CMD_MONITOR_GET_CHANNEL,

    IPRO_CMD_SET_LPM_MODE,

    // OTA
    IPRO_CMD_GET_DEV_VERSION,
    IPRO_CMD_OTA,

    IPRO_CMD_EXT,

    IPRO_CMD_USER_EXT,

    IPRO_CMD_MAX,
} ipro_cmd_t;

#pragma pack(push, 1)
//// This protocol is LE so LE/BE conversion is hardly needed.

typedef enum {
    STATUS_OK,
    STATUS_NOMEM = 128,
    STATUS_INVALID_INPUT,
    STATUS_INVALID_MODE,
    STATUS_ERR_UNSPECIFIED,
    STATUS_NOT_IMPLEMENTED,
} cmd_status_t;

//// common header
typedef struct {
    uint16_t cmd;
    // flag ACK is used by server to indicate a response to client
#define RNM_MSG_FLAG_ACK         (1 << 0)
    // flag TRANSPARENT is never transfered to peer but used locally
#define RNM_MSG_FLAG_TRANSPARENT (1 << 1)
    // flag ASYNC is used by server to notify client events such as STA_CONNECTED
#define RNM_MSG_FLAG_ASYNC       (1 << 2)
    uint16_t flags;
    uint16_t status;
    uint16_t msg_id;
    uint16_t session_id;
    uint16_t msg_id_replying;
} rnm_base_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
} rnm_ack_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t sta_mac[6];
    uint8_t ap_mac[6];
} rnm_mac_addr_ind_msg_t;

//// Scan
struct ipro_wifi_scan_record {
    uint8_t bssid[6];
    // TODO use compressed SSID encoding to save room
    uint8_t ssid[32 + 1];
    uint16_t channel;
    int8_t rssi;
    uint8_t auth_mode;
    uint8_t cipher;
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t num;
    struct ipro_wifi_scan_record records[];
} rnm_scan_ind_msg_t;

//// STA
typedef struct {
    rnm_base_msg_t hdr;
    uint16_t ssid_len;
    uint8_t ssid[32];
    uint8_t password[64];
} rnm_sta_connect_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t ip4_addr[4];
    uint8_t ip4_mask[4];
    uint8_t ip4_gw[4];
    uint8_t ip4_dns1[4];
    uint8_t ip4_dns2[4];
    uint8_t gw_mac[6];
} rnm_sta_ip_update_ind_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t en;
} rnm_sta_set_auto_reconnect_msg_t;

typedef enum {
    RNM_WIFI_AUTH_UNKNOWN = 0,
    RNM_WIFI_AUTH_OPEN,
    RNM_WIFI_AUTH_WEP,
    RNM_WIFI_AUTH_WPA_PSK,
    RNM_WIFI_AUTH_WPA2_PSK,
    RNM_WIFI_AUTH_WPA_WPA2_PSK,
    RNM_WIFI_AUTH_WPA_ENTERPRISE,
    RNM_WIFI_AUTH_WPA3_SAE,
    RNM_WIFI_AUTH_WPA2_PSK_WPA3_SAE,
    RNM_WIFI_AUTH_MAX,
} rnm_wifi_auth_mode_t;

typedef enum {
    RNM_WIFI_CIPHER_UNKNOWN = 0,
    RNM_WIFI_CIPHER_NONE,
    RNM_WIFI_CIPHER_WEP,
    RNM_WIFI_CIPHER_AES,
    RNM_WIFI_CIPHER_TKIP,
    RNM_WIFI_CIPHER_TKIP_AES,
    RNM_WIFI_CIPHER_MAX,
} rnm_wifi_cipher_t;

typedef enum {
    IPRO_WIFI_LINK_STATUS_UNKNOWN = 0,
    IPRO_WIFI_LINK_STATUS_DOWN,
    IPRO_WIFI_LINK_STATUS_UP,
} ipro_wifi_link_status_t;

struct ipro_wifi_ap_record {
    uint8_t link_status;
    uint8_t bssid[6];
    uint8_t ssid[32 + 1];
    uint8_t channel;
    int8_t rssi;
    uint8_t auth_mode;
    uint8_t cipher;
};

typedef struct {
    rnm_base_msg_t hdr;
    struct ipro_wifi_ap_record record;
} rnm_sta_link_status_ind_msg_t;

//// AP
// TODO hidden SSID mode
typedef struct {
    rnm_base_msg_t hdr;
    uint8_t is_open;
    uint16_t channel;
    uint16_t ssid_len;
    uint8_t ssid[32];
    uint8_t password[64];
} rnm_ap_start_msg_t;

// get station list
struct wifi_sta_info {
    uint8_t mac[6];
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t num;
    struct wifi_sta_info sta[];
} rnm_ap_sta_list_ind_msg_t;

//// Monitor
typedef struct {
    rnm_base_msg_t hdr;
    uint16_t channel;
} rnm_monitor_set_channel_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t channel;
} rnm_monitor_channel_ind_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t en;
} rnm_set_lpm_mode_msg_t;

//// OTA
typedef struct {
    uint32_t version_num;
} ipro_ota_dev_version_t;

typedef struct {
    rnm_base_msg_t hdr;
    ipro_ota_dev_version_t version;
} rnm_dev_version_ind_msg_t;

typedef enum {
    IPRO_OTA_NONE,
    IPRO_OTA_START_ACK,
    IPRO_OTA_ABORT_ACK,
    IPRO_OTA_PROGRAM_ACK,
    IPRO_OTA_FIN_ACK,
    IPRO_OTA_COMMIT_ACK,
    IPRO_OTA_ERROR,
} ipro_ota_status_type_t;

#define RNM_OTA_PART_SIZE 1024

typedef enum {
    IPRO_OTA_ERROR_STATUS_INCORRECT = 8888,
    IPRO_OTA_ERROR_NO_ALT_PARTITION,
    IPRO_OTA_ERROR_MALFORMED_HEADER,
    IPRO_OTA_ERROR_HEADER_ADV_LEN_TOO_BIG,
    IPRO_OTA_ERROR_CKSUM_MISMATCH,
    IPRO_OTA_ERROR_PART_MISSING,
    IPRO_OTA_ERROR_SIZE_NOT_EXPECTED,
    IPRO_OTA_ERROR_MISC,
} ipro_ota_error_code_t;

typedef enum {
    IPRO_OTA_MSG_START,
    IPRO_OTA_MSG_ABORT,
    IPRO_OTA_MSG_PROGRAM_PART,
    IPRO_OTA_MSG_COMMIT,
    IPRO_OTA_MSG_STATUS,
} ipro_ota_msg_type_t;

struct ipro_ota_status {
    uint16_t status_type;
    union {
        struct {
            uint32_t offset;
        } program_ack;
        struct {
            uint16_t code;
        } error;
    };
};

typedef struct {
    rnm_base_msg_t hdr;
    uint16_t msg_type;
    union {
        struct {
            uint32_t offset;
            uint32_t data_len;
            uint8_t data[];
        } program_part;
        struct ipro_ota_status status;
    };
} rnm_ota_msg_t;

typedef struct {
    rnm_base_msg_t hdr;
    uint8_t payload[];
} rnm_user_ext_msg_t;

#pragma pack(pop)

#ifdef __cplusplus
}
#endif
