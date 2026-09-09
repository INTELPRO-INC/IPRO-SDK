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
 * @file bench_s8.c
 * @brief Upstream NMSIS-NN int8 convolution, timed.
 *
 * DSP acceleration is not selected here - riscv_nn_math_types.h keys off the
 * compiler-predefined __riscv_xxldsp*, so building this app with the vendor
 * toolchain's march is what turns kmada/sunpkd820 on. Nothing to configure.
 *
 * MUST NOT include any components/ai header: both define nmsis_nn_conv_params
 * with different layouts. See nn_bench.h.
 */

#include "nmsis_nn/riscv_nnfunctions.h"

#include <stdio.h>
#include <string.h>

#include "nn_bench.h"

/* Deterministic filler so the checksum is comparable across runs/builds. */
static void fill_pattern(int8_t *p, int32_t n, int32_t seed)
{
    for (int32_t i = 0; i < n; i++) {
        seed = seed * 1103515245 + 12345;
        p[i] = (int8_t)((seed >> 16) & 0x7f) - 64;
    }
}

bool nn_bench_s8(const nn_bench_shape_t *s, nn_bench_result_t *out)
{
    memset(out, 0, sizeof(*out));
    nn_bench_arena_reset();

    const int32_t out_h = (s->in_h + 2 * s->pad - s->k_h) / s->stride + 1;
    const int32_t out_w = (s->in_w + 2 * s->pad - s->k_w) / s->stride + 1;

    nmsis_nn_dims in_dims   = { 1, s->in_h, s->in_w, s->in_c };
    nmsis_nn_dims filt_dims = { s->out_c, s->k_h, s->k_w, s->in_c };
    nmsis_nn_dims bias_dims = { 1, 1, 1, s->out_c };
    nmsis_nn_dims out_dims  = { 1, out_h, out_w, s->out_c };
    nmsis_nn_dims ups_dims  = { 1, 1, 1, 1 };

    const int32_t in_n   = s->in_h * s->in_w * s->in_c;
    const int32_t filt_n = s->out_c * s->k_h * s->k_w * s->in_c;
    const int32_t out_n  = out_h * out_w * s->out_c;

    const int32_t scratch = riscv_convolve_s8_get_buffer_size(&in_dims, &filt_dims);
    out->scratch = scratch;

    int8_t  *in   = nn_bench_alloc((size_t)in_n);
    int8_t  *filt = nn_bench_alloc((size_t)filt_n);
    int8_t  *outb = nn_bench_alloc((size_t)out_n);
    int32_t *bias = nn_bench_alloc((size_t)s->out_c * sizeof(int32_t));
    int32_t *mult = nn_bench_alloc((size_t)s->out_c * sizeof(int32_t));
    int32_t *shft = nn_bench_alloc((size_t)s->out_c * sizeof(int32_t));
    void    *buf  = scratch > 0 ? nn_bench_alloc((size_t)scratch) : NULL;

    if (!in || !filt || !outb || !bias || !mult || !shft || (scratch > 0 && !buf)) {
        printf("  [s8] OOM (in=%ld filt=%ld out=%ld scratch=%ld)\r\n",
               (long)in_n, (long)filt_n, (long)out_n, (long)scratch);
        goto done;
    }

    fill_pattern(in, in_n, 1);
    fill_pattern(filt, filt_n, 2);
    for (int32_t i = 0; i < s->out_c; i++) {
        bias[i] = 0;
        mult[i] = 0x40000000;   /* ~0.5 in Q31 */
        shft[i] = -8;
    }

    nmsis_nn_context ctx = { buf, scratch };
    nmsis_nn_tile stride = { s->stride, s->stride };
    nmsis_nn_tile pad    = { s->pad, s->pad };
    nmsis_nn_tile dil    = { 1, 1 };
    nmsis_nn_activation act = { -128, 127 };
    nmsis_nn_conv_params cp = { -8, 12, stride, pad, dil, act };
    nmsis_nn_per_channel_quant_params qp = { mult, shft };

    const uint32_t t0 = nn_bench_cycles();
    const riscv_nmsis_nn_status st =
        riscv_convolve_s8(&ctx, &cp, &qp, &in_dims, in, &filt_dims, filt,
                          &bias_dims, bias, &ups_dims, &out_dims, outb);
    const uint32_t t1 = nn_bench_cycles();

    if (st != RISCV_NMSIS_NN_SUCCESS) {
        printf("  [s8] kernel returned %d\r\n", (int)st);
        goto done;
    }

    int32_t sum = 0;
    for (int32_t i = 0; i < out_n; i++) {
        sum += outb[i];
    }

    out->ran      = true;
    out->cycles   = t1 - t0;
    out->checksum = sum;

done:
    nn_bench_arena_reset();
    return out->ran;
}
