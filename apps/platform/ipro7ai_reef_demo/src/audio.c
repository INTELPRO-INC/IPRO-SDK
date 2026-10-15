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
 * @file audio.c
 * @brief Tone, record, play and monitor on top of components/audio.
 */

#include "audio.h"
#include "board_config.h"

#include <string.h>
#include <math.h>

#include <FreeRTOS.h>
#include <task.h>

#include "audio_output.h"
#include "audio_capture.h"
#include "keys.h"
#include "hal_gpio.h"
#include "drv_glb.h"
#include "ipro_log.h"

#define LOG_TAG "AUDIO"

#define TONE_RATE_HZ      32000u   /* DAC native rate, skips the resampler */
#define TONE_CHUNK        512u
#define TONE_LUT_SIZE     256u

#define MIC_DIGITAL_GAIN_DB   18.0f  /* AUADC hardware maximum for PDM */
#define HSMIC_ANALOG_GAIN_DB  24     /* AUADC PGA, 6..42 in 3 dB steps */
#define AUX_ANALOG_GAIN_DB    6      /* line level - keep the PGA low */
#define GAIN_DB_DEFAULT       20

typedef enum { JOB_NONE = 0, JOB_TONE, JOB_RECORD, JOB_PLAY, JOB_LOOP } job_t;

typedef struct {
    job_t         job;
    audio_in_t   src;
    audio_route_t route;
    uint32_t      freq_hz;
    uint32_t      ms;
    uint32_t      amp_pct;
    uint32_t      seconds;
} job_args_t;

static TaskHandle_t   s_task;
static volatile bool  s_busy;
static volatile bool  s_stop_req;
static job_args_t     s_args;

static int16_t       *s_rec;
static uint32_t       s_rec_cap;       /* samples */
static uint32_t       s_rec_len;       /* samples recorded */
static int            s_gain_db = GAIN_DB_DEFAULT;
static int32_t        s_gain_q8  = 256 * 10;   /* 20 dB */
static int            s_pdm_ch   = BOARD_MIC_PDM_CHANNEL;
static bool           s_spk_on;
static int16_t        s_peak;
static uint32_t       s_rms_amp;       /* RMS of the last take, after the settle window */
static int32_t        s_settle_peak;   /* peak inside the settle window */
static uint32_t       s_band_rms_amp;  /* 300-3400 Hz RMS of the last take */
static int32_t        s_dc;            /* mean of the last take */
static int16_t        s_lut[TONE_LUT_SIZE];
static bool           s_lut_ready;

/* ---------------------------------------------------------------------------
 * Amplifier
 * -------------------------------------------------------------------------*/

void audio_spk_enable(bool on)
{
    uint8_t pin[1] = { BOARD_SPK_EN_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SPK_EN_PIN, GPIO_OUTPUT_PP_MODE);
    gpio_write(BOARD_SPK_EN_PIN, on ? BOARD_SPK_EN_ACTIVE : !BOARD_SPK_EN_ACTIVE);
    s_spk_on = on;
}

bool audio_spk_enabled(void) { return s_spk_on; }

static void route_apply(audio_route_t route)
{
    audio_spk_enable(route == AUDIO_ROUTE_SPK);
}

/* ---------------------------------------------------------------------------
 * Output sink
 * -------------------------------------------------------------------------*/

static audio_output_t *sink_open(uint32_t rate, audio_route_t route)
{
    audio_output_cfg_t cfg = {
        .type             = AUDIO_OUTPUT_DAC,
        .mode             = AUDIO_OUTPUT_MODE_STREAMING,
        .sample_rate      = rate,
        .channels         = 1,
        .dac_spk_gpio     = BOARD_SPK_EN_PIN,
        .dac_spk_en_active = BOARD_SPK_EN_ACTIVE ? 1 : -1,
    };
    audio_output_t *out = audio_output_create(&cfg);

    if (!out) {
        IPRO_LOGE(LOG_TAG, "audio_output_create failed");
        return NULL;
    }
    if (audio_output_start(out) != 0) {
        IPRO_LOGE(LOG_TAG, "audio_output_start failed");
        audio_output_destroy(out);
        return NULL;
    }
    /* start() switched the amplifier on; the route decides whether it stays. */
    route_apply(route);
    return out;
}

static void sink_close(audio_output_t *out)
{
    if (!out) {
        return;
    }
    audio_output_stop(out);
    audio_output_destroy(out);
    audio_spk_enable(false);
}

/* ---------------------------------------------------------------------------
 * Capture source
 * -------------------------------------------------------------------------*/

static int capture_open(audio_in_t src, int *consumer)
{
    audio_capture_cfg_t cfg = {
        .sample_rate   = AUDIO_RATE_HZ,
        .frame_samples = AUDIO_FRAME_SAMPLES,
    };

    switch (src) {
    case AUDIO_IN_PDM:
        cfg.type = AUDIO_SRC_DMIC;
        cfg.dmic.clk_pin         = BOARD_MIC_CLK_PIN;
        cfg.dmic.data_pin        = BOARD_MIC_DAT_PIN;
        cfg.dmic.channel         = (uint8_t)s_pdm_ch;
        cfg.dmic.digital_gain_db = MIC_DIGITAL_GAIN_DB;
        break;
    case AUDIO_IN_HSMIC:
        cfg.type = AUDIO_SRC_AMIC;
        cfg.amic.single_ended = 0;
        cfg.amic.pos_ch       = BOARD_HSMIC_POS_CH;
        cfg.amic.neg_ch       = BOARD_HSMIC_NEG_CH;
        cfg.amic.gain_db      = HSMIC_ANALOG_GAIN_DB;
        break;
    case AUDIO_IN_AUX:
        cfg.type = AUDIO_SRC_AMIC;
        cfg.amic.single_ended = 1;
        cfg.amic.pos_ch       = BOARD_AUX_L_CH;
        cfg.amic.neg_ch       = BOARD_AUX_R_CH;
        cfg.amic.gain_db      = AUX_ANALOG_GAIN_DB;
        break;
    default:
        return -1;
    }

    if (audio_capture_init(&cfg) != 0) {
        IPRO_LOGE(LOG_TAG, "capture init failed (%s)", audio_src_name(src));
        return -1;
    }
    *consumer = audio_capture_add_consumer(AUDIO_FRAME_SAMPLES * sizeof(int16_t) * 8);
    if (*consumer < 0) {
        audio_capture_deinit();
        return -1;
    }
    if (audio_capture_start() != 0) {
        audio_capture_deinit();
        return -1;
    }
    return 0;
}

static void capture_close(void)
{
    audio_capture_stop();
    audio_capture_deinit();
}

/* ---------------------------------------------------------------------------
 * Helpers
 * -------------------------------------------------------------------------*/

static void gain_apply(int16_t *pcm, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        int32_t v = ((int32_t)pcm[i] * s_gain_q8) >> 8;
        if (v > 32767) v = 32767;
        if (v < -32768) v = -32768;
        pcm[i] = (int16_t)v;
        if (v < 0) v = -v;
        if (v > s_peak) s_peak = (int16_t)v;
    }
}

static void lut_build(void)
{
    if (s_lut_ready) {
        return;
    }
    for (uint32_t i = 0; i < TONE_LUT_SIZE; i++) {
        s_lut[i] = (int16_t)(32767.0f * sinf(2.0f * 3.14159265f * (float)i / TONE_LUT_SIZE));
    }
    s_lut_ready = true;
}

static int rec_alloc(void)
{
    if (s_rec) {
        return 0;
    }
    s_rec_cap = AUDIO_RATE_HZ * AUDIO_REC_MAX_SEC;
    s_rec = pvPortMalloc(s_rec_cap * sizeof(int16_t));
    if (!s_rec) {
        IPRO_LOGE(LOG_TAG, "record buffer alloc failed (%u samples)", (unsigned)s_rec_cap);
        s_rec_cap = 0;
        return -1;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 * Jobs
 * -------------------------------------------------------------------------*/

static void do_tone(const job_args_t *a)
{
    static int16_t chunk[TONE_CHUNK];
    uint32_t total = (uint64_t)a->ms * TONE_RATE_HZ / 1000u;
    uint32_t done = 0, phase = 0;
    uint32_t step = (uint32_t)(((uint64_t)a->freq_hz * TONE_LUT_SIZE * 65536u) / TONE_RATE_HZ);
    int32_t  amp_q15 = (int32_t)((a->amp_pct > 100u ? 100u : a->amp_pct) * 32768u / 100u);
    audio_output_t *out;

    lut_build();
    out = sink_open(TONE_RATE_HZ, a->route);
    if (!out) {
        return;
    }
    while (done < total && !s_stop_req) {
        uint32_t n = (total - done < TONE_CHUNK) ? (total - done) : TONE_CHUNK;
        for (uint32_t i = 0; i < n; i++) {
            chunk[i] = (int16_t)(((int32_t)s_lut[(phase >> 16) & (TONE_LUT_SIZE - 1)] * amp_q15) >> 15);
            phase += step;
        }
        audio_output_write(out, chunk, n);
        done += n;
    }
    /* Let the ring drain so the tone is not clipped short. */
    vTaskDelay(pdMS_TO_TICKS(200));
    sink_close(out);
}

/* Every capture_open() starts the PDM decimator / AUADC from zero, and the
 * first tens of ms carry that step: on the Reef a silent PDM take peaked at
 * -2 dBFS with it counted (bench, 2026-10-06). Peak and RMS therefore leave
 * out the first REC_SETTLE_MS; the step's own peak is reported separately. */
#define REC_SETTLE_MS  100u

static int amp_dbfs(int32_t amp)
{
    if (amp <= 0) {
        return -96;
    }
    return (int)(20.0f * log10f((float)amp / 32768.0f));
}

/* One-pole high-pass 300 Hz and low-pass 3400 Hz at 16 kHz, Q15: the voice
 * band. Full-band RMS on a MEMS mic is dominated by DC and mains hum, so it
 * says little about speech (see the EVK PDM gain-budget measurements). */
#define BAND_HPF_A_Q15  29315   /* 1 / (1 + 2*pi*300/16000) */
#define BAND_LPF_A_Q15  18736   /* x / (1 + x), x = 2*pi*3400/16000 */

/* Peak / RMS / DC / voice-band RMS of the raw take, before make-up gain, so
 * they report the mic. Integer filters; only the final square roots are float. */
static void rec_stats(void)
{
    uint32_t skip = AUDIO_RATE_HZ * REC_SETTLE_MS / 1000u;
    uint64_t sq = 0, bsq = 0;
    int64_t  sum = 0;
    int32_t  peak = 0, x_prev = 0, hp = 0, lp = 0;
    uint32_t n = (s_rec_len > skip) ? s_rec_len - skip : 0u;

    s_settle_peak = 0;
    for (uint32_t i = 0; i < s_rec_len; i++) {
        int32_t x = s_rec[i];
        int32_t v = x < 0 ? -x : x;

        hp = (int32_t)(((int64_t)BAND_HPF_A_Q15 * (hp + x - x_prev)) >> 15);
        x_prev = x;
        lp += (int32_t)(((int64_t)BAND_LPF_A_Q15 * (hp - lp)) >> 15);
        if (i < skip) {
            if (v > s_settle_peak) s_settle_peak = v;
            continue;
        }
        if (v > peak) peak = v;
        sq  += (uint64_t)((int64_t)x * x);
        bsq += (uint64_t)((int64_t)lp * lp);
        sum += x;
    }
    s_peak = (int16_t)(peak > 32767 ? 32767 : peak);
    s_rms_amp      = n ? (uint32_t)sqrtf((float)(sq / n)) : 0u;
    s_band_rms_amp = n ? (uint32_t)sqrtf((float)(bsq / n)) : 0u;
    s_dc           = n ? (int32_t)(sum / (int64_t)n) : 0;
}

static void do_record(const job_args_t *a)
{
    int consumer;
    uint32_t want = a->seconds * AUDIO_RATE_HZ;

    if (rec_alloc() != 0 || capture_open(a->src, &consumer) != 0) {
        return;
    }
    if (want > s_rec_cap) {
        want = s_rec_cap;
    }
    s_rec_len = 0;
    s_peak = 0;
    while (s_rec_len < want && !s_stop_req) {
        uint32_t room = want - s_rec_len;
        uint32_t n = (room < AUDIO_FRAME_SAMPLES) ? room : AUDIO_FRAME_SAMPLES;
        size_t got = audio_capture_read(consumer, &s_rec[s_rec_len], n * sizeof(int16_t), 200);
        if (got == 0) {
            IPRO_LOGW(LOG_TAG, "capture stalled at %u samples", (unsigned)s_rec_len);
            break;
        }
        s_rec_len += got / sizeof(int16_t);
    }
    capture_close();
    rec_stats();
    IPRO_LOGI(LOG_TAG, "recorded %u samples from %s: peak %d dBFS, rms %d dBFS, dc %d, "
                       "voice band (300-3400 Hz) rms %d dBFS; first %u ms left out "
                       "(peak there %d dBFS)",
              (unsigned)s_rec_len, audio_src_name(a->src), audio_last_peak_dbfs(),
              audio_last_rms_dbfs(), (int)s_dc, audio_last_band_rms_dbfs(),
              (unsigned)REC_SETTLE_MS, amp_dbfs(s_settle_peak));
}

static void do_play(const job_args_t *a)
{
    static int16_t chunk[AUDIO_FRAME_SAMPLES];
    audio_output_t *out;
    uint32_t pos = 0;

    if (!s_rec || s_rec_len == 0) {
        IPRO_LOGW(LOG_TAG, "nothing recorded yet");
        return;
    }
    out = sink_open(AUDIO_RATE_HZ, a->route);
    if (!out) {
        return;
    }
    while (pos < s_rec_len && !s_stop_req) {
        uint32_t n = (s_rec_len - pos < AUDIO_FRAME_SAMPLES) ? (s_rec_len - pos) : AUDIO_FRAME_SAMPLES;
        memcpy(chunk, &s_rec[pos], n * sizeof(int16_t));
        gain_apply(chunk, n);
        audio_output_write(out, chunk, n);
        pos += n;
    }
    /* Let the ring drain before the DAC is torn down. */
    vTaskDelay(pdMS_TO_TICKS(200));
    sink_close(out);
}

static void do_loop(const job_args_t *a)
{
    static int16_t chunk[AUDIO_FRAME_SAMPLES];
    audio_output_t *out;
    int consumer;
    uint32_t frames = a->seconds ? a->seconds * (AUDIO_RATE_HZ / AUDIO_FRAME_SAMPLES) : 0;
    uint32_t done = 0;

    if (capture_open(a->src, &consumer) != 0) {
        return;
    }
    out = sink_open(AUDIO_RATE_HZ, a->route);
    if (!out) {
        capture_close();
        return;
    }
    s_peak = 0;
    while (!s_stop_req && (frames == 0 || done < frames)) {
        size_t got = audio_capture_read(consumer, chunk, sizeof(chunk), 200);
        if (got == 0) {
            IPRO_LOGW(LOG_TAG, "capture stalled");
            break;
        }
        gain_apply(chunk, got / sizeof(int16_t));
        audio_output_write(out, chunk, got / sizeof(int16_t));
        done++;
    }
    sink_close(out);
    capture_close();
}

static void audio_task(void *arg)
{
    (void)arg;

    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        bool captures_aux = (s_args.job == JOB_RECORD || s_args.job == JOB_LOOP) &&
                            s_args.src == AUDIO_IN_AUX;

        if (captures_aux) {
            keys_set_aux_mode(true);     /* GPIO10/11 become AIN4/AIN5 */
        }
        switch (s_args.job) {
        case JOB_TONE:   do_tone(&s_args);   break;
        case JOB_RECORD: do_record(&s_args); break;
        case JOB_PLAY:   do_play(&s_args);   break;
        case JOB_LOOP:   do_loop(&s_args);   break;
        default: break;
        }
        if (captures_aux) {
            keys_set_aux_mode(false);    /* back to SW3/SW5 */
        }
        s_args.job = JOB_NONE;
        s_stop_req = false;
        s_busy = false;
    }
}

static int job_submit(const job_args_t *a)
{
    if (!s_task) {
        return -1;
    }
    if (s_busy) {
        IPRO_LOGW(LOG_TAG, "busy (%s) - `spk stop` / `mic stop` first", audio_job_name());
        return -1;
    }
    s_args = *a;
    s_stop_req = false;
    s_busy = true;              /* claimed here so a second submit is refused
                                 * even before the worker has woken up */
    xTaskNotifyGive(s_task);
    return 0;
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

int audio_init(void)
{
    if (s_task) {
        return 0;
    }
    audio_spk_enable(false);
    if (xTaskCreate(audio_task, "audio", 2048, NULL, configMAX_PRIORITIES - 3, &s_task) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "task create failed");
        return -1;
    }
    return 0;
}

int audio_tone(uint32_t freq_hz, uint32_t ms, uint32_t amp_pct, audio_route_t route)
{
    job_args_t a = { .job = JOB_TONE, .route = route,
                     .freq_hz = freq_hz, .ms = ms, .amp_pct = amp_pct };
    if (freq_hz < 20u || freq_hz > 12000u || ms == 0u) {
        return -1;
    }
    return job_submit(&a);
}

int audio_record(audio_in_t src, uint32_t seconds)
{
    job_args_t a = { .job = JOB_RECORD, .src = src, .seconds = seconds };
    if (src >= AUDIO_IN_COUNT || seconds == 0u || seconds > AUDIO_REC_MAX_SEC) {
        return -1;
    }
    return job_submit(&a);
}

int audio_play(audio_route_t route)
{
    job_args_t a = { .job = JOB_PLAY, .route = route };
    return job_submit(&a);
}

int audio_loop(audio_in_t src, uint32_t seconds, audio_route_t route)
{
    job_args_t a = { .job = JOB_LOOP, .src = src, .route = route, .seconds = seconds };
    if (src >= AUDIO_IN_COUNT) {
        return -1;
    }
    return job_submit(&a);
}

void audio_stop(void)
{
    if (!s_busy) {
        return;
    }
    s_stop_req = true;
    for (int i = 0; i < 100 && s_busy; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

bool audio_busy(void) { return s_busy; }

const char *audio_job_name(void)
{
    switch (s_args.job) {
    case JOB_TONE:   return "tone";
    case JOB_RECORD: return "record";
    case JOB_PLAY:   return "play";
    case JOB_LOOP:   return "loop";
    default:         return "idle";
    }
}

int audio_set_gain_db(int db)
{
    if (db < 0 || db > 40) {
        return -1;
    }
    s_gain_db = db;
    s_gain_q8 = (int32_t)(256.0f * powf(10.0f, (float)db / 20.0f));
    return 0;
}

int audio_get_gain_db(void) { return s_gain_db; }

int audio_set_pdm_channel(int ch)
{
    if (ch != 0 && ch != 1) {
        return -1;
    }
    s_pdm_ch = ch;
    return 0;
}

int audio_get_pdm_channel(void) { return s_pdm_ch; }

int audio_last_peak_dbfs(void)
{
    return amp_dbfs(s_peak);
}

uint32_t audio_last_rms_amp(void)
{
    return s_rms_amp;
}

int audio_last_rms_dbfs(void)
{
    return amp_dbfs((int32_t)s_rms_amp);
}

int audio_last_band_rms_dbfs(void)
{
    return amp_dbfs((int32_t)s_band_rms_amp);
}

uint32_t audio_rec_samples(void) { return s_rec_len; }

const char *audio_src_name(audio_in_t src)
{
    switch (src) {
    case AUDIO_IN_PDM:   return "pdm";
    case AUDIO_IN_HSMIC: return "headset";
    case AUDIO_IN_AUX:   return "aux";
    default:              return "?";
    }
}
