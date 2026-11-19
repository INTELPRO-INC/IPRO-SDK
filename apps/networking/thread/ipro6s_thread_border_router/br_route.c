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

#include <string.h>
#include "br_route.h"

bool br_prefix_match(const uint8_t *address, const uint8_t *prefix, uint8_t bits)
{
    if (!address || !prefix || bits == 0 || bits > 128) return false;
    unsigned bytes = bits / 8;
    if (memcmp(address, prefix, bytes) != 0) return false;
    unsigned remainder = bits % 8;
    if (!remainder) return true;
    uint8_t mask = (uint8_t)(0xffu << (8 - remainder));
    return (address[bytes] & mask) == (prefix[bytes] & mask);
}

bool br_route_to_thread(const uint8_t *address, const struct br_route_prefix *prefixes,
                        size_t count, const struct br_route_prefix *domain)
{
    if (!address || !prefixes) return false;
    if (domain && br_prefix_match(address, domain->address, domain->length)) return false;
    for (size_t i = 0; i < count; ++i) {
        if (br_prefix_match(address, prefixes[i].address, prefixes[i].length)) return true;
    }
    return false;
}

size_t br_domain_pio(const struct br_route_prefix *domain, bool withdraw,
                     uint8_t *output, size_t capacity)
{
    if (!domain || !output || capacity < 32 || domain->length != 64 ||
        ((domain->address[0] & 0xe0) != 0x20 && (domain->address[0] & 0xfe) != 0xfc)) return 0;
    uint8_t option[32] = {3, 4, 64, 0x80}; /* PIO, L=1, A=0 */
    if (!withdraw) { option[6] = 0x07; option[7] = 0x08; } /* Valid 1800s. */
    /* Preferred lifetime zero: no address creation or favored LAN prefix. */
    memcpy(option + 16, domain->address, 8);
    memcpy(output, option, sizeof(option));
    return sizeof(option);
}
