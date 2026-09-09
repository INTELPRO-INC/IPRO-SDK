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

#ifndef _REG_BL_RC2_H_
#define _REG_BL_RC2_H_
// This file is generated based on the provided Excel sheet

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"

#ifndef BL_RC2_BASE
#define BL_RC2_BASE (0x24c0c000)
#endif


#define RX0_VGA_CFG_ADDR (BL_RC2_BASE + 0x00000040)

static inline uint32_t rx0_vga_cfg_get(void)
{
    return REG_PL_RD(RX0_VGA_CFG_ADDR);
}

static inline void rx0_vga_cfg_set(uint32_t x)
{
    REG_PL_WR(RX0_VGA_CFG_ADDR, x);
}


static inline uint32_t rc2_rx0_vga_approx_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_rx0_vga_approx_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(RX0_VGA_CFG_ADDR, localVal);
}


static inline uint32_t rc2_rx0_vga_gain_min_db_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x7e000000)) >> 25);
}

static inline void rc2_rx0_vga_gain_min_db_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    localVal &= ~((uint32_t)0x7e000000);
    localVal |= (x << 25) & ((uint32_t)0x7e000000);
    REG_PL_WR(RX0_VGA_CFG_ADDR, localVal);
}


static inline uint32_t rc2_rx0_vga_idx_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x01f00000)) >> 20);
}

static inline void rc2_rx0_vga_idx_max_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    localVal &= ~((uint32_t)0x01f00000);
    localVal |= (x << 20) & ((uint32_t)0x01f00000);
    REG_PL_WR(RX0_VGA_CFG_ADDR, localVal);
}


static inline uint32_t rc2_rx0_vga_idx_min_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000f8000)) >> 15);
}

static inline void rc2_rx0_vga_idx_min_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_VGA_CFG_ADDR);
    localVal &= ~((uint32_t)0x000f8000);
    localVal |= (x << 15) & ((uint32_t)0x000f8000);
    REG_PL_WR(RX0_VGA_CFG_ADDR, localVal);
}

#define RX0_LNA_CFG_ADDR (BL_RC2_BASE + 0x00000044)

static inline uint32_t rx0_lna_cfg_get(void)
{
    return REG_PL_RD(RX0_LNA_CFG_ADDR);
}

static inline void rx0_lna_cfg_set(uint32_t x)
{
    REG_PL_WR(RX0_LNA_CFG_ADDR, x);
}


static inline uint32_t rc2_rx0_lna_idx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_rx0_lna_idx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RX0_LNA_CFG_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_idx_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_rx0_lna_idx_max_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RX0_LNA_CFG_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_idx_min_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_rx0_lna_idx_min_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_CFG_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RX0_LNA_CFG_ADDR, localVal);
}

#define RX0_LNA_GAIN_TABLE_0_ADDR (BL_RC2_BASE + 0x00000080)

static inline uint32_t rx0_lna_gain_table_0_get(void)
{
    return REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
}

static inline void rx0_lna_gain_table_0_set(uint32_t x)
{
    REG_PL_WR(RX0_LNA_GAIN_TABLE_0_ADDR, x);
}


static inline uint32_t rc2_rx0_lna_gain_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_rx0_lna_gain_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RX0_LNA_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_rx0_lna_gain_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RX0_LNA_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_rx0_lna_gain_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RX0_LNA_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_rx0_lna_gain_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RX0_LNA_GAIN_TABLE_0_ADDR, localVal);
}

#define RX0_LNA_GAIN_TALE_1_ADDR (BL_RC2_BASE + 0x00000084)

static inline uint32_t rx0_lna_gain_tale_1_get(void)
{
    return REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
}

static inline void rx0_lna_gain_tale_1_set(uint32_t x)
{
    REG_PL_WR(RX0_LNA_GAIN_TALE_1_ADDR, x);
}


static inline uint32_t rc2_rx0_lna_gain_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_rx0_lna_gain_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RX0_LNA_GAIN_TALE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_rx0_lna_gain_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RX0_LNA_GAIN_TALE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_rx0_lna_gain_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RX0_LNA_GAIN_TALE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_rx0_lna_gain_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RX0_LNA_GAIN_TALE_1_ADDR, localVal);
}

#define RX0_LNA_GAIN_TALE_2_ADDR (BL_RC2_BASE + 0x00000088)

static inline uint32_t rx0_lna_gain_tale_2_get(void)
{
    return REG_PL_RD(RX0_LNA_GAIN_TALE_2_ADDR);
}

static inline void rx0_lna_gain_tale_2_set(uint32_t x)
{
    REG_PL_WR(RX0_LNA_GAIN_TALE_2_ADDR, x);
}


static inline uint32_t rc2_rx0_lna_gain_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_rx0_lna_gain_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RX0_LNA_GAIN_TALE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_lna_gain_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_2_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_rx0_lna_gain_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_LNA_GAIN_TALE_2_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RX0_LNA_GAIN_TALE_2_ADDR, localVal);
}

#define RX0_RBB_CFG_TABLE_0_ADDR (BL_RC2_BASE + 0x00000100)

static inline uint32_t rx0_rbb_cfg_table_0_get(void)
{
    return REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
}

static inline void rx0_rbb_cfg_table_0_set(uint32_t x)
{
    REG_PL_WR(RX0_RBB_CFG_TABLE_0_ADDR, x);
}


static inline uint32_t rc2_rx0_rbb_cfg_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x1f000000)) >> 24);
}

static inline void rc2_rx0_rbb_cfg_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x1f000000);
    localVal |= (x << 24) & ((uint32_t)0x1f000000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rc2_rx0_rbb_cfg_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16) & ((uint32_t)0x001f0000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00001f00)) >> 8);
}

static inline void rc2_rx0_rbb_cfg_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00001f00);
    localVal |= (x << 8) & ((uint32_t)0x00001f00);
    REG_PL_WR(RX0_RBB_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rc2_rx0_rbb_cfg_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(RX0_RBB_CFG_TABLE_0_ADDR, localVal);
}

#define RX0_RBB_CFG_TABLE_1_ADDR (BL_RC2_BASE + 0x00000104)

static inline uint32_t rx0_rbb_cfg_table_1_get(void)
{
    return REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
}

static inline void rx0_rbb_cfg_table_1_set(uint32_t x)
{
    REG_PL_WR(RX0_RBB_CFG_TABLE_1_ADDR, x);
}


static inline uint32_t rc2_rx0_rbb_cfg_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x1f000000)) >> 24);
}

static inline void rc2_rx0_rbb_cfg_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x1f000000);
    localVal |= (x << 24) & ((uint32_t)0x1f000000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rc2_rx0_rbb_cfg_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16) & ((uint32_t)0x001f0000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00001f00)) >> 8);
}

static inline void rc2_rx0_rbb_cfg_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00001f00);
    localVal |= (x << 8) & ((uint32_t)0x00001f00);
    REG_PL_WR(RX0_RBB_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rc2_rx0_rbb_cfg_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(RX0_RBB_CFG_TABLE_1_ADDR, localVal);
}

#define RX0_RBB_CFG_TABLE_2_ADDR (BL_RC2_BASE + 0x00000108)

static inline uint32_t rx0_rbb_cfg_table_2_get(void)
{
    return REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
}

static inline void rx0_rbb_cfg_table_2_set(uint32_t x)
{
    REG_PL_WR(RX0_RBB_CFG_TABLE_2_ADDR, x);
}


static inline uint32_t rc2_rx0_rbb_cfg_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x1f000000)) >> 24);
}

static inline void rc2_rx0_rbb_cfg_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x1f000000);
    localVal |= (x << 24) & ((uint32_t)0x1f000000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rc2_rx0_rbb_cfg_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16) & ((uint32_t)0x001f0000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x00001f00)) >> 8);
}

static inline void rc2_rx0_rbb_cfg_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x00001f00);
    localVal |= (x << 8) & ((uint32_t)0x00001f00);
    REG_PL_WR(RX0_RBB_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rc2_rx0_rbb_cfg_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(RX0_RBB_CFG_TABLE_2_ADDR, localVal);
}

#define RX0_RBB_CFG_TABLE_3_ADDR (BL_RC2_BASE + 0x0000010c)

static inline uint32_t rx0_rbb_cfg_table_3_get(void)
{
    return REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
}

static inline void rx0_rbb_cfg_table_3_set(uint32_t x)
{
    REG_PL_WR(RX0_RBB_CFG_TABLE_3_ADDR, x);
}


static inline uint32_t rc2_rx0_rbb_cfg_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x1f000000)) >> 24);
}

static inline void rc2_rx0_rbb_cfg_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x1f000000);
    localVal |= (x << 24) & ((uint32_t)0x1f000000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rc2_rx0_rbb_cfg_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16) & ((uint32_t)0x001f0000);
    REG_PL_WR(RX0_RBB_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x00001f00)) >> 8);
}

static inline void rc2_rx0_rbb_cfg_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x00001f00);
    localVal |= (x << 8) & ((uint32_t)0x00001f00);
    REG_PL_WR(RX0_RBB_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_rbb_cfg_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rc2_rx0_rbb_cfg_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(RX0_RBB_CFG_TABLE_3_ADDR, localVal);
}

#define RX0_RBB_CFG_TABLE_4_ADDR (BL_RC2_BASE + 0x00000110)

static inline uint32_t rx0_rbb_cfg_table_4_get(void)
{
    return REG_PL_RD(RX0_RBB_CFG_TABLE_4_ADDR);
}

static inline void rx0_rbb_cfg_table_4_set(uint32_t x)
{
    REG_PL_WR(RX0_RBB_CFG_TABLE_4_ADDR, x);
}


static inline uint32_t rc2_rx0_rbb_cfg_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x0000001f)) >> 0);
}

static inline void rc2_rx0_rbb_cfg_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_RBB_CFG_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x0000001f);
    localVal |= (x << 0) & ((uint32_t)0x0000001f);
    REG_PL_WR(RX0_RBB_CFG_TABLE_4_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_0_ADDR (BL_RC2_BASE + 0x00000180)

static inline uint32_t rx0_nf_gain_table_0_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
}

static inline void rx0_nf_gain_table_0_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_0_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_0_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_1_ADDR (BL_RC2_BASE + 0x00000184)

static inline uint32_t rx0_nf_gain_table_1_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
}

static inline void rx0_nf_gain_table_1_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_1_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_1_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_2_ADDR (BL_RC2_BASE + 0x00000188)

static inline uint32_t rx0_nf_gain_table_2_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
}

static inline void rx0_nf_gain_table_2_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_2_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_2_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_3_ADDR (BL_RC2_BASE + 0x0000018c)

static inline uint32_t rx0_nf_gain_table_3_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
}

static inline void rx0_nf_gain_table_3_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_3_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_3_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_4_ADDR (BL_RC2_BASE + 0x00000190)

static inline uint32_t rx0_nf_gain_table_4_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
}

static inline void rx0_nf_gain_table_4_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_4_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_19_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_19_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_4_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_18_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_18_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_4_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_17_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_17_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_4_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_16_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_16_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_4_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_4_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_5_ADDR (BL_RC2_BASE + 0x00000194)

static inline uint32_t rx0_nf_gain_table_5_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
}

static inline void rx0_nf_gain_table_5_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_5_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_23_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_23_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_5_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_22_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_22_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_5_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_21_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_21_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_5_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_5_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_5_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_6_ADDR (BL_RC2_BASE + 0x00000198)

static inline uint32_t rx0_nf_gain_table_6_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
}

static inline void rx0_nf_gain_table_6_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_6_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_27_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_27_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_6_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_26_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_26_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_6_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_25_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_25_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_6_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_24_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_24_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_6_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_6_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_7_ADDR (BL_RC2_BASE + 0x0000019c)

static inline uint32_t rx0_nf_gain_table_7_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
}

static inline void rx0_nf_gain_table_7_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_7_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_31_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_31_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_7_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_30_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_30_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_7_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_29_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_29_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_7_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_28_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_28_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_7_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_7_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_8_ADDR (BL_RC2_BASE + 0x000001a0)

static inline uint32_t rx0_nf_gain_table_8_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
}

static inline void rx0_nf_gain_table_8_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_8_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_35_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_35_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_8_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_34_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_34_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_8_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_33_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_33_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_8_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_32_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_32_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_8_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_8_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_9_ADDR (BL_RC2_BASE + 0x000001a4)

static inline uint32_t rx0_nf_gain_table_9_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
}

static inline void rx0_nf_gain_table_9_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_9_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_39_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_39_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_9_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_38_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_38_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_9_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_37_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_37_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_9_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_36_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_36_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_9_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_9_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_10_ADDR (BL_RC2_BASE + 0x000001a8)

static inline uint32_t rx0_nf_gain_table_10_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
}

static inline void rx0_nf_gain_table_10_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_10_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_43_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_43_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_10_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_42_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_42_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_10_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_41_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_41_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_10_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_10_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_10_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_11_ADDR (BL_RC2_BASE + 0x000001ac)

static inline uint32_t rx0_nf_gain_table_11_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
}

static inline void rx0_nf_gain_table_11_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_11_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_47_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_47_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_11_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_46_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_46_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_11_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_45_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_45_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_11_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_44_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_44_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_11_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_11_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_12_ADDR (BL_RC2_BASE + 0x000001b0)

static inline uint32_t rx0_nf_gain_table_12_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
}

static inline void rx0_nf_gain_table_12_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_12_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_51_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_51_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_12_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_50_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_50_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_12_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_49_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_49_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_12_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_48_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_48_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_12_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_12_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_13_ADDR (BL_RC2_BASE + 0x000001b4)

static inline uint32_t rx0_nf_gain_table_13_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
}

static inline void rx0_nf_gain_table_13_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_13_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_55_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_55_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_13_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_54_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_54_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_13_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_53_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_53_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_13_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_52_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_52_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_13_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_13_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_14_ADDR (BL_RC2_BASE + 0x000001b8)

static inline uint32_t rx0_nf_gain_table_14_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
}

static inline void rx0_nf_gain_table_14_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_14_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_59_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_59_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_14_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_58_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_58_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_14_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_57_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_57_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_14_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_56_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_56_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_14_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_14_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_15_ADDR (BL_RC2_BASE + 0x000001bc)

static inline uint32_t rx0_nf_gain_table_15_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
}

static inline void rx0_nf_gain_table_15_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_15_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_63_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_63_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_15_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_62_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_62_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_15_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_61_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_61_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_15_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_60_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_60_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_15_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_15_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_16_ADDR (BL_RC2_BASE + 0x000001c0)

static inline uint32_t rx0_nf_gain_table_16_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
}

static inline void rx0_nf_gain_table_16_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_16_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_67_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_67_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_16_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_66_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_66_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_16_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_65_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_65_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_16_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_64_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_64_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_16_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_16_ADDR, localVal);
}

#define RX0_NF_GAIN_TABLE_17_ADDR (BL_RC2_BASE + 0x000001c4)

static inline uint32_t rx0_nf_gain_table_17_get(void)
{
    return REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
}

static inline void rx0_nf_gain_table_17_set(uint32_t x)
{
    REG_PL_WR(RX0_NF_GAIN_TABLE_17_ADDR, x);
}


static inline uint32_t rc2_rx0_nf_gain_71_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    return ((localVal & ((uint32_t)0x3f000000)) >> 24);
}

static inline void rc2_rx0_nf_gain_71_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    localVal &= ~((uint32_t)0x3f000000);
    localVal |= (x << 24) & ((uint32_t)0x3f000000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_17_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_70_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    return ((localVal & ((uint32_t)0x003f0000)) >> 16);
}

static inline void rc2_rx0_nf_gain_70_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    localVal &= ~((uint32_t)0x003f0000);
    localVal |= (x << 16) & ((uint32_t)0x003f0000);
    REG_PL_WR(RX0_NF_GAIN_TABLE_17_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_69_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_rx0_nf_gain_69_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(RX0_NF_GAIN_TABLE_17_ADDR, localVal);
}


static inline uint32_t rc2_rx0_nf_gain_68_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_rx0_nf_gain_68_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX0_NF_GAIN_TABLE_17_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RX0_NF_GAIN_TABLE_17_ADDR, localVal);
}

#define TX0_TXG_CFG_TABLE_0_ADDR (BL_RC2_BASE + 0x00000200)

static inline uint32_t tx0_txg_cfg_table_0_get(void)
{
    return REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
}

static inline void tx0_txg_cfg_table_0_set(uint32_t x)
{
    REG_PL_WR(TX0_TXG_CFG_TABLE_0_ADDR, x);
}


static inline uint32_t rc2_tx0_txg_cfg_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_tx0_txg_cfg_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_tx0_txg_cfg_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_tx0_txg_cfg_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(TX0_TXG_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_tx0_txg_cfg_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(TX0_TXG_CFG_TABLE_0_ADDR, localVal);
}

#define TX0_TXG_CFG_TABLE_1_ADDR (BL_RC2_BASE + 0x00000204)

static inline uint32_t tx0_txg_cfg_table_1_get(void)
{
    return REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
}

static inline void tx0_txg_cfg_table_1_set(uint32_t x)
{
    REG_PL_WR(TX0_TXG_CFG_TABLE_1_ADDR, x);
}


static inline uint32_t rc2_tx0_txg_cfg_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_tx0_txg_cfg_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_tx0_txg_cfg_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_tx0_txg_cfg_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(TX0_TXG_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_tx0_txg_cfg_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(TX0_TXG_CFG_TABLE_1_ADDR, localVal);
}

#define TX0_TXG_CFG_TABLE_2_ADDR (BL_RC2_BASE + 0x00000208)

static inline uint32_t tx0_txg_cfg_table_2_get(void)
{
    return REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
}

static inline void tx0_txg_cfg_table_2_set(uint32_t x)
{
    REG_PL_WR(TX0_TXG_CFG_TABLE_2_ADDR, x);
}


static inline uint32_t rc2_tx0_txg_cfg_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_tx0_txg_cfg_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_tx0_txg_cfg_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_tx0_txg_cfg_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(TX0_TXG_CFG_TABLE_2_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_tx0_txg_cfg_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_2_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(TX0_TXG_CFG_TABLE_2_ADDR, localVal);
}

#define TX0_TXG_CFG_TABLE_3_ADDR (BL_RC2_BASE + 0x0000020c)

static inline uint32_t tx0_txg_cfg_table_3_get(void)
{
    return REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
}

static inline void tx0_txg_cfg_table_3_set(uint32_t x)
{
    REG_PL_WR(TX0_TXG_CFG_TABLE_3_ADDR, x);
}


static inline uint32_t rc2_tx0_txg_cfg_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_tx0_txg_cfg_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_tx0_txg_cfg_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(TX0_TXG_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_tx0_txg_cfg_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(TX0_TXG_CFG_TABLE_3_ADDR, localVal);
}


static inline uint32_t rc2_tx0_txg_cfg_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_tx0_txg_cfg_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TXG_CFG_TABLE_3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(TX0_TXG_CFG_TABLE_3_ADDR, localVal);
}

#define TX0_TBBD_CFG_TABLE_0_ADDR (BL_RC2_BASE + 0x00000210)

static inline uint32_t tx0_tbbd_cfg_table_0_get(void)
{
    return REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
}

static inline void tx0_tbbd_cfg_table_0_set(uint32_t x)
{
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, x);
}


static inline uint32_t rc2_tx0_tbbd_cfg_7_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rc2_tx0_tbbd_cfg_7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28) & ((uint32_t)0x70000000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_6_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rc2_tx0_tbbd_cfg_6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24) & ((uint32_t)0x07000000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_5_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rc2_tx0_tbbd_cfg_5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20) & ((uint32_t)0x00700000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_4_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rc2_tx0_tbbd_cfg_4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16) & ((uint32_t)0x00070000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_3_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rc2_tx0_tbbd_cfg_3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12) & ((uint32_t)0x00007000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_2_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rc2_tx0_tbbd_cfg_2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8) & ((uint32_t)0x00000700);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_1_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rc2_tx0_tbbd_cfg_1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4) & ((uint32_t)0x00000070);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_0_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rc2_tx0_tbbd_cfg_0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_0_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0) & ((uint32_t)0x00000007);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_0_ADDR, localVal);
}

#define TX0_TBBD_CFG_TABLE_1_ADDR (BL_RC2_BASE + 0x00000214)

static inline uint32_t tx0_tbbd_cfg_table_1_get(void)
{
    return REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
}

static inline void tx0_tbbd_cfg_table_1_set(uint32_t x)
{
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, x);
}


static inline uint32_t rc2_tx0_tbbd_cfg_15_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rc2_tx0_tbbd_cfg_15_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28) & ((uint32_t)0x70000000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_14_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x07000000)) >> 24);
}

static inline void rc2_tx0_tbbd_cfg_14_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x07000000);
    localVal |= (x << 24) & ((uint32_t)0x07000000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_13_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00700000)) >> 20);
}

static inline void rc2_tx0_tbbd_cfg_13_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00700000);
    localVal |= (x << 20) & ((uint32_t)0x00700000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_12_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rc2_tx0_tbbd_cfg_12_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16) & ((uint32_t)0x00070000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_11_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rc2_tx0_tbbd_cfg_11_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12) & ((uint32_t)0x00007000);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_10_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00000700)) >> 8);
}

static inline void rc2_tx0_tbbd_cfg_10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00000700);
    localVal |= (x << 8) & ((uint32_t)0x00000700);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_9_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rc2_tx0_tbbd_cfg_9_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4) & ((uint32_t)0x00000070);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}


static inline uint32_t rc2_tx0_tbbd_cfg_8_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rc2_tx0_tbbd_cfg_8_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX0_TBBD_CFG_TABLE_1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0) & ((uint32_t)0x00000007);
    REG_PL_WR(TX0_TBBD_CFG_TABLE_1_ADDR, localVal);
}

#define TX_FRONTEND_0_ADDR (BL_RC2_BASE + 0x00000218)

static inline uint32_t tx_frontend_0_get(void)
{
    return REG_PL_RD(TX_FRONTEND_0_ADDR);
}

static inline void tx_frontend_0_set(uint32_t x)
{
    REG_PL_WR(TX_FRONTEND_0_ADDR, x);
}


static inline uint32_t rc2_txhbf20coeffsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX_FRONTEND_0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_txhbf20coeffsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX_FRONTEND_0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(TX_FRONTEND_0_ADDR, localVal);
}

#define RWNXFERXINCNTL_ADDR (BL_RC2_BASE + 0x00000800)

static inline uint32_t rwnxferxincntl_get(void)
{
    return REG_PL_RD(RWNXFERXINCNTL_ADDR);
}

static inline void rwnxferxincntl_set(uint32_t x)
{
    REG_PL_WR(RWNXFERXINCNTL_ADDR, x);
}


static inline uint32_t rc2_rx_ramp_ctrl_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0xc0000000)) >> 30);
}

static inline void rc2_rx_ramp_ctrl_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0xc0000000);
    localVal |= (x << 30) & ((uint32_t)0xc0000000);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_feadc0_dly_ena______getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline void rc2_feadc0_dly_ena______setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x00080000);
    localVal |= (x << 19) & ((uint32_t)0x00080000);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_feadc0_dly_num______getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rc2_feadc0_dly_num______setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16) & ((uint32_t)0x00070000);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_fe0dcest_dly_ena_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00001000)) >> 12);
}

static inline void rc2_fe0dcest_dly_ena_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x00001000);
    localVal |= (x << 12) & ((uint32_t)0x00001000);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_fe0dcest_dly_num_20_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rc2_fe0dcest_dly_num_20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8) & ((uint32_t)0x00000f00);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_fe0dcest_dly_ena_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rc2_fe0dcest_dly_ena_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4) & ((uint32_t)0x00000010);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}


static inline uint32_t rc2_fe0dcest_dly_num_40_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rc2_fe0dcest_dly_num_40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFERXINCNTL_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0) & ((uint32_t)0x0000000f);
    REG_PL_WR(RWNXFERXINCNTL_ADDR, localVal);
}

#define RC2_CCA_CTRL_ADDR (BL_RC2_BASE + 0x00000804)

static inline uint32_t rc2_cca_ctrl_get(void)
{
    return REG_PL_RD(RC2_CCA_CTRL_ADDR);
}

static inline void rc2_cca_ctrl_set(uint32_t x)
{
    REG_PL_WR(RC2_CCA_CTRL_ADDR, x);
}


static inline uint32_t rc2_tx_atten_src_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rc2_tx_atten_src_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3) & ((uint32_t)0x00000008);
    REG_PL_WR(RC2_CCA_CTRL_ADDR, localVal);
}


static inline uint32_t rc2_tx_atten_sw_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rc2_tx_atten_sw_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0) & ((uint32_t)0x00000007);
    REG_PL_WR(RC2_CCA_CTRL_ADDR, localVal);
}


static inline uint32_t rc2_cr_4s_cca_primary_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rc2_cr_4s_cca_primary_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_CCA_CTRL_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0) & ((uint32_t)0x00000001);
    REG_PL_WR(RC2_CCA_CTRL_ADDR, localVal);
}

#define RWNXFETXDCCOMP_ADDR (BL_RC2_BASE + 0x00000808)

static inline uint32_t rwnxfetxdccomp_get(void)
{
    return REG_PL_RD(RWNXFETXDCCOMP_ADDR);
}

static inline void rwnxfetxdccomp_set(uint32_t x)
{
    REG_PL_WR(RWNXFETXDCCOMP_ADDR, x);
}


static inline uint32_t rc2_dc_comp_q_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFETXDCCOMP_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_dc_comp_q_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFETXDCCOMP_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RWNXFETXDCCOMP_ADDR, localVal);
}


static inline uint32_t rc2_dc_comp_i_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFETXDCCOMP_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_dc_comp_i_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFETXDCCOMP_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RWNXFETXDCCOMP_ADDR, localVal);
}

#define RWNXFECTRL0_ADDR (BL_RC2_BASE + 0x0000080c)

static inline uint32_t rwnxfectrl0_get(void)
{
    return REG_PL_RD(RWNXFECTRL0_ADDR);
}

static inline void rwnxfectrl0_set(uint32_t x)
{
    REG_PL_WR(RWNXFECTRL0_ADDR, x);
}


static inline uint32_t rc2_txdsssdiggainlin0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXFECTRL0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_txdsssdiggainlin0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXFECTRL0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RWNXFECTRL0_ADDR, localVal);
}

#define RWNXAGCDSP5_ADDR (BL_RC2_BASE + 0x00000810)

static inline uint32_t rwnxagcdsp5_get(void)
{
    return REG_PL_RD(RWNXAGCDSP5_ADDR);
}

static inline void rwnxagcdsp5_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCDSP5_ADDR, x);
}


static inline uint32_t rc2_corr_qdbm_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_corr_qdbm_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RWNXAGCDSP5_ADDR, localVal);
}


static inline uint32_t rc2_corr_dbm_thr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_corr_dbm_thr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RWNXAGCDSP5_ADDR, localVal);
}


static inline uint32_t rc2_corr_rfgain_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rc2_corr_rfgain_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14) & ((uint32_t)0x00004000);
    REG_PL_WR(RWNXAGCDSP5_ADDR, localVal);
}


static inline uint32_t rc2_corr_dbm_lsbless_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    return ((localVal & ((uint32_t)0x00003ff0)) >> 4);
}

static inline void rc2_corr_dbm_lsbless_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    localVal &= ~((uint32_t)0x00003ff0);
    localVal |= (x << 4) & ((uint32_t)0x00003ff0);
    REG_PL_WR(RWNXAGCDSP5_ADDR, localVal);
}


static inline uint32_t rc2_corr_iir_mu_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rc2_corr_iir_mu_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCDSP5_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0) & ((uint32_t)0x0000000f);
    REG_PL_WR(RWNXAGCDSP5_ADDR, localVal);
}

#define RWNXAGCPKDETIF_ADDR (BL_RC2_BASE + 0x00000814)

static inline uint32_t rwnxagcpkdetif_get(void)
{
    return REG_PL_RD(RWNXAGCPKDETIF_ADDR);
}

static inline void rwnxagcpkdetif_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCPKDETIF_ADDR, x);
}


static inline uint32_t rc2_pkdet_module_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rc2_pkdet_module_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10) & ((uint32_t)0x00000400);
    REG_PL_WR(RWNXAGCPKDETIF_ADDR, localVal);
}


static inline uint32_t rc2_pkdet_cnt_win_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    return ((localVal & ((uint32_t)0x000003c0)) >> 6);
}

static inline void rc2_pkdet_cnt_win_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    localVal &= ~((uint32_t)0x000003c0);
    localVal |= (x << 6) & ((uint32_t)0x000003c0);
    REG_PL_WR(RWNXAGCPKDETIF_ADDR, localVal);
}


static inline uint32_t rc2_pkdet_cnt_thr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    return ((localVal & ((uint32_t)0x0000003c)) >> 2);
}

static inline void rc2_pkdet_cnt_thr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    localVal &= ~((uint32_t)0x0000003c);
    localVal |= (x << 2) & ((uint32_t)0x0000003c);
    REG_PL_WR(RWNXAGCPKDETIF_ADDR, localVal);
}


static inline uint32_t rc2_pkdet_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rc2_pkdet_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCPKDETIF_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0) & ((uint32_t)0x00000003);
    REG_PL_WR(RWNXAGCPKDETIF_ADDR, localVal);
}

#define RWNXAGCOFDMDET0_ADDR (BL_RC2_BASE + 0x00000818)

static inline uint32_t rwnxagcofdmdet0_get(void)
{
    return REG_PL_RD(RWNXAGCOFDMDET0_ADDR);
}

static inline void rwnxagcofdmdet0_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCOFDMDET0_ADDR, x);
}


static inline uint32_t rc2_adj_thr_2040_qdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET0_ADDR);
    return ((localVal & ((uint32_t)0x03ffe000)) >> 13);
}

static inline void rc2_adj_thr_2040_qdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET0_ADDR);
    localVal &= ~((uint32_t)0x03ffe000);
    localVal |= (x << 13) & ((uint32_t)0x03ffe000);
    REG_PL_WR(RWNXAGCOFDMDET0_ADDR, localVal);
}


static inline uint32_t rc2_adj_thr_20_qdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET0_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rc2_adj_thr_20_qdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET0_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(RWNXAGCOFDMDET0_ADDR, localVal);
}

#define RWNXAGCOFDMDET1_ADDR (BL_RC2_BASE + 0x0000081c)

static inline uint32_t rwnxagcofdmdet1_get(void)
{
    return REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
}

static inline void rwnxagcofdmdet1_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, x);
}


static inline uint32_t rc2_force_ed_thr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rc2_force_ed_thr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17) & ((uint32_t)0x00020000);
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, localVal);
}


static inline uint32_t rc2_ed_thr_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rc2_ed_thr_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16) & ((uint32_t)0x00010000);
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, localVal);
}


static inline uint32_t rc2_ed_adj_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rc2_ed_adj_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15) & ((uint32_t)0x00008000);
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, localVal);
}


static inline uint32_t rc2_ed_thr_adj_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    return ((localVal & ((uint32_t)0x00006000)) >> 13);
}

static inline void rc2_ed_thr_adj_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    localVal &= ~((uint32_t)0x00006000);
    localVal |= (x << 13) & ((uint32_t)0x00006000);
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, localVal);
}


static inline uint32_t rc2_ed_thr_qdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rc2_ed_thr_qdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET1_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(RWNXAGCOFDMDET1_ADDR, localVal);
}

#define RWNXAGCOFDMDET2_ADDR (BL_RC2_BASE + 0x00000820)

static inline uint32_t rwnxagcofdmdet2_get(void)
{
    return REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
}

static inline void rwnxagcofdmdet2_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, x);
}


static inline uint32_t rc2_force_cs_thr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void rc2_force_cs_thr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17) & ((uint32_t)0x00020000);
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, localVal);
}


static inline uint32_t rc2_cs_thr_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void rc2_cs_thr_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16) & ((uint32_t)0x00010000);
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, localVal);
}


static inline uint32_t rc2_cs_adj_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rc2_cs_adj_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15) & ((uint32_t)0x00008000);
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, localVal);
}


static inline uint32_t rc2_cs_thr_adj_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    return ((localVal & ((uint32_t)0x00006000)) >> 13);
}

static inline void rc2_cs_thr_adj_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    localVal &= ~((uint32_t)0x00006000);
    localVal |= (x << 13) & ((uint32_t)0x00006000);
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, localVal);
}


static inline uint32_t rc2_cs_thr_qdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rc2_cs_thr_qdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET2_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(RWNXAGCOFDMDET2_ADDR, localVal);
}

#define RWNXAGCOFDMDET3_ADDR (BL_RC2_BASE + 0x00000824)

static inline uint32_t rwnxagcofdmdet3_get(void)
{
    return REG_PL_RD(RWNXAGCOFDMDET3_ADDR);
}

static inline void rwnxagcofdmdet3_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCOFDMDET3_ADDR, x);
}


static inline uint32_t rc2_cs_cnt_thr_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET3_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_cs_cnt_thr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET3_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RWNXAGCOFDMDET3_ADDR, localVal);
}


static inline uint32_t rc2_cs_cnt_win_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_cs_cnt_win_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCOFDMDET3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RWNXAGCOFDMDET3_ADDR, localVal);
}

#define RWNXAGCFASTCNTL_ADDR (BL_RC2_BASE + 0x00000828)

static inline uint32_t rwnxagcfastcntl_get(void)
{
    return REG_PL_RD(RWNXAGCFASTCNTL_ADDR);
}

static inline void rwnxagcfastcntl_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCFASTCNTL_ADDR, x);
}


static inline uint32_t rc2_fast_cntl_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCFASTCNTL_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rc2_fast_cntl_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCFASTCNTL_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4) & ((uint32_t)0x00000010);
    REG_PL_WR(RWNXAGCFASTCNTL_ADDR, localVal);
}


static inline uint32_t rc2_fast_cntl_limit_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCFASTCNTL_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rc2_fast_cntl_limit_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCFASTCNTL_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0) & ((uint32_t)0x0000000f);
    REG_PL_WR(RWNXAGCFASTCNTL_ADDR, localVal);
}

#define RWNXAGCENERGYCOMP_ADDR (BL_RC2_BASE + 0x0000082c)

static inline uint32_t rwnxagcenergycomp_get(void)
{
    return REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
}

static inline void rwnxagcenergycomp_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, x);
}


static inline uint32_t rc2_inbdpowfastvalid_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00ff8000)) >> 15);
}

static inline void rc2_inbdpowfastvalid_cnt_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00ff8000);
    localVal |= (x << 15) & ((uint32_t)0x00ff8000);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpowfastvalid_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rc2_inbdpowfastvalid_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14) & ((uint32_t)0x00004000);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpowinfthr_adj_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline void rc2_inbdpowinfthr_adj_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00003000);
    localVal |= (x << 12) & ((uint32_t)0x00003000);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpowinfthr_adj_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rc2_inbdpowinfthr_adj_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11) & ((uint32_t)0x00000800);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpowsupthr_adj_step_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00000600)) >> 9);
}

static inline void rc2_inbdpowsupthr_adj_step_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00000600);
    localVal |= (x << 9) & ((uint32_t)0x00000600);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpowsupthr_adj_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void rc2_inbdpowsupthr_adj_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8) & ((uint32_t)0x00000100);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}


static inline uint32_t rc2_inbdpow_adj_thr_dbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_inbdpow_adj_thr_dbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCENERGYCOMP_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RWNXAGCENERGYCOMP_ADDR, localVal);
}

#define RWNXAGCEVT4_ADDR (BL_RC2_BASE + 0x00000830)

static inline uint32_t rwnxagcevt4_get(void)
{
    return REG_PL_RD(RWNXAGCEVT4_ADDR);
}

static inline void rwnxagcevt4_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT4_ADDR, x);
}


static inline uint32_t rc2_evt4op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt4op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt4op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt4op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt4pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt4opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt4pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}


static inline uint32_t rc2_evt4tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt4tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT4_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT4_ADDR, localVal);
}

#define ADCPOWEST_ADDR (BL_RC2_BASE + 0x00000834)

static inline uint32_t adcpowest_get(void)
{
    return REG_PL_RD(ADCPOWEST_ADDR);
}

static inline void adcpowest_set(uint32_t x)
{
    REG_PL_WR(ADCPOWEST_ADDR, x);
}


static inline uint32_t rc2_deten_start_bias_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCPOWEST_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline void rc2_deten_start_bias_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCPOWEST_ADDR);
    localVal &= ~((uint32_t)0x000000f0);
    localVal |= (x << 4) & ((uint32_t)0x000000f0);
    REG_PL_WR(ADCPOWEST_ADDR, localVal);
}


static inline uint32_t rc2_adcpowinsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(ADCPOWEST_ADDR);
    return ((localVal & ((uint32_t)0x00000003)) >> 0);
}

static inline void rc2_adcpowinsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(ADCPOWEST_ADDR);
    localVal &= ~((uint32_t)0x00000003);
    localVal |= (x << 0) & ((uint32_t)0x00000003);
    REG_PL_WR(ADCPOWEST_ADDR, localVal);
}

#define CORRCOMP0_ADDR (BL_RC2_BASE + 0x00000838)

static inline uint32_t corrcomp0_get(void)
{
    return REG_PL_RD(CORRCOMP0_ADDR);
}

static inline void corrcomp0_set(uint32_t x)
{
    REG_PL_WR(CORRCOMP0_ADDR, x);
}


static inline uint32_t rc2_reflevofdmthd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_reflevofdmthd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(CORRCOMP0_ADDR, localVal);
}


static inline uint32_t rc2_cccombcc2flag_getf(void)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rc2_cccombcc2flag_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30) & ((uint32_t)0x40000000);
    REG_PL_WR(CORRCOMP0_ADDR, localVal);
}


static inline uint32_t rc2_reflevofdmthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    return ((localVal & ((uint32_t)0x0007ffff)) >> 0);
}

static inline void rc2_reflevofdmthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP0_ADDR);
    localVal &= ~((uint32_t)0x0007ffff);
    localVal |= (x << 0) & ((uint32_t)0x0007ffff);
    REG_PL_WR(CORRCOMP0_ADDR, localVal);
}

#define CORRCOMP1_ADDR (BL_RC2_BASE + 0x0000083c)

static inline uint32_t corrcomp1_get(void)
{
    return REG_PL_RD(CORRCOMP1_ADDR);
}

static inline void corrcomp1_set(uint32_t x)
{
    REG_PL_WR(CORRCOMP1_ADDR, x);
}


static inline uint32_t rc2_reflevdsssthd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_reflevdsssthd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP1_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(CORRCOMP1_ADDR, localVal);
}


static inline uint32_t rc2_reflevdsssthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP1_ADDR);
    return ((localVal & ((uint32_t)0x000fffff)) >> 0);
}

static inline void rc2_reflevdsssthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CORRCOMP1_ADDR);
    localVal &= ~((uint32_t)0x000fffff);
    localVal |= (x << 0) & ((uint32_t)0x000fffff);
    REG_PL_WR(CORRCOMP1_ADDR, localVal);
}

#define DSSSDET_ADDR (BL_RC2_BASE + 0x00000840)

static inline uint32_t dsssdet_get(void)
{
    return REG_PL_RD(DSSSDET_ADDR);
}

static inline void dsssdet_set(uint32_t x)
{
    REG_PL_WR(DSSSDET_ADDR, x);
}


static inline uint32_t rc2_reflevdssscontthd_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_reflevdssscontthd_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSSSDET_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(DSSSDET_ADDR, localVal);
}


static inline uint32_t rc2_reflevdssscontthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(DSSSDET_ADDR);
    return ((localVal & ((uint32_t)0x003fffff)) >> 0);
}

static inline void rc2_reflevdssscontthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(DSSSDET_ADDR);
    localVal &= ~((uint32_t)0x003fffff);
    localVal |= (x << 0) & ((uint32_t)0x003fffff);
    REG_PL_WR(DSSSDET_ADDR, localVal);
}

#define AGCRAMPUP0_ADDR (BL_RC2_BASE + 0x00000844)

static inline uint32_t agcrampup0_get(void)
{
    return REG_PL_RD(AGCRAMPUP0_ADDR);
}

static inline void agcrampup0_set(uint32_t x)
{
    REG_PL_WR(AGCRAMPUP0_ADDR, x);
}


static inline uint32_t rc2_agcrampupthr2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP0_ADDR);
    return ((localVal & ((uint32_t)0x3fff0000)) >> 16);
}

static inline void rc2_agcrampupthr2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP0_ADDR);
    localVal &= ~((uint32_t)0x3fff0000);
    localVal |= (x << 16) & ((uint32_t)0x3fff0000);
    REG_PL_WR(AGCRAMPUP0_ADDR, localVal);
}


static inline uint32_t rc2_agcrampupthr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP0_ADDR);
    return ((localVal & ((uint32_t)0x00003fff)) >> 0);
}

static inline void rc2_agcrampupthr1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP0_ADDR);
    localVal &= ~((uint32_t)0x00003fff);
    localVal |= (x << 0) & ((uint32_t)0x00003fff);
    REG_PL_WR(AGCRAMPUP0_ADDR, localVal);
}

#define AGCRAMPUP1_ADDR (BL_RC2_BASE + 0x00000848)

static inline uint32_t agcrampup1_get(void)
{
    return REG_PL_RD(AGCRAMPUP1_ADDR);
}

static inline void agcrampup1_set(uint32_t x)
{
    REG_PL_WR(AGCRAMPUP1_ADDR, x);
}


static inline uint32_t rc2_agcrampupthr3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP1_ADDR);
    return ((localVal & ((uint32_t)0x00003fff)) >> 0);
}

static inline void rc2_agcrampupthr3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP1_ADDR);
    localVal &= ~((uint32_t)0x00003fff);
    localVal |= (x << 0) & ((uint32_t)0x00003fff);
    REG_PL_WR(AGCRAMPUP1_ADDR, localVal);
}

#define AGCRAMPUP2_ADDR (BL_RC2_BASE + 0x0000084c)

static inline uint32_t agcrampup2_get(void)
{
    return REG_PL_RD(AGCRAMPUP2_ADDR);
}

static inline void agcrampup2_set(uint32_t x)
{
    REG_PL_WR(AGCRAMPUP2_ADDR, x);
}


static inline uint32_t rc2_agcrampupcorrecten_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_agcrampupcorrecten_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCRAMPUP2_ADDR, localVal);
}


static inline uint32_t rc2_agcrampupcorrect3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_agcrampupcorrect3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCRAMPUP2_ADDR, localVal);
}


static inline uint32_t rc2_agcrampupcorrect2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_agcrampupcorrect2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCRAMPUP2_ADDR, localVal);
}


static inline uint32_t rc2_agcrampupcorrect1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_agcrampupcorrect1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCRAMPUP2_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCRAMPUP2_ADDR, localVal);
}

#define AGCLOADREG0_ADDR (BL_RC2_BASE + 0x00000850)

static inline uint32_t agcloadreg0_get(void)
{
    return REG_PL_RD(AGCLOADREG0_ADDR);
}

static inline void agcloadreg0_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG0_ADDR, x);
}


static inline uint32_t rc2_reglnaindex_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_reglnaindex_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(AGCLOADREG0_ADDR, localVal);
}


static inline uint32_t rc2_reglnagain_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_reglnagain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG0_ADDR, localVal);
}


static inline uint32_t rc2_reglnastep_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_reglnastep_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG0_ADDR, localVal);
}


static inline uint32_t rc2_regrbbstep_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_regrbbstep_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG0_ADDR, localVal);
}

#define AGCLOADREG1_ADDR (BL_RC2_BASE + 0x00000854)

static inline uint32_t agcloadreg1_get(void)
{
    return REG_PL_RD(AGCLOADREG1_ADDR);
}

static inline void agcloadreg1_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG1_ADDR, x);
}


static inline uint32_t rc2_regrfgaininit0_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_regrfgaininit0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(AGCLOADREG1_ADDR, localVal);
}


static inline uint32_t rc2_regadctgtdvb_11b_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_regadctgtdvb_11b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG1_ADDR, localVal);
}


static inline uint32_t rc2_regadctgtdvb_11g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_regadctgtdvb_11g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG1_ADDR, localVal);
}


static inline uint32_t rc2_regadctgtdvb_11n_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_regadctgtdvb_11n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG1_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG1_ADDR, localVal);
}

#define AGCLOADREG2_ADDR (BL_RC2_BASE + 0x00000858)

static inline uint32_t agcloadreg2_get(void)
{
    return REG_PL_RD(AGCLOADREG2_ADDR);
}

static inline void agcloadreg2_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG2_ADDR, x);
}


static inline uint32_t rc2_regrfgaininit1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_regrfgaininit1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(AGCLOADREG2_ADDR, localVal);
}


static inline uint32_t rc2_reginbdtgtdbv_11b_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_reginbdtgtdbv_11b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG2_ADDR, localVal);
}


static inline uint32_t rc2_reginbdtgtdbv_11g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_reginbdtgtdbv_11g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG2_ADDR, localVal);
}


static inline uint32_t rc2_reginbdtgtdbv_11n_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_reginbdtgtdbv_11n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG2_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG2_ADDR, localVal);
}

#define AGCLOADREG3_ADDR (BL_RC2_BASE + 0x0000085c)

static inline uint32_t agcloadreg3_get(void)
{
    return REG_PL_RD(AGCLOADREG3_ADDR);
}

static inline void agcloadreg3_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG3_ADDR, x);
}


static inline uint32_t rc2_regmaxgain_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_regmaxgain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG3_ADDR, localVal);
}


static inline uint32_t rc2_regmingain_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_regmingain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG3_ADDR, localVal);
}


static inline uint32_t rc2_regmingain4sat_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_regmingain4sat_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG3_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG3_ADDR, localVal);
}

#define CSCCAREG0_ADDR (BL_RC2_BASE + 0x00000860)

static inline uint32_t csccareg0_get(void)
{
    return REG_PL_RD(CSCCAREG0_ADDR);
}

static inline void csccareg0_set(uint32_t x)
{
    REG_PL_WR(CSCCAREG0_ADDR, x);
}


static inline uint32_t rc2_regcsccarssithr_getf(void)
{
    uint32_t localVal = REG_PL_RD(CSCCAREG0_ADDR);
    return ((localVal & ((uint32_t)0x00001fff)) >> 0);
}

static inline void rc2_regcsccarssithr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(CSCCAREG0_ADDR);
    localVal &= ~((uint32_t)0x00001fff);
    localVal |= (x << 0) & ((uint32_t)0x00001fff);
    REG_PL_WR(CSCCAREG0_ADDR, localVal);
}

#define BLAGCFSM_ADDR (BL_RC2_BASE + 0x00000864)

static inline uint32_t blagcfsm_get(void)
{
    return REG_PL_RD(BLAGCFSM_ADDR);
}

static inline void blagcfsm_set(uint32_t x)
{
    REG_PL_WR(BLAGCFSM_ADDR, x);
}


static inline uint32_t rc2_cfg_band_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfg_band_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(BLAGCFSM_ADDR, localVal);
}

static inline uint32_t rc2_cfg_force5_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    return ((localVal & ((uint32_t)0x00008000)) >> 15);
}

static inline void rc2_cfg_force5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    localVal &= ~((uint32_t)0x00008000);
    localVal |= (x << 15) & ((uint32_t)0x00008000);
    REG_PL_WR(BLAGCFSM_ADDR, localVal);
}


static inline uint32_t rc2_cfg_force10_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    return ((localVal & ((uint32_t)0x00004000)) >> 14);
}

static inline void rc2_cfg_force10_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    localVal &= ~((uint32_t)0x00004000);
    localVal |= (x << 14) & ((uint32_t)0x00004000);
    REG_PL_WR(BLAGCFSM_ADDR, localVal);
}

static inline uint32_t rc2_blagcfsmreg_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    return ((localVal & ((uint32_t)0x00003fff)) >> 0);
}

static inline void rc2_blagcfsmreg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLAGCFSM_ADDR);
    localVal &= ~((uint32_t)0x00003fff);
    localVal |= (x << 0) & ((uint32_t)0x00003fff);
    REG_PL_WR(BLAGCFSM_ADDR, localVal);
}

#define BLRADAR0_ADDR (BL_RC2_BASE + 0x00000868)

static inline uint32_t blradar0_get(void)
{
    return REG_PL_RD(BLRADAR0_ADDR);
}

static inline void blradar0_set(uint32_t x)
{
    REG_PL_WR(BLRADAR0_ADDR, x);
}


static inline uint32_t rc2_blcrradarploadfromagc_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLRADAR0_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_blcrradarploadfromagc_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLRADAR0_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(BLRADAR0_ADDR, localVal);
}


static inline uint32_t rc2_blcrradarparam_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLRADAR0_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void rc2_blcrradarparam_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLRADAR0_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0) & ((uint32_t)0x0000ffff);
    REG_PL_WR(BLRADAR0_ADDR, localVal);
}

#define RWNXAGCEVT5_ADDR (BL_RC2_BASE + 0x0000086c)

static inline uint32_t rwnxagcevt5_get(void)
{
    return REG_PL_RD(RWNXAGCEVT5_ADDR);
}

static inline void rwnxagcevt5_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT5_ADDR, x);
}


static inline uint32_t rc2_evt5op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt5op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt5op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt5op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt5pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt5opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt5pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}


static inline uint32_t rc2_evt5tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt5tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT5_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT5_ADDR, localVal);
}

#define RWNXAGCEVT6_ADDR (BL_RC2_BASE + 0x00000870)

static inline uint32_t rwnxagcevt6_get(void)
{
    return REG_PL_RD(RWNXAGCEVT6_ADDR);
}

static inline void rwnxagcevt6_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT6_ADDR, x);
}


static inline uint32_t rc2_evt6op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt6op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt6op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt6op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt6pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt6opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt6pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}


static inline uint32_t rc2_evt6tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt6tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT6_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT6_ADDR, localVal);
}

#define RWNXAGCEVT7_ADDR (BL_RC2_BASE + 0x00000874)

static inline uint32_t rwnxagcevt7_get(void)
{
    return REG_PL_RD(RWNXAGCEVT7_ADDR);
}

static inline void rwnxagcevt7_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT7_ADDR, x);
}


static inline uint32_t rc2_evt7op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt7op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt7op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt7op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt7pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt7opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt7pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}


static inline uint32_t rc2_evt7tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt7tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT7_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT7_ADDR, localVal);
}

#define RWNXAGCEVT8_ADDR (BL_RC2_BASE + 0x00000878)

static inline uint32_t rwnxagcevt8_get(void)
{
    return REG_PL_RD(RWNXAGCEVT8_ADDR);
}

static inline void rwnxagcevt8_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT8_ADDR, x);
}


static inline uint32_t rc2_evt8op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt8op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt8op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt8op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt8pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt8opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt8pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}


static inline uint32_t rc2_evt8tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt8tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT8_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT8_ADDR, localVal);
}

#define RWNXAGCEVT9_ADDR (BL_RC2_BASE + 0x0000087c)

static inline uint32_t rwnxagcevt9_get(void)
{
    return REG_PL_RD(RWNXAGCEVT9_ADDR);
}

static inline void rwnxagcevt9_set(uint32_t x)
{
    REG_PL_WR(RWNXAGCEVT9_ADDR, x);
}


static inline uint32_t rc2_evt9op1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_evt9op1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9op2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_evt9op2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9op3_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_evt9op3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9pathcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x00002000)) >> 13);
}

static inline void rc2_evt9pathcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x00002000);
    localVal |= (x << 13) & ((uint32_t)0x00002000);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9opcomb_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_evt9opcomb_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9pgsel_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x00000200)) >> 9);
}

static inline void rc2_evt9pgsel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x00000200);
    localVal |= (x << 9) & ((uint32_t)0x00000200);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}


static inline uint32_t rc2_evt9tgtadd_getf(void)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    return ((localVal & ((uint32_t)0x000001ff)) >> 0);
}

static inline void rc2_evt9tgtadd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RWNXAGCEVT9_ADDR);
    localVal &= ~((uint32_t)0x000001ff);
    localVal |= (x << 0) & ((uint32_t)0x000001ff);
    REG_PL_WR(RWNXAGCEVT9_ADDR, localVal);
}

#define BLSINGLEANT_ADDR (BL_RC2_BASE + 0x00000880)

static inline uint32_t blsingleant_get(void)
{
    return REG_PL_RD(BLSINGLEANT_ADDR);
}

static inline void blsingleant_set(uint32_t x)
{
    REG_PL_WR(BLSINGLEANT_ADDR, x);
}


static inline uint32_t rc2_blcrrx0only_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_blcrrx0only_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_blcrrx1only_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rc2_blcrrx1only_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30) & ((uint32_t)0x40000000);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_blcr_sec40cca_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x20000000)) >> 29);
}

static inline void rc2_blcr_sec40cca_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x20000000);
    localVal |= (x << 29) & ((uint32_t)0x20000000);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_blcr_sec80cca_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rc2_blcr_sec80cca_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_blcrccamethod_sel_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rc2_blcrccamethod_sel_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27) & ((uint32_t)0x08000000);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_reg_ant1_rbb_wrong_bit_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rc2_reg_ant1_rbb_wrong_bit_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11) & ((uint32_t)0x00000800);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_reg_ant1_set_ant0_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x00000400)) >> 10);
}

static inline void rc2_reg_ant1_set_ant0_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x00000400);
    localVal |= (x << 10) & ((uint32_t)0x00000400);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}


static inline uint32_t rc2_fsmeventmask_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}

static inline void rc2_fsmeventmask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSINGLEANT_ADDR);
    localVal &= ~((uint32_t)0x000003ff);
    localVal |= (x << 0) & ((uint32_t)0x000003ff);
    REG_PL_WR(BLSINGLEANT_ADDR, localVal);
}

#define BLSPUR_0_ADDR (BL_RC2_BASE + 0x00000884)

static inline uint32_t blspur_0_get(void)
{
    return REG_PL_RD(BLSPUR_0_ADDR);
}

static inline void blspur_0_set(uint32_t x)
{
    REG_PL_WR(BLSPUR_0_ADDR, x);
}


static inline uint32_t rc2_blcr_spur_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void rc2_blcr_spur_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28) & ((uint32_t)0x30000000);
    REG_PL_WR(BLSPUR_0_ADDR, localVal);
}


static inline uint32_t rc2_blcr_spur_fc1_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    return ((localVal & ((uint32_t)0x00800000)) >> 23);
}

static inline void rc2_blcr_spur_fc1_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    localVal &= ~((uint32_t)0x00800000);
    localVal |= (x << 23) & ((uint32_t)0x00800000);
    REG_PL_WR(BLSPUR_0_ADDR, localVal);
}


static inline uint32_t rc2_blcr_spur_fc1_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    return ((localVal & ((uint32_t)0x007ff000)) >> 12);
}

static inline void rc2_blcr_spur_fc1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    localVal &= ~((uint32_t)0x007ff000);
    localVal |= (x << 12) & ((uint32_t)0x007ff000);
    REG_PL_WR(BLSPUR_0_ADDR, localVal);
}


static inline uint32_t rc2_blcr_spur_fc0_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rc2_blcr_spur_fc0_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11) & ((uint32_t)0x00000800);
    REG_PL_WR(BLSPUR_0_ADDR, localVal);
}


static inline uint32_t rc2_blcr_spur_fc0_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rc2_blcr_spur_fc0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR_0_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0) & ((uint32_t)0x000007ff);
    REG_PL_WR(BLSPUR_0_ADDR, localVal);
}

#define BLSPUR1_ADDR (BL_RC2_BASE + 0x00000888)

static inline uint32_t blspur1_get(void)
{
    return REG_PL_RD(BLSPUR1_ADDR);
}

static inline void blspur1_set(uint32_t x)
{
    REG_PL_WR(BLSPUR1_ADDR, x);
}


static inline uint32_t rc2_blcr_spur_fc2_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR1_ADDR);
    return ((localVal & ((uint32_t)0x00000800)) >> 11);
}

static inline void rc2_blcr_spur_fc2_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR1_ADDR);
    localVal &= ~((uint32_t)0x00000800);
    localVal |= (x << 11) & ((uint32_t)0x00000800);
    REG_PL_WR(BLSPUR1_ADDR, localVal);
}


static inline uint32_t rc2_blcr_spur_fc2_getf(void)
{
    uint32_t localVal = REG_PL_RD(BLSPUR1_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void rc2_blcr_spur_fc2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(BLSPUR1_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0) & ((uint32_t)0x000007ff);
    REG_PL_WR(BLSPUR1_ADDR, localVal);
}

#define AGCLOADREG4_ADDR (BL_RC2_BASE + 0x0000088c)

static inline uint32_t agcloadreg4_get(void)
{
    return REG_PL_RD(AGCLOADREG4_ADDR);
}

static inline void agcloadreg4_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG4_ADDR, x);
}


static inline uint32_t rc2_regrfgaincmd1316_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rc2_regrfgaincmd1316_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(AGCLOADREG4_ADDR, localVal);
}


static inline uint32_t rc2_regrfgainbound_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rc2_regrfgainbound_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24) & ((uint32_t)0x0f000000);
    REG_PL_WR(AGCLOADREG4_ADDR, localVal);
}


static inline uint32_t rc2_regdiggaininit_11b_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_regdiggaininit_11b_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG4_ADDR, localVal);
}


static inline uint32_t rc2_regdiggaininit_11g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_regdiggaininit_11g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG4_ADDR, localVal);
}


static inline uint32_t rc2_regdiggaininit_11n_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_regdiggaininit_11n_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG4_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG4_ADDR, localVal);
}

#define AGCLOADREG5_ADDR (BL_RC2_BASE + 0x00000890)

static inline uint32_t agcloadreg5_get(void)
{
    return REG_PL_RD(AGCLOADREG5_ADDR);
}

static inline void agcloadreg5_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG5_ADDR, x);
}


static inline uint32_t rc2_cfg_adcpowlna_gainadmode_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfg_adcpowlna_gainadmode_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpowlna_gainctrl_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rc2_cfg_adcpowlna_gainctrl_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30) & ((uint32_t)0x40000000);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd_en_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x3fc00000)) >> 22);
}

static inline void rc2_cfg_adcpow_lnagain_thd_en_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x3fc00000);
    localVal |= (x << 22) & ((uint32_t)0x3fc00000);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd0_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_cfg_adcpow_lnagain_thd0_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd1_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_cfg_adcpow_lnagain_thd1_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd2_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rc2_cfg_adcpow_lnagain_thd2_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG5_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2) & ((uint32_t)0x000000fc);
    REG_PL_WR(AGCLOADREG5_ADDR, localVal);
}

#define AGCLOADREG6_ADDR (BL_RC2_BASE + 0x00000894)

static inline uint32_t agcloadreg6_get(void)
{
    return REG_PL_RD(AGCLOADREG6_ADDR);
}

static inline void agcloadreg6_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG6_ADDR, x);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd3_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_cfg_adcpow_lnagain_thd3_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(AGCLOADREG6_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd4_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_cfg_adcpow_lnagain_thd4_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(AGCLOADREG6_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd5_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_cfg_adcpow_lnagain_thd5_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(AGCLOADREG6_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd6_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_cfg_adcpow_lnagain_thd6_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(AGCLOADREG6_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd7_2g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rc2_cfg_adcpow_lnagain_thd7_2g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG6_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2) & ((uint32_t)0x000000fc);
    REG_PL_WR(AGCLOADREG6_ADDR, localVal);
}

#define AGCLOADREG7_ADDR (BL_RC2_BASE + 0x00000898)

static inline uint32_t agcloadreg7_get(void)
{
    return REG_PL_RD(AGCLOADREG7_ADDR);
}

static inline void agcloadreg7_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG7_ADDR, x);
}


static inline uint32_t rc2_cfg_adcpowlna_gainadmode_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfg_adcpowlna_gainadmode_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpowlna_gainctrl_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x40000000)) >> 30);
}

static inline void rc2_cfg_adcpowlna_gainctrl_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x40000000);
    localVal |= (x << 30) & ((uint32_t)0x40000000);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd_en_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x3fc00000)) >> 22);
}

static inline void rc2_cfg_adcpow_lnagain_thd_en_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x3fc00000);
    localVal |= (x << 22) & ((uint32_t)0x3fc00000);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd0_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_cfg_adcpow_lnagain_thd0_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd1_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_cfg_adcpow_lnagain_thd1_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd2_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rc2_cfg_adcpow_lnagain_thd2_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG7_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2) & ((uint32_t)0x000000fc);
    REG_PL_WR(AGCLOADREG7_ADDR, localVal);
}

#define AGCLOADREG8_ADDR (BL_RC2_BASE + 0x0000089c)

static inline uint32_t agcloadreg8_get(void)
{
    return REG_PL_RD(AGCLOADREG8_ADDR);
}

static inline void agcloadreg8_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG8_ADDR, x);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd3_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    return ((localVal & ((uint32_t)0xfc000000)) >> 26);
}

static inline void rc2_cfg_adcpow_lnagain_thd3_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    localVal &= ~((uint32_t)0xfc000000);
    localVal |= (x << 26) & ((uint32_t)0xfc000000);
    REG_PL_WR(AGCLOADREG8_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd4_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    return ((localVal & ((uint32_t)0x03f00000)) >> 20);
}

static inline void rc2_cfg_adcpow_lnagain_thd4_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    localVal &= ~((uint32_t)0x03f00000);
    localVal |= (x << 20) & ((uint32_t)0x03f00000);
    REG_PL_WR(AGCLOADREG8_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd5_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    return ((localVal & ((uint32_t)0x000fc000)) >> 14);
}

static inline void rc2_cfg_adcpow_lnagain_thd5_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    localVal &= ~((uint32_t)0x000fc000);
    localVal |= (x << 14) & ((uint32_t)0x000fc000);
    REG_PL_WR(AGCLOADREG8_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd6_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    return ((localVal & ((uint32_t)0x00003f00)) >> 8);
}

static inline void rc2_cfg_adcpow_lnagain_thd6_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    localVal &= ~((uint32_t)0x00003f00);
    localVal |= (x << 8) & ((uint32_t)0x00003f00);
    REG_PL_WR(AGCLOADREG8_ADDR, localVal);
}


static inline uint32_t rc2_cfg_adcpow_lnagain_thd7_5g_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    return ((localVal & ((uint32_t)0x000000fc)) >> 2);
}

static inline void rc2_cfg_adcpow_lnagain_thd7_5g_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG8_ADDR);
    localVal &= ~((uint32_t)0x000000fc);
    localVal |= (x << 2) & ((uint32_t)0x000000fc);
    REG_PL_WR(AGCLOADREG8_ADDR, localVal);
}

#define AGCLOADREG9_ADDR (BL_RC2_BASE + 0x000008a0)

static inline uint32_t agcloadreg9_get(void)
{
    return REG_PL_RD(AGCLOADREG9_ADDR);
}

static inline void agcloadreg9_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG9_ADDR, x);
}


static inline uint32_t rc2_cfg_reflev_ofdmthd_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    return ((localVal & ((uint32_t)0x7ffff000)) >> 12);
}

static inline void rc2_cfg_reflev_ofdmthd_max_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    localVal &= ~((uint32_t)0x7ffff000);
    localVal |= (x << 12) & ((uint32_t)0x7ffff000);
    REG_PL_WR(AGCLOADREG9_ADDR, localVal);
}


static inline uint32_t rc2_cfg_reflev_ofdmthd_noise_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    return ((localVal & ((uint32_t)0x00000ff0)) >> 4);
}

static inline void rc2_cfg_reflev_ofdmthd_noise_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    localVal &= ~((uint32_t)0x00000ff0);
    localVal |= (x << 4) & ((uint32_t)0x00000ff0);
    REG_PL_WR(AGCLOADREG9_ADDR, localVal);
}


static inline uint32_t rc2_cfg_reflev_update_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rc2_cfg_reflev_update_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1) & ((uint32_t)0x00000002);
    REG_PL_WR(AGCLOADREG9_ADDR, localVal);
}


static inline uint32_t rc2_cfg_reflev_thd_bycr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rc2_cfg_reflev_thd_bycr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG9_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0) & ((uint32_t)0x00000001);
    REG_PL_WR(AGCLOADREG9_ADDR, localVal);
}

#define AGCLOADREG10_ADDR (BL_RC2_BASE + 0x000008a4)

static inline uint32_t agcloadreg10_get(void)
{
    return REG_PL_RD(AGCLOADREG10_ADDR);
}

static inline void agcloadreg10_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG10_ADDR, x);
}


static inline uint32_t rc2_cfg_reflev_ofdmthd_min_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG10_ADDR);
    return ((localVal & ((uint32_t)0x7ffff000)) >> 12);
}

static inline void rc2_cfg_reflev_ofdmthd_min_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG10_ADDR);
    localVal &= ~((uint32_t)0x7ffff000);
    localVal |= (x << 12) & ((uint32_t)0x7ffff000);
    REG_PL_WR(AGCLOADREG10_ADDR, localVal);
}

#define AGCLOADREG11_ADDR (BL_RC2_BASE + 0x000008a8)

static inline uint32_t agcloadreg11_get(void)
{
    return REG_PL_RD(AGCLOADREG11_ADDR);
}

static inline void agcloadreg11_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG11_ADDR, x);
}


static inline uint32_t rc2_cfg_reflev_dsssthd_max_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG11_ADDR);
    return ((localVal & ((uint32_t)0xfffff000)) >> 12);
}

static inline void rc2_cfg_reflev_dsssthd_max_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG11_ADDR);
    localVal &= ~((uint32_t)0xfffff000);
    localVal |= (x << 12) & ((uint32_t)0xfffff000);
    REG_PL_WR(AGCLOADREG11_ADDR, localVal);
}


static inline uint32_t rc2_cfg_reflev_dsssthd_noise_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG11_ADDR);
    return ((localVal & ((uint32_t)0x00000ff0)) >> 4);
}

static inline void rc2_cfg_reflev_dsssthd_noise_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG11_ADDR);
    localVal &= ~((uint32_t)0x00000ff0);
    localVal |= (x << 4) & ((uint32_t)0x00000ff0);
    REG_PL_WR(AGCLOADREG11_ADDR, localVal);
}

#define AGCLOADREG12_ADDR (BL_RC2_BASE + 0x000008ac)

static inline uint32_t agcloadreg12_get(void)
{
    return REG_PL_RD(AGCLOADREG12_ADDR);
}

static inline void agcloadreg12_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG12_ADDR, x);
}


static inline uint32_t rc2_cfg_reflev_dsssthd_min_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG12_ADDR);
    return ((localVal & ((uint32_t)0xfffff000)) >> 12);
}

static inline void rc2_cfg_reflev_dsssthd_min_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG12_ADDR);
    localVal &= ~((uint32_t)0xfffff000);
    localVal |= (x << 12) & ((uint32_t)0xfffff000);
    REG_PL_WR(AGCLOADREG12_ADDR, localVal);
}

#define AGCLOADREG13_ADDR (BL_RC2_BASE + 0x000008b0)

static inline uint32_t agcloadreg13_get(void)
{
    return REG_PL_RD(AGCLOADREG13_ADDR);
}

static inline void agcloadreg13_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG13_ADDR, x);
}


static inline uint32_t rc2_ro_reflev_ofdmthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG13_ADDR);
    return ((localVal & ((uint32_t)0x7ffff000)) >> 12);
}

static inline void rc2_ro_reflev_ofdmthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG13_ADDR);
    localVal &= ~((uint32_t)0x7ffff000);
    localVal |= (x << 12) & ((uint32_t)0x7ffff000);
    REG_PL_WR(AGCLOADREG13_ADDR, localVal);
}


static inline uint32_t rc2_ro_reflev_vld_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG13_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rc2_ro_reflev_vld_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG13_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0) & ((uint32_t)0x00000001);
    REG_PL_WR(AGCLOADREG13_ADDR, localVal);
}

#define AGCLOADREG14_ADDR (BL_RC2_BASE + 0x000008b4)

static inline uint32_t agcloadreg14_get(void)
{
    return REG_PL_RD(AGCLOADREG14_ADDR);
}

static inline void agcloadreg14_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG14_ADDR, x);
}


static inline uint32_t rc2_ro_reflev_dsssthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG14_ADDR);
    return ((localVal & ((uint32_t)0xfffff000)) >> 12);
}

static inline void rc2_ro_reflev_dsssthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG14_ADDR);
    localVal &= ~((uint32_t)0xfffff000);
    localVal |= (x << 12) & ((uint32_t)0xfffff000);
    REG_PL_WR(AGCLOADREG14_ADDR, localVal);
}

#define AGCLOADREG15_ADDR (BL_RC2_BASE + 0x000008b8)

static inline uint32_t agcloadreg15_get(void)
{
    return REG_PL_RD(AGCLOADREG15_ADDR);
}

static inline void agcloadreg15_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG15_ADDR, x);
}


static inline uint32_t rc2_regbw40adcmargsb20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rc2_regbw40adcmargsb20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28) & ((uint32_t)0xf0000000);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_regbw80adcmargsb20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void rc2_regbw80adcmargsb20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24) & ((uint32_t)0x0f000000);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_regbw80adcmargsb40_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x00f00000)) >> 20);
}

static inline void rc2_regbw80adcmargsb40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x00f00000);
    localVal |= (x << 20) & ((uint32_t)0x00f00000);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_regbw40inbdmargsb20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x000f0000)) >> 16);
}

static inline void rc2_regbw40inbdmargsb20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x000f0000);
    localVal |= (x << 16) & ((uint32_t)0x000f0000);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_regbw80inbdmargsb20_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline void rc2_regbw80inbdmargsb20_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x0000f000);
    localVal |= (x << 12) & ((uint32_t)0x0000f000);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_regbw80inbdmargsb40_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline void rc2_regbw80inbdmargsb40_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x00000f00);
    localVal |= (x << 8) & ((uint32_t)0x00000f00);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_cfg_rampdown_method_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void rc2_cfg_rampdown_method_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4) & ((uint32_t)0x00000010);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}


static inline uint32_t rc2_agcregdetrampdownthr_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    return ((localVal & ((uint32_t)0x0000000f)) >> 0);
}

static inline void rc2_agcregdetrampdownthr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG15_ADDR);
    localVal &= ~((uint32_t)0x0000000f);
    localVal |= (x << 0) & ((uint32_t)0x0000000f);
    REG_PL_WR(AGCLOADREG15_ADDR, localVal);
}

#define AGCLOADREG16_ADDR (BL_RC2_BASE + 0x000008bc)

static inline uint32_t agcloadreg16_get(void)
{
    return REG_PL_RD(AGCLOADREG16_ADDR);
}

static inline void agcloadreg16_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG16_ADDR, x);
}


static inline uint32_t rc2_agcregdetrampdndet_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_agcregdetrampdndet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG16_ADDR, localVal);
}


static inline uint32_t rc2_agcregdetrampdnsat_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_agcregdetrampdnsat_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG16_ADDR, localVal);
}


static inline uint32_t rc2_agcregdetrampdnpkdet_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_agcregdetrampdnpkdet_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG16_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG16_ADDR, localVal);
}

#define AGCLOADREG17_ADDR (BL_RC2_BASE + 0x000008c0)

static inline uint32_t agcloadreg17_get(void)
{
    return REG_PL_RD(AGCLOADREG17_ADDR);
}

static inline void agcloadreg17_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG17_ADDR, x);
}


static inline uint32_t rc2_agcregdelrddetpotap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x70000000)) >> 28);
}

static inline void rc2_agcregdelrddetpotap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x70000000);
    localVal |= (x << 28) & ((uint32_t)0x70000000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrddetprtap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x0e000000)) >> 25);
}

static inline void rc2_agcregdelrddetprtap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x0e000000);
    localVal |= (x << 25) & ((uint32_t)0x0e000000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrddettstap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x01c00000)) >> 22);
}

static inline void rc2_agcregdelrddettstap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x01c00000);
    localVal |= (x << 22) & ((uint32_t)0x01c00000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdsatpotap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00380000)) >> 19);
}

static inline void rc2_agcregdelrdsatpotap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00380000);
    localVal |= (x << 19) & ((uint32_t)0x00380000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdsatprtap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00070000)) >> 16);
}

static inline void rc2_agcregdelrdsatprtap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00070000);
    localVal |= (x << 16) & ((uint32_t)0x00070000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdsattstap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x0000e000)) >> 13);
}

static inline void rc2_agcregdelrdsattstap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x0000e000);
    localVal |= (x << 13) & ((uint32_t)0x0000e000);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdpkdetpotap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00001c00)) >> 10);
}

static inline void rc2_agcregdelrdpkdetpotap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00001c00);
    localVal |= (x << 10) & ((uint32_t)0x00001c00);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdpkdetprtap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00000380)) >> 7);
}

static inline void rc2_agcregdelrdpkdetprtap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00000380);
    localVal |= (x << 7) & ((uint32_t)0x00000380);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregdelrdpkdettstap_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00000070)) >> 4);
}

static inline void rc2_agcregdelrdpkdettstap_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00000070);
    localVal |= (x << 4) & ((uint32_t)0x00000070);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregrddettsen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rc2_agcregrddettsen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2) & ((uint32_t)0x00000004);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregrdsattsen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rc2_agcregrdsattsen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1) & ((uint32_t)0x00000002);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}


static inline uint32_t rc2_agcregrdpkdettsen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rc2_agcregrdpkdettsen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG17_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0) & ((uint32_t)0x00000001);
    REG_PL_WR(AGCLOADREG17_ADDR, localVal);
}

#define AGCLOADREG18_ADDR (BL_RC2_BASE + 0x000008c4)

static inline uint32_t agcloadreg18_get(void)
{
    return REG_PL_RD(AGCLOADREG18_ADDR);
}

static inline void agcloadreg18_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG18_ADDR, x);
}


static inline uint32_t rc2_cfgtimeouten1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfgtimeouten1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCLOADREG18_ADDR, localVal);
}


static inline uint32_t rc2_cfgtimeoutval1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    return ((localVal & ((uint32_t)0x7ff00000)) >> 20);
}

static inline void rc2_cfgtimeoutval1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    localVal &= ~((uint32_t)0x7ff00000);
    localVal |= (x << 20) & ((uint32_t)0x7ff00000);
    REG_PL_WR(AGCLOADREG18_ADDR, localVal);
}


static inline uint32_t rc2_cfgagcmemaddr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    return ((localVal & ((uint32_t)0x000ff800)) >> 11);
}

static inline void rc2_cfgagcmemaddr1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG18_ADDR);
    localVal &= ~((uint32_t)0x000ff800);
    localVal |= (x << 11) & ((uint32_t)0x000ff800);
    REG_PL_WR(AGCLOADREG18_ADDR, localVal);
}

#define AGCLOADREG19_ADDR (BL_RC2_BASE + 0x000008c8)

static inline uint32_t agcloadreg19_get(void)
{
    return REG_PL_RD(AGCLOADREG19_ADDR);
}

static inline void agcloadreg19_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG19_ADDR, x);
}


static inline uint32_t rc2_cfgtimeouten2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfgtimeouten2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCLOADREG19_ADDR, localVal);
}


static inline uint32_t rc2_cfgtimeoutval2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    return ((localVal & ((uint32_t)0x7ff00000)) >> 20);
}

static inline void rc2_cfgtimeoutval2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    localVal &= ~((uint32_t)0x7ff00000);
    localVal |= (x << 20) & ((uint32_t)0x7ff00000);
    REG_PL_WR(AGCLOADREG19_ADDR, localVal);
}


static inline uint32_t rc2_cfgagcmemaddr2_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    return ((localVal & ((uint32_t)0x000ff800)) >> 11);
}

static inline void rc2_cfgagcmemaddr2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG19_ADDR);
    localVal &= ~((uint32_t)0x000ff800);
    localVal |= (x << 11) & ((uint32_t)0x000ff800);
    REG_PL_WR(AGCLOADREG19_ADDR, localVal);
}

#define AGCLOADREG20_ADDR (BL_RC2_BASE + 0x000008cc)

static inline uint32_t agcloadreg20_get(void)
{
    return REG_PL_RD(AGCLOADREG20_ADDR);
}

static inline void agcloadreg20_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG20_ADDR, x);
}


static inline uint32_t rc2_cfgtimeouten3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    return ((localVal & ((uint32_t)0x80000000)) >> 31);
}

static inline void rc2_cfgtimeouten3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    localVal &= ~((uint32_t)0x80000000);
    localVal |= (x << 31) & ((uint32_t)0x80000000);
    REG_PL_WR(AGCLOADREG20_ADDR, localVal);
}


static inline uint32_t rc2_cfgtimeoutval3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    return ((localVal & ((uint32_t)0x7ff00000)) >> 20);
}

static inline void rc2_cfgtimeoutval3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    localVal &= ~((uint32_t)0x7ff00000);
    localVal |= (x << 20) & ((uint32_t)0x7ff00000);
    REG_PL_WR(AGCLOADREG20_ADDR, localVal);
}


static inline uint32_t rc2_cfgagcmemaddr3_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    return ((localVal & ((uint32_t)0x000ff800)) >> 11);
}

static inline void rc2_cfgagcmemaddr3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG20_ADDR);
    localVal &= ~((uint32_t)0x000ff800);
    localVal |= (x << 11) & ((uint32_t)0x000ff800);
    REG_PL_WR(AGCLOADREG20_ADDR, localVal);
}

#define AGCLOADREG21_ADDR (BL_RC2_BASE + 0x000008d0)

static inline uint32_t agcloadreg21_get(void)
{
    return REG_PL_RD(AGCLOADREG21_ADDR);
}

static inline void agcloadreg21_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG21_ADDR, x);
}


static inline uint32_t rc2_useacifilterstf_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x10000000)) >> 28);
}

static inline void rc2_useacifilterstf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x10000000);
    localVal |= (x << 28) & ((uint32_t)0x10000000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_useacifilterltf_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x08000000)) >> 27);
}

static inline void rc2_useacifilterltf_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x08000000);
    localVal |= (x << 27) & ((uint32_t)0x08000000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgacidetlstfpowen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x04000000)) >> 26);
}

static inline void rc2_cfgacidetlstfpowen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x04000000);
    localVal |= (x << 26) & ((uint32_t)0x04000000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetadcpowen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x02000000)) >> 25);
}

static inline void rc2_cfgpopdetadcpowen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x02000000);
    localVal |= (x << 25) & ((uint32_t)0x02000000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetinbdpowen_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x01000000)) >> 24);
}

static inline void rc2_cfgpopdetinbdpowen_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x01000000);
    localVal |= (x << 24) & ((uint32_t)0x01000000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgacidetlstfpowthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_cfgacidetlstfpowthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetadcpowthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_cfgpopdetadcpowthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetinbdpowthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_cfgpopdetinbdpowthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG21_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG21_ADDR, localVal);
}

#define AGCLOADREG22_ADDR (BL_RC2_BASE + 0x000008d4)

static inline uint32_t agcloadreg22_get(void)
{
    return REG_PL_RD(AGCLOADREG22_ADDR);
}

static inline void agcloadreg22_set(uint32_t x)
{
    REG_PL_WR(AGCLOADREG22_ADDR, x);
}


static inline uint32_t rc2_cfgpopdetadcpowhgdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_cfgpopdetadcpowhgdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(AGCLOADREG22_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetinbdpowhgdbm_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_cfgpopdetinbdpowhgdbm_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(AGCLOADREG22_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetadcpowhgthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_cfgpopdetadcpowhgthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(AGCLOADREG22_ADDR, localVal);
}


static inline uint32_t rc2_cfgpopdetinbdpowhgthd_getf(void)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_cfgpopdetinbdpowhgthd_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(AGCLOADREG22_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(AGCLOADREG22_ADDR, localVal);
}

#define RC2_STATUS_0_ADDR (BL_RC2_BASE + 0x00000900)

static inline uint32_t rc2_status_0_get(void)
{
    return REG_PL_RD(RC2_STATUS_0_ADDR);
}

static inline void rc2_status_0_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_0_ADDR, x);
}


static inline uint32_t rc2_sts_tx0_txg_cfg_idx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0xf0000000)) >> 28);
}

static inline void rc2_sts_tx0_txg_cfg_idx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    localVal &= ~((uint32_t)0xf0000000);
    localVal |= (x << 28) & ((uint32_t)0xf0000000);
    REG_PL_WR(RC2_STATUS_0_ADDR, localVal);
}


static inline uint32_t rc2_sts_tx0_txg_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_sts_tx0_txg_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RC2_STATUS_0_ADDR, localVal);
}


static inline uint32_t rc2_sts_tx0_tbbd_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x00007000)) >> 12);
}

static inline void rc2_sts_tx0_tbbd_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    localVal &= ~((uint32_t)0x00007000);
    localVal |= (x << 12) & ((uint32_t)0x00007000);
    REG_PL_WR(RC2_STATUS_0_ADDR, localVal);
}


static inline uint32_t rc2_sts_agc_gain_target0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}

static inline void rc2_sts_agc_gain_target0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_0_ADDR);
    localVal &= ~((uint32_t)0x000000ff);
    localVal |= (x << 0) & ((uint32_t)0x000000ff);
    REG_PL_WR(RC2_STATUS_0_ADDR, localVal);
}

#define RC2_STATUS_1_ADDR (BL_RC2_BASE + 0x00000904)

static inline uint32_t rc2_status_1_get(void)
{
    return REG_PL_RD(RC2_STATUS_1_ADDR);
}

static inline void rc2_status_1_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_1_ADDR, x);
}


static inline uint32_t rc2_sts_rx0_lna_gain_idx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_sts_rx0_lna_gain_idx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_rbb_cfg_idx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x001f0000)) >> 16);
}

static inline void rc2_sts_rx0_rbb_cfg_idx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x001f0000);
    localVal |= (x << 16) & ((uint32_t)0x001f0000);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_nf_gain_idx_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x0000fe00)) >> 9);
}

static inline void rc2_sts_rx0_nf_gain_idx_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x0000fe00);
    localVal |= (x << 9) & ((uint32_t)0x0000fe00);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_lna_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x000001c0)) >> 6);
}

static inline void rc2_sts_rx0_lna_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x000001c0);
    localVal |= (x << 6) & ((uint32_t)0x000001c0);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_gm_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void rc2_sts_rx0_gm_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5) & ((uint32_t)0x00000020);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_rbb1_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000018)) >> 3);
}

static inline void rc2_sts_rx0_rbb1_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x00000018);
    localVal |= (x << 3) & ((uint32_t)0x00000018);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_rbb2_cfg_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    return ((localVal & ((uint32_t)0x00000007)) >> 0);
}

static inline void rc2_sts_rx0_rbb2_cfg_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_1_ADDR);
    localVal &= ~((uint32_t)0x00000007);
    localVal |= (x << 0) & ((uint32_t)0x00000007);
    REG_PL_WR(RC2_STATUS_1_ADDR, localVal);
}

#define RC2_STATUS_2_ADDR (BL_RC2_BASE + 0x00000908)

static inline uint32_t rc2_status_2_get(void)
{
    return REG_PL_RD(RC2_STATUS_2_ADDR);
}

static inline void rc2_status_2_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_2_ADDR, x);
}


static inline uint32_t rc2_sts_rx0_lna_gain_db_s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0xff000000)) >> 24);
}

static inline void rc2_sts_rx0_lna_gain_db_s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    localVal &= ~((uint32_t)0xff000000);
    localVal |= (x << 24) & ((uint32_t)0xff000000);
    REG_PL_WR(RC2_STATUS_2_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_vga_gain_db_s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x00ff0000)) >> 16);
}

static inline void rc2_sts_rx0_vga_gain_db_s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    localVal &= ~((uint32_t)0x00ff0000);
    localVal |= (x << 16) & ((uint32_t)0x00ff0000);
    REG_PL_WR(RC2_STATUS_2_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_total_gain_db_s_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x0000ff00)) >> 8);
}

static inline void rc2_sts_rx0_total_gain_db_s_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    localVal &= ~((uint32_t)0x0000ff00);
    localVal |= (x << 8) & ((uint32_t)0x0000ff00);
    REG_PL_WR(RC2_STATUS_2_ADDR, localVal);
}


static inline uint32_t rc2_sts_rx0_nf_gain_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    return ((localVal & ((uint32_t)0x0000003f)) >> 0);
}

static inline void rc2_sts_rx0_nf_gain_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_2_ADDR);
    localVal &= ~((uint32_t)0x0000003f);
    localVal |= (x << 0) & ((uint32_t)0x0000003f);
    REG_PL_WR(RC2_STATUS_2_ADDR, localVal);
}

#define RC2_STATUS_3_ADDR (BL_RC2_BASE + 0x0000090c)

static inline uint32_t rc2_status_3_get(void)
{
    return REG_PL_RD(RC2_STATUS_3_ADDR);
}

static inline void rc2_status_3_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_3_ADDR, x);
}


static inline uint32_t rc2_sts_rw_nx_diag0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_rw_nx_diag0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_3_ADDR, localVal);
}

#define RC2_STATUS_4_ADDR (BL_RC2_BASE + 0x00000910)

static inline uint32_t rc2_status_4_get(void)
{
    return REG_PL_RD(RC2_STATUS_4_ADDR);
}

static inline void rc2_status_4_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_4_ADDR, x);
}


static inline uint32_t rc2_sts_rw_nx_diag1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_rw_nx_diag1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_4_ADDR, localVal);
}

#define RC2_STATUS_5_ADDR (BL_RC2_BASE + 0x00000914)

static inline uint32_t rc2_status_5_get(void)
{
    return REG_PL_RD(RC2_STATUS_5_ADDR);
}

static inline void rc2_status_5_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_5_ADDR, x);
}


static inline uint32_t rc2_sts_rw_nx_diag2_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_rw_nx_diag2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_5_ADDR, localVal);
}

#define RC2_STATUS_6_ADDR (BL_RC2_BASE + 0x00000918)

static inline uint32_t rc2_status_6_get(void)
{
    return REG_PL_RD(RC2_STATUS_6_ADDR);
}

static inline void rc2_status_6_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_6_ADDR, x);
}


static inline uint32_t rc2_sts_diag_phy0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_diag_phy0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_6_ADDR, localVal);
}

#define RC2_STATUS_7_ADDR (BL_RC2_BASE + 0x0000091c)

static inline uint32_t rc2_status_7_get(void)
{
    return REG_PL_RD(RC2_STATUS_7_ADDR);
}

static inline void rc2_status_7_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_7_ADDR, x);
}


static inline uint32_t rc2_sts_diag_phy1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_7_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_diag_phy1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_7_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_7_ADDR, localVal);
}

#define RC2_STATUS_8_ADDR (BL_RC2_BASE + 0x00000920)

static inline uint32_t rc2_status_8_get(void)
{
    return REG_PL_RD(RC2_STATUS_8_ADDR);
}

static inline void rc2_status_8_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_8_ADDR, x);
}


static inline uint32_t rc2_sts_diag_riu0_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_8_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_diag_riu0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_8_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_8_ADDR, localVal);
}

#define RC2_STATUS_9_ADDR (BL_RC2_BASE + 0x00000924)

static inline uint32_t rc2_status_9_get(void)
{
    return REG_PL_RD(RC2_STATUS_9_ADDR);
}

static inline void rc2_status_9_set(uint32_t x)
{
    REG_PL_WR(RC2_STATUS_9_ADDR, x);
}


static inline uint32_t rc2_sts_diag_riu1_getf(void)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_9_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void rc2_sts_diag_riu1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RC2_STATUS_9_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0) & ((uint32_t)0xffffffff);
    REG_PL_WR(RC2_STATUS_9_ADDR, localVal);
}

#endif // _REG_BL_RC2_H_
