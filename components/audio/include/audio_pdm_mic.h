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
 * @file audio_pdm_mic.h
 * @brief The Reef's PDM microphone as 16 kHz speech-ready samples.
 *
 * One chain, shared by the voice tool (apps/ai/ipro_reef_teach/voice) and Sensor Teach's
 * `mic` source, which each had their own copy until 2026-11-01:
 *
 *   AUADC PDM capture (+18 dB digital, the maximum)
 *     -> audio_asrc from the PDM path's real rate to 16,000 Hz
 *     -> RBJ high-pass at 100 Hz, Q = 1/sqrt(2) (DC and hum are not the room)
 *     -> x10 make-up gain (+20 dB), rounded and clipped to int16
 *
 * The cutoff and the gain are compile-time constants on purpose: the filter
 * coefficients are then folded by the compiler exactly as both copies were,
 * so the samples are bit-for-bit those of the old code
 * (test_host/test_audio_pdm_mic_host.c checks it against both).
 *
 * One microphone, one owner at a time. Not reentrant.
 */
#ifndef AUDIO_PDM_MIC_H
#define AUDIO_PDM_MIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_PDM_MIC_RATE      16000u
#define AUDIO_PDM_MIC_FRAME     320u             /* capture frame, 20 ms */
#define AUDIO_PDM_MIC_HPF_HZ    100.0f
#define AUDIO_PDM_MIC_SW_GAIN   10.0f            /* +20 dB after the high-pass */
#define AUDIO_PDM_MIC_GAIN_DB   18.0f            /* AUADC maximum for PDM */
/* The PDM path runs near 2.032 MHz / 128; a Reef measured 15,871.5-15,872.1 Hz. */
#define AUDIO_PDM_MIC_IN_MHZ    15872500u

typedef struct {
    uint8_t  clk_pin, data_pin, channel;         /* BOARD_MIC_CLK_PIN, _DAT_PIN, _PDM_CHANNEL */
    uint32_t buf_bytes;                          /* capture ring; 0: 2 s */
} audio_pdm_mic_cfg_t;

/** @return 0, or -1 when the capture cannot start (nothing left open). */
int audio_pdm_mic_open(const audio_pdm_mic_cfg_t *cfg);

/**
 * @brief Up to max 16 kHz samples. Fetches one capture frame (waiting up to
 *        timeout_ms) only when the previous one is used up; its unconverted
 *        rest waits for the next call, so dst may be any size.
 * @return samples written (0 on timeout or when closed)
 */
uint32_t audio_pdm_mic_read(int16_t *dst, uint32_t max, uint32_t timeout_ms);

/** @brief True while the last capture frame still has samples to convert. */
bool audio_pdm_mic_pending(void);

void audio_pdm_mic_close(void);
bool audio_pdm_mic_is_open(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_PDM_MIC_H */
