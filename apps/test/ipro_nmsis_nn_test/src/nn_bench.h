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
 * @file nn_bench.h
 * @brief A/B benchmark interface for the two int8/uint8 conv kernel worlds.
 *
 * The two implementations live in separate translation units on purpose:
 * components/ai and components/3rdparty/nmsis_nn each define a struct named
 * nmsis_nn_conv_params with a DIFFERENT layout (the components/ai one carries
 * an extra filter_offset field), so their headers can never be included into
 * one TU. Everything crossing this boundary is plain C.
 */
#ifndef __NN_BENCH_H__
#define __NN_BENCH_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** One convolution geometry, shared by both kernels. */
typedef struct {
    const char *name;
    int32_t in_h, in_w, in_c;
    int32_t k_h,  k_w,  out_c;
    int32_t stride, pad;
} nn_bench_shape_t;

/** Result of one timed run. */
typedef struct {
    bool     ran;         /**< false when the kernel refused / was unavailable */
    uint32_t cycles;      /**< mcycle delta for the kernel call alone */
    int32_t  checksum;    /**< sum of output bytes - guards against no-op runs */
    int32_t  scratch;     /**< scratch bytes the kernel asked for */
} nn_bench_result_t;

/** Upstream NMSIS-NN int8 path (DSP-accelerated when built with xxldsp). */
bool nn_bench_s8(const nn_bench_shape_t *s, nn_bench_result_t *out);

/** components/ai uint8 path (plain C direct convolution). */
bool nn_bench_u8(const nn_bench_shape_t *s, nn_bench_result_t *out);

/** Which memory the benchmark buffers should live in. */
typedef enum {
    NN_BENCH_MEM_OCRAM = 0,   /**< .ocram_bss - 0x11010000, 256KB, fast */
    NN_BENCH_MEM_PSRAM,       /**< plain .bss - 0x1e000000 on a USE_PSRAM build */
} nn_bench_mem_t;

void        nn_bench_arena_select(nn_bench_mem_t mem);
const void *nn_bench_arena_base(void);
void        nn_bench_arena_reset(void);
void       *nn_bench_alloc(size_t n);
size_t      nn_bench_arena_used(void);

/** Read the RISC-V M-mode cycle counter. */
static inline uint32_t nn_bench_cycles(void)
{
    uint32_t v;
    __asm__ volatile("csrr %0, mcycle" : "=r"(v));
    return v;
}

#endif /* __NN_BENCH_H__ */
