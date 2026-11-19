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

#include <ipro_matter_wifi_profile.h>
#include <ipro_matter_wifi.h>
#include "br_wifi_transaction.h"
#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(ipro_wifi_profile) == IPRO_WIFI_PROFILE_SIZE, "Wi-Fi profile ABI size");
_Static_assert(offsetof(ipro_wifi_profile, ssid_length) == 96, "Wi-Fi profile ABI layout");

/* Permanent storage-owner address. Serialized by the caller's CHIP stack lock. */
static struct br_wifi_transaction transaction;

int ipro_wifi_profile_open(void)
{
    if (!ipro_wifi_connectivity_in_chip_context()) return -1;
    return br_wifi_transaction_open(&transaction);
}

int ipro_wifi_profile_read(ipro_wifi_profile *out, uint32_t size, uint8_t *present)
{
    if (present) *present = 0;
    if (!out || size != sizeof(*out)) return -1;
    br_wifi_profile_wipe(out, sizeof(*out));
    if (!present || !ipro_wifi_connectivity_in_chip_context()) return -1;
    struct br_wifi_profile profile = {0};
    bool exists = false;
    const int error = br_wifi_transaction_get(&transaction, &profile, &exists);
    if (error == 0 && exists) {
        out->ssid_length = profile.ssid_len;
        out->key_length = profile.key_len;
        out->security = profile.akm;
        out->pmf = profile.pmf;
        memcpy(out->ssid, profile.ssid, profile.ssid_len);
        memcpy(out->key, profile.key, profile.key_len);
        *present = 1;
    }
    br_wifi_profile_wipe(&profile, sizeof(profile));
    return error;
}

int ipro_wifi_profile_stage(const ipro_wifi_profile *input, uint32_t size)
{
    if (!input || size != sizeof(*input) || !ipro_wifi_connectivity_in_chip_context()) return -1;
    struct br_wifi_profile profile = {.magic = {'B','W','0','1'},
        .ssid_len = input->ssid_length, .key_len = input->key_length,
        .akm = input->security, .pmf = input->pmf};
    memcpy(profile.ssid, input->ssid, sizeof(profile.ssid));
    memcpy(profile.key, input->key, sizeof(profile.key));
    const int error = br_wifi_transaction_stage(&transaction, &profile);
    br_wifi_profile_wipe(&profile, sizeof(profile));
    return error;
}

int ipro_wifi_profile_remove(void)
{
    if (!ipro_wifi_connectivity_in_chip_context()) return -1;
    return br_wifi_transaction_stage(&transaction, NULL);
}

int ipro_wifi_profile_revert(void)
{
    if (!ipro_wifi_connectivity_in_chip_context()) return -1;
    return br_wifi_transaction_revert(&transaction);
}

int ipro_wifi_profile_commit(void)
{
    if (!ipro_wifi_connectivity_in_chip_context()) return -1;
    return br_wifi_transaction_commit(&transaction);
}

int ipro_wifi_profile_close(void)
{
    if (!ipro_wifi_connectivity_in_chip_context()) return -1;
    return br_wifi_transaction_close(&transaction);
}
