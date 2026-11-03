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
 * @file sensor_app.c
 * @brief Source -> ring -> stream and inference. See sensor_app.h.
 */
#include "sensor_app.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "board_config.h"
#include "drv_glb.h"
#include "hal_gpio.h"
#include "ipro_log.h"
#include "rgb_led.h"

#include "sensor_front.h"
#include "sensor_model.h"
#include "sensor_src.h"

#define LOG_TAG        "SENS"
#define RING_BYTES     (1024u * 1024u)          /* PSRAM: 8 s of mic, 170 s of sim */
#define CHUNK_FRAMES   256u
#define STREAM_BYTES   4000u                    /* one DATA payload at most */
#define STREAM_MS      100u
#define LIVE_HOPS      4u                       /* inferences per second */
#define TASK_STACK     4096u                    /* words; the NPU call wants 16 KB */
#define TASK_PRIO      (configMAX_PRIORITIES - 4)
#define KEY_DEBOUNCE   3u                       /* polls of ~10 ms */

typedef enum {
    REQ_HELLO, REQ_SRC, REQ_SIM, REQ_STREAM, REQ_LIVE, REQ_PAUSE, REQ_RESUME, REQ_SRCS, REQ_SIMS,
    REQ_SIMCFG
} req_type_t;

typedef struct {
    req_type_t  type;
    int         on;
    char        name[24];
    sa_simcfg_t cfg;
} req_t;

static const uint8_t s_colors[SM_MAX_CLASSES][3] = {
    { 0, 40, 0 }, { 40, 0, 0 }, { 0, 0, 40 }, { 40, 25, 0 },
    { 30, 0, 30 }, { 0, 30, 30 }, { 30, 30, 30 }, { 40, 8, 0 },
};

static QueueHandle_t s_q;
static sa_line_fn    s_line;
static sa_bin_fn     s_bin;
static char          s_txt[256];

static int16_t      *s_ring;                    /* frames x channels */
static uint32_t      s_ring_frames, s_ch;
static uint64_t      s_wpos;                    /* frames written since the source opened */
static uint64_t      s_next_live;
static int16_t       s_chunk[CHUNK_FRAMES * SF_MAX_CH];
static uint8_t       s_stream[STREAM_BYTES];
static uint32_t      s_stream_n, s_stream_due;
static uint64_t      s_stream_first;          /* frame index of the payload's first frame */
static bool          s_streaming, s_live, s_paused, s_print;
static volatile bool s_pause_ack;
static int8_t        s_img[SF_IMG_BYTES];
static uint32_t      s_runs, s_last_us, s_mismatch_warned;
static int           s_last_class = -1;
static uint8_t       s_key_stable, s_key_down;

static void emit(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(s_txt, sizeof(s_txt) - 2, fmt, ap);
    va_end(ap);
    if (n < 0) {
        return;
    }
    if (n > (int)sizeof(s_txt) - 3) {
        n = (int)sizeof(s_txt) - 3;
    }
    s_txt[n] = '\n';
    s_txt[n + 1] = 0;
    if (s_line) {
        s_line(s_txt);
    }
}

/* ---------------------------------------------------------------- ring */

static int open_source(src_id_t id)
{
    const src_info_t *in = src_info(id);

    if (!in || src_open(id) != 0) {
        return -1;
    }
    s_ch = in->channels;
    s_ring_frames = RING_BYTES / (2u * s_ch);
    s_wpos = 0;
    s_next_live = 0;
    s_stream_n = 0;
    s_stream_due = in->rate * STREAM_MS / 1000u;
    if (s_stream_due * s_ch * 2u > STREAM_BYTES) {
        s_stream_due = STREAM_BYTES / (s_ch * 2u);
    }
    return 0;
}

static void ring_push(const int16_t *x, uint32_t frames)
{
    for (uint32_t i = 0; i < frames; i++) {
        memcpy(&s_ring[(uint32_t)((s_wpos + i) % s_ring_frames) * s_ch], &x[i * s_ch],
               s_ch * sizeof(int16_t));
    }
    s_wpos += frames;
}

static void stream_push(const int16_t *x, uint32_t frames)
{
    const uint32_t fbytes = s_ch * 2u;

    for (uint32_t i = 0; i < frames; i++) {
        if (s_stream_n == 0) {
            s_stream_first = s_wpos - frames + i;
        }
        memcpy(&s_stream[s_stream_n], &x[i * s_ch], fbytes);
        s_stream_n += fbytes;
        if (s_stream_n >= s_stream_due * fbytes || s_stream_n + fbytes > STREAM_BYTES) {
            char hdr[48];
            /* the frame number lets the host see a dropped payload as a gap */
            snprintf(hdr, sizeof(hdr), "DATA %lu %lu\n", (unsigned long)s_stream_n,
                     (unsigned long)s_stream_first);
            if (s_bin) {
                s_bin(hdr, s_stream, s_stream_n);
            }
            s_stream_n = 0;
        }
    }
}

typedef struct { uint64_t start; } ring_view_t;

static float ring_sample(void *ctx, uint32_t ch, uint32_t i)
{
    const ring_view_t *v = ctx;
    return (float)s_ring[(uint32_t)((v->start + i) % s_ring_frames) * s_ch + ch];
}

/* ---------------------------------------------------------------- live */

static bool model_fits(const sensor_cfg_t *m)
{
    const src_info_t *in = src_info(src_current());
    const bool ok = src_is_open() && !strcmp(m->source, in->name) && m->front.channels == s_ch &&
                    fabsf(m->rate - (float)in->rate) < 0.5f && m->front.window < s_ring_frames;
    if (!ok && !s_mismatch_warned) {
        s_mismatch_warned = 1;
        IPRO_LOGW(LOG_TAG, "model wants %s %lu Hz x%lu, source is %s %lu Hz x%lu: no inference",
                  m->source, (unsigned long)lrintf(m->rate), (unsigned long)m->front.channels, in->name,
                  (unsigned long)in->rate, (unsigned long)s_ch);
    }
    return ok;
}

static void live_step(void)
{
    const sensor_cfg_t *m = sensor_model_cfg();

    if (!m || s_wpos < m->front.window || s_wpos < s_next_live || !model_fits(m)) {
        return;
    }
    const uint32_t hop = (uint32_t)(m->rate / LIVE_HOPS) ? (uint32_t)(m->rate / LIVE_HOPS) : 1u;
    s_next_live = s_wpos + hop;
    ring_view_t v = { s_wpos - m->front.window };
    ipro_ai_result_t res;
    /* the window's first sample, in the source's time */
    const uint64_t t_us = (uint64_t)v.start * 1000000u / src_info(src_current())->rate;
    if (sensor_front(&m->front, ring_sample, &v, s_img) != 0 ||
        sensor_model_classify(s_img, t_us, &res, &s_last_us) != 0) {
        return;
    }
    s_runs++;
    /* the classification adapter (components/ai/app): averaged logits, top
     * class, expf(logit - top), and UNKNOWN under the model's floor */
    const float *p = res.cls.exp, sum = res.cls.exp_sum;
    const int top_class = res.cls.best;
    int best = res.cls.index;               /* -1: nothing it was taught scores like this */
    if (best != s_last_class) {
        s_last_class = best;
        if (best < 0) {
            rgb_led_set(25, 25, 25);
        } else {
            rgb_led_set(s_colors[best][0], s_colors[best][1], s_colors[best][2]);
        }
    }
    const unsigned long ms = (unsigned long)(s_wpos * 1000u / src_info(src_current())->rate);
    if (s_live || s_print) {
        char *o = s_txt;
        int left = (int)sizeof(s_txt) - 2;
        int n = snprintf(o, (size_t)left, "RES %lu %d", ms, best);
        for (uint32_t k = 0; k < m->n && n > 0 && n < left; k++) {
            n += snprintf(o + n, (size_t)(left - n), " %d", (int)lrintf(1000.0f * p[k] / sum));
        }
        if (s_live && s_line) {
            s_txt[n] = '\n';
            s_txt[n + 1] = 0;
            s_line(s_txt);
            s_txt[n] = 0;
        }
        if (s_print) {
            printf("[SENS] %s %s %d%% (%lu us)\r\n", s_txt, best < 0 ? "UNKNOWN" : m->labels[best],
                   (int)lrintf(100.0f * p[top_class] / sum), (unsigned long)s_last_us);
        }
    }
}

/* SW4 steps the simulated machine through its states. */
static void key_poll(void)
{
    const uint8_t down = gpio_read(BOARD_KEY_SW4_PIN) == 0;

    if (down == s_key_down) {
        s_key_stable = 0;
        return;
    }
    if (++s_key_stable < KEY_DEBOUNCE) {
        return;
    }
    s_key_down = down;
    s_key_stable = 0;
    if (down && src_is_sim(src_current())) {
        src_sim_next();
        emit("SIM %s", src_sim_state());
        IPRO_LOGI(LOG_TAG, "sim: %s", src_sim_state());
    }
}

/* ---------------------------------------------------------------- requests */

static void report_sims(void)
{
    if (!src_is_sim(src_current())) {
        emit("SIMS none");
        return;
    }
    const sim_type_info_t *t = sim_type_info((sim_type_t)src_current());
    char *o = s_txt;
    int n = snprintf(o, sizeof(s_txt) - 2, "SIMS %s %s", t->name, src_sim_state());
    for (uint32_t i = 0; i < t->n_states && n > 0 && n < (int)sizeof(s_txt) - 24; i++) {
        n += snprintf(o + n, sizeof(s_txt) - 2 - (size_t)n, " %s", t->states[i]);
    }
    o[n] = 0;
    emit("%s", o);
}

static void report_simcfg(void)
{
    const sim_params_t *p = src_sim_params();
    if (!p) {
        emit("SIMCFG ERR not_simulated");
        return;
    }
    const src_info_t *in = src_info(src_current());
    emit("SIMCFG %s rate %lu ch %lu noise %ld drift %ld severity %ld ramp %ld", in->name,
         (unsigned long)in->rate, (unsigned long)in->channels, lrintf(p->noise * 100.0f),
         lrintf(p->drift * 100.0f), lrintf(p->severity * 100.0f), lrintf(p->ramp_s));
}

static void report_src(void)
{
    const src_info_t *in = src_info(src_current());
    emit("SRC %s %lu %lu %lu %s", in->name, (unsigned long)in->rate, (unsigned long)in->channels,
         (unsigned long)lrintf(in->lsb_per_unit), in->unit);
    if (src_is_sim(src_current())) {
        report_sims();
        report_simcfg();
    }
}

static void apply_simcfg(const sa_simcfg_t *c)
{
    const sim_params_t *cur = src_sim_params();
    if (!cur) {
        emit("SIMCFG ERR not_simulated");
        return;
    }
    const src_info_t *in = src_info(src_current());
    sim_params_t p = *cur;
    p.rate = (c->set & SA_SET_RATE) ? c->rate : in->rate;
    p.channels = (c->set & SA_SET_CH) ? c->channels : in->channels;
    if (c->set & SA_SET_NOISE) {
        p.noise = (float)c->noise_pct / 100.0f;
    }
    if (c->set & SA_SET_DRIFT) {
        p.drift = (float)c->drift_pct / 100.0f;
    }
    if (c->set & SA_SET_SEVERITY) {
        p.severity = (float)c->severity_pct / 100.0f;
    }
    if (c->set & SA_SET_RAMP) {
        p.ramp_s = (float)c->ramp_s;
    }
    bool reopen = false;
    if (src_sim_configure(&p, &reopen) != 0) {
        emit("SIMCFG ERR not_simulated");
        return;
    }
    if (reopen) {                           /* new rate or channels: a new ring */
        open_source(src_current());
        s_mismatch_warned = 0;
        report_src();
        return;
    }
    report_simcfg();
}

static void report_hello(void)
{
    const sensor_cfg_t *m = sensor_model_cfg();
    const src_info_t *in = src_info(src_current());
    char labels[SM_MAX_CLASSES * SM_LABEL_LEN + 2] = "-";

    if (m) {
        labels[0] = 0;
        for (uint32_t k = 0; k < m->n; k++) {
            strcat(labels, m->labels[k]);
            if (k + 1 < m->n) {
                strcat(labels, ",");
            }
        }
    }
    emit("READY %s %lu %s %lu %lu %lu %s %s %s", m ? "model" : "none", (unsigned long)(m ? m->n : 0),
         in->name, (unsigned long)in->rate, (unsigned long)in->channels,
         (unsigned long)lrintf(in->lsb_per_unit), in->unit, src_sim_state(), labels);
}

static void do_request(const req_t *r)
{
    switch (r->type) {
    case REQ_HELLO:
        report_hello();
        break;
    case REQ_SRC: {
        const int id = src_find(r->name);
        const src_id_t prev = src_current();
        if (id < 0) {
            emit("SRC ERR unknown %s", r->name);
        } else if (open_source((src_id_t)id) != 0) {
            /* src_open closed the old one first: put it back, and say so */
            const int back = open_source(prev);
            emit("SRC ERR open %s %s", r->name, back == 0 ? src_info(prev)->name : "none");
        } else {
            s_mismatch_warned = 0;
            report_src();
        }
        break;
    }
    case REQ_SIM:
        if (src_sim_set(r->name) != 0) {
            emit("SIM ERR %s", r->name);
        } else {
            emit("SIM %s", src_sim_state());
        }
        break;
    case REQ_SRCS: {
        char *o = s_txt;
        int n = snprintf(o, sizeof(s_txt) - 2, "SRCS");
        for (int i = 0; i < SRC_COUNT && n > 0; i++) {
            n += snprintf(o + n, sizeof(s_txt) - 2 - (size_t)n, " %s", src_info((src_id_t)i)->name);
        }
        emit("%s", o);
        break;
    }
    case REQ_SIMS:
        report_sims();
        break;
    case REQ_SIMCFG:
        apply_simcfg(&r->cfg);
        break;
    case REQ_STREAM:
        s_streaming = r->on;
        s_stream_n = 0;
        emit("STREAM %s", r->on ? "ON" : "OFF");
        break;
    case REQ_LIVE:
        s_live = r->on;
        emit("LIVE %s", r->on ? "ON" : "OFF");
        break;
    case REQ_PAUSE:
        s_paused = true;
        s_streaming = false;
        src_close();
        s_pause_ack = true;
        break;
    case REQ_RESUME:
        if (s_paused) {
            s_paused = false;
            open_source(src_current());
        }
        break;
    }
}

static void sensor_task(void *arg)
{
    req_t r;

    (void)arg;
    for (;;) {
        while (xQueueReceive(s_q, &r, 0) == pdTRUE) {
            do_request(&r);
        }
        if (s_paused) {
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }
        const uint32_t n = src_read(s_chunk, CHUNK_FRAMES);
        if (n) {
            ring_push(s_chunk, n);
            if (s_streaming) {
                stream_push(s_chunk, n);
            }
            live_step();
        }
        key_poll();
    }
}

/* ---------------------------------------------------------------- API */

static int post(req_type_t type, int on, const char *name)
{
    req_t r = { .type = type, .on = on };

    if (name) {
        if (strlen(name) >= sizeof(r.name)) {
            return -1;
        }
        strcpy(r.name, name);
    }
    return (s_q && xQueueSend(s_q, &r, pdMS_TO_TICKS(100)) == pdTRUE) ? 0 : -1;
}

int sensor_app_hello(void)            { return post(REQ_HELLO, 0, NULL); }
int sensor_app_src(const char *name)  { return post(REQ_SRC, 0, name); }
int sensor_app_sim(const char *state) { return post(REQ_SIM, 0, state); }
int sensor_app_stream(bool on)        { return post(REQ_STREAM, on, NULL); }
int sensor_app_live(bool on)          { return post(REQ_LIVE, on, NULL); }
int sensor_app_srcs(void)             { return post(REQ_SRCS, 0, NULL); }
int sensor_app_sims(void)             { return post(REQ_SIMS, 0, NULL); }

int sensor_app_simcfg(const sa_simcfg_t *c)
{
    req_t r = { .type = REQ_SIMCFG, .cfg = *c };
    return (s_q && xQueueSend(s_q, &r, pdMS_TO_TICKS(100)) == pdTRUE) ? 0 : -1;
}

int sensor_app_pause(void)
{
    s_pause_ack = false;
    if (post(REQ_PAUSE, 0, NULL) != 0) {
        return -1;
    }
    /* longer than an inference plus the slowest send (2 s) */
    for (int i = 0; i < 300 && !s_pause_ack; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (!s_pause_ack) {
        post(REQ_RESUME, 0, NULL);          /* the pause still queued must not park the task */
        return -1;
    }
    return 0;
}

int sensor_app_resume(void)
{
    return post(REQ_RESUME, 0, NULL);
}

void sensor_app_set_sink(sa_line_fn line, sa_bin_fn bin)
{
    s_line = line;
    s_bin = bin;
}

void sensor_app_set_print(bool on)
{
    s_print = on;
}

void sensor_app_print_status(void)
{
    const sensor_cfg_t *m = sensor_model_cfg();
    const src_info_t *in = src_info(src_current());

    const sim_params_t *sp = src_sim_params();
    printf("source  : %s %lu Hz x%lu (%s), %llu frames\r\n", in->name,
           (unsigned long)in->rate, (unsigned long)in->channels, src_is_open() ? "open" : "closed",
           (unsigned long long)s_wpos);
    ipro_ai_source_stats_t st;
    if (src_stats(&st) == 0) {
        printf("  registry: %llu frames, %llu lost, %lu reads (%lu empty), %lu stalls\r\n",
               (unsigned long long)st.frames, (unsigned long long)st.lost, (unsigned long)st.reads,
               (unsigned long)st.empty_reads, (unsigned long)st.stalls);
    }
    if (sp) {
        printf("sim     : %s, noise %ld %%, drift %ld %%, severity %ld %% (now %ld %%), ramp %ld s\r\n",
               src_sim_state(), lrintf(sp->noise * 100.0f), lrintf(sp->drift * 100.0f),
               lrintf(sp->severity * 100.0f), lrintf(sim_severity_now() * 100.0f),
               lrintf(sp->ramp_s));
    }
    if (m) {
        printf("model   : %lu B, %lu classes, %s %lu Hz x%lu, window %lu, %s; %lu runs, last %lu us\r\n",
               (unsigned long)m->inm_bytes, (unsigned long)m->n, m->source,
               (unsigned long)lrintf(m->rate),
               (unsigned long)m->front.channels, (unsigned long)m->front.window,
               m->front.mode == SF_SPECTROGRAM ? "spectrogram" : "wave", (unsigned long)s_runs,
               (unsigned long)s_last_us);
        printf("  size %s, UNKNOWN %s\r\n", m->size[0] ? m->size : "-", m->unknown ? "on" : "off");
        for (uint32_t k = 0; k < m->n; k++) {
            printf("  %lu %s%s\r\n", (unsigned long)k, m->labels[k],
                   (int)k == s_last_class ? "  <" : "");
        }
        if (s_last_class < 0 && s_runs) {
            printf("  last result: UNKNOWN\r\n");
        }
    } else {
        printf("model   : none (%s)\r\n", sensor_model_why());
    }
    printf("host    : stream %s, live %s\r\n", s_streaming ? "on" : "off", s_live ? "on" : "off");
}

int sensor_app_init(void)
{
    uint8_t pin[1] = { BOARD_KEY_SW4_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_KEY_SW4_PIN, GPIO_INPUT_PP_MODE);
    s_q = xQueueCreate(8, sizeof(req_t));
    s_ring = pvPortMalloc(RING_BYTES);
    if (!s_q || !s_ring) {
        IPRO_LOGE(LOG_TAG, "out of memory");
        return -1;
    }
    src_init();
    sensor_model_init();
    const sensor_cfg_t *m = sensor_model_cfg();
    const int id = m ? src_find(m->source) : SRC_SIM_VIB;
    if (open_source(id < 0 ? SRC_SIM_VIB : (src_id_t)id) != 0) {
        IPRO_LOGE(LOG_TAG, "source would not open");
    }
    if (!m) {
        rgb_led_set(0, 0, 10);
    }
    if (xTaskCreate(sensor_task, "sensor", TASK_STACK, NULL, TASK_PRIO, NULL) != pdPASS) {
        return -1;
    }
    return 0;
}
