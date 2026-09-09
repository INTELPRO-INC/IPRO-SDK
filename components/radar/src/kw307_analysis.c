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

#include "kw307_analysis.h"

#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>

#ifdef CONFIG_NMSIS_DSP
#include "riscv_math.h"
#endif

/* ------------------------------------------------------------------ */
/* Activity classification                                            */
/* ------------------------------------------------------------------ */

const char *kw307_activity_str(kw307_activity_class_t cls)
{
    switch (cls) {
    case KW307_ACT_EMPTY:     return "empty";
    case KW307_ACT_MOVING:    return "moving";
    case KW307_ACT_STILL:     return "still";
    case KW307_ACT_SETTLING:  return "settling";
    default:                  return "unknown";
    }
}

/* Range noise on a genuinely stationary target, measured 2026-08-31: a seated
 * subject wandered 22 cm peak-to-peak over ten seconds. Radial speed has to
 * clear that to mean anything, so the moving threshold is set well above it. */
#define STILL_RANGE_NOISE_MM   220
#define MOVING_SPEED_MM_S      150

bool kw307_activity(uint32_t window_s, kw307_activity_t *out)
{
    if (!out || !window_s) {
        return false;
    }

    const uint32_t want = window_s * KW307_HISTORY_RATE_HZ;
    const uint32_t have = kw307_history_count();
    if (have < KW307_HISTORY_RATE_HZ) {
        return false;                       /* less than a second of history */
    }

    const uint32_t n = want < have ? want : have;
    memset(out, 0, sizeof(*out));
    out->window_samples = (uint16_t)n;

    uint32_t present = 0, moving = 0, still = 0;
    uint32_t dist_sum = 0, dist_n = 0;
    int32_t  ang_sum = 0;
    bool saw_moving_early = false, saw_still_late = false;

    /* age 0 is the newest sample, so the loop walks time BACKWARDS. */
    uint16_t newest_dist = 0, oldest_dist = 0;

    for (uint32_t age = 0; age < n; age++) {
        kw307_sample_t s;
        if (!kw307_history_at(age, &s)) {
            break;
        }
        const bool m = (s.valid_mask & KW307_VALID_M0) != 0;
        const bool v = (s.valid_mask & KW307_VALID_STILL) != 0;

        if (m || v) present++;
        if (m) moving++;
        if (v) still++;

        /* age 0 is newest, so "early" in time is a LARGE age. */
        if (m && age > n / 2) saw_moving_early = true;
        if (v && age < n / 2) saw_still_late = true;

        const uint16_t d = m ? s.m0_dist_mm : (v ? s.s_dist_mm : 0);
        if (d) {
            dist_sum += d;
            ang_sum += m ? s.m0_ang : s.s_ang;
            if (!dist_n) {
                newest_dist = d;          /* first hit, walking backwards */
            }
            oldest_dist = d;              /* last hit is the furthest back */
            dist_n++;
        }
    }

    out->presence_pct = (uint8_t)(present * 100u / n);
    out->moving_pct   = (uint8_t)(moving * 100u / n);
    if (dist_n) {
        out->dist_mm = (uint16_t)(dist_sum / dist_n);
        out->angle_tenth = (int16_t)(ang_sum / (int32_t)dist_n);

        /* Positive means receding: newest range greater than the oldest. */
        const int32_t delta_mm = (int32_t)newest_dist - (int32_t)oldest_dist;
        const int32_t secs_x100 = (int32_t)(n * 100u / KW307_HISTORY_RATE_HZ);
        out->radial_mm_s = secs_x100 ? (int16_t)(delta_mm * 100 / secs_x100) : 0;
    }

    const int32_t speed = out->radial_mm_s < 0 ? -out->radial_mm_s : out->radial_mm_s;

    if (present * 10u < n) {                       /* under 10% of the window */
        out->cls = KW307_ACT_EMPTY;
    } else if (saw_moving_early && saw_still_late && moving * 4u < n) {
        out->cls = KW307_ACT_SETTLING;
    } else if (moving * 4u > n && speed > MOVING_SPEED_MM_S) {
        out->cls = KW307_ACT_MOVING;
    } else if (still * 2u > n) {
        out->cls = KW307_ACT_STILL;
    } else {
        out->cls = KW307_ACT_UNKNOWN;
    }
    return true;
}

/* ------------------------------------------------------------------ */
/* Breathing estimate - q15 FFT over the stationary magnitude series  */
/* ------------------------------------------------------------------ */

#ifdef CONFIG_NMSIS_DSP

#define N       KW307_BREATH_FFT_N
#define BAND_LO ((KW307_BREATH_MIN_BPM * N) / (60 * KW307_HISTORY_RATE_HZ))
#define BAND_HI ((KW307_BREATH_MAX_BPM * N) / (60 * KW307_HISTORY_RATE_HZ))

static int16_t *s_series;       /* N   */
static int16_t *s_fft;          /* 2N  - rfft_q15 writes N complex pairs */
static int16_t *s_mag;          /* N/2 */
static riscv_rfft_instance_q15 s_rfft;
static bool s_ready;

int kw307_analysis_init(void)
{
    kw307_analysis_free();

    s_series = pvPortMalloc(N * sizeof(int16_t));
    s_fft    = pvPortMalloc(2 * N * sizeof(int16_t));
    s_mag    = pvPortMalloc((N / 2) * sizeof(int16_t));
    if (!s_series || !s_fft || !s_mag) {
        kw307_analysis_free();
        return -1;
    }
    if (riscv_rfft_init_q15(&s_rfft, N, 0, 1) != RISCV_MATH_SUCCESS) {
        kw307_analysis_free();
        return -2;
    }
    s_ready = true;
    return 0;
}

void kw307_analysis_free(void)
{
    s_ready = false;
    if (s_series) { vPortFree(s_series); s_series = NULL; }
    if (s_fft)    { vPortFree(s_fft);    s_fft = NULL; }
    if (s_mag)    { vPortFree(s_mag);    s_mag = NULL; }
}

/* Remove the mean and the linear trend. Breathing rides on a baseline that
 * drifts as the subject shifts; mean removal alone leaves that drift as a
 * large low-frequency component right next to the band we care about. Integer
 * least squares - no float, see the header. */
static void detrend(int16_t *x, uint32_t n)
{
    int64_t sy = 0, sxy = 0;
    const int32_t mid_x2 = (int32_t)n - 1;          /* 2*mean(i) */

    for (uint32_t i = 0; i < n; i++) {
        sy += x[i];
        sxy += (int64_t)(2 * (int32_t)i - mid_x2) * x[i];
    }
    /* sum((2i - (n-1))^2) = n(n^2-1)/3 */
    const int64_t sxx = (int64_t)n * ((int64_t)n * n - 1) / 3;
    const int64_t mean_x1024 = (sy * 1024) / n;
    const int64_t slope_x1024 = sxx ? (sxy * 1024) / sxx : 0;   /* per unit of (2i-(n-1)) */

    for (uint32_t i = 0; i < n; i++) {
        const int64_t trend = mean_x1024 + slope_x1024 * (2 * (int32_t)i - mid_x2);
        int64_t v = (int64_t)x[i] - trend / 1024;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        x[i] = (int16_t)v;
    }
}

/* Bartlett (triangular) window, computed in integers. Hann would have ~5 dB
 * better sidelobes but needs a cosine, i.e. float - see the header on why this
 * file has none. For finding one dominant peak inside a narrow band, the
 * difference does not change the answer. */
static void window_bartlett(int16_t *x, uint32_t n)
{
    const int32_t half = (int32_t)n / 2;
    for (uint32_t i = 0; i < n; i++) {
        int32_t d = (int32_t)i - half;
        if (d < 0) d = -d;
        const int32_t w = 32767 - (d * 32767) / half;     /* q15 triangle */
        x[i] = (int16_t)(((int32_t)x[i] * w) >> 15);
    }
}

/* Scale to about half of q15 full scale. The q15 FFT divides by two at every
 * one of its log2(N) stages, so an input left at magnitude-counts scale would
 * come out of a 1024-point transform a thousand times smaller and quantise to
 * nothing. Headroom is left for the window and for the transform's own growth. */
static void normalise(int16_t *x, uint32_t n)
{
    int32_t peak = 1;
    for (uint32_t i = 0; i < n; i++) {
        const int32_t a = x[i] < 0 ? -x[i] : x[i];
        if (a > peak) peak = a;
    }
    const int32_t gain = 16384 / peak;
    if (gain <= 1) {
        return;
    }
    for (uint32_t i = 0; i < n; i++) {
        int32_t v = (int32_t)x[i] * gain;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        x[i] = (int16_t)v;
    }
}

static uint16_t band_median(const int16_t *mag, uint32_t lo, uint32_t hi)
{
    int16_t tmp[BAND_HI - BAND_LO + 1];
    const uint32_t n = hi - lo + 1;
    memcpy(tmp, &mag[lo], n * sizeof(int16_t));

    for (uint32_t i = 1; i < n; i++) {          /* insertion sort, n is ~50 */
        const int16_t v = tmp[i];
        uint32_t j = i;
        while (j && tmp[j - 1] > v) { tmp[j] = tmp[j - 1]; j--; }
        tmp[j] = v;
    }
    return (uint16_t)tmp[n / 2];
}

bool kw307_breath(kw307_breath_t *out)
{
    if (!out || !s_ready) {
        return false;
    }
    memset(out, 0, sizeof(*out));

    const uint32_t got = kw307_history_series(KW307_SERIES_STILL_MAG, N, s_series);
    if (got < N) {
        return false;                          /* not enough history yet */
    }
    out->window_samples = N;
    out->valid_samples = (uint16_t)kw307_history_series_valid(KW307_SERIES_STILL_MAG, N);

    /* A window that was mostly empty has nothing to measure; the zeros from
     * the gaps would themselves look like a slow oscillation. */
    if (out->valid_samples * 10u < N * 8u) {
        return true;                           /* valid stays false */
    }

    detrend(s_series, N);
    normalise(s_series, N);
    window_bartlett(s_series, N);

    riscv_rfft_q15(&s_rfft, s_series, s_fft);
    riscv_cmplx_mag_q15(s_fft, s_mag, N / 2);

    uint32_t peak_bin = BAND_LO;
    int16_t  peak = s_mag[BAND_LO];
    for (uint32_t b = BAND_LO; b <= BAND_HI; b++) {
        if (s_mag[b] > peak) { peak = s_mag[b]; peak_bin = b; }
    }

    const uint16_t med = band_median(s_mag, BAND_LO, BAND_HI);
    out->peak_bin = (uint16_t)peak_bin;
    out->snr_x10 = med ? (uint16_t)((uint32_t)peak * 10u / med) : 0;

    /* bin -> Hz is bin * fs / N; breaths per minute is that times 60. */
    out->rate_bpm_x10 = (uint16_t)((uint32_t)peak_bin * KW307_HISTORY_RATE_HZ * 600u / N);

    /* A peak only three times the band median is noise. This threshold is a
     * starting point - it has NOT been validated against a reference belt. */
    out->valid = out->snr_x10 >= 30;
    return true;
}

#else  /* !CONFIG_NMSIS_DSP */

int  kw307_analysis_init(void) { return -3; }
void kw307_analysis_free(void) { }
bool kw307_breath(kw307_breath_t *out) { (void)out; return false; }

#endif
