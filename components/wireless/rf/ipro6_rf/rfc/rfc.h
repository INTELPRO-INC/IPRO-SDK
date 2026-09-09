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

#ifndef _IPRO6_RFC_H_
#define _IPRO6_RFC_H_

#include "co_int.h"
#include "rfc_helper.h"

void modem_init(uint32_t xtal_freq_hz);
void modem_restore(uint32_t xtal_freq_hz);
void rfc_config_channel(uint16_t channel_freq);

bool rfc_config_power_ble(int32_t pwr_dbm);
uint32_t rfc_get_power_level(uint8_t mode, int32_t power);
void rfc_apply_tx_power_offset(uint8_t channel,int8_t power_offset[14]);
void rfc_init(uint32_t xtalfreq, uint8_t cold);

#endif
