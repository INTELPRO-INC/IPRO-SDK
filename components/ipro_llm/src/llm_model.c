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
 * @file llm_model.c
 * @brief Decoder forward of the IPRO SAN (tools/ipro_llm/train/model.py).
 *
 *   x = embed[tok] * sqrt(d)
 *   per layer (engram first at its site):
 *     x += sigmoid(attn_gate) * post_norm(attn(attn_norm(x)))
 *     x += mlp(mlp_norm(x))                  weightless Hadamard MLP
 *   logits = embed . final_norm(x)            tied head, only where asked
 *
 * Attention: GQA, ZC-RMSNorm on q / k, half-split RoPE, sigmoid output gate,
 * a KV cache per layer. Engram: two hashed n-gram tables read by gather, a
 * key / value projection, dilated taps over past values, a cosine gate.
 *
 * One forward takes up to LLM_BATCH consecutive tokens (the prompt in chunks,
 * then one token per decode step). The projections of the whole batch run as
 * one matmul, so each weight row is read and expanded once per batch; the
 * per-token work keeps the token order (KV written before any query reads it,
 * engram values pushed into their ring one position at a time), so a batch
 * computes exactly what one token at a time would - except with an accelerator
 * attached (llm_accel.c): prompt chunks then run q/k/v/gate and out in its int8
 * arithmetic.
 */
#include <math.h>
#include <string.h>

#include "llm_internal.h"
#include "llm_simd.h"

#if defined(LLM_PROF_CYCLES)
uint32_t g_llm_cycles[CB_COUNT];
#endif

#define ENGRAM_SEED  0x9E3779B9u
#define ENGRAM_PRIME 0x01000193u

/* Accumulate the clock time of stmt into m->prof.field. */
#define PROF(field, stmt)                                       \
    do {                                                        \
        const uint32_t t0_ = ipro_llm_clock();                  \
        stmt;                                                   \
        m->prof.field += ipro_llm_clock() - t0_;                \
    } while (0)

static float *kv_at(const ipro_llm_t *m, float *base, int layer, int kvh, int pos)
{
    const llm_cfg_t *c = &m->cfg;
    return base + (((size_t)layer * c->kv_heads + (size_t)kvh) * LLM_CTX + (size_t)pos) * c->head_dim;
}

static void rope(const ipro_llm_t *m, float *v, int pos)
{
    const int h = m->cfg.head_dim / 2;
    const float *cs = m->rope_cos + (size_t)pos * h, *sn = m->rope_sin + (size_t)pos * h;
    for (int i = 0; i < h; i++) {
        const float a = v[i], b = v[i + h];
        v[i] = a * cs[i] - b * sn[i];
        v[i + h] = b * cs[i] + a * sn[i];
    }
}

/* Rotate (and for int8 quantise) rows[t] for t < n into the batch acts. */
static void prep_batch(ipro_llm_t *m, const float *rows, uint32_t cols, uint32_t group, int n)
{
    for (int t = 0; t < n; t++) {
        llm_act_prep(&m->bact[t], rows + (size_t)t * LLM_MAX_D, cols, group, m->int8);
    }
}

static void engram(ipro_llm_t *m, int pos0, int n)
{
    const llm_cfg_t *c = &m->cfg;
    const int d = c->d, sub = d / 2;
    float *e = m->bh, *k = m->bq, *v = m->bg;           /* free until attention() */
    for (int t = 0; t < n; t++) {
        const int pos = pos0 + t;
        for (int oi = 0; oi < 2; oi++) {
            const int order = c->orders[oi];
            float *row = e + (size_t)t * LLM_MAX_D + oi * sub;
            if (pos < order - 1) {
                memset(row, 0, (size_t)sub * sizeof *row);
                continue;
            }
            uint32_t acc = ENGRAM_SEED * (uint32_t)(oi + 1);
            for (int j = 0; j < order; j++) {
                acc = (acc ^ m->tokens[pos - j]) * ENGRAM_PRIME;
            }
            acc ^= acc >> 15;
            llm_cq_row(row, &m->eg_tables, (uint32_t)oi * c->engram_slots + acc % c->engram_slots);
        }
    }
    prep_batch(m, e, (uint32_t)d, m->eg_key.group, n);
    llm_cq_matmul(k, LLM_MAX_D, &m->eg_key, m->bactp, (uint32_t)n);
    llm_cq_matmul(v, LLM_MAX_D, &m->eg_value, m->bactp, (uint32_t)n);

    for (int t = 0; t < n; t++) {        /* in position order: the ring holds the past */
        const int pos = pos0 + t;
        float *x = m->bx + (size_t)t * LLM_MAX_D, *kt = k + (size_t)t * LLM_MAX_D;
        memcpy(m->eg_v + (size_t)(pos % m->eg_ring) * d, v + (size_t)t * LLM_MAX_D,
               (size_t)d * sizeof(float));
        memset(m->mix, 0, (size_t)d * sizeof *m->mix);
        for (int j = 0; j < c->engram_taps; j++) {
            const int p = pos - j * c->engram_dilation;
            if (p < 0) {
                break;
            }
            const float *vp = m->eg_v + (size_t)(p % m->eg_ring) * d, *tap = m->eg_taps_f + (size_t)j * d;
            for (int i = 0; i < d; i++) {
                m->mix[i] += tap[i] * vp[i];
            }
        }
        llm_rmsnorm(m->ux, x, NULL, d, 1e-6f);
        llm_rmsnorm(m->uk, kt, NULL, d, 1e-6f);
        float dot = 0.0f;
        for (int i = 0; i < d; i++) {
            dot += m->ux[i] * m->uk[i];
        }
        const float alpha = llm_sigmoid(dot / sqrtf((float)d));
        for (int i = 0; i < d; i++) {
            x[i] += alpha * m->mix[i];
        }
    }
}

static void attend(ipro_llm_t *m, int layer, int pos, const float *qv, float *yv, int kh)
{
    const int hd = m->cfg.head_dim;
    const float scale = 1.0f / sqrtf((float)hd);
    float top = -INFINITY;
    for (int j = 0; j <= pos; j++) {
        const float *kj = kv_at(m, m->kcache, layer, kh, j);
        float s = 0.0f;
        for (int i = 0; i < hd; i++) {
            s += qv[i] * kj[i];
        }
        m->scores[j] = s * scale;
        top = m->scores[j] > top ? m->scores[j] : top;
    }
    float sum = 0.0f;
    for (int j = 0; j <= pos; j++) {
        m->scores[j] = llm_expf(m->scores[j] - top);
        sum += m->scores[j];
    }
    memset(yv, 0, (size_t)hd * sizeof *yv);
    for (int j = 0; j <= pos; j++) {
        const float p = m->scores[j] / sum;
        const float *vj = kv_at(m, m->vcache, layer, kh, j);
        for (int i = 0; i < hd; i++) {
            yv[i] += p * vj[i];
        }
    }
}

/* attend() for the two query heads of KV head kh (GQA, heads = 2 x kv_heads): every K and V
 * row is read once for both, and two positions are scored at a time, so four independent sums
 * overlap. Per head and position the arithmetic and its order are attend()'s: bit-identical.
 * fast (the int8 kernel): the softmax's exp from the table and one reciprocal per head instead
 * of a division per position. */
static void attend2(ipro_llm_t *m, int layer, int pos, const float *q0, const float *q1,
                    float *y0, float *y1, int kh, bool fast)
{
    const int hd = m->cfg.head_dim;
    const float scale = 1.0f / sqrtf((float)hd);
    float *s0 = m->scores, *s1 = m->scores2;
    float top0 = -INFINITY, top1 = -INFINITY;
    int j = 0;
    for (; j + 1 <= pos; j += 2) {
        const float *ka = kv_at(m, m->kcache, layer, kh, j), *kb = ka + hd;
        float a0 = 0.0f, a1 = 0.0f, b0 = 0.0f, b1 = 0.0f;
        for (int i = 0; i < hd; i++) {
            a0 += q0[i] * ka[i];
            a1 += q1[i] * ka[i];
            b0 += q0[i] * kb[i];
            b1 += q1[i] * kb[i];
        }
        s0[j] = a0 * scale, s1[j] = a1 * scale, s0[j + 1] = b0 * scale, s1[j + 1] = b1 * scale;
        top0 = s0[j] > top0 ? s0[j] : top0;
        top1 = s1[j] > top1 ? s1[j] : top1;
        top0 = s0[j + 1] > top0 ? s0[j + 1] : top0;
        top1 = s1[j + 1] > top1 ? s1[j + 1] : top1;
    }
    for (; j <= pos; j++) {
        const float *kj = kv_at(m, m->kcache, layer, kh, j);
        float a0 = 0.0f, a1 = 0.0f;
        for (int i = 0; i < hd; i++) {
            a0 += q0[i] * kj[i];
            a1 += q1[i] * kj[i];
        }
        s0[j] = a0 * scale, s1[j] = a1 * scale;
        top0 = s0[j] > top0 ? s0[j] : top0;
        top1 = s1[j] > top1 ? s1[j] : top1;
    }
    float sum0 = 0.0f, sum1 = 0.0f;
    for (j = 0; j <= pos; j++) {
        s0[j] = fast ? llm_exp_neg_lut(s0[j] - top0) : llm_expf(s0[j] - top0);
        s1[j] = fast ? llm_exp_neg_lut(s1[j] - top1) : llm_expf(s1[j] - top1);
        sum0 += s0[j];
        sum1 += s1[j];
    }
    memset(y0, 0, (size_t)hd * sizeof *y0);
    memset(y1, 0, (size_t)hd * sizeof *y1);
    const float r0 = 1.0f / sum0, r1 = 1.0f / sum1;
    for (j = 0; j <= pos; j++) {
        const float p0 = fast ? s0[j] * r0 : s0[j] / sum0, p1 = fast ? s1[j] * r1 : s1[j] / sum1;
        const float *vj = kv_at(m, m->vcache, layer, kh, j);
        for (int i = 0; i < hd; i++) {
            y0[i] += p0 * vj[i];
            y1[i] += p1 * vj[i];
        }
    }
}

/* ---- the int8 kernel's attention: integer dot products (SMAQA, four int8 products per step) */

static size_t kv_rowset(const ipro_llm_t *m, int layer, int kvh)
{
    return (size_t)layer * m->cfg.kv_heads + (size_t)kvh;
}

/* dst = round(x / s), s = max |x| / 127; returns s */
static float quant_row(int8_t *dst, const float *x, int n)
{
    float mx = 0.0f;
    for (int i = 0; i < n; i++) {
        const float a = fabsf(x[i]);
        mx = a > mx ? a : mx;
    }
    const float s = mx > 0.0f ? mx / 127.0f : 1.0f, inv = 1.0f / s;
    for (int i = 0; i < n; i++) {
        dst[i] = (int8_t)lrintf(x[i] * inv);
    }
    return s;
}

/* K row and V column of position pos into the int8 KV */
static void kv_write_q8(ipro_llm_t *m, int layer, int kvh, int pos, const float *k, const float *v)
{
    const int hd = m->cfg.head_dim;
    const size_t r = kv_rowset(m, layer, kvh);
    m->ks[r * LLM_CTX + (size_t)pos] = quant_row(m->kq + (r * LLM_CTX + (size_t)pos) * (size_t)hd, k, hd);
    float mx = 0.0f;
    for (int i = 0; i < hd; i++) {
        const float a = fabsf(v[i]);
        mx = a > mx ? a : mx;
    }
    const float sv = mx > 0.0f ? mx / 127.0f : 1.0f, inv = 1.0f / sv;
    int8_t *vt = m->vq + r * (size_t)hd * LLM_CTX + (size_t)pos;
    for (int i = 0; i < hd; i++) {
        vt[(size_t)i * LLM_CTX] = (int8_t)lrintf(v[i] * inv);
    }
    m->vs[r * LLM_CTX + (size_t)pos] = sv;
}

/* attend2() on the int8 KV. Scores: q (int8, a scale per head) . K row (int8, a scale per row)
 * by SMAQA; the softmax in float (exp from the table); then p_j * vscale_j quantised to int8 per
 * head and multiplied with the transposed V four positions per SMAQA. hd must be a multiple of 4. */
static void attend2_q8(ipro_llm_t *m, int layer, int pos, const float *q0, const float *q1,
                       float *y0, float *y1, int kh)
{
    const int hd = m->cfg.head_dim, words = hd / 4, n = pos + 1, nw = (n + 3) / 4;
    const size_t r = kv_rowset(m, layer, kh);
    int8_t qa[LLM_MAX_HEAD_DIM] __attribute__((aligned(4))), qb[LLM_MAX_HEAD_DIM] __attribute__((aligned(4)));
    int8_t pa[LLM_CTX + 4] __attribute__((aligned(4))), pb[LLM_CTX + 4] __attribute__((aligned(4)));
    const float scale = 1.0f / sqrtf((float)hd);
    const float sa = quant_row(qa, q0, hd) * scale, sb = quant_row(qb, q1, hd) * scale;
    const int8_t *kq = m->kq + r * LLM_CTX * (size_t)hd;
    const float *ks = m->ks + r * LLM_CTX, *vs = m->vs + r * LLM_CTX;
    float *s0 = m->scores, *s1 = m->scores2;
    float top0 = -INFINITY, top1 = -INFINITY;
    for (int j = 0; j < n; j++) {
        const uint8_t *kp = (const uint8_t *)(kq + (size_t)j * (size_t)hd);
        int32_t da = 0, db = 0;
        for (int w = 0; w < words; w++) {
            const uint32_t kw = rd32(kp + 4 * w);
            da = SMAQA(da, rd32(qa + 4 * w), kw);
            db = SMAQA(db, rd32(qb + 4 * w), kw);
        }
        s0[j] = (float)da * (sa * ks[j]);
        s1[j] = (float)db * (sb * ks[j]);
        top0 = s0[j] > top0 ? s0[j] : top0;
        top1 = s1[j] > top1 ? s1[j] : top1;
    }
    float sum0 = 0.0f, sum1 = 0.0f;
    for (int j = 0; j < n; j++) {
        s0[j] = llm_exp_neg_lut(s0[j] - top0);
        s1[j] = llm_exp_neg_lut(s1[j] - top1);
        sum0 += s0[j];
        sum1 += s1[j];
    }
    const float r0 = 1.0f / sum0, r1 = 1.0f / sum1;
    float m0 = 0.0f, m1 = 0.0f;
    for (int j = 0; j < n; j++) {               /* p_j * vscale_j: the weight of V row j's int8 */
        s0[j] *= r0 * vs[j];
        s1[j] *= r1 * vs[j];
        m0 = s0[j] > m0 ? s0[j] : m0;
        m1 = s1[j] > m1 ? s1[j] : m1;
    }
    const float ps0 = m0 > 0.0f ? m0 / 127.0f : 1.0f, ps1 = m1 > 0.0f ? m1 / 127.0f : 1.0f;
    const float i0 = 1.0f / ps0, i1 = 1.0f / ps1;
    for (int j = 0; j < n; j++) {
        pa[j] = (int8_t)lrintf(s0[j] * i0);
        pb[j] = (int8_t)lrintf(s1[j] * i1);
    }
    for (int j = n; j < 4 * nw; j++) {          /* the last word's unused positions weigh 0 */
        pa[j] = 0;
        pb[j] = 0;
    }
    const int8_t *vt = m->vq + r * (size_t)hd * LLM_CTX;
    for (int i = 0; i < hd; i++) {
        const uint8_t *row = (const uint8_t *)(vt + (size_t)i * LLM_CTX);
        int32_t ya = 0, yb = 0;
        for (int w = 0; w < nw; w++) {
            const uint32_t vw = rd32(row + 4 * w);
            ya = SMAQA(ya, rd32(pa + 4 * w), vw);
            yb = SMAQA(yb, rd32(pb + 4 * w), vw);
        }
        y0[i] = (float)ya * ps0;
        y1[i] = (float)yb * ps1;
    }
}

/* y[i] *= sigmoid(g[i]), four elements at a time so their expf chains overlap; fast (the int8
 * kernel): the sigmoid from the table. */
static void gate_mul(float *y, const float *g, int n, bool fast)
{
    if (fast) {
        for (int i = 0; i < n; i++) {
            y[i] *= llm_sigmoid_lut(g[i]);
        }
        return;
    }
    int i = 0;
    for (; i + 4 <= n; i += 4) {
        const float s0 = llm_sigmoid(g[i]), s1 = llm_sigmoid(g[i + 1]);
        const float s2 = llm_sigmoid(g[i + 2]), s3 = llm_sigmoid(g[i + 3]);
        y[i] *= s0, y[i + 1] *= s1, y[i + 2] *= s2, y[i + 3] *= s3;
    }
    for (; i < n; i++) {
        y[i] *= llm_sigmoid(g[i]);
    }
}

static void attention(ipro_llm_t *m, int layer, int pos0, int n)
{
    const llm_cfg_t *c = &m->cfg;
    const llm_layer_w_t *w = &m->lw[layer];
    const llm_layer_f_t *f = &m->lf[layer];
    const int d = c->d, hd = c->head_dim, rep = c->heads / c->kv_heads;
    const bool q8 = m->int8 && rep == 2 && hd % 4 == 0;     /* the int8 kernel's attention */

    PROF(attn_proj, {
        for (int t = 0; t < n; t++) {
            llm_rmsnorm(m->bh + (size_t)t * LLM_MAX_D, m->bx + (size_t)t * LLM_MAX_D,
                        f->attn_norm, d, c->eps);
        }
        if (!llm_accel_qkvg(m, layer, n)) {     /* the NPU for this chunk or step, if attached */
            prep_batch(m, m->bh, (uint32_t)d, w->q.group, n);
            llm_cq_matmul(m->bq, LLM_MAX_D, &w->q, m->bactp, (uint32_t)n);
            llm_cq_matmul(m->bk, LLM_MAX_D, &w->k, m->bactp, (uint32_t)n);
            llm_cq_matmul(m->bv, LLM_MAX_D, &w->v, m->bactp, (uint32_t)n);
            llm_cq_matmul(m->bg, LLM_MAX_D, &w->gate, m->bactp, (uint32_t)n);
        }
    });

    const uint32_t t_core = ipro_llm_clock();
    LLM_CB(CB_KV, for (int t = 0; t < n; t++) {     /* the batch's KV first: later tokens attend to it */
        const int pos = pos0 + t;
        for (int kh = 0; kh < c->kv_heads; kh++) {
            float *kc = kv_at(m, m->kcache, layer, kh, pos), *vc = kv_at(m, m->vcache, layer, kh, pos);
            llm_rmsnorm(kc, m->bk + (size_t)t * LLM_MAX_D + kh * hd, f->k_norm, hd, c->eps);
            rope(m, kc, pos);
            memcpy(vc, m->bv + (size_t)t * LLM_MAX_D + kh * hd, (size_t)hd * sizeof *vc);
            if (q8) {
                kv_write_q8(m, layer, kh, pos, kc, vc);
            }
        }
    });
    for (int t = 0; t < n; t++) {
        const int pos = pos0 + t;
        float *q = m->bq + (size_t)t * LLM_MAX_D, *y = m->by + (size_t)t * LLM_MAX_D;
        const float *g = m->bg + (size_t)t * LLM_MAX_D;
        LLM_CB(CB_QK, for (int qh = 0; qh < c->heads; qh++) {
            float *qv = q + qh * hd;
            llm_rmsnorm(qv, qv, f->q_norm, hd, c->eps);
            rope(m, qv, pos);
        });
        LLM_CB(CB_ATTEND, if (q8) {
            for (int kh = 0; kh < c->kv_heads; kh++) {
                attend2_q8(m, layer, pos, q + 2 * kh * hd, q + (2 * kh + 1) * hd, y + 2 * kh * hd,
                           y + (2 * kh + 1) * hd, kh);
            }
        } else if (rep == 2) {
            for (int kh = 0; kh < c->kv_heads; kh++) {
                attend2(m, layer, pos, q + 2 * kh * hd, q + (2 * kh + 1) * hd, y + 2 * kh * hd,
                        y + (2 * kh + 1) * hd, kh, m->int8);
            }
        } else {
            for (int qh = 0; qh < c->heads; qh++) {
                attend(m, layer, pos, q + qh * hd, y + qh * hd, qh / rep);
            }
        });
        LLM_CB(CB_GATE, gate_mul(y, g, d, m->int8));
    }
    m->prof.attn_core += ipro_llm_clock() - t_core;

    PROF(attn_proj, {
        if (!llm_accel_out(m, layer, n)) {
            prep_batch(m, m->by, (uint32_t)d, w->out.group, n);
            llm_cq_matmul(m->bo, LLM_MAX_D, &w->out, m->bactp, (uint32_t)n);
        }
    });
    for (int t = 0; t < n; t++) {
        float *o = m->bo + (size_t)t * LLM_MAX_D, *x = m->bx + (size_t)t * LLM_MAX_D;
        LLM_CB(CB_NORM, llm_rmsnorm(o, o, f->post_attn_norm, d, c->eps));
        for (int i = 0; i < d; i++) {
            x[i] += f->attn_gate * o[i];
        }
    }
}

static void hadamard_mlp(ipro_llm_t *m, int layer, float *x)
{
    const llm_cfg_t *c = &m->cfg;
    const llm_layer_f_t *f = &m->lf[layer];
    const int d = c->d, n = c->mlp_n;
    const float inv = 1.0f / sqrtf((float)n);
    float *h = m->ux, *z = m->z;
    LLM_CB(CB_NORM, llm_rmsnorm(h, x, f->mlp_norm, d, c->eps);
           for (int i = 0; i < n; i++) {
               z[i] = i < d ? f->d1[i] * h[i] : 0.0f;
           });
    LLM_CB(CB_FWHT, llm_fwht(z, n));
    LLM_CB(CB_SILU, if (m->int8) {
        for (int i = 0; i < n; i++) {                /* the int8 kernel: SiLU from the table */
            z[i] = llm_silu_lut(f->d2[i] * z[i] * inv);
        }
    } else {
        int i = 0;
        for (; i + 4 <= n; i += 4) {            /* four at a time: the expf chains overlap */
            const float a = llm_silu(f->d2[i] * z[i] * inv), b = llm_silu(f->d2[i + 1] * z[i + 1] * inv);
            const float e = llm_silu(f->d2[i + 2] * z[i + 2] * inv), g = llm_silu(f->d2[i + 3] * z[i + 3] * inv);
            z[i] = a, z[i + 1] = b, z[i + 2] = e, z[i + 3] = g;
        }
        for (; i < n; i++) {
            z[i] = llm_silu(f->d2[i] * z[i] * inv);
        }
    });
    LLM_CB(CB_FWHT, llm_fwht(z, n));
    LLM_CB(CB_NORM, for (int i = 0; i < d; i++) {
        x[i] += f->d3[i] * z[i] * inv;
    });
}

void llm_forward(ipro_llm_t *m, const uint16_t *tokens, int pos0, int n)
{
    const llm_cfg_t *c = &m->cfg;
    for (int t = 0; t < n; t++) {
        m->bactp[t] = &m->bact[t];
    }
    PROF(embed, {
        const float s = sqrtf((float)c->d);
        for (int t = 0; t < n; t++) {
            float *x = m->bx + (size_t)t * LLM_MAX_D;
            m->tokens[pos0 + t] = tokens[t];
            llm_cq_row(x, &m->embed, tokens[t]);
            for (int i = 0; i < c->d; i++) {
                x[i] *= s;
            }
        }
    });
    for (int layer = 0; layer < c->layers; layer++) {
        if (layer == c->engram_site) {
            PROF(engram, LLM_CB(CB_ENGRAM, engram(m, pos0, n)));
        }
        attention(m, layer, pos0, n);
        PROF(mlp, {
            for (int t = 0; t < n; t++) {
                hadamard_mlp(m, layer, m->bx + (size_t)t * LLM_MAX_D);
            }
        });
    }
    PROF(head, {
        llm_rmsnorm(m->final_h, m->bx + (size_t)(n - 1) * LLM_MAX_D, m->final_norm, c->d, c->eps);
        llm_act_prep(&m->head_act, m->final_h, c->d, m->embed.group, m->int8);
    });
}

float llm_logit(const ipro_llm_t *m, uint16_t token)
{
    return llm_cq_row_dot(&m->embed, token, &m->head_act);
}

void llm_logits_all(const ipro_llm_t *m, float *out)
{
    for (uint32_t t = 0; t < m->cfg.vocab; t++) {
        out[t] = llm_cq_row_dot(&m->embed, (uint16_t)t, &m->head_act);
    }
}
