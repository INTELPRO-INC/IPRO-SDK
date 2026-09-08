/**
 * @file ble_demo_app.c
 * @brief BLE Peripheral and Central demo using Zephyr blestack API
 *
 * Migrated from RivieraWaves API to Zephyr Bluetooth API
 *
 * Features:
 * - Peripheral: Advertise and accept connections (auto-restart after disconnect)
 * - Central: Scan and connect to devices (auto-connect to IPRO7_DEMO)
 * - Both roles can run simultaneously
 *
 * Architecture:
 * 1. Use bt_* Zephyr API instead of gapm_/gapc_
 * 2. Register callbacks for events
 * 3. Simple state machine for both peripheral and central
 *
 * Copyright (C) IPRO SDK 2026
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ble_demo.h"
#include "shell.h"
#include "ipro_log.h"

// Zephyr Bluetooth Stack API
#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/gap.h>

#define TAG "BLE_APP"

/*
 * CONFIGURATION
 ****************************************************************************************
 */

#define DEVICE_NAME         "IPRO7_DEMO"
#define APP_ADV_INTV_MIN    160     // 100ms (units of 0.625ms)
#define APP_ADV_INTV_MAX    320     // 200ms

// Central role configuration
#define TARGET_DEVICE_NAME  "IPRO7_DEMO"  // Auto-connect to peripheral with this name
#define SCAN_DURATION       1000          // Scan duration in ms
#define MAX_SCAN_RESULTS    20
#define AUTO_CONNECT        1             // Automatically connect to target device

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

typedef enum {
    APP_STATE_INIT,
    APP_STATE_READY,
    APP_STATE_ADVERTISING,          // Peripheral is advertising
    APP_STATE_SCANNING,             // Central is scanning
    APP_STATE_CONNECTING,           // Central is connecting
    APP_STATE_CONNECTED,            // Either peripheral or central connected
} app_state_t;

typedef struct {
    bt_addr_le_t addr;
    int8_t rssi;
    char name[32];
    bool has_name;
} scan_result_t;

/*
 * APPLICATION ENVIRONMENT
 ****************************************************************************************
 */

static struct {
    app_state_t state;
    bool peripheral_enabled;
    bool central_enabled;

    // Connection info
    struct bt_conn *conn;
    bool is_peripheral;  // True if acting as peripheral, false if central

    // Scan results (for central)
    scan_result_t scan_results[MAX_SCAN_RESULTS];
    uint8_t scan_count;

    // Task handle for FSM notifications
    TaskHandle_t fsm_task_handle;
} app_env;

/*
 * ADVERTISING DATA
 ****************************************************************************************
 */

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, sizeof(DEVICE_NAME) - 1),
};

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static void start_advertising(void);
static void start_scan(void);

/*
 * CONNECTION CALLBACKS
 ****************************************************************************************
 */

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    if (err) {
        IPRO_LOGE(TAG, "Connection failed (err 0x%02x)", err);

        // Restart advertising or scan based on role
        if (app_env.peripheral_enabled) {
            start_advertising();
        }
        return;
    }

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(TAG, "Connected: %s", addr_str);

    app_env.conn = bt_conn_ref(conn);
    app_env.state = APP_STATE_CONNECTED;

    // Determine if we're acting as peripheral or central
    // (Note: In Zephyr, we can check connection type/role if needed)
    app_env.is_peripheral = app_env.peripheral_enabled;
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    char addr_str[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(TAG, "Disconnected: %s (reason 0x%02x)", addr_str, reason);

    if (app_env.conn) {
        bt_conn_unref(app_env.conn);
        app_env.conn = NULL;
    }

    app_env.state = APP_STATE_READY;

    // Auto-restart advertising or scanning based on original role
    if (app_env.is_peripheral && app_env.peripheral_enabled) {
        IPRO_LOGI(TAG, "Auto-restarting advertising...", "");
        start_advertising();
    } else if (!app_env.is_peripheral && app_env.central_enabled) {
        IPRO_LOGI(TAG, "Central mode - ready for next scan", "");
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/*
 * PERIPHERAL FUNCTIONS
 ****************************************************************************************
 */

static void start_advertising(void)
{
    int err;
    struct bt_le_adv_param adv_param = {
        .options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME,
        .interval_min = APP_ADV_INTV_MIN,
        .interval_max = APP_ADV_INTV_MAX,
    };

    err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        IPRO_LOGE(TAG, "Advertising failed to start (err %d)", err);
        return;
    }

    IPRO_LOGI(TAG, "Advertising started as \"%s\"", DEVICE_NAME);
    app_env.state = APP_STATE_ADVERTISING;
}

int ble_demo_start_peripheral(void)
{
    if (app_env.state == APP_STATE_INIT) {
        IPRO_LOGE(TAG, "BLE not initialized", "");
        return -1;
    }

    if (app_env.state == APP_STATE_ADVERTISING) {
        IPRO_LOGW(TAG, "Already advertising", "");
        return 0;
    }

    if (app_env.state == APP_STATE_CONNECTED) {
        IPRO_LOGW(TAG, "Already connected", "");
        return 0;
    }

    app_env.peripheral_enabled = true;
    start_advertising();

    return 0;
}

/*
 * CENTRAL FUNCTIONS
 ****************************************************************************************
 */

static bool device_found_cb(struct bt_data *data, void *user_data)
{
    bt_addr_le_t *addr = user_data;

    if (data->type == BT_DATA_NAME_COMPLETE || data->type == BT_DATA_NAME_SHORTENED) {
        uint8_t len = MIN(data->data_len, sizeof(app_env.scan_results[0].name) - 1);

        if (app_env.scan_count < MAX_SCAN_RESULTS) {
            memcpy(&app_env.scan_results[app_env.scan_count].addr, addr, sizeof(bt_addr_le_t));
            memcpy(app_env.scan_results[app_env.scan_count].name, data->data, len);
            app_env.scan_results[app_env.scan_count].name[len] = '\0';
            app_env.scan_results[app_env.scan_count].has_name = true;

            // Print the found device
            char addr_str[BT_ADDR_LE_STR_LEN];
            bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
            IPRO_LOGI(TAG, "[%d] %s (%s)",
                     app_env.scan_count + 1,
                     app_env.scan_results[app_env.scan_count].name,
                     addr_str);

            app_env.scan_count++;
        }
    }

    return true;
}

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                    struct net_buf_simple *buf)
{
    // Parse advertising data to find device name
    bt_data_parse(buf, device_found_cb, (void *)addr);
}

static void start_scan(void)
{
    int err;
    struct bt_le_scan_param scan_param = {
        .type = BT_LE_SCAN_TYPE_ACTIVE,
        .filter_dup = 0,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,
        .window = BT_GAP_SCAN_FAST_WINDOW,
    };

    app_env.scan_count = 0;
    memset(app_env.scan_results, 0, sizeof(app_env.scan_results));

    err = bt_le_scan_start(&scan_param, scan_cb);
    if (err) {
        IPRO_LOGE(TAG, "Scanning failed to start (err %d)", err);
        return;
    }

    IPRO_LOGI(TAG, "Scanning started for %d ms...", SCAN_DURATION);
    app_env.state = APP_STATE_SCANNING;

    // Stop scan after duration
    vTaskDelay(pdMS_TO_TICKS(SCAN_DURATION));
    bt_le_scan_stop();

    IPRO_LOGI(TAG, "Scan complete. Found %d devices.", app_env.scan_count);

    #if AUTO_CONNECT
    // Auto-connect to target device if found
    for (uint8_t i = 0; i < app_env.scan_count; i++) {
        if (strcmp(app_env.scan_results[i].name, TARGET_DEVICE_NAME) == 0) {
            IPRO_LOGI(TAG, "Auto-connecting to %s...", TARGET_DEVICE_NAME);
            ble_central_connect(i + 1);  // 1-based index
            return;
        }
    }
    IPRO_LOGW(TAG, "Target device \"%s\" not found", TARGET_DEVICE_NAME);
    #endif

    app_env.state = APP_STATE_READY;
}

void ble_central_scan(void)
{
    if (app_env.state == APP_STATE_INIT) {
        IPRO_LOGE(TAG, "BLE not initialized", "");
        return;
    }

    if (app_env.state == APP_STATE_CONNECTED) {
        IPRO_LOGW(TAG, "Already connected. Disconnect first.", "");
        return;
    }

    app_env.central_enabled = true;
    start_scan();
}

void ble_central_list_devices(void)
{
    if (app_env.scan_count == 0) {
        IPRO_LOGI(TAG, "No devices found. Run ble_central first.", "");
        return;
    }

    IPRO_LOGI(TAG, "=== Scanned Devices (%d) ===", app_env.scan_count);
    for (uint8_t i = 0; i < app_env.scan_count; i++) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&app_env.scan_results[i].addr, addr_str, sizeof(addr_str));

        if (app_env.scan_results[i].has_name) {
            IPRO_LOGI(TAG, "[%d] %s (%s)",
                     i + 1,
                     app_env.scan_results[i].name,
                     addr_str);
        } else {
            IPRO_LOGI(TAG, "[%d] (Unknown) (%s)", i + 1, addr_str);
        }
    }
}

int ble_central_connect(uint8_t device_index)
{
    struct bt_conn *conn;

    if (device_index == 0 || device_index > app_env.scan_count) {
        IPRO_LOGE(TAG, "Invalid device index %d (valid: 1-%d)", device_index, app_env.scan_count);
        return -1;
    }

    if (app_env.state == APP_STATE_CONNECTED) {
        IPRO_LOGW(TAG, "Already connected. Disconnect first.", "");
        return -1;
    }

    uint8_t idx = device_index - 1;  // Convert to 0-based

    struct bt_le_conn_param conn_param = {
        .interval_min = BT_GAP_INIT_CONN_INT_MIN,
        .interval_max = BT_GAP_INIT_CONN_INT_MAX,
        .latency = 0,
        .timeout = 400,
    };

    IPRO_LOGI(TAG, "Connecting to %s...", app_env.scan_results[idx].name);

    conn = bt_conn_create_le(&app_env.scan_results[idx].addr, &conn_param);
    if (!conn) {
        IPRO_LOGE(TAG, "Connection failed", "");
        return -1;
    }

    app_env.state = APP_STATE_CONNECTING;
    app_env.is_peripheral = false;

    // Connection callbacks will handle the rest
    bt_conn_unref(conn);  // Release our reference

    return 0;
}

int ble_central_disconnect(void)
{
    if (!app_env.conn) {
        IPRO_LOGW(TAG, "Not connected", "");
        return -1;
    }

    IPRO_LOGI(TAG, "Disconnecting...", "");
    bt_conn_disconnect(app_env.conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);

    return 0;
}

/*
 * FSM (Finite State Machine) - For periodic processing
 ****************************************************************************************
 */

void app_process_fsm(void)
{
    // Wait for notification with 100ms timeout
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));

    // Process state machine
    switch (app_env.state) {
        case APP_STATE_INIT:
        case APP_STATE_READY:
        case APP_STATE_ADVERTISING:
        case APP_STATE_SCANNING:
        case APP_STATE_CONNECTING:
        case APP_STATE_CONNECTED:
            // Nothing to do in these states
            break;
    }
}

void app_set_fsm_task_handle(TaskHandle_t task_handle)
{
    app_env.fsm_task_handle = task_handle;
}

/*
 * INITIALIZATION
 ****************************************************************************************
 */

void app_init(void)
{
    int err;

    IPRO_LOGI(TAG, "Initializing BLE application...", "");

    memset(&app_env, 0, sizeof(app_env));
    app_env.state = APP_STATE_INIT;

    // Enable Bluetooth
    err = bt_enable(NULL);
    if (err) {
        IPRO_LOGE(TAG, "Bluetooth init failed (err %d)", err);
        return;
    }

    IPRO_LOGI(TAG, "Bluetooth initialized", "");

    // Register connection callbacks
    bt_conn_cb_register(&conn_callbacks);

    app_env.state = APP_STATE_READY;

    IPRO_LOGI(TAG, "BLE application ready", "");
    IPRO_LOGI(TAG, "Use 'ble_peripheral' or 'ble_central' to start", "");
}
