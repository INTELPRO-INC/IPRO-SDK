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

#ifndef  __BZ_PHY_REG_H__
#define  __BZ_PHY_REG_H__

#include "ipro6.h"

/* 0x800 : phy_top_0 */
#define BZ_PHY_PHY_TOP_0_OFFSET                                 (0x800)
#define BZ_PHY_RXDFE_EN                                         BZ_PHY_RXDFE_EN
#define BZ_PHY_RXDFE_EN_POS                                     (0U)
#define BZ_PHY_RXDFE_EN_LEN                                     (1U)
#define BZ_PHY_RXDFE_EN_MSK                                     (((1ULL<<BZ_PHY_RXDFE_EN_LEN)-1)<<BZ_PHY_RXDFE_EN_POS)
#define BZ_PHY_RXDFE_EN_UMSK                                    (~(((1ULL<<BZ_PHY_RXDFE_EN_LEN)-1)<<BZ_PHY_RXDFE_EN_POS))
#define BZ_PHY_DFE_RXMODE_FORCE                                 BZ_PHY_DFE_RXMODE_FORCE
#define BZ_PHY_DFE_RXMODE_FORCE_POS                             (1U)
#define BZ_PHY_DFE_RXMODE_FORCE_LEN                             (1U)
#define BZ_PHY_DFE_RXMODE_FORCE_MSK                             (((1ULL<<BZ_PHY_DFE_RXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_RXMODE_FORCE_POS)
#define BZ_PHY_DFE_RXMODE_FORCE_UMSK                            (~(((1ULL<<BZ_PHY_DFE_RXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_RXMODE_FORCE_POS))
#define BZ_PHY_DFE_RXMODE                                       BZ_PHY_DFE_RXMODE
#define BZ_PHY_DFE_RXMODE_POS                                   (2U)
#define BZ_PHY_DFE_RXMODE_LEN                                   (3U)
#define BZ_PHY_DFE_RXMODE_MSK                                   (((1ULL<<BZ_PHY_DFE_RXMODE_LEN)-1)<<BZ_PHY_DFE_RXMODE_POS)
#define BZ_PHY_DFE_RXMODE_UMSK                                  (~(((1ULL<<BZ_PHY_DFE_RXMODE_LEN)-1)<<BZ_PHY_DFE_RXMODE_POS))
#define BZ_PHY_TXDFE_EN                                         BZ_PHY_TXDFE_EN
#define BZ_PHY_TXDFE_EN_POS                                     (5U)
#define BZ_PHY_TXDFE_EN_LEN                                     (1U)
#define BZ_PHY_TXDFE_EN_MSK                                     (((1ULL<<BZ_PHY_TXDFE_EN_LEN)-1)<<BZ_PHY_TXDFE_EN_POS)
#define BZ_PHY_TXDFE_EN_UMSK                                    (~(((1ULL<<BZ_PHY_TXDFE_EN_LEN)-1)<<BZ_PHY_TXDFE_EN_POS))
#define BZ_PHY_DFE_TXMODE_FORCE                                 BZ_PHY_DFE_TXMODE_FORCE
#define BZ_PHY_DFE_TXMODE_FORCE_POS                             (6U)
#define BZ_PHY_DFE_TXMODE_FORCE_LEN                             (1U)
#define BZ_PHY_DFE_TXMODE_FORCE_MSK                             (((1ULL<<BZ_PHY_DFE_TXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_TXMODE_FORCE_POS)
#define BZ_PHY_DFE_TXMODE_FORCE_UMSK                            (~(((1ULL<<BZ_PHY_DFE_TXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_TXMODE_FORCE_POS))
#define BZ_PHY_DFE_TXMODE                                       BZ_PHY_DFE_TXMODE
#define BZ_PHY_DFE_TXMODE_POS                                   (7U)
#define BZ_PHY_DFE_TXMODE_LEN                                   (3U)
#define BZ_PHY_DFE_TXMODE_MSK                                   (((1ULL<<BZ_PHY_DFE_TXMODE_LEN)-1)<<BZ_PHY_DFE_TXMODE_POS)
#define BZ_PHY_DFE_TXMODE_UMSK                                  (~(((1ULL<<BZ_PHY_DFE_TXMODE_LEN)-1)<<BZ_PHY_DFE_TXMODE_POS))
#define BZ_PHY_DFE_CLK_FORCE_ON                                 BZ_PHY_DFE_CLK_FORCE_ON
#define BZ_PHY_DFE_CLK_FORCE_ON_POS                             (10U)
#define BZ_PHY_DFE_CLK_FORCE_ON_LEN                             (1U)
#define BZ_PHY_DFE_CLK_FORCE_ON_MSK                             (((1ULL<<BZ_PHY_DFE_CLK_FORCE_ON_LEN)-1)<<BZ_PHY_DFE_CLK_FORCE_ON_POS)
#define BZ_PHY_DFE_CLK_FORCE_ON_UMSK                            (~(((1ULL<<BZ_PHY_DFE_CLK_FORCE_ON_LEN)-1)<<BZ_PHY_DFE_CLK_FORCE_ON_POS))
#define BZ_PHY_ADC_CLK_SEL                                      BZ_PHY_ADC_CLK_SEL
#define BZ_PHY_ADC_CLK_SEL_POS                                  (11U)
#define BZ_PHY_ADC_CLK_SEL_LEN                                  (1U)
#define BZ_PHY_ADC_CLK_SEL_MSK                                  (((1ULL<<BZ_PHY_ADC_CLK_SEL_LEN)-1)<<BZ_PHY_ADC_CLK_SEL_POS)
#define BZ_PHY_ADC_CLK_SEL_UMSK                                 (~(((1ULL<<BZ_PHY_ADC_CLK_SEL_LEN)-1)<<BZ_PHY_ADC_CLK_SEL_POS))
#define BZ_PHY_DFE_RXDFE_4S_ON                                  BZ_PHY_DFE_RXDFE_4S_ON
#define BZ_PHY_DFE_RXDFE_4S_ON_POS                              (12U)
#define BZ_PHY_DFE_RXDFE_4S_ON_LEN                              (1U)
#define BZ_PHY_DFE_RXDFE_4S_ON_MSK                              (((1ULL<<BZ_PHY_DFE_RXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_RXDFE_4S_ON_POS)
#define BZ_PHY_DFE_RXDFE_4S_ON_UMSK                             (~(((1ULL<<BZ_PHY_DFE_RXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_RXDFE_4S_ON_POS))
#define BZ_PHY_DFE_TXDFE_4S_ON                                  BZ_PHY_DFE_TXDFE_4S_ON
#define BZ_PHY_DFE_TXDFE_4S_ON_POS                              (13U)
#define BZ_PHY_DFE_TXDFE_4S_ON_LEN                              (1U)
#define BZ_PHY_DFE_TXDFE_4S_ON_MSK                              (((1ULL<<BZ_PHY_DFE_TXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_TXDFE_4S_ON_POS)
#define BZ_PHY_DFE_TXDFE_4S_ON_UMSK                             (~(((1ULL<<BZ_PHY_DFE_TXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_TXDFE_4S_ON_POS))

/* 0x804 : phy_tx_dfe_0 */
#define BZ_PHY_PHY_TX_DFE_0_OFFSET                              (0x804)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ                                BZ_PHY_TX_DFE_IQ_IF_FREQ
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_POS                            (0U)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN                            (16U)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_MSK                            (((1ULL<<BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN)-1)<<BZ_PHY_TX_DFE_IQ_IF_FREQ_POS)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_UMSK                           (~(((1ULL<<BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN)-1)<<BZ_PHY_TX_DFE_IQ_IF_FREQ_POS))
#define BZ_PHY_TX_DFE_PSF_BLE1M                                 BZ_PHY_TX_DFE_PSF_BLE1M
#define BZ_PHY_TX_DFE_PSF_BLE1M_POS                             (16U)
#define BZ_PHY_TX_DFE_PSF_BLE1M_LEN                             (2U)
#define BZ_PHY_TX_DFE_PSF_BLE1M_MSK                             (((1ULL<<BZ_PHY_TX_DFE_PSF_BLE1M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE1M_POS)
#define BZ_PHY_TX_DFE_PSF_BLE1M_UMSK                            (~(((1ULL<<BZ_PHY_TX_DFE_PSF_BLE1M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE1M_POS))
#define BZ_PHY_TX_DFE_PSF_BLE2M                                 BZ_PHY_TX_DFE_PSF_BLE2M
#define BZ_PHY_TX_DFE_PSF_BLE2M_POS                             (18U)
#define BZ_PHY_TX_DFE_PSF_BLE2M_LEN                             (2U)
#define BZ_PHY_TX_DFE_PSF_BLE2M_MSK                             (((1ULL<<BZ_PHY_TX_DFE_PSF_BLE2M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE2M_POS)
#define BZ_PHY_TX_DFE_PSF_BLE2M_UMSK                            (~(((1ULL<<BZ_PHY_TX_DFE_PSF_BLE2M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE2M_POS))
#define BZ_PHY_TX_DFE_PSF_ZIG                                   BZ_PHY_TX_DFE_PSF_ZIG
#define BZ_PHY_TX_DFE_PSF_ZIG_POS                               (20U)
#define BZ_PHY_TX_DFE_PSF_ZIG_LEN                               (2U)
#define BZ_PHY_TX_DFE_PSF_ZIG_MSK                               (((1ULL<<BZ_PHY_TX_DFE_PSF_ZIG_LEN)-1)<<BZ_PHY_TX_DFE_PSF_ZIG_POS)
#define BZ_PHY_TX_DFE_PSF_ZIG_UMSK                              (~(((1ULL<<BZ_PHY_TX_DFE_PSF_ZIG_LEN)-1)<<BZ_PHY_TX_DFE_PSF_ZIG_POS))
#define BZ_PHY_TX_PLL_BW_SWITCH_TH                              BZ_PHY_TX_PLL_BW_SWITCH_TH
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_POS                          (22U)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN                          (10U)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_MSK                          (((1ULL<<BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN)-1)<<BZ_PHY_TX_PLL_BW_SWITCH_TH_POS)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_UMSK                         (~(((1ULL<<BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN)-1)<<BZ_PHY_TX_PLL_BW_SWITCH_TH_POS))

/* 0x808 : phy_tx_dfe_1 */
#define BZ_PHY_PHY_TX_DFE_1_OFFSET                              (0x808)
#define BZ_PHY_TX_PA_PWR_CODE                                   BZ_PHY_TX_PA_PWR_CODE
#define BZ_PHY_TX_PA_PWR_CODE_POS                               (9U)
#define BZ_PHY_TX_PA_PWR_CODE_LEN                               (5U)
#define BZ_PHY_TX_PA_PWR_CODE_MSK                               (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_POS)
#define BZ_PHY_TX_PA_PWR_CODE_UMSK                              (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_POS))
#define BZ_PHY_TX_RAMPUP_AM_STEP                                BZ_PHY_TX_RAMPUP_AM_STEP
#define BZ_PHY_TX_RAMPUP_AM_STEP_POS                            (14U)
#define BZ_PHY_TX_RAMPUP_AM_STEP_LEN                            (8U)
#define BZ_PHY_TX_RAMPUP_AM_STEP_MSK                            (((1ULL<<BZ_PHY_TX_RAMPUP_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_STEP_POS)
#define BZ_PHY_TX_RAMPUP_AM_STEP_UMSK                           (~(((1ULL<<BZ_PHY_TX_RAMPUP_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_STEP_POS))
#define BZ_PHY_TX_RAMPUP_AM_ON                                  BZ_PHY_TX_RAMPUP_AM_ON
#define BZ_PHY_TX_RAMPUP_AM_ON_POS                              (22U)
#define BZ_PHY_TX_RAMPUP_AM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPUP_AM_ON_MSK                              (((1ULL<<BZ_PHY_TX_RAMPUP_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_ON_POS)
#define BZ_PHY_TX_RAMPUP_AM_ON_UMSK                             (~(((1ULL<<BZ_PHY_TX_RAMPUP_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_ON_POS))
#define BZ_PHY_TX_RAMPUP_FM_ON                                  BZ_PHY_TX_RAMPUP_FM_ON
#define BZ_PHY_TX_RAMPUP_FM_ON_POS                              (23U)
#define BZ_PHY_TX_RAMPUP_FM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPUP_FM_ON_MSK                              (((1ULL<<BZ_PHY_TX_RAMPUP_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_FM_ON_POS)
#define BZ_PHY_TX_RAMPUP_FM_ON_UMSK                             (~(((1ULL<<BZ_PHY_TX_RAMPUP_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_FM_ON_POS))
#define BZ_PHY_TX_RAMPUP_TIME_US                                BZ_PHY_TX_RAMPUP_TIME_US
#define BZ_PHY_TX_RAMPUP_TIME_US_POS                            (24U)
#define BZ_PHY_TX_RAMPUP_TIME_US_LEN                            (8U)
#define BZ_PHY_TX_RAMPUP_TIME_US_MSK                            (((1ULL<<BZ_PHY_TX_RAMPUP_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPUP_TIME_US_POS)
#define BZ_PHY_TX_RAMPUP_TIME_US_UMSK                           (~(((1ULL<<BZ_PHY_TX_RAMPUP_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPUP_TIME_US_POS))

/* 0x80C : phy_tx_dfe_2 */
#define BZ_PHY_PHY_TX_DFE_2_OFFSET                              (0x80C)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US                           BZ_PHY_TX_RAMPDN_PAD0_TIME_US
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS                       (0U)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN                       (8U)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_MSK                       (((1ULL<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_UMSK                      (~(((1ULL<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS))
#define BZ_PHY_TX_RAMPDN_AM_START_DLY                           BZ_PHY_TX_RAMPDN_AM_START_DLY
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_POS                       (8U)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN                       (6U)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_MSK                       (((1ULL<<BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_START_DLY_POS)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_UMSK                      (~(((1ULL<<BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_START_DLY_POS))
#define BZ_PHY_TX_RAMPDN_AM_STEP                                BZ_PHY_TX_RAMPDN_AM_STEP
#define BZ_PHY_TX_RAMPDN_AM_STEP_POS                            (14U)
#define BZ_PHY_TX_RAMPDN_AM_STEP_LEN                            (8U)
#define BZ_PHY_TX_RAMPDN_AM_STEP_MSK                            (((1ULL<<BZ_PHY_TX_RAMPDN_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_STEP_POS)
#define BZ_PHY_TX_RAMPDN_AM_STEP_UMSK                           (~(((1ULL<<BZ_PHY_TX_RAMPDN_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_STEP_POS))
#define BZ_PHY_TX_RAMPDN_AM_ON                                  BZ_PHY_TX_RAMPDN_AM_ON
#define BZ_PHY_TX_RAMPDN_AM_ON_POS                              (22U)
#define BZ_PHY_TX_RAMPDN_AM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPDN_AM_ON_MSK                              (((1ULL<<BZ_PHY_TX_RAMPDN_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_ON_POS)
#define BZ_PHY_TX_RAMPDN_AM_ON_UMSK                             (~(((1ULL<<BZ_PHY_TX_RAMPDN_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_ON_POS))
#define BZ_PHY_TX_RAMPDN_FM_ON                                  BZ_PHY_TX_RAMPDN_FM_ON
#define BZ_PHY_TX_RAMPDN_FM_ON_POS                              (23U)
#define BZ_PHY_TX_RAMPDN_FM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPDN_FM_ON_MSK                              (((1ULL<<BZ_PHY_TX_RAMPDN_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_FM_ON_POS)
#define BZ_PHY_TX_RAMPDN_FM_ON_UMSK                             (~(((1ULL<<BZ_PHY_TX_RAMPDN_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_FM_ON_POS))
#define BZ_PHY_TX_RAMPDN_TIME_US                                BZ_PHY_TX_RAMPDN_TIME_US
#define BZ_PHY_TX_RAMPDN_TIME_US_POS                            (24U)
#define BZ_PHY_TX_RAMPDN_TIME_US_LEN                            (8U)
#define BZ_PHY_TX_RAMPDN_TIME_US_MSK                            (((1ULL<<BZ_PHY_TX_RAMPDN_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_TIME_US_POS)
#define BZ_PHY_TX_RAMPDN_TIME_US_UMSK                           (~(((1ULL<<BZ_PHY_TX_RAMPDN_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_TIME_US_POS))

/* 0x810 : phy_rx_dfe_0 */
#define BZ_PHY_PHY_RX_DFE_0_OFFSET                              (0x810)
#define BZ_PHY_RX_DFE_TOC_EN                                    BZ_PHY_RX_DFE_TOC_EN
#define BZ_PHY_RX_DFE_TOC_EN_POS                                (1U)
#define BZ_PHY_RX_DFE_TOC_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_TOC_EN_MSK                                (((1ULL<<BZ_PHY_RX_DFE_TOC_EN_LEN)-1)<<BZ_PHY_RX_DFE_TOC_EN_POS)
#define BZ_PHY_RX_DFE_TOC_EN_UMSK                               (~(((1ULL<<BZ_PHY_RX_DFE_TOC_EN_LEN)-1)<<BZ_PHY_RX_DFE_TOC_EN_POS))
#define BZ_PHY_RX_DFE_DDC_EN                                    BZ_PHY_RX_DFE_DDC_EN
#define BZ_PHY_RX_DFE_DDC_EN_POS                                (2U)
#define BZ_PHY_RX_DFE_DDC_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_DDC_EN_MSK                                (((1ULL<<BZ_PHY_RX_DFE_DDC_EN_LEN)-1)<<BZ_PHY_RX_DFE_DDC_EN_POS)
#define BZ_PHY_RX_DFE_DDC_EN_UMSK                               (~(((1ULL<<BZ_PHY_RX_DFE_DDC_EN_LEN)-1)<<BZ_PHY_RX_DFE_DDC_EN_POS))
#define BZ_PHY_RX_DFE_CS_EN                                     BZ_PHY_RX_DFE_CS_EN
#define BZ_PHY_RX_DFE_CS_EN_POS                                 (3U)
#define BZ_PHY_RX_DFE_CS_EN_LEN                                 (1U)
#define BZ_PHY_RX_DFE_CS_EN_MSK                                 (((1ULL<<BZ_PHY_RX_DFE_CS_EN_LEN)-1)<<BZ_PHY_RX_DFE_CS_EN_POS)
#define BZ_PHY_RX_DFE_CS_EN_UMSK                                (~(((1ULL<<BZ_PHY_RX_DFE_CS_EN_LEN)-1)<<BZ_PHY_RX_DFE_CS_EN_POS))
#define BZ_PHY_RX_DFE_DGA_EN                                    BZ_PHY_RX_DFE_DGA_EN
#define BZ_PHY_RX_DFE_DGA_EN_POS                                (4U)
#define BZ_PHY_RX_DFE_DGA_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_DGA_EN_MSK                                (((1ULL<<BZ_PHY_RX_DFE_DGA_EN_LEN)-1)<<BZ_PHY_RX_DFE_DGA_EN_POS)
#define BZ_PHY_RX_DFE_DGA_EN_UMSK                               (~(((1ULL<<BZ_PHY_RX_DFE_DGA_EN_LEN)-1)<<BZ_PHY_RX_DFE_DGA_EN_POS))
#define BZ_PHY_RX_DFE_DIFFOSR_EN                                BZ_PHY_RX_DFE_DIFFOSR_EN
#define BZ_PHY_RX_DFE_DIFFOSR_EN_POS                            (5U)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_LEN                            (1U)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_MSK                            (((1ULL<<BZ_PHY_RX_DFE_DIFFOSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_DIFFOSR_EN_POS)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_UMSK                           (~(((1ULL<<BZ_PHY_RX_DFE_DIFFOSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_DIFFOSR_EN_POS))
#define BZ_PHY_RX_DFE_NSR_EN                                    BZ_PHY_RX_DFE_NSR_EN
#define BZ_PHY_RX_DFE_NSR_EN_POS                                (6U)
#define BZ_PHY_RX_DFE_NSR_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_NSR_EN_MSK                                (((1ULL<<BZ_PHY_RX_DFE_NSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_NSR_EN_POS)
#define BZ_PHY_RX_DFE_NSR_EN_UMSK                               (~(((1ULL<<BZ_PHY_RX_DFE_NSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_NSR_EN_POS))
#define BZ_PHY_RX_DFE_LOOP_EN                                   BZ_PHY_RX_DFE_LOOP_EN
#define BZ_PHY_RX_DFE_LOOP_EN_POS                               (7U)
#define BZ_PHY_RX_DFE_LOOP_EN_LEN                               (1U)
#define BZ_PHY_RX_DFE_LOOP_EN_MSK                               (((1ULL<<BZ_PHY_RX_DFE_LOOP_EN_LEN)-1)<<BZ_PHY_RX_DFE_LOOP_EN_POS)
#define BZ_PHY_RX_DFE_LOOP_EN_UMSK                              (~(((1ULL<<BZ_PHY_RX_DFE_LOOP_EN_LEN)-1)<<BZ_PHY_RX_DFE_LOOP_EN_POS))
#define BZ_PHY_RX_DFE_DCCAN_EN                                  BZ_PHY_RX_DFE_DCCAN_EN
#define BZ_PHY_RX_DFE_DCCAN_EN_POS                              (8U)
#define BZ_PHY_RX_DFE_DCCAN_EN_LEN                              (1U)
#define BZ_PHY_RX_DFE_DCCAN_EN_MSK                              (((1ULL<<BZ_PHY_RX_DFE_DCCAN_EN_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_EN_POS)
#define BZ_PHY_RX_DFE_DCCAN_EN_UMSK                             (~(((1ULL<<BZ_PHY_RX_DFE_DCCAN_EN_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_EN_POS))

/* 0x814 : phy_rx_dfe_1 */
#define BZ_PHY_PHY_RX_DFE_1_OFFSET                              (0x814)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL                              BZ_PHY_RX_DFE_FREQ_TRK_VAL
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS                          (0U)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN                          (13U)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_MSK                          (((1ULL<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_UMSK                         (~(((1ULL<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS))
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE                            BZ_PHY_RX_DFE_FREQ_TRK_FORCE
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS                        (15U)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN                        (1U)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_MSK                        (((1ULL<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_UMSK                       (~(((1ULL<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS))

/* 0x818 : phy_rx_dfe_2 */
#define BZ_PHY_PHY_RX_DFE_2_OFFSET                              (0x818)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA                               BZ_PHY_RX_DFE_DCCAN_ALPHA
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_POS                           (23U)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN                           (2U)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_MSK                           (((1ULL<<BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_ALPHA_POS)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_UMSK                          (~(((1ULL<<BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_ALPHA_POS))

/* 0x81C : phy_rx_dfe_3 */
#define BZ_PHY_PHY_RX_DFE_3_OFFSET                              (0x81C)
#define BZ_PHY_RX_DFE_TOC_FAC                                   BZ_PHY_RX_DFE_TOC_FAC
#define BZ_PHY_RX_DFE_TOC_FAC_POS                               (0U)
#define BZ_PHY_RX_DFE_TOC_FAC_LEN                               (12U)
#define BZ_PHY_RX_DFE_TOC_FAC_MSK                               (((1ULL<<BZ_PHY_RX_DFE_TOC_FAC_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_POS)
#define BZ_PHY_RX_DFE_TOC_FAC_UMSK                              (~(((1ULL<<BZ_PHY_RX_DFE_TOC_FAC_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_POS))
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE                             BZ_PHY_RX_DFE_TOC_FAC_FORCE
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS                         (12U)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN                         (1U)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_MSK                         (((1ULL<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_UMSK                        (~(((1ULL<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS))
#define BZ_PHY_RX_DFE_DF_IQ_SEL                                 BZ_PHY_RX_DFE_DF_IQ_SEL
#define BZ_PHY_RX_DFE_DF_IQ_SEL_POS                             (30U)
#define BZ_PHY_RX_DFE_DF_IQ_SEL_LEN                             (2U)
#define BZ_PHY_RX_DFE_DF_IQ_SEL_MSK                             (((1ULL<<BZ_PHY_RX_DFE_DF_IQ_SEL_LEN)-1)<<BZ_PHY_RX_DFE_DF_IQ_SEL_POS)
#define BZ_PHY_RX_DFE_DF_IQ_SEL_UMSK                            (~(((1ULL<<BZ_PHY_RX_DFE_DF_IQ_SEL_LEN)-1)<<BZ_PHY_RX_DFE_DF_IQ_SEL_POS))

/* 0x820 : phy_rx_zb_receiver_0 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_0_OFFSET                      (0x820)
#define BZ_PHY_ZB_TS_THRES_TIMING                               BZ_PHY_ZB_TS_THRES_TIMING
#define BZ_PHY_ZB_TS_THRES_TIMING_POS                           (0U)
#define BZ_PHY_ZB_TS_THRES_TIMING_LEN                           (3U)
#define BZ_PHY_ZB_TS_THRES_TIMING_MSK                           (((1ULL<<BZ_PHY_ZB_TS_THRES_TIMING_LEN)-1)<<BZ_PHY_ZB_TS_THRES_TIMING_POS)
#define BZ_PHY_ZB_TS_THRES_TIMING_UMSK                          (~(((1ULL<<BZ_PHY_ZB_TS_THRES_TIMING_LEN)-1)<<BZ_PHY_ZB_TS_THRES_TIMING_POS))
#define BZ_PHY_ZB_TS_THRES3                                     BZ_PHY_ZB_TS_THRES3
#define BZ_PHY_ZB_TS_THRES3_POS                                 (4U)
#define BZ_PHY_ZB_TS_THRES3_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES3_MSK                                 (((1ULL<<BZ_PHY_ZB_TS_THRES3_LEN)-1)<<BZ_PHY_ZB_TS_THRES3_POS)
#define BZ_PHY_ZB_TS_THRES3_UMSK                                (~(((1ULL<<BZ_PHY_ZB_TS_THRES3_LEN)-1)<<BZ_PHY_ZB_TS_THRES3_POS))
#define BZ_PHY_ZB_TS_THRES2                                     BZ_PHY_ZB_TS_THRES2
#define BZ_PHY_ZB_TS_THRES2_POS                                 (10U)
#define BZ_PHY_ZB_TS_THRES2_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES2_MSK                                 (((1ULL<<BZ_PHY_ZB_TS_THRES2_LEN)-1)<<BZ_PHY_ZB_TS_THRES2_POS)
#define BZ_PHY_ZB_TS_THRES2_UMSK                                (~(((1ULL<<BZ_PHY_ZB_TS_THRES2_LEN)-1)<<BZ_PHY_ZB_TS_THRES2_POS))
#define BZ_PHY_ZB_TS_THRES1                                     BZ_PHY_ZB_TS_THRES1
#define BZ_PHY_ZB_TS_THRES1_POS                                 (16U)
#define BZ_PHY_ZB_TS_THRES1_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES1_MSK                                 (((1ULL<<BZ_PHY_ZB_TS_THRES1_LEN)-1)<<BZ_PHY_ZB_TS_THRES1_POS)
#define BZ_PHY_ZB_TS_THRES1_UMSK                                (~(((1ULL<<BZ_PHY_ZB_TS_THRES1_LEN)-1)<<BZ_PHY_ZB_TS_THRES1_POS))
#define BZ_PHY_ZB_TS_THRES0                                     BZ_PHY_ZB_TS_THRES0
#define BZ_PHY_ZB_TS_THRES0_POS                                 (22U)
#define BZ_PHY_ZB_TS_THRES0_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES0_MSK                                 (((1ULL<<BZ_PHY_ZB_TS_THRES0_LEN)-1)<<BZ_PHY_ZB_TS_THRES0_POS)
#define BZ_PHY_ZB_TS_THRES0_UMSK                                (~(((1ULL<<BZ_PHY_ZB_TS_THRES0_LEN)-1)<<BZ_PHY_ZB_TS_THRES0_POS))
#define BZ_PHY_ZB_TS_CC_ALPHA                                   BZ_PHY_ZB_TS_CC_ALPHA
#define BZ_PHY_ZB_TS_CC_ALPHA_POS                               (28U)
#define BZ_PHY_ZB_TS_CC_ALPHA_LEN                               (4U)
#define BZ_PHY_ZB_TS_CC_ALPHA_MSK                               (((1ULL<<BZ_PHY_ZB_TS_CC_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CC_ALPHA_POS)
#define BZ_PHY_ZB_TS_CC_ALPHA_UMSK                              (~(((1ULL<<BZ_PHY_ZB_TS_CC_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CC_ALPHA_POS))

/* 0x824 : phy_rx_zb_receiver_1 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_1_OFFSET                      (0x824)
#define BZ_PHY_ZB_DM_SFD_CNT_N                                  BZ_PHY_ZB_DM_SFD_CNT_N
#define BZ_PHY_ZB_DM_SFD_CNT_N_POS                              (0U)
#define BZ_PHY_ZB_DM_SFD_CNT_N_LEN                              (4U)
#define BZ_PHY_ZB_DM_SFD_CNT_N_MSK                              (((1ULL<<BZ_PHY_ZB_DM_SFD_CNT_N_LEN)-1)<<BZ_PHY_ZB_DM_SFD_CNT_N_POS)
#define BZ_PHY_ZB_DM_SFD_CNT_N_UMSK                             (~(((1ULL<<BZ_PHY_ZB_DM_SFD_CNT_N_LEN)-1)<<BZ_PHY_ZB_DM_SFD_CNT_N_POS))
#define BZ_PHY_ZB_DM_SFD_THRES                                  BZ_PHY_ZB_DM_SFD_THRES
#define BZ_PHY_ZB_DM_SFD_THRES_POS                              (4U)
#define BZ_PHY_ZB_DM_SFD_THRES_LEN                              (9U)
#define BZ_PHY_ZB_DM_SFD_THRES_MSK                              (((1ULL<<BZ_PHY_ZB_DM_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_POS)
#define BZ_PHY_ZB_DM_SFD_THRES_UMSK                             (~(((1ULL<<BZ_PHY_ZB_DM_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_POS))
#define BZ_PHY_ZB_DM_P_MODE_EN                                  BZ_PHY_ZB_DM_P_MODE_EN
#define BZ_PHY_ZB_DM_P_MODE_EN_POS                              (13U)
#define BZ_PHY_ZB_DM_P_MODE_EN_LEN                              (1U)
#define BZ_PHY_ZB_DM_P_MODE_EN_MSK                              (((1ULL<<BZ_PHY_ZB_DM_P_MODE_EN_LEN)-1)<<BZ_PHY_ZB_DM_P_MODE_EN_POS)
#define BZ_PHY_ZB_DM_P_MODE_EN_UMSK                             (~(((1ULL<<BZ_PHY_ZB_DM_P_MODE_EN_LEN)-1)<<BZ_PHY_ZB_DM_P_MODE_EN_POS))
#define BZ_PHY_ZB_DM_PLEN_ERR_MODE                              BZ_PHY_ZB_DM_PLEN_ERR_MODE
#define BZ_PHY_ZB_DM_PLEN_ERR_MODE_POS                          (15U)
#define BZ_PHY_ZB_DM_PLEN_ERR_MODE_LEN                          (1U)
#define BZ_PHY_ZB_DM_PLEN_ERR_MODE_MSK                          (((1ULL<<BZ_PHY_ZB_DM_PLEN_ERR_MODE_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_ERR_MODE_POS)
#define BZ_PHY_ZB_DM_PLEN_ERR_MODE_UMSK                         (~(((1ULL<<BZ_PHY_ZB_DM_PLEN_ERR_MODE_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_ERR_MODE_POS))
#define BZ_PHY_ZB_FT_COEFF                                      BZ_PHY_ZB_FT_COEFF
#define BZ_PHY_ZB_FT_COEFF_POS                                  (16U)
#define BZ_PHY_ZB_FT_COEFF_LEN                                  (4U)
#define BZ_PHY_ZB_FT_COEFF_MSK                                  (((1ULL<<BZ_PHY_ZB_FT_COEFF_LEN)-1)<<BZ_PHY_ZB_FT_COEFF_POS)
#define BZ_PHY_ZB_FT_COEFF_UMSK                                 (~(((1ULL<<BZ_PHY_ZB_FT_COEFF_LEN)-1)<<BZ_PHY_ZB_FT_COEFF_POS))
#define BZ_PHY_ZB_DM_PLEN_MIN                                   BZ_PHY_ZB_DM_PLEN_MIN
#define BZ_PHY_ZB_DM_PLEN_MIN_POS                               (20U)
#define BZ_PHY_ZB_DM_PLEN_MIN_LEN                               (4U)
#define BZ_PHY_ZB_DM_PLEN_MIN_MSK                               (((1ULL<<BZ_PHY_ZB_DM_PLEN_MIN_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MIN_POS)
#define BZ_PHY_ZB_DM_PLEN_MIN_UMSK                              (~(((1ULL<<BZ_PHY_ZB_DM_PLEN_MIN_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MIN_POS))
#define BZ_PHY_ZB_DM_PLEN_MAX                                   BZ_PHY_ZB_DM_PLEN_MAX
#define BZ_PHY_ZB_DM_PLEN_MAX_POS                               (24U)
#define BZ_PHY_ZB_DM_PLEN_MAX_LEN                               (8U)
#define BZ_PHY_ZB_DM_PLEN_MAX_MSK                               (((1ULL<<BZ_PHY_ZB_DM_PLEN_MAX_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MAX_POS)
#define BZ_PHY_ZB_DM_PLEN_MAX_UMSK                              (~(((1ULL<<BZ_PHY_ZB_DM_PLEN_MAX_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MAX_POS))

/* 0x828 : phy_rx_zb_receiver_2 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_2_OFFSET                      (0x828)
#define BZ_PHY_ZB_TS_RL_THRES                                   BZ_PHY_ZB_TS_RL_THRES
#define BZ_PHY_ZB_TS_RL_THRES_POS                               (0U)
#define BZ_PHY_ZB_TS_RL_THRES_LEN                               (4U)
#define BZ_PHY_ZB_TS_RL_THRES_MSK                               (((1ULL<<BZ_PHY_ZB_TS_RL_THRES_LEN)-1)<<BZ_PHY_ZB_TS_RL_THRES_POS)
#define BZ_PHY_ZB_TS_RL_THRES_UMSK                              (~(((1ULL<<BZ_PHY_ZB_TS_RL_THRES_LEN)-1)<<BZ_PHY_ZB_TS_RL_THRES_POS))
#define BZ_PHY_ZB_TS_CCRL_ALPHA                                 BZ_PHY_ZB_TS_CCRL_ALPHA
#define BZ_PHY_ZB_TS_CCRL_ALPHA_POS                             (4U)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_LEN                             (4U)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_MSK                             (((1ULL<<BZ_PHY_ZB_TS_CCRL_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CCRL_ALPHA_POS)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_UMSK                            (~(((1ULL<<BZ_PHY_ZB_TS_CCRL_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CCRL_ALPHA_POS))
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE                         BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS                     (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN                     (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_MSK                     (((1ULL<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_UMSK                    (~(((1ULL<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS))
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE                           BZ_PHY_ZB_DM_SFD_THRES_P_MODE
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS                       (16U)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN                       (8U)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_MSK                       (((1ULL<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_UMSK                      (~(((1ULL<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS))
#define BZ_PHY_ZB_DM_H_SFD_THRES                                BZ_PHY_ZB_DM_H_SFD_THRES
#define BZ_PHY_ZB_DM_H_SFD_THRES_POS                            (24U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_LEN                            (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_MSK                            (((1ULL<<BZ_PHY_ZB_DM_H_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_POS)
#define BZ_PHY_ZB_DM_H_SFD_THRES_UMSK                           (~(((1ULL<<BZ_PHY_ZB_DM_H_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_POS))

/* 0x82C : phy_rx_zb_receiver_3 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_3_OFFSET                      (0x82C)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME                               BZ_PHY_ZB_FSM_MF_DLY_TIME
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_POS                           (0U)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN                           (8U)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_MSK                           (((1ULL<<BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_MF_DLY_TIME_POS)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_UMSK                          (~(((1ULL<<BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_MF_DLY_TIME_POS))
#define BZ_PHY_ZB_FSM_DM_DLY_TIME                               BZ_PHY_ZB_FSM_DM_DLY_TIME
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_POS                           (8U)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN                           (10U)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_MSK                           (((1ULL<<BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_DM_DLY_TIME_POS)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_UMSK                          (~(((1ULL<<BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_DM_DLY_TIME_POS))
#define BZ_PHY_ZB_FSM_CG_4S_ON                                  BZ_PHY_ZB_FSM_CG_4S_ON
#define BZ_PHY_ZB_FSM_CG_4S_ON_POS                              (19U)
#define BZ_PHY_ZB_FSM_CG_4S_ON_LEN                              (1U)
#define BZ_PHY_ZB_FSM_CG_4S_ON_MSK                              (((1ULL<<BZ_PHY_ZB_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_ZB_FSM_CG_4S_ON_POS)
#define BZ_PHY_ZB_FSM_CG_4S_ON_UMSK                             (~(((1ULL<<BZ_PHY_ZB_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_ZB_FSM_CG_4S_ON_POS))
#define BZ_PHY_ZB_FSM_LO_TIME_US                                BZ_PHY_ZB_FSM_LO_TIME_US
#define BZ_PHY_ZB_FSM_LO_TIME_US_POS                            (20U)
#define BZ_PHY_ZB_FSM_LO_TIME_US_LEN                            (7U)
#define BZ_PHY_ZB_FSM_LO_TIME_US_MSK                            (((1ULL<<BZ_PHY_ZB_FSM_LO_TIME_US_LEN)-1)<<BZ_PHY_ZB_FSM_LO_TIME_US_POS)
#define BZ_PHY_ZB_FSM_LO_TIME_US_UMSK                           (~(((1ULL<<BZ_PHY_ZB_FSM_LO_TIME_US_LEN)-1)<<BZ_PHY_ZB_FSM_LO_TIME_US_POS))
#define BZ_PHY_ZB_FSM_CCA_ENABLE                                BZ_PHY_ZB_FSM_CCA_ENABLE
#define BZ_PHY_ZB_FSM_CCA_ENABLE_POS                            (27U)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_LEN                            (1U)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_MSK                            (((1ULL<<BZ_PHY_ZB_FSM_CCA_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_CCA_ENABLE_POS)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_UMSK                           (~(((1ULL<<BZ_PHY_ZB_FSM_CCA_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_CCA_ENABLE_POS))
#define BZ_PHY_ZB_FSM_ED_ENABLE                                 BZ_PHY_ZB_FSM_ED_ENABLE
#define BZ_PHY_ZB_FSM_ED_ENABLE_POS                             (28U)
#define BZ_PHY_ZB_FSM_ED_ENABLE_LEN                             (1U)
#define BZ_PHY_ZB_FSM_ED_ENABLE_MSK                             (((1ULL<<BZ_PHY_ZB_FSM_ED_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_ED_ENABLE_POS)
#define BZ_PHY_ZB_FSM_ED_ENABLE_UMSK                            (~(((1ULL<<BZ_PHY_ZB_FSM_ED_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_ED_ENABLE_POS))
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN                           BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS                       (29U)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_MSK                       (((1ULL<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_UMSK                      (~(((1ULL<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS))
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN                           BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS                       (30U)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_MSK                       (((1ULL<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN)-1)<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_UMSK                      (~(((1ULL<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN)-1)<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS))
#define BZ_PHY_ZB_FSM_EN                                        BZ_PHY_ZB_FSM_EN
#define BZ_PHY_ZB_FSM_EN_POS                                    (31U)
#define BZ_PHY_ZB_FSM_EN_LEN                                    (1U)
#define BZ_PHY_ZB_FSM_EN_MSK                                    (((1ULL<<BZ_PHY_ZB_FSM_EN_LEN)-1)<<BZ_PHY_ZB_FSM_EN_POS)
#define BZ_PHY_ZB_FSM_EN_UMSK                                   (~(((1ULL<<BZ_PHY_ZB_FSM_EN_LEN)-1)<<BZ_PHY_ZB_FSM_EN_POS))

/* 0x830 : phy_rx_zb_receiver_4 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_4_OFFSET                      (0x830)
#define BZ_PHY_ZB_FT_INIT_VAL                                   BZ_PHY_ZB_FT_INIT_VAL
#define BZ_PHY_ZB_FT_INIT_VAL_POS                               (0U)
#define BZ_PHY_ZB_FT_INIT_VAL_LEN                               (13U)
#define BZ_PHY_ZB_FT_INIT_VAL_MSK                               (((1ULL<<BZ_PHY_ZB_FT_INIT_VAL_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_POS)
#define BZ_PHY_ZB_FT_INIT_VAL_UMSK                              (~(((1ULL<<BZ_PHY_ZB_FT_INIT_VAL_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_POS))
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE                             BZ_PHY_ZB_FT_INIT_VAL_FORCE
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS                         (13U)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN                         (1U)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_MSK                         (((1ULL<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_UMSK                        (~(((1ULL<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS))
#define BZ_PHY_ZB_DM_ALPHA_2000                                 BZ_PHY_ZB_DM_ALPHA_2000
#define BZ_PHY_ZB_DM_ALPHA_2000_POS                             (14U)
#define BZ_PHY_ZB_DM_ALPHA_2000_LEN                             (4U)
#define BZ_PHY_ZB_DM_ALPHA_2000_MSK                             (((1ULL<<BZ_PHY_ZB_DM_ALPHA_2000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_2000_POS)
#define BZ_PHY_ZB_DM_ALPHA_2000_UMSK                            (~(((1ULL<<BZ_PHY_ZB_DM_ALPHA_2000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_2000_POS))
#define BZ_PHY_ZB_DM_ALPHA_1000                                 BZ_PHY_ZB_DM_ALPHA_1000
#define BZ_PHY_ZB_DM_ALPHA_1000_POS                             (18U)
#define BZ_PHY_ZB_DM_ALPHA_1000_LEN                             (4U)
#define BZ_PHY_ZB_DM_ALPHA_1000_MSK                             (((1ULL<<BZ_PHY_ZB_DM_ALPHA_1000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_1000_POS)
#define BZ_PHY_ZB_DM_ALPHA_1000_UMSK                            (~(((1ULL<<BZ_PHY_ZB_DM_ALPHA_1000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_1000_POS))
#define BZ_PHY_ZB_DM_ALPHA_500                                  BZ_PHY_ZB_DM_ALPHA_500
#define BZ_PHY_ZB_DM_ALPHA_500_POS                              (22U)
#define BZ_PHY_ZB_DM_ALPHA_500_LEN                              (4U)
#define BZ_PHY_ZB_DM_ALPHA_500_MSK                              (((1ULL<<BZ_PHY_ZB_DM_ALPHA_500_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_500_POS)
#define BZ_PHY_ZB_DM_ALPHA_500_UMSK                             (~(((1ULL<<BZ_PHY_ZB_DM_ALPHA_500_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_500_POS))
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS                           BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS                       (26U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN                       (5U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_MSK                       (((1ULL<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_UMSK                      (~(((1ULL<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS))
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN                           BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS                       (31U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_MSK                       (((1ULL<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_UMSK                      (~(((1ULL<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS))

/* 0x834 : phy_cca_0 */
#define BZ_PHY_PHY_CCA_0_OFFSET                                 (0x834)
#define BZ_PHY_CCA_ST_INT_SET                                   BZ_PHY_CCA_ST_INT_SET
#define BZ_PHY_CCA_ST_INT_SET_POS                               (0U)
#define BZ_PHY_CCA_ST_INT_SET_LEN                               (1U)
#define BZ_PHY_CCA_ST_INT_SET_MSK                               (((1ULL<<BZ_PHY_CCA_ST_INT_SET_LEN)-1)<<BZ_PHY_CCA_ST_INT_SET_POS)
#define BZ_PHY_CCA_ST_INT_SET_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_INT_SET_LEN)-1)<<BZ_PHY_CCA_ST_INT_SET_POS))
#define BZ_PHY_CCA_ST_INT_CLR                                   BZ_PHY_CCA_ST_INT_CLR
#define BZ_PHY_CCA_ST_INT_CLR_POS                               (1U)
#define BZ_PHY_CCA_ST_INT_CLR_LEN                               (1U)
#define BZ_PHY_CCA_ST_INT_CLR_MSK                               (((1ULL<<BZ_PHY_CCA_ST_INT_CLR_LEN)-1)<<BZ_PHY_CCA_ST_INT_CLR_POS)
#define BZ_PHY_CCA_ST_INT_CLR_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_INT_CLR_LEN)-1)<<BZ_PHY_CCA_ST_INT_CLR_POS))
#define BZ_PHY_CCA_ST_INT                                       BZ_PHY_CCA_ST_INT
#define BZ_PHY_CCA_ST_INT_POS                                   (2U)
#define BZ_PHY_CCA_ST_INT_LEN                                   (1U)
#define BZ_PHY_CCA_ST_INT_MSK                                   (((1ULL<<BZ_PHY_CCA_ST_INT_LEN)-1)<<BZ_PHY_CCA_ST_INT_POS)
#define BZ_PHY_CCA_ST_INT_UMSK                                  (~(((1ULL<<BZ_PHY_CCA_ST_INT_LEN)-1)<<BZ_PHY_CCA_ST_INT_POS))
#define BZ_PHY_CCA_ST_DM_FLAG                                   BZ_PHY_CCA_ST_DM_FLAG
#define BZ_PHY_CCA_ST_DM_FLAG_POS                               (4U)
#define BZ_PHY_CCA_ST_DM_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_DM_FLAG_MSK                               (((1ULL<<BZ_PHY_CCA_ST_DM_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_DM_FLAG_POS)
#define BZ_PHY_CCA_ST_DM_FLAG_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_DM_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_DM_FLAG_POS))
#define BZ_PHY_CCA_ST_CS_FLAG                                   BZ_PHY_CCA_ST_CS_FLAG
#define BZ_PHY_CCA_ST_CS_FLAG_POS                               (5U)
#define BZ_PHY_CCA_ST_CS_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_CS_FLAG_MSK                               (((1ULL<<BZ_PHY_CCA_ST_CS_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CS_FLAG_POS)
#define BZ_PHY_CCA_ST_CS_FLAG_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_CS_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CS_FLAG_POS))
#define BZ_PHY_CCA_ST_ED_FLAG                                   BZ_PHY_CCA_ST_ED_FLAG
#define BZ_PHY_CCA_ST_ED_FLAG_POS                               (6U)
#define BZ_PHY_CCA_ST_ED_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_ED_FLAG_MSK                               (((1ULL<<BZ_PHY_CCA_ST_ED_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_ED_FLAG_POS)
#define BZ_PHY_CCA_ST_ED_FLAG_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_ED_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_ED_FLAG_POS))
#define BZ_PHY_CCA_ST_CCA_FLAG                                  BZ_PHY_CCA_ST_CCA_FLAG
#define BZ_PHY_CCA_ST_CCA_FLAG_POS                              (7U)
#define BZ_PHY_CCA_ST_CCA_FLAG_LEN                              (1U)
#define BZ_PHY_CCA_ST_CCA_FLAG_MSK                              (((1ULL<<BZ_PHY_CCA_ST_CCA_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CCA_FLAG_POS)
#define BZ_PHY_CCA_ST_CCA_FLAG_UMSK                             (~(((1ULL<<BZ_PHY_CCA_ST_CCA_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CCA_FLAG_POS))
#define BZ_PHY_CCA_ST_ED_MEAS                                   BZ_PHY_CCA_ST_ED_MEAS
#define BZ_PHY_CCA_ST_ED_MEAS_POS                               (8U)
#define BZ_PHY_CCA_ST_ED_MEAS_LEN                               (7U)
#define BZ_PHY_CCA_ST_ED_MEAS_MSK                               (((1ULL<<BZ_PHY_CCA_ST_ED_MEAS_LEN)-1)<<BZ_PHY_CCA_ST_ED_MEAS_POS)
#define BZ_PHY_CCA_ST_ED_MEAS_UMSK                              (~(((1ULL<<BZ_PHY_CCA_ST_ED_MEAS_LEN)-1)<<BZ_PHY_CCA_ST_ED_MEAS_POS))
#define BZ_PHY_PHY_SW_CCA_EN_SEL                                BZ_PHY_PHY_SW_CCA_EN_SEL
#define BZ_PHY_PHY_SW_CCA_EN_SEL_POS                            (15U)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_LEN                            (1U)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_MSK                            (((1ULL<<BZ_PHY_PHY_SW_CCA_EN_SEL_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_SEL_POS)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_UMSK                           (~(((1ULL<<BZ_PHY_PHY_SW_CCA_EN_SEL_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_SEL_POS))
#define BZ_PHY_PHY_CCA_ED_TH                                    BZ_PHY_PHY_CCA_ED_TH
#define BZ_PHY_PHY_CCA_ED_TH_POS                                (16U)
#define BZ_PHY_PHY_CCA_ED_TH_LEN                                (7U)
#define BZ_PHY_PHY_CCA_ED_TH_MSK                                (((1ULL<<BZ_PHY_PHY_CCA_ED_TH_LEN)-1)<<BZ_PHY_PHY_CCA_ED_TH_POS)
#define BZ_PHY_PHY_CCA_ED_TH_UMSK                               (~(((1ULL<<BZ_PHY_PHY_CCA_ED_TH_LEN)-1)<<BZ_PHY_PHY_CCA_ED_TH_POS))
#define BZ_PHY_PHY_CCA_FOUND_SRC                                BZ_PHY_PHY_CCA_FOUND_SRC
#define BZ_PHY_PHY_CCA_FOUND_SRC_POS                            (23U)
#define BZ_PHY_PHY_CCA_FOUND_SRC_LEN                            (1U)
#define BZ_PHY_PHY_CCA_FOUND_SRC_MSK                            (((1ULL<<BZ_PHY_PHY_CCA_FOUND_SRC_LEN)-1)<<BZ_PHY_PHY_CCA_FOUND_SRC_POS)
#define BZ_PHY_PHY_CCA_FOUND_SRC_UMSK                           (~(((1ULL<<BZ_PHY_PHY_CCA_FOUND_SRC_LEN)-1)<<BZ_PHY_PHY_CCA_FOUND_SRC_POS))
#define BZ_PHY_PHY_CCA_DUR                                      BZ_PHY_PHY_CCA_DUR
#define BZ_PHY_PHY_CCA_DUR_POS                                  (24U)
#define BZ_PHY_PHY_CCA_DUR_LEN                                  (3U)
#define BZ_PHY_PHY_CCA_DUR_MSK                                  (((1ULL<<BZ_PHY_PHY_CCA_DUR_LEN)-1)<<BZ_PHY_PHY_CCA_DUR_POS)
#define BZ_PHY_PHY_CCA_DUR_UMSK                                 (~(((1ULL<<BZ_PHY_PHY_CCA_DUR_LEN)-1)<<BZ_PHY_PHY_CCA_DUR_POS))
#define BZ_PHY_PHY_CCA_MODE                                     BZ_PHY_PHY_CCA_MODE
#define BZ_PHY_PHY_CCA_MODE_POS                                 (27U)
#define BZ_PHY_PHY_CCA_MODE_LEN                                 (2U)
#define BZ_PHY_PHY_CCA_MODE_MSK                                 (((1ULL<<BZ_PHY_PHY_CCA_MODE_LEN)-1)<<BZ_PHY_PHY_CCA_MODE_POS)
#define BZ_PHY_PHY_CCA_MODE_UMSK                                (~(((1ULL<<BZ_PHY_PHY_CCA_MODE_LEN)-1)<<BZ_PHY_PHY_CCA_MODE_POS))
#define BZ_PHY_PHY_CCA_EN                                       BZ_PHY_PHY_CCA_EN
#define BZ_PHY_PHY_CCA_EN_POS                                   (29U)
#define BZ_PHY_PHY_CCA_EN_LEN                                   (1U)
#define BZ_PHY_PHY_CCA_EN_MSK                                   (((1ULL<<BZ_PHY_PHY_CCA_EN_LEN)-1)<<BZ_PHY_PHY_CCA_EN_POS)
#define BZ_PHY_PHY_CCA_EN_UMSK                                  (~(((1ULL<<BZ_PHY_PHY_CCA_EN_LEN)-1)<<BZ_PHY_PHY_CCA_EN_POS))
#define BZ_PHY_PHY_SW_CCA_EN                                    BZ_PHY_PHY_SW_CCA_EN
#define BZ_PHY_PHY_SW_CCA_EN_POS                                (30U)
#define BZ_PHY_PHY_SW_CCA_EN_LEN                                (1U)
#define BZ_PHY_PHY_SW_CCA_EN_MSK                                (((1ULL<<BZ_PHY_PHY_SW_CCA_EN_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_POS)
#define BZ_PHY_PHY_SW_CCA_EN_UMSK                               (~(((1ULL<<BZ_PHY_PHY_SW_CCA_EN_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_POS))
#define BZ_PHY_PHY_SW_ED_EN                                     BZ_PHY_PHY_SW_ED_EN
#define BZ_PHY_PHY_SW_ED_EN_POS                                 (31U)
#define BZ_PHY_PHY_SW_ED_EN_LEN                                 (1U)
#define BZ_PHY_PHY_SW_ED_EN_MSK                                 (((1ULL<<BZ_PHY_PHY_SW_ED_EN_LEN)-1)<<BZ_PHY_PHY_SW_ED_EN_POS)
#define BZ_PHY_PHY_SW_ED_EN_UMSK                                (~(((1ULL<<BZ_PHY_PHY_SW_ED_EN_LEN)-1)<<BZ_PHY_PHY_SW_ED_EN_POS))

/* 0x838 : phy_ble_sync_0 */
#define BZ_PHY_PHY_BLE_SYNC_0_OFFSET                            (0x838)
#define BZ_PHY_T2F_MAG_TH                                       BZ_PHY_T2F_MAG_TH
#define BZ_PHY_T2F_MAG_TH_POS                                   (0U)
#define BZ_PHY_T2F_MAG_TH_LEN                                   (9U)
#define BZ_PHY_T2F_MAG_TH_MSK                                   (((1ULL<<BZ_PHY_T2F_MAG_TH_LEN)-1)<<BZ_PHY_T2F_MAG_TH_POS)
#define BZ_PHY_T2F_MAG_TH_UMSK                                  (~(((1ULL<<BZ_PHY_T2F_MAG_TH_LEN)-1)<<BZ_PHY_T2F_MAG_TH_POS))
#define BZ_PHY_BLEC_CFOE_ALPHA                                  BZ_PHY_BLEC_CFOE_ALPHA
#define BZ_PHY_BLEC_CFOE_ALPHA_POS                              (10U)
#define BZ_PHY_BLEC_CFOE_ALPHA_LEN                              (3U)
#define BZ_PHY_BLEC_CFOE_ALPHA_MSK                              (((1ULL<<BZ_PHY_BLEC_CFOE_ALPHA_LEN)-1)<<BZ_PHY_BLEC_CFOE_ALPHA_POS)
#define BZ_PHY_BLEC_CFOE_ALPHA_UMSK                             (~(((1ULL<<BZ_PHY_BLEC_CFOE_ALPHA_LEN)-1)<<BZ_PHY_BLEC_CFOE_ALPHA_POS))
#define BZ_PHY_BLE_SYNC_CORR_AVG_L                              BZ_PHY_BLE_SYNC_CORR_AVG_L
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_POS                          (13U)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN                          (5U)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_MSK                          (((1ULL<<BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN)-1)<<BZ_PHY_BLE_SYNC_CORR_AVG_L_POS)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_UMSK                         (~(((1ULL<<BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN)-1)<<BZ_PHY_BLE_SYNC_CORR_AVG_L_POS))
#define BZ_PHY_BLE_CFOE_AVG_L                                   BZ_PHY_BLE_CFOE_AVG_L
#define BZ_PHY_BLE_CFOE_AVG_L_POS                               (18U)
#define BZ_PHY_BLE_CFOE_AVG_L_LEN                               (4U)
#define BZ_PHY_BLE_CFOE_AVG_L_MSK                               (((1ULL<<BZ_PHY_BLE_CFOE_AVG_L_LEN)-1)<<BZ_PHY_BLE_CFOE_AVG_L_POS)
#define BZ_PHY_BLE_CFOE_AVG_L_UMSK                              (~(((1ULL<<BZ_PHY_BLE_CFOE_AVG_L_LEN)-1)<<BZ_PHY_BLE_CFOE_AVG_L_POS))
#define BZ_PHY_BLE_AA_OFFSET_COEF                               BZ_PHY_BLE_AA_OFFSET_COEF
#define BZ_PHY_BLE_AA_OFFSET_COEF_POS                           (22U)
#define BZ_PHY_BLE_AA_OFFSET_COEF_LEN                           (5U)
#define BZ_PHY_BLE_AA_OFFSET_COEF_MSK                           (((1ULL<<BZ_PHY_BLE_AA_OFFSET_COEF_LEN)-1)<<BZ_PHY_BLE_AA_OFFSET_COEF_POS)
#define BZ_PHY_BLE_AA_OFFSET_COEF_UMSK                          (~(((1ULL<<BZ_PHY_BLE_AA_OFFSET_COEF_LEN)-1)<<BZ_PHY_BLE_AA_OFFSET_COEF_POS))
#define BZ_PHY_BLE_AA_ERROR_MAX                                 BZ_PHY_BLE_AA_ERROR_MAX
#define BZ_PHY_BLE_AA_ERROR_MAX_POS                             (27U)
#define BZ_PHY_BLE_AA_ERROR_MAX_LEN                             (5U)
#define BZ_PHY_BLE_AA_ERROR_MAX_MSK                             (((1ULL<<BZ_PHY_BLE_AA_ERROR_MAX_LEN)-1)<<BZ_PHY_BLE_AA_ERROR_MAX_POS)
#define BZ_PHY_BLE_AA_ERROR_MAX_UMSK                            (~(((1ULL<<BZ_PHY_BLE_AA_ERROR_MAX_LEN)-1)<<BZ_PHY_BLE_AA_ERROR_MAX_POS))

/* 0x83C : phy_ble_sync_1 */
#define BZ_PHY_PHY_BLE_SYNC_1_OFFSET                            (0x83C)
#define BZ_PHY_T2F_LIMITE_TH                                    BZ_PHY_T2F_LIMITE_TH
#define BZ_PHY_T2F_LIMITE_TH_POS                                (0U)
#define BZ_PHY_T2F_LIMITE_TH_LEN                                (13U)
#define BZ_PHY_T2F_LIMITE_TH_MSK                                (((1ULL<<BZ_PHY_T2F_LIMITE_TH_LEN)-1)<<BZ_PHY_T2F_LIMITE_TH_POS)
#define BZ_PHY_T2F_LIMITE_TH_UMSK                               (~(((1ULL<<BZ_PHY_T2F_LIMITE_TH_LEN)-1)<<BZ_PHY_T2F_LIMITE_TH_POS))
#define BZ_PHY_T2F_IMPULSE_TH                                   BZ_PHY_T2F_IMPULSE_TH
#define BZ_PHY_T2F_IMPULSE_TH_POS                               (13U)
#define BZ_PHY_T2F_IMPULSE_TH_LEN                               (14U)
#define BZ_PHY_T2F_IMPULSE_TH_MSK                               (((1ULL<<BZ_PHY_T2F_IMPULSE_TH_LEN)-1)<<BZ_PHY_T2F_IMPULSE_TH_POS)
#define BZ_PHY_T2F_IMPULSE_TH_UMSK                              (~(((1ULL<<BZ_PHY_T2F_IMPULSE_TH_LEN)-1)<<BZ_PHY_T2F_IMPULSE_TH_POS))
#define BZ_PHY_T2F_FILTER_MODE                                  BZ_PHY_T2F_FILTER_MODE
#define BZ_PHY_T2F_FILTER_MODE_POS                              (27U)
#define BZ_PHY_T2F_FILTER_MODE_LEN                              (1U)
#define BZ_PHY_T2F_FILTER_MODE_MSK                              (((1ULL<<BZ_PHY_T2F_FILTER_MODE_LEN)-1)<<BZ_PHY_T2F_FILTER_MODE_POS)
#define BZ_PHY_T2F_FILTER_MODE_UMSK                             (~(((1ULL<<BZ_PHY_T2F_FILTER_MODE_LEN)-1)<<BZ_PHY_T2F_FILTER_MODE_POS))
#define BZ_PHY_T2F_FILTER_EN                                    BZ_PHY_T2F_FILTER_EN
#define BZ_PHY_T2F_FILTER_EN_POS                                (28U)
#define BZ_PHY_T2F_FILTER_EN_LEN                                (1U)
#define BZ_PHY_T2F_FILTER_EN_MSK                                (((1ULL<<BZ_PHY_T2F_FILTER_EN_LEN)-1)<<BZ_PHY_T2F_FILTER_EN_POS)
#define BZ_PHY_T2F_FILTER_EN_UMSK                               (~(((1ULL<<BZ_PHY_T2F_FILTER_EN_LEN)-1)<<BZ_PHY_T2F_FILTER_EN_POS))

/* 0x840 : phy_reserved_0 */
#define BZ_PHY_PHY_RESERVED_0_OFFSET                            (0x840)
#define BZ_PHY_PHY_RESERVED_0                                   BZ_PHY_PHY_RESERVED_0
#define BZ_PHY_PHY_RESERVED_0_POS                               (0U)
#define BZ_PHY_PHY_RESERVED_0_LEN                               (32U)
#define BZ_PHY_PHY_RESERVED_0_MSK                               (((1ULL<<BZ_PHY_PHY_RESERVED_0_LEN)-1)<<BZ_PHY_PHY_RESERVED_0_POS)
#define BZ_PHY_PHY_RESERVED_0_UMSK                              (~(((1ULL<<BZ_PHY_PHY_RESERVED_0_LEN)-1)<<BZ_PHY_PHY_RESERVED_0_POS))

/* 0x844 : phy_rx_proc_time_1 */
#define BZ_PHY_PHY_RX_PROC_TIME_1_OFFSET                        (0x844)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K                     BZ_PHY_RX_PROC_TIME_VITERBI_US_500K
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS                 (0U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN                 (8U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_MSK                 (((1ULL<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_UMSK                (~(((1ULL<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS))
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M                          BZ_PHY_RX_PROC_TIME_MLSD_US_2M
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_MSK                      (((1ULL<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_UMSK                     (~(((1ULL<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS))
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M                            BZ_PHY_RX_PROC_TIME_EQ_US_2M
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS                        (16U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN                        (8U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_MSK                        (((1ULL<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_UMSK                       (~(((1ULL<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS))
#define BZ_PHY_RX_PROC_TIME_BT_US                               BZ_PHY_RX_PROC_TIME_BT_US
#define BZ_PHY_RX_PROC_TIME_BT_US_POS                           (24U)
#define BZ_PHY_RX_PROC_TIME_BT_US_LEN                           (8U)
#define BZ_PHY_RX_PROC_TIME_BT_US_MSK                           (((1ULL<<BZ_PHY_RX_PROC_TIME_BT_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_BT_US_POS)
#define BZ_PHY_RX_PROC_TIME_BT_US_UMSK                          (~(((1ULL<<BZ_PHY_RX_PROC_TIME_BT_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_BT_US_POS))

/* 0x848 : phy_ble_dfe_0 */
#define BZ_PHY_PHY_BLE_DFE_0_OFFSET                             (0x848)
#define BZ_PHY_FB_COEF_0                                        BZ_PHY_FB_COEF_0
#define BZ_PHY_FB_COEF_0_POS                                    (0U)
#define BZ_PHY_FB_COEF_0_LEN                                    (8U)
#define BZ_PHY_FB_COEF_0_MSK                                    (((1ULL<<BZ_PHY_FB_COEF_0_LEN)-1)<<BZ_PHY_FB_COEF_0_POS)
#define BZ_PHY_FB_COEF_0_UMSK                                   (~(((1ULL<<BZ_PHY_FB_COEF_0_LEN)-1)<<BZ_PHY_FB_COEF_0_POS))
#define BZ_PHY_FF_COEF_2                                        BZ_PHY_FF_COEF_2
#define BZ_PHY_FF_COEF_2_POS                                    (8U)
#define BZ_PHY_FF_COEF_2_LEN                                    (8U)
#define BZ_PHY_FF_COEF_2_MSK                                    (((1ULL<<BZ_PHY_FF_COEF_2_LEN)-1)<<BZ_PHY_FF_COEF_2_POS)
#define BZ_PHY_FF_COEF_2_UMSK                                   (~(((1ULL<<BZ_PHY_FF_COEF_2_LEN)-1)<<BZ_PHY_FF_COEF_2_POS))
#define BZ_PHY_FF_COEF_1                                        BZ_PHY_FF_COEF_1
#define BZ_PHY_FF_COEF_1_POS                                    (16U)
#define BZ_PHY_FF_COEF_1_LEN                                    (8U)
#define BZ_PHY_FF_COEF_1_MSK                                    (((1ULL<<BZ_PHY_FF_COEF_1_LEN)-1)<<BZ_PHY_FF_COEF_1_POS)
#define BZ_PHY_FF_COEF_1_UMSK                                   (~(((1ULL<<BZ_PHY_FF_COEF_1_LEN)-1)<<BZ_PHY_FF_COEF_1_POS))
#define BZ_PHY_FF_COEF_0                                        BZ_PHY_FF_COEF_0
#define BZ_PHY_FF_COEF_0_POS                                    (24U)
#define BZ_PHY_FF_COEF_0_LEN                                    (8U)
#define BZ_PHY_FF_COEF_0_MSK                                    (((1ULL<<BZ_PHY_FF_COEF_0_LEN)-1)<<BZ_PHY_FF_COEF_0_POS)
#define BZ_PHY_FF_COEF_0_UMSK                                   (~(((1ULL<<BZ_PHY_FF_COEF_0_LEN)-1)<<BZ_PHY_FF_COEF_0_POS))

/* 0x84C : phy_ble_sync_2 */
#define BZ_PHY_PHY_BLE_SYNC_2_OFFSET                            (0x84C)
#define BZ_PHY_BLE_AA_CORR_THRES                                BZ_PHY_BLE_AA_CORR_THRES
#define BZ_PHY_BLE_AA_CORR_THRES_POS                            (0U)
#define BZ_PHY_BLE_AA_CORR_THRES_LEN                            (12U)
#define BZ_PHY_BLE_AA_CORR_THRES_MSK                            (((1ULL<<BZ_PHY_BLE_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLE_AA_CORR_THRES_POS)
#define BZ_PHY_BLE_AA_CORR_THRES_UMSK                           (~(((1ULL<<BZ_PHY_BLE_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLE_AA_CORR_THRES_POS))
#define BZ_PHY_BLE_SEARCH_OFFSET                                BZ_PHY_BLE_SEARCH_OFFSET
#define BZ_PHY_BLE_SEARCH_OFFSET_POS                            (12U)
#define BZ_PHY_BLE_SEARCH_OFFSET_LEN                            (5U)
#define BZ_PHY_BLE_SEARCH_OFFSET_MSK                            (((1ULL<<BZ_PHY_BLE_SEARCH_OFFSET_LEN)-1)<<BZ_PHY_BLE_SEARCH_OFFSET_POS)
#define BZ_PHY_BLE_SEARCH_OFFSET_UMSK                           (~(((1ULL<<BZ_PHY_BLE_SEARCH_OFFSET_LEN)-1)<<BZ_PHY_BLE_SEARCH_OFFSET_POS))

/* 0x850 : phy_ble_fsm_0 */
#define BZ_PHY_PHY_BLE_FSM_0_OFFSET                             (0x850)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB                              BZ_PHY_BLE_RX_CFO_ADJ_SYMB
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS                          (0U)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN                          (8U)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_MSK                          (((1ULL<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN)-1)<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_UMSK                         (~(((1ULL<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN)-1)<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS))
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M                             BZ_PHY_BLE_FSM_RX_DMTIME_2M
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_MSK                         (((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_UMSK                        (~(((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS))
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M                             BZ_PHY_BLE_FSM_RX_DMTIME_1M
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS                         (16U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_MSK                         (((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_UMSK                        (~(((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS))
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY                             BZ_PHY_BLE_FSM_CFO_PEAK_DLY
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS                         (24U)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN                         (4U)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_MSK                         (((1ULL<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN)-1)<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_UMSK                        (~(((1ULL<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN)-1)<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS))
#define BZ_PHY_BLE_DEMOD_MODE                                   BZ_PHY_BLE_DEMOD_MODE
#define BZ_PHY_BLE_DEMOD_MODE_POS                               (28U)
#define BZ_PHY_BLE_DEMOD_MODE_LEN                               (2U)
#define BZ_PHY_BLE_DEMOD_MODE_MSK                               (((1ULL<<BZ_PHY_BLE_DEMOD_MODE_LEN)-1)<<BZ_PHY_BLE_DEMOD_MODE_POS)
#define BZ_PHY_BLE_DEMOD_MODE_UMSK                              (~(((1ULL<<BZ_PHY_BLE_DEMOD_MODE_LEN)-1)<<BZ_PHY_BLE_DEMOD_MODE_POS))
#define BZ_PHY_BLE_FSM_CG_4S_ON                                 BZ_PHY_BLE_FSM_CG_4S_ON
#define BZ_PHY_BLE_FSM_CG_4S_ON_POS                             (30U)
#define BZ_PHY_BLE_FSM_CG_4S_ON_LEN                             (1U)
#define BZ_PHY_BLE_FSM_CG_4S_ON_MSK                             (((1ULL<<BZ_PHY_BLE_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_BLE_FSM_CG_4S_ON_POS)
#define BZ_PHY_BLE_FSM_CG_4S_ON_UMSK                            (~(((1ULL<<BZ_PHY_BLE_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_BLE_FSM_CG_4S_ON_POS))
#define BZ_PHY_BLE_FSM_EN                                       BZ_PHY_BLE_FSM_EN
#define BZ_PHY_BLE_FSM_EN_POS                                   (31U)
#define BZ_PHY_BLE_FSM_EN_LEN                                   (1U)
#define BZ_PHY_BLE_FSM_EN_MSK                                   (((1ULL<<BZ_PHY_BLE_FSM_EN_LEN)-1)<<BZ_PHY_BLE_FSM_EN_POS)
#define BZ_PHY_BLE_FSM_EN_UMSK                                  (~(((1ULL<<BZ_PHY_BLE_FSM_EN_LEN)-1)<<BZ_PHY_BLE_FSM_EN_POS))

/* 0x854 : phy_rx_proc_time_0 */
#define BZ_PHY_PHY_RX_PROC_TIME_0_OFFSET                        (0x854)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K                     BZ_PHY_RX_PROC_TIME_VITERBI_US_125K
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS                 (0U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN                 (8U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_MSK                 (((1ULL<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_UMSK                (~(((1ULL<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS))
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M                          BZ_PHY_RX_PROC_TIME_MLSD_US_1M
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_MSK                      (((1ULL<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_UMSK                     (~(((1ULL<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS))
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M                            BZ_PHY_RX_PROC_TIME_EQ_US_1M
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS                        (16U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN                        (8U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_MSK                        (((1ULL<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_UMSK                       (~(((1ULL<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS))
#define BZ_PHY_RX_PROC_TIME_M154_US                             BZ_PHY_RX_PROC_TIME_M154_US
#define BZ_PHY_RX_PROC_TIME_M154_US_POS                         (24U)
#define BZ_PHY_RX_PROC_TIME_M154_US_LEN                         (8U)
#define BZ_PHY_RX_PROC_TIME_M154_US_MSK                         (((1ULL<<BZ_PHY_RX_PROC_TIME_M154_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_M154_US_POS)
#define BZ_PHY_RX_PROC_TIME_M154_US_UMSK                        (~(((1ULL<<BZ_PHY_RX_PROC_TIME_M154_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_M154_US_POS))

/* 0x858 : phy_testbus_0 */
#define BZ_PHY_PHY_TESTBUS_0_OFFSET                             (0x858)
#define BZ_PHY_PHY_TESTBUS_READ                                 BZ_PHY_PHY_TESTBUS_READ
#define BZ_PHY_PHY_TESTBUS_READ_POS                             (0U)
#define BZ_PHY_PHY_TESTBUS_READ_LEN                             (32U)
#define BZ_PHY_PHY_TESTBUS_READ_MSK                             (((1ULL<<BZ_PHY_PHY_TESTBUS_READ_LEN)-1)<<BZ_PHY_PHY_TESTBUS_READ_POS)
#define BZ_PHY_PHY_TESTBUS_READ_UMSK                            (~(((1ULL<<BZ_PHY_PHY_TESTBUS_READ_LEN)-1)<<BZ_PHY_PHY_TESTBUS_READ_POS))

/* 0x85C : phy_testbus_1 */
#define BZ_PHY_PHY_TESTBUS_1_OFFSET                             (0x85C)
#define BZ_PHY_PHY_TESTBUS_SEL                                  BZ_PHY_PHY_TESTBUS_SEL
#define BZ_PHY_PHY_TESTBUS_SEL_POS                              (0U)
#define BZ_PHY_PHY_TESTBUS_SEL_LEN                              (4U)
#define BZ_PHY_PHY_TESTBUS_SEL_MSK                              (((1ULL<<BZ_PHY_PHY_TESTBUS_SEL_LEN)-1)<<BZ_PHY_PHY_TESTBUS_SEL_POS)
#define BZ_PHY_PHY_TESTBUS_SEL_UMSK                             (~(((1ULL<<BZ_PHY_PHY_TESTBUS_SEL_LEN)-1)<<BZ_PHY_PHY_TESTBUS_SEL_POS))

/* 0x860 : phy_status_0 */
#define BZ_PHY_PHY_STATUS_0_OFFSET                              (0x860)
#define BZ_PHY_STS_AGC_FE_DG                                    BZ_PHY_STS_AGC_FE_DG
#define BZ_PHY_STS_AGC_FE_DG_POS                                (0U)
#define BZ_PHY_STS_AGC_FE_DG_LEN                                (6U)
#define BZ_PHY_STS_AGC_FE_DG_MSK                                (((1ULL<<BZ_PHY_STS_AGC_FE_DG_LEN)-1)<<BZ_PHY_STS_AGC_FE_DG_POS)
#define BZ_PHY_STS_AGC_FE_DG_UMSK                               (~(((1ULL<<BZ_PHY_STS_AGC_FE_DG_LEN)-1)<<BZ_PHY_STS_AGC_FE_DG_POS))
#define BZ_PHY_STS_AGC_AGC_RSSI                                 BZ_PHY_STS_AGC_AGC_RSSI
#define BZ_PHY_STS_AGC_AGC_RSSI_POS                             (8U)
#define BZ_PHY_STS_AGC_AGC_RSSI_LEN                             (8U)
#define BZ_PHY_STS_AGC_AGC_RSSI_MSK                             (((1ULL<<BZ_PHY_STS_AGC_AGC_RSSI_LEN)-1)<<BZ_PHY_STS_AGC_AGC_RSSI_POS)
#define BZ_PHY_STS_AGC_AGC_RSSI_UMSK                            (~(((1ULL<<BZ_PHY_STS_AGC_AGC_RSSI_LEN)-1)<<BZ_PHY_STS_AGC_AGC_RSSI_POS))
#define BZ_PHY_STS_AGC_RF_RBB_IND                               BZ_PHY_STS_AGC_RF_RBB_IND
#define BZ_PHY_STS_AGC_RF_RBB_IND_POS                           (16U)
#define BZ_PHY_STS_AGC_RF_RBB_IND_LEN                           (5U)
#define BZ_PHY_STS_AGC_RF_RBB_IND_MSK                           (((1ULL<<BZ_PHY_STS_AGC_RF_RBB_IND_LEN)-1)<<BZ_PHY_STS_AGC_RF_RBB_IND_POS)
#define BZ_PHY_STS_AGC_RF_RBB_IND_UMSK                          (~(((1ULL<<BZ_PHY_STS_AGC_RF_RBB_IND_LEN)-1)<<BZ_PHY_STS_AGC_RF_RBB_IND_POS))
#define BZ_PHY_STS_AGC_RF_LNA                                   BZ_PHY_STS_AGC_RF_LNA
#define BZ_PHY_STS_AGC_RF_LNA_POS                               (21U)
#define BZ_PHY_STS_AGC_RF_LNA_LEN                               (4U)
#define BZ_PHY_STS_AGC_RF_LNA_MSK                               (((1ULL<<BZ_PHY_STS_AGC_RF_LNA_LEN)-1)<<BZ_PHY_STS_AGC_RF_LNA_POS)
#define BZ_PHY_STS_AGC_RF_LNA_UMSK                              (~(((1ULL<<BZ_PHY_STS_AGC_RF_LNA_LEN)-1)<<BZ_PHY_STS_AGC_RF_LNA_POS))
#define BZ_PHY_STS_AGC_CLEAR                                    BZ_PHY_STS_AGC_CLEAR
#define BZ_PHY_STS_AGC_CLEAR_POS                                (31U)
#define BZ_PHY_STS_AGC_CLEAR_LEN                                (1U)
#define BZ_PHY_STS_AGC_CLEAR_MSK                                (((1ULL<<BZ_PHY_STS_AGC_CLEAR_LEN)-1)<<BZ_PHY_STS_AGC_CLEAR_POS)
#define BZ_PHY_STS_AGC_CLEAR_UMSK                               (~(((1ULL<<BZ_PHY_STS_AGC_CLEAR_LEN)-1)<<BZ_PHY_STS_AGC_CLEAR_POS))

/* 0x864 : phy_status_1 */
#define BZ_PHY_PHY_STATUS_1_OFFSET                              (0x864)
#define BZ_PHY_STS_ZB_SFD_CORR                                  BZ_PHY_STS_ZB_SFD_CORR
#define BZ_PHY_STS_ZB_SFD_CORR_POS                              (0U)
#define BZ_PHY_STS_ZB_SFD_CORR_LEN                              (15U)
#define BZ_PHY_STS_ZB_SFD_CORR_MSK                              (((1ULL<<BZ_PHY_STS_ZB_SFD_CORR_LEN)-1)<<BZ_PHY_STS_ZB_SFD_CORR_POS)
#define BZ_PHY_STS_ZB_SFD_CORR_UMSK                             (~(((1ULL<<BZ_PHY_STS_ZB_SFD_CORR_LEN)-1)<<BZ_PHY_STS_ZB_SFD_CORR_POS))
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT                              BZ_PHY_STS_ZB_FREQ_TRK_OUT
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS                          (16U)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN                          (13U)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_MSK                          (((1ULL<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_UMSK                         (~(((1ULL<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS))
#define BZ_PHY_STS_ZB_SFD_MODE                                  BZ_PHY_STS_ZB_SFD_MODE
#define BZ_PHY_STS_ZB_SFD_MODE_POS                              (29U)
#define BZ_PHY_STS_ZB_SFD_MODE_LEN                              (2U)
#define BZ_PHY_STS_ZB_SFD_MODE_MSK                              (((1ULL<<BZ_PHY_STS_ZB_SFD_MODE_LEN)-1)<<BZ_PHY_STS_ZB_SFD_MODE_POS)
#define BZ_PHY_STS_ZB_SFD_MODE_UMSK                             (~(((1ULL<<BZ_PHY_STS_ZB_SFD_MODE_LEN)-1)<<BZ_PHY_STS_ZB_SFD_MODE_POS))
#define BZ_PHY_STS_ZB_CLEAR                                     BZ_PHY_STS_ZB_CLEAR
#define BZ_PHY_STS_ZB_CLEAR_POS                                 (31U)
#define BZ_PHY_STS_ZB_CLEAR_LEN                                 (1U)
#define BZ_PHY_STS_ZB_CLEAR_MSK                                 (((1ULL<<BZ_PHY_STS_ZB_CLEAR_LEN)-1)<<BZ_PHY_STS_ZB_CLEAR_POS)
#define BZ_PHY_STS_ZB_CLEAR_UMSK                                (~(((1ULL<<BZ_PHY_STS_ZB_CLEAR_LEN)-1)<<BZ_PHY_STS_ZB_CLEAR_POS))

/* 0x868 : phy_status_2 */
#define BZ_PHY_PHY_STATUS_2_OFFSET                              (0x868)
#define BZ_PHY_STS_ZB_PEAK_VAL_2                                BZ_PHY_STS_ZB_PEAK_VAL_2
#define BZ_PHY_STS_ZB_PEAK_VAL_2_POS                            (0U)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_LEN                            (14U)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_MSK                            (((1ULL<<BZ_PHY_STS_ZB_PEAK_VAL_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_2_POS)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_UMSK                           (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_VAL_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_2_POS))
#define BZ_PHY_STS_ZB_PEAK_VAL_1                                BZ_PHY_STS_ZB_PEAK_VAL_1
#define BZ_PHY_STS_ZB_PEAK_VAL_1_POS                            (16U)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_LEN                            (14U)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_MSK                            (((1ULL<<BZ_PHY_STS_ZB_PEAK_VAL_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_1_POS)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_UMSK                           (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_VAL_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_1_POS))

/* 0x86C : phy_status_3 */
#define BZ_PHY_PHY_STATUS_3_OFFSET                              (0x86C)
#define BZ_PHY_STS_ZB_PEAK_LOC_2                                BZ_PHY_STS_ZB_PEAK_LOC_2
#define BZ_PHY_STS_ZB_PEAK_LOC_2_POS                            (0U)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_LEN                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_MSK                            (((1ULL<<BZ_PHY_STS_ZB_PEAK_LOC_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_2_POS)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_UMSK                           (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_LOC_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_2_POS))
#define BZ_PHY_STS_ZB_PEAK_LOC_1                                BZ_PHY_STS_ZB_PEAK_LOC_1
#define BZ_PHY_STS_ZB_PEAK_LOC_1_POS                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_LEN                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_MSK                            (((1ULL<<BZ_PHY_STS_ZB_PEAK_LOC_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_1_POS)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_UMSK                           (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_LOC_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_1_POS))

/* 0x870 : phy_status_4 */
#define BZ_PHY_PHY_STATUS_4_OFFSET                              (0x870)
#define BZ_PHY_STS_ZB_PEAK_IM                                   BZ_PHY_STS_ZB_PEAK_IM
#define BZ_PHY_STS_ZB_PEAK_IM_POS                               (0U)
#define BZ_PHY_STS_ZB_PEAK_IM_LEN                               (12U)
#define BZ_PHY_STS_ZB_PEAK_IM_MSK                               (((1ULL<<BZ_PHY_STS_ZB_PEAK_IM_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_IM_POS)
#define BZ_PHY_STS_ZB_PEAK_IM_UMSK                              (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_IM_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_IM_POS))
#define BZ_PHY_STS_ZB_PEAK_RE                                   BZ_PHY_STS_ZB_PEAK_RE
#define BZ_PHY_STS_ZB_PEAK_RE_POS                               (16U)
#define BZ_PHY_STS_ZB_PEAK_RE_LEN                               (12U)
#define BZ_PHY_STS_ZB_PEAK_RE_MSK                               (((1ULL<<BZ_PHY_STS_ZB_PEAK_RE_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_RE_POS)
#define BZ_PHY_STS_ZB_PEAK_RE_UMSK                              (~(((1ULL<<BZ_PHY_STS_ZB_PEAK_RE_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_RE_POS))

/* 0x874 : phy_status_5 */
#define BZ_PHY_PHY_STATUS_5_OFFSET                              (0x874)
#define BZ_PHY_STS_ZB_CURR_LOC                                  BZ_PHY_STS_ZB_CURR_LOC
#define BZ_PHY_STS_ZB_CURR_LOC_POS                              (0U)
#define BZ_PHY_STS_ZB_CURR_LOC_LEN                              (8U)
#define BZ_PHY_STS_ZB_CURR_LOC_MSK                              (((1ULL<<BZ_PHY_STS_ZB_CURR_LOC_LEN)-1)<<BZ_PHY_STS_ZB_CURR_LOC_POS)
#define BZ_PHY_STS_ZB_CURR_LOC_UMSK                             (~(((1ULL<<BZ_PHY_STS_ZB_CURR_LOC_LEN)-1)<<BZ_PHY_STS_ZB_CURR_LOC_POS))
#define BZ_PHY_STS_ZB_CURR_MAX                                  BZ_PHY_STS_ZB_CURR_MAX
#define BZ_PHY_STS_ZB_CURR_MAX_POS                              (16U)
#define BZ_PHY_STS_ZB_CURR_MAX_LEN                              (14U)
#define BZ_PHY_STS_ZB_CURR_MAX_MSK                              (((1ULL<<BZ_PHY_STS_ZB_CURR_MAX_LEN)-1)<<BZ_PHY_STS_ZB_CURR_MAX_POS)
#define BZ_PHY_STS_ZB_CURR_MAX_UMSK                             (~(((1ULL<<BZ_PHY_STS_ZB_CURR_MAX_LEN)-1)<<BZ_PHY_STS_ZB_CURR_MAX_POS))

/* 0x878 : phy_status_6 */
#define BZ_PHY_PHY_STATUS_6_OFFSET                              (0x878)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT                         BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS                     (0U)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN                     (13U)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_MSK                     (((1ULL<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_UMSK                    (~(((1ULL<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS))
#define BZ_PHY_STS_ZB_TS_CS_FOUND                               BZ_PHY_STS_ZB_TS_CS_FOUND
#define BZ_PHY_STS_ZB_TS_CS_FOUND_POS                           (13U)
#define BZ_PHY_STS_ZB_TS_CS_FOUND_LEN                           (1U)
#define BZ_PHY_STS_ZB_TS_CS_FOUND_MSK                           (((1ULL<<BZ_PHY_STS_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_STS_ZB_TS_CS_FOUND_POS)
#define BZ_PHY_STS_ZB_TS_CS_FOUND_UMSK                          (~(((1ULL<<BZ_PHY_STS_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_STS_ZB_TS_CS_FOUND_POS))

/* 0x87C : phy_status_7 */
#define BZ_PHY_PHY_STATUS_7_OFFSET                              (0x87C)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK                             BZ_PHY_STS_BLE_CFO_OUT_PEAK
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS                         (0U)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN                         (8U)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_MSK                         (((1ULL<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_UMSK                        (~(((1ULL<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS))
#define BZ_PHY_STS_BLE_FRTK_OUT                                 BZ_PHY_STS_BLE_FRTK_OUT
#define BZ_PHY_STS_BLE_FRTK_OUT_POS                             (8U)
#define BZ_PHY_STS_BLE_FRTK_OUT_LEN                             (13U)
#define BZ_PHY_STS_BLE_FRTK_OUT_MSK                             (((1ULL<<BZ_PHY_STS_BLE_FRTK_OUT_LEN)-1)<<BZ_PHY_STS_BLE_FRTK_OUT_POS)
#define BZ_PHY_STS_BLE_FRTK_OUT_UMSK                            (~(((1ULL<<BZ_PHY_STS_BLE_FRTK_OUT_LEN)-1)<<BZ_PHY_STS_BLE_FRTK_OUT_POS))
#define BZ_PHY_STS_BLE_CLEAR                                    BZ_PHY_STS_BLE_CLEAR
#define BZ_PHY_STS_BLE_CLEAR_POS                                (31U)
#define BZ_PHY_STS_BLE_CLEAR_LEN                                (1U)
#define BZ_PHY_STS_BLE_CLEAR_MSK                                (((1ULL<<BZ_PHY_STS_BLE_CLEAR_LEN)-1)<<BZ_PHY_STS_BLE_CLEAR_POS)
#define BZ_PHY_STS_BLE_CLEAR_UMSK                               (~(((1ULL<<BZ_PHY_STS_BLE_CLEAR_LEN)-1)<<BZ_PHY_STS_BLE_CLEAR_POS))

/* 0x880 : phy_status_8 */
#define BZ_PHY_PHY_STATUS_8_OFFSET                              (0x880)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS                          BZ_PHY_STS_BLE_AA_CONFIRM_BITS
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS                      (0U)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN                      (32U)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_MSK                      (((1ULL<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN)-1)<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_UMSK                     (~(((1ULL<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN)-1)<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS))

/* 0x884 : phy_status_9 */
#define BZ_PHY_PHY_STATUS_9_OFFSET                              (0x884)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK                        BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS                    (0U)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN                    (8U)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_MSK                    (((1ULL<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_UMSK                   (~(((1ULL<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS))
#define BZ_PHY_STS_BLE_CORR_OUT_MAX                             BZ_PHY_STS_BLE_CORR_OUT_MAX
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_POS                         (8U)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN                         (12U)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_MSK                         (((1ULL<<BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN)-1)<<BZ_PHY_STS_BLE_CORR_OUT_MAX_POS)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_UMSK                        (~(((1ULL<<BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN)-1)<<BZ_PHY_STS_BLE_CORR_OUT_MAX_POS))
#define BZ_PHY_STS_BLE_AA_ERROR_CNT                             BZ_PHY_STS_BLE_AA_ERROR_CNT
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_POS                         (20U)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN                         (6U)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_MSK                         (((1ULL<<BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN)-1)<<BZ_PHY_STS_BLE_AA_ERROR_CNT_POS)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_UMSK                        (~(((1ULL<<BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN)-1)<<BZ_PHY_STS_BLE_AA_ERROR_CNT_POS))

/* 0x888 : phy_status_10 */
#define BZ_PHY_PHY_STATUS_10_OFFSET                             (0x888)
#define BZ_PHY_ZB_FSM_ERR_STS                                   BZ_PHY_ZB_FSM_ERR_STS
#define BZ_PHY_ZB_FSM_ERR_STS_POS                               (0U)
#define BZ_PHY_ZB_FSM_ERR_STS_LEN                               (8U)
#define BZ_PHY_ZB_FSM_ERR_STS_MSK                               (((1ULL<<BZ_PHY_ZB_FSM_ERR_STS_LEN)-1)<<BZ_PHY_ZB_FSM_ERR_STS_POS)
#define BZ_PHY_ZB_FSM_ERR_STS_UMSK                              (~(((1ULL<<BZ_PHY_ZB_FSM_ERR_STS_LEN)-1)<<BZ_PHY_ZB_FSM_ERR_STS_POS))
#define BZ_PHY_ZB_FSM_DBG_EN                                    BZ_PHY_ZB_FSM_DBG_EN
#define BZ_PHY_ZB_FSM_DBG_EN_POS                                (8U)
#define BZ_PHY_ZB_FSM_DBG_EN_LEN                                (1U)
#define BZ_PHY_ZB_FSM_DBG_EN_MSK                                (((1ULL<<BZ_PHY_ZB_FSM_DBG_EN_LEN)-1)<<BZ_PHY_ZB_FSM_DBG_EN_POS)
#define BZ_PHY_ZB_FSM_DBG_EN_UMSK                               (~(((1ULL<<BZ_PHY_ZB_FSM_DBG_EN_LEN)-1)<<BZ_PHY_ZB_FSM_DBG_EN_POS))
#define BZ_PHY_ZB_TS_CS_FOUND                                   BZ_PHY_ZB_TS_CS_FOUND
#define BZ_PHY_ZB_TS_CS_FOUND_POS                               (9U)
#define BZ_PHY_ZB_TS_CS_FOUND_LEN                               (1U)
#define BZ_PHY_ZB_TS_CS_FOUND_MSK                               (((1ULL<<BZ_PHY_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_ZB_TS_CS_FOUND_POS)
#define BZ_PHY_ZB_TS_CS_FOUND_UMSK                              (~(((1ULL<<BZ_PHY_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_ZB_TS_CS_FOUND_POS))
#define BZ_PHY_STS_ZB_SFD_FOUND                                 BZ_PHY_STS_ZB_SFD_FOUND
#define BZ_PHY_STS_ZB_SFD_FOUND_POS                             (10U)
#define BZ_PHY_STS_ZB_SFD_FOUND_LEN                             (1U)
#define BZ_PHY_STS_ZB_SFD_FOUND_MSK                             (((1ULL<<BZ_PHY_STS_ZB_SFD_FOUND_LEN)-1)<<BZ_PHY_STS_ZB_SFD_FOUND_POS)
#define BZ_PHY_STS_ZB_SFD_FOUND_UMSK                            (~(((1ULL<<BZ_PHY_STS_ZB_SFD_FOUND_LEN)-1)<<BZ_PHY_STS_ZB_SFD_FOUND_POS))
#define BZ_PHY_STS_ZB_PHY_LEN                                   BZ_PHY_STS_ZB_PHY_LEN
#define BZ_PHY_STS_ZB_PHY_LEN_POS                               (16U)
#define BZ_PHY_STS_ZB_PHY_LEN_LEN                               (8U)
#define BZ_PHY_STS_ZB_PHY_LEN_MSK                               (((1ULL<<BZ_PHY_STS_ZB_PHY_LEN_LEN)-1)<<BZ_PHY_STS_ZB_PHY_LEN_POS)
#define BZ_PHY_STS_ZB_PHY_LEN_UMSK                              (~(((1ULL<<BZ_PHY_STS_ZB_PHY_LEN_LEN)-1)<<BZ_PHY_STS_ZB_PHY_LEN_POS))
#define BZ_PHY_ZB_DM_WHITE_INIT                                 BZ_PHY_ZB_DM_WHITE_INIT
#define BZ_PHY_ZB_DM_WHITE_INIT_POS                             (24U)
#define BZ_PHY_ZB_DM_WHITE_INIT_LEN                             (7U)
#define BZ_PHY_ZB_DM_WHITE_INIT_MSK                             (((1ULL<<BZ_PHY_ZB_DM_WHITE_INIT_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_INIT_POS)
#define BZ_PHY_ZB_DM_WHITE_INIT_UMSK                            (~(((1ULL<<BZ_PHY_ZB_DM_WHITE_INIT_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_INIT_POS))
#define BZ_PHY_ZB_DM_WHITE_EN                                   BZ_PHY_ZB_DM_WHITE_EN
#define BZ_PHY_ZB_DM_WHITE_EN_POS                               (31U)
#define BZ_PHY_ZB_DM_WHITE_EN_LEN                               (1U)
#define BZ_PHY_ZB_DM_WHITE_EN_MSK                               (((1ULL<<BZ_PHY_ZB_DM_WHITE_EN_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_EN_POS)
#define BZ_PHY_ZB_DM_WHITE_EN_UMSK                              (~(((1ULL<<BZ_PHY_ZB_DM_WHITE_EN_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_EN_POS))

/* 0x88C : phy_ble_coded_0 */
#define BZ_PHY_PHY_BLE_CODED_0_OFFSET                           (0x88C)
#define BZ_PHY_BLEC_AA_SEARCH_START                             BZ_PHY_BLEC_AA_SEARCH_START
#define BZ_PHY_BLEC_AA_SEARCH_START_POS                         (0U)
#define BZ_PHY_BLEC_AA_SEARCH_START_LEN                         (5U)
#define BZ_PHY_BLEC_AA_SEARCH_START_MSK                         (((1ULL<<BZ_PHY_BLEC_AA_SEARCH_START_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_START_POS)
#define BZ_PHY_BLEC_AA_SEARCH_START_UMSK                        (~(((1ULL<<BZ_PHY_BLEC_AA_SEARCH_START_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_START_POS))
#define BZ_PHY_BLEC_PK_JUMP_THRES                               BZ_PHY_BLEC_PK_JUMP_THRES
#define BZ_PHY_BLEC_PK_JUMP_THRES_POS                           (5U)
#define BZ_PHY_BLEC_PK_JUMP_THRES_LEN                           (6U)
#define BZ_PHY_BLEC_PK_JUMP_THRES_MSK                           (((1ULL<<BZ_PHY_BLEC_PK_JUMP_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_JUMP_THRES_POS)
#define BZ_PHY_BLEC_PK_JUMP_THRES_UMSK                          (~(((1ULL<<BZ_PHY_BLEC_PK_JUMP_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_JUMP_THRES_POS))
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES                        BZ_PHY_BLEC_PK_TIMING_DIFF_THRES
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS                    (11U)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN                    (3U)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_MSK                    (((1ULL<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_UMSK                   (~(((1ULL<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS))
#define BZ_PHY_BLEC_PK_THRES                                    BZ_PHY_BLEC_PK_THRES
#define BZ_PHY_BLEC_PK_THRES_POS                                (14U)
#define BZ_PHY_BLEC_PK_THRES_LEN                                (9U)
#define BZ_PHY_BLEC_PK_THRES_MSK                                (((1ULL<<BZ_PHY_BLEC_PK_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_THRES_POS)
#define BZ_PHY_BLEC_PK_THRES_UMSK                               (~(((1ULL<<BZ_PHY_BLEC_PK_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_THRES_POS))
#define BZ_PHY_BLEC_PK_START_THRES                              BZ_PHY_BLEC_PK_START_THRES
#define BZ_PHY_BLEC_PK_START_THRES_POS                          (23U)
#define BZ_PHY_BLEC_PK_START_THRES_LEN                          (9U)
#define BZ_PHY_BLEC_PK_START_THRES_MSK                          (((1ULL<<BZ_PHY_BLEC_PK_START_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_START_THRES_POS)
#define BZ_PHY_BLEC_PK_START_THRES_UMSK                         (~(((1ULL<<BZ_PHY_BLEC_PK_START_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_START_THRES_POS))

/* 0x890 : phy_ble_coded_1 */
#define BZ_PHY_PHY_BLE_CODED_1_OFFSET                           (0x890)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD                             BZ_PHY_BLE_FSM_RX_DMTIME_CD
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS                         (0U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_MSK                         (((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_UMSK                        (~(((1ULL<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS))
#define BZ_PHY_BLEC_RST_THRES                                   BZ_PHY_BLEC_RST_THRES
#define BZ_PHY_BLEC_RST_THRES_POS                               (8U)
#define BZ_PHY_BLEC_RST_THRES_LEN                               (4U)
#define BZ_PHY_BLEC_RST_THRES_MSK                               (((1ULL<<BZ_PHY_BLEC_RST_THRES_LEN)-1)<<BZ_PHY_BLEC_RST_THRES_POS)
#define BZ_PHY_BLEC_RST_THRES_UMSK                              (~(((1ULL<<BZ_PHY_BLEC_RST_THRES_LEN)-1)<<BZ_PHY_BLEC_RST_THRES_POS))
#define BZ_PHY_BLEC_TIMING_ADJ                                  BZ_PHY_BLEC_TIMING_ADJ
#define BZ_PHY_BLEC_TIMING_ADJ_POS                              (12U)
#define BZ_PHY_BLEC_TIMING_ADJ_LEN                              (5U)
#define BZ_PHY_BLEC_TIMING_ADJ_MSK                              (((1ULL<<BZ_PHY_BLEC_TIMING_ADJ_LEN)-1)<<BZ_PHY_BLEC_TIMING_ADJ_POS)
#define BZ_PHY_BLEC_TIMING_ADJ_UMSK                             (~(((1ULL<<BZ_PHY_BLEC_TIMING_ADJ_LEN)-1)<<BZ_PHY_BLEC_TIMING_ADJ_POS))
#define BZ_PHY_BLEC_AA_CORR_THRES                               BZ_PHY_BLEC_AA_CORR_THRES
#define BZ_PHY_BLEC_AA_CORR_THRES_POS                           (17U)
#define BZ_PHY_BLEC_AA_CORR_THRES_LEN                           (8U)
#define BZ_PHY_BLEC_AA_CORR_THRES_MSK                           (((1ULL<<BZ_PHY_BLEC_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLEC_AA_CORR_THRES_POS)
#define BZ_PHY_BLEC_AA_CORR_THRES_UMSK                          (~(((1ULL<<BZ_PHY_BLEC_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLEC_AA_CORR_THRES_POS))
#define BZ_PHY_BLEC_AA_SEARCH_STOP                              BZ_PHY_BLEC_AA_SEARCH_STOP
#define BZ_PHY_BLEC_AA_SEARCH_STOP_POS                          (25U)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_LEN                          (7U)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_MSK                          (((1ULL<<BZ_PHY_BLEC_AA_SEARCH_STOP_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_STOP_POS)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_UMSK                         (~(((1ULL<<BZ_PHY_BLEC_AA_SEARCH_STOP_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_STOP_POS))

/* 0x894 : phy_ble_frtk_0 */
#define BZ_PHY_PHY_BLE_FRTK_0_OFFSET                            (0x894)
#define BZ_PHY_BLE_FRTK_MAX_STEP                                BZ_PHY_BLE_FRTK_MAX_STEP
#define BZ_PHY_BLE_FRTK_MAX_STEP_POS                            (0U)
#define BZ_PHY_BLE_FRTK_MAX_STEP_LEN                            (6U)
#define BZ_PHY_BLE_FRTK_MAX_STEP_MSK                            (((1ULL<<BZ_PHY_BLE_FRTK_MAX_STEP_LEN)-1)<<BZ_PHY_BLE_FRTK_MAX_STEP_POS)
#define BZ_PHY_BLE_FRTK_MAX_STEP_UMSK                           (~(((1ULL<<BZ_PHY_BLE_FRTK_MAX_STEP_LEN)-1)<<BZ_PHY_BLE_FRTK_MAX_STEP_POS))
#define BZ_PHY_DF_FTRK_SEL                                      BZ_PHY_DF_FTRK_SEL
#define BZ_PHY_DF_FTRK_SEL_POS                                  (6U)
#define BZ_PHY_DF_FTRK_SEL_LEN                                  (2U)
#define BZ_PHY_DF_FTRK_SEL_MSK                                  (((1ULL<<BZ_PHY_DF_FTRK_SEL_LEN)-1)<<BZ_PHY_DF_FTRK_SEL_POS)
#define BZ_PHY_DF_FTRK_SEL_UMSK                                 (~(((1ULL<<BZ_PHY_DF_FTRK_SEL_LEN)-1)<<BZ_PHY_DF_FTRK_SEL_POS))
#define BZ_PHY_BLE_FRTK_TH3                                     BZ_PHY_BLE_FRTK_TH3
#define BZ_PHY_BLE_FRTK_TH3_POS                                 (8U)
#define BZ_PHY_BLE_FRTK_TH3_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH3_MSK                                 (((1ULL<<BZ_PHY_BLE_FRTK_TH3_LEN)-1)<<BZ_PHY_BLE_FRTK_TH3_POS)
#define BZ_PHY_BLE_FRTK_TH3_UMSK                                (~(((1ULL<<BZ_PHY_BLE_FRTK_TH3_LEN)-1)<<BZ_PHY_BLE_FRTK_TH3_POS))
#define BZ_PHY_BLE_FRTK_TH2                                     BZ_PHY_BLE_FRTK_TH2
#define BZ_PHY_BLE_FRTK_TH2_POS                                 (16U)
#define BZ_PHY_BLE_FRTK_TH2_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH2_MSK                                 (((1ULL<<BZ_PHY_BLE_FRTK_TH2_LEN)-1)<<BZ_PHY_BLE_FRTK_TH2_POS)
#define BZ_PHY_BLE_FRTK_TH2_UMSK                                (~(((1ULL<<BZ_PHY_BLE_FRTK_TH2_LEN)-1)<<BZ_PHY_BLE_FRTK_TH2_POS))
#define BZ_PHY_BLE_FRTK_TH1                                     BZ_PHY_BLE_FRTK_TH1
#define BZ_PHY_BLE_FRTK_TH1_POS                                 (24U)
#define BZ_PHY_BLE_FRTK_TH1_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH1_MSK                                 (((1ULL<<BZ_PHY_BLE_FRTK_TH1_LEN)-1)<<BZ_PHY_BLE_FRTK_TH1_POS)
#define BZ_PHY_BLE_FRTK_TH1_UMSK                                (~(((1ULL<<BZ_PHY_BLE_FRTK_TH1_LEN)-1)<<BZ_PHY_BLE_FRTK_TH1_POS))

/* 0x898 : phy_ble_frtk_1 */
#define BZ_PHY_PHY_BLE_FRTK_1_OFFSET                            (0x898)
#define BZ_PHY_BLE_FRTK_EN                                      BZ_PHY_BLE_FRTK_EN
#define BZ_PHY_BLE_FRTK_EN_POS                                  (0U)
#define BZ_PHY_BLE_FRTK_EN_LEN                                  (1U)
#define BZ_PHY_BLE_FRTK_EN_MSK                                  (((1ULL<<BZ_PHY_BLE_FRTK_EN_LEN)-1)<<BZ_PHY_BLE_FRTK_EN_POS)
#define BZ_PHY_BLE_FRTK_EN_UMSK                                 (~(((1ULL<<BZ_PHY_BLE_FRTK_EN_LEN)-1)<<BZ_PHY_BLE_FRTK_EN_POS))
#define BZ_PHY_BLE_FRTK_COEFF                                   BZ_PHY_BLE_FRTK_COEFF
#define BZ_PHY_BLE_FRTK_COEFF_POS                               (1U)
#define BZ_PHY_BLE_FRTK_COEFF_LEN                               (9U)
#define BZ_PHY_BLE_FRTK_COEFF_MSK                               (((1ULL<<BZ_PHY_BLE_FRTK_COEFF_LEN)-1)<<BZ_PHY_BLE_FRTK_COEFF_POS)
#define BZ_PHY_BLE_FRTK_COEFF_UMSK                              (~(((1ULL<<BZ_PHY_BLE_FRTK_COEFF_LEN)-1)<<BZ_PHY_BLE_FRTK_COEFF_POS))

/* 0x89C : phy_tx_dfe_3 */
#define BZ_PHY_PHY_TX_DFE_3_OFFSET                              (0x89C)
#define BZ_PHY_TX_PSTIMEOUT                                     BZ_PHY_TX_PSTIMEOUT
#define BZ_PHY_TX_PSTIMEOUT_POS                                 (0U)
#define BZ_PHY_TX_PSTIMEOUT_LEN                                 (6U)
#define BZ_PHY_TX_PSTIMEOUT_MSK                                 (((1ULL<<BZ_PHY_TX_PSTIMEOUT_LEN)-1)<<BZ_PHY_TX_PSTIMEOUT_POS)
#define BZ_PHY_TX_PSTIMEOUT_UMSK                                (~(((1ULL<<BZ_PHY_TX_PSTIMEOUT_LEN)-1)<<BZ_PHY_TX_PSTIMEOUT_POS))
#define BZ_PHY_TX_PSDELAY                                       BZ_PHY_TX_PSDELAY
#define BZ_PHY_TX_PSDELAY_POS                                   (6U)
#define BZ_PHY_TX_PSDELAY_LEN                                   (6U)
#define BZ_PHY_TX_PSDELAY_MSK                                   (((1ULL<<BZ_PHY_TX_PSDELAY_LEN)-1)<<BZ_PHY_TX_PSDELAY_POS)
#define BZ_PHY_TX_PSDELAY_UMSK                                  (~(((1ULL<<BZ_PHY_TX_PSDELAY_LEN)-1)<<BZ_PHY_TX_PSDELAY_POS))
#define BZ_PHY_TX_FSK_DELAY                                     BZ_PHY_TX_FSK_DELAY
#define BZ_PHY_TX_FSK_DELAY_POS                                 (12U)
#define BZ_PHY_TX_FSK_DELAY_LEN                                 (3U)
#define BZ_PHY_TX_FSK_DELAY_MSK                                 (((1ULL<<BZ_PHY_TX_FSK_DELAY_LEN)-1)<<BZ_PHY_TX_FSK_DELAY_POS)
#define BZ_PHY_TX_FSK_DELAY_UMSK                                (~(((1ULL<<BZ_PHY_TX_FSK_DELAY_LEN)-1)<<BZ_PHY_TX_FSK_DELAY_POS))
#define BZ_PHY_TX_GAIN_NOM_EDR                                  BZ_PHY_TX_GAIN_NOM_EDR
#define BZ_PHY_TX_GAIN_NOM_EDR_POS                              (16U)
#define BZ_PHY_TX_GAIN_NOM_EDR_LEN                              (8U)
#define BZ_PHY_TX_GAIN_NOM_EDR_MSK                              (((1ULL<<BZ_PHY_TX_GAIN_NOM_EDR_LEN)-1)<<BZ_PHY_TX_GAIN_NOM_EDR_POS)
#define BZ_PHY_TX_GAIN_NOM_EDR_UMSK                             (~(((1ULL<<BZ_PHY_TX_GAIN_NOM_EDR_LEN)-1)<<BZ_PHY_TX_GAIN_NOM_EDR_POS))
#define BZ_PHY_TX_BT_NOM                                        BZ_PHY_TX_BT_NOM
#define BZ_PHY_TX_BT_NOM_POS                                    (24U)
#define BZ_PHY_TX_BT_NOM_LEN                                    (8U)
#define BZ_PHY_TX_BT_NOM_MSK                                    (((1ULL<<BZ_PHY_TX_BT_NOM_LEN)-1)<<BZ_PHY_TX_BT_NOM_POS)
#define BZ_PHY_TX_BT_NOM_UMSK                                   (~(((1ULL<<BZ_PHY_TX_BT_NOM_LEN)-1)<<BZ_PHY_TX_BT_NOM_POS))

/* 0x8A0 : phy_tx_dfe_4 */
#define BZ_PHY_PHY_TX_DFE_4_OFFSET                              (0x8A0)
#define BZ_PHY_TX_DFE_GAIN                                      BZ_PHY_TX_DFE_GAIN
#define BZ_PHY_TX_DFE_GAIN_POS                                  (0U)
#define BZ_PHY_TX_DFE_GAIN_LEN                                  (7U)
#define BZ_PHY_TX_DFE_GAIN_MSK                                  (((1ULL<<BZ_PHY_TX_DFE_GAIN_LEN)-1)<<BZ_PHY_TX_DFE_GAIN_POS)
#define BZ_PHY_TX_DFE_GAIN_UMSK                                 (~(((1ULL<<BZ_PHY_TX_DFE_GAIN_LEN)-1)<<BZ_PHY_TX_DFE_GAIN_POS))
#define BZ_PHY_TX_DFE_GAIN_EN                                   BZ_PHY_TX_DFE_GAIN_EN
#define BZ_PHY_TX_DFE_GAIN_EN_POS                               (7U)
#define BZ_PHY_TX_DFE_GAIN_EN_LEN                               (1U)
#define BZ_PHY_TX_DFE_GAIN_EN_MSK                               (((1ULL<<BZ_PHY_TX_DFE_GAIN_EN_LEN)-1)<<BZ_PHY_TX_DFE_GAIN_EN_POS)
#define BZ_PHY_TX_DFE_GAIN_EN_UMSK                              (~(((1ULL<<BZ_PHY_TX_DFE_GAIN_EN_LEN)-1)<<BZ_PHY_TX_DFE_GAIN_EN_POS))
#define BZ_PHY_TX_DPSK_DELAY                                    BZ_PHY_TX_DPSK_DELAY
#define BZ_PHY_TX_DPSK_DELAY_POS                                (8U)
#define BZ_PHY_TX_DPSK_DELAY_LEN                                (3U)
#define BZ_PHY_TX_DPSK_DELAY_MSK                                (((1ULL<<BZ_PHY_TX_DPSK_DELAY_LEN)-1)<<BZ_PHY_TX_DPSK_DELAY_POS)
#define BZ_PHY_TX_DPSK_DELAY_UMSK                               (~(((1ULL<<BZ_PHY_TX_DPSK_DELAY_LEN)-1)<<BZ_PHY_TX_DPSK_DELAY_POS))
#define BZ_PHY_TX_EDR_GUARD_N                                   BZ_PHY_TX_EDR_GUARD_N
#define BZ_PHY_TX_EDR_GUARD_N_POS                               (15U)
#define BZ_PHY_TX_EDR_GUARD_N_LEN                               (7U)
#define BZ_PHY_TX_EDR_GUARD_N_MSK                               (((1ULL<<BZ_PHY_TX_EDR_GUARD_N_LEN)-1)<<BZ_PHY_TX_EDR_GUARD_N_POS)
#define BZ_PHY_TX_EDR_GUARD_N_UMSK                              (~(((1ULL<<BZ_PHY_TX_EDR_GUARD_N_LEN)-1)<<BZ_PHY_TX_EDR_GUARD_N_POS))
#define BZ_PHY_TX_EPSILON                                       BZ_PHY_TX_EPSILON
#define BZ_PHY_TX_EPSILON_POS                                   (22U)
#define BZ_PHY_TX_EPSILON_LEN                                   (5U)
#define BZ_PHY_TX_EPSILON_MSK                                   (((1ULL<<BZ_PHY_TX_EPSILON_LEN)-1)<<BZ_PHY_TX_EPSILON_POS)
#define BZ_PHY_TX_EPSILON_UMSK                                  (~(((1ULL<<BZ_PHY_TX_EPSILON_LEN)-1)<<BZ_PHY_TX_EPSILON_POS))
#define BZ_PHY_TX_RCF_RATE                                      BZ_PHY_TX_RCF_RATE
#define BZ_PHY_TX_RCF_RATE_POS                                  (27U)
#define BZ_PHY_TX_RCF_RATE_LEN                                  (2U)
#define BZ_PHY_TX_RCF_RATE_MSK                                  (((1ULL<<BZ_PHY_TX_RCF_RATE_LEN)-1)<<BZ_PHY_TX_RCF_RATE_POS)
#define BZ_PHY_TX_RCF_RATE_UMSK                                 (~(((1ULL<<BZ_PHY_TX_RCF_RATE_LEN)-1)<<BZ_PHY_TX_RCF_RATE_POS))
#define BZ_PHY_TX_RCF_EN                                        BZ_PHY_TX_RCF_EN
#define BZ_PHY_TX_RCF_EN_POS                                    (29U)
#define BZ_PHY_TX_RCF_EN_LEN                                    (1U)
#define BZ_PHY_TX_RCF_EN_MSK                                    (((1ULL<<BZ_PHY_TX_RCF_EN_LEN)-1)<<BZ_PHY_TX_RCF_EN_POS)
#define BZ_PHY_TX_RCF_EN_UMSK                                   (~(((1ULL<<BZ_PHY_TX_RCF_EN_LEN)-1)<<BZ_PHY_TX_RCF_EN_POS))

/* 0x8A4 : phy_tx_dfe_5 */
#define BZ_PHY_PHY_TX_DFE_5_OFFSET                              (0x8A4)
#define BZ_PHY_TX_GAIN_NOM                                      BZ_PHY_TX_GAIN_NOM
#define BZ_PHY_TX_GAIN_NOM_POS                                  (24U)
#define BZ_PHY_TX_GAIN_NOM_LEN                                  (8U)
#define BZ_PHY_TX_GAIN_NOM_MSK                                  (((1ULL<<BZ_PHY_TX_GAIN_NOM_LEN)-1)<<BZ_PHY_TX_GAIN_NOM_POS)
#define BZ_PHY_TX_GAIN_NOM_UMSK                                 (~(((1ULL<<BZ_PHY_TX_GAIN_NOM_LEN)-1)<<BZ_PHY_TX_GAIN_NOM_POS))

/* 0x8A8 : phy_ble_coded_2 */
#define BZ_PHY_PHY_BLE_CODED_2_OFFSET                           (0x8A8)
#define BZ_PHY_CI_TIME_OUT_TH                                   BZ_PHY_CI_TIME_OUT_TH
#define BZ_PHY_CI_TIME_OUT_TH_POS                               (0U)
#define BZ_PHY_CI_TIME_OUT_TH_LEN                               (9U)
#define BZ_PHY_CI_TIME_OUT_TH_MSK                               (((1ULL<<BZ_PHY_CI_TIME_OUT_TH_LEN)-1)<<BZ_PHY_CI_TIME_OUT_TH_POS)
#define BZ_PHY_CI_TIME_OUT_TH_UMSK                              (~(((1ULL<<BZ_PHY_CI_TIME_OUT_TH_LEN)-1)<<BZ_PHY_CI_TIME_OUT_TH_POS))

/* 0x8C0 : phy_bt_ctrl_0 */
#define BZ_PHY_PHY_BT_CTRL_0_OFFSET                             (0x8C0)
#define BZ_PHY_NBT_BLE_4S                                       BZ_PHY_NBT_BLE_4S
#define BZ_PHY_NBT_BLE_4S_POS                                   (0U)
#define BZ_PHY_NBT_BLE_4S_LEN                                   (1U)
#define BZ_PHY_NBT_BLE_4S_MSK                                   (((1ULL<<BZ_PHY_NBT_BLE_4S_LEN)-1)<<BZ_PHY_NBT_BLE_4S_POS)
#define BZ_PHY_NBT_BLE_4S_UMSK                                  (~(((1ULL<<BZ_PHY_NBT_BLE_4S_LEN)-1)<<BZ_PHY_NBT_BLE_4S_POS))
#define BZ_PHY_NBT_BLE_4S_EN                                    BZ_PHY_NBT_BLE_4S_EN
#define BZ_PHY_NBT_BLE_4S_EN_POS                                (1U)
#define BZ_PHY_NBT_BLE_4S_EN_LEN                                (1U)
#define BZ_PHY_NBT_BLE_4S_EN_MSK                                (((1ULL<<BZ_PHY_NBT_BLE_4S_EN_LEN)-1)<<BZ_PHY_NBT_BLE_4S_EN_POS)
#define BZ_PHY_NBT_BLE_4S_EN_UMSK                               (~(((1ULL<<BZ_PHY_NBT_BLE_4S_EN_LEN)-1)<<BZ_PHY_NBT_BLE_4S_EN_POS))
#define BZ_PHY_BT_RSSI_OFFSET                                   BZ_PHY_BT_RSSI_OFFSET
#define BZ_PHY_BT_RSSI_OFFSET_POS                               (8U)
#define BZ_PHY_BT_RSSI_OFFSET_LEN                               (8U)
#define BZ_PHY_BT_RSSI_OFFSET_MSK                               (((1ULL<<BZ_PHY_BT_RSSI_OFFSET_LEN)-1)<<BZ_PHY_BT_RSSI_OFFSET_POS)
#define BZ_PHY_BT_RSSI_OFFSET_UMSK                              (~(((1ULL<<BZ_PHY_BT_RSSI_OFFSET_LEN)-1)<<BZ_PHY_BT_RSSI_OFFSET_POS))
#define BZ_PHY_BT_RX_AC_TH                                      BZ_PHY_BT_RX_AC_TH
#define BZ_PHY_BT_RX_AC_TH_POS                                  (16U)
#define BZ_PHY_BT_RX_AC_TH_LEN                                  (10U)
#define BZ_PHY_BT_RX_AC_TH_MSK                                  (((1ULL<<BZ_PHY_BT_RX_AC_TH_LEN)-1)<<BZ_PHY_BT_RX_AC_TH_POS)
#define BZ_PHY_BT_RX_AC_TH_UMSK                                 (~(((1ULL<<BZ_PHY_BT_RX_AC_TH_LEN)-1)<<BZ_PHY_BT_RX_AC_TH_POS))
#define BZ_PHY_TX_DFE_PSF_BT                                    BZ_PHY_TX_DFE_PSF_BT
#define BZ_PHY_TX_DFE_PSF_BT_POS                                (30U)
#define BZ_PHY_TX_DFE_PSF_BT_LEN                                (2U)
#define BZ_PHY_TX_DFE_PSF_BT_MSK                                (((1ULL<<BZ_PHY_TX_DFE_PSF_BT_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BT_POS)
#define BZ_PHY_TX_DFE_PSF_BT_UMSK                               (~(((1ULL<<BZ_PHY_TX_DFE_PSF_BT_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BT_POS))

/* 0x8C4 : phy_tx_pwr_ctrl_0 */
#define BZ_PHY_PHY_TX_PWR_CTRL_0_OFFSET                         (0x8C4)
#define BZ_PHY_TX_PA_PWR_CODE_00                                BZ_PHY_TX_PA_PWR_CODE_00
#define BZ_PHY_TX_PA_PWR_CODE_00_POS                            (0U)
#define BZ_PHY_TX_PA_PWR_CODE_00_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_00_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_00_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_00_POS)
#define BZ_PHY_TX_PA_PWR_CODE_00_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_00_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_00_POS))
#define BZ_PHY_TX_PA_PWR_CODE_01                                BZ_PHY_TX_PA_PWR_CODE_01
#define BZ_PHY_TX_PA_PWR_CODE_01_POS                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_01_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_01_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_01_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_01_POS)
#define BZ_PHY_TX_PA_PWR_CODE_01_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_01_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_01_POS))
#define BZ_PHY_TX_PA_PWR_CODE_02                                BZ_PHY_TX_PA_PWR_CODE_02
#define BZ_PHY_TX_PA_PWR_CODE_02_POS                            (10U)
#define BZ_PHY_TX_PA_PWR_CODE_02_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_02_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_02_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_02_POS)
#define BZ_PHY_TX_PA_PWR_CODE_02_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_02_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_02_POS))
#define BZ_PHY_TX_PA_PWR_CODE_03                                BZ_PHY_TX_PA_PWR_CODE_03
#define BZ_PHY_TX_PA_PWR_CODE_03_POS                            (15U)
#define BZ_PHY_TX_PA_PWR_CODE_03_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_03_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_03_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_03_POS)
#define BZ_PHY_TX_PA_PWR_CODE_03_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_03_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_03_POS))
#define BZ_PHY_TX_PA_PWR_CODE_04                                BZ_PHY_TX_PA_PWR_CODE_04
#define BZ_PHY_TX_PA_PWR_CODE_04_POS                            (20U)
#define BZ_PHY_TX_PA_PWR_CODE_04_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_04_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_04_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_04_POS)
#define BZ_PHY_TX_PA_PWR_CODE_04_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_04_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_04_POS))
#define BZ_PHY_TX_PA_PWR_CODE_05                                BZ_PHY_TX_PA_PWR_CODE_05
#define BZ_PHY_TX_PA_PWR_CODE_05_POS                            (25U)
#define BZ_PHY_TX_PA_PWR_CODE_05_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_05_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_05_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_05_POS)
#define BZ_PHY_TX_PA_PWR_CODE_05_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_05_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_05_POS))

/* 0x8C8 : phy_tx_pwr_ctrl_1 */
#define BZ_PHY_PHY_TX_PWR_CTRL_1_OFFSET                         (0x8C8)
#define BZ_PHY_TX_PA_PWR_CODE_06                                BZ_PHY_TX_PA_PWR_CODE_06
#define BZ_PHY_TX_PA_PWR_CODE_06_POS                            (0U)
#define BZ_PHY_TX_PA_PWR_CODE_06_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_06_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_06_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_06_POS)
#define BZ_PHY_TX_PA_PWR_CODE_06_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_06_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_06_POS))
#define BZ_PHY_TX_PA_PWR_CODE_07                                BZ_PHY_TX_PA_PWR_CODE_07
#define BZ_PHY_TX_PA_PWR_CODE_07_POS                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_07_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_07_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_07_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_07_POS)
#define BZ_PHY_TX_PA_PWR_CODE_07_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_07_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_07_POS))
#define BZ_PHY_TX_PA_PWR_CODE_08                                BZ_PHY_TX_PA_PWR_CODE_08
#define BZ_PHY_TX_PA_PWR_CODE_08_POS                            (10U)
#define BZ_PHY_TX_PA_PWR_CODE_08_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_08_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_08_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_08_POS)
#define BZ_PHY_TX_PA_PWR_CODE_08_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_08_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_08_POS))
#define BZ_PHY_TX_PA_PWR_CODE_09                                BZ_PHY_TX_PA_PWR_CODE_09
#define BZ_PHY_TX_PA_PWR_CODE_09_POS                            (15U)
#define BZ_PHY_TX_PA_PWR_CODE_09_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_09_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_09_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_09_POS)
#define BZ_PHY_TX_PA_PWR_CODE_09_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_09_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_09_POS))
#define BZ_PHY_TX_PA_PWR_CODE_10                                BZ_PHY_TX_PA_PWR_CODE_10
#define BZ_PHY_TX_PA_PWR_CODE_10_POS                            (20U)
#define BZ_PHY_TX_PA_PWR_CODE_10_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_10_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_10_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_10_POS)
#define BZ_PHY_TX_PA_PWR_CODE_10_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_10_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_10_POS))
#define BZ_PHY_TX_PA_PWR_CODE_11                                BZ_PHY_TX_PA_PWR_CODE_11
#define BZ_PHY_TX_PA_PWR_CODE_11_POS                            (25U)
#define BZ_PHY_TX_PA_PWR_CODE_11_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_11_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_11_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_11_POS)
#define BZ_PHY_TX_PA_PWR_CODE_11_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_11_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_11_POS))

/* 0x8CC : phy_tx_pwr_ctrl_2 */
#define BZ_PHY_PHY_TX_PWR_CTRL_2_OFFSET                         (0x8CC)
#define BZ_PHY_TX_PA_PWR_CODE_12                                BZ_PHY_TX_PA_PWR_CODE_12
#define BZ_PHY_TX_PA_PWR_CODE_12_POS                            (0U)
#define BZ_PHY_TX_PA_PWR_CODE_12_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_12_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_12_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_12_POS)
#define BZ_PHY_TX_PA_PWR_CODE_12_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_12_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_12_POS))
#define BZ_PHY_TX_PA_PWR_CODE_13                                BZ_PHY_TX_PA_PWR_CODE_13
#define BZ_PHY_TX_PA_PWR_CODE_13_POS                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_13_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_13_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_13_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_13_POS)
#define BZ_PHY_TX_PA_PWR_CODE_13_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_13_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_13_POS))
#define BZ_PHY_TX_PA_PWR_CODE_14                                BZ_PHY_TX_PA_PWR_CODE_14
#define BZ_PHY_TX_PA_PWR_CODE_14_POS                            (10U)
#define BZ_PHY_TX_PA_PWR_CODE_14_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_14_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_14_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_14_POS)
#define BZ_PHY_TX_PA_PWR_CODE_14_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_14_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_14_POS))
#define BZ_PHY_TX_PA_PWR_CODE_15                                BZ_PHY_TX_PA_PWR_CODE_15
#define BZ_PHY_TX_PA_PWR_CODE_15_POS                            (15U)
#define BZ_PHY_TX_PA_PWR_CODE_15_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_15_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_15_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_15_POS)
#define BZ_PHY_TX_PA_PWR_CODE_15_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_15_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_15_POS))
#define BZ_PHY_TX_PA_PWR_CODE_16                                BZ_PHY_TX_PA_PWR_CODE_16
#define BZ_PHY_TX_PA_PWR_CODE_16_POS                            (20U)
#define BZ_PHY_TX_PA_PWR_CODE_16_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_16_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_16_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_16_POS)
#define BZ_PHY_TX_PA_PWR_CODE_16_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_16_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_16_POS))
#define BZ_PHY_TX_PA_PWR_CODE_17                                BZ_PHY_TX_PA_PWR_CODE_17
#define BZ_PHY_TX_PA_PWR_CODE_17_POS                            (25U)
#define BZ_PHY_TX_PA_PWR_CODE_17_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_17_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_17_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_17_POS)
#define BZ_PHY_TX_PA_PWR_CODE_17_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_17_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_17_POS))

/* 0x8D0 : phy_tx_pwr_ctrl_3 */
#define BZ_PHY_PHY_TX_PWR_CTRL_3_OFFSET                         (0x8D0)
#define BZ_PHY_TX_PA_PWR_CODE_18                                BZ_PHY_TX_PA_PWR_CODE_18
#define BZ_PHY_TX_PA_PWR_CODE_18_POS                            (0U)
#define BZ_PHY_TX_PA_PWR_CODE_18_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_18_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_18_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_18_POS)
#define BZ_PHY_TX_PA_PWR_CODE_18_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_18_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_18_POS))
#define BZ_PHY_TX_PA_PWR_CODE_19                                BZ_PHY_TX_PA_PWR_CODE_19
#define BZ_PHY_TX_PA_PWR_CODE_19_POS                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_19_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_19_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_19_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_19_POS)
#define BZ_PHY_TX_PA_PWR_CODE_19_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_19_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_19_POS))
#define BZ_PHY_TX_PA_PWR_CODE_20                                BZ_PHY_TX_PA_PWR_CODE_20
#define BZ_PHY_TX_PA_PWR_CODE_20_POS                            (10U)
#define BZ_PHY_TX_PA_PWR_CODE_20_LEN                            (5U)
#define BZ_PHY_TX_PA_PWR_CODE_20_MSK                            (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_20_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_20_POS)
#define BZ_PHY_TX_PA_PWR_CODE_20_UMSK                           (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_20_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_20_POS))
#define BZ_PHY_TX_PA_PWR_CODE_CTRL_HW                           BZ_PHY_TX_PA_PWR_CODE_CTRL_HW
#define BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_POS                       (31U)
#define BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_LEN                       (1U)
#define BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_MSK                       (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_POS)
#define BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_UMSK                      (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_CTRL_HW_POS))

/* 0x8D4 : phy_tx_pwr_ctrl_4 */
#define BZ_PHY_PHY_TX_PWR_CTRL_4_OFFSET                         (0x8D4)
#define BZ_PHY_TX_PA_PWR_CODE_REDUCE                            BZ_PHY_TX_PA_PWR_CODE_REDUCE
#define BZ_PHY_TX_PA_PWR_CODE_REDUCE_POS                        (0U)
#define BZ_PHY_TX_PA_PWR_CODE_REDUCE_LEN                        (5U)
#define BZ_PHY_TX_PA_PWR_CODE_REDUCE_MSK                        (((1ULL<<BZ_PHY_TX_PA_PWR_CODE_REDUCE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_REDUCE_POS)
#define BZ_PHY_TX_PA_PWR_CODE_REDUCE_UMSK                       (~(((1ULL<<BZ_PHY_TX_PA_PWR_CODE_REDUCE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_REDUCE_POS))
#define BZ_PHY_TX_PA_REDUCE_PWR_ENA                             BZ_PHY_TX_PA_REDUCE_PWR_ENA
#define BZ_PHY_TX_PA_REDUCE_PWR_ENA_POS                         (5U)
#define BZ_PHY_TX_PA_REDUCE_PWR_ENA_LEN                         (1U)
#define BZ_PHY_TX_PA_REDUCE_PWR_ENA_MSK                         (((1ULL<<BZ_PHY_TX_PA_REDUCE_PWR_ENA_LEN)-1)<<BZ_PHY_TX_PA_REDUCE_PWR_ENA_POS)
#define BZ_PHY_TX_PA_REDUCE_PWR_ENA_UMSK                        (~(((1ULL<<BZ_PHY_TX_PA_REDUCE_PWR_ENA_LEN)-1)<<BZ_PHY_TX_PA_REDUCE_PWR_ENA_POS))

/* 0x900 : rx_notch_0 */
#define BZ_PHY_RX_NOTCH_0_OFFSET                                (0x900)
#define BZ_PHY_RX_NOTCH2_FNRM                                   BZ_PHY_RX_NOTCH2_FNRM
#define BZ_PHY_RX_NOTCH2_FNRM_POS                               (0U)
#define BZ_PHY_RX_NOTCH2_FNRM_LEN                               (11U)
#define BZ_PHY_RX_NOTCH2_FNRM_MSK                               (((1ULL<<BZ_PHY_RX_NOTCH2_FNRM_LEN)-1)<<BZ_PHY_RX_NOTCH2_FNRM_POS)
#define BZ_PHY_RX_NOTCH2_FNRM_UMSK                              (~(((1ULL<<BZ_PHY_RX_NOTCH2_FNRM_LEN)-1)<<BZ_PHY_RX_NOTCH2_FNRM_POS))
#define BZ_PHY_RX_NOTCH2_ALPHA                                  BZ_PHY_RX_NOTCH2_ALPHA
#define BZ_PHY_RX_NOTCH2_ALPHA_POS                              (11U)
#define BZ_PHY_RX_NOTCH2_ALPHA_LEN                              (4U)
#define BZ_PHY_RX_NOTCH2_ALPHA_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH2_ALPHA_LEN)-1)<<BZ_PHY_RX_NOTCH2_ALPHA_POS)
#define BZ_PHY_RX_NOTCH2_ALPHA_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH2_ALPHA_LEN)-1)<<BZ_PHY_RX_NOTCH2_ALPHA_POS))
#define BZ_PHY_RX_NOTCH2_EN                                     BZ_PHY_RX_NOTCH2_EN
#define BZ_PHY_RX_NOTCH2_EN_POS                                 (15U)
#define BZ_PHY_RX_NOTCH2_EN_LEN                                 (1U)
#define BZ_PHY_RX_NOTCH2_EN_MSK                                 (((1ULL<<BZ_PHY_RX_NOTCH2_EN_LEN)-1)<<BZ_PHY_RX_NOTCH2_EN_POS)
#define BZ_PHY_RX_NOTCH2_EN_UMSK                                (~(((1ULL<<BZ_PHY_RX_NOTCH2_EN_LEN)-1)<<BZ_PHY_RX_NOTCH2_EN_POS))
#define BZ_PHY_RX_NOTCH1_FNRM                                   BZ_PHY_RX_NOTCH1_FNRM
#define BZ_PHY_RX_NOTCH1_FNRM_POS                               (16U)
#define BZ_PHY_RX_NOTCH1_FNRM_LEN                               (11U)
#define BZ_PHY_RX_NOTCH1_FNRM_MSK                               (((1ULL<<BZ_PHY_RX_NOTCH1_FNRM_LEN)-1)<<BZ_PHY_RX_NOTCH1_FNRM_POS)
#define BZ_PHY_RX_NOTCH1_FNRM_UMSK                              (~(((1ULL<<BZ_PHY_RX_NOTCH1_FNRM_LEN)-1)<<BZ_PHY_RX_NOTCH1_FNRM_POS))
#define BZ_PHY_RX_NOTCH1_ALPHA                                  BZ_PHY_RX_NOTCH1_ALPHA
#define BZ_PHY_RX_NOTCH1_ALPHA_POS                              (27U)
#define BZ_PHY_RX_NOTCH1_ALPHA_LEN                              (4U)
#define BZ_PHY_RX_NOTCH1_ALPHA_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH1_ALPHA_LEN)-1)<<BZ_PHY_RX_NOTCH1_ALPHA_POS)
#define BZ_PHY_RX_NOTCH1_ALPHA_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH1_ALPHA_LEN)-1)<<BZ_PHY_RX_NOTCH1_ALPHA_POS))
#define BZ_PHY_RX_NOTCH1_EN                                     BZ_PHY_RX_NOTCH1_EN
#define BZ_PHY_RX_NOTCH1_EN_POS                                 (31U)
#define BZ_PHY_RX_NOTCH1_EN_LEN                                 (1U)
#define BZ_PHY_RX_NOTCH1_EN_MSK                                 (((1ULL<<BZ_PHY_RX_NOTCH1_EN_LEN)-1)<<BZ_PHY_RX_NOTCH1_EN_POS)
#define BZ_PHY_RX_NOTCH1_EN_UMSK                                (~(((1ULL<<BZ_PHY_RX_NOTCH1_EN_LEN)-1)<<BZ_PHY_RX_NOTCH1_EN_POS))

/* 0x904 : rx_notch_1 */
#define BZ_PHY_RX_NOTCH_1_OFFSET                                (0x904)
#define BZ_PHY_RX_NOTCH_TAR_CH_0                                BZ_PHY_RX_NOTCH_TAR_CH_0
#define BZ_PHY_RX_NOTCH_TAR_CH_0_POS                            (0U)
#define BZ_PHY_RX_NOTCH_TAR_CH_0_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_0_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_0_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_0_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_0_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_0_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_0_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_1                                BZ_PHY_RX_NOTCH_TAR_CH_1
#define BZ_PHY_RX_NOTCH_TAR_CH_1_POS                            (8U)
#define BZ_PHY_RX_NOTCH_TAR_CH_1_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_1_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_1_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_1_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_1_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_1_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_1_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_2                                BZ_PHY_RX_NOTCH_TAR_CH_2
#define BZ_PHY_RX_NOTCH_TAR_CH_2_POS                            (16U)
#define BZ_PHY_RX_NOTCH_TAR_CH_2_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_2_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_2_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_2_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_2_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_2_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_2_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_3                                BZ_PHY_RX_NOTCH_TAR_CH_3
#define BZ_PHY_RX_NOTCH_TAR_CH_3_POS                            (24U)
#define BZ_PHY_RX_NOTCH_TAR_CH_3_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_3_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_3_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_3_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_3_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_3_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_3_POS))
#define BZ_PHY_RX_NOTCH_CTRL_HW                                 BZ_PHY_RX_NOTCH_CTRL_HW
#define BZ_PHY_RX_NOTCH_CTRL_HW_POS                             (31U)
#define BZ_PHY_RX_NOTCH_CTRL_HW_LEN                             (1U)
#define BZ_PHY_RX_NOTCH_CTRL_HW_MSK                             (((1ULL<<BZ_PHY_RX_NOTCH_CTRL_HW_LEN)-1)<<BZ_PHY_RX_NOTCH_CTRL_HW_POS)
#define BZ_PHY_RX_NOTCH_CTRL_HW_UMSK                            (~(((1ULL<<BZ_PHY_RX_NOTCH_CTRL_HW_LEN)-1)<<BZ_PHY_RX_NOTCH_CTRL_HW_POS))

/* 0x908 : rx_notch_2 */
#define BZ_PHY_RX_NOTCH_2_OFFSET                                (0x908)
#define BZ_PHY_RX_NOTCH_TAR_CH_4                                BZ_PHY_RX_NOTCH_TAR_CH_4
#define BZ_PHY_RX_NOTCH_TAR_CH_4_POS                            (0U)
#define BZ_PHY_RX_NOTCH_TAR_CH_4_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_4_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_4_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_4_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_4_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_4_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_4_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_5                                BZ_PHY_RX_NOTCH_TAR_CH_5
#define BZ_PHY_RX_NOTCH_TAR_CH_5_POS                            (8U)
#define BZ_PHY_RX_NOTCH_TAR_CH_5_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_5_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_5_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_5_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_5_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_5_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_5_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_6                                BZ_PHY_RX_NOTCH_TAR_CH_6
#define BZ_PHY_RX_NOTCH_TAR_CH_6_POS                            (16U)
#define BZ_PHY_RX_NOTCH_TAR_CH_6_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_6_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_6_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_6_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_6_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_6_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_6_POS))
#define BZ_PHY_RX_NOTCH_TAR_CH_7                                BZ_PHY_RX_NOTCH_TAR_CH_7
#define BZ_PHY_RX_NOTCH_TAR_CH_7_POS                            (24U)
#define BZ_PHY_RX_NOTCH_TAR_CH_7_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_7_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_7_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_7_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_7_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_7_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_7_POS))

/* 0x90C : rx_notch_3 */
#define BZ_PHY_RX_NOTCH_3_OFFSET                                (0x90C)
#define BZ_PHY_RX_NOTCH_TAR_CH_8                                BZ_PHY_RX_NOTCH_TAR_CH_8
#define BZ_PHY_RX_NOTCH_TAR_CH_8_POS                            (0U)
#define BZ_PHY_RX_NOTCH_TAR_CH_8_LEN                            (7U)
#define BZ_PHY_RX_NOTCH_TAR_CH_8_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_8_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_8_POS)
#define BZ_PHY_RX_NOTCH_TAR_CH_8_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH_TAR_CH_8_LEN)-1)<<BZ_PHY_RX_NOTCH_TAR_CH_8_POS))

/* 0x910 : rx_notch_4 */
#define BZ_PHY_RX_NOTCH_4_OFFSET                                (0x910)
#define BZ_PHY_RX_NOTCH_PARA_0                                  BZ_PHY_RX_NOTCH_PARA_0
#define BZ_PHY_RX_NOTCH_PARA_0_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_0_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_0_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_0_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_0_POS)
#define BZ_PHY_RX_NOTCH_PARA_0_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_0_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_0_POS))

/* 0x914 : rx_notch_5 */
#define BZ_PHY_RX_NOTCH_5_OFFSET                                (0x914)
#define BZ_PHY_RX_NOTCH_PARA_1                                  BZ_PHY_RX_NOTCH_PARA_1
#define BZ_PHY_RX_NOTCH_PARA_1_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_1_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_1_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_1_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_1_POS)
#define BZ_PHY_RX_NOTCH_PARA_1_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_1_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_1_POS))

/* 0x918 : rx_notch_6 */
#define BZ_PHY_RX_NOTCH_6_OFFSET                                (0x918)
#define BZ_PHY_RX_NOTCH_PARA_2                                  BZ_PHY_RX_NOTCH_PARA_2
#define BZ_PHY_RX_NOTCH_PARA_2_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_2_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_2_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_2_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_2_POS)
#define BZ_PHY_RX_NOTCH_PARA_2_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_2_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_2_POS))

/* 0x91C : rx_notch_7 */
#define BZ_PHY_RX_NOTCH_7_OFFSET                                (0x91C)
#define BZ_PHY_RX_NOTCH_PARA_3                                  BZ_PHY_RX_NOTCH_PARA_3
#define BZ_PHY_RX_NOTCH_PARA_3_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_3_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_3_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_3_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_3_POS)
#define BZ_PHY_RX_NOTCH_PARA_3_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_3_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_3_POS))

/* 0x920 : rx_notch_8 */
#define BZ_PHY_RX_NOTCH_8_OFFSET                                (0x920)
#define BZ_PHY_RX_NOTCH_PARA_4                                  BZ_PHY_RX_NOTCH_PARA_4
#define BZ_PHY_RX_NOTCH_PARA_4_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_4_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_4_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_4_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_4_POS)
#define BZ_PHY_RX_NOTCH_PARA_4_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_4_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_4_POS))

/* 0x924 : rx_notch_9 */
#define BZ_PHY_RX_NOTCH_9_OFFSET                                (0x924)
#define BZ_PHY_RX_NOTCH_PARA_5                                  BZ_PHY_RX_NOTCH_PARA_5
#define BZ_PHY_RX_NOTCH_PARA_5_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_5_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_5_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_5_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_5_POS)
#define BZ_PHY_RX_NOTCH_PARA_5_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_5_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_5_POS))

/* 0x928 : rx_notch_10 */
#define BZ_PHY_RX_NOTCH_10_OFFSET                               (0x928)
#define BZ_PHY_RX_NOTCH_PARA_6                                  BZ_PHY_RX_NOTCH_PARA_6
#define BZ_PHY_RX_NOTCH_PARA_6_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_6_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_6_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_6_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_6_POS)
#define BZ_PHY_RX_NOTCH_PARA_6_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_6_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_6_POS))

/* 0x92C : rx_notch_11 */
#define BZ_PHY_RX_NOTCH_11_OFFSET                               (0x92C)
#define BZ_PHY_RX_NOTCH_PARA_7                                  BZ_PHY_RX_NOTCH_PARA_7
#define BZ_PHY_RX_NOTCH_PARA_7_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_7_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_7_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_7_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_7_POS)
#define BZ_PHY_RX_NOTCH_PARA_7_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_7_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_7_POS))

/* 0x930 : rx_notch_12 */
#define BZ_PHY_RX_NOTCH_12_OFFSET                               (0x930)
#define BZ_PHY_RX_NOTCH_PARA_8                                  BZ_PHY_RX_NOTCH_PARA_8
#define BZ_PHY_RX_NOTCH_PARA_8_POS                              (0U)
#define BZ_PHY_RX_NOTCH_PARA_8_LEN                              (32U)
#define BZ_PHY_RX_NOTCH_PARA_8_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH_PARA_8_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_8_POS)
#define BZ_PHY_RX_NOTCH_PARA_8_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH_PARA_8_LEN)-1)<<BZ_PHY_RX_NOTCH_PARA_8_POS))

/* 0x934 : rx_notch_13 */
#define BZ_PHY_RX_NOTCH_13_OFFSET                               (0x934)
#define BZ_PHY_RX_NOTCH2_FNRM_HW                                BZ_PHY_RX_NOTCH2_FNRM_HW
#define BZ_PHY_RX_NOTCH2_FNRM_HW_POS                            (0U)
#define BZ_PHY_RX_NOTCH2_FNRM_HW_LEN                            (11U)
#define BZ_PHY_RX_NOTCH2_FNRM_HW_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH2_FNRM_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_FNRM_HW_POS)
#define BZ_PHY_RX_NOTCH2_FNRM_HW_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH2_FNRM_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_FNRM_HW_POS))
#define BZ_PHY_RX_NOTCH2_ALPHA_HW                               BZ_PHY_RX_NOTCH2_ALPHA_HW
#define BZ_PHY_RX_NOTCH2_ALPHA_HW_POS                           (11U)
#define BZ_PHY_RX_NOTCH2_ALPHA_HW_LEN                           (4U)
#define BZ_PHY_RX_NOTCH2_ALPHA_HW_MSK                           (((1ULL<<BZ_PHY_RX_NOTCH2_ALPHA_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_ALPHA_HW_POS)
#define BZ_PHY_RX_NOTCH2_ALPHA_HW_UMSK                          (~(((1ULL<<BZ_PHY_RX_NOTCH2_ALPHA_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_ALPHA_HW_POS))
#define BZ_PHY_RX_NOTCH2_EN_HW                                  BZ_PHY_RX_NOTCH2_EN_HW
#define BZ_PHY_RX_NOTCH2_EN_HW_POS                              (15U)
#define BZ_PHY_RX_NOTCH2_EN_HW_LEN                              (1U)
#define BZ_PHY_RX_NOTCH2_EN_HW_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH2_EN_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_EN_HW_POS)
#define BZ_PHY_RX_NOTCH2_EN_HW_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH2_EN_HW_LEN)-1)<<BZ_PHY_RX_NOTCH2_EN_HW_POS))
#define BZ_PHY_RX_NOTCH1_FNRM_HW                                BZ_PHY_RX_NOTCH1_FNRM_HW
#define BZ_PHY_RX_NOTCH1_FNRM_HW_POS                            (16U)
#define BZ_PHY_RX_NOTCH1_FNRM_HW_LEN                            (11U)
#define BZ_PHY_RX_NOTCH1_FNRM_HW_MSK                            (((1ULL<<BZ_PHY_RX_NOTCH1_FNRM_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_FNRM_HW_POS)
#define BZ_PHY_RX_NOTCH1_FNRM_HW_UMSK                           (~(((1ULL<<BZ_PHY_RX_NOTCH1_FNRM_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_FNRM_HW_POS))
#define BZ_PHY_RX_NOTCH1_ALPHA_HW                               BZ_PHY_RX_NOTCH1_ALPHA_HW
#define BZ_PHY_RX_NOTCH1_ALPHA_HW_POS                           (27U)
#define BZ_PHY_RX_NOTCH1_ALPHA_HW_LEN                           (4U)
#define BZ_PHY_RX_NOTCH1_ALPHA_HW_MSK                           (((1ULL<<BZ_PHY_RX_NOTCH1_ALPHA_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_ALPHA_HW_POS)
#define BZ_PHY_RX_NOTCH1_ALPHA_HW_UMSK                          (~(((1ULL<<BZ_PHY_RX_NOTCH1_ALPHA_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_ALPHA_HW_POS))
#define BZ_PHY_RX_NOTCH1_EN_HW                                  BZ_PHY_RX_NOTCH1_EN_HW
#define BZ_PHY_RX_NOTCH1_EN_HW_POS                              (31U)
#define BZ_PHY_RX_NOTCH1_EN_HW_LEN                              (1U)
#define BZ_PHY_RX_NOTCH1_EN_HW_MSK                              (((1ULL<<BZ_PHY_RX_NOTCH1_EN_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_EN_HW_POS)
#define BZ_PHY_RX_NOTCH1_EN_HW_UMSK                             (~(((1ULL<<BZ_PHY_RX_NOTCH1_EN_HW_LEN)-1)<<BZ_PHY_RX_NOTCH1_EN_HW_POS))

/* 0x980 : phy_fpga_ctrl_0 */
#define BZ_PHY_PHY_FPGA_CTRL_0_OFFSET                           (0x980)
#define BZ_PHY_PF_TH2                                           BZ_PHY_PF_TH2
#define BZ_PHY_PF_TH2_POS                                       (0U)
#define BZ_PHY_PF_TH2_LEN                                       (10U)
#define BZ_PHY_PF_TH2_MSK                                       (((1ULL<<BZ_PHY_PF_TH2_LEN)-1)<<BZ_PHY_PF_TH2_POS)
#define BZ_PHY_PF_TH2_UMSK                                      (~(((1ULL<<BZ_PHY_PF_TH2_LEN)-1)<<BZ_PHY_PF_TH2_POS))
#define BZ_PHY_PF_TH1                                           BZ_PHY_PF_TH1
#define BZ_PHY_PF_TH1_POS                                       (10U)
#define BZ_PHY_PF_TH1_LEN                                       (10U)
#define BZ_PHY_PF_TH1_MSK                                       (((1ULL<<BZ_PHY_PF_TH1_LEN)-1)<<BZ_PHY_PF_TH1_POS)
#define BZ_PHY_PF_TH1_UMSK                                      (~(((1ULL<<BZ_PHY_PF_TH1_LEN)-1)<<BZ_PHY_PF_TH1_POS))
#define BZ_PHY_PF_EN_Q                                          BZ_PHY_PF_EN_Q
#define BZ_PHY_PF_EN_Q_POS                                      (20U)
#define BZ_PHY_PF_EN_Q_LEN                                      (1U)
#define BZ_PHY_PF_EN_Q_MSK                                      (((1ULL<<BZ_PHY_PF_EN_Q_LEN)-1)<<BZ_PHY_PF_EN_Q_POS)
#define BZ_PHY_PF_EN_Q_UMSK                                     (~(((1ULL<<BZ_PHY_PF_EN_Q_LEN)-1)<<BZ_PHY_PF_EN_Q_POS))
#define BZ_PHY_PF_EN_I                                          BZ_PHY_PF_EN_I
#define BZ_PHY_PF_EN_I_POS                                      (21U)
#define BZ_PHY_PF_EN_I_LEN                                      (1U)
#define BZ_PHY_PF_EN_I_MSK                                      (((1ULL<<BZ_PHY_PF_EN_I_LEN)-1)<<BZ_PHY_PF_EN_I_POS)
#define BZ_PHY_PF_EN_I_UMSK                                     (~(((1ULL<<BZ_PHY_PF_EN_I_LEN)-1)<<BZ_PHY_PF_EN_I_POS))
#define BZ_PHY_FPGA_DIG_GAIN                                    BZ_PHY_FPGA_DIG_GAIN
#define BZ_PHY_FPGA_DIG_GAIN_POS                                (26U)
#define BZ_PHY_FPGA_DIG_GAIN_LEN                                (6U)
#define BZ_PHY_FPGA_DIG_GAIN_MSK                                (((1ULL<<BZ_PHY_FPGA_DIG_GAIN_LEN)-1)<<BZ_PHY_FPGA_DIG_GAIN_POS)
#define BZ_PHY_FPGA_DIG_GAIN_UMSK                               (~(((1ULL<<BZ_PHY_FPGA_DIG_GAIN_LEN)-1)<<BZ_PHY_FPGA_DIG_GAIN_POS))


struct  bz_phy_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : phy_top_0 */
    union {
        struct {
            uint32_t rxdfe_en                       :  1; /* [    0],        r/w,        0x1 */
            uint32_t dfe_rxmode_force               :  1; /* [    1],        r/w,        0x0 */
            uint32_t dfe_rxmode                     :  3; /* [ 4: 2],        r/w,        0x0 */
            uint32_t txdfe_en                       :  1; /* [    5],        r/w,        0x1 */
            uint32_t dfe_txmode_force               :  1; /* [    6],        r/w,        0x0 */
            uint32_t dfe_txmode                     :  3; /* [ 9: 7],        r/w,        0x0 */
            uint32_t dfe_clk_force_on               :  1; /* [   10],        r/w,        0x0 */
            uint32_t adc_clk_sel                    :  1; /* [   11],        r/w,        0x0 */
            uint32_t dfe_rxdfe_4s_on                :  1; /* [   12],        r/w,        0x0 */
            uint32_t dfe_txdfe_4s_on                :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_top_0;

    /* 0x804 : phy_tx_dfe_0 */
    union {
        struct {
            uint32_t tx_dfe_iq_if_freq              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t tx_dfe_psf_ble1m               :  2; /* [17:16],        r/w,        0x1 */
            uint32_t tx_dfe_psf_ble2m               :  2; /* [19:18],        r/w,        0x1 */
            uint32_t tx_dfe_psf_zig                 :  2; /* [21:20],        r/w,        0x2 */
            uint32_t tx_pll_bw_switch_th            : 10; /* [31:22],        r/w,      0x300 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_0;

    /* 0x808 : phy_tx_dfe_1 */
    union {
        struct {
            uint32_t reserved_0_8                   :  9; /* [ 8: 0],       rsvd,        0x0 */
            uint32_t tx_pa_pwr_code                 :  5; /* [13: 9],        r/w,       0x1a */
            uint32_t tx_rampup_am_step              :  8; /* [21:14],        r/w,        0x8 */
            uint32_t tx_rampup_am_on                :  1; /* [   22],        r/w,        0x1 */
            uint32_t tx_rampup_fm_on                :  1; /* [   23],        r/w,        0x1 */
            uint32_t tx_rampup_time_us              :  8; /* [31:24],        r/w,        0x8 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_1;

    /* 0x80C : phy_tx_dfe_2 */
    union {
        struct {
            uint32_t tx_rampdn_pad0_time_us         :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t tx_rampdn_am_start_dly         :  6; /* [13: 8],        r/w,       0x10 */
            uint32_t tx_rampdn_am_step              :  8; /* [21:14],        r/w,       0x10 */
            uint32_t tx_rampdn_am_on                :  1; /* [   22],        r/w,        0x1 */
            uint32_t tx_rampdn_fm_on                :  1; /* [   23],        r/w,        0x1 */
            uint32_t tx_rampdn_time_us              :  8; /* [31:24],        r/w,        0x4 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_2;

    /* 0x810 : phy_rx_dfe_0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t rx_dfe_toc_en                  :  1; /* [    1],        r/w,        0x1 */
            uint32_t rx_dfe_ddc_en                  :  1; /* [    2],        r/w,        0x1 */
            uint32_t rx_dfe_cs_en                   :  1; /* [    3],        r/w,        0x1 */
            uint32_t rx_dfe_dga_en                  :  1; /* [    4],        r/w,        0x1 */
            uint32_t rx_dfe_diffOsr_en              :  1; /* [    5],        r/w,        0x1 */
            uint32_t rx_dfe_nsr_en                  :  1; /* [    6],        r/w,        0x1 */
            uint32_t rx_dfe_loop_en                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t rx_dfe_dccan_en                :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_rx_dfe_0;

    /* 0x814 : phy_rx_dfe_1 */
    union {
        struct {
            uint32_t rx_dfe_freq_trk_val            : 13; /* [12: 0],        r/w,        0x0 */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t rx_dfe_freq_trk_force          :  1; /* [   15],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_rx_dfe_1;

    /* 0x818 : phy_rx_dfe_2 */
    union {
        struct {
            uint32_t reserved_0_22                  : 23; /* [22: 0],       rsvd,        0x0 */
            uint32_t rx_dfe_dccan_alpha             :  2; /* [24:23],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_rx_dfe_2;

    /* 0x81C : phy_rx_dfe_3 */
    union {
        struct {
            uint32_t rx_dfe_toc_fac                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t rx_dfe_toc_fac_force           :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_29                 : 17; /* [29:13],       rsvd,        0x0 */
            uint32_t rx_dfe_df_iq_sel               :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_rx_dfe_3;

    /* 0x820 : phy_rx_zb_receiver_0 */
    union {
        struct {
            uint32_t zb_ts_thres_timing             :  3; /* [ 2: 0],        r/w,        0x4 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t zb_ts_thres3                   :  6; /* [ 9: 4],        r/w,        0xa */
            uint32_t zb_ts_thres2                   :  6; /* [15:10],        r/w,        0x8 */
            uint32_t zb_ts_thres1                   :  6; /* [21:16],        r/w,        0x6 */
            uint32_t zb_ts_thres0                   :  6; /* [27:22],        r/w,        0x4 */
            uint32_t zb_ts_cc_alpha                 :  4; /* [31:28],        r/w,        0x6 */
        }BF;
        uint32_t WORD;
    } phy_rx_zb_receiver_0;

    /* 0x824 : phy_rx_zb_receiver_1 */
    union {
        struct {
            uint32_t zb_dm_sfd_cnt_n                :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t zb_dm_sfd_thres                :  9; /* [12: 4],        r/w,       0x28 */
            uint32_t zb_dm_p_mode_en                :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t zb_dm_plen_err_mode            :  1; /* [   15],        r/w,        0x0 */
            uint32_t zb_ft_coeff                    :  4; /* [19:16],        r/w,        0x4 */
            uint32_t zb_dm_plen_min                 :  4; /* [23:20],        r/w,        0x4 */
            uint32_t zb_dm_plen_max                 :  8; /* [31:24],        r/w,       0x7f */
        }BF;
        uint32_t WORD;
    } phy_rx_zb_receiver_1;

    /* 0x828 : phy_rx_zb_receiver_2 */
    union {
        struct {
            uint32_t zb_ts_rl_thres                 :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t zb_ts_ccrl_alpha               :  4; /* [ 7: 4],        r/w,        0xc */
            uint32_t zb_dm_h_sfd_thres_p_mode       :  8; /* [15: 8],        r/w,       0x12 */
            uint32_t zb_dm_sfd_thres_p_mode         :  8; /* [23:16],        r/w,       0x28 */
            uint32_t zb_dm_h_sfd_thres              :  8; /* [31:24],        r/w,       0x12 */
        }BF;
        uint32_t WORD;
    } phy_rx_zb_receiver_2;

    /* 0x82C : phy_rx_zb_receiver_3 */
    union {
        struct {
            uint32_t zb_fsm_mf_dly_time             :  8; /* [ 7: 0],        r/w,       0x20 */
            uint32_t zb_fsm_dm_dly_time             : 10; /* [17: 8],        r/w,       0x2f */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t zb_fsm_cg_4s_on                :  1; /* [   19],        r/w,        0x0 */
            uint32_t zb_fsm_lo_time_us              :  7; /* [26:20],        r/w,       0x33 */
            uint32_t zb_fsm_cca_enable              :  1; /* [   27],        r/w,        0x0 */
            uint32_t zb_fsm_ed_enable               :  1; /* [   28],        r/w,        0x0 */
            uint32_t zb_fsm_sfd_time_out_en         :  1; /* [   29],        r/w,        0x1 */
            uint32_t zb_fsm_rx_listen_st_en         :  1; /* [   30],        r/w,        0x0 */
            uint32_t zb_fsm_en                      :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } phy_rx_zb_receiver_3;

    /* 0x830 : phy_rx_zb_receiver_4 */
    union {
        struct {
            uint32_t zb_ft_init_val                 : 13; /* [12: 0],        r/w,        0x0 */
            uint32_t zb_ft_init_val_force           :  1; /* [   13],        r/w,        0x0 */
            uint32_t zb_dm_alpha_2000               :  4; /* [17:14],        r/w,        0x4 */
            uint32_t zb_dm_alpha_1000               :  4; /* [21:18],        r/w,        0x4 */
            uint32_t zb_dm_alpha_500                :  4; /* [25:22],        r/w,        0x4 */
            uint32_t zb_fsm_sync_timeout_ms         :  5; /* [30:26],        r/w,        0x3 */
            uint32_t zb_fsm_sync_timeout_en         :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } phy_rx_zb_receiver_4;

    /* 0x834 : phy_cca_0 */
    union {
        struct {
            uint32_t cca_st_int_set                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t cca_st_int_clr                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t cca_st_int                     :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t cca_st_dm_flag                 :  1; /* [    4],          r,        0x0 */
            uint32_t cca_st_cs_flag                 :  1; /* [    5],          r,        0x0 */
            uint32_t cca_st_ed_flag                 :  1; /* [    6],          r,        0x0 */
            uint32_t cca_st_cca_flag                :  1; /* [    7],          r,        0x0 */
            uint32_t cca_st_ed_meas                 :  7; /* [14: 8],          r,        0x0 */
            uint32_t phy_sw_cca_en_sel              :  1; /* [   15],        r/w,        0x0 */
            uint32_t phy_cca_ed_th                  :  7; /* [22:16],        r/w,       0x4c */
            uint32_t phy_cca_found_src              :  1; /* [   23],        r/w,        0x0 */
            uint32_t phy_cca_dur                    :  3; /* [26:24],        r/w,        0x7 */
            uint32_t phy_cca_mode                   :  2; /* [28:27],        r/w,        0x2 */
            uint32_t phy_cca_en                     :  1; /* [   29],        r/w,        0x1 */
            uint32_t phy_sw_cca_en                  :  1; /* [   30],        r/w,        0x0 */
            uint32_t phy_sw_ed_en                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_cca_0;

    /* 0x838 : phy_ble_sync_0 */
    union {
        struct {
            uint32_t t2f_mag_th                     :  9; /* [ 8: 0],        r/w,       0x20 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t blec_cfoe_alpha                :  3; /* [12:10],        r/w,        0x0 */
            uint32_t ble_sync_corr_avg_l            :  5; /* [17:13],        r/w,        0x7 */
            uint32_t ble_cfoe_avg_l                 :  4; /* [21:18],        r/w,        0x8 */
            uint32_t ble_aa_offset_coef             :  5; /* [26:22],        r/w,       0x18 */
            uint32_t ble_aa_error_max               :  5; /* [31:27],        r/w,       0x1f */
        }BF;
        uint32_t WORD;
    } phy_ble_sync_0;

    /* 0x83C : phy_ble_sync_1 */
    union {
        struct {
            uint32_t t2f_limite_th                  : 13; /* [12: 0],        r/w,      0x400 */
            uint32_t t2f_impulse_th                 : 14; /* [26:13],        r/w,      0x400 */
            uint32_t t2f_filter_mode                :  1; /* [   27],        r/w,        0x0 */
            uint32_t t2f_filter_en                  :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_ble_sync_1;

    /* 0x840 : phy_reserved_0 */
    union {
        struct {
            uint32_t phy_reserved_0                 : 32; /* [31: 0],        r/w,     0xffff */
        }BF;
        uint32_t WORD;
    } phy_reserved_0;

    /* 0x844 : phy_rx_proc_time_1 */
    union {
        struct {
            uint32_t rx_proc_time_viterbi_us_500k   :  8; /* [ 7: 0],        r/w,        0xa */
            uint32_t rx_proc_time_mlsd_us_2m        :  8; /* [15: 8],        r/w,        0xa */
            uint32_t rx_proc_time_eq_us_2m          :  8; /* [23:16],        r/w,        0xa */
            uint32_t rx_proc_time_bt_us             :  8; /* [31:24],        r/w,        0xa */
        }BF;
        uint32_t WORD;
    } phy_rx_proc_time_1;

    /* 0x848 : phy_ble_dfe_0 */
    union {
        struct {
            uint32_t fb_coef_0                      :  8; /* [ 7: 0],        r/w,       0x27 */
            uint32_t ff_coef_2                      :  8; /* [15: 8],        r/w,       0x41 */
            uint32_t ff_coef_1                      :  8; /* [23:16],        r/w,       0xbf */
            uint32_t ff_coef_0                      :  8; /* [31:24],        r/w,       0x16 */
        }BF;
        uint32_t WORD;
    } phy_ble_dfe_0;

    /* 0x84C : phy_ble_sync_2 */
    union {
        struct {
            uint32_t ble_aa_corr_thres              : 12; /* [11: 0],        r/w,       0xa8 */
            uint32_t ble_search_offset              :  5; /* [16:12],        r/w,        0x5 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_ble_sync_2;

    /* 0x850 : phy_ble_fsm_0 */
    union {
        struct {
            uint32_t ble_rx_cfo_adj_symb            :  8; /* [ 7: 0],        r/w,        0x3 */
            uint32_t ble_fsm_rx_dmtime_2m           :  8; /* [15: 8],        r/w,       0x22 */
            uint32_t ble_fsm_rx_dmtime_1m           :  8; /* [23:16],        r/w,       0x22 */
            uint32_t ble_fsm_cfo_peak_dly           :  4; /* [27:24],        r/w,        0x2 */
            uint32_t ble_demod_mode                 :  2; /* [29:28],        r/w,        0x2 */
            uint32_t ble_fsm_cg_4s_on               :  1; /* [   30],        r/w,        0x0 */
            uint32_t ble_fsm_en                     :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } phy_ble_fsm_0;

    /* 0x854 : phy_rx_proc_time_0 */
    union {
        struct {
            uint32_t rx_proc_time_viterbi_us_125k   :  8; /* [ 7: 0],        r/w,        0xa */
            uint32_t rx_proc_time_mlsd_us_1m        :  8; /* [15: 8],        r/w,        0xa */
            uint32_t rx_proc_time_eq_us_1m          :  8; /* [23:16],        r/w,        0xa */
            uint32_t rx_proc_time_m154_us           :  8; /* [31:24],        r/w,        0xa */
        }BF;
        uint32_t WORD;
    } phy_rx_proc_time_0;

    /* 0x858 : phy_testbus_0 */
    union {
        struct {
            uint32_t phy_testbus_read               : 32; /* [31: 0],          r,     0xffff */
        }BF;
        uint32_t WORD;
    } phy_testbus_0;

    /* 0x85C : phy_testbus_1 */
    union {
        struct {
            uint32_t phy_testbus_sel                :  4; /* [ 3: 0],        r/w,        0xf */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_testbus_1;

    /* 0x860 : phy_status_0 */
    union {
        struct {
            uint32_t sts_agc_fe_dg                  :  6; /* [ 5: 0],          r,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t sts_agc_agc_rssi               :  8; /* [15: 8],          r,        0x0 */
            uint32_t sts_agc_rf_rbb_ind             :  5; /* [20:16],          r,        0x0 */
            uint32_t sts_agc_rf_lna                 :  4; /* [24:21],          r,        0x0 */
            uint32_t reserved_25_30                 :  6; /* [30:25],       rsvd,        0x0 */
            uint32_t sts_agc_clear                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_0;

    /* 0x864 : phy_status_1 */
    union {
        struct {
            uint32_t sts_zb_sfd_corr                : 15; /* [14: 0],          r,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t sts_zb_freq_trk_out            : 13; /* [28:16],          r,        0x0 */
            uint32_t sts_zb_sfd_mode                :  2; /* [30:29],          r,        0x0 */
            uint32_t sts_zb_clear                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_1;

    /* 0x868 : phy_status_2 */
    union {
        struct {
            uint32_t sts_zb_peak_val_2              : 14; /* [13: 0],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t sts_zb_peak_val_1              : 14; /* [29:16],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_2;

    /* 0x86C : phy_status_3 */
    union {
        struct {
            uint32_t sts_zb_peak_loc_2              :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t sts_zb_peak_loc_1              :  8; /* [15: 8],          r,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_3;

    /* 0x870 : phy_status_4 */
    union {
        struct {
            uint32_t sts_zb_peak_im                 : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t sts_zb_peak_re                 : 12; /* [27:16],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_4;

    /* 0x874 : phy_status_5 */
    union {
        struct {
            uint32_t sts_zb_curr_loc                :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
            uint32_t sts_zb_curr_max                : 14; /* [29:16],          r,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_5;

    /* 0x878 : phy_status_6 */
    union {
        struct {
            uint32_t sts_done_zb_freq_trk_out       : 13; /* [12: 0],          r,        0x0 */
            uint32_t sts_zb_ts_cs_found             :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_6;

    /* 0x87C : phy_status_7 */
    union {
        struct {
            uint32_t sts_ble_cfo_out_peak           :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t sts_ble_frtk_out               : 13; /* [20: 8],          r,        0x0 */
            uint32_t reserved_21_30                 : 10; /* [30:21],       rsvd,        0x0 */
            uint32_t sts_ble_clear                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_7;

    /* 0x880 : phy_status_8 */
    union {
        struct {
            uint32_t sts_ble_aa_confirm_bits        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_8;

    /* 0x884 : phy_status_9 */
    union {
        struct {
            uint32_t sts_ble_done_cfo_out_peak      :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t sts_ble_corr_out_max           : 12; /* [19: 8],          r,        0x0 */
            uint32_t sts_ble_aa_error_cnt           :  6; /* [25:20],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_status_9;

    /* 0x888 : phy_status_10 */
    union {
        struct {
            uint32_t zb_fsm_err_sts                 :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t zb_fsm_dbg_en                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t zb_ts_cs_found                 :  1; /* [    9],          r,        0x0 */
            uint32_t sts_zb_sfd_found               :  1; /* [   10],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t sts_zb_phy_len                 :  8; /* [23:16],          r,        0x0 */
            uint32_t zb_dm_white_init               :  7; /* [30:24],        r/w,       0x69 */
            uint32_t zb_dm_white_en                 :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } phy_status_10;

    /* 0x88C : phy_ble_coded_0 */
    union {
        struct {
            uint32_t blec_aa_search_start           :  5; /* [ 4: 0],        r/w,        0x8 */
            uint32_t blec_pk_jump_thres             :  6; /* [10: 5],        r/w,       0x10 */
            uint32_t blec_pk_timing_diff_thres      :  3; /* [13:11],        r/w,        0x3 */
            uint32_t blec_pk_thres                  :  9; /* [22:14],        r/w,        0x6 */
            uint32_t blec_pk_start_thres            :  9; /* [31:23],        r/w,       0x40 */
        }BF;
        uint32_t WORD;
    } phy_ble_coded_0;

    /* 0x890 : phy_ble_coded_1 */
    union {
        struct {
            uint32_t ble_fsm_rx_dmtime_cd           :  8; /* [ 7: 0],        r/w,       0x84 */
            uint32_t blec_rst_thres                 :  4; /* [11: 8],        r/w,        0x5 */
            uint32_t blec_timing_adj                :  5; /* [16:12],        r/w,        0xe */
            uint32_t blec_aa_corr_thres             :  8; /* [24:17],        r/w,       0xac */
            uint32_t blec_aa_search_stop            :  7; /* [31:25],        r/w,       0x50 */
        }BF;
        uint32_t WORD;
    } phy_ble_coded_1;

    /* 0x894 : phy_ble_frtk_0 */
    union {
        struct {
            uint32_t ble_frtk_max_step              :  6; /* [ 5: 0],        r/w,        0x8 */
            uint32_t df_ftrk_sel                    :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t ble_frtk_th3                   :  7; /* [14: 8],        r/w,       0x40 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ble_frtk_th2                   :  7; /* [22:16],        r/w,       0x2e */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t ble_frtk_th1                   :  7; /* [30:24],        r/w,       0x1c */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_ble_frtk_0;

    /* 0x898 : phy_ble_frtk_1 */
    union {
        struct {
            uint32_t ble_frtk_en                    :  1; /* [    0],        r/w,        0x1 */
            uint32_t ble_frtk_coeff                 :  9; /* [ 9: 1],        r/w,        0xa */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_ble_frtk_1;

    /* 0x89C : phy_tx_dfe_3 */
    union {
        struct {
            uint32_t tx_pstimeout                   :  6; /* [ 5: 0],        r/w,       0x3f */
            uint32_t tx_psdelay                     :  6; /* [11: 6],        r/w,       0x14 */
            uint32_t tx_fsk_delay                   :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t tx_gain_nom_edr                :  8; /* [23:16],        r/w,       0x40 */
            uint32_t tx_bt_nom                      :  8; /* [31:24],        r/w,       0x29 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_3;

    /* 0x8A0 : phy_tx_dfe_4 */
    union {
        struct {
            uint32_t tx_dfe_gain                    :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t tx_dfe_gain_en                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t tx_dpsk_delay                  :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_14                 :  4; /* [14:11],       rsvd,        0x0 */
            uint32_t tx_edr_guard_n                 :  7; /* [21:15],        r/w,       0x5f */
            uint32_t tx_epsilon                     :  5; /* [26:22],        r/w,        0xb */
            uint32_t tx_rcf_rate                    :  2; /* [28:27],        r/w,        0x1 */
            uint32_t tx_rcf_en                      :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_4;

    /* 0x8A4 : phy_tx_dfe_5 */
    union {
        struct {
            uint32_t reserved_0_23                  : 24; /* [23: 0],       rsvd,        0x0 */
            uint32_t tx_gain_nom                    :  8; /* [31:24],        r/w,       0x40 */
        }BF;
        uint32_t WORD;
    } phy_tx_dfe_5;

    /* 0x8A8 : phy_ble_coded_2 */
    union {
        struct {
            uint32_t ci_time_out_th                 :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_ble_coded_2;

    /* 0x8ac  reserved */
    uint8_t RESERVED0x8ac[20];

    /* 0x8C0 : phy_bt_ctrl_0 */
    union {
        struct {
            uint32_t nbt_ble_4s                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t nbt_ble_4s_en                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t bt_rssi_offset                 :  8; /* [15: 8],        r/w,       0xcc */
            uint32_t bt_rx_ac_th                    : 10; /* [25:16],        r/w,      0x2d9 */
            uint32_t reserved_26_29                 :  4; /* [29:26],       rsvd,        0x0 */
            uint32_t tx_dfe_psf_bt                  :  2; /* [31:30],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } phy_bt_ctrl_0;

    /* 0x8C4 : phy_tx_pwr_ctrl_0 */
    union {
        struct {
            uint32_t tx_pa_pwr_code_00              :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_01              :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_02              :  5; /* [14:10],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_03              :  5; /* [19:15],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_04              :  5; /* [24:20],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_05              :  5; /* [29:25],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_pwr_ctrl_0;

    /* 0x8C8 : phy_tx_pwr_ctrl_1 */
    union {
        struct {
            uint32_t tx_pa_pwr_code_06              :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_07              :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_08              :  5; /* [14:10],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_09              :  5; /* [19:15],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_10              :  5; /* [24:20],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_11              :  5; /* [29:25],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_pwr_ctrl_1;

    /* 0x8CC : phy_tx_pwr_ctrl_2 */
    union {
        struct {
            uint32_t tx_pa_pwr_code_12              :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_13              :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_14              :  5; /* [14:10],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_15              :  5; /* [19:15],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_16              :  5; /* [24:20],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_17              :  5; /* [29:25],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_pwr_ctrl_2;

    /* 0x8D0 : phy_tx_pwr_ctrl_3 */
    union {
        struct {
            uint32_t tx_pa_pwr_code_18              :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_19              :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t tx_pa_pwr_code_20              :  5; /* [14:10],        r/w,        0x0 */
            uint32_t reserved_15_30                 : 16; /* [30:15],       rsvd,        0x0 */
            uint32_t tx_pa_pwr_code_ctrl_hw         :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_pwr_ctrl_3;

    /* 0x8D4 : phy_tx_pwr_ctrl_4 */
    union {
        struct {
            uint32_t tx_pa_pwr_code_reduce          :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t tx_pa_reduce_pwr_ena           :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } phy_tx_pwr_ctrl_4;

    /* 0x8d8  reserved */
    uint8_t RESERVED0x8d8[40];

    /* 0x900 : rx_notch_0 */
    union {
        struct {
            uint32_t rx_notch2_fnrm                 : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t rx_notch2_alpha                :  4; /* [14:11],        r/w,        0x0 */
            uint32_t rx_notch2_en                   :  1; /* [   15],        r/w,        0x0 */
            uint32_t rx_notch1_fnrm                 : 11; /* [26:16],        r/w,        0x0 */
            uint32_t rx_notch1_alpha                :  4; /* [30:27],        r/w,        0x0 */
            uint32_t rx_notch1_en                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_0;

    /* 0x904 : rx_notch_1 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_0              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_1              :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_2              :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_3              :  7; /* [30:24],        r/w,        0x0 */
            uint32_t rx_notch_ctrl_hw               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_1;

    /* 0x908 : rx_notch_2 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_4              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_5              :  7; /* [14: 8],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_6              :  7; /* [22:16],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t rx_notch_tar_ch_7              :  7; /* [30:24],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_2;

    /* 0x90C : rx_notch_3 */
    union {
        struct {
            uint32_t rx_notch_tar_ch_8              :  7; /* [ 6: 0],        r/w,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_3;

    /* 0x910 : rx_notch_4 */
    union {
        struct {
            uint32_t rx_notch_para_0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_4;

    /* 0x914 : rx_notch_5 */
    union {
        struct {
            uint32_t rx_notch_para_1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_5;

    /* 0x918 : rx_notch_6 */
    union {
        struct {
            uint32_t rx_notch_para_2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_6;

    /* 0x91C : rx_notch_7 */
    union {
        struct {
            uint32_t rx_notch_para_3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_7;

    /* 0x920 : rx_notch_8 */
    union {
        struct {
            uint32_t rx_notch_para_4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_8;

    /* 0x924 : rx_notch_9 */
    union {
        struct {
            uint32_t rx_notch_para_5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_9;

    /* 0x928 : rx_notch_10 */
    union {
        struct {
            uint32_t rx_notch_para_6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_10;

    /* 0x92C : rx_notch_11 */
    union {
        struct {
            uint32_t rx_notch_para_7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_11;

    /* 0x930 : rx_notch_12 */
    union {
        struct {
            uint32_t rx_notch_para_8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_12;

    /* 0x934 : rx_notch_13 */
    union {
        struct {
            uint32_t rx_notch2_fnrm_hw              : 11; /* [10: 0],          r,       0xa0 */
            uint32_t rx_notch2_alpha_hw             :  4; /* [14:11],          r,        0x0 */
            uint32_t rx_notch2_en_hw                :  1; /* [   15],          r,        0x0 */
            uint32_t rx_notch1_fnrm_hw              : 11; /* [26:16],          r,       0xa0 */
            uint32_t rx_notch1_alpha_hw             :  4; /* [30:27],          r,        0x0 */
            uint32_t rx_notch1_en_hw                :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } rx_notch_13;

    /* 0x938  reserved */
    uint8_t RESERVED0x938[72];

    /* 0x980 : phy_fpga_ctrl_0 */
    union {
        struct {
            uint32_t pf_th2                         : 10; /* [ 9: 0],        r/w,        0xf */
            uint32_t pf_th1                         : 10; /* [19:10],        r/w,       0x7f */
            uint32_t pf_en_q                        :  1; /* [   20],        r/w,        0x0 */
            uint32_t pf_en_i                        :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_25                 :  4; /* [25:22],       rsvd,        0x0 */
            uint32_t fpga_dig_gain                  :  6; /* [31:26],        r/w,       0x20 */
        }BF;
        uint32_t WORD;
    } phy_fpga_ctrl_0;

};

typedef volatile struct bz_phy_reg bz_phy_reg_t;


#endif  /* __BZ_PHY_REG_H__ */
