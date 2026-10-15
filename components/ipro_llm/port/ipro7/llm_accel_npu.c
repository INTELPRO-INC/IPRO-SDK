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
 * @file llm_accel_npu.c
 * @brief ipro_llm accelerator on the IPRO7 NPU (see ipro_llm_npu.h).
 *
 * An .inm of an int8 1x1 convolution is a size table and eight sections; for this layer only
 * cpu.inst, npu.inst (the templates, three value fields patched), npu.bias (zero), npu.wei and
 * npu.ms are non-empty. tools/ipro_llm/tests/test_npu_inm.py checks the same packing byte for
 * byte against IPRO AI Studio's converter.
 */
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "drv_glb.h"            /* GLB_PER_Clock_UnGate */
#include "hal_mtimer.h"
#include "hal_npu.h"            /* NPU_Img_Ext_Addr_Cfg, imgBuffer */
#include "iproai_core.h"
#include "iproai_def.h"         /* IPROAI_Model_t: share_buffer, the loaded sections */
#include "iproai_inst_util.h"   /* BALI_CHN_ALIGN */
#include <system_soc.h>         /* L1C_DCACHE_* */

#include "ipro_llm_npu.h"
#include "llm_npu_templates.h"
#include "../../src/llm_internal.h"   /* LLM_BATCH */

#define NPU_SLOTS       16
#define SIZE_TABLE      32u
#define MS_GROUP        8
#define MS_GROUP_BYTES  40u
#define CACHE_LINE      64u
#define FEED_HEAD       4u      /* the runtime cleans imgBuffer + 4 (ext_input) */
#define NPU_TOKENS      32      /* a prompt chunk's template: the engine's LLM_BATCH */

_Static_assert(NPU_TOKENS == LLM_BATCH, "the templates are made for LLM_BATCH tokens");

enum { CHUNK = 0, ONE = 1 };    /* per projection: the 32-token model, the one-token model */

typedef struct {
    iproai_model_hdl_t        hdl[2];
    const llm_npu_template_t *tmpl[2];
} npu_slot_t;

static npu_slot_t s_slot[NPU_SLOTS];
static uint8_t *s_act;          /* activation memory shared by every model */
static uint8_t *s_feed;         /* FEED_HEAD + tokens * cin, input in the unsigned domain */
static uint32_t s_act_bytes, s_feed_bytes;
static ipro_llm_npu_stats_t s_stats;

static uint32_t now_us(void)
{
    return (uint32_t)mtimer_get_time_us();
}

static uint32_t model_bytes(int cin, int cout)
{
    return LLM_NPU_INST_BYTES * 2u + 4u * (uint32_t)cout + (uint32_t)cin * (uint32_t)cout
           + (uint32_t)cout / MS_GROUP * MS_GROUP_BYTES + sizeof(IPROAI_Model_t);
}

static void free_slot(int slot)
{
    npu_slot_t *s = &s_slot[slot];
    if (s->hdl[ONE]) {
        IPROAI_Model_t *one = (IPROAI_Model_t *)s->hdl[ONE];
        one->buffer = NULL;                         /* the shared activation is ours */
        one->inst.npu.weights_buf = NULL;           /* the chunk model's */
        iproai_free(s->hdl[ONE]);
        s_stats.heap_bytes -= model_bytes(s->tmpl[ONE]->cin, s->tmpl[ONE]->cout)
                              - (uint32_t)s->tmpl[ONE]->cin * s->tmpl[ONE]->cout;
    }
    if (s->hdl[CHUNK]) {
        ((IPROAI_Model_t *)s->hdl[CHUNK])->buffer = NULL;
        iproai_free(s->hdl[CHUNK]);
        s_stats.models--;
        s_stats.heap_bytes -= model_bytes(s->tmpl[CHUNK]->cin, s->tmpl[CHUNK]->cout);
    }
    memset(s, 0, sizeof *s);
}

static void *alloc_lines(uint32_t bytes)
{
    uint8_t *p = pvPortMalloc(bytes + CACHE_LINE);
    return p ? (void *)(((uintptr_t)p + CACHE_LINE - 1u) & ~(uintptr_t)(CACHE_LINE - 1u)) : NULL;
}

static const llm_npu_template_t *find_template(int cin, int cout, int tokens)
{
    for (int i = 0; i < LLM_NPU_TEMPLATES; i++) {
        const llm_npu_template_t *t = &k_llm_npu_templates[i];
        if (t->cin == cin && t->cout == cout && t->tokens == tokens) {
            return t;
        }
    }
    return NULL;
}

/* value into bits [lo, lo + width) of a little-endian byte string */
static void set_bits(uint8_t *word, unsigned lo, unsigned width, uint32_t value)
{
    for (unsigned i = 0; i < width; i++) {
        const unsigned bit = lo + i;
        const uint8_t m = (uint8_t)(1u << (bit & 7u));
        word[bit >> 3] = (uint8_t)((value >> i) & 1u ? word[bit >> 3] | m : word[bit >> 3] & ~m);
    }
}

static void wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v, p[1] = (uint8_t)(v >> 8), p[2] = (uint8_t)(v >> 16), p[3] = (uint8_t)(v >> 24);
}

/* The whole .inm of one projection; the caller frees it once the runtime has copied it. */
static uint8_t *build_inm(const llm_npu_template_t *t, int cin, int cout, const int8_t *w,
                          const int32_t *mult, const int8_t *shift, float s_out)
{
    const uint32_t n_bias = 4u * (uint32_t)cout, n_wei = (uint32_t)cin * (uint32_t)cout;
    const uint32_t n_ms = (uint32_t)cout / MS_GROUP * MS_GROUP_BYTES;
    const uint32_t sizes[8] = { LLM_NPU_INST_BYTES, 0, 0, 0, LLM_NPU_INST_BYTES, n_bias, n_wei, n_ms };
    uint32_t total = SIZE_TABLE;
    for (int i = 0; i < 8; i++) {
        total += sizes[i];
    }
    uint8_t *b = pvPortMalloc(total);
    if (!b) {
        return NULL;
    }
    for (int i = 0; i < 8; i++) {
        wr32(b + 4 * i, sizes[i]);
    }
    uint8_t *p = b + SIZE_TABLE;
    uint32_t bits;
    memcpy(&bits, &s_out, sizeof bits);
    memcpy(p, t->cpu_inst, LLM_NPU_INST_BYTES);
    set_bits(p, LLM_NPU_CPU_OUT_SCALE_BIT, 32, bits);
    p += LLM_NPU_INST_BYTES;
    memcpy(p, t->npu_inst, LLM_NPU_INST_BYTES);                 /* the converter leaves the last */
    set_bits(p, LLM_NPU_OUT_MULT_BIT, 32, (uint32_t)mult[cout - 1]);   /* channel's here */
    set_bits(p, LLM_NPU_OUT_SHIFT_BIT, 6, (uint32_t)shift[cout - 1] & 0x3Fu);
    p += LLM_NPU_INST_BYTES;
    memset(p, 0, n_bias);
    p += n_bias;
    for (int ob = 0; ob < cout / 4; ob++) {                      /* 4 x 4 channel blocks, +128 */
        for (int ib = 0; ib < cin / 4; ib++) {
            for (int u = 0; u < 4; u++) {
                const int8_t *src = w + (size_t)(ob * 4 + u) * (size_t)cin + (size_t)ib * 4u;
                for (int v = 0; v < 4; v++) {
                    *p++ = (uint8_t)(src[v] + 128);
                }
            }
        }
    }
    for (int g = 0; g < cout; g += MS_GROUP) {                   /* 8 x 6-bit shifts, pad, 8 x mult */
        uint64_t packed = 0;
        for (int i = 0; i < MS_GROUP; i++) {
            packed |= (uint64_t)((uint32_t)shift[g + i] & 0x3Fu) << (6 * i);
        }
        for (int i = 0; i < 6; i++) {
            p[i] = (uint8_t)(packed >> (8 * i));
        }
        p[6] = p[7] = 0;
        for (int i = 0; i < MS_GROUP; i++) {
            wr32(p + 8 + 4 * i, (uint32_t)mult[g + i]);
        }
        p += MS_GROUP_BYTES;
    }
    return b;
}

static iproai_model_hdl_t load_model(const llm_npu_template_t *t, int cin, int cout, const int8_t *w,
                                     const int32_t *mult, const int8_t *shift, float s_out)
{
    if (t->act_bytes > s_act_bytes || FEED_HEAD + (uint32_t)t->tokens * (uint32_t)cin > s_feed_bytes) {
        return NULL;
    }
    uint8_t *inm = build_inm(t, cin, cout, w, mult, shift, s_out);
    iproai_model_hdl_t hdl = inm ? iproai_create() : NULL;
    if (!hdl) {
        vPortFree(inm);
        return NULL;
    }
    IPROAI_Model_t *model = (IPROAI_Model_t *)hdl;
    model->share_buffer = 1;            /* every projection runs on the same activation memory */
    model->buffer = s_act;
    imgBuffer = (char *)s_feed;
    const IPROAI_Status_e st = iproai_load_model_from_buffer(hdl, inm);
    vPortFree(inm);                     /* the runtime copied every section */
    if (st != IPROAI_STATUS_NO_ERROR) {
        model->buffer = NULL;           /* not ours to free */
        iproai_free(hdl);
        return NULL;
    }
    return hdl;
}

/* The 32-token model, and the one-token model for decode steps when its template exists; npu.wei
 * does not depend on the tokens, so the one-token model uses the chunk model's weights. */
static int npu_load(void *ctx, int slot, int cin, int cout, const int8_t *w, const int32_t *mult,
                    const int8_t *shift, float s_out)
{
    (void)ctx;
    const llm_npu_template_t *tc = find_template(cin, cout, NPU_TOKENS), *t1 = find_template(cin, cout, 1);
    if (slot < 0 || slot >= NPU_SLOTS || !tc || cin % 4 || cout % MS_GROUP) {
        return -1;
    }
    const uint32_t t0 = now_us();
    iproai_model_hdl_t hc = load_model(tc, cin, cout, w, mult, shift, s_out);
    iproai_model_hdl_t h1 = hc && t1 ? load_model(t1, cin, cout, w, mult, shift, s_out) : NULL;
    if (!hc) {
        return -1;
    }
    free_slot(slot);
    s_slot[slot].hdl[CHUNK] = hc;
    s_slot[slot].tmpl[CHUNK] = tc;
    s_stats.models++;
    s_stats.heap_bytes += model_bytes(cin, cout);
    if (h1) {
        IPROAI_Model_t *one = (IPROAI_Model_t *)h1;
        IPROAI_FREE(one->inst.npu.weights_buf);     /* the runtime's allocator (aligned, header before) */
        one->inst.npu.weights_buf = ((IPROAI_Model_t *)hc)->inst.npu.weights_buf;
        s_slot[slot].hdl[ONE] = h1;
        s_slot[slot].tmpl[ONE] = t1;
        s_stats.heap_bytes += model_bytes(cin, cout) - (uint32_t)cin * (uint32_t)cout;
    }
    s_stats.load_us += now_us() - t0;
    return 0;
}

/* x -> the feed (unsigned domain, padded to the template's tokens), run, outputs back to int8.
 * One token runs on the one-token model when there is one. */
static const uint8_t *npu_infer(int slot, const int8_t *x, int n, uint32_t *pitch)
{
    const npu_slot_t *ps = &s_slot[slot];
    const int k = n == 1 && ps->hdl[ONE] ? ONE : CHUNK;
    iproai_model_hdl_t hdl = ps->hdl[k];
    const llm_npu_template_t *tmpl = ps->tmpl[k];
    const int cin = tmpl->cin, tokens = tmpl->tokens;
    uint8_t *in = s_feed + FEED_HEAD;
    const size_t used = (size_t)n * (size_t)cin, all = (size_t)tokens * (size_t)cin;
    for (size_t i = 0; i < used; i++) {
        in[i] = (uint8_t)x[i] ^ 0x80u;
    }
    memset(in + used, 0x80, all - used);
    L1C_DCACHE_CLEAN_INVALID_RANGE((uintptr_t)in, (uint32_t)all);
    /* The runtime writes a model's buffer addresses into the NPU only when it loads it
     * (iproai_npu_initCfg), so with several models loaded every run would use the last one's
     * weights: point the NPU at this model's sections first. */
    const IPROAI_Model_t *md = (const IPROAI_Model_t *)hdl;
    imgBuffer = (char *)s_feed;         /* a global of the AI runtime: another user may have moved it */
    hal_npu_layer_setup(md->inst.npu.inst_buf, md->inst.npu.weights_buf, md->inst.npu.bias_buf,
                        md->inst.npu.mult_shift_buf);
    hal_npu_set_input_buffer(md->buffer, md->net->patch_size);
    hal_npu_set_net_param((uint8_t)md->net->unsign_input, (uint8_t)md->net->relu_n,
                          (uint8_t)md->net->use_tflite, (uint32_t)md->net->mode);
    NPU_Img_Ext_Addr_Cfg((uint32_t)(uintptr_t)in);
    if (iproai_startCompute(hdl) != IPROAI_STATUS_NO_ERROR) {
        return NULL;
    }
    uint32_t patch = 0;
    const uint8_t *out = iproai_getOutputBuffer(hdl, &patch);
    *pitch = BALI_CHN_ALIGN((uint32_t)tmpl->cout);
    if (out) {
        L1C_DCACHE_INVALID_RANGE((uintptr_t)out, (uint32_t)n * *pitch);
    }
    return out;
}

static int npu_run(void *ctx, int slot, const int8_t *x, int n, int8_t *y)
{
    (void)ctx;
    if (slot < 0 || slot >= NPU_SLOTS || !s_slot[slot].hdl[CHUNK] || n < 1 || n > NPU_TOKENS) {
        return -1;
    }
    uint32_t pitch;
    const uint8_t *out = npu_infer(slot, x, n, &pitch);
    if (!out) {
        return -1;
    }
    const int cout = s_slot[slot].tmpl[CHUNK]->cout;
    for (int t = 0; t < n; t++) {
        const uint8_t *o = out + (size_t)t * pitch;
        int8_t *yt = y + (size_t)t * (size_t)cout;
        for (int c = 0; c < cout; c++) {
            yt[c] = (int8_t)(o[c] ^ 0x80u);
        }
    }
    return 0;
}

int ipro_llm_npu_init(ipro_llm_accel_t *a)
{
    if (!a) {
        return -1;
    }
    if (!s_act || !s_feed) {
        uint32_t act = 0, feed = 0;
        for (int i = 0; i < LLM_NPU_TEMPLATES; i++) {
            const llm_npu_template_t *t = &k_llm_npu_templates[i];
            act = t->act_bytes > act ? t->act_bytes : act;
            const uint32_t f = FEED_HEAD + (uint32_t)t->tokens * t->cin;
            feed = f > feed ? f : feed;
        }
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);     /* the NPU sits behind the MM clock gate */
        s_act = s_act ? s_act : alloc_lines(act);
        s_feed = s_feed ? s_feed : alloc_lines(feed);
        if (!s_act || !s_feed) {
            return -1;
        }
        s_act_bytes = act;
        s_feed_bytes = feed;
        s_stats.heap_bytes = act + feed + 2u * CACHE_LINE;
    }
    a->load = npu_load;
    a->run = npu_run;
    a->ctx = NULL;
    a->max_tokens = NPU_TOKENS;
    a->min_tokens = 2;
    for (int i = 0; i < LLM_NPU_TEMPLATES; i++) {
        a->min_tokens = k_llm_npu_templates[i].tokens == 1 ? 1 : a->min_tokens;   /* decode steps too */
    }
    return 0;
}

void ipro_llm_npu_release(void)
{
    for (int slot = 0; slot < NPU_SLOTS; slot++) {
        free_slot(slot);
    }
    s_stats.load_us = 0;
}

const ipro_llm_npu_stats_t *ipro_llm_npu_stats(void)
{
    return &s_stats;
}

/* ------------------------------------------------------------------ check */

int ipro_llm_npu_check(int slot, int n, int *mismatches, int *max_diff)
{
    if (slot < 0 || slot >= NPU_SLOTS || !s_slot[slot].hdl[CHUNK] || n < 1 || n > NPU_TOKENS
        || (n == 1 && !s_slot[slot].hdl[ONE]) || !mismatches || !max_diff) {
        return -1;
    }
    const npu_slot_t *s = &s_slot[slot];
    const int k = n == 1 ? ONE : CHUNK;         /* the model npu_infer runs n tokens on */
    const int cin = s->tmpl[k]->cin, cout = s->tmpl[k]->cout;
    int8_t *x = pvPortMalloc((size_t)n * (size_t)cin);
    if (!x) {
        return -1;
    }
    uint32_t r = 0x12345678u + (uint32_t)slot;
    for (int i = 0; i < n * cin; i++) {
        r = r * 1664525u + 1013904223u;
        x[i] = (int8_t)(r >> 24);
    }
    uint32_t pitch;
    const uint8_t *out = npu_infer(slot, x, n, &pitch);
    if (!out) {
        vPortFree(x);
        return -1;
    }
    const IPROAI_Model_t *md = (const IPROAI_Model_t *)s->hdl[k];
    const uint8_t *wb = md->inst.npu.weights_buf, *ms = md->inst.npu.mult_shift_buf;
    const uint8_t *bias = md->inst.npu.bias_buf;
    *mismatches = 0;
    *max_diff = 0;
    for (int c = 0; c < cout; c++) {
        const uint8_t *g = ms + (size_t)(c / MS_GROUP) * MS_GROUP_BYTES;
        uint64_t packed = 0;
        for (int i = 0; i < 6; i++) {
            packed |= (uint64_t)g[i] << (8 * i);
        }
        int sh = (int)((packed >> (6 * (c % MS_GROUP))) & 0x3Fu);
        sh = sh >= 32 ? sh - 64 : sh;
        int32_t mult, b;
        memcpy(&mult, g + 8 + 4 * (c % MS_GROUP), sizeof mult);
        memcpy(&b, bias + 4 * c, sizeof b);
        for (int t = 0; t < n; t++) {
            int32_t acc = b;
            for (int i = 0; i < cin; i++) {
                const uint8_t wv = wb[((size_t)(c / 4) * (size_t)(cin / 4) + (size_t)(i / 4)) * 16u
                                      + (size_t)(c % 4) * 4u + (size_t)(i % 4)];
                acc += (int32_t)x[(size_t)t * (size_t)cin + (size_t)i] * ((int32_t)wv - 128);
            }
            const int want = ipro_llm_requant(acc, mult, sh);
            const int got = (int8_t)(out[(size_t)t * pitch + (size_t)c] ^ 0x80u);
            const int d = got > want ? got - want : want - got;
            *mismatches += d != 0;
            *max_diff = d > *max_diff ? d : *max_diff;
        }
    }
    vPortFree(x);
    return 0;
}
