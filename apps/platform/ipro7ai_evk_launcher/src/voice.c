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
 * @file voice.c
 * @brief Mic -> PSRAM -> speaker worker. See voice.h.
 */

#include "voice.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "evk.h"
#include "ipro_log.h"

#define LOG_TAG "VOICE"

/* Above the display task (polling SPI, ~58 ms spins) and below the NPU: a
 * 20 ms mic frame must never wait behind a blit or the stream buffer fills. */
#define VOICE_TASK_STACK    1536
#define VOICE_TASK_PRIO     (configMAX_PRIORITIES - 4)

#define FRAME_SAMPLES       EVK_MIC_FRAME_SAMPLES
#define FRAME_MS            EVK_MIC_FRAME_MS
#define CLIP_MAX_SAMPLES    (EVK_MIC_RATE_HZ * VOICE_MAX_SECONDS)

/* Playback ring: 4 x 20 ms at 16 kHz through the sink's resampler. The
 * producer is this task reading from PSRAM, always on time. */
#define PLAY_RING_COUNT     4
#define PLAY_RING_MS        20

#define MIC_READ_TIMEOUT_MS 100
#define MIC_STALL_LIMIT     20      /* 2 s without a frame: give up and say why */

#define GAIN_DB_DEFAULT     24
#define GAIN_DB_MAX         40

typedef enum {
    CMD_LISTEN_ON,
    CMD_LISTEN_OFF,
    CMD_RECORD,
    CMD_PLAY,
    CMD_STOP,
} voice_cmd_t;

static QueueHandle_t   s_cmds;
static TaskHandle_t    s_task;
static int16_t        *s_clip;                /* PSRAM */
static uint32_t        s_clip_len;            /* samples */
static volatile bool   s_stop_req;
static volatile uint32_t s_want_samples;
static volatile bool   s_then_play;
static bool            s_listening;
static int             s_gain_db = GAIN_DB_DEFAULT;
static int32_t         s_gain_q12;
static voice_status_t  s_st = { .error = "" };

static uint8_t         s_wave[VOICE_WAVE_COLUMNS];
static volatile uint32_t s_wave_head;

static int16_t         s_frame[FRAME_SAMPLES];
static int16_t         s_work[FRAME_SAMPLES];

/* ---------------------------------------------------------------------------
 * Helpers
 * -------------------------------------------------------------------------*/

static void wave_push(uint16_t peak)
{
    uint32_t h = (s_wave_head + 1u) % VOICE_WAVE_COLUMNS;
    s_wave[h] = (uint8_t)(peak >= 32768u ? 255u : (peak * 255u) / 32768u);
    s_wave_head = h;
}

static void meter(const int16_t *pcm, uint32_t n)
{
    uint16_t peak, rms;

    evk_pcm_level(pcm, n, &peak, &rms);
    s_st.peak = peak;
    s_st.rms  = rms;
    s_st.peak_dbfs_x10 = evk_pcm_dbfs_x10(peak);
    wave_push(peak);
}

/* Pull one mic frame with stall detection. Returns samples or 0 (and sets
 * the error text after MIC_STALL_LIMIT consecutive misses). */
static uint32_t mic_frame(uint32_t *misses)
{
    uint32_t n = evk_mic_read(s_frame, MIC_READ_TIMEOUT_MS);

    if (n) {
        *misses = 0;
        s_st.mic_ok = true;
        s_st.frames++;
        return n;
    }
    if (++(*misses) >= MIC_STALL_LIMIT) {
        evk_mic_stats_t ms;
        evk_mic_get_stats(&ms);
        s_st.error = ms.isr_frames ? "MIC FRAMES NOT DELIVERED" : "MIC SILENT - NO AUADC FRAMES";
        IPRO_LOGE(LOG_TAG, "%s (isr %u, delivered %u, dropped %u)", s_st.error,
                  (unsigned)ms.isr_frames, (unsigned)ms.delivered, (unsigned)ms.dropped);
        *misses = 0;
    }
    return 0;
}

static bool mic_on(void)
{
    if (evk_mic_running()) {
        return true;
    }
    if (evk_mic_start() != EVK_MIC_OK) {
        s_st.error = "MIC START FAILED";
        return false;
    }
    evk_mic_flush();
    return true;
}

/* ---------------------------------------------------------------------------
 * Modes
 * -------------------------------------------------------------------------*/

/* Listen until a command arrives; returns it. */
static bool do_listen(voice_cmd_t *cmd)
{
    uint32_t misses = 0;

    s_st.state = VOICE_LISTEN;
    while (xQueueReceive(s_cmds, cmd, 0) != pdTRUE) {
        if (!s_listening) {
            /* Nothing to do: block for the next command. */
            s_st.state = VOICE_IDLE;
            return xQueueReceive(s_cmds, cmd, portMAX_DELAY) == pdTRUE;
        }
        if (!mic_on()) {
            s_listening = false;
            continue;
        }
        uint32_t n = mic_frame(&misses);
        if (n) {
            meter(s_frame, n);
        }
    }
    return true;
}

static void do_record(void)
{
    uint32_t want = s_want_samples, misses = 0;

    if (!mic_on()) {
        return;
    }
    s_st.state     = VOICE_RECORDING;
    s_st.target_ms = want * 1000u / EVK_MIC_RATE_HZ;
    s_st.pos_ms    = 0;
    s_st.error     = "";
    s_clip_len     = 0;
    evk_mic_flush();

    while (s_clip_len < want && !s_stop_req) {
        uint32_t n = mic_frame(&misses);
        if (!n) {
            if (s_st.error[0]) {
                break;
            }
            continue;
        }
        if (s_clip_len + n > want) {
            n = want - s_clip_len;
        }
        memcpy(&s_clip[s_clip_len], s_frame, n * sizeof(int16_t));
        s_clip_len += n;
        s_st.pos_ms = s_clip_len * 1000u / EVK_MIC_RATE_HZ;
        meter(s_frame, n);
    }
    s_st.clip_ms = s_clip_len * 1000u / EVK_MIC_RATE_HZ;
    IPRO_LOGI(LOG_TAG, "recorded %u ms (%u samples)", (unsigned)s_st.clip_ms,
              (unsigned)s_clip_len);
}

static void do_play(void)
{
    uint32_t done = 0;

    if (!s_clip_len) {
        s_st.error = "NOTHING RECORDED";
        return;
    }
    /* Nobody reads the mic while we play, so stop it rather than let the
     * stream buffer overflow and count 140 "dropped" frames per clip (seen
     * on the bench); it comes back below if the page is still listening. */
    bool mic_was_on = evk_mic_running();
    if (mic_was_on) {
        evk_mic_stop();
    }
    if (evk_spk_open(EVK_MIC_RATE_HZ, PLAY_RING_COUNT, PLAY_RING_MS) != EVK_SPK_OK) {
        s_st.error = "SPEAKER OPEN FAILED";
        if (mic_was_on) {
            evk_mic_start();
        }
        return;
    }
    s_st.state   = VOICE_PLAYING;
    s_st.pos_ms  = 0;
    s_st.clipped = 0;
    s_st.error   = "";

    while (done < s_clip_len && !s_stop_req) {
        uint32_t n = s_clip_len - done;
        if (n > FRAME_SAMPLES) {
            n = FRAME_SAMPLES;
        }
        s_st.clipped += evk_pcm_apply_gain(&s_clip[done], s_work, n, s_gain_q12);
        meter(s_work, n);
        if (evk_spk_write(s_work, n) != EVK_SPK_OK) {
            s_st.error = "SPEAKER WRITE FAILED";
            break;
        }
        done += n;
        s_st.pos_ms = done * 1000u / EVK_MIC_RATE_HZ;
    }
    /* Drain the ring before the amplifier goes off, or the tail is cut. */
    evk_spk_write_silence(EVK_MIC_RATE_HZ * PLAY_RING_COUNT * PLAY_RING_MS / 1000u);
    evk_spk_close();
    if (mic_was_on && s_listening) {
        evk_mic_start();                  /* discards its own settling frames */
    }
    IPRO_LOGI(LOG_TAG, "played %u ms, %u clipped", (unsigned)s_st.pos_ms,
              (unsigned)s_st.clipped);
}

/* ---------------------------------------------------------------------------
 * Task
 * -------------------------------------------------------------------------*/

static void voice_task(void *arg)
{
    voice_cmd_t cmd;

    (void)arg;
    for (;;) {
        if (!do_listen(&cmd)) {
            continue;
        }
        s_stop_req = false;
        switch (cmd) {
        case CMD_LISTEN_ON:
            s_listening = true;
            break;
        case CMD_LISTEN_OFF:
            s_listening = false;
            evk_mic_stop();
            s_st.state = VOICE_IDLE;
            break;
        case CMD_RECORD:
            do_record();
            if (s_then_play && s_clip_len && !s_stop_req) {
                do_play();
            }
            break;
        case CMD_PLAY:
            do_play();
            break;
        case CMD_STOP:
        default:
            break;
        }
        if (!s_listening) {
            evk_mic_stop();
        }
    }
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

static int send(voice_cmd_t c)
{
    if (!s_cmds) {
        return -1;
    }
    return xQueueSend(s_cmds, &c, pdMS_TO_TICKS(100)) == pdTRUE ? 0 : -1;
}

int voice_init(void)
{
    if (s_task) {
        return 0;
    }
    if (!s_clip) {
        s_clip = pvPortMalloc(CLIP_MAX_SAMPLES * sizeof(int16_t));   /* 320 KB PSRAM */
        if (!s_clip) {
            s_st.error = "NO PSRAM FOR CLIP";
            IPRO_LOGE(LOG_TAG, "clip buffer alloc failed");
            return -1;
        }
    }
    s_gain_q12 = evk_pcm_gain_q12(s_gain_db);

    /* Configure the AUADC now, on the boot task: the pad hand-over and the
     * one-time hal_auadc_pdm_init() happen before anything else competes for
     * the DMA allocator, and a mic that is dead shows up in the boot log
     * rather than on the first visit to the voice page. Later starts only
     * gate the AUADC. */
    if (evk_mic_start() == EVK_MIC_OK) {
        evk_mic_stop();
    } else {
        s_st.error = "MIC INIT FAILED";
        IPRO_LOGE(LOG_TAG, "PDM init failed - the voice page will say so");
    }

    s_cmds = xQueueCreate(4, sizeof(voice_cmd_t));
    if (!s_cmds) {
        return -1;
    }
    if (xTaskCreate(voice_task, "voice", VOICE_TASK_STACK, NULL, VOICE_TASK_PRIO,
                    &s_task) != pdPASS) {
        return -1;
    }
    return 0;
}

int voice_listen(bool on)
{
    return send(on ? CMD_LISTEN_ON : CMD_LISTEN_OFF);
}

int voice_record(uint32_t seconds, bool then_play)
{
    if (seconds < 1 || seconds > VOICE_MAX_SECONDS) {
        return -1;
    }
    if (s_st.state == VOICE_RECORDING || s_st.state == VOICE_PLAYING) {
        return -1;
    }
    s_want_samples = seconds * EVK_MIC_RATE_HZ;
    s_then_play    = then_play;
    return send(CMD_RECORD);
}

int voice_play(void)
{
    if (!s_clip_len || s_st.state == VOICE_RECORDING || s_st.state == VOICE_PLAYING) {
        return -1;
    }
    return send(CMD_PLAY);
}

void voice_stop(void)
{
    s_stop_req = true;
}

void voice_get_status(voice_status_t *out)
{
    if (out) {
        *out = s_st;
    }
}

bool voice_has_clip(void)
{
    return s_clip_len != 0;
}

int voice_set_gain_db(int db)
{
    if (db < 0 || db > GAIN_DB_MAX) {
        return -1;
    }
    s_gain_db  = db;
    s_gain_q12 = evk_pcm_gain_q12(db);
    return 0;
}

int voice_get_gain_db(void)
{
    return s_gain_db;
}

const uint8_t *voice_wave(uint32_t *head)
{
    if (head) {
        *head = s_wave_head;
    }
    return s_wave;
}

void voice_wave_clear(void)
{
    memset(s_wave, 0, sizeof(s_wave));
}
