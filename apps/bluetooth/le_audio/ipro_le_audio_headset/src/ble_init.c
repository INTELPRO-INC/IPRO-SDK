/**
 * @file ble_init.c
 * @brief BLE Stack Initialization Implementation (Zephyr blestack)
 *
 * Initializes BLE controller, HCI driver, and Zephyr bluetooth host stack.
 * Based on ipro_ble_remote implementation pattern.
 *
 * Copyright (C) IPRO SDK 2026
 */

#include "ble_init.h"
#include "ipro_log.h"

#include "FreeRTOS.h"
#include "task.h"

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/assigned_numbers.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/kernel.h>
#if defined(CONFIG_BT_SETTINGS)
#include <zephyr/settings/settings.h>
#endif
#include "hci_driver_ipro_ble_ctlr.h"
#include "ipro_ble_controller.h"

#if !defined(CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
#include "ble_main.h"
#include "cap_acceptor.h"
#include <zephyr/bluetooth/audio/audio.h>
#endif

#define LOG_TAG "BLE_INIT"
#if defined(CONFIG_IPRO_LE_AUDIO_SMOKE_INTERACTIVE_PAIRING)
#define SMOKE_INTERACTIVE_PAIRING 1
#else
#define SMOKE_INTERACTIVE_PAIRING 0
#endif
#if defined(CONFIG_IPRO_LE_AUDIO_SMOKE_REQUIRE_SECURITY)
#define SMOKE_REQUIRE_SECURITY 1
#else
#define SMOKE_REQUIRE_SECURITY 0
#endif

/*
 * ADVERTISING DATA
 ****************************************************************************************
 */

#define DEVICE_NAME "IPRO-LE-Audio"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

/* Advertising data using Zephyr bt_data format */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
    /* LE Audio Unicast Server announcement (ASCS service data) */
    BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                  0x4E, 0x18,   /* ASCS UUID 0x184E (little-endian) */
                  0x00,         /* BT_AUDIO_UNICAST_ANNOUNCEMENT_GENERAL */
                  0x00),  /* Metadata length=0 (1 byte) */
};

static const uint8_t sd_appearance[] = { 0x41, 0x04 };  /* 0x0441 = Earbud */
static const uint8_t sd_le_audio_uuid16[] = {
    0x53, 0x18,   /* CAS  0x1853 */
    0x46, 0x18,   /* CSIS 0x1846 */
    0x4E, 0x18,   /* ASCS 0x184E */
    0x50, 0x18,   /* PACS 0x1850 */
    0x44, 0x18,   /* VCS  0x1844 */
};
static uint8_t sd_csis_rsi[6];

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static bool g_ble_ready = false;
static struct bt_conn *g_current_conn = NULL;
static struct k_work_delayable g_adv_restart_work;

static void adv_restart_work_handler(struct k_work *work)
{
    (void)work;
    (void)ble_advertising_start();
}

/*
 * CONNECTION CALLBACKS
 ****************************************************************************************
 */

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err %u)", err);
        return;
    }
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "Connected: %s", addr);

    if (g_current_conn != NULL) {
        IPRO_LOGW(LOG_TAG, "Already connected; rejecting extra link from %s", addr);
        (void)bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        return;
    }

    g_current_conn = bt_conn_ref(conn);

    /* Single-link qualification build: keep connectable advertising off
     * while the ACL link is active.  This prevents Android from opening a
     * second ACL handle against CONFIG_BT_MAX_CONN=1 after bonding/privacy
     * procedures restart advertising internally.
     */
    (void)bt_le_adv_stop();

#if defined(CONFIG_BT_SMP)
    if (SMOKE_REQUIRE_SECURITY) {
        int sec_err = bt_conn_set_security(conn, BT_SECURITY_L2);
        if (sec_err) {
            IPRO_LOGE(LOG_TAG, "Failed to request security: %d", sec_err);
        } else {
            IPRO_LOGI(LOG_TAG, "Security requested: level %u", BT_SECURITY_L2);
        }
    } else {
        IPRO_LOGI(LOG_TAG, "Security request skipped (new-host smoke)");
    }
#endif
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "Disconnected: %s (reason %u)", addr, reason);
    
    if (g_current_conn) {
        bt_conn_unref(g_current_conn);
        g_current_conn = NULL;
    }
    
    /* Restart advertising after the host has finished releasing the old
     * connection object; immediate restart can fail with -ENOMEM on the
     * single-link build.
     */
    (void)k_work_reschedule(&g_adv_restart_work, K_MSEC(500));
}

static void le_param_updated_cb(struct bt_conn *conn, uint16_t interval,
                                uint16_t latency, uint16_t timeout)
{
    IPRO_LOGI(LOG_TAG, "LE conn param updated: int %u, lat %u, to %u",
              interval, latency, timeout);
}

static void security_changed_cb(struct bt_conn *conn, bt_security_t level,
                                enum bt_security_err err)
{
    char addr[BT_ADDR_LE_STR_LEN];
    
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    
    if (!err) {
        IPRO_LOGI(LOG_TAG, "Security changed: %s level %u", addr, level);
    } else {
        IPRO_LOGE(LOG_TAG, "Security failed: %s err %u", addr, err);
    }
}

/* Connection callback structure */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
    .le_param_updated = le_param_updated_cb,
    .security_changed = security_changed_cb,
};

void ble_conn_callbacks_register(void)
{
    /* Register connection callbacks */
    bt_conn_cb_register(&conn_callbacks);
    IPRO_LOGI(LOG_TAG, "Connection callbacks registered");
}

/*
 * SMP AUTHENTICATION CALLBACKS (for LE Audio pairing)
 ****************************************************************************************
 */

#if SMOKE_INTERACTIVE_PAIRING
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "[SMP] Passkey for %s: %06u", addr, passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int err;

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "[SMP] Confirm passkey for %s: %06u", addr, passkey);
    /* Auto-confirm for Numeric Comparison pairing */
    err = bt_conn_auth_passkey_confirm(conn);
    if (err) {
        IPRO_LOGE(LOG_TAG, "[SMP] passkey_confirm failed: %d", err);
    } else {
        IPRO_LOGI(LOG_TAG, "[SMP] passkey confirmed (our side)");
    }
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing confirm request from %s", addr);
    /* Auto-accept pairing request */
    bt_conn_auth_pairing_confirm(conn);
}

static void auth_cancel(struct bt_conn *conn)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing cancelled: %s", addr);
}
#endif

static void auth_pairing_complete(struct bt_conn *conn, bool bonded)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGI(LOG_TAG, "[SMP] Pairing complete: %s, bonded=%d", addr, bonded);
}

static void auth_pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
    IPRO_LOGE(LOG_TAG, "[SMP] Pairing failed: %s, reason=%d", addr, reason);
}

#if SMOKE_INTERACTIVE_PAIRING
static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};
#endif

static struct bt_conn_auth_info_cb auth_info_callbacks = {
    .pairing_complete = auth_pairing_complete,
    .pairing_failed = auth_pairing_failed,
};

/*
 * BLE STACK INITIALIZATION
 ****************************************************************************************
 */

static void bt_ready_cb(int err)
{
    int ret;
    
    if (err) {
        IPRO_LOGE(LOG_TAG, "Bluetooth init failed (err %d)", err);
        g_ble_ready = false;
        return;
    }
    
    IPRO_LOGI(LOG_TAG, "Bluetooth initialized successfully");
    ipro_ble_ctlr_hcitl_host_ready();

#if defined(CONFIG_BT_SETTINGS)
    ret = settings_load_subtree("bt");
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Bluetooth settings load failed: %d", ret);
        g_ble_ready = false;
        return;
    }
    IPRO_LOGI(LOG_TAG, "Bluetooth settings loaded");
#endif

    g_ble_ready = true;
    k_work_init_delayable(&g_adv_restart_work, adv_restart_work_handler);

#if SMOKE_INTERACTIVE_PAIRING
    /* Register interactive authentication callbacks for manual LE Audio pairing. */
    ret = bt_conn_auth_cb_register(&auth_callbacks);
    if (ret) {
        IPRO_LOGE(LOG_TAG, "Failed to register auth callbacks: %d", ret);
    } else {
        IPRO_LOGI(LOG_TAG, "SMP auth callbacks registered");
    }
#else
    IPRO_LOGI(LOG_TAG, "SMP auth callbacks not registered (Just Works smoke)");
#endif
    ret = bt_conn_auth_info_cb_register(&auth_info_callbacks);
    if (ret) {
        IPRO_LOGE(LOG_TAG, "Failed to register auth info callbacks: %d", ret);
    } else {
        IPRO_LOGI(LOG_TAG, "SMP auth info callbacks registered");
    }

#if !defined(CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE)
    /* Initialize LE Audio services (CAP, BAP, VCP) */
    IPRO_LOGI(LOG_TAG, "Initializing LE Audio services...");
    ret = le_audio_init();
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "LE Audio init failed: %d", ret);
        /* Continue anyway - basic BLE still works */
    }
#else
    IPRO_LOGI(LOG_TAG, "New-host smoke: LE Audio profile registration skipped");
#endif

    /* Register connection callbacks */
    ble_conn_callbacks_register();

    /* Start advertising automatically */
    ble_advertising_start();
}

int ble_stack_init(void)
{
    int ret;
    
    IPRO_LOGI(LOG_TAG, "========================================");
    IPRO_LOGI(LOG_TAG, "   LE Audio Headset (Zephyr blestack)");
    IPRO_LOGI(LOG_TAG, "========================================");
    
    IPRO_LOGI(LOG_TAG, "Initializing BLE controller...");
    ret = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    if (ret) {
        IPRO_LOGE(LOG_TAG, "controller init failed: %d", ret);
        return ret;
    }

    /* Initialize HCI driver */
    IPRO_LOGI(LOG_TAG, "Initializing HCI driver...");
    ret = hci_driver_ipro_ble_ctlr_init();
    if (ret) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", ret);
        return ret;
    }
    
    /* Small delay for HCI to stabilize */
    vTaskDelay(pdMS_TO_TICKS(50));
    
    /* Initialize Zephyr Bluetooth stack */
    IPRO_LOGI(LOG_TAG, "Initializing BLE stack...");
    ret = bt_enable(bt_ready_cb);
    if (ret) {
        IPRO_LOGE(LOG_TAG, "bt_enable failed: %d", ret);
        return ret;
    }
    
    IPRO_LOGI(LOG_TAG, "BLE stack initialization in progress...");
    return 0;
}

bool ble_stack_is_ready(void)
{
    return g_ble_ready;
}

/*
 * ADVERTISING CONTROL
 ****************************************************************************************
 */

int ble_advertising_start(void)
{
    int err;
    struct bt_data sd[3];
    size_t sd_count = 0;
    
    if (!g_ble_ready) {
        IPRO_LOGW(LOG_TAG, "BLE not ready, cannot start advertising");
        return -1;
    }

    if (g_current_conn != NULL) {
        IPRO_LOGW(LOG_TAG, "Connection active; skip advertising start");
        return 0;
    }

    const struct bt_le_adv_param adv_param =
        BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_CONN
#if defined(CONFIG_IPRO_LE_AUDIO_SMOKE_USE_IDENTITY_ADV)
                             | BT_LE_ADV_OPT_USE_IDENTITY
#endif
                             ,
                             BT_GAP_ADV_FAST_INT_MIN_2,
                             BT_GAP_ADV_FAST_INT_MAX_2,
                             NULL);

    sd[sd_count++] = (struct bt_data)BT_DATA(BT_DATA_GAP_APPEARANCE,
                                             sd_appearance,
                                             sizeof(sd_appearance));
    sd[sd_count++] = (struct bt_data)BT_DATA(BT_DATA_UUID16_ALL,
                                             sd_le_audio_uuid16,
                                             sizeof(sd_le_audio_uuid16));
#if !defined(CONFIG_IPRO_LE_AUDIO_NEW_HOST_SMOKE) && defined(CONFIG_BT_CAP_ACCEPTOR_SET_MEMBER)
    if (cap_acceptor_generate_rsi(sd_csis_rsi)) {
        sd[sd_count++] = (struct bt_data)BT_DATA(BT_DATA_CSIS_RSI,
                                                 sd_csis_rsi,
                                                 sizeof(sd_csis_rsi));
        IPRO_LOGI(LOG_TAG, "CSIS RSI included in scan response");
    } else {
        IPRO_LOGW(LOG_TAG, "CSIS RSI unavailable; advertising without RSI");
    }
#endif

    /* Start connectable advertising */
    err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, sd_count);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Advertising failed to start (err %d)", err);
        return err;
    }
    
    IPRO_LOGI(LOG_TAG, "Advertising started: %s", DEVICE_NAME);
    return 0;
}

int ble_advertising_stop(void)
{
    int err = bt_le_adv_stop();
    if (err) {
        IPRO_LOGE(LOG_TAG, "Advertising failed to stop (err %d)", err);
        return err;
    }
    
    IPRO_LOGI(LOG_TAG, "Advertising stopped");
    return 0;
}
