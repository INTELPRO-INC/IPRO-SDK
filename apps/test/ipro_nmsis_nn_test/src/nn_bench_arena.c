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
 * @file nn_bench_arena.c
 * @brief Bump allocator so the benchmark can place its buffers deliberately.
 *
 * Why this exists: with CONFIG_USE_PSRAM=y the SDK heap lands in PSRAM - the
 * linker symbol is called __ocram_heap_start but resolves to 0x1e002090, which
 * is inside the psram region (0x1e000000, 8MB), not ocram (0x11010000, 256KB).
 * A malloc-based benchmark therefore measures PSRAM latency, and the naive u8
 * kernel - which re-reads its input for every output pixel - is punished far
 * harder by that than the im2col s8 kernel is. The speedup ratio comes out
 * flattering rather than true.
 *
 * Running both placements from one image keeps the comparison honest.
 */

#include <stdint.h>
#include <string.h>

#include "nn_bench.h"

/* Sized for the largest shape in k_shapes: 1x1 32x32x32->32 needs
 * 32768 in + 1024 filt + 32768 out + 384 params + 128 scratch = 67072 B.
 * Rounded up for alignment slack across six separate allocations. */
#define NN_BENCH_ARENA_SIZE (80 * 1024)

static __attribute__((aligned(16), section(".ocram_bss")))
uint8_t s_ocram_arena[NN_BENCH_ARENA_SIZE];

/* PSRAM side: plain .bss, which for a USE_PSRAM build lives in psram. */
static __attribute__((aligned(16)))
uint8_t s_psram_arena[NN_BENCH_ARENA_SIZE];

static uint8_t *s_base = s_ocram_arena;
static size_t   s_used;

void nn_bench_arena_select(nn_bench_mem_t mem)
{
    s_base = (mem == NN_BENCH_MEM_PSRAM) ? s_psram_arena : s_ocram_arena;
    s_used = 0;
}

const void *nn_bench_arena_base(void)
{
    return s_base;
}

void nn_bench_arena_reset(void)
{
    s_used = 0;
}

void *nn_bench_alloc(size_t n)
{
    const size_t aligned = (n + 15u) & ~(size_t)15u;
    if (s_used + aligned > NN_BENCH_ARENA_SIZE) {
        return NULL;
    }
    void *p = s_base + s_used;
    s_used += aligned;
    return p;
}

size_t nn_bench_arena_used(void)
{
    return s_used;
}
