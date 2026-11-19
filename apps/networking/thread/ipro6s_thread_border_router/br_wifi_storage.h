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
#include <stddef.h>
#include "br_wifi_profile.h"

#define BR_WIFI_KEY "br-wifi-v1"

#ifdef __cplusplus
extern "C" {
#endif

/* Task-only, serialized writes to the boot-profile key. The caller validates
 * profile format and storage readiness. -1 rejects admission; otherwise the
 * return value is the checked EasyFlash result (clear treats NOT_FOUND as
 * success). No flash access occurs under IRQ lock. A failed save may have made
 * partial persistent progress; callers must recover/re-read before claiming
 * the previously committed configuration is intact. */
int br_wifi_storage_save(const void *profile, size_t size);
int br_wifi_storage_clear(void);

/* Checked and format-validated snapshot. Missing key returns 0/present=false;
 * corruption and I/O errors propagate. Valid output pointers are cleared on
 * rejected admission or read failure. Uses the same gate as profile writers,
 * so no read can outlive a successful terminal reset fence. */
int br_wifi_storage_load(struct br_wifi_profile *profile, bool *present);

/* Claim while idle using a stable, non-NULL owner token (never a task handle).
 * Owner lifetime extends until reboot. No release/transfer is supported.
 * After success, legacy NULL-owner APIs reject all access. This protects this
 * key's wrapper APIs only, not raw EasyFlash calls or live Wi-Fi operations.
 * Return 0 on claim/idempotent claim, -1 on busy/closed/invalid/other owner. */
int br_wifi_storage_claim(const void *owner);
int br_wifi_storage_save_owned(const void *owner, const void *profile, size_t size);
int br_wifi_storage_clear_owned(const void *owner);
int br_wifi_storage_load_owned(const void *owner, struct br_wifi_profile *profile, bool *present);

/* Terminal reset fence: closes admission atomically, then reports 1 while an
 * admitted flash read/write operation is active, 0 when drained, -1 for invalid context.
 * Retry from the reset worker. There is intentionally no reopen operation;
 * the fence survives deletion failures until reboot. This fences ONLY Wi-Fi
 * profile storage, not OT writers or live network activity. */
int br_wifi_storage_fence(void);

#ifdef __cplusplus
}
#endif
