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

#ifndef __UTILS_PSK_FAST_H__
#define __UTILS_PSK_FAST_H__

int utils_wifi_psk_cal_fast(char *password, char *ssid, int ssid_len, char *output);
int utils_wifi_psk_cal_fast_bin(char *password, unsigned char *ssid, int ssidlength, unsigned char *output);

#endif /* __UTILS_PSK_FAST_H__ */
