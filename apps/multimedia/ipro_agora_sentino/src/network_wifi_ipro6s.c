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

/**
 * @file network_wifi_ipro6s.c
 * @brief Wi-Fi network backend: IPRO6S_FIN over SDIO (components/network/ipronet_host)
 *
 * network_ops_t implementation next to network_emac.c / network_wifi_aic.c,
 * selected with CONFIG_NETWORK_USE_WIFI_IPRO6S. The IPRO6S on the fin runs
 * ipro6_if_wifi (defconfig_ipro6_sdio) and is the radio; this side owns lwIP
 * and sees the link as the netif iproeth0 (apps/platform/ipro7_sdio_wifi is
 * the reference bench app for the same component).
 *
 *   init()      creates the lock; nothing else before the scheduler:
 *               ipronet_start() runs tcpip_init() itself, once per boot, and
 *               a second call here would start a second tcpip thread.
 *   connect()   starts the supervisor task: ipronet_start() (power-cycles
 *               the fin through its rail, enumerates, brings iproeth0 up),
 *               then ipronet_connect() with the stored credentials. The
 *               device runs DHCP; when its address arrives the task installs
 *               the DNS servers, which the component leaves to the app.
 *   reconnect() nothing to force. The fin's own wifi_mgmr autoreconnect
 *               covers a dropped association (the component's reconnect
 *               worker is off, see fin_config()), but not a dead transport (a
 *               fin that lost power or rebooted only shows as TX errors and
 *               stalled TCP). So the supervisor gives the link
 *               WIFI_LINK_GRACE_MS to come back and then restarts the
 *               transport from scratch.
 *   scan        network_wifi_ipro6s_scan() serves BLE provisioning's AP list,
 *               which runs before connect(): it brings the transport up on its
 *               own and leaves it up for the supervisor.
 *
 * Every ipronet_* call that changes state goes through s_lock, because the
 * scan (provisioning task) and the supervisor can both want the component.
 *
 * Credentials: EasyFlash keys "wifi_ssid" / "wifi_psk" (BLE provisioning or
 * the `wifi_set` shell command), falling back to CONFIG_WIFI_STA_SSID / _PSK.
 *
 * Rail: on the IPRO7AI Reef the fin is fed from SD_VDD, switched by GPIO14
 * (low = on) only with JP3 on 2-3. With JP3 on 1-2 the rail stays on, the
 * power-cycle does nothing and a warm IPRO7 reset leaves the fin holding the
 * old SDIO session, so enumeration fails until the board is power-cycled.
 */
#include "network_wifi_ipro6s.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "board_config.h"
#include "ipronet_host.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/api.h"
#include "app_log.h"

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define LOG_TAG "WIFI"

#ifndef CONFIG_WIFI_STA_SSID
#define CONFIG_WIFI_STA_SSID ""
#endif
#ifndef CONFIG_WIFI_STA_PSK
#define CONFIG_WIFI_STA_PSK  ""
#endif

#define WIFI_SSID_MAX        32
#define WIFI_PSK_MAX         64
#define WIFI_TASK_STACK      3072
#define WIFI_TASK_PRIO       (configMAX_PRIORITIES - 4)
#define WIFI_POLL_MS         500
#define WIFI_START_BACKOFF_S 2
#define WIFI_BACKOFF_MAX_S   30
/* From ipronet_connect() (or from losing the address) to giving up on the
 * component's own recovery and restarting the transport. Association plus
 * the device's DHCP normally take 3-8 s. */
#define WIFI_LINK_GRACE_MS   30000
#define WIFI_DNS_FALLBACK    "8.8.8.8"
#define WIFI_PRESCAN_TIMEOUT_MS 6000
/* The most APs the device reports in one SCAN_RESULTS. */
#define WIFI_SCAN_KEEP       45

static char s_ssid[WIFI_SSID_MAX + 1];
static char s_psk[WIFI_PSK_MAX + 1];
static TaskHandle_t s_task;
static SemaphoreHandle_t s_lock;          /* serialises ipronet start/stop/connect/scan */
static volatile bool s_transport_up;      /* ipronet_start() succeeded, no stop since */
static volatile bool s_scanned;           /* a scan completed on this transport session */
static volatile bool s_restart;           /* set by reconnect()/wifi_set, served by the task */

/* Written by the component's RX task (callbacks), read by the supervisor. */
static volatile uint32_t s_ip_epoch;      /* bumped on every on_got_ip */
static volatile uint32_t s_ip_epoch_done; /* epoch the DNS servers were installed for */
static volatile uint32_t s_disconnects;

/* Scan results, filled by on_scan_result and handed out by the scan call. */
static SemaphoreHandle_t s_scan_done;
static network_wifi_ap_t s_scan_buf[WIFI_SCAN_KEEP];
static volatile int s_scan_n;

/* ------------------------------------------------------------------ */
/* Credentials                                                         */
/* ------------------------------------------------------------------ */

static void load_credentials(void)
{
    s_ssid[0] = '\0';
    s_psk[0] = '\0';
#ifdef CONFIG_EASYFLASH4
    {
        char *v = ef_get_env("wifi_ssid");
        if (v != NULL && v[0] != '\0') {
            strncpy(s_ssid, v, WIFI_SSID_MAX);
            v = ef_get_env("wifi_psk");
            if (v != NULL) {
                strncpy(s_psk, v, WIFI_PSK_MAX);
            }
        }
    }
#endif
    if (s_ssid[0] == '\0') {
        strncpy(s_ssid, CONFIG_WIFI_STA_SSID, WIFI_SSID_MAX);
        strncpy(s_psk, CONFIG_WIFI_STA_PSK, WIFI_PSK_MAX);
    }
    s_ssid[WIFI_SSID_MAX] = '\0';
    s_psk[WIFI_PSK_MAX] = '\0';
}

int network_wifi_ipro6s_set_credentials(const char *ssid, const char *psk)
{
    if (ssid == NULL || ssid[0] == '\0' || strlen(ssid) > WIFI_SSID_MAX ||
        (psk != NULL && strlen(psk) > WIFI_PSK_MAX)) {
        return -1;
    }
#ifdef CONFIG_EASYFLASH4
    ef_set_env("wifi_ssid", ssid);
    ef_set_env("wifi_psk", psk ? psk : "");
    ef_save_env();
#endif
    IPRO_LOGI(LOG_TAG, "credentials stored for \"%s\"", ssid);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Component callbacks (RX task: no blocking, no ipronet_* calls)      */
/* ------------------------------------------------------------------ */

static void on_got_ip(void *arg, const ipronet_ip_info_t *ip)
{
    (void)arg;
    (void)ip;   /* read back with ipronet_get_ip() from the supervisor */
    s_ip_epoch++;
}

static void on_disconnected(void *arg)
{
    (void)arg;
    s_disconnects++;
}

static void on_scan_result(void *arg, const ipronet_ap_t *aps, uint16_t n)
{
    (void)arg;
    int keep = (n < WIFI_SCAN_KEEP) ? (int)n : WIFI_SCAN_KEEP;

    for (int i = 0; i < keep; i++) {
        memcpy(s_scan_buf[i].ssid, aps[i].ssid, sizeof(s_scan_buf[i].ssid));
        s_scan_buf[i].ssid[sizeof(s_scan_buf[i].ssid) - 1] = '\0';
        s_scan_buf[i].rssi = aps[i].rssi;
        s_scan_buf[i].secure = (aps[i].auth_mode != IPRONET_AUTH_OPEN);
    }
    s_scan_n = keep;
    if (s_scan_done != NULL) {
        xSemaphoreGive(s_scan_done);
    }
}

static const ipronet_callbacks_t s_callbacks = {
    .on_got_ip       = on_got_ip,
    .on_disconnected = on_disconnected,
    .on_scan_result  = on_scan_result,
};

/* ------------------------------------------------------------------ */
/* Transport (call with s_lock held)                                   */
/* ------------------------------------------------------------------ */

static void fin_config(ipronet_config_t *cfg)
{
    ipronet_config_default(cfg);   /* 25 MHz, 16 Mbps pacer, EVB rail */
    /* One owner for re-association. ipro6_if_wifi turns wifi_mgmr's own
     * autoreconnect on at boot and ACKs-but-ignores the host's request to
     * turn it off, so the device already retries by itself. With the
     * component's reconnect worker on as well, every failed attempt drew a
     * second STA_CONNECT from the host that aborted the device's own retry
     * (Reef, 2026-10-08: two DISCONNECTED_IND, "[reconn] attempt 1", never an
     * address). The worker also survives ipronet_stop() and fired into the
     * next session's fresh connect. The supervisor covers what is left: no
     * address for WIFI_LINK_GRACE_MS restarts the transport. */
    cfg->auto_reconnect = 0;
#if defined(CONFIG_IPRO7AI_REEF)
    cfg->pwr_en_gpio    = BOARD_SD_PWR_EN_PIN;
    cfg->pwr_active_low = (BOARD_SD_PWR_ON_LEVEL == 0);
#endif
}

static bool transport_start_locked(void)
{
    ipronet_config_t cfg;

    if (s_transport_up) {
        return true;
    }
    fin_config(&cfg);
    IPRO_LOGI(LOG_TAG, "IPRO6S fin: power-cycle (GPIO%d) + SDIO bring-up",
              (int)cfg.pwr_en_gpio);
    int rc = ipronet_start(&cfg, &s_callbacks);
    if (rc != IPRONET_OK) {
        IPRO_LOGE(LOG_TAG, "ipronet_start failed (%d)%s", rc,
                  rc == IPRONET_ERR_NO_DEVICE
                      ? " - fin seated? ipro6_if_wifi flashed? JP3 on 2-3?" : "");
        return false;
    }

    uint8_t mac[6];
    if (ipronet_get_mac(mac) == IPRONET_OK) {
        IPRO_LOGI(LOG_TAG, "iproeth0 up, MAC %02X:%02X:%02X:%02X:%02X:%02X",
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    s_transport_up = true;
    s_scanned = false;
    return true;
}

static void transport_stop_locked(void)
{
    (void)ipronet_stop();
    s_transport_up = false;
    s_ip_epoch_done = s_ip_epoch;   /* nothing pending until the next on_got_ip */
}

/* ------------------------------------------------------------------ */
/* Scan (BLE provisioning's AP list)                                   */
/* ------------------------------------------------------------------ */

int network_wifi_ipro6s_scan(network_wifi_ap_t *aps, int max, uint32_t timeout_ms)
{
    if (aps == NULL || max <= 0 || s_lock == NULL || s_scan_done == NULL) {
        return -1;
    }

    xSemaphoreTake(s_lock, portMAX_DELAY);
    if (!transport_start_locked()) {
        xSemaphoreGive(s_lock);
        return -1;
    }
    (void)xSemaphoreTake(s_scan_done, 0);   /* drop a result nobody waited for */
    int rc = ipronet_scan();
    xSemaphoreGive(s_lock);
    if (rc != IPRONET_OK) {
        IPRO_LOGE(LOG_TAG, "ipronet_scan failed (%d)", rc);
        return -1;
    }

    if (xSemaphoreTake(s_scan_done, pdMS_TO_TICKS(timeout_ms)) != pdTRUE) {
        IPRO_LOGW(LOG_TAG, "scan: no result in %lu ms", (unsigned long)timeout_ms);
        return -1;
    }
    int n = (s_scan_n < max) ? s_scan_n : max;
    memcpy(aps, s_scan_buf, (size_t)n * sizeof(*aps));
    s_scanned = true;
    IPRO_LOGI(LOG_TAG, "scan: %d APs", s_scan_n);
    return n;
}

/* ------------------------------------------------------------------ */
/* Supervisor                                                          */
/* ------------------------------------------------------------------ */

static bool ip_nonzero(const uint8_t a[4])
{
    return (a[0] | a[1] | a[2] | a[3]) != 0;
}

/* The device's DHCP result carries up to two DNS servers; the component only
 * sets address, mask and gateway. Keep the DHCP servers first and put a
 * public resolver behind a lone one (lwIP only moves to slot 1 after slot 0
 * exhausts its retries, and some lab resolvers ignore Wi-Fi clients). */
static void install_dns(const ipronet_ip_info_t *ip)
{
    ip_addr_t srv[2];
    int n = 0;

    if (ip_nonzero(ip->dns1)) {
        IP_ADDR4(&srv[n], ip->dns1[0], ip->dns1[1], ip->dns1[2], ip->dns1[3]);
        n++;
    }
    if (ip_nonzero(ip->dns2)) {
        IP_ADDR4(&srv[n], ip->dns2[0], ip->dns2[1], ip->dns2[2], ip->dns2[3]);
        n++;
    }
    if (n < 2 && ipaddr_aton(WIFI_DNS_FALLBACK, &srv[n])) {
        n++;
    }

    LOCK_TCPIP_CORE();
    for (int i = 0; i < n; i++) {
        dns_setserver((u8_t)i, &srv[i]);
    }
    UNLOCK_TCPIP_CORE();

    IPRO_LOGI(LOG_TAG, "DNS: %s, %s", ipaddr_ntoa(dns_getserver(0)),
              ipaddr_ntoa(dns_getserver(1)));
}

/* Track the address: install DNS once per on_got_ip, and ask for a transport
 * restart when there has been no address for WIFI_LINK_GRACE_MS. */
static void watch_link(bool *had_ip, TickType_t *since)
{
    bool has_ip = ipronet_has_ip();

    if (has_ip && s_ip_epoch != s_ip_epoch_done) {
        uint32_t epoch = s_ip_epoch;
        ipronet_ip_info_t ip;

        if (ipronet_get_ip(&ip) == IPRONET_OK) {
            install_dns(&ip);
            s_ip_epoch_done = epoch;
            IPRO_LOGI(LOG_TAG, "connected, IP %u.%u.%u.%u gw %u.%u.%u.%u",
                      ip.ip4[0], ip.ip4[1], ip.ip4[2], ip.ip4[3],
                      ip.gw4[0], ip.gw4[1], ip.gw4[2], ip.gw4[3]);
        }
    }
    if (has_ip) {
        *had_ip = true;
        return;
    }
    if (*had_ip) {
        *had_ip = false;
        *since = xTaskGetTickCount();
    }
    if ((xTaskGetTickCount() - *since) > pdMS_TO_TICKS(WIFI_LINK_GRACE_MS)) {
        IPRO_LOGW(LOG_TAG, "no address for %d s", WIFI_LINK_GRACE_MS / 1000);
        s_restart = true;
    }
}

static void wifi_task(void *arg)
{
    (void)arg;
    bool assoc_sent = false;
    bool had_ip = false;
    int backoff_s = WIFI_START_BACKOFF_S;
    TickType_t since = 0;            /* start of the current no-address stretch */
    uint32_t disc_seen = s_disconnects;

    for (;;) {
        if (s_restart) {
            s_restart = false;
            xSemaphoreTake(s_lock, portMAX_DELAY);
            if (s_transport_up) {
                IPRO_LOGI(LOG_TAG, "restarting the fin transport");
                transport_stop_locked();
            }
            xSemaphoreGive(s_lock);
            assoc_sent = false;
            had_ip = false;
        }

        if (!s_transport_up) {
            xSemaphoreTake(s_lock, portMAX_DELAY);
            bool ok = transport_start_locked();
            xSemaphoreGive(s_lock);
            if (!ok) {
                vTaskDelay(pdMS_TO_TICKS(backoff_s * 1000));
                if (backoff_s < WIFI_BACKOFF_MAX_S) {
                    backoff_s *= 2;
                }
                continue;
            }
            backoff_s = WIFI_START_BACKOFF_S;
        }

        if (!assoc_sent) {
            load_credentials();
            if (s_ssid[0] == '\0') {
                IPRO_LOGW(LOG_TAG, "no Wi-Fi credentials: use `wifi_set <ssid> <psk>`");
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            if (!s_scanned) {
                /* The first association after the fin boots failed every
                 * time when it was sent straight after bring-up, and never
                 * when a scan had run first (the provisioning path). */
                static network_wifi_ap_t first[1];
                (void)network_wifi_ipro6s_scan(first, 1, WIFI_PRESCAN_TIMEOUT_MS);
            }
            IPRO_LOGI(LOG_TAG, "connecting to \"%s\"...", s_ssid);
            xSemaphoreTake(s_lock, portMAX_DELAY);
            int rc = ipronet_connect(s_ssid, s_psk);
            xSemaphoreGive(s_lock);
            if (rc != IPRONET_OK) {
                IPRO_LOGE(LOG_TAG, "ipronet_connect failed (%d)", rc);
                s_restart = true;
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
            assoc_sent = true;
            since = xTaskGetTickCount();
        }

        if (s_disconnects != disc_seen) {
            disc_seen = s_disconnects;
            IPRO_LOGW(LOG_TAG, "STA disconnected (the fin re-associates by itself)");
        }

        watch_link(&had_ip, &since);
        if (s_restart) {
            continue;
        }
        (void)ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(WIFI_POLL_MS));
    }
}

static void request_restart(void)
{
    s_restart = true;
    if (s_task != NULL) {
        xTaskNotifyGive(s_task);
    }
}

/* ------------------------------------------------------------------ */
/* network_ops_t                                                       */
/* ------------------------------------------------------------------ */

static app_err_t wifi_init(void)
{
    s_lock = xSemaphoreCreateMutex();
    s_scan_done = xSemaphoreCreateBinary();
    if (s_lock == NULL || s_scan_done == NULL) {
        IPRO_LOGE(LOG_TAG, "cannot create Wi-Fi semaphores");
        return APP_ERR_NOMEM;
    }
    IPRO_LOGI(LOG_TAG, "IPRO6S fin Wi-Fi backend (lwIP starts with the transport)");
    return APP_OK;
}

static app_err_t wifi_connect(void)
{
    if (s_task != NULL) {
        return APP_OK;
    }
    if (xTaskCreate(wifi_task, "wifi_fin", WIFI_TASK_STACK, NULL, WIFI_TASK_PRIO, &s_task) !=
        pdPASS) {
        IPRO_LOGE(LOG_TAG, "cannot create Wi-Fi task");
        return APP_ERR_NOMEM;
    }
    return APP_OK;
}

static app_err_t wifi_disconnect(void)
{
    /* Only the supervisor drives the link; ask it to drop and rebuild it.
     * A deliberate "stay down" has no user in this app. */
    request_restart();
    return APP_OK;
}

static int wifi_is_connected(void)
{
    return (ipronet_has_ip() && s_ip_epoch_done == s_ip_epoch) ? 1 : 0;
}

static app_err_t wifi_wait_ready(int timeout_s)
{
    for (int i = 0; i < timeout_s * 10; i++) {
        if (wifi_is_connected()) {
            return APP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    return APP_ERR_TIMEOUT;
}

static app_err_t wifi_reconnect(void)
{
    /* The gateway calls this as soon as is_connected() drops. Recovery is the
     * supervisor's alone: the device re-associates by itself, and no address
     * for WIFI_LINK_GRACE_MS restarts the transport. Forcing a restart here
     * as well landed in the middle of one already running and power-cycled
     * the fin a second time (Reef, 2026-10-08). */
    if (s_task == NULL) {
        return wifi_connect();
    }
    return APP_OK;
}

const network_ops_t network_wifi_ipro6s_ops = {
    .name         = "wifi_ipro6s",
    .init         = wifi_init,
    .connect      = wifi_connect,
    .disconnect   = wifi_disconnect,
    .is_connected = wifi_is_connected,
    .wait_ready   = wifi_wait_ready,
    .reconnect    = wifi_reconnect,
};

/* ------------------------------------------------------------------ */
/* Shell                                                               */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_SHELL
static int cmd_wifi_set(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: wifi_set <ssid> [psk]\r\n");
        return -1;
    }
    if (network_wifi_ipro6s_set_credentials(argv[1], argc > 2 ? argv[2] : "") != 0) {
        printf("wifi_set: invalid ssid/psk\r\n");
        return -1;
    }
    printf("stored; restarting the link\r\n");
    request_restart();
    return 0;
}

static int cmd_wifi_status(int argc, char **argv)
{
    ipronet_ip_info_t ip;
    uint8_t mac[6] = {0};

    (void)argc;
    (void)argv;
    (void)ipronet_get_mac(mac);
    printf("transport=%s assoc=%s ip=%s ssid=\"%s\" disconnects=%lu\r\n",
           ipronet_is_ready() ? "up" : "down",
           ipronet_is_connected() ? "up" : "down",
           ipronet_has_ip() ? "yes" : "no", s_ssid, (unsigned long)s_disconnects);
    printf("mac=%02X:%02X:%02X:%02X:%02X:%02X\r\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    if (ipronet_get_ip(&ip) == IPRONET_OK) {
        printf("ip=%u.%u.%u.%u mask=%u.%u.%u.%u gw=%u.%u.%u.%u\r\n",
               ip.ip4[0], ip.ip4[1], ip.ip4[2], ip.ip4[3],
               ip.mask4[0], ip.mask4[1], ip.mask4[2], ip.mask4[3],
               ip.gw4[0], ip.gw4[1], ip.gw4[2], ip.gw4[3]);
        printf("dns=%s, %s\r\n", ipaddr_ntoa(dns_getserver(0)),
               ipaddr_ntoa(dns_getserver(1)));
    }
    return 0;
}

static int cmd_wifi_scan(int argc, char **argv)
{
    static network_wifi_ap_t aps[WIFI_SCAN_KEEP];

    (void)argc;
    (void)argv;
    int n = network_wifi_ipro6s_scan(aps, WIFI_SCAN_KEEP, 8000);
    if (n < 0) {
        printf("wifi_scan failed\r\n");
        return -1;
    }
    for (int i = 0; i < n; i++) {
        printf("  %-32s %4d %s\r\n", aps[i].ssid, (int)aps[i].rssi,
               aps[i].secure ? "secure" : "open");
    }
    printf("%d APs\r\n", n);
    return 0;
}

static int cmd_wifi_dns(int argc, char **argv)
{
    ip_addr_t addr;

    if (argc < 2) {
        printf("usage: wifi_dns <host>\r\n");
        return -1;
    }
    if (!wifi_is_connected()) {
        printf("wifi_dns: link not up\r\n");
        return -1;
    }
    err_t err = netconn_gethostbyname(argv[1], &addr);
    printf("%s -> %s (err %d)\r\n", argv[1], err == ERR_OK ? ipaddr_ntoa(&addr) : "?",
           (int)err);
    return err == ERR_OK ? 0 : -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_set, wifi_set, "wifi_set <ssid> [psk]: store credentials and reconnect");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_status, wifi_status, "IPRO6S fin Wi-Fi link status");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_scan, wifi_scan, "wifi_scan: list APs through the fin");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_dns, wifi_dns, "wifi_dns <host>: resolve a name");
#endif
