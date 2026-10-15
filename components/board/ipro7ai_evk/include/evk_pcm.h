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
 * @file evk_pcm.h
 * @brief Integer-only PCM helpers shared by the microphone and speaker paths.
 *
 * No libm, no float: the FreeRTOS port on IPRO7 does not save the FPU
 * registers across a context switch, so two tasks using floating point
 * corrupt each other silently. Audio runs beside an NPU task here.
 */
#ifndef EVK_PCM_H
#define EVK_PCM_H

#include <stdint.h>

/** @brief Peak (max |x|, 0..32768) and RMS (0..32767) of @p n samples. */
void evk_pcm_level(const int16_t *pcm, uint32_t n, uint16_t *peak, uint16_t *rms);

/** @brief 20*log10(v / 32768) in tenths of a dB. v = 0 gives -1200. */
int  evk_pcm_dbfs_x10(uint32_t v);

/** @brief Gain in Q12 (4096 = 0 dB) for -60..+48 dB, table-driven. */
int32_t evk_pcm_gain_q12(int db);

/** @brief dst = clamp(src * gain_q12 >> 12). In-place allowed.
 *  @return Number of samples that clipped. */
uint32_t evk_pcm_apply_gain(const int16_t *src, int16_t *dst, uint32_t n,
                            int32_t gain_q12);

/**
 * @brief Fill @p n samples of a sine wave.
 * @param phase   32-bit phase accumulator, carried between calls.
 * @param step    Phase increment per sample = freq * 2^32 / sample_rate
 *                (use evk_pcm_sine_step()).
 * @param amp_q15 Amplitude, 0..32767.
 */
void evk_pcm_sine_fill(int16_t *dst, uint32_t n, uint32_t *phase, uint32_t step,
                       int32_t amp_q15);
uint32_t evk_pcm_sine_step(uint32_t freq_hz, uint32_t rate_hz);

/** @brief Integer square root (floor). */
uint32_t evk_pcm_isqrt(uint32_t v);

/** @brief log2(v) in Q8 (v >= 1). log2(0) is reported as 0. */
uint32_t evk_pcm_ilog2_q8(uint32_t v);

#endif /* EVK_PCM_H */
