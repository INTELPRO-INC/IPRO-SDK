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
 * @file llm_accel.c
 * @brief The attention projections on an accelerator (the IPRO7 NPU, port/ipro7), and
 *        the reference accelerator: the same integer arithmetic in C.
 *
 * On the accelerator a projection is TFLite's int8 convolution with zero points 0 (the NPU's 1x1
 * convolution, apps/test/ipro_llm_npu_bench):
 *
 *     x8 = sat8(round(x * (1 / s_in)))                one static scale per input (NPUS section)
 *     w8 = round(w / s_w[c]),  s_w[c] = max |w[c]| / 127   per output channel, from the CQ rows
 *     y8 = sat8(MultiplyByQuantizedMultiplier(sum x8 w8, QuantizeMultiplier(s_in s_w[c] / s_out)))
 *     y  = y8 * s_out                                 one static scale per output (NPUS section)
 *
 * q, k, v and gate share their input and run as one projection with one output scale; out runs
 * alone: two accelerator calls per layer and prompt chunk, and per decode step when the
 * accelerator takes one token (min_tokens 1). tools/ipro_llm/train/npu_sim.py simulates exactly
 * this, at every position, and calibrates the scales (export_blob.py --npu-calib): it costs the
 * v7 model under 0.1 point.
 */
#include <math.h>
#include <string.h>

#include "llm_internal.h"

#define NPUS_VERSION    1u
#define NPUS_HEADER     16u
#define NPUS_LAYER      16u     /* f32 s_in, s_out of q/k/v/gate, then of out */
#define ACCEL_SLOTS     (2 * LLM_MAX_LAYERS)

static inline int8_t sat8(int32_t v)
{
    return (int8_t)(v > 127 ? 127 : v < -128 ? -128 : v);
}

static float rd_f32(const uint8_t *p)
{
    const uint32_t bits = llm_rd32(p);
    float f;
    memcpy(&f, &bits, sizeof f);
    return f;
}

int llm_accel_qkvg_rows(const llm_cfg_t *c)
{
    return (c->heads + 2 * c->kv_heads) * c->head_dim + c->d;
}

int llm_accel_parse(ipro_llm_t *m, const uint8_t *p, uint32_t len)
{
    const llm_cfg_t *c = &m->cfg;
    const int rows = llm_accel_qkvg_rows(c);
    if (len < NPUS_HEADER + NPUS_LAYER * c->layers || llm_rd16(p) != NPUS_VERSION
        || llm_rd16(p + 2) != c->layers || llm_rd16(p + 4) != rows || rows > LLM_ACCEL_OUT * LLM_MAX_D) {
        return IPRO_LLM_E_BLOB;
    }
    for (int L = 0; L < c->layers; L++) {
        const uint8_t *e = p + NPUS_HEADER + NPUS_LAYER * (uint32_t)L;
        for (int j = 0; j < 2; j++) {
            const float s_in = rd_f32(e + 8u * j), s_out = rd_f32(e + 8u * j + 4u);
            if (!(s_in > 0.0f && s_out > 0.0f && isfinite(s_in) && isfinite(s_out))) {
                return IPRO_LLM_E_BLOB;
            }
            m->npus[L][j].s_in = s_in;
            m->npus[L][j].s_out = s_out;
        }
    }
    m->has_npus = true;
    return IPRO_LLM_OK;
}

/* ------------------------------------------------------------- arithmetic */

int8_t ipro_llm_requant(int32_t acc, int32_t mult, int shift)
{
    const int left = shift > 0 ? shift : 0;
    const int right = shift > 0 ? 0 : (-shift > 31 ? 31 : -shift);
    const int32_t x = (int32_t)((int64_t)acc * ((int64_t)1 << left));
    int32_t hi;                                         /* SaturatingRoundingDoublingHighMul */
    if (x == INT32_MIN && mult == INT32_MIN) {
        hi = INT32_MAX;
    } else {
        const int64_t ab = (int64_t)x * mult;
        const int64_t nudge = ab >= 0 ? ((int64_t)1 << 30) : (1 - ((int64_t)1 << 30));
        hi = (int32_t)((ab + nudge) / ((int64_t)1 << 31));
    }
    const int32_t mask = (int32_t)(((int64_t)1 << right) - 1);      /* RoundingDivideByPOT */
    const int32_t threshold = (mask >> 1) + (hi < 0 ? 1 : 0);
    return sat8((hi >> right) + ((hi & mask) > threshold ? 1 : 0));
}

/* TFLite QuantizeMultiplier: real = mult * 2^(shift - 31), mult in [2^30, 2^31). false: the
 * shift does not fit the NPU's 6 bits (a scale in the NPUS section is far off). */
static bool quantize_multiplier(double real, int32_t *mult, int8_t *shift)
{
    int e = 0;
    const double q = real > 0.0 ? frexp(real, &e) : 0.0;
    int64_t qf = llround(q * 2147483648.0);
    if (qf == ((int64_t)1 << 31)) {
        qf /= 2;
        e++;
    }
    if (qf == 0 || e < -31) {               /* TFLite: too small to matter, the output is 0 */
        *mult = 0;
        *shift = 0;
        return true;
    }
    if (e > 31) {
        return false;
    }
    *mult = (int32_t)qf;
    *shift = (int8_t)e;
    return true;
}

/* ---------------------------------------------------------------- attach */

typedef struct {
    int8_t  *w;         /* [rows][d] */
    int32_t *mult;
    int8_t  *shift;
} proj_buf_t;

static int proj_rows(const ipro_llm_t *m, int which)
{
    return which == 0 ? llm_accel_qkvg_rows(&m->cfg) : m->cfg.d;
}

static size_t proj_bytes(int rows, int d)
{
    return ((size_t)rows * (size_t)d + 3u) / 4u * 4u + (size_t)rows * (sizeof(int32_t) + 1u);
}

size_t ipro_llm_accel_scratch_bytes(const ipro_llm_t *m)
{
    if (!m || !m->has_npus) {
        return 0;
    }
    return proj_bytes(llm_accel_qkvg_rows(&m->cfg), m->cfg.d) + 16u;
}

static proj_buf_t carve(uint8_t *p, int rows, int d)
{
    proj_buf_t b;
    b.w = (int8_t *)p;
    p += ((size_t)rows * (size_t)d + 3u) / 4u * 4u;
    b.mult = (int32_t *)(void *)p;
    b.shift = (int8_t *)(p + (size_t)rows * sizeof(int32_t));
    return b;
}

/* One projection's rows (q, k, v, gate stacked, or out) as int8, a scale and multiplier per row. */
static int build_proj(const ipro_llm_t *m, int layer, int which, const proj_buf_t *b)
{
    const llm_layer_w_t *lw = &m->lw[layer];
    const llm_tensor_t *t[4] = { &lw->q, &lw->k, &lw->v, &lw->gate };
    const int nt = which == 0 ? 4 : 1;
    if (which) {
        t[0] = &lw->out;
    }
    const llm_qscale_t *qs = &m->npus[layer][which];
    const uint32_t d = m->cfg.d;
    float row[LLM_MAX_D];
    int r = 0;
    for (int i = 0; i < nt; i++) {
        if (t[i]->cols != d) {
            return IPRO_LLM_E_BLOB;
        }
        for (uint32_t j = 0; j < t[i]->rows; j++, r++) {
            llm_cq_row(row, t[i], j);
            float mx = 0.0f;
            for (uint32_t k = 0; k < d; k++) {
                const float a = fabsf(row[k]);
                mx = a > mx ? a : mx;
            }
            const float s = mx > 0.0f ? mx / 127.0f : 1.0f;
            int8_t *dst = b->w + (size_t)r * d;
            for (uint32_t k = 0; k < d; k++) {
                dst[k] = sat8((int32_t)lrintf(row[k] / s));
            }
            if (!quantize_multiplier((double)qs->s_in * (double)s / (double)qs->s_out, &b->mult[r], &b->shift[r])) {
                return IPRO_LLM_E_BLOB;
            }
        }
    }
    return r == proj_rows(m, which) ? IPRO_LLM_OK : IPRO_LLM_E_BLOB;
}

int ipro_llm_accel_attach(ipro_llm_t *m, const ipro_llm_accel_t *a, void *scratch, size_t len)
{
    if (!m) {
        return IPRO_LLM_E_ARG;
    }
    m->accel = NULL;
    m->accel_on = IPRO_LLM_ACCEL_OFF;
    if (!a) {
        return IPRO_LLM_OK;
    }
    if (!m->has_npus || !m->ax || !m->ay) {
        return IPRO_LLM_E_ACCEL;
    }
    if (!a->load || !a->run || a->max_tokens < 2 || a->min_tokens < 1 || a->min_tokens > 2) {
        return IPRO_LLM_E_ARG;
    }
    if (!scratch || len < ipro_llm_accel_scratch_bytes(m)) {
        return IPRO_LLM_E_NOMEM;
    }
    uint8_t *base = (uint8_t *)(((uintptr_t)scratch + 15u) & ~(uintptr_t)15u);
    const int d = m->cfg.d;
    for (int L = 0; L < m->cfg.layers; L++) {
        for (int which = 0; which < 2; which++) {
            const int rows = proj_rows(m, which);
            const proj_buf_t b = carve(base, rows, d);
            int err = build_proj(m, L, which, &b);
            if (!err && a->load(a->ctx, 2 * L + which, d, rows, b.w, b.mult, b.shift,
                                m->npus[L][which].s_out)) {
                err = IPRO_LLM_E_ACCEL;
            }
            if (err) {
                return err;
            }
        }
    }
    m->accel = a;
    ipro_llm_accel_enable(m, IPRO_LLM_ACCEL_ALL);
    return IPRO_LLM_OK;
}

void ipro_llm_accel_enable(ipro_llm_t *m, int what)
{
    if (m) {
        const int can = !m->accel ? 0 : m->accel->min_tokens == 1 ? IPRO_LLM_ACCEL_ALL : IPRO_LLM_ACCEL_PREFILL;
        m->accel_on = what & can;
    }
}

/* ----------------------------------------------------------- prefill hooks */

/* rows[t * LLM_MAX_D + i] -> m->ax, run `slot`, outputs in m->ay; false = use the CPU kernels. */
static bool run_proj(ipro_llm_t *m, int slot, const float *rows, int n, float s_in)
{
    const uint32_t t0 = ipro_llm_clock();
    const int d = m->cfg.d;
    const float inv = 1.0f / s_in;         /* fdiv per element would cost ~0.5 ms per projection */
    for (int t = 0; t < n; t++) {
        const float *x = rows + (size_t)t * LLM_MAX_D;
        int8_t *q = m->ax + (size_t)t * (size_t)d;
        for (int i = 0; i < d; i++) {      /* clamp first: lrintf of an out-of-range value is undefined */
            q[i] = (int8_t)lrintf(fminf(fmaxf(x[i] * inv, -128.0f), 127.0f));
        }
    }
    const int rc = m->accel->run(m->accel->ctx, slot, m->ax, n, m->ay);
    m->prof.accel += ipro_llm_clock() - t0;
    if (rc) {
        m->prof.accel_fails++;
        return false;
    }
    m->prof.accel_runs++;
    return true;
}

static bool use_accel(const ipro_llm_t *m, int n)
{
    const int need = n == 1 ? IPRO_LLM_ACCEL_DECODE : IPRO_LLM_ACCEL_PREFILL;
    return (m->accel_on & need) && n <= m->accel->max_tokens;
}

bool llm_accel_qkvg(ipro_llm_t *m, int layer, int n)
{
    if (!use_accel(m, n)) {
        return false;
    }
    const llm_cfg_t *c = &m->cfg;
    const llm_qscale_t *qs = &m->npus[layer][0];
    if (!run_proj(m, 2 * layer, m->bh, n, qs->s_in)) {
        return false;
    }
    const int qd = c->heads * c->head_dim, kvd = c->kv_heads * c->head_dim;
    const int rows = llm_accel_qkvg_rows(c);
    const float s = qs->s_out;
    for (int t = 0; t < n; t++) {
        const int8_t *y = m->ay + (size_t)t * (size_t)rows;
        float *q = m->bq + (size_t)t * LLM_MAX_D, *k = m->bk + (size_t)t * LLM_MAX_D;
        float *v = m->bv + (size_t)t * LLM_MAX_D, *g = m->bg + (size_t)t * LLM_MAX_D;
        for (int i = 0; i < qd; i++) {
            q[i] = (float)y[i] * s;
        }
        for (int i = 0; i < kvd; i++) {
            k[i] = (float)y[qd + i] * s;
            v[i] = (float)y[qd + kvd + i] * s;
        }
        for (int i = 0; i < c->d; i++) {
            g[i] = (float)y[qd + 2 * kvd + i] * s;
        }
    }
    return true;
}

bool llm_accel_out(ipro_llm_t *m, int layer, int n)
{
    if (!use_accel(m, n)) {
        return false;
    }
    const llm_qscale_t *qs = &m->npus[layer][1];
    if (!run_proj(m, 2 * layer + 1, m->by, n, qs->s_in)) {
        return false;
    }
    const int d = m->cfg.d;
    for (int t = 0; t < n; t++) {
        const int8_t *y = m->ay + (size_t)t * (size_t)d;
        float *o = m->bo + (size_t)t * LLM_MAX_D;
        for (int i = 0; i < d; i++) {
            o[i] = (float)y[i] * qs->s_out;
        }
    }
    return true;
}

/* ------------------------------------------------------ reference backend */

typedef struct {
    struct {
        int8_t  *w;
        int32_t *mult;
        int8_t  *shift;
        int      cin, cout;
    } slot[ACCEL_SLOTS];
    uint8_t *mem;
    size_t   used, cap;
} ref_ctx_t;

size_t ipro_llm_accel_ref_bytes(const ipro_llm_t *m)
{
    if (!m || !m->has_npus) {
        return 0;
    }
    const size_t per_layer = proj_bytes(proj_rows(m, 0), m->cfg.d) + proj_bytes(proj_rows(m, 1), m->cfg.d) + 32u;
    return sizeof(ref_ctx_t) + 16u + per_layer * m->cfg.layers;
}

static int ref_load(void *ctx, int slot, int cin, int cout, const int8_t *w, const int32_t *mult,
                    const int8_t *shift, float s_out)
{
    (void)s_out;
    ref_ctx_t *r = ctx;
    const size_t need = proj_bytes(cout, cin);
    r->used = (r->used + 15u) & ~(size_t)15u;
    if (slot < 0 || slot >= ACCEL_SLOTS || r->used + need > r->cap) {
        return -1;
    }
    const proj_buf_t b = carve(r->mem + r->used, cout, cin);
    r->used += need;
    memcpy(b.w, w, (size_t)cout * (size_t)cin);
    memcpy(b.mult, mult, (size_t)cout * sizeof *mult);
    memcpy(b.shift, shift, (size_t)cout);
    r->slot[slot].w = b.w;
    r->slot[slot].mult = b.mult;
    r->slot[slot].shift = b.shift;
    r->slot[slot].cin = cin;
    r->slot[slot].cout = cout;
    return 0;
}

static int ref_run(void *ctx, int slot, const int8_t *x, int n, int8_t *y)
{
    const ref_ctx_t *r = ctx;
    if (slot < 0 || slot >= ACCEL_SLOTS || !r->slot[slot].w) {
        return -1;
    }
    const int cin = r->slot[slot].cin, cout = r->slot[slot].cout;
    for (int t = 0; t < n; t++) {
        const int8_t *xt = x + (size_t)t * (size_t)cin;
        for (int c = 0; c < cout; c++) {
            const int8_t *wc = r->slot[slot].w + (size_t)c * (size_t)cin;
            int32_t acc = 0;
            for (int i = 0; i < cin; i++) {
                acc += (int32_t)xt[i] * wc[i];
            }
            y[(size_t)t * (size_t)cout + (size_t)c] = ipro_llm_requant(acc, r->slot[slot].mult[c], r->slot[slot].shift[c]);
        }
    }
    return 0;
}

int ipro_llm_accel_ref(ipro_llm_accel_t *a, void *mem, size_t len, const ipro_llm_t *m)
{
    if (!a || !mem || !m) {
        return IPRO_LLM_E_ARG;
    }
    if (len < ipro_llm_accel_ref_bytes(m) || !m->has_npus) {
        return m->has_npus ? IPRO_LLM_E_NOMEM : IPRO_LLM_E_ACCEL;
    }
    ref_ctx_t *r = (ref_ctx_t *)(((uintptr_t)mem + 15u) & ~(uintptr_t)15u);
    memset(r, 0, sizeof *r);
    r->mem = (uint8_t *)(r + 1);
    r->cap = len - (size_t)(r->mem - (uint8_t *)mem);
    a->load = ref_load;
    a->run = ref_run;
    a->ctx = r;
    a->max_tokens = LLM_BATCH;
    a->min_tokens = 1;
    return IPRO_LLM_OK;
}
