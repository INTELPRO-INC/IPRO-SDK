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
 * @file dac_test.c
 * @brief PDM -> DAC test commands for ipro_pdm_mic_demo
 *
 * Exists to answer one question: how much gain does the mic-to-speaker path
 * actually need? A PDM MEMS mic delivers roughly -52 dBFS at conversational
 * level, and pcm_to_dac() in components/audio maps int16 full scale onto the
 * 10-bit DAC full scale — unity. With pdm_gain left at its 0 dB default the
 * whole chain runs at unity, so playback lands ~40 dB below where it needs
 * to be. These commands make each stage measurable instead of guessable.
 *
 * Shell commands:
 *   dac_tone [freq] [sec] [amp%]  - synthetic sine straight to the DAC
 *   pdm_play [gain_dB]            - replay the pdm_rec buffer via the DAC
 *   pdm_loop [sec] [gain_dB]      - live PDM -> DAC loopback
 *   dac_gain [dB]                 - show/set the software makeup gain
 *   dac_dc <int16>                - hold a constant code for DMM work
 *   dac_spk <gpio> <unmute_lvl>   - retarget the amplifier enable pin
 *   dac_stop                      - stop tone/playback/loopback
 *
 * Gain staging, in the order it should be tuned:
 *   1. pdm_gain <dB> — AUADC decimator gain, applied before the 16-bit
 *      truncation, max +18 dB. The only stage that improves resolution
 *      rather than scaling up what is already quantised.
 *   2. dac_gain <dB> — the software makeup gain applied here. Pure scaling:
 *      it lifts the noise floor along with the signal.
 *   3. Amplifier gain resistors. Whatever is still missing belongs there,
 *      not in more digital gain.
 *
 * Clipping is counted and reported rather than compressed away — the point
 * of a test command is to find the level at which the chain starts to clip.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <queue.h>
#include <hal_auadc.h>
#include <hal_gpio.h>

#include "shell.h"
#include "ipro_log.h"
#include "audio_output.h"

#define TAG "DAC_TEST"

/* --- Board wiring -------------------------------------------------------
 * Two known variants, switchable at runtime with `dac_spk` so one image
 * covers both. The product board is the default; for the EVB run
 * `dac_spk 22 0`.
 *   Product bd   GPIO3  -> LM48580 /SD,     active low   => unmute level 1
 *   EVB          GPIO22 -> LM4871 SHUTDOWN, active high  => unmute level 0
 *
 * Note the product-board net is called AMP_POWER_DOWN but lands on /SD, so
 * driving it HIGH ENABLES the amplifier — the name reads backwards. The pin
 * is parked in its muted state at init so the amp is never left floating.
 *
 * audio_output_cfg_t carries no polarity field and audio_output_dac.c
 * hardcodes the EVB sense (writes 0 to unmute, 1 to mute), so for an
 * active-high enable this file re-drives the pin after start and stop. */
/* Board-dependent, so they come from .config and can still be overridden at
 * runtime with `dac_spk <gpio> <unmute_level>`.
 *
 *   EVB          GPIO22 -> LM4871 SHUTDOWN, active high  => unmute level 0
 *   IPRO7AI EVK  GPIO22 -> TPA2037D1 enable, active high => unmute level 1
 *
 * The polarity really is opposite between the two boards; do not copy one to
 * the other. Getting it wrong is silent: every statistic reads healthy and
 * nothing comes out of the speaker. */
#ifndef CONFIG_IPRO_PDM_MIC_SPK_GPIO
#define CONFIG_IPRO_PDM_MIC_SPK_GPIO        3
#endif
#ifndef CONFIG_IPRO_PDM_MIC_SPK_UNMUTE
#define CONFIG_IPRO_PDM_MIC_SPK_UNMUTE      1
#endif
#define DAC_SPK_GPIO_DEFAULT        CONFIG_IPRO_PDM_MIC_SPK_GPIO
#define DAC_SPK_UNMUTE_LEVEL_DEF    CONFIG_IPRO_PDM_MIC_SPK_UNMUTE

static int s_spk_gpio   = DAC_SPK_GPIO_DEFAULT;
static int s_spk_unmute = DAC_SPK_UNMUTE_LEVEL_DEF;

/* --- Audio parameters --- */
#define PCM_RATE            16000   /* PDM capture rate, matches main.c */
#define FRAME_SAMPLES       320     /* 20 ms frame, matches main.c */
#define FRAME_BYTES         (FRAME_SAMPLES * (int)sizeof(int16_t))
#define TONE_RATE           32000   /* DAC native rate — bypasses resampling */
#define TONE_CHUNK          320
#define TONE_LUT_SIZE       256
#define TONE_AMP_DEFAULT    30      /* percent of full scale — see cmd_dac_tone */

/* Ring sizing for the DAC sink. 4 x 20 ms keeps loopback latency at 80 ms;
 * one 16 kHz frame resamples to exactly one 32 kHz segment. */
#define DAC_RING_COUNT      4
#define DAC_RING_MS         20

/* --- Software makeup gain --- */
#define GAIN_DB_MIN         (-20)
#define GAIN_DB_MAX         48
/* Bench-tuned on the EVB against PDM_DIGITAL_GAIN 18: normal speech peaks
 * at -19.3 dBFS with an 18.7 dB crest factor, so +16 dB puts the output
 * peak at -3.3 dBFS (rms -22 dBFS) with zero clipped samples. Above ~19 dB
 * the peaks start hitting full scale. */
#define GAIN_DB_DEFAULT     16

/* Loopback queue depth (frames of FRAME_SAMPLES) */
#define LOOP_QUEUE_LEN      8

enum {
    DAC_IDLE = 0,
    DAC_TONE,
    DAC_PLAY,
    DAC_LOOP,
    DAC_DC,
};

static volatile int   s_mode;
static volatile bool  s_stop_req;
static TaskHandle_t   s_task;
static audio_output_t *s_out;

static int      s_gain_db = GAIN_DB_DEFAULT;
static int32_t  s_gain_q8 = 256;        /* Q8 linear multiplier */

static int16_t  s_lut[TONE_LUT_SIZE];
static int      s_lut_ready;

/* Loopback plumbing: ISR fills the ring and queues a pointer, task drains */
static QueueHandle_t s_loop_queue;
static int16_t      *s_loop_ring;
static volatile uint32_t s_loop_widx;
static volatile uint32_t s_loop_drops;
static volatile bool s_loop_active;

/* dac_dc: live-updatable so a meter sweep needs no stop/start between points */
static volatile int16_t s_dc_value;

/* From main.c */
extern bool pdm_is_recording(void);
extern bool pdm_is_streaming(void);
extern const int16_t *pdm_get_buffer(void);
extern uint32_t pdm_get_recorded_bytes(void);
extern int  pdm_get_gain(void);
extern int  pdm_get_rec_gain(void);

/* ===================================================================
 * Level metering
 * =================================================================== */

typedef struct {
    int32_t  peak;
    uint64_t energy;
    uint32_t samples;
} level_t;

static void level_add(level_t *l, const int16_t *pcm, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        int32_t v = pcm[i];
        int32_t mag = (v < 0) ? -v : v;
        if (mag > l->peak) {
            l->peak = mag;
        }
        l->energy += (uint64_t)((int64_t)v * v);
        l->samples++;
    }
}

static float level_dbfs(float v)
{
    return (v >= 1.0f) ? 20.0f * log10f(v / 32768.0f) : -99.0f;
}

static float level_rms(const level_t *l)
{
    if (l->samples == 0) {
        return 0.0f;
    }
    return sqrtf((float)l->energy / (float)l->samples);
}

static void level_print(const char *what, const level_t *l)
{
    float rms = level_rms(l);
    printf("  %-6s peak %5ld (%6.1f dBFS)   rms %7.1f (%6.1f dBFS)\r\n",
           what, (long)l->peak, (double)level_dbfs((float)l->peak),
           (double)rms, (double)level_dbfs(rms));
}

/* ===================================================================
 * Software makeup gain
 * =================================================================== */

static void gain_recalc(void)
{
    s_gain_q8 = (int32_t)(powf(10.0f, (float)s_gain_db / 20.0f) * 256.0f + 0.5f);
}

/* Apply gain with hard clamping; returns the number of clipped samples.
 * Deliberately not soft-knee compressed — this is a measurement tool and
 * the clip count is the number worth reading. */
static uint32_t gain_apply(const int16_t *src, int16_t *dst, uint32_t n)
{
    uint32_t clipped = 0;

    for (uint32_t i = 0; i < n; i++) {
        int32_t s = ((int32_t)src[i] * s_gain_q8) >> 8;
        if (s > 32767) {
            s = 32767;
            clipped++;
        } else if (s < -32768) {
            s = -32768;
            clipped++;
        }
        dst[i] = (int16_t)s;
    }
    return clipped;
}

/* ===================================================================
 * DAC sink open/close
 * =================================================================== */

static void dac_set_mute(bool mute)
{
    int level = mute ? !s_spk_unmute : s_spk_unmute;

    gpio_set_mode(s_spk_gpio, GPIO_OUTPUT_PP_MODE);
    gpio_write(s_spk_gpio, level);
}

static audio_output_t *dac_open(uint32_t rate)
{
    audio_output_cfg_t cfg = {
        .type           = AUDIO_OUTPUT_DAC,
        .mode           = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate    = rate,
        .channels       = 1,
        .ring_buf_count = DAC_RING_COUNT,
        .ring_buf_ms    = DAC_RING_MS,
        .dac_spk_gpio   = (int8_t)s_spk_gpio,
    };

    audio_output_t *out = audio_output_create(&cfg);
    if (out == NULL) {
        printf("dac: create failed (already in use? run dac_stop)\r\n");
        return NULL;
    }
    if (audio_output_start(out) != 0) {
        printf("dac: start failed\r\n");
        audio_output_destroy(out);
        return NULL;
    }

    /* audio_output_dac.c unmutes by driving the pin low. Re-drive it for
     * boards whose amplifier enable is active high. */
    if (s_spk_unmute != 0) {
        dac_set_mute(false);
    }
    return out;
}

static void dac_close(audio_output_t *out)
{
    if (out == NULL) {
        return;
    }
    audio_output_stop(out);
    audio_output_destroy(out);

    /* audio_output_dac.c mutes by driving the pin high — wrong way round
     * for an active-high enable, so force the muted level here. */
    if (s_spk_unmute != 0) {
        dac_set_mute(true);
    }
}

static void dac_task_finish(void)
{
    dac_close(s_out);
    s_out = NULL;
    s_mode = DAC_IDLE;
    s_stop_req = false;
    s_task = NULL;
    vTaskDelete(NULL);
}

/* ===================================================================
 * dac_tone — synthetic sine, bypasses the mic entirely
 * =================================================================== */

struct tone_arg {
    uint32_t freq_hz;
    uint32_t seconds;
    uint32_t amp_pct;
};

static void tone_lut_build(void)
{
    if (s_lut_ready) {
        return;
    }
    for (int i = 0; i < TONE_LUT_SIZE; i++) {
        s_lut[i] = (int16_t)lrintf(32767.0f *
                       sinf(2.0f * (float)M_PI * (float)i / TONE_LUT_SIZE));
    }
    s_lut_ready = 1;
}

static void tone_task(void *param)
{
    struct tone_arg arg = *(struct tone_arg *)param;
    vPortFree(param);

    int16_t  chunk[TONE_CHUNK];
    uint32_t total = TONE_RATE * arg.seconds;
    uint32_t done  = 0;
    uint32_t phase = 0;
    uint32_t step  = (uint32_t)(((uint64_t)arg.freq_hz << 16) *
                                TONE_LUT_SIZE / TONE_RATE);
    level_t  lvl = {0};

    s_out = dac_open(TONE_RATE);
    if (s_out == NULL) {
        dac_task_finish();
        return;
    }

    printf("dac_tone: %lu Hz  %lu s  amp %lu%%  -> GPIO23\r\n",
           (unsigned long)arg.freq_hz, (unsigned long)arg.seconds,
           (unsigned long)arg.amp_pct);
    printf("  expect about %.2f Vpp on GPIO23 (full scale is 1.6 Vpp)\r\n",
           1.6 * (double)arg.amp_pct / 100.0);
    /* The line that settles "no sound": the amplifier enable this image is
     * driving, and to which level. Wrong board image = wrong level here. */
    printf("  amplifier enable: GPIO%d driven %s while playing (dac_spk to change)\r\n",
           s_spk_gpio, s_spk_unmute ? "HIGH" : "LOW");
    if (arg.amp_pct > 50) {
        printf("  WARNING: sustained high-amplitude tone — unplug the "
               "speaker, it can brown out the supply and cook the coil\r\n");
    }

    while (done < total && !s_stop_req) {
        uint32_t n = total - done;
        if (n > TONE_CHUNK) {
            n = TONE_CHUNK;
        }
        for (uint32_t i = 0; i < n; i++) {
            int32_t s = s_lut[(phase >> 16) & (TONE_LUT_SIZE - 1)];
            chunk[i] = (int16_t)(s * (int32_t)arg.amp_pct / 100);
            phase += step;
        }
        level_add(&lvl, chunk, n);
        if (audio_output_write(s_out, chunk, n) != 0) {
            printf("dac_tone: write failed at %lu\r\n", (unsigned long)done);
            break;
        }
        done += n;
    }

    vTaskDelay(pdMS_TO_TICKS(100));     /* let the ring drain */

    printf("dac_tone: %s (%lu/%lu samples)\r\n",
           s_stop_req ? "stopped" : "done",
           (unsigned long)done, (unsigned long)total);
    level_print("tone", &lvl);

    dac_task_finish();
}

/* ===================================================================
 * pdm_play — replay the pdm_rec buffer through the DAC
 * =================================================================== */

static void play_task(void *param)
{
    uint32_t total = (uint32_t)(uintptr_t)param;
    const int16_t *src = pdm_get_buffer();
    uint32_t offset = 0;
    uint32_t clipped = 0;
    int16_t  buf[FRAME_SAMPLES];
    level_t  in_lvl = {0};
    level_t  out_lvl = {0};

    s_out = dac_open(PCM_RATE);
    if (s_out == NULL) {
        dac_task_finish();
        return;
    }

    printf("pdm_play: %lu samples (%.2f s)  pdm_gain %d dB  dac_gain %d dB\r\n",
           (unsigned long)total, (double)total / PCM_RATE,
           pdm_get_rec_gain(), s_gain_db);

    while (offset < total && !s_stop_req) {
        uint32_t n = total - offset;
        if (n > FRAME_SAMPLES) {
            n = FRAME_SAMPLES;
        }
        level_add(&in_lvl, &src[offset], n);
        clipped += gain_apply(&src[offset], buf, n);
        level_add(&out_lvl, buf, n);

        if (audio_output_write(s_out, buf, n) != 0) {
            printf("pdm_play: write failed at %lu\r\n", (unsigned long)offset);
            break;
        }
        offset += n;
    }

    vTaskDelay(pdMS_TO_TICKS(100));     /* let the ring drain */

    printf("pdm_play: %s (%lu/%lu samples)\r\n",
           s_stop_req ? "stopped" : "done",
           (unsigned long)offset, (unsigned long)total);
    level_print("in", &in_lvl);
    level_print("out", &out_lvl);
    if (clipped) {
        printf("  clipped %lu samples (%.2f%%) — lower dac_gain\r\n",
               (unsigned long)clipped,
               100.0 * (double)clipped / (double)(offset ? offset : 1));
    }

    dac_task_finish();
}

/* ===================================================================
 * pdm_loop — live PDM -> DAC loopback
 * =================================================================== */

/* Called from main.c's PDM DMA ISR for every captured frame. */
void dac_test_feed_frame_isr(const int16_t *pcm, uint32_t bytes,
                             BaseType_t *woken)
{
    if (!s_loop_active || s_loop_queue == NULL || s_loop_ring == NULL) {
        return;
    }

    uint32_t idx = s_loop_widx % LOOP_QUEUE_LEN;
    int16_t *dst = s_loop_ring + (idx * FRAME_SAMPLES);

    memcpy(dst, pcm, bytes);

    /* Only advance on a successful queue send. Advancing unconditionally
     * would march the write index over ring slots the task has not drained
     * yet, corrupting queued frames on every drop. */
    if (xQueueSendFromISR(s_loop_queue, &dst, woken) == pdTRUE) {
        s_loop_widx++;
    } else {
        s_loop_drops++;
    }
}

static void loop_task(void *param)
{
    uint32_t seconds = (uint32_t)(uintptr_t)param;
    uint32_t frames_target = seconds * (PCM_RATE / FRAME_SAMPLES);
    uint32_t frames = 0;
    uint32_t clipped = 0;
    int16_t  buf[FRAME_SAMPLES];
    level_t  in_lvl = {0};
    level_t  out_lvl = {0};

    s_out = dac_open(PCM_RATE);
    if (s_out == NULL) {
        dac_task_finish();
        return;
    }

    /* Start capture directly — pdm_start_recording/streaming would also
     * arm the recorder and the BLE stream, which this test does not want.
     * hal_auadc was configured once at boot; only gain and start/stop
     * are touched here. */
    s_loop_widx = 0;
    s_loop_drops = 0;
    xQueueReset(s_loop_queue);
    hal_auadc_set_digital_gain(pdm_get_gain());
    s_loop_active = true;
    hal_auadc_start();

    printf("pdm_loop: %lu s  pdm_gain %d dB  dac_gain %d dB  latency ~%d ms\r\n",
           (unsigned long)seconds, pdm_get_gain(), s_gain_db,
           DAC_RING_COUNT * DAC_RING_MS);
    printf("  mic and speaker in one enclosure will howl — keep them apart\r\n");

    while (frames < frames_target && !s_stop_req) {
        int16_t *src = NULL;
        if (xQueueReceive(s_loop_queue, &src, pdMS_TO_TICKS(200)) != pdTRUE) {
            continue;
        }
        level_add(&in_lvl, src, FRAME_SAMPLES);
        clipped += gain_apply(src, buf, FRAME_SAMPLES);
        level_add(&out_lvl, buf, FRAME_SAMPLES);

        if (audio_output_write(s_out, buf, FRAME_SAMPLES) != 0) {
            printf("pdm_loop: write failed at frame %lu\r\n",
                   (unsigned long)frames);
            break;
        }
        frames++;
    }

    s_loop_active = false;
    hal_auadc_stop();

    printf("pdm_loop: %s (%lu/%lu frames)\r\n",
           s_stop_req ? "stopped" : "done",
           (unsigned long)frames, (unsigned long)frames_target);
    level_print("in", &in_lvl);
    level_print("out", &out_lvl);
    if (clipped) {
        printf("  clipped %lu samples (%.2f%%) — lower dac_gain\r\n",
               (unsigned long)clipped,
               100.0 * (double)clipped /
               (double)(frames ? frames * FRAME_SAMPLES : 1));
    }
    if (s_loop_drops) {
        printf("  dropped %lu frames (DAC could not keep up)\r\n",
               (unsigned long)s_loop_drops);
    }

    dac_task_finish();
}

/* ===================================================================
 * dac_dc — hold a constant DAC code so a multimeter can characterise
 * the analogue output range without a scope
 * =================================================================== */

static void dc_task(void *param)
{
    (void)param;

    int16_t buf[FRAME_SAMPLES];
    int32_t applied = INT32_MIN;    /* force the first fill */

    s_out = dac_open(PCM_RATE);
    if (s_out == NULL) {
        dac_task_finish();
        return;
    }

    /* Hold the amplifier shut down for the whole sweep. The measurement
     * point is GPIO23, ahead of the input coupling cap, so the amp is pure
     * risk here: stepping between codes is a DC step of up to full scale,
     * which the amp turns into a rail-to-rail transient big enough to brown
     * the supply out (measured: a 1023 -> 0 step resets the board). */
    dac_set_mute(true);

    printf("  amp held muted; probe GPIO23 itself — the far side of the\r\n"
           "  input coupling cap sits at the amp's own bias and never moves\r\n");

    while (!s_stop_req) {
        int16_t want = s_dc_value;

        if (applied != want) {
            applied = want;
            for (uint32_t i = 0; i < FRAME_SAMPLES; i++) {
                buf[i] = want;
            }
            /* pcm_to_dac() in components/audio maps int16 -> (s >> 6) + 512 */
            printf("dac_dc: holding %d  -> 10-bit DAC code %d of 1023\r\n",
                   want, (want >> 6) + 512);
        }

        if (audio_output_write(s_out, buf, FRAME_SAMPLES) != 0) {
            printf("dac_dc: write failed\r\n");
            break;
        }
    }

    printf("dac_dc: stopped\r\n");
    dac_task_finish();
}

/* ===================================================================
 * Shell commands
 * =================================================================== */

static int dac_busy(void)
{
    if (s_mode != DAC_IDLE) {
        printf("dac: busy (run dac_stop first)\r\n");
        return 1;
    }
    return 0;
}

static int cmd_dac_tone(int argc, char **argv)
{
    if (dac_busy()) {
        return 0;
    }

    struct tone_arg *arg = pvPortMalloc(sizeof(*arg));
    if (arg == NULL) {
        return -1;
    }
    arg->freq_hz = (argc > 1) ? (uint32_t)atoi(argv[1]) : 1000;
    arg->seconds = (argc > 2) ? (uint32_t)atoi(argv[2]) : 5;
    /* Default well below full scale on purpose. A sustained full-scale sine
     * has ~80x the average power of speech and no gaps for the amplifier's
     * boost rail to recover. Measured on the product board: 80% is clean,
     * 100% distorts, and 100% held for 5 s browns the supply out into a
     * reset. It will also cook a small speaker's voice coil, which speech
     * never does. Pass 100 explicitly, with the speaker unplugged, when the
     * goal is measuring full-scale swing on the DAC pin. */
    arg->amp_pct = (argc > 3) ? (uint32_t)atoi(argv[3]) : TONE_AMP_DEFAULT;

    if (arg->freq_hz < 100 || arg->freq_hz > 12000) arg->freq_hz = 1000;
    if (arg->seconds < 1   || arg->seconds > 60)    arg->seconds = 5;
    if (arg->amp_pct < 1   || arg->amp_pct > 100)   arg->amp_pct = 100;

    tone_lut_build();
    s_stop_req = false;
    s_mode = DAC_TONE;

    if (xTaskCreate(tone_task, "dac_tone", 2048, arg, 10, &s_task) != pdPASS) {
        vPortFree(arg);
        s_mode = DAC_IDLE;
        printf("dac_tone: task create failed\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_tone, dac_tone,
                       Play sine via DAC: dac_tone [freq] [sec] [amp pct]);

static int cmd_pdm_play(int argc, char **argv)
{
    if (dac_busy()) {
        return 0;
    }
    if (pdm_is_recording() || pdm_is_streaming()) {
        printf("pdm_play: stop recording/streaming first\r\n");
        return 0;
    }

    uint32_t total = pdm_get_recorded_bytes() / sizeof(int16_t);
    if (pdm_get_buffer() == NULL || total == 0) {
        printf("pdm_play: nothing recorded — run pdm_rec first\r\n");
        return 0;
    }
    if (argc > 1) {
        int g = atoi(argv[1]);
        s_gain_db = (g < GAIN_DB_MIN) ? GAIN_DB_MIN
                  : (g > GAIN_DB_MAX) ? GAIN_DB_MAX : g;
        gain_recalc();
    }

    s_stop_req = false;
    s_mode = DAC_PLAY;

    if (xTaskCreate(play_task, "dac_play", 2048,
                    (void *)(uintptr_t)total, 10, &s_task) != pdPASS) {
        s_mode = DAC_IDLE;
        printf("pdm_play: task create failed\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_play, pdm_play,
                       Replay pdm_rec buffer via DAC: pdm_play [gain dB]);

/* Same arithmetic as pdm_play, without opening the DAC: the levels are all
 * computed before anything reaches the converter, so a gain sweep needs no
 * sound and no real-time wait. Prints in the identical format so the same
 * host parser handles both. */
static int cmd_pdm_level(int argc, char **argv)
{
    uint32_t total = pdm_get_recorded_bytes() / sizeof(int16_t);
    const int16_t *src = pdm_get_buffer();

    if (src == NULL || total == 0) {
        printf("pdm_level: nothing recorded — run pdm_rec first\r\n");
        return 0;
    }
    if (argc > 1) {
        int g = atoi(argv[1]);
        s_gain_db = (g < GAIN_DB_MIN) ? GAIN_DB_MIN
                  : (g > GAIN_DB_MAX) ? GAIN_DB_MAX : g;
        gain_recalc();
    }

    int16_t  buf[FRAME_SAMPLES];
    level_t  in_lvl = {0};
    level_t  out_lvl = {0};
    uint32_t clipped = 0;

    for (uint32_t off = 0; off < total; off += FRAME_SAMPLES) {
        uint32_t n = total - off;
        if (n > FRAME_SAMPLES) {
            n = FRAME_SAMPLES;
        }
        level_add(&in_lvl, &src[off], n);
        clipped += gain_apply(&src[off], buf, n);
        level_add(&out_lvl, buf, n);
    }

    printf("pdm_level: %lu samples (%.2f s)  pdm_gain %d dB  dac_gain %d dB\r\n",
           (unsigned long)total, (double)total / PCM_RATE,
           pdm_get_rec_gain(), s_gain_db);
    level_print("in", &in_lvl);
    level_print("out", &out_lvl);
    if (clipped) {
        printf("  clipped %lu samples (%.2f%%)\r\n",
               (unsigned long)clipped,
               100.0 * (double)clipped / (double)total);
    }
    printf("pdm_level: done\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_level, pdm_level,
                       Level check without playing: pdm_level [gain dB]);

static int cmd_pdm_loop(int argc, char **argv)
{
    if (dac_busy()) {
        return 0;
    }
    if (pdm_is_recording() || pdm_is_streaming()) {
        printf("pdm_loop: stop recording/streaming first\r\n");
        return 0;
    }
    if (s_loop_queue == NULL || s_loop_ring == NULL) {
        printf("pdm_loop: loopback buffers unavailable\r\n");
        return 0;
    }

    uint32_t seconds = (argc > 1) ? (uint32_t)atoi(argv[1]) : 10;
    if (seconds < 1 || seconds > 120) {
        seconds = 10;
    }
    if (argc > 2) {
        int g = atoi(argv[2]);
        s_gain_db = (g < GAIN_DB_MIN) ? GAIN_DB_MIN
                  : (g > GAIN_DB_MAX) ? GAIN_DB_MAX : g;
        gain_recalc();
    }

    s_stop_req = false;
    s_mode = DAC_LOOP;

    if (xTaskCreate(loop_task, "dac_loop", 2048,
                    (void *)(uintptr_t)seconds, 10, &s_task) != pdPASS) {
        s_mode = DAC_IDLE;
        printf("pdm_loop: task create failed\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pdm_loop, pdm_loop,
                       Live PDM to DAC loopback: pdm_loop [sec] [gain dB]);

static int cmd_dac_gain(int argc, char **argv)
{
    if (argc < 2) {
        printf("dac_gain: %d dB (software makeup gain, range %d..%d)\r\n",
               s_gain_db, GAIN_DB_MIN, GAIN_DB_MAX);
        printf("  pdm_gain is the AUADC stage and should be raised first\r\n");
        return 0;
    }

    int g = atoi(argv[1]);
    if (g < GAIN_DB_MIN || g > GAIN_DB_MAX) {
        printf("dac_gain: out of range (%d..%d)\r\n", GAIN_DB_MIN, GAIN_DB_MAX);
        return 0;
    }
    s_gain_db = g;
    gain_recalc();
    printf("dac_gain: %d dB (x%.2f)\r\n", s_gain_db, (double)s_gain_q8 / 256.0);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_gain, dac_gain, Show/set DAC software makeup gain in dB);

static int cmd_dac_dc(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: dac_dc <int16>   (-32768..32767)\r\n");
        printf("  Holds a constant DAC code so a multimeter can measure the\r\n");
        printf("  analogue range. Three points give the whole transfer curve:\r\n");
        printf("    dac_dc -32768   -> code    0  = range minimum\r\n");
        printf("    dac_dc 0        -> code  512  = mid-scale\r\n");
        printf("    dac_dc 32767    -> code 1023  = range maximum\r\n");
        printf("  Safe for the speaker: the amp input is AC-coupled, so a\r\n");
        printf("  steady level produces no sustained speaker current.\r\n");
        printf("  Probe GPIO23 itself, not the amplifier side of C64.\r\n");
        return 0;
    }

    int v = atoi(argv[1]);
    if (v < -32768 || v > 32767) {
        printf("dac_dc: out of int16 range\r\n");
        return 0;
    }

    /* Already holding: just retarget, so sweeping the range needs no
     * stop/start dance between measurement points. */
    if (s_mode == DAC_DC) {
        s_dc_value = (int16_t)v;
        return 0;
    }
    if (dac_busy()) {
        return 0;
    }

    s_dc_value = (int16_t)v;
    s_stop_req = false;
    s_mode = DAC_DC;

    if (xTaskCreate(dc_task, "dac_dc", 2048, NULL, 10, &s_task) != pdPASS) {
        s_mode = DAC_IDLE;
        printf("dac_dc: task create failed\r\n");
        return -1;
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_dc, dac_dc, Hold a constant DAC code for DMM measurement);

static int cmd_dac_spk(int argc, char **argv)
{
    if (argc < 2) {
        printf("dac_spk: GPIO%d, unmute level %d (%s)\r\n",
               s_spk_gpio, s_spk_unmute,
               s_spk_unmute ? "active high — LM48580 /SD"
                            : "active low — LM4871 SHUTDOWN");
        printf("Usage: dac_spk <gpio> <unmute_level>\r\n");
        printf("  EVB          : dac_spk 22 0\r\n");
        printf("  product board: dac_spk 3 1   (AMP_POWER_DOWN -> /SD)\r\n");
        return 0;
    }
    if (argc < 3) {
        printf("dac_spk: need both gpio and unmute level\r\n");
        return 0;
    }
    if (dac_busy()) {
        return 0;
    }

    int gpio = atoi(argv[1]);
    int level = atoi(argv[2]);

    if (gpio < 0 || gpio > 41) {
        printf("dac_spk: gpio out of range (0-41)\r\n");
        return 0;
    }
    if (level != 0 && level != 1) {
        printf("dac_spk: unmute level must be 0 or 1\r\n");
        return 0;
    }

    s_spk_gpio = gpio;
    s_spk_unmute = level;
    dac_set_mute(true);         /* park the new pin in its muted state */

    printf("dac_spk: GPIO%d, unmute level %d\r\n", s_spk_gpio, s_spk_unmute);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_spk, dac_spk, Set amp enable pin: dac_spk <gpio> <unmute_level>);

static int cmd_dac_stop(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (s_mode == DAC_IDLE) {
        printf("dac: not running\r\n");
        return 0;
    }

    s_stop_req = true;
    for (int i = 0; i < 50 && s_mode != DAC_IDLE; i++) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    if (s_mode != DAC_IDLE) {
        printf("dac: stop timed out\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dac_stop, dac_stop, Stop DAC tone/playback/loopback);

/* ===================================================================
 * Init — called from main.c
 * =================================================================== */

void dac_test_init(void)
{
    gain_recalc();

    /* Park the amplifier shut down. Without this the enable pin stays an
     * undriven input until the first playback, leaving the amp in whatever
     * state the board's leakage puts it in — on this board it floats
     * enabled, which idles current and lets mic noise through. */
    dac_set_mute(true);

    s_loop_queue = xQueueCreate(LOOP_QUEUE_LEN, sizeof(int16_t *));
    s_loop_ring  = pvPortMalloc(LOOP_QUEUE_LEN * FRAME_BYTES);

    if (s_loop_queue == NULL || s_loop_ring == NULL) {
        IPRO_LOGW(TAG, "loopback buffers alloc failed — pdm_loop disabled");
        return;
    }

    IPRO_LOGI(TAG, "DAC test ready (dac_tone / pdm_play / pdm_loop), "
                   "makeup gain %d dB", s_gain_db);
}
