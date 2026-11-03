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
 * @brief Enrollment and live speaker labelling on the Reef's PDM microphone.
 *
 * One task owns the model, the gallery and the microphone; everything here
 * posts a request to it and returns. Results come back as text lines through
 * the sink (voice_link.c sends them to the Teach page, the shell can mirror
 * them); the line formats are listed in README.md.
 *
 * Enroll: the person reads for N seconds; every voiced 2.6 s window (1 s hop)
 * is embedded and added to their voiceprint, which is saved to the media
 * partition. Live: once a second the latest window is embedded and matched -
 * an enrolled name above the id threshold, otherwise an anonymous speaker
 * ("speaker k", online clustering) that can be named later from its centroid.
 */

#ifndef VOICE_H
#define VOICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "spk_model.h"

#define VOICE_NAME_MAX       63
#define VOICE_ENROLL_MIN_S    8
#define VOICE_ENROLL_MAX_S    30
#define VOICE_PCM_TEST_BYTES  (SPK_MODEL_SAMPLES * 2u)

typedef void (*voice_sink_t)(const char *line);

/** Model + gallery + tasks. 0, or -1 when the model is missing (still usable
 *  for status, so the page can say why). */
int  voice_init(void);
void voice_set_sink(voice_sink_t sink);
/** Mirror every event line to the console too. */
void voice_set_echo(bool on);
/** Send every voiceprint computed (enrollment windows, voiced live seconds)
 *  to the sink as a VEC line: numbers only, no audio. For tuning on a PC. */
void voice_set_vectors(bool on);

/* Requests: 0 = queued, -1 = refused (an ERR line says why). */
int voice_enroll(const char *name, uint32_t seconds);
int voice_live(bool on);
int voice_stop(void);
int voice_delete(const char *name);
/** Enroll anonymous speaker k from its live centroid under `name`. */
int voice_name_anon(int k, const char *name);
int voice_set_thresholds(float id, float anon);
/** Stranger mode: compare voiceprints with the session's common component
 *  removed and let a heard stranger keep their anonymous number (see voice.c);
 *  resets both thresholds to the mode's defaults; saved with the voiceprints. */
int voice_set_mode(bool stranger);
/** Audio source for the next enroll / live run: the PDM microphone (default)
 *  or 16 kHz PCM the host streams with voice_feed() (testing on recordings). */
int voice_set_source(bool host);
/** Host PCM into the running enroll / live; blocks while the 1 s buffer is
 *  full, which holds the USB host back. -1 when not running from the host. */
int voice_feed(const int16_t *pcm, uint32_t samples);
/** Embed SPK_MODEL_SAMPLES of 16 kHz PCM the host sent (a check of the
 *  model path, not an enrollment); answered with an EMB line. */
int voice_embed_pcm(const int16_t *pcm, uint32_t samples);

/** HELLO answer, then one SPK line per speaker and SPK END. */
void voice_report_hello(void);
void voice_report_list(void);
/** While enrolling or listening: one BANDS line with the mean, RMS and octave
 *  band levels of the last second (no audio is sent). */
void voice_report_bands(void);
/** Human-readable state for the shell. */
void voice_print_status(void);

#endif /* VOICE_H */
