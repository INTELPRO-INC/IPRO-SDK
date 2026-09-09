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

#ifndef _REG_RF_EPA_H_
#define _REG_RF_EPA_H_
// Date: 20211018

#include "co_int.h"
#include "compiler.h"
#include "arch.h"
#include "reg_access.h"


#ifndef RF_EPA_BASE
#define RF_EPA_BASE (0x20001000)
#endif



#define RF_EPA_0_ADDR (RF_EPA_BASE + 0x00000580)

static inline uint32_t rf_epa_0_get(void)
{
    return REG_PL_RD(RF_EPA_0_ADDR);
}

static inline void rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RF_EPA_0_ADDR,localVal);
}


#define RX2ON_RF_EPA_0_ADDR (RF_EPA_BASE + 0x00000584)

static inline uint32_t rx2on_rf_epa_0_get(void)
{
    return REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
}

static inline void rx2on_rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(RX2ON_RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_rx2on_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_rx2on_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rx2on_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_rx2on_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rx2on_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_rx2on_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rx2on_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_rx2on_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RX2ON_RF_EPA_0_ADDR,localVal);
}


#define RXON_RF_EPA_0_ADDR (RF_EPA_BASE + 0x00000588)

static inline uint32_t rxon_rf_epa_0_get(void)
{
    return REG_PL_RD(RXON_RF_EPA_0_ADDR);
}

static inline void rxon_rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(RXON_RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_rxon_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_rxon_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(RXON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rxon_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_rxon_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(RXON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rxon_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_rxon_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(RXON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_rxon_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_rxon_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(RXON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(RXON_RF_EPA_0_ADDR,localVal);
}


#define TX2ON_RF_EPA_0_ADDR (RF_EPA_BASE + 0x0000058c)

static inline uint32_t tx2on_rf_epa_0_get(void)
{
    return REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
}

static inline void tx2on_rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(TX2ON_RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_tx2on_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_tx2on_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(TX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2on_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_tx2on_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(TX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2on_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_tx2on_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(TX2ON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2on_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_tx2on_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2ON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(TX2ON_RF_EPA_0_ADDR,localVal);
}


#define TX2PAON_RF_EPA_0_ADDR (RF_EPA_BASE + 0x00000590)

static inline uint32_t tx2paon_rf_epa_0_get(void)
{
    return REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
}

static inline void tx2paon_rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(TX2PAON_RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_tx2paon_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_tx2paon_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(TX2PAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2paon_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_tx2paon_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(TX2PAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2paon_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_tx2paon_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(TX2PAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_tx2paon_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_tx2paon_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TX2PAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(TX2PAON_RF_EPA_0_ADDR,localVal);
}


#define TXPAON_RF_EPA_0_ADDR (RF_EPA_BASE + 0x00000594)

static inline uint32_t txpaon_rf_epa_0_get(void)
{
    return REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
}

static inline void txpaon_rf_epa_0_set(uint32_t x)
{
    REG_PL_WR(TXPAON_RF_EPA_0_ADDR,x);
}

static inline uint32_t rf_epa_txpaon_vector_03_getf(void)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000008)) >> 3);
}

static inline void rf_epa_txpaon_vector_03_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000008);
    localVal |= (x << 3)&((uint32_t)0x00000008);
    REG_PL_WR(TXPAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_txpaon_vector_02_getf(void)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000004)) >> 2);
}

static inline void rf_epa_txpaon_vector_02_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000004);
    localVal |= (x << 2)&((uint32_t)0x00000004);
    REG_PL_WR(TXPAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_txpaon_vector_01_getf(void)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000002)) >> 1);
}

static inline void rf_epa_txpaon_vector_01_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000002);
    localVal |= (x << 1)&((uint32_t)0x00000002);
    REG_PL_WR(TXPAON_RF_EPA_0_ADDR,localVal);
}

static inline uint32_t rf_epa_txpaon_vector_00_getf(void)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    return ((localVal & ((uint32_t)0x00000001)) >> 0);
}

static inline void rf_epa_txpaon_vector_00_setf(uint32_t x)
{
    uint32_t localVal = REG_PL_RD(TXPAON_RF_EPA_0_ADDR);
    localVal &= ~((uint32_t)0x00000001);
    localVal |= (x << 0)&((uint32_t)0x00000001);
    REG_PL_WR(TXPAON_RF_EPA_0_ADDR,localVal);
}

#endif