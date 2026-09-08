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
 * @file ble_audio.c
 * @brief BLE NUS service with audio command protocol
 *
 * Provides a Nordic UART Service (NUS) over BLE for phone control of the
 * PDM recorder. Commands arrive on the RX characteristic; responses and
 * audio data are sent as notifications on the TX characteristic.
 *
 * Audio transfer uses a dedicated FreeRTOS task that chunks PCM data
 * into MTU-sized BLE notifications.
 */

#include "ble_audio.h"
#include "ipro_log.h"

#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <lc3.h>

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

#define TAG "BLE_AUDIO"

#define DEVICE_NAME         "IPRO-MIC"
#define ADV_INT_MIN         160     /* 100ms in 0.625ms units */
#define ADV_INT_MAX         320     /* 200ms */

/* Audio send task */
#define SEND_TASK_STACK     1024
#define SEND_TASK_PRIO      8

/* Stream send task */
#define STREAM_TASK_STACK   1536
#define STREAM_TASK_PRIO    9   /* higher than bulk send */
#define FRAME_SAMPLES       320
#define FRAME_BYTES         (FRAME_SAMPLES * sizeof(int16_t))  /* 640 */

/* LC3 encoder: 10ms frames, 16kHz, 40 bytes output = 32 kbps */
#define LC3_FRAME_US        10000
#define LC3_SAMPLE_RATE     16000
#define LC3_FRAME_SAMPLES   160   /* 10ms at 16kHz */
#define LC3_OCTETS          40    /* 32 kbps target */

static lc3_encoder_mem_16k_t s_enc_mem;
static lc3_encoder_t s_encoder;
static bool s_encoder_init;
static bool s_use_lc3 = true;  /* toggle via shell: pdm_lc3 on|off */

/* --- NUS UUIDs (same as ipro_ble_transparent) --- */
static struct bt_uuid_128 nus_svc_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_rx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E);

static struct bt_uuid_128 nus_tx_uuid = BT_UUID_INIT_128(
    0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
    0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E);

/* --- State --- */
static struct {
    struct bt_conn *conn;
    bool tx_notify_enabled;
    const ble_audio_recorder_t *recorder;
} s_ble;

/* Audio send task */
static TaskHandle_t s_send_task_handle;
static SemaphoreHandle_t s_send_sem;

/* --- Forward declarations --- */
static void start_advertising(void);
static void handle_command(const uint8_t *data, uint16_t len);

/* --- GATT callbacks --- */
static ssize_t nus_rx_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    if (len == 0)
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);

    handle_command((const uint8_t *)buf, len);
    return len;
}

static void nus_tx_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    s_ble.tx_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(TAG, "TX notify %s", s_ble.tx_notify_enabled ? "enabled" : "disabled");
}

/* --- NUS Service definition --- */
BT_GATT_SERVICE_DEFINE(nus_svc,
    BT_GATT_PRIMARY_SERVICE(&nus_svc_uuid),
    /* TX characteristic (device → phone, notify) */
    BT_GATT_CHARACTERISTIC(&nus_tx_uuid.uuid,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(nus_tx_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    /* RX characteristic (phone → device, write) */
    BT_GATT_CHARACTERISTIC(&nus_rx_uuid.uuid,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, nus_rx_write, NULL),
);

/* --- Advertising data --- */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, sizeof(DEVICE_NAME) - 1),
};

static const struct bt_data sd[] = {
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0,
        0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E),
};

/* --- Send helper (chunked notification) --- */
static int ble_send(const uint8_t *data, uint16_t len)
{
    if (!s_ble.conn || !s_ble.tx_notify_enabled)
        return -1;

    uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
    uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
    uint16_t offset = 0;

    while (offset < len) {
        uint16_t chunk = len - offset;
        if (chunk > max_payload)
            chunk = max_payload;

        int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1], data + offset, chunk);
        if (ret)
            return ret;
        offset += chunk;
    }
    return 0;
}

/* --- Send typed response --- */
static void send_status_response(void)
{
    uint8_t buf[6];
    buf[0] = BLE_RSP_STATUS;

    if (s_ble.recorder) {
        if (s_ble.recorder->is_streaming && s_ble.recorder->is_streaming())
            buf[1] = BLE_STATUS_STREAMING;
        else if (s_ble.recorder->is_recording())
            buf[1] = BLE_STATUS_RECORDING;
        else
            buf[1] = BLE_STATUS_IDLE;
    } else {
        buf[1] = BLE_STATUS_IDLE;
    }

    /* Recorded bytes (4 bytes, little-endian) */
    uint32_t bytes = s_ble.recorder ? s_ble.recorder->get_bytes() : 0;
    memcpy(&buf[2], &bytes, 4);

    ble_send(buf, sizeof(buf));
}

static void send_error(uint8_t code)
{
    uint8_t buf[2] = { BLE_RSP_ERROR, code };
    ble_send(buf, sizeof(buf));
}

/* The old blestack exported bt_le_read_rssi(); the Zephyr 4.4 host does not,
 * so read it over HCI directly. Synchronous, and safe from the BT RX context
 * for the same reason the DLE request in connected_cb() is: the command
 * complete comes back through the controller task, not this thread. */
static int bt_le_read_rssi(uint16_t handle, int8_t *rssi)
{
    struct net_buf *buf, *rsp = NULL;

    buf = bt_hci_cmd_alloc(K_FOREVER);
    if (!buf) {
        return -1;
    }
    struct bt_hci_cp_read_rssi *cp = net_buf_add(buf, sizeof(*cp));
    cp->handle = sys_cpu_to_le16(handle);

    int err = bt_hci_cmd_send_sync(BT_HCI_OP_READ_RSSI, buf, &rsp);
    if (err) {
        return err;
    }
    *rssi = ((struct bt_hci_rp_read_rssi *)rsp->data)->rssi;
    net_buf_unref(rsp);
    return 0;
}

static void send_config_response(void)
{
    uint8_t  buf[6];
    int8_t   gain = 0;
    uint16_t vad  = 0;
    uint8_t  thr  = 0;

    if (s_ble.recorder && s_ble.recorder->cfg_get)
        s_ble.recorder->cfg_get(&gain, &vad, &thr);

    buf[0] = BLE_RSP_CONFIG;
    buf[1] = 1;                       /* ver */
    buf[2] = (uint8_t)gain;
    buf[3] = (uint8_t)(vad & 0xFF);
    buf[4] = (uint8_t)(vad >> 8);
    buf[5] = thr;
    IPRO_LOGI(TAG, "CONFIG rsp: gain=%d dB vad=%u thr=0.%02u",
              (int)gain, (unsigned)vad, (unsigned)thr);
    ble_send(buf, sizeof(buf));
}

static void send_health_response(void)
{
    uint8_t  buf[8];
    int8_t   rssi  = 0x7F;            /* unknown */
    int16_t  temp  = 0;
    uint16_t vbat  = 0;
    uint8_t  ready = 0, count = 0;

    /* Safe from this (BT RX) context: READ_RSSI's CMD_COMPLETE is delivered
     * via bt_recv_prio() in the controller task, not this thread — same
     * reason the sync DLE request in connected_cb() works. */
    if (s_ble.conn && bt_le_read_rssi(s_ble.conn->handle, &rssi) != 0)
        rssi = 0x7F;

    if (s_ble.recorder && s_ble.recorder->health_get)
        s_ble.recorder->health_get(&temp, &vbat, &ready, &count);

    buf[0] = BLE_RSP_HEALTH;
    buf[1] = (uint8_t)rssi;
    buf[2] = (uint8_t)((uint16_t)temp & 0xFF);
    buf[3] = (uint8_t)((uint16_t)temp >> 8);
    buf[4] = (uint8_t)(vbat & 0xFF);
    buf[5] = (uint8_t)(vbat >> 8);
    buf[6] = ready;
    buf[7] = count;
    IPRO_LOGI(TAG, "HEALTH rsp: rssi=%d temp=%d.%u vbat=%umV ready=%u n=%u",
              (int)rssi, temp / 10, (unsigned)((temp < 0 ? -temp : temp) % 10),
              (unsigned)vbat, ready, count);
    ble_send(buf, sizeof(buf));
}

/* --- Audio send task --- */
static void audio_send_task(void *arg)
{
    (void)arg;

    for (;;) {
        /* Wait for GET_AUDIO trigger */
        xSemaphoreTake(s_send_sem, portMAX_DELAY);

        if (!s_ble.conn || !s_ble.tx_notify_enabled || !s_ble.recorder) {
            send_error(0x01); /* not ready */
            continue;
        }

        const int16_t *pcm = s_ble.recorder->get_buf();
        uint32_t total_bytes = s_ble.recorder->get_bytes();

        if (!pcm || total_bytes == 0) {
            send_error(0x02); /* no data */
            continue;
        }

        /* Send audio header: sample rate (16-bit LE) + total bytes (32-bit LE) */
        uint8_t hdr[7];
        hdr[0] = BLE_RSP_AUDIO_HDR;
        uint16_t sr = 16000;
        memcpy(&hdr[1], &sr, 2);
        memcpy(&hdr[3], &total_bytes, 4);
        ble_send(hdr, sizeof(hdr));

        /* Chunk audio data via notifications */
        uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
        uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
        /* Reserve 1 byte for response ID */
        uint16_t data_chunk = max_payload - 1;
        const uint8_t *src = (const uint8_t *)pcm;
        uint32_t sent = 0;

        IPRO_LOGI(TAG, "Sending %lu bytes audio (MTU=%u, chunk=%u)",
                  (unsigned long)total_bytes, mtu, data_chunk);

        while (sent < total_bytes) {
            if (!s_ble.conn || !s_ble.tx_notify_enabled)
                break;

            uint16_t remain = (total_bytes - sent > data_chunk)
                              ? data_chunk : (uint16_t)(total_bytes - sent);

            uint8_t pkt[252]; /* max BLE payload + 1 */
            pkt[0] = BLE_RSP_AUDIO_DATA;
            memcpy(&pkt[1], src + sent, remain);

            int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1], pkt, remain + 1);
            if (ret) {
                /* BLE buffer full — back off and retry */
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }
            sent += remain;

            /* Yield periodically to let BLE stack flush */
            if ((sent % 4096) < data_chunk)
                vTaskDelay(pdMS_TO_TICKS(2));
        }

        /* Send done */
        uint8_t done[5];
        done[0] = BLE_RSP_AUDIO_DONE;
        memcpy(&done[1], &sent, 4);
        ble_send(done, sizeof(done));

        IPRO_LOGI(TAG, "Audio send complete: %lu bytes", (unsigned long)sent);
    }
}

/* --- Stream send task: LC3-encodes and forwards live PDM frames over BLE --- */
static void stream_send_task(void *arg)
{
    (void)arg;
    int16_t *frame;

    /* LC3 packet: 1 byte header + 2 × 40 bytes LC3 = 81 bytes — single BLE notif */
    uint8_t lc3_pkt[1 + LC3_OCTETS * 2];
    lc3_pkt[0] = BLE_RSP_AUDIO_LC3;

    /* Fallback raw PCM packet */
    uint8_t raw_pkt[1 + 243];

    /* Init LC3 encoder (once, static memory) */
    if (!s_encoder_init) {
        s_encoder = lc3_setup_encoder(LC3_FRAME_US, LC3_SAMPLE_RATE,
                                      0, &s_enc_mem);
        if (s_encoder) {
            s_encoder_init = true;
            IPRO_LOGI(TAG, "LC3 encoder ready: %dHz, %dus, %d bytes/frame",
                      LC3_SAMPLE_RATE, LC3_FRAME_US, LC3_OCTETS);
        } else {
            IPRO_LOGW(TAG, "LC3 encoder setup failed — raw PCM fallback");
        }
    }

    for (;;) {
        QueueHandle_t q = s_ble.recorder ?
            (QueueHandle_t)s_ble.recorder->stream_queue : NULL;
        if (!q) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }

        if (xQueueReceive(q, &frame, portMAX_DELAY) != pdTRUE)
            continue;

        if (!s_ble.conn || !s_ble.tx_notify_enabled)
            continue;

        /* Ring buffer is in PSRAM (cached). ISR wrote via CPU cache,
         * so data is already in cache — do NOT invalidate. */

        if (s_use_lc3 && s_encoder_init && s_encoder) {
            /* LC3 mode: encode 320 samples as 2 × 160-sample (10ms) LC3 frames,
             * pack both into ONE notification to halve the notify rate (50/sec). */
            int ok = 1;
            if (lc3_encode(s_encoder, LC3_PCM_FORMAT_S16,
                           frame, 1, LC3_OCTETS, &lc3_pkt[1]) != 0)
                ok = 0;
            if (lc3_encode(s_encoder, LC3_PCM_FORMAT_S16,
                           frame + LC3_FRAME_SAMPLES, 1,
                           LC3_OCTETS, &lc3_pkt[1 + LC3_OCTETS]) != 0)
                ok = 0;

            if (ok) {
                int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                         lc3_pkt, 1 + LC3_OCTETS * 2);
                if (ret) {
                    vTaskDelay(pdMS_TO_TICKS(5));
                    bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                   lc3_pkt, 1 + LC3_OCTETS * 2);
                }
            }
        } else {
            /* Fallback: send raw PCM (chunked per notification) */
            const uint8_t *src = (const uint8_t *)frame;
            uint16_t mtu = bt_gatt_get_mtu(s_ble.conn);
            uint16_t max_payload = (mtu > 3) ? (mtu - 3) : 20;
            uint16_t data_per_notif = (max_payload - 1) & ~1;
            uint16_t offset = 0;

            while (offset < FRAME_BYTES) {
                if (!s_ble.conn || !s_ble.tx_notify_enabled)
                    break;
                uint16_t remain = FRAME_BYTES - offset;
                uint16_t chunk = (remain < data_per_notif) ? remain : data_per_notif;

                raw_pkt[0] = BLE_RSP_AUDIO_DATA;
                memcpy(&raw_pkt[1], src + offset, chunk);

                int ret = bt_gatt_notify(s_ble.conn, &attr_nus_svc[1],
                                         raw_pkt, chunk + 1);
                if (ret) {
                    vTaskDelay(pdMS_TO_TICKS(2));
                    continue;
                }
                offset += chunk;
            }
        }
    }
}

/* --- Command handler --- */
static void handle_command(const uint8_t *data, uint16_t len)
{
    if (len == 0)
        return;

    uint8_t cmd = data[0];

    switch (cmd) {
    case BLE_CMD_START_REC: {
        int seconds = 5;
        if (len >= 2)
            seconds = data[1];
        if (seconds < 1) seconds = 1;
        if (seconds > 30) seconds = 30;

        IPRO_LOGI(TAG, "CMD: START_REC %d sec", seconds);
        if (s_ble.recorder && s_ble.recorder->start_rec)
            s_ble.recorder->start_rec(seconds);

        send_status_response();
        break;
    }
    case BLE_CMD_STOP_REC:
        IPRO_LOGI(TAG, "CMD: STOP_REC");
        if (s_ble.recorder && s_ble.recorder->stop_rec)
            s_ble.recorder->stop_rec();
        send_status_response();
        break;

    case BLE_CMD_GET_AUDIO:
        IPRO_LOGI(TAG, "CMD: GET_AUDIO");
        xSemaphoreGive(s_send_sem);
        break;

    case BLE_CMD_GET_STATUS:
        send_status_response();
        break;

    case BLE_CMD_SET_GAIN:
        if (len >= 2) {
            int8_t gain = (int8_t)data[1];
            IPRO_LOGI(TAG, "CMD: SET_GAIN %d dB", gain);
            if (s_ble.recorder && s_ble.recorder->set_gain)
                s_ble.recorder->set_gain(gain);
        }
        send_status_response();
        break;

    case BLE_CMD_START_STREAM:
        IPRO_LOGI(TAG, "CMD: START_STREAM");
        if (s_ble.recorder && s_ble.recorder->start_stream)
            s_ble.recorder->start_stream();
        send_status_response();
        break;

    case BLE_CMD_STOP_STREAM:
        IPRO_LOGI(TAG, "CMD: STOP_STREAM");
        if (s_ble.recorder && s_ble.recorder->stop_stream)
            s_ble.recorder->stop_stream();
        send_status_response();
        break;

    case BLE_CMD_START_VOICE_ID:
        IPRO_LOGI(TAG, "CMD: START_VOICE_ID");
        if (s_ble.recorder && s_ble.recorder->vi_start)
            s_ble.recorder->vi_start();
        break;

    case BLE_CMD_STOP_VOICE_ID:
        IPRO_LOGI(TAG, "CMD: STOP_VOICE_ID");
        if (s_ble.recorder && s_ble.recorder->vi_stop)
            s_ble.recorder->vi_stop();
        break;

    case BLE_CMD_REG_SPEAKER: {
        /* data[1..] = speaker name (ASCII) */
        if (len >= 2 && s_ble.recorder && s_ble.recorder->vi_reg) {
            char name[32] = {0};
            int name_len = len - 1;
            if (name_len > 31) name_len = 31;
            memcpy(name, &data[1], name_len);
            IPRO_LOGI(TAG, "CMD: REG_SPEAKER '%s'", name);
            s_ble.recorder->vi_reg(name);
        }
        break;
    }

    case BLE_CMD_LIST_SPEAKERS:
        IPRO_LOGI(TAG, "CMD: LIST_SPEAKERS");
        if (s_ble.recorder && s_ble.recorder->vi_list)
            s_ble.recorder->vi_list();
        break;

    case BLE_CMD_CLEAR_SPEAKERS:
        IPRO_LOGI(TAG, "CMD: CLEAR_SPEAKERS");
        if (s_ble.recorder && s_ble.recorder->vi_clear)
            s_ble.recorder->vi_clear();
        break;

    case BLE_CMD_START_MEETING: {
        /* Optional payload byte requests the audio stream alongside voice ID.
         * Absent (older app builds) = 0 = previous behaviour, so this stays
         * backwards compatible. */
        bool with_stream = (len > 1) && (data[1] != 0);
        IPRO_LOGI(TAG, "CMD: START_MEETING (capture + VI%s)",
                  with_stream ? " + stream" : ", no stream");
        if (s_ble.recorder && s_ble.recorder->meeting_start)
            s_ble.recorder->meeting_start(with_stream);
        break;
    }

    case BLE_CMD_STOP_MEETING:
        IPRO_LOGI(TAG, "CMD: STOP_MEETING");
        if (s_ble.recorder && s_ble.recorder->meeting_stop)
            s_ble.recorder->meeting_stop();
        break;

    case BLE_CMD_REG_PCM_START: {
        /* Payload: [name_len:8][name][total_bytes:32LE] */
        if (len < 6) break;
        uint8_t name_len = data[1];
        if (name_len == 0 || name_len >= 32 || len < (uint16_t)(2 + name_len + 4))
            break;
        char name[32] = {0};
        memcpy(name, &data[2], name_len);
        uint32_t total_bytes = (uint32_t)data[2 + name_len] |
                               ((uint32_t)data[3 + name_len] << 8) |
                               ((uint32_t)data[4 + name_len] << 16) |
                               ((uint32_t)data[5 + name_len] << 24);
        IPRO_LOGI(TAG, "CMD: REG_PCM_START '%s' %lu bytes",
                  name, (unsigned long)total_bytes);
        if (s_ble.recorder && s_ble.recorder->reg_pcm_start)
            s_ble.recorder->reg_pcm_start(name, total_bytes);
        break;
    }

    case BLE_CMD_REG_PCM_DATA: {
        /* Payload: [pcm chunk] */
        if (len < 2) break;
        if (s_ble.recorder && s_ble.recorder->reg_pcm_data)
            s_ble.recorder->reg_pcm_data(&data[1], len - 1);
        break;
    }

    case BLE_CMD_DELETE_SPEAKER: {
        /* Payload: [slot:u8] */
        if (len < 2) { send_error(0x03); break; }
        uint8_t slot = data[1];
        IPRO_LOGI(TAG, "CMD: DELETE_SPEAKER slot %u", slot);
        int rc = -1;
        if (s_ble.recorder && s_ble.recorder->vi_delete)
            rc = s_ble.recorder->vi_delete(slot);
        if (rc != 0)
            send_error(0x03);             /* bad slot / AI not ready */
        if (s_ble.recorder && s_ble.recorder->vi_list)
            s_ble.recorder->vi_list();    /* always emit current 0x88 — app resyncs */
        break;
    }

    case BLE_CMD_RENAME_SPEAKER: {
        /* Payload: [slot:u8][new name ASCII, rest of packet] */
        if (len < 3) { send_error(0x03); break; }
        uint8_t slot = data[1];
        char name[32] = {0};
        int name_len = len - 2;
        if (name_len > 31) name_len = 31;
        memcpy(name, &data[2], name_len);
        IPRO_LOGI(TAG, "CMD: RENAME_SPEAKER %u -> '%s'", slot, name);
        int rc = -1;
        if (s_ble.recorder && s_ble.recorder->vi_rename)
            rc = s_ble.recorder->vi_rename(slot, name);
        if (rc != 0)
            send_error(0x03);             /* bad slot / duplicate name */
        if (s_ble.recorder && s_ble.recorder->vi_list)
            s_ble.recorder->vi_list();
        break;
    }

    case BLE_CMD_GET_CONFIG:
        IPRO_LOGI(TAG, "CMD: GET_CONFIG");
        send_config_response();
        break;

    case BLE_CMD_SET_CONFIG: {
        /* Payload: [mask:u8][gain:i8][vad:u16LE][thr_x100:u8] */
        if (len < 6) { send_error(0x03); break; }
        uint8_t  mask = data[1];
        int8_t   gain = (int8_t)data[2];
        uint16_t vad  = (uint16_t)data[3] | ((uint16_t)data[4] << 8);
        uint8_t  thr  = data[5];
        IPRO_LOGI(TAG, "CMD: SET_CONFIG mask=0x%02x", mask);
        if (s_ble.recorder && s_ble.recorder->cfg_set)
            s_ble.recorder->cfg_set(mask, gain, vad, thr);
        send_config_response();           /* echo CURRENT values */
        break;
    }

    case BLE_CMD_GET_HEALTH:
        IPRO_LOGI(TAG, "CMD: GET_HEALTH");
        send_health_response();
        break;

    default:
        IPRO_LOGW(TAG, "Unknown cmd 0x%02x", cmd);
        send_error(0xFF);
        break;
    }
}

/* --- Connection callbacks --- */
static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(TAG, "Connection failed (err 0x%02x)", err);
        return;
    }

    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr_str, sizeof(addr_str));
    IPRO_LOGI(TAG, "Connected: %s", addr_str);

    s_ble.conn = bt_conn_ref(conn);

    /* Request DLE for better throughput (251 octets, 2120us) */
    struct net_buf *buf = bt_hci_cmd_alloc(K_FOREVER);
    if (buf) {
        struct bt_hci_cp_le_set_data_len *cp = net_buf_add(buf, sizeof(*cp));
        cp->handle = sys_cpu_to_le16(conn->handle);
        cp->tx_octets = sys_cpu_to_le16(251);
        cp->tx_time = sys_cpu_to_le16(2120);
        bt_hci_cmd_send_sync(BT_HCI_OP_LE_SET_DATA_LEN, buf, NULL);
    }

    /* Request fast connection interval (7.5ms – 15ms) */
    struct bt_le_conn_param param = {
        .interval_min = 6,      /* 7.5ms */
        .interval_max = 12,     /* 15ms */
        .latency = 0,
        .timeout = 400,         /* 4s */
    };
    bt_conn_le_param_update(conn, &param);
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(TAG, "Disconnected (reason 0x%02x)", reason);

    /* Stop streaming if active */
    if (s_ble.recorder && s_ble.recorder->stop_stream &&
        s_ble.recorder->is_streaming && s_ble.recorder->is_streaming()) {
        s_ble.recorder->stop_stream();
    }

    if (s_ble.conn) {
        bt_conn_unref(s_ble.conn);
        s_ble.conn = NULL;
    }
    s_ble.tx_notify_enabled = false;

    /* Auto-restart advertising */
    vTaskDelay(pdMS_TO_TICKS(200));
    start_advertising();
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/* --- SMP auth (auto-accept) --- */
static void auth_passkey_display(struct bt_conn *conn, unsigned int passkey)
{
    IPRO_LOGI(TAG, "Passkey: %06u", passkey);
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
    IPRO_LOGW(TAG, "Pairing cancelled");
}

static struct bt_conn_auth_cb auth_callbacks = {
    .passkey_display = auth_passkey_display,
    .passkey_confirm = auth_passkey_confirm,
    .pairing_confirm = auth_pairing_confirm,
    .cancel = auth_cancel,
};

/* --- Advertising --- */
static void start_advertising(void)
{
    struct bt_le_adv_param adv_param = {
        .id = BT_ID_DEFAULT,
        .options = BT_LE_ADV_OPT_CONN,
        .interval_min = ADV_INT_MIN,
        .interval_max = ADV_INT_MAX,
    };

    int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        IPRO_LOGE(TAG, "Advertising start failed: %d", err);
        return;
    }
    IPRO_LOGI(TAG, "Advertising as '%s'", DEVICE_NAME);
}

/* --- bt_ready callback --- */
static void bt_ready_cb(int err)
{
    if (err) {
        IPRO_LOGE(TAG, "BLE init failed (err %d)", err);
        return;
    }

    IPRO_LOGI(TAG, "BLE initialized");

    bt_set_name(DEVICE_NAME);
    bt_conn_cb_register(&conn_callbacks);
    bt_conn_auth_cb_register(&auth_callbacks);
    bt_set_bondable(true);
    bt_passkey_set(123456);

    start_advertising();
}

/* --- Public API --- */
int ble_audio_init(const ble_audio_recorder_t *recorder)
{
    s_ble.recorder = recorder;

    /* Create audio send semaphore and task */
    s_send_sem = xSemaphoreCreateBinary();

    xTaskCreate(audio_send_task, "ble_send", SEND_TASK_STACK, NULL,
                SEND_TASK_PRIO, &s_send_task_handle);

    /* Create stream send task (reads from stream_queue) */
    xTaskCreate(stream_send_task, "ble_strm", STREAM_TASK_STACK, NULL,
                STREAM_TASK_PRIO, NULL);

    /* Init BLE controller */
    IPRO_LOGI(TAG, "Initializing BLE controller...");
    ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Init HCI driver */
    IPRO_LOGI(TAG, "Initializing HCI driver...");
    int err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(TAG, "HCI driver init failed: %d", err);
        return -1;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    /* Enable BLE stack */
    IPRO_LOGI(TAG, "Enabling BLE stack...");
    err = bt_enable(bt_ready_cb);
    if (err) {
        IPRO_LOGE(TAG, "bt_enable failed: %d", err);
        return -1;
    }

    return 0;
}

void ble_audio_notify_recording_done(void)
{
    send_status_response();
}

void ble_audio_notify_status(void)
{
    send_status_response();
}

void ble_audio_set_lc3(bool enable)
{
    s_use_lc3 = enable;
    IPRO_LOGI(TAG, "Stream codec: %s", enable ? "LC3" : "raw PCM");
}

bool ble_audio_get_lc3(void)
{
    return s_use_lc3;
}

void ble_audio_notify_speaker_id(int speaker_id, float similarity, const char *name)
{
    if (!s_ble.conn || !s_ble.tx_notify_enabled)
        return;

    /* Packet: [0x87][speaker_id:8][similarity_x100:16LE][name_len:8][name...] */
    uint8_t buf[40];
    buf[0] = BLE_RSP_SPEAKER_ID;
    buf[1] = (uint8_t)(speaker_id & 0xFF);
    uint16_t sim_x100 = (uint16_t)(similarity * 100.0f);
    buf[2] = (uint8_t)(sim_x100 & 0xFF);
    buf[3] = (uint8_t)((sim_x100 >> 8) & 0xFF);

    uint8_t name_len = 0;
    if (name) {
        name_len = (uint8_t)strlen(name);
        if (name_len > sizeof(buf) - 5)
            name_len = sizeof(buf) - 5;
    }
    buf[4] = name_len;
    if (name_len > 0)
        memcpy(&buf[5], name, name_len);

    bt_gatt_notify(s_ble.conn, &attr_nus_svc[1], buf, 5 + name_len);
}

void ble_audio_notify_speaker_list(int count, const char **names)
{
    if (!s_ble.conn || !s_ble.tx_notify_enabled)
        return;

    /* Packet: [0x88][count:8]{[id:8][name_len:8][name...]}... */
    uint8_t buf[200];
    uint16_t pos = 0;
    buf[pos++] = BLE_RSP_SPEAKER_LIST;
    buf[pos++] = (uint8_t)count;

    for (int i = 0; i < count && pos < sizeof(buf) - 2; i++) {
        buf[pos++] = (uint8_t)i;
        uint8_t nlen = names[i] ? (uint8_t)strlen(names[i]) : 0;
        if (nlen > sizeof(buf) - pos - 1)
            nlen = sizeof(buf) - pos - 1;
        buf[pos++] = nlen;
        if (nlen > 0) {
            memcpy(&buf[pos], names[i], nlen);
            pos += nlen;
        }
    }

    bt_gatt_notify(s_ble.conn, &attr_nus_svc[1], buf, pos);
}

/* Required by BLE controller */
void app_init(void)
{
    IPRO_LOGI(TAG, "BLE stack callback: app_init");
}
