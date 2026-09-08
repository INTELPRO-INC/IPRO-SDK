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
 * ipronet_host.h - FreeRTOS host API for the ipronet SDIO-WiFi bridge.
 *
 * IPRO7 is the SDIO host; the IPRO6S device runs ipro6_if_wifi (auto-start
 * firmware) and is the radio. This component drives it over the SDH
 * controller, presents the link as the lwIP netif "iproeth0" and reports WiFi
 * events through the callbacks below. Once on_got_ip has fired, use BSD
 * sockets as on any netif.
 *
 * Minimal program:
 *
 *   static void got_ip(void *arg, const ipronet_ip_info_t *ip) { ... }
 *
 *   ipronet_config_t cfg;
 *   ipronet_config_default(&cfg);
 *   ipronet_callbacks_t cb = { .on_got_ip = got_ip };
 *   while (ipronet_start(&cfg, &cb) != IPRONET_OK)   // device may still be booting
 *       vTaskDelay(pdMS_TO_TICKS(1000));
 *   ipronet_connect("SSID", "psk");
 *
 * lwIP: copy apps/platform/ipro7_sdio_wifi/lwipopts_app.h into your app.
 * Without it (PSRAM pools, window scale, the fork's TCP loss recovery with
 * dupack threshold 3) TCP uplink drops from ~8 Mbps to ~2 Mbps.
 *
 * Callback contract:
 *   - Callbacks run in the component's RX task, never in ISR context.
 *   - They must not block, and must not call ipronet_connect/disconnect/scan/
 *     stop/device_power (the RX task holds the SDIO host around the read that
 *     produced the event). The state getters are safe. Hand the event to your
 *     own task if you need to act on it.
 *   - on_disconnected is also delivered right before the component's own
 *     reconnect loop runs; on_connected / on_got_ip follow when it succeeds.
 *
 * Not provided: transport-death detection (a powered-off device makes TX
 * return IPRONET_ERR_IO and TCP time out; recover with ipronet_stop() then
 * ipronet_start()), AP / monitor mode, more than one netif.
 */
#ifndef IPRONET_HOST_H
#define IPRONET_HOST_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- return codes ------------------------------------------------------- */
#define IPRONET_OK              0
#define IPRONET_ERR_STATE      -1   /* wrong order (e.g. connect before start) */
#define IPRONET_ERR_NOMEM      -2   /* task / semaphore allocation failed */
#define IPRONET_ERR_SETUP      -3   /* SDH host init or bus setup failed */
#define IPRONET_ERR_NO_DEVICE  -4   /* enumerate failed or FBR std-IF != WLAN */
#define IPRONET_ERR_NO_MAC     -5   /* GET_MAC reply did not arrive within 1 s */
#define IPRONET_ERR_IO         -6   /* CMD53 write failed */
#define IPRONET_ERR_PARAM      -7

/* ---- auth modes in scan records (wire values, do not renumber) ---------- */
#define IPRONET_AUTH_UNKNOWN            0
#define IPRONET_AUTH_OPEN               1
#define IPRONET_AUTH_WEP                2
#define IPRONET_AUTH_WPA_PSK            3
#define IPRONET_AUTH_WPA2_PSK           4
#define IPRONET_AUTH_WPA_WPA2_PSK       5
#define IPRONET_AUTH_WPA_ENTERPRISE     6
#define IPRONET_AUTH_WPA3_SAE           7
#define IPRONET_AUTH_WPA2_PSK_WPA3_SAE  8

/* ---- payloads ----------------------------------------------------------- */
typedef struct {
    uint8_t ip4[4];
    uint8_t mask4[4];
    uint8_t gw4[4];
    uint8_t dns1[4];
    uint8_t dns2[4];
} ipronet_ip_info_t;                   /* octets a.b.c.d */

typedef struct {
    uint8_t  bssid[6];
    char     ssid[33];                 /* NUL-terminated */
    uint16_t channel;
    int8_t   rssi;
    uint8_t  auth_mode;                /* IPRONET_AUTH_* */
} ipronet_ap_t;

/* ---- callbacks (any member may be NULL) --------------------------------- */
typedef struct {
    void (*on_connected)(void *arg);
    void (*on_disconnected)(void *arg);
    void (*on_got_ip)(void *arg, const ipronet_ip_info_t *ip);
    void (*on_scan_result)(void *arg, const ipronet_ap_t *aps, uint16_t n);
    void *arg;
} ipronet_callbacks_t;

/* ---- config ------------------------------------------------------------- */
typedef struct {
    int8_t   pwr_en_gpio;      /* -1 = host does not control the device rail; EVB: 15 */
    uint8_t  pwr_active_low;   /* EVB: 1 (writing 0 turns the rail ON) */
    uint32_t bus_hz;           /* 25000000 (default) or 50000000 (High Speed) */
    uint8_t  auto_reconnect;   /* default 1 */
    uint32_t tx_pace_kbps;     /* default 16000; 0 = pacer off */
} ipronet_config_t;

/* Fill the EVB values: pwr_en_gpio 15 active-low, 25 MHz, reconnect on, 16000 kbps. */
void ipronet_config_default(ipronet_config_t *cfg);

/* ---- lifecycle ---------------------------------------------------------- */
/* Synchronous bring-up (~1.5 s, ~3.5 s with a power-cycle):
 *   [power-cycle the device if cfg->pwr_en_gpio >= 0] -> SDH init -> enumerate
 *   -> 4-bit / bus_hz / F1 block 512 -> RX task -> HELLO -> GET_MAC -> zero-copy
 *   opt-in -> iproeth0 up with the device STA MAC.
 * Every failure rolls back to idle, so calling again is safe. cb may be NULL. */
int  ipronet_start(const ipronet_config_t *cfg, const ipronet_callbacks_t *cb);
/* RX task stopped, F1 disabled, SDH controller back in ID mode, netif down.
 * Idempotent: IPRONET_OK when not started or only partially brought up. */
int  ipronet_stop(void);
/* Device rail control; IPRONET_ERR_STATE when pwr_en_gpio < 0. on=0 parks the
 * SDH pads and cuts the rail; on=1 restores the rail, holds 200 ms, restores
 * the SDH pinmux. The SDIO session is lost either way: ipronet_stop() +
 * ipronet_start() afterwards. */
int  ipronet_device_power(int on);

/* ---- station control (asynchronous; results come through the callbacks) - */
/* Stores the credentials, asks the DEVICE to switch its own autoconnect off
 * (link recovery needs exactly one owner: two of them race and strand the
 * link) and sends STA_CONNECT. NOTE: the ipro6_if_wifi firmware of 2026-09-02
 * ACKs that request and ignores it, so on that firmware the device also keeps
 * retrying by itself; see ipronet_ctrl.c. */
int  ipronet_connect(const char *ssid, const char *psk);   /* arms auto-reconnect */
int  ipronet_disconnect(void);                             /* deliberate down: reconnect disarmed */
int  ipronet_scan(void);                                   /* on_scan_result after ~3 s */

/* ---- state -------------------------------------------------------------- */
bool ipronet_is_ready(void);           /* transport up + RX task + device MAC learned: what start() leaves behind */
bool ipronet_is_connected(void);       /* STA associated */
bool ipronet_has_ip(void);             /* on_got_ip has fired since the last connect */
int  ipronet_get_mac(uint8_t mac[6]);  /* IPRONET_ERR_STATE before start() */
int  ipronet_get_ip(ipronet_ip_info_t *ip);   /* IPRONET_ERR_STATE while !has_ip */

#ifdef __cplusplus
}
#endif
#endif /* IPRONET_HOST_H */
