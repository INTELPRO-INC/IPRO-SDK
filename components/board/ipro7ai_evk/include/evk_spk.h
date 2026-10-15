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
 * @file evk_spk.h
 * @brief Speaker: on-chip DAC (GPIO23) into the TPA2037D1 class-D amplifier.
 *
 * A thin, board-correct wrapper over components/audio's DAC sink: opens it
 * with this board's amplifier-enable polarity, applies a software volume in
 * Q12 (integer, no FPU), and offers a tone generator for self-test.
 *
 * The DAC's native rate is 32 kHz. Opening at another rate (16 kHz for a
 * microphone recording) goes through the sink's linear resampler. Writes
 * block until the DMA ring has room, so a producer is paced by the DAC.
 */
#ifndef EVK_SPK_H
#define EVK_SPK_H

#include <stdint.h>
#include <stdbool.h>

#include "evk_audio_port.h"

#define EVK_SPK_OK          0
#define EVK_SPK_ERR_PARAM  -1
#define EVK_SPK_ERR_STATE  -2
#define EVK_SPK_ERR_HW     -3

#define EVK_SPK_VOLUME_MAX  200     /* percent; 100 = unity, 200 = +6 dB */

/**
 * @brief Open the DAC for streaming at @p rate_hz (mono int16).
 * @param ring_count Segments in the DMA ring (0 = 4).
 * @param ring_ms    Milliseconds per segment (0 = 20). Deeper rings survive
 *                   a producer that is sometimes late; shallower ones start
 *                   sooner.
 */
int  evk_spk_open(uint32_t rate_hz, uint8_t ring_count, uint16_t ring_ms);
void evk_spk_close(void);
bool evk_spk_is_open(void);

/** @brief Write @p n samples (volume applied). Blocks on the DMA ring. */
int  evk_spk_write(const int16_t *pcm, uint32_t n);

/** @brief Write @p n samples of silence (keeps the ring fed while idle). */
int  evk_spk_write_silence(uint32_t n);

/**
 * @brief Samples written since evk_spk_open(), and samples the DAC has played
 *        (counted per DMA segment). written - played is what the sink still
 *        holds - the DAC queue an audio_asrc_ctl level reading has to include.
 *        Both count at the sink's own 32 kHz rate, so open it at 32000 to
 *        compare them with what you wrote.
 */
uint32_t evk_spk_written(void);
uint32_t evk_spk_played(void);

/** @brief Software volume, 0..EVK_SPK_VOLUME_MAX percent (default 100). */
int  evk_spk_set_volume(int pct);
int  evk_spk_get_volume(void);

/** @brief Amplifier enable (GPIO22, active high). The sink drives it on
 *         start/stop; this is for bench checks. */
void evk_spk_enable(int on);

/**
 * @brief Play a sine of @p freq_hz for @p ms at @p amp_pct of full scale.
 *        Opens and closes the DAC itself; blocking.
 */
int  evk_spk_tone(uint32_t freq_hz, uint32_t ms, uint32_t amp_pct);

#endif /* EVK_SPK_H */
