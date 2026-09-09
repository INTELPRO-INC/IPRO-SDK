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

#include <fhost_connect.h>
#include <fhost_wpa.h>
#include "fhost_config.h"
#include "fhost_cntrl.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "ipro_fw_api.h"
#include "export/phy.h"

#include "platform_al.h"
#include "rtos_al.h"
#include "net_al_ext.h"
#include "eloop_rtos.h"

/* TODO: LP stub -- bl_lp.h removed; re-enable for post-ASIC low-power */

#include "cli_al.h"

/*just limit packet len with a reasonable value*/
#define LEN_PKT_RAW_80211   (480)

// akm_str must be all in upper case
static int fhost_read_akm(const char *akm_str, uint32_t *akm, bool show_error)
{
    const char *next_akm, *cur_akm = akm_str;
    int ret = 0;

    next_akm = strchr(cur_akm, ',');

    *akm = 0;
    while (cur_akm)
    {
        if (strncmp(cur_akm, "OPEN", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_NONE);
        }
        else if (strncmp(cur_akm, "WEP", 3) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN);
        }
        else if ((strncmp(cur_akm, "RSN", 3) == 0) ||
                 (strncmp(cur_akm, "WPA2", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_PSK) | CO_BIT(MAC_AKM_PSK_SHA256);
        }
        else if ((strncmp(cur_akm, "SAE", 3) == 0) ||
                 (strncmp(cur_akm, "WPA3", 4) == 0))
        {
            *akm |= CO_BIT(MAC_AKM_SAE);
        }
        else if (strncmp(cur_akm, "WPA", 4) == 0)
        {
            *akm |= CO_BIT(MAC_AKM_PRE_RSN) | CO_BIT(MAC_AKM_PSK);
        }
        else
        {
            ret++;
        }

        cur_akm = next_akm;
        if (cur_akm)
        {
            cur_akm++; //skip coma
            next_akm = strchr(cur_akm, ',');
        }
    }

    if (ret && show_error)
    {
        fhost_print(RTOS_TASK_NULL, "[%s] Only the following AKM are supported:\n"
                    "OPEN: For open AP\n"
#if 0 // not supported now
                    "WEP: For AP with WEP security\n"
#endif
                    "WPA: For AP with WPA/PSK security (pre WPA2)\n"
                    "RSN (or WPA2): For AP with WPA2/PSK security\n"
                    "SAE (or WPA3): For AP with WPA3/PSK security\n"
                    , akm_str);
    }
    return ret;
}

/// Power Save mode setting
enum
{
    /// Power-save off
    MGMR_PS_MODE_OFF,
    /// Power-save on - Normal mode
    MGMR_PS_MODE_ON,
    /// Power-save on - Dynamic mode
    MGMR_PS_MODE_ON_DYN,
};

wifi_mgmr_t wifiMgmr;

extern int fhost_ipc_wpa_cli_api(int fhost_vif_idx, char *cmd, char **resp_result);
extern int fhost_ipc_wpa_cli_api_free(char *resp_result);

static int check_wifi_ready(void)
{
    if (0 == wifiMgmr.ready) {
        fhost_printf("Run WifiMgmr init first...\r\n");
        return -1;
    }

    return 0;
}

char *wifi_mgmr_mode_to_str(uint32_t mode)
{
    switch (mode)
    {
    case (WIFI_MODE_802_11B):
        return "B";
    case (WIFI_MODE_802_11B | WIFI_MODE_802_11G):
        return "BG";
    case (WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4):
        return "BGN";
    case (WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4 | WIFI_MODE_802_11AX_2_4):
        return "BGNAX";
    default:
        return "Unknown";
    }
}

void show_auth_cipher(struct mac_scan_result *result)
{
    bool sep = false;
    uint16_t mfp;
    int i;

    if (result->akm & CO_BIT(MAC_AKM_NONE)) {
        fhost_printf(" [OPEN]\n");
         return;
    }

    if (result->akm == CO_BIT(MAC_AKM_PRE_RSN)) {
        fhost_printf(" [WEP]\n");
        return;
    }

    if (result->akm & CO_BIT(MAC_AKM_WAPI_CERT) ||
        result->akm & CO_BIT(MAC_AKM_WAPI_PSK)) {
        bool cert = result->akm & CO_BIT(MAC_AKM_WAPI_CERT);
        bool psk = result->akm & CO_BIT(MAC_AKM_WAPI_PSK);
        fhost_printf("[WAPI:%s%s%s %s/%s]\n",
                    cert ? "CERT" : "",
                    cert && psk ? "-" : "",
                    psk ? "PSK" : "",
                    result->group_cipher == CO_BIT(MAC_CIPHER_WPI_SMS4) ? "SMS4" : "?",
                    result->pairwise_cipher == CO_BIT(MAC_CIPHER_WPI_SMS4) ? "SMS4" : "?");
        return;
    }

    if (result->akm & CO_BIT(MAC_AKM_PRE_RSN))
        fhost_printf(" [WPA:");
    else
        fhost_printf(" [RSN:");

    for (i = MAC_AKM_8021X ; i <= MAC_AKM_OWE ; i++)
    {
        if (result->akm & CO_BIT(i))
        {
            fhost_ipc_print_akm(i, sep);
            sep = true;
        }
    }

    fhost_printf(" ");
    sep = false;
    for (i = MAC_CIPHER_WEP40 ; i <= MAC_CIPHER_BIP_CMAC_256 ; i++)
    {
        if (result->pairwise_cipher & CO_BIT(i))
        {
            fhost_ipc_print_cipher(i, sep);
            sep = true;
        }
    }
    fhost_printf("/");

    // remove MFP cipher from group
    mfp = (CO_BIT(MAC_CIPHER_BIP_CMAC_128) | CO_BIT(MAC_CIPHER_BIP_GMAC_128) |
           CO_BIT(MAC_CIPHER_BIP_GMAC_256) | CO_BIT(MAC_CIPHER_BIP_CMAC_256));

    mfp = result->group_cipher & mfp;
    result->group_cipher &= ~(mfp);

    if (result->group_cipher)
        fhost_ipc_print_cipher(31 - co_clz(result->group_cipher), false);
    else
        fhost_printf("?");

    if (mfp)
    {
        fhost_printf("][MFP:");
        sep = false;
        for (i = MAC_CIPHER_BIP_CMAC_128 ; i <= MAC_CIPHER_BIP_CMAC_256 ; i++)
        {
              if (mfp & CO_BIT(i))
              {
                  fhost_ipc_print_cipher(i, sep);
                  sep = true;
              }
        }
    }

    fhost_printf("]\n");

}

static void fill_sta_connect_params(const char *ssid, const char *key, const char *bssid, const char *akm_str, 
                                    uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint16_t duration, 
                                    uint16_t probe_cnt, uint8_t use_dhcp, wifi_mgmr_sta_connect_params_t *config)
{
    memset(config, 0 ,sizeof(*config));

    if (ssid) {
        if(strlcpy(config->ssid, ssid, MGMR_SSID_LEN + 1) >= MGMR_SSID_LEN + 1) {
            printf("[OS]: strlcpy truncated \r\n");
        }
        config->ssid_len = strlen(config->ssid);
    }

    if (key) {
        if(strlcpy(config->key, key, MGMR_KEY_LEN + 1) >= MGMR_KEY_LEN + 1) {
            printf("[OS]: strlcpy truncated \r\n");
        }
        config->key_len = strlen(config->key);
    }

    if (bssid) {
        if(strlcpy(config->bssid_str, bssid, MGMR_BSSID_LEN) >= MGMR_BSSID_LEN) {
            printf("[OS]: strlcpy truncated \r\n");
        }
    }

    if (akm_str) {
        if(strlcpy(config->akm_str, akm_str, MGMR_AKM_LEN) >= MGMR_AKM_LEN) {
            printf("[OS]: strlcpy truncated \r\n");
        }
        config->akm_len = strlen(config->akm_str);
    }

    config->pmf_cfg = pmf_cfg;
    config->freq1 = freq1;
    config->freq2 = freq2;
    config->use_dhcp = use_dhcp;
    config->duration = duration;
    config->probe_cnt = probe_cnt;
    config->auth_timeout = 0;
    config->eapol_1_timeout = 0;
    config->eapol_rem_timeout = 0;
}

int wifi_sta_scan_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg,
                          uint16_t freq1, uint16_t freq2, uint16_t duration, uint16_t probe_cnt, uint8_t use_dhcp)
{
    wifi_mgmr_sta_connect_params_t config;
    fill_sta_connect_params(ssid, key, bssid, akm_str, pmf_cfg, freq1, freq2, duration, probe_cnt, use_dhcp, &config);
    return wifi_mgmr_sta_connect(&config);
}

int wifi_sta_connect(const char *ssid, const char *key, const char *bssid, const char *akm_str, uint8_t pmf_cfg, uint16_t freq1, uint16_t freq2, uint8_t use_dhcp)
{
    return wifi_sta_scan_connect(ssid, key, bssid, akm_str, pmf_cfg, freq1, freq2, 0, 0, use_dhcp);
}

int wifi_mgmr_sta_quickconnect(const char *ssid, const char *key, uint16_t freq1, uint16_t freq2)
{
    wifi_mgmr_sta_connect_params_t config;
    fill_sta_connect_params(ssid, key, NULL, NULL, 1, freq1, freq2, 0, 0, 1, &config);
    config.quick_connect = 1;
    config.wpa_flags = FHOST_WPA_SSID_PREFER_WPA2_TO_WPA3;
    return wifi_mgmr_sta_connect(&config);
}

int wifi_mgmr_sta_connect(const wifi_mgmr_sta_connect_params_t *config)
{
    //fhost_printf("wifi_mgmr_sta_connect: ssid:<%s>/<%d> key:<%s>/<%d> akm:<%s>/<%d> %d-%d-%d-%d\r\n",
    //        config->ssid,config->ssid_len,config->key,config->key_len,config->akm_str,config->akm_len,config->pmf_cfg,config->freq1,config->freq2,config->use_dhcp);
    struct fhost_ipc_connect_cfg cfg;
    size_t ssid_len = config->ssid_len;
    size_t key_len = config->key_len;

    if (check_wifi_ready())
        return -1;

    memset(&cfg, 0, sizeof(cfg));

    cfg.fhost_vif_idx = MGMR_VIF_STA;

    // timeout for connnect, default 200s (this var will impact auto connect when scan an unexist ssid)
    if (config->timeout_ms != 0) {
        cfg.cfg.timeout_ms = config->timeout_ms;
    } else {
        cfg.cfg.timeout_ms = 200000;
    }

    if (ssid_len > sizeof(cfg.cfg.ssid.array))
    {
        fhost_printf("Invalid SSID\r\n");
        return -1;
    }
    wifi_mgmr_sta_info_reset();
    memcpy(cfg.cfg.ssid.array, config->ssid, ssid_len);
    wifi_mgmr_sta_ssid_set(config->ssid);
    cfg.cfg.ssid.length = ssid_len;

    fhost_ipc_parse_mac_addr(config->bssid_str, &cfg.cfg.bssid);

    if ((key_len + 1) > sizeof(cfg.cfg.key))
    {
        fhost_printf("Invalid Key\r\n");
        return -1;
    }
    if (config->key_len != 0) {
        memcpy(cfg.cfg.key, config->key, config->key_len);
        wifi_mgmr_sta_passphr_set(config->key);
    }

    cfg.cfg.pmf = config->pmf_cfg;
    cfg.cfg.freq[0] = config->freq1;
    cfg.cfg.freq[1] = config->freq2;
    cfg.cfg.duration = config->duration;
    cfg.cfg.probe_cnt = config->probe_cnt;
    cfg.cfg.auth_timeout = config->auth_timeout;
    cfg.cfg.eapol_1_timeout = config->eapol_1_timeout;
    cfg.cfg.eapol_rem_timeout = config->eapol_rem_timeout;

    if (config->akm_len != 0) {
        fhost_read_akm(config->akm_str, &cfg.cfg.akm, true);
    }

    if (cfg.cfg.ssid.length == 0) {
        fhost_printf("cfg.cfg.ssid.length == 0 \r\n");
        return -1;
    }

    cfg.cfg.wpa_flags = config->wpa_flags;

    cfg.link = cntrl_link;

    fhost_printf("start connecting ... \r\n");

    PLATFORM_HOOK(prevent_sleep, PSM_EVENT_CONNECT, 1);

    memcpy(&wifiMgmr.sta_connect_param, config, sizeof(*config));

    fhost_ipc_connect_task_start(&cfg);

    return 0;
}

int wifi_sta_disconnect(void)
{
    /* An explicit disconnect must not be undone by the auto-reconnect backoff
     * armed on the disconnect indication - that path exists for deauths we did
     * not ask for. wifi_sta_connect() re-enables it. */
    net_al_ext_sta_reconnect_suppress();

    // Disable network connection
    if (fhost_wpa_disable_network(MGMR_VIF_STA)) {
       return -1;
    }
    if (fhost_wpa_remove_vif(MGMR_VIF_STA)) {
       return -1;
    }

    return 0;
}

int wifi_mgmr_sta_ip_set(uint32_t ip, uint32_t mask, uint32_t gw, uint32_t dns)
{

    struct net_al_ext_ip_addr_cfg ip_cfg;
    ip_cfg.mode = IP_ADDR_STATIC_IPV4;
    ip_cfg.default_output = true;
    ip_cfg.ipv4.addr = ip;
    ip_cfg.ipv4.mask = mask;
    ip_cfg.ipv4.gw = gw;
    ip_cfg.ipv4.dns = dns;

    if (net_al_ext_set_vif_ip(MGMR_VIF_STA, &ip_cfg)) {
        fhost_printf("set static ipv4 error!\r\n");
        return -1;
    } else {
        fhost_printf("set static ipv4 success!\r\n");
    }

    return 0;
}

int wifi_sta_ip4_addr_get(uint32_t *addr, uint32_t *mask, uint32_t *gw, uint32_t *dns)
{
    uint8_t vif_idx = MGMR_VIF_STA;
    struct net_al_ext_ip_addr_cfg ip_cfg;

    if (fhost_env.vif[vif_idx].mac_vif &&
        (mac_vif_get_type(fhost_env.vif[vif_idx].mac_vif) == VIF_STA) &&
        mac_vif_get_active(fhost_env.vif[vif_idx].mac_vif)) {
        if (net_al_ext_get_vif_ip(vif_idx, &ip_cfg)) {
            return -1;
        }
        if (addr) {
            *addr = ip_cfg.ipv4.addr;
        }
        if (mask) {
            *mask = ip_cfg.ipv4.mask;
        }
        if (gw) {
            *gw = ip_cfg.ipv4.gw;
        }
        if (dns) {
            *dns = ip_cfg.ipv4.dns;
        }
        return 0;
    }

    return -1;
}

#if NX_FHOST_MONITOR
int wifi_mgmr_sniffer_enable(wifi_mgmr_sniffer_item_t sniffer_item)
{
    int fhost_vif_idx = 0;
    const struct {
        uint8_t name;
        uint8_t val;
    } bwmap[] = {
        { .name = 20, .val = PHY_CHNL_BW_20, },
        { .name = 40, .val = PHY_CHNL_BW_40, },
        { .name = 80, .val = PHY_CHNL_BW_80, },
        { .name = 80+80, .val = PHY_CHNL_BW_80P80, },
        { .name = 160, .val = PHY_CHNL_BW_160, },
    };

    struct fhost_vif_monitor_cfg cfg;
    struct mac_chan_def *chan;
    unsigned int freq_offset;

    if (check_wifi_ready())
        return -1;

    // get the interface index
    fhost_vif_idx = fhost_search_itf(sniffer_item.itf);
    if (fhost_vif_idx < 0) {
        fhost_print(RTOS_TASK_NULL, "Invalid itf %s\r\n", sniffer_item.itf);
        return -1;
    }

    // frequency
    cfg.chan.prim20_freq = sniffer_item.prim20_freq;
    chan = fhost_chan_get(cfg.chan.prim20_freq);
    if (chan == NULL) {
        fhost_print(RTOS_TASK_NULL, "Invalid freq %d\r\n", cfg.chan.prim20_freq);
        return -1;
    }
    cfg.chan.band = chan->band;
    cfg.chan.tx_power = chan->tx_power;

    // by default 20Mhz bandwidth
    cfg.chan.type = PHY_CHNL_BW_20;
    cfg.chan.center1_freq = cfg.chan.prim20_freq;
    cfg.chan.center2_freq = 0;

    // bw
    if (sniffer_item.type != 0) {
        for (int i = 0; i < CO_ARRAY_SIZE(bwmap); i++) {
            if (bwmap[i].name == sniffer_item.type)
            {
                cfg.chan.type = bwmap[i].val;
                break;
            }
        }
    }

    // center1_freq
    if (sniffer_item.center1_freq != 0) {
        cfg.chan.center1_freq = sniffer_item.center1_freq;
    }

    if (cfg.chan.center1_freq > cfg.chan.prim20_freq)
        freq_offset = cfg.chan.center1_freq - cfg.chan.prim20_freq;
    else
        freq_offset = cfg.chan.prim20_freq - cfg.chan.center1_freq;

    switch(cfg.chan.type) {
        case PHY_CHNL_BW_20:
            if (freq_offset != 0) {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                            "Center frequency of primary channel different from "
                            "frequency of primary channel in 20MHz (%d != %d)\r\n",
                            cfg.chan.center1_freq, cfg.chan.prim20_freq);
                return -1;
            }
            break;
        case PHY_CHNL_BW_40:
            if (freq_offset != 10) {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                            "Center frequency of primary channel different from "
                            "frequency of primary channel +/- 10 in 40MHz (%d != %d)\r\n",
                            cfg.chan.center1_freq, cfg.chan.prim20_freq);
                return -1;
            }
            break;
        case PHY_CHNL_BW_80P80:
            //center2_freq
            if (sniffer_item.center2_freq != 0) {
                cfg.chan.center2_freq = sniffer_item.center2_freq;
            } else {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                            "Center frequency of secondary channel must be set\r\n");
                return -1;
            }

            //adjacent channel rejection
            if ((cfg.chan.center1_freq - cfg.chan.center2_freq == 80) ||
                (cfg.chan.center2_freq - cfg.chan.center1_freq == 80)) {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                            "Adjacent channel is not allowed, use 160MHz bandwidth\r\n");
                return -1;
            }

            __attribute__ ((fallthrough));
        case PHY_CHNL_BW_80:
            if ((freq_offset != 10) && (freq_offset != 30)) {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                            "Center frequency of primary channel different from "
                            "frequency of primary channel +/- 10 and frequency of"
                            "primary channel +/- 30 (%d != %d)\r\n",
                            cfg.chan.center1_freq, cfg.chan.prim20_freq);
                return -1;
            }
            break;
        case PHY_CHNL_BW_160:
            if ((freq_offset != 10) && (freq_offset != 30) &&
                (freq_offset != 50) && (freq_offset != 70)) {
                fhost_print(RTOS_TASK_NULL, "monitor_start :"
                        "Center frequency of primary channel must belong to the range:"
                        "frequency of primary channel +/- [10, 30, 50, 70]\r\n");
                return -1;
            }
            break;
        default:
            fhost_print(RTOS_TASK_NULL, "monitor_start :"
                        "Invalid bandwidth %d\r\n", cfg.chan.type);
            return -1;
    }

    if (fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_MONITOR, false)) {
        fhost_print(RTOS_TASK_NULL, "Error while enabling monitor mode\r\n");
        return -1;
    }

    cfg.uf = true;
    cfg.cb = sniffer_item.cb;
    cfg.cb_arg = sniffer_item.cb_arg;

    if (fhost_cntrl_monitor_cfg(cntrl_link, fhost_vif_idx, &cfg)) {
        fhost_print(RTOS_TASK_NULL, "Error while configuring monitor mode\r\n");
        return -1;
    }

    return 0;
}

int wifi_mgmr_sniffer_disable(wifi_mgmr_sniffer_item_t sniffer_item)
{
    int fhost_vif_idx = 0;

    if (!sniffer_item.itf) {
        fhost_print(RTOS_TASK_NULL, "Set itf\r\n");
        return -1;
    }

    // get the interface index
    fhost_vif_idx = fhost_search_itf(sniffer_item.itf);
    if (fhost_vif_idx < 0) {
        fhost_print(RTOS_TASK_NULL, "Invalid itf %s\r\n", sniffer_item.itf);
        return -1;
    }

    if (fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_STA, false)) {
        fhost_print(RTOS_TASK_NULL, "Error while disabling monitor mode\r\n");
        return -1;
    }

    return 0;
}
#endif

void wifi_mgmr_sta_info_dump(wifi_mgmr_connect_ind_stat_info_t *connection_info)
{
    int passphr_len = strlen(connection_info->passphr);
    fhost_print(RTOS_TASK_NULL, "Connected\r\n");
    fhost_print(RTOS_TASK_NULL, "ssid:            %s\r\n", connection_info->ssid);
    fhost_print(RTOS_TASK_NULL, "passphr:         %s\r\n", passphr_len ? "********" : "Open");
    fhost_print(RTOS_TASK_NULL, "bssid:           %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                connection_info->bssid[0],
                                connection_info->bssid[1],
                                connection_info->bssid[2],
                                connection_info->bssid[3],
                                connection_info->bssid[4],
                                connection_info->bssid[5]);
    fhost_print(RTOS_TASK_NULL, "aid:             %d\r\n", connection_info->aid);
    fhost_print(RTOS_TASK_NULL, "channel:         %d\r\n", connection_info->channel);
    fhost_print(RTOS_TASK_NULL, "security:        %d\r\n", connection_info->security);
}

void wifi_mgmr_ap_info_dump(wifi_mgmr_ap_info_t *ap)
{

    wifi_mgmr_ap_info_t *ap_info= &wifiMgmr.ap_info;
    if (NULL != ap) {
        ap_info = ap;
    }
    fhost_print(RTOS_TASK_NULL, "ssid:            %s\r\n", ap_info->ucSSID);
    fhost_print(RTOS_TASK_NULL, "passphr:         %s\r\n", "********");
    fhost_print(RTOS_TASK_NULL, "bssid:           %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                                ap_info->ucBSSID[0],
                                ap_info->ucBSSID[1],
                                ap_info->ucBSSID[2],
                                ap_info->ucBSSID[3],
                                ap_info->ucBSSID[4],
                                ap_info->ucBSSID[5]);
    fhost_print(RTOS_TASK_NULL, "security:        %d\r\n", ap_info->ucSecurity);
    fhost_print(RTOS_TASK_NULL, "channel:         %d\r\n", ap_info->ucChannel);
}

uint16_t wifi_mgmr_sta_info_status_code_get()
{
    return (wifiMgmr.wifi_mgmr_stat_info.status_code);
}

int wifi_mgmr_sta_state_get(void)
{
    return (fhost_get_vif_state(MGMR_VIF_STA) == FHOST_STA_CONNECTED);
}

int wifi_mgmr_sta_state_get_ext(void)
{
    return fhost_get_vif_raw_state(MGMR_VIF_STA);
}

int wifi_mgmr_ap_state_get(void)
{
    return (fhost_get_vif_state(MGMR_VIF_AP) == FHOST_AP_ENABLED);
}

int wifi_mgmr_ap_state_get_ext(void)
{
    return fhost_get_vif_raw_state(MGMR_VIF_AP);
}

int wifi_mgmr_state_get(void)
{
    int fhost_vif_idx;
    struct fhost_vif_status vif_status;
    char vif_name[4];

    fhost_print(RTOS_TASK_NULL, "-------------------------------------\r\n");
    for (int i=0; i<2; i++) {
        fhost_vif_idx = i;
        fhost_get_vif_status(fhost_vif_idx, &vif_status);
        fhost_vif_name(fhost_vif_idx, vif_name, sizeof(vif_name) - 1);
        vif_name[3] = '\0';
        fhost_print(RTOS_TASK_NULL, "vif_name:        %s\r\n", vif_name);
        fhost_print(RTOS_TASK_NULL, "vif_idx:         %d\r\n", fhost_vif_idx);
        fhost_print(RTOS_TASK_NULL, "vif_state:       ");
        switch (vif_status.type)
        {
            case VIF_STA:
                fhost_print(RTOS_TASK_NULL, "STA\r\n");
                fhost_print(RTOS_TASK_NULL, "state:           ");
                if (vif_status.chan.prim20_freq != 0) {
                    wifi_mgmr_sta_info_dump(&wifiMgmr.wifi_mgmr_stat_info);
                } else {
                    fhost_print(RTOS_TASK_NULL, "Not Connect\r\n");
                }
                break;
            case VIF_AP:
                fhost_print(RTOS_TASK_NULL, "AP\r\n");
                if (vif_status.chan.prim20_freq != 0) {
                    wifi_mgmr_ap_info_dump(NULL);
                } else {
                }
                break;
            case VIF_MONITOR:
                fhost_print(RTOS_TASK_NULL, "MONITOR\r\n");
                break;
            case VIF_MESH_POINT:
                fhost_print(RTOS_TASK_NULL, "MESH\r\n");
                break;
            default:
                fhost_print(RTOS_TASK_NULL, "INACTIVE\r\n");
                break;
        }
        fhost_print(RTOS_TASK_NULL, "-------------------------------------\r\n");
    }

    return 0;
}

int wifi_mgmr_sta_rssi_get(int *rssi)
{
    int fhost_vif_idx = MGMR_VIF_STA;
    struct fhost_vif_status vif_status;

    fhost_get_vif_status(fhost_vif_idx, &vif_status);
    *rssi = vif_status.sta.rssi;
    return 0;
}

int wifi_mgmr_sta_channel_get(int *channel)
{
    int fhost_vif_idx = MGMR_VIF_STA;
    struct fhost_vif_status vif_status;
    char vif_name[4];
    uint16_t freq;

    fhost_get_vif_status(fhost_vif_idx, &vif_status);
    fhost_vif_name(fhost_vif_idx, vif_name, sizeof(vif_name) - 1);
    if (vif_status.chan.prim20_freq != 0) {
#if 0
        fhost_print(RTOS_TASK_NULL, "%s: VIF[%d] : Operating Channel: %dMHz@%dMHz\r\n",
                    vif_name,
                    fhost_vif_idx,
                    vif_status.chan.prim20_freq,
                    (1 << vif_status.chan.type) * 20);
#endif
        freq = vif_status.chan.prim20_freq;
        *channel = phy_freq_to_channel(vif_status.chan.band, freq);

        return 0;
    } else {
        fhost_print(RTOS_TASK_NULL, "%s: VIF[%d] : No Operating Channel\r\n", vif_name, fhost_vif_idx);
        return -1;
    }
}

int wifi_mgmr_sta_ssid_set(const char *ssid)
{
    int len = strlen(ssid);

    if (len > 0 && len <= MGMR_SSID_LEN) {
        memcpy(wifiMgmr.wifi_mgmr_stat_info.ssid, ssid, len);
        wifiMgmr.wifi_mgmr_stat_info.ssid[len] = 0;
        return 0;
    }

    return -1;
}

int wifi_mgmr_sta_ssid_get(char *ssid)
{
    int len = strlen(wifiMgmr.wifi_mgmr_stat_info.ssid);

    if (len > 0 && len <= MGMR_SSID_LEN) {
        memcpy(ssid, wifiMgmr.wifi_mgmr_stat_info.ssid, len);
        ssid[len] = 0;
        return 0;
    }

    return -1;
}

int wifi_mgmr_sta_passphr_set(const char *passphr)
{
    int len = strlen(passphr);

    if (len > 0 && len <= MGMR_KEY_LEN) {
        memcpy(wifiMgmr.wifi_mgmr_stat_info.passphr, passphr, len);
        wifiMgmr.wifi_mgmr_stat_info.passphr[len] = 0;
        return 0;
    }

    return -1;
}

int wifi_mgmr_sta_connect_ind_stat_get(wifi_mgmr_connect_ind_stat_info_t *wifi_mgmr_ind_stat)
{
    int fhost_vif_idx = MGMR_VIF_STA;
    struct fhost_vif_status vif_status;
    char vif_name[4];

    fhost_get_vif_status(fhost_vif_idx, &vif_status);
    fhost_vif_name(fhost_vif_idx, vif_name, sizeof(vif_name) - 1);
    if (vif_status.chan.prim20_freq != 0) {
        int ssid_len = strlen(wifiMgmr.wifi_mgmr_stat_info.ssid);
        if (ssid_len > 0) {
            memcpy(wifi_mgmr_ind_stat->ssid, wifiMgmr.wifi_mgmr_stat_info.ssid, ssid_len);
            wifi_mgmr_ind_stat->ssid[ssid_len] = '\0';
        }

        int passphr_len = strlen(wifiMgmr.wifi_mgmr_stat_info.passphr);
        if (passphr_len > 0) {
            memcpy(wifi_mgmr_ind_stat->passphr, wifiMgmr.wifi_mgmr_stat_info.passphr, passphr_len);
            wifi_mgmr_ind_stat->passphr[passphr_len] = '\0';
        }
        memcpy(wifi_mgmr_ind_stat->bssid, wifiMgmr.wifi_mgmr_stat_info.bssid, sizeof(wifiMgmr.wifi_mgmr_stat_info.bssid));
        wifi_mgmr_ind_stat->aid = wifiMgmr.wifi_mgmr_stat_info.aid;
        wifi_mgmr_ind_stat->channel = wifiMgmr.wifi_mgmr_stat_info.channel;
        wifi_mgmr_ind_stat->security = wifiMgmr.wifi_mgmr_stat_info.security;

        /*
        fhost_print(RTOS_TASK_NULL, "ssid: %s, passphr: %s\r\n",
            wifi_mgmr_ind_stat->ssid,
            passphr_len ? wifi_mgmr_ind_stat->passphr : "Open");
        */
        wifi_mgmr_sta_info_dump(wifi_mgmr_ind_stat);

        return 0;
    } else {
        fhost_print(RTOS_TASK_NULL, "No Connect\r\n");
        return -1;
    }
}

int wifi_mgmr_rate_config(uint16_t fixed_rate_cfg)
{
    struct cfgrwnx_rc_set_rate cmd;
    struct cfgrwnx_resp resp;

    if (check_wifi_ready())
        return -1;

    if (!wifi_mgmr_sta_state_get()) {
        return -1;
    }
    // prepare CFGRWNX_RC_SET_RATE_CMD to send
    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_RC_SET_RATE_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;
    cmd.sta_idx = wifiMgmr.wifi_mgmr_stat_info.ap_idx;
    cmd.fixed_rate_cfg = (uint16_t) fixed_rate_cfg;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_RC_SET_RATE_RESP;

    // Send CFGRWNX_RC_SET_RATE_CMD
    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS))
    {
        return -1;
    }
    return 0;

}

int wifi_mgmr_sta_scan(const wifi_mgmr_scan_params_t *config)
{
    struct cfgrwnx_scan cmd;
    struct cfgrwnx_resp resp;
    struct cfgrwnx_scan_ssid ssid;

    if (check_wifi_ready())
        return -1;

    ssid.len = config->ssid_length;
    ssid.ssid = ssid.len ? config->ssid_array : NULL;
    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SCAN_CMD;
    cmd.fhost_vif_idx = MGMR_VIF_STA;
    cmd.freqs = NULL;
    cmd.chans_cnt = config->channels_cnt;
    cmd.chans = cmd.chans_cnt ? config->channels : NULL;
    cmd.extra_ies = NULL;
    cmd.ssids = &ssid;
    cmd.ssid_cnt = 1;
    cmd.bssid = config->bssid_set_flag ? config->bssid : NULL;
    cmd.extra_ies_len = 0;
    cmd.no_cck = 0;
    cmd.duration = config->duration;
    //if is_cntrl_link is true, donot need scan sock
    //cmd.sock = cntrl_link->sock_send;
    cmd.is_cntrl_link = true;
    cmd.hdr.resp_queue = cntrl_link->queue;
    cmd.passive = config->passive;
    cmd.flags = 0;
    cmd.probe_cnt = config->probe_cnt;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SCAN_RESP;

    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS))
        return -1;

    return 0;
}

static inline int wifi_mgmr_scan_item_is_timeout(wifi_mgmr_t *mgmr, wifi_mgmr_scan_item_t *item)
{
    return ((unsigned int)rtos_now(0) - (unsigned int)item->timestamp_lastseen) >= mgmr->scan_item_timeout ? 1 : 0;
}


int wifi_mgmr_sta_scanlist(void)
{
    int i;

    fhost_printf("cached scan list\r\n");
    fhost_printf("****************************************************************************************************\r\n");
    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if (wifiMgmr.scan_items[i].is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &wifiMgmr.scan_items[i]))) {
            fhost_printf("index[%02d]: channel %02u, bssid %02X:%02X:%02X:%02X:%02X:%02X, rssi %3d, ppm abs:rel %3d : %3d, wps %2d, mode %6s, auth %20s, cipher:%12s, SSID %s\r\n",
                    i,
                    wifiMgmr.scan_items[i].channel,
                    wifiMgmr.scan_items[i].bssid[0],
                    wifiMgmr.scan_items[i].bssid[1],
                    wifiMgmr.scan_items[i].bssid[2],
                    wifiMgmr.scan_items[i].bssid[3],
                    wifiMgmr.scan_items[i].bssid[4],
                    wifiMgmr.scan_items[i].bssid[5],
                    wifiMgmr.scan_items[i].rssi,
                    wifiMgmr.scan_items[i].ppm_abs,
                    wifiMgmr.scan_items[i].ppm_rel,
                    wifiMgmr.scan_items[i].wps,
                    wifi_mgmr_mode_to_str(wifiMgmr.scan_items[i].mode),
                    wifi_mgmr_auth_to_str(wifiMgmr.scan_items[i].auth),
                    wifi_mgmr_cipher_to_str(wifiMgmr.scan_items[i].cipher),
                    wifiMgmr.scan_items[i].ssid
            );
        } else {
            fhost_printf("index[%02d]: empty\r\n", i);
        }
    }
    fhost_printf("----------------------------------------------------------------------------------------------------\r\n");
    return 0;
}


uint32_t wifi_mgmr_sta_scanlist_nums_get()
{
    uint32_t i, cnt = 0;

    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]); i++) {
        if (wifiMgmr.scan_items[i].is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &wifiMgmr.scan_items[i]))) {
            cnt++;
        }
    }

    return cnt;
}

uint32_t wifi_mgmr_sta_scanlist_dump(void * results, uint32_t resultNums)
{
    int i, j = 0;
    wifi_mgmr_scan_item_t *scanResults = (wifi_mgmr_scan_item_t *)results;

    for (i = 0; i < sizeof(wifiMgmr.scan_items)/sizeof(wifiMgmr.scan_items[0]) && j < resultNums; i++) {
        if (wifiMgmr.scan_items[i].is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &wifiMgmr.scan_items[i]))) {
            memcpy(&scanResults[j++], &wifiMgmr.scan_items[i], sizeof(wifi_mgmr_scan_item_t));
        }
    }

    return j;
}

int wifi_mgmr_scan_filter_hidden_ssid(int filter)
{
    rtos_protect();
    if (filter) {
        wifiMgmr.features &= (~WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID);
    } else {
        wifiMgmr.features |= WIFI_MGMR_FEATURES_SCAN_SAVE_HIDDEN_SSID;
    }
    rtos_unprotect(0);
    return 0;
}

int wifi_mgmr_scan_ap_all(void *env, void *arg, scan_item_cb_t cb)
{
    int i = 0;

    for (i = 0; i < sizeof(wifiMgmr.scan_items) / sizeof(wifiMgmr.scan_items[0]); i++) {
        if (wifiMgmr.scan_items[i].is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &wifiMgmr.scan_items[i]))) {
            cb(env, arg, &wifiMgmr.scan_items[i]);
        }
    }

    return 0;
}

static void wifi_mgmr_basic_ap_info_save(struct fhost_vif_ap_cfg *cfg)
{
    wifi_mgmr_ap_info_t *ap_info= &wifiMgmr.ap_info;
    if(cfg == NULL){
        return;
    }

    /* save ap cfg info */
    memset(ap_info, 0, sizeof(wifi_mgmr_ap_info_t));
    memcpy(ap_info->ucSSID, cfg->ssid.array, cfg->ssid.length);
    ap_info->ucSSIDLength = cfg->ssid.length;
    memcpy(ap_info->ucPWD, cfg->key, strlen(cfg->key));
    ap_info->ucChannel = phy_freq_to_channel(cfg->chan.band, cfg->chan.prim20_freq);
    /**
     * Bit field of Authentication and Key Management (@ref mac_akm_suite)
     * Supported combination are:\n
     * - MAC_AKM_NONE : for open security
     * - MAC_AKM_PRE_RSN : for WEP security (deprecated)
     * - MAC_AKM_PRE_RSN | MAC_AKM_PSK : For WPA1 security (deprecated)
     * - MAC_AKM_PSK : For WPA2 security
     * - MAC_AKM_SAE : For WPA3 security
     * - MAC_AKM_PSK | MAC_AKM_SAE : For WPA2/WPA3 security
     */
    if(cfg->akm & CO_BIT(MAC_AKM_NONE)){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_OPEN;
    }else if(cfg->akm & CO_BIT(MAC_AKM_PRE_RSN)){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_WEP;
    }else if((cfg->akm & CO_BIT(MAC_AKM_PRE_RSN)) && (cfg->akm & CO_BIT(MAC_AKM_PSK))){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK;
    }else if(cfg->akm & CO_BIT(MAC_AKM_PSK)){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK;
    }else if(cfg->akm & CO_BIT(MAC_AKM_SAE)){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE;
    }else if((cfg->akm & CO_BIT(MAC_AKM_PSK)) && (cfg->akm & CO_BIT(MAC_AKM_SAE))){
        ap_info->ucSecurity = WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE;
    }else{

    }
    wifi_mgmr_ap_mac_get(ap_info->ucBSSID);

    return ;
}

static void wifi_mgmr_basic_ap_info_release()
{
    wifi_mgmr_ap_info_t *ap_info= &wifiMgmr.ap_info;
    memset(ap_info, 0, sizeof(wifi_mgmr_ap_info_t));
    return;
}

int wifi_mgmr_ap_start(const wifi_mgmr_ap_params_t *config)
{
    struct fhost_vif_ap_cfg cfg;
    struct net_al_ext_ip_addr_cfg ip_cfg;
    int res = -1;
    int fhost_vif_idx = MGMR_VIF_AP;

    if (check_wifi_ready())
        return -1;

    if (wifiMgmr.wlan_ap.started) {
        fhost_printf("ap has started");
        return -1;
    }

    wifiMgmr.wlan_ap.started = 1;

    memset(&cfg, 0, sizeof(cfg));

    size_t ssid_len = strlen(config->ssid);
    if (ssid_len == 0 || ssid_len > sizeof(cfg.ssid.array)) {
        fhost_printf("Invalid SSID");
        goto end;
    }
    memcpy(cfg.ssid.array, config->ssid, ssid_len);
    cfg.ssid.length = ssid_len;

    if (config->key != NULL) {
        size_t key_len = strlen(config->key);
        if ((key_len + 1) > sizeof(cfg.key)) {
            fhost_printf("Invalid Key");
            goto end;
        }
        if(strlcpy(cfg.key, config->key, sizeof(cfg.key)) >= sizeof(cfg.key)) {
            printf("[OS]: strlcpy truncated \r\n");
        }
    }

    if (IS_11B_ONLY(wifiMgmr.ap_mode) || IS_11G_EN(wifiMgmr.ap_mode)) {
        cfg.mode = FHOST_AP_NON_HT;
    }

    if (IS_11AX_EN(wifiMgmr.ap_mode)) {
        cfg.mode = FHOST_AP_HE;
    } else if (IS_11N_EN(wifiMgmr.ap_mode)) {
        cfg.mode = FHOST_AP_HT;
    }

    cfg.chan.type = PHY_CHNL_BW_20;
    cfg.chan.band = PHY_BAND_2G4;
    if (config->type != 0) {
        cfg.chan.type = config->type;
    }
    if (config->channel != 0) {
        cfg.chan.prim20_freq = phy_channel_to_freq(cfg.chan.band, config->channel);
    } else {
        // default channel is 6
        cfg.chan.prim20_freq = phy_channel_to_freq(cfg.chan.band, 6);
    }

    if (PHY_CHNL_BW_20 == cfg.chan.type) {
        cfg.chan.center1_freq = cfg.chan.prim20_freq;
    }
    if (PHY_CHNL_BW_40 == cfg.chan.type) {
        //for example, ht40-
        cfg.chan.center1_freq = cfg.chan.prim20_freq - 10;
    }

    struct mac_chan_def *chan = NULL;
    chan = fhost_chan_get(cfg.chan.prim20_freq);
    if (!chan) {
        fhost_printf("Invalid channel\n");
        goto end;
    }

    if (NULL != config->akm && fhost_read_akm(config->akm, &cfg.akm, true))
        goto end;

    if ((cfg.ssid.length == 0) || (cfg.chan.prim20_freq == 0)) {
         goto end;
    }
    #define OPT_LEN 50
    if (config->ap_max_inactivity) {
        cfg.extra_cfg = rtos_malloc(OPT_LEN);
        if (!cfg.extra_cfg) {
            fhost_printf("%s: no more memory\r\n", __func__);
            goto end;
        }

        dbg_snprintf(cfg.extra_cfg, OPT_LEN, "%s%u;", "ap_max_inactivity ", config->ap_max_inactivity);

    }

    #define SET_AP_VENDOR_ELEMENTS "SET ap_vendor_elements "
    #define MAX_AP_VENDOR_ELEMENTS_LEN (ELOOP_MSG_INBUF_LEN - 36)
    if (config->ap_vendor_elements) {
        if (strlen(config->ap_vendor_elements) > MAX_AP_VENDOR_ELEMENTS_LEN) {
            fhost_printf("config->ap_vendor_elements exceeded %d bytes\r\n", MAX_AP_VENDOR_ELEMENTS_LEN);
            goto end;
        }
        cfg.ap_vendor_elements = rtos_malloc(strlen(config->ap_vendor_elements) + strlen(SET_AP_VENDOR_ELEMENTS) + 1);
        if (!cfg.ap_vendor_elements) {
            fhost_printf("%s: no more memory\r\n", __func__);
            goto end;
        }

        dbg_snprintf(cfg.ap_vendor_elements, strlen(config->ap_vendor_elements) + strlen(SET_AP_VENDOR_ELEMENTS) + 1, "%s%s", SET_AP_VENDOR_ELEMENTS, config->ap_vendor_elements);
    }
    // try to select the best AKM if not set
    if (cfg.akm == 0) {
        if (strlen(cfg.key) == 0)
            cfg.akm = CO_BIT(MAC_AKM_NONE);
#if 0 // not support WEP now
        else if (strlen(cfg.key) == 5)
            cfg.akm = CO_BIT(MAC_AKM_PRE_RSN);
#endif
        else
            cfg.akm = CO_BIT(MAC_AKM_PSK);
    } else if (config->key == NULL) {
        // default key
        if(strlcpy(cfg.key, "12345678", sizeof(cfg.key)) >= sizeof(cfg.key)) {
            printf("[OS]: strlcpy truncated \r\n");
        }
    }

    if (cfg.akm & CO_BIT(MAC_AKM_SAE) && cfg.mfp == 0) {
        cfg.mfp = 1;
    }

    if (config->use_ipcfg) {
        // Save IP configuration (if any)
        net_al_ext_get_vif_ip(fhost_vif_idx, &ip_cfg);
    }

    // (Re)Set interface type to AP
    if (fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_UNKNOWN, false) ||
        fhost_set_vif_type(cntrl_link, fhost_vif_idx, VIF_AP, false))
        goto end;

    // Set hidden ssid
    cfg.hidden_ssid = config->hidden_ssid;

    // Set isolation mode
    cfg.isolation_mode = config->isolation;

    cfg.bcn_interval = config->bcn_interval;

    if (fhost_ap_cfg(fhost_vif_idx, &cfg, false)) {
        fhost_printf("Failed to start AP, check your configuration");
        goto end;
    }

    /* save ap cfg info */
    wifi_mgmr_basic_ap_info_save(&cfg);

    if (config->use_ipcfg) {
        // Reconfigure IP only if it was static
        if (ip_cfg.mode == IP_ADDR_STATIC_IPV4) {
            #ifdef CONFIG_WIFI_GATEWAY
            ip_cfg.default_output = false;
            #else
            ip_cfg.default_output = true;
            #endif
            // set ap addr and mask
            ip_cfg.ipv4.addr = config->ap_ipaddr;
            ip_cfg.ipv4.mask = config->ap_mask;

            net_al_ext_set_vif_ip(fhost_vif_idx, &ip_cfg);
        }

        if (config->use_dhcpd) {
            net_al_dhcpd_start(fhost_to_net_if(fhost_vif_idx), config->start, config->limit);
            wifiMgmr.use_dhcpd = 1;
        }

        #ifdef CONFIG_WIFI_GATEWAY
        net_al_gw_service_enable(fhost_to_net_if(fhost_vif_idx));
        #endif
    } else {
        net_al_link_set(fhost_to_net_if(fhost_vif_idx));
    }

    res = 0;

  end:
    if (res)
        wifiMgmr.wlan_ap.started = 0;
    if (cfg.extra_cfg)
        rtos_free(cfg.extra_cfg);
    if (cfg.ap_vendor_elements)
        rtos_free(cfg.ap_vendor_elements);

    return res;
}

int wifi_mgmr_ap_stop(void)
{
#ifdef CONFIG_WIFI_GATEWAY
    net_al_gw_service_disable(fhost_to_net_if(MGMR_VIF_AP));
#endif

    if (wifiMgmr.use_dhcpd) {
        net_al_dhcpd_stop(fhost_to_net_if(MGMR_VIF_AP));
        wifiMgmr.use_dhcpd = 0;
    }

    // Disable network connection
    if (fhost_wpa_disable_network(MGMR_VIF_AP)) {
       return -1;
    }
    if (fhost_wpa_remove_vif(MGMR_VIF_AP)) {
       return -1;
    }

    /* release ap cfg info */
    wifi_mgmr_basic_ap_info_release();
    wifiMgmr.wlan_ap.started = 0;

    return 0;
}

int wifi_mgmr_mac_set(uint8_t mac[6])
{
    memcpy(wifiMgmr.wlan_sta.mac, mac, 6);
    // mac[5] ^= 0x01;
    #ifdef CONFIG_UNSINGLE_MAC_ADDR
    mac[0] ^= 0x02;
    #endif
    memcpy(wifiMgmr.wlan_ap.mac, mac, 6);
    wifiMgmr.wlan_sta.set = 1;
    wifiMgmr.wlan_ap.set = 1;
    return 0;
}

static int _mac_use_ramdom(uint8_t *addr)
{
    static uint8_t random[6] = {0x00};

    if (random[0] != 0x00) {
        //Now AP and STA different MAC addresses are not support.
        goto _end;
    }

    if (platform_get_random(random, 6) < 0) {
        //XXX: change to ASSERT 
        fhost_printf("Get random fail\n\r");
        while(1);
    }
    random[0] &= 0xfe; /* unicast */
    random[0] |= 0x02; /* locally administered */
   
_end:
    memcpy(addr, random, 6);
    fhost_printf("Warning: MAC use random address \n\r");
    return 0;
}

int wifi_mgmr_sta_mac_get(uint8_t mac[6])
{
    if (wifiMgmr.wlan_sta.set == 0) {
        if (platform_get_mac(wifiMgmr.wlan_sta.mac)) {
            _mac_use_ramdom(wifiMgmr.wlan_sta.mac);
        } else if ((wifiMgmr.wlan_sta.mac[0] == 0x00) &&
            (wifiMgmr.wlan_sta.mac[1] == 0x00) &&
            (wifiMgmr.wlan_sta.mac[2] == 0x00) &&
            (wifiMgmr.wlan_sta.mac[3] == 0x00) &&
            (wifiMgmr.wlan_sta.mac[4] == 0x00) &&
            (wifiMgmr.wlan_sta.mac[5] == 0x00)) {
            _mac_use_ramdom(wifiMgmr.wlan_sta.mac);
        }
        fhost_printf("get sta mac: %02x:%02x:%02x:%02x:%02x:%02x\n\r",
                wifiMgmr.wlan_sta.mac[0], wifiMgmr.wlan_sta.mac[1], wifiMgmr.wlan_sta.mac[2],
                wifiMgmr.wlan_sta.mac[3], wifiMgmr.wlan_sta.mac[4], wifiMgmr.wlan_sta.mac[5]);
        wifiMgmr.wlan_sta.set = 1;
    }

    memcpy(mac, wifiMgmr.wlan_sta.mac, 6);
    return 0;
}

int wifi_mgmr_ap_mac_get(uint8_t mac[6])
{
    if (wifiMgmr.wlan_ap.set == 0) {
        if (platform_get_mac(wifiMgmr.wlan_ap.mac)) {
            _mac_use_ramdom(wifiMgmr.wlan_ap.mac);
        } else if ((wifiMgmr.wlan_ap.mac[0] == 0x00) &&
            (wifiMgmr.wlan_ap.mac[1] == 0x00) &&
            (wifiMgmr.wlan_ap.mac[2] == 0x00) &&
            (wifiMgmr.wlan_ap.mac[3] == 0x00) &&
            (wifiMgmr.wlan_ap.mac[4] == 0x00) &&
            (wifiMgmr.wlan_ap.mac[5] == 0x00)) {
            _mac_use_ramdom(wifiMgmr.wlan_ap.mac);
        }
        // wifiMgmr.wlan_ap.mac[5] ^= 0x01;
        #ifdef CONFIG_UNSINGLE_MAC_ADDR
        wifiMgmr.wlan_ap.mac[0] ^= 0x02;
        #endif
        fhost_printf("get ap mac: %02x:%02x:%02x:%02x:%02x:%02x\n\r",
                wifiMgmr.wlan_ap.mac[0], wifiMgmr.wlan_ap.mac[1], wifiMgmr.wlan_ap.mac[2],
                wifiMgmr.wlan_ap.mac[3], wifiMgmr.wlan_ap.mac[4], wifiMgmr.wlan_ap.mac[5]);
        wifiMgmr.wlan_ap.set = 1;
    }

    memcpy(mac, wifiMgmr.wlan_ap.mac, 6);
    return 0;
}

int wifi_mgmr_get_country_code(char *country_code)
{
    if (country_code == NULL){
        return -1;
    }

    memcpy(country_code, wifiMgmr.country_code, sizeof(wifiMgmr.country_code));
    return 0;
}

int wifi_mgmr_set_country_code(char *country_code)
{
    int ret;
    if(strlcpy(wifiMgmr.country_code, country_code, sizeof(wifiMgmr.country_code)) >= sizeof(wifiMgmr.country_code)) {
        printf("[OS]: strlcpy truncated \r\n");
    }
    wifiMgmr.country_code[2] = '\0';
    wifiMgmr.channel_nums = wifi_mgmr_get_channel_nums(country_code);
    ret = fhost_cntrl_chan_config_update(wifiMgmr.channel_nums);
    if (ret != 0) {
        fhost_printf("%s channel config failed\r\n", __func__);
        return -1;
    }

    fhost_printf("%s:code = %s\r\n", __func__, country_code);
    return 0;
}

int wifi_mgmr_sta_autoconnect_enable(void)
{
    wifiMgmr.disable_autoreconnect = 0;

    if (check_wifi_ready())
        return -1;

    if (FHOST_WPA_STATE_STOPPED == fhost_wpa_get_state(MGMR_VIF_STA)) {
        goto end;
    }

    if (fhost_wpa_execute_cmd(MGMR_VIF_STA, NULL, 0, 300, "STA_AUTOCONNECT %u", !wifiMgmr.disable_autoreconnect)) {
        return -1;
    }
end:
    fhost_printf("Enable Auto Reconnect\r\n");
    return 0;
}

int wifi_mgmr_sta_autoconnect_disable(void)
{
    wifiMgmr.disable_autoreconnect = 1;

    if (check_wifi_ready())
        return -1;

    if (FHOST_WPA_STATE_STOPPED == fhost_wpa_get_state(MGMR_VIF_STA)) {
        goto end;
    }

    if (fhost_wpa_execute_cmd(MGMR_VIF_STA, NULL, 0, 300, "STA_AUTOCONNECT %u", !wifiMgmr.disable_autoreconnect)) {
        return -1;
    }
end:
    fhost_printf("Disable Auto Reconnect\r\n");
    return 0;
}

int wifi_mgmr_sta_ps_enter(void)
{
    struct cfgrwnx_set_ps_mode cmd;
    struct cfgrwnx_resp resp;
    int fhost_vif_idx = MGMR_VIF_STA;

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SET_PS_MODE_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SET_PS_MODE_RESP;

    cmd.ps_mode = MGMR_PS_MODE_ON_DYN;
    cmd.enabled = true;

    if (check_wifi_ready())
        return -1;

    cmd.fhost_vif_idx = fhost_vif_idx;
    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr))
        return -1;

    return 0;
}

/*
 * Static (forced) power-save.
 *
 * Unlike wifi_mgmr_sta_ps_enter (MGMR_PS_MODE_ON_DYN), this uses
 * MGMR_PS_MODE_ON: the MAC keeps the PM bit set to 1 regardless of traffic, so
 * the STA does not auto-exit PS when frames flow. The AP therefore keeps
 * treating us as asleep — buffering our unicast and setting our TIM bit — which
 * is exactly what a DTIM deep-sleep needs. Dynamic PS would toggle PM back to 0
 * the moment we ACK/reply, and the AP would stop buffering (TIM bit never set).
 */
int wifi_mgmr_sta_ps_enter_static(void)
{
    struct cfgrwnx_set_ps_mode cmd;
    struct cfgrwnx_resp resp;
    int fhost_vif_idx = MGMR_VIF_STA;

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SET_PS_MODE_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SET_PS_MODE_RESP;

    cmd.ps_mode = MGMR_PS_MODE_ON;
    cmd.enabled = true;

    if (check_wifi_ready())
        return -1;

    cmd.fhost_vif_idx = fhost_vif_idx;
    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr))
        return -1;

    return 0;
}

int wifi_mgmr_sta_ps_exit(void)
{
    struct cfgrwnx_set_ps_mode cmd;
    struct cfgrwnx_resp resp;
    int fhost_vif_idx = MGMR_VIF_STA;

    if (check_wifi_ready())
        return -1;

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_SET_PS_MODE_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_SET_PS_MODE_RESP;

    cmd.ps_mode = MGMR_PS_MODE_OFF;
    cmd.enabled = false;

    cmd.fhost_vif_idx = fhost_vif_idx;
    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr))
        return -1;

    return 0;
}

int wifi_mgmr_sta_set_listen_itv(uint8_t itv)
{
    if (check_wifi_ready())
        return -1;
    return fhost_set_listen_itv(itv);
}

uint8_t wifi_mgmr_sta_get_listen_itv(void)
{
    if (check_wifi_ready())
        return -1;
    return fhost_get_listen_itv();
}

int wifi_mgmr_sta_aid_get(void)
{
    uint8_t vif_idx = MGMR_VIF_STA;
    struct fhost_vif_status vif_status;
    char vif_name[4];
    int16_t aid = -1;

    if (fhost_env.vif[vif_idx].mac_vif &&
        (mac_vif_get_type(fhost_env.vif[vif_idx].mac_vif) == VIF_STA) &&
        mac_vif_get_active(fhost_env.vif[vif_idx].mac_vif)) {
        fhost_get_vif_status(vif_idx, &vif_status);
        fhost_vif_name(vif_idx, vif_name, sizeof(vif_name) - 1);
        aid = vif_status.sta.aid;
    }

    return aid;
}

int wifi_mgmr_sta_get_bssid(uint8_t bssid[6])
{
    struct fhost_vif_status vif_status;

    fhost_get_vif_status(MGMR_VIF_STA, &vif_status);

    if (vif_status.chan.prim20_freq != 0) {
        memcpy(bssid, wifiMgmr.wifi_mgmr_stat_info.bssid, sizeof(wifiMgmr.wifi_mgmr_stat_info.bssid));

        return 0;
    } else {
        return -1;
    }
}

int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx)
{
    rtos_mutex_lock(wifiMgmr.ap_sta_info_lock);
    sta_info->is_used = wifiMgmr.ap_sta_info[idx].is_used;
    sta_info->sta_idx = wifiMgmr.ap_sta_info[idx].sta_idx;
    sta_info->aid = wifiMgmr.ap_sta_info[idx].aid;
    memcpy(sta_info->sta_mac, wifiMgmr.ap_sta_info[idx].sta_mac, 6);
    rtos_mutex_unlock(wifiMgmr.ap_sta_info_lock);
    return 0;
}

int wifi_mgmr_tpc_pwr_set(rf_pwr_table_t *power_table)
{
    tpc_update_power_table((int8_t *)power_table->pwr_11b);

    return 0;
}

int wifi_mgmr_tpc_pwr_get(rf_pwr_table_t *power_table)
{
    tpc_power_table_get((int8_t *)power_table->pwr_11b);

    return 0;
}

int wifi_mgmr_sta_keepalive_time_set(uint8_t time_seconds)
{
    sta_set_keepalive_period(time_seconds);
    return 0;
}

int wifi_mgmr_get_ap_info(wifi_mgmr_ap_info_t *ap)
{
    wifi_mgmr_ap_info_t *ap_info= &wifiMgmr.ap_info;
    if(ap == NULL){
        return -1;
    }

    memcpy(ap, ap_info, sizeof(wifi_mgmr_ap_info_t));
    wifi_mgmr_ap_info_dump(ap);
    return 0;
}

int wifi_mgmr_conf_max_sta(uint8_t max_sta_supported)
{
    if (max_sta_supported <= CFG_STA_MAX) {
        wifiMgmr.max_sta_supported = max_sta_supported;
        fhost_printf("Conf Max Sta to %u\r\n", max_sta_supported);
    } else {
        fhost_printf("You should set max_sta_supported <= %d\r\n", CFG_STA_MAX);
    }
    return 0;
}

int wifi_mgmr_ap_sta_delete(uint8_t sta_idx)
{
    if (!wifiMgmr.ap_sta_info[sta_idx].is_used) {
        fhost_printf("STA %d already remove\r\n", sta_idx);
        return 0;
    }

    fhost_wpa_ap_sta_del(MGMR_VIF_AP, wifiMgmr.ap_sta_info[sta_idx].sta_mac);

    return 0;
}

int wifi_mgmr_set_mode(uint8_t ap_or_sta, int mode)
{
    if (ap_or_sta) {
        wifiMgmr.ap_mode = mode;
    }
    else {
        wifiMgmr.sta_mode = mode;
    }
    return 0;
}

int wifi_mgmr_get_mode(uint8_t ap_or_sta)
{
    return (ap_or_sta) ? wifiMgmr.ap_mode : wifiMgmr.sta_mode;
}

int wifi_mgmr_raw_80211_send(const wifi_mgmr_raw_send_params_t *config)
{
    struct cfgrwnx_raw_send cmd;
    struct cfgrwnx_resp resp;

    if (check_wifi_ready())
        return -1;

    if (config->len > LEN_PKT_RAW_80211) {
        /*raw packet len is too long*/
        fhost_printf("Raw packet len is too long");
        return -1;
    }

    if (!config->pkt || !config->len) {
        return -1;
    }

    if (config->channel > fhost_chan.chan2G4_cnt) {
        fhost_printf("Not supported channel\r\n");
        return -1;
    }

    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_RAW_SEND_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;
    cmd.fhost_vif_idx = MGMR_VIF_STA;
    cmd.pkt = config->pkt;
    cmd.len = config->len;
    cmd.channel = config->channel;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_RAW_SEND_RESP;

    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS)) {
        return -1;
    }

    return 0;
}

int wifi_mgmr_psk_cal(char *password, const uint8_t *ssid, int ssid_len, char *output)
{
    int ret = 1;
    char psk[32];

    extern int pbkdf2_sha1(const char *passphrase, const uint8_t *ssid, size_t ssid_len, int iterations, uint8_t *buf, size_t buflen);
    ret = pbkdf2_sha1(password, ssid, ssid_len, 4096, (uint8_t *)psk, sizeof(psk));
    if (0 == ret) {
        utils_al_bin2hex(output, psk, 32);
    }

    return ret;
}

int wifi_mgmr_set_ht40_enable(uint8_t value)
{
    if (!check_wifi_ready()){
        fhost_printf("configuration takes effect only before WifiMgmr init\r\n");
        return -1;
    }

    return fhost_set_ht40_mode_enable(value);
}

#if defined(CFG_IPRO_WIFI_PS_ENABLE) || defined(CFG_WIFI_PDS_RESUME)
int wifi_mgmr_wifi_pwr_off(void)
{
    if (wifi_mgmr_sta_state_get() || wifi_mgmr_ap_state_get()) {
        fhost_printf("Wifi is connected or ap mode is on, should not turn off wifi.");
        return -1;
    }

    /* TODO: LP stub -- implement for post-ASIC low-power RF control */
    /* bl_lp_turnoff_rf(); */

    return 0;
}

int wifi_mgmr_wifi_pwr_on(void)
{
    /* TODO: LP stub -- implement for post-ASIC low-power RF control */
    /* bl_lp_turnon_rf(); */

    return 0;
}
#endif

#if CONFIG_COEX_WIFI_MODE
void wifi_mgmr_coex_enable(bool en)
{
    fhost_set_coex_enable(en);
}
#endif
