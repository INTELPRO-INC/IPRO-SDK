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

#ifndef BR_MULTICAST_POLICY_H
#define BR_MULTICAST_POLICY_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Policy only; does not validate extension headers, transport checksums, or
 * authenticate a source. Caller supplies one normalized IPv6 datagram and
 * current /64 mesh prefix. Never substitute this for stack packet validation.
 * ready means BOTH infrastructure up and Thread attached. Listener must come
 * from the live MLR table, not merely a local application subscription.
 * Does not mutate the packet: egress must decrement hop limit exactly once. */
bool br_multicast_eligible(const uint8_t *packet, size_t length,
                           const uint8_t mesh_prefix[8], bool primary,
                           bool ready, bool from_thread, bool listener);
#endif
