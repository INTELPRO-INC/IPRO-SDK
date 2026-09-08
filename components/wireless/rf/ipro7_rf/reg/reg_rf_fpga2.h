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

#ifndef _REG_RF_FPGA2_H_
#define _REG_RF_FPGA2_H_
// This file is generated based on the provided Excel sheet

#include <stdint.h>
#include "_reg_access.h"
#include "ipro7.h"

#ifndef RF_FPGA2_BASE
#define RF_FPGA2_BASE (0x30001000)
#endif


#define RF_FPGA2_CTRL_0_ADDR (RF_FPGA2_BASE + 0x00000880)

static inline uint32_t rf_fpga2_ctrl_0_get(void)
{
    return REG_PL_RD(RF_FPGA2_CTRL_0_ADDR);
}

static inline void rf_fpga2_ctrl_0_set(uint32_t x)
{
    REG_PL_WR(RF_FPGA2_CTRL_0_ADDR, x);
}


static inline uint32_t rf_fpga2_rf_fpga2_cfg_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA2_CTRL_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rf_fpga2_rf_fpga2_cfg_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_FPGA2_CTRL_0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RF_FPGA2_CTRL_0_ADDR, localVal);
}

#define GPM_COMMON0_ADDR (RF_FPGA2_BASE + 0x00000884)

static inline uint32_t gpm_common0_get(void)
{
    return REG_PL_RD(GPM_COMMON0_ADDR);
}

static inline void gpm_common0_set(uint32_t x)
{
    REG_PL_WR(GPM_COMMON0_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_inv_rssi_0p6_man_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    return ((localVal & ((uint32_t)0xffc00000)) >> 22);
}

static inline void rf_fpga2_cfg_gpm_inv_rssi_0p6_man_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    localVal &= ~((uint32_t)0xffc00000);
    localVal |= (x << 22) & ((uint32_t)0xffc00000);
    REG_PL_WR(GPM_COMMON0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_inv_rssi_0p6_exp_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    return ((localVal & ((uint32_t)0x003e0000)) >> 17);
}

static inline void rf_fpga2_cfg_gpm_inv_rssi_0p6_exp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    localVal &= ~((uint32_t)0x003e0000);
    localVal |= (x << 17) & ((uint32_t)0x003e0000);
    REG_PL_WR(GPM_COMMON0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_clip_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    return ((localVal & ((uint32_t)0x00007fe0)) >> 5);
}

static inline void rf_fpga2_cfg_gpm_clip_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    localVal &= ~((uint32_t)0x00007fe0);
    localVal |= (x << 5) & ((uint32_t)0x00007fe0);
    REG_PL_WR(GPM_COMMON0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_clip_exp_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_clip_exp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON0_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(GPM_COMMON0_ADDR, localVal);
}

#define GPM_COMMON1_ADDR (RF_FPGA2_BASE + 0x00000888)

static inline uint32_t gpm_common1_get(void)
{
    return REG_PL_RD(GPM_COMMON1_ADDR);
}

static inline void gpm_common1_set(uint32_t x)
{
    REG_PL_WR(GPM_COMMON1_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_inv_0p6_man_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON1_ADDR);
    return ((localVal & ((uint32_t)0x00007fe0)) >> 5);
}

static inline void rf_fpga2_cfg_gpm_inv_0p6_man_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON1_ADDR);
    localVal &= ~((uint32_t)0x00007fe0);
    localVal |= (x << 5) & ((uint32_t)0x00007fe0);
    REG_PL_WR(GPM_COMMON1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_inv_0p6_exp_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON1_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_inv_0p6_exp_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_COMMON1_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(GPM_COMMON1_ADDR, localVal);
}

#define GPM_PATH0_1_ADDR (RF_FPGA2_BASE + 0x0000088c)

static inline uint32_t gpm_path0_1_get(void)
{
    return REG_PL_RD(GPM_PATH0_1_ADDR);
}

static inline void gpm_path0_1_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH0_1_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_en_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_fpga2_cfg_gpm_en_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(GPM_PATH0_1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_noise_i_init_addr_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    return ((localVal & ((uint32_t)0x03ffe000)) >> 13);
}

static inline void rf_fpga2_cfg_gpm_noise_i_init_addr_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    localVal &= ~((uint32_t)0x03ffe000);
    localVal |= (x << 13) & ((uint32_t)0x03ffe000);
    REG_PL_WR(GPM_PATH0_1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_noise_q_init_addr_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_noise_q_init_addr_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_1_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(GPM_PATH0_1_ADDR, localVal);
}

#define GPM_PATH0_2_ADDR (RF_FPGA2_BASE + 0x00000890)

static inline uint32_t gpm_path0_2_get(void)
{
    return REG_PL_RD(GPM_PATH0_2_ADDR);
}

static inline void gpm_path0_2_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH0_2_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_pant1_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fpga2_cfg_gpm_pant1_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20) & ((uint32_t)0x3ff00000);
    REG_PL_WR(GPM_PATH0_2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_tia_att_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void rf_fpga2_cfg_gpm_tia_att_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10) & ((uint32_t)0x000ffc00);
    REG_PL_WR(GPM_PATH0_2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_tia_att_noise_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_tia_att_noise_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_2_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0) & ((uint32_t)0x000003ff);
    REG_PL_WR(GPM_PATH0_2_ADDR, localVal);
}

#define GPM_PATH0_3_ADDR (RF_FPGA2_BASE + 0x00000894)

static inline uint32_t gpm_path0_3_get(void)
{
    return REG_PL_RD(GPM_PATH0_3_ADDR);
}

static inline void gpm_path0_3_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH0_3_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_biquad_att_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fpga2_cfg_gpm_biquad_att_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20) & ((uint32_t)0x3ff00000);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_bypass_noise_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_fpga2_cfg_gpm_bypass_noise_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18) & ((uint32_t)0x00040000);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gain_en_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_fpga2_cfg_gpm_force_gain_en_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17) & ((uint32_t)0x00020000);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_lna_gain_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x0001e000)) >> 13);
}

static inline void rf_fpga2_cfg_gpm_force_lna_gain_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x0001e000);
    localVal |= (x << 13) & ((uint32_t)0x0001e000);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gc_rbb1_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

static inline void rf_fpga2_cfg_gpm_force_gc_rbb1_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x00001800);
    localVal |= (x << 11) & ((uint32_t)0x00001800);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gc_rbb2_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_fpga2_cfg_gpm_force_gc_rbb2_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8) & ((uint32_t)0x00000700);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_cfo_path0_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_cfo_path0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH0_3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(GPM_PATH0_3_ADDR, localVal);
}

#define GPM_PATH1_1_ADDR (RF_FPGA2_BASE + 0x00000898)

static inline uint32_t gpm_path1_1_get(void)
{
    return REG_PL_RD(GPM_PATH1_1_ADDR);
}

static inline void gpm_path1_1_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH1_1_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_en_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rf_fpga2_cfg_gpm_en_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(GPM_PATH1_1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_noise_i_init_addr_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    return ((localVal & ((uint32_t)0x03ffe000)) >> 13);
}

static inline void rf_fpga2_cfg_gpm_noise_i_init_addr_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    localVal &= ~((uint32_t)0x03ffe000);
    localVal |= (x << 13) & ((uint32_t)0x03ffe000);
    REG_PL_WR(GPM_PATH1_1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_noise_q_init_addr_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_noise_q_init_addr_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_1_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(GPM_PATH1_1_ADDR, localVal);
}

#define GPM_PATH1_2_ADDR (RF_FPGA2_BASE + 0x0000089c)

static inline uint32_t gpm_path1_2_get(void)
{
    return REG_PL_RD(GPM_PATH1_2_ADDR);
}

static inline void gpm_path1_2_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH1_2_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_pant1_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fpga2_cfg_gpm_pant1_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20) & ((uint32_t)0x3ff00000);
    REG_PL_WR(GPM_PATH1_2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_tia_att_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    return ((localVal & ((uint32_t)0x000ffc00)) >> 10);
}

static inline void rf_fpga2_cfg_gpm_tia_att_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    localVal &= ~((uint32_t)0x000ffc00);
    localVal |= (x << 10) & ((uint32_t)0x000ffc00);
    REG_PL_WR(GPM_PATH1_2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_tia_att_noise_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_tia_att_noise_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_2_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0) & ((uint32_t)0x000003ff);
    REG_PL_WR(GPM_PATH1_2_ADDR, localVal);
}

#define GPM_PATH1_3_ADDR (RF_FPGA2_BASE + 0x000008a0)

static inline uint32_t gpm_path1_3_get(void)
{
    return REG_PL_RD(GPM_PATH1_3_ADDR);
}

static inline void gpm_path1_3_set(uint32_t x)
{
    REG_PL_WR(GPM_PATH1_3_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_biquad_att_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x3ff00000)) >> 20);
}

static inline void rf_fpga2_cfg_gpm_biquad_att_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x3ff00000);
    localVal |= (x << 20) & ((uint32_t)0x3ff00000);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_bypass_noise_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_fpga2_cfg_gpm_bypass_noise_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18) & ((uint32_t)0x00040000);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gain_en_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rf_fpga2_cfg_gpm_force_gain_en_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17) & ((uint32_t)0x00020000);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_lna_gain_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x0001e000)) >> 13);
}

static inline void rf_fpga2_cfg_gpm_force_lna_gain_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x0001e000);
    localVal |= (x << 13) & ((uint32_t)0x0001e000);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gc_rbb1_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x00001800)) >> 11);
}

static inline void rf_fpga2_cfg_gpm_force_gc_rbb1_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x00001800);
    localVal |= (x << 11) & ((uint32_t)0x00001800);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_force_gc_rbb2_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rf_fpga2_cfg_gpm_force_gc_rbb2_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8) & ((uint32_t)0x00000700);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_gpm_cfo_path1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_cfo_path1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PATH1_3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(GPM_PATH1_3_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR (RF_FPGA2_BASE + 0x000008a4)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl0_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl0_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end_top_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end_top_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12) & ((uint32_t)0x0000f000);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cfg_hold_dac_rd_data_th_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR);
    return ((localVal & ((uint32_t)0x00000fff)) >> 0);
}

static inline void rf_fpga2_cfg_hold_dac_rd_data_th_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR);
    localVal &= ~((uint32_t)0x00000fff);
    localVal |= (x << 0) & ((uint32_t)0x00000fff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL0_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR (RF_FPGA2_BASE + 0x000008a8)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl1_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl1_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end1_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL1_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR (RF_FPGA2_BASE + 0x000008ac)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl2_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl2_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end2_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL2_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR (RF_FPGA2_BASE + 0x000008b0)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl3_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl3_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end3_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL3_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR (RF_FPGA2_BASE + 0x000008b4)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl4_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl4_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end4_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL4_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR (RF_FPGA2_BASE + 0x000008b8)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl5_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl5_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end5_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL5_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR (RF_FPGA2_BASE + 0x000008bc)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl6_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl6_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end6_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL6_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR (RF_FPGA2_BASE + 0x000008c0)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl7_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl7_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end7_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL7_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR (RF_FPGA2_BASE + 0x000008c4)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl8_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl8_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end8_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL8_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR (RF_FPGA2_BASE + 0x000008c8)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl9_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl9_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end9_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL9_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR (RF_FPGA2_BASE + 0x000008cc)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl10_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl10_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end10_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL10_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR (RF_FPGA2_BASE + 0x000008d0)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl11_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl11_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end11_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL11_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR (RF_FPGA2_BASE + 0x000008d4)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl12_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl12_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end12_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL12_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR (RF_FPGA2_BASE + 0x000008d8)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl13_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl13_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end13_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL13_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR (RF_FPGA2_BASE + 0x000008dc)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl14_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl14_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end14_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL14_ADDR, localVal);
}

#define GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR (RF_FPGA2_BASE + 0x000008e0)

static inline uint32_t gpm_rf_sram_dac_addr_ctrl15_get(void)
{
    return REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR);
}

static inline void gpm_rf_sram_dac_addr_ctrl15_set(uint32_t x)
{
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_rf_sram_dac_addr_end15_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR);
    return ((localVal & ((uint32_t)0x0003ffff)) >> 0);
}

static inline void rf_fpga2_cfg_rf_sram_dac_addr_end15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR);
    localVal &= ~((uint32_t)0x0003ffff);
    localVal |= (x << 0) & ((uint32_t)0x0003ffff);
    REG_PL_WR(GPM_RF_SRAM_DAC_ADDR_CTRL15_ADDR, localVal);
}

#define GPM_PKDET_ADDR (RF_FPGA2_BASE + 0x000008e4)

static inline uint32_t gpm_pkdet_get(void)
{
    return REG_PL_RD(GPM_PKDET_ADDR);
}

static inline void gpm_pkdet_set(uint32_t x)
{
    REG_PL_WR(GPM_PKDET_ADDR, x);
}


static inline uint32_t rf_fpga2_cfg_gpm_pkdetflagth_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_PKDET_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rf_fpga2_cfg_gpm_pkdetflagth_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_PKDET_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0) & ((uint32_t)0x0000ffff);
    REG_PL_WR(GPM_PKDET_ADDR, localVal);
}

#define WRF_SPI_CFG_ADDR (RF_FPGA2_BASE + 0x000008f0)

static inline uint32_t wrf_spi_cfg_get(void)
{
    return REG_PL_RD(WRF_SPI_CFG_ADDR);
}

static inline void wrf_spi_cfg_set(uint32_t x)
{
    REG_PL_WR(WRF_SPI_CFG_ADDR, x);
}


static inline uint32_t rf_fpga2_wrf_spi_rdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rf_fpga2_wrf_spi_rdata_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(WRF_SPI_CFG_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_spi_valid_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline void rf_fpga2_wrf_spi_valid_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    localVal &= ~((uint32_t)0x00100000);
    localVal |= (x << 20) & ((uint32_t)0x00100000);
    REG_PL_WR(WRF_SPI_CFG_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_spi_write_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline void rf_fpga2_wrf_spi_write_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    localVal &= ~((uint32_t)0x00040000);
    localVal |= (x << 18) & ((uint32_t)0x00040000);
    REG_PL_WR(WRF_SPI_CFG_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_spi_addr_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0003ff00)) >> 8);
}

static inline void rf_fpga2_wrf_spi_addr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    localVal &= ~((uint32_t)0x0003ff00);
    localVal |= (x << 8) & ((uint32_t)0x0003ff00);
    REG_PL_WR(WRF_SPI_CFG_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_spi_wdata_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rf_fpga2_wrf_spi_wdata_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_SPI_CFG_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(WRF_SPI_CFG_ADDR, localVal);
}

#define WRF_TX_CLK_DELAY_ADDR (RF_FPGA2_BASE + 0x000008f4)

static inline uint32_t wrf_tx_clk_delay_get(void)
{
    return REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
}

static inline void wrf_tx_clk_delay_set(uint32_t x)
{
    REG_PL_WR(WRF_TX_CLK_DELAY_ADDR, x);
}


static inline uint32_t rf_fpga2_wrf_tx_clk_wrflag_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_fpga2_wrf_tx_clk_wrflag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(WRF_TX_CLK_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_tx_clk_wrcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rf_fpga2_wrf_tx_clk_wrcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16) & ((uint32_t)0x01ff0000);
    REG_PL_WR(WRF_TX_CLK_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_tx_clk_rdcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_fpga2_wrf_tx_clk_rdcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_CLK_DELAY_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(WRF_TX_CLK_DELAY_ADDR, localVal);
}

#define WRF_TX_DAT_DELAY_ADDR (RF_FPGA2_BASE + 0x000008f8)

static inline uint32_t wrf_tx_dat_delay_get(void)
{
    return REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
}

static inline void wrf_tx_dat_delay_set(uint32_t x)
{
    REG_PL_WR(WRF_TX_DAT_DELAY_ADDR, x);
}


static inline uint32_t rf_fpga2_wrf_tx_dat_wrflag_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_fpga2_wrf_tx_dat_wrflag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(WRF_TX_DAT_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_tx_dat_wrcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rf_fpga2_wrf_tx_dat_wrcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16) & ((uint32_t)0x01ff0000);
    REG_PL_WR(WRF_TX_DAT_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_tx_dat_rdcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_fpga2_wrf_tx_dat_rdcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_TX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(WRF_TX_DAT_DELAY_ADDR, localVal);
}

#define WRF_RX_DAT_DELAY_ADDR (RF_FPGA2_BASE + 0x000008fc)

static inline uint32_t wrf_rx_dat_delay_get(void)
{
    return REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
}

static inline void wrf_rx_dat_delay_set(uint32_t x)
{
    REG_PL_WR(WRF_RX_DAT_DELAY_ADDR, x);
}


static inline uint32_t rf_fpga2_wrf_rx_dat_wrflag_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rf_fpga2_wrf_rx_dat_wrflag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(WRF_RX_DAT_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_rx_dat_wrcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline void rf_fpga2_wrf_rx_dat_wrcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x01ff0000);
    localVal |= (x << 16) & ((uint32_t)0x01ff0000);
    REG_PL_WR(WRF_RX_DAT_DELAY_ADDR, localVal);
}


static inline uint32_t rf_fpga2_wrf_rx_dat_rdcnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rf_fpga2_wrf_rx_dat_rdcnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(WRF_RX_DAT_DELAY_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(WRF_RX_DAT_DELAY_ADDR, localVal);
}

#define GPM_LNA_GAN0_ADDR (RF_FPGA2_BASE + 0x00000900)

static inline uint32_t gpm_lna_gan0_get(void)
{
    return REG_PL_RD(GPM_LNA_GAN0_ADDR);
}

static inline void gpm_lna_gan0_set(uint32_t x)
{
    REG_PL_WR(GPM_LNA_GAN0_ADDR, x);
}


static inline uint32_t rf_fpga2_cr_gpm_lna4_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_fpga2_cr_gpm_lna4_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(GPM_LNA_GAN0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna3_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_fpga2_cr_gpm_lna3_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18) & ((uint32_t)0x00fc0000);
    REG_PL_WR(GPM_LNA_GAN0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna2_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_fpga2_cr_gpm_lna2_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12) & ((uint32_t)0x0003f000);
    REG_PL_WR(GPM_LNA_GAN0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna1_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_fpga2_cr_gpm_lna1_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6) & ((uint32_t)0x00000fc0);
    REG_PL_WR(GPM_LNA_GAN0_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna0_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_fpga2_cr_gpm_lna0_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(GPM_LNA_GAN0_ADDR, localVal);
}

#define GPM_LNA_GAN1_ADDR (RF_FPGA2_BASE + 0x00000904)

static inline uint32_t gpm_lna_gan1_get(void)
{
    return REG_PL_RD(GPM_LNA_GAN1_ADDR);
}

static inline void gpm_lna_gan1_set(uint32_t x)
{
    REG_PL_WR(GPM_LNA_GAN1_ADDR, x);
}


static inline uint32_t rf_fpga2_cr_gpm_lna9_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_fpga2_cr_gpm_lna9_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(GPM_LNA_GAN1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna8_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_fpga2_cr_gpm_lna8_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18) & ((uint32_t)0x00fc0000);
    REG_PL_WR(GPM_LNA_GAN1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna7_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_fpga2_cr_gpm_lna7_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12) & ((uint32_t)0x0003f000);
    REG_PL_WR(GPM_LNA_GAN1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna6_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_fpga2_cr_gpm_lna6_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6) & ((uint32_t)0x00000fc0);
    REG_PL_WR(GPM_LNA_GAN1_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna5_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_fpga2_cr_gpm_lna5_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(GPM_LNA_GAN1_ADDR, localVal);
}

#define GPM_LNA_GAN2_ADDR (RF_FPGA2_BASE + 0x00000908)

static inline uint32_t gpm_lna_gan2_get(void)
{
    return REG_PL_RD(GPM_LNA_GAN2_ADDR);
}

static inline void gpm_lna_gan2_set(uint32_t x)
{
    REG_PL_WR(GPM_LNA_GAN2_ADDR, x);
}


static inline uint32_t rf_fpga2_cr_gpm_lna14_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rf_fpga2_cr_gpm_lna14_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(GPM_LNA_GAN2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna13_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    return ((localVal & ((uint32_t)0x00fc0000)) >> 18);
}

static inline void rf_fpga2_cr_gpm_lna13_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    localVal &= ~((uint32_t)0x00fc0000);
    localVal |= (x << 18) & ((uint32_t)0x00fc0000);
    REG_PL_WR(GPM_LNA_GAN2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna12_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    return ((localVal & ((uint32_t)0x0003f000)) >> 12);
}

static inline void rf_fpga2_cr_gpm_lna12_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    localVal &= ~((uint32_t)0x0003f000);
    localVal |= (x << 12) & ((uint32_t)0x0003f000);
    REG_PL_WR(GPM_LNA_GAN2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna11_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    return ((localVal & ((uint32_t)0x00000fc0)) >> 6);
}

static inline void rf_fpga2_cr_gpm_lna11_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    localVal &= ~((uint32_t)0x00000fc0);
    localVal |= (x << 6) & ((uint32_t)0x00000fc0);
    REG_PL_WR(GPM_LNA_GAN2_ADDR, localVal);
}


static inline uint32_t rf_fpga2_cr_gpm_lna10_val_getf(void)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rf_fpga2_cr_gpm_lna10_val_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(GPM_LNA_GAN2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(GPM_LNA_GAN2_ADDR, localVal);
}

#endif // _REG_RF_FPGA2_H_
