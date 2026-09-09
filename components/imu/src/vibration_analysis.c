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

#include <generated/autoconf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vibration_analysis.h"

/* ------------------------------------------------------------------ */
/* FFT working buffers — PSRAM when available, else OCRAM             */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_USE_PSRAM
static float s_fft_real[VIB_FFT_SIZE] __attribute__((section(".psram_data")));
static float s_fft_imag[VIB_FFT_SIZE] __attribute__((section(".psram_data")));
static float s_fft_mag[VIB_FFT_SIZE / 2] __attribute__((section(".psram_data")));
#else
static float s_fft_real[VIB_FFT_SIZE];
static float s_fft_imag[VIB_FFT_SIZE];
static float s_fft_mag[VIB_FFT_SIZE / 2];
#endif

/* Band configuration */
static vib_band_t s_bands[VIB_NUM_BANDS];

/* RPM tracking for order-based bands */
static float s_rpm      = 0.0f;
static bool  s_auto_rpm = true;

/* Frequency resolution: sample_rate / FFT_size */
#define FREQ_RES  ((float)VIB_SAMPLE_RATE / VIB_FFT_SIZE)

/* ------------------------------------------------------------------ */
/* Hanning window (pre-computed once)                                 */
/* ------------------------------------------------------------------ */
#ifdef CONFIG_USE_PSRAM
static float s_window[VIB_FFT_SIZE] __attribute__((section(".psram_data")));
#else
static float s_window[VIB_FFT_SIZE];
#endif
static bool s_window_ready = false;

static void compute_hanning_window(int n)
{
    for (int i = 0; i < n; i++) {
        s_window[i] = 0.5f * (1.0f - cosf(2.0f * (float)M_PI * i / (n - 1)));
    }
    /* Zero-pad region */
    for (int i = n; i < VIB_FFT_SIZE; i++) {
        s_window[i] = 0.0f;
    }
    s_window_ready = true;
}

/* ------------------------------------------------------------------ */
/* Radix-2 Cooley-Tukey FFT (in-place, decimation-in-time)            */
/* ------------------------------------------------------------------ */
static void fft_radix2(float *re, float *im, int n)
{
    /* Bit-reversal permutation */
    int j = 0;
    for (int i = 0; i < n - 1; i++) {
        if (i < j) {
            float tr = re[i]; re[i] = re[j]; re[j] = tr;
            float ti = im[i]; im[i] = im[j]; im[j] = ti;
        }
        int m = n >> 1;
        while (m >= 1 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }

    /* Butterfly stages */
    for (int step = 2; step <= n; step <<= 1) {
        int half = step >> 1;
        float angle = -(float)M_PI / half;  /* -2π/step = -π/half */
        float wr = cosf(angle);
        float wi = sinf(angle);

        for (int group = 0; group < n; group += step) {
            float cur_r = 1.0f, cur_i = 0.0f;
            for (int pair = 0; pair < half; pair++) {
                int a = group + pair;
                int b = a + half;
                float tr = cur_r * re[b] - cur_i * im[b];
                float ti = cur_r * im[b] + cur_i * re[b];
                re[b] = re[a] - tr;
                im[b] = im[a] - ti;
                re[a] += tr;
                im[a] += ti;
                /* Rotate twiddle factor */
                float tmp = cur_r * wr - cur_i * wi;
                cur_i = cur_r * wi + cur_i * wr;
                cur_r = tmp;
            }
        }
    }
}

/* ------------------------------------------------------------------ */
/* Time-domain feature extraction (one axis)                          */
/* ------------------------------------------------------------------ */
static void compute_time_features(const int16_t *raw, uint16_t n,
                                  float scale, vib_time_features_t *out)
{
    if (n == 0) {
        memset(out, 0, sizeof(*out));
        return;
    }

    double sum = 0, sum_sq = 0;
    int16_t vmin = raw[0], vmax = raw[0];

    for (uint16_t i = 0; i < n; i++) {
        int16_t v = raw[i];
        sum += v;
        sum_sq += (double)v * v;
        if (v < vmin) vmin = v;
        if (v > vmax) vmax = v;
    }

    double mean     = sum / n;
    double variance = (sum_sq / n) - (mean * mean);

    /* AC-coupled peak: max absolute deviation from mean (removes gravity DC) */
    double ac_peak = 0;
    for (uint16_t i = 0; i < n; i++) {
        double dev = fabs((double)raw[i] - mean);
        if (dev > ac_peak) ac_peak = dev;
    }

    double ac_rms = sqrt(variance > 0 ? variance : 0);

    out->mean         = (float)(mean * scale);
    out->rms          = (float)(ac_rms * scale);
    out->peak         = (float)(ac_peak * scale);
    out->peak_to_peak = (float)((int)(vmax - vmin) * scale);
    out->variance     = (float)(variance * scale * scale);
    out->crest_factor = (ac_rms > 0) ? (float)(ac_peak / ac_rms) : 0.0f;
}

/* ------------------------------------------------------------------ */
/* Frequency-domain feature extraction (one axis)                     */
/* ------------------------------------------------------------------ */
static void compute_freq_features(const int16_t *raw, uint16_t n,
                                  float scale, vib_freq_features_t *out)
{
    /* Compute mean (DC offset) to subtract before FFT.
     * Gravity on Z-axis creates a massive DC spike that leaks into bin 1
     * (~1 Hz) via sidelobes, causing dominant_freq to always read ~1 Hz
     * on a stationary sensor. Removing the mean eliminates this. */
    double mean_sum = 0;
    for (int i = 0; i < (int)n; i++) {
        mean_sum += raw[i];
    }
    float raw_mean = (n > 0) ? (float)(mean_sum / n) : 0.0f;

    /* Prepare FFT input: subtract DC, apply window and scale, zero-pad */
    for (int i = 0; i < (int)n && i < VIB_FFT_SIZE; i++) {
        s_fft_real[i] = (raw[i] - raw_mean) * scale * s_window[i];
        s_fft_imag[i] = 0.0f;
    }
    for (int i = n; i < VIB_FFT_SIZE; i++) {
        s_fft_real[i] = 0.0f;
        s_fft_imag[i] = 0.0f;
    }

    /* Run FFT */
    fft_radix2(s_fft_real, s_fft_imag, VIB_FFT_SIZE);

    /* Compute magnitude spectrum (only first half — Nyquist) */
    int half = VIB_FFT_SIZE / 2;
    float max_amp = 0;
    int max_bin = 1;  /* Skip DC bin 0 */

    for (int i = 0; i < half; i++) {
        float mag = sqrtf(s_fft_real[i] * s_fft_real[i] +
                          s_fft_imag[i] * s_fft_imag[i]);
        s_fft_mag[i] = mag / VIB_FFT_SIZE;  /* Normalize */

        if (i > 0 && s_fft_mag[i] > max_amp) {
            max_amp = s_fft_mag[i];
            max_bin = i;
        }
    }

    out->dominant_freq = max_bin * FREQ_RES;
    out->dominant_amp  = max_amp;

    /* Compute power in each band */
    for (int b = 0; b < VIB_NUM_BANDS; b++) {
        float power = 0;
        if (s_bands[b].freq_lo >= s_bands[b].freq_hi) {
            out->band_power[b] = 0;
            continue;
        }

        int bin_lo = (int)(s_bands[b].freq_lo / FREQ_RES);
        int bin_hi = (int)(s_bands[b].freq_hi / FREQ_RES);
        if (bin_lo < 1) bin_lo = 1;       /* Skip DC */
        if (bin_hi >= half) bin_hi = half - 1;

        for (int i = bin_lo; i <= bin_hi; i++) {
            power += s_fft_mag[i] * s_fft_mag[i];  /* Sum of squared magnitudes */
        }
        out->band_power[b] = power;
    }
}

/* ------------------------------------------------------------------ */
/* Order-based band calculation                                       */
/* ------------------------------------------------------------------ */

/**
 * Recompute bands 0-2 (VIB_ORDER_BAND_FIRST..VIB_ORDER_BAND_LAST) from rpm.
 * Band N (0-indexed): center = (N+1) * order_freq, width = ±0.5 * order_freq
 *   i.e. lo = (N+0.5) * order_freq, hi = (N+1.5) * order_freq
 * where order_freq = rpm / 60.
 * If rpm <= 0, order bands are set to {0,0} (disabled).
 */
static void recalc_order_bands(float rpm)
{
    if (rpm <= 0.0f) {
        for (int n = VIB_ORDER_BAND_FIRST; n <= VIB_ORDER_BAND_LAST; n++) {
            s_bands[n].freq_lo = 0.0f;
            s_bands[n].freq_hi = 0.0f;
        }
        return;
    }

    float order_freq = rpm / 60.0f;
    for (int n = VIB_ORDER_BAND_FIRST; n <= VIB_ORDER_BAND_LAST; n++) {
        float center = (float)(n + 1) * order_freq;
        s_bands[n].freq_lo = center - 0.5f * order_freq;
        s_bands[n].freq_hi = center + 0.5f * order_freq;
        /* Clamp lo to 1 Hz to avoid DC bin */
        if (s_bands[n].freq_lo < 1.0f)
            s_bands[n].freq_lo = 1.0f;
    }
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */
int vib_init(const vib_band_t *bands)
{
    /* Compute Hanning window for actual sample count */
    if (!s_window_ready) {
        compute_hanning_window(VIB_SAMPLE_RATE);  /* 2000 samples */
    }

    /* Set default bands if none provided */
    if (bands) {
        memcpy(s_bands, bands, sizeof(s_bands));
    } else {
        vib_band_t defaults[VIB_NUM_BANDS] = VIB_DEFAULT_BANDS;
        memcpy(s_bands, defaults, sizeof(s_bands));
    }

    return 0;
}

void vib_set_bands(const vib_band_t *bands)
{
    if (bands)
        memcpy(s_bands, bands, sizeof(s_bands));
}

void vib_set_rpm(float rpm)
{
    s_rpm      = (rpm > 0.0f) ? rpm : 0.0f;
    s_auto_rpm = false;
    recalc_order_bands(s_rpm);
}

float vib_get_rpm(void)
{
    return s_rpm;
}

void vib_set_auto_rpm(bool enable)
{
    s_auto_rpm = enable;
}

int vib_compute(const int16_t *accel_x,
                const int16_t *accel_y,
                const int16_t *accel_z,
                uint16_t n_samples,
                float accel_scale,
                vib_features_t *out)
{
    if (!out || !accel_x || !accel_y || !accel_z || n_samples == 0)
        return -1;

    const int16_t *axes[VIB_NUM_AXES] = { accel_x, accel_y, accel_z };

    out->sample_count = n_samples;

    for (int a = 0; a < VIB_NUM_AXES; a++) {
        compute_time_features(axes[a], n_samples, accel_scale, &out->time[a]);
        compute_freq_features(axes[a], n_samples, accel_scale, &out->freq[a]);
    }

    /* Auto-RPM detection: use Z-axis dominant_freq as 1X shaft speed estimate.
     * Only accept frequencies in the 5-200 Hz range (300-12000 RPM).
     * Takes effect from the next window (order bands updated here for next call). */
    if (s_auto_rpm) {
        float dom_z = out->freq[2].dominant_freq;
        if (dom_z >= 5.0f && dom_z <= 200.0f) {
            s_rpm = dom_z * 60.0f;
            recalc_order_bands(s_rpm);
        }
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* CSV output                                                         */
/* ------------------------------------------------------------------ */
static const char *axis_name[] = { "X", "Y", "Z" };

void vib_print_csv_header(void)
{
    printf("timestamp,samples");
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf(",%s_RMS,%s_Peak,%s_PP,%s_CF",
               axis_name[a], axis_name[a], axis_name[a], axis_name[a]);
    }
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        for (int b = 0; b < VIB_NUM_BANDS; b++) {
            printf(",%s_Band%02d", axis_name[a], b + 1);
        }
    }
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf(",%s_DomFreq,%s_DomAmp", axis_name[a], axis_name[a]);
    }
    printf("\r\n");
}

void vib_print_csv(const vib_features_t *f)
{
    printf("%lu,%u", (unsigned long)f->timestamp, f->sample_count);
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf(",%.6f,%.6f,%.6f,%.3f",
               f->time[a].rms, f->time[a].peak,
               f->time[a].peak_to_peak, f->time[a].crest_factor);
    }
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        for (int b = 0; b < VIB_NUM_BANDS; b++) {
            printf(",%.8f", f->freq[a].band_power[b]);
        }
    }
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf(",%.1f,%.8f", f->freq[a].dominant_freq, f->freq[a].dominant_amp);
    }
    printf("\r\n");
}

void vib_print_summary(const vib_features_t *f)
{
    printf("=== Vibration Features (N=%u) ===\r\n", f->sample_count);

    printf("\n[Time Domain]\r\n");
    printf("%-5s %10s %10s %10s %10s\r\n",
           "Axis", "RMS", "Peak", "P-P", "CF");
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf("%-5s %10.4f %10.4f %10.4f %10.3f\r\n",
               axis_name[a],
               f->time[a].rms, f->time[a].peak,
               f->time[a].peak_to_peak, f->time[a].crest_factor);
    }

    printf("\n[Frequency Bands — Power]\r\n");
    printf("%-8s", "Band");
    for (int a = 0; a < VIB_NUM_AXES; a++)
        printf(" %12s", axis_name[a]);
    printf("   Hz Range\r\n");
    for (int b = 0; b < VIB_NUM_BANDS; b++) {
        if (s_bands[b].freq_lo >= s_bands[b].freq_hi)
            continue;
        printf("Band %02d", b + 1);
        for (int a = 0; a < VIB_NUM_AXES; a++)
            printf(" %12.8f", f->freq[a].band_power[b]);
        printf("   %.0f-%.0f\r\n", s_bands[b].freq_lo, s_bands[b].freq_hi);
    }

    printf("\n[Dominant Frequency]\r\n");
    for (int a = 0; a < VIB_NUM_AXES; a++) {
        printf("%-5s  %.1f Hz  (amp=%.6f)\r\n",
               axis_name[a], f->freq[a].dominant_freq, f->freq[a].dominant_amp);
    }
}
