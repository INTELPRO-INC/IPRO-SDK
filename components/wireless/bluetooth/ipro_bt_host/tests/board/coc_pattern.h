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

#ifndef IPRO_COC_PATTERN_H
#define IPRO_COC_PATTERN_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define COC_TEST_MTU 1024u
static inline uint8_t coc_pattern_byte(size_t i, size_t len, uint16_t seq)
{
    if (i == 0) return (uint8_t)len;
    if (i == 1) return (uint8_t)(len >> 8);
    if (i == 2) return (uint8_t)seq;
    if (i == 3) return (uint8_t)(seq >> 8);
    return (uint8_t)((i * 73u) ^ (i >> 3) ^ seq ^ (seq >> 8));
}
static inline bool coc_pattern_fill(uint8_t *data, size_t len, uint16_t seq)
{
    if (len < 4 || len > COC_TEST_MTU) return false;
    for (size_t i = 0; i < len; ++i) data[i] = coc_pattern_byte(i, len, seq);
    return true;
}
static inline bool coc_pattern_check(const uint8_t *data, size_t len, uint16_t seq)
{
    if (len < 4 || len > COC_TEST_MTU) return false;
    for (size_t i = 0; i < len; ++i)
        if (data[i] != coc_pattern_byte(i, len, seq)) return false;
    return true;
}
#endif
