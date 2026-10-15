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

/* Optional strong SDK override; default is a no-op. Called on the FHOST
 * control task for an accepted nonzero request ID when its firmware scan CFM
 * arrives. result is WIFI_SCAN_RESULT_SUCCESS/FAILED, count is firmware's raw
 * result count (not the cache snapshot count). No callback on rejected request.
 * The callback may precede the submitting task's return. It must only publish
 * bounded state and return: no CHIP calls, blocking, scan submission, or task
 * deletion. This direct hook does not depend on the lossy platform event queue.
 *
 * IDs are caller-owned and must never be reused before reboot. Zero is legacy,
 * untracked scanning. There is no cancellation or deadline here. Do not reset
 * or reinitialize the VIF/control task with a scan outstanding: firmware CFMs
 * carry a VIF index, not this ID, so IDs do NOT detect stale CFMs across reset.
 * Callers must separately fence competing network writers and drain teardown.
 * This changes the host cfgrwnx_scan ABI: rebuild all host producers together;
 * an older CONFIG_WIFI6_PREBUILT archive is not compatible. Verify the strong
 * override is retained by the final firmware link (the weak default is no-op). */
void fhost_scan_request_completed(int vif, uint32_t request_id, uint32_t result, uint32_t count);

struct fhost_scan_observation {
    uint8_t ssid[32], bssid[6];
    uint8_t ssid_length, channel;
    int8_t rssi;
    uint8_t auth; /* WIFI_EVENT_BEACON_IND_AUTH_* classification. */
};
/* Parsed beacon/probe response from the active nonzero request, before CFM.
 * Same control-task restrictions as completed(). entry is borrowed only during
 * this call. Never emitted from the aged manager cache. Firmware ordering and
 * the no-VIF-reset requirement above also apply to these result indications. */
void fhost_scan_request_observed(int vif, uint32_t request_id, const struct fhost_scan_observation *entry);

#ifdef __cplusplus
}
#endif
