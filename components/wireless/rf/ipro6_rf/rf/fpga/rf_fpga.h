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

#ifndef __IPRO6_RF_FPGA_H__
#define __IPRO6_RF_FPGA_H__

#include "co_int.h"

enum {
    E_RF_XTAL_24M = 0,
    E_RF_XTAL_26M = 1,
    E_RF_XTAL_32M = 2,
    E_RF_XTAL_38M4 = 3,
    E_RF_XTAL_40M = 4,
    E_RF_XTAL_52M = 5,
};

void rf_pri_xtalfreq(uint32_t xtalfreq);
void rf_pri_init(uint8_t reset, uint8_t chipv);
uint32_t rf_pri_get_vco_freq_cw(uint32_t chanfreq_MHz);
uint32_t rf_pri_get_vco_idac_cw(uint32_t chanfreq_MHz);
uint32_t rf_pri_check_lo_lock();
void rf_pri_txtest_start();
void rf_pri_txtest_stop();
// uint32_t rf_pri_set_channel(uint32_t channel_freq);
void rf_pri_update_param(uint32_t channel_freq);
void rf_pri_get_notch_param(uint32_t chanfreq_MHz,uint8_t *ncf_on, int32_t *ncf_freq_Hz);
void rf_pri_write(uint32_t a,uint32_t d);
uint32_t rf_pri_read(uint32_t a);
void rf_pri_txcal();
void rf_pri_set_bandwidth(uint8_t bw);
uint32_t rf_pri_get_xtalfreq();
#endif
