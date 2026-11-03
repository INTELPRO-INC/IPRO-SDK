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

/*
 * First-fit heap over SPM_HEAP_SIZE bytes. Blocks are runs of 8-byte
 * units, the first unit is the header. Free neighbours merge forward
 * on free and while malloc walks the arena.
 */
#include <stdint.h>
#include <string.h>

#include "spm_heap.h"

struct spm_blk {
    uint32_t units;             /* block length, header included */
    uint32_t used;
};

#define SPM_UNIT   ((uint32_t)sizeof(struct spm_blk))
#define SPM_UNITS  (SPM_HEAP_SIZE / SPM_UNIT)

static struct spm_blk spm_heap[SPM_UNITS] __attribute__((aligned(8)));
static size_t spm_heap_used;
static size_t spm_heap_peak;

_Static_assert(sizeof(struct spm_blk) == 8u, "heap unit is 8 bytes");

static void spm_heap_init(void)
{
    if (spm_heap[0].units == 0) {
        spm_heap[0].units = SPM_UNITS;
        spm_heap[0].used = 0;
    }
}

/* units of block i, 0 when the header is out of range */
static uint32_t spm_blk_units(uint32_t i)
{
    uint32_t u = spm_heap[i].units;

    return (u == 0 || u > SPM_UNITS - i) ? 0 : u;
}

static void spm_merge(uint32_t i)
{
    uint32_t next = i + spm_heap[i].units;

    while (next < SPM_UNITS && !spm_heap[next].used &&
           spm_blk_units(next) != 0) {
        spm_heap[i].units += spm_heap[next].units;
        spm_heap[next].units = 0;
        next = i + spm_heap[i].units;
    }
}

void *spm_malloc(size_t n)
{
    uint32_t need;
    uint32_t u;

    if (n > SPM_HEAP_SIZE - SPM_UNIT) {
        return NULL;
    }
    need = 1u + (uint32_t)((n + SPM_UNIT - 1u) / SPM_UNIT);
    if (need < 2u) {
        need = 2u;
    }
    spm_heap_init();
    for (uint32_t i = 0; i < SPM_UNITS; i += u) {
        u = spm_blk_units(i);
        if (u == 0) {
            return NULL;
        }
        if (spm_heap[i].used) {
            continue;
        }
        spm_merge(i);
        u = spm_heap[i].units;
        if (u < need) {
            continue;
        }
        if (u > need) {
            spm_heap[i + need].units = u - need;
            spm_heap[i + need].used = 0;
            spm_heap[i].units = need;
        }
        spm_heap[i].used = 1;
        spm_heap_used += need * SPM_UNIT;
        if (spm_heap_used > spm_heap_peak) {
            spm_heap_peak = spm_heap_used;
        }
        return &spm_heap[i + 1u];
    }
    return NULL;
}

void *spm_calloc(size_t n, size_t size)
{
    void *p;

    if (size != 0 && n > SIZE_MAX / size) {
        return NULL;
    }
    p = spm_malloc(n * size);
    if (p != NULL) {
        memset(p, 0, n * size);
    }
    return p;
}

void spm_free(void *p)
{
    uintptr_t addr = (uintptr_t)p;
    uint32_t u;

    if (p == NULL || spm_heap[0].units == 0) {
        return;
    }
    for (uint32_t i = 0; i < SPM_UNITS; i += u) {
        uintptr_t payload = (uintptr_t)&spm_heap[i + 1u];

        u = spm_blk_units(i);
        if (u == 0 || payload > addr) {
            return;
        }
        if (payload == addr) {
            if (spm_heap[i].used) {
                memset(p, 0, (u - 1u) * SPM_UNIT);
                spm_heap[i].used = 0;
                spm_heap_used -= u * SPM_UNIT;
                spm_merge(i);
            }
            return;
        }
    }
}

void spm_heap_stats(size_t *used, size_t *peak)
{
    *used = spm_heap_used;
    *peak = spm_heap_peak;
}
