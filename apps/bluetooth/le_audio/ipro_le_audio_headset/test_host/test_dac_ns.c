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
 * Host test for src/dac_ns.c.  Run: sh test_host/run.sh
 *
 * In-band noise is measured by passing the shaper's error (output minus the
 * ideal interpolated signal) through four cascaded 16-tap boxcars - a sinc^4
 * low-pass with nulls on every multiple of 32 kHz - and taking the rms of
 * what is left. The same measurement on plain 32 kHz rounding (what the DAC
 * gets without the shaper) is the reference.
 */
#include "dac_ns.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FS_IN    32000
#define N_IN     (FS_IN / 2)                 /* 0.5 s */
#define N_OUT    (N_IN * DAC_NS_OSR)

static int failures;

#define CHECK(cond, ...) do { \
    if (!(cond)) { printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); failures++; } \
} while (0)

/* rms of x after four 16-tap boxcars, skipping the filter's start-up */
static double inband_rms(const double *x, int n)
{
    double *a = malloc(sizeof(double) * n), *b = malloc(sizeof(double) * n);
    memcpy(a, x, sizeof(double) * n);
    for (int pass = 0; pass < 4; pass++) {
        double s = 0;
        for (int i = 0; i < n; i++) {
            s += a[i];
            if (i >= DAC_NS_OSR) s -= a[i - DAC_NS_OSR];
            b[i] = s / DAC_NS_OSR;
        }
        memcpy(a, b, sizeof(double) * n);
    }
    double acc = 0;
    int cnt = 0;
    for (int i = 8 * DAC_NS_OSR; i < n; i++) { acc += a[i] * a[i]; cnt++; }
    free(a); free(b);
    return sqrt(acc / cnt);
}

static void make_input(int16_t *in, double amp)
{
    for (int i = 0; i < N_IN; i++) {
        double t = (double)i / FS_IN;
        in[i] = (int16_t)lround(amp * (0.6 * sin(2 * M_PI * 997 * t) + 0.4 * sin(2 * M_PI * 3001 * t)));
    }
}

/* error of the shaper vs. the ideal interpolated signal, in codes */
static double shaper_inband(const int16_t *in, uint8_t order, int *min_code, int *max_code)
{
    dac_ns_t ns;
    uint16_t blk[DAC_NS_OSR];
    double *err = malloc(sizeof(double) * N_OUT);
    int32_t prev = 0;

    dac_ns_reset(&ns, order);
    *min_code = 1023; *max_code = 0;
    for (int i = 0; i < N_IN; i++) {
        dac_ns_run(&ns, &in[i], 1, blk);
        for (int j = 0; j < DAC_NS_OSR; j++) {
            double ideal = (prev + (in[i] - prev) * (j + 1) / (double)DAC_NS_OSR) / 16.0;
            err[i * DAC_NS_OSR + j] = (blk[j] - 512) - ideal;
            if (blk[j] < *min_code) *min_code = blk[j];
            if (blk[j] > *max_code) *max_code = blk[j];
        }
        prev = in[i];
    }
    double r = inband_rms(err, N_OUT);
    free(err);
    return r;
}

/* plain rounding at 32 kHz, held for 16 output samples (zero-order hold) */
static double flat32k_inband(const int16_t *in)
{
    double *err = malloc(sizeof(double) * N_OUT);
    for (int i = 0; i < N_IN; i++) {
        double ideal = in[i] / 16.0;
        double q = floor(ideal + 0.5);
        for (int j = 0; j < DAC_NS_OSR; j++) err[i * DAC_NS_OSR + j] = q - ideal;
    }
    double r = inband_rms(err, N_OUT);
    free(err);
    return r;
}

static void test_silence(void)
{
    dac_ns_t ns;
    uint16_t blk[DAC_NS_OSR];
    long sum = 0;
    int lo = 1023, hi = 0;

    const int16_t zero = 0;

    dac_ns_reset(&ns, 1);
    for (int i = 0; i < 4000; i++) {
        dac_ns_run(&ns, &zero, 1, blk);
        for (int j = 0; j < DAC_NS_OSR; j++) {
            sum += blk[j];
            if (blk[j] < lo) lo = blk[j];
            if (blk[j] > hi) hi = blk[j];
        }
    }
    double mean = (double)sum / (4000 * DAC_NS_OSR);
    CHECK(lo >= 510 && hi <= 514, "silence wanders to %d..%d", lo, hi);
    CHECK(fabs(mean - 512.0) < 0.05, "silence mean %.3f", mean);
}

static void test_full_scale_does_not_clip(void)
{
    static int16_t in[N_IN];
    for (int order = 1; order <= 2; order++) {
        int lo, hi;
        for (int i = 0; i < N_IN; i++) {
            in[i] = (i / 37) % 2 ? DAC_NS_TARGET_MAX : -DAC_NS_TARGET_MAX;   /* square wave */
        }
        shaper_inband(in, (uint8_t)order, &lo, &hi);
        CHECK(lo > 0 && hi < 1023, "order %d square wave hit the rail: %d..%d", order, lo, hi);
    }
}

static void test_noise_reduction(void)
{
    static int16_t in[N_IN];
    int lo, hi;

    make_input(in, 2000);                          /* about -12 dB re the DAC's range */
    double flat = flat32k_inband(in);
    double o1 = shaper_inband(in, 1, &lo, &hi);
    double o2 = shaper_inband(in, 2, &lo, &hi);
    double g1 = 20 * log10(flat / o1), g2 = 20 * log10(flat / o2);

    printf("in-band error rms: 32 kHz rounding %.4f codes, order 1 %.5f (%.1f dB better), "
           "order 2 %.6f (%.1f dB better)\n", flat, o1, g1, o2, g2);
    CHECK(g1 > 20.0, "order 1 only %.1f dB better", g1);
    CHECK(g2 > g1 + 6.0, "order 2 (%.1f dB) not clearly better than order 1 (%.1f dB)", g2, g1);
}

static void test_tracks_input(void)
{
    /* The mean of each 16-code block follows a slow ramp to within a code. */
    dac_ns_t ns;
    uint16_t blk[DAC_NS_OSR];
    double worst = 0;

    dac_ns_reset(&ns, 1);
    for (int i = 0; i < 2000; i++) {
        int16_t x = (int16_t)(-6000 + i * 6);
        dac_ns_run(&ns, &x, 1, blk);
        double m = 0;
        for (int j = 0; j < DAC_NS_OSR; j++) m += blk[j];
        m = m / DAC_NS_OSR - 512;
        double want = (x - 3) / 16.0;      /* block centre lags by half a step */
        if (i > 4 && fabs(m - want) > worst) worst = fabs(m - want);
    }
    CHECK(worst < 1.0, "block mean off by %.2f codes", worst);
}

static void test_block_equals_single(void)
{
    static int16_t in[512];
    static uint16_t a[512 * DAC_NS_OSR], b[512 * DAC_NS_OSR];
    dac_ns_t na, nb;

    make_input(in, 3000);
    for (int order = 1; order <= 2; order++) {
        dac_ns_reset(&na, (uint8_t)order);
        dac_ns_reset(&nb, (uint8_t)order);
        dac_ns_run(&na, in, 512, a);
        for (int i = 0; i < 512; i++) dac_ns_run(&nb, &in[i], 1, &b[i * DAC_NS_OSR]);
        CHECK(memcmp(a, b, sizeof(a)) == 0, "order %d: block and per-sample output differ", order);
    }
}

int main(void)
{
    test_block_equals_single();
    test_silence();
    test_full_scale_does_not_clip();
    test_tracks_input();
    test_noise_reduction();
    if (failures) {
        printf("%d check(s) failed\n", failures);
        return 1;
    }
    printf("dac_ns: all tests passed\n");
    return 0;
}
