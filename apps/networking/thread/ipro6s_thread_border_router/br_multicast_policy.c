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

#include "br_multicast_policy.h"
#include <string.h>

bool br_multicast_eligible(const uint8_t *packet, size_t length,
                           const uint8_t mesh_prefix[8], bool primary,
                           bool ready, bool from_thread, bool listener)
{
    static const uint8_t zero[16] = {0};
    if (!packet || !mesh_prefix || !primary || !ready || length < 40 || length > 1280)
        return false;
    if ((packet[0] >> 4) != 6 ||
        length != 40u + ((unsigned)packet[4] << 8) + packet[5] || packet[7] <= 1)
        return false;
    const uint8_t *source = packet + 8;
    const uint8_t *destination = packet + 24;
    unsigned scope = destination[1] & 15u;
    if (destination[0] != 0xff || scope <= 3 || scope == 15)
        return false;
    if (source[0] == 0xff ||
        (source[0] == 0xfe && (source[1] & 0xc0u) == 0x80) ||
        memcmp(source, mesh_prefix, 8) == 0 ||
        (memcmp(source, zero, 15) == 0 && source[15] <= 1) ||
        (memcmp(source, zero, 10) == 0 && source[10] == 0xff && source[11] == 0xff))
        return false;
    return from_thread || listener;
}
