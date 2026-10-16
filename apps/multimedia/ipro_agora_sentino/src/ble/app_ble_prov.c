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
 * @file app_ble_prov.c
 * @brief BLE provisioning service for Sentino device binding
 *
 * GATT Service 0x1910:
 *   - Notify Char 0x2B10: device -> phone
 *   - Write  Char 0x2B11: phone -> device
 *
 * ADV: Flags + UUID 0xA101 + pid
 * Scan Resp: Name "RY" + MfgSpec + bind_status + uuid
 */

#include "app_ble_prov.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/uuid.h>

#include "hci_driver_ipro_ble_ctlr.h"
#if defined(CONFIG_BT_CTLR_IPRO)
#include "ipro_ble_controller.h"
#endif

#include <errno.h>

#include "FreeRTOS.h"
#include "task.h"

#include "app_log.h"
#include "app_json.h"

#ifdef CONFIG_EASYFLASH4
#include "easyflash.h"
#endif

#define LOG_TAG "BLE_PROV"

/* ========== UUIDs ========== */
#define PROV_SVC_UUID16         0x1910
#define PROV_NOTIFY_UUID16      0x2B10
#define PROV_WRITE_UUID16       0x2B11

#define BT_UUID_PROV_SVC        BT_UUID_DECLARE_16(PROV_SVC_UUID16)
#define BT_UUID_PROV_NOTIFY     BT_UUID_DECLARE_16(PROV_NOTIFY_UUID16)
#define BT_UUID_PROV_WRITE      BT_UUID_DECLARE_16(PROV_WRITE_UUID16)

/* ADV service UUID for scan filter */
#define ADV_SVC_UUID16          0xA101

/* ========== EasyFlash Keys ========== */
#define KV_BIND_STATUS  "sn_bind"
#define KV_USER_ID      "sn_uid"
#define KV_ASSET_ID     "sn_aid"
#define KV_MQTT_HOST    "sn_mq_host"
#define KV_MQTT_PORT    "sn_mq_port"

/* ========== State ========== */
static struct {
    ble_prov_done_cb_t done_cb;
    struct bt_conn *conn;
    char pid[20];
    char uuid[20];
    char user_id[64];
    char asset_id[64];
    char mqtt_host[64];
    uint16_t mqtt_port;
    bool bound;
    bool notify_enabled;
    bool initialized;
} g_prov = {0};

/* ========== Persistence ========== */

static void prov_load_bind_state(void)
{
#ifdef CONFIG_EASYFLASH4
    char val[8] = {0};
    size_t len = sizeof(val);
    if (ef_get_env_blob(KV_BIND_STATUS, val, sizeof(val) - 1, &len) > 0) {
        g_prov.bound = (val[0] == '1');
    }
    if (g_prov.bound) {
        len = sizeof(g_prov.user_id) - 1;
        ef_get_env_blob(KV_USER_ID, g_prov.user_id, sizeof(g_prov.user_id) - 1, &len);
        len = sizeof(g_prov.asset_id) - 1;
        ef_get_env_blob(KV_ASSET_ID, g_prov.asset_id, sizeof(g_prov.asset_id) - 1, &len);
        len = sizeof(g_prov.mqtt_host) - 1;
        ef_get_env_blob(KV_MQTT_HOST, g_prov.mqtt_host, sizeof(g_prov.mqtt_host) - 1, &len);
        char port_str[8] = {0};
        len = sizeof(port_str) - 1;
        if (ef_get_env_blob(KV_MQTT_PORT, port_str, sizeof(port_str) - 1, &len) > 0) {
            g_prov.mqtt_port = (uint16_t)atoi(port_str);
        }
    }
    IPRO_LOGI(LOG_TAG, "Bind state loaded: bound=%d uid='%s' aid='%s' mq='%s' port=%u",
              g_prov.bound, g_prov.user_id, g_prov.asset_id,
              g_prov.mqtt_host, g_prov.mqtt_port);
#endif
}

static void prov_save_bind_state(const ble_prov_data_t *data)
{
#ifdef CONFIG_EASYFLASH4
    ef_set_env_blob(KV_BIND_STATUS, "1", 1);
    ef_set_env_blob(KV_USER_ID, data->user_id, strlen(data->user_id));
    ef_set_env_blob(KV_ASSET_ID, data->asset_id, strlen(data->asset_id));
    strncpy(g_prov.user_id, data->user_id, sizeof(g_prov.user_id) - 1);
    strncpy(g_prov.asset_id, data->asset_id, sizeof(g_prov.asset_id) - 1);
    /* Save MQTT broker info if available */
    if (g_prov.mqtt_host[0]) {
        ef_set_env_blob(KV_MQTT_HOST, g_prov.mqtt_host, strlen(g_prov.mqtt_host));
    }
    if (g_prov.mqtt_port > 0) {
        char port_str[8];
        snprintf(port_str, sizeof(port_str), "%u", g_prov.mqtt_port);
        ef_set_env_blob(KV_MQTT_PORT, port_str, strlen(port_str));
    }
    g_prov.bound = true;
    IPRO_LOGI(LOG_TAG, "Bind state saved (mq='%s' port=%u)",
              g_prov.mqtt_host, g_prov.mqtt_port);
#endif
}

void app_ble_prov_clear_bind(void)
{
#ifdef CONFIG_EASYFLASH4
    ef_set_env_blob(KV_BIND_STATUS, "0", 1);
    ef_del_env(KV_USER_ID);
    ef_del_env(KV_ASSET_ID);
    ef_del_env(KV_MQTT_HOST);
    ef_del_env(KV_MQTT_PORT);
#endif
    g_prov.bound = false;
    g_prov.user_id[0] = '\0';
    g_prov.asset_id[0] = '\0';
    g_prov.mqtt_host[0] = '\0';
    g_prov.mqtt_port = 0;
    IPRO_LOGI(LOG_TAG, "Bind state cleared");
}

/* ========== GATT Callbacks ========== */

static void prov_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    g_prov.notify_enabled = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "Notify %s", g_prov.notify_enabled ? "enabled" : "disabled");
}

/* ========== BLE Framing Protocol (from reference SDK ble_parser_packet) ========== */
/*
 * Frame format per BLE write:
 *   [0]     head       = 0xFF (start marker)
 *   [1]     type       = command type (0x01 for incoming)
 *   [2-3]   index      = part index (big-endian uint16, 0-based)
 *   [4-5]   total_pack = total parts (big-endian uint16)
 *   [6-7]   total_len  = total payload length across ALL parts (big-endian uint16)
 *   [8]     data_len   = this fragment's payload length (uint8)
 *   [9..9+data_len-1]  = payload (JSON fragment)
 *   [9+data_len]       = crc = checksum of bytes[1..frame_len-2]
 *
 * Checksum: get_check_sum(data + 1, len - 2) = sum of bytes[1] through bytes[len-2]
 */

#define PROV_FRAME_HEADER_SIZE 9
#define PROV_FRAME_CHECKSUM_SIZE 1
#define PROV_FRAME_START_MARKER 0xFF
#define PROV_RX_BUF_SIZE 1024

/* Reassembly state */
static uint8_t g_rx_buf[PROV_RX_BUF_SIZE];
static uint16_t g_rx_len = 0;
static uint16_t g_rx_total_parts = 0;
static uint16_t g_rx_parts_received = 0;
static uint16_t g_rx_total_payload_len = 0;
static uint8_t g_rx_frame_type = 0;

static void prov_hex_dump(const uint8_t *data, uint16_t len)
{
    uint16_t dump_len = (len > 64) ? 64 : len;
    for (uint16_t i = 0; i < dump_len; i++) {
        printf("%02x ", data[i]);
        if ((i & 0xF) == 0xF) printf("\n");
    }
    if (dump_len & 0xF) printf("\n");
    if (len > 64) printf("  ... (%u more bytes)\n", len - 64);
}

static uint8_t prov_checksum(const uint8_t *data, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++) sum += data[i];
    return sum;
}

static void prov_rx_reset(void)
{
    g_rx_len = 0;
    g_rx_total_parts = 0;
    g_rx_parts_received = 0;
    g_rx_total_payload_len = 0;
    g_rx_frame_type = 0;
}

/* Build and send a framed message via BLE notify, split like the reference
 * SDK's ble_make_send_frame(): parts of up to PROV_PART_DATA_MAX payload
 * bytes, each [0xFF, type, index(2), total_parts(2), total_len(2), data_len,
 * payload..., crc], and never more than the connection's ATT MTU allows.
 * may_block: retry while the host is out of TX buffers (task context only;
 * the BT RX thread must not wait for buffers it is the one to free). */
#define PROV_PART_DATA_MAX 118
#define PROV_ATT_NOTIFY_HDR 3

static int prov_send_framed(const char *json, bool may_block)
{
    size_t total = strlen(json);
    uint8_t frame[PROV_FRAME_HEADER_SIZE + PROV_PART_DATA_MAX + PROV_FRAME_CHECKSUM_SIZE];

    if (g_prov.conn == NULL || total > 0xFFFF) {
        return -ENOTCONN;
    }
    uint16_t mtu = bt_gatt_get_mtu(g_prov.conn);
    int room = (int)mtu - PROV_ATT_NOTIFY_HDR - PROV_FRAME_HEADER_SIZE - PROV_FRAME_CHECKSUM_SIZE;
    if (room <= 0) {
        IPRO_LOGW(LOG_TAG, "ATT MTU %u too small for a frame", mtu);
        return -EMSGSIZE;
    }
    uint16_t chunk = (room < PROV_PART_DATA_MAX) ? (uint16_t)room : PROV_PART_DATA_MAX;
    uint16_t parts = (uint16_t)((total + chunk - 1) / chunk);
    if (parts == 0) {
        parts = 1;
    }

    size_t off = 0;
    for (uint16_t i = 0; i < parts; i++) {
        uint16_t n = (uint16_t)((total - off < chunk) ? (total - off) : chunk);
        uint16_t frame_len = PROV_FRAME_HEADER_SIZE + n + PROV_FRAME_CHECKSUM_SIZE;

        frame[0] = PROV_FRAME_START_MARKER;
        frame[1] = 0x01;                          /* type */
        frame[2] = (uint8_t)(i >> 8);
        frame[3] = (uint8_t)i;
        frame[4] = (uint8_t)(parts >> 8);
        frame[5] = (uint8_t)parts;
        frame[6] = (uint8_t)(total >> 8);
        frame[7] = (uint8_t)total;
        frame[8] = (uint8_t)n;
        memcpy(&frame[PROV_FRAME_HEADER_SIZE], json + off, n);
        /* crc = sum of bytes[1..frame_len-2] */
        frame[frame_len - 1] = prov_checksum(&frame[1], frame_len - 2);

        int err;
        int tries = may_block ? 100 : 1;
        do {
            err = app_ble_prov_notify(frame, frame_len);
            if (err == -ENOMEM && may_block) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        } while (err == -ENOMEM && --tries > 0);
        if (err) {
            IPRO_LOGW(LOG_TAG, "notify part %u/%u failed: %d", i + 1, parts, err);
            return err;
        }
        off += n;
    }

    IPRO_LOGI(LOG_TAG, "TX response: %u bytes in %u part(s) (mtu %u)",
              (unsigned)total, parts, mtu);
    return 0;
}

static void prov_send_framed_response(const char *json)
{
    (void)prov_send_framed(json, false);
}

/* ========== Wi-Fi list (thing.network.getwifis) ========== */
/*
 * The phone app runs the reference SDK's Wi-Fi flow: it repeats
 * {"type":"thing.network.getwifis","ts":...,"scan":true} until it gets
 *   {"type":"thing.network.getwifis.response","msgId":..,"code":0,"ts":..,
 *    "data":{"wifis":[{"ssid":"..","rssi":"-48","security":true},...]}}
 * (strongest first, rssi as a string - rino_ble_gatts_wifi_list_handle()),
 * lets the user pick one and sends thing.network.set with sid / pw next to
 * the bind fields. A scan takes seconds, so the request is handed from the BT
 * RX thread to this worker; requests that arrive meanwhile coalesce.
 */
#define PROV_WIFI_SCAN_MAX   45     /* what the scanner may return */
#define PROV_WIFI_LIST_MAX   20     /* what goes back to the phone */
#define PROV_WIFI_JSON_SIZE  2048
#define PROV_WORKER_STACK    6144   /* bytes; the scan may run ipronet_start() */

static ble_prov_wifi_scan_fn  s_wifi_scan;
static ble_prov_wifi_creds_fn s_wifi_creds;
static TaskHandle_t s_worker;
static volatile uint32_t s_req_ts;   /* "ts" of the last getwifis, echoed back */

void app_ble_prov_set_wifi_hooks(ble_prov_wifi_scan_fn scan, ble_prov_wifi_creds_fn creds)
{
    s_wifi_scan = scan;
    s_wifi_creds = creds;
}

static uint32_t prov_json_u32(const char *json, const char *quoted_key)
{
    const char *p = strstr(json, quoted_key);
    if (p == NULL) {
        return 0;
    }
    p += strlen(quoted_key);
    while (*p == ' ' || *p == ':' || *p == '\t' || *p == '"') {
        p++;
    }
    return (uint32_t)strtoul(p, NULL, 10);
}

/* Append s as a JSON string body (no quotes); false when out of room. */
static bool prov_json_put_escaped(char *buf, size_t size, size_t *pos, const char *s)
{
    for (; *s != '\0'; s++) {
        unsigned char c = (unsigned char)*s;
        if (c < 0x20) {
            continue;                       /* no control characters in an SSID list */
        }
        if (*pos + 2 >= size) {
            return false;
        }
        if (c == '"' || c == '\\') {
            buf[(*pos)++] = '\\';
        }
        buf[(*pos)++] = (char)c;
    }
    buf[*pos] = '\0';
    return true;
}

static int prov_ap_by_rssi(const void *a, const void *b)
{
    return ((const ble_prov_ap_t *)b)->rssi - ((const ble_prov_ap_t *)a)->rssi;
}

/* Sort strongest first, drop hidden networks and repeated SSIDs (the
 * strongest BSS of each name survives). Returns the new count. */
static int prov_ap_list_tidy(ble_prov_ap_t *aps, int n)
{
    qsort(aps, (size_t)n, sizeof(*aps), prov_ap_by_rssi);
    int out = 0;
    for (int i = 0; i < n; i++) {
        bool dup = (aps[i].ssid[0] == '\0');
        for (int j = 0; j < out && !dup; j++) {
            dup = (strcmp(aps[j].ssid, aps[i].ssid) == 0);
        }
        if (!dup) {
            aps[out++] = aps[i];
        }
    }
    return out;
}

static void prov_send_wifi_list(void)
{
    static ble_prov_ap_t aps[PROV_WIFI_SCAN_MAX];
    static char json[PROV_WIFI_JSON_SIZE];
    int n = 0;
    int code = 0;

    if (s_wifi_scan != NULL) {
        n = s_wifi_scan(aps, PROV_WIFI_SCAN_MAX);
        if (n < 0) {
            IPRO_LOGW(LOG_TAG, "Wi-Fi scan failed (%d), sending an empty list", n);
            n = 0;
            code = 1;
        }
        n = prov_ap_list_tidy(aps, n);
    }

    size_t pos = (size_t)snprintf(json, sizeof(json),
        "{\"type\":\"thing.network.getwifis.response\",\"msgId\":\"%08lx\","
        "\"code\":%d,\"ts\":%lu,\"data\":{\"wifis\":[",
        (unsigned long)xTaskGetTickCount(), code, (unsigned long)s_req_ts);
    int listed = 0;
    for (int i = 0; i < n && listed < PROV_WIFI_LIST_MAX; i++) {
        size_t mark = pos;
        bool ok = pos + 16 < sizeof(json);
        if (ok) {
            pos += (size_t)snprintf(json + pos, sizeof(json) - pos, "%s{\"ssid\":\"",
                                    listed ? "," : "");
            ok = prov_json_put_escaped(json, sizeof(json), &pos, aps[i].ssid);
        }
        if (ok) {
            int w = snprintf(json + pos, sizeof(json) - pos,
                             "\",\"rssi\":\"%d\",\"security\":%s}",
                             (int)aps[i].rssi, aps[i].secure ? "true" : "false");
            ok = (w > 0 && pos + (size_t)w + 4 < sizeof(json));
            if (ok) {
                pos += (size_t)w;
            }
        }
        if (!ok) {
            pos = mark;                     /* drop the entry that did not fit */
            json[pos] = '\0';
            break;
        }
        listed++;
    }
    snprintf(json + pos, sizeof(json) - pos, "]}}");

    IPRO_LOGI(LOG_TAG, "getwifis: %d AP(s) listed, code %d", listed, code);
    (void)prov_send_framed(json, true);
}

static void prov_worker_task(void *arg)
{
    (void)arg;
    for (;;) {
        (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (g_prov.conn != NULL && g_prov.notify_enabled) {
            prov_send_wifi_list();
        }
    }
}

static void prov_request_wifi_list(void)
{
    s_req_ts = prov_json_u32((const char *)g_rx_buf, "\"ts\"");
    if (s_worker == NULL) {
        IPRO_LOGW(LOG_TAG, "getwifis: no worker task");
        return;
    }
    xTaskNotifyGive(s_worker);
}

static void prov_process_complete_message(void)
{
    /* Null-terminate for JSON parsing */
    if (g_rx_len >= PROV_RX_BUF_SIZE) g_rx_len = PROV_RX_BUF_SIZE - 1;
    g_rx_buf[g_rx_len] = '\0';

    IPRO_LOGI(LOG_TAG, "Complete message: type=0x%02x %u bytes", g_rx_frame_type, g_rx_len);
    if (strstr((const char *)g_rx_buf, "\"pw\"") != NULL) {
        IPRO_LOGI(LOG_TAG, "JSON: (carries a Wi-Fi password, not logged)");
    } else {
        IPRO_LOGI(LOG_TAG, "JSON: %.*s", g_rx_len > 512 ? 512 : (int)g_rx_len,
                  (const char *)g_rx_buf);
    }

    if (strstr((const char *)g_rx_buf, "\"thing.network.getwifis\"") != NULL) {
        prov_request_wifi_list();
        prov_rx_reset();
        return;
    }

    /* Parse the provisioning JSON.
     * Phone sends: {"type":"thing.network.set","data":{"userId":"x","bid":"y",...}}
     * We need: data.userId and data.bid (= assetId) */
    ble_prov_data_t pdata = {0};
    /* This runs in the BT RX work queue (GATT write callback), whose stack is
     * static and small: an overflow rewrites neighbouring .bss instead of
     * faulting, and the board hung right here on the first phone write. Only
     * this thread ever parses, so the scratch copy can live outside it. */
    static char work[512];
    uint16_t parse_len = (g_rx_len < sizeof(work) - 1) ? g_rx_len : sizeof(work) - 1;

    /* Extract userId from nested JSON */
    memcpy(work, g_rx_buf, parse_len);
    work[parse_len] = '\0';
    char *user_id = json_extract_string(work, "userId");
    if (user_id) {
        strncpy(pdata.user_id, user_id, sizeof(pdata.user_id) - 1);
    }

    /* Extract bid (assetId) */
    memcpy(work, g_rx_buf, parse_len);
    work[parse_len] = '\0';
    char *bid = json_extract_string(work, "bid");
    if (bid) {
        strncpy(pdata.asset_id, bid, sizeof(pdata.asset_id) - 1);
    }

    /* Also try "assetId" key as fallback */
    if (!pdata.asset_id[0]) {
        memcpy(work, g_rx_buf, parse_len);
        work[parse_len] = '\0';
        char *asset_id = json_extract_string(work, "assetId");
        if (asset_id) {
            strncpy(pdata.asset_id, asset_id, sizeof(pdata.asset_id) - 1);
        }
    }

    /* Extract MQTT broker info: mq (host) and port */
    memcpy(work, g_rx_buf, parse_len);
    work[parse_len] = '\0';
    char *mq = json_extract_string(work, "mq");
    if (mq) {
        strncpy(g_prov.mqtt_host, mq, sizeof(g_prov.mqtt_host) - 1);
    }

    memcpy(work, g_rx_buf, parse_len);
    work[parse_len] = '\0';
    const char *port_key = strstr(work, "\"port\"");
    if (port_key) {
        port_key += 6;
        while (*port_key == ' ' || *port_key == ':' || *port_key == '\t') port_key++;
        g_prov.mqtt_port = (uint16_t)strtoul(port_key, NULL, 10);
    }

    IPRO_LOGI(LOG_TAG, "Parsed: userId='%s' bid='%s' mq='%s' port=%u",
              pdata.user_id, pdata.asset_id, g_prov.mqtt_host, g_prov.mqtt_port);

    /* Wi-Fi credentials (sid / pw, reference para_connect_wifi()). Stored
     * before the bind state: saving the bind is what releases the gateway
     * into its network phase, which reads them. */
    int code = 0;
    memcpy(work, g_rx_buf, parse_len);
    work[parse_len] = '\0';
    char *sid = json_extract_string(work, "sid");
    if (sid != NULL && sid[0] != '\0') {
        char ssid[33];
        bool ssid_ok = strlen(sid) < sizeof(ssid);
        strncpy(ssid, sid, sizeof(ssid) - 1);
        ssid[sizeof(ssid) - 1] = '\0';

        memcpy(work, g_rx_buf, parse_len);
        work[parse_len] = '\0';
        const char *pw = json_extract_string(work, "pw");
        if (pw == NULL) {
            pw = "";
        }
        IPRO_LOGI(LOG_TAG, "Wi-Fi: sid='%s' pw=%u chars", ssid, (unsigned)strlen(pw));
        if (!ssid_ok || (s_wifi_creds != NULL && s_wifi_creds(ssid, pw) != 0)) {
            IPRO_LOGW(LOG_TAG, "Wi-Fi credentials rejected");
            code = 1;
        }
    }

    if (code == 0 && (pdata.user_id[0] || pdata.asset_id[0]) && !g_prov.bound) {
        /* Save only once — ignore phone retries */
        prov_save_bind_state(&pdata);
        if (g_prov.done_cb) {
            g_prov.done_cb(&pdata);
        }
        IPRO_LOGI(LOG_TAG, "Bind state saved (first successful parse)");
    } else if (g_prov.bound) {
        IPRO_LOGI(LOG_TAG, "Already bound, skipping duplicate save");
    }

    /* Always send framed ACK response (phone expects it even on retries) */
    char resp[128];
    snprintf(resp, sizeof(resp),
             "{\"ts\":0,\"code\":%d,\"msg\":\"%s\","
             "\"type\":\"thing.network.set.response\"}",
             code, code == 0 ? "success" : "wifi info error");
    prov_send_framed_response(resp);

    prov_rx_reset();
}

static ssize_t prov_write_cb(struct bt_conn *conn,
                             const struct bt_gatt_attr *attr,
                             const void *buf, uint16_t len,
                             uint16_t offset, uint8_t flags)
{
    const uint8_t *raw = (const uint8_t *)buf;

    IPRO_LOGI(LOG_TAG, "RX write: len=%u offset=%u flags=0x%02x", len, offset, flags);
    /* Frame header plus a few payload bytes: the payload can carry the
     * Wi-Fi password. */
    prov_hex_dump(raw, len < 16 ? len : 16);

    /* Check for framed protocol: starts with 0xFF and has at least header + 1 byte */
    if (len >= PROV_FRAME_HEADER_SIZE + PROV_FRAME_CHECKSUM_SIZE &&
        raw[0] == PROV_FRAME_START_MARKER) {

        uint8_t  frame_type = raw[1];
        uint16_t part_index = ((uint16_t)raw[2] << 8) | raw[3];
        uint16_t total_parts = ((uint16_t)raw[4] << 8) | raw[5];
        uint16_t total_payload_len = ((uint16_t)raw[6] << 8) | raw[7];
        uint8_t  data_len  = raw[8];

        IPRO_LOGI(LOG_TAG, "Frame: type=0x%02x part=%u/%u total_len=%u data_len=%u",
                  frame_type, part_index, total_parts, total_payload_len, data_len);

        /* Validate data_len fits in the BLE write */
        if (PROV_FRAME_HEADER_SIZE + data_len + PROV_FRAME_CHECKSUM_SIZE > len) {
            IPRO_LOGW(LOG_TAG, "Frame data_len %u exceeds write len %u", data_len, len);
            data_len = len - PROV_FRAME_HEADER_SIZE - PROV_FRAME_CHECKSUM_SIZE;
        }

        /* Verify checksum: sum of bytes[1..frame_len-2] */
        uint16_t frame_len = PROV_FRAME_HEADER_SIZE + data_len + PROV_FRAME_CHECKSUM_SIZE;
        uint8_t expected_crc = prov_checksum(&raw[1], frame_len - 2);
        uint8_t actual_crc = raw[frame_len - 1];
        if (expected_crc != actual_crc) {
            IPRO_LOGW(LOG_TAG, "CRC mismatch: expected=0x%02x actual=0x%02x",
                      expected_crc, actual_crc);
        }

        /* First part: reset reassembly state */
        if (part_index == 0) {
            prov_rx_reset();
            g_rx_frame_type = frame_type;
            g_rx_total_parts = total_parts;
            g_rx_total_payload_len = total_payload_len;
        }

        /* Append payload (skip 9-byte header, skip 1-byte crc) */
        const uint8_t *payload = &raw[PROV_FRAME_HEADER_SIZE];
        uint16_t space = PROV_RX_BUF_SIZE - 1 - g_rx_len;
        uint16_t copy_len = (data_len < space) ? data_len : space;
        if (copy_len > 0) {
            memcpy(&g_rx_buf[g_rx_len], payload, copy_len);
            g_rx_len += copy_len;
        }
        g_rx_parts_received++;

        IPRO_LOGI(LOG_TAG, "Reassembly: %u/%u parts, %u/%u bytes",
                  g_rx_parts_received, g_rx_total_parts,
                  g_rx_len, g_rx_total_payload_len);

        /* All parts received? */
        if (g_rx_parts_received >= g_rx_total_parts) {
            prov_process_complete_message();
        }
    } else {
        /* Fallback: raw data (no framing) — accumulate and check for JSON end */
        IPRO_LOGW(LOG_TAG, "Non-framed data (no 0xFF marker), raw accumulate");
        uint16_t space = PROV_RX_BUF_SIZE - 1 - g_rx_len;
        uint16_t copy_len = (len < space) ? len : space;
        if (copy_len > 0) {
            memcpy(&g_rx_buf[g_rx_len], raw, copy_len);
            g_rx_len += copy_len;
        }
        if (g_rx_len > 0 && g_rx_buf[g_rx_len - 1] == '}') {
            prov_process_complete_message();
        }
    }

    return len;
}

/* ========== GATT Service Definition ========== */

BT_GATT_SERVICE_DEFINE(prov_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_PROV_SVC),

    /* Notify characteristic (device -> phone) */
    BT_GATT_CHARACTERISTIC(BT_UUID_PROV_NOTIFY,
                           BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_NONE,
                           NULL, NULL, NULL),
    BT_GATT_CCC(prov_ccc_changed,
                 BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    /* Write characteristic (phone -> device) */
    BT_GATT_CHARACTERISTIC(BT_UUID_PROV_WRITE,
                           BT_GATT_CHRC_WRITE | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
                           BT_GATT_PERM_WRITE,
                           NULL, prov_write_cb, NULL),
);

/* ========== Connection Callbacks ========== */

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        IPRO_LOGE(LOG_TAG, "Connection failed (err %u)", err);
        return;
    }
    g_prov.conn = bt_conn_ref(conn);
    IPRO_LOGI(LOG_TAG, "Connected");
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
    IPRO_LOGI(LOG_TAG, "Disconnected (reason %u)", reason);
    if (g_prov.conn) {
        bt_conn_unref(g_prov.conn);
        g_prov.conn = NULL;
    }
    g_prov.notify_enabled = false;

    /* Restart advertising after disconnect if not bound */
    if (!g_prov.bound) {
        app_ble_prov_start();
    }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected_cb,
    .disconnected = disconnected_cb,
};

/* ========== Advertising ========== */

/* ADV service data: UUID16(0xA101) + status(1) + pid[16] = 19 bytes */
static uint8_t adv_svc_data[19];

/* Scan response manufacturer data:
 * company(2) + status(1) + fixed_pad(4) + uuid[16] = 23 bytes
 * Layout: [0x00, 0x00, status, 0x10, 0x00, 0x00, 0x07, uuid[16]] */
static uint8_t scan_mfg_data[23];

/* ========== Public API ========== */

int app_ble_prov_init(const char *pid, const char *uuid,
                      ble_prov_done_cb_t done_cb)
{
    if (g_prov.initialized) return 0;

    strncpy(g_prov.pid, pid ? pid : "", sizeof(g_prov.pid) - 1);
    strncpy(g_prov.uuid, uuid ? uuid : "", sizeof(g_prov.uuid) - 1);
    g_prov.done_cb = done_cb;

    /* Load bind state from flash */
    prov_load_bind_state();

    /* Build ADV service data: UUID16(0xA101) + status + pid[16] */
    memset(adv_svc_data, 0, sizeof(adv_svc_data));
    adv_svc_data[0] = (ADV_SVC_UUID16 & 0xFF);       /* 0x01 */
    adv_svc_data[1] = (ADV_SVC_UUID16 >> 8);          /* 0xA1 */
    adv_svc_data[2] = 0x00;                           /* status: unbound */
    size_t pid_len = strlen(g_prov.pid);
    if (pid_len > 16) pid_len = 16;
    memcpy(&adv_svc_data[3], g_prov.pid, pid_len);

    /* Build scan response manufacturer data:
     * company=0x0000, status, fixed pad [0x10,0x00,0x00,0x07], uuid[16] */
    memset(scan_mfg_data, 0, sizeof(scan_mfg_data));
    scan_mfg_data[0] = 0x00;                          /* company ID low */
    scan_mfg_data[1] = 0x00;                          /* company ID high */
    scan_mfg_data[2] = g_prov.bound ? 0x07 : 0x00;   /* bind status */
    scan_mfg_data[3] = 0x10;                          /* fixed pad */
    scan_mfg_data[4] = 0x00;                          /* fixed pad */
    scan_mfg_data[5] = 0x00;                          /* fixed pad */
    scan_mfg_data[6] = 0x07;                          /* fixed pad */
    size_t uuid_len = strlen(g_prov.uuid);
    if (uuid_len > 16) uuid_len = 16;
    memcpy(&scan_mfg_data[7], g_prov.uuid, uuid_len);

    /* Only init BLE HW when not bound (first boot provisioning).
     * When bound, skip BLE entirely to keep PSRAM clean for Agora. */
    /* The IPRO HCI transport has no close(), so bt_disable() in
     * app_ble_prov_deinit() only resets the controller and the stack stays
     * up: a second init (ble_prov_start from the shell) must not start the
     * controller or bind the transport again. */
    static bool s_hw_up;

    if (!g_prov.bound && !s_hw_up) {
        int err;
#if defined(CONFIG_BT_CTLR_IPRO)
        /* On-chip controller (IPRO7AI Reef): it has to be running before the
         * HCI transport binds to it, or HCI_Reset goes unanswered and
         * bt_enable() blocks forever. Same order and settle time as
         * apps/platform/ipro7ai_evk_demo/src/bt.c. */
        err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
        if (err) {
            IPRO_LOGE(LOG_TAG, "BLE controller init failed: %d", err);
            return err;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
#endif
        /* Controller: whichever HCI transport the host was built with. On
         * the IPRO7AI EVK that is the AIC8800 D80-H over the SDIO mailbox
         * (CONFIG_BT_CTLR_AIC8800_SDIO) - the same chip that carries Wi-Fi,
         * so its driver must be probed first: it loads the BT patch tables.
         * hci_driver_ipro_ble_ctlr_init() is the entry point every IPRO HCI
         * transport exports. */
        err = hci_driver_ipro_ble_ctlr_init();
        if (err) {
            IPRO_LOGE(LOG_TAG, "hci_driver init failed: %d", err);
            return err;
        }
#if defined(CONFIG_BT_CTLR_IPRO)
        vTaskDelay(pdMS_TO_TICKS(50));
#endif

        err = bt_enable(NULL);
        IPRO_LOGI(LOG_TAG, "bt_enable returned: %d", err);
        if (err && err != -EALREADY) {
            IPRO_LOGE(LOG_TAG, "bt_enable failed: %d", err);
            return err;
        }

        /* Give BLE host time to fully initialize */
        vTaskDelay(pdMS_TO_TICKS(500));

        bt_conn_cb_register(&conn_callbacks);
        /* getwifis worker. It outlives app_ble_prov_deinit() blocked on its
         * notification; provisioning only runs on an unbound first boot. */
        if (s_worker == NULL &&
            xTaskCreate(prov_worker_task, "ble_prov", PROV_WORKER_STACK / sizeof(StackType_t),
                        NULL, tskIDLE_PRIORITY + 4, &s_worker) != pdPASS) {
            IPRO_LOGE(LOG_TAG, "cannot create the provisioning worker");
            s_worker = NULL;
        }
        s_hw_up = true;
        IPRO_LOGI(LOG_TAG, "BLE HW initialized for provisioning");
    } else if (s_hw_up) {
        IPRO_LOGI(LOG_TAG, "BLE HW already up");
    } else {
        IPRO_LOGI(LOG_TAG, "Bound — BLE HW init skipped");
    }

    g_prov.initialized = true;
    IPRO_LOGI(LOG_TAG, "Initialized (pid=%s uuid=%s bound=%d)",
              g_prov.pid, g_prov.uuid, g_prov.bound);
    return 0;
}

int app_ble_prov_start(void)
{
    if (!g_prov.initialized) return -1;

    size_t pid_len = strlen(g_prov.pid);
    if (pid_len > 16) pid_len = 16;
    size_t uuid_len = strlen(g_prov.uuid);
    if (uuid_len > 16) uuid_len = 16;

    /* Update dynamic status byte before each ADV start */
    adv_svc_data[2] = g_prov.bound ? 0x07 : 0x00;
    scan_mfg_data[2] = g_prov.bound ? 0x07 : 0x00;

    /* UUID 0xA101 as little-endian bytes */
    static const uint8_t adv_uuid[] = {
        (ADV_SVC_UUID16 & 0xFF), (ADV_SVC_UUID16 >> 8)
    };

    /* ADV: Flags + Incomplete UUID16 (0x02) + Service Data (UUID+status+pid) */
    struct bt_data ad_dyn[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
        BT_DATA(BT_DATA_UUID16_SOME, adv_uuid, sizeof(adv_uuid)),
        BT_DATA(BT_DATA_SVC_DATA16, adv_svc_data, 3 + pid_len),
    };

    /* Scan Resp: Complete Name "RY" + MfgSpec (company+status+pad+uuid) */
    struct bt_data sd_dyn[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, "RY", 2),
        BT_DATA(BT_DATA_MANUFACTURER_DATA, scan_mfg_data, 7 + uuid_len),
    };

    IPRO_LOGI(LOG_TAG, "ADV: ad_count=%d sd_count=%d pid_len=%u uuid_len=%u",
              (int)ARRAY_SIZE(ad_dyn), (int)ARRAY_SIZE(sd_dyn),
              (unsigned)pid_len, (unsigned)uuid_len);

    /* Zephyr v4.4 renamed BT_LE_ADV_CONN; FAST_2 (100-150 ms) is the
     * parameter set GAP recommends for advertising that runs longer than
     * the initial 30 s, which is what provisioning does. */
    int err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, ad_dyn, ARRAY_SIZE(ad_dyn),
                               sd_dyn, ARRAY_SIZE(sd_dyn));
    IPRO_LOGI(LOG_TAG, "bt_le_adv_start returned: %d", err);
    if (err) {
        IPRO_LOGE(LOG_TAG, "ADV start failed: %d", err);
        /* Retry once after additional delay */
        vTaskDelay(pdMS_TO_TICKS(500));
        err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2, ad_dyn, ARRAY_SIZE(ad_dyn),
                               sd_dyn, ARRAY_SIZE(sd_dyn));
        IPRO_LOGI(LOG_TAG, "ADV retry returned: %d", err);
        if (err) return err;
    }

    IPRO_LOGI(LOG_TAG, "Advertising started");
    return 0;
}

int app_ble_prov_stop(void)
{
    int err = bt_le_adv_stop();
    if (err) {
        IPRO_LOGW(LOG_TAG, "ADV stop failed: %d", err);
    }
    return err;
}

bool app_ble_prov_is_bound(void)
{
    return g_prov.bound;
}

void app_ble_prov_deinit(void)
{
    if (!g_prov.initialized) return;

    /* Disconnect any active connection */
    if (g_prov.conn) {
        bt_conn_disconnect(g_prov.conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
        bt_conn_unref(g_prov.conn);
        g_prov.conn = NULL;
    }

    /* Disable BLE host + controller (frees threads, queues, heap allocs) */
    int err = bt_disable();
    IPRO_LOGI(LOG_TAG, "bt_disable: %d", err);

    g_prov.initialized = false;
    g_prov.notify_enabled = false;
}

int app_ble_prov_notify(const uint8_t *data, uint16_t len)
{
    if (!g_prov.conn || !g_prov.notify_enabled) return -1;

    /* Find the notify characteristic attribute (index 1 in service = char value) */
    const struct bt_gatt_attr *attr = &prov_svc.attrs[2]; /* CCC is at [3] */

    return bt_gatt_notify(g_prov.conn, attr, data, len);
}

const char *app_ble_prov_get_user_id(void)
{
    return g_prov.user_id;
}

const char *app_ble_prov_get_asset_id(void)
{
    return g_prov.asset_id;
}

void app_ble_prov_send_framed(const char *json)
{
    (void)prov_send_framed(json, true);
}

const char *app_ble_prov_get_mqtt_host(void)
{
    return g_prov.mqtt_host;
}

uint16_t app_ble_prov_get_mqtt_port(void)
{
    return g_prov.mqtt_port;
}
