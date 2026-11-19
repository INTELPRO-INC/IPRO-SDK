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

#ifndef BR_DOMAIN_RA_H
#define BR_DOMAIN_RA_H

#include "br_route.h"

/* OT-task-owned state. Zero-initialize; no concurrent or nested RA preparation.
 * 'advertised' means accepted by the platform send, not delivery acknowledged
 * by every LAN host. A failed send retains the previous withdrawal obligation. */
struct br_domain_ra {
    struct br_route_prefix desired;
    struct br_route_prefix advertised;
    struct br_route_prefix prepared;
    bool pending;
};

bool br_domain_ra_set(struct br_domain_ra *state, const struct br_route_prefix *domain);
/* Atomic on failure, including output/length. At most two 32-byte PIOs.
 * Every successful prepare must be paired with complete, even on TX failure. */
bool br_domain_ra_prepare(struct br_domain_ra *state, bool final, uint8_t *output,
                          size_t capacity, size_t *length);
void br_domain_ra_complete(struct br_domain_ra *state, bool sent);

#endif
