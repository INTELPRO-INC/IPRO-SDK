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
 * Host tests for audio_asrc (integer streaming resampler + FIFO level controller).
 * Built by test_host/run.sh. Double-precision references are fine here; the
 * code under test is integer-only.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_asrc.h"

static int s_fail, s_pass;

#define CHECK(cond, ...) do { \
    if (cond) { s_pass++; } else { s_fail++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } \
} while (0)

#define MIC_MHZ   15875000u   /* EVK PDM real rate */
#define NET_MHZ   16000000u   /* LC3 / network rate */
#define DAC_MHZ   32136000u   /* EVK DAC real rate (div 62) */

static uint32_t s_rng = 12345u;
static uint32_t rnd(void)
{
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng >> 8;
}

/* Run @p n_in samples of @p src through @p s in blocks (0 = random 1..MAX), collect output. */
static uint32_t run(audio_asrc_t *s, const int16_t *src, uint32_t n_in, uint32_t block,
                    int16_t *dst, uint32_t dst_cap)
{
    static int16_t tmp[AUDIO_ASRC_MAX_IN * 3];
    uint32_t pos = 0, out = 0;

    while (pos < n_in) {
        uint32_t n = block ? block : 1u + rnd() % AUDIO_ASRC_MAX_IN;
        if (n > n_in - pos) {
            n = n_in - pos;
        }
        uint32_t used = 0;
        uint32_t got = audio_asrc_process(s, &src[pos], n, tmp, sizeof(tmp) / sizeof(tmp[0]), &used);
        if (used != n) {
            printf("  run: used %u of %u\n", used, n);
            return out;
        }
        for (uint32_t i = 0; i < got && out < dst_cap; i++) {
            dst[out++] = tmp[i];
        }
        pos += n;
    }
    return out;
}

static double tone_level(const int16_t *x, uint32_t n, double f, double fs)
{
    /* Goertzel magnitude, normalised so a full-scale sine reads its amplitude */
    double w = 2.0 * M_PI * f / fs, c = 2.0 * cos(w), s0, s1 = 0, s2 = 0;
    for (uint32_t i = 0; i < n; i++) {
        s0 = x[i] + c * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    double re = s1 - s2 * cos(w), im = s2 * sin(w);
    return 2.0 * sqrt(re * re + im * im) / n;
}

static void make_tone(int16_t *x, uint32_t n, double f, double fs, double amp)
{
    for (uint32_t i = 0; i < n; i++) {
        x[i] = (int16_t)lrint(amp * sin(2.0 * M_PI * f * i / fs));
    }
}

/* ---------------------------------------------------------------- tests */

static void test_init_rejects_bad_rates(void)
{
    audio_asrc_t s;
    CHECK(audio_asrc_init(&s, 0, NET_MHZ) != 0, "in rate 0 accepted");
    CHECK(audio_asrc_init(&s, NET_MHZ, 0) != 0, "out rate 0 accepted");
    CHECK(audio_asrc_init(&s, 48000000u, 8000000u) != 0, "6:1 decimation accepted");
    CHECK(audio_asrc_init(&s, 8000000u, 48000000u) != 0, "1:6 interpolation accepted");
    CHECK(audio_asrc_init(&s, MIC_MHZ, NET_MHZ) == 0, "mic -> net rejected");
    CHECK(audio_asrc_init(&s, NET_MHZ, DAC_MHZ) == 0, "net -> dac rejected");
}

static void test_dc_gain_unity(void)
{
    static int16_t in[20000], out[45000];
    audio_asrc_t s;
    for (uint32_t i = 0; i < 20000; i++) {
        in[i] = 10000;
    }
    audio_asrc_init(&s, NET_MHZ, DAC_MHZ);
    uint32_t n = run(&s, in, 20000, 160, out, 45000);
    int worst = 0;
    for (uint32_t i = 64; i < n; i++) {
        int d = abs(out[i] - 10000);
        if (d > worst) {
            worst = d;
        }
    }
    CHECK(worst <= 2, "DC 10000 comes out off by %d", worst);
}

static void test_block_size_does_not_change_output(void)
{
    static int16_t in[50000], a[60000], b[60000];
    audio_asrc_t s1, s2;
    make_tone(in, 50000, 997.0, 15875.0, 12000.0);
    audio_asrc_init(&s1, MIC_MHZ, NET_MHZ);
    audio_asrc_init(&s2, MIC_MHZ, NET_MHZ);
    uint32_t na = run(&s1, in, 50000, 317, a, 60000);
    uint32_t nb = run(&s2, in, 50000, 0, b, 60000);
    CHECK(na == nb, "output count %u (blocks of 317) vs %u (random blocks)", na, nb);
    uint32_t n = na < nb ? na : nb;
    CHECK(memcmp(a, b, n * sizeof(int16_t)) == 0, "outputs differ with the block size");
}

static void test_output_count_tracks_ratio(void)
{
    static int16_t in[AUDIO_ASRC_MAX_IN], out[AUDIO_ASRC_MAX_IN * 3];
    audio_asrc_t s;
    audio_asrc_init(&s, MIC_MHZ, NET_MHZ);
    uint64_t total_in = 0, total_out = 0;
    memset(in, 0, sizeof(in));
    while (total_in < 5000000u) {                       /* ~5 min of audio */
        uint32_t n = 1u + rnd() % AUDIO_ASRC_MAX_IN, used = 0;
        total_out += audio_asrc_process(&s, in, n, out, sizeof(out) / sizeof(out[0]), &used);
        total_in += used;
    }
    double expect = (double)total_in * 16000.0 / 15875.0;
    CHECK(fabs((double)total_out - expect) <= AUDIO_ASRC_TAPS, "out %llu vs %.1f expected",
          (unsigned long long)total_out, expect);
}

static void test_tone_gain_and_images(void)
{
    static int16_t in[32000], out[70000];
    const double fin = 16000.0, fout = 32136.0;
    const double freqs[] = { 1000.0, 4000.0, 6000.0 };
    for (unsigned k = 0; k < 3; k++) {
        audio_asrc_t s;
        audio_asrc_init(&s, NET_MHZ, DAC_MHZ);
        make_tone(in, 32000, freqs[k], fin, 16000.0);
        uint32_t n = run(&s, in, 32000, 160, out, 70000);
        double lvl = tone_level(out + 512, n - 1024, freqs[k], fout);
        double db = 20.0 * log10(lvl / 16000.0);
        double limit = (k < 2) ? -0.1 : -1.0;
        CHECK(db > limit && db < 0.1, "%.0f Hz gain %.2f dB", freqs[k], db);
    }
    /* 7 kHz in: its image lands at 9 kHz in the 32 kHz output */
    audio_asrc_t s;
    audio_asrc_init(&s, NET_MHZ, DAC_MHZ);
    make_tone(in, 32000, 7000.0, fin, 16000.0);
    uint32_t n = run(&s, in, 32000, 160, out, 70000);
    double tone = tone_level(out + 512, n - 1024, 7000.0, fout);
    double image = tone_level(out + 512, n - 1024, 9000.0, fout);
    double rej = 20.0 * log10(image / tone);
    CHECK(rej < -25.0, "9 kHz image of a 7 kHz tone only %.1f dB down", rej);
}

static void test_mic_tone_keeps_pitch(void)
{
    /* 1 kHz recorded at the real mic rate must read 1 kHz at the network rate */
    static int16_t in[31750], out[33000];
    audio_asrc_t s;
    audio_asrc_init(&s, MIC_MHZ, NET_MHZ);
    make_tone(in, 31750, 1000.0, 15875.0, 16000.0);
    uint32_t n = run(&s, in, 31750, 317, out, 33000);
    double on = tone_level(out + 256, n - 512, 1000.0, 16000.0);
    double off = tone_level(out + 256, n - 512, 1008.0, 16000.0);   /* 1000 * 16000/15875 */
    CHECK(on > 15500.0, "1 kHz after mic->net only %.0f", on);
    CHECK(on > 4.0 * off, "pitch shifted: 1000 Hz %.0f vs 1008 Hz %.0f", on, off);
}

static void test_trim_changes_consumption(void)
{
    static int16_t in[AUDIO_ASRC_MAX_IN], out[AUDIO_ASRC_MAX_IN * 3];
    audio_asrc_t a, b;
    memset(in, 0, sizeof(in));
    audio_asrc_init(&a, NET_MHZ, NET_MHZ);
    audio_asrc_init(&b, NET_MHZ, NET_MHZ);
    audio_asrc_trim_ppm_x100(&b, 100000);                 /* +1000 ppm: consume 0.1 % more input */
    uint64_t out_a = 0, out_b = 0;
    for (int i = 0; i < 5000; i++) {
        uint32_t used;
        out_a += audio_asrc_process(&a, in, 400, out, sizeof(out) / sizeof(out[0]), &used);
        out_b += audio_asrc_process(&b, in, 400, out, sizeof(out) / sizeof(out[0]), &used);
    }
    double ratio = (double)out_a / (double)out_b;
    CHECK(fabs(ratio - 1.001) < 0.00005, "trim +1000 ppm gave out ratio %.6f", ratio);
    audio_asrc_trim_ppm_x100(&b, 0);
    CHECK(b.step == b.step_nom, "trim 0 did not restore the nominal step");
}

static void test_small_output_buffer_is_safe(void)
{
    /* An output buffer too small for the input: the converter keeps what it
     * took, never overruns its history, reports how much it took, and the
     * outputs it owes come out on later calls. */
    static int16_t in[AUDIO_ASRC_MAX_IN];
    int16_t out[8];
    audio_asrc_t s;
    make_tone(in, AUDIO_ASRC_MAX_IN, 500.0, 16000.0, 1000.0);
    audio_asrc_init(&s, NET_MHZ, DAC_MHZ);
    uint64_t used_total = 0, out_total = 0;
    for (int i = 0; i < 400; i++) {
        uint32_t used = 0;
        uint32_t got = audio_asrc_process(&s, in, AUDIO_ASRC_MAX_IN + 1, out, 8, &used);
        CHECK(got <= 8 && used <= AUDIO_ASRC_MAX_IN, "call %d: got %u used %u", i, got, used);
        CHECK(s.count <= AUDIO_ASRC_TAPS + AUDIO_ASRC_MAX_IN, "history overflow: %u", s.count);
        used_total += used;
        out_total += got;
    }
    /* drain with no new input */
    for (int i = 0; i < 1000; i++) {
        uint32_t used = 0;
        out_total += audio_asrc_process(&s, in, 0, out, 8, &used);
    }
    double expect = (double)used_total * 32136.0 / 16000.0;
    CHECK(fabs((double)out_total - expect) <= 2.0 * AUDIO_ASRC_TAPS, "owed outputs lost: %llu vs %.1f",
          (unsigned long long)out_total, expect);
}

static void test_ctl_holds_fifo_level(void)
{
    /*
     * Listener side for one hour: 640 network samples arrive every 40 ms from
     * a talker whose crystal is +50 ppm against ours; our DAC takes 643 samples
     * (20 ms at 32136 Hz) every 20 ms. The resampler reads the FIFO, the
     * controller trims it from the FIFO level once per 40 ms.
     */
    static int16_t fifo[8192], out[AUDIO_ASRC_MAX_IN * 3];
    /* total queued = DAC ring (~643) + one 640-sample burst + margin */
    const int32_t target = 2000;
    audio_asrc_t s;
    audio_asrc_ctl_t c;
    uint32_t level = 1280;
    double talker_acc = 0.0;
    int32_t lo = 1 << 30, hi = -(1 << 30), ppm = 0;
    int64_t ppm_sum = 0;
    int32_t ppm_n = 0, ppm_lo = 1 << 30, ppm_hi = -(1 << 30);

    audio_asrc_init(&s, NET_MHZ, DAC_MHZ);
    audio_asrc_ctl_init(&c, target, 500);
    memset(fifo, 0, sizeof(fifo));

    const int32_t dac_fill = 1286;                              /* 40 ms queued in the DAC ring */
    int32_t backlog = dac_fill;
    double dac_acc = 0.0;

    for (uint32_t tick = 0; tick < 3600u * 50u; tick++) {       /* 20 ms ticks */
        if ((tick & 1u) == 0) {
            talker_acc += 640.0 * (1.0 + 50e-6);
            uint32_t add = (uint32_t)talker_acc;
            talker_acc -= add;
            level += add;
            /* everything queued, in network samples: FIFO + converter + DAC ring */
            int32_t total = (int32_t)level + (int32_t)audio_asrc_pending(&s) + backlog / 2;
            ppm = audio_asrc_ctl_update(&c, total);
            audio_asrc_trim_ppm_x100(&s, ppm);
            if (tick > 50u * 600u) {                             /* last 50 minutes */
                ppm_sum += ppm;
                ppm_n++;
                if (ppm < ppm_lo) ppm_lo = ppm;
                if (ppm > ppm_hi) ppm_hi = ppm;
            }
        }
        /* the DAC plays exactly 32136 samples a second ... */
        dac_acc += 32136.0 * 0.020;
        int32_t play = (int32_t)dac_acc;
        dac_acc -= play;
        backlog -= play;
        /* ... and the resampler tops its ring back up from the FIFO */
        while (backlog < dac_fill && level > 0) {
            uint32_t used = 0;
            uint32_t n = (level < 64u) ? level : 64u;
            backlog += (int32_t)audio_asrc_process(&s, fifo, n, out, sizeof(out) / sizeof(out[0]), &used);
            level -= used;
        }
        if (tick > 50u * 60u) {                                  /* after the first minute */
            if ((int32_t)level < lo) lo = (int32_t)level;
            if ((int32_t)level > hi) hi = (int32_t)level;
        }
        if (level > 7000u || backlog < 0) {
            CHECK(0, "FIFO ran away: level %u backlog %d at tick %u", level, backlog, tick);
            return;
        }
    }
    CHECK(lo > 100 && hi < 2000, "FIFO level wandered to [%d, %d] (never empty, never runaway)", lo, hi);
    int32_t avg = (int32_t)(ppm_sum / (ppm_n ? ppm_n : 1));
    CHECK(avg > 4000 && avg < 6000, "average trim %d (ppm x100), expected about +50 ppm", avg);
    CHECK(ppm_lo > -5000 && ppm_hi < 15000, "trim swings [%d, %d] (ppm x100)", ppm_lo, ppm_hi);
}

static void test_ctl_rides_out_frame_beat(void)
{
    /*
     * Talker side, as on the board: the mic hands over 320 samples every
     * 20.16 ms (15873 Hz real), the radio takes 640 every 40 ms. The level
     * seen at the pull beats by a whole mic frame every ~5 s; the trim must
     * not follow that beat (bench 2026-09-26: 8 ppm/sample swung it by
     * +/-1600 ppm).
     */
    static int16_t frame[320], conv[1000];
    audio_asrc_t s;
    audio_asrc_ctl_t c;
    uint32_t fifo = 700;
    double mic_t = 0.0;
    int32_t ppm = 0, lo = 1 << 30, hi = -(1 << 30);

    memset(frame, 0, sizeof(frame));
    audio_asrc_init(&s, 15873000u, NET_MHZ);          /* nominal = the true mic rate */
    audio_asrc_ctl_init(&c, 700, 3000);
    for (uint32_t ms = 0; ms < 20u * 60u * 1000u; ms++) {   /* 20 min in 1 ms steps */
        while (mic_t <= (double)ms) {
            mic_t += 320.0 * 1000.0 / 15873.0;
            uint32_t used = 0;
            fifo += audio_asrc_process(&s, frame, 320, conv, 1000, &used);
        }
        if (ms % 40u == 39u) {
            fifo = (fifo >= 640u) ? fifo - 640u : 0u;
            ppm = audio_asrc_ctl_update(&c, (int32_t)(fifo + audio_asrc_pending(&s)));
            audio_asrc_trim_ppm_x100(&s, ppm);
            if (ms > 5u * 60u * 1000u) {
                if (ppm < lo) lo = ppm;
                if (ppm > hi) hi = ppm;
            }
        }
    }
    CHECK(lo > -5000 && hi < 5000, "trim followed the frame beat: [%d, %d] (ppm x100)", lo, hi);
}

static void test_ctl_clamps_and_sign(void)
{
    audio_asrc_ctl_t c;
    audio_asrc_ctl_init(&c, 1000, 200);
    int32_t p = 0;
    for (int i = 0; i < 2000; i++) p = audio_asrc_ctl_update(&c, 100000);
    CHECK(p == 20000, "full FIFO should saturate at +200 ppm, got %d", p);
    for (int i = 0; i < 4000; i++) p = audio_asrc_ctl_update(&c, -100000);
    CHECK(p == -20000, "empty FIFO should saturate at -200 ppm, got %d", p);
    audio_asrc_ctl_init(&c, 1000, 200);
    CHECK(audio_asrc_ctl_update(&c, 1000) == 0, "on target should give no trim");
}

int main(void)
{
    test_init_rejects_bad_rates();
    test_dc_gain_unity();
    test_block_size_does_not_change_output();
    test_output_count_tracks_ratio();
    test_tone_gain_and_images();
    test_mic_tone_keeps_pitch();
    test_trim_changes_consumption();
    test_small_output_buffer_is_safe();
    test_ctl_holds_fifo_level();
    test_ctl_rides_out_frame_beat();
    test_ctl_clamps_and_sign();
    printf("audio_asrc: %d passed, %d failed\n", s_pass, s_fail);
    return s_fail ? 1 : 0;
}
