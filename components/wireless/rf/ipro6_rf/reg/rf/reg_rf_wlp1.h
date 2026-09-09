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

#ifndef _REG_RF_WLP1_H_
#define _REG_RF_WLP1_H_
// Date: 20211018

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef RF_WLP1_BASE
#define RF_WLP1_BASE (0x20001000)
#endif



#define WLP1_CFG0_ADDR (RF_WLP1_BASE + 0x00000400)

static inline uint32_t wlp1_cfg0_get(void)
{
    return REG_PL_RD(WLP1_CFG0_ADDR);
}

static inline void wlp1_cfg0_set(uint32_t x)
{
    REG_PL_WR(WLP1_CFG0_ADDR,x);
}

static inline uint32_t rf_wlp1_wifi_mac_wt_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rf_wlp1_wifi_mac_wt_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28)&((uint32_t)0xf0000000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_wifi_mac_core_div_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rf_wlp1_wifi_mac_core_div_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_ble_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rf_wlp1_ble_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23)&((uint32_t)0x00800000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_480m_soc_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline void rf_wlp1_cgen_480m_soc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00400000);
    localVal |= (x << 22)&((uint32_t)0x00400000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_ble_clk_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rf_wlp1_ble_clk_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16)&((uint32_t)0x003f0000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_m154_zben_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rf_wlp1_m154_zben_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15)&((uint32_t)0x00008000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_clk_phy_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rf_wlp1_clk_phy_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14)&((uint32_t)0x00004000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_m_wifi_platform_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rf_wlp1_cgen_m_wifi_platform_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13)&((uint32_t)0x00002000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_m_wifi_mac_phy_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rf_wlp1_cgen_m_wifi_mac_phy_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12)&((uint32_t)0x00001000);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_m_wifi_phy_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rf_wlp1_cgen_m_wifi_phy_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11)&((uint32_t)0x00000800);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_s1_rf_top_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rf_wlp1_cgen_s1_rf_top_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10)&((uint32_t)0x00000400);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_s3_bt_ble_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rf_wlp1_cgen_s3_bt_ble_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9)&((uint32_t)0x00000200);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_cgen_s3_m154_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rf_wlp1_cgen_s3_m154_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_force_ram_clk_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000080)) >> 7);
}

static inline void rf_wlp1_force_ram_clk_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000080);
    localVal |= (x << 7)&((uint32_t)0x00000080);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_m154_rx_ignore_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void rf_wlp1_m154_rx_ignore_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_bmx_gating_dis_scan_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rf_wlp1_bmx_gating_dis_scan_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_arb_mode_wl1_gp_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rf_wlp1_arb_mode_wl1_gp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_wifi_csi_hqos_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_wlp1_wifi_csi_hqos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_pldma_hqos_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_wlp1_pldma_hqos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_m154m_hqos_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_wlp1_m154m_hqos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_wl1_slv_qos_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_wlp1_wl1_slv_qos_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(WLP1_CFG0_ADDR,localVal);
}


#define WIFI_FTM_CTRL_0_ADDR (RF_WLP1_BASE + 0x00000404)

static inline uint32_t wifi_ftm_ctrl_0_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_0_ADDR);
}

static inline void wifi_ftm_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_0_ADDR,x);
}

static inline uint32_t rf_wlp1_ftmtoaclockfreqmhz_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_wlp1_ftmtoaclockfreqmhz_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24)&((uint32_t)0xff000000);
    REG_PL_WR(WIFI_FTM_CTRL_0_ADDR,localVal);
}

static inline uint32_t rf_wlp1_todtimestamplo_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}


#define WIFI_FTM_CTRL_1_ADDR (RF_WLP1_BASE + 0x00000408)

static inline uint32_t wifi_ftm_ctrl_1_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_1_ADDR);
}

static inline void wifi_ftm_ctrl_1_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_1_ADDR,x);
}

static inline uint32_t rf_wlp1_todtimestamphi_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define WIFI_FTM_CTRL_2_ADDR (RF_WLP1_BASE + 0x0000040c)

static inline uint32_t wifi_ftm_ctrl_2_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_2_ADDR);
}

static inline void wifi_ftm_ctrl_2_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_2_ADDR,x);
}

static inline uint32_t rf_wlp1_ftmtoatimestamplo_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_2_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}


#define WIFI_FTM_CTRL_3_ADDR (RF_WLP1_BASE + 0x00000410)

static inline uint32_t wifi_ftm_ctrl_3_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_3_ADDR);
}

static inline void wifi_ftm_ctrl_3_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_3_ADDR,x);
}

static inline uint32_t rf_wlp1_ftmtoatimestamphi_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define WIFI_FTM_CTRL_4_ADDR (RF_WLP1_BASE + 0x00000414)

static inline uint32_t wifi_ftm_ctrl_4_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_4_ADDR);
}

static inline void wifi_ftm_ctrl_4_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_4_ADDR,x);
}

static inline uint32_t rf_wlp1_toatimestamplo_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_4_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}


#define WIFI_FTM_CTRL_5_ADDR (RF_WLP1_BASE + 0x00000418)

static inline uint32_t wifi_ftm_ctrl_5_get(void)
{
    return REG_PL_RD(WIFI_FTM_CTRL_5_ADDR);
}

static inline void wifi_ftm_ctrl_5_set(uint32_t x)
{
    REG_PL_WR(WIFI_FTM_CTRL_5_ADDR,x);
}

static inline uint32_t rf_wlp1_toatimestamphi_getf(void)
{
    uint32_t localVal = REG_PL_RD(WIFI_FTM_CTRL_5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define WLP1_CFG1_ADDR (RF_WLP1_BASE + 0x0000041c)

static inline uint32_t wlp1_cfg1_get(void)
{
    return REG_PL_RD(WLP1_CFG1_ADDR);
}

static inline void wlp1_cfg1_set(uint32_t x)
{
    REG_PL_WR(WLP1_CFG1_ADDR,x);
}

static inline uint32_t rf_wlp1_wlp1_rsvd_getf(void)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_wlp1_wlp1_rsvd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WLP1_CFG1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0)&((uint32_t)0x000000ff);
    REG_PL_WR(WLP1_CFG1_ADDR,localVal);
}

#endif