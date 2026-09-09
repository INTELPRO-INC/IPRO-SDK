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

#ifndef _REG_RF_FPGA_H_
#define _REG_RF_FPGA_H_
// Date: 20211018

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef RF_FPGA_BASE
#define RF_FPGA_BASE (0x20001000)
#endif



#define RF_FPGA_CTRL_0_ADDR (RF_FPGA_BASE + 0x00000300)

static inline uint32_t rf_fpga_ctrl_0_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
}

static inline void rf_fpga_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_pu_sb_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void fpga_rf_fsm_pu_sb_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_pu_lo_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void fpga_rf_fsm_pu_lo_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_pu_tx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void fpga_rf_fsm_pu_tx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_pu_rx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void fpga_rf_fsm_pu_rx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_state_cci_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0f800000)) >> 23);
}

static inline uint32_t fpga_rf_fsm_pu_source_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void fpga_rf_fsm_pu_source_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_ms_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void fpga_rf_fsm_ms_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_gpio_first_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void fpga_rf_fsm_gpio_first_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void fpga_rf_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_st_cci_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void fpga_rf_fsm_st_cci_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_st_cci_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void fpga_rf_fsm_st_cci_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(RF_FPGA_CTRL_0_ADDR,localVal);
}


#define RF_FPGA_CTRL_1_ADDR (RF_FPGA_BASE + 0x00000304)

static inline uint32_t rf_fpga_ctrl_1_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_1_ADDR);
}

static inline void rf_fpga_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_1_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_cci_wr_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void fpga_rf_fsm_cci_wr_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF_FPGA_CTRL_1_ADDR,localVal);
}


#define RF_FPGA_CTRL_2_ADDR (RF_FPGA_BASE + 0x00000308)

static inline uint32_t rf_fpga_ctrl_2_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_2_ADDR);
}

static inline void rf_fpga_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_2_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_dly_01_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void fpga_rf_fsm_dly_01_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(RF_FPGA_CTRL_2_ADDR,localVal);
}

static inline uint32_t fpga_rf_fsm_dly_30_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void fpga_rf_fsm_dly_30_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF_FPGA_CTRL_2_ADDR,localVal);
}


#define RF_FPGA_CTRL_3_ADDR (RF_FPGA_BASE + 0x0000030c)

static inline uint32_t rf_fpga_ctrl_3_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_3_ADDR);
}

static inline void rf_fpga_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_3_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_pu_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void fpga_rf_fsm_pu_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_FPGA_CTRL_3_ADDR,localVal);
}


#define RF_FPGA_CTRL_4_ADDR (RF_FPGA_BASE + 0x00000310)

static inline uint32_t rf_fpga_ctrl_4_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_4_ADDR);
}

static inline void rf_fpga_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_4_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_pu_lo_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void fpga_rf_fsm_pu_lo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_FPGA_CTRL_4_ADDR,localVal);
}


#define RF_FPGA_CTRL_5_ADDR (RF_FPGA_BASE + 0x00000314)

static inline uint32_t rf_fpga_ctrl_5_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_5_ADDR);
}

static inline void rf_fpga_ctrl_5_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_5_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_pu_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void fpga_rf_fsm_pu_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_FPGA_CTRL_5_ADDR,localVal);
}


#define RF_FPGA_CTRL_6_ADDR (RF_FPGA_BASE + 0x00000318)

static inline uint32_t rf_fpga_ctrl_6_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_6_ADDR);
}

static inline void rf_fpga_ctrl_6_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_6_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_pu_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void fpga_rf_fsm_pu_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF_FPGA_CTRL_6_ADDR,localVal);
}


#define RF_FPGA_CTRL_8_ADDR (RF_FPGA_BASE + 0x0000031c)

static inline uint32_t rf_fpga_ctrl_8_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_8_ADDR);
}

static inline void rf_fpga_ctrl_8_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_8_ADDR,x);
}

static inline uint32_t fpga_rf_fsm_cmd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_8_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void fpga_rf_fsm_cmd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_8_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF_FPGA_CTRL_8_ADDR,localVal);
}


#define RF_FPGA_CTRL_9_ADDR (RF_FPGA_BASE + 0x00000320)

static inline uint32_t rf_fpga_ctrl_9_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
}

static inline void rf_fpga_ctrl_9_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,x);
}

static inline uint32_t fpga_rf_fpga_wlock_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void fpga_rf_fpga_wlock_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void fpga_rf_fpga_wlock_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void fpga_rf_fpga_wlock_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void fpga_rf_fpga_wlock_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void fpga_rf_fpga_wlock_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void fpga_rf_fpga_wlock_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void fpga_rf_fpga_wlock_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}

static inline uint32_t fpga_rf_fpga_wlock_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void fpga_rf_fpga_wlock_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_9_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_FPGA_CTRL_9_ADDR,localVal);
}


#define RF_FPGA_CTRL_10_ADDR (RF_FPGA_BASE + 0x00000324)

static inline uint32_t rf_fpga_ctrl_10_get(void)
{
    return REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
}

static inline void rf_fpga_ctrl_10_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA_CTRL_10_ADDR,x);
}

static inline uint32_t fpga_st_cci_overlap_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t fpga_st_state_diff_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t fpga_cci_dbg_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void fpga_cci_dbg_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF_FPGA_CTRL_10_ADDR,localVal);
}

static inline uint32_t fpga_state_diff_cnt_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void fpga_state_diff_cnt_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA_CTRL_10_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(RF_FPGA_CTRL_10_ADDR,localVal);
}

#endif