/**
 * @file reef_service.c
 * @brief IPRO7AI Reef hardware service (see reef_service.h)
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "reef_service.h"

#include <string.h>

#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#include "ipro_log.h"
#include <FreeRTOS.h>
#include <task.h>

#define LOG_TAG "REEF_SVC"

#define BT_UUID_REEF_SVC        BT_UUID_DECLARE_16(REEF_SVC_UUID16)
#define BT_UUID_REEF_LED_COLOR  BT_UUID_DECLARE_16(REEF_LED_COLOR_CHAR_UUID16)
#define BT_UUID_REEF_KEY_EVENT  BT_UUID_DECLARE_16(REEF_KEY_EVENT_CHAR_UUID16)
#define BT_UUID_REEF_SOUND      BT_UUID_DECLARE_16(REEF_SOUND_CHAR_UUID16)
#define BT_UUID_REEF_TONE       BT_UUID_DECLARE_16(REEF_TONE_CHAR_UUID16)
#define BT_UUID_REEF_INFO       BT_UUID_DECLARE_16(REEF_INFO_CHAR_UUID16)

static struct {
    reef_service_handlers_t h;
    uint8_t rgb[3];
    reef_key_event_t last_key;
    int8_t sound_dbfs;
    bool key_notify;
    bool sound_notify;
} s_ctx = { .rgb = { 255, 255, 255 }, .sound_dbfs = -128 };

static ssize_t color_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, s_ctx.rgb, sizeof(s_ctx.rgb));
}

static ssize_t color_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                           const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    const uint8_t *d = buf;

    (void)conn; (void)attr; (void)flags;
    if (offset != 0u) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (len != sizeof(s_ctx.rgb)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    memcpy(s_ctx.rgb, d, sizeof(s_ctx.rgb));
    IPRO_LOGI(LOG_TAG, "[LED] Colour %u,%u,%u", d[0], d[1], d[2]);
    if (s_ctx.h.led_color) {
        s_ctx.h.led_color(d[0], d[1], d[2]);
    }
    return len;
}

static ssize_t key_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                        void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &s_ctx.last_key, sizeof(s_ctx.last_key));
}

static void key_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    (void)attr;
    s_ctx.key_notify = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[KEY] Notifications %s", s_ctx.key_notify ? "on" : "off");
}

static ssize_t sound_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          void *buf, uint16_t len, uint16_t offset)
{
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &s_ctx.sound_dbfs, sizeof(s_ctx.sound_dbfs));
}

static void sound_ccc_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    (void)attr;
    s_ctx.sound_notify = (value == BT_GATT_CCC_NOTIFY);
    IPRO_LOGI(LOG_TAG, "[MIC] Level notifications %s", s_ctx.sound_notify ? "on" : "off");
    if (s_ctx.h.sound_notify) {
        s_ctx.h.sound_notify(s_ctx.sound_notify);
    }
}

static ssize_t tone_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                          const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    reef_tone_req_t req;

    (void)conn; (void)attr; (void)flags;
    if (offset != 0u) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }
    if (len != sizeof(req)) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
    }
    memcpy(&req, buf, sizeof(req));
    if (req.freq_hz < 20u || req.freq_hz > 12000u || req.duration_ms == 0u ||
        req.duration_ms > 10000u || req.volume_pct > 100u) {
        return BT_GATT_ERR(BT_ATT_ERR_VALUE_NOT_ALLOWED);
    }
    IPRO_LOGI(LOG_TAG, "[SPK] Tone %u Hz, %u ms, %u%%", req.freq_hz, req.duration_ms, req.volume_pct);
    if (s_ctx.h.tone) {
        s_ctx.h.tone(&req);
    }
    return len;
}

static ssize_t info_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                         void *buf, uint16_t len, uint16_t offset)
{
    const char *txt = s_ctx.h.info ? s_ctx.h.info() : "IPRO7AI Reef";

    return bt_gatt_attr_read(conn, attr, buf, len, offset, txt, strlen(txt));
}

BT_GATT_SERVICE_DEFINE(reef_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_REEF_SVC),
    BT_GATT_CHARACTERISTIC(BT_UUID_REEF_LED_COLOR,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
                           color_read, color_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_REEF_KEY_EVENT,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           key_read, NULL, NULL),
    BT_GATT_CCC(key_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_REEF_SOUND,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
                           BT_GATT_PERM_READ,
                           sound_read, NULL, NULL),
    BT_GATT_CCC(sound_ccc_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
    BT_GATT_CHARACTERISTIC(BT_UUID_REEF_TONE,
                           BT_GATT_CHRC_WRITE,
                           BT_GATT_PERM_WRITE,
                           NULL, tone_write, NULL),
    BT_GATT_CHARACTERISTIC(BT_UUID_REEF_INFO,
                           BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ,
                           info_read, NULL, NULL),
);

/* Value attributes: [0] service, [1]/[2] colour, [3]/[4] key, [5] key CCC,
 * [6]/[7] sound, [8] sound CCC, [9]/[10] tone, [11]/[12] info. */
#define ATTR_KEY_VALUE      4
#define ATTR_SOUND_VALUE    7

void reef_service_init(const reef_service_handlers_t *h)
{
    if (h) {
        s_ctx.h = *h;
    }
}

void reef_service_get_color(uint8_t rgb[3])
{
    memcpy(rgb, s_ctx.rgb, sizeof(s_ctx.rgb));
}

int reef_service_notify_key(struct bt_conn *conn, uint8_t key, uint8_t event, uint32_t count)
{
    /* keys task and timer task (long press) both get here */
    reef_key_event_t ev = { .key = key, .event = event, .count = count };

    taskENTER_CRITICAL();
    s_ctx.last_key = ev;
    taskEXIT_CRITICAL();
    if (conn == NULL || !s_ctx.key_notify) {
        return 0;
    }
    return bt_gatt_notify(conn, &reef_svc.attrs[ATTR_KEY_VALUE], &ev, sizeof(ev));
}

int reef_service_notify_sound(struct bt_conn *conn, int8_t dbfs)
{
    s_ctx.sound_dbfs = dbfs;
    if (conn == NULL || !s_ctx.sound_notify) {
        return 0;
    }
    return bt_gatt_notify(conn, &reef_svc.attrs[ATTR_SOUND_VALUE], &s_ctx.sound_dbfs, sizeof(s_ctx.sound_dbfs));
}

bool reef_service_key_notify_enabled(void)
{
    return s_ctx.key_notify;
}

bool reef_service_sound_notify_enabled(void)
{
    return s_ctx.sound_notify;
}
