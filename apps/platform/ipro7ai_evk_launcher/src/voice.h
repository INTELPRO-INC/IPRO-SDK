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
 * @file voice.h
 * @brief Microphone -> PSRAM -> speaker: the launcher's voice page engine.
 *
 *   LISTEN     mic running, live level + waveform, nothing stored
 *   RECORDING  frames appended to the PSRAM clip until the length is up
 *   PLAYING    the clip out of the DAC (make-up gain, then volume)
 *
 * One worker task owns the mic and the speaker; the UI reads the status
 * struct and the waveform ring. All integer (see evk_pcm.h).
 */
#ifndef VOICE_H
#define VOICE_H

#include <stdint.h>
#include <stdbool.h>

#define VOICE_MAX_SECONDS     10
#define VOICE_DEFAULT_SECONDS 5
#define VOICE_WAVE_COLUMNS    320    /* one per 20 ms frame = 6.4 s of history */

typedef enum {
    VOICE_IDLE = 0,
    VOICE_LISTEN,
    VOICE_RECORDING,
    VOICE_PLAYING,
} voice_state_t;

typedef struct {
    voice_state_t state;
    uint32_t clip_ms;        /* length of the stored clip                      */
    uint32_t pos_ms;         /* recording / playback position                  */
    uint32_t target_ms;      /* recording length asked for                     */
    uint16_t peak, rms;      /* of the most recent 20 ms frame                 */
    int      peak_dbfs_x10;
    uint32_t frames;         /* mic frames handled since boot                  */
    uint32_t clipped;        /* playback samples that hit the rail             */
    bool     mic_ok;         /* the mic delivered at least one frame           */
    const char *error;       /* last failure, "" when none                     */
} voice_status_t;

int  voice_init(void);

/** @brief Run the mic for the live meter (page enter/leave). */
int  voice_listen(bool on);

/** @brief Record @p seconds (1..VOICE_MAX_SECONDS) then, if @p then_play,
 *         play it back. Starts the mic if needed. */
int  voice_record(uint32_t seconds, bool then_play);

/** @brief Play the stored clip. */
int  voice_play(void);

/** @brief Stop recording or playback; the mic keeps listening if it was. */
void voice_stop(void);

void voice_get_status(voice_status_t *out);
bool voice_has_clip(void);

/** @brief Make-up gain on playback, 0..40 dB (default 24). */
int  voice_set_gain_db(int db);
int  voice_get_gain_db(void);

/**
 * @brief Waveform ring: VOICE_WAVE_COLUMNS bytes, one per 20 ms frame, each
 *        the frame's peak scaled to 0..255. @p head is the index of the
 *        newest column; the oldest is (head + 1) % VOICE_WAVE_COLUMNS.
 */
const uint8_t *voice_wave(uint32_t *head);
void voice_wave_clear(void);

#endif /* VOICE_H */
