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

/*
 * On-board analysis of the KW307 history - the part the module does not do.
 *
 * The module answers "is someone there, and where". Everything here answers
 * questions that need a window of time: what are they doing, how long have
 * they been doing it, and are they breathing. All of it reads
 * kw307_history.h; none of it needs a second sensor.
 *
 * ALL FIXED POINT, DELIBERATELY. This FreeRTOS port does not save the FPU
 * context (f0-f31) across a task switch even though the build is hard-float
 * ilp32f, so any float here would be silently corrupted the moment a second
 * task also touched one. NMSIS-DSP's q15 transforms avoid the issue entirely,
 * and the module's magnitudes are uint16 to begin with, so q15 is the natural
 * domain rather than a workaround.
 */
#ifndef __KW307_ANALYSIS_H__
#define __KW307_ANALYSIS_H__

#include <stdbool.h>
#include <stdint.h>

#include "kw307_history.h"

/* What the subject is doing. Deliberately NOT a fall detector: the module
 * reports range and azimuth but no elevation, and a fall is a vertical event.
 * "Moving target stopped" is all the geometry supports, and that is the same
 * signature as sitting down. */
typedef enum {
    KW307_ACT_UNKNOWN = 0,
    KW307_ACT_EMPTY,          /* no valid target for the whole window        */
    KW307_ACT_MOVING,         /* a moving slot is Valid and the range changes */
    KW307_ACT_STILL,          /* present, but only the stationary slot holds  */
    KW307_ACT_SETTLING,       /* moving -> stationary inside this window      */
} kw307_activity_class_t;

typedef struct {
    kw307_activity_class_t cls;
    uint16_t dist_mm;         /* representative target range, 0 when empty   */
    int16_t  angle_tenth;
    int16_t  radial_mm_s;     /* signed: positive = receding                 */
    uint8_t  presence_pct;    /* share of the window with any valid target   */
    uint8_t  moving_pct;      /* share of the window with a valid moving slot */
    uint16_t window_samples;
} kw307_activity_t;

/* Breathing estimate from the stationary magnitude series.
 *
 * HONEST LIMITATION: the module exposes magnitude, not phase. 24 GHz breathing
 * detection normally works on phase (lambda/2 = 6.21 mm is a full turn, so one
 * degree is 17.2 um), and magnitude is what survives the module's CFAR and
 * tracker - possibly after its own gain control. So this is an amplitude
 * proxy, and `snr_x10` is what says whether to believe a given estimate. Treat
 * a low-SNR result as "no reading", not as a slow breath. */
typedef struct {
    bool     valid;
    uint16_t rate_bpm_x10;    /* breaths per minute x10 */
    uint16_t peak_bin;
    uint16_t snr_x10;         /* band peak / band median, x10 */
    uint16_t window_samples;
    uint16_t valid_samples;   /* of those, how many had a Valid stationary slot */
} kw307_breath_t;

/* FFT length over the stationary magnitude series. 1024 samples at 20 Hz is a
 * 51.2 s window with 0.0195 Hz (1.17 breaths/min) resolution - comfortable for
 * the 6..36 breaths/min band that matters. */
#define KW307_BREATH_FFT_N     1024
#define KW307_BREATH_MIN_BPM   6
#define KW307_BREATH_MAX_BPM   36

/* Allocate the transform scratch (about 7 KB, from the PSRAM heap). Returns 0,
 * or negative if the heap cannot satisfy it or the DSP library is absent. */
int  kw307_analysis_init(void);
void kw307_analysis_free(void);

/* Classify the last `window_s` seconds. Returns false when the history holds
 * less than a second of it. */
bool kw307_activity(uint32_t window_s, kw307_activity_t *out);

const char *kw307_activity_str(kw307_activity_class_t cls);

/* Estimate the breathing rate over the last KW307_BREATH_FFT_N samples.
 * Returns false when the history is too short or the analysis was never
 * initialised; `out->valid` additionally reflects whether the peak cleared the
 * SNR floor. */
bool kw307_breath(kw307_breath_t *out);

#endif /* __KW307_ANALYSIS_H__ */
