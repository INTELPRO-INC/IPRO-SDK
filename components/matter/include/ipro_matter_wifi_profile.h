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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPRO_WIFI_PROFILE_SIZE 100u
/* Byte-only C ABI, independent of the SDK's short-enum compiler option.
 * Personal WPA only: security 0=manager default, 1=WPA2, 2=WPA3; pmf 0..2.
 * SSID: 1..32 non-NUL bytes. Key: 8..63 printable ASCII bytes or 64 hex bytes.
 * This interface does not encrypt Flash. Never log profiles. */
typedef struct {
    uint8_t ssid[32], key[64];
    uint8_t ssid_length, key_length, security, pmf;
} ipro_wifi_profile;

/* All calls require the Matter stack lock (including Init before its event
 * loop starts). One static SDK transaction owns the persistent-profile wrapper
 * until reboot. No second Matter KVS copy is needed. This does not acquire
 * ownership of live Wi-Fi operations or fence raw Flash writers.
 * Returns 0 on success, -1 for invalid context/input/state, or storage error.
 * Read failures seal uncertainty; open cannot restart an initialized/closed or
 * uncertain transaction. A failed storage claim may be retried. */
int ipro_wifi_profile_open(void);
/* Read staged configuration. present is 0 or 1. Output and present must not
 * overlap. With a valid-size output, failure wipes it; a size mismatch leaves
 * that buffer untouched. Caller owns and must wipe returned credentials. */
int ipro_wifi_profile_read(ipro_wifi_profile *out, uint32_t size, uint8_t *present);
/* Input remains caller-owned and must be immutable during this call. */
int ipro_wifi_profile_stage(const ipro_wifi_profile *profile, uint32_t size);
int ipro_wifi_profile_remove(void);
/* RAM-only revert: driver must separately restore the actual association,
 * including the no-network case. Success is NOT completed network rollback. */
int ipro_wifi_profile_revert(void);
/* Call only on CommissioningComplete. Persistence failure seals uncertainty
 * and wipes snapshots; do not claim rollback from an old RAM copy succeeded. */
int ipro_wifi_profile_commit(void);
/* Terminal close; 1 means an admitted storage operation must finish before
 * wiping completes. Does not release ownership or erase persistent settings. */
int ipro_wifi_profile_close(void);

#ifdef __cplusplus
}
#endif
