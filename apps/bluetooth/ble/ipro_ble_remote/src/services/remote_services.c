/**
 * @file remote_services.c
 * @brief BLE Remote Control Services Implementation
 *
 * Implements GATT services for mobile phone interaction
 * Uses Zephyr blestack directly (without ipro_ble_lib)
 *
 * Copyright (C) IPRO SDK 2025
 */

#include "remote_services.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>

#include <stdint.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_TAG "REMOTE_SVC"

/*
 * UUID DEFINITIONS
 ****************************************************************************************
 */

/* LED Service UUID: 0xFF10 */
#define BT_UUID_LED_SVC_VAL        BT_UUID_16_ENCODE(LED_SVC_UUID16)
#define BT_UUID_LED_SVC            BT_UUID_DECLARE_16(LED_SVC_UUID16)
#define BT_UUID_LED_STATE          BT_UUID_DECLARE_16(LED_STATE_CHAR_UUID16)
#define BT_UUID_LED_BRIGHT         BT_UUID_DECLARE_16(LED_BRIGHT_CHAR_UUID16)
#define BT_UUID_LED_MODE           BT_UUID_DECLARE_16(LED_MODE_CHAR_UUID16)

/* Button Service UUID: 0xFF20 */
#define BT_UUID_BTN_SVC_VAL        BT_UUID_16_ENCODE(BTN_SVC_UUID16)
#define BT_UUID_BTN_SVC            BT_UUID_DECLARE_16(BTN_SVC_UUID16)
#define BT_UUID_BTN_STATE          BT_UUID_DECLARE_16(BTN_STATE_CHAR_UUID16)
#define BT_UUID_BTN_COUNT          BT_UUID_DECLARE_16(BTN_COUNT_CHAR_UUID16)

/* Environment Service UUID: 0xFF30 */
#define BT_UUID_ENV_SVC_VAL        BT_UUID_16_ENCODE(ENV_SVC_UUID16)
#define BT_UUID_ENV_SVC            BT_UUID_DECLARE_16(ENV_SVC_UUID16)
#define BT_UUID_ENV_TEMP           BT_UUID_DECLARE_16(ENV_TEMP_CHAR_UUID16)
#define BT_UUID_ENV_HUMID          BT_UUID_DECLARE_16(ENV_HUMID_CHAR_UUID16)

/* UART Service UUID: 0xFF40 */
#define BT_UUID_UART_SVC_VAL       BT_UUID_16_ENCODE(UART_SVC_UUID16)
#define BT_UUID_UART_SVC           BT_UUID_DECLARE_16(UART_SVC_UUID16)
#define BT_UUID_UART_TX            BT_UUID_DECLARE_16(UART_TX_CHAR_UUID16)
#define BT_UUID_UART_RX            BT_UUID_DECLARE_16(UART_RX_CHAR_UUID16)

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static struct {
    remote_event_callback_t callback;
    void *user_data;
    remote_svc_handles_t handles;
    led_state_t led_state;
    btn_state_t btn_state;
    env_data_t env_data;
    bool initialized;
} g_ctx = {0};

/*
 * LED SERVICE CALLBACKS
 ****************************************************************************************
 */

static ssize_t led_state_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    uint8_t value = g_ctx.led_state.on ? 1 : 0;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

static ssize_t led_state_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (offset != 0 || len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    const uint8_t *data = buf;
    g_ctx.led_state.on = (data[0] != 0);

    IPRO_LOGI(LOG_TAG, "[LED] State: %s", g_ctx.led_state.on ? "ON" : "OFF");

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = REMOTE_EVENT_LED_STATE_CHANGED,
            .data.led = g_ctx.led_state
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }

    return len;
}

static ssize_t led_bright_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                            &g_ctx.led_state.brightness, sizeof(g_ctx.led_state.brightness));
}

static ssize_t led_bright_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (offset != 0 || len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    const uint8_t *data = buf;
    g_ctx.led_state.brightness = data[0];
    if (g_ctx.led_state.brightness > 100) {
        g_ctx.led_state.brightness = 100;
    }

    IPRO_LOGI(LOG_TAG, "[LED] Brightness: %d%%", g_ctx.led_state.brightness);

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = REMOTE_EVENT_LED_BRIGHT_CHANGED,
            .data.led = g_ctx.led_state
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }

    return len;
}

static ssize_t led_mode_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    uint8_t value = (uint8_t)g_ctx.led_state.mode;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

static ssize_t led_mode_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (offset != 0 || len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    const uint8_t *data = buf;
    g_ctx.led_state.mode = (led_mode_t)data[0];

    IPRO_LOGI(LOG_TAG, "[LED] Mode: %d", g_ctx.led_state.mode);

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = REMOTE_EVENT_LED_MODE_CHANGED,
            .data.led = g_ctx.led_state
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }

    return len;
}

/*
 * BUTTON SERVICE CALLBACKS
 ****************************************************************************************
 */

static ssize_t btn_state_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    uint8_t value = (uint8_t)g_ctx.btn_state.event;
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value, sizeof(value));
}

static ssize_t btn_count_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                            &g_ctx.btn_state.press_count, sizeof(g_ctx.btn_state.press_count));
}

static void btn_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ctx.handles.btn_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[BTN] Notifications %s", g_ctx.handles.btn_notify_enabled ? "enabled" : "disabled");

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = g_ctx.handles.btn_notify_enabled ? REMOTE_EVENT_BTN_NOTIFY_ENABLED : REMOTE_EVENT_BTN_NOTIFY_DISABLED
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }
}

/*
 * ENVIRONMENT SERVICE CALLBACKS
 ****************************************************************************************
 */

static ssize_t env_temp_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                            &g_ctx.env_data.temperature, sizeof(g_ctx.env_data.temperature));
}

static ssize_t env_humid_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                            &g_ctx.env_data.humidity, sizeof(g_ctx.env_data.humidity));
}

static void temp_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ctx.handles.temp_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[ENV] Temp notifications %s", g_ctx.handles.temp_notify_enabled ? "enabled" : "disabled");

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = g_ctx.handles.temp_notify_enabled ? REMOTE_EVENT_ENV_TEMP_NOTIFY_ENABLED : REMOTE_EVENT_ENV_TEMP_NOTIFY_DISABLED
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }
}

static void humid_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ctx.handles.humid_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[ENV] Humidity notifications %s", g_ctx.handles.humid_notify_enabled ? "enabled" : "disabled");

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = g_ctx.handles.humid_notify_enabled ? REMOTE_EVENT_ENV_HUMID_NOTIFY_ENABLED : REMOTE_EVENT_ENV_HUMID_NOTIFY_DISABLED
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }
}

/*
 * UART SERVICE CALLBACKS
 ****************************************************************************************
 */

static ssize_t uart_tx_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            void *buf, uint16_t len, uint16_t offset)
{
    /* TX characteristic has no persistent value - return empty */
    return 0;
}

static ssize_t uart_rx_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len == 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    IPRO_LOGI(LOG_TAG, "[UART] RX: %d bytes", len);

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = REMOTE_EVENT_UART_RX_DATA,
            .data.uart.data = (uint8_t *)buf,
            .data.uart.length = len
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }

    return len;
}

static void uart_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ctx.handles.uart_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[UART] TX notifications %s", g_ctx.handles.uart_notify_enabled ? "enabled" : "disabled");

    if (g_ctx.callback) {
        remote_event_data_t evt = {
            .event = g_ctx.handles.uart_notify_enabled ? REMOTE_EVENT_UART_TX_ENABLED : REMOTE_EVENT_UART_TX_DISABLED
        };
        g_ctx.callback(&evt, g_ctx.user_data);
    }
}

/*
 * SERVICE DEFINITIONS (Zephyr style)
 ****************************************************************************************
 */

/* LED Service */
BT_GATT_SERVICE_DEFINE(led_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_LED_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_LED_STATE,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           led_state_read, led_state_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_LED_BRIGHT,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           led_bright_read, led_bright_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_LED_MODE,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           led_mode_read, led_mode_write, NULL),
);

/* Button Service */
BT_GATT_SERVICE_DEFINE(btn_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_BTN_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_BTN_STATE,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           btn_state_read, NULL, NULL),
    BT_GATT_CCC(btn_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_BTN_COUNT,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           btn_count_read, NULL, NULL),
);

/* Environment Service */
BT_GATT_SERVICE_DEFINE(env_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_ENV_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_ENV_TEMP,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           env_temp_read, NULL, NULL),
    BT_GATT_CCC(temp_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_ENV_HUMID,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           env_humid_read, NULL, NULL),
    BT_GATT_CCC(humid_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/* UART Service */
BT_GATT_SERVICE_DEFINE(uart_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_UART_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_UART_TX,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           uart_tx_read, NULL, NULL),
    BT_GATT_CCC(uart_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_UART_RX,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, uart_rx_write, NULL),
);

/*
 * API IMPLEMENTATION
 ****************************************************************************************
 */

int remote_services_init(remote_event_callback_t callback, void *user_data)
{
    if (g_ctx.initialized) {
        return -1;
    }

    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.callback = callback;
    g_ctx.user_data = user_data;

    /* Initialize default values */
    g_ctx.led_state.on = false;
    g_ctx.led_state.brightness = 100;
    g_ctx.led_state.mode = LED_MODE_OFF;

    g_ctx.btn_state.event = BTN_EVENT_RELEASED;
    g_ctx.btn_state.press_count = 0;

    g_ctx.env_data.temperature = 2500;  /* 25.00C */
    g_ctx.env_data.humidity = 50;       /* 50% */

    /* Services are statically defined using BT_GATT_SERVICE_DEFINE,
     * so no runtime registration needed. Just log that we're ready. */
    IPRO_LOGI(LOG_TAG, "[BLE] Services initialized (static GATT registration)");

    g_ctx.initialized = true;
    return 0;
}

int remote_services_deinit(void)
{
    if (!g_ctx.initialized) {
        return -1;
    }

    g_ctx.initialized = false;
    return 0;
}

const remote_svc_handles_t *remote_services_get_handles(void)
{
    return &g_ctx.handles;
}

int remote_get_led_state(led_state_t *state)
{
    if (!g_ctx.initialized || state == NULL) {
        return -1;
    }
    *state = g_ctx.led_state;
    return 0;
}

int remote_set_led_state(const led_state_t *state)
{
    if (!g_ctx.initialized || state == NULL) {
        return -1;
    }
    g_ctx.led_state = *state;
    return 0;
}

int remote_notify_button(struct bt_conn *conn, btn_event_t event)
{
    if (!g_ctx.initialized || !g_ctx.handles.btn_notify_enabled) {
        return -1;
    }

    g_ctx.btn_state.event = event;
    if (event == BTN_EVENT_PRESSED) {
        g_ctx.btn_state.press_count++;
    }

    uint8_t value = (uint8_t)event;

    /* Find the button state characteristic attribute (index 1 in btn_svc) */
    return bt_gatt_notify(conn, &attr_btn_svc[1], &value, sizeof(value));
}

int remote_notify_environment(struct bt_conn *conn, const env_data_t *data)
{
    if (!g_ctx.initialized || data == NULL) {
        return -1;
    }

    g_ctx.env_data = *data;
    int ret = 0;

    if (g_ctx.handles.temp_notify_enabled) {
        /* Temperature characteristic is at index 1 in env_svc */
        ret = bt_gatt_notify(conn, &attr_env_svc[1], &data->temperature, sizeof(data->temperature));
        if (ret < 0) {
            return ret;
        }
    }

    if (g_ctx.handles.humid_notify_enabled) {
        /* Humidity characteristic is at index 4 in env_svc (after temp + CCC) */
        ret = bt_gatt_notify(conn, &attr_env_svc[4], &data->humidity, sizeof(data->humidity));
    }

    return ret;
}

int remote_uart_send(struct bt_conn *conn, const uint8_t *data, uint16_t length)
{
    if (!g_ctx.initialized || !g_ctx.handles.uart_notify_enabled) {
        return -1;
    }

    if (data == NULL || length == 0) {
        return -1;
    }

    /* TX characteristic is at index 1 in uart_svc */
    return bt_gatt_notify(conn, &attr_uart_svc[1], data, length);
}

int remote_uart_send_string(struct bt_conn *conn, const char *str)
{
    if (str == NULL) {
        return -1;
    }
    return remote_uart_send(conn, (const uint8_t*)str, strlen(str));
}
