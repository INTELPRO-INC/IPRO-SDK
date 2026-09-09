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
#include <stdio.h>
#include "ipro6_avrcp_decode.h"

int main(void)
{
    struct ipro6_avrcp_play_status status;
    const uint8_t play[] = {0, 0, 0xea, 0x60, 0, 0, 3, 0xe8, 1};
    const uint8_t attrs[] = {1, 0, 0, 0, 1, 0, 106, 0, 3, 'A', 'B', 'C'};
    const uint8_t empty[] = {0};
    uint8_t malformed[] = {1, 0, 0, 0, 1, 0, 106, 0xff, 0xff};

    assert(ipro6_avrcp_decode_play(play, sizeof(play), &status));
    assert(status.length_ms == 60000 && status.position_ms == 1000 && status.state == 1);
    for (size_t n = 0; n < sizeof(play); ++n) {
        assert(!ipro6_avrcp_decode_play(play, n, &status));
    }
    assert(!ipro6_avrcp_decode_play(NULL, sizeof(play), &status));
    assert(!ipro6_avrcp_decode_play(play, sizeof(play), NULL));
    assert(ipro6_avrcp_validate_attrs(attrs, sizeof(attrs)));
    assert(ipro6_avrcp_validate_attrs(empty, sizeof(empty)));
    assert(!ipro6_avrcp_validate_attrs(NULL, 1));
    for (size_t n = 0; n < sizeof(attrs); ++n) {
        assert(!ipro6_avrcp_validate_attrs(attrs, n));
    }
    assert(!ipro6_avrcp_validate_attrs(malformed, sizeof(malformed)));
    malformed[0] = 0;
    assert(!ipro6_avrcp_validate_attrs(malformed, sizeof(malformed)));
    puts("AVRCP decode PASS");
    return 0;
}
