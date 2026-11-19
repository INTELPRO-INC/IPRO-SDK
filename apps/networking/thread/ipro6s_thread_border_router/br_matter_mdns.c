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

#include "br_matter_mdns.h"
#include <openthread/mdns.h>
#include <string.h>

otError br_matter_mdns_prepare(otInstance *instance, const uint8_t mac[6])
{
    static const char hex[] = "0123456789ABCDEF";
    char name[13];
    uint8_t any = 0;
    if (!instance || !mac || (mac[0] & 1)) return OT_ERROR_INVALID_ARGS;
    for (unsigned i = 0; i < 6; ++i) {
        any |= mac[i];
        name[2 * i] = hex[mac[i] >> 4];
        name[2 * i + 1] = hex[mac[i] & 15];
    }
    if (!any) return OT_ERROR_INVALID_ARGS;
    name[12] = '\0';
    if (otMdnsIsEnabled(instance)) {
        const char *current = otMdnsGetLocalHostName(instance);
        return current && strcmp(current, name) == 0 ? OT_ERROR_NONE : OT_ERROR_INVALID_STATE;
    }
    /* Use the local host: OT's custom host registration only publishes AAAA,
     * while br_mdns reports both IPv4 and IPv6 for the local host. */
    return otMdnsSetLocalHostName(instance, name);
}
