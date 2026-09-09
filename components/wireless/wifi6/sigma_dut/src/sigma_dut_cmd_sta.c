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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <event_groups.h>

#include <lwip/inet.h>
#include <lwip/netif.h>

#include <ipro_event.h>
#include <wifi_mgmr_ext.h>

#include "sigma_dut.h"
#include "sigma_dut_priv.h"

IPRO_EVENT_DECLARE_BASE(WIFI_EVENT);

/* Event bits for synchronization */
#define SIGMA_EVT_CONNECTED    (1 << 0)
#define SIGMA_EVT_DISCONNECTED (1 << 1)
#define SIGMA_EVT_GOT_IP       (1 << 2)
#define SIGMA_EVT_SCAN_DONE    (1 << 3)

/* Accumulated state between set_security/set_wireless and associate */
static struct {
    EventGroupHandle_t events;
    int initialized;

    /* Security params (accumulated by sta_set_security/sta_set_encryption) */
    char ssid[33];
    char passphrase[65];
    char akm[16];          /* "WPA2-PSK", "SAE", "OWE", "OPEN" */
    uint8_t pmf;           /* 0=disabled, 1=optional, 2=required */
    char pairwise[16];     /* "AES-CCMP", "TKIP" */

    /* IP config */
    uint8_t use_dhcp;
    uint32_t ip, mask, gw, dns;

    /* Wireless params */
    int mode;              /* WIFI_MODE bitfield */
} sta_ctx;

/* --- Event handler --- */

static void sigma_wifi_event_handler(const ipro_event_t *event, void *user_data)
{
    if (!sta_ctx.events)
        return;

    switch (event->event_id) {
    case CODE_WIFI_ON_CONNECTED:
        xEventGroupSetBits(sta_ctx.events, SIGMA_EVT_CONNECTED);
        break;
    case CODE_WIFI_ON_DISCONNECT:
        xEventGroupSetBits(sta_ctx.events, SIGMA_EVT_DISCONNECTED);
        break;
    case CODE_WIFI_ON_GOT_IP:
        xEventGroupSetBits(sta_ctx.events, SIGMA_EVT_GOT_IP);
        break;
    case CODE_WIFI_ON_SCAN_DONE:
    case CODE_WIFI_ON_SCAN_DONE_ONJOIN:
        xEventGroupSetBits(sta_ctx.events, SIGMA_EVT_SCAN_DONE);
        break;
    default:
        break;
    }
}

static void sigma_sta_ensure_init(void)
{
    if (sta_ctx.initialized)
        return;

    sta_ctx.events = xEventGroupCreate();
    ipro_event_handler_register(WIFI_EVENT, IPRO_EVENT_ANY_ID,
                                sigma_wifi_event_handler, NULL);
    sta_ctx.use_dhcp = 1;
    sta_ctx.initialized = 1;
}

/* --- Command handlers --- */

static int cmd_sta_reset_default(struct sigma_conn *conn,
                                 const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    wifi_sta_disconnect();

    /* Clear accumulated state */
    memset(sta_ctx.ssid, 0, sizeof(sta_ctx.ssid));
    memset(sta_ctx.passphrase, 0, sizeof(sta_ctx.passphrase));
    strncpy(sta_ctx.akm, "OPEN", sizeof(sta_ctx.akm));
    sta_ctx.pmf = 0;
    memset(sta_ctx.pairwise, 0, sizeof(sta_ctx.pairwise));
    sta_ctx.use_dhcp = 1;
    sta_ctx.ip = sta_ctx.mask = sta_ctx.gw = sta_ctx.dns = 0;
    sta_ctx.mode = 0;

    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_set_ip_config(struct sigma_conn *conn,
                                 const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    const char *dhcp = sigma_cmd_get_param(cmd, "dhcp");
    if (dhcp) {
        sta_ctx.use_dhcp = (atoi(dhcp) != 0) ? 1 : 0;
    }

    if (!sta_ctx.use_dhcp) {
        const char *ip = sigma_cmd_get_param(cmd, "ip");
        const char *mask = sigma_cmd_get_param(cmd, "mask");
        const char *gw = sigma_cmd_get_param(cmd, "defaultgateway");

        if (ip)   sta_ctx.ip   = ipaddr_addr(ip);
        if (mask) sta_ctx.mask = ipaddr_addr(mask);
        if (gw)   sta_ctx.gw   = ipaddr_addr(gw);
    }

    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_get_ip_config(struct sigma_conn *conn,
                                 const struct sigma_cmd *cmd)
{
    uint32_t ip, mask, gw, dns;
    char ip_str[16], mask_str[16], gw_str[16];

    if (wifi_sta_ip4_addr_get(&ip, &mask, &gw, &dns) < 0)
        return sigma_resp_error(conn, "errorCode,NoIPAddress");

    strncpy(ip_str, inet_ntoa(*(struct in_addr *)&ip), sizeof(ip_str));
    strncpy(mask_str, inet_ntoa(*(struct in_addr *)&mask), sizeof(mask_str));
    strncpy(gw_str, inet_ntoa(*(struct in_addr *)&gw), sizeof(gw_str));

    return sigma_resp_complete(conn,
        "dhcp,%d,ip,%s,mask,%s,defaultGateway,%s",
        sta_ctx.use_dhcp, ip_str, mask_str, gw_str);
}

static int cmd_sta_set_security(struct sigma_conn *conn,
                                const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    const char *ssid = sigma_cmd_get_param(cmd, "ssid");
    const char *pass = sigma_cmd_get_param(cmd, "passphrase");
    const char *keymgmt = sigma_cmd_get_param(cmd, "keymgmttype");
    const char *encp = sigma_cmd_get_param(cmd, "encptype");
    const char *pmf = sigma_cmd_get_param(cmd, "pmf");

    if (ssid)
        strncpy(sta_ctx.ssid, ssid, sizeof(sta_ctx.ssid) - 1);
    if (pass)
        strncpy(sta_ctx.passphrase, pass, sizeof(sta_ctx.passphrase) - 1);

    /* Map key management type to akm_str */
    if (keymgmt) {
        if (strcasecmp(keymgmt, "wpa2") == 0 ||
            strcasecmp(keymgmt, "wpa2-psk") == 0) {
            strncpy(sta_ctx.akm, "WPA2-PSK", sizeof(sta_ctx.akm));
        } else if (strcasecmp(keymgmt, "sae") == 0 ||
                   strcasecmp(keymgmt, "wpa3") == 0) {
            strncpy(sta_ctx.akm, "SAE", sizeof(sta_ctx.akm));
            if (sta_ctx.pmf < 2)
                sta_ctx.pmf = 2; /* WPA3 requires PMF */
        } else if (strcasecmp(keymgmt, "owe") == 0) {
            strncpy(sta_ctx.akm, "OWE", sizeof(sta_ctx.akm));
            if (sta_ctx.pmf < 2)
                sta_ctx.pmf = 2;
        } else if (strcasecmp(keymgmt, "open") == 0 ||
                   strcasecmp(keymgmt, "none") == 0) {
            strncpy(sta_ctx.akm, "OPEN", sizeof(sta_ctx.akm));
        }
    }

    if (encp)
        strncpy(sta_ctx.pairwise, encp, sizeof(sta_ctx.pairwise) - 1);

    /* Map PMF */
    if (pmf) {
        if (strcasecmp(pmf, "required") == 0)
            sta_ctx.pmf = 2;
        else if (strcasecmp(pmf, "optional") == 0)
            sta_ctx.pmf = 1;
        else if (strcasecmp(pmf, "disabled") == 0)
            sta_ctx.pmf = 0;
    }

    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_set_encryption(struct sigma_conn *conn,
                                  const struct sigma_cmd *cmd)
{
    /* Alias for simpler encryption config — same fields */
    return cmd_sta_set_security(conn, cmd);
}

static int cmd_sta_set_wireless(struct sigma_conn *conn,
                                const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    const char *mode = sigma_cmd_get_param(cmd, "mode");

    if (mode) {
        if (strcasecmp(mode, "11b") == 0)
            sta_ctx.mode = 0x01;
        else if (strcasecmp(mode, "11g") == 0)
            sta_ctx.mode = 0x01 | 0x04;
        else if (strcasecmp(mode, "11n") == 0 ||
                 strcasecmp(mode, "11bg") == 0)
            sta_ctx.mode = 0x01 | 0x04 | 0x08;
        else if (strcasecmp(mode, "11ax") == 0 ||
                 strcasecmp(mode, "11bgn") == 0)
            sta_ctx.mode = 0x01 | 0x04 | 0x08 | 0x80;

        wifi_mgmr_set_mode(0, sta_ctx.mode);
    }

    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_associate(struct sigma_conn *conn,
                             const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    /* Override SSID from associate params if provided */
    const char *ssid = sigma_cmd_get_param(cmd, "ssid");
    if (ssid)
        strncpy(sta_ctx.ssid, ssid, sizeof(sta_ctx.ssid) - 1);

    if (!sta_ctx.ssid[0])
        return sigma_resp_error(conn, "errorCode,MissingSSID");

    /* Build connect params */
    wifi_mgmr_sta_connect_params_t params;
    memset(&params, 0, sizeof(params));

    strncpy(params.ssid, sta_ctx.ssid, sizeof(params.ssid) - 1);
    params.ssid_len = strlen(params.ssid);

    if (sta_ctx.passphrase[0]) {
        strncpy(params.key, sta_ctx.passphrase, sizeof(params.key) - 1);
        params.key_len = strlen(params.key);
    }

    if (strcmp(sta_ctx.akm, "OPEN") != 0) {
        strncpy(params.akm_str, sta_ctx.akm, sizeof(params.akm_str) - 1);
        params.akm_len = strlen(params.akm_str);
    }

    params.pmf_cfg = sta_ctx.pmf;
    params.use_dhcp = sta_ctx.use_dhcp;

    /*
     * Send COMPLETE response BEFORE triggering connect.
     *
     * wifi_mgmr_sta_connect() will disconnect the current WiFi first,
     * which kills the TCP socket carrying the CAPI session. If we wait
     * for association to complete before responding, the response can
     * never be delivered.
     *
     * WFA UCC uses sta_is_connected to poll for association result
     * after sta_associate returns.
     */
    sigma_resp_complete(conn, NULL);

    /* Small delay to let TCP flush the response */
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Clear event bits before connect */
    xEventGroupClearBits(sta_ctx.events,
        SIGMA_EVT_CONNECTED | SIGMA_EVT_DISCONNECTED | SIGMA_EVT_GOT_IP);

    int ret = wifi_mgmr_sta_connect(&params);
    if (ret < 0)
        return 0; /* response already sent */

    /* Wait for connection result (best-effort, response already sent) */
    EventBits_t wait_bits;
    if (sta_ctx.use_dhcp)
        wait_bits = SIGMA_EVT_GOT_IP | SIGMA_EVT_DISCONNECTED;
    else
        wait_bits = SIGMA_EVT_CONNECTED | SIGMA_EVT_DISCONNECTED;

    EventBits_t bits = xEventGroupWaitBits(sta_ctx.events, wait_bits,
                                           pdTRUE, pdFALSE,
                                           pdMS_TO_TICKS(30000));

    if (!sta_ctx.use_dhcp && (bits & SIGMA_EVT_CONNECTED)) {
        wifi_mgmr_sta_ip_set(sta_ctx.ip, sta_ctx.mask,
                             sta_ctx.gw, sta_ctx.dns);
    }

    return 0; /* response already sent */
}

static int cmd_sta_disconnect(struct sigma_conn *conn,
                              const struct sigma_cmd *cmd)
{
    wifi_sta_disconnect();
    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_get_bssid(struct sigma_conn *conn,
                             const struct sigma_cmd *cmd)
{
    uint8_t bssid[6];

    if (wifi_mgmr_sta_get_bssid(bssid) < 0)
        return sigma_resp_error(conn, "errorCode,NotConnected");

    return sigma_resp_complete(conn,
        "bssid,%02x:%02x:%02x:%02x:%02x:%02x",
        bssid[0], bssid[1], bssid[2],
        bssid[3], bssid[4], bssid[5]);
}

static int cmd_sta_is_connected(struct sigma_conn *conn,
                                const struct sigma_cmd *cmd)
{
    int state = wifi_mgmr_sta_state_get();
    return sigma_resp_complete(conn, "connected,%d", state);
}

static int cmd_sta_get_info(struct sigma_conn *conn,
                            const struct sigma_cmd *cmd)
{
    uint8_t mac[6];
    int rssi = 0, channel = 0;

    wifi_mgmr_sta_mac_get(mac);
    wifi_mgmr_sta_rssi_get(&rssi);
    wifi_mgmr_sta_channel_get(&channel);

    return sigma_resp_complete(conn,
        "mac,%02x:%02x:%02x:%02x:%02x:%02x,rssi,%d,channel,%d",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
        rssi, channel);
}

static int cmd_sta_scan(struct sigma_conn *conn,
                        const struct sigma_cmd *cmd)
{
    sigma_sta_ensure_init();

    wifi_mgmr_scan_params_t params;
    memset(&params, 0, sizeof(params));

    const char *ssid = sigma_cmd_get_param(cmd, "ssid");
    if (ssid) {
        params.ssid_length = strlen(ssid);
        strncpy((char *)params.ssid_array, ssid, sizeof(params.ssid_array) - 1);
    }

    xEventGroupClearBits(sta_ctx.events, SIGMA_EVT_SCAN_DONE);

    if (wifi_mgmr_sta_scan(&params) < 0)
        return sigma_resp_error(conn, "errorCode,ScanFailed");

    EventBits_t bits = xEventGroupWaitBits(sta_ctx.events, SIGMA_EVT_SCAN_DONE,
                                           pdTRUE, pdFALSE,
                                           pdMS_TO_TICKS(15000));
    if (!(bits & SIGMA_EVT_SCAN_DONE))
        return sigma_resp_error(conn, "errorCode,ScanTimeout");

    return sigma_resp_complete(conn, NULL);
}

static int cmd_sta_set_pwrsave(struct sigma_conn *conn,
                               const struct sigma_cmd *cmd)
{
    const char *mode = sigma_cmd_get_param(cmd, "powersave");
    if (!mode)
        mode = sigma_cmd_get_param(cmd, "mode");

    if (mode && strcasecmp(mode, "on") == 0)
        wifi_mgmr_sta_ps_enter();
    else
        wifi_mgmr_sta_ps_exit();

    return sigma_resp_complete(conn, NULL);
}

/* --- Command table --- */

const struct sigma_cmd_entry sigma_sta_commands[] = {
    { "sta_reset_default",  cmd_sta_reset_default },
    { "sta_set_ip_config",  cmd_sta_set_ip_config },
    { "sta_get_ip_config",  cmd_sta_get_ip_config },
    { "sta_set_security",   cmd_sta_set_security },
    { "sta_set_encryption", cmd_sta_set_encryption },
    { "sta_set_wireless",   cmd_sta_set_wireless },
    { "sta_associate",      cmd_sta_associate },
    { "sta_disconnect",     cmd_sta_disconnect },
    { "sta_get_bssid",      cmd_sta_get_bssid },
    { "sta_is_connected",   cmd_sta_is_connected },
    { "sta_get_info",       cmd_sta_get_info },
    { "sta_scan",           cmd_sta_scan },
    { "sta_set_pwrsave",    cmd_sta_set_pwrsave },
    { NULL, NULL }
};
