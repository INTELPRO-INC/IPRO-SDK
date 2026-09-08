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
 * @file throughput_app.c
 * @brief IPRO BLE Throughput Test Application Implementation
 *
 * Uses native Zephyr bt_* APIs for all BLE operations.
 */

#include "throughput_app.h"
#include "throughput_service.h"
#include "throughput_client.h"
#include "ipro_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

/* Blestack headers */
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>
#include <hci_core.h>
#include <hci_driver_ipro_ble_ctlr.h>
#include <ipro_ble_controller.h>

#define LOG_TAG "TP_APP"

/*
 * CONSTANTS
 ****************************************************************************************
 */

#define THROUGHPUT_DEVICE_NAME      "IPRO-TP-TEST"
#define THROUGHPUT_DEVICE_NAME_LEN  (sizeof(THROUGHPUT_DEVICE_NAME) - 1)
#define THROUGHPUT_DEFAULT_PKT_SIZE 200
#define THROUGHPUT_DEFAULT_DURATION 10

/*
 * ADVERTISING DATA
 ****************************************************************************************
 */

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, THROUGHPUT_DEVICE_NAME, THROUGHPUT_DEVICE_NAME_LEN),
};

/*
 * GLOBAL VARIABLES
 ****************************************************************************************
 */

static throughput_app_ctx_t g_ctx = {0};
static TimerHandle_t g_stats_timer = NULL;
static TaskHandle_t g_tx_task_handle = NULL;
static volatile bool g_tx_task_running = false;
static volatile bool g_print_stats_flag = false;

/* Target address for master mode (stored as bt_addr_le_t) */
static bt_addr_le_t g_target_addr;

/* MTU exchange */
static struct bt_gatt_exchange_params g_mtu_exchange_params;

#define TX_TASK_STACK_SIZE 512
#define TX_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static void service_event_callback(const throughput_svc_event_data_t *event_data, void *user_data);
static void client_event_callback(const throughput_client_event_data_t *event_data, void *user_data);
static void stats_timer_callback(TimerHandle_t timer);

/*
 * HELPER FUNCTIONS
 ****************************************************************************************
 */

static uint32_t get_tick_ms(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

static void update_stats(void)
{
    if (!g_ctx.test_running) return;

    uint32_t now = get_tick_ms();
    g_ctx.stats.duration_ms = now - g_ctx.stats.start_time_ms;

    if (g_ctx.stats.duration_ms > 0) {
        g_ctx.stats.tx_throughput_kbps = (float)g_ctx.stats.tx_bytes * 8.0f / (float)g_ctx.stats.duration_ms;
        g_ctx.stats.rx_throughput_kbps = (float)g_ctx.stats.rx_bytes * 8.0f / (float)g_ctx.stats.duration_ms;
    }
}

/*
 * ZEPHYR CONNECTION CALLBACKS
 ****************************************************************************************
 */

static void mtu_exchange_cb(struct bt_conn *conn, uint8_t err,
                            struct bt_gatt_exchange_params *params)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "MTU exchange failed (err %d)", err);
    } else {
        g_ctx.mtu_size = bt_gatt_get_mtu(conn);
        IPRO_LOGI(LOG_TAG, "MTU exchange complete: MTU=%u", g_ctx.mtu_size);
    }

    /* After MTU exchange, start discovery (master) or mark ready (slave) */
    if (g_ctx.role == THROUGHPUT_ROLE_MASTER) {
        IPRO_LOGI(LOG_TAG, "Starting service discovery...");
        g_ctx.state = THROUGHPUT_STATE_DISCOVERING;
        throughput_client_discover(conn);
    } else {
        g_ctx.state = THROUGHPUT_STATE_READY;
        IPRO_LOGI(LOG_TAG, "Slave ready for throughput test");
    }
}

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err 0x%02x)", err);
        g_ctx.state = THROUGHPUT_STATE_IDLE;
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Connected: %s", addr_str);

    g_ctx.conn = bt_conn_ref(conn);
    g_ctx.state = THROUGHPUT_STATE_CONNECTED;

    /* Get connection info for interval */
    struct bt_conn_info info;
    if (bt_conn_get_info(conn, &info) == 0) {
        g_ctx.conn_interval = info.le.interval;
    }

    /* Request DLE for high throughput */
    IPRO_LOGI(LOG_TAG, "Requesting DLE: tx_octets=251, tx_time=2120us");
    bt_le_set_data_len(conn, 251, 2120);

    /* Exchange MTU for larger packets */
    g_mtu_exchange_params.func = mtu_exchange_cb;
    int ret = bt_gatt_exchange_mtu(conn, &g_mtu_exchange_params);
    if (ret) {
        IPRO_LOGE(LOG_TAG, "MTU exchange request failed: %d", ret);
        /* Fall through - proceed without MTU exchange */
        if (g_ctx.role == THROUGHPUT_ROLE_MASTER) {
            IPRO_LOGI(LOG_TAG, "Starting service discovery...");
            g_ctx.state = THROUGHPUT_STATE_DISCOVERING;
            throughput_client_discover(conn);
        } else {
            g_ctx.state = THROUGHPUT_STATE_READY;
            IPRO_LOGI(LOG_TAG, "Slave ready for throughput test");
        }
    } else {
        IPRO_LOGI(LOG_TAG, "MTU exchange requested, waiting for response...");
    }
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(LOG_TAG, "Disconnected: %s (reason 0x%02x)", addr_str, reason);

    g_ctx.test_running = false;
    g_tx_task_running = false;

    if (g_stats_timer) {
        xTimerStop(g_stats_timer, 0);
    }

    if (g_ctx.conn) {
        bt_conn_unref(g_ctx.conn);
        g_ctx.conn = NULL;
    }

    /* Auto-restart advertising for slave */
    if (g_ctx.role == THROUGHPUT_ROLE_SLAVE) {
        IPRO_LOGI(LOG_TAG, "Restarting advertising...");
        vTaskDelay(pdMS_TO_TICKS(500));
        throughput_app_start_slave();
    } else {
        g_ctx.state = THROUGHPUT_STATE_IDLE;
    }
}

static void le_param_updated_cb(struct bt_conn *conn, uint16_t interval,
                                uint16_t latency, uint16_t timeout)
{
    IPRO_LOGI(LOG_TAG, "Connection params updated: interval=%d latency=%d timeout=%d",
             interval, latency, timeout);
    g_ctx.conn_interval = interval;
}

static void le_phy_updated_cb(struct bt_conn *conn, struct bt_conn_le_phy_info *param)
{
    uint8_t tx_phy = param ? param->tx_phy : 0U;
    uint8_t rx_phy = param ? param->rx_phy : 0U;

    IPRO_LOGI(LOG_TAG, "PHY updated: TX=%d RX=%d", tx_phy, rx_phy);
    g_ctx.phy = tx_phy;
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
    .le_param_updated = le_param_updated_cb,
    .le_phy_updated = le_phy_updated_cb,
};

/*
 * SCAN CALLBACK (Master mode)
 ****************************************************************************************
 */

static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                    struct net_buf_simple *buf)
{
    /* Parse advertising data to find device name */
    const uint8_t *p = buf->data;
    const uint8_t *end = p + buf->len;

    while (p < end) {
        uint8_t len = *p++;
        if (len == 0 || p + len > end) break;

        uint8_t type = *p++;
        len--;

        /* Check for complete or short local name */
        if ((type == BT_DATA_NAME_COMPLETE || type == BT_DATA_NAME_SHORTENED) && len > 0) {
            char name[32] = {0};
            memcpy(name, p, (len < 31) ? len : 31);

            if (strstr(name, "IPRO-TP") != NULL) {
                char addr_str[BT_ADDR_LE_STR_LEN];
                bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
                IPRO_LOGI(LOG_TAG, "Found target: %s RSSI:%d Addr:%s", name, rssi, addr_str);

                g_ctx.target_found = true;

                /* Stop scanning */
                bt_le_scan_stop();

                /* Connect to device */
                g_ctx.state = THROUGHPUT_STATE_CONNECTING;

                struct bt_le_conn_param conn_param = {
                    .interval_min = 6,    /* 7.5ms */
                    .interval_max = 6,    /* 7.5ms */
                    .latency = 0,
                    .timeout = 100,       /* 1s */
                };

                struct bt_conn *conn = NULL;
                int cerr = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN,
                                             &conn_param, &conn);
                if (cerr != 0 || conn == NULL) {
                    IPRO_LOGE(LOG_TAG, "Failed to create connection: %d", cerr);
                    g_ctx.state = THROUGHPUT_STATE_IDLE;
                } else {
                    /* connected_cb will be called on success */
                    bt_conn_unref(conn);
                }
                return;
            }
        }
        p += len;
    }
}

/*
 * SERVICE CALLBACK (Slave side)
 ****************************************************************************************
 */

static void service_event_callback(const throughput_svc_event_data_t *event_data, void *user_data)
{
    (void)user_data;

    switch (event_data->event) {
        case THROUGHPUT_SVC_EVENT_DATA_RECEIVED:
            g_ctx.stats.rx_packets++;
            g_ctx.stats.rx_bytes += event_data->length;
            break;

        case THROUGHPUT_SVC_EVENT_NOTIFY_ENABLED:
            IPRO_LOGI(LOG_TAG, "Notifications enabled by master");
            g_ctx.state = THROUGHPUT_STATE_READY;
            break;

        case THROUGHPUT_SVC_EVENT_NOTIFY_DISABLED:
            IPRO_LOGI(LOG_TAG, "Notifications disabled by master");
            break;

        case THROUGHPUT_SVC_EVENT_NOTIFY_COMPLETE:
            break;

        default:
            break;
    }
}

/*
 * CLIENT CALLBACK (Master side)
 ****************************************************************************************
 */

static void client_event_callback(const throughput_client_event_data_t *event_data, void *user_data)
{
    (void)user_data;

    switch (event_data->event) {
        case THROUGHPUT_CLIENT_EVENT_DISCOVERY_COMPLETE:
            IPRO_LOGI(LOG_TAG, "Service discovery complete");
            throughput_client_enable_notify(g_ctx.conn);
            break;

        case THROUGHPUT_CLIENT_EVENT_DISCOVERY_FAILED:
            IPRO_LOGE(LOG_TAG, "Service discovery failed");
            g_ctx.state = THROUGHPUT_STATE_CONNECTED;
            break;

        case THROUGHPUT_CLIENT_EVENT_NOTIFY_ENABLED:
            IPRO_LOGI(LOG_TAG, "Notifications enabled - ready for test");
            g_ctx.state = THROUGHPUT_STATE_READY;
            break;

        case THROUGHPUT_CLIENT_EVENT_DATA_RECEIVED:
            g_ctx.stats.rx_packets++;
            g_ctx.stats.rx_bytes += event_data->length;
            break;

        case THROUGHPUT_CLIENT_EVENT_WRITE_COMPLETE:
            break;

        default:
            break;
    }
}

/*
 * TX TASK - Dedicated task for sending data
 ****************************************************************************************
 */

static void tx_task(void *param)
{
    (void)param;

    IPRO_LOGI(LOG_TAG, "TX task started, waiting for test to begin...");

    while (1) {
        while (!g_tx_task_running) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        IPRO_LOGI(LOG_TAG, "TX task: test running, starting TX loop (role=%d, dir=%d)",
                 g_ctx.role, g_ctx.test_config.direction);

        uint32_t interval_ms = g_ctx.test_config.interval_ms;
        bool flood_mode = (interval_ms == 0);

        if (flood_mode) {
            IPRO_LOGI(LOG_TAG, "TX flood mode: sending as fast as possible");
        }

        /* Pre-fill TX buffer with pattern (only update seq num per packet) */
        for (int i = 2; i < g_ctx.test_config.packet_size; i++) {
            g_ctx.tx_buffer[i] = (uint8_t)(i & 0xFF);
        }

        uint32_t flood_burst_count = 0;

        while (g_tx_task_running && g_ctx.test_running) {
            /* Check if stats print requested */
            if (g_print_stats_flag) {
                g_print_stats_flag = false;
                update_stats();
                printf("[TP] TX: %lu pkts, %lu bytes, %.1f Kbps | RX: %lu pkts, %lu bytes, %.1f Kbps | Err: %lu\r\n",
                       (unsigned long)g_ctx.stats.tx_packets,
                       (unsigned long)g_ctx.stats.tx_bytes,
                       g_ctx.stats.tx_throughput_kbps,
                       (unsigned long)g_ctx.stats.rx_packets,
                       (unsigned long)g_ctx.stats.rx_bytes,
                       g_ctx.stats.rx_throughput_kbps,
                       (unsigned long)g_ctx.stats.errors);
            }

            /* Check if test duration expired */
            if (g_ctx.test_config.duration_sec > 0) {
                uint32_t elapsed = (get_tick_ms() - g_ctx.stats.start_time_ms) / 1000;
                if (elapsed >= g_ctx.test_config.duration_sec) {
                    g_tx_task_running = false;
                    break;
                }
            }

            /* Send data based on role and direction */
            bool should_tx = (g_ctx.test_config.direction == THROUGHPUT_DIR_TX ||
                              g_ctx.test_config.direction == THROUGHPUT_DIR_BIDIR);

            if (should_tx && g_ctx.conn != NULL) {
                /* Update sequence number only */
                g_ctx.tx_seq_num++;
                g_ctx.tx_buffer[0] = (g_ctx.tx_seq_num >> 8) & 0xFF;
                g_ctx.tx_buffer[1] = g_ctx.tx_seq_num & 0xFF;

                int ret;

                if (g_ctx.role == THROUGHPUT_ROLE_SLAVE) {
                    ret = throughput_service_notify(g_ctx.conn,
                                                   g_ctx.tx_buffer,
                                                   g_ctx.test_config.packet_size);
                } else {
                    ret = throughput_client_write(g_ctx.conn,
                                                 g_ctx.tx_buffer,
                                                 g_ctx.test_config.packet_size);
                }

                if (ret == 0) {
                    g_ctx.stats.tx_packets++;
                    g_ctx.stats.tx_bytes += g_ctx.test_config.packet_size;

                    if (flood_mode) {
                        /* Yield every 8 packets to let shell/other tasks run */
                        if (++flood_burst_count >= 8) {
                            flood_burst_count = 0;
                            taskYIELD();
                        }
                        continue;
                    }
                } else if (ret == -2) {
                    /* Flow control busy (-ENOMEM) - yield briefly and retry */
                    g_ctx.tx_seq_num--;  /* Will retry same seq */
                    vTaskDelay(pdMS_TO_TICKS(1));
                    continue;
                } else {
                    /* Real error */
                    g_ctx.stats.errors++;
                    g_ctx.tx_seq_num--;
                    vTaskDelay(pdMS_TO_TICKS(5));
                    continue;
                }
            }

            if (!flood_mode) {
                vTaskDelay(pdMS_TO_TICKS(interval_ms));
            } else {
                /* Flood mode fallthrough (no conn or not TX dir) */
                vTaskDelay(pdMS_TO_TICKS(1));
            }
        }

        /* Test ended */
        update_stats();
        g_ctx.state = THROUGHPUT_STATE_READY;
        g_ctx.test_running = false;
        xTimerStop(g_stats_timer, 0);

        IPRO_LOGI(LOG_TAG, "Test stopped");
        throughput_app_print_stats();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/*
 * TIMER CALLBACK
 ****************************************************************************************
 */

static void stats_timer_callback(TimerHandle_t timer)
{
    (void)timer;
    if (!g_ctx.test_running) return;
    g_print_stats_flag = true;
}

/*
 * BLE READY CALLBACK
 ****************************************************************************************
 */

static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Bluetooth init failed (err %d)", err);
        return;
    }

    IPRO_LOGI(LOG_TAG, "BLE initialized");

    bt_set_name(THROUGHPUT_DEVICE_NAME);
    bt_conn_cb_register(&conn_callbacks);
}

/*
 * PUBLIC FUNCTIONS
 ****************************************************************************************
 */

int throughput_app_init(void)
{
    IPRO_LOGI(LOG_TAG, "Initializing throughput test application");

    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.state = THROUGHPUT_STATE_IDLE;
    g_ctx.role = THROUGHPUT_ROLE_IDLE;

    /* Default test config */
    g_ctx.test_config.packet_size = THROUGHPUT_DEFAULT_PKT_SIZE;
    g_ctx.test_config.duration_sec = THROUGHPUT_DEFAULT_DURATION;
    g_ctx.test_config.direction = THROUGHPUT_DIR_TX;
    g_ctx.test_config.use_notification = true;
    g_ctx.test_config.interval_ms = 0;

    /* Initialize HCI driver (must be done before bt_enable) */
    int err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", err);
        return -1;
    }

    /* Initialize BLE stack via Zephyr API */
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to initialize BLE: %d", err);
        return -1;
    }

    /* Create timers */
    g_stats_timer = xTimerCreate("tp_stats", pdMS_TO_TICKS(1000), pdTRUE, NULL, stats_timer_callback);

    /* Create TX task */
    if (g_tx_task_handle == NULL) {
        BaseType_t ret = xTaskCreate(tx_task, "tp_tx", TX_TASK_STACK_SIZE, NULL,
                                      TX_TASK_PRIORITY, &g_tx_task_handle);
        if (ret != pdPASS) {
            IPRO_LOGE(LOG_TAG, "Failed to create TX task");
            return -1;
        }
    }

    IPRO_LOGI(LOG_TAG, "Throughput test application initialized");
    return 0;
}

int throughput_app_start_slave(void)
{
    IPRO_LOGI(LOG_TAG, "Starting as slave (peripheral)");

    if (g_ctx.state != THROUGHPUT_STATE_IDLE && g_ctx.state != THROUGHPUT_STATE_READY) {
        throughput_app_stop();
    }

    g_ctx.role = THROUGHPUT_ROLE_SLAVE;

    /* Initialize throughput service (static GATT, just registers callback) */
    throughput_svc_config_t svc_config = {
        .callback = service_event_callback,
        .user_data = NULL
    };

    int ret = throughput_service_init(&svc_config);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to initialize throughput service");
        return -1;
    }

    /* Start advertising using Zephyr API */
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = 32,    /* 20ms (N * 0.625ms) */
        .interval_max = 48,    /* 30ms */
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to start advertising: %d", err);
        return -1;
    }

    g_ctx.state = THROUGHPUT_STATE_ADVERTISING;
    IPRO_LOGI(LOG_TAG, "Slave started - advertising as '%s'", THROUGHPUT_DEVICE_NAME);
    return 0;
}

int throughput_app_start_master(const char *target_addr)
{
    IPRO_LOGI(LOG_TAG, "Starting as master (central)");

    if (g_ctx.state != THROUGHPUT_STATE_IDLE) {
        throughput_app_stop();
    }

    g_ctx.role = THROUGHPUT_ROLE_MASTER;
    g_ctx.target_found = false;
    g_ctx.has_target_addr = false;

    /* Initialize throughput client */
    throughput_client_config_t client_config = {
        .callback = client_event_callback,
        .user_data = NULL
    };

    int ret = throughput_client_init(&client_config);
    if (ret != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to initialize throughput client");
        return -1;
    }

    /* Parse target address if provided */
    if (target_addr != NULL && strlen(target_addr) > 0) {
        bt_addr_t addr;
        if (bt_addr_from_str(target_addr, &addr) == 0) {
            g_target_addr.type = BT_ADDR_LE_PUBLIC;
            memcpy(&g_target_addr.a, &addr, sizeof(bt_addr_t));
            g_ctx.has_target_addr = true;
            IPRO_LOGI(LOG_TAG, "Target address set: %s", target_addr);
        }
    }

    /* Start scanning using Zephyr API */
    struct bt_le_scan_param scan_param = {
        .type = BT_LE_SCAN_TYPE_ACTIVE,
        .options = BT_LE_SCAN_OPT_FILTER_DUPLICATE,
        .interval = 80,     /* 50ms */
        .window = 40,       /* 25ms */
    };

    int err = bt_le_scan_start(&scan_param, scan_cb);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to start scanning: %d", err);
        return -1;
    }

    g_ctx.state = THROUGHPUT_STATE_SCANNING;
    IPRO_LOGI(LOG_TAG, "Master started - scanning for slave");
    return 0;
}

int throughput_app_stop(void)
{
    IPRO_LOGI(LOG_TAG, "Stopping");

    if (g_ctx.test_running) {
        throughput_app_stop_test();
    }

    if (g_ctx.state == THROUGHPUT_STATE_ADVERTISING) {
        bt_le_adv_stop();
    } else if (g_ctx.state == THROUGHPUT_STATE_SCANNING) {
        bt_le_scan_stop();
    }

    if (g_ctx.conn != NULL) {
        bt_conn_disconnect(g_ctx.conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
    }

    g_ctx.state = THROUGHPUT_STATE_IDLE;
    g_ctx.role = THROUGHPUT_ROLE_IDLE;

    return 0;
}

int throughput_app_start_test(const throughput_test_config_t *config)
{
    if (g_ctx.state != THROUGHPUT_STATE_READY) {
        IPRO_LOGE(LOG_TAG, "Not ready for test (state=%d)", g_ctx.state);
        return -1;
    }

    if (g_ctx.role == THROUGHPUT_ROLE_SLAVE) {
        if (!throughput_service_is_notify_enabled()) {
            IPRO_LOGE(LOG_TAG, "Cannot start test - notifications not enabled by client");
            IPRO_LOGI(LOG_TAG, "Please wait for master to enable notifications first");
            return -1;
        }
    }

    if (config != NULL) {
        memcpy(&g_ctx.test_config, config, sizeof(throughput_test_config_t));
    }

    if (g_ctx.test_config.packet_size < 20) {
        g_ctx.test_config.packet_size = 20;
    } else if (g_ctx.test_config.packet_size > THROUGHPUT_MAX_DATA_LEN) {
        g_ctx.test_config.packet_size = THROUGHPUT_MAX_DATA_LEN;
    }

    IPRO_LOGI(LOG_TAG, "Starting throughput test: size=%d, duration=%ds, dir=%d",
             g_ctx.test_config.packet_size,
             g_ctx.test_config.duration_sec,
             g_ctx.test_config.direction);

    memset(&g_ctx.stats, 0, sizeof(throughput_stats_t));
    g_ctx.stats.start_time_ms = get_tick_ms();
    g_ctx.tx_seq_num = 0;

    uint32_t interval_ms = g_ctx.test_config.interval_ms;

    if (interval_ms == 0) {
        IPRO_LOGI(LOG_TAG, "TX interval: flood mode (max throughput)");
    } else {
        IPRO_LOGI(LOG_TAG, "TX interval: %lu ms", (unsigned long)interval_ms);
    }

    xTimerStart(g_stats_timer, 0);

    g_ctx.test_running = true;
    g_ctx.state = THROUGHPUT_STATE_TESTING;
    g_tx_task_running = true;

    IPRO_LOGI(LOG_TAG, "Test started");
    return 0;
}

int throughput_app_stop_test(void)
{
    if (!g_ctx.test_running) {
        return 0;
    }

    g_tx_task_running = false;
    g_ctx.test_running = false;

    xTimerStop(g_stats_timer, 0);
    update_stats();

    g_ctx.state = THROUGHPUT_STATE_READY;

    IPRO_LOGI(LOG_TAG, "Test stopped");
    throughput_app_print_stats();

    return 0;
}

int throughput_app_get_stats(throughput_stats_t *stats)
{
    if (stats == NULL) return -1;

    update_stats();
    memcpy(stats, &g_ctx.stats, sizeof(throughput_stats_t));
    return 0;
}

void throughput_app_print_stats(void)
{
    update_stats();

    printf("\n");
    printf("=== Throughput Test Statistics ===\n");
    printf("Duration: %lu ms\n", (unsigned long)g_ctx.stats.duration_ms);
    printf("TX: %lu packets, %lu bytes, %.2f Kbps (%.2f KBps)\n",
           (unsigned long)g_ctx.stats.tx_packets,
           (unsigned long)g_ctx.stats.tx_bytes,
           g_ctx.stats.tx_throughput_kbps,
           g_ctx.stats.tx_throughput_kbps / 8.0f);
    printf("RX: %lu packets, %lu bytes, %.2f Kbps (%.2f KBps)\n",
           (unsigned long)g_ctx.stats.rx_packets,
           (unsigned long)g_ctx.stats.rx_bytes,
           g_ctx.stats.rx_throughput_kbps,
           g_ctx.stats.rx_throughput_kbps / 8.0f);
    printf("Errors: %lu\n", (unsigned long)g_ctx.stats.errors);
    printf("Connection: interval=%.2fms, PHY=%dM\n",
           g_ctx.conn_interval * 1.25f,
           g_ctx.phy == 2 ? 2 : 1);
    printf("==================================\n");
}

throughput_app_ctx_t *throughput_app_get_ctx(void)
{
    return &g_ctx;
}

int throughput_app_update_conn_params(uint16_t interval_min, uint16_t interval_max,
                                       uint16_t latency, uint16_t timeout)
{
    if (g_ctx.conn == NULL) {
        IPRO_LOGE(LOG_TAG, "Not connected");
        return -1;
    }

    struct bt_le_conn_param param = {
        .interval_min = interval_min,
        .interval_max = interval_max,
        .latency = latency,
        .timeout = timeout
    };

    int err = bt_conn_le_param_update(g_ctx.conn, &param);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to update connection params: %d", err);
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "Requested connection params update: interval=%d-%d, latency=%d, timeout=%d",
             interval_min, interval_max, latency, timeout);
    return 0;
}

int throughput_app_set_tx_interval(uint32_t interval_ms)
{
    if (interval_ms < 1) {
        interval_ms = 1;
    } else if (interval_ms > 1000) {
        interval_ms = 1000;
    }

    g_ctx.test_config.interval_ms = interval_ms;

    IPRO_LOGI(LOG_TAG, "TX interval set to %lu ms", (unsigned long)interval_ms);
    return 0;
}

int throughput_app_set_phy(uint8_t phy)
{
    if (g_ctx.state != THROUGHPUT_STATE_CONNECTED &&
        g_ctx.state != THROUGHPUT_STATE_DISCOVERING &&
        g_ctx.state != THROUGHPUT_STATE_READY) {
        IPRO_LOGE(LOG_TAG, "Not connected (state=%d)", g_ctx.state);
        return -1;
    }

    if (phy < 1 || phy > 3) {
        IPRO_LOGE(LOG_TAG, "Invalid PHY value: %u (must be 1=1M, 2=2M, 3=Coded)", phy);
        return -1;
    }

    /* Map phy value to HCI PHY preferences:
     * 1 = 1M   -> BIT(0)
     * 2 = 2M   -> BIT(1)
     * 3 = Coded -> BIT(2) */
    uint8_t phy_pref;
    switch (phy) {
        case 1: phy_pref = BIT(0); break;  /* 1M */
        case 2: phy_pref = BIT(1); break;  /* 2M */
        case 3: phy_pref = BIT(2); break;  /* Coded */
        default: phy_pref = BIT(0); break;
    }

    const struct bt_conn_le_phy_param phy_param = {
        .options = BT_CONN_LE_PHY_OPT_NONE,
        .pref_tx_phy = phy_pref,
        .pref_rx_phy = phy_pref,
    };
    int err = bt_conn_le_phy_update(g_ctx.conn, &phy_param);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Failed to set PHY: %d", err);
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "PHY update initiated: %s",
              phy == 1 ? "1M" : (phy == 2 ? "2M" : "Coded"));

    return 0;
}

void throughput_app_task(void *arg)
{
    (void)arg;

    IPRO_LOGI(LOG_TAG, "Throughput test task started");

    /* Wait for BLE stack to be ready */
    vTaskDelay(pdMS_TO_TICKS(1000));

    /* Initialize application (calls bt_enable) */
    if (throughput_app_init() != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to initialize application");
        vTaskDelete(NULL);
        return;
    }

    /* Main loop - just keep task alive */
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
