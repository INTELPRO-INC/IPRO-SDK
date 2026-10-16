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
 * @file network_wifi_aic.h
 * @brief AIC8800 D80-H SDIO Wi-Fi network backend — network_ops_t implementation
 */
#ifndef NETWORK_WIFI_AIC_H
#define NETWORK_WIFI_AIC_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const network_ops_t network_wifi_aic_ops;

/** Store STA credentials (EasyFlash "wifi_ssid"/"wifi_psk"); 0 on success. */
int network_wifi_aic_set_credentials(const char *ssid, const char *psk);

#ifdef __cplusplus
}
#endif
#endif /* NETWORK_WIFI_AIC_H */
