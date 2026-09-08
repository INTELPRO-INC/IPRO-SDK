/**
 * @file image_upload_service.c
 * @brief Image Upload Service Implementation
 *
 * Receives images from phone via BLE and stores in PIR buffer for retrieval.
 *
 * Copyright (C) IPRO SDK 2025
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "image_upload_service.h"
#include "pir_image_service.h"
#include "ipro_log.h"

// Zephyr BLE includes
#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "FreeRTOS.h"
#include "task.h"

#define LOG_TAG "IMG_UPLOAD"
#define INVALID_CONN_ID 0xFF

/*
 * ZEPHYR BLE UUID DEFINITIONS
 ****************************************************************************************
 */

// Image Upload Service UUID (16-bit: 0xFF70)
#define BT_UUID_UPLOAD_SVC_VAL      BT_UUID_16_ENCODE(IMG_UPLOAD_SVC_UUID16)
#define BT_UUID_UPLOAD_SVC          BT_UUID_DECLARE_16(IMG_UPLOAD_SVC_UUID16)

// Upload Control Characteristic UUID (16-bit: 0xFF71)
#define BT_UUID_UPLOAD_CTRL_VAL     BT_UUID_16_ENCODE(UPLOAD_CTRL_CHAR_UUID16)
#define BT_UUID_UPLOAD_CTRL         BT_UUID_DECLARE_16(UPLOAD_CTRL_CHAR_UUID16)

// Upload Data Characteristic UUID (16-bit: 0xFF72)
#define BT_UUID_UPLOAD_DATA_VAL     BT_UUID_16_ENCODE(UPLOAD_DATA_CHAR_UUID16)
#define BT_UUID_UPLOAD_DATA         BT_UUID_DECLARE_16(UPLOAD_DATA_CHAR_UUID16)

// Upload Status Characteristic UUID (16-bit: 0xFF73)
#define BT_UUID_UPLOAD_STATUS_VAL   BT_UUID_16_ENCODE(UPLOAD_STATUS_CHAR_UUID16)
#define BT_UUID_UPLOAD_STATUS       BT_UUID_DECLARE_16(UPLOAD_STATUS_CHAR_UUID16)

/*
 * CONTEXT STRUCTURE
 ****************************************************************************************
 */

typedef struct {
    upload_callback_t callback;
    void *user_data;
    upload_handles_t handles;

    // Connection
    struct bt_conn *conn;
    uint8_t conn_id;

    // State machine
    upload_state_t state;
    uint32_t total_size;
    uint32_t received_size;
    uint16_t chunk_size;
    uint16_t expected_chunk_idx;
    uint16_t total_chunks;

    // Image buffer (allocated on START)
    uint8_t *img_buffer;

    bool initialized;
} upload_ctx_t;

static upload_ctx_t g_upload_ctx = {0};

/*
 * FORWARD DECLARATIONS
 ****************************************************************************************
 */

static void handle_ctrl_write(const uint8_t *data, uint16_t length);
static void handle_data_write(const uint8_t *data, uint16_t length);
static void send_response(upload_resp_t resp, const uint8_t *payload, uint8_t payload_len);
static void reset_upload_state(void);
static void notify_event(upload_event_t event);

/*
 * ZEPHYR GATT CALLBACKS
 ****************************************************************************************
 */

static ssize_t upload_ctrl_write_cb(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr,
                                    const void *buf, uint16_t len,
                                    uint16_t offset, uint8_t flags)
{
    (void)attr;
    (void)flags;

    if (offset != 0) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    // Store connection
    g_upload_ctx.conn = conn;

    handle_ctrl_write(buf, len);

    return len;
}

static ssize_t upload_data_write_cb(struct bt_conn *conn,
                                    const struct bt_gatt_attr *attr,
                                    const void *buf, uint16_t len,
                                    uint16_t offset, uint8_t flags)
{
    (void)attr;
    (void)offset;
    (void)flags;

    // Store connection
    g_upload_ctx.conn = conn;

    handle_data_write(buf, len);

    return len;
}

static ssize_t upload_status_read_cb(struct bt_conn *conn,
                                     const struct bt_gatt_attr *attr,
                                     void *buf, uint16_t len, uint16_t offset)
{
    upload_status_t status;
    image_upload_get_status(&status);

    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             &status, sizeof(status));
}

static void upload_ctrl_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_upload_ctx.handles.ctrl_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "Ctrl notify %s",
              g_upload_ctx.handles.ctrl_notify_enabled ? "enabled" : "disabled");
}

static void upload_status_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_upload_ctx.handles.status_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "Status notify %s",
              g_upload_ctx.handles.status_notify_enabled ? "enabled" : "disabled");
}

/*
 * ZEPHYR GATT SERVICE DEFINITION
 ****************************************************************************************
 */

BT_GATT_SERVICE_DEFINE(upload_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_UPLOAD_SVC),
    // Control characteristic - Write/Notify
    BT_GATT_CHARACTERISTIC(BT_UUID_UPLOAD_CTRL,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_WRITE,
                           NULL, upload_ctrl_write_cb, NULL),
    BT_GATT_CCC(upload_ctrl_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    // Data characteristic - Write Without Response
    BT_GATT_CHARACTERISTIC(BT_UUID_UPLOAD_DATA,
                           BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, upload_data_write_cb, NULL),
    // Status characteristic - Read/Notify
    BT_GATT_CHARACTERISTIC(BT_UUID_UPLOAD_STATUS,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           upload_status_read_cb, NULL, NULL),
    BT_GATT_CCC(upload_status_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/*
 * COMMAND HANDLERS
 ****************************************************************************************
 */

static void handle_ctrl_write(const uint8_t *data, uint16_t length)
{
    if (length < 1) return;

    uint8_t cmd = data[0];
    IPRO_LOGD(LOG_TAG, "Ctrl cmd: 0x%02X, len=%d", cmd, length);

    switch (cmd) {
    case UPLOAD_CMD_START: {
        if (length < 5) {
            IPRO_LOGW(LOG_TAG, "START: invalid length %d", length);
            uint8_t err = UPLOAD_ERR_SIZE;
            send_response(UPLOAD_RESP_ERROR, &err, 1);
            return;
        }

        // Check if PIR transfer is in progress
        if (image_transfer_in_progress()) {
            IPRO_LOGW(LOG_TAG, "START: PIR transfer busy");
            uint8_t err = UPLOAD_ERR_BUSY;
            send_response(UPLOAD_RESP_ERROR, &err, 1);
            return;
        }

        // Parse image size (LE32)
        uint32_t size = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);

        if (size == 0 || size > UPLOAD_IMAGE_MAX_SIZE) {
            IPRO_LOGW(LOG_TAG, "START: invalid size %lu", size);
            uint8_t err = UPLOAD_ERR_SIZE;
            send_response(UPLOAD_RESP_ERROR, &err, 1);
            return;
        }

        // Reset any previous state
        reset_upload_state();

        // Allocate buffer
        g_upload_ctx.img_buffer = (uint8_t *)malloc(size);
        if (!g_upload_ctx.img_buffer) {
            IPRO_LOGE(LOG_TAG, "START: alloc failed for %lu bytes", size);
            uint8_t err = UPLOAD_ERR_ALLOC;
            send_response(UPLOAD_RESP_ERROR, &err, 1);
            return;
        }

        // Initialize state
        g_upload_ctx.total_size = size;
        g_upload_ctx.received_size = 0;
        g_upload_ctx.expected_chunk_idx = 0;
        g_upload_ctx.total_chunks = (size + g_upload_ctx.chunk_size - 1) / g_upload_ctx.chunk_size;
        g_upload_ctx.state = UPLOAD_STATE_RECEIVING;

        IPRO_LOGI(LOG_TAG, "START: size=%lu, chunks=%d, chunk_size=%d",
                  size, g_upload_ctx.total_chunks, g_upload_ctx.chunk_size);

        // Send READY response with suggested chunk size
        uint8_t payload[2] = {
            g_upload_ctx.chunk_size & 0xFF,
            (g_upload_ctx.chunk_size >> 8) & 0xFF
        };
        send_response(UPLOAD_RESP_READY, payload, 2);

        notify_event(UPLOAD_EVENT_STARTED);
        break;
    }

    case UPLOAD_CMD_ABORT: {
        if (g_upload_ctx.state == UPLOAD_STATE_RECEIVING) {
            IPRO_LOGI(LOG_TAG, "ABORT: received");
            reset_upload_state();
            notify_event(UPLOAD_EVENT_ABORTED);
        }
        break;
    }

    default:
        IPRO_LOGW(LOG_TAG, "Unknown command: 0x%02X", cmd);
        break;
    }
}

static void handle_data_write(const uint8_t *data, uint16_t length)
{
    if (g_upload_ctx.state != UPLOAD_STATE_RECEIVING) {
        IPRO_LOGW(LOG_TAG, "DATA: not in receiving state");
        uint8_t err = UPLOAD_ERR_NO_START;
        send_response(UPLOAD_RESP_ERROR, &err, 1);
        return;
    }

    if (length < 3) {  // Minimum: 2 bytes index + 1 byte data
        IPRO_LOGW(LOG_TAG, "DATA: too short (%d)", length);
        return;
    }

    // Parse chunk index (LE16)
    uint16_t chunk_idx = data[0] | (data[1] << 8);
    uint16_t data_len = length - 2;
    const uint8_t *chunk_data = data + 2;

    // Validate chunk index
    if (chunk_idx != g_upload_ctx.expected_chunk_idx) {
        IPRO_LOGW(LOG_TAG, "DATA: wrong index %d, expected %d", chunk_idx, g_upload_ctx.expected_chunk_idx);
        uint8_t err = UPLOAD_ERR_CHUNK_IDX;
        send_response(UPLOAD_RESP_ERROR, &err, 1);
        reset_upload_state();
        return;
    }

    // Calculate offset and validate
    uint32_t offset = (uint32_t)chunk_idx * g_upload_ctx.chunk_size;
    if (offset + data_len > g_upload_ctx.total_size) {
        data_len = g_upload_ctx.total_size - offset;  // Truncate last chunk
    }

    // Copy data to buffer
    memcpy(g_upload_ctx.img_buffer + offset, chunk_data, data_len);
    g_upload_ctx.received_size += data_len;
    g_upload_ctx.expected_chunk_idx++;

    // Log progress every 10 chunks
    if (chunk_idx % 10 == 0 || g_upload_ctx.received_size >= g_upload_ctx.total_size) {
        IPRO_LOGD(LOG_TAG, "DATA: chunk %d/%d, %lu/%lu bytes",
                  chunk_idx + 1, g_upload_ctx.total_chunks,
                  g_upload_ctx.received_size, g_upload_ctx.total_size);
    }

    // Check if complete
    if (g_upload_ctx.received_size >= g_upload_ctx.total_size) {
        IPRO_LOGI(LOG_TAG, "Upload complete: %lu bytes", g_upload_ctx.total_size);

        // Transfer to PIR buffer
        int status = image_set_data(g_upload_ctx.img_buffer, g_upload_ctx.total_size);
        if (status == 0) {
            IPRO_LOGI(LOG_TAG, "Image set to PIR buffer");

            // Notify phone via PIR Image Info
            image_notify_info(g_upload_ctx.conn_id);
        } else {
            IPRO_LOGE(LOG_TAG, "Failed to set PIR buffer: %d", status);
        }

        // Send COMPLETE response
        send_response(UPLOAD_RESP_COMPLETE, NULL, 0);
        notify_event(UPLOAD_EVENT_COMPLETE);

        // Free our buffer (PIR has its own copy)
        free(g_upload_ctx.img_buffer);
        g_upload_ctx.img_buffer = NULL;
        g_upload_ctx.state = UPLOAD_STATE_IDLE;
    }
}

/*
 * HELPER FUNCTIONS
 ****************************************************************************************
 */

static void send_response(upload_resp_t resp, const uint8_t *payload, uint8_t payload_len)
{
    if (!g_upload_ctx.conn || !g_upload_ctx.handles.ctrl_notify_enabled) {
        IPRO_LOGD(LOG_TAG, "Cannot send response: conn=%p, notify=%d",
                  g_upload_ctx.conn, g_upload_ctx.handles.ctrl_notify_enabled);
        return;
    }

    uint8_t buf[16];
    buf[0] = resp;
    if (payload && payload_len > 0 && payload_len < sizeof(buf) - 1) {
        memcpy(buf + 1, payload, payload_len);
    }

    // Use Zephyr bt_gatt_notify - attribute index 2 is the ctrl characteristic value
    int err = bt_gatt_notify(g_upload_ctx.conn, &upload_svc.attrs[2], buf, 1 + payload_len);
    if (err) {
        IPRO_LOGW(LOG_TAG, "Failed to send response 0x%02X: %d", resp, err);
    }
}

static void reset_upload_state(void)
{
    if (g_upload_ctx.img_buffer) {
        free(g_upload_ctx.img_buffer);
        g_upload_ctx.img_buffer = NULL;
    }
    g_upload_ctx.state = UPLOAD_STATE_IDLE;
    g_upload_ctx.total_size = 0;
    g_upload_ctx.received_size = 0;
    g_upload_ctx.expected_chunk_idx = 0;
    g_upload_ctx.total_chunks = 0;
}

static void notify_event(upload_event_t event)
{
    if (!g_upload_ctx.callback) return;

    upload_event_data_t evt = {
        .event = event,
        .conn_id = g_upload_ctx.conn_id
    };

    switch (event) {
    case UPLOAD_EVENT_STARTED:
        evt.data.total_size = g_upload_ctx.total_size;
        break;
    case UPLOAD_EVENT_PROGRESS:
    case UPLOAD_EVENT_COMPLETE:
        evt.data.received_size = g_upload_ctx.received_size;
        break;
    default:
        break;
    }

    g_upload_ctx.callback(&evt, g_upload_ctx.user_data);
}

/*
 * PUBLIC API IMPLEMENTATION
 ****************************************************************************************
 */

int image_upload_service_init(upload_callback_t callback,
                              void *user_data)
{
    if (g_upload_ctx.initialized) {
        IPRO_LOGW(LOG_TAG, "Already initialized");
        return 0;
    }

    memset(&g_upload_ctx, 0, sizeof(g_upload_ctx));
    g_upload_ctx.callback = callback;
    g_upload_ctx.user_data = user_data;
    g_upload_ctx.conn_id = INVALID_CONN_ID;
    g_upload_ctx.state = UPLOAD_STATE_IDLE;
    g_upload_ctx.chunk_size = 240;  // Default for MTU 247

    g_upload_ctx.initialized = true;
    IPRO_LOGI(LOG_TAG, "Image Upload Service initialized (UUID=0x%04X)", IMG_UPLOAD_SVC_UUID16);

    return 0;
}

void image_upload_on_connect(uint8_t conn_id)
{
    g_upload_ctx.conn_id = conn_id;

    // Calculate chunk size based on MTU
    if (g_upload_ctx.conn) {
        uint16_t mtu = bt_gatt_get_mtu(g_upload_ctx.conn);
        // Chunk size = MTU - 3 (ATT header) - 2 (chunk index) - 4 (margin)
        g_upload_ctx.chunk_size = (mtu > 9) ? (mtu - 9) : 240;
    } else {
        g_upload_ctx.chunk_size = 240;  // Default for MTU 247
    }

    IPRO_LOGI(LOG_TAG, "Connected (conn_id=%d, chunk_size=%d)", conn_id, g_upload_ctx.chunk_size);
}

void image_upload_on_disconnect(uint8_t conn_id)
{
    if (g_upload_ctx.conn_id == conn_id) {
        // Abort any ongoing upload
        if (g_upload_ctx.state == UPLOAD_STATE_RECEIVING) {
            IPRO_LOGW(LOG_TAG, "Upload aborted due to disconnect");
            reset_upload_state();
        }
        g_upload_ctx.conn = NULL;
        g_upload_ctx.conn_id = INVALID_CONN_ID;
        g_upload_ctx.handles.ctrl_notify_enabled = false;
        g_upload_ctx.handles.status_notify_enabled = false;
    }
}

int image_upload_get_status(upload_status_t *status)
{
    if (!status) {
        return -EINVAL;
    }

    status->state = (uint8_t)g_upload_ctx.state;
    status->total_size = g_upload_ctx.total_size;
    status->received_size = g_upload_ctx.received_size;
    status->chunk_count = g_upload_ctx.total_chunks;
    status->chunks_received = g_upload_ctx.expected_chunk_idx;

    return 0;
}

const upload_handles_t *image_upload_get_handles(void)
{
    return &g_upload_ctx.handles;
}

void image_upload_set_conn(struct bt_conn *conn)
{
    g_upload_ctx.conn = conn;
    if (conn) {
        g_upload_ctx.conn_id = bt_conn_index(conn);
        // Update chunk size based on MTU
        uint16_t mtu = bt_gatt_get_mtu(conn);
        g_upload_ctx.chunk_size = (mtu > 9) ? (mtu - 9) : 240;
        IPRO_LOGI(LOG_TAG, "Connection set (MTU=%d, chunk_size=%d)", mtu, g_upload_ctx.chunk_size);
    } else {
        if (g_upload_ctx.state == UPLOAD_STATE_RECEIVING) {
            IPRO_LOGW(LOG_TAG, "Connection lost during upload, aborting...");
            reset_upload_state();
        }
        g_upload_ctx.conn_id = INVALID_CONN_ID;
        g_upload_ctx.handles.ctrl_notify_enabled = false;
        g_upload_ctx.handles.status_notify_enabled = false;
    }
}
