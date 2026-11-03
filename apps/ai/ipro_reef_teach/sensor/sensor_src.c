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
 * @file sensor_src.c
 * @brief Sample sources. See sensor_src.h.
 *
 * Each source is an entry of the platform's source registry
 * (components/ai/source, ipro_ai_source.h): the five simulators and the PDM
 * microphone register at src_init(), and src_open/src_read/src_close go
 * through the registry's lifecycle, which also counts frames, losses and
 * stalls. The src_* API the rest of the app uses is unchanged.
 */
#include "sensor_src.h"

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "board_config.h"
#include "audio_pdm_mic.h"
#include "hal_mtimer.h"
#include "ipro_ai_source.h"

#include "sensor_sim.h"

/* ---------------------------------------------------------------- table */

/* sim_* entries are filled from sensor_sim.c with their current parameters */
static src_info_t s_info[SRC_COUNT] = {
    [SRC_MIC] = { "mic", 16000u, 1u, 32768.0f, "fs" },
};
static sim_params_t s_params[SIM_TYPES];   /* per simulated type, kept across switches */
static src_id_t     s_cur = SRC_SIM_VIB;
static bool         s_open;
static ipro_ai_source_t *s_reg[SRC_COUNT];  /* the registry's entries, by id */

static void sim_entry(src_id_t id)
{
    const sim_type_info_t *t = sim_type_info((sim_type_t)id);
    const sim_params_t *p = &s_params[id];
    s_info[id].name = t->name;
    s_info[id].rate = p->rate ? p->rate : t->rate;
    s_info[id].channels = p->channels ? p->channels : t->channels;
    s_info[id].lsb_per_unit = t->lsb_per_unit;
    s_info[id].unit = t->unit;
}

static void register_all(void);

void src_init(void)
{
    for (int i = 0; i < SIM_TYPES; i++) {
        s_params[i] = sim_default_params();
        sim_entry((src_id_t)i);
    }
    register_all();
}

const src_info_t *src_info(src_id_t id)
{
    return id < SRC_COUNT ? &s_info[id] : NULL;
}

int src_find(const char *name)
{
    for (int i = 0; i < SRC_COUNT; i++) {
        if (s_info[i].name && !strcmp(name, s_info[i].name)) {
            return i;
        }
    }
    return -1;
}

bool src_is_open(void)
{
    return s_open;
}

src_id_t src_current(void)
{
    return s_cur;
}

bool src_is_sim(src_id_t id)
{
    return id < (src_id_t)SIM_TYPES;
}

/* ---------------------------------------------------------------- sim */

static uint64_t s_t0_us, s_made;

static int sim_open(void *ctx, const ipro_ai_source_cfg_t *cfg)
{
    const src_id_t id = (src_id_t)(uintptr_t)ctx;

    (void)cfg;                                   /* s_params[id] already holds rate and channels */
    sim_entry(id);
    s_t0_us = mtimer_get_time_us();
    s_made = 0;
    sim_start((sim_type_t)id, &s_params[id], (uint32_t)s_t0_us);
    return 0;
}

static int32_t sim_read(void *ctx, void *dst, uint32_t max_frames, uint32_t timeout_ms,
                        uint32_t *lost, uint64_t *t_us)
{
    const uint32_t rate = s_info[(src_id_t)(uintptr_t)ctx].rate;
    const uint64_t due = (mtimer_get_time_us() - s_t0_us) * rate / 1000000u;
    uint32_t n = due > s_made ? (uint32_t)(due - s_made) : 0u;

    (void)timeout_ms;
    (void)t_us;
    *lost = 0;
    if (n == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        return 0;
    }
    if (n > max_frames) {
        n = max_frames;
    }
    sim_generate(dst, n);
    s_made += n;
    return (int32_t)n;
}

static void sim_close(void *ctx)
{
    (void)ctx;
}

static void sim_caps(void *ctx, ipro_ai_source_caps_t *caps)
{
    const sim_type_info_t *t = sim_type_info((sim_type_t)(uintptr_t)ctx);

    caps->kind = IPRO_AI_SRC_SAMPLES;
    caps->caps = IPRO_AI_SRC_CAP_SIMULATED | IPRO_AI_SRC_CAP_RATE | IPRO_AI_SRC_CAP_CHANNELS;
    caps->min_rate = t->min_rate;
    caps->max_rate = t->max_rate;
    caps->max_channels = t->channels;
}

static int src_format(void *ctx, ipro_ai_source_format_t *fmt)
{
    const src_info_t *in = &s_info[(src_id_t)(uintptr_t)ctx];

    fmt->rate = in->rate;
    fmt->channels = in->channels;
    fmt->lsb_per_unit = in->lsb_per_unit;
    fmt->unit = in->unit;
    return 0;
}

static const ipro_ai_source_ops_t s_sim_ops = {
    .open = sim_open, .read = sim_read, .close = sim_close, .caps = sim_caps, .format = src_format,
};

int src_sim_set(const char *state)
{
    return src_is_sim(s_cur) ? sim_set_state(state) : -1;
}

void src_sim_next(void)
{
    if (src_is_sim(s_cur)) {
        sim_next_state();
    }
}

const char *src_sim_state(void)
{
    return src_is_sim(s_cur) ? sim_state() : "-";
}

const sim_params_t *src_sim_params(void)
{
    return src_is_sim(s_cur) ? &s_params[s_cur] : NULL;
}

int src_sim_configure(const sim_params_t *p, bool *reopen)
{
    if (!src_is_sim(s_cur)) {
        return -1;
    }
    const sim_type_info_t *t = sim_type_info((sim_type_t)s_cur);
    sim_params_t q = *p;
    q.rate = q.rate < t->min_rate ? t->min_rate : q.rate > t->max_rate ? t->max_rate : q.rate;
    q.channels = q.channels < 1u ? 1u : q.channels > t->channels ? t->channels : q.channels;
    *reopen = q.rate != s_info[s_cur].rate || q.channels != s_info[s_cur].channels;
    s_params[s_cur] = q;
    sim_entry(s_cur);
    if (!*reopen && s_open) {
        /* noise, drift, severity and ramp change in place, the state kept */
        const char *keep = sim_state();
        char name[24];
        strncpy(name, keep, sizeof(name) - 1);
        name[sizeof(name) - 1] = 0;
        sim_start((sim_type_t)s_cur, &s_params[s_cur], (uint32_t)mtimer_get_time_us());
        sim_set_state(name);
    }
    return 0;
}

/* ---------------------------------------------------------------- mic */

/* components/audio/audio_pdm_mic.h: 16 kHz, 100 Hz high-pass, +38 dB in all */
#define MIC_BUF_BYTES (16000u * 2u * 2u)          /* 2 s */

static int mic_open(void *ctx, const ipro_ai_source_cfg_t *cfg)
{
    const audio_pdm_mic_cfg_t mic = {
        .clk_pin   = BOARD_MIC_CLK_PIN,
        .data_pin  = BOARD_MIC_DAT_PIN,
        .channel   = BOARD_MIC_PDM_CHANNEL,
        .buf_bytes = MIC_BUF_BYTES,
    };

    (void)ctx;
    (void)cfg;
    return audio_pdm_mic_open(&mic) == 0 ? 0 : IPRO_AI_E_RUN;
}

static int32_t mic_read(void *ctx, void *dst, uint32_t max_frames, uint32_t timeout_ms,
                        uint32_t *lost, uint64_t *t_us)
{
    (void)ctx;
    (void)t_us;
    *lost = 0;
    return (int32_t)audio_pdm_mic_read(dst, max_frames, timeout_ms);
}

static void mic_close(void *ctx)
{
    (void)ctx;
    audio_pdm_mic_close();
}

static void mic_caps(void *ctx, ipro_ai_source_caps_t *caps)
{
    (void)ctx;
    caps->kind = IPRO_AI_SRC_SAMPLES;
    caps->caps = 0;
    caps->min_rate = caps->max_rate = AUDIO_PDM_MIC_RATE;
    caps->max_channels = 1;
}

static const ipro_ai_source_ops_t s_mic_ops = {
    .open = mic_open, .read = mic_read, .close = mic_close, .caps = mic_caps, .format = src_format,
};

/* ---------------------------------------------------------------- dispatch */

static void register_all(void)
{
    for (int i = 0; i < SRC_COUNT; i++) {
        if (!s_reg[i]) {
            ipro_ai_source_register(s_info[i].name, src_is_sim((src_id_t)i) ? &s_sim_ops : &s_mic_ops,
                                    (void *)(uintptr_t)i);
            s_reg[i] = ipro_ai_source_find(s_info[i].name);
        }
    }
}

int src_open(src_id_t id)
{
    if (id >= SRC_COUNT || !s_reg[id]) {
        return -1;
    }
    char keep[24] = "";
    if (src_is_sim(id) && id == s_cur && s_open) {
        strncpy(keep, sim_state(), sizeof(keep) - 1);      /* a reopen keeps the state */
    }
    src_close();
    const ipro_ai_source_cfg_t cfg = { s_info[id].rate, s_info[id].channels, NULL };
    if (ipro_ai_source_open(s_reg[id], &cfg) != 0) {
        return -1;
    }
    if (ipro_ai_source_start(s_reg[id]) != 0) {
        ipro_ai_source_close(s_reg[id]);
        return -1;
    }
    if (keep[0]) {
        sim_set_state(keep);
    }
    s_cur = id;
    s_open = true;
    return 0;
}

void src_close(void)
{
    if (!s_open) {
        return;
    }
    ipro_ai_source_close(s_reg[s_cur]);
    s_open = false;
}

uint32_t src_read(int16_t *dst, uint32_t max_frames)
{
    if (!s_open || max_frames == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
        return 0;
    }
    const int32_t n = ipro_ai_source_read(s_reg[s_cur], dst, max_frames, 10, NULL);
    return n > 0 ? (uint32_t)n : 0u;
}

int src_stats(ipro_ai_source_stats_t *st)
{
    return s_open ? ipro_ai_source_get_stats(s_reg[s_cur], st) : -1;
}
