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

#ifndef IPRO_BR_ONLINK_H
#define IPRO_BR_ONLINK_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Local prefix, three old local prefixes, and Domain Prefix transitions. */
#define BR_ONLINK_CAPACITY 8
struct br_onlink_entry {
    uint8_t prefix[8];
    uint32_t updated_ms;
    uint32_t valid_ms;
};
struct br_onlink_cache { struct br_onlink_entry entries[BR_ONLINK_CAPACITY]; };
/* Only locally transmitted RA payloads, after successful transmission.
 * Preferred lifetime does not affect on-link reachability. All calls are
 * serialized by the lwIP core lock. Malformed input leaves the cache intact. */
bool br_onlink_record(struct br_onlink_cache *cache, const uint8_t *ra,
                      size_t length, uint32_t now_ms);
bool br_onlink_expire(struct br_onlink_cache *cache, uint32_t now_ms);
bool br_onlink_contains(const struct br_onlink_cache *cache,
                        const uint8_t address[16], uint32_t now_ms);
#endif
