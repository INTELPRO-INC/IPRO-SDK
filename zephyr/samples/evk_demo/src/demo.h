/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef EVK_DEMO_H_
#define EVK_DEMO_H_

#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <stdint.h>
#include <stdbool.h>

/* RGB565 helpers */
#define RGB565(r, g, b) ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))
#define C_BLACK   RGB565(0, 0, 0)
#define C_WHITE   RGB565(255, 255, 255)
#define C_RED     RGB565(255, 0, 0)
#define C_GREEN   RGB565(0, 255, 0)
#define C_BLUE    RGB565(0, 0, 255)
#define C_YELLOW  RGB565(255, 255, 0)
#define C_CYAN    RGB565(0, 255, 255)
#define C_MAGENTA RGB565(255, 0, 255)

/* lcd.c */
int lcd_init(void);
uint16_t lcd_width(void);
uint16_t lcd_height(void);
int lcd_fill(uint16_t color);
int lcd_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
int lcd_bars(void);
int lcd_checker(uint16_t cell);
int lcd_gradient(void);
/** Blit a w x h RGB565 frame (row-major, wire byte order handled here). */
int lcd_blit(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *rgb565);
int lcd_backlight(bool on);
int lcd_write_raw(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const void *wire_pixels, bool last);
bool lcd_big_endian(void);
/** Horizontal level bar at the bottom of the panel, 0..1000 permille. */
int lcd_level_bar(uint16_t permille, uint16_t color);

/* audio.c */
int audio_init(void);
int audio_mic_level(const struct shell *sh, uint32_t seconds, bool on_lcd);
int audio_mic_record(const struct shell *sh, uint32_t seconds);
int audio_mic_dump(const struct shell *sh);
int audio_set_gain_shift(int shift);
int audio_play_recording(const struct shell *sh);
int audio_tone(const struct shell *sh, uint32_t hz, uint32_t ms);
int audio_loopback(const struct shell *sh, uint32_t seconds);
uint32_t audio_recorded_samples(void);
uint32_t audio_rate(void);

/* cam.c (CONFIG_EVK_DEMO_CAMERA) */
int cam_init(void);
int cam_start(const struct shell *sh);
int cam_stop(const struct shell *sh);
int cam_snap(const struct shell *sh);
bool cam_running(void);
int cam_set_fps(const struct shell *sh, uint32_t n);
int cam_set_mirror(const struct shell *sh, bool on);
int cam_pixels(const struct shell *sh);

#endif /* EVK_DEMO_H_ */
