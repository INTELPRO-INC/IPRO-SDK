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
 * @file main.c
 * @brief BLE HID Keyboard Demo Application (blestack host)
 *
 * Uses Zephyr-based blestack host APIs directly.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

#include "ipro_log.h"
#include "freertos_app_hooks.h"

/* Blestack headers */
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <hci_driver_ipro_ble_ctlr.h>
#include <ipro_ble_controller.h>
#include <ble_lib_api.h>

#include "hid_keyboard_service.h"

#define LOG_TAG "HID_MAIN"

/*
 * Device configuration
 */
#define DEVICE_NAME     "IPRO HID Keyboard"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/*
 * Application state
 */
static struct bt_conn *g_conn;
static bool g_connected;

/*
 * Advertising data
 */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0x12, 0x18),               /* HID Service UUID */
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0xC1, 0x03),           /* Keyboard */
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static int start_advertising(void)
{
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = 48,   /* 30ms */
        .interval_max = 96,   /* 60ms */
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Advertising failed: %d", err);
    } else {
        IPRO_LOGI(LOG_TAG, "Advertising as '%s'", DEVICE_NAME);
    }
    return err;
}

/*
 * Connection callbacks
 */
static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err 0x%02x)", err);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Connected: %s", addr_str);

    g_conn = bt_conn_ref(conn);
    g_connected = true;
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(LOG_TAG, "Disconnected (reason 0x%02x)", reason);

    if (g_conn) {
        bt_conn_unref(g_conn);
        g_conn = NULL;
    }
    g_connected = false;

    /* Auto-restart advertising */
    vTaskDelay(pdMS_TO_TICKS(500));
    start_advertising();
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/*
 * SMP Authentication callbacks
 */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    IPRO_LOGI(LOG_TAG, "[SMP] Passkey: %06u", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    IPRO_LOGI(LOG_TAG, "[SMP] Confirm passkey: %06u", passkey);
    bt_conn_auth_passkey_confirm(conn);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing confirm");
    bt_conn_auth_pairing_confirm(conn);
}

static void auth_cancel(struct bt_conn *conn)
{
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing cancelled");
}

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing complete, bonded=%d", bonded);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    IPRO_LOGE(LOG_TAG, "[SMP] Pairing failed, reason=%d", reason);
}

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};

/* Zephyr 4.x moved the pairing outcome callbacks out of bt_conn_auth_cb into
 * their own registration, so more than one listener can observe them. */
static struct bt_conn_auth_info_cb auth_info_callbacks = {
    .pairing_complete = auth_pairing_complete,
    .pairing_failed = auth_pairing_failed,
};

/*
 * BLE ready callback
 */
static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "BLE init failed: %d", err);
        return;
    }

    IPRO_LOGI(LOG_TAG, "BLE initialized");

    bt_set_name(DEVICE_NAME);
    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_callbacks);
    bt_conn_auth_info_cb_register(&auth_info_callbacks);

    /* Register HID keyboard GATT service */
    err = hid_keyboard_service_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HID service init failed: %d", err);
        return;
    }

    /* Start advertising */
    start_advertising();

    IPRO_LOGI(LOG_TAG, "========================================");
    IPRO_LOGI(LOG_TAG, "BLE HID Keyboard Ready");
    IPRO_LOGI(LOG_TAG, "Device: %s", DEVICE_NAME);
    IPRO_LOGI(LOG_TAG, "========================================");
}

/*
 * BLE stack callback (required by controller)
 */
void app_init(void)
{
    IPRO_LOGI(LOG_TAG, "BLE controller initialized");
}

/*
 * HID Keyboard application task
 */
static void hid_keyboard_task(void *arg)
{
    (void)arg;

    IPRO_LOGI(LOG_TAG, "HID Keyboard task started");

    /* Initialize BLE controller */
    IPRO_LOGI(LOG_TAG, "Initializing BLE controller...");
    ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Initialize HCI driver */
    IPRO_LOGI(LOG_TAG, "Initializing HCI driver...");
    int err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", err);
        vTaskDelete(NULL);
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Initialize BLE stack */
    IPRO_LOGI(LOG_TAG, "Initializing BLE stack...");
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_enable failed: %d", err);
        vTaskDelete(NULL);
        return;
    }

    /* Main loop - periodic status */
    uint32_t counter = 0;
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        counter++;

        if (counter % 12 == 0) {
            IPRO_LOGI(LOG_TAG, "Status: connected=%d, notify=%d",
                      g_connected, hid_keyboard_is_notify_enabled());
        }
    }
}

/*
 * Shell commands
 */
#ifdef CONFIG_SHELL
static int cmd_hid_key(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: hid_key <string>\r\n");
        return -1;
    }

    if (!g_connected) {
        printf("Not connected\r\n");
        return -1;
    }

    if (!hid_keyboard_is_notify_enabled()) {
        printf("Notifications not enabled\r\n");
        return -1;
    }

    int ret = hid_keyboard_send_string(g_conn, argv[1], 50);
    if (ret == 0) {
        printf("Sent: %s\r\n", argv[1]);
    } else {
        printf("Failed: %d\r\n", ret);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hid_key, hid_key, Send keyboard input);

static int cmd_hid_status(int argc, char **argv)
{
    (void)argc; (void)argv;

    printf("HID Keyboard Status:\r\n");
    printf("  Connected: %s\r\n", g_connected ? "yes" : "no");
    printf("  Notify: %s\r\n", hid_keyboard_is_notify_enabled() ? "enabled" : "disabled");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hid_status, hid_status, Show HID status);
#endif

/*
 * Main entry point
 */
int main(void)
{
    static StackType_t hid_task_stack[2048];
    static StaticTask_t hid_task_tcb;

    ipro_platform_init();

    printf("\n=== IPRO7 BLE HID Keyboard ===\n");
    printf("Build: %s %s\n", __DATE__, __TIME__);

    xTaskCreateStatic(hid_keyboard_task, "hid_kb",
                      sizeof(hid_task_stack) / sizeof(StackType_t),
                      NULL, 5, hid_task_stack, &hid_task_tcb);

    vTaskStartScheduler();

    printf("ERROR: Scheduler exited\n");
    return -1;
}
