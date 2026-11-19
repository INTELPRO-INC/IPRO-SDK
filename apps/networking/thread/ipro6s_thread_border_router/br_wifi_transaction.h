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
#include "br_wifi_profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Zero-initialize once, then use only these APIs in serialized CHIP context.
 * Use open to acquire exclusive ownership of the persistent-profile wrapper.
 * This module does not connect, scan or deliver Matter callbacks. Raw Flash
 * writers and live network admission need separate fencing. Do not copy live transactions.
 */
struct br_wifi_transaction {
    struct br_wifi_profile committed, staged;
    bool committed_present, staged_present, initialized, busy, closed, uncertain;
    const void *storage_owner;
};

/* Driver startup: atomically claim wrapper ownership, then load/validate the
 * persistent profile before initializing RAM. Keep t alive until reboot even
 * on error or close; claim cannot be released. Read failure seals uncertainty.
 * No live Wi-Fi ownership is acquired by this operation. */
int br_wifi_transaction_open(struct br_wifi_transaction *t);

/* Low-level RAM initialization; production drivers should use open instead.
 * NULL initial/profile means no configured network. Initial must be a verified
 * persistent snapshot. Invalid input/state returns -1 without changing state. */
int br_wifi_transaction_init(struct br_wifi_transaction *t, const struct br_wifi_profile *initial);
int br_wifi_transaction_stage(struct br_wifi_transaction *t, const struct br_wifi_profile *profile);
int br_wifi_transaction_get(const struct br_wifi_transaction *t, struct br_wifi_profile *profile, bool *present);
/* Reverts staged RAM only. The driver must separately restore live association
 * and report rollback failure; this is NOT a completed network rollback. */
int br_wifi_transaction_revert(struct br_wifi_transaction *t);
/* Only after commissioning complete. Any persistence error seals the state as
 * uncertain and wipes snapshots; never re-init from an old RAM snapshot. */
int br_wifi_transaction_commit(struct br_wifi_transaction *t);
/* Terminal close. Busy returns 1 and defers wiping borrowed data until commit
 * returns; admission closes immediately. Owner must remain alive throughout. */
int br_wifi_transaction_close(struct br_wifi_transaction *t);

#ifdef __cplusplus
}
#endif
