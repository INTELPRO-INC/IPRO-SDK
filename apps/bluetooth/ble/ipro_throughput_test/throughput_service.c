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
 * @file throughput_service.c
 * @brief Throughput Test Service Implementation (Peripheral/Slave side)
 *
 * Uses native Zephyr BT_GATT_SERVICE_DEFINE for compile-time GATT registration.
 */

#include "throughput_service.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_TAG "TP_SVC"

/*
 * UUID DEFINITIONS
 ****************************************************************************************
 */

#define BT_UUID_TP_SVC     BT_UUID_DECLARE_16(0xFF00)
#define BT_UUID_TP_TX      BT_UUID_DECLARE_16(0xFF01)
#define BT_UUID_TP_RX      BT_UUID_DECLARE_16(0xFF02)

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static struct {
    throughput_svc_callback_t callback;
    void *user_data;
    bool notify_enabled;
    bool initialized;
} g_svc_ctx = {0};

/*
 * GATT CALLBACKS
 ****************************************************************************************
 */

static ssize_t tp_tx_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                           void *buf, uint16_t len, uint16_t offset)
{
    /* TX characteristic has no persistent value */
    return 0;
}

static ssize_t tp_rx_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                            const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len == 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }

    IPRO_LOGD(LOG_TAG, "Data received: len=%d", len);

    if (g_svc_ctx.callback != NULL) {
        throughput_svc_event_data_t svc_event = {
            .event = THROUGHPUT_SVC_EVENT_DATA_RECEIVED,
            .conn_id = bt_conn_index(conn),
            .length = len,
            .data = buf
        };
        g_svc_ctx.callback(&svc_event, g_svc_ctx.user_data);
    }

    return len;
}

static void tp_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_svc_ctx.notify_enabled = (value == BT_GATT_CCC_NOTIFY);

    IPRO_LOGI(LOG_TAG, "Notifications %s", g_svc_ctx.notify_enabled ? "enabled" : "disabled");

    if (g_svc_ctx.callback != NULL) {
        throughput_svc_event_data_t svc_event = {
            .event = g_svc_ctx.notify_enabled ?
                     THROUGHPUT_SVC_EVENT_NOTIFY_ENABLED :
                     THROUGHPUT_SVC_EVENT_NOTIFY_DISABLED,
            .conn_id = 0,
            .length = 0,
            .data = NULL
        };
        g_svc_ctx.callback(&svc_event, g_svc_ctx.user_data);
    }
}

/*
 * STATIC GATT SERVICE DEFINITION
 ****************************************************************************************
 */

BT_GATT_SERVICE_DEFINE(tp_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_TP_SVC),

    /* TX Characteristic (Notify from slave to master) */
    BT_GATT_CHARACTERISTIC(BT_UUID_TP_TX,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           tp_tx_read, NULL, NULL),
    BT_GATT_CCC(tp_tx_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* RX Characteristic (Write from master to slave) */
    BT_GATT_CHARACTERISTIC(BT_UUID_TP_RX,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, tp_rx_write, NULL),
);

/*
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

int throughput_service_init(const throughput_svc_config_t *config)
{
    if (config == NULL) {
        return -1;
    }

    if (g_svc_ctx.initialized) {
        IPRO_LOGW(LOG_TAG, "Service already initialized");
        return 0;
    }

    IPRO_LOGI(LOG_TAG, "Initializing throughput service (static GATT registration)");

    g_svc_ctx.callback = config->callback;
    g_svc_ctx.user_data = config->user_data;
    g_svc_ctx.notify_enabled = false;
    g_svc_ctx.initialized = true;

    IPRO_LOGI(LOG_TAG, "Service init complete");
    return 0;
}

int throughput_service_notify(struct bt_conn *conn,
                             const uint8_t *data,
                             uint16_t length)
{
    if (!g_svc_ctx.initialized || !g_svc_ctx.notify_enabled) {
        return -1;
    }

    if (length > THROUGHPUT_MAX_DATA_LEN) {
        length = THROUGHPUT_MAX_DATA_LEN;
    }

    /* TX characteristic value attribute is at index 2 in tp_svc
     * (index 0 = service, 1 = char decl, 2 = char value, 3 = CCC) */
    int ret = bt_gatt_notify(conn, &attr_tp_svc[2], data, length);

    if (ret == -ENOMEM) {
        return -2;  /* Flow control busy */
    } else if (ret != 0) {
        return -1;
    }

    return 0;
}

bool throughput_service_is_notify_enabled(void)
{
    return g_svc_ctx.notify_enabled;
}
