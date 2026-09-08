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
 * @file app_audio.h
 * @brief Audio Processing Module
 * 
 * Handles LC3 decoding and audio output.
 */

#ifndef APP_AUDIO_H
#define APP_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Audio Configuration
 */
typedef struct {
    uint32_t sample_rate;       // Hz (8000, 16000, 24000, 32000, 48000)
    uint32_t frame_duration_us; // Microseconds (7500 or 10000)
    uint16_t octets_per_frame;  // Bytes per LC3 frame
    uint8_t num_channels;       // 1 = Mono, 2 = Stereo
} app_audio_config_t;

/*
 * Default Configuration (can be overridden by BAP)
 */
#ifdef CONFIG_AUDIO_OUTPUT_UAC
#define APP_AUDIO_DEFAULT_SAMPLE_RATE       48000   // 48 kHz (UAC passthrough)
#define APP_AUDIO_DEFAULT_OCTETS_PER_FRAME  120     // 96 kbps @ 48kHz/10ms
#else
#define APP_AUDIO_DEFAULT_SAMPLE_RATE       32000   // 32 kHz (DAC native rate)
#define APP_AUDIO_DEFAULT_OCTETS_PER_FRAME  80      // 64 kbps @ 32kHz/10ms
#endif
#define APP_AUDIO_DEFAULT_FRAME_DURATION    10000   // 10 ms
#define APP_AUDIO_DEFAULT_CHANNELS          1       // Mono

/*
 * Test Tone Configuration
 */
#define TEST_TONE_FREQUENCY_HZ      1000    // Test tone frequency (1kHz)
#define TEST_TONE_DEFAULT_AMPLITUDE 5000    // Default amplitude (0-32767)

/*
 * Statistics
 */
typedef struct {
    uint32_t frames_received;
    uint32_t frames_decoded;
    uint32_t frames_lost;
    uint32_t decode_errors;
    // Frame arrival timing
    uint32_t frame_interval_min_us;   // Minimum interval between frames
    uint32_t frame_interval_max_us;   // Maximum interval between frames
    uint32_t frame_interval_avg_us;   // Average interval (rolling)
    uint32_t late_frames;             // Frames arriving > 1.5x expected interval
    uint32_t burst_frames;            // Frames arriving < 0.5x expected interval
    // Accumulated delay tracking
    int32_t  current_delay_us;        // Current accumulated delay (negative = ahead, positive = behind)
    int32_t  max_delay_us;            // Maximum accumulated delay observed
    int32_t  min_delay_us;            // Minimum accumulated delay (most ahead)
} app_audio_stats_t;

/*
 * Public API
 */

/**
 * Initialize audio module with LC3 decoder
 */
void app_audio_init(void);

/**
 * Deinitialize audio module and cleanup hardware
 */
void app_audio_deinit(void);

/**
 * Reconfigure audio parameters (called when BAP negotiates new config)
 */
int app_audio_reconfigure(const app_audio_config_t* config);

/**
 * Process incoming LC3 frame
 * 
 * @param p_data    LC3 encoded data
 * @param length    Data length in bytes
 * @param timestamp Frame timestamp
 * @param status    Reception status (0=OK)
 * @param stream_id Stream identifier (for stereo routing)
 */
void app_audio_process_frame(const uint8_t* p_data, uint16_t length,
                              uint32_t timestamp, uint8_t status, uint8_t stream_id);

/**
 * Stop audio processing
 */
void app_audio_stop(void);

/**
 * Get current audio statistics
 */
void app_audio_get_stats(app_audio_stats_t* stats);

/**
 * Get current audio configuration
 */
const app_audio_config_t* app_audio_get_config(void);

/**
 * Play test tone (1kHz sine wave) for hardware verification
 * 
 * @param duration_ms Duration in milliseconds (0 = continuous until stopped)
 * @param amplitude   Peak amplitude (0-32767, recommend 3000-5000 for testing)
 * @return 0 on success, -1 on error
 */
int app_audio_play_test_tone(uint32_t duration_ms, int16_t amplitude);

/**
 * Play an exponential (log) sine sweep out of the audio sink.
 *
 * For measuring the acoustic response of the DAC + amplifier + speaker chain,
 * which is the target this app's NN enhancement model has to be trained
 * against.  Record it with a measurement microphone at the normal listening
 * position; the PC-side reference is regenerated from the same formula by
 * tools/audio_enhance_nn/make_sweep_ref.py, so nothing has to be transferred
 * off the board.
 *
 * The two sides do not produce bit-identical samples - the device accumulates
 * phase in double, numpy does the same but not in the same order - and that is
 * fine: the target is a per-band magnitude ratio, where phase does not appear.
 *
 * @param seconds   sweep length; 30 gives a good noise floor
 * @param f1,f2     start/end frequency in Hz
 * @param amplitude peak sample value; 10000 is about -10 dBFS, which keeps the
 *                  class-D amp out of clipping while sitting well above noise
 * @return 0 on success
 */
int app_audio_play_sweep(uint32_t seconds, uint32_t f1, uint32_t f2, int16_t amplitude);

/**
 * Stop test tone playback
 */
void app_audio_stop_test_tone(void);

/** @brief Per-stage cycle cost of the LC3 -> resample -> DAC path. */
void app_audio_profile_report(void);
void app_audio_profile_reset(void);

#ifdef __cplusplus
}
#endif

#endif // APP_AUDIO_H
