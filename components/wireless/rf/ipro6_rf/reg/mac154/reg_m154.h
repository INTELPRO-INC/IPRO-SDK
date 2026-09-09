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

#ifndef _REG_M154_H_
#define _REG_M154_H_
// Date: 20210607

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef M154_BASE
#define M154_BASE (0x20000000)
#endif



#define M154_GLB_CFG_ADDR (M154_BASE + 0x00000000)

static inline uint32_t m154_glb_cfg_get(void)
{
    return REG_PL_RD(M154_GLB_CFG_ADDR);
}

static inline void m154_glb_cfg_set(uint32_t x)
{
    REG_PL_WR(M154_GLB_CFG_ADDR,x);
}

static inline uint32_t m154_rf_center_freq_reg_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline void m154_rf_center_freq_reg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x0fff0000);
    localVal |= (x << 16)&((uint32_t)0x0fff0000);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}

static inline uint32_t m154_std_version_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void m154_std_version_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12)&((uint32_t)0x00003000);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}

static inline uint32_t m154_clk_xtal_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000003f0)) >> 4);
}

static inline void m154_clk_xtal_freq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x000003f0);
    localVal |= (x << 4)&((uint32_t)0x000003f0);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}

static inline uint32_t m154_zben_protect_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_zben_protect_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}

static inline uint32_t m154_beacon_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_beacon_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}

static inline uint32_t m154_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GLB_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_GLB_CFG_ADDR,localVal);
}


#define M154_DATA_CFG_ADDR (M154_BASE + 0x00000004)

static inline uint32_t m154_data_cfg_get(void)
{
    return REG_PL_RD(M154_DATA_CFG_ADDR);
}

static inline void m154_data_cfg_set(uint32_t x)
{
    REG_PL_WR(M154_DATA_CFG_ADDR,x);
}

static inline uint32_t m154_tx_data_fault_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_tx_data_fault_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}

static inline uint32_t m154_rx_data_fault_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_rx_data_fault_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}

static inline uint32_t m154_tx_crc_fault_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_tx_crc_fault_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}

static inline uint32_t m154_rx_crc_fault_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_rx_crc_fault_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}

static inline uint32_t m154_tx_sym_bit_reverse_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_tx_sym_bit_reverse_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}

static inline uint32_t m154_rx_sym_bit_reverse_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_rx_sym_bit_reverse_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DATA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_DATA_CFG_ADDR,localVal);
}


#define M154_SWRST_ADDR (M154_BASE + 0x00000008)

static inline uint32_t m154_swrst_get(void)
{
    return REG_PL_RD(M154_SWRST_ADDR);
}

static inline void m154_swrst_set(uint32_t x)
{
    REG_PL_WR(M154_SWRST_ADDR,x);
}

static inline uint32_t m154_tx_crc_sw_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_tx_crc_sw_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_rx_crc_sw_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_rx_crc_sw_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_bist_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_soft_reset_bist_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_csma_ca_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_soft_reset_csma_ca_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_tx_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_soft_reset_tx_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_rx_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_soft_reset_rx_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_tcon_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_soft_reset_tcon_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}

static inline uint32_t m154_soft_reset_regs_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_soft_reset_regs_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SWRST_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_SWRST_ADDR,localVal);
}


#define M154_DBG_SEL_ADDR (M154_BASE + 0x0000000c)

static inline uint32_t m154_dbg_sel_get(void)
{
    return REG_PL_RD(M154_DBG_SEL_ADDR);
}

static inline void m154_dbg_sel_set(uint32_t x)
{
    REG_PL_WR(M154_DBG_SEL_ADDR,x);
}

static inline uint32_t m154_debug_out_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DBG_SEL_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void m154_debug_out_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DBG_SEL_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(M154_DBG_SEL_ADDR,localVal);
}


#define M154_DBG_INT_EN_ADDR (M154_BASE + 0x00000010)

static inline uint32_t m154_dbg_int_en_get(void)
{
    return REG_PL_RD(M154_DBG_INT_EN_ADDR);
}

static inline void m154_dbg_int_en_set(uint32_t x)
{
    REG_PL_WR(M154_DBG_INT_EN_ADDR,x);
}

static inline uint32_t m154_debug_int_raw_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_DBG_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_debug_int_raw_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_DBG_INT_EN_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_DBG_INT_EN_ADDR,localVal);
}


#define M154_BIST_CTRL_ADDR (M154_BASE + 0x00000014)

static inline uint32_t m154_bist_ctrl_get(void)
{
    return REG_PL_RD(M154_BIST_CTRL_ADDR);
}

static inline void m154_bist_ctrl_set(uint32_t x)
{
    REG_PL_WR(M154_BIST_CTRL_ADDR,x);
}

static inline uint32_t m154_bist_auto_ack_doing_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_bist_doing_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_bist_pattern_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void m154_bist_pattern_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_periodic_beacon_order_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void m154_bist_periodic_beacon_order_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_bist_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_ext_dut_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_bist_ext_dut_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_periodic_beacon_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_bist_periodic_beacon_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_periodic_beacon_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_bist_periodic_beacon_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_auto_ack_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_bist_auto_ack_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_pattern_ar_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_bist_pattern_ar_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_pattern_bp_bound_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_bist_pattern_bp_bound_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}

static inline uint32_t m154_bist_pattern_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_bist_pattern_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_BIST_CTRL_ADDR,localVal);
}


#define M154_BIST_DLY_ADDR (M154_BASE + 0x00000018)

static inline uint32_t m154_bist_dly_get(void)
{
    return REG_PL_RD(M154_BIST_DLY_ADDR);
}

static inline void m154_bist_dly_set(uint32_t x)
{
    REG_PL_WR(M154_BIST_DLY_ADDR,x);
}

static inline uint32_t m154_bist_pattern_trig_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_DLY_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_bist_pattern_trig_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_BIST_DLY_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_BIST_DLY_ADDR,localVal);
}


#define M154_INT_EN_ADDR (M154_BASE + 0x00000020)

static inline uint32_t m154_int_en_get(void)
{
    return REG_PL_RD(M154_INT_EN_ADDR);
}

static inline void m154_int_en_set(uint32_t x)
{
    REG_PL_WR(M154_INT_EN_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void m154_tx_abort_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_abort_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void m154_rx_abort_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_done_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void m154_tx_auto_ack_done_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_tx_done_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_tx_done_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_timeout_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void m154_csma_ca_timeout_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_failure_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void m154_csma_ca_failure_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_success_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void m154_csma_ca_success_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_others_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void m154_rx_end_ack_others_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_cmd_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void m154_rx_end_cmd_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void m154_rx_end_ack_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_data_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void m154_rx_end_data_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_beacon_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void m154_rx_end_beacon_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_filter_fail_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void m154_rx_filter_fail_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_security_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void m154_rx_mhr_security_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_rx_mhr_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_start_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_start_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_cs_alert_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void m154_cs_alert_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_nbr_search_fail_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void m154_rx_nbr_search_fail_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_mpp_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void m154_rx_end_mpp_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ext_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void m154_rx_end_ext_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_req_enh_ack_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void m154_rx_end_req_enh_ack_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_rx_end_frak_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void m154_rx_end_frak_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_09_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void m154_alarm_09_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_08_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_alarm_08_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_07_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_alarm_07_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_06_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_alarm_06_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_05_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_alarm_05_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_04_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_alarm_04_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_03_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_alarm_03_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_02_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_alarm_02_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_01_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_alarm_01_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}

static inline uint32_t m154_alarm_00_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_alarm_00_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_EN_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_INT_EN_ADDR,localVal);
}


#define M154_INT_CLR_ADDR (M154_BASE + 0x00000024)

static inline uint32_t m154_int_clr_get(void)
{
    return REG_PL_RD(M154_INT_CLR_ADDR);
}

static inline void m154_int_clr_set(uint32_t x)
{
    REG_PL_WR(M154_INT_CLR_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void m154_tx_abort_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_abort_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void m154_rx_abort_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_done_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void m154_tx_auto_ack_done_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_tx_done_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_tx_done_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_timeout_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void m154_csma_ca_timeout_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_failure_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void m154_csma_ca_failure_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_success_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void m154_csma_ca_success_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_others_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void m154_rx_end_ack_others_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_cmd_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void m154_rx_end_cmd_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void m154_rx_end_ack_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_data_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void m154_rx_end_data_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_beacon_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void m154_rx_end_beacon_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_filter_fail_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void m154_rx_filter_fail_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_security_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void m154_rx_mhr_security_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_rx_mhr_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_start_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_start_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_cs_alert_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void m154_cs_alert_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_nbr_search_fail_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void m154_rx_nbr_search_fail_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_mpp_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void m154_rx_end_mpp_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ext_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void m154_rx_end_ext_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_req_enh_ack_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void m154_rx_end_req_enh_ack_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_rx_end_frak_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void m154_rx_end_frak_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_09_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void m154_alarm_09_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_08_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_alarm_08_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_07_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_alarm_07_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_06_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_alarm_06_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_05_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_alarm_05_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_04_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_alarm_04_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_03_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_alarm_03_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_02_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_alarm_02_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_01_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_alarm_01_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}

static inline uint32_t m154_alarm_00_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_alarm_00_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_CLR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_INT_CLR_ADDR,localVal);
}


#define M154_INT_MASK_ADDR (M154_BASE + 0x00000028)

static inline uint32_t m154_int_mask_get(void)
{
    return REG_PL_RD(M154_INT_MASK_ADDR);
}

static inline void m154_int_mask_set(uint32_t x)
{
    REG_PL_WR(M154_INT_MASK_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void m154_tx_abort_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_abort_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void m154_rx_abort_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_done_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void m154_tx_auto_ack_done_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_tx_done_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_tx_done_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_timeout_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void m154_csma_ca_timeout_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_failure_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void m154_csma_ca_failure_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26)&((uint32_t)0x04000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_success_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void m154_csma_ca_success_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25)&((uint32_t)0x02000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_others_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void m154_rx_end_ack_others_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_cmd_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void m154_rx_end_cmd_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ack_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void m154_rx_end_ack_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_data_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void m154_rx_end_data_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_beacon_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void m154_rx_end_beacon_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_filter_fail_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void m154_rx_filter_fail_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_security_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void m154_rx_mhr_security_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18)&((uint32_t)0x00040000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_mhr_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_rx_mhr_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_start_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_start_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_cs_alert_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void m154_cs_alert_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_nbr_search_fail_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void m154_rx_nbr_search_fail_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_mpp_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void m154_rx_end_mpp_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_ext_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void m154_rx_end_ext_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_req_enh_ack_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void m154_rx_end_req_enh_ack_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_rx_end_frak_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void m154_rx_end_frak_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_09_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void m154_alarm_09_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_08_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_alarm_08_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_07_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_alarm_07_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_06_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_alarm_06_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_05_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_alarm_05_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_04_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_alarm_04_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_03_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_alarm_03_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_02_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_alarm_02_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_01_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_alarm_01_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}

static inline uint32_t m154_alarm_00_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_alarm_00_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_INT_MASK_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_INT_MASK_ADDR,localVal);
}


#define M154_INT_RAW_ADDR (M154_BASE + 0x0000002c)

static inline uint32_t m154_int_raw_get(void)
{
    return REG_PL_RD(M154_INT_RAW_ADDR);
}

static inline void m154_int_raw_set(uint32_t x)
{
    REG_PL_WR(M154_INT_RAW_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t m154_rx_abort_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t m154_tx_auto_ack_done_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t m154_tx_done_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t m154_csma_ca_timeout_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t m154_csma_ca_failure_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t m154_csma_ca_success_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t m154_rx_end_ack_others_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t m154_rx_end_cmd_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t m154_rx_end_ack_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t m154_rx_end_data_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t m154_rx_end_beacon_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t m154_rx_filter_fail_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t m154_rx_mhr_security_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t m154_rx_mhr_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_rx_start_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_cs_alert_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t m154_rx_nbr_search_fail_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t m154_rx_end_mpp_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t m154_rx_end_ext_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t m154_rx_end_req_enh_ack_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t m154_rx_end_frak_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t m154_alarm_09_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t m154_alarm_08_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t m154_alarm_07_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t m154_alarm_06_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t m154_alarm_05_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t m154_alarm_04_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t m154_alarm_03_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t m154_alarm_02_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t m154_alarm_01_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_alarm_00_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_RAW_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define M154_INT_ADDR (M154_BASE + 0x00000030)

static inline uint32_t m154_int_get(void)
{
    return REG_PL_RD(M154_INT_ADDR);
}

static inline void m154_int_set(uint32_t x)
{
    REG_PL_WR(M154_INT_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline uint32_t m154_rx_abort_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline uint32_t m154_tx_auto_ack_done_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t m154_tx_done_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t m154_csma_ca_timeout_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline uint32_t m154_csma_ca_failure_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline uint32_t m154_csma_ca_success_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline uint32_t m154_rx_end_ack_others_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline uint32_t m154_rx_end_cmd_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline uint32_t m154_rx_end_ack_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t m154_rx_end_data_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t m154_rx_end_beacon_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t m154_rx_filter_fail_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t m154_rx_mhr_security_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t m154_rx_mhr_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_rx_start_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_cs_alert_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline uint32_t m154_rx_nbr_search_fail_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline uint32_t m154_rx_end_mpp_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline uint32_t m154_rx_end_ext_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t m154_rx_end_req_enh_ack_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t m154_rx_end_frak_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t m154_alarm_09_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t m154_alarm_08_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t m154_alarm_07_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t m154_alarm_06_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t m154_alarm_05_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t m154_alarm_04_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t m154_alarm_03_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t m154_alarm_02_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t m154_alarm_01_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_alarm_00_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_INT_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define M154_GCI_CTRL0_ADDR (M154_BASE + 0x00000034)

static inline uint32_t m154_gci_ctrl0_get(void)
{
    return REG_PL_RD(M154_GCI_CTRL0_ADDR);
}

static inline void m154_gci_ctrl0_set(uint32_t x)
{
    REG_PL_WR(M154_GCI_CTRL0_ADDR,x);
}

static inline uint32_t m154_gci_int_sync_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t m154_gci_ctrl_sync_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline uint32_t m154_gci_ctrl_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline uint32_t m154_gci_ctrl_fw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void m154_gci_ctrl_fw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(M154_GCI_CTRL0_ADDR,localVal);
}

static inline uint32_t m154_gci_ctrl_fw_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void m154_gci_ctrl_fw_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(M154_GCI_CTRL0_ADDR,localVal);
}


#define M154_GCI_CTRL1_ADDR (M154_BASE + 0x00000038)

static inline uint32_t m154_gci_ctrl1_get(void)
{
    return REG_PL_RD(M154_GCI_CTRL1_ADDR);
}

static inline void m154_gci_ctrl1_set(uint32_t x)
{
    REG_PL_WR(M154_GCI_CTRL1_ADDR,x);
}

static inline uint32_t m154_gci_int_dual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_gci_int_dual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_GCI_CTRL1_ADDR,localVal);
}

static inline uint32_t m154_gci_tx_confx_int_dual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_gci_tx_confx_int_dual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_GCI_CTRL1_ADDR,localVal);
}

static inline uint32_t m154_gci_tx_confx_cancel_tx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_gci_tx_confx_cancel_tx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_GCI_CTRL1_ADDR,localVal);
}

static inline uint32_t m154_gci_tx_confx_cancel_rx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_gci_tx_confx_cancel_rx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_GCI_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_GCI_CTRL1_ADDR,localVal);
}


#define M154_SYM_CNT_ADDR (M154_BASE + 0x00000040)

static inline uint32_t m154_sym_cnt_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_ADDR);
}

static inline void m154_sym_cnt_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_ADDR,x);
}

static inline uint32_t m154_sym_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_FRC_ADDR (M154_BASE + 0x00000044)

static inline uint32_t m154_sym_cnt_frc_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_FRC_ADDR);
}

static inline void m154_sym_cnt_frc_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_FRC_ADDR,x);
}

static inline uint32_t m154_sym_cnt_rx_beacon_p2_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_FRC_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t m154_sym_cnt_rx_beacon_p1_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_FRC_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t m154_sym_cnt_frc_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_FRC_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}


#define M154_SYM_CNT_RX_BEACON_P1_ADDR (M154_BASE + 0x00000048)

static inline uint32_t m154_sym_cnt_rx_beacon_p1_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_RX_BEACON_P1_ADDR);
}

static inline void m154_sym_cnt_rx_beacon_p1_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_RX_BEACON_P1_ADDR,x);
}

static inline uint32_t m154_sym_cnt_rx_beacon_p1_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_RX_BEACON_P1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_RX_BEACON_P2_ADDR (M154_BASE + 0x0000004c)

static inline uint32_t m154_sym_cnt_rx_beacon_p2_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_RX_BEACON_P2_ADDR);
}

static inline void m154_sym_cnt_rx_beacon_p2_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_RX_BEACON_P2_ADDR,x);
}

static inline uint32_t m154_sym_cnt_rx_beacon_p2_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_RX_BEACON_P2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_RX_START_ADDR (M154_BASE + 0x00000050)

static inline uint32_t m154_sym_cnt_rx_start_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_RX_START_ADDR);
}

static inline void m154_sym_cnt_rx_start_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_RX_START_ADDR,x);
}

static inline uint32_t m154_sym_cnt_rx_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_RX_START_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_RX_END_ADDR (M154_BASE + 0x00000054)

static inline uint32_t m154_sym_cnt_rx_end_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_RX_END_ADDR);
}

static inline void m154_sym_cnt_rx_end_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_RX_END_ADDR,x);
}

static inline uint32_t m154_sym_cnt_rx_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_RX_END_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_CSMA_CA_START_ADDR (M154_BASE + 0x00000058)

static inline uint32_t m154_sym_cnt_csma_ca_start_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_CSMA_CA_START_ADDR);
}

static inline void m154_sym_cnt_csma_ca_start_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_CSMA_CA_START_ADDR,x);
}

static inline uint32_t m154_sym_cnt_csma_ca_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_CSMA_CA_START_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_TX_BEACON_ADDR (M154_BASE + 0x0000005c)

static inline uint32_t m154_sym_cnt_tx_beacon_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_TX_BEACON_ADDR);
}

static inline void m154_sym_cnt_tx_beacon_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_TX_BEACON_ADDR,x);
}

static inline uint32_t m154_sym_cnt_tx_beacon_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_TX_BEACON_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_SYM_CNT_TX_END_ADDR (M154_BASE + 0x00000060)

static inline uint32_t m154_sym_cnt_tx_end_get(void)
{
    return REG_PL_RD(M154_SYM_CNT_TX_END_ADDR);
}

static inline void m154_sym_cnt_tx_end_set(uint32_t x)
{
    REG_PL_WR(M154_SYM_CNT_TX_END_ADDR,x);
}

static inline uint32_t m154_sym_cnt_tx_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SYM_CNT_TX_END_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_ALARM_00_TARGET_ADDR (M154_BASE + 0x00000064)

static inline uint32_t m154_alarm_00_target_get(void)
{
    return REG_PL_RD(M154_ALARM_00_TARGET_ADDR);
}

static inline void m154_alarm_00_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_00_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_00_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_00_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_00_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_00_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_00_TARGET_ADDR,localVal);
}


#define M154_ALARM_01_TARGET_ADDR (M154_BASE + 0x00000068)

static inline uint32_t m154_alarm_01_target_get(void)
{
    return REG_PL_RD(M154_ALARM_01_TARGET_ADDR);
}

static inline void m154_alarm_01_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_01_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_01_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_01_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_01_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_01_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_01_TARGET_ADDR,localVal);
}


#define M154_ALARM_02_TARGET_ADDR (M154_BASE + 0x0000006c)

static inline uint32_t m154_alarm_02_target_get(void)
{
    return REG_PL_RD(M154_ALARM_02_TARGET_ADDR);
}

static inline void m154_alarm_02_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_02_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_02_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_02_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_02_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_02_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_02_TARGET_ADDR,localVal);
}


#define M154_ALARM_03_TARGET_ADDR (M154_BASE + 0x00000070)

static inline uint32_t m154_alarm_03_target_get(void)
{
    return REG_PL_RD(M154_ALARM_03_TARGET_ADDR);
}

static inline void m154_alarm_03_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_03_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_03_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_03_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_03_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_03_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_03_TARGET_ADDR,localVal);
}


#define M154_ALARM_04_TARGET_ADDR (M154_BASE + 0x00000074)

static inline uint32_t m154_alarm_04_target_get(void)
{
    return REG_PL_RD(M154_ALARM_04_TARGET_ADDR);
}

static inline void m154_alarm_04_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_04_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_04_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_04_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_04_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_04_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_04_TARGET_ADDR,localVal);
}


#define M154_ALARM_05_TARGET_ADDR (M154_BASE + 0x00000078)

static inline uint32_t m154_alarm_05_target_get(void)
{
    return REG_PL_RD(M154_ALARM_05_TARGET_ADDR);
}

static inline void m154_alarm_05_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_05_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_05_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_05_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_05_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_05_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_05_TARGET_ADDR,localVal);
}


#define M154_ALARM_06_TARGET_ADDR (M154_BASE + 0x0000007c)

static inline uint32_t m154_alarm_06_target_get(void)
{
    return REG_PL_RD(M154_ALARM_06_TARGET_ADDR);
}

static inline void m154_alarm_06_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_06_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_06_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_06_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_06_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_06_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_06_TARGET_ADDR,localVal);
}


#define M154_ALARM_07_TARGET_ADDR (M154_BASE + 0x00000080)

static inline uint32_t m154_alarm_07_target_get(void)
{
    return REG_PL_RD(M154_ALARM_07_TARGET_ADDR);
}

static inline void m154_alarm_07_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_07_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_07_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_07_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_07_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_07_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_07_TARGET_ADDR,localVal);
}


#define M154_ALARM_08_TARGET_ADDR (M154_BASE + 0x00000084)

static inline uint32_t m154_alarm_08_target_get(void)
{
    return REG_PL_RD(M154_ALARM_08_TARGET_ADDR);
}

static inline void m154_alarm_08_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_08_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_08_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_08_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_08_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_08_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_08_TARGET_ADDR,localVal);
}


#define M154_ALARM_09_TARGET_ADDR (M154_BASE + 0x00000088)

static inline uint32_t m154_alarm_09_target_get(void)
{
    return REG_PL_RD(M154_ALARM_09_TARGET_ADDR);
}

static inline void m154_alarm_09_target_set(uint32_t x)
{
    REG_PL_WR(M154_ALARM_09_TARGET_ADDR,x);
}

static inline uint32_t m154_alarm_09_target_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_09_TARGET_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_alarm_09_target_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_ALARM_09_TARGET_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_ALARM_09_TARGET_ADDR,localVal);
}


#define M154_CSMA_0_ADDR (M154_BASE + 0x00000090)

static inline uint32_t m154_csma_0_get(void)
{
    return REG_PL_RD(M154_CSMA_0_ADDR);
}

static inline void m154_csma_0_set(uint32_t x)
{
    REG_PL_WR(M154_CSMA_0_ADDR,x);
}

static inline uint32_t m154_backoff_rx_dis_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void m154_backoff_rx_dis_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_cca_timeout_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_csma_ca_cca_timeout_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_transaction_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void m154_csma_ca_transaction_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16)&((uint32_t)0x01ff0000);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_cca_en_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void m154_csma_ca_cca_en_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_cw0_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void m154_cw0_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_sw_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_csma_ca_sw_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_timeout_check_option_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_csma_ca_timeout_check_option_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_slot_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_csma_ca_slot_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_ecr_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_csma_ca_ecr_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_eb_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_csma_ca_eb_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_ble_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_ble_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_csma_ca_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_CSMA_0_ADDR,localVal);
}


#define M154_CSMA_1_ADDR (M154_BASE + 0x00000094)

static inline uint32_t m154_csma_1_get(void)
{
    return REG_PL_RD(M154_CSMA_1_ADDR);
}

static inline void m154_csma_1_set(uint32_t x)
{
    REG_PL_WR(M154_CSMA_1_ADDR,x);
}

static inline uint32_t m154_csma_ca_avg_pkt_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void m154_csma_ca_avg_pkt_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_max_csma_backoffs_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void m154_max_csma_backoffs_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_max_be_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void m154_max_be_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_min_be_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void m154_min_be_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_plus_be_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void m154_plus_be_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12)&((uint32_t)0x00007000);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_minus_be_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void m154_minus_be_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8)&((uint32_t)0x00000700);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_manual_be_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_manual_be_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}

static inline uint32_t m154_manual_be_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void m154_manual_be_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_1_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(M154_CSMA_1_ADDR,localVal);
}


#define M154_CSMA_2_ADDR (M154_BASE + 0x00000098)

static inline uint32_t m154_csma_2_get(void)
{
    return REG_PL_RD(M154_CSMA_2_ADDR);
}

static inline void m154_csma_2_set(uint32_t x)
{
    REG_PL_WR(M154_CSMA_2_ADDR,x);
}

static inline uint32_t m154_csma_ca_cap_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_csma_ca_cap_end_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_CSMA_2_ADDR,localVal);
}


#define M154_CSMA_3_ADDR (M154_BASE + 0x0000009c)

static inline uint32_t m154_csma_3_get(void)
{
    return REG_PL_RD(M154_CSMA_3_ADDR);
}

static inline void m154_csma_3_set(uint32_t x)
{
    REG_PL_WR(M154_CSMA_3_ADDR,x);
}

static inline uint32_t m154_csma_ca_ble_period_end_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_csma_ca_ble_period_end_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_CSMA_3_ADDR,localVal);
}


#define M154_CSMA_4_ADDR (M154_BASE + 0x000000a0)

static inline uint32_t m154_csma_4_get(void)
{
    return REG_PL_RD(M154_CSMA_4_ADDR);
}

static inline void m154_csma_4_set(uint32_t x)
{
    REG_PL_WR(M154_CSMA_4_ADDR,x);
}

static inline uint32_t m154_csma_ca_be_curr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_4_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t m154_csma_ca_fsm_curr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CSMA_4_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}


#define M154_TX_0_ADDR (M154_BASE + 0x000000b0)

static inline uint32_t m154_tx_0_get(void)
{
    return REG_PL_RD(M154_TX_0_ADDR);
}

static inline void m154_tx_0_set(uint32_t x)
{
    REG_PL_WR(M154_TX_0_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_sym_cnt_acc_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline uint32_t m154_tx_auto_ack_doing_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline uint32_t m154_tx_doing_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t m154_tx_mpdu_aes_ddt_done_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t m154_tx_mpdu_aes_ddt_phr_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t m154_tx_sn_for_rx_ack_judge_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_TX_1_ADDR (M154_BASE + 0x000000b4)

static inline uint32_t m154_tx_1_get(void)
{
    return REG_PL_RD(M154_TX_1_ADDR);
}

static inline void m154_tx_1_set(uint32_t x)
{
    REG_PL_WR(M154_TX_1_ADDR,x);
}

static inline uint32_t m154_tx_postpone_by_rx_doing_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_tx_postpone_by_rx_doing_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_start_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_start_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_infinite_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void m154_tx_infinite_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_fp_from_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_tx_auto_ack_fp_from_sw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_fp_bit_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_tx_auto_ack_fp_bit_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_auto_ack_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_tx_auto_ack_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_aes_ddt_wait_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_tx_mpdu_aes_ddt_wait_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_trig_time_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_tx_trig_time_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_src_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_tx_mpdu_src_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}

static inline uint32_t m154_tx_trig_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_tx_trig_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_TX_1_ADDR,localVal);
}


#define M154_TX_2_ADDR (M154_BASE + 0x000000b8)

static inline uint32_t m154_tx_2_get(void)
{
    return REG_PL_RD(M154_TX_2_ADDR);
}

static inline void m154_tx_2_set(uint32_t x)
{
    REG_PL_WR(M154_TX_2_ADDR,x);
}

static inline uint32_t m154_tx_trig_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_tx_trig_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_TX_2_ADDR,localVal);
}


#define M154_TX_3_ADDR (M154_BASE + 0x000000bc)

static inline uint32_t m154_tx_3_get(void)
{
    return REG_PL_RD(M154_TX_3_ADDR);
}

static inline void m154_tx_3_set(uint32_t x)
{
    REG_PL_WR(M154_TX_3_ADDR,x);
}

static inline uint32_t m154_tx_rx_transition_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_3_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_rx_transition_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_3_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_3_ADDR,localVal);
}

static inline uint32_t m154_rx_after_tx_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_after_tx_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_3_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_TX_3_ADDR,localVal);
}

static inline uint32_t m154_tx_fsm_curr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_3_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}


#define M154_RX_00_ADDR (M154_BASE + 0x000000c0)

static inline uint32_t m154_rx_00_get(void)
{
    return REG_PL_RD(M154_RX_00_ADDR);
}

static inline void m154_rx_00_set(uint32_t x)
{
    REG_PL_WR(M154_RX_00_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_sym_cnt_acc_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    return ((localVal & ((uint32_t)0x1ff00000)) >> 20);
}

static inline uint32_t m154_rx_doing_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_aes_ddt_trig_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline uint32_t m154_rx_mpdu_aes_ddt_trig_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_rx_mpdu_aes_ddt_trig_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_RX_00_ADDR,localVal);
}

static inline uint32_t m154_rx_tx_transition_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_rx_tx_transition_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_00_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_RX_00_ADDR,localVal);
}


#define M154_RX_01_ADDR (M154_BASE + 0x000000c4)

static inline uint32_t m154_rx_01_get(void)
{
    return REG_PL_RD(M154_RX_01_ADDR);
}

static inline void m154_rx_01_set(uint32_t x)
{
    REG_PL_WR(M154_RX_01_ADDR,x);
}

static inline uint32_t m154_rx_int_clr_by_start_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_rx_int_clr_by_start_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_RX_01_ADDR,localVal);
}

static inline uint32_t m154_rx_phr_err_filter_fail_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void m154_rx_phr_err_filter_fail_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24)&((uint32_t)0x01000000);
    REG_PL_WR(M154_RX_01_ADDR,localVal);
}

static inline uint32_t m154_rx_phr_err_uv_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_rx_phr_err_uv_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_RX_01_ADDR,localVal);
}

static inline uint32_t m154_rx_phr_err_uv_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_rx_bcn_pan_id_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline uint32_t m154_rx_src_pan_id_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t m154_rx_dst_addr_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t m154_rx_dst_pan_id_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t m154_rx_src_addr_mode_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t m154_rx_dst_addr_mode_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t m154_rx_frame_version_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t m154_rx_frame_type_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_rx_crc_fail_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_01_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define M154_RX_02_ADDR (M154_BASE + 0x000000c8)

static inline uint32_t m154_rx_02_get(void)
{
    return REG_PL_RD(M154_RX_02_ADDR);
}

static inline void m154_rx_02_set(uint32_t x)
{
    REG_PL_WR(M154_RX_02_ADDR,x);
}

static inline uint32_t m154_rx_implicit_broadcast_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void m154_rx_implicit_broadcast_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_bp_bound_cmd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void m154_rx_bp_bound_cmd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_bp_bound_data_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void m154_rx_bp_bound_data_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_bp_bound_beacon_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void m154_rx_bp_bound_beacon_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_bp_bound_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void m154_rx_bp_bound_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_bp_bound_offset_frc_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void m154_rx_bp_bound_offset_frc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_group_rx_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void m154_group_rx_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_filter_bypass_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_rx_filter_bypass_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_promiscuous_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_promiscuous_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_ch_scan_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x0000c000)) >> 14);
}

static inline void m154_rx_ch_scan_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x0000c000);
    localVal |= (x << 14)&((uint32_t)0x0000c000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_ch_scan_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void m154_rx_ch_scan_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_stack_2nd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void m154_rx_stack_2nd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_stack_1st_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_rx_stack_1st_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_pan_coordinator_2nd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_rx_pan_coordinator_2nd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}

static inline uint32_t m154_rx_pan_coordinator_1st_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_rx_pan_coordinator_1st_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_02_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_RX_02_ADDR,localVal);
}


#define M154_RX_03_ADDR (M154_BASE + 0x000000cc)

static inline uint32_t m154_rx_03_get(void)
{
    return REG_PL_RD(M154_RX_03_ADDR);
}

static inline void m154_rx_03_set(uint32_t x)
{
    REG_PL_WR(M154_RX_03_ADDR,x);
}

static inline uint32_t m154_rx_pan_id_1st_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_03_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_rx_pan_id_1st_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_03_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_RX_03_ADDR,localVal);
}

static inline uint32_t m154_rx_addr_short_1st_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_03_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_rx_addr_short_1st_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_03_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_RX_03_ADDR,localVal);
}


#define M154_RX_04_ADDR (M154_BASE + 0x000000d0)

static inline uint32_t m154_rx_04_get(void)
{
    return REG_PL_RD(M154_RX_04_ADDR);
}

static inline void m154_rx_04_set(uint32_t x)
{
    REG_PL_WR(M154_RX_04_ADDR,x);
}

static inline uint32_t m154_rx_addr_extended_1st_31_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_04_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rx_addr_extended_1st_31_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_04_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_04_ADDR,localVal);
}


#define M154_RX_05_ADDR (M154_BASE + 0x000000d4)

static inline uint32_t m154_rx_05_get(void)
{
    return REG_PL_RD(M154_RX_05_ADDR);
}

static inline void m154_rx_05_set(uint32_t x)
{
    REG_PL_WR(M154_RX_05_ADDR,x);
}

static inline uint32_t m154_rx_addr_extended_1st_63_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_05_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rx_addr_extended_1st_63_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_05_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_05_ADDR,localVal);
}


#define M154_RX_06_ADDR (M154_BASE + 0x000000d8)

static inline uint32_t m154_rx_06_get(void)
{
    return REG_PL_RD(M154_RX_06_ADDR);
}

static inline void m154_rx_06_set(uint32_t x)
{
    REG_PL_WR(M154_RX_06_ADDR,x);
}

static inline uint32_t m154_rx_pan_id_2nd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_06_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_rx_pan_id_2nd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_06_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_RX_06_ADDR,localVal);
}

static inline uint32_t m154_rx_addr_short_2nd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_06_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_rx_addr_short_2nd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_06_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_RX_06_ADDR,localVal);
}


#define M154_RX_07_ADDR (M154_BASE + 0x000000dc)

static inline uint32_t m154_rx_07_get(void)
{
    return REG_PL_RD(M154_RX_07_ADDR);
}

static inline void m154_rx_07_set(uint32_t x)
{
    REG_PL_WR(M154_RX_07_ADDR,x);
}

static inline uint32_t m154_rx_addr_extended_2nd_31_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_07_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rx_addr_extended_2nd_31_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_07_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_07_ADDR,localVal);
}


#define M154_RX_08_ADDR (M154_BASE + 0x000000e0)

static inline uint32_t m154_rx_08_get(void)
{
    return REG_PL_RD(M154_RX_08_ADDR);
}

static inline void m154_rx_08_set(uint32_t x)
{
    REG_PL_WR(M154_RX_08_ADDR,x);
}

static inline uint32_t m154_rx_addr_extended_2nd_63_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_08_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rx_addr_extended_2nd_63_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_08_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_08_ADDR,localVal);
}


#define M154_RX_09_ADDR (M154_BASE + 0x000000e4)

static inline uint32_t m154_rx_09_get(void)
{
    return REG_PL_RD(M154_RX_09_ADDR);
}

static inline void m154_rx_09_set(uint32_t x)
{
    REG_PL_WR(M154_RX_09_ADDR,x);
}

static inline uint32_t m154_rx_start_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_rx_start_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}

static inline uint32_t m154_csma_ca_rx_st_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t m154_rx_ppdu_sym_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t m154_sfd_found_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t m154_rx_phy_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_rx_start_mac_rx_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_rx_on_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void m154_rx_on_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}

static inline uint32_t m154_rx_off_cmd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_rx_off_cmd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}

static inline uint32_t m154_rx_off_trig_by_end_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void m154_rx_off_trig_by_end_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}

static inline uint32_t m154_rx_off_trig_by_end_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_rx_off_trig_by_end_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}

static inline uint32_t m154_rx_off_trig_time_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_rx_off_trig_time_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_09_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_RX_09_ADDR,localVal);
}


#define M154_RX_10_ADDR (M154_BASE + 0x000000e8)

static inline uint32_t m154_rx_10_get(void)
{
    return REG_PL_RD(M154_RX_10_ADDR);
}

static inline void m154_rx_10_set(uint32_t x)
{
    REG_PL_WR(M154_RX_10_ADDR,x);
}

static inline uint32_t m154_rx_off_trig_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_10_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rx_off_trig_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_10_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_10_ADDR,localVal);
}


#define M154_RX_11_ADDR (M154_BASE + 0x000000ec)

static inline uint32_t m154_rx_11_get(void)
{
    return REG_PL_RD(M154_RX_11_ADDR);
}

static inline void m154_rx_11_set(uint32_t x)
{
    REG_PL_WR(M154_RX_11_ADDR,x);
}

static inline uint32_t m154_rx_frame_counter_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_11_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_RX_12_ADDR (M154_BASE + 0x000000f0)

static inline uint32_t m154_rx_12_get(void)
{
    return REG_PL_RD(M154_RX_12_ADDR);
}

static inline void m154_rx_12_set(uint32_t x)
{
    REG_PL_WR(M154_RX_12_ADDR,x);
}

static inline uint32_t m154_rx_a_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_12_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mic_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_12_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline uint32_t m154_rx_security_level_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_12_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline uint32_t m154_rx_aux_sec_header_length_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_12_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t m154_rx_aux_sec_header_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_12_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}


#define M154_RX_13_ADDR (M154_BASE + 0x000000f4)

static inline uint32_t m154_rx_13_get(void)
{
    return REG_PL_RD(M154_RX_13_ADDR);
}

static inline void m154_rx_13_set(uint32_t x)
{
    REG_PL_WR(M154_RX_13_ADDR,x);
}

static inline uint32_t m154_rx_beacon_payload_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_13_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_beacon_pend_addr_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_13_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline uint32_t m154_rx_beacon_gts_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_13_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline uint32_t m154_rx_mac_payload_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_13_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_14_ADDR (M154_BASE + 0x000000f8)

static inline uint32_t m154_rx_14_get(void)
{
    return REG_PL_RD(M154_RX_14_ADDR);
}

static inline void m154_rx_14_set(uint32_t x)
{
    REG_PL_WR(M154_RX_14_ADDR,x);
}

static inline uint32_t m154_rx_cmd_id_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_14_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_cmd_id_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_14_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}


#define M154_RX_15_ADDR (M154_BASE + 0x000000fc)

static inline uint32_t m154_rx_15_get(void)
{
    return REG_PL_RD(M154_RX_15_ADDR);
}

static inline void m154_rx_15_set(uint32_t x)
{
    REG_PL_WR(M154_RX_15_ADDR,x);
}

static inline uint32_t m154_rx_ack_wait_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_15_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void m154_rx_ack_wait_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_15_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(M154_RX_15_ADDR,localVal);
}

static inline uint32_t m154_rx_auto_restart_wait_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_15_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_rx_auto_restart_wait_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_15_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_RX_15_ADDR,localVal);
}


#define M154_RX_16_ADDR (M154_BASE + 0x00000100)

static inline uint32_t m154_rx_16_get(void)
{
    return REG_PL_RD(M154_RX_16_ADDR);
}

static inline void m154_rx_16_set(uint32_t x)
{
    REG_PL_WR(M154_RX_16_ADDR,x);
}

static inline uint32_t m154_total_frame_wait_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_16_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_total_frame_wait_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_16_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RX_16_ADDR,localVal);
}


#define M154_RX_17_ADDR (M154_BASE + 0x00000104)

static inline uint32_t m154_rx_17_get(void)
{
    return REG_PL_RD(M154_RX_17_ADDR);
}

static inline void m154_rx_17_set(uint32_t x)
{
    REG_PL_WR(M154_RX_17_ADDR,x);
}

static inline uint32_t m154_rsvd_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_17_ADDR);
    return ((localVal & ((uint32_t)0xfffffffe)) >> 1);
}

static inline void m154_rsvd_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_17_ADDR);
    localVal &= ~((uint32_t)0xfffffffe);
    localVal |= (x << 1)&((uint32_t)0xfffffffe);
    REG_PL_WR(M154_RX_17_ADDR,localVal);
}

static inline uint32_t m154_rx_on_after_rx_ack_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_17_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_rx_on_after_rx_ack_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RX_17_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_RX_17_ADDR,localVal);
}


#define M154_RSVD_01_ADDR (M154_BASE + 0x00000108)

static inline uint32_t m154_rsvd_01_get(void)
{
    return REG_PL_RD(M154_RSVD_01_ADDR);
}

static inline void m154_rsvd_01_set(uint32_t x)
{
    REG_PL_WR(M154_RSVD_01_ADDR,x);
}

static inline uint32_t m154_rsvd_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RSVD_01_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_rsvd_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_RSVD_01_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_RSVD_01_ADDR,localVal);
}


#define M154_CAPT_FRM_00_ADDR (M154_BASE + 0x0000010c)

static inline uint32_t m154_capt_frm_00_get(void)
{
    return REG_PL_RD(M154_CAPT_FRM_00_ADDR);
}

static inline void m154_capt_frm_00_set(uint32_t x)
{
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,x);
}

static inline uint32_t m154_capt_frame_mhr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_capt_frame_mhr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,localVal);
}

static inline uint32_t m154_capt_frame_phr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_capt_frame_phr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,localVal);
}

static inline uint32_t m154_capt_frame_retry_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void m154_capt_frame_retry_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2)&((uint32_t)0x000000fc);
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,localVal);
}

static inline uint32_t m154_capt_frame_tx_rxn_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_capt_frame_tx_rxn_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,localVal);
}

static inline uint32_t m154_capt_frame_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_capt_frame_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_CAPT_FRM_00_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_CAPT_FRM_00_ADDR,localVal);
}


#define M154_SW_PROFILE_ADDR (M154_BASE + 0x00000110)

static inline uint32_t m154_sw_profile_get(void)
{
    return REG_PL_RD(M154_SW_PROFILE_ADDR);
}

static inline void m154_sw_profile_set(uint32_t x)
{
    REG_PL_WR(M154_SW_PROFILE_ADDR,x);
}

static inline uint32_t m154_sw_profile_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_SW_PROFILE_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_sw_profile_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_SW_PROFILE_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_SW_PROFILE_ADDR,localVal);
}


#define M154_COEX_PRIORITY_ADDR (M154_BASE + 0x00000180)

static inline uint32_t m154_coex_priority_get(void)
{
    return REG_PL_RD(M154_COEX_PRIORITY_ADDR);
}

static inline void m154_coex_priority_set(uint32_t x)
{
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,x);
}

static inline uint32_t m154_tx_abort_int_dual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void m154_tx_abort_int_dual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_rx_abort_int_dual_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void m154_rx_abort_int_dual_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_tx_abort_int_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline uint32_t m154_rx_abort_int_status_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline uint32_t m154_rx_on_after_rx_abort_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_rx_on_after_rx_abort_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_tx_priority_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void m154_tx_priority_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12)&((uint32_t)0x0000f000);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_rx_unicast_priority_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void m154_rx_unicast_priority_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_rx_sfd_priority_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void m154_rx_sfd_priority_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}

static inline uint32_t m154_rx_idle_priority_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void m154_rx_idle_priority_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_COEX_PRIORITY_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(M154_COEX_PRIORITY_ADDR,localVal);
}


#define M154_TX_MPDU_00_ADDR (M154_BASE + 0x00000200)

static inline uint32_t m154_tx_mpdu_00_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_00_ADDR);
}

static inline void m154_tx_mpdu_00_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_00_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_003_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_003_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_00_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_002_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_002_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_00_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_001_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_001_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_00_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_000_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_000_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_00_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_00_ADDR,localVal);
}


#define M154_TX_MPDU_01_ADDR (M154_BASE + 0x00000204)

static inline uint32_t m154_tx_mpdu_01_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_01_ADDR);
}

static inline void m154_tx_mpdu_01_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_01_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_007_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_007_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_01_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_006_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_006_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_01_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_005_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_005_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_01_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_004_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_004_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_01_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_01_ADDR,localVal);
}


#define M154_TX_MPDU_02_ADDR (M154_BASE + 0x00000208)

static inline uint32_t m154_tx_mpdu_02_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_02_ADDR);
}

static inline void m154_tx_mpdu_02_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_02_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_011_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_011_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_02_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_010_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_010_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_02_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_009_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_009_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_02_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_008_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_008_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_02_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_02_ADDR,localVal);
}


#define M154_TX_MPDU_03_ADDR (M154_BASE + 0x0000020c)

static inline uint32_t m154_tx_mpdu_03_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_03_ADDR);
}

static inline void m154_tx_mpdu_03_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_03_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_015_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_015_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_03_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_014_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_014_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_03_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_013_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_013_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_03_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_012_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_012_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_03_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_03_ADDR,localVal);
}


#define M154_TX_MPDU_04_ADDR (M154_BASE + 0x00000210)

static inline uint32_t m154_tx_mpdu_04_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_04_ADDR);
}

static inline void m154_tx_mpdu_04_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_04_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_019_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_019_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_04_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_018_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_018_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_04_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_017_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_017_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_04_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_016_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_016_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_04_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_04_ADDR,localVal);
}


#define M154_TX_MPDU_05_ADDR (M154_BASE + 0x00000214)

static inline uint32_t m154_tx_mpdu_05_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_05_ADDR);
}

static inline void m154_tx_mpdu_05_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_05_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_023_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_023_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_05_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_022_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_022_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_05_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_021_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_021_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_05_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_020_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_020_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_05_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_05_ADDR,localVal);
}


#define M154_TX_MPDU_06_ADDR (M154_BASE + 0x00000218)

static inline uint32_t m154_tx_mpdu_06_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_06_ADDR);
}

static inline void m154_tx_mpdu_06_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_06_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_027_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_027_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_06_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_026_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_026_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_06_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_025_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_025_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_06_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_024_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_024_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_06_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_06_ADDR,localVal);
}


#define M154_TX_MPDU_07_ADDR (M154_BASE + 0x0000021c)

static inline uint32_t m154_tx_mpdu_07_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_07_ADDR);
}

static inline void m154_tx_mpdu_07_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_07_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_031_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_031_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_07_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_030_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_030_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_07_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_029_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_029_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_07_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_028_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_028_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_07_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_07_ADDR,localVal);
}


#define M154_TX_MPDU_08_ADDR (M154_BASE + 0x00000220)

static inline uint32_t m154_tx_mpdu_08_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_08_ADDR);
}

static inline void m154_tx_mpdu_08_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_08_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_035_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_035_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_08_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_034_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_034_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_08_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_033_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_033_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_08_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_032_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_032_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_08_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_08_ADDR,localVal);
}


#define M154_TX_MPDU_09_ADDR (M154_BASE + 0x00000224)

static inline uint32_t m154_tx_mpdu_09_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_09_ADDR);
}

static inline void m154_tx_mpdu_09_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_09_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_039_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_039_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_09_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_038_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_038_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_09_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_037_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_037_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_09_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_036_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_036_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_09_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_09_ADDR,localVal);
}


#define M154_TX_MPDU_10_ADDR (M154_BASE + 0x00000228)

static inline uint32_t m154_tx_mpdu_10_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_10_ADDR);
}

static inline void m154_tx_mpdu_10_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_10_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_043_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_043_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_10_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_042_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_042_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_10_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_041_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_041_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_10_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_040_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_040_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_10_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_10_ADDR,localVal);
}


#define M154_TX_MPDU_11_ADDR (M154_BASE + 0x0000022c)

static inline uint32_t m154_tx_mpdu_11_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_11_ADDR);
}

static inline void m154_tx_mpdu_11_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_11_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_047_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_047_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_11_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_046_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_046_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_11_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_045_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_045_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_11_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_044_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_044_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_11_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_11_ADDR,localVal);
}


#define M154_TX_MPDU_12_ADDR (M154_BASE + 0x00000230)

static inline uint32_t m154_tx_mpdu_12_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_12_ADDR);
}

static inline void m154_tx_mpdu_12_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_12_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_051_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_051_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_12_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_050_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_050_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_12_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_049_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_049_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_12_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_048_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_048_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_12_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_12_ADDR,localVal);
}


#define M154_TX_MPDU_13_ADDR (M154_BASE + 0x00000234)

static inline uint32_t m154_tx_mpdu_13_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_13_ADDR);
}

static inline void m154_tx_mpdu_13_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_13_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_055_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_055_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_13_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_054_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_054_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_13_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_053_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_053_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_13_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_052_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_052_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_13_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_13_ADDR,localVal);
}


#define M154_TX_MPDU_14_ADDR (M154_BASE + 0x00000238)

static inline uint32_t m154_tx_mpdu_14_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_14_ADDR);
}

static inline void m154_tx_mpdu_14_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_14_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_059_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_059_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_14_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_058_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_058_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_14_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_057_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_057_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_14_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_056_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_056_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_14_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_14_ADDR,localVal);
}


#define M154_TX_MPDU_15_ADDR (M154_BASE + 0x0000023c)

static inline uint32_t m154_tx_mpdu_15_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_15_ADDR);
}

static inline void m154_tx_mpdu_15_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_15_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_063_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_063_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_15_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_062_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_062_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_15_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_061_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_061_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_15_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_060_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_060_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_15_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_15_ADDR,localVal);
}


#define M154_TX_MPDU_16_ADDR (M154_BASE + 0x00000240)

static inline uint32_t m154_tx_mpdu_16_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_16_ADDR);
}

static inline void m154_tx_mpdu_16_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_16_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_067_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_067_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_16_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_066_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_066_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_16_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_065_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_065_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_16_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_064_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_064_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_16_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_16_ADDR,localVal);
}


#define M154_TX_MPDU_17_ADDR (M154_BASE + 0x00000244)

static inline uint32_t m154_tx_mpdu_17_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_17_ADDR);
}

static inline void m154_tx_mpdu_17_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_17_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_071_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_071_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_17_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_070_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_070_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_17_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_069_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_069_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_17_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_068_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_068_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_17_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_17_ADDR,localVal);
}


#define M154_TX_MPDU_18_ADDR (M154_BASE + 0x00000248)

static inline uint32_t m154_tx_mpdu_18_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_18_ADDR);
}

static inline void m154_tx_mpdu_18_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_18_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_075_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_075_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_18_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_074_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_074_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_18_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_073_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_073_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_18_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_072_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_072_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_18_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_18_ADDR,localVal);
}


#define M154_TX_MPDU_19_ADDR (M154_BASE + 0x0000024c)

static inline uint32_t m154_tx_mpdu_19_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_19_ADDR);
}

static inline void m154_tx_mpdu_19_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_19_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_079_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_079_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_19_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_078_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_078_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_19_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_077_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_077_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_19_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_076_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_076_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_19_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_19_ADDR,localVal);
}


#define M154_TX_MPDU_20_ADDR (M154_BASE + 0x00000250)

static inline uint32_t m154_tx_mpdu_20_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_20_ADDR);
}

static inline void m154_tx_mpdu_20_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_20_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_083_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_083_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_20_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_082_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_082_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_20_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_081_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_081_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_20_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_080_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_080_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_20_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_20_ADDR,localVal);
}


#define M154_TX_MPDU_21_ADDR (M154_BASE + 0x00000254)

static inline uint32_t m154_tx_mpdu_21_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_21_ADDR);
}

static inline void m154_tx_mpdu_21_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_21_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_087_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_087_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_21_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_086_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_086_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_21_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_085_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_085_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_21_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_084_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_084_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_21_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_21_ADDR,localVal);
}


#define M154_TX_MPDU_22_ADDR (M154_BASE + 0x00000258)

static inline uint32_t m154_tx_mpdu_22_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_22_ADDR);
}

static inline void m154_tx_mpdu_22_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_22_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_091_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_091_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_22_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_090_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_090_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_22_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_089_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_089_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_22_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_088_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_088_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_22_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_22_ADDR,localVal);
}


#define M154_TX_MPDU_23_ADDR (M154_BASE + 0x0000025c)

static inline uint32_t m154_tx_mpdu_23_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_23_ADDR);
}

static inline void m154_tx_mpdu_23_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_23_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_095_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_095_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_23_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_094_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_094_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_23_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_093_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_093_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_23_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_092_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_092_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_23_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_23_ADDR,localVal);
}


#define M154_TX_MPDU_24_ADDR (M154_BASE + 0x00000260)

static inline uint32_t m154_tx_mpdu_24_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_24_ADDR);
}

static inline void m154_tx_mpdu_24_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_24_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_099_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_099_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_24_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_098_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_098_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_24_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_097_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_097_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_24_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_096_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_096_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_24_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_24_ADDR,localVal);
}


#define M154_TX_MPDU_25_ADDR (M154_BASE + 0x00000264)

static inline uint32_t m154_tx_mpdu_25_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_25_ADDR);
}

static inline void m154_tx_mpdu_25_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_25_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_103_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_103_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_25_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_102_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_102_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_25_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_101_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_101_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_25_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_100_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_100_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_25_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_25_ADDR,localVal);
}


#define M154_TX_MPDU_26_ADDR (M154_BASE + 0x00000268)

static inline uint32_t m154_tx_mpdu_26_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_26_ADDR);
}

static inline void m154_tx_mpdu_26_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_26_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_107_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_107_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_26_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_106_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_106_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_26_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_105_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_105_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_26_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_104_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_104_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_26_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_26_ADDR,localVal);
}


#define M154_TX_MPDU_27_ADDR (M154_BASE + 0x0000026c)

static inline uint32_t m154_tx_mpdu_27_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_27_ADDR);
}

static inline void m154_tx_mpdu_27_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_27_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_111_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_111_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_27_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_110_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_110_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_27_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_109_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_109_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_27_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_108_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_108_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_27_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_27_ADDR,localVal);
}


#define M154_TX_MPDU_28_ADDR (M154_BASE + 0x00000270)

static inline uint32_t m154_tx_mpdu_28_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_28_ADDR);
}

static inline void m154_tx_mpdu_28_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_28_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_115_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_115_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_28_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_114_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_114_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_28_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_113_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_113_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_28_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_112_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_112_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_28_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_28_ADDR,localVal);
}


#define M154_TX_MPDU_29_ADDR (M154_BASE + 0x00000274)

static inline uint32_t m154_tx_mpdu_29_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_29_ADDR);
}

static inline void m154_tx_mpdu_29_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_29_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_119_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_119_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_29_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_118_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_118_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_29_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_117_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_117_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_29_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_116_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_116_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_29_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_29_ADDR,localVal);
}


#define M154_TX_MPDU_30_ADDR (M154_BASE + 0x00000278)

static inline uint32_t m154_tx_mpdu_30_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_30_ADDR);
}

static inline void m154_tx_mpdu_30_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_30_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_123_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_123_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_30_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_122_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_122_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_30_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_121_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_121_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_30_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_120_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_120_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_30_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_30_ADDR,localVal);
}


#define M154_TX_MPDU_31_ADDR (M154_BASE + 0x0000027c)

static inline uint32_t m154_tx_mpdu_31_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_31_ADDR);
}

static inline void m154_tx_mpdu_31_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_31_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_127_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_127_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_31_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_126_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_126_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_31_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_125_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_125_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_31_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_124_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_124_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_31_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_31_ADDR,localVal);
}


#define M154_TX_MPDU_32_ADDR (M154_BASE + 0x00000280)

static inline uint32_t m154_tx_mpdu_32_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_32_ADDR);
}

static inline void m154_tx_mpdu_32_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_32_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_131_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_131_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_32_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_130_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_130_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_32_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_129_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_129_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_32_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_128_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_128_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_32_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_32_ADDR,localVal);
}


#define M154_TX_MPDU_33_ADDR (M154_BASE + 0x00000284)

static inline uint32_t m154_tx_mpdu_33_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_33_ADDR);
}

static inline void m154_tx_mpdu_33_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_33_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_135_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_135_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_33_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_134_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_134_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_33_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_133_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_133_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_33_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_132_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_132_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_33_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_33_ADDR,localVal);
}


#define M154_TX_MPDU_34_ADDR (M154_BASE + 0x00000288)

static inline uint32_t m154_tx_mpdu_34_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_34_ADDR);
}

static inline void m154_tx_mpdu_34_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_34_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_139_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_139_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_34_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_138_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_138_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_34_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_137_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_137_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_34_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_136_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_136_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_34_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_34_ADDR,localVal);
}


#define M154_TX_MPDU_35_ADDR (M154_BASE + 0x0000028c)

static inline uint32_t m154_tx_mpdu_35_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_35_ADDR);
}

static inline void m154_tx_mpdu_35_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_35_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_143_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_143_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_35_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_142_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_142_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_35_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_141_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_141_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_35_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_140_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_140_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_35_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_35_ADDR,localVal);
}


#define M154_TX_MPDU_36_ADDR (M154_BASE + 0x00000290)

static inline uint32_t m154_tx_mpdu_36_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_36_ADDR);
}

static inline void m154_tx_mpdu_36_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_36_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_147_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_147_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_36_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_146_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_146_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_36_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_145_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_145_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_36_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_144_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_144_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_36_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_36_ADDR,localVal);
}


#define M154_TX_MPDU_37_ADDR (M154_BASE + 0x00000294)

static inline uint32_t m154_tx_mpdu_37_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_37_ADDR);
}

static inline void m154_tx_mpdu_37_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_37_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_151_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_151_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_37_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_150_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_150_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_37_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_149_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_149_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_37_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_148_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_148_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_37_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_37_ADDR,localVal);
}


#define M154_TX_MPDU_38_ADDR (M154_BASE + 0x00000298)

static inline uint32_t m154_tx_mpdu_38_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_38_ADDR);
}

static inline void m154_tx_mpdu_38_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_38_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_155_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_155_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_38_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_154_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_154_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_38_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_153_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_153_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_38_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_152_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_152_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_38_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_38_ADDR,localVal);
}


#define M154_TX_MPDU_39_ADDR (M154_BASE + 0x0000029c)

static inline uint32_t m154_tx_mpdu_39_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_39_ADDR);
}

static inline void m154_tx_mpdu_39_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_39_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_159_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_159_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_39_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_158_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_158_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_39_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_157_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_157_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_39_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_156_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_156_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_39_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_39_ADDR,localVal);
}


#define M154_TX_MPDU_40_ADDR (M154_BASE + 0x000002a0)

static inline uint32_t m154_tx_mpdu_40_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_40_ADDR);
}

static inline void m154_tx_mpdu_40_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_40_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_163_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_163_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_40_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_162_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_162_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_40_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_161_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_161_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_40_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_160_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_160_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_40_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_40_ADDR,localVal);
}


#define M154_TX_MPDU_41_ADDR (M154_BASE + 0x000002a4)

static inline uint32_t m154_tx_mpdu_41_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_41_ADDR);
}

static inline void m154_tx_mpdu_41_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_41_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_167_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_167_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_41_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_166_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_166_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_41_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_165_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_165_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_41_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_164_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_164_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_41_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_41_ADDR,localVal);
}


#define M154_TX_MPDU_42_ADDR (M154_BASE + 0x000002a8)

static inline uint32_t m154_tx_mpdu_42_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_42_ADDR);
}

static inline void m154_tx_mpdu_42_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_42_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_171_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_171_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_42_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_170_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_170_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_42_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_169_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_169_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_42_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_168_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_168_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_42_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_42_ADDR,localVal);
}


#define M154_TX_MPDU_43_ADDR (M154_BASE + 0x000002ac)

static inline uint32_t m154_tx_mpdu_43_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_43_ADDR);
}

static inline void m154_tx_mpdu_43_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_43_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_175_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_175_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_43_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_174_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_174_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_43_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_173_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_173_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_43_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_172_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_172_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_43_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_43_ADDR,localVal);
}


#define M154_TX_MPDU_44_ADDR (M154_BASE + 0x000002b0)

static inline uint32_t m154_tx_mpdu_44_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_44_ADDR);
}

static inline void m154_tx_mpdu_44_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_44_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_179_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_179_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_44_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_178_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_178_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_44_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_177_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_177_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_44_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_176_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_176_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_44_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_44_ADDR,localVal);
}


#define M154_TX_MPDU_45_ADDR (M154_BASE + 0x000002b4)

static inline uint32_t m154_tx_mpdu_45_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_45_ADDR);
}

static inline void m154_tx_mpdu_45_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_45_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_183_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_183_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_45_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_182_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_182_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_45_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_181_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_181_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_45_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_180_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_180_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_45_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_45_ADDR,localVal);
}


#define M154_TX_MPDU_46_ADDR (M154_BASE + 0x000002b8)

static inline uint32_t m154_tx_mpdu_46_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_46_ADDR);
}

static inline void m154_tx_mpdu_46_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_46_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_187_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_187_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_46_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_186_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_186_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_46_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_185_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_185_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_46_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_184_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_184_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_46_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_46_ADDR,localVal);
}


#define M154_TX_MPDU_47_ADDR (M154_BASE + 0x000002bc)

static inline uint32_t m154_tx_mpdu_47_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_47_ADDR);
}

static inline void m154_tx_mpdu_47_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_47_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_191_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_191_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_47_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_190_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_190_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_47_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_189_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_189_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_47_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_188_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_188_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_47_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_47_ADDR,localVal);
}


#define M154_TX_MPDU_48_ADDR (M154_BASE + 0x000002c0)

static inline uint32_t m154_tx_mpdu_48_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_48_ADDR);
}

static inline void m154_tx_mpdu_48_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_48_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_195_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_195_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_48_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_194_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_194_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_48_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_193_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_193_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_48_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_192_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_192_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_48_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_48_ADDR,localVal);
}


#define M154_TX_MPDU_49_ADDR (M154_BASE + 0x000002c4)

static inline uint32_t m154_tx_mpdu_49_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_49_ADDR);
}

static inline void m154_tx_mpdu_49_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_49_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_199_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_199_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_49_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_198_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_198_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_49_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_197_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_197_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_49_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_196_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_196_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_49_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_49_ADDR,localVal);
}


#define M154_TX_MPDU_50_ADDR (M154_BASE + 0x000002c8)

static inline uint32_t m154_tx_mpdu_50_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_50_ADDR);
}

static inline void m154_tx_mpdu_50_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_50_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_203_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_203_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_50_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_202_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_202_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_50_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_201_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_201_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_50_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_200_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_200_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_50_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_50_ADDR,localVal);
}


#define M154_TX_MPDU_51_ADDR (M154_BASE + 0x000002cc)

static inline uint32_t m154_tx_mpdu_51_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_51_ADDR);
}

static inline void m154_tx_mpdu_51_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_51_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_207_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_207_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_51_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_206_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_206_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_51_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_205_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_205_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_51_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_204_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_204_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_51_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_51_ADDR,localVal);
}


#define M154_TX_MPDU_52_ADDR (M154_BASE + 0x000002d0)

static inline uint32_t m154_tx_mpdu_52_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_52_ADDR);
}

static inline void m154_tx_mpdu_52_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_52_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_211_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_211_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_52_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_210_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_210_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_52_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_209_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_209_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_52_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_208_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_208_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_52_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_52_ADDR,localVal);
}


#define M154_TX_MPDU_53_ADDR (M154_BASE + 0x000002d4)

static inline uint32_t m154_tx_mpdu_53_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_53_ADDR);
}

static inline void m154_tx_mpdu_53_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_53_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_215_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_215_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_53_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_214_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_214_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_53_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_213_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_213_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_53_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_212_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_212_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_53_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_53_ADDR,localVal);
}


#define M154_TX_MPDU_54_ADDR (M154_BASE + 0x000002d8)

static inline uint32_t m154_tx_mpdu_54_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_54_ADDR);
}

static inline void m154_tx_mpdu_54_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_54_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_219_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_219_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_54_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_218_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_218_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_54_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_217_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_217_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_54_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_216_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_216_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_54_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_54_ADDR,localVal);
}


#define M154_TX_MPDU_55_ADDR (M154_BASE + 0x000002dc)

static inline uint32_t m154_tx_mpdu_55_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_55_ADDR);
}

static inline void m154_tx_mpdu_55_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_55_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_223_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_223_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_55_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_222_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_222_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_55_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_221_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_221_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_55_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_220_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_220_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_55_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_55_ADDR,localVal);
}


#define M154_TX_MPDU_56_ADDR (M154_BASE + 0x000002e0)

static inline uint32_t m154_tx_mpdu_56_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_56_ADDR);
}

static inline void m154_tx_mpdu_56_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_56_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_227_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_227_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_56_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_226_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_226_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_56_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_225_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_225_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_56_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_224_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_224_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_56_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_56_ADDR,localVal);
}


#define M154_TX_MPDU_57_ADDR (M154_BASE + 0x000002e4)

static inline uint32_t m154_tx_mpdu_57_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_57_ADDR);
}

static inline void m154_tx_mpdu_57_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_57_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_231_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_231_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_57_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_230_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_230_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_57_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_229_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_229_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_57_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_228_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_228_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_57_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_57_ADDR,localVal);
}


#define M154_TX_MPDU_58_ADDR (M154_BASE + 0x000002e8)

static inline uint32_t m154_tx_mpdu_58_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_58_ADDR);
}

static inline void m154_tx_mpdu_58_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_58_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_235_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_235_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_58_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_234_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_234_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_58_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_233_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_233_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_58_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_232_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_232_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_58_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_58_ADDR,localVal);
}


#define M154_TX_MPDU_59_ADDR (M154_BASE + 0x000002ec)

static inline uint32_t m154_tx_mpdu_59_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_59_ADDR);
}

static inline void m154_tx_mpdu_59_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_59_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_239_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_239_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_59_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_238_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_238_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_59_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_237_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_237_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_59_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_236_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_236_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_59_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_59_ADDR,localVal);
}


#define M154_TX_MPDU_60_ADDR (M154_BASE + 0x000002f0)

static inline uint32_t m154_tx_mpdu_60_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_60_ADDR);
}

static inline void m154_tx_mpdu_60_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_60_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_243_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_243_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_60_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_242_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_242_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_60_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_241_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_241_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_60_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_240_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_240_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_60_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_60_ADDR,localVal);
}


#define M154_TX_MPDU_61_ADDR (M154_BASE + 0x000002f4)

static inline uint32_t m154_tx_mpdu_61_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_61_ADDR);
}

static inline void m154_tx_mpdu_61_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_61_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_247_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_247_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_61_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_246_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_246_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_61_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_245_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_245_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_61_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_244_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_244_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_61_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_61_ADDR,localVal);
}


#define M154_TX_MPDU_62_ADDR (M154_BASE + 0x000002f8)

static inline uint32_t m154_tx_mpdu_62_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_62_ADDR);
}

static inline void m154_tx_mpdu_62_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_62_ADDR,x);
}

static inline uint32_t m154_tx_mpdu_251_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_mpdu_251_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_62_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_250_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void m154_tx_mpdu_250_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(M154_TX_MPDU_62_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_249_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void m154_tx_mpdu_249_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(M154_TX_MPDU_62_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_248_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_248_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_62_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_62_ADDR,localVal);
}


#define M154_TX_MPDU_63_ADDR (M154_BASE + 0x000002fc)

static inline uint32_t m154_tx_mpdu_63_get(void)
{
    return REG_PL_RD(M154_TX_MPDU_63_ADDR);
}

static inline void m154_tx_mpdu_63_set(uint32_t x)
{
    REG_PL_WR(M154_TX_MPDU_63_ADDR,x);
}

static inline uint32_t m154_tx_phr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void m154_tx_phr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(M154_TX_MPDU_63_ADDR,localVal);
}

static inline uint32_t m154_tx_mpdu_fcs_15_08_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_tx_mpdu_fcs_07_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_tx_mpdu_252_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_mpdu_252_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_MPDU_63_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_MPDU_63_ADDR,localVal);
}


#define M154_TX_PPDU_CTRL_ADDR (M154_BASE + 0x00000300)

static inline uint32_t m154_tx_ppdu_ctrl_get(void)
{
    return REG_PL_RD(M154_TX_PPDU_CTRL_ADDR);
}

static inline void m154_tx_ppdu_ctrl_set(uint32_t x)
{
    REG_PL_WR(M154_TX_PPDU_CTRL_ADDR,x);
}

static inline uint32_t m154_tx_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_PPDU_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000300)) >> 8);
}

static inline void m154_tx_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_PPDU_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000300);
    localVal |= (x << 8)&((uint32_t)0x00000300);
    REG_PL_WR(M154_TX_PPDU_CTRL_ADDR,localVal);
}

static inline uint32_t m154_tx_sfd_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_TX_PPDU_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void m154_tx_sfd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_TX_PPDU_CTRL_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(M154_TX_PPDU_CTRL_ADDR,localVal);
}


#define M154_RX_MPDU_00_ADDR (M154_BASE + 0x00000400)

static inline uint32_t m154_rx_mpdu_00_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_00_ADDR);
}

static inline void m154_rx_mpdu_00_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_00_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_003_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_002_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_001_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_000_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_01_ADDR (M154_BASE + 0x00000404)

static inline uint32_t m154_rx_mpdu_01_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_01_ADDR);
}

static inline void m154_rx_mpdu_01_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_01_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_007_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_006_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_005_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_004_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_02_ADDR (M154_BASE + 0x00000408)

static inline uint32_t m154_rx_mpdu_02_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_02_ADDR);
}

static inline void m154_rx_mpdu_02_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_02_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_011_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_010_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_009_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_008_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_03_ADDR (M154_BASE + 0x0000040c)

static inline uint32_t m154_rx_mpdu_03_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_03_ADDR);
}

static inline void m154_rx_mpdu_03_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_03_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_015_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_014_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_013_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_012_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_04_ADDR (M154_BASE + 0x00000410)

static inline uint32_t m154_rx_mpdu_04_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_04_ADDR);
}

static inline void m154_rx_mpdu_04_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_04_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_019_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_018_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_017_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_016_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_05_ADDR (M154_BASE + 0x00000414)

static inline uint32_t m154_rx_mpdu_05_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_05_ADDR);
}

static inline void m154_rx_mpdu_05_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_05_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_023_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_022_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_021_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_020_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_06_ADDR (M154_BASE + 0x00000418)

static inline uint32_t m154_rx_mpdu_06_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_06_ADDR);
}

static inline void m154_rx_mpdu_06_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_06_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_027_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_026_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_025_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_024_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_07_ADDR (M154_BASE + 0x0000041c)

static inline uint32_t m154_rx_mpdu_07_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_07_ADDR);
}

static inline void m154_rx_mpdu_07_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_07_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_031_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_030_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_029_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_028_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_08_ADDR (M154_BASE + 0x00000420)

static inline uint32_t m154_rx_mpdu_08_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_08_ADDR);
}

static inline void m154_rx_mpdu_08_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_08_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_035_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_034_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_033_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_032_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_09_ADDR (M154_BASE + 0x00000424)

static inline uint32_t m154_rx_mpdu_09_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_09_ADDR);
}

static inline void m154_rx_mpdu_09_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_09_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_039_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_038_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_037_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_036_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_10_ADDR (M154_BASE + 0x00000428)

static inline uint32_t m154_rx_mpdu_10_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_10_ADDR);
}

static inline void m154_rx_mpdu_10_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_10_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_043_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_042_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_041_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_040_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_11_ADDR (M154_BASE + 0x0000042c)

static inline uint32_t m154_rx_mpdu_11_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_11_ADDR);
}

static inline void m154_rx_mpdu_11_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_11_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_047_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_046_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_045_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_044_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_12_ADDR (M154_BASE + 0x00000430)

static inline uint32_t m154_rx_mpdu_12_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_12_ADDR);
}

static inline void m154_rx_mpdu_12_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_12_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_051_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_050_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_049_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_048_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_13_ADDR (M154_BASE + 0x00000434)

static inline uint32_t m154_rx_mpdu_13_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_13_ADDR);
}

static inline void m154_rx_mpdu_13_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_13_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_055_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_054_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_053_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_052_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_14_ADDR (M154_BASE + 0x00000438)

static inline uint32_t m154_rx_mpdu_14_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_14_ADDR);
}

static inline void m154_rx_mpdu_14_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_14_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_059_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_058_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_057_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_056_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_15_ADDR (M154_BASE + 0x0000043c)

static inline uint32_t m154_rx_mpdu_15_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_15_ADDR);
}

static inline void m154_rx_mpdu_15_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_15_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_063_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_062_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_061_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_060_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_16_ADDR (M154_BASE + 0x00000440)

static inline uint32_t m154_rx_mpdu_16_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_16_ADDR);
}

static inline void m154_rx_mpdu_16_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_16_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_067_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_066_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_065_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_064_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_17_ADDR (M154_BASE + 0x00000444)

static inline uint32_t m154_rx_mpdu_17_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_17_ADDR);
}

static inline void m154_rx_mpdu_17_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_17_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_071_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_070_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_069_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_068_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_18_ADDR (M154_BASE + 0x00000448)

static inline uint32_t m154_rx_mpdu_18_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_18_ADDR);
}

static inline void m154_rx_mpdu_18_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_18_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_075_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_074_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_073_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_072_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_19_ADDR (M154_BASE + 0x0000044c)

static inline uint32_t m154_rx_mpdu_19_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_19_ADDR);
}

static inline void m154_rx_mpdu_19_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_19_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_079_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_078_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_077_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_076_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_20_ADDR (M154_BASE + 0x00000450)

static inline uint32_t m154_rx_mpdu_20_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_20_ADDR);
}

static inline void m154_rx_mpdu_20_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_20_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_083_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_082_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_081_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_080_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_21_ADDR (M154_BASE + 0x00000454)

static inline uint32_t m154_rx_mpdu_21_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_21_ADDR);
}

static inline void m154_rx_mpdu_21_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_21_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_087_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_086_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_085_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_084_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_22_ADDR (M154_BASE + 0x00000458)

static inline uint32_t m154_rx_mpdu_22_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_22_ADDR);
}

static inline void m154_rx_mpdu_22_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_22_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_091_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_090_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_089_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_088_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_23_ADDR (M154_BASE + 0x0000045c)

static inline uint32_t m154_rx_mpdu_23_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_23_ADDR);
}

static inline void m154_rx_mpdu_23_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_23_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_095_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_094_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_093_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_092_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_24_ADDR (M154_BASE + 0x00000460)

static inline uint32_t m154_rx_mpdu_24_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_24_ADDR);
}

static inline void m154_rx_mpdu_24_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_24_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_099_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_098_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_097_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_096_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_25_ADDR (M154_BASE + 0x00000464)

static inline uint32_t m154_rx_mpdu_25_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_25_ADDR);
}

static inline void m154_rx_mpdu_25_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_25_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_103_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_102_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_101_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_100_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_26_ADDR (M154_BASE + 0x00000468)

static inline uint32_t m154_rx_mpdu_26_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_26_ADDR);
}

static inline void m154_rx_mpdu_26_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_26_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_107_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_106_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_105_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_104_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_27_ADDR (M154_BASE + 0x0000046c)

static inline uint32_t m154_rx_mpdu_27_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_27_ADDR);
}

static inline void m154_rx_mpdu_27_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_27_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_111_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_110_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_109_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_108_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_28_ADDR (M154_BASE + 0x00000470)

static inline uint32_t m154_rx_mpdu_28_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_28_ADDR);
}

static inline void m154_rx_mpdu_28_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_28_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_115_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_114_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_113_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_112_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_29_ADDR (M154_BASE + 0x00000474)

static inline uint32_t m154_rx_mpdu_29_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_29_ADDR);
}

static inline void m154_rx_mpdu_29_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_29_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_119_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_118_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_117_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_116_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_30_ADDR (M154_BASE + 0x00000478)

static inline uint32_t m154_rx_mpdu_30_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_30_ADDR);
}

static inline void m154_rx_mpdu_30_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_30_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_123_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_122_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_121_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_120_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_31_ADDR (M154_BASE + 0x0000047c)

static inline uint32_t m154_rx_mpdu_31_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_31_ADDR);
}

static inline void m154_rx_mpdu_31_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_31_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_127_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_126_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_125_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_124_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_32_ADDR (M154_BASE + 0x00000480)

static inline uint32_t m154_rx_mpdu_32_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_32_ADDR);
}

static inline void m154_rx_mpdu_32_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_32_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_131_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_130_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_129_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_128_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_32_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_33_ADDR (M154_BASE + 0x00000484)

static inline uint32_t m154_rx_mpdu_33_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_33_ADDR);
}

static inline void m154_rx_mpdu_33_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_33_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_135_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_134_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_133_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_132_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_33_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_34_ADDR (M154_BASE + 0x00000488)

static inline uint32_t m154_rx_mpdu_34_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_34_ADDR);
}

static inline void m154_rx_mpdu_34_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_34_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_139_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_138_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_137_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_136_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_34_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_35_ADDR (M154_BASE + 0x0000048c)

static inline uint32_t m154_rx_mpdu_35_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_35_ADDR);
}

static inline void m154_rx_mpdu_35_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_35_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_143_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_142_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_141_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_140_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_35_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_36_ADDR (M154_BASE + 0x00000490)

static inline uint32_t m154_rx_mpdu_36_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_36_ADDR);
}

static inline void m154_rx_mpdu_36_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_36_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_147_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_146_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_145_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_144_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_36_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_37_ADDR (M154_BASE + 0x00000494)

static inline uint32_t m154_rx_mpdu_37_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_37_ADDR);
}

static inline void m154_rx_mpdu_37_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_37_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_151_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_150_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_149_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_148_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_37_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_38_ADDR (M154_BASE + 0x00000498)

static inline uint32_t m154_rx_mpdu_38_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_38_ADDR);
}

static inline void m154_rx_mpdu_38_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_38_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_155_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_154_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_153_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_152_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_38_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_39_ADDR (M154_BASE + 0x0000049c)

static inline uint32_t m154_rx_mpdu_39_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_39_ADDR);
}

static inline void m154_rx_mpdu_39_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_39_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_159_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_158_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_157_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_156_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_39_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_40_ADDR (M154_BASE + 0x000004a0)

static inline uint32_t m154_rx_mpdu_40_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_40_ADDR);
}

static inline void m154_rx_mpdu_40_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_40_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_163_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_162_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_161_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_160_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_40_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_41_ADDR (M154_BASE + 0x000004a4)

static inline uint32_t m154_rx_mpdu_41_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_41_ADDR);
}

static inline void m154_rx_mpdu_41_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_41_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_167_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_166_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_165_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_164_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_41_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_42_ADDR (M154_BASE + 0x000004a8)

static inline uint32_t m154_rx_mpdu_42_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_42_ADDR);
}

static inline void m154_rx_mpdu_42_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_42_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_171_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_170_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_169_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_168_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_42_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_43_ADDR (M154_BASE + 0x000004ac)

static inline uint32_t m154_rx_mpdu_43_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_43_ADDR);
}

static inline void m154_rx_mpdu_43_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_43_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_175_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_174_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_173_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_172_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_43_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_44_ADDR (M154_BASE + 0x000004b0)

static inline uint32_t m154_rx_mpdu_44_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_44_ADDR);
}

static inline void m154_rx_mpdu_44_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_44_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_179_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_178_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_177_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_176_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_44_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_45_ADDR (M154_BASE + 0x000004b4)

static inline uint32_t m154_rx_mpdu_45_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_45_ADDR);
}

static inline void m154_rx_mpdu_45_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_45_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_183_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_182_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_181_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_180_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_45_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_46_ADDR (M154_BASE + 0x000004b8)

static inline uint32_t m154_rx_mpdu_46_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_46_ADDR);
}

static inline void m154_rx_mpdu_46_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_46_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_187_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_186_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_185_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_184_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_46_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_47_ADDR (M154_BASE + 0x000004bc)

static inline uint32_t m154_rx_mpdu_47_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_47_ADDR);
}

static inline void m154_rx_mpdu_47_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_47_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_191_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_190_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_189_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_188_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_47_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_48_ADDR (M154_BASE + 0x000004c0)

static inline uint32_t m154_rx_mpdu_48_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_48_ADDR);
}

static inline void m154_rx_mpdu_48_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_48_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_195_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_194_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_193_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_192_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_48_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_49_ADDR (M154_BASE + 0x000004c4)

static inline uint32_t m154_rx_mpdu_49_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_49_ADDR);
}

static inline void m154_rx_mpdu_49_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_49_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_199_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_198_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_197_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_196_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_49_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_50_ADDR (M154_BASE + 0x000004c8)

static inline uint32_t m154_rx_mpdu_50_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_50_ADDR);
}

static inline void m154_rx_mpdu_50_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_50_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_203_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_202_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_201_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_200_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_50_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_51_ADDR (M154_BASE + 0x000004cc)

static inline uint32_t m154_rx_mpdu_51_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_51_ADDR);
}

static inline void m154_rx_mpdu_51_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_51_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_207_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_206_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_205_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_204_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_51_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_52_ADDR (M154_BASE + 0x000004d0)

static inline uint32_t m154_rx_mpdu_52_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_52_ADDR);
}

static inline void m154_rx_mpdu_52_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_52_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_211_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_210_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_209_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_208_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_52_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_53_ADDR (M154_BASE + 0x000004d4)

static inline uint32_t m154_rx_mpdu_53_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_53_ADDR);
}

static inline void m154_rx_mpdu_53_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_53_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_215_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_214_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_213_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_212_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_53_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_54_ADDR (M154_BASE + 0x000004d8)

static inline uint32_t m154_rx_mpdu_54_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_54_ADDR);
}

static inline void m154_rx_mpdu_54_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_54_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_219_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_218_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_217_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_216_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_54_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_55_ADDR (M154_BASE + 0x000004dc)

static inline uint32_t m154_rx_mpdu_55_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_55_ADDR);
}

static inline void m154_rx_mpdu_55_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_55_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_223_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_222_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_221_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_220_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_55_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_56_ADDR (M154_BASE + 0x000004e0)

static inline uint32_t m154_rx_mpdu_56_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_56_ADDR);
}

static inline void m154_rx_mpdu_56_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_56_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_227_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_226_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_225_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_224_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_56_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_57_ADDR (M154_BASE + 0x000004e4)

static inline uint32_t m154_rx_mpdu_57_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_57_ADDR);
}

static inline void m154_rx_mpdu_57_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_57_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_231_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_230_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_229_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_228_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_57_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_58_ADDR (M154_BASE + 0x000004e8)

static inline uint32_t m154_rx_mpdu_58_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_58_ADDR);
}

static inline void m154_rx_mpdu_58_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_58_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_235_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_234_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_233_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_232_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_58_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_59_ADDR (M154_BASE + 0x000004ec)

static inline uint32_t m154_rx_mpdu_59_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_59_ADDR);
}

static inline void m154_rx_mpdu_59_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_59_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_239_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_238_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_237_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_236_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_59_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_60_ADDR (M154_BASE + 0x000004f0)

static inline uint32_t m154_rx_mpdu_60_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_60_ADDR);
}

static inline void m154_rx_mpdu_60_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_60_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_243_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_242_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_241_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_240_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_60_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_61_ADDR (M154_BASE + 0x000004f4)

static inline uint32_t m154_rx_mpdu_61_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_61_ADDR);
}

static inline void m154_rx_mpdu_61_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_61_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_247_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_246_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_245_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_244_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_61_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_62_ADDR (M154_BASE + 0x000004f8)

static inline uint32_t m154_rx_mpdu_62_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_62_ADDR);
}

static inline void m154_rx_mpdu_62_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_62_ADDR,x);
}

static inline uint32_t m154_rx_mpdu_251_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_250_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_249_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_248_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_62_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_MPDU_63_ADDR (M154_BASE + 0x000004fc)

static inline uint32_t m154_rx_mpdu_63_get(void)
{
    return REG_PL_RD(M154_RX_MPDU_63_ADDR);
}

static inline void m154_rx_mpdu_63_set(uint32_t x)
{
    REG_PL_WR(M154_RX_MPDU_63_ADDR,x);
}

static inline uint32_t m154_rx_phr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_rx_mpdu_fcs_15_08_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_rx_mpdu_fcs_07_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_rx_mpdu_252_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_MPDU_63_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_RX_PPDU_STATUS_ADDR (M154_BASE + 0x00000500)

static inline uint32_t m154_rx_ppdu_status_get(void)
{
    return REG_PL_RD(M154_RX_PPDU_STATUS_ADDR);
}

static inline void m154_rx_ppdu_status_set(uint32_t x)
{
    REG_PL_WR(M154_RX_PPDU_STATUS_ADDR,x);
}

static inline uint32_t m154_rx_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_RX_PPDU_STATUS_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}


#define M154_NBR_VALID_31_00_ADDR (M154_BASE + 0x00000600)

static inline uint32_t m154_nbr_valid_31_00_get(void)
{
    return REG_PL_RD(M154_NBR_VALID_31_00_ADDR);
}

static inline void m154_nbr_valid_31_00_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_VALID_31_00_ADDR,x);
}

static inline uint32_t m154_nbr_valid_31_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_31_00_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_valid_31_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_31_00_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_VALID_31_00_ADDR,localVal);
}


#define M154_NBR_VALID_63_32_ADDR (M154_BASE + 0x00000604)

static inline uint32_t m154_nbr_valid_63_32_get(void)
{
    return REG_PL_RD(M154_NBR_VALID_63_32_ADDR);
}

static inline void m154_nbr_valid_63_32_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_VALID_63_32_ADDR,x);
}

static inline uint32_t m154_nbr_valid_63_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_63_32_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_valid_63_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_63_32_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_VALID_63_32_ADDR,localVal);
}


#define M154_NBR_VALID_95_64_ADDR (M154_BASE + 0x00000608)

static inline uint32_t m154_nbr_valid_95_64_get(void)
{
    return REG_PL_RD(M154_NBR_VALID_95_64_ADDR);
}

static inline void m154_nbr_valid_95_64_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_VALID_95_64_ADDR,x);
}

static inline uint32_t m154_nbr_valid_95_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_95_64_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_valid_95_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_95_64_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_VALID_95_64_ADDR,localVal);
}


#define M154_NBR_VALID_127_96_ADDR (M154_BASE + 0x0000060c)

static inline uint32_t m154_nbr_valid_127_96_get(void)
{
    return REG_PL_RD(M154_NBR_VALID_127_96_ADDR);
}

static inline void m154_nbr_valid_127_96_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_VALID_127_96_ADDR,x);
}

static inline uint32_t m154_nbr_valid_127_96_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_127_96_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_valid_127_96_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_VALID_127_96_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_VALID_127_96_ADDR,localVal);
}


#define M154_NBR_EXT_ADDR_MODE_31_00_ADDR (M154_BASE + 0x00000610)

static inline uint32_t m154_nbr_ext_addr_mode_31_00_get(void)
{
    return REG_PL_RD(M154_NBR_EXT_ADDR_MODE_31_00_ADDR);
}

static inline void m154_nbr_ext_addr_mode_31_00_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_31_00_ADDR,x);
}

static inline uint32_t m154_nbr_ext_addr_mode_31_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_31_00_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_ext_addr_mode_31_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_31_00_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_31_00_ADDR,localVal);
}


#define M154_NBR_EXT_ADDR_MODE_63_32_ADDR (M154_BASE + 0x00000614)

static inline uint32_t m154_nbr_ext_addr_mode_63_32_get(void)
{
    return REG_PL_RD(M154_NBR_EXT_ADDR_MODE_63_32_ADDR);
}

static inline void m154_nbr_ext_addr_mode_63_32_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_63_32_ADDR,x);
}

static inline uint32_t m154_nbr_ext_addr_mode_63_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_63_32_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_ext_addr_mode_63_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_63_32_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_63_32_ADDR,localVal);
}


#define M154_NBR_EXT_ADDR_MODE_95_64_ADDR (M154_BASE + 0x00000618)

static inline uint32_t m154_nbr_ext_addr_mode_95_64_get(void)
{
    return REG_PL_RD(M154_NBR_EXT_ADDR_MODE_95_64_ADDR);
}

static inline void m154_nbr_ext_addr_mode_95_64_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_95_64_ADDR,x);
}

static inline uint32_t m154_nbr_ext_addr_mode_95_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_95_64_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_ext_addr_mode_95_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_95_64_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_95_64_ADDR,localVal);
}


#define M154_NBR_EXT_ADDR_MODE_127_96_ADDR (M154_BASE + 0x0000061c)

static inline uint32_t m154_nbr_ext_addr_mode_127_96_get(void)
{
    return REG_PL_RD(M154_NBR_EXT_ADDR_MODE_127_96_ADDR);
}

static inline void m154_nbr_ext_addr_mode_127_96_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_127_96_ADDR,x);
}

static inline uint32_t m154_nbr_ext_addr_mode_127_96_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_127_96_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_ext_addr_mode_127_96_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_EXT_ADDR_MODE_127_96_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_EXT_ADDR_MODE_127_96_ADDR,localVal);
}


#define M154_NBR_FP_31_00_ADDR (M154_BASE + 0x00000620)

static inline uint32_t m154_nbr_fp_31_00_get(void)
{
    return REG_PL_RD(M154_NBR_FP_31_00_ADDR);
}

static inline void m154_nbr_fp_31_00_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_FP_31_00_ADDR,x);
}

static inline uint32_t m154_nbr_fp_31_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_31_00_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_fp_31_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_31_00_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_FP_31_00_ADDR,localVal);
}


#define M154_NBR_FP_63_32_ADDR (M154_BASE + 0x00000624)

static inline uint32_t m154_nbr_fp_63_32_get(void)
{
    return REG_PL_RD(M154_NBR_FP_63_32_ADDR);
}

static inline void m154_nbr_fp_63_32_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_FP_63_32_ADDR,x);
}

static inline uint32_t m154_nbr_fp_63_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_63_32_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_fp_63_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_63_32_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_FP_63_32_ADDR,localVal);
}


#define M154_NBR_FP_95_64_ADDR (M154_BASE + 0x00000628)

static inline uint32_t m154_nbr_fp_95_64_get(void)
{
    return REG_PL_RD(M154_NBR_FP_95_64_ADDR);
}

static inline void m154_nbr_fp_95_64_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_FP_95_64_ADDR,x);
}

static inline uint32_t m154_nbr_fp_95_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_95_64_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_fp_95_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_95_64_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_FP_95_64_ADDR,localVal);
}


#define M154_NBR_FP_127_96_ADDR (M154_BASE + 0x0000062c)

static inline uint32_t m154_nbr_fp_127_96_get(void)
{
    return REG_PL_RD(M154_NBR_FP_127_96_ADDR);
}

static inline void m154_nbr_fp_127_96_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_FP_127_96_ADDR,x);
}

static inline uint32_t m154_nbr_fp_127_96_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_127_96_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_nbr_fp_127_96_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_FP_127_96_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_NBR_FP_127_96_ADDR,localVal);
}


#define M154_NBR_ADDR_01_00_ADDR (M154_BASE + 0x00000630)

static inline uint32_t m154_nbr_addr_01_00_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_01_00_ADDR);
}

static inline void m154_nbr_addr_01_00_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_01_00_ADDR,x);
}

static inline uint32_t m154_nbr_addr_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_01_00_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_01_00_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_01_00_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_01_00_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_01_00_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_01_00_ADDR,localVal);
}


#define M154_NBR_ADDR_03_02_ADDR (M154_BASE + 0x00000634)

static inline uint32_t m154_nbr_addr_03_02_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_03_02_ADDR);
}

static inline void m154_nbr_addr_03_02_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_03_02_ADDR,x);
}

static inline uint32_t m154_nbr_addr_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_03_02_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_03_02_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_03_02_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_03_02_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_03_02_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_03_02_ADDR,localVal);
}


#define M154_NBR_ADDR_05_04_ADDR (M154_BASE + 0x00000638)

static inline uint32_t m154_nbr_addr_05_04_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_05_04_ADDR);
}

static inline void m154_nbr_addr_05_04_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_05_04_ADDR,x);
}

static inline uint32_t m154_nbr_addr_05_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_05_04_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_05_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_05_04_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_05_04_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_04_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_05_04_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_04_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_05_04_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_05_04_ADDR,localVal);
}


#define M154_NBR_ADDR_07_06_ADDR (M154_BASE + 0x0000063c)

static inline uint32_t m154_nbr_addr_07_06_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_07_06_ADDR);
}

static inline void m154_nbr_addr_07_06_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_07_06_ADDR,x);
}

static inline uint32_t m154_nbr_addr_07_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_07_06_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_07_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_07_06_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_07_06_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_06_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_07_06_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_06_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_07_06_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_07_06_ADDR,localVal);
}


#define M154_NBR_ADDR_09_08_ADDR (M154_BASE + 0x00000640)

static inline uint32_t m154_nbr_addr_09_08_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_09_08_ADDR);
}

static inline void m154_nbr_addr_09_08_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_09_08_ADDR,x);
}

static inline uint32_t m154_nbr_addr_09_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_09_08_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_09_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_09_08_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_09_08_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_08_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_09_08_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_08_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_09_08_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_09_08_ADDR,localVal);
}


#define M154_NBR_ADDR_11_10_ADDR (M154_BASE + 0x00000644)

static inline uint32_t m154_nbr_addr_11_10_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_11_10_ADDR);
}

static inline void m154_nbr_addr_11_10_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_11_10_ADDR,x);
}

static inline uint32_t m154_nbr_addr_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_11_10_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_11_10_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_11_10_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_11_10_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_11_10_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_11_10_ADDR,localVal);
}


#define M154_NBR_ADDR_13_12_ADDR (M154_BASE + 0x00000648)

static inline uint32_t m154_nbr_addr_13_12_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_13_12_ADDR);
}

static inline void m154_nbr_addr_13_12_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_13_12_ADDR,x);
}

static inline uint32_t m154_nbr_addr_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_13_12_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_13_12_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_13_12_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_13_12_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_13_12_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_13_12_ADDR,localVal);
}


#define M154_NBR_ADDR_15_14_ADDR (M154_BASE + 0x0000064c)

static inline uint32_t m154_nbr_addr_15_14_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_15_14_ADDR);
}

static inline void m154_nbr_addr_15_14_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_15_14_ADDR,x);
}

static inline uint32_t m154_nbr_addr_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_15_14_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_15_14_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_15_14_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_15_14_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_15_14_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_15_14_ADDR,localVal);
}


#define M154_NBR_ADDR_17_16_ADDR (M154_BASE + 0x00000650)

static inline uint32_t m154_nbr_addr_17_16_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_17_16_ADDR);
}

static inline void m154_nbr_addr_17_16_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_17_16_ADDR,x);
}

static inline uint32_t m154_nbr_addr_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_17_16_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_17_16_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_17_16_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_17_16_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_17_16_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_17_16_ADDR,localVal);
}


#define M154_NBR_ADDR_19_18_ADDR (M154_BASE + 0x00000654)

static inline uint32_t m154_nbr_addr_19_18_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_19_18_ADDR);
}

static inline void m154_nbr_addr_19_18_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_19_18_ADDR,x);
}

static inline uint32_t m154_nbr_addr_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_19_18_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_19_18_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_19_18_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_19_18_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_19_18_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_19_18_ADDR,localVal);
}


#define M154_NBR_ADDR_21_20_ADDR (M154_BASE + 0x00000658)

static inline uint32_t m154_nbr_addr_21_20_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_21_20_ADDR);
}

static inline void m154_nbr_addr_21_20_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_21_20_ADDR,x);
}

static inline uint32_t m154_nbr_addr_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_21_20_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_21_20_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_21_20_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_21_20_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_21_20_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_21_20_ADDR,localVal);
}


#define M154_NBR_ADDR_23_22_ADDR (M154_BASE + 0x0000065c)

static inline uint32_t m154_nbr_addr_23_22_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_23_22_ADDR);
}

static inline void m154_nbr_addr_23_22_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_23_22_ADDR,x);
}

static inline uint32_t m154_nbr_addr_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_23_22_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_23_22_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_23_22_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_23_22_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_23_22_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_23_22_ADDR,localVal);
}


#define M154_NBR_ADDR_25_24_ADDR (M154_BASE + 0x00000660)

static inline uint32_t m154_nbr_addr_25_24_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_25_24_ADDR);
}

static inline void m154_nbr_addr_25_24_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_25_24_ADDR,x);
}

static inline uint32_t m154_nbr_addr_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_25_24_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_25_24_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_25_24_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_25_24_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_25_24_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_25_24_ADDR,localVal);
}


#define M154_NBR_ADDR_27_26_ADDR (M154_BASE + 0x00000664)

static inline uint32_t m154_nbr_addr_27_26_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_27_26_ADDR);
}

static inline void m154_nbr_addr_27_26_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_27_26_ADDR,x);
}

static inline uint32_t m154_nbr_addr_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_27_26_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_27_26_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_27_26_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_27_26_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_27_26_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_27_26_ADDR,localVal);
}


#define M154_NBR_ADDR_29_28_ADDR (M154_BASE + 0x00000668)

static inline uint32_t m154_nbr_addr_29_28_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_29_28_ADDR);
}

static inline void m154_nbr_addr_29_28_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_29_28_ADDR,x);
}

static inline uint32_t m154_nbr_addr_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_29_28_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_29_28_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_29_28_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_29_28_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_29_28_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_29_28_ADDR,localVal);
}


#define M154_NBR_ADDR_31_30_ADDR (M154_BASE + 0x0000066c)

static inline uint32_t m154_nbr_addr_31_30_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_31_30_ADDR);
}

static inline void m154_nbr_addr_31_30_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_31_30_ADDR,x);
}

static inline uint32_t m154_nbr_addr_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_31_30_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_31_30_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_31_30_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_31_30_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_31_30_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_31_30_ADDR,localVal);
}


#define M154_NBR_ADDR_33_32_ADDR (M154_BASE + 0x00000670)

static inline uint32_t m154_nbr_addr_33_32_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_33_32_ADDR);
}

static inline void m154_nbr_addr_33_32_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_33_32_ADDR,x);
}

static inline uint32_t m154_nbr_addr_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_33_32_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_33_32_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_33_32_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_33_32_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_33_32_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_33_32_ADDR,localVal);
}


#define M154_NBR_ADDR_35_34_ADDR (M154_BASE + 0x00000674)

static inline uint32_t m154_nbr_addr_35_34_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_35_34_ADDR);
}

static inline void m154_nbr_addr_35_34_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_35_34_ADDR,x);
}

static inline uint32_t m154_nbr_addr_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_35_34_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_35_34_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_35_34_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_35_34_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_35_34_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_35_34_ADDR,localVal);
}


#define M154_NBR_ADDR_37_36_ADDR (M154_BASE + 0x00000678)

static inline uint32_t m154_nbr_addr_37_36_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_37_36_ADDR);
}

static inline void m154_nbr_addr_37_36_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_37_36_ADDR,x);
}

static inline uint32_t m154_nbr_addr_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_37_36_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_37_36_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_37_36_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_37_36_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_37_36_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_37_36_ADDR,localVal);
}


#define M154_NBR_ADDR_39_38_ADDR (M154_BASE + 0x0000067c)

static inline uint32_t m154_nbr_addr_39_38_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_39_38_ADDR);
}

static inline void m154_nbr_addr_39_38_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_39_38_ADDR,x);
}

static inline uint32_t m154_nbr_addr_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_39_38_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_39_38_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_39_38_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_39_38_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_39_38_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_39_38_ADDR,localVal);
}


#define M154_NBR_ADDR_41_40_ADDR (M154_BASE + 0x00000680)

static inline uint32_t m154_nbr_addr_41_40_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_41_40_ADDR);
}

static inline void m154_nbr_addr_41_40_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_41_40_ADDR,x);
}

static inline uint32_t m154_nbr_addr_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_41_40_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_41_40_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_41_40_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_41_40_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_41_40_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_41_40_ADDR,localVal);
}


#define M154_NBR_ADDR_43_42_ADDR (M154_BASE + 0x00000684)

static inline uint32_t m154_nbr_addr_43_42_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_43_42_ADDR);
}

static inline void m154_nbr_addr_43_42_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_43_42_ADDR,x);
}

static inline uint32_t m154_nbr_addr_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_43_42_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_43_42_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_43_42_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_43_42_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_43_42_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_43_42_ADDR,localVal);
}


#define M154_NBR_ADDR_45_44_ADDR (M154_BASE + 0x00000688)

static inline uint32_t m154_nbr_addr_45_44_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_45_44_ADDR);
}

static inline void m154_nbr_addr_45_44_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_45_44_ADDR,x);
}

static inline uint32_t m154_nbr_addr_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_45_44_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_45_44_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_45_44_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_45_44_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_45_44_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_45_44_ADDR,localVal);
}


#define M154_NBR_ADDR_47_46_ADDR (M154_BASE + 0x0000068c)

static inline uint32_t m154_nbr_addr_47_46_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_47_46_ADDR);
}

static inline void m154_nbr_addr_47_46_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_47_46_ADDR,x);
}

static inline uint32_t m154_nbr_addr_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_47_46_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_47_46_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_47_46_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_47_46_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_47_46_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_47_46_ADDR,localVal);
}


#define M154_NBR_ADDR_49_48_ADDR (M154_BASE + 0x00000690)

static inline uint32_t m154_nbr_addr_49_48_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_49_48_ADDR);
}

static inline void m154_nbr_addr_49_48_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_49_48_ADDR,x);
}

static inline uint32_t m154_nbr_addr_49_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_49_48_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_49_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_49_48_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_49_48_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_48_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_49_48_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_48_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_49_48_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_49_48_ADDR,localVal);
}


#define M154_NBR_ADDR_51_50_ADDR (M154_BASE + 0x00000694)

static inline uint32_t m154_nbr_addr_51_50_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_51_50_ADDR);
}

static inline void m154_nbr_addr_51_50_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_51_50_ADDR,x);
}

static inline uint32_t m154_nbr_addr_51_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_51_50_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_51_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_51_50_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_51_50_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_50_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_51_50_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_50_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_51_50_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_51_50_ADDR,localVal);
}


#define M154_NBR_ADDR_53_52_ADDR (M154_BASE + 0x00000698)

static inline uint32_t m154_nbr_addr_53_52_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_53_52_ADDR);
}

static inline void m154_nbr_addr_53_52_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_53_52_ADDR,x);
}

static inline uint32_t m154_nbr_addr_53_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_53_52_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_53_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_53_52_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_53_52_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_52_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_53_52_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_52_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_53_52_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_53_52_ADDR,localVal);
}


#define M154_NBR_ADDR_55_54_ADDR (M154_BASE + 0x0000069c)

static inline uint32_t m154_nbr_addr_55_54_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_55_54_ADDR);
}

static inline void m154_nbr_addr_55_54_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_55_54_ADDR,x);
}

static inline uint32_t m154_nbr_addr_55_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_55_54_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_55_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_55_54_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_55_54_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_54_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_55_54_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_54_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_55_54_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_55_54_ADDR,localVal);
}


#define M154_NBR_ADDR_57_56_ADDR (M154_BASE + 0x000006a0)

static inline uint32_t m154_nbr_addr_57_56_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_57_56_ADDR);
}

static inline void m154_nbr_addr_57_56_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_57_56_ADDR,x);
}

static inline uint32_t m154_nbr_addr_57_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_57_56_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_57_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_57_56_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_57_56_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_56_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_57_56_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_57_56_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_57_56_ADDR,localVal);
}


#define M154_NBR_ADDR_59_58_ADDR (M154_BASE + 0x000006a4)

static inline uint32_t m154_nbr_addr_59_58_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_59_58_ADDR);
}

static inline void m154_nbr_addr_59_58_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_59_58_ADDR,x);
}

static inline uint32_t m154_nbr_addr_59_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_59_58_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_59_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_59_58_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_59_58_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_58_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_59_58_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_58_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_59_58_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_59_58_ADDR,localVal);
}


#define M154_NBR_ADDR_61_60_ADDR (M154_BASE + 0x000006a8)

static inline uint32_t m154_nbr_addr_61_60_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_61_60_ADDR);
}

static inline void m154_nbr_addr_61_60_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_61_60_ADDR,x);
}

static inline uint32_t m154_nbr_addr_61_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_61_60_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_61_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_61_60_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_61_60_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_60_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_61_60_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_60_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_61_60_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_61_60_ADDR,localVal);
}


#define M154_NBR_ADDR_63_62_ADDR (M154_BASE + 0x000006ac)

static inline uint32_t m154_nbr_addr_63_62_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_63_62_ADDR);
}

static inline void m154_nbr_addr_63_62_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_63_62_ADDR,x);
}

static inline uint32_t m154_nbr_addr_63_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_63_62_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_63_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_63_62_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_63_62_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_62_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_63_62_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_62_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_63_62_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_63_62_ADDR,localVal);
}


#define M154_NBR_ADDR_65_64_ADDR (M154_BASE + 0x000006b0)

static inline uint32_t m154_nbr_addr_65_64_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_65_64_ADDR);
}

static inline void m154_nbr_addr_65_64_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_65_64_ADDR,x);
}

static inline uint32_t m154_nbr_addr_65_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_65_64_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_65_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_65_64_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_65_64_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_65_64_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_65_64_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_65_64_ADDR,localVal);
}


#define M154_NBR_ADDR_67_66_ADDR (M154_BASE + 0x000006b4)

static inline uint32_t m154_nbr_addr_67_66_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_67_66_ADDR);
}

static inline void m154_nbr_addr_67_66_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_67_66_ADDR,x);
}

static inline uint32_t m154_nbr_addr_67_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_67_66_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_67_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_67_66_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_67_66_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_66_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_67_66_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_66_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_67_66_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_67_66_ADDR,localVal);
}


#define M154_NBR_ADDR_69_68_ADDR (M154_BASE + 0x000006b8)

static inline uint32_t m154_nbr_addr_69_68_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_69_68_ADDR);
}

static inline void m154_nbr_addr_69_68_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_69_68_ADDR,x);
}

static inline uint32_t m154_nbr_addr_69_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_69_68_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_69_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_69_68_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_69_68_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_68_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_69_68_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_68_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_69_68_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_69_68_ADDR,localVal);
}


#define M154_NBR_ADDR_71_70_ADDR (M154_BASE + 0x000006bc)

static inline uint32_t m154_nbr_addr_71_70_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_71_70_ADDR);
}

static inline void m154_nbr_addr_71_70_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_71_70_ADDR,x);
}

static inline uint32_t m154_nbr_addr_71_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_71_70_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_71_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_71_70_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_71_70_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_70_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_71_70_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_70_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_71_70_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_71_70_ADDR,localVal);
}


#define M154_NBR_ADDR_73_72_ADDR (M154_BASE + 0x000006c0)

static inline uint32_t m154_nbr_addr_73_72_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_73_72_ADDR);
}

static inline void m154_nbr_addr_73_72_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_73_72_ADDR,x);
}

static inline uint32_t m154_nbr_addr_73_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_73_72_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_73_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_73_72_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_73_72_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_72_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_73_72_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_72_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_73_72_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_73_72_ADDR,localVal);
}


#define M154_NBR_ADDR_75_74_ADDR (M154_BASE + 0x000006c4)

static inline uint32_t m154_nbr_addr_75_74_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_75_74_ADDR);
}

static inline void m154_nbr_addr_75_74_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_75_74_ADDR,x);
}

static inline uint32_t m154_nbr_addr_75_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_75_74_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_75_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_75_74_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_75_74_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_74_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_75_74_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_74_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_75_74_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_75_74_ADDR,localVal);
}


#define M154_NBR_ADDR_77_76_ADDR (M154_BASE + 0x000006c8)

static inline uint32_t m154_nbr_addr_77_76_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_77_76_ADDR);
}

static inline void m154_nbr_addr_77_76_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_77_76_ADDR,x);
}

static inline uint32_t m154_nbr_addr_77_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_77_76_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_77_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_77_76_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_77_76_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_76_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_77_76_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_76_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_77_76_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_77_76_ADDR,localVal);
}


#define M154_NBR_ADDR_79_78_ADDR (M154_BASE + 0x000006cc)

static inline uint32_t m154_nbr_addr_79_78_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_79_78_ADDR);
}

static inline void m154_nbr_addr_79_78_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_79_78_ADDR,x);
}

static inline uint32_t m154_nbr_addr_79_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_79_78_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_79_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_79_78_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_79_78_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_78_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_79_78_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_78_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_79_78_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_79_78_ADDR,localVal);
}


#define M154_NBR_ADDR_81_80_ADDR (M154_BASE + 0x000006d0)

static inline uint32_t m154_nbr_addr_81_80_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_81_80_ADDR);
}

static inline void m154_nbr_addr_81_80_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_81_80_ADDR,x);
}

static inline uint32_t m154_nbr_addr_81_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_81_80_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_81_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_81_80_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_81_80_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_80_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_81_80_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_80_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_81_80_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_81_80_ADDR,localVal);
}


#define M154_NBR_ADDR_83_82_ADDR (M154_BASE + 0x000006d4)

static inline uint32_t m154_nbr_addr_83_82_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_83_82_ADDR);
}

static inline void m154_nbr_addr_83_82_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_83_82_ADDR,x);
}

static inline uint32_t m154_nbr_addr_83_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_83_82_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_83_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_83_82_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_83_82_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_82_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_83_82_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_82_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_83_82_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_83_82_ADDR,localVal);
}


#define M154_NBR_ADDR_85_84_ADDR (M154_BASE + 0x000006d8)

static inline uint32_t m154_nbr_addr_85_84_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_85_84_ADDR);
}

static inline void m154_nbr_addr_85_84_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_85_84_ADDR,x);
}

static inline uint32_t m154_nbr_addr_85_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_85_84_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_85_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_85_84_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_85_84_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_84_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_85_84_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_84_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_85_84_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_85_84_ADDR,localVal);
}


#define M154_NBR_ADDR_87_86_ADDR (M154_BASE + 0x000006dc)

static inline uint32_t m154_nbr_addr_87_86_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_87_86_ADDR);
}

static inline void m154_nbr_addr_87_86_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_87_86_ADDR,x);
}

static inline uint32_t m154_nbr_addr_87_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_87_86_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_87_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_87_86_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_87_86_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_86_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_87_86_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_86_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_87_86_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_87_86_ADDR,localVal);
}


#define M154_NBR_ADDR_89_88_ADDR (M154_BASE + 0x000006e0)

static inline uint32_t m154_nbr_addr_89_88_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_89_88_ADDR);
}

static inline void m154_nbr_addr_89_88_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_89_88_ADDR,x);
}

static inline uint32_t m154_nbr_addr_89_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_89_88_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_89_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_89_88_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_89_88_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_88_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_89_88_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_88_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_89_88_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_89_88_ADDR,localVal);
}


#define M154_NBR_ADDR_91_90_ADDR (M154_BASE + 0x000006e4)

static inline uint32_t m154_nbr_addr_91_90_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_91_90_ADDR);
}

static inline void m154_nbr_addr_91_90_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_91_90_ADDR,x);
}

static inline uint32_t m154_nbr_addr_91_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_91_90_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_91_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_91_90_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_91_90_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_90_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_91_90_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_90_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_91_90_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_91_90_ADDR,localVal);
}


#define M154_NBR_ADDR_93_92_ADDR (M154_BASE + 0x000006e8)

static inline uint32_t m154_nbr_addr_93_92_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_93_92_ADDR);
}

static inline void m154_nbr_addr_93_92_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_93_92_ADDR,x);
}

static inline uint32_t m154_nbr_addr_93_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_93_92_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_93_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_93_92_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_93_92_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_92_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_93_92_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_92_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_93_92_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_93_92_ADDR,localVal);
}


#define M154_NBR_ADDR_95_94_ADDR (M154_BASE + 0x000006ec)

static inline uint32_t m154_nbr_addr_95_94_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_95_94_ADDR);
}

static inline void m154_nbr_addr_95_94_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_95_94_ADDR,x);
}

static inline uint32_t m154_nbr_addr_95_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_95_94_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_95_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_95_94_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_95_94_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_94_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_95_94_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_94_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_95_94_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_95_94_ADDR,localVal);
}


#define M154_NBR_ADDR_97_96_ADDR (M154_BASE + 0x000006f0)

static inline uint32_t m154_nbr_addr_97_96_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_97_96_ADDR);
}

static inline void m154_nbr_addr_97_96_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_97_96_ADDR,x);
}

static inline uint32_t m154_nbr_addr_97_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_97_96_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_97_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_97_96_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_97_96_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_96_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_97_96_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_96_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_97_96_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_97_96_ADDR,localVal);
}


#define M154_NBR_ADDR_99_98_ADDR (M154_BASE + 0x000006f4)

static inline uint32_t m154_nbr_addr_99_98_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_99_98_ADDR);
}

static inline void m154_nbr_addr_99_98_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_99_98_ADDR,x);
}

static inline uint32_t m154_nbr_addr_99_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_99_98_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_99_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_99_98_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_99_98_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_98_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_99_98_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_98_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_99_98_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_99_98_ADDR,localVal);
}


#define M154_NBR_ADDR_101_100_ADDR (M154_BASE + 0x000006f8)

static inline uint32_t m154_nbr_addr_101_100_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_101_100_ADDR);
}

static inline void m154_nbr_addr_101_100_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_101_100_ADDR,x);
}

static inline uint32_t m154_nbr_addr_101_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_101_100_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_101_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_101_100_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_101_100_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_100_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_101_100_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_100_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_101_100_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_101_100_ADDR,localVal);
}


#define M154_NBR_ADDR_103_102_ADDR (M154_BASE + 0x000006fc)

static inline uint32_t m154_nbr_addr_103_102_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_103_102_ADDR);
}

static inline void m154_nbr_addr_103_102_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_103_102_ADDR,x);
}

static inline uint32_t m154_nbr_addr_103_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_103_102_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_103_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_103_102_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_103_102_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_102_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_103_102_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_102_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_103_102_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_103_102_ADDR,localVal);
}


#define M154_NBR_ADDR_105_104_ADDR (M154_BASE + 0x00000700)

static inline uint32_t m154_nbr_addr_105_104_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_105_104_ADDR);
}

static inline void m154_nbr_addr_105_104_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_105_104_ADDR,x);
}

static inline uint32_t m154_nbr_addr_105_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_105_104_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_105_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_105_104_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_105_104_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_104_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_105_104_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_104_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_105_104_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_105_104_ADDR,localVal);
}


#define M154_NBR_ADDR_107_106_ADDR (M154_BASE + 0x00000704)

static inline uint32_t m154_nbr_addr_107_106_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_107_106_ADDR);
}

static inline void m154_nbr_addr_107_106_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_107_106_ADDR,x);
}

static inline uint32_t m154_nbr_addr_107_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_107_106_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_107_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_107_106_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_107_106_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_106_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_107_106_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_106_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_107_106_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_107_106_ADDR,localVal);
}


#define M154_NBR_ADDR_109_108_ADDR (M154_BASE + 0x00000708)

static inline uint32_t m154_nbr_addr_109_108_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_109_108_ADDR);
}

static inline void m154_nbr_addr_109_108_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_109_108_ADDR,x);
}

static inline uint32_t m154_nbr_addr_109_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_109_108_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_109_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_109_108_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_109_108_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_108_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_109_108_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_108_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_109_108_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_109_108_ADDR,localVal);
}


#define M154_NBR_ADDR_111_110_ADDR (M154_BASE + 0x0000070c)

static inline uint32_t m154_nbr_addr_111_110_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_111_110_ADDR);
}

static inline void m154_nbr_addr_111_110_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_111_110_ADDR,x);
}

static inline uint32_t m154_nbr_addr_111_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_111_110_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_111_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_111_110_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_111_110_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_110_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_111_110_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_110_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_111_110_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_111_110_ADDR,localVal);
}


#define M154_NBR_ADDR_113_112_ADDR (M154_BASE + 0x00000710)

static inline uint32_t m154_nbr_addr_113_112_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_113_112_ADDR);
}

static inline void m154_nbr_addr_113_112_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_113_112_ADDR,x);
}

static inline uint32_t m154_nbr_addr_113_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_113_112_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_113_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_113_112_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_113_112_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_112_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_113_112_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_112_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_113_112_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_113_112_ADDR,localVal);
}


#define M154_NBR_ADDR_115_114_ADDR (M154_BASE + 0x00000714)

static inline uint32_t m154_nbr_addr_115_114_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_115_114_ADDR);
}

static inline void m154_nbr_addr_115_114_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_115_114_ADDR,x);
}

static inline uint32_t m154_nbr_addr_115_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_115_114_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_115_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_115_114_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_115_114_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_114_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_115_114_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_114_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_115_114_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_115_114_ADDR,localVal);
}


#define M154_NBR_ADDR_117_116_ADDR (M154_BASE + 0x00000718)

static inline uint32_t m154_nbr_addr_117_116_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_117_116_ADDR);
}

static inline void m154_nbr_addr_117_116_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_117_116_ADDR,x);
}

static inline uint32_t m154_nbr_addr_117_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_117_116_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_117_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_117_116_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_117_116_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_116_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_117_116_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_116_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_117_116_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_117_116_ADDR,localVal);
}


#define M154_NBR_ADDR_119_118_ADDR (M154_BASE + 0x0000071c)

static inline uint32_t m154_nbr_addr_119_118_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_119_118_ADDR);
}

static inline void m154_nbr_addr_119_118_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_119_118_ADDR,x);
}

static inline uint32_t m154_nbr_addr_119_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_119_118_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_119_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_119_118_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_119_118_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_118_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_119_118_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_118_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_119_118_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_119_118_ADDR,localVal);
}


#define M154_NBR_ADDR_121_120_ADDR (M154_BASE + 0x00000720)

static inline uint32_t m154_nbr_addr_121_120_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_121_120_ADDR);
}

static inline void m154_nbr_addr_121_120_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_121_120_ADDR,x);
}

static inline uint32_t m154_nbr_addr_121_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_121_120_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_121_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_121_120_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_121_120_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_120_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_121_120_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_120_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_121_120_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_121_120_ADDR,localVal);
}


#define M154_NBR_ADDR_123_122_ADDR (M154_BASE + 0x00000724)

static inline uint32_t m154_nbr_addr_123_122_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_123_122_ADDR);
}

static inline void m154_nbr_addr_123_122_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_123_122_ADDR,x);
}

static inline uint32_t m154_nbr_addr_123_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_123_122_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_123_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_123_122_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_123_122_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_122_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_123_122_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_122_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_123_122_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_123_122_ADDR,localVal);
}


#define M154_NBR_ADDR_125_124_ADDR (M154_BASE + 0x00000728)

static inline uint32_t m154_nbr_addr_125_124_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_125_124_ADDR);
}

static inline void m154_nbr_addr_125_124_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_125_124_ADDR,x);
}

static inline uint32_t m154_nbr_addr_125_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_125_124_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_125_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_125_124_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_125_124_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_124_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_125_124_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_124_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_125_124_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_125_124_ADDR,localVal);
}


#define M154_NBR_ADDR_127_126_ADDR (M154_BASE + 0x0000072c)

static inline uint32_t m154_nbr_addr_127_126_get(void)
{
    return REG_PL_RD(M154_NBR_ADDR_127_126_ADDR);
}

static inline void m154_nbr_addr_127_126_set(uint32_t x)
{
    REG_PL_WR(M154_NBR_ADDR_127_126_ADDR,x);
}

static inline uint32_t m154_nbr_addr_127_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_127_126_ADDR);
    return ((localVal & ((uint32_t)0xffff0000)) >> 16);
}

static inline void m154_nbr_addr_127_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_127_126_ADDR);
    localVal &= ~((uint32_t)0xffff0000);
    localVal |= (x << 16)&((uint32_t)0xffff0000);
    REG_PL_WR(M154_NBR_ADDR_127_126_ADDR,localVal);
}

static inline uint32_t m154_nbr_addr_126_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_127_126_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_nbr_addr_126_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_NBR_ADDR_127_126_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_NBR_ADDR_127_126_ADDR,localVal);
}


#define M154_LB_MPDU_MISMATCH_31_00_ADDR (M154_BASE + 0x00000770)

static inline uint32_t m154_lb_mpdu_mismatch_31_00_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_MISMATCH_31_00_ADDR);
}

static inline void m154_lb_mpdu_mismatch_31_00_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_MISMATCH_31_00_ADDR,x);
}

static inline uint32_t m154_lb_mismatch_31_downto_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_MISMATCH_31_00_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_LB_MPDU_MISMATCH_63_32_ADDR (M154_BASE + 0x00000774)

static inline uint32_t m154_lb_mpdu_mismatch_63_32_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_MISMATCH_63_32_ADDR);
}

static inline void m154_lb_mpdu_mismatch_63_32_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_MISMATCH_63_32_ADDR,x);
}

static inline uint32_t m154_lb_mismatch_63_downto_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_MISMATCH_63_32_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_LB_MPDU_MISMATCH_95_64_ADDR (M154_BASE + 0x00000778)

static inline uint32_t m154_lb_mpdu_mismatch_95_64_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_MISMATCH_95_64_ADDR);
}

static inline void m154_lb_mpdu_mismatch_95_64_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_MISMATCH_95_64_ADDR,x);
}

static inline uint32_t m154_lb_mismatch_95_downto_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_MISMATCH_95_64_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_LB_MPDU_MISMATCH_127_96_ADDR (M154_BASE + 0x0000077c)

static inline uint32_t m154_lb_mpdu_mismatch_127_96_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_MISMATCH_127_96_ADDR);
}

static inline void m154_lb_mpdu_mismatch_127_96_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_MISMATCH_127_96_ADDR,x);
}

static inline uint32_t m154_lb_mismatch_127_downto_96_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_MISMATCH_127_96_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_LB_MPDU_00_ADDR (M154_BASE + 0x00000780)

static inline uint32_t m154_lb_mpdu_00_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_00_ADDR);
}

static inline void m154_lb_mpdu_00_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_00_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_003_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_002_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_001_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_000_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_00_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_01_ADDR (M154_BASE + 0x00000784)

static inline uint32_t m154_lb_mpdu_01_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_01_ADDR);
}

static inline void m154_lb_mpdu_01_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_01_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_007_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_006_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_005_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_004_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_01_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_02_ADDR (M154_BASE + 0x00000788)

static inline uint32_t m154_lb_mpdu_02_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_02_ADDR);
}

static inline void m154_lb_mpdu_02_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_02_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_011_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_010_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_009_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_008_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_02_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_03_ADDR (M154_BASE + 0x0000078c)

static inline uint32_t m154_lb_mpdu_03_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_03_ADDR);
}

static inline void m154_lb_mpdu_03_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_03_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_015_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_014_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_013_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_012_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_03_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_04_ADDR (M154_BASE + 0x00000790)

static inline uint32_t m154_lb_mpdu_04_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_04_ADDR);
}

static inline void m154_lb_mpdu_04_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_04_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_019_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_018_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_017_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_016_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_04_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_05_ADDR (M154_BASE + 0x00000794)

static inline uint32_t m154_lb_mpdu_05_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_05_ADDR);
}

static inline void m154_lb_mpdu_05_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_05_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_023_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_022_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_021_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_020_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_05_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_06_ADDR (M154_BASE + 0x00000798)

static inline uint32_t m154_lb_mpdu_06_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_06_ADDR);
}

static inline void m154_lb_mpdu_06_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_06_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_027_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_026_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_025_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_024_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_06_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_07_ADDR (M154_BASE + 0x0000079c)

static inline uint32_t m154_lb_mpdu_07_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_07_ADDR);
}

static inline void m154_lb_mpdu_07_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_07_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_031_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_030_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_029_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_028_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_07_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_08_ADDR (M154_BASE + 0x000007a0)

static inline uint32_t m154_lb_mpdu_08_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_08_ADDR);
}

static inline void m154_lb_mpdu_08_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_08_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_035_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_034_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_033_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_032_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_08_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_09_ADDR (M154_BASE + 0x000007a4)

static inline uint32_t m154_lb_mpdu_09_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_09_ADDR);
}

static inline void m154_lb_mpdu_09_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_09_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_039_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_038_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_037_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_036_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_09_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_10_ADDR (M154_BASE + 0x000007a8)

static inline uint32_t m154_lb_mpdu_10_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_10_ADDR);
}

static inline void m154_lb_mpdu_10_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_10_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_043_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_042_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_041_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_040_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_10_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_11_ADDR (M154_BASE + 0x000007ac)

static inline uint32_t m154_lb_mpdu_11_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_11_ADDR);
}

static inline void m154_lb_mpdu_11_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_11_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_047_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_046_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_045_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_044_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_11_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_12_ADDR (M154_BASE + 0x000007b0)

static inline uint32_t m154_lb_mpdu_12_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_12_ADDR);
}

static inline void m154_lb_mpdu_12_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_12_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_051_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_050_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_049_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_048_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_12_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_13_ADDR (M154_BASE + 0x000007b4)

static inline uint32_t m154_lb_mpdu_13_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_13_ADDR);
}

static inline void m154_lb_mpdu_13_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_13_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_055_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_054_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_053_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_052_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_13_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_14_ADDR (M154_BASE + 0x000007b8)

static inline uint32_t m154_lb_mpdu_14_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_14_ADDR);
}

static inline void m154_lb_mpdu_14_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_14_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_059_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_058_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_057_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_056_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_14_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_15_ADDR (M154_BASE + 0x000007bc)

static inline uint32_t m154_lb_mpdu_15_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_15_ADDR);
}

static inline void m154_lb_mpdu_15_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_15_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_063_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_062_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_061_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_060_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_15_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_16_ADDR (M154_BASE + 0x000007c0)

static inline uint32_t m154_lb_mpdu_16_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_16_ADDR);
}

static inline void m154_lb_mpdu_16_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_16_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_067_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_066_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_065_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_064_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_16_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_17_ADDR (M154_BASE + 0x000007c4)

static inline uint32_t m154_lb_mpdu_17_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_17_ADDR);
}

static inline void m154_lb_mpdu_17_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_17_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_071_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_070_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_069_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_068_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_17_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_18_ADDR (M154_BASE + 0x000007c8)

static inline uint32_t m154_lb_mpdu_18_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_18_ADDR);
}

static inline void m154_lb_mpdu_18_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_18_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_075_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_074_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_073_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_072_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_18_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_19_ADDR (M154_BASE + 0x000007cc)

static inline uint32_t m154_lb_mpdu_19_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_19_ADDR);
}

static inline void m154_lb_mpdu_19_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_19_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_079_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_078_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_077_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_076_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_19_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_20_ADDR (M154_BASE + 0x000007d0)

static inline uint32_t m154_lb_mpdu_20_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_20_ADDR);
}

static inline void m154_lb_mpdu_20_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_20_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_083_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_082_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_081_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_080_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_20_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_21_ADDR (M154_BASE + 0x000007d4)

static inline uint32_t m154_lb_mpdu_21_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_21_ADDR);
}

static inline void m154_lb_mpdu_21_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_21_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_087_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_086_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_085_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_084_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_21_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_22_ADDR (M154_BASE + 0x000007d8)

static inline uint32_t m154_lb_mpdu_22_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_22_ADDR);
}

static inline void m154_lb_mpdu_22_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_22_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_091_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_090_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_089_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_088_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_22_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_23_ADDR (M154_BASE + 0x000007dc)

static inline uint32_t m154_lb_mpdu_23_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_23_ADDR);
}

static inline void m154_lb_mpdu_23_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_23_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_095_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_094_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_093_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_092_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_23_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_24_ADDR (M154_BASE + 0x000007e0)

static inline uint32_t m154_lb_mpdu_24_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_24_ADDR);
}

static inline void m154_lb_mpdu_24_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_24_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_099_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_098_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_097_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_096_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_24_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_25_ADDR (M154_BASE + 0x000007e4)

static inline uint32_t m154_lb_mpdu_25_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_25_ADDR);
}

static inline void m154_lb_mpdu_25_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_25_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_103_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_102_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_101_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_100_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_25_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_26_ADDR (M154_BASE + 0x000007e8)

static inline uint32_t m154_lb_mpdu_26_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_26_ADDR);
}

static inline void m154_lb_mpdu_26_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_26_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_107_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_106_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_105_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_104_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_26_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_27_ADDR (M154_BASE + 0x000007ec)

static inline uint32_t m154_lb_mpdu_27_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_27_ADDR);
}

static inline void m154_lb_mpdu_27_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_27_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_111_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_110_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_109_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_108_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_27_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_28_ADDR (M154_BASE + 0x000007f0)

static inline uint32_t m154_lb_mpdu_28_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_28_ADDR);
}

static inline void m154_lb_mpdu_28_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_28_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_115_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_114_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_113_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_112_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_28_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_29_ADDR (M154_BASE + 0x000007f4)

static inline uint32_t m154_lb_mpdu_29_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_29_ADDR);
}

static inline void m154_lb_mpdu_29_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_29_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_119_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_118_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_117_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_116_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_29_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_30_ADDR (M154_BASE + 0x000007f8)

static inline uint32_t m154_lb_mpdu_30_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_30_ADDR);
}

static inline void m154_lb_mpdu_30_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_30_ADDR,x);
}

static inline uint32_t m154_lb_mpdu_123_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_122_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_121_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_120_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_30_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_LB_MPDU_31_ADDR (M154_BASE + 0x000007fc)

static inline uint32_t m154_lb_mpdu_31_get(void)
{
    return REG_PL_RD(M154_LB_MPDU_31_ADDR);
}

static inline void m154_lb_mpdu_31_set(uint32_t x)
{
    REG_PL_WR(M154_LB_MPDU_31_ADDR,x);
}

static inline uint32_t m154_lb_phr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline uint32_t m154_lb_mpdu_fcs_15_08_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t m154_lb_mpdu_fcs_07_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t m154_lb_mpdu_124_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_LB_MPDU_31_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

#endif