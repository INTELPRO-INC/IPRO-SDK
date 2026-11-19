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

#include "br_domain_ra.h"
#include <string.h>

bool br_domain_ra_set(struct br_domain_ra *state, const struct br_route_prefix *domain)
{
    struct br_route_prefix canonical = {0};
    uint8_t option[32];
    if (!state || !domain) return false;
    if (domain->length) {
        if (!br_domain_pio(domain, false, option, sizeof(option))) return false;
        canonical.length = 64;
        memcpy(canonical.address, domain->address, 8);
    }
    state->desired = canonical;
    return true;
}

bool br_domain_ra_prepare(struct br_domain_ra *state, bool final, uint8_t *output,
                          size_t capacity, size_t *length)
{
    uint8_t options[64];
    size_t used = 0;
    struct br_route_prefix next = {0};
    if (!state || !output || !length || state->pending) return false;
    if (!final) next = state->desired;
    if (state->advertised.length != next.length ||
        memcmp(state->advertised.address, next.address, 16) != 0)
        used = br_domain_pio(&state->advertised, true, options, sizeof(options));
    used += br_domain_pio(&next, false, options + used, sizeof(options) - used);
    if (capacity < used) return false;
    memcpy(output, options, used);
    *length = used;
    state->prepared = next;
    state->pending = true;
    return true;
}

void br_domain_ra_complete(struct br_domain_ra *state, bool sent)
{
    if (!state || !state->pending) return;
    if (sent) state->advertised = state->prepared;
    state->pending = false;
}
