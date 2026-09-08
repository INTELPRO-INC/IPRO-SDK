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
 * @file app_dac.h
 * @brief DAC Output Module for LE Audio Headset (Triple Buffer Mode)
 *
 * Hardware DAC output using DMA triple buffering for seamless,
 * glitch-free audio playback with improved underrun tolerance.
 *
 * Triple Buffer Architecture:
 *   [Buffer 0] -> [Buffer 1] -> [Buffer 2] -> [Buffer 0] (circular)
 *
 *   While DMA plays Buffer N:
 *     - Buffer (N+1) should already be filled (ready)
 *     - Buffer (N+2) is being filled by application
 *
 *   This provides 2 buffer periods of tolerance for frame timing jitter,
 *   compared to only 1 buffer period with ping-pong (double buffering).
 *
 * Usage Pattern (Triple Buffer Mode):
 *   1. app_dac_init()              - Initialize hardware
 *   2. app_dac_feed(samples, n)    - Fill buffers 0, 1, 2
 *   3. app_dac_start()             - Start DMA playback
 *   4. Loop:
 *      - app_dac_wait_buffer_needed()  - Wait for callback
 *      - app_dac_feed(samples, n)      - Fill completed buffer
 *   5. app_dac_stop()              - Stop playback
 *   6. app_dac_deinit()            - Cleanup
 */

#ifndef APP_DAC_H
#define APP_DAC_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Buffer Configuration (shared with app_audio.c)
 *
 * ┌─────────────────────────────────────────────────────────────────┐
 * │ TUNABLE PARAMETERS - Adjust these values for your needs        │
 * └─────────────────────────────────────────────────────────────────┘
 *
 * DAC_BUFFER_DURATION_MS: Duration per buffer segment in milliseconds.
 *   Larger = more tolerance for frame timing jitter, but higher latency.
 *   Smaller = lower latency, but more susceptible to underruns.
 *   Recommended: 64-128ms for low latency, 128-256ms for stability
 *
 * DAC_BUFFER_COUNT: Number of buffer segments (2=ping-pong, 3=triple, 4=quad).
 *   2 = Ping-pong mode (lower latency, less tolerance)
 *   3 = Triple buffer mode (balanced latency and tolerance)
 *   4 = Quad buffer mode (highest tolerance for timing jitter)
 *
 * Total latency = DAC_BUFFER_DURATION_MS × DAC_BUFFER_COUNT
 *   Example: 128ms × 4 = 512ms total latency (absorbs 64ms frame bursts)
 */
#define DAC_BUFFER_DURATION_MS      128  // Keep 128ms per buffer
#define DAC_BUFFER_COUNT            4    // Increase to 4 (quad buffer) for burst tolerance
#define DAC_PREBUFFER_COUNT         3    // Start after 3/4 buffers filled (75%)

/*
 * Hardware Configuration
 */
#define DAC_GPIO_OUTPUT             23      // DAC analog output GPIO
#define DAC_GPIO_SPEAKER_EN         22      // Speaker/amplifier enable GPIO

/*
 * Level that turns the amplifier ON.
 *
 * This is a board property, not a chip one, and the two boards disagree:
 * IPRO7AI EVK drives a TPA2037D1 whose EN is ACTIVE HIGH (see the EVK's
 * audio_port.h, SPK_EN_ACTIVE = 1).  This file previously hardcoded the
 * opposite - "0 = unmute" - so on the EVK it muted the amplifier exactly when
 * playback started and unmuted it on stop.  The symptom is silence with a
 * completely healthy data path: DAC ready, buffers sent, zero underruns, zero
 * errors, because everything up to the analogue output really is working.
 *
 * components/audio/src/audio_output_dac.c has the same active-low assumption
 * baked in, which is why the EVK app calls audio_spk_enable(1) after
 * audio_output_start() to undo it.
 *
 * Override in the app's .config or here if a board wires it the other way.
 */
#ifndef DAC_SPK_EN_ACTIVE
#define DAC_SPK_EN_ACTIVE           1       // 1 = amplifier ON when pin high
#endif
#define DAC_SILENCE_VALUE           512     // DAC center value (10-bit: 0-1023)

/* Derived values - DO NOT MODIFY DIRECTLY */
#define DAC_SAMPLE_RATE_HZ          32000
#define DAC_SAMPLES_PER_SEGMENT     ((DAC_BUFFER_DURATION_MS * DAC_SAMPLE_RATE_HZ) / 1000)

/* Legacy alias for compatibility */
#define DAC_PP_SAMPLES_PER_HALF     DAC_SAMPLES_PER_SEGMENT

/*
 * Configuration
 */
typedef struct {
    uint32_t sample_rate;     ///< Sample rate in Hz (8000, 16000, 32000)
    uint8_t  channels;        ///< Number of channels (1=mono, 2=stereo)
    uint8_t  volume;          ///< Volume level (0-100) [NOT IMPLEMENTED]
    bool     gain_2x;         ///< Enable 2x digital gain (default: false)
} app_dac_config_t;

/*
 * Statistics
 */
typedef struct {
    uint32_t buffers_sent;        ///< Total buffer halves played
    uint32_t underruns;           ///< Buffer underrun count (silence filled)
    uint32_t errors;              ///< Error count
    // Enhanced underrun tracking
    uint32_t consecutive_underruns;   ///< Current consecutive underrun count
    uint32_t max_consecutive_underruns; ///< Max consecutive underruns observed
    uint32_t last_underrun_time_ms;   ///< Timestamp of last underrun (ms since boot)
    // Buffer level tracking
    uint8_t  min_buffers_ready;       ///< Minimum buffers ready observed (excluding underrun)
    uint8_t  buffers_ready_at_underrun; ///< Buffers ready when underrun occurred
} app_dac_stats_t;

/*
 * Initialization / Cleanup
 */

/**
 * @brief Initialize DAC module (Ping-Pong mode)
 * @param config Initial configuration (NULL for defaults: 32kHz mono)
 * @return 0 on success, negative on error
 */
int app_dac_init(const app_dac_config_t* config);

/**
 * @brief Deinitialize DAC module
 */
void app_dac_deinit(void);

/**
 * @brief Reconfigure DAC (sample rate, channels)
 * @param config New configuration
 * @return 0 on success, negative on error
 */
int app_dac_reconfigure(const app_dac_config_t* config);

/*
 * Ping-Pong Playback Control
 */

/**
 * @brief Start DMA ping-pong playback
 *
 * Call app_dac_feed() at least once before starting.
 * DMA will continuously cycle between PING and PONG buffers.
 *
 * @return 0 on success, negative on error
 */
int app_dac_start(void);

/**
 * @brief Stop DMA playback
 * @return 0 on success
 */
int app_dac_stop(void);

/**
 * @brief Feed audio samples to the inactive buffer
 *
 * Converts int16_t PCM to DAC format and fills the buffer
 * that is not currently being played by DMA.
 *
 * @param samples PCM buffer (int16_t format, -32768 to +32767)
 * @param count Number of samples (ideally app_dac_get_buffer_size())
 * @return 0 on success, negative on error
 */
int app_dac_feed(const int16_t* samples, uint32_t count);

/**
 * @brief Wait for buffer to need filling
 *
 * Blocks until DMA callback signals that a buffer has been consumed.
 * Use this in your audio processing loop.
 *
 * @param timeout_ms Timeout in milliseconds (UINT32_MAX = forever)
 * @return true if buffer needs filling, false on timeout
 */
bool app_dac_wait_buffer_needed(uint32_t timeout_ms);

/**
 * @brief Get index of buffer that needs filling (0 or 1)
 */
uint8_t app_dac_get_fill_index(void);

/**
 * @brief Get samples per buffer segment
 * @return Number of samples each buffer segment can hold
 */
uint32_t app_dac_get_buffer_size(void);

/**
 * @brief Get count of ready (filled) buffers
 * @return Number of buffers with valid data (0 to DAC_BUFFER_COUNT)
 */
uint8_t app_dac_get_ready_count(void);

/*
 * Legacy / Convenience API
 */

/**
 * @brief Write PCM samples (legacy interface)
 *
 * Feeds samples to inactive buffer. If auto_start is true and
 * playback is not running, starts playback automatically.
 *
 * @param samples PCM buffer (int16_t format)
 * @param count Number of samples
 * @param auto_start Start playback if not running
 * @return 0 on success, negative on error
 */
int app_dac_write(const int16_t* samples, uint32_t count, bool auto_start);

/*
 * Status / Configuration
 */

/**
 * @brief Set volume (0-100 maps to linear gain 0.0-1.0)
 * @param volume Volume level (0-100)
 */
void app_dac_set_volume(uint8_t volume);

/**
 * @brief Set linear gain directly
 *
 * Provides finer control than volume percentage.
 * Note: With FIR input headroom of 0.7, gain up to ~1.43 won't clip.
 *       Gain > 1.43 may cause clipping on loud signals.
 *
 * @param gain Linear gain (0.0 = mute, 1.0 = unity, 2.0 = +6dB max)
 */
/*
 * Linear range of the DAC path. Samples above this are soft-limited 4:1
 * before the shift to DAC counts, so anything that needs a linear response
 * (acoustic measurement, distortion tests) must keep its peak below it.
 */
#define DAC_KNEE                    5120

/** @brief Report measured cycles-per-sample for the DAC conversion chain. */
void app_dac_profile_report(void);
void app_dac_profile_reset(void);

void app_dac_set_gain(float gain);

/**
 * @brief Get current linear gain
 * @return Current gain value (0.0 - 2.0)
 */
float app_dac_get_gain(void);

/**
 * @brief Get DAC statistics
 * @param stats Output statistics
 */
void app_dac_get_stats(app_dac_stats_t* stats);

/**
 * @brief Check if DAC is initialized
 * @return true if initialized
 */
bool app_dac_is_ready(void);

#endif // APP_DAC_H
