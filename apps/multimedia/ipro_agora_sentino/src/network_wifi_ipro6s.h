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

/**
 * @file network_wifi_ipro6s.h
 * @brief IPRO6S_FIN SDIO Wi-Fi network backend — network_ops_t implementation
 */
#ifndef NETWORK_WIFI_IPRO6S_H
#define NETWORK_WIFI_IPRO6S_H

#include <stdbool.h>
#include <stdint.h>

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char   ssid[33];   /* NUL-terminated, empty for a hidden network */
    int8_t rssi;       /* dBm */
    bool   secure;     /* anything but open */
} network_wifi_ap_t;

extern const network_ops_t network_wifi_ipro6s_ops;

/** Store STA credentials (EasyFlash "wifi_ssid"/"wifi_psk"); 0 on success. */
int network_wifi_ipro6s_set_credentials(const char *ssid, const char *psk);

/**
 * Scan for APs through the fin, bringing the SDIO transport up first if it is
 * not (it stays up for connect()). Blocks up to the bring-up time plus
 * timeout_ms; call from a task, never from a BT or ipronet callback.
 * @return number of entries written to aps (unsorted), or -1
 */
int network_wifi_ipro6s_scan(network_wifi_ap_t *aps, int max, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
#endif /* NETWORK_WIFI_IPRO6S_H */
