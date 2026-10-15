/**
 * @file reef_service.h
 * @brief IPRO7AI Reef hardware service (0xFF80)
 *
 * What the Reef board has beyond the generic remote services, as one extra
 * service so the original LED / button / environment / UART / image / OTA
 * services keep their exact layout for existing phone apps:
 *
 *   0xFF81 LED Colour   R/W     3 bytes R, G, B (0-255). The LED service's
 *                               on/off, brightness and blink mode apply on top.
 *   0xFF82 Key Event    R/N     6 bytes: key (0 BOOT/SW1, 1 SW3, 2 SW4, 3 SW5),
 *                               event (btn_event_t), press count (u32 LE)
 *   0xFF83 Sound Level  R/N     int8: PDM microphone level in dBFS over the
 *                               last second; notifications start the meter
 *   0xFF84 Tone         W       5 bytes: frequency Hz (u16 LE), duration ms
 *                               (u16 LE), volume % (u8) - speaker + headphone
 *   0xFF85 Board Info   R       UTF-8 text: board, camera, SD card, sensors
 *
 * Copyright (c) 2026 INTELPRO
 */

#ifndef REEF_SERVICE_H
#define REEF_SERVICE_H

#include <stdbool.h>
#include <stdint.h>

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/conn.h>

#define REEF_SVC_UUID16             0xFF80
#define REEF_LED_COLOR_CHAR_UUID16  0xFF81
#define REEF_KEY_EVENT_CHAR_UUID16  0xFF82
#define REEF_SOUND_CHAR_UUID16      0xFF83
#define REEF_TONE_CHAR_UUID16       0xFF84
#define REEF_INFO_CHAR_UUID16       0xFF85

typedef struct __attribute__((packed)) {
    uint8_t  key;
    uint8_t  event;
    uint32_t count;
} reef_key_event_t;

typedef struct __attribute__((packed)) {
    uint16_t freq_hz;
    uint16_t duration_ms;
    uint8_t  volume_pct;
} reef_tone_req_t;

typedef struct {
    void (*led_color)(uint8_t r, uint8_t g, uint8_t b);   /* BT host thread */
    void (*tone)(const reef_tone_req_t *req);             /* BT host thread */
    void (*sound_notify)(bool on);                        /* BT host thread */
    const char *(*info)(void);                            /* BT host thread */
} reef_service_handlers_t;

/** @brief Install the handlers (call before advertising). */
void reef_service_init(const reef_service_handlers_t *h);

/** @brief Current colour, as the phone last wrote it (default white). */
void reef_service_get_color(uint8_t rgb[3]);

/** @brief Notify a key event; no-op unless the phone enabled it. */
int reef_service_notify_key(struct bt_conn *conn, uint8_t key, uint8_t event, uint32_t count);

/** @brief Notify the sound level; no-op unless the phone enabled it. */
int reef_service_notify_sound(struct bt_conn *conn, int8_t dbfs);

bool reef_service_key_notify_enabled(void);
bool reef_service_sound_notify_enabled(void);

#endif /* REEF_SERVICE_H */
