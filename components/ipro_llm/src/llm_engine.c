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
 * @file llm_engine.c
 * @brief Blob checks, work-memory layout and the constrained greedy decoder.
 */
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "llm_internal.h"

#define MAGIC        "IPLM"
#define SEC_TDIR     "TDIR"
#define SEC_TDAT     "TDAT"
#define SEC_TOKN     "TOKN"
#define SEC_GRAM     "GRAM"
#define SEC_NPUS     "NPUS"     /* optional: static scales for the NPU prefill */
#define TDIR_ENTRY   32u
#define MAX_ALLOWED  256

typedef struct {
    const uint8_t *p;
    uint32_t       len;
} span_t;

typedef struct {
    const uint8_t *blob;
    uint32_t       len;
    span_t         tdir, tdat, tokn, gram, npus;
    bool           has_npus;
} blob_t;

typedef struct {        /* bump allocator; base NULL = only count */
    uint8_t *base;
    size_t   used, cap;
} arena_t;

static void *take(arena_t *a, size_t n)
{
    a->used = (a->used + 15u) & ~(size_t)15u;
    void *p = a->base && a->used + n <= a->cap ? a->base + a->used : NULL;
    a->used += n;
    return p;
}

/* ------------------------------------------------------------ blob */

static int find_section(const uint8_t *b, uint32_t total, const char *tag, span_t *out)
{
    const uint16_t n = llm_rd16(b + 56);
    for (uint16_t i = 0; i < n; i++) {
        const uint8_t *e = b + LLM_HEADER_BYTES + 12u * i;
        if ((size_t)LLM_HEADER_BYTES + 12u * (i + 1u) > total) {
            return IPRO_LLM_E_BLOB;
        }
        if (memcmp(e, tag, 4) == 0) {
            const uint32_t off = llm_rd32(e + 4), size = llm_rd32(e + 8);
            if (off > total || size > total - off) {
                return IPRO_LLM_E_BLOB;
            }
            out->p = b + off;
            out->len = size;
            return IPRO_LLM_OK;
        }
    }
    return IPRO_LLM_E_BLOB;
}

static bool has_tag(const uint8_t *b, uint32_t total, const char *tag)
{
    const uint16_t n = llm_rd16(b + 56);
    for (uint16_t i = 0; i < n && (size_t)LLM_HEADER_BYTES + 12u * (i + 1u) <= total; i++) {
        if (memcmp(b + LLM_HEADER_BYTES + 12u * i, tag, 4) == 0) {
            return true;
        }
    }
    return false;
}

static int parse_header(const uint8_t *b, size_t len, llm_cfg_t *c, blob_t *bl, bool check_crc)
{
    if (!b || len < LLM_HEADER_BYTES || memcmp(b, MAGIC, 4) != 0
        || llm_rd16(b + 4) != LLM_VERSION || llm_rd16(b + 6) != LLM_HEADER_BYTES) {
        return IPRO_LLM_E_BLOB;
    }
    const uint32_t total = llm_rd32(b + 8);
    if (total > len || total < LLM_HEADER_BYTES) {
        return IPRO_LLM_E_BLOB;
    }
    if (check_crc && llm_crc32(b + LLM_HEADER_BYTES, total - LLM_HEADER_BYTES) != llm_rd32(b + 12)) {
        return IPRO_LLM_E_CRC;
    }
    c->vocab = llm_rd16(b + 16);
    c->d = llm_rd16(b + 18);
    c->layers = llm_rd16(b + 20);
    c->heads = llm_rd16(b + 22);
    c->kv_heads = llm_rd16(b + 24);
    c->head_dim = llm_rd16(b + 26);
    c->mlp_n = llm_rd16(b + 28);
    c->max_seq = llm_rd16(b + 30);
    c->engram_site = llm_rd16(b + 32);
    c->engram_slots = llm_rd16(b + 34);
    c->engram_taps = llm_rd16(b + 36);
    c->engram_dilation = llm_rd16(b + 38);
    c->orders[0] = llm_rd16(b + 42);
    c->orders[1] = llm_rd16(b + 44);
    uint32_t bits = llm_rd32(b + 48);
    memcpy(&c->rope_theta, &bits, 4);
    bits = llm_rd32(b + 52);
    memcpy(&c->eps, &bits, 4);
    if (llm_rd16(b + 40) != 2 || c->d == 0 || c->d > LLM_MAX_D || c->d % 2 || c->layers == 0
        || c->layers > LLM_MAX_LAYERS || c->kv_heads == 0 || c->heads % c->kv_heads
        || c->heads > LLM_MAX_HEADS || c->head_dim * c->heads != c->d || c->head_dim % 2
        || c->head_dim > LLM_MAX_HEAD_DIM || c->mlp_n < c->d || c->mlp_n > LLM_MAX_MLP
        || (c->mlp_n & (c->mlp_n - 1u)) || c->engram_site >= c->layers || c->engram_slots == 0
        || c->engram_taps == 0 || c->engram_dilation == 0 || c->orders[0] == 0 || c->orders[1] == 0
        || c->max_seq < LLM_CTX) {
        return IPRO_LLM_E_BLOB;
    }
    bl->blob = b;
    bl->len = total;
    if (find_section(b, total, SEC_TDIR, &bl->tdir) || find_section(b, total, SEC_TDAT, &bl->tdat)
        || find_section(b, total, SEC_TOKN, &bl->tokn) || find_section(b, total, SEC_GRAM, &bl->gram)) {
        return IPRO_LLM_E_BLOB;
    }
    bl->has_npus = find_section(b, total, SEC_NPUS, &bl->npus) == IPRO_LLM_OK;
    if (!bl->has_npus && has_tag(b, total, SEC_NPUS)) {
        return IPRO_LLM_E_BLOB;              /* there, but out of the blob: corrupt, not absent */
    }
    return IPRO_LLM_OK;
}

/* Look a tensor up by name and check its shape (rows x cols, last dim = cols). */
static int tensor(const blob_t *bl, const char *name, uint32_t rows, uint32_t cols, bool cq,
                  llm_tensor_t *t)
{
    const uint32_t h = llm_fnv1a(name);
    for (uint32_t off = 0; off + TDIR_ENTRY <= bl->tdir.len; off += TDIR_ENTRY) {
        const uint8_t *e = bl->tdir.p + off;
        if (llm_rd32(e) != h) {
            continue;
        }
        const uint8_t kind = e[4], ndim = e[5];
        const uint32_t d0 = llm_rd32(e + 8), d1 = llm_rd32(e + 12), d2 = llm_rd32(e + 16);
        uint32_t r = 1, c = 1;
        if (ndim == 1) {
            c = d0;
        } else if (ndim == 2) {
            r = d0, c = d1;
        } else if (ndim == 3) {
            r = d0 * d1, c = d2;
        } else if (ndim != 0) {
            return IPRO_LLM_E_BLOB;
        }
        if (r != rows || c != cols || (cq != (kind != LLM_F16)) || kind > LLM_CQ4) {
            return IPRO_LLM_E_BLOB;
        }
        t->kind = kind;
        t->group = llm_rd16(e + 6);
        t->rows = r;
        t->cols = c;
        const uint32_t data_off = llm_rd32(e + 20), norm_off = llm_rd32(e + 24);
        size_t data_len = (size_t)r * c * 2u, norm_len = 0;
        if (cq) {
            if (t->group == 0 || c % t->group || (t->group & (t->group - 1u))) {
                return IPRO_LLM_E_BLOB;
            }
            data_len = (size_t)r * c * (kind == LLM_CQ2 ? 2u : 4u) / 8u;
            norm_len = (size_t)r * (c / t->group) * 2u;
        }
        if (data_off > bl->tdat.len || data_len > bl->tdat.len - data_off
            || norm_off > bl->tdat.len || norm_len > bl->tdat.len - norm_off) {
            return IPRO_LLM_E_BLOB;
        }
        t->data = bl->tdat.p + data_off;
        t->norm = cq ? bl->tdat.p + norm_off : NULL;
        return IPRO_LLM_OK;
    }
    return IPRO_LLM_E_BLOB;
}

static int layer_tensors(const blob_t *bl, const llm_cfg_t *c, int L, llm_layer_w_t *w)
{
    char n[64];
    const uint32_t d = c->d, kv = (uint32_t)c->kv_heads * c->head_dim;
#define T(field, fmt, rows, cols, cq)                                            \
    do {                                                                         \
        snprintf(n, sizeof n, fmt, L);                                           \
        if (tensor(bl, n, rows, cols, cq, &w->field)) {                          \
            return IPRO_LLM_E_BLOB;                                              \
        }                                                                        \
    } while (0)
    T(attn_norm, "blocks.%d.attn_norm.scale", 1, d, false);
    T(q, "blocks.%d.attn.q_proj.weight", d, d, true);
    T(k, "blocks.%d.attn.k_proj.weight", kv, d, true);
    T(v, "blocks.%d.attn.v_proj.weight", kv, d, true);
    T(gate, "blocks.%d.attn.gate_proj.weight", d, d, true);
    T(out, "blocks.%d.attn.out_proj.weight", d, d, true);
    T(q_norm, "blocks.%d.attn.q_norm.scale", 1, c->head_dim, false);
    T(k_norm, "blocks.%d.attn.k_norm.scale", 1, c->head_dim, false);
    T(post_attn_norm, "blocks.%d.post_attn_norm.scale", 1, d, false);
    T(attn_gate, "blocks.%d.attn_gate", 1, 1, false);
    T(mlp_norm, "blocks.%d.mlp_norm.scale", 1, d, false);
    T(d1, "blocks.%d.mlp.d1", 1, c->mlp_n, false);
    T(d2, "blocks.%d.mlp.d2", 1, c->mlp_n, false);
    T(d3, "blocks.%d.mlp.d3", 1, c->mlp_n, false);
#undef T
    /* one rotation of the input serves all four input projections */
    if (w->k.group != w->q.group || w->v.group != w->q.group || w->gate.group != w->q.group) {
        return IPRO_LLM_E_BLOB;
    }
    return IPRO_LLM_OK;
}

/* ------------------------------------------------------- tokenizer, grammar */

static int parse_tokenizer(const span_t *s, const llm_cfg_t *c, llm_tok_t *t, uint32_t *n_normal)
{
    if (s->len < 8) {
        return IPRO_LLM_E_BLOB;
    }
    t->n_pieces = llm_rd16(s->p);
    t->byte_base = llm_rd16(s->p + 2);
    t->max_piece_bytes = llm_rd16(s->p + 4);
    const uint32_t text_len = llm_rd16(s->p + 6), n = t->n_pieces;
    const size_t need = 8u + 2u * (n + 1u) + 2u * n + text_len;
    if (n != c->vocab || need > s->len || (uint32_t)t->byte_base + 256u > n) {
        return IPRO_LLM_E_BLOB;
    }
    t->offsets = s->p + 8;
    t->rank = t->offsets + 2u * (n + 1u);
    t->text = (const char *)(t->rank + 2u * n);
    if (llm_rd16(t->offsets + 2u * n) != text_len) {
        return IPRO_LLM_E_BLOB;
    }
    uint32_t normal = 0;
    for (uint32_t i = 0; i < n; i++) {
        if (llm_rd16(t->offsets + 2u * i) > llm_rd16(t->offsets + 2u * (i + 1u))) {
            return IPRO_LLM_E_BLOB;
        }
        normal += llm_rd16(t->rank + 2u * i) != LLM_NO_RANK;
    }
    *n_normal = normal;
    return IPRO_LLM_OK;
}

static uint32_t hash_slots(uint32_t n_normal)
{
    uint32_t s = 1;
    while (s < 2u * n_normal) {
        s <<= 1;
    }
    return s;
}

static int parse_grammar(const span_t *s, const llm_cfg_t *c, llm_grammar_t *g)
{
    const uint8_t *p = s->p, *end = s->p + s->len;
    if (s->len < 40 || end[-1] != '\0') {
        return IPRO_LLM_E_BLOB;
    }
    g->open = llm_rd16(p);
    g->close = llm_rd16(p + 2);
    g->im_end = llm_rd16(p + 4);
    g->bos = llm_rd16(p + 6);
    g->n_tools = llm_rd16(p + 8);
    g->n_args = llm_rd16(p + 10);
    g->n_values = llm_rd16(p + 12);
    g->n_prefix = llm_rd16(p + 14);
    g->n_suffix = llm_rd16(p + 16);
    const uint16_t user = llm_rd16(p + 18);
    for (int i = 0; i < 10; i++) {
        g->digit[i] = llm_rd16(p + 20 + 2 * i);
    }
    if (g->n_prefix == 0 || g->n_tools == 0 || g->n_tools > LLM_MAX_TOOLS || g->n_args > LLM_MAX_GRAM_ARGS
        || g->n_values > LLM_MAX_VALUES || g->n_prefix > LLM_MAX_FIX || g->n_suffix > LLM_MAX_FIX) {
        return IPRO_LLM_E_BLOB;
    }
    p += 40;
    const size_t fixed = 2u * (g->n_prefix + g->n_suffix) + 8u * g->n_tools + 14u * g->n_args
                         + 6u * g->n_values;
    if (fixed > (size_t)(end - p)) {
        return IPRO_LLM_E_BLOB;
    }
    for (int i = 0; i < g->n_prefix; i++, p += 2) {
        g->prefix[i] = llm_rd16(p);
    }
    for (int i = 0; i < g->n_suffix; i++, p += 2) {
        g->suffix[i] = llm_rd16(p);
    }
    for (int i = 0; i < g->n_tools; i++, p += 8) {
        llm_tool_t *t = &g->tools[i];
        t->token = llm_rd16(p);
        t->name = llm_rd16(p + 2);
        t->first_arg = llm_rd16(p + 4);
        t->n_args = p[6];
        if ((uint32_t)t->first_arg + t->n_args > g->n_args || t->n_args > 32) {
            return IPRO_LLM_E_BLOB;
        }
    }
    for (int i = 0; i < g->n_args; i++, p += 14) {
        llm_arg_t *a = &g->args[i];
        a->name = llm_rd16(p);
        a->kind = p[2];
        a->required = p[3];
        a->key_token = llm_rd16(p + 4);
        a->min = (int16_t)llm_rd16(p + 6);
        a->max = (int16_t)llm_rd16(p + 8);
        a->first_value = llm_rd16(p + 10);
        a->n_values = p[12];
        if (a->kind > LLM_ARG_BOOL || (uint32_t)a->first_value + a->n_values > g->n_values) {
            return IPRO_LLM_E_BLOB;
        }
    }
    for (int i = 0; i < g->n_values; i++, p += 6) {
        g->values[i].token = llm_rd16(p);
        g->values[i].name = llm_rd16(p + 2);
        g->values[i].value = (int16_t)llm_rd16(p + 4);
    }
    g->strings = (const char *)p;
    const size_t strings_len = (size_t)(end - p);
    if (user >= strings_len) {
        return IPRO_LLM_E_BLOB;
    }
    g->user_literal = g->strings + user;
    for (int i = 0; i < g->n_tools; i++) {
        llm_tool_t *t = &g->tools[i];
        t->n_required = 0;
        if (t->name >= strings_len || t->token >= c->vocab) {
            return IPRO_LLM_E_BLOB;
        }
        for (int a = 0; a < t->n_args; a++) {
            const llm_arg_t *ar = &g->args[t->first_arg + a];
            t->n_required += ar->required;
            if (ar->name >= strings_len) {
                return IPRO_LLM_E_BLOB;
            }
        }
    }
    for (int i = 0; i < g->n_values; i++) {
        if (g->values[i].name >= strings_len || g->values[i].token >= c->vocab) {
            return IPRO_LLM_E_BLOB;
        }
    }
    return IPRO_LLM_OK;
}

/* -------------------------------------------------------------- layout */

static void layout(ipro_llm_t *m, const llm_cfg_t *c, uint32_t n_normal, bool npus, arena_t *a)
{
    const size_t d = c->d, hd = c->head_dim, h2 = hd / 2;
    const size_t kv = (size_t)c->layers * c->kv_heads * LLM_CTX * hd;
    const uint16_t ring = (uint16_t)((c->engram_taps - 1u) * c->engram_dilation + 1u);
    for (int L = 0; L < c->layers; L++) {
        llm_layer_f_t *f = m ? &m->lf[L] : NULL;
        float *p = take(a, (4 * d + 2 * hd + 3 * (size_t)c->mlp_n) * sizeof(float));
        if (f && p) {
            f->attn_norm = p, p += d;
            f->post_attn_norm = p, p += d;
            f->mlp_norm = p, p += d;
            f->q_norm = p, p += hd;
            f->k_norm = p, p += hd;
            f->d1 = p, p += c->mlp_n;
            f->d2 = p, p += c->mlp_n;
            f->d3 = p;
        }
    }
    float *fn = take(a, d * sizeof(float));
    float *taps = take(a, (size_t)c->engram_taps * d * sizeof(float));
    float *rc = take(a, LLM_CTX * h2 * sizeof(float)), *rs = take(a, LLM_CTX * h2 * sizeof(float));
    float *kc = take(a, kv * sizeof(float)), *vc = take(a, kv * sizeof(float));
    int8_t *kq = take(a, kv), *vq = take(a, kv);
    const size_t rows = (size_t)c->layers * c->kv_heads * LLM_CTX;
    float *ks = take(a, rows * sizeof(float)), *vs = take(a, rows * sizeof(float));
    float *egv = take(a, (size_t)ring * d * sizeof(float));
    float *egp = take(a, (size_t)LLM_MAX_FIX * d * sizeof(float));
    float *batch = take(a, 8u * LLM_BATCH * LLM_MAX_D * sizeof(float));
    llm_act_t *bact = take(a, LLM_BATCH * sizeof(llm_act_t));
    uint16_t *hash = take(a, hash_slots(n_normal) * sizeof(uint16_t));
    char *buf = take(a, (size_t)LLM_MAX_TEXT * 3u);
    llm_sym_t *sym = take(a, LLM_MAX_TEXT * sizeof(llm_sym_t));
    uint16_t *pair = take(a, LLM_MAX_TEXT * sizeof(uint16_t));
    int8_t *ax = npus ? take(a, (size_t)LLM_BATCH * LLM_MAX_D) : NULL;
    int8_t *ay = npus ? take(a, (size_t)LLM_BATCH * LLM_ACCEL_OUT * LLM_MAX_D) : NULL;
    if (m) {
        m->ax = ax;
        m->ay = ay;
        m->final_norm = fn;
        m->eg_taps_f = taps;
        m->rope_cos = rc;
        m->rope_sin = rs;
        m->kcache = kc;
        m->vcache = vc;
        m->kq = kq;
        m->vq = vq;
        m->ks = ks;
        m->vs = vs;
        m->eg_v = egv;
        m->eg_ring = ring;
        m->eg_prefix = egp;
        m->bact = bact;
        if (batch) {
            float **bufs[] = { &m->bx, &m->bh, &m->bq, &m->bk, &m->bv, &m->bg, &m->by, &m->bo };
            for (size_t i = 0; i < sizeof bufs / sizeof bufs[0]; i++) {
                *bufs[i] = batch + i * LLM_BATCH * LLM_MAX_D;
            }
        }
        m->tok.hash = hash;
        m->tok.buf = buf;
        m->tok.sym = sym;
        m->tok.pair = pair;
    }
}

size_t ipro_llm_work_bytes(const void *blob, size_t blob_len)
{
    llm_cfg_t c;
    blob_t bl;
    llm_tok_t t;
    uint32_t n_normal;
    if (parse_header(blob, blob_len, &c, &bl, false) || parse_tokenizer(&bl.tokn, &c, &t, &n_normal)) {
        return 0;
    }
    arena_t a = { NULL, 0, 0 };
    (void)take(&a, sizeof(ipro_llm_t));
    layout(NULL, &c, n_normal, bl.has_npus, &a);
    return a.used + 16u;
}

static void convert(float *out, const llm_tensor_t *t)
{
    llm_f16_vec(out, t);
}

/* A ZC-RMSNorm scale as the gain llm_rmsnorm multiplies by: 1 + scale, once. */
static void convert_gain(float *out, const llm_tensor_t *t)
{
    llm_f16_vec(out, t);
    for (uint32_t i = 0; i < t->rows * t->cols; i++) {
        out[i] = 1.0f + out[i];
    }
}

int ipro_llm_open(ipro_llm_t **out, const void *blob, size_t blob_len, void *work, size_t work_len)
{
    if (!out || !blob || !work) {
        return IPRO_LLM_E_ARG;
    }
    *out = NULL;
    llm_cfg_t c;
    blob_t bl;
    int err = parse_header(blob, blob_len, &c, &bl, true);
    if (err) {
        return err;
    }
    const size_t need = ipro_llm_work_bytes(blob, blob_len);
    uint8_t *base = (uint8_t *)(((uintptr_t)work + 15u) & ~(uintptr_t)15u);
    if (!need || work_len < need) {
        return IPRO_LLM_E_NOMEM;
    }
    arena_t a = { base, 0, work_len - (size_t)(base - (uint8_t *)work) };
    ipro_llm_t *m = take(&a, sizeof *m);
    memset(m, 0, sizeof *m);
    m->cfg = c;
    m->int8 = true;
    llm_cq_init();
    llm_lut_init();
    uint32_t n_normal;
    if (parse_tokenizer(&bl.tokn, &c, &m->tok, &n_normal) || parse_grammar(&bl.gram, &c, &m->g)) {
        return IPRO_LLM_E_BLOB;
    }
    layout(m, &c, n_normal, bl.has_npus, &a);
    if (bl.has_npus && llm_accel_parse(m, bl.npus.p, bl.npus.len)) {
        return IPRO_LLM_E_BLOB;
    }

    const uint32_t d = c.d;
    if (tensor(&bl, "embed.weight", c.vocab, d, true, &m->embed)
        || tensor(&bl, "final_norm.scale", 1, d, false, &m->final_norm_t)) {
        return IPRO_LLM_E_BLOB;
    }
    char n[64];
    snprintf(n, sizeof n, "engrams.%u.tables", (unsigned)c.engram_site);
    err = tensor(&bl, n, 2u * c.engram_slots, d / 2u, true, &m->eg_tables);
    snprintf(n, sizeof n, "engrams.%u.key_proj.weight", (unsigned)c.engram_site);
    err = err ? err : tensor(&bl, n, d, d, true, &m->eg_key);
    snprintf(n, sizeof n, "engrams.%u.value_proj.weight", (unsigned)c.engram_site);
    err = err ? err : tensor(&bl, n, d, d, true, &m->eg_value);
    snprintf(n, sizeof n, "engrams.%u.taps", (unsigned)c.engram_site);
    err = err ? err : tensor(&bl, n, c.engram_taps, d, false, &m->eg_taps);
    if (err || m->eg_value.group != m->eg_key.group) {
        return IPRO_LLM_E_BLOB;
    }
    for (int L = 0; L < c.layers; L++) {
        if (layer_tensors(&bl, &c, L, &m->lw[L])) {
            return IPRO_LLM_E_BLOB;
        }
        const llm_layer_w_t *w = &m->lw[L];
        llm_layer_f_t *f = &m->lf[L];
        convert_gain(f->attn_norm, &w->attn_norm);
        convert_gain(f->post_attn_norm, &w->post_attn_norm);
        convert_gain(f->mlp_norm, &w->mlp_norm);
        convert_gain(f->q_norm, &w->q_norm);
        convert_gain(f->k_norm, &w->k_norm);
        convert(f->d1, &w->d1);
        convert(f->d2, &w->d2);
        convert(f->d3, &w->d3);
        f->attn_gate = llm_sigmoid(llm_f16(llm_rd16(w->attn_gate.data)));
    }
    convert_gain(m->final_norm, &m->final_norm_t);
    convert(m->eg_taps_f, &m->eg_taps);

    llm_tensor_t *cq[4 + 5 * LLM_MAX_LAYERS];
    size_t ncq = 0;
    cq[ncq++] = &m->embed, cq[ncq++] = &m->eg_tables, cq[ncq++] = &m->eg_key, cq[ncq++] = &m->eg_value;
    for (int L = 0; L < c.layers; L++) {
        llm_layer_w_t *w = &m->lw[L];
        cq[ncq++] = &w->q, cq[ncq++] = &w->k, cq[ncq++] = &w->v, cq[ncq++] = &w->gate, cq[ncq++] = &w->out;
    }
    for (size_t i = 0; i < ncq; i++) {
        cq[i]->fast_norm = llm_cq_check_norms(cq[i]);
    }

    const int h2 = c.head_dim / 2;
    for (int i = 0; i < h2; i++) {
        const float freq = 1.0f / powf(c.rope_theta, (float)(2 * i) / (float)c.head_dim);
        for (int pos = 0; pos < LLM_CTX; pos++) {
            const float ang = (float)pos * freq;
            m->rope_cos[pos * h2 + i] = cosf(ang);
            m->rope_sin[pos * h2 + i] = sinf(ang);
        }
    }
    err = llm_tok_init(&m->tok, m->tok.hash, hash_slots(n_normal));
    if (err) {
        return err;
    }
    *out = m;
    return IPRO_LLM_OK;
}

/* ------------------------------------------------------------ decode */

/* Every prompt starts with the same tokens at the same positions (<s>
 * <|im_start|>): their KV rows are computed once and never overwritten; only
 * the engram value ring, which later positions reuse, is put back per request.
 * Recomputed when the kernel or the accelerator changes. */
static void prefix(ipro_llm_t *m)
{
    const int np = m->g.n_prefix, d = m->cfg.d;
    if (!m->prefix_ready || m->prefix_int8 != m->int8 || m->prefix_accel != m->accel_on) {
        llm_forward(m, m->g.prefix, 0, np);
        for (int p = 0; p < np; p++) {
            memcpy(m->eg_prefix + (size_t)p * d, m->eg_v + (size_t)(p % m->eg_ring) * d,
                   (size_t)d * sizeof(float));
        }
        m->prefix_ready = true;
        m->prefix_int8 = m->int8;
        m->prefix_accel = m->accel_on;
        return;
    }
    for (int p = 0; p < np; p++) {
        m->tokens[p] = m->g.prefix[p];
        memcpy(m->eg_v + (size_t)(p % m->eg_ring) * d, m->eg_prefix + (size_t)p * d,
               (size_t)d * sizeof(float));
    }
}

/* The only token the state allows, or -1 (decode.py: "allowed or [im_end]"). */
static int forced_token(const ipro_llm_t *m, const llm_gstate_t *st)
{
    uint16_t a[MAX_ALLOWED];
    const int k = llm_g_allowed(&m->g, st, a, MAX_ALLOWED);
    return k == 0 ? m->g.im_end : k == 1 ? a[0] : -1;
}

/* If every token from here to the end is forced (after </tool_call> only
 * <|im_end|> is left), append them and finish without another forward. */
static bool finish_forced(const ipro_llm_t *m, llm_gstate_t *st, ipro_llm_result_t *res)
{
    llm_gstate_t s = *st;
    uint16_t tail[IPRO_LLM_MAX_NEW];
    int n = 0;
    while (s.phase != G_DONE) {
        const int tok = forced_token(m, &s);
        if (tok < 0 || res->n_gen + n >= IPRO_LLM_MAX_NEW) {
            return false;
        }
        tail[n++] = (uint16_t)tok;
        if (llm_g_step(&m->g, &s, (uint16_t)tok)) {
            return false;
        }
    }
    memcpy(res->gen + res->n_gen, tail, (size_t)n * sizeof *tail);
    res->n_gen = (uint16_t)(res->n_gen + n);
    *st = s;
    return true;
}

int ipro_llm_run(ipro_llm_t *m, const char *text, ipro_llm_result_t *res)
{
    if (!m || !text || !res) {
        return IPRO_LLM_E_ARG;
    }
    memset(res, 0, sizeof *res);
    uint16_t ids[IPRO_LLM_MAX_PROMPT + IPRO_LLM_MAX_NEW];
    int n = llm_encode_prompt(m, text, ids, IPRO_LLM_MAX_PROMPT);
    if (n < 0) {
        return n;
    }
    res->n_prompt = (uint16_t)n;
    memset(&m->prof, 0, sizeof m->prof);

    /* Output tokens the grammar forces before the first choice (<tool_call>)
     * need no logits: they go through the prefill with the prompt. */
    llm_gstate_t st;
    llm_g_start(&st);
    for (int tok; st.phase != G_DONE && (tok = forced_token(m, &st)) >= 0;) {
        res->gen[res->n_gen++] = (uint16_t)tok;
        ids[n++] = (uint16_t)tok;
        if (llm_g_step(&m->g, &st, (uint16_t)tok)) {
            return IPRO_LLM_E_GRAMMAR;
        }
    }

    const uint32_t t_prefill = ipro_llm_clock();
    prefix(m);
    for (int p = m->g.n_prefix; p < n; p += LLM_BATCH) {
        llm_forward(m, ids + p, p, n - p < LLM_BATCH ? n - p : LLM_BATCH);
    }
    m->prof.prefill = ipro_llm_clock() - t_prefill;
    m->prof.prefill_tokens = (uint16_t)(n - m->g.n_prefix);
    const uint32_t t_decode = ipro_llm_clock();

    uint16_t allowed[MAX_ALLOWED];
    int pos = n;
    while (st.phase != G_DONE && res->n_gen < IPRO_LLM_MAX_NEW) {
        int k = llm_g_allowed(&m->g, &st, allowed, MAX_ALLOWED);
        if (k > MAX_ALLOWED) {
            return IPRO_LLM_E_GRAMMAR;
        }
        if (k == 0) {
            allowed[0] = m->g.im_end;
            k = 1;
        }
        uint16_t tok = allowed[0];
        if (k > 1) {                /* torch argmax: first index of the maximum */
            const uint32_t t_head = ipro_llm_clock();
            float best = llm_logit(m, tok);
            for (int i = 1; i < k; i++) {
                const float v = llm_logit(m, allowed[i]);
                if (v > best || (v == best && allowed[i] < tok)) {
                    best = v;
                    tok = allowed[i];
                }
            }
            m->prof.head += ipro_llm_clock() - t_head;
        }
        res->gen[res->n_gen++] = tok;
        if (llm_g_step(&m->g, &st, tok)) {
            return IPRO_LLM_E_GRAMMAR;
        }
        if (st.phase == G_DONE || finish_forced(m, &st, res)) {
            break;
        }
        if (pos >= LLM_CTX) {
            return IPRO_LLM_E_TOOLONG;
        }
        llm_forward(m, &tok, pos++, 1);
        m->prof.decode_tokens++;
    }
    m->prof.decode = ipro_llm_clock() - t_decode;
#if defined(LLM_PROF_CYCLES)
    printf("LLM_CYCLES kv %lu qk %lu attend %lu gate %lu norm %lu fwht %lu silu %lu engram %lu (k)\r\n",
           (unsigned long)(g_llm_cycles[CB_KV] / 1000u), (unsigned long)(g_llm_cycles[CB_QK] / 1000u),
           (unsigned long)(g_llm_cycles[CB_ATTEND] / 1000u), (unsigned long)(g_llm_cycles[CB_GATE] / 1000u),
           (unsigned long)(g_llm_cycles[CB_NORM] / 1000u), (unsigned long)(g_llm_cycles[CB_FWHT] / 1000u),
           (unsigned long)(g_llm_cycles[CB_SILU] / 1000u), (unsigned long)(g_llm_cycles[CB_ENGRAM] / 1000u));
    memset(g_llm_cycles, 0, sizeof g_llm_cycles);
#endif
    int ng = res->n_gen;
    if (ng && res->gen[ng - 1] == m->g.im_end) {
        ng--;
    }
    return llm_g_calls(&m->g, res->gen, ng, res);
}

void ipro_llm_set_kernel(ipro_llm_t *m, ipro_llm_kernel_t kernel)
{
    if (m) {
        m->int8 = kernel == IPRO_LLM_KERNEL_INT8;
    }
}

const ipro_llm_prof_t *ipro_llm_profile(const ipro_llm_t *m)
{
    return m ? &m->prof : NULL;
}

__attribute__((weak)) uint32_t ipro_llm_clock(void)
{
    return 0;
}

/* -------------------------------------------------------------- names */

int ipro_llm_n_tools(const ipro_llm_t *m)
{
    return m ? m->g.n_tools : 0;
}

const char *ipro_llm_tool_name(const ipro_llm_t *m, int tool)
{
    if (!m || tool < 0 || tool >= m->g.n_tools) {
        return NULL;
    }
    return m->g.strings + m->g.tools[tool].name;
}

static const llm_arg_t *arg_at(const ipro_llm_t *m, int tool, int arg)
{
    if (!m || tool < 0 || tool >= m->g.n_tools || arg < 0 || arg >= m->g.tools[tool].n_args) {
        return NULL;
    }
    return &m->g.args[m->g.tools[tool].first_arg + arg];
}

const char *ipro_llm_arg_name(const ipro_llm_t *m, int tool, int arg)
{
    const llm_arg_t *a = arg_at(m, tool, arg);
    return a ? m->g.strings + a->name : NULL;
}

const char *ipro_llm_value_name(const ipro_llm_t *m, int tool, int arg, int value)
{
    const llm_arg_t *a = arg_at(m, tool, arg);
    if (!a || a->kind == LLM_ARG_INT) {
        return NULL;
    }
    for (int v = 0; v < a->n_values; v++) {
        const llm_value_t *x = &m->g.values[a->first_value + v];
        if (x->value == value) {
            return m->g.strings + x->name;
        }
    }
    return NULL;
}

int ipro_llm_result_json(const ipro_llm_t *m, const ipro_llm_result_t *res, char *buf, size_t len)
{
    if (!m || !res || !buf || len == 0) {
        return IPRO_LLM_E_ARG;
    }
    size_t o = 0;
#define PUT(...)                                                                  \
    do {                                                                          \
        const int w_ = snprintf(buf + o, len - o, __VA_ARGS__);                   \
        if (w_ < 0 || (size_t)w_ >= len - o) {                                    \
            return IPRO_LLM_E_TOOLONG;                                            \
        }                                                                         \
        o += (size_t)w_;                                                          \
    } while (0)
    PUT("[");
    for (int i = 0; i < res->n_calls; i++) {
        const ipro_llm_call_t *c = &res->calls[i];
        PUT("%s{\"name\":\"%s\",\"arguments\":{", i ? "," : "", ipro_llm_tool_name(m, c->tool));
        for (int a = 0; a < c->n_args; a++) {
            const ipro_llm_arg_t *x = &c->args[a];
            const llm_arg_t *ar = arg_at(m, c->tool, x->arg);
            PUT("%s\"%s\":", a ? "," : "", ipro_llm_arg_name(m, c->tool, x->arg));
            if (ar->kind == LLM_ARG_INT) {
                PUT("%d", x->value);
            } else if (ar->kind == LLM_ARG_BOOL) {
                PUT("%s", x->value ? "true" : "false");
            } else {
                PUT("\"%s\"", ipro_llm_value_name(m, c->tool, x->arg, x->value));
            }
        }
        PUT("}}");
    }
    PUT("]");
#undef PUT
    return (int)o;
}
