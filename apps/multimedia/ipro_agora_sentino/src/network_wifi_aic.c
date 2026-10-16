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
 * @file network_wifi_aic.c
 * @brief Wi-Fi network backend: AIC8800 D80-H over SDIO (components/wireless/wifi_aic8800)
 *
 * network_ops_t implementation next to network_emac.c / network_lte.c, selected
 * with CONFIG_NETWORK_USE_WIFI_AIC. The prebuilt AIC archive owns the driver,
 * wpa_supplicant and the lwIP netif; this file only sequences it:
 *
 *   init()      tcpip_init() before the scheduler (the driver needs the
 *               tcpip thread and its core lock, like the EMAC backend)
 *   connect()   background task: power-cycle the module, probe the SDIO
 *               driver once, then STA connect with the stored credentials,
 *               retrying with backoff until DHCP gives an address
 *   is_connected / wait_ready / reconnect / disconnect on top of that
 *
 * Credentials: EasyFlash keys "wifi_ssid" / "wifi_psk" (set with the
 * `wifi_set` shell command), falling back to CONFIG_WIFI_STA_SSID / _PSK.
 *
 * Module supply: on the IPRO7AI EVK the module sits on the micro-SD slot and
 * is powered through GPIO27 (BOARD_SD_PWR_EN_PIN, active low); on the EVB it
 * is WIFI_PWR_EN = GPIO16. A warm reset of IPRO7 leaves the module running
 * the old firmware with an RCA assigned, which the fresh enumeration cannot
 * talk to, so the supply is cycled before every bring-up.
 */
#include "network_wifi_aic.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "aic8800_ipro7.h"
#include "board_config.h"
#include "hal_gpio.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "app_log.h"

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif
#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#define LOG_TAG "WIFI"

/* Vendor STA API (platform/wifi/wifi_port_priv.h; exported by the archive).
 * Declared here so the vendor headers stay off the app's include path. */
extern int      app_wlan_sta_start(uint8_t *ssid, uint8_t *pw, int timeout_ms);
extern int      app_wlan_sta_stop(void);
extern uint32_t app_wlan_sta_get_ip(void);
extern int32_t  app_wlan_sta_conn_notify_reg(void (*notify_cb)(uint8_t, uint8_t *));

#if defined(CONFIG_IPRO7AI_EVK)
#define WIFI_PWR_PIN        BOARD_SD_PWR_EN_PIN
#define WIFI_PWR_ON_LEVEL   BOARD_SD_PWR_ON_LEVEL
#else
#define WIFI_PWR_PIN        16
#define WIFI_PWR_ON_LEVEL   1
#endif
#define WIFI_PWR_OFF_MS     200
#define WIFI_PWR_SETTLE_MS  100

#ifndef CONFIG_WIFI_STA_SSID
#define CONFIG_WIFI_STA_SSID ""
#endif
#ifndef CONFIG_WIFI_STA_PSK
#define CONFIG_WIFI_STA_PSK  ""
#endif

#define WIFI_SSID_MAX        32
#define WIFI_PSK_MAX         64
#define WIFI_CONNECT_TO_MS   20000
#define WIFI_TASK_STACK      6144
#define WIFI_TASK_PRIO       (configMAX_PRIORITIES - 4)

static char s_ssid[WIFI_SSID_MAX + 1];
static char s_psk[WIFI_PSK_MAX + 1];
static volatile bool s_driver_up;
static volatile bool s_link_up;
static volatile bool s_want_connect;
static TaskHandle_t s_task;

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
        }
        v = ef_get_env("wifi_psk");
        if (v != NULL) {
            strncpy(s_psk, v, WIFI_PSK_MAX);
        }
    }
#endif
    if (s_ssid[0] == '\0') {
        strncpy(s_ssid, CONFIG_WIFI_STA_SSID, WIFI_SSID_MAX);
        strncpy(s_psk, CONFIG_WIFI_STA_PSK, WIFI_PSK_MAX);
    }
}

int network_wifi_aic_set_credentials(const char *ssid, const char *psk)
{
    if (ssid == NULL || ssid[0] == '\0' || strlen(ssid) > WIFI_SSID_MAX ||
        (psk != NULL && strlen(psk) > WIFI_PSK_MAX)) {
        return -1;
    }
    strncpy(s_ssid, ssid, WIFI_SSID_MAX);
    s_ssid[WIFI_SSID_MAX] = '\0';
    strncpy(s_psk, psk ? psk : "", WIFI_PSK_MAX);
    s_psk[WIFI_PSK_MAX] = '\0';
#ifdef CONFIG_EASYFLASH4
    ef_set_env("wifi_ssid", s_ssid);
    ef_set_env("wifi_psk", s_psk);
    ef_save_env();
#endif
    IPRO_LOGI(LOG_TAG, "credentials stored for \"%s\"", s_ssid);
    return 0;
}

/* ------------------------------------------------------------------ */
/* Driver / link                                                       */
/* ------------------------------------------------------------------ */

static void sta_notify(uint8_t status, uint8_t *mac)
{
    (void)mac;
    /* Vendor callback on association state changes; the IP is the
     * authoritative "up" signal (DHCP finishes inside app_wlan_sta_start). */
    IPRO_LOGI(LOG_TAG, "STA link event %u", status);
    if (status == 0) {
        s_link_up = false;
    }
}

static void module_power_cycle(void)
{
    gpio_set_mode(WIFI_PWR_PIN, GPIO_OUTPUT_MODE);
    gpio_write(WIFI_PWR_PIN, !WIFI_PWR_ON_LEVEL);
    vTaskDelay(pdMS_TO_TICKS(WIFI_PWR_OFF_MS));
    gpio_write(WIFI_PWR_PIN, WIFI_PWR_ON_LEVEL);
    vTaskDelay(pdMS_TO_TICKS(WIFI_PWR_SETTLE_MS));
}

static bool driver_bringup(void)
{
    if (s_driver_up) {
        return true;
    }
    IPRO_LOGI(LOG_TAG, "AIC8800: power-cycle (GPIO%d on=%d) + SDIO probe", WIFI_PWR_PIN,
              WIFI_PWR_ON_LEVEL);
    module_power_cycle();
    if (aic_wifi_demo_test() != 0) {
        IPRO_LOGE(LOG_TAG, "AIC8800 driver init failed");
        return false;
    }
    (void)app_wlan_sta_conn_notify_reg(sta_notify);
    s_driver_up = true;
    IPRO_LOGI(LOG_TAG, "AIC8800 driver up");
    return true;
}

/* DHCP hands lwIP one DNS server (slot 0). On some networks that server
 * never answers Wi-Fi clients while the gateway and public
 * resolvers do, and lwIP only tries slot 1 after slot 0 exhausts its
 * retries. Keep the DHCP one first and add a public resolver behind it. */
/* The vendor netif glue brings the STA netif up with an address and gateway
 * but does not always leave it as lwIP's default netif, so anything off the
 * local subnet fails in ip4_route() with ERR_RTE (no route). Find the netif
 * that carries the STA address and make it the default. */
static void ensure_default_route(uint32_t ip)
{
    struct netif *n;
    struct netif *sta = NULL;

    LOCK_TCPIP_CORE();
    NETIF_FOREACH(n) {
        if (ip4_addr_get_u32(netif_ip4_addr(n)) == ip) {
            sta = n;
        }
    }
    if (sta != NULL) {
        if (!netif_is_link_up(sta)) {
            netif_set_link_up(sta);
        }
        if (netif_default != sta) {
            netif_set_default(sta);
        }
    }
    UNLOCK_TCPIP_CORE();
    if (sta != NULL) {
        IPRO_LOGI(LOG_TAG, "netif %c%c%d is default, gw %s", sta->name[0], sta->name[1],
                  sta->num, ipaddr_ntoa(netif_ip4_gw(sta)));
    } else {
        IPRO_LOGW(LOG_TAG, "no netif carries the STA address");
    }
}

#define WIFI_DNS_FALLBACK "8.8.8.8"
static void install_dns_fallback(void)
{
    ip_addr_t fb;

    if (ipaddr_aton(WIFI_DNS_FALLBACK, &fb) && ip_addr_isany(dns_getserver(1))) {
        dns_setserver(1, &fb);
    }
    IPRO_LOGI(LOG_TAG, "DNS: %s (dhcp), %s (fallback)", ipaddr_ntoa(dns_getserver(0)),
              ipaddr_ntoa(dns_getserver(1)));
}

static void wifi_task(void *arg)
{
    (void)arg;
    int backoff_s = 2;

    while (s_want_connect) {
        if (!driver_bringup()) {
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }
        load_credentials();
        if (s_ssid[0] == '\0') {
            IPRO_LOGW(LOG_TAG, "no Wi-Fi credentials: use `wifi_set <ssid> <psk>`");
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }
        IPRO_LOGI(LOG_TAG, "connecting to \"%s\"...", s_ssid);
        if (app_wlan_sta_start((uint8_t *)s_ssid, (uint8_t *)s_psk, WIFI_CONNECT_TO_MS) >= 0 &&
            app_wlan_sta_get_ip() != 0U) {
            uint32_t ip = app_wlan_sta_get_ip();
            uint8_t b[4];

            memcpy(b, &ip, sizeof(b));
            ensure_default_route(ip);
            install_dns_fallback();
            s_link_up = true;
            IPRO_LOGI(LOG_TAG, "connected, IP %u.%u.%u.%u", b[0], b[1], b[2], b[3]);
            break;
        }
        IPRO_LOGW(LOG_TAG, "connect failed, retry in %ds", backoff_s);
        vTaskDelay(pdMS_TO_TICKS(backoff_s * 1000));
        if (backoff_s < 30) {
            backoff_s *= 2;
        }
    }
    s_task = NULL;
    vTaskDelete(NULL);
}

static app_err_t wifi_start_task(void)
{
    if (s_task != NULL) {
        return APP_OK;
    }
    s_want_connect = true;
    if (xTaskCreate(wifi_task, "wifi_aic", WIFI_TASK_STACK, NULL, WIFI_TASK_PRIO, &s_task) !=
        pdPASS) {
        IPRO_LOGE(LOG_TAG, "cannot create Wi-Fi task");
        return APP_ERR_NOMEM;
    }
    return APP_OK;
}

/* ------------------------------------------------------------------ */
/* network_ops_t                                                       */
/* ------------------------------------------------------------------ */

static app_err_t wifi_init(void)
{
    /* Before the scheduler, like network_emac.c: the AIC netif glue, its
     * sockets and DHCP server all need the tcpip thread. */
    tcpip_init(NULL, NULL);
    IPRO_LOGI(LOG_TAG, "lwIP started (AIC8800 Wi-Fi backend)");
    return APP_OK;
}

static app_err_t wifi_connect(void)
{
    return wifi_start_task();
}

static app_err_t wifi_disconnect(void)
{
    s_want_connect = false;
    if (s_driver_up) {
        (void)app_wlan_sta_stop();
    }
    s_link_up = false;
    return APP_OK;
}

static int wifi_is_connected(void)
{
    return (s_link_up && app_wlan_sta_get_ip() != 0U) ? 1 : 0;
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
    (void)wifi_disconnect();
    vTaskDelay(pdMS_TO_TICKS(500));
    return wifi_start_task();
}

const network_ops_t network_wifi_aic_ops = {
    .name         = "wifi_aic",
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
    if (network_wifi_aic_set_credentials(argv[1], argc > 2 ? argv[2] : "") != 0) {
        printf("wifi_set: invalid ssid/psk\r\n");
        return -1;
    }
    printf("stored; reconnecting\r\n");
    (void)wifi_reconnect();
    return 0;
}

static int cmd_wifi_status(int argc, char **argv)
{
    uint32_t ip = s_driver_up ? app_wlan_sta_get_ip() : 0U;
    uint8_t b[4];

    (void)argc;
    (void)argv;
    memcpy(b, &ip, sizeof(b));
    printf("driver=%s link=%s ssid=\"%s\" ip=%u.%u.%u.%u\r\n",
           s_driver_up ? "up" : "down", s_link_up ? "up" : "down", s_ssid,
           b[0], b[1], b[2], b[3]);
    return 0;
}

static int cmd_wifi_dns(int argc, char **argv)
{
    ip_addr_t addr;
    const ip_addr_t *srv = dns_getserver(0);
    err_t err;

    printf("dns server[0]=%s\r\n", ipaddr_ntoa(srv));
    if (argc < 2) {
        return 0;
    }
    err = netconn_gethostbyname(argv[1], &addr);
    printf("%s -> %s (err %d)\r\n", argv[1], err == ERR_OK ? ipaddr_ntoa(&addr) : "?", (int)err);
    return err == ERR_OK ? 0 : -1;
}

static int cmd_wifi_dns_set(int argc, char **argv)
{
    ip_addr_t addr;

    if (argc < 2 || !ipaddr_aton(argv[1], &addr)) {
        printf("usage: wifi_dns_set <ipv4>\r\n");
        return -1;
    }
    dns_setserver(0, &addr);
    printf("dns server[0]=%s\r\n", ipaddr_ntoa(dns_getserver(0)));
    return 0;
}

static int cmd_wifi_tcp(int argc, char **argv)
{
    ip_addr_t addr;
    struct netconn *conn;
    err_t err;
    uint32_t t0;

    if (argc < 3 || !ipaddr_aton(argv[1], &addr)) {
        printf("usage: wifi_tcp <ipv4> <port>\r\n");
        return -1;
    }
    conn = netconn_new(NETCONN_TCP);
    if (conn == NULL) {
        printf("netconn_new failed\r\n");
        return -1;
    }
    t0 = xTaskGetTickCount();
    err = netconn_connect(conn, &addr, (uint16_t)atoi(argv[2]));
    printf("tcp %s:%s -> err %d in %lu ms\r\n", argv[1], argv[2], (int)err,
           (unsigned long)((xTaskGetTickCount() - t0) * portTICK_PERIOD_MS));
    netconn_close(conn);
    netconn_delete(conn);
    return err == ERR_OK ? 0 : -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_wifi_tcp, wifi_tcp, "wifi_tcp <ip> <port>: TCP connect test");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_dns_set, wifi_dns_set, "wifi_dns_set <ip>: override DNS server 0");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_dns, wifi_dns, "wifi_dns [host]: show DNS server / resolve a name");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_set, wifi_set, "wifi_set <ssid> [psk]: store credentials and reconnect");
SHELL_CMD_EXPORT_ALIAS(cmd_wifi_status, wifi_status, "AIC8800 Wi-Fi link status");
#endif
