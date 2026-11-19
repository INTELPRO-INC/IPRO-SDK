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
#include <stdbool.h>
#include <stdint.h>
#include <openthread/dataset.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t version; /* Thread Version TLV value, not certification status. */
    bool interface_enabled; /* OT IPv6 interface enabled; not attachment/BR health. */
    uint8_t border_agent_id[16];
    char service_name[64]; /* Actual MeshCoP DNS instance label, NUL terminated. */
} ipro_matter_br_thread_state;

/* Use from task context outside the SDK OT mutex. Each operation acquires the
 * one SDK instance nonblockingly: unready => INVALID_STATE, locked => BUSY.
 * Caller exclusively owns output until return. Failure clears the output.
 * Border Agent ID uses OT's persistent identity API (may initialize its ID).
 * No callbacks, new OT instance, or live network configuration changes. */
otError ipro_matter_br_thread_read(ipro_matter_br_thread_state *output);

/* Active/pending dataset contains credentials. Never log it; caller must wipe
 * its copy after use. Bytes outside mLength are zeroed. NOT_FOUND is preserved.
 * Capacity is checked, not TLV semantic validity; consumers must parse it.
 * Each read is independent, not a transaction with a preceding state read. */
otError ipro_matter_br_thread_dataset_read(bool pending, otOperationalDatasetTlvs *output);

/* Pending network change, not initial provisioning or failsafe rollback.
 * Requires an existing commissioned dataset. Input must contain valid TLVs,
 * including active/pending timestamps and delay. Caller owns immutable input
 * for this call and must erase its credentials afterwards. A private copy is
 * wiped before return. Uses the existing SDK OT mutex without waiting; no
 * callbacks or second OT instance. Success means OT accepted/stored the pending
 * dataset, NOT that its delay elapsed or that migration completed. Caller must
 * separately serialize Matter lifecycle and other live configuration writers.
 * Storage and parse failures propagate; BUSY has made no configuration change. */
otError ipro_matter_br_thread_dataset_set_pending(const otOperationalDatasetTlvs *input);

/* Single static initial-provisioning transaction. Calls require a serialized
 * Matter driver task holding its CHIP lock, not the SDK OT mutex. Each takes
 * the existing OT mutex nonblockingly. open claims CLI configuration ownership
 * until reboot, but does not fence arbitrary direct SDK callers. SDK startup
 * must have recovered the persistent activation journal before starting OT.
 * begin only accepts an absent active AND pending dataset; it records intent
 * before setting credentials/enabling Thread. Errors may leave a partial
 * operation: revert is required, never assume the old live state survived.
 * attached checks the requested dataset and Thread role, not backhaul health.
 * commit is authorized only by CommissioningComplete. revert disables Thread
 * and IPv6, verifies deletion of both datasets, then clears durable intent.
 * Errors retain the journal/transaction for retry. Neither operation erases
 * all OT settings, Wi-Fi or Matter credentials. Input credentials are copied;
 * the caller must wipe its own input. No callbacks or extra OT instance. */
otError ipro_matter_br_thread_activation_open(void);
otError ipro_matter_br_thread_activation_begin(const otOperationalDatasetTlvs *input);
otError ipro_matter_br_thread_activation_attached(bool *attached);
otError ipro_matter_br_thread_activation_commit(void);
otError ipro_matter_br_thread_activation_revert(void);

#ifdef __cplusplus
}
#endif
