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
 * @file llm_cq.c
 * @brief CQ2 / CQ4 weights (Cactus-Quants layout, as tools/ipro_llm/train/quant.py).
 *
 * Per group of G input weights the blob keeps codebook indices and one fp16
 * norm; the weights are  w = (cb[idx] / sqrt(G) * norm) @ walsh(G),  with
 * walsh(G) = H / sqrt(G) symmetric. So for an activation x:
 *
 *     w . x = (norm / G) * sum_i cb[idx_i] * (H x)_i
 *
 * The activation is rotated once per group (llm_act_prep) and every row is a
 * dot product with codebook values: weights are never expanded.
 *
 * Two kernels share that layout:
 *   float  the reference; matches the PyTorch model to ~1e-5.
 *   int8   (H x) per group quantised to int8 (scale = max / 127) and the
 *          codebook as int8 (level / top * 127), so a row group is one int32
 *          sum. Each packed index byte maps through a table straight to four
 *          (2-bit) or two (4-bit) int8 codebook values, and SMAQA multiplies
 *          four of them with four activations per instruction - the kernels
 *          measured in apps/test/ipro_llm_kernel_bench (k_cq.c). Off IPRO7 the
 *          same integer arithmetic runs in C, so the host tests measure the
 *          board's accuracy exactly.
 *
 * The batched form (prefill) expands a row group's indices once and reuses
 * them for every token of the batch; per token it computes exactly what the
 * single-token form does, in the same order, so the two agree bit for bit.
 *
 * Index packing follows k_cq.c: 2-bit weight t of each four in bits 2t of a
 * byte, 4-bit low nibble first.
 */
#include <math.h>
#include <string.h>

#include "llm_internal.h"

#include "llm_simd.h"

static const float k_cb2[4] = { -1.5104f, -0.4528f, 0.4528f, 1.5104f };
static const float k_cb4[16] = {
    -2.7326f, -2.0690f, -1.6181f, -1.2562f, -0.9424f, -0.6568f, -0.3881f, -0.1284f,
     0.1284f,  0.3881f,  0.6568f,  0.9424f,  1.2562f,  1.6181f,  2.0690f,  2.7326f,
};

/* byte of packed indices -> int8 codebook values, little-endian lanes */
static LLM_OCRAM uint32_t s_lut2[256];
static LLM_OCRAM uint16_t s_lut4[256];

static int8_t level_s8(const float *cb, int n, int i)
{
    return (int8_t)lrintf(cb[i] / cb[n - 1] * 127.0f);
}

void llm_cq_init(void)
{
    for (uint32_t b = 0; b < 256u; b++) {
        uint32_t w = 0;
        for (uint32_t t = 0; t < 4u; t++) {
            w |= (uint32_t)(uint8_t)level_s8(k_cb2, 4, (int)((b >> (2u * t)) & 3u)) << (8u * t);
        }
        s_lut2[b] = w;
        s_lut4[b] = (uint16_t)((uint8_t)level_s8(k_cb4, 16, (int)(b & 15u))
                               | (uint16_t)(uint8_t)level_s8(k_cb4, 16, (int)(b >> 4)) << 8);
    }
}


static inline int cq_bits(const llm_tensor_t *w)
{
    return w->kind == LLM_CQ2 ? 2 : 4;
}

static inline uint32_t row_bytes(const llm_tensor_t *w)
{
    return w->cols * (uint32_t)cq_bits(w) / 8u;
}

static inline uint32_t norm_bytes(const llm_tensor_t *w)
{
    return 2u * (w->cols / w->group);
}

/* fp16 -> fp32 for +0 and positive normals (what CQ norms are; checked at
 * open by llm_cq_check_norms): exact, without llm_f16's branches. */
static inline float norm_fast(uint16_t h)
{
    union {
        uint32_t u;
        float f;
    } v;
    v.u = ((uint32_t)h << 13) + (112u << 23);
    return h ? v.f : 0.0f;
}

static inline float norm_at(const llm_tensor_t *w, const uint8_t *p)
{
    const uint16_t h = rd16(p);
    return w->fast_norm ? norm_fast(h) : llm_f16(h);
}

bool llm_cq_check_norms(const llm_tensor_t *w)
{
    const uint32_t n = w->rows * (w->cols / w->group);
    for (uint32_t i = 0; i < n; i++) {
        const uint16_t h = llm_rd16(w->norm + 2u * i);
        const uint32_t e = (h >> 10) & 0x1Fu;
        if (h != 0 && ((h & 0x8000u) || e == 0 || e == 0x1Fu)) {
            return false;
        }
    }
    return true;
}

/* --------------------------------------------------------- activations */

void llm_act_prep(llm_act_t *a, const float *x, uint32_t cols, uint32_t group, bool int8)
{
    a->cols = cols;
    a->group = group;
    a->int8 = int8 && group % 16u == 0;
    memcpy(a->f, x, cols * sizeof *a->f);
    for (uint32_t c = 0, g = 0; c < cols; c += group, g++) {
        float *v = a->f + c;
        llm_fwht(v, (int)group);
        if (!a->int8) {
            continue;
        }
        float mx = 0.0f;
        for (uint32_t i = 0; i < group; i++) {
            const float av = fabsf(v[i]);
            mx = av > mx ? av : mx;
        }
        const float s = mx > 0.0f ? mx / 127.0f : 1.0f, inv = 1.0f / s;
        for (uint32_t i = 0; i < group; i++) {
            a->q[c + i] = (int8_t)lrintf(v[i] * inv);
        }
        a->scale[g] = s;
    }
}

/* ------------------------------------------------------------- float */

static inline float cq_value(const llm_tensor_t *w, const uint8_t *idx, uint32_t col)
{
    if (w->kind == LLM_CQ2) {
        return k_cb2[(idx[col >> 2] >> (2u * (col & 3u))) & 3u];
    }
    return k_cb4[(idx[col >> 1] >> (4u * (col & 1u))) & 15u];
}

static float dot_float(const llm_tensor_t *w, uint32_t row, const llm_act_t *a)
{
    const uint8_t *idx = w->data + (size_t)row * row_bytes(w);
    const uint8_t *np = w->norm + (size_t)row * norm_bytes(w);
    const float inv_g = 1.0f / (float)w->group;
    float acc = 0.0f;
    for (uint32_t c = 0; c < w->cols; c += w->group, np += 2) {
        float s = 0.0f;
        for (uint32_t i = 0; i < w->group; i++) {
            s += cq_value(w, idx, c + i) * a->f[c + i];
        }
        acc += norm_at(w, np) * inv_g * s;
    }
    return acc;
}

/* -------------------------------------------------------------- int8 */

/* A row group's packed indices as int8x4 words (G / 4 of them). */
static inline void expand_group(const llm_tensor_t *w, const uint8_t *wp, uint32_t *wv)
{
    if (w->kind == LLM_CQ2) {
        for (uint32_t k = 0; k < w->group / 16u; k++, wp += 4) {
            const uint32_t ww = rd32(wp);
            wv[4 * k + 0] = s_lut2[ww & 0xFFu];
            wv[4 * k + 1] = s_lut2[(ww >> 8) & 0xFFu];
            wv[4 * k + 2] = s_lut2[(ww >> 16) & 0xFFu];
            wv[4 * k + 3] = s_lut2[ww >> 24];
        }
    } else {
        for (uint32_t k = 0; k < w->group / 8u; k++, wp += 4) {
            const uint32_t ww = rd32(wp);
            wv[2 * k + 0] = PKBB16(s_lut4[(ww >> 8) & 0xFFu], s_lut4[ww & 0xFFu]);
            wv[2 * k + 1] = PKBB16(s_lut4[ww >> 24], s_lut4[(ww >> 16) & 0xFFu]);
        }
    }
}

static inline float cb_scale(const llm_tensor_t *w)
{
    return (w->kind == LLM_CQ2 ? k_cb2[3] : k_cb4[15]) / 127.0f / (float)w->group;
}

/* One row against one activation: wp / np walk the row's indices and norms. */
static inline float row_cq2(const uint8_t *wp, const uint8_t *np, const llm_tensor_t *w,
                            const llm_act_t *a)
{
    const uint8_t *xp = (const uint8_t *)a->q;
    const uint32_t ng = w->cols / w->group, wpg = w->group / 16u;
    float acc = 0.0f;
    for (uint32_t g = 0; g < ng; g++, np += 2) {
        int32_t d = 0;
        for (uint32_t k = 0; k < wpg; k++, wp += 4, xp += 16) {
            const uint32_t ww = rd32(wp);
            d = SMAQA(d, s_lut2[ww & 0xFFu], rd32(xp));
            d = SMAQA(d, s_lut2[(ww >> 8) & 0xFFu], rd32(xp + 4));
            d = SMAQA(d, s_lut2[(ww >> 16) & 0xFFu], rd32(xp + 8));
            d = SMAQA(d, s_lut2[ww >> 24], rd32(xp + 12));
        }
        acc += norm_at(w, np) * a->scale[g] * (float)d;
    }
    return acc * cb_scale(w);
}

static inline float row_cq4(const uint8_t *wp, const uint8_t *np, const llm_tensor_t *w,
                            const llm_act_t *a)
{
    const uint8_t *xp = (const uint8_t *)a->q;
    const uint32_t ng = w->cols / w->group, wpg = w->group / 8u;
    float acc = 0.0f;
    for (uint32_t g = 0; g < ng; g++, np += 2) {
        int32_t d = 0;
        for (uint32_t k = 0; k < wpg; k++, wp += 4, xp += 8) {
            const uint32_t ww = rd32(wp);
            d = SMAQA(d, PKBB16(s_lut4[(ww >> 8) & 0xFFu], s_lut4[ww & 0xFFu]), rd32(xp));
            d = SMAQA(d, PKBB16(s_lut4[ww >> 24], s_lut4[(ww >> 16) & 0xFFu]), rd32(xp + 4));
        }
        acc += norm_at(w, np) * a->scale[g] * (float)d;
    }
    return acc * cb_scale(w);
}

static float dot_int8(const llm_tensor_t *w, uint32_t row, const llm_act_t *a)
{
    const uint8_t *wp = w->data + (size_t)row * row_bytes(w);
    const uint8_t *np = w->norm + (size_t)row * norm_bytes(w);
    return w->kind == LLM_CQ2 ? row_cq2(wp, np, w, a) : row_cq4(wp, np, w, a);
}

static void matvec_int8(float *y, const llm_tensor_t *w, const llm_act_t *a)
{
    const uint8_t *wp = w->data, *np = w->norm;
    const uint32_t rb = row_bytes(w), nb = norm_bytes(w);
    if (w->kind == LLM_CQ2) {
        for (uint32_t r = 0; r < w->rows; r++, wp += rb, np += nb) {
            y[r] = row_cq2(wp, np, w, a);
        }
    } else {
        for (uint32_t r = 0; r < w->rows; r++, wp += rb, np += nb) {
            y[r] = row_cq4(wp, np, w, a);
        }
    }
}

/* y[t * y_stride + r] for t < n: the indices of a row group are expanded once
 * and reused for every activation; the per-token arithmetic is row_cq2/4's. */
static void matmul_int8(float *y, uint32_t y_stride, const llm_tensor_t *w,
                        const llm_act_t *const *a, uint32_t n)
{
    const uint8_t *wp = w->data, *np = w->norm;
    const uint32_t rb = row_bytes(w), nb = norm_bytes(w), G = w->group, ng = w->cols / G;
    const uint32_t gb = G * (uint32_t)cq_bits(w) / 8u;
    const float cs = cb_scale(w);
    uint32_t wv[LLM_MAX_D / 4];
    float acc[LLM_BATCH];
    for (uint32_t r = 0; r < w->rows; r++, wp += rb, np += nb) {
        for (uint32_t t = 0; t < n; t++) {
            acc[t] = 0.0f;
        }
        for (uint32_t g = 0; g < ng; g++) {
            expand_group(w, wp + g * gb, wv);
            const float nf = norm_at(w, np + 2u * g);
            for (uint32_t t = 0; t < n; t++) {
                const uint8_t *xp = (const uint8_t *)a[t]->q + g * G;
                int32_t d = 0;
                for (uint32_t j = 0; j < G / 4u; j += 4, xp += 16) {
                    d = SMAQA(d, wv[j + 0], rd32(xp));
                    d = SMAQA(d, wv[j + 1], rd32(xp + 4));
                    d = SMAQA(d, wv[j + 2], rd32(xp + 8));
                    d = SMAQA(d, wv[j + 3], rd32(xp + 12));
                }
                acc[t] += nf * a[t]->scale[g] * (float)d;
            }
        }
        for (uint32_t t = 0; t < n; t++) {
            y[t * y_stride + r] = acc[t] * cs;
        }
    }
}

/* ------------------------------------------------------------ public */

float llm_cq_row_dot(const llm_tensor_t *w, uint32_t row, const llm_act_t *a)
{
    return a->int8 ? dot_int8(w, row, a) : dot_float(w, row, a);
}

void llm_cq_matvec(float *y, const llm_tensor_t *w, const llm_act_t *a)
{
    if (a->int8) {
        matvec_int8(y, w, a);
        return;
    }
    for (uint32_t r = 0; r < w->rows; r++) {
        y[r] = dot_float(w, r, a);
    }
}

void llm_cq_matmul(float *y, uint32_t y_stride, const llm_tensor_t *w,
                   const llm_act_t *const *a, uint32_t n)
{
    if (n && a[0]->int8 && w->group % 16u == 0 && w->cols <= LLM_MAX_D && n <= LLM_BATCH) {
        matmul_int8(y, y_stride, w, a, n);
        return;
    }
    for (uint32_t t = 0; t < n; t++) {
        llm_cq_matvec(y + t * y_stride, w, a[t]);
    }
}

/* Dequantise one row: (cb[idx] * norm / G) transformed by H, per group. */
void llm_cq_row(float *out, const llm_tensor_t *w, uint32_t row)
{
    const uint8_t *idx = w->data + (size_t)row * row_bytes(w);
    const uint8_t *np = w->norm + (size_t)row * norm_bytes(w);
    for (uint32_t c = 0; c < w->cols; c += w->group, np += 2) {
        const float s = norm_at(w, np) / (float)w->group;
        for (uint32_t i = 0; i < w->group; i++) {
            out[c + i] = cq_value(w, idx, c + i) * s;
        }
        llm_fwht(out + c, (int)w->group);
    }
}

void llm_f16_vec(float *out, const llm_tensor_t *t)
{
    const uint32_t n = t->rows * t->cols;
    for (uint32_t i = 0; i < n; i++) {
        out[i] = llm_f16(llm_rd16(t->data + 2u * i));
    }
}
