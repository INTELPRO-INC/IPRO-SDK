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

#ifndef __HAL_WIRELESS_H__
#define __HAL_WIRELESS_H__

#include <stdint.h>

#define MAX_CAPCODE_TABLE_SIZE     30

int hal_wireless_mac_addr_set(uint8_t mac[8]);
int hal_wireless_mac_addr_get(uint8_t mac[8]);
int hal_wireless_power_offset_set(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[4]);
int hal_wireless_power_offset_get(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[4]);
int hal_wireless_capcode_offset_table_set(int8_t temp[], int8_t offset[], uint8_t size);
int hal_wireless_capcode_offset_table_get(int8_t temp[], int8_t offset[], uint8_t *size);
int8_t hal_wireless_capcode_offset_get(int8_t temp);
void hal_wireless_default_tx_power_set(int8_t power);
int8_t hal_wireless_default_tx_power_get(void);
void hal_wireless_power_tcal_en_set(uint8_t en);
uint8_t hal_wireless_power_tcal_en_get(void);
void hal_wireless_capcode_tcal_en_set(uint8_t en);
uint8_t hal_wireless_capcode_tcal_en_get(void);

#endif
