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
 * @file audio_speaker_eq.h
 * @brief Speaker correction EQ for the on-chip DAC path.
 *
 * A property of the board's speaker, not of any one application, so it lives
 * here rather than in an app: anything driving the DAC gets the same
 * correction.
 *
 * Measured on an IPRO7AI EVK with the on-board IM69D129 microphone
 * (`mic measure`, tools/audio_enhance_nn/analyse_response.py). Relative to
 * 1 kHz the raw acoustic response has a broad resonance peaking at +8.8 dB
 * near 2.8 kHz - right where the ear is most sensitive, which is what makes
 * the speaker sound thin and harsh.
 *
 * The correction is two peaking cuts, at 2850 Hz and 7225 Hz. Only features
 * that reproduced across independent measurement sessions are corrected: a
 * narrow notch that moves with microphone position is acoustic path, not
 * speaker, and chasing it would bake a room artefact into the firmware.
 *
 * Effect over 1.2 - 10 kHz: flatness 4.80 -> 2.57 dB rms, peak-to-trough
 * 16.8 -> 11.4 dB, at the cost of ~4 dB of broadband level.
 *
 * The filter only ever attenuates (peak |H| = 1.0), so it needs no headroom
 * and cannot introduce clipping.
 */

#ifndef AUDIO_SPEAKER_EQ_H
#define AUDIO_SPEAKER_EQ_H

#include <stdint.h>
#include <stdbool.h>

/** Rate the coefficients are designed for. */
#define AUDIO_SPEAKER_EQ_RATE   32000

/**
 * @brief Filter one sample.
 * @return the corrected sample, clamped to int16 range.
 *
 * Integer only. The IPRO7 FreeRTOS port does not save f0-f31 across a context
 * switch, so a float biquad here would be corrupted by - and would corrupt -
 * any other task doing float work, the LC3 decoder above all.
 */
int32_t audio_speaker_eq_sample(int32_t x);

/** @brief Clear the filter state; call when a stream starts. */
void audio_speaker_eq_reset(void);

/** @brief Turn the correction on or off (for A/B measurement). */
void audio_speaker_eq_enable(bool on);
bool audio_speaker_eq_enabled(void);

#endif /* AUDIO_SPEAKER_EQ_H */
