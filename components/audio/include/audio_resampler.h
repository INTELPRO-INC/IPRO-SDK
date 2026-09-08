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
 * @file audio_resampler.h
 * @brief Reusable audio sample rate converter
 *
 * Supports rational resampling between common audio rates using
 * FIR polyphase filters (when CONFIG_NMSIS_DSP is available) or
 * linear interpolation as fallback.
 *
 * Usage:
 *   static audio_resampler_t s_resampler;
 *   audio_resampler_config_t cfg = { .input_rate = 48000, .output_rate = 32000 };
 *   audio_resampler_init(&s_resampler, &cfg);
 *   uint32_t n = audio_resampler_process(&s_resampler, in, 480, out, 640);
 */

#ifndef AUDIO_RESAMPLER_H
#define AUDIO_RESAMPLER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Internal constants — needed for struct sizing */
#define AUDIO_RESAMPLER_NUM_TAPS       128
#define AUDIO_RESAMPLER_MAX_INPUT      480
#define AUDIO_RESAMPLER_MAX_INTERP     1920
#define AUDIO_RESAMPLER_MAX_OUTPUT     640
#define AUDIO_RESAMPLER_INTERP_MIN_L   2

/*
 * Opaque storage sizes for DSP filter instances (avoids exposing riscv_math.h).
 * riscv_fir_interpolate_instance_f32: { uint8_t L, uint16_t phaseLength,
 *                                       const float *pCoeffs, float *pState }
 * riscv_fir_decimate_instance_f32:    { uint8_t M, uint16_t numTaps,
 *                                       const float *pCoeffs, float *pState }
 * Both are 12 bytes on 32-bit, pad to 16 for alignment safety.
 */
#define AUDIO_RESAMPLER_DSP_INST_SIZE  16

/** Resampler context — statically allocatable */
typedef struct audio_resampler {
    uint8_t  mode;
    uint32_t input_rate;
    uint32_t output_rate;
    float    headroom;
    bool     initialized;

#ifdef CONFIG_NMSIS_DSP
    uint8_t  interp_factor;
    uint8_t  decimate_factor;
    float    output_gain;

    /* Opaque DSP filter instances (cast in .c file) */
    uint8_t  interp_inst[AUDIO_RESAMPLER_DSP_INST_SIZE] __attribute__((aligned(4)));
    uint8_t  decimate_inst[AUDIO_RESAMPLER_DSP_INST_SIZE] __attribute__((aligned(4)));

    float interp_state[AUDIO_RESAMPLER_NUM_TAPS / AUDIO_RESAMPLER_INTERP_MIN_L
                       + AUDIO_RESAMPLER_MAX_INPUT - 1];
    float decimate_state[AUDIO_RESAMPLER_NUM_TAPS + AUDIO_RESAMPLER_MAX_INTERP - 1];
    float interp_coeffs[AUDIO_RESAMPLER_NUM_TAPS];

    float input_f32[AUDIO_RESAMPLER_MAX_INPUT];
    float interp_f32[AUDIO_RESAMPLER_MAX_INTERP];
    float output_f32[AUDIO_RESAMPLER_MAX_OUTPUT];
#endif
} audio_resampler_t;

/** Resampler configuration */
typedef struct {
    uint32_t input_rate;         /**< Input sample rate in Hz (e.g. 48000) */
    uint32_t output_rate;        /**< Output sample rate in Hz (e.g. 32000) */
    uint32_t max_input_samples;  /**< Max samples per process call. 0 = default 480 */
    float    headroom;           /**< Input headroom 0.0-1.0. 0 = default 0.9 */
} audio_resampler_config_t;

/**
 * Initialize the resampler.
 * @return 0 on success, -1 on error (unsupported rate combination)
 */
int audio_resampler_init(audio_resampler_t *ctx, const audio_resampler_config_t *cfg);

/**
 * Reset resampler state (clear filter history). Configuration is preserved.
 */
void audio_resampler_reset(audio_resampler_t *ctx);

/**
 * Process a block of audio samples.
 * @return Number of output samples produced
 */
uint32_t audio_resampler_process(audio_resampler_t *ctx,
                                  const int16_t *in, uint32_t in_count,
                                  int16_t *out, uint32_t out_capacity);

/**
 * Calculate the expected number of output samples for a given input count.
 */
uint32_t audio_resampler_get_output_count(const audio_resampler_t *ctx, uint32_t in_count);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_RESAMPLER_H */
