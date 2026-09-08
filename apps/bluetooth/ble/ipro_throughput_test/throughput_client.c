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
 * @file throughput_client.c
 * @brief Throughput Test Client Implementation (Central/Master side)
 *
 * Uses native Zephyr bt_gatt_discover / bt_gatt_subscribe / bt_gatt_write APIs.
 */

#include "throughput_client.h"
#include "throughput_service.h"
#include "ipro_log.h"

#include <string.h>
#include <stdio.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_TAG "TP_CLIENT"

/*
 * DEFINES
 ****************************************************************************************
 */

#define THROUGHPUT_SERVICE_UUID     0xFF00
#define THROUGHPUT_CHAR_TX_UUID     0xFF01  /* TX char (we receive notifications from this) */
#define THROUGHPUT_CHAR_RX_UUID     0xFF02  /* RX char (we write to this) */

/// Discovery state machine
typedef enum {
    DISC_STATE_IDLE = 0,
    DISC_STATE_DISCOVER_SERVICE,
    DISC_STATE_DISCOVER_CHARS,
    DISC_STATE_DISCOVER_DESCS,
    DISC_STATE_SUBSCRIBE,
    DISC_STATE_COMPLETE,
} disc_state_t;

/*
 * LOCAL VARIABLES
 ****************************************************************************************
 */

static struct {
    throughput_client_callback_t callback;
    void *user_data;
    throughput_client_handles_t handles;
    bool initialized;

    /* Discovery state */
    disc_state_t disc_state;
    struct bt_conn *disc_conn;

    /* Zephyr discover/subscribe params (must be static - used asynchronously) */
    struct bt_gatt_discover_params discover_params;
    struct bt_gatt_subscribe_params subscribe_params;

    /* UUID storage for discovery */
    struct bt_uuid_16 uuid;
} g_client_ctx = {0};

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static uint8_t discover_func(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           struct bt_gatt_discover_params *params);

static uint8_t notify_func(struct bt_conn *conn,
                          struct bt_gatt_subscribe_params *params,
                          const void *data, uint16_t length);

/*
 * DISCOVERY STATE MACHINE
 ****************************************************************************************
 */

static void start_char_discovery(struct bt_conn *conn)
{
    IPRO_LOGI(LOG_TAG, "Starting characteristic discovery");
    g_client_ctx.disc_state = DISC_STATE_DISCOVER_CHARS;

    memset(&g_client_ctx.discover_params, 0, sizeof(g_client_ctx.discover_params));
    g_client_ctx.discover_params.func = discover_func;
    g_client_ctx.discover_params.start_handle = g_client_ctx.handles.service_start_handle;
    g_client_ctx.discover_params.end_handle = g_client_ctx.handles.service_end_handle;
    g_client_ctx.discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

    int err = bt_gatt_discover(conn, &g_client_ctx.discover_params);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Char discovery failed: %d", err);
        g_client_ctx.disc_state = DISC_STATE_IDLE;
    }
}

static void start_desc_discovery(struct bt_conn *conn)
{
    IPRO_LOGI(LOG_TAG, "Starting descriptor discovery");
    g_client_ctx.disc_state = DISC_STATE_DISCOVER_DESCS;

    memset(&g_client_ctx.discover_params, 0, sizeof(g_client_ctx.discover_params));
    g_client_ctx.discover_params.func = discover_func;
    g_client_ctx.discover_params.start_handle = g_client_ctx.handles.tx_char_handle + 1;
    g_client_ctx.discover_params.end_handle = g_client_ctx.handles.service_end_handle;
    g_client_ctx.discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;

    int err = bt_gatt_discover(conn, &g_client_ctx.discover_params);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Desc discovery failed: %d", err);
        g_client_ctx.disc_state = DISC_STATE_IDLE;
    }
}

static void start_subscribe(struct bt_conn *conn)
{
    if (g_client_ctx.handles.tx_cccd_handle == 0) {
        IPRO_LOGW(LOG_TAG, "CCCD not found, completing without notifications");
        g_client_ctx.disc_state = DISC_STATE_COMPLETE;
        g_client_ctx.handles.discovery_complete = true;

        if (g_client_ctx.callback != NULL) {
            throughput_client_event_data_t evt = {
                .event = THROUGHPUT_CLIENT_EVENT_DISCOVERY_COMPLETE,
            };
            g_client_ctx.callback(&evt, g_client_ctx.user_data);
        }
        return;
    }

    IPRO_LOGI(LOG_TAG, "Subscribing to notifications: val=0x%04X cccd=0x%04X",
              g_client_ctx.handles.tx_char_handle,
              g_client_ctx.handles.tx_cccd_handle);

    g_client_ctx.disc_state = DISC_STATE_SUBSCRIBE;

    memset(&g_client_ctx.subscribe_params, 0, sizeof(g_client_ctx.subscribe_params));
    g_client_ctx.subscribe_params.notify = notify_func;
    g_client_ctx.subscribe_params.value_handle = g_client_ctx.handles.tx_char_handle;
    g_client_ctx.subscribe_params.ccc_handle = g_client_ctx.handles.tx_cccd_handle;
    g_client_ctx.subscribe_params.value = BT_GATT_CCC_NOTIFY;

    int err = bt_gatt_subscribe(conn, &g_client_ctx.subscribe_params);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Subscribe failed: %d", err);
        /* Still report discovery complete even without notifications */
        g_client_ctx.disc_state = DISC_STATE_COMPLETE;
        g_client_ctx.handles.discovery_complete = true;

        if (g_client_ctx.callback != NULL) {
            throughput_client_event_data_t evt = {
                .event = THROUGHPUT_CLIENT_EVENT_DISCOVERY_COMPLETE,
            };
            g_client_ctx.callback(&evt, g_client_ctx.user_data);
        }
    } else {
        /* Subscribe succeeded */
        g_client_ctx.disc_state = DISC_STATE_COMPLETE;
        g_client_ctx.handles.discovery_complete = true;

        IPRO_LOGI(LOG_TAG, "*** Discovery and notification subscribe complete ***");
        IPRO_LOGI(LOG_TAG, "Handles: rx=0x%04X tx=0x%04X cccd=0x%04X",
                  g_client_ctx.handles.rx_char_handle,
                  g_client_ctx.handles.tx_char_handle,
                  g_client_ctx.handles.tx_cccd_handle);

        if (g_client_ctx.callback != NULL) {
            throughput_client_event_data_t evt = {
                .event = THROUGHPUT_CLIENT_EVENT_DISCOVERY_COMPLETE,
            };
            g_client_ctx.callback(&evt, g_client_ctx.user_data);

            evt.event = THROUGHPUT_CLIENT_EVENT_NOTIFY_ENABLED;
            g_client_ctx.callback(&evt, g_client_ctx.user_data);
        }
    }
}

/*
 * GATT DISCOVER CALLBACK
 ****************************************************************************************
 */

static uint8_t discover_func(struct bt_conn *conn,
                           const struct bt_gatt_attr *attr,
                           struct bt_gatt_discover_params *params)
{
    if (attr == NULL) {
        /* Discovery complete for current phase */
        IPRO_LOGI(LOG_TAG, "Discovery phase complete, state=%d", g_client_ctx.disc_state);

        switch (g_client_ctx.disc_state) {
            case DISC_STATE_DISCOVER_SERVICE:
                if (g_client_ctx.handles.service_start_handle != 0) {
                    start_char_discovery(conn);
                } else {
                    IPRO_LOGE(LOG_TAG, "Throughput service not found!");
                    g_client_ctx.disc_state = DISC_STATE_IDLE;
                    if (g_client_ctx.callback) {
                        throughput_client_event_data_t evt = {
                            .event = THROUGHPUT_CLIENT_EVENT_DISCOVERY_FAILED,
                        };
                        g_client_ctx.callback(&evt, g_client_ctx.user_data);
                    }
                }
                break;

            case DISC_STATE_DISCOVER_CHARS:
                if (g_client_ctx.handles.tx_char_handle != 0) {
                    start_desc_discovery(conn);
                } else {
                    IPRO_LOGE(LOG_TAG, "TX characteristic not found!");
                    g_client_ctx.disc_state = DISC_STATE_IDLE;
                    if (g_client_ctx.callback) {
                        throughput_client_event_data_t evt = {
                            .event = THROUGHPUT_CLIENT_EVENT_DISCOVERY_FAILED,
                        };
                        g_client_ctx.callback(&evt, g_client_ctx.user_data);
                    }
                }
                break;

            case DISC_STATE_DISCOVER_DESCS:
                start_subscribe(conn);
                break;

            default:
                break;
        }

        return BT_GATT_ITER_STOP;
    }

    switch (g_client_ctx.disc_state) {
        case DISC_STATE_DISCOVER_SERVICE: {
            struct bt_gatt_service_val *svc = attr->user_data;
            IPRO_LOGI(LOG_TAG, "Service found: start=0x%04X end=0x%04X",
                      attr->handle, svc->end_handle);
            g_client_ctx.handles.service_start_handle = attr->handle;
            g_client_ctx.handles.service_end_handle = svc->end_handle;
        } break;

        case DISC_STATE_DISCOVER_CHARS: {
            struct bt_gatt_chrc *chrc = attr->user_data;
            uint16_t uuid16 = BT_UUID_16(chrc->uuid)->val;

            IPRO_LOGI(LOG_TAG, "Char found: handle=0x%04X val_handle=0x%04X uuid=0x%04X prop=0x%02X",
                      attr->handle, chrc->value_handle, uuid16, chrc->properties);

            if (uuid16 == THROUGHPUT_CHAR_TX_UUID) {
                IPRO_LOGI(LOG_TAG, "*** Found TX Characteristic ***");
                g_client_ctx.handles.tx_char_handle = chrc->value_handle;
            } else if (uuid16 == THROUGHPUT_CHAR_RX_UUID) {
                IPRO_LOGI(LOG_TAG, "*** Found RX Characteristic ***");
                g_client_ctx.handles.rx_char_handle = chrc->value_handle;
            }
        } break;

        case DISC_STATE_DISCOVER_DESCS: {
            uint16_t uuid16 = BT_UUID_16(attr->uuid)->val;

            IPRO_LOGI(LOG_TAG, "Desc found: handle=0x%04X uuid=0x%04X",
                      attr->handle, uuid16);

            if (uuid16 == 0x2902) {  /* BT_UUID_GATT_CCC */
                IPRO_LOGI(LOG_TAG, "*** Found CCCD ***");
                g_client_ctx.handles.tx_cccd_handle = attr->handle;
            }
        } break;

        default:
            break;
    }

    return BT_GATT_ITER_CONTINUE;
}

/*
 * NOTIFICATION CALLBACK
 ****************************************************************************************
 */

static uint8_t notify_func(struct bt_conn *conn,
                          struct bt_gatt_subscribe_params *params,
                          const void *data, uint16_t length)
{
    if (data == NULL) {
        /* Subscription removed */
        IPRO_LOGI(LOG_TAG, "Notification subscription removed");
        return BT_GATT_ITER_STOP;
    }

    IPRO_LOGD(LOG_TAG, "Notification received: len=%d", length);

    if (g_client_ctx.callback != NULL) {
        throughput_client_event_data_t evt = {
            .event = THROUGHPUT_CLIENT_EVENT_DATA_RECEIVED,
            .conn_id = bt_conn_index(conn),
            .length = length,
            .data = data
        };
        g_client_ctx.callback(&evt, g_client_ctx.user_data);
    }

    return BT_GATT_ITER_CONTINUE;
}

/*
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

int throughput_client_init(const throughput_client_config_t *config)
{
    if (config == NULL) {
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Initializing throughput client");

    memset(&g_client_ctx, 0, sizeof(g_client_ctx));
    g_client_ctx.callback = config->callback;
    g_client_ctx.user_data = config->user_data;
    g_client_ctx.initialized = true;

    IPRO_LOGI(LOG_TAG, "Throughput client initialized successfully");
    return 0;
}

int throughput_client_discover(struct bt_conn *conn)
{
    if (!g_client_ctx.initialized) {
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Starting service discovery");

    /* Reset discovery state */
    g_client_ctx.disc_state = DISC_STATE_DISCOVER_SERVICE;
    g_client_ctx.disc_conn = conn;
    memset(&g_client_ctx.handles, 0, sizeof(g_client_ctx.handles));

    /* Set up UUID for service discovery */
    g_client_ctx.uuid.uuid.type = BT_UUID_TYPE_16;
    g_client_ctx.uuid.val = THROUGHPUT_SERVICE_UUID;

    memset(&g_client_ctx.discover_params, 0, sizeof(g_client_ctx.discover_params));
    g_client_ctx.discover_params.uuid = &g_client_ctx.uuid.uuid;
    g_client_ctx.discover_params.func = discover_func;
    g_client_ctx.discover_params.start_handle = 0x0001;
    g_client_ctx.discover_params.end_handle = 0xFFFF;
    g_client_ctx.discover_params.type = BT_GATT_DISCOVER_PRIMARY;

    int err = bt_gatt_discover(conn, &g_client_ctx.discover_params);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Service discovery failed: %d", err);
        g_client_ctx.disc_state = DISC_STATE_IDLE;
        return -1;
    }

    return 0;
}

int throughput_client_enable_notify(struct bt_conn *conn)
{
    if (!g_client_ctx.initialized) {
        return -1;
    }

    if (g_client_ctx.handles.tx_cccd_handle == 0) {
        IPRO_LOGE(LOG_TAG, "CCCD handle not known, run discovery first");
        return -1;
    }

    /* If already subscribed during discovery, just report success */
    if (g_client_ctx.handles.discovery_complete) {
        IPRO_LOGI(LOG_TAG, "Already subscribed to notifications");
        return 0;
    }

    start_subscribe(conn);
    return 0;
}

int throughput_client_write(struct bt_conn *conn,
                           const uint8_t *data,
                           uint16_t length)
{
    if (!g_client_ctx.initialized || !g_client_ctx.handles.discovery_complete) {
        return -1;
    }

    if (g_client_ctx.handles.rx_char_handle == 0) {
        IPRO_LOGE(LOG_TAG, "RX characteristic handle not known");
        return -1;
    }

    if (length > THROUGHPUT_MAX_DATA_LEN) {
        length = THROUGHPUT_MAX_DATA_LEN;
    }

    int err = bt_gatt_write_without_response(conn,
                                              g_client_ctx.handles.rx_char_handle,
                                              data, length, false);
    if (err) {
        return -1;
    }

    return 0;
}

const throughput_client_handles_t *throughput_client_get_handles(void)
{
    return &g_client_ctx.handles;
}

bool throughput_client_is_ready(void)
{
    return g_client_ctx.initialized && g_client_ctx.handles.discovery_complete;
}

int throughput_client_deinit(void)
{
    memset(&g_client_ctx, 0, sizeof(g_client_ctx));
    IPRO_LOGI(LOG_TAG, "Client deinitialized");
    return 0;
}
