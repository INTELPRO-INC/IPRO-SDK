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
// Date: 20211018

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef RF_BASE
#define RF_BASE (0x20001000)
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

static inline uint32_t rf_id_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_REV_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define RF_FSM_CTRL_HW_ADDR (RF_BASE + 0x00000004)

static inline uint32_t rf_fsm_ctrl_hw_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
}

static inline void rf_fsm_ctrl_hw_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,x);
}

static inline uint32_t rf_rc_state_value_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline uint32_t rf_fsm_st_int_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_fsm_st_int_set_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_fsm_st_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_fsm_st_int_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_fsm_st_int_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_rc_state_dbg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_rc_state_dbg_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_rc_state_dbg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_rc_state_dbg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_fsm_state_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline uint32_t rf_fsm_t2r_cal_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_fsm_t2r_cal_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_fsm_ctrl_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_fsm_ctrl_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_FSM_CTRL_HW_ADDR,localVal);
}


#define RF_FSM_CTRL_SW_ADDR (RF_BASE + 0x00000008)

static inline uint32_t rf_fsm_ctrl_sw_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
}

static inline void rf_fsm_ctrl_sw_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL_SW_ADDR,x);
}

static inline uint32_t rf_lo_unlocked_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_inc_cal_timeout_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_inc_cal_timeout_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RF_FSM_CTRL_SW_ADDR,localVal);
}

static inline uint32_t rf_full_cal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_full_cal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RF_FSM_CTRL_SW_ADDR,localVal);
}

static inline uint32_t rf_fsm_sw_st_vld_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_fsm_sw_st_vld_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RF_FSM_CTRL_SW_ADDR,localVal);
}

static inline uint32_t rf_fsm_sw_st_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf_fsm_sw_st_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL_SW_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(RF_FSM_CTRL_SW_ADDR,localVal);
}


#define RFCTRL_HW_EN_ADDR (RF_BASE + 0x0000000c)

static inline uint32_t rfctrl_hw_en_get(void)
{
    return REG_PL_RD(RFCTRL_HW_EN_ADDR);
}

static inline void rfctrl_hw_en_set(uint32_t x)
{
    REG_PL_WR(RFCTRL_HW_EN_ADDR,x);
}

static inline uint32_t rf_reserved3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCTRL_HW_EN_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_reserved3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCTRL_HW_EN_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RFCTRL_HW_EN_ADDR,localVal);
}


#define TEMP_COMP_ADDR (RF_BASE + 0x00000010)

static inline uint32_t temp_comp_get(void)
{
    return REG_PL_RD(TEMP_COMP_ADDR);
}

static inline void temp_comp_set(uint32_t x)
{
    REG_PL_WR(TEMP_COMP_ADDR,x);
}

static inline uint32_t rf_temp_comp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_temp_comp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(TEMP_COMP_ADDR,localVal);
}

static inline uint32_t rf_const_fcal_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_const_fcal_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(TEMP_COMP_ADDR,localVal);
}

static inline uint32_t rf_const_acal_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_const_acal_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEMP_COMP_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(TEMP_COMP_ADDR,localVal);
}


#define RFCAL_STATUS_ADDR (RF_BASE + 0x00000014)

static inline uint32_t rfcal_status_get(void)
{
    return REG_PL_RD(RFCAL_STATUS_ADDR);
}

static inline void rfcal_status_set(uint32_t x)
{
    REG_PL_WR(RFCAL_STATUS_ADDR,x);
}

static inline uint32_t rf_dpd_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_dpd_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_tenscal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_tenscal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_pwdet_cal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_pwdet_cal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_riqcal_status_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_riqcal_status_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_tiqcal_status_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline void rf_tiqcal_status_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00c00000);
    localVal |= (x << 22)&((uint32_t)0x00c00000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_lo_leakcal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lo_leakcal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_rccal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_rccal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_tos_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_tos_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_ros_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_ros_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_clkpll_cal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_clkpll_cal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_inc_acal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_inc_acal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_inc_fcal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_inc_fcal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_acal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_acal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_fcal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_fcal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_adc_oscal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_adc_oscal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}

static inline uint32_t rf_rcal_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_rcal_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RFCAL_STATUS_ADDR,localVal);
}


#define RFCAL_STATUS2_ADDR (RF_BASE + 0x00000018)

static inline uint32_t rfcal_status2_get(void)
{
    return REG_PL_RD(RFCAL_STATUS2_ADDR);
}

static inline void rfcal_status2_set(uint32_t x)
{
    REG_PL_WR(RFCAL_STATUS2_ADDR,x);
}

static inline uint32_t rf_dl_rfcal_table_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS2_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_dl_rfcal_table_status_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATUS2_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RFCAL_STATUS2_ADDR,localVal);
}


#define RFCAL_CTRLEN_ADDR (RF_BASE + 0x0000001c)

static inline uint32_t rfcal_ctrlen_get(void)
{
    return REG_PL_RD(RFCAL_CTRLEN_ADDR);
}

static inline void rfcal_ctrlen_set(uint32_t x)
{
    REG_PL_WR(RFCAL_CTRLEN_ADDR,x);
}

static inline uint32_t rf_dpd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_dpd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_tsencal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_tsencal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_pwdet_cal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_pwdet_cal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_riqcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_riqcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_tiqcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_tiqcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_lo_leakcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_lo_leakcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_rccal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_rccal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_toscal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_toscal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_roscal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_roscal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_clkpll_cal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_clkpll_cal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_roscal_inc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_roscal_inc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_acal_inc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_acal_inc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_fcal_inc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_fcal_inc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_acal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_acal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_fcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_fcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_dl_rfcal_table_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_dl_rfcal_table_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_adc_oscal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_adc_oscal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}

static inline uint32_t rf_rcal_en_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_rcal_en_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_CTRLEN_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RFCAL_CTRLEN_ADDR,localVal);
}


#define RFCAL_STATEEN_ADDR (RF_BASE + 0x00000020)

static inline uint32_t rfcal_stateen_get(void)
{
    return REG_PL_RD(RFCAL_STATEEN_ADDR);
}

static inline void rfcal_stateen_set(uint32_t x)
{
    REG_PL_WR(RFCAL_STATEEN_ADDR,x);
}

static inline uint32_t rfcal_level_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rfcal_level_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_dpd_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_dpd_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_tsencal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_tsencal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_pwdet_cal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pwdet_cal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_riqcal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_riqcal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_tiqcal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_tiqcal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_lo_leakcal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_lo_leakcal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_rccal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_rccal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_toscal_sten_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_toscal_sten_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_roscal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_roscal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_clkpll_cal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_clkpll_cal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_inc_acal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_inc_acal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_inc_fcal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_inc_fcal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_acal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_acal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_fcal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_fcal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_dl_rfcal_table_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_dl_rfcal_table_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_adc_oscal_sten_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_adc_oscal_sten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}

static inline uint32_t rf_rcal_sten_resv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_rcal_sten_resv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFCAL_STATEEN_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RFCAL_STATEEN_ADDR,localVal);
}


#define SARADC_RESV_ADDR (RF_BASE + 0x00000024)

static inline uint32_t saradc_resv_get(void)
{
    return REG_PL_RD(SARADC_RESV_ADDR);
}

static inline void saradc_resv_set(uint32_t x)
{
    REG_PL_WR(SARADC_RESV_ADDR,x);
}


#define RF_BASE_CTRL1_ADDR (RF_BASE + 0x00000028)

static inline uint32_t rf_base_ctrl1_get(void)
{
    return REG_PL_RD(RF_BASE_CTRL1_ADDR);
}

static inline void rf_base_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_BASE_CTRL1_ADDR,x);
}

static inline uint32_t rf_mbg_trim_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x18000000)) >> 27);
}

static inline void rf_mbg_trim_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x18000000);
    localVal |= (x << 27)&((uint32_t)0x18000000);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pud_vbuf_lodist_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_pud_vbuf_lodist_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pud_vbuf_fbdv_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_pud_vbuf_fbdv_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pud_pa_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_pud_pa_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pud_iref_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_pud_iref_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_pud_vco_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_pud_vco_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_ppu_lead_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_ppu_lead_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_rst_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_lo_sdm_rst_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_aupll_sdm_rst_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_aupll_sdm_rst_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RF_BASE_CTRL1_ADDR,localVal);
}


#define RF_BASE_CTRL2_ADDR (RF_BASE + 0x0000002c)

static inline uint32_t rf_base_ctrl2_get(void)
{
    return REG_PL_RD(RF_BASE_CTRL2_ADDR);
}

static inline void rf_base_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_BASE_CTRL2_ADDR,x);
}

static inline uint32_t rf_txcal_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_txcal_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_adda_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adda_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_rbb_pkdet_out_rstn_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_en_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_rbb_pkdet_en_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_sdm_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_sdm_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_inc_fcal_ctrl_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_inc_fcal_ctrl_en_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_inc_acal_ctrl_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_inc_acal_ctrl_en_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_lo_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_lo_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_rxcal_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_rxcal_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_rbb_bw_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_lna_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_lna_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_tx_gain_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_tx_gain_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_rx_gain_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_rx_gain_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_pu_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_pu_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_BASE_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_BASE_CTRL2_ADDR,localVal);
}


#define PUCR1_ADDR (RF_BASE + 0x00000030)

static inline uint32_t pucr1_get(void)
{
    return REG_PL_RD(PUCR1_ADDR);
}

static inline void pucr1_set(uint32_t x)
{
    REG_PL_WR(PUCR1_ADDR,x);
}

static inline uint32_t rf_pu_tosdac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_pu_tosdac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_pwrmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_pu_pwrmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rosdac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_pu_rosdac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_pkdet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_pu_pkdet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_trsw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_trsw_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_txbuf_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_pu_txbuf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rxbuf_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pu_rxbuf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_osmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_pu_osmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_pfd_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_pu_pfd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_fbdv_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_pu_fbdv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_vco_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_pu_vco_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_pu_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_tbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_pu_tbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_tmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pu_tmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_pa_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_pu_pa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_op_atest_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_pu_op_atest_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pu_adc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_adc_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_adc_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_adda_ldo_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_adda_ldo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_pu_rbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_pu_rmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_pu_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_pu_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_rcal_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_pu_rcal_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PUCR1_ADDR,localVal);
}

static inline uint32_t rf_pu_sfreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_pu_sfreg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PUCR1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PUCR1_ADDR,localVal);
}


#define PUCR1_HW_ADDR (RF_BASE + 0x00000034)

static inline uint32_t pucr1_hw_get(void)
{
    return REG_PL_RD(PUCR1_HW_ADDR);
}

static inline void pucr1_hw_set(uint32_t x)
{
    REG_PL_WR(PUCR1_HW_ADDR,x);
}

static inline uint32_t rf_pu_tosdac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_pu_rosdac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t rf_pu_pkdet_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t rf_trsw_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline uint32_t rf_pu_txbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_pu_rxbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_pu_osmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t rf_pu_pfd_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t rf_pu_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t rf_pu_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_pu_dac_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t rf_pu_tbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t rf_pu_tmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_pu_pa_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_pu_adc_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t rf_adc_clk_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t rf_pu_adda_ldo_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_pu_rbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t rf_pu_rmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t rf_pu_rmxgm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_pu_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_pu_rcal_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t rf_pu_sfreg_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUCR1_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define PUCR2_ADDR (RF_BASE + 0x00000038)

static inline uint32_t pucr2_get(void)
{
    return REG_PL_RD(PUCR2_ADDR);
}

static inline void pucr2_set(uint32_t x)
{
    REG_PL_WR(PUCR2_ADDR,x);
}


#define PUCR2_HW_ADDR (RF_BASE + 0x0000003c)

static inline uint32_t pucr2_hw_get(void)
{
    return REG_PL_RD(PUCR2_HW_ADDR);
}

static inline void pucr2_hw_set(uint32_t x)
{
    REG_PL_WR(PUCR2_HW_ADDR,x);
}


#define PPU_CTRL_HW_ADDR (RF_BASE + 0x00000040)

static inline uint32_t ppu_ctrl_hw_get(void)
{
    return REG_PL_RD(PPU_CTRL_HW_ADDR);
}

static inline void ppu_ctrl_hw_set(uint32_t x)
{
    REG_PL_WR(PPU_CTRL_HW_ADDR,x);
}

static inline uint32_t rf_ppu_txbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_ppu_rxbuf_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_ppu_osmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t rf_ppu_pfd_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t rf_ppu_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t rf_ppu_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_ppu_rbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t rf_ppu_rmxgm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t rf_ppu_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PPU_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}


#define PUD_CTRL_HW_ADDR (RF_BASE + 0x00000044)

static inline uint32_t pud_ctrl_hw_get(void)
{
    return REG_PL_RD(PUD_CTRL_HW_ADDR);
}

static inline void pud_ctrl_hw_set(uint32_t x)
{
    REG_PL_WR(PUD_CTRL_HW_ADDR,x);
}

static inline uint32_t rf_pud_vco_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PUD_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}


#define TRX_GAIN1_ADDR (RF_BASE + 0x00000048)

static inline uint32_t trx_gain1_get(void)
{
    return REG_PL_RD(TRX_GAIN1_ADDR);
}

static inline void trx_gain1_set(uint32_t x)
{
    REG_PL_WR(TRX_GAIN1_ADDR,x);
}

static inline uint32_t rf_gc_tbb_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_gc_tbb_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_tbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x01f00000)) >> 20);
}

static inline void rf_gc_tbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x01f00000);
    localVal |= (x << 20)&((uint32_t)0x01f00000);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_tmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_gc_tmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline void rf_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x000000e0);
    localVal |= (x << 5)&((uint32_t)0x000000e0);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rf_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3)&((uint32_t)0x00000018);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}

static inline uint32_t rf_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(TRX_GAIN1_ADDR,localVal);
}


#define TRX_GAIN_HW_ADDR (RF_BASE + 0x0000004c)

static inline uint32_t trx_gain_hw_get(void)
{
    return REG_PL_RD(TRX_GAIN_HW_ADDR);
}

static inline void trx_gain_hw_set(uint32_t x)
{
    REG_PL_WR(TRX_GAIN_HW_ADDR,x);
}

static inline uint32_t rf_dac_bias_sel_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline uint32_t rf_gc_tbb_boost_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline uint32_t rf_gc_tbb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x01f00000)) >> 20);
}

static inline uint32_t rf_gc_tmx_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline uint32_t rf_gc_rbb2_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline uint32_t rf_gc_rbb1_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline uint32_t rf_rmxgm_ratt_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline uint32_t rf_gc_rmxgm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline uint32_t rf_gc_lna_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TRX_GAIN_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}


#define TEN_DC_ADDR (RF_BASE + 0x00000050)

static inline uint32_t ten_dc_get(void)
{
    return REG_PL_RD(TEN_DC_ADDR);
}

static inline void ten_dc_set(uint32_t x)
{
    REG_PL_WR(TEN_DC_ADDR,x);
}

static inline uint32_t rf_ten_lodist_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_ten_lodist_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_lf_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_ten_lf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_pfdcp_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_ten_pfdcp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_vco_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_ten_vco_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_vco_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_ten_vco_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_dac_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_ten_dac_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_dac_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_ten_dac_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_adc_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_ten_adc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_tbb_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_ten_tbb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_atest_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_ten_atest_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_bq_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_ten_bq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_tia_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_ten_tia_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_tmx_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_ten_tmx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_pa_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_ten_pa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_rrf_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_ten_rrf_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_ten_rrf_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_ten_rrf_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_dc_tp_15_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_dc_tp_15_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_dc_tp_18_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_dc_tp_18_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_dc_tp_25_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_dc_tp_25_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}

static inline uint32_t rf_dc_tp_33_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_dc_tp_33_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DC_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(TEN_DC_ADDR,localVal);
}


#define TEN_DIG_ADDR (RF_BASE + 0x00000054)

static inline uint32_t ten_dig_get(void)
{
    return REG_PL_RD(TEN_DIG_ADDR);
}

static inline void ten_dig_set(uint32_t x)
{
    REG_PL_WR(TEN_DIG_ADDR,x);
}

static inline uint32_t rf_dtest_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_dtest_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(TEN_DIG_ADDR,localVal);
}

static inline uint32_t rf_dtest_pull_down_rf_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_dtest_pull_down_rf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(TEN_DIG_ADDR,localVal);
}

static inline uint32_t rf_dten_lo_fref_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_dten_lo_fref_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(TEN_DIG_ADDR,localVal);
}

static inline uint32_t rf_dten_lo_fsdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_dten_lo_fsdm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_DIG_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(TEN_DIG_ADDR,localVal);
}


#define TEN_AC_ADDR (RF_BASE + 0x00000058)

static inline uint32_t ten_ac_get(void)
{
    return REG_PL_RD(TEN_AC_ADDR);
}

static inline void ten_ac_set(uint32_t x)
{
    REG_PL_WR(TEN_AC_ADDR,x);
}

static inline uint32_t rf_atest_in_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_atest_in_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_in_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_atest_in_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_out_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_atest_out_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_out_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_atest_out_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_gain_r5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_atest_gain_r5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_gain_r6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_atest_gain_r6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_gain_r7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_atest_gain_r7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_gain_r8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_atest_gain_r8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_gain_r9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_atest_gain_r9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_in_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_atest_in_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_in_trx_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_atest_in_trx_sw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_dac_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_atest_dac_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}

static inline uint32_t rf_atest_op_cc_getf(void)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_atest_op_cc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TEN_AC_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(TEN_AC_ADDR,localVal);
}


#define CIP_0_ADDR (RF_BASE + 0x0000005c)

static inline uint32_t cip_0_get(void)
{
    return REG_PL_RD(CIP_0_ADDR);
}

static inline void cip_0_set(uint32_t x)
{
    REG_PL_WR(CIP_0_ADDR,x);
}

static inline uint32_t rf_rcal_iptat_tswitch_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_rcal_iptat_tswitch_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_rcal_iptat_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_rcal_iptat_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_rcal_iptat_code_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rf_rcal_iptat_code_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16)&((uint32_t)0x001f0000);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_rcal_icx_tswitch_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_rcal_icx_tswitch_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_rcal_icx_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_rcal_icx_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_rcal_icx_code_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x000003f0)) >> 4);
}

static inline void rf_rcal_icx_code_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x000003f0);
    localVal |= (x << 4)&((uint32_t)0x000003f0);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_vg13_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_vg13_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(CIP_0_ADDR,localVal);
}

static inline uint32_t rf_vg11_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_vg11_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_0_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(CIP_0_ADDR,localVal);
}


#define PA0_ADDR (RF_BASE + 0x00000060)

static inline uint32_t pa0_get(void)
{
    return REG_PL_RD(PA0_ADDR);
}

static inline void pa0_set(uint32_t x)
{
    REG_PL_WR(PA0_ADDR,x);
}

static inline uint32_t rf_pa_pwrmx_osdac_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA0_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_pa_pwrmx_osdac_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA0_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PA0_ADDR,localVal);
}


#define PA1_ADDR (RF_BASE + 0x00000064)

static inline uint32_t pa1_get(void)
{
    return REG_PL_RD(PA1_ADDR);
}

static inline void pa1_set(uint32_t x)
{
    REG_PL_WR(PA1_ADDR,x);
}

static inline uint32_t rf_pa_att_gc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_pa_att_gc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PA1_ADDR,localVal);
}

static inline uint32_t rf_pa_pwrmx_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_pa_pwrmx_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PA1_ADDR,localVal);
}

static inline uint32_t rf_pa_pwrmx_dac_pn_switch_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_pa_pwrmx_dac_pn_switch_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PA1_ADDR,localVal);
}

static inline uint32_t rf_pa_pwrmx_osdac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    return ((localVal & ((uint32_t)0x007c0000)) >> 18);
}

static inline void rf_pa_pwrmx_osdac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    localVal &= ~((uint32_t)0x007c0000);
    localVal |= (x << 18)&((uint32_t)0x007c0000);
    REG_PL_WR(PA1_ADDR,localVal);
}

static inline uint32_t rf_pa_lz_bias_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_pa_lz_bias_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA1_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(PA1_ADDR,localVal);
}


#define PA2_ADDR (RF_BASE + 0x00000068)

static inline uint32_t pa2_get(void)
{
    return REG_PL_RD(PA2_ADDR);
}

static inline void pa2_set(uint32_t x)
{
    REG_PL_WR(PA2_ADDR,x);
}

static inline uint32_t rf_gc_pa_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0xe0000000)) >> 29);
}

static inline void rf_gc_pa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0xe0000000);
    localVal |= (x << 29)&((uint32_t)0xe0000000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_ib_fix_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_pa_ib_fix_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_lp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_pa_lp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_etb_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pa_etb_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_vbcore_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_pa_vbcore_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_vbpmos_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_pa_vbpmos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_vbpmos_iet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_pa_vbpmos_iet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_iet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_pa_iet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_iaq_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_pa_iaq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(PA2_ADDR,localVal);
}

static inline uint32_t rf_pa_vbcas_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_pa_vbcas_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PA2_ADDR,localVal);
}


#define PA3_ADDR (RF_BASE + 0x0000006c)

static inline uint32_t pa3_get(void)
{
    return REG_PL_RD(PA3_ADDR);
}

static inline void pa3_set(uint32_t x)
{
    REG_PL_WR(PA3_ADDR,x);
}

static inline uint32_t rf_gc_pa_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0xe0000000)) >> 29);
}

static inline uint32_t rf_pa_ib_fix_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t rf_pa_lp_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_pa_etb_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_pa_vbcore_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline uint32_t rf_pa_vbpmos_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline uint32_t rf_pa_vbpmos_iet_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline uint32_t rf_pa_iet_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t rf_pa_iaq_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline uint32_t rf_pa_vbcas_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA3_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}


#define TMX_ADDR (RF_BASE + 0x00000070)

static inline uint32_t tmx_get(void)
{
    return REG_PL_RD(TMX_ADDR);
}

static inline void tmx_set(uint32_t x)
{
    REG_PL_WR(TMX_ADDR,x);
}

static inline uint32_t rf_tmx_r_att_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_tmx_r_att_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(TMX_ADDR,localVal);
}

static inline uint32_t rf_tx_tsense_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_tx_tsense_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(TMX_ADDR,localVal);
}

static inline uint32_t rf_tmx_bm_cas_bulk_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_tmx_bm_cas_bulk_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(TMX_ADDR,localVal);
}

static inline uint32_t rf_tmx_bm_cas_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_tmx_bm_cas_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(TMX_ADDR,localVal);
}

static inline uint32_t rf_tmx_bm_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_tmx_bm_sw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(TMX_ADDR,localVal);
}

static inline uint32_t rf_tmx_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_tmx_cs_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TMX_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(TMX_ADDR,localVal);
}


#define TBB_ADDR (RF_BASE + 0x00000074)

static inline uint32_t tbb_get(void)
{
    return REG_PL_RD(TBB_ADDR);
}

static inline void tbb_set(uint32_t x)
{
    REG_PL_WR(TBB_ADDR,x);
}

static inline uint32_t rf_tbb_tosdac_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_tbb_tosdac_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_tosdac_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_tbb_tosdac_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_atest_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_tbb_atest_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_iq_bias_short_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_tbb_iq_bias_short_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_cflt_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_tbb_cflt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_vcm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_tbb_vcm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_bm_cg_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_tbb_bm_cg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(TBB_ADDR,localVal);
}

static inline uint32_t rf_tbb_bm_sf_getf(void)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_tbb_bm_sf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TBB_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(TBB_ADDR,localVal);
}


#define LNA_ADDR (RF_BASE + 0x00000078)

static inline uint32_t lna_get(void)
{
    return REG_PL_RD(LNA_ADDR);
}

static inline void lna_set(uint32_t x)
{
    REG_PL_WR(LNA_ADDR,x);
}

static inline uint32_t rf_lna_cmatch_preind_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_lna_cmatch_preind_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(LNA_ADDR,localVal);
}

static inline uint32_t rf_lna_lg_gsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_lna_lg_gsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(LNA_ADDR,localVal);
}

static inline uint32_t rf_lna_cap_lg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lna_cap_lg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(LNA_ADDR,localVal);
}

static inline uint32_t rf_lna_rfb_match_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_lna_rfb_match_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(LNA_ADDR,localVal);
}

static inline uint32_t rf_lna_load_csw_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t rf_lna_load_csw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_lna_load_csw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(LNA_ADDR,localVal);
}

static inline uint32_t rf_lna_bm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline uint32_t rf_lna_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_lna_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(LNA_ADDR,localVal);
}


#define RMXGM_ADDR (RF_BASE + 0x0000007c)

static inline uint32_t rmxgm_get(void)
{
    return REG_PL_RD(RMXGM_ADDR);
}

static inline void rmxgm_set(uint32_t x)
{
    REG_PL_WR(RMXGM_ADDR,x);
}

static inline uint32_t rf_rrfldo_vout_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_rrfldo_vout_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_rrfldo_pulldown_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rrfldo_pulldown_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_rrfldo_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_rrfldo_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_pu_rrfldo_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pu_rrfldo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_pu_rrf_avdd15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_pu_rrf_avdd15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_rmxgm_10m_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_rmxgm_10m_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_rmxgm_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_rmxgm_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RMXGM_ADDR,localVal);
}

static inline uint32_t rf_rmx_bm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_rmx_bm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RMXGM_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RMXGM_ADDR,localVal);
}


#define RBB1_ADDR (RF_BASE + 0x00000080)

static inline uint32_t rbb1_get(void)
{
    return REG_PL_RD(RBB1_ADDR);
}

static inline void rbb1_set(uint32_t x)
{
    REG_PL_WR(RBB1_ADDR,x);
}

static inline uint32_t rf_rosdac_range_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rosdac_range_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RBB1_ADDR,localVal);
}

static inline uint32_t rf_rbb_mfb_r2_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_rbb_mfb_r2_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RBB1_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline uint32_t rf_rosdac_q_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline uint32_t rf_rosdac_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rosdac_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(RBB1_ADDR,localVal);
}

static inline uint32_t rf_rosdac_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rosdac_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB1_ADDR,localVal);
}


#define RBB2_ADDR (RF_BASE + 0x00000084)

static inline uint32_t rbb2_get(void)
{
    return REG_PL_RD(RBB2_ADDR);
}

static inline void rbb2_set(uint32_t x)
{
    REG_PL_WR(RBB2_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_fc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rbb_cap1_fc_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(RBB2_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap1_fc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rbb_cap1_fc_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(RBB2_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rbb_cap2_fc_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(RBB2_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap2_fc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rbb_cap2_fc_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(RBB2_ADDR,localVal);
}


#define RBB3_ADDR (RF_BASE + 0x00000088)

static inline uint32_t rbb3_get(void)
{
    return REG_PL_RD(RBB3_ADDR);
}

static inline void rbb3_set(uint32_t x)
{
    REG_PL_WR(RBB3_ADDR,x);
}

static inline uint32_t rf_pwr_det_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_pwr_det_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rxiqcal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_rxiqcal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_bw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_rbb_bw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_tia_iqbias_short_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_rbb_tia_iqbias_short_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_bq_iqbias_short_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_rbb_bq_iqbias_short_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_vcm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_rbb_vcm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_bm_op_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_rbb_bm_op_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_deq_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_rbb_deq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_bt_fif_tune_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00000060)) >> 5);
}

static inline void rf_rbb_bt_fif_tune_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00000060);
    localVal |= (x << 5)&((uint32_t)0x00000060);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_startup_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_rbb_startup_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RBB3_ADDR,localVal);
}

static inline uint32_t rf_rbb_mfb_deq_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_rbb_mfb_deq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB3_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(RBB3_ADDR,localVal);
}


#define RBB4_ADDR (RF_BASE + 0x0000008c)

static inline uint32_t rbb4_get(void)
{
    return REG_PL_RD(RBB4_ADDR);
}

static inline void rbb4_set(uint32_t x)
{
    REG_PL_WR(RBB4_ADDR,x);
}

static inline uint32_t rf_rbb_cap1_deq_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_rbb_cap1_deq_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_cap1_deq_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_rbb_cap1_deq_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_bt_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_rbb_bt_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_bt_mode_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t rf_pkdet_out_latch_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_pkdet_out_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_rbb_pkdet_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_rbb_mfb_c1_fc_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_rbb_mfb_c1_fc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_rbb_pkdet_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_out_rstn_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_rbb_pkdet_out_rstn_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RBB4_ADDR,localVal);
}

static inline uint32_t rf_rbb_pkdet_vth_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_rbb_pkdet_vth_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB4_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(RBB4_ADDR,localVal);
}


#define ADDA1_ADDR (RF_BASE + 0x00000090)

static inline uint32_t adda1_get(void)
{
    return REG_PL_RD(ADDA1_ADDR);
}

static inline void adda1_set(uint32_t x)
{
    REG_PL_WR(ADDA1_ADDR,x);
}

static inline uint32_t rf_dac_nsink_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_dac_nsink_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_vlow_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_dac_vlow_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_adda_ldo_dvdd_sel_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline uint32_t rf_adda_ldo_dvdd_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_adda_ldo_dvdd_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_adda_ldo_byps_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_adda_ldo_byps_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_rccal_iqswap_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_dac_rccal_iqswap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_clk_sync_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_dac_clk_sync_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_rccalsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_dac_rccalsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_dac_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_bias_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_dac_bias_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(ADDA1_ADDR,localVal);
}

static inline uint32_t rf_dac_dvdd_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_dac_dvdd_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(ADDA1_ADDR,localVal);
}


#define ADDA2_ADDR (RF_BASE + 0x00000094)

static inline uint32_t adda2_get(void)
{
    return REG_PL_RD(ADDA2_ADDR);
}

static inline void adda2_set(uint32_t x)
{
    REG_PL_WR(ADDA2_ADDR,x);
}

static inline uint32_t rf_adc_vbuf_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_adc_vbuf_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_ti_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_adc_ti_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_clk_div_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_adc_clk_div_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_vref_comp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_adc_vref_comp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_clk_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_adc_clk_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_dclk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_adc_dclk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_dclk_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_adc_dclk_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_gt_rm_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_adc_gt_rm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_sar_ascal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_adc_sar_ascal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_dvdd_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_adc_dvdd_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_dly_ctl_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_adc_dly_ctl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(ADDA2_ADDR,localVal);
}

static inline uint32_t rf_adc_vref_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_adc_vref_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA2_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(ADDA2_ADDR,localVal);
}


#define ADDA3_ADDR (RF_BASE + 0x00000098)

static inline uint32_t adda3_get(void)
{
    return REG_PL_RD(ADDA3_ADDR);
}

static inline void adda3_set(uint32_t x)
{
    REG_PL_WR(ADDA3_ADDR,x);
}

static inline uint32_t rf_adc_dout_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA3_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline uint32_t rf_adc_dout_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA3_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}


#define VCO1_ADDR (RF_BASE + 0x000000a0)

static inline uint32_t vco1_get(void)
{
    return REG_PL_RD(VCO1_ADDR);
}

static inline void vco1_set(uint32_t x)
{
    REG_PL_WR(VCO1_ADDR,x);
}

static inline uint32_t rf_lo_vco_idac_cw_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline uint32_t rf_lo_vco_idac_cw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(VCO1_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t rf_lo_vco_freq_cw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_lo_vco_freq_cw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(VCO1_ADDR,localVal);
}


#define VCO2_ADDR (RF_BASE + 0x000000a4)

static inline uint32_t vco2_get(void)
{
    return REG_PL_RD(VCO2_ADDR);
}

static inline void vco2_set(uint32_t x)
{
    REG_PL_WR(VCO2_ADDR,x);
}

static inline uint32_t rf_lo_vco_ldo_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_lo_vco_ldo_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_ldo_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_lo_vco_ldo_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_acal_inc_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_acal_vco_ud_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_acal_vref_cw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_acal_vref_cw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_short_idac_filter_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_lo_vco_short_idac_filter_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_short_vbias_filter_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_lo_vco_short_vbias_filter_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_boot_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_lo_vco_idac_boot_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(VCO2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_vbias_cw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_lo_vco_vbias_cw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(VCO2_ADDR,localVal);
}


#define VCO3_ADDR (RF_BASE + 0x000000a8)

static inline uint32_t vco3_get(void)
{
    return REG_PL_RD(VCO3_ADDR);
}

static inline void vco3_set(uint32_t x)
{
    REG_PL_WR(VCO3_ADDR,x);
}

static inline uint32_t rf_fcal_cnt_op_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO3_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_fcal_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO3_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_fcal_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO3_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(VCO3_ADDR,localVal);
}


#define VCO4_ADDR (RF_BASE + 0x000000ac)

static inline uint32_t vco4_get(void)
{
    return REG_PL_RD(VCO4_ADDR);
}

static inline void vco4_set(uint32_t x)
{
    REG_PL_WR(VCO4_ADDR,x);
}

static inline uint32_t rf_fcal_inc_vctrl_ud_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline uint32_t rf_fcal_cnt_rdy_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_fcal_inc_large_range_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_fcal_inc_large_range_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(VCO4_ADDR,localVal);
}

static inline uint32_t rf_fcal_inc_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t rf_fcal_cnt_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_fcal_cnt_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(VCO4_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(VCO4_ADDR,localVal);
}


#define PFDCP_ADDR (RF_BASE + 0x000000b0)

static inline uint32_t pfdcp_get(void)
{
    return REG_PL_RD(PFDCP_ADDR);
}

static inline void pfdcp_set(uint32_t x)
{
    REG_PL_WR(PFDCP_ADDR,x);
}

static inline uint32_t rf_lo_pfd_rst_csd_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t rf_lo_pfd_rst_csd_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_lo_pfd_rst_csd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_pfd_rvdd_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lo_pfd_rvdd_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_hiz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_lo_cp_hiz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_opamp_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_lo_cp_opamp_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_ota_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_lo_cp_ota_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_startup_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lo_cp_startup_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_offset_p_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lo_cp_offset_p_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_offset_n_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_cp_offset_n_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_lp_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_lo_cp_lp_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PFDCP_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_sel_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t rf_lo_cp_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lo_cp_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PFDCP_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PFDCP_ADDR,localVal);
}


#define LO_ADDR (RF_BASE + 0x000000b4)

static inline uint32_t lo_get(void)
{
    return REG_PL_RD(LO_ADDR);
}

static inline void lo_set(uint32_t x)
{
    REG_PL_WR(LO_ADDR,x);
}

static inline uint32_t rf_lo_slipped_up_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t rf_lo_slipped_dn_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_lo_lf_r4_short_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_lo_lf_r4_short_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(LO_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_r4_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_lf_r4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LO_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_cz_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_lo_lf_cz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(LO_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_rz_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00003800)) >> 11);
}

static inline void rf_lo_lf_rz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    localVal &= ~((uint32_t)0x00003800);
    localVal |= (x << 11)&((uint32_t)0x00003800);
    REG_PL_WR(LO_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_cz_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline uint32_t rf_lo_lf_r4_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline uint32_t rf_lo_lf_rz_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}


#define FBDV_ADDR (RF_BASE + 0x000000b8)

static inline uint32_t fbdv_get(void)
{
    return REG_PL_RD(FBDV_ADDR);
}

static inline void fbdv_set(uint32_t x)
{
    REG_PL_WR(FBDV_ADDR,x);
}

static inline uint32_t rf_ppu_vbuf_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t rf_pu_vbuf_fbdv_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t rf_pu_vbuf_fbdv_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pu_vbuf_fbdv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_rst_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_lo_fbdv_rst_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_lo_fbdv_rst_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_sel_fb_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_fbdv_sel_fb_clk_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_sel_sample_clk_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_fbdv_sel_sample_clk_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_halfstep_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_lo_fbdv_halfstep_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(FBDV_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_halfstep_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(FBDV_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define LODIST_ADDR (RF_BASE + 0x000000bc)

static inline uint32_t lodist_get(void)
{
    return REG_PL_RD(LODIST_ADDR);
}

static inline void lodist_set(uint32_t x)
{
    REG_PL_WR(LODIST_ADDR,x);
}

static inline uint32_t rf_ppu_vbuf_lodist_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_pu_vbuf_lodist_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t rf_pu_vbuf_lodist_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_pu_vbuf_lodist_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_lodist_dpd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_lo_lodist_dpd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_lodist_rxbuf_stre_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_lo_lodist_rxbuf_stre_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_lodist_txbuf_stre_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_lo_lodist_txbuf_stre_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_cap_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_lo_osmx_cap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_capbank_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_osmx_capbank_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_vbuf_stre_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_lo_osmx_vbuf_stre_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_fix_cap_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_lo_osmx_fix_cap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_en_xgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_lo_osmx_en_xgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(LODIST_ADDR,localVal);
}

static inline uint32_t rf_lo_osmx_xgm_boost_getf(void)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lo_osmx_xgm_boost_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LODIST_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(LODIST_ADDR,localVal);
}


#define SDM1_ADDR (RF_BASE + 0x000000c0)

static inline uint32_t sdm1_get(void)
{
    return REG_PL_RD(SDM1_ADDR);
}

static inline void sdm1_set(uint32_t x)
{
    REG_PL_WR(SDM1_ADDR,x);
}

static inline uint32_t rf_lo_sdm_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_lo_sdm_flag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(SDM1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_rstb_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t rf_lo_sdm_rstb_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf_lo_sdm_rstb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(SDM1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_lo_sdm_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(SDM1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_sdm_dither_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(SDM1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_bypass_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t rf_lo_sdm_dither_sel_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}


#define SDM2_ADDR (RF_BASE + 0x000000c4)

static inline uint32_t sdm2_get(void)
{
    return REG_PL_RD(SDM2_ADDR);
}

static inline void sdm2_set(uint32_t x)
{
    REG_PL_WR(SDM2_ADDR,x);
}

static inline uint32_t rf_lo_sdmin_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM2_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf_lo_sdmin_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SDM2_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(SDM2_ADDR,localVal);
}


#define SDM3_ADDR (RF_BASE + 0x000000c8)

static inline uint32_t sdm3_get(void)
{
    return REG_PL_RD(SDM3_ADDR);
}

static inline void sdm3_set(uint32_t x)
{
    REG_PL_WR(SDM3_ADDR,x);
}

static inline uint32_t rf_lo_sdmin_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(SDM3_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}


#define REFBUF_ADDR (RF_BASE + 0x000000cc)

static inline uint32_t refbuf_get(void)
{
    return REG_PL_RD(REFBUF_ADDR);
}

static inline void refbuf_set(uint32_t x)
{
    REG_PL_WR(REFBUF_ADDR,x);
}

static inline uint32_t rf_lo_refbuf_loclk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(REFBUF_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_lo_refbuf_loclk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(REFBUF_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(REFBUF_ADDR,localVal);
}


#define GLB_MISC_ADDR (RF_BASE + 0x000000d0)

static inline uint32_t glb_misc_get(void)
{
    return REG_PL_RD(GLB_MISC_ADDR);
}

static inline void glb_misc_set(uint32_t x)
{
    REG_PL_WR(GLB_MISC_ADDR,x);
}

static inline uint32_t rf_wifipll_sdm_bypass_tx_wf_getf(void)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_wifipll_sdm_bypass_tx_wf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(GLB_MISC_ADDR,localVal);
}

static inline uint32_t rf_wifipll_sdm_bypass_rx_wf_getf(void)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_wifipll_sdm_bypass_rx_wf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(GLB_MISC_ADDR,localVal);
}

static inline uint32_t rf_wifipll_sdm_bypass_tx_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_wifipll_sdm_bypass_tx_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(GLB_MISC_ADDR,localVal);
}

static inline uint32_t rf_wifipll_sdm_bypass_rx_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_wifipll_sdm_bypass_rx_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GLB_MISC_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(GLB_MISC_ADDR,localVal);
}


#define CIP_1_ADDR (RF_BASE + 0x000000d4)

static inline uint32_t cip_1_get(void)
{
    return REG_PL_RD(CIP_1_ADDR);
}

static inline void cip_1_set(uint32_t x)
{
    REG_PL_WR(CIP_1_ADDR,x);
}

static inline uint32_t rf_dac_iptat_scale_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_dac_iptat_scale_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(CIP_1_ADDR,localVal);
}

static inline uint32_t rf_dac_icx_scale_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_dac_icx_scale_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(CIP_1_ADDR,localVal);
}

static inline uint32_t rf_dac_imix_bias_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_dac_imix_bias_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(CIP_1_ADDR,localVal);
}

static inline uint32_t rf_pa_iptat_scale_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_pa_iptat_scale_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(CIP_1_ADDR,localVal);
}

static inline uint32_t rf_pa_icx_scale_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_pa_icx_scale_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(CIP_1_ADDR,localVal);
}

static inline uint32_t rf_pa_imix_bias_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_pa_imix_bias_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CIP_1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(CIP_1_ADDR,localVal);
}


#define RF_RESV_REG_0_ADDR (RF_BASE + 0x000000ec)

static inline uint32_t rf_resv_reg_0_get(void)
{
    return REG_PL_RD(RF_RESV_REG_0_ADDR);
}

static inline void rf_resv_reg_0_set(uint32_t x)
{
    REG_PL_WR(RF_RESV_REG_0_ADDR,x);
}

static inline uint32_t rf_reserved0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RESV_REG_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_reserved0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RESV_REG_0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_RESV_REG_0_ADDR,localVal);
}


#define RF_RESV_REG_1_ADDR (RF_BASE + 0x000000f0)

static inline uint32_t rf_resv_reg_1_get(void)
{
    return REG_PL_RD(RF_RESV_REG_1_ADDR);
}

static inline void rf_resv_reg_1_set(uint32_t x)
{
    REG_PL_WR(RF_RESV_REG_1_ADDR,x);
}

static inline uint32_t rf_reserved1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RESV_REG_1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_reserved1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_RESV_REG_1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_RESV_REG_1_ADDR,localVal);
}


#define RF_RESV_REG_2_ADDR (RF_BASE + 0x000000f4)

static inline uint32_t rf_resv_reg_2_get(void)
{
    return REG_PL_RD(RF_RESV_REG_2_ADDR);
}

static inline void rf_resv_reg_2_set(uint32_t x)
{
    REG_PL_WR(RF_RESV_REG_2_ADDR,x);
}

static inline uint32_t rf_reserved2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_RESV_REG_2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RRF_GAIN_INDEX1_ADDR (RF_BASE + 0x00000100)

static inline uint32_t rrf_gain_index1_get(void)
{
    return REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
}

static inline void rrf_gain_index1_set(uint32_t x)
{
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,x);
}

static inline uint32_t rf_gain_ctrl3_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0xe0000000)) >> 29);
}

static inline void rf_gain_ctrl3_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0xe0000000);
    localVal |= (x << 29)&((uint32_t)0xe0000000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x18000000)) >> 27);
}

static inline void rf_gain_ctrl3_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x18000000);
    localVal |= (x << 27)&((uint32_t)0x18000000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl3_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00e00000)) >> 21);
}

static inline void rf_gain_ctrl2_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00e00000);
    localVal |= (x << 21)&((uint32_t)0x00e00000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00180000)) >> 19);
}

static inline void rf_gain_ctrl2_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00180000);
    localVal |= (x << 19)&((uint32_t)0x00180000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_gain_ctrl2_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x0000e000)) >> 13);
}

static inline void rf_gain_ctrl1_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x0000e000);
    localVal |= (x << 13)&((uint32_t)0x0000e000);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

static inline void rf_gain_ctrl1_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00001800);
    localVal |= (x << 11)&((uint32_t)0x00001800);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_gain_ctrl1_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline void rf_gain_ctrl0_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x000000e0);
    localVal |= (x << 5)&((uint32_t)0x000000e0);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rf_gain_ctrl0_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3)&((uint32_t)0x00000018);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gain_ctrl0_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RRF_GAIN_INDEX1_ADDR,localVal);
}


#define RRF_GAIN_INDEX2_ADDR (RF_BASE + 0x00000104)

static inline uint32_t rrf_gain_index2_get(void)
{
    return REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
}

static inline void rrf_gain_index2_set(uint32_t x)
{
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,x);
}

static inline uint32_t rf_gain_ctrl7_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0xe0000000)) >> 29);
}

static inline void rf_gain_ctrl7_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0xe0000000);
    localVal |= (x << 29)&((uint32_t)0xe0000000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x18000000)) >> 27);
}

static inline void rf_gain_ctrl7_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x18000000);
    localVal |= (x << 27)&((uint32_t)0x18000000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_gain_ctrl7_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00e00000)) >> 21);
}

static inline void rf_gain_ctrl6_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00e00000);
    localVal |= (x << 21)&((uint32_t)0x00e00000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00180000)) >> 19);
}

static inline void rf_gain_ctrl6_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00180000);
    localVal |= (x << 19)&((uint32_t)0x00180000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_gain_ctrl6_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x0000e000)) >> 13);
}

static inline void rf_gain_ctrl5_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x0000e000);
    localVal |= (x << 13)&((uint32_t)0x0000e000);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

static inline void rf_gain_ctrl5_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00001800);
    localVal |= (x << 11)&((uint32_t)0x00001800);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_gain_ctrl5_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline void rf_gain_ctrl4_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x000000e0);
    localVal |= (x << 5)&((uint32_t)0x000000e0);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rf_gain_ctrl4_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3)&((uint32_t)0x00000018);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gain_ctrl4_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RRF_GAIN_INDEX2_ADDR,localVal);
}


#define RRF_GAIN_INDEX3_ADDR (RF_BASE + 0x00000108)

static inline uint32_t rrf_gain_index3_get(void)
{
    return REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
}

static inline void rrf_gain_index3_set(uint32_t x)
{
    REG_PL_WR(RRF_GAIN_INDEX3_ADDR,x);
}

static inline uint32_t rf_gain_ctrl8_gc_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x000000e0)) >> 5);
}

static inline void rf_gain_ctrl8_gc_lna_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x000000e0);
    localVal |= (x << 5)&((uint32_t)0x000000e0);
    REG_PL_WR(RRF_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_gc_rmxgm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rf_gain_ctrl8_gc_rmxgm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3)&((uint32_t)0x00000018);
    REG_PL_WR(RRF_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_gc_rmxgm_ratt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_gain_ctrl8_gc_rmxgm_ratt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RRF_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RRF_GAIN_INDEX3_ADDR,localVal);
}


#define LNA_CTRL_HW_MUX_ADDR (RF_BASE + 0x0000010c)

static inline uint32_t lna_ctrl_hw_mux_get(void)
{
    return REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
}

static inline void lna_ctrl_hw_mux_set(uint32_t x)
{
    REG_PL_WR(LNA_CTRL_HW_MUX_ADDR,x);
}

static inline uint32_t rf_lna_load_csw_lg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_lna_load_csw_lg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(LNA_CTRL_HW_MUX_ADDR,localVal);
}

static inline uint32_t rf_lna_load_csw_hg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_lna_load_csw_hg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(LNA_CTRL_HW_MUX_ADDR,localVal);
}

static inline uint32_t rf_lna_bm_lg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_lna_bm_lg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(LNA_CTRL_HW_MUX_ADDR,localVal);
}

static inline uint32_t rf_lna_bm_hg_getf(void)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_lna_bm_hg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LNA_CTRL_HW_MUX_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(LNA_CTRL_HW_MUX_ADDR,localVal);
}


#define RBB_GAIN_INDEX1_ADDR (RF_BASE + 0x00000110)

static inline uint32_t rbb_gain_index1_get(void)
{
    return REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
}

static inline void rbb_gain_index1_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,x);
}

static inline uint32_t rf_gain_ctrl3_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_gain_ctrl3_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl3_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_gain_ctrl3_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_gain_ctrl2_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl2_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_gain_ctrl2_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gain_ctrl1_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl1_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_gain_ctrl1_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl0_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl0_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_gain_ctrl0_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX1_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RBB_GAIN_INDEX1_ADDR,localVal);
}


#define RBB_GAIN_INDEX2_ADDR (RF_BASE + 0x00000114)

static inline uint32_t rbb_gain_index2_get(void)
{
    return REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
}

static inline void rbb_gain_index2_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,x);
}

static inline uint32_t rf_gain_ctrl7_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_gain_ctrl7_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl7_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_gain_ctrl7_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_gain_ctrl6_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl6_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_gain_ctrl6_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gain_ctrl5_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl5_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_gain_ctrl5_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl4_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl4_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_gain_ctrl4_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX2_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RBB_GAIN_INDEX2_ADDR,localVal);
}


#define RBB_GAIN_INDEX3_ADDR (RF_BASE + 0x00000118)

static inline uint32_t rbb_gain_index3_get(void)
{
    return REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
}

static inline void rbb_gain_index3_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,x);
}

static inline uint32_t rf_gain_ctrl11_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_gain_ctrl11_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl11_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_gain_ctrl11_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_gain_ctrl10_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl10_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_gain_ctrl10_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gain_ctrl9_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl9_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_gain_ctrl9_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl8_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl8_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_gain_ctrl8_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX3_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RBB_GAIN_INDEX3_ADDR,localVal);
}


#define RBB_GAIN_INDEX4_ADDR (RF_BASE + 0x0000011c)

static inline uint32_t rbb_gain_index4_get(void)
{
    return REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
}

static inline void rbb_gain_index4_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,x);
}

static inline uint32_t rf_gain_ctrl15_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_gain_ctrl15_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl15_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_gain_ctrl15_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rf_gain_ctrl14_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20)&((uint32_t)0x00700000);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl14_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_gain_ctrl14_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_gain_ctrl13_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl13_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_gain_ctrl13_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl12_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl12_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_gain_ctrl12_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX4_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RBB_GAIN_INDEX4_ADDR,localVal);
}


#define RBB_GAIN_INDEX5_ADDR (RF_BASE + 0x00000120)

static inline uint32_t rbb_gain_index5_get(void)
{
    return REG_PL_RD(RBB_GAIN_INDEX5_ADDR);
}

static inline void rbb_gain_index5_set(uint32_t x)
{
    REG_PL_WR(RBB_GAIN_INDEX5_ADDR,x);
}

static inline uint32_t rf_gain_ctrl16_gc_rbb2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX5_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_gain_ctrl16_gc_rbb2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX5_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RBB_GAIN_INDEX5_ADDR,localVal);
}

static inline uint32_t rf_gain_ctrl16_gc_rbb1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX5_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_gain_ctrl16_gc_rbb1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_GAIN_INDEX5_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(RBB_GAIN_INDEX5_ADDR,localVal);
}


#define PA_CONFIG_BZ_ADDR (RF_BASE + 0x00000130)

static inline uint32_t pa_config_bz_get(void)
{
    return REG_PL_RD(PA_CONFIG_BZ_ADDR);
}

static inline void pa_config_bz_set(uint32_t x)
{
    REG_PL_WR(PA_CONFIG_BZ_ADDR,x);
}

static inline uint32_t rf_pa_ib_fix_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_pa_ib_fix_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_etb_en_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_pa_etb_en_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_vbcore_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_pa_vbcore_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_vbpmos_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_pa_vbpmos_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_vbpmos_iet_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_pa_vbpmos_iet_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_iet_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_pa_iet_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_iaq_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_pa_iaq_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}

static inline uint32_t rf_pa_vbcas_bz_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_pa_vbcas_bz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_CONFIG_BZ_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PA_CONFIG_BZ_ADDR,localVal);
}


#define ADDA_REG_CTRL_HW_ADDR (RF_BASE + 0x00000134)

static inline uint32_t adda_reg_ctrl_hw_get(void)
{
    return REG_PL_RD(ADDA_REG_CTRL_HW_ADDR);
}

static inline void adda_reg_ctrl_hw_set(uint32_t x)
{
    REG_PL_WR(ADDA_REG_CTRL_HW_ADDR,x);
}

static inline uint32_t rf_adda_ldo_dvdd_sel_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA_REG_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_adda_ldo_dvdd_sel_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA_REG_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(ADDA_REG_CTRL_HW_ADDR,localVal);
}

static inline uint32_t rf_adda_ldo_dvdd_sel_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADDA_REG_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_adda_ldo_dvdd_sel_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADDA_REG_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(ADDA_REG_CTRL_HW_ADDR,localVal);
}


#define LO_REG_CTRL_HW1_ADDR (RF_BASE + 0x00000138)

static inline uint32_t lo_reg_ctrl_hw1_get(void)
{
    return REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
}

static inline void lo_reg_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,x);
}

static inline uint32_t rf_lo_lf_r4_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_lo_lf_r4_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_r4_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lo_lf_r4_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_rz_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_lo_lf_rz_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_rz_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_lo_lf_rz_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_cz_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_lf_cz_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_lf_cz_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_lf_cz_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_sel_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_lo_cp_sel_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_cp_sel_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_lo_cp_sel_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_halfstep_en_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_lo_fbdv_halfstep_en_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_fbdv_halfstep_en_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_lo_fbdv_halfstep_en_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_REG_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(LO_REG_CTRL_HW1_ADDR,localVal);
}


#define LO_CAL_CTRL_HW1_ADDR (RF_BASE + 0x0000013c)

static inline uint32_t lo_cal_ctrl_hw1_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
}

static inline void lo_cal_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW1_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2408_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2408_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2408_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2408_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2404_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2404_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2404_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2404_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW1_ADDR,localVal);
}


#define LO_CAL_CTRL_HW2_ADDR (RF_BASE + 0x00000140)

static inline uint32_t lo_cal_ctrl_hw2_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
}

static inline void lo_cal_ctrl_hw2_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW2_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2416_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2416_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2416_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2416_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW2_ADDR,localVal);
}


#define LO_CAL_CTRL_HW3_ADDR (RF_BASE + 0x00000144)

static inline uint32_t lo_cal_ctrl_hw3_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
}

static inline void lo_cal_ctrl_hw3_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW3_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2424_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2424_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2424_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2424_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2420_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2420_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2420_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2420_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW3_ADDR,localVal);
}


#define LO_CAL_CTRL_HW4_ADDR (RF_BASE + 0x00000148)

static inline uint32_t lo_cal_ctrl_hw4_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
}

static inline void lo_cal_ctrl_hw4_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW4_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2428_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2428_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2428_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2428_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW4_ADDR,localVal);
}


#define LO_CAL_CTRL_HW5_ADDR (RF_BASE + 0x0000014c)

static inline uint32_t lo_cal_ctrl_hw5_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
}

static inline void lo_cal_ctrl_hw5_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW5_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2440_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2440_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW5_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2440_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2440_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW5_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2436_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2436_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW5_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2436_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2436_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW5_ADDR,localVal);
}


#define LO_CAL_CTRL_HW6_ADDR (RF_BASE + 0x00000150)

static inline uint32_t lo_cal_ctrl_hw6_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
}

static inline void lo_cal_ctrl_hw6_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW6_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2448_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2448_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW6_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2448_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2448_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW6_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2444_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2444_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW6_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2444_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2444_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW6_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW6_ADDR,localVal);
}


#define LO_CAL_CTRL_HW7_ADDR (RF_BASE + 0x00000154)

static inline uint32_t lo_cal_ctrl_hw7_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
}

static inline void lo_cal_ctrl_hw7_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW7_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2456_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2456_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW7_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2456_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2456_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW7_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW7_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW7_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW7_ADDR,localVal);
}


#define LO_CAL_CTRL_HW8_ADDR (RF_BASE + 0x00000158)

static inline uint32_t lo_cal_ctrl_hw8_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
}

static inline void lo_cal_ctrl_hw8_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW8_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2464_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2464_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW8_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2464_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2464_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW8_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2460_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2460_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW8_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2460_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2460_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW8_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW8_ADDR,localVal);
}


#define LO_CAL_CTRL_HW9_ADDR (RF_BASE + 0x0000015c)

static inline uint32_t lo_cal_ctrl_hw9_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
}

static inline void lo_cal_ctrl_hw9_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW9_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW9_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW9_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2468_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2468_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW9_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2468_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2468_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW9_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW9_ADDR,localVal);
}


#define LO_CAL_CTRL_HW10_ADDR (RF_BASE + 0x00000160)

static inline uint32_t lo_cal_ctrl_hw10_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
}

static inline void lo_cal_ctrl_hw10_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW10_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2480_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_lo_vco_freq_cw_2480_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(LO_CAL_CTRL_HW10_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2480_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_lo_vco_idac_cw_2480_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(LO_CAL_CTRL_HW10_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_freq_cw_2476_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2476_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW10_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2476_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2476_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW10_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW10_ADDR,localVal);
}


#define LO_CAL_CTRL_HW11_ADDR (RF_BASE + 0x00000164)

static inline uint32_t lo_cal_ctrl_hw11_get(void)
{
    return REG_PL_RD(LO_CAL_CTRL_HW11_ADDR);
}

static inline void lo_cal_ctrl_hw11_set(uint32_t x)
{
    REG_PL_WR(LO_CAL_CTRL_HW11_ADDR,x);
}

static inline uint32_t rf_lo_vco_freq_cw_2484_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW11_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_lo_vco_freq_cw_2484_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW11_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(LO_CAL_CTRL_HW11_ADDR,localVal);
}

static inline uint32_t rf_lo_vco_idac_cw_2484_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW11_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_lo_vco_idac_cw_2484_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_CAL_CTRL_HW11_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(LO_CAL_CTRL_HW11_ADDR,localVal);
}


#define ROSDAC_CTRL_HW1_ADDR (RF_BASE + 0x00000168)

static inline uint32_t rosdac_ctrl_hw1_get(void)
{
    return REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
}

static inline void rosdac_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(ROSDAC_CTRL_HW1_ADDR,x);
}

static inline uint32_t rf_rosdac_q_gc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rosdac_q_gc1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(ROSDAC_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_gc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rosdac_i_gc1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(ROSDAC_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_rosdac_q_gc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rosdac_q_gc0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(ROSDAC_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_gc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rosdac_i_gc0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(ROSDAC_CTRL_HW1_ADDR,localVal);
}


#define ROSDAC_CTRL_HW2_ADDR (RF_BASE + 0x0000016c)

static inline uint32_t rosdac_ctrl_hw2_get(void)
{
    return REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
}

static inline void rosdac_ctrl_hw2_set(uint32_t x)
{
    REG_PL_WR(ROSDAC_CTRL_HW2_ADDR,x);
}

static inline uint32_t rf_rosdac_q_gc3_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_rosdac_q_gc3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(ROSDAC_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_gc3_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_rosdac_i_gc3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(ROSDAC_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_rosdac_q_gc2_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_rosdac_q_gc2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(ROSDAC_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_rosdac_i_gc2_getf(void)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_rosdac_i_gc2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ROSDAC_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(ROSDAC_CTRL_HW2_ADDR,localVal);
}


#define RXIQ_CTRL_HW1_ADDR (RF_BASE + 0x00000170)

static inline uint32_t rxiq_ctrl_hw1_get(void)
{
    return REG_PL_RD(RXIQ_CTRL_HW1_ADDR);
}

static inline void rxiq_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(RXIQ_CTRL_HW1_ADDR,x);
}

static inline uint32_t rf_rx_iq_gain_comp_gc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_iq_gain_comp_gc0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(RXIQ_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_rx_iq_phase_comp_gc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_rx_iq_phase_comp_gc0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RXIQ_CTRL_HW1_ADDR,localVal);
}


#define RXIQ_CTRL_HW2_ADDR (RF_BASE + 0x00000174)

static inline uint32_t rxiq_ctrl_hw2_get(void)
{
    return REG_PL_RD(RXIQ_CTRL_HW2_ADDR);
}

static inline void rxiq_ctrl_hw2_set(uint32_t x)
{
    REG_PL_WR(RXIQ_CTRL_HW2_ADDR,x);
}

static inline uint32_t rf_rx_iq_gain_comp_gc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_iq_gain_comp_gc1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(RXIQ_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_rx_iq_phase_comp_gc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_rx_iq_phase_comp_gc1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RXIQ_CTRL_HW2_ADDR,localVal);
}


#define RXIQ_CTRL_HW3_ADDR (RF_BASE + 0x00000178)

static inline uint32_t rxiq_ctrl_hw3_get(void)
{
    return REG_PL_RD(RXIQ_CTRL_HW3_ADDR);
}

static inline void rxiq_ctrl_hw3_set(uint32_t x)
{
    REG_PL_WR(RXIQ_CTRL_HW3_ADDR,x);
}

static inline uint32_t rf_rx_iq_gain_comp_gc2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_iq_gain_comp_gc2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(RXIQ_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_rx_iq_phase_comp_gc2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_rx_iq_phase_comp_gc2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RXIQ_CTRL_HW3_ADDR,localVal);
}


#define RXIQ_CTRL_HW4_ADDR (RF_BASE + 0x0000017c)

static inline uint32_t rxiq_ctrl_hw4_get(void)
{
    return REG_PL_RD(RXIQ_CTRL_HW4_ADDR);
}

static inline void rxiq_ctrl_hw4_set(uint32_t x)
{
    REG_PL_WR(RXIQ_CTRL_HW4_ADDR,x);
}

static inline uint32_t rf_rx_iq_gain_comp_gc3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_iq_gain_comp_gc3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(RXIQ_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_rx_iq_phase_comp_gc3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_rx_iq_phase_comp_gc3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXIQ_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RXIQ_CTRL_HW4_ADDR,localVal);
}


#define TOSDAC_CTRL_HW1_ADDR (RF_BASE + 0x00000180)

static inline uint32_t tosdac_ctrl_hw1_get(void)
{
    return REG_PL_RD(TOSDAC_CTRL_HW1_ADDR);
}

static inline void tosdac_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(TOSDAC_CTRL_HW1_ADDR,x);
}


#define TOSDAC_CTRL_HW2_ADDR (RF_BASE + 0x00000184)

static inline uint32_t tosdac_ctrl_hw2_get(void)
{
    return REG_PL_RD(TOSDAC_CTRL_HW2_ADDR);
}

static inline void tosdac_ctrl_hw2_set(uint32_t x)
{
    REG_PL_WR(TOSDAC_CTRL_HW2_ADDR,x);
}


#define TOSDAC_CTRL_HW3_ADDR (RF_BASE + 0x00000188)

static inline uint32_t tosdac_ctrl_hw3_get(void)
{
    return REG_PL_RD(TOSDAC_CTRL_HW3_ADDR);
}

static inline void tosdac_ctrl_hw3_set(uint32_t x)
{
    REG_PL_WR(TOSDAC_CTRL_HW3_ADDR,x);
}


#define TOSDAC_CTRL_HW4_ADDR (RF_BASE + 0x0000018c)

static inline uint32_t tosdac_ctrl_hw4_get(void)
{
    return REG_PL_RD(TOSDAC_CTRL_HW4_ADDR);
}

static inline void tosdac_ctrl_hw4_set(uint32_t x)
{
    REG_PL_WR(TOSDAC_CTRL_HW4_ADDR,x);
}


#define TX_IQ_GAIN_HW0_ADDR (RF_BASE + 0x00000190)

static inline uint32_t tx_iq_gain_hw0_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW0_ADDR);
}

static inline void tx_iq_gain_hw0_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW0_ADDR,x);
}


#define TX_IQ_GAIN_HW1_ADDR (RF_BASE + 0x00000194)

static inline uint32_t tx_iq_gain_hw1_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW1_ADDR);
}

static inline void tx_iq_gain_hw1_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW1_ADDR,x);
}


#define TX_IQ_GAIN_HW2_ADDR (RF_BASE + 0x00000198)

static inline uint32_t tx_iq_gain_hw2_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW2_ADDR);
}

static inline void tx_iq_gain_hw2_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW2_ADDR,x);
}


#define TX_IQ_GAIN_HW3_ADDR (RF_BASE + 0x0000019c)

static inline uint32_t tx_iq_gain_hw3_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW3_ADDR);
}

static inline void tx_iq_gain_hw3_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW3_ADDR,x);
}


#define TX_IQ_GAIN_HW4_ADDR (RF_BASE + 0x000001a0)

static inline uint32_t tx_iq_gain_hw4_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW4_ADDR);
}

static inline void tx_iq_gain_hw4_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW4_ADDR,x);
}


#define TX_IQ_GAIN_HW5_ADDR (RF_BASE + 0x000001a4)

static inline uint32_t tx_iq_gain_hw5_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW5_ADDR);
}

static inline void tx_iq_gain_hw5_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW5_ADDR,x);
}


#define TX_IQ_GAIN_HW6_ADDR (RF_BASE + 0x000001a8)

static inline uint32_t tx_iq_gain_hw6_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW6_ADDR);
}

static inline void tx_iq_gain_hw6_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW6_ADDR,x);
}


#define TX_IQ_GAIN_HW7_ADDR (RF_BASE + 0x000001ac)

static inline uint32_t tx_iq_gain_hw7_get(void)
{
    return REG_PL_RD(TX_IQ_GAIN_HW7_ADDR);
}

static inline void tx_iq_gain_hw7_set(uint32_t x)
{
    REG_PL_WR(TX_IQ_GAIN_HW7_ADDR,x);
}


#define LO_SDM_CTRL_HW1_ADDR (RF_BASE + 0x000001b0)

static inline uint32_t lo_sdm_ctrl_hw1_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
}

static inline void lo_sdm_ctrl_hw1_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,x);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2484_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_lo_sdm_dither_sel_wlan_2484_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_lo_sdm_dither_sel_wlan_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2467_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline void rf_lo_sdm_dither_sel_wlan_2467_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00c00000);
    localVal |= (x << 22)&((uint32_t)0x00c00000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2462_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lo_sdm_dither_sel_wlan_2462_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2457_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_lo_sdm_dither_sel_wlan_2457_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_sdm_dither_sel_wlan_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2447_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_lo_sdm_dither_sel_wlan_2447_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2442_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_sdm_dither_sel_wlan_2442_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2437_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_lo_sdm_dither_sel_wlan_2437_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_sdm_dither_sel_wlan_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2427_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lo_sdm_dither_sel_wlan_2427_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2422_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_sdm_dither_sel_wlan_2422_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2417_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_lo_sdm_dither_sel_wlan_2417_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_wlan_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lo_sdm_dither_sel_wlan_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW1_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LO_SDM_CTRL_HW1_ADDR,localVal);
}


#define LO_SDM_CTRL_HW2_ADDR (RF_BASE + 0x000001b4)

static inline uint32_t lo_sdm_ctrl_hw2_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
}

static inline void lo_sdm_ctrl_hw2_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,x);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2432_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_lo_sdm_dither_sel_ble_2432_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2430_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_lo_sdm_dither_sel_ble_2430_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2428_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_lo_sdm_dither_sel_ble_2428_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2426_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_lo_sdm_dither_sel_ble_2426_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2424_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline void rf_lo_sdm_dither_sel_ble_2424_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00c00000);
    localVal |= (x << 22)&((uint32_t)0x00c00000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2422_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lo_sdm_dither_sel_ble_2422_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2420_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_lo_sdm_dither_sel_ble_2420_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2418_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_sdm_dither_sel_ble_2418_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2416_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_lo_sdm_dither_sel_ble_2416_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2414_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_sdm_dither_sel_ble_2414_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2412_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_lo_sdm_dither_sel_ble_2412_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2410_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_sdm_dither_sel_ble_2410_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2408_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lo_sdm_dither_sel_ble_2408_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2406_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_sdm_dither_sel_ble_2406_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2404_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_lo_sdm_dither_sel_ble_2404_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2402_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lo_sdm_dither_sel_ble_2402_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW2_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LO_SDM_CTRL_HW2_ADDR,localVal);
}


#define LO_SDM_CTRL_HW3_ADDR (RF_BASE + 0x000001b8)

static inline uint32_t lo_sdm_ctrl_hw3_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
}

static inline void lo_sdm_ctrl_hw3_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,x);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2464_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_lo_sdm_dither_sel_ble_2464_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2462_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_lo_sdm_dither_sel_ble_2462_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2460_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_lo_sdm_dither_sel_ble_2460_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2458_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_lo_sdm_dither_sel_ble_2458_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2456_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline void rf_lo_sdm_dither_sel_ble_2456_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00c00000);
    localVal |= (x << 22)&((uint32_t)0x00c00000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2454_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void rf_lo_sdm_dither_sel_ble_2454_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2452_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void rf_lo_sdm_dither_sel_ble_2452_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2450_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_sdm_dither_sel_ble_2450_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2448_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_lo_sdm_dither_sel_ble_2448_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2446_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_sdm_dither_sel_ble_2446_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2444_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_lo_sdm_dither_sel_ble_2444_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2442_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_sdm_dither_sel_ble_2442_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2440_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lo_sdm_dither_sel_ble_2440_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2438_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_sdm_dither_sel_ble_2438_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2436_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_lo_sdm_dither_sel_ble_2436_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2434_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lo_sdm_dither_sel_ble_2434_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW3_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LO_SDM_CTRL_HW3_ADDR,localVal);
}


#define LO_SDM_CTRL_HW4_ADDR (RF_BASE + 0x000001bc)

static inline uint32_t lo_sdm_ctrl_hw4_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
}

static inline void lo_sdm_ctrl_hw4_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,x);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_lo_sdm_dither_sel_ble_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2480_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void rf_lo_sdm_dither_sel_ble_2480_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2478_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rf_lo_sdm_dither_sel_ble_2478_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2476_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_lo_sdm_dither_sel_ble_2476_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2474_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_lo_sdm_dither_sel_ble_2474_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2472_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void rf_lo_sdm_dither_sel_ble_2472_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2470_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void rf_lo_sdm_dither_sel_ble_2470_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2468_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void rf_lo_sdm_dither_sel_ble_2468_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}

static inline uint32_t rf_lo_sdm_dither_sel_ble_2466_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_lo_sdm_dither_sel_ble_2466_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW4_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(LO_SDM_CTRL_HW4_ADDR,localVal);
}


#define LO_SDM_CTRL_HW5_ADDR (RF_BASE + 0x000001c0)

static inline uint32_t lo_sdm_ctrl_hw5_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW5_ADDR);
}

static inline void lo_sdm_ctrl_hw5_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW5_ADDR,x);
}

static inline uint32_t rf_lo_sdm_bypass_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_lo_sdm_bypass_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(LO_SDM_CTRL_HW5_ADDR,localVal);
}

static inline uint32_t rf_lo_center_freq_mhz_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW5_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_lo_center_freq_mhz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW5_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(LO_SDM_CTRL_HW5_ADDR,localVal);
}


#define LO_SDM_CTRL_HW6_ADDR (RF_BASE + 0x000001c4)

static inline uint32_t lo_sdm_ctrl_hw6_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW6_ADDR);
}

static inline void lo_sdm_ctrl_hw6_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW6_ADDR,x);
}

static inline uint32_t rf_lo_sdmin_center_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW6_ADDR);
    return ((localVal & ((uint32_t)0x1fffffff)) >> 0);
}

static inline void rf_lo_sdmin_center_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW6_ADDR);
    localVal &= ~((uint32_t)0x1fffffff);
    localVal |= (x << 0)&((uint32_t)0x1fffffff);
    REG_PL_WR(LO_SDM_CTRL_HW6_ADDR,localVal);
}


#define LO_SDM_CTRL_HW7_ADDR (RF_BASE + 0x000001c8)

static inline uint32_t lo_sdm_ctrl_hw7_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW7_ADDR);
}

static inline void lo_sdm_ctrl_hw7_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW7_ADDR,x);
}

static inline uint32_t rf_lo_sdmin_1m_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW7_ADDR);
    return ((localVal & ((uint32_t)0x000fffff)) >> 0);
}

static inline void rf_lo_sdmin_1m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW7_ADDR);
    localVal &= ~((uint32_t)0x000fffff);
    localVal |= (x << 0)&((uint32_t)0x000fffff);
    REG_PL_WR(LO_SDM_CTRL_HW7_ADDR,localVal);
}


#define LO_SDM_CTRL_HW8_ADDR (RF_BASE + 0x000001cc)

static inline uint32_t lo_sdm_ctrl_hw8_get(void)
{
    return REG_PL_RD(LO_SDM_CTRL_HW8_ADDR);
}

static inline void lo_sdm_ctrl_hw8_set(uint32_t x)
{
    REG_PL_WR(LO_SDM_CTRL_HW8_ADDR,x);
}

static inline uint32_t rf_lo_sdmin_if_getf(void)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW8_ADDR);
    return ((localVal & ((uint32_t)0x000fffff)) >> 0);
}

static inline void rf_lo_sdmin_if_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(LO_SDM_CTRL_HW8_ADDR);
    localVal &= ~((uint32_t)0x000fffff);
    localVal |= (x << 0)&((uint32_t)0x000fffff);
    REG_PL_WR(LO_SDM_CTRL_HW8_ADDR,localVal);
}


#define RBB_BW_CTRL_HW_ADDR (RF_BASE + 0x000001d0)

static inline uint32_t rbb_bw_ctrl_hw_get(void)
{
    return REG_PL_RD(RBB_BW_CTRL_HW_ADDR);
}

static inline void rbb_bw_ctrl_hw_set(uint32_t x)
{
    REG_PL_WR(RBB_BW_CTRL_HW_ADDR,x);
}

static inline uint32_t rf_rbb_bt_mode_ble_getf(void)
{
    uint32_t localVal = REG_PL_RD(RBB_BW_CTRL_HW_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_rbb_bt_mode_ble_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RBB_BW_CTRL_HW_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RBB_BW_CTRL_HW_ADDR,localVal);
}


#define RF_TOP_CTRL_ADDR (RF_BASE + 0x00000208)

static inline uint32_t rf_top_ctrl_get(void)
{
    return REG_PL_RD(RF_TOP_CTRL_ADDR);
}

static inline void rf_top_ctrl_set(uint32_t x)
{
    REG_PL_WR(RF_TOP_CTRL_ADDR,x);
}

static inline uint32_t rfckg_clk_phy_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rfckg_clk_phy_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TOP_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_TOP_CTRL_ADDR,localVal);
}


#define SINGEN_CTRL0_ADDR (RF_BASE + 0x0000020c)

static inline uint32_t singen_ctrl0_get(void)
{
    return REG_PL_RD(SINGEN_CTRL0_ADDR);
}

static inline void singen_ctrl0_set(uint32_t x)
{
    REG_PL_WR(SINGEN_CTRL0_ADDR,x);
}

static inline uint32_t rf_singen_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_singen_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(SINGEN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x60000000)) >> 29);
}

static inline void rf_singen_clkdiv_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x60000000);
    localVal |= (x << 29)&((uint32_t)0x60000000);
    REG_PL_WR(SINGEN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_singen_unsign_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_singen_unsign_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(SINGEN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_singen_inc_step0_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void rf_singen_inc_step0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(SINGEN_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_singen_inc_step1_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_singen_inc_step1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(SINGEN_CTRL0_ADDR,localVal);
}


#define SINGEN_CTRL1_ADDR (RF_BASE + 0x00000210)

static inline uint32_t singen_ctrl1_get(void)
{
    return REG_PL_RD(SINGEN_CTRL1_ADDR);
}

static inline void singen_ctrl1_set(uint32_t x)
{
    REG_PL_WR(SINGEN_CTRL1_ADDR,x);
}

static inline uint32_t rf_singen_mode_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_singen_mode_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(SINGEN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void rf_singen_clkdiv_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(SINGEN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_singen_mode_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_singen_mode_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(SINGEN_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_singen_clkdiv_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_singen_clkdiv_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(SINGEN_CTRL1_ADDR,localVal);
}


#define SINGEN_CTRL2_ADDR (RF_BASE + 0x00000214)

static inline uint32_t singen_ctrl2_get(void)
{
    return REG_PL_RD(SINGEN_CTRL2_ADDR);
}

static inline void singen_ctrl2_set(uint32_t x)
{
    REG_PL_WR(SINGEN_CTRL2_ADDR,x);
}

static inline uint32_t rf_singen_start_addr0_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void rf_singen_start_addr0_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22)&((uint32_t)0xffc00000);
    REG_PL_WR(SINGEN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_singen_start_addr1_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x003ff000)) >> 12);
}

static inline void rf_singen_start_addr1_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x003ff000);
    localVal |= (x << 12)&((uint32_t)0x003ff000);
    REG_PL_WR(SINGEN_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_singen_gain_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_singen_gain_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(SINGEN_CTRL2_ADDR,localVal);
}


#define SINGEN_CTRL3_ADDR (RF_BASE + 0x00000218)

static inline uint32_t singen_ctrl3_get(void)
{
    return REG_PL_RD(SINGEN_CTRL3_ADDR);
}

static inline void singen_ctrl3_set(uint32_t x)
{
    REG_PL_WR(SINGEN_CTRL3_ADDR,x);
}

static inline uint32_t rf_singen_start_addr0_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void rf_singen_start_addr0_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22)&((uint32_t)0xffc00000);
    REG_PL_WR(SINGEN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_singen_start_addr1_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x003ff000)) >> 12);
}

static inline void rf_singen_start_addr1_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x003ff000);
    localVal |= (x << 12)&((uint32_t)0x003ff000);
    REG_PL_WR(SINGEN_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_singen_gain_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_singen_gain_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(SINGEN_CTRL3_ADDR,localVal);
}


#define SINGEN_CTRL4_ADDR (RF_BASE + 0x0000021c)

static inline uint32_t singen_ctrl4_get(void)
{
    return REG_PL_RD(SINGEN_CTRL4_ADDR);
}

static inline void singen_ctrl4_set(uint32_t x)
{
    REG_PL_WR(SINGEN_CTRL4_ADDR,x);
}

static inline uint32_t rf_singen_fix_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_singen_fix_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(SINGEN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf_singen_fix_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(SINGEN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_singen_fix_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(SINGEN_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_singen_fix_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_singen_fix_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(SINGEN_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(SINGEN_CTRL4_ADDR,localVal);
}


#define RFIF_DFE_CTRL0_ADDR (RF_BASE + 0x00000220)

static inline uint32_t rfif_dfe_ctrl0_get(void)
{
    return REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
}

static inline void rfif_dfe_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,x);
}

static inline uint32_t rf_test_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_test_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_bbmode_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_bbmode_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_bbmode_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_bbmode_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_wifimode_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_wifimode_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_wifibw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_wifibw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ch_ind_ble_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_ch_ind_ble_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ch_ind_ble_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x003f8000)) >> 15);
}

static inline void rf_ch_ind_ble_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x003f8000);
    localVal |= (x << 15)&((uint32_t)0x003f8000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_pad_dac_clkout_inv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_pad_dac_clkout_inv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_pad_adc_clkout_inv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_pad_adc_clkout_inv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_tx_test_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

static inline void rf_tx_test_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00001800);
    localVal |= (x << 11)&((uint32_t)0x00001800);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_rx_test_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000600)) >> 9);
}

static inline void rf_rx_test_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000600);
    localVal |= (x << 9)&((uint32_t)0x00000600);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_tx_dfe_en_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_tx_dfe_en_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_tx_dfe_en_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_tx_dfe_en_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_rx_dfe_en_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_rx_dfe_en_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_rx_dfe_en_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_rx_dfe_en_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rfckg_dac_afifo_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rfckg_dac_afifo_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rfckg_adc_clkout_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rfckg_adc_clkout_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rfckg_adc_afifo_inv_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rfckg_adc_afifo_inv_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rfckg_txclk_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rfckg_txclk_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}

static inline uint32_t rfckg_rxclk_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rfckg_rxclk_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DFE_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RFIF_DFE_CTRL0_ADDR,localVal);
}


#define RFIF_TEST_READ_ADDR (RF_BASE + 0x00000224)

static inline uint32_t rfif_test_read_get(void)
{
    return REG_PL_RD(RFIF_TEST_READ_ADDR);
}

static inline void rfif_test_read_set(uint32_t x)
{
    REG_PL_WR(RFIF_TEST_READ_ADDR,x);
}

static inline uint32_t rf_test_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_TEST_READ_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RFIF_DIG_CTRL_ADDR (RF_BASE + 0x00000228)

static inline uint32_t rfif_dig_ctrl_get(void)
{
    return REG_PL_RD(RFIF_DIG_CTRL_ADDR);
}

static inline void rfif_dig_ctrl_set(uint32_t x)
{
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,x);
}

static inline uint32_t rfif_ppud_manaual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rfif_ppud_manaual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rfif_ppud_cnt1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x3e000000)) >> 25);
}

static inline void rfif_ppud_cnt1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x3e000000);
    localVal |= (x << 25)&((uint32_t)0x3e000000);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rfif_ppud_cnt2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rfif_ppud_cnt2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16)&((uint32_t)0x01ff0000);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rf_wifimode_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00007ff0)) >> 4);
}

static inline void rf_wifimode_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00007ff0);
    localVal |= (x << 4)&((uint32_t)0x00007ff0);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rfif_int_lo_unlocked_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rfif_int_lo_unlocked_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rfckg_rxclk_div2_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rfckg_rxclk_div2_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rf_test_gc_from_pad_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_test_gc_from_pad_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}

static inline uint32_t rf_test_from_pad_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_test_from_pad_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RFIF_DIG_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RFIF_DIG_CTRL_ADDR,localVal);
}


#define RF_DATA_TEMP_0_ADDR (RF_BASE + 0x0000022c)

static inline uint32_t rf_data_temp_0_get(void)
{
    return REG_PL_RD(RF_DATA_TEMP_0_ADDR);
}

static inline void rf_data_temp_0_set(uint32_t x)
{
    REG_PL_WR(RF_DATA_TEMP_0_ADDR,x);
}

static inline uint32_t rf_data_temp_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_data_temp_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DATA_TEMP_0_ADDR,localVal);
}


#define RF_DATA_TEMP_1_ADDR (RF_BASE + 0x00000230)

static inline uint32_t rf_data_temp_1_get(void)
{
    return REG_PL_RD(RF_DATA_TEMP_1_ADDR);
}

static inline void rf_data_temp_1_set(uint32_t x)
{
    REG_PL_WR(RF_DATA_TEMP_1_ADDR,x);
}

static inline uint32_t rf_data_temp_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_data_temp_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DATA_TEMP_1_ADDR,localVal);
}


#define RF_DATA_TEMP_2_ADDR (RF_BASE + 0x00000234)

static inline uint32_t rf_data_temp_2_get(void)
{
    return REG_PL_RD(RF_DATA_TEMP_2_ADDR);
}

static inline void rf_data_temp_2_set(uint32_t x)
{
    REG_PL_WR(RF_DATA_TEMP_2_ADDR,x);
}

static inline uint32_t rf_data_temp_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_data_temp_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DATA_TEMP_2_ADDR,localVal);
}


#define RF_DATA_TEMP_3_ADDR (RF_BASE + 0x00000238)

static inline uint32_t rf_data_temp_3_get(void)
{
    return REG_PL_RD(RF_DATA_TEMP_3_ADDR);
}

static inline void rf_data_temp_3_set(uint32_t x)
{
    REG_PL_WR(RF_DATA_TEMP_3_ADDR,x);
}

static inline uint32_t rf_data_temp_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_data_temp_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DATA_TEMP_3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DATA_TEMP_3_ADDR,localVal);
}


#define RF_SRAM_CTRL0_ADDR (RF_BASE + 0x0000023c)

static inline uint32_t rf_sram_ctrl0_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL0_ADDR);
}

static inline void rf_sram_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,x);
}

static inline uint32_t rf_sram_sel_dma_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_sram_sel_dma_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_tx_test_2rx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_tx_test_2rx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_tx_test_2rx_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x01800000)) >> 23);
}

static inline void rf_tx_test_2rx_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x01800000);
    localVal |= (x << 23)&((uint32_t)0x01800000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_ext_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00600000)) >> 21);
}

static inline void rf_sram_ext_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00600000);
    localVal |= (x << 21)&((uint32_t)0x00600000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_ext_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_sram_ext_set_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_ext_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_sram_ext_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_swap_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_sram_swap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_link_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void rf_sram_link_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_sram_link_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_sram_link_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_SRAM_CTRL0_ADDR,localVal);
}


#define RF_SRAM_CTRL1_ADDR (RF_BASE + 0x00000240)

static inline uint32_t rf_sram_ctrl1_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL1_ADDR);
}

static inline void rf_sram_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,x);
}

static inline uint32_t rf_sram_adc_done_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_sram_adc_sts_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_sram_adc_sts_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_sram_adc_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_sram_adc_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_sram_adc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_sram_adc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_SRAM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_sram_adc_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RF_SRAM_CTRL2_ADDR (RF_BASE + 0x00000244)

static inline uint32_t rf_sram_ctrl2_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL2_ADDR);
}

static inline void rf_sram_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL2_ADDR,x);
}

static inline uint32_t rf_sram_adc_addr_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void rf_sram_adc_addr_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(RF_SRAM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_sram_adc_addr_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_sram_adc_addr_end_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_SRAM_CTRL2_ADDR,localVal);
}


#define RF_SRAM_CTRL3_ADDR (RF_BASE + 0x00000248)

static inline uint32_t rf_sram_ctrl3_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL3_ADDR);
}

static inline void rf_sram_ctrl3_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL3_ADDR,x);
}

static inline uint32_t rf_sram_adc_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RF_SRAM_CTRL4_ADDR (RF_BASE + 0x0000024c)

static inline uint32_t rf_sram_ctrl4_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL4_ADDR);
}

static inline void rf_sram_ctrl4_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL4_ADDR,x);
}

static inline uint32_t rf_sram_dac_done_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_sram_dac_done_cnt_limit_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0000fff0)) >> 4);
}

static inline void rf_sram_dac_done_cnt_limit_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0000fff0);
    localVal |= (x << 4)&((uint32_t)0x0000fff0);
    REG_PL_WR(RF_SRAM_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_sram_dac_sts_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_sram_dac_sts_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_SRAM_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_sram_dac_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_sram_dac_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_SRAM_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_sram_dac_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_sram_dac_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_SRAM_CTRL4_ADDR,localVal);
}

static inline uint32_t rf_sram_dac_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RF_SRAM_CTRL5_ADDR (RF_BASE + 0x00000250)

static inline uint32_t rf_sram_ctrl5_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL5_ADDR);
}

static inline void rf_sram_ctrl5_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL5_ADDR,x);
}

static inline uint32_t rf_sram_dac_addr_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void rf_sram_dac_addr_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL5_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(RF_SRAM_CTRL5_ADDR,localVal);
}

static inline uint32_t rf_sram_dac_addr_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_sram_dac_addr_end_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_SRAM_CTRL5_ADDR,localVal);
}


#define RF_SRAM_CTRL6_ADDR (RF_BASE + 0x00000254)

static inline uint32_t rf_sram_ctrl6_get(void)
{
    return REG_PL_RD(RF_SRAM_CTRL6_ADDR);
}

static inline void rf_sram_ctrl6_set(uint32_t x)
{
    REG_PL_WR(RF_SRAM_CTRL6_ADDR,x);
}

static inline uint32_t rf_sram_dac_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_SRAM_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RF_ICAL_CTRL0_ADDR (RF_BASE + 0x00000258)

static inline uint32_t rf_ical_ctrl0_get(void)
{
    return REG_PL_RD(RF_ICAL_CTRL0_ADDR);
}

static inline void rf_ical_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,x);
}

static inline uint32_t rf_ical_f_ud_inv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_ical_f_ud_inv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ical_a_ud_inv_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_ical_a_ud_inv_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ical_f_cnt_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_ical_f_cnt_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20)&((uint32_t)0x3ff00000);
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ical_a_cnt_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void rf_ical_a_cnt_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10)&((uint32_t)0x000ffc00);
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_ical_r_cnt_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_ical_r_cnt_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(RF_ICAL_CTRL0_ADDR,localVal);
}


#define RF_ICAL_CTRL1_ADDR (RF_BASE + 0x0000025c)

static inline uint32_t rf_ical_ctrl1_get(void)
{
    return REG_PL_RD(RF_ICAL_CTRL1_ADDR);
}

static inline void rf_ical_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_ICAL_CTRL1_ADDR,x);
}

static inline uint32_t rf_ical_r_os_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xffe00000)) >> 21);
}

static inline uint32_t rf_ical_r_os_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x001ffc00)) >> 10);
}

static inline uint32_t rf_ical_r_avg_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf_ical_r_avg_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(RF_ICAL_CTRL1_ADDR,localVal);
}


#define RF_ICAL_CTRL2_ADDR (RF_BASE + 0x00000260)

static inline uint32_t rf_ical_ctrl2_get(void)
{
    return REG_PL_RD(RF_ICAL_CTRL2_ADDR);
}

static inline void rf_ical_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_ICAL_CTRL2_ADDR,x);
}

static inline uint32_t rf_fsm_ic_wifi_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_fsm_ic_wifi_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_ICAL_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_fsm_ic_bletx_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_fsm_ic_bletx_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF_ICAL_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_fsm_ic_blerx_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_fsm_ic_blerx_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(RF_ICAL_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_ical_period_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_ical_period_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ICAL_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_ICAL_CTRL2_ADDR,localVal);
}


#define RF_FSM_CTRL0_ADDR (RF_BASE + 0x00000264)

static inline uint32_t rf_fsm_ctrl0_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL0_ADDR);
}

static inline void rf_fsm_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL0_ADDR,x);
}

static inline uint32_t rf_ch_ind_wifi_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_ch_ind_wifi_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF_FSM_CTRL0_ADDR,localVal);
}


#define RF_FSM_CTRL1_ADDR (RF_BASE + 0x00000268)

static inline uint32_t rf_fsm_ctrl1_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL1_ADDR);
}

static inline void rf_fsm_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL1_ADDR,x);
}

static inline uint32_t rf_fsm_pu_pa_dly_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fsm_pu_pa_dly_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20)&((uint32_t)0x3ff00000);
    REG_PL_WR(RF_FSM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_fsm_lo_rdy_sbclr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rf_fsm_lo_rdy_sbclr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(RF_FSM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_fsm_lo_rdy_4s_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_fsm_lo_rdy_4s_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(RF_FSM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_fsm_lo_rdy_rst_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_fsm_lo_rdy_rst_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(RF_FSM_CTRL1_ADDR,localVal);
}

static inline uint32_t rf_fsm_lo_rdy_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t rf_fsm_lo_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_fsm_lo_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_FSM_CTRL1_ADDR,localVal);
}


#define RF_FSM_CTRL2_ADDR (RF_BASE + 0x0000026c)

static inline uint32_t rf_fsm_ctrl2_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL2_ADDR);
}

static inline void rf_fsm_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL2_ADDR,x);
}

static inline uint32_t rf_fsm_dfe_rx_dly_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fsm_dfe_rx_dly_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20)&((uint32_t)0x3ff00000);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_fsm_dfe_tx_dly_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void rf_fsm_dfe_tx_dly_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10)&((uint32_t)0x000ffc00);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_trx_m154_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_trx_m154_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_m154_tx_start_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_m154_tx_start_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_m154_rx_start_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_m154_rx_start_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_trx_ble_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_trx_ble_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_trx_sw_ble_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_trx_sw_ble_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_trx_en_ble_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_trx_en_ble_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_dbg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_fsm_st_dbg_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}

static inline uint32_t rf_fsm_st_dbg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_fsm_st_dbg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RF_FSM_CTRL2_ADDR,localVal);
}


#define RF_PKDET_CTRL0_ADDR (RF_BASE + 0x00000270)

static inline uint32_t rf_pkdet_ctrl0_get(void)
{
    return REG_PL_RD(RF_PKDET_CTRL0_ADDR);
}

static inline void rf_pkdet_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_PKDET_CTRL0_ADDR,x);
}

static inline uint32_t rf_pkdet_out_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_PKDET_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_pkdet_out_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_PKDET_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_PKDET_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_pkdet_out_cnt_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_PKDET_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_pkdet_out_cnt_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_PKDET_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_PKDET_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_pkdet_out_cnt_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_PKDET_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}


#define RF_FSM_CTRL3_ADDR (RF_BASE + 0x00000274)

static inline uint32_t rf_fsm_ctrl3_get(void)
{
    return REG_PL_RD(RF_FSM_CTRL3_ADDR);
}

static inline void rf_fsm_ctrl3_set(uint32_t x)
{
    REG_PL_WR(RF_FSM_CTRL3_ADDR,x);
}

static inline uint32_t rf_phy_txmode_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_phy_txmode_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RF_FSM_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_phy_txmode_4s_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rf_phy_txmode_4s_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4)&((uint32_t)0x00000070);
    REG_PL_WR(RF_FSM_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_phy_rxmode_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_phy_rxmode_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_FSM_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_phy_rxmode_4s_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_phy_rxmode_4s_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FSM_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(RF_FSM_CTRL3_ADDR,localVal);
}


#define RF_DMA_CTRL0_ADDR (RF_BASE + 0x00000278)

static inline uint32_t rf_dma_ctrl0_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL0_ADDR);
}

static inline void rf_dma_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL0_ADDR,x);
}

static inline uint32_t rf_dma_adc_done_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_dma_adc_axi_w_idle_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_dma_adc_wcmd_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_dma_adc_wcmd_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RF_DMA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_dma_adc_ostd_lmt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_dma_adc_ostd_lmt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(RF_DMA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_dma_adc_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_dma_adc_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_DMA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_dma_adc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_dma_adc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_DMA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_dma_adc_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RF_DMA_CTRL1_ADDR (RF_BASE + 0x0000027c)

static inline uint32_t rf_dma_ctrl1_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL1_ADDR);
}

static inline void rf_dma_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL1_ADDR,x);
}

static inline uint32_t rf_dma_adc_addr_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_dma_adc_addr_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DMA_CTRL1_ADDR,localVal);
}


#define RF_DMA_CTRL2_ADDR (RF_BASE + 0x00000280)

static inline uint32_t rf_dma_ctrl2_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL2_ADDR);
}

static inline void rf_dma_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL2_ADDR,x);
}

static inline uint32_t rf_dma_adc_addr_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_dma_adc_addr_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DMA_CTRL2_ADDR,localVal);
}


#define RF_DMA_CTRL3_ADDR (RF_BASE + 0x00000284)

static inline uint32_t rf_dma_ctrl3_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL3_ADDR);
}

static inline void rf_dma_ctrl3_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL3_ADDR,x);
}

static inline uint32_t rf_dma_dac_done_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline uint32_t rf_dma_dac_axi_r_idle_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf_dma_dac_rcmd_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_dma_dac_rcmd_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(RF_DMA_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_dma_dac_ostd_lmt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_dma_dac_ostd_lmt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(RF_DMA_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_dma_dac_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_dma_dac_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_DMA_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_dma_dac_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_dma_dac_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_DMA_CTRL3_ADDR,localVal);
}

static inline uint32_t rf_dma_dac_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RF_DMA_CTRL5_ADDR (RF_BASE + 0x00000288)

static inline uint32_t rf_dma_ctrl5_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL5_ADDR);
}

static inline void rf_dma_ctrl5_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL5_ADDR,x);
}

static inline uint32_t rf_dma_dac_addr_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_dma_dac_addr_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DMA_CTRL5_ADDR,localVal);
}


#define RF_DMA_CTRL6_ADDR (RF_BASE + 0x0000028c)

static inline uint32_t rf_dma_ctrl6_get(void)
{
    return REG_PL_RD(RF_DMA_CTRL6_ADDR);
}

static inline void rf_dma_ctrl6_set(uint32_t x)
{
    REG_PL_WR(RF_DMA_CTRL6_ADDR,x);
}

static inline uint32_t rf_dma_dac_addr_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_dma_dac_addr_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_DMA_CTRL6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_DMA_CTRL6_ADDR,localVal);
}


#define RF_ILA_CTRL0_ADDR (RF_BASE + 0x00000290)

static inline uint32_t rf_ila_ctrl0_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL0_ADDR);
}

static inline void rf_ila_ctrl0_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL0_ADDR,x);
}

static inline uint32_t rf_ila_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_cfg_ila_data_src_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rf_cfg_ila_data_src_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28)&((uint32_t)0x70000000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_op_sel3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_cfg_ila_trig_op_sel3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_op_sel2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x03000000)) >> 24);
}

static inline void rf_cfg_ila_trig_op_sel2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x03000000);
    localVal |= (x << 24)&((uint32_t)0x03000000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_op_sel1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00c00000)) >> 22);
}

static inline void rf_cfg_ila_trig_op_sel1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00c00000);
    localVal |= (x << 22)&((uint32_t)0x00c00000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_data_src_to_sram_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_cfg_ila_data_src_to_sram_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_force_stop_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t rf_cfg_ila_trig_src_sel3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rf_cfg_ila_trig_src_sel3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16)&((uint32_t)0x00070000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_src_sel2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_cfg_ila_trig_src_sel2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_src_sel1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_cfg_ila_trig_src_sel1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_trig_ph_op_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_cfg_ila_trig_ph_op_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_deci_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000000e)) >> 1);
}

static inline void rf_cfg_ila_deci_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000000e);
    localVal |= (x << 1)&((uint32_t)0x0000000e);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_cfg_ila_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_ILA_CTRL0_ADDR,localVal);
}


#define RF_ILA_CTRL1_ADDR (RF_BASE + 0x00000294)

static inline uint32_t rf_ila_ctrl1_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL1_ADDR);
}

static inline void rf_ila_ctrl1_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL1_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_mask_sel1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_mask_sel1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL1_ADDR,localVal);
}


#define RF_ILA_CTRL2_ADDR (RF_BASE + 0x00000298)

static inline uint32_t rf_ila_ctrl2_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL2_ADDR);
}

static inline void rf_ila_ctrl2_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL2_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_mask_sel2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_mask_sel2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL2_ADDR,localVal);
}


#define RF_ILA_CTRL3_ADDR (RF_BASE + 0x0000029c)

static inline uint32_t rf_ila_ctrl3_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL3_ADDR);
}

static inline void rf_ila_ctrl3_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL3_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_mask_sel3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_mask_sel3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL3_ADDR,localVal);
}


#define RF_ILA_CTRL4_ADDR (RF_BASE + 0x000002a0)

static inline uint32_t rf_ila_ctrl4_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL4_ADDR);
}

static inline void rf_ila_ctrl4_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL4_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_target_sel1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_target_sel1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL4_ADDR,localVal);
}


#define RF_ILA_CTRL5_ADDR (RF_BASE + 0x000002a4)

static inline uint32_t rf_ila_ctrl5_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL5_ADDR);
}

static inline void rf_ila_ctrl5_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL5_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_target_sel2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_target_sel2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL5_ADDR,localVal);
}


#define RF_ILA_CTRL6_ADDR (RF_BASE + 0x000002a8)

static inline uint32_t rf_ila_ctrl6_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL6_ADDR);
}

static inline void rf_ila_ctrl6_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL6_ADDR,x);
}

static inline uint32_t rf_cfg_ila_trig_target_sel3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_trig_target_sel3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL6_ADDR,localVal);
}


#define RF_ILA_CTRL7_ADDR (RF_BASE + 0x000002ac)

static inline uint32_t rf_ila_ctrl7_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL7_ADDR);
}

static inline void rf_ila_ctrl7_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL7_ADDR,x);
}

static inline uint32_t rf_cfg_ila_end_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_cfg_ila_end_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL7_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_ILA_CTRL7_ADDR,localVal);
}


#define RF_ILA_CTRL8_ADDR (RF_BASE + 0x000002b0)

static inline uint32_t rf_ila_ctrl8_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL8_ADDR);
}

static inline void rf_ila_ctrl8_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL8_ADDR,x);
}

static inline uint32_t rf_sram_adc_addr_keep_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}


#define RF_ILA_CTRL9_ADDR (RF_BASE + 0x000002b4)

static inline uint32_t rf_ila_ctrl9_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL9_ADDR);
}

static inline void rf_ila_ctrl9_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL9_ADDR,x);
}

static inline uint32_t rf_dma_adc_addr_keep_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define RF_ILA_CTRL10_ADDR (RF_BASE + 0x000002b8)

static inline uint32_t rf_ila_ctrl10_get(void)
{
    return REG_PL_RD(RF_ILA_CTRL10_ADDR);
}

static inline void rf_ila_ctrl10_set(uint32_t x)
{
    REG_PL_WR(RF_ILA_CTRL10_ADDR,x);
}

static inline uint32_t rf_cfg_ila_reserved_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0xfffffffe)) >> 1);
}

static inline void rf_cfg_ila_reserved_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL10_ADDR);
    localVal &= ~((uint32_t)0xfffffffe);
    localVal |= (x << 1)&((uint32_t)0xfffffffe);
    REG_PL_WR(RF_ILA_CTRL10_ADDR,localVal);
}

static inline uint32_t rf_cfg_ila_dma_ctrl_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_cfg_ila_dma_ctrl_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_ILA_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_ILA_CTRL10_ADDR,localVal);
}


#define RF_TIMER_CTRL_ADDR (RF_BASE + 0x000002bc)

static inline uint32_t rf_timer_ctrl_get(void)
{
    return REG_PL_RD(RF_TIMER_CTRL_ADDR);
}

static inline void rf_timer_ctrl_set(uint32_t x)
{
    REG_PL_WR(RF_TIMER_CTRL_ADDR,x);
}

static inline uint32_t rf_timer_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TIMER_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_timer_read_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TIMER_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_TIMER_CTRL_ADDR,localVal);
}

static inline uint32_t rf_timer_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TIMER_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_timer_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_TIMER_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_TIMER_CTRL_ADDR,localVal);
}


#define RF_TIMER_VALUE_ADDR (RF_BASE + 0x000002c0)

static inline uint32_t rf_timer_value_get(void)
{
    return REG_PL_RD(RF_TIMER_VALUE_ADDR);
}

static inline void rf_timer_value_set(uint32_t x)
{
    REG_PL_WR(RF_TIMER_VALUE_ADDR,x);
}

static inline uint32_t rf_timer_keep_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_TIMER_VALUE_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define FDIQ_CTRL_0_ADDR (RF_BASE + 0x000002c4)

static inline uint32_t fdiq_ctrl_0_get(void)
{
    return REG_PL_RD(FDIQ_CTRL_0_ADDR);
}

static inline void fdiq_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(FDIQ_CTRL_0_ADDR,x);
}

static inline uint32_t rf_fdiq_acculength_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fdiq_acculength_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20)&((uint32_t)0x3ff00000);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_fdiq_accuoffset_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0003ff00)) >> 8);
}

static inline void rf_fdiq_accuoffset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0003ff00);
    localVal |= (x << 8)&((uint32_t)0x0003ff00);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_fdiq_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_fdiq_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_fdiq_accen_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_fdiq_accen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_fdiq_bypass_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_fdiq_bypass_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_fdiq_rstb_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_fdiq_rstb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(FDIQ_CTRL_0_ADDR,localVal);
}


#define FDIQ_CTRL_1_ADDR (RF_BASE + 0x000002c8)

static inline uint32_t fdiq_ctrl_1_get(void)
{
    return REG_PL_RD(FDIQ_CTRL_1_ADDR);
}

static inline void fdiq_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(FDIQ_CTRL_1_ADDR,x);
}

static inline uint32_t rf_fdiq_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_fdiq_corr_getf(void)
{
    uint32_t localVal = REG_PL_RD(FDIQ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}


#define DFE_CTRL_0_ADDR (RF_BASE + 0x00000600)

static inline uint32_t dfe_ctrl_0_get(void)
{
    return REG_PL_RD(DFE_CTRL_0_ADDR);
}

static inline void dfe_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_0_ADDR,x);
}

static inline uint32_t rf_tx2rx_loop_input_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_tx2rx_loop_input_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx2rx_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_tx2rx_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_iqc_gain_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_tx_iqc_gain_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_iqc_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x007ff000)) >> 12);
}

static inline void rf_tx_iqc_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x007ff000);
    localVal |= (x << 12)&((uint32_t)0x007ff000);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_iqc_phase_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_tx_iqc_phase_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_iqc_phase_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_tx_iqc_phase_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(DFE_CTRL_0_ADDR,localVal);
}


#define DFE_CTRL_1_ADDR (RF_BASE + 0x00000604)

static inline uint32_t dfe_ctrl_1_get(void)
{
    return REG_PL_RD(DFE_CTRL_1_ADDR);
}

static inline void dfe_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_1_ADDR,x);
}

static inline uint32_t rf_tx_dac_iq_swap_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_tx_dac_iq_swap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dac_dat_format_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_tx_dac_dat_format_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dac_dit_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_tx_dac_dit_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dac_os_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf_tx_dac_os_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_rc3_csel_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_tx_rc3_csel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dly_tune_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rf_tx_dly_tune_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dac_os_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_tx_dac_os_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(DFE_CTRL_1_ADDR,localVal);
}


#define DFE_CTRL_2_ADDR (RF_BASE + 0x00000608)

static inline uint32_t dfe_ctrl_2_get(void)
{
    return REG_PL_RD(DFE_CTRL_2_ADDR);
}

static inline void dfe_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_2_ADDR,x);
}

static inline uint32_t rf_rx_adc_iq_swap_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rx_adc_iq_swap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_dat_format_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_rx_adc_dat_format_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_low_pow_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_rx_adc_low_pow_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_dce_flt_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_rx_adc_dce_flt_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_os_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_adc_os_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_os_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_rx_adc_os_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(DFE_CTRL_2_ADDR,localVal);
}


#define DFE_CTRL_3_ADDR (RF_BASE + 0x0000060c)

static inline uint32_t dfe_ctrl_3_get(void)
{
    return REG_PL_RD(DFE_CTRL_3_ADDR);
}

static inline void dfe_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_3_ADDR,x);
}

static inline uint32_t rf_rx_adc_4s_q_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_rx_adc_4s_q_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(DFE_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_4s_q_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_adc_4s_q_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(DFE_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_4s_i_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_rx_adc_4s_i_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(DFE_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_rx_adc_4s_i_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_rx_adc_4s_i_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(DFE_CTRL_3_ADDR,localVal);
}


#define DFE_CTRL_4_ADDR (RF_BASE + 0x00000610)

static inline uint32_t dfe_ctrl_4_get(void)
{
    return REG_PL_RD(DFE_CTRL_4_ADDR);
}

static inline void dfe_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_4_ADDR,x);
}

static inline uint32_t rf_rx_pf_i_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rx_pf_i_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DFE_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_rx_pf_q_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_rx_pf_q_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(DFE_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_rx_pf_th1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_pf_th1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(DFE_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_rx_pf_th2_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_rx_pf_th2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(DFE_CTRL_4_ADDR,localVal);
}


#define DFE_CTRL_5_ADDR (RF_BASE + 0x00000614)

static inline uint32_t dfe_ctrl_5_get(void)
{
    return REG_PL_RD(DFE_CTRL_5_ADDR);
}

static inline void dfe_ctrl_5_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_5_ADDR,x);
}

static inline uint32_t rf_rx_iqc_gain_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_rx_iqc_gain_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(DFE_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_rx_iqc_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x007ff000)) >> 12);
}

static inline void rf_rx_iqc_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x007ff000);
    localVal |= (x << 12)&((uint32_t)0x007ff000);
    REG_PL_WR(DFE_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_rx_iqc_phase_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_rx_iqc_phase_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(DFE_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_rx_iqc_phase_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_rx_iqc_phase_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(DFE_CTRL_5_ADDR,localVal);
}


#define DFE_CTRL_6_ADDR (RF_BASE + 0x00000618)

static inline uint32_t dfe_ctrl_6_get(void)
{
    return REG_PL_RD(DFE_CTRL_6_ADDR);
}

static inline void dfe_ctrl_6_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_6_ADDR,x);
}

static inline uint32_t rf_rx_pm_in_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_rx_pm_in_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(DFE_CTRL_6_ADDR,localVal);
}

static inline uint32_t rf_rx_pm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_rx_pm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(DFE_CTRL_6_ADDR,localVal);
}

static inline uint32_t rf_rx_pm_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t rf_rx_pm_freqshift_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_rx_pm_freqshift_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(DFE_CTRL_6_ADDR,localVal);
}

static inline uint32_t rf_rx_pm_freqshift_cw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0x000fffff)) >> 0);
}

static inline void rf_rx_pm_freqshift_cw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0x000fffff);
    localVal |= (x << 0)&((uint32_t)0x000fffff);
    REG_PL_WR(DFE_CTRL_6_ADDR,localVal);
}


#define DFE_CTRL_7_ADDR (RF_BASE + 0x0000061c)

static inline uint32_t dfe_ctrl_7_get(void)
{
    return REG_PL_RD(DFE_CTRL_7_ADDR);
}

static inline void dfe_ctrl_7_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_7_ADDR,x);
}

static inline uint32_t rf_rx_pm_acc_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_7_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void rf_rx_pm_acc_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_7_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(DFE_CTRL_7_ADDR,localVal);
}

static inline uint32_t rf_rx_pm_start_ofs_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_7_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_rx_pm_start_ofs_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_7_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(DFE_CTRL_7_ADDR,localVal);
}


#define DFE_CTRL_8_ADDR (RF_BASE + 0x00000620)

static inline uint32_t dfe_ctrl_8_get(void)
{
    return REG_PL_RD(DFE_CTRL_8_ADDR);
}

static inline void dfe_ctrl_8_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_8_ADDR,x);
}

static inline uint32_t rf_rx_pm_iqacc_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_8_ADDR);
    return ((localVal & ((uint32_t)0x01ffffff)) >> 0);
}


#define DFE_CTRL_9_ADDR (RF_BASE + 0x00000624)

static inline uint32_t dfe_ctrl_9_get(void)
{
    return REG_PL_RD(DFE_CTRL_9_ADDR);
}

static inline void dfe_ctrl_9_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_9_ADDR,x);
}

static inline uint32_t rf_rx_pm_iqacc_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x01ffffff)) >> 0);
}


#define DFE_CTRL_10_ADDR (RF_BASE + 0x00000628)

static inline uint32_t dfe_ctrl_10_get(void)
{
    return REG_PL_RD(DFE_CTRL_10_ADDR);
}

static inline void dfe_ctrl_10_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_10_ADDR,x);
}

static inline uint32_t rf_dfe_dac_raw_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline uint32_t rf_dfe_dac_raw_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}


#define DFE_CTRL_11_ADDR (RF_BASE + 0x0000062c)

static inline uint32_t dfe_ctrl_11_get(void)
{
    return REG_PL_RD(DFE_CTRL_11_ADDR);
}

static inline void dfe_ctrl_11_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_11_ADDR,x);
}

static inline uint32_t rf_dfe_adc_raw_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_11_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline uint32_t rf_dfe_adc_raw_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_11_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}


#define DFE_CTRL_12_ADDR (RF_BASE + 0x00000630)

static inline uint32_t dfe_ctrl_12_get(void)
{
    return REG_PL_RD(DFE_CTRL_12_ADDR);
}

static inline void dfe_ctrl_12_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_12_ADDR,x);
}

static inline uint32_t rf_tx_cfr_en_ofdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_tx_cfr_en_ofdm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(DFE_CTRL_12_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_tx_cfr_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(DFE_CTRL_12_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_tx_cfr_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(DFE_CTRL_12_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_tx_cfr_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(DFE_CTRL_12_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_tx_cfr_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_12_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(DFE_CTRL_12_ADDR,localVal);
}


#define DFE_CTRL_13_ADDR (RF_BASE + 0x00000634)

static inline uint32_t dfe_ctrl_13_get(void)
{
    return REG_PL_RD(DFE_CTRL_13_ADDR);
}

static inline void dfe_ctrl_13_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_13_ADDR,x);
}

static inline uint32_t rf_tx_cfr_th_mcs0_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_tx_cfr_th_mcs0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(DFE_CTRL_13_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rf_tx_cfr_th_mcs1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(DFE_CTRL_13_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs2_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_tx_cfr_th_mcs2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(DFE_CTRL_13_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs3_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_tx_cfr_th_mcs3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_13_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(DFE_CTRL_13_ADDR,localVal);
}


#define DFE_CTRL_14_ADDR (RF_BASE + 0x00000638)

static inline uint32_t dfe_ctrl_14_get(void)
{
    return REG_PL_RD(DFE_CTRL_14_ADDR);
}

static inline void dfe_ctrl_14_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_14_ADDR,x);
}

static inline uint32_t rf_tx_cfr_th_mcs4_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_tx_cfr_th_mcs4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(DFE_CTRL_14_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs5_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rf_tx_cfr_th_mcs5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(DFE_CTRL_14_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs6_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_tx_cfr_th_mcs6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(DFE_CTRL_14_ADDR,localVal);
}

static inline uint32_t rf_tx_cfr_th_mcs7_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_tx_cfr_th_mcs7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_14_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(DFE_CTRL_14_ADDR,localVal);
}


#define DFE_CTRL_15_ADDR (RF_BASE + 0x0000063c)

static inline uint32_t dfe_ctrl_15_get(void)
{
    return REG_PL_RD(DFE_CTRL_15_ADDR);
}

static inline void dfe_ctrl_15_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_15_ADDR,x);
}

static inline uint32_t rf_tx_dvga_2_qdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf_tx_dvga_2_qdb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(DFE_CTRL_15_ADDR,localVal);
}

static inline uint32_t rf_tx_dvga_1_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_tx_dvga_1_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(DFE_CTRL_15_ADDR,localVal);
}

static inline uint32_t rf_tx_dvga_1_qdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf_tx_dvga_1_qdb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(DFE_CTRL_15_ADDR,localVal);
}

static inline uint32_t rf_tx_dvga_0_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_tx_dvga_0_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(DFE_CTRL_15_ADDR,localVal);
}

static inline uint32_t rf_tx_dvga_0_qdb_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf_tx_dvga_0_qdb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_15_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(DFE_CTRL_15_ADDR,localVal);
}


#define DFE_CTRL_16_ADDR (RF_BASE + 0x00000640)

static inline uint32_t dfe_ctrl_16_get(void)
{
    return REG_PL_RD(DFE_CTRL_16_ADDR);
}

static inline void dfe_ctrl_16_set(uint32_t x)
{
    REG_PL_WR(DFE_CTRL_16_ADDR,x);
}

static inline uint32_t rf_tx_dfe_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_16_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_tx_dfe_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_CTRL_16_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0)&((uint32_t)0x000001ff);
    REG_PL_WR(DFE_CTRL_16_ADDR,localVal);
}


#define DFE_RX_NOTCH_0_ADDR (RF_BASE + 0x00000680)

static inline uint32_t dfe_rx_notch_0_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
}

static inline void dfe_rx_notch_0_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,x);
}

static inline uint32_t rf_rx_notch1_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rx_notch1_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}

static inline uint32_t rf_rx_notch1_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x78000000)) >> 27);
}

static inline void rf_rx_notch1_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x78000000);
    localVal |= (x << 27)&((uint32_t)0x78000000);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}

static inline uint32_t rf_rx_notch1_fnrm_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline void rf_rx_notch1_fnrm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x07ff0000);
    localVal |= (x << 16)&((uint32_t)0x07ff0000);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}

static inline uint32_t rf_rx_notch2_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_rx_notch2_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}

static inline uint32_t rf_rx_notch2_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x00007800)) >> 11);
}

static inline void rf_rx_notch2_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x00007800);
    localVal |= (x << 11)&((uint32_t)0x00007800);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}

static inline uint32_t rf_rx_notch2_fnrm_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_rx_notch2_fnrm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_0_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(DFE_RX_NOTCH_0_ADDR,localVal);
}


#define DFE_RX_NOTCH_1_ADDR (RF_BASE + 0x00000684)

static inline uint32_t dfe_rx_notch_1_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
}

static inline void dfe_rx_notch_1_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,x);
}

static inline uint32_t rf_rx_notch_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_rx_notch_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf_rx_notch_tar_ch_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf_rx_notch_tar_ch_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf_rx_notch_tar_ch_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf_rx_notch_tar_ch_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_1_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(DFE_RX_NOTCH_1_ADDR,localVal);
}


#define DFE_RX_NOTCH_2_ADDR (RF_BASE + 0x00000688)

static inline uint32_t dfe_rx_notch_2_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
}

static inline void dfe_rx_notch_2_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_2_ADDR,x);
}

static inline uint32_t rf_rx_notch_tar_ch_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf_rx_notch_tar_ch_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(DFE_RX_NOTCH_2_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf_rx_notch_tar_ch_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(DFE_RX_NOTCH_2_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf_rx_notch_tar_ch_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(DFE_RX_NOTCH_2_ADDR,localVal);
}

static inline uint32_t rf_rx_notch_tar_ch_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf_rx_notch_tar_ch_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_2_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(DFE_RX_NOTCH_2_ADDR,localVal);
}


#define DFE_RX_NOTCH_3_ADDR (RF_BASE + 0x0000068c)

static inline uint32_t dfe_rx_notch_3_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_3_ADDR);
}

static inline void dfe_rx_notch_3_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_3_ADDR,x);
}

static inline uint32_t rf_rx_notch_tar_ch_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_3_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf_rx_notch_tar_ch_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_3_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(DFE_RX_NOTCH_3_ADDR,localVal);
}


#define DFE_RX_NOTCH_4_ADDR (RF_BASE + 0x00000690)

static inline uint32_t dfe_rx_notch_4_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_4_ADDR);
}

static inline void dfe_rx_notch_4_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_4_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_4_ADDR,localVal);
}


#define DFE_RX_NOTCH_5_ADDR (RF_BASE + 0x00000694)

static inline uint32_t dfe_rx_notch_5_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_5_ADDR);
}

static inline void dfe_rx_notch_5_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_5_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_5_ADDR,localVal);
}


#define DFE_RX_NOTCH_6_ADDR (RF_BASE + 0x00000698)

static inline uint32_t dfe_rx_notch_6_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_6_ADDR);
}

static inline void dfe_rx_notch_6_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_6_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_6_ADDR,localVal);
}


#define DFE_RX_NOTCH_7_ADDR (RF_BASE + 0x0000069c)

static inline uint32_t dfe_rx_notch_7_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_7_ADDR);
}

static inline void dfe_rx_notch_7_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_7_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_7_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_7_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_7_ADDR,localVal);
}


#define DFE_RX_NOTCH_8_ADDR (RF_BASE + 0x000006a0)

static inline uint32_t dfe_rx_notch_8_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_8_ADDR);
}

static inline void dfe_rx_notch_8_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_8_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_8_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_8_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_8_ADDR,localVal);
}


#define DFE_RX_NOTCH_9_ADDR (RF_BASE + 0x000006a4)

static inline uint32_t dfe_rx_notch_9_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_9_ADDR);
}

static inline void dfe_rx_notch_9_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_9_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_9_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_9_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_9_ADDR,localVal);
}


#define DFE_RX_NOTCH_10_ADDR (RF_BASE + 0x000006a8)

static inline uint32_t dfe_rx_notch_10_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_10_ADDR);
}

static inline void dfe_rx_notch_10_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_10_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_10_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_10_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_10_ADDR,localVal);
}


#define DFE_RX_NOTCH_11_ADDR (RF_BASE + 0x000006ac)

static inline uint32_t dfe_rx_notch_11_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_11_ADDR);
}

static inline void dfe_rx_notch_11_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_11_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_11_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_11_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_11_ADDR,localVal);
}


#define DFE_RX_NOTCH_12_ADDR (RF_BASE + 0x000006b0)

static inline uint32_t dfe_rx_notch_12_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_12_ADDR);
}

static inline void dfe_rx_notch_12_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_12_ADDR,x);
}

static inline uint32_t rf_rx_notch_para_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_12_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_rx_notch_para_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_12_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(DFE_RX_NOTCH_12_ADDR,localVal);
}


#define DFE_RX_NOTCH_13_ADDR (RF_BASE + 0x000006b4)

static inline uint32_t dfe_rx_notch_13_get(void)
{
    return REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
}

static inline void dfe_rx_notch_13_set(uint32_t x)
{
    REG_PL_WR(DFE_RX_NOTCH_13_ADDR,x);
}

static inline uint32_t rf_rx_notch1_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t rf_rx_notch1_alpha_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x78000000)) >> 27);
}

static inline uint32_t rf_rx_notch1_fnrm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x07ff0000)) >> 16);
}

static inline uint32_t rf_rx_notch2_en_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t rf_rx_notch2_alpha_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x00007800)) >> 11);
}

static inline uint32_t rf_rx_notch2_fnrm_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(DFE_RX_NOTCH_13_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}


#define PWR_WIFI_CTRL_0_ADDR (RF_BASE + 0x00000700)

static inline uint32_t pwr_wifi_ctrl_0_get(void)
{
    return REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
}

static inline void pwr_wifi_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,x);
}

static inline uint32_t rf_m154_tx_pwr_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_m154_tx_pwr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_wifi_nominal_pwr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_wifi_nominal_pwr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_wifi_nominal_pwr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_wifi_nominal_pwr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_WIFI_CTRL_0_ADDR,localVal);
}


#define PWR_WIFI_CTRL_1_ADDR (RF_BASE + 0x00000704)

static inline uint32_t pwr_wifi_ctrl_1_get(void)
{
    return REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
}

static inline void pwr_wifi_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,x);
}

static inline uint32_t rf_wifi_tx_corr_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_wifi_tx_corr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_wifi_nominal_pwr_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_wifi_nominal_pwr_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_wifi_nominal_pwr_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_wifi_nominal_pwr_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_WIFI_CTRL_1_ADDR,localVal);
}


#define PWR_WIFI_CTRL_2_ADDR (RF_BASE + 0x00000708)

static inline uint32_t pwr_wifi_ctrl_2_get(void)
{
    return REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
}

static inline void pwr_wifi_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,x);
}

static inline uint32_t rf_wifi_nominal_pwr_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_wifi_nominal_pwr_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_wifi_nominal_pwr_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_wifi_nominal_pwr_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_wifi_nominal_pwr_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_nominal_pwr_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_wifi_nominal_pwr_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_WIFI_CTRL_2_ADDR,localVal);
}


#define PWR_WIFI_TBL_0_ADDR (RF_BASE + 0x0000070c)

static inline uint32_t pwr_wifi_tbl_0_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
}

static inline void pwr_wifi_tbl_0_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_0_ADDR,localVal);
}


#define PWR_WIFI_TBL_1_ADDR (RF_BASE + 0x00000710)

static inline uint32_t pwr_wifi_tbl_1_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
}

static inline void pwr_wifi_tbl_1_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_1_ADDR,localVal);
}


#define PWR_WIFI_TBL_2_ADDR (RF_BASE + 0x00000714)

static inline uint32_t pwr_wifi_tbl_2_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
}

static inline void pwr_wifi_tbl_2_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_2_ADDR,localVal);
}


#define PWR_WIFI_TBL_3_ADDR (RF_BASE + 0x00000718)

static inline uint32_t pwr_wifi_tbl_3_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
}

static inline void pwr_wifi_tbl_3_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_3_ADDR,localVal);
}


#define PWR_WIFI_TBL_4_ADDR (RF_BASE + 0x0000071c)

static inline uint32_t pwr_wifi_tbl_4_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
}

static inline void pwr_wifi_tbl_4_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_4_ADDR,localVal);
}


#define PWR_WIFI_TBL_5_ADDR (RF_BASE + 0x00000720)

static inline uint32_t pwr_wifi_tbl_5_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
}

static inline void pwr_wifi_tbl_5_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_5_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_5_ADDR,localVal);
}


#define PWR_WIFI_TBL_6_ADDR (RF_BASE + 0x00000724)

static inline uint32_t pwr_wifi_tbl_6_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
}

static inline void pwr_wifi_tbl_6_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_6_ADDR,localVal);
}


#define PWR_WIFI_TBL_7_ADDR (RF_BASE + 0x00000728)

static inline uint32_t pwr_wifi_tbl_7_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
}

static inline void pwr_wifi_tbl_7_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_7_ADDR,localVal);
}


#define PWR_WIFI_TBL_8_ADDR (RF_BASE + 0x0000072c)

static inline uint32_t pwr_wifi_tbl_8_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
}

static inline void pwr_wifi_tbl_8_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_8_ADDR,localVal);
}


#define PWR_WIFI_TBL_9_ADDR (RF_BASE + 0x00000730)

static inline uint32_t pwr_wifi_tbl_9_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
}

static inline void pwr_wifi_tbl_9_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_9_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_9_ADDR,localVal);
}


#define PWR_WIFI_TBL_10_ADDR (RF_BASE + 0x00000734)

static inline uint32_t pwr_wifi_tbl_10_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
}

static inline void pwr_wifi_tbl_10_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_10_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_10_ADDR,localVal);
}


#define PWR_WIFI_TBL_11_ADDR (RF_BASE + 0x00000738)

static inline uint32_t pwr_wifi_tbl_11_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
}

static inline void pwr_wifi_tbl_11_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_11_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_11_ADDR,localVal);
}


#define PWR_WIFI_TBL_12_ADDR (RF_BASE + 0x0000073c)

static inline uint32_t pwr_wifi_tbl_12_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
}

static inline void pwr_wifi_tbl_12_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_12_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_12_ADDR,localVal);
}


#define PWR_WIFI_TBL_13_ADDR (RF_BASE + 0x00000740)

static inline uint32_t pwr_wifi_tbl_13_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
}

static inline void pwr_wifi_tbl_13_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_13_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_13_ADDR,localVal);
}


#define PWR_WIFI_TBL_14_ADDR (RF_BASE + 0x00000744)

static inline uint32_t pwr_wifi_tbl_14_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
}

static inline void pwr_wifi_tbl_14_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_14_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_14_ADDR,localVal);
}


#define PWR_WIFI_TBL_15_ADDR (RF_BASE + 0x00000748)

static inline uint32_t pwr_wifi_tbl_15_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
}

static inline void pwr_wifi_tbl_15_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_15_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_15_ADDR,localVal);
}


#define PWR_WIFI_TBL_16_ADDR (RF_BASE + 0x0000074c)

static inline uint32_t pwr_wifi_tbl_16_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
}

static inline void pwr_wifi_tbl_16_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_16_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_16_ADDR,localVal);
}


#define PWR_WIFI_TBL_17_ADDR (RF_BASE + 0x00000750)

static inline uint32_t pwr_wifi_tbl_17_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
}

static inline void pwr_wifi_tbl_17_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_17_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_17_ADDR,localVal);
}


#define PWR_WIFI_TBL_18_ADDR (RF_BASE + 0x00000754)

static inline uint32_t pwr_wifi_tbl_18_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
}

static inline void pwr_wifi_tbl_18_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_18_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_18_ADDR,localVal);
}


#define PWR_WIFI_TBL_19_ADDR (RF_BASE + 0x00000758)

static inline uint32_t pwr_wifi_tbl_19_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
}

static inline void pwr_wifi_tbl_19_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_19_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_19_ADDR,localVal);
}


#define PWR_WIFI_TBL_20_ADDR (RF_BASE + 0x0000075c)

static inline uint32_t pwr_wifi_tbl_20_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
}

static inline void pwr_wifi_tbl_20_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_20_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_20_ADDR,localVal);
}


#define PWR_WIFI_TBL_21_ADDR (RF_BASE + 0x00000760)

static inline uint32_t pwr_wifi_tbl_21_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
}

static inline void pwr_wifi_tbl_21_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_21_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_21_ADDR,localVal);
}


#define PWR_WIFI_TBL_22_ADDR (RF_BASE + 0x00000764)

static inline uint32_t pwr_wifi_tbl_22_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
}

static inline void pwr_wifi_tbl_22_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_22_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_22_ADDR,localVal);
}


#define PWR_WIFI_TBL_23_ADDR (RF_BASE + 0x00000768)

static inline uint32_t pwr_wifi_tbl_23_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
}

static inline void pwr_wifi_tbl_23_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_23_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_23_ADDR,localVal);
}


#define PWR_WIFI_TBL_24_ADDR (RF_BASE + 0x0000076c)

static inline uint32_t pwr_wifi_tbl_24_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
}

static inline void pwr_wifi_tbl_24_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,x);
}

static inline uint32_t rf_wifi_ax_submode_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rf_wifi_ax_submode_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30)&((uint32_t)0xc0000000);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_dac_bias_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_wifi_dac_bias_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_boost_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_wifi_gc_tbb_boost_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tbb_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_wifi_gc_tbb_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_tmx_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_wifi_gc_tmx_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_dg1_qdb_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_wifi_dg1_qdb_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_gain_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_wifi_iqc_gain_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_24_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_WIFI_TBL_24_ADDR,localVal);
}


#define PWR_WIFI_TBL_25_ADDR (RF_BASE + 0x00000770)

static inline uint32_t pwr_wifi_tbl_25_get(void)
{
    return REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
}

static inline void pwr_wifi_tbl_25_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,x);
}

static inline uint32_t rf_wifi_dpd_en_ax_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_wifi_dpd_en_ax_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_n_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf_wifi_dpd_en_n_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_g_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf_wifi_dpd_en_g_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_en_b_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_wifi_dpd_en_b_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_lp_en_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_wifi_pa_lp_en_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_gc_pa_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_wifi_gc_pa_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_iqc_phase_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_wifi_iqc_phase_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_i_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_wifi_tosdac_i_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_tosdac_q_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_wifi_tosdac_q_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}

static inline uint32_t rf_wifi_dpd_sel_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_wifi_dpd_sel_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_TBL_25_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_WIFI_TBL_25_ADDR,localVal);
}


#define PWR_BZ_CTRL_0_ADDR (RF_BASE + 0x00000774)

static inline uint32_t pwr_bz_ctrl_0_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
}

static inline void pwr_bz_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,x);
}

static inline uint32_t rf_bz_tx_backoff_07_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_bz_tx_backoff_07_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_06_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_bz_tx_backoff_06_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_05_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_bz_tx_backoff_05_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_04_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_bz_tx_backoff_04_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_bz_tx_backoff_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_bz_tx_backoff_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_bz_tx_backoff_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_bz_tx_backoff_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PWR_BZ_CTRL_0_ADDR,localVal);
}


#define PWR_BZ_CTRL_1_ADDR (RF_BASE + 0x00000778)

static inline uint32_t pwr_bz_ctrl_1_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
}

static inline void pwr_bz_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,x);
}

static inline uint32_t rf_bz_tx_backoff_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_bz_tx_backoff_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_bz_tx_backoff_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rf_bz_tx_backoff_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_bz_tx_backoff_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_bz_tx_backoff_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_bz_tx_backoff_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_09_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_bz_tx_backoff_09_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_08_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_bz_tx_backoff_08_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PWR_BZ_CTRL_1_ADDR,localVal);
}


#define PWR_BZ_CTRL_2_ADDR (RF_BASE + 0x0000077c)

static inline uint32_t pwr_bz_ctrl_2_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
}

static inline void pwr_bz_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,x);
}

static inline uint32_t rf_bz_tx_backoff_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_bz_tx_backoff_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_bz_tx_backoff_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_bz_tx_backoff_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rf_bz_tx_backoff_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_backoff_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf_bz_tx_backoff_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PWR_BZ_CTRL_2_ADDR,localVal);
}


#define PWR_BZ_CTRL_3_ADDR (RF_BASE + 0x00000780)

static inline uint32_t pwr_bz_ctrl_3_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
}

static inline void pwr_bz_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,x);
}

static inline uint32_t rf_bz_tx_corr_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bz_tx_corr_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_corr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_bz_tx_corr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_corr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_bz_tx_corr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_corr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_bz_tx_corr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_corr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bz_tx_corr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_BZ_CTRL_3_ADDR,localVal);
}


#define PWR_BZ_CTRL_4_ADDR (RF_BASE + 0x00000784)

static inline uint32_t pwr_bz_ctrl_4_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
}

static inline void pwr_bz_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,x);
}

static inline uint32_t rf_bz_tx_temp_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bz_tx_temp_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_temp_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_bz_tx_temp_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_temp_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_bz_tx_temp_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_temp_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_bz_tx_temp_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_tx_temp_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bz_tx_temp_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_BZ_CTRL_4_ADDR,localVal);
}


#define PWR_BZ_CTRL_5_ADDR (RF_BASE + 0x00000788)

static inline uint32_t pwr_bz_ctrl_5_get(void)
{
    return REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
}

static inline void pwr_bz_ctrl_5_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_CTRL_5_ADDR,x);
}

static inline uint32_t rf_bz_nominal_pwr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_bz_nominal_pwr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_BZ_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_bz_nominal_pwr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_BZ_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_bz_nominal_pwr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_BZ_CTRL_5_ADDR,localVal);
}

static inline uint32_t rf_bz_nominal_pwr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bz_nominal_pwr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_BZ_CTRL_5_ADDR,localVal);
}


#define PWR_BZ_TBL_0_ADDR (RF_BASE + 0x0000078c)

static inline uint32_t pwr_bz_tbl_0_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_0_ADDR);
}

static inline void pwr_bz_tbl_0_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,x);
}

static inline uint32_t rf_bz_dac_bias_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_bz_dac_bias_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_boost_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_bz_gc_tbb_boost_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_bz_gc_tbb_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tmx_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_bz_gc_tmx_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_dg1_qdb_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_bz_dg1_qdb_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_gain_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_bz_iqc_gain_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_0_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_BZ_TBL_0_ADDR,localVal);
}


#define PWR_BZ_TBL_1_ADDR (RF_BASE + 0x00000790)

static inline uint32_t pwr_bz_tbl_1_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_1_ADDR);
}

static inline void pwr_bz_tbl_1_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,x);
}

static inline uint32_t rf_bz_dpd_en_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_bz_dpd_en_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_lp_en_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_bz_pa_lp_en_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_pa_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_bz_gc_pa_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_phase_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_bz_iqc_phase_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_i_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_tosdac_i_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_q_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_bz_tosdac_q_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}

static inline uint32_t rf_bz_dpd_sel_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_bz_dpd_sel_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_1_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_BZ_TBL_1_ADDR,localVal);
}


#define PWR_BZ_TBL_2_ADDR (RF_BASE + 0x00000794)

static inline uint32_t pwr_bz_tbl_2_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_2_ADDR);
}

static inline void pwr_bz_tbl_2_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,x);
}

static inline uint32_t rf_bz_dac_bias_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_bz_dac_bias_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_boost_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_bz_gc_tbb_boost_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_bz_gc_tbb_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tmx_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_bz_gc_tmx_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_dg1_qdb_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_bz_dg1_qdb_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_gain_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_bz_iqc_gain_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_2_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_BZ_TBL_2_ADDR,localVal);
}


#define PWR_BZ_TBL_3_ADDR (RF_BASE + 0x00000798)

static inline uint32_t pwr_bz_tbl_3_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_3_ADDR);
}

static inline void pwr_bz_tbl_3_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,x);
}

static inline uint32_t rf_bz_dpd_en_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_bz_dpd_en_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_lp_en_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_bz_pa_lp_en_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_pa_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_bz_gc_pa_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_phase_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_bz_iqc_phase_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_i_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_tosdac_i_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_q_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_bz_tosdac_q_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}

static inline uint32_t rf_bz_dpd_sel_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_bz_dpd_sel_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_3_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_BZ_TBL_3_ADDR,localVal);
}


#define PWR_BZ_TBL_4_ADDR (RF_BASE + 0x0000079c)

static inline uint32_t pwr_bz_tbl_4_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_4_ADDR);
}

static inline void pwr_bz_tbl_4_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,x);
}

static inline uint32_t rf_bz_dac_bias_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_bz_dac_bias_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_boost_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_bz_gc_tbb_boost_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_bz_gc_tbb_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tmx_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_bz_gc_tmx_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_dg1_qdb_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_bz_dg1_qdb_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_gain_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_bz_iqc_gain_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_4_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_BZ_TBL_4_ADDR,localVal);
}


#define PWR_BZ_TBL_6_ADDR (RF_BASE + 0x000007a0)

static inline uint32_t pwr_bz_tbl_6_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_6_ADDR);
}

static inline void pwr_bz_tbl_6_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,x);
}

static inline uint32_t rf_bz_dpd_en_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_bz_dpd_en_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_lp_en_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_bz_pa_lp_en_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_pa_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_bz_gc_pa_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_phase_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_bz_iqc_phase_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_i_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_tosdac_i_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_q_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_bz_tosdac_q_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}

static inline uint32_t rf_bz_dpd_sel_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_bz_dpd_sel_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_6_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_BZ_TBL_6_ADDR,localVal);
}


#define PWR_BZ_TBL_7_ADDR (RF_BASE + 0x000007a4)

static inline uint32_t pwr_bz_tbl_7_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_7_ADDR);
}

static inline void pwr_bz_tbl_7_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,x);
}

static inline uint32_t rf_bz_dac_bias_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rf_bz_dac_bias_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_boost_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x0c000000)) >> 26);
}

static inline void rf_bz_gc_tbb_boost_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x0c000000);
    localVal |= (x << 26)&((uint32_t)0x0c000000);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tbb_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf_bz_gc_tbb_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_tmx_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_bz_gc_tmx_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_bz_dg1_qdb_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x0003f800)) >> 11);
}

static inline void rf_bz_dg1_qdb_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x0003f800);
    localVal |= (x << 11)&((uint32_t)0x0003f800);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_gain_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rf_bz_iqc_gain_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_7_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(PWR_BZ_TBL_7_ADDR,localVal);
}


#define PWR_BZ_TBL_8_ADDR (RF_BASE + 0x000007a8)

static inline uint32_t pwr_bz_tbl_8_get(void)
{
    return REG_PL_RD(PWR_BZ_TBL_8_ADDR);
}

static inline void pwr_bz_tbl_8_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,x);
}

static inline uint32_t rf_bz_dpd_en_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_bz_dpd_en_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_lp_en_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_bz_pa_lp_en_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_gc_pa_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rf_bz_gc_pa_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_iqc_phase_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x00ffc000)) >> 14);
}

static inline void rf_bz_iqc_phase_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x00ffc000);
    localVal |= (x << 14)&((uint32_t)0x00ffc000);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_i_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rf_bz_tosdac_i_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_tosdac_q_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rf_bz_tosdac_q_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}

static inline uint32_t rf_bz_dpd_sel_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rf_bz_dpd_sel_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_TBL_8_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0)&((uint32_t)0x00000003);
    REG_PL_WR(PWR_BZ_TBL_8_ADDR,localVal);
}


#define PWR_CTRL_0_ADDR (RF_BASE + 0x000007ac)

static inline uint32_t pwr_ctrl_0_get(void)
{
    return REG_PL_RD(PWR_CTRL_0_ADDR);
}

static inline void pwr_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(PWR_CTRL_0_ADDR,x);
}

static inline uint32_t rf_tx_reduce_pwr_ena_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_tx_reduce_pwr_ena_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PWR_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_pwr_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0fffff00)) >> 8);
}

static inline uint32_t rf_wifi_tx_corr_dsss_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_wifi_tx_corr_dsss_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PWR_CTRL_0_ADDR,localVal);
}


#define PWR_CTRL_1_ADDR (RF_BASE + 0x000007b0)

static inline uint32_t pwr_ctrl_1_get(void)
{
    return REG_PL_RD(PWR_CTRL_1_ADDR);
}

static inline void pwr_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(PWR_CTRL_1_ADDR,x);
}

static inline uint32_t rf_tx_reduce_pwr_ble_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_tx_reduce_pwr_ble_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PWR_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_reduce_pwr_zb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rf_tx_reduce_pwr_zb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PWR_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_reduce_pwr_bt_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rf_tx_reduce_pwr_bt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PWR_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_reduce_pwr_wifi_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_tx_reduce_pwr_wifi_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PWR_CTRL_1_ADDR,localVal);
}


#define PWR_CTRL_3_ADDR (RF_BASE + 0x000007b4)

static inline uint32_t pwr_ctrl_3_get(void)
{
    return REG_PL_RD(PWR_CTRL_3_ADDR);
}

static inline void pwr_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(PWR_CTRL_3_ADDR,x);
}

static inline uint32_t rf_bt_tx_corr_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bt_tx_corr_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PWR_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_corr_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_bt_tx_corr_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_corr_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_bt_tx_corr_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_corr_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_bt_tx_corr_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_CTRL_3_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_corr_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bt_tx_corr_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_CTRL_3_ADDR,localVal);
}


#define PWR_CTRL_4_ADDR (RF_BASE + 0x000007b8)

static inline uint32_t pwr_ctrl_4_get(void)
{
    return REG_PL_RD(PWR_CTRL_4_ADDR);
}

static inline void pwr_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(PWR_CTRL_4_ADDR,x);
}

static inline uint32_t rf_bt_tx_temp_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_bt_tx_temp_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PWR_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_temp_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_bt_tx_temp_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18)&((uint32_t)0x00fc0000);
    REG_PL_WR(PWR_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_temp_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_bt_tx_temp_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12)&((uint32_t)0x0003f000);
    REG_PL_WR(PWR_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_temp_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_bt_tx_temp_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6)&((uint32_t)0x00000fc0);
    REG_PL_WR(PWR_CTRL_4_ADDR,localVal);
}

static inline uint32_t rf_bt_tx_temp_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_bt_tx_temp_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PWR_CTRL_4_ADDR,localVal);
}


#define PWR_WIFI_PA_0_ADDR (RF_BASE + 0x000007bc)

static inline uint32_t pwr_wifi_pa_0_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_0_ADDR);
}

static inline void pwr_wifi_pa_0_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_b_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_0_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_0_ADDR,localVal);
}


#define PWR_WIFI_PA_1_ADDR (RF_BASE + 0x000007c0)

static inline uint32_t pwr_wifi_pa_1_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_1_ADDR);
}

static inline void pwr_wifi_pa_1_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_g_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_1_ADDR,localVal);
}


#define PWR_WIFI_PA_2_ADDR (RF_BASE + 0x000007c4)

static inline uint32_t pwr_wifi_pa_2_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_2_ADDR);
}

static inline void pwr_wifi_pa_2_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_2_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_2_ADDR,localVal);
}


#define PWR_WIFI_PA_3_ADDR (RF_BASE + 0x000007c8)

static inline uint32_t pwr_wifi_pa_3_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_3_ADDR);
}

static inline void pwr_wifi_pa_3_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_ax0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_ax0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_3_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_3_ADDR,localVal);
}


#define PWR_WIFI_PA_4_ADDR (RF_BASE + 0x000007cc)

static inline uint32_t pwr_wifi_pa_4_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_4_ADDR);
}

static inline void pwr_wifi_pa_4_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_ax1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_ax1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_4_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_4_ADDR,localVal);
}


#define PWR_WIFI_PA_5_ADDR (RF_BASE + 0x000007d0)

static inline uint32_t pwr_wifi_pa_5_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_5_ADDR);
}

static inline void pwr_wifi_pa_5_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_ax2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_ax2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_5_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_5_ADDR,localVal);
}


#define PWR_WIFI_PA_6_ADDR (RF_BASE + 0x000007d4)

static inline uint32_t pwr_wifi_pa_6_get(void)
{
    return REG_PL_RD(PWR_WIFI_PA_6_ADDR);
}

static inline void pwr_wifi_pa_6_set(uint32_t x)
{
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,x);
}

static inline uint32_t rf_wifi_pa_ib_fix_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_wifi_pa_ib_fix_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_etb_en_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_wifi_pa_etb_en_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcore_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_wifi_pa_vbcore_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_wifi_pa_vbpmos_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbpmos_iet_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_wifi_pa_vbpmos_iet_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iet_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_wifi_pa_iet_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_iaq_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_wifi_pa_iaq_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}

static inline uint32_t rf_wifi_pa_vbcas_ax3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_wifi_pa_vbcas_ax3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_WIFI_PA_6_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_WIFI_PA_6_ADDR,localVal);
}


#define PWR_BZ_PA_ADDR (RF_BASE + 0x000007d8)

static inline uint32_t pwr_bz_pa_get(void)
{
    return REG_PL_RD(PWR_BZ_PA_ADDR);
}

static inline void pwr_bz_pa_set(uint32_t x)
{
    REG_PL_WR(PWR_BZ_PA_ADDR,x);
}

static inline uint32_t rf_bz_pa_ib_fix_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void rf_bz_pa_ib_fix_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_etb_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_bz_pa_etb_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_vbcore_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rf_bz_pa_vbcore_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_vbpmos_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_bz_pa_vbpmos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_vbpmos_iet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_bz_pa_vbpmos_iet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_iet_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rf_bz_pa_iet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6)&((uint32_t)0x000003c0);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_iaq_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x00000038)) >> 3);
}

static inline void rf_bz_pa_iaq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x00000038);
    localVal |= (x << 3)&((uint32_t)0x00000038);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}

static inline uint32_t rf_bz_pa_vbcas_getf(void)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rf_bz_pa_vbcas_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PWR_BZ_PA_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PWR_BZ_PA_ADDR,localVal);
}


#define TX_DPD_CTRL_0_ADDR (RF_BASE + 0x00000800)

static inline uint32_t tx_dpd_ctrl_0_get(void)
{
    return REG_PL_RD(TX_DPD_CTRL_0_ADDR);
}

static inline void tx_dpd_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,x);
}

static inline uint32_t rf_tx_dpd_comp_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00600000)) >> 21);
}

static inline void rf_tx_dpd_comp_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00600000);
    localVal |= (x << 21)&((uint32_t)0x00600000);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_lp_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x001f8000)) >> 15);
}

static inline void rf_tx_dpd_lp_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x001f8000);
    localVal |= (x << 15)&((uint32_t)0x001f8000);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_lp_ctrl_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_tx_dpd_lp_ctrl_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_tx_dpd_ctrl_hw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_lut_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_tx_dpd_lut_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_lut_x_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_tx_dpd_lut_x_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_lut_p_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_tx_dpd_lut_p_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_in_scal_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_tx_dpd_in_scal_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_in_scal_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000001f8)) >> 3);
}

static inline void rf_tx_dpd_in_scal_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000001f8);
    localVal |= (x << 3)&((uint32_t)0x000001f8);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_pmon_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_tx_dpd_pmon_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_amon_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_tx_dpd_amon_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_tx_dpd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(TX_DPD_CTRL_0_ADDR,localVal);
}


#define TX_DPD_CTRL_1_ADDR (RF_BASE + 0x00000804)

static inline uint32_t tx_dpd_ctrl_1_get(void)
{
    return REG_PL_RD(TX_DPD_CTRL_1_ADDR);
}

static inline void tx_dpd_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_CTRL_1_ADDR,x);
}

static inline uint32_t rf_tx_dpd_en_dsss_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rf_tx_dpd_en_dsss_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(TX_DPD_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_tx_dpd_en_ofdm_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_tx_dpd_en_ofdm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(TX_DPD_CTRL_1_ADDR,localVal);
}


#define TX_DPD_LUT0_0_ADDR (RF_BASE + 0x00000808)

static inline uint32_t tx_dpd_lut0_0_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_0_ADDR);
}

static inline void tx_dpd_lut0_0_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_0_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_0_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_0_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_0_ADDR,localVal);
}


#define TX_DPD_LUT0_1_ADDR (RF_BASE + 0x0000080c)

static inline uint32_t tx_dpd_lut0_1_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_1_ADDR);
}

static inline void tx_dpd_lut0_1_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_1_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_1_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_1_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_1_ADDR,localVal);
}


#define TX_DPD_LUT0_2_ADDR (RF_BASE + 0x00000810)

static inline uint32_t tx_dpd_lut0_2_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_2_ADDR);
}

static inline void tx_dpd_lut0_2_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_2_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_2_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_2_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_2_ADDR,localVal);
}


#define TX_DPD_LUT0_3_ADDR (RF_BASE + 0x00000814)

static inline uint32_t tx_dpd_lut0_3_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_3_ADDR);
}

static inline void tx_dpd_lut0_3_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_3_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_3_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_3_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_3_ADDR,localVal);
}


#define TX_DPD_LUT0_4_ADDR (RF_BASE + 0x00000818)

static inline uint32_t tx_dpd_lut0_4_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_4_ADDR);
}

static inline void tx_dpd_lut0_4_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_4_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_4_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_4_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_4_ADDR,localVal);
}


#define TX_DPD_LUT0_5_ADDR (RF_BASE + 0x0000081c)

static inline uint32_t tx_dpd_lut0_5_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_5_ADDR);
}

static inline void tx_dpd_lut0_5_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_5_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_5_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_5_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_5_ADDR,localVal);
}


#define TX_DPD_LUT0_6_ADDR (RF_BASE + 0x00000820)

static inline uint32_t tx_dpd_lut0_6_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_6_ADDR);
}

static inline void tx_dpd_lut0_6_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_6_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_6_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_6_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_6_ADDR,localVal);
}


#define TX_DPD_LUT0_7_ADDR (RF_BASE + 0x00000824)

static inline uint32_t tx_dpd_lut0_7_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_7_ADDR);
}

static inline void tx_dpd_lut0_7_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_7_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_7_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_7_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_7_ADDR,localVal);
}


#define TX_DPD_LUT0_8_ADDR (RF_BASE + 0x00000828)

static inline uint32_t tx_dpd_lut0_8_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_8_ADDR);
}

static inline void tx_dpd_lut0_8_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_8_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_8_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_8_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_8_ADDR,localVal);
}


#define TX_DPD_LUT0_9_ADDR (RF_BASE + 0x0000082c)

static inline uint32_t tx_dpd_lut0_9_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_9_ADDR);
}

static inline void tx_dpd_lut0_9_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_9_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_9_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_9_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_9_ADDR,localVal);
}


#define TX_DPD_LUT0_10_ADDR (RF_BASE + 0x00000830)

static inline uint32_t tx_dpd_lut0_10_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_10_ADDR);
}

static inline void tx_dpd_lut0_10_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_10_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_10_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_10_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_10_ADDR,localVal);
}


#define TX_DPD_LUT0_11_ADDR (RF_BASE + 0x00000834)

static inline uint32_t tx_dpd_lut0_11_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_11_ADDR);
}

static inline void tx_dpd_lut0_11_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_11_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_11_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_11_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_11_ADDR,localVal);
}


#define TX_DPD_LUT0_12_ADDR (RF_BASE + 0x00000838)

static inline uint32_t tx_dpd_lut0_12_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_12_ADDR);
}

static inline void tx_dpd_lut0_12_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_12_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_12_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_12_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_12_ADDR,localVal);
}


#define TX_DPD_LUT0_13_ADDR (RF_BASE + 0x0000083c)

static inline uint32_t tx_dpd_lut0_13_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_13_ADDR);
}

static inline void tx_dpd_lut0_13_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_13_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_13_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_13_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_13_ADDR,localVal);
}


#define TX_DPD_LUT0_14_ADDR (RF_BASE + 0x00000840)

static inline uint32_t tx_dpd_lut0_14_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_14_ADDR);
}

static inline void tx_dpd_lut0_14_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_14_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_14_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_14_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_14_ADDR,localVal);
}


#define TX_DPD_LUT0_15_ADDR (RF_BASE + 0x00000844)

static inline uint32_t tx_dpd_lut0_15_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_15_ADDR);
}

static inline void tx_dpd_lut0_15_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_15_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_15_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_15_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_15_ADDR,localVal);
}


#define TX_DPD_LUT0_16_ADDR (RF_BASE + 0x00000848)

static inline uint32_t tx_dpd_lut0_16_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_16_ADDR);
}

static inline void tx_dpd_lut0_16_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_16_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_16_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_16_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_16_ADDR,localVal);
}


#define TX_DPD_LUT0_17_ADDR (RF_BASE + 0x0000084c)

static inline uint32_t tx_dpd_lut0_17_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_17_ADDR);
}

static inline void tx_dpd_lut0_17_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_17_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_17_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_17_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_17_ADDR,localVal);
}


#define TX_DPD_LUT0_18_ADDR (RF_BASE + 0x00000850)

static inline uint32_t tx_dpd_lut0_18_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_18_ADDR);
}

static inline void tx_dpd_lut0_18_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_18_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_18_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_18_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_18_ADDR,localVal);
}


#define TX_DPD_LUT0_19_ADDR (RF_BASE + 0x00000854)

static inline uint32_t tx_dpd_lut0_19_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_19_ADDR);
}

static inline void tx_dpd_lut0_19_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_19_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_19_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_19_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_19_ADDR,localVal);
}


#define TX_DPD_LUT0_20_ADDR (RF_BASE + 0x00000858)

static inline uint32_t tx_dpd_lut0_20_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_20_ADDR);
}

static inline void tx_dpd_lut0_20_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_20_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_20_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_20_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_20_ADDR,localVal);
}


#define TX_DPD_LUT0_21_ADDR (RF_BASE + 0x0000085c)

static inline uint32_t tx_dpd_lut0_21_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_21_ADDR);
}

static inline void tx_dpd_lut0_21_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_21_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_21_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_21_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_21_ADDR,localVal);
}


#define TX_DPD_LUT0_22_ADDR (RF_BASE + 0x00000860)

static inline uint32_t tx_dpd_lut0_22_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_22_ADDR);
}

static inline void tx_dpd_lut0_22_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_22_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_22_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_22_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_22_ADDR,localVal);
}


#define TX_DPD_LUT0_23_ADDR (RF_BASE + 0x00000864)

static inline uint32_t tx_dpd_lut0_23_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_23_ADDR);
}

static inline void tx_dpd_lut0_23_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_23_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_23_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_23_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_23_ADDR,localVal);
}


#define TX_DPD_LUT0_24_ADDR (RF_BASE + 0x00000868)

static inline uint32_t tx_dpd_lut0_24_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_24_ADDR);
}

static inline void tx_dpd_lut0_24_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_24_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_24_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_24_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_24_ADDR,localVal);
}


#define TX_DPD_LUT0_25_ADDR (RF_BASE + 0x0000086c)

static inline uint32_t tx_dpd_lut0_25_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_25_ADDR);
}

static inline void tx_dpd_lut0_25_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_25_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_25_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_25_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_25_ADDR,localVal);
}


#define TX_DPD_LUT0_26_ADDR (RF_BASE + 0x00000870)

static inline uint32_t tx_dpd_lut0_26_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_26_ADDR);
}

static inline void tx_dpd_lut0_26_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_26_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_26_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_26_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_26_ADDR,localVal);
}


#define TX_DPD_LUT0_27_ADDR (RF_BASE + 0x00000874)

static inline uint32_t tx_dpd_lut0_27_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_27_ADDR);
}

static inline void tx_dpd_lut0_27_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_27_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_27_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_27_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_27_ADDR,localVal);
}


#define TX_DPD_LUT0_28_ADDR (RF_BASE + 0x00000878)

static inline uint32_t tx_dpd_lut0_28_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_28_ADDR);
}

static inline void tx_dpd_lut0_28_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_28_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_28_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_28_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_28_ADDR,localVal);
}


#define TX_DPD_LUT0_29_ADDR (RF_BASE + 0x0000087c)

static inline uint32_t tx_dpd_lut0_29_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_29_ADDR);
}

static inline void tx_dpd_lut0_29_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_29_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_29_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_29_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_29_ADDR,localVal);
}


#define TX_DPD_LUT0_30_ADDR (RF_BASE + 0x00000880)

static inline uint32_t tx_dpd_lut0_30_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_30_ADDR);
}

static inline void tx_dpd_lut0_30_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_30_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_30_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_30_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_30_ADDR,localVal);
}


#define TX_DPD_LUT0_31_ADDR (RF_BASE + 0x00000884)

static inline uint32_t tx_dpd_lut0_31_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_31_ADDR);
}

static inline void tx_dpd_lut0_31_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_31_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_31_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_31_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_31_ADDR,localVal);
}


#define TX_DPD_LUT0_32_ADDR (RF_BASE + 0x00000888)

static inline uint32_t tx_dpd_lut0_32_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_32_ADDR);
}

static inline void tx_dpd_lut0_32_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_32_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_32_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_32_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_32_ADDR,localVal);
}


#define TX_DPD_LUT0_33_ADDR (RF_BASE + 0x0000088c)

static inline uint32_t tx_dpd_lut0_33_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_33_ADDR);
}

static inline void tx_dpd_lut0_33_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_33_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_33_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_33_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_33_ADDR,localVal);
}


#define TX_DPD_LUT0_34_ADDR (RF_BASE + 0x00000890)

static inline uint32_t tx_dpd_lut0_34_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_34_ADDR);
}

static inline void tx_dpd_lut0_34_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_34_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_34_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_34_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_34_ADDR,localVal);
}


#define TX_DPD_LUT0_35_ADDR (RF_BASE + 0x00000894)

static inline uint32_t tx_dpd_lut0_35_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_35_ADDR);
}

static inline void tx_dpd_lut0_35_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_35_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_35_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_35_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_35_ADDR,localVal);
}


#define TX_DPD_LUT0_36_ADDR (RF_BASE + 0x00000898)

static inline uint32_t tx_dpd_lut0_36_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_36_ADDR);
}

static inline void tx_dpd_lut0_36_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_36_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_36_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_36_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_36_ADDR,localVal);
}


#define TX_DPD_LUT0_37_ADDR (RF_BASE + 0x0000089c)

static inline uint32_t tx_dpd_lut0_37_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_37_ADDR);
}

static inline void tx_dpd_lut0_37_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_37_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_37_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_37_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_37_ADDR,localVal);
}


#define TX_DPD_LUT0_38_ADDR (RF_BASE + 0x000008a0)

static inline uint32_t tx_dpd_lut0_38_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_38_ADDR);
}

static inline void tx_dpd_lut0_38_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_38_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_38_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_38_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_38_ADDR,localVal);
}


#define TX_DPD_LUT0_39_ADDR (RF_BASE + 0x000008a4)

static inline uint32_t tx_dpd_lut0_39_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_39_ADDR);
}

static inline void tx_dpd_lut0_39_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_39_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_39_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_39_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_39_ADDR,localVal);
}


#define TX_DPD_LUT0_40_ADDR (RF_BASE + 0x000008a8)

static inline uint32_t tx_dpd_lut0_40_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_40_ADDR);
}

static inline void tx_dpd_lut0_40_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_40_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_40_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_40_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_40_ADDR,localVal);
}


#define TX_DPD_LUT0_41_ADDR (RF_BASE + 0x000008ac)

static inline uint32_t tx_dpd_lut0_41_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_41_ADDR);
}

static inline void tx_dpd_lut0_41_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_41_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_41_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_41_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_41_ADDR,localVal);
}


#define TX_DPD_LUT0_42_ADDR (RF_BASE + 0x000008b0)

static inline uint32_t tx_dpd_lut0_42_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_42_ADDR);
}

static inline void tx_dpd_lut0_42_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_42_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_42_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_42_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_42_ADDR,localVal);
}


#define TX_DPD_LUT0_43_ADDR (RF_BASE + 0x000008b4)

static inline uint32_t tx_dpd_lut0_43_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_43_ADDR);
}

static inline void tx_dpd_lut0_43_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_43_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_43_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_43_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_43_ADDR,localVal);
}


#define TX_DPD_LUT0_44_ADDR (RF_BASE + 0x000008b8)

static inline uint32_t tx_dpd_lut0_44_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_44_ADDR);
}

static inline void tx_dpd_lut0_44_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_44_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_44_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_44_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_44_ADDR,localVal);
}


#define TX_DPD_LUT0_45_ADDR (RF_BASE + 0x000008bc)

static inline uint32_t tx_dpd_lut0_45_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_45_ADDR);
}

static inline void tx_dpd_lut0_45_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_45_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_45_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_45_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_45_ADDR,localVal);
}


#define TX_DPD_LUT0_46_ADDR (RF_BASE + 0x000008c0)

static inline uint32_t tx_dpd_lut0_46_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_46_ADDR);
}

static inline void tx_dpd_lut0_46_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_46_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_46_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_46_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_46_ADDR,localVal);
}


#define TX_DPD_LUT0_47_ADDR (RF_BASE + 0x000008c4)

static inline uint32_t tx_dpd_lut0_47_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_47_ADDR);
}

static inline void tx_dpd_lut0_47_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_47_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_47_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_47_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_47_ADDR,localVal);
}


#define TX_DPD_LUT0_48_ADDR (RF_BASE + 0x000008c8)

static inline uint32_t tx_dpd_lut0_48_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_48_ADDR);
}

static inline void tx_dpd_lut0_48_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_48_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_48_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_48_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_48_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_48_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_48_ADDR,localVal);
}


#define TX_DPD_LUT0_49_ADDR (RF_BASE + 0x000008cc)

static inline uint32_t tx_dpd_lut0_49_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_49_ADDR);
}

static inline void tx_dpd_lut0_49_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_49_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_49_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_49_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_49_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_49_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_49_ADDR,localVal);
}


#define TX_DPD_LUT0_50_ADDR (RF_BASE + 0x000008d0)

static inline uint32_t tx_dpd_lut0_50_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_50_ADDR);
}

static inline void tx_dpd_lut0_50_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_50_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_50_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_50_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_50_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_50_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_50_ADDR,localVal);
}


#define TX_DPD_LUT0_51_ADDR (RF_BASE + 0x000008d4)

static inline uint32_t tx_dpd_lut0_51_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_51_ADDR);
}

static inline void tx_dpd_lut0_51_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_51_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_51_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_51_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_51_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_51_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_51_ADDR,localVal);
}


#define TX_DPD_LUT0_52_ADDR (RF_BASE + 0x000008d8)

static inline uint32_t tx_dpd_lut0_52_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_52_ADDR);
}

static inline void tx_dpd_lut0_52_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_52_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_52_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_52_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_52_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_52_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_52_ADDR,localVal);
}


#define TX_DPD_LUT0_53_ADDR (RF_BASE + 0x000008dc)

static inline uint32_t tx_dpd_lut0_53_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_53_ADDR);
}

static inline void tx_dpd_lut0_53_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_53_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_53_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_53_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_53_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_53_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_53_ADDR,localVal);
}


#define TX_DPD_LUT0_54_ADDR (RF_BASE + 0x000008e0)

static inline uint32_t tx_dpd_lut0_54_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_54_ADDR);
}

static inline void tx_dpd_lut0_54_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_54_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_54_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_54_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_54_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_54_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_54_ADDR,localVal);
}


#define TX_DPD_LUT0_55_ADDR (RF_BASE + 0x000008e4)

static inline uint32_t tx_dpd_lut0_55_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_55_ADDR);
}

static inline void tx_dpd_lut0_55_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_55_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_55_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_55_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_55_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_55_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_55_ADDR,localVal);
}


#define TX_DPD_LUT0_56_ADDR (RF_BASE + 0x000008e8)

static inline uint32_t tx_dpd_lut0_56_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_56_ADDR);
}

static inline void tx_dpd_lut0_56_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_56_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_56_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_56_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_56_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_56_ADDR,localVal);
}


#define TX_DPD_LUT0_57_ADDR (RF_BASE + 0x000008ec)

static inline uint32_t tx_dpd_lut0_57_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_57_ADDR);
}

static inline void tx_dpd_lut0_57_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_57_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_57_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_57_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_57_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_57_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_57_ADDR,localVal);
}


#define TX_DPD_LUT0_58_ADDR (RF_BASE + 0x000008f0)

static inline uint32_t tx_dpd_lut0_58_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_58_ADDR);
}

static inline void tx_dpd_lut0_58_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_58_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_58_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_58_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_58_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_58_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_58_ADDR,localVal);
}


#define TX_DPD_LUT0_59_ADDR (RF_BASE + 0x000008f4)

static inline uint32_t tx_dpd_lut0_59_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_59_ADDR);
}

static inline void tx_dpd_lut0_59_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_59_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_59_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_59_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_59_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_59_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_59_ADDR,localVal);
}


#define TX_DPD_LUT0_60_ADDR (RF_BASE + 0x000008f8)

static inline uint32_t tx_dpd_lut0_60_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_60_ADDR);
}

static inline void tx_dpd_lut0_60_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_60_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_60_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_60_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_60_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_60_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_60_ADDR,localVal);
}


#define TX_DPD_LUT0_61_ADDR (RF_BASE + 0x000008fc)

static inline uint32_t tx_dpd_lut0_61_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_61_ADDR);
}

static inline void tx_dpd_lut0_61_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_61_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_61_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_61_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_61_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_61_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_61_ADDR,localVal);
}


#define TX_DPD_LUT0_62_ADDR (RF_BASE + 0x00000900)

static inline uint32_t tx_dpd_lut0_62_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_62_ADDR);
}

static inline void tx_dpd_lut0_62_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_62_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_62_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_62_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_62_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_62_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_62_ADDR,localVal);
}


#define TX_DPD_LUT0_63_ADDR (RF_BASE + 0x00000904)

static inline uint32_t tx_dpd_lut0_63_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_63_ADDR);
}

static inline void tx_dpd_lut0_63_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_63_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_63_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_63_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_63_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_63_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_63_ADDR,localVal);
}


#define TX_DPD_LUT0_64_ADDR (RF_BASE + 0x00000908)

static inline uint32_t tx_dpd_lut0_64_get(void)
{
    return REG_PL_RD(TX_DPD_LUT0_64_ADDR);
}

static inline void tx_dpd_lut0_64_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT0_64_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut0_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_64_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut0_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT0_64_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT0_64_ADDR,localVal);
}


#define TX_DPD_LUT1_0_ADDR (RF_BASE + 0x0000090c)

static inline uint32_t tx_dpd_lut1_0_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_0_ADDR);
}

static inline void tx_dpd_lut1_0_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_0_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_0_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_0_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_0_ADDR,localVal);
}


#define TX_DPD_LUT1_1_ADDR (RF_BASE + 0x00000910)

static inline uint32_t tx_dpd_lut1_1_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_1_ADDR);
}

static inline void tx_dpd_lut1_1_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_1_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_1_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_1_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_1_ADDR,localVal);
}


#define TX_DPD_LUT1_2_ADDR (RF_BASE + 0x00000914)

static inline uint32_t tx_dpd_lut1_2_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_2_ADDR);
}

static inline void tx_dpd_lut1_2_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_2_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_2_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_2_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_2_ADDR,localVal);
}


#define TX_DPD_LUT1_3_ADDR (RF_BASE + 0x00000918)

static inline uint32_t tx_dpd_lut1_3_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_3_ADDR);
}

static inline void tx_dpd_lut1_3_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_3_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_3_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_3_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_3_ADDR,localVal);
}


#define TX_DPD_LUT1_4_ADDR (RF_BASE + 0x0000091c)

static inline uint32_t tx_dpd_lut1_4_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_4_ADDR);
}

static inline void tx_dpd_lut1_4_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_4_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_4_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_4_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_4_ADDR,localVal);
}


#define TX_DPD_LUT1_5_ADDR (RF_BASE + 0x00000920)

static inline uint32_t tx_dpd_lut1_5_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_5_ADDR);
}

static inline void tx_dpd_lut1_5_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_5_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_5_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_5_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_5_ADDR,localVal);
}


#define TX_DPD_LUT1_6_ADDR (RF_BASE + 0x00000924)

static inline uint32_t tx_dpd_lut1_6_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_6_ADDR);
}

static inline void tx_dpd_lut1_6_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_6_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_6_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_6_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_6_ADDR,localVal);
}


#define TX_DPD_LUT1_7_ADDR (RF_BASE + 0x00000928)

static inline uint32_t tx_dpd_lut1_7_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_7_ADDR);
}

static inline void tx_dpd_lut1_7_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_7_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_7_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_7_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_7_ADDR,localVal);
}


#define TX_DPD_LUT1_8_ADDR (RF_BASE + 0x0000092c)

static inline uint32_t tx_dpd_lut1_8_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_8_ADDR);
}

static inline void tx_dpd_lut1_8_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_8_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_8_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_8_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_8_ADDR,localVal);
}


#define TX_DPD_LUT1_9_ADDR (RF_BASE + 0x00000930)

static inline uint32_t tx_dpd_lut1_9_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_9_ADDR);
}

static inline void tx_dpd_lut1_9_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_9_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_9_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_9_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_9_ADDR,localVal);
}


#define TX_DPD_LUT1_10_ADDR (RF_BASE + 0x00000934)

static inline uint32_t tx_dpd_lut1_10_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_10_ADDR);
}

static inline void tx_dpd_lut1_10_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_10_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_10_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_10_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_10_ADDR,localVal);
}


#define TX_DPD_LUT1_11_ADDR (RF_BASE + 0x00000938)

static inline uint32_t tx_dpd_lut1_11_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_11_ADDR);
}

static inline void tx_dpd_lut1_11_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_11_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_11_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_11_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_11_ADDR,localVal);
}


#define TX_DPD_LUT1_12_ADDR (RF_BASE + 0x0000093c)

static inline uint32_t tx_dpd_lut1_12_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_12_ADDR);
}

static inline void tx_dpd_lut1_12_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_12_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_12_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_12_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_12_ADDR,localVal);
}


#define TX_DPD_LUT1_13_ADDR (RF_BASE + 0x00000940)

static inline uint32_t tx_dpd_lut1_13_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_13_ADDR);
}

static inline void tx_dpd_lut1_13_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_13_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_13_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_13_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_13_ADDR,localVal);
}


#define TX_DPD_LUT1_14_ADDR (RF_BASE + 0x00000944)

static inline uint32_t tx_dpd_lut1_14_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_14_ADDR);
}

static inline void tx_dpd_lut1_14_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_14_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_14_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_14_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_14_ADDR,localVal);
}


#define TX_DPD_LUT1_15_ADDR (RF_BASE + 0x00000948)

static inline uint32_t tx_dpd_lut1_15_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_15_ADDR);
}

static inline void tx_dpd_lut1_15_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_15_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_15_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_15_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_15_ADDR,localVal);
}


#define TX_DPD_LUT1_16_ADDR (RF_BASE + 0x0000094c)

static inline uint32_t tx_dpd_lut1_16_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_16_ADDR);
}

static inline void tx_dpd_lut1_16_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_16_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_16_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_16_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_16_ADDR,localVal);
}


#define TX_DPD_LUT1_17_ADDR (RF_BASE + 0x00000950)

static inline uint32_t tx_dpd_lut1_17_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_17_ADDR);
}

static inline void tx_dpd_lut1_17_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_17_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_17_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_17_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_17_ADDR,localVal);
}


#define TX_DPD_LUT1_18_ADDR (RF_BASE + 0x00000954)

static inline uint32_t tx_dpd_lut1_18_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_18_ADDR);
}

static inline void tx_dpd_lut1_18_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_18_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_18_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_18_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_18_ADDR,localVal);
}


#define TX_DPD_LUT1_19_ADDR (RF_BASE + 0x00000958)

static inline uint32_t tx_dpd_lut1_19_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_19_ADDR);
}

static inline void tx_dpd_lut1_19_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_19_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_19_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_19_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_19_ADDR,localVal);
}


#define TX_DPD_LUT1_20_ADDR (RF_BASE + 0x0000095c)

static inline uint32_t tx_dpd_lut1_20_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_20_ADDR);
}

static inline void tx_dpd_lut1_20_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_20_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_20_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_20_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_20_ADDR,localVal);
}


#define TX_DPD_LUT1_21_ADDR (RF_BASE + 0x00000960)

static inline uint32_t tx_dpd_lut1_21_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_21_ADDR);
}

static inline void tx_dpd_lut1_21_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_21_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_21_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_21_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_21_ADDR,localVal);
}


#define TX_DPD_LUT1_22_ADDR (RF_BASE + 0x00000964)

static inline uint32_t tx_dpd_lut1_22_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_22_ADDR);
}

static inline void tx_dpd_lut1_22_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_22_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_22_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_22_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_22_ADDR,localVal);
}


#define TX_DPD_LUT1_23_ADDR (RF_BASE + 0x00000968)

static inline uint32_t tx_dpd_lut1_23_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_23_ADDR);
}

static inline void tx_dpd_lut1_23_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_23_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_23_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_23_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_23_ADDR,localVal);
}


#define TX_DPD_LUT1_24_ADDR (RF_BASE + 0x0000096c)

static inline uint32_t tx_dpd_lut1_24_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_24_ADDR);
}

static inline void tx_dpd_lut1_24_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_24_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_24_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_24_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_24_ADDR,localVal);
}


#define TX_DPD_LUT1_25_ADDR (RF_BASE + 0x00000970)

static inline uint32_t tx_dpd_lut1_25_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_25_ADDR);
}

static inline void tx_dpd_lut1_25_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_25_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_25_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_25_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_25_ADDR,localVal);
}


#define TX_DPD_LUT1_26_ADDR (RF_BASE + 0x00000974)

static inline uint32_t tx_dpd_lut1_26_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_26_ADDR);
}

static inline void tx_dpd_lut1_26_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_26_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_26_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_26_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_26_ADDR,localVal);
}


#define TX_DPD_LUT1_27_ADDR (RF_BASE + 0x00000978)

static inline uint32_t tx_dpd_lut1_27_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_27_ADDR);
}

static inline void tx_dpd_lut1_27_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_27_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_27_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_27_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_27_ADDR,localVal);
}


#define TX_DPD_LUT1_28_ADDR (RF_BASE + 0x0000097c)

static inline uint32_t tx_dpd_lut1_28_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_28_ADDR);
}

static inline void tx_dpd_lut1_28_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_28_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_28_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_28_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_28_ADDR,localVal);
}


#define TX_DPD_LUT1_29_ADDR (RF_BASE + 0x00000980)

static inline uint32_t tx_dpd_lut1_29_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_29_ADDR);
}

static inline void tx_dpd_lut1_29_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_29_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_29_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_29_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_29_ADDR,localVal);
}


#define TX_DPD_LUT1_30_ADDR (RF_BASE + 0x00000984)

static inline uint32_t tx_dpd_lut1_30_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_30_ADDR);
}

static inline void tx_dpd_lut1_30_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_30_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_30_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_30_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_30_ADDR,localVal);
}


#define TX_DPD_LUT1_31_ADDR (RF_BASE + 0x00000988)

static inline uint32_t tx_dpd_lut1_31_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_31_ADDR);
}

static inline void tx_dpd_lut1_31_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_31_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_31_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_31_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_31_ADDR,localVal);
}


#define TX_DPD_LUT1_32_ADDR (RF_BASE + 0x0000098c)

static inline uint32_t tx_dpd_lut1_32_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_32_ADDR);
}

static inline void tx_dpd_lut1_32_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_32_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_32_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_32_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_32_ADDR,localVal);
}


#define TX_DPD_LUT1_33_ADDR (RF_BASE + 0x00000990)

static inline uint32_t tx_dpd_lut1_33_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_33_ADDR);
}

static inline void tx_dpd_lut1_33_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_33_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_33_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_33_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_33_ADDR,localVal);
}


#define TX_DPD_LUT1_34_ADDR (RF_BASE + 0x00000994)

static inline uint32_t tx_dpd_lut1_34_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_34_ADDR);
}

static inline void tx_dpd_lut1_34_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_34_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_34_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_34_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_34_ADDR,localVal);
}


#define TX_DPD_LUT1_35_ADDR (RF_BASE + 0x00000998)

static inline uint32_t tx_dpd_lut1_35_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_35_ADDR);
}

static inline void tx_dpd_lut1_35_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_35_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_35_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_35_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_35_ADDR,localVal);
}


#define TX_DPD_LUT1_36_ADDR (RF_BASE + 0x0000099c)

static inline uint32_t tx_dpd_lut1_36_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_36_ADDR);
}

static inline void tx_dpd_lut1_36_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_36_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_36_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_36_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_36_ADDR,localVal);
}


#define TX_DPD_LUT1_37_ADDR (RF_BASE + 0x000009a0)

static inline uint32_t tx_dpd_lut1_37_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_37_ADDR);
}

static inline void tx_dpd_lut1_37_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_37_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_37_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_37_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_37_ADDR,localVal);
}


#define TX_DPD_LUT1_38_ADDR (RF_BASE + 0x000009a4)

static inline uint32_t tx_dpd_lut1_38_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_38_ADDR);
}

static inline void tx_dpd_lut1_38_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_38_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_38_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_38_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_38_ADDR,localVal);
}


#define TX_DPD_LUT1_39_ADDR (RF_BASE + 0x000009a8)

static inline uint32_t tx_dpd_lut1_39_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_39_ADDR);
}

static inline void tx_dpd_lut1_39_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_39_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_39_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_39_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_39_ADDR,localVal);
}


#define TX_DPD_LUT1_40_ADDR (RF_BASE + 0x000009ac)

static inline uint32_t tx_dpd_lut1_40_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_40_ADDR);
}

static inline void tx_dpd_lut1_40_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_40_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_40_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_40_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_40_ADDR,localVal);
}


#define TX_DPD_LUT1_41_ADDR (RF_BASE + 0x000009b0)

static inline uint32_t tx_dpd_lut1_41_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_41_ADDR);
}

static inline void tx_dpd_lut1_41_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_41_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_41_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_41_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_41_ADDR,localVal);
}


#define TX_DPD_LUT1_42_ADDR (RF_BASE + 0x000009b4)

static inline uint32_t tx_dpd_lut1_42_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_42_ADDR);
}

static inline void tx_dpd_lut1_42_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_42_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_42_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_42_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_42_ADDR,localVal);
}


#define TX_DPD_LUT1_43_ADDR (RF_BASE + 0x000009b8)

static inline uint32_t tx_dpd_lut1_43_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_43_ADDR);
}

static inline void tx_dpd_lut1_43_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_43_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_43_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_43_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_43_ADDR,localVal);
}


#define TX_DPD_LUT1_44_ADDR (RF_BASE + 0x000009bc)

static inline uint32_t tx_dpd_lut1_44_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_44_ADDR);
}

static inline void tx_dpd_lut1_44_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_44_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_44_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_44_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_44_ADDR,localVal);
}


#define TX_DPD_LUT1_45_ADDR (RF_BASE + 0x000009c0)

static inline uint32_t tx_dpd_lut1_45_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_45_ADDR);
}

static inline void tx_dpd_lut1_45_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_45_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_45_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_45_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_45_ADDR,localVal);
}


#define TX_DPD_LUT1_46_ADDR (RF_BASE + 0x000009c4)

static inline uint32_t tx_dpd_lut1_46_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_46_ADDR);
}

static inline void tx_dpd_lut1_46_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_46_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_46_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_46_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_46_ADDR,localVal);
}


#define TX_DPD_LUT1_47_ADDR (RF_BASE + 0x000009c8)

static inline uint32_t tx_dpd_lut1_47_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_47_ADDR);
}

static inline void tx_dpd_lut1_47_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_47_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_47_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_47_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_47_ADDR,localVal);
}


#define TX_DPD_LUT1_48_ADDR (RF_BASE + 0x000009cc)

static inline uint32_t tx_dpd_lut1_48_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_48_ADDR);
}

static inline void tx_dpd_lut1_48_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_48_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_48_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_48_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_48_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_48_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_48_ADDR,localVal);
}


#define TX_DPD_LUT1_49_ADDR (RF_BASE + 0x000009d0)

static inline uint32_t tx_dpd_lut1_49_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_49_ADDR);
}

static inline void tx_dpd_lut1_49_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_49_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_49_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_49_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_49_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_49_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_49_ADDR,localVal);
}


#define TX_DPD_LUT1_50_ADDR (RF_BASE + 0x000009d4)

static inline uint32_t tx_dpd_lut1_50_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_50_ADDR);
}

static inline void tx_dpd_lut1_50_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_50_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_50_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_50_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_50_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_50_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_50_ADDR,localVal);
}


#define TX_DPD_LUT1_51_ADDR (RF_BASE + 0x000009d8)

static inline uint32_t tx_dpd_lut1_51_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_51_ADDR);
}

static inline void tx_dpd_lut1_51_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_51_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_51_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_51_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_51_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_51_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_51_ADDR,localVal);
}


#define TX_DPD_LUT1_52_ADDR (RF_BASE + 0x000009dc)

static inline uint32_t tx_dpd_lut1_52_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_52_ADDR);
}

static inline void tx_dpd_lut1_52_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_52_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_52_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_52_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_52_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_52_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_52_ADDR,localVal);
}


#define TX_DPD_LUT1_53_ADDR (RF_BASE + 0x000009e0)

static inline uint32_t tx_dpd_lut1_53_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_53_ADDR);
}

static inline void tx_dpd_lut1_53_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_53_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_53_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_53_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_53_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_53_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_53_ADDR,localVal);
}


#define TX_DPD_LUT1_54_ADDR (RF_BASE + 0x000009e4)

static inline uint32_t tx_dpd_lut1_54_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_54_ADDR);
}

static inline void tx_dpd_lut1_54_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_54_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_54_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_54_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_54_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_54_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_54_ADDR,localVal);
}


#define TX_DPD_LUT1_55_ADDR (RF_BASE + 0x000009e8)

static inline uint32_t tx_dpd_lut1_55_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_55_ADDR);
}

static inline void tx_dpd_lut1_55_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_55_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_55_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_55_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_55_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_55_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_55_ADDR,localVal);
}


#define TX_DPD_LUT1_56_ADDR (RF_BASE + 0x000009ec)

static inline uint32_t tx_dpd_lut1_56_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_56_ADDR);
}

static inline void tx_dpd_lut1_56_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_56_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_56_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_56_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_56_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_56_ADDR,localVal);
}


#define TX_DPD_LUT1_57_ADDR (RF_BASE + 0x000009f0)

static inline uint32_t tx_dpd_lut1_57_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_57_ADDR);
}

static inline void tx_dpd_lut1_57_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_57_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_57_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_57_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_57_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_57_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_57_ADDR,localVal);
}


#define TX_DPD_LUT1_58_ADDR (RF_BASE + 0x000009f4)

static inline uint32_t tx_dpd_lut1_58_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_58_ADDR);
}

static inline void tx_dpd_lut1_58_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_58_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_58_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_58_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_58_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_58_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_58_ADDR,localVal);
}


#define TX_DPD_LUT1_59_ADDR (RF_BASE + 0x000009f8)

static inline uint32_t tx_dpd_lut1_59_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_59_ADDR);
}

static inline void tx_dpd_lut1_59_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_59_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_59_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_59_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_59_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_59_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_59_ADDR,localVal);
}


#define TX_DPD_LUT1_60_ADDR (RF_BASE + 0x000009fc)

static inline uint32_t tx_dpd_lut1_60_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_60_ADDR);
}

static inline void tx_dpd_lut1_60_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_60_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_60_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_60_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_60_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_60_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_60_ADDR,localVal);
}


#define TX_DPD_LUT1_61_ADDR (RF_BASE + 0x00000a00)

static inline uint32_t tx_dpd_lut1_61_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_61_ADDR);
}

static inline void tx_dpd_lut1_61_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_61_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_61_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_61_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_61_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_61_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_61_ADDR,localVal);
}


#define TX_DPD_LUT1_62_ADDR (RF_BASE + 0x00000a04)

static inline uint32_t tx_dpd_lut1_62_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_62_ADDR);
}

static inline void tx_dpd_lut1_62_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_62_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_62_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_62_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_62_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_62_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_62_ADDR,localVal);
}


#define TX_DPD_LUT1_63_ADDR (RF_BASE + 0x00000a08)

static inline uint32_t tx_dpd_lut1_63_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_63_ADDR);
}

static inline void tx_dpd_lut1_63_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_63_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_63_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_63_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_63_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_63_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_63_ADDR,localVal);
}


#define TX_DPD_LUT1_64_ADDR (RF_BASE + 0x00000a0c)

static inline uint32_t tx_dpd_lut1_64_get(void)
{
    return REG_PL_RD(TX_DPD_LUT1_64_ADDR);
}

static inline void tx_dpd_lut1_64_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT1_64_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut1_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_64_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut1_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT1_64_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT1_64_ADDR,localVal);
}


#define RX_DPD_CTRL_0_ADDR (RF_BASE + 0x00000a10)

static inline uint32_t rx_dpd_ctrl_0_get(void)
{
    return REG_PL_RD(RX_DPD_CTRL_0_ADDR);
}

static inline void rx_dpd_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,x);
}

static inline uint32_t rf_sram_clk_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_sram_clk_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sw_reset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rf_rx_dpd_sw_reset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sram_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rf_rx_dpd_sram_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rf_rx_dpd_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rf_rx_dpd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_n_period_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00e00000)) >> 21);
}

static inline void rf_rx_dpd_n_period_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00e00000);
    localVal |= (x << 21)&((uint32_t)0x00e00000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_dgc_shift_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x001c0000)) >> 18);
}

static inline void rf_rx_dpd_dgc_shift_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x001c0000);
    localVal |= (x << 18)&((uint32_t)0x001c0000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_dgc_gain_db_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0003e000)) >> 13);
}

static inline void rf_rx_dpd_dgc_gain_db_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0003e000);
    localVal |= (x << 13)&((uint32_t)0x0003e000);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_fc_mhz_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00001f00)) >> 8);
}

static inline void rf_rx_dpd_fc_mhz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00001f00);
    localVal |= (x << 8)&((uint32_t)0x00001f00);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_wait_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000000f8)) >> 3);
}

static inline void rf_rx_dpd_wait_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000000f8);
    localVal |= (x << 3)&((uint32_t)0x000000f8);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_bypass_filter_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_rx_dpd_bypass_filter_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_i_path_only_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_rx_dpd_i_path_only_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RX_DPD_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define RX_DPD_CTRL_1_ADDR (RF_BASE + 0x00000a14)

static inline uint32_t rx_dpd_ctrl_1_get(void)
{
    return REG_PL_RD(RX_DPD_CTRL_1_ADDR);
}

static inline void rx_dpd_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,x);
}

static inline uint32_t rf_rx_dpd_dly_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void rf_rx_dpd_dly_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_use_sync_mf_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_rx_dpd_use_sync_mf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sync_mf_in_mag_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000c00)) >> 10);
}

static inline void rf_rx_dpd_sync_mf_in_mag_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000c00);
    localVal |= (x << 10)&((uint32_t)0x00000c00);
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sync_mf_out_mag_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void rf_rx_dpd_sync_mf_out_mag_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf_rx_dpd_sample_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_rx_dpd_sample_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX_DPD_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(RX_DPD_CTRL_1_ADDR,localVal);
}


#define TX_DPD_LUT2_0_ADDR (RF_BASE + 0x00000a18)

static inline uint32_t tx_dpd_lut2_0_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_0_ADDR);
}

static inline void tx_dpd_lut2_0_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_0_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_0_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_0_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_0_ADDR,localVal);
}


#define TX_DPD_LUT2_1_ADDR (RF_BASE + 0x00000a1c)

static inline uint32_t tx_dpd_lut2_1_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_1_ADDR);
}

static inline void tx_dpd_lut2_1_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_1_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_1_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_1_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_1_ADDR,localVal);
}


#define TX_DPD_LUT2_2_ADDR (RF_BASE + 0x00000a20)

static inline uint32_t tx_dpd_lut2_2_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_2_ADDR);
}

static inline void tx_dpd_lut2_2_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_2_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_2_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_2_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_2_ADDR,localVal);
}


#define TX_DPD_LUT2_3_ADDR (RF_BASE + 0x00000a24)

static inline uint32_t tx_dpd_lut2_3_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_3_ADDR);
}

static inline void tx_dpd_lut2_3_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_3_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_3_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_3_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_3_ADDR,localVal);
}


#define TX_DPD_LUT2_4_ADDR (RF_BASE + 0x00000a28)

static inline uint32_t tx_dpd_lut2_4_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_4_ADDR);
}

static inline void tx_dpd_lut2_4_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_4_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_4_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_4_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_4_ADDR,localVal);
}


#define TX_DPD_LUT2_5_ADDR (RF_BASE + 0x00000a2c)

static inline uint32_t tx_dpd_lut2_5_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_5_ADDR);
}

static inline void tx_dpd_lut2_5_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_5_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_5_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_5_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_5_ADDR,localVal);
}


#define TX_DPD_LUT2_6_ADDR (RF_BASE + 0x00000a30)

static inline uint32_t tx_dpd_lut2_6_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_6_ADDR);
}

static inline void tx_dpd_lut2_6_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_6_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_6_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_6_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_6_ADDR,localVal);
}


#define TX_DPD_LUT2_7_ADDR (RF_BASE + 0x00000a34)

static inline uint32_t tx_dpd_lut2_7_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_7_ADDR);
}

static inline void tx_dpd_lut2_7_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_7_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_7_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_7_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_7_ADDR,localVal);
}


#define TX_DPD_LUT2_8_ADDR (RF_BASE + 0x00000a38)

static inline uint32_t tx_dpd_lut2_8_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_8_ADDR);
}

static inline void tx_dpd_lut2_8_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_8_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_8_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_8_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_8_ADDR,localVal);
}


#define TX_DPD_LUT2_9_ADDR (RF_BASE + 0x00000a3c)

static inline uint32_t tx_dpd_lut2_9_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_9_ADDR);
}

static inline void tx_dpd_lut2_9_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_9_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_9_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_9_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_9_ADDR,localVal);
}


#define TX_DPD_LUT2_10_ADDR (RF_BASE + 0x00000a40)

static inline uint32_t tx_dpd_lut2_10_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_10_ADDR);
}

static inline void tx_dpd_lut2_10_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_10_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_10_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_10_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_10_ADDR,localVal);
}


#define TX_DPD_LUT2_11_ADDR (RF_BASE + 0x00000a44)

static inline uint32_t tx_dpd_lut2_11_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_11_ADDR);
}

static inline void tx_dpd_lut2_11_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_11_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_11_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_11_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_11_ADDR,localVal);
}


#define TX_DPD_LUT2_12_ADDR (RF_BASE + 0x00000a48)

static inline uint32_t tx_dpd_lut2_12_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_12_ADDR);
}

static inline void tx_dpd_lut2_12_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_12_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_12_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_12_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_12_ADDR,localVal);
}


#define TX_DPD_LUT2_13_ADDR (RF_BASE + 0x00000a4c)

static inline uint32_t tx_dpd_lut2_13_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_13_ADDR);
}

static inline void tx_dpd_lut2_13_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_13_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_13_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_13_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_13_ADDR,localVal);
}


#define TX_DPD_LUT2_14_ADDR (RF_BASE + 0x00000a50)

static inline uint32_t tx_dpd_lut2_14_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_14_ADDR);
}

static inline void tx_dpd_lut2_14_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_14_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_14_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_14_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_14_ADDR,localVal);
}


#define TX_DPD_LUT2_15_ADDR (RF_BASE + 0x00000a54)

static inline uint32_t tx_dpd_lut2_15_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_15_ADDR);
}

static inline void tx_dpd_lut2_15_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_15_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_15_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_15_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_15_ADDR,localVal);
}


#define TX_DPD_LUT2_16_ADDR (RF_BASE + 0x00000a58)

static inline uint32_t tx_dpd_lut2_16_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_16_ADDR);
}

static inline void tx_dpd_lut2_16_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_16_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_16_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_16_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_16_ADDR,localVal);
}


#define TX_DPD_LUT2_17_ADDR (RF_BASE + 0x00000a5c)

static inline uint32_t tx_dpd_lut2_17_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_17_ADDR);
}

static inline void tx_dpd_lut2_17_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_17_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_17_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_17_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_17_ADDR,localVal);
}


#define TX_DPD_LUT2_18_ADDR (RF_BASE + 0x00000a60)

static inline uint32_t tx_dpd_lut2_18_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_18_ADDR);
}

static inline void tx_dpd_lut2_18_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_18_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_18_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_18_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_18_ADDR,localVal);
}


#define TX_DPD_LUT2_19_ADDR (RF_BASE + 0x00000a64)

static inline uint32_t tx_dpd_lut2_19_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_19_ADDR);
}

static inline void tx_dpd_lut2_19_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_19_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_19_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_19_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_19_ADDR,localVal);
}


#define TX_DPD_LUT2_20_ADDR (RF_BASE + 0x00000a68)

static inline uint32_t tx_dpd_lut2_20_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_20_ADDR);
}

static inline void tx_dpd_lut2_20_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_20_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_20_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_20_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_20_ADDR,localVal);
}


#define TX_DPD_LUT2_21_ADDR (RF_BASE + 0x00000a6c)

static inline uint32_t tx_dpd_lut2_21_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_21_ADDR);
}

static inline void tx_dpd_lut2_21_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_21_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_21_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_21_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_21_ADDR,localVal);
}


#define TX_DPD_LUT2_22_ADDR (RF_BASE + 0x00000a70)

static inline uint32_t tx_dpd_lut2_22_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_22_ADDR);
}

static inline void tx_dpd_lut2_22_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_22_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_22_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_22_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_22_ADDR,localVal);
}


#define TX_DPD_LUT2_23_ADDR (RF_BASE + 0x00000a74)

static inline uint32_t tx_dpd_lut2_23_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_23_ADDR);
}

static inline void tx_dpd_lut2_23_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_23_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_23_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_23_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_23_ADDR,localVal);
}


#define TX_DPD_LUT2_24_ADDR (RF_BASE + 0x00000a78)

static inline uint32_t tx_dpd_lut2_24_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_24_ADDR);
}

static inline void tx_dpd_lut2_24_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_24_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_24_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_24_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_24_ADDR,localVal);
}


#define TX_DPD_LUT2_25_ADDR (RF_BASE + 0x00000a7c)

static inline uint32_t tx_dpd_lut2_25_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_25_ADDR);
}

static inline void tx_dpd_lut2_25_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_25_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_25_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_25_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_25_ADDR,localVal);
}


#define TX_DPD_LUT2_26_ADDR (RF_BASE + 0x00000a80)

static inline uint32_t tx_dpd_lut2_26_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_26_ADDR);
}

static inline void tx_dpd_lut2_26_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_26_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_26_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_26_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_26_ADDR,localVal);
}


#define TX_DPD_LUT2_27_ADDR (RF_BASE + 0x00000a84)

static inline uint32_t tx_dpd_lut2_27_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_27_ADDR);
}

static inline void tx_dpd_lut2_27_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_27_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_27_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_27_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_27_ADDR,localVal);
}


#define TX_DPD_LUT2_28_ADDR (RF_BASE + 0x00000a88)

static inline uint32_t tx_dpd_lut2_28_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_28_ADDR);
}

static inline void tx_dpd_lut2_28_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_28_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_28_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_28_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_28_ADDR,localVal);
}


#define TX_DPD_LUT2_29_ADDR (RF_BASE + 0x00000a8c)

static inline uint32_t tx_dpd_lut2_29_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_29_ADDR);
}

static inline void tx_dpd_lut2_29_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_29_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_29_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_29_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_29_ADDR,localVal);
}


#define TX_DPD_LUT2_30_ADDR (RF_BASE + 0x00000a90)

static inline uint32_t tx_dpd_lut2_30_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_30_ADDR);
}

static inline void tx_dpd_lut2_30_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_30_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_30_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_30_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_30_ADDR,localVal);
}


#define TX_DPD_LUT2_31_ADDR (RF_BASE + 0x00000a94)

static inline uint32_t tx_dpd_lut2_31_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_31_ADDR);
}

static inline void tx_dpd_lut2_31_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_31_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_31_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_31_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_31_ADDR,localVal);
}


#define TX_DPD_LUT2_32_ADDR (RF_BASE + 0x00000a98)

static inline uint32_t tx_dpd_lut2_32_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_32_ADDR);
}

static inline void tx_dpd_lut2_32_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_32_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_32_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_32_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_32_ADDR,localVal);
}


#define TX_DPD_LUT2_33_ADDR (RF_BASE + 0x00000a9c)

static inline uint32_t tx_dpd_lut2_33_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_33_ADDR);
}

static inline void tx_dpd_lut2_33_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_33_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_33_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_33_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_33_ADDR,localVal);
}


#define TX_DPD_LUT2_34_ADDR (RF_BASE + 0x00000aa0)

static inline uint32_t tx_dpd_lut2_34_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_34_ADDR);
}

static inline void tx_dpd_lut2_34_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_34_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_34_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_34_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_34_ADDR,localVal);
}


#define TX_DPD_LUT2_35_ADDR (RF_BASE + 0x00000aa4)

static inline uint32_t tx_dpd_lut2_35_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_35_ADDR);
}

static inline void tx_dpd_lut2_35_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_35_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_35_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_35_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_35_ADDR,localVal);
}


#define TX_DPD_LUT2_36_ADDR (RF_BASE + 0x00000aa8)

static inline uint32_t tx_dpd_lut2_36_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_36_ADDR);
}

static inline void tx_dpd_lut2_36_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_36_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_36_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_36_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_36_ADDR,localVal);
}


#define TX_DPD_LUT2_37_ADDR (RF_BASE + 0x00000aac)

static inline uint32_t tx_dpd_lut2_37_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_37_ADDR);
}

static inline void tx_dpd_lut2_37_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_37_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_37_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_37_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_37_ADDR,localVal);
}


#define TX_DPD_LUT2_38_ADDR (RF_BASE + 0x00000ab0)

static inline uint32_t tx_dpd_lut2_38_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_38_ADDR);
}

static inline void tx_dpd_lut2_38_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_38_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_38_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_38_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_38_ADDR,localVal);
}


#define TX_DPD_LUT2_39_ADDR (RF_BASE + 0x00000ab4)

static inline uint32_t tx_dpd_lut2_39_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_39_ADDR);
}

static inline void tx_dpd_lut2_39_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_39_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_39_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_39_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_39_ADDR,localVal);
}


#define TX_DPD_LUT2_40_ADDR (RF_BASE + 0x00000ab8)

static inline uint32_t tx_dpd_lut2_40_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_40_ADDR);
}

static inline void tx_dpd_lut2_40_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_40_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_40_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_40_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_40_ADDR,localVal);
}


#define TX_DPD_LUT2_41_ADDR (RF_BASE + 0x00000abc)

static inline uint32_t tx_dpd_lut2_41_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_41_ADDR);
}

static inline void tx_dpd_lut2_41_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_41_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_41_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_41_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_41_ADDR,localVal);
}


#define TX_DPD_LUT2_42_ADDR (RF_BASE + 0x00000ac0)

static inline uint32_t tx_dpd_lut2_42_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_42_ADDR);
}

static inline void tx_dpd_lut2_42_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_42_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_42_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_42_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_42_ADDR,localVal);
}


#define TX_DPD_LUT2_43_ADDR (RF_BASE + 0x00000ac4)

static inline uint32_t tx_dpd_lut2_43_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_43_ADDR);
}

static inline void tx_dpd_lut2_43_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_43_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_43_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_43_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_43_ADDR,localVal);
}


#define TX_DPD_LUT2_44_ADDR (RF_BASE + 0x00000ac8)

static inline uint32_t tx_dpd_lut2_44_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_44_ADDR);
}

static inline void tx_dpd_lut2_44_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_44_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_44_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_44_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_44_ADDR,localVal);
}


#define TX_DPD_LUT2_45_ADDR (RF_BASE + 0x00000acc)

static inline uint32_t tx_dpd_lut2_45_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_45_ADDR);
}

static inline void tx_dpd_lut2_45_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_45_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_45_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_45_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_45_ADDR,localVal);
}


#define TX_DPD_LUT2_46_ADDR (RF_BASE + 0x00000ad0)

static inline uint32_t tx_dpd_lut2_46_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_46_ADDR);
}

static inline void tx_dpd_lut2_46_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_46_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_46_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_46_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_46_ADDR,localVal);
}


#define TX_DPD_LUT2_47_ADDR (RF_BASE + 0x00000ad4)

static inline uint32_t tx_dpd_lut2_47_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_47_ADDR);
}

static inline void tx_dpd_lut2_47_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_47_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_47_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_47_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_47_ADDR,localVal);
}


#define TX_DPD_LUT2_48_ADDR (RF_BASE + 0x00000ad8)

static inline uint32_t tx_dpd_lut2_48_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_48_ADDR);
}

static inline void tx_dpd_lut2_48_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_48_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_48_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_48_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_48_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_48_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_48_ADDR,localVal);
}


#define TX_DPD_LUT2_49_ADDR (RF_BASE + 0x00000adc)

static inline uint32_t tx_dpd_lut2_49_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_49_ADDR);
}

static inline void tx_dpd_lut2_49_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_49_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_49_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_49_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_49_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_49_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_49_ADDR,localVal);
}


#define TX_DPD_LUT2_50_ADDR (RF_BASE + 0x00000ae0)

static inline uint32_t tx_dpd_lut2_50_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_50_ADDR);
}

static inline void tx_dpd_lut2_50_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_50_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_50_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_50_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_50_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_50_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_50_ADDR,localVal);
}


#define TX_DPD_LUT2_51_ADDR (RF_BASE + 0x00000ae4)

static inline uint32_t tx_dpd_lut2_51_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_51_ADDR);
}

static inline void tx_dpd_lut2_51_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_51_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_51_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_51_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_51_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_51_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_51_ADDR,localVal);
}


#define TX_DPD_LUT2_52_ADDR (RF_BASE + 0x00000ae8)

static inline uint32_t tx_dpd_lut2_52_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_52_ADDR);
}

static inline void tx_dpd_lut2_52_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_52_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_52_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_52_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_52_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_52_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_52_ADDR,localVal);
}


#define TX_DPD_LUT2_53_ADDR (RF_BASE + 0x00000aec)

static inline uint32_t tx_dpd_lut2_53_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_53_ADDR);
}

static inline void tx_dpd_lut2_53_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_53_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_53_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_53_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_53_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_53_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_53_ADDR,localVal);
}


#define TX_DPD_LUT2_54_ADDR (RF_BASE + 0x00000af0)

static inline uint32_t tx_dpd_lut2_54_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_54_ADDR);
}

static inline void tx_dpd_lut2_54_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_54_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_54_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_54_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_54_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_54_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_54_ADDR,localVal);
}


#define TX_DPD_LUT2_55_ADDR (RF_BASE + 0x00000af4)

static inline uint32_t tx_dpd_lut2_55_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_55_ADDR);
}

static inline void tx_dpd_lut2_55_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_55_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_55_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_55_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_55_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_55_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_55_ADDR,localVal);
}


#define TX_DPD_LUT2_56_ADDR (RF_BASE + 0x00000af8)

static inline uint32_t tx_dpd_lut2_56_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_56_ADDR);
}

static inline void tx_dpd_lut2_56_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_56_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_56_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_56_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_56_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_56_ADDR,localVal);
}


#define TX_DPD_LUT2_57_ADDR (RF_BASE + 0x00000afc)

static inline uint32_t tx_dpd_lut2_57_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_57_ADDR);
}

static inline void tx_dpd_lut2_57_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_57_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_57_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_57_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_57_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_57_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_57_ADDR,localVal);
}


#define TX_DPD_LUT2_58_ADDR (RF_BASE + 0x00000b00)

static inline uint32_t tx_dpd_lut2_58_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_58_ADDR);
}

static inline void tx_dpd_lut2_58_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_58_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_58_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_58_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_58_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_58_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_58_ADDR,localVal);
}


#define TX_DPD_LUT2_59_ADDR (RF_BASE + 0x00000b04)

static inline uint32_t tx_dpd_lut2_59_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_59_ADDR);
}

static inline void tx_dpd_lut2_59_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_59_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_59_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_59_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_59_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_59_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_59_ADDR,localVal);
}


#define TX_DPD_LUT2_60_ADDR (RF_BASE + 0x00000b08)

static inline uint32_t tx_dpd_lut2_60_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_60_ADDR);
}

static inline void tx_dpd_lut2_60_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_60_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_60_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_60_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_60_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_60_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_60_ADDR,localVal);
}


#define TX_DPD_LUT2_61_ADDR (RF_BASE + 0x00000b0c)

static inline uint32_t tx_dpd_lut2_61_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_61_ADDR);
}

static inline void tx_dpd_lut2_61_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_61_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_61_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_61_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_61_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_61_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_61_ADDR,localVal);
}


#define TX_DPD_LUT2_62_ADDR (RF_BASE + 0x00000b10)

static inline uint32_t tx_dpd_lut2_62_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_62_ADDR);
}

static inline void tx_dpd_lut2_62_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_62_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_62_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_62_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_62_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_62_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_62_ADDR,localVal);
}


#define TX_DPD_LUT2_63_ADDR (RF_BASE + 0x00000b14)

static inline uint32_t tx_dpd_lut2_63_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_63_ADDR);
}

static inline void tx_dpd_lut2_63_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_63_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_63_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_63_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_63_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_63_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_63_ADDR,localVal);
}


#define TX_DPD_LUT2_64_ADDR (RF_BASE + 0x00000b18)

static inline uint32_t tx_dpd_lut2_64_get(void)
{
    return REG_PL_RD(TX_DPD_LUT2_64_ADDR);
}

static inline void tx_dpd_lut2_64_set(uint32_t x)
{
    REG_PL_WR(TX_DPD_LUT2_64_ADDR,x);
}

static inline uint32_t rf_tx_dpd_lut2_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_64_ADDR);
    return ((localVal & ((uint32_t)0x03ffffff)) >> 0);
}

static inline void rf_tx_dpd_lut2_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_DPD_LUT2_64_ADDR);
    localVal &= ~((uint32_t)0x03ffffff);
    localVal |= (x << 0)&((uint32_t)0x03ffffff);
    REG_PL_WR(TX_DPD_LUT2_64_ADDR,localVal);
}

#endif