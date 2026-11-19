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

#include <ipro_matter_wifi_scan.h>
#include <ipro_matter_wifi.h>
#include "FreeRTOS.h"
#include "task.h"
#include "wifi_mgmr.h"
#include "rtos_al.h"
#include <stddef.h>
#include <string.h>

_Static_assert(sizeof(ipro_wifi_scan_entry) == IPRO_WIFI_SCAN_ENTRY_SIZE, "scan entry ABI");
_Static_assert(sizeof(ipro_wifi_scan_snapshot) == IPRO_WIFI_SCAN_SNAPSHOT_SIZE, "scan snapshot ABI");
_Static_assert(offsetof(ipro_wifi_scan_snapshot, entries) == 4, "scan array ABI");
_Static_assert(WIFI_MGMR_SCAN_ITEMS_MAX <= IPRO_WIFI_SCAN_CAPACITY, "snapshot must fit entire cache");
_Static_assert(WIFI_EVENT_BEACON_IND_AUTH_OPEN == 0 && WIFI_EVENT_BEACON_IND_AUTH_WEP == 1 &&
               WIFI_EVENT_BEACON_IND_AUTH_WPA_PSK == 2 && WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK == 3 &&
               WIFI_EVENT_BEACON_IND_AUTH_WPA_WPA2_PSK == 4 && WIFI_EVENT_BEACON_IND_AUTH_WPA_ENT == 5 &&
               WIFI_EVENT_BEACON_IND_AUTH_WPA3_SAE == 6 && WIFI_EVENT_BEACON_IND_AUTH_WPA2_PSK_WPA3_SAE == 7 &&
               WIFI_EVENT_BEACON_IND_AUTH_UNKNOWN == 255, "scan auth ABI requires explicit remapping");

int ipro_wifi_scan_cache_read(ipro_wifi_scan_snapshot *out, uint32_t size)
{
    if (!out || size != sizeof(*out)) return -1;
    memset(out, 0, sizeof(*out));
    if (xPortIsInsideInterrupt() || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING ||
        !ipro_wifi_connectivity_in_chip_context() || !wifiMgmr.scan_items_lock) return -1;
    if (rtos_mutex_lock_timeout(wifiMgmr.scan_items_lock, 0) != 0) return 1;
    if (wifiMgmr.scan_item_timeout <= 0) {
        rtos_mutex_unlock(wifiMgmr.scan_items_lock);
        return -1;
    }
    /* Match the manager cache's tick-based age calculation, including wrap. */
    const uint32_t now = rtos_now(false);
    const uint32_t timeout = (uint32_t)wifiMgmr.scan_item_timeout;
    for (unsigned i = 0; i < WIFI_MGMR_SCAN_ITEMS_MAX; ++i) {
        const wifi_mgmr_scan_item_t *item = &wifiMgmr.scan_items[i];
        if (!item->is_used || (uint32_t)(now - item->timestamp_lastseen) >= timeout ||
            item->ssid_len < 0 || item->ssid_len > 32 || item->channel == 0 || item->channel > 14)
            continue;
        ipro_wifi_scan_entry *entry = &out->entries[out->count++];
        entry->ssid_length = (uint8_t)item->ssid_len;
        memcpy(entry->ssid, item->ssid, entry->ssid_length);
        memcpy(entry->bssid, item->bssid, sizeof(entry->bssid));
        entry->channel = item->channel;
        entry->rssi = item->rssi;
        entry->auth = item->auth <= 7 ? item->auth : 255;
    }
    rtos_mutex_unlock(wifiMgmr.scan_items_lock);
    return 0;
}
