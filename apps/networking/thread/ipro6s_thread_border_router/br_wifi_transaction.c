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

#include "br_wifi_transaction.h"
#include "br_wifi_storage.h"
#include <string.h>

static bool usable(const struct br_wifi_transaction *t)
{
    return t && t->initialized && !t->busy && !t->closed && !t->uncertain;
}

static void wipe(struct br_wifi_transaction *t)
{
    br_wifi_profile_wipe(&t->committed, sizeof(t->committed));
    br_wifi_profile_wipe(&t->staged, sizeof(t->staged));
    t->committed_present = t->staged_present = false;
}

static void canonical_copy(struct br_wifi_profile *out, const struct br_wifi_profile *in)
{
    struct br_wifi_profile value = {0};
    if (in) {
        memcpy(value.magic, "BW01", 4);
        value.ssid_len = in->ssid_len;
        value.key_len = in->key_len;
        value.pmf = in->pmf;
        value.akm = in->akm;
        memcpy(value.ssid, in->ssid, in->ssid_len);
        memcpy(value.key, in->key, in->key_len);
    }
    br_wifi_profile_wipe(out, sizeof(*out));
    *out = value;
    br_wifi_profile_wipe(&value, sizeof(value));
}

int br_wifi_transaction_init(struct br_wifi_transaction *t, const struct br_wifi_profile *initial)
{
    if (!t || t->initialized || t->busy || t->closed || t->uncertain ||
        (initial && !br_wifi_profile_valid(initial, sizeof(*initial)))) return -1;
    canonical_copy(&t->committed, initial);
    t->staged = t->committed;
    t->committed_present = t->staged_present = initial != NULL;
    t->initialized = true;
    return 0;
}

int br_wifi_transaction_open(struct br_wifi_transaction *t)
{
    if (!t || t->initialized || t->busy || t->closed || t->uncertain) return -1;
    int error = br_wifi_storage_claim(t);
    if (error != 0) return error;
    t->storage_owner = t;
    t->busy = true;
    struct br_wifi_profile initial = {0};
    bool present = false;
    error = br_wifi_storage_load_owned(t, &initial, &present);
    t->busy = false;
    if (error != 0) {
        t->uncertain = true;
        wipe(t);
    } else {
        error = br_wifi_transaction_init(t, present ? &initial : NULL);
    }
    br_wifi_profile_wipe(&initial, sizeof(initial));
    return error;
}

int br_wifi_transaction_stage(struct br_wifi_transaction *t, const struct br_wifi_profile *profile)
{
    if (!usable(t) || (profile && !br_wifi_profile_valid(profile, sizeof(*profile)))) return -1;
    canonical_copy(&t->staged, profile);
    t->staged_present = profile != NULL;
    return 0;
}

int br_wifi_transaction_get(const struct br_wifi_transaction *t, struct br_wifi_profile *profile, bool *present)
{
    if (!usable(t) || !profile || !present) return -1;
    *profile = t->staged;
    *present = t->staged_present;
    return 0;
}

int br_wifi_transaction_revert(struct br_wifi_transaction *t)
{
    if (!usable(t)) return -1;
    canonical_copy(&t->staged, t->committed_present ? &t->committed : NULL);
    t->staged_present = t->committed_present;
    return 0;
}

int br_wifi_transaction_commit(struct br_wifi_transaction *t)
{
    if (!usable(t)) return -1;
    if (t->staged_present == t->committed_present &&
        memcmp(&t->staged, &t->committed, sizeof(t->staged)) == 0) return 0;
    t->busy = true;
    const int error = t->staged_present ? br_wifi_storage_save_owned(t->storage_owner, &t->staged, sizeof(t->staged)) :
                                        br_wifi_storage_clear_owned(t->storage_owner);
    t->busy = false;
    if (error != 0 || t->closed) {
        t->uncertain = error != 0;
        wipe(t);
        return error != 0 ? error : -1;
    }
    canonical_copy(&t->committed, t->staged_present ? &t->staged : NULL);
    t->committed_present = t->staged_present;
    return 0;
}

int br_wifi_transaction_close(struct br_wifi_transaction *t)
{
    if (!t) return -1;
    t->closed = true;
    if (t->busy) return 1;
    wipe(t);
    return 0;
}
