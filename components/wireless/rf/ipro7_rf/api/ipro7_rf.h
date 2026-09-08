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

#ifndef _IPRO7_RF_PUBLIC_H_
#define _IPRO7_RF_PUBLIC_H_

#include <stdint.h>

uint32_t rf_set_init_tsen_value(int16_t tsen_value);
uint32_t rf_inc_cal_tsen_based(int16_t tsen_value);

// callback function, should be implemented by user
void rf_reset_done_callback(void);
void rf_full_cal_start_callback(uint32_t addr, uint32_t size);

#ifdef CFG_FPGA
void rf_fpga_init();
#endif

/* RF API */
enum{
	MODE_BLE_ONLY = 0,
    MODE_ZB_ONLY = 1,
    MODE_BZ_COEX = 2,
};
void rf_set_bz_mode(uint8_t mode);

typedef struct {
    uint32_t requested_channel_mhz;
    uint32_t effective_channel_mhz;
    uint32_t bt_index;
    uint32_t bt_kcal;
    int32_t  zb_lut_index;
    uint32_t zb_kcal;
    int32_t  tx_power_ble_dbm;
    int32_t  tx_power_zb_dbm;
    int32_t  tx_pwr_offset_zb;
    uint32_t ctrl_source;
    uint32_t pucr;
    uint32_t pucr_tx;
    uint32_t pucr_rx;
    uint32_t acal_config;
    uint32_t kcal1;
    uint32_t kcal2;
    uint32_t lo_fc_config1;
    uint32_t adpll_output;
    uint32_t pa_gain_ctrl4;
    uint32_t pa_gain_ctrl5;
    uint32_t pa_gain_ctrl6;
    uint32_t pa_gain_ctrl7;
    uint32_t phy_status8;
    uint32_t phy_status9;
    uint32_t phy_status8_rf_alias;
    uint32_t phy_status9_rf_alias;
} rf_diag_t;

void rf_get_diag(uint32_t channel_mhz, rf_diag_t *diag);

// RF calibration control
void rf_trigger_full_calibration(void);

void rf_set_tx_bypass(uint8_t en);
uint8_t rf_get_rf_state(void);

#endif
