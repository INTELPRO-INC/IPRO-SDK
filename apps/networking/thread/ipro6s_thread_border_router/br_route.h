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

#ifndef IPRO_BR_ROUTE_H
#define IPRO_BR_ROUTE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
struct br_route_prefix {
    uint8_t address[16];
    uint8_t length;
};
bool br_prefix_match(const uint8_t *address, const uint8_t *prefix, uint8_t bits);
/* Ordinary on-mesh routes only. The selected shared Domain Prefix takes
 * precedence over any overlapping route. Local addresses and future live
 * ND-proxy /128 authorizations must be resolved separately by the caller. */
bool br_route_to_thread(const uint8_t *address, const struct br_route_prefix *prefixes,
                        size_t count, const struct br_route_prefix *domain);
/* Encode a /64 ULA/GUA Domain Prefix PIO: on-link, never SLAAC. Withdraw
 * uses zero lifetime. Returns 32 or zero without changing output on error. */
size_t br_domain_pio(const struct br_route_prefix *domain, bool withdraw,
                     uint8_t *output, size_t capacity);
#endif
