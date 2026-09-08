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
 * @file ble_transparent.c
 * @brief BLE NUS (Nordic UART Service) transparent bridge
 */

#include "ble_transparent.h"
#include "app_config.h"
#include "ipro_log.h"

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/sys/byteorder.h>
#include <hci_driver_ipro_ble_ctlr.h>
#include <ipro_ble_controller.h>
#include <hci_core.h>
#include <ble_lib_api.h>
#include "conn_internal.h"

#define LOG_TAG "BLE_TRANS"

/* NUS UUIDs (Nordic UART Service) */
static struct bt_uuid_128 nus_svc_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_rx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_tx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E);

static struct {
    ble_state_t state;
    struct bt_conn *conn;
    ble_rx_callback_t rx_cb;
    bool tx_notify_enabled;
    char dev_name[MAX_DEVICE_NAME_LEN + 1];
    uint16_t adv_interval_ms;
    int8_t tx_power;
    bool bondable;
    uint32_t passkey;
    /* Custom adv/scan rsp data */
    uint8_t custom_ad[31];
    uint8_t custom_ad_len;
    uint8_t custom_sd[31];
    uint8_t custom_sd_len;
} g_ble = {
    .state = BLE_STATE_IDLE,
    .dev_name = DEFAULT_DEVICE_NAME,
    .adv_interval_ms = 100, /* matches DEFAULT_ADV_INT_MIN */
    .tx_power = DEFAULT_TX_POWER,
    .bondable = DEFAULT_BONDABLE,
    .passkey = DEFAULT_PASSKEY,
};

/* GATT callbacks */
static ssize_t nus_rx_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len == 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    if (g_ble.rx_cb) {
        g_ble.rx_cb((const uint8_t *)buf, len);
    }

    return len;
}

static void nus_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_ble.tx_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "NUS TX notify %s", g_ble.tx_notify_enabled ? "enabled" : "disabled");
}

/* NUS Service definition */
BT_GATT_SERVICE_DEFINE(nus_svc,
    BT_GATT_PRIMARY_SERVICE(&nus_svc_uuid),
    /* TX characteristic (device -> phone, notify) */
    BT_GATT_CHARACTERISTIC(&nus_tx_uuid.uuid,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(nus_tx_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    /* RX characteristic (phone -> device, write) */
    BT_GATT_CHARACTERISTIC(&nus_rx_uuid.uuid,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, nus_rx_write, NULL),
);

/* Advertising data */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEFAULT_DEVICE_NAME, sizeof(DEFAULT_DEVICE_NAME) - 1),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
        0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E),
};

/* Connection callbacks */
static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err 0x%02x)", err);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Connected: %s", addr_str);

    g_ble.conn = bt_conn_ref(conn);
    g_ble.state = BLE_STATE_CONNECTED;
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(LOG_TAG, "Disconnected (reason 0x%02x)", reason);

    if (g_ble.conn) {
        bt_conn_unref(g_ble.conn);
        g_ble.conn = NULL;
    }
    g_ble.state = BLE_STATE_IDLE;
    g_ble.tx_notify_enabled = false;

    /* Auto-restart advertising */
    vTaskDelay(pdMS_TO_TICKS(200));
    ble_transparent_adv_start();
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/* SMP auth callbacks */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    IPRO_LOGI(LOG_TAG, "Passkey: %06u", passkey);
}

static void auth_passkey_confirm(struct bt_conn *conn, unsigned int passkey)
{
    bt_conn_auth_passkey_confirm(conn);
}

static void auth_pairing_confirm(struct bt_conn *conn)
{
    bt_conn_auth_pairing_confirm(conn);
}

static void auth_cancel(struct bt_conn *conn)
{
    IPRO_LOGW(LOG_TAG, "Pairing cancelled");
}

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};

/* bt_ready callback */
static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "BLE init failed (err %d)", err);
        return;
    }

    IPRO_LOGI(LOG_TAG, "BLE initialized");

    bt_set_name(g_ble.dev_name);
    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_callbacks);

    bt_set_bondable(g_ble.bondable);
    bt_passkey_set(g_ble.passkey);

    ble_transparent_adv_start();
}

/* Public API */
int ble_transparent_init(void)
{
    IPRO_LOGI(LOG_TAG, "Initializing BLE controller...");
    ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    IPRO_LOGI(LOG_TAG, "Initializing HCI driver...");
    int err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", err);
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    IPRO_LOGI(LOG_TAG, "Initializing BLE stack...");
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(LOG_TAG, "bt_enable failed: %d", err);
        return -1;
    }

    return 0;
}

int ble_transparent_send(const uint8_t *data, uint16_t len)
{
    if (!g_ble.conn || !g_ble.tx_notify_enabled) {
        return -1;
    }

    /* TX characteristic attr is at index 1 in nus_svc */
    uint16_t mtu = bt_gatt_get_mtu(g_ble.conn);
    uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
    uint16_t offset = 0;

    while (offset < len) {
        uint16_t chunk = len - offset;
        if (chunk > max_payload) {
            chunk = max_payload;
        }

        int ret = bt_gatt_notify(g_ble.conn, &attr_nus_svc[1], data + offset, chunk);
        if (ret) {
            return ret;
        }
        offset += chunk;
    }

    return 0;
}

void ble_transparent_set_rx_cb(ble_rx_callback_t cb)
{
    g_ble.rx_cb = cb;
}

int ble_transparent_set_name(const char *name)
{
    if (!name || strlen(name) == 0 || strlen(name) > MAX_DEVICE_NAME_LEN) {
        return -1;
    }
    strncpy(g_ble.dev_name, name, MAX_DEVICE_NAME_LEN);
    g_ble.dev_name[MAX_DEVICE_NAME_LEN] = '\0';
    bt_set_name(g_ble.dev_name);
    return 0;
}

const char *ble_transparent_get_name(void)
{
    return g_ble.dev_name;
}

int ble_transparent_adv_start(void)
{
    if (g_ble.state == BLE_STATE_CONNECTED) {
        return -1;
    }

    /* Convert ms to BLE units (N * 0.625ms) */
    uint16_t interval_units = (g_ble.adv_interval_ms * 1000) / 625;
    if (interval_units < 0x20) interval_units = 0x20;   /* min 20ms */
    if (interval_units > 0x4000) interval_units = 0x4000;

    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = interval_units,
        .interval_max = interval_units + (interval_units / 2), /* max = 1.5x min */
    };

    /* Use custom adv data if set, otherwise use defaults */
    const struct bt_data *ad_ptr = ad;
    size_t ad_cnt = ARRAY_SIZE(ad);
    const struct bt_data *sd_ptr = sd;
    size_t sd_cnt = ARRAY_SIZE(sd);

    struct bt_data custom_ad_data, custom_sd_data;
    if (g_ble.custom_ad_len > 0) {
        custom_ad_data.type = 0xFF; /* Manufacturer specific - raw data */
        custom_ad_data.data_len = g_ble.custom_ad_len;
        custom_ad_data.data = g_ble.custom_ad;
        ad_ptr = &custom_ad_data;
        ad_cnt = 1;
    }
    if (g_ble.custom_sd_len > 0) {
        custom_sd_data.type = 0xFF;
        custom_sd_data.data_len = g_ble.custom_sd_len;
        custom_sd_data.data = g_ble.custom_sd;
        sd_ptr = &custom_sd_data;
        sd_cnt = 1;
    }

    int err = bt_le_adv_start(&adv_param, ad_ptr, ad_cnt, sd_ptr, sd_cnt);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Advertising start failed: %d", err);
        return -1;
    }

    g_ble.state = BLE_STATE_ADVERTISING;
    IPRO_LOGI(LOG_TAG, "Advertising as '%s' (interval %ums)", g_ble.dev_name, g_ble.adv_interval_ms);
    return 0;
}

int ble_transparent_adv_stop(void)
{
    if (g_ble.state != BLE_STATE_ADVERTISING) {
        return 0;
    }

    int err = bt_le_adv_stop();
    if (err) {
        return -1;
    }

    g_ble.state = BLE_STATE_IDLE;
    return 0;
}

int ble_transparent_set_adv_interval(uint16_t interval_ms)
{
    if (interval_ms < 20 || interval_ms > 10240) {
        return -1;
    }
    g_ble.adv_interval_ms = interval_ms;

    /* If currently advertising, restart with new interval */
    if (g_ble.state == BLE_STATE_ADVERTISING) {
        ble_transparent_adv_stop();
        return ble_transparent_adv_start();
    }
    return 0;
}

uint16_t ble_transparent_get_adv_interval(void)
{
    return g_ble.adv_interval_ms;
}

int ble_transparent_set_adv_data(const uint8_t *data, uint16_t len)
{
    if (!data || len == 0 || len > 31) {
        return -1;
    }
    memcpy(g_ble.custom_ad, data, len);
    g_ble.custom_ad_len = (uint8_t)len;

    if (g_ble.state == BLE_STATE_ADVERTISING) {
        ble_transparent_adv_stop();
        return ble_transparent_adv_start();
    }
    return 0;
}

int ble_transparent_set_scan_rsp_data(const uint8_t *data, uint16_t len)
{
    if (!data || len == 0 || len > 31) {
        return -1;
    }
    memcpy(g_ble.custom_sd, data, len);
    g_ble.custom_sd_len = (uint8_t)len;

    if (g_ble.state == BLE_STATE_ADVERTISING) {
        ble_transparent_adv_stop();
        return ble_transparent_adv_start();
    }
    return 0;
}

int ble_transparent_disconnect(void)
{
    if (!g_ble.conn) {
        return -1;
    }
    return bt_conn_disconnect(g_ble.conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
}

ble_state_t ble_transparent_get_state(void)
{
    return g_ble.state;
}

int ble_transparent_set_conn_params(uint16_t min_ms, uint16_t max_ms)
{
    if (!g_ble.conn) {
        return -1;
    }

    struct bt_le_conn_param param = {
        .interval_min = (min_ms * 100) / 125,
        .interval_max = (max_ms * 100) / 125,
        .latency = 0,
        .timeout = 400,
    };

    return bt_conn_le_param_update(g_ble.conn, &param);
}

uint16_t ble_transparent_get_mtu(void)
{
    if (!g_ble.conn) {
        return 0;
    }
    return bt_gatt_get_mtu(g_ble.conn);
}

int ble_transparent_get_rssi(int8_t *rssi)
{
    if (!g_ble.conn || !rssi) {
        return -1;
    }

    struct net_buf *buf, *rsp = NULL;
    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (!buf) {
        return -1;
    }

    struct bt_hci_cp_read_rssi *cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(g_ble.conn->handle);

    int err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_RSSI, buf, &rsp);
    if (err) {
        return err;
    }

    struct bt_hci_rp_read_rssi *rp = (void *)rsp->data;
    *rssi = rp->rssi;
    net_buf_unref(rsp);
    return 0;
}

int ble_transparent_get_conn_info(uint16_t *interval, uint16_t *latency,
                                   uint16_t *timeout, uint16_t *mtu)
{
    if (!g_ble.conn) {
        return -1;
    }

    struct bt_conn_info info;
    int err = bt_conn_get_info(g_ble.conn, &info);
    if (err) {
        return err;
    }

    if (interval) *interval = info.le.interval;
    if (latency)  *latency  = info.le.latency;
    if (timeout)  *timeout  = info.le.timeout;
    if (mtu)      *mtu      = bt_gatt_get_mtu(g_ble.conn);
    return 0;
}

int ble_transparent_set_tx_power(int8_t dbm)
{
    g_ble.tx_power = dbm;
    /* TX power is typically set via vendor-specific HCI command.
     * Store the value; actual HW control depends on controller support. */
    return 0;
}

int8_t ble_transparent_get_tx_power(void)
{
    return g_ble.tx_power;
}

int ble_transparent_get_addr(char *buf, uint16_t buf_size)
{
    if (!buf || buf_size < BT_ADDR_LE_STR_LEN) {
        return -1;
    }

    bt_addr_le_t addr;
    size_t count = 1;
    bt_id_get(&addr, &count);
    bt_addr_le_to_str(&addr, buf, buf_size);
    return 0;
}

int ble_transparent_enter_sleep(void)
{
    /* Stop advertising if active */
    if (g_ble.state == BLE_STATE_ADVERTISING) {
        ble_transparent_adv_stop();
    }
    /* Disconnect if connected */
    if (g_ble.conn) {
        ble_transparent_disconnect();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    /* Platform-specific low-power entry would go here */
    return 0;
}

/* --- Security --- */

int ble_transparent_set_bondable(bool enable)
{
    g_ble.bondable = enable;
    bt_set_bondable(enable);
    return 0;
}

bool ble_transparent_get_bondable(void)
{
    return g_ble.bondable;
}

int ble_transparent_set_passkey(uint32_t passkey)
{
    if (passkey > 999999) {
        return -1;
    }
    g_ble.passkey = passkey;
    return bt_passkey_set(passkey);
}

uint32_t ble_transparent_get_passkey(void)
{
    return g_ble.passkey;
}

int ble_transparent_set_security(uint8_t level)
{
    if (!g_ble.conn || level < 1 || level > 4) {
        return -1;
    }
    return bt_conn_set_security(g_ble.conn, (bt_security_t)level);
}

int ble_transparent_clear_bonds(void)
{
    return bt_unpair(BT_ID_DEFAULT, NULL);
}

struct bond_iter_ctx {
    char *buf;
    uint16_t buf_size;
    uint16_t offset;
    int count;
};

static void bond_iter_cb(const struct bt_bond_info *info, void *user_data)
{
    struct bond_iter_ctx *ctx = user_data;
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(&info->addr, addr_str, sizeof(addr_str));

    int written = snprintf(ctx->buf + ctx->offset, ctx->buf_size - ctx->offset,
                           "+BOND:%d,%s\r\n", ctx->count, addr_str);
    if (written > 0 && (uint16_t)written < ctx->buf_size - ctx->offset) {
        ctx->offset += (uint16_t)written;
    }
    ctx->count++;
}

int ble_transparent_get_bonds(char *buf, uint16_t buf_size)
{
    if (!buf || buf_size == 0) {
        return -1;
    }

    struct bond_iter_ctx ctx = { .buf = buf, .buf_size = buf_size };
    buf[0] = '\0';
    bt_foreach_bond(BT_ID_DEFAULT, bond_iter_cb, &ctx);
    return ctx.count;
}

/* --- PHY --- */

int ble_transparent_set_phy(uint8_t phy)
{
    if (!g_ble.conn) {
        return -1;
    }

    struct net_buf *buf;
    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (!buf) {
        return -1;
    }

    struct bt_hci_cp_le_set_phy *cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(g_ble.conn->handle);
    cp->all_phys = 0;      /* No preference bits */
    cp->tx_phys = phy;
    cp->rx_phys = phy;
    cp->phy_opts = 0;      /* No coding preference */

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_PHY, buf, NULL);
}

int ble_transparent_get_phy(uint8_t *tx_phy, uint8_t *rx_phy)
{
    if (!g_ble.conn || !tx_phy || !rx_phy) {
        return -1;
    }

    struct net_buf *buf, *rsp = NULL;
    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (!buf) {
        return -1;
    }

    struct bt_hci_cp_le_read_phy *cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(g_ble.conn->handle);

    int err = bt_hci_cmd_send_sync(BT_HCI_OP_LE_READ_PHY, buf, &rsp);
    if (err) {
        return err;
    }

    struct bt_hci_rp_le_read_phy *rp = (void *)rsp->data;
    *tx_phy = rp->tx_phy;
    *rx_phy = rp->rx_phy;
    net_buf_unref(rsp);
    return 0;
}

/* --- Data Length Extension --- */

int ble_transparent_set_data_length(uint16_t tx_octets, uint16_t tx_time)
{
    if (!g_ble.conn) {
        return -1;
    }

    if (tx_octets < 27 || tx_octets > 251) {
        return -1;
    }
    if (tx_time < 328 || tx_time > 17040) {
        return -1;
    }

    struct net_buf *buf;
    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (!buf) {
        return -1;
    }

    struct bt_hci_cp_le_set_data_len *cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(g_ble.conn->handle);
    cp->tx_octets = sys_cpu_to_le16(tx_octets);
    cp->tx_time = sys_cpu_to_le16(tx_time);

    return bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_DATA_LEN, buf, NULL);
}
