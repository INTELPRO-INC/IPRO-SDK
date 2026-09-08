/*
 * esp32_wifi_at.c - ESP32 WiFi AT command client (STA mode)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp32_wifi_at.h"
#include "esp32_wifi_hw.h"
#include "at_client.h"

#include <string.h>
#include <stdio.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "hal_uart.h"
#include "ipro_log.h"

#define TAG "ESP32_WIFI"

/* ------------------------------------------------------------------ */
/* State                                                              */
/* ------------------------------------------------------------------ */

static at_client_t *s_at;
static volatile esp32_wifi_state_t s_state = ESP32_WIFI_STATE_OFF;
static esp32_wifi_event_cb_t s_event_cb;
static void *s_event_ctx;
static bool s_initialized;

/* ------------------------------------------------------------------ */
/* URC handlers                                                       */
/* ------------------------------------------------------------------ */

static void urc_wifi_connected(const char *line, void *ctx)
{
    (void)line; (void)ctx;
    s_state = ESP32_WIFI_STATE_CONNECTED;
    IPRO_LOGI(TAG, "URC: WIFI CONNECTED");
    if (s_event_cb)
        s_event_cb(ESP32_WIFI_STATE_CONNECTED, s_event_ctx);
}

static void urc_wifi_got_ip(const char *line, void *ctx)
{
    (void)line; (void)ctx;
    s_state = ESP32_WIFI_STATE_GOT_IP;
    IPRO_LOGI(TAG, "URC: WIFI GOT IP");
    if (s_event_cb)
        s_event_cb(ESP32_WIFI_STATE_GOT_IP, s_event_ctx);
}

static void urc_wifi_disconnect(const char *line, void *ctx)
{
    (void)line; (void)ctx;
    s_state = ESP32_WIFI_STATE_DISCONNECTED;
    IPRO_LOGW(TAG, "URC: WIFI DISCONNECT");
    if (s_event_cb)
        s_event_cb(ESP32_WIFI_STATE_DISCONNECTED, s_event_ctx);
}

static void urc_ready(const char *line, void *ctx)
{
    (void)line; (void)ctx;
    s_state = ESP32_WIFI_STATE_READY;
    IPRO_LOGI(TAG, "URC: ESP32 ready");
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

int esp32_wifi_init(void)
{
    if (s_initialized)
        return 0;

    char resp[128];
    int rc;

    IPRO_LOGI(TAG, "=== ESP32 WiFi Init ===");

    /* Step 1: UART GPIO + peripheral */
    esp32_hw_uart_init();

    /* Step 2: AT client instance */
    at_client_transport_t transport;
    at_client_uart_transport_init(&transport, CONFIG_ESP32_WIFI_UART_ID);

    at_client_cfg_t cfg = {
        .name        = "ESP32",
        .transport   = transport,
        .rx_buf_size = CONFIG_ESP32_WIFI_RX_BUF_SIZE,
        .task_stack  = CONFIG_ESP32_WIFI_TASK_STACK * sizeof(uint32_t),
        .task_prio   = CONFIG_ESP32_WIFI_TASK_PRIO,
    };
    rc = at_client_init(&s_at, &cfg);
    if (rc != 0) {
        IPRO_LOGE(TAG, "at_client_init failed: %d", rc);
        s_state = ESP32_WIFI_STATE_ERROR;
        return rc;
    }

    /* Step 3: Enable RX interrupts after callback is registered */
    esp32_hw_uart_enable_rx();

    /* Step 4: Register URC handlers */
    at_client_register_urc(s_at, "WIFI CONNECTED", urc_wifi_connected, NULL);
    at_client_register_urc(s_at, "WIFI GOT IP", urc_wifi_got_ip, NULL);
    at_client_register_urc(s_at, "WIFI DISCONNECT", urc_wifi_disconnect, NULL);
    at_client_register_urc(s_at, "ready", urc_ready, NULL);

    /* Step 5: Hardware reset if EN pin configured */
    esp32_hw_reset();

    /* Step 6: AT sync */
    rc = esp32_wifi_sync(5, 1000);
    if (rc != 0) {
        IPRO_LOGE(TAG, "AT sync failed");
        s_state = ESP32_WIFI_STATE_ERROR;
        return -1;
    }

    /* Step 7: Disable echo */
    at_client_send_cmd(s_at, "ATE0", resp, sizeof(resp), 1000);

    /* Step 8: STA mode */
    rc = at_client_send_cmd(s_at, "AT+CWMODE=1", resp, sizeof(resp), 2000);
    if (rc != 0)
        IPRO_LOGW(TAG, "Set STA mode failed (non-fatal)");

    s_state = ESP32_WIFI_STATE_READY;
    s_initialized = true;

    IPRO_LOGI(TAG, "=== ESP32 WiFi Ready ===");
    return 0;
}

void esp32_wifi_deinit(void)
{
    if (!s_initialized)
        return;

    at_client_deinit(s_at);
    s_at = NULL;
    uart_uninit(CONFIG_ESP32_WIFI_UART_ID);
    s_state = ESP32_WIFI_STATE_OFF;
    s_event_cb = NULL;
    s_event_ctx = NULL;
    s_initialized = false;
}

int esp32_wifi_sync(int retries, uint32_t timeout_ms)
{
    char resp[64];

    for (int i = 0; i < retries; i++) {
        int rc = at_client_send_cmd(s_at, "AT", resp, sizeof(resp), timeout_ms);
        if (rc == 0)
            return 0;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    return -1;
}

int esp32_wifi_reset(void)
{
    if (!s_initialized)
        return -1;

    char resp[128];
    int rc = at_client_send_cmd(s_at, "AT+RST", resp, sizeof(resp), 3000);

    /* Wait for ESP32 to reboot and send "ready" URC */
    vTaskDelay(pdMS_TO_TICKS(2000));
    return rc;
}

int esp32_wifi_set_mode(int mode)
{
    if (!s_initialized)
        return -1;

    char cmd[32];
    snprintf(cmd, sizeof(cmd), "AT+CWMODE=%d", mode);

    char resp[64];
    return at_client_send_cmd(s_at, cmd, resp, sizeof(resp), 2000);
}

int esp32_wifi_connect(const char *ssid, const char *password,
                       uint32_t timeout_ms)
{
    if (!s_initialized)
        return -1;

    char cmd[128];
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password);

    s_state = ESP32_WIFI_STATE_CONNECTING;

    char resp[256];
    int rc = at_client_send_cmd(s_at, cmd, resp, sizeof(resp),
                                timeout_ms ? timeout_ms : 20000);
    if (rc != 0) {
        s_state = ESP32_WIFI_STATE_ERROR;
        IPRO_LOGE(TAG, "CWJAP failed: %s", resp);
    }
    /* State updates come from URCs (WIFI CONNECTED -> WIFI GOT IP) */
    return rc;
}

int esp32_wifi_disconnect(void)
{
    if (!s_initialized)
        return -1;

    char resp[64];
    int rc = at_client_send_cmd(s_at, "AT+CWQAP", resp, sizeof(resp), 5000);
    if (rc == 0)
        s_state = ESP32_WIFI_STATE_DISCONNECTED;
    return rc;
}

int esp32_wifi_get_ip(esp32_wifi_ip_t *ip)
{
    if (!s_initialized || !ip)
        return -1;

    memset(ip, 0, sizeof(*ip));

    char resp[256];
    int rc = at_client_send_cmd(s_at, "AT+CIPSTA?", resp, sizeof(resp), 2000);
    if (rc != 0)
        return rc;

    /* Parse: +CIPSTA:ip:"x.x.x.x"\n+CIPSTA:gateway:"x.x.x.x"\n+CIPSTA:netmask:"x.x.x.x" */
    char *p;
    p = strstr(resp, "+CIPSTA:ip:\"");
    if (p) {
        p += strlen("+CIPSTA:ip:\"");
        int i = 0;
        while (*p && *p != '"' && i < 15)
            ip->ip[i++] = *p++;
    }
    p = strstr(resp, "+CIPSTA:gateway:\"");
    if (p) {
        p += strlen("+CIPSTA:gateway:\"");
        int i = 0;
        while (*p && *p != '"' && i < 15)
            ip->gateway[i++] = *p++;
    }
    p = strstr(resp, "+CIPSTA:netmask:\"");
    if (p) {
        p += strlen("+CIPSTA:netmask:\"");
        int i = 0;
        while (*p && *p != '"' && i < 15)
            ip->netmask[i++] = *p++;
    }

    return 0;
}

esp32_wifi_state_t esp32_wifi_get_state(void)
{
    return s_state;
}

int esp32_wifi_set_event_cb(esp32_wifi_event_cb_t cb, void *ctx)
{
    s_event_cb = cb;
    s_event_ctx = ctx;
    return 0;
}

at_client_t *esp32_wifi_get_at_client(void)
{
    return s_initialized ? s_at : NULL;
}

int esp32_wifi_at_cmd(const char *cmd, char *resp, uint16_t resp_size,
                      uint32_t timeout_ms)
{
    if (!s_initialized)
        return -1;

    return at_client_send_cmd(s_at, cmd, resp, resp_size, timeout_ms);
}

/* ------------------------------------------------------------------ */
/* Shell commands                                                     */
/* ------------------------------------------------------------------ */

#ifdef CONFIG_SHELL
#include "shell.h"

static int cmd_esp32_at(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: esp32_at <AT command>\r\n");
        printf("Example: esp32_at AT+GMR\r\n");
        return -1;
    }

    /* Concatenate all args as AT command */
    char cmd[128];
    int pos = 0;
    for (int i = 1; i < argc && pos < (int)sizeof(cmd) - 1; i++) {
        if (i > 1 && pos < (int)sizeof(cmd) - 1)
            cmd[pos++] = ' ';
        int len = strlen(argv[i]);
        if (pos + len >= (int)sizeof(cmd))
            len = sizeof(cmd) - 1 - pos;
        memcpy(cmd + pos, argv[i], len);
        pos += len;
    }
    cmd[pos] = '\0';

    char resp[256];
    printf(">> %s\r\n", cmd);
    int rc = esp32_wifi_at_cmd(cmd, resp, sizeof(resp), 5000);
    printf("<< [%s] %s", rc == 0 ? "OK" : (rc == -1 ? "ERROR" : "TIMEOUT"), resp);
    if (resp[0] && resp[strlen(resp) - 1] != '\n')
        printf("\r\n");
    return rc;
}

static int cmd_esp32_wifi_connect(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: esp32_wifi_connect <ssid> <password>\r\n");
        return -1;
    }
    int rc = esp32_wifi_connect(argv[1], argv[2], 20000);
    printf("connect: %s\r\n", rc == 0 ? "OK" : "FAIL");
    return rc;
}

static int cmd_esp32_wifi_status(int argc, char **argv)
{
    (void)argc; (void)argv;

    static const char *state_names[] = {
        [ESP32_WIFI_STATE_OFF]          = "OFF",
        [ESP32_WIFI_STATE_READY]        = "READY",
        [ESP32_WIFI_STATE_CONNECTING]   = "CONNECTING",
        [ESP32_WIFI_STATE_CONNECTED]    = "CONNECTED",
        [ESP32_WIFI_STATE_GOT_IP]       = "GOT_IP",
        [ESP32_WIFI_STATE_DISCONNECTED] = "DISCONNECTED",
        [ESP32_WIFI_STATE_ERROR]        = "ERROR",
    };

    esp32_wifi_state_t st = esp32_wifi_get_state();
    printf("state: %s (%d)\r\n", state_names[st], (int)st);

    esp32_wifi_ip_t ip;
    if (esp32_wifi_get_ip(&ip) == 0)
        printf("ip=%s gw=%s mask=%s\r\n", ip.ip, ip.gateway, ip.netmask);

    return 0;
}

static int cmd_esp32_wifi_init(int argc, char **argv)
{
    (void)argc; (void)argv;
    int rc = esp32_wifi_init();
    printf("init: %s (%d)\r\n", rc == 0 ? "OK" : "FAIL", rc);
    return rc;
}

SHELL_CMD_EXPORT_ALIAS(cmd_esp32_wifi_init, esp32_wifi_init, Initialize ESP32 WiFi module);
SHELL_CMD_EXPORT_ALIAS(cmd_esp32_at, esp32_at, Send AT command to ESP32);
SHELL_CMD_EXPORT_ALIAS(cmd_esp32_wifi_connect, esp32_wifi_connect, Connect to WiFi AP);
SHELL_CMD_EXPORT_ALIAS(cmd_esp32_wifi_status, esp32_wifi_status, Show WiFi status);

#endif /* CONFIG_SHELL */
