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

#include "br_domain.h"
#include <stdint.h>
#include <string.h>
#include <openthread/backbone_router_ftd.h>
#include <openthread/border_router.h>
#include <openthread/border_routing.h>
#include <openthread/ip6.h>
#include <openthread/netdata.h>
#include <openthread/thread.h>
#include <openthread/platform/alarm-milli.h>
#include "ipro_log.h"

#ifndef IPRO_THREAD_BR_DOMAIN_PREFIX
#define IPRO_THREAD_BR_DOMAIN_PREFIX ""
#endif

enum br_domain_state {
    BR_DOMAIN_DISABLED, BR_DOMAIN_WAITING, BR_DOMAIN_INVALID, BR_DOMAIN_CONFLICT,
    BR_DOMAIN_RETRY, BR_DOMAIN_PENDING, BR_DOMAIN_ACTIVE
};
static struct {
    otBorderRouterConfig config;
    bool parsed, invalid, disabled, owns, blocked, register_needed, polled, withdrawing;
    uint32_t last_poll;
    enum br_domain_state state;
    otError error;
} sDomain;

static void report(enum br_domain_state state, otError error)
{
    if (sDomain.state == state && sDomain.error == error) return;
    sDomain.state = state;
    sDomain.error = error;
    IPRO_LOGI("BR-Domain", "state=%u error=%u prefix=%s", (unsigned)state,
              (unsigned)error, IPRO_THREAD_BR_DOMAIN_PREFIX);
}

static bool same_prefix(const otIp6Prefix *a, const otIp6Prefix *b)
{
    return a->mLength == 64 && b->mLength == 64 &&
           memcmp(a->mPrefix.mFields.m8, b->mPrefix.mFields.m8, 16) == 0;
}

static bool overlap(const otIp6Prefix *a, const otIp6Prefix *b)
{
    unsigned length = a->mLength < b->mLength ? a->mLength : b->mLength;
    if (!length || a->mLength > 128 || b->mLength > 128) return true;
    const uint8_t *left = a->mPrefix.mFields.m8, *right = b->mPrefix.mFields.m8;
    if (memcmp(left, right, length / 8) != 0) return false;
    return !(length % 8) || ((left[length / 8] ^ right[length / 8]) & (0xffu << (8 - length % 8))) == 0;
}

static bool same_config(const otBorderRouterConfig *config)
{
    return same_prefix(&config->mPrefix, &sDomain.config.mPrefix) &&
           config->mDp && config->mPreferred && config->mOnMesh && config->mStable &&
           config->mDefaultRoute && !config->mSlaac && !config->mDhcp && !config->mConfigure &&
           !config->mNdDns && config->mPreference == OT_ROUTE_PREFERENCE_MED;
}

static void parse(void)
{
    sDomain.parsed = true;
    sDomain.register_needed = true;
    if (IPRO_THREAD_BR_DOMAIN_PREFIX[0] == '\0') { sDomain.disabled = true; return; }
    otIp6Prefix *prefix = &sDomain.config.mPrefix;
    if (otIp6PrefixFromString(IPRO_THREAD_BR_DOMAIN_PREFIX, prefix) != OT_ERROR_NONE || prefix->mLength != 64) {
        sDomain.invalid = true;
        return;
    }
    const uint8_t *bytes = prefix->mPrefix.mFields.m8;
    if ((bytes[0] & 0xfe) != 0xfc && (bytes[0] & 0xe0) != 0x20) sDomain.invalid = true;
    for (unsigned i = 8; i < 16; ++i) if (bytes[i]) sDomain.invalid = true;
    sDomain.config.mDp = sDomain.config.mPreferred = sDomain.config.mOnMesh = true;
    sDomain.config.mStable = sDomain.config.mDefaultRoute = true;
    sDomain.config.mPreference = OT_ROUTE_PREFERENCE_MED;
}

/* Read every entry, including multiple Domain Prefixes. Never treat the first
 * matching entry or a truncated/error observation as complete convergence. */
static otError scan(otInstance *instance, bool local, bool *matched)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    *matched = false;
    for (unsigned count = 0; count < 256; ++count) {
        otBorderRouterConfig entry;
        otError error = local ? otBorderRouterGetNextOnMeshPrefix(instance, &iterator, &entry)
                              : otNetDataGetNextOnMeshPrefix(instance, &iterator, &entry);
        if (error == OT_ERROR_NOT_FOUND) return OT_ERROR_NONE;
        if (error != OT_ERROR_NONE) return error;
        if (entry.mDp) {
            if (!same_prefix(&entry.mPrefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
            if (local && !same_config(&entry)) return OT_ERROR_ALREADY;
            *matched = true;
        } else if (overlap(&entry.mPrefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
    }
    return OT_ERROR_NO_BUFS;
}

static otError check_reserved(otInstance *instance)
{
    const otMeshLocalPrefix *mesh = otThreadGetMeshLocalPrefix(instance);
    if (!mesh) return OT_ERROR_INVALID_STATE;
    if (memcmp(mesh->m8, sDomain.config.mPrefix.mPrefix.mFields.m8, 8) == 0) return OT_ERROR_ALREADY;
    otIp6Prefix prefix;
    otError (*const getters[])(otInstance *, otIp6Prefix *) = {
        otBorderRoutingGetOmrPrefix, otBorderRoutingGetOnLinkPrefix, otBorderRoutingGetFavoredOnLinkPrefix
    };
    for (unsigned n = 0; n < sizeof(getters) / sizeof(getters[0]); ++n) {
        otError error = getters[n](instance, &prefix);
        if (error == OT_ERROR_NOT_FOUND) continue;
        if (error != OT_ERROR_NONE) return error;
        if (prefix.mLength && overlap(&prefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
    }
    otRoutePreference preference;
    otError error = otBorderRoutingGetFavoredOmrPrefix(instance, &prefix, &preference);
    if (error != OT_ERROR_NONE && error != OT_ERROR_NOT_FOUND) return error;
    if (error == OT_ERROR_NONE && prefix.mLength && overlap(&prefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
    otBorderRoutingPrefixTableIterator iterator;
    otBorderRoutingPrefixTableInitIterator(instance, &iterator);
    for (unsigned n = 0; n < 256; ++n) {
        otBorderRoutingPrefixTableEntry entry;
        error = otBorderRoutingGetNextPrefixTableEntry(instance, &iterator, &entry);
        if (error == OT_ERROR_NOT_FOUND) return OT_ERROR_NONE;
        if (error != OT_ERROR_NONE) return error;
        /* This pinned OT table tracks L && (A || PD-preferred) PIOs, not
         * ordinary Domain PIOs (L1/A0/P0). Deprecated valid LANs still conflict.
         * Table observation is not a substitute for deployment uniqueness. */
        bool valid = entry.mValidLifetime == UINT32_MAX ||
                     (uint64_t)entry.mValidLifetime * 1000 > entry.mMsecSinceLastUpdate;
        if (entry.mIsOnLink && valid && overlap(&entry.mPrefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
    }
    return OT_ERROR_NO_BUFS;
}

static otError check_local_routes(otInstance *instance)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    for (unsigned n = 0; n < 256; ++n) {
        otExternalRouteConfig route;
        otError error = otBorderRouterGetNextRoute(instance, &iterator, &route);
        if (error == OT_ERROR_NOT_FOUND) return OT_ERROR_NONE;
        if (error != OT_ERROR_NONE) return error;
        /* Core AddPrefix replaces a same-prefix TLV, including external routes. */
        if (same_prefix(&route.mPrefix, &sDomain.config.mPrefix)) return OT_ERROR_ALREADY;
    }
    return OT_ERROR_NO_BUFS;
}

/* Inspect only the target for ownership/withdrawal. Unrelated conflicts must
 * not suppress registration of a removal that already succeeded. */
static otError target_entry(otInstance *instance, bool strict, bool *present)
{
    otNetworkDataIterator iterator = OT_NETWORK_DATA_ITERATOR_INIT;
    *present = false;
    for (unsigned n = 0; n < 256; ++n) {
        otBorderRouterConfig entry;
        otError error = otBorderRouterGetNextOnMeshPrefix(instance, &iterator, &entry);
        if (error == OT_ERROR_NOT_FOUND) return OT_ERROR_NONE;
        if (error != OT_ERROR_NONE) return error;
        if (!same_prefix(&entry.mPrefix, &sDomain.config.mPrefix)) continue;
        if (strict && !same_config(&entry)) return OT_ERROR_ALREADY;
        if (entry.mDp) *present = true;
    }
    return OT_ERROR_NO_BUFS;
}

static bool complete_withdrawal(otInstance *instance)
{
    otBorderRouterConfig current;
    otError error = otBackboneRouterGetDomainPrefix(instance, &current);
    bool present;
    if ((error != OT_ERROR_NONE && error != OT_ERROR_NOT_FOUND) ||
        (error == OT_ERROR_NONE && same_prefix(&current.mPrefix, &sDomain.config.mPrefix)) ||
        target_entry(instance, false, &present) != OT_ERROR_NONE || present) {
        report(BR_DOMAIN_RETRY, OT_ERROR_INVALID_STATE);
        return false;
    }
    error = otBorderRouterRegister(instance);
    if (error != OT_ERROR_NONE) { report(BR_DOMAIN_RETRY, error); return false; }
    sDomain.withdrawing = false;
    sDomain.register_needed = true;
    report(BR_DOMAIN_CONFLICT, OT_ERROR_NONE);
    return true;
}

static void withdraw_owned(otInstance *instance)
{
    if (!sDomain.owns) return;
    otBorderRouterConfig current;
    otError error = otBackboneRouterGetDomainPrefix(instance, &current);
    if (error != OT_ERROR_NONE || !same_config(&current)) {
        if (error == OT_ERROR_NOT_FOUND || error == OT_ERROR_NONE) sDomain.blocked = true;
        report(BR_DOMAIN_RETRY, error == OT_ERROR_NONE ? OT_ERROR_ALREADY : error);
        return;
    }
    bool present;
    otError ownership = target_entry(instance, true, &present);
    if (ownership == OT_ERROR_NONE) ownership = check_local_routes(instance);
    if (ownership != OT_ERROR_NONE) {
        if (ownership == OT_ERROR_ALREADY) sDomain.blocked = true;
        report(BR_DOMAIN_RETRY, ownership);
        return;
    }
    /* Cached config alone is insufficient: removal is by prefix, irrespective
     * of the current local entry's flags. Never remove a replaced ordinary
     * entry or external route, even if the cached config still matches. */
    error = otBorderRouterRemoveOnMeshPrefix(instance, &sDomain.config.mPrefix);
    if (error != OT_ERROR_NONE) { report(BR_DOMAIN_RETRY, error); return; }
    sDomain.owns = false;
    sDomain.withdrawing = true;
    (void)complete_withdrawal(instance);
}

void br_domain_process(otInstance *instance, bool ready)
{
    if (!sDomain.parsed) parse();
    if (sDomain.disabled) return;
    if (sDomain.invalid) { report(BR_DOMAIN_INVALID, OT_ERROR_INVALID_ARGS); return; }
    if (sDomain.blocked) { report(BR_DOMAIN_CONFLICT, OT_ERROR_ALREADY); return; }
    if (!ready || otThreadGetDeviceRole(instance) < OT_DEVICE_ROLE_CHILD ||
        otBackboneRouterGetState(instance) == OT_BACKBONE_ROUTER_STATE_DISABLED) {
        sDomain.polled = false;
        sDomain.register_needed = true;
        report(BR_DOMAIN_WAITING, OT_ERROR_NONE);
        return;
    }
    uint32_t now = otPlatAlarmMilliGetNow();
    if (sDomain.polled && (uint32_t)(now - sDomain.last_poll) < 1000) return;
    sDomain.polled = true;
    sDomain.last_poll = now;
    if (sDomain.withdrawing && !complete_withdrawal(instance)) return;
    otBorderRouterConfig current;
    otError error = otBackboneRouterGetDomainPrefix(instance, &current);
    if ((error == OT_ERROR_NONE && (!sDomain.owns || !same_config(&current))) ||
        (error == OT_ERROR_NOT_FOUND && sDomain.owns)) {
        sDomain.blocked = true;
        report(BR_DOMAIN_CONFLICT, OT_ERROR_ALREADY);
        return;
    }
    if (error != OT_ERROR_NONE && error != OT_ERROR_NOT_FOUND) { report(BR_DOMAIN_RETRY, error); return; }
    bool local_match = false, leader_match = false;
    error = check_reserved(instance);
    if (error == OT_ERROR_NONE) error = check_local_routes(instance);
    if (error == OT_ERROR_NONE) error = scan(instance, false, &leader_match);
    if (error == OT_ERROR_NONE) error = scan(instance, true, &local_match);
    if (error != OT_ERROR_NONE) {
        report(error == OT_ERROR_ALREADY ? BR_DOMAIN_CONFLICT : BR_DOMAIN_RETRY, error);
        if (error == OT_ERROR_ALREADY) withdraw_owned(instance);
        return;
    }
    if (local_match && !sDomain.owns) { sDomain.blocked = true; report(BR_DOMAIN_CONFLICT, OT_ERROR_ALREADY); return; }
    if (!local_match) {
        error = otBorderRouterAddOnMeshPrefix(instance, &sDomain.config);
        if (error != OT_ERROR_NONE) { report(BR_DOMAIN_RETRY, error); return; }
        sDomain.owns = true;
        sDomain.register_needed = true;
        error = otBackboneRouterGetDomainPrefix(instance, &current);
        if (error != OT_ERROR_NONE || !same_config(&current)) { report(BR_DOMAIN_RETRY, OT_ERROR_INVALID_STATE); return; }
        error = scan(instance, true, &local_match);
        if (error != OT_ERROR_NONE || !local_match) { report(BR_DOMAIN_RETRY, OT_ERROR_NO_BUFS); return; }
    }
    if (sDomain.register_needed) {
        error = otBorderRouterRegister(instance);
        if (error != OT_ERROR_NONE) { report(BR_DOMAIN_RETRY, error); return; }
        sDomain.register_needed = false;
    }
    report(leader_match ? BR_DOMAIN_ACTIVE : BR_DOMAIN_PENDING, OT_ERROR_NONE);
}
