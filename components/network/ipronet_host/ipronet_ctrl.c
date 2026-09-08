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
 * ipronet_ctrl.c - control plane of the ipronet SDIO-WiFi host: HELLO / GET_MAC
 * / ZCEN handshakes, scan / connect / disconnect, the reconnect worker and the
 * device->host event dispatch into the application callbacks.
 *
 * Wire format pinned to the ipronet RNM/BF1B protocol — see ipronet_ctrl.h.
 * Host sends IPRONET_CMD_* as a 0xEF01 CMD PDU; device replies/events arrive
 * via ipronet_sdio.c (ipro6s_rx_handle_pdu) -> ipronet_ctrl_handle, i.e. in
 * the RX task; so do the callbacks.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "compiler/common.h"     /* ATTR_PSRAM_NOINIT_SECTION */
#include "ipronet_priv.h"

static ipronet_callbacks_t s_cb;              /* zeroed = no callbacks */

/* SCAN_RESULTS staging for on_scan_result. The device caps the indication at
 * one 2000-byte SDIO message (rnm handle_scan_results), i.e. 45 x 44-byte
 * records; that is more than the rx_task stack can hold, so it lives in PSRAM
 * like the transport buffers. Only the RX task touches it. */
#define IPRONET_SCAN_MAX_APS  45U
static ATTR_PSRAM_NOINIT_SECTION ipronet_ap_t s_scan_aps[IPRONET_SCAN_MAX_APS];
static uint16_t       s_ipronet_msg_id = 0;
static volatile int   s_link_up        = 0;

/* Cached STA/AP MAC from the GET_MAC handshake. The device STA MAC is random
 * per device boot; ipronet_ctrl_reset_session() clears s_mac_learned on a
 * device power-cycle. Written by the RX path (ipronet_ctrl_handle GET_MAC). */
static volatile int s_mac_learned = 0;
static uint8_t s_sta_mac[6];
static uint8_t s_ap_mac[6];
/* Last device ACK. The RX task's printf is not reliable on this console, so
 * the bench reads the ACK back instead of grepping the log for it. */
static volatile uint16_t s_last_ack_cmd    = 0xFFFFU;
static volatile uint16_t s_last_ack_status = 0xFFFFU;
static ipronet_ip_info_t s_ip;                /* last IP_UPDATE_IND */

void ipronet_ctrl_set_callbacks(const ipronet_callbacks_t *cb)
{
    if (cb) s_cb = *cb; else memset(&s_cb, 0, sizeof(s_cb));
}

/* Auto-reconnect on STA_DISCONNECTED_IND. The device tries to associate exactly
 * once per STA_CONNECT (its own autoconnect stays disabled — the associated-scan
 * crash landmine), so link recovery is the HOST's job. The trigger in the field:
 * the corporate AP hops channels under sustained load (seen live: LAB02 ch13 ->
 * ch2 mid-flood, same BSSID), we keep transmitting into the old channel until
 * BCNLOSS kills the association, and one failed reconnect used to strand the
 * bench until a manual re-bring-up. Credentials are captured by ipronet_connect;
 * a host-issued ipronet_disconnect stays down on purpose. */
static char     s_reconn_ssid[33];
static char     s_reconn_psk[65];
static volatile uint8_t  s_reconn_en        = 1;
static volatile uint8_t  s_reconn_user_down = 1;   /* until first connect */
static SemaphoreHandle_t s_reconn_sem       = NULL;
static volatile uint8_t  s_reconn_task_up   = 0;
static volatile uint32_t s_reconn_attempts  = 0;
static volatile uint32_t s_reconn_restores  = 0;
/* "Restored" means USABLE: IP_UPDATE_IND arrived, not merely association.
 * Seen live: reassociation completes, DHCP never finishes, and a
 * CONNECTED-keyed loop would declare victory over a dead link. */
static volatile uint8_t  s_ip_valid         = 0;

/* Device reboot = new random STA MAC + lost WiFi session. Clear the cached MAC
 * (learned via GET_MAC) and link state so stale values are not reported after a
 * power-cycle. The transport half (aggregation / zero-copy state) is
 * ipronet_sdio_teardown()'s job. */
void ipronet_ctrl_reset_session(void)
{
    s_mac_learned = 0;
    s_link_up     = 0;
    s_ip_valid    = 0;
    /* Power-cycle also killed the SMID session: reconnect attempts could only
     * time out against a dead transport. Stand down until the next explicit
     * ipronet_connect re-arms. */
    s_reconn_user_down = 1;
}

/* Build an ipronet_ctrl_hdr_t + optional body, send as a CMD PDU. */
static int ipronet_ctrl_send(uint16_t cmd, uint16_t flags,
                             const void *body, uint16_t body_len)
{
    uint8_t buf[sizeof(ipronet_ctrl_hdr_t) + 128];   /* connect body = 98 B */
    ipronet_ctrl_hdr_t *h = (ipronet_ctrl_hdr_t *)buf;

    if (body_len > sizeof(buf) - sizeof(*h)) {
        return IPRONET_ERR_PARAM;
    }
    memset(h, 0, sizeof(*h));
    h->cmd    = cmd;
    h->flags  = flags;
    h->msg_id = ++s_ipronet_msg_id;
    if (body && body_len) {
        memcpy(buf + sizeof(*h), body, body_len);
    }
    return ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_CMD, 0, buf,
                                 (uint16_t)(sizeof(*h) + body_len));
}

/* The device enables its OWN autoconnect at boot, and its supplicant then
 * races the reconnect worker below: after a deauth the device starts its own
 * scan+associate, our STA_CONNECT lands mid-association and tears that attempt
 * down ("wpa_supplicant_deinit_iface: send deauth"), after which every
 * association request fails until something reboots. Two recovery owners is
 * one too many, and the host is the one that knows whether an IP arrived. The
 * bench used to type wifi_sta_autoconnect_disable on the device console; a
 * customer has no console, so hand the device the same instruction in band.
 * Wire: RNM STA_SET_AUTO_RECONNECT, header + one byte (== iprctl on the Linux
 * host); the device answers with a plain ACK.
 *
 * MEASURED 2026-09-02 on ipro6_if_wifi d01be61a: the device ACKs this cmd with
 * status 0 but its handler's wifi_mgmr_sta_autoconnect_disable() returns early
 * on the silent fhost_wpa_execute_cmd("STA_AUTOCONNECT 0") path, so the
 * supplicant flag never changes and the device keeps reconnecting on its own.
 * Until that device-side path is fixed, a bench must still type
 * wifi_sta_autoconnect_disable on the device console; the call is kept here
 * because it is the correct wire instruction and costs one 512 B CMD53. */
static int ipronet_ctrl_device_autoconnect(uint8_t en)
{
    return ipronet_ctrl_send(IPRONET_CMD_STA_SET_AUTO_RECONNECT, 0, &en, sizeof(en));
}

int ipronet_diag_device_autoconnect(int en)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;
    return ipronet_ctrl_device_autoconnect(en ? 1U : 0U);
}

void ipronet_diag_last_ack(uint16_t *cmd, uint16_t *status)
{
    if (cmd)    *cmd    = s_last_ack_cmd;
    if (status) *status = s_last_ack_status;
}

/* Send STA_CONNECT for the given credentials (shared by ipronet_connect and
 * the reconnect worker). */
static int ipro6s_sta_connect_send(const char *ssid, const char *psk)
{
    ipronet_sta_connect_body_t body;
    memset(&body, 0, sizeof(body));
    size_t sl = strlen(ssid); if (sl > sizeof(body.ssid))     sl = sizeof(body.ssid);
    size_t pl = strlen(psk);  if (pl > sizeof(body.password)) pl = sizeof(body.password);
    body.ssid_len = (uint16_t)sl;
    memcpy(body.ssid, ssid, sl);
    memcpy(body.password, psk, pl);
    return ipronet_ctrl_send(IPRONET_CMD_STA_CONNECT, 0, &body, sizeof(body));
}

/* Reconnect worker: woken by STA_DISCONNECTED_IND, re-issues STA_CONNECT until
 * the link is back. The device answers each request with exactly one scan +
 * association attempt, and a single attempt is known to time out right after an
 * AP channel hop (the AP is still settling), so the retry loop IS the fix —
 * short backoff first, then a lazy 10 s cadence that also rides out longer AP
 * outages without spamming the air. */
static void reconn_task(void *arg)
{
    (void)arg;
    for (;;) {
        xSemaphoreTake(s_reconn_sem, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(2000));       /* let the AP settle / coalesce */
        int attempt = 0;
        while (s_reconn_en && !s_reconn_user_down && !s_ip_valid &&
               s_reconn_ssid[0] != '\0') {
            attempt++;
            s_reconn_attempts++;
            printf("[reconn] attempt %d: '%s'\r\n", attempt, s_reconn_ssid);
            (void)ipro6s_sta_connect_send(s_reconn_ssid, s_reconn_psk);
            for (int i = 0; i < 30 && !s_ip_valid; i++) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            if (s_ip_valid) {
                s_reconn_restores++;
                printf("[reconn] link restored (attempt %d)\r\n", attempt);
                break;
            }
            if (s_link_up) {
                /* Associated but no DHCP lease inside the window: half-open.
                 * Tear it down so the next attempt reruns the full sequence
                 * (a bare re-CONNECT on a COMPLETED supplicant is a no-op). */
                printf("[reconn] assoc up but no IP — reset link\r\n");
                (void)ipronet_ctrl_send(IPRONET_CMD_STA_DISCONNECT, 0, NULL, 0);
                vTaskDelay(pdMS_TO_TICKS(2000));
            }
            vTaskDelay(pdMS_TO_TICKS(attempt < 5 ? 3000 : 10000));
        }
    }
}

static void reconn_arm(const char *ssid, const char *psk)
{
    size_t sl = strlen(ssid); if (sl > sizeof(s_reconn_ssid) - 1) sl = sizeof(s_reconn_ssid) - 1;
    size_t pl = strlen(psk);  if (pl > sizeof(s_reconn_psk) - 1)  pl = sizeof(s_reconn_psk) - 1;
    memcpy(s_reconn_ssid, ssid, sl); s_reconn_ssid[sl] = '\0';
    memcpy(s_reconn_psk,  psk,  pl); s_reconn_psk[pl]  = '\0';
    s_reconn_user_down = 0;
    if (!s_reconn_task_up) {
        s_reconn_sem = xSemaphoreCreateBinary();
        if (s_reconn_sem &&
            xTaskCreate(reconn_task, "ipro6s_reconn", 2048, NULL,
                        tskIDLE_PRIORITY + 2, NULL) == pdPASS) {
            s_reconn_task_up = 1;
        } else {
            printf("[reconn] worker create FAILED\r\n");
        }
    }
}

void ipronet_ctrl_set_auto_reconnect(uint8_t en) { s_reconn_en = en; }
int  ipronet_diag_set_reconnect(int en)          { s_reconn_en = (en != 0); return IPRONET_OK; }
void ipronet_diag_reconn_stats(ipronet_reconn_stats_t *o)
{
    o->enabled = s_reconn_en;
    o->armed = (!s_reconn_user_down && s_reconn_task_up);
    memcpy(o->ssid, s_reconn_ssid, sizeof(o->ssid));
    o->attempts = s_reconn_attempts; o->restores = s_reconn_restores;
}

/* Dispatch a device->host control/event frame (called from the RX worker). */
void ipronet_ctrl_handle(const uint8_t *pld, uint16_t len)
{
    const ipronet_ctrl_hdr_t *h = (const ipronet_ctrl_hdr_t *)pld;

    switch (h->cmd) {
    case IPRONET_CMD_STA_CONNECTED_IND:
        s_link_up = 1;
        if (s_cb.on_connected) s_cb.on_connected(s_cb.arg);
        break;

    case IPRONET_CMD_STA_DISCONNECTED_IND:
        s_link_up  = 0;
        s_ip_valid = 0;
        if (s_cb.on_disconnected) s_cb.on_disconnected(s_cb.arg);
        if (s_reconn_en && !s_reconn_user_down && s_reconn_task_up) {
            xSemaphoreGive(s_reconn_sem);      /* rx_task context: task-level give */
        }
        break;

    case IPRONET_CMD_STA_IP_UPDATE_IND:
        if (len >= sizeof(ipronet_sta_ip_update_ind_msg_t)) {
            const ipronet_sta_ip_update_ind_msg_t *ip =
                (const ipronet_sta_ip_update_ind_msg_t *)pld;
            memcpy(s_ip.ip4,   ip->ip4_addr, 4);
            memcpy(s_ip.mask4, ip->ip4_mask, 4);
            memcpy(s_ip.gw4,   ip->ip4_gw,   4);
            memcpy(s_ip.dns1,  ip->ip4_dns1, 4);
            memcpy(s_ip.dns2,  ip->ip4_dns2, 4);
            ipronet_netif_set_ip(ip->ip4_addr, ip->ip4_mask, ip->ip4_gw);
            s_link_up  = 1;
            s_ip_valid = 1;
            if (s_cb.on_got_ip) s_cb.on_got_ip(s_cb.arg, &s_ip);
        }
        break;

    case IPRONET_CMD_SCAN_RESULTS:
        if (len >= sizeof(ipronet_scan_ind_msg_t)) {
            const ipronet_scan_ind_msg_t *s = (const ipronet_scan_ind_msg_t *)pld;
            uint16_t maxn = (uint16_t)((len - sizeof(ipronet_scan_ind_msg_t)) /
                                       sizeof(ipronet_scan_record_t));
            uint16_t n = (s->num < maxn) ? s->num : maxn;
            if (n > IPRONET_SCAN_MAX_APS) {
                printf("[ctrl] scan: %u results, delivering the first %u\r\n",
                       (unsigned)n, (unsigned)IPRONET_SCAN_MAX_APS);
                n = IPRONET_SCAN_MAX_APS;
            }
            for (uint16_t i = 0; i < n; i++) {
                const ipronet_scan_record_t *r = &s->records[i];
                ipronet_ap_t *ap = &s_scan_aps[i];
                memcpy(ap->bssid, r->bssid, 6);
                memcpy(ap->ssid, r->ssid, 32); ap->ssid[32] = '\0';
                ap->channel = r->channel; ap->rssi = r->rssi;
                ap->auth_mode = r->auth_mode;
            }
            if (s_cb.on_scan_result) s_cb.on_scan_result(s_cb.arg, s_scan_aps, n);
        }
        break;

    case IPRONET_CMD_GET_MAC_ADDR:
        /* Async ACK reply to our GET_MAC request: hdr + sta_mac[6] + ap_mac[6]
         * (24 B). Validate length, cache both MACs, and push the STA MAC into
         * iproeth0 (netif hwaddr). */
        if (len >= sizeof(ipronet_mac_addr_ind_msg_t)) {
            const ipronet_mac_addr_ind_msg_t *m =
                (const ipronet_mac_addr_ind_msg_t *)pld;
            memcpy(s_sta_mac, m->sta_mac, 6);
            memcpy(s_ap_mac,  m->ap_mac,  6);
            s_mac_learned = 1;
            ipronet_netif_set_mac(s_sta_mac);
        }
        break;

    case IPRONET_CMD_USER_EXT: {
        /* Reply to our ZCEN opt-in: a granting device answers "ZCOK<n>" (n =
         * armed slot count); a pre-zc FW echoes its own template string, which
         * just means the copy path stays. Printed, not acted on: ZCEN1 needs
         * no host-side wire change beyond the single-frame commit already made
         * when the opt-in was sent. */
        const char *p = (const char *)pld + sizeof(ipronet_ctrl_hdr_t);
        int plen = (int)len - (int)sizeof(ipronet_ctrl_hdr_t);
        char rsp[32];   /* the console printf has no %.*s — NUL-terminate here */
        char *endp = NULL;

        if (plen < 0) {
            plen = 0;
        }
        if (plen > (int)sizeof(rsp) - 1) {
            plen = (int)sizeof(rsp) - 1;
        }
        memcpy(rsp, p, (size_t)plen);
        rsp[plen] = '\0';

        if (plen >= 6 && memcmp(rsp, "ZCOK3 ", 6) == 0) {
            unsigned long nslot = strtoul(rsp + 6, &endp, 10);
            unsigned long stride = (endp != NULL) ? strtoul(endp, &endp, 10) : 0;
            unsigned long uslots = (endp != NULL) ? strtoul(endp, NULL, 10) : 0;

            if (nslot >= 2 && nslot <= IPRO6S_ZC2_MAX_SLOT &&
                stride == IPRO6S_ZC2_STRIDE) {
                ipronet_sdio_tx_agg_accept_grant((uint32_t)nslot,
                                                 (uint32_t)stride);
                printf("[zc] stride TX granted: nslot=%lu stride=%lu\r\n",
                       nslot, stride);
            } else {
                printf("[zc] stride TX refused (nslot=%lu stride=%lu)\r\n",
                       nslot, stride);
            }
            if (uslots >= 1 && uslots <= IPRO6S_ZC3_MAX_SLOT) {
                ipronet_sdio_set_zc3_uslots((uint32_t)uslots);
                printf("[zc] stride RX granted: uslots=%lu stride=%u\r\n",
                       uslots, (unsigned)IPRO6S_ZC3_STRIDE);
            } else {
                printf("[zc] stride RX refused (uslots=%lu) - legacy wire\r\n",
                       uslots);
            }
        } else if (plen >= 6 && memcmp(rsp, "ZCOK2 ", 6) == 0) {
            /* "ZCOK2 <nslot> <stride>" — arm the stride packer. Publish every
             * negotiated field before restoring the cap: the cap is what opens
             * aggregation after ipronet_ctrl_zcen() forced it to zero. */
            unsigned long nslot = strtoul(rsp + 6, &endp, 10);
            unsigned long stride = (endp != NULL) ? strtoul(endp, NULL, 10) : 0;

            if (nslot >= 2 && nslot <= IPRO6S_ZC2_MAX_SLOT &&
                stride == IPRO6S_ZC2_STRIDE) {
                ipronet_sdio_tx_agg_accept_grant((uint32_t)nslot,
                                                 (uint32_t)stride);
                printf("[zc] stride TX granted: nslot=%lu stride=%lu\r\n",
                       nslot, stride);
            } else {
                printf("[zc] stride refused (nslot=%lu stride=%lu) - single frame\r\n",
                       nslot, stride);
            }
        } else if (plen >= 4 && memcmp(rsp, "ZCOK", 4) == 0) {
            printf("[zc] device granted zero-copy: %s\r\n", rsp);
        } else {
            printf("[zc] no grant (device replied '%s') - copy path stays\r\n", rsp);
        }
        break;
    }

    default:
        if (h->flags & IPRONET_FLAG_ACK) {
            s_last_ack_cmd    = h->cmd;
            s_last_ack_status = h->status;
            printf("[ctrl] ACK cmd=%u status=%u replying=%u\r\n",
                   (unsigned)h->cmd, (unsigned)h->status,
                   (unsigned)h->msg_id_replying);
        } else {
            printf("[ctrl] cmd=%u flags=0x%x\r\n",
                   (unsigned)h->cmd, (unsigned)h->flags);
        }
        break;
    }
}

/* IPRO_CMD_HELLO sentinel — engages DUT bridge per ipronet guide §11
 * "send_hello CMD on attach". Linux driver issues this at the end of
 * ipro_sdio_init() to tell the DUT FW the host is up. The DUT logs the
 * cmd but doesn't act on it; the side effect is that subsequent WiFi
 * RX frames now route through SDIO instead of getting dropped at the
 * bridge input filter.
 *
 * Body is a 12-byte rnm_base_msg_t with cmd=IPRO_CMD_HELLO=2 (per
 * ~/work/ext/ipro_iotnet/rnm_msg.h:7-12; HELLO is the third enum). */
int ipronet_ctrl_send_hello(void)
{
    uint8_t rnm[12] = {0};
    rnm[0] = IPRONET_CMD_HELLO & 0xff;
    rnm[1] = (IPRONET_CMD_HELLO >> 8) & 0xff;
    return ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_CMD, 0, rnm, sizeof(rnm));
}

/* Request the device STA/AP MAC (GET_MAC_ADDR, bare header) and wait (bounded)
 * for the async CMD reply. The RX task MUST already be running — the reply
 * lands on the doorbell RX path that ipronet_ctrl_handle drains. The device
 * STA MAC is random per device boot, so this must be re-run after any device
 * reset. */
int ipronet_ctrl_get_mac(void)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;
    s_mac_learned = 0;
    int rc = ipronet_ctrl_send(IPRONET_CMD_GET_MAC_ADDR, 0, NULL, 0);
    if (rc != IPRONET_OK) return rc;
    /* Bounded ~1s wait for the reply (rx_task dispatches it into s_sta_mac). */
    for (int i = 0; i < 100 && !s_mac_learned; i++) vTaskDelay(pdMS_TO_TICKS(10));
    return s_mac_learned ? IPRONET_OK : IPRONET_ERR_NO_MAC;
}

/* Zero-copy opt-in, the contract the linux host runs by default (ipro_iotnet
 * ipro_sdio_eth.c, module param zc=1). Without it the device cannot arm its
 * zero-copy dnld slots, so every uplink frame takes the copy/bounce path
 * through g_txpbuf_payload_normal — a pool cut 24 -> 6 in cc19374b2 precisely
 * BECAUSE the zc slots were meant to carry the traffic.
 *
 * ZCEN1 = plain zero-copy, ZCEN2 adds host->device stride packing, and ZCEN3
 * adds device->host stride packing. All start single-frame; each stride
 * direction opens only after the corresponding device grant. Ordering is part
 * of the contract (ipronet_sdio_tx_single_frame_lock): commit to single-frame
 * TX and drain the pending batch BEFORE telling the device. The device answers
 * "ZCOK<n>" via USER_EXT; a pre-zc FW answers with its template string and we
 * simply stay on the copy path. */
int ipronet_ctrl_zcen(int gen)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;
    if (gen < 1 || gen > 3) return IPRONET_ERR_PARAM;
    uint8_t msg[sizeof(ipronet_ctrl_hdr_t) + 6] = {0};
    ipronet_ctrl_hdr_t *h = (ipronet_ctrl_hdr_t *)msg;
    h->cmd = IPRONET_CMD_USER_EXT;
    memcpy(msg + sizeof(*h), (gen == 3) ? "ZCEN3" : (gen == 2) ? "ZCEN2" : "ZCEN1", 6);
    ipronet_sdio_tx_single_frame_lock();      /* drain the batch, commit to single-frame TX */
    int rc = ipronet_sdio_send_pdu(IPRONET_MSG_TYPE_CMD, 0, msg, sizeof(msg));
    ipronet_sdio_tx_single_frame_unlock();
    return rc;
}

int ipronet_ctrl_link_status(void)
{
    if (!ipronet_sdio_is_setup()) return IPRONET_ERR_STATE;
    return ipronet_ctrl_send(IPRONET_CMD_STA_GET_LINK_STATUS, 0, NULL, 0);
}

bool ipronet_ctrl_mac_learned(void)       { return s_mac_learned != 0; }
const uint8_t *ipronet_ctrl_sta_mac(void) { return s_sta_mac; }
const uint8_t *ipronet_ctrl_ap_mac(void)  { return s_ap_mac; }
int ipronet_diag_send_hello(void)  { return ipronet_ctrl_send_hello(); }
void ipronet_diag_set_callbacks(const ipronet_callbacks_t *cb) { ipronet_ctrl_set_callbacks(cb); }
int ipronet_diag_get_mac(void)     { return ipronet_ctrl_get_mac(); }
int ipronet_diag_zcen(int gen)     { return ipronet_ctrl_zcen(gen); }
int ipronet_diag_link_status(void) { return ipronet_ctrl_link_status(); }
int ipronet_diag_ap_mac(uint8_t mac[6])
{
    if (!s_mac_learned) return IPRONET_ERR_STATE;
    memcpy(mac, s_ap_mac, 6); return IPRONET_OK;
}

/* ---- public station API ------------------------------------------------- */

int ipronet_connect(const char *ssid, const char *psk)
{
    if (!ipronet_is_ready()) return IPRONET_ERR_STATE;
    if (!ssid || !psk || strlen(ssid) == 0 || strlen(ssid) > 32 || strlen(psk) > 64)
        return IPRONET_ERR_PARAM;
    (void)ipronet_ctrl_device_autoconnect(0);   /* link recovery is the host's job */
    reconn_arm(ssid, psk);
    return ipro6s_sta_connect_send(ssid, psk);
}

int ipronet_disconnect(void)
{
    if (!ipronet_is_ready()) return IPRONET_ERR_STATE;
    s_reconn_user_down = 1;              /* deliberate: stay down */
    return ipronet_ctrl_send(IPRONET_CMD_STA_DISCONNECT, 0, NULL, 0);
}

/* Start a scan, then request the results (delivered via on_scan_result). */
int ipronet_scan(void)
{
    if (!ipronet_is_ready()) return IPRONET_ERR_STATE;
    int rc = ipronet_ctrl_send(IPRONET_CMD_SCAN, 0, NULL, 0);
    if (rc != IPRONET_OK) return rc;
    vTaskDelay(pdMS_TO_TICKS(3000));
    return ipronet_ctrl_send(IPRONET_CMD_SCAN_RESULTS, 0, NULL, 0);
}

bool ipronet_is_connected(void) { return s_link_up != 0; }
bool ipronet_has_ip(void)       { return s_ip_valid != 0; }

int ipronet_get_mac(uint8_t mac[6])
{
    if (!s_mac_learned) return IPRONET_ERR_STATE;
    memcpy(mac, s_sta_mac, 6); return IPRONET_OK;
}

int ipronet_get_ip(ipronet_ip_info_t *ip)
{
    if (!ip) return IPRONET_ERR_PARAM;
    if (!s_ip_valid) return IPRONET_ERR_STATE;
    *ip = s_ip; return IPRONET_OK;
}
