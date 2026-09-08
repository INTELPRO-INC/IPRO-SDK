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

#ifndef __IPRO7_PHY_H__
#define __IPRO7_PHY_H__

#include <stdint.h>
#include <stdbool.h>

void bz_phy_reset();
void bz_phy_reset_lp(void);
void bz_phy_reset_lp_zb(void);
void bz_phy_refresh_ble_mdm(void);
void bz_phy_force_agc_enable(bool en);
bool bz_phy_set_tx_power(int power_dbm);
bool bz_phy_set_tx_power_offset(int8_t pwr_offset_zb[16], int8_t pwr_offset_ble[4]);
int bz_phy_get_tx_power(void);
bool bz_phy_optimize_tx_channel(uint32_t channel); // MHz
int16_t bz_phy_get_freq_offset_zb(void);
void bz_phy_start_cont_rx_zb();
void bz_phy_stop_cont_rx_zb();
void bz_phy_cw_switch_channel(double cw_freq_hz);
void bz_phy_cw_start();
void bz_phy_cw_stop();

/* BZ_PHY API */
void bz_phy_reset(void);
bool bz_phy_p_mode_enabled(void);
void bz_phy_set_p_mode(uint8_t en);
int bz_phy_get_rssi(void);
int bz_phy_get_lqi(void);
uint8_t bz_phy_get_sfd_correlation(void);
int bz_phy_get_frequency_offset(void);
void bz_phy_set_ed_threshold(int threashold);
int bz_phy_get_ed_threshold(void);
void bz_phy_set_cca_mode(uint8_t mode);
uint8_t bz_phy_run_cca(int *rssi);
void bz_phy_clear_cca_status(void);
void bz_phy_enable_cca_interrupt(void);
void bz_phy_disable_cca_interrupt(void);
void bz_phy_enable_ed(void);
void bz_phy_disable_ed(void);
uint8_t bz_phy_get_ed_meas(void);

#endif
