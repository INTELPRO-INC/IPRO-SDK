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

#include "br_wifi_profile.h"
#include <string.h>

bool br_wifi_profile_valid(const struct br_wifi_profile *p, size_t length)
{
    if (!p || length != sizeof(*p) || memcmp(p->magic, "BW01", 4) ||
        p->ssid_len == 0 || p->ssid_len > sizeof(p->ssid) ||
        p->key_len < 8 || p->key_len > sizeof(p->key) || p->pmf > 2 || p->akm > 2)
        return false;
    for (unsigned i = 0; i < p->ssid_len; ++i)
        if (p->ssid[i] == 0) return false; /* Manager also uses C strings. */
    for (unsigned i = 0; i < p->key_len; ++i) {
        uint8_t ch = p->key[i];
        if (ch < 0x20 || ch > 0x7e) return false;
        if (p->key_len == 64 && !((ch >= '0' && ch <= '9') ||
            (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))) return false;
    }
    return true;
}

void br_wifi_profile_wipe(void *data, size_t length)
{
    volatile uint8_t *p = data;
    while (length--) *p++ = 0;
}
