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

#ifndef __WIFI_MGMR_EXT_H_
#define __WIFI_MGMR_EXT_H_

#include <stdint.h>
#include <stdbool.h>
#include "fhost_connect_result.h"

#define MAX_FIXED_CHANNELS_LIMIT (14)
#define MAX_AP_SCAN     50
#define MGMR_SSID_LEN   32
#define MGMR_KEY_LEN    64
#define MGMR_BSSID_LEN  18
#define MGMR_AKM_LEN    15

/* WiFi async event */
#define  EV_WIFI                  0x0002
#define  CODE_WIFI_ON_INIT_DONE   1
#define  CODE_WIFI_ON_MGMR_DONE   2
#define  CODE_WIFI_CMD_RECONNECT  3
#define  CODE_WIFI_ON_CONNECTED   4
#define  CODE_WIFI_ON_DISCONNECT  5
#define  CODE_WIFI_ON_PRE_GOT_IP  6
#define  CODE_WIFI_ON_GOT_IP      7
#define  CODE_WIFI_ON_CONNECTING  8
#define  CODE_WIFI_ON_SCAN_DONE   9
#define  CODE_WIFI_ON_SCAN_DONE_ONJOIN  10
#define  CODE_WIFI_ON_AP_STARTED        11
#define  CODE_WIFI_ON_AP_STOPPED        12
#define  CODE_WIFI_ON_PROV_SSID         13
#define  CODE_WIFI_ON_PROV_BSSID        14
#define  CODE_WIFI_ON_PROV_PASSWD       15
#define  CODE_WIFI_ON_PROV_CONNECT      16
#define  CODE_WIFI_ON_PROV_DISCONNECT   17
#define  CODE_WIFI_ON_PROV_SCAN_START   18
#define  CODE_WIFI_ON_PROV_STATE_GET    19
#define  CODE_WIFI_ON_MGMR_DENOISE      20
#define  CODE_WIFI_ON_AP_STA_ADD        21
#define  CODE_WIFI_ON_AP_STA_DEL        22
#define  CODE_WIFI_ON_EMERGENCY_MAC     23
#define  CODE_WIFI_ON_EXIT_PS           24
#define  CODE_WIFI_ON_GOT_IP6           25
#define  CODE_WIFI_ON_SCAN_DONE_CONNECTING  31

/* Completion payload (platform code2, int copied by ipro_event): applies to
 * CODE_WIFI_ON_SCAN_DONE and CODE_WIFI_ON_SCAN_DONE_CONNECTING. This reports
 * firmware completion, not request admission. No request token is carried;
 * consumers must separately serialize/correlate scans and handle lost events.
 * Do not treat a failed scan's retained cache as a successful fresh result. */
#define WIFI_SCAN_RESULT_SUCCESS 0
#define WIFI_SCAN_RESULT_FAILED  1

#define WIFI_EVENT_BEACON_IND_AUTH_OPEN            0
#define WIFI_EVENT_BEACON_IND_AUTH_WEP             1
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK         2
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK        3
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK    4
#define WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT         5
#define WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE        6
#define WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE 7
#define WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN      0xff

#define WIFI_EVENT_BEACON_IND_CIPHER_NONE           0
#define WIFI_EVENT_BEACON_IND_CIPHER_WEP            1
#define WIFI_EVENT_BEACON_IND_CIPHER_AES            2
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP           3
#define WIFI_EVENT_BEACON_IND_CIPHER_TKIP_AES       4

/// Interface types
typedef enum
{
    /// STA interface
    MGMR_VIF_STA = 0,
    /// AP interface
    MGMR_VIF_AP
}wifi_mgmr_vif_type;

typedef struct wifi_mgmr_scan_item {
    uint32_t mode;
    uint32_t timestamp_lastseen;
    int ssid_len;
    uint8_t channel;
    int8_t rssi;
    char ssid[32];
    char ssid_tail[1];//always put ssid_tail after ssid
    uint8_t bssid[6];
    int8_t ppm_abs;
    int8_t ppm_rel;
    uint8_t auth;
    uint8_t cipher;
    uint8_t is_used;
    uint8_t wps;
} wifi_mgmr_scan_item_t;

typedef struct wifi_mgmr_sniffer_item {
    /// interface index
    char *itf;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// Frequency for Primary 20MHz channel (in MHz)
    uint16_t prim20_freq;
    /// Frequency center of the contiguous channel or center of Primary 80+80 (in MHz)
    uint16_t center1_freq;
    /// Frequency center of the non-contiguous secondary 80+80 (in MHz)
    uint16_t center2_freq;
    /// Frame received callback.
    void *cb;
    /// Parameter for the monitor callback
    void *cb_arg;
} wifi_mgmr_sniffer_item_t;

/// sta connect config params
/*
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  bssid    : BSSID of AP
 *  akm_str  : AKM of AP
 *  pmf_cfg  : PMF config
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 *  use_dhcp : Whether to use the dhcp server which provided by AP
 */
typedef struct wifi_mgmr_sta_connect_params {
    char ssid[MGMR_SSID_LEN];
    uint8_t ssid_tail[1];
    char key[MGMR_KEY_LEN];
    char key_tail[1];
    uint8_t ssid_len;
    uint8_t key_len;
    char bssid_str[MGMR_BSSID_LEN];
    // must be uppercase, NULL terminated string
    char akm_str[MGMR_AKM_LEN];
    uint8_t akm_len;
    uint16_t freq1;
    uint16_t freq2;
    uint8_t pmf_cfg;
    uint8_t use_dhcp;
    // listen_interval to monitor AP beacon
    // range:[1, 100]
    uint8_t listen_interval;
    // default: 0, scan on all channels
    // if 1, quick scan, connect to the first ssid_matched AP
    uint8_t scan_mode;
    // default: normal connect
    // if 1, quick connect
    uint8_t quick_connect;
    int timeout_ms;
    // Extra flags passed to wpa_supplicant. See FHOST_WPA_SSID_* in fhost_wpa.h.
    int wpa_flags;
    // conn scan duration, In TUs.
    uint16_t duration;
    // conn scan probe req cnt
    uint16_t probe_cnt;
    // Auth and Assoc timeout, in sec. 0, use default value
    uint8_t auth_timeout;
    // Timeout before EAPOL 1 after associtiated, in sec. 0, use default value
    uint8_t eapol_1_timeout;
    // Remaining EAPOL session timeout, in sec. 0, use default value
    uint8_t eapol_rem_timeout;
} wifi_mgmr_sta_connect_params_t;

/// scan params
typedef struct wifi_mgmr_scan_params {
    uint8_t ssid_length;
    uint8_t ssid_array[MGMR_SSID_LEN];
    uint8_t bssid[6];
    uint8_t bssid_set_flag;
    uint8_t probe_cnt;
    int channels_cnt;
    uint8_t channels[MAX_FIXED_CHANNELS_LIMIT];
    uint32_t duration;
    /// 0: active scan; 1: Passive scan request
    bool passive;
} wifi_mgmr_scan_params_t;

typedef struct wifi_mgmr_raw_send_params {
    // start addr of the packet to send
    void *pkt;
    // length of the packet to send
    uint32_t len;
    // channel of the packet will be sent at
    uint8_t channel;
} wifi_mgmr_raw_send_params_t;

/// ap start params
typedef struct wifi_mgmr_ap_params {
    /// must be setted
    char *ssid;
    /// if NULL and the akm is not NULL, the default value "12345678" will be setted
    char *key;
    /// OPEN/WPA/WPA2 can be supported now, if NULL and the key is not NULL, the default value "WPA2" will be setted
    char *akm;
    /// if zero, the default value 6 will be setted
    uint8_t channel;
    /// Channel type (@ref mac_chan_bandwidth)
    uint8_t type;
    /// use_ipcfg;
    bool use_ipcfg;
    /// Whether use dhcpd
    bool use_dhcpd;
    /// dhcpd pool start
    int start;
    /// dhcpd pool limit
    int limit;
    /// ap ip addr
    uint32_t ap_ipaddr;
    /// ap subnet mask
    uint32_t ap_mask;
    /// STA MAX inactivity under connection
    uint32_t ap_max_inactivity;
    /// whether use hidden ssid
    bool hidden_ssid;
    /// whether enable isolation
    bool isolation;
    /// Beacon interval in TU
    int bcn_interval;
    /// Additional vendor specific elements for Beacon and Probe Response frames
    /// a hexdump of the raw information elements (id+len+payload for one or more elements),
    //  the maximum length supported is MAX_AP_VENDOR_ELEMENTS_LEN,
    /// ref wap_supplicant.conf
    char *ap_vendor_elements;
} wifi_mgmr_ap_params_t;

/**
 * Fully hosted frame information
 */
struct wifi_frame_info
{
    /**
     * Interface index that received the frame. (-1 if unknown)
     */
    int fvif_idx;
    /**
     * Length (in bytes) of the frame.
     */
    uint16_t length;
    /**
     * Primary channel frequency (in MHz) on which the frame has been received.
     */
    uint16_t freq;
    /**
     * Received signal strength (in dBm)
     */
    int8_t rssi;
    /**
     * Frame payload. Can be NULL if monitor mode is started with @p uf parameter set to
     * true. In this case all other fields are still valid.
     */
    uint8_t *payload;
};

typedef struct wifi_mgmr_connect_ind_stat_info {
    uint16_t status_code;
    uint16_t reason_code;
    char ssid[33];
    char passphr[65];
    /// BSSID
    uint8_t bssid[6];
    uint8_t type_ind;
    uint8_t chan_band;
    uint8_t channel;
    uint8_t security;
    /// Association Id allocated by the AP for this connection
    uint16_t aid;
    /// Index of the VIF for which the association process is complete
    uint8_t vif_idx;
    /// Index of the STA entry allocated for the AP
    uint8_t ap_idx;
    /// Index of the LMAC channel context the connection is attached to
    uint8_t ch_idx;
    /// Flag indicating if the AP is supporting QoS
    bool qos;
} wifi_mgmr_connect_ind_stat_info_t;

typedef struct wifi_conf {
    char country_code[3];
    int channel_nums;
} wifi_conf_t;

typedef struct wifi_sta_basic_info {
    uint8_t sta_idx;
    uint8_t is_used;
    uint8_t sta_mac[6];
    uint16_t aid;
} wifi_sta_basic_info_t;

typedef struct rf_pwr_table {
    int8_t     pwr_11b[4];
    int8_t     pwr_11g[8];
    int8_t     pwr_11n_ht20[8];
    int8_t     pwr_11n_ht40[8];
    int8_t     pwr_11ac_vht20[10];
    int8_t     pwr_11ac_vht40[10];
    int8_t     reserved[10];
    int8_t     pwr_11ax_he20[12];
    int8_t     pwr_11ax_he40[12];
    int8_t     reserved2[12];
    int8_t     reserved3[12];
}rf_pwr_table_t;

typedef struct
{
    uint8_t ucSSID[32];         /**< SSID of the Wi-Fi network. */
    uint8_t ucSSIDLength;       /**< SSID length. */
    uint8_t ucBSSID[6];         /**< BSSID of the Wi-Fi network. */
    uint8_t ucSecurity;         /**< Wi-Fi Security. */
    uint8_t ucPWD[65];          /**< WPA/WPA2 passphrase. */
    uint8_t ucChannel;          /**< Channel number. */
} wifi_mgmr_ap_info_t;

typedef void (*scan_item_cb_t)(void *env, void *arg, wifi_mgmr_scan_item_t *item);

/**
 * wifi_sta_connect
 * Connect to the AP
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  bssid    : BSSID of AP
 *  akm_str  : AKM of AP, must be all in upper case
 *  pmf_cfg  : PMF config
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 *  use_dhcp : Whether to use the dhcp server which provided by AP
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp);
/**
 * wifi_mgmr_sta_quickconnect
 * Connect to the AP
 * param:
 *  ssid     : SSID of target AP
 *  key      : Password of AP
 *  freq1    : Frequency of AP
 *  freq2    : Frequency of AP (You can specify up to two frequencies on which AP will be scanned.)
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_quickconnect(const char *ssid, const char *key, uint16_t freq1, uint16_t freq2);


/**
 * wifi_mgmr_sta_connect
 * Connect to the AP
 * param:
 *  param1 : Configuration of connecting
 * return:
 *  0 : Connection entry task accepted; association is asynchronous
 *  -1 : Failed
 *  Others is Failed
 * Rejects a busy connection entry/worker without queuing. Failure does not guarantee
 * that connection parameters or other manager state remain unchanged.
 */
int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *config);

/** Submit a connection with an optional worker completion notification.
 * Returns 0 for admission only, -1 for rejection (no callback). Accepted work
 * reports once on normal worker exit or failure to create its CONNECT task.
 * Callback can run before this function returns. Context must stay valid until
 * callback returns. It runs on CONNECT/CONNECT_ENTRY, not the Matter event loop,
 * outside critical sections while admission is still busy; only post a bounded
 * notification and return. Do not block, delete the worker, or call CHIP APIs.
 * CONNECTED is a WPA snapshot, not DHCP/IPv6 readiness. Manager timeout 0 keeps
 * its legacy 200-second default (unlike fhost's direct async timeout 0 mode).
 * Submission itself provides no cancellation/drain; see the separate terminal
 * close API below. Reset/external task deletion cannot promise callback delivery.
 * Like the legacy API, rejection at task admission does not roll back earlier
 * STA state, saved reconnect parameters or sleep-prevention updates.
 */
int wifi_mgmr_sta_connect_with_completion(const wifi_mgmr_sta_connect_params_t *config,
                                         fhost_connect_completion_fn completion,
                                         void *context);
/* Managed provisioning: same admission/completion contract, but restore
 * automatic retries on the CONNECT worker after old-interface preparation.
 * No WPA command runs on the submitting task. Preparation failure preserves
 * policy; once new configuration starts, retries remain enabled even if
 * association fails. Legacy connect APIs retain their existing policy.
 * Caller must serialize live configuration ownership with other Wi-Fi writers. */
int wifi_mgmr_sta_connect_managed(const wifi_mgmr_sta_connect_params_t *config,
                                 fhost_connect_completion_fn completion, void *context);

/* Task-only terminal shutdown stage, no locks held. Close all manager/fhost
 * CONNECT and autoconnect-enable admission until reboot. Poll:
 * 1=submission/enable-command/worker/callback/cleanup/pause-lease
 * still active, 0=those paths drained (task self-deletion may remain). Existing
 * work is not cancelled. Call only after fail-safe rollback is done; no further
 * connection may start. This alone does not disable supplicant autoconnect;
 * call autoconnect_disable only after drain succeeds. Does NOT fence other Wi-Fi writers, scan, direct WPA,
 * autoreconnect, DHCP or lwIP, and does not disconnect/power down the radio. */
int wifi_mgmr_sta_connect_close(void);
/* Internal auto-reconnect task entry. Revalidates its arm-time generation and
 * the auto-reconnect flag atomically with manager admission, before changing
 * STA status/credentials. 0=admitted, -1=stale/disabled/busy/rejected; not an
 * association result. Shares CONNECT pause/close and submission drainage. */
int wifi_mgmr_sta_reconnect(uint32_t generation);
/* wifi_mgmr_sta_reconnect that also runs with auto-reconnect off:
 * the one-shot 20 MHz retry after a failed 40 MHz EAPOL exchange. */
int wifi_mgmr_sta_reconnect_bw20(uint32_t generation);
/* Internal DHCP-failure recovery with the same generation/admission fence.
 * Retry if automatic reconnect is enabled; otherwise disconnect. The chosen
 * action participates in pause/close drainage, including blocking WPA calls.
 * Task-only, no locks held. 0=success/admitted, -1=stale/busy/failed. */
int wifi_mgmr_sta_dhcp_recover(uint32_t generation);

/* Task-only temporary pause of manager CONNECT/autoconnect-enable mutations
 * and native CONNECT admission. One externally serialized owner: start with
 * *lease=0, then poll using the returned nonzero token. 1=accepted, draining;
 * 0=drained and lease held; -1=invalid/stale/other owner/closed/exhausted.
 * Accepted submissions/workers run normally; no forced cancellation. Keep the
 * lease through the exclusive operation and release once, even if a later
 * poll fails due to terminal close. Close waits for this release.
 * Does NOT fence scans, direct WPA/IPC network changes, DHCP or other writers;
 * does not disable supplicant autoconnect or disconnect. Callers must separately
 * coordinate those paths. Do not mix with direct native pause/resume calls. */
int wifi_mgmr_sta_connect_pause(uint32_t *lease);

/* Release a matching token (0=success, -1=invalid/stale/native error). The
 * caller must stop exclusive work first, then clear its token after success.
 * May abandon a still-draining pause; cannot undo terminal close. */
int wifi_mgmr_sta_connect_resume(uint32_t lease);
/**
 * wifi_sta_disconnect
 * Disconnect from the AP
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_sta_disconnect(void);

/**
 * Set IPv4 address of STA interface
 *
 * return 0 on success or -1 on error
 */
int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns);

/**
 * Get IPv4 address of STA interface
 *
 * return 0 on success or -1 on error
 */
int wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns);

#if NX_FHOST_MONITOR
/**
 * wifi_mgmr_sniffer_enable
 * start sniffer mode
 * param:
 *  param1 : Configuration of sniffer mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sniffer_enable(wifi_mgmr_sniffer_item_t sniffer_item);

/**
 * wifi_mgmr_sniffer_disable
 * stop sniffer mode
 * param:
 *  param1 : Handle of sniffer mode (frome wifi_mgmr_sniffer_enable())
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sniffer_disable(wifi_mgmr_sniffer_item_t sniffer_item);
#endif

/**
 * wifi_mgmr_state_get
 * get ap/sta state
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_state_get(void);

/**
 * wifi_mgmr_sta_rssi_get
 * get the last beacon rssi
 * param:
 *  param1 : ptr of rssi
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_rssi_get(int *rssi);

/**
 * wifi_mgmr_sta_channel_get
 * get sta mode channel
 * param:
 *  param1 : ptr of channel
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_channel_get(int *channel);

/**
 * wifi_mgmr_sta_ssid_set
 * set sta mode ssid
 * param:
 *  param1 : ptr of ssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ssid_set(const char *ssid);

/**
 * wifi_mgmr_sta_ssid_get
 * get sta mode ssid
 * param:
 *  param1 : ptr of ssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ssid_get(char *ssid);

/**
 * wifi_mgmr_sta_passphr_set
 * set sta mode password
 * param:
 *  param1 : ptr of password
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_passphr_set(const char *passphr);

/**
 * wifi_mgmr_sta_connect_ind_stat_get
 * get state (ssid password) of sta mode
 * param:
 *  param1 : ptr of struct wifi_mgmr_connect_ind_stat_info_t
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat);

/**
 * wifi_mgmr_sta_scan
 * Scan available APs
 * param:
 *  param1 : Configuration of scanning
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config);
/* SDK-native tracked variant. For nonzero IDs both config and command must
 * remain alive until completion or explicit rejection, including after an
 * uncertain transport return. Do not read/write command while outstanding:
 * the control task owns access after submission. The queue carries a pointer,
 * NOT a copy. Config must remain immutable. Command embeds the SSID descriptor
 * but borrows config's data arrays.
 * Accepted completion uses fhost_scan_request_completed.
 * See fhost_scan_result.h for ID lifetime, callback context and reset limits.
 * A zero ID is untracked. Returns 0=accepted, -1=rejected, -2=uncertain transport
 * outcome (a callback may still arrive). Does not impose a timeout. */
struct cfgrwnx_scan;
int wifi_mgmr_sta_scan_with_id(const wifi_mgmr_scan_params_t *config, uint32_t request_id,
                             struct cfgrwnx_scan *command);

/**
 * wifi_mgmr_sta_scanlist
 * List the scan results in last scan
 * return:
 *  0 : Success
 *  Others is Failed
 */
int wifi_mgmr_sta_scanlist(void);

/**
 * wifi_mgmr_sta_scanlist_nums_get
 * Get the number of scan results
 * return:
 *  Number of scan results
 */
uint32_t wifi_mgmr_sta_scanlist_nums_get(void);

/**
 * wifi_mgmr_sta_scanlist_dump
 * Get the scan results
 * param:
 *  param1 : addr for store scan results
 *  param2 : max number of items can be used for storing the scan results
 * return:
 *  Number of scan results
 */
uint32_t wifi_mgmr_sta_scanlist_dump(void * results, uint32_t resultNums);

/**
 * wifi_mgmr_scan_filter_hidden_ssid
 * Whether filter hidden ssid from scan results
 * param:
 *  param1 : Whether enable filter
 * return:
 *  0 : Success
 *  Others is Failed
 */
int wifi_mgmr_scan_filter_hidden_ssid(int filter);

/**
 * @brief Get scan results
 *
 * @param env env for cb
 * @param arg arg for cb
 * @param cb  Callback to receive scan results
 *
 * @return 0
 */
int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb);

/**
 * wifi_mgmr_ap_start
 * Start AP mode
 * param:
 *  param1 : Configuration of AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_start(const wifi_mgmr_ap_params_t *config);

/**
 * wifi_mgmr_ap_stop
 * Stop AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_stop(void);

/**
 * wifi_mgmr_mode_to_str
 * Get string of eht mode
 * param:
 *  param1 : Phy mode
 * return:
 *  "Unknown" : the mode can not be convert to string
 *  Others is the string of mode
 */
char *wifi_mgmr_mode_to_str(uint32_t mode);

/**
 * wifi_mgmr_mac_set
 * Set mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_mac_set(uint8_t mac[6]);

/**
 * wifi_mgmr_sta_mac_get
 * Get sta mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_mac_get(uint8_t mac[6]);

/**
 * wifi_mgmr_ap_mac_get
 * Get ap mac
 * param:
 *  param1 : Array of mac address
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_mac_get(uint8_t mac[6]);

/**
 * wifi_mgmr_set_country_code
 * Set country code
 * param:
 *  param1 : Pointer of country code, such as "CN","US","JP","EU"
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_set_country_code(char *country_code);

/**
 * wifi_mgmr_get_country_code
 * Get country code
 * param:
 *  country_code : Pointer to get country code, such as "CN","US","JP","EU"
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_get_country_code(char *country_code);

/**
 * wifi_mgmr_sta_tx_ac_min_set
 * Lowest WMM access category for station data frames:
 * 0 = AC_BK (default, no change), 1 = AC_BE, 2 = AC_VI, 3 = AC_VO.
 * A frame on a lower AC is sent on this one; an AP that requires
 * admission control on it still gets the frame downgraded.
 * return:
 *  0 : Success
 *  -1 : ac out of range
 */
int wifi_mgmr_sta_tx_ac_min_set(int ac);

/**
 * wifi_mgmr_sta_tx_ac_min_get
 * return: the access category set by wifi_mgmr_sta_tx_ac_min_set
 */
int wifi_mgmr_sta_tx_ac_min_get(void);

/**
 * wifi_mgmr_set_autoconnect_enable
 * Enable station autoconnect
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_autoconnect_enable(void);

/**
 * wifi_mgmr_set_autoconnect_disable
 * Disable station autoconnect
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_autoconnect_disable(void);

/**
 * wifi_mgmr_sta_ps_enter
 * Enter powersave mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ps_enter(void);

/**
 * wifi_mgmr_sta_ps_exit
 * Exit powersave mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_ps_exit(void);

/**
 * wifi_mgmr_sta_set_listen_itv
 * Set listen interval
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_set_listen_itv(uint8_t itv);

/**
 * wifi_mgmr_sta_get_listen_itv
 * Get listen interval
 * return: listen interval
 */
uint8_t wifi_mgmr_sta_get_listen_itv(void);

/**
 * wifi_mgmr_sta_aid_get
 * Get aid
 * return:aid
 */
int wifi_mgmr_sta_aid_get(void);

/**
 * wifi_mgmr_sta_get_bssid
 * Get STA bssid
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_sta_get_bssid(uint8_t bssid[6]);

/**
 * wifi_mgmr_ap_sta_info_get
 * Get STA List in AP mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx);

/**
 * wifi_mgmr_tpc_pwr_set
 * Set power table
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_tpc_pwr_set(rf_pwr_table_t *power_table);

/**
 * wifi_mgmr_tpc_pwr_get
 * Get power table
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_tpc_pwr_get(rf_pwr_table_t *power_table);

/**
 * wifi_mgmr_connection_info
 * Get connected ap/router info
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_connection_info(wifi_mgmr_connect_ind_stat_info_t * connection_info);

/**
 * wifi_mgmr_get_ap_info
 * Get ap info
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_get_ap_info(wifi_mgmr_ap_info_t *ap);

/**
 ****************************************************************************************
 * @brief Set keep alive time for sta mode
 *
 * @param[in] the keep alive time for sta mode(unit: seconds)
 ****************************************************************************************
 */
int wifi_mgmr_sta_keepalive_time_set(uint8_t time_seconds);

/**
 ****************************************************************************************
 * @brief get sta connected or not connected for sta mode
 * return:
 *  0 : not connected
 *  1 : connected
 ****************************************************************************************
 */
int wifi_mgmr_sta_state_get(void);

/**
 ****************************************************************************************
 * @brief get sta specific state
 * return: state
 ****************************************************************************************
 */
int wifi_mgmr_sta_state_get_ext(void);

/**
 * wifi_mgmr_sta_info_status_code_get
 * Get status code
 * return:
 *  status code
 */
uint16_t wifi_mgmr_sta_info_status_code_get();
/**
 ****************************************************************************************
 * @brief get whether ap has started for ap mode
 * return:
 *  0 : not started
 *  1 : has started
 ****************************************************************************************
 */
int wifi_mgmr_ap_state_get(void);

/**
 ****************************************************************************************
 * @brief get ap specific state
 * return: state
 ****************************************************************************************
 */
int wifi_mgmr_ap_state_get_ext(void);

/**
 * wifi_mgmr_conf_max_sta
 * Set max number of sta supported for ap
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_conf_max_sta(uint8_t max_sta_supported);

/**
 * wifi_mgmr_ap_sta_delete
 * Del sta in ap mode
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_ap_sta_delete(uint8_t sta_idx);

/**
 * wifi_mgmr_raw_80211_send
 * Send raw packet
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_raw_80211_send(const wifi_mgmr_raw_send_params_t *config);

/**
 * wifi_mgmr_psk_cal
 * Calucate psk by using passpharse
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_psk_cal(char *password, const uint8_t *ssid, int ssid_len, char *output);

/**
 * wifi_mgmr_wifi_pwr_off
 * Turn off wifi pwr
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_wifi_pwr_off(void);

/**
 * wifi_mgmr_wifi_pwr_on
 * Turn on wifi pwr
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_wifi_pwr_on(void);

/**
 * wifi_mgmr_rate_config
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 *0xFFFF: disable fixed rate
 */
int wifi_mgmr_rate_config(uint16_t fixed_rate_cfg);

/**
 * wifi_mgmr_set_ht40_enable
 * Toggle state of HT40 mode
 * Attention: configuration takes effect only 
 *            before the initiation of the WiFi task.
 * param:
 *  value : 1 = enable; 0 = disable
 * return:
 *  0 : Success
 *  -1 : Failed
 *  Others is Failed
 */
int wifi_mgmr_set_ht40_enable(uint8_t value);

/**
 * wifi_mgmr_coex_enable
 * Enable or disable wifi coex
 */
void wifi_mgmr_coex_enable(bool en);

/**
 * wifi_mgmr_set_mode
 * Set sta/ap mode (e.g. b/g/n/ax)
 */
int wifi_mgmr_set_mode(uint8_t ap_or_sta, int mode);

/**
 * wifi_mgmr_get_mode
 * Get sta/ap mode (e.g. b/g/n/ax)
 */
int wifi_mgmr_get_mode(uint8_t ap_or_sta);
#endif
