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
 * @brief Speaker (on-chip DAC + TPA2037D1) and PDM microphone bring-up.
 *
 * Built on the SDK's audio_output / audio_capture components, with the two
 * board-specific corrections described in audio_port.h: the amplifier enable
 * is active high (the components assume active low), and the mic is the left
 * PDM slot on GPIO18/19.
 *
 * Only one activity runs at a time - starting a new one stops the previous.
 */
#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#include "audio_output.h"

#define AUDIO_OK          0
#define AUDIO_ERR_PARAM  -1
#define AUDIO_ERR_NOMEM  -2
#define AUDIO_ERR_STATE  -3
#define AUDIO_ERR_HW     -4

/** What the worker task is currently doing. */
typedef enum {
    AUDIO_MODE_IDLE = 0,
    AUDIO_MODE_TONE,     /* synthesised sine to the DAC          */
    AUDIO_MODE_REC,      /* PDM capture into the PSRAM buffer    */
    AUDIO_MODE_PLAY,     /* PSRAM buffer out to the DAC          */
    AUDIO_MODE_LOOP,     /* live PDM -> DAC                      */
    AUDIO_MODE_SCOPE,    /* live PDM waveform on the LCD         */
    AUDIO_MODE_MIC,      /* capture only, no output              */
    AUDIO_MODE_MEASURE,  /* duplex sweep out + mic into PSRAM    */
} audio_mode_t;

/** Peak/RMS over the most recent measurement window. */
typedef struct {
    float    peak_dbfs;
    float    rms_dbfs;
    uint32_t frames;      /* 20 ms frames processed        */
    uint32_t clipped;     /* samples that hit full scale   */
    uint32_t overruns;    /* capture frames dropped        */
} audio_level_t;

/**
 * @brief Open/close the DAC with this board's amplifier polarity corrected.
 *
 * Exposed so other sinks - the LE Audio decoder in bt_audio_out.c - can write
 * PCM without re-implementing the TPA2037D1 workaround described above.
 * The caller owns the handle and writes with audio_output_write().
 */
audio_output_t *audio_dac_open(uint32_t rate);
audio_output_t *audio_dac_open_ex(uint32_t rate, uint8_t ring_count,
                                  uint16_t ring_ms);
void audio_dac_close(audio_output_t *out);

/** @brief Amplifier enable (GPIO22). Active high on this board. */
void audio_spk_enable(int on);
int  audio_spk_is_enabled(void);

/** @brief Apply the current make-up gain, saturating. In-place is allowed. */
void audio_gain_apply(const int16_t *src, int16_t *dst, uint32_t n);

/** @brief Hard mute, independent of the gain setting. */
void audio_set_mute(int on);
int  audio_get_mute(void);

/** @brief Software make-up gain applied before the DAC, -60..+48 dB. */
int  audio_set_gain(int db);
int  audio_get_gain(void);

/** @brief AUADC digital gain applied inside the capture path, 0..+18 dB.
 *         Takes effect the next time capture starts. */
int  audio_set_mic_gain(int db);
int  audio_get_mic_gain(void);

/** @brief The gain the AUADC reports back, which is not necessarily the gain
 *         that was requested - the init path re-runs AUADC_Init(). */
float audio_get_mic_gain_actual(void);

/** @brief Make-up gain the recorded peak can still absorb before clipping. */
int  audio_headroom_db(void);

/** @brief Set the make-up gain and play in one step. */
int  audio_play_gain(int db);

/** @brief Play using a make-up gain derived from the recorded peak. */
int  audio_play_auto(void);

/** @brief Play a synthesised sine. amp_pct is percent of full scale. */
int audio_tone(uint32_t freq_hz, uint32_t seconds, uint32_t amp_pct);

/** @brief Play a DC code on the DAC for multimeter checks (0..1023).
 *
 * Keep the amplifier OFF while stepping codes: a full-scale step is ~1.5 V
 * through the DC-blocking cap and the class-D will turn it into a rail-to-rail
 * transient. The measurement point is before that cap anyway. */
int audio_dac_dc(int code);

/** @brief Lend the PSRAM record buffer to an external producer, so its output
 *         can be replayed with `mic play` through the known-good DAC path. */
int16_t *audio_capture_buffer(uint32_t *cap_samples);
void     audio_capture_commit(uint32_t samples);

/**
 * @brief Acoustic frequency-response measurement.
 *
 * Plays a 1 s pilot tone then a @p seconds logarithmic sweep from @p f1 to
 * @p f2 out of the DAC, recording the result with the PDM mic into PSRAM.
 * Dump it with audio_dump_b64() and analyse on the host.
 *
 * Capture runs at a higher rate than AUDIO_RATE, so the sweep may reach well
 * past the 7.9 kHz ceiling the normal capture path imposes.
 */
int audio_measure(uint32_t seconds, uint32_t f1, uint32_t f2, uint32_t amp_pct);

/** @brief Base64 the recording to the console for host-side analysis. */
int audio_dump_b64(void);

/** @brief Record from the mic into PSRAM for up to @p seconds. */
int audio_record(uint32_t seconds);

/** @brief Play back whatever audio_record() captured. */
int audio_play(void);

/** @brief Live PDM -> DAC for @p seconds (0 = until `spk stop`).
 *
 * Expect howling if the speaker can reach the microphone - that is acoustic
 * feedback, not excess gain. A reported input peak of exactly 32768 is the
 * signature: it means samples pinned to the negative rail, which int16 can
 * only reach on the negative side. */
int audio_loop(uint32_t seconds);

/** @brief Live microphone waveform scrolling across the panel. */
int audio_scope(uint32_t seconds);

/** @brief Capture only, no output - for level checks without feedback. */
int audio_mic_start(void);

/** @brief Stop whatever is running and idle the amplifier. */
void audio_stop(void);

audio_mode_t audio_get_mode(void);
void audio_get_level(audio_level_t *out);

/** @brief AUADC completion-ISR frame count since boot, and frames the ISR
 *         could not hand on. The first number separates "the microphone path
 *         is not running at all" from "it runs but the data is not arriving",
 *         which is the only distinction worth making when nothing records. */
uint32_t audio_isr_frames(void);
uint32_t audio_isr_dropped(void);

/**
 * @brief Software PDM master: clock GPIO18 by hand and sample GPIO19.
 *
 * Bypasses the AUADC entirely, so it separates "the mic and its wiring are
 * fine, the AUADC just is not reaching these pins" from "the mic is not
 * responding at all". A live PDM mic answers with a stream that toggles
 * constantly even in silence; a dead rail or a pin still owned by the crystal
 * oscillator gives a line stuck at one level.
 *
 * The achieved clock is reported because a negative result only means
 * something if the clock was fast enough to bring the mic out of standby.
 *
 * Leaves the pins as GPIO - the mic needs a reboot afterwards.
 */
int audio_mic_probe(uint32_t *out_clk_hz, uint32_t *out_ones,
                    uint32_t *out_edges, uint32_t *out_samples);

/** @brief Print recorded samples, to tell "no frames arrived" from "frames
 *         arrived but every sample is zero". */
int audio_dump_samples(uint32_t offset, uint32_t n);

/** @brief PDM slot select. Only settable before the first capture, because
 *         hal_auadc_pdm_init() may not be re-run. */
int audio_set_channel(int right);
int audio_get_channel(void);

/** @brief Dump the GLB and HBN pad registers for GPIO18/19, to show which
 *         block actually owns the pins. */
void audio_dump_pads(void);

/** @brief Write HBN_PAD_CTRL_2 directly, so the AON ownership and ie_smt
 *         fields can be swept on the bench instead of guessed at. The reset
 *         default is 0; this board boots with 0xC00. */
void audio_write_pad_ctrl2(uint32_t val);

/** @brief Nominal and measured stream rates, for reporting. */
uint32_t audio_get_rate(void);
uint32_t audio_recorded_samples(void);

#endif /* AUDIO_H */
