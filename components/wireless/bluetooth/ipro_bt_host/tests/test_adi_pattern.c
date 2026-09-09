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
#include <string.h>
#include "board/adi_pattern.h"

int main(void)
{
    uint8_t payload[IPRO_ADI_PATTERN_LEN];
    uint16_t sequence = 0;

    ipro_adi_pattern_fill(payload, 0);
    assert(ipro_adi_pattern_check(payload, sizeof(payload), &sequence));
    assert(sequence == 0);

    ipro_adi_pattern_fill(payload, UINT16_MAX);
    assert(ipro_adi_pattern_check(payload, sizeof(payload), &sequence));
    assert(sequence == UINT16_MAX);

    payload[17] ^= 1U;
    assert(!ipro_adi_pattern_check(payload, sizeof(payload), &sequence));
    payload[0] = 'X';
    assert(!ipro_adi_pattern_check(payload, sizeof(payload), &sequence));
    assert(!ipro_adi_pattern_check(payload, sizeof(payload) - 1U, &sequence));
    assert(!ipro_adi_pattern_check(NULL, sizeof(payload), &sequence));
    assert(!ipro_adi_pattern_check(payload, sizeof(payload), NULL));
    return 0;
}
