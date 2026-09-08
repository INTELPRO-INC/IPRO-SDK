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

#ifndef _REG_BZ_PHY_H_
#define _REG_BZ_PHY_H_
// Date: 20220513

#include <stdint.h>
#include "_reg_access.h"
#include "ipro7.h"

#ifndef BZ_PHY_BASE
#define BZ_PHY_BASE (0x30002000)
#endif



#define PHY_TOP_0_ADDR (BZ_PHY_BASE + 0x00000800)

static inline uint32_t phy_top_0_get(void)
{
    return REG_PL_RD(PHY_TOP_0_ADDR);
}

static inline void phy_top_0_set(uint32_t x)
{
    REG_PL_WR(PHY_TOP_0_ADDR,x);
}

static inline uint32_t bz_phy_dfe_txdfe_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void bz_phy_dfe_txdfe_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_rxdfe_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void bz_phy_dfe_rxdfe_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void bz_phy_adc_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_clk_force_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void bz_phy_dfe_clk_force_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_txmode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void bz_phy_dfe_txmode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_txmode_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void bz_phy_dfe_txmode_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_txdfe_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void bz_phy_txdfe_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_rxmode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x0000000c)) >> 2);
}

static inline void bz_phy_dfe_rxmode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x0000000c);
    localVal |= (x << 2)&((uint32_t)0x0000000c);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_dfe_rxmode_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void bz_phy_dfe_rxmode_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rxdfe_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void bz_phy_rxdfe_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TOP_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PHY_TOP_0_ADDR,localVal);
}


#define PHY_TX_DFE_0_ADDR (BZ_PHY_BASE + 0x00000804)

static inline uint32_t phy_tx_dfe_0_get(void)
{
    return REG_PL_RD(PHY_TX_DFE_0_ADDR);
}

static inline void phy_tx_dfe_0_set(uint32_t x)
{
    REG_PL_WR(PHY_TX_DFE_0_ADDR,x);
}

static inline uint32_t bz_phy_tx_pll_bw_switch_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void bz_phy_tx_pll_bw_switch_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22)&((uint32_t)0xffc00000);
    REG_PL_WR(PHY_TX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_dfe_psf_zig_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void bz_phy_tx_dfe_psf_zig_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(PHY_TX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_dfe_psf_ble2m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x000c0000)) >> 18);
}

static inline void bz_phy_tx_dfe_psf_ble2m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x000c0000);
    localVal |= (x << 18)&((uint32_t)0x000c0000);
    REG_PL_WR(PHY_TX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_dfe_psf_ble1m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00030000)) >> 16);
}

static inline void bz_phy_tx_dfe_psf_ble1m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00030000);
    localVal |= (x << 16)&((uint32_t)0x00030000);
    REG_PL_WR(PHY_TX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_dfe_iq_if_freq_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void bz_phy_tx_dfe_iq_if_freq_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(PHY_TX_DFE_0_ADDR,localVal);
}


#define PHY_TX_DFE_1_ADDR (BZ_PHY_BASE + 0x00000808)

static inline uint32_t phy_tx_dfe_1_get(void)
{
    return REG_PL_RD(PHY_TX_DFE_1_ADDR);
}

static inline void phy_tx_dfe_1_set(uint32_t x)
{
    REG_PL_WR(PHY_TX_DFE_1_ADDR,x);
}

static inline uint32_t bz_phy_tx_rampup_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_tx_rampup_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_TX_DFE_1_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampup_fm_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void bz_phy_tx_rampup_fm_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PHY_TX_DFE_1_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampup_am_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void bz_phy_tx_rampup_am_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PHY_TX_DFE_1_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampup_am_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x003fc000)) >> 14);
}

static inline void bz_phy_tx_rampup_am_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x003fc000);
    localVal |= (x << 14)&((uint32_t)0x003fc000);
    REG_PL_WR(PHY_TX_DFE_1_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_pa_pwr_code_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x00003e00)) >> 9);
}

static inline void bz_phy_tx_pa_pwr_code_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x00003e00);
    localVal |= (x << 9)&((uint32_t)0x00003e00);
    REG_PL_WR(PHY_TX_DFE_1_ADDR,localVal);
}


#define PHY_TX_DFE_2_ADDR (BZ_PHY_BASE + 0x0000080c)

static inline uint32_t phy_tx_dfe_2_get(void)
{
    return REG_PL_RD(PHY_TX_DFE_2_ADDR);
}

static inline void phy_tx_dfe_2_set(uint32_t x)
{
    REG_PL_WR(PHY_TX_DFE_2_ADDR,x);
}

static inline uint32_t bz_phy_tx_rampdn_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_tx_rampdn_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampdn_fm_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void bz_phy_tx_rampdn_fm_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampdn_am_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void bz_phy_tx_rampdn_am_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampdn_am_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x003fc000)) >> 14);
}

static inline void bz_phy_tx_rampdn_am_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x003fc000);
    localVal |= (x << 14)&((uint32_t)0x003fc000);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampdn_am_start_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_tx_rampdn_am_start_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_tx_rampdn_pad0_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_tx_rampdn_pad0_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_TX_DFE_2_ADDR,localVal);
}


#define PHY_RX_DFE_0_ADDR (BZ_PHY_BASE + 0x00000810)

static inline uint32_t phy_rx_dfe_0_get(void)
{
    return REG_PL_RD(PHY_RX_DFE_0_ADDR);
}

static inline void phy_rx_dfe_0_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_DFE_0_ADDR,x);
}

static inline uint32_t bz_phy_rx_dfe_dccan_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void bz_phy_rx_dfe_dccan_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_loop_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void bz_phy_rx_dfe_loop_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_nsr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void bz_phy_rx_dfe_nsr_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_diffosr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void bz_phy_rx_dfe_diffosr_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_dga_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void bz_phy_rx_dfe_dga_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_cs_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void bz_phy_rx_dfe_cs_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_ddc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void bz_phy_rx_dfe_ddc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_toc_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void bz_phy_rx_dfe_toc_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_notch_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void bz_phy_rx_dfe_notch_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PHY_RX_DFE_0_ADDR,localVal);
}


#define PHY_RX_DFE_1_ADDR (BZ_PHY_BASE + 0x00000814)

static inline uint32_t phy_rx_dfe_1_get(void)
{
    return REG_PL_RD(PHY_RX_DFE_1_ADDR);
}

static inline void phy_rx_dfe_1_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_DFE_1_ADDR,x);
}

static inline uint32_t bz_phy_rx_dfe_freq_trk_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void bz_phy_rx_dfe_freq_trk_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PHY_RX_DFE_1_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_freq_trk_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_1_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void bz_phy_rx_dfe_freq_trk_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_1_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0)&((uint32_t)0x00001fff);
    REG_PL_WR(PHY_RX_DFE_1_ADDR,localVal);
}


#define PHY_RX_DFE_2_ADDR (BZ_PHY_BASE + 0x00000818)

static inline uint32_t phy_rx_dfe_2_get(void)
{
    return REG_PL_RD(PHY_RX_DFE_2_ADDR);
}

static inline void phy_rx_dfe_2_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_DFE_2_ADDR,x);
}

static inline uint32_t bz_phy_rx_dfe_dccan_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x01800000)) >> 23);
}

static inline void bz_phy_rx_dfe_dccan_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x01800000);
    localVal |= (x << 23)&((uint32_t)0x01800000);
    REG_PL_WR(PHY_RX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_notch_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void bz_phy_rx_dfe_notch_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(PHY_RX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_notch_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x00300000)) >> 20);
}

static inline void bz_phy_rx_dfe_notch_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x00300000);
    localVal |= (x << 20)&((uint32_t)0x00300000);
    REG_PL_WR(PHY_RX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_notch_rot_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void bz_phy_rx_dfe_notch_rot_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10)&((uint32_t)0x000ffc00);
    REG_PL_WR(PHY_RX_DFE_2_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_notch_rot_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_rx_dfe_notch_rot_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_2_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(PHY_RX_DFE_2_ADDR,localVal);
}


#define PHY_RX_DFE_3_ADDR (BZ_PHY_BASE + 0x0000081c)

static inline uint32_t phy_rx_dfe_3_get(void)
{
    return REG_PL_RD(PHY_RX_DFE_3_ADDR);
}

static inline void phy_rx_dfe_3_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_DFE_3_ADDR,x);
}

static inline uint32_t bz_phy_rx_dfe_toc_fac_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_3_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void bz_phy_rx_dfe_toc_fac_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_3_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(PHY_RX_DFE_3_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_dfe_toc_fac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_3_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void bz_phy_rx_dfe_toc_fac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_DFE_3_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(PHY_RX_DFE_3_ADDR,localVal);
}


#define PHY_RX_ZB_RECEIVER_0_ADDR (BZ_PHY_BASE + 0x00000820)

static inline uint32_t phy_rx_zb_receiver_0_get(void)
{
    return REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
}

static inline void phy_rx_zb_receiver_0_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,x);
}

static inline uint32_t bz_phy_zb_ts_cc_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void bz_phy_zb_ts_cc_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_thres0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0x0fc00000)) >> 22);
}

static inline void bz_phy_zb_ts_thres0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0x0fc00000);
    localVal |= (x << 22)&((uint32_t)0x0fc00000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_thres1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_zb_ts_thres1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_thres2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0x0000fc00)) >> 10);
}

static inline void bz_phy_zb_ts_thres2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0x0000fc00);
    localVal |= (x << 10)&((uint32_t)0x0000fc00);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_thres3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0x000003f0)) >> 4);
}

static inline void bz_phy_zb_ts_thres3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0x000003f0);
    localVal |= (x << 4)&((uint32_t)0x000003f0);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_thres_timing_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void bz_phy_zb_ts_thres_timing_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_0_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0)&((uint32_t)0x00000007);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_0_ADDR,localVal);
}


#define PHY_RX_ZB_RECEIVER_1_ADDR (BZ_PHY_BASE + 0x00000824)

static inline uint32_t phy_rx_zb_receiver_1_get(void)
{
    return REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
}

static inline void phy_rx_zb_receiver_1_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,x);
}

static inline uint32_t bz_phy_zb_dm_plen_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_zb_dm_plen_max_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_plen_min_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void bz_phy_zb_dm_plen_min_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20)&((uint32_t)0x00f00000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ft_coeff_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void bz_phy_zb_ft_coeff_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16)&((uint32_t)0x000f0000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_p_mode_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void bz_phy_zb_dm_p_mode_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_sfd_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0x00001ff0)) >> 4);
}

static inline void bz_phy_zb_dm_sfd_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0x00001ff0);
    localVal |= (x << 4)&((uint32_t)0x00001ff0);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_sfd_cnt_n_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void bz_phy_zb_dm_sfd_cnt_n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_1_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_1_ADDR,localVal);
}


#define PHY_RX_ZB_RECEIVER_2_ADDR (BZ_PHY_BASE + 0x00000828)

static inline uint32_t phy_rx_zb_receiver_2_get(void)
{
    return REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
}

static inline void phy_rx_zb_receiver_2_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,x);
}

static inline uint32_t bz_phy_zb_dm_h_sfd_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_zb_dm_h_sfd_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_sfd_thres_p_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void bz_phy_zb_dm_sfd_thres_p_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_h_sfd_thres_p_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void bz_phy_zb_dm_h_sfd_thres_p_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_ccrl_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void bz_phy_zb_ts_ccrl_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4)&((uint32_t)0x000000f0);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ts_rl_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void bz_phy_zb_ts_rl_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_2_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_2_ADDR,localVal);
}


#define PHY_RX_ZB_RECEIVER_3_ADDR (BZ_PHY_BASE + 0x0000082c)

static inline uint32_t phy_rx_zb_receiver_3_get(void)
{
    return REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
}

static inline void phy_rx_zb_receiver_3_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,x);
}

static inline uint32_t bz_phy_zb_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_zb_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_rx_listen_st_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void bz_phy_zb_fsm_rx_listen_st_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_sfd_time_out_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void bz_phy_zb_fsm_sfd_time_out_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_ed_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void bz_phy_zb_fsm_ed_enable_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_cca_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void bz_phy_zb_fsm_cca_enable_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_lo_time_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x07f00000)) >> 20);
}

static inline void bz_phy_zb_fsm_lo_time_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x07f00000);
    localVal |= (x << 20)&((uint32_t)0x07f00000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_cg_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void bz_phy_zb_fsm_cg_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19)&((uint32_t)0x00080000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_dm_dly_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x0003ff00)) >> 8);
}

static inline void bz_phy_zb_fsm_dm_dly_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x0003ff00);
    localVal |= (x << 8)&((uint32_t)0x0003ff00);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_mf_dly_time_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_zb_fsm_mf_dly_time_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_3_ADDR,localVal);
}


#define PHY_RX_ZB_RECEIVER_4_ADDR (BZ_PHY_BASE + 0x00000830)

static inline uint32_t phy_rx_zb_receiver_4_get(void)
{
    return REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
}

static inline void phy_rx_zb_receiver_4_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,x);
}

static inline uint32_t bz_phy_zb_fsm_sync_timeout_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_zb_fsm_sync_timeout_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_sync_timeout_ms_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x7c000000)) >> 26);
}

static inline void bz_phy_zb_fsm_sync_timeout_ms_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x7c000000);
    localVal |= (x << 26)&((uint32_t)0x7c000000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_alpha_500_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x03c00000)) >> 22);
}

static inline void bz_phy_zb_dm_alpha_500_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x03c00000);
    localVal |= (x << 22)&((uint32_t)0x03c00000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_alpha_1000_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x003c0000)) >> 18);
}

static inline void bz_phy_zb_dm_alpha_1000_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x003c0000);
    localVal |= (x << 18)&((uint32_t)0x003c0000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_alpha_2000_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x0003c000)) >> 14);
}

static inline void bz_phy_zb_dm_alpha_2000_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x0003c000);
    localVal |= (x << 14)&((uint32_t)0x0003c000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ft_init_val_force_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void bz_phy_zb_ft_init_val_force_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_ft_init_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void bz_phy_zb_ft_init_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_ZB_RECEIVER_4_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0)&((uint32_t)0x00001fff);
    REG_PL_WR(PHY_RX_ZB_RECEIVER_4_ADDR,localVal);
}


#define PHY_CCA_0_ADDR (BZ_PHY_BASE + 0x00000834)

static inline uint32_t phy_cca_0_get(void)
{
    return REG_PL_RD(PHY_CCA_0_ADDR);
}

static inline void phy_cca_0_set(uint32_t x)
{
    REG_PL_WR(PHY_CCA_0_ADDR,x);
}

static inline uint32_t bz_phy_phy_sw_ed_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_phy_sw_ed_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_sw_cca_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void bz_phy_phy_sw_cca_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_cca_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void bz_phy_phy_cca_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29)&((uint32_t)0x20000000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_cca_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x18000000)) >> 27);
}

static inline void bz_phy_phy_cca_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x18000000);
    localVal |= (x << 27)&((uint32_t)0x18000000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_cca_dur_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void bz_phy_phy_cca_dur_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24)&((uint32_t)0x07000000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_cca_found_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void bz_phy_phy_cca_found_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_cca_ed_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void bz_phy_phy_cca_ed_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_phy_sw_cca_en_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void bz_phy_phy_sw_cca_en_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_cca_st_ed_meas_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline uint32_t bz_phy_cca_st_cca_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline uint32_t bz_phy_cca_st_ed_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t bz_phy_cca_st_cs_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t bz_phy_cca_st_dm_flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t bz_phy_cca_st_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t bz_phy_cca_st_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void bz_phy_cca_st_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}

static inline uint32_t bz_phy_cca_st_int_set_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void bz_phy_cca_st_int_set_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PHY_CCA_0_ADDR,localVal);
}


#define PHY_BLE_SYNC_0_ADDR (BZ_PHY_BASE + 0x00000838)

static inline uint32_t phy_ble_sync_0_get(void)
{
    return REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
}

static inline void phy_ble_sync_0_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,x);
}

static inline uint32_t bz_phy_ble_aa_offset_coef_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    return ((localVal & ((uint32_t)0x07c00000)) >> 22);
}

static inline void bz_phy_ble_aa_offset_coef_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    localVal &= ~((uint32_t)0x07c00000);
    localVal |= (x << 22)&((uint32_t)0x07c00000);
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_cfoe_avg_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    return ((localVal & ((uint32_t)0x003c0000)) >> 18);
}

static inline void bz_phy_ble_cfoe_avg_l_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    localVal &= ~((uint32_t)0x003c0000);
    localVal |= (x << 18)&((uint32_t)0x003c0000);
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_sync_corr_avg_l_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    return ((localVal & ((uint32_t)0x0003e000)) >> 13);
}

static inline void bz_phy_ble_sync_corr_avg_l_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    localVal &= ~((uint32_t)0x0003e000);
    localVal |= (x << 13)&((uint32_t)0x0003e000);
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_cfoe_alpha_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void bz_phy_blec_cfoe_alpha_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10)&((uint32_t)0x00001c00);
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,localVal);
}

static inline uint32_t bz_phy_t2f_mag_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void bz_phy_t2f_mag_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_0_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0)&((uint32_t)0x000001ff);
    REG_PL_WR(PHY_BLE_SYNC_0_ADDR,localVal);
}


#define PHY_BLE_SYNC_1_ADDR (BZ_PHY_BASE + 0x0000083c)

static inline uint32_t phy_ble_sync_1_get(void)
{
    return REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
}

static inline void phy_ble_sync_1_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_SYNC_1_ADDR,x);
}

static inline uint32_t bz_phy_t2f_filter_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void bz_phy_t2f_filter_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28)&((uint32_t)0x10000000);
    REG_PL_WR(PHY_BLE_SYNC_1_ADDR,localVal);
}

static inline uint32_t bz_phy_t2f_filter_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void bz_phy_t2f_filter_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27)&((uint32_t)0x08000000);
    REG_PL_WR(PHY_BLE_SYNC_1_ADDR,localVal);
}

static inline uint32_t bz_phy_t2f_impulse_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    return ((localVal & ((uint32_t)0x07ffe000)) >> 13);
}

static inline void bz_phy_t2f_impulse_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    localVal &= ~((uint32_t)0x07ffe000);
    localVal |= (x << 13)&((uint32_t)0x07ffe000);
    REG_PL_WR(PHY_BLE_SYNC_1_ADDR,localVal);
}

static inline uint32_t bz_phy_t2f_limite_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void bz_phy_t2f_limite_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_1_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0)&((uint32_t)0x00001fff);
    REG_PL_WR(PHY_BLE_SYNC_1_ADDR,localVal);
}


#define PHY_RESERVED_0_ADDR (BZ_PHY_BASE + 0x00000840)

static inline uint32_t phy_reserved_0_get(void)
{
    return REG_PL_RD(PHY_RESERVED_0_ADDR);
}

static inline void phy_reserved_0_set(uint32_t x)
{
    REG_PL_WR(PHY_RESERVED_0_ADDR,x);
}

static inline uint32_t bz_phy_phy_reserved_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RESERVED_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void bz_phy_phy_reserved_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RESERVED_0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(PHY_RESERVED_0_ADDR,localVal);
}


#define PHY_RX_PROC_TIME_1_ADDR (BZ_PHY_BASE + 0x00000844)

static inline uint32_t phy_rx_proc_time_1_get(void)
{
    return REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
}

static inline void phy_rx_proc_time_1_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_PROC_TIME_1_ADDR,x);
}

static inline uint32_t bz_phy_rx_proc_time_eq_us_2m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void bz_phy_rx_proc_time_eq_us_2m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PHY_RX_PROC_TIME_1_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_proc_time_mlsd_us_2m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void bz_phy_rx_proc_time_mlsd_us_2m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PHY_RX_PROC_TIME_1_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_proc_time_viterbi_us_500k_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_rx_proc_time_viterbi_us_500k_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_RX_PROC_TIME_1_ADDR,localVal);
}


#define PHY_BLE_DFE_0_ADDR (BZ_PHY_BASE + 0x00000848)

static inline uint32_t phy_ble_dfe_0_get(void)
{
    return REG_PL_RD(PHY_BLE_DFE_0_ADDR);
}

static inline void phy_ble_dfe_0_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_DFE_0_ADDR,x);
}

static inline uint32_t bz_phy_ff_coef_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_ff_coef_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_BLE_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ff_coef_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void bz_phy_ff_coef_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PHY_BLE_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ff_coef_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void bz_phy_ff_coef_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PHY_BLE_DFE_0_ADDR,localVal);
}

static inline uint32_t bz_phy_fb_coef_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_fb_coef_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_DFE_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_BLE_DFE_0_ADDR,localVal);
}


#define PHY_BLE_SYNC_2_ADDR (BZ_PHY_BASE + 0x0000084c)

static inline uint32_t phy_ble_sync_2_get(void)
{
    return REG_PL_RD(PHY_BLE_SYNC_2_ADDR);
}

static inline void phy_ble_sync_2_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_SYNC_2_ADDR,x);
}

static inline uint32_t bz_phy_ble_search_offset_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_2_ADDR);
    return ((localVal & ((uint32_t)0x0001f000)) >> 12);
}

static inline void bz_phy_ble_search_offset_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_2_ADDR);
    localVal &= ~((uint32_t)0x0001f000);
    localVal |= (x << 12)&((uint32_t)0x0001f000);
    REG_PL_WR(PHY_BLE_SYNC_2_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_aa_corr_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_2_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void bz_phy_ble_aa_corr_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_SYNC_2_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0)&((uint32_t)0x00000fff);
    REG_PL_WR(PHY_BLE_SYNC_2_ADDR,localVal);
}


#define PHY_BLE_FSM_0_ADDR (BZ_PHY_BASE + 0x00000850)

static inline uint32_t phy_ble_fsm_0_get(void)
{
    return REG_PL_RD(PHY_BLE_FSM_0_ADDR);
}

static inline void phy_ble_fsm_0_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,x);
}

static inline uint32_t bz_phy_ble_fsm_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_ble_fsm_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_fsm_cg_4s_on_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void bz_phy_ble_fsm_cg_4s_on_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30)&((uint32_t)0x40000000);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_demod_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_ble_demod_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_fsm_cfo_peak_dly_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void bz_phy_ble_fsm_cfo_peak_dly_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_fsm_rx_dmtime_1m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void bz_phy_ble_fsm_rx_dmtime_1m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_fsm_rx_dmtime_2m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void bz_phy_ble_fsm_rx_dmtime_2m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_rx_cfo_adj_symb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_ble_rx_cfo_adj_symb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FSM_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_BLE_FSM_0_ADDR,localVal);
}


#define PHY_RX_PROC_TIME_0_ADDR (BZ_PHY_BASE + 0x00000854)

static inline uint32_t phy_rx_proc_time_0_get(void)
{
    return REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
}

static inline void phy_rx_proc_time_0_set(uint32_t x)
{
    REG_PL_WR(PHY_RX_PROC_TIME_0_ADDR,x);
}

static inline uint32_t bz_phy_rx_proc_time_m154_us_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void bz_phy_rx_proc_time_m154_us_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(PHY_RX_PROC_TIME_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_proc_time_eq_us_1m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void bz_phy_rx_proc_time_eq_us_1m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16)&((uint32_t)0x00ff0000);
    REG_PL_WR(PHY_RX_PROC_TIME_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_proc_time_mlsd_us_1m_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void bz_phy_rx_proc_time_mlsd_us_1m_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8)&((uint32_t)0x0000ff00);
    REG_PL_WR(PHY_RX_PROC_TIME_0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_proc_time_viterbi_us_125k_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_rx_proc_time_viterbi_us_125k_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_RX_PROC_TIME_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_RX_PROC_TIME_0_ADDR,localVal);
}


#define PHY_TESTBUS_0_ADDR (BZ_PHY_BASE + 0x00000858)

static inline uint32_t phy_testbus_0_get(void)
{
    return REG_PL_RD(PHY_TESTBUS_0_ADDR);
}

static inline void phy_testbus_0_set(uint32_t x)
{
    REG_PL_WR(PHY_TESTBUS_0_ADDR,x);
}

static inline uint32_t bz_phy_phy_testbus_read_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TESTBUS_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define PHY_TESTBUS_1_ADDR (BZ_PHY_BASE + 0x0000085c)

static inline uint32_t phy_testbus_1_get(void)
{
    return REG_PL_RD(PHY_TESTBUS_1_ADDR);
}

static inline void phy_testbus_1_set(uint32_t x)
{
    REG_PL_WR(PHY_TESTBUS_1_ADDR,x);
}

static inline uint32_t bz_phy_phy_testbus_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_TESTBUS_1_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void bz_phy_phy_testbus_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_TESTBUS_1_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0)&((uint32_t)0x0000000f);
    REG_PL_WR(PHY_TESTBUS_1_ADDR,localVal);
}


#define PHY_STATUS_0_ADDR (BZ_PHY_BASE + 0x00000860)

static inline uint32_t phy_status_0_get(void)
{
    return REG_PL_RD(PHY_STATUS_0_ADDR);
}

static inline void phy_status_0_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_0_ADDR,x);
}

static inline uint32_t bz_phy_sts_agc_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_sts_agc_clear_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_STATUS_0_ADDR,localVal);
}

static inline uint32_t bz_phy_sts_agc_rf_lna_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x00e00000)) >> 21);
}

static inline uint32_t bz_phy_sts_agc_rf_rbb_ind_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline uint32_t bz_phy_sts_agc_agc_rssi_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t bz_phy_sts_agc_fe_dg_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}


#define PHY_STATUS_1_ADDR (BZ_PHY_BASE + 0x00000864)

static inline uint32_t phy_status_1_get(void)
{
    return REG_PL_RD(PHY_STATUS_1_ADDR);
}

static inline void phy_status_1_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_1_ADDR,x);
}

static inline uint32_t bz_phy_sts_zb_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_sts_zb_clear_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_STATUS_1_ADDR,localVal);
}

static inline uint32_t bz_phy_sts_zb_sfd_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x60000000)) >> 29);
}

static inline uint32_t bz_phy_sts_zb_freq_trk_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x1fff0000)) >> 16);
}

static inline uint32_t bz_phy_sts_zb_sfd_corr_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00007fff)) >> 0);
}


#define PHY_STATUS_2_ADDR (BZ_PHY_BASE + 0x00000868)

static inline uint32_t phy_status_2_get(void)
{
    return REG_PL_RD(PHY_STATUS_2_ADDR);
}

static inline void phy_status_2_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_2_ADDR,x);
}

static inline uint32_t bz_phy_sts_zb_peak_val_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x3fff0000)) >> 16);
}

static inline uint32_t bz_phy_sts_zb_peak_val_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00003fff)) >> 0);
}


#define PHY_STATUS_3_ADDR (BZ_PHY_BASE + 0x0000086c)

static inline uint32_t phy_status_3_get(void)
{
    return REG_PL_RD(PHY_STATUS_3_ADDR);
}

static inline void phy_status_3_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_3_ADDR,x);
}

static inline uint32_t bz_phy_sts_zb_peak_loc_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline uint32_t bz_phy_sts_zb_peak_loc_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define PHY_STATUS_4_ADDR (BZ_PHY_BASE + 0x00000870)

static inline uint32_t phy_status_4_get(void)
{
    return REG_PL_RD(PHY_STATUS_4_ADDR);
}

static inline void phy_status_4_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_4_ADDR,x);
}

static inline uint32_t bz_phy_sts_zb_peak_re_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x0fff0000)) >> 16);
}

static inline uint32_t bz_phy_sts_zb_peak_im_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}


#define PHY_STATUS_5_ADDR (BZ_PHY_BASE + 0x00000874)

static inline uint32_t phy_status_5_get(void)
{
    return REG_PL_RD(PHY_STATUS_5_ADDR);
}

static inline void phy_status_5_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_5_ADDR,x);
}

static inline uint32_t bz_phy_sts_zb_curr_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_5_ADDR);
    return ((localVal & ((uint32_t)0x3fff0000)) >> 16);
}

static inline uint32_t bz_phy_sts_zb_curr_loc_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_5_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define PHY_STATUS_6_ADDR (BZ_PHY_BASE + 0x00000878)

static inline uint32_t phy_status_6_get(void)
{
    return REG_PL_RD(PHY_STATUS_6_ADDR);
}

static inline void phy_status_6_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_6_ADDR,x);
}

static inline uint32_t bz_phy_sts_done_zb_freq_trk_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_6_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}


#define PHY_STATUS_7_ADDR (BZ_PHY_BASE + 0x0000087c)

static inline uint32_t phy_status_7_get(void)
{
    return REG_PL_RD(PHY_STATUS_7_ADDR);
}

static inline void phy_status_7_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_7_ADDR,x);
}

static inline uint32_t bz_phy_sts_ble_clear_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_7_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_sts_ble_clear_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_7_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_STATUS_7_ADDR,localVal);
}

static inline uint32_t bz_phy_sts_ble_frtk_out_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_7_ADDR);
    return ((localVal & ((uint32_t)0x001fff00)) >> 8);
}

static inline uint32_t bz_phy_sts_ble_cfo_out_peak_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_7_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define PHY_STATUS_8_ADDR (BZ_PHY_BASE + 0x00000880)

static inline uint32_t phy_status_8_get(void)
{
    return REG_PL_RD(PHY_STATUS_8_ADDR);
}

static inline void phy_status_8_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_8_ADDR,x);
}

static inline uint32_t bz_phy_sts_ble_aa_confirm_bits_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_8_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define PHY_STATUS_9_ADDR (BZ_PHY_BASE + 0x00000884)

static inline uint32_t phy_status_9_get(void)
{
    return REG_PL_RD(PHY_STATUS_9_ADDR);
}

static inline void phy_status_9_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_9_ADDR,x);
}

static inline uint32_t bz_phy_sts_ble_aa_error_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_9_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline uint32_t bz_phy_sts_ble_corr_out_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_9_ADDR);
    return ((localVal & ((uint32_t)0x000fff00)) >> 8);
}

static inline uint32_t bz_phy_sts_ble_done_cfo_out_peak_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_9_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define PHY_STATUS_10_ADDR (BZ_PHY_BASE + 0x00000888)

static inline uint32_t phy_status_10_get(void)
{
    return REG_PL_RD(PHY_STATUS_10_ADDR);
}

static inline void phy_status_10_set(uint32_t x)
{
    REG_PL_WR(PHY_STATUS_10_ADDR,x);
}

static inline uint32_t bz_phy_zb_dm_white_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_zb_dm_white_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(PHY_STATUS_10_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_white_init_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void bz_phy_zb_dm_white_init_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(PHY_STATUS_10_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_dm_phy_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline uint32_t bz_phy_zb_dm_sfd_found_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline uint32_t bz_phy_zb_ts_cs_found_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline uint32_t bz_phy_zb_fsm_dbg_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void bz_phy_zb_fsm_dbg_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(PHY_STATUS_10_ADDR,localVal);
}

static inline uint32_t bz_phy_zb_fsm_err_sts_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_STATUS_10_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define PHY_BLE_CODED_0_ADDR (BZ_PHY_BASE + 0x0000088c)

static inline uint32_t phy_ble_coded_0_get(void)
{
    return REG_PL_RD(PHY_BLE_CODED_0_ADDR);
}

static inline void phy_ble_coded_0_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,x);
}

static inline uint32_t bz_phy_blec_pk_start_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    return ((localVal & ((uint32_t)0xff800000)) >> 23);
}

static inline void bz_phy_blec_pk_start_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    localVal &= ~((uint32_t)0xff800000);
    localVal |= (x << 23)&((uint32_t)0xff800000);
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_pk_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    return ((localVal & ((uint32_t)0x007fc000)) >> 14);
}

static inline void bz_phy_blec_pk_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    localVal &= ~((uint32_t)0x007fc000);
    localVal |= (x << 14)&((uint32_t)0x007fc000);
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_pk_timing_diff_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    return ((localVal & ((uint32_t)0x00003800)) >> 11);
}

static inline void bz_phy_blec_pk_timing_diff_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    localVal &= ~((uint32_t)0x00003800);
    localVal |= (x << 11)&((uint32_t)0x00003800);
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_pk_jump_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    return ((localVal & ((uint32_t)0x000007e0)) >> 5);
}

static inline void bz_phy_blec_pk_jump_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    localVal &= ~((uint32_t)0x000007e0);
    localVal |= (x << 5)&((uint32_t)0x000007e0);
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_aa_search_start_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void bz_phy_blec_aa_search_start_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_0_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0)&((uint32_t)0x0000001f);
    REG_PL_WR(PHY_BLE_CODED_0_ADDR,localVal);
}


#define PHY_BLE_CODED_1_ADDR (BZ_PHY_BASE + 0x00000890)

static inline uint32_t phy_ble_coded_1_get(void)
{
    return REG_PL_RD(PHY_BLE_CODED_1_ADDR);
}

static inline void phy_ble_coded_1_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,x);
}

static inline uint32_t bz_phy_blec_aa_search_stop_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    return ((localVal & ((uint32_t)0xfe000000)) >> 25);
}

static inline void bz_phy_blec_aa_search_stop_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    localVal &= ~((uint32_t)0xfe000000);
    localVal |= (x << 25)&((uint32_t)0xfe000000);
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_aa_corr_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    return ((localVal & ((uint32_t)0x01fe0000)) >> 17);
}

static inline void bz_phy_blec_aa_corr_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    localVal &= ~((uint32_t)0x01fe0000);
    localVal |= (x << 17)&((uint32_t)0x01fe0000);
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_timing_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    return ((localVal & ((uint32_t)0x0001f000)) >> 12);
}

static inline void bz_phy_blec_timing_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    localVal &= ~((uint32_t)0x0001f000);
    localVal |= (x << 12)&((uint32_t)0x0001f000);
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,localVal);
}

static inline uint32_t bz_phy_blec_rst_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void bz_phy_blec_rst_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_fsm_rx_dmtime_cd_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void bz_phy_ble_fsm_rx_dmtime_cd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_CODED_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(PHY_BLE_CODED_1_ADDR,localVal);
}


#define PHY_BLE_FRTK_0_ADDR (BZ_PHY_BASE + 0x00000894)

static inline uint32_t phy_ble_frtk_0_get(void)
{
    return REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
}

static inline void phy_ble_frtk_0_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_FRTK_0_ADDR,x);
}

static inline uint32_t bz_phy_ble_frtk_th1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void bz_phy_ble_frtk_th1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(PHY_BLE_FRTK_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_frtk_th2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void bz_phy_ble_frtk_th2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(PHY_BLE_FRTK_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_frtk_th3_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void bz_phy_ble_frtk_th3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(PHY_BLE_FRTK_0_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_frtk_max_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_ble_frtk_max_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PHY_BLE_FRTK_0_ADDR,localVal);
}


#define PHY_BLE_FRTK_1_ADDR (BZ_PHY_BASE + 0x00000898)

static inline uint32_t phy_ble_frtk_1_get(void)
{
    return REG_PL_RD(PHY_BLE_FRTK_1_ADDR);
}

static inline void phy_ble_frtk_1_set(uint32_t x)
{
    REG_PL_WR(PHY_BLE_FRTK_1_ADDR,x);
}

static inline uint32_t bz_phy_ble_frtk_coeff_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_1_ADDR);
    return ((localVal & ((uint32_t)0x000003fe)) >> 1);
}

static inline void bz_phy_ble_frtk_coeff_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_1_ADDR);
    localVal &= ~((uint32_t)0x000003fe);
    localVal |= (x << 1)&((uint32_t)0x000003fe);
    REG_PL_WR(PHY_BLE_FRTK_1_ADDR,localVal);
}

static inline uint32_t bz_phy_ble_frtk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void bz_phy_ble_frtk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_BLE_FRTK_1_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(PHY_BLE_FRTK_1_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR0_ADDR (BZ_PHY_BASE + 0x00000900)

static inline uint32_t notch_config_par0_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
}

static inline void notch_config_par0_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR0_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR0_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR0_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR0_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR0_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR1_ADDR (BZ_PHY_BASE + 0x00000904)

static inline uint32_t notch_config_par1_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
}

static inline void notch_config_par1_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR1_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR1_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR1_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR1_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR1_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR1_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR2_ADDR (BZ_PHY_BASE + 0x00000908)

static inline uint32_t notch_config_par2_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
}

static inline void notch_config_par2_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR2_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR2_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR2_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR2_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR2_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR2_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR3_ADDR (BZ_PHY_BASE + 0x0000090c)

static inline uint32_t notch_config_par3_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
}

static inline void notch_config_par3_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR3_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR3_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR3_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR3_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR3_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR3_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR4_ADDR (BZ_PHY_BASE + 0x00000910)

static inline uint32_t notch_config_par4_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
}

static inline void notch_config_par4_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR4_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR4_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR4_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR4_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR4_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR4_ADDR,localVal);
}


#define NOTCH_CONFIG_PAR5_ADDR (BZ_PHY_BASE + 0x00000914)

static inline uint32_t notch_config_par5_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
}

static inline void notch_config_par5_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_PAR5_ADDR,x);
}

static inline uint32_t bz_phy_notch_en_par5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void bz_phy_notch_en_par5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31)&((uint32_t)0x80000000);
    REG_PL_WR(NOTCH_CONFIG_PAR5_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_alpha_par5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void bz_phy_notch_alpha_par5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(NOTCH_CONFIG_PAR5_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_re_par5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    return ((localVal & ((uint32_t)0x03ff0000)) >> 16);
}

static inline void bz_phy_notch_coeff_re_par5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    localVal &= ~((uint32_t)0x03ff0000);
    localVal |= (x << 16)&((uint32_t)0x03ff0000);
    REG_PL_WR(NOTCH_CONFIG_PAR5_ADDR,localVal);
}

static inline uint32_t bz_phy_notch_coeff_im_par5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_notch_coeff_im_par5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_PAR5_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(NOTCH_CONFIG_PAR5_ADDR,localVal);
}


#define NOTCH_CONFIG_CH0_ADDR (BZ_PHY_BASE + 0x00000918)

static inline uint32_t notch_config_ch0_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
}

static inline void notch_config_ch0_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_CH0_ADDR,x);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void bz_phy_rx_notch_tar_ch_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(NOTCH_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void bz_phy_rx_notch_tar_ch_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(NOTCH_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void bz_phy_rx_notch_tar_ch_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(NOTCH_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void bz_phy_rx_notch_tar_ch_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(NOTCH_CONFIG_CH0_ADDR,localVal);
}


#define NOTCH_CONFIG_CH1_ADDR (BZ_PHY_BASE + 0x0000091c)

static inline uint32_t notch_config_ch1_get(void)
{
    return REG_PL_RD(NOTCH_CONFIG_CH1_ADDR);
}

static inline void notch_config_ch1_set(uint32_t x)
{
    REG_PL_WR(NOTCH_CONFIG_CH1_ADDR,x);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH1_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void bz_phy_rx_notch_tar_ch_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH1_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(NOTCH_CONFIG_CH1_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_notch_tar_ch_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH1_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void bz_phy_rx_notch_tar_ch_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(NOTCH_CONFIG_CH1_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(NOTCH_CONFIG_CH1_ADDR,localVal);
}


#define ADCCLK_CONFIG_PAR_ADDR (BZ_PHY_BASE + 0x00000920)

static inline uint32_t adcclk_config_par_get(void)
{
    return REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
}

static inline void adcclk_config_par_set(uint32_t x)
{
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,x);
}

static inline uint32_t bz_phy_adc_clk_sel_par5_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void bz_phy_adc_clk_sel_par5_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_par4_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void bz_phy_adc_clk_sel_par4_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_par3_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void bz_phy_adc_clk_sel_par3_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_par2_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void bz_phy_adc_clk_sel_par2_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_par1_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void bz_phy_adc_clk_sel_par1_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}

static inline uint32_t bz_phy_adc_clk_sel_par0_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void bz_phy_adc_clk_sel_par0_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_PAR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(ADCCLK_CONFIG_PAR_ADDR,localVal);
}


#define ADCCLK_CONFIG_CH0_ADDR (BZ_PHY_BASE + 0x00000924)

static inline uint32_t adcclk_config_ch0_get(void)
{
    return REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
}

static inline void adcclk_config_ch0_set(uint32_t x)
{
    REG_PL_WR(ADCCLK_CONFIG_CH0_ADDR,x);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x7f000000)) >> 24);
}

static inline void bz_phy_rx_adcclk_tar_ch_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x7f000000);
    localVal |= (x << 24)&((uint32_t)0x7f000000);
    REG_PL_WR(ADCCLK_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x007f0000)) >> 16);
}

static inline void bz_phy_rx_adcclk_tar_ch_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x007f0000);
    localVal |= (x << 16)&((uint32_t)0x007f0000);
    REG_PL_WR(ADCCLK_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void bz_phy_rx_adcclk_tar_ch_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(ADCCLK_CONFIG_CH0_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void bz_phy_rx_adcclk_tar_ch_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH0_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(ADCCLK_CONFIG_CH0_ADDR,localVal);
}


#define ADCCLK_CONFIG_CH1_ADDR (BZ_PHY_BASE + 0x00000928)

static inline uint32_t adcclk_config_ch1_get(void)
{
    return REG_PL_RD(ADCCLK_CONFIG_CH1_ADDR);
}

static inline void adcclk_config_ch1_set(uint32_t x)
{
    REG_PL_WR(ADCCLK_CONFIG_CH1_ADDR,x);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH1_ADDR);
    return ((localVal & ((uint32_t)0x00007f00)) >> 8);
}

static inline void bz_phy_rx_adcclk_tar_ch_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH1_ADDR);
    localVal &= ~((uint32_t)0x00007f00);
    localVal |= (x << 8)&((uint32_t)0x00007f00);
    REG_PL_WR(ADCCLK_CONFIG_CH1_ADDR,localVal);
}

static inline uint32_t bz_phy_rx_adcclk_tar_ch_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH1_ADDR);
    return ((localVal & ((uint32_t)0x0000007f)) >> 0);
}

static inline void bz_phy_rx_adcclk_tar_ch_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCCLK_CONFIG_CH1_ADDR);
    localVal &= ~((uint32_t)0x0000007f);
    localVal |= (x << 0)&((uint32_t)0x0000007f);
    REG_PL_WR(ADCCLK_CONFIG_CH1_ADDR,localVal);
}


#define PHY_CCA_1_ADDR (BZ_PHY_BASE + 0x0000092c)

static inline uint32_t phy_cca_1_get(void)
{
    return REG_PL_RD(PHY_CCA_1_ADDR);
}

static inline void phy_cca_1_set(uint32_t x)
{
    REG_PL_WR(PHY_CCA_1_ADDR,x);
}

static inline uint32_t bz_phy_cca_cs_corr_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void bz_phy_cca_cs_corr_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_1_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8)&((uint32_t)0x00000f00);
    REG_PL_WR(PHY_CCA_1_ADDR,localVal);
}

static inline uint32_t bz_phy_cca_corr_thres_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_cca_corr_thres_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_CCA_1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PHY_CCA_1_ADDR,localVal);
}


#define PA_REF_DAC0_ADDR (BZ_PHY_BASE + 0x00000930)

static inline uint32_t pa_ref_dac0_get(void)
{
    return REG_PL_RD(PA_REF_DAC0_ADDR);
}

static inline void pa_ref_dac0_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC0_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl3_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl3_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC0_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl2_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl2_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC0_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl1_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl1_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC0_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl0_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl0_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC0_ADDR,localVal);
}


#define PA_REF_DAC1_ADDR (BZ_PHY_BASE + 0x00000934)

static inline uint32_t pa_ref_dac1_get(void)
{
    return REG_PL_RD(PA_REF_DAC1_ADDR);
}

static inline void pa_ref_dac1_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC1_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl7_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl7_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC1_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl6_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl6_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC1_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl5_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl5_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC1_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl4_pa_ref_dac_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl4_pa_ref_dac_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC1_ADDR,localVal);
}


#define PA_REF_DAC2_ADDR (BZ_PHY_BASE + 0x00000938)

static inline uint32_t pa_ref_dac2_get(void)
{
    return REG_PL_RD(PA_REF_DAC2_ADDR);
}

static inline void pa_ref_dac2_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC2_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl_pa_ref_dac_zb_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl_pa_ref_dac_zb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC2_ADDR,localVal);
}


#define PA_REF_DAC3_ADDR (BZ_PHY_BASE + 0x0000093c)

static inline uint32_t pa_ref_dac3_get(void)
{
    return REG_PL_RD(PA_REF_DAC3_ADDR);
}

static inline void pa_ref_dac3_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC3_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl3_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl3_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC3_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl2_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl2_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC3_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl1_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl1_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC3_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl0_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl0_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC3_ADDR,localVal);
}


#define PA_REF_DAC4_ADDR (BZ_PHY_BASE + 0x00000940)

static inline uint32_t pa_ref_dac4_get(void)
{
    return REG_PL_RD(PA_REF_DAC4_ADDR);
}

static inline void pa_ref_dac4_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC4_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl7_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl7_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC4_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl6_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl6_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC4_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl5_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl5_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC4_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl4_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl4_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC4_ADDR,localVal);
}


#define PA_REF_DAC5_ADDR (BZ_PHY_BASE + 0x00000944)

static inline uint32_t pa_ref_dac5_get(void)
{
    return REG_PL_RD(PA_REF_DAC5_ADDR);
}

static inline void pa_ref_dac5_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC5_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl11_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl11_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC5_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl10_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl10_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC5_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl9_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl9_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC5_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl8_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl8_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC5_ADDR,localVal);
}


#define PA_REF_DAC6_ADDR (BZ_PHY_BASE + 0x00000948)

static inline uint32_t pa_ref_dac6_get(void)
{
    return REG_PL_RD(PA_REF_DAC6_ADDR);
}

static inline void pa_ref_dac6_set(uint32_t x)
{
    REG_PL_WR(PA_REF_DAC6_ADDR,x);
}

static inline uint32_t bz_phy_gain_ctrl15_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void bz_phy_gain_ctrl15_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24)&((uint32_t)0x3f000000);
    REG_PL_WR(PA_REF_DAC6_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl14_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void bz_phy_gain_ctrl14_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(PA_REF_DAC6_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl13_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void bz_phy_gain_ctrl13_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8)&((uint32_t)0x00003f00);
    REG_PL_WR(PA_REF_DAC6_ADDR,localVal);
}

static inline uint32_t bz_phy_gain_ctrl12_pa_ref_dac_adj_getf(void)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void bz_phy_gain_ctrl12_pa_ref_dac_adj_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PA_REF_DAC6_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0)&((uint32_t)0x0000003f);
    REG_PL_WR(PA_REF_DAC6_ADDR,localVal);
}


#define PHY_FPGA_CTRL_0_ADDR (BZ_PHY_BASE + 0x00000a00)

static inline uint32_t phy_fpga_ctrl_0_get(void)
{
    return REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
}

static inline void phy_fpga_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,x);
}

static inline uint32_t bz_phy_fpga_dig_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void bz_phy_fpga_dig_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26)&((uint32_t)0xfc000000);
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t bz_phy_pf_en_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline void bz_phy_pf_en_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00200000);
    localVal |= (x << 21)&((uint32_t)0x00200000);
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t bz_phy_pf_en_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void bz_phy_pf_en_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20)&((uint32_t)0x00100000);
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t bz_phy_pf_th1_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void bz_phy_pf_th1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10)&((uint32_t)0x000ffc00);
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,localVal);
}

static inline uint32_t bz_phy_pf_th2_getf(void)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void bz_phy_pf_th2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(PHY_FPGA_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0)&((uint32_t)0x000003ff);
    REG_PL_WR(PHY_FPGA_CTRL_0_ADDR,localVal);
}

#endif
