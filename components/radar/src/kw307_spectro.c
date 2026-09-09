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

#include "kw307_spectro.h"

#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>

#ifdef CONFIG_NMSIS_DSP
#include "riscv_math.h"
#endif

uint32_t kw307_spectro_bin_mhz(uint32_t bin)
{
    /* bin * fs / N, in milli-hertz. */
    return bin * KW307_SPECTRO_RATE_HZ * 1000u / KW307_SPECTRO_FFT_N;
}

#ifdef CONFIG_NMSIS_DSP

#define SRC_SAMPLES  KW307_SPECTRO_NEED_SAMPLES
#define DEC_SAMPLES  (SRC_SAMPLES / KW307_SPECTRO_DECIMATE)

static int16_t *s_src;      /* SRC_SAMPLES at 20 Hz  */
static int16_t *s_dec;      /* DEC_SAMPLES at 5 Hz   */
static int16_t *s_win;      /* FFT_N working copy    */
static int16_t *s_fft;      /* 2 * FFT_N             */
static int16_t *s_mag;      /* FFT_N / 2             */
static riscv_rfft_instance_q15 s_rfft;
static bool s_ready;

int kw307_spectro_init(void)
{
    kw307_spectro_free();

    s_src = pvPortMalloc(SRC_SAMPLES * sizeof(int16_t));
    s_dec = pvPortMalloc(DEC_SAMPLES * sizeof(int16_t));
    s_win = pvPortMalloc(KW307_SPECTRO_FFT_N * sizeof(int16_t));
    s_fft = pvPortMalloc(2 * KW307_SPECTRO_FFT_N * sizeof(int16_t));
    s_mag = pvPortMalloc((KW307_SPECTRO_FFT_N / 2) * sizeof(int16_t));
    if (!s_src || !s_dec || !s_win || !s_fft || !s_mag) {
        kw307_spectro_free();
        return -1;
    }
    if (riscv_rfft_init_q15(&s_rfft, KW307_SPECTRO_FFT_N, 0, 1) != RISCV_MATH_SUCCESS) {
        kw307_spectro_free();
        return -2;
    }
    s_ready = true;
    return 0;
}

void kw307_spectro_free(void)
{
    s_ready = false;
    if (s_src) { vPortFree(s_src); s_src = NULL; }
    if (s_dec) { vPortFree(s_dec); s_dec = NULL; }
    if (s_win) { vPortFree(s_win); s_win = NULL; }
    if (s_fft) { vPortFree(s_fft); s_fft = NULL; }
    if (s_mag) { vPortFree(s_mag); s_mag = NULL; }
}

/* Box-average decimation. A boxcar is a crude anti-alias filter, but it is the
 * one that averaging four adjacent samples already is, and the alias it lets
 * through sits above 2.5 Hz where this image has nothing to say anyway. */
static void decimate(const int16_t *src, uint32_t n_src, int16_t *dst)
{
    for (uint32_t i = 0; i + KW307_SPECTRO_DECIMATE <= n_src;
         i += KW307_SPECTRO_DECIMATE) {
        int32_t acc = 0;
        for (uint32_t k = 0; k < KW307_SPECTRO_DECIMATE; k++) {
            acc += src[i + k];
        }
        dst[i / KW307_SPECTRO_DECIMATE] = (int16_t)(acc / KW307_SPECTRO_DECIMATE);
    }
}

/* Remove mean and linear trend - see kw307_analysis.c for why this is integer
 * least squares and not a float fit. Per column, because the baseline drifts
 * across the whole span and each column should be judged on its own. */
static void detrend(int16_t *x, uint32_t n)
{
    int64_t sy = 0, sxy = 0;
    const int32_t mid_x2 = (int32_t)n - 1;

    for (uint32_t i = 0; i < n; i++) {
        sy += x[i];
        sxy += (int64_t)(2 * (int32_t)i - mid_x2) * x[i];
    }
    const int64_t sxx = (int64_t)n * ((int64_t)n * n - 1) / 3;
    const int64_t mean_x1024 = (sy * 1024) / n;
    const int64_t slope_x1024 = sxx ? (sxy * 1024) / sxx : 0;

    for (uint32_t i = 0; i < n; i++) {
        const int64_t trend = mean_x1024 + slope_x1024 * (2 * (int32_t)i - mid_x2);
        int64_t v = (int64_t)x[i] - trend / 1024;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        x[i] = (int16_t)v;
    }
}

static void window_bartlett(int16_t *x, uint32_t n)
{
    const int32_t half = (int32_t)n / 2;
    for (uint32_t i = 0; i < n; i++) {
        int32_t d = (int32_t)i - half;
        if (d < 0) d = -d;
        const int32_t w = 32767 - (d * 32767) / half;
        x[i] = (int16_t)(((int32_t)x[i] * w) >> 15);
    }
}

/* The q15 transform halves at each of its log2(N) stages, so an input left at
 * magnitude-counts scale would quantise to nothing. Returns the gain applied so
 * the caller can undo it and keep columns comparable to each other - without
 * that, a per-column normalisation would erase exactly the amplitude
 * differences the image is supposed to show. */
static int32_t normalise(int16_t *x, uint32_t n)
{
    int32_t peak = 1;
    for (uint32_t i = 0; i < n; i++) {
        const int32_t a = x[i] < 0 ? -x[i] : x[i];
        if (a > peak) peak = a;
    }
    int32_t gain = 16384 / peak;
    if (gain < 1) {
        gain = 1;
    }
    if (gain > 1) {
        for (uint32_t i = 0; i < n; i++) {
            int32_t v = (int32_t)x[i] * gain;
            if (v > 32767) v = 32767;
            if (v < -32768) v = -32768;
            x[i] = (int16_t)v;
        }
    }
    return gain;
}

/* Integer log2 in 1/16ths of an octave, for log compression without a float
 * logarithm. Energy in these images spans several decades; plotted linearly,
 * everything but the strongest cell reads as zero. */
static uint32_t log2_x16(uint32_t v)
{
    if (!v) {
        return 0;
    }
    uint32_t msb = 31u - (uint32_t)__builtin_clz(v);
    /* Interpolate inside the octave using the bits below the MSB. */
    uint32_t frac = msb ? ((v << (31 - msb)) & 0x7FFFFFFFu) >> 27 : 0;
    return msb * 16u + frac;
}

bool kw307_spectro_build(uint8_t *out, kw307_spectro_info_t *info)
{
    if (!out || !s_ready) {
        return false;
    }
    if (kw307_history_count() < SRC_SAMPLES) {
        return false;
    }

    const uint32_t got = kw307_history_series(KW307_SERIES_STILL_MAG,
                                              SRC_SAMPLES, s_src);
    if (got < SRC_SAMPLES) {
        return false;
    }

    const uint32_t valid = kw307_history_series_valid(KW307_SERIES_STILL_MAG,
                                                      SRC_SAMPLES);
    if (info) {
        memset(info, 0, sizeof(*info));
        info->span_s = (uint16_t)(SRC_SAMPLES / KW307_HISTORY_RATE_HZ);
        info->valid_samples = (uint16_t)valid;
    }
    /* Gaps read as zero, and a run of zeros is itself a strong low-frequency
     * component - it would show up as breathing. Refuse rather than invent. */
    if (valid * 10u < SRC_SAMPLES * 8u) {
        return false;
    }

    decimate(s_src, SRC_SAMPLES, s_dec);
    memset(out, 0, KW307_SPECTRO_CELLS);

    uint32_t global_peak = 0, peak_bin = 0;

    for (uint32_t step = 0; step < KW307_SPECTRO_STEPS; step++) {
        const uint32_t off = step * KW307_SPECTRO_HOP;
        memcpy(s_win, &s_dec[off], KW307_SPECTRO_FFT_N * sizeof(int16_t));

        detrend(s_win, KW307_SPECTRO_FFT_N);
        const int32_t gain = normalise(s_win, KW307_SPECTRO_FFT_N);
        window_bartlett(s_win, KW307_SPECTRO_FFT_N);

        riscv_rfft_q15(&s_rfft, s_win, s_fft);
        riscv_cmplx_mag_q15(s_fft, s_mag, KW307_SPECTRO_FFT_N / 2);

        for (uint32_t b = 0; b < KW307_SPECTRO_BINS; b++) {
            /* Bin 0 is DC, already removed; start the image at bin 1 so the
             * top row is the slowest motion that survives detrending. */
            uint32_t m = (uint32_t)s_mag[b + 1];
            /* Undo the per-column gain so columns stay comparable. */
            m = gain > 1 ? m / (uint32_t)gain : m;

            const uint32_t l = log2_x16(m + 1u);
            uint32_t v = l * 255u / (16u * 12u);        /* 12 octaves full scale */
            if (v > 255u) v = 255u;

            out[step * KW307_SPECTRO_BINS + b] = (uint8_t)v;
            if (v > global_peak) { global_peak = v; peak_bin = b; }
        }
    }

    if (info) {
        info->peak = (uint8_t)global_peak;
        info->peak_bin = (uint8_t)peak_bin;
        info->peak_mhz = (uint16_t)kw307_spectro_bin_mhz(peak_bin + 1u);
    }
    return true;
}

#else  /* !CONFIG_NMSIS_DSP */

int  kw307_spectro_init(void) { return -3; }
void kw307_spectro_free(void) { }
bool kw307_spectro_build(uint8_t *out, kw307_spectro_info_t *info)
{
    (void)out; (void)info;
    return false;
}

#endif

static char cell_char(uint8_t v)
{
    static const char ramp[] = " .:-=+*#%@";
    uint32_t i = (uint32_t)v * 9u / 255u;
    if (i > 9u) i = 9u;
    return ramp[i];
}

void kw307_spectro_print(const uint8_t *img, const kw307_spectro_info_t *info)
{
    if (!img) {
        return;
    }
    printf("magnitude spectrogram: %u s span, %u samples Valid, peak bin %u "
           "(%u.%02u /min)\r\n",
           info ? info->span_s : 0, info ? info->valid_samples : 0,
           info ? info->peak_bin : 0,
           info ? (unsigned)(info->peak_mhz * 60u / 1000u) : 0,
           info ? (unsigned)((info->peak_mhz * 6u) % 100u) : 0);
    printf("        oldest %*s newest\r\n", KW307_SPECTRO_STEPS - 14, "->");

    for (uint32_t b = 0; b < KW307_SPECTRO_BINS; b++) {
        char line[KW307_SPECTRO_STEPS + 1];
        for (uint32_t c = 0; c < KW307_SPECTRO_STEPS; c++) {
            line[c] = cell_char(img[c * KW307_SPECTRO_BINS + b]);
        }
        line[KW307_SPECTRO_STEPS] = '\0';
        /* Label in cycles per minute - breaths, not hertz. */
        const uint32_t cpm = kw307_spectro_bin_mhz(b + 1u) * 60u / 1000u;
        printf("%3lu/min |%s|\r\n", (unsigned long)cpm, line);
    }
}

void kw307_spectro_dump_csv(const uint8_t *img, const kw307_spectro_info_t *info,
                            const char *label)
{
    if (!img) {
        return;
    }
    printf("SPEC,%s,%u,%u,%u,%u,%u",
           label ? label : "-", KW307_SPECTRO_BINS, KW307_SPECTRO_STEPS,
           info ? info->span_s : 0, info ? info->peak_bin : 0,
           info ? info->peak_mhz : 0);

    for (uint32_t b = 0; b < KW307_SPECTRO_BINS; b++) {
        for (uint32_t c = 0; c < KW307_SPECTRO_STEPS; c++) {
            printf(",%u", img[c * KW307_SPECTRO_BINS + b]);
        }
    }
    printf("\r\n");
}
