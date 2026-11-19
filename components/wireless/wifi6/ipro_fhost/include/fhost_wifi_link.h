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

/* Optional bounded observer called synchronously on the WPA task. connected is
 * 1 only at operstate-up (WPA completed), 0 on down/disconnect/init/deinit.
 * Identity comes from the association event, not manager target credentials.
 * SSID/BSSID are borrowed only during this call; SSID can be unavailable even
 * when connected. No blocking, WPA commands or Matter calls in the observer.
 * Default is a no-op; verify a required strong override survives final linking.
 * Rebuild the source-built WPA driver when enabling this observer. */
void fhost_wpa_link_observed(int vif, uint8_t connected, const uint8_t *ssid,
                            uint32_t ssid_length, const uint8_t *bssid);

#ifdef __cplusplus
}
#endif
