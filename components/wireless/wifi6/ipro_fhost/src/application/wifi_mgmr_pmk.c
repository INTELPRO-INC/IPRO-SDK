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

#include "wifi_mgmr_pmk.h"

#include <string.h>
#include <rtos_al.h>

static struct wifi_mgmr_pmk_tag pmk_env;

int wifi_mgmr_pmk_cache_entry_update(const wifi_mgmr_pmk_cache_entry *entry)
{
    int ret;

    ret = wifi_mgmr_pmk_cache_entry_restore(entry);
    if (ret == 0) {
        wifi_mgmr_pmk_cache_entry_updated();
    }

    return ret;
}

int wifi_mgmr_pmk_cache_entry_restore(const wifi_mgmr_pmk_cache_entry *entry)
{
    int ret = -1;
    uint32_t ppl;

    if (!entry) {
        goto exit;
    }

    ppl = rtos_protect();
    if (pmk_env.cache_entry_valid) {
        if (!memcmp(&pmk_env.entry, entry, sizeof(*entry))) {
            ret = 1;
            goto same;
        }
    }
    pmk_env.cache_entry_valid = true;
    memcpy(&pmk_env.entry, entry, sizeof(*entry));
    ret = 0;
same:
    rtos_unprotect(ppl);
exit:
    return ret;
}

int wifi_mgmr_pmk_cache_entry_read(wifi_mgmr_pmk_cache_entry *entry)
{
    int ret = -1;
    uint32_t ppl;

    if (!entry) {
        goto exit;
    }

    ppl = rtos_protect();
    if (!pmk_env.cache_entry_valid) {
        goto invalid;
    }
    memcpy(entry, &pmk_env.entry, sizeof(*entry));
    ret = 0;
invalid:
    rtos_unprotect(ppl);
exit:
    return ret;
}

void wifi_mgmr_pmk_cache_entry_invalidate(void)
{
    uint32_t ppl;

    ppl = rtos_protect();
    pmk_env.cache_entry_valid = false;
    memset(&pmk_env.entry, 0, sizeof(pmk_env.entry));
    rtos_unprotect(ppl);

    wifi_mgmr_pmk_cache_entry_invalidated();
}

WIFI_MGMR_PMK_ATTR_WEAK void wifi_mgmr_pmk_cache_entry_updated(void)
{
}

WIFI_MGMR_PMK_ATTR_WEAK void wifi_mgmr_pmk_cache_entry_invalidated(void)
{
}
