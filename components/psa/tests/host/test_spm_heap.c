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
 * Host test of spm_heap.c: first fit, split, merge, reuse, wipe on
 * free, zeroed calloc, exhaustion and refused frees.
 */
#include <stdint.h>
#include <string.h>

#include "check.h"
#include "spm_heap.h"

static size_t used(void)
{
    size_t u;
    size_t p;

    spm_heap_stats(&u, &p);
    return u;
}

static int all(const void *p, size_t n, uint8_t v)
{
    const uint8_t *b = p;

    for (size_t i = 0; i < n; i++) {
        if (b[i] != v) {
            return 0;
        }
    }
    return 1;
}

static void test_basic(void)
{
    uint8_t *a = spm_malloc(1);
    uint8_t *b = spm_malloc(13);
    size_t peak;
    size_t u;

    check(a != NULL && b != NULL && ((uintptr_t)a & 7u) == 0 &&
          ((uintptr_t)b & 7u) == 0, "payloads 8-byte aligned");
    check(b == a + 16, "1 byte = header + 8: next block 16 bytes on");
    check(used() == 16u + 24u, "used counts headers and rounding");
    memset(b, 0xA5, 13);
    spm_free(b);
    check(all(b, 16, 0), "free wipes the payload");
    check(spm_malloc(16) == b, "first fit reuses the freed block");
    spm_free(b);
    spm_free(a);
    spm_heap_stats(&u, &peak);
    check(u == 0 && peak == 40u, "all freed: used 0, peak kept");
}

static void test_merge(void)
{
    uint8_t *a = spm_malloc(1000);
    uint8_t *b = spm_malloc(1000);
    uint8_t *c = spm_malloc(1000);
    uint8_t *d;

    spm_free(a);
    spm_free(b);
    d = spm_malloc(2000);
    check(d == a, "two freed neighbours merge for a larger request");
    spm_free(d);
    spm_free(c);
    d = spm_malloc(SPM_HEAP_SIZE - 8u);
    check(d == a, "everything merges back into one block");
    spm_free(d);
}

static void test_calloc(void)
{
    uint8_t *a = spm_malloc(64);
    uint8_t *b;
    void *z0;
    void *z1;

    memset(a, 0xFF, 64);
    spm_free(a);
    memset(a, 0xFF, 64);
    b = spm_calloc(8, 8);
    check(b == a && all(b, 64, 0), "calloc zeroes reused memory");
    check(spm_calloc(SIZE_MAX / 2u, 4) == NULL, "calloc n*size overflow");
    z0 = spm_calloc(0, 4);
    z1 = spm_calloc(0, 4);
    check(z0 != NULL && z1 != NULL && z0 != z1,
          "zero-size requests get distinct blocks");
    spm_free(z0);
    spm_free(z1);
    spm_free(b);
}

static void test_exhaustion(void)
{
    static uint8_t *p[SPM_HEAP_SIZE / 16u + 1u];
    size_t n = 0;
    size_t base = used();
    uint8_t *x;

    check(spm_malloc(SPM_HEAP_SIZE) == NULL, "larger than the arena");
    while (n < sizeof(p) / sizeof(p[0]) && (p[n] = spm_malloc(8)) != NULL) {
        n++;
    }
    check(n == (SPM_HEAP_SIZE - base) / 16u, "8-byte blocks fill the arena");
    check(spm_malloc(1) == NULL, "full arena: NULL");
    x = p[n / 2];
    spm_free(x);
    check(spm_malloc(9) == NULL && spm_malloc(8) == x,
          "only a fitting hole is used");
    for (size_t i = 0; i < n; i++) {
        spm_free(p[i]);
    }
    check(used() == base, "free of every block returns to baseline");
}

static void test_refused_free(void)
{
    uint8_t *a = spm_malloc(32);
    static uint8_t outside[32];
    size_t u = used();

    spm_free(a);
    spm_free(a);
    check(used() == u - 40u, "double free is ignored");
    a = spm_malloc(32);
    spm_free(a + 8);
    spm_free(outside);
    spm_free(NULL);
    check(used() == u, "interior, foreign and NULL frees are ignored");
    spm_free(a);
}

static void test_size(void)
{
    void *p = spm_malloc(SPM_HEAP_SIZE - 8u);

    check(SPM_HEAP_SIZE == 16u * 1024u,
          "arena 16 KB (host crypto peak 9256 B)");
    check(p != NULL, "one block takes the whole arena but its header");
    spm_free(p);
    check(used() == 0, "and frees back to empty");
}

int main(void)
{
    test_basic();
    test_merge();
    test_calloc();
    test_exhaustion();
    test_refused_free();
    test_size();
    return check_summary();
}
