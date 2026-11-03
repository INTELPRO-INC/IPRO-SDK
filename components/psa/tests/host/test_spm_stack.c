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
 * Host test of spm_stack.c: the free count of a stack array filled
 * with SPM_STACK_FILL and then used from the top.
 */
#include <stdint.h>

#include "check.h"
#include "spm_stack.h"

#define WORDS 64u

static uint32_t g_stack[WORDS];

static void fill(void)
{
    for (uint32_t i = 0; i < WORDS; i++) {
        g_stack[i] = SPM_STACK_FILL;
    }
}

/* the top n words of g_stack hold stack data */
static void use_top(uint32_t n)
{
    for (uint32_t i = WORDS - n; i < WORDS; i++) {
        g_stack[i] = 0x1000u + i;
    }
}

static uint32_t free_bytes(void)
{
    return spm_stack_min_free(g_stack, g_stack + WORDS);
}

int main(void)
{
    fill();
    check(free_bytes() == 4u * WORDS, "untouched stack: all 256 B free");
    use_top(5);
    check(free_bytes() == 4u * (WORDS - 5u), "top 5 words used: 236 B");
    g_stack[10] = 0;
    check(free_bytes() == 4u * 10u,
          "word 10 written: 40 B, pattern above it ignored");
    fill();
    use_top(WORDS);
    check(free_bytes() == 0, "every word used: 0 B");
    fill();
    g_stack[WORDS - 1u] = SPM_STACK_FILL ^ 1u;
    check(free_bytes() == 4u * (WORDS - 1u), "only the top word: 252 B");
    check(spm_stack_min_free(g_stack, g_stack) == 0, "empty range: 0 B");
    return check_summary();
}
