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

#ifndef IPRO6_AVRCP_DECODE_H
#define IPRO6_AVRCP_DECODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ipro6_avrcp_play_status {
    uint32_t length_ms;
    uint32_t position_ms;
    uint8_t state;
};

static inline uint32_t ipro6_avrcp_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8) | p[3];
}

static inline bool ipro6_avrcp_decode_play(const uint8_t *p, size_t len,
                                          struct ipro6_avrcp_play_status *out)
{
    if (p == NULL || out == NULL || len != 9) {
        return false;
    }
    out->length_ms = ipro6_avrcp_be32(p);
    out->position_ms = ipro6_avrcp_be32(p + 4);
    out->state = p[8];
    return true;
}

/* Validate the complete variable-length response before displaying any entry.
 * Values are wire bytes, never NUL-terminated strings or aligned C structs. */
static inline bool ipro6_avrcp_validate_attrs(const uint8_t *p, size_t len)
{
    if (p == NULL || len == 0) {
        return false;
    }
    unsigned count = *p++;
    --len;
    while (count-- != 0) {
        if (len < 8) {
            return false;
        }
        size_t text_len = ((size_t)p[6] << 8) | p[7];
        if (text_len > len - 8) {
            return false;
        }
        p += 8 + text_len;
        len -= 8 + text_len;
    }
    return len == 0;
}

#endif
