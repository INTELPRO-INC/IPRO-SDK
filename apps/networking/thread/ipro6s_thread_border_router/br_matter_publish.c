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

#include "br_matter_publish.h"
#include <openthread/platform/alarm-milli.h>
#include <string.h>

struct publication {
    char name[64];
    char type[16];
    uint32_t id;
    uint32_t started;
    otError error;
    bool active;
    bool pending;
    bool completed;
};
static struct publication sEntries[BR_MATTER_PUBLISH_CAPACITY];
static otInstance *sInstance;
static uint32_t sLastId;
static uint64_t sGeneration = 1;
static const uint8_t sOwnerToken;

static bool expired(const struct publication *entry)
{
    /* Unsigned subtraction covers the 32-bit millisecond counter wrapping.
     * Consumer's maximum observation gap must be less than UINT32_MAX minus
     * the timeout; otherwise it could skip the entire expired interval. */
    return (uint32_t)(otPlatAlarmMilliGetNow() - entry->started) >= BR_MATTER_PUBLISH_TIMEOUT_MS;
}

static bool same_name(const char *a, const char *b)
{
    while (*a && *b) {
        unsigned char x = (unsigned char)*a++, y = (unsigned char)*b++;
        if (x >= 'A' && x <= 'Z') x += 'a' - 'A';
        if (y >= 'A' && y <= 'Z') y += 'a' - 'A';
        if (x != y) return false;
    }
    return *a == *b;
}

static bool label_valid(const char *label)
{
    if (!label || !*label) return false;
    for (unsigned i = 0; i < 64; ++i) if (!label[i]) return true;
    return false;
}

static bool service_valid(otInstance *instance, const otMdnsService *service)
{
    if (!service || !label_valid(service->mServiceInstance) || !service->mServiceType ||
        !service->mPort) return false;
    if (strcmp(service->mServiceType, "_matter._tcp") != 0 &&
        strcmp(service->mServiceType, "_matterc._udp") != 0) return false;
    const char *local = otMdnsGetLocalHostName(instance);
    if (service->mHostName && (!local || !same_name(local, service->mHostName))) return false;
    if (service->mSubTypeLabelsLength && !service->mSubTypeLabels) return false;
    for (unsigned i = 0; i < service->mSubTypeLabelsLength; ++i)
        if (!label_valid(service->mSubTypeLabels[i])) return false;
    if (service->mTxtDataLength && !service->mTxtData) return false;
    unsigned offset = 0;
    while (offset < service->mTxtDataLength) {
        unsigned length = service->mTxtData[offset++];
        if (length > service->mTxtDataLength - offset) return false;
        offset += length;
    }
    return true;
}

static void registered(otInstance *instance, otMdnsRequestId id, otError error)
{
    if (instance != sInstance) return;
    for (unsigned i = 0; i < BR_MATTER_PUBLISH_CAPACITY; ++i) {
        struct publication *entry = &sEntries[i];
        if (entry->pending && entry->id == id) {
            entry->pending = false;
            entry->completed = true;
            entry->error = expired(entry) ? OT_ERROR_RESPONSE_TIMEOUT : error;
            break;
        }
    }
}

otError br_matter_publish(otInstance *instance, const otMdnsService *service, uint32_t *id)
{
    if (!instance || !id || !service_valid(instance, service)) return OT_ERROR_INVALID_ARGS;
    if ((sInstance && sInstance != instance) || !otMdnsIsEnabled(instance))
        return OT_ERROR_INVALID_STATE;
    struct publication *entry = NULL, *available = NULL;
    for (unsigned i = 0; i < BR_MATTER_PUBLISH_CAPACITY; ++i) {
        struct publication *candidate = &sEntries[i];
        if (!candidate->active && !candidate->pending && !candidate->completed) {
            if (!available) available = candidate;
        } else if (same_name(candidate->name, service->mServiceInstance) &&
                   strcmp(candidate->type, service->mServiceType) == 0) {
            entry = candidate;
            break;
        }
    }
    if (entry && (entry->pending || entry->completed)) return OT_ERROR_BUSY;
    if (!entry) entry = available;
    if (!entry || sLastId == UINT32_MAX) return OT_ERROR_NO_BUFS;
    sInstance = instance;
    bool was_active = entry->active;
    strcpy(entry->name, service->mServiceInstance);
    strcpy(entry->type, service->mServiceType);
    entry->id = ++sLastId; /* Never reuse a cancelled or failed request ID. */
    entry->started = otPlatAlarmMilliGetNow();
    entry->pending = true;
    otMdnsService local = *service;
    local.mHostName = NULL;
    otError error = otMdnsRegisterServiceOwned(instance, &local, entry->id, registered, &sOwnerToken);
    if (error != OT_ERROR_NONE) {
        entry->pending = false;
        if (!was_active) memset(entry, 0, sizeof(*entry));
        return error;
    }
    entry->active = true;
    *id = entry->id;
    return OT_ERROR_NONE;
}

bool br_matter_publish_take(otInstance *instance, br_matter_publish_result *result)
{
    if (!instance || instance != sInstance || !result) return false;
    for (unsigned i = 0; i < BR_MATTER_PUBLISH_CAPACITY; ++i) {
        struct publication *entry = &sEntries[i];
        if (entry->pending && expired(entry)) {
            entry->pending = false;
            entry->completed = true;
            entry->error = OT_ERROR_RESPONSE_TIMEOUT;
        }
        if (!entry->completed) continue;
        result->id = entry->id;
        result->error = entry->error;
        entry->completed = false;
        if (!entry->active) memset(entry, 0, sizeof(*entry));
        return true;
    }
    return false;
}

static void cancelled(struct publication *entry)
{
    entry->active = false;
    if (entry->pending || entry->completed) {
        entry->pending = false;
        entry->completed = true;
        entry->error = OT_ERROR_ABORT;
    } else memset(entry, 0, sizeof(*entry));
}

void br_matter_publish_network_down(otInstance *instance)
{
    if (!instance || (sInstance && instance != sInstance)) return;
    sInstance = instance;
    /* Zero is terminal exhaustion, never wrap back to an old valid epoch. */
    if (sGeneration) sGeneration = sGeneration == UINT64_MAX ? 0 : sGeneration + 1;
    for (unsigned i = 0; i < BR_MATTER_PUBLISH_CAPACITY; ++i) cancelled(&sEntries[i]);
}

uint64_t br_matter_publish_generation(otInstance *instance)
{
    if (!instance || (sInstance && instance != sInstance)) return 0;
    return sGeneration;
}

otError br_matter_publish_remove_all(otInstance *instance)
{
    if (!instance || (sInstance && instance != sInstance)) return OT_ERROR_INVALID_ARGS;
    if (!otMdnsIsEnabled(instance)) {
        br_matter_publish_network_down(instance);
        return OT_ERROR_NONE;
    }
    for (unsigned i = 0; i < BR_MATTER_PUBLISH_CAPACITY; ++i) {
        struct publication *entry = &sEntries[i];
        if (entry->active) {
            otMdnsService service = {0};
            service.mServiceInstance = entry->name;
            service.mServiceType = entry->type;
            otError error = otMdnsUnregisterServiceOwned(instance, &service, &sOwnerToken);
            if (error != OT_ERROR_NONE) return error; /* Retry retains remaining entries. */
        }
        cancelled(entry);
    }
    return OT_ERROR_NONE;
}
