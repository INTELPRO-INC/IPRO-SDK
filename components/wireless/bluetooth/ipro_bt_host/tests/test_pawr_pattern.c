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

#include <assert.h>
#include <string.h>
#include "board/pawr_pattern.h"
int main(void)
{
    uint8_t data[PAWR_PATTERN_MAX] = {0}, other[PAWR_PATTERN_MAX] = {0};
    const unsigned lengths[] = {24, 40, 64, 120};
    for (unsigned sub = 0; sub < PAWR_PATTERN_SUBEVENTS; sub++) {
        assert(pawr_pattern_size(sub) == lengths[sub]);
        assert(pawr_pattern_fill(data, sizeof(data), sub, 17));
        assert(pawr_pattern_check(data, lengths[sub], sub));
        assert(!pawr_pattern_check(data, lengths[sub] - 1, sub));
        assert(!pawr_pattern_check(data, lengths[sub], (sub + 1) % 4));
        assert(!pawr_pattern_fill(data, lengths[sub] - 1, sub, 17));
        memcpy(other, data, sizeof(data));
        for (unsigned i = 0; i < lengths[sub]; i++) {
            other[i] ^= 1;
            assert(!pawr_pattern_check(other, lengths[sub], sub));
            other[i] ^= 1;
        }
        assert(pawr_pattern_fill(other, sizeof(other), sub, 18));
        assert(memcmp(data, other, lengths[sub]) != 0);
        assert(pawr_pattern_check(other, lengths[sub], sub));
        assert(pawr_pattern_fill(other, sizeof(other), sub, UINT16_MAX));
        assert(pawr_pattern_check(other, lengths[sub], sub));
    }
    pawr_long_payloads = true;
    for (unsigned sub = 0; sub < PAWR_PATTERN_SUBEVENTS; sub++) {
        unsigned length = 244 + sub;
        assert(pawr_pattern_size(sub) == length);
        assert(pawr_pattern_fill(data, sizeof(data), sub, 19));
        assert(pawr_pattern_check(data, length, sub));
        assert(!pawr_pattern_check(data, length - 1, sub));
        for (unsigned i = 0; i < length; i++) {
            data[i] ^= 1;
            assert(!pawr_pattern_check(data, length, sub));
            data[i] ^= 1;
        }
    }
    pawr_long_payloads = false;
    assert(!pawr_pattern_fill(NULL, 120, 0, 17));
    assert(!pawr_pattern_check(NULL, 24, 0));
    assert(!pawr_pattern_fill(data, 120, 4, 17));
    assert(!pawr_pattern_check(data, 24, 4));
    assert(!pawr_pattern_size(4));
    return 0;
}
