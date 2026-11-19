/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 *
 * IPRO6S Classic Bluetooth bring-up shared by the classic examples.
 *
 * Order matters on IPRO6S:
 *   1. The 2.4 GHz front end is shared by Wi-Fi and Bluetooth. With no
 *      Wi-Fi in the image nothing else calibrates it, so the app brings it
 *      up and hands it to Bluetooth before the controller starts.
 *   2. btdm_init() starts the BR/EDR + LE controller (prebuilt archive).
 *   3. The host starts from a task: HCI driver, then bt_enable().
 *   4. When the host is ready: bonds are loaded from flash, the app
 *      registers its profiles, and the board becomes discoverable.
 *
 * Radio policy: the controller supports one BR/EDR link. While a phone or
 * computer is connected, Inquiry Scan is switched off (Page Scan stays on
 * so a bonded device can reconnect); it comes back when the link drops.
 * The switch runs in its own task: an HCI command issued from a connection
 * callback would wait on the thread that delivers its own completion.
 */
#include "bt_classic_common.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>

#include <generated/autoconf.h>
#include <FreeRTOS.h>
#include <task.h>

#include "ipro_log.h"
#include "shell.h"
#include "btdm_sdk.h"
#include "ipro_ble_ctlr_hcitl.h"
#include "hci_driver_ipro_ble_ctlr.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/classic/classic.h>
#if defined(CONFIG_BT_SETTINGS)
#include <zephyr/settings/settings.h>
#endif

#define TAG "BT"

/* RF bring-up entry points of the IPRO6S RF library (CONFIG_PHY_CLI). */
extern void cmd_init_do(uint32_t xtal_freq_hz, uint8_t capcode);
extern void rf_bz_enter(void);
extern void bz_phy_reset(void);

#define BT_CLASSIC_XTAL_HZ          40000000u
#define BT_CLASSIC_XTAL_CAPCODE     32u
#define BT_CLASSIC_POLICY_STACK     1024u
#define BT_CLASSIC_POLICY_PRIORITY  8u

static bt_classic_ready_cb_t s_on_ready;
static TaskHandle_t s_policy_task;
static volatile bool s_br_connected;

int bt_classic_controller_init(void)
{
    int err;

    /* Calibrate the shared front end and give it to Bluetooth. */
    cmd_init_do(BT_CLASSIC_XTAL_HZ, BT_CLASSIC_XTAL_CAPCODE);
    rf_bz_enter();
    bz_phy_reset();

    err = btdm_init(NULL);
    if (err != 0) {
        IPRO_LOGE(TAG, "controller init failed: %d", err);
        return err;
    }
    IPRO_LOGI(TAG, "controller ready (BR/EDR + LE)");
    return 0;
}

bool bt_classic_connected(void)
{
    return s_br_connected;
}

static void bt_classic_apply_scan_policy(void)
{
    /* Discoverable (Inquiry Scan) only while no link is up; connectable
     * (Page Scan) always. */
    bool discoverable = !s_br_connected;
    int err = bt_br_set_connectable(true, NULL);

    if (err != 0 && err != -EALREADY) {
        IPRO_LOGW(TAG, "set connectable: %d", err);
    }
    err = bt_br_set_discoverable(discoverable, false);
    if (err != 0 && err != -EALREADY) {
        IPRO_LOGW(TAG, "set discoverable=%d: %d", discoverable, err);
    }
}

static void bt_classic_policy_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        bt_classic_apply_scan_policy();
    }
}

static void bt_classic_connected_cb(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_STR_LEN];

    if (err != 0 || !bt_conn_is_type(conn, BT_CONN_TYPE_BR)) {
        return;
    }
    bt_addr_to_str(bt_conn_get_dst_br(conn), addr, sizeof(addr));
    IPRO_LOGI(TAG, "connected: %s", addr);
    s_br_connected = true;
    xTaskNotifyGive(s_policy_task);
}

static void bt_classic_disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    if (!bt_conn_is_type(conn, BT_CONN_TYPE_BR)) {
        return;
    }
    IPRO_LOGI(TAG, "disconnected (reason 0x%02x)", reason);
    s_br_connected = false;
    xTaskNotifyGive(s_policy_task);
}

static void bt_classic_security_cb(struct bt_conn *conn, bt_security_t level,
                                   enum bt_security_err err)
{
    (void)conn;
    if (err != BT_SECURITY_ERR_SUCCESS) {
        IPRO_LOGW(TAG, "security failed: %d", (int)err);
        return;
    }
    IPRO_LOGI(TAG, "encrypted, security level %d", (int)level);
}

static struct bt_conn_cb s_conn_callbacks = {
    .connected = bt_classic_connected_cb,
    .disconnected = bt_classic_disconnected_cb,
    .security_changed = bt_classic_security_cb,
};

static void bt_classic_pairing_complete(struct bt_conn *conn, bool bonded)
{
    (void)conn;
    IPRO_LOGI(TAG, "paired%s", bonded ? ", bond saved" : "");
}

static void bt_classic_pairing_failed(struct bt_conn *conn,
                                      enum bt_security_err reason)
{
    (void)conn;
    IPRO_LOGW(TAG, "pairing failed: %d", (int)reason);
}

static struct bt_conn_auth_info_cb s_auth_info_callbacks = {
    .pairing_complete = bt_classic_pairing_complete,
    .pairing_failed = bt_classic_pairing_failed,
};

static void bt_classic_print_identity(void)
{
    struct bt_br_oob oob;
    char addr[BT_ADDR_STR_LEN];

    if (bt_br_oob_get_local(&oob) == 0) {
        bt_addr_to_str(&oob.addr, addr, sizeof(addr));
        IPRO_LOGI(TAG, "name \"%s\", address %s", CONFIG_BT_DEVICE_NAME, addr);
    }
}

static void bt_classic_ready(int err)
{
    if (err != 0) {
        IPRO_LOGE(TAG, "bt_enable failed: %d", err);
        return;
    }

    ipro_ble_ctlr_hcitl_host_ready();
    bt_conn_cb_register(&s_conn_callbacks);
    (void)bt_conn_auth_info_cb_register(&s_auth_info_callbacks);
    bt_set_bondable(true);

#if defined(CONFIG_BT_SETTINGS)
    err = settings_load_subtree("bt");
    if (err != 0) {
        IPRO_LOGW(TAG, "loading bonds failed: %d", err);
    }
#endif

    if (s_on_ready != NULL) {
        s_on_ready();
    }

    bt_classic_print_identity();
    bt_classic_apply_scan_policy();
    IPRO_LOGI(TAG, "discoverable, waiting for a connection");
}

int bt_classic_host_start(bt_classic_ready_cb_t on_ready)
{
    int err;

    s_on_ready = on_ready;
    if (xTaskCreate(bt_classic_policy_task, "bt_policy", BT_CLASSIC_POLICY_STACK,
                    NULL, BT_CLASSIC_POLICY_PRIORITY, &s_policy_task) != pdPASS) {
        return -ENOMEM;
    }

    err = hci_driver_ipro_ble_ctlr_init();
    if (err == 0) {
        err = bt_enable(bt_classic_ready);
    }
    if (err != 0) {
        IPRO_LOGE(TAG, "host start failed: %d", err);
    }
    return err;
}

#if defined(CONFIG_SHELL)
static void cmd_bt_unpair(int argc, char **argv)
{
    int err;

    (void)argc;
    (void)argv;
    if (s_br_connected) {
        IPRO_LOGW(TAG, "disconnect the phone first");
        return;
    }
    err = bt_br_unpair(NULL);
    IPRO_LOGI(TAG, "bonds cleared: %d", err);
}
SHELL_CMD_EXPORT_ALIAS(cmd_bt_unpair, bt_unpair, Forget every paired phone or computer);
#endif
