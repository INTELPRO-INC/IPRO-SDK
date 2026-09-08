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
 * Residual link stubs for the ATBM6162 STA (Phase 2) build.
 *
 * Phase 2 enabled the real data path + WPA core, so the netdev/skbuf glue
 * (os/fh_rtt/atbm_os_skbuf.c), lwIP netif bring-up (os/fh_rtt/sdio/atbm_os_api.c)
 * and the wpa_supplicant core (net/wpa/wpa_main.c, wpa_event.c, wpa_common.c)
 * now provide the real symbols — their former stubs were removed here.
 *
 * What remains are symbols with no compiled owner in the STA-only build:
 *   - hostapd / AP-mode entry points: referenced by api/atbm_api.c's AP branch
 *     (iftype == AP/P2P_GO) but NEVER executed in STA mode. net/wpa/hostapd_main.c
 *     is not compiled (no AP support yet), so these stay as no-ops to satisfy the
 *     linker. Compile hostapd_main.c instead when AP mode is added.
 *   - atbm_sdio_check_init_func4: IPRO7 port shim (func4 is COEUS-only).
 * See .claude/plans/atbm6162-integration.md.
 */
#include "atbm_hal.h"

/* ---- hostapd / AP-mode (api/atbm_api.c AP branch; never run in STA) --------- */
atbm_void hostapd_setup_4_way_handshake(struct atbmwifi_vif *priv, atbm_uint8 *da) { (void)priv; (void)da; }
int  hostapd_rx_assoc_req_event(struct atbmwifi_vif *priv, struct atbm_buff *skb) { (void)priv; (void)skb; return 0; }
atbm_void hostap_sta_del(struct atbmwifi_vif *priv, atbm_uint8 *staMacAddr) { (void)priv; (void)staMacAddr; }
atbm_void free_hostapd(struct atbmwifi_vif *priv) { (void)priv; }

/* hostapd_main.c symbols referenced by the compiled WPA core / net but only on the
 * AP/P2P_GO branch (api/atbm_api.c) — never executed in STA OPEN mode. Stubbed
 * instead of compiling net/wpa/hostapd_main.c (which drags in eloop + sta_info AP
 * infrastructure). Compile hostapd_main.c when AP mode is added. */
struct hostapd_data *init_hostapd(struct atbmwifi_vif *priv) { (void)priv; return ATBM_NULL; }
int  hostapd_start(struct atbmwifi_vif *priv) { (void)priv; return 0; }
atbm_void hostapd_run(struct atbmwifi_vif *priv, struct hostapd_sta_info *sta) { (void)priv; (void)sta; }
int  hostapd_eapol_init(struct atbmwifi_vif *priv, struct atbmwifi_wpa_state_machine *sm) { (void)priv; (void)sm; return 0; }
atbm_void hostapd_4_way_handshake_start(struct atbmwifi_vif *priv, struct hostapd_sta_info *sta) { (void)priv; (void)sta; }
atbm_void ap_sta_del(struct hostapd_data *hostapd, struct hostapd_sta_info *sta) { (void)hostapd; (void)sta; }
struct hostapd_sta_info *atbm_ap_get_sta(struct hostapd_data *hapd, const atbm_uint8 *sta) { (void)hapd; (void)sta; return ATBM_NULL; }
/* AP SSID-match check (AP reassoc-req validation) — STA never receives assoc-reqs. */
atbm_uint16 check_ssid(struct atbmwifi_cfg *config, const atbm_uint8 *ssid_ie, atbm_size_t ssid_ie_len) { (void)config; (void)ssid_ie; (void)ssid_ie_len; return 0; }

/* 802.1x/EAPOL receive — OPEN APs have no EAPOL, so never reached (Phase 3/secured). */
atbm_void atbm_ieee802_1x_receive(struct atbmwifi_vif *priv, const atbm_uint8 *sa, const atbm_uint8 *buf, atbm_size_t len) { (void)priv; (void)sa; (void)buf; (void)len; }

/* DHCP client stop (vendor SAL, disconnect/netif-deinit path). No-op for Phase 2;
 * wire netifapi_dhcp_release_and_stop when hardening the disconnect path. */
void dhcpc_stop(char *name) { (void)name; }

/* ---- SDIO function-4 command interface init -------------------------------
 * func4 is a COEUS-only command channel. OCEANUS enumerates only func1, so it
 * has NO func4 — return NULL so atbm_init_chip_priv keeps chip_ver=0 (V0 ops). */
struct atbm_sdio_func *atbm_sdio_check_init_func4(struct atbm_sdio_func *func)
{
    (void)func;
    return ATBM_NULL;
}
