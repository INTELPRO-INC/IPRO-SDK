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
#include "board/pawr_diagnose.h"
int main(void)
{
    uint8_t bytes[PAWR_PATTERN_MAX];
    struct pawr_diagnosis d;
    for (unsigned sub = 0; sub < 4; sub++) {
        assert(pawr_pattern_fill(bytes, sizeof(bytes), sub, 123));
        d = pawr_diagnose(bytes, pawr_pattern_size(sub), sub);
        assert(d.self_valid && d.encoded_sub == sub && d.first_diff == -1);
        d = pawr_diagnose(bytes, pawr_pattern_size(sub), (sub + 1) % 4);
        assert(d.self_valid && d.encoded_sub == sub);
        bytes[7] ^= 1;
        d = pawr_diagnose(bytes, pawr_pattern_size(sub), sub);
        assert(!d.self_valid && d.first_diff == 7 && d.actual != d.expected);
    }
    d = pawr_diagnose(NULL, 0, 0);
    assert(!d.self_valid && d.encoded_sub == 255 && d.first_diff == -1);
    d = pawr_diagnose(bytes, 3, 0);
    assert(!d.self_valid && d.encoded_sub == 255);
    return 0;
}
