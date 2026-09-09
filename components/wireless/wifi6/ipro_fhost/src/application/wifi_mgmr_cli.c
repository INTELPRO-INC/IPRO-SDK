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

#include "export/rwnx_config.h"
#include "fhost.h"
#include "fhost_wpa.h"
#include "fhost_cntrl.h"
#include "fhost_config.h"
#include "mac_frame.h"
#include "fhost_connect.h"
#include "fhost_rx.h"
#include "cfgrwnx.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"
#include "cli_al.h"
#include "net_al_ext.h"

#define MAC_ADDR_LIST(m) (m)[0], (m)[1], (m)[2], (m)[3], (m)[4], (m)[5]
#define DEFAULT_CHAN_FOR_RAW_PKT 6

void cmd_hello(int argc, char **argv)
{
    int i;
    fhost_printf("cmd_hello\r\n");
#if 1
    fhost_print(RTOS_TASK_NULL, "cmd_hello argc = %d\r\n", argc);
    for (i = 0; i < argc; i++) {
        fhost_print(RTOS_TASK_NULL, "cmd_hello argv[%d] = %s\r\n", i, argv[i]);
    }
#endif
}

void cmd_phy(int argc, char **argv)
{
    int i;

    fhost_printf("cmd_phy\r\n");
#if 1
    fhost_print(RTOS_TASK_NULL, "cmd_phy argc = %d\r\n", argc);
    for (i = 0; i < argc; i++) {
        fhost_print(RTOS_TASK_NULL, "cmd_phy argc = %s\r\n", argv[i]);
    }
#endif
    fhost_printf("riu_psselect_getf     *0x24C0B080 = 0x%08lx, = %ld, bit0_1 = 0x%01lx\r\n",
            (*(volatile uint32_t *)0x24C0B080),
            (*(volatile uint32_t *)0x24C0B080),
            (*(volatile uint32_t *)0x24C0B080) & 0x3);
    fhost_printf("mdm_primaryind_get    *0x24C00850 = 0x%08lx\r\n",
            (*(volatile uint32_t *)0x24C00850));
    fhost_printf("*0x24C00824           *0x24C00824 = 0x%08lx, = %ld, bit25_24 = 0x%01lx\r\n",
            (*(volatile uint32_t *)0x24C00824),
            (*(volatile uint32_t *)0x24C00824),
            ((*(volatile uint32_t *)0x24C00824)>>24) & 0x3);
    fhost_printf("dot11WEPExcludedCount *0x24C00800 = 0x%08lx, = %ld, bit0_3 = 0x%01lx\r\n",
            (*(volatile uint32_t *)0x24C00800),
            (*(volatile uint32_t *)0x24C00800),
            (*(volatile uint32_t *)0x24C00800) & 0xF);
}


#ifdef CMD_TXL_CNTRL_PUSH_AC_ENABLE
void cmd_ac_set(int argc, char **argv)
{
    int i;
    uint8_t ac;

    if (argc != 2) {
        fhost_printf("arg error.\r\n");
        return;
    }

    ac = (uint8_t)atoi(argv[1]);

    fhost_printf("set ac = %d\r\n", ac);

    extern void wifi_set_txl_cntrl_push_access_category(uint8_t type);
    wifi_set_txl_cntrl_push_access_category(ac);
}
#endif

//In order to do coexistence test, Made a temporary patch
//#ifdef CFG_FOR_COEXISTENCE_TEST_STOPAP_PATCH
void cmd_ap_stop(int argc, char **argv)
{
    extern int wpa_stop_ap(void);
    fhost_printf("--------------- 1 wpa_stop_ap\r\n");
    wpa_stop_ap();

    extern void wpa_rwnx_deinitap(void);
    ///wpa_rwnx_deinitap();
}
//#endif

static unsigned char char_to_hex(char asccode)
{
    unsigned char ret;

    if('0'<=asccode && asccode<='9')
        ret=asccode-'0';
    else if('a'<=asccode && asccode<='f')
        ret=asccode-'a'+10;
    else if('A'<=asccode && asccode<='F')
        ret=asccode-'A'+10;
    else
        ret=0;

    return ret;
}

static void chan_str_to_hex(uint8_t *sta_num, char *sta_str)
{
    int i, str_len, base=1;
    uint16_t val = 0;
    char *q;

    str_len = strlen(sta_str);
    q = sta_str;
    q[str_len] = '\0';
    for (i=0; i< str_len; i++) {
        val = val + char_to_hex(q[str_len-1-i]) * base;
        base = base * 10;
    }
    (*sta_num) = val;
    fhost_printf("sta_str: %s, str_len: %d, sta_num: %d, q: %s\r\n", sta_str, str_len, (*sta_num), q);

}

void wifi_scan_cmd(int argc, char **argv)
{
    int opt, ret;
    getopt_env_t getopt_env;

    wifi_mgmr_scan_params_t config;
    memset(&config, 0 , sizeof(wifi_mgmr_scan_params_t));

    utils_al_getopt_init(&getopt_env, 0);

    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "s:c:b:t:p:")) != -1) {
         switch (opt) {
             case 's':
             {
                 memcpy(config.ssid_array, getopt_env.optarg, MAC_SSID_LEN);
                 config.ssid_length = strlen(getopt_env.optarg);
                 fhost_printf("ssid: %s len: %d\r\n", config.ssid_array, config.ssid_length);
             }
             break;
             case 'c':
             {
                 utils_al_parse_number_adv(getopt_env.optarg, ',', config.channels, MAX_FIXED_CHANNELS_LIMIT, 10, &config.channels_cnt);
             }
             break;
             case 'b':
             {
                 config.bssid_set_flag = 1;
                 utils_al_parse_number(getopt_env.optarg, ':', config.bssid, 6, 16);
                 fhost_printf("bssid: %s, mac:%02X:%02X:%02X:%02X:%02X:%02X\r\n", getopt_env.optarg,
                          MAC_ADDR_LIST(config.bssid));
             }
             break;
             case 't':
             {
                 config.duration = atoi(getopt_env.optarg);
             }
             break;
             case 'p':
             {
                 config.probe_cnt = atoi(getopt_env.optarg);
             }
             break;
             default:
             {
                 fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
             }
         }

    }
    ret = wifi_mgmr_sta_scan(&config);
    if (ret < 0) {
        fhost_printf("scan failed \r\n");
        return;
    }
}

#define WEP_PASSWORD_BUF_LEN (13 * 2 + 1)

static void cvt_bin_2_hex(const void *in, size_t in_len, char *out)
{
    for (size_t i = 0; i < in_len; ++i) {
        uint8_t b = ((uint8_t *)in)[i];
        uint8_t b_l = b & 0xf;
        uint8_t b_h = b >> 4;
        *out++ = b_h <= 9 ? b_h + '0' : b_h - 10 + 'A';
        *out++ = b_l <= 9 ? b_l + '0' : b_l - 10 + 'A';
    }
    *out = '\0';
}

static bool is_valid_hex_str(const char *str)
{
    size_t len = strlen(str);
    if (len % 2) {
        return false;
    }
    for (size_t i = 0; i < len; ++i) {
        char c = str[i];
        if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))) {
            return false;
        }
    }
    return true;
}

static int cvt_wep_password(const char *in, char *out)
{
    size_t len = strlen(in);

    memset(out, 0, WEP_PASSWORD_BUF_LEN);

    if (len == 5 || len == 13) {
        cvt_bin_2_hex(in, len, out);
    } else if (len == 10 || len == 26) {
        if (!is_valid_hex_str(in)) {
            return -1;
        }
        if(strlcpy(out, in, WEP_PASSWORD_BUF_LEN) >= WEP_PASSWORD_BUF_LEN) {
            printf("[OS]: strlcpy truncated \r\n");
        }
    } else {
        return -1;
    }
    return 0;
}

/**
 * CLI options:
 * b: BSSID
 * 2: WPA2-Only
 * 3: WPA3-Only
 * f: PMF cfg, 0/1/2, defaults to 1(PMF Capable)
 * w: WEP. In this mode, the password can be given in ASCII or HEX format(wo prefix 0x)
 * D: Disable DHCP
 * c: channel index
 * q: quick_connect
 */
int wifi_sta_scan_connect(char *ssid, char *key, char *bssid, char *akm_str, uint8_t pmf_cfg,
                          uint16_t freq1, uint16_t freq2, uint16_t duration, uint16_t probe_cnt, uint8_t use_dhcp);
void wifi_connect_cmd(int argc, char **argv)
{
    getopt_env_t getopt_env;
    int opt;
    int bssid_set_flag = 0;
    char bssid[18] = {0};
    char *password = NULL;
    char password_buf[WEP_PASSWORD_BUF_LEN];
    int wpa2_only = 0, wpa3_only = 0;
    const char *akm = NULL;
    int pmf_cfg = 1;
    int use_dhcp = 1;
    int wep = 0;
    uint8_t channel_index = 0;
    uint16_t freq = 0;
    uint8_t quick_connect = 0;
    uint16_t duration = 0;
    uint16_t probe_cnt = 0;
    uint8_t auth_timeout = 0;
    uint8_t eapol_1_timeout = 0;
    uint8_t eapol_rem_timeout = 0;

    if (2 > argc) {
        goto _ERROUT;
    }

    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "b:23f:qwDc:d:p:a:u:e:t:")) != -1) {
        switch (opt) {
        case 'b':
            bssid_set_flag = 1;
            memcpy(bssid, getopt_env.optarg, 17);
            fhost_printf("bssid: %s \r\n", getopt_env.optarg);
            break;
        case 'f':
            pmf_cfg = atoi(getopt_env.optarg);
            if (!(0 <= pmf_cfg && pmf_cfg <= 2)) {
                goto _ERROUT;
            }
            break;
        case '2':
            wpa2_only = 1;
            break;
        case '3':
            wpa3_only = 1;
            break;
        case 'a':
            akm = getopt_env.optarg;
            fhost_printf("akm: %s\r\n", akm);
            break;
        case 'D':
            use_dhcp = 0;
            break;
        case 'q':
            ++quick_connect;
            break;
        case 'w':
            wep = 1;
            break;
        case 'c':
            channel_index = atoi(getopt_env.optarg);
            fhost_printf("channel_index: %d\r\n", channel_index);
            break;
        case 'd':
             duration = atoi(getopt_env.optarg);
             break;
        case 'p':
             probe_cnt = atoi(getopt_env.optarg);
             break;
        case 'u':
             auth_timeout = atoi(getopt_env.optarg);
             break;
        case 'e':
             eapol_1_timeout = atoi(getopt_env.optarg);
             break;
        case 't':
             eapol_rem_timeout = atoi(getopt_env.optarg);
             break;
        case '?':
            fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
            goto _ERROUT;
        }
    }

    if (getopt_env.optind == argc) {
        fhost_printf("Expected ssid\r\n");
        goto _ERROUT;
    }

    password = argv[getopt_env.optind + 1];

    // Reject conflicting config
    if (wpa2_only + wpa3_only + wep > 1) {
        goto _ERROUT;
    }
    if (!password && (wpa2_only || wpa3_only || wep)) {
        goto _ERROUT;
    }

    if (wpa2_only) {
        akm = "WPA2";
    }
    if (wpa3_only) {
        akm = "WPA3";
        // PMF required for WPA3 only
        pmf_cfg = 2;
    }
    if (wep) {
        akm = "WEP";
        if (cvt_wep_password(password, password_buf)) {
            fhost_printf("invalid WEP password\r\n");
            goto _ERROUT;
        }
        password = password_buf;
    }

    fhost_printf("connect wifi ssid:%s, psk:%s, bssid:%d, q:%d\r\n", argv[getopt_env.optind], password, bssid_set_flag, quick_connect);
    freq = phy_channel_to_freq(PHY_BAND_2G4, channel_index);
    if (quick_connect) {
        wifi_mgmr_sta_connect_params_t conn_param;
        memset(&conn_param, 0, sizeof(conn_param));
        if(strlcpy((char *)conn_param.ssid, argv[getopt_env.optind], MGMR_SSID_LEN + 1) >= MGMR_SSID_LEN + 1) {
            printf("[OS]: strlcpy truncated \r\n");
        }
        conn_param.ssid_len = strlen((char *)conn_param.ssid);

        if (password) {
            if(strlcpy((char *)conn_param.key, password, MGMR_KEY_LEN + 1) >= MGMR_KEY_LEN + 1) {
                printf("[OS]: strlcpy truncated \r\n");
            }
            conn_param.key_len = strlen((char *)conn_param.key);
        }
        conn_param.freq1 = freq;
        conn_param.freq2 = 0;
        conn_param.use_dhcp = 1;
        conn_param.pmf_cfg = 1;
        conn_param.quick_connect = 1;
        conn_param.duration = duration;
        conn_param.probe_cnt = probe_cnt;
        conn_param.wpa_flags = FHOST_WPA_SSID_PREFER_WPA2_TO_WPA3;
        conn_param.auth_timeout = auth_timeout;
        conn_param.eapol_1_timeout = eapol_1_timeout;
        conn_param.eapol_rem_timeout = eapol_rem_timeout;
        wifi_mgmr_sta_connect(&conn_param);
    } else {
        wifi_sta_scan_connect(argv[getopt_env.optind], password, bssid_set_flag ? bssid : NULL,
                              (char *)akm, pmf_cfg, freq, freq, duration, probe_cnt, use_dhcp);
    }

    return;

_ERROUT:
    fhost_printf("[USAGE]: %s [-b <bssid>] [-2/3] [-f 0/1/2] [-q] [-w] [-D] [-c <ch_idx>] <ssid> [password]\r\n", argv[0]);
    return;
}

void wifi_disconnect_cmd(int argc, char **argv)
{
    wifi_sta_disconnect();
}

void lwip_cmd(int argc, char **argv)
{
void stats_display(void);
    stats_display();
}

#ifdef CFG_FHOST_MONITOR
static void cb_sniffer(struct wifi_frame_info *info, void *arg)
{
    if (info->payload == NULL)
    {
        TRACE_APP(INF, "Unsupported frame: length = %d", info->length)
        fhost_printf("Unsupported frame: length = %dr\r\n", info->length);
    }
    else
    {
        struct mac_hdr *hdr __MAYBE_UNUSED = (struct mac_hdr *)info->payload;
        TRACE_APP(INF, "%pM %pM %pM %fc SN:%d length = %d", TR_MAC(hdr->addr1.array),
                  TR_MAC(hdr->addr2.array), TR_MAC(hdr->addr3.array), hdr->fctl, hdr->seq >> 4,
                  info->length);
        fhost_printf("SN:%d length = %d\r\n", hdr->seq >> 4, info->length);
    }
}

void cmd_wifi_sniffer_on(int argc, char **argv)
{
    wifi_mgmr_sniffer_item_t sniffer_item;
    getopt_env_t getopt_env;
    int opt;

    memset(&sniffer_item, 0 , sizeof(wifi_mgmr_sniffer_item_t));
    utils_al_getopt_init(&getopt_env, 0);

    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "i:t:f:c:C:")) != -1) {
        switch (opt) {
        case 'i':
            sniffer_item.itf = getopt_env.optarg;
            fhost_printf("itf: %s\r\n", sniffer_item.itf);
            break;

        case 't':
            sniffer_item.type = atoi(getopt_env.optarg);
            fhost_printf("type: %d\r\n", sniffer_item.type);
            break;

        case 'f':
            sniffer_item.prim20_freq = atoi(getopt_env.optarg);
            fhost_printf("prim20_freq: %d\r\n", sniffer_item.prim20_freq);
            break;

        case 'c':
            sniffer_item.center1_freq = atoi(getopt_env.optarg);
            fhost_printf("center1_freq: %d\r\n", sniffer_item.center1_freq);
            break;

        case 'C':
            sniffer_item.center2_freq = atoi(getopt_env.optarg);
            fhost_printf("center2_freq: %d\r\n", sniffer_item.center2_freq);
            break;

        case '?':
            fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
            goto _ERROUT;
        }
    }

    sniffer_item.cb = cb_sniffer;
    sniffer_item.cb_arg = NULL;
    wifi_mgmr_sniffer_enable(sniffer_item);
    return;
_ERROUT:
    fhost_printf("[USAGE]: %s -i <itf> -f <prim20_freq> [-t <bw>] [-c <center1_freq>] [-C <center2_freq>]\r\n", argv[0]);
    return;
}

void cmd_wifi_sniffer_off(int argc, char **argv)
{
    wifi_mgmr_sniffer_item_t sniffer_item;
    getopt_env_t getopt_env;
    int opt;

    memset(&sniffer_item, 0 , sizeof(wifi_mgmr_sniffer_item_t));
    utils_al_getopt_init(&getopt_env, 0);

    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "i:")) != -1) {
        switch (opt) {
        case 'i':
            sniffer_item.itf = getopt_env.optarg;
            fhost_printf("itf: %s\r\n", sniffer_item.itf);
            break;

        case '?':
            fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
            goto _ERROUT;
        }
    }

    wifi_mgmr_sniffer_disable(sniffer_item);
    return;
_ERROUT:
    fhost_printf("[USAGE]: %s -i <itf>\r\n", argv[0]);
    return;
}
#endif

void cmd_wifi_state_get(int argc, char **argv)
{
    wifi_mgmr_state_get();
    return;
}

void cmd_wifi_sta_rssi_get(int argc, char **argv)
{
    int rssi = 0;

    wifi_mgmr_sta_rssi_get(&rssi);
    fhost_print(RTOS_TASK_NULL, "rssi: %ddB\r\n", rssi);
    return;
}

void cmd_wifi_sta_channel_get(int argc, char **argv)
{
    int channel = 0;

    wifi_mgmr_sta_channel_get(&channel);
    fhost_printf("STA Channel: %d\r\n", channel);
    return;
}

void cmd_wifi_ht40(int argc, char **argv)
{
    if (argc > 2) {
        fhost_printf("Usage: wifi_ht40 [0|1]\r\n");
        return;
    }

    if (2 == argc)
        fhost_set_ht40_mode_enable(atoi(argv[1]));

    fhost_printf("HT40 request: %d (%s)\r\n", fhost_get_ht40_mode_enable(),
                 fhost_get_ht40_mode_enable() ? "HT40 permitted" : "20 MHz only");
    fhost_printf("  takes effect at the next stack_wifi; the AP and the modem\r\n"
                 "  capability still decide - read `rc` bw_max for the result\r\n");
    return;
}

void cmd_wifi_sta_ssid_passphr_get(int argc, char **argv)
{
    wifi_mgmr_connect_ind_stat_info_t wifi_mgmr_ind_stat;

    wifi_mgmr_sta_connect_ind_stat_get(&wifi_mgmr_ind_stat);
    return;
}

void cmd_wifi_sta_mac_get(int argc, char **argv)
{
    uint8_t mac_addr[6];

    wifi_mgmr_sta_mac_get(mac_addr);
    fhost_printf("STA MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                        mac_addr[0], mac_addr[1],
                        mac_addr[2], mac_addr[3],
                        mac_addr[4], mac_addr[5]);
    return;
}

void cmd_wifi_ap_mac_get(int argc, char **argv)
{
    uint8_t mac_addr[6];

    wifi_mgmr_ap_mac_get(mac_addr);
    fhost_printf("AP MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                        mac_addr[0], mac_addr[1],
                        mac_addr[2], mac_addr[3],
                        mac_addr[4], mac_addr[5]);
    return;
}

void wifi_enable_autoreconnect_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_autoconnect_enable();
}

void wifi_disable_autoreconnect_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_autoconnect_disable();
}

void wifi_sta_ps_on_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_ps_enter();
}

void wifi_sta_ps_off_cmd(int argc, char **argv)
{
    wifi_mgmr_sta_ps_exit();
}

void wifi_ap_sta_list_get_cmd(int argc, char **argv)
{
    int i, j;
    struct wifi_sta_basic_info sta_info;

    memset(&sta_info, 0, sizeof(struct wifi_sta_basic_info));
    fhost_printf("sta list:\r\n");
    fhost_printf("--------------------------------------------\r\n");
    fhost_printf("No.      StaIndex      Mac-Address       Aid\r\n");
    fhost_printf("--------------------------------------------\r\n");
    for(i = 0, j = 0;i < CFG_STA_MAX;i++){
        wifi_mgmr_ap_sta_info_get(&sta_info, i);
        if (!sta_info.is_used) {
            continue;
        }

        fhost_printf(" %u       "
            "   %u        "
            "%02X:%02X:%02X:%02X:%02X:%02X    "
            "%d      "
            "\r\n",
            j++,
            sta_info.sta_idx,
            sta_info.sta_mac[0],
            sta_info.sta_mac[1],
            sta_info.sta_mac[2],
            sta_info.sta_mac[3],
            sta_info.sta_mac[4],
            sta_info.sta_mac[5],
            sta_info.aid
        );
    }
}

void wifi_ap_sta_delete_cmd(int argc, char **argv)
{
    struct wifi_sta_basic_info sta_info;
    uint8_t sta_idx = 0;

    if (2 != argc) {
        fhost_printf("[USAGE]: %s sta_idx\r\n", argv[0]);
        return;
    }

    fhost_printf("Delete Sta Idx.%s \r\n", argv[1]);
    chan_str_to_hex(&sta_idx, argv[1]);
    fhost_printf("sta idx = %d \r\n", sta_idx);

    memset(&sta_info, 0, sizeof(struct wifi_sta_basic_info));
    wifi_mgmr_ap_sta_info_get(&sta_info, sta_idx);
    if (!sta_info.is_used || (sta_info.sta_idx == 0xef)){
        fhost_printf("Idx.%d sta is invalid\r\n", sta_idx);
        return;
    }

    fhost_printf("sta info: Idx = %u,"
        "mac = %02X:%02X:%02X:%02X:%02X:%02X,"
        "aid = %d"
        "\r\n",
        sta_info.sta_idx,
        sta_info.sta_mac[0],
        sta_info.sta_mac[1],
        sta_info.sta_mac[2],
        sta_info.sta_mac[3],
        sta_info.sta_mac[4],
        sta_info.sta_mac[5],
        sta_info.aid
    );
    wifi_mgmr_ap_sta_delete(sta_info.sta_idx);
}

void cmd_wifi_ap_conf_max_sta(int argc, char **argv)
{
    int max_sta_supported;

    if (2 != argc) {
        fhost_printf("Usage: wifi_ap_max_sta [num]\r\n");
        return;
    }

    max_sta_supported = atoi(argv[1]);
    wifi_mgmr_conf_max_sta(max_sta_supported);
}

static uint8_t packet_raw[] = {
    0x48, 0x02,
    0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00
};

void cmd_wifi_raw_send(int argc, char **argv)
{
    static uint32_t seq = 0;
    getopt_env_t getopt_env;
    int opt;
    int channel = DEFAULT_CHAN_FOR_RAW_PKT;
    wifi_mgmr_raw_send_params_t config;

    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c':
            channel = atoi(getopt_env.optarg);
            fhost_printf("set chan: %d\r\n", channel);
            break;

        case '?':
            fhost_printf("unknow option: %c\r\n", getopt_env.optopt);
            goto _ERROUT;
        }
    }

    memset(&config, 0, sizeof(config));
    config.channel = channel;
    config.pkt = packet_raw;;
    config.len = sizeof(packet_raw);

    for (int i = 0; i < 5; i++) {
        packet_raw[sizeof(packet_raw) - 2] = ((seq << 4) & 0xFF);
        packet_raw[sizeof(packet_raw) - 1] = (((seq << 4) & 0xFF00) >> 8);
        seq++;

        if (wifi_mgmr_raw_80211_send(&config)) {
            fhost_printf("Raw send failed\r\n");
        } else {
            fhost_printf("Raw send succeed\r\n");
        }

        //delay 200ms
        rtos_task_suspend(200);
    }
    return;

_ERROUT:
    fhost_printf("[USAGE]: %s -c <channel num>\r\n", argv[0]);
}

#ifdef CONFIG_CLI_WIFI_DUBUG
static void cmd_wifi(int argc, char **argv)
{
void mm_sec_keydump(void);
    mm_sec_keydump();
}
#endif

#define LINE_MAX_SZ 150
/// Rate Control configuration
struct fhost_rc_conf {
    /// String describing the RC configuration
    char line[LINE_MAX_SZ + 1];
    /// Index of the RC configuration
    int r_idx;
};

/**
 ****************************************************************************************
 * @brief Compare indexes of rate control configurations
 *
 * @param[in] a         First rate control configuration
 * @param[in] b         Second rate control configuration
 * @return 1 if the index of a is higher than the index of b
 ****************************************************************************************
 */
static __attribute__((unused)) int fhost_rc_compare_idx(const void *a, const void *b)
{
    struct fhost_rc_conf *idx_a = *(struct fhost_rc_conf **) a;
    struct fhost_rc_conf *idx_b = *(struct fhost_rc_conf **) b;

    if (idx_a->r_idx < idx_b->r_idx)
        return -1;
    else
        return 1;
}

/**
 ****************************************************************************************
 * @brief Convert RC table index to Rate configuration
 *
 * @param[in] idx      Index of the RC table
 * @param[in] is_rx    Indicates whether the conversion is for RX stats
 * @param[out] r_cfg   Rate configuration to fill
 * @param[out] ru_size RU size for the HE TB
 *
 * @return 0 if successful, != 0 otherwise
 ****************************************************************************************
 */
static int fhost_idx_to_rate_cfg(int idx, bool is_rx, union fhost_rc_rate_ctrl_info *r_cfg,
                                 int *ru_size)
{
    union fhost_rc_mcs_index {
        struct {
            uint32_t mcs : 3;
            uint32_t nss : 2;
        } ht;
        struct {
            uint32_t mcs : 4;
            uint32_t nss : 3;
        } vht;
        struct {
            uint32_t mcs : 4;
            uint32_t nss : 3;
        } he;
    };
    uint16_t first_ht, first_vht, first_he_su , first_he_er __MAYBE_UNUSED, first_he_mu __MAYBE_UNUSED;
    uint8_t he_mu_rates_per_mcs __MAYBE_UNUSED, he_su_rates_per_mcs __MAYBE_UNUSED,
            he_er_rates_per_mcs __MAYBE_UNUSED, vht_rates_per_mcs, ht_rates_per_mcs;
    uint8_t max_vht_mcs, max_he_mcs __MAYBE_UNUSED;

    #if NX_FHOST_RX_STATS
    if (is_rx)
    {
        first_ht =  fhost_conf_rx.first_ht;
        first_vht = fhost_conf_rx.first_vht;
        #if NX_HE
        first_he_su = fhost_conf_rx.first_he_su;
        first_he_er = fhost_conf_rx.first_he_er;
        #else
        first_he_su = fhost_conf_rx.n_rates;
        #endif
        first_he_mu = fhost_conf_rx.first_he_mu;
        ht_rates_per_mcs = fhost_conf_rx.ht_rates_per_mcs;
        vht_rates_per_mcs = fhost_conf_rx.vht_rates_per_mcs;
        he_su_rates_per_mcs = fhost_conf_rx.he_su_rates_per_mcs;
        he_er_rates_per_mcs = fhost_conf_rx.he_er_rates_per_mcs;
        he_mu_rates_per_mcs = fhost_conf_rx.he_mu_rates_per_mcs;
        max_vht_mcs = fhost_conf_rx.max_vht_mcs;
        max_he_mcs = fhost_conf_rx.max_he_mcs;
    }
    else
    #endif
    {
        first_ht = FIRST_HT;
        first_vht = FIRST_VHT;
        first_he_su = FIRST_HE_SU;
        first_he_er = FIRST_HE_ER;
        first_he_mu = FIRST_HE_MU;
        he_er_rates_per_mcs = 3;
        he_su_rates_per_mcs = 4 * 3;
        vht_rates_per_mcs = 4 * 2;
        ht_rates_per_mcs = 2 * 2;
        he_mu_rates_per_mcs = 6 * 3;
        max_vht_mcs = 10;
        max_he_mcs = 12;
    }

    r_cfg->value = 0;
    if (idx < FIRST_OFDM)
    {
        r_cfg->format_mod_tx = FORMATMOD_NON_HT;
        r_cfg->gi_and_pre_type_tx = (idx & 1) << 1;
        r_cfg->mcs_idx = idx / 2;
    }
    else if (idx < first_ht)
    {

        r_cfg->format_mod_tx = FORMATMOD_NON_HT;
        r_cfg->mcs_idx =  idx - N_CCK + 4;
    }
    else if (idx < first_vht)
    {
        union fhost_rc_mcs_index *r = (union fhost_rc_mcs_index *)r_cfg;

        idx -= first_ht;
        r_cfg->format_mod_tx = FORMATMOD_HT_MF;
        r->ht.nss = idx / (8*ht_rates_per_mcs);
        r->ht.mcs = (idx % (8*ht_rates_per_mcs)) / ht_rates_per_mcs;
        r_cfg->bw_tx = ((idx % (8*ht_rates_per_mcs)) % ht_rates_per_mcs) / 2;
        r_cfg->gi_and_pre_type_tx = idx & 1;
    }
    else if (idx < first_he_su)
    {
        union fhost_rc_mcs_index *r = (union fhost_rc_mcs_index *)r_cfg;

        idx -= first_vht;
        r_cfg->format_mod_tx = FORMATMOD_VHT;
        r->vht.nss = idx / (max_vht_mcs*vht_rates_per_mcs);
        r->vht.mcs = (idx % (max_vht_mcs*vht_rates_per_mcs)) / vht_rates_per_mcs;
        r_cfg->bw_tx = ((idx % (max_vht_mcs*vht_rates_per_mcs)) % vht_rates_per_mcs) / 2;
        r_cfg->gi_and_pre_type_tx = idx & 1;
    }
    #if NX_HE
    else if (idx < first_he_mu)
    {
        union fhost_rc_mcs_index *r = (union fhost_rc_mcs_index *)r_cfg;

        idx -= first_he_su;
        r_cfg->format_mod_tx = FORMATMOD_HE_SU;
        // Jochen 2023/3/6: use msb of nss[2:0] to indicate dcm
        // r->vht.nss = idx / (max_he_mcs*he_su_rates_per_mcs);
        r_cfg->dcm_tx = idx / (4*max_he_mcs*he_su_rates_per_mcs);
        r->vht.nss = (idx % (4*max_he_mcs*he_su_rates_per_mcs)) / (max_he_mcs*he_su_rates_per_mcs);
        r->vht.mcs = (idx % (max_he_mcs*he_su_rates_per_mcs)) / he_su_rates_per_mcs;
        r_cfg->bw_tx = ((idx % (max_he_mcs*he_su_rates_per_mcs)) % he_su_rates_per_mcs) / 3;
        r_cfg->gi_and_pre_type_tx = idx % 3;
    }
    else if (idx < first_he_er)
    {
        union fhost_rc_mcs_index *r = (union fhost_rc_mcs_index *)r_cfg;

        if (ru_size == NULL)
            return -1;

        idx -= first_he_mu;
        r_cfg->format_mod_tx = FORMATMOD_HE_MU;
        r->vht.nss = idx / (max_he_mcs*he_mu_rates_per_mcs);
        r->vht.mcs = (idx % (max_he_mcs*he_mu_rates_per_mcs)) / he_mu_rates_per_mcs;
        *ru_size = ((idx % (max_he_mcs*he_mu_rates_per_mcs)) % he_mu_rates_per_mcs)/ 3;
        r_cfg->gi_and_pre_type_tx = idx % 3;
        r_cfg->bw_tx = 0;
    }
    else
    {
        union fhost_rc_mcs_index *r = (union fhost_rc_mcs_index *)r_cfg;

        idx -= first_he_er;
        r_cfg->format_mod_tx = FORMATMOD_HE_ER;
        // Jochen 2023/3/6: add dcm tx option
        r_cfg->dcm_tx = idx / (2 * 3 * he_er_rates_per_mcs);
        r_cfg->bw_tx = (idx % (2 * 3 *he_er_rates_per_mcs)) / (3 * he_er_rates_per_mcs);
        if (ru_size != NULL)
            *ru_size = idx / (3 * he_er_rates_per_mcs);
        r_cfg->gi_and_pre_type_tx = idx % 3;
        r->vht.mcs = (idx % (3 * he_er_rates_per_mcs)) / he_er_rates_per_mcs;
        r->vht.nss = 0;
    }
    #endif // NX_HE
    return 0;
}

/**
 ****************************************************************************************
 * @brief Write one rate configuration on a string buffer passed as parameter
 *
 * @param[in] buf       String buffer where to write the Rate Control result
 * @param[in] size      Size of the string buffer
 * @param[in] format    Format of the rate configuration
 * @param[in] nss       Number of spatial streams of the rate configuration
 * @param[in] mcs       MCS index of the rate configuration
 * @param[in] bw        Bandwidth of the rate configuration
 * @param[in] sgi       Short Guard interval setting of the rate configuration
 * @param[in] pre       Preamble type of the rate configuration
 * @param[in] is_rx     Indicates whether we are printing RX stats
 * @param[out] r_idx    Index of the rate configuration
 * @return Length of the written string
 ****************************************************************************************
 */
static int fhost_print_rate(char *buf, int size, int format, int nss, int mcs, int bw,
                            int sgi, int pre, int dcm, bool is_rx, int *r_idx)
{
    static const int ru_size_he_er[] = { 242, 106 };
    static const int ru_size_he_mu[] = { 26, 52, 106, 242, 484, 996 };

    int res = 0;
    int bitrates_cck[4] = { 10, 20, 55, 110 };
    int bitrates_ofdm[8] = { 6, 9, 12, 18, 24, 36, 48, 54 };
    char he_gi[3][4] = { "0.8", "1.6", "3.2" };
    uint16_t first_ht, first_vht, first_he_su, first_he_er, first_he_mu;
    uint8_t max_vht_mcs, max_he_mcs, ht_rates_per_mcs, vht_rates_per_mcs;
    uint8_t he_su_rates_per_mcs, he_er_rates_per_mcs, he_mu_rates_per_mcs;

    #if NX_FHOST_RX_STATS
    if (is_rx)
    {
        first_ht = fhost_conf_rx.first_ht;
        first_vht = fhost_conf_rx.first_vht;
        first_he_su = fhost_conf_rx.first_he_su;
        first_he_mu = fhost_conf_rx.first_he_mu;
        first_he_er = fhost_conf_rx.first_he_er;

        max_vht_mcs = fhost_conf_rx.max_vht_mcs;
        max_he_mcs = fhost_conf_rx.max_he_mcs;
        ht_rates_per_mcs = fhost_conf_rx.ht_rates_per_mcs;
        vht_rates_per_mcs = fhost_conf_rx.vht_rates_per_mcs;
        he_su_rates_per_mcs = fhost_conf_rx.he_su_rates_per_mcs;
        he_mu_rates_per_mcs = fhost_conf_rx.he_mu_rates_per_mcs;
        he_er_rates_per_mcs = fhost_conf_rx.he_er_rates_per_mcs;
    }
    else
    #endif
    {
        first_ht = FIRST_HT;
        first_vht = FIRST_VHT;
        first_he_su = FIRST_HE_SU;
        first_he_mu = FIRST_HE_MU;
        first_he_er = FIRST_HE_ER;
        he_su_rates_per_mcs = 4 * 3;
        vht_rates_per_mcs = 4 * 2;
        ht_rates_per_mcs  = 2 * 2;
        he_mu_rates_per_mcs = 6 * 3;
        he_er_rates_per_mcs = 3;
        max_vht_mcs = 10;
        max_he_mcs = 12;
    }

    if (format < FORMATMOD_HT_MF)
    {
        if (mcs < 4)
        {
            if (r_idx)
            {
                *r_idx = (mcs * 2) + pre;
                res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
            }
            res += dbg_snprintf(&buf[res], size - res, "L-CCK/%cP%11c%2u.%1uM   ",
                                pre > 0 ? 'L' : 'S', ' ', bitrates_cck[mcs] / 10,
                                bitrates_cck[mcs] % 10);
        }
        else
        {
            mcs -= 4;
            if (r_idx)
            {
                *r_idx = FIRST_OFDM + mcs;
                res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
            }
            res += dbg_snprintf(&buf[res], size - res, "L-OFDM%13c%2u.0M   ",
                                ' ', bitrates_ofdm[mcs]);
        }
    }
    else if (format < FORMATMOD_VHT)
    {
        if (r_idx)
        {
            *r_idx = first_ht + nss * 8 * ht_rates_per_mcs + mcs * ht_rates_per_mcs +
                      bw * 2 + sgi;
            res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
        }
        mcs += nss * 8;
        res += dbg_snprintf(&buf[res], size - res, "HT%d/%cGI%11cMCS%-2d   ",
                            20 * (1 << bw), sgi ? 'S' : 'L', ' ', mcs);
    }
    else if (format == FORMATMOD_VHT)
    {
        if (r_idx)
        {
            *r_idx = first_vht + nss * max_vht_mcs * vht_rates_per_mcs +
                     mcs * vht_rates_per_mcs + bw * 2 + sgi;
            res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
        }
        res += dbg_snprintf(&buf[res], size - res, "VHT%d/%cGI%*cMCS%d/%1d  ",
                            20 * (1 << bw), sgi ? 'S' : 'L', bw > 2 ? 9 : 10, ' ', mcs,
                            nss + 1);
    }
    else if (format == FORMATMOD_HE_SU)
    {
        if (r_idx)
        {
            *r_idx = first_he_su + nss * max_he_mcs * he_su_rates_per_mcs +
                     mcs * he_su_rates_per_mcs+ bw * 3 + sgi;
            res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
        }
        res += dbg_snprintf(&buf[res], size - res, "HE%d/GI%s%4s%*cMCS%d/%1d%*c",
                            20 * (1 << bw), he_gi[sgi], dcm ? "/DCM" : "",
                            bw > 2 ? 4 : 5, ' ', mcs, nss + 1, mcs > 9 ? 1 : 2, ' ');
    }
    else if (format == FORMATMOD_HE_MU)
    {
        if (r_idx)
        {
            *r_idx = first_he_mu + nss * max_he_mcs * he_mu_rates_per_mcs +
                     mcs * he_mu_rates_per_mcs + bw * 3 + sgi;
            res = dbg_snprintf(buf, size - res, "%4d ", *r_idx);
        }
        res += dbg_snprintf(&buf[res], size - res, "HEMU-%d/GI%s%*cMCS%d/%1d%*c",
                            ru_size_he_mu[bw], he_gi[sgi], bw > 1 ? 5 : 6, ' ', mcs, nss + 1,
                            mcs > 9 ? 1 : 2, ' ');
    }
    else // HE ER
    {
        if (r_idx)
        {
            *r_idx = first_he_er + bw * 3 * he_er_rates_per_mcs +
                     mcs * he_er_rates_per_mcs + sgi;
            res = dbg_snprintf(buf, size - res, "%3d ", *r_idx);
        }
        res += dbg_snprintf(&buf[res], size - res, "HEER-%d/GI%s%4s%1cMCS%d/NSS%1d%2c",
                            ru_size_he_er[bw], he_gi[sgi], dcm ? "/DCM" : "",
                            ' ', mcs, nss + 1, ' ');
    }

    return res;
}

/**
 ****************************************************************************************
 * @brief Extract parameters from rate configuration and print it on a buffer
 *
 * @param[in] buf           String buffer where to write the Rate Control result
 * @param[in] size          Size of the string buffer
 * @param[in] rate_config   Rate configuration of the sample
 * @param[in] ru_size       RU size for the HE TB
 * @param[in] is_rx         Indicates whether we are printing RX stats
 * @param[out] r_idx        Index of the rate configuration
 * @return Length of the written string
 ****************************************************************************************
 */
static int fhost_print_rate_from_cfg(char *buf, int size, uint32_t rate_config,
                                     uint8_t ru_size, bool is_rx, int *r_idx)
{
    uint8_t ft, pre, gi, bw, nss, mcs, dcm;

    ft = export_rc_get_format_mod(rate_config);
    mcs = export_rc_get_mcs_index(rate_config);
    nss = export_rc_get_nss(rate_config);
    bw = export_rc_get_bw(rate_config);
    pre = export_rc_get_pre_type(rate_config);
    gi = export_rc_get_sgi(rate_config) | (pre << 1);
    dcm = 0;
    if (ft >= FORMATMOD_HE_SU)
    {
        dcm = (rate_config & RC_DCM_MOD_MASK) >> RC_DCM_MOD_OFT;
        if ((ft == FORMATMOD_HE_MU) || (ft == FORMATMOD_HE_ER))
            bw = ru_size;
    }
    return fhost_print_rate(buf, size, ft, nss, mcs, bw, gi, pre, dcm, is_rx, r_idx);
}

#if NX_FHOST_RX_STATS & 0
// move to mac
/**
 ****************************************************************************************
 * @brief Print RX statistics about a sta passed as parameter
 *
 * @param[in] sta_idx       Index of the STA
 *
 * return 0 if successful, != 0 otherwise
 ****************************************************************************************
 */
static int fhost_ipc_print_rx_stats(int sta_idx)
{
    char hist[] = "##################################################";
    int hist_len = sizeof(hist) - 1, len = 0;
    int8_t rssi[2];
    uint8_t mac_addr[MAC_ADDR_LEN];
    struct rx_vector_1 *last_rx;
    unsigned int mcs, gi, nss, pre, bw, dcm = 0;
    uint32_t i;
    char buf[LINE_MAX_SZ];
    // Extract Mac address
    MAC_ADDR_EXTRACT(mac_addr, sta_mgmt_get_peer_addr(sta_idx));
    fhost_print(RTOS_TASK_NULL, "\nRX rate info for %02x:%02x:%02x:%02x:%02x:%02x:\n",
                mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
                mac_addr[5]);

    if (!rx_stats[sta_idx])
    {
        fhost_print(RTOS_TASK_NULL, "\nThis station has not been initialized\n");
        return -1;
    }

    // Display Statistics
    for (i = 0 ; i < fhost_conf_rx.n_rates ; i++ )
    {
        if (rx_stats[sta_idx]->table[i]) {
            union fhost_rc_rate_ctrl_info r_cfg;
            int percent = (((uint64_t)rx_stats[sta_idx]->table[i]) * 1000) / rx_stats[sta_idx]->cpt;
            int p;
            int ru_size = 0;

            if(fhost_idx_to_rate_cfg(i, true, &r_cfg, &ru_size))
                return -1;

            fhost_print_rate_from_cfg(buf, LINE_MAX_SZ, r_cfg.value, ru_size, true, NULL);
            p = (percent * hist_len) / 1000;
            fhost_print(RTOS_TASK_NULL,"%s: %9d(%2d.%1d%%)%.*s\n",
                        buf, rx_stats[sta_idx]->table[i], percent / 10, percent % 10, p, hist);
        }
    }

    // Display detailed info of the last received rate
    last_rx = &rx_stats[sta_idx]->last_rx;

    fhost_print(RTOS_TASK_NULL,
                "\nLast received rate\n"
                "  type         rate    LDPC STBC BEAMFM DCM DOPPLER %s\n",
                (phy_get_nrx() > 0) ? "rssi1(dBm) rssi2(dBm)" : "rssi(dBm)");

    bw = last_rx->ch_bw;
    pre = last_rx->pre_type;
    #if NX_MAC_VER >= 20
    if (last_rx->format_mod >= FORMATMOD_HE_SU) {
        mcs = last_rx->he.mcs;
        nss = last_rx->he.nss;
        gi = last_rx->he.gi_type;
        if ((last_rx->format_mod == FORMATMOD_HE_MU) ||
            (last_rx->format_mod == FORMATMOD_HE_ER))
            bw = last_rx->he.ru_size;
        dcm = last_rx->he.dcm;
    } else
    #endif
    if (last_rx->format_mod == FORMATMOD_VHT) {
        #if NX_MAC_VER >= 20
        mcs = last_rx->vht.mcs;
        nss = last_rx->vht.nss;
        gi = last_rx->vht.short_gi;
        #else
        mcs = last_rx->mcs;
        nss = last_rx->stbc ? last_rx->n_sts/2 : last_rx->n_sts;
        gi = last_rx->short_gi;
        #endif
    } else if (last_rx->format_mod >= FORMATMOD_HT_MF) {
        #if NX_MAC_VER >= 20
        mcs = last_rx->ht.mcs % 8;
        nss = last_rx->ht.mcs / 8;;
        gi = last_rx->ht.short_gi;
        #else
        mcs = last_rx->mcs % 8;
        nss = last_rx->mcs / 8;;
        gi = last_rx->short_gi;
        #endif
    } else {
        mcs = rxv2macrate[last_rx->leg_rate];
        nss = 0;
        gi = 0;
    }

    len = fhost_print_rate(buf, LINE_MAX_SZ - len, last_rx->format_mod, nss, mcs, bw, gi, pre, dcm, true, NULL);

    // flags for HT/VHT/HE
    #if NX_MAC_VER >= 20
    if (last_rx->format_mod >= FORMATMOD_HE_SU) {
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "  %c    %c     %c    %c     %c",
                            last_rx->he.fec ? 'L' : ' ',
                            last_rx->he.stbc ? 'S' : ' ',
                            last_rx->he.beamformed ? 'B' : ' ',
                            last_rx->he.dcm ? 'D' : ' ',
                            last_rx->he.doppler ? 'D' : ' ');
    } else
    #endif
    if (last_rx->format_mod == FORMATMOD_VHT) {
        #if NX_MAC_VER >= 20
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "  %c    %c     %c           ",
                            last_rx->vht.fec ? 'L' : ' ',
                            last_rx->vht.stbc ? 'S' : ' ',
                            last_rx->vht.beamformed ? 'B' : ' ');
        #else
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "  %c    %c     %c           ",
                            last_rx->fec_coding ? 'L' : ' ',
                            last_rx->stbc ? 'S' : ' ',
                            !last_rx->smoothing ? 'B' : ' ');
        #endif
    } else if (last_rx->format_mod >= FORMATMOD_HT_MF) {
        #if NX_MAC_VER >= 20
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "  %c    %c                  ",
                            last_rx->ht.fec ? 'L' : ' ',
                            last_rx->ht.stbc ? 'S' : ' ');
        #else
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "  %c    %c                  ",
                            last_rx->fec_coding ? 'L' : ' ',
                            last_rx->stbc ? 'S' : ' ');
        #endif
    } else {
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "                         ");
    }

    hal_desc_get_rssi(last_rx, rssi);
    if (phy_get_nrx() > 0) {
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "       %-4d       %d\n",
                            rssi[0], rssi[1]);
    } else {
        len += dbg_snprintf(&buf[len], LINE_MAX_SZ - len, "      %d\n", rssi[0]);
    }

    fhost_print(RTOS_TASK_NULL, "%s\n", buf);
    return 0;
}
#endif // NX_FHOST_RX_STATS

/**
 ****************************************************************************************
 * @brief Print Rate Control statistics for a station passed as parameter
 *
 * @param[in] sta_idx    Station index
 * @return 0 on success and !=0 if error occurred
 ****************************************************************************************
 */
static int fhost_rc_print_stats_sta(int sta_idx, bool is_rx)
{
    static const char *const rc_mode_str[] = { "active", "fixed-req", "fixed" };
    struct macsw_rc_snapshot snap;
    char line[LINE_MAX_SZ];
    int i;

    if (is_rx)
    {
        #if NX_FHOST_RX_STATS
        if (fhost_ipc_print_rx_stats(sta_idx))
            return FHOST_IPC_ERROR;
        return FHOST_IPC_SUCCESS;
        #else
        fhost_print(RTOS_TASK_NULL, "RX statistics not enabled in FW\n");
        return FHOST_IPC_ERROR;
        #endif
    }

    /* This used to send CFGRWNX_RC_CMD and decode me_rc_stats_cfm. That
     * message was removed from macsw in v6.41.2, so the whole body sat behind
     * an #if 0 and `rc` printed nothing at all -- not even "No results". Read
     * the RC tables directly instead; see macsw_rc_snapshot_get(). */
    if (macsw_rc_snapshot_get(sta_idx, &snap))
    {
        fhost_print(RTOS_TASK_NULL, "sta %d: no rate control state\n", sta_idx);
        return FHOST_IPC_SUCCESS;
    }

    fhost_print(RTOS_TASK_NULL, "\nTX rate info for %02X:%02X:%02X:%02X:%02X:%02X (sta %d)\n",
                snap.peer_addr[0], snap.peer_addr[1], snap.peer_addr[2],
                snap.peer_addr[3], snap.peer_addr[4], snap.peer_addr[5], sta_idx);

    /* The policy table is the ground truth: this is the rate the MAC HW is
     * transmitting with right now, whatever the RC believes. */
    fhost_print(RTOS_TASK_NULL, "in use now (HW policy table):\n");
    for (i = 0; i < MACSW_RC_STEPS; i++)
    {
        uint32_t w = snap.hw_step[i];

        fhost_print_rate_from_cfg(line, LINE_MAX_SZ, w, 0, false, NULL);
        fhost_print(RTOS_TASK_NULL, "  step%d %08lx %s retries %lu\n", i,
                    (unsigned long)w, line, (unsigned long)((w >> 29) & 0x7));
    }

    fhost_print(RTOS_TASK_NULL,
                "\n #  type           rate            eprob    ok(   tot)  skipped\n");
    for (i = 0; i < snap.no_samples; i++)
    {
        unsigned int eprob = ((snap.sample[i].probability * 1000) >> 16) + 1;
        int len = 0;

        /* T/t/P mark the retry chain steps, * the step A-MPDU TX is using. */
        len += dbg_snprintf(&line[len], LINE_MAX_SZ - len, "%2d ", i);
        len += fhost_print_rate_from_cfg(&line[len], LINE_MAX_SZ - len,
                                         snap.sample[i].rate_config, 0, false, NULL);
        len += dbg_snprintf(&line[len], LINE_MAX_SZ - len, "%c",
                            (snap.sw_retry_step &&
                             snap.retry_step_idx[snap.sw_retry_step] == i) ? '*' : ' ');
        len += dbg_snprintf(&line[len], LINE_MAX_SZ - len, "%c%c%c ",
                            snap.retry_step_idx[0] == i ? 'T' : ' ',
                            snap.retry_step_idx[1] == i ? 't' : ' ',
                            snap.retry_step_idx[2] == i ? 'P' : ' ');
        len += dbg_snprintf(&line[len], LINE_MAX_SZ - len, " %4u.%1u %5u(%6u) %6u",
                            eprob / 10, eprob % 10, snap.sample[i].success,
                            snap.sample[i].attempts, snap.sample[i].sample_skipped);
        fhost_print(RTOS_TASK_NULL, "%s\n", line);
    }

    fhost_print(RTOS_TASK_NULL, "\nmode %s", (snap.mode < 3) ? rc_mode_str[snap.mode] : "?");
    if (snap.mode != 0)
    {
        fhost_print_rate_from_cfg(line, LINE_MAX_SZ, snap.fixed_rate_config, 0, false, NULL);
        fhost_print(RTOS_TASK_NULL, " (%s)", line);
    }
    fhost_print(RTOS_TASK_NULL, "  format_mod %u  bw_max %u  amsdu %u\n",
                snap.format_mod, snap.bw_max, snap.curr_amsdu_len);
    fhost_print(RTOS_TASK_NULL, " MPDUs %u  AMPDUs %u  AvLen %d.%d  trialP %u\n",
                snap.ampdu_len, snap.ampdu_packets, (int)(snap.avg_ampdu_len >> 16),
                (int)(((snap.avg_ampdu_len * 10) >> 16) % 10), snap.trial_sample_wait);

    return FHOST_IPC_SUCCESS;
}

/**
 ****************************************************************************************
 * @brief Print Rate Control statistics for every station connected to
 * a VIF passed as parameter
 *
 * @param[in] fvif_idx      Index of virtual interface in FHOST
 * @return 0 on success and !=0 if error occurred
 ****************************************************************************************
 */
static int fhost_rc_print_stats(int fvif_idx, bool is_rx)
{
    uint8_t i, idx_table_size;
    uint8_t idx_table[CFG_STA_MAX];
    struct fhost_vif_tag *fhost_vif;

    fhost_vif = &fhost_env.vif[fvif_idx];

    // Get VIF status
    idx_table_size = fhost_get_sta_idx(fhost_vif, CFG_STA_MAX, idx_table);

    for (i = 0; i < idx_table_size; i++)
    {
        if (fhost_rc_print_stats_sta(idx_table[i], is_rx) == FHOST_IPC_ERROR)
            return FHOST_IPC_ERROR;
    }
    if (!idx_table_size)
        fhost_print(RTOS_TASK_NULL, "No results\n");

    return FHOST_IPC_SUCCESS;
}

/**
 ****************************************************************************************
 * @brief Set TX Rate Control for a certain station
 *
 * @param[in] sta               Pointer to the STA entry
 * @param[in] fixed_rate_idx    Rate control index to use
 * @return FHOST_IPC_SUCCESS if successful, FHOST_IPC_ERROR otherwise
 ****************************************************************************************
 */
static int fhost_rc_set_rate_sta(void *sta, int fixed_rate_idx, uint8_t gi)
{
    struct cfgrwnx_rc_set_rate cmd;
    struct cfgrwnx_resp resp;
    union fhost_rc_rate_ctrl_info rate_config;
    char buf[LINE_MAX_SZ];

    // Convert rate index into rate configuration
    if ((fixed_rate_idx < 0) || (fixed_rate_idx >= TOT_RATES))
    {
        // disable fixed rate
        rate_config.value = (uint32_t)-1;
    }
    else if (fhost_idx_to_rate_cfg(fixed_rate_idx, false, &rate_config, NULL))
        return FHOST_IPC_ERROR;

    if (gi != 0xff) {
         rate_config.gi_and_pre_type_tx = gi;
    }
    fhost_printf("ridx %d => rate (format %d,dcm %d,bw %d,mcs %d,gi %d)\n",
        fixed_rate_idx, rate_config.format_mod_tx,
        rate_config.dcm_tx, rate_config.bw_tx, rate_config.mcs_idx, rate_config.gi_and_pre_type_tx
        );

    fhost_print_rate_from_cfg(buf, LINE_MAX_SZ, rate_config.value,
                              0, false, NULL);

    fhost_print(RTOS_TASK_NULL, "Fix rate to %s\n", buf);

    // prepare CFGRWNX_RC_SET_RATE_CMD to send
    cmd.hdr.len = sizeof(cmd);
    cmd.hdr.id = CFGRWNX_RC_SET_RATE_CMD;
    cmd.hdr.resp_queue = cntrl_link->queue;
    cmd.sta_idx = sta_get_staid(sta);
    cmd.fixed_rate_cfg = (uint16_t) rate_config.value;

    resp.hdr.len = sizeof(resp);
    resp.hdr.id = CFGRWNX_RC_SET_RATE_RESP;

    // Send CFGRWNX_RC_SET_RATE_CMD
    if (fhost_cntrl_cfgrwnx_cmd_send(&cmd.hdr, &resp.hdr) ||
        (resp.status != CFGRWNX_SUCCESS))
    {
        return FHOST_IPC_ERROR;
    }
    return FHOST_IPC_SUCCESS;
}

/**
 ****************************************************************************************
 * @brief Set TX Rate Control for all the stations associated to a VIF
 *
 * @param[in] vif_idx           Index of the virtual interface
 * @param[in] fixed_rate_idx    Rate control index to use
 * @return FHOST_IPC_SUCCESS if successful, FHOST_IPC_ERROR otherwise
 ****************************************************************************************
 */

static int fhost_rc_set_rate(int fvif_idx, int fixed_rate_idx, uint8_t gi)
{
    struct sta_info_tag *sta;
    struct fhost_vif_tag *fhost_vif = &fhost_env.vif[fvif_idx];

    sta = (struct sta_info_tag *)co_list_pick(mac_vif_get_sta_list(fhost_vif->mac_vif));

    while (sta != NULL)
    {
        if (fhost_rc_set_rate_sta(sta, fixed_rate_idx, gi) == FHOST_IPC_ERROR)
            return FHOST_IPC_ERROR;

        sta = sta_get_list_next(sta);
    }
    return FHOST_IPC_SUCCESS;
}

/**
 ****************************************************************************************
 * @brief Search one connected station based on the mac address
 *
 * @param[in] addr      MAC Address of the connected STA
 * @return pointer to sta_info_tag on success, NULL if error.
 ****************************************************************************************
 */
static void *fhost_search_connected_sta_by_mac(struct mac_addr *addr)
{
    int i;
    struct fhost_vif_tag *fhost_vif;

    for (i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        void *sta;
        if (fhost_env.vif[i].mac_vif == NULL)
            continue;

        fhost_vif = &fhost_env.vif[i];
        sta = co_list_pick(mac_vif_get_sta_list(fhost_vif->mac_vif));

        while (sta != NULL)
        {
            if (MAC_ADDR_CMP(sta_getp_mac_addr(sta), addr))
                return sta;

            sta = sta_get_list_next(sta);
        }
    }
    return NULL;
}

/**
 ****************************************************************************************
 * @brief Process function for 'rc' command
 *
 * Print the Rate Control Table
 * @verbatim
 rc [-m <mac_addr> | -i <itf>] [-f <idx>] <-g <gi>>
 <gi>: 0 -- 0.8us and 2x(6.4) HE_LTF
 <gi>: 1 -- 1.6us and 2x(6.4) HE_LTF
 <gi>: 2 -- 3.2us and 4x(12.8) HE_LTF
 @endverbatim
 *
 * @param[in] params  Vif interface
 * @return 0 on success and !=0 if error occurred
 ****************************************************************************************
 */
void cmd_rc(int argc, char **argv)
{
    struct sta_info_tag *sta = NULL;
    int fhost_vif_idx = INVALID_VIF_IDX;
    int fixed_rate_idx = -1;
    bool is_print = true;
    bool is_rx = false;
    uint8_t i;
    uint8_t gi = 0xff;
    int opt;
    getopt_env_t getopt_env;
    struct mac_addr mac_addr;

    utils_al_getopt_init(&getopt_env, 0);
    while ((opt = utils_al_getopt(&getopt_env, argc, argv, "i:m:f:g:r")) != -1) {
        switch(opt) {
        case 'i':
            fhost_vif_idx = fhost_search_itf(getopt_env.optarg);
            if (fhost_vif_idx < 0)
                return;
            break;
        case 'm':
            if (fhost_ipc_parse_mac_addr(getopt_env.optarg, &mac_addr)) {
                fhost_print(RTOS_TASK_NULL, "Invalid Mac address");
                return;
            }

            sta = fhost_search_connected_sta_by_mac(&mac_addr);
            if (!sta) {
                fhost_print(RTOS_TASK_NULL, "Mac address not found");
                return;
            }
            break;
        case 'f':
            fixed_rate_idx = atoi(getopt_env.optarg);
            is_print = false;
            break;
        case 'r':
            is_rx = true;
            break;
        case 'g':
            gi = atoi(getopt_env.optarg);
            if (gi < 3) {
                break;
            } else {
                fhost_print(RTOS_TASK_NULL, "error! gi should be less than 3, 0 - 0.8us, 1 - 1.6us, 2 - 3.2us");
                return;
            }
        default:
            fhost_print(RTOS_TASK_NULL, "valid option");
            return;
        }
    }

    // Action is printing
    if (is_print)
    {
        // A sta has been provided (-m option)
        if (sta)
        {
            fhost_rc_print_stats_sta(sta_get_staid(sta), is_rx);
            return;
        }
        // A vif has been provided (-i option)
        else if (fhost_vif_idx != INVALID_VIF_IDX)
        {
            fhost_rc_print_stats(fhost_vif_idx, is_rx);
            return;
        }
        // No vif or sta has been provided (no options)
        else
        {
            // Print rates for every VIF
            for (i = 0; i < NX_VIRT_DEV_MAX; i++)
            {
                if ((fhost_env.vif[i].mac_vif != NULL) &&
                    (fhost_rc_print_stats(i, is_rx) != FHOST_IPC_SUCCESS))
                    return;
            }
            return;
        }
    }
    // Action is setting rate (-f option)
    else
    {
        // A sta has been provided (-m option)
        if (sta)
        {
            fhost_rc_set_rate_sta(sta, fixed_rate_idx, gi);
            return;
        }
        // A vif has been provided (-i option)
        else if (fhost_vif_idx != INVALID_VIF_IDX)
        {
            fhost_rc_set_rate(fhost_vif_idx, fixed_rate_idx, gi);
            return;
        }
        // No vif or sta has been provided (no options)
        else
        {
            for (i = 0; i < NX_VIRT_DEV_MAX; i++)
            {
                if ((fhost_env.vif[i].mac_vif != NULL) &&
                    (fhost_rc_set_rate(i, fixed_rate_idx, gi) == FHOST_IPC_ERROR))
                    return;
            }
            return;
        }
    }

    return;
}

/*
 * Bind a fixed HE-SU rate by driving the rate controller directly.
 *
 * `rc -f <idx>` cannot do this on this build: it echoes a host-side decode of
 * the index and then sends CFGRWNX_RC_SET_RATE_CMD, which becomes
 * ME_RC_SET_RATE_REQ - a message removed from macsw in v6.41.2. Its allocator
 * is a stub returning NULL and no task handles the id, so the command reports
 * success and has no effect. Verified on air: a capture taken under
 * `rc -f 892` (HE MCS9) carried 23 distinct data rates, none above 16.5%.
 *
 * The rate-controller side of fixed rate is present and unused:
 * rc_tx_set_fixed_rate_config() arms RC_MODE_FIXED_RATE_REQUESTED and
 * rc_tx_update_stats_fixed_rate() promotes it to RC_MODE_FIXED_RATE.
 * rc_tx_find_connected_sta() must be used to pick the peer - sta_idx 0 is the
 * BCMC entry in STA mode and has no sta_stats, which faults inside the RC.
 *
 * This writes RC state from the shell task rather than the WiFi task. The
 * fields are word sized and the RC consumes them on its next update, so the
 * race is benign for bench use, but this is a debug instrument, not a
 * production control path.
 */
extern uint16_t rc_tx_find_connected_sta(void);
extern void rc_tx_set_fixed_rate_config(uint16_t sta_idx, uint32_t rate_config,
                                        bool enable);

void cmd_fix_mcs(int argc, char **argv)
{
    uint16_t sta_idx;
    uint32_t rate_config;
    int mcs, gi = 0;

    int fmt = FORMATMOD_HE_SU, bw = 0;

    if (argc < 2) {
        fhost_print(RTOS_TASK_NULL,
                    "usage: fix_mcs {off | <mcs> [gi] [fmt] [bw]}\n"
                    "  gi  0=long/0.8us 1=short/1.6us\n"
                    "  fmt 2=HT-MF 5=HE-SU (default 5)\n"
                    "  bw  0=20MHz 1=40MHz (default 0)\n");
        return;
    }

    sta_idx = rc_tx_find_connected_sta();
    if (sta_idx == 0xFFFF) {
        fhost_print(RTOS_TASK_NULL, "fix_mcs: no connected STA\n");
        return;
    }

    if (!strcmp(argv[1], "off")) {
        rc_tx_set_fixed_rate_config(sta_idx, 0, false);
        fhost_print(RTOS_TASK_NULL, "fix_mcs: off (sta %d)\n", sta_idx);
        return;
    }

    mcs = atoi(argv[1]);
    if (argc > 2)
        gi = atoi(argv[2]);
    if (argc > 3)
        fmt = atoi(argv[3]);
    if (argc > 4)
        bw = atoi(argv[4]);

    if (fmt != FORMATMOD_HT_MF && fmt != FORMATMOD_HE_SU) {
        fhost_print(RTOS_TASK_NULL, "fix_mcs: fmt must be 2 (HT-MF) or 5 (HE-SU)\n");
        return;
    }
    /* One spatial stream: HT tops out at MCS7, HE-SU at MCS9 here. */
    if (mcs < 0 || mcs > (fmt == FORMATMOD_HT_MF ? 7 : 9)) {
        fhost_print(RTOS_TASK_NULL, "fix_mcs: mcs must be 0-%d for this format\n",
                    fmt == FORMATMOD_HT_MF ? 7 : 9);
        return;
    }
    if (bw < 0 || bw > 1) {
        fhost_print(RTOS_TASK_NULL, "fix_mcs: bw must be 0 (20MHz) or 1 (40MHz)\n");
        return;
    }

    /* RC-internal rate_config, enum rc_rate_bf in rc.h:
     *   FORMAT_MOD[3:0] MCS[7:4] NSS[10:8] GI[12:11] LONG_PRE[13] BW[16:14]
     * NSS 0 is one spatial stream. Format and BW are arguments because the
     * comparison this instrument exists for - is 40 MHz mode itself expensive,
     * independent of the PPDU width? - needs the SAME HT rate pinned in both a
     * 20 MHz and a 40 MHz association, and an 11ac-only AP cannot receive the
     * HE-SU rate this used to hardcode. */
    rate_config = (uint32_t)fmt | ((uint32_t)mcs << 4)
                | (((uint32_t)gi & 3u) << 11) | ((uint32_t)bw << 14);

    rc_tx_set_fixed_rate_config(sta_idx, rate_config, true);
    fhost_print(RTOS_TASK_NULL,
                "fix_mcs: sta %d %s %dMHz NSS1 MCS%d gi%d -> cfg 0x%x\n",
                sta_idx, fmt == FORMATMOD_HT_MF ? "HT-MF" : "HE-SU",
                bw ? 40 : 20, mcs, gi, (unsigned int)rate_config);
}

void cmd_rate(int argc, char **argv)
{
    int msg_id;

    if(argc != 2) {
	fhost_printf("rate [number] \r\n");
        return;
    }

    msg_id = atoi(argv[1]);

    fhost_printf("rate is %x:%d\r\n", msg_id, msg_id);
extern uint32_t g_fw_rate;
    g_fw_rate = msg_id;

    return;
}
void cmd_wifi_mode_set(int argc, char **argv)
{
    char ap_or_sta;
    int mode = 0, i;
    struct {
        char* mode_str;
        int mode;
    } mode_map[] = {
        {"b",   WIFI_MODE_802_11B},
        {"bg",  WIFI_MODE_802_11B | WIFI_MODE_802_11G},
        {"bgn", WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4},
        {"bgnax", WIFI_MODE_802_11B | WIFI_MODE_802_11G | WIFI_MODE_802_11N_2_4 | WIFI_MODE_802_11AX_2_4},

    };

    if (3 != argc && 2 != argc) {
        printf("Usage: %s [ap/sta] [mode]\r\n", argv[0]);
        return;
    }

    if (0 == strcmp(argv[1], "sta")) {
        ap_or_sta = 0;
    } else if (0 == strcmp(argv[1], "ap")) {
        ap_or_sta = 1;
    } else {
        printf("Usage: %s [ap/sta] [mode]\r\n", argv[0]);
        return;
    }

    /* get WiFi mode */
    if (2 == argc) {
        mode = wifi_mgmr_get_mode(ap_or_sta);
        for (i = 0; i < 4; i++)
        {
            if (mode_map[i].mode == mode) {
                break;
            }
        }
        if (i < 4) {
            printf("WiFi %s mode %s is being used\r\n", argv[1], mode_map[i].mode_str);
        }

        return;
    }

    /* set WiFi mode */
    for (i = 0; i < 4; i++)
    {
        if (0 == strcmp(mode_map[i].mode_str, argv[2])) {
            break;
        }
    }
    if (i == 4) {
        printf("Wrong WiFi mode %s\r\n", argv[2]);
        return;
    }

    wifi_mgmr_set_mode(ap_or_sta, mode_map[i].mode);
    printf("Setting WiFi %s mode to %s\r\n", argv[1], argv[2]);
}
