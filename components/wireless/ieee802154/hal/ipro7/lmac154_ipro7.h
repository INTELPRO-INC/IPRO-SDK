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

#ifndef __LMAC154_IPRO7_H__
#define __LMAC154_IPRO7_H__

#include "ipro7.h"
// #include <bl616_glb.h>
// #include "rf.h"
#include <bz_phy_reg.h>
#include <m154_reg.h>
#include <m154_aes_reg.h>
// #include <wl_api.h>

/**
 * @brief Memory access macro
 */
#define BL_RD_WORD(addr)       (*((volatile uint32_t *)(uintptr_t)(addr)))
#define BL_WR_WORD(addr, val)  ((*(volatile uint32_t *)(uintptr_t)(addr)) = (val))
#define BL_RD_SHORT(addr)      (*((volatile uint16_t *)(uintptr_t)(addr)))
#define BL_WR_SHORT(addr, val) ((*(volatile uint16_t *)(uintptr_t)(addr)) = (val))
#define BL_RD_BYTE(addr)       (*((volatile uint8_t *)(uintptr_t)(addr)))
#define BL_WR_BYTE(addr, val)  ((*(volatile uint8_t *)(uintptr_t)(addr)) = (val))
#define BL_RDWD_FRM_BYTEP(p)   ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))

#define BL_WRWD_TO_BYTEP(p, val)   \
    {                              \
        p[0] = val & 0xff;         \
        p[1] = (val >> 8) & 0xff;  \
        p[2] = (val >> 16) & 0xff; \
        p[3] = (val >> 24) & 0xff; \
    }
/**
 * @brief Register access macro
 */
#define BL_RD_REG16(addr, regname)                BL_RD_SHORT(addr + regname##_OFFSET)
#define BL_WR_REG16(addr, regname, val)           BL_WR_SHORT(addr + regname##_OFFSET, val)
#define BL_RD_REG(addr, regname)                  BL_RD_WORD(addr + regname##_OFFSET)
#define BL_WR_REG(addr, regname, val)             BL_WR_WORD(addr + regname##_OFFSET, val)
#define BL_SET_REG_BIT(val, bitname)              ((val) | (1U << bitname##_POS))
#define BL_CLR_REG_BIT(val, bitname)              ((val)&bitname##_UMSK)
#define BL_GET_REG_BITS_VAL(val, bitname)         (((val)&bitname##_MSK) >> bitname##_POS)
#define BL_SET_REG_BITS_VAL(val, bitname, bitval) (((val)&bitname##_UMSK) | ((uint32_t)(bitval) << bitname##_POS))
#define BL_IS_REG_BIT_SET(val, bitname)           (((val) & (1U << (bitname##_POS))) != 0)

// #define MAC154_BASE             ((uint32_t)0x2C000000)
// #define BZ_PHY_BASE             ((uint32_t)0x20002000)
// #define RF_BASE                 ((uint32_t)0x20001000)
#define RF_REV_ADDR             ((uint32_t)(RF_BASE + 0x00000000))
#define RF_FSM_ADDR             ((uint32_t)(RF_BASE + 0x00000510))
#define BL_PTA_CONFIG_ADDR      ((uint32_t)0x24920004)
#define BL_PTA_CONFIG2_ADDR     ((uint32_t)0x24920028)
#define BL_PTA_CONFIG2_ADDR_BZ_PTI_MSK ((uint32_t)0x00000002)


// set tx start offset: 64us (value of multiple of 16 will eliminate sym_cnt deviation)
// for AD9361
#ifdef FPGA
#ifdef RFDTB9361
#define LMAC154_TX_START_OFFSET_VALUE   (96)
#else
#define LMAC154_TX_START_OFFSET_VALUE   (64)
#endif
#else
#define LMAC154_TX_START_OFFSET_VALUE   (64)
#endif
#define LMAC154_RF_STATE_TABLE          {{0, LMAC154_RF_STATE_IDLE}, {1, LMAC154_RF_STATE_TX}, {2, LMAC154_RF_STATE_TX}}

#define LMAC154_GET_TX_POWER()          (lmac154_tx_power_value)
#define LMAC154_RFPHY_START_CONT_RX()   
#define LMAC154_RFPHY_STOP_CONT_RX()    

//  #define LMAC154_RF_STATE()              rf_fsm_state_getf()
// #define LMAC154_RF_STATE()              ((*(volatile uint32_t *)RF_FSM_CTRL_HW_ADDR & 0x00000070) >> 4)
#define LMAC154_RF_STATE()              ((*(volatile uint32_t *)RF_FSM_ADDR & 0x00007000) >> 12)
#define LMAC154_IS_DISABLED()           !(BL_IS_REG_BIT_SET(BL_RD_REG(MAC154_BASE, M154_GLB_CFG), M154_CLK_EN)) // TODO: sync with 702L
#define LMAC154_RFPHY_IS_IDLE()         (LMAC154_RF_STATE() == 0)

// Original BL616/BL702 check at unmapped IPRO7 address — replaced.
// #define LMAC154_COEX_ENABLED()          ((*((volatile uint32_t *)BL_PTA_CONFIG2_ADDR) & BL_PTA_CONFIG2_ADDR_BZ_PTI_MSK) == BL_PTA_CONFIG2_ADDR_BZ_PTI_MSK)
//
// IPRO7 puts the coex enable bit in the GLB block: BZ_COEX_CTRL @ 0x300000C0 bit[0]
// (per IPRO7AI_REG_all_20260515.xlsx "glb" sheet, addr=0xC0). The matter
// lighting-app turns this on in ble_glue.c::ipro7_ble_init AFTER bt_enable.
// Reading the real bit makes the SDK ISR's coex-aware TX abort recovery
// actually run (lmac154.c::LMAC154_ISR_2015 line ~2028): disable abort IRQ
// for the in-flight TX (avoid IRQ storm when BLE conn-event bursts beat
// arbitration) and bump TX priority for the next retry. Without this fix,
// the macro returns 0, the recovery branch is dead code, and abort IRQs
// can flood the CPU while priority never escalates above the static value
// set in m154_coex_priority.
#define LMAC154_COEX_ENABLED()          ((*((volatile uint32_t *)0x300000C0) & 0x1u) != 0)

#define LMAC154_RX_PROMISCUOUS()        BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_RX_02), M154_RX_PROMISCUOUS_MODE)

#define LMAC154_TX_FORBIDDEN()          ( ( BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_RX_09), M154_SFD_FOUND) == 1 ) ||        \
                                          ( BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_TX_0), M154_TX_AUTO_ACK_DOING) == 1 ) || \
                                          ( BL_GET_REG_BITS_VAL(BL_RD_REG(MAC154_BASE, M154_TX_3), M154_TX_FSM_CURR) != 1 ) )


#define LMAC154_GET_RX_MPDU_LEN_FCS()       BL_RD_REG(MAC154_BASE, M154_RX_PPDU_STATUS)
#define LMAC154_GET_TX_MPDU_LEN_FCS()       BL_RD_REG(MAC154_BASE, M154_TX_PPDU_PHR)
#define LMAC154_SET_TX_MPDU_LEN_FCS(x)      BL_WR_REG(MAC154_BASE, M154_TX_PPDU_PHR, x)

#define LMAC154_RF_SET_TX_BYPASS(x)

#define LMAC154_ISR_RX_MHR(x, y, z)            LMAC154_EVENT_RX_MHR
#define LMAC154_ISR_RX_ACK()
#define LMAC154_ISR_ACK_TIMEOUT()


#endif
