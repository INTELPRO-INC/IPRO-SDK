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
 * @file audio_output.h
 * @brief Pluggable audio output sink abstraction
 *
 * Provides a uniform interface for writing PCM audio to different
 * output backends. Each backend handles its own buffering and transport.
 *
 * Backends:
 *   DAC  - On-chip DAC (one-shot blocking OR N-ring streaming, resamples
 *          internally to 32 kHz)
 *   UAC  - USB Audio Class with cyclic DMA ring buffer
 *   I2S  - I2S external codec
 *   DUMP - Raw PCM dump to SD card or custom hook (CDC, network, etc.)
 *
 * Streaming usage:
 *   audio_output_cfg_t cfg = {
 *       .type        = AUDIO_OUTPUT_DAC,
 *       .mode        = AUDIO_OUTPUT_MODE_STREAMING,
 *       .sample_rate = 44100,
 *       .channels    = 2,
 *       .ring_buf_count = 3,   // 0 = backend default
 *       .ring_buf_ms    = 64,  // 0 = backend default
 *   };
 *   audio_output_t *out = audio_output_create(&cfg);
 *   audio_output_start(out);
 *   while (have_pcm) audio_output_write(out, pcm, samples);
 *   audio_output_stop(out);
 *   audio_output_destroy(out);
 *
 * One-shot usage:
 *   audio_output_cfg_t cfg = {
 *       .type        = AUDIO_OUTPUT_DAC,
 *       .mode        = AUDIO_OUTPUT_MODE_ONESHOT,
 *       .sample_rate = 16000,
 *       .channels    = 1,
 *   };
 *   audio_output_t *out = audio_output_create(&cfg);
 *   audio_output_start(out);
 *   audio_output_write(out, pcm, total_samples);  // blocks until DMA done
 *   audio_output_stop(out);
 *   audio_output_destroy(out);
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2026 IPRO Technology Co., Ltd.
 */

#ifndef AUDIO_OUTPUT_H
#define AUDIO_OUTPUT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_OUTPUT_DAC,   /**< On-chip DAC (resamples to 32 kHz if needed) */
    AUDIO_OUTPUT_UAC,   /**< USB Audio Class output */
    AUDIO_OUTPUT_I2S,   /**< I2S external codec */
    AUDIO_OUTPUT_DUMP,  /**< Raw PCM dump (SD card file or custom hook) */
} audio_output_type_t;

typedef enum {
    AUDIO_OUTPUT_MODE_STREAMING = 0, /**< Default: N-ring cyclic DMA,
                                          non-blocking write (continuous) */
    AUDIO_OUTPUT_MODE_ONESHOT,       /**< Single blocking DMA transfer
                                          of a complete buffer */
} audio_output_mode_t;

/**
 * Dump write hook - called for each PCM block when type = AUDIO_OUTPUT_DUMP.
 * @param pcm      Interleaved int16_t samples
 * @param samples  Number of samples per channel
 * @param ctx      User context from audio_output_cfg_t.dump_ctx
 */
typedef void (*audio_dump_hook_t)(const int16_t *pcm, uint32_t samples, void *ctx);

typedef struct {
    audio_output_type_t type;
    audio_output_mode_t mode;       /**< Default: STREAMING (0) */
    uint32_t sample_rate;           /**< PCM sample rate in Hz */
    uint8_t  channels;              /**< Channel count (1 or 2) */

    /* Streaming-mode tuning (DAC). 0 = backend default. */
    uint8_t  ring_buf_count;        /**< N in N-ring (0 -> backend default) */
    uint16_t ring_buf_ms;           /**< ms per ring segment (0 -> default) */

    /* DAC output-hardware tuning. -1 = backend default. */
    int8_t   dac_spk_gpio;          /**< Speaker enable GPIO. 0 (the
                                         designated-initializer default)
                                         OR negative -> use board default.
                                         Only positive values override. */
    int8_t   dac_spk_en_active;     /**< GPIO level that turns the amplifier
                                         ON. 0 (the designated-initializer
                                         default) -> use the board header's
                                         BOARD_SPK_EN_ACTIVE. Use +1 to force
                                         active high or -1 to force active
                                         low; 0 cannot mean "active low"
                                         here because it is indistinguishable
                                         from an unset field. */

    /* AUDIO_OUTPUT_DUMP config (ignored by other backends) */
    const char        *dump_path;   /**< SD card path (used when dump_hook is NULL) */
    audio_dump_hook_t  dump_hook;   /**< Custom write hook (NULL = file dump) */
    void              *dump_ctx;    /**< Hook user context */
} audio_output_cfg_t;

/** Opaque output handle */
typedef struct audio_output audio_output_t;

/**
 * Create an output sink.
 * @return Handle, or NULL on error.
 */
audio_output_t *audio_output_create(const audio_output_cfg_t *cfg);

/**
 * Destroy output sink and free resources.
 */
void audio_output_destroy(audio_output_t *out);

/**
 * Start the output.
 * @return 0 on success
 */
int audio_output_start(audio_output_t *out);

/**
 * Stop the output.
 * @return 0 on success
 */
int audio_output_stop(audio_output_t *out);

/**
 * Write PCM samples to the output.
 *
 * STREAMING mode: buffers into ring; blocks briefly only when ring is full.
 * ONESHOT mode:   blocks until the whole buffer has been played by DMA.
 *
 * @param out      Output handle
 * @param pcm      Interleaved int16_t samples
 * @param samples  Number of samples per channel (frames)
 * @return 0 on success, -1 on error
 */
/**
 * @brief Samples the DAC backend has actually played, since start.
 *
 * Streaming mode only; 0 otherwise. Derived from DMA segment completions, so
 * it reflects the hardware's true rate rather than the nominal one the config
 * asked for - the two are not the same on IPRO7.
 */
uint32_t audio_output_dac_consumed(audio_output_t *out);

/**
 * @brief Samples the sink has actually played, since start, for any backend.
 *
 * Same contract as audio_output_dac_consumed() but dispatched on the handle's
 * type, so it is safe on a UAC handle (where it counts USB SOF-driven DMA
 * segments).  Returns 0 for backends with no drain counter and in one-shot
 * mode.  Prefer this over the DAC-specific call in code that can be pointed
 * at more than one sink.
 */
uint32_t audio_output_consumed(audio_output_t *out);

/**
 * @brief Samples per DMA segment - the quantum of audio_output_consumed().
 *
 * A drift measurement can resolve no finer than this, so use it to decide
 * whether an observed difference is real or just quantisation.
 */
uint32_t audio_output_segment_samples(audio_output_t *out);

int audio_output_write(audio_output_t *out,
                       const int16_t *pcm, uint32_t samples);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_OUTPUT_H */
