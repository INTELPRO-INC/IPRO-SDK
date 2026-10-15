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
 * @file audio.h
 * @brief Speaker / headphone playback and the three capture paths.
 *
 * Output: DAC channel A (GPIO23) feeds the TLV9061 headphone buffer and the
 * PAM8302A speaker amplifier at the same time. The only routing control is
 * the amplifier's SD# on GPIO25 (HIGH = on), so "headphone" here means
 * "amplifier muted" and "speaker" means "amplifier on"; the jack always gets
 * the signal.
 *
 * Capture: one AUADC, three front-ends -
 *   PDM   MSM261DGT003 on GPIO16 (CLK) / GPIO17 (DATA), left slot
 *   HSMIC headset microphone, differential AIN7 (GPIO13) / AIN6 (GPIO12)
 *   AUX   AUX-IN jack, left channel single-ended on AIN4 (GPIO10)
 *
 * Every job runs on a worker task so the shell returns at once; audio_stop()
 * ends the current one.
 */
#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    AUDIO_ROUTE_SPK = 0,   /* amplifier on  (speaker + headphone) */
    AUDIO_ROUTE_HP,        /* amplifier off (headphone only)      */
} audio_route_t;

typedef enum {
    AUDIO_IN_PDM = 0,
    AUDIO_IN_HSMIC,
    AUDIO_IN_AUX,
    AUDIO_IN_COUNT
} audio_in_t;

#define AUDIO_RATE_HZ        16000u
#define AUDIO_FRAME_SAMPLES  320u        /* 20 ms */
#define AUDIO_REC_MAX_SEC    10u

int  audio_init(void);

/** Sine tone. amp_pct 1..100 of full scale (keep <= 30 on the speaker). */
int  audio_tone(uint32_t freq_hz, uint32_t ms, uint32_t amp_pct, audio_route_t route);

/** Record `seconds` (<= AUDIO_REC_MAX_SEC) of 16 kHz mono from `src`. */
int  audio_record(audio_in_t src, uint32_t seconds);

/** Play back the last recording. */
int  audio_play(audio_route_t route);

/** Live monitor: `src` straight to the output for `seconds` (0 = until stop). */
int  audio_loop(audio_in_t src, uint32_t seconds, audio_route_t route);

/** Stop whatever is running and wait for the worker to release the hardware. */
void audio_stop(void);

bool audio_busy(void);
const char *audio_job_name(void);

/** Amplifier enable, directly. */
void audio_spk_enable(bool on);
bool audio_spk_enabled(void);

/** Software make-up gain applied to captured audio, dB (0..40). */
int  audio_set_gain_db(int db);
int  audio_get_gain_db(void);

/** PDM slot: 0 = left (board default), 1 = right. */
int  audio_set_pdm_channel(int ch);
int  audio_get_pdm_channel(void);

/** Peak of the last recording / monitor, in dBFS (negative), -96 if none. */
int  audio_last_peak_dbfs(void);
/** RMS of the last recording, after the start-up settle window (-96 if none). */
int  audio_last_rms_dbfs(void);
/* RMS amplitude (int16 full scale 32768) of the last take - no float */
uint32_t audio_last_rms_amp(void);
/** 300-3400 Hz RMS of the last recording (-96 if none). */
int  audio_last_band_rms_dbfs(void);
uint32_t audio_rec_samples(void);
const char *audio_src_name(audio_in_t src);

#endif /* AUDIO_H */
