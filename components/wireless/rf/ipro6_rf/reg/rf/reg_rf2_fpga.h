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

#ifndef _REG_RF2_FPGA_H_
#define _REG_RF2_FPGA_H_
// Date: 20210916

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef RF2_FPGA_BASE
#define RF2_FPGA_BASE (0x20054000)
#endif



#define RF2_FPGA_CTRL_0_ADDR (RF2_FPGA_BASE + 0x00000600)

static inline uint32_t rf2_fpga_ctrl_0_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
}

static inline void rf2_fpga_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_sb_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf2_fpga_rf_fsm_pu_sb_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_lo_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rf2_fpga_rf_fsm_pu_lo_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_tx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rf2_fpga_rf_fsm_pu_tx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_rx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf2_fpga_rf_fsm_pu_rx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_state_cci_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0f800000)) >> 23);
}

static inline uint32_t rf2_fpga_rf_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rf2_fpga_rf_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_st_cci_4s_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf2_fpga_rf_fsm_st_cci_4s_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_st_cci_4s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf2_fpga_rf_fsm_st_cci_4s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(RF2_FPGA_CTRL_0_ADDR,localVal);
}


#define RF2_FPGA_CTRL_1_ADDR (RF2_FPGA_BASE + 0x00000604)

static inline uint32_t rf2_fpga_ctrl_1_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_1_ADDR);
}

static inline void rf2_fpga_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_1_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_lo_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf2_fpga_rf_fsm_lo_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(RF2_FPGA_CTRL_1_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_cci_wr_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_cci_wr_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_1_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF2_FPGA_CTRL_1_ADDR,localVal);
}


#define RF2_FPGA_CTRL_2_ADDR (RF2_FPGA_BASE + 0x00000608)

static inline uint32_t rf2_fpga_ctrl_2_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_2_ADDR);
}

static inline void rf2_fpga_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_2_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_dly_01_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void rf2_fpga_rf_fsm_dly_01_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(RF2_FPGA_CTRL_2_ADDR,localVal);
}

static inline uint32_t rf2_fpga_rf_fsm_dly_30_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_dly_30_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_2_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(RF2_FPGA_CTRL_2_ADDR,localVal);
}


#define RF2_FPGA_CTRL_3_ADDR (RF2_FPGA_BASE + 0x0000060c)

static inline uint32_t rf2_fpga_ctrl_3_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_3_ADDR);
}

static inline void rf2_fpga_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_3_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_sb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_pu_sb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF2_FPGA_CTRL_3_ADDR,localVal);
}


#define RF2_FPGA_CTRL_4_ADDR (RF2_FPGA_BASE + 0x00000610)

static inline uint32_t rf2_fpga_ctrl_4_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_4_ADDR);
}

static inline void rf2_fpga_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_4_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_lo_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_pu_lo_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF2_FPGA_CTRL_4_ADDR,localVal);
}


#define RF2_FPGA_CTRL_5_ADDR (RF2_FPGA_BASE + 0x00000614)

static inline uint32_t rf2_fpga_ctrl_5_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_5_ADDR);
}

static inline void rf2_fpga_ctrl_5_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_5_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_tx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_pu_tx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF2_FPGA_CTRL_5_ADDR,localVal);
}


#define RF2_FPGA_CTRL_6_ADDR (RF2_FPGA_BASE + 0x00000618)

static inline uint32_t rf2_fpga_ctrl_6_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_6_ADDR);
}

static inline void rf2_fpga_ctrl_6_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_6_ADDR,x);
}

static inline uint32_t rf2_fpga_rf_fsm_pu_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf2_fpga_rf_fsm_pu_rx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(RF2_FPGA_CTRL_6_ADDR,localVal);
}


#define RF2_FPGA_CTRL_7_ADDR (RF2_FPGA_BASE + 0x0000061c)

static inline uint32_t rf2_fpga_ctrl_7_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_7_ADDR);
}

static inline void rf2_fpga_ctrl_7_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_7_ADDR,x);
}

static inline uint32_t rf2_fpga_idac_cw_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_7_ADDR);
    return ((localVal & ((uint32_t)0x03e00000)) >> 21);
}

static inline void rf2_fpga_idac_cw_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_7_ADDR);
    localVal &= ~((uint32_t)0x03e00000);
    localVal |= (x << 21)&((uint32_t)0x03e00000);
    REG_PL_WR(RF2_FPGA_CTRL_7_ADDR,localVal);
}


#define RF2_FPGA_TABLE_0_ADDR (RF2_FPGA_BASE + 0x00000620)

static inline uint32_t rf2_fpga_table_0_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_0_ADDR);
}

static inline void rf2_fpga_table_0_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_0_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_0_ADDR,localVal);
}


#define RF2_FPGA_TABLE_1_ADDR (RF2_FPGA_BASE + 0x00000624)

static inline uint32_t rf2_fpga_table_1_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_1_ADDR);
}

static inline void rf2_fpga_table_1_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_1_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_1_ADDR,localVal);
}


#define RF2_FPGA_TABLE_2_ADDR (RF2_FPGA_BASE + 0x00000628)

static inline uint32_t rf2_fpga_table_2_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_2_ADDR);
}

static inline void rf2_fpga_table_2_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_2_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_2_ADDR,localVal);
}


#define RF2_FPGA_TABLE_3_ADDR (RF2_FPGA_BASE + 0x0000062c)

static inline uint32_t rf2_fpga_table_3_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_3_ADDR);
}

static inline void rf2_fpga_table_3_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_3_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_3_ADDR,localVal);
}


#define RF2_FPGA_TABLE_4_ADDR (RF2_FPGA_BASE + 0x00000630)

static inline uint32_t rf2_fpga_table_4_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_4_ADDR);
}

static inline void rf2_fpga_table_4_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_4_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_4_ADDR,localVal);
}


#define RF2_FPGA_TABLE_5_ADDR (RF2_FPGA_BASE + 0x00000634)

static inline uint32_t rf2_fpga_table_5_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_5_ADDR);
}

static inline void rf2_fpga_table_5_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_5_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_5_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_5_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_5_ADDR,localVal);
}


#define RF2_FPGA_TABLE_6_ADDR (RF2_FPGA_BASE + 0x00000638)

static inline uint32_t rf2_fpga_table_6_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_6_ADDR);
}

static inline void rf2_fpga_table_6_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_6_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_6_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_6_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_6_ADDR,localVal);
}


#define RF2_FPGA_TABLE_7_ADDR (RF2_FPGA_BASE + 0x0000063c)

static inline uint32_t rf2_fpga_table_7_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_7_ADDR);
}

static inline void rf2_fpga_table_7_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_7_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_7_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_7_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_7_ADDR,localVal);
}


#define RF2_FPGA_TABLE_8_ADDR (RF2_FPGA_BASE + 0x00000640)

static inline uint32_t rf2_fpga_table_8_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_8_ADDR);
}

static inline void rf2_fpga_table_8_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_8_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_8_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_8_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_8_ADDR,localVal);
}


#define RF2_FPGA_TABLE_9_ADDR (RF2_FPGA_BASE + 0x00000644)

static inline uint32_t rf2_fpga_table_9_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_9_ADDR);
}

static inline void rf2_fpga_table_9_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_9_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_9_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_9_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_9_ADDR,localVal);
}


#define RF2_FPGA_TABLE_10_ADDR (RF2_FPGA_BASE + 0x00000648)

static inline uint32_t rf2_fpga_table_10_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_10_ADDR);
}

static inline void rf2_fpga_table_10_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_10_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_10_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_10_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_10_ADDR,localVal);
}


#define RF2_FPGA_TABLE_11_ADDR (RF2_FPGA_BASE + 0x0000064c)

static inline uint32_t rf2_fpga_table_11_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_11_ADDR);
}

static inline void rf2_fpga_table_11_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_11_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_11_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_11_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_11_ADDR,localVal);
}


#define RF2_FPGA_TABLE_12_ADDR (RF2_FPGA_BASE + 0x00000650)

static inline uint32_t rf2_fpga_table_12_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_12_ADDR);
}

static inline void rf2_fpga_table_12_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_12_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_12_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_12_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_12_ADDR,localVal);
}


#define RF2_FPGA_TABLE_13_ADDR (RF2_FPGA_BASE + 0x00000654)

static inline uint32_t rf2_fpga_table_13_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_13_ADDR);
}

static inline void rf2_fpga_table_13_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_13_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_13_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_13_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_13_ADDR,localVal);
}


#define RF2_FPGA_TABLE_14_ADDR (RF2_FPGA_BASE + 0x00000658)

static inline uint32_t rf2_fpga_table_14_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_14_ADDR);
}

static inline void rf2_fpga_table_14_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_14_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_14_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_14_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_14_ADDR,localVal);
}


#define RF2_FPGA_TABLE_15_ADDR (RF2_FPGA_BASE + 0x0000065c)

static inline uint32_t rf2_fpga_table_15_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_15_ADDR);
}

static inline void rf2_fpga_table_15_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_15_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_15_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_15_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_15_ADDR,localVal);
}


#define RF2_FPGA_TABLE_16_ADDR (RF2_FPGA_BASE + 0x00000660)

static inline uint32_t rf2_fpga_table_16_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_16_ADDR);
}

static inline void rf2_fpga_table_16_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_16_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_16_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_16_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_16_ADDR,localVal);
}


#define RF2_FPGA_TABLE_17_ADDR (RF2_FPGA_BASE + 0x00000664)

static inline uint32_t rf2_fpga_table_17_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_17_ADDR);
}

static inline void rf2_fpga_table_17_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_17_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_17_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_17_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_17_ADDR,localVal);
}


#define RF2_FPGA_TABLE_18_ADDR (RF2_FPGA_BASE + 0x00000668)

static inline uint32_t rf2_fpga_table_18_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_18_ADDR);
}

static inline void rf2_fpga_table_18_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_18_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_18_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_18_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_18_ADDR,localVal);
}


#define RF2_FPGA_TABLE_19_ADDR (RF2_FPGA_BASE + 0x0000066c)

static inline uint32_t rf2_fpga_table_19_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_19_ADDR);
}

static inline void rf2_fpga_table_19_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_19_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_19_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_19_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_19_ADDR,localVal);
}


#define RF2_FPGA_TABLE_20_ADDR (RF2_FPGA_BASE + 0x00000670)

static inline uint32_t rf2_fpga_table_20_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_20_ADDR);
}

static inline void rf2_fpga_table_20_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_20_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_20_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_20_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_20_ADDR,localVal);
}


#define RF2_FPGA_TABLE_21_ADDR (RF2_FPGA_BASE + 0x00000674)

static inline uint32_t rf2_fpga_table_21_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_21_ADDR);
}

static inline void rf2_fpga_table_21_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_21_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_21_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_21_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_21_ADDR,localVal);
}


#define RF2_FPGA_TABLE_22_ADDR (RF2_FPGA_BASE + 0x00000678)

static inline uint32_t rf2_fpga_table_22_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_22_ADDR);
}

static inline void rf2_fpga_table_22_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_22_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_22_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_22_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_22_ADDR,localVal);
}


#define RF2_FPGA_TABLE_23_ADDR (RF2_FPGA_BASE + 0x0000067c)

static inline uint32_t rf2_fpga_table_23_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_23_ADDR);
}

static inline void rf2_fpga_table_23_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_23_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_23_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_23_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_23_ADDR,localVal);
}


#define RF2_FPGA_TABLE_24_ADDR (RF2_FPGA_BASE + 0x00000680)

static inline uint32_t rf2_fpga_table_24_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_24_ADDR);
}

static inline void rf2_fpga_table_24_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_24_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_24_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_24_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_24_ADDR,localVal);
}


#define RF2_FPGA_TABLE_25_ADDR (RF2_FPGA_BASE + 0x00000684)

static inline uint32_t rf2_fpga_table_25_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_25_ADDR);
}

static inline void rf2_fpga_table_25_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_25_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_25_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_25_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_25_ADDR,localVal);
}


#define RF2_FPGA_TABLE_26_ADDR (RF2_FPGA_BASE + 0x00000688)

static inline uint32_t rf2_fpga_table_26_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_26_ADDR);
}

static inline void rf2_fpga_table_26_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_26_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_26_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_26_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_26_ADDR,localVal);
}


#define RF2_FPGA_TABLE_27_ADDR (RF2_FPGA_BASE + 0x0000068c)

static inline uint32_t rf2_fpga_table_27_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_27_ADDR);
}

static inline void rf2_fpga_table_27_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_27_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_27_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_27_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_27_ADDR,localVal);
}


#define RF2_FPGA_TABLE_28_ADDR (RF2_FPGA_BASE + 0x00000690)

static inline uint32_t rf2_fpga_table_28_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_28_ADDR);
}

static inline void rf2_fpga_table_28_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_28_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_28_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_28_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_28_ADDR,localVal);
}


#define RF2_FPGA_TABLE_29_ADDR (RF2_FPGA_BASE + 0x00000694)

static inline uint32_t rf2_fpga_table_29_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_29_ADDR);
}

static inline void rf2_fpga_table_29_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_29_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_29_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_29_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_29_ADDR,localVal);
}


#define RF2_FPGA_TABLE_30_ADDR (RF2_FPGA_BASE + 0x00000698)

static inline uint32_t rf2_fpga_table_30_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_30_ADDR);
}

static inline void rf2_fpga_table_30_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_30_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_30_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_30_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_30_ADDR,localVal);
}


#define RF2_FPGA_TABLE_31_ADDR (RF2_FPGA_BASE + 0x0000069c)

static inline uint32_t rf2_fpga_table_31_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_31_ADDR);
}

static inline void rf2_fpga_table_31_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_31_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_31_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_31_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_31_ADDR,localVal);
}


#define RF2_FPGA_TABLE_32_ADDR (RF2_FPGA_BASE + 0x000006a0)

static inline uint32_t rf2_fpga_table_32_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_32_ADDR);
}

static inline void rf2_fpga_table_32_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_32_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_32_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_32_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_32_ADDR,localVal);
}


#define RF2_FPGA_TABLE_33_ADDR (RF2_FPGA_BASE + 0x000006a4)

static inline uint32_t rf2_fpga_table_33_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_33_ADDR);
}

static inline void rf2_fpga_table_33_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_33_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_33_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_33_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_33_ADDR,localVal);
}


#define RF2_FPGA_TABLE_34_ADDR (RF2_FPGA_BASE + 0x000006a8)

static inline uint32_t rf2_fpga_table_34_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_34_ADDR);
}

static inline void rf2_fpga_table_34_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_34_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_34_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_34_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_34_ADDR,localVal);
}


#define RF2_FPGA_TABLE_35_ADDR (RF2_FPGA_BASE + 0x000006ac)

static inline uint32_t rf2_fpga_table_35_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_35_ADDR);
}

static inline void rf2_fpga_table_35_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_35_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_35_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_35_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_35_ADDR,localVal);
}


#define RF2_FPGA_TABLE_36_ADDR (RF2_FPGA_BASE + 0x000006b0)

static inline uint32_t rf2_fpga_table_36_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_36_ADDR);
}

static inline void rf2_fpga_table_36_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_36_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_36_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_36_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_36_ADDR,localVal);
}


#define RF2_FPGA_TABLE_37_ADDR (RF2_FPGA_BASE + 0x000006b4)

static inline uint32_t rf2_fpga_table_37_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_37_ADDR);
}

static inline void rf2_fpga_table_37_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_37_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_37_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_37_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_37_ADDR,localVal);
}


#define RF2_FPGA_TABLE_38_ADDR (RF2_FPGA_BASE + 0x000006b8)

static inline uint32_t rf2_fpga_table_38_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_38_ADDR);
}

static inline void rf2_fpga_table_38_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_38_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_38_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_38_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_38_ADDR,localVal);
}


#define RF2_FPGA_TABLE_39_ADDR (RF2_FPGA_BASE + 0x000006bc)

static inline uint32_t rf2_fpga_table_39_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_39_ADDR);
}

static inline void rf2_fpga_table_39_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_39_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_39_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_39_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_39_ADDR,localVal);
}


#define RF2_FPGA_TABLE_40_ADDR (RF2_FPGA_BASE + 0x000006c0)

static inline uint32_t rf2_fpga_table_40_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_40_ADDR);
}

static inline void rf2_fpga_table_40_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_40_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_40_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_40_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_40_ADDR,localVal);
}


#define RF2_FPGA_TABLE_41_ADDR (RF2_FPGA_BASE + 0x000006c4)

static inline uint32_t rf2_fpga_table_41_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_41_ADDR);
}

static inline void rf2_fpga_table_41_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_41_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_41_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_41_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_41_ADDR,localVal);
}


#define RF2_FPGA_TABLE_42_ADDR (RF2_FPGA_BASE + 0x000006c8)

static inline uint32_t rf2_fpga_table_42_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_42_ADDR);
}

static inline void rf2_fpga_table_42_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_42_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_42_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_42_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_42_ADDR,localVal);
}


#define RF2_FPGA_TABLE_43_ADDR (RF2_FPGA_BASE + 0x000006cc)

static inline uint32_t rf2_fpga_table_43_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_43_ADDR);
}

static inline void rf2_fpga_table_43_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_43_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_43_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_43_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_43_ADDR,localVal);
}


#define RF2_FPGA_TABLE_44_ADDR (RF2_FPGA_BASE + 0x000006d0)

static inline uint32_t rf2_fpga_table_44_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_44_ADDR);
}

static inline void rf2_fpga_table_44_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_44_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_44_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_44_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_44_ADDR,localVal);
}


#define RF2_FPGA_TABLE_45_ADDR (RF2_FPGA_BASE + 0x000006d4)

static inline uint32_t rf2_fpga_table_45_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_45_ADDR);
}

static inline void rf2_fpga_table_45_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_45_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_45_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_45_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_45_ADDR,localVal);
}


#define RF2_FPGA_TABLE_46_ADDR (RF2_FPGA_BASE + 0x000006d8)

static inline uint32_t rf2_fpga_table_46_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_46_ADDR);
}

static inline void rf2_fpga_table_46_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_46_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_46_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_46_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_46_ADDR,localVal);
}


#define RF2_FPGA_TABLE_47_ADDR (RF2_FPGA_BASE + 0x000006dc)

static inline uint32_t rf2_fpga_table_47_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_47_ADDR);
}

static inline void rf2_fpga_table_47_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_47_ADDR,x);
}

static inline uint32_t rf2_fpga_sdmin_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_47_ADDR);
    return ((localVal & ((uint32_t)0x3fffffff)) >> 0);
}

static inline void rf2_fpga_sdmin_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_47_ADDR);
    localVal &= ~((uint32_t)0x3fffffff);
    localVal |= (x << 0)&((uint32_t)0x3fffffff);
    REG_PL_WR(RF2_FPGA_TABLE_47_ADDR,localVal);
}


#define RF2_FPGA_TABLE_48_ADDR (RF2_FPGA_BASE + 0x000006e0)

static inline uint32_t rf2_fpga_table_48_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
}

static inline void rf2_fpga_table_48_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_48_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_48_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_48_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_48_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_48_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_48_ADDR,localVal);
}


#define RF2_FPGA_TABLE_49_ADDR (RF2_FPGA_BASE + 0x000006e4)

static inline uint32_t rf2_fpga_table_49_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
}

static inline void rf2_fpga_table_49_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_49_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_49_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_49_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_49_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_49_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_49_ADDR,localVal);
}


#define RF2_FPGA_TABLE_50_ADDR (RF2_FPGA_BASE + 0x000006e8)

static inline uint32_t rf2_fpga_table_50_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
}

static inline void rf2_fpga_table_50_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_50_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_50_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_50_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_50_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_50_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_50_ADDR,localVal);
}


#define RF2_FPGA_TABLE_51_ADDR (RF2_FPGA_BASE + 0x000006ec)

static inline uint32_t rf2_fpga_table_51_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
}

static inline void rf2_fpga_table_51_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_51_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_51_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_51_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_51_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_51_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_51_ADDR,localVal);
}


#define RF2_FPGA_TABLE_52_ADDR (RF2_FPGA_BASE + 0x000006f0)

static inline uint32_t rf2_fpga_table_52_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
}

static inline void rf2_fpga_table_52_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_52_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_52_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_52_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_52_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_52_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_52_ADDR,localVal);
}


#define RF2_FPGA_TABLE_53_ADDR (RF2_FPGA_BASE + 0x000006f4)

static inline uint32_t rf2_fpga_table_53_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
}

static inline void rf2_fpga_table_53_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_53_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_53_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_53_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_53_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_53_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_53_ADDR,localVal);
}


#define RF2_FPGA_TABLE_54_ADDR (RF2_FPGA_BASE + 0x000006f8)

static inline uint32_t rf2_fpga_table_54_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
}

static inline void rf2_fpga_table_54_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_54_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_54_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_54_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_54_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_54_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_54_ADDR,localVal);
}


#define RF2_FPGA_TABLE_55_ADDR (RF2_FPGA_BASE + 0x000006fc)

static inline uint32_t rf2_fpga_table_55_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
}

static inline void rf2_fpga_table_55_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_55_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_55_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_55_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_55_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_55_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_55_ADDR,localVal);
}


#define RF2_FPGA_TABLE_56_ADDR (RF2_FPGA_BASE + 0x00000700)

static inline uint32_t rf2_fpga_table_56_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
}

static inline void rf2_fpga_table_56_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_56_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_56_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_56_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_56_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_56_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_56_ADDR,localVal);
}


#define RF2_FPGA_TABLE_57_ADDR (RF2_FPGA_BASE + 0x00000704)

static inline uint32_t rf2_fpga_table_57_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
}

static inline void rf2_fpga_table_57_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_57_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_57_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_57_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_57_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_57_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_57_ADDR,localVal);
}


#define RF2_FPGA_TABLE_58_ADDR (RF2_FPGA_BASE + 0x00000708)

static inline uint32_t rf2_fpga_table_58_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
}

static inline void rf2_fpga_table_58_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_58_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_58_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_58_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_58_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_58_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_58_ADDR,localVal);
}


#define RF2_FPGA_TABLE_59_ADDR (RF2_FPGA_BASE + 0x0000070c)

static inline uint32_t rf2_fpga_table_59_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
}

static inline void rf2_fpga_table_59_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_59_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_59_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_59_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_59_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_59_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_59_ADDR,localVal);
}


#define RF2_FPGA_CTRL_8_ADDR (RF2_FPGA_BASE + 0x00000710)

static inline uint32_t rf2_fpga_ctrl_8_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_8_ADDR);
}

static inline void rf2_fpga_ctrl_8_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_8_ADDR,x);
}

static inline uint32_t rf2_fpga_cmd_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_8_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf2_fpga_cmd_enable_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_8_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(RF2_FPGA_CTRL_8_ADDR,localVal);
}


#define RF2_FPGA_TABLE_60_ADDR (RF2_FPGA_BASE + 0x00000714)

static inline uint32_t rf2_fpga_table_60_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
}

static inline void rf2_fpga_table_60_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_60_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_60_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_60_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_60_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_60_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_60_ADDR,localVal);
}


#define RF2_FPGA_TABLE_61_ADDR (RF2_FPGA_BASE + 0x00000718)

static inline uint32_t rf2_fpga_table_61_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
}

static inline void rf2_fpga_table_61_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_61_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_61_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_61_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_61_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_61_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_61_ADDR,localVal);
}


#define RF2_FPGA_TABLE_62_ADDR (RF2_FPGA_BASE + 0x0000071c)

static inline uint32_t rf2_fpga_table_62_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
}

static inline void rf2_fpga_table_62_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_62_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_62_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_62_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_62_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_62_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_62_ADDR,localVal);
}


#define RF2_FPGA_TABLE_63_ADDR (RF2_FPGA_BASE + 0x00000720)

static inline uint32_t rf2_fpga_table_63_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
}

static inline void rf2_fpga_table_63_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_63_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_63_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_63_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_63_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_63_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_63_ADDR,localVal);
}


#define RF2_FPGA_TABLE_64_ADDR (RF2_FPGA_BASE + 0x00000724)

static inline uint32_t rf2_fpga_table_64_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
}

static inline void rf2_fpga_table_64_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_64_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_64_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_64_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_64_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_64_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_64_ADDR,localVal);
}


#define RF2_FPGA_TABLE_65_ADDR (RF2_FPGA_BASE + 0x00000728)

static inline uint32_t rf2_fpga_table_65_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
}

static inline void rf2_fpga_table_65_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_65_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_65_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_65_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_65_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_65_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_65_ADDR,localVal);
}


#define RF2_FPGA_TABLE_66_ADDR (RF2_FPGA_BASE + 0x0000072c)

static inline uint32_t rf2_fpga_table_66_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
}

static inline void rf2_fpga_table_66_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_66_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_66_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_66_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_66_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_66_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_66_ADDR,localVal);
}


#define RF2_FPGA_TABLE_67_ADDR (RF2_FPGA_BASE + 0x00000730)

static inline uint32_t rf2_fpga_table_67_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
}

static inline void rf2_fpga_table_67_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_67_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_67_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_67_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_67_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_67_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_67_ADDR,localVal);
}


#define RF2_FPGA_TABLE_68_ADDR (RF2_FPGA_BASE + 0x00000734)

static inline uint32_t rf2_fpga_table_68_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
}

static inline void rf2_fpga_table_68_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_68_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_68_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_68_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_68_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_68_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_68_ADDR,localVal);
}


#define RF2_FPGA_TABLE_69_ADDR (RF2_FPGA_BASE + 0x00000738)

static inline uint32_t rf2_fpga_table_69_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
}

static inline void rf2_fpga_table_69_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_69_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_69_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_69_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_69_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_69_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_69_ADDR,localVal);
}


#define RF2_FPGA_TABLE_70_ADDR (RF2_FPGA_BASE + 0x0000073c)

static inline uint32_t rf2_fpga_table_70_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
}

static inline void rf2_fpga_table_70_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_70_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_70_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_70_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_70_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_70_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_70_ADDR,localVal);
}


#define RF2_FPGA_TABLE_71_ADDR (RF2_FPGA_BASE + 0x00000740)

static inline uint32_t rf2_fpga_table_71_get(void)
{
    return REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
}

static inline void rf2_fpga_table_71_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_TABLE_71_ADDR,x);
}

static inline uint32_t rf2_fpga_freq_cw_rx_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void rf2_fpga_freq_cw_rx_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(RF2_FPGA_TABLE_71_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void rf2_fpga_freq_cw_rx_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(RF2_FPGA_TABLE_71_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void rf2_fpga_freq_cw_rx_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(RF2_FPGA_TABLE_71_ADDR,localVal);
}

static inline uint32_t rf2_fpga_freq_cw_rx_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void rf2_fpga_freq_cw_rx_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_TABLE_71_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(RF2_FPGA_TABLE_71_ADDR,localVal);
}


#define RF2_FPGA_CTRL_10_ADDR (RF2_FPGA_BASE + 0x00000744)

static inline uint32_t rf2_fpga_ctrl_10_get(void)
{
    return REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
}

static inline void rf2_fpga_ctrl_10_set(uint32_t x)
{
    REG_PL_WR(RF2_FPGA_CTRL_10_ADDR,x);
}

static inline uint32_t rf2_fpga_st_cci_overlap_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t rf2_fpga_st_state_diff_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t rf2_fpga_cci_dbg_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf2_fpga_cci_dbg_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(RF2_FPGA_CTRL_10_ADDR,localVal);
}

static inline uint32_t rf2_fpga_state_diff_cnt_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rf2_fpga_state_diff_cnt_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF2_FPGA_CTRL_10_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(RF2_FPGA_CTRL_10_ADDR,localVal);
}

#endif