/**
 * @file pir_image_service.c
 * @brief PIR Motion Detection + Image Transfer Service Implementation
 *
 * Features:
 * - PIR sensor GPIO interrupt detection
 * - Motion event notification to phone
 * - Image capture and transfer over BLE
 * - Chunked data transfer protocol for large images
 *
 * Copyright (C) IPRO SDK 2025
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "pir_image_service.h"
#include "ipro_log.h"

#include "FreeRTOS.h"
#include "task.h"

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_TAG "PIR_IMAGE"
#define INVALID_CONN_ID 0xFF

/*
 * TX TASK CONFIGURATION
 ****************************************************************************************
 */
#define IMG_TX_TASK_STACK_SIZE 1024
#define IMG_TX_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)

/*
 * UUID DEFINITIONS
 ****************************************************************************************
 */

/* PIR Image Service UUID: 0xFF50 */
#define BT_UUID_PIR_IMG_SVC_VAL     BT_UUID_16_ENCODE(PIR_IMG_SVC_UUID16)
#define BT_UUID_PIR_IMG_SVC         BT_UUID_DECLARE_16(PIR_IMG_SVC_UUID16)
#define BT_UUID_PIR_STATUS          BT_UUID_DECLARE_16(PIR_STATUS_CHAR_UUID16)
#define BT_UUID_IMG_CTRL            BT_UUID_DECLARE_16(IMG_CTRL_CHAR_UUID16)
#define BT_UUID_IMG_DATA            BT_UUID_DECLARE_16(IMG_DATA_CHAR_UUID16)
#define BT_UUID_IMG_INFO            BT_UUID_DECLARE_16(IMG_INFO_CHAR_UUID16)

/**
 * Demo JPEG images - alternates between demo1 and demo2 on each capture
 */
#include "demo1_array.h"
#include "demo2_array.h"

// Image sequence counter (0 = demo1, 1 = demo2)
static uint8_t g_image_sequence = 0;

/*
 * CONTEXT STRUCTURE
 ****************************************************************************************
 */

/// PIR Image service context
typedef struct {
    pir_img_callback_t callback;
    void *user_data;
    pir_img_handles_t handles;
    pir_status_t pir_status;
    struct bt_conn *conn;

    // Image management
    uint8_t *img_buffer;
    uint32_t img_size;
    uint16_t img_chunk_count;
    img_state_t img_state;
    bool img_buffer_is_static;
    uint8_t current_image_index;

    // Transfer management
    uint16_t current_chunk;
    uint8_t transfer_conn_id;
    bool auto_transfer;
    bool notify_pending;
    bool initialized;
    uint16_t chunk_size;

    // Transfer rate control
    uint32_t last_chunk_tick;
    uint16_t chunk_interval_ms;
    uint32_t transfer_start_tick;

    // Transfer mode control
    img_transfer_mode_t transfer_mode;
    uint16_t phone_img_rx_handle;

    // TX task management
    TaskHandle_t tx_task_handle;
    bool tx_task_running;
} pir_ctx_t;

/// Global context
static pir_ctx_t g_pir_ctx = {0};

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static void image_tx_task(void *param);

/*
 * GATT CALLBACKS
 ****************************************************************************************
 */

/* PIR Status Read */
static ssize_t pir_status_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                               void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &g_pir_ctx.pir_status, sizeof(pir_status_t));
}

/* PIR Status CCC Changed */
static void pir_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_pir_ctx.handles.pir_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[PIR] Status notify: %s",
              g_pir_ctx.handles.pir_notify_enabled ? "ON" : "OFF");

    if (g_pir_ctx.callback) {
        pir_img_event_data_t evt = {
            .event = g_pir_ctx.handles.pir_notify_enabled ?
                     PIR_IMG_EVENT_NOTIFY_ENABLED : PIR_IMG_EVENT_NOTIFY_DISABLED,
            .conn_id = g_pir_ctx.transfer_conn_id
        };
        g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
    }
}

/* Image Control Write */
static ssize_t img_ctrl_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                              const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (offset != 0 || len < 1) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    const uint8_t *data = buf;
    img_ctrl_cmd_t cmd = (img_ctrl_cmd_t)data[0];

    IPRO_LOGI(LOG_TAG, "[IMG] Cmd: 0x%02X", cmd);

    switch (cmd) {
        case IMG_CTRL_CAPTURE:
            IPRO_LOGI(LOG_TAG, "[IMG] Capture");
            if (g_pir_ctx.callback) {
                pir_img_event_data_t evt = {
                    .event = PIR_IMG_EVENT_CAPTURE_REQUEST,
                    .conn_id = g_pir_ctx.transfer_conn_id
                };
                g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
            }
            break;

        case IMG_CTRL_REQUEST_CHUNK:
            if (len >= 3) {
                uint16_t chunk_idx = (data[2] << 8) | data[1];
                IPRO_LOGI(LOG_TAG, "[IMG] Request chunk #%d", chunk_idx);
                image_send_chunk(g_pir_ctx.transfer_conn_id, chunk_idx);
            }
            break;

        case IMG_CTRL_ABORT:
            IPRO_LOGI(LOG_TAG, "[IMG] Abort");
            image_abort_transfer();
            break;

        default:
            IPRO_LOGW(LOG_TAG, "Unknown control command: 0x%02X", cmd);
            break;
    }

    return len;
}

/* Image Data CCC Changed */
static void img_data_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_pir_ctx.handles.img_data_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[IMG] Data notify: %s",
              g_pir_ctx.handles.img_data_notify_enabled ? "ON" : "OFF");
}

/* Image Info Read */
static ssize_t img_info_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             void *buf, uint16_t len, uint16_t offset)
{
    img_info_t info;
    info.image_size = g_pir_ctx.img_size;
    info.chunk_count = g_pir_ctx.img_chunk_count;
    info.chunk_size = g_pir_ctx.chunk_size;
    info.format = 0;  // JPEG
    info.status = (uint8_t)g_pir_ctx.img_state;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &info, sizeof(img_info_t));
}

/* Image Info CCC Changed */
static void img_info_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_pir_ctx.handles.img_info_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[IMG] Info notify: %s",
              g_pir_ctx.handles.img_info_notify_enabled ? "ON" : "OFF");
}

/*
 * GATT SERVICE DEFINITION
 ****************************************************************************************
 */

BT_GATT_SERVICE_DEFINE(pir_img_svc,
    /* Primary Service Declaration */
    BT_GATT_PRIMARY_SERVICE(BT_UUID_PIR_IMG_SVC),

    /* PIR Status Characteristic - Read/Notify */
    BT_GATT_CHARACTERISTIC(BT_UUID_PIR_STATUS,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           pir_status_read, NULL, NULL),
    BT_GATT_CCC(pir_status_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Image Control Characteristic - Write Without Response */
    BT_GATT_CHARACTERISTIC(BT_UUID_IMG_CTRL,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, img_ctrl_write, NULL),

    /* Image Data Characteristic - Notify */
    BT_GATT_CHARACTERISTIC(BT_UUID_IMG_DATA,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(img_data_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Image Info Characteristic - Read/Notify */
    BT_GATT_CHARACTERISTIC(BT_UUID_IMG_INFO,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           img_info_read, NULL, NULL),
    BT_GATT_CCC(img_info_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/*
 * HELPER FUNCTIONS
 ****************************************************************************************
 */

void image_set_type(pir_image_type_t type)
{
    (void)type;
}

pir_image_type_t image_get_type(void)
{
    return PIR_IMAGE_TYPE_CAT;
}

int image_capture(void)
{
    // If uploaded image is ready, use it
    if (g_pir_ctx.img_state == IMG_STATE_READY && !g_pir_ctx.img_buffer_is_static) {
        IPRO_LOGI(LOG_TAG, "[IMG] Using uploaded image: %lu bytes", g_pir_ctx.img_size);
        if (g_pir_ctx.conn && g_pir_ctx.handles.img_data_notify_enabled) {
            IPRO_LOGI(LOG_TAG, "[IMG] Auto-starting transfer");
            image_start_transfer(g_pir_ctx.transfer_conn_id);
        }
        return 0;
    }

    // Alternate between demo1 and demo2
    const uint8_t *image_data;
    uint32_t image_size;
    const char *image_name;

    if (g_image_sequence == 0) {
        image_data = (const uint8_t *)demo1_jpg;
        image_size = demo1_jpg_len;
        image_name = "demo1";
    } else {
        image_data = (const uint8_t *)demo2_jpg;
        image_size = demo2_jpg_len;
        image_name = "demo2";
    }

    g_image_sequence = (g_image_sequence + 1) % 2;

    IPRO_LOGI(LOG_TAG, "[IMG] Capture %s: %lu bytes", image_name, image_size);

    g_pir_ctx.img_state = IMG_STATE_CAPTURING;

    // Free existing buffer only if dynamically allocated
    if (g_pir_ctx.img_buffer && !g_pir_ctx.img_buffer_is_static) {
        free(g_pir_ctx.img_buffer);
        g_pir_ctx.img_buffer = NULL;
    }

    // Use static array directly
    g_pir_ctx.img_buffer = (uint8_t *)image_data;
    g_pir_ctx.img_buffer_is_static = true;
    g_pir_ctx.img_size = image_size;

    // Calculate chunk count
    g_pir_ctx.img_chunk_count = (g_pir_ctx.img_size + g_pir_ctx.chunk_size - 1) / g_pir_ctx.chunk_size;
    g_pir_ctx.current_chunk = 0;
    g_pir_ctx.img_state = IMG_STATE_READY;

    IPRO_LOGI(LOG_TAG, "[IMG] Ready: %lu bytes, %d chunks",
              g_pir_ctx.img_size, g_pir_ctx.img_chunk_count);

    // Auto-start transfer if connection ready and notifications enabled
    if (g_pir_ctx.conn && g_pir_ctx.handles.img_data_notify_enabled) {
        IPRO_LOGI(LOG_TAG, "[IMG] Auto-starting transfer");
        image_start_transfer(g_pir_ctx.transfer_conn_id);
    }

    return 0;
}

int image_set_data(const uint8_t *data, uint32_t size)
{
    if (!data || size == 0 || size > IMAGE_MAX_SIZE) {
        return -1;
    }

    if (g_pir_ctx.img_buffer && !g_pir_ctx.img_buffer_is_static) {
        free(g_pir_ctx.img_buffer);
    }
    g_pir_ctx.img_buffer = NULL;

    g_pir_ctx.img_buffer = (uint8_t *)malloc(size);
    if (!g_pir_ctx.img_buffer) {
        return -2;
    }

    memcpy(g_pir_ctx.img_buffer, data, size);
    g_pir_ctx.img_buffer_is_static = false;
    g_pir_ctx.img_size = size;
    g_pir_ctx.img_chunk_count = (size + g_pir_ctx.chunk_size - 1) / g_pir_ctx.chunk_size;
    g_pir_ctx.current_chunk = 0;
    g_pir_ctx.img_state = IMG_STATE_READY;

    IPRO_LOGI(LOG_TAG, "[IMG] Data set: %lu bytes, %d chunks", size, g_pir_ctx.img_chunk_count);
    return 0;
}

int image_get_info(img_info_t *info)
{
    if (!info) {
        return -1;
    }

    info->image_size = g_pir_ctx.img_size;
    info->chunk_count = g_pir_ctx.img_chunk_count;
    info->chunk_size = g_pir_ctx.chunk_size;
    info->format = 0;  // JPEG
    info->status = (uint8_t)g_pir_ctx.img_state;

    return 0;
}

int image_notify_info(uint8_t conn_id)
{
    (void)conn_id;

    if (!g_pir_ctx.initialized || !g_pir_ctx.handles.img_info_notify_enabled || !g_pir_ctx.conn) {
        return -1;
    }

    img_info_t info;
    image_get_info(&info);

    /* Get attribute for Image Info characteristic value */
    const struct bt_gatt_attr *attr = &pir_img_svc.attrs[10];  /* Index of IMG_INFO value */

    return bt_gatt_notify(g_pir_ctx.conn, attr, &info, sizeof(img_info_t));
}

int image_send_chunk(uint8_t conn_id, uint16_t chunk_index)
{
    (void)conn_id;

    if (!g_pir_ctx.initialized || !g_pir_ctx.conn) {
        return -1;
    }

    if (!g_pir_ctx.handles.img_data_notify_enabled) {
        return -1;
    }

    if (g_pir_ctx.img_state != IMG_STATE_READY &&
        g_pir_ctx.img_state != IMG_STATE_TRANSFERRING) {
        return -1;
    }

    if (chunk_index >= g_pir_ctx.img_chunk_count) {
        IPRO_LOGW(LOG_TAG, "Invalid chunk index: %d (max: %d)",
                  chunk_index, g_pir_ctx.img_chunk_count - 1);
        return -1;
    }

    // Calculate chunk offset and size
    uint32_t offset = chunk_index * g_pir_ctx.chunk_size;
    uint16_t chunk_size = g_pir_ctx.chunk_size;

    // Last chunk may be smaller
    if (offset + chunk_size > g_pir_ctx.img_size) {
        chunk_size = g_pir_ctx.img_size - offset;
    }

    // Build chunk packet (header + data)
    uint8_t packet[4 + MAX_IMAGE_CHUNK_SIZE];
    img_chunk_header_t *header = (img_chunk_header_t *)packet;
    header->chunk_index = chunk_index;
    header->chunk_size = chunk_size;
    memcpy(header->data, g_pir_ctx.img_buffer + offset, chunk_size);

    uint16_t packet_len = 4 + chunk_size;

    /* Get attribute for Image Data characteristic value */
    const struct bt_gatt_attr *attr = &pir_img_svc.attrs[7];  /* Index of IMG_DATA value */

    int ret = bt_gatt_notify(g_pir_ctx.conn, attr, packet, packet_len);

    if (ret == -ENOMEM) {
        return -2;  // Flow control busy
    } else if (ret != 0) {
        IPRO_LOGW(LOG_TAG, "Failed to send chunk %d: %d", chunk_index, ret);
        return -1;
    }

    IPRO_LOGD(LOG_TAG, "Chunk %d/%d sent (%d bytes)",
              chunk_index + 1, g_pir_ctx.img_chunk_count, chunk_size);

    return 0;
}

int image_start_transfer(uint8_t conn_id)
{
    IPRO_LOGI(LOG_TAG, "[IMG] Start: conn=%d, state=%d", conn_id, g_pir_ctx.img_state);

    if (g_pir_ctx.img_state != IMG_STATE_READY) {
        IPRO_LOGW(LOG_TAG, "[IMG] Not ready");
        return -1;
    }

    if (!g_pir_ctx.handles.img_data_notify_enabled) {
        IPRO_LOGE(LOG_TAG, "[IMG] Notifications not enabled");
        return -1;
    }

    g_pir_ctx.transfer_conn_id = conn_id;
    g_pir_ctx.current_chunk = 0;
    g_pir_ctx.auto_transfer = true;
    g_pir_ctx.img_state = IMG_STATE_TRANSFERRING;
    g_pir_ctx.transfer_start_tick = xTaskGetTickCount();
    g_pir_ctx.last_chunk_tick = g_pir_ctx.transfer_start_tick;

    IPRO_LOGI(LOG_TAG, "[IMG] Transfer: %lu bytes, %d chunks",
              g_pir_ctx.img_size, g_pir_ctx.img_chunk_count);

    // Notify info first
    image_notify_info(conn_id);

    // Start TX task
    g_pir_ctx.tx_task_running = true;

    // Callback
    if (g_pir_ctx.callback) {
        pir_img_event_data_t evt = {
            .event = PIR_IMG_EVENT_TRANSFER_START,
            .conn_id = conn_id
        };
        g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
    }

    return 0;
}

int image_abort_transfer(void)
{
    if (g_pir_ctx.img_state == IMG_STATE_TRANSFERRING) {
        g_pir_ctx.auto_transfer = false;
        g_pir_ctx.tx_task_running = false;
        g_pir_ctx.img_state = IMG_STATE_READY;

        IPRO_LOGI(LOG_TAG, "[IMG] Aborted at chunk %d", g_pir_ctx.current_chunk);

        if (g_pir_ctx.callback) {
            pir_img_event_data_t evt = {
                .event = PIR_IMG_EVENT_TRANSFER_ABORT,
                .conn_id = g_pir_ctx.transfer_conn_id
            };
            g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
        }
    }

    return 0;
}

/*
 * TX TASK - Dedicated task for sending image chunks
 ****************************************************************************************
 */

static void image_tx_task(void *param)
{
    (void)param;

    while (1) {
        // Wait for transfer to start
        while (!g_pir_ctx.tx_task_running) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }

        // Transfer loop — flood-mode burst sending for max throughput
        uint32_t burst_count = 0;
        while (g_pir_ctx.tx_task_running && g_pir_ctx.auto_transfer) {
            if (g_pir_ctx.current_chunk < g_pir_ctx.img_chunk_count) {
                int ret = image_send_chunk(g_pir_ctx.transfer_conn_id, g_pir_ctx.current_chunk);

                if (ret == 0) {
                    g_pir_ctx.current_chunk++;

                    if (g_pir_ctx.current_chunk % 10 == 0) {
                        IPRO_LOGI(LOG_TAG, "[IMG] %d/%d chunks",
                                 g_pir_ctx.current_chunk, g_pir_ctx.img_chunk_count);
                    }

                    /* Burst mode: send up to 8 packets, then delay 1ms */
                    if (++burst_count >= 8) {
                        burst_count = 0;
                        vTaskDelay(pdMS_TO_TICKS(1));
                    }
                } else if (ret == -2) {
                    /* Flow control busy (ENOMEM) — wait 1ms for BLE controller to drain */
                    vTaskDelay(pdMS_TO_TICKS(1));
                } else {
                    IPRO_LOGE(LOG_TAG, "[IMG] Failed at chunk %d", g_pir_ctx.current_chunk);
                    image_abort_transfer();
                    break;
                }
            } else {
                // Transfer complete
                g_pir_ctx.auto_transfer = false;
                g_pir_ctx.tx_task_running = false;
                g_pir_ctx.img_state = IMG_STATE_READY;

                uint32_t end_tick = xTaskGetTickCount();
                uint32_t duration_ms = (end_tick - g_pir_ctx.transfer_start_tick) * portTICK_PERIOD_MS;
                uint32_t bytes_per_sec = (duration_ms > 0) ? (g_pir_ctx.img_size * 1000 / duration_ms) : 0;
                uint32_t bits_per_sec = bytes_per_sec * 8;

                IPRO_LOGI(LOG_TAG, "[IMG] Complete: %d chunks, %lu bytes in %lu ms (%.1f KB/s, %lu Kbps)",
                         g_pir_ctx.img_chunk_count, g_pir_ctx.img_size, duration_ms,
                         bytes_per_sec / 1024.0f, bits_per_sec / 1000);

                if (g_pir_ctx.callback) {
                    pir_img_event_data_t evt = {
                        .event = PIR_IMG_EVENT_TRANSFER_COMPLETE,
                        .conn_id = g_pir_ctx.transfer_conn_id
                    };
                    g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
                }
            }
        }

        g_pir_ctx.tx_task_running = false;
    }
}

void image_transfer_process(void)
{
    // Now handled by dedicated TX task
}

bool image_transfer_in_progress(void)
{
    return g_pir_ctx.img_state == IMG_STATE_TRANSFERRING;
}

/*
 * PIR SENSOR FUNCTIONS
 ****************************************************************************************
 */

int pir_get_status(pir_status_t *status)
{
    if (!status) {
        return -1;
    }

    *status = g_pir_ctx.pir_status;
    return 0;
}

int pir_notify_motion(uint8_t conn_id, const pir_status_t *status)
{
    (void)conn_id;

    if (!g_pir_ctx.initialized || !g_pir_ctx.handles.pir_notify_enabled || !g_pir_ctx.conn) {
        return -1;
    }

    if (!status) {
        return -1;
    }

    IPRO_LOGD(LOG_TAG, "PIR notification: motion=%d, count=%lu",
              status->motion_detected, status->event_count);

    /* Get attribute for PIR Status characteristic value */
    const struct bt_gatt_attr *attr = &pir_img_svc.attrs[1];  /* Index of PIR_STATUS value */

    int ret = bt_gatt_notify(g_pir_ctx.conn, attr, status, sizeof(pir_status_t));

    if (ret != 0) {
        IPRO_LOGW(LOG_TAG, "[PIR] Notification failed: %d", ret);
    }

    return ret;
}

int pir_image_set_transfer_rate(uint16_t conn_interval_ms)
{
    if (!g_pir_ctx.initialized) {
        return -1;
    }

    if (conn_interval_ms < 7 || conn_interval_ms > 4000) {
        IPRO_LOGW(LOG_TAG, "[IMG] Invalid connection interval: %d ms", conn_interval_ms);
        return -1;
    }

    uint16_t target_interval = (conn_interval_ms * 3) / 10;

    if (target_interval < 7) {
        target_interval = 7;
    } else if (target_interval > 50) {
        target_interval = 50;
    }

    g_pir_ctx.chunk_interval_ms = target_interval;

    return 0;
}

int pir_image_set_transfer_mode(img_transfer_mode_t mode, uint16_t phone_char_handle)
{
    if (!g_pir_ctx.initialized) {
        return -1;
    }

    if (mode != IMG_TRANSFER_NOTIFICATION && mode != IMG_TRANSFER_WRITE_NO_RESP) {
        IPRO_LOGW(LOG_TAG, "[IMG] Invalid transfer mode: %d", mode);
        return -1;
    }

    if (mode == IMG_TRANSFER_WRITE_NO_RESP && phone_char_handle == 0) {
        IPRO_LOGW(LOG_TAG, "[IMG] Write mode requires valid handle");
        return -1;
    }

    g_pir_ctx.transfer_mode = mode;
    g_pir_ctx.phone_img_rx_handle = phone_char_handle;

    IPRO_LOGI(LOG_TAG, "[IMG] Mode: %s",
              mode == IMG_TRANSFER_NOTIFICATION ? "NOTIFICATION" : "WRITE");

    return 0;
}

int pir_image_set_conn_id(uint8_t conn_id)
{
    g_pir_ctx.transfer_conn_id = conn_id;

    if (conn_id < 0xF0) {
        g_pir_ctx.handles.pir_notify_enabled = true;
        g_pir_ctx.handles.img_data_notify_enabled = true;
        g_pir_ctx.handles.img_info_notify_enabled = true;
    } else {
        g_pir_ctx.handles.pir_notify_enabled = false;
        g_pir_ctx.handles.img_data_notify_enabled = false;
        g_pir_ctx.handles.img_info_notify_enabled = false;
    }

    IPRO_LOGI(LOG_TAG, "[PIR] conn_id=%d, notify=%d",
              conn_id, g_pir_ctx.handles.pir_notify_enabled);
    return 0;
}

int pir_simulate_trigger(void)
{
    if (!g_pir_ctx.initialized) {
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "[PIR] Simulated trigger");

    g_pir_ctx.pir_status.motion_detected = 1;
    g_pir_ctx.pir_status.timestamp = xTaskGetTickCount();
    g_pir_ctx.pir_status.event_count++;

    if (g_pir_ctx.conn) {
        pir_notify_motion(g_pir_ctx.transfer_conn_id, &g_pir_ctx.pir_status);
    }

    if (g_pir_ctx.callback) {
        pir_img_event_data_t evt = {
            .event = PIR_IMG_EVENT_MOTION_DETECTED,
            .conn_id = g_pir_ctx.transfer_conn_id,
            .data.pir = g_pir_ctx.pir_status
        };
        g_pir_ctx.callback(&evt, g_pir_ctx.user_data);
    }

    // Auto capture on motion
    if (g_pir_ctx.img_state == IMG_STATE_IDLE || g_pir_ctx.img_state == IMG_STATE_READY) {
        IPRO_LOGI(LOG_TAG, "[PIR] Auto-capture");
        image_capture();
    }

    return 0;
}

/*
 * PUBLIC API FUNCTIONS
 ****************************************************************************************
 */

int pir_image_service_init(pir_img_callback_t callback, void *user_data)
{
    if (g_pir_ctx.initialized) {
        IPRO_LOGW(LOG_TAG, "PIR Image service already initialized");
        return 0;
    }

    IPRO_LOGI(LOG_TAG, "[PIR] Initializing service...");

    memset(&g_pir_ctx, 0, sizeof(g_pir_ctx));
    g_pir_ctx.callback = callback;
    g_pir_ctx.user_data = user_data;
    g_pir_ctx.img_state = IMG_STATE_IDLE;
    g_pir_ctx.transfer_conn_id = INVALID_CONN_ID;
    g_pir_ctx.chunk_interval_ms = 15;
    g_pir_ctx.chunk_size = 240;  // Default for MTU 247
    g_pir_ctx.transfer_mode = IMG_TRANSFER_NOTIFICATION;

    // Create TX task
    if (g_pir_ctx.tx_task_handle == NULL) {
        g_pir_ctx.tx_task_running = false;
        BaseType_t ret = xTaskCreate(image_tx_task, "img_tx",
                                      IMG_TX_TASK_STACK_SIZE, NULL,
                                      IMG_TX_TASK_PRIORITY, &g_pir_ctx.tx_task_handle);
        if (ret != pdPASS) {
            IPRO_LOGE(LOG_TAG, "[IMG] TX task creation failed");
            return -1;
        }
    }

    g_pir_ctx.initialized = true;
    IPRO_LOGI(LOG_TAG, "[PIR] Service ready");

    return 0;
}

int pir_image_service_deinit(void)
{
    if (!g_pir_ctx.initialized) {
        return 0;
    }

    // Stop TX task
    if (g_pir_ctx.tx_task_handle) {
        g_pir_ctx.tx_task_running = false;
        vTaskDelay(pdMS_TO_TICKS(50));
        vTaskDelete(g_pir_ctx.tx_task_handle);
        g_pir_ctx.tx_task_handle = NULL;
    }

    // Free image buffer if allocated
    if (g_pir_ctx.img_buffer && !g_pir_ctx.img_buffer_is_static) {
        free(g_pir_ctx.img_buffer);
    }

    memset(&g_pir_ctx, 0, sizeof(g_pir_ctx));

    IPRO_LOGI(LOG_TAG, "[PIR] Service deinitialized");

    return 0;
}

const pir_img_handles_t *pir_image_service_get_handles(void)
{
    if (!g_pir_ctx.initialized) {
        return NULL;
    }

    return &g_pir_ctx.handles;
}

int pir_sensor_start(uint8_t gpio_pin)
{
    if (!g_pir_ctx.initialized) {
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "[PIR] Sensor started (GPIO %d)", gpio_pin);
    return 0;
}

int pir_sensor_stop(void)
{
    if (!g_pir_ctx.initialized) {
        return -1;
    }

    IPRO_LOGI(LOG_TAG, "[PIR] Sensor stopped");
    g_pir_ctx.pir_status.motion_detected = 0;

    return 0;
}

/**
 * @brief Set connection reference for notifications
 */
void pir_image_service_set_conn(struct bt_conn *conn)
{
    g_pir_ctx.conn = conn;
    if (conn) {
        g_pir_ctx.transfer_conn_id = 0;  // First connection
    } else {
        g_pir_ctx.transfer_conn_id = INVALID_CONN_ID;
        // Abort any ongoing transfer
        if (g_pir_ctx.img_state == IMG_STATE_TRANSFERRING) {
            image_abort_transfer();
        }
    }
}
