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

#ifndef RF_CALIB_DATA_H
#define RF_CALIB_DATA_H

#include "rf_pri.h"

/// Definitions of RF without LO calibration structure
typedef struct
{
    uint32_t gpadc_oscode      : 12;

    uint32_t rx_offset_i       : 10;
    uint32_t rx_offset_q       : 10;

    uint32_t rbb_cap1_fc_i     : 6;
    uint32_t rbb_cap1_fc_q     : 6;
    uint32_t rbb_cap2_fc_i     : 6;
    uint32_t rbb_cap2_fc_q     : 6;    
    uint32_t reserved          : 8; // 32bit required       

    uint32_t tx_dc_comp_i      : 12;
    uint32_t tx_dc_comp_q      : 12;
    uint32_t reserved1         : 8;       

    uint32_t tmx_cs            : 4;
    uint32_t txpwr_att_rec     : 3;
    uint32_t pa_pwrmx_osdac    : 4;
    uint32_t tmx_csh           : 4;
    uint32_t tmx_csl           : 4;
    uint32_t reserved2         : 13;       

    uint32_t tsen_refcode_rfcal  : 12;
    uint32_t tsen_refcode_corner : 12;
    uint32_t reserved3           : 8;       

    uint32_t rc32k_code_fr_ext : 10;
    uint32_t rc32m_code_fr_ext : 8;
    uint32_t saradc_oscode     : 10;
    uint32_t fcal_4osmx        : 4;

} rf_calib1_tag;

/// Definitions of LO dependent calibration structure #pragma pack (2)
typedef struct 
{
    // uint16_t acal       : 6;
    uint16_t acal       : 8;
    uint16_t fcal       : 8;
} rf_calib2_tag;

// 
// 6,6,11,10 -> 16,16,16 since bits-field of 16 to save size
typedef struct 
{
    uint16_t rosdac_i          : 6;
    uint16_t rosdac_q          : 6;
    uint16_t reserved          : 4;
    uint16_t rx_iq_gain_comp   : 11;
    uint16_t reserved1         : 5;   
    int16_t  rx_iq_phase_comp  : 10;    
    uint16_t reserved2         : 6;   
} rf_calib3_tag;

// 
typedef struct 
{
    uint16_t tosdac_i          : 6;
    uint16_t tosdac_q          : 6;
    uint16_t reserved          : 4;
    uint16_t tx_iq_gain_comp   : 11;
    uint16_t reserved1         : 5;  
    int16_t  tx_iq_phase_comp  : 10;
    uint16_t reserved2         : 6;  
} rf_calib4_tag;

typedef struct 
{
    uint16_t bz_tosdac_i          : 6;
    uint16_t bz_tosdac_q          : 6;
    uint16_t reserved             : 4;  
    uint16_t bz_tx_iq_gain_comp   : 11;
    uint16_t reserved1            : 5;  
    int16_t  bz_tx_iq_phase_comp  : 10;
    uint16_t reserved2            : 6;  
} rf_calib5_tag;

typedef struct 
// txcal for Temperature 
{
    uint16_t tx_iq_gain_comp   : 11;
    uint16_t reserved          : 5;
    int16_t  tx_iq_phase_comp  : 10;
    uint16_t reserved1         : 6;      
} rf_calib6_tag;


/*
* rf cal for low power 
* start
*/

typedef struct
{
    uint16_t rbb_cap1_fc_i     : 6;
    uint16_t rbb_cap1_fc_q     : 6;
    uint16_t reserved          : 4;
    uint16_t rbb_cap2_fc_i     : 6;
    uint16_t rbb_cap2_fc_q     : 6;    
    uint16_t reserved1         : 4;
} lp_rf_calib1_tag;

typedef struct 
{
    // uint16_t acal       : 6;
    uint16_t acal       : 8;
    uint16_t fcal       : 8;
} lp_rf_calib2_tag;

typedef struct 
{
    uint16_t rosdac_i          : 6;
    uint16_t rosdac_q          : 6;
    uint16_t reserved          : 4;  
    uint16_t rx_iq_gain_comp   : 11;
    uint16_t reserved1         : 5;  
    int16_t rx_iq_phase_comp   : 10;      
    uint16_t reserved2         : 6;      
} lp_rf_calib3_tag;

/*
* rf cal for low power 
* end
*/


/// Definitions of the RF calibration result structure
typedef volatile struct
{
    uint32_t        inited;
    int16_t         temp_txcal[E_RF_TXCAL_TEMP_CNT+1];
    uint8_t         temp_idx;
    uint8_t         pre_temp_idx;
    rf_calib1_tag   cal;
    rf_calib2_tag   lo[E_RF_CHANNEL_NUM];
    rf_calib3_tag   rxcal[E_RF_RXCAL_GAIN_CNT];      // 0:rosdac_ctrl_hw1.rosdac_i/q_gc0 + rxiq_ctrl_hw1, .., 3:rosdac_ctrl_hw2.rosdac_i/q_gc3 + rxiq_ctrl_hw4
    rf_calib4_tag   txcal[E_RF_TXCAL_GAIN_CNT];       // 0:tosdac_ctrl_hw1.tbb_tosdac_i/q_gc0 + tx_iq_gain_hw0, .., 7:tosdac_ctrl_hw4.tbb_tosdac_i/q_gc7 + tx_iq_gain_hw7
    rf_calib5_tag   bz_txcal[E_RF_BZ_TXCAL_GAIN_CNT];       // 0:tosdac_ctrl_hw1.tbb_tosdac_i/q_gc0 + tx_iq_gain_hw0, .., 7:tosdac_ctrl_hw4.tbb_tosdac_i/q_gc7 + tx_iq_gain_hw7
    rf_calib6_tag   txcal_temp[E_RF_TXCAL_TEMP_CNT*E_RF_TXCAL_TEMP_GAIN_CNT];
    rf_calib6_tag   txcal_temp_origin[E_RF_TXCAL_GAIN_CNT];

    // uint32_t           lp_inited;
    // lp_rf_calib1_tag   lp_cal;
    lp_rf_calib2_tag   lp_lo[E_RF_CHANNEL_NUM];
    // lp_rf_calib3_tag   lp_rxcal[E_RF_RXCAL_GAIN_CNT];
} rf_calib_data_tag;

/// export rf calibration data
extern rf_calib_data_tag* rf_calib_data;

/// init calibration data
void rf_pri_init_calib_mem(void);

#endif
