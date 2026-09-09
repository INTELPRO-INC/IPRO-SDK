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
#include <stdint.h>
#include "board/coc_pattern.h"
int main(void)
{
    uint8_t data[1025];
    assert(!coc_pattern_fill(data, 3, 1));
    assert(!coc_pattern_fill(data, 1025, 1));
    assert(!coc_pattern_check(data, 0, 1));
    for (unsigned len = 4; len <= 1024; ++len) {
        assert(coc_pattern_fill(data, len, 0x1234));
        assert(coc_pattern_check(data, len, 0x1234));
        assert(!coc_pattern_check(data, len, 0x1235));
        for (unsigned i = 0; i < len; ++i) {
            data[i] ^= 1;
            assert(!coc_pattern_check(data, len, 0x1234));
            data[i] ^= 1;
        }
        assert(!coc_pattern_check(data, len - 1, 0x1234));
    }
    return 0;
}
