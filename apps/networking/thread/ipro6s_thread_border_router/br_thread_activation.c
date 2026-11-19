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

#include "br_thread_activation.h"
#include <ipro_matter_br_thread.h>
#include <openthread/platform/settings.h>
#include <openthread/thread.h>
#include <openthread/ip6.h>
#include <easyflash.h>
#include "openthread_port.h"
#include <string.h>

/* Runtime fields are accessed only under the one SDK OT mutex. The durable
 * record contains no credentials. Retain expected bytes only until commit or
 * verified rollback. Static ownership is intentionally not released at runtime. */
static bool opened, in_progress, dirty;
static otOperationalDatasetTlvs expected;

static void wipe(void *data, size_t size)
{
    volatile unsigned char *bytes = data;
    while (size--) *bytes++ = 0;
}

static int read_journal(bool *pending)
{
    unsigned char record[4] = {0};
    size_t copied = 0, total = 0;
    *pending = false;
    const int error = ef_env_get_checked(BR_THREAD_ACTIVATION_KEY, 0, record, sizeof(record), &copied, &total);
    if (error == EF_ENV_NOT_FOUND) return EF_NO_ERR;
    if (error != EF_NO_ERR) return error;
    if (copied != sizeof(record) || total != sizeof(record) || record[0] != 'T' || record[1] != 'A' ||
        record[2] != 1 || record[3] > 1) return EF_ENV_CORRUPT;
    *pending = record[3] == 1;
    return EF_NO_ERR;
}

static int save_journal(bool pending)
{
    const unsigned char record[] = {'T', 'A', 1, pending ? 1 : 0};
    return ef_env_set_checked(BR_THREAD_ACTIVATION_KEY, record, sizeof(record));
}

int br_thread_activation_boot_recover(void)
{
    /* Startup caller guarantees neither network worker has been started. */
    if (otrGetInstanceIfReady()) return EF_ENV_CORRUPT;
    int error = easyflash_init();
    if (error != EF_NO_ERR) return error;
    bool pending = false;
    error = read_journal(&pending);
    if (error != EF_NO_ERR || !pending) return error;
    /* These are checked whole-key deletions, idempotent after interrupted
     * cleanup. Keep the intent until both datasets are verified absent. */
    if (otPlatSettingsDelete(NULL, OT_SETTINGS_KEY_PENDING_DATASET, -1) != OT_ERROR_NONE ||
        otPlatSettingsDelete(NULL, OT_SETTINGS_KEY_ACTIVE_DATASET, -1) != OT_ERROR_NONE) return EF_WRITE_ERR;
    return save_journal(false);
}

static otError acquire(otInstance **instance)
{
    *instance = otrGetInstanceIfReady();
    if (!*instance) return OT_ERROR_INVALID_STATE;
    return otrTryLock() ? OT_ERROR_NONE : OT_ERROR_BUSY;
}

otError ipro_matter_br_thread_activation_open(void)
{
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    bool pending = false;
    if (opened) error = OT_ERROR_ALREADY;
    else if (read_journal(&pending) != EF_NO_ERR) error = OT_ERROR_FAILED;
    else if (pending) error = OT_ERROR_INVALID_STATE; // Boot recovery was skipped.
    else opened = true;
    otrUnlock();
    return error;
}

static otError validate_active(const otOperationalDatasetTlvs *input)
{
    otOperationalDataset parsed = {0};
    otError error = otDatasetParseTlvs(input, &parsed);
    const otOperationalDatasetComponents *c = &parsed.mComponents;
    /* Required commissioned fields match the SDK ActiveDatasetManager; also
     * require the active timestamp and reject pending-only fields. Native
     * SetActiveTlvs performs its own TLV/value validation before saving. */
    if (error == OT_ERROR_NONE && (!c->mIsActiveTimestampPresent || !c->mIsNetworkKeyPresent ||
        !c->mIsNetworkNamePresent || !c->mIsExtendedPanIdPresent || !c->mIsPanIdPresent ||
        !c->mIsChannelPresent || c->mIsPendingTimestampPresent || c->mIsDelayPresent)) error = OT_ERROR_INVALID_ARGS;
    wipe(&parsed, sizeof(parsed));
    return error;
}

static otError require_absent(otInstance *instance, bool pending)
{
    otOperationalDatasetTlvs dataset = {0};
    otError error = pending ? otDatasetGetPendingTlvs(instance, &dataset) : otDatasetGetActiveTlvs(instance, &dataset);
    if (error == OT_ERROR_NOT_FOUND) error = OT_ERROR_NONE;
    else if (error == OT_ERROR_NONE && dataset.mLength != 0) error = OT_ERROR_INVALID_STATE;
    wipe(&dataset, sizeof(dataset));
    return error;
}

otError ipro_matter_br_thread_activation_begin(const otOperationalDatasetTlvs *input)
{
    if (!input || !input->mLength || input->mLength > sizeof(input->mTlvs)) return OT_ERROR_INVALID_ARGS;
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    if (!opened) error = OT_ERROR_INVALID_STATE;
    else if (in_progress) error = OT_ERROR_BUSY;
    else error = validate_active(input);
    if (error == OT_ERROR_NONE) error = require_absent(instance, false);
    if (error == OT_ERROR_NONE) error = require_absent(instance, true);
    if (error == OT_ERROR_NONE) {
        in_progress = true;
        expected.mLength = input->mLength;
        memcpy(expected.mTlvs, input->mTlvs, input->mLength);
        if (save_journal(true) != EF_NO_ERR) error = OT_ERROR_FAILED;
        else {
            /* Set before entering storage: an error can still mean partial
             * persistence. Do not publish CLEAN until rollback verifies it. */
            dirty = true;
            error = otDatasetSetActiveTlvs(instance, &expected);
            if (error == OT_ERROR_NONE) error = otIp6SetEnabled(instance, true);
            if (error == OT_ERROR_NONE) error = otThreadSetEnabled(instance, true);
        }
    }
    otrUnlock();
    return error;
}

static otError attached_locked(otInstance *instance, bool *attached)
{
    if (!opened || !in_progress || !dirty) return OT_ERROR_INVALID_STATE;
    otOperationalDatasetTlvs current = {0};
    otError error = otDatasetGetActiveTlvs(instance, &current);
    if (error == OT_ERROR_NONE && (current.mLength != expected.mLength ||
        memcmp(current.mTlvs, expected.mTlvs, expected.mLength) != 0)) error = OT_ERROR_INVALID_STATE;
    if (error == OT_ERROR_NONE) {
        const otDeviceRole role = otThreadGetDeviceRole(instance);
        *attached = role == OT_DEVICE_ROLE_CHILD || role == OT_DEVICE_ROLE_ROUTER || role == OT_DEVICE_ROLE_LEADER;
    }
    wipe(&current, sizeof(current));
    return error;
}

otError ipro_matter_br_thread_activation_attached(bool *attached)
{
    if (!attached) return OT_ERROR_INVALID_ARGS;
    *attached = false;
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    error = attached_locked(instance, attached);
    otrUnlock();
    return error;
}

otError ipro_matter_br_thread_activation_commit(void)
{
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    if (!opened) error = OT_ERROR_INVALID_STATE;
    else if (in_progress) {
        bool attached = false;
        error = attached_locked(instance, &attached);
        if (error == OT_ERROR_NONE && !attached) error = OT_ERROR_BUSY;
        if (error == OT_ERROR_NONE && save_journal(false) != EF_NO_ERR) error = OT_ERROR_FAILED;
        if (error == OT_ERROR_NONE) { in_progress = dirty = false; wipe(&expected, sizeof(expected)); }
    }
    otrUnlock();
    return error;
}

otError ipro_matter_br_thread_activation_revert(void)
{
    otInstance *instance;
    otError error = acquire(&instance);
    if (error != OT_ERROR_NONE) return error;
    if (!opened) error = OT_ERROR_INVALID_STATE;
    else if (in_progress) {
        if (dirty) {
            const otOperationalDatasetTlvs empty = {0};
            error = otThreadSetEnabled(instance, false);
            if (error == OT_ERROR_NONE) error = otIp6SetEnabled(instance, false);
            if (error == OT_ERROR_NONE) error = otDatasetSetPendingTlvs(instance, &empty);
            if (error == OT_ERROR_NONE) error = otDatasetSetActiveTlvs(instance, &empty);
        }
        if (error == OT_ERROR_NONE && save_journal(false) != EF_NO_ERR) error = OT_ERROR_FAILED;
        if (error == OT_ERROR_NONE) { in_progress = dirty = false; wipe(&expected, sizeof(expected)); }
    }
    otrUnlock();
    return error;
}

bool br_thread_activation_pending_allowed(void) { return !in_progress; }

bool br_thread_activation_cli_allowed(const char *line)
{
    if (!opened) return true;
    if (!line) return false;
    static const char *const diagnostics[] = {
        "state", "version", "ipaddr", "rloc16", "extaddr", "channel", "panid", "networkname",
        "br pd state", "br pd omrprefix", "nat64 state", "nat64 counters", "nat64 mappings", "br nat64prefix"
    };
    for (size_t i = 0; i < sizeof(diagnostics) / sizeof(diagnostics[0]); ++i)
        if (strcmp(line, diagnostics[i]) == 0) return true;
    return false;
}
