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
 * @file llm_internal.h
 * @brief Blob layout, context and the module interfaces of the engine.
 *        The blob format is described in components/ipro_llm/README.md.
 */
#ifndef LLM_INTERNAL_H
#define LLM_INTERNAL_H

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ipro_llm.h"

#define LLM_VERSION         1
#define LLM_HEADER_BYTES    64
#define LLM_MAX_LAYERS      8
#define LLM_MAX_D           256
#define LLM_MAX_MLP         256
#define LLM_MAX_HEAD_DIM    64
#define LLM_MAX_HEADS       8
#define LLM_MAX_TEXT        512     /* request bytes after normalisation */
#define LLM_CTX             (IPRO_LLM_MAX_PROMPT + IPRO_LLM_MAX_NEW)
#define LLM_MAX_TOOLS       32
#define LLM_MAX_GRAM_ARGS   64
#define LLM_MAX_VALUES      128
#define LLM_MAX_FIX         16      /* prompt prefix / suffix tokens */
#define LLM_BATCH           32      /* prompt tokens per batched forward */
#define LLM_ACCEL_OUT       4       /* q/k/v/gate rows per d, at most */
#define LLM_NO_RANK         0xFFFFu
#define LLM_NO_PIECE        0xFFFFu

enum { LLM_F16 = 0, LLM_CQ2 = 1, LLM_CQ4 = 2 };
enum { LLM_ARG_INT = 0, LLM_ARG_ENUM = 1, LLM_ARG_BOOL = 2 };

typedef struct {
    uint8_t         kind;       /* LLM_F16 / LLM_CQ2 / LLM_CQ4 */
    uint16_t        group;      /* CQ group size */
    uint32_t        rows;       /* product of the leading dims */
    uint32_t        cols;       /* last dim */
    const uint8_t  *data;       /* f16 values, or packed codebook indices */
    const uint8_t  *norm;       /* CQ: fp16 norm per (row, group) */
    bool            fast_norm;  /* every norm is +0 or a positive normal fp16 */
} llm_tensor_t;

typedef struct {
    llm_tensor_t attn_norm, q, k, v, gate, out, q_norm, k_norm,
                 post_attn_norm, attn_gate, mlp_norm, d1, d2, d3;
} llm_layer_w_t;

/* fp16 vectors converted once at open (the norms as gains, 1 + scale) */
typedef struct {
    float *attn_norm, *q_norm, *k_norm, *post_attn_norm, *mlp_norm, *d1, *d2, *d3;
    float  attn_gate;           /* sigmoid already applied */
} llm_layer_f_t;

typedef struct {
    uint16_t token, name, first_arg;
    uint8_t  n_args, n_required;
} llm_tool_t;

typedef struct {
    uint16_t name, key_token, first_value;
    uint8_t  kind, required, n_values;
    int16_t  min, max;
} llm_arg_t;

typedef struct {
    uint16_t token, name;
    int16_t  value;
} llm_value_t;

typedef struct {
    uint16_t open, close, im_end, bos;
    uint16_t digit[10];
    uint16_t n_tools, n_args, n_values, n_prefix, n_suffix;
    uint16_t prefix[LLM_MAX_FIX], suffix[LLM_MAX_FIX];
    const char *user_literal;
    llm_tool_t  tools[LLM_MAX_TOOLS];
    llm_arg_t   args[LLM_MAX_GRAM_ARGS];
    llm_value_t values[LLM_MAX_VALUES];
    const char *strings;
} llm_grammar_t;

typedef struct {
    uint16_t start, len;        /* a BPE symbol: bytes in the escaped text */
} llm_sym_t;

typedef struct {
    uint16_t        n_pieces, byte_base, max_piece_bytes;
    const uint8_t  *offsets;    /* u16[n+1], little-endian */
    const uint8_t  *rank;       /* u16[n] */
    const char     *text;
    uint16_t       *hash;       /* built at open, in work memory */
    uint32_t        hash_mask;
    /* BPE scratch in work memory (too big for a task stack) */
    char           *buf;        /* LLM_MAX_TEXT * 3 bytes: spaces escaped to U+2581 */
    llm_sym_t      *sym;        /* LLM_MAX_TEXT */
    uint16_t       *pair;       /* LLM_MAX_TEXT: rank of the pair (i, i + 1) */
} llm_tok_t;

typedef struct {
    uint16_t vocab, d, layers, heads, kv_heads, head_dim, mlp_n, max_seq;
    uint16_t engram_site, engram_slots, engram_taps, engram_dilation;
    uint16_t orders[2];
    float    rope_theta, eps;
} llm_cfg_t;

/* An activation ready for CQ rows: rotated by Walsh-Hadamard per group, and
 * for the int8 kernel quantised per group (scale = max |value| / 127). */
typedef struct {
    uint32_t cols, group;
    bool     int8;
    float    f[LLM_MAX_D];
    int8_t   q[LLM_MAX_D] __attribute__((aligned(4)));
    float    scale[LLM_MAX_D / 16];
} llm_act_t;

/* Grammar decoding state (decode.py State) */
typedef enum { G_START, G_BETWEEN, G_CALL, G_INT, G_END, G_DONE } llm_phase_t;

typedef struct {
    llm_phase_t phase;
    int8_t      tool;           /* -1 before the first tool */
    uint8_t     pos;            /* next argument index (schema order) */
    uint32_t    args_set;       /* bit per argument index already written */
    int8_t      key;            /* integer argument being written, -1 = none */
    int16_t     n_digits;
    int32_t     digits;         /* value of the digits so far */
    bool        lead_zero;      /* the digits are exactly "0" */
} llm_gstate_t;

/* Static int8 scales of one projection on the accelerator (NPUS section) */
typedef struct {
    float s_in, s_out;
} llm_qscale_t;

struct ipro_llm {
    llm_cfg_t      cfg;
    llm_tensor_t   embed, final_norm_t, eg_tables, eg_key, eg_value, eg_taps;
    llm_layer_w_t  lw[LLM_MAX_LAYERS];
    llm_layer_f_t  lf[LLM_MAX_LAYERS];
    float         *final_norm, *eg_taps_f;
    float         *rope_cos, *rope_sin;     /* [LLM_CTX][head_dim / 2] */
    llm_tok_t      tok;
    llm_grammar_t  g;

    /* decoding state */
    float         *kcache, *vcache;         /* [layer][kv_head][LLM_CTX][head_dim] */
    /* the int8 kernel's KV: K rows int8 [layer][kv_head][LLM_CTX][head_dim], V transposed
     * [layer][kv_head][head_dim][LLM_CTX], one scale per K row and per V row */
    int8_t        *kq, *vq;
    float         *ks, *vs;                 /* [layer][kv_head][LLM_CTX] */
    float         *eg_v;                    /* engram value ring [ring][d] */
    uint16_t       eg_ring;
    float         *eg_prefix;               /* engram values of the prompt prefix */
    bool           prefix_ready, prefix_int8;  /* prefix KV cached, and with which kernel */
    uint16_t       tokens[LLM_CTX];
    bool           int8;                    /* IPRO_LLM_KERNEL_INT8 */
    ipro_llm_prof_t prof;
    llm_act_t      head_act;                /* final hidden state, for the head */
    /* one forward of up to LLM_BATCH tokens: [LLM_BATCH][d] (bk, bv: [..][kv dim]) */
    float         *bx, *bh, *bq, *bk, *bv, *bg, *by, *bo;
    llm_act_t     *bact;
    const llm_act_t *bactp[LLM_BATCH];
    float          z[LLM_MAX_MLP], mix[LLM_MAX_D], ux[LLM_MAX_D], uk[LLM_MAX_D];
    float          scores[LLM_CTX], scores2[LLM_CTX];   /* attention: the two heads of a KV head */
    float          final_h[LLM_MAX_D];      /* final norm of the last position */

    /* accelerator for the attention projections (llm_accel.c) */
    bool           has_npus;                /* the blob has the NPUS section */
    llm_qscale_t   npus[LLM_MAX_LAYERS][2]; /* [layer][q/k/v/gate, out] */
    const ipro_llm_accel_t *accel;
    int            accel_on, prefix_accel;  /* IPRO_LLM_ACCEL_* bits; the prefix KV's */
    int8_t        *ax, *ay;                 /* [LLM_BATCH][LLM_MAX_D], [LLM_BATCH][LLM_ACCEL_OUT * LLM_MAX_D] */
};

/* Cycle buckets for profiling the float work (-DLLM_PROF_CYCLES; off: LLM_CB(b, stmt) is stmt).
 * ipro_llm_run prints them per request. */
enum { CB_KV, CB_QK, CB_ATTEND, CB_GATE, CB_NORM, CB_FWHT, CB_SILU, CB_ENGRAM, CB_COUNT };
#if defined(LLM_PROF_CYCLES)
#if defined(__riscv)
static inline uint32_t llm_cycles(void)
{
    uint32_t c;
    __asm__ volatile("csrr %0, mcycle" : "=r"(c));
    return c;
}
#else
#define llm_cycles() ipro_llm_clock()
#endif
extern uint32_t g_llm_cycles[CB_COUNT];
#define LLM_CB(b, ...)                                                  \
    do {                                                                \
        const uint32_t c0_ = llm_cycles();                              \
        __VA_ARGS__;                                                    \
        g_llm_cycles[b] += llm_cycles() - c0_;                          \
    } while (0)
#else
#define LLM_CB(b, ...) do { __VA_ARGS__; } while (0)
#endif

/* blob / work memory */
uint32_t llm_crc32(const uint8_t *p, size_t n);
uint32_t llm_fnv1a(const char *s);
uint16_t llm_rd16(const uint8_t *p);
uint32_t llm_rd32(const uint8_t *p);

/* math */
float llm_f16(uint16_t h);
void  llm_fwht(float *x, int n);
void  llm_rmsnorm(float *out, const float *x, const float *gain, int n, float eps);

/* expf within ~1 ulp, the same code on every target: x = k ln2 + r with
 * |r| <= ln2 / 2 (Cody-Waite split of ln2, as cephes expf), e^r by its degree-6
 * Taylor polynomial (truncation 1.2e-7), 2^k written into the exponent.
 * Saturates to 0 below e^-87.3 (newlib would give denormals) and to +inf above
 * e^88; the gates and the softmax never care. newlib's expf is ~177 cycles on
 * the IPRO7 core, this one a few tens. Inline, so a loop over independent
 * elements can overlap their polynomial chains. */
static inline float llm_expf(float x)
{
    if (x > 88.0f) {
        return INFINITY;
    }
    if (x < -87.3f) {
        return 0.0f;
    }
    const int k = (int)lrintf(x * 1.44269504088896341f);
    const float r = (x - (float)k * 0.693359375f) + (float)k * 2.12194440e-4f;
    float p = 1.0f / 720.0f;
    p = p * r + 1.0f / 120.0f;
    p = p * r + 1.0f / 24.0f;
    p = p * r + 1.0f / 6.0f;
    p = p * r + 0.5f;
    p = p * r + 1.0f;
    p = p * r + 1.0f;
    const uint32_t bits = (uint32_t)(k + 127) << 23;
    float s;
    memcpy(&s, &bits, sizeof s);
    return p * s;
}

static inline float llm_sigmoid(float x)
{
    return 1.0f / (1.0f + llm_expf(-x));
}

static inline float llm_silu(float x)
{
    return x * llm_sigmoid(x);
}

/* Table versions for the int8 kernel (the board's arithmetic; the float kernel keeps the
 * functions above as the PyTorch reference). Built once by llm_lut_init() from llm_expf, so
 * every target holds the same tables; linear interpolation between knots:
 *   sigmoid  x in [-12, 12] in steps of 1/16, outside that 0 / 1 (error < 5e-5, below 1e-5 out
 *            there)
 *   exp      x in [-16, 0] in steps of 1/32 (the softmax's x - max), below -16 0 (relative
 *            error < 1.3e-4) */
#define LLM_SIG_RANGE   12.0f
#define LLM_SIG_STEPS   16.0f
#define LLM_SIG_N       385             /* 24 * 16 + 1 knots */
#define LLM_EXP_RANGE   16.0f
#define LLM_EXP_STEPS   32.0f
#define LLM_EXP_N       513             /* 16 * 32 + 1 knots */
extern float llm_sig_lut[LLM_SIG_N + 1][2];    /* value, slope to the next knot */
extern float llm_exp_lut[LLM_EXP_N + 1][2];
void llm_lut_init(void);

static inline float llm_sigmoid_lut(float x)
{
    const float t = (fminf(fmaxf(x, -LLM_SIG_RANGE), LLM_SIG_RANGE) + LLM_SIG_RANGE) * LLM_SIG_STEPS;
    const int i = (int)t;                       /* t >= 0: truncation is floor */
    return llm_sig_lut[i][0] + (t - (float)i) * llm_sig_lut[i][1];
}

static inline float llm_silu_lut(float x)
{
    return x * llm_sigmoid_lut(x);
}

/* e^x for x <= 0 */
static inline float llm_exp_neg_lut(float x)
{
    const float t = (fmaxf(x, -LLM_EXP_RANGE) + LLM_EXP_RANGE) * LLM_EXP_STEPS;
    const int i = (int)t;
    return llm_exp_lut[i][0] + (t - (float)i) * llm_exp_lut[i][1];
}

/* CQ weights (llm_cq.c) */
void  llm_cq_init(void);
bool  llm_cq_check_norms(const llm_tensor_t *w);
void  llm_act_prep(llm_act_t *a, const float *x, uint32_t cols, uint32_t group, bool int8);
void  llm_cq_matvec(float *y, const llm_tensor_t *w, const llm_act_t *a);
void  llm_cq_matmul(float *y, uint32_t y_stride, const llm_tensor_t *w,
                    const llm_act_t *const *a, uint32_t n);
float llm_cq_row_dot(const llm_tensor_t *w, uint32_t row, const llm_act_t *a);
void  llm_cq_row(float *out, const llm_tensor_t *w, uint32_t row);
void  llm_f16_vec(float *out, const llm_tensor_t *t);

/* tokenizer (llm_tok.c) */
int   llm_tok_init(llm_tok_t *t, uint16_t *hash, uint32_t hash_slots);
int   llm_normalize_query(char *out, size_t out_len, const char *in);
int   llm_bpe(const llm_tok_t *t, const char *text, uint16_t *ids, int max_ids);
int   llm_encode_prompt(const ipro_llm_t *m, const char *text, uint16_t *ids, int max_ids);
const char *llm_piece(const llm_tok_t *t, uint16_t id, uint16_t *len);

/* model (llm_model.c) */
void  llm_forward(ipro_llm_t *m, const uint16_t *tokens, int pos0, int n);   /* n <= LLM_BATCH */
float llm_logit(const ipro_llm_t *m, uint16_t token);    /* after llm_forward */
void  llm_logits_all(const ipro_llm_t *m, float *out);

/* accelerator (llm_accel.c) */
int   llm_accel_parse(ipro_llm_t *m, const uint8_t *p, uint32_t len);
int   llm_accel_qkvg_rows(const llm_cfg_t *c);
bool  llm_accel_qkvg(ipro_llm_t *m, int layer, int n);    /* bh -> bq, bk, bv, bg */
bool  llm_accel_out(ipro_llm_t *m, int layer, int n);     /* by -> bo */

/* grammar (llm_grammar.c) */
void  llm_g_start(llm_gstate_t *s);
int   llm_g_allowed(const llm_grammar_t *g, const llm_gstate_t *s, uint16_t *out, int max);
int   llm_g_step(const llm_grammar_t *g, llm_gstate_t *s, uint16_t tok);
int   llm_g_calls(const llm_grammar_t *g, const uint16_t *gen, int n, ipro_llm_result_t *res);

#endif /* LLM_INTERNAL_H */
