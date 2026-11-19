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

#include "ipro_matter_dnssd.h"
#include <openthread/platform/alarm-milli.h>
#include <string.h>

enum { SRV = 1, TXT = 2, IP4 = 4, IP6 = 8 };
struct query {
    br_matter_resolve_result value;
    uint32_t started, srv_ttl, txt_ttl;
    uint8_t families, active, count4, count6;
    bool closing, srv_valid, txt_valid;
    char address_host[64];
    otMdnsAddressAndTtl ipv4[BR_MATTER_RESOLVE_ADDRESSES], ipv6[BR_MATTER_RESOLVE_ADDRESSES];
};
static struct query sQueries[BR_MATTER_RESOLVE_CAPACITY];
static otInstance *sInstance;
static uint32_t sLastId;
static unsigned sNext;
_Static_assert(BR_MATTER_RESOLVE_CAPACITY == 4, "Update callback table with capacity");

static bool label(const char *s)
{
    if (!s || !*s) return false;
    for (unsigned i = 0; i < 64; ++i) if (!s[i]) return true;
    return false;
}
static unsigned fold(unsigned c) { return c >= 'A' && c <= 'Z' ? c + ('a' - 'A') : c; }
static bool same(const char *a, const char *b)
{
    if (!a || !b) return a == b;
    while (*a && *b) if (fold((unsigned char)*a++) != fold((unsigned char)*b++)) return false;
    return *a == *b;
}
static void fail(struct query *q, otError error) { q->value.error = error; q->closing = true; }
static bool matches(struct query *q, otInstance *i, const char *name, const char *type, uint32_t index)
{
    return i == sInstance && q->value.id && !q->closing && q->value.interface_index == index &&
           same(q->value.name, name) && same(q->value.type, type);
}
static void srv_result(unsigned n, otInstance *i, const otMdnsSrvResult *r)
{
    struct query *q = &sQueries[n];
    if (!r || !matches(q, i, r->mServiceInstance, r->mServiceType, r->mInfraIfIndex)) return;
    q->srv_valid = false;
    q->value.host[0] = 0;
    if (!r->mTtl) return;
    if (!label(r->mHostName) || !r->mPort) { fail(q, OT_ERROR_PARSE); return; }
    strcpy(q->value.host, r->mHostName);
    q->value.port = r->mPort;
    q->srv_ttl = r->mTtl;
    q->srv_valid = true;
}
static void txt_result(unsigned n, otInstance *i, const otMdnsTxtResult *r)
{
    struct query *q = &sQueries[n];
    if (!r || !matches(q, i, r->mServiceInstance, r->mServiceType, r->mInfraIfIndex)) return;
    q->txt_valid = false;
    q->value.txt_length = 0;
    if (!r->mTtl) return;
    if (r->mTxtDataLength > sizeof(q->value.txt)) { fail(q, OT_ERROR_NO_BUFS); return; }
    if (r->mTxtDataLength && !r->mTxtData) { fail(q, OT_ERROR_PARSE); return; }
    for (unsigned offset = 0; offset < r->mTxtDataLength;) {
        unsigned length = r->mTxtData[offset++];
        if (length > r->mTxtDataLength - offset) { fail(q, OT_ERROR_PARSE); return; }
        offset += length;
    }
    if (r->mTxtDataLength) memcpy(q->value.txt, r->mTxtData, r->mTxtDataLength);
    q->value.txt_length = r->mTxtDataLength;
    q->txt_ttl = r->mTtl;
    q->txt_valid = true;
}
static void address_result(unsigned n, bool ipv4, otInstance *i, const otMdnsAddressResult *r)
{
    struct query *q = &sQueries[n];
    if (!r || i != sInstance || !q->value.id || q->closing || r->mInfraIfIndex != q->value.interface_index ||
        !same(r->mHostName, q->address_host) || !same(q->address_host, q->value.host)) return;
    uint8_t *count = ipv4 ? &q->count4 : &q->count6;
    otMdnsAddressAndTtl *entries = ipv4 ? q->ipv4 : q->ipv6;
    *count = 0;
    if (r->mAddressesLength && !r->mAddresses) { fail(q, OT_ERROR_PARSE); return; }
    for (unsigned j = 0; j < r->mAddressesLength && *count < BR_MATTER_RESOLVE_ADDRESSES; ++j) {
        if (!r->mAddresses[j].mTtl) continue;
        entries[(*count)++] = r->mAddresses[j];
    }
}
#define CALLBACKS(n) \
 static void srv##n(otInstance *i, const otMdnsSrvResult *r) { srv_result(n, i, r); } \
 static void txt##n(otInstance *i, const otMdnsTxtResult *r) { txt_result(n, i, r); } \
 static void ip4##n(otInstance *i, const otMdnsAddressResult *r) { address_result(n, true, i, r); } \
 static void ip6##n(otInstance *i, const otMdnsAddressResult *r) { address_result(n, false, i, r); }
CALLBACKS(0)
CALLBACKS(1)
CALLBACKS(2)
CALLBACKS(3)
static const otMdnsSrvCallback sSrv[] = {srv0, srv1, srv2, srv3};
static const otMdnsTxtCallback sTxt[] = {txt0, txt1, txt2, txt3};
static const otMdnsAddressCallback sIp4[] = {ip40, ip41, ip42, ip43};
static const otMdnsAddressCallback sIp6[] = {ip60, ip61, ip62, ip63};

static otError stop_addresses(unsigned n)
{
    struct query *q = &sQueries[n];
    otMdnsAddressResolver r = {q->address_host, q->value.interface_index, sIp4[n]};
    if (q->active & IP4) {
        otError error = otMdnsStopIp4AddressResolver(sInstance, &r);
        if (error) return error;
        q->active &= (uint8_t)~IP4;
    }
    r.mCallback = sIp6[n];
    if (q->active & IP6) {
        otError error = otMdnsStopIp6AddressResolver(sInstance, &r);
        if (error) return error;
        q->active &= (uint8_t)~IP6;
    }
    return OT_ERROR_NONE;
}
static otError cleanup(unsigned n)
{
    struct query *q = &sQueries[n];
    otError error = stop_addresses(n);
    if (error) return error;
    if (q->active & TXT) {
        otMdnsTxtResolver r = {q->value.name, q->value.type, q->value.interface_index, sTxt[n]};
        error = otMdnsStopTxtResolver(sInstance, &r);
        if (error) return error;
        q->active &= (uint8_t)~TXT;
    }
    if (q->active & SRV) {
        otMdnsSrvResolver r = {q->value.name, q->value.type, q->value.interface_index, sSrv[n]};
        error = otMdnsStopSrvResolver(sInstance, &r);
        if (error) return error;
        q->active &= (uint8_t)~SRV;
    }
    return OT_ERROR_NONE;
}
static void advance(unsigned n)
{
    struct query *q = &sQueries[n];
    if (q->closing) return;
    if ((uint32_t)(otPlatAlarmMilliGetNow() - q->started) >= BR_MATTER_RESOLVE_TIMEOUT_MS) {
        fail(q, OT_ERROR_RESPONSE_TIMEOUT); return;
    }
    otError error;
    if (!(q->active & SRV)) {
        otMdnsSrvResolver r = {q->value.name, q->value.type, q->value.interface_index, sSrv[n]};
        error = otMdnsStartSrvResolver(sInstance, &r);
        if (error) { fail(q, error); return; }
        q->active |= SRV;
    }
    if (q->closing) return;
    if (!(q->active & TXT)) {
        otMdnsTxtResolver r = {q->value.name, q->value.type, q->value.interface_index, sTxt[n]};
        error = otMdnsStartTxtResolver(sInstance, &r);
        if (error) { fail(q, error); return; }
        q->active |= TXT;
    }
    if (q->closing) return;
    if (!same(q->value.host, q->address_host)) {
        if (stop_addresses(n) != OT_ERROR_NONE) return;
        strcpy(q->address_host, q->value.host);
        q->count4 = q->count6 = 0;
    }
    if (!q->srv_valid) return;
    otMdnsAddressResolver r = {q->address_host, q->value.interface_index, sIp6[n]};
    if ((q->families & 2) && !(q->active & IP6)) {
        error = otMdnsStartIp6AddressResolver(sInstance, &r);
        if (error) { fail(q, error); return; }
        q->active |= IP6;
    }
    if (q->closing) return;
    if ((q->families & 1) && !(q->active & IP4)) {
        r.mCallback = sIp4[n];
        error = otMdnsStartIp4AddressResolver(sInstance, &r);
        if (error) { fail(q, error); return; }
        q->active |= IP4;
    }
    if (q->closing || !q->txt_valid || (!q->count4 && !q->count6)) return;
    q->value.ttl = q->srv_ttl < q->txt_ttl ? q->srv_ttl : q->txt_ttl;
    for (unsigned family = 0; family < 2; ++family) {
        const otMdnsAddressAndTtl *entries = family ? q->ipv4 : q->ipv6;
        unsigned count = family ? q->count4 : q->count6;
        for (unsigned j = 0; j < count && q->value.address_count < BR_MATTER_RESOLVE_ADDRESSES; ++j) {
            q->value.addresses[q->value.address_count++] = entries[j].mAddress;
            if (entries[j].mTtl < q->value.ttl) q->value.ttl = entries[j].mTtl;
        }
    }
    q->closing = true;
}
otError br_matter_resolve_start(otInstance *instance, const char *name, const char *type,
                               uint32_t index, uint8_t families, uint32_t *id)
{
    if (!instance || !id || !label(name) || !type || !families || (families & ~3U) ||
        (strcmp(type, "_matter._tcp") && strcmp(type, "_matterc._udp"))) return OT_ERROR_INVALID_ARGS;
    if (sInstance && sInstance != instance) return OT_ERROR_INVALID_STATE;
    uint32_t active_index;
    otError error = ipro_matter_dnssd_get_interface(instance, &active_index);
    if (error) return error;
    if (!index || index != active_index) return OT_ERROR_INVALID_ARGS;
    if (sLastId == UINT32_MAX) return OT_ERROR_NO_BUFS;
    for (unsigned n = 0; n < BR_MATTER_RESOLVE_CAPACITY; ++n) {
        struct query *q = &sQueries[n];
        if (q->value.id) continue;
        memset(q, 0, sizeof(*q));
        q->value.id = ++sLastId;
        q->value.interface_index = index;
        strcpy(q->value.name, name);
        strcpy(q->value.type, type);
        q->families = families;
        q->started = otPlatAlarmMilliGetNow();
        sInstance = instance;
        *id = q->value.id;
        return OT_ERROR_NONE;
    }
    return OT_ERROR_NO_BUFS;
}
otError br_matter_resolve_cancel(otInstance *instance, uint32_t id)
{
    if (!instance || instance != sInstance) return OT_ERROR_INVALID_ARGS;
    for (unsigned n = 0; n < BR_MATTER_RESOLVE_CAPACITY; ++n) {
        if (id && sQueries[n].value.id == id) { fail(&sQueries[n], OT_ERROR_ABORT); return OT_ERROR_NONE; }
    }
    return OT_ERROR_NOT_FOUND;
}
bool br_matter_resolve_take(otInstance *instance, br_matter_resolve_result *result)
{
    if (!instance || instance != sInstance || !result) return false;
    if (!otMdnsIsEnabled(instance)) br_matter_resolve_network_down(instance);
    for (unsigned offset = 0; offset < BR_MATTER_RESOLVE_CAPACITY; ++offset) {
        unsigned n = (sNext + offset) % BR_MATTER_RESOLVE_CAPACITY;
        struct query *q = &sQueries[n];
        if (!q->value.id) continue;
        advance(n);
        if (!q->closing || cleanup(n) != OT_ERROR_NONE) continue;
        if (q->value.error == OT_ERROR_NONE) *result = q->value;
        else *result = (br_matter_resolve_result){.id = q->value.id, .error = q->value.error};
        memset(q, 0, sizeof(*q));
        sNext = (n + 1) % BR_MATTER_RESOLVE_CAPACITY;
        return true;
    }
    return false;
}
void br_matter_resolve_network_down(otInstance *instance)
{
    if (!instance || instance != sInstance) return;
    for (unsigned n = 0; n < BR_MATTER_RESOLVE_CAPACITY; ++n) {
        if (!sQueries[n].value.id) continue;
        sQueries[n].active = 0; // The disabled engine has discarded all resolvers.
        fail(&sQueries[n], OT_ERROR_INVALID_STATE);
    }
}
