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

#ifndef _REG_RF_H_
#define _REG_RF_H_
// Date: 20220513

#include <stdint.h>
#include "_reg_access.h"
#include "ipro7.h"

#ifndef RF_BASE
#define RF_BASE (0x30001000)
#endif



#define RF_REV_ADDR (RF_BASE + 0x00000000)

static inline uint32_t rf_rev_get(void)
{
    return REG_PL_RD(RF_REV_ADDR);
}

static inline void rf_rev_set(uint32_t x)
{
    REG_PL_WR(RF_REV_ADDR,x);
}

static inline uint32_t rf_hw_rev_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_REV_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t rf_fw_rev_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_REV_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t rf_rev_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_REV_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define DSP_READBACK_ADDR (RF_BASE + 0x00000004)

static inline uint32_t dsp_readback_get(void)
{
    return REG_PL_RD(DSP_READBACK_ADDR);
}

static inline void dsp_readback_set(uint32_t x)
{
    REG_PL_WR(DSP_READBACK_ADDR,x);
}

static inline uint32_t rf_rbb_bw_ind_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rbb_bw_ind_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_ind_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_rbb_bw_ind_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_rbb_ind_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_rbb_ind_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_rbb_ind_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x1f000000)) >> 24);
}

static inline void rf_rbb_ind_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x1f000000);
    localVal |= (x << 24)&((uint32_t)0x1f000000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_ch_ind_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_ch_ind_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_ch_ind_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf_ch_ind_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(DSP_READBACK_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_ind_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t rf_rbb_ind_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00003e00)) >> 9);
}

static inline uint32_t rf_ch_ind_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSP_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x000001fc)) >> 2);
}


#define RF_CTRL_SOURCE_ADDR (RF_BASE + 0x00000008)

static inline uint32_t rf_ctrl_source_get(void)
{
    return REG_PL_RD(RF_CTRL_SOURCE_ADDR);
}

static inline void rf_ctrl_source_set(uint32_t x)
{
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,x);
}

static inline uint32_t rf_vco_idac_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_vco_idac_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_inc_fcal_en_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_inc_fcal_en_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_lo_fcw_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_lo_fcw_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_rbb_bw_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_kcal_ratio_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_rosdac_ctrl_rccal_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_rosdac_ctrl_rccal_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_rosdac_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rosdac_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl_rx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_gain_ctrl_rx_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl_tx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_gain_ctrl_tx_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}

static inline uint32_t rf_pu_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_pu_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CTRL_SOURCE_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_CTRL_SOURCE_ADDR,localVal);
}


#define RF_CAL_STATE_CTRL_ADDR (RF_BASE + 0x0000000c)

static inline uint32_t rf_cal_state_ctrl_get(void)
{
    return REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
}

static inline void rf_cal_state_ctrl_set(uint32_t x)
{
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,x);
}

static inline uint32_t rf_inc_roscal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_inc_roscal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_inc_acal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_inc_acal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_inc_fcal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_inc_fcal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_rccal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_rccal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_roscal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_roscal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_kcal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_kcal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_acal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_acal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}

static inline uint32_t rf_fcal_state_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_fcal_state_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATE_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_CAL_STATE_CTRL_ADDR,localVal);
}


#define RF_CAL_SWITCH_CTRL_ADDR (RF_BASE + 0x00000010)

static inline uint32_t rf_cal_switch_ctrl_get(void)
{
    return REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
}

static inline void rf_cal_switch_ctrl_set(uint32_t x)
{
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,x);
}

static inline uint32_t rf_inc_fcal_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_inc_fcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_inc_fcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,localVal);
}

static inline uint32_t rf_inc_acal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_inc_acal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,localVal);
}

static inline uint32_t rf_rccal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rccal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,localVal);
}

static inline uint32_t rf_kcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_kcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,localVal);
}

static inline uint32_t rf_acal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_acal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_SWITCH_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_CAL_SWITCH_CTRL_ADDR,localVal);
}


#define RF_CAL_STATUS_ADDR (RF_BASE + 0x00000014)

static inline uint32_t rf_cal_status_get(void)
{
    return REG_PL_RD(RF_CAL_STATUS_ADDR);
}

static inline void rf_cal_status_set(uint32_t x)
{
    REG_PL_WR(RF_CAL_STATUS_ADDR,x);
}

static inline uint32_t rf_dl_rfcal_table_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_dl_rfcal_table_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(RF_CAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_rccal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_rccal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RF_CAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_roscal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_roscal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(RF_CAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_kcal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_kcal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(RF_CAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_acal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_acal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_CAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(RF_CAL_STATUS_ADDR,localVal);
}


#define PU_DELAY_CONFG_ADDR (RF_BASE + 0x00000018)

static inline uint32_t pu_delay_confg_get(void)
{
    return REG_PL_RD(PU_DELAY_CONFG_ADDR);
}

static inline void pu_delay_confg_set(uint32_t x)
{
    REG_PL_WR(PU_DELAY_CONFG_ADDR,x);
}

static inline uint32_t rf_adpll_reset_width_getf(void)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_reset_width_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(PU_DELAY_CONFG_ADDR,localVal);
}

static inline uint32_t rf_lo_reset_width_getf(void)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_reset_width_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(PU_DELAY_CONFG_ADDR,localVal);
}

static inline uint32_t rf_lo_reset_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_reset_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(PU_DELAY_CONFG_ADDR,localVal);
}

static inline uint32_t rf_pud_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_pud_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(PU_DELAY_CONFG_ADDR,localVal);
}

static inline uint32_t rf_ppu_lead_getf(void)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_ppu_lead_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PU_DELAY_CONFG_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PU_DELAY_CONFG_ADDR,localVal);
}


#define PUCR_REG_ADDR (RF_BASE + 0x00000100)

static inline uint32_t pucr_reg_get(void)
{
    return REG_PL_RD(PUCR_REG_ADDR);
}

static inline void pucr_reg_set(uint32_t x)
{
    REG_PL_WR(PUCR_REG_ADDR,x);
}

static inline uint32_t rf_rst_fbdv_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_rst_fbdv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_rst_lotpm_hfp_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_rst_lotpm_hfp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_rst_adpll_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_rst_adpll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_lna_shunt_sw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_REG_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_REG_ADDR,localVal);
}


#define PUCR_SB_ADDR (RF_BASE + 0x00000104)

static inline uint32_t pucr_sb_get(void)
{
    return REG_PL_RD(PUCR_SB_ADDR);
}

static inline void pucr_sb_set(uint32_t x)
{
    REG_PL_WR(PUCR_SB_ADDR,x);
}

static inline uint32_t rf_lna_shunt_sw_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_SB_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_SB_ADDR,localVal);
}


#define PUCR_LOTX_ADDR (RF_BASE + 0x00000108)

static inline uint32_t pucr_lotx_get(void)
{
    return REG_PL_RD(PUCR_LOTX_ADDR);
}

static inline void pucr_lotx_set(uint32_t x)
{
    REG_PL_WR(PUCR_LOTX_ADDR,x);
}

static inline uint32_t rf_lna_shunt_sw_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LOTX_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_LOTX_ADDR,localVal);
}


#define PUCR_LORX_ADDR (RF_BASE + 0x0000010c)

static inline uint32_t pucr_lorx_get(void)
{
    return REG_PL_RD(PUCR_LORX_ADDR);
}

static inline void pucr_lorx_set(uint32_t x)
{
    REG_PL_WR(PUCR_LORX_ADDR,x);
}

static inline uint32_t rf_lna_shunt_sw_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_LORX_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_LORX_ADDR,localVal);
}


#define PUCR_TX_ADDR (RF_BASE + 0x00000110)

static inline uint32_t pucr_tx_get(void)
{
    return REG_PL_RD(PUCR_TX_ADDR);
}

static inline void pucr_tx_set(uint32_t x)
{
    REG_PL_WR(PUCR_TX_ADDR,x);
}

static inline uint32_t rf_lna_shunt_sw_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_TX_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_TX_ADDR,localVal);
}


#define PUCR_RX_ADDR (RF_BASE + 0x00000114)

static inline uint32_t pucr_rx_get(void)
{
    return REG_PL_RD(PUCR_RX_ADDR);
}

static inline void pucr_rx_set(uint32_t x)
{
    REG_PL_WR(PUCR_RX_ADDR,x);
}

static inline uint32_t rf_lna_shunt_sw_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lna_shunt_sw_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_trsw_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pa_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_lna_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_rmx_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_rbb_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_pkdet_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_rbb_pkdet_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_rosdac_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxadc_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_rxadc_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rxadc_clk_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_lodist_body_bias_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_lodist_body_bias_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_ldo_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_vco_ldo_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_vco_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_fbdv_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_buf_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_fbdv_buf_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_clk_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_lotpm_hfp_clk_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_bypass_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lotpm_lfp_bypass_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_bypass_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_lotpm_hfp_bypass_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_clk_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_clk_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_adc_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pu_adpll_adc_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_adpll_sfreg_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pu_adpll_sfreg_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_dtc_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_pu_dtc_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_pu_rxbuf_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_txbuf_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}

static inline uint32_t rf_lodist_tx_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lodist_tx_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR_RX_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR_RX_ADDR,localVal);
}


#define PUCR_HW_ADDR (RF_BASE + 0x00000118)

static inline uint32_t pucr_hw_get(void)
{
    return REG_PL_RD(PUCR_HW_ADDR);
}

static inline void pucr_hw_set(uint32_t x)
{
    REG_PL_WR(PUCR_HW_ADDR,x);
}

static inline uint32_t rf_rst_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t rf_rst_lotpm_hfp_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t rf_rst_adpll_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_lna_shunt_sw_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_trsw_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t rf_pu_pa_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t rf_pu_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t rf_pu_rmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_pu_rbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t rf_pu_rbb_pkdet_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t rf_pu_rosdac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_pu_rxadc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_rxadc_clk_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t rf_pu_lodist_body_bias_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t rf_pu_vco_ldo_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t rf_pu_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_pu_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t rf_pu_fbdv_buf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t rf_lotpm_hfp_clk_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_lotpm_lfp_bypass_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_lotpm_hfp_bypass_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t rf_adpll_clk_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t rf_pu_adpll_adc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t rf_pu_adpll_sfreg_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t rf_pu_dtc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t rf_pu_rxbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t rf_pu_txbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t rf_lodist_tx_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define NON_REG_READBACK_ADDR (RF_BASE + 0x0000011c)

static inline uint32_t non_reg_readback_get(void)
{
    return REG_PL_RD(NON_REG_READBACK_ADDR);
}

static inline void non_reg_readback_set(uint32_t x)
{
    REG_PL_WR(NON_REG_READBACK_ADDR,x);
}

static inline uint32_t rf_pud_pa_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t rf_ppu_rxadc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_ppu_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_ppu_rbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t rf_ppu_lodist_body_bias_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t rf_ppu_vco_ldo_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_ppu_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t rf_pud_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t rf_ppu_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_ppu_adpll_sfreg_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_ppu_rxbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t rf_ppu_txbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t rf_ppu_testbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t rf_ppu_fbdv_buf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(NON_REG_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}


#define TRX_GAIN_BW_ADDR (RF_BASE + 0x00000120)

static inline uint32_t trx_gain_bw_get(void)
{
    return REG_PL_RD(TRX_GAIN_BW_ADDR);
}

static inline void trx_gain_bw_set(uint32_t x)
{
    REG_PL_WR(TRX_GAIN_BW_ADDR,x);
}

static inline uint32_t rf_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}

static inline uint32_t rf_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rbb_bw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}

static inline uint32_t rf_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline void rf_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x000000e0);
    localVal |= (x << 5)&((uint32_t)0x000000e0);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}

static inline uint32_t rf_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rf_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3)&((uint32_t)0x00000018);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}

static inline uint32_t rf_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(TRX_GAIN_BW_ADDR,localVal);
}


#define TRX_GAIN_BW_HW_ADDR (RF_BASE + 0x00000124)

static inline uint32_t trx_gain_bw_hw_get(void)
{
    return REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
}

static inline void trx_gain_bw_hw_set(uint32_t x)
{
    REG_PL_WR(TRX_GAIN_BW_HW_ADDR,x);
}

static inline uint32_t rf_pa_gc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline uint32_t rf_pa_ref_dac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline uint32_t rf_rbb_bw_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_gc_rbb2_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline uint32_t rf_gc_rbb1_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline uint32_t rf_gc_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_BW_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}


#define DCTEST_ACTEST_ADDR (RF_BASE + 0x00000128)

static inline uint32_t dctest_actest_get(void)
{
    return REG_PL_RD(DCTEST_ACTEST_ADDR);
}

static inline void dctest_actest_set(uint32_t x)
{
    REG_PL_WR(DCTEST_ACTEST_ADDR,x);
}

static inline uint32_t rf_ten_mbg_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_ten_mbg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_dll_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_ten_dll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_lodist_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_ten_lodist_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_pa_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_ten_pa_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_pa_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_ten_pa_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_rrf0_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_ten_rrf0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_rrf1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_ten_rrf1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_rxadc_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_ten_rxadc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_vco_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_ten_vco_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_adpll_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_ten_adpll_adc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_rbb_actest_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_ten_rbb_actest_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_rbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_ten_rbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_ten_dtc_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_ten_dtc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_atest_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_atest_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}

static inline uint32_t rf_dc_tp_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_dc_tp_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DCTEST_ACTEST_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(DCTEST_ACTEST_ADDR,localVal);
}


#define DTEST_ADDR (RF_BASE + 0x0000012c)

static inline uint32_t dtest_get(void)
{
    return REG_PL_RD(DTEST_ADDR);
}

static inline void dtest_set(uint32_t x)
{
    REG_PL_WR(DTEST_ADDR,x);
}

static inline uint32_t rf_dtest_en_dtc_in_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_dtest_en_dtc_in_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_dtc_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_dtest_en_dtc_out_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_fref_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_dtest_en_fref_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_mod4_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_dtest_en_mod4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_adpll_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_dtest_en_adpll_adc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_rxadc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_dtest_en_rxadc_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_en_rxadc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_dtest_en_rxadc_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(DTEST_ADDR,localVal);
}

static inline uint32_t rf_dtest_pulldown_getf(void)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_dtest_pulldown_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DTEST_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(DTEST_ADDR,localVal);
}


#define ADPLL_TEST_ADDR (RF_BASE + 0x00000130)

static inline uint32_t adpll_test_get(void)
{
    return REG_PL_RD(ADPLL_TEST_ADDR);
}

static inline void adpll_test_set(uint32_t x)
{
    REG_PL_WR(ADPLL_TEST_ADDR,x);
}

static inline uint32_t rf_adpll_test_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_adpll_test_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adpll_test_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADPLL_TEST_ADDR,localVal);
}

static inline uint32_t rf_adpll_test_start_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_test_start_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(ADPLL_TEST_ADDR,localVal);
}

static inline uint32_t rf_adpll_test_data_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_adpll_test_data_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(ADPLL_TEST_ADDR,localVal);
}


#define ADPLL_TEST2_ADDR (RF_BASE + 0x00000134)

static inline uint32_t adpll_test2_get(void)
{
    return REG_PL_RD(ADPLL_TEST2_ADDR);
}

static inline void adpll_test2_set(uint32_t x)
{
    REG_PL_WR(ADPLL_TEST2_ADDR,x);
}

static inline uint32_t rf_adpll_test_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_TEST2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RF_EXT_PA_ADDR (RF_BASE + 0x00000138)

static inline uint32_t rf_ext_pa_get(void)
{
    return REG_PL_RD(RF_EXT_PA_ADDR);
}

static inline void rf_ext_pa_set(uint32_t x)
{
    REG_PL_WR(RF_EXT_PA_ADDR,x);
}

static inline uint32_t rf_ext_pa_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    return ((localVal & ((uint32_t)0x01f00000)) >> 20);
}

static inline void rf_ext_pa_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    localVal &= ~((uint32_t)0x01f00000);
    localVal |= (x << 20)&((uint32_t)0x01f00000);
    REG_PL_WR(RF_EXT_PA_ADDR,localVal);
}

static inline uint32_t rf_ext_pa_lorx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    return ((localVal & ((uint32_t)0x000f8000)) >> 15);
}

static inline void rf_ext_pa_lorx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    localVal &= ~((uint32_t)0x000f8000);
    localVal |= (x << 15)&((uint32_t)0x000f8000);
    REG_PL_WR(RF_EXT_PA_ADDR,localVal);
}

static inline uint32_t rf_ext_pa_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    return ((localVal & ((uint32_t)0x00007c00)) >> 10);
}

static inline void rf_ext_pa_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    localVal &= ~((uint32_t)0x00007c00);
    localVal |= (x << 10)&((uint32_t)0x00007c00);
    REG_PL_WR(RF_EXT_PA_ADDR,localVal);
}

static inline uint32_t rf_ext_pa_lotx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    return ((localVal & ((uint32_t)0x000003e0)) >> 5);
}

static inline void rf_ext_pa_lotx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    localVal &= ~((uint32_t)0x000003e0);
    localVal |= (x << 5)&((uint32_t)0x000003e0);
    REG_PL_WR(RF_EXT_PA_ADDR,localVal);
}

static inline uint32_t rf_ext_pa_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf_ext_pa_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EXT_PA_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(RF_EXT_PA_ADDR,localVal);
}


#define CIP_LDO15_ADDR (RF_BASE + 0x00000200)

static inline uint32_t cip_ldo15_get(void)
{
    return REG_PL_RD(CIP_LDO15_ADDR);
}

static inline void cip_ldo15_set(uint32_t x)
{
    REG_PL_WR(CIP_LDO15_ADDR,x);
}

static inline uint32_t rf_vg11_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_LDO15_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_vg11_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_LDO15_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(CIP_LDO15_ADDR,localVal);
}


#define PA_ADDR (RF_BASE + 0x00000204)

static inline uint32_t pa_get(void)
{
    return REG_PL_RD(PA_ADDR);
}

static inline void pa_set(uint32_t x)
{
    REG_PL_WR(PA_ADDR,x);
}

static inline uint32_t rf_pa_cap2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0xe0000000)) >> 29);
}

static inline void rf_pa_cap2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0xe0000000);
    localVal |= (x << 29)&((uint32_t)0xe0000000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_cap1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x0e000000)) >> 25);
}

static inline void rf_pa_cap1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x0e000000);
    localVal |= (x << 25)&((uint32_t)0x0e000000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_cap1_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00e00000)) >> 21);
}

static inline uint32_t rf_pa_pu_ldo2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pa_pu_ldo2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_pu_ldo1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pa_pu_ldo1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_pa_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_para_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_pa_para_cs_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_ldo2_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_pa_ldo2_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(PA_ADDR,localVal);
}

static inline uint32_t rf_pa_ldo1_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_pa_ldo1_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PA_ADDR,localVal);
}


#define LNA_MX_ADDR (RF_BASE + 0x00000208)

static inline uint32_t lna_mx_get(void)
{
    return REG_PL_RD(LNA_MX_ADDR);
}

static inline void lna_mx_set(uint32_t x)
{
    REG_PL_WR(LNA_MX_ADDR,x);
}

static inline uint32_t rf_lna_bm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline uint32_t rf_lna_bm_hg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_lna_bm_hg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(LNA_MX_ADDR,localVal);
}

static inline uint32_t rf_lna_bm_lg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_lna_bm_lg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(LNA_MX_ADDR,localVal);
}

static inline uint32_t rf_lna_cap_lg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_lna_cap_lg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(LNA_MX_ADDR,localVal);
}

static inline uint32_t rf_lna_vdd13_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lna_vdd13_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(LNA_MX_ADDR,localVal);
}

static inline uint32_t rf_rmx_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_rmx_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(LNA_MX_ADDR,localVal);
}


#define LNA_MX2_ADDR (RF_BASE + 0x0000020c)

static inline uint32_t lna_mx2_get(void)
{
    return REG_PL_RD(LNA_MX2_ADDR);
}

static inline void lna_mx2_set(uint32_t x)
{
    REG_PL_WR(LNA_MX2_ADDR,x);
}

static inline uint32_t rf_lna_rfb_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_lna_rfb_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}

static inline uint32_t rf_lna_rfb_match3_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_lna_rfb_match3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}

static inline uint32_t rf_lna_rfb_match2_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_lna_rfb_match2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}

static inline uint32_t rf_lna_rfb_match1_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_lna_rfb_match1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}

static inline uint32_t rf_lna_cap2_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_lna_cap2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}

static inline uint32_t rf_lna_cap1_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_lna_cap1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_MX2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(LNA_MX2_ADDR,localVal);
}


#define RBB_ROSDAC_ADDR (RF_BASE + 0x00000210)

static inline uint32_t rbb_rosdac_get(void)
{
    return REG_PL_RD(RBB_ROSDAC_ADDR);
}

static inline void rbb_rosdac_set(uint32_t x)
{
    REG_PL_WR(RBB_ROSDAC_ADDR,x);
}

static inline uint32_t rf_rosdac_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rosdac_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(RBB_ROSDAC_ADDR,localVal);
}

static inline uint32_t rf_rosdac_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rosdac_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RBB_ROSDAC_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline uint32_t rf_rosdac_q_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ROSDAC_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}


#define RBB_CAP_1_ADDR (RF_BASE + 0x00000214)

static inline uint32_t rbb_cap_1_get(void)
{
    return REG_PL_RD(RBB_CAP_1_ADDR);
}

static inline void rbb_cap_1_set(uint32_t x)
{
    REG_PL_WR(RBB_CAP_1_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_fc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rbb_cap1_fc_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(RBB_CAP_1_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap1_fc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rbb_cap1_fc_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RBB_CAP_1_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rbb_cap2_fc_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(RBB_CAP_1_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rbb_cap2_fc_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_CAP_1_ADDR,localVal);
}


#define RBB_CAP_2_ADDR (RF_BASE + 0x00000218)

static inline uint32_t rbb_cap_2_get(void)
{
    return REG_PL_RD(RBB_CAP_2_ADDR);
}

static inline void rbb_cap_2_set(uint32_t x)
{
    REG_PL_WR(RBB_CAP_2_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_fc_i_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline uint32_t rf_rbb_cap1_fc_q_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_2_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline uint32_t rf_rbb_cap2_fc_i_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_2_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline uint32_t rf_rbb_cap2_fc_q_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}


#define RBB_CAP_3_ADDR (RF_BASE + 0x0000021c)

static inline uint32_t rbb_cap_3_get(void)
{
    return REG_PL_RD(RBB_CAP_3_ADDR);
}

static inline void rbb_cap_3_set(uint32_t x)
{
    REG_PL_WR(RBB_CAP_3_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_fc_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rbb_cap1_fc_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(RBB_CAP_3_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap1_fc_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rbb_cap1_fc_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RBB_CAP_3_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rbb_cap2_fc_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(RBB_CAP_3_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rbb_cap2_fc_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP_3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_CAP_3_ADDR,localVal);
}


#define RBB_CAP4_ADDR (RF_BASE + 0x00000220)

static inline uint32_t rbb_cap4_get(void)
{
    return REG_PL_RD(RBB_CAP4_ADDR);
}

static inline void rbb_cap4_set(uint32_t x)
{
    REG_PL_WR(RBB_CAP4_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_fc_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rbb_cap1_fc_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(RBB_CAP4_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap1_fc_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rbb_cap1_fc_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RBB_CAP4_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rbb_cap2_fc_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(RBB_CAP4_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rbb_cap2_fc_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_CAP4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_CAP4_ADDR,localVal);
}


#define RBB_RX_ADDR (RF_BASE + 0x00000224)

static inline uint32_t rbb_rx_get(void)
{
    return REG_PL_RD(RBB_RX_ADDR);
}

static inline void rbb_rx_set(uint32_t x)
{
    REG_PL_WR(RBB_RX_ADDR,x);
}

static inline uint32_t rf_rbb_rx1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_rbb_rx1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}

static inline uint32_t rf_rbb_rx2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_rbb_rx2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}

static inline uint32_t rf_rbb_rx1_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline uint32_t rf_rbb_rx2_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline uint32_t rf_rbb_rx1_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_rbb_rx1_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}

static inline uint32_t rf_rbb_rx2_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_rbb_rx2_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}

static inline uint32_t rf_rbb_rx1_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_rbb_rx1_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}

static inline uint32_t rf_rbb_rx2_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_rbb_rx2_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_RX_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RBB_RX_ADDR,localVal);
}


#define RBB_ADDR (RF_BASE + 0x00000228)

static inline uint32_t rbb_get(void)
{
    return REG_PL_RD(RBB_ADDR);
}

static inline void rbb_set(uint32_t x)
{
    REG_PL_WR(RBB_ADDR,x);
}

static inline uint32_t rf_rbb_cgm_r_tune_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_rbb_cgm_r_tune_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_bm_op_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_rbb_bm_op_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_vcm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_rbb_vcm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_bias_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_rbb_bias_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_vstartup_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_rbb_vstartup_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_deq_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_rbb_deq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_lpf_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_rbb_lpf_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rosdac_range_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_rosdac_range_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_vth_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_rbb_pkdet_vth_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_rbb_pkdet_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_rbb_pkdet_en_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rbb_pkdet_en_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_rbb_pkdet_out_rstn_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_rbb_pkdet_out_rstn_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RBB_ADDR,localVal);
}

static inline uint32_t rf_pkdet_out_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t rf_pkdet_out_latch_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RXADC_ADDR (RF_BASE + 0x0000022c)

static inline uint32_t rxadc_get(void)
{
    return REG_PL_RD(RXADC_ADDR);
}

static inline void rxadc_set(uint32_t x)
{
    REG_PL_WR(RXADC_ADDR,x);
}

static inline uint32_t rf_rxadc_dly_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_rxadc_dly_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_glitch_remove_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_rxadc_glitch_remove_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_div_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_rxadc_clk_div_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_rxadc_clk_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_clk_sync_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rxadc_clk_sync_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_rxadc_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(RXADC_ADDR,localVal);
}

static inline uint32_t rf_rxadc_oscal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_rxadc_oscal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXADC_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RXADC_ADDR,localVal);
}


#define RXADC_READBACK_ADDR (RF_BASE + 0x00000230)

static inline uint32_t rxadc_readback_get(void)
{
    return REG_PL_RD(RXADC_READBACK_ADDR);
}

static inline void rxadc_readback_set(uint32_t x)
{
    REG_PL_WR(RXADC_READBACK_ADDR,x);
}

static inline uint32_t rf_rxadc_dout_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline uint32_t rf_rxadc_dout_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXADC_READBACK_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}


#define RF_ADC_OSDATA_ADDR (RF_BASE + 0x00000234)

static inline uint32_t rf_adc_osdata_get(void)
{
    return REG_PL_RD(RF_ADC_OSDATA_ADDR);
}

static inline void rf_adc_osdata_set(uint32_t x)
{
    REG_PL_WR(RF_ADC_OSDATA_ADDR,x);
}

static inline uint32_t rf_rxadc_os_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ADC_OSDATA_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rf_rxadc_os_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ADC_OSDATA_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16)&((uint32_t)0x01ff0000);
    REG_PL_WR(RF_ADC_OSDATA_ADDR,localVal);
}

static inline uint32_t rf_rxadc_os_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ADC_OSDATA_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_rxadc_os_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ADC_OSDATA_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0)&((uint32_t)0x000001ff);
    REG_PL_WR(RF_ADC_OSDATA_ADDR,localVal);
}


#define TESTBUF_ADDR (RF_BASE + 0x00000238)

static inline uint32_t testbuf_get(void)
{
    return REG_PL_RD(TESTBUF_ADDR);
}

static inline void testbuf_set(uint32_t x)
{
    REG_PL_WR(TESTBUF_ADDR,x);
}

static inline uint32_t rf_pu_testbuf_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pu_testbuf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_vcm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_testbuf_vcm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_testbuf_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_testbuf_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_op_cc_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_testbuf_op_cc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_rfb_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_testbuf_rfb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}

static inline uint32_t rf_testbuf_rin_getf(void)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_testbuf_rin_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TESTBUF_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(TESTBUF_ADDR,localVal);
}


#define VCO_ADDR (RF_BASE + 0x0000023c)

static inline uint32_t vco_get(void)
{
    return REG_PL_RD(VCO_ADDR);
}

static inline void vco_set(uint32_t x)
{
    REG_PL_WR(VCO_ADDR,x);
}

static inline uint32_t rf_vco_acal_ud_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_vco_idac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline uint32_t rf_vco_idac_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_vco_idac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_ldo_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_vco_ldo_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_ldo_vsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00006000)) >> 13);
}

static inline void rf_vco_ldo_vsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00006000);
    localVal |= (x << 13)&((uint32_t)0x00006000);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_idac_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_vco_idac_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_vbias_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_vco_vbias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_acal_vref_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_vco_acal_vref_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_modcap_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_vco_modcap_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_short_idac_filter_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_vco_short_idac_filter_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(VCO_ADDR,localVal);
}

static inline uint32_t rf_vco_short_vbias_filter_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_vco_short_vbias_filter_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(VCO_ADDR,localVal);
}


#define LODIST_ADDR (RF_BASE + 0x00000240)

static inline uint32_t lodist_get(void)
{
    return REG_PL_RD(LODIST_ADDR);
}

static inline void lodist_set(uint32_t x)
{
    REG_PL_WR(LODIST_ADDR,x);
}

static inline uint32_t rf_lodist_75dc_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_lodist_75dc_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lodist_nwell_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lodist_nwell_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lodist_rwell_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lodist_rwell_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lodist_rxbuf_supply_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_lodist_rxbuf_supply_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lodist_rxbuf_supply_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lodist_rxbuf_supply_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lodist_txbuf_supply_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lodist_txbuf_supply_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LODIST_ADDR,localVal);
}


#define FBDV_ADDR (RF_BASE + 0x00000244)

static inline uint32_t fbdv_get(void)
{
    return REG_PL_RD(FBDV_ADDR);
}

static inline void fbdv_set(uint32_t x)
{
    REG_PL_WR(FBDV_ADDR,x);
}

static inline uint32_t rf_dco_dither_clk_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_dco_dither_clk_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lotpm_fmash_clk_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_lotpm_fmash_clk_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_rst_mmdiv_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_rst_mmdiv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_resample_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_fbdv_resample_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_stg_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_fbdv_stg_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_sample_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_fbdv_sample_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_fb_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_fbdv_fb_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_dco_dither_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_fbdv_dco_dither_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_adpll_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_fbdv_adpll_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_fbdv_tpm_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_fbdv_tpm_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(FBDV_ADDR,localVal);
}


#define KCAL1_ADDR (RF_BASE + 0x00000248)

static inline uint32_t kcal1_get(void)
{
    return REG_PL_RD(KCAL1_ADDR);
}

static inline void kcal1_set(uint32_t x)
{
    REG_PL_WR(KCAL1_ADDR,x);
}

static inline uint32_t rf_kcal_ratio_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_kcal_ratio_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20)&((uint32_t)0x3ff00000);
    REG_PL_WR(KCAL1_ADDR,localVal);
}

static inline uint32_t rf_kcal_cnt_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_kcal_cnt_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(KCAL1_ADDR,localVal);
}

static inline uint32_t rf_kcal_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_kcal_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(KCAL1_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(KCAL1_ADDR,localVal);
}


#define KCAL2_ADDR (RF_BASE + 0x0000024c)

static inline uint32_t kcal2_get(void)
{
    return REG_PL_RD(KCAL2_ADDR);
}

static inline void kcal2_set(uint32_t x)
{
    REG_PL_WR(KCAL2_ADDR,x);
}

static inline uint32_t rf_kcal_ratio_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL2_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline uint32_t rf_kcal_cnt_rdy_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL2_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_kcal_cnt_op_getf(void)
{
    uint32_t localVal = REG_PL_RD(KCAL2_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}


#define ADPLL_SLOPE_GEN_ADDR (RF_BASE + 0x00000250)

static inline uint32_t adpll_slope_gen_get(void)
{
    return REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
}

static inline void adpll_slope_gen_set(uint32_t x)
{
    REG_PL_WR(ADPLL_SLOPE_GEN_ADDR,x);
}

static inline uint32_t rf_adpll_slope_gen_pulse_width_enhance_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_slope_gen_pulse_width_enhance_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(ADPLL_SLOPE_GEN_ADDR,localVal);
}

static inline uint32_t rf_adpll_slope_gen_dc_corr_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_adpll_slope_gen_dc_corr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(ADPLL_SLOPE_GEN_ADDR,localVal);
}

static inline uint32_t rf_adpll_slope_gen_r_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_adpll_slope_gen_r_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SLOPE_GEN_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(ADPLL_SLOPE_GEN_ADDR,localVal);
}


#define ADPLL_ADC_ADDR (RF_BASE + 0x00000254)

static inline uint32_t adpll_adc_get(void)
{
    return REG_PL_RD(ADPLL_ADC_ADDR);
}

static inline void adpll_adc_set(uint32_t x)
{
    REG_PL_WR(ADPLL_ADC_ADDR,x);
}

static inline uint32_t rf_adpll_adc_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_adpll_adc_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_clk_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_adpll_adc_clk_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_clk_div_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adpll_adc_clk_div_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_clk_sync_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_adpll_adc_clk_sync_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_oscal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_adc_oscal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_vref_coarse_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_adpll_adc_vref_coarse_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_vref_fine_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_adpll_adc_vref_fine_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_data_sign_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_adpll_adc_data_sign_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_vth_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_adpll_adc_vth_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}

static inline uint32_t rf_adpll_adc_vth_bias_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_adpll_adc_vth_bias_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_ADC_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(ADPLL_ADC_ADDR,localVal);
}


#define ADPLL_DTC_ADDR (RF_BASE + 0x00000258)

static inline uint32_t adpll_dtc_get(void)
{
    return REG_PL_RD(ADPLL_DTC_ADDR);
}

static inline void adpll_dtc_set(uint32_t x)
{
    REG_PL_WR(ADPLL_DTC_ADDR,x);
}

static inline uint32_t rf_adpll_dtc_clk_inv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_dtc_clk_inv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL_DTC_ADDR,localVal);
}

static inline uint32_t rf_adpll_dtc_inv_vth_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_adpll_dtc_inv_vth_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADPLL_DTC_ADDR,localVal);
}

static inline uint32_t rf_adpll_dtc_r_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_adpll_dtc_r_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(ADPLL_DTC_ADDR,localVal);
}

static inline uint32_t rf_adpll_dtc_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_adpll_dtc_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_DTC_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(ADPLL_DTC_ADDR,localVal);
}


#define LO_FC_CONFIG1_ADDR (RF_BASE + 0x0000025c)

static inline uint32_t lo_fc_config1_get(void)
{
    return REG_PL_RD(LO_FC_CONFIG1_ADDR);
}

static inline void lo_fc_config1_set(uint32_t x)
{
    REG_PL_WR(LO_FC_CONFIG1_ADDR,x);
}

static inline uint32_t rf_lo_fcw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_FC_CONFIG1_ADDR);
    return ((localVal & ((uint32_t)0x01ffffff)) >> 0);
}

static inline void rf_lo_fcw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_FC_CONFIG1_ADDR);
    localVal &= ~((uint32_t)0x01ffffff);
    localVal |= (x << 0)&((uint32_t)0x01ffffff);
    REG_PL_WR(LO_FC_CONFIG1_ADDR,localVal);
}


#define LO_FCW_CONFIG2_ADDR (RF_BASE + 0x00000260)

static inline uint32_t lo_fcw_config2_get(void)
{
    return REG_PL_RD(LO_FCW_CONFIG2_ADDR);
}

static inline void lo_fcw_config2_set(uint32_t x)
{
    REG_PL_WR(LO_FCW_CONFIG2_ADDR,x);
}

static inline uint32_t rf_lo_fcw_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_FCW_CONFIG2_ADDR);
    return ((localVal & ((uint32_t)0x01ffffff)) >> 0);
}


#define LO_FCW3_ADDR (RF_BASE + 0x00000264)

static inline uint32_t lo_fcw3_get(void)
{
    return REG_PL_RD(LO_FCW3_ADDR);
}

static inline void lo_fcw3_set(uint32_t x)
{
    REG_PL_WR(LO_FCW3_ADDR,x);
}

static inline uint32_t rf_tx_freq_mod_hp_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_FCW3_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline uint32_t rf_tx_freq_mod_lp_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_FCW3_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}


#define LO_FCW4_ADDR (RF_BASE + 0x00000268)

static inline uint32_t lo_fcw4_get(void)
{
    return REG_PL_RD(LO_FCW4_ADDR);
}

static inline void lo_fcw4_set(uint32_t x)
{
    REG_PL_WR(LO_FCW4_ADDR,x);
}

static inline uint32_t rf_lo_fcw_frac_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_FCW4_ADDR);
    return ((localVal & ((uint32_t)0x0001ffff)) >> 0);
}

static inline void rf_lo_fcw_frac_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_FCW4_ADDR);
    localVal &= ~((uint32_t)0x0001ffff);
    localVal |= (x << 0)&((uint32_t)0x0001ffff);
    REG_PL_WR(LO_FCW4_ADDR,localVal);
}


#define LOTPM_ADDR (RF_BASE + 0x0000026c)

static inline uint32_t lotpm_get(void)
{
    return REG_PL_RD(LOTPM_ADDR);
}

static inline void lotpm_set(uint32_t x)
{
    REG_PL_WR(LOTPM_ADDR,x);
}

static inline uint32_t rf_lotpm_hfp_mash1_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_lotpm_hfp_mash1_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(LOTPM_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_lotpm_hfp_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LOTPM_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_delay_fref_getf(void)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lotpm_hfp_delay_fref_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LOTPM_ADDR,localVal);
}

static inline uint32_t rf_lotpm_hfp_delay_fmash_getf(void)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_lotpm_hfp_delay_fmash_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(LOTPM_ADDR,localVal);
}

static inline uint32_t rf_lotpm_lfp_delay_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lotpm_lfp_delay_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LOTPM_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LOTPM_ADDR,localVal);
}


#define ADPLL1_ADDR (RF_BASE + 0x00000270)

static inline uint32_t adpll1_get(void)
{
    return REG_PL_RD(ADPLL1_ADDR);
}

static inline void adpll1_set(uint32_t x)
{
    REG_PL_WR(ADPLL1_ADDR,x);
}

static inline uint32_t rf_adpll_force_inc_fcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_adpll_force_inc_fcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_unlock_intrpt_clear_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_adpll_lo_unlock_intrpt_clear_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_sfreg_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adpll_sfreg_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_open_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_adpll_lo_open_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_mom_search_en_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_adpll_mom_search_en_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_freqerr_det_start_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_adpll_freqerr_det_start_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_mom_update_en_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_adpll_mom_update_en_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_vctrl_det_en_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_adpll_vctrl_det_en_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_vctrl_det_start_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_adpll_vctrl_det_start_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_abnormal_dealed_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_adpll_abnormal_dealed_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lock_fail_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_adpll_lock_fail_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_fsm_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adpll_fsm_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_fsm_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_adpll_lo_fsm_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_lock_directly_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_adpll_lo_lock_directly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_fcal_start_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_adpll_fcal_start_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_fcal_done_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_adpll_fcal_done_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_loop_lock_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_adpll_loop_lock_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_rst_spd_det_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_rst_spd_det_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_rst_coarse_det_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_adpll_rst_coarse_det_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_momhold_lmsenb_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_adpll_momhold_lmsenb_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_timeout_cnt_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_adpll_timeout_cnt_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_timeout_cnt1_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_adpll_timeout_cnt1_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_lock_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_adpll_lo_lock_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}

static inline uint32_t rf_adpll_lo_unlock_intrpt_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_adpll_lo_unlock_intrpt_clear_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(ADPLL1_ADDR,localVal);
}


#define ADPLL_LF_REG_ADDR (RF_BASE + 0x00000274)

static inline uint32_t adpll_lf_reg_get(void)
{
    return REG_PL_RD(ADPLL_LF_REG_ADDR);
}

static inline void adpll_lf_reg_set(uint32_t x)
{
    REG_PL_WR(ADPLL_LF_REG_ADDR,x);
}

static inline uint32_t rf_adpll_lf_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_adpll_lf_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_base_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_adpll_lf_alpha_base_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_exp_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_adpll_lf_alpha_exp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_fast_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_lf_alpha_fast_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_base_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00060000)) >> 17);
}

static inline void rf_adpll_lf_beta_base_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00060000);
    localVal |= (x << 17)&((uint32_t)0x00060000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_exp_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x0001c000)) >> 14);
}

static inline void rf_adpll_lf_beta_exp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x0001c000);
    localVal |= (x << 14)&((uint32_t)0x0001c000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_fast_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adpll_lf_beta_fast_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_f_p3_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_adpll_lf_f_p3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_avg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_adpll_lf_avg_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_lsb_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x000001fc)) >> 2);
}

static inline void rf_adpll_lf_lsb_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x000001fc);
    localVal |= (x << 2)&((uint32_t)0x000001fc);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_vctrl_range_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_adpll_lf_vctrl_range_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_REG_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(ADPLL_LF_REG_ADDR,localVal);
}


#define ADPLL_LF_TX_ADDR (RF_BASE + 0x00000278)

static inline uint32_t adpll_lf_tx_get(void)
{
    return REG_PL_RD(ADPLL_LF_TX_ADDR);
}

static inline void adpll_lf_tx_set(uint32_t x)
{
    REG_PL_WR(ADPLL_LF_TX_ADDR,x);
}

static inline uint32_t rf_adpll_lf_alpha_base_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_adpll_lf_alpha_base_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_exp_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_adpll_lf_alpha_exp_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_fast_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_lf_alpha_fast_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_base_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x00060000)) >> 17);
}

static inline void rf_adpll_lf_beta_base_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x00060000);
    localVal |= (x << 17)&((uint32_t)0x00060000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_exp_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x0001c000)) >> 14);
}

static inline void rf_adpll_lf_beta_exp_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x0001c000);
    localVal |= (x << 14)&((uint32_t)0x0001c000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_fast_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adpll_lf_beta_fast_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_f_p3_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_adpll_lf_f_p3_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_TX_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(ADPLL_LF_TX_ADDR,localVal);
}


#define ADPLL_LF_RX_ADDR (RF_BASE + 0x0000027c)

static inline uint32_t adpll_lf_rx_get(void)
{
    return REG_PL_RD(ADPLL_LF_RX_ADDR);
}

static inline void adpll_lf_rx_set(uint32_t x)
{
    REG_PL_WR(ADPLL_LF_RX_ADDR,x);
}

static inline uint32_t rf_adpll_lf_alpha_base_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_adpll_lf_alpha_base_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_exp_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_adpll_lf_alpha_exp_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_alpha_fast_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_lf_alpha_fast_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_base_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x00060000)) >> 17);
}

static inline void rf_adpll_lf_beta_base_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x00060000);
    localVal |= (x << 17)&((uint32_t)0x00060000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_exp_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x0001c000)) >> 14);
}

static inline void rf_adpll_lf_beta_exp_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x0001c000);
    localVal |= (x << 14)&((uint32_t)0x0001c000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_beta_fast_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adpll_lf_beta_fast_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}

static inline uint32_t rf_adpll_lf_f_p3_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_adpll_lf_f_p3_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_RX_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(ADPLL_LF_RX_ADDR,localVal);
}


#define ADPLL_LF_HW_ADDR (RF_BASE + 0x00000280)

static inline uint32_t adpll_lf_hw_get(void)
{
    return REG_PL_RD(ADPLL_LF_HW_ADDR);
}

static inline void adpll_lf_hw_set(uint32_t x)
{
    REG_PL_WR(ADPLL_LF_HW_ADDR,x);
}

static inline uint32_t rf_adpll_lf_alpha_base_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t rf_adpll_lf_alpha_exp_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline uint32_t rf_adpll_lf_alpha_fast_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline uint32_t rf_adpll_lf_beta_base_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x00060000)) >> 17);
}

static inline uint32_t rf_adpll_lf_beta_exp_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x0001c000)) >> 14);
}

static inline uint32_t rf_adpll_lf_beta_fast_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t rf_adpll_lf_f_p3_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LF_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}


#define ADPLL_VCTRL_ADDR (RF_BASE + 0x00000284)

static inline uint32_t adpll_vctrl_get(void)
{
    return REG_PL_RD(ADPLL_VCTRL_ADDR);
}

static inline void adpll_vctrl_set(uint32_t x)
{
    REG_PL_WR(ADPLL_VCTRL_ADDR,x);
}

static inline uint32_t rf_adpll_vctrl_range_sel_ext_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_adpll_vctrl_range_sel_ext_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_vctrl_lock_win_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_adpll_vctrl_lock_win_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_vctrl_moni_win_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_adpll_vctrl_moni_win_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_vctrl_det_cons_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adpll_vctrl_det_cons_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_mom_update_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_adpll_mom_update_period_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_force_mom_hold_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_adpll_force_mom_hold_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_dco_mash_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_adpll_dco_mash_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_capcode_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_adpll_capcode_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_sdm_order_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_sdm_order_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_sdm_dither_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_sdm_dither_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_sdm_noi_dith_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_adpll_sdm_noi_dith_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_sdm_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_sdm_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_adpll_sdm_dith_force_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t rf_adpll_sdm_dith_force_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_adpll_sdm_dith_force_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}

static inline uint32_t rf_sdmout_dly_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_sdmout_dly_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_VCTRL_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(ADPLL_VCTRL_ADDR,localVal);
}


#define ADPLL_LMS_ADDR (RF_BASE + 0x00000288)

static inline uint32_t adpll_lms_get(void)
{
    return REG_PL_RD(ADPLL_LMS_ADDR);
}

static inline void adpll_lms_set(uint32_t x)
{
    REG_PL_WR(ADPLL_LMS_ADDR,x);
}

static inline uint32_t rf_adpll_fref_div2_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_adpll_fref_div2_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_ext_value_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_adpll_lms_ext_value_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_ext_value_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x1ff00000)) >> 20);
}

static inline void rf_adpll_lms_ext_value_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x1ff00000);
    localVal |= (x << 20)&((uint32_t)0x1ff00000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_sdm_dither_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_adpll_sdm_dither_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_sdm_dither_en_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_adpll_sdm_dither_en_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_adpll_lms_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_sdm_dither_prbs_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_adpll_sdm_dither_prbs_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_dem_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_adpll_pha_dem_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_dither_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adpll_pha_dither_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_step_enlarge_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_lms_step_enlarge_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_prbs_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_adpll_pha_prbs_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_q_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_adpll_lms_q_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_msb_err_prbs_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_adpll_pha_msb_err_prbs_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_cancel_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_adpll_pha_cancel_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_msb_err_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_adpll_pha_msb_err_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}

static inline uint32_t rf_adpll_pha_cancel_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_adpll_pha_cancel_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_LMS_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(ADPLL_LMS_ADDR,localVal);
}


#define ADPLL_SPD_ADDR (RF_BASE + 0x0000028c)

static inline uint32_t adpll_spd_get(void)
{
    return REG_PL_RD(ADPLL_SPD_ADDR);
}

static inline void adpll_spd_set(uint32_t x)
{
    REG_PL_WR(ADPLL_SPD_ADDR,x);
}

static inline uint32_t rf_adpll_spd_in_range_delay_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_adpll_spd_in_range_delay_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarsepha_dly_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_adpll_coarsepha_dly_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_force_coarse_path_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_adpll_force_coarse_path_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_lms_sstp_win_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_adpll_spd_lms_sstp_win_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_outrange_dly_sel_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x06000000)) >> 25);
}

static inline void rf_adpll_spd_outrange_dly_sel_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x06000000);
    localVal |= (x << 25)&((uint32_t)0x06000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarse_path_offtime_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adpll_coarse_path_offtime_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarse_phaerr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_adpll_coarse_phaerr_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_force_lf_fast_mode_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t rf_adpll_force_lf_fast_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_adpll_force_lf_fast_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_force_lf_fast_mode_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_adpll_force_lf_fast_mode_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarse_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_adpll_coarse_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_adpll_spd_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarse_in_range_cons_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_adpll_coarse_in_range_cons_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_adpll_spd_threshold_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_coarse_path_turnoff_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_adpll_coarse_path_turnoff_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_in_range_cons_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_adpll_spd_in_range_cons_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_sign_det_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_adpll_spd_sign_det_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_outrange_gate_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_adpll_spd_outrange_gate_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_out_range_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_adpll_spd_out_range_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_sign_det_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_adpll_spd_sign_det_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}

static inline uint32_t rf_adpll_spd_in_range_delay_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_adpll_spd_in_range_delay_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_SPD_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(ADPLL_SPD_ADDR,localVal);
}


#define FCAL_ADDR (RF_BASE + 0x00000290)

static inline uint32_t fcal_get(void)
{
    return REG_PL_RD(FCAL_ADDR);
}

static inline void fcal_set(uint32_t x)
{
    REG_PL_WR(FCAL_ADDR,x);
}

static inline uint32_t rf_fcal_mom_ini_ext_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rf_fcal_mom_ini_ext_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_phaserr_limit_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_fcal_phaserr_limit_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_fcal_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_clk_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_fcal_clk_period_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_mom_toggle_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_fcal_mom_toggle_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_coarse_pha_threshold_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00000006)) >> 1);
}

static inline void rf_fcal_coarse_pha_threshold_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00000006);
    localVal |= (x << 1)&((uint32_t)0x00000006);
    REG_PL_WR(FCAL_ADDR,localVal);
}

static inline uint32_t rf_fcal_div_ratio_adj_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_fcal_div_ratio_adj_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FCAL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(FCAL_ADDR,localVal);
}


#define ADPLL_POLARITY_ADDR (RF_BASE + 0x00000294)

static inline uint32_t adpll_polarity_get(void)
{
    return REG_PL_RD(ADPLL_POLARITY_ADDR);
}

static inline void adpll_polarity_set(uint32_t x)
{
    REG_PL_WR(ADPLL_POLARITY_ADDR,x);
}

static inline uint32_t rf_adpll_lp_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_adpll_lp_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(ADPLL_POLARITY_ADDR,localVal);
}

static inline uint32_t rf_adpll_fcal_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_adpll_fcal_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(ADPLL_POLARITY_ADDR,localVal);
}

static inline uint32_t rf_adpll_lms_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_lms_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADPLL_POLARITY_ADDR,localVal);
}

static inline uint32_t rf_adpll_lp_mom_polarity_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_adpll_lp_mom_polarity_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_POLARITY_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(ADPLL_POLARITY_ADDR,localVal);
}


#define ADPLL_OUTPUT_ADDR (RF_BASE + 0x00000298)

static inline uint32_t adpll_output_get(void)
{
    return REG_PL_RD(ADPLL_OUTPUT_ADDR);
}

static inline void adpll_output_set(uint32_t x)
{
    REG_PL_WR(ADPLL_OUTPUT_ADDR,x);
}

static inline uint32_t rf_adpll_freqerr_det_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t rf_adpll_freqerr_ou_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_adpll_freqerr_sign_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t rf_adpll_vctrl_det_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t rf_adpll_capcode_ud_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_adpll_mom_update_total_ou_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00018000)) >> 15);
}

static inline uint32_t rf_adpll_capcode_out_range_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t rf_adpll_fcal_done_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t rf_adpll_spd_lock_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_adpll_spd_unlock_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t rf_adpll_mom_update_ou_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t rf_adpll_mom_update_fail_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_adpll_vctrl_out_range_fsm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_adpll_spd_unlock_sign_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t rf_adpll_fsm_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000078)) >> 3);
}

static inline uint32_t rf_adpll_lo_lock_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t rf_adpll_unlock_intrpt_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_OUTPUT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define ADPLL_RESERVED_ADDR (RF_BASE + 0x0000029c)

static inline uint32_t adpll_reserved_get(void)
{
    return REG_PL_RD(ADPLL_RESERVED_ADDR);
}

static inline void adpll_reserved_set(uint32_t x)
{
    REG_PL_WR(ADPLL_RESERVED_ADDR,x);
}

static inline uint32_t rf_adpll_resv0_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_RESERVED_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void rf_adpll_resv0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_RESERVED_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(ADPLL_RESERVED_ADDR,localVal);
}

static inline uint32_t rf_adpll_resv1_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADPLL_RESERVED_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_adpll_resv1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADPLL_RESERVED_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(ADPLL_RESERVED_ADDR,localVal);
}


#define RF_RESERVED_ADDR (RF_BASE + 0x000002a0)

static inline uint32_t rf_reserved_get(void)
{
    return REG_PL_RD(RF_RESERVED_ADDR);
}

static inline void rf_reserved_set(uint32_t x)
{
    REG_PL_WR(RF_RESERVED_ADDR,x);
}

static inline uint32_t rf_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RESERVED_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RESERVED_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_RESERVED_ADDR,localVal);
}


#define RBB_GAIN_CTRL0_ADDR (RF_BASE + 0x00000300)

static inline uint32_t rbb_gain_ctrl0_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
}

static inline void rbb_gain_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,x);
}

static inline uint32_t rf_gain_ctrl0_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl0_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl0_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl0_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl0_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl0_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl0_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL0_ADDR,localVal);
}


#define RBB_GAIN_CTRL1_ADDR (RF_BASE + 0x00000304)

static inline uint32_t rbb_gain_ctrl1_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
}

static inline void rbb_gain_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,x);
}

static inline uint32_t rf_gain_ctrl1_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl1_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl1_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl1_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl1_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl1_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl1_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL1_ADDR,localVal);
}


#define RBB_GAIN_CTRL2_ADDR (RF_BASE + 0x00000308)

static inline uint32_t rbb_gain_ctrl2_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
}

static inline void rbb_gain_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,x);
}

static inline uint32_t rf_gain_ctrl2_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl2_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl2_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl2_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl2_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl2_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl2_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL2_ADDR,localVal);
}


#define RBB_GAIN_CTRL3_ADDR (RF_BASE + 0x0000030c)

static inline uint32_t rbb_gain_ctrl3_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
}

static inline void rbb_gain_ctrl3_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,x);
}

static inline uint32_t rf_gain_ctrl3_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl3_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl3_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl3_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl3_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl3_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl3_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL3_ADDR,localVal);
}


#define RBB_GAIN_CTRL4_ADDR (RF_BASE + 0x00000310)

static inline uint32_t rbb_gain_ctrl4_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
}

static inline void rbb_gain_ctrl4_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,x);
}

static inline uint32_t rf_gain_ctrl4_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl4_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl4_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl4_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl4_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl4_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl4_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL4_ADDR,localVal);
}


#define RBB_GAIN_CTRL5_ADDR (RF_BASE + 0x00000314)

static inline uint32_t rbb_gain_ctrl5_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
}

static inline void rbb_gain_ctrl5_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,x);
}

static inline uint32_t rf_gain_ctrl5_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl5_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl5_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl5_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl5_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl5_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl5_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL5_ADDR,localVal);
}


#define RBB_GAIN_CTRL6_ADDR (RF_BASE + 0x00000318)

static inline uint32_t rbb_gain_ctrl6_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
}

static inline void rbb_gain_ctrl6_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,x);
}

static inline uint32_t rf_gain_ctrl6_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl6_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl6_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl6_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl6_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl6_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl6_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL6_ADDR,localVal);
}


#define RBB_GAIN_CTRL7_ADDR (RF_BASE + 0x0000031c)

static inline uint32_t rbb_gain_ctrl7_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
}

static inline void rbb_gain_ctrl7_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,x);
}

static inline uint32_t rf_gain_ctrl7_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl7_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl7_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl7_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl7_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl7_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl7_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL7_ADDR,localVal);
}


#define RBB_GAIN_CTRL8_ADDR (RF_BASE + 0x00000320)

static inline uint32_t rbb_gain_ctrl8_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
}

static inline void rbb_gain_ctrl8_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,x);
}

static inline uint32_t rf_gain_ctrl8_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl8_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl8_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl8_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl8_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl8_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl8_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL8_ADDR,localVal);
}


#define RBB_GAIN_CTRL9_ADDR (RF_BASE + 0x00000324)

static inline uint32_t rbb_gain_ctrl9_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
}

static inline void rbb_gain_ctrl9_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,x);
}

static inline uint32_t rf_gain_ctrl9_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl9_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl9_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl9_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl9_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl9_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl9_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL9_ADDR,localVal);
}


#define RBB_GAIN_CTRL10_ADDR (RF_BASE + 0x00000328)

static inline uint32_t rbb_gain_ctrl10_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
}

static inline void rbb_gain_ctrl10_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,x);
}

static inline uint32_t rf_gain_ctrl10_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl10_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl10_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl10_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl10_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl10_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl10_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL10_ADDR,localVal);
}


#define RBB_GAIN_CTRL11_ADDR (RF_BASE + 0x0000032c)

static inline uint32_t rbb_gain_ctrl11_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
}

static inline void rbb_gain_ctrl11_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,x);
}

static inline uint32_t rf_gain_ctrl11_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl11_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl11_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl11_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl11_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl11_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl11_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL11_ADDR,localVal);
}


#define RBB_GAIN_CTRL12_ADDR (RF_BASE + 0x00000330)

static inline uint32_t rbb_gain_ctrl12_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
}

static inline void rbb_gain_ctrl12_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,x);
}

static inline uint32_t rf_gain_ctrl12_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl12_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl12_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl12_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl12_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl12_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl12_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL12_ADDR,localVal);
}


#define RBB_GAIN_CTRL13_ADDR (RF_BASE + 0x00000334)

static inline uint32_t rbb_gain_ctrl13_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
}

static inline void rbb_gain_ctrl13_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,x);
}

static inline uint32_t rf_gain_ctrl13_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl13_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl13_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl13_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl13_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl13_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl13_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL13_ADDR,localVal);
}


#define RBB_GAIN_CTRL14_ADDR (RF_BASE + 0x00000338)

static inline uint32_t rbb_gain_ctrl14_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
}

static inline void rbb_gain_ctrl14_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,x);
}

static inline uint32_t rf_gain_ctrl14_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl14_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl14_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl14_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl14_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl14_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl14_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL14_ADDR,localVal);
}


#define RBB_GAIN_CTRL15_ADDR (RF_BASE + 0x0000033c)

static inline uint32_t rbb_gain_ctrl15_get(void)
{
    return REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
}

static inline void rbb_gain_ctrl15_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,x);
}

static inline uint32_t rf_gain_ctrl15_g_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gain_ctrl15_g_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_g_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl15_g_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_rosdac_i_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_gain_ctrl15_rosdac_i_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_rosdac_q_bw1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_gain_ctrl15_rosdac_q_bw1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_rosdac_i_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_gain_ctrl15_rosdac_i_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_rosdac_q_bw0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_gain_ctrl15_rosdac_q_bw0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB_GAIN_CTRL15_ADDR,localVal);
}


#define PA_GAIN_CTRL0_ADDR (RF_BASE + 0x00000340)

static inline uint32_t pa_gain_ctrl0_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL0_ADDR);
}

static inline void pa_gain_ctrl0_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,x);
}

static inline uint32_t rf_gain_ctrl7_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_gain_ctrl7_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_gain_ctrl6_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_gain_ctrl5_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_gain_ctrl4_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_gain_ctrl3_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_gain_ctrl2_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_gain_ctrl1_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_pa_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_gain_ctrl0_pa_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PA_GAIN_CTRL0_ADDR,localVal);
}


#define PA_GAIN_CTRL1_ADDR (RF_BASE + 0x00000344)

static inline uint32_t pa_gain_ctrl1_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL1_ADDR);
}

static inline void pa_gain_ctrl1_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dith_force_en_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_adpll_sdm_dith_force_en_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl_pa_lp_en_zb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_gain_ctrl_pa_lp_en_zb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl_pa_gc_zb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_gain_ctrl_pa_gc_zb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl_pa_ldo1_vref_sel_zb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_gain_ctrl_pa_ldo1_vref_sel_zb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pa_cap1_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_pa_cap1_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pa_cap1_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_pa_cap1_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_gain_ctrl7_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_gain_ctrl6_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_gain_ctrl5_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_gain_ctrl4_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_gain_ctrl3_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_gain_ctrl2_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_gain_ctrl1_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_gain_ctrl0_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PA_GAIN_CTRL1_ADDR,localVal);
}


#define PA_GAIN_CTRL2_ADDR (RF_BASE + 0x00000348)

static inline uint32_t pa_gain_ctrl2_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL2_ADDR);
}

static inline void pa_gain_ctrl2_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,x);
}

static inline uint32_t rf_gain_ctrl7_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_gain_ctrl7_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl6_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_gain_ctrl5_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_gain_ctrl4_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gain_ctrl3_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_gain_ctrl2_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl1_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_pa_ldo1_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gain_ctrl0_pa_ldo1_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PA_GAIN_CTRL2_ADDR,localVal);
}


#define PA_GAIN_CTRL3_ADDR (RF_BASE + 0x0000034c)

static inline uint32_t pa_gain_ctrl3_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL3_ADDR);
}

static inline void pa_gain_ctrl3_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,x);
}

static inline uint32_t rf_pa_lp_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_pa_ldo1_vref_sel_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline uint32_t rf_ref_dac_adj_rpt_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline uint32_t rf_tx_gain_index_rpt_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline uint32_t rf_bz_backoff_db_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_bz_backoff_db_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_bz_backoff_db_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_bz_backoff_db_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_bz_backoff_db_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_bz_backoff_db_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_bz_backoff_db_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_bz_backoff_db_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_bz_backoff_db_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_bz_backoff_db_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PA_GAIN_CTRL3_ADDR,localVal);
}


#define PA_GAIN_CTRL4_ADDR (RF_BASE + 0x00000350)

static inline uint32_t pa_gain_ctrl4_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL4_ADDR);
}

static inline void pa_gain_ctrl4_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,x);
}

static inline uint32_t rf_bz_tempcorr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_bz_tempcorr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_tempcorr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_bz_tempcorr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_tempcorr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_bz_tempcorr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_tempcorr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_bz_tempcorr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_chancorr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_bz_chancorr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_chancorr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_bz_chancorr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_chancorr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_bz_chancorr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_bz_chancorr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_bz_chancorr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PA_GAIN_CTRL4_ADDR,localVal);
}


#define PA_GAIN_CTRL5_ADDR (RF_BASE + 0x00000354)

static inline uint32_t pa_gain_ctrl5_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL5_ADDR);
}

static inline void pa_gain_ctrl5_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL5_ADDR,x);
}

static inline uint32_t rf_bz_nominal_pwr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bz_nominal_pwr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_bz_nominal_pwr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_nominal_pwr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_GAIN_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bz_nominal_pwr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_GAIN_CTRL5_ADDR,localVal);
}


#define PA_GAIN_CTRL6_ADDR (RF_BASE + 0x00000358)

static inline uint32_t pa_gain_ctrl6_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL6_ADDR);
}

static inline void pa_gain_ctrl6_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL6_ADDR,x);
}

static inline uint32_t rf_bz_nominal_pwr_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bz_nominal_pwr_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_bz_nominal_pwr_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_nominal_pwr_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_GAIN_CTRL6_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bz_nominal_pwr_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_GAIN_CTRL6_ADDR,localVal);
}


#define PA_GAIN_CTRL7_ADDR (RF_BASE + 0x0000035c)

static inline uint32_t pa_gain_ctrl7_get(void)
{
    return REG_PL_RD(PA_GAIN_CTRL7_ADDR);
}

static inline void pa_gain_ctrl7_set(uint32_t x)
{
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,x);
}

static inline uint32_t rf_pa_short_n_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_gain_ctrl7_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_gain_ctrl7_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_gain_ctrl6_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_gain_ctrl5_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_gain_ctrl4_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_gain_ctrl3_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_gain_ctrl2_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_gain_ctrl1_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_pa_short_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_gain_ctrl0_pa_short_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_GAIN_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PA_GAIN_CTRL7_ADDR,localVal);
}


#define ACAL_CONFIG_ADDR (RF_BASE + 0x00000400)

static inline uint32_t acal_config_get(void)
{
    return REG_PL_RD(ACAL_CONFIG_ADDR);
}

static inline void acal_config_set(uint32_t x)
{
    REG_PL_WR(ACAL_CONFIG_ADDR,x);
}

static inline uint32_t rf_vco_idac_hh_getf(void)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_vco_idac_hh_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(ACAL_CONFIG_ADDR,localVal);
}

static inline uint32_t rf_vco_idac_hl_getf(void)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_vco_idac_hl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(ACAL_CONFIG_ADDR,localVal);
}

static inline uint32_t rf_vco_idac_lh_getf(void)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_vco_idac_lh_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(ACAL_CONFIG_ADDR,localVal);
}

static inline uint32_t rf_vco_idac_ll_getf(void)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_vco_idac_ll_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ACAL_CONFIG_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(ACAL_CONFIG_ADDR,localVal);
}


#define LO_CONFIG_2402_ADDR (RF_BASE + 0x00000404)

static inline uint32_t lo_config_2402_get(void)
{
    return REG_PL_RD(LO_CONFIG_2402_ADDR);
}

static inline void lo_config_2402_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2402_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2402_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2402_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2402_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2402_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2402_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2402_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2402_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2402_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2402_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2402_ADDR,localVal);
}


#define LO_CONFIG_2404_ADDR (RF_BASE + 0x00000408)

static inline uint32_t lo_config_2404_get(void)
{
    return REG_PL_RD(LO_CONFIG_2404_ADDR);
}

static inline void lo_config_2404_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2404_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2404_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2404_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2404_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2404_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2404_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2404_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2404_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2404_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2404_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2404_ADDR,localVal);
}


#define LO_CONFIG_2406_ADDR (RF_BASE + 0x0000040c)

static inline uint32_t lo_config_2406_get(void)
{
    return REG_PL_RD(LO_CONFIG_2406_ADDR);
}

static inline void lo_config_2406_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2406_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2406_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2406_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2406_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2406_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2406_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2406_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2406_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2406_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2406_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2406_ADDR,localVal);
}


#define LO_CONFIG_2408_ADDR (RF_BASE + 0x00000410)

static inline uint32_t lo_config_2408_get(void)
{
    return REG_PL_RD(LO_CONFIG_2408_ADDR);
}

static inline void lo_config_2408_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2408_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2408_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2408_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2408_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2408_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2408_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2408_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2408_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2408_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2408_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2408_ADDR,localVal);
}


#define LO_CONFIG_2410_ADDR (RF_BASE + 0x00000414)

static inline uint32_t lo_config_2410_get(void)
{
    return REG_PL_RD(LO_CONFIG_2410_ADDR);
}

static inline void lo_config_2410_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2410_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2410_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2410_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2410_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2410_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2410_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2410_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2410_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2410_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2410_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2410_ADDR,localVal);
}


#define LO_CONFIG_2412_ADDR (RF_BASE + 0x00000418)

static inline uint32_t lo_config_2412_get(void)
{
    return REG_PL_RD(LO_CONFIG_2412_ADDR);
}

static inline void lo_config_2412_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2412_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2412_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2412_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2412_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2412_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2412_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2412_ADDR,localVal);
}


#define LO_CONFIG_2414_ADDR (RF_BASE + 0x0000041c)

static inline uint32_t lo_config_2414_get(void)
{
    return REG_PL_RD(LO_CONFIG_2414_ADDR);
}

static inline void lo_config_2414_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2414_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2414_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2414_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2414_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2414_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2414_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2414_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2414_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2414_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2414_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2414_ADDR,localVal);
}


#define LO_CONFIG_2416_ADDR (RF_BASE + 0x00000420)

static inline uint32_t lo_config_2416_get(void)
{
    return REG_PL_RD(LO_CONFIG_2416_ADDR);
}

static inline void lo_config_2416_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2416_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2416_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2416_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2416_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2416_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2416_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2416_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2416_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2416_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2416_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2416_ADDR,localVal);
}


#define LO_CONFIG_2418_ADDR (RF_BASE + 0x00000424)

static inline uint32_t lo_config_2418_get(void)
{
    return REG_PL_RD(LO_CONFIG_2418_ADDR);
}

static inline void lo_config_2418_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2418_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2418_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2418_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2418_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2418_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2418_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2418_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2418_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2418_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2418_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2418_ADDR,localVal);
}


#define LO_CONFIG_2420_ADDR (RF_BASE + 0x00000428)

static inline uint32_t lo_config_2420_get(void)
{
    return REG_PL_RD(LO_CONFIG_2420_ADDR);
}

static inline void lo_config_2420_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2420_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2420_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2420_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2420_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2420_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2420_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2420_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2420_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2420_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2420_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2420_ADDR,localVal);
}


#define LO_CONFIG_2422_ADDR (RF_BASE + 0x0000042c)

static inline uint32_t lo_config_2422_get(void)
{
    return REG_PL_RD(LO_CONFIG_2422_ADDR);
}

static inline void lo_config_2422_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2422_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2422_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2422_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2422_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2422_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2422_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2422_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2422_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2422_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2422_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2422_ADDR,localVal);
}


#define LO_CONFIG_2424_ADDR (RF_BASE + 0x00000430)

static inline uint32_t lo_config_2424_get(void)
{
    return REG_PL_RD(LO_CONFIG_2424_ADDR);
}

static inline void lo_config_2424_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2424_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2424_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2424_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2424_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2424_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2424_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2424_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2424_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2424_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2424_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2424_ADDR,localVal);
}


#define LO_CONFIG_2426_ADDR (RF_BASE + 0x00000434)

static inline uint32_t lo_config_2426_get(void)
{
    return REG_PL_RD(LO_CONFIG_2426_ADDR);
}

static inline void lo_config_2426_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2426_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2426_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2426_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2426_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2426_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2426_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2426_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2426_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2426_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2426_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2426_ADDR,localVal);
}


#define LO_CONFIG_2428_ADDR (RF_BASE + 0x00000438)

static inline uint32_t lo_config_2428_get(void)
{
    return REG_PL_RD(LO_CONFIG_2428_ADDR);
}

static inline void lo_config_2428_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2428_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2428_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2428_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2428_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2428_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2428_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2428_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2428_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2428_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2428_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2428_ADDR,localVal);
}


#define LO_CONFIG_2430_ADDR (RF_BASE + 0x0000043c)

static inline uint32_t lo_config_2430_get(void)
{
    return REG_PL_RD(LO_CONFIG_2430_ADDR);
}

static inline void lo_config_2430_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2430_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2430_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2430_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2430_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2430_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2430_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2430_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2430_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2430_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2430_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2430_ADDR,localVal);
}


#define LO_CONFIG_2432_ADDR (RF_BASE + 0x00000440)

static inline uint32_t lo_config_2432_get(void)
{
    return REG_PL_RD(LO_CONFIG_2432_ADDR);
}

static inline void lo_config_2432_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2432_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2432_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2432_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2432_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2432_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2432_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2432_ADDR,localVal);
}


#define LO_CONFIG_2434_ADDR (RF_BASE + 0x00000444)

static inline uint32_t lo_config_2434_get(void)
{
    return REG_PL_RD(LO_CONFIG_2434_ADDR);
}

static inline void lo_config_2434_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2434_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2434_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2434_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2434_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2434_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2434_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2434_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2434_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2434_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2434_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2434_ADDR,localVal);
}


#define LO_CONFIG_2436_ADDR (RF_BASE + 0x00000448)

static inline uint32_t lo_config_2436_get(void)
{
    return REG_PL_RD(LO_CONFIG_2436_ADDR);
}

static inline void lo_config_2436_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2436_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2436_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2436_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2436_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2436_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2436_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2436_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2436_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2436_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2436_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2436_ADDR,localVal);
}


#define LO_CONFIG_2438_ADDR (RF_BASE + 0x0000044c)

static inline uint32_t lo_config_2438_get(void)
{
    return REG_PL_RD(LO_CONFIG_2438_ADDR);
}

static inline void lo_config_2438_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2438_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2438_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2438_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2438_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2438_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2438_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2438_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2438_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2438_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2438_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2438_ADDR,localVal);
}


#define LO_CONFIG_2440_ADDR (RF_BASE + 0x00000450)

static inline uint32_t lo_config_2440_get(void)
{
    return REG_PL_RD(LO_CONFIG_2440_ADDR);
}

static inline void lo_config_2440_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2440_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2440_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2440_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2440_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2440_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2440_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2440_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2440_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2440_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2440_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2440_ADDR,localVal);
}


#define LO_CONFIG_2442_ADDR (RF_BASE + 0x00000454)

static inline uint32_t lo_config_2442_get(void)
{
    return REG_PL_RD(LO_CONFIG_2442_ADDR);
}

static inline void lo_config_2442_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2442_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2442_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2442_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2442_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2442_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2442_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2442_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2442_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2442_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2442_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2442_ADDR,localVal);
}


#define LO_CONFIG_2444_ADDR (RF_BASE + 0x00000458)

static inline uint32_t lo_config_2444_get(void)
{
    return REG_PL_RD(LO_CONFIG_2444_ADDR);
}

static inline void lo_config_2444_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2444_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2444_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2444_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2444_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2444_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2444_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2444_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2444_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2444_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2444_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2444_ADDR,localVal);
}


#define LO_CONFIG_2446_ADDR (RF_BASE + 0x0000045c)

static inline uint32_t lo_config_2446_get(void)
{
    return REG_PL_RD(LO_CONFIG_2446_ADDR);
}

static inline void lo_config_2446_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2446_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2446_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2446_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2446_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2446_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2446_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2446_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2446_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2446_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2446_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2446_ADDR,localVal);
}


#define LO_CONFIG_2448_ADDR (RF_BASE + 0x00000460)

static inline uint32_t lo_config_2448_get(void)
{
    return REG_PL_RD(LO_CONFIG_2448_ADDR);
}

static inline void lo_config_2448_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2448_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2448_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2448_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2448_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2448_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2448_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2448_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2448_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2448_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2448_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2448_ADDR,localVal);
}


#define LO_CONFIG_2450_ADDR (RF_BASE + 0x00000464)

static inline uint32_t lo_config_2450_get(void)
{
    return REG_PL_RD(LO_CONFIG_2450_ADDR);
}

static inline void lo_config_2450_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2450_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2450_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2450_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2450_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2450_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2450_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2450_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2450_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2450_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2450_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2450_ADDR,localVal);
}


#define LO_CONFIG_2452_ADDR (RF_BASE + 0x00000468)

static inline uint32_t lo_config_2452_get(void)
{
    return REG_PL_RD(LO_CONFIG_2452_ADDR);
}

static inline void lo_config_2452_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2452_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2452_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2452_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2452_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2452_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2452_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2452_ADDR,localVal);
}


#define LO_CONFIG_2454_ADDR (RF_BASE + 0x0000046c)

static inline uint32_t lo_config_2454_get(void)
{
    return REG_PL_RD(LO_CONFIG_2454_ADDR);
}

static inline void lo_config_2454_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2454_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2454_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2454_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2454_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2454_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2454_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2454_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2454_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2454_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2454_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2454_ADDR,localVal);
}


#define LO_CONFIG_2456_ADDR (RF_BASE + 0x00000470)

static inline uint32_t lo_config_2456_get(void)
{
    return REG_PL_RD(LO_CONFIG_2456_ADDR);
}

static inline void lo_config_2456_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2456_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2456_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2456_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2456_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2456_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2456_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2456_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2456_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2456_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2456_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2456_ADDR,localVal);
}


#define LO_CONFIG_2458_ADDR (RF_BASE + 0x00000474)

static inline uint32_t lo_config_2458_get(void)
{
    return REG_PL_RD(LO_CONFIG_2458_ADDR);
}

static inline void lo_config_2458_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2458_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2458_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2458_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2458_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2458_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2458_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2458_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2458_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2458_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2458_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2458_ADDR,localVal);
}


#define LO_CONFIG_2460_ADDR (RF_BASE + 0x00000478)

static inline uint32_t lo_config_2460_get(void)
{
    return REG_PL_RD(LO_CONFIG_2460_ADDR);
}

static inline void lo_config_2460_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2460_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2460_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2460_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2460_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2460_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2460_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2460_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2460_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2460_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2460_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2460_ADDR,localVal);
}


#define LO_CONFIG_2462_ADDR (RF_BASE + 0x0000047c)

static inline uint32_t lo_config_2462_get(void)
{
    return REG_PL_RD(LO_CONFIG_2462_ADDR);
}

static inline void lo_config_2462_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2462_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2462_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2462_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2462_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2462_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2462_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2462_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2462_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2462_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2462_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2462_ADDR,localVal);
}


#define LO_CONFIG_2464_ADDR (RF_BASE + 0x00000480)

static inline uint32_t lo_config_2464_get(void)
{
    return REG_PL_RD(LO_CONFIG_2464_ADDR);
}

static inline void lo_config_2464_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2464_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2464_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2464_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2464_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2464_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2464_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2464_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2464_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2464_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2464_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2464_ADDR,localVal);
}


#define LO_CONFIG_2466_ADDR (RF_BASE + 0x00000484)

static inline uint32_t lo_config_2466_get(void)
{
    return REG_PL_RD(LO_CONFIG_2466_ADDR);
}

static inline void lo_config_2466_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2466_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2466_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2466_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2466_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2466_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2466_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2466_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2466_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2466_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2466_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2466_ADDR,localVal);
}


#define LO_CONFIG_2468_ADDR (RF_BASE + 0x00000488)

static inline uint32_t lo_config_2468_get(void)
{
    return REG_PL_RD(LO_CONFIG_2468_ADDR);
}

static inline void lo_config_2468_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2468_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2468_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2468_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2468_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2468_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2468_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2468_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2468_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2468_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2468_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2468_ADDR,localVal);
}


#define LO_CONFIG_2470_ADDR (RF_BASE + 0x0000048c)

static inline uint32_t lo_config_2470_get(void)
{
    return REG_PL_RD(LO_CONFIG_2470_ADDR);
}

static inline void lo_config_2470_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2470_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2470_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2470_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2470_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2470_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2470_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2470_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2470_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2470_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2470_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2470_ADDR,localVal);
}


#define LO_CONFIG_2472_ADDR (RF_BASE + 0x00000490)

static inline uint32_t lo_config_2472_get(void)
{
    return REG_PL_RD(LO_CONFIG_2472_ADDR);
}

static inline void lo_config_2472_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2472_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2472_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2472_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2472_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2472_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2472_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2472_ADDR,localVal);
}


#define LO_CONFIG_2474_ADDR (RF_BASE + 0x00000494)

static inline uint32_t lo_config_2474_get(void)
{
    return REG_PL_RD(LO_CONFIG_2474_ADDR);
}

static inline void lo_config_2474_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2474_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2474_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2474_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2474_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2474_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2474_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2474_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2474_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2474_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2474_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2474_ADDR,localVal);
}


#define LO_CONFIG_2476_ADDR (RF_BASE + 0x00000498)

static inline uint32_t lo_config_2476_get(void)
{
    return REG_PL_RD(LO_CONFIG_2476_ADDR);
}

static inline void lo_config_2476_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2476_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2476_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2476_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2476_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2476_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2476_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2476_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2476_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2476_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2476_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2476_ADDR,localVal);
}


#define LO_CONFIG_2478_ADDR (RF_BASE + 0x0000049c)

static inline uint32_t lo_config_2478_get(void)
{
    return REG_PL_RD(LO_CONFIG_2478_ADDR);
}

static inline void lo_config_2478_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2478_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2478_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2478_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2478_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2478_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2478_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2478_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2478_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2478_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2478_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2478_ADDR,localVal);
}


#define LO_CONFIG_2480_ADDR (RF_BASE + 0x000004a0)

static inline uint32_t lo_config_2480_get(void)
{
    return REG_PL_RD(LO_CONFIG_2480_ADDR);
}

static inline void lo_config_2480_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2480_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2480_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2480_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2480_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2480_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2480_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2480_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2480_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2480_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2480_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2480_ADDR,localVal);
}


#define LO_CONFIG_2405_ADDR (RF_BASE + 0x000004a4)

static inline uint32_t lo_config_2405_get(void)
{
    return REG_PL_RD(LO_CONFIG_2405_ADDR);
}

static inline void lo_config_2405_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2405_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2405_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2405_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2405_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2405_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2405_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2405_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2405_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2405_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2405_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2405_ADDR,localVal);
}


#define LO_CONFIG_2415_ADDR (RF_BASE + 0x000004a8)

static inline uint32_t lo_config_2415_get(void)
{
    return REG_PL_RD(LO_CONFIG_2415_ADDR);
}

static inline void lo_config_2415_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2415_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2415_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2415_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2415_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2415_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2415_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2415_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2415_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2415_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2415_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2415_ADDR,localVal);
}


#define LO_CONFIG_2425_ADDR (RF_BASE + 0x000004ac)

static inline uint32_t lo_config_2425_get(void)
{
    return REG_PL_RD(LO_CONFIG_2425_ADDR);
}

static inline void lo_config_2425_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2425_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2425_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2425_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2425_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2425_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2425_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2425_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2425_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2425_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2425_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2425_ADDR,localVal);
}


#define LO_CONFIG_2435_ADDR (RF_BASE + 0x000004b0)

static inline uint32_t lo_config_2435_get(void)
{
    return REG_PL_RD(LO_CONFIG_2435_ADDR);
}

static inline void lo_config_2435_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2435_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2435_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2435_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2435_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2435_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2435_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2435_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2435_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2435_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2435_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2435_ADDR,localVal);
}


#define LO_CONFIG_2445_ADDR (RF_BASE + 0x000004b4)

static inline uint32_t lo_config_2445_get(void)
{
    return REG_PL_RD(LO_CONFIG_2445_ADDR);
}

static inline void lo_config_2445_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2445_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2445_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2445_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2445_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2445_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2445_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2445_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2445_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2445_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2445_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2445_ADDR,localVal);
}


#define LO_CONFIG_2455_ADDR (RF_BASE + 0x000004b8)

static inline uint32_t lo_config_2455_get(void)
{
    return REG_PL_RD(LO_CONFIG_2455_ADDR);
}

static inline void lo_config_2455_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2455_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2455_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2455_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2455_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2455_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2455_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2455_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2455_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2455_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2455_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2455_ADDR,localVal);
}


#define LO_CONFIG_2465_ADDR (RF_BASE + 0x000004bc)

static inline uint32_t lo_config_2465_get(void)
{
    return REG_PL_RD(LO_CONFIG_2465_ADDR);
}

static inline void lo_config_2465_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2465_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2465_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2465_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2465_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2465_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2465_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2465_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2465_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2465_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2465_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2465_ADDR,localVal);
}


#define LO_CONFIG_2475_ADDR (RF_BASE + 0x000004c0)

static inline uint32_t lo_config_2475_get(void)
{
    return REG_PL_RD(LO_CONFIG_2475_ADDR);
}

static inline void lo_config_2475_set(uint32_t x)
{
    REG_PL_WR(LO_CONFIG_2475_ADDR,x);
}

static inline uint32_t rf_adpll_sdm_dither_en_2475_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2475_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adpll_sdm_dither_en_2475_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2475_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LO_CONFIG_2475_ADDR,localVal);
}

static inline uint32_t rf_kcal_ratio_2475_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2475_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_kcal_ratio_2475_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CONFIG_2475_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(LO_CONFIG_2475_ADDR,localVal);
}


#define DG_TESTBUS_0_ADDR (RF_BASE + 0x00000500)

static inline uint32_t dg_testbus_0_get(void)
{
    return REG_PL_RD(DG_TESTBUS_0_ADDR);
}

static inline void dg_testbus_0_set(uint32_t x)
{
    REG_PL_WR(DG_TESTBUS_0_ADDR,x);
}

static inline uint32_t rf_testbus_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(DG_TESTBUS_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define DG_TESTBUS_1_ADDR (RF_BASE + 0x00000504)

static inline uint32_t dg_testbus_1_get(void)
{
    return REG_PL_RD(DG_TESTBUS_1_ADDR);
}

static inline void dg_testbus_1_set(uint32_t x)
{
    REG_PL_WR(DG_TESTBUS_1_ADDR,x);
}

static inline uint32_t rf_testbus_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(DG_TESTBUS_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_testbus_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DG_TESTBUS_1_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(DG_TESTBUS_1_ADDR,localVal);
}


#define DG_PPUD_0_ADDR (RF_BASE + 0x00000508)

static inline uint32_t dg_ppud_0_get(void)
{
    return REG_PL_RD(DG_PPUD_0_ADDR);
}

static inline void dg_ppud_0_set(uint32_t x)
{
    REG_PL_WR(DG_PPUD_0_ADDR,x);
}

static inline uint32_t rf_ppud_manaual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_ppud_manaual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(DG_PPUD_0_ADDR,localVal);
}

static inline uint32_t rf_ppud_cnt1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    return ((localVal & ((uint32_t)0x3e000000)) >> 25);
}

static inline void rf_ppud_cnt1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    localVal &= ~((uint32_t)0x3e000000);
    localVal |= (x << 25)&((uint32_t)0x3e000000);
    REG_PL_WR(DG_PPUD_0_ADDR,localVal);
}

static inline uint32_t rf_ppud_cnt2_getf(void)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rf_ppud_cnt2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DG_PPUD_0_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16)&((uint32_t)0x01ff0000);
    REG_PL_WR(DG_PPUD_0_ADDR,localVal);
}


#define RF_TOP_ADDR (RF_BASE + 0x0000050c)

static inline uint32_t rf_top_get(void)
{
    return REG_PL_RD(RF_TOP_ADDR);
}

static inline void rf_top_set(uint32_t x)
{
    REG_PL_WR(RF_TOP_ADDR,x);
}

static inline uint32_t rf_tx_en_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_tx_en_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_tx_en_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_tx_en_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_rx_en_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_rx_en_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_rx_en_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_rx_en_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_rx_mode_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_rx_mode_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_rx_mode_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_rx_mode_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rf_rx_mode_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline uint32_t rf_mac_lo_time_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_mac_lo_time_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rfckg_afifo_tx_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rfckg_afifo_tx_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rfckg_afifo_rxadc_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rfckg_afifo_rxadc_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}

static inline uint32_t rfckg_afifo_adpll_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rfckg_afifo_adpll_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_TOP_ADDR,localVal);
}


#define RF_FSM_ADDR (RF_BASE + 0x00000510)

static inline uint32_t rf_fsm_get(void)
{
    return REG_PL_RD(RF_FSM_ADDR);
}

static inline void rf_fsm_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_ADDR,x);
}

static inline uint32_t rf_fsm_afifo_dly_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rf_fsm_afifo_dly_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26)&((uint32_t)0xfc000000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_tx_afifo_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_fsm_tx_afifo_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_tx_afifo_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_fsm_tx_afifo_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_rx_afifo_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_fsm_rx_afifo_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_rx_afifo_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_fsm_rx_afifo_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_fsm_st_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_fsm_st_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}

static inline uint32_t rf_fsm_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline uint32_t rf_fsm_lo_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_fsm_lo_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF_FSM_ADDR,localVal);
}


#define RF_SINGEN_0_ADDR (RF_BASE + 0x00000514)

static inline uint32_t rf_singen_0_get(void)
{
    return REG_PL_RD(RF_SINGEN_0_ADDR);
}

static inline void rf_singen_0_set(uint32_t x)
{
    REG_PL_WR(RF_SINGEN_0_ADDR,x);
}

static inline uint32_t rf_singen_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_singen_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_SINGEN_0_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    return ((localVal & ((uint32_t)0x60000000)) >> 29);
}

static inline void rf_singen_clkdiv_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    localVal &= ~((uint32_t)0x60000000);
    localVal |= (x << 29)&((uint32_t)0x60000000);
    REG_PL_WR(RF_SINGEN_0_ADDR,localVal);
}

static inline uint32_t rf_singen_unsign_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_singen_unsign_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_SINGEN_0_ADDR,localVal);
}

static inline uint32_t rf_singen_inc_step0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void rf_singen_inc_step0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(RF_SINGEN_0_ADDR,localVal);
}

static inline uint32_t rf_singen_inc_step1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_singen_inc_step1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RF_SINGEN_0_ADDR,localVal);
}


#define RF_SINGEN_1_ADDR (RF_BASE + 0x00000518)

static inline uint32_t rf_singen_1_get(void)
{
    return REG_PL_RD(RF_SINGEN_1_ADDR);
}

static inline void rf_singen_1_set(uint32_t x)
{
    REG_PL_WR(RF_SINGEN_1_ADDR,x);
}

static inline uint32_t rf_singen_mode_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_singen_mode_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(RF_SINGEN_1_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void rf_singen_clkdiv_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(RF_SINGEN_1_ADDR,localVal);
}

static inline uint32_t rf_singen_mode_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_singen_mode_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(RF_SINGEN_1_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_singen_clkdiv_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_1_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RF_SINGEN_1_ADDR,localVal);
}


#define RF_SINGEN_2_ADDR (RF_BASE + 0x0000051c)

static inline uint32_t rf_singen_2_get(void)
{
    return REG_PL_RD(RF_SINGEN_2_ADDR);
}

static inline void rf_singen_2_set(uint32_t x)
{
    REG_PL_WR(RF_SINGEN_2_ADDR,x);
}

static inline uint32_t rf_singen_start_addr0_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void rf_singen_start_addr0_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22)&((uint32_t)0xffc00000);
    REG_PL_WR(RF_SINGEN_2_ADDR,localVal);
}

static inline uint32_t rf_singen_start_addr1_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    return ((localVal & ((uint32_t)0x003ff000)) >> 12);
}

static inline void rf_singen_start_addr1_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    localVal &= ~((uint32_t)0x003ff000);
    localVal |= (x << 12)&((uint32_t)0x003ff000);
    REG_PL_WR(RF_SINGEN_2_ADDR,localVal);
}

static inline uint32_t rf_singen_gain_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_singen_gain_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_2_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(RF_SINGEN_2_ADDR,localVal);
}


#define RF_SINGEN_3_ADDR (RF_BASE + 0x00000520)

static inline uint32_t rf_singen_3_get(void)
{
    return REG_PL_RD(RF_SINGEN_3_ADDR);
}

static inline void rf_singen_3_set(uint32_t x)
{
    REG_PL_WR(RF_SINGEN_3_ADDR,x);
}

static inline uint32_t rf_singen_start_addr0_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void rf_singen_start_addr0_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22)&((uint32_t)0xffc00000);
    REG_PL_WR(RF_SINGEN_3_ADDR,localVal);
}

static inline uint32_t rf_singen_start_addr1_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    return ((localVal & ((uint32_t)0x003ff000)) >> 12);
}

static inline void rf_singen_start_addr1_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    localVal &= ~((uint32_t)0x003ff000);
    localVal |= (x << 12)&((uint32_t)0x003ff000);
    REG_PL_WR(RF_SINGEN_3_ADDR,localVal);
}

static inline uint32_t rf_singen_gain_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_singen_gain_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_3_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(RF_SINGEN_3_ADDR,localVal);
}


#define RF_SINGEN_4_ADDR (RF_BASE + 0x00000524)

static inline uint32_t rf_singen_4_get(void)
{
    return REG_PL_RD(RF_SINGEN_4_ADDR);
}

static inline void rf_singen_4_set(uint32_t x)
{
    REG_PL_WR(RF_SINGEN_4_ADDR,x);
}

static inline uint32_t rf_singen_fix_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_singen_fix_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_SINGEN_4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf_singen_fix_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(RF_SINGEN_4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_singen_fix_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RF_SINGEN_4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_singen_fix_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SINGEN_4_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF_SINGEN_4_ADDR,localVal);
}


#define RF_SRAM_CTRL0_ADDR (RF_BASE + 0x00000528)

static inline uint32_t rf_sram_ctrl0_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL0_ADDR);
}

static inline void rf_sram_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,x);
}

static inline uint32_t rf_sram_done_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_sram_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_sram_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_sts_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_sram_sts_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_sram_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_sram_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RF_SRAM_CTRL1_ADDR (RF_BASE + 0x0000052c)

static inline uint32_t rf_sram_ctrl1_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL1_ADDR);
}

static inline void rf_sram_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,x);
}

static inline uint32_t rf_sram_addr_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void rf_sram_addr_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_sram_addr_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_sram_addr_end_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,localVal);
}


#define RF_SRAM_CTRL2_ADDR (RF_BASE + 0x00000530)

static inline uint32_t rf_sram_ctrl2_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL2_ADDR);
}

static inline void rf_sram_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL2_ADDR,x);
}

static inline uint32_t rf_sram_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RF_TEST_MODE_ADDR (RF_BASE + 0x00000534)

static inline uint32_t rf_test_mode_get(void)
{
    return REG_PL_RD(RF_TEST_MODE_ADDR);
}

static inline void rf_test_mode_set(uint32_t x)
{
    REG_PL_WR(RF_TEST_MODE_ADDR,x);
}

static inline uint32_t rf_test_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_test_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF_TEST_MODE_ADDR,localVal);
}

static inline uint32_t rf_dacout_4s_sram_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_dacout_4s_sram_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(RF_TEST_MODE_ADDR,localVal);
}

static inline uint32_t rf_dacout_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_dacout_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_TEST_MODE_ADDR,localVal);
}

static inline uint32_t rf_dacout_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf_dacout_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(RF_TEST_MODE_ADDR,localVal);
}

static inline uint32_t rf_dacout_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TEST_MODE_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}


#define RF_RX_PULSE_FILTER_ADDR (RF_BASE + 0x00000538)

static inline uint32_t rf_rx_pulse_filter_get(void)
{
    return REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
}

static inline void rf_rx_pulse_filter_set(uint32_t x)
{
    REG_PL_WR(RF_RX_PULSE_FILTER_ADDR,x);
}

static inline uint32_t rf_pf_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pf_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(RF_RX_PULSE_FILTER_ADDR,localVal);
}

static inline uint32_t rf_pf_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pf_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RF_RX_PULSE_FILTER_ADDR,localVal);
}

static inline uint32_t rf_pf_th1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    return ((localVal & ((uint32_t)0x0007fc00)) >> 10);
}

static inline void rf_pf_th1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    localVal &= ~((uint32_t)0x0007fc00);
    localVal |= (x << 10)&((uint32_t)0x0007fc00);
    REG_PL_WR(RF_RX_PULSE_FILTER_ADDR,localVal);
}

static inline uint32_t rf_pf_th2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_pf_th2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RX_PULSE_FILTER_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0)&((uint32_t)0x000001ff);
    REG_PL_WR(RF_RX_PULSE_FILTER_ADDR,localVal);
}

#endif
