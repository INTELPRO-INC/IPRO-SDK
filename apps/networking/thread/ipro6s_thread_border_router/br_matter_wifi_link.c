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

#include <ipro_matter_wifi.h>
#include <fhost_wifi_link.h>
#include <wifi_mgmr_ext.h>
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>
#include "br_wifi_link_epoch.h"

_Static_assert(sizeof(ipro_wifi_link_snapshot) == IPRO_WIFI_LINK_SNAPSHOT_SIZE, "Wi-Fi link ABI");
static ipro_wifi_link_snapshot link_snapshot;
static uint32_t link_epoch;

void fhost_wpa_link_observed(int vif, uint8_t connected, const uint8_t *ssid,
                            uint32_t ssid_length, const uint8_t *bssid)
{
    if (vif != MGMR_VIF_STA) return;
    ipro_wifi_link_snapshot next = {0};
    next.observed = 1;
    next.connected = connected != 0;
    if (connected && ssid && ssid_length > 0 && ssid_length <= sizeof(next.ssid) && bssid) {
        uint8_t any = 0;
        for (unsigned i = 0; i < sizeof(next.bssid); ++i) any |= bssid[i];
        if (any && !(bssid[0] & 1u)) {
            next.identity_valid = 1;
            next.ssid_length = (uint8_t)ssid_length;
            memcpy(next.ssid, ssid, ssid_length);
            memcpy(next.bssid, bssid, sizeof(next.bssid));
        }
    }
    /* One WPA task publishes; short single-core critical section also protects
     * the CHIP reader. No locks, credential reads, callbacks or allocation. */
    taskENTER_CRITICAL();
    if (link_epoch != UINT32_MAX) ++link_epoch;
    link_snapshot = next;
    taskEXIT_CRITICAL();
}

uint32_t br_wifi_link_epoch(void)
{
    taskENTER_CRITICAL();
    uint32_t epoch = link_snapshot.connected && link_epoch != UINT32_MAX ? link_epoch : 0;
    taskEXIT_CRITICAL();
    return epoch;
}

int ipro_wifi_link_read(ipro_wifi_link_snapshot *output, uint32_t size)
{
    if (!output || size != sizeof(*output)) return -1;
    memset(output, 0, sizeof(*output));
    if (xPortIsInsideInterrupt() || xTaskGetSchedulerState() != taskSCHEDULER_RUNNING ||
        !ipro_wifi_connectivity_in_chip_context()) return -1;
    taskENTER_CRITICAL();
    *output = link_snapshot;
    taskEXIT_CRITICAL();
    return 0;
}
