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
#include <string.h>

_Static_assert(BR_MATTER_BROWSE_CAPACITY > 0 && BR_MATTER_BROWSE_CAPACITY <= 8,
               "Browser membership bitmask holds at most eight subscribers");

struct record {
    char name[64];
    uint32_t ttl;
    uint8_t pending;
};
struct query {
    bool active;
    char type[16], subtype[64];
    uint32_t interface_index;
    uint8_t members;
    otError error;
    struct record records[BR_MATTER_BROWSE_RECORDS];
};
struct subscriber {
    uint32_t id;
    unsigned query_index;
    bool final;
    otError error;
    unsigned next_record;
};
static struct query sQueries[BR_MATTER_BROWSE_CAPACITY];
static struct subscriber sSubscribers[BR_MATTER_BROWSE_CAPACITY];
static otInstance *sInstance;
static uint32_t sLastId;
static unsigned sNextSubscriber;

static bool label_valid(const char *text)
{
    if (!text || !*text) return false;
    for (unsigned i = 0; i < 64; ++i) if (!text[i]) return true;
    return false;
}
static unsigned fold(unsigned ch) { return ch >= 'A' && ch <= 'Z' ? ch + ('a' - 'A') : ch; }
static bool same(const char *a, const char *b)
{
    if (!a) a = "";
    if (!b) b = "";
    while (*a && *b) {
        if (fold((unsigned char)*a++) != fold((unsigned char)*b++)) return false;
    }
    return *a == *b;
}
static void received(otInstance *instance, const otMdnsBrowseResult *result)
{
    if (instance != sInstance || !result || !result->mServiceType) return;
    for (unsigned i = 0; i < BR_MATTER_BROWSE_CAPACITY; ++i) {
        struct query *query = &sQueries[i];
        if (!query->active || query->error != OT_ERROR_NONE ||
            query->interface_index != result->mInfraIfIndex ||
            !same(query->type, result->mServiceType) || !same(query->subtype, result->mSubTypeLabel)) continue;
        if (!label_valid(result->mServiceInstance)) { query->error = OT_ERROR_PARSE; continue; }
        struct record *record = NULL, *available = NULL;
        for (unsigned n = 0; n < BR_MATTER_BROWSE_RECORDS; ++n) {
            struct record *candidate = &query->records[n];
            if (candidate->name[0] && same(candidate->name, result->mServiceInstance)) record = candidate;
            if (!candidate->name[0] && !available) available = candidate;
        }
        if (!record && !result->mTtl) continue;
        if (!record) record = available;
        if (!record) { query->error = OT_ERROR_NO_BUFS; continue; }
        strcpy(record->name, result->mServiceInstance);
        record->ttl = result->mTtl;
        record->pending = query->members;
    }
}
static otMdnsBrowser browser_for(struct query *query)
{
    otMdnsBrowser browser = {query->type, query->subtype[0] ? query->subtype : NULL,
                            query->interface_index, received};
    return browser;
}

otError br_matter_browse_start(otInstance *instance, const char *type,
                              const char *subtype, uint32_t interface_index, uint32_t *id)
{
    if (!instance || !id || !interface_index || !type ||
        (strcmp(type, "_matter._tcp") && strcmp(type, "_matterc._udp")) ||
        (subtype && !label_valid(subtype))) return OT_ERROR_INVALID_ARGS;
    if ((sInstance && sInstance != instance) || !otMdnsIsEnabled(instance)) return OT_ERROR_INVALID_STATE;
    uint32_t active_index;
    otError interface_error = ipro_matter_dnssd_get_interface(instance, &active_index);
    if (interface_error != OT_ERROR_NONE) return interface_error;
    if (interface_index != active_index) return OT_ERROR_INVALID_ARGS;
    unsigned slot = BR_MATTER_BROWSE_CAPACITY, selected = BR_MATTER_BROWSE_CAPACITY;
    unsigned free_query = BR_MATTER_BROWSE_CAPACITY;
    for (unsigned i = 0; i < BR_MATTER_BROWSE_CAPACITY; ++i) {
        if (!sSubscribers[i].id && slot == BR_MATTER_BROWSE_CAPACITY) slot = i;
        struct query *query = &sQueries[i];
        if (!query->active) { if (free_query == BR_MATTER_BROWSE_CAPACITY) free_query = i; }
        else if (same(query->type, type) && same(query->subtype, subtype) &&
                 query->interface_index == interface_index) selected = i;
    }
    if (slot == BR_MATTER_BROWSE_CAPACITY || sLastId == UINT32_MAX) return OT_ERROR_NO_BUFS;
    bool fresh = selected == BR_MATTER_BROWSE_CAPACITY;
    if (fresh) selected = free_query;
    if (selected == BR_MATTER_BROWSE_CAPACITY) return OT_ERROR_NO_BUFS;
    struct query *query = &sQueries[selected];
    if (!fresh && query->error != OT_ERROR_NONE) return query->error;
    if (fresh) {
        memset(query, 0, sizeof(*query));
        query->active = true;
        strcpy(query->type, type);
        if (subtype) strcpy(query->subtype, subtype);
        query->interface_index = interface_index;
    }
    uint8_t bit = (uint8_t)(1U << slot);
    struct subscriber *subscriber = &sSubscribers[slot];
    *subscriber = (struct subscriber){.id = ++sLastId, .query_index = selected};
    query->members |= bit;
    sInstance = instance; /* Install all state before synchronous cached callbacks. */
    if (fresh) {
        otMdnsBrowser browser = browser_for(query);
        otError error = otMdnsStartBrowser(instance, &browser);
        if (error != OT_ERROR_NONE) {
            memset(subscriber, 0, sizeof(*subscriber));
            memset(query, 0, sizeof(*query));
            return error;
        }
    } else {
        for (unsigned n = 0; n < BR_MATTER_BROWSE_RECORDS; ++n)
            if (query->records[n].ttl) query->records[n].pending |= bit;
    }
    *id = subscriber->id;
    return OT_ERROR_NONE;
}

otError br_matter_browse_stop(otInstance *instance, uint32_t id)
{
    if (!instance || instance != sInstance) return OT_ERROR_INVALID_ARGS;
    for (unsigned i = 0; i < BR_MATTER_BROWSE_CAPACITY; ++i) {
        struct subscriber *subscriber = &sSubscribers[i];
        if (!id || subscriber->id != id) continue;
        if (subscriber->final) return OT_ERROR_NONE;
        struct query *query = &sQueries[subscriber->query_index];
        uint8_t bit = (uint8_t)(1U << i);
        if (query->members == bit) {
            otMdnsBrowser browser = browser_for(query);
            otError error = otMdnsStopBrowser(instance, &browser);
            if (error != OT_ERROR_NONE) return error;
        }
        query->members &= (uint8_t)~bit;
        for (unsigned n = 0; n < BR_MATTER_BROWSE_RECORDS; ++n) {
            struct record *record = &query->records[n];
            record->pending &= (uint8_t)~bit;
            if (!record->pending && !record->ttl) memset(record, 0, sizeof(*record));
        }
        if (!query->members) memset(query, 0, sizeof(*query));
        subscriber->final = true;
        return OT_ERROR_NONE;
    }
    return OT_ERROR_NOT_FOUND;
}

bool br_matter_browse_take(otInstance *instance, br_matter_browse_result *result)
{
    if (!instance || instance != sInstance || !result) return false;
    for (unsigned offset = 0; offset < BR_MATTER_BROWSE_CAPACITY; ++offset) {
        unsigned i = (sNextSubscriber + offset) % BR_MATTER_BROWSE_CAPACITY;
        struct subscriber *subscriber = &sSubscribers[i];
        if (!subscriber->id) continue;
        struct query *query = &sQueries[subscriber->query_index];
        if (!subscriber->final && query->error != OT_ERROR_NONE) {
            subscriber->error = query->error;
            if (br_matter_browse_stop(instance, subscriber->id) != OT_ERROR_NONE) continue;
        }
        if (subscriber->final) {
            *result = (br_matter_browse_result){.id = subscriber->id, .error = subscriber->error, .final = true};
            memset(subscriber, 0, sizeof(*subscriber));
            sNextSubscriber = (i + 1) % BR_MATTER_BROWSE_CAPACITY;
            return true;
        }
        uint8_t bit = (uint8_t)(1U << i);
        for (unsigned offset_record = 0; offset_record < BR_MATTER_BROWSE_RECORDS; ++offset_record) {
            unsigned n = (subscriber->next_record + offset_record) % BR_MATTER_BROWSE_RECORDS;
            struct record *record = &query->records[n];
            if (!(record->pending & bit)) continue;
            *result = (br_matter_browse_result){.id = subscriber->id, .ttl = record->ttl,
                                              .interface_index = query->interface_index};
            strcpy(result->name, record->name);
            record->pending &= (uint8_t)~bit;
            if (!record->pending && !record->ttl) memset(record, 0, sizeof(*record));
            sNextSubscriber = (i + 1) % BR_MATTER_BROWSE_CAPACITY;
            subscriber->next_record = (n + 1) % BR_MATTER_BROWSE_RECORDS;
            return true;
        }
    }
    return false;
}

void br_matter_browse_network_down(otInstance *instance)
{
    if (!instance || instance != sInstance) return;
    for (unsigned i = 0; i < BR_MATTER_BROWSE_CAPACITY; ++i) {
        if (sSubscribers[i].id && !sSubscribers[i].final) {
            sSubscribers[i].final = true;
            sSubscribers[i].error = OT_ERROR_INVALID_STATE;
        }
    }
    memset(sQueries, 0, sizeof(sQueries));
}
