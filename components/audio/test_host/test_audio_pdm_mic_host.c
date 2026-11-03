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
 * Host test for audio_pdm_mic: the shared chain gives exactly the samples of
 * the two copies it replaced (ipro_sensor_teach's mic source and
 * ipro_reef_diarize's capture pump, as of 2026-10-31, kept below verbatim but
 * for names), read in each app's pattern. A fake capture plays the same
 * deterministic PDM-rate signal to all three. Built by test_host/run.sh.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_asrc.h"
#include "audio_capture.h"
#include "audio_pdm_mic.h"

static int s_fail, s_pass;

#define CHECK(cond, ...) do { \
    if (cond) { s_pass++; } else { s_fail++; printf("FAIL %s:%d: ", __FILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); } \
} while (0)

/* ---------------------------------------------------------------- fake capture */

#define SIGNAL_N (15873u * 6u)          /* six seconds at the PDM path's rate */
static int16_t  s_signal[SIGNAL_N];
static uint32_t s_play;
static int      s_open;

static void make_signal(void)
{
    uint32_t rng = 7u;
    for (uint32_t i = 0; i < SIGNAL_N; i++) {
        rng = rng * 1664525u + 1013904223u;
        const double t = i / 15872.5;
        double v = 2500.0 * sin(2 * M_PI * 440.0 * t) + 900.0 * sin(2 * M_PI * 50.0 * t) + 300.0 +
                   (double)((int32_t)(rng >> 16) - 32768) * 0.05;
        if (i > SIGNAL_N / 2 && i < SIGNAL_N / 2 + 4000) {
            v *= 12.0;                                       /* a loud burst that clips */
        }
        s_signal[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : lrint(v));
    }
}

int audio_capture_init(const audio_capture_cfg_t *cfg)
{
    (void)cfg;
    s_open = 1;
    s_play = 0;
    return 0;
}
int audio_capture_add_consumer(uint32_t buf_size) { (void)buf_size; return 0; }
int audio_capture_start(void) { return 0; }
int audio_capture_stop(void) { return 0; }
void audio_capture_deinit(void) { s_open = 0; }

/* whole 320-sample frames, like the DMA's, until the signal ends */
size_t audio_capture_read(int consumer_id, void *buf, size_t size, uint32_t timeout_ms)
{
    (void)consumer_id;
    (void)timeout_ms;
    uint32_t n = (uint32_t)(size / 2u);
    if (n > 320u) {
        n = 320u;
    }
    if (s_play + n > SIGNAL_N) {
        return 0;
    }
    memcpy(buf, &s_signal[s_play], n * 2u);
    s_play += n;
    return n * 2u;
}

/* ---------------------------------------------------------------- the old copies */

#define PDM_IN_MHZ 15872500u
#define OUT_MHZ    16000000u
#define PI_F       3.14159265f

/* ipro_reef_teach/sensor/sensor_src.c mic_read() */
static audio_asrc_t o_asrc;
static struct { float b0, b1, b2, a1, a2, x1, x2, y1, y2; } o_hpf;
static int16_t  o_in[320];
static uint32_t o_in_pos, o_in_n;

static void sensor_hpf_reset(void)
{
    const float w0 = 2.0f * PI_F * 100.0f / 16000.0f, c = cosf(w0);
    const float alpha = sinf(w0) / (2.0f * 0.70710678f), a0 = 1.0f + alpha;

    o_hpf.b0 = (1.0f + c) / 2.0f / a0;
    o_hpf.b1 = -(1.0f + c) / a0;
    o_hpf.b2 = o_hpf.b0;
    o_hpf.a1 = -2.0f * c / a0;
    o_hpf.a2 = (1.0f - alpha) / a0;
    o_hpf.x1 = o_hpf.x2 = o_hpf.y1 = o_hpf.y2 = 0.0f;
}

static uint32_t sensor_mic_read(int16_t *dst, uint32_t max_frames)
{
    uint32_t out = 0;

    if (o_in_pos >= o_in_n) {
        const size_t got = audio_capture_read(0, o_in, sizeof(o_in), 10);
        o_in_pos = 0;
        o_in_n = (uint32_t)(got / sizeof(int16_t));
    }
    while (o_in_pos < o_in_n && out < max_frames) {
        uint32_t used = 0;
        const uint32_t n = audio_asrc_process(&o_asrc, &o_in[o_in_pos], o_in_n - o_in_pos,
                                              &dst[out], max_frames - out, &used);
        for (uint32_t i = out; i < out + n; i++) {
            const float x = (float)dst[i];
            const float y = o_hpf.b0 * x + o_hpf.b1 * o_hpf.x1 + o_hpf.b2 * o_hpf.x2 -
                            o_hpf.a1 * o_hpf.y1 - o_hpf.a2 * o_hpf.y2;
            o_hpf.x2 = o_hpf.x1;
            o_hpf.x1 = x;
            o_hpf.y2 = o_hpf.y1;
            o_hpf.y1 = y;
            const long v = lrintf(y * 10.0f);
            dst[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
        }
        out += n;
        o_in_pos += used;
        if (used == 0 && n == 0) {
            break;
        }
    }
    return out;
}

/* ipro_reef_teach/voice/voice.c hpf_reset(), ring_push(), capture_pump() */
#define SW_GAIN_Q8 (256 * 10)

static void voice_hpf_reset(void)
{
    const float w0    = 2.0f * 3.14159265f * 100.0f / (float)16000u;
    const float c     = cosf(w0);
    const float alpha = sinf(w0) / (2.0f * 0.70710678f);
    const float a0    = 1.0f + alpha;

    o_hpf.b0 = (1.0f + c) / 2.0f / a0;
    o_hpf.b1 = -(1.0f + c) / a0;
    o_hpf.b2 = o_hpf.b0;
    o_hpf.a1 = -2.0f * c / a0;
    o_hpf.a2 = (1.0f - alpha) / a0;
    o_hpf.x1 = o_hpf.x2 = o_hpf.y1 = o_hpf.y2 = 0.0f;
}

static int16_t *o_ring;
static uint32_t o_wpos;

static void voice_ring_push(int16_t *pcm, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        const float x = (float)pcm[i];
        const float y = o_hpf.b0 * x + o_hpf.b1 * o_hpf.x1 + o_hpf.b2 * o_hpf.x2
                      - o_hpf.a1 * o_hpf.y1 - o_hpf.a2 * o_hpf.y2;
        o_hpf.x2 = o_hpf.x1;
        o_hpf.x1 = x;
        o_hpf.y2 = o_hpf.y1;
        o_hpf.y1 = y;
        const int32_t v = (int32_t)lrintf(y * (float)SW_GAIN_Q8 / 256.0f);
        pcm[i] = (int16_t)(v > 32767 ? 32767 : v < -32768 ? -32768 : v);
    }
    memcpy(&o_ring[o_wpos], pcm, n * sizeof(int16_t));
    o_wpos += n;
}

static int voice_capture_pump(void)
{
    static int16_t in[320];
    static int16_t out[320 + 320 / 8u];

    const size_t got = audio_capture_read(0, in, sizeof(in), 20);
    uint32_t pos = 0, n_in = (uint32_t)(got / sizeof(int16_t));
    while (pos < n_in) {
        uint32_t used = 0;
        const uint32_t n = audio_asrc_process(&o_asrc, &in[pos], n_in - pos, out,
                                              sizeof(out) / sizeof(out[0]), &used);
        if (n) {
            voice_ring_push(out, n);
        }
        if (used == 0 && n == 0) {
            break;
        }
        pos += used;
    }
    return got != 0;
}

/* ---------------------------------------------------------------- tests */

#define OUT_MAX (16000u * 7u)

static uint32_t run_sensor_old(int16_t *dst, uint32_t chunk)
{
    audio_capture_init(NULL);
    audio_asrc_init(&o_asrc, PDM_IN_MHZ, OUT_MHZ);
    sensor_hpf_reset();
    o_in_pos = o_in_n = 0;
    uint32_t n = 0, got;
    while ((got = sensor_mic_read(&dst[n], chunk)) || o_in_pos < o_in_n || s_play + 320 <= SIGNAL_N) {
        n += got;
        if (!got && s_play + 320 > SIGNAL_N && o_in_pos >= o_in_n) {
            break;
        }
    }
    return n;
}

static uint32_t run_voice_old(int16_t *dst)
{
    audio_capture_init(NULL);
    audio_asrc_init(&o_asrc, PDM_IN_MHZ, OUT_MHZ);
    voice_hpf_reset();
    o_ring = dst;
    o_wpos = 0;
    while (voice_capture_pump()) {
    }
    return o_wpos;
}

static uint32_t run_new(int16_t *dst, uint32_t chunk, int voice_pattern)
{
    const audio_pdm_mic_cfg_t cfg = { 1, 2, 0, 0 };
    uint32_t n = 0;

    CHECK(audio_pdm_mic_open(&cfg) == 0, "open");
    CHECK(audio_pdm_mic_open(&cfg) == -1, "second open refused");
    for (;;) {
        uint32_t got;
        if (voice_pattern) {
            /* the voice task: one capture frame per pump, drained in 360-sample pieces */
            uint32_t frame = 0;
            do {
                got = audio_pdm_mic_read(&dst[n], 360, 20);
                n += got;
                frame += got;
            } while (got && audio_pdm_mic_pending());
            got = frame;
        } else {
            got = audio_pdm_mic_read(&dst[n], chunk, 10);
            n += got;
        }
        if (!got && !audio_pdm_mic_pending() && s_play + 320 > SIGNAL_N) {
            break;
        }
    }
    audio_pdm_mic_close();
    CHECK(!audio_pdm_mic_is_open() && audio_pdm_mic_read(dst, 4, 0) == 0, "closed");
    return n;
}

int main(void)
{
    static int16_t a[OUT_MAX], b[OUT_MAX];
    make_signal();

    const uint32_t chunks[] = { 1, 7, 64, 250, 320, 1000 };
    for (unsigned k = 0; k < sizeof(chunks) / sizeof(chunks[0]); k++) {
        memset(a, 0, sizeof(a));
        memset(b, 0, sizeof(b));
        const uint32_t na = run_sensor_old(a, chunks[k]);
        const uint32_t nb = run_new(b, chunks[k], 0);
        CHECK(na > 16000u * 5u && na == nb, "sensor pattern, %u-sample reads: %u vs %u samples",
              chunks[k], na, nb);
        CHECK(!memcmp(a, b, na * sizeof(int16_t)), "sensor pattern, %u-sample reads: samples differ",
              chunks[k]);
    }
    memset(a, 0, sizeof(a));
    memset(b, 0, sizeof(b));
    const uint32_t na = run_voice_old(a);
    const uint32_t nb = run_new(b, 0, 1);
    CHECK(na > 16000u * 5u && na == nb, "voice pattern: %u vs %u samples", na, nb);
    CHECK(!memcmp(a, b, na * sizeof(int16_t)), "voice pattern: samples differ");
    int clipped = 0;
    for (uint32_t i = 0; i < na; i++) {
        clipped += a[i] == 32767 || a[i] == -32768;
    }
    CHECK(clipped > 0, "the burst reaches the clip");

    CHECK(audio_pdm_mic_open(NULL) == -1, "NULL cfg");
    printf("audio_pdm_mic: %d passed, %d failed\n", s_pass, s_fail);
    return s_fail ? 1 : 0;
}
