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

#ifndef BR_ND_PROXY_PACKET_H
#define BR_ND_PROXY_PACKET_H
#include <stddef.h>
#include <stdint.h>

#define BR_ND_PROXY_NA_SIZE 72
#define BR_DUA_RECENT_SECONDS 20u

/* Pure packet helpers, no I/O or allocation. Return full IPv6 packet length,
 * or zero without modifying output. Input/output buffers must not overlap.
 * Caller must authorize target against live, DAD-completed OT ND proxy state,
 * check Primary/infra readiness, own its solicited-node MLD reference, and
 * supply the interface's valid link-local source and Ethernet MAC.
 * age_seconds is OT mTimeSinceLastTransaction, refreshed at transmission.
 * R=1; Thread's recent-DUA policy sets O iff age <=20 seconds (OTBR reference
 * and vendored thread-cert/backbone/test_ndproxy.py), not generic O=0 proxy.
 * These functions do not validate the authority of a caller-supplied target. */
size_t br_nd_proxy_na(const uint8_t *request, size_t length,
                     const uint8_t target[16], const uint8_t local_source[16],
                     const uint8_t mac[6], uint32_t age_seconds,
                     uint8_t *output, size_t capacity);

/* Unsolicited all-nodes advertisement, used after a valid proxy Added event.
 * Caller also owns timing/rate limiting and revalidation of queued responses. */
size_t br_nd_proxy_announce(const uint8_t target[16], const uint8_t local_source[16],
                           const uint8_t mac[6], uint32_t age_seconds,
                           uint8_t *output, size_t capacity);
#endif
