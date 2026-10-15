/**
 * @file remote_reef.c
 * @brief BLE remote on the IPRO7AI Reef (see remote_reef.h)
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "remote_reef.h"
#include "remote_app.h"
#include "pir_image_service.h"
#include "reef_service.h"
#include "reef_hw.h"
#include "ipro_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#define LOG_TAG             "REMOTE_REEF"

#define REEF_KEYS           4u
#define REEF_KEY_BOOT       0u
#define KEY_LONG_MS         1000u
#define KEY_DOUBLE_MS       400u
#define TEMP_FALLBACK       2500        /* 25.00 C until the sensor answers */

typedef struct {
    TimerHandle_t long_timer;
    TickType_t last_release;
    uint32_t count;
    bool pressed;
    bool long_sent;
} key_state_t;

static key_state_t s_keys[REEF_KEYS];

/* ---------------------------------------------------------------------------
 * Keys: SW1/BOOT, SW3, SW4, SW5
 * -------------------------------------------------------------------------*/

static void key_emit(uint8_t key, btn_event_t ev)
{
    struct bt_conn *conn = remote_app_get_context()->connected ? remote_app_get_conn() : NULL;

    if (conn) {
        (void)remote_notify_button(conn, ev);       /* legacy 0xFF21: any key */
    }
    (void)reef_service_notify_key(conn, key, (uint8_t)ev, s_keys[key].count);
}

static void key_long_cb(TimerHandle_t t)
{
    uint8_t key = (uint8_t)(uintptr_t)pvTimerGetTimerID(t);

    if (key < REEF_KEYS && s_keys[key].pressed) {
        s_keys[key].long_sent = true;
        key_emit(key, BTN_EVENT_LONG_PRESS);
    }
}

/* keys task context */
static void key_changed(uint8_t key, bool pressed)
{
    key_state_t *k;
    TickType_t now = xTaskGetTickCount();

    if (key >= REEF_KEYS) {
        return;
    }
    k = &s_keys[key];
    k->pressed = pressed;
    if (pressed) {
        k->count++;
        k->long_sent = false;
        key_emit(key, BTN_EVENT_PRESSED);
        if (k->last_release != 0 && now - k->last_release < pdMS_TO_TICKS(KEY_DOUBLE_MS)) {
            key_emit(key, BTN_EVENT_DOUBLE_CLICK);
            k->last_release = 0;                    /* a third press starts over */
        }
        if (k->long_timer) {
            (void)xTimerReset(k->long_timer, 0);
        }
        return;
    }
    if (k->long_timer) {
        (void)xTimerStop(k->long_timer, 0);
    }
    key_emit(key, BTN_EVENT_RELEASED);
    if (k->long_sent) {
        k->last_release = 0;
        return;
    }
    k->last_release = now;
    if (key == REEF_KEY_BOOT) {
        IPRO_LOGI(LOG_TAG, "BOOT key: photo");
        (void)image_capture();
    }
}

/* ---------------------------------------------------------------------------
 * Board callbacks (reef_hw worker task)
 * -------------------------------------------------------------------------*/

static void capture_done(const uint8_t *jpg, uint32_t len)
{
    (void)image_capture_complete(jpg, len);
}

static void motion_changed(bool active)
{
    (void)pir_report_motion(active);
}

static void sound_level(int8_t dbfs)
{
    struct bt_conn *conn = remote_app_get_context()->connected ? remote_app_get_conn() : NULL;

    (void)reef_service_notify_sound(conn, dbfs);
}

static int capture_hook(void)
{
    return reef_hw_capture();
}

/* ---------------------------------------------------------------------------
 * Reef service handlers (BT host thread)
 * -------------------------------------------------------------------------*/

static void svc_color(uint8_t r, uint8_t g, uint8_t b)
{
    reef_hw_led_color(r, g, b);
}

static void svc_tone(const reef_tone_req_t *req)
{
    (void)reef_hw_tone(req->freq_hz, req->duration_ms, req->volume_pct);
}

static void svc_sound_notify(bool on)
{
    reef_hw_sound_meter(on);
}

/* ---------------------------------------------------------------------------
 * API
 * -------------------------------------------------------------------------*/

int remote_reef_init(void)
{
    static const reef_hw_callbacks_t cb = {
        .key = key_changed,
        .capture_done = capture_done,
        .motion = motion_changed,
        .sound = sound_level,
    };
    static const reef_service_handlers_t svc = {
        .led_color = svc_color,
        .tone = svc_tone,
        .sound_notify = svc_sound_notify,
        .info = reef_hw_info,
    };

    for (uint32_t i = 0; i < REEF_KEYS; i++) {
        s_keys[i].long_timer = xTimerCreate("key_long", pdMS_TO_TICKS(KEY_LONG_MS), pdFALSE,
                                            (void *)(uintptr_t)i, key_long_cb);
    }
    reef_service_init(&svc);
    image_set_capture_hook(capture_hook);
    return reef_hw_init(&cb);
}

void remote_reef_led(const led_state_t *state)
{
    reef_hw_led(state->on, state->brightness, (uint8_t)state->mode);
}

void remote_reef_connection(bool connected)
{
    if (!connected) {
        reef_hw_motion(false);
        reef_hw_sound_meter(false);
    }
}

void remote_reef_pir_notify(bool on)
{
    reef_hw_motion(on);
}

int16_t remote_reef_temperature(void)
{
    int16_t t = reef_hw_temperature_centi();

    return (t == REEF_HW_TEMP_INVALID) ? TEMP_FALLBACK : t;
}
