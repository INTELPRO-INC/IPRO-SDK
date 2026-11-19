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

#ifndef BR_WIFI_PROFILE_H
#define BR_WIFI_PROFILE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Stable byte-only disk format. Personal WPA only; no pointers/ABI padding.
 * akm: 0=manager default, 1=WPA2, 2=WPA3. Flash is not encrypted by this code. */
struct br_wifi_profile {
    uint8_t magic[4];
    uint8_t ssid_len, key_len, pmf, akm;
    uint8_t ssid[32];
    uint8_t key[64];
};
bool br_wifi_profile_valid(const struct br_wifi_profile *p, size_t length);
void br_wifi_profile_wipe(void *data, size_t length);
#endif
