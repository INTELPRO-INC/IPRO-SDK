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

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "hal_wireless.h"

typedef struct _hal_wireless_env {
    uint8_t mac_addr[8];
    int8_t power_offset_zigbee[16];
    int8_t power_offset_ble[4];
    int8_t capcode_temp[MAX_CAPCODE_TABLE_SIZE];
    int8_t capcode_offset[MAX_CAPCODE_TABLE_SIZE];
    uint8_t capcode_size;
    int8_t default_tx_power;
    uint8_t power_tcal_en;
    uint8_t capcode_tcal_en;
} hal_wireless_env_t;

//ATTR_HBN_DATA_SECTION
hal_wireless_env_t wireless_env;


int hal_wireless_mac_addr_set(uint8_t mac[8])
{
    memcpy(wireless_env.mac_addr, mac, 8);
    return 0;
}

int hal_wireless_mac_addr_get(uint8_t mac[8])
{
    memcpy(mac, wireless_env.mac_addr, 8);
    return 0;
}

int hal_wireless_power_offset_set(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[4])
{
    memcpy(wireless_env.power_offset_zigbee, poweroffset_zigbee, 16);
    memcpy(wireless_env.power_offset_ble, poweroffset_ble, 4);
    return 0;
}

int hal_wireless_power_offset_get(int8_t poweroffset_zigbee[16], int8_t poweroffset_ble[4])
{
    memcpy(poweroffset_zigbee, wireless_env.power_offset_zigbee, 16);
    memcpy(poweroffset_ble, wireless_env.power_offset_ble, 4);
    return 0;
}

int hal_wireless_capcode_offset_table_set(int8_t temp[], int8_t offset[], uint8_t size)
{
    if(size > MAX_CAPCODE_TABLE_SIZE){
        return -1;
    }

    memcpy(wireless_env.capcode_temp, temp, size);
    memcpy(wireless_env.capcode_offset, offset, size);
    wireless_env.capcode_size = size;
    return 0;
}

int hal_wireless_capcode_offset_table_get(int8_t temp[], int8_t offset[], uint8_t *size)
{
    memcpy(temp, wireless_env.capcode_temp, wireless_env.capcode_size);
    memcpy(offset, wireless_env.capcode_offset, wireless_env.capcode_size);
    *size = wireless_env.capcode_size;
    return 0;
}

int8_t hal_wireless_capcode_offset_get(int8_t temp)
{
    int i;

    for(i=1; i<wireless_env.capcode_size; i++){
        if(temp < wireless_env.capcode_temp[i]){
            return wireless_env.capcode_offset[i - 1];
        }
    }

    return wireless_env.capcode_offset[i - 1];
}

void hal_wireless_default_tx_power_set(int8_t power)
{
    wireless_env.default_tx_power = power;
}

int8_t hal_wireless_default_tx_power_get(void)
{
    return wireless_env.default_tx_power;
}

void hal_wireless_power_tcal_en_set(uint8_t en)
{
    wireless_env.power_tcal_en = en;
}

uint8_t hal_wireless_power_tcal_en_get(void)
{
    return wireless_env.power_tcal_en;
}

void hal_wireless_capcode_tcal_en_set(uint8_t en)
{
    wireless_env.capcode_tcal_en = en;
}

uint8_t hal_wireless_capcode_tcal_en_get(void)
{
    return wireless_env.capcode_tcal_en;
}


void rf_full_cal_start_callback(uint32_t addr, uint32_t size)
{
}

void rf_reset_done_callback(void)
{
#if 0 
#if defined(CFG_BLE_ENABLE)
#if defined(CFG_ZIGBEE_ENABLE) || defined(CFG_OPENTHREAD_ENABLE)
    rf_set_bz_mode(MODE_BZ_COEX);
#else
    rf_set_bz_mode(MODE_BLE_ONLY);
#endif
#else
    rf_set_bz_mode(MODE_ZB_ONLY);
#endif

#if defined(CFG_BLE_ENABLE) || defined(CFG_ZIGBEE_ENABLE) || defined(CFG_OPENTHREAD_ENABLE)
    bz_phy_set_tx_power_offset(wireless_env.power_offset_zigbee, wireless_env.power_offset_ble);
#endif

#if defined(CFG_TCAL_ENABLE)
    extern int hal_tcal_restart(void);
    hal_tcal_restart();
#endif
#endif
}
