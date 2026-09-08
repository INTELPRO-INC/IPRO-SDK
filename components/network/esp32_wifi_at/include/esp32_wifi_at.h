/*
 * esp32_wifi_at.h - ESP32 WiFi AT command client (STA mode)
 *
 * Controls an external ESP32 module running ESP-AT firmware via UART.
 * Provides WiFi STA connect/disconnect, IP query, event callbacks,
 * and shell commands.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP32_WIFI_AT_H__
#define __ESP32_WIFI_AT_H__

#include <stdint.h>
#include <stdbool.h>

#include "at_client.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP32_WIFI_STATE_OFF = 0,
    ESP32_WIFI_STATE_READY,
    ESP32_WIFI_STATE_CONNECTING,
    ESP32_WIFI_STATE_CONNECTED,
    ESP32_WIFI_STATE_GOT_IP,
    ESP32_WIFI_STATE_DISCONNECTED,
    ESP32_WIFI_STATE_ERROR,
} esp32_wifi_state_t;

typedef struct {
    char ip[16];
    char gateway[16];
    char netmask[16];
} esp32_wifi_ip_t;

typedef void (*esp32_wifi_event_cb_t)(esp32_wifi_state_t state, void *ctx);

/**
 * @brief Initialize ESP32 WiFi module.
 *
 * Sets up UART, AT client, URC handlers, resets module if EN pin
 * is configured, syncs AT, disables echo, and sets STA mode.
 *
 * @return 0 on success, negative on error
 */
int  esp32_wifi_init(void);

/**
 * @brief Deinitialize ESP32 WiFi module.
 *
 * Tears down AT client and UART.
 */
void esp32_wifi_deinit(void);

/**
 * @brief AT sync - verify ESP32 module is responsive.
 *
 * Sends "AT" and expects "OK" response.
 *
 * @param retries    Number of retry attempts
 * @param timeout_ms Timeout per attempt in milliseconds
 * @return 0 on success, -1 on failure
 */
int  esp32_wifi_sync(int retries, uint32_t timeout_ms);

/**
 * @brief Reset ESP32 module (AT+RST).
 * @return 0 on success, negative on error
 */
int  esp32_wifi_reset(void);

/**
 * @brief Set WiFi mode.
 * @param mode 1=STA, 2=AP, 3=STA+AP
 * @return 0 on success, negative on error
 */
int  esp32_wifi_set_mode(int mode);

/**
 * @brief Connect to WiFi AP.
 *
 * Sends AT+CWJAP="ssid","password". State updates come from URCs.
 *
 * @param ssid       SSID string
 * @param password   Password string
 * @param timeout_ms Timeout in ms (0 = default 20s)
 * @return 0 on success, negative on error
 */
int  esp32_wifi_connect(const char *ssid, const char *password,
                        uint32_t timeout_ms);

/**
 * @brief Disconnect from WiFi AP.
 * @return 0 on success, negative on error
 */
int  esp32_wifi_disconnect(void);

/**
 * @brief Query IP address from ESP32.
 * @param ip  Output struct for ip/gateway/netmask
 * @return 0 on success, negative on error
 */
int  esp32_wifi_get_ip(esp32_wifi_ip_t *ip);

/**
 * @brief Query current connection state.
 * @return Current wifi state
 */
esp32_wifi_state_t esp32_wifi_get_state(void);

/**
 * @brief Register event callback.
 *
 * Single callback for all events: CONNECTED, GOT_IP, DISCONNECTED.
 *
 * @param cb   Callback function
 * @param ctx  User context passed to callback
 * @return 0 on success
 */
int  esp32_wifi_set_event_cb(esp32_wifi_event_cb_t cb, void *ctx);

/**
 * @brief Raw AT command pass-through.
 *
 * For advanced use or shell debugging.
 *
 * @param cmd       AT command string (without trailing \r\n)
 * @param resp      Buffer for response
 * @param resp_size Size of resp buffer
 * @param timeout_ms Timeout in milliseconds
 * @return 0 on OK, -1 on ERROR, -2 on timeout
 */
int  esp32_wifi_at_cmd(const char *cmd, char *resp, uint16_t resp_size,
                       uint32_t timeout_ms);

/**
 * @brief Get the underlying at_client instance.
 *
 * Exposed so applications can register custom URCs (e.g. +IPD, +MQTTSUBRECV)
 * and perform raw transport writes for commands that need prompt-then-data
 * handshakes (e.g. AT+CIPSEND).
 *
 * @return at_client_t* if initialized, NULL otherwise
 */
at_client_t *esp32_wifi_get_at_client(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP32_WIFI_AT_H__ */
