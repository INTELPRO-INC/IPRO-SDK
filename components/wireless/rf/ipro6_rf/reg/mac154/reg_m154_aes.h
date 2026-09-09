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

#ifndef _REG_M154_AES_H_
#define _REG_M154_AES_H_
// Date: 20210607

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef M154_AES_BASE
#define M154_AES_BASE (0x20000000)
#endif



#define M154_AES_ACR_ADDR (M154_AES_BASE + 0x00000800)

static inline uint32_t m154_aes_acr_get(void)
{
    return REG_PL_RD(M154_AES_ACR_ADDR);
}

static inline void m154_aes_acr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ACR_ADDR,x);
}

static inline uint32_t m154_aes_reserved_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0xfff00000)) >> 20);
}

static inline uint32_t m154_aes_sw_rst_mst_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline void m154_aes_sw_rst_mst_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    localVal &= ~((uint32_t)0x00020000);
    localVal |= (x << 17)&((uint32_t)0x00020000);
    REG_PL_WR(M154_AES_ACR_ADDR,localVal);
}

static inline uint32_t m154_aes_sw_rst_aes_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline void m154_aes_sw_rst_aes_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    localVal &= ~((uint32_t)0x00010000);
    localVal |= (x << 16)&((uint32_t)0x00010000);
    REG_PL_WR(M154_AES_ACR_ADDR,localVal);
}

static inline uint32_t m154_aes_rxkey_rdy_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0x00000100)) >> 8);
}

static inline void m154_aes_rxkey_rdy_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    localVal &= ~((uint32_t)0x00000100);
    localVal |= (x << 8)&((uint32_t)0x00000100);
    REG_PL_WR(M154_AES_ACR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_dma_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void m154_aes_aes_dma_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(M154_AES_ACR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesdma_trig_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_aes_aesdma_trig_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ACR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_AES_ACR_ADDR,localVal);
}


#define M154_AES_AMR_ADDR (M154_AES_BASE + 0x00000804)

static inline uint32_t m154_aes_amr_get(void)
{
    return REG_PL_RD(M154_AES_AMR_ADDR);
}

static inline void m154_aes_amr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMR_ADDR,x);
}

static inline uint32_t m154_aes_aes_key_size_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x000000c0)) >> 6);
}

static inline void m154_aes_aes_key_size_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x000000c0);
    localVal |= (x << 6)&((uint32_t)0x000000c0);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_mode_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x00000030)) >> 4);
}

static inline void m154_aes_aes_mode_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x00000030);
    localVal |= (x << 4)&((uint32_t)0x00000030);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}

static inline uint32_t m154_aes_ahb_bytewr_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_aes_ahb_bytewr_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}

static inline uint32_t m154_aes_rxdec_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_aes_rxdec_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}

static inline uint32_t m154_aes_rxdma_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_aes_rxdma_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_enable_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_aes_aes_enable_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_AES_AMR_ADDR,localVal);
}


#define M154_AES_ALR_ADDR (M154_AES_BASE + 0x00000808)

static inline uint32_t m154_aes_alr_get(void)
{
    return REG_PL_RD(M154_AES_ALR_ADDR);
}

static inline void m154_aes_alr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ALR_ADDR,x);
}

static inline uint32_t m154_aes_aes_mic_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline void m154_aes_aes_mic_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    localVal &= ~((uint32_t)0x30000000);
    localVal |= (x << 28)&((uint32_t)0x30000000);
    REG_PL_WR(M154_AES_ALR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_nonce_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    return ((localVal & ((uint32_t)0x0f000000)) >> 24);
}

static inline void m154_aes_aes_nonce_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    localVal &= ~((uint32_t)0x0f000000);
    localVal |= (x << 24)&((uint32_t)0x0f000000);
    REG_PL_WR(M154_AES_ALR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_a_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    return ((localVal & ((uint32_t)0x007ff000)) >> 12);
}

static inline void m154_aes_aes_a_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    localVal &= ~((uint32_t)0x007ff000);
    localVal |= (x << 12)&((uint32_t)0x007ff000);
    REG_PL_WR(M154_AES_ALR_ADDR,localVal);
}

static inline uint32_t m154_aes_aes_m_len_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    return ((localVal & ((uint32_t)0x000007ff)) >> 0);
}

static inline void m154_aes_aes_m_len_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ALR_ADDR);
    localVal &= ~((uint32_t)0x000007ff);
    localVal |= (x << 0)&((uint32_t)0x000007ff);
    REG_PL_WR(M154_AES_ALR_ADDR,localVal);
}


#define M154_AES_RXALR_ADDR (M154_AES_BASE + 0x0000080c)

static inline uint32_t m154_aes_rxalr_get(void)
{
    return REG_PL_RD(M154_AES_RXALR_ADDR);
}

static inline void m154_aes_rxalr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_RXALR_ADDR,x);
}

static inline uint32_t m154_aes_rx_mic_len_r_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXALR_ADDR);
    return ((localVal & ((uint32_t)0x30000000)) >> 28);
}

static inline uint32_t m154_aes_rx_a_len_r_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXALR_ADDR);
    return ((localVal & ((uint32_t)0x000ff000)) >> 12);
}

static inline uint32_t m154_aes_rx_m_len_r_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXALR_ADDR);
    return ((localVal & ((uint32_t)0x000000ff)) >> 0);
}


#define M154_AES_AASAR_ADDR (M154_AES_BASE + 0x00000810)

static inline uint32_t m154_aes_aasar_get(void)
{
    return REG_PL_RD(M154_AES_AASAR_ADDR);
}

static inline void m154_aes_aasar_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AASAR_ADDR,x);
}

static inline uint32_t m154_aes_aes_asa_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AASAR_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_aes_asa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AASAR_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AASAR_ADDR,localVal);
}


#define M154_AES_AMSAR_ADDR (M154_AES_BASE + 0x00000814)

static inline uint32_t m154_aes_amsar_get(void)
{
    return REG_PL_RD(M154_AES_AMSAR_ADDR);
}

static inline void m154_aes_amsar_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMSAR_ADDR,x);
}

static inline uint32_t m154_aes_aes_msa_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMSAR_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_aes_msa_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMSAR_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AMSAR_ADDR,localVal);
}


#define M154_AES_AMDAR_ADDR (M154_AES_BASE + 0x00000818)

static inline uint32_t m154_aes_amdar_get(void)
{
    return REG_PL_RD(M154_AES_AMDAR_ADDR);
}

static inline void m154_aes_amdar_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMDAR_ADDR,x);
}

static inline uint32_t m154_aes_aes_mda_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMDAR_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_aes_mda_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMDAR_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AMDAR_ADDR,localVal);
}


#define M154_AES_RXAADAR_ADDR (M154_AES_BASE + 0x0000081c)

static inline uint32_t m154_aes_rxaadar_get(void)
{
    return REG_PL_RD(M154_AES_RXAADAR_ADDR);
}

static inline void m154_aes_rxaadar_set(uint32_t x)
{
    REG_PL_WR(M154_AES_RXAADAR_ADDR,x);
}

static inline uint32_t m154_aes_aes_rxada_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXAADAR_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_aes_rxada_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXAADAR_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_RXAADAR_ADDR,localVal);
}


#define M154_AES_RXAMDAR_ADDR (M154_AES_BASE + 0x00000820)

static inline uint32_t m154_aes_rxamdar_get(void)
{
    return REG_PL_RD(M154_AES_RXAMDAR_ADDR);
}

static inline void m154_aes_rxamdar_set(uint32_t x)
{
    REG_PL_WR(M154_AES_RXAMDAR_ADDR,x);
}

static inline uint32_t m154_aes_aes_rxmda_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXAMDAR_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_aes_rxmda_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_RXAMDAR_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_RXAMDAR_ADDR,localVal);
}


#define M154_AES_ANR0_ADDR (M154_AES_BASE + 0x00000830)

static inline uint32_t m154_aes_anr0_get(void)
{
    return REG_PL_RD(M154_AES_ANR0_ADDR);
}

static inline void m154_aes_anr0_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ANR0_ADDR,x);
}

static inline uint32_t m154_aes_anr0_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_anr0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_ANR0_ADDR,localVal);
}


#define M154_AES_ANR1_ADDR (M154_AES_BASE + 0x00000834)

static inline uint32_t m154_aes_anr1_get(void)
{
    return REG_PL_RD(M154_AES_ANR1_ADDR);
}

static inline void m154_aes_anr1_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ANR1_ADDR,x);
}

static inline uint32_t m154_aes_anr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_anr1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_ANR1_ADDR,localVal);
}


#define M154_AES_ANR2_ADDR (M154_AES_BASE + 0x00000838)

static inline uint32_t m154_aes_anr2_get(void)
{
    return REG_PL_RD(M154_AES_ANR2_ADDR);
}

static inline void m154_aes_anr2_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ANR2_ADDR,x);
}

static inline uint32_t m154_aes_anr2_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_anr2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_ANR2_ADDR,localVal);
}


#define M154_AES_ANR3_ADDR (M154_AES_BASE + 0x0000083c)

static inline uint32_t m154_aes_anr3_get(void)
{
    return REG_PL_RD(M154_AES_ANR3_ADDR);
}

static inline void m154_aes_anr3_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ANR3_ADDR,x);
}

static inline uint32_t m154_aes_anr3_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR3_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}

static inline void m154_aes_anr3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ANR3_ADDR);
    localVal &= ~((uint32_t)0x0000ffff);
    localVal |= (x << 0)&((uint32_t)0x0000ffff);
    REG_PL_WR(M154_AES_ANR3_ADDR,localVal);
}


#define M154_AES_AKR0_ADDR (M154_AES_BASE + 0x00000840)

static inline uint32_t m154_aes_akr0_get(void)
{
    return REG_PL_RD(M154_AES_AKR0_ADDR);
}

static inline void m154_aes_akr0_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR0_ADDR,x);
}

static inline uint32_t m154_aes_akr0_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr0_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR0_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR0_ADDR,localVal);
}


#define M154_AES_AKR1_ADDR (M154_AES_BASE + 0x00000844)

static inline uint32_t m154_aes_akr1_get(void)
{
    return REG_PL_RD(M154_AES_AKR1_ADDR);
}

static inline void m154_aes_akr1_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR1_ADDR,x);
}

static inline uint32_t m154_aes_akr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr1_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR1_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR1_ADDR,localVal);
}


#define M154_AES_AKR2_ADDR (M154_AES_BASE + 0x00000848)

static inline uint32_t m154_aes_akr2_get(void)
{
    return REG_PL_RD(M154_AES_AKR2_ADDR);
}

static inline void m154_aes_akr2_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR2_ADDR,x);
}

static inline uint32_t m154_aes_akr2_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr2_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR2_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR2_ADDR,localVal);
}


#define M154_AES_AKR3_ADDR (M154_AES_BASE + 0x0000084c)

static inline uint32_t m154_aes_akr3_get(void)
{
    return REG_PL_RD(M154_AES_AKR3_ADDR);
}

static inline void m154_aes_akr3_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR3_ADDR,x);
}

static inline uint32_t m154_aes_akr3_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr3_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR3_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR3_ADDR,localVal);
}


#define M154_AES_AKR4_ADDR (M154_AES_BASE + 0x00000850)

static inline uint32_t m154_aes_akr4_get(void)
{
    return REG_PL_RD(M154_AES_AKR4_ADDR);
}

static inline void m154_aes_akr4_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR4_ADDR,x);
}

static inline uint32_t m154_aes_akr4_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR4_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr4_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR4_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR4_ADDR,localVal);
}


#define M154_AES_AKR5_ADDR (M154_AES_BASE + 0x00000854)

static inline uint32_t m154_aes_akr5_get(void)
{
    return REG_PL_RD(M154_AES_AKR5_ADDR);
}

static inline void m154_aes_akr5_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR5_ADDR,x);
}

static inline uint32_t m154_aes_akr5_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR5_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr5_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR5_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR5_ADDR,localVal);
}


#define M154_AES_AKR6_ADDR (M154_AES_BASE + 0x00000858)

static inline uint32_t m154_aes_akr6_get(void)
{
    return REG_PL_RD(M154_AES_AKR6_ADDR);
}

static inline void m154_aes_akr6_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR6_ADDR,x);
}

static inline uint32_t m154_aes_akr6_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR6_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr6_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR6_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR6_ADDR,localVal);
}


#define M154_AES_AKR7_ADDR (M154_AES_BASE + 0x0000085c)

static inline uint32_t m154_aes_akr7_get(void)
{
    return REG_PL_RD(M154_AES_AKR7_ADDR);
}

static inline void m154_aes_akr7_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AKR7_ADDR,x);
}

static inline uint32_t m154_aes_akr7_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR7_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}

static inline void m154_aes_akr7_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AKR7_ADDR);
    localVal &= ~((uint32_t)0xffffffff);
    localVal |= (x << 0)&((uint32_t)0xffffffff);
    REG_PL_WR(M154_AES_AKR7_ADDR,localVal);
}


#define M154_AES_AMICR0_ADDR (M154_AES_BASE + 0x00000860)

static inline uint32_t m154_aes_amicr0_get(void)
{
    return REG_PL_RD(M154_AES_AMICR0_ADDR);
}

static inline void m154_aes_amicr0_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMICR0_ADDR,x);
}

static inline uint32_t m154_aes_amicr0_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMICR0_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_AES_AMICR1_ADDR (M154_AES_BASE + 0x00000864)

static inline uint32_t m154_aes_amicr1_get(void)
{
    return REG_PL_RD(M154_AES_AMICR1_ADDR);
}

static inline void m154_aes_amicr1_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMICR1_ADDR,x);
}

static inline uint32_t m154_aes_amicr1_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMICR1_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_AES_AMICR2_ADDR (M154_AES_BASE + 0x00000868)

static inline uint32_t m154_aes_amicr2_get(void)
{
    return REG_PL_RD(M154_AES_AMICR2_ADDR);
}

static inline void m154_aes_amicr2_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMICR2_ADDR,x);
}

static inline uint32_t m154_aes_amicr2_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMICR2_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_AES_AMICR3_ADDR (M154_AES_BASE + 0x0000086c)

static inline uint32_t m154_aes_amicr3_get(void)
{
    return REG_PL_RD(M154_AES_AMICR3_ADDR);
}

static inline void m154_aes_amicr3_set(uint32_t x)
{
    REG_PL_WR(M154_AES_AMICR3_ADDR,x);
}

static inline uint32_t m154_aes_amicr3_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_AMICR3_ADDR);
    return ((localVal & ((uint32_t)0xffffffff)) >> 0);
}


#define M154_AES_STATUS1_ADDR (M154_AES_BASE + 0x00000870)

static inline uint32_t m154_aes_status1_get(void)
{
    return REG_PL_RD(M154_AES_STATUS1_ADDR);
}

static inline void m154_aes_status1_set(uint32_t x)
{
    REG_PL_WR(M154_AES_STATUS1_ADDR,x);
}

static inline uint32_t m154_aes_tx_mpdu_aes_ddt_done_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t m154_aes_first_a_blk_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t m154_aes_first_m_blk_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t m154_aes_last_a_blk_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_aes_last_m_blk_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_aes_dma_ctrl_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x0000f000)) >> 12);
}

static inline uint32_t m154_aes_aes_ctrl_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00000f00)) >> 8);
}

static inline uint32_t m154_aes_aes_mst_cs_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x000000f0)) >> 4);
}

static inline uint32_t m154_aes_mic_error_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_aes_aes_busy_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS1_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define M154_AES_STATUS2_ADDR (M154_AES_BASE + 0x00000874)

static inline uint32_t m154_aes_status2_get(void)
{
    return REG_PL_RD(M154_AES_STATUS2_ADDR);
}

static inline void m154_aes_status2_set(uint32_t x)
{
    REG_PL_WR(M154_AES_STATUS2_ADDR,x);
}

static inline uint32_t m154_aes_u_c_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS2_ADDR);
    return ((localVal & ((uint32_t)0x01ff0000)) >> 16);
}

static inline uint32_t m154_aes_u_ctr_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS2_ADDR);
    return ((localVal & ((uint32_t)0x0000ffff)) >> 0);
}


#define M154_AES_STATUS3_ADDR (M154_AES_BASE + 0x00000878)

static inline uint32_t m154_aes_status3_get(void)
{
    return REG_PL_RD(M154_AES_STATUS3_ADDR);
}

static inline void m154_aes_status3_set(uint32_t x)
{
    REG_PL_WR(M154_AES_STATUS3_ADDR,x);
}

static inline uint32_t m154_aes_u_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS3_ADDR);
    return ((localVal & ((uint32_t)0x00003000)) >> 12);
}

static inline uint32_t m154_aes_u_am_cnt_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_STATUS3_ADDR);
    return ((localVal & ((uint32_t)0x000003ff)) >> 0);
}


#define M154_AES_IER_ADDR (M154_AES_BASE + 0x00000880)

static inline uint32_t m154_aes_ier_get(void)
{
    return REG_PL_RD(M154_AES_IER_ADDR);
}

static inline void m154_aes_ier_set(uint32_t x)
{
    REG_PL_WR(M154_AES_IER_ADDR,x);
}

static inline uint32_t m154_aes_dmarx_done_hw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_aes_dmarx_done_hw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_dmatx_done_hw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_aes_dmatx_done_hw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_rxerr_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_aes_aesrx_done_rxerr_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_hw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_aes_aesrx_done_hw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_aestx_done_hw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_aes_aestx_done_hw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_aesdec_done_fw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_aes_aesdec_done_fw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}

static inline uint32_t m154_aes_aesenc_done_fw_int_en_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_aes_aesenc_done_fw_int_en_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IER_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_AES_IER_ADDR,localVal);
}


#define M154_AES_ICR_ADDR (M154_AES_BASE + 0x00000884)

static inline uint32_t m154_aes_icr_get(void)
{
    return REG_PL_RD(M154_AES_ICR_ADDR);
}

static inline void m154_aes_icr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ICR_ADDR,x);
}

static inline uint32_t m154_aes_dmarx_done_hw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_aes_dmarx_done_hw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_dmatx_done_hw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_aes_dmatx_done_hw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_rxerr_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_aes_aesrx_done_rxerr_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_hw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_aes_aesrx_done_hw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_aestx_done_hw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_aes_aestx_done_hw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesdec_done_fw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_aes_aesdec_done_fw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesenc_done_fw_int_clr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_aes_aesenc_done_fw_int_clr_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ICR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_AES_ICR_ADDR,localVal);
}


#define M154_AES_IMR_ADDR (M154_AES_BASE + 0x00000888)

static inline uint32_t m154_aes_imr_get(void)
{
    return REG_PL_RD(M154_AES_IMR_ADDR);
}

static inline void m154_aes_imr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_IMR_ADDR,x);
}

static inline uint32_t m154_aes_dmarx_done_hw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline void m154_aes_dmarx_done_hw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000040);
    localVal |= (x << 6)&((uint32_t)0x00000040);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_dmatx_done_hw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline void m154_aes_dmatx_done_hw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000020);
    localVal |= (x << 5)&((uint32_t)0x00000020);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_rxerr_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline void m154_aes_aesrx_done_rxerr_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000010);
    localVal |= (x << 4)&((uint32_t)0x00000010);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesrx_done_hw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void m154_aes_aesrx_done_hw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aestx_done_hw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void m154_aes_aestx_done_hw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesdec_done_fw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void m154_aes_aesdec_done_fw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}

static inline uint32_t m154_aes_aesenc_done_fw_int_mask_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void m154_aes_aesenc_done_fw_int_mask_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IMR_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(M154_AES_IMR_ADDR,localVal);
}


#define M154_AES_ISR_ADDR (M154_AES_BASE + 0x0000088c)

static inline uint32_t m154_aes_isr_get(void)
{
    return REG_PL_RD(M154_AES_ISR_ADDR);
}

static inline void m154_aes_isr_set(uint32_t x)
{
    REG_PL_WR(M154_AES_ISR_ADDR,x);
}

static inline uint32_t m154_aes_set_dmarx_done_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00400000)) >> 22);
}

static inline uint32_t m154_aes_set_dmatx_done_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00200000)) >> 21);
}

static inline uint32_t m154_aes_set_aesrx_done_rxerr_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00100000)) >> 20);
}

static inline uint32_t m154_aes_set_aesrx_done_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00080000)) >> 19);
}

static inline uint32_t m154_aes_set_aestx_done_hw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00040000)) >> 18);
}

static inline uint32_t m154_aes_set_aesdec_done_fw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00020000)) >> 17);
}

static inline uint32_t m154_aes_set_aesenc_done_fw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00010000)) >> 16);
}

static inline uint32_t m154_aes_dmarx_done_hw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t m154_aes_dmatx_done_hw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t m154_aes_aesrx_done_rxerr_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t m154_aes_aesrx_done_hw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t m154_aes_aestx_done_hw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t m154_aes_aesdec_done_fw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_aes_aesenc_done_fw_int_raw_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_ISR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}


#define M154_AES_IR_ADDR (M154_AES_BASE + 0x00000890)

static inline uint32_t m154_aes_ir_get(void)
{
    return REG_PL_RD(M154_AES_IR_ADDR);
}

static inline void m154_aes_ir_set(uint32_t x)
{
    REG_PL_WR(M154_AES_IR_ADDR,x);
}

static inline uint32_t m154_aes_dmarx_done_hw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000040)) >> 6);
}

static inline uint32_t m154_aes_dmatx_done_hw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000020)) >> 5);
}

static inline uint32_t m154_aes_aesrx_done_rxerr_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000010)) >> 4);
}

static inline uint32_t m154_aes_aesrx_done_hw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline uint32_t m154_aes_aestx_done_hw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline uint32_t m154_aes_aesdec_done_fw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline uint32_t m154_aes_aesenc_done_fw_int_getf(void)
{
    uint32_t localVal = REG_PL_RD(M154_AES_IR_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

#endif