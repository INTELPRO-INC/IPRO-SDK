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
 * ATBM6162 STA-mode public API wrappers (Phase 2).
 * Thin shims over the vendor driver-API so the app never includes vendor headers
 * (keeps the unprefixed vendor typedefs contained). See atbm6162.h.
 */
#include "atbm6162.h"
#include "atbm_hal.h"   /* atbm_wifi_on / atbm_wifi_scan_network / atbm_wifi_sta_join_ap / atbmwifi_set_pm */
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include <string.h>

/* The single global driver context; vif resolver is declared in atbm_hal.h. */
extern struct atbmwifi_common g_hw_prv;

/* Bring the interface up in STA mode (chip must already be brought up via
 * atbm6162_bringup()). Starts the WPA event thread + supplicant for the vif. */
int atbm6162_sta_start(void)
{
    void *ndev = atbm_wifi_on(ATBM_WIFI_STA_MODE);
    return (ndev != ATBM_NULL) ? 0 : -1;
}

/* Active scan; fills caller buffer with the vendor-formatted AP list. */
int atbm6162_scan(char *buf, unsigned size)
{
    if (buf == ATBM_NULL || size == 0) {
        return -1;
    }
    return atbm_wifi_scan_network(buf, (atbm_uint32)size);
}

/* Join an OPEN AP (no encryption). Returns 0 on the join request being accepted. */
int atbm6162_connect_open(const char *ssid)
{
    if (ssid == ATBM_NULL) {
        return -1;
    }
    return (int)atbm_wifi_sta_join_ap((char *)ssid, ATBM_NULL,
                                      WLAN_WPA_AUTH_NONE, WLAN_ENCRYPT_NONE, ATBM_NULL);
}

/* Join a WPA2-PSK (AES/CCMP) AP. The driver derives the PMK from `pass` + ssid.
 * authMode WLAN_WPA2_AUTH_PSK -> key_mgmt ATBM_KEY_WPA2. 0 = join request accepted. */
int atbm6162_connect_wpa2(const char *ssid, const char *pass)
{
    if (ssid == ATBM_NULL || pass == ATBM_NULL) {
        return -1;
    }
    return (int)atbm_wifi_sta_join_ap((char *)ssid, ATBM_NULL,
                                      WLAN_WPA2_AUTH_PSK, WLAN_ENCRYPT_AES, (char *)pass);
}

/* 1 = associated to an AP, 0 = not. */
int atbm6162_is_connected(void)
{
    return atbm_wifi_isconnected(0);
}

/* Enable(1)/disable(0) WiFi power-save on the STA vif (if_id 0).
 * PS off -> WSM_PSM_ACTIVE: low, steady latency + reliable, higher current.
 * PS on  -> WSM_PSM_PS: lower current, but AP buffers downlink to DTIM -> jitter.
 * atbmwifi_set_pm pushes to firmware immediately if already associated. 0 = ok. */
int atbm6162_set_powersave(int enable)
{
    struct atbmwifi_vif *priv = _atbmwifi_hwpriv_to_vifpriv(&g_hw_prv, 0);
    if (priv == ATBM_NULL) {
        return -1;
    }
    atbmwifi_set_pm(priv, enable ? 1 : 0, 0);
    return 0;
}

/* Fill `buf` with the current DHCP-assigned IPv4 address ("a.b.c.d"). 0 = ok,
 * <0 if no netif / not up / no address yet. */
int atbm6162_get_ip(char *buf, unsigned size)
{
    const char *s;
    if (buf == ATBM_NULL || size == 0 || netif_default == ATBM_NULL) {
        return -1;
    }
    s = ip4addr_ntoa(netif_ip4_addr(netif_default));
    if (s == ATBM_NULL) {
        return -1;
    }
    strncpy(buf, s, size - 1);
    buf[size - 1] = '\0';
    return 0;
}
