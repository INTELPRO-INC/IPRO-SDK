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
 * @file audio_bass_boost.h
 * @brief Psychoacoustic bass enhancement for a speaker with no bass.
 *
 * The IPRO7AI EVK speaker delivers -32.6 dB at 250-400 Hz with a 24 dB cliff
 * between 400 and 630 Hz, yet 76-89% of the energy in captured LE Audio clips
 * sits below 400 Hz. That energy never becomes sound; it only consumes
 * amplifier power and drives the cone to excursions it cannot make, which
 * intermodulates the frequencies that do get out.
 *
 * Two things happen here, in order of how much they matter:
 *
 *   1. The unreproducible band is removed. Measured on the captured clips this
 *      cuts RMS by 6.0-7.5 dB - amplifier power, not digital headroom: the
 *      peaks are broadband transients, so peak level barely moves.
 *   2. A harmonic series derived from the removed band is put back at
 *      550-1600 Hz, where the speaker works. The ear reconstructs the absent
 *      fundamental from the spacing of its harmonics, so bass is perceived
 *      without any being radiated.
 *
 * Works well for upper bass and poorly for deep bass, unavoidably: the third
 * harmonic of 200 Hz is 600 Hz and lands in the usable band, while 60 Hz needs
 * its tenth harmonic to get there and pitch inference from harmonics that high
 * is weak.
 *
 * Integer throughout. The IPRO7 FreeRTOS port does not save f0-f31 across a
 * context switch, so float here would race the LC3 decoder.
 */

#ifndef AUDIO_BASS_BOOST_H
#define AUDIO_BASS_BOOST_H

#include <stdint.h>
#include <stdbool.h>

/** Rate the coefficients are designed for. */
#define AUDIO_BASS_BOOST_RATE   32000

/** @brief Process one sample. Returns the enhanced sample, int16 range. */
int32_t audio_bass_boost_sample(int32_t x);

/** @brief Clear filter and envelope state; call when a stream starts. */
void audio_bass_boost_reset(void);

/** @brief Enable/disable. Off by default - a nonlinear process would corrupt
 *         the acoustic sweep measurement, and it should be an explicit A/B. */
void audio_bass_boost_enable(bool on);
bool audio_bass_boost_enabled(void);

/**
 * @brief How much synthesised harmonic to mix in, as a Q8 fraction.
 *        256 = the harmonic envelope matches the original bass envelope.
 *        Lower is subtler; start around 128 and listen.
 */
void audio_bass_boost_set_gain_q8(uint32_t g_q8);
uint32_t audio_bass_boost_get_gain_q8(void);

#endif /* AUDIO_BASS_BOOST_H */
