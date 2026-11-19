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

#pragma once
#include <stdbool.h>
#include <openthread/instance.h>
#include "br_dns64.h"
/* OT-owner context, outside the lwIP core lock. */
void br_dns_upstream_process(otInstance *instance, bool forwarding_ready);
/* OT-owner context after initial process(), outside the lwIP lock. Copies up
 * to eight extra ranges; NULL/0 restores only the default mapped exclusion.
 * Accepted updates invalidate active DNS64 queries at the next process/RX.
 * Returns false for invalid arguments, wrong owner, or disabled DNS64. */
bool br_dns_upstream_set_dns64_exclusions(otInstance *instance,
    const struct br_dns64_prefix *prefixes, size_t count);
