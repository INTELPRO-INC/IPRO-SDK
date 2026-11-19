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

#ifndef IPRO_MATTER_DNSSD_H
#define IPRO_MATTER_DNSSD_H

#include <openthread/mdns.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BR_MATTER_PUBLISH_CAPACITY 8
#define BR_MATTER_PUBLISH_TIMEOUT_MS 30000U
/* Nonblocking access to the initialized SDK OT instance. On success, release
 * exactly once after all OT calls, before invoking any CHIP callback. */
otError ipro_matter_dnssd_acquire(otInstance **instance);
void ipro_matter_dnssd_release(void);
/* Caller holds the acquired SDK OT lock. Returns the currently listening mDNS
 * transport's infrastructure index, never a configured-but-inactive index.
 * No lwIP pointer crosses the boundary. Output is zero on failure when nonnull.
 * Keep the OT lock across this read and browser/resolver start so shutdown or
 * interface reconfiguration cannot invalidate the selection between calls. */
otError ipro_matter_dnssd_get_interface(otInstance *instance, uint32_t *interface_index);
typedef struct {
    uint32_t id;
    otError error;
} br_matter_publish_result;

/* Single SDK instance, all calls under its OT lock (including take). No CHIP
 * callbacks occur here. Consumer maps IDs to owned CHIP callback state, polls
 * take, releases OT lock, then dispatches on CHIP context. Inputs are borrowed
 * only during publish: OT copies payload synchronously. Output ID is unchanged
 * on failure. BUSY requires draining the previous completion before retry.
 * Take also expires pending operations after 30 seconds; consumer must poll
 * periodically (recommended <= 1 second; maximum observation gap must be less
 * than UINT32_MAX - BR_MATTER_PUBLISH_TIMEOUT_MS milliseconds).
 * Timeout does not withdraw an advertisement: caller must remove or retry.
 * Late callbacks cannot complete a timed-out or superseded operation.
 * This module does not retry/re-publish after network loss. The IPRO OT owner
 * API reserves its keys against other trusted publishers, including SRP.
 * It is process-local coordination, not authentication against hostile code. */
otError br_matter_publish(otInstance *instance, const otMdnsService *service, uint32_t *id);
bool br_matter_publish_take(otInstance *instance, br_matter_publish_result *result);
otError br_matter_publish_remove_all(otInstance *instance);
/* Called by the mDNS transport owner whenever disabled, before re-enabling. */
void br_matter_publish_network_down(otInstance *instance);
/* Read under the SDK OT lock. Changes on every network-down notification,
 * even before publishing or after a successful completion was consumed.
 * Consumers compare epochs to detect outages shorter than their poll period.
 * Zero means invalid instance or exhausted epoch counter; never a valid epoch.
 * A change requests rebuilding current desired advertisements, not replaying
 * old completion callbacks. Normal removal while online does not change it. */
uint64_t br_matter_publish_generation(otInstance *instance);

#define BR_MATTER_BROWSE_CAPACITY 8
#define BR_MATTER_BROWSE_RECORDS 8
typedef struct {
    uint32_t id;
    uint32_t ttl;
    uint32_t interface_index;
    otError error;
    bool final;
    char name[64];
} br_matter_browse_result;
/* All calls require the SDK OT lock. No application callbacks are invoked.
 * Duplicate service/subtype/interface queries share one OT browser. Results
 * are owned snapshots, coalesced per instance name; TTL zero means withdrawal.
 * New subscribers receive currently cached nonzero-TTL records. OT owns TTL
 * expiration and reports removals; returned TTL is the last reported value.
 * Stop queues exactly one final result; ID remains reserved until taken.
 * Cache overflow terminates subscribers with NO_BUFS, after successful OT
 * cleanup. Poll again if cleanup is temporarily busy. IDs are never reused.
 * Inputs are borrowed only during start; output ID is unchanged on failure. */
otError br_matter_browse_start(otInstance *instance, const char *type,
                              const char *subtype, uint32_t interface_index, uint32_t *id);
otError br_matter_browse_stop(otInstance *instance, uint32_t id);
bool br_matter_browse_take(otInstance *instance, br_matter_browse_result *result);
/* Called by transport owner when mDNS is disabled and its browsers discarded. */
void br_matter_browse_network_down(otInstance *instance);

#define BR_MATTER_RESOLVE_CAPACITY 4
#define BR_MATTER_RESOLVE_ADDRESSES 5
#define BR_MATTER_RESOLVE_TXT_SIZE 1024
#define BR_MATTER_RESOLVE_TIMEOUT_MS 30000U
typedef struct {
    uint32_t id, interface_index, ttl;
    otError error;
    uint16_t port, txt_length;
    uint8_t address_count;
    char name[64], type[16], host[64];
    uint8_t txt[BR_MATTER_RESOLVE_TXT_SIZE];
    otIp6Address addresses[BR_MATTER_RESOLVE_ADDRESSES];
} br_matter_resolve_result;
/* Single-shot resolve, all calls under SDK OT lock. families: bit 0 IPv4,
 * bit 1 IPv6. IPv4 uses mapped IPv6 format. Start only admits work; a rejected
 * start retains nothing and leaves ID unchanged. Take drives all OT stages,
 * copies one terminal result, and releases the slot only after cleanup.
 * Poll <=1s; wrap-safe deadline requires observation gaps < UINT32_MAX-30000ms.
 * Completion includes currently available addresses (IPv6 first), at most five;
 * it does not wait for a missing optional family or report later updates.
 * No CHIP callback is invoked here. Input strings are copied at admission. */
otError br_matter_resolve_start(otInstance *instance, const char *name, const char *type,
                               uint32_t interface_index, uint8_t families, uint32_t *id);
otError br_matter_resolve_cancel(otInstance *instance, uint32_t id);
bool br_matter_resolve_take(otInstance *instance, br_matter_resolve_result *result);
void br_matter_resolve_network_down(otInstance *instance);

#ifdef __cplusplus
}
#endif
#endif
