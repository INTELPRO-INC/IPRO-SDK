/**
  ******************************************************************************
  * @file    bz_phy_reg.h
  * @version V1.2
  * @date    2020-03-30
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 IntelPro</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of IntelPro nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef  __BZ_PHY_REG_H__
#define  __BZ_PHY_REG_H__

#include "ipro7.h"

/* 0x800 : phy_top_0 */
#define BZ_PHY_PHY_TOP_0_OFFSET                                 (0x800)
#define BZ_PHY_RXDFE_EN                                         BZ_PHY_RXDFE_EN
#define BZ_PHY_RXDFE_EN_POS                                     (0U)
#define BZ_PHY_RXDFE_EN_LEN                                     (1U)
#define BZ_PHY_RXDFE_EN_MSK                                     (((1U<<BZ_PHY_RXDFE_EN_LEN)-1)<<BZ_PHY_RXDFE_EN_POS)
#define BZ_PHY_RXDFE_EN_UMSK                                    (~(((1U<<BZ_PHY_RXDFE_EN_LEN)-1)<<BZ_PHY_RXDFE_EN_POS))
#define BZ_PHY_DFE_RXMODE_FORCE                                 BZ_PHY_DFE_RXMODE_FORCE
#define BZ_PHY_DFE_RXMODE_FORCE_POS                             (1U)
#define BZ_PHY_DFE_RXMODE_FORCE_LEN                             (1U)
#define BZ_PHY_DFE_RXMODE_FORCE_MSK                             (((1U<<BZ_PHY_DFE_RXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_RXMODE_FORCE_POS)
#define BZ_PHY_DFE_RXMODE_FORCE_UMSK                            (~(((1U<<BZ_PHY_DFE_RXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_RXMODE_FORCE_POS))
#define BZ_PHY_DFE_RXMODE                                       BZ_PHY_DFE_RXMODE
#define BZ_PHY_DFE_RXMODE_POS                                   (2U)
#define BZ_PHY_DFE_RXMODE_LEN                                   (2U)
#define BZ_PHY_DFE_RXMODE_MSK                                   (((1U<<BZ_PHY_DFE_RXMODE_LEN)-1)<<BZ_PHY_DFE_RXMODE_POS)
#define BZ_PHY_DFE_RXMODE_UMSK                                  (~(((1U<<BZ_PHY_DFE_RXMODE_LEN)-1)<<BZ_PHY_DFE_RXMODE_POS))
#define BZ_PHY_TXDFE_EN                                         BZ_PHY_TXDFE_EN
#define BZ_PHY_TXDFE_EN_POS                                     (4U)
#define BZ_PHY_TXDFE_EN_LEN                                     (1U)
#define BZ_PHY_TXDFE_EN_MSK                                     (((1U<<BZ_PHY_TXDFE_EN_LEN)-1)<<BZ_PHY_TXDFE_EN_POS)
#define BZ_PHY_TXDFE_EN_UMSK                                    (~(((1U<<BZ_PHY_TXDFE_EN_LEN)-1)<<BZ_PHY_TXDFE_EN_POS))
#define BZ_PHY_DFE_TXMODE_FORCE                                 BZ_PHY_DFE_TXMODE_FORCE
#define BZ_PHY_DFE_TXMODE_FORCE_POS                             (5U)
#define BZ_PHY_DFE_TXMODE_FORCE_LEN                             (1U)
#define BZ_PHY_DFE_TXMODE_FORCE_MSK                             (((1U<<BZ_PHY_DFE_TXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_TXMODE_FORCE_POS)
#define BZ_PHY_DFE_TXMODE_FORCE_UMSK                            (~(((1U<<BZ_PHY_DFE_TXMODE_FORCE_LEN)-1)<<BZ_PHY_DFE_TXMODE_FORCE_POS))
#define BZ_PHY_DFE_TXMODE                                       BZ_PHY_DFE_TXMODE
#define BZ_PHY_DFE_TXMODE_POS                                   (6U)
#define BZ_PHY_DFE_TXMODE_LEN                                   (2U)
#define BZ_PHY_DFE_TXMODE_MSK                                   (((1U<<BZ_PHY_DFE_TXMODE_LEN)-1)<<BZ_PHY_DFE_TXMODE_POS)
#define BZ_PHY_DFE_TXMODE_UMSK                                  (~(((1U<<BZ_PHY_DFE_TXMODE_LEN)-1)<<BZ_PHY_DFE_TXMODE_POS))
#define BZ_PHY_DFE_CLK_FORCE_ON                                 BZ_PHY_DFE_CLK_FORCE_ON
#define BZ_PHY_DFE_CLK_FORCE_ON_POS                             (8U)
#define BZ_PHY_DFE_CLK_FORCE_ON_LEN                             (1U)
#define BZ_PHY_DFE_CLK_FORCE_ON_MSK                             (((1U<<BZ_PHY_DFE_CLK_FORCE_ON_LEN)-1)<<BZ_PHY_DFE_CLK_FORCE_ON_POS)
#define BZ_PHY_DFE_CLK_FORCE_ON_UMSK                            (~(((1U<<BZ_PHY_DFE_CLK_FORCE_ON_LEN)-1)<<BZ_PHY_DFE_CLK_FORCE_ON_POS))
#define BZ_PHY_ADC_CLK_SEL                                      BZ_PHY_ADC_CLK_SEL
#define BZ_PHY_ADC_CLK_SEL_POS                                  (9U)
#define BZ_PHY_ADC_CLK_SEL_LEN                                  (1U)
#define BZ_PHY_ADC_CLK_SEL_MSK                                  (((1U<<BZ_PHY_ADC_CLK_SEL_LEN)-1)<<BZ_PHY_ADC_CLK_SEL_POS)
#define BZ_PHY_ADC_CLK_SEL_UMSK                                 (~(((1U<<BZ_PHY_ADC_CLK_SEL_LEN)-1)<<BZ_PHY_ADC_CLK_SEL_POS))
#define BZ_PHY_DFE_RXDFE_4S_ON                                  BZ_PHY_DFE_RXDFE_4S_ON
#define BZ_PHY_DFE_RXDFE_4S_ON_POS                              (10U)
#define BZ_PHY_DFE_RXDFE_4S_ON_LEN                              (1U)
#define BZ_PHY_DFE_RXDFE_4S_ON_MSK                              (((1U<<BZ_PHY_DFE_RXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_RXDFE_4S_ON_POS)
#define BZ_PHY_DFE_RXDFE_4S_ON_UMSK                             (~(((1U<<BZ_PHY_DFE_RXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_RXDFE_4S_ON_POS))
#define BZ_PHY_DFE_TXDFE_4S_ON                                  BZ_PHY_DFE_TXDFE_4S_ON
#define BZ_PHY_DFE_TXDFE_4S_ON_POS                              (11U)
#define BZ_PHY_DFE_TXDFE_4S_ON_LEN                              (1U)
#define BZ_PHY_DFE_TXDFE_4S_ON_MSK                              (((1U<<BZ_PHY_DFE_TXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_TXDFE_4S_ON_POS)
#define BZ_PHY_DFE_TXDFE_4S_ON_UMSK                             (~(((1U<<BZ_PHY_DFE_TXDFE_4S_ON_LEN)-1)<<BZ_PHY_DFE_TXDFE_4S_ON_POS))

/* 0x804 : phy_tx_dfe_0 */
#define BZ_PHY_PHY_TX_DFE_0_OFFSET                              (0x804)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ                                BZ_PHY_TX_DFE_IQ_IF_FREQ
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_POS                            (0U)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN                            (16U)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_MSK                            (((1U<<BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN)-1)<<BZ_PHY_TX_DFE_IQ_IF_FREQ_POS)
#define BZ_PHY_TX_DFE_IQ_IF_FREQ_UMSK                           (~(((1U<<BZ_PHY_TX_DFE_IQ_IF_FREQ_LEN)-1)<<BZ_PHY_TX_DFE_IQ_IF_FREQ_POS))
#define BZ_PHY_TX_DFE_PSF_BLE1M                                 BZ_PHY_TX_DFE_PSF_BLE1M
#define BZ_PHY_TX_DFE_PSF_BLE1M_POS                             (16U)
#define BZ_PHY_TX_DFE_PSF_BLE1M_LEN                             (2U)
#define BZ_PHY_TX_DFE_PSF_BLE1M_MSK                             (((1U<<BZ_PHY_TX_DFE_PSF_BLE1M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE1M_POS)
#define BZ_PHY_TX_DFE_PSF_BLE1M_UMSK                            (~(((1U<<BZ_PHY_TX_DFE_PSF_BLE1M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE1M_POS))
#define BZ_PHY_TX_DFE_PSF_BLE2M                                 BZ_PHY_TX_DFE_PSF_BLE2M
#define BZ_PHY_TX_DFE_PSF_BLE2M_POS                             (18U)
#define BZ_PHY_TX_DFE_PSF_BLE2M_LEN                             (2U)
#define BZ_PHY_TX_DFE_PSF_BLE2M_MSK                             (((1U<<BZ_PHY_TX_DFE_PSF_BLE2M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE2M_POS)
#define BZ_PHY_TX_DFE_PSF_BLE2M_UMSK                            (~(((1U<<BZ_PHY_TX_DFE_PSF_BLE2M_LEN)-1)<<BZ_PHY_TX_DFE_PSF_BLE2M_POS))
#define BZ_PHY_TX_DFE_PSF_ZIG                                   BZ_PHY_TX_DFE_PSF_ZIG
#define BZ_PHY_TX_DFE_PSF_ZIG_POS                               (20U)
#define BZ_PHY_TX_DFE_PSF_ZIG_LEN                               (2U)
#define BZ_PHY_TX_DFE_PSF_ZIG_MSK                               (((1U<<BZ_PHY_TX_DFE_PSF_ZIG_LEN)-1)<<BZ_PHY_TX_DFE_PSF_ZIG_POS)
#define BZ_PHY_TX_DFE_PSF_ZIG_UMSK                              (~(((1U<<BZ_PHY_TX_DFE_PSF_ZIG_LEN)-1)<<BZ_PHY_TX_DFE_PSF_ZIG_POS))
#define BZ_PHY_TX_PLL_BW_SWITCH_TH                              BZ_PHY_TX_PLL_BW_SWITCH_TH
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_POS                          (22U)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN                          (10U)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_MSK                          (((1U<<BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN)-1)<<BZ_PHY_TX_PLL_BW_SWITCH_TH_POS)
#define BZ_PHY_TX_PLL_BW_SWITCH_TH_UMSK                         (~(((1U<<BZ_PHY_TX_PLL_BW_SWITCH_TH_LEN)-1)<<BZ_PHY_TX_PLL_BW_SWITCH_TH_POS))

/* 0x808 : phy_tx_dfe_1 */
#define BZ_PHY_PHY_TX_DFE_1_OFFSET                              (0x808)
#define BZ_PHY_TX_PA_PWR_CODE                                   BZ_PHY_TX_PA_PWR_CODE
#define BZ_PHY_TX_PA_PWR_CODE_POS                               (9U)
#define BZ_PHY_TX_PA_PWR_CODE_LEN                               (5U)
#define BZ_PHY_TX_PA_PWR_CODE_MSK                               (((1U<<BZ_PHY_TX_PA_PWR_CODE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_POS)
#define BZ_PHY_TX_PA_PWR_CODE_UMSK                              (~(((1U<<BZ_PHY_TX_PA_PWR_CODE_LEN)-1)<<BZ_PHY_TX_PA_PWR_CODE_POS))
#define BZ_PHY_TX_RAMPUP_AM_STEP                                BZ_PHY_TX_RAMPUP_AM_STEP
#define BZ_PHY_TX_RAMPUP_AM_STEP_POS                            (14U)
#define BZ_PHY_TX_RAMPUP_AM_STEP_LEN                            (8U)
#define BZ_PHY_TX_RAMPUP_AM_STEP_MSK                            (((1U<<BZ_PHY_TX_RAMPUP_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_STEP_POS)
#define BZ_PHY_TX_RAMPUP_AM_STEP_UMSK                           (~(((1U<<BZ_PHY_TX_RAMPUP_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_STEP_POS))
#define BZ_PHY_TX_RAMPUP_AM_ON                                  BZ_PHY_TX_RAMPUP_AM_ON
#define BZ_PHY_TX_RAMPUP_AM_ON_POS                              (22U)
#define BZ_PHY_TX_RAMPUP_AM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPUP_AM_ON_MSK                              (((1U<<BZ_PHY_TX_RAMPUP_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_ON_POS)
#define BZ_PHY_TX_RAMPUP_AM_ON_UMSK                             (~(((1U<<BZ_PHY_TX_RAMPUP_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_AM_ON_POS))
#define BZ_PHY_TX_RAMPUP_FM_ON                                  BZ_PHY_TX_RAMPUP_FM_ON
#define BZ_PHY_TX_RAMPUP_FM_ON_POS                              (23U)
#define BZ_PHY_TX_RAMPUP_FM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPUP_FM_ON_MSK                              (((1U<<BZ_PHY_TX_RAMPUP_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_FM_ON_POS)
#define BZ_PHY_TX_RAMPUP_FM_ON_UMSK                             (~(((1U<<BZ_PHY_TX_RAMPUP_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPUP_FM_ON_POS))
#define BZ_PHY_TX_RAMPUP_TIME_US                                BZ_PHY_TX_RAMPUP_TIME_US
#define BZ_PHY_TX_RAMPUP_TIME_US_POS                            (24U)
#define BZ_PHY_TX_RAMPUP_TIME_US_LEN                            (8U)
#define BZ_PHY_TX_RAMPUP_TIME_US_MSK                            (((1U<<BZ_PHY_TX_RAMPUP_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPUP_TIME_US_POS)
#define BZ_PHY_TX_RAMPUP_TIME_US_UMSK                           (~(((1U<<BZ_PHY_TX_RAMPUP_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPUP_TIME_US_POS))

/* 0x80C : phy_tx_dfe_2 */
#define BZ_PHY_PHY_TX_DFE_2_OFFSET                              (0x80C)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US                           BZ_PHY_TX_RAMPDN_PAD0_TIME_US
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS                       (0U)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN                       (8U)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_MSK                       (((1U<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS)
#define BZ_PHY_TX_RAMPDN_PAD0_TIME_US_UMSK                      (~(((1U<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_PAD0_TIME_US_POS))
#define BZ_PHY_TX_RAMPDN_AM_START_DLY                           BZ_PHY_TX_RAMPDN_AM_START_DLY
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_POS                       (8U)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN                       (6U)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_MSK                       (((1U<<BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_START_DLY_POS)
#define BZ_PHY_TX_RAMPDN_AM_START_DLY_UMSK                      (~(((1U<<BZ_PHY_TX_RAMPDN_AM_START_DLY_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_START_DLY_POS))
#define BZ_PHY_TX_RAMPDN_AM_STEP                                BZ_PHY_TX_RAMPDN_AM_STEP
#define BZ_PHY_TX_RAMPDN_AM_STEP_POS                            (14U)
#define BZ_PHY_TX_RAMPDN_AM_STEP_LEN                            (8U)
#define BZ_PHY_TX_RAMPDN_AM_STEP_MSK                            (((1U<<BZ_PHY_TX_RAMPDN_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_STEP_POS)
#define BZ_PHY_TX_RAMPDN_AM_STEP_UMSK                           (~(((1U<<BZ_PHY_TX_RAMPDN_AM_STEP_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_STEP_POS))
#define BZ_PHY_TX_RAMPDN_AM_ON                                  BZ_PHY_TX_RAMPDN_AM_ON
#define BZ_PHY_TX_RAMPDN_AM_ON_POS                              (22U)
#define BZ_PHY_TX_RAMPDN_AM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPDN_AM_ON_MSK                              (((1U<<BZ_PHY_TX_RAMPDN_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_ON_POS)
#define BZ_PHY_TX_RAMPDN_AM_ON_UMSK                             (~(((1U<<BZ_PHY_TX_RAMPDN_AM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_AM_ON_POS))
#define BZ_PHY_TX_RAMPDN_FM_ON                                  BZ_PHY_TX_RAMPDN_FM_ON
#define BZ_PHY_TX_RAMPDN_FM_ON_POS                              (23U)
#define BZ_PHY_TX_RAMPDN_FM_ON_LEN                              (1U)
#define BZ_PHY_TX_RAMPDN_FM_ON_MSK                              (((1U<<BZ_PHY_TX_RAMPDN_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_FM_ON_POS)
#define BZ_PHY_TX_RAMPDN_FM_ON_UMSK                             (~(((1U<<BZ_PHY_TX_RAMPDN_FM_ON_LEN)-1)<<BZ_PHY_TX_RAMPDN_FM_ON_POS))
#define BZ_PHY_TX_RAMPDN_TIME_US                                BZ_PHY_TX_RAMPDN_TIME_US
#define BZ_PHY_TX_RAMPDN_TIME_US_POS                            (24U)
#define BZ_PHY_TX_RAMPDN_TIME_US_LEN                            (8U)
#define BZ_PHY_TX_RAMPDN_TIME_US_MSK                            (((1U<<BZ_PHY_TX_RAMPDN_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_TIME_US_POS)
#define BZ_PHY_TX_RAMPDN_TIME_US_UMSK                           (~(((1U<<BZ_PHY_TX_RAMPDN_TIME_US_LEN)-1)<<BZ_PHY_TX_RAMPDN_TIME_US_POS))

/* 0x810 : phy_rx_dfe_0 */
#define BZ_PHY_PHY_RX_DFE_0_OFFSET                              (0x810)
#define BZ_PHY_RX_DFE_NOTCH_EN                                  BZ_PHY_RX_DFE_NOTCH_EN
#define BZ_PHY_RX_DFE_NOTCH_EN_POS                              (0U)
#define BZ_PHY_RX_DFE_NOTCH_EN_LEN                              (1U)
#define BZ_PHY_RX_DFE_NOTCH_EN_MSK                              (((1U<<BZ_PHY_RX_DFE_NOTCH_EN_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_EN_POS)
#define BZ_PHY_RX_DFE_NOTCH_EN_UMSK                             (~(((1U<<BZ_PHY_RX_DFE_NOTCH_EN_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_EN_POS))
#define BZ_PHY_RX_DFE_TOC_EN                                    BZ_PHY_RX_DFE_TOC_EN
#define BZ_PHY_RX_DFE_TOC_EN_POS                                (1U)
#define BZ_PHY_RX_DFE_TOC_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_TOC_EN_MSK                                (((1U<<BZ_PHY_RX_DFE_TOC_EN_LEN)-1)<<BZ_PHY_RX_DFE_TOC_EN_POS)
#define BZ_PHY_RX_DFE_TOC_EN_UMSK                               (~(((1U<<BZ_PHY_RX_DFE_TOC_EN_LEN)-1)<<BZ_PHY_RX_DFE_TOC_EN_POS))
#define BZ_PHY_RX_DFE_DDC_EN                                    BZ_PHY_RX_DFE_DDC_EN
#define BZ_PHY_RX_DFE_DDC_EN_POS                                (2U)
#define BZ_PHY_RX_DFE_DDC_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_DDC_EN_MSK                                (((1U<<BZ_PHY_RX_DFE_DDC_EN_LEN)-1)<<BZ_PHY_RX_DFE_DDC_EN_POS)
#define BZ_PHY_RX_DFE_DDC_EN_UMSK                               (~(((1U<<BZ_PHY_RX_DFE_DDC_EN_LEN)-1)<<BZ_PHY_RX_DFE_DDC_EN_POS))
#define BZ_PHY_RX_DFE_CS_EN                                     BZ_PHY_RX_DFE_CS_EN
#define BZ_PHY_RX_DFE_CS_EN_POS                                 (3U)
#define BZ_PHY_RX_DFE_CS_EN_LEN                                 (1U)
#define BZ_PHY_RX_DFE_CS_EN_MSK                                 (((1U<<BZ_PHY_RX_DFE_CS_EN_LEN)-1)<<BZ_PHY_RX_DFE_CS_EN_POS)
#define BZ_PHY_RX_DFE_CS_EN_UMSK                                (~(((1U<<BZ_PHY_RX_DFE_CS_EN_LEN)-1)<<BZ_PHY_RX_DFE_CS_EN_POS))
#define BZ_PHY_RX_DFE_DGA_EN                                    BZ_PHY_RX_DFE_DGA_EN
#define BZ_PHY_RX_DFE_DGA_EN_POS                                (4U)
#define BZ_PHY_RX_DFE_DGA_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_DGA_EN_MSK                                (((1U<<BZ_PHY_RX_DFE_DGA_EN_LEN)-1)<<BZ_PHY_RX_DFE_DGA_EN_POS)
#define BZ_PHY_RX_DFE_DGA_EN_UMSK                               (~(((1U<<BZ_PHY_RX_DFE_DGA_EN_LEN)-1)<<BZ_PHY_RX_DFE_DGA_EN_POS))
#define BZ_PHY_RX_DFE_DIFFOSR_EN                                BZ_PHY_RX_DFE_DIFFOSR_EN
#define BZ_PHY_RX_DFE_DIFFOSR_EN_POS                            (5U)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_LEN                            (1U)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_MSK                            (((1U<<BZ_PHY_RX_DFE_DIFFOSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_DIFFOSR_EN_POS)
#define BZ_PHY_RX_DFE_DIFFOSR_EN_UMSK                           (~(((1U<<BZ_PHY_RX_DFE_DIFFOSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_DIFFOSR_EN_POS))
#define BZ_PHY_RX_DFE_NSR_EN                                    BZ_PHY_RX_DFE_NSR_EN
#define BZ_PHY_RX_DFE_NSR_EN_POS                                (6U)
#define BZ_PHY_RX_DFE_NSR_EN_LEN                                (1U)
#define BZ_PHY_RX_DFE_NSR_EN_MSK                                (((1U<<BZ_PHY_RX_DFE_NSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_NSR_EN_POS)
#define BZ_PHY_RX_DFE_NSR_EN_UMSK                               (~(((1U<<BZ_PHY_RX_DFE_NSR_EN_LEN)-1)<<BZ_PHY_RX_DFE_NSR_EN_POS))
#define BZ_PHY_RX_DFE_LOOP_EN                                   BZ_PHY_RX_DFE_LOOP_EN
#define BZ_PHY_RX_DFE_LOOP_EN_POS                               (7U)
#define BZ_PHY_RX_DFE_LOOP_EN_LEN                               (1U)
#define BZ_PHY_RX_DFE_LOOP_EN_MSK                               (((1U<<BZ_PHY_RX_DFE_LOOP_EN_LEN)-1)<<BZ_PHY_RX_DFE_LOOP_EN_POS)
#define BZ_PHY_RX_DFE_LOOP_EN_UMSK                              (~(((1U<<BZ_PHY_RX_DFE_LOOP_EN_LEN)-1)<<BZ_PHY_RX_DFE_LOOP_EN_POS))
#define BZ_PHY_RX_DFE_DCCAN_EN                                  BZ_PHY_RX_DFE_DCCAN_EN
#define BZ_PHY_RX_DFE_DCCAN_EN_POS                              (8U)
#define BZ_PHY_RX_DFE_DCCAN_EN_LEN                              (1U)
#define BZ_PHY_RX_DFE_DCCAN_EN_MSK                              (((1U<<BZ_PHY_RX_DFE_DCCAN_EN_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_EN_POS)
#define BZ_PHY_RX_DFE_DCCAN_EN_UMSK                             (~(((1U<<BZ_PHY_RX_DFE_DCCAN_EN_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_EN_POS))

/* 0x814 : phy_rx_dfe_1 */
#define BZ_PHY_PHY_RX_DFE_1_OFFSET                              (0x814)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL                              BZ_PHY_RX_DFE_FREQ_TRK_VAL
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS                          (0U)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN                          (13U)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_MSK                          (((1U<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS)
#define BZ_PHY_RX_DFE_FREQ_TRK_VAL_UMSK                         (~(((1U<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_VAL_POS))
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE                            BZ_PHY_RX_DFE_FREQ_TRK_FORCE
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS                        (15U)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN                        (1U)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_MSK                        (((1U<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS)
#define BZ_PHY_RX_DFE_FREQ_TRK_FORCE_UMSK                       (~(((1U<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_FREQ_TRK_FORCE_POS))

/* 0x818 : phy_rx_dfe_2 */
#define BZ_PHY_PHY_RX_DFE_2_OFFSET                              (0x818)
#define BZ_PHY_RX_DFE_NOTCH_ROT_Q                               BZ_PHY_RX_DFE_NOTCH_ROT_Q
#define BZ_PHY_RX_DFE_NOTCH_ROT_Q_POS                           (0U)
#define BZ_PHY_RX_DFE_NOTCH_ROT_Q_LEN                           (10U)
#define BZ_PHY_RX_DFE_NOTCH_ROT_Q_MSK                           (((1U<<BZ_PHY_RX_DFE_NOTCH_ROT_Q_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ROT_Q_POS)
#define BZ_PHY_RX_DFE_NOTCH_ROT_Q_UMSK                          (~(((1U<<BZ_PHY_RX_DFE_NOTCH_ROT_Q_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ROT_Q_POS))
#define BZ_PHY_RX_DFE_NOTCH_ROT_I                               BZ_PHY_RX_DFE_NOTCH_ROT_I
#define BZ_PHY_RX_DFE_NOTCH_ROT_I_POS                           (10U)
#define BZ_PHY_RX_DFE_NOTCH_ROT_I_LEN                           (10U)
#define BZ_PHY_RX_DFE_NOTCH_ROT_I_MSK                           (((1U<<BZ_PHY_RX_DFE_NOTCH_ROT_I_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ROT_I_POS)
#define BZ_PHY_RX_DFE_NOTCH_ROT_I_UMSK                          (~(((1U<<BZ_PHY_RX_DFE_NOTCH_ROT_I_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ROT_I_POS))
#define BZ_PHY_RX_DFE_NOTCH_ALPHA                               BZ_PHY_RX_DFE_NOTCH_ALPHA
#define BZ_PHY_RX_DFE_NOTCH_ALPHA_POS                           (20U)
#define BZ_PHY_RX_DFE_NOTCH_ALPHA_LEN                           (2U)
#define BZ_PHY_RX_DFE_NOTCH_ALPHA_MSK                           (((1U<<BZ_PHY_RX_DFE_NOTCH_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ALPHA_POS)
#define BZ_PHY_RX_DFE_NOTCH_ALPHA_UMSK                          (~(((1U<<BZ_PHY_RX_DFE_NOTCH_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_ALPHA_POS))
#define BZ_PHY_RX_DFE_NOTCH_FORCE                               BZ_PHY_RX_DFE_NOTCH_FORCE
#define BZ_PHY_RX_DFE_NOTCH_FORCE_POS                           (22U)
#define BZ_PHY_RX_DFE_NOTCH_FORCE_LEN                           (1U)
#define BZ_PHY_RX_DFE_NOTCH_FORCE_MSK                           (((1U<<BZ_PHY_RX_DFE_NOTCH_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_FORCE_POS)
#define BZ_PHY_RX_DFE_NOTCH_FORCE_UMSK                          (~(((1U<<BZ_PHY_RX_DFE_NOTCH_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_NOTCH_FORCE_POS))
#define BZ_PHY_RX_DFE_DCCAN_ALPHA                               BZ_PHY_RX_DFE_DCCAN_ALPHA
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_POS                           (23U)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN                           (2U)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_MSK                           (((1U<<BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_ALPHA_POS)
#define BZ_PHY_RX_DFE_DCCAN_ALPHA_UMSK                          (~(((1U<<BZ_PHY_RX_DFE_DCCAN_ALPHA_LEN)-1)<<BZ_PHY_RX_DFE_DCCAN_ALPHA_POS))

/* 0x81C : phy_rx_dfe_3 */
#define BZ_PHY_PHY_RX_DFE_3_OFFSET                              (0x81C)
#define BZ_PHY_RX_DFE_TOC_FAC                                   BZ_PHY_RX_DFE_TOC_FAC
#define BZ_PHY_RX_DFE_TOC_FAC_POS                               (0U)
#define BZ_PHY_RX_DFE_TOC_FAC_LEN                               (12U)
#define BZ_PHY_RX_DFE_TOC_FAC_MSK                               (((1U<<BZ_PHY_RX_DFE_TOC_FAC_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_POS)
#define BZ_PHY_RX_DFE_TOC_FAC_UMSK                              (~(((1U<<BZ_PHY_RX_DFE_TOC_FAC_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_POS))
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE                             BZ_PHY_RX_DFE_TOC_FAC_FORCE
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS                         (12U)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN                         (1U)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_MSK                         (((1U<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS)
#define BZ_PHY_RX_DFE_TOC_FAC_FORCE_UMSK                        (~(((1U<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_LEN)-1)<<BZ_PHY_RX_DFE_TOC_FAC_FORCE_POS))

/* 0x820 : phy_rx_zb_receiver_0 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_0_OFFSET                      (0x820)
#define BZ_PHY_ZB_TS_THRES_TIMING                               BZ_PHY_ZB_TS_THRES_TIMING
#define BZ_PHY_ZB_TS_THRES_TIMING_POS                           (0U)
#define BZ_PHY_ZB_TS_THRES_TIMING_LEN                           (3U)
#define BZ_PHY_ZB_TS_THRES_TIMING_MSK                           (((1U<<BZ_PHY_ZB_TS_THRES_TIMING_LEN)-1)<<BZ_PHY_ZB_TS_THRES_TIMING_POS)
#define BZ_PHY_ZB_TS_THRES_TIMING_UMSK                          (~(((1U<<BZ_PHY_ZB_TS_THRES_TIMING_LEN)-1)<<BZ_PHY_ZB_TS_THRES_TIMING_POS))
#define BZ_PHY_ZB_TS_THRES3                                     BZ_PHY_ZB_TS_THRES3
#define BZ_PHY_ZB_TS_THRES3_POS                                 (4U)
#define BZ_PHY_ZB_TS_THRES3_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES3_MSK                                 (((1U<<BZ_PHY_ZB_TS_THRES3_LEN)-1)<<BZ_PHY_ZB_TS_THRES3_POS)
#define BZ_PHY_ZB_TS_THRES3_UMSK                                (~(((1U<<BZ_PHY_ZB_TS_THRES3_LEN)-1)<<BZ_PHY_ZB_TS_THRES3_POS))
#define BZ_PHY_ZB_TS_THRES2                                     BZ_PHY_ZB_TS_THRES2
#define BZ_PHY_ZB_TS_THRES2_POS                                 (10U)
#define BZ_PHY_ZB_TS_THRES2_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES2_MSK                                 (((1U<<BZ_PHY_ZB_TS_THRES2_LEN)-1)<<BZ_PHY_ZB_TS_THRES2_POS)
#define BZ_PHY_ZB_TS_THRES2_UMSK                                (~(((1U<<BZ_PHY_ZB_TS_THRES2_LEN)-1)<<BZ_PHY_ZB_TS_THRES2_POS))
#define BZ_PHY_ZB_TS_THRES1                                     BZ_PHY_ZB_TS_THRES1
#define BZ_PHY_ZB_TS_THRES1_POS                                 (16U)
#define BZ_PHY_ZB_TS_THRES1_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES1_MSK                                 (((1U<<BZ_PHY_ZB_TS_THRES1_LEN)-1)<<BZ_PHY_ZB_TS_THRES1_POS)
#define BZ_PHY_ZB_TS_THRES1_UMSK                                (~(((1U<<BZ_PHY_ZB_TS_THRES1_LEN)-1)<<BZ_PHY_ZB_TS_THRES1_POS))
#define BZ_PHY_ZB_TS_THRES0                                     BZ_PHY_ZB_TS_THRES0
#define BZ_PHY_ZB_TS_THRES0_POS                                 (22U)
#define BZ_PHY_ZB_TS_THRES0_LEN                                 (6U)
#define BZ_PHY_ZB_TS_THRES0_MSK                                 (((1U<<BZ_PHY_ZB_TS_THRES0_LEN)-1)<<BZ_PHY_ZB_TS_THRES0_POS)
#define BZ_PHY_ZB_TS_THRES0_UMSK                                (~(((1U<<BZ_PHY_ZB_TS_THRES0_LEN)-1)<<BZ_PHY_ZB_TS_THRES0_POS))
#define BZ_PHY_ZB_TS_CC_ALPHA                                   BZ_PHY_ZB_TS_CC_ALPHA
#define BZ_PHY_ZB_TS_CC_ALPHA_POS                               (28U)
#define BZ_PHY_ZB_TS_CC_ALPHA_LEN                               (4U)
#define BZ_PHY_ZB_TS_CC_ALPHA_MSK                               (((1U<<BZ_PHY_ZB_TS_CC_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CC_ALPHA_POS)
#define BZ_PHY_ZB_TS_CC_ALPHA_UMSK                              (~(((1U<<BZ_PHY_ZB_TS_CC_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CC_ALPHA_POS))

/* 0x824 : phy_rx_zb_receiver_1 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_1_OFFSET                      (0x824)
#define BZ_PHY_ZB_DM_SFD_CNT_N                                  BZ_PHY_ZB_DM_SFD_CNT_N
#define BZ_PHY_ZB_DM_SFD_CNT_N_POS                              (0U)
#define BZ_PHY_ZB_DM_SFD_CNT_N_LEN                              (4U)
#define BZ_PHY_ZB_DM_SFD_CNT_N_MSK                              (((1U<<BZ_PHY_ZB_DM_SFD_CNT_N_LEN)-1)<<BZ_PHY_ZB_DM_SFD_CNT_N_POS)
#define BZ_PHY_ZB_DM_SFD_CNT_N_UMSK                             (~(((1U<<BZ_PHY_ZB_DM_SFD_CNT_N_LEN)-1)<<BZ_PHY_ZB_DM_SFD_CNT_N_POS))
#define BZ_PHY_ZB_DM_SFD_THRES                                  BZ_PHY_ZB_DM_SFD_THRES
#define BZ_PHY_ZB_DM_SFD_THRES_POS                              (4U)
#define BZ_PHY_ZB_DM_SFD_THRES_LEN                              (9U)
#define BZ_PHY_ZB_DM_SFD_THRES_MSK                              (((1U<<BZ_PHY_ZB_DM_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_POS)
#define BZ_PHY_ZB_DM_SFD_THRES_UMSK                             (~(((1U<<BZ_PHY_ZB_DM_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_POS))
#define BZ_PHY_ZB_DM_P_MODE_EN                                  BZ_PHY_ZB_DM_P_MODE_EN
#define BZ_PHY_ZB_DM_P_MODE_EN_POS                              (13U)
#define BZ_PHY_ZB_DM_P_MODE_EN_LEN                              (1U)
#define BZ_PHY_ZB_DM_P_MODE_EN_MSK                              (((1U<<BZ_PHY_ZB_DM_P_MODE_EN_LEN)-1)<<BZ_PHY_ZB_DM_P_MODE_EN_POS)
#define BZ_PHY_ZB_DM_P_MODE_EN_UMSK                             (~(((1U<<BZ_PHY_ZB_DM_P_MODE_EN_LEN)-1)<<BZ_PHY_ZB_DM_P_MODE_EN_POS))
#define BZ_PHY_ZB_FT_COEFF                                      BZ_PHY_ZB_FT_COEFF
#define BZ_PHY_ZB_FT_COEFF_POS                                  (16U)
#define BZ_PHY_ZB_FT_COEFF_LEN                                  (4U)
#define BZ_PHY_ZB_FT_COEFF_MSK                                  (((1U<<BZ_PHY_ZB_FT_COEFF_LEN)-1)<<BZ_PHY_ZB_FT_COEFF_POS)
#define BZ_PHY_ZB_FT_COEFF_UMSK                                 (~(((1U<<BZ_PHY_ZB_FT_COEFF_LEN)-1)<<BZ_PHY_ZB_FT_COEFF_POS))
#define BZ_PHY_ZB_DM_PLEN_MIN                                   BZ_PHY_ZB_DM_PLEN_MIN
#define BZ_PHY_ZB_DM_PLEN_MIN_POS                               (20U)
#define BZ_PHY_ZB_DM_PLEN_MIN_LEN                               (4U)
#define BZ_PHY_ZB_DM_PLEN_MIN_MSK                               (((1U<<BZ_PHY_ZB_DM_PLEN_MIN_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MIN_POS)
#define BZ_PHY_ZB_DM_PLEN_MIN_UMSK                              (~(((1U<<BZ_PHY_ZB_DM_PLEN_MIN_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MIN_POS))
#define BZ_PHY_ZB_DM_PLEN_MAX                                   BZ_PHY_ZB_DM_PLEN_MAX
#define BZ_PHY_ZB_DM_PLEN_MAX_POS                               (24U)
#define BZ_PHY_ZB_DM_PLEN_MAX_LEN                               (8U)
#define BZ_PHY_ZB_DM_PLEN_MAX_MSK                               (((1U<<BZ_PHY_ZB_DM_PLEN_MAX_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MAX_POS)
#define BZ_PHY_ZB_DM_PLEN_MAX_UMSK                              (~(((1U<<BZ_PHY_ZB_DM_PLEN_MAX_LEN)-1)<<BZ_PHY_ZB_DM_PLEN_MAX_POS))

/* 0x828 : phy_rx_zb_receiver_2 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_2_OFFSET                      (0x828)
#define BZ_PHY_ZB_TS_RL_THRES                                   BZ_PHY_ZB_TS_RL_THRES
#define BZ_PHY_ZB_TS_RL_THRES_POS                               (0U)
#define BZ_PHY_ZB_TS_RL_THRES_LEN                               (4U)
#define BZ_PHY_ZB_TS_RL_THRES_MSK                               (((1U<<BZ_PHY_ZB_TS_RL_THRES_LEN)-1)<<BZ_PHY_ZB_TS_RL_THRES_POS)
#define BZ_PHY_ZB_TS_RL_THRES_UMSK                              (~(((1U<<BZ_PHY_ZB_TS_RL_THRES_LEN)-1)<<BZ_PHY_ZB_TS_RL_THRES_POS))
#define BZ_PHY_ZB_TS_CCRL_ALPHA                                 BZ_PHY_ZB_TS_CCRL_ALPHA
#define BZ_PHY_ZB_TS_CCRL_ALPHA_POS                             (4U)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_LEN                             (4U)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_MSK                             (((1U<<BZ_PHY_ZB_TS_CCRL_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CCRL_ALPHA_POS)
#define BZ_PHY_ZB_TS_CCRL_ALPHA_UMSK                            (~(((1U<<BZ_PHY_ZB_TS_CCRL_ALPHA_LEN)-1)<<BZ_PHY_ZB_TS_CCRL_ALPHA_POS))
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE                         BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS                     (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN                     (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_MSK                     (((1U<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS)
#define BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_UMSK                    (~(((1U<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_P_MODE_POS))
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE                           BZ_PHY_ZB_DM_SFD_THRES_P_MODE
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS                       (16U)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN                       (8U)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_MSK                       (((1U<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS)
#define BZ_PHY_ZB_DM_SFD_THRES_P_MODE_UMSK                      (~(((1U<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_LEN)-1)<<BZ_PHY_ZB_DM_SFD_THRES_P_MODE_POS))
#define BZ_PHY_ZB_DM_H_SFD_THRES                                BZ_PHY_ZB_DM_H_SFD_THRES
#define BZ_PHY_ZB_DM_H_SFD_THRES_POS                            (24U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_LEN                            (8U)
#define BZ_PHY_ZB_DM_H_SFD_THRES_MSK                            (((1U<<BZ_PHY_ZB_DM_H_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_POS)
#define BZ_PHY_ZB_DM_H_SFD_THRES_UMSK                           (~(((1U<<BZ_PHY_ZB_DM_H_SFD_THRES_LEN)-1)<<BZ_PHY_ZB_DM_H_SFD_THRES_POS))

/* 0x82C : phy_rx_zb_receiver_3 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_3_OFFSET                      (0x82C)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME                               BZ_PHY_ZB_FSM_MF_DLY_TIME
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_POS                           (0U)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN                           (8U)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_MSK                           (((1U<<BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_MF_DLY_TIME_POS)
#define BZ_PHY_ZB_FSM_MF_DLY_TIME_UMSK                          (~(((1U<<BZ_PHY_ZB_FSM_MF_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_MF_DLY_TIME_POS))
#define BZ_PHY_ZB_FSM_DM_DLY_TIME                               BZ_PHY_ZB_FSM_DM_DLY_TIME
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_POS                           (8U)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN                           (10U)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_MSK                           (((1U<<BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_DM_DLY_TIME_POS)
#define BZ_PHY_ZB_FSM_DM_DLY_TIME_UMSK                          (~(((1U<<BZ_PHY_ZB_FSM_DM_DLY_TIME_LEN)-1)<<BZ_PHY_ZB_FSM_DM_DLY_TIME_POS))
#define BZ_PHY_ZB_FSM_CG_4S_ON                                  BZ_PHY_ZB_FSM_CG_4S_ON
#define BZ_PHY_ZB_FSM_CG_4S_ON_POS                              (19U)
#define BZ_PHY_ZB_FSM_CG_4S_ON_LEN                              (1U)
#define BZ_PHY_ZB_FSM_CG_4S_ON_MSK                              (((1U<<BZ_PHY_ZB_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_ZB_FSM_CG_4S_ON_POS)
#define BZ_PHY_ZB_FSM_CG_4S_ON_UMSK                             (~(((1U<<BZ_PHY_ZB_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_ZB_FSM_CG_4S_ON_POS))
#define BZ_PHY_ZB_FSM_LO_TIME_US                                BZ_PHY_ZB_FSM_LO_TIME_US
#define BZ_PHY_ZB_FSM_LO_TIME_US_POS                            (20U)
#define BZ_PHY_ZB_FSM_LO_TIME_US_LEN                            (7U)
#define BZ_PHY_ZB_FSM_LO_TIME_US_MSK                            (((1U<<BZ_PHY_ZB_FSM_LO_TIME_US_LEN)-1)<<BZ_PHY_ZB_FSM_LO_TIME_US_POS)
#define BZ_PHY_ZB_FSM_LO_TIME_US_UMSK                           (~(((1U<<BZ_PHY_ZB_FSM_LO_TIME_US_LEN)-1)<<BZ_PHY_ZB_FSM_LO_TIME_US_POS))
#define BZ_PHY_ZB_FSM_CCA_ENABLE                                BZ_PHY_ZB_FSM_CCA_ENABLE
#define BZ_PHY_ZB_FSM_CCA_ENABLE_POS                            (27U)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_LEN                            (1U)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_MSK                            (((1U<<BZ_PHY_ZB_FSM_CCA_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_CCA_ENABLE_POS)
#define BZ_PHY_ZB_FSM_CCA_ENABLE_UMSK                           (~(((1U<<BZ_PHY_ZB_FSM_CCA_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_CCA_ENABLE_POS))
#define BZ_PHY_ZB_FSM_ED_ENABLE                                 BZ_PHY_ZB_FSM_ED_ENABLE
#define BZ_PHY_ZB_FSM_ED_ENABLE_POS                             (28U)
#define BZ_PHY_ZB_FSM_ED_ENABLE_LEN                             (1U)
#define BZ_PHY_ZB_FSM_ED_ENABLE_MSK                             (((1U<<BZ_PHY_ZB_FSM_ED_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_ED_ENABLE_POS)
#define BZ_PHY_ZB_FSM_ED_ENABLE_UMSK                            (~(((1U<<BZ_PHY_ZB_FSM_ED_ENABLE_LEN)-1)<<BZ_PHY_ZB_FSM_ED_ENABLE_POS))
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN                           BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS                       (29U)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_MSK                       (((1U<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS)
#define BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_UMSK                      (~(((1U<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SFD_TIME_OUT_EN_POS))
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN                           BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS                       (30U)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_MSK                       (((1U<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN)-1)<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS)
#define BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_UMSK                      (~(((1U<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_LEN)-1)<<BZ_PHY_ZB_FSM_RX_LISTEN_ST_EN_POS))
#define BZ_PHY_ZB_FSM_EN                                        BZ_PHY_ZB_FSM_EN
#define BZ_PHY_ZB_FSM_EN_POS                                    (31U)
#define BZ_PHY_ZB_FSM_EN_LEN                                    (1U)
#define BZ_PHY_ZB_FSM_EN_MSK                                    (((1U<<BZ_PHY_ZB_FSM_EN_LEN)-1)<<BZ_PHY_ZB_FSM_EN_POS)
#define BZ_PHY_ZB_FSM_EN_UMSK                                   (~(((1U<<BZ_PHY_ZB_FSM_EN_LEN)-1)<<BZ_PHY_ZB_FSM_EN_POS))

/* 0x830 : phy_rx_zb_receiver_4 */
#define BZ_PHY_PHY_RX_ZB_RECEIVER_4_OFFSET                      (0x830)
#define BZ_PHY_ZB_FT_INIT_VAL                                   BZ_PHY_ZB_FT_INIT_VAL
#define BZ_PHY_ZB_FT_INIT_VAL_POS                               (0U)
#define BZ_PHY_ZB_FT_INIT_VAL_LEN                               (13U)
#define BZ_PHY_ZB_FT_INIT_VAL_MSK                               (((1U<<BZ_PHY_ZB_FT_INIT_VAL_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_POS)
#define BZ_PHY_ZB_FT_INIT_VAL_UMSK                              (~(((1U<<BZ_PHY_ZB_FT_INIT_VAL_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_POS))
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE                             BZ_PHY_ZB_FT_INIT_VAL_FORCE
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS                         (13U)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN                         (1U)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_MSK                         (((1U<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS)
#define BZ_PHY_ZB_FT_INIT_VAL_FORCE_UMSK                        (~(((1U<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_LEN)-1)<<BZ_PHY_ZB_FT_INIT_VAL_FORCE_POS))
#define BZ_PHY_ZB_DM_ALPHA_2000                                 BZ_PHY_ZB_DM_ALPHA_2000
#define BZ_PHY_ZB_DM_ALPHA_2000_POS                             (14U)
#define BZ_PHY_ZB_DM_ALPHA_2000_LEN                             (4U)
#define BZ_PHY_ZB_DM_ALPHA_2000_MSK                             (((1U<<BZ_PHY_ZB_DM_ALPHA_2000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_2000_POS)
#define BZ_PHY_ZB_DM_ALPHA_2000_UMSK                            (~(((1U<<BZ_PHY_ZB_DM_ALPHA_2000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_2000_POS))
#define BZ_PHY_ZB_DM_ALPHA_1000                                 BZ_PHY_ZB_DM_ALPHA_1000
#define BZ_PHY_ZB_DM_ALPHA_1000_POS                             (18U)
#define BZ_PHY_ZB_DM_ALPHA_1000_LEN                             (4U)
#define BZ_PHY_ZB_DM_ALPHA_1000_MSK                             (((1U<<BZ_PHY_ZB_DM_ALPHA_1000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_1000_POS)
#define BZ_PHY_ZB_DM_ALPHA_1000_UMSK                            (~(((1U<<BZ_PHY_ZB_DM_ALPHA_1000_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_1000_POS))
#define BZ_PHY_ZB_DM_ALPHA_500                                  BZ_PHY_ZB_DM_ALPHA_500
#define BZ_PHY_ZB_DM_ALPHA_500_POS                              (22U)
#define BZ_PHY_ZB_DM_ALPHA_500_LEN                              (4U)
#define BZ_PHY_ZB_DM_ALPHA_500_MSK                              (((1U<<BZ_PHY_ZB_DM_ALPHA_500_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_500_POS)
#define BZ_PHY_ZB_DM_ALPHA_500_UMSK                             (~(((1U<<BZ_PHY_ZB_DM_ALPHA_500_LEN)-1)<<BZ_PHY_ZB_DM_ALPHA_500_POS))
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS                           BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS                       (26U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN                       (5U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_MSK                       (((1U<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_UMSK                      (~(((1U<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_MS_POS))
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN                           BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS                       (31U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN                       (1U)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_MSK                       (((1U<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS)
#define BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_UMSK                      (~(((1U<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_LEN)-1)<<BZ_PHY_ZB_FSM_SYNC_TIMEOUT_EN_POS))

/* 0x834 : phy_cca_0 */
#define BZ_PHY_PHY_CCA_0_OFFSET                                 (0x834)
#define BZ_PHY_CCA_ST_INT_SET                                   BZ_PHY_CCA_ST_INT_SET
#define BZ_PHY_CCA_ST_INT_SET_POS                               (0U)
#define BZ_PHY_CCA_ST_INT_SET_LEN                               (1U)
#define BZ_PHY_CCA_ST_INT_SET_MSK                               (((1U<<BZ_PHY_CCA_ST_INT_SET_LEN)-1)<<BZ_PHY_CCA_ST_INT_SET_POS)
#define BZ_PHY_CCA_ST_INT_SET_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_INT_SET_LEN)-1)<<BZ_PHY_CCA_ST_INT_SET_POS))
#define BZ_PHY_CCA_ST_INT_CLR                                   BZ_PHY_CCA_ST_INT_CLR
#define BZ_PHY_CCA_ST_INT_CLR_POS                               (1U)
#define BZ_PHY_CCA_ST_INT_CLR_LEN                               (1U)
#define BZ_PHY_CCA_ST_INT_CLR_MSK                               (((1U<<BZ_PHY_CCA_ST_INT_CLR_LEN)-1)<<BZ_PHY_CCA_ST_INT_CLR_POS)
#define BZ_PHY_CCA_ST_INT_CLR_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_INT_CLR_LEN)-1)<<BZ_PHY_CCA_ST_INT_CLR_POS))
#define BZ_PHY_CCA_ST_INT                                       BZ_PHY_CCA_ST_INT
#define BZ_PHY_CCA_ST_INT_POS                                   (2U)
#define BZ_PHY_CCA_ST_INT_LEN                                   (1U)
#define BZ_PHY_CCA_ST_INT_MSK                                   (((1U<<BZ_PHY_CCA_ST_INT_LEN)-1)<<BZ_PHY_CCA_ST_INT_POS)
#define BZ_PHY_CCA_ST_INT_UMSK                                  (~(((1U<<BZ_PHY_CCA_ST_INT_LEN)-1)<<BZ_PHY_CCA_ST_INT_POS))
#define BZ_PHY_CCA_ST_DM_FLAG                                   BZ_PHY_CCA_ST_DM_FLAG
#define BZ_PHY_CCA_ST_DM_FLAG_POS                               (4U)
#define BZ_PHY_CCA_ST_DM_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_DM_FLAG_MSK                               (((1U<<BZ_PHY_CCA_ST_DM_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_DM_FLAG_POS)
#define BZ_PHY_CCA_ST_DM_FLAG_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_DM_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_DM_FLAG_POS))
#define BZ_PHY_CCA_ST_CS_FLAG                                   BZ_PHY_CCA_ST_CS_FLAG
#define BZ_PHY_CCA_ST_CS_FLAG_POS                               (5U)
#define BZ_PHY_CCA_ST_CS_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_CS_FLAG_MSK                               (((1U<<BZ_PHY_CCA_ST_CS_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CS_FLAG_POS)
#define BZ_PHY_CCA_ST_CS_FLAG_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_CS_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CS_FLAG_POS))
#define BZ_PHY_CCA_ST_ED_FLAG                                   BZ_PHY_CCA_ST_ED_FLAG
#define BZ_PHY_CCA_ST_ED_FLAG_POS                               (6U)
#define BZ_PHY_CCA_ST_ED_FLAG_LEN                               (1U)
#define BZ_PHY_CCA_ST_ED_FLAG_MSK                               (((1U<<BZ_PHY_CCA_ST_ED_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_ED_FLAG_POS)
#define BZ_PHY_CCA_ST_ED_FLAG_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_ED_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_ED_FLAG_POS))
#define BZ_PHY_CCA_ST_CCA_FLAG                                  BZ_PHY_CCA_ST_CCA_FLAG
#define BZ_PHY_CCA_ST_CCA_FLAG_POS                              (7U)
#define BZ_PHY_CCA_ST_CCA_FLAG_LEN                              (1U)
#define BZ_PHY_CCA_ST_CCA_FLAG_MSK                              (((1U<<BZ_PHY_CCA_ST_CCA_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CCA_FLAG_POS)
#define BZ_PHY_CCA_ST_CCA_FLAG_UMSK                             (~(((1U<<BZ_PHY_CCA_ST_CCA_FLAG_LEN)-1)<<BZ_PHY_CCA_ST_CCA_FLAG_POS))
#define BZ_PHY_CCA_ST_ED_MEAS                                   BZ_PHY_CCA_ST_ED_MEAS
#define BZ_PHY_CCA_ST_ED_MEAS_POS                               (8U)
#define BZ_PHY_CCA_ST_ED_MEAS_LEN                               (7U)
#define BZ_PHY_CCA_ST_ED_MEAS_MSK                               (((1U<<BZ_PHY_CCA_ST_ED_MEAS_LEN)-1)<<BZ_PHY_CCA_ST_ED_MEAS_POS)
#define BZ_PHY_CCA_ST_ED_MEAS_UMSK                              (~(((1U<<BZ_PHY_CCA_ST_ED_MEAS_LEN)-1)<<BZ_PHY_CCA_ST_ED_MEAS_POS))
#define BZ_PHY_PHY_SW_CCA_EN_SEL                                BZ_PHY_PHY_SW_CCA_EN_SEL
#define BZ_PHY_PHY_SW_CCA_EN_SEL_POS                            (15U)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_LEN                            (1U)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_MSK                            (((1U<<BZ_PHY_PHY_SW_CCA_EN_SEL_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_SEL_POS)
#define BZ_PHY_PHY_SW_CCA_EN_SEL_UMSK                           (~(((1U<<BZ_PHY_PHY_SW_CCA_EN_SEL_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_SEL_POS))
#define BZ_PHY_PHY_CCA_ED_TH                                    BZ_PHY_PHY_CCA_ED_TH
#define BZ_PHY_PHY_CCA_ED_TH_POS                                (16U)
#define BZ_PHY_PHY_CCA_ED_TH_LEN                                (7U)
#define BZ_PHY_PHY_CCA_ED_TH_MSK                                (((1U<<BZ_PHY_PHY_CCA_ED_TH_LEN)-1)<<BZ_PHY_PHY_CCA_ED_TH_POS)
#define BZ_PHY_PHY_CCA_ED_TH_UMSK                               (~(((1U<<BZ_PHY_PHY_CCA_ED_TH_LEN)-1)<<BZ_PHY_PHY_CCA_ED_TH_POS))
#define BZ_PHY_PHY_CCA_FOUND_SRC                                BZ_PHY_PHY_CCA_FOUND_SRC
#define BZ_PHY_PHY_CCA_FOUND_SRC_POS                            (23U)
#define BZ_PHY_PHY_CCA_FOUND_SRC_LEN                            (1U)
#define BZ_PHY_PHY_CCA_FOUND_SRC_MSK                            (((1U<<BZ_PHY_PHY_CCA_FOUND_SRC_LEN)-1)<<BZ_PHY_PHY_CCA_FOUND_SRC_POS)
#define BZ_PHY_PHY_CCA_FOUND_SRC_UMSK                           (~(((1U<<BZ_PHY_PHY_CCA_FOUND_SRC_LEN)-1)<<BZ_PHY_PHY_CCA_FOUND_SRC_POS))
#define BZ_PHY_PHY_CCA_DUR                                      BZ_PHY_PHY_CCA_DUR
#define BZ_PHY_PHY_CCA_DUR_POS                                  (24U)
#define BZ_PHY_PHY_CCA_DUR_LEN                                  (3U)
#define BZ_PHY_PHY_CCA_DUR_MSK                                  (((1U<<BZ_PHY_PHY_CCA_DUR_LEN)-1)<<BZ_PHY_PHY_CCA_DUR_POS)
#define BZ_PHY_PHY_CCA_DUR_UMSK                                 (~(((1U<<BZ_PHY_PHY_CCA_DUR_LEN)-1)<<BZ_PHY_PHY_CCA_DUR_POS))
#define BZ_PHY_PHY_CCA_MODE                                     BZ_PHY_PHY_CCA_MODE
#define BZ_PHY_PHY_CCA_MODE_POS                                 (27U)
#define BZ_PHY_PHY_CCA_MODE_LEN                                 (2U)
#define BZ_PHY_PHY_CCA_MODE_MSK                                 (((1U<<BZ_PHY_PHY_CCA_MODE_LEN)-1)<<BZ_PHY_PHY_CCA_MODE_POS)
#define BZ_PHY_PHY_CCA_MODE_UMSK                                (~(((1U<<BZ_PHY_PHY_CCA_MODE_LEN)-1)<<BZ_PHY_PHY_CCA_MODE_POS))
#define BZ_PHY_PHY_CCA_EN                                       BZ_PHY_PHY_CCA_EN
#define BZ_PHY_PHY_CCA_EN_POS                                   (29U)
#define BZ_PHY_PHY_CCA_EN_LEN                                   (1U)
#define BZ_PHY_PHY_CCA_EN_MSK                                   (((1U<<BZ_PHY_PHY_CCA_EN_LEN)-1)<<BZ_PHY_PHY_CCA_EN_POS)
#define BZ_PHY_PHY_CCA_EN_UMSK                                  (~(((1U<<BZ_PHY_PHY_CCA_EN_LEN)-1)<<BZ_PHY_PHY_CCA_EN_POS))
#define BZ_PHY_PHY_SW_CCA_EN                                    BZ_PHY_PHY_SW_CCA_EN
#define BZ_PHY_PHY_SW_CCA_EN_POS                                (30U)
#define BZ_PHY_PHY_SW_CCA_EN_LEN                                (1U)
#define BZ_PHY_PHY_SW_CCA_EN_MSK                                (((1U<<BZ_PHY_PHY_SW_CCA_EN_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_POS)
#define BZ_PHY_PHY_SW_CCA_EN_UMSK                               (~(((1U<<BZ_PHY_PHY_SW_CCA_EN_LEN)-1)<<BZ_PHY_PHY_SW_CCA_EN_POS))
#define BZ_PHY_PHY_SW_ED_EN                                     BZ_PHY_PHY_SW_ED_EN
#define BZ_PHY_PHY_SW_ED_EN_POS                                 (31U)
#define BZ_PHY_PHY_SW_ED_EN_LEN                                 (1U)
#define BZ_PHY_PHY_SW_ED_EN_MSK                                 (((1U<<BZ_PHY_PHY_SW_ED_EN_LEN)-1)<<BZ_PHY_PHY_SW_ED_EN_POS)
#define BZ_PHY_PHY_SW_ED_EN_UMSK                                (~(((1U<<BZ_PHY_PHY_SW_ED_EN_LEN)-1)<<BZ_PHY_PHY_SW_ED_EN_POS))

/* 0x838 : phy_ble_sync_0 */
#define BZ_PHY_PHY_BLE_SYNC_0_OFFSET                            (0x838)
#define BZ_PHY_T2F_MAG_TH                                       BZ_PHY_T2F_MAG_TH
#define BZ_PHY_T2F_MAG_TH_POS                                   (0U)
#define BZ_PHY_T2F_MAG_TH_LEN                                   (9U)
#define BZ_PHY_T2F_MAG_TH_MSK                                   (((1U<<BZ_PHY_T2F_MAG_TH_LEN)-1)<<BZ_PHY_T2F_MAG_TH_POS)
#define BZ_PHY_T2F_MAG_TH_UMSK                                  (~(((1U<<BZ_PHY_T2F_MAG_TH_LEN)-1)<<BZ_PHY_T2F_MAG_TH_POS))
#define BZ_PHY_BLEC_CFOE_ALPHA                                  BZ_PHY_BLEC_CFOE_ALPHA
#define BZ_PHY_BLEC_CFOE_ALPHA_POS                              (10U)
#define BZ_PHY_BLEC_CFOE_ALPHA_LEN                              (3U)
#define BZ_PHY_BLEC_CFOE_ALPHA_MSK                              (((1U<<BZ_PHY_BLEC_CFOE_ALPHA_LEN)-1)<<BZ_PHY_BLEC_CFOE_ALPHA_POS)
#define BZ_PHY_BLEC_CFOE_ALPHA_UMSK                             (~(((1U<<BZ_PHY_BLEC_CFOE_ALPHA_LEN)-1)<<BZ_PHY_BLEC_CFOE_ALPHA_POS))
#define BZ_PHY_BLE_SYNC_CORR_AVG_L                              BZ_PHY_BLE_SYNC_CORR_AVG_L
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_POS                          (13U)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN                          (5U)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_MSK                          (((1U<<BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN)-1)<<BZ_PHY_BLE_SYNC_CORR_AVG_L_POS)
#define BZ_PHY_BLE_SYNC_CORR_AVG_L_UMSK                         (~(((1U<<BZ_PHY_BLE_SYNC_CORR_AVG_L_LEN)-1)<<BZ_PHY_BLE_SYNC_CORR_AVG_L_POS))
#define BZ_PHY_BLE_CFOE_AVG_L                                   BZ_PHY_BLE_CFOE_AVG_L
#define BZ_PHY_BLE_CFOE_AVG_L_POS                               (18U)
#define BZ_PHY_BLE_CFOE_AVG_L_LEN                               (4U)
#define BZ_PHY_BLE_CFOE_AVG_L_MSK                               (((1U<<BZ_PHY_BLE_CFOE_AVG_L_LEN)-1)<<BZ_PHY_BLE_CFOE_AVG_L_POS)
#define BZ_PHY_BLE_CFOE_AVG_L_UMSK                              (~(((1U<<BZ_PHY_BLE_CFOE_AVG_L_LEN)-1)<<BZ_PHY_BLE_CFOE_AVG_L_POS))
#define BZ_PHY_BLE_AA_OFFSET_COEF                               BZ_PHY_BLE_AA_OFFSET_COEF
#define BZ_PHY_BLE_AA_OFFSET_COEF_POS                           (22U)
#define BZ_PHY_BLE_AA_OFFSET_COEF_LEN                           (5U)
#define BZ_PHY_BLE_AA_OFFSET_COEF_MSK                           (((1U<<BZ_PHY_BLE_AA_OFFSET_COEF_LEN)-1)<<BZ_PHY_BLE_AA_OFFSET_COEF_POS)
#define BZ_PHY_BLE_AA_OFFSET_COEF_UMSK                          (~(((1U<<BZ_PHY_BLE_AA_OFFSET_COEF_LEN)-1)<<BZ_PHY_BLE_AA_OFFSET_COEF_POS))

/* 0x83C : phy_ble_sync_1 */
#define BZ_PHY_PHY_BLE_SYNC_1_OFFSET                            (0x83C)
#define BZ_PHY_T2F_LIMITE_TH                                    BZ_PHY_T2F_LIMITE_TH
#define BZ_PHY_T2F_LIMITE_TH_POS                                (0U)
#define BZ_PHY_T2F_LIMITE_TH_LEN                                (13U)
#define BZ_PHY_T2F_LIMITE_TH_MSK                                (((1U<<BZ_PHY_T2F_LIMITE_TH_LEN)-1)<<BZ_PHY_T2F_LIMITE_TH_POS)
#define BZ_PHY_T2F_LIMITE_TH_UMSK                               (~(((1U<<BZ_PHY_T2F_LIMITE_TH_LEN)-1)<<BZ_PHY_T2F_LIMITE_TH_POS))
#define BZ_PHY_T2F_IMPULSE_TH                                   BZ_PHY_T2F_IMPULSE_TH
#define BZ_PHY_T2F_IMPULSE_TH_POS                               (13U)
#define BZ_PHY_T2F_IMPULSE_TH_LEN                               (14U)
#define BZ_PHY_T2F_IMPULSE_TH_MSK                               (((1U<<BZ_PHY_T2F_IMPULSE_TH_LEN)-1)<<BZ_PHY_T2F_IMPULSE_TH_POS)
#define BZ_PHY_T2F_IMPULSE_TH_UMSK                              (~(((1U<<BZ_PHY_T2F_IMPULSE_TH_LEN)-1)<<BZ_PHY_T2F_IMPULSE_TH_POS))
#define BZ_PHY_T2F_FILTER_MODE                                  BZ_PHY_T2F_FILTER_MODE
#define BZ_PHY_T2F_FILTER_MODE_POS                              (27U)
#define BZ_PHY_T2F_FILTER_MODE_LEN                              (1U)
#define BZ_PHY_T2F_FILTER_MODE_MSK                              (((1U<<BZ_PHY_T2F_FILTER_MODE_LEN)-1)<<BZ_PHY_T2F_FILTER_MODE_POS)
#define BZ_PHY_T2F_FILTER_MODE_UMSK                             (~(((1U<<BZ_PHY_T2F_FILTER_MODE_LEN)-1)<<BZ_PHY_T2F_FILTER_MODE_POS))
#define BZ_PHY_T2F_FILTER_EN                                    BZ_PHY_T2F_FILTER_EN
#define BZ_PHY_T2F_FILTER_EN_POS                                (28U)
#define BZ_PHY_T2F_FILTER_EN_LEN                                (1U)
#define BZ_PHY_T2F_FILTER_EN_MSK                                (((1U<<BZ_PHY_T2F_FILTER_EN_LEN)-1)<<BZ_PHY_T2F_FILTER_EN_POS)
#define BZ_PHY_T2F_FILTER_EN_UMSK                               (~(((1U<<BZ_PHY_T2F_FILTER_EN_LEN)-1)<<BZ_PHY_T2F_FILTER_EN_POS))

/* 0x840 : phy_reserved_0 */
#define BZ_PHY_PHY_RESERVED_0_OFFSET                            (0x840)
#define BZ_PHY_PHY_RESERVED_0                                   BZ_PHY_PHY_RESERVED_0
#define BZ_PHY_PHY_RESERVED_0_POS                               (0U)
#define BZ_PHY_PHY_RESERVED_0_LEN                               (32U)
#define BZ_PHY_PHY_RESERVED_0_MSK                               (((1U<<BZ_PHY_PHY_RESERVED_0_LEN)-1)<<BZ_PHY_PHY_RESERVED_0_POS)
#define BZ_PHY_PHY_RESERVED_0_UMSK                              (~(((1U<<BZ_PHY_PHY_RESERVED_0_LEN)-1)<<BZ_PHY_PHY_RESERVED_0_POS))

/* 0x844 : phy_rx_proc_time_1 */
#define BZ_PHY_PHY_RX_PROC_TIME_1_OFFSET                        (0x844)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K                     BZ_PHY_RX_PROC_TIME_VITERBI_US_500K
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS                 (0U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN                 (8U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_MSK                 (((1U<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_UMSK                (~(((1U<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_500K_POS))
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M                          BZ_PHY_RX_PROC_TIME_MLSD_US_2M
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_MSK                      (((1U<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_2M_UMSK                     (~(((1U<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_2M_POS))
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M                            BZ_PHY_RX_PROC_TIME_EQ_US_2M
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS                        (16U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN                        (8U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_MSK                        (((1U<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS)
#define BZ_PHY_RX_PROC_TIME_EQ_US_2M_UMSK                       (~(((1U<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_2M_POS))

/* 0x848 : phy_ble_dfe_0 */
#define BZ_PHY_PHY_BLE_DFE_0_OFFSET                             (0x848)
#define BZ_PHY_FB_COEF_0                                        BZ_PHY_FB_COEF_0
#define BZ_PHY_FB_COEF_0_POS                                    (0U)
#define BZ_PHY_FB_COEF_0_LEN                                    (8U)
#define BZ_PHY_FB_COEF_0_MSK                                    (((1U<<BZ_PHY_FB_COEF_0_LEN)-1)<<BZ_PHY_FB_COEF_0_POS)
#define BZ_PHY_FB_COEF_0_UMSK                                   (~(((1U<<BZ_PHY_FB_COEF_0_LEN)-1)<<BZ_PHY_FB_COEF_0_POS))
#define BZ_PHY_FF_COEF_2                                        BZ_PHY_FF_COEF_2
#define BZ_PHY_FF_COEF_2_POS                                    (8U)
#define BZ_PHY_FF_COEF_2_LEN                                    (8U)
#define BZ_PHY_FF_COEF_2_MSK                                    (((1U<<BZ_PHY_FF_COEF_2_LEN)-1)<<BZ_PHY_FF_COEF_2_POS)
#define BZ_PHY_FF_COEF_2_UMSK                                   (~(((1U<<BZ_PHY_FF_COEF_2_LEN)-1)<<BZ_PHY_FF_COEF_2_POS))
#define BZ_PHY_FF_COEF_1                                        BZ_PHY_FF_COEF_1
#define BZ_PHY_FF_COEF_1_POS                                    (16U)
#define BZ_PHY_FF_COEF_1_LEN                                    (8U)
#define BZ_PHY_FF_COEF_1_MSK                                    (((1U<<BZ_PHY_FF_COEF_1_LEN)-1)<<BZ_PHY_FF_COEF_1_POS)
#define BZ_PHY_FF_COEF_1_UMSK                                   (~(((1U<<BZ_PHY_FF_COEF_1_LEN)-1)<<BZ_PHY_FF_COEF_1_POS))
#define BZ_PHY_FF_COEF_0                                        BZ_PHY_FF_COEF_0
#define BZ_PHY_FF_COEF_0_POS                                    (24U)
#define BZ_PHY_FF_COEF_0_LEN                                    (8U)
#define BZ_PHY_FF_COEF_0_MSK                                    (((1U<<BZ_PHY_FF_COEF_0_LEN)-1)<<BZ_PHY_FF_COEF_0_POS)
#define BZ_PHY_FF_COEF_0_UMSK                                   (~(((1U<<BZ_PHY_FF_COEF_0_LEN)-1)<<BZ_PHY_FF_COEF_0_POS))

/* 0x84C : phy_ble_sync_2 */
#define BZ_PHY_PHY_BLE_SYNC_2_OFFSET                            (0x84C)
#define BZ_PHY_BLE_AA_CORR_THRES                                BZ_PHY_BLE_AA_CORR_THRES
#define BZ_PHY_BLE_AA_CORR_THRES_POS                            (0U)
#define BZ_PHY_BLE_AA_CORR_THRES_LEN                            (12U)
#define BZ_PHY_BLE_AA_CORR_THRES_MSK                            (((1U<<BZ_PHY_BLE_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLE_AA_CORR_THRES_POS)
#define BZ_PHY_BLE_AA_CORR_THRES_UMSK                           (~(((1U<<BZ_PHY_BLE_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLE_AA_CORR_THRES_POS))
#define BZ_PHY_BLE_SEARCH_OFFSET                                BZ_PHY_BLE_SEARCH_OFFSET
#define BZ_PHY_BLE_SEARCH_OFFSET_POS                            (12U)
#define BZ_PHY_BLE_SEARCH_OFFSET_LEN                            (5U)
#define BZ_PHY_BLE_SEARCH_OFFSET_MSK                            (((1U<<BZ_PHY_BLE_SEARCH_OFFSET_LEN)-1)<<BZ_PHY_BLE_SEARCH_OFFSET_POS)
#define BZ_PHY_BLE_SEARCH_OFFSET_UMSK                           (~(((1U<<BZ_PHY_BLE_SEARCH_OFFSET_LEN)-1)<<BZ_PHY_BLE_SEARCH_OFFSET_POS))

/* 0x850 : phy_ble_fsm_0 */
#define BZ_PHY_PHY_BLE_FSM_0_OFFSET                             (0x850)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB                              BZ_PHY_BLE_RX_CFO_ADJ_SYMB
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS                          (0U)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN                          (8U)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_MSK                          (((1U<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN)-1)<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS)
#define BZ_PHY_BLE_RX_CFO_ADJ_SYMB_UMSK                         (~(((1U<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_LEN)-1)<<BZ_PHY_BLE_RX_CFO_ADJ_SYMB_POS))
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M                             BZ_PHY_BLE_FSM_RX_DMTIME_2M
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_MSK                         (((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_2M_UMSK                        (~(((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_2M_POS))
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M                             BZ_PHY_BLE_FSM_RX_DMTIME_1M
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS                         (16U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_MSK                         (((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_1M_UMSK                        (~(((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_1M_POS))
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY                             BZ_PHY_BLE_FSM_CFO_PEAK_DLY
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS                         (24U)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN                         (4U)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_MSK                         (((1U<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN)-1)<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS)
#define BZ_PHY_BLE_FSM_CFO_PEAK_DLY_UMSK                        (~(((1U<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_LEN)-1)<<BZ_PHY_BLE_FSM_CFO_PEAK_DLY_POS))
#define BZ_PHY_BLE_DEMOD_MODE                                   BZ_PHY_BLE_DEMOD_MODE
#define BZ_PHY_BLE_DEMOD_MODE_POS                               (28U)
#define BZ_PHY_BLE_DEMOD_MODE_LEN                               (2U)
#define BZ_PHY_BLE_DEMOD_MODE_MSK                               (((1U<<BZ_PHY_BLE_DEMOD_MODE_LEN)-1)<<BZ_PHY_BLE_DEMOD_MODE_POS)
#define BZ_PHY_BLE_DEMOD_MODE_UMSK                              (~(((1U<<BZ_PHY_BLE_DEMOD_MODE_LEN)-1)<<BZ_PHY_BLE_DEMOD_MODE_POS))
#define BZ_PHY_BLE_FSM_CG_4S_ON                                 BZ_PHY_BLE_FSM_CG_4S_ON
#define BZ_PHY_BLE_FSM_CG_4S_ON_POS                             (30U)
#define BZ_PHY_BLE_FSM_CG_4S_ON_LEN                             (1U)
#define BZ_PHY_BLE_FSM_CG_4S_ON_MSK                             (((1U<<BZ_PHY_BLE_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_BLE_FSM_CG_4S_ON_POS)
#define BZ_PHY_BLE_FSM_CG_4S_ON_UMSK                            (~(((1U<<BZ_PHY_BLE_FSM_CG_4S_ON_LEN)-1)<<BZ_PHY_BLE_FSM_CG_4S_ON_POS))
#define BZ_PHY_BLE_FSM_EN                                       BZ_PHY_BLE_FSM_EN
#define BZ_PHY_BLE_FSM_EN_POS                                   (31U)
#define BZ_PHY_BLE_FSM_EN_LEN                                   (1U)
#define BZ_PHY_BLE_FSM_EN_MSK                                   (((1U<<BZ_PHY_BLE_FSM_EN_LEN)-1)<<BZ_PHY_BLE_FSM_EN_POS)
#define BZ_PHY_BLE_FSM_EN_UMSK                                  (~(((1U<<BZ_PHY_BLE_FSM_EN_LEN)-1)<<BZ_PHY_BLE_FSM_EN_POS))

/* 0x854 : phy_rx_proc_time_0 */
#define BZ_PHY_PHY_RX_PROC_TIME_0_OFFSET                        (0x854)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K                     BZ_PHY_RX_PROC_TIME_VITERBI_US_125K
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS                 (0U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN                 (8U)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_MSK                 (((1U<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS)
#define BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_UMSK                (~(((1U<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_LEN)-1)<<BZ_PHY_RX_PROC_TIME_VITERBI_US_125K_POS))
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M                          BZ_PHY_RX_PROC_TIME_MLSD_US_1M
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN                      (8U)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_MSK                      (((1U<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS)
#define BZ_PHY_RX_PROC_TIME_MLSD_US_1M_UMSK                     (~(((1U<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_MLSD_US_1M_POS))
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M                            BZ_PHY_RX_PROC_TIME_EQ_US_1M
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS                        (16U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN                        (8U)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_MSK                        (((1U<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS)
#define BZ_PHY_RX_PROC_TIME_EQ_US_1M_UMSK                       (~(((1U<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_LEN)-1)<<BZ_PHY_RX_PROC_TIME_EQ_US_1M_POS))
#define BZ_PHY_RX_PROC_TIME_M154_US                             BZ_PHY_RX_PROC_TIME_M154_US
#define BZ_PHY_RX_PROC_TIME_M154_US_POS                         (24U)
#define BZ_PHY_RX_PROC_TIME_M154_US_LEN                         (8U)
#define BZ_PHY_RX_PROC_TIME_M154_US_MSK                         (((1U<<BZ_PHY_RX_PROC_TIME_M154_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_M154_US_POS)
#define BZ_PHY_RX_PROC_TIME_M154_US_UMSK                        (~(((1U<<BZ_PHY_RX_PROC_TIME_M154_US_LEN)-1)<<BZ_PHY_RX_PROC_TIME_M154_US_POS))

/* 0x858 : phy_testbus_0 */
#define BZ_PHY_PHY_TESTBUS_0_OFFSET                             (0x858)
#define BZ_PHY_PHY_TESTBUS_READ                                 BZ_PHY_PHY_TESTBUS_READ
#define BZ_PHY_PHY_TESTBUS_READ_POS                             (0U)
#define BZ_PHY_PHY_TESTBUS_READ_LEN                             (32U)
#define BZ_PHY_PHY_TESTBUS_READ_MSK                             (((1U<<BZ_PHY_PHY_TESTBUS_READ_LEN)-1)<<BZ_PHY_PHY_TESTBUS_READ_POS)
#define BZ_PHY_PHY_TESTBUS_READ_UMSK                            (~(((1U<<BZ_PHY_PHY_TESTBUS_READ_LEN)-1)<<BZ_PHY_PHY_TESTBUS_READ_POS))

/* 0x85C : phy_testbus_1 */
#define BZ_PHY_PHY_TESTBUS_1_OFFSET                             (0x85C)
#define BZ_PHY_PHY_TESTBUS_SEL                                  BZ_PHY_PHY_TESTBUS_SEL
#define BZ_PHY_PHY_TESTBUS_SEL_POS                              (0U)
#define BZ_PHY_PHY_TESTBUS_SEL_LEN                              (4U)
#define BZ_PHY_PHY_TESTBUS_SEL_MSK                              (((1U<<BZ_PHY_PHY_TESTBUS_SEL_LEN)-1)<<BZ_PHY_PHY_TESTBUS_SEL_POS)
#define BZ_PHY_PHY_TESTBUS_SEL_UMSK                             (~(((1U<<BZ_PHY_PHY_TESTBUS_SEL_LEN)-1)<<BZ_PHY_PHY_TESTBUS_SEL_POS))

/* 0x860 : phy_status_0 */
#define BZ_PHY_PHY_STATUS_0_OFFSET                              (0x860)
#define BZ_PHY_STS_AGC_FE_DG                                    BZ_PHY_STS_AGC_FE_DG
#define BZ_PHY_STS_AGC_FE_DG_POS                                (0U)
#define BZ_PHY_STS_AGC_FE_DG_LEN                                (6U)
#define BZ_PHY_STS_AGC_FE_DG_MSK                                (((1U<<BZ_PHY_STS_AGC_FE_DG_LEN)-1)<<BZ_PHY_STS_AGC_FE_DG_POS)
#define BZ_PHY_STS_AGC_FE_DG_UMSK                               (~(((1U<<BZ_PHY_STS_AGC_FE_DG_LEN)-1)<<BZ_PHY_STS_AGC_FE_DG_POS))
#define BZ_PHY_STS_AGC_AGC_RSSI                                 BZ_PHY_STS_AGC_AGC_RSSI
#define BZ_PHY_STS_AGC_AGC_RSSI_POS                             (8U)
#define BZ_PHY_STS_AGC_AGC_RSSI_LEN                             (8U)
#define BZ_PHY_STS_AGC_AGC_RSSI_MSK                             (((1U<<BZ_PHY_STS_AGC_AGC_RSSI_LEN)-1)<<BZ_PHY_STS_AGC_AGC_RSSI_POS)
#define BZ_PHY_STS_AGC_AGC_RSSI_UMSK                            (~(((1U<<BZ_PHY_STS_AGC_AGC_RSSI_LEN)-1)<<BZ_PHY_STS_AGC_AGC_RSSI_POS))
#define BZ_PHY_STS_AGC_RF_RBB_IND                               BZ_PHY_STS_AGC_RF_RBB_IND
#define BZ_PHY_STS_AGC_RF_RBB_IND_POS                           (16U)
#define BZ_PHY_STS_AGC_RF_RBB_IND_LEN                           (5U)
#define BZ_PHY_STS_AGC_RF_RBB_IND_MSK                           (((1U<<BZ_PHY_STS_AGC_RF_RBB_IND_LEN)-1)<<BZ_PHY_STS_AGC_RF_RBB_IND_POS)
#define BZ_PHY_STS_AGC_RF_RBB_IND_UMSK                          (~(((1U<<BZ_PHY_STS_AGC_RF_RBB_IND_LEN)-1)<<BZ_PHY_STS_AGC_RF_RBB_IND_POS))
#define BZ_PHY_STS_AGC_RF_LNA                                   BZ_PHY_STS_AGC_RF_LNA
#define BZ_PHY_STS_AGC_RF_LNA_POS                               (21U)
#define BZ_PHY_STS_AGC_RF_LNA_LEN                               (3U)
#define BZ_PHY_STS_AGC_RF_LNA_MSK                               (((1U<<BZ_PHY_STS_AGC_RF_LNA_LEN)-1)<<BZ_PHY_STS_AGC_RF_LNA_POS)
#define BZ_PHY_STS_AGC_RF_LNA_UMSK                              (~(((1U<<BZ_PHY_STS_AGC_RF_LNA_LEN)-1)<<BZ_PHY_STS_AGC_RF_LNA_POS))
#define BZ_PHY_STS_AGC_CLEAR                                    BZ_PHY_STS_AGC_CLEAR
#define BZ_PHY_STS_AGC_CLEAR_POS                                (31U)
#define BZ_PHY_STS_AGC_CLEAR_LEN                                (1U)
#define BZ_PHY_STS_AGC_CLEAR_MSK                                (((1U<<BZ_PHY_STS_AGC_CLEAR_LEN)-1)<<BZ_PHY_STS_AGC_CLEAR_POS)
#define BZ_PHY_STS_AGC_CLEAR_UMSK                               (~(((1U<<BZ_PHY_STS_AGC_CLEAR_LEN)-1)<<BZ_PHY_STS_AGC_CLEAR_POS))

/* 0x864 : phy_status_1 */
#define BZ_PHY_PHY_STATUS_1_OFFSET                              (0x864)
#define BZ_PHY_STS_ZB_SFD_CORR                                  BZ_PHY_STS_ZB_SFD_CORR
#define BZ_PHY_STS_ZB_SFD_CORR_POS                              (0U)
#define BZ_PHY_STS_ZB_SFD_CORR_LEN                              (15U)
#define BZ_PHY_STS_ZB_SFD_CORR_MSK                              (((1U<<BZ_PHY_STS_ZB_SFD_CORR_LEN)-1)<<BZ_PHY_STS_ZB_SFD_CORR_POS)
#define BZ_PHY_STS_ZB_SFD_CORR_UMSK                             (~(((1U<<BZ_PHY_STS_ZB_SFD_CORR_LEN)-1)<<BZ_PHY_STS_ZB_SFD_CORR_POS))
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT                              BZ_PHY_STS_ZB_FREQ_TRK_OUT
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS                          (16U)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN                          (13U)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_MSK                          (((1U<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS)
#define BZ_PHY_STS_ZB_FREQ_TRK_OUT_UMSK                         (~(((1U<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_ZB_FREQ_TRK_OUT_POS))
#define BZ_PHY_STS_ZB_SFD_MODE                                  BZ_PHY_STS_ZB_SFD_MODE
#define BZ_PHY_STS_ZB_SFD_MODE_POS                              (29U)
#define BZ_PHY_STS_ZB_SFD_MODE_LEN                              (2U)
#define BZ_PHY_STS_ZB_SFD_MODE_MSK                              (((1U<<BZ_PHY_STS_ZB_SFD_MODE_LEN)-1)<<BZ_PHY_STS_ZB_SFD_MODE_POS)
#define BZ_PHY_STS_ZB_SFD_MODE_UMSK                             (~(((1U<<BZ_PHY_STS_ZB_SFD_MODE_LEN)-1)<<BZ_PHY_STS_ZB_SFD_MODE_POS))
#define BZ_PHY_STS_ZB_CLEAR                                     BZ_PHY_STS_ZB_CLEAR
#define BZ_PHY_STS_ZB_CLEAR_POS                                 (31U)
#define BZ_PHY_STS_ZB_CLEAR_LEN                                 (1U)
#define BZ_PHY_STS_ZB_CLEAR_MSK                                 (((1U<<BZ_PHY_STS_ZB_CLEAR_LEN)-1)<<BZ_PHY_STS_ZB_CLEAR_POS)
#define BZ_PHY_STS_ZB_CLEAR_UMSK                                (~(((1U<<BZ_PHY_STS_ZB_CLEAR_LEN)-1)<<BZ_PHY_STS_ZB_CLEAR_POS))

/* 0x868 : phy_status_2 */
#define BZ_PHY_PHY_STATUS_2_OFFSET                              (0x868)
#define BZ_PHY_STS_ZB_PEAK_VAL_2                                BZ_PHY_STS_ZB_PEAK_VAL_2
#define BZ_PHY_STS_ZB_PEAK_VAL_2_POS                            (0U)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_LEN                            (14U)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_MSK                            (((1U<<BZ_PHY_STS_ZB_PEAK_VAL_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_2_POS)
#define BZ_PHY_STS_ZB_PEAK_VAL_2_UMSK                           (~(((1U<<BZ_PHY_STS_ZB_PEAK_VAL_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_2_POS))
#define BZ_PHY_STS_ZB_PEAK_VAL_1                                BZ_PHY_STS_ZB_PEAK_VAL_1
#define BZ_PHY_STS_ZB_PEAK_VAL_1_POS                            (16U)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_LEN                            (14U)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_MSK                            (((1U<<BZ_PHY_STS_ZB_PEAK_VAL_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_1_POS)
#define BZ_PHY_STS_ZB_PEAK_VAL_1_UMSK                           (~(((1U<<BZ_PHY_STS_ZB_PEAK_VAL_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_VAL_1_POS))

/* 0x86C : phy_status_3 */
#define BZ_PHY_PHY_STATUS_3_OFFSET                              (0x86C)
#define BZ_PHY_STS_ZB_PEAK_LOC_2                                BZ_PHY_STS_ZB_PEAK_LOC_2
#define BZ_PHY_STS_ZB_PEAK_LOC_2_POS                            (0U)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_LEN                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_MSK                            (((1U<<BZ_PHY_STS_ZB_PEAK_LOC_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_2_POS)
#define BZ_PHY_STS_ZB_PEAK_LOC_2_UMSK                           (~(((1U<<BZ_PHY_STS_ZB_PEAK_LOC_2_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_2_POS))
#define BZ_PHY_STS_ZB_PEAK_LOC_1                                BZ_PHY_STS_ZB_PEAK_LOC_1
#define BZ_PHY_STS_ZB_PEAK_LOC_1_POS                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_LEN                            (8U)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_MSK                            (((1U<<BZ_PHY_STS_ZB_PEAK_LOC_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_1_POS)
#define BZ_PHY_STS_ZB_PEAK_LOC_1_UMSK                           (~(((1U<<BZ_PHY_STS_ZB_PEAK_LOC_1_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_LOC_1_POS))

/* 0x870 : phy_status_4 */
#define BZ_PHY_PHY_STATUS_4_OFFSET                              (0x870)
#define BZ_PHY_STS_ZB_PEAK_IM                                   BZ_PHY_STS_ZB_PEAK_IM
#define BZ_PHY_STS_ZB_PEAK_IM_POS                               (0U)
#define BZ_PHY_STS_ZB_PEAK_IM_LEN                               (12U)
#define BZ_PHY_STS_ZB_PEAK_IM_MSK                               (((1U<<BZ_PHY_STS_ZB_PEAK_IM_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_IM_POS)
#define BZ_PHY_STS_ZB_PEAK_IM_UMSK                              (~(((1U<<BZ_PHY_STS_ZB_PEAK_IM_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_IM_POS))
#define BZ_PHY_STS_ZB_PEAK_RE                                   BZ_PHY_STS_ZB_PEAK_RE
#define BZ_PHY_STS_ZB_PEAK_RE_POS                               (16U)
#define BZ_PHY_STS_ZB_PEAK_RE_LEN                               (12U)
#define BZ_PHY_STS_ZB_PEAK_RE_MSK                               (((1U<<BZ_PHY_STS_ZB_PEAK_RE_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_RE_POS)
#define BZ_PHY_STS_ZB_PEAK_RE_UMSK                              (~(((1U<<BZ_PHY_STS_ZB_PEAK_RE_LEN)-1)<<BZ_PHY_STS_ZB_PEAK_RE_POS))

/* 0x874 : phy_status_5 */
#define BZ_PHY_PHY_STATUS_5_OFFSET                              (0x874)
#define BZ_PHY_STS_ZB_CURR_LOC                                  BZ_PHY_STS_ZB_CURR_LOC
#define BZ_PHY_STS_ZB_CURR_LOC_POS                              (0U)
#define BZ_PHY_STS_ZB_CURR_LOC_LEN                              (8U)
#define BZ_PHY_STS_ZB_CURR_LOC_MSK                              (((1U<<BZ_PHY_STS_ZB_CURR_LOC_LEN)-1)<<BZ_PHY_STS_ZB_CURR_LOC_POS)
#define BZ_PHY_STS_ZB_CURR_LOC_UMSK                             (~(((1U<<BZ_PHY_STS_ZB_CURR_LOC_LEN)-1)<<BZ_PHY_STS_ZB_CURR_LOC_POS))
#define BZ_PHY_STS_ZB_CURR_MAX                                  BZ_PHY_STS_ZB_CURR_MAX
#define BZ_PHY_STS_ZB_CURR_MAX_POS                              (16U)
#define BZ_PHY_STS_ZB_CURR_MAX_LEN                              (14U)
#define BZ_PHY_STS_ZB_CURR_MAX_MSK                              (((1U<<BZ_PHY_STS_ZB_CURR_MAX_LEN)-1)<<BZ_PHY_STS_ZB_CURR_MAX_POS)
#define BZ_PHY_STS_ZB_CURR_MAX_UMSK                             (~(((1U<<BZ_PHY_STS_ZB_CURR_MAX_LEN)-1)<<BZ_PHY_STS_ZB_CURR_MAX_POS))

/* 0x878 : phy_status_6 */
#define BZ_PHY_PHY_STATUS_6_OFFSET                              (0x878)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT                         BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS                     (0U)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN                     (13U)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_MSK                     (((1U<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS)
#define BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_UMSK                    (~(((1U<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_LEN)-1)<<BZ_PHY_STS_DONE_ZB_FREQ_TRK_OUT_POS))

/* 0x87C : phy_status_7 */
#define BZ_PHY_PHY_STATUS_7_OFFSET                              (0x87C)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK                             BZ_PHY_STS_BLE_CFO_OUT_PEAK
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS                         (0U)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN                         (8U)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_MSK                         (((1U<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS)
#define BZ_PHY_STS_BLE_CFO_OUT_PEAK_UMSK                        (~(((1U<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_CFO_OUT_PEAK_POS))
#define BZ_PHY_STS_BLE_FRTK_OUT                                 BZ_PHY_STS_BLE_FRTK_OUT
#define BZ_PHY_STS_BLE_FRTK_OUT_POS                             (8U)
#define BZ_PHY_STS_BLE_FRTK_OUT_LEN                             (13U)
#define BZ_PHY_STS_BLE_FRTK_OUT_MSK                             (((1U<<BZ_PHY_STS_BLE_FRTK_OUT_LEN)-1)<<BZ_PHY_STS_BLE_FRTK_OUT_POS)
#define BZ_PHY_STS_BLE_FRTK_OUT_UMSK                            (~(((1U<<BZ_PHY_STS_BLE_FRTK_OUT_LEN)-1)<<BZ_PHY_STS_BLE_FRTK_OUT_POS))
#define BZ_PHY_STS_BLE_CLEAR                                    BZ_PHY_STS_BLE_CLEAR
#define BZ_PHY_STS_BLE_CLEAR_POS                                (31U)
#define BZ_PHY_STS_BLE_CLEAR_LEN                                (1U)
#define BZ_PHY_STS_BLE_CLEAR_MSK                                (((1U<<BZ_PHY_STS_BLE_CLEAR_LEN)-1)<<BZ_PHY_STS_BLE_CLEAR_POS)
#define BZ_PHY_STS_BLE_CLEAR_UMSK                               (~(((1U<<BZ_PHY_STS_BLE_CLEAR_LEN)-1)<<BZ_PHY_STS_BLE_CLEAR_POS))

/* 0x880 : phy_status_8 */
#define BZ_PHY_PHY_STATUS_8_OFFSET                              (0x880)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS                          BZ_PHY_STS_BLE_AA_CONFIRM_BITS
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS                      (0U)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN                      (32U)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_MSK                      (((1U<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN)-1)<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS)
#define BZ_PHY_STS_BLE_AA_CONFIRM_BITS_UMSK                     (~(((1U<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_LEN)-1)<<BZ_PHY_STS_BLE_AA_CONFIRM_BITS_POS))

/* 0x884 : phy_status_9 */
#define BZ_PHY_PHY_STATUS_9_OFFSET                              (0x884)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK                        BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS                    (0U)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN                    (8U)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_MSK                    (((1U<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS)
#define BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_UMSK                   (~(((1U<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_LEN)-1)<<BZ_PHY_STS_BLE_DONE_CFO_OUT_PEAK_POS))
#define BZ_PHY_STS_BLE_CORR_OUT_MAX                             BZ_PHY_STS_BLE_CORR_OUT_MAX
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_POS                         (8U)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN                         (12U)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_MSK                         (((1U<<BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN)-1)<<BZ_PHY_STS_BLE_CORR_OUT_MAX_POS)
#define BZ_PHY_STS_BLE_CORR_OUT_MAX_UMSK                        (~(((1U<<BZ_PHY_STS_BLE_CORR_OUT_MAX_LEN)-1)<<BZ_PHY_STS_BLE_CORR_OUT_MAX_POS))
#define BZ_PHY_STS_BLE_AA_ERROR_CNT                             BZ_PHY_STS_BLE_AA_ERROR_CNT
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_POS                         (20U)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN                         (6U)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_MSK                         (((1U<<BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN)-1)<<BZ_PHY_STS_BLE_AA_ERROR_CNT_POS)
#define BZ_PHY_STS_BLE_AA_ERROR_CNT_UMSK                        (~(((1U<<BZ_PHY_STS_BLE_AA_ERROR_CNT_LEN)-1)<<BZ_PHY_STS_BLE_AA_ERROR_CNT_POS))

/* 0x888 : phy_status_10 */
#define BZ_PHY_PHY_STATUS_10_OFFSET                             (0x888)
#define BZ_PHY_ZB_FSM_ERR_STS                                   BZ_PHY_ZB_FSM_ERR_STS
#define BZ_PHY_ZB_FSM_ERR_STS_POS                               (0U)
#define BZ_PHY_ZB_FSM_ERR_STS_LEN                               (8U)
#define BZ_PHY_ZB_FSM_ERR_STS_MSK                               (((1U<<BZ_PHY_ZB_FSM_ERR_STS_LEN)-1)<<BZ_PHY_ZB_FSM_ERR_STS_POS)
#define BZ_PHY_ZB_FSM_ERR_STS_UMSK                              (~(((1U<<BZ_PHY_ZB_FSM_ERR_STS_LEN)-1)<<BZ_PHY_ZB_FSM_ERR_STS_POS))
#define BZ_PHY_ZB_FSM_DBG_EN                                    BZ_PHY_ZB_FSM_DBG_EN
#define BZ_PHY_ZB_FSM_DBG_EN_POS                                (8U)
#define BZ_PHY_ZB_FSM_DBG_EN_LEN                                (1U)
#define BZ_PHY_ZB_FSM_DBG_EN_MSK                                (((1U<<BZ_PHY_ZB_FSM_DBG_EN_LEN)-1)<<BZ_PHY_ZB_FSM_DBG_EN_POS)
#define BZ_PHY_ZB_FSM_DBG_EN_UMSK                               (~(((1U<<BZ_PHY_ZB_FSM_DBG_EN_LEN)-1)<<BZ_PHY_ZB_FSM_DBG_EN_POS))
#define BZ_PHY_ZB_TS_CS_FOUND                                   BZ_PHY_ZB_TS_CS_FOUND
#define BZ_PHY_ZB_TS_CS_FOUND_POS                               (9U)
#define BZ_PHY_ZB_TS_CS_FOUND_LEN                               (1U)
#define BZ_PHY_ZB_TS_CS_FOUND_MSK                               (((1U<<BZ_PHY_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_ZB_TS_CS_FOUND_POS)
#define BZ_PHY_ZB_TS_CS_FOUND_UMSK                              (~(((1U<<BZ_PHY_ZB_TS_CS_FOUND_LEN)-1)<<BZ_PHY_ZB_TS_CS_FOUND_POS))
#define BZ_PHY_ZB_DM_SFD_FOUND                                  BZ_PHY_ZB_DM_SFD_FOUND
#define BZ_PHY_ZB_DM_SFD_FOUND_POS                              (10U)
#define BZ_PHY_ZB_DM_SFD_FOUND_LEN                              (1U)
#define BZ_PHY_ZB_DM_SFD_FOUND_MSK                              (((1U<<BZ_PHY_ZB_DM_SFD_FOUND_LEN)-1)<<BZ_PHY_ZB_DM_SFD_FOUND_POS)
#define BZ_PHY_ZB_DM_SFD_FOUND_UMSK                             (~(((1U<<BZ_PHY_ZB_DM_SFD_FOUND_LEN)-1)<<BZ_PHY_ZB_DM_SFD_FOUND_POS))
#define BZ_PHY_ZB_DM_PHY_LEN                                    BZ_PHY_ZB_DM_PHY_LEN
#define BZ_PHY_ZB_DM_PHY_LEN_POS                                (16U)
#define BZ_PHY_ZB_DM_PHY_LEN_LEN                                (8U)
#define BZ_PHY_ZB_DM_PHY_LEN_MSK                                (((1U<<BZ_PHY_ZB_DM_PHY_LEN_LEN)-1)<<BZ_PHY_ZB_DM_PHY_LEN_POS)
#define BZ_PHY_ZB_DM_PHY_LEN_UMSK                               (~(((1U<<BZ_PHY_ZB_DM_PHY_LEN_LEN)-1)<<BZ_PHY_ZB_DM_PHY_LEN_POS))
#define BZ_PHY_ZB_DM_WHITE_INIT                                 BZ_PHY_ZB_DM_WHITE_INIT
#define BZ_PHY_ZB_DM_WHITE_INIT_POS                             (24U)
#define BZ_PHY_ZB_DM_WHITE_INIT_LEN                             (7U)
#define BZ_PHY_ZB_DM_WHITE_INIT_MSK                             (((1U<<BZ_PHY_ZB_DM_WHITE_INIT_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_INIT_POS)
#define BZ_PHY_ZB_DM_WHITE_INIT_UMSK                            (~(((1U<<BZ_PHY_ZB_DM_WHITE_INIT_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_INIT_POS))
#define BZ_PHY_ZB_DM_WHITE_EN                                   BZ_PHY_ZB_DM_WHITE_EN
#define BZ_PHY_ZB_DM_WHITE_EN_POS                               (31U)
#define BZ_PHY_ZB_DM_WHITE_EN_LEN                               (1U)
#define BZ_PHY_ZB_DM_WHITE_EN_MSK                               (((1U<<BZ_PHY_ZB_DM_WHITE_EN_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_EN_POS)
#define BZ_PHY_ZB_DM_WHITE_EN_UMSK                              (~(((1U<<BZ_PHY_ZB_DM_WHITE_EN_LEN)-1)<<BZ_PHY_ZB_DM_WHITE_EN_POS))

/* 0x88C : phy_ble_coded_0 */
#define BZ_PHY_PHY_BLE_CODED_0_OFFSET                           (0x88C)
#define BZ_PHY_BLEC_AA_SEARCH_START                             BZ_PHY_BLEC_AA_SEARCH_START
#define BZ_PHY_BLEC_AA_SEARCH_START_POS                         (0U)
#define BZ_PHY_BLEC_AA_SEARCH_START_LEN                         (5U)
#define BZ_PHY_BLEC_AA_SEARCH_START_MSK                         (((1U<<BZ_PHY_BLEC_AA_SEARCH_START_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_START_POS)
#define BZ_PHY_BLEC_AA_SEARCH_START_UMSK                        (~(((1U<<BZ_PHY_BLEC_AA_SEARCH_START_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_START_POS))
#define BZ_PHY_BLEC_PK_JUMP_THRES                               BZ_PHY_BLEC_PK_JUMP_THRES
#define BZ_PHY_BLEC_PK_JUMP_THRES_POS                           (5U)
#define BZ_PHY_BLEC_PK_JUMP_THRES_LEN                           (6U)
#define BZ_PHY_BLEC_PK_JUMP_THRES_MSK                           (((1U<<BZ_PHY_BLEC_PK_JUMP_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_JUMP_THRES_POS)
#define BZ_PHY_BLEC_PK_JUMP_THRES_UMSK                          (~(((1U<<BZ_PHY_BLEC_PK_JUMP_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_JUMP_THRES_POS))
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES                        BZ_PHY_BLEC_PK_TIMING_DIFF_THRES
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS                    (11U)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN                    (3U)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_MSK                    (((1U<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS)
#define BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_UMSK                   (~(((1U<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_TIMING_DIFF_THRES_POS))
#define BZ_PHY_BLEC_PK_THRES                                    BZ_PHY_BLEC_PK_THRES
#define BZ_PHY_BLEC_PK_THRES_POS                                (14U)
#define BZ_PHY_BLEC_PK_THRES_LEN                                (9U)
#define BZ_PHY_BLEC_PK_THRES_MSK                                (((1U<<BZ_PHY_BLEC_PK_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_THRES_POS)
#define BZ_PHY_BLEC_PK_THRES_UMSK                               (~(((1U<<BZ_PHY_BLEC_PK_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_THRES_POS))
#define BZ_PHY_BLEC_PK_START_THRES                              BZ_PHY_BLEC_PK_START_THRES
#define BZ_PHY_BLEC_PK_START_THRES_POS                          (23U)
#define BZ_PHY_BLEC_PK_START_THRES_LEN                          (9U)
#define BZ_PHY_BLEC_PK_START_THRES_MSK                          (((1U<<BZ_PHY_BLEC_PK_START_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_START_THRES_POS)
#define BZ_PHY_BLEC_PK_START_THRES_UMSK                         (~(((1U<<BZ_PHY_BLEC_PK_START_THRES_LEN)-1)<<BZ_PHY_BLEC_PK_START_THRES_POS))

/* 0x890 : phy_ble_coded_1 */
#define BZ_PHY_PHY_BLE_CODED_1_OFFSET                           (0x890)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD                             BZ_PHY_BLE_FSM_RX_DMTIME_CD
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS                         (0U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN                         (8U)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_MSK                         (((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS)
#define BZ_PHY_BLE_FSM_RX_DMTIME_CD_UMSK                        (~(((1U<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_LEN)-1)<<BZ_PHY_BLE_FSM_RX_DMTIME_CD_POS))
#define BZ_PHY_BLEC_RST_THRES                                   BZ_PHY_BLEC_RST_THRES
#define BZ_PHY_BLEC_RST_THRES_POS                               (8U)
#define BZ_PHY_BLEC_RST_THRES_LEN                               (4U)
#define BZ_PHY_BLEC_RST_THRES_MSK                               (((1U<<BZ_PHY_BLEC_RST_THRES_LEN)-1)<<BZ_PHY_BLEC_RST_THRES_POS)
#define BZ_PHY_BLEC_RST_THRES_UMSK                              (~(((1U<<BZ_PHY_BLEC_RST_THRES_LEN)-1)<<BZ_PHY_BLEC_RST_THRES_POS))
#define BZ_PHY_BLEC_TIMING_ADJ                                  BZ_PHY_BLEC_TIMING_ADJ
#define BZ_PHY_BLEC_TIMING_ADJ_POS                              (12U)
#define BZ_PHY_BLEC_TIMING_ADJ_LEN                              (5U)
#define BZ_PHY_BLEC_TIMING_ADJ_MSK                              (((1U<<BZ_PHY_BLEC_TIMING_ADJ_LEN)-1)<<BZ_PHY_BLEC_TIMING_ADJ_POS)
#define BZ_PHY_BLEC_TIMING_ADJ_UMSK                             (~(((1U<<BZ_PHY_BLEC_TIMING_ADJ_LEN)-1)<<BZ_PHY_BLEC_TIMING_ADJ_POS))
#define BZ_PHY_BLEC_AA_CORR_THRES                               BZ_PHY_BLEC_AA_CORR_THRES
#define BZ_PHY_BLEC_AA_CORR_THRES_POS                           (17U)
#define BZ_PHY_BLEC_AA_CORR_THRES_LEN                           (8U)
#define BZ_PHY_BLEC_AA_CORR_THRES_MSK                           (((1U<<BZ_PHY_BLEC_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLEC_AA_CORR_THRES_POS)
#define BZ_PHY_BLEC_AA_CORR_THRES_UMSK                          (~(((1U<<BZ_PHY_BLEC_AA_CORR_THRES_LEN)-1)<<BZ_PHY_BLEC_AA_CORR_THRES_POS))
#define BZ_PHY_BLEC_AA_SEARCH_STOP                              BZ_PHY_BLEC_AA_SEARCH_STOP
#define BZ_PHY_BLEC_AA_SEARCH_STOP_POS                          (25U)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_LEN                          (7U)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_MSK                          (((1U<<BZ_PHY_BLEC_AA_SEARCH_STOP_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_STOP_POS)
#define BZ_PHY_BLEC_AA_SEARCH_STOP_UMSK                         (~(((1U<<BZ_PHY_BLEC_AA_SEARCH_STOP_LEN)-1)<<BZ_PHY_BLEC_AA_SEARCH_STOP_POS))

/* 0x894 : phy_ble_frtk_0 */
#define BZ_PHY_PHY_BLE_FRTK_0_OFFSET                            (0x894)
#define BZ_PHY_BLE_FRTK_MAX_STEP                                BZ_PHY_BLE_FRTK_MAX_STEP
#define BZ_PHY_BLE_FRTK_MAX_STEP_POS                            (0U)
#define BZ_PHY_BLE_FRTK_MAX_STEP_LEN                            (6U)
#define BZ_PHY_BLE_FRTK_MAX_STEP_MSK                            (((1U<<BZ_PHY_BLE_FRTK_MAX_STEP_LEN)-1)<<BZ_PHY_BLE_FRTK_MAX_STEP_POS)
#define BZ_PHY_BLE_FRTK_MAX_STEP_UMSK                           (~(((1U<<BZ_PHY_BLE_FRTK_MAX_STEP_LEN)-1)<<BZ_PHY_BLE_FRTK_MAX_STEP_POS))
#define BZ_PHY_BLE_FRTK_TH3                                     BZ_PHY_BLE_FRTK_TH3
#define BZ_PHY_BLE_FRTK_TH3_POS                                 (8U)
#define BZ_PHY_BLE_FRTK_TH3_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH3_MSK                                 (((1U<<BZ_PHY_BLE_FRTK_TH3_LEN)-1)<<BZ_PHY_BLE_FRTK_TH3_POS)
#define BZ_PHY_BLE_FRTK_TH3_UMSK                                (~(((1U<<BZ_PHY_BLE_FRTK_TH3_LEN)-1)<<BZ_PHY_BLE_FRTK_TH3_POS))
#define BZ_PHY_BLE_FRTK_TH2                                     BZ_PHY_BLE_FRTK_TH2
#define BZ_PHY_BLE_FRTK_TH2_POS                                 (16U)
#define BZ_PHY_BLE_FRTK_TH2_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH2_MSK                                 (((1U<<BZ_PHY_BLE_FRTK_TH2_LEN)-1)<<BZ_PHY_BLE_FRTK_TH2_POS)
#define BZ_PHY_BLE_FRTK_TH2_UMSK                                (~(((1U<<BZ_PHY_BLE_FRTK_TH2_LEN)-1)<<BZ_PHY_BLE_FRTK_TH2_POS))
#define BZ_PHY_BLE_FRTK_TH1                                     BZ_PHY_BLE_FRTK_TH1
#define BZ_PHY_BLE_FRTK_TH1_POS                                 (24U)
#define BZ_PHY_BLE_FRTK_TH1_LEN                                 (7U)
#define BZ_PHY_BLE_FRTK_TH1_MSK                                 (((1U<<BZ_PHY_BLE_FRTK_TH1_LEN)-1)<<BZ_PHY_BLE_FRTK_TH1_POS)
#define BZ_PHY_BLE_FRTK_TH1_UMSK                                (~(((1U<<BZ_PHY_BLE_FRTK_TH1_LEN)-1)<<BZ_PHY_BLE_FRTK_TH1_POS))

/* 0x898 : phy_ble_frtk_1 */
#define BZ_PHY_PHY_BLE_FRTK_1_OFFSET                            (0x898)
#define BZ_PHY_BLE_FRTK_EN                                      BZ_PHY_BLE_FRTK_EN
#define BZ_PHY_BLE_FRTK_EN_POS                                  (0U)
#define BZ_PHY_BLE_FRTK_EN_LEN                                  (1U)
#define BZ_PHY_BLE_FRTK_EN_MSK                                  (((1U<<BZ_PHY_BLE_FRTK_EN_LEN)-1)<<BZ_PHY_BLE_FRTK_EN_POS)
#define BZ_PHY_BLE_FRTK_EN_UMSK                                 (~(((1U<<BZ_PHY_BLE_FRTK_EN_LEN)-1)<<BZ_PHY_BLE_FRTK_EN_POS))
#define BZ_PHY_BLE_FRTK_COEFF                                   BZ_PHY_BLE_FRTK_COEFF
#define BZ_PHY_BLE_FRTK_COEFF_POS                               (1U)
#define BZ_PHY_BLE_FRTK_COEFF_LEN                               (9U)
#define BZ_PHY_BLE_FRTK_COEFF_MSK                               (((1U<<BZ_PHY_BLE_FRTK_COEFF_LEN)-1)<<BZ_PHY_BLE_FRTK_COEFF_POS)
#define BZ_PHY_BLE_FRTK_COEFF_UMSK                              (~(((1U<<BZ_PHY_BLE_FRTK_COEFF_LEN)-1)<<BZ_PHY_BLE_FRTK_COEFF_POS))

/* 0x900 : notch_config_2402 */
#define BZ_PHY_NOTCH_CONFIG_2402_OFFSET                         (0x900)
#define BZ_PHY_NOTCH_COEFF_IM_2402                              BZ_PHY_NOTCH_COEFF_IM_2402
#define BZ_PHY_NOTCH_COEFF_IM_2402_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2402_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2402_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2402_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2402_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2402_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2402_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2402_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2402                              BZ_PHY_NOTCH_COEFF_RE_2402
#define BZ_PHY_NOTCH_COEFF_RE_2402_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2402_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2402_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2402_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2402_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2402_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2402_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2402_POS))
#define BZ_PHY_NOTCH_ALPHA_2402                                 BZ_PHY_NOTCH_ALPHA_2402
#define BZ_PHY_NOTCH_ALPHA_2402_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2402_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2402_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2402_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2402_POS)
#define BZ_PHY_NOTCH_ALPHA_2402_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2402_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2402_POS))
#define BZ_PHY_NOTCH_EN_2402                                    BZ_PHY_NOTCH_EN_2402
#define BZ_PHY_NOTCH_EN_2402_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2402_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2402_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2402_LEN)-1)<<BZ_PHY_NOTCH_EN_2402_POS)
#define BZ_PHY_NOTCH_EN_2402_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2402_LEN)-1)<<BZ_PHY_NOTCH_EN_2402_POS))

/* 0x904 : notch_config_2404 */
#define BZ_PHY_NOTCH_CONFIG_2404_OFFSET                         (0x904)
#define BZ_PHY_NOTCH_COEFF_IM_2404                              BZ_PHY_NOTCH_COEFF_IM_2404
#define BZ_PHY_NOTCH_COEFF_IM_2404_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2404_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2404_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2404_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2404_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2404_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2404_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2404_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2404                              BZ_PHY_NOTCH_COEFF_RE_2404
#define BZ_PHY_NOTCH_COEFF_RE_2404_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2404_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2404_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2404_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2404_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2404_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2404_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2404_POS))
#define BZ_PHY_NOTCH_ALPHA_2404                                 BZ_PHY_NOTCH_ALPHA_2404
#define BZ_PHY_NOTCH_ALPHA_2404_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2404_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2404_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2404_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2404_POS)
#define BZ_PHY_NOTCH_ALPHA_2404_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2404_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2404_POS))
#define BZ_PHY_NOTCH_EN_2404                                    BZ_PHY_NOTCH_EN_2404
#define BZ_PHY_NOTCH_EN_2404_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2404_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2404_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2404_LEN)-1)<<BZ_PHY_NOTCH_EN_2404_POS)
#define BZ_PHY_NOTCH_EN_2404_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2404_LEN)-1)<<BZ_PHY_NOTCH_EN_2404_POS))

/* 0x908 : notch_config_2406 */
#define BZ_PHY_NOTCH_CONFIG_2406_OFFSET                         (0x908)
#define BZ_PHY_NOTCH_COEFF_IM_2406                              BZ_PHY_NOTCH_COEFF_IM_2406
#define BZ_PHY_NOTCH_COEFF_IM_2406_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2406_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2406_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2406_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2406_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2406_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2406_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2406_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2406                              BZ_PHY_NOTCH_COEFF_RE_2406
#define BZ_PHY_NOTCH_COEFF_RE_2406_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2406_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2406_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2406_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2406_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2406_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2406_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2406_POS))
#define BZ_PHY_NOTCH_ALPHA_2406                                 BZ_PHY_NOTCH_ALPHA_2406
#define BZ_PHY_NOTCH_ALPHA_2406_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2406_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2406_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2406_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2406_POS)
#define BZ_PHY_NOTCH_ALPHA_2406_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2406_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2406_POS))
#define BZ_PHY_NOTCH_EN_2406                                    BZ_PHY_NOTCH_EN_2406
#define BZ_PHY_NOTCH_EN_2406_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2406_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2406_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2406_LEN)-1)<<BZ_PHY_NOTCH_EN_2406_POS)
#define BZ_PHY_NOTCH_EN_2406_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2406_LEN)-1)<<BZ_PHY_NOTCH_EN_2406_POS))

/* 0x90C : notch_config_2408 */
#define BZ_PHY_NOTCH_CONFIG_2408_OFFSET                         (0x90C)
#define BZ_PHY_NOTCH_COEFF_IM_2408                              BZ_PHY_NOTCH_COEFF_IM_2408
#define BZ_PHY_NOTCH_COEFF_IM_2408_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2408_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2408_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2408_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2408_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2408_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2408_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2408_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2408                              BZ_PHY_NOTCH_COEFF_RE_2408
#define BZ_PHY_NOTCH_COEFF_RE_2408_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2408_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2408_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2408_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2408_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2408_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2408_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2408_POS))
#define BZ_PHY_NOTCH_ALPHA_2408                                 BZ_PHY_NOTCH_ALPHA_2408
#define BZ_PHY_NOTCH_ALPHA_2408_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2408_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2408_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2408_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2408_POS)
#define BZ_PHY_NOTCH_ALPHA_2408_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2408_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2408_POS))
#define BZ_PHY_NOTCH_EN_2408                                    BZ_PHY_NOTCH_EN_2408
#define BZ_PHY_NOTCH_EN_2408_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2408_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2408_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2408_LEN)-1)<<BZ_PHY_NOTCH_EN_2408_POS)
#define BZ_PHY_NOTCH_EN_2408_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2408_LEN)-1)<<BZ_PHY_NOTCH_EN_2408_POS))

/* 0x910 : notch_config_2410 */
#define BZ_PHY_NOTCH_CONFIG_2410_OFFSET                         (0x910)
#define BZ_PHY_NOTCH_COEFF_IM_2410                              BZ_PHY_NOTCH_COEFF_IM_2410
#define BZ_PHY_NOTCH_COEFF_IM_2410_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2410_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2410_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2410_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2410_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2410_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2410_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2410_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2410                              BZ_PHY_NOTCH_COEFF_RE_2410
#define BZ_PHY_NOTCH_COEFF_RE_2410_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2410_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2410_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2410_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2410_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2410_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2410_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2410_POS))
#define BZ_PHY_NOTCH_ALPHA_2410                                 BZ_PHY_NOTCH_ALPHA_2410
#define BZ_PHY_NOTCH_ALPHA_2410_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2410_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2410_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2410_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2410_POS)
#define BZ_PHY_NOTCH_ALPHA_2410_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2410_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2410_POS))
#define BZ_PHY_NOTCH_EN_2410                                    BZ_PHY_NOTCH_EN_2410
#define BZ_PHY_NOTCH_EN_2410_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2410_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2410_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2410_LEN)-1)<<BZ_PHY_NOTCH_EN_2410_POS)
#define BZ_PHY_NOTCH_EN_2410_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2410_LEN)-1)<<BZ_PHY_NOTCH_EN_2410_POS))

/* 0x914 : notch_config_2412 */
#define BZ_PHY_NOTCH_CONFIG_2412_OFFSET                         (0x914)
#define BZ_PHY_NOTCH_COEFF_IM_2412                              BZ_PHY_NOTCH_COEFF_IM_2412
#define BZ_PHY_NOTCH_COEFF_IM_2412_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2412_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2412_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2412_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2412_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2412_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2412_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2412_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2412                              BZ_PHY_NOTCH_COEFF_RE_2412
#define BZ_PHY_NOTCH_COEFF_RE_2412_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2412_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2412_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2412_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2412_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2412_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2412_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2412_POS))
#define BZ_PHY_NOTCH_ALPHA_2412                                 BZ_PHY_NOTCH_ALPHA_2412
#define BZ_PHY_NOTCH_ALPHA_2412_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2412_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2412_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2412_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2412_POS)
#define BZ_PHY_NOTCH_ALPHA_2412_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2412_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2412_POS))
#define BZ_PHY_NOTCH_EN_2412                                    BZ_PHY_NOTCH_EN_2412
#define BZ_PHY_NOTCH_EN_2412_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2412_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2412_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2412_LEN)-1)<<BZ_PHY_NOTCH_EN_2412_POS)
#define BZ_PHY_NOTCH_EN_2412_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2412_LEN)-1)<<BZ_PHY_NOTCH_EN_2412_POS))

/* 0x918 : notch_config_2414 */
#define BZ_PHY_NOTCH_CONFIG_2414_OFFSET                         (0x918)
#define BZ_PHY_NOTCH_COEFF_IM_2414                              BZ_PHY_NOTCH_COEFF_IM_2414
#define BZ_PHY_NOTCH_COEFF_IM_2414_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2414_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2414_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2414_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2414_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2414_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2414_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2414_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2414                              BZ_PHY_NOTCH_COEFF_RE_2414
#define BZ_PHY_NOTCH_COEFF_RE_2414_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2414_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2414_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2414_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2414_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2414_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2414_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2414_POS))
#define BZ_PHY_NOTCH_ALPHA_2414                                 BZ_PHY_NOTCH_ALPHA_2414
#define BZ_PHY_NOTCH_ALPHA_2414_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2414_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2414_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2414_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2414_POS)
#define BZ_PHY_NOTCH_ALPHA_2414_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2414_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2414_POS))
#define BZ_PHY_NOTCH_EN_2414                                    BZ_PHY_NOTCH_EN_2414
#define BZ_PHY_NOTCH_EN_2414_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2414_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2414_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2414_LEN)-1)<<BZ_PHY_NOTCH_EN_2414_POS)
#define BZ_PHY_NOTCH_EN_2414_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2414_LEN)-1)<<BZ_PHY_NOTCH_EN_2414_POS))

/* 0x91C : notch_config_2416 */
#define BZ_PHY_NOTCH_CONFIG_2416_OFFSET                         (0x91C)
#define BZ_PHY_NOTCH_COEFF_IM_2416                              BZ_PHY_NOTCH_COEFF_IM_2416
#define BZ_PHY_NOTCH_COEFF_IM_2416_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2416_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2416_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2416_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2416_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2416_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2416_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2416_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2416                              BZ_PHY_NOTCH_COEFF_RE_2416
#define BZ_PHY_NOTCH_COEFF_RE_2416_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2416_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2416_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2416_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2416_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2416_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2416_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2416_POS))
#define BZ_PHY_NOTCH_ALPHA_2416                                 BZ_PHY_NOTCH_ALPHA_2416
#define BZ_PHY_NOTCH_ALPHA_2416_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2416_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2416_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2416_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2416_POS)
#define BZ_PHY_NOTCH_ALPHA_2416_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2416_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2416_POS))
#define BZ_PHY_NOTCH_EN_2416                                    BZ_PHY_NOTCH_EN_2416
#define BZ_PHY_NOTCH_EN_2416_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2416_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2416_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2416_LEN)-1)<<BZ_PHY_NOTCH_EN_2416_POS)
#define BZ_PHY_NOTCH_EN_2416_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2416_LEN)-1)<<BZ_PHY_NOTCH_EN_2416_POS))

/* 0x920 : notch_config_2418 */
#define BZ_PHY_NOTCH_CONFIG_2418_OFFSET                         (0x920)
#define BZ_PHY_NOTCH_COEFF_IM_2418                              BZ_PHY_NOTCH_COEFF_IM_2418
#define BZ_PHY_NOTCH_COEFF_IM_2418_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2418_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2418_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2418_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2418_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2418_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2418_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2418_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2418                              BZ_PHY_NOTCH_COEFF_RE_2418
#define BZ_PHY_NOTCH_COEFF_RE_2418_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2418_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2418_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2418_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2418_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2418_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2418_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2418_POS))
#define BZ_PHY_NOTCH_ALPHA_2418                                 BZ_PHY_NOTCH_ALPHA_2418
#define BZ_PHY_NOTCH_ALPHA_2418_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2418_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2418_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2418_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2418_POS)
#define BZ_PHY_NOTCH_ALPHA_2418_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2418_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2418_POS))
#define BZ_PHY_NOTCH_EN_2418                                    BZ_PHY_NOTCH_EN_2418
#define BZ_PHY_NOTCH_EN_2418_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2418_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2418_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2418_LEN)-1)<<BZ_PHY_NOTCH_EN_2418_POS)
#define BZ_PHY_NOTCH_EN_2418_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2418_LEN)-1)<<BZ_PHY_NOTCH_EN_2418_POS))

/* 0x924 : notch_config_2420 */
#define BZ_PHY_NOTCH_CONFIG_2420_OFFSET                         (0x924)
#define BZ_PHY_NOTCH_COEFF_IM_2420                              BZ_PHY_NOTCH_COEFF_IM_2420
#define BZ_PHY_NOTCH_COEFF_IM_2420_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2420_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2420_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2420_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2420_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2420_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2420_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2420_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2420                              BZ_PHY_NOTCH_COEFF_RE_2420
#define BZ_PHY_NOTCH_COEFF_RE_2420_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2420_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2420_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2420_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2420_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2420_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2420_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2420_POS))
#define BZ_PHY_NOTCH_ALPHA_2420                                 BZ_PHY_NOTCH_ALPHA_2420
#define BZ_PHY_NOTCH_ALPHA_2420_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2420_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2420_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2420_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2420_POS)
#define BZ_PHY_NOTCH_ALPHA_2420_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2420_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2420_POS))
#define BZ_PHY_NOTCH_EN_2420                                    BZ_PHY_NOTCH_EN_2420
#define BZ_PHY_NOTCH_EN_2420_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2420_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2420_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2420_LEN)-1)<<BZ_PHY_NOTCH_EN_2420_POS)
#define BZ_PHY_NOTCH_EN_2420_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2420_LEN)-1)<<BZ_PHY_NOTCH_EN_2420_POS))

/* 0x928 : notch_config_2422 */
#define BZ_PHY_NOTCH_CONFIG_2422_OFFSET                         (0x928)
#define BZ_PHY_NOTCH_COEFF_IM_2422                              BZ_PHY_NOTCH_COEFF_IM_2422
#define BZ_PHY_NOTCH_COEFF_IM_2422_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2422_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2422_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2422_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2422_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2422_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2422_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2422_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2422                              BZ_PHY_NOTCH_COEFF_RE_2422
#define BZ_PHY_NOTCH_COEFF_RE_2422_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2422_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2422_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2422_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2422_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2422_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2422_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2422_POS))
#define BZ_PHY_NOTCH_ALPHA_2422                                 BZ_PHY_NOTCH_ALPHA_2422
#define BZ_PHY_NOTCH_ALPHA_2422_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2422_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2422_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2422_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2422_POS)
#define BZ_PHY_NOTCH_ALPHA_2422_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2422_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2422_POS))
#define BZ_PHY_NOTCH_EN_2422                                    BZ_PHY_NOTCH_EN_2422
#define BZ_PHY_NOTCH_EN_2422_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2422_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2422_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2422_LEN)-1)<<BZ_PHY_NOTCH_EN_2422_POS)
#define BZ_PHY_NOTCH_EN_2422_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2422_LEN)-1)<<BZ_PHY_NOTCH_EN_2422_POS))

/* 0x92C : notch_config_2424 */
#define BZ_PHY_NOTCH_CONFIG_2424_OFFSET                         (0x92C)
#define BZ_PHY_NOTCH_COEFF_IM_2424                              BZ_PHY_NOTCH_COEFF_IM_2424
#define BZ_PHY_NOTCH_COEFF_IM_2424_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2424_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2424_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2424_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2424_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2424_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2424_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2424_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2424                              BZ_PHY_NOTCH_COEFF_RE_2424
#define BZ_PHY_NOTCH_COEFF_RE_2424_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2424_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2424_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2424_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2424_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2424_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2424_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2424_POS))
#define BZ_PHY_NOTCH_ALPHA_2424                                 BZ_PHY_NOTCH_ALPHA_2424
#define BZ_PHY_NOTCH_ALPHA_2424_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2424_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2424_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2424_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2424_POS)
#define BZ_PHY_NOTCH_ALPHA_2424_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2424_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2424_POS))
#define BZ_PHY_NOTCH_EN_2424                                    BZ_PHY_NOTCH_EN_2424
#define BZ_PHY_NOTCH_EN_2424_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2424_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2424_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2424_LEN)-1)<<BZ_PHY_NOTCH_EN_2424_POS)
#define BZ_PHY_NOTCH_EN_2424_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2424_LEN)-1)<<BZ_PHY_NOTCH_EN_2424_POS))

/* 0x930 : notch_config_2426 */
#define BZ_PHY_NOTCH_CONFIG_2426_OFFSET                         (0x930)
#define BZ_PHY_NOTCH_COEFF_IM_2426                              BZ_PHY_NOTCH_COEFF_IM_2426
#define BZ_PHY_NOTCH_COEFF_IM_2426_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2426_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2426_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2426_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2426_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2426_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2426_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2426_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2426                              BZ_PHY_NOTCH_COEFF_RE_2426
#define BZ_PHY_NOTCH_COEFF_RE_2426_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2426_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2426_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2426_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2426_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2426_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2426_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2426_POS))
#define BZ_PHY_NOTCH_ALPHA_2426                                 BZ_PHY_NOTCH_ALPHA_2426
#define BZ_PHY_NOTCH_ALPHA_2426_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2426_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2426_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2426_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2426_POS)
#define BZ_PHY_NOTCH_ALPHA_2426_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2426_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2426_POS))
#define BZ_PHY_NOTCH_EN_2426                                    BZ_PHY_NOTCH_EN_2426
#define BZ_PHY_NOTCH_EN_2426_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2426_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2426_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2426_LEN)-1)<<BZ_PHY_NOTCH_EN_2426_POS)
#define BZ_PHY_NOTCH_EN_2426_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2426_LEN)-1)<<BZ_PHY_NOTCH_EN_2426_POS))

/* 0x934 : notch_config_2428 */
#define BZ_PHY_NOTCH_CONFIG_2428_OFFSET                         (0x934)
#define BZ_PHY_NOTCH_COEFF_IM_2428                              BZ_PHY_NOTCH_COEFF_IM_2428
#define BZ_PHY_NOTCH_COEFF_IM_2428_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2428_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2428_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2428_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2428_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2428_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2428_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2428_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2428                              BZ_PHY_NOTCH_COEFF_RE_2428
#define BZ_PHY_NOTCH_COEFF_RE_2428_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2428_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2428_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2428_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2428_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2428_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2428_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2428_POS))
#define BZ_PHY_NOTCH_ALPHA_2428                                 BZ_PHY_NOTCH_ALPHA_2428
#define BZ_PHY_NOTCH_ALPHA_2428_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2428_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2428_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2428_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2428_POS)
#define BZ_PHY_NOTCH_ALPHA_2428_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2428_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2428_POS))
#define BZ_PHY_NOTCH_EN_2428                                    BZ_PHY_NOTCH_EN_2428
#define BZ_PHY_NOTCH_EN_2428_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2428_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2428_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2428_LEN)-1)<<BZ_PHY_NOTCH_EN_2428_POS)
#define BZ_PHY_NOTCH_EN_2428_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2428_LEN)-1)<<BZ_PHY_NOTCH_EN_2428_POS))

/* 0x938 : notch_config_2430 */
#define BZ_PHY_NOTCH_CONFIG_2430_OFFSET                         (0x938)
#define BZ_PHY_NOTCH_COEFF_IM_2430                              BZ_PHY_NOTCH_COEFF_IM_2430
#define BZ_PHY_NOTCH_COEFF_IM_2430_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2430_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2430_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2430_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2430_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2430_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2430_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2430_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2430                              BZ_PHY_NOTCH_COEFF_RE_2430
#define BZ_PHY_NOTCH_COEFF_RE_2430_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2430_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2430_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2430_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2430_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2430_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2430_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2430_POS))
#define BZ_PHY_NOTCH_ALPHA_2430                                 BZ_PHY_NOTCH_ALPHA_2430
#define BZ_PHY_NOTCH_ALPHA_2430_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2430_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2430_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2430_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2430_POS)
#define BZ_PHY_NOTCH_ALPHA_2430_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2430_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2430_POS))
#define BZ_PHY_NOTCH_EN_2430                                    BZ_PHY_NOTCH_EN_2430
#define BZ_PHY_NOTCH_EN_2430_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2430_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2430_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2430_LEN)-1)<<BZ_PHY_NOTCH_EN_2430_POS)
#define BZ_PHY_NOTCH_EN_2430_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2430_LEN)-1)<<BZ_PHY_NOTCH_EN_2430_POS))

/* 0x93C : notch_config_2432 */
#define BZ_PHY_NOTCH_CONFIG_2432_OFFSET                         (0x93C)
#define BZ_PHY_NOTCH_COEFF_IM_2432                              BZ_PHY_NOTCH_COEFF_IM_2432
#define BZ_PHY_NOTCH_COEFF_IM_2432_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2432_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2432_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2432_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2432_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2432_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2432_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2432_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2432                              BZ_PHY_NOTCH_COEFF_RE_2432
#define BZ_PHY_NOTCH_COEFF_RE_2432_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2432_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2432_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2432_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2432_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2432_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2432_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2432_POS))
#define BZ_PHY_NOTCH_ALPHA_2432                                 BZ_PHY_NOTCH_ALPHA_2432
#define BZ_PHY_NOTCH_ALPHA_2432_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2432_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2432_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2432_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2432_POS)
#define BZ_PHY_NOTCH_ALPHA_2432_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2432_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2432_POS))
#define BZ_PHY_NOTCH_EN_2432                                    BZ_PHY_NOTCH_EN_2432
#define BZ_PHY_NOTCH_EN_2432_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2432_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2432_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2432_LEN)-1)<<BZ_PHY_NOTCH_EN_2432_POS)
#define BZ_PHY_NOTCH_EN_2432_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2432_LEN)-1)<<BZ_PHY_NOTCH_EN_2432_POS))

/* 0x940 : notch_config_2434 */
#define BZ_PHY_NOTCH_CONFIG_2434_OFFSET                         (0x940)
#define BZ_PHY_NOTCH_COEFF_IM_2434                              BZ_PHY_NOTCH_COEFF_IM_2434
#define BZ_PHY_NOTCH_COEFF_IM_2434_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2434_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2434_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2434_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2434_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2434_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2434_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2434_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2434                              BZ_PHY_NOTCH_COEFF_RE_2434
#define BZ_PHY_NOTCH_COEFF_RE_2434_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2434_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2434_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2434_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2434_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2434_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2434_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2434_POS))
#define BZ_PHY_NOTCH_ALPHA_2434                                 BZ_PHY_NOTCH_ALPHA_2434
#define BZ_PHY_NOTCH_ALPHA_2434_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2434_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2434_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2434_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2434_POS)
#define BZ_PHY_NOTCH_ALPHA_2434_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2434_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2434_POS))
#define BZ_PHY_NOTCH_EN_2434                                    BZ_PHY_NOTCH_EN_2434
#define BZ_PHY_NOTCH_EN_2434_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2434_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2434_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2434_LEN)-1)<<BZ_PHY_NOTCH_EN_2434_POS)
#define BZ_PHY_NOTCH_EN_2434_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2434_LEN)-1)<<BZ_PHY_NOTCH_EN_2434_POS))

/* 0x944 : notch_config_2436 */
#define BZ_PHY_NOTCH_CONFIG_2436_OFFSET                         (0x944)
#define BZ_PHY_NOTCH_COEFF_IM_2436                              BZ_PHY_NOTCH_COEFF_IM_2436
#define BZ_PHY_NOTCH_COEFF_IM_2436_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2436_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2436_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2436_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2436_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2436_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2436_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2436_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2436                              BZ_PHY_NOTCH_COEFF_RE_2436
#define BZ_PHY_NOTCH_COEFF_RE_2436_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2436_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2436_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2436_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2436_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2436_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2436_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2436_POS))
#define BZ_PHY_NOTCH_ALPHA_2436                                 BZ_PHY_NOTCH_ALPHA_2436
#define BZ_PHY_NOTCH_ALPHA_2436_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2436_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2436_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2436_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2436_POS)
#define BZ_PHY_NOTCH_ALPHA_2436_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2436_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2436_POS))
#define BZ_PHY_NOTCH_EN_2436                                    BZ_PHY_NOTCH_EN_2436
#define BZ_PHY_NOTCH_EN_2436_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2436_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2436_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2436_LEN)-1)<<BZ_PHY_NOTCH_EN_2436_POS)
#define BZ_PHY_NOTCH_EN_2436_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2436_LEN)-1)<<BZ_PHY_NOTCH_EN_2436_POS))

/* 0x948 : notch_config_2438 */
#define BZ_PHY_NOTCH_CONFIG_2438_OFFSET                         (0x948)
#define BZ_PHY_NOTCH_COEFF_IM_2438                              BZ_PHY_NOTCH_COEFF_IM_2438
#define BZ_PHY_NOTCH_COEFF_IM_2438_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2438_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2438_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2438_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2438_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2438_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2438_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2438_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2438                              BZ_PHY_NOTCH_COEFF_RE_2438
#define BZ_PHY_NOTCH_COEFF_RE_2438_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2438_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2438_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2438_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2438_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2438_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2438_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2438_POS))
#define BZ_PHY_NOTCH_ALPHA_2438                                 BZ_PHY_NOTCH_ALPHA_2438
#define BZ_PHY_NOTCH_ALPHA_2438_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2438_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2438_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2438_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2438_POS)
#define BZ_PHY_NOTCH_ALPHA_2438_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2438_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2438_POS))
#define BZ_PHY_NOTCH_EN_2438                                    BZ_PHY_NOTCH_EN_2438
#define BZ_PHY_NOTCH_EN_2438_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2438_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2438_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2438_LEN)-1)<<BZ_PHY_NOTCH_EN_2438_POS)
#define BZ_PHY_NOTCH_EN_2438_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2438_LEN)-1)<<BZ_PHY_NOTCH_EN_2438_POS))

/* 0x94C : notch_config_2440 */
#define BZ_PHY_NOTCH_CONFIG_2440_OFFSET                         (0x94C)
#define BZ_PHY_NOTCH_COEFF_IM_2440                              BZ_PHY_NOTCH_COEFF_IM_2440
#define BZ_PHY_NOTCH_COEFF_IM_2440_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2440_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2440_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2440_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2440_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2440_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2440_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2440_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2440                              BZ_PHY_NOTCH_COEFF_RE_2440
#define BZ_PHY_NOTCH_COEFF_RE_2440_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2440_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2440_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2440_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2440_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2440_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2440_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2440_POS))
#define BZ_PHY_NOTCH_ALPHA_2440                                 BZ_PHY_NOTCH_ALPHA_2440
#define BZ_PHY_NOTCH_ALPHA_2440_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2440_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2440_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2440_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2440_POS)
#define BZ_PHY_NOTCH_ALPHA_2440_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2440_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2440_POS))
#define BZ_PHY_NOTCH_EN_2440                                    BZ_PHY_NOTCH_EN_2440
#define BZ_PHY_NOTCH_EN_2440_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2440_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2440_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2440_LEN)-1)<<BZ_PHY_NOTCH_EN_2440_POS)
#define BZ_PHY_NOTCH_EN_2440_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2440_LEN)-1)<<BZ_PHY_NOTCH_EN_2440_POS))

/* 0x950 : notch_config_2442 */
#define BZ_PHY_NOTCH_CONFIG_2442_OFFSET                         (0x950)
#define BZ_PHY_NOTCH_COEFF_IM_2442                              BZ_PHY_NOTCH_COEFF_IM_2442
#define BZ_PHY_NOTCH_COEFF_IM_2442_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2442_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2442_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2442_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2442_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2442_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2442_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2442_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2442                              BZ_PHY_NOTCH_COEFF_RE_2442
#define BZ_PHY_NOTCH_COEFF_RE_2442_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2442_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2442_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2442_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2442_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2442_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2442_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2442_POS))
#define BZ_PHY_NOTCH_ALPHA_2442                                 BZ_PHY_NOTCH_ALPHA_2442
#define BZ_PHY_NOTCH_ALPHA_2442_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2442_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2442_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2442_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2442_POS)
#define BZ_PHY_NOTCH_ALPHA_2442_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2442_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2442_POS))
#define BZ_PHY_NOTCH_EN_2442                                    BZ_PHY_NOTCH_EN_2442
#define BZ_PHY_NOTCH_EN_2442_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2442_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2442_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2442_LEN)-1)<<BZ_PHY_NOTCH_EN_2442_POS)
#define BZ_PHY_NOTCH_EN_2442_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2442_LEN)-1)<<BZ_PHY_NOTCH_EN_2442_POS))

/* 0x954 : notch_config_2444 */
#define BZ_PHY_NOTCH_CONFIG_2444_OFFSET                         (0x954)
#define BZ_PHY_NOTCH_COEFF_IM_2444                              BZ_PHY_NOTCH_COEFF_IM_2444
#define BZ_PHY_NOTCH_COEFF_IM_2444_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2444_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2444_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2444_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2444_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2444_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2444_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2444_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2444                              BZ_PHY_NOTCH_COEFF_RE_2444
#define BZ_PHY_NOTCH_COEFF_RE_2444_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2444_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2444_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2444_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2444_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2444_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2444_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2444_POS))
#define BZ_PHY_NOTCH_ALPHA_2444                                 BZ_PHY_NOTCH_ALPHA_2444
#define BZ_PHY_NOTCH_ALPHA_2444_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2444_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2444_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2444_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2444_POS)
#define BZ_PHY_NOTCH_ALPHA_2444_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2444_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2444_POS))
#define BZ_PHY_NOTCH_EN_2444                                    BZ_PHY_NOTCH_EN_2444
#define BZ_PHY_NOTCH_EN_2444_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2444_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2444_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2444_LEN)-1)<<BZ_PHY_NOTCH_EN_2444_POS)
#define BZ_PHY_NOTCH_EN_2444_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2444_LEN)-1)<<BZ_PHY_NOTCH_EN_2444_POS))

/* 0x958 : notch_config_2446 */
#define BZ_PHY_NOTCH_CONFIG_2446_OFFSET                         (0x958)
#define BZ_PHY_NOTCH_COEFF_IM_2446                              BZ_PHY_NOTCH_COEFF_IM_2446
#define BZ_PHY_NOTCH_COEFF_IM_2446_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2446_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2446_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2446_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2446_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2446_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2446_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2446_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2446                              BZ_PHY_NOTCH_COEFF_RE_2446
#define BZ_PHY_NOTCH_COEFF_RE_2446_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2446_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2446_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2446_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2446_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2446_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2446_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2446_POS))
#define BZ_PHY_NOTCH_ALPHA_2446                                 BZ_PHY_NOTCH_ALPHA_2446
#define BZ_PHY_NOTCH_ALPHA_2446_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2446_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2446_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2446_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2446_POS)
#define BZ_PHY_NOTCH_ALPHA_2446_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2446_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2446_POS))
#define BZ_PHY_NOTCH_EN_2446                                    BZ_PHY_NOTCH_EN_2446
#define BZ_PHY_NOTCH_EN_2446_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2446_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2446_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2446_LEN)-1)<<BZ_PHY_NOTCH_EN_2446_POS)
#define BZ_PHY_NOTCH_EN_2446_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2446_LEN)-1)<<BZ_PHY_NOTCH_EN_2446_POS))

/* 0x95C : notch_config_2448 */
#define BZ_PHY_NOTCH_CONFIG_2448_OFFSET                         (0x95C)
#define BZ_PHY_NOTCH_COEFF_IM_2448                              BZ_PHY_NOTCH_COEFF_IM_2448
#define BZ_PHY_NOTCH_COEFF_IM_2448_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2448_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2448_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2448_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2448_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2448_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2448_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2448_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2448                              BZ_PHY_NOTCH_COEFF_RE_2448
#define BZ_PHY_NOTCH_COEFF_RE_2448_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2448_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2448_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2448_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2448_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2448_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2448_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2448_POS))
#define BZ_PHY_NOTCH_ALPHA_2448                                 BZ_PHY_NOTCH_ALPHA_2448
#define BZ_PHY_NOTCH_ALPHA_2448_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2448_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2448_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2448_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2448_POS)
#define BZ_PHY_NOTCH_ALPHA_2448_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2448_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2448_POS))
#define BZ_PHY_NOTCH_EN_2448                                    BZ_PHY_NOTCH_EN_2448
#define BZ_PHY_NOTCH_EN_2448_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2448_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2448_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2448_LEN)-1)<<BZ_PHY_NOTCH_EN_2448_POS)
#define BZ_PHY_NOTCH_EN_2448_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2448_LEN)-1)<<BZ_PHY_NOTCH_EN_2448_POS))

/* 0x960 : notch_config_2450 */
#define BZ_PHY_NOTCH_CONFIG_2450_OFFSET                         (0x960)
#define BZ_PHY_NOTCH_COEFF_IM_2450                              BZ_PHY_NOTCH_COEFF_IM_2450
#define BZ_PHY_NOTCH_COEFF_IM_2450_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2450_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2450_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2450_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2450_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2450_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2450_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2450_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2450                              BZ_PHY_NOTCH_COEFF_RE_2450
#define BZ_PHY_NOTCH_COEFF_RE_2450_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2450_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2450_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2450_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2450_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2450_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2450_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2450_POS))
#define BZ_PHY_NOTCH_ALPHA_2450                                 BZ_PHY_NOTCH_ALPHA_2450
#define BZ_PHY_NOTCH_ALPHA_2450_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2450_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2450_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2450_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2450_POS)
#define BZ_PHY_NOTCH_ALPHA_2450_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2450_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2450_POS))
#define BZ_PHY_NOTCH_EN_2450                                    BZ_PHY_NOTCH_EN_2450
#define BZ_PHY_NOTCH_EN_2450_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2450_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2450_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2450_LEN)-1)<<BZ_PHY_NOTCH_EN_2450_POS)
#define BZ_PHY_NOTCH_EN_2450_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2450_LEN)-1)<<BZ_PHY_NOTCH_EN_2450_POS))

/* 0x964 : notch_config_2452 */
#define BZ_PHY_NOTCH_CONFIG_2452_OFFSET                         (0x964)
#define BZ_PHY_NOTCH_COEFF_IM_2452                              BZ_PHY_NOTCH_COEFF_IM_2452
#define BZ_PHY_NOTCH_COEFF_IM_2452_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2452_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2452_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2452_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2452_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2452_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2452_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2452_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2452                              BZ_PHY_NOTCH_COEFF_RE_2452
#define BZ_PHY_NOTCH_COEFF_RE_2452_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2452_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2452_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2452_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2452_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2452_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2452_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2452_POS))
#define BZ_PHY_NOTCH_ALPHA_2452                                 BZ_PHY_NOTCH_ALPHA_2452
#define BZ_PHY_NOTCH_ALPHA_2452_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2452_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2452_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2452_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2452_POS)
#define BZ_PHY_NOTCH_ALPHA_2452_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2452_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2452_POS))
#define BZ_PHY_NOTCH_EN_2452                                    BZ_PHY_NOTCH_EN_2452
#define BZ_PHY_NOTCH_EN_2452_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2452_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2452_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2452_LEN)-1)<<BZ_PHY_NOTCH_EN_2452_POS)
#define BZ_PHY_NOTCH_EN_2452_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2452_LEN)-1)<<BZ_PHY_NOTCH_EN_2452_POS))

/* 0x968 : notch_config_2454 */
#define BZ_PHY_NOTCH_CONFIG_2454_OFFSET                         (0x968)
#define BZ_PHY_NOTCH_COEFF_IM_2454                              BZ_PHY_NOTCH_COEFF_IM_2454
#define BZ_PHY_NOTCH_COEFF_IM_2454_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2454_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2454_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2454_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2454_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2454_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2454_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2454_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2454                              BZ_PHY_NOTCH_COEFF_RE_2454
#define BZ_PHY_NOTCH_COEFF_RE_2454_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2454_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2454_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2454_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2454_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2454_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2454_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2454_POS))
#define BZ_PHY_NOTCH_ALPHA_2454                                 BZ_PHY_NOTCH_ALPHA_2454
#define BZ_PHY_NOTCH_ALPHA_2454_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2454_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2454_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2454_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2454_POS)
#define BZ_PHY_NOTCH_ALPHA_2454_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2454_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2454_POS))
#define BZ_PHY_NOTCH_EN_2454                                    BZ_PHY_NOTCH_EN_2454
#define BZ_PHY_NOTCH_EN_2454_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2454_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2454_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2454_LEN)-1)<<BZ_PHY_NOTCH_EN_2454_POS)
#define BZ_PHY_NOTCH_EN_2454_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2454_LEN)-1)<<BZ_PHY_NOTCH_EN_2454_POS))

/* 0x96C : notch_config_2456 */
#define BZ_PHY_NOTCH_CONFIG_2456_OFFSET                         (0x96C)
#define BZ_PHY_NOTCH_COEFF_IM_2456                              BZ_PHY_NOTCH_COEFF_IM_2456
#define BZ_PHY_NOTCH_COEFF_IM_2456_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2456_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2456_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2456_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2456_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2456_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2456_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2456_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2456                              BZ_PHY_NOTCH_COEFF_RE_2456
#define BZ_PHY_NOTCH_COEFF_RE_2456_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2456_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2456_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2456_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2456_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2456_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2456_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2456_POS))
#define BZ_PHY_NOTCH_ALPHA_2456                                 BZ_PHY_NOTCH_ALPHA_2456
#define BZ_PHY_NOTCH_ALPHA_2456_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2456_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2456_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2456_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2456_POS)
#define BZ_PHY_NOTCH_ALPHA_2456_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2456_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2456_POS))
#define BZ_PHY_NOTCH_EN_2456                                    BZ_PHY_NOTCH_EN_2456
#define BZ_PHY_NOTCH_EN_2456_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2456_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2456_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2456_LEN)-1)<<BZ_PHY_NOTCH_EN_2456_POS)
#define BZ_PHY_NOTCH_EN_2456_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2456_LEN)-1)<<BZ_PHY_NOTCH_EN_2456_POS))

/* 0x970 : notch_config_2458 */
#define BZ_PHY_NOTCH_CONFIG_2458_OFFSET                         (0x970)
#define BZ_PHY_NOTCH_COEFF_IM_2458                              BZ_PHY_NOTCH_COEFF_IM_2458
#define BZ_PHY_NOTCH_COEFF_IM_2458_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2458_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2458_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2458_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2458_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2458_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2458_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2458_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2458                              BZ_PHY_NOTCH_COEFF_RE_2458
#define BZ_PHY_NOTCH_COEFF_RE_2458_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2458_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2458_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2458_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2458_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2458_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2458_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2458_POS))
#define BZ_PHY_NOTCH_ALPHA_2458                                 BZ_PHY_NOTCH_ALPHA_2458
#define BZ_PHY_NOTCH_ALPHA_2458_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2458_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2458_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2458_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2458_POS)
#define BZ_PHY_NOTCH_ALPHA_2458_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2458_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2458_POS))
#define BZ_PHY_NOTCH_EN_2458                                    BZ_PHY_NOTCH_EN_2458
#define BZ_PHY_NOTCH_EN_2458_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2458_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2458_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2458_LEN)-1)<<BZ_PHY_NOTCH_EN_2458_POS)
#define BZ_PHY_NOTCH_EN_2458_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2458_LEN)-1)<<BZ_PHY_NOTCH_EN_2458_POS))

/* 0x974 : notch_config_2460 */
#define BZ_PHY_NOTCH_CONFIG_2460_OFFSET                         (0x974)
#define BZ_PHY_NOTCH_COEFF_IM_2460                              BZ_PHY_NOTCH_COEFF_IM_2460
#define BZ_PHY_NOTCH_COEFF_IM_2460_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2460_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2460_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2460_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2460_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2460_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2460_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2460_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2460                              BZ_PHY_NOTCH_COEFF_RE_2460
#define BZ_PHY_NOTCH_COEFF_RE_2460_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2460_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2460_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2460_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2460_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2460_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2460_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2460_POS))
#define BZ_PHY_NOTCH_ALPHA_2460                                 BZ_PHY_NOTCH_ALPHA_2460
#define BZ_PHY_NOTCH_ALPHA_2460_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2460_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2460_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2460_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2460_POS)
#define BZ_PHY_NOTCH_ALPHA_2460_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2460_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2460_POS))
#define BZ_PHY_NOTCH_EN_2460                                    BZ_PHY_NOTCH_EN_2460
#define BZ_PHY_NOTCH_EN_2460_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2460_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2460_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2460_LEN)-1)<<BZ_PHY_NOTCH_EN_2460_POS)
#define BZ_PHY_NOTCH_EN_2460_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2460_LEN)-1)<<BZ_PHY_NOTCH_EN_2460_POS))

/* 0x978 : notch_config_2462 */
#define BZ_PHY_NOTCH_CONFIG_2462_OFFSET                         (0x978)
#define BZ_PHY_NOTCH_COEFF_IM_2462                              BZ_PHY_NOTCH_COEFF_IM_2462
#define BZ_PHY_NOTCH_COEFF_IM_2462_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2462_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2462_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2462_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2462_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2462_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2462_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2462_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2462                              BZ_PHY_NOTCH_COEFF_RE_2462
#define BZ_PHY_NOTCH_COEFF_RE_2462_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2462_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2462_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2462_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2462_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2462_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2462_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2462_POS))
#define BZ_PHY_NOTCH_ALPHA_2462                                 BZ_PHY_NOTCH_ALPHA_2462
#define BZ_PHY_NOTCH_ALPHA_2462_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2462_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2462_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2462_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2462_POS)
#define BZ_PHY_NOTCH_ALPHA_2462_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2462_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2462_POS))
#define BZ_PHY_NOTCH_EN_2462                                    BZ_PHY_NOTCH_EN_2462
#define BZ_PHY_NOTCH_EN_2462_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2462_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2462_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2462_LEN)-1)<<BZ_PHY_NOTCH_EN_2462_POS)
#define BZ_PHY_NOTCH_EN_2462_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2462_LEN)-1)<<BZ_PHY_NOTCH_EN_2462_POS))

/* 0x97C : notch_config_2464 */
#define BZ_PHY_NOTCH_CONFIG_2464_OFFSET                         (0x97C)
#define BZ_PHY_NOTCH_COEFF_IM_2464                              BZ_PHY_NOTCH_COEFF_IM_2464
#define BZ_PHY_NOTCH_COEFF_IM_2464_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2464_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2464_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2464_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2464_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2464_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2464_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2464_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2464                              BZ_PHY_NOTCH_COEFF_RE_2464
#define BZ_PHY_NOTCH_COEFF_RE_2464_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2464_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2464_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2464_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2464_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2464_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2464_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2464_POS))
#define BZ_PHY_NOTCH_ALPHA_2464                                 BZ_PHY_NOTCH_ALPHA_2464
#define BZ_PHY_NOTCH_ALPHA_2464_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2464_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2464_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2464_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2464_POS)
#define BZ_PHY_NOTCH_ALPHA_2464_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2464_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2464_POS))
#define BZ_PHY_NOTCH_EN_2464                                    BZ_PHY_NOTCH_EN_2464
#define BZ_PHY_NOTCH_EN_2464_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2464_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2464_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2464_LEN)-1)<<BZ_PHY_NOTCH_EN_2464_POS)
#define BZ_PHY_NOTCH_EN_2464_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2464_LEN)-1)<<BZ_PHY_NOTCH_EN_2464_POS))

/* 0x980 : notch_config_2466 */
#define BZ_PHY_NOTCH_CONFIG_2466_OFFSET                         (0x980)
#define BZ_PHY_NOTCH_COEFF_IM_2466                              BZ_PHY_NOTCH_COEFF_IM_2466
#define BZ_PHY_NOTCH_COEFF_IM_2466_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2466_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2466_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2466_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2466_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2466_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2466_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2466_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2466                              BZ_PHY_NOTCH_COEFF_RE_2466
#define BZ_PHY_NOTCH_COEFF_RE_2466_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2466_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2466_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2466_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2466_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2466_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2466_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2466_POS))
#define BZ_PHY_NOTCH_ALPHA_2466                                 BZ_PHY_NOTCH_ALPHA_2466
#define BZ_PHY_NOTCH_ALPHA_2466_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2466_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2466_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2466_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2466_POS)
#define BZ_PHY_NOTCH_ALPHA_2466_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2466_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2466_POS))
#define BZ_PHY_NOTCH_EN_2466                                    BZ_PHY_NOTCH_EN_2466
#define BZ_PHY_NOTCH_EN_2466_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2466_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2466_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2466_LEN)-1)<<BZ_PHY_NOTCH_EN_2466_POS)
#define BZ_PHY_NOTCH_EN_2466_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2466_LEN)-1)<<BZ_PHY_NOTCH_EN_2466_POS))

/* 0x984 : notch_config_2468 */
#define BZ_PHY_NOTCH_CONFIG_2468_OFFSET                         (0x984)
#define BZ_PHY_NOTCH_COEFF_IM_2468                              BZ_PHY_NOTCH_COEFF_IM_2468
#define BZ_PHY_NOTCH_COEFF_IM_2468_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2468_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2468_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2468_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2468_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2468_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2468_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2468_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2468                              BZ_PHY_NOTCH_COEFF_RE_2468
#define BZ_PHY_NOTCH_COEFF_RE_2468_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2468_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2468_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2468_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2468_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2468_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2468_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2468_POS))
#define BZ_PHY_NOTCH_ALPHA_2468                                 BZ_PHY_NOTCH_ALPHA_2468
#define BZ_PHY_NOTCH_ALPHA_2468_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2468_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2468_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2468_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2468_POS)
#define BZ_PHY_NOTCH_ALPHA_2468_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2468_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2468_POS))
#define BZ_PHY_NOTCH_EN_2468                                    BZ_PHY_NOTCH_EN_2468
#define BZ_PHY_NOTCH_EN_2468_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2468_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2468_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2468_LEN)-1)<<BZ_PHY_NOTCH_EN_2468_POS)
#define BZ_PHY_NOTCH_EN_2468_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2468_LEN)-1)<<BZ_PHY_NOTCH_EN_2468_POS))

/* 0x988 : notch_config_2470 */
#define BZ_PHY_NOTCH_CONFIG_2470_OFFSET                         (0x988)
#define BZ_PHY_NOTCH_COEFF_IM_2470                              BZ_PHY_NOTCH_COEFF_IM_2470
#define BZ_PHY_NOTCH_COEFF_IM_2470_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2470_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2470_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2470_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2470_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2470_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2470_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2470_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2470                              BZ_PHY_NOTCH_COEFF_RE_2470
#define BZ_PHY_NOTCH_COEFF_RE_2470_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2470_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2470_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2470_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2470_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2470_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2470_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2470_POS))
#define BZ_PHY_NOTCH_ALPHA_2470                                 BZ_PHY_NOTCH_ALPHA_2470
#define BZ_PHY_NOTCH_ALPHA_2470_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2470_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2470_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2470_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2470_POS)
#define BZ_PHY_NOTCH_ALPHA_2470_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2470_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2470_POS))
#define BZ_PHY_NOTCH_EN_2470                                    BZ_PHY_NOTCH_EN_2470
#define BZ_PHY_NOTCH_EN_2470_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2470_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2470_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2470_LEN)-1)<<BZ_PHY_NOTCH_EN_2470_POS)
#define BZ_PHY_NOTCH_EN_2470_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2470_LEN)-1)<<BZ_PHY_NOTCH_EN_2470_POS))

/* 0x98C : notch_config_2472 */
#define BZ_PHY_NOTCH_CONFIG_2472_OFFSET                         (0x98C)
#define BZ_PHY_NOTCH_COEFF_IM_2472                              BZ_PHY_NOTCH_COEFF_IM_2472
#define BZ_PHY_NOTCH_COEFF_IM_2472_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2472_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2472_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2472_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2472_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2472_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2472_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2472_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2472                              BZ_PHY_NOTCH_COEFF_RE_2472
#define BZ_PHY_NOTCH_COEFF_RE_2472_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2472_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2472_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2472_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2472_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2472_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2472_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2472_POS))
#define BZ_PHY_NOTCH_ALPHA_2472                                 BZ_PHY_NOTCH_ALPHA_2472
#define BZ_PHY_NOTCH_ALPHA_2472_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2472_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2472_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2472_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2472_POS)
#define BZ_PHY_NOTCH_ALPHA_2472_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2472_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2472_POS))
#define BZ_PHY_NOTCH_EN_2472                                    BZ_PHY_NOTCH_EN_2472
#define BZ_PHY_NOTCH_EN_2472_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2472_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2472_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2472_LEN)-1)<<BZ_PHY_NOTCH_EN_2472_POS)
#define BZ_PHY_NOTCH_EN_2472_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2472_LEN)-1)<<BZ_PHY_NOTCH_EN_2472_POS))

/* 0x990 : notch_config_2474 */
#define BZ_PHY_NOTCH_CONFIG_2474_OFFSET                         (0x990)
#define BZ_PHY_NOTCH_COEFF_IM_2474                              BZ_PHY_NOTCH_COEFF_IM_2474
#define BZ_PHY_NOTCH_COEFF_IM_2474_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2474_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2474_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2474_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2474_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2474_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2474_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2474_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2474                              BZ_PHY_NOTCH_COEFF_RE_2474
#define BZ_PHY_NOTCH_COEFF_RE_2474_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2474_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2474_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2474_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2474_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2474_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2474_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2474_POS))
#define BZ_PHY_NOTCH_ALPHA_2474                                 BZ_PHY_NOTCH_ALPHA_2474
#define BZ_PHY_NOTCH_ALPHA_2474_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2474_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2474_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2474_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2474_POS)
#define BZ_PHY_NOTCH_ALPHA_2474_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2474_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2474_POS))
#define BZ_PHY_NOTCH_EN_2474                                    BZ_PHY_NOTCH_EN_2474
#define BZ_PHY_NOTCH_EN_2474_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2474_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2474_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2474_LEN)-1)<<BZ_PHY_NOTCH_EN_2474_POS)
#define BZ_PHY_NOTCH_EN_2474_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2474_LEN)-1)<<BZ_PHY_NOTCH_EN_2474_POS))

/* 0x994 : notch_config_2476 */
#define BZ_PHY_NOTCH_CONFIG_2476_OFFSET                         (0x994)
#define BZ_PHY_NOTCH_COEFF_IM_2476                              BZ_PHY_NOTCH_COEFF_IM_2476
#define BZ_PHY_NOTCH_COEFF_IM_2476_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2476_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2476_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2476_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2476_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2476_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2476_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2476_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2476                              BZ_PHY_NOTCH_COEFF_RE_2476
#define BZ_PHY_NOTCH_COEFF_RE_2476_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2476_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2476_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2476_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2476_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2476_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2476_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2476_POS))
#define BZ_PHY_NOTCH_ALPHA_2476                                 BZ_PHY_NOTCH_ALPHA_2476
#define BZ_PHY_NOTCH_ALPHA_2476_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2476_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2476_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2476_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2476_POS)
#define BZ_PHY_NOTCH_ALPHA_2476_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2476_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2476_POS))
#define BZ_PHY_NOTCH_EN_2476                                    BZ_PHY_NOTCH_EN_2476
#define BZ_PHY_NOTCH_EN_2476_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2476_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2476_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2476_LEN)-1)<<BZ_PHY_NOTCH_EN_2476_POS)
#define BZ_PHY_NOTCH_EN_2476_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2476_LEN)-1)<<BZ_PHY_NOTCH_EN_2476_POS))

/* 0x998 : notch_config_2478 */
#define BZ_PHY_NOTCH_CONFIG_2478_OFFSET                         (0x998)
#define BZ_PHY_NOTCH_COEFF_IM_2478                              BZ_PHY_NOTCH_COEFF_IM_2478
#define BZ_PHY_NOTCH_COEFF_IM_2478_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2478_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2478_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2478_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2478_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2478_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2478_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2478_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2478                              BZ_PHY_NOTCH_COEFF_RE_2478
#define BZ_PHY_NOTCH_COEFF_RE_2478_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2478_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2478_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2478_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2478_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2478_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2478_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2478_POS))
#define BZ_PHY_NOTCH_ALPHA_2478                                 BZ_PHY_NOTCH_ALPHA_2478
#define BZ_PHY_NOTCH_ALPHA_2478_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2478_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2478_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2478_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2478_POS)
#define BZ_PHY_NOTCH_ALPHA_2478_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2478_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2478_POS))
#define BZ_PHY_NOTCH_EN_2478                                    BZ_PHY_NOTCH_EN_2478
#define BZ_PHY_NOTCH_EN_2478_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2478_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2478_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2478_LEN)-1)<<BZ_PHY_NOTCH_EN_2478_POS)
#define BZ_PHY_NOTCH_EN_2478_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2478_LEN)-1)<<BZ_PHY_NOTCH_EN_2478_POS))

/* 0x99C : notch_config_2480 */
#define BZ_PHY_NOTCH_CONFIG_2480_OFFSET                         (0x99C)
#define BZ_PHY_NOTCH_COEFF_IM_2480                              BZ_PHY_NOTCH_COEFF_IM_2480
#define BZ_PHY_NOTCH_COEFF_IM_2480_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2480_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2480_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2480_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2480_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2480_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2480_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2480_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2480                              BZ_PHY_NOTCH_COEFF_RE_2480
#define BZ_PHY_NOTCH_COEFF_RE_2480_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2480_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2480_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2480_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2480_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2480_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2480_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2480_POS))
#define BZ_PHY_NOTCH_ALPHA_2480                                 BZ_PHY_NOTCH_ALPHA_2480
#define BZ_PHY_NOTCH_ALPHA_2480_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2480_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2480_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2480_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2480_POS)
#define BZ_PHY_NOTCH_ALPHA_2480_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2480_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2480_POS))
#define BZ_PHY_NOTCH_EN_2480                                    BZ_PHY_NOTCH_EN_2480
#define BZ_PHY_NOTCH_EN_2480_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2480_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2480_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2480_LEN)-1)<<BZ_PHY_NOTCH_EN_2480_POS)
#define BZ_PHY_NOTCH_EN_2480_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2480_LEN)-1)<<BZ_PHY_NOTCH_EN_2480_POS))

/* 0x9A0 : notch_config_2405 */
#define BZ_PHY_NOTCH_CONFIG_2405_OFFSET                         (0x9A0)
#define BZ_PHY_NOTCH_COEFF_IM_2405                              BZ_PHY_NOTCH_COEFF_IM_2405
#define BZ_PHY_NOTCH_COEFF_IM_2405_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2405_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2405_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2405_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2405_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2405_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2405_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2405_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2405                              BZ_PHY_NOTCH_COEFF_RE_2405
#define BZ_PHY_NOTCH_COEFF_RE_2405_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2405_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2405_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2405_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2405_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2405_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2405_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2405_POS))
#define BZ_PHY_NOTCH_ALPHA_2405                                 BZ_PHY_NOTCH_ALPHA_2405
#define BZ_PHY_NOTCH_ALPHA_2405_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2405_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2405_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2405_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2405_POS)
#define BZ_PHY_NOTCH_ALPHA_2405_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2405_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2405_POS))
#define BZ_PHY_NOTCH_EN_2405                                    BZ_PHY_NOTCH_EN_2405
#define BZ_PHY_NOTCH_EN_2405_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2405_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2405_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2405_LEN)-1)<<BZ_PHY_NOTCH_EN_2405_POS)
#define BZ_PHY_NOTCH_EN_2405_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2405_LEN)-1)<<BZ_PHY_NOTCH_EN_2405_POS))

/* 0x9A4 : notch_config_2415 */
#define BZ_PHY_NOTCH_CONFIG_2415_OFFSET                         (0x9A4)
#define BZ_PHY_NOTCH_COEFF_IM_2415                              BZ_PHY_NOTCH_COEFF_IM_2415
#define BZ_PHY_NOTCH_COEFF_IM_2415_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2415_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2415_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2415_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2415_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2415_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2415_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2415_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2415                              BZ_PHY_NOTCH_COEFF_RE_2415
#define BZ_PHY_NOTCH_COEFF_RE_2415_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2415_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2415_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2415_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2415_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2415_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2415_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2415_POS))
#define BZ_PHY_NOTCH_ALPHA_2415                                 BZ_PHY_NOTCH_ALPHA_2415
#define BZ_PHY_NOTCH_ALPHA_2415_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2415_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2415_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2415_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2415_POS)
#define BZ_PHY_NOTCH_ALPHA_2415_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2415_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2415_POS))
#define BZ_PHY_NOTCH_EN_2415                                    BZ_PHY_NOTCH_EN_2415
#define BZ_PHY_NOTCH_EN_2415_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2415_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2415_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2415_LEN)-1)<<BZ_PHY_NOTCH_EN_2415_POS)
#define BZ_PHY_NOTCH_EN_2415_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2415_LEN)-1)<<BZ_PHY_NOTCH_EN_2415_POS))

/* 0x9A8 : notch_config_2425 */
#define BZ_PHY_NOTCH_CONFIG_2425_OFFSET                         (0x9A8)
#define BZ_PHY_NOTCH_COEFF_IM_2425                              BZ_PHY_NOTCH_COEFF_IM_2425
#define BZ_PHY_NOTCH_COEFF_IM_2425_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2425_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2425_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2425_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2425_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2425_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2425_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2425_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2425                              BZ_PHY_NOTCH_COEFF_RE_2425
#define BZ_PHY_NOTCH_COEFF_RE_2425_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2425_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2425_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2425_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2425_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2425_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2425_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2425_POS))
#define BZ_PHY_NOTCH_ALPHA_2425                                 BZ_PHY_NOTCH_ALPHA_2425
#define BZ_PHY_NOTCH_ALPHA_2425_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2425_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2425_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2425_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2425_POS)
#define BZ_PHY_NOTCH_ALPHA_2425_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2425_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2425_POS))
#define BZ_PHY_NOTCH_EN_2425                                    BZ_PHY_NOTCH_EN_2425
#define BZ_PHY_NOTCH_EN_2425_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2425_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2425_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2425_LEN)-1)<<BZ_PHY_NOTCH_EN_2425_POS)
#define BZ_PHY_NOTCH_EN_2425_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2425_LEN)-1)<<BZ_PHY_NOTCH_EN_2425_POS))

/* 0x9AC : notch_config_2435 */
#define BZ_PHY_NOTCH_CONFIG_2435_OFFSET                         (0x9AC)
#define BZ_PHY_NOTCH_COEFF_IM_2435                              BZ_PHY_NOTCH_COEFF_IM_2435
#define BZ_PHY_NOTCH_COEFF_IM_2435_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2435_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2435_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2435_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2435_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2435_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2435_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2435_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2435                              BZ_PHY_NOTCH_COEFF_RE_2435
#define BZ_PHY_NOTCH_COEFF_RE_2435_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2435_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2435_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2435_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2435_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2435_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2435_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2435_POS))
#define BZ_PHY_NOTCH_ALPHA_2435                                 BZ_PHY_NOTCH_ALPHA_2435
#define BZ_PHY_NOTCH_ALPHA_2435_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2435_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2435_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2435_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2435_POS)
#define BZ_PHY_NOTCH_ALPHA_2435_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2435_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2435_POS))
#define BZ_PHY_NOTCH_EN_2435                                    BZ_PHY_NOTCH_EN_2435
#define BZ_PHY_NOTCH_EN_2435_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2435_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2435_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2435_LEN)-1)<<BZ_PHY_NOTCH_EN_2435_POS)
#define BZ_PHY_NOTCH_EN_2435_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2435_LEN)-1)<<BZ_PHY_NOTCH_EN_2435_POS))

/* 0x9B0 : notch_config_2445 */
#define BZ_PHY_NOTCH_CONFIG_2445_OFFSET                         (0x9B0)
#define BZ_PHY_NOTCH_COEFF_IM_2445                              BZ_PHY_NOTCH_COEFF_IM_2445
#define BZ_PHY_NOTCH_COEFF_IM_2445_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2445_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2445_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2445_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2445_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2445_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2445_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2445_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2445                              BZ_PHY_NOTCH_COEFF_RE_2445
#define BZ_PHY_NOTCH_COEFF_RE_2445_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2445_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2445_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2445_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2445_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2445_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2445_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2445_POS))
#define BZ_PHY_NOTCH_ALPHA_2445                                 BZ_PHY_NOTCH_ALPHA_2445
#define BZ_PHY_NOTCH_ALPHA_2445_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2445_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2445_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2445_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2445_POS)
#define BZ_PHY_NOTCH_ALPHA_2445_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2445_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2445_POS))
#define BZ_PHY_NOTCH_EN_2445                                    BZ_PHY_NOTCH_EN_2445
#define BZ_PHY_NOTCH_EN_2445_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2445_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2445_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2445_LEN)-1)<<BZ_PHY_NOTCH_EN_2445_POS)
#define BZ_PHY_NOTCH_EN_2445_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2445_LEN)-1)<<BZ_PHY_NOTCH_EN_2445_POS))

/* 0x9B4 : notch_config_2455 */
#define BZ_PHY_NOTCH_CONFIG_2455_OFFSET                         (0x9B4)
#define BZ_PHY_NOTCH_COEFF_IM_2455                              BZ_PHY_NOTCH_COEFF_IM_2455
#define BZ_PHY_NOTCH_COEFF_IM_2455_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2455_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2455_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2455_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2455_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2455_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2455_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2455_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2455                              BZ_PHY_NOTCH_COEFF_RE_2455
#define BZ_PHY_NOTCH_COEFF_RE_2455_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2455_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2455_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2455_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2455_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2455_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2455_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2455_POS))
#define BZ_PHY_NOTCH_ALPHA_2455                                 BZ_PHY_NOTCH_ALPHA_2455
#define BZ_PHY_NOTCH_ALPHA_2455_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2455_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2455_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2455_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2455_POS)
#define BZ_PHY_NOTCH_ALPHA_2455_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2455_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2455_POS))
#define BZ_PHY_NOTCH_EN_2455                                    BZ_PHY_NOTCH_EN_2455
#define BZ_PHY_NOTCH_EN_2455_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2455_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2455_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2455_LEN)-1)<<BZ_PHY_NOTCH_EN_2455_POS)
#define BZ_PHY_NOTCH_EN_2455_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2455_LEN)-1)<<BZ_PHY_NOTCH_EN_2455_POS))

/* 0x9B8 : notch_config_2465 */
#define BZ_PHY_NOTCH_CONFIG_2465_OFFSET                         (0x9B8)
#define BZ_PHY_NOTCH_COEFF_IM_2465                              BZ_PHY_NOTCH_COEFF_IM_2465
#define BZ_PHY_NOTCH_COEFF_IM_2465_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2465_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2465_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2465_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2465_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2465_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2465_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2465_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2465                              BZ_PHY_NOTCH_COEFF_RE_2465
#define BZ_PHY_NOTCH_COEFF_RE_2465_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2465_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2465_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2465_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2465_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2465_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2465_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2465_POS))
#define BZ_PHY_NOTCH_ALPHA_2465                                 BZ_PHY_NOTCH_ALPHA_2465
#define BZ_PHY_NOTCH_ALPHA_2465_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2465_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2465_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2465_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2465_POS)
#define BZ_PHY_NOTCH_ALPHA_2465_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2465_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2465_POS))
#define BZ_PHY_NOTCH_EN_2465                                    BZ_PHY_NOTCH_EN_2465
#define BZ_PHY_NOTCH_EN_2465_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2465_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2465_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2465_LEN)-1)<<BZ_PHY_NOTCH_EN_2465_POS)
#define BZ_PHY_NOTCH_EN_2465_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2465_LEN)-1)<<BZ_PHY_NOTCH_EN_2465_POS))

/* 0x9BC : notch_config_2475 */
#define BZ_PHY_NOTCH_CONFIG_2475_OFFSET                         (0x9BC)
#define BZ_PHY_NOTCH_COEFF_IM_2475                              BZ_PHY_NOTCH_COEFF_IM_2475
#define BZ_PHY_NOTCH_COEFF_IM_2475_POS                          (0U)
#define BZ_PHY_NOTCH_COEFF_IM_2475_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_IM_2475_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_IM_2475_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2475_POS)
#define BZ_PHY_NOTCH_COEFF_IM_2475_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_IM_2475_LEN)-1)<<BZ_PHY_NOTCH_COEFF_IM_2475_POS))
#define BZ_PHY_NOTCH_COEFF_RE_2475                              BZ_PHY_NOTCH_COEFF_RE_2475
#define BZ_PHY_NOTCH_COEFF_RE_2475_POS                          (16U)
#define BZ_PHY_NOTCH_COEFF_RE_2475_LEN                          (10U)
#define BZ_PHY_NOTCH_COEFF_RE_2475_MSK                          (((1U<<BZ_PHY_NOTCH_COEFF_RE_2475_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2475_POS)
#define BZ_PHY_NOTCH_COEFF_RE_2475_UMSK                         (~(((1U<<BZ_PHY_NOTCH_COEFF_RE_2475_LEN)-1)<<BZ_PHY_NOTCH_COEFF_RE_2475_POS))
#define BZ_PHY_NOTCH_ALPHA_2475                                 BZ_PHY_NOTCH_ALPHA_2475
#define BZ_PHY_NOTCH_ALPHA_2475_POS                             (28U)
#define BZ_PHY_NOTCH_ALPHA_2475_LEN                             (2U)
#define BZ_PHY_NOTCH_ALPHA_2475_MSK                             (((1U<<BZ_PHY_NOTCH_ALPHA_2475_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2475_POS)
#define BZ_PHY_NOTCH_ALPHA_2475_UMSK                            (~(((1U<<BZ_PHY_NOTCH_ALPHA_2475_LEN)-1)<<BZ_PHY_NOTCH_ALPHA_2475_POS))
#define BZ_PHY_NOTCH_EN_2475                                    BZ_PHY_NOTCH_EN_2475
#define BZ_PHY_NOTCH_EN_2475_POS                                (31U)
#define BZ_PHY_NOTCH_EN_2475_LEN                                (1U)
#define BZ_PHY_NOTCH_EN_2475_MSK                                (((1U<<BZ_PHY_NOTCH_EN_2475_LEN)-1)<<BZ_PHY_NOTCH_EN_2475_POS)
#define BZ_PHY_NOTCH_EN_2475_UMSK                               (~(((1U<<BZ_PHY_NOTCH_EN_2475_LEN)-1)<<BZ_PHY_NOTCH_EN_2475_POS))

/* 0xA00 : phy_fpga_ctrl_0 */
#define BZ_PHY_PHY_FPGA_CTRL_0_OFFSET                           (0xA00)
#define BZ_PHY_PF_TH2                                           BZ_PHY_PF_TH2
#define BZ_PHY_PF_TH2_POS                                       (0U)
#define BZ_PHY_PF_TH2_LEN                                       (10U)
#define BZ_PHY_PF_TH2_MSK                                       (((1U<<BZ_PHY_PF_TH2_LEN)-1)<<BZ_PHY_PF_TH2_POS)
#define BZ_PHY_PF_TH2_UMSK                                      (~(((1U<<BZ_PHY_PF_TH2_LEN)-1)<<BZ_PHY_PF_TH2_POS))
#define BZ_PHY_PF_TH1                                           BZ_PHY_PF_TH1
#define BZ_PHY_PF_TH1_POS                                       (10U)
#define BZ_PHY_PF_TH1_LEN                                       (10U)
#define BZ_PHY_PF_TH1_MSK                                       (((1U<<BZ_PHY_PF_TH1_LEN)-1)<<BZ_PHY_PF_TH1_POS)
#define BZ_PHY_PF_TH1_UMSK                                      (~(((1U<<BZ_PHY_PF_TH1_LEN)-1)<<BZ_PHY_PF_TH1_POS))
#define BZ_PHY_PF_EN_Q                                          BZ_PHY_PF_EN_Q
#define BZ_PHY_PF_EN_Q_POS                                      (20U)
#define BZ_PHY_PF_EN_Q_LEN                                      (1U)
#define BZ_PHY_PF_EN_Q_MSK                                      (((1U<<BZ_PHY_PF_EN_Q_LEN)-1)<<BZ_PHY_PF_EN_Q_POS)
#define BZ_PHY_PF_EN_Q_UMSK                                     (~(((1U<<BZ_PHY_PF_EN_Q_LEN)-1)<<BZ_PHY_PF_EN_Q_POS))
#define BZ_PHY_PF_EN_I                                          BZ_PHY_PF_EN_I
#define BZ_PHY_PF_EN_I_POS                                      (21U)
#define BZ_PHY_PF_EN_I_LEN                                      (1U)
#define BZ_PHY_PF_EN_I_MSK                                      (((1U<<BZ_PHY_PF_EN_I_LEN)-1)<<BZ_PHY_PF_EN_I_POS)
#define BZ_PHY_PF_EN_I_UMSK                                     (~(((1U<<BZ_PHY_PF_EN_I_LEN)-1)<<BZ_PHY_PF_EN_I_POS))
#define BZ_PHY_FPGA_DIG_GAIN                                    BZ_PHY_FPGA_DIG_GAIN
#define BZ_PHY_FPGA_DIG_GAIN_POS                                (26U)
#define BZ_PHY_FPGA_DIG_GAIN_LEN                                (6U)
#define BZ_PHY_FPGA_DIG_GAIN_MSK                                (((1U<<BZ_PHY_FPGA_DIG_GAIN_LEN)-1)<<BZ_PHY_FPGA_DIG_GAIN_POS)
#define BZ_PHY_FPGA_DIG_GAIN_UMSK                               (~(((1U<<BZ_PHY_FPGA_DIG_GAIN_LEN)-1)<<BZ_PHY_FPGA_DIG_GAIN_POS))


struct  bz_phy_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[2048];

    /* 0x800 : phy_top_0 */
    union {
        struct {
            uint32_t rxdfe_en                       :  1; /* [    0],        r/w,        0x1 */
            uint32_t dfe_rxmode_force               :  1; /* [    1],        r/w,        0x0 */
            uint32_t dfe_rxmode                     :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t txdfe_en                       :  1; /* [    4],        r/w,        0x1 */
            uint32_t dfe_txmode_force               :  1; /* [    5],        r/w,        0x0 */
            uint32_t dfe_txmode                     :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t dfe_clk_force_on               :  1; /* [    8],        r/w,        0x0 */
            uint32_t adc_clk_sel                    :  1; /* [    9],        r/w,        0x1 */
            uint32_t dfe_rxdfe_4s_on                :  1; /* [   10],        r/w,        0x0 */
            uint32_t dfe_txdfe_4s_on                :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
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
            uint32_t rx_dfe_notch_en                :  1; /* [    0],        r/w,        0x0 */
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
            uint32_t rx_dfe_notch_rot_q             : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t rx_dfe_notch_rot_i             : 10; /* [19:10],        r/w,        0x0 */
            uint32_t rx_dfe_notch_alpha             :  2; /* [21:20],        r/w,        0x0 */
            uint32_t rx_dfe_notch_force             :  1; /* [   22],        r/w,        0x0 */
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
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
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
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
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
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
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
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
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
            uint32_t phy_testbus_read               : 32; /* [31: 0],          r,    0xbf707 */
        }BF;
        uint32_t WORD;
    } phy_testbus_0;

    /* 0x85C : phy_testbus_1 */
    union {
        struct {
            uint32_t phy_testbus_sel                :  4; /* [ 3: 0],        r/w,        0x0 */
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
            uint32_t sts_agc_rf_lna                 :  3; /* [23:21],          r,        0x0 */
            uint32_t reserved_24_30                 :  7; /* [30:24],       rsvd,        0x0 */
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
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
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
            uint32_t zb_dm_sfd_found                :  1; /* [   10],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t zb_dm_phy_len                  :  8; /* [23:16],          r,        0x0 */
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
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
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

    /* 0x89c  reserved */
    uint8_t RESERVED0x89c[100];

    /* 0x900 : notch_config_2402 */
    union {
        struct {
            uint32_t notch_coeff_im_2402            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2402            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2402               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2402                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2402;

    /* 0x904 : notch_config_2404 */
    union {
        struct {
            uint32_t notch_coeff_im_2404            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2404            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2404               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2404                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2404;

    /* 0x908 : notch_config_2406 */
    union {
        struct {
            uint32_t notch_coeff_im_2406            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2406            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2406               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2406                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2406;

    /* 0x90C : notch_config_2408 */
    union {
        struct {
            uint32_t notch_coeff_im_2408            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2408            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2408               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2408                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2408;

    /* 0x910 : notch_config_2410 */
    union {
        struct {
            uint32_t notch_coeff_im_2410            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2410            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2410               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2410                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2410;

    /* 0x914 : notch_config_2412 */
    union {
        struct {
            uint32_t notch_coeff_im_2412            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2412            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2412               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2412                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2412;

    /* 0x918 : notch_config_2414 */
    union {
        struct {
            uint32_t notch_coeff_im_2414            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2414            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2414               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2414                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2414;

    /* 0x91C : notch_config_2416 */
    union {
        struct {
            uint32_t notch_coeff_im_2416            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2416            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2416               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2416                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2416;

    /* 0x920 : notch_config_2418 */
    union {
        struct {
            uint32_t notch_coeff_im_2418            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2418            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2418               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2418                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2418;

    /* 0x924 : notch_config_2420 */
    union {
        struct {
            uint32_t notch_coeff_im_2420            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2420            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2420               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2420                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2420;

    /* 0x928 : notch_config_2422 */
    union {
        struct {
            uint32_t notch_coeff_im_2422            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2422            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2422               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2422                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2422;

    /* 0x92C : notch_config_2424 */
    union {
        struct {
            uint32_t notch_coeff_im_2424            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2424            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2424               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2424                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2424;

    /* 0x930 : notch_config_2426 */
    union {
        struct {
            uint32_t notch_coeff_im_2426            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2426            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2426               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2426                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2426;

    /* 0x934 : notch_config_2428 */
    union {
        struct {
            uint32_t notch_coeff_im_2428            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2428            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2428               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2428                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2428;

    /* 0x938 : notch_config_2430 */
    union {
        struct {
            uint32_t notch_coeff_im_2430            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2430            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2430               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2430                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2430;

    /* 0x93C : notch_config_2432 */
    union {
        struct {
            uint32_t notch_coeff_im_2432            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2432            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2432               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2432                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2432;

    /* 0x940 : notch_config_2434 */
    union {
        struct {
            uint32_t notch_coeff_im_2434            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2434            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2434               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2434                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2434;

    /* 0x944 : notch_config_2436 */
    union {
        struct {
            uint32_t notch_coeff_im_2436            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2436            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2436               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2436                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2436;

    /* 0x948 : notch_config_2438 */
    union {
        struct {
            uint32_t notch_coeff_im_2438            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2438            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2438               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2438                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2438;

    /* 0x94C : notch_config_2440 */
    union {
        struct {
            uint32_t notch_coeff_im_2440            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2440            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2440               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2440                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2440;

    /* 0x950 : notch_config_2442 */
    union {
        struct {
            uint32_t notch_coeff_im_2442            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2442            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2442               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2442                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2442;

    /* 0x954 : notch_config_2444 */
    union {
        struct {
            uint32_t notch_coeff_im_2444            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2444            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2444               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2444                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2444;

    /* 0x958 : notch_config_2446 */
    union {
        struct {
            uint32_t notch_coeff_im_2446            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2446            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2446               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2446                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2446;

    /* 0x95C : notch_config_2448 */
    union {
        struct {
            uint32_t notch_coeff_im_2448            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2448            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2448               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2448                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2448;

    /* 0x960 : notch_config_2450 */
    union {
        struct {
            uint32_t notch_coeff_im_2450            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2450            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2450               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2450                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2450;

    /* 0x964 : notch_config_2452 */
    union {
        struct {
            uint32_t notch_coeff_im_2452            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2452            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2452               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2452                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2452;

    /* 0x968 : notch_config_2454 */
    union {
        struct {
            uint32_t notch_coeff_im_2454            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2454            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2454               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2454                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2454;

    /* 0x96C : notch_config_2456 */
    union {
        struct {
            uint32_t notch_coeff_im_2456            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2456            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2456               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2456                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2456;

    /* 0x970 : notch_config_2458 */
    union {
        struct {
            uint32_t notch_coeff_im_2458            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2458            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2458               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2458                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2458;

    /* 0x974 : notch_config_2460 */
    union {
        struct {
            uint32_t notch_coeff_im_2460            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2460            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2460               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2460                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2460;

    /* 0x978 : notch_config_2462 */
    union {
        struct {
            uint32_t notch_coeff_im_2462            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2462            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2462               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2462                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2462;

    /* 0x97C : notch_config_2464 */
    union {
        struct {
            uint32_t notch_coeff_im_2464            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2464            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2464               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2464                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2464;

    /* 0x980 : notch_config_2466 */
    union {
        struct {
            uint32_t notch_coeff_im_2466            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2466            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2466               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2466                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2466;

    /* 0x984 : notch_config_2468 */
    union {
        struct {
            uint32_t notch_coeff_im_2468            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2468            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2468               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2468                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2468;

    /* 0x988 : notch_config_2470 */
    union {
        struct {
            uint32_t notch_coeff_im_2470            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2470            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2470               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2470                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2470;

    /* 0x98C : notch_config_2472 */
    union {
        struct {
            uint32_t notch_coeff_im_2472            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2472            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2472               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2472                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2472;

    /* 0x990 : notch_config_2474 */
    union {
        struct {
            uint32_t notch_coeff_im_2474            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2474            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2474               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2474                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2474;

    /* 0x994 : notch_config_2476 */
    union {
        struct {
            uint32_t notch_coeff_im_2476            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2476            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2476               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2476                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2476;

    /* 0x998 : notch_config_2478 */
    union {
        struct {
            uint32_t notch_coeff_im_2478            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2478            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2478               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2478                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2478;

    /* 0x99C : notch_config_2480 */
    union {
        struct {
            uint32_t notch_coeff_im_2480            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2480            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2480               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2480                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2480;

    /* 0x9A0 : notch_config_2405 */
    union {
        struct {
            uint32_t notch_coeff_im_2405            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2405            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2405               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2405                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2405;

    /* 0x9A4 : notch_config_2415 */
    union {
        struct {
            uint32_t notch_coeff_im_2415            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2415            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2415               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2415                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2415;

    /* 0x9A8 : notch_config_2425 */
    union {
        struct {
            uint32_t notch_coeff_im_2425            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2425            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2425               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2425                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2425;

    /* 0x9AC : notch_config_2435 */
    union {
        struct {
            uint32_t notch_coeff_im_2435            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2435            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2435               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2435                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2435;

    /* 0x9B0 : notch_config_2445 */
    union {
        struct {
            uint32_t notch_coeff_im_2445            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2445            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2445               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2445                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2445;

    /* 0x9B4 : notch_config_2455 */
    union {
        struct {
            uint32_t notch_coeff_im_2455            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2455            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2455               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2455                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2455;

    /* 0x9B8 : notch_config_2465 */
    union {
        struct {
            uint32_t notch_coeff_im_2465            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2465            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2465               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2465                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2465;

    /* 0x9BC : notch_config_2475 */
    union {
        struct {
            uint32_t notch_coeff_im_2475            : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t notch_coeff_re_2475            : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t notch_alpha_2475               :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t notch_en_2475                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } notch_config_2475;

    /* 0x9c0  reserved */
    uint8_t RESERVED0x9c0[64];

    /* 0xA00 : phy_fpga_ctrl_0 */
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
