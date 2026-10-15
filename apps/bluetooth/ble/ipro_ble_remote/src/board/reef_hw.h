/**
 * @file reef_hw.h
 * @brief IPRO7AI Reef hardware behind the BLE remote services
 *
 * One worker task owns the board: WS2812 LED (colour, brightness, blink),
 * OV5640 camera (JPEG capture, adaptive block motion detection; the camera
 * streams from its first use on, because a stopped DVP stream does not
 * restart reliably), on-chip
 * temperature sensor, PDM microphone level, speaker / headphone tones and the
 * SD card (each capture is also written to sd:/REMOTE/ when a card is in).
 * Callers post requests and return at once, so GATT callbacks never block the
 * BT host thread, and only this task (plus the reef demo audio task it
 * drives) touches the FPU - the IPRO7 FreeRTOS port does not save FPU state.
 *
 * Keys come from the reef demo keys.c task (SW1/BOOT, SW3, SW4, SW5).
 *
 * Copyright (c) 2026 INTELPRO
 */

#ifndef REEF_HW_H
#define REEF_HW_H

#include <stdbool.h>
#include <stdint.h>

#define REEF_HW_TEMP_INVALID    INT16_MIN
#define REEF_HW_SOUND_INVALID   (-128)

typedef struct {
    /* keys task: key 0 BOOT/SW1, 1 SW3, 2 SW4, 3 SW5 */
    void (*key)(uint8_t key, bool pressed);
    /* worker task: JPEG valid until the next capture; jpg NULL = failed */
    void (*capture_done)(const uint8_t *jpg, uint32_t len);
    /* worker task: motion started / stopped */
    void (*motion)(bool active);
    /* worker task: a new one-second microphone level, dBFS */
    void (*sound)(int8_t dbfs);
} reef_hw_callbacks_t;

/** @brief Bring the board up and start the worker. */
int reef_hw_init(const reef_hw_callbacks_t *cb);

/**
 * @brief LED from the LED service: lit when @p on; @p mode 2 / 3 blink slow /
 *        fast (led_mode_t), anything else is steady; @p brightness 0-100.
 */
void reef_hw_led(bool on, uint8_t brightness, uint8_t mode);
void reef_hw_led_color(uint8_t r, uint8_t g, uint8_t b);

/** @brief Start a JPEG capture; capture_done follows. Non-zero: no camera. */
int reef_hw_capture(void);

/** @brief Camera motion detection on / off (analysis only; the stream stays up). */
void reef_hw_motion(bool on);

/** @brief Tone on speaker + headphone; 20-12000 Hz, 1-10000 ms, 0-100 %. */
int reef_hw_tone(uint16_t freq_hz, uint16_t ms, uint8_t volume_pct);

/** @brief Keep measuring the microphone level (one-second windows). */
void reef_hw_sound_meter(bool on);

/** @brief Last chip temperature in 0.01 C, or REEF_HW_TEMP_INVALID. */
int16_t reef_hw_temperature_centi(void);

/** @brief Last microphone level in dBFS, or REEF_HW_SOUND_INVALID. */
int8_t reef_hw_sound_dbfs(void);

bool reef_hw_camera_ok(void);
bool reef_hw_sd_ok(void);

/** @brief One-line description for the Board Info characteristic. */
const char *reef_hw_info(void);

#endif /* REEF_HW_H */
