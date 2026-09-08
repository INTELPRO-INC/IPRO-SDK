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
 * @file app_uac.h
 * @brief UAC Output Module for LE Audio Headset
 *
 * Streams decoded PCM audio to USB Host as a capture device (USB Audio Class v1.0).
 * The device appears as a USB microphone to the PC.
 *
 * Uses a SPSC lock-free ring buffer to decouple BLE ISO timing (10ms LC3 frames)
 * from USB SOF timing (1ms isochronous transfers).
 *
 * Usage:
 *   1. app_uac_init(config)         - Initialize USB stack + ring buffer
 *   2. app_uac_write(samples, n, auto_start) - Push PCM from BLE task
 *      (UAC task internally pulls from ring buffer at USB SOF rate)
 *   3. app_uac_stop()               - Stop streaming
 *   4. app_uac_deinit()             - Cleanup
 */

#ifndef APP_UAC_H
#define APP_UAC_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Ring Buffer Configuration
 *
 * UAC_RING_BUFFER_MS: Total ring buffer capacity in milliseconds.
 *   Should be large enough to handle BLE ISO timing variations.
 *
 * UAC_PREBUFFER_MS: Pre-buffer threshold before starting USB streaming.
 *   Trade-off: Higher value = more underrun protection, but higher latency.
 *   - 30ms: Low latency, but may underrun with BLE jitter
 *   - 60ms: Balanced (6 LC3 frames @ 10ms)
 *   - 80ms: Conservative (8 LC3 frames @ 10ms)
 *
 * BLE LE Audio typically has 10-50ms jitter between ISO events.
 * Recommended: 60-80ms prebuffer for stable playback.
 */
#define UAC_RING_BUFFER_MS          200
#define UAC_PREBUFFER_MS            80

/*
 * USB Endpoint
 */
#define UAC_AUDIO_IN_EP             0x81

/*
 * UAC endpoint sample rate (Hz).
 *
 * 48 kHz matches the LC3 rate the phone negotiates for media, so app_audio.c's
 * resampler drops into passthrough and the decoded PCM reaches USB untouched.
 * At 48 kHz mono 16-bit the isochronous packet is 96 bytes/ms; the EP
 * max-packet field (USB_CR_EP1_SIZE) is 11 bits wide, so the only ceiling is
 * the EP1 TX FIFO depth - measure it with `uac_test fifo` before assuming a
 * higher rate fits.
 */
#define UAC_OUTPUT_RATE_HZ          48000U

/*
 * Configuration
 */
typedef struct {
    uint32_t sample_rate;     ///< Hz - matches LE Audio decoded rate
    uint8_t  channels;        ///< 1=mono, 2=stereo
} app_uac_config_t;

/*
 * Statistics
 */
typedef struct {
    uint32_t frames_sent;              ///< USB frames transmitted
    uint32_t underruns;                ///< Silence inserted (ring empty)
    uint32_t overruns;                 ///< Data dropped (ring full)
    uint32_t errors;                   ///< USB/DMA errors
    uint32_t ring_level_min;           ///< Min fill level observed (samples)
    uint32_t ring_level_max;           ///< Max fill level observed (samples)
    uint32_t consecutive_underruns;    ///< Current consecutive underrun count
    uint32_t max_consecutive_underruns;///< Max consecutive underruns observed
    uint32_t last_underrun_time_ms;    ///< Timestamp of last underrun
    uint32_t ring_level_at_underrun;   ///< Ring level when underrun occurred
    uint32_t samples_written;          ///< Total samples written to ring buffer
    uint32_t samples_read;             ///< Total samples read from ring buffer
    uint32_t restarts;                 ///< DMA restart count (underrun recovery)
} app_uac_stats_t;

/*
 * Public API
 */

/**
 * @brief Initialize UAC output module
 * @param config Audio configuration (sample rate, channels)
 * @return 0 on success, negative on error
 */
int app_uac_init(const app_uac_config_t *config);

/**
 * @brief Deinitialize UAC module
 */
void app_uac_deinit(void);

/**
 * @brief Write PCM samples to UAC ring buffer
 *
 * Called from BLE task context after LC3 decode.
 * Data is consumed by the internal UAC task at USB SOF rate.
 *
 * @param samples PCM buffer (int16_t format)
 * @param count Number of samples
 * @param auto_start Start streaming if prebuffer threshold reached
 * @return 0 on success, negative on error
 */
int app_uac_write(const int16_t *samples, uint32_t count, bool auto_start);

/**
 * @brief Stop UAC streaming
 */
void app_uac_stop(void);

/**
 * @brief Get UAC statistics
 */
void app_uac_get_stats(app_uac_stats_t *stats);

/**
 * @brief Check if UAC is initialized and USB host has opened the stream
 */
bool app_uac_is_ready(void);

/**
 * @brief Start test tone generation (for debugging without BLE)
 * @param freq_hz Tone frequency in Hz (e.g., 1000 for 1kHz)
 * @param duration_sec Duration in seconds (0 = continuous until stop)
 * @return 0 on success, negative on error
 */
int app_uac_test_tone(uint32_t freq_hz, uint32_t duration_sec);

#endif /* APP_UAC_H */
