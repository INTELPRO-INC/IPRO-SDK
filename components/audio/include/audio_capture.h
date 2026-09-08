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
 * @file audio_capture.h
 * @brief Audio capture producer with multi-consumer multicast
 *
 * Supports AMIC (analog mic), DMIC (PDM digital mic), and I2S input.
 * All sources produce 16-bit mono PCM frames via DMA double-buffering.
 * Each registered consumer receives an independent copy of every frame
 * via its own FreeRTOS StreamBuffer (ISR-safe, lock-free).
 *
 * Usage:
 *   audio_capture_cfg_t cfg = {
 *       .type = AUDIO_SRC_AMIC,
 *       .sample_rate = 16000,
 *       .frame_samples = 320,
 *       .amic = { .pos_ch = 4, .single_ended = 1, .gain_db = 42 },
 *   };
 *   audio_capture_init(&cfg);
 *   int id = audio_capture_add_consumer(3200);
 *   audio_capture_start();
 *   // ... audio_capture_read(id, buf, size, timeout_ms) ...
 *   audio_capture_stop();
 */

#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_CAPTURE_MAX_CONSUMERS  4

typedef enum {
    AUDIO_SRC_AMIC,     /* Analog mic via AUADC */
    AUDIO_SRC_DMIC,     /* PDM digital mic via AUADC */
    AUDIO_SRC_I2S,      /* I2S RX interface */
} audio_src_type_t;

typedef struct {
    audio_src_type_t type;
    uint32_t sample_rate;       /* Hz, e.g. 16000 */
    uint16_t frame_samples;     /* Samples per DMA frame, e.g. 320 */

    union {
        struct {
            uint8_t pos_ch;         /* GPIO index for positive input (0-7) */
            uint8_t neg_ch;         /* GPIO index for negative input (diff mode) */
            uint8_t single_ended;   /* 1=single-ended, 0=differential */
            int     gain_db;        /* Analog gain: 6-42 dB in 3 dB steps */
        } amic;

        struct {
            uint8_t clk_pin;        /* PDM clock GPIO */
            uint8_t data_pin;       /* PDM data GPIO */
            uint8_t channel;        /* 0=left, 1=right */
            float   digital_gain_db;/* Digital gain in dB */
        } dmic;

        struct {
            uint8_t i2s_id;         /* I2S peripheral index */
            uint8_t master;         /* 1=master, 0=slave */
            uint8_t channels;       /* 1=mono, 2=stereo (will extract left ch) */
        } i2s;
    };
} audio_capture_cfg_t;

/**
 * Initialize audio capture with given source configuration.
 * Allocates DMA buffers and configures hardware, but does not start capture.
 * @return 0 on success, -1 on error
 */
int audio_capture_init(const audio_capture_cfg_t *cfg);

/**
 * De-initialize audio capture. Stops capture if running,
 * removes all consumers, frees buffers.
 */
void audio_capture_deinit(void);

/**
 * Start audio capture. Frames begin flowing to all consumers.
 * @return 0 on success, -1 if not initialized
 */
int audio_capture_start(void);

/**
 * Stop audio capture. No more frames are delivered.
 * Consumers and buffers remain valid for restart.
 * @return 0 on success
 */
int audio_capture_stop(void);

/**
 * Register a consumer. Returns consumer ID (0..MAX-1).
 * Creates a StreamBuffer of the given size for this consumer.
 * @param buf_size StreamBuffer size in bytes (should be multiple of frame_bytes)
 * @return consumer ID >= 0 on success, -1 if full or not initialized
 */
int audio_capture_add_consumer(uint32_t buf_size);

/**
 * Unregister a consumer and free its StreamBuffer.
 */
void audio_capture_remove_consumer(int consumer_id);

/**
 * Read PCM data from a consumer's StreamBuffer.
 * Blocks up to timeout_ms for data to become available.
 * @param consumer_id  Consumer ID from audio_capture_add_consumer()
 * @param buf          Output buffer for PCM data
 * @param size         Max bytes to read
 * @param timeout_ms   Timeout in milliseconds (0 = non-blocking)
 * @return bytes read, 0 on timeout
 */
size_t audio_capture_read(int consumer_id, void *buf, size_t size,
                          uint32_t timeout_ms);

/**
 * Get current capture sample rate.
 */
uint32_t audio_capture_get_sample_rate(void);

/**
 * Get samples per DMA frame.
 */
uint16_t audio_capture_get_frame_samples(void);

/**
 * Check if capture is currently running.
 */
int audio_capture_is_running(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_CAPTURE_H */
