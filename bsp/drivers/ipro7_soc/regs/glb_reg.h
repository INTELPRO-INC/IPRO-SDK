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

#ifndef  __GLB_REG_H__
#define  __GLB_REG_H__

#include "ipro7.h"

/* 0x0 : clk_cfg0 */
#define GLB_CLK_CFG0_OFFSET                                     (0x0)
#define GLB_REG_HCLK_EN                                         GLB_REG_HCLK_EN
#define GLB_REG_HCLK_EN_POS                                     (0U)
#define GLB_REG_HCLK_EN_LEN                                     (1U)
#define GLB_REG_HCLK_EN_MSK                                     (((1U<<GLB_REG_HCLK_EN_LEN)-1)<<GLB_REG_HCLK_EN_POS)
#define GLB_REG_HCLK_EN_UMSK                                    (~(((1U<<GLB_REG_HCLK_EN_LEN)-1)<<GLB_REG_HCLK_EN_POS))
#define GLB_REG_BCLK_EN                                         GLB_REG_BCLK_EN
#define GLB_REG_BCLK_EN_POS                                     (1U)
#define GLB_REG_BCLK_EN_LEN                                     (1U)
#define GLB_REG_BCLK_EN_MSK                                     (((1U<<GLB_REG_BCLK_EN_LEN)-1)<<GLB_REG_BCLK_EN_POS)
#define GLB_REG_BCLK_EN_UMSK                                    (~(((1U<<GLB_REG_BCLK_EN_LEN)-1)<<GLB_REG_BCLK_EN_POS))
#define GLB_REG_PLL_SEL                                         GLB_REG_PLL_SEL
#define GLB_REG_PLL_SEL_POS                                     (4U)
#define GLB_REG_PLL_SEL_LEN                                     (2U)
#define GLB_REG_PLL_SEL_MSK                                     (((1U<<GLB_REG_PLL_SEL_LEN)-1)<<GLB_REG_PLL_SEL_POS)
#define GLB_REG_PLL_SEL_UMSK                                    (~(((1U<<GLB_REG_PLL_SEL_LEN)-1)<<GLB_REG_PLL_SEL_POS))
#define GLB_HBN_ROOT_CLK_SEL                                    GLB_HBN_ROOT_CLK_SEL
#define GLB_HBN_ROOT_CLK_SEL_POS                                (6U)
#define GLB_HBN_ROOT_CLK_SEL_LEN                                (2U)
#define GLB_HBN_ROOT_CLK_SEL_MSK                                (((1U<<GLB_HBN_ROOT_CLK_SEL_LEN)-1)<<GLB_HBN_ROOT_CLK_SEL_POS)
#define GLB_HBN_ROOT_CLK_SEL_UMSK                               (~(((1U<<GLB_HBN_ROOT_CLK_SEL_LEN)-1)<<GLB_HBN_ROOT_CLK_SEL_POS))
#define GLB_REG_BCLK_DIV_ACT_PULSE                              GLB_REG_BCLK_DIV_ACT_PULSE
#define GLB_REG_BCLK_DIV_ACT_PULSE_POS                          (8U)
#define GLB_REG_BCLK_DIV_ACT_PULSE_LEN                          (1U)
#define GLB_REG_BCLK_DIV_ACT_PULSE_MSK                          (((1U<<GLB_REG_BCLK_DIV_ACT_PULSE_LEN)-1)<<GLB_REG_BCLK_DIV_ACT_PULSE_POS)
#define GLB_REG_BCLK_DIV_ACT_PULSE_UMSK                         (~(((1U<<GLB_REG_BCLK_DIV_ACT_PULSE_LEN)-1)<<GLB_REG_BCLK_DIV_ACT_PULSE_POS))
#define GLB_REG_BCLK_DIV_BYPASS                                 GLB_REG_BCLK_DIV_BYPASS
#define GLB_REG_BCLK_DIV_BYPASS_POS                             (9U)
#define GLB_REG_BCLK_DIV_BYPASS_LEN                             (1U)
#define GLB_REG_BCLK_DIV_BYPASS_MSK                             (((1U<<GLB_REG_BCLK_DIV_BYPASS_LEN)-1)<<GLB_REG_BCLK_DIV_BYPASS_POS)
#define GLB_REG_BCLK_DIV_BYPASS_UMSK                            (~(((1U<<GLB_REG_BCLK_DIV_BYPASS_LEN)-1)<<GLB_REG_BCLK_DIV_BYPASS_POS))
#define GLB_STS_BCLK_PROT_DONE                                  GLB_STS_BCLK_PROT_DONE
#define GLB_STS_BCLK_PROT_DONE_POS                              (10U)
#define GLB_STS_BCLK_PROT_DONE_LEN                              (1U)
#define GLB_STS_BCLK_PROT_DONE_MSK                              (((1U<<GLB_STS_BCLK_PROT_DONE_LEN)-1)<<GLB_STS_BCLK_PROT_DONE_POS)
#define GLB_STS_BCLK_PROT_DONE_UMSK                             (~(((1U<<GLB_STS_BCLK_PROT_DONE_LEN)-1)<<GLB_STS_BCLK_PROT_DONE_POS))
#define GLB_REG_BCLK_SW_DONE_CNT                                GLB_REG_BCLK_SW_DONE_CNT
#define GLB_REG_BCLK_SW_DONE_CNT_POS                            (12U)
#define GLB_REG_BCLK_SW_DONE_CNT_LEN                            (4U)
#define GLB_REG_BCLK_SW_DONE_CNT_MSK                            (((1U<<GLB_REG_BCLK_SW_DONE_CNT_LEN)-1)<<GLB_REG_BCLK_SW_DONE_CNT_POS)
#define GLB_REG_BCLK_SW_DONE_CNT_UMSK                           (~(((1U<<GLB_REG_BCLK_SW_DONE_CNT_LEN)-1)<<GLB_REG_BCLK_SW_DONE_CNT_POS))
#define GLB_FCLK_SW_STATE                                       GLB_FCLK_SW_STATE
#define GLB_FCLK_SW_STATE_POS                                   (24U)
#define GLB_FCLK_SW_STATE_LEN                                   (3U)
#define GLB_FCLK_SW_STATE_MSK                                   (((1U<<GLB_FCLK_SW_STATE_LEN)-1)<<GLB_FCLK_SW_STATE_POS)
#define GLB_FCLK_SW_STATE_UMSK                                  (~(((1U<<GLB_FCLK_SW_STATE_LEN)-1)<<GLB_FCLK_SW_STATE_POS))
#define GLB_CHIP_RDY                                            GLB_CHIP_RDY
#define GLB_CHIP_RDY_POS                                        (27U)
#define GLB_CHIP_RDY_LEN                                        (1U)
#define GLB_CHIP_RDY_MSK                                        (((1U<<GLB_CHIP_RDY_LEN)-1)<<GLB_CHIP_RDY_POS)
#define GLB_CHIP_RDY_UMSK                                       (~(((1U<<GLB_CHIP_RDY_LEN)-1)<<GLB_CHIP_RDY_POS))
#define GLB_ID                                                  GLB_ID
#define GLB_ID_POS                                              (28U)
#define GLB_ID_LEN                                              (4U)
#define GLB_ID_MSK                                              (((1U<<GLB_ID_LEN)-1)<<GLB_ID_POS)
#define GLB_ID_UMSK                                             (~(((1U<<GLB_ID_LEN)-1)<<GLB_ID_POS))

/* 0x4 : clk_cfg1 */
#define GLB_CLK_CFG1_OFFSET                                     (0x4)
#define GLB_REG_PLL_EN                                          GLB_REG_PLL_EN
#define GLB_REG_PLL_EN_POS                                      (0U)
#define GLB_REG_PLL_EN_LEN                                      (1U)
#define GLB_REG_PLL_EN_MSK                                      (((1U<<GLB_REG_PLL_EN_LEN)-1)<<GLB_REG_PLL_EN_POS)
#define GLB_REG_PLL_EN_UMSK                                     (~(((1U<<GLB_REG_PLL_EN_LEN)-1)<<GLB_REG_PLL_EN_POS))
#define GLB_REG_FCLK_EN                                         GLB_REG_FCLK_EN
#define GLB_REG_FCLK_EN_POS                                     (1U)
#define GLB_REG_FCLK_EN_LEN                                     (1U)
#define GLB_REG_FCLK_EN_MSK                                     (((1U<<GLB_REG_FCLK_EN_LEN)-1)<<GLB_REG_FCLK_EN_POS)
#define GLB_REG_FCLK_EN_UMSK                                    (~(((1U<<GLB_REG_FCLK_EN_LEN)-1)<<GLB_REG_FCLK_EN_POS))
#define GLB_REG_HCLK_DIS                                        GLB_REG_HCLK_DIS
#define GLB_REG_HCLK_DIS_POS                                    (2U)
#define GLB_REG_HCLK_DIS_LEN                                    (1U)
#define GLB_REG_HCLK_DIS_MSK                                    (((1U<<GLB_REG_HCLK_DIS_LEN)-1)<<GLB_REG_HCLK_DIS_POS)
#define GLB_REG_HCLK_DIS_UMSK                                   (~(((1U<<GLB_REG_HCLK_DIS_LEN)-1)<<GLB_REG_HCLK_DIS_POS))
#define GLB_REG_BCLK_DIS                                        GLB_REG_BCLK_DIS
#define GLB_REG_BCLK_DIS_POS                                    (3U)
#define GLB_REG_BCLK_DIS_LEN                                    (1U)
#define GLB_REG_BCLK_DIS_MSK                                    (((1U<<GLB_REG_BCLK_DIS_LEN)-1)<<GLB_REG_BCLK_DIS_POS)
#define GLB_REG_BCLK_DIS_UMSK                                   (~(((1U<<GLB_REG_BCLK_DIS_LEN)-1)<<GLB_REG_BCLK_DIS_POS))
#define GLB_REG_HCLK_DIV                                        GLB_REG_HCLK_DIV
#define GLB_REG_HCLK_DIV_POS                                    (8U)
#define GLB_REG_HCLK_DIV_LEN                                    (8U)
#define GLB_REG_HCLK_DIV_MSK                                    (((1U<<GLB_REG_HCLK_DIV_LEN)-1)<<GLB_REG_HCLK_DIV_POS)
#define GLB_REG_HCLK_DIV_UMSK                                   (~(((1U<<GLB_REG_HCLK_DIV_LEN)-1)<<GLB_REG_HCLK_DIV_POS))
#define GLB_REG_BCLK_DIV                                        GLB_REG_BCLK_DIV
#define GLB_REG_BCLK_DIV_POS                                    (16U)
#define GLB_REG_BCLK_DIV_LEN                                    (8U)
#define GLB_REG_BCLK_DIV_MSK                                    (((1U<<GLB_REG_BCLK_DIV_LEN)-1)<<GLB_REG_BCLK_DIV_POS)
#define GLB_REG_BCLK_DIV_UMSK                                   (~(((1U<<GLB_REG_BCLK_DIV_LEN)-1)<<GLB_REG_BCLK_DIV_POS))

/* 0x8 : clk_cfg2 */
#define GLB_CLK_CFG2_OFFSET                                     (0x8)
#define GLB_CLK_1M_CLK_DIV                                      GLB_CLK_1M_CLK_DIV
#define GLB_CLK_1M_CLK_DIV_POS                                  (0U)
#define GLB_CLK_1M_CLK_DIV_LEN                                  (5U)
#define GLB_CLK_1M_CLK_DIV_MSK                                  (((1U<<GLB_CLK_1M_CLK_DIV_LEN)-1)<<GLB_CLK_1M_CLK_DIV_POS)
#define GLB_CLK_1M_CLK_DIV_UMSK                                 (~(((1U<<GLB_CLK_1M_CLK_DIV_LEN)-1)<<GLB_CLK_1M_CLK_DIV_POS))
#define GLB_CLK_1M_EN                                           GLB_CLK_1M_EN
#define GLB_CLK_1M_EN_POS                                       (5U)
#define GLB_CLK_1M_EN_LEN                                       (1U)
#define GLB_CLK_1M_EN_MSK                                       (((1U<<GLB_CLK_1M_EN_LEN)-1)<<GLB_CLK_1M_EN_POS)
#define GLB_CLK_1M_EN_UMSK                                      (~(((1U<<GLB_CLK_1M_EN_LEN)-1)<<GLB_CLK_1M_EN_POS))
#define GLB_CLK_1M_CLK_SEL                                      GLB_CLK_1M_CLK_SEL
#define GLB_CLK_1M_CLK_SEL_POS                                  (7U)
#define GLB_CLK_1M_CLK_SEL_LEN                                  (1U)
#define GLB_CLK_1M_CLK_SEL_MSK                                  (((1U<<GLB_CLK_1M_CLK_SEL_LEN)-1)<<GLB_CLK_1M_CLK_SEL_POS)
#define GLB_CLK_1M_CLK_SEL_UMSK                                 (~(((1U<<GLB_CLK_1M_CLK_SEL_LEN)-1)<<GLB_CLK_1M_CLK_SEL_POS))
#define GLB_REG_AUDIO_CLK_DIV                                   GLB_REG_AUDIO_CLK_DIV
#define GLB_REG_AUDIO_CLK_DIV_POS                               (8U)
#define GLB_REG_AUDIO_CLK_DIV_LEN                               (6U)
#define GLB_REG_AUDIO_CLK_DIV_MSK                               (((1U<<GLB_REG_AUDIO_CLK_DIV_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_POS)
#define GLB_REG_AUDIO_CLK_DIV_UMSK                              (~(((1U<<GLB_REG_AUDIO_CLK_DIV_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_POS))
#define GLB_REG_I2S_0_REF_CLK_OE                                GLB_REG_I2S_0_REF_CLK_OE
#define GLB_REG_I2S_0_REF_CLK_OE_POS                            (15U)
#define GLB_REG_I2S_0_REF_CLK_OE_LEN                            (1U)
#define GLB_REG_I2S_0_REF_CLK_OE_MSK                            (((1U<<GLB_REG_I2S_0_REF_CLK_OE_LEN)-1)<<GLB_REG_I2S_0_REF_CLK_OE_POS)
#define GLB_REG_I2S_0_REF_CLK_OE_UMSK                           (~(((1U<<GLB_REG_I2S_0_REF_CLK_OE_LEN)-1)<<GLB_REG_I2S_0_REF_CLK_OE_POS))
#define GLB_BLE_CLK_SEL                                         GLB_BLE_CLK_SEL
#define GLB_BLE_CLK_SEL_POS                                     (16U)
#define GLB_BLE_CLK_SEL_LEN                                     (6U)
#define GLB_BLE_CLK_SEL_MSK                                     (((1U<<GLB_BLE_CLK_SEL_LEN)-1)<<GLB_BLE_CLK_SEL_POS)
#define GLB_BLE_CLK_SEL_UMSK                                    (~(((1U<<GLB_BLE_CLK_SEL_LEN)-1)<<GLB_BLE_CLK_SEL_POS))
#define GLB_USB_CLK_EN                                          GLB_USB_CLK_EN
#define GLB_USB_CLK_EN_POS                                      (22U)
#define GLB_USB_CLK_EN_LEN                                      (1U)
#define GLB_USB_CLK_EN_MSK                                      (((1U<<GLB_USB_CLK_EN_LEN)-1)<<GLB_USB_CLK_EN_POS)
#define GLB_USB_CLK_EN_UMSK                                     (~(((1U<<GLB_USB_CLK_EN_LEN)-1)<<GLB_USB_CLK_EN_POS))
#define GLB_PLL_48M_DIV_EN                                      GLB_PLL_48M_DIV_EN
#define GLB_PLL_48M_DIV_EN_POS                                  (23U)
#define GLB_PLL_48M_DIV_EN_LEN                                  (1U)
#define GLB_PLL_48M_DIV_EN_MSK                                  (((1U<<GLB_PLL_48M_DIV_EN_LEN)-1)<<GLB_PLL_48M_DIV_EN_POS)
#define GLB_PLL_48M_DIV_EN_UMSK                                 (~(((1U<<GLB_PLL_48M_DIV_EN_LEN)-1)<<GLB_PLL_48M_DIV_EN_POS))
#define GLB_BLE_EN                                              GLB_BLE_EN
#define GLB_BLE_EN_POS                                          (24U)
#define GLB_BLE_EN_LEN                                          (1U)
#define GLB_BLE_EN_MSK                                          (((1U<<GLB_BLE_EN_LEN)-1)<<GLB_BLE_EN_POS)
#define GLB_BLE_EN_UMSK                                         (~(((1U<<GLB_BLE_EN_LEN)-1)<<GLB_BLE_EN_POS))
#define GLB_M154_ZBEN                                           GLB_M154_ZBEN
#define GLB_M154_ZBEN_POS                                       (25U)
#define GLB_M154_ZBEN_LEN                                       (1U)
#define GLB_M154_ZBEN_MSK                                       (((1U<<GLB_M154_ZBEN_LEN)-1)<<GLB_M154_ZBEN_POS)
#define GLB_M154_ZBEN_UMSK                                      (~(((1U<<GLB_M154_ZBEN_LEN)-1)<<GLB_M154_ZBEN_POS))
#define GLB_REG_AUDIO_CLK_SRC_SEL                               GLB_REG_AUDIO_CLK_SRC_SEL
#define GLB_REG_AUDIO_CLK_SRC_SEL_POS                           (28U)
#define GLB_REG_AUDIO_CLK_SRC_SEL_LEN                           (2U)
#define GLB_REG_AUDIO_CLK_SRC_SEL_MSK                           (((1U<<GLB_REG_AUDIO_CLK_SRC_SEL_LEN)-1)<<GLB_REG_AUDIO_CLK_SRC_SEL_POS)
#define GLB_REG_AUDIO_CLK_SRC_SEL_UMSK                          (~(((1U<<GLB_REG_AUDIO_CLK_SRC_SEL_LEN)-1)<<GLB_REG_AUDIO_CLK_SRC_SEL_POS))
#define GLB_REG_AUDIO_CLK_DIV_EN                                GLB_REG_AUDIO_CLK_DIV_EN
#define GLB_REG_AUDIO_CLK_DIV_EN_POS                            (30U)
#define GLB_REG_AUDIO_CLK_DIV_EN_LEN                            (1U)
#define GLB_REG_AUDIO_CLK_DIV_EN_MSK                            (((1U<<GLB_REG_AUDIO_CLK_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_EN_POS)
#define GLB_REG_AUDIO_CLK_DIV_EN_UMSK                           (~(((1U<<GLB_REG_AUDIO_CLK_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_CLK_DIV_EN_POS))
#define GLB_REG_AUDIO_AUTO_DIV_EN                               GLB_REG_AUDIO_AUTO_DIV_EN
#define GLB_REG_AUDIO_AUTO_DIV_EN_POS                           (31U)
#define GLB_REG_AUDIO_AUTO_DIV_EN_LEN                           (1U)
#define GLB_REG_AUDIO_AUTO_DIV_EN_MSK                           (((1U<<GLB_REG_AUDIO_AUTO_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_AUTO_DIV_EN_POS)
#define GLB_REG_AUDIO_AUTO_DIV_EN_UMSK                          (~(((1U<<GLB_REG_AUDIO_AUTO_DIV_EN_LEN)-1)<<GLB_REG_AUDIO_AUTO_DIV_EN_POS))

/* 0xC : clk_cfg3 */
#define GLB_CLK_CFG3_OFFSET                                     (0xC)
#define GLB_UART_CLK_DIV                                        GLB_UART_CLK_DIV
#define GLB_UART_CLK_DIV_POS                                    (0U)
#define GLB_UART_CLK_DIV_LEN                                    (3U)
#define GLB_UART_CLK_DIV_MSK                                    (((1U<<GLB_UART_CLK_DIV_LEN)-1)<<GLB_UART_CLK_DIV_POS)
#define GLB_UART_CLK_DIV_UMSK                                   (~(((1U<<GLB_UART_CLK_DIV_LEN)-1)<<GLB_UART_CLK_DIV_POS))
#define GLB_UART_CLK_EN                                         GLB_UART_CLK_EN
#define GLB_UART_CLK_EN_POS                                     (4U)
#define GLB_UART_CLK_EN_LEN                                     (1U)
#define GLB_UART_CLK_EN_MSK                                     (((1U<<GLB_UART_CLK_EN_LEN)-1)<<GLB_UART_CLK_EN_POS)
#define GLB_UART_CLK_EN_UMSK                                    (~(((1U<<GLB_UART_CLK_EN_LEN)-1)<<GLB_UART_CLK_EN_POS))
#define GLB_HBN_UART_CLK_SEL                                    GLB_HBN_UART_CLK_SEL
#define GLB_HBN_UART_CLK_SEL_POS                                (7U)
#define GLB_HBN_UART_CLK_SEL_LEN                                (1U)
#define GLB_HBN_UART_CLK_SEL_MSK                                (((1U<<GLB_HBN_UART_CLK_SEL_LEN)-1)<<GLB_HBN_UART_CLK_SEL_POS)
#define GLB_HBN_UART_CLK_SEL_UMSK                               (~(((1U<<GLB_HBN_UART_CLK_SEL_LEN)-1)<<GLB_HBN_UART_CLK_SEL_POS))
#define GLB_SF_CLK_DIV                                          GLB_SF_CLK_DIV
#define GLB_SF_CLK_DIV_POS                                      (8U)
#define GLB_SF_CLK_DIV_LEN                                      (3U)
#define GLB_SF_CLK_DIV_MSK                                      (((1U<<GLB_SF_CLK_DIV_LEN)-1)<<GLB_SF_CLK_DIV_POS)
#define GLB_SF_CLK_DIV_UMSK                                     (~(((1U<<GLB_SF_CLK_DIV_LEN)-1)<<GLB_SF_CLK_DIV_POS))
#define GLB_SF_CLK_EN                                           GLB_SF_CLK_EN
#define GLB_SF_CLK_EN_POS                                       (11U)
#define GLB_SF_CLK_EN_LEN                                       (1U)
#define GLB_SF_CLK_EN_MSK                                       (((1U<<GLB_SF_CLK_EN_LEN)-1)<<GLB_SF_CLK_EN_POS)
#define GLB_SF_CLK_EN_UMSK                                      (~(((1U<<GLB_SF_CLK_EN_LEN)-1)<<GLB_SF_CLK_EN_POS))
#define GLB_SF_CLK_SEL                                          GLB_SF_CLK_SEL
#define GLB_SF_CLK_SEL_POS                                      (12U)
#define GLB_SF_CLK_SEL_LEN                                      (2U)
#define GLB_SF_CLK_SEL_MSK                                      (((1U<<GLB_SF_CLK_SEL_LEN)-1)<<GLB_SF_CLK_SEL_POS)
#define GLB_SF_CLK_SEL_UMSK                                     (~(((1U<<GLB_SF_CLK_SEL_LEN)-1)<<GLB_SF_CLK_SEL_POS))
#define GLB_CFG_INV_RF_TEST_CLK_O                               GLB_CFG_INV_RF_TEST_CLK_O
#define GLB_CFG_INV_RF_TEST_CLK_O_POS                           (14U)
#define GLB_CFG_INV_RF_TEST_CLK_O_LEN                           (1U)
#define GLB_CFG_INV_RF_TEST_CLK_O_MSK                           (((1U<<GLB_CFG_INV_RF_TEST_CLK_O_LEN)-1)<<GLB_CFG_INV_RF_TEST_CLK_O_POS)
#define GLB_CFG_INV_RF_TEST_CLK_O_UMSK                          (~(((1U<<GLB_CFG_INV_RF_TEST_CLK_O_LEN)-1)<<GLB_CFG_INV_RF_TEST_CLK_O_POS))
#define GLB_IR_CLK_DIV                                          GLB_IR_CLK_DIV
#define GLB_IR_CLK_DIV_POS                                      (16U)
#define GLB_IR_CLK_DIV_LEN                                      (6U)
#define GLB_IR_CLK_DIV_MSK                                      (((1U<<GLB_IR_CLK_DIV_LEN)-1)<<GLB_IR_CLK_DIV_POS)
#define GLB_IR_CLK_DIV_UMSK                                     (~(((1U<<GLB_IR_CLK_DIV_LEN)-1)<<GLB_IR_CLK_DIV_POS))
#define GLB_HBN_UART_CLK_SEL2                                   GLB_HBN_UART_CLK_SEL2
#define GLB_HBN_UART_CLK_SEL2_POS                               (22U)
#define GLB_HBN_UART_CLK_SEL2_LEN                               (1U)
#define GLB_HBN_UART_CLK_SEL2_MSK                               (((1U<<GLB_HBN_UART_CLK_SEL2_LEN)-1)<<GLB_HBN_UART_CLK_SEL2_POS)
#define GLB_HBN_UART_CLK_SEL2_UMSK                              (~(((1U<<GLB_HBN_UART_CLK_SEL2_LEN)-1)<<GLB_HBN_UART_CLK_SEL2_POS))
#define GLB_IR_CLK_EN                                           GLB_IR_CLK_EN
#define GLB_IR_CLK_EN_POS                                       (23U)
#define GLB_IR_CLK_EN_LEN                                       (1U)
#define GLB_IR_CLK_EN_MSK                                       (((1U<<GLB_IR_CLK_EN_LEN)-1)<<GLB_IR_CLK_EN_POS)
#define GLB_IR_CLK_EN_UMSK                                      (~(((1U<<GLB_IR_CLK_EN_LEN)-1)<<GLB_IR_CLK_EN_POS))
#define GLB_DMA_CLK_EN                                          GLB_DMA_CLK_EN
#define GLB_DMA_CLK_EN_POS                                      (24U)
#define GLB_DMA_CLK_EN_LEN                                      (8U)
#define GLB_DMA_CLK_EN_MSK                                      (((1U<<GLB_DMA_CLK_EN_LEN)-1)<<GLB_DMA_CLK_EN_POS)
#define GLB_DMA_CLK_EN_UMSK                                     (~(((1U<<GLB_DMA_CLK_EN_LEN)-1)<<GLB_DMA_CLK_EN_POS))

/* 0x10 : clk_cfg4 */
#define GLB_CLK_CFG4_OFFSET                                     (0x10)
#define GLB_SPI_CLK_DIV                                         GLB_SPI_CLK_DIV
#define GLB_SPI_CLK_DIV_POS                                     (0U)
#define GLB_SPI_CLK_DIV_LEN                                     (5U)
#define GLB_SPI_CLK_DIV_MSK                                     (((1U<<GLB_SPI_CLK_DIV_LEN)-1)<<GLB_SPI_CLK_DIV_POS)
#define GLB_SPI_CLK_DIV_UMSK                                    (~(((1U<<GLB_SPI_CLK_DIV_LEN)-1)<<GLB_SPI_CLK_DIV_POS))
#define GLB_SPI_CLK_EN                                          GLB_SPI_CLK_EN
#define GLB_SPI_CLK_EN_POS                                      (8U)
#define GLB_SPI_CLK_EN_LEN                                      (1U)
#define GLB_SPI_CLK_EN_MSK                                      (((1U<<GLB_SPI_CLK_EN_LEN)-1)<<GLB_SPI_CLK_EN_POS)
#define GLB_SPI_CLK_EN_UMSK                                     (~(((1U<<GLB_SPI_CLK_EN_LEN)-1)<<GLB_SPI_CLK_EN_POS))
#define GLB_SPI_CLK_SEL                                         GLB_SPI_CLK_SEL
#define GLB_SPI_CLK_SEL_POS                                     (9U)
#define GLB_SPI_CLK_SEL_LEN                                     (1U)
#define GLB_SPI_CLK_SEL_MSK                                     (((1U<<GLB_SPI_CLK_SEL_LEN)-1)<<GLB_SPI_CLK_SEL_POS)
#define GLB_SPI_CLK_SEL_UMSK                                    (~(((1U<<GLB_SPI_CLK_SEL_LEN)-1)<<GLB_SPI_CLK_SEL_POS))
#define GLB_CHIP_CLK_OUT_EN                                     GLB_CHIP_CLK_OUT_EN
#define GLB_CHIP_CLK_OUT_EN_POS                                 (10U)
#define GLB_CHIP_CLK_OUT_EN_LEN                                 (4U)
#define GLB_CHIP_CLK_OUT_EN_MSK                                 (((1U<<GLB_CHIP_CLK_OUT_EN_LEN)-1)<<GLB_CHIP_CLK_OUT_EN_POS)
#define GLB_CHIP_CLK_OUT_EN_UMSK                                (~(((1U<<GLB_CHIP_CLK_OUT_EN_LEN)-1)<<GLB_CHIP_CLK_OUT_EN_POS))
#define GLB_GPIO_TMR_CLK_SEL                                    GLB_GPIO_TMR_CLK_SEL
#define GLB_GPIO_TMR_CLK_SEL_POS                                (14U)
#define GLB_GPIO_TMR_CLK_SEL_LEN                                (2U)
#define GLB_GPIO_TMR_CLK_SEL_MSK                                (((1U<<GLB_GPIO_TMR_CLK_SEL_LEN)-1)<<GLB_GPIO_TMR_CLK_SEL_POS)
#define GLB_GPIO_TMR_CLK_SEL_UMSK                               (~(((1U<<GLB_GPIO_TMR_CLK_SEL_LEN)-1)<<GLB_GPIO_TMR_CLK_SEL_POS))
#define GLB_I2C_CLK_DIV                                         GLB_I2C_CLK_DIV
#define GLB_I2C_CLK_DIV_POS                                     (16U)
#define GLB_I2C_CLK_DIV_LEN                                     (8U)
#define GLB_I2C_CLK_DIV_MSK                                     (((1U<<GLB_I2C_CLK_DIV_LEN)-1)<<GLB_I2C_CLK_DIV_POS)
#define GLB_I2C_CLK_DIV_UMSK                                    (~(((1U<<GLB_I2C_CLK_DIV_LEN)-1)<<GLB_I2C_CLK_DIV_POS))
#define GLB_I2C_CLK_EN                                          GLB_I2C_CLK_EN
#define GLB_I2C_CLK_EN_POS                                      (24U)
#define GLB_I2C_CLK_EN_LEN                                      (1U)
#define GLB_I2C_CLK_EN_MSK                                      (((1U<<GLB_I2C_CLK_EN_LEN)-1)<<GLB_I2C_CLK_EN_POS)
#define GLB_I2C_CLK_EN_UMSK                                     (~(((1U<<GLB_I2C_CLK_EN_LEN)-1)<<GLB_I2C_CLK_EN_POS))
#define GLB_I2C_CLK_SEL                                         GLB_I2C_CLK_SEL
#define GLB_I2C_CLK_SEL_POS                                     (25U)
#define GLB_I2C_CLK_SEL_LEN                                     (1U)
#define GLB_I2C_CLK_SEL_MSK                                     (((1U<<GLB_I2C_CLK_SEL_LEN)-1)<<GLB_I2C_CLK_SEL_POS)
#define GLB_I2C_CLK_SEL_UMSK                                    (~(((1U<<GLB_I2C_CLK_SEL_LEN)-1)<<GLB_I2C_CLK_SEL_POS))
#define GLB_CHIP_CLK_OUT_2_SEL                                  GLB_CHIP_CLK_OUT_2_SEL
#define GLB_CHIP_CLK_OUT_2_SEL_POS                              (26U)
#define GLB_CHIP_CLK_OUT_2_SEL_LEN                              (2U)
#define GLB_CHIP_CLK_OUT_2_SEL_MSK                              (((1U<<GLB_CHIP_CLK_OUT_2_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_2_SEL_POS)
#define GLB_CHIP_CLK_OUT_2_SEL_UMSK                             (~(((1U<<GLB_CHIP_CLK_OUT_2_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_2_SEL_POS))
#define GLB_CHIP_CLK_OUT_0_SEL                                  GLB_CHIP_CLK_OUT_0_SEL
#define GLB_CHIP_CLK_OUT_0_SEL_POS                              (28U)
#define GLB_CHIP_CLK_OUT_0_SEL_LEN                              (2U)
#define GLB_CHIP_CLK_OUT_0_SEL_MSK                              (((1U<<GLB_CHIP_CLK_OUT_0_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_0_SEL_POS)
#define GLB_CHIP_CLK_OUT_0_SEL_UMSK                             (~(((1U<<GLB_CHIP_CLK_OUT_0_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_0_SEL_POS))
#define GLB_CHIP_CLK_OUT_1_SEL                                  GLB_CHIP_CLK_OUT_1_SEL
#define GLB_CHIP_CLK_OUT_1_SEL_POS                              (30U)
#define GLB_CHIP_CLK_OUT_1_SEL_LEN                              (2U)
#define GLB_CHIP_CLK_OUT_1_SEL_MSK                              (((1U<<GLB_CHIP_CLK_OUT_1_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_1_SEL_POS)
#define GLB_CHIP_CLK_OUT_1_SEL_UMSK                             (~(((1U<<GLB_CHIP_CLK_OUT_1_SEL_LEN)-1)<<GLB_CHIP_CLK_OUT_1_SEL_POS))

/* 0x14 : clk_cfg5 */
#define GLB_CLK_CFG5_OFFSET                                     (0x14)
#define GLB_CFG_INV_ETH_TX_CLK                                  GLB_CFG_INV_ETH_TX_CLK
#define GLB_CFG_INV_ETH_TX_CLK_POS                              (0U)
#define GLB_CFG_INV_ETH_TX_CLK_LEN                              (1U)
#define GLB_CFG_INV_ETH_TX_CLK_MSK                              (((1U<<GLB_CFG_INV_ETH_TX_CLK_LEN)-1)<<GLB_CFG_INV_ETH_TX_CLK_POS)
#define GLB_CFG_INV_ETH_TX_CLK_UMSK                             (~(((1U<<GLB_CFG_INV_ETH_TX_CLK_LEN)-1)<<GLB_CFG_INV_ETH_TX_CLK_POS))
#define GLB_CFG_INV_ETH_RX_CLK                                  GLB_CFG_INV_ETH_RX_CLK
#define GLB_CFG_INV_ETH_RX_CLK_POS                              (1U)
#define GLB_CFG_INV_ETH_RX_CLK_LEN                              (1U)
#define GLB_CFG_INV_ETH_RX_CLK_MSK                              (((1U<<GLB_CFG_INV_ETH_RX_CLK_LEN)-1)<<GLB_CFG_INV_ETH_RX_CLK_POS)
#define GLB_CFG_INV_ETH_RX_CLK_UMSK                             (~(((1U<<GLB_CFG_INV_ETH_RX_CLK_LEN)-1)<<GLB_CFG_INV_ETH_RX_CLK_POS))
#define GLB_CFG_INV_ETH_REF_CLK_I                               GLB_CFG_INV_ETH_REF_CLK_I
#define GLB_CFG_INV_ETH_REF_CLK_I_POS                           (2U)
#define GLB_CFG_INV_ETH_REF_CLK_I_LEN                           (1U)
#define GLB_CFG_INV_ETH_REF_CLK_I_MSK                           (((1U<<GLB_CFG_INV_ETH_REF_CLK_I_LEN)-1)<<GLB_CFG_INV_ETH_REF_CLK_I_POS)
#define GLB_CFG_INV_ETH_REF_CLK_I_UMSK                          (~(((1U<<GLB_CFG_INV_ETH_REF_CLK_I_LEN)-1)<<GLB_CFG_INV_ETH_REF_CLK_I_POS))
#define GLB_CFG_INV_ETH_REF_CLK_O                               GLB_CFG_INV_ETH_REF_CLK_O
#define GLB_CFG_INV_ETH_REF_CLK_O_POS                           (3U)
#define GLB_CFG_INV_ETH_REF_CLK_O_LEN                           (1U)
#define GLB_CFG_INV_ETH_REF_CLK_O_MSK                           (((1U<<GLB_CFG_INV_ETH_REF_CLK_O_LEN)-1)<<GLB_CFG_INV_ETH_REF_CLK_O_POS)
#define GLB_CFG_INV_ETH_REF_CLK_O_UMSK                          (~(((1U<<GLB_CFG_INV_ETH_REF_CLK_O_LEN)-1)<<GLB_CFG_INV_ETH_REF_CLK_O_POS))
#define GLB_CFG_SEL_ETH_REF_CLK_O                               GLB_CFG_SEL_ETH_REF_CLK_O
#define GLB_CFG_SEL_ETH_REF_CLK_O_POS                           (4U)
#define GLB_CFG_SEL_ETH_REF_CLK_O_LEN                           (1U)
#define GLB_CFG_SEL_ETH_REF_CLK_O_MSK                           (((1U<<GLB_CFG_SEL_ETH_REF_CLK_O_LEN)-1)<<GLB_CFG_SEL_ETH_REF_CLK_O_POS)
#define GLB_CFG_SEL_ETH_REF_CLK_O_UMSK                          (~(((1U<<GLB_CFG_SEL_ETH_REF_CLK_O_LEN)-1)<<GLB_CFG_SEL_ETH_REF_CLK_O_POS))
#define GLB_REG_CAM_REF_CLK_DIV                                 GLB_REG_CAM_REF_CLK_DIV
#define GLB_REG_CAM_REF_CLK_DIV_POS                             (5U)
#define GLB_REG_CAM_REF_CLK_DIV_LEN                             (2U)
#define GLB_REG_CAM_REF_CLK_DIV_MSK                             (((1U<<GLB_REG_CAM_REF_CLK_DIV_LEN)-1)<<GLB_REG_CAM_REF_CLK_DIV_POS)
#define GLB_REG_CAM_REF_CLK_DIV_UMSK                            (~(((1U<<GLB_REG_CAM_REF_CLK_DIV_LEN)-1)<<GLB_REG_CAM_REF_CLK_DIV_POS))
#define GLB_REG_CAM_REF_CLK_DIV_EN                              GLB_REG_CAM_REF_CLK_DIV_EN
#define GLB_REG_CAM_REF_CLK_DIV_EN_POS                          (7U)
#define GLB_REG_CAM_REF_CLK_DIV_EN_LEN                          (1U)
#define GLB_REG_CAM_REF_CLK_DIV_EN_MSK                          (((1U<<GLB_REG_CAM_REF_CLK_DIV_EN_LEN)-1)<<GLB_REG_CAM_REF_CLK_DIV_EN_POS)
#define GLB_REG_CAM_REF_CLK_DIV_EN_UMSK                         (~(((1U<<GLB_REG_CAM_REF_CLK_DIV_EN_LEN)-1)<<GLB_REG_CAM_REF_CLK_DIV_EN_POS))
#define GLB_REG_CAM_REF_CLK_SEL                                 GLB_REG_CAM_REF_CLK_SEL
#define GLB_REG_CAM_REF_CLK_SEL_POS                             (8U)
#define GLB_REG_CAM_REF_CLK_SEL_LEN                             (2U)
#define GLB_REG_CAM_REF_CLK_SEL_MSK                             (((1U<<GLB_REG_CAM_REF_CLK_SEL_LEN)-1)<<GLB_REG_CAM_REF_CLK_SEL_POS)
#define GLB_REG_CAM_REF_CLK_SEL_UMSK                            (~(((1U<<GLB_REG_CAM_REF_CLK_SEL_LEN)-1)<<GLB_REG_CAM_REF_CLK_SEL_POS))
#define GLB_REG_IPROAI_CLK_SEL                                    GLB_REG_IPROAI_CLK_SEL
#define GLB_REG_IPROAI_CLK_SEL_POS                                (10U)
#define GLB_REG_IPROAI_CLK_SEL_LEN                                (2U)
#define GLB_REG_IPROAI_CLK_SEL_MSK                                (((1U<<GLB_REG_IPROAI_CLK_SEL_LEN)-1)<<GLB_REG_IPROAI_CLK_SEL_POS)
#define GLB_REG_IPROAI_CLK_SEL_UMSK                               (~(((1U<<GLB_REG_IPROAI_CLK_SEL_LEN)-1)<<GLB_REG_IPROAI_CLK_SEL_POS))
#define GLB_REG_IPROAI_CLK_DIV_EN                                 GLB_REG_IPROAI_CLK_DIV_EN
#define GLB_REG_IPROAI_CLK_DIV_EN_POS                             (12U)
#define GLB_REG_IPROAI_CLK_DIV_EN_LEN                             (1U)
#define GLB_REG_IPROAI_CLK_DIV_EN_MSK                             (((1U<<GLB_REG_IPROAI_CLK_DIV_EN_LEN)-1)<<GLB_REG_IPROAI_CLK_DIV_EN_POS)
#define GLB_REG_IPROAI_CLK_DIV_EN_UMSK                            (~(((1U<<GLB_REG_IPROAI_CLK_DIV_EN_LEN)-1)<<GLB_REG_IPROAI_CLK_DIV_EN_POS))
#define GLB_REG_IPROAI_CLK_DIV                                    GLB_REG_IPROAI_CLK_DIV
#define GLB_REG_IPROAI_CLK_DIV_POS                                (13U)
#define GLB_REG_IPROAI_CLK_DIV_LEN                                (3U)
#define GLB_REG_IPROAI_CLK_DIV_MSK                                (((1U<<GLB_REG_IPROAI_CLK_DIV_LEN)-1)<<GLB_REG_IPROAI_CLK_DIV_POS)
#define GLB_REG_IPROAI_CLK_DIV_UMSK                               (~(((1U<<GLB_REG_IPROAI_CLK_DIV_LEN)-1)<<GLB_REG_IPROAI_CLK_DIV_POS))
#define GLB_SPI2_CLK_DIV                                        GLB_SPI2_CLK_DIV
#define GLB_SPI2_CLK_DIV_POS                                    (16U)
#define GLB_SPI2_CLK_DIV_LEN                                    (5U)
#define GLB_SPI2_CLK_DIV_MSK                                    (((1U<<GLB_SPI2_CLK_DIV_LEN)-1)<<GLB_SPI2_CLK_DIV_POS)
#define GLB_SPI2_CLK_DIV_UMSK                                   (~(((1U<<GLB_SPI2_CLK_DIV_LEN)-1)<<GLB_SPI2_CLK_DIV_POS))
#define GLB_SPI2_CLK_EN                                         GLB_SPI2_CLK_EN
#define GLB_SPI2_CLK_EN_POS                                     (21U)
#define GLB_SPI2_CLK_EN_LEN                                     (1U)
#define GLB_SPI2_CLK_EN_MSK                                     (((1U<<GLB_SPI2_CLK_EN_LEN)-1)<<GLB_SPI2_CLK_EN_POS)
#define GLB_SPI2_CLK_EN_UMSK                                    (~(((1U<<GLB_SPI2_CLK_EN_LEN)-1)<<GLB_SPI2_CLK_EN_POS))
#define GLB_SPI2_CLK_SEL                                        GLB_SPI2_CLK_SEL
#define GLB_SPI2_CLK_SEL_POS                                    (22U)
#define GLB_SPI2_CLK_SEL_LEN                                    (1U)
#define GLB_SPI2_CLK_SEL_MSK                                    (((1U<<GLB_SPI2_CLK_SEL_LEN)-1)<<GLB_SPI2_CLK_SEL_POS)
#define GLB_SPI2_CLK_SEL_UMSK                                   (~(((1U<<GLB_SPI2_CLK_SEL_LEN)-1)<<GLB_SPI2_CLK_SEL_POS))
#define GLB_REG_PSRAM_CLK_SEL                                   GLB_REG_PSRAM_CLK_SEL
#define GLB_REG_PSRAM_CLK_SEL_POS                               (26U)
#define GLB_REG_PSRAM_CLK_SEL_LEN                               (2U)
#define GLB_REG_PSRAM_CLK_SEL_MSK                               (((1U<<GLB_REG_PSRAM_CLK_SEL_LEN)-1)<<GLB_REG_PSRAM_CLK_SEL_POS)
#define GLB_REG_PSRAM_CLK_SEL_UMSK                              (~(((1U<<GLB_REG_PSRAM_CLK_SEL_LEN)-1)<<GLB_REG_PSRAM_CLK_SEL_POS))
#define GLB_REG_PSRAM_CLK_DIV_EN                                GLB_REG_PSRAM_CLK_DIV_EN
#define GLB_REG_PSRAM_CLK_DIV_EN_POS                            (28U)
#define GLB_REG_PSRAM_CLK_DIV_EN_LEN                            (1U)
#define GLB_REG_PSRAM_CLK_DIV_EN_MSK                            (((1U<<GLB_REG_PSRAM_CLK_DIV_EN_LEN)-1)<<GLB_REG_PSRAM_CLK_DIV_EN_POS)
#define GLB_REG_PSRAM_CLK_DIV_EN_UMSK                           (~(((1U<<GLB_REG_PSRAM_CLK_DIV_EN_LEN)-1)<<GLB_REG_PSRAM_CLK_DIV_EN_POS))
#define GLB_REG_PSRAM_CLK_DIV                                   GLB_REG_PSRAM_CLK_DIV
#define GLB_REG_PSRAM_CLK_DIV_POS                               (29U)
#define GLB_REG_PSRAM_CLK_DIV_LEN                               (3U)
#define GLB_REG_PSRAM_CLK_DIV_MSK                               (((1U<<GLB_REG_PSRAM_CLK_DIV_LEN)-1)<<GLB_REG_PSRAM_CLK_DIV_POS)
#define GLB_REG_PSRAM_CLK_DIV_UMSK                              (~(((1U<<GLB_REG_PSRAM_CLK_DIV_LEN)-1)<<GLB_REG_PSRAM_CLK_DIV_POS))

/* 0x18 : clk_cfg6 */
#define GLB_CLK_CFG6_OFFSET                                     (0x18)
#define GLB_I2S_REF_CLK_DIV                                     GLB_I2S_REF_CLK_DIV
#define GLB_I2S_REF_CLK_DIV_POS                                 (0U)
#define GLB_I2S_REF_CLK_DIV_LEN                                 (6U)
#define GLB_I2S_REF_CLK_DIV_MSK                                 (((1U<<GLB_I2S_REF_CLK_DIV_LEN)-1)<<GLB_I2S_REF_CLK_DIV_POS)
#define GLB_I2S_REF_CLK_DIV_UMSK                                (~(((1U<<GLB_I2S_REF_CLK_DIV_LEN)-1)<<GLB_I2S_REF_CLK_DIV_POS))
#define GLB_I2S_REF_CLK_EN                                      GLB_I2S_REF_CLK_EN
#define GLB_I2S_REF_CLK_EN_POS                                  (7U)
#define GLB_I2S_REF_CLK_EN_LEN                                  (1U)
#define GLB_I2S_REF_CLK_EN_MSK                                  (((1U<<GLB_I2S_REF_CLK_EN_LEN)-1)<<GLB_I2S_REF_CLK_EN_POS)
#define GLB_I2S_REF_CLK_EN_UMSK                                 (~(((1U<<GLB_I2S_REF_CLK_EN_LEN)-1)<<GLB_I2S_REF_CLK_EN_POS))
#define GLB_PIO_CLK_DIV                                         GLB_PIO_CLK_DIV
#define GLB_PIO_CLK_DIV_POS                                     (8U)
#define GLB_PIO_CLK_DIV_LEN                                     (5U)
#define GLB_PIO_CLK_DIV_MSK                                     (((1U<<GLB_PIO_CLK_DIV_LEN)-1)<<GLB_PIO_CLK_DIV_POS)
#define GLB_PIO_CLK_DIV_UMSK                                    (~(((1U<<GLB_PIO_CLK_DIV_LEN)-1)<<GLB_PIO_CLK_DIV_POS))
#define GLB_PIO_CLK_EN                                          GLB_PIO_CLK_EN
#define GLB_PIO_CLK_EN_POS                                      (14U)
#define GLB_PIO_CLK_EN_LEN                                      (1U)
#define GLB_PIO_CLK_EN_MSK                                      (((1U<<GLB_PIO_CLK_EN_LEN)-1)<<GLB_PIO_CLK_EN_POS)
#define GLB_PIO_CLK_EN_UMSK                                     (~(((1U<<GLB_PIO_CLK_EN_LEN)-1)<<GLB_PIO_CLK_EN_POS))
#define GLB_PIO_CLK_SEL                                         GLB_PIO_CLK_SEL
#define GLB_PIO_CLK_SEL_POS                                     (15U)
#define GLB_PIO_CLK_SEL_LEN                                     (1U)
#define GLB_PIO_CLK_SEL_MSK                                     (((1U<<GLB_PIO_CLK_SEL_LEN)-1)<<GLB_PIO_CLK_SEL_POS)
#define GLB_PIO_CLK_SEL_UMSK                                    (~(((1U<<GLB_PIO_CLK_SEL_LEN)-1)<<GLB_PIO_CLK_SEL_POS))
#define GLB_I2S_REF_CLK_SEL                                     GLB_I2S_REF_CLK_SEL
#define GLB_I2S_REF_CLK_SEL_POS                                 (16U)
#define GLB_I2S_REF_CLK_SEL_LEN                                 (2U)
#define GLB_I2S_REF_CLK_SEL_MSK                                 (((1U<<GLB_I2S_REF_CLK_SEL_LEN)-1)<<GLB_I2S_REF_CLK_SEL_POS)
#define GLB_I2S_REF_CLK_SEL_UMSK                                (~(((1U<<GLB_I2S_REF_CLK_SEL_LEN)-1)<<GLB_I2S_REF_CLK_SEL_POS))

/* 0x20 : swrst_cfg0 */
#define GLB_SWRST_CFG0_OFFSET                                   (0x20)
#define GLB_SWRST_S300                                          GLB_SWRST_S300
#define GLB_SWRST_S300_POS                                      (4U)
#define GLB_SWRST_S300_LEN                                      (1U)
#define GLB_SWRST_S300_MSK                                      (((1U<<GLB_SWRST_S300_LEN)-1)<<GLB_SWRST_S300_POS)
#define GLB_SWRST_S300_UMSK                                     (~(((1U<<GLB_SWRST_S300_LEN)-1)<<GLB_SWRST_S300_POS))
#define GLB_SWRST_S301                                          GLB_SWRST_S301
#define GLB_SWRST_S301_POS                                      (8U)
#define GLB_SWRST_S301_LEN                                      (1U)
#define GLB_SWRST_S301_MSK                                      (((1U<<GLB_SWRST_S301_LEN)-1)<<GLB_SWRST_S301_POS)
#define GLB_SWRST_S301_UMSK                                     (~(((1U<<GLB_SWRST_S301_LEN)-1)<<GLB_SWRST_S301_POS))
#define GLB_SWRST_I2C_1                                         GLB_SWRST_I2C_1
#define GLB_SWRST_I2C_1_POS                                     (16U)
#define GLB_SWRST_I2C_1_LEN                                     (1U)
#define GLB_SWRST_I2C_1_MSK                                     (((1U<<GLB_SWRST_I2C_1_LEN)-1)<<GLB_SWRST_I2C_1_POS)
#define GLB_SWRST_I2C_1_UMSK                                    (~(((1U<<GLB_SWRST_I2C_1_LEN)-1)<<GLB_SWRST_I2C_1_POS))

/* 0x24 : swrst_cfg1 */
#define GLB_SWRST_CFG1_OFFSET                                   (0x24)
#define GLB_SWRST_GLB                                           GLB_SWRST_GLB
#define GLB_SWRST_GLB_POS                                       (0U)
#define GLB_SWRST_GLB_LEN                                       (1U)
#define GLB_SWRST_GLB_MSK                                       (((1U<<GLB_SWRST_GLB_LEN)-1)<<GLB_SWRST_GLB_POS)
#define GLB_SWRST_GLB_UMSK                                      (~(((1U<<GLB_SWRST_GLB_LEN)-1)<<GLB_SWRST_GLB_POS))
#define GLB_SWRST_MIX                                           GLB_SWRST_MIX
#define GLB_SWRST_MIX_POS                                       (1U)
#define GLB_SWRST_MIX_LEN                                       (1U)
#define GLB_SWRST_MIX_MSK                                       (((1U<<GLB_SWRST_MIX_LEN)-1)<<GLB_SWRST_MIX_POS)
#define GLB_SWRST_MIX_UMSK                                      (~(((1U<<GLB_SWRST_MIX_LEN)-1)<<GLB_SWRST_MIX_POS))
#define GLB_SWRST_GPIP                                          GLB_SWRST_GPIP
#define GLB_SWRST_GPIP_POS                                      (2U)
#define GLB_SWRST_GPIP_LEN                                      (1U)
#define GLB_SWRST_GPIP_MSK                                      (((1U<<GLB_SWRST_GPIP_LEN)-1)<<GLB_SWRST_GPIP_POS)
#define GLB_SWRST_GPIP_UMSK                                     (~(((1U<<GLB_SWRST_GPIP_LEN)-1)<<GLB_SWRST_GPIP_POS))
#define GLB_SWRST_SEC_DBG                                       GLB_SWRST_SEC_DBG
#define GLB_SWRST_SEC_DBG_POS                                   (3U)
#define GLB_SWRST_SEC_DBG_LEN                                   (1U)
#define GLB_SWRST_SEC_DBG_MSK                                   (((1U<<GLB_SWRST_SEC_DBG_LEN)-1)<<GLB_SWRST_SEC_DBG_POS)
#define GLB_SWRST_SEC_DBG_UMSK                                  (~(((1U<<GLB_SWRST_SEC_DBG_LEN)-1)<<GLB_SWRST_SEC_DBG_POS))
#define GLB_SWRST_SEC_ENG                                       GLB_SWRST_SEC_ENG
#define GLB_SWRST_SEC_ENG_POS                                   (4U)
#define GLB_SWRST_SEC_ENG_LEN                                   (1U)
#define GLB_SWRST_SEC_ENG_MSK                                   (((1U<<GLB_SWRST_SEC_ENG_LEN)-1)<<GLB_SWRST_SEC_ENG_POS)
#define GLB_SWRST_SEC_ENG_UMSK                                  (~(((1U<<GLB_SWRST_SEC_ENG_LEN)-1)<<GLB_SWRST_SEC_ENG_POS))
#define GLB_SWRST_TZ1                                           GLB_SWRST_TZ1
#define GLB_SWRST_TZ1_POS                                       (5U)
#define GLB_SWRST_TZ1_LEN                                       (1U)
#define GLB_SWRST_TZ1_MSK                                       (((1U<<GLB_SWRST_TZ1_LEN)-1)<<GLB_SWRST_TZ1_POS)
#define GLB_SWRST_TZ1_UMSK                                      (~(((1U<<GLB_SWRST_TZ1_LEN)-1)<<GLB_SWRST_TZ1_POS))
#define GLB_SWRST_TZ2                                           GLB_SWRST_TZ2
#define GLB_SWRST_TZ2_POS                                       (6U)
#define GLB_SWRST_TZ2_LEN                                       (1U)
#define GLB_SWRST_TZ2_MSK                                       (((1U<<GLB_SWRST_TZ2_LEN)-1)<<GLB_SWRST_TZ2_POS)
#define GLB_SWRST_TZ2_UMSK                                      (~(((1U<<GLB_SWRST_TZ2_LEN)-1)<<GLB_SWRST_TZ2_POS))
#define GLB_SWRST_EFUSE                                         GLB_SWRST_EFUSE
#define GLB_SWRST_EFUSE_POS                                     (7U)
#define GLB_SWRST_EFUSE_LEN                                     (1U)
#define GLB_SWRST_EFUSE_MSK                                     (((1U<<GLB_SWRST_EFUSE_LEN)-1)<<GLB_SWRST_EFUSE_POS)
#define GLB_SWRST_EFUSE_UMSK                                    (~(((1U<<GLB_SWRST_EFUSE_LEN)-1)<<GLB_SWRST_EFUSE_POS))
#define GLB_SWRST_CCI                                           GLB_SWRST_CCI
#define GLB_SWRST_CCI_POS                                       (8U)
#define GLB_SWRST_CCI_LEN                                       (1U)
#define GLB_SWRST_CCI_MSK                                       (((1U<<GLB_SWRST_CCI_LEN)-1)<<GLB_SWRST_CCI_POS)
#define GLB_SWRST_CCI_UMSK                                      (~(((1U<<GLB_SWRST_CCI_LEN)-1)<<GLB_SWRST_CCI_POS))
#define GLB_SWRST_L1C                                           GLB_SWRST_L1C
#define GLB_SWRST_L1C_POS                                       (9U)
#define GLB_SWRST_L1C_LEN                                       (1U)
#define GLB_SWRST_L1C_MSK                                       (((1U<<GLB_SWRST_L1C_LEN)-1)<<GLB_SWRST_L1C_POS)
#define GLB_SWRST_L1C_UMSK                                      (~(((1U<<GLB_SWRST_L1C_LEN)-1)<<GLB_SWRST_L1C_POS))
#define GLB_SWRST_SF                                            GLB_SWRST_SF
#define GLB_SWRST_SF_POS                                        (10U)
#define GLB_SWRST_SF_LEN                                        (1U)
#define GLB_SWRST_SF_MSK                                        (((1U<<GLB_SWRST_SF_LEN)-1)<<GLB_SWRST_SF_POS)
#define GLB_SWRST_SF_UMSK                                       (~(((1U<<GLB_SWRST_SF_LEN)-1)<<GLB_SWRST_SF_POS))
#define GLB_SWRST_DMA                                           GLB_SWRST_DMA
#define GLB_SWRST_DMA_POS                                       (11U)
#define GLB_SWRST_DMA_LEN                                       (1U)
#define GLB_SWRST_DMA_MSK                                       (((1U<<GLB_SWRST_DMA_LEN)-1)<<GLB_SWRST_DMA_POS)
#define GLB_SWRST_DMA_UMSK                                      (~(((1U<<GLB_SWRST_DMA_LEN)-1)<<GLB_SWRST_DMA_POS))
#define GLB_SWRST_PDS                                           GLB_SWRST_PDS
#define GLB_SWRST_PDS_POS                                       (12U)
#define GLB_SWRST_PDS_LEN                                       (1U)
#define GLB_SWRST_PDS_MSK                                       (((1U<<GLB_SWRST_PDS_LEN)-1)<<GLB_SWRST_PDS_POS)
#define GLB_SWRST_PDS_UMSK                                      (~(((1U<<GLB_SWRST_PDS_LEN)-1)<<GLB_SWRST_PDS_POS))
#define GLB_SWRST_UART0                                         GLB_SWRST_UART0
#define GLB_SWRST_UART0_POS                                     (13U)
#define GLB_SWRST_UART0_LEN                                     (1U)
#define GLB_SWRST_UART0_MSK                                     (((1U<<GLB_SWRST_UART0_LEN)-1)<<GLB_SWRST_UART0_POS)
#define GLB_SWRST_UART0_UMSK                                    (~(((1U<<GLB_SWRST_UART0_LEN)-1)<<GLB_SWRST_UART0_POS))
#define GLB_SWRST_SPI                                           GLB_SWRST_SPI
#define GLB_SWRST_SPI_POS                                       (14U)
#define GLB_SWRST_SPI_LEN                                       (1U)
#define GLB_SWRST_SPI_MSK                                       (((1U<<GLB_SWRST_SPI_LEN)-1)<<GLB_SWRST_SPI_POS)
#define GLB_SWRST_SPI_UMSK                                      (~(((1U<<GLB_SWRST_SPI_LEN)-1)<<GLB_SWRST_SPI_POS))
#define GLB_SWRST_I2C                                           GLB_SWRST_I2C
#define GLB_SWRST_I2C_POS                                       (15U)
#define GLB_SWRST_I2C_LEN                                       (1U)
#define GLB_SWRST_I2C_MSK                                       (((1U<<GLB_SWRST_I2C_LEN)-1)<<GLB_SWRST_I2C_POS)
#define GLB_SWRST_I2C_UMSK                                      (~(((1U<<GLB_SWRST_I2C_LEN)-1)<<GLB_SWRST_I2C_POS))
#define GLB_SWRST_PWM                                           GLB_SWRST_PWM
#define GLB_SWRST_PWM_POS                                       (16U)
#define GLB_SWRST_PWM_LEN                                       (1U)
#define GLB_SWRST_PWM_MSK                                       (((1U<<GLB_SWRST_PWM_LEN)-1)<<GLB_SWRST_PWM_POS)
#define GLB_SWRST_PWM_UMSK                                      (~(((1U<<GLB_SWRST_PWM_LEN)-1)<<GLB_SWRST_PWM_POS))
#define GLB_SWRST_TMR                                           GLB_SWRST_TMR
#define GLB_SWRST_TMR_POS                                       (17U)
#define GLB_SWRST_TMR_LEN                                       (1U)
#define GLB_SWRST_TMR_MSK                                       (((1U<<GLB_SWRST_TMR_LEN)-1)<<GLB_SWRST_TMR_POS)
#define GLB_SWRST_TMR_UMSK                                      (~(((1U<<GLB_SWRST_TMR_LEN)-1)<<GLB_SWRST_TMR_POS))
#define GLB_SWRST_IR                                            GLB_SWRST_IR
#define GLB_SWRST_IR_POS                                        (18U)
#define GLB_SWRST_IR_LEN                                        (1U)
#define GLB_SWRST_IR_MSK                                        (((1U<<GLB_SWRST_IR_LEN)-1)<<GLB_SWRST_IR_POS)
#define GLB_SWRST_IR_UMSK                                       (~(((1U<<GLB_SWRST_IR_LEN)-1)<<GLB_SWRST_IR_POS))
#define GLB_SWRST_CHK                                           GLB_SWRST_CHK
#define GLB_SWRST_CHK_POS                                       (19U)
#define GLB_SWRST_CHK_LEN                                       (1U)
#define GLB_SWRST_CHK_MSK                                       (((1U<<GLB_SWRST_CHK_LEN)-1)<<GLB_SWRST_CHK_POS)
#define GLB_SWRST_CHK_UMSK                                      (~(((1U<<GLB_SWRST_CHK_LEN)-1)<<GLB_SWRST_CHK_POS))
#define GLB_SWRST_KYS                                           GLB_SWRST_KYS
#define GLB_SWRST_KYS_POS                                       (20U)
#define GLB_SWRST_KYS_LEN                                       (1U)
#define GLB_SWRST_KYS_MSK                                       (((1U<<GLB_SWRST_KYS_LEN)-1)<<GLB_SWRST_KYS_POS)
#define GLB_SWRST_KYS_UMSK                                      (~(((1U<<GLB_SWRST_KYS_LEN)-1)<<GLB_SWRST_KYS_POS))
#define GLB_SWRST_AUSOLO                                        GLB_SWRST_AUSOLO
#define GLB_SWRST_AUSOLO_POS                                    (21U)
#define GLB_SWRST_AUSOLO_LEN                                    (1U)
#define GLB_SWRST_AUSOLO_MSK                                    (((1U<<GLB_SWRST_AUSOLO_LEN)-1)<<GLB_SWRST_AUSOLO_POS)
#define GLB_SWRST_AUSOLO_UMSK                                   (~(((1U<<GLB_SWRST_AUSOLO_LEN)-1)<<GLB_SWRST_AUSOLO_POS))
#define GLB_SWRST_PSRAM                                         GLB_SWRST_PSRAM
#define GLB_SWRST_PSRAM_POS                                     (22U)
#define GLB_SWRST_PSRAM_LEN                                     (1U)
#define GLB_SWRST_PSRAM_MSK                                     (((1U<<GLB_SWRST_PSRAM_LEN)-1)<<GLB_SWRST_PSRAM_POS)
#define GLB_SWRST_PSRAM_UMSK                                    (~(((1U<<GLB_SWRST_PSRAM_LEN)-1)<<GLB_SWRST_PSRAM_POS))
#define GLB_SWRST_QDEC                                          GLB_SWRST_QDEC
#define GLB_SWRST_QDEC_POS                                      (23U)
#define GLB_SWRST_QDEC_LEN                                      (1U)
#define GLB_SWRST_QDEC_MSK                                      (((1U<<GLB_SWRST_QDEC_LEN)-1)<<GLB_SWRST_QDEC_POS)
#define GLB_SWRST_QDEC_UMSK                                     (~(((1U<<GLB_SWRST_QDEC_LEN)-1)<<GLB_SWRST_QDEC_POS))
#define GLB_SWRST_PIO                                           GLB_SWRST_PIO
#define GLB_SWRST_PIO_POS                                       (24U)
#define GLB_SWRST_PIO_LEN                                       (1U)
#define GLB_SWRST_PIO_MSK                                       (((1U<<GLB_SWRST_PIO_LEN)-1)<<GLB_SWRST_PIO_POS)
#define GLB_SWRST_PIO_UMSK                                      (~(((1U<<GLB_SWRST_PIO_LEN)-1)<<GLB_SWRST_PIO_POS))
#define GLB_SWRST_MM                                            GLB_SWRST_MM
#define GLB_SWRST_MM_POS                                        (25U)
#define GLB_SWRST_MM_LEN                                        (1U)
#define GLB_SWRST_MM_MSK                                        (((1U<<GLB_SWRST_MM_LEN)-1)<<GLB_SWRST_MM_POS)
#define GLB_SWRST_MM_UMSK                                       (~(((1U<<GLB_SWRST_MM_LEN)-1)<<GLB_SWRST_MM_POS))
#define GLB_SWRST_USB                                           GLB_SWRST_USB
#define GLB_SWRST_USB_POS                                       (26U)
#define GLB_SWRST_USB_LEN                                       (1U)
#define GLB_SWRST_USB_MSK                                       (((1U<<GLB_SWRST_USB_LEN)-1)<<GLB_SWRST_USB_POS)
#define GLB_SWRST_USB_UMSK                                      (~(((1U<<GLB_SWRST_USB_LEN)-1)<<GLB_SWRST_USB_POS))
#define GLB_SWRST_EMAC                                          GLB_SWRST_EMAC
#define GLB_SWRST_EMAC_POS                                      (27U)
#define GLB_SWRST_EMAC_LEN                                      (1U)
#define GLB_SWRST_EMAC_MSK                                      (((1U<<GLB_SWRST_EMAC_LEN)-1)<<GLB_SWRST_EMAC_POS)
#define GLB_SWRST_EMAC_UMSK                                     (~(((1U<<GLB_SWRST_EMAC_LEN)-1)<<GLB_SWRST_EMAC_POS))
#define GLB_SWRST_UART1                                         GLB_SWRST_UART1
#define GLB_SWRST_UART1_POS                                     (28U)
#define GLB_SWRST_UART1_LEN                                     (1U)
#define GLB_SWRST_UART1_MSK                                     (((1U<<GLB_SWRST_UART1_LEN)-1)<<GLB_SWRST_UART1_POS)
#define GLB_SWRST_UART1_UMSK                                    (~(((1U<<GLB_SWRST_UART1_LEN)-1)<<GLB_SWRST_UART1_POS))
#define GLB_SWRST_CAN                                           GLB_SWRST_CAN
#define GLB_SWRST_CAN_POS                                       (29U)
#define GLB_SWRST_CAN_LEN                                       (1U)
#define GLB_SWRST_CAN_MSK                                       (((1U<<GLB_SWRST_CAN_LEN)-1)<<GLB_SWRST_CAN_POS)
#define GLB_SWRST_CAN_UMSK                                      (~(((1U<<GLB_SWRST_CAN_LEN)-1)<<GLB_SWRST_CAN_POS))
#define GLB_SWRST_I2S                                           GLB_SWRST_I2S
#define GLB_SWRST_I2S_POS                                       (30U)
#define GLB_SWRST_I2S_LEN                                       (1U)
#define GLB_SWRST_I2S_MSK                                       (((1U<<GLB_SWRST_I2S_LEN)-1)<<GLB_SWRST_I2S_POS)
#define GLB_SWRST_I2S_UMSK                                      (~(((1U<<GLB_SWRST_I2S_LEN)-1)<<GLB_SWRST_I2S_POS))
#define GLB_SWRST_SPI2                                          GLB_SWRST_SPI2
#define GLB_SWRST_SPI2_POS                                      (31U)
#define GLB_SWRST_SPI2_LEN                                      (1U)
#define GLB_SWRST_SPI2_MSK                                      (((1U<<GLB_SWRST_SPI2_LEN)-1)<<GLB_SWRST_SPI2_POS)
#define GLB_SWRST_SPI2_UMSK                                     (~(((1U<<GLB_SWRST_SPI2_LEN)-1)<<GLB_SWRST_SPI2_POS))

/* 0x28 : swrst_cfg2 */
#define GLB_SWRST_CFG2_OFFSET                                   (0x28)
#define GLB_REG_CTRL_PWRON_RST                                  GLB_REG_CTRL_PWRON_RST
#define GLB_REG_CTRL_PWRON_RST_POS                              (0U)
#define GLB_REG_CTRL_PWRON_RST_LEN                              (1U)
#define GLB_REG_CTRL_PWRON_RST_MSK                              (((1U<<GLB_REG_CTRL_PWRON_RST_LEN)-1)<<GLB_REG_CTRL_PWRON_RST_POS)
#define GLB_REG_CTRL_PWRON_RST_UMSK                             (~(((1U<<GLB_REG_CTRL_PWRON_RST_LEN)-1)<<GLB_REG_CTRL_PWRON_RST_POS))
#define GLB_REG_CTRL_CPU_RESET                                  GLB_REG_CTRL_CPU_RESET
#define GLB_REG_CTRL_CPU_RESET_POS                              (1U)
#define GLB_REG_CTRL_CPU_RESET_LEN                              (1U)
#define GLB_REG_CTRL_CPU_RESET_MSK                              (((1U<<GLB_REG_CTRL_CPU_RESET_LEN)-1)<<GLB_REG_CTRL_CPU_RESET_POS)
#define GLB_REG_CTRL_CPU_RESET_UMSK                             (~(((1U<<GLB_REG_CTRL_CPU_RESET_LEN)-1)<<GLB_REG_CTRL_CPU_RESET_POS))
#define GLB_REG_CTRL_SYS_RESET                                  GLB_REG_CTRL_SYS_RESET
#define GLB_REG_CTRL_SYS_RESET_POS                              (2U)
#define GLB_REG_CTRL_SYS_RESET_LEN                              (1U)
#define GLB_REG_CTRL_SYS_RESET_MSK                              (((1U<<GLB_REG_CTRL_SYS_RESET_LEN)-1)<<GLB_REG_CTRL_SYS_RESET_POS)
#define GLB_REG_CTRL_SYS_RESET_UMSK                             (~(((1U<<GLB_REG_CTRL_SYS_RESET_LEN)-1)<<GLB_REG_CTRL_SYS_RESET_POS))
#define GLB_REG_CTRL_RESET_DUMMY                                GLB_REG_CTRL_RESET_DUMMY
#define GLB_REG_CTRL_RESET_DUMMY_POS                            (4U)
#define GLB_REG_CTRL_RESET_DUMMY_LEN                            (4U)
#define GLB_REG_CTRL_RESET_DUMMY_MSK                            (((1U<<GLB_REG_CTRL_RESET_DUMMY_LEN)-1)<<GLB_REG_CTRL_RESET_DUMMY_POS)
#define GLB_REG_CTRL_RESET_DUMMY_UMSK                           (~(((1U<<GLB_REG_CTRL_RESET_DUMMY_LEN)-1)<<GLB_REG_CTRL_RESET_DUMMY_POS))
#define GLB_SWRST_SDHA                                          GLB_SWRST_SDHA
#define GLB_SWRST_SDHA_POS                                      (8U)
#define GLB_SWRST_SDHA_LEN                                      (1U)
#define GLB_SWRST_SDHA_MSK                                      (((1U<<GLB_SWRST_SDHA_LEN)-1)<<GLB_SWRST_SDHA_POS)
#define GLB_SWRST_SDHA_UMSK                                     (~(((1U<<GLB_SWRST_SDHA_LEN)-1)<<GLB_SWRST_SDHA_POS))
#define GLB_DISRST_SDHA                                         GLB_DISRST_SDHA
#define GLB_DISRST_SDHA_POS                                     (9U)
#define GLB_DISRST_SDHA_LEN                                     (1U)
#define GLB_DISRST_SDHA_MSK                                     (((1U<<GLB_DISRST_SDHA_LEN)-1)<<GLB_DISRST_SDHA_POS)
#define GLB_DISRST_SDHA_UMSK                                    (~(((1U<<GLB_DISRST_SDHA_LEN)-1)<<GLB_DISRST_SDHA_POS))
#define GLB_PKA_CLK_SEL                                         GLB_PKA_CLK_SEL
#define GLB_PKA_CLK_SEL_POS                                     (24U)
#define GLB_PKA_CLK_SEL_LEN                                     (2U)
#define GLB_PKA_CLK_SEL_MSK                                     (((1U<<GLB_PKA_CLK_SEL_LEN)-1)<<GLB_PKA_CLK_SEL_POS)
#define GLB_PKA_CLK_SEL_UMSK                                    (~(((1U<<GLB_PKA_CLK_SEL_LEN)-1)<<GLB_PKA_CLK_SEL_POS))

/* 0x2C : swrst_cfg3 */
#define GLB_SWRST_CFG3_OFFSET                                   (0x2C)
#define GLB_DISRST_MIX                                          GLB_DISRST_MIX
#define GLB_DISRST_MIX_POS                                      (1U)
#define GLB_DISRST_MIX_LEN                                      (1U)
#define GLB_DISRST_MIX_MSK                                      (((1U<<GLB_DISRST_MIX_LEN)-1)<<GLB_DISRST_MIX_POS)
#define GLB_DISRST_MIX_UMSK                                     (~(((1U<<GLB_DISRST_MIX_LEN)-1)<<GLB_DISRST_MIX_POS))
#define GLB_DISRST_GPIP                                         GLB_DISRST_GPIP
#define GLB_DISRST_GPIP_POS                                     (2U)
#define GLB_DISRST_GPIP_LEN                                     (1U)
#define GLB_DISRST_GPIP_MSK                                     (((1U<<GLB_DISRST_GPIP_LEN)-1)<<GLB_DISRST_GPIP_POS)
#define GLB_DISRST_GPIP_UMSK                                    (~(((1U<<GLB_DISRST_GPIP_LEN)-1)<<GLB_DISRST_GPIP_POS))
#define GLB_DISRST_CCI                                          GLB_DISRST_CCI
#define GLB_DISRST_CCI_POS                                      (8U)
#define GLB_DISRST_CCI_LEN                                      (1U)
#define GLB_DISRST_CCI_MSK                                      (((1U<<GLB_DISRST_CCI_LEN)-1)<<GLB_DISRST_CCI_POS)
#define GLB_DISRST_CCI_UMSK                                     (~(((1U<<GLB_DISRST_CCI_LEN)-1)<<GLB_DISRST_CCI_POS))
#define GLB_DISRST_SF                                           GLB_DISRST_SF
#define GLB_DISRST_SF_POS                                       (10U)
#define GLB_DISRST_SF_LEN                                       (1U)
#define GLB_DISRST_SF_MSK                                       (((1U<<GLB_DISRST_SF_LEN)-1)<<GLB_DISRST_SF_POS)
#define GLB_DISRST_SF_UMSK                                      (~(((1U<<GLB_DISRST_SF_LEN)-1)<<GLB_DISRST_SF_POS))
#define GLB_DISRST_DMA                                          GLB_DISRST_DMA
#define GLB_DISRST_DMA_POS                                      (11U)
#define GLB_DISRST_DMA_LEN                                      (1U)
#define GLB_DISRST_DMA_MSK                                      (((1U<<GLB_DISRST_DMA_LEN)-1)<<GLB_DISRST_DMA_POS)
#define GLB_DISRST_DMA_UMSK                                     (~(((1U<<GLB_DISRST_DMA_LEN)-1)<<GLB_DISRST_DMA_POS))
#define GLB_DISRST_I2C_1                                        GLB_DISRST_I2C_1
#define GLB_DISRST_I2C_1_POS                                    (12U)
#define GLB_DISRST_I2C_1_LEN                                    (1U)
#define GLB_DISRST_I2C_1_MSK                                    (((1U<<GLB_DISRST_I2C_1_LEN)-1)<<GLB_DISRST_I2C_1_POS)
#define GLB_DISRST_I2C_1_UMSK                                   (~(((1U<<GLB_DISRST_I2C_1_LEN)-1)<<GLB_DISRST_I2C_1_POS))
#define GLB_DISRST_UART0                                        GLB_DISRST_UART0
#define GLB_DISRST_UART0_POS                                    (13U)
#define GLB_DISRST_UART0_LEN                                    (1U)
#define GLB_DISRST_UART0_MSK                                    (((1U<<GLB_DISRST_UART0_LEN)-1)<<GLB_DISRST_UART0_POS)
#define GLB_DISRST_UART0_UMSK                                   (~(((1U<<GLB_DISRST_UART0_LEN)-1)<<GLB_DISRST_UART0_POS))
#define GLB_DISRST_SPI                                          GLB_DISRST_SPI
#define GLB_DISRST_SPI_POS                                      (14U)
#define GLB_DISRST_SPI_LEN                                      (1U)
#define GLB_DISRST_SPI_MSK                                      (((1U<<GLB_DISRST_SPI_LEN)-1)<<GLB_DISRST_SPI_POS)
#define GLB_DISRST_SPI_UMSK                                     (~(((1U<<GLB_DISRST_SPI_LEN)-1)<<GLB_DISRST_SPI_POS))
#define GLB_DISRST_I2C                                          GLB_DISRST_I2C
#define GLB_DISRST_I2C_POS                                      (15U)
#define GLB_DISRST_I2C_LEN                                      (1U)
#define GLB_DISRST_I2C_MSK                                      (((1U<<GLB_DISRST_I2C_LEN)-1)<<GLB_DISRST_I2C_POS)
#define GLB_DISRST_I2C_UMSK                                     (~(((1U<<GLB_DISRST_I2C_LEN)-1)<<GLB_DISRST_I2C_POS))
#define GLB_DISRST_PWM                                          GLB_DISRST_PWM
#define GLB_DISRST_PWM_POS                                      (16U)
#define GLB_DISRST_PWM_LEN                                      (1U)
#define GLB_DISRST_PWM_MSK                                      (((1U<<GLB_DISRST_PWM_LEN)-1)<<GLB_DISRST_PWM_POS)
#define GLB_DISRST_PWM_UMSK                                     (~(((1U<<GLB_DISRST_PWM_LEN)-1)<<GLB_DISRST_PWM_POS))
#define GLB_DISRST_TMR                                          GLB_DISRST_TMR
#define GLB_DISRST_TMR_POS                                      (17U)
#define GLB_DISRST_TMR_LEN                                      (1U)
#define GLB_DISRST_TMR_MSK                                      (((1U<<GLB_DISRST_TMR_LEN)-1)<<GLB_DISRST_TMR_POS)
#define GLB_DISRST_TMR_UMSK                                     (~(((1U<<GLB_DISRST_TMR_LEN)-1)<<GLB_DISRST_TMR_POS))
#define GLB_DISRST_IR                                           GLB_DISRST_IR
#define GLB_DISRST_IR_POS                                       (18U)
#define GLB_DISRST_IR_LEN                                       (1U)
#define GLB_DISRST_IR_MSK                                       (((1U<<GLB_DISRST_IR_LEN)-1)<<GLB_DISRST_IR_POS)
#define GLB_DISRST_IR_UMSK                                      (~(((1U<<GLB_DISRST_IR_LEN)-1)<<GLB_DISRST_IR_POS))
#define GLB_DISRST_CHK                                          GLB_DISRST_CHK
#define GLB_DISRST_CHK_POS                                      (19U)
#define GLB_DISRST_CHK_LEN                                      (1U)
#define GLB_DISRST_CHK_MSK                                      (((1U<<GLB_DISRST_CHK_LEN)-1)<<GLB_DISRST_CHK_POS)
#define GLB_DISRST_CHK_UMSK                                     (~(((1U<<GLB_DISRST_CHK_LEN)-1)<<GLB_DISRST_CHK_POS))
#define GLB_DISRST_KYS                                          GLB_DISRST_KYS
#define GLB_DISRST_KYS_POS                                      (20U)
#define GLB_DISRST_KYS_LEN                                      (1U)
#define GLB_DISRST_KYS_MSK                                      (((1U<<GLB_DISRST_KYS_LEN)-1)<<GLB_DISRST_KYS_POS)
#define GLB_DISRST_KYS_UMSK                                     (~(((1U<<GLB_DISRST_KYS_LEN)-1)<<GLB_DISRST_KYS_POS))
#define GLB_DISRST_AUSOLO                                       GLB_DISRST_AUSOLO
#define GLB_DISRST_AUSOLO_POS                                   (21U)
#define GLB_DISRST_AUSOLO_LEN                                   (1U)
#define GLB_DISRST_AUSOLO_MSK                                   (((1U<<GLB_DISRST_AUSOLO_LEN)-1)<<GLB_DISRST_AUSOLO_POS)
#define GLB_DISRST_AUSOLO_UMSK                                  (~(((1U<<GLB_DISRST_AUSOLO_LEN)-1)<<GLB_DISRST_AUSOLO_POS))
#define GLB_DISRST_PSRAM                                        GLB_DISRST_PSRAM
#define GLB_DISRST_PSRAM_POS                                    (22U)
#define GLB_DISRST_PSRAM_LEN                                    (1U)
#define GLB_DISRST_PSRAM_MSK                                    (((1U<<GLB_DISRST_PSRAM_LEN)-1)<<GLB_DISRST_PSRAM_POS)
#define GLB_DISRST_PSRAM_UMSK                                   (~(((1U<<GLB_DISRST_PSRAM_LEN)-1)<<GLB_DISRST_PSRAM_POS))
#define GLB_DISRST_QDEC                                         GLB_DISRST_QDEC
#define GLB_DISRST_QDEC_POS                                     (23U)
#define GLB_DISRST_QDEC_LEN                                     (1U)
#define GLB_DISRST_QDEC_MSK                                     (((1U<<GLB_DISRST_QDEC_LEN)-1)<<GLB_DISRST_QDEC_POS)
#define GLB_DISRST_QDEC_UMSK                                    (~(((1U<<GLB_DISRST_QDEC_LEN)-1)<<GLB_DISRST_QDEC_POS))
#define GLB_DISRST_PIO                                          GLB_DISRST_PIO
#define GLB_DISRST_PIO_POS                                      (24U)
#define GLB_DISRST_PIO_LEN                                      (1U)
#define GLB_DISRST_PIO_MSK                                      (((1U<<GLB_DISRST_PIO_LEN)-1)<<GLB_DISRST_PIO_POS)
#define GLB_DISRST_PIO_UMSK                                     (~(((1U<<GLB_DISRST_PIO_LEN)-1)<<GLB_DISRST_PIO_POS))
#define GLB_DISRST_MM                                           GLB_DISRST_MM
#define GLB_DISRST_MM_POS                                       (25U)
#define GLB_DISRST_MM_LEN                                       (1U)
#define GLB_DISRST_MM_MSK                                       (((1U<<GLB_DISRST_MM_LEN)-1)<<GLB_DISRST_MM_POS)
#define GLB_DISRST_MM_UMSK                                      (~(((1U<<GLB_DISRST_MM_LEN)-1)<<GLB_DISRST_MM_POS))
#define GLB_DISRST_USB                                          GLB_DISRST_USB
#define GLB_DISRST_USB_POS                                      (26U)
#define GLB_DISRST_USB_LEN                                      (1U)
#define GLB_DISRST_USB_MSK                                      (((1U<<GLB_DISRST_USB_LEN)-1)<<GLB_DISRST_USB_POS)
#define GLB_DISRST_USB_UMSK                                     (~(((1U<<GLB_DISRST_USB_LEN)-1)<<GLB_DISRST_USB_POS))
#define GLB_DISRST_EMAC                                         GLB_DISRST_EMAC
#define GLB_DISRST_EMAC_POS                                     (27U)
#define GLB_DISRST_EMAC_LEN                                     (1U)
#define GLB_DISRST_EMAC_MSK                                     (((1U<<GLB_DISRST_EMAC_LEN)-1)<<GLB_DISRST_EMAC_POS)
#define GLB_DISRST_EMAC_UMSK                                    (~(((1U<<GLB_DISRST_EMAC_LEN)-1)<<GLB_DISRST_EMAC_POS))
#define GLB_DISRST_UART1                                        GLB_DISRST_UART1
#define GLB_DISRST_UART1_POS                                    (28U)
#define GLB_DISRST_UART1_LEN                                    (1U)
#define GLB_DISRST_UART1_MSK                                    (((1U<<GLB_DISRST_UART1_LEN)-1)<<GLB_DISRST_UART1_POS)
#define GLB_DISRST_UART1_UMSK                                   (~(((1U<<GLB_DISRST_UART1_LEN)-1)<<GLB_DISRST_UART1_POS))
#define GLB_DISRST_CAN                                          GLB_DISRST_CAN
#define GLB_DISRST_CAN_POS                                      (29U)
#define GLB_DISRST_CAN_LEN                                      (1U)
#define GLB_DISRST_CAN_MSK                                      (((1U<<GLB_DISRST_CAN_LEN)-1)<<GLB_DISRST_CAN_POS)
#define GLB_DISRST_CAN_UMSK                                     (~(((1U<<GLB_DISRST_CAN_LEN)-1)<<GLB_DISRST_CAN_POS))
#define GLB_DISRST_I2S                                          GLB_DISRST_I2S
#define GLB_DISRST_I2S_POS                                      (30U)
#define GLB_DISRST_I2S_LEN                                      (1U)
#define GLB_DISRST_I2S_MSK                                      (((1U<<GLB_DISRST_I2S_LEN)-1)<<GLB_DISRST_I2S_POS)
#define GLB_DISRST_I2S_UMSK                                     (~(((1U<<GLB_DISRST_I2S_LEN)-1)<<GLB_DISRST_I2S_POS))
#define GLB_DISRST_SPI2                                         GLB_DISRST_SPI2
#define GLB_DISRST_SPI2_POS                                     (31U)
#define GLB_DISRST_SPI2_LEN                                     (1U)
#define GLB_DISRST_SPI2_MSK                                     (((1U<<GLB_DISRST_SPI2_LEN)-1)<<GLB_DISRST_SPI2_POS)
#define GLB_DISRST_SPI2_UMSK                                    (~(((1U<<GLB_DISRST_SPI2_LEN)-1)<<GLB_DISRST_SPI2_POS))

/* 0x30 : cgen_cfg0 */
#define GLB_CGEN_CFG0_OFFSET                                    (0x30)
#define GLB_CGEN_MCU                                            GLB_CGEN_MCU
#define GLB_CGEN_MCU_POS                                        (0U)
#define GLB_CGEN_MCU_LEN                                        (1U)
#define GLB_CGEN_MCU_MSK                                        (((1U<<GLB_CGEN_MCU_LEN)-1)<<GLB_CGEN_MCU_POS)
#define GLB_CGEN_MCU_UMSK                                       (~(((1U<<GLB_CGEN_MCU_LEN)-1)<<GLB_CGEN_MCU_POS))
#define GLB_CGEN_SEC                                            GLB_CGEN_SEC
#define GLB_CGEN_SEC_POS                                        (1U)
#define GLB_CGEN_SEC_LEN                                        (1U)
#define GLB_CGEN_SEC_MSK                                        (((1U<<GLB_CGEN_SEC_LEN)-1)<<GLB_CGEN_SEC_POS)
#define GLB_CGEN_SEC_UMSK                                       (~(((1U<<GLB_CGEN_SEC_LEN)-1)<<GLB_CGEN_SEC_POS))
#define GLB_CGEN_DMA                                            GLB_CGEN_DMA
#define GLB_CGEN_DMA_POS                                        (2U)
#define GLB_CGEN_DMA_LEN                                        (1U)
#define GLB_CGEN_DMA_MSK                                        (((1U<<GLB_CGEN_DMA_LEN)-1)<<GLB_CGEN_DMA_POS)
#define GLB_CGEN_DMA_UMSK                                       (~(((1U<<GLB_CGEN_DMA_LEN)-1)<<GLB_CGEN_DMA_POS))
#define GLB_CGEN_CCI                                            GLB_CGEN_CCI
#define GLB_CGEN_CCI_POS                                        (3U)
#define GLB_CGEN_CCI_LEN                                        (1U)
#define GLB_CGEN_CCI_MSK                                        (((1U<<GLB_CGEN_CCI_LEN)-1)<<GLB_CGEN_CCI_POS)
#define GLB_CGEN_CCI_UMSK                                       (~(((1U<<GLB_CGEN_CCI_LEN)-1)<<GLB_CGEN_CCI_POS))
#define GLB_CGEN_GPIP                                           GLB_CGEN_GPIP
#define GLB_CGEN_GPIP_POS                                       (4U)
#define GLB_CGEN_GPIP_LEN                                       (1U)
#define GLB_CGEN_GPIP_MSK                                       (((1U<<GLB_CGEN_GPIP_LEN)-1)<<GLB_CGEN_GPIP_POS)
#define GLB_CGEN_GPIP_UMSK                                      (~(((1U<<GLB_CGEN_GPIP_LEN)-1)<<GLB_CGEN_GPIP_POS))
#define GLB_CGEN_SEC_DBG                                        GLB_CGEN_SEC_DBG
#define GLB_CGEN_SEC_DBG_POS                                    (5U)
#define GLB_CGEN_SEC_DBG_LEN                                    (1U)
#define GLB_CGEN_SEC_DBG_MSK                                    (((1U<<GLB_CGEN_SEC_DBG_LEN)-1)<<GLB_CGEN_SEC_DBG_POS)
#define GLB_CGEN_SEC_DBG_UMSK                                   (~(((1U<<GLB_CGEN_SEC_DBG_LEN)-1)<<GLB_CGEN_SEC_DBG_POS))
#define GLB_CGEN_SEC_ENG                                        GLB_CGEN_SEC_ENG
#define GLB_CGEN_SEC_ENG_POS                                    (6U)
#define GLB_CGEN_SEC_ENG_LEN                                    (1U)
#define GLB_CGEN_SEC_ENG_MSK                                    (((1U<<GLB_CGEN_SEC_ENG_LEN)-1)<<GLB_CGEN_SEC_ENG_POS)
#define GLB_CGEN_SEC_ENG_UMSK                                   (~(((1U<<GLB_CGEN_SEC_ENG_LEN)-1)<<GLB_CGEN_SEC_ENG_POS))
#define GLB_CGEN_TZ1                                            GLB_CGEN_TZ1
#define GLB_CGEN_TZ1_POS                                        (7U)
#define GLB_CGEN_TZ1_LEN                                        (1U)
#define GLB_CGEN_TZ1_MSK                                        (((1U<<GLB_CGEN_TZ1_LEN)-1)<<GLB_CGEN_TZ1_POS)
#define GLB_CGEN_TZ1_UMSK                                       (~(((1U<<GLB_CGEN_TZ1_LEN)-1)<<GLB_CGEN_TZ1_POS))
#define GLB_CGEN_EFUSE                                          GLB_CGEN_EFUSE
#define GLB_CGEN_EFUSE_POS                                      (8U)
#define GLB_CGEN_EFUSE_LEN                                      (1U)
#define GLB_CGEN_EFUSE_MSK                                      (((1U<<GLB_CGEN_EFUSE_LEN)-1)<<GLB_CGEN_EFUSE_POS)
#define GLB_CGEN_EFUSE_UMSK                                     (~(((1U<<GLB_CGEN_EFUSE_LEN)-1)<<GLB_CGEN_EFUSE_POS))
#define GLB_CGEN_SF                                             GLB_CGEN_SF
#define GLB_CGEN_SF_POS                                         (9U)
#define GLB_CGEN_SF_LEN                                         (1U)
#define GLB_CGEN_SF_MSK                                         (((1U<<GLB_CGEN_SF_LEN)-1)<<GLB_CGEN_SF_POS)
#define GLB_CGEN_SF_UMSK                                        (~(((1U<<GLB_CGEN_SF_LEN)-1)<<GLB_CGEN_SF_POS))
#define GLB_CGEN_SPI2                                           GLB_CGEN_SPI2
#define GLB_CGEN_SPI2_POS                                       (10U)
#define GLB_CGEN_SPI2_LEN                                       (1U)
#define GLB_CGEN_SPI2_MSK                                       (((1U<<GLB_CGEN_SPI2_LEN)-1)<<GLB_CGEN_SPI2_POS)
#define GLB_CGEN_SPI2_UMSK                                      (~(((1U<<GLB_CGEN_SPI2_LEN)-1)<<GLB_CGEN_SPI2_POS))
#define GLB_CGEN_UART                                           GLB_CGEN_UART
#define GLB_CGEN_UART_POS                                       (11U)
#define GLB_CGEN_UART_LEN                                       (1U)
#define GLB_CGEN_UART_MSK                                       (((1U<<GLB_CGEN_UART_LEN)-1)<<GLB_CGEN_UART_POS)
#define GLB_CGEN_UART_UMSK                                      (~(((1U<<GLB_CGEN_UART_LEN)-1)<<GLB_CGEN_UART_POS))
#define GLB_CGEN_SPI                                            GLB_CGEN_SPI
#define GLB_CGEN_SPI_POS                                        (12U)
#define GLB_CGEN_SPI_LEN                                        (1U)
#define GLB_CGEN_SPI_MSK                                        (((1U<<GLB_CGEN_SPI_LEN)-1)<<GLB_CGEN_SPI_POS)
#define GLB_CGEN_SPI_UMSK                                       (~(((1U<<GLB_CGEN_SPI_LEN)-1)<<GLB_CGEN_SPI_POS))
#define GLB_CGEN_I2C                                            GLB_CGEN_I2C
#define GLB_CGEN_I2C_POS                                        (13U)
#define GLB_CGEN_I2C_LEN                                        (1U)
#define GLB_CGEN_I2C_MSK                                        (((1U<<GLB_CGEN_I2C_LEN)-1)<<GLB_CGEN_I2C_POS)
#define GLB_CGEN_I2C_UMSK                                       (~(((1U<<GLB_CGEN_I2C_LEN)-1)<<GLB_CGEN_I2C_POS))
#define GLB_CGEN_PWM                                            GLB_CGEN_PWM
#define GLB_CGEN_PWM_POS                                        (14U)
#define GLB_CGEN_PWM_LEN                                        (1U)
#define GLB_CGEN_PWM_MSK                                        (((1U<<GLB_CGEN_PWM_LEN)-1)<<GLB_CGEN_PWM_POS)
#define GLB_CGEN_PWM_UMSK                                       (~(((1U<<GLB_CGEN_PWM_LEN)-1)<<GLB_CGEN_PWM_POS))
#define GLB_CGEN_TMR                                            GLB_CGEN_TMR
#define GLB_CGEN_TMR_POS                                        (15U)
#define GLB_CGEN_TMR_LEN                                        (1U)
#define GLB_CGEN_TMR_MSK                                        (((1U<<GLB_CGEN_TMR_LEN)-1)<<GLB_CGEN_TMR_POS)
#define GLB_CGEN_TMR_UMSK                                       (~(((1U<<GLB_CGEN_TMR_LEN)-1)<<GLB_CGEN_TMR_POS))
#define GLB_CGEN_IRR                                            GLB_CGEN_IRR
#define GLB_CGEN_IRR_POS                                        (16U)
#define GLB_CGEN_IRR_LEN                                        (1U)
#define GLB_CGEN_IRR_MSK                                        (((1U<<GLB_CGEN_IRR_LEN)-1)<<GLB_CGEN_IRR_POS)
#define GLB_CGEN_IRR_UMSK                                       (~(((1U<<GLB_CGEN_IRR_LEN)-1)<<GLB_CGEN_IRR_POS))
#define GLB_CGEN_CKS                                            GLB_CGEN_CKS
#define GLB_CGEN_CKS_POS                                        (17U)
#define GLB_CGEN_CKS_LEN                                        (1U)
#define GLB_CGEN_CKS_MSK                                        (((1U<<GLB_CGEN_CKS_LEN)-1)<<GLB_CGEN_CKS_POS)
#define GLB_CGEN_CKS_UMSK                                       (~(((1U<<GLB_CGEN_CKS_LEN)-1)<<GLB_CGEN_CKS_POS))
#define GLB_CGEN_KYS                                            GLB_CGEN_KYS
#define GLB_CGEN_KYS_POS                                        (18U)
#define GLB_CGEN_KYS_LEN                                        (1U)
#define GLB_CGEN_KYS_MSK                                        (((1U<<GLB_CGEN_KYS_LEN)-1)<<GLB_CGEN_KYS_POS)
#define GLB_CGEN_KYS_UMSK                                       (~(((1U<<GLB_CGEN_KYS_LEN)-1)<<GLB_CGEN_KYS_POS))
#define GLB_CGEN_AUDIO                                          GLB_CGEN_AUDIO
#define GLB_CGEN_AUDIO_POS                                      (19U)
#define GLB_CGEN_AUDIO_LEN                                      (1U)
#define GLB_CGEN_AUDIO_MSK                                      (((1U<<GLB_CGEN_AUDIO_LEN)-1)<<GLB_CGEN_AUDIO_POS)
#define GLB_CGEN_AUDIO_UMSK                                     (~(((1U<<GLB_CGEN_AUDIO_LEN)-1)<<GLB_CGEN_AUDIO_POS))
#define GLB_CGEN_MM                                             GLB_CGEN_MM
#define GLB_CGEN_MM_POS                                         (20U)
#define GLB_CGEN_MM_LEN                                         (1U)
#define GLB_CGEN_MM_MSK                                         (((1U<<GLB_CGEN_MM_LEN)-1)<<GLB_CGEN_MM_POS)
#define GLB_CGEN_MM_UMSK                                        (~(((1U<<GLB_CGEN_MM_LEN)-1)<<GLB_CGEN_MM_POS))
#define GLB_CGEN_UART1                                          GLB_CGEN_UART1
#define GLB_CGEN_UART1_POS                                      (21U)
#define GLB_CGEN_UART1_LEN                                      (1U)
#define GLB_CGEN_UART1_MSK                                      (((1U<<GLB_CGEN_UART1_LEN)-1)<<GLB_CGEN_UART1_POS)
#define GLB_CGEN_UART1_UMSK                                     (~(((1U<<GLB_CGEN_UART1_LEN)-1)<<GLB_CGEN_UART1_POS))
#define GLB_CGEN_CAN                                            GLB_CGEN_CAN
#define GLB_CGEN_CAN_POS                                        (22U)
#define GLB_CGEN_CAN_LEN                                        (1U)
#define GLB_CGEN_CAN_MSK                                        (((1U<<GLB_CGEN_CAN_LEN)-1)<<GLB_CGEN_CAN_POS)
#define GLB_CGEN_CAN_UMSK                                       (~(((1U<<GLB_CGEN_CAN_LEN)-1)<<GLB_CGEN_CAN_POS))
#define GLB_CGEN_I2S                                            GLB_CGEN_I2S
#define GLB_CGEN_I2S_POS                                        (23U)
#define GLB_CGEN_I2S_LEN                                        (1U)
#define GLB_CGEN_I2S_MSK                                        (((1U<<GLB_CGEN_I2S_LEN)-1)<<GLB_CGEN_I2S_POS)
#define GLB_CGEN_I2S_UMSK                                       (~(((1U<<GLB_CGEN_I2S_LEN)-1)<<GLB_CGEN_I2S_POS))
#define GLB_CGEN_QDEC0                                          GLB_CGEN_QDEC0
#define GLB_CGEN_QDEC0_POS                                      (24U)
#define GLB_CGEN_QDEC0_LEN                                      (1U)
#define GLB_CGEN_QDEC0_MSK                                      (((1U<<GLB_CGEN_QDEC0_LEN)-1)<<GLB_CGEN_QDEC0_POS)
#define GLB_CGEN_QDEC0_UMSK                                     (~(((1U<<GLB_CGEN_QDEC0_LEN)-1)<<GLB_CGEN_QDEC0_POS))
#define GLB_CGEN_QDEC1                                          GLB_CGEN_QDEC1
#define GLB_CGEN_QDEC1_POS                                      (25U)
#define GLB_CGEN_QDEC1_LEN                                      (1U)
#define GLB_CGEN_QDEC1_MSK                                      (((1U<<GLB_CGEN_QDEC1_LEN)-1)<<GLB_CGEN_QDEC1_POS)
#define GLB_CGEN_QDEC1_UMSK                                     (~(((1U<<GLB_CGEN_QDEC1_LEN)-1)<<GLB_CGEN_QDEC1_POS))
#define GLB_CGEN_QDEC2                                          GLB_CGEN_QDEC2
#define GLB_CGEN_QDEC2_POS                                      (26U)
#define GLB_CGEN_QDEC2_LEN                                      (1U)
#define GLB_CGEN_QDEC2_MSK                                      (((1U<<GLB_CGEN_QDEC2_LEN)-1)<<GLB_CGEN_QDEC2_POS)
#define GLB_CGEN_QDEC2_UMSK                                     (~(((1U<<GLB_CGEN_QDEC2_LEN)-1)<<GLB_CGEN_QDEC2_POS))
#define GLB_CGEN_PIO                                            GLB_CGEN_PIO
#define GLB_CGEN_PIO_POS                                        (27U)
#define GLB_CGEN_PIO_LEN                                        (1U)
#define GLB_CGEN_PIO_MSK                                        (((1U<<GLB_CGEN_PIO_LEN)-1)<<GLB_CGEN_PIO_POS)
#define GLB_CGEN_PIO_UMSK                                       (~(((1U<<GLB_CGEN_PIO_LEN)-1)<<GLB_CGEN_PIO_POS))
#define GLB_CGEN_USB                                            GLB_CGEN_USB
#define GLB_CGEN_USB_POS                                        (28U)
#define GLB_CGEN_USB_LEN                                        (1U)
#define GLB_CGEN_USB_MSK                                        (((1U<<GLB_CGEN_USB_LEN)-1)<<GLB_CGEN_USB_POS)
#define GLB_CGEN_USB_UMSK                                       (~(((1U<<GLB_CGEN_USB_LEN)-1)<<GLB_CGEN_USB_POS))
#define GLB_CGEN_PKA                                            GLB_CGEN_PKA
#define GLB_CGEN_PKA_POS                                        (29U)
#define GLB_CGEN_PKA_LEN                                        (1U)
#define GLB_CGEN_PKA_MSK                                        (((1U<<GLB_CGEN_PKA_LEN)-1)<<GLB_CGEN_PKA_POS)
#define GLB_CGEN_PKA_UMSK                                       (~(((1U<<GLB_CGEN_PKA_LEN)-1)<<GLB_CGEN_PKA_POS))
#define GLB_CGEN_EMAC                                           GLB_CGEN_EMAC
#define GLB_CGEN_EMAC_POS                                       (30U)
#define GLB_CGEN_EMAC_LEN                                       (1U)
#define GLB_CGEN_EMAC_MSK                                       (((1U<<GLB_CGEN_EMAC_LEN)-1)<<GLB_CGEN_EMAC_POS)
#define GLB_CGEN_EMAC_UMSK                                      (~(((1U<<GLB_CGEN_EMAC_LEN)-1)<<GLB_CGEN_EMAC_POS))
#define GLB_CGEN_SDHA                                           GLB_CGEN_SDHA
#define GLB_CGEN_SDHA_POS                                       (31U)
#define GLB_CGEN_SDHA_LEN                                       (1U)
#define GLB_CGEN_SDHA_MSK                                       (((1U<<GLB_CGEN_SDHA_LEN)-1)<<GLB_CGEN_SDHA_POS)
#define GLB_CGEN_SDHA_UMSK                                      (~(((1U<<GLB_CGEN_SDHA_LEN)-1)<<GLB_CGEN_SDHA_POS))

/* 0x34 : cgen_cfg1 */
#define GLB_CGEN_CFG1_OFFSET                                    (0x34)
#define GLB_CGEN_I2C_1                                          GLB_CGEN_I2C_1
#define GLB_CGEN_I2C_1_POS                                      (14U)
#define GLB_CGEN_I2C_1_LEN                                      (1U)
#define GLB_CGEN_I2C_1_MSK                                      (((1U<<GLB_CGEN_I2C_1_LEN)-1)<<GLB_CGEN_I2C_1_POS)
#define GLB_CGEN_I2C_1_UMSK                                     (~(((1U<<GLB_CGEN_I2C_1_LEN)-1)<<GLB_CGEN_I2C_1_POS))

/* 0x38 : cgen_cfg2 */
#define GLB_CGEN_CFG2_OFFSET                                    (0x38)
#define GLB_CGEN_S300                                           GLB_CGEN_S300
#define GLB_CGEN_S300_POS                                       (0U)
#define GLB_CGEN_S300_LEN                                       (1U)
#define GLB_CGEN_S300_MSK                                       (((1U<<GLB_CGEN_S300_LEN)-1)<<GLB_CGEN_S300_POS)
#define GLB_CGEN_S300_UMSK                                      (~(((1U<<GLB_CGEN_S300_LEN)-1)<<GLB_CGEN_S300_POS))
#define GLB_CGEN_S301                                           GLB_CGEN_S301
#define GLB_CGEN_S301_POS                                       (4U)
#define GLB_CGEN_S301_LEN                                       (1U)
#define GLB_CGEN_S301_MSK                                       (((1U<<GLB_CGEN_S301_LEN)-1)<<GLB_CGEN_S301_POS)
#define GLB_CGEN_S301_UMSK                                      (~(((1U<<GLB_CGEN_S301_LEN)-1)<<GLB_CGEN_S301_POS))

/* 0x40 : MBIST_CTL */
#define GLB_MBIST_CTL_OFFSET                                    (0x40)
#define GLB_IROM_MBIST_MODE                                     GLB_IROM_MBIST_MODE
#define GLB_IROM_MBIST_MODE_POS                                 (0U)
#define GLB_IROM_MBIST_MODE_LEN                                 (1U)
#define GLB_IROM_MBIST_MODE_MSK                                 (((1U<<GLB_IROM_MBIST_MODE_LEN)-1)<<GLB_IROM_MBIST_MODE_POS)
#define GLB_IROM_MBIST_MODE_UMSK                                (~(((1U<<GLB_IROM_MBIST_MODE_LEN)-1)<<GLB_IROM_MBIST_MODE_POS))
#define GLB_HSRAM_CACHE_MBIST_MODE                              GLB_HSRAM_CACHE_MBIST_MODE
#define GLB_HSRAM_CACHE_MBIST_MODE_POS                          (2U)
#define GLB_HSRAM_CACHE_MBIST_MODE_LEN                          (1U)
#define GLB_HSRAM_CACHE_MBIST_MODE_MSK                          (((1U<<GLB_HSRAM_CACHE_MBIST_MODE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_MODE_POS)
#define GLB_HSRAM_CACHE_MBIST_MODE_UMSK                         (~(((1U<<GLB_HSRAM_CACHE_MBIST_MODE_LEN)-1)<<GLB_HSRAM_CACHE_MBIST_MODE_POS))
#define GLB_TAG_MBIST_MODE                                      GLB_TAG_MBIST_MODE
#define GLB_TAG_MBIST_MODE_POS                                  (3U)
#define GLB_TAG_MBIST_MODE_LEN                                  (1U)
#define GLB_TAG_MBIST_MODE_MSK                                  (((1U<<GLB_TAG_MBIST_MODE_LEN)-1)<<GLB_TAG_MBIST_MODE_POS)
#define GLB_TAG_MBIST_MODE_UMSK                                 (~(((1U<<GLB_TAG_MBIST_MODE_LEN)-1)<<GLB_TAG_MBIST_MODE_POS))
#define GLB_OCRAM_MBIST_MODE                                    GLB_OCRAM_MBIST_MODE
#define GLB_OCRAM_MBIST_MODE_POS                                (4U)
#define GLB_OCRAM_MBIST_MODE_LEN                                (1U)
#define GLB_OCRAM_MBIST_MODE_MSK                                (((1U<<GLB_OCRAM_MBIST_MODE_LEN)-1)<<GLB_OCRAM_MBIST_MODE_POS)
#define GLB_OCRAM_MBIST_MODE_UMSK                               (~(((1U<<GLB_OCRAM_MBIST_MODE_LEN)-1)<<GLB_OCRAM_MBIST_MODE_POS))
#define GLB_EM_RAM_MBIST_MODE                                   GLB_EM_RAM_MBIST_MODE
#define GLB_EM_RAM_MBIST_MODE_POS                               (5U)
#define GLB_EM_RAM_MBIST_MODE_LEN                               (1U)
#define GLB_EM_RAM_MBIST_MODE_MSK                               (((1U<<GLB_EM_RAM_MBIST_MODE_LEN)-1)<<GLB_EM_RAM_MBIST_MODE_POS)
#define GLB_EM_RAM_MBIST_MODE_UMSK                              (~(((1U<<GLB_EM_RAM_MBIST_MODE_LEN)-1)<<GLB_EM_RAM_MBIST_MODE_POS))
#define GLB_M154_MBIST_MODE                                     GLB_M154_MBIST_MODE
#define GLB_M154_MBIST_MODE_POS                                 (6U)
#define GLB_M154_MBIST_MODE_LEN                                 (1U)
#define GLB_M154_MBIST_MODE_MSK                                 (((1U<<GLB_M154_MBIST_MODE_LEN)-1)<<GLB_M154_MBIST_MODE_POS)
#define GLB_M154_MBIST_MODE_UMSK                                (~(((1U<<GLB_M154_MBIST_MODE_LEN)-1)<<GLB_M154_MBIST_MODE_POS))
#define GLB_SF_MBIST_MODE                                       GLB_SF_MBIST_MODE
#define GLB_SF_MBIST_MODE_POS                                   (7U)
#define GLB_SF_MBIST_MODE_LEN                                   (1U)
#define GLB_SF_MBIST_MODE_MSK                                   (((1U<<GLB_SF_MBIST_MODE_LEN)-1)<<GLB_SF_MBIST_MODE_POS)
#define GLB_SF_MBIST_MODE_UMSK                                  (~(((1U<<GLB_SF_MBIST_MODE_LEN)-1)<<GLB_SF_MBIST_MODE_POS))
#define GLB_SEC_MBIST_MODE                                      GLB_SEC_MBIST_MODE
#define GLB_SEC_MBIST_MODE_POS                                  (8U)
#define GLB_SEC_MBIST_MODE_LEN                                  (1U)
#define GLB_SEC_MBIST_MODE_MSK                                  (((1U<<GLB_SEC_MBIST_MODE_LEN)-1)<<GLB_SEC_MBIST_MODE_POS)
#define GLB_SEC_MBIST_MODE_UMSK                                 (~(((1U<<GLB_SEC_MBIST_MODE_LEN)-1)<<GLB_SEC_MBIST_MODE_POS))
#define GLB_MBIST_MODE                                          GLB_MBIST_MODE
#define GLB_MBIST_MODE_POS                                      (9U)
#define GLB_MBIST_MODE_LEN                                      (1U)
#define GLB_MBIST_MODE_MSK                                      (((1U<<GLB_MBIST_MODE_LEN)-1)<<GLB_MBIST_MODE_POS)
#define GLB_MBIST_MODE_UMSK                                     (~(((1U<<GLB_MBIST_MODE_LEN)-1)<<GLB_MBIST_MODE_POS))
#define GLB_REG_MBIST_RST_N                                     GLB_REG_MBIST_RST_N
#define GLB_REG_MBIST_RST_N_POS                                 (31U)
#define GLB_REG_MBIST_RST_N_LEN                                 (1U)
#define GLB_REG_MBIST_RST_N_MSK                                 (((1U<<GLB_REG_MBIST_RST_N_LEN)-1)<<GLB_REG_MBIST_RST_N_POS)
#define GLB_REG_MBIST_RST_N_UMSK                                (~(((1U<<GLB_REG_MBIST_RST_N_LEN)-1)<<GLB_REG_MBIST_RST_N_POS))

/* 0x44 : MBIST_STAT */
#define GLB_MBIST_STAT_OFFSET                                   (0x44)

/* 0x50 : NEW_RTC_TIME_L */
#define GLB_NEW_RTC_TIME_L_OFFSET                               (0x50)
#define GLB_RTC_TIMER_LATCH_L                                   GLB_RTC_TIMER_LATCH_L
#define GLB_RTC_TIMER_LATCH_L_POS                               (0U)
#define GLB_RTC_TIMER_LATCH_L_LEN                               (32U)
#define GLB_RTC_TIMER_LATCH_L_MSK                               (((1U<<GLB_RTC_TIMER_LATCH_L_LEN)-1)<<GLB_RTC_TIMER_LATCH_L_POS)
#define GLB_RTC_TIMER_LATCH_L_UMSK                              (~(((1U<<GLB_RTC_TIMER_LATCH_L_LEN)-1)<<GLB_RTC_TIMER_LATCH_L_POS))

/* 0x54 : NEW_RTC_TIME_H */
#define GLB_NEW_RTC_TIME_H_OFFSET                               (0x54)
#define GLB_RTC_TIMER_LATCH_H                                   GLB_RTC_TIMER_LATCH_H
#define GLB_RTC_TIMER_LATCH_H_POS                               (0U)
#define GLB_RTC_TIMER_LATCH_H_LEN                               (13U)
#define GLB_RTC_TIMER_LATCH_H_MSK                               (((1U<<GLB_RTC_TIMER_LATCH_H_LEN)-1)<<GLB_RTC_TIMER_LATCH_H_POS)
#define GLB_RTC_TIMER_LATCH_H_UMSK                              (~(((1U<<GLB_RTC_TIMER_LATCH_H_LEN)-1)<<GLB_RTC_TIMER_LATCH_H_POS))
#define GLB_REG_RTC_TIMER_LATCH_EN                              GLB_REG_RTC_TIMER_LATCH_EN
#define GLB_REG_RTC_TIMER_LATCH_EN_POS                          (24U)
#define GLB_REG_RTC_TIMER_LATCH_EN_LEN                          (1U)
#define GLB_REG_RTC_TIMER_LATCH_EN_MSK                          (((1U<<GLB_REG_RTC_TIMER_LATCH_EN_LEN)-1)<<GLB_REG_RTC_TIMER_LATCH_EN_POS)
#define GLB_REG_RTC_TIMER_LATCH_EN_UMSK                         (~(((1U<<GLB_REG_RTC_TIMER_LATCH_EN_LEN)-1)<<GLB_REG_RTC_TIMER_LATCH_EN_POS))

/* 0x5C : XTAL_DEG_32K */
#define GLB_XTAL_DEG_32K_OFFSET                                 (0x5C)
#define GLB_XTAL_DEG_CNT_LIMIT                                  GLB_XTAL_DEG_CNT_LIMIT
#define GLB_XTAL_DEG_CNT_LIMIT_POS                              (0U)
#define GLB_XTAL_DEG_CNT_LIMIT_LEN                              (8U)
#define GLB_XTAL_DEG_CNT_LIMIT_MSK                              (((1U<<GLB_XTAL_DEG_CNT_LIMIT_LEN)-1)<<GLB_XTAL_DEG_CNT_LIMIT_POS)
#define GLB_XTAL_DEG_CNT_LIMIT_UMSK                             (~(((1U<<GLB_XTAL_DEG_CNT_LIMIT_LEN)-1)<<GLB_XTAL_DEG_CNT_LIMIT_POS))
#define GLB_REG_RC32K_DEG_EN                                    GLB_REG_RC32K_DEG_EN
#define GLB_REG_RC32K_DEG_EN_POS                                (13U)
#define GLB_REG_RC32K_DEG_EN_LEN                                (1U)
#define GLB_REG_RC32K_DEG_EN_MSK                                (((1U<<GLB_REG_RC32K_DEG_EN_LEN)-1)<<GLB_REG_RC32K_DEG_EN_POS)
#define GLB_REG_RC32K_DEG_EN_UMSK                               (~(((1U<<GLB_REG_RC32K_DEG_EN_LEN)-1)<<GLB_REG_RC32K_DEG_EN_POS))
#define GLB_RC32K_DEG_END_PS                                    GLB_RC32K_DEG_END_PS
#define GLB_RC32K_DEG_END_PS_POS                                (14U)
#define GLB_RC32K_DEG_END_PS_LEN                                (1U)
#define GLB_RC32K_DEG_END_PS_MSK                                (((1U<<GLB_RC32K_DEG_END_PS_LEN)-1)<<GLB_RC32K_DEG_END_PS_POS)
#define GLB_RC32K_DEG_END_PS_UMSK                               (~(((1U<<GLB_RC32K_DEG_END_PS_LEN)-1)<<GLB_RC32K_DEG_END_PS_POS))
#define GLB_RC32K_DEG_START_PS                                  GLB_RC32K_DEG_START_PS
#define GLB_RC32K_DEG_START_PS_POS                              (15U)
#define GLB_RC32K_DEG_START_PS_LEN                              (1U)
#define GLB_RC32K_DEG_START_PS_MSK                              (((1U<<GLB_RC32K_DEG_START_PS_LEN)-1)<<GLB_RC32K_DEG_START_PS_POS)
#define GLB_RC32K_DEG_START_PS_UMSK                             (~(((1U<<GLB_RC32K_DEG_START_PS_LEN)-1)<<GLB_RC32K_DEG_START_PS_POS))
#define GLB_XTAL_CNT_32K_CGEN                                   GLB_XTAL_CNT_32K_CGEN
#define GLB_XTAL_CNT_32K_CGEN_POS                               (27U)
#define GLB_XTAL_CNT_32K_CGEN_LEN                               (1U)
#define GLB_XTAL_CNT_32K_CGEN_MSK                               (((1U<<GLB_XTAL_CNT_32K_CGEN_LEN)-1)<<GLB_XTAL_CNT_32K_CGEN_POS)
#define GLB_XTAL_CNT_32K_CGEN_UMSK                              (~(((1U<<GLB_XTAL_CNT_32K_CGEN_LEN)-1)<<GLB_XTAL_CNT_32K_CGEN_POS))
#define GLB_CLR_XTAL_CNT_32K_DONE                               GLB_CLR_XTAL_CNT_32K_DONE
#define GLB_CLR_XTAL_CNT_32K_DONE_POS                           (28U)
#define GLB_CLR_XTAL_CNT_32K_DONE_LEN                           (1U)
#define GLB_CLR_XTAL_CNT_32K_DONE_MSK                           (((1U<<GLB_CLR_XTAL_CNT_32K_DONE_LEN)-1)<<GLB_CLR_XTAL_CNT_32K_DONE_POS)
#define GLB_CLR_XTAL_CNT_32K_DONE_UMSK                          (~(((1U<<GLB_CLR_XTAL_CNT_32K_DONE_LEN)-1)<<GLB_CLR_XTAL_CNT_32K_DONE_POS))
#define GLB_XTAL_CNT_32K_SW_TRIG_PS                             GLB_XTAL_CNT_32K_SW_TRIG_PS
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_POS                         (31U)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN                         (1U)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_MSK                         (((1U<<GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN)-1)<<GLB_XTAL_CNT_32K_SW_TRIG_PS_POS)
#define GLB_XTAL_CNT_32K_SW_TRIG_PS_UMSK                        (~(((1U<<GLB_XTAL_CNT_32K_SW_TRIG_PS_LEN)-1)<<GLB_XTAL_CNT_32K_SW_TRIG_PS_POS))

/* 0x60 : bmx_cfg1 */
#define GLB_BMX_CFG1_OFFSET                                     (0x60)
#define GLB_REG_TIMEOUT_EN                                      GLB_REG_TIMEOUT_EN
#define GLB_REG_TIMEOUT_EN_POS                                  (0U)
#define GLB_REG_TIMEOUT_EN_LEN                                  (3U)
#define GLB_REG_TIMEOUT_EN_MSK                                  (((1U<<GLB_REG_TIMEOUT_EN_LEN)-1)<<GLB_REG_TIMEOUT_EN_POS)
#define GLB_REG_TIMEOUT_EN_UMSK                                 (~(((1U<<GLB_REG_TIMEOUT_EN_LEN)-1)<<GLB_REG_TIMEOUT_EN_POS))
#define GLB_TIMEOUT_STS                                         GLB_TIMEOUT_STS
#define GLB_TIMEOUT_STS_POS                                     (8U)
#define GLB_TIMEOUT_STS_LEN                                     (3U)
#define GLB_TIMEOUT_STS_MSK                                     (((1U<<GLB_TIMEOUT_STS_LEN)-1)<<GLB_TIMEOUT_STS_POS)
#define GLB_TIMEOUT_STS_UMSK                                    (~(((1U<<GLB_TIMEOUT_STS_LEN)-1)<<GLB_TIMEOUT_STS_POS))
#define GLB_REG_TIMEOUT_CLR                                     GLB_REG_TIMEOUT_CLR
#define GLB_REG_TIMEOUT_CLR_POS                                 (16U)
#define GLB_REG_TIMEOUT_CLR_LEN                                 (1U)
#define GLB_REG_TIMEOUT_CLR_MSK                                 (((1U<<GLB_REG_TIMEOUT_CLR_LEN)-1)<<GLB_REG_TIMEOUT_CLR_POS)
#define GLB_REG_TIMEOUT_CLR_UMSK                                (~(((1U<<GLB_REG_TIMEOUT_CLR_LEN)-1)<<GLB_REG_TIMEOUT_CLR_POS))
#define GLB_REG_ARB_MODE                                        GLB_REG_ARB_MODE
#define GLB_REG_ARB_MODE_POS                                    (17U)
#define GLB_REG_ARB_MODE_LEN                                    (1U)
#define GLB_REG_ARB_MODE_MSK                                    (((1U<<GLB_REG_ARB_MODE_LEN)-1)<<GLB_REG_ARB_MODE_POS)
#define GLB_REG_ARB_MODE_UMSK                                   (~(((1U<<GLB_REG_ARB_MODE_LEN)-1)<<GLB_REG_ARB_MODE_POS))
#define GLB_HBN_APB_CFG                                         GLB_HBN_APB_CFG
#define GLB_HBN_APB_CFG_POS                                     (24U)
#define GLB_HBN_APB_CFG_LEN                                     (8U)
#define GLB_HBN_APB_CFG_MSK                                     (((1U<<GLB_HBN_APB_CFG_LEN)-1)<<GLB_HBN_APB_CFG_POS)
#define GLB_HBN_APB_CFG_UMSK                                    (~(((1U<<GLB_HBN_APB_CFG_LEN)-1)<<GLB_HBN_APB_CFG_POS))

/* 0x64 : bmx_cfg2 */
#define GLB_BMX_CFG2_OFFSET                                     (0x64)
#define GLB_REG_INFRA_BERR_EN                                   GLB_REG_INFRA_BERR_EN
#define GLB_REG_INFRA_BERR_EN_POS                               (0U)
#define GLB_REG_INFRA_BERR_EN_LEN                               (12U)
#define GLB_REG_INFRA_BERR_EN_MSK                               (((1U<<GLB_REG_INFRA_BERR_EN_LEN)-1)<<GLB_REG_INFRA_BERR_EN_POS)
#define GLB_REG_INFRA_BERR_EN_UMSK                              (~(((1U<<GLB_REG_INFRA_BERR_EN_LEN)-1)<<GLB_REG_INFRA_BERR_EN_POS))
#define GLB_REG_INFRA_BERR_LAST                                 GLB_REG_INFRA_BERR_LAST
#define GLB_REG_INFRA_BERR_LAST_POS                             (14U)
#define GLB_REG_INFRA_BERR_LAST_LEN                             (1U)
#define GLB_REG_INFRA_BERR_LAST_MSK                             (((1U<<GLB_REG_INFRA_BERR_LAST_LEN)-1)<<GLB_REG_INFRA_BERR_LAST_POS)
#define GLB_REG_INFRA_BERR_LAST_UMSK                            (~(((1U<<GLB_REG_INFRA_BERR_LAST_LEN)-1)<<GLB_REG_INFRA_BERR_LAST_POS))
#define GLB_REG_INFRA_BERR_CLR                                  GLB_REG_INFRA_BERR_CLR
#define GLB_REG_INFRA_BERR_CLR_POS                              (15U)
#define GLB_REG_INFRA_BERR_CLR_LEN                              (1U)
#define GLB_REG_INFRA_BERR_CLR_MSK                              (((1U<<GLB_REG_INFRA_BERR_CLR_LEN)-1)<<GLB_REG_INFRA_BERR_CLR_POS)
#define GLB_REG_INFRA_BERR_CLR_UMSK                             (~(((1U<<GLB_REG_INFRA_BERR_CLR_LEN)-1)<<GLB_REG_INFRA_BERR_CLR_POS))
#define GLB_STS_INFRA_BERR_SRC                                  GLB_STS_INFRA_BERR_SRC
#define GLB_STS_INFRA_BERR_SRC_POS                              (16U)
#define GLB_STS_INFRA_BERR_SRC_LEN                              (12U)
#define GLB_STS_INFRA_BERR_SRC_MSK                              (((1U<<GLB_STS_INFRA_BERR_SRC_LEN)-1)<<GLB_STS_INFRA_BERR_SRC_POS)
#define GLB_STS_INFRA_BERR_SRC_UMSK                             (~(((1U<<GLB_STS_INFRA_BERR_SRC_LEN)-1)<<GLB_STS_INFRA_BERR_SRC_POS))
#define GLB_STS_INFRA_BERR_WRITE                                GLB_STS_INFRA_BERR_WRITE
#define GLB_STS_INFRA_BERR_WRITE_POS                            (30U)
#define GLB_STS_INFRA_BERR_WRITE_LEN                            (1U)
#define GLB_STS_INFRA_BERR_WRITE_MSK                            (((1U<<GLB_STS_INFRA_BERR_WRITE_LEN)-1)<<GLB_STS_INFRA_BERR_WRITE_POS)
#define GLB_STS_INFRA_BERR_WRITE_UMSK                           (~(((1U<<GLB_STS_INFRA_BERR_WRITE_LEN)-1)<<GLB_STS_INFRA_BERR_WRITE_POS))
#define GLB_STS_INFRA_BERR                                      GLB_STS_INFRA_BERR
#define GLB_STS_INFRA_BERR_POS                                  (31U)
#define GLB_STS_INFRA_BERR_LEN                                  (1U)
#define GLB_STS_INFRA_BERR_MSK                                  (((1U<<GLB_STS_INFRA_BERR_LEN)-1)<<GLB_STS_INFRA_BERR_POS)
#define GLB_STS_INFRA_BERR_UMSK                                 (~(((1U<<GLB_STS_INFRA_BERR_LEN)-1)<<GLB_STS_INFRA_BERR_POS))

/* 0x68 : bmx_err_addr */
#define GLB_BMX_ERR_ADDR_OFFSET                                 (0x68)
#define GLB_STS_INFRA_BERR_ADDR                                 GLB_STS_INFRA_BERR_ADDR
#define GLB_STS_INFRA_BERR_ADDR_POS                             (0U)
#define GLB_STS_INFRA_BERR_ADDR_LEN                             (32U)
#define GLB_STS_INFRA_BERR_ADDR_MSK                             (((1U<<GLB_STS_INFRA_BERR_ADDR_LEN)-1)<<GLB_STS_INFRA_BERR_ADDR_POS)
#define GLB_STS_INFRA_BERR_ADDR_UMSK                            (~(((1U<<GLB_STS_INFRA_BERR_ADDR_LEN)-1)<<GLB_STS_INFRA_BERR_ADDR_POS))

/* 0x6C : bmx_cfg3 */
#define GLB_BMX_CFG3_OFFSET                                     (0x6C)
#define GLB_REG_FRUN_PCLK                                       GLB_REG_FRUN_PCLK
#define GLB_REG_FRUN_PCLK_POS                                   (0U)
#define GLB_REG_FRUN_PCLK_LEN                                   (32U)
#define GLB_REG_FRUN_PCLK_MSK                                   (((1U<<GLB_REG_FRUN_PCLK_LEN)-1)<<GLB_REG_FRUN_PCLK_POS)
#define GLB_REG_FRUN_PCLK_UMSK                                  (~(((1U<<GLB_REG_FRUN_PCLK_LEN)-1)<<GLB_REG_FRUN_PCLK_POS))

/* 0x70 : rsv0 */
#define GLB_RSV0_OFFSET                                         (0x70)
#define GLB_REG_PROT_REQ_OPI                                    GLB_REG_PROT_REQ_OPI
#define GLB_REG_PROT_REQ_OPI_POS                                (1U)
#define GLB_REG_PROT_REQ_OPI_LEN                                (1U)
#define GLB_REG_PROT_REQ_OPI_MSK                                (((1U<<GLB_REG_PROT_REQ_OPI_LEN)-1)<<GLB_REG_PROT_REQ_OPI_POS)
#define GLB_REG_PROT_REQ_OPI_UMSK                               (~(((1U<<GLB_REG_PROT_REQ_OPI_LEN)-1)<<GLB_REG_PROT_REQ_OPI_POS))
#define GLB_STS_PROT_RDY_OPI                                    GLB_STS_PROT_RDY_OPI
#define GLB_STS_PROT_RDY_OPI_POS                                (5U)
#define GLB_STS_PROT_RDY_OPI_LEN                                (1U)
#define GLB_STS_PROT_RDY_OPI_MSK                                (((1U<<GLB_STS_PROT_RDY_OPI_LEN)-1)<<GLB_STS_PROT_RDY_OPI_POS)
#define GLB_STS_PROT_RDY_OPI_UMSK                               (~(((1U<<GLB_STS_PROT_RDY_OPI_LEN)-1)<<GLB_STS_PROT_RDY_OPI_POS))
#define GLB_REG_XASLI_W_ISSUE_THRE_X2SF                         GLB_REG_XASLI_W_ISSUE_THRE_X2SF
#define GLB_REG_XASLI_W_ISSUE_THRE_X2SF_POS                     (16U)
#define GLB_REG_XASLI_W_ISSUE_THRE_X2SF_LEN                     (2U)
#define GLB_REG_XASLI_W_ISSUE_THRE_X2SF_MSK                     (((1U<<GLB_REG_XASLI_W_ISSUE_THRE_X2SF_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_X2SF_POS)
#define GLB_REG_XASLI_W_ISSUE_THRE_X2SF_UMSK                    (~(((1U<<GLB_REG_XASLI_W_ISSUE_THRE_X2SF_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_X2SF_POS))
#define GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT                        GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT
#define GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_POS                    (20U)
#define GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_LEN                    (2U)
#define GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_MSK                    (((1U<<GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_LEN)-1)<<GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_POS)
#define GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_UMSK                   (~(((1U<<GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_LEN)-1)<<GLB_REG_HS2F_W_ISSUE_THRE_IH2EXT_POS))
#define GLB_REG_HS2F_RCMD_BP_EN_IH2EXT                          GLB_REG_HS2F_RCMD_BP_EN_IH2EXT
#define GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_POS                      (23U)
#define GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_LEN                      (1U)
#define GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_MSK                      (((1U<<GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_LEN)-1)<<GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_POS)
#define GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_UMSK                     (~(((1U<<GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_LEN)-1)<<GLB_REG_HS2F_RCMD_BP_EN_IH2EXT_POS))
#define GLB_REG_HASLI_W_ISSUE_THRE_H2SF                         GLB_REG_HASLI_W_ISSUE_THRE_H2SF
#define GLB_REG_HASLI_W_ISSUE_THRE_H2SF_POS                     (24U)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2SF_LEN                     (2U)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2SF_MSK                     (((1U<<GLB_REG_HASLI_W_ISSUE_THRE_H2SF_LEN)-1)<<GLB_REG_HASLI_W_ISSUE_THRE_H2SF_POS)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2SF_UMSK                    (~(((1U<<GLB_REG_HASLI_W_ISSUE_THRE_H2SF_LEN)-1)<<GLB_REG_HASLI_W_ISSUE_THRE_H2SF_POS))
#define GLB_REG_HASLI_W_ISSUE_THRE_H2OPI                        GLB_REG_HASLI_W_ISSUE_THRE_H2OPI
#define GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_POS                    (26U)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_LEN                    (2U)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_MSK                    (((1U<<GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_LEN)-1)<<GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_POS)
#define GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_UMSK                   (~(((1U<<GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_LEN)-1)<<GLB_REG_HASLI_W_ISSUE_THRE_H2OPI_POS))
#define GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT                       GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT
#define GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_POS                   (28U)
#define GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_LEN                   (2U)
#define GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_MSK                   (((1U<<GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_POS)
#define GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_UMSK                  (~(((1U<<GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_IX2EXT_POS))
#define GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT                     GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT
#define GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_POS                 (30U)
#define GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_LEN                 (2U)
#define GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_MSK                 (((1U<<GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_POS)
#define GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_UMSK                (~(((1U<<GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_LEN)-1)<<GLB_REG_XASLI_W_ISSUE_THRE_IPROAI2EXT_POS))

/* 0x74 : rsv1 */
#define GLB_RSV1_OFFSET                                         (0x74)
#define GLB_CR_SNSR_RESET                                       GLB_CR_SNSR_RESET
#define GLB_CR_SNSR_RESET_POS                                   (0U)
#define GLB_CR_SNSR_RESET_LEN                                   (1U)
#define GLB_CR_SNSR_RESET_MSK                                   (((1U<<GLB_CR_SNSR_RESET_LEN)-1)<<GLB_CR_SNSR_RESET_POS)
#define GLB_CR_SNSR_RESET_UMSK                                  (~(((1U<<GLB_CR_SNSR_RESET_LEN)-1)<<GLB_CR_SNSR_RESET_POS))
#define GLB_CR_SNSR_PWDN                                        GLB_CR_SNSR_PWDN
#define GLB_CR_SNSR_PWDN_POS                                    (1U)
#define GLB_CR_SNSR_PWDN_LEN                                    (1U)
#define GLB_CR_SNSR_PWDN_MSK                                    (((1U<<GLB_CR_SNSR_PWDN_LEN)-1)<<GLB_CR_SNSR_PWDN_POS)
#define GLB_CR_SNSR_PWDN_UMSK                                   (~(((1U<<GLB_CR_SNSR_PWDN_LEN)-1)<<GLB_CR_SNSR_PWDN_POS))

/* 0x78 : rsv2 */
#define GLB_RSV2_OFFSET                                         (0x78)

/* 0x7C : rsv3 */
#define GLB_RSV3_OFFSET                                         (0x7C)

/* 0x84 : ocram_parm */
#define GLB_OCRAM_PARM_OFFSET                                   (0x84)
#define GLB_REG_SRAM_PARM2_MM                                   GLB_REG_SRAM_PARM2_MM
#define GLB_REG_SRAM_PARM2_MM_POS                               (0U)
#define GLB_REG_SRAM_PARM2_MM_LEN                               (5U)
#define GLB_REG_SRAM_PARM2_MM_MSK                               (((1U<<GLB_REG_SRAM_PARM2_MM_LEN)-1)<<GLB_REG_SRAM_PARM2_MM_POS)
#define GLB_REG_SRAM_PARM2_MM_UMSK                              (~(((1U<<GLB_REG_SRAM_PARM2_MM_LEN)-1)<<GLB_REG_SRAM_PARM2_MM_POS))
#define GLB_REG_SRAM_PARM1                                      GLB_REG_SRAM_PARM1
#define GLB_REG_SRAM_PARM1_POS                                  (12U)
#define GLB_REG_SRAM_PARM1_LEN                                  (19U)
#define GLB_REG_SRAM_PARM1_MSK                                  (((1U<<GLB_REG_SRAM_PARM1_LEN)-1)<<GLB_REG_SRAM_PARM1_POS)
#define GLB_REG_SRAM_PARM1_UMSK                                 (~(((1U<<GLB_REG_SRAM_PARM1_LEN)-1)<<GLB_REG_SRAM_PARM1_POS))

/* 0x88 : sram_parm */
#define GLB_SRAM_PARM_OFFSET                                    (0x88)
#define GLB_BZ_RAM_RET1                                         GLB_BZ_RAM_RET1
#define GLB_BZ_RAM_RET1_POS                                     (16U)
#define GLB_BZ_RAM_RET1_LEN                                     (1U)
#define GLB_BZ_RAM_RET1_MSK                                     (((1U<<GLB_BZ_RAM_RET1_LEN)-1)<<GLB_BZ_RAM_RET1_POS)
#define GLB_BZ_RAM_RET1_UMSK                                    (~(((1U<<GLB_BZ_RAM_RET1_LEN)-1)<<GLB_BZ_RAM_RET1_POS))
#define GLB_BZ_RAM_RET2                                         GLB_BZ_RAM_RET2
#define GLB_BZ_RAM_RET2_POS                                     (17U)
#define GLB_BZ_RAM_RET2_LEN                                     (1U)
#define GLB_BZ_RAM_RET2_MSK                                     (((1U<<GLB_BZ_RAM_RET2_LEN)-1)<<GLB_BZ_RAM_RET2_POS)
#define GLB_BZ_RAM_RET2_UMSK                                    (~(((1U<<GLB_BZ_RAM_RET2_LEN)-1)<<GLB_BZ_RAM_RET2_POS))
#define GLB_BZ_RAM_PGEN                                         GLB_BZ_RAM_PGEN
#define GLB_BZ_RAM_PGEN_POS                                     (18U)
#define GLB_BZ_RAM_PGEN_LEN                                     (1U)
#define GLB_BZ_RAM_PGEN_MSK                                     (((1U<<GLB_BZ_RAM_PGEN_LEN)-1)<<GLB_BZ_RAM_PGEN_POS)
#define GLB_BZ_RAM_PGEN_UMSK                                    (~(((1U<<GLB_BZ_RAM_PGEN_LEN)-1)<<GLB_BZ_RAM_PGEN_POS))
#define GLB_TOP_RAM_RET1                                        GLB_TOP_RAM_RET1
#define GLB_TOP_RAM_RET1_POS                                    (19U)
#define GLB_TOP_RAM_RET1_LEN                                    (1U)
#define GLB_TOP_RAM_RET1_MSK                                    (((1U<<GLB_TOP_RAM_RET1_LEN)-1)<<GLB_TOP_RAM_RET1_POS)
#define GLB_TOP_RAM_RET1_UMSK                                   (~(((1U<<GLB_TOP_RAM_RET1_LEN)-1)<<GLB_TOP_RAM_RET1_POS))
#define GLB_TOP_RAM_RET2                                        GLB_TOP_RAM_RET2
#define GLB_TOP_RAM_RET2_POS                                    (20U)
#define GLB_TOP_RAM_RET2_LEN                                    (1U)
#define GLB_TOP_RAM_RET2_MSK                                    (((1U<<GLB_TOP_RAM_RET2_LEN)-1)<<GLB_TOP_RAM_RET2_POS)
#define GLB_TOP_RAM_RET2_UMSK                                   (~(((1U<<GLB_TOP_RAM_RET2_LEN)-1)<<GLB_TOP_RAM_RET2_POS))
#define GLB_TOP_RAM_PGEN                                        GLB_TOP_RAM_PGEN
#define GLB_TOP_RAM_PGEN_POS                                    (21U)
#define GLB_TOP_RAM_PGEN_LEN                                    (1U)
#define GLB_TOP_RAM_PGEN_MSK                                    (((1U<<GLB_TOP_RAM_PGEN_LEN)-1)<<GLB_TOP_RAM_PGEN_POS)
#define GLB_TOP_RAM_PGEN_UMSK                                   (~(((1U<<GLB_TOP_RAM_PGEN_LEN)-1)<<GLB_TOP_RAM_PGEN_POS))
#define GLB_CPU_RAM_RET1                                        GLB_CPU_RAM_RET1
#define GLB_CPU_RAM_RET1_POS                                    (22U)
#define GLB_CPU_RAM_RET1_LEN                                    (1U)
#define GLB_CPU_RAM_RET1_MSK                                    (((1U<<GLB_CPU_RAM_RET1_LEN)-1)<<GLB_CPU_RAM_RET1_POS)
#define GLB_CPU_RAM_RET1_UMSK                                   (~(((1U<<GLB_CPU_RAM_RET1_LEN)-1)<<GLB_CPU_RAM_RET1_POS))
#define GLB_CPU_RAM_RET2                                        GLB_CPU_RAM_RET2
#define GLB_CPU_RAM_RET2_POS                                    (23U)
#define GLB_CPU_RAM_RET2_LEN                                    (1U)
#define GLB_CPU_RAM_RET2_MSK                                    (((1U<<GLB_CPU_RAM_RET2_LEN)-1)<<GLB_CPU_RAM_RET2_POS)
#define GLB_CPU_RAM_RET2_UMSK                                   (~(((1U<<GLB_CPU_RAM_RET2_LEN)-1)<<GLB_CPU_RAM_RET2_POS))
#define GLB_CPU_RAM_PGEN                                        GLB_CPU_RAM_PGEN
#define GLB_CPU_RAM_PGEN_POS                                    (24U)
#define GLB_CPU_RAM_PGEN_LEN                                    (1U)
#define GLB_CPU_RAM_PGEN_MSK                                    (((1U<<GLB_CPU_RAM_PGEN_LEN)-1)<<GLB_CPU_RAM_PGEN_POS)
#define GLB_CPU_RAM_PGEN_UMSK                                   (~(((1U<<GLB_CPU_RAM_PGEN_LEN)-1)<<GLB_CPU_RAM_PGEN_POS))
#define GLB_MM_RAM_RET1                                         GLB_MM_RAM_RET1
#define GLB_MM_RAM_RET1_POS                                     (25U)
#define GLB_MM_RAM_RET1_LEN                                     (1U)
#define GLB_MM_RAM_RET1_MSK                                     (((1U<<GLB_MM_RAM_RET1_LEN)-1)<<GLB_MM_RAM_RET1_POS)
#define GLB_MM_RAM_RET1_UMSK                                    (~(((1U<<GLB_MM_RAM_RET1_LEN)-1)<<GLB_MM_RAM_RET1_POS))
#define GLB_MM_RAM_RET2                                         GLB_MM_RAM_RET2
#define GLB_MM_RAM_RET2_POS                                     (26U)
#define GLB_MM_RAM_RET2_LEN                                     (1U)
#define GLB_MM_RAM_RET2_MSK                                     (((1U<<GLB_MM_RAM_RET2_LEN)-1)<<GLB_MM_RAM_RET2_POS)
#define GLB_MM_RAM_RET2_UMSK                                    (~(((1U<<GLB_MM_RAM_RET2_LEN)-1)<<GLB_MM_RAM_RET2_POS))
#define GLB_MM_RAM_PGEN                                         GLB_MM_RAM_PGEN
#define GLB_MM_RAM_PGEN_POS                                     (27U)
#define GLB_MM_RAM_PGEN_LEN                                     (1U)
#define GLB_MM_RAM_PGEN_MSK                                     (((1U<<GLB_MM_RAM_PGEN_LEN)-1)<<GLB_MM_RAM_PGEN_POS)
#define GLB_MM_RAM_PGEN_UMSK                                    (~(((1U<<GLB_MM_RAM_PGEN_LEN)-1)<<GLB_MM_RAM_PGEN_POS))

/* 0x8C : sram_misc */
#define GLB_SRAM_MISC_OFFSET                                    (0x8C)
#define GLB_CR_EM_SEL                                           GLB_CR_EM_SEL
#define GLB_CR_EM_SEL_POS                                       (0U)
#define GLB_CR_EM_SEL_LEN                                       (4U)
#define GLB_CR_EM_SEL_MSK                                       (((1U<<GLB_CR_EM_SEL_LEN)-1)<<GLB_CR_EM_SEL_POS)
#define GLB_CR_EM_SEL_UMSK                                      (~(((1U<<GLB_CR_EM_SEL_LEN)-1)<<GLB_CR_EM_SEL_POS))
#define GLB_CR_IPROAI_EN                                          GLB_CR_IPROAI_EN
#define GLB_CR_IPROAI_EN_POS                                      (8U)
#define GLB_CR_IPROAI_EN_LEN                                      (1U)
#define GLB_CR_IPROAI_EN_MSK                                      (((1U<<GLB_CR_IPROAI_EN_LEN)-1)<<GLB_CR_IPROAI_EN_POS)
#define GLB_CR_IPROAI_EN_UMSK                                     (~(((1U<<GLB_CR_IPROAI_EN_LEN)-1)<<GLB_CR_IPROAI_EN_POS))

/* 0x90 : glb_parm */
#define GLB_PARM_OFFSET                                         (0x90)
#define GLB_JTAG_SWAP_SET                                       GLB_JTAG_SWAP_SET
#define GLB_JTAG_SWAP_SET_POS                                   (0U)
#define GLB_JTAG_SWAP_SET_LEN                                   (8U)
#define GLB_JTAG_SWAP_SET_MSK                                   (((1U<<GLB_JTAG_SWAP_SET_LEN)-1)<<GLB_JTAG_SWAP_SET_POS)
#define GLB_JTAG_SWAP_SET_UMSK                                  (~(((1U<<GLB_JTAG_SWAP_SET_LEN)-1)<<GLB_JTAG_SWAP_SET_POS))
#define GLB_CFG_SFLASH_SWAP_IO0_IO3                             GLB_CFG_SFLASH_SWAP_IO0_IO3
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_POS                         (8U)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN                         (1U)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_MSK                         (((1U<<GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN)-1)<<GLB_CFG_SFLASH_SWAP_IO0_IO3_POS)
#define GLB_CFG_SFLASH_SWAP_IO0_IO3_UMSK                        (~(((1U<<GLB_CFG_SFLASH_SWAP_IO0_IO3_LEN)-1)<<GLB_CFG_SFLASH_SWAP_IO0_IO3_POS))
#define GLB_CFG_SFLASH_SWAP_CS_IO2                              GLB_CFG_SFLASH_SWAP_CS_IO2
#define GLB_CFG_SFLASH_SWAP_CS_IO2_POS                          (9U)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_LEN                          (1U)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_MSK                          (((1U<<GLB_CFG_SFLASH_SWAP_CS_IO2_LEN)-1)<<GLB_CFG_SFLASH_SWAP_CS_IO2_POS)
#define GLB_CFG_SFLASH_SWAP_CS_IO2_UMSK                         (~(((1U<<GLB_CFG_SFLASH_SWAP_CS_IO2_LEN)-1)<<GLB_CFG_SFLASH_SWAP_CS_IO2_POS))
#define GLB_CFG_SFLASH_REVERSE                                  GLB_CFG_SFLASH_REVERSE
#define GLB_CFG_SFLASH_REVERSE_POS                              (10U)
#define GLB_CFG_SFLASH_REVERSE_LEN                              (1U)
#define GLB_CFG_SFLASH_REVERSE_MSK                              (((1U<<GLB_CFG_SFLASH_REVERSE_LEN)-1)<<GLB_CFG_SFLASH_REVERSE_POS)
#define GLB_CFG_SFLASH_REVERSE_UMSK                             (~(((1U<<GLB_CFG_SFLASH_REVERSE_LEN)-1)<<GLB_CFG_SFLASH_REVERSE_POS))
#define GLB_CFG_FLASH2_SCENARIO                                 GLB_CFG_FLASH2_SCENARIO
#define GLB_CFG_FLASH2_SCENARIO_POS                             (11U)
#define GLB_CFG_FLASH2_SCENARIO_LEN                             (1U)
#define GLB_CFG_FLASH2_SCENARIO_MSK                             (((1U<<GLB_CFG_FLASH2_SCENARIO_LEN)-1)<<GLB_CFG_FLASH2_SCENARIO_POS)
#define GLB_CFG_FLASH2_SCENARIO_UMSK                            (~(((1U<<GLB_CFG_FLASH2_SCENARIO_LEN)-1)<<GLB_CFG_FLASH2_SCENARIO_POS))
#define GLB_REG_SPI_0_MASTER_MODE                               GLB_REG_SPI_0_MASTER_MODE
#define GLB_REG_SPI_0_MASTER_MODE_POS                           (12U)
#define GLB_REG_SPI_0_MASTER_MODE_LEN                           (1U)
#define GLB_REG_SPI_0_MASTER_MODE_MSK                           (((1U<<GLB_REG_SPI_0_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_0_MASTER_MODE_POS)
#define GLB_REG_SPI_0_MASTER_MODE_UMSK                          (~(((1U<<GLB_REG_SPI_0_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_0_MASTER_MODE_POS))
#define GLB_REG_SPI_0_SWAP                                      GLB_REG_SPI_0_SWAP
#define GLB_REG_SPI_0_SWAP_POS                                  (13U)
#define GLB_REG_SPI_0_SWAP_LEN                                  (1U)
#define GLB_REG_SPI_0_SWAP_MSK                                  (((1U<<GLB_REG_SPI_0_SWAP_LEN)-1)<<GLB_REG_SPI_0_SWAP_POS)
#define GLB_REG_SPI_0_SWAP_UMSK                                 (~(((1U<<GLB_REG_SPI_0_SWAP_LEN)-1)<<GLB_REG_SPI_0_SWAP_POS))
#define GLB_REG_CCI_USE_JTAG_PIN                                GLB_REG_CCI_USE_JTAG_PIN
#define GLB_REG_CCI_USE_JTAG_PIN_POS                            (16U)
#define GLB_REG_CCI_USE_JTAG_PIN_LEN                            (1U)
#define GLB_REG_CCI_USE_JTAG_PIN_MSK                            (((1U<<GLB_REG_CCI_USE_JTAG_PIN_LEN)-1)<<GLB_REG_CCI_USE_JTAG_PIN_POS)
#define GLB_REG_CCI_USE_JTAG_PIN_UMSK                           (~(((1U<<GLB_REG_CCI_USE_JTAG_PIN_LEN)-1)<<GLB_REG_CCI_USE_JTAG_PIN_POS))
#define GLB_P1_ADC_TEST_WITH_CCI                                GLB_P1_ADC_TEST_WITH_CCI
#define GLB_P1_ADC_TEST_WITH_CCI_POS                            (17U)
#define GLB_P1_ADC_TEST_WITH_CCI_LEN                            (1U)
#define GLB_P1_ADC_TEST_WITH_CCI_MSK                            (((1U<<GLB_P1_ADC_TEST_WITH_CCI_LEN)-1)<<GLB_P1_ADC_TEST_WITH_CCI_POS)
#define GLB_P1_ADC_TEST_WITH_CCI_UMSK                           (~(((1U<<GLB_P1_ADC_TEST_WITH_CCI_LEN)-1)<<GLB_P1_ADC_TEST_WITH_CCI_POS))
#define GLB_P2_DAC_TEST_WITH_CCI                                GLB_P2_DAC_TEST_WITH_CCI
#define GLB_P2_DAC_TEST_WITH_CCI_POS                            (18U)
#define GLB_P2_DAC_TEST_WITH_CCI_LEN                            (1U)
#define GLB_P2_DAC_TEST_WITH_CCI_MSK                            (((1U<<GLB_P2_DAC_TEST_WITH_CCI_LEN)-1)<<GLB_P2_DAC_TEST_WITH_CCI_POS)
#define GLB_P2_DAC_TEST_WITH_CCI_UMSK                           (~(((1U<<GLB_P2_DAC_TEST_WITH_CCI_LEN)-1)<<GLB_P2_DAC_TEST_WITH_CCI_POS))
#define GLB_P3_CCI_USE_IO_0_2_7                                 GLB_P3_CCI_USE_IO_0_2_7
#define GLB_P3_CCI_USE_IO_0_2_7_POS                             (19U)
#define GLB_P3_CCI_USE_IO_0_2_7_LEN                             (1U)
#define GLB_P3_CCI_USE_IO_0_2_7_MSK                             (((1U<<GLB_P3_CCI_USE_IO_0_2_7_LEN)-1)<<GLB_P3_CCI_USE_IO_0_2_7_POS)
#define GLB_P3_CCI_USE_IO_0_2_7_UMSK                            (~(((1U<<GLB_P3_CCI_USE_IO_0_2_7_LEN)-1)<<GLB_P3_CCI_USE_IO_0_2_7_POS))
#define GLB_P4_ADC_TEST_WITH_JTAG                               GLB_P4_ADC_TEST_WITH_JTAG
#define GLB_P4_ADC_TEST_WITH_JTAG_POS                           (20U)
#define GLB_P4_ADC_TEST_WITH_JTAG_LEN                           (1U)
#define GLB_P4_ADC_TEST_WITH_JTAG_MSK                           (((1U<<GLB_P4_ADC_TEST_WITH_JTAG_LEN)-1)<<GLB_P4_ADC_TEST_WITH_JTAG_POS)
#define GLB_P4_ADC_TEST_WITH_JTAG_UMSK                          (~(((1U<<GLB_P4_ADC_TEST_WITH_JTAG_LEN)-1)<<GLB_P4_ADC_TEST_WITH_JTAG_POS))
#define GLB_P5_DAC_TEST_WITH_JTAG                               GLB_P5_DAC_TEST_WITH_JTAG
#define GLB_P5_DAC_TEST_WITH_JTAG_POS                           (21U)
#define GLB_P5_DAC_TEST_WITH_JTAG_LEN                           (1U)
#define GLB_P5_DAC_TEST_WITH_JTAG_MSK                           (((1U<<GLB_P5_DAC_TEST_WITH_JTAG_LEN)-1)<<GLB_P5_DAC_TEST_WITH_JTAG_POS)
#define GLB_P5_DAC_TEST_WITH_JTAG_UMSK                          (~(((1U<<GLB_P5_DAC_TEST_WITH_JTAG_LEN)-1)<<GLB_P5_DAC_TEST_WITH_JTAG_POS))
#define GLB_P6_JTAG_USE_IO_0_2_7                                GLB_P6_JTAG_USE_IO_0_2_7
#define GLB_P6_JTAG_USE_IO_0_2_7_POS                            (23U)
#define GLB_P6_JTAG_USE_IO_0_2_7_LEN                            (1U)
#define GLB_P6_JTAG_USE_IO_0_2_7_MSK                            (((1U<<GLB_P6_JTAG_USE_IO_0_2_7_LEN)-1)<<GLB_P6_JTAG_USE_IO_0_2_7_POS)
#define GLB_P6_JTAG_USE_IO_0_2_7_UMSK                           (~(((1U<<GLB_P6_JTAG_USE_IO_0_2_7_LEN)-1)<<GLB_P6_JTAG_USE_IO_0_2_7_POS))
#define GLB_REG_EN_GPIO_O_LATCH_MODE                            GLB_REG_EN_GPIO_O_LATCH_MODE
#define GLB_REG_EN_GPIO_O_LATCH_MODE_POS                        (28U)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_LEN                        (1U)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_MSK                        (((1U<<GLB_REG_EN_GPIO_O_LATCH_MODE_LEN)-1)<<GLB_REG_EN_GPIO_O_LATCH_MODE_POS)
#define GLB_REG_EN_GPIO_O_LATCH_MODE_UMSK                       (~(((1U<<GLB_REG_EN_GPIO_O_LATCH_MODE_LEN)-1)<<GLB_REG_EN_GPIO_O_LATCH_MODE_POS))
#define GLB_REG_KYS_DRV_VAL                                     GLB_REG_KYS_DRV_VAL
#define GLB_REG_KYS_DRV_VAL_POS                                 (29U)
#define GLB_REG_KYS_DRV_VAL_LEN                                 (1U)
#define GLB_REG_KYS_DRV_VAL_MSK                                 (((1U<<GLB_REG_KYS_DRV_VAL_LEN)-1)<<GLB_REG_KYS_DRV_VAL_POS)
#define GLB_REG_KYS_DRV_VAL_UMSK                                (~(((1U<<GLB_REG_KYS_DRV_VAL_LEN)-1)<<GLB_REG_KYS_DRV_VAL_POS))
#define GLB_REG_SPI_1_MASTER_MODE                               GLB_REG_SPI_1_MASTER_MODE
#define GLB_REG_SPI_1_MASTER_MODE_POS                           (30U)
#define GLB_REG_SPI_1_MASTER_MODE_LEN                           (1U)
#define GLB_REG_SPI_1_MASTER_MODE_MSK                           (((1U<<GLB_REG_SPI_1_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_1_MASTER_MODE_POS)
#define GLB_REG_SPI_1_MASTER_MODE_UMSK                          (~(((1U<<GLB_REG_SPI_1_MASTER_MODE_LEN)-1)<<GLB_REG_SPI_1_MASTER_MODE_POS))
#define GLB_REG_SPI_1_SWAP                                      GLB_REG_SPI_1_SWAP
#define GLB_REG_SPI_1_SWAP_POS                                  (31U)
#define GLB_REG_SPI_1_SWAP_LEN                                  (1U)
#define GLB_REG_SPI_1_SWAP_MSK                                  (((1U<<GLB_REG_SPI_1_SWAP_LEN)-1)<<GLB_REG_SPI_1_SWAP_POS)
#define GLB_REG_SPI_1_SWAP_UMSK                                 (~(((1U<<GLB_REG_SPI_1_SWAP_LEN)-1)<<GLB_REG_SPI_1_SWAP_POS))

/* 0x94 : PDM_CLK_CTRL */
#define GLB_PDM_CLK_CTRL_OFFSET                                 (0x94)

/* 0x98 : GPIO_USE_PSRAM__IO */
#define GLB_GPIO_USE_PSRAM__IO_OFFSET                           (0x98)

/* 0xA0 : CPU_CLK_CFG */
#define GLB_CPU_CLK_CFG_OFFSET                                  (0xA0)
#define GLB_CPU_RTC_DIV                                         GLB_CPU_RTC_DIV
#define GLB_CPU_RTC_DIV_POS                                     (0U)
#define GLB_CPU_RTC_DIV_LEN                                     (17U)
#define GLB_CPU_RTC_DIV_MSK                                     (((1U<<GLB_CPU_RTC_DIV_LEN)-1)<<GLB_CPU_RTC_DIV_POS)
#define GLB_CPU_RTC_DIV_UMSK                                    (~(((1U<<GLB_CPU_RTC_DIV_LEN)-1)<<GLB_CPU_RTC_DIV_POS))
#define GLB_CPU_RTC_EN                                          GLB_CPU_RTC_EN
#define GLB_CPU_RTC_EN_POS                                      (18U)
#define GLB_CPU_RTC_EN_LEN                                      (1U)
#define GLB_CPU_RTC_EN_MSK                                      (((1U<<GLB_CPU_RTC_EN_LEN)-1)<<GLB_CPU_RTC_EN_POS)
#define GLB_CPU_RTC_EN_UMSK                                     (~(((1U<<GLB_CPU_RTC_EN_LEN)-1)<<GLB_CPU_RTC_EN_POS))
#define GLB_CPU_RTC_SEL                                         GLB_CPU_RTC_SEL
#define GLB_CPU_RTC_SEL_POS                                     (19U)
#define GLB_CPU_RTC_SEL_LEN                                     (1U)
#define GLB_CPU_RTC_SEL_MSK                                     (((1U<<GLB_CPU_RTC_SEL_LEN)-1)<<GLB_CPU_RTC_SEL_POS)
#define GLB_CPU_RTC_SEL_UMSK                                    (~(((1U<<GLB_CPU_RTC_SEL_LEN)-1)<<GLB_CPU_RTC_SEL_POS))
#define GLB_DEBUG_NDRESET_GATE                                  GLB_DEBUG_NDRESET_GATE
#define GLB_DEBUG_NDRESET_GATE_POS                              (20U)
#define GLB_DEBUG_NDRESET_GATE_LEN                              (1U)
#define GLB_DEBUG_NDRESET_GATE_MSK                              (((1U<<GLB_DEBUG_NDRESET_GATE_LEN)-1)<<GLB_DEBUG_NDRESET_GATE_POS)
#define GLB_DEBUG_NDRESET_GATE_UMSK                             (~(((1U<<GLB_DEBUG_NDRESET_GATE_LEN)-1)<<GLB_DEBUG_NDRESET_GATE_POS))

/* 0xA4 : CLK_DESPUR_ROOT_CLK_CFG */
#define GLB_CLK_DESPUR_ROOT_CLK_CFG_OFFSET                      (0xA4)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK                            GLB_CFG_DESPUR_BMAP_ROOT_CLK
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS                        (0U)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN                        (28U)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_MSK                        (((1U<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_BMAP_ROOT_CLK_UMSK                       (~(((1U<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_BMAP_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK                            GLB_CFG_DESPUR_CGEN_ROOT_CLK
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS                        (28U)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN                        (1U)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_MSK                        (((1U<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_CGEN_ROOT_CLK_UMSK                       (~(((1U<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CGEN_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK                          GLB_CFG_DESPUR_ENABLE_ROOT_CLK
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS                      (29U)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN                      (1U)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_MSK                      (((1U<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_ENABLE_ROOT_CLK_UMSK                     (~(((1U<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_ROOT_CLK_POS))
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK                         GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS                     (30U)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN                     (1U)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_MSK                     (((1U<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS)
#define GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_UMSK                    (~(((1U<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_ROOT_CLK_POS))

/* 0xA8 : CLK_DESPUR_CLK_SF_CFG */
#define GLB_CLK_DESPUR_CLK_SF_CFG_OFFSET                        (0xA8)
#define GLB_CFG_DESPUR_BMAP_CLK_SF                              GLB_CFG_DESPUR_BMAP_CLK_SF
#define GLB_CFG_DESPUR_BMAP_CLK_SF_POS                          (0U)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_LEN                          (28U)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_MSK                          (((1U<<GLB_CFG_DESPUR_BMAP_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_BMAP_CLK_SF_POS)
#define GLB_CFG_DESPUR_BMAP_CLK_SF_UMSK                         (~(((1U<<GLB_CFG_DESPUR_BMAP_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_BMAP_CLK_SF_POS))
#define GLB_CFG_DESPUR_CGEN_CLK_SF                              GLB_CFG_DESPUR_CGEN_CLK_SF
#define GLB_CFG_DESPUR_CGEN_CLK_SF_POS                          (28U)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_LEN                          (1U)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_MSK                          (((1U<<GLB_CFG_DESPUR_CGEN_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CGEN_CLK_SF_POS)
#define GLB_CFG_DESPUR_CGEN_CLK_SF_UMSK                         (~(((1U<<GLB_CFG_DESPUR_CGEN_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CGEN_CLK_SF_POS))
#define GLB_CFG_DESPUR_ENABLE_CLK_SF                            GLB_CFG_DESPUR_ENABLE_CLK_SF
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_POS                        (29U)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN                        (1U)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_MSK                        (((1U<<GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_CLK_SF_POS)
#define GLB_CFG_DESPUR_ENABLE_CLK_SF_UMSK                       (~(((1U<<GLB_CFG_DESPUR_ENABLE_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_ENABLE_CLK_SF_POS))
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF                           GLB_CFG_DESPUR_CLK_SEL_CLK_SF
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS                       (30U)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN                       (1U)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_MSK                       (((1U<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS)
#define GLB_CFG_DESPUR_CLK_SEL_CLK_SF_UMSK                      (~(((1U<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_LEN)-1)<<GLB_CFG_DESPUR_CLK_SEL_CLK_SF_POS))

/* 0xAC : WIFI_BT_COEX_CTRL */
#define GLB_WIFI_BT_COEX_CTRL_OFFSET                            (0xAC)

/* 0xB4 : GPADC_32M_SRC_CTRL */
#define GLB_GPADC_32M_SRC_CTRL_OFFSET                           (0xB4)
#define GLB_GPADC_32M_CLK_DIV                                   GLB_GPADC_32M_CLK_DIV
#define GLB_GPADC_32M_CLK_DIV_POS                               (0U)
#define GLB_GPADC_32M_CLK_DIV_LEN                               (6U)
#define GLB_GPADC_32M_CLK_DIV_MSK                               (((1U<<GLB_GPADC_32M_CLK_DIV_LEN)-1)<<GLB_GPADC_32M_CLK_DIV_POS)
#define GLB_GPADC_32M_CLK_DIV_UMSK                              (~(((1U<<GLB_GPADC_32M_CLK_DIV_LEN)-1)<<GLB_GPADC_32M_CLK_DIV_POS))
#define GLB_GPADC_32M_CLK_SEL                                   GLB_GPADC_32M_CLK_SEL
#define GLB_GPADC_32M_CLK_SEL_POS                               (7U)
#define GLB_GPADC_32M_CLK_SEL_LEN                               (1U)
#define GLB_GPADC_32M_CLK_SEL_MSK                               (((1U<<GLB_GPADC_32M_CLK_SEL_LEN)-1)<<GLB_GPADC_32M_CLK_SEL_POS)
#define GLB_GPADC_32M_CLK_SEL_UMSK                              (~(((1U<<GLB_GPADC_32M_CLK_SEL_LEN)-1)<<GLB_GPADC_32M_CLK_SEL_POS))
#define GLB_GPADC_32M_DIV_EN                                    GLB_GPADC_32M_DIV_EN
#define GLB_GPADC_32M_DIV_EN_POS                                (8U)
#define GLB_GPADC_32M_DIV_EN_LEN                                (1U)
#define GLB_GPADC_32M_DIV_EN_MSK                                (((1U<<GLB_GPADC_32M_DIV_EN_LEN)-1)<<GLB_GPADC_32M_DIV_EN_POS)
#define GLB_GPADC_32M_DIV_EN_UMSK                               (~(((1U<<GLB_GPADC_32M_DIV_EN_LEN)-1)<<GLB_GPADC_32M_DIV_EN_POS))

/* 0xB8 : DIG32K_WAKEUP_CTRL */
#define GLB_DIG32K_WAKEUP_CTRL_OFFSET                           (0xB8)
#define GLB_DIG_32K_DIV                                         GLB_DIG_32K_DIV
#define GLB_DIG_32K_DIV_POS                                     (0U)
#define GLB_DIG_32K_DIV_LEN                                     (11U)
#define GLB_DIG_32K_DIV_MSK                                     (((1U<<GLB_DIG_32K_DIV_LEN)-1)<<GLB_DIG_32K_DIV_POS)
#define GLB_DIG_32K_DIV_UMSK                                    (~(((1U<<GLB_DIG_32K_DIV_LEN)-1)<<GLB_DIG_32K_DIV_POS))
#define GLB_DIG_32K_EN                                          GLB_DIG_32K_EN
#define GLB_DIG_32K_EN_POS                                      (12U)
#define GLB_DIG_32K_EN_LEN                                      (1U)
#define GLB_DIG_32K_EN_MSK                                      (((1U<<GLB_DIG_32K_EN_LEN)-1)<<GLB_DIG_32K_EN_POS)
#define GLB_DIG_32K_EN_UMSK                                     (~(((1U<<GLB_DIG_32K_EN_LEN)-1)<<GLB_DIG_32K_EN_POS))
#define GLB_DIG_32K_COMP                                        GLB_DIG_32K_COMP
#define GLB_DIG_32K_COMP_POS                                    (13U)
#define GLB_DIG_32K_COMP_LEN                                    (1U)
#define GLB_DIG_32K_COMP_MSK                                    (((1U<<GLB_DIG_32K_COMP_LEN)-1)<<GLB_DIG_32K_COMP_POS)
#define GLB_DIG_32K_COMP_UMSK                                   (~(((1U<<GLB_DIG_32K_COMP_LEN)-1)<<GLB_DIG_32K_COMP_POS))
#define GLB_DIG_512K_DIV                                        GLB_DIG_512K_DIV
#define GLB_DIG_512K_DIV_POS                                    (16U)
#define GLB_DIG_512K_DIV_LEN                                    (7U)
#define GLB_DIG_512K_DIV_MSK                                    (((1U<<GLB_DIG_512K_DIV_LEN)-1)<<GLB_DIG_512K_DIV_POS)
#define GLB_DIG_512K_DIV_UMSK                                   (~(((1U<<GLB_DIG_512K_DIV_LEN)-1)<<GLB_DIG_512K_DIV_POS))
#define GLB_DIG_512K_EN                                         GLB_DIG_512K_EN
#define GLB_DIG_512K_EN_POS                                     (24U)
#define GLB_DIG_512K_EN_LEN                                     (1U)
#define GLB_DIG_512K_EN_MSK                                     (((1U<<GLB_DIG_512K_EN_LEN)-1)<<GLB_DIG_512K_EN_POS)
#define GLB_DIG_512K_EN_UMSK                                    (~(((1U<<GLB_DIG_512K_EN_LEN)-1)<<GLB_DIG_512K_EN_POS))
#define GLB_DIG_512K_COMP                                       GLB_DIG_512K_COMP
#define GLB_DIG_512K_COMP_POS                                   (25U)
#define GLB_DIG_512K_COMP_LEN                                   (1U)
#define GLB_DIG_512K_COMP_MSK                                   (((1U<<GLB_DIG_512K_COMP_LEN)-1)<<GLB_DIG_512K_COMP_POS)
#define GLB_DIG_512K_COMP_UMSK                                  (~(((1U<<GLB_DIG_512K_COMP_LEN)-1)<<GLB_DIG_512K_COMP_POS))
#define GLB_DIG_CLK_SRC_SEL                                     GLB_DIG_CLK_SRC_SEL
#define GLB_DIG_CLK_SRC_SEL_POS                                 (28U)
#define GLB_DIG_CLK_SRC_SEL_LEN                                 (1U)
#define GLB_DIG_CLK_SRC_SEL_MSK                                 (((1U<<GLB_DIG_CLK_SRC_SEL_LEN)-1)<<GLB_DIG_CLK_SRC_SEL_POS)
#define GLB_DIG_CLK_SRC_SEL_UMSK                                (~(((1U<<GLB_DIG_CLK_SRC_SEL_LEN)-1)<<GLB_DIG_CLK_SRC_SEL_POS))
#define GLB_SW_BLE_WAKEUP_REQ                                   GLB_SW_BLE_WAKEUP_REQ
#define GLB_SW_BLE_WAKEUP_REQ_POS                               (31U)
#define GLB_SW_BLE_WAKEUP_REQ_LEN                               (1U)
#define GLB_SW_BLE_WAKEUP_REQ_MSK                               (((1U<<GLB_SW_BLE_WAKEUP_REQ_LEN)-1)<<GLB_SW_BLE_WAKEUP_REQ_POS)
#define GLB_SW_BLE_WAKEUP_REQ_UMSK                              (~(((1U<<GLB_SW_BLE_WAKEUP_REQ_LEN)-1)<<GLB_SW_BLE_WAKEUP_REQ_POS))

/* 0xC0 : BZ_COEX_CTRL */
#define GLB_BZ_COEX_CTRL_OFFSET                                 (0xC0)
#define GLB_COEX_EN                                             GLB_COEX_EN
#define GLB_COEX_EN_POS                                         (0U)
#define GLB_COEX_EN_LEN                                         (1U)
#define GLB_COEX_EN_MSK                                         (((1U<<GLB_COEX_EN_LEN)-1)<<GLB_COEX_EN_POS)
#define GLB_COEX_EN_UMSK                                        (~(((1U<<GLB_COEX_EN_LEN)-1)<<GLB_COEX_EN_POS))
#define GLB_WLAN_EN                                             GLB_WLAN_EN
#define GLB_WLAN_EN_POS                                         (1U)
#define GLB_WLAN_EN_LEN                                         (1U)
#define GLB_WLAN_EN_MSK                                         (((1U<<GLB_WLAN_EN_LEN)-1)<<GLB_WLAN_EN_POS)
#define GLB_WLAN_EN_UMSK                                        (~(((1U<<GLB_WLAN_EN_LEN)-1)<<GLB_WLAN_EN_POS))
#define GLB_BLE_RX_IGNORE                                       GLB_BLE_RX_IGNORE
#define GLB_BLE_RX_IGNORE_POS                                   (2U)
#define GLB_BLE_RX_IGNORE_LEN                                   (1U)
#define GLB_BLE_RX_IGNORE_MSK                                   (((1U<<GLB_BLE_RX_IGNORE_LEN)-1)<<GLB_BLE_RX_IGNORE_POS)
#define GLB_BLE_RX_IGNORE_UMSK                                  (~(((1U<<GLB_BLE_RX_IGNORE_LEN)-1)<<GLB_BLE_RX_IGNORE_POS))
#define GLB_M154_RX_IGNORE                                      GLB_M154_RX_IGNORE
#define GLB_M154_RX_IGNORE_POS                                  (3U)
#define GLB_M154_RX_IGNORE_LEN                                  (1U)
#define GLB_M154_RX_IGNORE_MSK                                  (((1U<<GLB_M154_RX_IGNORE_LEN)-1)<<GLB_M154_RX_IGNORE_POS)
#define GLB_M154_RX_IGNORE_UMSK                                 (~(((1U<<GLB_M154_RX_IGNORE_LEN)-1)<<GLB_M154_RX_IGNORE_POS))
#define GLB_BZ_PRI_THR                                          GLB_BZ_PRI_THR
#define GLB_BZ_PRI_THR_POS                                      (4U)
#define GLB_BZ_PRI_THR_LEN                                      (4U)
#define GLB_BZ_PRI_THR_MSK                                      (((1U<<GLB_BZ_PRI_THR_LEN)-1)<<GLB_BZ_PRI_THR_POS)
#define GLB_BZ_PRI_THR_UMSK                                     (~(((1U<<GLB_BZ_PRI_THR_LEN)-1)<<GLB_BZ_PRI_THR_POS))
#define GLB_BZ_PRI_EN                                           GLB_BZ_PRI_EN
#define GLB_BZ_PRI_EN_POS                                       (8U)
#define GLB_BZ_PRI_EN_LEN                                       (1U)
#define GLB_BZ_PRI_EN_MSK                                       (((1U<<GLB_BZ_PRI_EN_LEN)-1)<<GLB_BZ_PRI_EN_POS)
#define GLB_BZ_PRI_EN_UMSK                                      (~(((1U<<GLB_BZ_PRI_EN_LEN)-1)<<GLB_BZ_PRI_EN_POS))
#define GLB_BZ_PRI_POL                                          GLB_BZ_PRI_POL
#define GLB_BZ_PRI_POL_POS                                      (9U)
#define GLB_BZ_PRI_POL_LEN                                      (1U)
#define GLB_BZ_PRI_POL_MSK                                      (((1U<<GLB_BZ_PRI_POL_LEN)-1)<<GLB_BZ_PRI_POL_POS)
#define GLB_BZ_PRI_POL_UMSK                                     (~(((1U<<GLB_BZ_PRI_POL_LEN)-1)<<GLB_BZ_PRI_POL_POS))
#define GLB_BZ_ACTIVE_POL                                       GLB_BZ_ACTIVE_POL
#define GLB_BZ_ACTIVE_POL_POS                                   (10U)
#define GLB_BZ_ACTIVE_POL_LEN                                   (1U)
#define GLB_BZ_ACTIVE_POL_MSK                                   (((1U<<GLB_BZ_ACTIVE_POL_LEN)-1)<<GLB_BZ_ACTIVE_POL_POS)
#define GLB_BZ_ACTIVE_POL_UMSK                                  (~(((1U<<GLB_BZ_ACTIVE_POL_LEN)-1)<<GLB_BZ_ACTIVE_POL_POS))
#define GLB_BZ_ABORT_POL                                        GLB_BZ_ABORT_POL
#define GLB_BZ_ABORT_POL_POS                                    (11U)
#define GLB_BZ_ABORT_POL_LEN                                    (1U)
#define GLB_BZ_ABORT_POL_MSK                                    (((1U<<GLB_BZ_ABORT_POL_LEN)-1)<<GLB_BZ_ABORT_POL_POS)
#define GLB_BZ_ABORT_POL_UMSK                                   (~(((1U<<GLB_BZ_ABORT_POL_LEN)-1)<<GLB_BZ_ABORT_POL_POS))
#define GLB_COEX_PRI                                            GLB_COEX_PRI
#define GLB_COEX_PRI_POS                                        (12U)
#define GLB_COEX_PRI_LEN                                        (1U)
#define GLB_COEX_PRI_MSK                                        (((1U<<GLB_COEX_PRI_LEN)-1)<<GLB_COEX_PRI_POS)
#define GLB_COEX_PRI_UMSK                                       (~(((1U<<GLB_COEX_PRI_LEN)-1)<<GLB_COEX_PRI_POS))
#define GLB_FORCE_M154_WIN                                      GLB_FORCE_M154_WIN
#define GLB_FORCE_M154_WIN_POS                                  (13U)
#define GLB_FORCE_M154_WIN_LEN                                  (1U)
#define GLB_FORCE_M154_WIN_MSK                                  (((1U<<GLB_FORCE_M154_WIN_LEN)-1)<<GLB_FORCE_M154_WIN_POS)
#define GLB_FORCE_M154_WIN_UMSK                                 (~(((1U<<GLB_FORCE_M154_WIN_LEN)-1)<<GLB_FORCE_M154_WIN_POS))
#define GLB_FORCE_BLE_WIN                                       GLB_FORCE_BLE_WIN
#define GLB_FORCE_BLE_WIN_POS                                   (14U)
#define GLB_FORCE_BLE_WIN_LEN                                   (1U)
#define GLB_FORCE_BLE_WIN_MSK                                   (((1U<<GLB_FORCE_BLE_WIN_LEN)-1)<<GLB_FORCE_BLE_WIN_POS)
#define GLB_FORCE_BLE_WIN_UMSK                                  (~(((1U<<GLB_FORCE_BLE_WIN_LEN)-1)<<GLB_FORCE_BLE_WIN_POS))
#define GLB_COEX_OPTION                                         GLB_COEX_OPTION
#define GLB_COEX_OPTION_POS                                     (15U)
#define GLB_COEX_OPTION_LEN                                     (1U)
#define GLB_COEX_OPTION_MSK                                     (((1U<<GLB_COEX_OPTION_LEN)-1)<<GLB_COEX_OPTION_POS)
#define GLB_COEX_OPTION_UMSK                                    (~(((1U<<GLB_COEX_OPTION_LEN)-1)<<GLB_COEX_OPTION_POS))
#define GLB_COEX_FORCE_CH                                       GLB_COEX_FORCE_CH
#define GLB_COEX_FORCE_CH_POS                                   (16U)
#define GLB_COEX_FORCE_CH_LEN                                   (7U)
#define GLB_COEX_FORCE_CH_MSK                                   (((1U<<GLB_COEX_FORCE_CH_LEN)-1)<<GLB_COEX_FORCE_CH_POS)
#define GLB_COEX_FORCE_CH_UMSK                                  (~(((1U<<GLB_COEX_FORCE_CH_LEN)-1)<<GLB_COEX_FORCE_CH_POS))
#define GLB_M154_RX_ABORT_DIS                                   GLB_M154_RX_ABORT_DIS
#define GLB_M154_RX_ABORT_DIS_POS                               (24U)
#define GLB_M154_RX_ABORT_DIS_LEN                               (1U)
#define GLB_M154_RX_ABORT_DIS_MSK                               (((1U<<GLB_M154_RX_ABORT_DIS_LEN)-1)<<GLB_M154_RX_ABORT_DIS_POS)
#define GLB_M154_RX_ABORT_DIS_UMSK                              (~(((1U<<GLB_M154_RX_ABORT_DIS_LEN)-1)<<GLB_M154_RX_ABORT_DIS_POS))
#define GLB_M154_TX_ABORT_DIS                                   GLB_M154_TX_ABORT_DIS
#define GLB_M154_TX_ABORT_DIS_POS                               (25U)
#define GLB_M154_TX_ABORT_DIS_LEN                               (1U)
#define GLB_M154_TX_ABORT_DIS_MSK                               (((1U<<GLB_M154_TX_ABORT_DIS_LEN)-1)<<GLB_M154_TX_ABORT_DIS_POS)
#define GLB_M154_TX_ABORT_DIS_UMSK                              (~(((1U<<GLB_M154_TX_ABORT_DIS_LEN)-1)<<GLB_M154_TX_ABORT_DIS_POS))
#define GLB_BLE_RX_ABORT_DIS                                    GLB_BLE_RX_ABORT_DIS
#define GLB_BLE_RX_ABORT_DIS_POS                                (26U)
#define GLB_BLE_RX_ABORT_DIS_LEN                                (1U)
#define GLB_BLE_RX_ABORT_DIS_MSK                                (((1U<<GLB_BLE_RX_ABORT_DIS_LEN)-1)<<GLB_BLE_RX_ABORT_DIS_POS)
#define GLB_BLE_RX_ABORT_DIS_UMSK                               (~(((1U<<GLB_BLE_RX_ABORT_DIS_LEN)-1)<<GLB_BLE_RX_ABORT_DIS_POS))
#define GLB_BLE_TX_ABORT_DIS                                    GLB_BLE_TX_ABORT_DIS
#define GLB_BLE_TX_ABORT_DIS_POS                                (27U)
#define GLB_BLE_TX_ABORT_DIS_LEN                                (1U)
#define GLB_BLE_TX_ABORT_DIS_MSK                                (((1U<<GLB_BLE_TX_ABORT_DIS_LEN)-1)<<GLB_BLE_TX_ABORT_DIS_POS)
#define GLB_BLE_TX_ABORT_DIS_UMSK                               (~(((1U<<GLB_BLE_TX_ABORT_DIS_LEN)-1)<<GLB_BLE_TX_ABORT_DIS_POS))
#define GLB_COEX_ARB                                            GLB_COEX_ARB
#define GLB_COEX_ARB_POS                                        (28U)
#define GLB_COEX_ARB_LEN                                        (4U)
#define GLB_COEX_ARB_MSK                                        (((1U<<GLB_COEX_ARB_LEN)-1)<<GLB_COEX_ARB_POS)
#define GLB_COEX_ARB_UMSK                                       (~(((1U<<GLB_COEX_ARB_LEN)-1)<<GLB_COEX_ARB_POS))

/* 0xC8 : UART_SIG_SEL_0 */
#define GLB_UART_SIG_SEL_0_OFFSET                               (0xC8)
#define GLB_UART_SIG_0_SEL                                      GLB_UART_SIG_0_SEL
#define GLB_UART_SIG_0_SEL_POS                                  (0U)
#define GLB_UART_SIG_0_SEL_LEN                                  (4U)
#define GLB_UART_SIG_0_SEL_MSK                                  (((1U<<GLB_UART_SIG_0_SEL_LEN)-1)<<GLB_UART_SIG_0_SEL_POS)
#define GLB_UART_SIG_0_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_0_SEL_LEN)-1)<<GLB_UART_SIG_0_SEL_POS))
#define GLB_UART_SIG_1_SEL                                      GLB_UART_SIG_1_SEL
#define GLB_UART_SIG_1_SEL_POS                                  (4U)
#define GLB_UART_SIG_1_SEL_LEN                                  (4U)
#define GLB_UART_SIG_1_SEL_MSK                                  (((1U<<GLB_UART_SIG_1_SEL_LEN)-1)<<GLB_UART_SIG_1_SEL_POS)
#define GLB_UART_SIG_1_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_1_SEL_LEN)-1)<<GLB_UART_SIG_1_SEL_POS))
#define GLB_UART_SIG_2_SEL                                      GLB_UART_SIG_2_SEL
#define GLB_UART_SIG_2_SEL_POS                                  (8U)
#define GLB_UART_SIG_2_SEL_LEN                                  (4U)
#define GLB_UART_SIG_2_SEL_MSK                                  (((1U<<GLB_UART_SIG_2_SEL_LEN)-1)<<GLB_UART_SIG_2_SEL_POS)
#define GLB_UART_SIG_2_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_2_SEL_LEN)-1)<<GLB_UART_SIG_2_SEL_POS))
#define GLB_UART_SIG_3_SEL                                      GLB_UART_SIG_3_SEL
#define GLB_UART_SIG_3_SEL_POS                                  (12U)
#define GLB_UART_SIG_3_SEL_LEN                                  (4U)
#define GLB_UART_SIG_3_SEL_MSK                                  (((1U<<GLB_UART_SIG_3_SEL_LEN)-1)<<GLB_UART_SIG_3_SEL_POS)
#define GLB_UART_SIG_3_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_3_SEL_LEN)-1)<<GLB_UART_SIG_3_SEL_POS))
#define GLB_UART_SIG_4_SEL                                      GLB_UART_SIG_4_SEL
#define GLB_UART_SIG_4_SEL_POS                                  (16U)
#define GLB_UART_SIG_4_SEL_LEN                                  (4U)
#define GLB_UART_SIG_4_SEL_MSK                                  (((1U<<GLB_UART_SIG_4_SEL_LEN)-1)<<GLB_UART_SIG_4_SEL_POS)
#define GLB_UART_SIG_4_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_4_SEL_LEN)-1)<<GLB_UART_SIG_4_SEL_POS))
#define GLB_UART_SIG_5_SEL                                      GLB_UART_SIG_5_SEL
#define GLB_UART_SIG_5_SEL_POS                                  (20U)
#define GLB_UART_SIG_5_SEL_LEN                                  (4U)
#define GLB_UART_SIG_5_SEL_MSK                                  (((1U<<GLB_UART_SIG_5_SEL_LEN)-1)<<GLB_UART_SIG_5_SEL_POS)
#define GLB_UART_SIG_5_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_5_SEL_LEN)-1)<<GLB_UART_SIG_5_SEL_POS))
#define GLB_UART_SIG_6_SEL                                      GLB_UART_SIG_6_SEL
#define GLB_UART_SIG_6_SEL_POS                                  (24U)
#define GLB_UART_SIG_6_SEL_LEN                                  (4U)
#define GLB_UART_SIG_6_SEL_MSK                                  (((1U<<GLB_UART_SIG_6_SEL_LEN)-1)<<GLB_UART_SIG_6_SEL_POS)
#define GLB_UART_SIG_6_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_6_SEL_LEN)-1)<<GLB_UART_SIG_6_SEL_POS))
#define GLB_UART_SIG_7_SEL                                      GLB_UART_SIG_7_SEL
#define GLB_UART_SIG_7_SEL_POS                                  (28U)
#define GLB_UART_SIG_7_SEL_LEN                                  (4U)
#define GLB_UART_SIG_7_SEL_MSK                                  (((1U<<GLB_UART_SIG_7_SEL_LEN)-1)<<GLB_UART_SIG_7_SEL_POS)
#define GLB_UART_SIG_7_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_7_SEL_LEN)-1)<<GLB_UART_SIG_7_SEL_POS))

/* 0xCC : UART_SIG_SEL_1 */
#define GLB_UART_SIG_SEL_1_OFFSET                               (0xCC)
#define GLB_UART_SIG_8_SEL                                      GLB_UART_SIG_8_SEL
#define GLB_UART_SIG_8_SEL_POS                                  (0U)
#define GLB_UART_SIG_8_SEL_LEN                                  (4U)
#define GLB_UART_SIG_8_SEL_MSK                                  (((1U<<GLB_UART_SIG_8_SEL_LEN)-1)<<GLB_UART_SIG_8_SEL_POS)
#define GLB_UART_SIG_8_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_8_SEL_LEN)-1)<<GLB_UART_SIG_8_SEL_POS))
#define GLB_UART_SIG_9_SEL                                      GLB_UART_SIG_9_SEL
#define GLB_UART_SIG_9_SEL_POS                                  (4U)
#define GLB_UART_SIG_9_SEL_LEN                                  (4U)
#define GLB_UART_SIG_9_SEL_MSK                                  (((1U<<GLB_UART_SIG_9_SEL_LEN)-1)<<GLB_UART_SIG_9_SEL_POS)
#define GLB_UART_SIG_9_SEL_UMSK                                 (~(((1U<<GLB_UART_SIG_9_SEL_LEN)-1)<<GLB_UART_SIG_9_SEL_POS))
#define GLB_UART_SIG_10_SEL                                     GLB_UART_SIG_10_SEL
#define GLB_UART_SIG_10_SEL_POS                                 (8U)
#define GLB_UART_SIG_10_SEL_LEN                                 (4U)
#define GLB_UART_SIG_10_SEL_MSK                                 (((1U<<GLB_UART_SIG_10_SEL_LEN)-1)<<GLB_UART_SIG_10_SEL_POS)
#define GLB_UART_SIG_10_SEL_UMSK                                (~(((1U<<GLB_UART_SIG_10_SEL_LEN)-1)<<GLB_UART_SIG_10_SEL_POS))
#define GLB_UART_SIG_11_SEL                                     GLB_UART_SIG_11_SEL
#define GLB_UART_SIG_11_SEL_POS                                 (12U)
#define GLB_UART_SIG_11_SEL_LEN                                 (4U)
#define GLB_UART_SIG_11_SEL_MSK                                 (((1U<<GLB_UART_SIG_11_SEL_LEN)-1)<<GLB_UART_SIG_11_SEL_POS)
#define GLB_UART_SIG_11_SEL_UMSK                                (~(((1U<<GLB_UART_SIG_11_SEL_LEN)-1)<<GLB_UART_SIG_11_SEL_POS))

/* 0xD0 : DBG_SEL_LL */
#define GLB_DBG_SEL_LL_OFFSET                                   (0xD0)
#define GLB_REG_DBG_LL_CTRL                                     GLB_REG_DBG_LL_CTRL
#define GLB_REG_DBG_LL_CTRL_POS                                 (0U)
#define GLB_REG_DBG_LL_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_LL_CTRL_MSK                                 (((1U<<GLB_REG_DBG_LL_CTRL_LEN)-1)<<GLB_REG_DBG_LL_CTRL_POS)
#define GLB_REG_DBG_LL_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_LL_CTRL_LEN)-1)<<GLB_REG_DBG_LL_CTRL_POS))

/* 0xD4 : DBG_SEL_LH */
#define GLB_DBG_SEL_LH_OFFSET                                   (0xD4)
#define GLB_REG_DBG_LH_CTRL                                     GLB_REG_DBG_LH_CTRL
#define GLB_REG_DBG_LH_CTRL_POS                                 (0U)
#define GLB_REG_DBG_LH_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_LH_CTRL_MSK                                 (((1U<<GLB_REG_DBG_LH_CTRL_LEN)-1)<<GLB_REG_DBG_LH_CTRL_POS)
#define GLB_REG_DBG_LH_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_LH_CTRL_LEN)-1)<<GLB_REG_DBG_LH_CTRL_POS))

/* 0xD8 : DBG_SEL_HL */
#define GLB_DBG_SEL_HL_OFFSET                                   (0xD8)
#define GLB_REG_DBG_HL_CTRL                                     GLB_REG_DBG_HL_CTRL
#define GLB_REG_DBG_HL_CTRL_POS                                 (0U)
#define GLB_REG_DBG_HL_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_HL_CTRL_MSK                                 (((1U<<GLB_REG_DBG_HL_CTRL_LEN)-1)<<GLB_REG_DBG_HL_CTRL_POS)
#define GLB_REG_DBG_HL_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_HL_CTRL_LEN)-1)<<GLB_REG_DBG_HL_CTRL_POS))

/* 0xDC : DBG_SEL_HH */
#define GLB_DBG_SEL_HH_OFFSET                                   (0xDC)
#define GLB_REG_DBG_HH_CTRL                                     GLB_REG_DBG_HH_CTRL
#define GLB_REG_DBG_HH_CTRL_POS                                 (0U)
#define GLB_REG_DBG_HH_CTRL_LEN                                 (32U)
#define GLB_REG_DBG_HH_CTRL_MSK                                 (((1U<<GLB_REG_DBG_HH_CTRL_LEN)-1)<<GLB_REG_DBG_HH_CTRL_POS)
#define GLB_REG_DBG_HH_CTRL_UMSK                                (~(((1U<<GLB_REG_DBG_HH_CTRL_LEN)-1)<<GLB_REG_DBG_HH_CTRL_POS))

/* 0xE0 : debug */
#define GLB_DEBUG_OFFSET                                        (0xE0)
#define GLB_DEBUG_OE                                            GLB_DEBUG_OE
#define GLB_DEBUG_OE_POS                                        (0U)
#define GLB_DEBUG_OE_LEN                                        (1U)
#define GLB_DEBUG_OE_MSK                                        (((1U<<GLB_DEBUG_OE_LEN)-1)<<GLB_DEBUG_OE_POS)
#define GLB_DEBUG_OE_UMSK                                       (~(((1U<<GLB_DEBUG_OE_LEN)-1)<<GLB_DEBUG_OE_POS))
#define GLB_DEBUG_I                                             GLB_DEBUG_I
#define GLB_DEBUG_I_POS                                         (1U)
#define GLB_DEBUG_I_LEN                                         (31U)
#define GLB_DEBUG_I_MSK                                         (((1U<<GLB_DEBUG_I_LEN)-1)<<GLB_DEBUG_I_POS)
#define GLB_DEBUG_I_UMSK                                        (~(((1U<<GLB_DEBUG_I_LEN)-1)<<GLB_DEBUG_I_POS))

/* 0x100 : GPIO_CFGCTL0 */
#define GLB_GPIO_CFGCTL0_OFFSET                                 (0x100)
#define GLB_REG_GPIO_0_IE                                       GLB_REG_GPIO_0_IE
#define GLB_REG_GPIO_0_IE_POS                                   (0U)
#define GLB_REG_GPIO_0_IE_LEN                                   (1U)
#define GLB_REG_GPIO_0_IE_MSK                                   (((1U<<GLB_REG_GPIO_0_IE_LEN)-1)<<GLB_REG_GPIO_0_IE_POS)
#define GLB_REG_GPIO_0_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_IE_LEN)-1)<<GLB_REG_GPIO_0_IE_POS))
#define GLB_REG_GPIO_0_SMT                                      GLB_REG_GPIO_0_SMT
#define GLB_REG_GPIO_0_SMT_POS                                  (1U)
#define GLB_REG_GPIO_0_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_0_SMT_MSK                                  (((1U<<GLB_REG_GPIO_0_SMT_LEN)-1)<<GLB_REG_GPIO_0_SMT_POS)
#define GLB_REG_GPIO_0_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_SMT_LEN)-1)<<GLB_REG_GPIO_0_SMT_POS))
#define GLB_REG_GPIO_0_DRV                                      GLB_REG_GPIO_0_DRV
#define GLB_REG_GPIO_0_DRV_POS                                  (2U)
#define GLB_REG_GPIO_0_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_0_DRV_MSK                                  (((1U<<GLB_REG_GPIO_0_DRV_LEN)-1)<<GLB_REG_GPIO_0_DRV_POS)
#define GLB_REG_GPIO_0_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_DRV_LEN)-1)<<GLB_REG_GPIO_0_DRV_POS))
#define GLB_REG_GPIO_0_PU                                       GLB_REG_GPIO_0_PU
#define GLB_REG_GPIO_0_PU_POS                                   (4U)
#define GLB_REG_GPIO_0_PU_LEN                                   (1U)
#define GLB_REG_GPIO_0_PU_MSK                                   (((1U<<GLB_REG_GPIO_0_PU_LEN)-1)<<GLB_REG_GPIO_0_PU_POS)
#define GLB_REG_GPIO_0_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_PU_LEN)-1)<<GLB_REG_GPIO_0_PU_POS))
#define GLB_REG_GPIO_0_PD                                       GLB_REG_GPIO_0_PD
#define GLB_REG_GPIO_0_PD_POS                                   (5U)
#define GLB_REG_GPIO_0_PD_LEN                                   (1U)
#define GLB_REG_GPIO_0_PD_MSK                                   (((1U<<GLB_REG_GPIO_0_PD_LEN)-1)<<GLB_REG_GPIO_0_PD_POS)
#define GLB_REG_GPIO_0_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_PD_LEN)-1)<<GLB_REG_GPIO_0_PD_POS))
#define GLB_REG_GPIO_0_FUNC_SEL                                 GLB_REG_GPIO_0_FUNC_SEL
#define GLB_REG_GPIO_0_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_0_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_0_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_0_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_0_FUNC_SEL_POS)
#define GLB_REG_GPIO_0_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_0_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_0_FUNC_SEL_POS))
#define GLB_REG_GPIO_0_MODE                                     GLB_REG_GPIO_0_MODE
#define GLB_REG_GPIO_0_MODE_POS                                 (15U)
#define GLB_REG_GPIO_0_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_0_MODE_MSK                                 (((1U<<GLB_REG_GPIO_0_MODE_LEN)-1)<<GLB_REG_GPIO_0_MODE_POS)
#define GLB_REG_GPIO_0_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_0_MODE_LEN)-1)<<GLB_REG_GPIO_0_MODE_POS))
#define GLB_REG_GPIO_1_IE                                       GLB_REG_GPIO_1_IE
#define GLB_REG_GPIO_1_IE_POS                                   (16U)
#define GLB_REG_GPIO_1_IE_LEN                                   (1U)
#define GLB_REG_GPIO_1_IE_MSK                                   (((1U<<GLB_REG_GPIO_1_IE_LEN)-1)<<GLB_REG_GPIO_1_IE_POS)
#define GLB_REG_GPIO_1_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_IE_LEN)-1)<<GLB_REG_GPIO_1_IE_POS))
#define GLB_REG_GPIO_1_SMT                                      GLB_REG_GPIO_1_SMT
#define GLB_REG_GPIO_1_SMT_POS                                  (17U)
#define GLB_REG_GPIO_1_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_1_SMT_MSK                                  (((1U<<GLB_REG_GPIO_1_SMT_LEN)-1)<<GLB_REG_GPIO_1_SMT_POS)
#define GLB_REG_GPIO_1_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_SMT_LEN)-1)<<GLB_REG_GPIO_1_SMT_POS))
#define GLB_REG_GPIO_1_DRV                                      GLB_REG_GPIO_1_DRV
#define GLB_REG_GPIO_1_DRV_POS                                  (18U)
#define GLB_REG_GPIO_1_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_1_DRV_MSK                                  (((1U<<GLB_REG_GPIO_1_DRV_LEN)-1)<<GLB_REG_GPIO_1_DRV_POS)
#define GLB_REG_GPIO_1_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_DRV_LEN)-1)<<GLB_REG_GPIO_1_DRV_POS))
#define GLB_REG_GPIO_1_PU                                       GLB_REG_GPIO_1_PU
#define GLB_REG_GPIO_1_PU_POS                                   (20U)
#define GLB_REG_GPIO_1_PU_LEN                                   (1U)
#define GLB_REG_GPIO_1_PU_MSK                                   (((1U<<GLB_REG_GPIO_1_PU_LEN)-1)<<GLB_REG_GPIO_1_PU_POS)
#define GLB_REG_GPIO_1_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_PU_LEN)-1)<<GLB_REG_GPIO_1_PU_POS))
#define GLB_REG_GPIO_1_PD                                       GLB_REG_GPIO_1_PD
#define GLB_REG_GPIO_1_PD_POS                                   (21U)
#define GLB_REG_GPIO_1_PD_LEN                                   (1U)
#define GLB_REG_GPIO_1_PD_MSK                                   (((1U<<GLB_REG_GPIO_1_PD_LEN)-1)<<GLB_REG_GPIO_1_PD_POS)
#define GLB_REG_GPIO_1_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_PD_LEN)-1)<<GLB_REG_GPIO_1_PD_POS))
#define GLB_REG_GPIO_1_FUNC_SEL                                 GLB_REG_GPIO_1_FUNC_SEL
#define GLB_REG_GPIO_1_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_1_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_1_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_1_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_1_FUNC_SEL_POS)
#define GLB_REG_GPIO_1_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_1_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_1_FUNC_SEL_POS))
#define GLB_REG_GPIO_1_MODE                                     GLB_REG_GPIO_1_MODE
#define GLB_REG_GPIO_1_MODE_POS                                 (31U)
#define GLB_REG_GPIO_1_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_1_MODE_MSK                                 (((1U<<GLB_REG_GPIO_1_MODE_LEN)-1)<<GLB_REG_GPIO_1_MODE_POS)
#define GLB_REG_GPIO_1_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_1_MODE_LEN)-1)<<GLB_REG_GPIO_1_MODE_POS))

/* 0x104 : GPIO_CFGCTL1 */
#define GLB_GPIO_CFGCTL1_OFFSET                                 (0x104)
#define GLB_REG_GPIO_2_IE                                       GLB_REG_GPIO_2_IE
#define GLB_REG_GPIO_2_IE_POS                                   (0U)
#define GLB_REG_GPIO_2_IE_LEN                                   (1U)
#define GLB_REG_GPIO_2_IE_MSK                                   (((1U<<GLB_REG_GPIO_2_IE_LEN)-1)<<GLB_REG_GPIO_2_IE_POS)
#define GLB_REG_GPIO_2_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_IE_LEN)-1)<<GLB_REG_GPIO_2_IE_POS))
#define GLB_REG_GPIO_2_SMT                                      GLB_REG_GPIO_2_SMT
#define GLB_REG_GPIO_2_SMT_POS                                  (1U)
#define GLB_REG_GPIO_2_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_2_SMT_MSK                                  (((1U<<GLB_REG_GPIO_2_SMT_LEN)-1)<<GLB_REG_GPIO_2_SMT_POS)
#define GLB_REG_GPIO_2_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_SMT_LEN)-1)<<GLB_REG_GPIO_2_SMT_POS))
#define GLB_REG_GPIO_2_DRV                                      GLB_REG_GPIO_2_DRV
#define GLB_REG_GPIO_2_DRV_POS                                  (2U)
#define GLB_REG_GPIO_2_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_2_DRV_MSK                                  (((1U<<GLB_REG_GPIO_2_DRV_LEN)-1)<<GLB_REG_GPIO_2_DRV_POS)
#define GLB_REG_GPIO_2_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_DRV_LEN)-1)<<GLB_REG_GPIO_2_DRV_POS))
#define GLB_REG_GPIO_2_PU                                       GLB_REG_GPIO_2_PU
#define GLB_REG_GPIO_2_PU_POS                                   (4U)
#define GLB_REG_GPIO_2_PU_LEN                                   (1U)
#define GLB_REG_GPIO_2_PU_MSK                                   (((1U<<GLB_REG_GPIO_2_PU_LEN)-1)<<GLB_REG_GPIO_2_PU_POS)
#define GLB_REG_GPIO_2_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_PU_LEN)-1)<<GLB_REG_GPIO_2_PU_POS))
#define GLB_REG_GPIO_2_PD                                       GLB_REG_GPIO_2_PD
#define GLB_REG_GPIO_2_PD_POS                                   (5U)
#define GLB_REG_GPIO_2_PD_LEN                                   (1U)
#define GLB_REG_GPIO_2_PD_MSK                                   (((1U<<GLB_REG_GPIO_2_PD_LEN)-1)<<GLB_REG_GPIO_2_PD_POS)
#define GLB_REG_GPIO_2_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_PD_LEN)-1)<<GLB_REG_GPIO_2_PD_POS))
#define GLB_REG_GPIO_2_FUNC_SEL                                 GLB_REG_GPIO_2_FUNC_SEL
#define GLB_REG_GPIO_2_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_2_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_2_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_2_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_2_FUNC_SEL_POS)
#define GLB_REG_GPIO_2_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_2_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_2_FUNC_SEL_POS))
#define GLB_REG_GPIO_2_MODE                                     GLB_REG_GPIO_2_MODE
#define GLB_REG_GPIO_2_MODE_POS                                 (15U)
#define GLB_REG_GPIO_2_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_2_MODE_MSK                                 (((1U<<GLB_REG_GPIO_2_MODE_LEN)-1)<<GLB_REG_GPIO_2_MODE_POS)
#define GLB_REG_GPIO_2_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_2_MODE_LEN)-1)<<GLB_REG_GPIO_2_MODE_POS))
#define GLB_REG_GPIO_3_IE                                       GLB_REG_GPIO_3_IE
#define GLB_REG_GPIO_3_IE_POS                                   (16U)
#define GLB_REG_GPIO_3_IE_LEN                                   (1U)
#define GLB_REG_GPIO_3_IE_MSK                                   (((1U<<GLB_REG_GPIO_3_IE_LEN)-1)<<GLB_REG_GPIO_3_IE_POS)
#define GLB_REG_GPIO_3_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_IE_LEN)-1)<<GLB_REG_GPIO_3_IE_POS))
#define GLB_REG_GPIO_3_SMT                                      GLB_REG_GPIO_3_SMT
#define GLB_REG_GPIO_3_SMT_POS                                  (17U)
#define GLB_REG_GPIO_3_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_3_SMT_MSK                                  (((1U<<GLB_REG_GPIO_3_SMT_LEN)-1)<<GLB_REG_GPIO_3_SMT_POS)
#define GLB_REG_GPIO_3_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_SMT_LEN)-1)<<GLB_REG_GPIO_3_SMT_POS))
#define GLB_REG_GPIO_3_DRV                                      GLB_REG_GPIO_3_DRV
#define GLB_REG_GPIO_3_DRV_POS                                  (18U)
#define GLB_REG_GPIO_3_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_3_DRV_MSK                                  (((1U<<GLB_REG_GPIO_3_DRV_LEN)-1)<<GLB_REG_GPIO_3_DRV_POS)
#define GLB_REG_GPIO_3_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_DRV_LEN)-1)<<GLB_REG_GPIO_3_DRV_POS))
#define GLB_REG_GPIO_3_PU                                       GLB_REG_GPIO_3_PU
#define GLB_REG_GPIO_3_PU_POS                                   (20U)
#define GLB_REG_GPIO_3_PU_LEN                                   (1U)
#define GLB_REG_GPIO_3_PU_MSK                                   (((1U<<GLB_REG_GPIO_3_PU_LEN)-1)<<GLB_REG_GPIO_3_PU_POS)
#define GLB_REG_GPIO_3_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_PU_LEN)-1)<<GLB_REG_GPIO_3_PU_POS))
#define GLB_REG_GPIO_3_PD                                       GLB_REG_GPIO_3_PD
#define GLB_REG_GPIO_3_PD_POS                                   (21U)
#define GLB_REG_GPIO_3_PD_LEN                                   (1U)
#define GLB_REG_GPIO_3_PD_MSK                                   (((1U<<GLB_REG_GPIO_3_PD_LEN)-1)<<GLB_REG_GPIO_3_PD_POS)
#define GLB_REG_GPIO_3_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_PD_LEN)-1)<<GLB_REG_GPIO_3_PD_POS))
#define GLB_REG_GPIO_3_FUNC_SEL                                 GLB_REG_GPIO_3_FUNC_SEL
#define GLB_REG_GPIO_3_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_3_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_3_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_3_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_3_FUNC_SEL_POS)
#define GLB_REG_GPIO_3_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_3_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_3_FUNC_SEL_POS))
#define GLB_REG_GPIO_3_MODE                                     GLB_REG_GPIO_3_MODE
#define GLB_REG_GPIO_3_MODE_POS                                 (31U)
#define GLB_REG_GPIO_3_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_3_MODE_MSK                                 (((1U<<GLB_REG_GPIO_3_MODE_LEN)-1)<<GLB_REG_GPIO_3_MODE_POS)
#define GLB_REG_GPIO_3_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_3_MODE_LEN)-1)<<GLB_REG_GPIO_3_MODE_POS))

/* 0x108 : GPIO_CFGCTL2 */
#define GLB_GPIO_CFGCTL2_OFFSET                                 (0x108)
#define GLB_REG_GPIO_4_IE                                       GLB_REG_GPIO_4_IE
#define GLB_REG_GPIO_4_IE_POS                                   (0U)
#define GLB_REG_GPIO_4_IE_LEN                                   (1U)
#define GLB_REG_GPIO_4_IE_MSK                                   (((1U<<GLB_REG_GPIO_4_IE_LEN)-1)<<GLB_REG_GPIO_4_IE_POS)
#define GLB_REG_GPIO_4_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_4_IE_LEN)-1)<<GLB_REG_GPIO_4_IE_POS))
#define GLB_REG_GPIO_4_SMT                                      GLB_REG_GPIO_4_SMT
#define GLB_REG_GPIO_4_SMT_POS                                  (1U)
#define GLB_REG_GPIO_4_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_4_SMT_MSK                                  (((1U<<GLB_REG_GPIO_4_SMT_LEN)-1)<<GLB_REG_GPIO_4_SMT_POS)
#define GLB_REG_GPIO_4_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_4_SMT_LEN)-1)<<GLB_REG_GPIO_4_SMT_POS))
#define GLB_REG_GPIO_4_DRV                                      GLB_REG_GPIO_4_DRV
#define GLB_REG_GPIO_4_DRV_POS                                  (2U)
#define GLB_REG_GPIO_4_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_4_DRV_MSK                                  (((1U<<GLB_REG_GPIO_4_DRV_LEN)-1)<<GLB_REG_GPIO_4_DRV_POS)
#define GLB_REG_GPIO_4_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_4_DRV_LEN)-1)<<GLB_REG_GPIO_4_DRV_POS))
#define GLB_REG_GPIO_4_PU                                       GLB_REG_GPIO_4_PU
#define GLB_REG_GPIO_4_PU_POS                                   (4U)
#define GLB_REG_GPIO_4_PU_LEN                                   (1U)
#define GLB_REG_GPIO_4_PU_MSK                                   (((1U<<GLB_REG_GPIO_4_PU_LEN)-1)<<GLB_REG_GPIO_4_PU_POS)
#define GLB_REG_GPIO_4_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_4_PU_LEN)-1)<<GLB_REG_GPIO_4_PU_POS))
#define GLB_REG_GPIO_4_PD                                       GLB_REG_GPIO_4_PD
#define GLB_REG_GPIO_4_PD_POS                                   (5U)
#define GLB_REG_GPIO_4_PD_LEN                                   (1U)
#define GLB_REG_GPIO_4_PD_MSK                                   (((1U<<GLB_REG_GPIO_4_PD_LEN)-1)<<GLB_REG_GPIO_4_PD_POS)
#define GLB_REG_GPIO_4_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_4_PD_LEN)-1)<<GLB_REG_GPIO_4_PD_POS))
#define GLB_REG_GPIO_4_FUNC_SEL                                 GLB_REG_GPIO_4_FUNC_SEL
#define GLB_REG_GPIO_4_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_4_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_4_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_4_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_4_FUNC_SEL_POS)
#define GLB_REG_GPIO_4_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_4_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_4_FUNC_SEL_POS))
#define GLB_REG_GPIO_4_MODE                                     GLB_REG_GPIO_4_MODE
#define GLB_REG_GPIO_4_MODE_POS                                 (15U)
#define GLB_REG_GPIO_4_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_4_MODE_MSK                                 (((1U<<GLB_REG_GPIO_4_MODE_LEN)-1)<<GLB_REG_GPIO_4_MODE_POS)
#define GLB_REG_GPIO_4_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_4_MODE_LEN)-1)<<GLB_REG_GPIO_4_MODE_POS))
#define GLB_REG_GPIO_5_IE                                       GLB_REG_GPIO_5_IE
#define GLB_REG_GPIO_5_IE_POS                                   (16U)
#define GLB_REG_GPIO_5_IE_LEN                                   (1U)
#define GLB_REG_GPIO_5_IE_MSK                                   (((1U<<GLB_REG_GPIO_5_IE_LEN)-1)<<GLB_REG_GPIO_5_IE_POS)
#define GLB_REG_GPIO_5_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_5_IE_LEN)-1)<<GLB_REG_GPIO_5_IE_POS))
#define GLB_REG_GPIO_5_SMT                                      GLB_REG_GPIO_5_SMT
#define GLB_REG_GPIO_5_SMT_POS                                  (17U)
#define GLB_REG_GPIO_5_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_5_SMT_MSK                                  (((1U<<GLB_REG_GPIO_5_SMT_LEN)-1)<<GLB_REG_GPIO_5_SMT_POS)
#define GLB_REG_GPIO_5_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_5_SMT_LEN)-1)<<GLB_REG_GPIO_5_SMT_POS))
#define GLB_REG_GPIO_5_DRV                                      GLB_REG_GPIO_5_DRV
#define GLB_REG_GPIO_5_DRV_POS                                  (18U)
#define GLB_REG_GPIO_5_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_5_DRV_MSK                                  (((1U<<GLB_REG_GPIO_5_DRV_LEN)-1)<<GLB_REG_GPIO_5_DRV_POS)
#define GLB_REG_GPIO_5_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_5_DRV_LEN)-1)<<GLB_REG_GPIO_5_DRV_POS))
#define GLB_REG_GPIO_5_PU                                       GLB_REG_GPIO_5_PU
#define GLB_REG_GPIO_5_PU_POS                                   (20U)
#define GLB_REG_GPIO_5_PU_LEN                                   (1U)
#define GLB_REG_GPIO_5_PU_MSK                                   (((1U<<GLB_REG_GPIO_5_PU_LEN)-1)<<GLB_REG_GPIO_5_PU_POS)
#define GLB_REG_GPIO_5_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_5_PU_LEN)-1)<<GLB_REG_GPIO_5_PU_POS))
#define GLB_REG_GPIO_5_PD                                       GLB_REG_GPIO_5_PD
#define GLB_REG_GPIO_5_PD_POS                                   (21U)
#define GLB_REG_GPIO_5_PD_LEN                                   (1U)
#define GLB_REG_GPIO_5_PD_MSK                                   (((1U<<GLB_REG_GPIO_5_PD_LEN)-1)<<GLB_REG_GPIO_5_PD_POS)
#define GLB_REG_GPIO_5_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_5_PD_LEN)-1)<<GLB_REG_GPIO_5_PD_POS))
#define GLB_REG_GPIO_5_FUNC_SEL                                 GLB_REG_GPIO_5_FUNC_SEL
#define GLB_REG_GPIO_5_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_5_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_5_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_5_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_5_FUNC_SEL_POS)
#define GLB_REG_GPIO_5_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_5_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_5_FUNC_SEL_POS))
#define GLB_REG_GPIO_5_MODE                                     GLB_REG_GPIO_5_MODE
#define GLB_REG_GPIO_5_MODE_POS                                 (31U)
#define GLB_REG_GPIO_5_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_5_MODE_MSK                                 (((1U<<GLB_REG_GPIO_5_MODE_LEN)-1)<<GLB_REG_GPIO_5_MODE_POS)
#define GLB_REG_GPIO_5_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_5_MODE_LEN)-1)<<GLB_REG_GPIO_5_MODE_POS))

/* 0x10C : GPIO_CFGCTL3 */
#define GLB_GPIO_CFGCTL3_OFFSET                                 (0x10C)
#define GLB_REG_GPIO_6_IE                                       GLB_REG_GPIO_6_IE
#define GLB_REG_GPIO_6_IE_POS                                   (0U)
#define GLB_REG_GPIO_6_IE_LEN                                   (1U)
#define GLB_REG_GPIO_6_IE_MSK                                   (((1U<<GLB_REG_GPIO_6_IE_LEN)-1)<<GLB_REG_GPIO_6_IE_POS)
#define GLB_REG_GPIO_6_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_6_IE_LEN)-1)<<GLB_REG_GPIO_6_IE_POS))
#define GLB_REG_GPIO_6_SMT                                      GLB_REG_GPIO_6_SMT
#define GLB_REG_GPIO_6_SMT_POS                                  (1U)
#define GLB_REG_GPIO_6_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_6_SMT_MSK                                  (((1U<<GLB_REG_GPIO_6_SMT_LEN)-1)<<GLB_REG_GPIO_6_SMT_POS)
#define GLB_REG_GPIO_6_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_6_SMT_LEN)-1)<<GLB_REG_GPIO_6_SMT_POS))
#define GLB_REG_GPIO_6_DRV                                      GLB_REG_GPIO_6_DRV
#define GLB_REG_GPIO_6_DRV_POS                                  (2U)
#define GLB_REG_GPIO_6_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_6_DRV_MSK                                  (((1U<<GLB_REG_GPIO_6_DRV_LEN)-1)<<GLB_REG_GPIO_6_DRV_POS)
#define GLB_REG_GPIO_6_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_6_DRV_LEN)-1)<<GLB_REG_GPIO_6_DRV_POS))
#define GLB_REG_GPIO_6_PU                                       GLB_REG_GPIO_6_PU
#define GLB_REG_GPIO_6_PU_POS                                   (4U)
#define GLB_REG_GPIO_6_PU_LEN                                   (1U)
#define GLB_REG_GPIO_6_PU_MSK                                   (((1U<<GLB_REG_GPIO_6_PU_LEN)-1)<<GLB_REG_GPIO_6_PU_POS)
#define GLB_REG_GPIO_6_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_6_PU_LEN)-1)<<GLB_REG_GPIO_6_PU_POS))
#define GLB_REG_GPIO_6_PD                                       GLB_REG_GPIO_6_PD
#define GLB_REG_GPIO_6_PD_POS                                   (5U)
#define GLB_REG_GPIO_6_PD_LEN                                   (1U)
#define GLB_REG_GPIO_6_PD_MSK                                   (((1U<<GLB_REG_GPIO_6_PD_LEN)-1)<<GLB_REG_GPIO_6_PD_POS)
#define GLB_REG_GPIO_6_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_6_PD_LEN)-1)<<GLB_REG_GPIO_6_PD_POS))
#define GLB_REG_GPIO_6_FUNC_SEL                                 GLB_REG_GPIO_6_FUNC_SEL
#define GLB_REG_GPIO_6_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_6_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_6_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_6_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_6_FUNC_SEL_POS)
#define GLB_REG_GPIO_6_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_6_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_6_FUNC_SEL_POS))
#define GLB_REG_GPIO_6_MODE                                     GLB_REG_GPIO_6_MODE
#define GLB_REG_GPIO_6_MODE_POS                                 (15U)
#define GLB_REG_GPIO_6_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_6_MODE_MSK                                 (((1U<<GLB_REG_GPIO_6_MODE_LEN)-1)<<GLB_REG_GPIO_6_MODE_POS)
#define GLB_REG_GPIO_6_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_6_MODE_LEN)-1)<<GLB_REG_GPIO_6_MODE_POS))
#define GLB_REG_GPIO_7_IE                                       GLB_REG_GPIO_7_IE
#define GLB_REG_GPIO_7_IE_POS                                   (16U)
#define GLB_REG_GPIO_7_IE_LEN                                   (1U)
#define GLB_REG_GPIO_7_IE_MSK                                   (((1U<<GLB_REG_GPIO_7_IE_LEN)-1)<<GLB_REG_GPIO_7_IE_POS)
#define GLB_REG_GPIO_7_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_IE_LEN)-1)<<GLB_REG_GPIO_7_IE_POS))
#define GLB_REG_GPIO_7_SMT                                      GLB_REG_GPIO_7_SMT
#define GLB_REG_GPIO_7_SMT_POS                                  (17U)
#define GLB_REG_GPIO_7_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_7_SMT_MSK                                  (((1U<<GLB_REG_GPIO_7_SMT_LEN)-1)<<GLB_REG_GPIO_7_SMT_POS)
#define GLB_REG_GPIO_7_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_SMT_LEN)-1)<<GLB_REG_GPIO_7_SMT_POS))
#define GLB_REG_GPIO_7_DRV                                      GLB_REG_GPIO_7_DRV
#define GLB_REG_GPIO_7_DRV_POS                                  (18U)
#define GLB_REG_GPIO_7_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_7_DRV_MSK                                  (((1U<<GLB_REG_GPIO_7_DRV_LEN)-1)<<GLB_REG_GPIO_7_DRV_POS)
#define GLB_REG_GPIO_7_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_DRV_LEN)-1)<<GLB_REG_GPIO_7_DRV_POS))
#define GLB_REG_GPIO_7_PU                                       GLB_REG_GPIO_7_PU
#define GLB_REG_GPIO_7_PU_POS                                   (20U)
#define GLB_REG_GPIO_7_PU_LEN                                   (1U)
#define GLB_REG_GPIO_7_PU_MSK                                   (((1U<<GLB_REG_GPIO_7_PU_LEN)-1)<<GLB_REG_GPIO_7_PU_POS)
#define GLB_REG_GPIO_7_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_PU_LEN)-1)<<GLB_REG_GPIO_7_PU_POS))
#define GLB_REG_GPIO_7_PD                                       GLB_REG_GPIO_7_PD
#define GLB_REG_GPIO_7_PD_POS                                   (21U)
#define GLB_REG_GPIO_7_PD_LEN                                   (1U)
#define GLB_REG_GPIO_7_PD_MSK                                   (((1U<<GLB_REG_GPIO_7_PD_LEN)-1)<<GLB_REG_GPIO_7_PD_POS)
#define GLB_REG_GPIO_7_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_PD_LEN)-1)<<GLB_REG_GPIO_7_PD_POS))
#define GLB_REG_GPIO_7_FUNC_SEL                                 GLB_REG_GPIO_7_FUNC_SEL
#define GLB_REG_GPIO_7_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_7_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_7_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_7_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_7_FUNC_SEL_POS)
#define GLB_REG_GPIO_7_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_7_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_7_FUNC_SEL_POS))
#define GLB_REG_GPIO_7_MODE                                     GLB_REG_GPIO_7_MODE
#define GLB_REG_GPIO_7_MODE_POS                                 (31U)
#define GLB_REG_GPIO_7_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_7_MODE_MSK                                 (((1U<<GLB_REG_GPIO_7_MODE_LEN)-1)<<GLB_REG_GPIO_7_MODE_POS)
#define GLB_REG_GPIO_7_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_7_MODE_LEN)-1)<<GLB_REG_GPIO_7_MODE_POS))

/* 0x110 : GPIO_CFGCTL4 */
#define GLB_GPIO_CFGCTL4_OFFSET                                 (0x110)
#define GLB_REG_GPIO_8_IE                                       GLB_REG_GPIO_8_IE
#define GLB_REG_GPIO_8_IE_POS                                   (0U)
#define GLB_REG_GPIO_8_IE_LEN                                   (1U)
#define GLB_REG_GPIO_8_IE_MSK                                   (((1U<<GLB_REG_GPIO_8_IE_LEN)-1)<<GLB_REG_GPIO_8_IE_POS)
#define GLB_REG_GPIO_8_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_IE_LEN)-1)<<GLB_REG_GPIO_8_IE_POS))
#define GLB_REG_GPIO_8_SMT                                      GLB_REG_GPIO_8_SMT
#define GLB_REG_GPIO_8_SMT_POS                                  (1U)
#define GLB_REG_GPIO_8_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_8_SMT_MSK                                  (((1U<<GLB_REG_GPIO_8_SMT_LEN)-1)<<GLB_REG_GPIO_8_SMT_POS)
#define GLB_REG_GPIO_8_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_SMT_LEN)-1)<<GLB_REG_GPIO_8_SMT_POS))
#define GLB_REG_GPIO_8_DRV                                      GLB_REG_GPIO_8_DRV
#define GLB_REG_GPIO_8_DRV_POS                                  (2U)
#define GLB_REG_GPIO_8_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_8_DRV_MSK                                  (((1U<<GLB_REG_GPIO_8_DRV_LEN)-1)<<GLB_REG_GPIO_8_DRV_POS)
#define GLB_REG_GPIO_8_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_DRV_LEN)-1)<<GLB_REG_GPIO_8_DRV_POS))
#define GLB_REG_GPIO_8_PU                                       GLB_REG_GPIO_8_PU
#define GLB_REG_GPIO_8_PU_POS                                   (4U)
#define GLB_REG_GPIO_8_PU_LEN                                   (1U)
#define GLB_REG_GPIO_8_PU_MSK                                   (((1U<<GLB_REG_GPIO_8_PU_LEN)-1)<<GLB_REG_GPIO_8_PU_POS)
#define GLB_REG_GPIO_8_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_PU_LEN)-1)<<GLB_REG_GPIO_8_PU_POS))
#define GLB_REG_GPIO_8_PD                                       GLB_REG_GPIO_8_PD
#define GLB_REG_GPIO_8_PD_POS                                   (5U)
#define GLB_REG_GPIO_8_PD_LEN                                   (1U)
#define GLB_REG_GPIO_8_PD_MSK                                   (((1U<<GLB_REG_GPIO_8_PD_LEN)-1)<<GLB_REG_GPIO_8_PD_POS)
#define GLB_REG_GPIO_8_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_PD_LEN)-1)<<GLB_REG_GPIO_8_PD_POS))
#define GLB_REG_GPIO_8_FUNC_SEL                                 GLB_REG_GPIO_8_FUNC_SEL
#define GLB_REG_GPIO_8_FUNC_SEL_POS                             (8U)
#define GLB_REG_GPIO_8_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_8_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_8_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_8_FUNC_SEL_POS)
#define GLB_REG_GPIO_8_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_8_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_8_FUNC_SEL_POS))
#define GLB_REG_GPIO_8_MODE                                     GLB_REG_GPIO_8_MODE
#define GLB_REG_GPIO_8_MODE_POS                                 (15U)
#define GLB_REG_GPIO_8_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_8_MODE_MSK                                 (((1U<<GLB_REG_GPIO_8_MODE_LEN)-1)<<GLB_REG_GPIO_8_MODE_POS)
#define GLB_REG_GPIO_8_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_8_MODE_LEN)-1)<<GLB_REG_GPIO_8_MODE_POS))
#define GLB_REG_GPIO_9_IE                                       GLB_REG_GPIO_9_IE
#define GLB_REG_GPIO_9_IE_POS                                   (16U)
#define GLB_REG_GPIO_9_IE_LEN                                   (1U)
#define GLB_REG_GPIO_9_IE_MSK                                   (((1U<<GLB_REG_GPIO_9_IE_LEN)-1)<<GLB_REG_GPIO_9_IE_POS)
#define GLB_REG_GPIO_9_IE_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_IE_LEN)-1)<<GLB_REG_GPIO_9_IE_POS))
#define GLB_REG_GPIO_9_SMT                                      GLB_REG_GPIO_9_SMT
#define GLB_REG_GPIO_9_SMT_POS                                  (17U)
#define GLB_REG_GPIO_9_SMT_LEN                                  (1U)
#define GLB_REG_GPIO_9_SMT_MSK                                  (((1U<<GLB_REG_GPIO_9_SMT_LEN)-1)<<GLB_REG_GPIO_9_SMT_POS)
#define GLB_REG_GPIO_9_SMT_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_SMT_LEN)-1)<<GLB_REG_GPIO_9_SMT_POS))
#define GLB_REG_GPIO_9_DRV                                      GLB_REG_GPIO_9_DRV
#define GLB_REG_GPIO_9_DRV_POS                                  (18U)
#define GLB_REG_GPIO_9_DRV_LEN                                  (2U)
#define GLB_REG_GPIO_9_DRV_MSK                                  (((1U<<GLB_REG_GPIO_9_DRV_LEN)-1)<<GLB_REG_GPIO_9_DRV_POS)
#define GLB_REG_GPIO_9_DRV_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_DRV_LEN)-1)<<GLB_REG_GPIO_9_DRV_POS))
#define GLB_REG_GPIO_9_PU                                       GLB_REG_GPIO_9_PU
#define GLB_REG_GPIO_9_PU_POS                                   (20U)
#define GLB_REG_GPIO_9_PU_LEN                                   (1U)
#define GLB_REG_GPIO_9_PU_MSK                                   (((1U<<GLB_REG_GPIO_9_PU_LEN)-1)<<GLB_REG_GPIO_9_PU_POS)
#define GLB_REG_GPIO_9_PU_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_PU_LEN)-1)<<GLB_REG_GPIO_9_PU_POS))
#define GLB_REG_GPIO_9_PD                                       GLB_REG_GPIO_9_PD
#define GLB_REG_GPIO_9_PD_POS                                   (21U)
#define GLB_REG_GPIO_9_PD_LEN                                   (1U)
#define GLB_REG_GPIO_9_PD_MSK                                   (((1U<<GLB_REG_GPIO_9_PD_LEN)-1)<<GLB_REG_GPIO_9_PD_POS)
#define GLB_REG_GPIO_9_PD_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_PD_LEN)-1)<<GLB_REG_GPIO_9_PD_POS))
#define GLB_REG_GPIO_9_FUNC_SEL                                 GLB_REG_GPIO_9_FUNC_SEL
#define GLB_REG_GPIO_9_FUNC_SEL_POS                             (24U)
#define GLB_REG_GPIO_9_FUNC_SEL_LEN                             (5U)
#define GLB_REG_GPIO_9_FUNC_SEL_MSK                             (((1U<<GLB_REG_GPIO_9_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_9_FUNC_SEL_POS)
#define GLB_REG_GPIO_9_FUNC_SEL_UMSK                            (~(((1U<<GLB_REG_GPIO_9_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_9_FUNC_SEL_POS))
#define GLB_REG_GPIO_9_MODE                                     GLB_REG_GPIO_9_MODE
#define GLB_REG_GPIO_9_MODE_POS                                 (31U)
#define GLB_REG_GPIO_9_MODE_LEN                                 (1U)
#define GLB_REG_GPIO_9_MODE_MSK                                 (((1U<<GLB_REG_GPIO_9_MODE_LEN)-1)<<GLB_REG_GPIO_9_MODE_POS)
#define GLB_REG_GPIO_9_MODE_UMSK                                (~(((1U<<GLB_REG_GPIO_9_MODE_LEN)-1)<<GLB_REG_GPIO_9_MODE_POS))

/* 0x114 : GPIO_CFGCTL5 */
#define GLB_GPIO_CFGCTL5_OFFSET                                 (0x114)
#define GLB_REG_GPIO_10_IE                                      GLB_REG_GPIO_10_IE
#define GLB_REG_GPIO_10_IE_POS                                  (0U)
#define GLB_REG_GPIO_10_IE_LEN                                  (1U)
#define GLB_REG_GPIO_10_IE_MSK                                  (((1U<<GLB_REG_GPIO_10_IE_LEN)-1)<<GLB_REG_GPIO_10_IE_POS)
#define GLB_REG_GPIO_10_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_IE_LEN)-1)<<GLB_REG_GPIO_10_IE_POS))
#define GLB_REG_GPIO_10_SMT                                     GLB_REG_GPIO_10_SMT
#define GLB_REG_GPIO_10_SMT_POS                                 (1U)
#define GLB_REG_GPIO_10_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_10_SMT_MSK                                 (((1U<<GLB_REG_GPIO_10_SMT_LEN)-1)<<GLB_REG_GPIO_10_SMT_POS)
#define GLB_REG_GPIO_10_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_10_SMT_LEN)-1)<<GLB_REG_GPIO_10_SMT_POS))
#define GLB_REG_GPIO_10_DRV                                     GLB_REG_GPIO_10_DRV
#define GLB_REG_GPIO_10_DRV_POS                                 (2U)
#define GLB_REG_GPIO_10_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_10_DRV_MSK                                 (((1U<<GLB_REG_GPIO_10_DRV_LEN)-1)<<GLB_REG_GPIO_10_DRV_POS)
#define GLB_REG_GPIO_10_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_10_DRV_LEN)-1)<<GLB_REG_GPIO_10_DRV_POS))
#define GLB_REG_GPIO_10_PU                                      GLB_REG_GPIO_10_PU
#define GLB_REG_GPIO_10_PU_POS                                  (4U)
#define GLB_REG_GPIO_10_PU_LEN                                  (1U)
#define GLB_REG_GPIO_10_PU_MSK                                  (((1U<<GLB_REG_GPIO_10_PU_LEN)-1)<<GLB_REG_GPIO_10_PU_POS)
#define GLB_REG_GPIO_10_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_PU_LEN)-1)<<GLB_REG_GPIO_10_PU_POS))
#define GLB_REG_GPIO_10_PD                                      GLB_REG_GPIO_10_PD
#define GLB_REG_GPIO_10_PD_POS                                  (5U)
#define GLB_REG_GPIO_10_PD_LEN                                  (1U)
#define GLB_REG_GPIO_10_PD_MSK                                  (((1U<<GLB_REG_GPIO_10_PD_LEN)-1)<<GLB_REG_GPIO_10_PD_POS)
#define GLB_REG_GPIO_10_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_PD_LEN)-1)<<GLB_REG_GPIO_10_PD_POS))
#define GLB_REG_GPIO_10_FUNC_SEL                                GLB_REG_GPIO_10_FUNC_SEL
#define GLB_REG_GPIO_10_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_10_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_10_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_10_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_10_FUNC_SEL_POS)
#define GLB_REG_GPIO_10_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_10_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_10_FUNC_SEL_POS))
#define GLB_REG_GPIO_10_MODE                                    GLB_REG_GPIO_10_MODE
#define GLB_REG_GPIO_10_MODE_POS                                (15U)
#define GLB_REG_GPIO_10_MODE_LEN                                (1U)
#define GLB_REG_GPIO_10_MODE_MSK                                (((1U<<GLB_REG_GPIO_10_MODE_LEN)-1)<<GLB_REG_GPIO_10_MODE_POS)
#define GLB_REG_GPIO_10_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_10_MODE_LEN)-1)<<GLB_REG_GPIO_10_MODE_POS))
#define GLB_REG_GPIO_11_IE                                      GLB_REG_GPIO_11_IE
#define GLB_REG_GPIO_11_IE_POS                                  (16U)
#define GLB_REG_GPIO_11_IE_LEN                                  (1U)
#define GLB_REG_GPIO_11_IE_MSK                                  (((1U<<GLB_REG_GPIO_11_IE_LEN)-1)<<GLB_REG_GPIO_11_IE_POS)
#define GLB_REG_GPIO_11_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_IE_LEN)-1)<<GLB_REG_GPIO_11_IE_POS))
#define GLB_REG_GPIO_11_SMT                                     GLB_REG_GPIO_11_SMT
#define GLB_REG_GPIO_11_SMT_POS                                 (17U)
#define GLB_REG_GPIO_11_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_11_SMT_MSK                                 (((1U<<GLB_REG_GPIO_11_SMT_LEN)-1)<<GLB_REG_GPIO_11_SMT_POS)
#define GLB_REG_GPIO_11_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_11_SMT_LEN)-1)<<GLB_REG_GPIO_11_SMT_POS))
#define GLB_REG_GPIO_11_DRV                                     GLB_REG_GPIO_11_DRV
#define GLB_REG_GPIO_11_DRV_POS                                 (18U)
#define GLB_REG_GPIO_11_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_11_DRV_MSK                                 (((1U<<GLB_REG_GPIO_11_DRV_LEN)-1)<<GLB_REG_GPIO_11_DRV_POS)
#define GLB_REG_GPIO_11_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_11_DRV_LEN)-1)<<GLB_REG_GPIO_11_DRV_POS))
#define GLB_REG_GPIO_11_PU                                      GLB_REG_GPIO_11_PU
#define GLB_REG_GPIO_11_PU_POS                                  (20U)
#define GLB_REG_GPIO_11_PU_LEN                                  (1U)
#define GLB_REG_GPIO_11_PU_MSK                                  (((1U<<GLB_REG_GPIO_11_PU_LEN)-1)<<GLB_REG_GPIO_11_PU_POS)
#define GLB_REG_GPIO_11_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_PU_LEN)-1)<<GLB_REG_GPIO_11_PU_POS))
#define GLB_REG_GPIO_11_PD                                      GLB_REG_GPIO_11_PD
#define GLB_REG_GPIO_11_PD_POS                                  (21U)
#define GLB_REG_GPIO_11_PD_LEN                                  (1U)
#define GLB_REG_GPIO_11_PD_MSK                                  (((1U<<GLB_REG_GPIO_11_PD_LEN)-1)<<GLB_REG_GPIO_11_PD_POS)
#define GLB_REG_GPIO_11_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_PD_LEN)-1)<<GLB_REG_GPIO_11_PD_POS))
#define GLB_REG_GPIO_11_FUNC_SEL                                GLB_REG_GPIO_11_FUNC_SEL
#define GLB_REG_GPIO_11_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_11_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_11_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_11_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_11_FUNC_SEL_POS)
#define GLB_REG_GPIO_11_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_11_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_11_FUNC_SEL_POS))
#define GLB_REG_GPIO_11_MODE                                    GLB_REG_GPIO_11_MODE
#define GLB_REG_GPIO_11_MODE_POS                                (31U)
#define GLB_REG_GPIO_11_MODE_LEN                                (1U)
#define GLB_REG_GPIO_11_MODE_MSK                                (((1U<<GLB_REG_GPIO_11_MODE_LEN)-1)<<GLB_REG_GPIO_11_MODE_POS)
#define GLB_REG_GPIO_11_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_11_MODE_LEN)-1)<<GLB_REG_GPIO_11_MODE_POS))

/* 0x118 : GPIO_CFGCTL6 */
#define GLB_GPIO_CFGCTL6_OFFSET                                 (0x118)
#define GLB_REG_GPIO_12_IE                                      GLB_REG_GPIO_12_IE
#define GLB_REG_GPIO_12_IE_POS                                  (0U)
#define GLB_REG_GPIO_12_IE_LEN                                  (1U)
#define GLB_REG_GPIO_12_IE_MSK                                  (((1U<<GLB_REG_GPIO_12_IE_LEN)-1)<<GLB_REG_GPIO_12_IE_POS)
#define GLB_REG_GPIO_12_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_IE_LEN)-1)<<GLB_REG_GPIO_12_IE_POS))
#define GLB_REG_GPIO_12_SMT                                     GLB_REG_GPIO_12_SMT
#define GLB_REG_GPIO_12_SMT_POS                                 (1U)
#define GLB_REG_GPIO_12_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_12_SMT_MSK                                 (((1U<<GLB_REG_GPIO_12_SMT_LEN)-1)<<GLB_REG_GPIO_12_SMT_POS)
#define GLB_REG_GPIO_12_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_12_SMT_LEN)-1)<<GLB_REG_GPIO_12_SMT_POS))
#define GLB_REG_GPIO_12_DRV                                     GLB_REG_GPIO_12_DRV
#define GLB_REG_GPIO_12_DRV_POS                                 (2U)
#define GLB_REG_GPIO_12_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_12_DRV_MSK                                 (((1U<<GLB_REG_GPIO_12_DRV_LEN)-1)<<GLB_REG_GPIO_12_DRV_POS)
#define GLB_REG_GPIO_12_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_12_DRV_LEN)-1)<<GLB_REG_GPIO_12_DRV_POS))
#define GLB_REG_GPIO_12_PU                                      GLB_REG_GPIO_12_PU
#define GLB_REG_GPIO_12_PU_POS                                  (4U)
#define GLB_REG_GPIO_12_PU_LEN                                  (1U)
#define GLB_REG_GPIO_12_PU_MSK                                  (((1U<<GLB_REG_GPIO_12_PU_LEN)-1)<<GLB_REG_GPIO_12_PU_POS)
#define GLB_REG_GPIO_12_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_PU_LEN)-1)<<GLB_REG_GPIO_12_PU_POS))
#define GLB_REG_GPIO_12_PD                                      GLB_REG_GPIO_12_PD
#define GLB_REG_GPIO_12_PD_POS                                  (5U)
#define GLB_REG_GPIO_12_PD_LEN                                  (1U)
#define GLB_REG_GPIO_12_PD_MSK                                  (((1U<<GLB_REG_GPIO_12_PD_LEN)-1)<<GLB_REG_GPIO_12_PD_POS)
#define GLB_REG_GPIO_12_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_PD_LEN)-1)<<GLB_REG_GPIO_12_PD_POS))
#define GLB_REG_GPIO_12_FUNC_SEL                                GLB_REG_GPIO_12_FUNC_SEL
#define GLB_REG_GPIO_12_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_12_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_12_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_12_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_12_FUNC_SEL_POS)
#define GLB_REG_GPIO_12_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_12_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_12_FUNC_SEL_POS))
#define GLB_REG_GPIO_12_MODE                                    GLB_REG_GPIO_12_MODE
#define GLB_REG_GPIO_12_MODE_POS                                (15U)
#define GLB_REG_GPIO_12_MODE_LEN                                (1U)
#define GLB_REG_GPIO_12_MODE_MSK                                (((1U<<GLB_REG_GPIO_12_MODE_LEN)-1)<<GLB_REG_GPIO_12_MODE_POS)
#define GLB_REG_GPIO_12_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_12_MODE_LEN)-1)<<GLB_REG_GPIO_12_MODE_POS))
#define GLB_REG_GPIO_13_IE                                      GLB_REG_GPIO_13_IE
#define GLB_REG_GPIO_13_IE_POS                                  (16U)
#define GLB_REG_GPIO_13_IE_LEN                                  (1U)
#define GLB_REG_GPIO_13_IE_MSK                                  (((1U<<GLB_REG_GPIO_13_IE_LEN)-1)<<GLB_REG_GPIO_13_IE_POS)
#define GLB_REG_GPIO_13_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_IE_LEN)-1)<<GLB_REG_GPIO_13_IE_POS))
#define GLB_REG_GPIO_13_SMT                                     GLB_REG_GPIO_13_SMT
#define GLB_REG_GPIO_13_SMT_POS                                 (17U)
#define GLB_REG_GPIO_13_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_13_SMT_MSK                                 (((1U<<GLB_REG_GPIO_13_SMT_LEN)-1)<<GLB_REG_GPIO_13_SMT_POS)
#define GLB_REG_GPIO_13_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_13_SMT_LEN)-1)<<GLB_REG_GPIO_13_SMT_POS))
#define GLB_REG_GPIO_13_DRV                                     GLB_REG_GPIO_13_DRV
#define GLB_REG_GPIO_13_DRV_POS                                 (18U)
#define GLB_REG_GPIO_13_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_13_DRV_MSK                                 (((1U<<GLB_REG_GPIO_13_DRV_LEN)-1)<<GLB_REG_GPIO_13_DRV_POS)
#define GLB_REG_GPIO_13_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_13_DRV_LEN)-1)<<GLB_REG_GPIO_13_DRV_POS))
#define GLB_REG_GPIO_13_PU                                      GLB_REG_GPIO_13_PU
#define GLB_REG_GPIO_13_PU_POS                                  (20U)
#define GLB_REG_GPIO_13_PU_LEN                                  (1U)
#define GLB_REG_GPIO_13_PU_MSK                                  (((1U<<GLB_REG_GPIO_13_PU_LEN)-1)<<GLB_REG_GPIO_13_PU_POS)
#define GLB_REG_GPIO_13_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_PU_LEN)-1)<<GLB_REG_GPIO_13_PU_POS))
#define GLB_REG_GPIO_13_PD                                      GLB_REG_GPIO_13_PD
#define GLB_REG_GPIO_13_PD_POS                                  (21U)
#define GLB_REG_GPIO_13_PD_LEN                                  (1U)
#define GLB_REG_GPIO_13_PD_MSK                                  (((1U<<GLB_REG_GPIO_13_PD_LEN)-1)<<GLB_REG_GPIO_13_PD_POS)
#define GLB_REG_GPIO_13_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_PD_LEN)-1)<<GLB_REG_GPIO_13_PD_POS))
#define GLB_REG_GPIO_13_FUNC_SEL                                GLB_REG_GPIO_13_FUNC_SEL
#define GLB_REG_GPIO_13_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_13_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_13_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_13_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_13_FUNC_SEL_POS)
#define GLB_REG_GPIO_13_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_13_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_13_FUNC_SEL_POS))
#define GLB_REG_GPIO_13_MODE                                    GLB_REG_GPIO_13_MODE
#define GLB_REG_GPIO_13_MODE_POS                                (31U)
#define GLB_REG_GPIO_13_MODE_LEN                                (1U)
#define GLB_REG_GPIO_13_MODE_MSK                                (((1U<<GLB_REG_GPIO_13_MODE_LEN)-1)<<GLB_REG_GPIO_13_MODE_POS)
#define GLB_REG_GPIO_13_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_13_MODE_LEN)-1)<<GLB_REG_GPIO_13_MODE_POS))

/* 0x11C : GPIO_CFGCTL7 */
#define GLB_GPIO_CFGCTL7_OFFSET                                 (0x11C)
#define GLB_REG_GPIO_14_IE                                      GLB_REG_GPIO_14_IE
#define GLB_REG_GPIO_14_IE_POS                                  (0U)
#define GLB_REG_GPIO_14_IE_LEN                                  (1U)
#define GLB_REG_GPIO_14_IE_MSK                                  (((1U<<GLB_REG_GPIO_14_IE_LEN)-1)<<GLB_REG_GPIO_14_IE_POS)
#define GLB_REG_GPIO_14_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_IE_LEN)-1)<<GLB_REG_GPIO_14_IE_POS))
#define GLB_REG_GPIO_14_SMT                                     GLB_REG_GPIO_14_SMT
#define GLB_REG_GPIO_14_SMT_POS                                 (1U)
#define GLB_REG_GPIO_14_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_14_SMT_MSK                                 (((1U<<GLB_REG_GPIO_14_SMT_LEN)-1)<<GLB_REG_GPIO_14_SMT_POS)
#define GLB_REG_GPIO_14_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_14_SMT_LEN)-1)<<GLB_REG_GPIO_14_SMT_POS))
#define GLB_REG_GPIO_14_DRV                                     GLB_REG_GPIO_14_DRV
#define GLB_REG_GPIO_14_DRV_POS                                 (2U)
#define GLB_REG_GPIO_14_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_14_DRV_MSK                                 (((1U<<GLB_REG_GPIO_14_DRV_LEN)-1)<<GLB_REG_GPIO_14_DRV_POS)
#define GLB_REG_GPIO_14_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_14_DRV_LEN)-1)<<GLB_REG_GPIO_14_DRV_POS))
#define GLB_REG_GPIO_14_PU                                      GLB_REG_GPIO_14_PU
#define GLB_REG_GPIO_14_PU_POS                                  (4U)
#define GLB_REG_GPIO_14_PU_LEN                                  (1U)
#define GLB_REG_GPIO_14_PU_MSK                                  (((1U<<GLB_REG_GPIO_14_PU_LEN)-1)<<GLB_REG_GPIO_14_PU_POS)
#define GLB_REG_GPIO_14_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_PU_LEN)-1)<<GLB_REG_GPIO_14_PU_POS))
#define GLB_REG_GPIO_14_PD                                      GLB_REG_GPIO_14_PD
#define GLB_REG_GPIO_14_PD_POS                                  (5U)
#define GLB_REG_GPIO_14_PD_LEN                                  (1U)
#define GLB_REG_GPIO_14_PD_MSK                                  (((1U<<GLB_REG_GPIO_14_PD_LEN)-1)<<GLB_REG_GPIO_14_PD_POS)
#define GLB_REG_GPIO_14_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_PD_LEN)-1)<<GLB_REG_GPIO_14_PD_POS))
#define GLB_REG_GPIO_14_FUNC_SEL                                GLB_REG_GPIO_14_FUNC_SEL
#define GLB_REG_GPIO_14_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_14_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_14_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_14_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_14_FUNC_SEL_POS)
#define GLB_REG_GPIO_14_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_14_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_14_FUNC_SEL_POS))
#define GLB_REG_GPIO_14_MODE                                    GLB_REG_GPIO_14_MODE
#define GLB_REG_GPIO_14_MODE_POS                                (15U)
#define GLB_REG_GPIO_14_MODE_LEN                                (1U)
#define GLB_REG_GPIO_14_MODE_MSK                                (((1U<<GLB_REG_GPIO_14_MODE_LEN)-1)<<GLB_REG_GPIO_14_MODE_POS)
#define GLB_REG_GPIO_14_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_14_MODE_LEN)-1)<<GLB_REG_GPIO_14_MODE_POS))
#define GLB_REG_GPIO_15_IE                                      GLB_REG_GPIO_15_IE
#define GLB_REG_GPIO_15_IE_POS                                  (16U)
#define GLB_REG_GPIO_15_IE_LEN                                  (1U)
#define GLB_REG_GPIO_15_IE_MSK                                  (((1U<<GLB_REG_GPIO_15_IE_LEN)-1)<<GLB_REG_GPIO_15_IE_POS)
#define GLB_REG_GPIO_15_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_IE_LEN)-1)<<GLB_REG_GPIO_15_IE_POS))
#define GLB_REG_GPIO_15_SMT                                     GLB_REG_GPIO_15_SMT
#define GLB_REG_GPIO_15_SMT_POS                                 (17U)
#define GLB_REG_GPIO_15_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_15_SMT_MSK                                 (((1U<<GLB_REG_GPIO_15_SMT_LEN)-1)<<GLB_REG_GPIO_15_SMT_POS)
#define GLB_REG_GPIO_15_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_15_SMT_LEN)-1)<<GLB_REG_GPIO_15_SMT_POS))
#define GLB_REG_GPIO_15_DRV                                     GLB_REG_GPIO_15_DRV
#define GLB_REG_GPIO_15_DRV_POS                                 (18U)
#define GLB_REG_GPIO_15_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_15_DRV_MSK                                 (((1U<<GLB_REG_GPIO_15_DRV_LEN)-1)<<GLB_REG_GPIO_15_DRV_POS)
#define GLB_REG_GPIO_15_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_15_DRV_LEN)-1)<<GLB_REG_GPIO_15_DRV_POS))
#define GLB_REG_GPIO_15_PU                                      GLB_REG_GPIO_15_PU
#define GLB_REG_GPIO_15_PU_POS                                  (20U)
#define GLB_REG_GPIO_15_PU_LEN                                  (1U)
#define GLB_REG_GPIO_15_PU_MSK                                  (((1U<<GLB_REG_GPIO_15_PU_LEN)-1)<<GLB_REG_GPIO_15_PU_POS)
#define GLB_REG_GPIO_15_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_PU_LEN)-1)<<GLB_REG_GPIO_15_PU_POS))
#define GLB_REG_GPIO_15_PD                                      GLB_REG_GPIO_15_PD
#define GLB_REG_GPIO_15_PD_POS                                  (21U)
#define GLB_REG_GPIO_15_PD_LEN                                  (1U)
#define GLB_REG_GPIO_15_PD_MSK                                  (((1U<<GLB_REG_GPIO_15_PD_LEN)-1)<<GLB_REG_GPIO_15_PD_POS)
#define GLB_REG_GPIO_15_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_PD_LEN)-1)<<GLB_REG_GPIO_15_PD_POS))
#define GLB_REG_GPIO_15_FUNC_SEL                                GLB_REG_GPIO_15_FUNC_SEL
#define GLB_REG_GPIO_15_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_15_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_15_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_15_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_15_FUNC_SEL_POS)
#define GLB_REG_GPIO_15_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_15_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_15_FUNC_SEL_POS))
#define GLB_REG_GPIO_15_MODE                                    GLB_REG_GPIO_15_MODE
#define GLB_REG_GPIO_15_MODE_POS                                (31U)
#define GLB_REG_GPIO_15_MODE_LEN                                (1U)
#define GLB_REG_GPIO_15_MODE_MSK                                (((1U<<GLB_REG_GPIO_15_MODE_LEN)-1)<<GLB_REG_GPIO_15_MODE_POS)
#define GLB_REG_GPIO_15_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_15_MODE_LEN)-1)<<GLB_REG_GPIO_15_MODE_POS))

/* 0x120 : GPIO_CFGCTL8 */
#define GLB_GPIO_CFGCTL8_OFFSET                                 (0x120)
#define GLB_REG_GPIO_16_IE                                      GLB_REG_GPIO_16_IE
#define GLB_REG_GPIO_16_IE_POS                                  (0U)
#define GLB_REG_GPIO_16_IE_LEN                                  (1U)
#define GLB_REG_GPIO_16_IE_MSK                                  (((1U<<GLB_REG_GPIO_16_IE_LEN)-1)<<GLB_REG_GPIO_16_IE_POS)
#define GLB_REG_GPIO_16_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_IE_LEN)-1)<<GLB_REG_GPIO_16_IE_POS))
#define GLB_REG_GPIO_16_SMT                                     GLB_REG_GPIO_16_SMT
#define GLB_REG_GPIO_16_SMT_POS                                 (1U)
#define GLB_REG_GPIO_16_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_16_SMT_MSK                                 (((1U<<GLB_REG_GPIO_16_SMT_LEN)-1)<<GLB_REG_GPIO_16_SMT_POS)
#define GLB_REG_GPIO_16_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_16_SMT_LEN)-1)<<GLB_REG_GPIO_16_SMT_POS))
#define GLB_REG_GPIO_16_DRV                                     GLB_REG_GPIO_16_DRV
#define GLB_REG_GPIO_16_DRV_POS                                 (2U)
#define GLB_REG_GPIO_16_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_16_DRV_MSK                                 (((1U<<GLB_REG_GPIO_16_DRV_LEN)-1)<<GLB_REG_GPIO_16_DRV_POS)
#define GLB_REG_GPIO_16_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_16_DRV_LEN)-1)<<GLB_REG_GPIO_16_DRV_POS))
#define GLB_REG_GPIO_16_PU                                      GLB_REG_GPIO_16_PU
#define GLB_REG_GPIO_16_PU_POS                                  (4U)
#define GLB_REG_GPIO_16_PU_LEN                                  (1U)
#define GLB_REG_GPIO_16_PU_MSK                                  (((1U<<GLB_REG_GPIO_16_PU_LEN)-1)<<GLB_REG_GPIO_16_PU_POS)
#define GLB_REG_GPIO_16_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_PU_LEN)-1)<<GLB_REG_GPIO_16_PU_POS))
#define GLB_REG_GPIO_16_PD                                      GLB_REG_GPIO_16_PD
#define GLB_REG_GPIO_16_PD_POS                                  (5U)
#define GLB_REG_GPIO_16_PD_LEN                                  (1U)
#define GLB_REG_GPIO_16_PD_MSK                                  (((1U<<GLB_REG_GPIO_16_PD_LEN)-1)<<GLB_REG_GPIO_16_PD_POS)
#define GLB_REG_GPIO_16_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_PD_LEN)-1)<<GLB_REG_GPIO_16_PD_POS))
#define GLB_REG_GPIO_16_FUNC_SEL                                GLB_REG_GPIO_16_FUNC_SEL
#define GLB_REG_GPIO_16_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_16_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_16_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_16_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_16_FUNC_SEL_POS)
#define GLB_REG_GPIO_16_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_16_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_16_FUNC_SEL_POS))
#define GLB_REG_GPIO_16_MODE                                    GLB_REG_GPIO_16_MODE
#define GLB_REG_GPIO_16_MODE_POS                                (15U)
#define GLB_REG_GPIO_16_MODE_LEN                                (1U)
#define GLB_REG_GPIO_16_MODE_MSK                                (((1U<<GLB_REG_GPIO_16_MODE_LEN)-1)<<GLB_REG_GPIO_16_MODE_POS)
#define GLB_REG_GPIO_16_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_16_MODE_LEN)-1)<<GLB_REG_GPIO_16_MODE_POS))
#define GLB_REG_GPIO_17_IE                                      GLB_REG_GPIO_17_IE
#define GLB_REG_GPIO_17_IE_POS                                  (16U)
#define GLB_REG_GPIO_17_IE_LEN                                  (1U)
#define GLB_REG_GPIO_17_IE_MSK                                  (((1U<<GLB_REG_GPIO_17_IE_LEN)-1)<<GLB_REG_GPIO_17_IE_POS)
#define GLB_REG_GPIO_17_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_IE_LEN)-1)<<GLB_REG_GPIO_17_IE_POS))
#define GLB_REG_GPIO_17_SMT                                     GLB_REG_GPIO_17_SMT
#define GLB_REG_GPIO_17_SMT_POS                                 (17U)
#define GLB_REG_GPIO_17_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_17_SMT_MSK                                 (((1U<<GLB_REG_GPIO_17_SMT_LEN)-1)<<GLB_REG_GPIO_17_SMT_POS)
#define GLB_REG_GPIO_17_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_17_SMT_LEN)-1)<<GLB_REG_GPIO_17_SMT_POS))
#define GLB_REG_GPIO_17_DRV                                     GLB_REG_GPIO_17_DRV
#define GLB_REG_GPIO_17_DRV_POS                                 (18U)
#define GLB_REG_GPIO_17_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_17_DRV_MSK                                 (((1U<<GLB_REG_GPIO_17_DRV_LEN)-1)<<GLB_REG_GPIO_17_DRV_POS)
#define GLB_REG_GPIO_17_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_17_DRV_LEN)-1)<<GLB_REG_GPIO_17_DRV_POS))
#define GLB_REG_GPIO_17_PU                                      GLB_REG_GPIO_17_PU
#define GLB_REG_GPIO_17_PU_POS                                  (20U)
#define GLB_REG_GPIO_17_PU_LEN                                  (1U)
#define GLB_REG_GPIO_17_PU_MSK                                  (((1U<<GLB_REG_GPIO_17_PU_LEN)-1)<<GLB_REG_GPIO_17_PU_POS)
#define GLB_REG_GPIO_17_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_PU_LEN)-1)<<GLB_REG_GPIO_17_PU_POS))
#define GLB_REG_GPIO_17_PD                                      GLB_REG_GPIO_17_PD
#define GLB_REG_GPIO_17_PD_POS                                  (21U)
#define GLB_REG_GPIO_17_PD_LEN                                  (1U)
#define GLB_REG_GPIO_17_PD_MSK                                  (((1U<<GLB_REG_GPIO_17_PD_LEN)-1)<<GLB_REG_GPIO_17_PD_POS)
#define GLB_REG_GPIO_17_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_PD_LEN)-1)<<GLB_REG_GPIO_17_PD_POS))
#define GLB_REG_GPIO_17_FUNC_SEL                                GLB_REG_GPIO_17_FUNC_SEL
#define GLB_REG_GPIO_17_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_17_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_17_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_17_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_17_FUNC_SEL_POS)
#define GLB_REG_GPIO_17_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_17_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_17_FUNC_SEL_POS))
#define GLB_REG_GPIO_17_MODE                                    GLB_REG_GPIO_17_MODE
#define GLB_REG_GPIO_17_MODE_POS                                (31U)
#define GLB_REG_GPIO_17_MODE_LEN                                (1U)
#define GLB_REG_GPIO_17_MODE_MSK                                (((1U<<GLB_REG_GPIO_17_MODE_LEN)-1)<<GLB_REG_GPIO_17_MODE_POS)
#define GLB_REG_GPIO_17_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_17_MODE_LEN)-1)<<GLB_REG_GPIO_17_MODE_POS))

/* 0x124 : GPIO_CFGCTL9 */
#define GLB_GPIO_CFGCTL9_OFFSET                                 (0x124)
#define GLB_REG_GPIO_18_IE                                      GLB_REG_GPIO_18_IE
#define GLB_REG_GPIO_18_IE_POS                                  (0U)
#define GLB_REG_GPIO_18_IE_LEN                                  (1U)
#define GLB_REG_GPIO_18_IE_MSK                                  (((1U<<GLB_REG_GPIO_18_IE_LEN)-1)<<GLB_REG_GPIO_18_IE_POS)
#define GLB_REG_GPIO_18_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_IE_LEN)-1)<<GLB_REG_GPIO_18_IE_POS))
#define GLB_REG_GPIO_18_SMT                                     GLB_REG_GPIO_18_SMT
#define GLB_REG_GPIO_18_SMT_POS                                 (1U)
#define GLB_REG_GPIO_18_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_18_SMT_MSK                                 (((1U<<GLB_REG_GPIO_18_SMT_LEN)-1)<<GLB_REG_GPIO_18_SMT_POS)
#define GLB_REG_GPIO_18_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_18_SMT_LEN)-1)<<GLB_REG_GPIO_18_SMT_POS))
#define GLB_REG_GPIO_18_DRV                                     GLB_REG_GPIO_18_DRV
#define GLB_REG_GPIO_18_DRV_POS                                 (2U)
#define GLB_REG_GPIO_18_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_18_DRV_MSK                                 (((1U<<GLB_REG_GPIO_18_DRV_LEN)-1)<<GLB_REG_GPIO_18_DRV_POS)
#define GLB_REG_GPIO_18_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_18_DRV_LEN)-1)<<GLB_REG_GPIO_18_DRV_POS))
#define GLB_REG_GPIO_18_PU                                      GLB_REG_GPIO_18_PU
#define GLB_REG_GPIO_18_PU_POS                                  (4U)
#define GLB_REG_GPIO_18_PU_LEN                                  (1U)
#define GLB_REG_GPIO_18_PU_MSK                                  (((1U<<GLB_REG_GPIO_18_PU_LEN)-1)<<GLB_REG_GPIO_18_PU_POS)
#define GLB_REG_GPIO_18_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_PU_LEN)-1)<<GLB_REG_GPIO_18_PU_POS))
#define GLB_REG_GPIO_18_PD                                      GLB_REG_GPIO_18_PD
#define GLB_REG_GPIO_18_PD_POS                                  (5U)
#define GLB_REG_GPIO_18_PD_LEN                                  (1U)
#define GLB_REG_GPIO_18_PD_MSK                                  (((1U<<GLB_REG_GPIO_18_PD_LEN)-1)<<GLB_REG_GPIO_18_PD_POS)
#define GLB_REG_GPIO_18_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_PD_LEN)-1)<<GLB_REG_GPIO_18_PD_POS))
#define GLB_REG_GPIO_18_FUNC_SEL                                GLB_REG_GPIO_18_FUNC_SEL
#define GLB_REG_GPIO_18_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_18_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_18_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_18_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_18_FUNC_SEL_POS)
#define GLB_REG_GPIO_18_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_18_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_18_FUNC_SEL_POS))
#define GLB_REG_GPIO_18_MODE                                    GLB_REG_GPIO_18_MODE
#define GLB_REG_GPIO_18_MODE_POS                                (15U)
#define GLB_REG_GPIO_18_MODE_LEN                                (1U)
#define GLB_REG_GPIO_18_MODE_MSK                                (((1U<<GLB_REG_GPIO_18_MODE_LEN)-1)<<GLB_REG_GPIO_18_MODE_POS)
#define GLB_REG_GPIO_18_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_18_MODE_LEN)-1)<<GLB_REG_GPIO_18_MODE_POS))
#define GLB_REG_GPIO_19_IE                                      GLB_REG_GPIO_19_IE
#define GLB_REG_GPIO_19_IE_POS                                  (16U)
#define GLB_REG_GPIO_19_IE_LEN                                  (1U)
#define GLB_REG_GPIO_19_IE_MSK                                  (((1U<<GLB_REG_GPIO_19_IE_LEN)-1)<<GLB_REG_GPIO_19_IE_POS)
#define GLB_REG_GPIO_19_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_IE_LEN)-1)<<GLB_REG_GPIO_19_IE_POS))
#define GLB_REG_GPIO_19_SMT                                     GLB_REG_GPIO_19_SMT
#define GLB_REG_GPIO_19_SMT_POS                                 (17U)
#define GLB_REG_GPIO_19_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_19_SMT_MSK                                 (((1U<<GLB_REG_GPIO_19_SMT_LEN)-1)<<GLB_REG_GPIO_19_SMT_POS)
#define GLB_REG_GPIO_19_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_19_SMT_LEN)-1)<<GLB_REG_GPIO_19_SMT_POS))
#define GLB_REG_GPIO_19_DRV                                     GLB_REG_GPIO_19_DRV
#define GLB_REG_GPIO_19_DRV_POS                                 (18U)
#define GLB_REG_GPIO_19_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_19_DRV_MSK                                 (((1U<<GLB_REG_GPIO_19_DRV_LEN)-1)<<GLB_REG_GPIO_19_DRV_POS)
#define GLB_REG_GPIO_19_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_19_DRV_LEN)-1)<<GLB_REG_GPIO_19_DRV_POS))
#define GLB_REG_GPIO_19_PU                                      GLB_REG_GPIO_19_PU
#define GLB_REG_GPIO_19_PU_POS                                  (20U)
#define GLB_REG_GPIO_19_PU_LEN                                  (1U)
#define GLB_REG_GPIO_19_PU_MSK                                  (((1U<<GLB_REG_GPIO_19_PU_LEN)-1)<<GLB_REG_GPIO_19_PU_POS)
#define GLB_REG_GPIO_19_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_PU_LEN)-1)<<GLB_REG_GPIO_19_PU_POS))
#define GLB_REG_GPIO_19_PD                                      GLB_REG_GPIO_19_PD
#define GLB_REG_GPIO_19_PD_POS                                  (21U)
#define GLB_REG_GPIO_19_PD_LEN                                  (1U)
#define GLB_REG_GPIO_19_PD_MSK                                  (((1U<<GLB_REG_GPIO_19_PD_LEN)-1)<<GLB_REG_GPIO_19_PD_POS)
#define GLB_REG_GPIO_19_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_PD_LEN)-1)<<GLB_REG_GPIO_19_PD_POS))
#define GLB_REG_GPIO_19_FUNC_SEL                                GLB_REG_GPIO_19_FUNC_SEL
#define GLB_REG_GPIO_19_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_19_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_19_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_19_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_19_FUNC_SEL_POS)
#define GLB_REG_GPIO_19_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_19_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_19_FUNC_SEL_POS))
#define GLB_REG_GPIO_19_MODE                                    GLB_REG_GPIO_19_MODE
#define GLB_REG_GPIO_19_MODE_POS                                (31U)
#define GLB_REG_GPIO_19_MODE_LEN                                (1U)
#define GLB_REG_GPIO_19_MODE_MSK                                (((1U<<GLB_REG_GPIO_19_MODE_LEN)-1)<<GLB_REG_GPIO_19_MODE_POS)
#define GLB_REG_GPIO_19_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_19_MODE_LEN)-1)<<GLB_REG_GPIO_19_MODE_POS))

/* 0x128 : GPIO_CFGCTL10 */
#define GLB_GPIO_CFGCTL10_OFFSET                                (0x128)
#define GLB_REG_GPIO_20_IE                                      GLB_REG_GPIO_20_IE
#define GLB_REG_GPIO_20_IE_POS                                  (0U)
#define GLB_REG_GPIO_20_IE_LEN                                  (1U)
#define GLB_REG_GPIO_20_IE_MSK                                  (((1U<<GLB_REG_GPIO_20_IE_LEN)-1)<<GLB_REG_GPIO_20_IE_POS)
#define GLB_REG_GPIO_20_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_IE_LEN)-1)<<GLB_REG_GPIO_20_IE_POS))
#define GLB_REG_GPIO_20_SMT                                     GLB_REG_GPIO_20_SMT
#define GLB_REG_GPIO_20_SMT_POS                                 (1U)
#define GLB_REG_GPIO_20_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_20_SMT_MSK                                 (((1U<<GLB_REG_GPIO_20_SMT_LEN)-1)<<GLB_REG_GPIO_20_SMT_POS)
#define GLB_REG_GPIO_20_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_20_SMT_LEN)-1)<<GLB_REG_GPIO_20_SMT_POS))
#define GLB_REG_GPIO_20_DRV                                     GLB_REG_GPIO_20_DRV
#define GLB_REG_GPIO_20_DRV_POS                                 (2U)
#define GLB_REG_GPIO_20_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_20_DRV_MSK                                 (((1U<<GLB_REG_GPIO_20_DRV_LEN)-1)<<GLB_REG_GPIO_20_DRV_POS)
#define GLB_REG_GPIO_20_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_20_DRV_LEN)-1)<<GLB_REG_GPIO_20_DRV_POS))
#define GLB_REG_GPIO_20_PU                                      GLB_REG_GPIO_20_PU
#define GLB_REG_GPIO_20_PU_POS                                  (4U)
#define GLB_REG_GPIO_20_PU_LEN                                  (1U)
#define GLB_REG_GPIO_20_PU_MSK                                  (((1U<<GLB_REG_GPIO_20_PU_LEN)-1)<<GLB_REG_GPIO_20_PU_POS)
#define GLB_REG_GPIO_20_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_PU_LEN)-1)<<GLB_REG_GPIO_20_PU_POS))
#define GLB_REG_GPIO_20_PD                                      GLB_REG_GPIO_20_PD
#define GLB_REG_GPIO_20_PD_POS                                  (5U)
#define GLB_REG_GPIO_20_PD_LEN                                  (1U)
#define GLB_REG_GPIO_20_PD_MSK                                  (((1U<<GLB_REG_GPIO_20_PD_LEN)-1)<<GLB_REG_GPIO_20_PD_POS)
#define GLB_REG_GPIO_20_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_PD_LEN)-1)<<GLB_REG_GPIO_20_PD_POS))
#define GLB_REG_GPIO_20_FUNC_SEL                                GLB_REG_GPIO_20_FUNC_SEL
#define GLB_REG_GPIO_20_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_20_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_20_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_20_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_20_FUNC_SEL_POS)
#define GLB_REG_GPIO_20_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_20_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_20_FUNC_SEL_POS))
#define GLB_REG_GPIO_20_MODE                                    GLB_REG_GPIO_20_MODE
#define GLB_REG_GPIO_20_MODE_POS                                (15U)
#define GLB_REG_GPIO_20_MODE_LEN                                (1U)
#define GLB_REG_GPIO_20_MODE_MSK                                (((1U<<GLB_REG_GPIO_20_MODE_LEN)-1)<<GLB_REG_GPIO_20_MODE_POS)
#define GLB_REG_GPIO_20_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_20_MODE_LEN)-1)<<GLB_REG_GPIO_20_MODE_POS))
#define GLB_REG_GPIO_21_IE                                      GLB_REG_GPIO_21_IE
#define GLB_REG_GPIO_21_IE_POS                                  (16U)
#define GLB_REG_GPIO_21_IE_LEN                                  (1U)
#define GLB_REG_GPIO_21_IE_MSK                                  (((1U<<GLB_REG_GPIO_21_IE_LEN)-1)<<GLB_REG_GPIO_21_IE_POS)
#define GLB_REG_GPIO_21_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_IE_LEN)-1)<<GLB_REG_GPIO_21_IE_POS))
#define GLB_REG_GPIO_21_SMT                                     GLB_REG_GPIO_21_SMT
#define GLB_REG_GPIO_21_SMT_POS                                 (17U)
#define GLB_REG_GPIO_21_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_21_SMT_MSK                                 (((1U<<GLB_REG_GPIO_21_SMT_LEN)-1)<<GLB_REG_GPIO_21_SMT_POS)
#define GLB_REG_GPIO_21_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_21_SMT_LEN)-1)<<GLB_REG_GPIO_21_SMT_POS))
#define GLB_REG_GPIO_21_DRV                                     GLB_REG_GPIO_21_DRV
#define GLB_REG_GPIO_21_DRV_POS                                 (18U)
#define GLB_REG_GPIO_21_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_21_DRV_MSK                                 (((1U<<GLB_REG_GPIO_21_DRV_LEN)-1)<<GLB_REG_GPIO_21_DRV_POS)
#define GLB_REG_GPIO_21_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_21_DRV_LEN)-1)<<GLB_REG_GPIO_21_DRV_POS))
#define GLB_REG_GPIO_21_PU                                      GLB_REG_GPIO_21_PU
#define GLB_REG_GPIO_21_PU_POS                                  (20U)
#define GLB_REG_GPIO_21_PU_LEN                                  (1U)
#define GLB_REG_GPIO_21_PU_MSK                                  (((1U<<GLB_REG_GPIO_21_PU_LEN)-1)<<GLB_REG_GPIO_21_PU_POS)
#define GLB_REG_GPIO_21_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_PU_LEN)-1)<<GLB_REG_GPIO_21_PU_POS))
#define GLB_REG_GPIO_21_PD                                      GLB_REG_GPIO_21_PD
#define GLB_REG_GPIO_21_PD_POS                                  (21U)
#define GLB_REG_GPIO_21_PD_LEN                                  (1U)
#define GLB_REG_GPIO_21_PD_MSK                                  (((1U<<GLB_REG_GPIO_21_PD_LEN)-1)<<GLB_REG_GPIO_21_PD_POS)
#define GLB_REG_GPIO_21_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_PD_LEN)-1)<<GLB_REG_GPIO_21_PD_POS))
#define GLB_REG_GPIO_21_FUNC_SEL                                GLB_REG_GPIO_21_FUNC_SEL
#define GLB_REG_GPIO_21_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_21_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_21_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_21_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_21_FUNC_SEL_POS)
#define GLB_REG_GPIO_21_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_21_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_21_FUNC_SEL_POS))
#define GLB_REG_GPIO_21_MODE                                    GLB_REG_GPIO_21_MODE
#define GLB_REG_GPIO_21_MODE_POS                                (31U)
#define GLB_REG_GPIO_21_MODE_LEN                                (1U)
#define GLB_REG_GPIO_21_MODE_MSK                                (((1U<<GLB_REG_GPIO_21_MODE_LEN)-1)<<GLB_REG_GPIO_21_MODE_POS)
#define GLB_REG_GPIO_21_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_21_MODE_LEN)-1)<<GLB_REG_GPIO_21_MODE_POS))

/* 0x12C : GPIO_CFGCTL11 */
#define GLB_GPIO_CFGCTL11_OFFSET                                (0x12C)
#define GLB_REG_GPIO_22_IE                                      GLB_REG_GPIO_22_IE
#define GLB_REG_GPIO_22_IE_POS                                  (0U)
#define GLB_REG_GPIO_22_IE_LEN                                  (1U)
#define GLB_REG_GPIO_22_IE_MSK                                  (((1U<<GLB_REG_GPIO_22_IE_LEN)-1)<<GLB_REG_GPIO_22_IE_POS)
#define GLB_REG_GPIO_22_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_IE_LEN)-1)<<GLB_REG_GPIO_22_IE_POS))
#define GLB_REG_GPIO_22_SMT                                     GLB_REG_GPIO_22_SMT
#define GLB_REG_GPIO_22_SMT_POS                                 (1U)
#define GLB_REG_GPIO_22_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_22_SMT_MSK                                 (((1U<<GLB_REG_GPIO_22_SMT_LEN)-1)<<GLB_REG_GPIO_22_SMT_POS)
#define GLB_REG_GPIO_22_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_22_SMT_LEN)-1)<<GLB_REG_GPIO_22_SMT_POS))
#define GLB_REG_GPIO_22_DRV                                     GLB_REG_GPIO_22_DRV
#define GLB_REG_GPIO_22_DRV_POS                                 (2U)
#define GLB_REG_GPIO_22_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_22_DRV_MSK                                 (((1U<<GLB_REG_GPIO_22_DRV_LEN)-1)<<GLB_REG_GPIO_22_DRV_POS)
#define GLB_REG_GPIO_22_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_22_DRV_LEN)-1)<<GLB_REG_GPIO_22_DRV_POS))
#define GLB_REG_GPIO_22_PU                                      GLB_REG_GPIO_22_PU
#define GLB_REG_GPIO_22_PU_POS                                  (4U)
#define GLB_REG_GPIO_22_PU_LEN                                  (1U)
#define GLB_REG_GPIO_22_PU_MSK                                  (((1U<<GLB_REG_GPIO_22_PU_LEN)-1)<<GLB_REG_GPIO_22_PU_POS)
#define GLB_REG_GPIO_22_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_PU_LEN)-1)<<GLB_REG_GPIO_22_PU_POS))
#define GLB_REG_GPIO_22_PD                                      GLB_REG_GPIO_22_PD
#define GLB_REG_GPIO_22_PD_POS                                  (5U)
#define GLB_REG_GPIO_22_PD_LEN                                  (1U)
#define GLB_REG_GPIO_22_PD_MSK                                  (((1U<<GLB_REG_GPIO_22_PD_LEN)-1)<<GLB_REG_GPIO_22_PD_POS)
#define GLB_REG_GPIO_22_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_PD_LEN)-1)<<GLB_REG_GPIO_22_PD_POS))
#define GLB_REG_GPIO_22_FUNC_SEL                                GLB_REG_GPIO_22_FUNC_SEL
#define GLB_REG_GPIO_22_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_22_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_22_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_22_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_22_FUNC_SEL_POS)
#define GLB_REG_GPIO_22_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_22_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_22_FUNC_SEL_POS))
#define GLB_REG_GPIO_22_MODE                                    GLB_REG_GPIO_22_MODE
#define GLB_REG_GPIO_22_MODE_POS                                (15U)
#define GLB_REG_GPIO_22_MODE_LEN                                (1U)
#define GLB_REG_GPIO_22_MODE_MSK                                (((1U<<GLB_REG_GPIO_22_MODE_LEN)-1)<<GLB_REG_GPIO_22_MODE_POS)
#define GLB_REG_GPIO_22_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_22_MODE_LEN)-1)<<GLB_REG_GPIO_22_MODE_POS))
#define GLB_REG_GPIO_23_IE                                      GLB_REG_GPIO_23_IE
#define GLB_REG_GPIO_23_IE_POS                                  (16U)
#define GLB_REG_GPIO_23_IE_LEN                                  (1U)
#define GLB_REG_GPIO_23_IE_MSK                                  (((1U<<GLB_REG_GPIO_23_IE_LEN)-1)<<GLB_REG_GPIO_23_IE_POS)
#define GLB_REG_GPIO_23_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_IE_LEN)-1)<<GLB_REG_GPIO_23_IE_POS))
#define GLB_REG_GPIO_23_SMT                                     GLB_REG_GPIO_23_SMT
#define GLB_REG_GPIO_23_SMT_POS                                 (17U)
#define GLB_REG_GPIO_23_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_23_SMT_MSK                                 (((1U<<GLB_REG_GPIO_23_SMT_LEN)-1)<<GLB_REG_GPIO_23_SMT_POS)
#define GLB_REG_GPIO_23_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_23_SMT_LEN)-1)<<GLB_REG_GPIO_23_SMT_POS))
#define GLB_REG_GPIO_23_DRV                                     GLB_REG_GPIO_23_DRV
#define GLB_REG_GPIO_23_DRV_POS                                 (18U)
#define GLB_REG_GPIO_23_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_23_DRV_MSK                                 (((1U<<GLB_REG_GPIO_23_DRV_LEN)-1)<<GLB_REG_GPIO_23_DRV_POS)
#define GLB_REG_GPIO_23_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_23_DRV_LEN)-1)<<GLB_REG_GPIO_23_DRV_POS))
#define GLB_REG_GPIO_23_PU                                      GLB_REG_GPIO_23_PU
#define GLB_REG_GPIO_23_PU_POS                                  (20U)
#define GLB_REG_GPIO_23_PU_LEN                                  (1U)
#define GLB_REG_GPIO_23_PU_MSK                                  (((1U<<GLB_REG_GPIO_23_PU_LEN)-1)<<GLB_REG_GPIO_23_PU_POS)
#define GLB_REG_GPIO_23_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_PU_LEN)-1)<<GLB_REG_GPIO_23_PU_POS))
#define GLB_REG_GPIO_23_PD                                      GLB_REG_GPIO_23_PD
#define GLB_REG_GPIO_23_PD_POS                                  (21U)
#define GLB_REG_GPIO_23_PD_LEN                                  (1U)
#define GLB_REG_GPIO_23_PD_MSK                                  (((1U<<GLB_REG_GPIO_23_PD_LEN)-1)<<GLB_REG_GPIO_23_PD_POS)
#define GLB_REG_GPIO_23_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_PD_LEN)-1)<<GLB_REG_GPIO_23_PD_POS))
#define GLB_REG_GPIO_23_FUNC_SEL                                GLB_REG_GPIO_23_FUNC_SEL
#define GLB_REG_GPIO_23_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_23_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_23_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_23_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_23_FUNC_SEL_POS)
#define GLB_REG_GPIO_23_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_23_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_23_FUNC_SEL_POS))
#define GLB_REG_GPIO_23_MODE                                    GLB_REG_GPIO_23_MODE
#define GLB_REG_GPIO_23_MODE_POS                                (31U)
#define GLB_REG_GPIO_23_MODE_LEN                                (1U)
#define GLB_REG_GPIO_23_MODE_MSK                                (((1U<<GLB_REG_GPIO_23_MODE_LEN)-1)<<GLB_REG_GPIO_23_MODE_POS)
#define GLB_REG_GPIO_23_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_23_MODE_LEN)-1)<<GLB_REG_GPIO_23_MODE_POS))

/* 0x130 : GPIO_CFGCTL12 */
#define GLB_GPIO_CFGCTL12_OFFSET                                (0x130)
#define GLB_REG_GPIO_24_IE                                      GLB_REG_GPIO_24_IE
#define GLB_REG_GPIO_24_IE_POS                                  (0U)
#define GLB_REG_GPIO_24_IE_LEN                                  (1U)
#define GLB_REG_GPIO_24_IE_MSK                                  (((1U<<GLB_REG_GPIO_24_IE_LEN)-1)<<GLB_REG_GPIO_24_IE_POS)
#define GLB_REG_GPIO_24_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_IE_LEN)-1)<<GLB_REG_GPIO_24_IE_POS))
#define GLB_REG_GPIO_24_SMT                                     GLB_REG_GPIO_24_SMT
#define GLB_REG_GPIO_24_SMT_POS                                 (1U)
#define GLB_REG_GPIO_24_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_24_SMT_MSK                                 (((1U<<GLB_REG_GPIO_24_SMT_LEN)-1)<<GLB_REG_GPIO_24_SMT_POS)
#define GLB_REG_GPIO_24_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_24_SMT_LEN)-1)<<GLB_REG_GPIO_24_SMT_POS))
#define GLB_REG_GPIO_24_DRV                                     GLB_REG_GPIO_24_DRV
#define GLB_REG_GPIO_24_DRV_POS                                 (2U)
#define GLB_REG_GPIO_24_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_24_DRV_MSK                                 (((1U<<GLB_REG_GPIO_24_DRV_LEN)-1)<<GLB_REG_GPIO_24_DRV_POS)
#define GLB_REG_GPIO_24_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_24_DRV_LEN)-1)<<GLB_REG_GPIO_24_DRV_POS))
#define GLB_REG_GPIO_24_PU                                      GLB_REG_GPIO_24_PU
#define GLB_REG_GPIO_24_PU_POS                                  (4U)
#define GLB_REG_GPIO_24_PU_LEN                                  (1U)
#define GLB_REG_GPIO_24_PU_MSK                                  (((1U<<GLB_REG_GPIO_24_PU_LEN)-1)<<GLB_REG_GPIO_24_PU_POS)
#define GLB_REG_GPIO_24_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_PU_LEN)-1)<<GLB_REG_GPIO_24_PU_POS))
#define GLB_REG_GPIO_24_PD                                      GLB_REG_GPIO_24_PD
#define GLB_REG_GPIO_24_PD_POS                                  (5U)
#define GLB_REG_GPIO_24_PD_LEN                                  (1U)
#define GLB_REG_GPIO_24_PD_MSK                                  (((1U<<GLB_REG_GPIO_24_PD_LEN)-1)<<GLB_REG_GPIO_24_PD_POS)
#define GLB_REG_GPIO_24_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_PD_LEN)-1)<<GLB_REG_GPIO_24_PD_POS))
#define GLB_REG_GPIO_24_FUNC_SEL                                GLB_REG_GPIO_24_FUNC_SEL
#define GLB_REG_GPIO_24_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_24_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_24_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_24_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_24_FUNC_SEL_POS)
#define GLB_REG_GPIO_24_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_24_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_24_FUNC_SEL_POS))
#define GLB_REG_GPIO_24_MODE                                    GLB_REG_GPIO_24_MODE
#define GLB_REG_GPIO_24_MODE_POS                                (15U)
#define GLB_REG_GPIO_24_MODE_LEN                                (1U)
#define GLB_REG_GPIO_24_MODE_MSK                                (((1U<<GLB_REG_GPIO_24_MODE_LEN)-1)<<GLB_REG_GPIO_24_MODE_POS)
#define GLB_REG_GPIO_24_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_24_MODE_LEN)-1)<<GLB_REG_GPIO_24_MODE_POS))
#define GLB_REG_GPIO_25_IE                                      GLB_REG_GPIO_25_IE
#define GLB_REG_GPIO_25_IE_POS                                  (16U)
#define GLB_REG_GPIO_25_IE_LEN                                  (1U)
#define GLB_REG_GPIO_25_IE_MSK                                  (((1U<<GLB_REG_GPIO_25_IE_LEN)-1)<<GLB_REG_GPIO_25_IE_POS)
#define GLB_REG_GPIO_25_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_IE_LEN)-1)<<GLB_REG_GPIO_25_IE_POS))
#define GLB_REG_GPIO_25_SMT                                     GLB_REG_GPIO_25_SMT
#define GLB_REG_GPIO_25_SMT_POS                                 (17U)
#define GLB_REG_GPIO_25_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_25_SMT_MSK                                 (((1U<<GLB_REG_GPIO_25_SMT_LEN)-1)<<GLB_REG_GPIO_25_SMT_POS)
#define GLB_REG_GPIO_25_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_25_SMT_LEN)-1)<<GLB_REG_GPIO_25_SMT_POS))
#define GLB_REG_GPIO_25_DRV                                     GLB_REG_GPIO_25_DRV
#define GLB_REG_GPIO_25_DRV_POS                                 (18U)
#define GLB_REG_GPIO_25_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_25_DRV_MSK                                 (((1U<<GLB_REG_GPIO_25_DRV_LEN)-1)<<GLB_REG_GPIO_25_DRV_POS)
#define GLB_REG_GPIO_25_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_25_DRV_LEN)-1)<<GLB_REG_GPIO_25_DRV_POS))
#define GLB_REG_GPIO_25_PU                                      GLB_REG_GPIO_25_PU
#define GLB_REG_GPIO_25_PU_POS                                  (20U)
#define GLB_REG_GPIO_25_PU_LEN                                  (1U)
#define GLB_REG_GPIO_25_PU_MSK                                  (((1U<<GLB_REG_GPIO_25_PU_LEN)-1)<<GLB_REG_GPIO_25_PU_POS)
#define GLB_REG_GPIO_25_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_PU_LEN)-1)<<GLB_REG_GPIO_25_PU_POS))
#define GLB_REG_GPIO_25_PD                                      GLB_REG_GPIO_25_PD
#define GLB_REG_GPIO_25_PD_POS                                  (21U)
#define GLB_REG_GPIO_25_PD_LEN                                  (1U)
#define GLB_REG_GPIO_25_PD_MSK                                  (((1U<<GLB_REG_GPIO_25_PD_LEN)-1)<<GLB_REG_GPIO_25_PD_POS)
#define GLB_REG_GPIO_25_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_PD_LEN)-1)<<GLB_REG_GPIO_25_PD_POS))
#define GLB_REG_GPIO_25_FUNC_SEL                                GLB_REG_GPIO_25_FUNC_SEL
#define GLB_REG_GPIO_25_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_25_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_25_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_25_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_25_FUNC_SEL_POS)
#define GLB_REG_GPIO_25_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_25_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_25_FUNC_SEL_POS))
#define GLB_REG_GPIO_25_MODE                                    GLB_REG_GPIO_25_MODE
#define GLB_REG_GPIO_25_MODE_POS                                (31U)
#define GLB_REG_GPIO_25_MODE_LEN                                (1U)
#define GLB_REG_GPIO_25_MODE_MSK                                (((1U<<GLB_REG_GPIO_25_MODE_LEN)-1)<<GLB_REG_GPIO_25_MODE_POS)
#define GLB_REG_GPIO_25_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_25_MODE_LEN)-1)<<GLB_REG_GPIO_25_MODE_POS))

/* 0x134 : GPIO_CFGCTL13 */
#define GLB_GPIO_CFGCTL13_OFFSET                                (0x134)
#define GLB_REG_GPIO_26_IE                                      GLB_REG_GPIO_26_IE
#define GLB_REG_GPIO_26_IE_POS                                  (0U)
#define GLB_REG_GPIO_26_IE_LEN                                  (1U)
#define GLB_REG_GPIO_26_IE_MSK                                  (((1U<<GLB_REG_GPIO_26_IE_LEN)-1)<<GLB_REG_GPIO_26_IE_POS)
#define GLB_REG_GPIO_26_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_IE_LEN)-1)<<GLB_REG_GPIO_26_IE_POS))
#define GLB_REG_GPIO_26_SMT                                     GLB_REG_GPIO_26_SMT
#define GLB_REG_GPIO_26_SMT_POS                                 (1U)
#define GLB_REG_GPIO_26_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_26_SMT_MSK                                 (((1U<<GLB_REG_GPIO_26_SMT_LEN)-1)<<GLB_REG_GPIO_26_SMT_POS)
#define GLB_REG_GPIO_26_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_26_SMT_LEN)-1)<<GLB_REG_GPIO_26_SMT_POS))
#define GLB_REG_GPIO_26_DRV                                     GLB_REG_GPIO_26_DRV
#define GLB_REG_GPIO_26_DRV_POS                                 (2U)
#define GLB_REG_GPIO_26_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_26_DRV_MSK                                 (((1U<<GLB_REG_GPIO_26_DRV_LEN)-1)<<GLB_REG_GPIO_26_DRV_POS)
#define GLB_REG_GPIO_26_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_26_DRV_LEN)-1)<<GLB_REG_GPIO_26_DRV_POS))
#define GLB_REG_GPIO_26_PU                                      GLB_REG_GPIO_26_PU
#define GLB_REG_GPIO_26_PU_POS                                  (4U)
#define GLB_REG_GPIO_26_PU_LEN                                  (1U)
#define GLB_REG_GPIO_26_PU_MSK                                  (((1U<<GLB_REG_GPIO_26_PU_LEN)-1)<<GLB_REG_GPIO_26_PU_POS)
#define GLB_REG_GPIO_26_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_PU_LEN)-1)<<GLB_REG_GPIO_26_PU_POS))
#define GLB_REG_GPIO_26_PD                                      GLB_REG_GPIO_26_PD
#define GLB_REG_GPIO_26_PD_POS                                  (5U)
#define GLB_REG_GPIO_26_PD_LEN                                  (1U)
#define GLB_REG_GPIO_26_PD_MSK                                  (((1U<<GLB_REG_GPIO_26_PD_LEN)-1)<<GLB_REG_GPIO_26_PD_POS)
#define GLB_REG_GPIO_26_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_PD_LEN)-1)<<GLB_REG_GPIO_26_PD_POS))
#define GLB_REG_GPIO_26_FUNC_SEL                                GLB_REG_GPIO_26_FUNC_SEL
#define GLB_REG_GPIO_26_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_26_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_26_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_26_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_26_FUNC_SEL_POS)
#define GLB_REG_GPIO_26_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_26_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_26_FUNC_SEL_POS))
#define GLB_REG_GPIO_26_MODE                                    GLB_REG_GPIO_26_MODE
#define GLB_REG_GPIO_26_MODE_POS                                (15U)
#define GLB_REG_GPIO_26_MODE_LEN                                (1U)
#define GLB_REG_GPIO_26_MODE_MSK                                (((1U<<GLB_REG_GPIO_26_MODE_LEN)-1)<<GLB_REG_GPIO_26_MODE_POS)
#define GLB_REG_GPIO_26_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_26_MODE_LEN)-1)<<GLB_REG_GPIO_26_MODE_POS))
#define GLB_REG_GPIO_27_IE                                      GLB_REG_GPIO_27_IE
#define GLB_REG_GPIO_27_IE_POS                                  (16U)
#define GLB_REG_GPIO_27_IE_LEN                                  (1U)
#define GLB_REG_GPIO_27_IE_MSK                                  (((1U<<GLB_REG_GPIO_27_IE_LEN)-1)<<GLB_REG_GPIO_27_IE_POS)
#define GLB_REG_GPIO_27_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_IE_LEN)-1)<<GLB_REG_GPIO_27_IE_POS))
#define GLB_REG_GPIO_27_SMT                                     GLB_REG_GPIO_27_SMT
#define GLB_REG_GPIO_27_SMT_POS                                 (17U)
#define GLB_REG_GPIO_27_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_27_SMT_MSK                                 (((1U<<GLB_REG_GPIO_27_SMT_LEN)-1)<<GLB_REG_GPIO_27_SMT_POS)
#define GLB_REG_GPIO_27_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_27_SMT_LEN)-1)<<GLB_REG_GPIO_27_SMT_POS))
#define GLB_REG_GPIO_27_DRV                                     GLB_REG_GPIO_27_DRV
#define GLB_REG_GPIO_27_DRV_POS                                 (18U)
#define GLB_REG_GPIO_27_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_27_DRV_MSK                                 (((1U<<GLB_REG_GPIO_27_DRV_LEN)-1)<<GLB_REG_GPIO_27_DRV_POS)
#define GLB_REG_GPIO_27_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_27_DRV_LEN)-1)<<GLB_REG_GPIO_27_DRV_POS))
#define GLB_REG_GPIO_27_PU                                      GLB_REG_GPIO_27_PU
#define GLB_REG_GPIO_27_PU_POS                                  (20U)
#define GLB_REG_GPIO_27_PU_LEN                                  (1U)
#define GLB_REG_GPIO_27_PU_MSK                                  (((1U<<GLB_REG_GPIO_27_PU_LEN)-1)<<GLB_REG_GPIO_27_PU_POS)
#define GLB_REG_GPIO_27_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_PU_LEN)-1)<<GLB_REG_GPIO_27_PU_POS))
#define GLB_REG_GPIO_27_PD                                      GLB_REG_GPIO_27_PD
#define GLB_REG_GPIO_27_PD_POS                                  (21U)
#define GLB_REG_GPIO_27_PD_LEN                                  (1U)
#define GLB_REG_GPIO_27_PD_MSK                                  (((1U<<GLB_REG_GPIO_27_PD_LEN)-1)<<GLB_REG_GPIO_27_PD_POS)
#define GLB_REG_GPIO_27_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_PD_LEN)-1)<<GLB_REG_GPIO_27_PD_POS))
#define GLB_REG_GPIO_27_FUNC_SEL                                GLB_REG_GPIO_27_FUNC_SEL
#define GLB_REG_GPIO_27_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_27_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_27_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_27_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_27_FUNC_SEL_POS)
#define GLB_REG_GPIO_27_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_27_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_27_FUNC_SEL_POS))
#define GLB_REG_GPIO_27_MODE                                    GLB_REG_GPIO_27_MODE
#define GLB_REG_GPIO_27_MODE_POS                                (31U)
#define GLB_REG_GPIO_27_MODE_LEN                                (1U)
#define GLB_REG_GPIO_27_MODE_MSK                                (((1U<<GLB_REG_GPIO_27_MODE_LEN)-1)<<GLB_REG_GPIO_27_MODE_POS)
#define GLB_REG_GPIO_27_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_27_MODE_LEN)-1)<<GLB_REG_GPIO_27_MODE_POS))

/* 0x138 : GPIO_CFGCTL14 */
#define GLB_GPIO_CFGCTL14_OFFSET                                (0x138)
#define GLB_REG_GPIO_28_IE                                      GLB_REG_GPIO_28_IE
#define GLB_REG_GPIO_28_IE_POS                                  (0U)
#define GLB_REG_GPIO_28_IE_LEN                                  (1U)
#define GLB_REG_GPIO_28_IE_MSK                                  (((1U<<GLB_REG_GPIO_28_IE_LEN)-1)<<GLB_REG_GPIO_28_IE_POS)
#define GLB_REG_GPIO_28_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_IE_LEN)-1)<<GLB_REG_GPIO_28_IE_POS))
#define GLB_REG_GPIO_28_SMT                                     GLB_REG_GPIO_28_SMT
#define GLB_REG_GPIO_28_SMT_POS                                 (1U)
#define GLB_REG_GPIO_28_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_28_SMT_MSK                                 (((1U<<GLB_REG_GPIO_28_SMT_LEN)-1)<<GLB_REG_GPIO_28_SMT_POS)
#define GLB_REG_GPIO_28_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_28_SMT_LEN)-1)<<GLB_REG_GPIO_28_SMT_POS))
#define GLB_REG_GPIO_28_DRV                                     GLB_REG_GPIO_28_DRV
#define GLB_REG_GPIO_28_DRV_POS                                 (2U)
#define GLB_REG_GPIO_28_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_28_DRV_MSK                                 (((1U<<GLB_REG_GPIO_28_DRV_LEN)-1)<<GLB_REG_GPIO_28_DRV_POS)
#define GLB_REG_GPIO_28_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_28_DRV_LEN)-1)<<GLB_REG_GPIO_28_DRV_POS))
#define GLB_REG_GPIO_28_PU                                      GLB_REG_GPIO_28_PU
#define GLB_REG_GPIO_28_PU_POS                                  (4U)
#define GLB_REG_GPIO_28_PU_LEN                                  (1U)
#define GLB_REG_GPIO_28_PU_MSK                                  (((1U<<GLB_REG_GPIO_28_PU_LEN)-1)<<GLB_REG_GPIO_28_PU_POS)
#define GLB_REG_GPIO_28_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_PU_LEN)-1)<<GLB_REG_GPIO_28_PU_POS))
#define GLB_REG_GPIO_28_PD                                      GLB_REG_GPIO_28_PD
#define GLB_REG_GPIO_28_PD_POS                                  (5U)
#define GLB_REG_GPIO_28_PD_LEN                                  (1U)
#define GLB_REG_GPIO_28_PD_MSK                                  (((1U<<GLB_REG_GPIO_28_PD_LEN)-1)<<GLB_REG_GPIO_28_PD_POS)
#define GLB_REG_GPIO_28_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_PD_LEN)-1)<<GLB_REG_GPIO_28_PD_POS))
#define GLB_REG_GPIO_28_FUNC_SEL                                GLB_REG_GPIO_28_FUNC_SEL
#define GLB_REG_GPIO_28_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_28_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_28_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_28_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_28_FUNC_SEL_POS)
#define GLB_REG_GPIO_28_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_28_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_28_FUNC_SEL_POS))
#define GLB_REG_GPIO_28_MODE                                    GLB_REG_GPIO_28_MODE
#define GLB_REG_GPIO_28_MODE_POS                                (15U)
#define GLB_REG_GPIO_28_MODE_LEN                                (1U)
#define GLB_REG_GPIO_28_MODE_MSK                                (((1U<<GLB_REG_GPIO_28_MODE_LEN)-1)<<GLB_REG_GPIO_28_MODE_POS)
#define GLB_REG_GPIO_28_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_28_MODE_LEN)-1)<<GLB_REG_GPIO_28_MODE_POS))
#define GLB_REG_GPIO_29_IE                                      GLB_REG_GPIO_29_IE
#define GLB_REG_GPIO_29_IE_POS                                  (16U)
#define GLB_REG_GPIO_29_IE_LEN                                  (1U)
#define GLB_REG_GPIO_29_IE_MSK                                  (((1U<<GLB_REG_GPIO_29_IE_LEN)-1)<<GLB_REG_GPIO_29_IE_POS)
#define GLB_REG_GPIO_29_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_IE_LEN)-1)<<GLB_REG_GPIO_29_IE_POS))
#define GLB_REG_GPIO_29_SMT                                     GLB_REG_GPIO_29_SMT
#define GLB_REG_GPIO_29_SMT_POS                                 (17U)
#define GLB_REG_GPIO_29_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_29_SMT_MSK                                 (((1U<<GLB_REG_GPIO_29_SMT_LEN)-1)<<GLB_REG_GPIO_29_SMT_POS)
#define GLB_REG_GPIO_29_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_29_SMT_LEN)-1)<<GLB_REG_GPIO_29_SMT_POS))
#define GLB_REG_GPIO_29_DRV                                     GLB_REG_GPIO_29_DRV
#define GLB_REG_GPIO_29_DRV_POS                                 (18U)
#define GLB_REG_GPIO_29_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_29_DRV_MSK                                 (((1U<<GLB_REG_GPIO_29_DRV_LEN)-1)<<GLB_REG_GPIO_29_DRV_POS)
#define GLB_REG_GPIO_29_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_29_DRV_LEN)-1)<<GLB_REG_GPIO_29_DRV_POS))
#define GLB_REG_GPIO_29_PU                                      GLB_REG_GPIO_29_PU
#define GLB_REG_GPIO_29_PU_POS                                  (20U)
#define GLB_REG_GPIO_29_PU_LEN                                  (1U)
#define GLB_REG_GPIO_29_PU_MSK                                  (((1U<<GLB_REG_GPIO_29_PU_LEN)-1)<<GLB_REG_GPIO_29_PU_POS)
#define GLB_REG_GPIO_29_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_PU_LEN)-1)<<GLB_REG_GPIO_29_PU_POS))
#define GLB_REG_GPIO_29_PD                                      GLB_REG_GPIO_29_PD
#define GLB_REG_GPIO_29_PD_POS                                  (21U)
#define GLB_REG_GPIO_29_PD_LEN                                  (1U)
#define GLB_REG_GPIO_29_PD_MSK                                  (((1U<<GLB_REG_GPIO_29_PD_LEN)-1)<<GLB_REG_GPIO_29_PD_POS)
#define GLB_REG_GPIO_29_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_PD_LEN)-1)<<GLB_REG_GPIO_29_PD_POS))
#define GLB_REG_GPIO_29_FUNC_SEL                                GLB_REG_GPIO_29_FUNC_SEL
#define GLB_REG_GPIO_29_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_29_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_29_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_29_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_29_FUNC_SEL_POS)
#define GLB_REG_GPIO_29_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_29_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_29_FUNC_SEL_POS))
#define GLB_REG_GPIO_29_MODE                                    GLB_REG_GPIO_29_MODE
#define GLB_REG_GPIO_29_MODE_POS                                (31U)
#define GLB_REG_GPIO_29_MODE_LEN                                (1U)
#define GLB_REG_GPIO_29_MODE_MSK                                (((1U<<GLB_REG_GPIO_29_MODE_LEN)-1)<<GLB_REG_GPIO_29_MODE_POS)
#define GLB_REG_GPIO_29_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_29_MODE_LEN)-1)<<GLB_REG_GPIO_29_MODE_POS))

/* 0x13C : GPIO_CFGCTL15 */
#define GLB_GPIO_CFGCTL15_OFFSET                                (0x13C)
#define GLB_REG_GPIO_30_IE                                      GLB_REG_GPIO_30_IE
#define GLB_REG_GPIO_30_IE_POS                                  (0U)
#define GLB_REG_GPIO_30_IE_LEN                                  (1U)
#define GLB_REG_GPIO_30_IE_MSK                                  (((1U<<GLB_REG_GPIO_30_IE_LEN)-1)<<GLB_REG_GPIO_30_IE_POS)
#define GLB_REG_GPIO_30_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_IE_LEN)-1)<<GLB_REG_GPIO_30_IE_POS))
#define GLB_REG_GPIO_30_SMT                                     GLB_REG_GPIO_30_SMT
#define GLB_REG_GPIO_30_SMT_POS                                 (1U)
#define GLB_REG_GPIO_30_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_30_SMT_MSK                                 (((1U<<GLB_REG_GPIO_30_SMT_LEN)-1)<<GLB_REG_GPIO_30_SMT_POS)
#define GLB_REG_GPIO_30_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_30_SMT_LEN)-1)<<GLB_REG_GPIO_30_SMT_POS))
#define GLB_REG_GPIO_30_DRV                                     GLB_REG_GPIO_30_DRV
#define GLB_REG_GPIO_30_DRV_POS                                 (2U)
#define GLB_REG_GPIO_30_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_30_DRV_MSK                                 (((1U<<GLB_REG_GPIO_30_DRV_LEN)-1)<<GLB_REG_GPIO_30_DRV_POS)
#define GLB_REG_GPIO_30_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_30_DRV_LEN)-1)<<GLB_REG_GPIO_30_DRV_POS))
#define GLB_REG_GPIO_30_PU                                      GLB_REG_GPIO_30_PU
#define GLB_REG_GPIO_30_PU_POS                                  (4U)
#define GLB_REG_GPIO_30_PU_LEN                                  (1U)
#define GLB_REG_GPIO_30_PU_MSK                                  (((1U<<GLB_REG_GPIO_30_PU_LEN)-1)<<GLB_REG_GPIO_30_PU_POS)
#define GLB_REG_GPIO_30_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_PU_LEN)-1)<<GLB_REG_GPIO_30_PU_POS))
#define GLB_REG_GPIO_30_PD                                      GLB_REG_GPIO_30_PD
#define GLB_REG_GPIO_30_PD_POS                                  (5U)
#define GLB_REG_GPIO_30_PD_LEN                                  (1U)
#define GLB_REG_GPIO_30_PD_MSK                                  (((1U<<GLB_REG_GPIO_30_PD_LEN)-1)<<GLB_REG_GPIO_30_PD_POS)
#define GLB_REG_GPIO_30_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_PD_LEN)-1)<<GLB_REG_GPIO_30_PD_POS))
#define GLB_REG_GPIO_30_FUNC_SEL                                GLB_REG_GPIO_30_FUNC_SEL
#define GLB_REG_GPIO_30_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_30_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_30_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_30_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_30_FUNC_SEL_POS)
#define GLB_REG_GPIO_30_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_30_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_30_FUNC_SEL_POS))
#define GLB_REG_GPIO_30_MODE                                    GLB_REG_GPIO_30_MODE
#define GLB_REG_GPIO_30_MODE_POS                                (15U)
#define GLB_REG_GPIO_30_MODE_LEN                                (1U)
#define GLB_REG_GPIO_30_MODE_MSK                                (((1U<<GLB_REG_GPIO_30_MODE_LEN)-1)<<GLB_REG_GPIO_30_MODE_POS)
#define GLB_REG_GPIO_30_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_30_MODE_LEN)-1)<<GLB_REG_GPIO_30_MODE_POS))
#define GLB_REG_GPIO_31_IE                                      GLB_REG_GPIO_31_IE
#define GLB_REG_GPIO_31_IE_POS                                  (16U)
#define GLB_REG_GPIO_31_IE_LEN                                  (1U)
#define GLB_REG_GPIO_31_IE_MSK                                  (((1U<<GLB_REG_GPIO_31_IE_LEN)-1)<<GLB_REG_GPIO_31_IE_POS)
#define GLB_REG_GPIO_31_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_IE_LEN)-1)<<GLB_REG_GPIO_31_IE_POS))
#define GLB_REG_GPIO_31_SMT                                     GLB_REG_GPIO_31_SMT
#define GLB_REG_GPIO_31_SMT_POS                                 (17U)
#define GLB_REG_GPIO_31_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_31_SMT_MSK                                 (((1U<<GLB_REG_GPIO_31_SMT_LEN)-1)<<GLB_REG_GPIO_31_SMT_POS)
#define GLB_REG_GPIO_31_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_31_SMT_LEN)-1)<<GLB_REG_GPIO_31_SMT_POS))
#define GLB_REG_GPIO_31_DRV                                     GLB_REG_GPIO_31_DRV
#define GLB_REG_GPIO_31_DRV_POS                                 (18U)
#define GLB_REG_GPIO_31_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_31_DRV_MSK                                 (((1U<<GLB_REG_GPIO_31_DRV_LEN)-1)<<GLB_REG_GPIO_31_DRV_POS)
#define GLB_REG_GPIO_31_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_31_DRV_LEN)-1)<<GLB_REG_GPIO_31_DRV_POS))
#define GLB_REG_GPIO_31_PU                                      GLB_REG_GPIO_31_PU
#define GLB_REG_GPIO_31_PU_POS                                  (20U)
#define GLB_REG_GPIO_31_PU_LEN                                  (1U)
#define GLB_REG_GPIO_31_PU_MSK                                  (((1U<<GLB_REG_GPIO_31_PU_LEN)-1)<<GLB_REG_GPIO_31_PU_POS)
#define GLB_REG_GPIO_31_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_PU_LEN)-1)<<GLB_REG_GPIO_31_PU_POS))
#define GLB_REG_GPIO_31_PD                                      GLB_REG_GPIO_31_PD
#define GLB_REG_GPIO_31_PD_POS                                  (21U)
#define GLB_REG_GPIO_31_PD_LEN                                  (1U)
#define GLB_REG_GPIO_31_PD_MSK                                  (((1U<<GLB_REG_GPIO_31_PD_LEN)-1)<<GLB_REG_GPIO_31_PD_POS)
#define GLB_REG_GPIO_31_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_PD_LEN)-1)<<GLB_REG_GPIO_31_PD_POS))
#define GLB_REG_GPIO_31_FUNC_SEL                                GLB_REG_GPIO_31_FUNC_SEL
#define GLB_REG_GPIO_31_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_31_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_31_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_31_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_31_FUNC_SEL_POS)
#define GLB_REG_GPIO_31_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_31_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_31_FUNC_SEL_POS))
#define GLB_REG_GPIO_31_MODE                                    GLB_REG_GPIO_31_MODE
#define GLB_REG_GPIO_31_MODE_POS                                (31U)
#define GLB_REG_GPIO_31_MODE_LEN                                (1U)
#define GLB_REG_GPIO_31_MODE_MSK                                (((1U<<GLB_REG_GPIO_31_MODE_LEN)-1)<<GLB_REG_GPIO_31_MODE_POS)
#define GLB_REG_GPIO_31_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_31_MODE_LEN)-1)<<GLB_REG_GPIO_31_MODE_POS))

/* 0x140 : GPIO_CFGCTL16 */
#define GLB_GPIO_CFGCTL16_OFFSET                                (0x140)
#define GLB_REG_GPIO_32_IE                                      GLB_REG_GPIO_32_IE
#define GLB_REG_GPIO_32_IE_POS                                  (0U)
#define GLB_REG_GPIO_32_IE_LEN                                  (1U)
#define GLB_REG_GPIO_32_IE_MSK                                  (((1U<<GLB_REG_GPIO_32_IE_LEN)-1)<<GLB_REG_GPIO_32_IE_POS)
#define GLB_REG_GPIO_32_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_IE_LEN)-1)<<GLB_REG_GPIO_32_IE_POS))
#define GLB_REG_GPIO_32_SMT                                     GLB_REG_GPIO_32_SMT
#define GLB_REG_GPIO_32_SMT_POS                                 (1U)
#define GLB_REG_GPIO_32_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_32_SMT_MSK                                 (((1U<<GLB_REG_GPIO_32_SMT_LEN)-1)<<GLB_REG_GPIO_32_SMT_POS)
#define GLB_REG_GPIO_32_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_32_SMT_LEN)-1)<<GLB_REG_GPIO_32_SMT_POS))
#define GLB_REG_GPIO_32_DRV                                     GLB_REG_GPIO_32_DRV
#define GLB_REG_GPIO_32_DRV_POS                                 (2U)
#define GLB_REG_GPIO_32_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_32_DRV_MSK                                 (((1U<<GLB_REG_GPIO_32_DRV_LEN)-1)<<GLB_REG_GPIO_32_DRV_POS)
#define GLB_REG_GPIO_32_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_32_DRV_LEN)-1)<<GLB_REG_GPIO_32_DRV_POS))
#define GLB_REG_GPIO_32_PU                                      GLB_REG_GPIO_32_PU
#define GLB_REG_GPIO_32_PU_POS                                  (4U)
#define GLB_REG_GPIO_32_PU_LEN                                  (1U)
#define GLB_REG_GPIO_32_PU_MSK                                  (((1U<<GLB_REG_GPIO_32_PU_LEN)-1)<<GLB_REG_GPIO_32_PU_POS)
#define GLB_REG_GPIO_32_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_PU_LEN)-1)<<GLB_REG_GPIO_32_PU_POS))
#define GLB_REG_GPIO_32_PD                                      GLB_REG_GPIO_32_PD
#define GLB_REG_GPIO_32_PD_POS                                  (5U)
#define GLB_REG_GPIO_32_PD_LEN                                  (1U)
#define GLB_REG_GPIO_32_PD_MSK                                  (((1U<<GLB_REG_GPIO_32_PD_LEN)-1)<<GLB_REG_GPIO_32_PD_POS)
#define GLB_REG_GPIO_32_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_PD_LEN)-1)<<GLB_REG_GPIO_32_PD_POS))
#define GLB_REG_GPIO_32_FUNC_SEL                                GLB_REG_GPIO_32_FUNC_SEL
#define GLB_REG_GPIO_32_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_32_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_32_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_32_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_32_FUNC_SEL_POS)
#define GLB_REG_GPIO_32_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_32_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_32_FUNC_SEL_POS))
#define GLB_REG_GPIO_32_MODE                                    GLB_REG_GPIO_32_MODE
#define GLB_REG_GPIO_32_MODE_POS                                (15U)
#define GLB_REG_GPIO_32_MODE_LEN                                (1U)
#define GLB_REG_GPIO_32_MODE_MSK                                (((1U<<GLB_REG_GPIO_32_MODE_LEN)-1)<<GLB_REG_GPIO_32_MODE_POS)
#define GLB_REG_GPIO_32_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_32_MODE_LEN)-1)<<GLB_REG_GPIO_32_MODE_POS))
#define GLB_REG_GPIO_33_IE                                      GLB_REG_GPIO_33_IE
#define GLB_REG_GPIO_33_IE_POS                                  (16U)
#define GLB_REG_GPIO_33_IE_LEN                                  (1U)
#define GLB_REG_GPIO_33_IE_MSK                                  (((1U<<GLB_REG_GPIO_33_IE_LEN)-1)<<GLB_REG_GPIO_33_IE_POS)
#define GLB_REG_GPIO_33_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_IE_LEN)-1)<<GLB_REG_GPIO_33_IE_POS))
#define GLB_REG_GPIO_33_SMT                                     GLB_REG_GPIO_33_SMT
#define GLB_REG_GPIO_33_SMT_POS                                 (17U)
#define GLB_REG_GPIO_33_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_33_SMT_MSK                                 (((1U<<GLB_REG_GPIO_33_SMT_LEN)-1)<<GLB_REG_GPIO_33_SMT_POS)
#define GLB_REG_GPIO_33_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_33_SMT_LEN)-1)<<GLB_REG_GPIO_33_SMT_POS))
#define GLB_REG_GPIO_33_DRV                                     GLB_REG_GPIO_33_DRV
#define GLB_REG_GPIO_33_DRV_POS                                 (18U)
#define GLB_REG_GPIO_33_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_33_DRV_MSK                                 (((1U<<GLB_REG_GPIO_33_DRV_LEN)-1)<<GLB_REG_GPIO_33_DRV_POS)
#define GLB_REG_GPIO_33_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_33_DRV_LEN)-1)<<GLB_REG_GPIO_33_DRV_POS))
#define GLB_REG_GPIO_33_PU                                      GLB_REG_GPIO_33_PU
#define GLB_REG_GPIO_33_PU_POS                                  (20U)
#define GLB_REG_GPIO_33_PU_LEN                                  (1U)
#define GLB_REG_GPIO_33_PU_MSK                                  (((1U<<GLB_REG_GPIO_33_PU_LEN)-1)<<GLB_REG_GPIO_33_PU_POS)
#define GLB_REG_GPIO_33_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_PU_LEN)-1)<<GLB_REG_GPIO_33_PU_POS))
#define GLB_REG_GPIO_33_PD                                      GLB_REG_GPIO_33_PD
#define GLB_REG_GPIO_33_PD_POS                                  (21U)
#define GLB_REG_GPIO_33_PD_LEN                                  (1U)
#define GLB_REG_GPIO_33_PD_MSK                                  (((1U<<GLB_REG_GPIO_33_PD_LEN)-1)<<GLB_REG_GPIO_33_PD_POS)
#define GLB_REG_GPIO_33_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_PD_LEN)-1)<<GLB_REG_GPIO_33_PD_POS))
#define GLB_REG_GPIO_33_FUNC_SEL                                GLB_REG_GPIO_33_FUNC_SEL
#define GLB_REG_GPIO_33_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_33_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_33_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_33_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_33_FUNC_SEL_POS)
#define GLB_REG_GPIO_33_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_33_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_33_FUNC_SEL_POS))
#define GLB_REG_GPIO_33_MODE                                    GLB_REG_GPIO_33_MODE
#define GLB_REG_GPIO_33_MODE_POS                                (31U)
#define GLB_REG_GPIO_33_MODE_LEN                                (1U)
#define GLB_REG_GPIO_33_MODE_MSK                                (((1U<<GLB_REG_GPIO_33_MODE_LEN)-1)<<GLB_REG_GPIO_33_MODE_POS)
#define GLB_REG_GPIO_33_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_33_MODE_LEN)-1)<<GLB_REG_GPIO_33_MODE_POS))

/* 0x144 : GPIO_CFGCTL17 */
#define GLB_GPIO_CFGCTL17_OFFSET                                (0x144)
#define GLB_REG_GPIO_34_IE                                      GLB_REG_GPIO_34_IE
#define GLB_REG_GPIO_34_IE_POS                                  (0U)
#define GLB_REG_GPIO_34_IE_LEN                                  (1U)
#define GLB_REG_GPIO_34_IE_MSK                                  (((1U<<GLB_REG_GPIO_34_IE_LEN)-1)<<GLB_REG_GPIO_34_IE_POS)
#define GLB_REG_GPIO_34_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_IE_LEN)-1)<<GLB_REG_GPIO_34_IE_POS))
#define GLB_REG_GPIO_34_SMT                                     GLB_REG_GPIO_34_SMT
#define GLB_REG_GPIO_34_SMT_POS                                 (1U)
#define GLB_REG_GPIO_34_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_34_SMT_MSK                                 (((1U<<GLB_REG_GPIO_34_SMT_LEN)-1)<<GLB_REG_GPIO_34_SMT_POS)
#define GLB_REG_GPIO_34_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_34_SMT_LEN)-1)<<GLB_REG_GPIO_34_SMT_POS))
#define GLB_REG_GPIO_34_DRV                                     GLB_REG_GPIO_34_DRV
#define GLB_REG_GPIO_34_DRV_POS                                 (2U)
#define GLB_REG_GPIO_34_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_34_DRV_MSK                                 (((1U<<GLB_REG_GPIO_34_DRV_LEN)-1)<<GLB_REG_GPIO_34_DRV_POS)
#define GLB_REG_GPIO_34_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_34_DRV_LEN)-1)<<GLB_REG_GPIO_34_DRV_POS))
#define GLB_REG_GPIO_34_PU                                      GLB_REG_GPIO_34_PU
#define GLB_REG_GPIO_34_PU_POS                                  (4U)
#define GLB_REG_GPIO_34_PU_LEN                                  (1U)
#define GLB_REG_GPIO_34_PU_MSK                                  (((1U<<GLB_REG_GPIO_34_PU_LEN)-1)<<GLB_REG_GPIO_34_PU_POS)
#define GLB_REG_GPIO_34_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_PU_LEN)-1)<<GLB_REG_GPIO_34_PU_POS))
#define GLB_REG_GPIO_34_PD                                      GLB_REG_GPIO_34_PD
#define GLB_REG_GPIO_34_PD_POS                                  (5U)
#define GLB_REG_GPIO_34_PD_LEN                                  (1U)
#define GLB_REG_GPIO_34_PD_MSK                                  (((1U<<GLB_REG_GPIO_34_PD_LEN)-1)<<GLB_REG_GPIO_34_PD_POS)
#define GLB_REG_GPIO_34_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_PD_LEN)-1)<<GLB_REG_GPIO_34_PD_POS))
#define GLB_REG_GPIO_34_FUNC_SEL                                GLB_REG_GPIO_34_FUNC_SEL
#define GLB_REG_GPIO_34_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_34_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_34_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_34_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_34_FUNC_SEL_POS)
#define GLB_REG_GPIO_34_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_34_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_34_FUNC_SEL_POS))
#define GLB_REG_GPIO_34_MODE                                    GLB_REG_GPIO_34_MODE
#define GLB_REG_GPIO_34_MODE_POS                                (15U)
#define GLB_REG_GPIO_34_MODE_LEN                                (1U)
#define GLB_REG_GPIO_34_MODE_MSK                                (((1U<<GLB_REG_GPIO_34_MODE_LEN)-1)<<GLB_REG_GPIO_34_MODE_POS)
#define GLB_REG_GPIO_34_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_34_MODE_LEN)-1)<<GLB_REG_GPIO_34_MODE_POS))
#define GLB_REG_GPIO_35_IE                                      GLB_REG_GPIO_35_IE
#define GLB_REG_GPIO_35_IE_POS                                  (16U)
#define GLB_REG_GPIO_35_IE_LEN                                  (1U)
#define GLB_REG_GPIO_35_IE_MSK                                  (((1U<<GLB_REG_GPIO_35_IE_LEN)-1)<<GLB_REG_GPIO_35_IE_POS)
#define GLB_REG_GPIO_35_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_IE_LEN)-1)<<GLB_REG_GPIO_35_IE_POS))
#define GLB_REG_GPIO_35_SMT                                     GLB_REG_GPIO_35_SMT
#define GLB_REG_GPIO_35_SMT_POS                                 (17U)
#define GLB_REG_GPIO_35_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_35_SMT_MSK                                 (((1U<<GLB_REG_GPIO_35_SMT_LEN)-1)<<GLB_REG_GPIO_35_SMT_POS)
#define GLB_REG_GPIO_35_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_35_SMT_LEN)-1)<<GLB_REG_GPIO_35_SMT_POS))
#define GLB_REG_GPIO_35_DRV                                     GLB_REG_GPIO_35_DRV
#define GLB_REG_GPIO_35_DRV_POS                                 (18U)
#define GLB_REG_GPIO_35_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_35_DRV_MSK                                 (((1U<<GLB_REG_GPIO_35_DRV_LEN)-1)<<GLB_REG_GPIO_35_DRV_POS)
#define GLB_REG_GPIO_35_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_35_DRV_LEN)-1)<<GLB_REG_GPIO_35_DRV_POS))
#define GLB_REG_GPIO_35_PU                                      GLB_REG_GPIO_35_PU
#define GLB_REG_GPIO_35_PU_POS                                  (20U)
#define GLB_REG_GPIO_35_PU_LEN                                  (1U)
#define GLB_REG_GPIO_35_PU_MSK                                  (((1U<<GLB_REG_GPIO_35_PU_LEN)-1)<<GLB_REG_GPIO_35_PU_POS)
#define GLB_REG_GPIO_35_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_PU_LEN)-1)<<GLB_REG_GPIO_35_PU_POS))
#define GLB_REG_GPIO_35_PD                                      GLB_REG_GPIO_35_PD
#define GLB_REG_GPIO_35_PD_POS                                  (21U)
#define GLB_REG_GPIO_35_PD_LEN                                  (1U)
#define GLB_REG_GPIO_35_PD_MSK                                  (((1U<<GLB_REG_GPIO_35_PD_LEN)-1)<<GLB_REG_GPIO_35_PD_POS)
#define GLB_REG_GPIO_35_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_PD_LEN)-1)<<GLB_REG_GPIO_35_PD_POS))
#define GLB_REG_GPIO_35_FUNC_SEL                                GLB_REG_GPIO_35_FUNC_SEL
#define GLB_REG_GPIO_35_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_35_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_35_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_35_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_35_FUNC_SEL_POS)
#define GLB_REG_GPIO_35_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_35_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_35_FUNC_SEL_POS))
#define GLB_REG_GPIO_35_MODE                                    GLB_REG_GPIO_35_MODE
#define GLB_REG_GPIO_35_MODE_POS                                (31U)
#define GLB_REG_GPIO_35_MODE_LEN                                (1U)
#define GLB_REG_GPIO_35_MODE_MSK                                (((1U<<GLB_REG_GPIO_35_MODE_LEN)-1)<<GLB_REG_GPIO_35_MODE_POS)
#define GLB_REG_GPIO_35_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_35_MODE_LEN)-1)<<GLB_REG_GPIO_35_MODE_POS))

/* 0x148 : GPIO_CFGCTL18 */
#define GLB_GPIO_CFGCTL18_OFFSET                                (0x148)
#define GLB_REG_GPIO_36_IE                                      GLB_REG_GPIO_36_IE
#define GLB_REG_GPIO_36_IE_POS                                  (0U)
#define GLB_REG_GPIO_36_IE_LEN                                  (1U)
#define GLB_REG_GPIO_36_IE_MSK                                  (((1U<<GLB_REG_GPIO_36_IE_LEN)-1)<<GLB_REG_GPIO_36_IE_POS)
#define GLB_REG_GPIO_36_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_IE_LEN)-1)<<GLB_REG_GPIO_36_IE_POS))
#define GLB_REG_GPIO_36_SMT                                     GLB_REG_GPIO_36_SMT
#define GLB_REG_GPIO_36_SMT_POS                                 (1U)
#define GLB_REG_GPIO_36_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_36_SMT_MSK                                 (((1U<<GLB_REG_GPIO_36_SMT_LEN)-1)<<GLB_REG_GPIO_36_SMT_POS)
#define GLB_REG_GPIO_36_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_36_SMT_LEN)-1)<<GLB_REG_GPIO_36_SMT_POS))
#define GLB_REG_GPIO_36_DRV                                     GLB_REG_GPIO_36_DRV
#define GLB_REG_GPIO_36_DRV_POS                                 (2U)
#define GLB_REG_GPIO_36_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_36_DRV_MSK                                 (((1U<<GLB_REG_GPIO_36_DRV_LEN)-1)<<GLB_REG_GPIO_36_DRV_POS)
#define GLB_REG_GPIO_36_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_36_DRV_LEN)-1)<<GLB_REG_GPIO_36_DRV_POS))
#define GLB_REG_GPIO_36_PU                                      GLB_REG_GPIO_36_PU
#define GLB_REG_GPIO_36_PU_POS                                  (4U)
#define GLB_REG_GPIO_36_PU_LEN                                  (1U)
#define GLB_REG_GPIO_36_PU_MSK                                  (((1U<<GLB_REG_GPIO_36_PU_LEN)-1)<<GLB_REG_GPIO_36_PU_POS)
#define GLB_REG_GPIO_36_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_PU_LEN)-1)<<GLB_REG_GPIO_36_PU_POS))
#define GLB_REG_GPIO_36_PD                                      GLB_REG_GPIO_36_PD
#define GLB_REG_GPIO_36_PD_POS                                  (5U)
#define GLB_REG_GPIO_36_PD_LEN                                  (1U)
#define GLB_REG_GPIO_36_PD_MSK                                  (((1U<<GLB_REG_GPIO_36_PD_LEN)-1)<<GLB_REG_GPIO_36_PD_POS)
#define GLB_REG_GPIO_36_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_PD_LEN)-1)<<GLB_REG_GPIO_36_PD_POS))
#define GLB_REG_GPIO_36_FUNC_SEL                                GLB_REG_GPIO_36_FUNC_SEL
#define GLB_REG_GPIO_36_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_36_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_36_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_36_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_36_FUNC_SEL_POS)
#define GLB_REG_GPIO_36_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_36_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_36_FUNC_SEL_POS))
#define GLB_REG_GPIO_36_MODE                                    GLB_REG_GPIO_36_MODE
#define GLB_REG_GPIO_36_MODE_POS                                (15U)
#define GLB_REG_GPIO_36_MODE_LEN                                (1U)
#define GLB_REG_GPIO_36_MODE_MSK                                (((1U<<GLB_REG_GPIO_36_MODE_LEN)-1)<<GLB_REG_GPIO_36_MODE_POS)
#define GLB_REG_GPIO_36_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_36_MODE_LEN)-1)<<GLB_REG_GPIO_36_MODE_POS))
#define GLB_REG_GPIO_37_IE                                      GLB_REG_GPIO_37_IE
#define GLB_REG_GPIO_37_IE_POS                                  (16U)
#define GLB_REG_GPIO_37_IE_LEN                                  (1U)
#define GLB_REG_GPIO_37_IE_MSK                                  (((1U<<GLB_REG_GPIO_37_IE_LEN)-1)<<GLB_REG_GPIO_37_IE_POS)
#define GLB_REG_GPIO_37_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_IE_LEN)-1)<<GLB_REG_GPIO_37_IE_POS))
#define GLB_REG_GPIO_37_SMT                                     GLB_REG_GPIO_37_SMT
#define GLB_REG_GPIO_37_SMT_POS                                 (17U)
#define GLB_REG_GPIO_37_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_37_SMT_MSK                                 (((1U<<GLB_REG_GPIO_37_SMT_LEN)-1)<<GLB_REG_GPIO_37_SMT_POS)
#define GLB_REG_GPIO_37_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_37_SMT_LEN)-1)<<GLB_REG_GPIO_37_SMT_POS))
#define GLB_REG_GPIO_37_DRV                                     GLB_REG_GPIO_37_DRV
#define GLB_REG_GPIO_37_DRV_POS                                 (18U)
#define GLB_REG_GPIO_37_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_37_DRV_MSK                                 (((1U<<GLB_REG_GPIO_37_DRV_LEN)-1)<<GLB_REG_GPIO_37_DRV_POS)
#define GLB_REG_GPIO_37_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_37_DRV_LEN)-1)<<GLB_REG_GPIO_37_DRV_POS))
#define GLB_REG_GPIO_37_PU                                      GLB_REG_GPIO_37_PU
#define GLB_REG_GPIO_37_PU_POS                                  (20U)
#define GLB_REG_GPIO_37_PU_LEN                                  (1U)
#define GLB_REG_GPIO_37_PU_MSK                                  (((1U<<GLB_REG_GPIO_37_PU_LEN)-1)<<GLB_REG_GPIO_37_PU_POS)
#define GLB_REG_GPIO_37_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_PU_LEN)-1)<<GLB_REG_GPIO_37_PU_POS))
#define GLB_REG_GPIO_37_PD                                      GLB_REG_GPIO_37_PD
#define GLB_REG_GPIO_37_PD_POS                                  (21U)
#define GLB_REG_GPIO_37_PD_LEN                                  (1U)
#define GLB_REG_GPIO_37_PD_MSK                                  (((1U<<GLB_REG_GPIO_37_PD_LEN)-1)<<GLB_REG_GPIO_37_PD_POS)
#define GLB_REG_GPIO_37_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_PD_LEN)-1)<<GLB_REG_GPIO_37_PD_POS))
#define GLB_REG_GPIO_37_FUNC_SEL                                GLB_REG_GPIO_37_FUNC_SEL
#define GLB_REG_GPIO_37_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_37_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_37_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_37_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_37_FUNC_SEL_POS)
#define GLB_REG_GPIO_37_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_37_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_37_FUNC_SEL_POS))
#define GLB_REG_GPIO_37_MODE                                    GLB_REG_GPIO_37_MODE
#define GLB_REG_GPIO_37_MODE_POS                                (31U)
#define GLB_REG_GPIO_37_MODE_LEN                                (1U)
#define GLB_REG_GPIO_37_MODE_MSK                                (((1U<<GLB_REG_GPIO_37_MODE_LEN)-1)<<GLB_REG_GPIO_37_MODE_POS)
#define GLB_REG_GPIO_37_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_37_MODE_LEN)-1)<<GLB_REG_GPIO_37_MODE_POS))

/* 0x14C : GPIO_CFGCTL19 */
#define GLB_GPIO_CFGCTL19_OFFSET                                (0x14C)
#define GLB_REG_GPIO_38_IE                                      GLB_REG_GPIO_38_IE
#define GLB_REG_GPIO_38_IE_POS                                  (0U)
#define GLB_REG_GPIO_38_IE_LEN                                  (1U)
#define GLB_REG_GPIO_38_IE_MSK                                  (((1U<<GLB_REG_GPIO_38_IE_LEN)-1)<<GLB_REG_GPIO_38_IE_POS)
#define GLB_REG_GPIO_38_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_38_IE_LEN)-1)<<GLB_REG_GPIO_38_IE_POS))
#define GLB_REG_GPIO_38_SMT                                     GLB_REG_GPIO_38_SMT
#define GLB_REG_GPIO_38_SMT_POS                                 (1U)
#define GLB_REG_GPIO_38_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_38_SMT_MSK                                 (((1U<<GLB_REG_GPIO_38_SMT_LEN)-1)<<GLB_REG_GPIO_38_SMT_POS)
#define GLB_REG_GPIO_38_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_38_SMT_LEN)-1)<<GLB_REG_GPIO_38_SMT_POS))
#define GLB_REG_GPIO_38_DRV                                     GLB_REG_GPIO_38_DRV
#define GLB_REG_GPIO_38_DRV_POS                                 (2U)
#define GLB_REG_GPIO_38_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_38_DRV_MSK                                 (((1U<<GLB_REG_GPIO_38_DRV_LEN)-1)<<GLB_REG_GPIO_38_DRV_POS)
#define GLB_REG_GPIO_38_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_38_DRV_LEN)-1)<<GLB_REG_GPIO_38_DRV_POS))
#define GLB_REG_GPIO_38_PU                                      GLB_REG_GPIO_38_PU
#define GLB_REG_GPIO_38_PU_POS                                  (4U)
#define GLB_REG_GPIO_38_PU_LEN                                  (1U)
#define GLB_REG_GPIO_38_PU_MSK                                  (((1U<<GLB_REG_GPIO_38_PU_LEN)-1)<<GLB_REG_GPIO_38_PU_POS)
#define GLB_REG_GPIO_38_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_38_PU_LEN)-1)<<GLB_REG_GPIO_38_PU_POS))
#define GLB_REG_GPIO_38_PD                                      GLB_REG_GPIO_38_PD
#define GLB_REG_GPIO_38_PD_POS                                  (5U)
#define GLB_REG_GPIO_38_PD_LEN                                  (1U)
#define GLB_REG_GPIO_38_PD_MSK                                  (((1U<<GLB_REG_GPIO_38_PD_LEN)-1)<<GLB_REG_GPIO_38_PD_POS)
#define GLB_REG_GPIO_38_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_38_PD_LEN)-1)<<GLB_REG_GPIO_38_PD_POS))
#define GLB_REG_GPIO_38_FUNC_SEL                                GLB_REG_GPIO_38_FUNC_SEL
#define GLB_REG_GPIO_38_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_38_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_38_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_38_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_38_FUNC_SEL_POS)
#define GLB_REG_GPIO_38_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_38_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_38_FUNC_SEL_POS))
#define GLB_REG_GPIO_38_MODE                                    GLB_REG_GPIO_38_MODE
#define GLB_REG_GPIO_38_MODE_POS                                (15U)
#define GLB_REG_GPIO_38_MODE_LEN                                (1U)
#define GLB_REG_GPIO_38_MODE_MSK                                (((1U<<GLB_REG_GPIO_38_MODE_LEN)-1)<<GLB_REG_GPIO_38_MODE_POS)
#define GLB_REG_GPIO_38_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_38_MODE_LEN)-1)<<GLB_REG_GPIO_38_MODE_POS))
#define GLB_REG_GPIO_39_IE                                      GLB_REG_GPIO_39_IE
#define GLB_REG_GPIO_39_IE_POS                                  (16U)
#define GLB_REG_GPIO_39_IE_LEN                                  (1U)
#define GLB_REG_GPIO_39_IE_MSK                                  (((1U<<GLB_REG_GPIO_39_IE_LEN)-1)<<GLB_REG_GPIO_39_IE_POS)
#define GLB_REG_GPIO_39_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_39_IE_LEN)-1)<<GLB_REG_GPIO_39_IE_POS))
#define GLB_REG_GPIO_39_SMT                                     GLB_REG_GPIO_39_SMT
#define GLB_REG_GPIO_39_SMT_POS                                 (17U)
#define GLB_REG_GPIO_39_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_39_SMT_MSK                                 (((1U<<GLB_REG_GPIO_39_SMT_LEN)-1)<<GLB_REG_GPIO_39_SMT_POS)
#define GLB_REG_GPIO_39_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_39_SMT_LEN)-1)<<GLB_REG_GPIO_39_SMT_POS))
#define GLB_REG_GPIO_39_DRV                                     GLB_REG_GPIO_39_DRV
#define GLB_REG_GPIO_39_DRV_POS                                 (18U)
#define GLB_REG_GPIO_39_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_39_DRV_MSK                                 (((1U<<GLB_REG_GPIO_39_DRV_LEN)-1)<<GLB_REG_GPIO_39_DRV_POS)
#define GLB_REG_GPIO_39_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_39_DRV_LEN)-1)<<GLB_REG_GPIO_39_DRV_POS))
#define GLB_REG_GPIO_39_PU                                      GLB_REG_GPIO_39_PU
#define GLB_REG_GPIO_39_PU_POS                                  (20U)
#define GLB_REG_GPIO_39_PU_LEN                                  (1U)
#define GLB_REG_GPIO_39_PU_MSK                                  (((1U<<GLB_REG_GPIO_39_PU_LEN)-1)<<GLB_REG_GPIO_39_PU_POS)
#define GLB_REG_GPIO_39_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_39_PU_LEN)-1)<<GLB_REG_GPIO_39_PU_POS))
#define GLB_REG_GPIO_39_PD                                      GLB_REG_GPIO_39_PD
#define GLB_REG_GPIO_39_PD_POS                                  (21U)
#define GLB_REG_GPIO_39_PD_LEN                                  (1U)
#define GLB_REG_GPIO_39_PD_MSK                                  (((1U<<GLB_REG_GPIO_39_PD_LEN)-1)<<GLB_REG_GPIO_39_PD_POS)
#define GLB_REG_GPIO_39_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_39_PD_LEN)-1)<<GLB_REG_GPIO_39_PD_POS))
#define GLB_REG_GPIO_39_FUNC_SEL                                GLB_REG_GPIO_39_FUNC_SEL
#define GLB_REG_GPIO_39_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_39_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_39_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_39_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_39_FUNC_SEL_POS)
#define GLB_REG_GPIO_39_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_39_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_39_FUNC_SEL_POS))
#define GLB_REG_GPIO_39_MODE                                    GLB_REG_GPIO_39_MODE
#define GLB_REG_GPIO_39_MODE_POS                                (31U)
#define GLB_REG_GPIO_39_MODE_LEN                                (1U)
#define GLB_REG_GPIO_39_MODE_MSK                                (((1U<<GLB_REG_GPIO_39_MODE_LEN)-1)<<GLB_REG_GPIO_39_MODE_POS)
#define GLB_REG_GPIO_39_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_39_MODE_LEN)-1)<<GLB_REG_GPIO_39_MODE_POS))

/* 0x150 : GPIO_CFGCTL20 */
#define GLB_GPIO_CFGCTL20_OFFSET                                (0x150)
#define GLB_REG_GPIO_40_IE                                      GLB_REG_GPIO_40_IE
#define GLB_REG_GPIO_40_IE_POS                                  (0U)
#define GLB_REG_GPIO_40_IE_LEN                                  (1U)
#define GLB_REG_GPIO_40_IE_MSK                                  (((1U<<GLB_REG_GPIO_40_IE_LEN)-1)<<GLB_REG_GPIO_40_IE_POS)
#define GLB_REG_GPIO_40_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_40_IE_LEN)-1)<<GLB_REG_GPIO_40_IE_POS))
#define GLB_REG_GPIO_40_SMT                                     GLB_REG_GPIO_40_SMT
#define GLB_REG_GPIO_40_SMT_POS                                 (1U)
#define GLB_REG_GPIO_40_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_40_SMT_MSK                                 (((1U<<GLB_REG_GPIO_40_SMT_LEN)-1)<<GLB_REG_GPIO_40_SMT_POS)
#define GLB_REG_GPIO_40_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_40_SMT_LEN)-1)<<GLB_REG_GPIO_40_SMT_POS))
#define GLB_REG_GPIO_40_DRV                                     GLB_REG_GPIO_40_DRV
#define GLB_REG_GPIO_40_DRV_POS                                 (2U)
#define GLB_REG_GPIO_40_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_40_DRV_MSK                                 (((1U<<GLB_REG_GPIO_40_DRV_LEN)-1)<<GLB_REG_GPIO_40_DRV_POS)
#define GLB_REG_GPIO_40_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_40_DRV_LEN)-1)<<GLB_REG_GPIO_40_DRV_POS))
#define GLB_REG_GPIO_40_PU                                      GLB_REG_GPIO_40_PU
#define GLB_REG_GPIO_40_PU_POS                                  (4U)
#define GLB_REG_GPIO_40_PU_LEN                                  (1U)
#define GLB_REG_GPIO_40_PU_MSK                                  (((1U<<GLB_REG_GPIO_40_PU_LEN)-1)<<GLB_REG_GPIO_40_PU_POS)
#define GLB_REG_GPIO_40_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_40_PU_LEN)-1)<<GLB_REG_GPIO_40_PU_POS))
#define GLB_REG_GPIO_40_PD                                      GLB_REG_GPIO_40_PD
#define GLB_REG_GPIO_40_PD_POS                                  (5U)
#define GLB_REG_GPIO_40_PD_LEN                                  (1U)
#define GLB_REG_GPIO_40_PD_MSK                                  (((1U<<GLB_REG_GPIO_40_PD_LEN)-1)<<GLB_REG_GPIO_40_PD_POS)
#define GLB_REG_GPIO_40_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_40_PD_LEN)-1)<<GLB_REG_GPIO_40_PD_POS))
#define GLB_REG_GPIO_40_FUNC_SEL                                GLB_REG_GPIO_40_FUNC_SEL
#define GLB_REG_GPIO_40_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_40_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_40_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_40_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_40_FUNC_SEL_POS)
#define GLB_REG_GPIO_40_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_40_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_40_FUNC_SEL_POS))
#define GLB_REG_GPIO_40_MODE                                    GLB_REG_GPIO_40_MODE
#define GLB_REG_GPIO_40_MODE_POS                                (15U)
#define GLB_REG_GPIO_40_MODE_LEN                                (1U)
#define GLB_REG_GPIO_40_MODE_MSK                                (((1U<<GLB_REG_GPIO_40_MODE_LEN)-1)<<GLB_REG_GPIO_40_MODE_POS)
#define GLB_REG_GPIO_40_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_40_MODE_LEN)-1)<<GLB_REG_GPIO_40_MODE_POS))
#define GLB_REG_GPIO_41_IE                                      GLB_REG_GPIO_41_IE
#define GLB_REG_GPIO_41_IE_POS                                  (16U)
#define GLB_REG_GPIO_41_IE_LEN                                  (1U)
#define GLB_REG_GPIO_41_IE_MSK                                  (((1U<<GLB_REG_GPIO_41_IE_LEN)-1)<<GLB_REG_GPIO_41_IE_POS)
#define GLB_REG_GPIO_41_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_41_IE_LEN)-1)<<GLB_REG_GPIO_41_IE_POS))
#define GLB_REG_GPIO_41_SMT                                     GLB_REG_GPIO_41_SMT
#define GLB_REG_GPIO_41_SMT_POS                                 (17U)
#define GLB_REG_GPIO_41_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_41_SMT_MSK                                 (((1U<<GLB_REG_GPIO_41_SMT_LEN)-1)<<GLB_REG_GPIO_41_SMT_POS)
#define GLB_REG_GPIO_41_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_41_SMT_LEN)-1)<<GLB_REG_GPIO_41_SMT_POS))
#define GLB_REG_GPIO_41_DRV                                     GLB_REG_GPIO_41_DRV
#define GLB_REG_GPIO_41_DRV_POS                                 (18U)
#define GLB_REG_GPIO_41_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_41_DRV_MSK                                 (((1U<<GLB_REG_GPIO_41_DRV_LEN)-1)<<GLB_REG_GPIO_41_DRV_POS)
#define GLB_REG_GPIO_41_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_41_DRV_LEN)-1)<<GLB_REG_GPIO_41_DRV_POS))
#define GLB_REG_GPIO_41_PU                                      GLB_REG_GPIO_41_PU
#define GLB_REG_GPIO_41_PU_POS                                  (20U)
#define GLB_REG_GPIO_41_PU_LEN                                  (1U)
#define GLB_REG_GPIO_41_PU_MSK                                  (((1U<<GLB_REG_GPIO_41_PU_LEN)-1)<<GLB_REG_GPIO_41_PU_POS)
#define GLB_REG_GPIO_41_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_41_PU_LEN)-1)<<GLB_REG_GPIO_41_PU_POS))
#define GLB_REG_GPIO_41_PD                                      GLB_REG_GPIO_41_PD
#define GLB_REG_GPIO_41_PD_POS                                  (21U)
#define GLB_REG_GPIO_41_PD_LEN                                  (1U)
#define GLB_REG_GPIO_41_PD_MSK                                  (((1U<<GLB_REG_GPIO_41_PD_LEN)-1)<<GLB_REG_GPIO_41_PD_POS)
#define GLB_REG_GPIO_41_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_41_PD_LEN)-1)<<GLB_REG_GPIO_41_PD_POS))
#define GLB_REG_GPIO_41_FUNC_SEL                                GLB_REG_GPIO_41_FUNC_SEL
#define GLB_REG_GPIO_41_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_41_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_41_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_41_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_41_FUNC_SEL_POS)
#define GLB_REG_GPIO_41_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_41_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_41_FUNC_SEL_POS))
#define GLB_REG_GPIO_41_MODE                                    GLB_REG_GPIO_41_MODE
#define GLB_REG_GPIO_41_MODE_POS                                (31U)
#define GLB_REG_GPIO_41_MODE_LEN                                (1U)
#define GLB_REG_GPIO_41_MODE_MSK                                (((1U<<GLB_REG_GPIO_41_MODE_LEN)-1)<<GLB_REG_GPIO_41_MODE_POS)
#define GLB_REG_GPIO_41_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_41_MODE_LEN)-1)<<GLB_REG_GPIO_41_MODE_POS))

/* 0x154 : GPIO_CFGCTL21 */
#define GLB_GPIO_CFGCTL21_OFFSET                                (0x154)
#define GLB_REG_GPIO_42_IE                                      GLB_REG_GPIO_42_IE
#define GLB_REG_GPIO_42_IE_POS                                  (0U)
#define GLB_REG_GPIO_42_IE_LEN                                  (1U)
#define GLB_REG_GPIO_42_IE_MSK                                  (((1U<<GLB_REG_GPIO_42_IE_LEN)-1)<<GLB_REG_GPIO_42_IE_POS)
#define GLB_REG_GPIO_42_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_42_IE_LEN)-1)<<GLB_REG_GPIO_42_IE_POS))
#define GLB_REG_GPIO_42_SMT                                     GLB_REG_GPIO_42_SMT
#define GLB_REG_GPIO_42_SMT_POS                                 (1U)
#define GLB_REG_GPIO_42_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_42_SMT_MSK                                 (((1U<<GLB_REG_GPIO_42_SMT_LEN)-1)<<GLB_REG_GPIO_42_SMT_POS)
#define GLB_REG_GPIO_42_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_42_SMT_LEN)-1)<<GLB_REG_GPIO_42_SMT_POS))
#define GLB_REG_GPIO_42_DRV                                     GLB_REG_GPIO_42_DRV
#define GLB_REG_GPIO_42_DRV_POS                                 (2U)
#define GLB_REG_GPIO_42_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_42_DRV_MSK                                 (((1U<<GLB_REG_GPIO_42_DRV_LEN)-1)<<GLB_REG_GPIO_42_DRV_POS)
#define GLB_REG_GPIO_42_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_42_DRV_LEN)-1)<<GLB_REG_GPIO_42_DRV_POS))
#define GLB_REG_GPIO_42_PU                                      GLB_REG_GPIO_42_PU
#define GLB_REG_GPIO_42_PU_POS                                  (4U)
#define GLB_REG_GPIO_42_PU_LEN                                  (1U)
#define GLB_REG_GPIO_42_PU_MSK                                  (((1U<<GLB_REG_GPIO_42_PU_LEN)-1)<<GLB_REG_GPIO_42_PU_POS)
#define GLB_REG_GPIO_42_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_42_PU_LEN)-1)<<GLB_REG_GPIO_42_PU_POS))
#define GLB_REG_GPIO_42_PD                                      GLB_REG_GPIO_42_PD
#define GLB_REG_GPIO_42_PD_POS                                  (5U)
#define GLB_REG_GPIO_42_PD_LEN                                  (1U)
#define GLB_REG_GPIO_42_PD_MSK                                  (((1U<<GLB_REG_GPIO_42_PD_LEN)-1)<<GLB_REG_GPIO_42_PD_POS)
#define GLB_REG_GPIO_42_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_42_PD_LEN)-1)<<GLB_REG_GPIO_42_PD_POS))
#define GLB_REG_GPIO_42_FUNC_SEL                                GLB_REG_GPIO_42_FUNC_SEL
#define GLB_REG_GPIO_42_FUNC_SEL_POS                            (8U)
#define GLB_REG_GPIO_42_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_42_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_42_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_42_FUNC_SEL_POS)
#define GLB_REG_GPIO_42_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_42_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_42_FUNC_SEL_POS))
#define GLB_REG_GPIO_42_MODE                                    GLB_REG_GPIO_42_MODE
#define GLB_REG_GPIO_42_MODE_POS                                (15U)
#define GLB_REG_GPIO_42_MODE_LEN                                (1U)
#define GLB_REG_GPIO_42_MODE_MSK                                (((1U<<GLB_REG_GPIO_42_MODE_LEN)-1)<<GLB_REG_GPIO_42_MODE_POS)
#define GLB_REG_GPIO_42_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_42_MODE_LEN)-1)<<GLB_REG_GPIO_42_MODE_POS))
#define GLB_REG_GPIO_43_IE                                      GLB_REG_GPIO_43_IE
#define GLB_REG_GPIO_43_IE_POS                                  (16U)
#define GLB_REG_GPIO_43_IE_LEN                                  (1U)
#define GLB_REG_GPIO_43_IE_MSK                                  (((1U<<GLB_REG_GPIO_43_IE_LEN)-1)<<GLB_REG_GPIO_43_IE_POS)
#define GLB_REG_GPIO_43_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_43_IE_LEN)-1)<<GLB_REG_GPIO_43_IE_POS))
#define GLB_REG_GPIO_43_SMT                                     GLB_REG_GPIO_43_SMT
#define GLB_REG_GPIO_43_SMT_POS                                 (17U)
#define GLB_REG_GPIO_43_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_43_SMT_MSK                                 (((1U<<GLB_REG_GPIO_43_SMT_LEN)-1)<<GLB_REG_GPIO_43_SMT_POS)
#define GLB_REG_GPIO_43_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_43_SMT_LEN)-1)<<GLB_REG_GPIO_43_SMT_POS))
#define GLB_REG_GPIO_43_DRV                                     GLB_REG_GPIO_43_DRV
#define GLB_REG_GPIO_43_DRV_POS                                 (18U)
#define GLB_REG_GPIO_43_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_43_DRV_MSK                                 (((1U<<GLB_REG_GPIO_43_DRV_LEN)-1)<<GLB_REG_GPIO_43_DRV_POS)
#define GLB_REG_GPIO_43_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_43_DRV_LEN)-1)<<GLB_REG_GPIO_43_DRV_POS))
#define GLB_REG_GPIO_43_PU                                      GLB_REG_GPIO_43_PU
#define GLB_REG_GPIO_43_PU_POS                                  (20U)
#define GLB_REG_GPIO_43_PU_LEN                                  (1U)
#define GLB_REG_GPIO_43_PU_MSK                                  (((1U<<GLB_REG_GPIO_43_PU_LEN)-1)<<GLB_REG_GPIO_43_PU_POS)
#define GLB_REG_GPIO_43_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_43_PU_LEN)-1)<<GLB_REG_GPIO_43_PU_POS))
#define GLB_REG_GPIO_43_PD                                      GLB_REG_GPIO_43_PD
#define GLB_REG_GPIO_43_PD_POS                                  (21U)
#define GLB_REG_GPIO_43_PD_LEN                                  (1U)
#define GLB_REG_GPIO_43_PD_MSK                                  (((1U<<GLB_REG_GPIO_43_PD_LEN)-1)<<GLB_REG_GPIO_43_PD_POS)
#define GLB_REG_GPIO_43_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_43_PD_LEN)-1)<<GLB_REG_GPIO_43_PD_POS))
#define GLB_REG_GPIO_43_FUNC_SEL                                GLB_REG_GPIO_43_FUNC_SEL
#define GLB_REG_GPIO_43_FUNC_SEL_POS                            (24U)
#define GLB_REG_GPIO_43_FUNC_SEL_LEN                            (5U)
#define GLB_REG_GPIO_43_FUNC_SEL_MSK                            (((1U<<GLB_REG_GPIO_43_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_43_FUNC_SEL_POS)
#define GLB_REG_GPIO_43_FUNC_SEL_UMSK                           (~(((1U<<GLB_REG_GPIO_43_FUNC_SEL_LEN)-1)<<GLB_REG_GPIO_43_FUNC_SEL_POS))
#define GLB_REG_GPIO_43_MODE                                    GLB_REG_GPIO_43_MODE
#define GLB_REG_GPIO_43_MODE_POS                                (31U)
#define GLB_REG_GPIO_43_MODE_LEN                                (1U)
#define GLB_REG_GPIO_43_MODE_MSK                                (((1U<<GLB_REG_GPIO_43_MODE_LEN)-1)<<GLB_REG_GPIO_43_MODE_POS)
#define GLB_REG_GPIO_43_MODE_UMSK                               (~(((1U<<GLB_REG_GPIO_43_MODE_LEN)-1)<<GLB_REG_GPIO_43_MODE_POS))

/* 0x158 : GPIO_CFGCTL22 */
#define GLB_GPIO_CFGCTL22_OFFSET                                (0x158)
#define GLB_REG_GPIO_44_IE                                      GLB_REG_GPIO_44_IE
#define GLB_REG_GPIO_44_IE_POS                                  (0U)
#define GLB_REG_GPIO_44_IE_LEN                                  (1U)
#define GLB_REG_GPIO_44_IE_MSK                                  (((1U<<GLB_REG_GPIO_44_IE_LEN)-1)<<GLB_REG_GPIO_44_IE_POS)
#define GLB_REG_GPIO_44_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_44_IE_LEN)-1)<<GLB_REG_GPIO_44_IE_POS))
#define GLB_REG_GPIO_44_SMT                                     GLB_REG_GPIO_44_SMT
#define GLB_REG_GPIO_44_SMT_POS                                 (1U)
#define GLB_REG_GPIO_44_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_44_SMT_MSK                                 (((1U<<GLB_REG_GPIO_44_SMT_LEN)-1)<<GLB_REG_GPIO_44_SMT_POS)
#define GLB_REG_GPIO_44_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_44_SMT_LEN)-1)<<GLB_REG_GPIO_44_SMT_POS))
#define GLB_REG_GPIO_44_DRV                                     GLB_REG_GPIO_44_DRV
#define GLB_REG_GPIO_44_DRV_POS                                 (2U)
#define GLB_REG_GPIO_44_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_44_DRV_MSK                                 (((1U<<GLB_REG_GPIO_44_DRV_LEN)-1)<<GLB_REG_GPIO_44_DRV_POS)
#define GLB_REG_GPIO_44_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_44_DRV_LEN)-1)<<GLB_REG_GPIO_44_DRV_POS))
#define GLB_REG_GPIO_44_PU                                      GLB_REG_GPIO_44_PU
#define GLB_REG_GPIO_44_PU_POS                                  (4U)
#define GLB_REG_GPIO_44_PU_LEN                                  (1U)
#define GLB_REG_GPIO_44_PU_MSK                                  (((1U<<GLB_REG_GPIO_44_PU_LEN)-1)<<GLB_REG_GPIO_44_PU_POS)
#define GLB_REG_GPIO_44_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_44_PU_LEN)-1)<<GLB_REG_GPIO_44_PU_POS))
#define GLB_REG_GPIO_44_PD                                      GLB_REG_GPIO_44_PD
#define GLB_REG_GPIO_44_PD_POS                                  (5U)
#define GLB_REG_GPIO_44_PD_LEN                                  (1U)
#define GLB_REG_GPIO_44_PD_MSK                                  (((1U<<GLB_REG_GPIO_44_PD_LEN)-1)<<GLB_REG_GPIO_44_PD_POS)
#define GLB_REG_GPIO_44_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_44_PD_LEN)-1)<<GLB_REG_GPIO_44_PD_POS))
#define GLB_REG_GPIO_45_IE                                      GLB_REG_GPIO_45_IE
#define GLB_REG_GPIO_45_IE_POS                                  (16U)
#define GLB_REG_GPIO_45_IE_LEN                                  (1U)
#define GLB_REG_GPIO_45_IE_MSK                                  (((1U<<GLB_REG_GPIO_45_IE_LEN)-1)<<GLB_REG_GPIO_45_IE_POS)
#define GLB_REG_GPIO_45_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_45_IE_LEN)-1)<<GLB_REG_GPIO_45_IE_POS))
#define GLB_REG_GPIO_45_SMT                                     GLB_REG_GPIO_45_SMT
#define GLB_REG_GPIO_45_SMT_POS                                 (17U)
#define GLB_REG_GPIO_45_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_45_SMT_MSK                                 (((1U<<GLB_REG_GPIO_45_SMT_LEN)-1)<<GLB_REG_GPIO_45_SMT_POS)
#define GLB_REG_GPIO_45_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_45_SMT_LEN)-1)<<GLB_REG_GPIO_45_SMT_POS))
#define GLB_REG_GPIO_45_DRV                                     GLB_REG_GPIO_45_DRV
#define GLB_REG_GPIO_45_DRV_POS                                 (18U)
#define GLB_REG_GPIO_45_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_45_DRV_MSK                                 (((1U<<GLB_REG_GPIO_45_DRV_LEN)-1)<<GLB_REG_GPIO_45_DRV_POS)
#define GLB_REG_GPIO_45_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_45_DRV_LEN)-1)<<GLB_REG_GPIO_45_DRV_POS))
#define GLB_REG_GPIO_45_PU                                      GLB_REG_GPIO_45_PU
#define GLB_REG_GPIO_45_PU_POS                                  (20U)
#define GLB_REG_GPIO_45_PU_LEN                                  (1U)
#define GLB_REG_GPIO_45_PU_MSK                                  (((1U<<GLB_REG_GPIO_45_PU_LEN)-1)<<GLB_REG_GPIO_45_PU_POS)
#define GLB_REG_GPIO_45_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_45_PU_LEN)-1)<<GLB_REG_GPIO_45_PU_POS))
#define GLB_REG_GPIO_45_PD                                      GLB_REG_GPIO_45_PD
#define GLB_REG_GPIO_45_PD_POS                                  (21U)
#define GLB_REG_GPIO_45_PD_LEN                                  (1U)
#define GLB_REG_GPIO_45_PD_MSK                                  (((1U<<GLB_REG_GPIO_45_PD_LEN)-1)<<GLB_REG_GPIO_45_PD_POS)
#define GLB_REG_GPIO_45_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_45_PD_LEN)-1)<<GLB_REG_GPIO_45_PD_POS))

/* 0x15C : GPIO_CFGCTL23 */
#define GLB_GPIO_CFGCTL23_OFFSET                                (0x15C)
#define GLB_REG_GPIO_46_IE                                      GLB_REG_GPIO_46_IE
#define GLB_REG_GPIO_46_IE_POS                                  (0U)
#define GLB_REG_GPIO_46_IE_LEN                                  (1U)
#define GLB_REG_GPIO_46_IE_MSK                                  (((1U<<GLB_REG_GPIO_46_IE_LEN)-1)<<GLB_REG_GPIO_46_IE_POS)
#define GLB_REG_GPIO_46_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_46_IE_LEN)-1)<<GLB_REG_GPIO_46_IE_POS))
#define GLB_REG_GPIO_46_SMT                                     GLB_REG_GPIO_46_SMT
#define GLB_REG_GPIO_46_SMT_POS                                 (1U)
#define GLB_REG_GPIO_46_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_46_SMT_MSK                                 (((1U<<GLB_REG_GPIO_46_SMT_LEN)-1)<<GLB_REG_GPIO_46_SMT_POS)
#define GLB_REG_GPIO_46_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_46_SMT_LEN)-1)<<GLB_REG_GPIO_46_SMT_POS))
#define GLB_REG_GPIO_46_DRV                                     GLB_REG_GPIO_46_DRV
#define GLB_REG_GPIO_46_DRV_POS                                 (2U)
#define GLB_REG_GPIO_46_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_46_DRV_MSK                                 (((1U<<GLB_REG_GPIO_46_DRV_LEN)-1)<<GLB_REG_GPIO_46_DRV_POS)
#define GLB_REG_GPIO_46_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_46_DRV_LEN)-1)<<GLB_REG_GPIO_46_DRV_POS))
#define GLB_REG_GPIO_46_PU                                      GLB_REG_GPIO_46_PU
#define GLB_REG_GPIO_46_PU_POS                                  (4U)
#define GLB_REG_GPIO_46_PU_LEN                                  (1U)
#define GLB_REG_GPIO_46_PU_MSK                                  (((1U<<GLB_REG_GPIO_46_PU_LEN)-1)<<GLB_REG_GPIO_46_PU_POS)
#define GLB_REG_GPIO_46_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_46_PU_LEN)-1)<<GLB_REG_GPIO_46_PU_POS))
#define GLB_REG_GPIO_46_PD                                      GLB_REG_GPIO_46_PD
#define GLB_REG_GPIO_46_PD_POS                                  (5U)
#define GLB_REG_GPIO_46_PD_LEN                                  (1U)
#define GLB_REG_GPIO_46_PD_MSK                                  (((1U<<GLB_REG_GPIO_46_PD_LEN)-1)<<GLB_REG_GPIO_46_PD_POS)
#define GLB_REG_GPIO_46_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_46_PD_LEN)-1)<<GLB_REG_GPIO_46_PD_POS))
#define GLB_REG_GPIO_47_IE                                      GLB_REG_GPIO_47_IE
#define GLB_REG_GPIO_47_IE_POS                                  (16U)
#define GLB_REG_GPIO_47_IE_LEN                                  (1U)
#define GLB_REG_GPIO_47_IE_MSK                                  (((1U<<GLB_REG_GPIO_47_IE_LEN)-1)<<GLB_REG_GPIO_47_IE_POS)
#define GLB_REG_GPIO_47_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_47_IE_LEN)-1)<<GLB_REG_GPIO_47_IE_POS))
#define GLB_REG_GPIO_47_SMT                                     GLB_REG_GPIO_47_SMT
#define GLB_REG_GPIO_47_SMT_POS                                 (17U)
#define GLB_REG_GPIO_47_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_47_SMT_MSK                                 (((1U<<GLB_REG_GPIO_47_SMT_LEN)-1)<<GLB_REG_GPIO_47_SMT_POS)
#define GLB_REG_GPIO_47_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_47_SMT_LEN)-1)<<GLB_REG_GPIO_47_SMT_POS))
#define GLB_REG_GPIO_47_DRV                                     GLB_REG_GPIO_47_DRV
#define GLB_REG_GPIO_47_DRV_POS                                 (18U)
#define GLB_REG_GPIO_47_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_47_DRV_MSK                                 (((1U<<GLB_REG_GPIO_47_DRV_LEN)-1)<<GLB_REG_GPIO_47_DRV_POS)
#define GLB_REG_GPIO_47_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_47_DRV_LEN)-1)<<GLB_REG_GPIO_47_DRV_POS))
#define GLB_REG_GPIO_47_PU                                      GLB_REG_GPIO_47_PU
#define GLB_REG_GPIO_47_PU_POS                                  (20U)
#define GLB_REG_GPIO_47_PU_LEN                                  (1U)
#define GLB_REG_GPIO_47_PU_MSK                                  (((1U<<GLB_REG_GPIO_47_PU_LEN)-1)<<GLB_REG_GPIO_47_PU_POS)
#define GLB_REG_GPIO_47_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_47_PU_LEN)-1)<<GLB_REG_GPIO_47_PU_POS))
#define GLB_REG_GPIO_47_PD                                      GLB_REG_GPIO_47_PD
#define GLB_REG_GPIO_47_PD_POS                                  (21U)
#define GLB_REG_GPIO_47_PD_LEN                                  (1U)
#define GLB_REG_GPIO_47_PD_MSK                                  (((1U<<GLB_REG_GPIO_47_PD_LEN)-1)<<GLB_REG_GPIO_47_PD_POS)
#define GLB_REG_GPIO_47_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_47_PD_LEN)-1)<<GLB_REG_GPIO_47_PD_POS))

/* 0x160 : GPIO_CFGCTL24 */
#define GLB_GPIO_CFGCTL24_OFFSET                                (0x160)
#define GLB_REG_GPIO_48_IE                                      GLB_REG_GPIO_48_IE
#define GLB_REG_GPIO_48_IE_POS                                  (0U)
#define GLB_REG_GPIO_48_IE_LEN                                  (1U)
#define GLB_REG_GPIO_48_IE_MSK                                  (((1U<<GLB_REG_GPIO_48_IE_LEN)-1)<<GLB_REG_GPIO_48_IE_POS)
#define GLB_REG_GPIO_48_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_48_IE_LEN)-1)<<GLB_REG_GPIO_48_IE_POS))
#define GLB_REG_GPIO_48_SMT                                     GLB_REG_GPIO_48_SMT
#define GLB_REG_GPIO_48_SMT_POS                                 (1U)
#define GLB_REG_GPIO_48_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_48_SMT_MSK                                 (((1U<<GLB_REG_GPIO_48_SMT_LEN)-1)<<GLB_REG_GPIO_48_SMT_POS)
#define GLB_REG_GPIO_48_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_48_SMT_LEN)-1)<<GLB_REG_GPIO_48_SMT_POS))
#define GLB_REG_GPIO_48_DRV                                     GLB_REG_GPIO_48_DRV
#define GLB_REG_GPIO_48_DRV_POS                                 (2U)
#define GLB_REG_GPIO_48_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_48_DRV_MSK                                 (((1U<<GLB_REG_GPIO_48_DRV_LEN)-1)<<GLB_REG_GPIO_48_DRV_POS)
#define GLB_REG_GPIO_48_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_48_DRV_LEN)-1)<<GLB_REG_GPIO_48_DRV_POS))
#define GLB_REG_GPIO_48_PU                                      GLB_REG_GPIO_48_PU
#define GLB_REG_GPIO_48_PU_POS                                  (4U)
#define GLB_REG_GPIO_48_PU_LEN                                  (1U)
#define GLB_REG_GPIO_48_PU_MSK                                  (((1U<<GLB_REG_GPIO_48_PU_LEN)-1)<<GLB_REG_GPIO_48_PU_POS)
#define GLB_REG_GPIO_48_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_48_PU_LEN)-1)<<GLB_REG_GPIO_48_PU_POS))
#define GLB_REG_GPIO_48_PD                                      GLB_REG_GPIO_48_PD
#define GLB_REG_GPIO_48_PD_POS                                  (5U)
#define GLB_REG_GPIO_48_PD_LEN                                  (1U)
#define GLB_REG_GPIO_48_PD_MSK                                  (((1U<<GLB_REG_GPIO_48_PD_LEN)-1)<<GLB_REG_GPIO_48_PD_POS)
#define GLB_REG_GPIO_48_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_48_PD_LEN)-1)<<GLB_REG_GPIO_48_PD_POS))
#define GLB_REG_GPIO_49_IE                                      GLB_REG_GPIO_49_IE
#define GLB_REG_GPIO_49_IE_POS                                  (16U)
#define GLB_REG_GPIO_49_IE_LEN                                  (1U)
#define GLB_REG_GPIO_49_IE_MSK                                  (((1U<<GLB_REG_GPIO_49_IE_LEN)-1)<<GLB_REG_GPIO_49_IE_POS)
#define GLB_REG_GPIO_49_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_49_IE_LEN)-1)<<GLB_REG_GPIO_49_IE_POS))
#define GLB_REG_GPIO_49_SMT                                     GLB_REG_GPIO_49_SMT
#define GLB_REG_GPIO_49_SMT_POS                                 (17U)
#define GLB_REG_GPIO_49_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_49_SMT_MSK                                 (((1U<<GLB_REG_GPIO_49_SMT_LEN)-1)<<GLB_REG_GPIO_49_SMT_POS)
#define GLB_REG_GPIO_49_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_49_SMT_LEN)-1)<<GLB_REG_GPIO_49_SMT_POS))
#define GLB_REG_GPIO_49_DRV                                     GLB_REG_GPIO_49_DRV
#define GLB_REG_GPIO_49_DRV_POS                                 (18U)
#define GLB_REG_GPIO_49_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_49_DRV_MSK                                 (((1U<<GLB_REG_GPIO_49_DRV_LEN)-1)<<GLB_REG_GPIO_49_DRV_POS)
#define GLB_REG_GPIO_49_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_49_DRV_LEN)-1)<<GLB_REG_GPIO_49_DRV_POS))
#define GLB_REG_GPIO_49_PU                                      GLB_REG_GPIO_49_PU
#define GLB_REG_GPIO_49_PU_POS                                  (20U)
#define GLB_REG_GPIO_49_PU_LEN                                  (1U)
#define GLB_REG_GPIO_49_PU_MSK                                  (((1U<<GLB_REG_GPIO_49_PU_LEN)-1)<<GLB_REG_GPIO_49_PU_POS)
#define GLB_REG_GPIO_49_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_49_PU_LEN)-1)<<GLB_REG_GPIO_49_PU_POS))
#define GLB_REG_GPIO_49_PD                                      GLB_REG_GPIO_49_PD
#define GLB_REG_GPIO_49_PD_POS                                  (21U)
#define GLB_REG_GPIO_49_PD_LEN                                  (1U)
#define GLB_REG_GPIO_49_PD_MSK                                  (((1U<<GLB_REG_GPIO_49_PD_LEN)-1)<<GLB_REG_GPIO_49_PD_POS)
#define GLB_REG_GPIO_49_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_49_PD_LEN)-1)<<GLB_REG_GPIO_49_PD_POS))

/* 0x164 : GPIO_CFGCTL25 */
#define GLB_GPIO_CFGCTL25_OFFSET                                (0x164)
#define GLB_REG_GPIO_50_IE                                      GLB_REG_GPIO_50_IE
#define GLB_REG_GPIO_50_IE_POS                                  (0U)
#define GLB_REG_GPIO_50_IE_LEN                                  (1U)
#define GLB_REG_GPIO_50_IE_MSK                                  (((1U<<GLB_REG_GPIO_50_IE_LEN)-1)<<GLB_REG_GPIO_50_IE_POS)
#define GLB_REG_GPIO_50_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_50_IE_LEN)-1)<<GLB_REG_GPIO_50_IE_POS))
#define GLB_REG_GPIO_50_SMT                                     GLB_REG_GPIO_50_SMT
#define GLB_REG_GPIO_50_SMT_POS                                 (1U)
#define GLB_REG_GPIO_50_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_50_SMT_MSK                                 (((1U<<GLB_REG_GPIO_50_SMT_LEN)-1)<<GLB_REG_GPIO_50_SMT_POS)
#define GLB_REG_GPIO_50_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_50_SMT_LEN)-1)<<GLB_REG_GPIO_50_SMT_POS))
#define GLB_REG_GPIO_50_DRV                                     GLB_REG_GPIO_50_DRV
#define GLB_REG_GPIO_50_DRV_POS                                 (2U)
#define GLB_REG_GPIO_50_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_50_DRV_MSK                                 (((1U<<GLB_REG_GPIO_50_DRV_LEN)-1)<<GLB_REG_GPIO_50_DRV_POS)
#define GLB_REG_GPIO_50_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_50_DRV_LEN)-1)<<GLB_REG_GPIO_50_DRV_POS))
#define GLB_REG_GPIO_50_PU                                      GLB_REG_GPIO_50_PU
#define GLB_REG_GPIO_50_PU_POS                                  (4U)
#define GLB_REG_GPIO_50_PU_LEN                                  (1U)
#define GLB_REG_GPIO_50_PU_MSK                                  (((1U<<GLB_REG_GPIO_50_PU_LEN)-1)<<GLB_REG_GPIO_50_PU_POS)
#define GLB_REG_GPIO_50_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_50_PU_LEN)-1)<<GLB_REG_GPIO_50_PU_POS))
#define GLB_REG_GPIO_50_PD                                      GLB_REG_GPIO_50_PD
#define GLB_REG_GPIO_50_PD_POS                                  (5U)
#define GLB_REG_GPIO_50_PD_LEN                                  (1U)
#define GLB_REG_GPIO_50_PD_MSK                                  (((1U<<GLB_REG_GPIO_50_PD_LEN)-1)<<GLB_REG_GPIO_50_PD_POS)
#define GLB_REG_GPIO_50_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_50_PD_LEN)-1)<<GLB_REG_GPIO_50_PD_POS))
#define GLB_REG_GPIO_50_ES2                                     GLB_REG_GPIO_50_ES2
#define GLB_REG_GPIO_50_ES2_POS                                 (6U)
#define GLB_REG_GPIO_50_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_50_ES2_MSK                                 (((1U<<GLB_REG_GPIO_50_ES2_LEN)-1)<<GLB_REG_GPIO_50_ES2_POS)
#define GLB_REG_GPIO_50_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_50_ES2_LEN)-1)<<GLB_REG_GPIO_50_ES2_POS))
#define GLB_REG_GPIO_50_ES3                                     GLB_REG_GPIO_50_ES3
#define GLB_REG_GPIO_50_ES3_POS                                 (7U)
#define GLB_REG_GPIO_50_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_50_ES3_MSK                                 (((1U<<GLB_REG_GPIO_50_ES3_LEN)-1)<<GLB_REG_GPIO_50_ES3_POS)
#define GLB_REG_GPIO_50_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_50_ES3_LEN)-1)<<GLB_REG_GPIO_50_ES3_POS))
#define GLB_REG_GPIO_50_ES4                                     GLB_REG_GPIO_50_ES4
#define GLB_REG_GPIO_50_ES4_POS                                 (8U)
#define GLB_REG_GPIO_50_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_50_ES4_MSK                                 (((1U<<GLB_REG_GPIO_50_ES4_LEN)-1)<<GLB_REG_GPIO_50_ES4_POS)
#define GLB_REG_GPIO_50_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_50_ES4_LEN)-1)<<GLB_REG_GPIO_50_ES4_POS))
#define GLB_REG_GPIO_51_IE                                      GLB_REG_GPIO_51_IE
#define GLB_REG_GPIO_51_IE_POS                                  (16U)
#define GLB_REG_GPIO_51_IE_LEN                                  (1U)
#define GLB_REG_GPIO_51_IE_MSK                                  (((1U<<GLB_REG_GPIO_51_IE_LEN)-1)<<GLB_REG_GPIO_51_IE_POS)
#define GLB_REG_GPIO_51_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_51_IE_LEN)-1)<<GLB_REG_GPIO_51_IE_POS))
#define GLB_REG_GPIO_51_SMT                                     GLB_REG_GPIO_51_SMT
#define GLB_REG_GPIO_51_SMT_POS                                 (17U)
#define GLB_REG_GPIO_51_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_51_SMT_MSK                                 (((1U<<GLB_REG_GPIO_51_SMT_LEN)-1)<<GLB_REG_GPIO_51_SMT_POS)
#define GLB_REG_GPIO_51_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_51_SMT_LEN)-1)<<GLB_REG_GPIO_51_SMT_POS))
#define GLB_REG_GPIO_51_DRV                                     GLB_REG_GPIO_51_DRV
#define GLB_REG_GPIO_51_DRV_POS                                 (18U)
#define GLB_REG_GPIO_51_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_51_DRV_MSK                                 (((1U<<GLB_REG_GPIO_51_DRV_LEN)-1)<<GLB_REG_GPIO_51_DRV_POS)
#define GLB_REG_GPIO_51_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_51_DRV_LEN)-1)<<GLB_REG_GPIO_51_DRV_POS))
#define GLB_REG_GPIO_51_PU                                      GLB_REG_GPIO_51_PU
#define GLB_REG_GPIO_51_PU_POS                                  (20U)
#define GLB_REG_GPIO_51_PU_LEN                                  (1U)
#define GLB_REG_GPIO_51_PU_MSK                                  (((1U<<GLB_REG_GPIO_51_PU_LEN)-1)<<GLB_REG_GPIO_51_PU_POS)
#define GLB_REG_GPIO_51_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_51_PU_LEN)-1)<<GLB_REG_GPIO_51_PU_POS))
#define GLB_REG_GPIO_51_PD                                      GLB_REG_GPIO_51_PD
#define GLB_REG_GPIO_51_PD_POS                                  (21U)
#define GLB_REG_GPIO_51_PD_LEN                                  (1U)
#define GLB_REG_GPIO_51_PD_MSK                                  (((1U<<GLB_REG_GPIO_51_PD_LEN)-1)<<GLB_REG_GPIO_51_PD_POS)
#define GLB_REG_GPIO_51_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_51_PD_LEN)-1)<<GLB_REG_GPIO_51_PD_POS))
#define GLB_REG_GPIO_51_ES2                                     GLB_REG_GPIO_51_ES2
#define GLB_REG_GPIO_51_ES2_POS                                 (22U)
#define GLB_REG_GPIO_51_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_51_ES2_MSK                                 (((1U<<GLB_REG_GPIO_51_ES2_LEN)-1)<<GLB_REG_GPIO_51_ES2_POS)
#define GLB_REG_GPIO_51_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_51_ES2_LEN)-1)<<GLB_REG_GPIO_51_ES2_POS))
#define GLB_REG_GPIO_51_ES3                                     GLB_REG_GPIO_51_ES3
#define GLB_REG_GPIO_51_ES3_POS                                 (23U)
#define GLB_REG_GPIO_51_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_51_ES3_MSK                                 (((1U<<GLB_REG_GPIO_51_ES3_LEN)-1)<<GLB_REG_GPIO_51_ES3_POS)
#define GLB_REG_GPIO_51_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_51_ES3_LEN)-1)<<GLB_REG_GPIO_51_ES3_POS))
#define GLB_REG_GPIO_51_ES4                                     GLB_REG_GPIO_51_ES4
#define GLB_REG_GPIO_51_ES4_POS                                 (24U)
#define GLB_REG_GPIO_51_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_51_ES4_MSK                                 (((1U<<GLB_REG_GPIO_51_ES4_LEN)-1)<<GLB_REG_GPIO_51_ES4_POS)
#define GLB_REG_GPIO_51_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_51_ES4_LEN)-1)<<GLB_REG_GPIO_51_ES4_POS))

/* 0x168 : GPIO_CFGCTL26 */
#define GLB_GPIO_CFGCTL26_OFFSET                                (0x168)
#define GLB_REG_GPIO_52_IE                                      GLB_REG_GPIO_52_IE
#define GLB_REG_GPIO_52_IE_POS                                  (0U)
#define GLB_REG_GPIO_52_IE_LEN                                  (1U)
#define GLB_REG_GPIO_52_IE_MSK                                  (((1U<<GLB_REG_GPIO_52_IE_LEN)-1)<<GLB_REG_GPIO_52_IE_POS)
#define GLB_REG_GPIO_52_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_52_IE_LEN)-1)<<GLB_REG_GPIO_52_IE_POS))
#define GLB_REG_GPIO_52_SMT                                     GLB_REG_GPIO_52_SMT
#define GLB_REG_GPIO_52_SMT_POS                                 (1U)
#define GLB_REG_GPIO_52_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_52_SMT_MSK                                 (((1U<<GLB_REG_GPIO_52_SMT_LEN)-1)<<GLB_REG_GPIO_52_SMT_POS)
#define GLB_REG_GPIO_52_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_52_SMT_LEN)-1)<<GLB_REG_GPIO_52_SMT_POS))
#define GLB_REG_GPIO_52_DRV                                     GLB_REG_GPIO_52_DRV
#define GLB_REG_GPIO_52_DRV_POS                                 (2U)
#define GLB_REG_GPIO_52_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_52_DRV_MSK                                 (((1U<<GLB_REG_GPIO_52_DRV_LEN)-1)<<GLB_REG_GPIO_52_DRV_POS)
#define GLB_REG_GPIO_52_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_52_DRV_LEN)-1)<<GLB_REG_GPIO_52_DRV_POS))
#define GLB_REG_GPIO_52_PU                                      GLB_REG_GPIO_52_PU
#define GLB_REG_GPIO_52_PU_POS                                  (4U)
#define GLB_REG_GPIO_52_PU_LEN                                  (1U)
#define GLB_REG_GPIO_52_PU_MSK                                  (((1U<<GLB_REG_GPIO_52_PU_LEN)-1)<<GLB_REG_GPIO_52_PU_POS)
#define GLB_REG_GPIO_52_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_52_PU_LEN)-1)<<GLB_REG_GPIO_52_PU_POS))
#define GLB_REG_GPIO_52_PD                                      GLB_REG_GPIO_52_PD
#define GLB_REG_GPIO_52_PD_POS                                  (5U)
#define GLB_REG_GPIO_52_PD_LEN                                  (1U)
#define GLB_REG_GPIO_52_PD_MSK                                  (((1U<<GLB_REG_GPIO_52_PD_LEN)-1)<<GLB_REG_GPIO_52_PD_POS)
#define GLB_REG_GPIO_52_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_52_PD_LEN)-1)<<GLB_REG_GPIO_52_PD_POS))
#define GLB_REG_GPIO_52_ES2                                     GLB_REG_GPIO_52_ES2
#define GLB_REG_GPIO_52_ES2_POS                                 (6U)
#define GLB_REG_GPIO_52_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_52_ES2_MSK                                 (((1U<<GLB_REG_GPIO_52_ES2_LEN)-1)<<GLB_REG_GPIO_52_ES2_POS)
#define GLB_REG_GPIO_52_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_52_ES2_LEN)-1)<<GLB_REG_GPIO_52_ES2_POS))
#define GLB_REG_GPIO_52_ES3                                     GLB_REG_GPIO_52_ES3
#define GLB_REG_GPIO_52_ES3_POS                                 (7U)
#define GLB_REG_GPIO_52_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_52_ES3_MSK                                 (((1U<<GLB_REG_GPIO_52_ES3_LEN)-1)<<GLB_REG_GPIO_52_ES3_POS)
#define GLB_REG_GPIO_52_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_52_ES3_LEN)-1)<<GLB_REG_GPIO_52_ES3_POS))
#define GLB_REG_GPIO_52_ES4                                     GLB_REG_GPIO_52_ES4
#define GLB_REG_GPIO_52_ES4_POS                                 (8U)
#define GLB_REG_GPIO_52_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_52_ES4_MSK                                 (((1U<<GLB_REG_GPIO_52_ES4_LEN)-1)<<GLB_REG_GPIO_52_ES4_POS)
#define GLB_REG_GPIO_52_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_52_ES4_LEN)-1)<<GLB_REG_GPIO_52_ES4_POS))
#define GLB_REG_GPIO_53_IE                                      GLB_REG_GPIO_53_IE
#define GLB_REG_GPIO_53_IE_POS                                  (16U)
#define GLB_REG_GPIO_53_IE_LEN                                  (1U)
#define GLB_REG_GPIO_53_IE_MSK                                  (((1U<<GLB_REG_GPIO_53_IE_LEN)-1)<<GLB_REG_GPIO_53_IE_POS)
#define GLB_REG_GPIO_53_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_53_IE_LEN)-1)<<GLB_REG_GPIO_53_IE_POS))
#define GLB_REG_GPIO_53_SMT                                     GLB_REG_GPIO_53_SMT
#define GLB_REG_GPIO_53_SMT_POS                                 (17U)
#define GLB_REG_GPIO_53_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_53_SMT_MSK                                 (((1U<<GLB_REG_GPIO_53_SMT_LEN)-1)<<GLB_REG_GPIO_53_SMT_POS)
#define GLB_REG_GPIO_53_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_53_SMT_LEN)-1)<<GLB_REG_GPIO_53_SMT_POS))
#define GLB_REG_GPIO_53_DRV                                     GLB_REG_GPIO_53_DRV
#define GLB_REG_GPIO_53_DRV_POS                                 (18U)
#define GLB_REG_GPIO_53_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_53_DRV_MSK                                 (((1U<<GLB_REG_GPIO_53_DRV_LEN)-1)<<GLB_REG_GPIO_53_DRV_POS)
#define GLB_REG_GPIO_53_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_53_DRV_LEN)-1)<<GLB_REG_GPIO_53_DRV_POS))
#define GLB_REG_GPIO_53_PU                                      GLB_REG_GPIO_53_PU
#define GLB_REG_GPIO_53_PU_POS                                  (20U)
#define GLB_REG_GPIO_53_PU_LEN                                  (1U)
#define GLB_REG_GPIO_53_PU_MSK                                  (((1U<<GLB_REG_GPIO_53_PU_LEN)-1)<<GLB_REG_GPIO_53_PU_POS)
#define GLB_REG_GPIO_53_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_53_PU_LEN)-1)<<GLB_REG_GPIO_53_PU_POS))
#define GLB_REG_GPIO_53_PD                                      GLB_REG_GPIO_53_PD
#define GLB_REG_GPIO_53_PD_POS                                  (21U)
#define GLB_REG_GPIO_53_PD_LEN                                  (1U)
#define GLB_REG_GPIO_53_PD_MSK                                  (((1U<<GLB_REG_GPIO_53_PD_LEN)-1)<<GLB_REG_GPIO_53_PD_POS)
#define GLB_REG_GPIO_53_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_53_PD_LEN)-1)<<GLB_REG_GPIO_53_PD_POS))
#define GLB_REG_GPIO_53_ES2                                     GLB_REG_GPIO_53_ES2
#define GLB_REG_GPIO_53_ES2_POS                                 (22U)
#define GLB_REG_GPIO_53_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_53_ES2_MSK                                 (((1U<<GLB_REG_GPIO_53_ES2_LEN)-1)<<GLB_REG_GPIO_53_ES2_POS)
#define GLB_REG_GPIO_53_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_53_ES2_LEN)-1)<<GLB_REG_GPIO_53_ES2_POS))
#define GLB_REG_GPIO_53_ES3                                     GLB_REG_GPIO_53_ES3
#define GLB_REG_GPIO_53_ES3_POS                                 (23U)
#define GLB_REG_GPIO_53_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_53_ES3_MSK                                 (((1U<<GLB_REG_GPIO_53_ES3_LEN)-1)<<GLB_REG_GPIO_53_ES3_POS)
#define GLB_REG_GPIO_53_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_53_ES3_LEN)-1)<<GLB_REG_GPIO_53_ES3_POS))
#define GLB_REG_GPIO_53_ES4                                     GLB_REG_GPIO_53_ES4
#define GLB_REG_GPIO_53_ES4_POS                                 (24U)
#define GLB_REG_GPIO_53_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_53_ES4_MSK                                 (((1U<<GLB_REG_GPIO_53_ES4_LEN)-1)<<GLB_REG_GPIO_53_ES4_POS)
#define GLB_REG_GPIO_53_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_53_ES4_LEN)-1)<<GLB_REG_GPIO_53_ES4_POS))

/* 0x16C : GPIO_CFGCTL27 */
#define GLB_GPIO_CFGCTL27_OFFSET                                (0x16C)
#define GLB_REG_GPIO_54_IE                                      GLB_REG_GPIO_54_IE
#define GLB_REG_GPIO_54_IE_POS                                  (0U)
#define GLB_REG_GPIO_54_IE_LEN                                  (1U)
#define GLB_REG_GPIO_54_IE_MSK                                  (((1U<<GLB_REG_GPIO_54_IE_LEN)-1)<<GLB_REG_GPIO_54_IE_POS)
#define GLB_REG_GPIO_54_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_54_IE_LEN)-1)<<GLB_REG_GPIO_54_IE_POS))
#define GLB_REG_GPIO_54_SMT                                     GLB_REG_GPIO_54_SMT
#define GLB_REG_GPIO_54_SMT_POS                                 (1U)
#define GLB_REG_GPIO_54_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_54_SMT_MSK                                 (((1U<<GLB_REG_GPIO_54_SMT_LEN)-1)<<GLB_REG_GPIO_54_SMT_POS)
#define GLB_REG_GPIO_54_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_54_SMT_LEN)-1)<<GLB_REG_GPIO_54_SMT_POS))
#define GLB_REG_GPIO_54_DRV                                     GLB_REG_GPIO_54_DRV
#define GLB_REG_GPIO_54_DRV_POS                                 (2U)
#define GLB_REG_GPIO_54_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_54_DRV_MSK                                 (((1U<<GLB_REG_GPIO_54_DRV_LEN)-1)<<GLB_REG_GPIO_54_DRV_POS)
#define GLB_REG_GPIO_54_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_54_DRV_LEN)-1)<<GLB_REG_GPIO_54_DRV_POS))
#define GLB_REG_GPIO_54_PU                                      GLB_REG_GPIO_54_PU
#define GLB_REG_GPIO_54_PU_POS                                  (4U)
#define GLB_REG_GPIO_54_PU_LEN                                  (1U)
#define GLB_REG_GPIO_54_PU_MSK                                  (((1U<<GLB_REG_GPIO_54_PU_LEN)-1)<<GLB_REG_GPIO_54_PU_POS)
#define GLB_REG_GPIO_54_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_54_PU_LEN)-1)<<GLB_REG_GPIO_54_PU_POS))
#define GLB_REG_GPIO_54_PD                                      GLB_REG_GPIO_54_PD
#define GLB_REG_GPIO_54_PD_POS                                  (5U)
#define GLB_REG_GPIO_54_PD_LEN                                  (1U)
#define GLB_REG_GPIO_54_PD_MSK                                  (((1U<<GLB_REG_GPIO_54_PD_LEN)-1)<<GLB_REG_GPIO_54_PD_POS)
#define GLB_REG_GPIO_54_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_54_PD_LEN)-1)<<GLB_REG_GPIO_54_PD_POS))
#define GLB_REG_GPIO_54_ES2                                     GLB_REG_GPIO_54_ES2
#define GLB_REG_GPIO_54_ES2_POS                                 (6U)
#define GLB_REG_GPIO_54_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_54_ES2_MSK                                 (((1U<<GLB_REG_GPIO_54_ES2_LEN)-1)<<GLB_REG_GPIO_54_ES2_POS)
#define GLB_REG_GPIO_54_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_54_ES2_LEN)-1)<<GLB_REG_GPIO_54_ES2_POS))
#define GLB_REG_GPIO_54_ES3                                     GLB_REG_GPIO_54_ES3
#define GLB_REG_GPIO_54_ES3_POS                                 (7U)
#define GLB_REG_GPIO_54_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_54_ES3_MSK                                 (((1U<<GLB_REG_GPIO_54_ES3_LEN)-1)<<GLB_REG_GPIO_54_ES3_POS)
#define GLB_REG_GPIO_54_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_54_ES3_LEN)-1)<<GLB_REG_GPIO_54_ES3_POS))
#define GLB_REG_GPIO_54_ES4                                     GLB_REG_GPIO_54_ES4
#define GLB_REG_GPIO_54_ES4_POS                                 (8U)
#define GLB_REG_GPIO_54_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_54_ES4_MSK                                 (((1U<<GLB_REG_GPIO_54_ES4_LEN)-1)<<GLB_REG_GPIO_54_ES4_POS)
#define GLB_REG_GPIO_54_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_54_ES4_LEN)-1)<<GLB_REG_GPIO_54_ES4_POS))
#define GLB_REG_GPIO_55_IE                                      GLB_REG_GPIO_55_IE
#define GLB_REG_GPIO_55_IE_POS                                  (16U)
#define GLB_REG_GPIO_55_IE_LEN                                  (1U)
#define GLB_REG_GPIO_55_IE_MSK                                  (((1U<<GLB_REG_GPIO_55_IE_LEN)-1)<<GLB_REG_GPIO_55_IE_POS)
#define GLB_REG_GPIO_55_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_55_IE_LEN)-1)<<GLB_REG_GPIO_55_IE_POS))
#define GLB_REG_GPIO_55_SMT                                     GLB_REG_GPIO_55_SMT
#define GLB_REG_GPIO_55_SMT_POS                                 (17U)
#define GLB_REG_GPIO_55_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_55_SMT_MSK                                 (((1U<<GLB_REG_GPIO_55_SMT_LEN)-1)<<GLB_REG_GPIO_55_SMT_POS)
#define GLB_REG_GPIO_55_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_55_SMT_LEN)-1)<<GLB_REG_GPIO_55_SMT_POS))
#define GLB_REG_GPIO_55_DRV                                     GLB_REG_GPIO_55_DRV
#define GLB_REG_GPIO_55_DRV_POS                                 (18U)
#define GLB_REG_GPIO_55_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_55_DRV_MSK                                 (((1U<<GLB_REG_GPIO_55_DRV_LEN)-1)<<GLB_REG_GPIO_55_DRV_POS)
#define GLB_REG_GPIO_55_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_55_DRV_LEN)-1)<<GLB_REG_GPIO_55_DRV_POS))
#define GLB_REG_GPIO_55_PU                                      GLB_REG_GPIO_55_PU
#define GLB_REG_GPIO_55_PU_POS                                  (20U)
#define GLB_REG_GPIO_55_PU_LEN                                  (1U)
#define GLB_REG_GPIO_55_PU_MSK                                  (((1U<<GLB_REG_GPIO_55_PU_LEN)-1)<<GLB_REG_GPIO_55_PU_POS)
#define GLB_REG_GPIO_55_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_55_PU_LEN)-1)<<GLB_REG_GPIO_55_PU_POS))
#define GLB_REG_GPIO_55_PD                                      GLB_REG_GPIO_55_PD
#define GLB_REG_GPIO_55_PD_POS                                  (21U)
#define GLB_REG_GPIO_55_PD_LEN                                  (1U)
#define GLB_REG_GPIO_55_PD_MSK                                  (((1U<<GLB_REG_GPIO_55_PD_LEN)-1)<<GLB_REG_GPIO_55_PD_POS)
#define GLB_REG_GPIO_55_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_55_PD_LEN)-1)<<GLB_REG_GPIO_55_PD_POS))
#define GLB_REG_GPIO_55_ES2                                     GLB_REG_GPIO_55_ES2
#define GLB_REG_GPIO_55_ES2_POS                                 (22U)
#define GLB_REG_GPIO_55_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_55_ES2_MSK                                 (((1U<<GLB_REG_GPIO_55_ES2_LEN)-1)<<GLB_REG_GPIO_55_ES2_POS)
#define GLB_REG_GPIO_55_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_55_ES2_LEN)-1)<<GLB_REG_GPIO_55_ES2_POS))
#define GLB_REG_GPIO_55_ES3                                     GLB_REG_GPIO_55_ES3
#define GLB_REG_GPIO_55_ES3_POS                                 (23U)
#define GLB_REG_GPIO_55_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_55_ES3_MSK                                 (((1U<<GLB_REG_GPIO_55_ES3_LEN)-1)<<GLB_REG_GPIO_55_ES3_POS)
#define GLB_REG_GPIO_55_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_55_ES3_LEN)-1)<<GLB_REG_GPIO_55_ES3_POS))
#define GLB_REG_GPIO_55_ES4                                     GLB_REG_GPIO_55_ES4
#define GLB_REG_GPIO_55_ES4_POS                                 (24U)
#define GLB_REG_GPIO_55_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_55_ES4_MSK                                 (((1U<<GLB_REG_GPIO_55_ES4_LEN)-1)<<GLB_REG_GPIO_55_ES4_POS)
#define GLB_REG_GPIO_55_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_55_ES4_LEN)-1)<<GLB_REG_GPIO_55_ES4_POS))

/* 0x170 : GPIO_CFGCTL28 */
#define GLB_GPIO_CFGCTL28_OFFSET                                (0x170)
#define GLB_REG_GPIO_56_IE                                      GLB_REG_GPIO_56_IE
#define GLB_REG_GPIO_56_IE_POS                                  (0U)
#define GLB_REG_GPIO_56_IE_LEN                                  (1U)
#define GLB_REG_GPIO_56_IE_MSK                                  (((1U<<GLB_REG_GPIO_56_IE_LEN)-1)<<GLB_REG_GPIO_56_IE_POS)
#define GLB_REG_GPIO_56_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_56_IE_LEN)-1)<<GLB_REG_GPIO_56_IE_POS))
#define GLB_REG_GPIO_56_SMT                                     GLB_REG_GPIO_56_SMT
#define GLB_REG_GPIO_56_SMT_POS                                 (1U)
#define GLB_REG_GPIO_56_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_56_SMT_MSK                                 (((1U<<GLB_REG_GPIO_56_SMT_LEN)-1)<<GLB_REG_GPIO_56_SMT_POS)
#define GLB_REG_GPIO_56_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_56_SMT_LEN)-1)<<GLB_REG_GPIO_56_SMT_POS))
#define GLB_REG_GPIO_56_DRV                                     GLB_REG_GPIO_56_DRV
#define GLB_REG_GPIO_56_DRV_POS                                 (2U)
#define GLB_REG_GPIO_56_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_56_DRV_MSK                                 (((1U<<GLB_REG_GPIO_56_DRV_LEN)-1)<<GLB_REG_GPIO_56_DRV_POS)
#define GLB_REG_GPIO_56_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_56_DRV_LEN)-1)<<GLB_REG_GPIO_56_DRV_POS))
#define GLB_REG_GPIO_56_PU                                      GLB_REG_GPIO_56_PU
#define GLB_REG_GPIO_56_PU_POS                                  (4U)
#define GLB_REG_GPIO_56_PU_LEN                                  (1U)
#define GLB_REG_GPIO_56_PU_MSK                                  (((1U<<GLB_REG_GPIO_56_PU_LEN)-1)<<GLB_REG_GPIO_56_PU_POS)
#define GLB_REG_GPIO_56_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_56_PU_LEN)-1)<<GLB_REG_GPIO_56_PU_POS))
#define GLB_REG_GPIO_56_PD                                      GLB_REG_GPIO_56_PD
#define GLB_REG_GPIO_56_PD_POS                                  (5U)
#define GLB_REG_GPIO_56_PD_LEN                                  (1U)
#define GLB_REG_GPIO_56_PD_MSK                                  (((1U<<GLB_REG_GPIO_56_PD_LEN)-1)<<GLB_REG_GPIO_56_PD_POS)
#define GLB_REG_GPIO_56_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_56_PD_LEN)-1)<<GLB_REG_GPIO_56_PD_POS))
#define GLB_REG_GPIO_56_ES2                                     GLB_REG_GPIO_56_ES2
#define GLB_REG_GPIO_56_ES2_POS                                 (6U)
#define GLB_REG_GPIO_56_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_56_ES2_MSK                                 (((1U<<GLB_REG_GPIO_56_ES2_LEN)-1)<<GLB_REG_GPIO_56_ES2_POS)
#define GLB_REG_GPIO_56_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_56_ES2_LEN)-1)<<GLB_REG_GPIO_56_ES2_POS))
#define GLB_REG_GPIO_56_ES3                                     GLB_REG_GPIO_56_ES3
#define GLB_REG_GPIO_56_ES3_POS                                 (7U)
#define GLB_REG_GPIO_56_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_56_ES3_MSK                                 (((1U<<GLB_REG_GPIO_56_ES3_LEN)-1)<<GLB_REG_GPIO_56_ES3_POS)
#define GLB_REG_GPIO_56_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_56_ES3_LEN)-1)<<GLB_REG_GPIO_56_ES3_POS))
#define GLB_REG_GPIO_56_ES4                                     GLB_REG_GPIO_56_ES4
#define GLB_REG_GPIO_56_ES4_POS                                 (8U)
#define GLB_REG_GPIO_56_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_56_ES4_MSK                                 (((1U<<GLB_REG_GPIO_56_ES4_LEN)-1)<<GLB_REG_GPIO_56_ES4_POS)
#define GLB_REG_GPIO_56_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_56_ES4_LEN)-1)<<GLB_REG_GPIO_56_ES4_POS))
#define GLB_REG_GPIO_57_IE                                      GLB_REG_GPIO_57_IE
#define GLB_REG_GPIO_57_IE_POS                                  (16U)
#define GLB_REG_GPIO_57_IE_LEN                                  (1U)
#define GLB_REG_GPIO_57_IE_MSK                                  (((1U<<GLB_REG_GPIO_57_IE_LEN)-1)<<GLB_REG_GPIO_57_IE_POS)
#define GLB_REG_GPIO_57_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_57_IE_LEN)-1)<<GLB_REG_GPIO_57_IE_POS))
#define GLB_REG_GPIO_57_SMT                                     GLB_REG_GPIO_57_SMT
#define GLB_REG_GPIO_57_SMT_POS                                 (17U)
#define GLB_REG_GPIO_57_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_57_SMT_MSK                                 (((1U<<GLB_REG_GPIO_57_SMT_LEN)-1)<<GLB_REG_GPIO_57_SMT_POS)
#define GLB_REG_GPIO_57_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_57_SMT_LEN)-1)<<GLB_REG_GPIO_57_SMT_POS))
#define GLB_REG_GPIO_57_DRV                                     GLB_REG_GPIO_57_DRV
#define GLB_REG_GPIO_57_DRV_POS                                 (18U)
#define GLB_REG_GPIO_57_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_57_DRV_MSK                                 (((1U<<GLB_REG_GPIO_57_DRV_LEN)-1)<<GLB_REG_GPIO_57_DRV_POS)
#define GLB_REG_GPIO_57_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_57_DRV_LEN)-1)<<GLB_REG_GPIO_57_DRV_POS))
#define GLB_REG_GPIO_57_PU                                      GLB_REG_GPIO_57_PU
#define GLB_REG_GPIO_57_PU_POS                                  (20U)
#define GLB_REG_GPIO_57_PU_LEN                                  (1U)
#define GLB_REG_GPIO_57_PU_MSK                                  (((1U<<GLB_REG_GPIO_57_PU_LEN)-1)<<GLB_REG_GPIO_57_PU_POS)
#define GLB_REG_GPIO_57_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_57_PU_LEN)-1)<<GLB_REG_GPIO_57_PU_POS))
#define GLB_REG_GPIO_57_PD                                      GLB_REG_GPIO_57_PD
#define GLB_REG_GPIO_57_PD_POS                                  (21U)
#define GLB_REG_GPIO_57_PD_LEN                                  (1U)
#define GLB_REG_GPIO_57_PD_MSK                                  (((1U<<GLB_REG_GPIO_57_PD_LEN)-1)<<GLB_REG_GPIO_57_PD_POS)
#define GLB_REG_GPIO_57_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_57_PD_LEN)-1)<<GLB_REG_GPIO_57_PD_POS))
#define GLB_REG_GPIO_57_ES2                                     GLB_REG_GPIO_57_ES2
#define GLB_REG_GPIO_57_ES2_POS                                 (22U)
#define GLB_REG_GPIO_57_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_57_ES2_MSK                                 (((1U<<GLB_REG_GPIO_57_ES2_LEN)-1)<<GLB_REG_GPIO_57_ES2_POS)
#define GLB_REG_GPIO_57_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_57_ES2_LEN)-1)<<GLB_REG_GPIO_57_ES2_POS))
#define GLB_REG_GPIO_57_ES3                                     GLB_REG_GPIO_57_ES3
#define GLB_REG_GPIO_57_ES3_POS                                 (23U)
#define GLB_REG_GPIO_57_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_57_ES3_MSK                                 (((1U<<GLB_REG_GPIO_57_ES3_LEN)-1)<<GLB_REG_GPIO_57_ES3_POS)
#define GLB_REG_GPIO_57_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_57_ES3_LEN)-1)<<GLB_REG_GPIO_57_ES3_POS))
#define GLB_REG_GPIO_57_ES4                                     GLB_REG_GPIO_57_ES4
#define GLB_REG_GPIO_57_ES4_POS                                 (24U)
#define GLB_REG_GPIO_57_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_57_ES4_MSK                                 (((1U<<GLB_REG_GPIO_57_ES4_LEN)-1)<<GLB_REG_GPIO_57_ES4_POS)
#define GLB_REG_GPIO_57_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_57_ES4_LEN)-1)<<GLB_REG_GPIO_57_ES4_POS))

/* 0x174 : GPIO_CFGCTL29 */
#define GLB_GPIO_CFGCTL29_OFFSET                                (0x174)
#define GLB_REG_GPIO_58_IE                                      GLB_REG_GPIO_58_IE
#define GLB_REG_GPIO_58_IE_POS                                  (0U)
#define GLB_REG_GPIO_58_IE_LEN                                  (1U)
#define GLB_REG_GPIO_58_IE_MSK                                  (((1U<<GLB_REG_GPIO_58_IE_LEN)-1)<<GLB_REG_GPIO_58_IE_POS)
#define GLB_REG_GPIO_58_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_58_IE_LEN)-1)<<GLB_REG_GPIO_58_IE_POS))
#define GLB_REG_GPIO_58_SMT                                     GLB_REG_GPIO_58_SMT
#define GLB_REG_GPIO_58_SMT_POS                                 (1U)
#define GLB_REG_GPIO_58_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_58_SMT_MSK                                 (((1U<<GLB_REG_GPIO_58_SMT_LEN)-1)<<GLB_REG_GPIO_58_SMT_POS)
#define GLB_REG_GPIO_58_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_58_SMT_LEN)-1)<<GLB_REG_GPIO_58_SMT_POS))
#define GLB_REG_GPIO_58_DRV                                     GLB_REG_GPIO_58_DRV
#define GLB_REG_GPIO_58_DRV_POS                                 (2U)
#define GLB_REG_GPIO_58_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_58_DRV_MSK                                 (((1U<<GLB_REG_GPIO_58_DRV_LEN)-1)<<GLB_REG_GPIO_58_DRV_POS)
#define GLB_REG_GPIO_58_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_58_DRV_LEN)-1)<<GLB_REG_GPIO_58_DRV_POS))
#define GLB_REG_GPIO_58_PU                                      GLB_REG_GPIO_58_PU
#define GLB_REG_GPIO_58_PU_POS                                  (4U)
#define GLB_REG_GPIO_58_PU_LEN                                  (1U)
#define GLB_REG_GPIO_58_PU_MSK                                  (((1U<<GLB_REG_GPIO_58_PU_LEN)-1)<<GLB_REG_GPIO_58_PU_POS)
#define GLB_REG_GPIO_58_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_58_PU_LEN)-1)<<GLB_REG_GPIO_58_PU_POS))
#define GLB_REG_GPIO_58_PD                                      GLB_REG_GPIO_58_PD
#define GLB_REG_GPIO_58_PD_POS                                  (5U)
#define GLB_REG_GPIO_58_PD_LEN                                  (1U)
#define GLB_REG_GPIO_58_PD_MSK                                  (((1U<<GLB_REG_GPIO_58_PD_LEN)-1)<<GLB_REG_GPIO_58_PD_POS)
#define GLB_REG_GPIO_58_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_58_PD_LEN)-1)<<GLB_REG_GPIO_58_PD_POS))
#define GLB_REG_GPIO_58_ES2                                     GLB_REG_GPIO_58_ES2
#define GLB_REG_GPIO_58_ES2_POS                                 (6U)
#define GLB_REG_GPIO_58_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_58_ES2_MSK                                 (((1U<<GLB_REG_GPIO_58_ES2_LEN)-1)<<GLB_REG_GPIO_58_ES2_POS)
#define GLB_REG_GPIO_58_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_58_ES2_LEN)-1)<<GLB_REG_GPIO_58_ES2_POS))
#define GLB_REG_GPIO_58_ES3                                     GLB_REG_GPIO_58_ES3
#define GLB_REG_GPIO_58_ES3_POS                                 (7U)
#define GLB_REG_GPIO_58_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_58_ES3_MSK                                 (((1U<<GLB_REG_GPIO_58_ES3_LEN)-1)<<GLB_REG_GPIO_58_ES3_POS)
#define GLB_REG_GPIO_58_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_58_ES3_LEN)-1)<<GLB_REG_GPIO_58_ES3_POS))
#define GLB_REG_GPIO_58_ES4                                     GLB_REG_GPIO_58_ES4
#define GLB_REG_GPIO_58_ES4_POS                                 (8U)
#define GLB_REG_GPIO_58_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_58_ES4_MSK                                 (((1U<<GLB_REG_GPIO_58_ES4_LEN)-1)<<GLB_REG_GPIO_58_ES4_POS)
#define GLB_REG_GPIO_58_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_58_ES4_LEN)-1)<<GLB_REG_GPIO_58_ES4_POS))
#define GLB_REG_GPIO_59_IE                                      GLB_REG_GPIO_59_IE
#define GLB_REG_GPIO_59_IE_POS                                  (16U)
#define GLB_REG_GPIO_59_IE_LEN                                  (1U)
#define GLB_REG_GPIO_59_IE_MSK                                  (((1U<<GLB_REG_GPIO_59_IE_LEN)-1)<<GLB_REG_GPIO_59_IE_POS)
#define GLB_REG_GPIO_59_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_59_IE_LEN)-1)<<GLB_REG_GPIO_59_IE_POS))
#define GLB_REG_GPIO_59_SMT                                     GLB_REG_GPIO_59_SMT
#define GLB_REG_GPIO_59_SMT_POS                                 (17U)
#define GLB_REG_GPIO_59_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_59_SMT_MSK                                 (((1U<<GLB_REG_GPIO_59_SMT_LEN)-1)<<GLB_REG_GPIO_59_SMT_POS)
#define GLB_REG_GPIO_59_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_59_SMT_LEN)-1)<<GLB_REG_GPIO_59_SMT_POS))
#define GLB_REG_GPIO_59_DRV                                     GLB_REG_GPIO_59_DRV
#define GLB_REG_GPIO_59_DRV_POS                                 (18U)
#define GLB_REG_GPIO_59_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_59_DRV_MSK                                 (((1U<<GLB_REG_GPIO_59_DRV_LEN)-1)<<GLB_REG_GPIO_59_DRV_POS)
#define GLB_REG_GPIO_59_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_59_DRV_LEN)-1)<<GLB_REG_GPIO_59_DRV_POS))
#define GLB_REG_GPIO_59_PU                                      GLB_REG_GPIO_59_PU
#define GLB_REG_GPIO_59_PU_POS                                  (20U)
#define GLB_REG_GPIO_59_PU_LEN                                  (1U)
#define GLB_REG_GPIO_59_PU_MSK                                  (((1U<<GLB_REG_GPIO_59_PU_LEN)-1)<<GLB_REG_GPIO_59_PU_POS)
#define GLB_REG_GPIO_59_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_59_PU_LEN)-1)<<GLB_REG_GPIO_59_PU_POS))
#define GLB_REG_GPIO_59_PD                                      GLB_REG_GPIO_59_PD
#define GLB_REG_GPIO_59_PD_POS                                  (21U)
#define GLB_REG_GPIO_59_PD_LEN                                  (1U)
#define GLB_REG_GPIO_59_PD_MSK                                  (((1U<<GLB_REG_GPIO_59_PD_LEN)-1)<<GLB_REG_GPIO_59_PD_POS)
#define GLB_REG_GPIO_59_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_59_PD_LEN)-1)<<GLB_REG_GPIO_59_PD_POS))
#define GLB_REG_GPIO_59_ES2                                     GLB_REG_GPIO_59_ES2
#define GLB_REG_GPIO_59_ES2_POS                                 (22U)
#define GLB_REG_GPIO_59_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_59_ES2_MSK                                 (((1U<<GLB_REG_GPIO_59_ES2_LEN)-1)<<GLB_REG_GPIO_59_ES2_POS)
#define GLB_REG_GPIO_59_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_59_ES2_LEN)-1)<<GLB_REG_GPIO_59_ES2_POS))
#define GLB_REG_GPIO_59_ES3                                     GLB_REG_GPIO_59_ES3
#define GLB_REG_GPIO_59_ES3_POS                                 (23U)
#define GLB_REG_GPIO_59_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_59_ES3_MSK                                 (((1U<<GLB_REG_GPIO_59_ES3_LEN)-1)<<GLB_REG_GPIO_59_ES3_POS)
#define GLB_REG_GPIO_59_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_59_ES3_LEN)-1)<<GLB_REG_GPIO_59_ES3_POS))
#define GLB_REG_GPIO_59_ES4                                     GLB_REG_GPIO_59_ES4
#define GLB_REG_GPIO_59_ES4_POS                                 (24U)
#define GLB_REG_GPIO_59_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_59_ES4_MSK                                 (((1U<<GLB_REG_GPIO_59_ES4_LEN)-1)<<GLB_REG_GPIO_59_ES4_POS)
#define GLB_REG_GPIO_59_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_59_ES4_LEN)-1)<<GLB_REG_GPIO_59_ES4_POS))

/* 0x178 : GPIO_CFGCTL30 */
#define GLB_GPIO_CFGCTL30_OFFSET                                (0x178)
#define GLB_REG_GPIO_60_IE                                      GLB_REG_GPIO_60_IE
#define GLB_REG_GPIO_60_IE_POS                                  (0U)
#define GLB_REG_GPIO_60_IE_LEN                                  (1U)
#define GLB_REG_GPIO_60_IE_MSK                                  (((1U<<GLB_REG_GPIO_60_IE_LEN)-1)<<GLB_REG_GPIO_60_IE_POS)
#define GLB_REG_GPIO_60_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_60_IE_LEN)-1)<<GLB_REG_GPIO_60_IE_POS))
#define GLB_REG_GPIO_60_SMT                                     GLB_REG_GPIO_60_SMT
#define GLB_REG_GPIO_60_SMT_POS                                 (1U)
#define GLB_REG_GPIO_60_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_60_SMT_MSK                                 (((1U<<GLB_REG_GPIO_60_SMT_LEN)-1)<<GLB_REG_GPIO_60_SMT_POS)
#define GLB_REG_GPIO_60_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_60_SMT_LEN)-1)<<GLB_REG_GPIO_60_SMT_POS))
#define GLB_REG_GPIO_60_DRV                                     GLB_REG_GPIO_60_DRV
#define GLB_REG_GPIO_60_DRV_POS                                 (2U)
#define GLB_REG_GPIO_60_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_60_DRV_MSK                                 (((1U<<GLB_REG_GPIO_60_DRV_LEN)-1)<<GLB_REG_GPIO_60_DRV_POS)
#define GLB_REG_GPIO_60_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_60_DRV_LEN)-1)<<GLB_REG_GPIO_60_DRV_POS))
#define GLB_REG_GPIO_60_PU                                      GLB_REG_GPIO_60_PU
#define GLB_REG_GPIO_60_PU_POS                                  (4U)
#define GLB_REG_GPIO_60_PU_LEN                                  (1U)
#define GLB_REG_GPIO_60_PU_MSK                                  (((1U<<GLB_REG_GPIO_60_PU_LEN)-1)<<GLB_REG_GPIO_60_PU_POS)
#define GLB_REG_GPIO_60_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_60_PU_LEN)-1)<<GLB_REG_GPIO_60_PU_POS))
#define GLB_REG_GPIO_60_PD                                      GLB_REG_GPIO_60_PD
#define GLB_REG_GPIO_60_PD_POS                                  (5U)
#define GLB_REG_GPIO_60_PD_LEN                                  (1U)
#define GLB_REG_GPIO_60_PD_MSK                                  (((1U<<GLB_REG_GPIO_60_PD_LEN)-1)<<GLB_REG_GPIO_60_PD_POS)
#define GLB_REG_GPIO_60_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_60_PD_LEN)-1)<<GLB_REG_GPIO_60_PD_POS))
#define GLB_REG_GPIO_60_ES2                                     GLB_REG_GPIO_60_ES2
#define GLB_REG_GPIO_60_ES2_POS                                 (6U)
#define GLB_REG_GPIO_60_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_60_ES2_MSK                                 (((1U<<GLB_REG_GPIO_60_ES2_LEN)-1)<<GLB_REG_GPIO_60_ES2_POS)
#define GLB_REG_GPIO_60_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_60_ES2_LEN)-1)<<GLB_REG_GPIO_60_ES2_POS))
#define GLB_REG_GPIO_60_ES3                                     GLB_REG_GPIO_60_ES3
#define GLB_REG_GPIO_60_ES3_POS                                 (7U)
#define GLB_REG_GPIO_60_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_60_ES3_MSK                                 (((1U<<GLB_REG_GPIO_60_ES3_LEN)-1)<<GLB_REG_GPIO_60_ES3_POS)
#define GLB_REG_GPIO_60_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_60_ES3_LEN)-1)<<GLB_REG_GPIO_60_ES3_POS))
#define GLB_REG_GPIO_60_ES4                                     GLB_REG_GPIO_60_ES4
#define GLB_REG_GPIO_60_ES4_POS                                 (8U)
#define GLB_REG_GPIO_60_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_60_ES4_MSK                                 (((1U<<GLB_REG_GPIO_60_ES4_LEN)-1)<<GLB_REG_GPIO_60_ES4_POS)
#define GLB_REG_GPIO_60_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_60_ES4_LEN)-1)<<GLB_REG_GPIO_60_ES4_POS))
#define GLB_REG_GPIO_61_IE                                      GLB_REG_GPIO_61_IE
#define GLB_REG_GPIO_61_IE_POS                                  (16U)
#define GLB_REG_GPIO_61_IE_LEN                                  (1U)
#define GLB_REG_GPIO_61_IE_MSK                                  (((1U<<GLB_REG_GPIO_61_IE_LEN)-1)<<GLB_REG_GPIO_61_IE_POS)
#define GLB_REG_GPIO_61_IE_UMSK                                 (~(((1U<<GLB_REG_GPIO_61_IE_LEN)-1)<<GLB_REG_GPIO_61_IE_POS))
#define GLB_REG_GPIO_61_SMT                                     GLB_REG_GPIO_61_SMT
#define GLB_REG_GPIO_61_SMT_POS                                 (17U)
#define GLB_REG_GPIO_61_SMT_LEN                                 (1U)
#define GLB_REG_GPIO_61_SMT_MSK                                 (((1U<<GLB_REG_GPIO_61_SMT_LEN)-1)<<GLB_REG_GPIO_61_SMT_POS)
#define GLB_REG_GPIO_61_SMT_UMSK                                (~(((1U<<GLB_REG_GPIO_61_SMT_LEN)-1)<<GLB_REG_GPIO_61_SMT_POS))
#define GLB_REG_GPIO_61_DRV                                     GLB_REG_GPIO_61_DRV
#define GLB_REG_GPIO_61_DRV_POS                                 (18U)
#define GLB_REG_GPIO_61_DRV_LEN                                 (2U)
#define GLB_REG_GPIO_61_DRV_MSK                                 (((1U<<GLB_REG_GPIO_61_DRV_LEN)-1)<<GLB_REG_GPIO_61_DRV_POS)
#define GLB_REG_GPIO_61_DRV_UMSK                                (~(((1U<<GLB_REG_GPIO_61_DRV_LEN)-1)<<GLB_REG_GPIO_61_DRV_POS))
#define GLB_REG_GPIO_61_PU                                      GLB_REG_GPIO_61_PU
#define GLB_REG_GPIO_61_PU_POS                                  (20U)
#define GLB_REG_GPIO_61_PU_LEN                                  (1U)
#define GLB_REG_GPIO_61_PU_MSK                                  (((1U<<GLB_REG_GPIO_61_PU_LEN)-1)<<GLB_REG_GPIO_61_PU_POS)
#define GLB_REG_GPIO_61_PU_UMSK                                 (~(((1U<<GLB_REG_GPIO_61_PU_LEN)-1)<<GLB_REG_GPIO_61_PU_POS))
#define GLB_REG_GPIO_61_PD                                      GLB_REG_GPIO_61_PD
#define GLB_REG_GPIO_61_PD_POS                                  (21U)
#define GLB_REG_GPIO_61_PD_LEN                                  (1U)
#define GLB_REG_GPIO_61_PD_MSK                                  (((1U<<GLB_REG_GPIO_61_PD_LEN)-1)<<GLB_REG_GPIO_61_PD_POS)
#define GLB_REG_GPIO_61_PD_UMSK                                 (~(((1U<<GLB_REG_GPIO_61_PD_LEN)-1)<<GLB_REG_GPIO_61_PD_POS))
#define GLB_REG_GPIO_61_ES2                                     GLB_REG_GPIO_61_ES2
#define GLB_REG_GPIO_61_ES2_POS                                 (22U)
#define GLB_REG_GPIO_61_ES2_LEN                                 (1U)
#define GLB_REG_GPIO_61_ES2_MSK                                 (((1U<<GLB_REG_GPIO_61_ES2_LEN)-1)<<GLB_REG_GPIO_61_ES2_POS)
#define GLB_REG_GPIO_61_ES2_UMSK                                (~(((1U<<GLB_REG_GPIO_61_ES2_LEN)-1)<<GLB_REG_GPIO_61_ES2_POS))
#define GLB_REG_GPIO_61_ES3                                     GLB_REG_GPIO_61_ES3
#define GLB_REG_GPIO_61_ES3_POS                                 (23U)
#define GLB_REG_GPIO_61_ES3_LEN                                 (1U)
#define GLB_REG_GPIO_61_ES3_MSK                                 (((1U<<GLB_REG_GPIO_61_ES3_LEN)-1)<<GLB_REG_GPIO_61_ES3_POS)
#define GLB_REG_GPIO_61_ES3_UMSK                                (~(((1U<<GLB_REG_GPIO_61_ES3_LEN)-1)<<GLB_REG_GPIO_61_ES3_POS))
#define GLB_REG_GPIO_61_ES4                                     GLB_REG_GPIO_61_ES4
#define GLB_REG_GPIO_61_ES4_POS                                 (24U)
#define GLB_REG_GPIO_61_ES4_LEN                                 (1U)
#define GLB_REG_GPIO_61_ES4_MSK                                 (((1U<<GLB_REG_GPIO_61_ES4_LEN)-1)<<GLB_REG_GPIO_61_ES4_POS)
#define GLB_REG_GPIO_61_ES4_UMSK                                (~(((1U<<GLB_REG_GPIO_61_ES4_LEN)-1)<<GLB_REG_GPIO_61_ES4_POS))

/* 0x180 : GPIO_CFGCTL31 */
#define GLB_GPIO_CFGCTL31_OFFSET                                (0x180)
#define GLB_REG_GPIO_0_I                                        GLB_REG_GPIO_0_I
#define GLB_REG_GPIO_0_I_POS                                    (0U)
#define GLB_REG_GPIO_0_I_LEN                                    (1U)
#define GLB_REG_GPIO_0_I_MSK                                    (((1U<<GLB_REG_GPIO_0_I_LEN)-1)<<GLB_REG_GPIO_0_I_POS)
#define GLB_REG_GPIO_0_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_0_I_LEN)-1)<<GLB_REG_GPIO_0_I_POS))
#define GLB_REG_GPIO_1_I                                        GLB_REG_GPIO_1_I
#define GLB_REG_GPIO_1_I_POS                                    (1U)
#define GLB_REG_GPIO_1_I_LEN                                    (1U)
#define GLB_REG_GPIO_1_I_MSK                                    (((1U<<GLB_REG_GPIO_1_I_LEN)-1)<<GLB_REG_GPIO_1_I_POS)
#define GLB_REG_GPIO_1_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_1_I_LEN)-1)<<GLB_REG_GPIO_1_I_POS))
#define GLB_REG_GPIO_2_I                                        GLB_REG_GPIO_2_I
#define GLB_REG_GPIO_2_I_POS                                    (2U)
#define GLB_REG_GPIO_2_I_LEN                                    (1U)
#define GLB_REG_GPIO_2_I_MSK                                    (((1U<<GLB_REG_GPIO_2_I_LEN)-1)<<GLB_REG_GPIO_2_I_POS)
#define GLB_REG_GPIO_2_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_2_I_LEN)-1)<<GLB_REG_GPIO_2_I_POS))
#define GLB_REG_GPIO_3_I                                        GLB_REG_GPIO_3_I
#define GLB_REG_GPIO_3_I_POS                                    (3U)
#define GLB_REG_GPIO_3_I_LEN                                    (1U)
#define GLB_REG_GPIO_3_I_MSK                                    (((1U<<GLB_REG_GPIO_3_I_LEN)-1)<<GLB_REG_GPIO_3_I_POS)
#define GLB_REG_GPIO_3_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_3_I_LEN)-1)<<GLB_REG_GPIO_3_I_POS))
#define GLB_REG_GPIO_4_I                                        GLB_REG_GPIO_4_I
#define GLB_REG_GPIO_4_I_POS                                    (4U)
#define GLB_REG_GPIO_4_I_LEN                                    (1U)
#define GLB_REG_GPIO_4_I_MSK                                    (((1U<<GLB_REG_GPIO_4_I_LEN)-1)<<GLB_REG_GPIO_4_I_POS)
#define GLB_REG_GPIO_4_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_4_I_LEN)-1)<<GLB_REG_GPIO_4_I_POS))
#define GLB_REG_GPIO_5_I                                        GLB_REG_GPIO_5_I
#define GLB_REG_GPIO_5_I_POS                                    (5U)
#define GLB_REG_GPIO_5_I_LEN                                    (1U)
#define GLB_REG_GPIO_5_I_MSK                                    (((1U<<GLB_REG_GPIO_5_I_LEN)-1)<<GLB_REG_GPIO_5_I_POS)
#define GLB_REG_GPIO_5_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_5_I_LEN)-1)<<GLB_REG_GPIO_5_I_POS))
#define GLB_REG_GPIO_6_I                                        GLB_REG_GPIO_6_I
#define GLB_REG_GPIO_6_I_POS                                    (6U)
#define GLB_REG_GPIO_6_I_LEN                                    (1U)
#define GLB_REG_GPIO_6_I_MSK                                    (((1U<<GLB_REG_GPIO_6_I_LEN)-1)<<GLB_REG_GPIO_6_I_POS)
#define GLB_REG_GPIO_6_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_6_I_LEN)-1)<<GLB_REG_GPIO_6_I_POS))
#define GLB_REG_GPIO_7_I                                        GLB_REG_GPIO_7_I
#define GLB_REG_GPIO_7_I_POS                                    (7U)
#define GLB_REG_GPIO_7_I_LEN                                    (1U)
#define GLB_REG_GPIO_7_I_MSK                                    (((1U<<GLB_REG_GPIO_7_I_LEN)-1)<<GLB_REG_GPIO_7_I_POS)
#define GLB_REG_GPIO_7_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_7_I_LEN)-1)<<GLB_REG_GPIO_7_I_POS))
#define GLB_REG_GPIO_8_I                                        GLB_REG_GPIO_8_I
#define GLB_REG_GPIO_8_I_POS                                    (8U)
#define GLB_REG_GPIO_8_I_LEN                                    (1U)
#define GLB_REG_GPIO_8_I_MSK                                    (((1U<<GLB_REG_GPIO_8_I_LEN)-1)<<GLB_REG_GPIO_8_I_POS)
#define GLB_REG_GPIO_8_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_8_I_LEN)-1)<<GLB_REG_GPIO_8_I_POS))
#define GLB_REG_GPIO_9_I                                        GLB_REG_GPIO_9_I
#define GLB_REG_GPIO_9_I_POS                                    (9U)
#define GLB_REG_GPIO_9_I_LEN                                    (1U)
#define GLB_REG_GPIO_9_I_MSK                                    (((1U<<GLB_REG_GPIO_9_I_LEN)-1)<<GLB_REG_GPIO_9_I_POS)
#define GLB_REG_GPIO_9_I_UMSK                                   (~(((1U<<GLB_REG_GPIO_9_I_LEN)-1)<<GLB_REG_GPIO_9_I_POS))
#define GLB_REG_GPIO_10_I                                       GLB_REG_GPIO_10_I
#define GLB_REG_GPIO_10_I_POS                                   (10U)
#define GLB_REG_GPIO_10_I_LEN                                   (1U)
#define GLB_REG_GPIO_10_I_MSK                                   (((1U<<GLB_REG_GPIO_10_I_LEN)-1)<<GLB_REG_GPIO_10_I_POS)
#define GLB_REG_GPIO_10_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_10_I_LEN)-1)<<GLB_REG_GPIO_10_I_POS))
#define GLB_REG_GPIO_11_I                                       GLB_REG_GPIO_11_I
#define GLB_REG_GPIO_11_I_POS                                   (11U)
#define GLB_REG_GPIO_11_I_LEN                                   (1U)
#define GLB_REG_GPIO_11_I_MSK                                   (((1U<<GLB_REG_GPIO_11_I_LEN)-1)<<GLB_REG_GPIO_11_I_POS)
#define GLB_REG_GPIO_11_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_11_I_LEN)-1)<<GLB_REG_GPIO_11_I_POS))
#define GLB_REG_GPIO_12_I                                       GLB_REG_GPIO_12_I
#define GLB_REG_GPIO_12_I_POS                                   (12U)
#define GLB_REG_GPIO_12_I_LEN                                   (1U)
#define GLB_REG_GPIO_12_I_MSK                                   (((1U<<GLB_REG_GPIO_12_I_LEN)-1)<<GLB_REG_GPIO_12_I_POS)
#define GLB_REG_GPIO_12_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_12_I_LEN)-1)<<GLB_REG_GPIO_12_I_POS))
#define GLB_REG_GPIO_13_I                                       GLB_REG_GPIO_13_I
#define GLB_REG_GPIO_13_I_POS                                   (13U)
#define GLB_REG_GPIO_13_I_LEN                                   (1U)
#define GLB_REG_GPIO_13_I_MSK                                   (((1U<<GLB_REG_GPIO_13_I_LEN)-1)<<GLB_REG_GPIO_13_I_POS)
#define GLB_REG_GPIO_13_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_13_I_LEN)-1)<<GLB_REG_GPIO_13_I_POS))
#define GLB_REG_GPIO_14_I                                       GLB_REG_GPIO_14_I
#define GLB_REG_GPIO_14_I_POS                                   (14U)
#define GLB_REG_GPIO_14_I_LEN                                   (1U)
#define GLB_REG_GPIO_14_I_MSK                                   (((1U<<GLB_REG_GPIO_14_I_LEN)-1)<<GLB_REG_GPIO_14_I_POS)
#define GLB_REG_GPIO_14_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_14_I_LEN)-1)<<GLB_REG_GPIO_14_I_POS))
#define GLB_REG_GPIO_15_I                                       GLB_REG_GPIO_15_I
#define GLB_REG_GPIO_15_I_POS                                   (15U)
#define GLB_REG_GPIO_15_I_LEN                                   (1U)
#define GLB_REG_GPIO_15_I_MSK                                   (((1U<<GLB_REG_GPIO_15_I_LEN)-1)<<GLB_REG_GPIO_15_I_POS)
#define GLB_REG_GPIO_15_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_15_I_LEN)-1)<<GLB_REG_GPIO_15_I_POS))
#define GLB_REG_GPIO_16_I                                       GLB_REG_GPIO_16_I
#define GLB_REG_GPIO_16_I_POS                                   (16U)
#define GLB_REG_GPIO_16_I_LEN                                   (1U)
#define GLB_REG_GPIO_16_I_MSK                                   (((1U<<GLB_REG_GPIO_16_I_LEN)-1)<<GLB_REG_GPIO_16_I_POS)
#define GLB_REG_GPIO_16_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_16_I_LEN)-1)<<GLB_REG_GPIO_16_I_POS))
#define GLB_REG_GPIO_17_I                                       GLB_REG_GPIO_17_I
#define GLB_REG_GPIO_17_I_POS                                   (17U)
#define GLB_REG_GPIO_17_I_LEN                                   (1U)
#define GLB_REG_GPIO_17_I_MSK                                   (((1U<<GLB_REG_GPIO_17_I_LEN)-1)<<GLB_REG_GPIO_17_I_POS)
#define GLB_REG_GPIO_17_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_17_I_LEN)-1)<<GLB_REG_GPIO_17_I_POS))
#define GLB_REG_GPIO_18_I                                       GLB_REG_GPIO_18_I
#define GLB_REG_GPIO_18_I_POS                                   (18U)
#define GLB_REG_GPIO_18_I_LEN                                   (1U)
#define GLB_REG_GPIO_18_I_MSK                                   (((1U<<GLB_REG_GPIO_18_I_LEN)-1)<<GLB_REG_GPIO_18_I_POS)
#define GLB_REG_GPIO_18_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_18_I_LEN)-1)<<GLB_REG_GPIO_18_I_POS))
#define GLB_REG_GPIO_19_I                                       GLB_REG_GPIO_19_I
#define GLB_REG_GPIO_19_I_POS                                   (19U)
#define GLB_REG_GPIO_19_I_LEN                                   (1U)
#define GLB_REG_GPIO_19_I_MSK                                   (((1U<<GLB_REG_GPIO_19_I_LEN)-1)<<GLB_REG_GPIO_19_I_POS)
#define GLB_REG_GPIO_19_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_19_I_LEN)-1)<<GLB_REG_GPIO_19_I_POS))
#define GLB_REG_GPIO_20_I                                       GLB_REG_GPIO_20_I
#define GLB_REG_GPIO_20_I_POS                                   (20U)
#define GLB_REG_GPIO_20_I_LEN                                   (1U)
#define GLB_REG_GPIO_20_I_MSK                                   (((1U<<GLB_REG_GPIO_20_I_LEN)-1)<<GLB_REG_GPIO_20_I_POS)
#define GLB_REG_GPIO_20_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_20_I_LEN)-1)<<GLB_REG_GPIO_20_I_POS))
#define GLB_REG_GPIO_21_I                                       GLB_REG_GPIO_21_I
#define GLB_REG_GPIO_21_I_POS                                   (21U)
#define GLB_REG_GPIO_21_I_LEN                                   (1U)
#define GLB_REG_GPIO_21_I_MSK                                   (((1U<<GLB_REG_GPIO_21_I_LEN)-1)<<GLB_REG_GPIO_21_I_POS)
#define GLB_REG_GPIO_21_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_21_I_LEN)-1)<<GLB_REG_GPIO_21_I_POS))
#define GLB_REG_GPIO_22_I                                       GLB_REG_GPIO_22_I
#define GLB_REG_GPIO_22_I_POS                                   (22U)
#define GLB_REG_GPIO_22_I_LEN                                   (1U)
#define GLB_REG_GPIO_22_I_MSK                                   (((1U<<GLB_REG_GPIO_22_I_LEN)-1)<<GLB_REG_GPIO_22_I_POS)
#define GLB_REG_GPIO_22_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_22_I_LEN)-1)<<GLB_REG_GPIO_22_I_POS))
#define GLB_REG_GPIO_23_I                                       GLB_REG_GPIO_23_I
#define GLB_REG_GPIO_23_I_POS                                   (23U)
#define GLB_REG_GPIO_23_I_LEN                                   (1U)
#define GLB_REG_GPIO_23_I_MSK                                   (((1U<<GLB_REG_GPIO_23_I_LEN)-1)<<GLB_REG_GPIO_23_I_POS)
#define GLB_REG_GPIO_23_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_23_I_LEN)-1)<<GLB_REG_GPIO_23_I_POS))
#define GLB_REG_GPIO_24_I                                       GLB_REG_GPIO_24_I
#define GLB_REG_GPIO_24_I_POS                                   (24U)
#define GLB_REG_GPIO_24_I_LEN                                   (1U)
#define GLB_REG_GPIO_24_I_MSK                                   (((1U<<GLB_REG_GPIO_24_I_LEN)-1)<<GLB_REG_GPIO_24_I_POS)
#define GLB_REG_GPIO_24_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_24_I_LEN)-1)<<GLB_REG_GPIO_24_I_POS))
#define GLB_REG_GPIO_25_I                                       GLB_REG_GPIO_25_I
#define GLB_REG_GPIO_25_I_POS                                   (25U)
#define GLB_REG_GPIO_25_I_LEN                                   (1U)
#define GLB_REG_GPIO_25_I_MSK                                   (((1U<<GLB_REG_GPIO_25_I_LEN)-1)<<GLB_REG_GPIO_25_I_POS)
#define GLB_REG_GPIO_25_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_25_I_LEN)-1)<<GLB_REG_GPIO_25_I_POS))
#define GLB_REG_GPIO_26_I                                       GLB_REG_GPIO_26_I
#define GLB_REG_GPIO_26_I_POS                                   (26U)
#define GLB_REG_GPIO_26_I_LEN                                   (1U)
#define GLB_REG_GPIO_26_I_MSK                                   (((1U<<GLB_REG_GPIO_26_I_LEN)-1)<<GLB_REG_GPIO_26_I_POS)
#define GLB_REG_GPIO_26_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_26_I_LEN)-1)<<GLB_REG_GPIO_26_I_POS))
#define GLB_REG_GPIO_27_I                                       GLB_REG_GPIO_27_I
#define GLB_REG_GPIO_27_I_POS                                   (27U)
#define GLB_REG_GPIO_27_I_LEN                                   (1U)
#define GLB_REG_GPIO_27_I_MSK                                   (((1U<<GLB_REG_GPIO_27_I_LEN)-1)<<GLB_REG_GPIO_27_I_POS)
#define GLB_REG_GPIO_27_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_27_I_LEN)-1)<<GLB_REG_GPIO_27_I_POS))
#define GLB_REG_GPIO_28_I                                       GLB_REG_GPIO_28_I
#define GLB_REG_GPIO_28_I_POS                                   (28U)
#define GLB_REG_GPIO_28_I_LEN                                   (1U)
#define GLB_REG_GPIO_28_I_MSK                                   (((1U<<GLB_REG_GPIO_28_I_LEN)-1)<<GLB_REG_GPIO_28_I_POS)
#define GLB_REG_GPIO_28_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_28_I_LEN)-1)<<GLB_REG_GPIO_28_I_POS))
#define GLB_REG_GPIO_29_I                                       GLB_REG_GPIO_29_I
#define GLB_REG_GPIO_29_I_POS                                   (29U)
#define GLB_REG_GPIO_29_I_LEN                                   (1U)
#define GLB_REG_GPIO_29_I_MSK                                   (((1U<<GLB_REG_GPIO_29_I_LEN)-1)<<GLB_REG_GPIO_29_I_POS)
#define GLB_REG_GPIO_29_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_29_I_LEN)-1)<<GLB_REG_GPIO_29_I_POS))
#define GLB_REG_GPIO_30_I                                       GLB_REG_GPIO_30_I
#define GLB_REG_GPIO_30_I_POS                                   (30U)
#define GLB_REG_GPIO_30_I_LEN                                   (1U)
#define GLB_REG_GPIO_30_I_MSK                                   (((1U<<GLB_REG_GPIO_30_I_LEN)-1)<<GLB_REG_GPIO_30_I_POS)
#define GLB_REG_GPIO_30_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_30_I_LEN)-1)<<GLB_REG_GPIO_30_I_POS))
#define GLB_REG_GPIO_31_I                                       GLB_REG_GPIO_31_I
#define GLB_REG_GPIO_31_I_POS                                   (31U)
#define GLB_REG_GPIO_31_I_LEN                                   (1U)
#define GLB_REG_GPIO_31_I_MSK                                   (((1U<<GLB_REG_GPIO_31_I_LEN)-1)<<GLB_REG_GPIO_31_I_POS)
#define GLB_REG_GPIO_31_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_31_I_LEN)-1)<<GLB_REG_GPIO_31_I_POS))

/* 0x184 : GPIO_CFGCTL32 */
#define GLB_GPIO_CFGCTL32_OFFSET                                (0x184)
#define GLB_REG_GPIO_32_I                                       GLB_REG_GPIO_32_I
#define GLB_REG_GPIO_32_I_POS                                   (0U)
#define GLB_REG_GPIO_32_I_LEN                                   (1U)
#define GLB_REG_GPIO_32_I_MSK                                   (((1U<<GLB_REG_GPIO_32_I_LEN)-1)<<GLB_REG_GPIO_32_I_POS)
#define GLB_REG_GPIO_32_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_32_I_LEN)-1)<<GLB_REG_GPIO_32_I_POS))
#define GLB_REG_GPIO_33_I                                       GLB_REG_GPIO_33_I
#define GLB_REG_GPIO_33_I_POS                                   (1U)
#define GLB_REG_GPIO_33_I_LEN                                   (1U)
#define GLB_REG_GPIO_33_I_MSK                                   (((1U<<GLB_REG_GPIO_33_I_LEN)-1)<<GLB_REG_GPIO_33_I_POS)
#define GLB_REG_GPIO_33_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_33_I_LEN)-1)<<GLB_REG_GPIO_33_I_POS))
#define GLB_REG_GPIO_34_I                                       GLB_REG_GPIO_34_I
#define GLB_REG_GPIO_34_I_POS                                   (2U)
#define GLB_REG_GPIO_34_I_LEN                                   (1U)
#define GLB_REG_GPIO_34_I_MSK                                   (((1U<<GLB_REG_GPIO_34_I_LEN)-1)<<GLB_REG_GPIO_34_I_POS)
#define GLB_REG_GPIO_34_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_34_I_LEN)-1)<<GLB_REG_GPIO_34_I_POS))
#define GLB_REG_GPIO_35_I                                       GLB_REG_GPIO_35_I
#define GLB_REG_GPIO_35_I_POS                                   (3U)
#define GLB_REG_GPIO_35_I_LEN                                   (1U)
#define GLB_REG_GPIO_35_I_MSK                                   (((1U<<GLB_REG_GPIO_35_I_LEN)-1)<<GLB_REG_GPIO_35_I_POS)
#define GLB_REG_GPIO_35_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_35_I_LEN)-1)<<GLB_REG_GPIO_35_I_POS))
#define GLB_REG_GPIO_36_I                                       GLB_REG_GPIO_36_I
#define GLB_REG_GPIO_36_I_POS                                   (4U)
#define GLB_REG_GPIO_36_I_LEN                                   (1U)
#define GLB_REG_GPIO_36_I_MSK                                   (((1U<<GLB_REG_GPIO_36_I_LEN)-1)<<GLB_REG_GPIO_36_I_POS)
#define GLB_REG_GPIO_36_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_36_I_LEN)-1)<<GLB_REG_GPIO_36_I_POS))
#define GLB_REG_GPIO_37_I                                       GLB_REG_GPIO_37_I
#define GLB_REG_GPIO_37_I_POS                                   (5U)
#define GLB_REG_GPIO_37_I_LEN                                   (1U)
#define GLB_REG_GPIO_37_I_MSK                                   (((1U<<GLB_REG_GPIO_37_I_LEN)-1)<<GLB_REG_GPIO_37_I_POS)
#define GLB_REG_GPIO_37_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_37_I_LEN)-1)<<GLB_REG_GPIO_37_I_POS))
#define GLB_REG_GPIO_38_I                                       GLB_REG_GPIO_38_I
#define GLB_REG_GPIO_38_I_POS                                   (6U)
#define GLB_REG_GPIO_38_I_LEN                                   (1U)
#define GLB_REG_GPIO_38_I_MSK                                   (((1U<<GLB_REG_GPIO_38_I_LEN)-1)<<GLB_REG_GPIO_38_I_POS)
#define GLB_REG_GPIO_38_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_38_I_LEN)-1)<<GLB_REG_GPIO_38_I_POS))
#define GLB_REG_GPIO_39_I                                       GLB_REG_GPIO_39_I
#define GLB_REG_GPIO_39_I_POS                                   (7U)
#define GLB_REG_GPIO_39_I_LEN                                   (1U)
#define GLB_REG_GPIO_39_I_MSK                                   (((1U<<GLB_REG_GPIO_39_I_LEN)-1)<<GLB_REG_GPIO_39_I_POS)
#define GLB_REG_GPIO_39_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_39_I_LEN)-1)<<GLB_REG_GPIO_39_I_POS))
#define GLB_REG_GPIO_40_I                                       GLB_REG_GPIO_40_I
#define GLB_REG_GPIO_40_I_POS                                   (8U)
#define GLB_REG_GPIO_40_I_LEN                                   (1U)
#define GLB_REG_GPIO_40_I_MSK                                   (((1U<<GLB_REG_GPIO_40_I_LEN)-1)<<GLB_REG_GPIO_40_I_POS)
#define GLB_REG_GPIO_40_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_40_I_LEN)-1)<<GLB_REG_GPIO_40_I_POS))
#define GLB_REG_GPIO_41_I                                       GLB_REG_GPIO_41_I
#define GLB_REG_GPIO_41_I_POS                                   (9U)
#define GLB_REG_GPIO_41_I_LEN                                   (1U)
#define GLB_REG_GPIO_41_I_MSK                                   (((1U<<GLB_REG_GPIO_41_I_LEN)-1)<<GLB_REG_GPIO_41_I_POS)
#define GLB_REG_GPIO_41_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_41_I_LEN)-1)<<GLB_REG_GPIO_41_I_POS))
#define GLB_REG_GPIO_42_I                                       GLB_REG_GPIO_42_I
#define GLB_REG_GPIO_42_I_POS                                   (10U)
#define GLB_REG_GPIO_42_I_LEN                                   (1U)
#define GLB_REG_GPIO_42_I_MSK                                   (((1U<<GLB_REG_GPIO_42_I_LEN)-1)<<GLB_REG_GPIO_42_I_POS)
#define GLB_REG_GPIO_42_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_42_I_LEN)-1)<<GLB_REG_GPIO_42_I_POS))
#define GLB_REG_GPIO_43_I                                       GLB_REG_GPIO_43_I
#define GLB_REG_GPIO_43_I_POS                                   (11U)
#define GLB_REG_GPIO_43_I_LEN                                   (1U)
#define GLB_REG_GPIO_43_I_MSK                                   (((1U<<GLB_REG_GPIO_43_I_LEN)-1)<<GLB_REG_GPIO_43_I_POS)
#define GLB_REG_GPIO_43_I_UMSK                                  (~(((1U<<GLB_REG_GPIO_43_I_LEN)-1)<<GLB_REG_GPIO_43_I_POS))
#define GLB_GPIO_INT_STAT2                                      GLB_GPIO_INT_STAT2
#define GLB_GPIO_INT_STAT2_POS                                  (12U)
#define GLB_GPIO_INT_STAT2_LEN                                  (12U)
#define GLB_GPIO_INT_STAT2_MSK                                  (((1U<<GLB_GPIO_INT_STAT2_LEN)-1)<<GLB_GPIO_INT_STAT2_POS)
#define GLB_GPIO_INT_STAT2_UMSK                                 (~(((1U<<GLB_GPIO_INT_STAT2_LEN)-1)<<GLB_GPIO_INT_STAT2_POS))

/* 0x188 : GPIO_CFGCTL33 */
#define GLB_GPIO_CFGCTL33_OFFSET                                (0x188)
#define GLB_REG_GPIO_0_O                                        GLB_REG_GPIO_0_O
#define GLB_REG_GPIO_0_O_POS                                    (0U)
#define GLB_REG_GPIO_0_O_LEN                                    (1U)
#define GLB_REG_GPIO_0_O_MSK                                    (((1U<<GLB_REG_GPIO_0_O_LEN)-1)<<GLB_REG_GPIO_0_O_POS)
#define GLB_REG_GPIO_0_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_0_O_LEN)-1)<<GLB_REG_GPIO_0_O_POS))
#define GLB_REG_GPIO_1_O                                        GLB_REG_GPIO_1_O
#define GLB_REG_GPIO_1_O_POS                                    (1U)
#define GLB_REG_GPIO_1_O_LEN                                    (1U)
#define GLB_REG_GPIO_1_O_MSK                                    (((1U<<GLB_REG_GPIO_1_O_LEN)-1)<<GLB_REG_GPIO_1_O_POS)
#define GLB_REG_GPIO_1_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_1_O_LEN)-1)<<GLB_REG_GPIO_1_O_POS))
#define GLB_REG_GPIO_2_O                                        GLB_REG_GPIO_2_O
#define GLB_REG_GPIO_2_O_POS                                    (2U)
#define GLB_REG_GPIO_2_O_LEN                                    (1U)
#define GLB_REG_GPIO_2_O_MSK                                    (((1U<<GLB_REG_GPIO_2_O_LEN)-1)<<GLB_REG_GPIO_2_O_POS)
#define GLB_REG_GPIO_2_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_2_O_LEN)-1)<<GLB_REG_GPIO_2_O_POS))
#define GLB_REG_GPIO_3_O                                        GLB_REG_GPIO_3_O
#define GLB_REG_GPIO_3_O_POS                                    (3U)
#define GLB_REG_GPIO_3_O_LEN                                    (1U)
#define GLB_REG_GPIO_3_O_MSK                                    (((1U<<GLB_REG_GPIO_3_O_LEN)-1)<<GLB_REG_GPIO_3_O_POS)
#define GLB_REG_GPIO_3_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_3_O_LEN)-1)<<GLB_REG_GPIO_3_O_POS))
#define GLB_REG_GPIO_4_O                                        GLB_REG_GPIO_4_O
#define GLB_REG_GPIO_4_O_POS                                    (4U)
#define GLB_REG_GPIO_4_O_LEN                                    (1U)
#define GLB_REG_GPIO_4_O_MSK                                    (((1U<<GLB_REG_GPIO_4_O_LEN)-1)<<GLB_REG_GPIO_4_O_POS)
#define GLB_REG_GPIO_4_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_4_O_LEN)-1)<<GLB_REG_GPIO_4_O_POS))
#define GLB_REG_GPIO_5_O                                        GLB_REG_GPIO_5_O
#define GLB_REG_GPIO_5_O_POS                                    (5U)
#define GLB_REG_GPIO_5_O_LEN                                    (1U)
#define GLB_REG_GPIO_5_O_MSK                                    (((1U<<GLB_REG_GPIO_5_O_LEN)-1)<<GLB_REG_GPIO_5_O_POS)
#define GLB_REG_GPIO_5_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_5_O_LEN)-1)<<GLB_REG_GPIO_5_O_POS))
#define GLB_REG_GPIO_6_O                                        GLB_REG_GPIO_6_O
#define GLB_REG_GPIO_6_O_POS                                    (6U)
#define GLB_REG_GPIO_6_O_LEN                                    (1U)
#define GLB_REG_GPIO_6_O_MSK                                    (((1U<<GLB_REG_GPIO_6_O_LEN)-1)<<GLB_REG_GPIO_6_O_POS)
#define GLB_REG_GPIO_6_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_6_O_LEN)-1)<<GLB_REG_GPIO_6_O_POS))
#define GLB_REG_GPIO_7_O                                        GLB_REG_GPIO_7_O
#define GLB_REG_GPIO_7_O_POS                                    (7U)
#define GLB_REG_GPIO_7_O_LEN                                    (1U)
#define GLB_REG_GPIO_7_O_MSK                                    (((1U<<GLB_REG_GPIO_7_O_LEN)-1)<<GLB_REG_GPIO_7_O_POS)
#define GLB_REG_GPIO_7_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_7_O_LEN)-1)<<GLB_REG_GPIO_7_O_POS))
#define GLB_REG_GPIO_8_O                                        GLB_REG_GPIO_8_O
#define GLB_REG_GPIO_8_O_POS                                    (8U)
#define GLB_REG_GPIO_8_O_LEN                                    (1U)
#define GLB_REG_GPIO_8_O_MSK                                    (((1U<<GLB_REG_GPIO_8_O_LEN)-1)<<GLB_REG_GPIO_8_O_POS)
#define GLB_REG_GPIO_8_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_8_O_LEN)-1)<<GLB_REG_GPIO_8_O_POS))
#define GLB_REG_GPIO_9_O                                        GLB_REG_GPIO_9_O
#define GLB_REG_GPIO_9_O_POS                                    (9U)
#define GLB_REG_GPIO_9_O_LEN                                    (1U)
#define GLB_REG_GPIO_9_O_MSK                                    (((1U<<GLB_REG_GPIO_9_O_LEN)-1)<<GLB_REG_GPIO_9_O_POS)
#define GLB_REG_GPIO_9_O_UMSK                                   (~(((1U<<GLB_REG_GPIO_9_O_LEN)-1)<<GLB_REG_GPIO_9_O_POS))
#define GLB_REG_GPIO_10_O                                       GLB_REG_GPIO_10_O
#define GLB_REG_GPIO_10_O_POS                                   (10U)
#define GLB_REG_GPIO_10_O_LEN                                   (1U)
#define GLB_REG_GPIO_10_O_MSK                                   (((1U<<GLB_REG_GPIO_10_O_LEN)-1)<<GLB_REG_GPIO_10_O_POS)
#define GLB_REG_GPIO_10_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_10_O_LEN)-1)<<GLB_REG_GPIO_10_O_POS))
#define GLB_REG_GPIO_11_O                                       GLB_REG_GPIO_11_O
#define GLB_REG_GPIO_11_O_POS                                   (11U)
#define GLB_REG_GPIO_11_O_LEN                                   (1U)
#define GLB_REG_GPIO_11_O_MSK                                   (((1U<<GLB_REG_GPIO_11_O_LEN)-1)<<GLB_REG_GPIO_11_O_POS)
#define GLB_REG_GPIO_11_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_11_O_LEN)-1)<<GLB_REG_GPIO_11_O_POS))
#define GLB_REG_GPIO_12_O                                       GLB_REG_GPIO_12_O
#define GLB_REG_GPIO_12_O_POS                                   (12U)
#define GLB_REG_GPIO_12_O_LEN                                   (1U)
#define GLB_REG_GPIO_12_O_MSK                                   (((1U<<GLB_REG_GPIO_12_O_LEN)-1)<<GLB_REG_GPIO_12_O_POS)
#define GLB_REG_GPIO_12_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_12_O_LEN)-1)<<GLB_REG_GPIO_12_O_POS))
#define GLB_REG_GPIO_13_O                                       GLB_REG_GPIO_13_O
#define GLB_REG_GPIO_13_O_POS                                   (13U)
#define GLB_REG_GPIO_13_O_LEN                                   (1U)
#define GLB_REG_GPIO_13_O_MSK                                   (((1U<<GLB_REG_GPIO_13_O_LEN)-1)<<GLB_REG_GPIO_13_O_POS)
#define GLB_REG_GPIO_13_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_13_O_LEN)-1)<<GLB_REG_GPIO_13_O_POS))
#define GLB_REG_GPIO_14_O                                       GLB_REG_GPIO_14_O
#define GLB_REG_GPIO_14_O_POS                                   (14U)
#define GLB_REG_GPIO_14_O_LEN                                   (1U)
#define GLB_REG_GPIO_14_O_MSK                                   (((1U<<GLB_REG_GPIO_14_O_LEN)-1)<<GLB_REG_GPIO_14_O_POS)
#define GLB_REG_GPIO_14_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_14_O_LEN)-1)<<GLB_REG_GPIO_14_O_POS))
#define GLB_REG_GPIO_15_O                                       GLB_REG_GPIO_15_O
#define GLB_REG_GPIO_15_O_POS                                   (15U)
#define GLB_REG_GPIO_15_O_LEN                                   (1U)
#define GLB_REG_GPIO_15_O_MSK                                   (((1U<<GLB_REG_GPIO_15_O_LEN)-1)<<GLB_REG_GPIO_15_O_POS)
#define GLB_REG_GPIO_15_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_15_O_LEN)-1)<<GLB_REG_GPIO_15_O_POS))
#define GLB_REG_GPIO_16_O                                       GLB_REG_GPIO_16_O
#define GLB_REG_GPIO_16_O_POS                                   (16U)
#define GLB_REG_GPIO_16_O_LEN                                   (1U)
#define GLB_REG_GPIO_16_O_MSK                                   (((1U<<GLB_REG_GPIO_16_O_LEN)-1)<<GLB_REG_GPIO_16_O_POS)
#define GLB_REG_GPIO_16_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_16_O_LEN)-1)<<GLB_REG_GPIO_16_O_POS))
#define GLB_REG_GPIO_17_O                                       GLB_REG_GPIO_17_O
#define GLB_REG_GPIO_17_O_POS                                   (17U)
#define GLB_REG_GPIO_17_O_LEN                                   (1U)
#define GLB_REG_GPIO_17_O_MSK                                   (((1U<<GLB_REG_GPIO_17_O_LEN)-1)<<GLB_REG_GPIO_17_O_POS)
#define GLB_REG_GPIO_17_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_17_O_LEN)-1)<<GLB_REG_GPIO_17_O_POS))
#define GLB_REG_GPIO_18_O                                       GLB_REG_GPIO_18_O
#define GLB_REG_GPIO_18_O_POS                                   (18U)
#define GLB_REG_GPIO_18_O_LEN                                   (1U)
#define GLB_REG_GPIO_18_O_MSK                                   (((1U<<GLB_REG_GPIO_18_O_LEN)-1)<<GLB_REG_GPIO_18_O_POS)
#define GLB_REG_GPIO_18_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_18_O_LEN)-1)<<GLB_REG_GPIO_18_O_POS))
#define GLB_REG_GPIO_19_O                                       GLB_REG_GPIO_19_O
#define GLB_REG_GPIO_19_O_POS                                   (19U)
#define GLB_REG_GPIO_19_O_LEN                                   (1U)
#define GLB_REG_GPIO_19_O_MSK                                   (((1U<<GLB_REG_GPIO_19_O_LEN)-1)<<GLB_REG_GPIO_19_O_POS)
#define GLB_REG_GPIO_19_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_19_O_LEN)-1)<<GLB_REG_GPIO_19_O_POS))
#define GLB_REG_GPIO_20_O                                       GLB_REG_GPIO_20_O
#define GLB_REG_GPIO_20_O_POS                                   (20U)
#define GLB_REG_GPIO_20_O_LEN                                   (1U)
#define GLB_REG_GPIO_20_O_MSK                                   (((1U<<GLB_REG_GPIO_20_O_LEN)-1)<<GLB_REG_GPIO_20_O_POS)
#define GLB_REG_GPIO_20_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_20_O_LEN)-1)<<GLB_REG_GPIO_20_O_POS))
#define GLB_REG_GPIO_21_O                                       GLB_REG_GPIO_21_O
#define GLB_REG_GPIO_21_O_POS                                   (21U)
#define GLB_REG_GPIO_21_O_LEN                                   (1U)
#define GLB_REG_GPIO_21_O_MSK                                   (((1U<<GLB_REG_GPIO_21_O_LEN)-1)<<GLB_REG_GPIO_21_O_POS)
#define GLB_REG_GPIO_21_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_21_O_LEN)-1)<<GLB_REG_GPIO_21_O_POS))
#define GLB_REG_GPIO_22_O                                       GLB_REG_GPIO_22_O
#define GLB_REG_GPIO_22_O_POS                                   (22U)
#define GLB_REG_GPIO_22_O_LEN                                   (1U)
#define GLB_REG_GPIO_22_O_MSK                                   (((1U<<GLB_REG_GPIO_22_O_LEN)-1)<<GLB_REG_GPIO_22_O_POS)
#define GLB_REG_GPIO_22_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_22_O_LEN)-1)<<GLB_REG_GPIO_22_O_POS))
#define GLB_REG_GPIO_23_O                                       GLB_REG_GPIO_23_O
#define GLB_REG_GPIO_23_O_POS                                   (23U)
#define GLB_REG_GPIO_23_O_LEN                                   (1U)
#define GLB_REG_GPIO_23_O_MSK                                   (((1U<<GLB_REG_GPIO_23_O_LEN)-1)<<GLB_REG_GPIO_23_O_POS)
#define GLB_REG_GPIO_23_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_23_O_LEN)-1)<<GLB_REG_GPIO_23_O_POS))
#define GLB_REG_GPIO_24_O                                       GLB_REG_GPIO_24_O
#define GLB_REG_GPIO_24_O_POS                                   (24U)
#define GLB_REG_GPIO_24_O_LEN                                   (1U)
#define GLB_REG_GPIO_24_O_MSK                                   (((1U<<GLB_REG_GPIO_24_O_LEN)-1)<<GLB_REG_GPIO_24_O_POS)
#define GLB_REG_GPIO_24_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_24_O_LEN)-1)<<GLB_REG_GPIO_24_O_POS))
#define GLB_REG_GPIO_25_O                                       GLB_REG_GPIO_25_O
#define GLB_REG_GPIO_25_O_POS                                   (25U)
#define GLB_REG_GPIO_25_O_LEN                                   (1U)
#define GLB_REG_GPIO_25_O_MSK                                   (((1U<<GLB_REG_GPIO_25_O_LEN)-1)<<GLB_REG_GPIO_25_O_POS)
#define GLB_REG_GPIO_25_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_25_O_LEN)-1)<<GLB_REG_GPIO_25_O_POS))
#define GLB_REG_GPIO_26_O                                       GLB_REG_GPIO_26_O
#define GLB_REG_GPIO_26_O_POS                                   (26U)
#define GLB_REG_GPIO_26_O_LEN                                   (1U)
#define GLB_REG_GPIO_26_O_MSK                                   (((1U<<GLB_REG_GPIO_26_O_LEN)-1)<<GLB_REG_GPIO_26_O_POS)
#define GLB_REG_GPIO_26_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_26_O_LEN)-1)<<GLB_REG_GPIO_26_O_POS))
#define GLB_REG_GPIO_27_O                                       GLB_REG_GPIO_27_O
#define GLB_REG_GPIO_27_O_POS                                   (27U)
#define GLB_REG_GPIO_27_O_LEN                                   (1U)
#define GLB_REG_GPIO_27_O_MSK                                   (((1U<<GLB_REG_GPIO_27_O_LEN)-1)<<GLB_REG_GPIO_27_O_POS)
#define GLB_REG_GPIO_27_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_27_O_LEN)-1)<<GLB_REG_GPIO_27_O_POS))
#define GLB_REG_GPIO_28_O                                       GLB_REG_GPIO_28_O
#define GLB_REG_GPIO_28_O_POS                                   (28U)
#define GLB_REG_GPIO_28_O_LEN                                   (1U)
#define GLB_REG_GPIO_28_O_MSK                                   (((1U<<GLB_REG_GPIO_28_O_LEN)-1)<<GLB_REG_GPIO_28_O_POS)
#define GLB_REG_GPIO_28_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_28_O_LEN)-1)<<GLB_REG_GPIO_28_O_POS))
#define GLB_REG_GPIO_29_O                                       GLB_REG_GPIO_29_O
#define GLB_REG_GPIO_29_O_POS                                   (29U)
#define GLB_REG_GPIO_29_O_LEN                                   (1U)
#define GLB_REG_GPIO_29_O_MSK                                   (((1U<<GLB_REG_GPIO_29_O_LEN)-1)<<GLB_REG_GPIO_29_O_POS)
#define GLB_REG_GPIO_29_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_29_O_LEN)-1)<<GLB_REG_GPIO_29_O_POS))
#define GLB_REG_GPIO_30_O                                       GLB_REG_GPIO_30_O
#define GLB_REG_GPIO_30_O_POS                                   (30U)
#define GLB_REG_GPIO_30_O_LEN                                   (1U)
#define GLB_REG_GPIO_30_O_MSK                                   (((1U<<GLB_REG_GPIO_30_O_LEN)-1)<<GLB_REG_GPIO_30_O_POS)
#define GLB_REG_GPIO_30_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_30_O_LEN)-1)<<GLB_REG_GPIO_30_O_POS))
#define GLB_REG_GPIO_31_O                                       GLB_REG_GPIO_31_O
#define GLB_REG_GPIO_31_O_POS                                   (31U)
#define GLB_REG_GPIO_31_O_LEN                                   (1U)
#define GLB_REG_GPIO_31_O_MSK                                   (((1U<<GLB_REG_GPIO_31_O_LEN)-1)<<GLB_REG_GPIO_31_O_POS)
#define GLB_REG_GPIO_31_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_31_O_LEN)-1)<<GLB_REG_GPIO_31_O_POS))

/* 0x18C : GPIO_CFGCTL34 */
#define GLB_GPIO_CFGCTL34_OFFSET                                (0x18C)
#define GLB_REG_GPIO_32_OE                                      GLB_REG_GPIO_32_OE
#define GLB_REG_GPIO_32_OE_POS                                  (0U)
#define GLB_REG_GPIO_32_OE_LEN                                  (1U)
#define GLB_REG_GPIO_32_OE_MSK                                  (((1U<<GLB_REG_GPIO_32_OE_LEN)-1)<<GLB_REG_GPIO_32_OE_POS)
#define GLB_REG_GPIO_32_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_32_OE_LEN)-1)<<GLB_REG_GPIO_32_OE_POS))
#define GLB_REG_GPIO_33_OE                                      GLB_REG_GPIO_33_OE
#define GLB_REG_GPIO_33_OE_POS                                  (1U)
#define GLB_REG_GPIO_33_OE_LEN                                  (1U)
#define GLB_REG_GPIO_33_OE_MSK                                  (((1U<<GLB_REG_GPIO_33_OE_LEN)-1)<<GLB_REG_GPIO_33_OE_POS)
#define GLB_REG_GPIO_33_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_33_OE_LEN)-1)<<GLB_REG_GPIO_33_OE_POS))
#define GLB_REG_GPIO_34_OE                                      GLB_REG_GPIO_34_OE
#define GLB_REG_GPIO_34_OE_POS                                  (2U)
#define GLB_REG_GPIO_34_OE_LEN                                  (1U)
#define GLB_REG_GPIO_34_OE_MSK                                  (((1U<<GLB_REG_GPIO_34_OE_LEN)-1)<<GLB_REG_GPIO_34_OE_POS)
#define GLB_REG_GPIO_34_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_34_OE_LEN)-1)<<GLB_REG_GPIO_34_OE_POS))
#define GLB_REG_GPIO_35_OE                                      GLB_REG_GPIO_35_OE
#define GLB_REG_GPIO_35_OE_POS                                  (3U)
#define GLB_REG_GPIO_35_OE_LEN                                  (1U)
#define GLB_REG_GPIO_35_OE_MSK                                  (((1U<<GLB_REG_GPIO_35_OE_LEN)-1)<<GLB_REG_GPIO_35_OE_POS)
#define GLB_REG_GPIO_35_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_35_OE_LEN)-1)<<GLB_REG_GPIO_35_OE_POS))
#define GLB_REG_GPIO_36_OE                                      GLB_REG_GPIO_36_OE
#define GLB_REG_GPIO_36_OE_POS                                  (4U)
#define GLB_REG_GPIO_36_OE_LEN                                  (1U)
#define GLB_REG_GPIO_36_OE_MSK                                  (((1U<<GLB_REG_GPIO_36_OE_LEN)-1)<<GLB_REG_GPIO_36_OE_POS)
#define GLB_REG_GPIO_36_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_36_OE_LEN)-1)<<GLB_REG_GPIO_36_OE_POS))
#define GLB_REG_GPIO_37_OE                                      GLB_REG_GPIO_37_OE
#define GLB_REG_GPIO_37_OE_POS                                  (5U)
#define GLB_REG_GPIO_37_OE_LEN                                  (1U)
#define GLB_REG_GPIO_37_OE_MSK                                  (((1U<<GLB_REG_GPIO_37_OE_LEN)-1)<<GLB_REG_GPIO_37_OE_POS)
#define GLB_REG_GPIO_37_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_37_OE_LEN)-1)<<GLB_REG_GPIO_37_OE_POS))
#define GLB_REG_GPIO_38_OE                                      GLB_REG_GPIO_38_OE
#define GLB_REG_GPIO_38_OE_POS                                  (6U)
#define GLB_REG_GPIO_38_OE_LEN                                  (1U)
#define GLB_REG_GPIO_38_OE_MSK                                  (((1U<<GLB_REG_GPIO_38_OE_LEN)-1)<<GLB_REG_GPIO_38_OE_POS)
#define GLB_REG_GPIO_38_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_38_OE_LEN)-1)<<GLB_REG_GPIO_38_OE_POS))
#define GLB_REG_GPIO_39_OE                                      GLB_REG_GPIO_39_OE
#define GLB_REG_GPIO_39_OE_POS                                  (7U)
#define GLB_REG_GPIO_39_OE_LEN                                  (1U)
#define GLB_REG_GPIO_39_OE_MSK                                  (((1U<<GLB_REG_GPIO_39_OE_LEN)-1)<<GLB_REG_GPIO_39_OE_POS)
#define GLB_REG_GPIO_39_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_39_OE_LEN)-1)<<GLB_REG_GPIO_39_OE_POS))
#define GLB_REG_GPIO_40_OE                                      GLB_REG_GPIO_40_OE
#define GLB_REG_GPIO_40_OE_POS                                  (8U)
#define GLB_REG_GPIO_40_OE_LEN                                  (1U)
#define GLB_REG_GPIO_40_OE_MSK                                  (((1U<<GLB_REG_GPIO_40_OE_LEN)-1)<<GLB_REG_GPIO_40_OE_POS)
#define GLB_REG_GPIO_40_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_40_OE_LEN)-1)<<GLB_REG_GPIO_40_OE_POS))
#define GLB_REG_GPIO_41_OE                                      GLB_REG_GPIO_41_OE
#define GLB_REG_GPIO_41_OE_POS                                  (9U)
#define GLB_REG_GPIO_41_OE_LEN                                  (1U)
#define GLB_REG_GPIO_41_OE_MSK                                  (((1U<<GLB_REG_GPIO_41_OE_LEN)-1)<<GLB_REG_GPIO_41_OE_POS)
#define GLB_REG_GPIO_41_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_41_OE_LEN)-1)<<GLB_REG_GPIO_41_OE_POS))
#define GLB_REG_GPIO_42_OE                                      GLB_REG_GPIO_42_OE
#define GLB_REG_GPIO_42_OE_POS                                  (10U)
#define GLB_REG_GPIO_42_OE_LEN                                  (1U)
#define GLB_REG_GPIO_42_OE_MSK                                  (((1U<<GLB_REG_GPIO_42_OE_LEN)-1)<<GLB_REG_GPIO_42_OE_POS)
#define GLB_REG_GPIO_42_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_42_OE_LEN)-1)<<GLB_REG_GPIO_42_OE_POS))
#define GLB_REG_GPIO_43_OE                                      GLB_REG_GPIO_43_OE
#define GLB_REG_GPIO_43_OE_POS                                  (11U)
#define GLB_REG_GPIO_43_OE_LEN                                  (1U)
#define GLB_REG_GPIO_43_OE_MSK                                  (((1U<<GLB_REG_GPIO_43_OE_LEN)-1)<<GLB_REG_GPIO_43_OE_POS)
#define GLB_REG_GPIO_43_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_43_OE_LEN)-1)<<GLB_REG_GPIO_43_OE_POS))
#define GLB_REG_GPIO_32_O                                       GLB_REG_GPIO_32_O
#define GLB_REG_GPIO_32_O_POS                                   (12U)
#define GLB_REG_GPIO_32_O_LEN                                   (1U)
#define GLB_REG_GPIO_32_O_MSK                                   (((1U<<GLB_REG_GPIO_32_O_LEN)-1)<<GLB_REG_GPIO_32_O_POS)
#define GLB_REG_GPIO_32_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_32_O_LEN)-1)<<GLB_REG_GPIO_32_O_POS))
#define GLB_REG_GPIO_33_O                                       GLB_REG_GPIO_33_O
#define GLB_REG_GPIO_33_O_POS                                   (13U)
#define GLB_REG_GPIO_33_O_LEN                                   (1U)
#define GLB_REG_GPIO_33_O_MSK                                   (((1U<<GLB_REG_GPIO_33_O_LEN)-1)<<GLB_REG_GPIO_33_O_POS)
#define GLB_REG_GPIO_33_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_33_O_LEN)-1)<<GLB_REG_GPIO_33_O_POS))
#define GLB_REG_GPIO_34_O                                       GLB_REG_GPIO_34_O
#define GLB_REG_GPIO_34_O_POS                                   (14U)
#define GLB_REG_GPIO_34_O_LEN                                   (1U)
#define GLB_REG_GPIO_34_O_MSK                                   (((1U<<GLB_REG_GPIO_34_O_LEN)-1)<<GLB_REG_GPIO_34_O_POS)
#define GLB_REG_GPIO_34_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_34_O_LEN)-1)<<GLB_REG_GPIO_34_O_POS))
#define GLB_REG_GPIO_35_O                                       GLB_REG_GPIO_35_O
#define GLB_REG_GPIO_35_O_POS                                   (15U)
#define GLB_REG_GPIO_35_O_LEN                                   (1U)
#define GLB_REG_GPIO_35_O_MSK                                   (((1U<<GLB_REG_GPIO_35_O_LEN)-1)<<GLB_REG_GPIO_35_O_POS)
#define GLB_REG_GPIO_35_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_35_O_LEN)-1)<<GLB_REG_GPIO_35_O_POS))
#define GLB_REG_GPIO_36_O                                       GLB_REG_GPIO_36_O
#define GLB_REG_GPIO_36_O_POS                                   (16U)
#define GLB_REG_GPIO_36_O_LEN                                   (1U)
#define GLB_REG_GPIO_36_O_MSK                                   (((1U<<GLB_REG_GPIO_36_O_LEN)-1)<<GLB_REG_GPIO_36_O_POS)
#define GLB_REG_GPIO_36_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_36_O_LEN)-1)<<GLB_REG_GPIO_36_O_POS))
#define GLB_REG_GPIO_37_O                                       GLB_REG_GPIO_37_O
#define GLB_REG_GPIO_37_O_POS                                   (17U)
#define GLB_REG_GPIO_37_O_LEN                                   (1U)
#define GLB_REG_GPIO_37_O_MSK                                   (((1U<<GLB_REG_GPIO_37_O_LEN)-1)<<GLB_REG_GPIO_37_O_POS)
#define GLB_REG_GPIO_37_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_37_O_LEN)-1)<<GLB_REG_GPIO_37_O_POS))
#define GLB_REG_GPIO_38_O                                       GLB_REG_GPIO_38_O
#define GLB_REG_GPIO_38_O_POS                                   (18U)
#define GLB_REG_GPIO_38_O_LEN                                   (1U)
#define GLB_REG_GPIO_38_O_MSK                                   (((1U<<GLB_REG_GPIO_38_O_LEN)-1)<<GLB_REG_GPIO_38_O_POS)
#define GLB_REG_GPIO_38_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_38_O_LEN)-1)<<GLB_REG_GPIO_38_O_POS))
#define GLB_REG_GPIO_39_O                                       GLB_REG_GPIO_39_O
#define GLB_REG_GPIO_39_O_POS                                   (19U)
#define GLB_REG_GPIO_39_O_LEN                                   (1U)
#define GLB_REG_GPIO_39_O_MSK                                   (((1U<<GLB_REG_GPIO_39_O_LEN)-1)<<GLB_REG_GPIO_39_O_POS)
#define GLB_REG_GPIO_39_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_39_O_LEN)-1)<<GLB_REG_GPIO_39_O_POS))
#define GLB_REG_GPIO_40_O                                       GLB_REG_GPIO_40_O
#define GLB_REG_GPIO_40_O_POS                                   (20U)
#define GLB_REG_GPIO_40_O_LEN                                   (1U)
#define GLB_REG_GPIO_40_O_MSK                                   (((1U<<GLB_REG_GPIO_40_O_LEN)-1)<<GLB_REG_GPIO_40_O_POS)
#define GLB_REG_GPIO_40_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_40_O_LEN)-1)<<GLB_REG_GPIO_40_O_POS))
#define GLB_REG_GPIO_41_O                                       GLB_REG_GPIO_41_O
#define GLB_REG_GPIO_41_O_POS                                   (21U)
#define GLB_REG_GPIO_41_O_LEN                                   (1U)
#define GLB_REG_GPIO_41_O_MSK                                   (((1U<<GLB_REG_GPIO_41_O_LEN)-1)<<GLB_REG_GPIO_41_O_POS)
#define GLB_REG_GPIO_41_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_41_O_LEN)-1)<<GLB_REG_GPIO_41_O_POS))
#define GLB_REG_GPIO_42_O                                       GLB_REG_GPIO_42_O
#define GLB_REG_GPIO_42_O_POS                                   (22U)
#define GLB_REG_GPIO_42_O_LEN                                   (1U)
#define GLB_REG_GPIO_42_O_MSK                                   (((1U<<GLB_REG_GPIO_42_O_LEN)-1)<<GLB_REG_GPIO_42_O_POS)
#define GLB_REG_GPIO_42_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_42_O_LEN)-1)<<GLB_REG_GPIO_42_O_POS))
#define GLB_REG_GPIO_43_O                                       GLB_REG_GPIO_43_O
#define GLB_REG_GPIO_43_O_POS                                   (23U)
#define GLB_REG_GPIO_43_O_LEN                                   (1U)
#define GLB_REG_GPIO_43_O_MSK                                   (((1U<<GLB_REG_GPIO_43_O_LEN)-1)<<GLB_REG_GPIO_43_O_POS)
#define GLB_REG_GPIO_43_O_UMSK                                  (~(((1U<<GLB_REG_GPIO_43_O_LEN)-1)<<GLB_REG_GPIO_43_O_POS))

/* 0x190 : GPIO_CFGCTL35 */
#define GLB_GPIO_CFGCTL35_OFFSET                                (0x190)
#define GLB_REG_GPIO_0_OE                                       GLB_REG_GPIO_0_OE
#define GLB_REG_GPIO_0_OE_POS                                   (0U)
#define GLB_REG_GPIO_0_OE_LEN                                   (1U)
#define GLB_REG_GPIO_0_OE_MSK                                   (((1U<<GLB_REG_GPIO_0_OE_LEN)-1)<<GLB_REG_GPIO_0_OE_POS)
#define GLB_REG_GPIO_0_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_0_OE_LEN)-1)<<GLB_REG_GPIO_0_OE_POS))
#define GLB_REG_GPIO_1_OE                                       GLB_REG_GPIO_1_OE
#define GLB_REG_GPIO_1_OE_POS                                   (1U)
#define GLB_REG_GPIO_1_OE_LEN                                   (1U)
#define GLB_REG_GPIO_1_OE_MSK                                   (((1U<<GLB_REG_GPIO_1_OE_LEN)-1)<<GLB_REG_GPIO_1_OE_POS)
#define GLB_REG_GPIO_1_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_1_OE_LEN)-1)<<GLB_REG_GPIO_1_OE_POS))
#define GLB_REG_GPIO_2_OE                                       GLB_REG_GPIO_2_OE
#define GLB_REG_GPIO_2_OE_POS                                   (2U)
#define GLB_REG_GPIO_2_OE_LEN                                   (1U)
#define GLB_REG_GPIO_2_OE_MSK                                   (((1U<<GLB_REG_GPIO_2_OE_LEN)-1)<<GLB_REG_GPIO_2_OE_POS)
#define GLB_REG_GPIO_2_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_2_OE_LEN)-1)<<GLB_REG_GPIO_2_OE_POS))
#define GLB_REG_GPIO_3_OE                                       GLB_REG_GPIO_3_OE
#define GLB_REG_GPIO_3_OE_POS                                   (3U)
#define GLB_REG_GPIO_3_OE_LEN                                   (1U)
#define GLB_REG_GPIO_3_OE_MSK                                   (((1U<<GLB_REG_GPIO_3_OE_LEN)-1)<<GLB_REG_GPIO_3_OE_POS)
#define GLB_REG_GPIO_3_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_3_OE_LEN)-1)<<GLB_REG_GPIO_3_OE_POS))
#define GLB_REG_GPIO_4_OE                                       GLB_REG_GPIO_4_OE
#define GLB_REG_GPIO_4_OE_POS                                   (4U)
#define GLB_REG_GPIO_4_OE_LEN                                   (1U)
#define GLB_REG_GPIO_4_OE_MSK                                   (((1U<<GLB_REG_GPIO_4_OE_LEN)-1)<<GLB_REG_GPIO_4_OE_POS)
#define GLB_REG_GPIO_4_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_4_OE_LEN)-1)<<GLB_REG_GPIO_4_OE_POS))
#define GLB_REG_GPIO_5_OE                                       GLB_REG_GPIO_5_OE
#define GLB_REG_GPIO_5_OE_POS                                   (5U)
#define GLB_REG_GPIO_5_OE_LEN                                   (1U)
#define GLB_REG_GPIO_5_OE_MSK                                   (((1U<<GLB_REG_GPIO_5_OE_LEN)-1)<<GLB_REG_GPIO_5_OE_POS)
#define GLB_REG_GPIO_5_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_5_OE_LEN)-1)<<GLB_REG_GPIO_5_OE_POS))
#define GLB_REG_GPIO_6_OE                                       GLB_REG_GPIO_6_OE
#define GLB_REG_GPIO_6_OE_POS                                   (6U)
#define GLB_REG_GPIO_6_OE_LEN                                   (1U)
#define GLB_REG_GPIO_6_OE_MSK                                   (((1U<<GLB_REG_GPIO_6_OE_LEN)-1)<<GLB_REG_GPIO_6_OE_POS)
#define GLB_REG_GPIO_6_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_6_OE_LEN)-1)<<GLB_REG_GPIO_6_OE_POS))
#define GLB_REG_GPIO_7_OE                                       GLB_REG_GPIO_7_OE
#define GLB_REG_GPIO_7_OE_POS                                   (7U)
#define GLB_REG_GPIO_7_OE_LEN                                   (1U)
#define GLB_REG_GPIO_7_OE_MSK                                   (((1U<<GLB_REG_GPIO_7_OE_LEN)-1)<<GLB_REG_GPIO_7_OE_POS)
#define GLB_REG_GPIO_7_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_7_OE_LEN)-1)<<GLB_REG_GPIO_7_OE_POS))
#define GLB_REG_GPIO_8_OE                                       GLB_REG_GPIO_8_OE
#define GLB_REG_GPIO_8_OE_POS                                   (8U)
#define GLB_REG_GPIO_8_OE_LEN                                   (1U)
#define GLB_REG_GPIO_8_OE_MSK                                   (((1U<<GLB_REG_GPIO_8_OE_LEN)-1)<<GLB_REG_GPIO_8_OE_POS)
#define GLB_REG_GPIO_8_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_8_OE_LEN)-1)<<GLB_REG_GPIO_8_OE_POS))
#define GLB_REG_GPIO_9_OE                                       GLB_REG_GPIO_9_OE
#define GLB_REG_GPIO_9_OE_POS                                   (9U)
#define GLB_REG_GPIO_9_OE_LEN                                   (1U)
#define GLB_REG_GPIO_9_OE_MSK                                   (((1U<<GLB_REG_GPIO_9_OE_LEN)-1)<<GLB_REG_GPIO_9_OE_POS)
#define GLB_REG_GPIO_9_OE_UMSK                                  (~(((1U<<GLB_REG_GPIO_9_OE_LEN)-1)<<GLB_REG_GPIO_9_OE_POS))
#define GLB_REG_GPIO_10_OE                                      GLB_REG_GPIO_10_OE
#define GLB_REG_GPIO_10_OE_POS                                  (10U)
#define GLB_REG_GPIO_10_OE_LEN                                  (1U)
#define GLB_REG_GPIO_10_OE_MSK                                  (((1U<<GLB_REG_GPIO_10_OE_LEN)-1)<<GLB_REG_GPIO_10_OE_POS)
#define GLB_REG_GPIO_10_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_10_OE_LEN)-1)<<GLB_REG_GPIO_10_OE_POS))
#define GLB_REG_GPIO_11_OE                                      GLB_REG_GPIO_11_OE
#define GLB_REG_GPIO_11_OE_POS                                  (11U)
#define GLB_REG_GPIO_11_OE_LEN                                  (1U)
#define GLB_REG_GPIO_11_OE_MSK                                  (((1U<<GLB_REG_GPIO_11_OE_LEN)-1)<<GLB_REG_GPIO_11_OE_POS)
#define GLB_REG_GPIO_11_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_11_OE_LEN)-1)<<GLB_REG_GPIO_11_OE_POS))
#define GLB_REG_GPIO_12_OE                                      GLB_REG_GPIO_12_OE
#define GLB_REG_GPIO_12_OE_POS                                  (12U)
#define GLB_REG_GPIO_12_OE_LEN                                  (1U)
#define GLB_REG_GPIO_12_OE_MSK                                  (((1U<<GLB_REG_GPIO_12_OE_LEN)-1)<<GLB_REG_GPIO_12_OE_POS)
#define GLB_REG_GPIO_12_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_12_OE_LEN)-1)<<GLB_REG_GPIO_12_OE_POS))
#define GLB_REG_GPIO_13_OE                                      GLB_REG_GPIO_13_OE
#define GLB_REG_GPIO_13_OE_POS                                  (13U)
#define GLB_REG_GPIO_13_OE_LEN                                  (1U)
#define GLB_REG_GPIO_13_OE_MSK                                  (((1U<<GLB_REG_GPIO_13_OE_LEN)-1)<<GLB_REG_GPIO_13_OE_POS)
#define GLB_REG_GPIO_13_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_13_OE_LEN)-1)<<GLB_REG_GPIO_13_OE_POS))
#define GLB_REG_GPIO_14_OE                                      GLB_REG_GPIO_14_OE
#define GLB_REG_GPIO_14_OE_POS                                  (14U)
#define GLB_REG_GPIO_14_OE_LEN                                  (1U)
#define GLB_REG_GPIO_14_OE_MSK                                  (((1U<<GLB_REG_GPIO_14_OE_LEN)-1)<<GLB_REG_GPIO_14_OE_POS)
#define GLB_REG_GPIO_14_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_14_OE_LEN)-1)<<GLB_REG_GPIO_14_OE_POS))
#define GLB_REG_GPIO_15_OE                                      GLB_REG_GPIO_15_OE
#define GLB_REG_GPIO_15_OE_POS                                  (15U)
#define GLB_REG_GPIO_15_OE_LEN                                  (1U)
#define GLB_REG_GPIO_15_OE_MSK                                  (((1U<<GLB_REG_GPIO_15_OE_LEN)-1)<<GLB_REG_GPIO_15_OE_POS)
#define GLB_REG_GPIO_15_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_15_OE_LEN)-1)<<GLB_REG_GPIO_15_OE_POS))
#define GLB_REG_GPIO_16_OE                                      GLB_REG_GPIO_16_OE
#define GLB_REG_GPIO_16_OE_POS                                  (16U)
#define GLB_REG_GPIO_16_OE_LEN                                  (1U)
#define GLB_REG_GPIO_16_OE_MSK                                  (((1U<<GLB_REG_GPIO_16_OE_LEN)-1)<<GLB_REG_GPIO_16_OE_POS)
#define GLB_REG_GPIO_16_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_16_OE_LEN)-1)<<GLB_REG_GPIO_16_OE_POS))
#define GLB_REG_GPIO_17_OE                                      GLB_REG_GPIO_17_OE
#define GLB_REG_GPIO_17_OE_POS                                  (17U)
#define GLB_REG_GPIO_17_OE_LEN                                  (1U)
#define GLB_REG_GPIO_17_OE_MSK                                  (((1U<<GLB_REG_GPIO_17_OE_LEN)-1)<<GLB_REG_GPIO_17_OE_POS)
#define GLB_REG_GPIO_17_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_17_OE_LEN)-1)<<GLB_REG_GPIO_17_OE_POS))
#define GLB_REG_GPIO_18_OE                                      GLB_REG_GPIO_18_OE
#define GLB_REG_GPIO_18_OE_POS                                  (18U)
#define GLB_REG_GPIO_18_OE_LEN                                  (1U)
#define GLB_REG_GPIO_18_OE_MSK                                  (((1U<<GLB_REG_GPIO_18_OE_LEN)-1)<<GLB_REG_GPIO_18_OE_POS)
#define GLB_REG_GPIO_18_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_18_OE_LEN)-1)<<GLB_REG_GPIO_18_OE_POS))
#define GLB_REG_GPIO_19_OE                                      GLB_REG_GPIO_19_OE
#define GLB_REG_GPIO_19_OE_POS                                  (19U)
#define GLB_REG_GPIO_19_OE_LEN                                  (1U)
#define GLB_REG_GPIO_19_OE_MSK                                  (((1U<<GLB_REG_GPIO_19_OE_LEN)-1)<<GLB_REG_GPIO_19_OE_POS)
#define GLB_REG_GPIO_19_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_19_OE_LEN)-1)<<GLB_REG_GPIO_19_OE_POS))
#define GLB_REG_GPIO_20_OE                                      GLB_REG_GPIO_20_OE
#define GLB_REG_GPIO_20_OE_POS                                  (20U)
#define GLB_REG_GPIO_20_OE_LEN                                  (1U)
#define GLB_REG_GPIO_20_OE_MSK                                  (((1U<<GLB_REG_GPIO_20_OE_LEN)-1)<<GLB_REG_GPIO_20_OE_POS)
#define GLB_REG_GPIO_20_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_20_OE_LEN)-1)<<GLB_REG_GPIO_20_OE_POS))
#define GLB_REG_GPIO_21_OE                                      GLB_REG_GPIO_21_OE
#define GLB_REG_GPIO_21_OE_POS                                  (21U)
#define GLB_REG_GPIO_21_OE_LEN                                  (1U)
#define GLB_REG_GPIO_21_OE_MSK                                  (((1U<<GLB_REG_GPIO_21_OE_LEN)-1)<<GLB_REG_GPIO_21_OE_POS)
#define GLB_REG_GPIO_21_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_21_OE_LEN)-1)<<GLB_REG_GPIO_21_OE_POS))
#define GLB_REG_GPIO_22_OE                                      GLB_REG_GPIO_22_OE
#define GLB_REG_GPIO_22_OE_POS                                  (22U)
#define GLB_REG_GPIO_22_OE_LEN                                  (1U)
#define GLB_REG_GPIO_22_OE_MSK                                  (((1U<<GLB_REG_GPIO_22_OE_LEN)-1)<<GLB_REG_GPIO_22_OE_POS)
#define GLB_REG_GPIO_22_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_22_OE_LEN)-1)<<GLB_REG_GPIO_22_OE_POS))
#define GLB_REG_GPIO_23_OE                                      GLB_REG_GPIO_23_OE
#define GLB_REG_GPIO_23_OE_POS                                  (23U)
#define GLB_REG_GPIO_23_OE_LEN                                  (1U)
#define GLB_REG_GPIO_23_OE_MSK                                  (((1U<<GLB_REG_GPIO_23_OE_LEN)-1)<<GLB_REG_GPIO_23_OE_POS)
#define GLB_REG_GPIO_23_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_23_OE_LEN)-1)<<GLB_REG_GPIO_23_OE_POS))
#define GLB_REG_GPIO_24_OE                                      GLB_REG_GPIO_24_OE
#define GLB_REG_GPIO_24_OE_POS                                  (24U)
#define GLB_REG_GPIO_24_OE_LEN                                  (1U)
#define GLB_REG_GPIO_24_OE_MSK                                  (((1U<<GLB_REG_GPIO_24_OE_LEN)-1)<<GLB_REG_GPIO_24_OE_POS)
#define GLB_REG_GPIO_24_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_24_OE_LEN)-1)<<GLB_REG_GPIO_24_OE_POS))
#define GLB_REG_GPIO_25_OE                                      GLB_REG_GPIO_25_OE
#define GLB_REG_GPIO_25_OE_POS                                  (25U)
#define GLB_REG_GPIO_25_OE_LEN                                  (1U)
#define GLB_REG_GPIO_25_OE_MSK                                  (((1U<<GLB_REG_GPIO_25_OE_LEN)-1)<<GLB_REG_GPIO_25_OE_POS)
#define GLB_REG_GPIO_25_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_25_OE_LEN)-1)<<GLB_REG_GPIO_25_OE_POS))
#define GLB_REG_GPIO_26_OE                                      GLB_REG_GPIO_26_OE
#define GLB_REG_GPIO_26_OE_POS                                  (26U)
#define GLB_REG_GPIO_26_OE_LEN                                  (1U)
#define GLB_REG_GPIO_26_OE_MSK                                  (((1U<<GLB_REG_GPIO_26_OE_LEN)-1)<<GLB_REG_GPIO_26_OE_POS)
#define GLB_REG_GPIO_26_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_26_OE_LEN)-1)<<GLB_REG_GPIO_26_OE_POS))
#define GLB_REG_GPIO_27_OE                                      GLB_REG_GPIO_27_OE
#define GLB_REG_GPIO_27_OE_POS                                  (27U)
#define GLB_REG_GPIO_27_OE_LEN                                  (1U)
#define GLB_REG_GPIO_27_OE_MSK                                  (((1U<<GLB_REG_GPIO_27_OE_LEN)-1)<<GLB_REG_GPIO_27_OE_POS)
#define GLB_REG_GPIO_27_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_27_OE_LEN)-1)<<GLB_REG_GPIO_27_OE_POS))
#define GLB_REG_GPIO_28_OE                                      GLB_REG_GPIO_28_OE
#define GLB_REG_GPIO_28_OE_POS                                  (28U)
#define GLB_REG_GPIO_28_OE_LEN                                  (1U)
#define GLB_REG_GPIO_28_OE_MSK                                  (((1U<<GLB_REG_GPIO_28_OE_LEN)-1)<<GLB_REG_GPIO_28_OE_POS)
#define GLB_REG_GPIO_28_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_28_OE_LEN)-1)<<GLB_REG_GPIO_28_OE_POS))
#define GLB_REG_GPIO_29_OE                                      GLB_REG_GPIO_29_OE
#define GLB_REG_GPIO_29_OE_POS                                  (29U)
#define GLB_REG_GPIO_29_OE_LEN                                  (1U)
#define GLB_REG_GPIO_29_OE_MSK                                  (((1U<<GLB_REG_GPIO_29_OE_LEN)-1)<<GLB_REG_GPIO_29_OE_POS)
#define GLB_REG_GPIO_29_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_29_OE_LEN)-1)<<GLB_REG_GPIO_29_OE_POS))
#define GLB_REG_GPIO_30_OE                                      GLB_REG_GPIO_30_OE
#define GLB_REG_GPIO_30_OE_POS                                  (30U)
#define GLB_REG_GPIO_30_OE_LEN                                  (1U)
#define GLB_REG_GPIO_30_OE_MSK                                  (((1U<<GLB_REG_GPIO_30_OE_LEN)-1)<<GLB_REG_GPIO_30_OE_POS)
#define GLB_REG_GPIO_30_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_30_OE_LEN)-1)<<GLB_REG_GPIO_30_OE_POS))
#define GLB_REG_GPIO_31_OE                                      GLB_REG_GPIO_31_OE
#define GLB_REG_GPIO_31_OE_POS                                  (31U)
#define GLB_REG_GPIO_31_OE_LEN                                  (1U)
#define GLB_REG_GPIO_31_OE_MSK                                  (((1U<<GLB_REG_GPIO_31_OE_LEN)-1)<<GLB_REG_GPIO_31_OE_POS)
#define GLB_REG_GPIO_31_OE_UMSK                                 (~(((1U<<GLB_REG_GPIO_31_OE_LEN)-1)<<GLB_REG_GPIO_31_OE_POS))

/* 0x194 : GPIO_CFGCTL36 */
#define GLB_GPIO_CFGCTL36_OFFSET                                (0x194)
#define GLB_REG_GPIO_0_SET                                      GLB_REG_GPIO_0_SET
#define GLB_REG_GPIO_0_SET_POS                                  (0U)
#define GLB_REG_GPIO_0_SET_LEN                                  (1U)
#define GLB_REG_GPIO_0_SET_MSK                                  (((1U<<GLB_REG_GPIO_0_SET_LEN)-1)<<GLB_REG_GPIO_0_SET_POS)
#define GLB_REG_GPIO_0_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_SET_LEN)-1)<<GLB_REG_GPIO_0_SET_POS))
#define GLB_REG_GPIO_1_SET                                      GLB_REG_GPIO_1_SET
#define GLB_REG_GPIO_1_SET_POS                                  (1U)
#define GLB_REG_GPIO_1_SET_LEN                                  (1U)
#define GLB_REG_GPIO_1_SET_MSK                                  (((1U<<GLB_REG_GPIO_1_SET_LEN)-1)<<GLB_REG_GPIO_1_SET_POS)
#define GLB_REG_GPIO_1_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_SET_LEN)-1)<<GLB_REG_GPIO_1_SET_POS))
#define GLB_REG_GPIO_2_SET                                      GLB_REG_GPIO_2_SET
#define GLB_REG_GPIO_2_SET_POS                                  (2U)
#define GLB_REG_GPIO_2_SET_LEN                                  (1U)
#define GLB_REG_GPIO_2_SET_MSK                                  (((1U<<GLB_REG_GPIO_2_SET_LEN)-1)<<GLB_REG_GPIO_2_SET_POS)
#define GLB_REG_GPIO_2_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_SET_LEN)-1)<<GLB_REG_GPIO_2_SET_POS))
#define GLB_REG_GPIO_3_SET                                      GLB_REG_GPIO_3_SET
#define GLB_REG_GPIO_3_SET_POS                                  (3U)
#define GLB_REG_GPIO_3_SET_LEN                                  (1U)
#define GLB_REG_GPIO_3_SET_MSK                                  (((1U<<GLB_REG_GPIO_3_SET_LEN)-1)<<GLB_REG_GPIO_3_SET_POS)
#define GLB_REG_GPIO_3_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_SET_LEN)-1)<<GLB_REG_GPIO_3_SET_POS))
#define GLB_REG_GPIO_4_SET                                      GLB_REG_GPIO_4_SET
#define GLB_REG_GPIO_4_SET_POS                                  (4U)
#define GLB_REG_GPIO_4_SET_LEN                                  (1U)
#define GLB_REG_GPIO_4_SET_MSK                                  (((1U<<GLB_REG_GPIO_4_SET_LEN)-1)<<GLB_REG_GPIO_4_SET_POS)
#define GLB_REG_GPIO_4_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_4_SET_LEN)-1)<<GLB_REG_GPIO_4_SET_POS))
#define GLB_REG_GPIO_5_SET                                      GLB_REG_GPIO_5_SET
#define GLB_REG_GPIO_5_SET_POS                                  (5U)
#define GLB_REG_GPIO_5_SET_LEN                                  (1U)
#define GLB_REG_GPIO_5_SET_MSK                                  (((1U<<GLB_REG_GPIO_5_SET_LEN)-1)<<GLB_REG_GPIO_5_SET_POS)
#define GLB_REG_GPIO_5_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_5_SET_LEN)-1)<<GLB_REG_GPIO_5_SET_POS))
#define GLB_REG_GPIO_6_SET                                      GLB_REG_GPIO_6_SET
#define GLB_REG_GPIO_6_SET_POS                                  (6U)
#define GLB_REG_GPIO_6_SET_LEN                                  (1U)
#define GLB_REG_GPIO_6_SET_MSK                                  (((1U<<GLB_REG_GPIO_6_SET_LEN)-1)<<GLB_REG_GPIO_6_SET_POS)
#define GLB_REG_GPIO_6_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_6_SET_LEN)-1)<<GLB_REG_GPIO_6_SET_POS))
#define GLB_REG_GPIO_7_SET                                      GLB_REG_GPIO_7_SET
#define GLB_REG_GPIO_7_SET_POS                                  (7U)
#define GLB_REG_GPIO_7_SET_LEN                                  (1U)
#define GLB_REG_GPIO_7_SET_MSK                                  (((1U<<GLB_REG_GPIO_7_SET_LEN)-1)<<GLB_REG_GPIO_7_SET_POS)
#define GLB_REG_GPIO_7_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_SET_LEN)-1)<<GLB_REG_GPIO_7_SET_POS))
#define GLB_REG_GPIO_8_SET                                      GLB_REG_GPIO_8_SET
#define GLB_REG_GPIO_8_SET_POS                                  (8U)
#define GLB_REG_GPIO_8_SET_LEN                                  (1U)
#define GLB_REG_GPIO_8_SET_MSK                                  (((1U<<GLB_REG_GPIO_8_SET_LEN)-1)<<GLB_REG_GPIO_8_SET_POS)
#define GLB_REG_GPIO_8_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_SET_LEN)-1)<<GLB_REG_GPIO_8_SET_POS))
#define GLB_REG_GPIO_9_SET                                      GLB_REG_GPIO_9_SET
#define GLB_REG_GPIO_9_SET_POS                                  (9U)
#define GLB_REG_GPIO_9_SET_LEN                                  (1U)
#define GLB_REG_GPIO_9_SET_MSK                                  (((1U<<GLB_REG_GPIO_9_SET_LEN)-1)<<GLB_REG_GPIO_9_SET_POS)
#define GLB_REG_GPIO_9_SET_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_SET_LEN)-1)<<GLB_REG_GPIO_9_SET_POS))
#define GLB_REG_GPIO_10_SET                                     GLB_REG_GPIO_10_SET
#define GLB_REG_GPIO_10_SET_POS                                 (10U)
#define GLB_REG_GPIO_10_SET_LEN                                 (1U)
#define GLB_REG_GPIO_10_SET_MSK                                 (((1U<<GLB_REG_GPIO_10_SET_LEN)-1)<<GLB_REG_GPIO_10_SET_POS)
#define GLB_REG_GPIO_10_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_10_SET_LEN)-1)<<GLB_REG_GPIO_10_SET_POS))
#define GLB_REG_GPIO_11_SET                                     GLB_REG_GPIO_11_SET
#define GLB_REG_GPIO_11_SET_POS                                 (11U)
#define GLB_REG_GPIO_11_SET_LEN                                 (1U)
#define GLB_REG_GPIO_11_SET_MSK                                 (((1U<<GLB_REG_GPIO_11_SET_LEN)-1)<<GLB_REG_GPIO_11_SET_POS)
#define GLB_REG_GPIO_11_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_11_SET_LEN)-1)<<GLB_REG_GPIO_11_SET_POS))
#define GLB_REG_GPIO_12_SET                                     GLB_REG_GPIO_12_SET
#define GLB_REG_GPIO_12_SET_POS                                 (12U)
#define GLB_REG_GPIO_12_SET_LEN                                 (1U)
#define GLB_REG_GPIO_12_SET_MSK                                 (((1U<<GLB_REG_GPIO_12_SET_LEN)-1)<<GLB_REG_GPIO_12_SET_POS)
#define GLB_REG_GPIO_12_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_12_SET_LEN)-1)<<GLB_REG_GPIO_12_SET_POS))
#define GLB_REG_GPIO_13_SET                                     GLB_REG_GPIO_13_SET
#define GLB_REG_GPIO_13_SET_POS                                 (13U)
#define GLB_REG_GPIO_13_SET_LEN                                 (1U)
#define GLB_REG_GPIO_13_SET_MSK                                 (((1U<<GLB_REG_GPIO_13_SET_LEN)-1)<<GLB_REG_GPIO_13_SET_POS)
#define GLB_REG_GPIO_13_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_13_SET_LEN)-1)<<GLB_REG_GPIO_13_SET_POS))
#define GLB_REG_GPIO_14_SET                                     GLB_REG_GPIO_14_SET
#define GLB_REG_GPIO_14_SET_POS                                 (14U)
#define GLB_REG_GPIO_14_SET_LEN                                 (1U)
#define GLB_REG_GPIO_14_SET_MSK                                 (((1U<<GLB_REG_GPIO_14_SET_LEN)-1)<<GLB_REG_GPIO_14_SET_POS)
#define GLB_REG_GPIO_14_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_14_SET_LEN)-1)<<GLB_REG_GPIO_14_SET_POS))
#define GLB_REG_GPIO_15_SET                                     GLB_REG_GPIO_15_SET
#define GLB_REG_GPIO_15_SET_POS                                 (15U)
#define GLB_REG_GPIO_15_SET_LEN                                 (1U)
#define GLB_REG_GPIO_15_SET_MSK                                 (((1U<<GLB_REG_GPIO_15_SET_LEN)-1)<<GLB_REG_GPIO_15_SET_POS)
#define GLB_REG_GPIO_15_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_15_SET_LEN)-1)<<GLB_REG_GPIO_15_SET_POS))
#define GLB_REG_GPIO_16_SET                                     GLB_REG_GPIO_16_SET
#define GLB_REG_GPIO_16_SET_POS                                 (16U)
#define GLB_REG_GPIO_16_SET_LEN                                 (1U)
#define GLB_REG_GPIO_16_SET_MSK                                 (((1U<<GLB_REG_GPIO_16_SET_LEN)-1)<<GLB_REG_GPIO_16_SET_POS)
#define GLB_REG_GPIO_16_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_16_SET_LEN)-1)<<GLB_REG_GPIO_16_SET_POS))
#define GLB_REG_GPIO_17_SET                                     GLB_REG_GPIO_17_SET
#define GLB_REG_GPIO_17_SET_POS                                 (17U)
#define GLB_REG_GPIO_17_SET_LEN                                 (1U)
#define GLB_REG_GPIO_17_SET_MSK                                 (((1U<<GLB_REG_GPIO_17_SET_LEN)-1)<<GLB_REG_GPIO_17_SET_POS)
#define GLB_REG_GPIO_17_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_17_SET_LEN)-1)<<GLB_REG_GPIO_17_SET_POS))
#define GLB_REG_GPIO_18_SET                                     GLB_REG_GPIO_18_SET
#define GLB_REG_GPIO_18_SET_POS                                 (18U)
#define GLB_REG_GPIO_18_SET_LEN                                 (1U)
#define GLB_REG_GPIO_18_SET_MSK                                 (((1U<<GLB_REG_GPIO_18_SET_LEN)-1)<<GLB_REG_GPIO_18_SET_POS)
#define GLB_REG_GPIO_18_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_18_SET_LEN)-1)<<GLB_REG_GPIO_18_SET_POS))
#define GLB_REG_GPIO_19_SET                                     GLB_REG_GPIO_19_SET
#define GLB_REG_GPIO_19_SET_POS                                 (19U)
#define GLB_REG_GPIO_19_SET_LEN                                 (1U)
#define GLB_REG_GPIO_19_SET_MSK                                 (((1U<<GLB_REG_GPIO_19_SET_LEN)-1)<<GLB_REG_GPIO_19_SET_POS)
#define GLB_REG_GPIO_19_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_19_SET_LEN)-1)<<GLB_REG_GPIO_19_SET_POS))
#define GLB_REG_GPIO_20_SET                                     GLB_REG_GPIO_20_SET
#define GLB_REG_GPIO_20_SET_POS                                 (20U)
#define GLB_REG_GPIO_20_SET_LEN                                 (1U)
#define GLB_REG_GPIO_20_SET_MSK                                 (((1U<<GLB_REG_GPIO_20_SET_LEN)-1)<<GLB_REG_GPIO_20_SET_POS)
#define GLB_REG_GPIO_20_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_20_SET_LEN)-1)<<GLB_REG_GPIO_20_SET_POS))
#define GLB_REG_GPIO_21_SET                                     GLB_REG_GPIO_21_SET
#define GLB_REG_GPIO_21_SET_POS                                 (21U)
#define GLB_REG_GPIO_21_SET_LEN                                 (1U)
#define GLB_REG_GPIO_21_SET_MSK                                 (((1U<<GLB_REG_GPIO_21_SET_LEN)-1)<<GLB_REG_GPIO_21_SET_POS)
#define GLB_REG_GPIO_21_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_21_SET_LEN)-1)<<GLB_REG_GPIO_21_SET_POS))
#define GLB_REG_GPIO_22_SET                                     GLB_REG_GPIO_22_SET
#define GLB_REG_GPIO_22_SET_POS                                 (22U)
#define GLB_REG_GPIO_22_SET_LEN                                 (1U)
#define GLB_REG_GPIO_22_SET_MSK                                 (((1U<<GLB_REG_GPIO_22_SET_LEN)-1)<<GLB_REG_GPIO_22_SET_POS)
#define GLB_REG_GPIO_22_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_22_SET_LEN)-1)<<GLB_REG_GPIO_22_SET_POS))
#define GLB_REG_GPIO_23_SET                                     GLB_REG_GPIO_23_SET
#define GLB_REG_GPIO_23_SET_POS                                 (23U)
#define GLB_REG_GPIO_23_SET_LEN                                 (1U)
#define GLB_REG_GPIO_23_SET_MSK                                 (((1U<<GLB_REG_GPIO_23_SET_LEN)-1)<<GLB_REG_GPIO_23_SET_POS)
#define GLB_REG_GPIO_23_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_23_SET_LEN)-1)<<GLB_REG_GPIO_23_SET_POS))
#define GLB_REG_GPIO_24_SET                                     GLB_REG_GPIO_24_SET
#define GLB_REG_GPIO_24_SET_POS                                 (24U)
#define GLB_REG_GPIO_24_SET_LEN                                 (1U)
#define GLB_REG_GPIO_24_SET_MSK                                 (((1U<<GLB_REG_GPIO_24_SET_LEN)-1)<<GLB_REG_GPIO_24_SET_POS)
#define GLB_REG_GPIO_24_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_24_SET_LEN)-1)<<GLB_REG_GPIO_24_SET_POS))
#define GLB_REG_GPIO_25_SET                                     GLB_REG_GPIO_25_SET
#define GLB_REG_GPIO_25_SET_POS                                 (25U)
#define GLB_REG_GPIO_25_SET_LEN                                 (1U)
#define GLB_REG_GPIO_25_SET_MSK                                 (((1U<<GLB_REG_GPIO_25_SET_LEN)-1)<<GLB_REG_GPIO_25_SET_POS)
#define GLB_REG_GPIO_25_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_25_SET_LEN)-1)<<GLB_REG_GPIO_25_SET_POS))
#define GLB_REG_GPIO_26_SET                                     GLB_REG_GPIO_26_SET
#define GLB_REG_GPIO_26_SET_POS                                 (26U)
#define GLB_REG_GPIO_26_SET_LEN                                 (1U)
#define GLB_REG_GPIO_26_SET_MSK                                 (((1U<<GLB_REG_GPIO_26_SET_LEN)-1)<<GLB_REG_GPIO_26_SET_POS)
#define GLB_REG_GPIO_26_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_26_SET_LEN)-1)<<GLB_REG_GPIO_26_SET_POS))
#define GLB_REG_GPIO_27_SET                                     GLB_REG_GPIO_27_SET
#define GLB_REG_GPIO_27_SET_POS                                 (27U)
#define GLB_REG_GPIO_27_SET_LEN                                 (1U)
#define GLB_REG_GPIO_27_SET_MSK                                 (((1U<<GLB_REG_GPIO_27_SET_LEN)-1)<<GLB_REG_GPIO_27_SET_POS)
#define GLB_REG_GPIO_27_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_27_SET_LEN)-1)<<GLB_REG_GPIO_27_SET_POS))
#define GLB_REG_GPIO_28_SET                                     GLB_REG_GPIO_28_SET
#define GLB_REG_GPIO_28_SET_POS                                 (28U)
#define GLB_REG_GPIO_28_SET_LEN                                 (1U)
#define GLB_REG_GPIO_28_SET_MSK                                 (((1U<<GLB_REG_GPIO_28_SET_LEN)-1)<<GLB_REG_GPIO_28_SET_POS)
#define GLB_REG_GPIO_28_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_28_SET_LEN)-1)<<GLB_REG_GPIO_28_SET_POS))
#define GLB_REG_GPIO_29_SET                                     GLB_REG_GPIO_29_SET
#define GLB_REG_GPIO_29_SET_POS                                 (29U)
#define GLB_REG_GPIO_29_SET_LEN                                 (1U)
#define GLB_REG_GPIO_29_SET_MSK                                 (((1U<<GLB_REG_GPIO_29_SET_LEN)-1)<<GLB_REG_GPIO_29_SET_POS)
#define GLB_REG_GPIO_29_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_29_SET_LEN)-1)<<GLB_REG_GPIO_29_SET_POS))
#define GLB_REG_GPIO_30_SET                                     GLB_REG_GPIO_30_SET
#define GLB_REG_GPIO_30_SET_POS                                 (30U)
#define GLB_REG_GPIO_30_SET_LEN                                 (1U)
#define GLB_REG_GPIO_30_SET_MSK                                 (((1U<<GLB_REG_GPIO_30_SET_LEN)-1)<<GLB_REG_GPIO_30_SET_POS)
#define GLB_REG_GPIO_30_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_30_SET_LEN)-1)<<GLB_REG_GPIO_30_SET_POS))
#define GLB_REG_GPIO_31_SET                                     GLB_REG_GPIO_31_SET
#define GLB_REG_GPIO_31_SET_POS                                 (31U)
#define GLB_REG_GPIO_31_SET_LEN                                 (1U)
#define GLB_REG_GPIO_31_SET_MSK                                 (((1U<<GLB_REG_GPIO_31_SET_LEN)-1)<<GLB_REG_GPIO_31_SET_POS)
#define GLB_REG_GPIO_31_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_31_SET_LEN)-1)<<GLB_REG_GPIO_31_SET_POS))

/* 0x198 : GPIO_CFGCTL37 */
#define GLB_GPIO_CFGCTL37_OFFSET                                (0x198)
#define GLB_REG_GPIO_0_CLR                                      GLB_REG_GPIO_0_CLR
#define GLB_REG_GPIO_0_CLR_POS                                  (0U)
#define GLB_REG_GPIO_0_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_0_CLR_MSK                                  (((1U<<GLB_REG_GPIO_0_CLR_LEN)-1)<<GLB_REG_GPIO_0_CLR_POS)
#define GLB_REG_GPIO_0_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_0_CLR_LEN)-1)<<GLB_REG_GPIO_0_CLR_POS))
#define GLB_REG_GPIO_1_CLR                                      GLB_REG_GPIO_1_CLR
#define GLB_REG_GPIO_1_CLR_POS                                  (1U)
#define GLB_REG_GPIO_1_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_1_CLR_MSK                                  (((1U<<GLB_REG_GPIO_1_CLR_LEN)-1)<<GLB_REG_GPIO_1_CLR_POS)
#define GLB_REG_GPIO_1_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_1_CLR_LEN)-1)<<GLB_REG_GPIO_1_CLR_POS))
#define GLB_REG_GPIO_2_CLR                                      GLB_REG_GPIO_2_CLR
#define GLB_REG_GPIO_2_CLR_POS                                  (2U)
#define GLB_REG_GPIO_2_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_2_CLR_MSK                                  (((1U<<GLB_REG_GPIO_2_CLR_LEN)-1)<<GLB_REG_GPIO_2_CLR_POS)
#define GLB_REG_GPIO_2_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_2_CLR_LEN)-1)<<GLB_REG_GPIO_2_CLR_POS))
#define GLB_REG_GPIO_3_CLR                                      GLB_REG_GPIO_3_CLR
#define GLB_REG_GPIO_3_CLR_POS                                  (3U)
#define GLB_REG_GPIO_3_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_3_CLR_MSK                                  (((1U<<GLB_REG_GPIO_3_CLR_LEN)-1)<<GLB_REG_GPIO_3_CLR_POS)
#define GLB_REG_GPIO_3_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_3_CLR_LEN)-1)<<GLB_REG_GPIO_3_CLR_POS))
#define GLB_REG_GPIO_4_CLR                                      GLB_REG_GPIO_4_CLR
#define GLB_REG_GPIO_4_CLR_POS                                  (4U)
#define GLB_REG_GPIO_4_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_4_CLR_MSK                                  (((1U<<GLB_REG_GPIO_4_CLR_LEN)-1)<<GLB_REG_GPIO_4_CLR_POS)
#define GLB_REG_GPIO_4_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_4_CLR_LEN)-1)<<GLB_REG_GPIO_4_CLR_POS))
#define GLB_REG_GPIO_5_CLR                                      GLB_REG_GPIO_5_CLR
#define GLB_REG_GPIO_5_CLR_POS                                  (5U)
#define GLB_REG_GPIO_5_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_5_CLR_MSK                                  (((1U<<GLB_REG_GPIO_5_CLR_LEN)-1)<<GLB_REG_GPIO_5_CLR_POS)
#define GLB_REG_GPIO_5_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_5_CLR_LEN)-1)<<GLB_REG_GPIO_5_CLR_POS))
#define GLB_REG_GPIO_6_CLR                                      GLB_REG_GPIO_6_CLR
#define GLB_REG_GPIO_6_CLR_POS                                  (6U)
#define GLB_REG_GPIO_6_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_6_CLR_MSK                                  (((1U<<GLB_REG_GPIO_6_CLR_LEN)-1)<<GLB_REG_GPIO_6_CLR_POS)
#define GLB_REG_GPIO_6_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_6_CLR_LEN)-1)<<GLB_REG_GPIO_6_CLR_POS))
#define GLB_REG_GPIO_7_CLR                                      GLB_REG_GPIO_7_CLR
#define GLB_REG_GPIO_7_CLR_POS                                  (7U)
#define GLB_REG_GPIO_7_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_7_CLR_MSK                                  (((1U<<GLB_REG_GPIO_7_CLR_LEN)-1)<<GLB_REG_GPIO_7_CLR_POS)
#define GLB_REG_GPIO_7_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_7_CLR_LEN)-1)<<GLB_REG_GPIO_7_CLR_POS))
#define GLB_REG_GPIO_8_CLR                                      GLB_REG_GPIO_8_CLR
#define GLB_REG_GPIO_8_CLR_POS                                  (8U)
#define GLB_REG_GPIO_8_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_8_CLR_MSK                                  (((1U<<GLB_REG_GPIO_8_CLR_LEN)-1)<<GLB_REG_GPIO_8_CLR_POS)
#define GLB_REG_GPIO_8_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_8_CLR_LEN)-1)<<GLB_REG_GPIO_8_CLR_POS))
#define GLB_REG_GPIO_9_CLR                                      GLB_REG_GPIO_9_CLR
#define GLB_REG_GPIO_9_CLR_POS                                  (9U)
#define GLB_REG_GPIO_9_CLR_LEN                                  (1U)
#define GLB_REG_GPIO_9_CLR_MSK                                  (((1U<<GLB_REG_GPIO_9_CLR_LEN)-1)<<GLB_REG_GPIO_9_CLR_POS)
#define GLB_REG_GPIO_9_CLR_UMSK                                 (~(((1U<<GLB_REG_GPIO_9_CLR_LEN)-1)<<GLB_REG_GPIO_9_CLR_POS))
#define GLB_REG_GPIO_10_CLR                                     GLB_REG_GPIO_10_CLR
#define GLB_REG_GPIO_10_CLR_POS                                 (10U)
#define GLB_REG_GPIO_10_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_10_CLR_MSK                                 (((1U<<GLB_REG_GPIO_10_CLR_LEN)-1)<<GLB_REG_GPIO_10_CLR_POS)
#define GLB_REG_GPIO_10_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_10_CLR_LEN)-1)<<GLB_REG_GPIO_10_CLR_POS))
#define GLB_REG_GPIO_11_CLR                                     GLB_REG_GPIO_11_CLR
#define GLB_REG_GPIO_11_CLR_POS                                 (11U)
#define GLB_REG_GPIO_11_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_11_CLR_MSK                                 (((1U<<GLB_REG_GPIO_11_CLR_LEN)-1)<<GLB_REG_GPIO_11_CLR_POS)
#define GLB_REG_GPIO_11_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_11_CLR_LEN)-1)<<GLB_REG_GPIO_11_CLR_POS))
#define GLB_REG_GPIO_12_CLR                                     GLB_REG_GPIO_12_CLR
#define GLB_REG_GPIO_12_CLR_POS                                 (12U)
#define GLB_REG_GPIO_12_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_12_CLR_MSK                                 (((1U<<GLB_REG_GPIO_12_CLR_LEN)-1)<<GLB_REG_GPIO_12_CLR_POS)
#define GLB_REG_GPIO_12_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_12_CLR_LEN)-1)<<GLB_REG_GPIO_12_CLR_POS))
#define GLB_REG_GPIO_13_CLR                                     GLB_REG_GPIO_13_CLR
#define GLB_REG_GPIO_13_CLR_POS                                 (13U)
#define GLB_REG_GPIO_13_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_13_CLR_MSK                                 (((1U<<GLB_REG_GPIO_13_CLR_LEN)-1)<<GLB_REG_GPIO_13_CLR_POS)
#define GLB_REG_GPIO_13_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_13_CLR_LEN)-1)<<GLB_REG_GPIO_13_CLR_POS))
#define GLB_REG_GPIO_14_CLR                                     GLB_REG_GPIO_14_CLR
#define GLB_REG_GPIO_14_CLR_POS                                 (14U)
#define GLB_REG_GPIO_14_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_14_CLR_MSK                                 (((1U<<GLB_REG_GPIO_14_CLR_LEN)-1)<<GLB_REG_GPIO_14_CLR_POS)
#define GLB_REG_GPIO_14_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_14_CLR_LEN)-1)<<GLB_REG_GPIO_14_CLR_POS))
#define GLB_REG_GPIO_15_CLR                                     GLB_REG_GPIO_15_CLR
#define GLB_REG_GPIO_15_CLR_POS                                 (15U)
#define GLB_REG_GPIO_15_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_15_CLR_MSK                                 (((1U<<GLB_REG_GPIO_15_CLR_LEN)-1)<<GLB_REG_GPIO_15_CLR_POS)
#define GLB_REG_GPIO_15_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_15_CLR_LEN)-1)<<GLB_REG_GPIO_15_CLR_POS))
#define GLB_REG_GPIO_16_CLR                                     GLB_REG_GPIO_16_CLR
#define GLB_REG_GPIO_16_CLR_POS                                 (16U)
#define GLB_REG_GPIO_16_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_16_CLR_MSK                                 (((1U<<GLB_REG_GPIO_16_CLR_LEN)-1)<<GLB_REG_GPIO_16_CLR_POS)
#define GLB_REG_GPIO_16_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_16_CLR_LEN)-1)<<GLB_REG_GPIO_16_CLR_POS))
#define GLB_REG_GPIO_17_CLR                                     GLB_REG_GPIO_17_CLR
#define GLB_REG_GPIO_17_CLR_POS                                 (17U)
#define GLB_REG_GPIO_17_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_17_CLR_MSK                                 (((1U<<GLB_REG_GPIO_17_CLR_LEN)-1)<<GLB_REG_GPIO_17_CLR_POS)
#define GLB_REG_GPIO_17_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_17_CLR_LEN)-1)<<GLB_REG_GPIO_17_CLR_POS))
#define GLB_REG_GPIO_18_CLR                                     GLB_REG_GPIO_18_CLR
#define GLB_REG_GPIO_18_CLR_POS                                 (18U)
#define GLB_REG_GPIO_18_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_18_CLR_MSK                                 (((1U<<GLB_REG_GPIO_18_CLR_LEN)-1)<<GLB_REG_GPIO_18_CLR_POS)
#define GLB_REG_GPIO_18_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_18_CLR_LEN)-1)<<GLB_REG_GPIO_18_CLR_POS))
#define GLB_REG_GPIO_19_CLR                                     GLB_REG_GPIO_19_CLR
#define GLB_REG_GPIO_19_CLR_POS                                 (19U)
#define GLB_REG_GPIO_19_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_19_CLR_MSK                                 (((1U<<GLB_REG_GPIO_19_CLR_LEN)-1)<<GLB_REG_GPIO_19_CLR_POS)
#define GLB_REG_GPIO_19_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_19_CLR_LEN)-1)<<GLB_REG_GPIO_19_CLR_POS))
#define GLB_REG_GPIO_20_CLR                                     GLB_REG_GPIO_20_CLR
#define GLB_REG_GPIO_20_CLR_POS                                 (20U)
#define GLB_REG_GPIO_20_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_20_CLR_MSK                                 (((1U<<GLB_REG_GPIO_20_CLR_LEN)-1)<<GLB_REG_GPIO_20_CLR_POS)
#define GLB_REG_GPIO_20_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_20_CLR_LEN)-1)<<GLB_REG_GPIO_20_CLR_POS))
#define GLB_REG_GPIO_21_CLR                                     GLB_REG_GPIO_21_CLR
#define GLB_REG_GPIO_21_CLR_POS                                 (21U)
#define GLB_REG_GPIO_21_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_21_CLR_MSK                                 (((1U<<GLB_REG_GPIO_21_CLR_LEN)-1)<<GLB_REG_GPIO_21_CLR_POS)
#define GLB_REG_GPIO_21_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_21_CLR_LEN)-1)<<GLB_REG_GPIO_21_CLR_POS))
#define GLB_REG_GPIO_22_CLR                                     GLB_REG_GPIO_22_CLR
#define GLB_REG_GPIO_22_CLR_POS                                 (22U)
#define GLB_REG_GPIO_22_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_22_CLR_MSK                                 (((1U<<GLB_REG_GPIO_22_CLR_LEN)-1)<<GLB_REG_GPIO_22_CLR_POS)
#define GLB_REG_GPIO_22_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_22_CLR_LEN)-1)<<GLB_REG_GPIO_22_CLR_POS))
#define GLB_REG_GPIO_23_CLR                                     GLB_REG_GPIO_23_CLR
#define GLB_REG_GPIO_23_CLR_POS                                 (23U)
#define GLB_REG_GPIO_23_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_23_CLR_MSK                                 (((1U<<GLB_REG_GPIO_23_CLR_LEN)-1)<<GLB_REG_GPIO_23_CLR_POS)
#define GLB_REG_GPIO_23_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_23_CLR_LEN)-1)<<GLB_REG_GPIO_23_CLR_POS))
#define GLB_REG_GPIO_24_CLR                                     GLB_REG_GPIO_24_CLR
#define GLB_REG_GPIO_24_CLR_POS                                 (24U)
#define GLB_REG_GPIO_24_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_24_CLR_MSK                                 (((1U<<GLB_REG_GPIO_24_CLR_LEN)-1)<<GLB_REG_GPIO_24_CLR_POS)
#define GLB_REG_GPIO_24_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_24_CLR_LEN)-1)<<GLB_REG_GPIO_24_CLR_POS))
#define GLB_REG_GPIO_25_CLR                                     GLB_REG_GPIO_25_CLR
#define GLB_REG_GPIO_25_CLR_POS                                 (25U)
#define GLB_REG_GPIO_25_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_25_CLR_MSK                                 (((1U<<GLB_REG_GPIO_25_CLR_LEN)-1)<<GLB_REG_GPIO_25_CLR_POS)
#define GLB_REG_GPIO_25_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_25_CLR_LEN)-1)<<GLB_REG_GPIO_25_CLR_POS))
#define GLB_REG_GPIO_26_CLR                                     GLB_REG_GPIO_26_CLR
#define GLB_REG_GPIO_26_CLR_POS                                 (26U)
#define GLB_REG_GPIO_26_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_26_CLR_MSK                                 (((1U<<GLB_REG_GPIO_26_CLR_LEN)-1)<<GLB_REG_GPIO_26_CLR_POS)
#define GLB_REG_GPIO_26_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_26_CLR_LEN)-1)<<GLB_REG_GPIO_26_CLR_POS))
#define GLB_REG_GPIO_27_CLR                                     GLB_REG_GPIO_27_CLR
#define GLB_REG_GPIO_27_CLR_POS                                 (27U)
#define GLB_REG_GPIO_27_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_27_CLR_MSK                                 (((1U<<GLB_REG_GPIO_27_CLR_LEN)-1)<<GLB_REG_GPIO_27_CLR_POS)
#define GLB_REG_GPIO_27_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_27_CLR_LEN)-1)<<GLB_REG_GPIO_27_CLR_POS))
#define GLB_REG_GPIO_28_CLR                                     GLB_REG_GPIO_28_CLR
#define GLB_REG_GPIO_28_CLR_POS                                 (28U)
#define GLB_REG_GPIO_28_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_28_CLR_MSK                                 (((1U<<GLB_REG_GPIO_28_CLR_LEN)-1)<<GLB_REG_GPIO_28_CLR_POS)
#define GLB_REG_GPIO_28_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_28_CLR_LEN)-1)<<GLB_REG_GPIO_28_CLR_POS))
#define GLB_REG_GPIO_29_CLR                                     GLB_REG_GPIO_29_CLR
#define GLB_REG_GPIO_29_CLR_POS                                 (29U)
#define GLB_REG_GPIO_29_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_29_CLR_MSK                                 (((1U<<GLB_REG_GPIO_29_CLR_LEN)-1)<<GLB_REG_GPIO_29_CLR_POS)
#define GLB_REG_GPIO_29_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_29_CLR_LEN)-1)<<GLB_REG_GPIO_29_CLR_POS))
#define GLB_REG_GPIO_30_CLR                                     GLB_REG_GPIO_30_CLR
#define GLB_REG_GPIO_30_CLR_POS                                 (30U)
#define GLB_REG_GPIO_30_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_30_CLR_MSK                                 (((1U<<GLB_REG_GPIO_30_CLR_LEN)-1)<<GLB_REG_GPIO_30_CLR_POS)
#define GLB_REG_GPIO_30_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_30_CLR_LEN)-1)<<GLB_REG_GPIO_30_CLR_POS))
#define GLB_REG_GPIO_31_CLR                                     GLB_REG_GPIO_31_CLR
#define GLB_REG_GPIO_31_CLR_POS                                 (31U)
#define GLB_REG_GPIO_31_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_31_CLR_MSK                                 (((1U<<GLB_REG_GPIO_31_CLR_LEN)-1)<<GLB_REG_GPIO_31_CLR_POS)
#define GLB_REG_GPIO_31_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_31_CLR_LEN)-1)<<GLB_REG_GPIO_31_CLR_POS))

/* 0x19C : GPIO_CFGCTL38 */
#define GLB_GPIO_CFGCTL38_OFFSET                                (0x19C)
#define GLB_REG_GPIO_32_SET                                     GLB_REG_GPIO_32_SET
#define GLB_REG_GPIO_32_SET_POS                                 (0U)
#define GLB_REG_GPIO_32_SET_LEN                                 (1U)
#define GLB_REG_GPIO_32_SET_MSK                                 (((1U<<GLB_REG_GPIO_32_SET_LEN)-1)<<GLB_REG_GPIO_32_SET_POS)
#define GLB_REG_GPIO_32_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_32_SET_LEN)-1)<<GLB_REG_GPIO_32_SET_POS))
#define GLB_REG_GPIO_33_SET                                     GLB_REG_GPIO_33_SET
#define GLB_REG_GPIO_33_SET_POS                                 (1U)
#define GLB_REG_GPIO_33_SET_LEN                                 (1U)
#define GLB_REG_GPIO_33_SET_MSK                                 (((1U<<GLB_REG_GPIO_33_SET_LEN)-1)<<GLB_REG_GPIO_33_SET_POS)
#define GLB_REG_GPIO_33_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_33_SET_LEN)-1)<<GLB_REG_GPIO_33_SET_POS))
#define GLB_REG_GPIO_34_SET                                     GLB_REG_GPIO_34_SET
#define GLB_REG_GPIO_34_SET_POS                                 (2U)
#define GLB_REG_GPIO_34_SET_LEN                                 (1U)
#define GLB_REG_GPIO_34_SET_MSK                                 (((1U<<GLB_REG_GPIO_34_SET_LEN)-1)<<GLB_REG_GPIO_34_SET_POS)
#define GLB_REG_GPIO_34_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_34_SET_LEN)-1)<<GLB_REG_GPIO_34_SET_POS))
#define GLB_REG_GPIO_35_SET                                     GLB_REG_GPIO_35_SET
#define GLB_REG_GPIO_35_SET_POS                                 (3U)
#define GLB_REG_GPIO_35_SET_LEN                                 (1U)
#define GLB_REG_GPIO_35_SET_MSK                                 (((1U<<GLB_REG_GPIO_35_SET_LEN)-1)<<GLB_REG_GPIO_35_SET_POS)
#define GLB_REG_GPIO_35_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_35_SET_LEN)-1)<<GLB_REG_GPIO_35_SET_POS))
#define GLB_REG_GPIO_36_SET                                     GLB_REG_GPIO_36_SET
#define GLB_REG_GPIO_36_SET_POS                                 (4U)
#define GLB_REG_GPIO_36_SET_LEN                                 (1U)
#define GLB_REG_GPIO_36_SET_MSK                                 (((1U<<GLB_REG_GPIO_36_SET_LEN)-1)<<GLB_REG_GPIO_36_SET_POS)
#define GLB_REG_GPIO_36_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_36_SET_LEN)-1)<<GLB_REG_GPIO_36_SET_POS))
#define GLB_REG_GPIO_37_SET                                     GLB_REG_GPIO_37_SET
#define GLB_REG_GPIO_37_SET_POS                                 (5U)
#define GLB_REG_GPIO_37_SET_LEN                                 (1U)
#define GLB_REG_GPIO_37_SET_MSK                                 (((1U<<GLB_REG_GPIO_37_SET_LEN)-1)<<GLB_REG_GPIO_37_SET_POS)
#define GLB_REG_GPIO_37_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_37_SET_LEN)-1)<<GLB_REG_GPIO_37_SET_POS))
#define GLB_REG_GPIO_38_SET                                     GLB_REG_GPIO_38_SET
#define GLB_REG_GPIO_38_SET_POS                                 (6U)
#define GLB_REG_GPIO_38_SET_LEN                                 (1U)
#define GLB_REG_GPIO_38_SET_MSK                                 (((1U<<GLB_REG_GPIO_38_SET_LEN)-1)<<GLB_REG_GPIO_38_SET_POS)
#define GLB_REG_GPIO_38_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_38_SET_LEN)-1)<<GLB_REG_GPIO_38_SET_POS))
#define GLB_REG_GPIO_39_SET                                     GLB_REG_GPIO_39_SET
#define GLB_REG_GPIO_39_SET_POS                                 (7U)
#define GLB_REG_GPIO_39_SET_LEN                                 (1U)
#define GLB_REG_GPIO_39_SET_MSK                                 (((1U<<GLB_REG_GPIO_39_SET_LEN)-1)<<GLB_REG_GPIO_39_SET_POS)
#define GLB_REG_GPIO_39_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_39_SET_LEN)-1)<<GLB_REG_GPIO_39_SET_POS))
#define GLB_REG_GPIO_40_SET                                     GLB_REG_GPIO_40_SET
#define GLB_REG_GPIO_40_SET_POS                                 (8U)
#define GLB_REG_GPIO_40_SET_LEN                                 (1U)
#define GLB_REG_GPIO_40_SET_MSK                                 (((1U<<GLB_REG_GPIO_40_SET_LEN)-1)<<GLB_REG_GPIO_40_SET_POS)
#define GLB_REG_GPIO_40_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_40_SET_LEN)-1)<<GLB_REG_GPIO_40_SET_POS))
#define GLB_REG_GPIO_41_SET                                     GLB_REG_GPIO_41_SET
#define GLB_REG_GPIO_41_SET_POS                                 (9U)
#define GLB_REG_GPIO_41_SET_LEN                                 (1U)
#define GLB_REG_GPIO_41_SET_MSK                                 (((1U<<GLB_REG_GPIO_41_SET_LEN)-1)<<GLB_REG_GPIO_41_SET_POS)
#define GLB_REG_GPIO_41_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_41_SET_LEN)-1)<<GLB_REG_GPIO_41_SET_POS))
#define GLB_REG_GPIO_42_SET                                     GLB_REG_GPIO_42_SET
#define GLB_REG_GPIO_42_SET_POS                                 (10U)
#define GLB_REG_GPIO_42_SET_LEN                                 (1U)
#define GLB_REG_GPIO_42_SET_MSK                                 (((1U<<GLB_REG_GPIO_42_SET_LEN)-1)<<GLB_REG_GPIO_42_SET_POS)
#define GLB_REG_GPIO_42_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_42_SET_LEN)-1)<<GLB_REG_GPIO_42_SET_POS))
#define GLB_REG_GPIO_43_SET                                     GLB_REG_GPIO_43_SET
#define GLB_REG_GPIO_43_SET_POS                                 (11U)
#define GLB_REG_GPIO_43_SET_LEN                                 (1U)
#define GLB_REG_GPIO_43_SET_MSK                                 (((1U<<GLB_REG_GPIO_43_SET_LEN)-1)<<GLB_REG_GPIO_43_SET_POS)
#define GLB_REG_GPIO_43_SET_UMSK                                (~(((1U<<GLB_REG_GPIO_43_SET_LEN)-1)<<GLB_REG_GPIO_43_SET_POS))
#define GLB_REG_GPIO_32_CLR                                     GLB_REG_GPIO_32_CLR
#define GLB_REG_GPIO_32_CLR_POS                                 (12U)
#define GLB_REG_GPIO_32_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_32_CLR_MSK                                 (((1U<<GLB_REG_GPIO_32_CLR_LEN)-1)<<GLB_REG_GPIO_32_CLR_POS)
#define GLB_REG_GPIO_32_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_32_CLR_LEN)-1)<<GLB_REG_GPIO_32_CLR_POS))
#define GLB_REG_GPIO_33_CLR                                     GLB_REG_GPIO_33_CLR
#define GLB_REG_GPIO_33_CLR_POS                                 (13U)
#define GLB_REG_GPIO_33_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_33_CLR_MSK                                 (((1U<<GLB_REG_GPIO_33_CLR_LEN)-1)<<GLB_REG_GPIO_33_CLR_POS)
#define GLB_REG_GPIO_33_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_33_CLR_LEN)-1)<<GLB_REG_GPIO_33_CLR_POS))
#define GLB_REG_GPIO_34_CLR                                     GLB_REG_GPIO_34_CLR
#define GLB_REG_GPIO_34_CLR_POS                                 (14U)
#define GLB_REG_GPIO_34_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_34_CLR_MSK                                 (((1U<<GLB_REG_GPIO_34_CLR_LEN)-1)<<GLB_REG_GPIO_34_CLR_POS)
#define GLB_REG_GPIO_34_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_34_CLR_LEN)-1)<<GLB_REG_GPIO_34_CLR_POS))
#define GLB_REG_GPIO_35_CLR                                     GLB_REG_GPIO_35_CLR
#define GLB_REG_GPIO_35_CLR_POS                                 (15U)
#define GLB_REG_GPIO_35_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_35_CLR_MSK                                 (((1U<<GLB_REG_GPIO_35_CLR_LEN)-1)<<GLB_REG_GPIO_35_CLR_POS)
#define GLB_REG_GPIO_35_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_35_CLR_LEN)-1)<<GLB_REG_GPIO_35_CLR_POS))
#define GLB_REG_GPIO_36_CLR                                     GLB_REG_GPIO_36_CLR
#define GLB_REG_GPIO_36_CLR_POS                                 (16U)
#define GLB_REG_GPIO_36_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_36_CLR_MSK                                 (((1U<<GLB_REG_GPIO_36_CLR_LEN)-1)<<GLB_REG_GPIO_36_CLR_POS)
#define GLB_REG_GPIO_36_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_36_CLR_LEN)-1)<<GLB_REG_GPIO_36_CLR_POS))
#define GLB_REG_GPIO_37_CLR                                     GLB_REG_GPIO_37_CLR
#define GLB_REG_GPIO_37_CLR_POS                                 (17U)
#define GLB_REG_GPIO_37_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_37_CLR_MSK                                 (((1U<<GLB_REG_GPIO_37_CLR_LEN)-1)<<GLB_REG_GPIO_37_CLR_POS)
#define GLB_REG_GPIO_37_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_37_CLR_LEN)-1)<<GLB_REG_GPIO_37_CLR_POS))
#define GLB_REG_GPIO_38_CLR                                     GLB_REG_GPIO_38_CLR
#define GLB_REG_GPIO_38_CLR_POS                                 (18U)
#define GLB_REG_GPIO_38_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_38_CLR_MSK                                 (((1U<<GLB_REG_GPIO_38_CLR_LEN)-1)<<GLB_REG_GPIO_38_CLR_POS)
#define GLB_REG_GPIO_38_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_38_CLR_LEN)-1)<<GLB_REG_GPIO_38_CLR_POS))
#define GLB_REG_GPIO_39_CLR                                     GLB_REG_GPIO_39_CLR
#define GLB_REG_GPIO_39_CLR_POS                                 (19U)
#define GLB_REG_GPIO_39_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_39_CLR_MSK                                 (((1U<<GLB_REG_GPIO_39_CLR_LEN)-1)<<GLB_REG_GPIO_39_CLR_POS)
#define GLB_REG_GPIO_39_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_39_CLR_LEN)-1)<<GLB_REG_GPIO_39_CLR_POS))
#define GLB_REG_GPIO_40_CLR                                     GLB_REG_GPIO_40_CLR
#define GLB_REG_GPIO_40_CLR_POS                                 (20U)
#define GLB_REG_GPIO_40_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_40_CLR_MSK                                 (((1U<<GLB_REG_GPIO_40_CLR_LEN)-1)<<GLB_REG_GPIO_40_CLR_POS)
#define GLB_REG_GPIO_40_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_40_CLR_LEN)-1)<<GLB_REG_GPIO_40_CLR_POS))
#define GLB_REG_GPIO_41_CLR                                     GLB_REG_GPIO_41_CLR
#define GLB_REG_GPIO_41_CLR_POS                                 (21U)
#define GLB_REG_GPIO_41_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_41_CLR_MSK                                 (((1U<<GLB_REG_GPIO_41_CLR_LEN)-1)<<GLB_REG_GPIO_41_CLR_POS)
#define GLB_REG_GPIO_41_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_41_CLR_LEN)-1)<<GLB_REG_GPIO_41_CLR_POS))
#define GLB_REG_GPIO_42_CLR                                     GLB_REG_GPIO_42_CLR
#define GLB_REG_GPIO_42_CLR_POS                                 (22U)
#define GLB_REG_GPIO_42_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_42_CLR_MSK                                 (((1U<<GLB_REG_GPIO_42_CLR_LEN)-1)<<GLB_REG_GPIO_42_CLR_POS)
#define GLB_REG_GPIO_42_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_42_CLR_LEN)-1)<<GLB_REG_GPIO_42_CLR_POS))
#define GLB_REG_GPIO_43_CLR                                     GLB_REG_GPIO_43_CLR
#define GLB_REG_GPIO_43_CLR_POS                                 (23U)
#define GLB_REG_GPIO_43_CLR_LEN                                 (1U)
#define GLB_REG_GPIO_43_CLR_MSK                                 (((1U<<GLB_REG_GPIO_43_CLR_LEN)-1)<<GLB_REG_GPIO_43_CLR_POS)
#define GLB_REG_GPIO_43_CLR_UMSK                                (~(((1U<<GLB_REG_GPIO_43_CLR_LEN)-1)<<GLB_REG_GPIO_43_CLR_POS))

/* 0x1A0 : GPIO_INT_MASK1 */
#define GLB_GPIO_INT_MASK1_OFFSET                               (0x1A0)
#define GLB_REG_GPIO0_INT_MASK                                  GLB_REG_GPIO0_INT_MASK
#define GLB_REG_GPIO0_INT_MASK_POS                              (0U)
#define GLB_REG_GPIO0_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO0_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO0_INT_MASK_LEN)-1)<<GLB_REG_GPIO0_INT_MASK_POS)
#define GLB_REG_GPIO0_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO0_INT_MASK_LEN)-1)<<GLB_REG_GPIO0_INT_MASK_POS))
#define GLB_REG_GPIO1_INT_MASK                                  GLB_REG_GPIO1_INT_MASK
#define GLB_REG_GPIO1_INT_MASK_POS                              (1U)
#define GLB_REG_GPIO1_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO1_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO1_INT_MASK_LEN)-1)<<GLB_REG_GPIO1_INT_MASK_POS)
#define GLB_REG_GPIO1_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO1_INT_MASK_LEN)-1)<<GLB_REG_GPIO1_INT_MASK_POS))
#define GLB_REG_GPIO2_INT_MASK                                  GLB_REG_GPIO2_INT_MASK
#define GLB_REG_GPIO2_INT_MASK_POS                              (2U)
#define GLB_REG_GPIO2_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO2_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO2_INT_MASK_LEN)-1)<<GLB_REG_GPIO2_INT_MASK_POS)
#define GLB_REG_GPIO2_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO2_INT_MASK_LEN)-1)<<GLB_REG_GPIO2_INT_MASK_POS))
#define GLB_REG_GPIO3_INT_MASK                                  GLB_REG_GPIO3_INT_MASK
#define GLB_REG_GPIO3_INT_MASK_POS                              (3U)
#define GLB_REG_GPIO3_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO3_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO3_INT_MASK_LEN)-1)<<GLB_REG_GPIO3_INT_MASK_POS)
#define GLB_REG_GPIO3_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO3_INT_MASK_LEN)-1)<<GLB_REG_GPIO3_INT_MASK_POS))
#define GLB_REG_GPIO4_INT_MASK                                  GLB_REG_GPIO4_INT_MASK
#define GLB_REG_GPIO4_INT_MASK_POS                              (4U)
#define GLB_REG_GPIO4_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO4_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO4_INT_MASK_LEN)-1)<<GLB_REG_GPIO4_INT_MASK_POS)
#define GLB_REG_GPIO4_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO4_INT_MASK_LEN)-1)<<GLB_REG_GPIO4_INT_MASK_POS))
#define GLB_REG_GPIO5_INT_MASK                                  GLB_REG_GPIO5_INT_MASK
#define GLB_REG_GPIO5_INT_MASK_POS                              (5U)
#define GLB_REG_GPIO5_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO5_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO5_INT_MASK_LEN)-1)<<GLB_REG_GPIO5_INT_MASK_POS)
#define GLB_REG_GPIO5_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO5_INT_MASK_LEN)-1)<<GLB_REG_GPIO5_INT_MASK_POS))
#define GLB_REG_GPIO6_INT_MASK                                  GLB_REG_GPIO6_INT_MASK
#define GLB_REG_GPIO6_INT_MASK_POS                              (6U)
#define GLB_REG_GPIO6_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO6_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO6_INT_MASK_LEN)-1)<<GLB_REG_GPIO6_INT_MASK_POS)
#define GLB_REG_GPIO6_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO6_INT_MASK_LEN)-1)<<GLB_REG_GPIO6_INT_MASK_POS))
#define GLB_REG_GPIO7_INT_MASK                                  GLB_REG_GPIO7_INT_MASK
#define GLB_REG_GPIO7_INT_MASK_POS                              (7U)
#define GLB_REG_GPIO7_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO7_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO7_INT_MASK_LEN)-1)<<GLB_REG_GPIO7_INT_MASK_POS)
#define GLB_REG_GPIO7_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO7_INT_MASK_LEN)-1)<<GLB_REG_GPIO7_INT_MASK_POS))
#define GLB_REG_GPIO8_INT_MASK                                  GLB_REG_GPIO8_INT_MASK
#define GLB_REG_GPIO8_INT_MASK_POS                              (8U)
#define GLB_REG_GPIO8_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO8_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO8_INT_MASK_LEN)-1)<<GLB_REG_GPIO8_INT_MASK_POS)
#define GLB_REG_GPIO8_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO8_INT_MASK_LEN)-1)<<GLB_REG_GPIO8_INT_MASK_POS))
#define GLB_REG_GPIO9_INT_MASK                                  GLB_REG_GPIO9_INT_MASK
#define GLB_REG_GPIO9_INT_MASK_POS                              (9U)
#define GLB_REG_GPIO9_INT_MASK_LEN                              (1U)
#define GLB_REG_GPIO9_INT_MASK_MSK                              (((1U<<GLB_REG_GPIO9_INT_MASK_LEN)-1)<<GLB_REG_GPIO9_INT_MASK_POS)
#define GLB_REG_GPIO9_INT_MASK_UMSK                             (~(((1U<<GLB_REG_GPIO9_INT_MASK_LEN)-1)<<GLB_REG_GPIO9_INT_MASK_POS))
#define GLB_REG_GPIO10_INT_MASK                                 GLB_REG_GPIO10_INT_MASK
#define GLB_REG_GPIO10_INT_MASK_POS                             (10U)
#define GLB_REG_GPIO10_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO10_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO10_INT_MASK_LEN)-1)<<GLB_REG_GPIO10_INT_MASK_POS)
#define GLB_REG_GPIO10_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO10_INT_MASK_LEN)-1)<<GLB_REG_GPIO10_INT_MASK_POS))
#define GLB_REG_GPIO11_INT_MASK                                 GLB_REG_GPIO11_INT_MASK
#define GLB_REG_GPIO11_INT_MASK_POS                             (11U)
#define GLB_REG_GPIO11_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO11_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO11_INT_MASK_LEN)-1)<<GLB_REG_GPIO11_INT_MASK_POS)
#define GLB_REG_GPIO11_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO11_INT_MASK_LEN)-1)<<GLB_REG_GPIO11_INT_MASK_POS))
#define GLB_REG_GPIO12_INT_MASK                                 GLB_REG_GPIO12_INT_MASK
#define GLB_REG_GPIO12_INT_MASK_POS                             (12U)
#define GLB_REG_GPIO12_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO12_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO12_INT_MASK_LEN)-1)<<GLB_REG_GPIO12_INT_MASK_POS)
#define GLB_REG_GPIO12_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO12_INT_MASK_LEN)-1)<<GLB_REG_GPIO12_INT_MASK_POS))
#define GLB_REG_GPIO13_INT_MASK                                 GLB_REG_GPIO13_INT_MASK
#define GLB_REG_GPIO13_INT_MASK_POS                             (13U)
#define GLB_REG_GPIO13_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO13_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO13_INT_MASK_LEN)-1)<<GLB_REG_GPIO13_INT_MASK_POS)
#define GLB_REG_GPIO13_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO13_INT_MASK_LEN)-1)<<GLB_REG_GPIO13_INT_MASK_POS))
#define GLB_REG_GPIO14_INT_MASK                                 GLB_REG_GPIO14_INT_MASK
#define GLB_REG_GPIO14_INT_MASK_POS                             (14U)
#define GLB_REG_GPIO14_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO14_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO14_INT_MASK_LEN)-1)<<GLB_REG_GPIO14_INT_MASK_POS)
#define GLB_REG_GPIO14_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO14_INT_MASK_LEN)-1)<<GLB_REG_GPIO14_INT_MASK_POS))
#define GLB_REG_GPIO15_INT_MASK                                 GLB_REG_GPIO15_INT_MASK
#define GLB_REG_GPIO15_INT_MASK_POS                             (15U)
#define GLB_REG_GPIO15_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO15_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO15_INT_MASK_LEN)-1)<<GLB_REG_GPIO15_INT_MASK_POS)
#define GLB_REG_GPIO15_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO15_INT_MASK_LEN)-1)<<GLB_REG_GPIO15_INT_MASK_POS))
#define GLB_REG_GPIO16_INT_MASK                                 GLB_REG_GPIO16_INT_MASK
#define GLB_REG_GPIO16_INT_MASK_POS                             (16U)
#define GLB_REG_GPIO16_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO16_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO16_INT_MASK_LEN)-1)<<GLB_REG_GPIO16_INT_MASK_POS)
#define GLB_REG_GPIO16_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO16_INT_MASK_LEN)-1)<<GLB_REG_GPIO16_INT_MASK_POS))
#define GLB_REG_GPIO17_INT_MASK                                 GLB_REG_GPIO17_INT_MASK
#define GLB_REG_GPIO17_INT_MASK_POS                             (17U)
#define GLB_REG_GPIO17_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO17_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO17_INT_MASK_LEN)-1)<<GLB_REG_GPIO17_INT_MASK_POS)
#define GLB_REG_GPIO17_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO17_INT_MASK_LEN)-1)<<GLB_REG_GPIO17_INT_MASK_POS))
#define GLB_REG_GPIO18_INT_MASK                                 GLB_REG_GPIO18_INT_MASK
#define GLB_REG_GPIO18_INT_MASK_POS                             (18U)
#define GLB_REG_GPIO18_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO18_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO18_INT_MASK_LEN)-1)<<GLB_REG_GPIO18_INT_MASK_POS)
#define GLB_REG_GPIO18_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO18_INT_MASK_LEN)-1)<<GLB_REG_GPIO18_INT_MASK_POS))
#define GLB_REG_GPIO19_INT_MASK                                 GLB_REG_GPIO19_INT_MASK
#define GLB_REG_GPIO19_INT_MASK_POS                             (19U)
#define GLB_REG_GPIO19_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO19_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO19_INT_MASK_LEN)-1)<<GLB_REG_GPIO19_INT_MASK_POS)
#define GLB_REG_GPIO19_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO19_INT_MASK_LEN)-1)<<GLB_REG_GPIO19_INT_MASK_POS))
#define GLB_REG_GPIO20_INT_MASK                                 GLB_REG_GPIO20_INT_MASK
#define GLB_REG_GPIO20_INT_MASK_POS                             (20U)
#define GLB_REG_GPIO20_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO20_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO20_INT_MASK_LEN)-1)<<GLB_REG_GPIO20_INT_MASK_POS)
#define GLB_REG_GPIO20_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO20_INT_MASK_LEN)-1)<<GLB_REG_GPIO20_INT_MASK_POS))
#define GLB_REG_GPIO21_INT_MASK                                 GLB_REG_GPIO21_INT_MASK
#define GLB_REG_GPIO21_INT_MASK_POS                             (21U)
#define GLB_REG_GPIO21_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO21_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO21_INT_MASK_LEN)-1)<<GLB_REG_GPIO21_INT_MASK_POS)
#define GLB_REG_GPIO21_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO21_INT_MASK_LEN)-1)<<GLB_REG_GPIO21_INT_MASK_POS))
#define GLB_REG_GPIO22_INT_MASK                                 GLB_REG_GPIO22_INT_MASK
#define GLB_REG_GPIO22_INT_MASK_POS                             (22U)
#define GLB_REG_GPIO22_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO22_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO22_INT_MASK_LEN)-1)<<GLB_REG_GPIO22_INT_MASK_POS)
#define GLB_REG_GPIO22_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO22_INT_MASK_LEN)-1)<<GLB_REG_GPIO22_INT_MASK_POS))
#define GLB_REG_GPIO23_INT_MASK                                 GLB_REG_GPIO23_INT_MASK
#define GLB_REG_GPIO23_INT_MASK_POS                             (23U)
#define GLB_REG_GPIO23_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO23_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO23_INT_MASK_LEN)-1)<<GLB_REG_GPIO23_INT_MASK_POS)
#define GLB_REG_GPIO23_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO23_INT_MASK_LEN)-1)<<GLB_REG_GPIO23_INT_MASK_POS))
#define GLB_REG_GPIO24_INT_MASK                                 GLB_REG_GPIO24_INT_MASK
#define GLB_REG_GPIO24_INT_MASK_POS                             (24U)
#define GLB_REG_GPIO24_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO24_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO24_INT_MASK_LEN)-1)<<GLB_REG_GPIO24_INT_MASK_POS)
#define GLB_REG_GPIO24_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO24_INT_MASK_LEN)-1)<<GLB_REG_GPIO24_INT_MASK_POS))
#define GLB_REG_GPIO25_INT_MASK                                 GLB_REG_GPIO25_INT_MASK
#define GLB_REG_GPIO25_INT_MASK_POS                             (25U)
#define GLB_REG_GPIO25_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO25_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO25_INT_MASK_LEN)-1)<<GLB_REG_GPIO25_INT_MASK_POS)
#define GLB_REG_GPIO25_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO25_INT_MASK_LEN)-1)<<GLB_REG_GPIO25_INT_MASK_POS))
#define GLB_REG_GPIO26_INT_MASK                                 GLB_REG_GPIO26_INT_MASK
#define GLB_REG_GPIO26_INT_MASK_POS                             (26U)
#define GLB_REG_GPIO26_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO26_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO26_INT_MASK_LEN)-1)<<GLB_REG_GPIO26_INT_MASK_POS)
#define GLB_REG_GPIO26_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO26_INT_MASK_LEN)-1)<<GLB_REG_GPIO26_INT_MASK_POS))
#define GLB_REG_GPIO27_INT_MASK                                 GLB_REG_GPIO27_INT_MASK
#define GLB_REG_GPIO27_INT_MASK_POS                             (27U)
#define GLB_REG_GPIO27_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO27_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO27_INT_MASK_LEN)-1)<<GLB_REG_GPIO27_INT_MASK_POS)
#define GLB_REG_GPIO27_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO27_INT_MASK_LEN)-1)<<GLB_REG_GPIO27_INT_MASK_POS))
#define GLB_REG_GPIO28_INT_MASK                                 GLB_REG_GPIO28_INT_MASK
#define GLB_REG_GPIO28_INT_MASK_POS                             (28U)
#define GLB_REG_GPIO28_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO28_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO28_INT_MASK_LEN)-1)<<GLB_REG_GPIO28_INT_MASK_POS)
#define GLB_REG_GPIO28_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO28_INT_MASK_LEN)-1)<<GLB_REG_GPIO28_INT_MASK_POS))
#define GLB_REG_GPIO29_INT_MASK                                 GLB_REG_GPIO29_INT_MASK
#define GLB_REG_GPIO29_INT_MASK_POS                             (29U)
#define GLB_REG_GPIO29_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO29_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO29_INT_MASK_LEN)-1)<<GLB_REG_GPIO29_INT_MASK_POS)
#define GLB_REG_GPIO29_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO29_INT_MASK_LEN)-1)<<GLB_REG_GPIO29_INT_MASK_POS))
#define GLB_REG_GPIO30_INT_MASK                                 GLB_REG_GPIO30_INT_MASK
#define GLB_REG_GPIO30_INT_MASK_POS                             (30U)
#define GLB_REG_GPIO30_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO30_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO30_INT_MASK_LEN)-1)<<GLB_REG_GPIO30_INT_MASK_POS)
#define GLB_REG_GPIO30_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO30_INT_MASK_LEN)-1)<<GLB_REG_GPIO30_INT_MASK_POS))
#define GLB_REG_GPIO31_INT_MASK                                 GLB_REG_GPIO31_INT_MASK
#define GLB_REG_GPIO31_INT_MASK_POS                             (31U)
#define GLB_REG_GPIO31_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO31_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO31_INT_MASK_LEN)-1)<<GLB_REG_GPIO31_INT_MASK_POS)
#define GLB_REG_GPIO31_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO31_INT_MASK_LEN)-1)<<GLB_REG_GPIO31_INT_MASK_POS))

/* 0x1A4 : GPIO_INT_MASK2 */
#define GLB_GPIO_INT_MASK2_OFFSET                               (0x1A4)
#define GLB_REG_GPIO_32_MASK                                    GLB_REG_GPIO_32_MASK
#define GLB_REG_GPIO_32_MASK_POS                                (0U)
#define GLB_REG_GPIO_32_MASK_LEN                                (1U)
#define GLB_REG_GPIO_32_MASK_MSK                                (((1U<<GLB_REG_GPIO_32_MASK_LEN)-1)<<GLB_REG_GPIO_32_MASK_POS)
#define GLB_REG_GPIO_32_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_32_MASK_LEN)-1)<<GLB_REG_GPIO_32_MASK_POS))
#define GLB_REG_GPIO_33_MASK                                    GLB_REG_GPIO_33_MASK
#define GLB_REG_GPIO_33_MASK_POS                                (1U)
#define GLB_REG_GPIO_33_MASK_LEN                                (1U)
#define GLB_REG_GPIO_33_MASK_MSK                                (((1U<<GLB_REG_GPIO_33_MASK_LEN)-1)<<GLB_REG_GPIO_33_MASK_POS)
#define GLB_REG_GPIO_33_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_33_MASK_LEN)-1)<<GLB_REG_GPIO_33_MASK_POS))
#define GLB_REG_GPIO_34_MASK                                    GLB_REG_GPIO_34_MASK
#define GLB_REG_GPIO_34_MASK_POS                                (2U)
#define GLB_REG_GPIO_34_MASK_LEN                                (1U)
#define GLB_REG_GPIO_34_MASK_MSK                                (((1U<<GLB_REG_GPIO_34_MASK_LEN)-1)<<GLB_REG_GPIO_34_MASK_POS)
#define GLB_REG_GPIO_34_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_34_MASK_LEN)-1)<<GLB_REG_GPIO_34_MASK_POS))
#define GLB_REG_GPIO_35_MASK                                    GLB_REG_GPIO_35_MASK
#define GLB_REG_GPIO_35_MASK_POS                                (3U)
#define GLB_REG_GPIO_35_MASK_LEN                                (1U)
#define GLB_REG_GPIO_35_MASK_MSK                                (((1U<<GLB_REG_GPIO_35_MASK_LEN)-1)<<GLB_REG_GPIO_35_MASK_POS)
#define GLB_REG_GPIO_35_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_35_MASK_LEN)-1)<<GLB_REG_GPIO_35_MASK_POS))
#define GLB_REG_GPIO_36_MASK                                    GLB_REG_GPIO_36_MASK
#define GLB_REG_GPIO_36_MASK_POS                                (4U)
#define GLB_REG_GPIO_36_MASK_LEN                                (1U)
#define GLB_REG_GPIO_36_MASK_MSK                                (((1U<<GLB_REG_GPIO_36_MASK_LEN)-1)<<GLB_REG_GPIO_36_MASK_POS)
#define GLB_REG_GPIO_36_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_36_MASK_LEN)-1)<<GLB_REG_GPIO_36_MASK_POS))
#define GLB_REG_GPIO_37_MASK                                    GLB_REG_GPIO_37_MASK
#define GLB_REG_GPIO_37_MASK_POS                                (5U)
#define GLB_REG_GPIO_37_MASK_LEN                                (1U)
#define GLB_REG_GPIO_37_MASK_MSK                                (((1U<<GLB_REG_GPIO_37_MASK_LEN)-1)<<GLB_REG_GPIO_37_MASK_POS)
#define GLB_REG_GPIO_37_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_37_MASK_LEN)-1)<<GLB_REG_GPIO_37_MASK_POS))
#define GLB_REG_GPIO_38_MASK                                    GLB_REG_GPIO_38_MASK
#define GLB_REG_GPIO_38_MASK_POS                                (6U)
#define GLB_REG_GPIO_38_MASK_LEN                                (1U)
#define GLB_REG_GPIO_38_MASK_MSK                                (((1U<<GLB_REG_GPIO_38_MASK_LEN)-1)<<GLB_REG_GPIO_38_MASK_POS)
#define GLB_REG_GPIO_38_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_38_MASK_LEN)-1)<<GLB_REG_GPIO_38_MASK_POS))
#define GLB_REG_GPIO_39_MASK                                    GLB_REG_GPIO_39_MASK
#define GLB_REG_GPIO_39_MASK_POS                                (7U)
#define GLB_REG_GPIO_39_MASK_LEN                                (1U)
#define GLB_REG_GPIO_39_MASK_MSK                                (((1U<<GLB_REG_GPIO_39_MASK_LEN)-1)<<GLB_REG_GPIO_39_MASK_POS)
#define GLB_REG_GPIO_39_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_39_MASK_LEN)-1)<<GLB_REG_GPIO_39_MASK_POS))
#define GLB_REG_GPIO_40_MASK                                    GLB_REG_GPIO_40_MASK
#define GLB_REG_GPIO_40_MASK_POS                                (8U)
#define GLB_REG_GPIO_40_MASK_LEN                                (1U)
#define GLB_REG_GPIO_40_MASK_MSK                                (((1U<<GLB_REG_GPIO_40_MASK_LEN)-1)<<GLB_REG_GPIO_40_MASK_POS)
#define GLB_REG_GPIO_40_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_40_MASK_LEN)-1)<<GLB_REG_GPIO_40_MASK_POS))
#define GLB_REG_GPIO_41_MASK                                    GLB_REG_GPIO_41_MASK
#define GLB_REG_GPIO_41_MASK_POS                                (9U)
#define GLB_REG_GPIO_41_MASK_LEN                                (1U)
#define GLB_REG_GPIO_41_MASK_MSK                                (((1U<<GLB_REG_GPIO_41_MASK_LEN)-1)<<GLB_REG_GPIO_41_MASK_POS)
#define GLB_REG_GPIO_41_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_41_MASK_LEN)-1)<<GLB_REG_GPIO_41_MASK_POS))
#define GLB_REG_GPIO_42_MASK                                    GLB_REG_GPIO_42_MASK
#define GLB_REG_GPIO_42_MASK_POS                                (10U)
#define GLB_REG_GPIO_42_MASK_LEN                                (1U)
#define GLB_REG_GPIO_42_MASK_MSK                                (((1U<<GLB_REG_GPIO_42_MASK_LEN)-1)<<GLB_REG_GPIO_42_MASK_POS)
#define GLB_REG_GPIO_42_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_42_MASK_LEN)-1)<<GLB_REG_GPIO_42_MASK_POS))
#define GLB_REG_GPIO_43_MASK                                    GLB_REG_GPIO_43_MASK
#define GLB_REG_GPIO_43_MASK_POS                                (11U)
#define GLB_REG_GPIO_43_MASK_LEN                                (1U)
#define GLB_REG_GPIO_43_MASK_MSK                                (((1U<<GLB_REG_GPIO_43_MASK_LEN)-1)<<GLB_REG_GPIO_43_MASK_POS)
#define GLB_REG_GPIO_43_MASK_UMSK                               (~(((1U<<GLB_REG_GPIO_43_MASK_LEN)-1)<<GLB_REG_GPIO_43_MASK_POS))

/* 0x1A8 : GPIO_INT_STAT1 */
#define GLB_GPIO_INT_STAT1_OFFSET                               (0x1A8)
#define GLB_GPIO_INT_STAT1                                      GLB_GPIO_INT_STAT1
#define GLB_GPIO_INT_STAT1_POS                                  (0U)
#define GLB_GPIO_INT_STAT1_LEN                                  (32U)
#define GLB_GPIO_INT_STAT1_MSK                                  (((1U<<GLB_GPIO_INT_STAT1_LEN)-1)<<GLB_GPIO_INT_STAT1_POS)
#define GLB_GPIO_INT_STAT1_UMSK                                 (~(((1U<<GLB_GPIO_INT_STAT1_LEN)-1)<<GLB_GPIO_INT_STAT1_POS))

/* 0x1AC : GPIO_INT_MASK3 */
#define GLB_GPIO_INT_MASK3_OFFSET                               (0x1AC)
#define GLB_REG_GPIO32_INT_MASK                                 GLB_REG_GPIO32_INT_MASK
#define GLB_REG_GPIO32_INT_MASK_POS                             (0U)
#define GLB_REG_GPIO32_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO32_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO32_INT_MASK_LEN)-1)<<GLB_REG_GPIO32_INT_MASK_POS)
#define GLB_REG_GPIO32_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO32_INT_MASK_LEN)-1)<<GLB_REG_GPIO32_INT_MASK_POS))
#define GLB_REG_GPIO33_INT_MASK                                 GLB_REG_GPIO33_INT_MASK
#define GLB_REG_GPIO33_INT_MASK_POS                             (1U)
#define GLB_REG_GPIO33_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO33_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO33_INT_MASK_LEN)-1)<<GLB_REG_GPIO33_INT_MASK_POS)
#define GLB_REG_GPIO33_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO33_INT_MASK_LEN)-1)<<GLB_REG_GPIO33_INT_MASK_POS))
#define GLB_REG_GPIO34_INT_MASK                                 GLB_REG_GPIO34_INT_MASK
#define GLB_REG_GPIO34_INT_MASK_POS                             (2U)
#define GLB_REG_GPIO34_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO34_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO34_INT_MASK_LEN)-1)<<GLB_REG_GPIO34_INT_MASK_POS)
#define GLB_REG_GPIO34_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO34_INT_MASK_LEN)-1)<<GLB_REG_GPIO34_INT_MASK_POS))
#define GLB_REG_GPIO35_INT_MASK                                 GLB_REG_GPIO35_INT_MASK
#define GLB_REG_GPIO35_INT_MASK_POS                             (3U)
#define GLB_REG_GPIO35_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO35_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO35_INT_MASK_LEN)-1)<<GLB_REG_GPIO35_INT_MASK_POS)
#define GLB_REG_GPIO35_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO35_INT_MASK_LEN)-1)<<GLB_REG_GPIO35_INT_MASK_POS))
#define GLB_REG_GPIO36_INT_MASK                                 GLB_REG_GPIO36_INT_MASK
#define GLB_REG_GPIO36_INT_MASK_POS                             (4U)
#define GLB_REG_GPIO36_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO36_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO36_INT_MASK_LEN)-1)<<GLB_REG_GPIO36_INT_MASK_POS)
#define GLB_REG_GPIO36_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO36_INT_MASK_LEN)-1)<<GLB_REG_GPIO36_INT_MASK_POS))
#define GLB_REG_GPIO37_INT_MASK                                 GLB_REG_GPIO37_INT_MASK
#define GLB_REG_GPIO37_INT_MASK_POS                             (5U)
#define GLB_REG_GPIO37_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO37_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO37_INT_MASK_LEN)-1)<<GLB_REG_GPIO37_INT_MASK_POS)
#define GLB_REG_GPIO37_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO37_INT_MASK_LEN)-1)<<GLB_REG_GPIO37_INT_MASK_POS))
#define GLB_REG_GPIO38_INT_MASK                                 GLB_REG_GPIO38_INT_MASK
#define GLB_REG_GPIO38_INT_MASK_POS                             (6U)
#define GLB_REG_GPIO38_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO38_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO38_INT_MASK_LEN)-1)<<GLB_REG_GPIO38_INT_MASK_POS)
#define GLB_REG_GPIO38_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO38_INT_MASK_LEN)-1)<<GLB_REG_GPIO38_INT_MASK_POS))
#define GLB_REG_GPIO39_INT_MASK                                 GLB_REG_GPIO39_INT_MASK
#define GLB_REG_GPIO39_INT_MASK_POS                             (7U)
#define GLB_REG_GPIO39_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO39_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO39_INT_MASK_LEN)-1)<<GLB_REG_GPIO39_INT_MASK_POS)
#define GLB_REG_GPIO39_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO39_INT_MASK_LEN)-1)<<GLB_REG_GPIO39_INT_MASK_POS))
#define GLB_REG_GPIO40_INT_MASK                                 GLB_REG_GPIO40_INT_MASK
#define GLB_REG_GPIO40_INT_MASK_POS                             (8U)
#define GLB_REG_GPIO40_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO40_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO40_INT_MASK_LEN)-1)<<GLB_REG_GPIO40_INT_MASK_POS)
#define GLB_REG_GPIO40_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO40_INT_MASK_LEN)-1)<<GLB_REG_GPIO40_INT_MASK_POS))
#define GLB_REG_GPIO41_INT_MASK                                 GLB_REG_GPIO41_INT_MASK
#define GLB_REG_GPIO41_INT_MASK_POS                             (9U)
#define GLB_REG_GPIO41_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO41_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO41_INT_MASK_LEN)-1)<<GLB_REG_GPIO41_INT_MASK_POS)
#define GLB_REG_GPIO41_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO41_INT_MASK_LEN)-1)<<GLB_REG_GPIO41_INT_MASK_POS))
#define GLB_REG_GPIO42_INT_MASK                                 GLB_REG_GPIO42_INT_MASK
#define GLB_REG_GPIO42_INT_MASK_POS                             (10U)
#define GLB_REG_GPIO42_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO42_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO42_INT_MASK_LEN)-1)<<GLB_REG_GPIO42_INT_MASK_POS)
#define GLB_REG_GPIO42_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO42_INT_MASK_LEN)-1)<<GLB_REG_GPIO42_INT_MASK_POS))
#define GLB_REG_GPIO43_INT_MASK                                 GLB_REG_GPIO43_INT_MASK
#define GLB_REG_GPIO43_INT_MASK_POS                             (11U)
#define GLB_REG_GPIO43_INT_MASK_LEN                             (1U)
#define GLB_REG_GPIO43_INT_MASK_MSK                             (((1U<<GLB_REG_GPIO43_INT_MASK_LEN)-1)<<GLB_REG_GPIO43_INT_MASK_POS)
#define GLB_REG_GPIO43_INT_MASK_UMSK                            (~(((1U<<GLB_REG_GPIO43_INT_MASK_LEN)-1)<<GLB_REG_GPIO43_INT_MASK_POS))
#define GLB_REG_GPIO32_INT_CLR                                  GLB_REG_GPIO32_INT_CLR
#define GLB_REG_GPIO32_INT_CLR_POS                              (12U)
#define GLB_REG_GPIO32_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO32_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO32_INT_CLR_LEN)-1)<<GLB_REG_GPIO32_INT_CLR_POS)
#define GLB_REG_GPIO32_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO32_INT_CLR_LEN)-1)<<GLB_REG_GPIO32_INT_CLR_POS))
#define GLB_REG_GPIO33_INT_CLR                                  GLB_REG_GPIO33_INT_CLR
#define GLB_REG_GPIO33_INT_CLR_POS                              (13U)
#define GLB_REG_GPIO33_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO33_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO33_INT_CLR_LEN)-1)<<GLB_REG_GPIO33_INT_CLR_POS)
#define GLB_REG_GPIO33_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO33_INT_CLR_LEN)-1)<<GLB_REG_GPIO33_INT_CLR_POS))
#define GLB_REG_GPIO34_INT_CLR                                  GLB_REG_GPIO34_INT_CLR
#define GLB_REG_GPIO34_INT_CLR_POS                              (14U)
#define GLB_REG_GPIO34_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO34_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO34_INT_CLR_LEN)-1)<<GLB_REG_GPIO34_INT_CLR_POS)
#define GLB_REG_GPIO34_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO34_INT_CLR_LEN)-1)<<GLB_REG_GPIO34_INT_CLR_POS))
#define GLB_REG_GPIO35_INT_CLR                                  GLB_REG_GPIO35_INT_CLR
#define GLB_REG_GPIO35_INT_CLR_POS                              (15U)
#define GLB_REG_GPIO35_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO35_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO35_INT_CLR_LEN)-1)<<GLB_REG_GPIO35_INT_CLR_POS)
#define GLB_REG_GPIO35_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO35_INT_CLR_LEN)-1)<<GLB_REG_GPIO35_INT_CLR_POS))
#define GLB_REG_GPIO36_INT_CLR                                  GLB_REG_GPIO36_INT_CLR
#define GLB_REG_GPIO36_INT_CLR_POS                              (16U)
#define GLB_REG_GPIO36_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO36_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO36_INT_CLR_LEN)-1)<<GLB_REG_GPIO36_INT_CLR_POS)
#define GLB_REG_GPIO36_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO36_INT_CLR_LEN)-1)<<GLB_REG_GPIO36_INT_CLR_POS))
#define GLB_REG_GPIO37_INT_CLR                                  GLB_REG_GPIO37_INT_CLR
#define GLB_REG_GPIO37_INT_CLR_POS                              (17U)
#define GLB_REG_GPIO37_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO37_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO37_INT_CLR_LEN)-1)<<GLB_REG_GPIO37_INT_CLR_POS)
#define GLB_REG_GPIO37_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO37_INT_CLR_LEN)-1)<<GLB_REG_GPIO37_INT_CLR_POS))
#define GLB_REG_GPIO38_INT_CLR                                  GLB_REG_GPIO38_INT_CLR
#define GLB_REG_GPIO38_INT_CLR_POS                              (18U)
#define GLB_REG_GPIO38_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO38_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO38_INT_CLR_LEN)-1)<<GLB_REG_GPIO38_INT_CLR_POS)
#define GLB_REG_GPIO38_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO38_INT_CLR_LEN)-1)<<GLB_REG_GPIO38_INT_CLR_POS))
#define GLB_REG_GPIO39_INT_CLR                                  GLB_REG_GPIO39_INT_CLR
#define GLB_REG_GPIO39_INT_CLR_POS                              (19U)
#define GLB_REG_GPIO39_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO39_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO39_INT_CLR_LEN)-1)<<GLB_REG_GPIO39_INT_CLR_POS)
#define GLB_REG_GPIO39_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO39_INT_CLR_LEN)-1)<<GLB_REG_GPIO39_INT_CLR_POS))
#define GLB_REG_GPIO40_INT_CLR                                  GLB_REG_GPIO40_INT_CLR
#define GLB_REG_GPIO40_INT_CLR_POS                              (20U)
#define GLB_REG_GPIO40_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO40_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO40_INT_CLR_LEN)-1)<<GLB_REG_GPIO40_INT_CLR_POS)
#define GLB_REG_GPIO40_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO40_INT_CLR_LEN)-1)<<GLB_REG_GPIO40_INT_CLR_POS))
#define GLB_REG_GPIO41_INT_CLR                                  GLB_REG_GPIO41_INT_CLR
#define GLB_REG_GPIO41_INT_CLR_POS                              (21U)
#define GLB_REG_GPIO41_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO41_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO41_INT_CLR_LEN)-1)<<GLB_REG_GPIO41_INT_CLR_POS)
#define GLB_REG_GPIO41_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO41_INT_CLR_LEN)-1)<<GLB_REG_GPIO41_INT_CLR_POS))
#define GLB_REG_GPIO42_INT_CLR                                  GLB_REG_GPIO42_INT_CLR
#define GLB_REG_GPIO42_INT_CLR_POS                              (22U)
#define GLB_REG_GPIO42_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO42_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO42_INT_CLR_LEN)-1)<<GLB_REG_GPIO42_INT_CLR_POS)
#define GLB_REG_GPIO42_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO42_INT_CLR_LEN)-1)<<GLB_REG_GPIO42_INT_CLR_POS))
#define GLB_REG_GPIO43_INT_CLR                                  GLB_REG_GPIO43_INT_CLR
#define GLB_REG_GPIO43_INT_CLR_POS                              (23U)
#define GLB_REG_GPIO43_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO43_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO43_INT_CLR_LEN)-1)<<GLB_REG_GPIO43_INT_CLR_POS)
#define GLB_REG_GPIO43_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO43_INT_CLR_LEN)-1)<<GLB_REG_GPIO43_INT_CLR_POS))

/* 0x1B0 : GPIO_INT_CLR1 */
#define GLB_GPIO_INT_CLR1_OFFSET                                (0x1B0)
#define GLB_REG_GPIO0_INT_CLR                                   GLB_REG_GPIO0_INT_CLR
#define GLB_REG_GPIO0_INT_CLR_POS                               (0U)
#define GLB_REG_GPIO0_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO0_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO0_INT_CLR_LEN)-1)<<GLB_REG_GPIO0_INT_CLR_POS)
#define GLB_REG_GPIO0_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO0_INT_CLR_LEN)-1)<<GLB_REG_GPIO0_INT_CLR_POS))
#define GLB_REG_GPIO1_INT_CLR                                   GLB_REG_GPIO1_INT_CLR
#define GLB_REG_GPIO1_INT_CLR_POS                               (1U)
#define GLB_REG_GPIO1_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO1_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO1_INT_CLR_LEN)-1)<<GLB_REG_GPIO1_INT_CLR_POS)
#define GLB_REG_GPIO1_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO1_INT_CLR_LEN)-1)<<GLB_REG_GPIO1_INT_CLR_POS))
#define GLB_REG_GPIO2_INT_CLR                                   GLB_REG_GPIO2_INT_CLR
#define GLB_REG_GPIO2_INT_CLR_POS                               (2U)
#define GLB_REG_GPIO2_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO2_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO2_INT_CLR_LEN)-1)<<GLB_REG_GPIO2_INT_CLR_POS)
#define GLB_REG_GPIO2_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO2_INT_CLR_LEN)-1)<<GLB_REG_GPIO2_INT_CLR_POS))
#define GLB_REG_GPIO3_INT_CLR                                   GLB_REG_GPIO3_INT_CLR
#define GLB_REG_GPIO3_INT_CLR_POS                               (3U)
#define GLB_REG_GPIO3_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO3_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO3_INT_CLR_LEN)-1)<<GLB_REG_GPIO3_INT_CLR_POS)
#define GLB_REG_GPIO3_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO3_INT_CLR_LEN)-1)<<GLB_REG_GPIO3_INT_CLR_POS))
#define GLB_REG_GPIO4_INT_CLR                                   GLB_REG_GPIO4_INT_CLR
#define GLB_REG_GPIO4_INT_CLR_POS                               (4U)
#define GLB_REG_GPIO4_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO4_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO4_INT_CLR_LEN)-1)<<GLB_REG_GPIO4_INT_CLR_POS)
#define GLB_REG_GPIO4_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO4_INT_CLR_LEN)-1)<<GLB_REG_GPIO4_INT_CLR_POS))
#define GLB_REG_GPIO5_INT_CLR                                   GLB_REG_GPIO5_INT_CLR
#define GLB_REG_GPIO5_INT_CLR_POS                               (5U)
#define GLB_REG_GPIO5_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO5_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO5_INT_CLR_LEN)-1)<<GLB_REG_GPIO5_INT_CLR_POS)
#define GLB_REG_GPIO5_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO5_INT_CLR_LEN)-1)<<GLB_REG_GPIO5_INT_CLR_POS))
#define GLB_REG_GPIO6_INT_CLR                                   GLB_REG_GPIO6_INT_CLR
#define GLB_REG_GPIO6_INT_CLR_POS                               (6U)
#define GLB_REG_GPIO6_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO6_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO6_INT_CLR_LEN)-1)<<GLB_REG_GPIO6_INT_CLR_POS)
#define GLB_REG_GPIO6_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO6_INT_CLR_LEN)-1)<<GLB_REG_GPIO6_INT_CLR_POS))
#define GLB_REG_GPIO7_INT_CLR                                   GLB_REG_GPIO7_INT_CLR
#define GLB_REG_GPIO7_INT_CLR_POS                               (7U)
#define GLB_REG_GPIO7_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO7_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO7_INT_CLR_LEN)-1)<<GLB_REG_GPIO7_INT_CLR_POS)
#define GLB_REG_GPIO7_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO7_INT_CLR_LEN)-1)<<GLB_REG_GPIO7_INT_CLR_POS))
#define GLB_REG_GPIO8_INT_CLR                                   GLB_REG_GPIO8_INT_CLR
#define GLB_REG_GPIO8_INT_CLR_POS                               (8U)
#define GLB_REG_GPIO8_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO8_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO8_INT_CLR_LEN)-1)<<GLB_REG_GPIO8_INT_CLR_POS)
#define GLB_REG_GPIO8_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO8_INT_CLR_LEN)-1)<<GLB_REG_GPIO8_INT_CLR_POS))
#define GLB_REG_GPIO9_INT_CLR                                   GLB_REG_GPIO9_INT_CLR
#define GLB_REG_GPIO9_INT_CLR_POS                               (9U)
#define GLB_REG_GPIO9_INT_CLR_LEN                               (1U)
#define GLB_REG_GPIO9_INT_CLR_MSK                               (((1U<<GLB_REG_GPIO9_INT_CLR_LEN)-1)<<GLB_REG_GPIO9_INT_CLR_POS)
#define GLB_REG_GPIO9_INT_CLR_UMSK                              (~(((1U<<GLB_REG_GPIO9_INT_CLR_LEN)-1)<<GLB_REG_GPIO9_INT_CLR_POS))
#define GLB_REG_GPIO10_INT_CLR                                  GLB_REG_GPIO10_INT_CLR
#define GLB_REG_GPIO10_INT_CLR_POS                              (10U)
#define GLB_REG_GPIO10_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO10_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO10_INT_CLR_LEN)-1)<<GLB_REG_GPIO10_INT_CLR_POS)
#define GLB_REG_GPIO10_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO10_INT_CLR_LEN)-1)<<GLB_REG_GPIO10_INT_CLR_POS))
#define GLB_REG_GPIO11_INT_CLR                                  GLB_REG_GPIO11_INT_CLR
#define GLB_REG_GPIO11_INT_CLR_POS                              (11U)
#define GLB_REG_GPIO11_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO11_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO11_INT_CLR_LEN)-1)<<GLB_REG_GPIO11_INT_CLR_POS)
#define GLB_REG_GPIO11_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO11_INT_CLR_LEN)-1)<<GLB_REG_GPIO11_INT_CLR_POS))
#define GLB_REG_GPIO12_INT_CLR                                  GLB_REG_GPIO12_INT_CLR
#define GLB_REG_GPIO12_INT_CLR_POS                              (12U)
#define GLB_REG_GPIO12_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO12_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO12_INT_CLR_LEN)-1)<<GLB_REG_GPIO12_INT_CLR_POS)
#define GLB_REG_GPIO12_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO12_INT_CLR_LEN)-1)<<GLB_REG_GPIO12_INT_CLR_POS))
#define GLB_REG_GPIO13_INT_CLR                                  GLB_REG_GPIO13_INT_CLR
#define GLB_REG_GPIO13_INT_CLR_POS                              (13U)
#define GLB_REG_GPIO13_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO13_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO13_INT_CLR_LEN)-1)<<GLB_REG_GPIO13_INT_CLR_POS)
#define GLB_REG_GPIO13_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO13_INT_CLR_LEN)-1)<<GLB_REG_GPIO13_INT_CLR_POS))
#define GLB_REG_GPIO14_INT_CLR                                  GLB_REG_GPIO14_INT_CLR
#define GLB_REG_GPIO14_INT_CLR_POS                              (14U)
#define GLB_REG_GPIO14_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO14_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO14_INT_CLR_LEN)-1)<<GLB_REG_GPIO14_INT_CLR_POS)
#define GLB_REG_GPIO14_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO14_INT_CLR_LEN)-1)<<GLB_REG_GPIO14_INT_CLR_POS))
#define GLB_REG_GPIO15_INT_CLR                                  GLB_REG_GPIO15_INT_CLR
#define GLB_REG_GPIO15_INT_CLR_POS                              (15U)
#define GLB_REG_GPIO15_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO15_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO15_INT_CLR_LEN)-1)<<GLB_REG_GPIO15_INT_CLR_POS)
#define GLB_REG_GPIO15_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO15_INT_CLR_LEN)-1)<<GLB_REG_GPIO15_INT_CLR_POS))
#define GLB_REG_GPIO16_INT_CLR                                  GLB_REG_GPIO16_INT_CLR
#define GLB_REG_GPIO16_INT_CLR_POS                              (16U)
#define GLB_REG_GPIO16_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO16_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO16_INT_CLR_LEN)-1)<<GLB_REG_GPIO16_INT_CLR_POS)
#define GLB_REG_GPIO16_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO16_INT_CLR_LEN)-1)<<GLB_REG_GPIO16_INT_CLR_POS))
#define GLB_REG_GPIO17_INT_CLR                                  GLB_REG_GPIO17_INT_CLR
#define GLB_REG_GPIO17_INT_CLR_POS                              (17U)
#define GLB_REG_GPIO17_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO17_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO17_INT_CLR_LEN)-1)<<GLB_REG_GPIO17_INT_CLR_POS)
#define GLB_REG_GPIO17_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO17_INT_CLR_LEN)-1)<<GLB_REG_GPIO17_INT_CLR_POS))
#define GLB_REG_GPIO18_INT_CLR                                  GLB_REG_GPIO18_INT_CLR
#define GLB_REG_GPIO18_INT_CLR_POS                              (18U)
#define GLB_REG_GPIO18_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO18_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO18_INT_CLR_LEN)-1)<<GLB_REG_GPIO18_INT_CLR_POS)
#define GLB_REG_GPIO18_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO18_INT_CLR_LEN)-1)<<GLB_REG_GPIO18_INT_CLR_POS))
#define GLB_REG_GPIO19_INT_CLR                                  GLB_REG_GPIO19_INT_CLR
#define GLB_REG_GPIO19_INT_CLR_POS                              (19U)
#define GLB_REG_GPIO19_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO19_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO19_INT_CLR_LEN)-1)<<GLB_REG_GPIO19_INT_CLR_POS)
#define GLB_REG_GPIO19_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO19_INT_CLR_LEN)-1)<<GLB_REG_GPIO19_INT_CLR_POS))
#define GLB_REG_GPIO20_INT_CLR                                  GLB_REG_GPIO20_INT_CLR
#define GLB_REG_GPIO20_INT_CLR_POS                              (20U)
#define GLB_REG_GPIO20_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO20_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO20_INT_CLR_LEN)-1)<<GLB_REG_GPIO20_INT_CLR_POS)
#define GLB_REG_GPIO20_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO20_INT_CLR_LEN)-1)<<GLB_REG_GPIO20_INT_CLR_POS))
#define GLB_REG_GPIO21_INT_CLR                                  GLB_REG_GPIO21_INT_CLR
#define GLB_REG_GPIO21_INT_CLR_POS                              (21U)
#define GLB_REG_GPIO21_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO21_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO21_INT_CLR_LEN)-1)<<GLB_REG_GPIO21_INT_CLR_POS)
#define GLB_REG_GPIO21_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO21_INT_CLR_LEN)-1)<<GLB_REG_GPIO21_INT_CLR_POS))
#define GLB_REG_GPIO22_INT_CLR                                  GLB_REG_GPIO22_INT_CLR
#define GLB_REG_GPIO22_INT_CLR_POS                              (22U)
#define GLB_REG_GPIO22_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO22_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO22_INT_CLR_LEN)-1)<<GLB_REG_GPIO22_INT_CLR_POS)
#define GLB_REG_GPIO22_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO22_INT_CLR_LEN)-1)<<GLB_REG_GPIO22_INT_CLR_POS))
#define GLB_REG_GPIO23_INT_CLR                                  GLB_REG_GPIO23_INT_CLR
#define GLB_REG_GPIO23_INT_CLR_POS                              (23U)
#define GLB_REG_GPIO23_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO23_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO23_INT_CLR_LEN)-1)<<GLB_REG_GPIO23_INT_CLR_POS)
#define GLB_REG_GPIO23_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO23_INT_CLR_LEN)-1)<<GLB_REG_GPIO23_INT_CLR_POS))
#define GLB_REG_GPIO24_INT_CLR                                  GLB_REG_GPIO24_INT_CLR
#define GLB_REG_GPIO24_INT_CLR_POS                              (24U)
#define GLB_REG_GPIO24_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO24_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO24_INT_CLR_LEN)-1)<<GLB_REG_GPIO24_INT_CLR_POS)
#define GLB_REG_GPIO24_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO24_INT_CLR_LEN)-1)<<GLB_REG_GPIO24_INT_CLR_POS))
#define GLB_REG_GPIO25_INT_CLR                                  GLB_REG_GPIO25_INT_CLR
#define GLB_REG_GPIO25_INT_CLR_POS                              (25U)
#define GLB_REG_GPIO25_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO25_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO25_INT_CLR_LEN)-1)<<GLB_REG_GPIO25_INT_CLR_POS)
#define GLB_REG_GPIO25_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO25_INT_CLR_LEN)-1)<<GLB_REG_GPIO25_INT_CLR_POS))
#define GLB_REG_GPIO26_INT_CLR                                  GLB_REG_GPIO26_INT_CLR
#define GLB_REG_GPIO26_INT_CLR_POS                              (26U)
#define GLB_REG_GPIO26_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO26_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO26_INT_CLR_LEN)-1)<<GLB_REG_GPIO26_INT_CLR_POS)
#define GLB_REG_GPIO26_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO26_INT_CLR_LEN)-1)<<GLB_REG_GPIO26_INT_CLR_POS))
#define GLB_REG_GPIO27_INT_CLR                                  GLB_REG_GPIO27_INT_CLR
#define GLB_REG_GPIO27_INT_CLR_POS                              (27U)
#define GLB_REG_GPIO27_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO27_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO27_INT_CLR_LEN)-1)<<GLB_REG_GPIO27_INT_CLR_POS)
#define GLB_REG_GPIO27_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO27_INT_CLR_LEN)-1)<<GLB_REG_GPIO27_INT_CLR_POS))
#define GLB_REG_GPIO28_INT_CLR                                  GLB_REG_GPIO28_INT_CLR
#define GLB_REG_GPIO28_INT_CLR_POS                              (28U)
#define GLB_REG_GPIO28_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO28_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO28_INT_CLR_LEN)-1)<<GLB_REG_GPIO28_INT_CLR_POS)
#define GLB_REG_GPIO28_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO28_INT_CLR_LEN)-1)<<GLB_REG_GPIO28_INT_CLR_POS))
#define GLB_REG_GPIO29_INT_CLR                                  GLB_REG_GPIO29_INT_CLR
#define GLB_REG_GPIO29_INT_CLR_POS                              (29U)
#define GLB_REG_GPIO29_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO29_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO29_INT_CLR_LEN)-1)<<GLB_REG_GPIO29_INT_CLR_POS)
#define GLB_REG_GPIO29_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO29_INT_CLR_LEN)-1)<<GLB_REG_GPIO29_INT_CLR_POS))
#define GLB_REG_GPIO30_INT_CLR                                  GLB_REG_GPIO30_INT_CLR
#define GLB_REG_GPIO30_INT_CLR_POS                              (30U)
#define GLB_REG_GPIO30_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO30_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO30_INT_CLR_LEN)-1)<<GLB_REG_GPIO30_INT_CLR_POS)
#define GLB_REG_GPIO30_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO30_INT_CLR_LEN)-1)<<GLB_REG_GPIO30_INT_CLR_POS))
#define GLB_REG_GPIO31_INT_CLR                                  GLB_REG_GPIO31_INT_CLR
#define GLB_REG_GPIO31_INT_CLR_POS                              (31U)
#define GLB_REG_GPIO31_INT_CLR_LEN                              (1U)
#define GLB_REG_GPIO31_INT_CLR_MSK                              (((1U<<GLB_REG_GPIO31_INT_CLR_LEN)-1)<<GLB_REG_GPIO31_INT_CLR_POS)
#define GLB_REG_GPIO31_INT_CLR_UMSK                             (~(((1U<<GLB_REG_GPIO31_INT_CLR_LEN)-1)<<GLB_REG_GPIO31_INT_CLR_POS))

/* 0x1B4 : GPIO_INT_MODE_SET1 */
#define GLB_GPIO_INT_MODE_SET1_OFFSET                           (0x1B4)
#define GLB_REG_GPIO0_INT_MODE_SET                              GLB_REG_GPIO0_INT_MODE_SET
#define GLB_REG_GPIO0_INT_MODE_SET_POS                          (0U)
#define GLB_REG_GPIO0_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO0_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO0_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO0_INT_MODE_SET_POS)
#define GLB_REG_GPIO0_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO0_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO0_INT_MODE_SET_POS))
#define GLB_REG_GPIO1_INT_MODE_SET                              GLB_REG_GPIO1_INT_MODE_SET
#define GLB_REG_GPIO1_INT_MODE_SET_POS                          (4U)
#define GLB_REG_GPIO1_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO1_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO1_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO1_INT_MODE_SET_POS)
#define GLB_REG_GPIO1_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO1_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO1_INT_MODE_SET_POS))
#define GLB_REG_GPIO2_INT_MODE_SET                              GLB_REG_GPIO2_INT_MODE_SET
#define GLB_REG_GPIO2_INT_MODE_SET_POS                          (8U)
#define GLB_REG_GPIO2_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO2_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO2_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO2_INT_MODE_SET_POS)
#define GLB_REG_GPIO2_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO2_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO2_INT_MODE_SET_POS))
#define GLB_REG_GPIO3_INT_MODE_SET                              GLB_REG_GPIO3_INT_MODE_SET
#define GLB_REG_GPIO3_INT_MODE_SET_POS                          (12U)
#define GLB_REG_GPIO3_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO3_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO3_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO3_INT_MODE_SET_POS)
#define GLB_REG_GPIO3_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO3_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO3_INT_MODE_SET_POS))
#define GLB_REG_GPIO4_INT_MODE_SET                              GLB_REG_GPIO4_INT_MODE_SET
#define GLB_REG_GPIO4_INT_MODE_SET_POS                          (16U)
#define GLB_REG_GPIO4_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO4_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO4_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO4_INT_MODE_SET_POS)
#define GLB_REG_GPIO4_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO4_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO4_INT_MODE_SET_POS))
#define GLB_REG_GPIO5_INT_MODE_SET                              GLB_REG_GPIO5_INT_MODE_SET
#define GLB_REG_GPIO5_INT_MODE_SET_POS                          (20U)
#define GLB_REG_GPIO5_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO5_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO5_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO5_INT_MODE_SET_POS)
#define GLB_REG_GPIO5_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO5_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO5_INT_MODE_SET_POS))
#define GLB_REG_GPIO6_INT_MODE_SET                              GLB_REG_GPIO6_INT_MODE_SET
#define GLB_REG_GPIO6_INT_MODE_SET_POS                          (24U)
#define GLB_REG_GPIO6_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO6_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO6_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO6_INT_MODE_SET_POS)
#define GLB_REG_GPIO6_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO6_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO6_INT_MODE_SET_POS))
#define GLB_REG_GPIO7_INT_MODE_SET                              GLB_REG_GPIO7_INT_MODE_SET
#define GLB_REG_GPIO7_INT_MODE_SET_POS                          (28U)
#define GLB_REG_GPIO7_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO7_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO7_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO7_INT_MODE_SET_POS)
#define GLB_REG_GPIO7_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO7_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO7_INT_MODE_SET_POS))

/* 0x1B8 : GPIO_INT_MODE_SET2 */
#define GLB_GPIO_INT_MODE_SET2_OFFSET                           (0x1B8)
#define GLB_REG_GPIO8_INT_MODE_SET                              GLB_REG_GPIO8_INT_MODE_SET
#define GLB_REG_GPIO8_INT_MODE_SET_POS                          (0U)
#define GLB_REG_GPIO8_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO8_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO8_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO8_INT_MODE_SET_POS)
#define GLB_REG_GPIO8_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO8_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO8_INT_MODE_SET_POS))
#define GLB_REG_GPIO9_INT_MODE_SET                              GLB_REG_GPIO9_INT_MODE_SET
#define GLB_REG_GPIO9_INT_MODE_SET_POS                          (4U)
#define GLB_REG_GPIO9_INT_MODE_SET_LEN                          (4U)
#define GLB_REG_GPIO9_INT_MODE_SET_MSK                          (((1U<<GLB_REG_GPIO9_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO9_INT_MODE_SET_POS)
#define GLB_REG_GPIO9_INT_MODE_SET_UMSK                         (~(((1U<<GLB_REG_GPIO9_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO9_INT_MODE_SET_POS))
#define GLB_REG_GPIO10_INT_MODE_SET                             GLB_REG_GPIO10_INT_MODE_SET
#define GLB_REG_GPIO10_INT_MODE_SET_POS                         (8U)
#define GLB_REG_GPIO10_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO10_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO10_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO10_INT_MODE_SET_POS)
#define GLB_REG_GPIO10_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO10_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO10_INT_MODE_SET_POS))
#define GLB_REG_GPIO11_INT_MODE_SET                             GLB_REG_GPIO11_INT_MODE_SET
#define GLB_REG_GPIO11_INT_MODE_SET_POS                         (12U)
#define GLB_REG_GPIO11_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO11_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO11_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO11_INT_MODE_SET_POS)
#define GLB_REG_GPIO11_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO11_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO11_INT_MODE_SET_POS))
#define GLB_REG_GPIO12_INT_MODE_SET                             GLB_REG_GPIO12_INT_MODE_SET
#define GLB_REG_GPIO12_INT_MODE_SET_POS                         (16U)
#define GLB_REG_GPIO12_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO12_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO12_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO12_INT_MODE_SET_POS)
#define GLB_REG_GPIO12_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO12_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO12_INT_MODE_SET_POS))
#define GLB_REG_GPIO13_INT_MODE_SET                             GLB_REG_GPIO13_INT_MODE_SET
#define GLB_REG_GPIO13_INT_MODE_SET_POS                         (20U)
#define GLB_REG_GPIO13_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO13_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO13_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO13_INT_MODE_SET_POS)
#define GLB_REG_GPIO13_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO13_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO13_INT_MODE_SET_POS))
#define GLB_REG_GPIO14_INT_MODE_SET                             GLB_REG_GPIO14_INT_MODE_SET
#define GLB_REG_GPIO14_INT_MODE_SET_POS                         (24U)
#define GLB_REG_GPIO14_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO14_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO14_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO14_INT_MODE_SET_POS)
#define GLB_REG_GPIO14_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO14_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO14_INT_MODE_SET_POS))
#define GLB_REG_GPIO15_INT_MODE_SET                             GLB_REG_GPIO15_INT_MODE_SET
#define GLB_REG_GPIO15_INT_MODE_SET_POS                         (28U)
#define GLB_REG_GPIO15_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO15_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO15_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO15_INT_MODE_SET_POS)
#define GLB_REG_GPIO15_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO15_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO15_INT_MODE_SET_POS))

/* 0x1BC : GPIO_INT_MODE_SET3 */
#define GLB_GPIO_INT_MODE_SET3_OFFSET                           (0x1BC)
#define GLB_REG_GPIO16_INT_MODE_SET                             GLB_REG_GPIO16_INT_MODE_SET
#define GLB_REG_GPIO16_INT_MODE_SET_POS                         (0U)
#define GLB_REG_GPIO16_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO16_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO16_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO16_INT_MODE_SET_POS)
#define GLB_REG_GPIO16_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO16_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO16_INT_MODE_SET_POS))
#define GLB_REG_GPIO17_INT_MODE_SET                             GLB_REG_GPIO17_INT_MODE_SET
#define GLB_REG_GPIO17_INT_MODE_SET_POS                         (4U)
#define GLB_REG_GPIO17_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO17_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO17_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO17_INT_MODE_SET_POS)
#define GLB_REG_GPIO17_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO17_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO17_INT_MODE_SET_POS))
#define GLB_REG_GPIO18_INT_MODE_SET                             GLB_REG_GPIO18_INT_MODE_SET
#define GLB_REG_GPIO18_INT_MODE_SET_POS                         (8U)
#define GLB_REG_GPIO18_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO18_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO18_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO18_INT_MODE_SET_POS)
#define GLB_REG_GPIO18_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO18_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO18_INT_MODE_SET_POS))
#define GLB_REG_GPIO19_INT_MODE_SET                             GLB_REG_GPIO19_INT_MODE_SET
#define GLB_REG_GPIO19_INT_MODE_SET_POS                         (12U)
#define GLB_REG_GPIO19_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO19_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO19_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO19_INT_MODE_SET_POS)
#define GLB_REG_GPIO19_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO19_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO19_INT_MODE_SET_POS))
#define GLB_REG_GPIO20_INT_MODE_SET                             GLB_REG_GPIO20_INT_MODE_SET
#define GLB_REG_GPIO20_INT_MODE_SET_POS                         (16U)
#define GLB_REG_GPIO20_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO20_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO20_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO20_INT_MODE_SET_POS)
#define GLB_REG_GPIO20_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO20_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO20_INT_MODE_SET_POS))
#define GLB_REG_GPIO21_INT_MODE_SET                             GLB_REG_GPIO21_INT_MODE_SET
#define GLB_REG_GPIO21_INT_MODE_SET_POS                         (20U)
#define GLB_REG_GPIO21_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO21_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO21_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO21_INT_MODE_SET_POS)
#define GLB_REG_GPIO21_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO21_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO21_INT_MODE_SET_POS))
#define GLB_REG_GPIO22_INT_MODE_SET                             GLB_REG_GPIO22_INT_MODE_SET
#define GLB_REG_GPIO22_INT_MODE_SET_POS                         (24U)
#define GLB_REG_GPIO22_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO22_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO22_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO22_INT_MODE_SET_POS)
#define GLB_REG_GPIO22_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO22_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO22_INT_MODE_SET_POS))
#define GLB_REG_GPIO23_INT_MODE_SET                             GLB_REG_GPIO23_INT_MODE_SET
#define GLB_REG_GPIO23_INT_MODE_SET_POS                         (28U)
#define GLB_REG_GPIO23_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO23_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO23_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO23_INT_MODE_SET_POS)
#define GLB_REG_GPIO23_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO23_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO23_INT_MODE_SET_POS))

/* 0x1C0 : GPIO_INT_MODE_SET4 */
#define GLB_GPIO_INT_MODE_SET4_OFFSET                           (0x1C0)
#define GLB_REG_GPIO24_INT_MODE_SET                             GLB_REG_GPIO24_INT_MODE_SET
#define GLB_REG_GPIO24_INT_MODE_SET_POS                         (0U)
#define GLB_REG_GPIO24_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO24_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO24_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO24_INT_MODE_SET_POS)
#define GLB_REG_GPIO24_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO24_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO24_INT_MODE_SET_POS))
#define GLB_REG_GPIO25_INT_MODE_SET                             GLB_REG_GPIO25_INT_MODE_SET
#define GLB_REG_GPIO25_INT_MODE_SET_POS                         (4U)
#define GLB_REG_GPIO25_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO25_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO25_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO25_INT_MODE_SET_POS)
#define GLB_REG_GPIO25_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO25_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO25_INT_MODE_SET_POS))
#define GLB_REG_GPIO26_INT_MODE_SET                             GLB_REG_GPIO26_INT_MODE_SET
#define GLB_REG_GPIO26_INT_MODE_SET_POS                         (8U)
#define GLB_REG_GPIO26_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO26_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO26_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO26_INT_MODE_SET_POS)
#define GLB_REG_GPIO26_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO26_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO26_INT_MODE_SET_POS))
#define GLB_REG_GPIO27_INT_MODE_SET                             GLB_REG_GPIO27_INT_MODE_SET
#define GLB_REG_GPIO27_INT_MODE_SET_POS                         (12U)
#define GLB_REG_GPIO27_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO27_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO27_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO27_INT_MODE_SET_POS)
#define GLB_REG_GPIO27_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO27_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO27_INT_MODE_SET_POS))
#define GLB_REG_GPIO28_INT_MODE_SET                             GLB_REG_GPIO28_INT_MODE_SET
#define GLB_REG_GPIO28_INT_MODE_SET_POS                         (16U)
#define GLB_REG_GPIO28_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO28_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO28_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO28_INT_MODE_SET_POS)
#define GLB_REG_GPIO28_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO28_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO28_INT_MODE_SET_POS))
#define GLB_REG_GPIO29_INT_MODE_SET                             GLB_REG_GPIO29_INT_MODE_SET
#define GLB_REG_GPIO29_INT_MODE_SET_POS                         (20U)
#define GLB_REG_GPIO29_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO29_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO29_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO29_INT_MODE_SET_POS)
#define GLB_REG_GPIO29_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO29_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO29_INT_MODE_SET_POS))
#define GLB_REG_GPIO30_INT_MODE_SET                             GLB_REG_GPIO30_INT_MODE_SET
#define GLB_REG_GPIO30_INT_MODE_SET_POS                         (24U)
#define GLB_REG_GPIO30_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO30_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO30_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO30_INT_MODE_SET_POS)
#define GLB_REG_GPIO30_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO30_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO30_INT_MODE_SET_POS))
#define GLB_REG_GPIO31_INT_MODE_SET                             GLB_REG_GPIO31_INT_MODE_SET
#define GLB_REG_GPIO31_INT_MODE_SET_POS                         (28U)
#define GLB_REG_GPIO31_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO31_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO31_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO31_INT_MODE_SET_POS)
#define GLB_REG_GPIO31_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO31_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO31_INT_MODE_SET_POS))

/* 0x1C4 : GPIO_INT_MODE_SET5 */
#define GLB_GPIO_INT_MODE_SET5_OFFSET                           (0x1C4)
#define GLB_REG_GPIO32_INT_MODE_SET                             GLB_REG_GPIO32_INT_MODE_SET
#define GLB_REG_GPIO32_INT_MODE_SET_POS                         (0U)
#define GLB_REG_GPIO32_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO32_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO32_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO32_INT_MODE_SET_POS)
#define GLB_REG_GPIO32_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO32_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO32_INT_MODE_SET_POS))
#define GLB_REG_GPIO33_INT_MODE_SET                             GLB_REG_GPIO33_INT_MODE_SET
#define GLB_REG_GPIO33_INT_MODE_SET_POS                         (4U)
#define GLB_REG_GPIO33_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO33_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO33_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO33_INT_MODE_SET_POS)
#define GLB_REG_GPIO33_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO33_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO33_INT_MODE_SET_POS))
#define GLB_REG_GPIO34_INT_MODE_SET                             GLB_REG_GPIO34_INT_MODE_SET
#define GLB_REG_GPIO34_INT_MODE_SET_POS                         (8U)
#define GLB_REG_GPIO34_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO34_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO34_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO34_INT_MODE_SET_POS)
#define GLB_REG_GPIO34_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO34_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO34_INT_MODE_SET_POS))
#define GLB_REG_GPIO35_INT_MODE_SET                             GLB_REG_GPIO35_INT_MODE_SET
#define GLB_REG_GPIO35_INT_MODE_SET_POS                         (12U)
#define GLB_REG_GPIO35_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO35_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO35_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO35_INT_MODE_SET_POS)
#define GLB_REG_GPIO35_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO35_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO35_INT_MODE_SET_POS))
#define GLB_REG_GPIO36_INT_MODE_SET                             GLB_REG_GPIO36_INT_MODE_SET
#define GLB_REG_GPIO36_INT_MODE_SET_POS                         (16U)
#define GLB_REG_GPIO36_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO36_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO36_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO36_INT_MODE_SET_POS)
#define GLB_REG_GPIO36_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO36_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO36_INT_MODE_SET_POS))
#define GLB_REG_GPIO37_INT_MODE_SET                             GLB_REG_GPIO37_INT_MODE_SET
#define GLB_REG_GPIO37_INT_MODE_SET_POS                         (20U)
#define GLB_REG_GPIO37_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO37_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO37_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO37_INT_MODE_SET_POS)
#define GLB_REG_GPIO37_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO37_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO37_INT_MODE_SET_POS))
#define GLB_REG_GPIO38_INT_MODE_SET                             GLB_REG_GPIO38_INT_MODE_SET
#define GLB_REG_GPIO38_INT_MODE_SET_POS                         (24U)
#define GLB_REG_GPIO38_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO38_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO38_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO38_INT_MODE_SET_POS)
#define GLB_REG_GPIO38_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO38_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO38_INT_MODE_SET_POS))
#define GLB_REG_GPIO39_INT_MODE_SET                             GLB_REG_GPIO39_INT_MODE_SET
#define GLB_REG_GPIO39_INT_MODE_SET_POS                         (28U)
#define GLB_REG_GPIO39_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO39_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO39_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO39_INT_MODE_SET_POS)
#define GLB_REG_GPIO39_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO39_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO39_INT_MODE_SET_POS))

/* 0x1C8 : GPIO_INT_MODE_SET6 */
#define GLB_GPIO_INT_MODE_SET6_OFFSET                           (0x1C8)
#define GLB_REG_GPIO40_INT_MODE_SET                             GLB_REG_GPIO40_INT_MODE_SET
#define GLB_REG_GPIO40_INT_MODE_SET_POS                         (0U)
#define GLB_REG_GPIO40_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO40_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO40_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO40_INT_MODE_SET_POS)
#define GLB_REG_GPIO40_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO40_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO40_INT_MODE_SET_POS))
#define GLB_REG_GPIO41_INT_MODE_SET                             GLB_REG_GPIO41_INT_MODE_SET
#define GLB_REG_GPIO41_INT_MODE_SET_POS                         (4U)
#define GLB_REG_GPIO41_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO41_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO41_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO41_INT_MODE_SET_POS)
#define GLB_REG_GPIO41_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO41_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO41_INT_MODE_SET_POS))
#define GLB_REG_GPIO42_INT_MODE_SET                             GLB_REG_GPIO42_INT_MODE_SET
#define GLB_REG_GPIO42_INT_MODE_SET_POS                         (8U)
#define GLB_REG_GPIO42_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO42_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO42_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO42_INT_MODE_SET_POS)
#define GLB_REG_GPIO42_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO42_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO42_INT_MODE_SET_POS))
#define GLB_REG_GPIO43_INT_MODE_SET                             GLB_REG_GPIO43_INT_MODE_SET
#define GLB_REG_GPIO43_INT_MODE_SET_POS                         (12U)
#define GLB_REG_GPIO43_INT_MODE_SET_LEN                         (4U)
#define GLB_REG_GPIO43_INT_MODE_SET_MSK                         (((1U<<GLB_REG_GPIO43_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO43_INT_MODE_SET_POS)
#define GLB_REG_GPIO43_INT_MODE_SET_UMSK                        (~(((1U<<GLB_REG_GPIO43_INT_MODE_SET_LEN)-1)<<GLB_REG_GPIO43_INT_MODE_SET_POS))

/* 0x224 : led_driver */
#define GLB_LED_DRIVER_OFFSET                                   (0x224)
#define GLB_LED_DIN_REG                                         GLB_LED_DIN_REG
#define GLB_LED_DIN_REG_POS                                     (0U)
#define GLB_LED_DIN_REG_LEN                                     (1U)
#define GLB_LED_DIN_REG_MSK                                     (((1U<<GLB_LED_DIN_REG_LEN)-1)<<GLB_LED_DIN_REG_POS)
#define GLB_LED_DIN_REG_UMSK                                    (~(((1U<<GLB_LED_DIN_REG_LEN)-1)<<GLB_LED_DIN_REG_POS))
#define GLB_LED_DIN_SEL                                         GLB_LED_DIN_SEL
#define GLB_LED_DIN_SEL_POS                                     (1U)
#define GLB_LED_DIN_SEL_LEN                                     (1U)
#define GLB_LED_DIN_SEL_MSK                                     (((1U<<GLB_LED_DIN_SEL_LEN)-1)<<GLB_LED_DIN_SEL_POS)
#define GLB_LED_DIN_SEL_UMSK                                    (~(((1U<<GLB_LED_DIN_SEL_LEN)-1)<<GLB_LED_DIN_SEL_POS))
#define GLB_LED_DIN_POLARITY_SEL                                GLB_LED_DIN_POLARITY_SEL
#define GLB_LED_DIN_POLARITY_SEL_POS                            (2U)
#define GLB_LED_DIN_POLARITY_SEL_LEN                            (1U)
#define GLB_LED_DIN_POLARITY_SEL_MSK                            (((1U<<GLB_LED_DIN_POLARITY_SEL_LEN)-1)<<GLB_LED_DIN_POLARITY_SEL_POS)
#define GLB_LED_DIN_POLARITY_SEL_UMSK                           (~(((1U<<GLB_LED_DIN_POLARITY_SEL_LEN)-1)<<GLB_LED_DIN_POLARITY_SEL_POS))
#define GLB_LEDDRV_IBIAS                                        GLB_LEDDRV_IBIAS
#define GLB_LEDDRV_IBIAS_POS                                    (4U)
#define GLB_LEDDRV_IBIAS_LEN                                    (3U)
#define GLB_LEDDRV_IBIAS_MSK                                    (((1U<<GLB_LEDDRV_IBIAS_LEN)-1)<<GLB_LEDDRV_IBIAS_POS)
#define GLB_LEDDRV_IBIAS_UMSK                                   (~(((1U<<GLB_LEDDRV_IBIAS_LEN)-1)<<GLB_LEDDRV_IBIAS_POS))
#define GLB_LEDDRV_OUT_EN                                       GLB_LEDDRV_OUT_EN
#define GLB_LEDDRV_OUT_EN_POS                                   (28U)
#define GLB_LEDDRV_OUT_EN_LEN                                   (2U)
#define GLB_LEDDRV_OUT_EN_MSK                                   (((1U<<GLB_LEDDRV_OUT_EN_LEN)-1)<<GLB_LEDDRV_OUT_EN_POS)
#define GLB_LEDDRV_OUT_EN_UMSK                                  (~(((1U<<GLB_LEDDRV_OUT_EN_LEN)-1)<<GLB_LEDDRV_OUT_EN_POS))
#define GLB_PU_LEDDRV                                           GLB_PU_LEDDRV
#define GLB_PU_LEDDRV_POS                                       (31U)
#define GLB_PU_LEDDRV_LEN                                       (1U)
#define GLB_PU_LEDDRV_MSK                                       (((1U<<GLB_PU_LEDDRV_LEN)-1)<<GLB_PU_LEDDRV_POS)
#define GLB_PU_LEDDRV_UMSK                                      (~(((1U<<GLB_PU_LEDDRV_LEN)-1)<<GLB_PU_LEDDRV_POS))

/* 0x228 : usb_xcvr */
#define GLB_USB_XCVR_OFFSET                                     (0x228)
#define GLB_USB_LDO_VFB                                         GLB_USB_LDO_VFB
#define GLB_USB_LDO_VFB_POS                                     (0U)
#define GLB_USB_LDO_VFB_LEN                                     (3U)
#define GLB_USB_LDO_VFB_MSK                                     (((1U<<GLB_USB_LDO_VFB_LEN)-1)<<GLB_USB_LDO_VFB_POS)
#define GLB_USB_LDO_VFB_UMSK                                    (~(((1U<<GLB_USB_LDO_VFB_LEN)-1)<<GLB_USB_LDO_VFB_POS))
#define GLB_PU_USB_LDO                                          GLB_PU_USB_LDO
#define GLB_PU_USB_LDO_POS                                      (3U)
#define GLB_PU_USB_LDO_LEN                                      (1U)
#define GLB_PU_USB_LDO_MSK                                      (((1U<<GLB_PU_USB_LDO_LEN)-1)<<GLB_PU_USB_LDO_POS)
#define GLB_PU_USB_LDO_UMSK                                     (~(((1U<<GLB_PU_USB_LDO_LEN)-1)<<GLB_PU_USB_LDO_POS))
#define GLB_USB_ROUT_NMOS                                       GLB_USB_ROUT_NMOS
#define GLB_USB_ROUT_NMOS_POS                                   (4U)
#define GLB_USB_ROUT_NMOS_LEN                                   (3U)
#define GLB_USB_ROUT_NMOS_MSK                                   (((1U<<GLB_USB_ROUT_NMOS_LEN)-1)<<GLB_USB_ROUT_NMOS_POS)
#define GLB_USB_ROUT_NMOS_UMSK                                  (~(((1U<<GLB_USB_ROUT_NMOS_LEN)-1)<<GLB_USB_ROUT_NMOS_POS))
#define GLB_USB_ROUT_PMOS                                       GLB_USB_ROUT_PMOS
#define GLB_USB_ROUT_PMOS_POS                                   (8U)
#define GLB_USB_ROUT_PMOS_LEN                                   (3U)
#define GLB_USB_ROUT_PMOS_MSK                                   (((1U<<GLB_USB_ROUT_PMOS_LEN)-1)<<GLB_USB_ROUT_PMOS_POS)
#define GLB_USB_ROUT_PMOS_UMSK                                  (~(((1U<<GLB_USB_ROUT_PMOS_LEN)-1)<<GLB_USB_ROUT_PMOS_POS))
#define GLB_USB_OEB_SEL                                         GLB_USB_OEB_SEL
#define GLB_USB_OEB_SEL_POS                                     (12U)
#define GLB_USB_OEB_SEL_LEN                                     (1U)
#define GLB_USB_OEB_SEL_MSK                                     (((1U<<GLB_USB_OEB_SEL_LEN)-1)<<GLB_USB_OEB_SEL_POS)
#define GLB_USB_OEB_SEL_UMSK                                    (~(((1U<<GLB_USB_OEB_SEL_LEN)-1)<<GLB_USB_OEB_SEL_POS))
#define GLB_USB_OEB_REG                                         GLB_USB_OEB_REG
#define GLB_USB_OEB_REG_POS                                     (13U)
#define GLB_USB_OEB_REG_LEN                                     (1U)
#define GLB_USB_OEB_REG_MSK                                     (((1U<<GLB_USB_OEB_REG_LEN)-1)<<GLB_USB_OEB_REG_POS)
#define GLB_USB_OEB_REG_UMSK                                    (~(((1U<<GLB_USB_OEB_REG_LEN)-1)<<GLB_USB_OEB_REG_POS))
#define GLB_USB_OEB                                             GLB_USB_OEB
#define GLB_USB_OEB_POS                                         (14U)
#define GLB_USB_OEB_LEN                                         (1U)
#define GLB_USB_OEB_MSK                                         (((1U<<GLB_USB_OEB_LEN)-1)<<GLB_USB_OEB_POS)
#define GLB_USB_OEB_UMSK                                        (~(((1U<<GLB_USB_OEB_LEN)-1)<<GLB_USB_OEB_POS))
#define GLB_USB_DATA_CONVERT                                    GLB_USB_DATA_CONVERT
#define GLB_USB_DATA_CONVERT_POS                                (16U)
#define GLB_USB_DATA_CONVERT_LEN                                (1U)
#define GLB_USB_DATA_CONVERT_MSK                                (((1U<<GLB_USB_DATA_CONVERT_LEN)-1)<<GLB_USB_DATA_CONVERT_POS)
#define GLB_USB_DATA_CONVERT_UMSK                               (~(((1U<<GLB_USB_DATA_CONVERT_LEN)-1)<<GLB_USB_DATA_CONVERT_POS))
#define GLB_USB_ENUM                                            GLB_USB_ENUM
#define GLB_USB_ENUM_POS                                        (20U)
#define GLB_USB_ENUM_LEN                                        (1U)
#define GLB_USB_ENUM_MSK                                        (((1U<<GLB_USB_ENUM_LEN)-1)<<GLB_USB_ENUM_POS)
#define GLB_USB_ENUM_UMSK                                       (~(((1U<<GLB_USB_ENUM_LEN)-1)<<GLB_USB_ENUM_POS))
#define GLB_USB_SPD                                             GLB_USB_SPD
#define GLB_USB_SPD_POS                                         (21U)
#define GLB_USB_SPD_LEN                                         (1U)
#define GLB_USB_SPD_MSK                                         (((1U<<GLB_USB_SPD_LEN)-1)<<GLB_USB_SPD_POS)
#define GLB_USB_SPD_UMSK                                        (~(((1U<<GLB_USB_SPD_LEN)-1)<<GLB_USB_SPD_POS))
#define GLB_USB_SUS                                             GLB_USB_SUS
#define GLB_USB_SUS_POS                                         (22U)
#define GLB_USB_SUS_LEN                                         (1U)
#define GLB_USB_SUS_MSK                                         (((1U<<GLB_USB_SUS_LEN)-1)<<GLB_USB_SUS_POS)
#define GLB_USB_SUS_UMSK                                        (~(((1U<<GLB_USB_SUS_LEN)-1)<<GLB_USB_SUS_POS))
#define GLB_PU_USB                                              GLB_PU_USB
#define GLB_PU_USB_POS                                          (23U)
#define GLB_PU_USB_LEN                                          (1U)
#define GLB_PU_USB_MSK                                          (((1U<<GLB_PU_USB_LEN)-1)<<GLB_PU_USB_POS)
#define GLB_PU_USB_UMSK                                         (~(((1U<<GLB_PU_USB_LEN)-1)<<GLB_PU_USB_POS))
#define GLB_USB_BD                                              GLB_USB_BD
#define GLB_USB_BD_POS                                          (24U)
#define GLB_USB_BD_LEN                                          (1U)
#define GLB_USB_BD_MSK                                          (((1U<<GLB_USB_BD_LEN)-1)<<GLB_USB_BD_POS)
#define GLB_USB_BD_UMSK                                         (~(((1U<<GLB_USB_BD_LEN)-1)<<GLB_USB_BD_POS))
#define GLB_USB_VIM                                             GLB_USB_VIM
#define GLB_USB_VIM_POS                                         (25U)
#define GLB_USB_VIM_LEN                                         (1U)
#define GLB_USB_VIM_MSK                                         (((1U<<GLB_USB_VIM_LEN)-1)<<GLB_USB_VIM_POS)
#define GLB_USB_VIM_UMSK                                        (~(((1U<<GLB_USB_VIM_LEN)-1)<<GLB_USB_VIM_POS))
#define GLB_USB_VIP                                             GLB_USB_VIP
#define GLB_USB_VIP_POS                                         (26U)
#define GLB_USB_VIP_LEN                                         (1U)
#define GLB_USB_VIP_MSK                                         (((1U<<GLB_USB_VIP_LEN)-1)<<GLB_USB_VIP_POS)
#define GLB_USB_VIP_UMSK                                        (~(((1U<<GLB_USB_VIP_LEN)-1)<<GLB_USB_VIP_POS))
#define GLB_USB_RCV                                             GLB_USB_RCV
#define GLB_USB_RCV_POS                                         (27U)
#define GLB_USB_RCV_LEN                                         (1U)
#define GLB_USB_RCV_MSK                                         (((1U<<GLB_USB_RCV_LEN)-1)<<GLB_USB_RCV_POS)
#define GLB_USB_RCV_UMSK                                        (~(((1U<<GLB_USB_RCV_LEN)-1)<<GLB_USB_RCV_POS))

/* 0x22C : usb_xcvr_config */
#define GLB_USB_XCVR_CONFIG_OFFSET                              (0x22C)
#define GLB_USB_V_HYS_M                                         GLB_USB_V_HYS_M
#define GLB_USB_V_HYS_M_POS                                     (0U)
#define GLB_USB_V_HYS_M_LEN                                     (2U)
#define GLB_USB_V_HYS_M_MSK                                     (((1U<<GLB_USB_V_HYS_M_LEN)-1)<<GLB_USB_V_HYS_M_POS)
#define GLB_USB_V_HYS_M_UMSK                                    (~(((1U<<GLB_USB_V_HYS_M_LEN)-1)<<GLB_USB_V_HYS_M_POS))
#define GLB_USB_V_HYS_P                                         GLB_USB_V_HYS_P
#define GLB_USB_V_HYS_P_POS                                     (2U)
#define GLB_USB_V_HYS_P_LEN                                     (2U)
#define GLB_USB_V_HYS_P_MSK                                     (((1U<<GLB_USB_V_HYS_P_LEN)-1)<<GLB_USB_V_HYS_P_POS)
#define GLB_USB_V_HYS_P_UMSK                                    (~(((1U<<GLB_USB_V_HYS_P_LEN)-1)<<GLB_USB_V_HYS_P_POS))
#define GLB_USB_BD_VTH                                          GLB_USB_BD_VTH
#define GLB_USB_BD_VTH_POS                                      (4U)
#define GLB_USB_BD_VTH_LEN                                      (3U)
#define GLB_USB_BD_VTH_MSK                                      (((1U<<GLB_USB_BD_VTH_LEN)-1)<<GLB_USB_BD_VTH_POS)
#define GLB_USB_BD_VTH_UMSK                                     (~(((1U<<GLB_USB_BD_VTH_LEN)-1)<<GLB_USB_BD_VTH_POS))
#define GLB_REG_USB_USE_XCVR                                    GLB_REG_USB_USE_XCVR
#define GLB_REG_USB_USE_XCVR_POS                                (7U)
#define GLB_REG_USB_USE_XCVR_LEN                                (1U)
#define GLB_REG_USB_USE_XCVR_MSK                                (((1U<<GLB_REG_USB_USE_XCVR_LEN)-1)<<GLB_REG_USB_USE_XCVR_POS)
#define GLB_REG_USB_USE_XCVR_UMSK                               (~(((1U<<GLB_REG_USB_USE_XCVR_LEN)-1)<<GLB_REG_USB_USE_XCVR_POS))
#define GLB_USB_STR_DRV                                         GLB_USB_STR_DRV
#define GLB_USB_STR_DRV_POS                                     (8U)
#define GLB_USB_STR_DRV_LEN                                     (3U)
#define GLB_USB_STR_DRV_MSK                                     (((1U<<GLB_USB_STR_DRV_LEN)-1)<<GLB_USB_STR_DRV_POS)
#define GLB_USB_STR_DRV_UMSK                                    (~(((1U<<GLB_USB_STR_DRV_LEN)-1)<<GLB_USB_STR_DRV_POS))
#define GLB_REG_USB_USE_CTRL                                    GLB_REG_USB_USE_CTRL
#define GLB_REG_USB_USE_CTRL_POS                                (11U)
#define GLB_REG_USB_USE_CTRL_LEN                                (1U)
#define GLB_REG_USB_USE_CTRL_MSK                                (((1U<<GLB_REG_USB_USE_CTRL_LEN)-1)<<GLB_REG_USB_USE_CTRL_POS)
#define GLB_REG_USB_USE_CTRL_UMSK                               (~(((1U<<GLB_REG_USB_USE_CTRL_LEN)-1)<<GLB_REG_USB_USE_CTRL_POS))
#define GLB_USB_RES_PULLUP_TUNE                                 GLB_USB_RES_PULLUP_TUNE
#define GLB_USB_RES_PULLUP_TUNE_POS                             (12U)
#define GLB_USB_RES_PULLUP_TUNE_LEN                             (3U)
#define GLB_USB_RES_PULLUP_TUNE_MSK                             (((1U<<GLB_USB_RES_PULLUP_TUNE_LEN)-1)<<GLB_USB_RES_PULLUP_TUNE_POS)
#define GLB_USB_RES_PULLUP_TUNE_UMSK                            (~(((1U<<GLB_USB_RES_PULLUP_TUNE_LEN)-1)<<GLB_USB_RES_PULLUP_TUNE_POS))
#define GLB_USB_SLEWRATE_M_FALL                                 GLB_USB_SLEWRATE_M_FALL
#define GLB_USB_SLEWRATE_M_FALL_POS                             (16U)
#define GLB_USB_SLEWRATE_M_FALL_LEN                             (3U)
#define GLB_USB_SLEWRATE_M_FALL_MSK                             (((1U<<GLB_USB_SLEWRATE_M_FALL_LEN)-1)<<GLB_USB_SLEWRATE_M_FALL_POS)
#define GLB_USB_SLEWRATE_M_FALL_UMSK                            (~(((1U<<GLB_USB_SLEWRATE_M_FALL_LEN)-1)<<GLB_USB_SLEWRATE_M_FALL_POS))
#define GLB_USB_SLEWRATE_M_RISE                                 GLB_USB_SLEWRATE_M_RISE
#define GLB_USB_SLEWRATE_M_RISE_POS                             (20U)
#define GLB_USB_SLEWRATE_M_RISE_LEN                             (3U)
#define GLB_USB_SLEWRATE_M_RISE_MSK                             (((1U<<GLB_USB_SLEWRATE_M_RISE_LEN)-1)<<GLB_USB_SLEWRATE_M_RISE_POS)
#define GLB_USB_SLEWRATE_M_RISE_UMSK                            (~(((1U<<GLB_USB_SLEWRATE_M_RISE_LEN)-1)<<GLB_USB_SLEWRATE_M_RISE_POS))
#define GLB_USB_SLEWRATE_P_FALL                                 GLB_USB_SLEWRATE_P_FALL
#define GLB_USB_SLEWRATE_P_FALL_POS                             (24U)
#define GLB_USB_SLEWRATE_P_FALL_LEN                             (3U)
#define GLB_USB_SLEWRATE_P_FALL_MSK                             (((1U<<GLB_USB_SLEWRATE_P_FALL_LEN)-1)<<GLB_USB_SLEWRATE_P_FALL_POS)
#define GLB_USB_SLEWRATE_P_FALL_UMSK                            (~(((1U<<GLB_USB_SLEWRATE_P_FALL_LEN)-1)<<GLB_USB_SLEWRATE_P_FALL_POS))
#define GLB_USB_SLEWRATE_P_RISE                                 GLB_USB_SLEWRATE_P_RISE
#define GLB_USB_SLEWRATE_P_RISE_POS                             (28U)
#define GLB_USB_SLEWRATE_P_RISE_LEN                             (3U)
#define GLB_USB_SLEWRATE_P_RISE_MSK                             (((1U<<GLB_USB_SLEWRATE_P_RISE_LEN)-1)<<GLB_USB_SLEWRATE_P_RISE_POS)
#define GLB_USB_SLEWRATE_P_RISE_UMSK                            (~(((1U<<GLB_USB_SLEWRATE_P_RISE_LEN)-1)<<GLB_USB_SLEWRATE_P_RISE_POS))

/* 0x300 : rc32m_ctrl0 */
#define GLB_RC32M_CTRL0_OFFSET                                  (0x300)
#define GLB_RC32M_CAL_DONE                                      GLB_RC32M_CAL_DONE
#define GLB_RC32M_CAL_DONE_POS                                  (0U)
#define GLB_RC32M_CAL_DONE_LEN                                  (1U)
#define GLB_RC32M_CAL_DONE_MSK                                  (((1U<<GLB_RC32M_CAL_DONE_LEN)-1)<<GLB_RC32M_CAL_DONE_POS)
#define GLB_RC32M_CAL_DONE_UMSK                                 (~(((1U<<GLB_RC32M_CAL_DONE_LEN)-1)<<GLB_RC32M_CAL_DONE_POS))
#define GLB_RC32M_RDY                                           GLB_RC32M_RDY
#define GLB_RC32M_RDY_POS                                       (1U)
#define GLB_RC32M_RDY_LEN                                       (1U)
#define GLB_RC32M_RDY_MSK                                       (((1U<<GLB_RC32M_RDY_LEN)-1)<<GLB_RC32M_RDY_POS)
#define GLB_RC32M_RDY_UMSK                                      (~(((1U<<GLB_RC32M_RDY_LEN)-1)<<GLB_RC32M_RDY_POS))
#define GLB_RC32M_CAL_INPROGRESS                                GLB_RC32M_CAL_INPROGRESS
#define GLB_RC32M_CAL_INPROGRESS_POS                            (2U)
#define GLB_RC32M_CAL_INPROGRESS_LEN                            (1U)
#define GLB_RC32M_CAL_INPROGRESS_MSK                            (((1U<<GLB_RC32M_CAL_INPROGRESS_LEN)-1)<<GLB_RC32M_CAL_INPROGRESS_POS)
#define GLB_RC32M_CAL_INPROGRESS_UMSK                           (~(((1U<<GLB_RC32M_CAL_INPROGRESS_LEN)-1)<<GLB_RC32M_CAL_INPROGRESS_POS))
#define GLB_RC32M_CAL_DIV                                       GLB_RC32M_CAL_DIV
#define GLB_RC32M_CAL_DIV_POS                                   (3U)
#define GLB_RC32M_CAL_DIV_LEN                                   (2U)
#define GLB_RC32M_CAL_DIV_MSK                                   (((1U<<GLB_RC32M_CAL_DIV_LEN)-1)<<GLB_RC32M_CAL_DIV_POS)
#define GLB_RC32M_CAL_DIV_UMSK                                  (~(((1U<<GLB_RC32M_CAL_DIV_LEN)-1)<<GLB_RC32M_CAL_DIV_POS))
#define GLB_RC32M_CAL_PRECHARGE                                 GLB_RC32M_CAL_PRECHARGE
#define GLB_RC32M_CAL_PRECHARGE_POS                             (5U)
#define GLB_RC32M_CAL_PRECHARGE_LEN                             (1U)
#define GLB_RC32M_CAL_PRECHARGE_MSK                             (((1U<<GLB_RC32M_CAL_PRECHARGE_LEN)-1)<<GLB_RC32M_CAL_PRECHARGE_POS)
#define GLB_RC32M_CAL_PRECHARGE_UMSK                            (~(((1U<<GLB_RC32M_CAL_PRECHARGE_LEN)-1)<<GLB_RC32M_CAL_PRECHARGE_POS))
#define GLB_RC32M_DIG_CODE_FR_CAL                               GLB_RC32M_DIG_CODE_FR_CAL
#define GLB_RC32M_DIG_CODE_FR_CAL_POS                           (6U)
#define GLB_RC32M_DIG_CODE_FR_CAL_LEN                           (8U)
#define GLB_RC32M_DIG_CODE_FR_CAL_MSK                           (((1U<<GLB_RC32M_DIG_CODE_FR_CAL_LEN)-1)<<GLB_RC32M_DIG_CODE_FR_CAL_POS)
#define GLB_RC32M_DIG_CODE_FR_CAL_UMSK                          (~(((1U<<GLB_RC32M_DIG_CODE_FR_CAL_LEN)-1)<<GLB_RC32M_DIG_CODE_FR_CAL_POS))
#define GLB_RC32M_ALLOW_CAL                                     GLB_RC32M_ALLOW_CAL
#define GLB_RC32M_ALLOW_CAL_POS                                 (17U)
#define GLB_RC32M_ALLOW_CAL_LEN                                 (1U)
#define GLB_RC32M_ALLOW_CAL_MSK                                 (((1U<<GLB_RC32M_ALLOW_CAL_LEN)-1)<<GLB_RC32M_ALLOW_CAL_POS)
#define GLB_RC32M_ALLOW_CAL_UMSK                                (~(((1U<<GLB_RC32M_ALLOW_CAL_LEN)-1)<<GLB_RC32M_ALLOW_CAL_POS))
#define GLB_RC32M_EXT_CODE_EN                                   GLB_RC32M_EXT_CODE_EN
#define GLB_RC32M_EXT_CODE_EN_POS                               (19U)
#define GLB_RC32M_EXT_CODE_EN_LEN                               (1U)
#define GLB_RC32M_EXT_CODE_EN_MSK                               (((1U<<GLB_RC32M_EXT_CODE_EN_LEN)-1)<<GLB_RC32M_EXT_CODE_EN_POS)
#define GLB_RC32M_EXT_CODE_EN_UMSK                              (~(((1U<<GLB_RC32M_EXT_CODE_EN_LEN)-1)<<GLB_RC32M_EXT_CODE_EN_POS))
#define GLB_RC32M_CAL_EN                                        GLB_RC32M_CAL_EN
#define GLB_RC32M_CAL_EN_POS                                    (20U)
#define GLB_RC32M_CAL_EN_LEN                                    (1U)
#define GLB_RC32M_CAL_EN_MSK                                    (((1U<<GLB_RC32M_CAL_EN_LEN)-1)<<GLB_RC32M_CAL_EN_POS)
#define GLB_RC32M_CAL_EN_UMSK                                   (~(((1U<<GLB_RC32M_CAL_EN_LEN)-1)<<GLB_RC32M_CAL_EN_POS))
#define GLB_RC32M_PD                                            GLB_RC32M_PD
#define GLB_RC32M_PD_POS                                        (21U)
#define GLB_RC32M_PD_LEN                                        (1U)
#define GLB_RC32M_PD_MSK                                        (((1U<<GLB_RC32M_PD_LEN)-1)<<GLB_RC32M_PD_POS)
#define GLB_RC32M_PD_UMSK                                       (~(((1U<<GLB_RC32M_PD_LEN)-1)<<GLB_RC32M_PD_POS))
#define GLB_RC32M_RC32K_CAL_CGEN                                GLB_RC32M_RC32K_CAL_CGEN
#define GLB_RC32M_RC32K_CAL_CGEN_POS                            (30U)
#define GLB_RC32M_RC32K_CAL_CGEN_LEN                            (1U)
#define GLB_RC32M_RC32K_CAL_CGEN_MSK                            (((1U<<GLB_RC32M_RC32K_CAL_CGEN_LEN)-1)<<GLB_RC32M_RC32K_CAL_CGEN_POS)
#define GLB_RC32M_RC32K_CAL_CGEN_UMSK                           (~(((1U<<GLB_RC32M_RC32K_CAL_CGEN_LEN)-1)<<GLB_RC32M_RC32K_CAL_CGEN_POS))

/* 0x304 : rc32m_ctrl1 */
#define GLB_RC32M_CTRL1_OFFSET                                  (0x304)
#define GLB_RC32M_TEST_EN                                       GLB_RC32M_TEST_EN
#define GLB_RC32M_TEST_EN_POS                                   (0U)
#define GLB_RC32M_TEST_EN_LEN                                   (1U)
#define GLB_RC32M_TEST_EN_MSK                                   (((1U<<GLB_RC32M_TEST_EN_LEN)-1)<<GLB_RC32M_TEST_EN_POS)
#define GLB_RC32M_TEST_EN_UMSK                                  (~(((1U<<GLB_RC32M_TEST_EN_LEN)-1)<<GLB_RC32M_TEST_EN_POS))
#define GLB_RC32M_CODE_FR_EXT2                                  GLB_RC32M_CODE_FR_EXT2
#define GLB_RC32M_CODE_FR_EXT2_POS                              (8U)
#define GLB_RC32M_CODE_FR_EXT2_LEN                              (8U)
#define GLB_RC32M_CODE_FR_EXT2_MSK                              (((1U<<GLB_RC32M_CODE_FR_EXT2_LEN)-1)<<GLB_RC32M_CODE_FR_EXT2_POS)
#define GLB_RC32M_CODE_FR_EXT2_UMSK                             (~(((1U<<GLB_RC32M_CODE_FR_EXT2_LEN)-1)<<GLB_RC32M_CODE_FR_EXT2_POS))
#define GLB_RC32M_EXT_CODE_SEL                                  GLB_RC32M_EXT_CODE_SEL
#define GLB_RC32M_EXT_CODE_SEL_POS                              (16U)
#define GLB_RC32M_EXT_CODE_SEL_LEN                              (1U)
#define GLB_RC32M_EXT_CODE_SEL_MSK                              (((1U<<GLB_RC32M_EXT_CODE_SEL_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_POS)
#define GLB_RC32M_EXT_CODE_SEL_UMSK                             (~(((1U<<GLB_RC32M_EXT_CODE_SEL_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_POS))
#define GLB_RC32M_EXT_CODE_SEL_CGEN                             GLB_RC32M_EXT_CODE_SEL_CGEN
#define GLB_RC32M_EXT_CODE_SEL_CGEN_POS                         (17U)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_LEN                         (1U)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_MSK                         (((1U<<GLB_RC32M_EXT_CODE_SEL_CGEN_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_CGEN_POS)
#define GLB_RC32M_EXT_CODE_SEL_CGEN_UMSK                        (~(((1U<<GLB_RC32M_EXT_CODE_SEL_CGEN_LEN)-1)<<GLB_RC32M_EXT_CODE_SEL_CGEN_POS))
#define GLB_RC32M_RESERVED                                      GLB_RC32M_RESERVED
#define GLB_RC32M_RESERVED_POS                                  (24U)
#define GLB_RC32M_RESERVED_LEN                                  (8U)
#define GLB_RC32M_RESERVED_MSK                                  (((1U<<GLB_RC32M_RESERVED_LEN)-1)<<GLB_RC32M_RESERVED_POS)
#define GLB_RC32M_RESERVED_UMSK                                 (~(((1U<<GLB_RC32M_RESERVED_LEN)-1)<<GLB_RC32M_RESERVED_POS))

/* 0x308 : rc32k_ctrl2 */
#define GLB_RC32K_CTRL2_OFFSET                                  (0x308)
#define GLB_RC32K_DIG_CAL_DONE                                  GLB_RC32K_DIG_CAL_DONE
#define GLB_RC32K_DIG_CAL_DONE_POS                              (0U)
#define GLB_RC32K_DIG_CAL_DONE_LEN                              (1U)
#define GLB_RC32K_DIG_CAL_DONE_MSK                              (((1U<<GLB_RC32K_DIG_CAL_DONE_LEN)-1)<<GLB_RC32K_DIG_CAL_DONE_POS)
#define GLB_RC32K_DIG_CAL_DONE_UMSK                             (~(((1U<<GLB_RC32K_DIG_CAL_DONE_LEN)-1)<<GLB_RC32K_DIG_CAL_DONE_POS))
#define GLB_RC32K_RDY                                           GLB_RC32K_RDY
#define GLB_RC32K_RDY_POS                                       (1U)
#define GLB_RC32K_RDY_LEN                                       (1U)
#define GLB_RC32K_RDY_MSK                                       (((1U<<GLB_RC32K_RDY_LEN)-1)<<GLB_RC32K_RDY_POS)
#define GLB_RC32K_RDY_UMSK                                      (~(((1U<<GLB_RC32K_RDY_LEN)-1)<<GLB_RC32K_RDY_POS))
#define GLB_RC32K_DIG_CAL_INPROGRESS                            GLB_RC32K_DIG_CAL_INPROGRESS
#define GLB_RC32K_DIG_CAL_INPROGRESS_POS                        (2U)
#define GLB_RC32K_DIG_CAL_INPROGRESS_LEN                        (1U)
#define GLB_RC32K_DIG_CAL_INPROGRESS_MSK                        (((1U<<GLB_RC32K_DIG_CAL_INPROGRESS_LEN)-1)<<GLB_RC32K_DIG_CAL_INPROGRESS_POS)
#define GLB_RC32K_DIG_CAL_INPROGRESS_UMSK                       (~(((1U<<GLB_RC32K_DIG_CAL_INPROGRESS_LEN)-1)<<GLB_RC32K_DIG_CAL_INPROGRESS_POS))
#define GLB_RC32K_DIG_CAL_DIV                                   GLB_RC32K_DIG_CAL_DIV
#define GLB_RC32K_DIG_CAL_DIV_POS                               (3U)
#define GLB_RC32K_DIG_CAL_DIV_LEN                               (2U)
#define GLB_RC32K_DIG_CAL_DIV_MSK                               (((1U<<GLB_RC32K_DIG_CAL_DIV_LEN)-1)<<GLB_RC32K_DIG_CAL_DIV_POS)
#define GLB_RC32K_DIG_CAL_DIV_UMSK                              (~(((1U<<GLB_RC32K_DIG_CAL_DIV_LEN)-1)<<GLB_RC32K_DIG_CAL_DIV_POS))
#define GLB_RC32K_DIG_CAL_PRECHARGE                             GLB_RC32K_DIG_CAL_PRECHARGE
#define GLB_RC32K_DIG_CAL_PRECHARGE_POS                         (5U)
#define GLB_RC32K_DIG_CAL_PRECHARGE_LEN                         (1U)
#define GLB_RC32K_DIG_CAL_PRECHARGE_MSK                         (((1U<<GLB_RC32K_DIG_CAL_PRECHARGE_LEN)-1)<<GLB_RC32K_DIG_CAL_PRECHARGE_POS)
#define GLB_RC32K_DIG_CAL_PRECHARGE_UMSK                        (~(((1U<<GLB_RC32K_DIG_CAL_PRECHARGE_LEN)-1)<<GLB_RC32K_DIG_CAL_PRECHARGE_POS))
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL                           GLB_RC32K_DIG_CAP_CODE_FR_CAL
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS                       (6U)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN                       (4U)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_MSK                       (((1U<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN)-1)<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS)
#define GLB_RC32K_DIG_CAP_CODE_FR_CAL_UMSK                      (~(((1U<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_LEN)-1)<<GLB_RC32K_DIG_CAP_CODE_FR_CAL_POS))
#define GLB_RC32K_DIG_VREF_DLY                                  GLB_RC32K_DIG_VREF_DLY
#define GLB_RC32K_DIG_VREF_DLY_POS                              (16U)
#define GLB_RC32K_DIG_VREF_DLY_LEN                              (2U)
#define GLB_RC32K_DIG_VREF_DLY_MSK                              (((1U<<GLB_RC32K_DIG_VREF_DLY_LEN)-1)<<GLB_RC32K_DIG_VREF_DLY_POS)
#define GLB_RC32K_DIG_VREF_DLY_UMSK                             (~(((1U<<GLB_RC32K_DIG_VREF_DLY_LEN)-1)<<GLB_RC32K_DIG_VREF_DLY_POS))
#define GLB_RC32K_ALLOW_CAL                                     GLB_RC32K_ALLOW_CAL
#define GLB_RC32K_ALLOW_CAL_POS                                 (18U)
#define GLB_RC32K_ALLOW_CAL_LEN                                 (1U)
#define GLB_RC32K_ALLOW_CAL_MSK                                 (((1U<<GLB_RC32K_ALLOW_CAL_LEN)-1)<<GLB_RC32K_ALLOW_CAL_POS)
#define GLB_RC32K_ALLOW_CAL_UMSK                                (~(((1U<<GLB_RC32K_ALLOW_CAL_LEN)-1)<<GLB_RC32K_ALLOW_CAL_POS))

/* 0x430 : sdh_cfg0 */
#define GLB_SDH_CFG0_OFFSET                                     (0x430)
#define GLB_REG_SDH_CLK_DIV                                     GLB_REG_SDH_CLK_DIV
#define GLB_REG_SDH_CLK_DIV_POS                                 (0U)
#define GLB_REG_SDH_CLK_DIV_LEN                                 (10U)
#define GLB_REG_SDH_CLK_DIV_MSK                                 (((1U<<GLB_REG_SDH_CLK_DIV_LEN)-1)<<GLB_REG_SDH_CLK_DIV_POS)
#define GLB_REG_SDH_CLK_DIV_UMSK                                (~(((1U<<GLB_REG_SDH_CLK_DIV_LEN)-1)<<GLB_REG_SDH_CLK_DIV_POS))
#define GLB_REG_SDH_CLK_SEL                                     GLB_REG_SDH_CLK_SEL
#define GLB_REG_SDH_CLK_SEL_POS                                 (12U)
#define GLB_REG_SDH_CLK_SEL_LEN                                 (1U)
#define GLB_REG_SDH_CLK_SEL_MSK                                 (((1U<<GLB_REG_SDH_CLK_SEL_LEN)-1)<<GLB_REG_SDH_CLK_SEL_POS)
#define GLB_REG_SDH_CLK_SEL_UMSK                                (~(((1U<<GLB_REG_SDH_CLK_SEL_LEN)-1)<<GLB_REG_SDH_CLK_SEL_POS))
#define GLB_REG_SDH_CLK_EN                                      GLB_REG_SDH_CLK_EN
#define GLB_REG_SDH_CLK_EN_POS                                  (13U)
#define GLB_REG_SDH_CLK_EN_LEN                                  (1U)
#define GLB_REG_SDH_CLK_EN_MSK                                  (((1U<<GLB_REG_SDH_CLK_EN_LEN)-1)<<GLB_REG_SDH_CLK_EN_POS)
#define GLB_REG_SDH_CLK_EN_UMSK                                 (~(((1U<<GLB_REG_SDH_CLK_EN_LEN)-1)<<GLB_REG_SDH_CLK_EN_POS))

/* 0x434 : smih_0_cfg0 */
#define GLB_SMIH_0_CFG0_OFFSET                                  (0x434)
#define GLB_CR_SMIH_0_CD_N                                      GLB_CR_SMIH_0_CD_N
#define GLB_CR_SMIH_0_CD_N_POS                                  (0U)
#define GLB_CR_SMIH_0_CD_N_LEN                                  (1U)
#define GLB_CR_SMIH_0_CD_N_MSK                                  (((1U<<GLB_CR_SMIH_0_CD_N_LEN)-1)<<GLB_CR_SMIH_0_CD_N_POS)
#define GLB_CR_SMIH_0_CD_N_UMSK                                 (~(((1U<<GLB_CR_SMIH_0_CD_N_LEN)-1)<<GLB_CR_SMIH_0_CD_N_POS))
#define GLB_CR_SMIH_0_WP                                        GLB_CR_SMIH_0_WP
#define GLB_CR_SMIH_0_WP_POS                                    (1U)
#define GLB_CR_SMIH_0_WP_LEN                                    (1U)
#define GLB_CR_SMIH_0_WP_MSK                                    (((1U<<GLB_CR_SMIH_0_WP_LEN)-1)<<GLB_CR_SMIH_0_WP_POS)
#define GLB_CR_SMIH_0_WP_UMSK                                   (~(((1U<<GLB_CR_SMIH_0_WP_LEN)-1)<<GLB_CR_SMIH_0_WP_POS))
#define GLB_ST_SMIH_0_LED_ON                                    GLB_ST_SMIH_0_LED_ON
#define GLB_ST_SMIH_0_LED_ON_POS                                (16U)
#define GLB_ST_SMIH_0_LED_ON_LEN                                (1U)
#define GLB_ST_SMIH_0_LED_ON_MSK                                (((1U<<GLB_ST_SMIH_0_LED_ON_LEN)-1)<<GLB_ST_SMIH_0_LED_ON_POS)
#define GLB_ST_SMIH_0_LED_ON_UMSK                               (~(((1U<<GLB_ST_SMIH_0_LED_ON_LEN)-1)<<GLB_ST_SMIH_0_LED_ON_POS))
#define GLB_ST_SMIH_0_RST_N                                     GLB_ST_SMIH_0_RST_N
#define GLB_ST_SMIH_0_RST_N_POS                                 (17U)
#define GLB_ST_SMIH_0_RST_N_LEN                                 (1U)
#define GLB_ST_SMIH_0_RST_N_MSK                                 (((1U<<GLB_ST_SMIH_0_RST_N_LEN)-1)<<GLB_ST_SMIH_0_RST_N_POS)
#define GLB_ST_SMIH_0_RST_N_UMSK                                (~(((1U<<GLB_ST_SMIH_0_RST_N_LEN)-1)<<GLB_ST_SMIH_0_RST_N_POS))
#define GLB_ST_SMIH_0_WKUP_ON                                   GLB_ST_SMIH_0_WKUP_ON
#define GLB_ST_SMIH_0_WKUP_ON_POS                               (18U)
#define GLB_ST_SMIH_0_WKUP_ON_LEN                               (1U)
#define GLB_ST_SMIH_0_WKUP_ON_MSK                               (((1U<<GLB_ST_SMIH_0_WKUP_ON_LEN)-1)<<GLB_ST_SMIH_0_WKUP_ON_POS)
#define GLB_ST_SMIH_0_WKUP_ON_UMSK                              (~(((1U<<GLB_ST_SMIH_0_WKUP_ON_LEN)-1)<<GLB_ST_SMIH_0_WKUP_ON_POS))
#define GLB_ST_SMIH_0_OD_PP                                     GLB_ST_SMIH_0_OD_PP
#define GLB_ST_SMIH_0_OD_PP_POS                                 (19U)
#define GLB_ST_SMIH_0_OD_PP_LEN                                 (1U)
#define GLB_ST_SMIH_0_OD_PP_MSK                                 (((1U<<GLB_ST_SMIH_0_OD_PP_LEN)-1)<<GLB_ST_SMIH_0_OD_PP_POS)
#define GLB_ST_SMIH_0_OD_PP_UMSK                                (~(((1U<<GLB_ST_SMIH_0_OD_PP_LEN)-1)<<GLB_ST_SMIH_0_OD_PP_POS))
#define GLB_ST_SMIH_0_DRIVE_STRENGTH                            GLB_ST_SMIH_0_DRIVE_STRENGTH
#define GLB_ST_SMIH_0_DRIVE_STRENGTH_POS                        (20U)
#define GLB_ST_SMIH_0_DRIVE_STRENGTH_LEN                        (2U)
#define GLB_ST_SMIH_0_DRIVE_STRENGTH_MSK                        (((1U<<GLB_ST_SMIH_0_DRIVE_STRENGTH_LEN)-1)<<GLB_ST_SMIH_0_DRIVE_STRENGTH_POS)
#define GLB_ST_SMIH_0_DRIVE_STRENGTH_UMSK                       (~(((1U<<GLB_ST_SMIH_0_DRIVE_STRENGTH_LEN)-1)<<GLB_ST_SMIH_0_DRIVE_STRENGTH_POS))
#define GLB_ST_SMIH_0_VOL_SEL                                   GLB_ST_SMIH_0_VOL_SEL
#define GLB_ST_SMIH_0_VOL_SEL_POS                               (22U)
#define GLB_ST_SMIH_0_VOL_SEL_LEN                               (3U)
#define GLB_ST_SMIH_0_VOL_SEL_MSK                               (((1U<<GLB_ST_SMIH_0_VOL_SEL_LEN)-1)<<GLB_ST_SMIH_0_VOL_SEL_POS)
#define GLB_ST_SMIH_0_VOL_SEL_UMSK                              (~(((1U<<GLB_ST_SMIH_0_VOL_SEL_LEN)-1)<<GLB_ST_SMIH_0_VOL_SEL_POS))
#define GLB_ST_SMIH_0_VOL_ON                                    GLB_ST_SMIH_0_VOL_ON
#define GLB_ST_SMIH_0_VOL_ON_POS                                (25U)
#define GLB_ST_SMIH_0_VOL_ON_LEN                                (1U)
#define GLB_ST_SMIH_0_VOL_ON_MSK                                (((1U<<GLB_ST_SMIH_0_VOL_ON_LEN)-1)<<GLB_ST_SMIH_0_VOL_ON_POS)
#define GLB_ST_SMIH_0_VOL_ON_UMSK                               (~(((1U<<GLB_ST_SMIH_0_VOL_ON_LEN)-1)<<GLB_ST_SMIH_0_VOL_ON_POS))

/* 0x438 : smih_0_io_dly_0 */
#define GLB_SMIH_0_IO_DLY_0_OFFSET                              (0x438)
#define GLB_CR_SMIH_0_CLK_I_INV                                 GLB_CR_SMIH_0_CLK_I_INV
#define GLB_CR_SMIH_0_CLK_I_INV_POS                             (0U)
#define GLB_CR_SMIH_0_CLK_I_INV_LEN                             (1U)
#define GLB_CR_SMIH_0_CLK_I_INV_MSK                             (((1U<<GLB_CR_SMIH_0_CLK_I_INV_LEN)-1)<<GLB_CR_SMIH_0_CLK_I_INV_POS)
#define GLB_CR_SMIH_0_CLK_I_INV_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CLK_I_INV_LEN)-1)<<GLB_CR_SMIH_0_CLK_I_INV_POS))
#define GLB_CR_SMIH_0_CLK_O_INV                                 GLB_CR_SMIH_0_CLK_O_INV
#define GLB_CR_SMIH_0_CLK_O_INV_POS                             (1U)
#define GLB_CR_SMIH_0_CLK_O_INV_LEN                             (1U)
#define GLB_CR_SMIH_0_CLK_O_INV_MSK                             (((1U<<GLB_CR_SMIH_0_CLK_O_INV_LEN)-1)<<GLB_CR_SMIH_0_CLK_O_INV_POS)
#define GLB_CR_SMIH_0_CLK_O_INV_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CLK_O_INV_LEN)-1)<<GLB_CR_SMIH_0_CLK_O_INV_POS))
#define GLB_CR_SMIH_0_CLK_I_DLY                                 GLB_CR_SMIH_0_CLK_I_DLY
#define GLB_CR_SMIH_0_CLK_I_DLY_POS                             (2U)
#define GLB_CR_SMIH_0_CLK_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_CLK_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_CLK_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_CLK_I_DLY_POS)
#define GLB_CR_SMIH_0_CLK_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CLK_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_CLK_I_DLY_POS))
#define GLB_CR_SMIH_0_CLK_O_DLY                                 GLB_CR_SMIH_0_CLK_O_DLY
#define GLB_CR_SMIH_0_CLK_O_DLY_POS                             (4U)
#define GLB_CR_SMIH_0_CLK_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_CLK_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_CLK_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_CLK_O_DLY_POS)
#define GLB_CR_SMIH_0_CLK_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CLK_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_CLK_O_DLY_POS))
#define GLB_CR_SMIH_0_CMD_I_DLY                                 GLB_CR_SMIH_0_CMD_I_DLY
#define GLB_CR_SMIH_0_CMD_I_DLY_POS                             (6U)
#define GLB_CR_SMIH_0_CMD_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_CMD_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_CMD_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_I_DLY_POS)
#define GLB_CR_SMIH_0_CMD_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CMD_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_I_DLY_POS))
#define GLB_CR_SMIH_0_CMD_O_DLY                                 GLB_CR_SMIH_0_CMD_O_DLY
#define GLB_CR_SMIH_0_CMD_O_DLY_POS                             (8U)
#define GLB_CR_SMIH_0_CMD_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_CMD_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_CMD_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_O_DLY_POS)
#define GLB_CR_SMIH_0_CMD_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CMD_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_O_DLY_POS))
#define GLB_CR_SMIH_0_CMD_E_DLY                                 GLB_CR_SMIH_0_CMD_E_DLY
#define GLB_CR_SMIH_0_CMD_E_DLY_POS                             (10U)
#define GLB_CR_SMIH_0_CMD_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_CMD_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_CMD_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_E_DLY_POS)
#define GLB_CR_SMIH_0_CMD_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_CMD_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_CMD_E_DLY_POS))
#define GLB_CR_SMIH_0_D_0_I_DLY                                 GLB_CR_SMIH_0_D_0_I_DLY
#define GLB_CR_SMIH_0_D_0_I_DLY_POS                             (16U)
#define GLB_CR_SMIH_0_D_0_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_0_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_0_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_I_DLY_POS)
#define GLB_CR_SMIH_0_D_0_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_0_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_I_DLY_POS))
#define GLB_CR_SMIH_0_D_1_I_DLY                                 GLB_CR_SMIH_0_D_1_I_DLY
#define GLB_CR_SMIH_0_D_1_I_DLY_POS                             (18U)
#define GLB_CR_SMIH_0_D_1_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_1_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_1_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_I_DLY_POS)
#define GLB_CR_SMIH_0_D_1_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_1_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_I_DLY_POS))
#define GLB_CR_SMIH_0_D_2_I_DLY                                 GLB_CR_SMIH_0_D_2_I_DLY
#define GLB_CR_SMIH_0_D_2_I_DLY_POS                             (20U)
#define GLB_CR_SMIH_0_D_2_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_2_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_2_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_I_DLY_POS)
#define GLB_CR_SMIH_0_D_2_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_2_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_I_DLY_POS))
#define GLB_CR_SMIH_0_D_3_I_DLY                                 GLB_CR_SMIH_0_D_3_I_DLY
#define GLB_CR_SMIH_0_D_3_I_DLY_POS                             (22U)
#define GLB_CR_SMIH_0_D_3_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_3_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_3_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_I_DLY_POS)
#define GLB_CR_SMIH_0_D_3_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_3_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_I_DLY_POS))
#define GLB_CR_SMIH_0_D_4_I_DLY                                 GLB_CR_SMIH_0_D_4_I_DLY
#define GLB_CR_SMIH_0_D_4_I_DLY_POS                             (24U)
#define GLB_CR_SMIH_0_D_4_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_4_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_4_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_I_DLY_POS)
#define GLB_CR_SMIH_0_D_4_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_4_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_I_DLY_POS))
#define GLB_CR_SMIH_0_D_5_I_DLY                                 GLB_CR_SMIH_0_D_5_I_DLY
#define GLB_CR_SMIH_0_D_5_I_DLY_POS                             (26U)
#define GLB_CR_SMIH_0_D_5_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_5_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_5_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_I_DLY_POS)
#define GLB_CR_SMIH_0_D_5_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_5_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_I_DLY_POS))
#define GLB_CR_SMIH_0_D_6_I_DLY                                 GLB_CR_SMIH_0_D_6_I_DLY
#define GLB_CR_SMIH_0_D_6_I_DLY_POS                             (28U)
#define GLB_CR_SMIH_0_D_6_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_6_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_6_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_I_DLY_POS)
#define GLB_CR_SMIH_0_D_6_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_6_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_I_DLY_POS))
#define GLB_CR_SMIH_0_D_7_I_DLY                                 GLB_CR_SMIH_0_D_7_I_DLY
#define GLB_CR_SMIH_0_D_7_I_DLY_POS                             (30U)
#define GLB_CR_SMIH_0_D_7_I_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_7_I_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_7_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_I_DLY_POS)
#define GLB_CR_SMIH_0_D_7_I_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_7_I_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_I_DLY_POS))

/* 0x43C : smih_0_io_dly_1 */
#define GLB_SMIH_0_IO_DLY_1_OFFSET                              (0x43C)
#define GLB_CR_SMIH_0_D_0_O_DLY                                 GLB_CR_SMIH_0_D_0_O_DLY
#define GLB_CR_SMIH_0_D_0_O_DLY_POS                             (0U)
#define GLB_CR_SMIH_0_D_0_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_0_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_0_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_O_DLY_POS)
#define GLB_CR_SMIH_0_D_0_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_0_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_O_DLY_POS))
#define GLB_CR_SMIH_0_D_1_O_DLY                                 GLB_CR_SMIH_0_D_1_O_DLY
#define GLB_CR_SMIH_0_D_1_O_DLY_POS                             (2U)
#define GLB_CR_SMIH_0_D_1_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_1_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_1_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_O_DLY_POS)
#define GLB_CR_SMIH_0_D_1_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_1_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_O_DLY_POS))
#define GLB_CR_SMIH_0_D_2_O_DLY                                 GLB_CR_SMIH_0_D_2_O_DLY
#define GLB_CR_SMIH_0_D_2_O_DLY_POS                             (4U)
#define GLB_CR_SMIH_0_D_2_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_2_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_2_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_O_DLY_POS)
#define GLB_CR_SMIH_0_D_2_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_2_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_O_DLY_POS))
#define GLB_CR_SMIH_0_D_3_O_DLY                                 GLB_CR_SMIH_0_D_3_O_DLY
#define GLB_CR_SMIH_0_D_3_O_DLY_POS                             (6U)
#define GLB_CR_SMIH_0_D_3_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_3_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_3_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_O_DLY_POS)
#define GLB_CR_SMIH_0_D_3_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_3_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_O_DLY_POS))
#define GLB_CR_SMIH_0_D_4_O_DLY                                 GLB_CR_SMIH_0_D_4_O_DLY
#define GLB_CR_SMIH_0_D_4_O_DLY_POS                             (8U)
#define GLB_CR_SMIH_0_D_4_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_4_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_4_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_O_DLY_POS)
#define GLB_CR_SMIH_0_D_4_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_4_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_O_DLY_POS))
#define GLB_CR_SMIH_0_D_5_O_DLY                                 GLB_CR_SMIH_0_D_5_O_DLY
#define GLB_CR_SMIH_0_D_5_O_DLY_POS                             (10U)
#define GLB_CR_SMIH_0_D_5_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_5_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_5_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_O_DLY_POS)
#define GLB_CR_SMIH_0_D_5_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_5_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_O_DLY_POS))
#define GLB_CR_SMIH_0_D_6_O_DLY                                 GLB_CR_SMIH_0_D_6_O_DLY
#define GLB_CR_SMIH_0_D_6_O_DLY_POS                             (12U)
#define GLB_CR_SMIH_0_D_6_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_6_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_6_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_O_DLY_POS)
#define GLB_CR_SMIH_0_D_6_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_6_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_O_DLY_POS))
#define GLB_CR_SMIH_0_D_7_O_DLY                                 GLB_CR_SMIH_0_D_7_O_DLY
#define GLB_CR_SMIH_0_D_7_O_DLY_POS                             (14U)
#define GLB_CR_SMIH_0_D_7_O_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_7_O_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_7_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_O_DLY_POS)
#define GLB_CR_SMIH_0_D_7_O_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_7_O_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_O_DLY_POS))
#define GLB_CR_SMIH_0_D_0_E_DLY                                 GLB_CR_SMIH_0_D_0_E_DLY
#define GLB_CR_SMIH_0_D_0_E_DLY_POS                             (16U)
#define GLB_CR_SMIH_0_D_0_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_0_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_0_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_E_DLY_POS)
#define GLB_CR_SMIH_0_D_0_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_0_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_0_E_DLY_POS))
#define GLB_CR_SMIH_0_D_1_E_DLY                                 GLB_CR_SMIH_0_D_1_E_DLY
#define GLB_CR_SMIH_0_D_1_E_DLY_POS                             (18U)
#define GLB_CR_SMIH_0_D_1_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_1_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_1_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_E_DLY_POS)
#define GLB_CR_SMIH_0_D_1_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_1_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_1_E_DLY_POS))
#define GLB_CR_SMIH_0_D_2_E_DLY                                 GLB_CR_SMIH_0_D_2_E_DLY
#define GLB_CR_SMIH_0_D_2_E_DLY_POS                             (20U)
#define GLB_CR_SMIH_0_D_2_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_2_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_2_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_E_DLY_POS)
#define GLB_CR_SMIH_0_D_2_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_2_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_2_E_DLY_POS))
#define GLB_CR_SMIH_0_D_3_E_DLY                                 GLB_CR_SMIH_0_D_3_E_DLY
#define GLB_CR_SMIH_0_D_3_E_DLY_POS                             (22U)
#define GLB_CR_SMIH_0_D_3_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_3_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_3_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_E_DLY_POS)
#define GLB_CR_SMIH_0_D_3_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_3_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_3_E_DLY_POS))
#define GLB_CR_SMIH_0_D_4_E_DLY                                 GLB_CR_SMIH_0_D_4_E_DLY
#define GLB_CR_SMIH_0_D_4_E_DLY_POS                             (24U)
#define GLB_CR_SMIH_0_D_4_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_4_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_4_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_E_DLY_POS)
#define GLB_CR_SMIH_0_D_4_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_4_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_4_E_DLY_POS))
#define GLB_CR_SMIH_0_D_5_E_DLY                                 GLB_CR_SMIH_0_D_5_E_DLY
#define GLB_CR_SMIH_0_D_5_E_DLY_POS                             (26U)
#define GLB_CR_SMIH_0_D_5_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_5_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_5_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_E_DLY_POS)
#define GLB_CR_SMIH_0_D_5_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_5_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_5_E_DLY_POS))
#define GLB_CR_SMIH_0_D_6_E_DLY                                 GLB_CR_SMIH_0_D_6_E_DLY
#define GLB_CR_SMIH_0_D_6_E_DLY_POS                             (28U)
#define GLB_CR_SMIH_0_D_6_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_6_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_6_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_E_DLY_POS)
#define GLB_CR_SMIH_0_D_6_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_6_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_6_E_DLY_POS))
#define GLB_CR_SMIH_0_D_7_E_DLY                                 GLB_CR_SMIH_0_D_7_E_DLY
#define GLB_CR_SMIH_0_D_7_E_DLY_POS                             (30U)
#define GLB_CR_SMIH_0_D_7_E_DLY_LEN                             (2U)
#define GLB_CR_SMIH_0_D_7_E_DLY_MSK                             (((1U<<GLB_CR_SMIH_0_D_7_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_E_DLY_POS)
#define GLB_CR_SMIH_0_D_7_E_DLY_UMSK                            (~(((1U<<GLB_CR_SMIH_0_D_7_E_DLY_LEN)-1)<<GLB_CR_SMIH_0_D_7_E_DLY_POS))

/* 0x50C : gpadc_reg_cmd */
#define GLB_GPADC_REG_CMD_OFFSET                                (0x50C)
#define GLB_GPADC_GLOBAL_EN                                     GLB_GPADC_GLOBAL_EN
#define GLB_GPADC_GLOBAL_EN_POS                                 (0U)
#define GLB_GPADC_GLOBAL_EN_LEN                                 (1U)
#define GLB_GPADC_GLOBAL_EN_MSK                                 (((1U<<GLB_GPADC_GLOBAL_EN_LEN)-1)<<GLB_GPADC_GLOBAL_EN_POS)
#define GLB_GPADC_GLOBAL_EN_UMSK                                (~(((1U<<GLB_GPADC_GLOBAL_EN_LEN)-1)<<GLB_GPADC_GLOBAL_EN_POS))
#define GLB_GPADC_CONV_START                                    GLB_GPADC_CONV_START
#define GLB_GPADC_CONV_START_POS                                (1U)
#define GLB_GPADC_CONV_START_LEN                                (1U)
#define GLB_GPADC_CONV_START_MSK                                (((1U<<GLB_GPADC_CONV_START_LEN)-1)<<GLB_GPADC_CONV_START_POS)
#define GLB_GPADC_CONV_START_UMSK                               (~(((1U<<GLB_GPADC_CONV_START_LEN)-1)<<GLB_GPADC_CONV_START_POS))
#define GLB_GPADC_SOFT_RST                                      GLB_GPADC_SOFT_RST
#define GLB_GPADC_SOFT_RST_POS                                  (2U)
#define GLB_GPADC_SOFT_RST_LEN                                  (1U)
#define GLB_GPADC_SOFT_RST_MSK                                  (((1U<<GLB_GPADC_SOFT_RST_LEN)-1)<<GLB_GPADC_SOFT_RST_POS)
#define GLB_GPADC_SOFT_RST_UMSK                                 (~(((1U<<GLB_GPADC_SOFT_RST_LEN)-1)<<GLB_GPADC_SOFT_RST_POS))
#define GLB_GPADC_NEG_SEL                                       GLB_GPADC_NEG_SEL
#define GLB_GPADC_NEG_SEL_POS                                   (3U)
#define GLB_GPADC_NEG_SEL_LEN                                   (5U)
#define GLB_GPADC_NEG_SEL_MSK                                   (((1U<<GLB_GPADC_NEG_SEL_LEN)-1)<<GLB_GPADC_NEG_SEL_POS)
#define GLB_GPADC_NEG_SEL_UMSK                                  (~(((1U<<GLB_GPADC_NEG_SEL_LEN)-1)<<GLB_GPADC_NEG_SEL_POS))
#define GLB_GPADC_POS_SEL                                       GLB_GPADC_POS_SEL
#define GLB_GPADC_POS_SEL_POS                                   (8U)
#define GLB_GPADC_POS_SEL_LEN                                   (5U)
#define GLB_GPADC_POS_SEL_MSK                                   (((1U<<GLB_GPADC_POS_SEL_LEN)-1)<<GLB_GPADC_POS_SEL_POS)
#define GLB_GPADC_POS_SEL_UMSK                                  (~(((1U<<GLB_GPADC_POS_SEL_LEN)-1)<<GLB_GPADC_POS_SEL_POS))
#define GLB_GPADC_NEG_GND                                       GLB_GPADC_NEG_GND
#define GLB_GPADC_NEG_GND_POS                                   (13U)
#define GLB_GPADC_NEG_GND_LEN                                   (1U)
#define GLB_GPADC_NEG_GND_MSK                                   (((1U<<GLB_GPADC_NEG_GND_LEN)-1)<<GLB_GPADC_NEG_GND_POS)
#define GLB_GPADC_NEG_GND_UMSK                                  (~(((1U<<GLB_GPADC_NEG_GND_LEN)-1)<<GLB_GPADC_NEG_GND_POS))
#define GLB_GPADC_MICBIAS_EN                                    GLB_GPADC_MICBIAS_EN
#define GLB_GPADC_MICBIAS_EN_POS                                (14U)
#define GLB_GPADC_MICBIAS_EN_LEN                                (1U)
#define GLB_GPADC_MICBIAS_EN_MSK                                (((1U<<GLB_GPADC_MICBIAS_EN_LEN)-1)<<GLB_GPADC_MICBIAS_EN_POS)
#define GLB_GPADC_MICBIAS_EN_UMSK                               (~(((1U<<GLB_GPADC_MICBIAS_EN_LEN)-1)<<GLB_GPADC_MICBIAS_EN_POS))
#define GLB_GPADC_MICPGA_EN                                     GLB_GPADC_MICPGA_EN
#define GLB_GPADC_MICPGA_EN_POS                                 (15U)
#define GLB_GPADC_MICPGA_EN_LEN                                 (1U)
#define GLB_GPADC_MICPGA_EN_MSK                                 (((1U<<GLB_GPADC_MICPGA_EN_LEN)-1)<<GLB_GPADC_MICPGA_EN_POS)
#define GLB_GPADC_MICPGA_EN_UMSK                                (~(((1U<<GLB_GPADC_MICPGA_EN_LEN)-1)<<GLB_GPADC_MICPGA_EN_POS))
#define GLB_GPADC_BYP_MICBOOST                                  GLB_GPADC_BYP_MICBOOST
#define GLB_GPADC_BYP_MICBOOST_POS                              (16U)
#define GLB_GPADC_BYP_MICBOOST_LEN                              (1U)
#define GLB_GPADC_BYP_MICBOOST_MSK                              (((1U<<GLB_GPADC_BYP_MICBOOST_LEN)-1)<<GLB_GPADC_BYP_MICBOOST_POS)
#define GLB_GPADC_BYP_MICBOOST_UMSK                             (~(((1U<<GLB_GPADC_BYP_MICBOOST_LEN)-1)<<GLB_GPADC_BYP_MICBOOST_POS))
#define GLB_GPADC_RCAL_EN                                       GLB_GPADC_RCAL_EN
#define GLB_GPADC_RCAL_EN_POS                                   (17U)
#define GLB_GPADC_RCAL_EN_LEN                                   (1U)
#define GLB_GPADC_RCAL_EN_MSK                                   (((1U<<GLB_GPADC_RCAL_EN_LEN)-1)<<GLB_GPADC_RCAL_EN_POS)
#define GLB_GPADC_RCAL_EN_UMSK                                  (~(((1U<<GLB_GPADC_RCAL_EN_LEN)-1)<<GLB_GPADC_RCAL_EN_POS))
#define GLB_GPADC_DWA_EN                                        GLB_GPADC_DWA_EN
#define GLB_GPADC_DWA_EN_POS                                    (18U)
#define GLB_GPADC_DWA_EN_LEN                                    (1U)
#define GLB_GPADC_DWA_EN_MSK                                    (((1U<<GLB_GPADC_DWA_EN_LEN)-1)<<GLB_GPADC_DWA_EN_POS)
#define GLB_GPADC_DWA_EN_UMSK                                   (~(((1U<<GLB_GPADC_DWA_EN_LEN)-1)<<GLB_GPADC_DWA_EN_POS))
#define GLB_GPADC_MIC2_DIFF                                     GLB_GPADC_MIC2_DIFF
#define GLB_GPADC_MIC2_DIFF_POS                                 (19U)
#define GLB_GPADC_MIC2_DIFF_LEN                                 (1U)
#define GLB_GPADC_MIC2_DIFF_MSK                                 (((1U<<GLB_GPADC_MIC2_DIFF_LEN)-1)<<GLB_GPADC_MIC2_DIFF_POS)
#define GLB_GPADC_MIC2_DIFF_UMSK                                (~(((1U<<GLB_GPADC_MIC2_DIFF_LEN)-1)<<GLB_GPADC_MIC2_DIFF_POS))
#define GLB_GPADC_MIC1_DIFF                                     GLB_GPADC_MIC1_DIFF
#define GLB_GPADC_MIC1_DIFF_POS                                 (20U)
#define GLB_GPADC_MIC1_DIFF_LEN                                 (1U)
#define GLB_GPADC_MIC1_DIFF_MSK                                 (((1U<<GLB_GPADC_MIC1_DIFF_LEN)-1)<<GLB_GPADC_MIC1_DIFF_POS)
#define GLB_GPADC_MIC1_DIFF_UMSK                                (~(((1U<<GLB_GPADC_MIC1_DIFF_LEN)-1)<<GLB_GPADC_MIC1_DIFF_POS))
#define GLB_GPADC_MIC_PGA2_GAIN                                 GLB_GPADC_MIC_PGA2_GAIN
#define GLB_GPADC_MIC_PGA2_GAIN_POS                             (21U)
#define GLB_GPADC_MIC_PGA2_GAIN_LEN                             (2U)
#define GLB_GPADC_MIC_PGA2_GAIN_MSK                             (((1U<<GLB_GPADC_MIC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_MIC_PGA2_GAIN_POS)
#define GLB_GPADC_MIC_PGA2_GAIN_UMSK                            (~(((1U<<GLB_GPADC_MIC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_MIC_PGA2_GAIN_POS))
#define GLB_GPADC_MICBOOST_32DB_EN                              GLB_GPADC_MICBOOST_32DB_EN
#define GLB_GPADC_MICBOOST_32DB_EN_POS                          (23U)
#define GLB_GPADC_MICBOOST_32DB_EN_LEN                          (1U)
#define GLB_GPADC_MICBOOST_32DB_EN_MSK                          (((1U<<GLB_GPADC_MICBOOST_32DB_EN_LEN)-1)<<GLB_GPADC_MICBOOST_32DB_EN_POS)
#define GLB_GPADC_MICBOOST_32DB_EN_UMSK                         (~(((1U<<GLB_GPADC_MICBOOST_32DB_EN_LEN)-1)<<GLB_GPADC_MICBOOST_32DB_EN_POS))
#define GLB_GPADC_CHIP_SEN_PU                                   GLB_GPADC_CHIP_SEN_PU
#define GLB_GPADC_CHIP_SEN_PU_POS                               (27U)
#define GLB_GPADC_CHIP_SEN_PU_LEN                               (1U)
#define GLB_GPADC_CHIP_SEN_PU_MSK                               (((1U<<GLB_GPADC_CHIP_SEN_PU_LEN)-1)<<GLB_GPADC_CHIP_SEN_PU_POS)
#define GLB_GPADC_CHIP_SEN_PU_UMSK                              (~(((1U<<GLB_GPADC_CHIP_SEN_PU_LEN)-1)<<GLB_GPADC_CHIP_SEN_PU_POS))
#define GLB_GPADC_SEN_SEL                                       GLB_GPADC_SEN_SEL
#define GLB_GPADC_SEN_SEL_POS                                   (28U)
#define GLB_GPADC_SEN_SEL_LEN                                   (3U)
#define GLB_GPADC_SEN_SEL_MSK                                   (((1U<<GLB_GPADC_SEN_SEL_LEN)-1)<<GLB_GPADC_SEN_SEL_POS)
#define GLB_GPADC_SEN_SEL_UMSK                                  (~(((1U<<GLB_GPADC_SEN_SEL_LEN)-1)<<GLB_GPADC_SEN_SEL_POS))
#define GLB_GPADC_SEN_TEST_EN                                   GLB_GPADC_SEN_TEST_EN
#define GLB_GPADC_SEN_TEST_EN_POS                               (31U)
#define GLB_GPADC_SEN_TEST_EN_LEN                               (1U)
#define GLB_GPADC_SEN_TEST_EN_MSK                               (((1U<<GLB_GPADC_SEN_TEST_EN_LEN)-1)<<GLB_GPADC_SEN_TEST_EN_POS)
#define GLB_GPADC_SEN_TEST_EN_UMSK                              (~(((1U<<GLB_GPADC_SEN_TEST_EN_LEN)-1)<<GLB_GPADC_SEN_TEST_EN_POS))

/* 0x510 : gpadc_reg_config1 */
#define GLB_GPADC_REG_CONFIG1_OFFSET                            (0x510)
#define GLB_GPADC_CONT_CONV_EN                                  GLB_GPADC_CONT_CONV_EN
#define GLB_GPADC_CONT_CONV_EN_POS                              (1U)
#define GLB_GPADC_CONT_CONV_EN_LEN                              (1U)
#define GLB_GPADC_CONT_CONV_EN_MSK                              (((1U<<GLB_GPADC_CONT_CONV_EN_LEN)-1)<<GLB_GPADC_CONT_CONV_EN_POS)
#define GLB_GPADC_CONT_CONV_EN_UMSK                             (~(((1U<<GLB_GPADC_CONT_CONV_EN_LEN)-1)<<GLB_GPADC_CONT_CONV_EN_POS))
#define GLB_GPADC_RES_SEL                                       GLB_GPADC_RES_SEL
#define GLB_GPADC_RES_SEL_POS                                   (2U)
#define GLB_GPADC_RES_SEL_LEN                                   (3U)
#define GLB_GPADC_RES_SEL_MSK                                   (((1U<<GLB_GPADC_RES_SEL_LEN)-1)<<GLB_GPADC_RES_SEL_POS)
#define GLB_GPADC_RES_SEL_UMSK                                  (~(((1U<<GLB_GPADC_RES_SEL_LEN)-1)<<GLB_GPADC_RES_SEL_POS))
#define GLB_GPADC_VCM_SEL_EN                                    GLB_GPADC_VCM_SEL_EN
#define GLB_GPADC_VCM_SEL_EN_POS                                (8U)
#define GLB_GPADC_VCM_SEL_EN_LEN                                (1U)
#define GLB_GPADC_VCM_SEL_EN_MSK                                (((1U<<GLB_GPADC_VCM_SEL_EN_LEN)-1)<<GLB_GPADC_VCM_SEL_EN_POS)
#define GLB_GPADC_VCM_SEL_EN_UMSK                               (~(((1U<<GLB_GPADC_VCM_SEL_EN_LEN)-1)<<GLB_GPADC_VCM_SEL_EN_POS))
#define GLB_GPADC_VCM_HYST_SEL                                  GLB_GPADC_VCM_HYST_SEL
#define GLB_GPADC_VCM_HYST_SEL_POS                              (9U)
#define GLB_GPADC_VCM_HYST_SEL_LEN                              (1U)
#define GLB_GPADC_VCM_HYST_SEL_MSK                              (((1U<<GLB_GPADC_VCM_HYST_SEL_LEN)-1)<<GLB_GPADC_VCM_HYST_SEL_POS)
#define GLB_GPADC_VCM_HYST_SEL_UMSK                             (~(((1U<<GLB_GPADC_VCM_HYST_SEL_LEN)-1)<<GLB_GPADC_VCM_HYST_SEL_POS))
#define GLB_GPADC_LOWV_DET_EN                                   GLB_GPADC_LOWV_DET_EN
#define GLB_GPADC_LOWV_DET_EN_POS                               (10U)
#define GLB_GPADC_LOWV_DET_EN_LEN                               (1U)
#define GLB_GPADC_LOWV_DET_EN_MSK                               (((1U<<GLB_GPADC_LOWV_DET_EN_LEN)-1)<<GLB_GPADC_LOWV_DET_EN_POS)
#define GLB_GPADC_LOWV_DET_EN_UMSK                              (~(((1U<<GLB_GPADC_LOWV_DET_EN_LEN)-1)<<GLB_GPADC_LOWV_DET_EN_POS))
#define GLB_GPADC_PWM_TRG_EN                                    GLB_GPADC_PWM_TRG_EN
#define GLB_GPADC_PWM_TRG_EN_POS                                (11U)
#define GLB_GPADC_PWM_TRG_EN_LEN                                (1U)
#define GLB_GPADC_PWM_TRG_EN_MSK                                (((1U<<GLB_GPADC_PWM_TRG_EN_LEN)-1)<<GLB_GPADC_PWM_TRG_EN_POS)
#define GLB_GPADC_PWM_TRG_EN_UMSK                               (~(((1U<<GLB_GPADC_PWM_TRG_EN_LEN)-1)<<GLB_GPADC_PWM_TRG_EN_POS))
#define GLB_GPADC_CLK_ANA_DLY                                   GLB_GPADC_CLK_ANA_DLY
#define GLB_GPADC_CLK_ANA_DLY_POS                               (12U)
#define GLB_GPADC_CLK_ANA_DLY_LEN                               (4U)
#define GLB_GPADC_CLK_ANA_DLY_MSK                               (((1U<<GLB_GPADC_CLK_ANA_DLY_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_POS)
#define GLB_GPADC_CLK_ANA_DLY_UMSK                              (~(((1U<<GLB_GPADC_CLK_ANA_DLY_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_POS))
#define GLB_GPADC_CLK_ANA_DLY_EN                                GLB_GPADC_CLK_ANA_DLY_EN
#define GLB_GPADC_CLK_ANA_DLY_EN_POS                            (16U)
#define GLB_GPADC_CLK_ANA_DLY_EN_LEN                            (1U)
#define GLB_GPADC_CLK_ANA_DLY_EN_MSK                            (((1U<<GLB_GPADC_CLK_ANA_DLY_EN_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_EN_POS)
#define GLB_GPADC_CLK_ANA_DLY_EN_UMSK                           (~(((1U<<GLB_GPADC_CLK_ANA_DLY_EN_LEN)-1)<<GLB_GPADC_CLK_ANA_DLY_EN_POS))
#define GLB_GPADC_CLK_ANA_INV                                   GLB_GPADC_CLK_ANA_INV
#define GLB_GPADC_CLK_ANA_INV_POS                               (17U)
#define GLB_GPADC_CLK_ANA_INV_LEN                               (1U)
#define GLB_GPADC_CLK_ANA_INV_MSK                               (((1U<<GLB_GPADC_CLK_ANA_INV_LEN)-1)<<GLB_GPADC_CLK_ANA_INV_POS)
#define GLB_GPADC_CLK_ANA_INV_UMSK                              (~(((1U<<GLB_GPADC_CLK_ANA_INV_LEN)-1)<<GLB_GPADC_CLK_ANA_INV_POS))
#define GLB_GPADC_CLK_DIV_RATIO                                 GLB_GPADC_CLK_DIV_RATIO
#define GLB_GPADC_CLK_DIV_RATIO_POS                             (18U)
#define GLB_GPADC_CLK_DIV_RATIO_LEN                             (3U)
#define GLB_GPADC_CLK_DIV_RATIO_MSK                             (((1U<<GLB_GPADC_CLK_DIV_RATIO_LEN)-1)<<GLB_GPADC_CLK_DIV_RATIO_POS)
#define GLB_GPADC_CLK_DIV_RATIO_UMSK                            (~(((1U<<GLB_GPADC_CLK_DIV_RATIO_LEN)-1)<<GLB_GPADC_CLK_DIV_RATIO_POS))
#define GLB_GPADC_SCAN_LENGTH                                   GLB_GPADC_SCAN_LENGTH
#define GLB_GPADC_SCAN_LENGTH_POS                               (21U)
#define GLB_GPADC_SCAN_LENGTH_LEN                               (4U)
#define GLB_GPADC_SCAN_LENGTH_MSK                               (((1U<<GLB_GPADC_SCAN_LENGTH_LEN)-1)<<GLB_GPADC_SCAN_LENGTH_POS)
#define GLB_GPADC_SCAN_LENGTH_UMSK                              (~(((1U<<GLB_GPADC_SCAN_LENGTH_LEN)-1)<<GLB_GPADC_SCAN_LENGTH_POS))
#define GLB_GPADC_SCAN_EN                                       GLB_GPADC_SCAN_EN
#define GLB_GPADC_SCAN_EN_POS                                   (25U)
#define GLB_GPADC_SCAN_EN_LEN                                   (1U)
#define GLB_GPADC_SCAN_EN_MSK                                   (((1U<<GLB_GPADC_SCAN_EN_LEN)-1)<<GLB_GPADC_SCAN_EN_POS)
#define GLB_GPADC_SCAN_EN_UMSK                                  (~(((1U<<GLB_GPADC_SCAN_EN_LEN)-1)<<GLB_GPADC_SCAN_EN_POS))
#define GLB_GPADC_DITHER_EN                                     GLB_GPADC_DITHER_EN
#define GLB_GPADC_DITHER_EN_POS                                 (26U)
#define GLB_GPADC_DITHER_EN_LEN                                 (1U)
#define GLB_GPADC_DITHER_EN_MSK                                 (((1U<<GLB_GPADC_DITHER_EN_LEN)-1)<<GLB_GPADC_DITHER_EN_POS)
#define GLB_GPADC_DITHER_EN_UMSK                                (~(((1U<<GLB_GPADC_DITHER_EN_LEN)-1)<<GLB_GPADC_DITHER_EN_POS))
#define GLB_GPADC_V11_SEL                                       GLB_GPADC_V11_SEL
#define GLB_GPADC_V11_SEL_POS                                   (27U)
#define GLB_GPADC_V11_SEL_LEN                                   (2U)
#define GLB_GPADC_V11_SEL_MSK                                   (((1U<<GLB_GPADC_V11_SEL_LEN)-1)<<GLB_GPADC_V11_SEL_POS)
#define GLB_GPADC_V11_SEL_UMSK                                  (~(((1U<<GLB_GPADC_V11_SEL_LEN)-1)<<GLB_GPADC_V11_SEL_POS))
#define GLB_GPADC_V18_SEL                                       GLB_GPADC_V18_SEL
#define GLB_GPADC_V18_SEL_POS                                   (29U)
#define GLB_GPADC_V18_SEL_LEN                                   (2U)
#define GLB_GPADC_V18_SEL_MSK                                   (((1U<<GLB_GPADC_V18_SEL_LEN)-1)<<GLB_GPADC_V18_SEL_POS)
#define GLB_GPADC_V18_SEL_UMSK                                  (~(((1U<<GLB_GPADC_V18_SEL_LEN)-1)<<GLB_GPADC_V18_SEL_POS))

/* 0x514 : gpadc_reg_config2 */
#define GLB_GPADC_REG_CONFIG2_OFFSET                            (0x514)
#define GLB_GPADC_DIFF_MODE                                     GLB_GPADC_DIFF_MODE
#define GLB_GPADC_DIFF_MODE_POS                                 (2U)
#define GLB_GPADC_DIFF_MODE_LEN                                 (1U)
#define GLB_GPADC_DIFF_MODE_MSK                                 (((1U<<GLB_GPADC_DIFF_MODE_LEN)-1)<<GLB_GPADC_DIFF_MODE_POS)
#define GLB_GPADC_DIFF_MODE_UMSK                                (~(((1U<<GLB_GPADC_DIFF_MODE_LEN)-1)<<GLB_GPADC_DIFF_MODE_POS))
#define GLB_GPADC_VREF_SEL                                      GLB_GPADC_VREF_SEL
#define GLB_GPADC_VREF_SEL_POS                                  (3U)
#define GLB_GPADC_VREF_SEL_LEN                                  (1U)
#define GLB_GPADC_VREF_SEL_MSK                                  (((1U<<GLB_GPADC_VREF_SEL_LEN)-1)<<GLB_GPADC_VREF_SEL_POS)
#define GLB_GPADC_VREF_SEL_UMSK                                 (~(((1U<<GLB_GPADC_VREF_SEL_LEN)-1)<<GLB_GPADC_VREF_SEL_POS))
#define GLB_GPADC_VBAT_EN                                       GLB_GPADC_VBAT_EN
#define GLB_GPADC_VBAT_EN_POS                                   (4U)
#define GLB_GPADC_VBAT_EN_LEN                                   (1U)
#define GLB_GPADC_VBAT_EN_MSK                                   (((1U<<GLB_GPADC_VBAT_EN_LEN)-1)<<GLB_GPADC_VBAT_EN_POS)
#define GLB_GPADC_VBAT_EN_UMSK                                  (~(((1U<<GLB_GPADC_VBAT_EN_LEN)-1)<<GLB_GPADC_VBAT_EN_POS))
#define GLB_GPADC_TSEXT_SEL                                     GLB_GPADC_TSEXT_SEL
#define GLB_GPADC_TSEXT_SEL_POS                                 (5U)
#define GLB_GPADC_TSEXT_SEL_LEN                                 (1U)
#define GLB_GPADC_TSEXT_SEL_MSK                                 (((1U<<GLB_GPADC_TSEXT_SEL_LEN)-1)<<GLB_GPADC_TSEXT_SEL_POS)
#define GLB_GPADC_TSEXT_SEL_UMSK                                (~(((1U<<GLB_GPADC_TSEXT_SEL_LEN)-1)<<GLB_GPADC_TSEXT_SEL_POS))
#define GLB_GPADC_TS_EN                                         GLB_GPADC_TS_EN
#define GLB_GPADC_TS_EN_POS                                     (6U)
#define GLB_GPADC_TS_EN_LEN                                     (1U)
#define GLB_GPADC_TS_EN_MSK                                     (((1U<<GLB_GPADC_TS_EN_LEN)-1)<<GLB_GPADC_TS_EN_POS)
#define GLB_GPADC_TS_EN_UMSK                                    (~(((1U<<GLB_GPADC_TS_EN_LEN)-1)<<GLB_GPADC_TS_EN_POS))
#define GLB_GPADC_PGA_VCM                                       GLB_GPADC_PGA_VCM
#define GLB_GPADC_PGA_VCM_POS                                   (7U)
#define GLB_GPADC_PGA_VCM_LEN                                   (2U)
#define GLB_GPADC_PGA_VCM_MSK                                   (((1U<<GLB_GPADC_PGA_VCM_LEN)-1)<<GLB_GPADC_PGA_VCM_POS)
#define GLB_GPADC_PGA_VCM_UMSK                                  (~(((1U<<GLB_GPADC_PGA_VCM_LEN)-1)<<GLB_GPADC_PGA_VCM_POS))
#define GLB_GPADC_PGA_OS_CAL                                    GLB_GPADC_PGA_OS_CAL
#define GLB_GPADC_PGA_OS_CAL_POS                                (9U)
#define GLB_GPADC_PGA_OS_CAL_LEN                                (4U)
#define GLB_GPADC_PGA_OS_CAL_MSK                                (((1U<<GLB_GPADC_PGA_OS_CAL_LEN)-1)<<GLB_GPADC_PGA_OS_CAL_POS)
#define GLB_GPADC_PGA_OS_CAL_UMSK                               (~(((1U<<GLB_GPADC_PGA_OS_CAL_LEN)-1)<<GLB_GPADC_PGA_OS_CAL_POS))
#define GLB_GPADC_PGA_EN                                        GLB_GPADC_PGA_EN
#define GLB_GPADC_PGA_EN_POS                                    (13U)
#define GLB_GPADC_PGA_EN_LEN                                    (1U)
#define GLB_GPADC_PGA_EN_MSK                                    (((1U<<GLB_GPADC_PGA_EN_LEN)-1)<<GLB_GPADC_PGA_EN_POS)
#define GLB_GPADC_PGA_EN_UMSK                                   (~(((1U<<GLB_GPADC_PGA_EN_LEN)-1)<<GLB_GPADC_PGA_EN_POS))
#define GLB_GPADC_PGA_VCMI_EN                                   GLB_GPADC_PGA_VCMI_EN
#define GLB_GPADC_PGA_VCMI_EN_POS                               (14U)
#define GLB_GPADC_PGA_VCMI_EN_LEN                               (1U)
#define GLB_GPADC_PGA_VCMI_EN_MSK                               (((1U<<GLB_GPADC_PGA_VCMI_EN_LEN)-1)<<GLB_GPADC_PGA_VCMI_EN_POS)
#define GLB_GPADC_PGA_VCMI_EN_UMSK                              (~(((1U<<GLB_GPADC_PGA_VCMI_EN_LEN)-1)<<GLB_GPADC_PGA_VCMI_EN_POS))
#define GLB_GPADC_CHOP_MODE                                     GLB_GPADC_CHOP_MODE
#define GLB_GPADC_CHOP_MODE_POS                                 (15U)
#define GLB_GPADC_CHOP_MODE_LEN                                 (2U)
#define GLB_GPADC_CHOP_MODE_MSK                                 (((1U<<GLB_GPADC_CHOP_MODE_LEN)-1)<<GLB_GPADC_CHOP_MODE_POS)
#define GLB_GPADC_CHOP_MODE_UMSK                                (~(((1U<<GLB_GPADC_CHOP_MODE_LEN)-1)<<GLB_GPADC_CHOP_MODE_POS))
#define GLB_GPADC_BIAS_SEL                                      GLB_GPADC_BIAS_SEL
#define GLB_GPADC_BIAS_SEL_POS                                  (17U)
#define GLB_GPADC_BIAS_SEL_LEN                                  (1U)
#define GLB_GPADC_BIAS_SEL_MSK                                  (((1U<<GLB_GPADC_BIAS_SEL_LEN)-1)<<GLB_GPADC_BIAS_SEL_POS)
#define GLB_GPADC_BIAS_SEL_UMSK                                 (~(((1U<<GLB_GPADC_BIAS_SEL_LEN)-1)<<GLB_GPADC_BIAS_SEL_POS))
#define GLB_GPADC_TEST_EN                                       GLB_GPADC_TEST_EN
#define GLB_GPADC_TEST_EN_POS                                   (18U)
#define GLB_GPADC_TEST_EN_LEN                                   (1U)
#define GLB_GPADC_TEST_EN_MSK                                   (((1U<<GLB_GPADC_TEST_EN_LEN)-1)<<GLB_GPADC_TEST_EN_POS)
#define GLB_GPADC_TEST_EN_UMSK                                  (~(((1U<<GLB_GPADC_TEST_EN_LEN)-1)<<GLB_GPADC_TEST_EN_POS))
#define GLB_GPADC_TEST_SEL                                      GLB_GPADC_TEST_SEL
#define GLB_GPADC_TEST_SEL_POS                                  (19U)
#define GLB_GPADC_TEST_SEL_LEN                                  (3U)
#define GLB_GPADC_TEST_SEL_MSK                                  (((1U<<GLB_GPADC_TEST_SEL_LEN)-1)<<GLB_GPADC_TEST_SEL_POS)
#define GLB_GPADC_TEST_SEL_UMSK                                 (~(((1U<<GLB_GPADC_TEST_SEL_LEN)-1)<<GLB_GPADC_TEST_SEL_POS))
#define GLB_GPADC_PGA2_GAIN                                     GLB_GPADC_PGA2_GAIN
#define GLB_GPADC_PGA2_GAIN_POS                                 (22U)
#define GLB_GPADC_PGA2_GAIN_LEN                                 (3U)
#define GLB_GPADC_PGA2_GAIN_MSK                                 (((1U<<GLB_GPADC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_PGA2_GAIN_POS)
#define GLB_GPADC_PGA2_GAIN_UMSK                                (~(((1U<<GLB_GPADC_PGA2_GAIN_LEN)-1)<<GLB_GPADC_PGA2_GAIN_POS))
#define GLB_GPADC_PGA1_GAIN                                     GLB_GPADC_PGA1_GAIN
#define GLB_GPADC_PGA1_GAIN_POS                                 (25U)
#define GLB_GPADC_PGA1_GAIN_LEN                                 (3U)
#define GLB_GPADC_PGA1_GAIN_MSK                                 (((1U<<GLB_GPADC_PGA1_GAIN_LEN)-1)<<GLB_GPADC_PGA1_GAIN_POS)
#define GLB_GPADC_PGA1_GAIN_UMSK                                (~(((1U<<GLB_GPADC_PGA1_GAIN_LEN)-1)<<GLB_GPADC_PGA1_GAIN_POS))
#define GLB_GPADC_DLY_SEL                                       GLB_GPADC_DLY_SEL
#define GLB_GPADC_DLY_SEL_POS                                   (28U)
#define GLB_GPADC_DLY_SEL_LEN                                   (3U)
#define GLB_GPADC_DLY_SEL_MSK                                   (((1U<<GLB_GPADC_DLY_SEL_LEN)-1)<<GLB_GPADC_DLY_SEL_POS)
#define GLB_GPADC_DLY_SEL_UMSK                                  (~(((1U<<GLB_GPADC_DLY_SEL_LEN)-1)<<GLB_GPADC_DLY_SEL_POS))
#define GLB_GPADC_TSVBE_LOW                                     GLB_GPADC_TSVBE_LOW
#define GLB_GPADC_TSVBE_LOW_POS                                 (31U)
#define GLB_GPADC_TSVBE_LOW_LEN                                 (1U)
#define GLB_GPADC_TSVBE_LOW_MSK                                 (((1U<<GLB_GPADC_TSVBE_LOW_LEN)-1)<<GLB_GPADC_TSVBE_LOW_POS)
#define GLB_GPADC_TSVBE_LOW_UMSK                                (~(((1U<<GLB_GPADC_TSVBE_LOW_LEN)-1)<<GLB_GPADC_TSVBE_LOW_POS))

/* 0x518 : adc converation sequence 1 */
#define GLB_GPADC_REG_SCN_POS1_OFFSET                           (0x518)
#define GLB_GPADC_SCAN_POS_0                                    GLB_GPADC_SCAN_POS_0
#define GLB_GPADC_SCAN_POS_0_POS                                (0U)
#define GLB_GPADC_SCAN_POS_0_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_0_MSK                                (((1U<<GLB_GPADC_SCAN_POS_0_LEN)-1)<<GLB_GPADC_SCAN_POS_0_POS)
#define GLB_GPADC_SCAN_POS_0_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_0_LEN)-1)<<GLB_GPADC_SCAN_POS_0_POS))
#define GLB_GPADC_SCAN_POS_1                                    GLB_GPADC_SCAN_POS_1
#define GLB_GPADC_SCAN_POS_1_POS                                (5U)
#define GLB_GPADC_SCAN_POS_1_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_1_MSK                                (((1U<<GLB_GPADC_SCAN_POS_1_LEN)-1)<<GLB_GPADC_SCAN_POS_1_POS)
#define GLB_GPADC_SCAN_POS_1_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_1_LEN)-1)<<GLB_GPADC_SCAN_POS_1_POS))
#define GLB_GPADC_SCAN_POS_2                                    GLB_GPADC_SCAN_POS_2
#define GLB_GPADC_SCAN_POS_2_POS                                (10U)
#define GLB_GPADC_SCAN_POS_2_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_2_MSK                                (((1U<<GLB_GPADC_SCAN_POS_2_LEN)-1)<<GLB_GPADC_SCAN_POS_2_POS)
#define GLB_GPADC_SCAN_POS_2_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_2_LEN)-1)<<GLB_GPADC_SCAN_POS_2_POS))
#define GLB_GPADC_SCAN_POS_3                                    GLB_GPADC_SCAN_POS_3
#define GLB_GPADC_SCAN_POS_3_POS                                (15U)
#define GLB_GPADC_SCAN_POS_3_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_3_MSK                                (((1U<<GLB_GPADC_SCAN_POS_3_LEN)-1)<<GLB_GPADC_SCAN_POS_3_POS)
#define GLB_GPADC_SCAN_POS_3_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_3_LEN)-1)<<GLB_GPADC_SCAN_POS_3_POS))
#define GLB_GPADC_SCAN_POS_4                                    GLB_GPADC_SCAN_POS_4
#define GLB_GPADC_SCAN_POS_4_POS                                (20U)
#define GLB_GPADC_SCAN_POS_4_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_4_MSK                                (((1U<<GLB_GPADC_SCAN_POS_4_LEN)-1)<<GLB_GPADC_SCAN_POS_4_POS)
#define GLB_GPADC_SCAN_POS_4_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_4_LEN)-1)<<GLB_GPADC_SCAN_POS_4_POS))
#define GLB_GPADC_SCAN_POS_5                                    GLB_GPADC_SCAN_POS_5
#define GLB_GPADC_SCAN_POS_5_POS                                (25U)
#define GLB_GPADC_SCAN_POS_5_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_5_MSK                                (((1U<<GLB_GPADC_SCAN_POS_5_LEN)-1)<<GLB_GPADC_SCAN_POS_5_POS)
#define GLB_GPADC_SCAN_POS_5_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_5_LEN)-1)<<GLB_GPADC_SCAN_POS_5_POS))

/* 0x51C : adc converation sequence 2 */
#define GLB_GPADC_REG_SCN_POS2_OFFSET                           (0x51C)
#define GLB_GPADC_SCAN_POS_6                                    GLB_GPADC_SCAN_POS_6
#define GLB_GPADC_SCAN_POS_6_POS                                (0U)
#define GLB_GPADC_SCAN_POS_6_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_6_MSK                                (((1U<<GLB_GPADC_SCAN_POS_6_LEN)-1)<<GLB_GPADC_SCAN_POS_6_POS)
#define GLB_GPADC_SCAN_POS_6_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_6_LEN)-1)<<GLB_GPADC_SCAN_POS_6_POS))
#define GLB_GPADC_SCAN_POS_7                                    GLB_GPADC_SCAN_POS_7
#define GLB_GPADC_SCAN_POS_7_POS                                (5U)
#define GLB_GPADC_SCAN_POS_7_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_7_MSK                                (((1U<<GLB_GPADC_SCAN_POS_7_LEN)-1)<<GLB_GPADC_SCAN_POS_7_POS)
#define GLB_GPADC_SCAN_POS_7_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_7_LEN)-1)<<GLB_GPADC_SCAN_POS_7_POS))
#define GLB_GPADC_SCAN_POS_8                                    GLB_GPADC_SCAN_POS_8
#define GLB_GPADC_SCAN_POS_8_POS                                (10U)
#define GLB_GPADC_SCAN_POS_8_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_8_MSK                                (((1U<<GLB_GPADC_SCAN_POS_8_LEN)-1)<<GLB_GPADC_SCAN_POS_8_POS)
#define GLB_GPADC_SCAN_POS_8_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_8_LEN)-1)<<GLB_GPADC_SCAN_POS_8_POS))
#define GLB_GPADC_SCAN_POS_9                                    GLB_GPADC_SCAN_POS_9
#define GLB_GPADC_SCAN_POS_9_POS                                (15U)
#define GLB_GPADC_SCAN_POS_9_LEN                                (5U)
#define GLB_GPADC_SCAN_POS_9_MSK                                (((1U<<GLB_GPADC_SCAN_POS_9_LEN)-1)<<GLB_GPADC_SCAN_POS_9_POS)
#define GLB_GPADC_SCAN_POS_9_UMSK                               (~(((1U<<GLB_GPADC_SCAN_POS_9_LEN)-1)<<GLB_GPADC_SCAN_POS_9_POS))
#define GLB_GPADC_SCAN_POS_10                                   GLB_GPADC_SCAN_POS_10
#define GLB_GPADC_SCAN_POS_10_POS                               (20U)
#define GLB_GPADC_SCAN_POS_10_LEN                               (5U)
#define GLB_GPADC_SCAN_POS_10_MSK                               (((1U<<GLB_GPADC_SCAN_POS_10_LEN)-1)<<GLB_GPADC_SCAN_POS_10_POS)
#define GLB_GPADC_SCAN_POS_10_UMSK                              (~(((1U<<GLB_GPADC_SCAN_POS_10_LEN)-1)<<GLB_GPADC_SCAN_POS_10_POS))
#define GLB_GPADC_SCAN_POS_11                                   GLB_GPADC_SCAN_POS_11
#define GLB_GPADC_SCAN_POS_11_POS                               (25U)
#define GLB_GPADC_SCAN_POS_11_LEN                               (5U)
#define GLB_GPADC_SCAN_POS_11_MSK                               (((1U<<GLB_GPADC_SCAN_POS_11_LEN)-1)<<GLB_GPADC_SCAN_POS_11_POS)
#define GLB_GPADC_SCAN_POS_11_UMSK                              (~(((1U<<GLB_GPADC_SCAN_POS_11_LEN)-1)<<GLB_GPADC_SCAN_POS_11_POS))

/* 0x520 : adc converation sequence 3 */
#define GLB_GPADC_REG_SCN_NEG1_OFFSET                           (0x520)
#define GLB_GPADC_SCAN_NEG_0                                    GLB_GPADC_SCAN_NEG_0
#define GLB_GPADC_SCAN_NEG_0_POS                                (0U)
#define GLB_GPADC_SCAN_NEG_0_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_0_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_0_LEN)-1)<<GLB_GPADC_SCAN_NEG_0_POS)
#define GLB_GPADC_SCAN_NEG_0_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_0_LEN)-1)<<GLB_GPADC_SCAN_NEG_0_POS))
#define GLB_GPADC_SCAN_NEG_1                                    GLB_GPADC_SCAN_NEG_1
#define GLB_GPADC_SCAN_NEG_1_POS                                (5U)
#define GLB_GPADC_SCAN_NEG_1_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_1_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_1_LEN)-1)<<GLB_GPADC_SCAN_NEG_1_POS)
#define GLB_GPADC_SCAN_NEG_1_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_1_LEN)-1)<<GLB_GPADC_SCAN_NEG_1_POS))
#define GLB_GPADC_SCAN_NEG_2                                    GLB_GPADC_SCAN_NEG_2
#define GLB_GPADC_SCAN_NEG_2_POS                                (10U)
#define GLB_GPADC_SCAN_NEG_2_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_2_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_2_LEN)-1)<<GLB_GPADC_SCAN_NEG_2_POS)
#define GLB_GPADC_SCAN_NEG_2_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_2_LEN)-1)<<GLB_GPADC_SCAN_NEG_2_POS))
#define GLB_GPADC_SCAN_NEG_3                                    GLB_GPADC_SCAN_NEG_3
#define GLB_GPADC_SCAN_NEG_3_POS                                (15U)
#define GLB_GPADC_SCAN_NEG_3_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_3_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_3_LEN)-1)<<GLB_GPADC_SCAN_NEG_3_POS)
#define GLB_GPADC_SCAN_NEG_3_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_3_LEN)-1)<<GLB_GPADC_SCAN_NEG_3_POS))
#define GLB_GPADC_SCAN_NEG_4                                    GLB_GPADC_SCAN_NEG_4
#define GLB_GPADC_SCAN_NEG_4_POS                                (20U)
#define GLB_GPADC_SCAN_NEG_4_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_4_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_4_LEN)-1)<<GLB_GPADC_SCAN_NEG_4_POS)
#define GLB_GPADC_SCAN_NEG_4_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_4_LEN)-1)<<GLB_GPADC_SCAN_NEG_4_POS))
#define GLB_GPADC_SCAN_NEG_5                                    GLB_GPADC_SCAN_NEG_5
#define GLB_GPADC_SCAN_NEG_5_POS                                (25U)
#define GLB_GPADC_SCAN_NEG_5_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_5_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_5_LEN)-1)<<GLB_GPADC_SCAN_NEG_5_POS)
#define GLB_GPADC_SCAN_NEG_5_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_5_LEN)-1)<<GLB_GPADC_SCAN_NEG_5_POS))

/* 0x524 : adc converation sequence 4 */
#define GLB_GPADC_REG_SCN_NEG2_OFFSET                           (0x524)
#define GLB_GPADC_SCAN_NEG_6                                    GLB_GPADC_SCAN_NEG_6
#define GLB_GPADC_SCAN_NEG_6_POS                                (0U)
#define GLB_GPADC_SCAN_NEG_6_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_6_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_6_LEN)-1)<<GLB_GPADC_SCAN_NEG_6_POS)
#define GLB_GPADC_SCAN_NEG_6_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_6_LEN)-1)<<GLB_GPADC_SCAN_NEG_6_POS))
#define GLB_GPADC_SCAN_NEG_7                                    GLB_GPADC_SCAN_NEG_7
#define GLB_GPADC_SCAN_NEG_7_POS                                (5U)
#define GLB_GPADC_SCAN_NEG_7_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_7_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_7_LEN)-1)<<GLB_GPADC_SCAN_NEG_7_POS)
#define GLB_GPADC_SCAN_NEG_7_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_7_LEN)-1)<<GLB_GPADC_SCAN_NEG_7_POS))
#define GLB_GPADC_SCAN_NEG_8                                    GLB_GPADC_SCAN_NEG_8
#define GLB_GPADC_SCAN_NEG_8_POS                                (10U)
#define GLB_GPADC_SCAN_NEG_8_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_8_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_8_LEN)-1)<<GLB_GPADC_SCAN_NEG_8_POS)
#define GLB_GPADC_SCAN_NEG_8_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_8_LEN)-1)<<GLB_GPADC_SCAN_NEG_8_POS))
#define GLB_GPADC_SCAN_NEG_9                                    GLB_GPADC_SCAN_NEG_9
#define GLB_GPADC_SCAN_NEG_9_POS                                (15U)
#define GLB_GPADC_SCAN_NEG_9_LEN                                (5U)
#define GLB_GPADC_SCAN_NEG_9_MSK                                (((1U<<GLB_GPADC_SCAN_NEG_9_LEN)-1)<<GLB_GPADC_SCAN_NEG_9_POS)
#define GLB_GPADC_SCAN_NEG_9_UMSK                               (~(((1U<<GLB_GPADC_SCAN_NEG_9_LEN)-1)<<GLB_GPADC_SCAN_NEG_9_POS))
#define GLB_GPADC_SCAN_NEG_10                                   GLB_GPADC_SCAN_NEG_10
#define GLB_GPADC_SCAN_NEG_10_POS                               (20U)
#define GLB_GPADC_SCAN_NEG_10_LEN                               (5U)
#define GLB_GPADC_SCAN_NEG_10_MSK                               (((1U<<GLB_GPADC_SCAN_NEG_10_LEN)-1)<<GLB_GPADC_SCAN_NEG_10_POS)
#define GLB_GPADC_SCAN_NEG_10_UMSK                              (~(((1U<<GLB_GPADC_SCAN_NEG_10_LEN)-1)<<GLB_GPADC_SCAN_NEG_10_POS))
#define GLB_GPADC_SCAN_NEG_11                                   GLB_GPADC_SCAN_NEG_11
#define GLB_GPADC_SCAN_NEG_11_POS                               (25U)
#define GLB_GPADC_SCAN_NEG_11_LEN                               (5U)
#define GLB_GPADC_SCAN_NEG_11_MSK                               (((1U<<GLB_GPADC_SCAN_NEG_11_LEN)-1)<<GLB_GPADC_SCAN_NEG_11_POS)
#define GLB_GPADC_SCAN_NEG_11_UMSK                              (~(((1U<<GLB_GPADC_SCAN_NEG_11_LEN)-1)<<GLB_GPADC_SCAN_NEG_11_POS))

/* 0x528 : gpadc_reg_status */
#define GLB_GPADC_REG_STATUS_OFFSET                             (0x528)
#define GLB_GPADC_DATA_RDY                                      GLB_GPADC_DATA_RDY
#define GLB_GPADC_DATA_RDY_POS                                  (0U)
#define GLB_GPADC_DATA_RDY_LEN                                  (1U)
#define GLB_GPADC_DATA_RDY_MSK                                  (((1U<<GLB_GPADC_DATA_RDY_LEN)-1)<<GLB_GPADC_DATA_RDY_POS)
#define GLB_GPADC_DATA_RDY_UMSK                                 (~(((1U<<GLB_GPADC_DATA_RDY_LEN)-1)<<GLB_GPADC_DATA_RDY_POS))
#define GLB_GPADC_RESERVED                                      GLB_GPADC_RESERVED
#define GLB_GPADC_RESERVED_POS                                  (16U)
#define GLB_GPADC_RESERVED_LEN                                  (16U)
#define GLB_GPADC_RESERVED_MSK                                  (((1U<<GLB_GPADC_RESERVED_LEN)-1)<<GLB_GPADC_RESERVED_POS)
#define GLB_GPADC_RESERVED_UMSK                                 (~(((1U<<GLB_GPADC_RESERVED_LEN)-1)<<GLB_GPADC_RESERVED_POS))

/* 0x52C : gpadc_reg_isr */
#define GLB_GPADC_REG_ISR_OFFSET                                (0x52C)
#define GLB_GPADC_NEG_SATUR                                     GLB_GPADC_NEG_SATUR
#define GLB_GPADC_NEG_SATUR_POS                                 (0U)
#define GLB_GPADC_NEG_SATUR_LEN                                 (1U)
#define GLB_GPADC_NEG_SATUR_MSK                                 (((1U<<GLB_GPADC_NEG_SATUR_LEN)-1)<<GLB_GPADC_NEG_SATUR_POS)
#define GLB_GPADC_NEG_SATUR_UMSK                                (~(((1U<<GLB_GPADC_NEG_SATUR_LEN)-1)<<GLB_GPADC_NEG_SATUR_POS))
#define GLB_GPADC_POS_SATUR                                     GLB_GPADC_POS_SATUR
#define GLB_GPADC_POS_SATUR_POS                                 (1U)
#define GLB_GPADC_POS_SATUR_LEN                                 (1U)
#define GLB_GPADC_POS_SATUR_MSK                                 (((1U<<GLB_GPADC_POS_SATUR_LEN)-1)<<GLB_GPADC_POS_SATUR_POS)
#define GLB_GPADC_POS_SATUR_UMSK                                (~(((1U<<GLB_GPADC_POS_SATUR_LEN)-1)<<GLB_GPADC_POS_SATUR_POS))
#define GLB_GPADC_NEG_SATUR_CLR                                 GLB_GPADC_NEG_SATUR_CLR
#define GLB_GPADC_NEG_SATUR_CLR_POS                             (4U)
#define GLB_GPADC_NEG_SATUR_CLR_LEN                             (1U)
#define GLB_GPADC_NEG_SATUR_CLR_MSK                             (((1U<<GLB_GPADC_NEG_SATUR_CLR_LEN)-1)<<GLB_GPADC_NEG_SATUR_CLR_POS)
#define GLB_GPADC_NEG_SATUR_CLR_UMSK                            (~(((1U<<GLB_GPADC_NEG_SATUR_CLR_LEN)-1)<<GLB_GPADC_NEG_SATUR_CLR_POS))
#define GLB_GPADC_POS_SATUR_CLR                                 GLB_GPADC_POS_SATUR_CLR
#define GLB_GPADC_POS_SATUR_CLR_POS                             (5U)
#define GLB_GPADC_POS_SATUR_CLR_LEN                             (1U)
#define GLB_GPADC_POS_SATUR_CLR_MSK                             (((1U<<GLB_GPADC_POS_SATUR_CLR_LEN)-1)<<GLB_GPADC_POS_SATUR_CLR_POS)
#define GLB_GPADC_POS_SATUR_CLR_UMSK                            (~(((1U<<GLB_GPADC_POS_SATUR_CLR_LEN)-1)<<GLB_GPADC_POS_SATUR_CLR_POS))
#define GLB_GPADC_NEG_SATUR_MASK                                GLB_GPADC_NEG_SATUR_MASK
#define GLB_GPADC_NEG_SATUR_MASK_POS                            (8U)
#define GLB_GPADC_NEG_SATUR_MASK_LEN                            (1U)
#define GLB_GPADC_NEG_SATUR_MASK_MSK                            (((1U<<GLB_GPADC_NEG_SATUR_MASK_LEN)-1)<<GLB_GPADC_NEG_SATUR_MASK_POS)
#define GLB_GPADC_NEG_SATUR_MASK_UMSK                           (~(((1U<<GLB_GPADC_NEG_SATUR_MASK_LEN)-1)<<GLB_GPADC_NEG_SATUR_MASK_POS))
#define GLB_GPADC_POS_SATUR_MASK                                GLB_GPADC_POS_SATUR_MASK
#define GLB_GPADC_POS_SATUR_MASK_POS                            (9U)
#define GLB_GPADC_POS_SATUR_MASK_LEN                            (1U)
#define GLB_GPADC_POS_SATUR_MASK_MSK                            (((1U<<GLB_GPADC_POS_SATUR_MASK_LEN)-1)<<GLB_GPADC_POS_SATUR_MASK_POS)
#define GLB_GPADC_POS_SATUR_MASK_UMSK                           (~(((1U<<GLB_GPADC_POS_SATUR_MASK_LEN)-1)<<GLB_GPADC_POS_SATUR_MASK_POS))

/* 0x530 : gpadc_reg_result */
#define GLB_GPADC_REG_RESULT_OFFSET                             (0x530)
#define GLB_GPADC_DATA_OUT                                      GLB_GPADC_DATA_OUT
#define GLB_GPADC_DATA_OUT_POS                                  (0U)
#define GLB_GPADC_DATA_OUT_LEN                                  (26U)
#define GLB_GPADC_DATA_OUT_MSK                                  (((1U<<GLB_GPADC_DATA_OUT_LEN)-1)<<GLB_GPADC_DATA_OUT_POS)
#define GLB_GPADC_DATA_OUT_UMSK                                 (~(((1U<<GLB_GPADC_DATA_OUT_LEN)-1)<<GLB_GPADC_DATA_OUT_POS))

/* 0x534 : gpadc_reg_raw_result */
#define GLB_GPADC_REG_RAW_RESULT_OFFSET                         (0x534)
#define GLB_GPADC_RAW_DATA                                      GLB_GPADC_RAW_DATA
#define GLB_GPADC_RAW_DATA_POS                                  (0U)
#define GLB_GPADC_RAW_DATA_LEN                                  (12U)
#define GLB_GPADC_RAW_DATA_MSK                                  (((1U<<GLB_GPADC_RAW_DATA_LEN)-1)<<GLB_GPADC_RAW_DATA_POS)
#define GLB_GPADC_RAW_DATA_UMSK                                 (~(((1U<<GLB_GPADC_RAW_DATA_LEN)-1)<<GLB_GPADC_RAW_DATA_POS))

/* 0x538 : gpadc_reg_define */
#define GLB_GPADC_REG_DEFINE_OFFSET                             (0x538)
#define GLB_GPADC_OS_CAL_DATA                                   GLB_GPADC_OS_CAL_DATA
#define GLB_GPADC_OS_CAL_DATA_POS                               (0U)
#define GLB_GPADC_OS_CAL_DATA_LEN                               (16U)
#define GLB_GPADC_OS_CAL_DATA_MSK                               (((1U<<GLB_GPADC_OS_CAL_DATA_LEN)-1)<<GLB_GPADC_OS_CAL_DATA_POS)
#define GLB_GPADC_OS_CAL_DATA_UMSK                              (~(((1U<<GLB_GPADC_OS_CAL_DATA_LEN)-1)<<GLB_GPADC_OS_CAL_DATA_POS))

/* 0x608 : gpdac_ctrl */
#define GLB_GPDAC_CTRL_OFFSET                                   (0x608)
#define GLB_GPDACA_RSTN_ANA                                     GLB_GPDACA_RSTN_ANA
#define GLB_GPDACA_RSTN_ANA_POS                                 (0U)
#define GLB_GPDACA_RSTN_ANA_LEN                                 (1U)
#define GLB_GPDACA_RSTN_ANA_MSK                                 (((1U<<GLB_GPDACA_RSTN_ANA_LEN)-1)<<GLB_GPDACA_RSTN_ANA_POS)
#define GLB_GPDACA_RSTN_ANA_UMSK                                (~(((1U<<GLB_GPDACA_RSTN_ANA_LEN)-1)<<GLB_GPDACA_RSTN_ANA_POS))
#define GLB_GPDACB_RSTN_ANA                                     GLB_GPDACB_RSTN_ANA
#define GLB_GPDACB_RSTN_ANA_POS                                 (1U)
#define GLB_GPDACB_RSTN_ANA_LEN                                 (1U)
#define GLB_GPDACB_RSTN_ANA_MSK                                 (((1U<<GLB_GPDACB_RSTN_ANA_LEN)-1)<<GLB_GPDACB_RSTN_ANA_POS)
#define GLB_GPDACB_RSTN_ANA_UMSK                                (~(((1U<<GLB_GPDACB_RSTN_ANA_LEN)-1)<<GLB_GPDACB_RSTN_ANA_POS))
#define GLB_GPDAC_TEST_EN                                       GLB_GPDAC_TEST_EN
#define GLB_GPDAC_TEST_EN_POS                                   (7U)
#define GLB_GPDAC_TEST_EN_LEN                                   (1U)
#define GLB_GPDAC_TEST_EN_MSK                                   (((1U<<GLB_GPDAC_TEST_EN_LEN)-1)<<GLB_GPDAC_TEST_EN_POS)
#define GLB_GPDAC_TEST_EN_UMSK                                  (~(((1U<<GLB_GPDAC_TEST_EN_LEN)-1)<<GLB_GPDAC_TEST_EN_POS))
#define GLB_GPDAC_REF_SEL                                       GLB_GPDAC_REF_SEL
#define GLB_GPDAC_REF_SEL_POS                                   (8U)
#define GLB_GPDAC_REF_SEL_LEN                                   (1U)
#define GLB_GPDAC_REF_SEL_MSK                                   (((1U<<GLB_GPDAC_REF_SEL_LEN)-1)<<GLB_GPDAC_REF_SEL_POS)
#define GLB_GPDAC_REF_SEL_UMSK                                  (~(((1U<<GLB_GPDAC_REF_SEL_LEN)-1)<<GLB_GPDAC_REF_SEL_POS))
#define GLB_GPDAC_TEST_SEL                                      GLB_GPDAC_TEST_SEL
#define GLB_GPDAC_TEST_SEL_POS                                  (9U)
#define GLB_GPDAC_TEST_SEL_LEN                                  (3U)
#define GLB_GPDAC_TEST_SEL_MSK                                  (((1U<<GLB_GPDAC_TEST_SEL_LEN)-1)<<GLB_GPDAC_TEST_SEL_POS)
#define GLB_GPDAC_TEST_SEL_UMSK                                 (~(((1U<<GLB_GPDAC_TEST_SEL_LEN)-1)<<GLB_GPDAC_TEST_SEL_POS))
#define GLB_GPDAC_RESERVED                                      GLB_GPDAC_RESERVED
#define GLB_GPDAC_RESERVED_POS                                  (24U)
#define GLB_GPDAC_RESERVED_LEN                                  (8U)
#define GLB_GPDAC_RESERVED_MSK                                  (((1U<<GLB_GPDAC_RESERVED_LEN)-1)<<GLB_GPDAC_RESERVED_POS)
#define GLB_GPDAC_RESERVED_UMSK                                 (~(((1U<<GLB_GPDAC_RESERVED_LEN)-1)<<GLB_GPDAC_RESERVED_POS))

/* 0x60C : gpdac_actrl */
#define GLB_GPDAC_ACTRL_OFFSET                                  (0x60C)
#define GLB_GPDAC_A_EN                                          GLB_GPDAC_A_EN
#define GLB_GPDAC_A_EN_POS                                      (0U)
#define GLB_GPDAC_A_EN_LEN                                      (1U)
#define GLB_GPDAC_A_EN_MSK                                      (((1U<<GLB_GPDAC_A_EN_LEN)-1)<<GLB_GPDAC_A_EN_POS)
#define GLB_GPDAC_A_EN_UMSK                                     (~(((1U<<GLB_GPDAC_A_EN_LEN)-1)<<GLB_GPDAC_A_EN_POS))
#define GLB_GPDAC_IOA_EN                                        GLB_GPDAC_IOA_EN
#define GLB_GPDAC_IOA_EN_POS                                    (1U)
#define GLB_GPDAC_IOA_EN_LEN                                    (1U)
#define GLB_GPDAC_IOA_EN_MSK                                    (((1U<<GLB_GPDAC_IOA_EN_LEN)-1)<<GLB_GPDAC_IOA_EN_POS)
#define GLB_GPDAC_IOA_EN_UMSK                                   (~(((1U<<GLB_GPDAC_IOA_EN_LEN)-1)<<GLB_GPDAC_IOA_EN_POS))
#define GLB_GPDAC_A_RNG                                         GLB_GPDAC_A_RNG
#define GLB_GPDAC_A_RNG_POS                                     (18U)
#define GLB_GPDAC_A_RNG_LEN                                     (2U)
#define GLB_GPDAC_A_RNG_MSK                                     (((1U<<GLB_GPDAC_A_RNG_LEN)-1)<<GLB_GPDAC_A_RNG_POS)
#define GLB_GPDAC_A_RNG_UMSK                                    (~(((1U<<GLB_GPDAC_A_RNG_LEN)-1)<<GLB_GPDAC_A_RNG_POS))
#define GLB_GPDAC_A_OUTMUX                                      GLB_GPDAC_A_OUTMUX
#define GLB_GPDAC_A_OUTMUX_POS                                  (20U)
#define GLB_GPDAC_A_OUTMUX_LEN                                  (3U)
#define GLB_GPDAC_A_OUTMUX_MSK                                  (((1U<<GLB_GPDAC_A_OUTMUX_LEN)-1)<<GLB_GPDAC_A_OUTMUX_POS)
#define GLB_GPDAC_A_OUTMUX_UMSK                                 (~(((1U<<GLB_GPDAC_A_OUTMUX_LEN)-1)<<GLB_GPDAC_A_OUTMUX_POS))

/* 0x610 : gpdac_bctrl */
#define GLB_GPDAC_BCTRL_OFFSET                                  (0x610)
#define GLB_GPDAC_B_EN                                          GLB_GPDAC_B_EN
#define GLB_GPDAC_B_EN_POS                                      (0U)
#define GLB_GPDAC_B_EN_LEN                                      (1U)
#define GLB_GPDAC_B_EN_MSK                                      (((1U<<GLB_GPDAC_B_EN_LEN)-1)<<GLB_GPDAC_B_EN_POS)
#define GLB_GPDAC_B_EN_UMSK                                     (~(((1U<<GLB_GPDAC_B_EN_LEN)-1)<<GLB_GPDAC_B_EN_POS))
#define GLB_GPDAC_IOB_EN                                        GLB_GPDAC_IOB_EN
#define GLB_GPDAC_IOB_EN_POS                                    (1U)
#define GLB_GPDAC_IOB_EN_LEN                                    (1U)
#define GLB_GPDAC_IOB_EN_MSK                                    (((1U<<GLB_GPDAC_IOB_EN_LEN)-1)<<GLB_GPDAC_IOB_EN_POS)
#define GLB_GPDAC_IOB_EN_UMSK                                   (~(((1U<<GLB_GPDAC_IOB_EN_LEN)-1)<<GLB_GPDAC_IOB_EN_POS))
#define GLB_GPDAC_B_RNG                                         GLB_GPDAC_B_RNG
#define GLB_GPDAC_B_RNG_POS                                     (18U)
#define GLB_GPDAC_B_RNG_LEN                                     (2U)
#define GLB_GPDAC_B_RNG_MSK                                     (((1U<<GLB_GPDAC_B_RNG_LEN)-1)<<GLB_GPDAC_B_RNG_POS)
#define GLB_GPDAC_B_RNG_UMSK                                    (~(((1U<<GLB_GPDAC_B_RNG_LEN)-1)<<GLB_GPDAC_B_RNG_POS))
#define GLB_GPDAC_B_OUTMUX                                      GLB_GPDAC_B_OUTMUX
#define GLB_GPDAC_B_OUTMUX_POS                                  (20U)
#define GLB_GPDAC_B_OUTMUX_LEN                                  (3U)
#define GLB_GPDAC_B_OUTMUX_MSK                                  (((1U<<GLB_GPDAC_B_OUTMUX_LEN)-1)<<GLB_GPDAC_B_OUTMUX_POS)
#define GLB_GPDAC_B_OUTMUX_UMSK                                 (~(((1U<<GLB_GPDAC_B_OUTMUX_LEN)-1)<<GLB_GPDAC_B_OUTMUX_POS))

/* 0x614 : gpdac_data */
#define GLB_GPDAC_DATA_OFFSET                                   (0x614)
#define GLB_GPDAC_B_DATA                                        GLB_GPDAC_B_DATA
#define GLB_GPDAC_B_DATA_POS                                    (0U)
#define GLB_GPDAC_B_DATA_LEN                                    (10U)
#define GLB_GPDAC_B_DATA_MSK                                    (((1U<<GLB_GPDAC_B_DATA_LEN)-1)<<GLB_GPDAC_B_DATA_POS)
#define GLB_GPDAC_B_DATA_UMSK                                   (~(((1U<<GLB_GPDAC_B_DATA_LEN)-1)<<GLB_GPDAC_B_DATA_POS))
#define GLB_GPDAC_A_DATA                                        GLB_GPDAC_A_DATA
#define GLB_GPDAC_A_DATA_POS                                    (16U)
#define GLB_GPDAC_A_DATA_LEN                                    (10U)
#define GLB_GPDAC_A_DATA_MSK                                    (((1U<<GLB_GPDAC_A_DATA_LEN)-1)<<GLB_GPDAC_A_DATA_POS)
#define GLB_GPDAC_A_DATA_UMSK                                   (~(((1U<<GLB_GPDAC_A_DATA_LEN)-1)<<GLB_GPDAC_A_DATA_POS))

/* 0xF00 : tzc_glb_ctrl_0 */
#define GLB_TZC_GLB_CTRL_0_OFFSET                               (0xF00)
#define GLB_TZC_GLB_SWRST_S00_LOCK                              GLB_TZC_GLB_SWRST_S00_LOCK
#define GLB_TZC_GLB_SWRST_S00_LOCK_POS                          (0U)
#define GLB_TZC_GLB_SWRST_S00_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S00_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S00_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S00_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S00_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S00_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S00_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S01_LOCK                              GLB_TZC_GLB_SWRST_S01_LOCK
#define GLB_TZC_GLB_SWRST_S01_LOCK_POS                          (1U)
#define GLB_TZC_GLB_SWRST_S01_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S01_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S01_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S01_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S01_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S01_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S01_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S30_LOCK                              GLB_TZC_GLB_SWRST_S30_LOCK
#define GLB_TZC_GLB_SWRST_S30_LOCK_POS                          (8U)
#define GLB_TZC_GLB_SWRST_S30_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S30_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S30_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S30_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S30_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S30_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S30_LOCK_POS))
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK                         GLB_TZC_GLB_CTRL_PWRON_RST_LOCK
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS                     (12U)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS)
#define GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_PWRON_RST_LOCK_POS))
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK                         GLB_TZC_GLB_CTRL_CPU_RESET_LOCK
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS                     (13U)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS)
#define GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_CPU_RESET_LOCK_POS))
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK                         GLB_TZC_GLB_CTRL_SYS_RESET_LOCK
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS                     (14U)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN                     (1U)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_MSK                     (((1U<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS)
#define GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_UMSK                    (~(((1U<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_LEN)-1)<<GLB_TZC_GLB_CTRL_SYS_RESET_LOCK_POS))
#define GLB_TZC_GLB_MISC_LOCK                                   GLB_TZC_GLB_MISC_LOCK
#define GLB_TZC_GLB_MISC_LOCK_POS                               (25U)
#define GLB_TZC_GLB_MISC_LOCK_LEN                               (1U)
#define GLB_TZC_GLB_MISC_LOCK_MSK                               (((1U<<GLB_TZC_GLB_MISC_LOCK_LEN)-1)<<GLB_TZC_GLB_MISC_LOCK_POS)
#define GLB_TZC_GLB_MISC_LOCK_UMSK                              (~(((1U<<GLB_TZC_GLB_MISC_LOCK_LEN)-1)<<GLB_TZC_GLB_MISC_LOCK_POS))
#define GLB_TZC_GLB_SRAM_LOCK                                   GLB_TZC_GLB_SRAM_LOCK
#define GLB_TZC_GLB_SRAM_LOCK_POS                               (26U)
#define GLB_TZC_GLB_SRAM_LOCK_LEN                               (1U)
#define GLB_TZC_GLB_SRAM_LOCK_MSK                               (((1U<<GLB_TZC_GLB_SRAM_LOCK_LEN)-1)<<GLB_TZC_GLB_SRAM_LOCK_POS)
#define GLB_TZC_GLB_SRAM_LOCK_UMSK                              (~(((1U<<GLB_TZC_GLB_SRAM_LOCK_LEN)-1)<<GLB_TZC_GLB_SRAM_LOCK_POS))
#define GLB_TZC_GLB_BMX_LOCK                                    GLB_TZC_GLB_BMX_LOCK
#define GLB_TZC_GLB_BMX_LOCK_POS                                (28U)
#define GLB_TZC_GLB_BMX_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_BMX_LOCK_MSK                                (((1U<<GLB_TZC_GLB_BMX_LOCK_LEN)-1)<<GLB_TZC_GLB_BMX_LOCK_POS)
#define GLB_TZC_GLB_BMX_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_BMX_LOCK_LEN)-1)<<GLB_TZC_GLB_BMX_LOCK_POS))
#define GLB_TZC_GLB_DBG_LOCK                                    GLB_TZC_GLB_DBG_LOCK
#define GLB_TZC_GLB_DBG_LOCK_POS                                (29U)
#define GLB_TZC_GLB_DBG_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_DBG_LOCK_MSK                                (((1U<<GLB_TZC_GLB_DBG_LOCK_LEN)-1)<<GLB_TZC_GLB_DBG_LOCK_POS)
#define GLB_TZC_GLB_DBG_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_DBG_LOCK_LEN)-1)<<GLB_TZC_GLB_DBG_LOCK_POS))
#define GLB_TZC_GLB_MBIST_LOCK                                  GLB_TZC_GLB_MBIST_LOCK
#define GLB_TZC_GLB_MBIST_LOCK_POS                              (30U)
#define GLB_TZC_GLB_MBIST_LOCK_LEN                              (1U)
#define GLB_TZC_GLB_MBIST_LOCK_MSK                              (((1U<<GLB_TZC_GLB_MBIST_LOCK_LEN)-1)<<GLB_TZC_GLB_MBIST_LOCK_POS)
#define GLB_TZC_GLB_MBIST_LOCK_UMSK                             (~(((1U<<GLB_TZC_GLB_MBIST_LOCK_LEN)-1)<<GLB_TZC_GLB_MBIST_LOCK_POS))
#define GLB_TZC_GLB_CLK_LOCK                                    GLB_TZC_GLB_CLK_LOCK
#define GLB_TZC_GLB_CLK_LOCK_POS                                (31U)
#define GLB_TZC_GLB_CLK_LOCK_LEN                                (1U)
#define GLB_TZC_GLB_CLK_LOCK_MSK                                (((1U<<GLB_TZC_GLB_CLK_LOCK_LEN)-1)<<GLB_TZC_GLB_CLK_LOCK_POS)
#define GLB_TZC_GLB_CLK_LOCK_UMSK                               (~(((1U<<GLB_TZC_GLB_CLK_LOCK_LEN)-1)<<GLB_TZC_GLB_CLK_LOCK_POS))

/* 0xF04 : tzc_glb_ctrl_1 */
#define GLB_TZC_GLB_CTRL_1_OFFSET                               (0xF04)
#define GLB_TZC_GLB_SWRST_S20_LOCK                              GLB_TZC_GLB_SWRST_S20_LOCK
#define GLB_TZC_GLB_SWRST_S20_LOCK_POS                          (0U)
#define GLB_TZC_GLB_SWRST_S20_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S20_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S20_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S20_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S20_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S20_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S20_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S21_LOCK                              GLB_TZC_GLB_SWRST_S21_LOCK
#define GLB_TZC_GLB_SWRST_S21_LOCK_POS                          (1U)
#define GLB_TZC_GLB_SWRST_S21_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S21_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S21_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S21_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S21_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S21_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S21_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S22_LOCK                              GLB_TZC_GLB_SWRST_S22_LOCK
#define GLB_TZC_GLB_SWRST_S22_LOCK_POS                          (2U)
#define GLB_TZC_GLB_SWRST_S22_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S22_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S22_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S22_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S22_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S22_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S22_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S23_LOCK                              GLB_TZC_GLB_SWRST_S23_LOCK
#define GLB_TZC_GLB_SWRST_S23_LOCK_POS                          (3U)
#define GLB_TZC_GLB_SWRST_S23_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S23_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S23_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S23_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S23_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S23_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S23_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S24_LOCK                              GLB_TZC_GLB_SWRST_S24_LOCK
#define GLB_TZC_GLB_SWRST_S24_LOCK_POS                          (4U)
#define GLB_TZC_GLB_SWRST_S24_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S24_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S24_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S24_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S24_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S24_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S24_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S25_LOCK                              GLB_TZC_GLB_SWRST_S25_LOCK
#define GLB_TZC_GLB_SWRST_S25_LOCK_POS                          (5U)
#define GLB_TZC_GLB_SWRST_S25_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S25_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S25_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S25_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S25_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S25_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S25_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S26_LOCK                              GLB_TZC_GLB_SWRST_S26_LOCK
#define GLB_TZC_GLB_SWRST_S26_LOCK_POS                          (6U)
#define GLB_TZC_GLB_SWRST_S26_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S26_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S26_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S26_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S26_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S26_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S26_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S27_LOCK                              GLB_TZC_GLB_SWRST_S27_LOCK
#define GLB_TZC_GLB_SWRST_S27_LOCK_POS                          (7U)
#define GLB_TZC_GLB_SWRST_S27_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S27_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S27_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S27_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S27_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S27_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S27_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S28_LOCK                              GLB_TZC_GLB_SWRST_S28_LOCK
#define GLB_TZC_GLB_SWRST_S28_LOCK_POS                          (8U)
#define GLB_TZC_GLB_SWRST_S28_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S28_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S28_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S28_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S28_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S28_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S28_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S29_LOCK                              GLB_TZC_GLB_SWRST_S29_LOCK
#define GLB_TZC_GLB_SWRST_S29_LOCK_POS                          (9U)
#define GLB_TZC_GLB_SWRST_S29_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S29_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S29_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S29_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S29_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S29_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S29_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2A_LOCK                              GLB_TZC_GLB_SWRST_S2A_LOCK
#define GLB_TZC_GLB_SWRST_S2A_LOCK_POS                          (10U)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2A_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2A_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2A_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2B_LOCK                              GLB_TZC_GLB_SWRST_S2B_LOCK
#define GLB_TZC_GLB_SWRST_S2B_LOCK_POS                          (11U)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2B_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2B_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2B_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2C_LOCK                              GLB_TZC_GLB_SWRST_S2C_LOCK
#define GLB_TZC_GLB_SWRST_S2C_LOCK_POS                          (12U)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2C_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2C_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2C_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2D_LOCK                              GLB_TZC_GLB_SWRST_S2D_LOCK
#define GLB_TZC_GLB_SWRST_S2D_LOCK_POS                          (13U)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2D_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2D_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2D_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2E_LOCK                              GLB_TZC_GLB_SWRST_S2E_LOCK
#define GLB_TZC_GLB_SWRST_S2E_LOCK_POS                          (14U)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2E_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2E_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2E_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S2F_LOCK                              GLB_TZC_GLB_SWRST_S2F_LOCK
#define GLB_TZC_GLB_SWRST_S2F_LOCK_POS                          (15U)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S2F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2F_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S2F_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S2F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S2F_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S10_LOCK                              GLB_TZC_GLB_SWRST_S10_LOCK
#define GLB_TZC_GLB_SWRST_S10_LOCK_POS                          (16U)
#define GLB_TZC_GLB_SWRST_S10_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S10_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S10_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S10_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S10_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S10_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S10_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S11_LOCK                              GLB_TZC_GLB_SWRST_S11_LOCK
#define GLB_TZC_GLB_SWRST_S11_LOCK_POS                          (17U)
#define GLB_TZC_GLB_SWRST_S11_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S11_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S11_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S11_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S11_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S11_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S11_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S12_LOCK                              GLB_TZC_GLB_SWRST_S12_LOCK
#define GLB_TZC_GLB_SWRST_S12_LOCK_POS                          (18U)
#define GLB_TZC_GLB_SWRST_S12_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S12_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S12_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S12_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S12_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S12_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S12_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S13_LOCK                              GLB_TZC_GLB_SWRST_S13_LOCK
#define GLB_TZC_GLB_SWRST_S13_LOCK_POS                          (19U)
#define GLB_TZC_GLB_SWRST_S13_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S13_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S13_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S13_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S13_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S13_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S13_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S14_LOCK                              GLB_TZC_GLB_SWRST_S14_LOCK
#define GLB_TZC_GLB_SWRST_S14_LOCK_POS                          (20U)
#define GLB_TZC_GLB_SWRST_S14_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S14_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S14_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S14_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S14_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S14_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S14_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S15_LOCK                              GLB_TZC_GLB_SWRST_S15_LOCK
#define GLB_TZC_GLB_SWRST_S15_LOCK_POS                          (21U)
#define GLB_TZC_GLB_SWRST_S15_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S15_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S15_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S15_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S15_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S15_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S15_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S16_LOCK                              GLB_TZC_GLB_SWRST_S16_LOCK
#define GLB_TZC_GLB_SWRST_S16_LOCK_POS                          (22U)
#define GLB_TZC_GLB_SWRST_S16_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S16_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S16_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S16_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S16_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S16_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S16_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S17_LOCK                              GLB_TZC_GLB_SWRST_S17_LOCK
#define GLB_TZC_GLB_SWRST_S17_LOCK_POS                          (23U)
#define GLB_TZC_GLB_SWRST_S17_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S17_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S17_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S17_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S17_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S17_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S17_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S18_LOCK                              GLB_TZC_GLB_SWRST_S18_LOCK
#define GLB_TZC_GLB_SWRST_S18_LOCK_POS                          (24U)
#define GLB_TZC_GLB_SWRST_S18_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S18_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S18_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S18_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S18_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S18_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S18_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S19_LOCK                              GLB_TZC_GLB_SWRST_S19_LOCK
#define GLB_TZC_GLB_SWRST_S19_LOCK_POS                          (25U)
#define GLB_TZC_GLB_SWRST_S19_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S19_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S19_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S19_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S19_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S19_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S19_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1A_LOCK                              GLB_TZC_GLB_SWRST_S1A_LOCK
#define GLB_TZC_GLB_SWRST_S1A_LOCK_POS                          (26U)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1A_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1A_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1A_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1A_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1B_LOCK                              GLB_TZC_GLB_SWRST_S1B_LOCK
#define GLB_TZC_GLB_SWRST_S1B_LOCK_POS                          (27U)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1B_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1B_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1B_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1B_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1C_LOCK                              GLB_TZC_GLB_SWRST_S1C_LOCK
#define GLB_TZC_GLB_SWRST_S1C_LOCK_POS                          (28U)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1C_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1C_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1C_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1C_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1D_LOCK                              GLB_TZC_GLB_SWRST_S1D_LOCK
#define GLB_TZC_GLB_SWRST_S1D_LOCK_POS                          (29U)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1D_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1D_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1D_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1D_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1E_LOCK                              GLB_TZC_GLB_SWRST_S1E_LOCK
#define GLB_TZC_GLB_SWRST_S1E_LOCK_POS                          (30U)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1E_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1E_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1E_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1E_LOCK_POS))
#define GLB_TZC_GLB_SWRST_S1F_LOCK                              GLB_TZC_GLB_SWRST_S1F_LOCK
#define GLB_TZC_GLB_SWRST_S1F_LOCK_POS                          (31U)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_LEN                          (1U)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_MSK                          (((1U<<GLB_TZC_GLB_SWRST_S1F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1F_LOCK_POS)
#define GLB_TZC_GLB_SWRST_S1F_LOCK_UMSK                         (~(((1U<<GLB_TZC_GLB_SWRST_S1F_LOCK_LEN)-1)<<GLB_TZC_GLB_SWRST_S1F_LOCK_POS))

/* 0xF08 : tzc_glb_ctrl_2 */
#define GLB_TZC_GLB_CTRL_2_OFFSET                               (0xF08)
#define GLB_TZC_GLB_GPIO_0_LOCK                                 GLB_TZC_GLB_GPIO_0_LOCK
#define GLB_TZC_GLB_GPIO_0_LOCK_POS                             (0U)
#define GLB_TZC_GLB_GPIO_0_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_0_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_0_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_0_LOCK_POS)
#define GLB_TZC_GLB_GPIO_0_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_0_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_0_LOCK_POS))
#define GLB_TZC_GLB_GPIO_1_LOCK                                 GLB_TZC_GLB_GPIO_1_LOCK
#define GLB_TZC_GLB_GPIO_1_LOCK_POS                             (1U)
#define GLB_TZC_GLB_GPIO_1_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_1_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_1_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_1_LOCK_POS)
#define GLB_TZC_GLB_GPIO_1_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_1_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_1_LOCK_POS))
#define GLB_TZC_GLB_GPIO_2_LOCK                                 GLB_TZC_GLB_GPIO_2_LOCK
#define GLB_TZC_GLB_GPIO_2_LOCK_POS                             (2U)
#define GLB_TZC_GLB_GPIO_2_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_2_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_2_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_2_LOCK_POS)
#define GLB_TZC_GLB_GPIO_2_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_2_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_2_LOCK_POS))
#define GLB_TZC_GLB_GPIO_3_LOCK                                 GLB_TZC_GLB_GPIO_3_LOCK
#define GLB_TZC_GLB_GPIO_3_LOCK_POS                             (3U)
#define GLB_TZC_GLB_GPIO_3_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_3_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_3_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_3_LOCK_POS)
#define GLB_TZC_GLB_GPIO_3_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_3_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_3_LOCK_POS))
#define GLB_TZC_GLB_GPIO_4_LOCK                                 GLB_TZC_GLB_GPIO_4_LOCK
#define GLB_TZC_GLB_GPIO_4_LOCK_POS                             (4U)
#define GLB_TZC_GLB_GPIO_4_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_4_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_4_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_4_LOCK_POS)
#define GLB_TZC_GLB_GPIO_4_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_4_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_4_LOCK_POS))
#define GLB_TZC_GLB_GPIO_5_LOCK                                 GLB_TZC_GLB_GPIO_5_LOCK
#define GLB_TZC_GLB_GPIO_5_LOCK_POS                             (5U)
#define GLB_TZC_GLB_GPIO_5_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_5_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_5_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_5_LOCK_POS)
#define GLB_TZC_GLB_GPIO_5_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_5_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_5_LOCK_POS))
#define GLB_TZC_GLB_GPIO_6_LOCK                                 GLB_TZC_GLB_GPIO_6_LOCK
#define GLB_TZC_GLB_GPIO_6_LOCK_POS                             (6U)
#define GLB_TZC_GLB_GPIO_6_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_6_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_6_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_6_LOCK_POS)
#define GLB_TZC_GLB_GPIO_6_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_6_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_6_LOCK_POS))
#define GLB_TZC_GLB_GPIO_7_LOCK                                 GLB_TZC_GLB_GPIO_7_LOCK
#define GLB_TZC_GLB_GPIO_7_LOCK_POS                             (7U)
#define GLB_TZC_GLB_GPIO_7_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_7_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_7_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_7_LOCK_POS)
#define GLB_TZC_GLB_GPIO_7_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_7_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_7_LOCK_POS))
#define GLB_TZC_GLB_GPIO_8_LOCK                                 GLB_TZC_GLB_GPIO_8_LOCK
#define GLB_TZC_GLB_GPIO_8_LOCK_POS                             (8U)
#define GLB_TZC_GLB_GPIO_8_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_8_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_8_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_8_LOCK_POS)
#define GLB_TZC_GLB_GPIO_8_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_8_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_8_LOCK_POS))
#define GLB_TZC_GLB_GPIO_9_LOCK                                 GLB_TZC_GLB_GPIO_9_LOCK
#define GLB_TZC_GLB_GPIO_9_LOCK_POS                             (9U)
#define GLB_TZC_GLB_GPIO_9_LOCK_LEN                             (1U)
#define GLB_TZC_GLB_GPIO_9_LOCK_MSK                             (((1U<<GLB_TZC_GLB_GPIO_9_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_9_LOCK_POS)
#define GLB_TZC_GLB_GPIO_9_LOCK_UMSK                            (~(((1U<<GLB_TZC_GLB_GPIO_9_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_9_LOCK_POS))
#define GLB_TZC_GLB_GPIO_10_LOCK                                GLB_TZC_GLB_GPIO_10_LOCK
#define GLB_TZC_GLB_GPIO_10_LOCK_POS                            (10U)
#define GLB_TZC_GLB_GPIO_10_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_10_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_10_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_10_LOCK_POS)
#define GLB_TZC_GLB_GPIO_10_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_10_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_10_LOCK_POS))
#define GLB_TZC_GLB_GPIO_11_LOCK                                GLB_TZC_GLB_GPIO_11_LOCK
#define GLB_TZC_GLB_GPIO_11_LOCK_POS                            (11U)
#define GLB_TZC_GLB_GPIO_11_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_11_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_11_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_11_LOCK_POS)
#define GLB_TZC_GLB_GPIO_11_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_11_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_11_LOCK_POS))
#define GLB_TZC_GLB_GPIO_12_LOCK                                GLB_TZC_GLB_GPIO_12_LOCK
#define GLB_TZC_GLB_GPIO_12_LOCK_POS                            (12U)
#define GLB_TZC_GLB_GPIO_12_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_12_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_12_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_12_LOCK_POS)
#define GLB_TZC_GLB_GPIO_12_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_12_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_12_LOCK_POS))
#define GLB_TZC_GLB_GPIO_13_LOCK                                GLB_TZC_GLB_GPIO_13_LOCK
#define GLB_TZC_GLB_GPIO_13_LOCK_POS                            (13U)
#define GLB_TZC_GLB_GPIO_13_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_13_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_13_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_13_LOCK_POS)
#define GLB_TZC_GLB_GPIO_13_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_13_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_13_LOCK_POS))
#define GLB_TZC_GLB_GPIO_14_LOCK                                GLB_TZC_GLB_GPIO_14_LOCK
#define GLB_TZC_GLB_GPIO_14_LOCK_POS                            (14U)
#define GLB_TZC_GLB_GPIO_14_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_14_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_14_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_14_LOCK_POS)
#define GLB_TZC_GLB_GPIO_14_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_14_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_14_LOCK_POS))
#define GLB_TZC_GLB_GPIO_15_LOCK                                GLB_TZC_GLB_GPIO_15_LOCK
#define GLB_TZC_GLB_GPIO_15_LOCK_POS                            (15U)
#define GLB_TZC_GLB_GPIO_15_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_15_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_15_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_15_LOCK_POS)
#define GLB_TZC_GLB_GPIO_15_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_15_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_15_LOCK_POS))
#define GLB_TZC_GLB_GPIO_16_LOCK                                GLB_TZC_GLB_GPIO_16_LOCK
#define GLB_TZC_GLB_GPIO_16_LOCK_POS                            (16U)
#define GLB_TZC_GLB_GPIO_16_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_16_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_16_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_16_LOCK_POS)
#define GLB_TZC_GLB_GPIO_16_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_16_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_16_LOCK_POS))
#define GLB_TZC_GLB_GPIO_17_LOCK                                GLB_TZC_GLB_GPIO_17_LOCK
#define GLB_TZC_GLB_GPIO_17_LOCK_POS                            (17U)
#define GLB_TZC_GLB_GPIO_17_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_17_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_17_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_17_LOCK_POS)
#define GLB_TZC_GLB_GPIO_17_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_17_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_17_LOCK_POS))
#define GLB_TZC_GLB_GPIO_18_LOCK                                GLB_TZC_GLB_GPIO_18_LOCK
#define GLB_TZC_GLB_GPIO_18_LOCK_POS                            (18U)
#define GLB_TZC_GLB_GPIO_18_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_18_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_18_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_18_LOCK_POS)
#define GLB_TZC_GLB_GPIO_18_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_18_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_18_LOCK_POS))
#define GLB_TZC_GLB_GPIO_19_LOCK                                GLB_TZC_GLB_GPIO_19_LOCK
#define GLB_TZC_GLB_GPIO_19_LOCK_POS                            (19U)
#define GLB_TZC_GLB_GPIO_19_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_19_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_19_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_19_LOCK_POS)
#define GLB_TZC_GLB_GPIO_19_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_19_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_19_LOCK_POS))
#define GLB_TZC_GLB_GPIO_20_LOCK                                GLB_TZC_GLB_GPIO_20_LOCK
#define GLB_TZC_GLB_GPIO_20_LOCK_POS                            (20U)
#define GLB_TZC_GLB_GPIO_20_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_20_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_20_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_20_LOCK_POS)
#define GLB_TZC_GLB_GPIO_20_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_20_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_20_LOCK_POS))
#define GLB_TZC_GLB_GPIO_21_LOCK                                GLB_TZC_GLB_GPIO_21_LOCK
#define GLB_TZC_GLB_GPIO_21_LOCK_POS                            (21U)
#define GLB_TZC_GLB_GPIO_21_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_21_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_21_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_21_LOCK_POS)
#define GLB_TZC_GLB_GPIO_21_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_21_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_21_LOCK_POS))
#define GLB_TZC_GLB_GPIO_22_LOCK                                GLB_TZC_GLB_GPIO_22_LOCK
#define GLB_TZC_GLB_GPIO_22_LOCK_POS                            (22U)
#define GLB_TZC_GLB_GPIO_22_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_22_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_22_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_22_LOCK_POS)
#define GLB_TZC_GLB_GPIO_22_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_22_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_22_LOCK_POS))
#define GLB_TZC_GLB_GPIO_23_LOCK                                GLB_TZC_GLB_GPIO_23_LOCK
#define GLB_TZC_GLB_GPIO_23_LOCK_POS                            (23U)
#define GLB_TZC_GLB_GPIO_23_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_23_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_23_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_23_LOCK_POS)
#define GLB_TZC_GLB_GPIO_23_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_23_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_23_LOCK_POS))
#define GLB_TZC_GLB_GPIO_24_LOCK                                GLB_TZC_GLB_GPIO_24_LOCK
#define GLB_TZC_GLB_GPIO_24_LOCK_POS                            (24U)
#define GLB_TZC_GLB_GPIO_24_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_24_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_24_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_24_LOCK_POS)
#define GLB_TZC_GLB_GPIO_24_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_24_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_24_LOCK_POS))
#define GLB_TZC_GLB_GPIO_25_LOCK                                GLB_TZC_GLB_GPIO_25_LOCK
#define GLB_TZC_GLB_GPIO_25_LOCK_POS                            (25U)
#define GLB_TZC_GLB_GPIO_25_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_25_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_25_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_25_LOCK_POS)
#define GLB_TZC_GLB_GPIO_25_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_25_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_25_LOCK_POS))
#define GLB_TZC_GLB_GPIO_26_LOCK                                GLB_TZC_GLB_GPIO_26_LOCK
#define GLB_TZC_GLB_GPIO_26_LOCK_POS                            (26U)
#define GLB_TZC_GLB_GPIO_26_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_26_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_26_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_26_LOCK_POS)
#define GLB_TZC_GLB_GPIO_26_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_26_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_26_LOCK_POS))
#define GLB_TZC_GLB_GPIO_27_LOCK                                GLB_TZC_GLB_GPIO_27_LOCK
#define GLB_TZC_GLB_GPIO_27_LOCK_POS                            (27U)
#define GLB_TZC_GLB_GPIO_27_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_27_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_27_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_27_LOCK_POS)
#define GLB_TZC_GLB_GPIO_27_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_27_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_27_LOCK_POS))
#define GLB_TZC_GLB_GPIO_28_LOCK                                GLB_TZC_GLB_GPIO_28_LOCK
#define GLB_TZC_GLB_GPIO_28_LOCK_POS                            (28U)
#define GLB_TZC_GLB_GPIO_28_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_28_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_28_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_28_LOCK_POS)
#define GLB_TZC_GLB_GPIO_28_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_28_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_28_LOCK_POS))
#define GLB_TZC_GLB_GPIO_29_LOCK                                GLB_TZC_GLB_GPIO_29_LOCK
#define GLB_TZC_GLB_GPIO_29_LOCK_POS                            (29U)
#define GLB_TZC_GLB_GPIO_29_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_29_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_29_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_29_LOCK_POS)
#define GLB_TZC_GLB_GPIO_29_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_29_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_29_LOCK_POS))
#define GLB_TZC_GLB_GPIO_30_LOCK                                GLB_TZC_GLB_GPIO_30_LOCK
#define GLB_TZC_GLB_GPIO_30_LOCK_POS                            (30U)
#define GLB_TZC_GLB_GPIO_30_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_30_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_30_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_30_LOCK_POS)
#define GLB_TZC_GLB_GPIO_30_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_30_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_30_LOCK_POS))
#define GLB_TZC_GLB_GPIO_31_LOCK                                GLB_TZC_GLB_GPIO_31_LOCK
#define GLB_TZC_GLB_GPIO_31_LOCK_POS                            (31U)
#define GLB_TZC_GLB_GPIO_31_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_31_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_31_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_31_LOCK_POS)
#define GLB_TZC_GLB_GPIO_31_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_31_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_31_LOCK_POS))

/* 0xF0C : tzc_glb_ctrl_3 */
#define GLB_TZC_GLB_CTRL_3_OFFSET                               (0xF0C)
#define GLB_TZC_GLB_GPIO_32_LOCK                                GLB_TZC_GLB_GPIO_32_LOCK
#define GLB_TZC_GLB_GPIO_32_LOCK_POS                            (0U)
#define GLB_TZC_GLB_GPIO_32_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_32_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_32_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_32_LOCK_POS)
#define GLB_TZC_GLB_GPIO_32_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_32_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_32_LOCK_POS))
#define GLB_TZC_GLB_GPIO_33_LOCK                                GLB_TZC_GLB_GPIO_33_LOCK
#define GLB_TZC_GLB_GPIO_33_LOCK_POS                            (1U)
#define GLB_TZC_GLB_GPIO_33_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_33_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_33_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_33_LOCK_POS)
#define GLB_TZC_GLB_GPIO_33_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_33_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_33_LOCK_POS))
#define GLB_TZC_GLB_GPIO_34_LOCK                                GLB_TZC_GLB_GPIO_34_LOCK
#define GLB_TZC_GLB_GPIO_34_LOCK_POS                            (2U)
#define GLB_TZC_GLB_GPIO_34_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_34_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_34_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_34_LOCK_POS)
#define GLB_TZC_GLB_GPIO_34_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_34_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_34_LOCK_POS))
#define GLB_TZC_GLB_GPIO_35_LOCK                                GLB_TZC_GLB_GPIO_35_LOCK
#define GLB_TZC_GLB_GPIO_35_LOCK_POS                            (3U)
#define GLB_TZC_GLB_GPIO_35_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_35_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_35_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_35_LOCK_POS)
#define GLB_TZC_GLB_GPIO_35_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_35_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_35_LOCK_POS))
#define GLB_TZC_GLB_GPIO_36_LOCK                                GLB_TZC_GLB_GPIO_36_LOCK
#define GLB_TZC_GLB_GPIO_36_LOCK_POS                            (4U)
#define GLB_TZC_GLB_GPIO_36_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_36_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_36_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_36_LOCK_POS)
#define GLB_TZC_GLB_GPIO_36_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_36_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_36_LOCK_POS))
#define GLB_TZC_GLB_GPIO_37_LOCK                                GLB_TZC_GLB_GPIO_37_LOCK
#define GLB_TZC_GLB_GPIO_37_LOCK_POS                            (5U)
#define GLB_TZC_GLB_GPIO_37_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_37_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_37_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_37_LOCK_POS)
#define GLB_TZC_GLB_GPIO_37_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_37_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_37_LOCK_POS))
#define GLB_TZC_GLB_GPIO_38_LOCK                                GLB_TZC_GLB_GPIO_38_LOCK
#define GLB_TZC_GLB_GPIO_38_LOCK_POS                            (6U)
#define GLB_TZC_GLB_GPIO_38_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_38_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_38_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_38_LOCK_POS)
#define GLB_TZC_GLB_GPIO_38_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_38_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_38_LOCK_POS))
#define GLB_TZC_GLB_GPIO_39_LOCK                                GLB_TZC_GLB_GPIO_39_LOCK
#define GLB_TZC_GLB_GPIO_39_LOCK_POS                            (7U)
#define GLB_TZC_GLB_GPIO_39_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_39_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_39_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_39_LOCK_POS)
#define GLB_TZC_GLB_GPIO_39_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_39_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_39_LOCK_POS))
#define GLB_TZC_GLB_GPIO_40_LOCK                                GLB_TZC_GLB_GPIO_40_LOCK
#define GLB_TZC_GLB_GPIO_40_LOCK_POS                            (8U)
#define GLB_TZC_GLB_GPIO_40_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_40_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_40_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_40_LOCK_POS)
#define GLB_TZC_GLB_GPIO_40_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_40_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_40_LOCK_POS))
#define GLB_TZC_GLB_GPIO_41_LOCK                                GLB_TZC_GLB_GPIO_41_LOCK
#define GLB_TZC_GLB_GPIO_41_LOCK_POS                            (9U)
#define GLB_TZC_GLB_GPIO_41_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_41_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_41_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_41_LOCK_POS)
#define GLB_TZC_GLB_GPIO_41_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_41_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_41_LOCK_POS))
#define GLB_TZC_GLB_GPIO_42_LOCK                                GLB_TZC_GLB_GPIO_42_LOCK
#define GLB_TZC_GLB_GPIO_42_LOCK_POS                            (10U)
#define GLB_TZC_GLB_GPIO_42_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_42_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_42_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_42_LOCK_POS)
#define GLB_TZC_GLB_GPIO_42_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_42_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_42_LOCK_POS))
#define GLB_TZC_GLB_GPIO_43_LOCK                                GLB_TZC_GLB_GPIO_43_LOCK
#define GLB_TZC_GLB_GPIO_43_LOCK_POS                            (11U)
#define GLB_TZC_GLB_GPIO_43_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_43_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_43_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_43_LOCK_POS)
#define GLB_TZC_GLB_GPIO_43_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_43_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_43_LOCK_POS))
#define GLB_TZC_GLB_GPIO_44_LOCK                                GLB_TZC_GLB_GPIO_44_LOCK
#define GLB_TZC_GLB_GPIO_44_LOCK_POS                            (12U)
#define GLB_TZC_GLB_GPIO_44_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_44_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_44_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_44_LOCK_POS)
#define GLB_TZC_GLB_GPIO_44_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_44_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_44_LOCK_POS))
#define GLB_TZC_GLB_GPIO_45_LOCK                                GLB_TZC_GLB_GPIO_45_LOCK
#define GLB_TZC_GLB_GPIO_45_LOCK_POS                            (13U)
#define GLB_TZC_GLB_GPIO_45_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_45_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_45_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_45_LOCK_POS)
#define GLB_TZC_GLB_GPIO_45_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_45_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_45_LOCK_POS))
#define GLB_TZC_GLB_GPIO_46_LOCK                                GLB_TZC_GLB_GPIO_46_LOCK
#define GLB_TZC_GLB_GPIO_46_LOCK_POS                            (14U)
#define GLB_TZC_GLB_GPIO_46_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_46_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_46_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_46_LOCK_POS)
#define GLB_TZC_GLB_GPIO_46_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_46_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_46_LOCK_POS))
#define GLB_TZC_GLB_GPIO_47_LOCK                                GLB_TZC_GLB_GPIO_47_LOCK
#define GLB_TZC_GLB_GPIO_47_LOCK_POS                            (15U)
#define GLB_TZC_GLB_GPIO_47_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_47_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_47_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_47_LOCK_POS)
#define GLB_TZC_GLB_GPIO_47_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_47_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_47_LOCK_POS))
#define GLB_TZC_GLB_GPIO_48_LOCK                                GLB_TZC_GLB_GPIO_48_LOCK
#define GLB_TZC_GLB_GPIO_48_LOCK_POS                            (16U)
#define GLB_TZC_GLB_GPIO_48_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_48_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_48_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_48_LOCK_POS)
#define GLB_TZC_GLB_GPIO_48_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_48_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_48_LOCK_POS))
#define GLB_TZC_GLB_GPIO_49_LOCK                                GLB_TZC_GLB_GPIO_49_LOCK
#define GLB_TZC_GLB_GPIO_49_LOCK_POS                            (17U)
#define GLB_TZC_GLB_GPIO_49_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_49_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_49_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_49_LOCK_POS)
#define GLB_TZC_GLB_GPIO_49_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_49_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_49_LOCK_POS))
#define GLB_TZC_GLB_GPIO_50_LOCK                                GLB_TZC_GLB_GPIO_50_LOCK
#define GLB_TZC_GLB_GPIO_50_LOCK_POS                            (18U)
#define GLB_TZC_GLB_GPIO_50_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_50_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_50_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_50_LOCK_POS)
#define GLB_TZC_GLB_GPIO_50_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_50_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_50_LOCK_POS))
#define GLB_TZC_GLB_GPIO_51_LOCK                                GLB_TZC_GLB_GPIO_51_LOCK
#define GLB_TZC_GLB_GPIO_51_LOCK_POS                            (19U)
#define GLB_TZC_GLB_GPIO_51_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_51_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_51_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_51_LOCK_POS)
#define GLB_TZC_GLB_GPIO_51_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_51_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_51_LOCK_POS))
#define GLB_TZC_GLB_GPIO_52_LOCK                                GLB_TZC_GLB_GPIO_52_LOCK
#define GLB_TZC_GLB_GPIO_52_LOCK_POS                            (20U)
#define GLB_TZC_GLB_GPIO_52_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_52_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_52_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_52_LOCK_POS)
#define GLB_TZC_GLB_GPIO_52_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_52_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_52_LOCK_POS))
#define GLB_TZC_GLB_GPIO_53_LOCK                                GLB_TZC_GLB_GPIO_53_LOCK
#define GLB_TZC_GLB_GPIO_53_LOCK_POS                            (21U)
#define GLB_TZC_GLB_GPIO_53_LOCK_LEN                            (1U)
#define GLB_TZC_GLB_GPIO_53_LOCK_MSK                            (((1U<<GLB_TZC_GLB_GPIO_53_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_53_LOCK_POS)
#define GLB_TZC_GLB_GPIO_53_LOCK_UMSK                           (~(((1U<<GLB_TZC_GLB_GPIO_53_LOCK_LEN)-1)<<GLB_TZC_GLB_GPIO_53_LOCK_POS))


struct  glb_reg {
    /* 0x0 : clk_cfg0 */
    union {
        struct {
            uint32_t reg_hclk_en                    :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_bclk_en                    :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t reg_pll_sel                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t hbn_root_clk_sel               :  2; /* [ 7: 6],          r,        0x0 */
            uint32_t reg_bclk_div_act_pulse         :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_bclk_div_bypass            :  1; /* [    9],        r/w,        0x0 */
            uint32_t sts_bclk_prot_done             :  1; /* [   10],          r,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t reg_bclk_sw_done_cnt           :  4; /* [15:12],        r/w,        0x5 */
            uint32_t reserved_16_23                 :  8; /* [23:16],       rsvd,        0x0 */
            uint32_t fclk_sw_state                  :  3; /* [26:24],          r,        0x0 */
            uint32_t chip_rdy                       :  1; /* [   27],          r,        0x0 */
            uint32_t glb_id                         :  4; /* [31:28],          r,        0x7 */
        }BF;
        uint32_t WORD;
    } clk_cfg0;

    /* 0x4 : clk_cfg1 */
    union {
        struct {
            uint32_t reg_pll_en                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_fclk_en                    :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_hclk_dis                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_bclk_dis                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t reg_hclk_div                   :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_bclk_div                   :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg1;

    /* 0x8 : clk_cfg2 */
    union {
        struct {
            uint32_t clk_1m_clk_div                 :  5; /* [ 4: 0],        r/w,       0x1f */
            uint32_t clk_1m_en                      :  1; /* [    5],        r/w,        0x1 */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t clk_1m_clk_sel                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_audio_clk_div              :  6; /* [13: 8],        r/w,        0x0 */
            uint32_t reserved_14                    :  1; /* [   14],       rsvd,        0x0 */
            uint32_t reg_i2s_0_ref_clk_oe           :  1; /* [   15],        r/w,        0x0 */
            uint32_t ble_clk_sel                    :  6; /* [21:16],        r/w,       0x10 */
            uint32_t usb_clk_en                     :  1; /* [   22],        r/w,        0x0 */
            uint32_t pll_48m_div_en                 :  1; /* [   23],        r/w,        0x0 */
            uint32_t ble_en                         :  1; /* [   24],        r/w,        0x1 */
            uint32_t m154_zbEn                      :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t reg_audio_clk_src_sel          :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reg_audio_clk_div_en           :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_audio_auto_div_en          :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg2;

    /* 0xC : clk_cfg3 */
    union {
        struct {
            uint32_t uart_clk_div                   :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t uart_clk_en                    :  1; /* [    4],        r/w,        0x1 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t hbn_uart_clk_sel               :  1; /* [    7],          r,        0x0 */
            uint32_t sf_clk_div                     :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t sf_clk_en                      :  1; /* [   11],        r/w,        0x1 */
            uint32_t sf_clk_sel                     :  2; /* [13:12],        r/w,        0x2 */
            uint32_t cfg_inv_rf_test_clk_o          :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t ir_clk_div                     :  6; /* [21:16],        r/w,        0xf */
            uint32_t hbn_uart_clk_sel2              :  1; /* [   22],          r,        0x0 */
            uint32_t ir_clk_en                      :  1; /* [   23],        r/w,        0x1 */
            uint32_t dma_clk_en                     :  8; /* [31:24],        r/w,        0xf */
        }BF;
        uint32_t WORD;
    } clk_cfg3;

    /* 0x10 : clk_cfg4 */
    union {
        struct {
            uint32_t spi_clk_div                    :  5; /* [ 4: 0],        r/w,        0x3 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t spi_clk_en                     :  1; /* [    8],        r/w,        0x1 */
            uint32_t spi_clk_sel                    :  1; /* [    9],        r/w,        0x0 */
            uint32_t chip_clk_out_en                :  4; /* [13:10],        r/w,        0x0 */
            uint32_t gpio_tmr_clk_sel               :  2; /* [15:14],        r/w,        0x0 */
            uint32_t i2c_clk_div                    :  8; /* [23:16],        r/w,       0xff */
            uint32_t i2c_clk_en                     :  1; /* [   24],        r/w,        0x1 */
            uint32_t i2c_clk_sel                    :  1; /* [   25],        r/w,        0x0 */
            uint32_t chip_clk_out_2_sel             :  2; /* [27:26],        r/w,        0x0 */
            uint32_t chip_clk_out_0_sel             :  2; /* [29:28],        r/w,        0x0 */
            uint32_t chip_clk_out_1_sel             :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg4;

    /* 0x14 : clk_cfg5 */
    union {
        struct {
            uint32_t cfg_inv_eth_tx_clk             :  1; /* [    0],        r/w,        0x0 */
            uint32_t cfg_inv_eth_rx_clk             :  1; /* [    1],        r/w,        0x0 */
            uint32_t cfg_inv_eth_ref_clk_i          :  1; /* [    2],        r/w,        0x0 */
            uint32_t cfg_inv_eth_ref_clk_o          :  1; /* [    3],        r/w,        0x1 */
            uint32_t cfg_sel_eth_ref_clk_o          :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_cam_ref_clk_div            :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t reg_cam_ref_clk_div_en         :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_cam_ref_clk_sel            :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reg_iproai_clk_sel               :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reg_iproai_clk_div_en            :  1; /* [   12],        r/w,        0x1 */
            uint32_t reg_iproai_clk_div               :  3; /* [15:13],        r/w,        0x0 */
            uint32_t spi2_clk_div                   :  5; /* [20:16],        r/w,        0x3 */
            uint32_t spi2_clk_en                    :  1; /* [   21],        r/w,        0x1 */
            uint32_t spi2_clk_sel                   :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23_25                 :  3; /* [25:23],       rsvd,        0x0 */
            uint32_t reg_psram_clk_sel              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t reg_psram_clk_div_en           :  1; /* [   28],        r/w,        0x1 */
            uint32_t reg_psram_clk_div              :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg5;

    /* 0x18 : clk_cfg6 */
    union {
        struct {
            uint32_t i2s_ref_clk_div                :  6; /* [ 5: 0],        r/w,        0x0 */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t i2s_ref_clk_en                 :  1; /* [    7],        r/w,        0x1 */
            uint32_t pio_clk_div                    :  5; /* [12: 8],        r/w,        0x3 */
            uint32_t reserved_13                    :  1; /* [   13],       rsvd,        0x0 */
            uint32_t pio_clk_en                     :  1; /* [   14],        r/w,        0x1 */
            uint32_t pio_clk_sel                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t i2s_ref_clk_sel                :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clk_cfg6;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[4];

    /* 0x20 : swrst_cfg0 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t swrst_s300                     :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t swrst_s301                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t swrst_i2c_1                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg0;

    /* 0x24 : swrst_cfg1 */
    union {
        struct {
            uint32_t swrst_glb                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t swrst_mix                      :  1; /* [    1],        r/w,        0x0 */
            uint32_t swrst_gpip                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t swrst_sec_dbg                  :  1; /* [    3],        r/w,        0x0 */
            uint32_t swrst_sec_eng                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t swrst_tz1                      :  1; /* [    5],        r/w,        0x0 */
            uint32_t swrst_tz2                      :  1; /* [    6],        r/w,        0x0 */
            uint32_t swrst_efuse                    :  1; /* [    7],        r/w,        0x0 */
            uint32_t swrst_cci                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t swrst_l1c                      :  1; /* [    9],        r/w,        0x0 */
            uint32_t swrst_sf                       :  1; /* [   10],        r/w,        0x0 */
            uint32_t swrst_dma                      :  1; /* [   11],        r/w,        0x0 */
            uint32_t swrst_pds                      :  1; /* [   12],        r/w,        0x0 */
            uint32_t swrst_uart0                    :  1; /* [   13],        r/w,        0x0 */
            uint32_t swrst_spi                      :  1; /* [   14],        r/w,        0x0 */
            uint32_t swrst_i2c                      :  1; /* [   15],        r/w,        0x0 */
            uint32_t swrst_pwm                      :  1; /* [   16],        r/w,        0x0 */
            uint32_t swrst_tmr                      :  1; /* [   17],        r/w,        0x0 */
            uint32_t swrst_ir                       :  1; /* [   18],        r/w,        0x0 */
            uint32_t swrst_chk                      :  1; /* [   19],        r/w,        0x0 */
            uint32_t swrst_kys                      :  1; /* [   20],        r/w,        0x0 */
            uint32_t swrst_ausolo                   :  1; /* [   21],        r/w,        0x0 */
            uint32_t swrst_psram                    :  1; /* [   22],        r/w,        0x0 */
            uint32_t swrst_qdec                     :  1; /* [   23],        r/w,        0x0 */
            uint32_t swrst_pio                      :  1; /* [   24],        r/w,        0x0 */
            uint32_t swrst_mm                       :  1; /* [   25],        r/w,        0x0 */
            uint32_t swrst_usb                      :  1; /* [   26],        r/w,        0x0 */
            uint32_t swrst_emac                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t swrst_uart1                    :  1; /* [   28],        r/w,        0x0 */
            uint32_t swrst_can                      :  1; /* [   29],        r/w,        0x0 */
            uint32_t swrst_i2s                      :  1; /* [   30],        r/w,        0x0 */
            uint32_t swrst_spi2                     :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg1;

    /* 0x28 : swrst_cfg2 */
    union {
        struct {
            uint32_t reg_ctrl_pwron_rst             :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_ctrl_cpu_reset             :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_ctrl_sys_reset             :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t reg_ctrl_reset_dummy           :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t swrst_sdhA                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t disrst_sdhA                    :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_23                 : 14; /* [23:10],       rsvd,        0x0 */
            uint32_t pka_clk_sel                    :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg2;

    /* 0x2C : swrst_cfg3 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t disrst_mix                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t disrst_gpip                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t disrst_cci                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t disrst_sf                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t disrst_dma                     :  1; /* [   11],        r/w,        0x0 */
            uint32_t disrst_i2c_1                   :  1; /* [   12],        r/w,        0x0 */
            uint32_t disrst_uart0                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t disrst_spi                     :  1; /* [   14],        r/w,        0x0 */
            uint32_t disrst_i2c                     :  1; /* [   15],        r/w,        0x0 */
            uint32_t disrst_pwm                     :  1; /* [   16],        r/w,        0x0 */
            uint32_t disrst_tmr                     :  1; /* [   17],        r/w,        0x0 */
            uint32_t disrst_ir                      :  1; /* [   18],        r/w,        0x0 */
            uint32_t disrst_chk                     :  1; /* [   19],        r/w,        0x0 */
            uint32_t disrst_kys                     :  1; /* [   20],        r/w,        0x0 */
            uint32_t disrst_ausolo                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t disrst_psram                   :  1; /* [   22],        r/w,        0x0 */
            uint32_t disrst_qdec                    :  1; /* [   23],        r/w,        0x0 */
            uint32_t disrst_pio                     :  1; /* [   24],        r/w,        0x0 */
            uint32_t disrst_mm                      :  1; /* [   25],        r/w,        0x0 */
            uint32_t disrst_usb                     :  1; /* [   26],        r/w,        0x0 */
            uint32_t disrst_emac                    :  1; /* [   27],        r/w,        0x0 */
            uint32_t disrst_uart1                   :  1; /* [   28],        r/w,        0x0 */
            uint32_t disrst_can                     :  1; /* [   29],        r/w,        0x0 */
            uint32_t disrst_i2s                     :  1; /* [   30],        r/w,        0x0 */
            uint32_t disrst_spi2                    :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } swrst_cfg3;

    /* 0x30 : cgen_cfg0 */
    union {
        struct {
            uint32_t cgen_mcu                       :  1; /* [    0],        r/w,        0x1 */
            uint32_t cgen_sec                       :  1; /* [    1],        r/w,        0x1 */
            uint32_t cgen_dma                       :  1; /* [    2],        r/w,        0x1 */
            uint32_t cgen_cci                       :  1; /* [    3],        r/w,        0x0 */
            uint32_t cgen_gpip                      :  1; /* [    4],        r/w,        0x1 */
            uint32_t cgen_sec_dbg                   :  1; /* [    5],        r/w,        0x1 */
            uint32_t cgen_sec_eng                   :  1; /* [    6],        r/w,        0x1 */
            uint32_t cgen_tz1                       :  1; /* [    7],        r/w,        0x1 */
            uint32_t cgen_efuse                     :  1; /* [    8],        r/w,        0x1 */
            uint32_t cgen_sf                        :  1; /* [    9],        r/w,        0x1 */
            uint32_t cgen_spi2                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t cgen_uart                      :  1; /* [   11],        r/w,        0x1 */
            uint32_t cgen_spi                       :  1; /* [   12],        r/w,        0x0 */
            uint32_t cgen_i2c                       :  1; /* [   13],        r/w,        0x0 */
            uint32_t cgen_pwm                       :  1; /* [   14],        r/w,        0x0 */
            uint32_t cgen_tmr                       :  1; /* [   15],        r/w,        0x1 */
            uint32_t cgen_irr                       :  1; /* [   16],        r/w,        0x0 */
            uint32_t cgen_cks                       :  1; /* [   17],        r/w,        0x0 */
            uint32_t cgen_kys                       :  1; /* [   18],        r/w,        0x0 */
            uint32_t cgen_audio                     :  1; /* [   19],        r/w,        0x0 */
            uint32_t cgen_mm                        :  1; /* [   20],        r/w,        0x0 */
            uint32_t cgen_uart1                     :  1; /* [   21],        r/w,        0x0 */
            uint32_t cgen_can                       :  1; /* [   22],        r/w,        0x0 */
            uint32_t cgen_i2s                       :  1; /* [   23],        r/w,        0x0 */
            uint32_t cgen_qdec0                     :  1; /* [   24],        r/w,        0x0 */
            uint32_t cgen_qdec1                     :  1; /* [   25],        r/w,        0x0 */
            uint32_t cgen_qdec2                     :  1; /* [   26],        r/w,        0x0 */
            uint32_t cgen_pio                       :  1; /* [   27],        r/w,        0x0 */
            uint32_t cgen_usb                       :  1; /* [   28],        r/w,        0x0 */
            uint32_t cgen_pka                       :  1; /* [   29],        r/w,        0x0 */
            uint32_t cgen_emac                      :  1; /* [   30],        r/w,        0x0 */
            uint32_t cgen_sdhA                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg0;

    /* 0x34 : cgen_cfg1 */
    union {
        struct {
            uint32_t reserved_0_13                  : 14; /* [13: 0],       rsvd,        0x0 */
            uint32_t cgen_i2c_1                     :  1; /* [   14],        r/w,        0x0 */
            uint32_t reserved_15_31                 : 17; /* [31:15],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg1;

    /* 0x38 : cgen_cfg2 */
    union {
        struct {
            uint32_t cgen_s300                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t cgen_s301                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cgen_cfg2;

    /* 0x3c  reserved */
    uint8_t RESERVED0x3c[4];

    /* 0x40 : MBIST_CTL */
    union {
        struct {
            uint32_t irom_mbist_mode                :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t hsram_cache_mbist_mode         :  1; /* [    2],        r/w,        0x0 */
            uint32_t tag_mbist_mode                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t ocram_mbist_mode               :  1; /* [    4],        r/w,        0x0 */
            uint32_t em_ram_mbist_mode              :  1; /* [    5],        r/w,        0x0 */
            uint32_t m154_mbist_mode                :  1; /* [    6],        r/w,        0x0 */
            uint32_t sf_mbist_mode                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t sec_mbist_mode                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t mbist_mode                     :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_30                 : 21; /* [30:10],       rsvd,        0x0 */
            uint32_t reg_mbist_rst_n                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } MBIST_CTL;

    /* 0x44 : MBIST_STAT */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } MBIST_STAT;

    /* 0x48  reserved */
    uint8_t RESERVED0x48[8];

    /* 0x50 : NEW_RTC_TIME_L */
    union {
        struct {
            uint32_t rtc_timer_latch_l              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } NEW_RTC_TIME_L;

    /* 0x54 : NEW_RTC_TIME_H */
    union {
        struct {
            uint32_t rtc_timer_latch_h              : 13; /* [12: 0],          r,        0x0 */
            uint32_t reserved_13_23                 : 11; /* [23:13],       rsvd,        0x0 */
            uint32_t reg_rtc_timer_latch_en         :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } NEW_RTC_TIME_H;

    /* 0x58  reserved */
    uint8_t RESERVED0x58[4];

    /* 0x5C : XTAL_DEG_32K */
    union {
        struct {
            uint32_t xtal_deg_cnt_limit             :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_12                  :  5; /* [12: 8],       rsvd,        0x0 */
            uint32_t reg_rc32k_deg_en               :  1; /* [   13],        r/w,        0x0 */
            uint32_t rc32k_deg_end_ps               :  1; /* [   14],        w1p,        0x0 */
            uint32_t rc32k_deg_start_ps             :  1; /* [   15],        w1p,        0x0 */
            uint32_t reserved_16_26                 : 11; /* [26:16],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_cgen              :  1; /* [   27],        r/w,        0x1 */
            uint32_t clr_xtal_cnt_32k_done          :  1; /* [   28],        w1p,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t xtal_cnt_32k_sw_trig_ps        :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } XTAL_DEG_32K;

    /* 0x60 : bmx_cfg1 */
    union {
        struct {
            uint32_t reg_timeout_en                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t timeout_sts                    :  3; /* [10: 8],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_timeout_clr                :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_arb_mode                   :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_23                 :  6; /* [23:18],       rsvd,        0x0 */
            uint32_t hbn_apb_cfg                    :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_cfg1;

    /* 0x64 : bmx_cfg2 */
    union {
        struct {
            uint32_t reg_infra_berr_en              : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_13                 :  2; /* [13:12],       rsvd,        0x0 */
            uint32_t reg_infra_berr_last            :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_infra_berr_clr             :  1; /* [   15],        r/w,        0x0 */
            uint32_t sts_infra_berr_src             : 12; /* [27:16],          r,        0x0 */
            uint32_t reserved_28_29                 :  2; /* [29:28],       rsvd,        0x0 */
            uint32_t sts_infra_berr_write           :  1; /* [   30],          r,        0x0 */
            uint32_t sts_infra_berr                 :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_cfg2;

    /* 0x68 : bmx_err_addr */
    union {
        struct {
            uint32_t sts_infra_berr_addr            : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_err_addr;

    /* 0x6C : bmx_cfg3 */
    union {
        struct {
            uint32_t reg_frun_pclk                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } bmx_cfg3;

    /* 0x70 : rsv0 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t reg_prot_req_opi               :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_4                   :  3; /* [ 4: 2],       rsvd,        0x0 */
            uint32_t sts_prot_rdy_opi               :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_xasli_w_issue_thre_x2sf    :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t reg_hs2f_w_issue_thre_ih2ext   :  2; /* [21:20],        r/w,        0x0 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t reg_hs2f_rcmd_bp_en_ih2ext     :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_hasli_w_issue_thre_h2sf    :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reg_hasli_w_issue_thre_h2opi   :  2; /* [27:26],        r/w,        0x0 */
            uint32_t reg_xasli_w_issue_thre_ix2ext  :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reg_xasli_w_issue_thre_blai2ext :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv0;

    /* 0x74 : rsv1 */
    union {
        struct {
            uint32_t cr_snsr_reset                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_snsr_pwdn                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t rsvd_31_2                      : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv1;

    /* 0x78 : rsv2 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rsv2;

    /* 0x7C : rsv3 */
    union {
        struct {
            uint32_t rsvd_31_0                      : 32; /* [31: 0],       rsvd, 0xffffffff */
        }BF;
        uint32_t WORD;
    } rsv3;

    /* 0x80  reserved */
    uint8_t RESERVED0x80[4];

    /* 0x84 : ocram_parm */
    union {
        struct {
            uint32_t reg_sram_parm2_mm              :  5; /* [ 4: 0],        r/w,        0xc */
            uint32_t reserved_5_11                  :  7; /* [11: 5],       rsvd,        0x0 */
            uint32_t reg_sram_parm1                 : 19; /* [30:12],        r/w,     0xc2da */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ocram_parm;

    /* 0x88 : sram_parm */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t bz_ram_ret1                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t bz_ram_ret2                    :  1; /* [   17],        r/w,        0x0 */
            uint32_t bz_ram_pgen                    :  1; /* [   18],        r/w,        0x0 */
            uint32_t top_ram_ret1                   :  1; /* [   19],        r/w,        0x0 */
            uint32_t top_ram_ret2                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t top_ram_pgen                   :  1; /* [   21],        r/w,        0x0 */
            uint32_t cpu_ram_ret1                   :  1; /* [   22],        r/w,        0x0 */
            uint32_t cpu_ram_ret2                   :  1; /* [   23],        r/w,        0x0 */
            uint32_t cpu_ram_pgen                   :  1; /* [   24],        r/w,        0x0 */
            uint32_t mm_ram_ret1                    :  1; /* [   25],        r/w,        0x0 */
            uint32_t mm_ram_ret2                    :  1; /* [   26],        r/w,        0x0 */
            uint32_t mm_ram_pgen                    :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sram_parm;

    /* 0x8C : sram_misc */
    union {
        struct {
            uint32_t cr_em_sel                      :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t cr_iproai_en                     :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sram_misc;

    /* 0x90 : glb_parm */
    union {
        struct {
            uint32_t jtag_swap_set                  :  8; /* [ 7: 0],        r/w,        0x4 */
            uint32_t cfg_sflash_swap_io0_io3        :  1; /* [    8],        r/w,        0x0 */
            uint32_t cfg_sflash_swap_cs_io2         :  1; /* [    9],        r/w,        0x0 */
            uint32_t cfg_sflash_reverse             :  1; /* [   10],        r/w,        0x0 */
            uint32_t cfg_flash2_scenario            :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_spi_0_master_mode          :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_spi_0_swap                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_cci_use_jtag_pin           :  1; /* [   16],        r/w,        0x1 */
            uint32_t p1_adc_test_with_cci           :  1; /* [   17],        r/w,        0x0 */
            uint32_t p2_dac_test_with_cci           :  1; /* [   18],        r/w,        0x0 */
            uint32_t p3_cci_use_io_0_2_7            :  1; /* [   19],        r/w,        0x0 */
            uint32_t p4_adc_test_with_jtag          :  1; /* [   20],        r/w,        0x0 */
            uint32_t p5_dac_test_with_jtag          :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t p6_jtag_use_io_0_2_7           :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_27                 :  4; /* [27:24],       rsvd,        0x0 */
            uint32_t reg_en_gpio_o_latch_mode       :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_kys_drv_val                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reg_spi_1_master_mode          :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_spi_1_swap                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } glb_parm;

    /* 0x94 : PDM_CLK_CTRL */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } PDM_CLK_CTRL;

    /* 0x98 : GPIO_USE_PSRAM__IO */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_USE_PSRAM__IO;

    /* 0x9c  reserved */
    uint8_t RESERVED0x9c[4];

    /* 0xA0 : CPU_CLK_CFG */
    union {
        struct {
            uint32_t cpu_rtc_div                    : 17; /* [16: 0],        r/w,       0x1f */
            uint32_t reserved_17                    :  1; /* [   17],       rsvd,        0x0 */
            uint32_t cpu_rtc_en                     :  1; /* [   18],        r/w,        0x1 */
            uint32_t cpu_rtc_sel                    :  1; /* [   19],        r/w,        0x1 */
            uint32_t debug_ndreset_gate             :  1; /* [   20],        r/w,        0x0 */
            uint32_t reserved_21_31                 : 11; /* [31:21],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CPU_CLK_CFG;

    /* 0xA4 : CLK_DESPUR_ROOT_CLK_CFG */
    union {
        struct {
            uint32_t cfg_despur_bmap_root_clk       : 28; /* [27: 0],        r/w,  0xfffffff */
            uint32_t cfg_despur_cgen_root_clk       :  1; /* [   28],        r/w,        0x0 */
            uint32_t cfg_despur_enable_root_clk     :  1; /* [   29],        r/w,        0x0 */
            uint32_t cfg_despur_clk_sel_root_clk    :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CLK_DESPUR_ROOT_CLK_CFG;

    /* 0xA8 : CLK_DESPUR_CLK_SF_CFG */
    union {
        struct {
            uint32_t cfg_despur_bmap_clk_sf         : 28; /* [27: 0],        r/w,  0xfffffff */
            uint32_t cfg_despur_cgen_clk_sf         :  1; /* [   28],        r/w,        0x0 */
            uint32_t cfg_despur_enable_clk_sf       :  1; /* [   29],        r/w,        0x0 */
            uint32_t cfg_despur_clk_sel_clk_sf      :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CLK_DESPUR_CLK_SF_CFG;

    /* 0xAC : WIFI_BT_COEX_CTRL */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WIFI_BT_COEX_CTRL;

    /* 0xb0  reserved */
    uint8_t RESERVED0xb0[4];

    /* 0xB4 : GPADC_32M_SRC_CTRL */
    union {
        struct {
            uint32_t gpadc_32m_clk_div              :  6; /* [ 5: 0],        r/w,        0x2 */
            uint32_t reserved_6                     :  1; /* [    6],       rsvd,        0x0 */
            uint32_t gpadc_32m_clk_sel              :  1; /* [    7],        r/w,        0x1 */
            uint32_t gpadc_32m_div_en               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPADC_32M_SRC_CTRL;

    /* 0xB8 : DIG32K_WAKEUP_CTRL */
    union {
        struct {
            uint32_t dig_32k_div                    : 11; /* [10: 0],        r/w,      0x3e8 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dig_32k_en                     :  1; /* [   12],        r/w,        0x1 */
            uint32_t dig_32k_comp                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t dig_512k_div                   :  7; /* [22:16],        r/w,       0x3e */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t dig_512k_en                    :  1; /* [   24],        r/w,        0x1 */
            uint32_t dig_512k_comp                  :  1; /* [   25],        r/w,        0x1 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t dig_clk_src_sel                :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t sw_ble_wakeup_req              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DIG32K_WAKEUP_CTRL;

    /* 0xbc  reserved */
    uint8_t RESERVED0xbc[4];

    /* 0xC0 : BZ_COEX_CTRL */
    union {
        struct {
            uint32_t coex_en                        :  1; /* [    0],        r/w,        0x0 */
            uint32_t wlan_en                        :  1; /* [    1],        r/w,        0x0 */
            uint32_t ble_rx_ignore                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t m154_rx_ignore                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t bz_pri_thr                     :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t bz_pri_en                      :  1; /* [    8],        r/w,        0x1 */
            uint32_t bz_pri_pol                     :  1; /* [    9],        r/w,        0x1 */
            uint32_t bz_active_pol                  :  1; /* [   10],        r/w,        0x1 */
            uint32_t bz_abort_pol                   :  1; /* [   11],        r/w,        0x1 */
            uint32_t coex_pri                       :  1; /* [   12],        r/w,        0x0 */
            uint32_t force_m154_win                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t force_ble_win                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t coex_option                    :  1; /* [   15],        r/w,        0x0 */
            uint32_t coex_force_ch                  :  7; /* [22:16],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t m154_rx_abort_dis              :  1; /* [   24],        r/w,        0x0 */
            uint32_t m154_tx_abort_dis              :  1; /* [   25],        r/w,        0x0 */
            uint32_t ble_rx_abort_dis               :  1; /* [   26],        r/w,        0x0 */
            uint32_t ble_tx_abort_dis               :  1; /* [   27],        r/w,        0x0 */
            uint32_t coex_arb                       :  4; /* [31:28],          r,        0x8 */
        }BF;
        uint32_t WORD;
    } BZ_COEX_CTRL;

    /* 0xc4  reserved */
    uint8_t RESERVED0xc4[4];

    /* 0xC8 : UART_SIG_SEL_0 */
    union {
        struct {
            uint32_t uart_sig_0_sel                 :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t uart_sig_1_sel                 :  4; /* [ 7: 4],        r/w,        0x1 */
            uint32_t uart_sig_2_sel                 :  4; /* [11: 8],        r/w,        0x2 */
            uint32_t uart_sig_3_sel                 :  4; /* [15:12],        r/w,        0x3 */
            uint32_t uart_sig_4_sel                 :  4; /* [19:16],        r/w,        0x4 */
            uint32_t uart_sig_5_sel                 :  4; /* [23:20],        r/w,        0x5 */
            uint32_t uart_sig_6_sel                 :  4; /* [27:24],        r/w,        0x6 */
            uint32_t uart_sig_7_sel                 :  4; /* [31:28],        r/w,        0x7 */
        }BF;
        uint32_t WORD;
    } UART_SIG_SEL_0;

    /* 0xCC : UART_SIG_SEL_1 */
    union {
        struct {
            uint32_t uart_sig_8_sel                 :  4; /* [ 3: 0],        r/w,        0x8 */
            uint32_t uart_sig_9_sel                 :  4; /* [ 7: 4],        r/w,        0x9 */
            uint32_t uart_sig_10_sel                :  4; /* [11: 8],        r/w,        0xa */
            uint32_t uart_sig_11_sel                :  4; /* [15:12],        r/w,        0xb */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } UART_SIG_SEL_1;

    /* 0xD0 : DBG_SEL_LL */
    union {
        struct {
            uint32_t reg_dbg_ll_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_LL;

    /* 0xD4 : DBG_SEL_LH */
    union {
        struct {
            uint32_t reg_dbg_lh_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_LH;

    /* 0xD8 : DBG_SEL_HL */
    union {
        struct {
            uint32_t reg_dbg_hl_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_HL;

    /* 0xDC : DBG_SEL_HH */
    union {
        struct {
            uint32_t reg_dbg_hh_ctrl                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_SEL_HH;

    /* 0xE0 : debug */
    union {
        struct {
            uint32_t debug_oe                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t debug_i                        : 31; /* [31: 1],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } debug;

    /* 0xe4  reserved */
    uint8_t RESERVED0xe4[28];

    /* 0x100 : GPIO_CFGCTL0 */
    union {
        struct {
            uint32_t reg_gpio_0_ie                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio_0_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_0_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_0_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_0_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_0_func_sel            :  5; /* [12: 8],        r/w,        0xf */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_0_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_1_ie                  :  1; /* [   16],        r/w,        0x1 */
            uint32_t reg_gpio_1_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_1_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_1_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_1_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_1_func_sel            :  5; /* [28:24],        r/w,        0xf */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_1_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL0;

    /* 0x104 : GPIO_CFGCTL1 */
    union {
        struct {
            uint32_t reg_gpio_2_ie                  :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio_2_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_2_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_2_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_2_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_2_func_sel            :  5; /* [12: 8],        r/w,        0xf */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_2_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_3_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_3_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_3_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_3_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_3_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_3_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_3_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL1;

    /* 0x108 : GPIO_CFGCTL2 */
    union {
        struct {
            uint32_t reg_gpio_4_ie                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_4_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_4_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_4_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_4_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_4_func_sel            :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_4_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_5_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_5_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_5_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_5_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_5_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_5_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_5_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL2;

    /* 0x10C : GPIO_CFGCTL3 */
    union {
        struct {
            uint32_t reg_gpio_6_ie                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_6_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_6_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_6_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_6_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_6_func_sel            :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_6_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_7_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_7_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_7_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_7_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_7_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_7_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_7_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL3;

    /* 0x110 : GPIO_CFGCTL4 */
    union {
        struct {
            uint32_t reg_gpio_8_ie                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_8_smt                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_8_drv                 :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_8_pu                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_8_pd                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_8_func_sel            :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_8_mode                :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_9_ie                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_9_smt                 :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_9_drv                 :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_9_pu                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_9_pd                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_9_func_sel            :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_9_mode                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL4;

    /* 0x114 : GPIO_CFGCTL5 */
    union {
        struct {
            uint32_t reg_gpio_10_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_10_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_10_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_10_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_10_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_10_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_10_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_11_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_11_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_11_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_11_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_11_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_11_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_11_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL5;

    /* 0x118 : GPIO_CFGCTL6 */
    union {
        struct {
            uint32_t reg_gpio_12_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_12_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_12_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_12_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_12_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_12_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_12_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_13_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_13_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_13_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_13_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_13_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_13_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_13_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL6;

    /* 0x11C : GPIO_CFGCTL7 */
    union {
        struct {
            uint32_t reg_gpio_14_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_14_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_14_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_14_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_14_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_14_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_14_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_15_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_15_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_15_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_15_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_15_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_15_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_15_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL7;

    /* 0x120 : GPIO_CFGCTL8 */
    union {
        struct {
            uint32_t reg_gpio_16_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_16_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_16_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_16_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_16_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_16_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_16_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_17_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_17_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_17_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_17_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_17_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_17_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL8;

    /* 0x124 : GPIO_CFGCTL9 */
    union {
        struct {
            uint32_t reg_gpio_18_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_18_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_18_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_18_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_18_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_18_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_18_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_19_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_19_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_19_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_19_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_19_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_19_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_19_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL9;

    /* 0x128 : GPIO_CFGCTL10 */
    union {
        struct {
            uint32_t reg_gpio_20_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_20_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_20_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_20_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_20_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_20_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_20_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_21_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_21_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_21_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_21_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_21_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_21_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL10;

    /* 0x12C : GPIO_CFGCTL11 */
    union {
        struct {
            uint32_t reg_gpio_22_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_22_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_22_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_22_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_22_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_22_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_22_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_23_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_23_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_23_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_23_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_23_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_23_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_23_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL11;

    /* 0x130 : GPIO_CFGCTL12 */
    union {
        struct {
            uint32_t reg_gpio_24_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_24_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_24_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_24_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_24_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_24_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_24_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_25_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_25_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_25_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_25_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_25_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_25_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_25_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL12;

    /* 0x134 : GPIO_CFGCTL13 */
    union {
        struct {
            uint32_t reg_gpio_26_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_26_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_26_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_26_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_26_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_26_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_26_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_27_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_27_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_27_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_27_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_27_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_27_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_27_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL13;

    /* 0x138 : GPIO_CFGCTL14 */
    union {
        struct {
            uint32_t reg_gpio_28_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_28_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_28_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_28_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_28_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_28_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_28_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_29_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_29_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_29_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_29_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_29_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_29_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_29_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL14;

    /* 0x13C : GPIO_CFGCTL15 */
    union {
        struct {
            uint32_t reg_gpio_30_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_30_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_30_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_30_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_30_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_30_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_30_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_31_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_31_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_31_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_31_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_31_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_31_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_31_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL15;

    /* 0x140 : GPIO_CFGCTL16 */
    union {
        struct {
            uint32_t reg_gpio_32_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_32_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_32_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_32_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_32_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_32_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_32_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_33_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_33_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_33_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_33_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_33_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_33_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_33_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL16;

    /* 0x144 : GPIO_CFGCTL17 */
    union {
        struct {
            uint32_t reg_gpio_34_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_34_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_34_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_34_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_34_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_34_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_34_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_35_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_35_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_35_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_35_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_35_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_35_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_35_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL17;

    /* 0x148 : GPIO_CFGCTL18 */
    union {
        struct {
            uint32_t reg_gpio_36_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_36_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_36_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_36_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_36_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_36_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_36_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_37_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_37_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_37_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_37_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_37_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_37_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_37_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL18;

    /* 0x14C : GPIO_CFGCTL19 */
    union {
        struct {
            uint32_t reg_gpio_38_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_38_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_38_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_38_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_38_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_38_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_38_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_39_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_39_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_39_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_39_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_39_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_39_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_39_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL19;

    /* 0x150 : GPIO_CFGCTL20 */
    union {
        struct {
            uint32_t reg_gpio_40_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_40_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_40_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_40_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_40_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_40_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_40_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_41_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_41_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_41_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_41_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_41_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_41_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_41_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL20;

    /* 0x154 : GPIO_CFGCTL21 */
    union {
        struct {
            uint32_t reg_gpio_42_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_42_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_42_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_42_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_42_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_42_func_sel           :  5; /* [12: 8],        r/w,        0xb */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t reg_gpio_42_mode               :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_43_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_43_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_43_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_43_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_43_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_gpio_43_func_sel           :  5; /* [28:24],        r/w,        0xb */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t reg_gpio_43_mode               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL21;

    /* 0x158 : GPIO_CFGCTL22 */
    union {
        struct {
            uint32_t reg_gpio_44_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_44_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_44_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_44_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_44_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_45_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_45_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_45_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_45_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_45_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL22;

    /* 0x15C : GPIO_CFGCTL23 */
    union {
        struct {
            uint32_t reg_gpio_46_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_46_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_46_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_46_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_46_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_47_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_47_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_47_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_47_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_47_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL23;

    /* 0x160 : GPIO_CFGCTL24 */
    union {
        struct {
            uint32_t reg_gpio_48_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_48_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_48_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_48_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_48_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t reg_gpio_49_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_49_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_49_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_49_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_49_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL24;

    /* 0x164 : GPIO_CFGCTL25 */
    union {
        struct {
            uint32_t reg_gpio_50_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_50_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_50_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_50_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_50_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_50_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_50_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_50_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_51_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_51_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_51_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_51_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_51_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_51_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_51_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_51_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL25;

    /* 0x168 : GPIO_CFGCTL26 */
    union {
        struct {
            uint32_t reg_gpio_52_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_52_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_52_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_52_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_52_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_52_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_52_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_52_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_53_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_53_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_53_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_53_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_53_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_53_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_53_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_53_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL26;

    /* 0x16C : GPIO_CFGCTL27 */
    union {
        struct {
            uint32_t reg_gpio_54_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_54_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_54_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_54_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_54_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_54_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_54_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_54_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_55_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_55_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_55_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_55_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_55_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_55_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_55_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_55_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL27;

    /* 0x170 : GPIO_CFGCTL28 */
    union {
        struct {
            uint32_t reg_gpio_56_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_56_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_56_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_56_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_56_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_56_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_56_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_56_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_57_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_57_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_57_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_57_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_57_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_57_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_57_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_57_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL28;

    /* 0x174 : GPIO_CFGCTL29 */
    union {
        struct {
            uint32_t reg_gpio_58_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_58_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_58_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_58_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_58_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_58_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_58_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_58_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_59_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_59_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_59_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_59_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_59_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_59_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_59_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_59_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL29;

    /* 0x178 : GPIO_CFGCTL30 */
    union {
        struct {
            uint32_t reg_gpio_60_ie                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_60_smt                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio_60_drv                :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t reg_gpio_60_pu                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_60_pd                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_60_es2                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_60_es3                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_60_es4                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t reg_gpio_61_ie                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_61_smt                :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio_61_drv                :  2; /* [19:18],        r/w,        0x0 */
            uint32_t reg_gpio_61_pu                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_61_pd                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_61_es2                :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_61_es3                :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_61_es4                :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL30;

    /* 0x17c  reserved */
    uint8_t RESERVED0x17c[4];

    /* 0x180 : GPIO_CFGCTL31 */
    union {
        struct {
            uint32_t reg_gpio_0_i                   :  1; /* [    0],          r,        0x0 */
            uint32_t reg_gpio_1_i                   :  1; /* [    1],          r,        0x0 */
            uint32_t reg_gpio_2_i                   :  1; /* [    2],          r,        0x0 */
            uint32_t reg_gpio_3_i                   :  1; /* [    3],          r,        0x0 */
            uint32_t reg_gpio_4_i                   :  1; /* [    4],          r,        0x0 */
            uint32_t reg_gpio_5_i                   :  1; /* [    5],          r,        0x0 */
            uint32_t reg_gpio_6_i                   :  1; /* [    6],          r,        0x0 */
            uint32_t reg_gpio_7_i                   :  1; /* [    7],          r,        0x0 */
            uint32_t reg_gpio_8_i                   :  1; /* [    8],          r,        0x0 */
            uint32_t reg_gpio_9_i                   :  1; /* [    9],          r,        0x0 */
            uint32_t reg_gpio_10_i                  :  1; /* [   10],          r,        0x0 */
            uint32_t reg_gpio_11_i                  :  1; /* [   11],          r,        0x0 */
            uint32_t reg_gpio_12_i                  :  1; /* [   12],          r,        0x0 */
            uint32_t reg_gpio_13_i                  :  1; /* [   13],          r,        0x0 */
            uint32_t reg_gpio_14_i                  :  1; /* [   14],          r,        0x0 */
            uint32_t reg_gpio_15_i                  :  1; /* [   15],          r,        0x0 */
            uint32_t reg_gpio_16_i                  :  1; /* [   16],          r,        0x0 */
            uint32_t reg_gpio_17_i                  :  1; /* [   17],          r,        0x0 */
            uint32_t reg_gpio_18_i                  :  1; /* [   18],          r,        0x0 */
            uint32_t reg_gpio_19_i                  :  1; /* [   19],          r,        0x0 */
            uint32_t reg_gpio_20_i                  :  1; /* [   20],          r,        0x0 */
            uint32_t reg_gpio_21_i                  :  1; /* [   21],          r,        0x0 */
            uint32_t reg_gpio_22_i                  :  1; /* [   22],          r,        0x0 */
            uint32_t reg_gpio_23_i                  :  1; /* [   23],          r,        0x0 */
            uint32_t reg_gpio_24_i                  :  1; /* [   24],          r,        0x0 */
            uint32_t reg_gpio_25_i                  :  1; /* [   25],          r,        0x0 */
            uint32_t reg_gpio_26_i                  :  1; /* [   26],          r,        0x0 */
            uint32_t reg_gpio_27_i                  :  1; /* [   27],          r,        0x0 */
            uint32_t reg_gpio_28_i                  :  1; /* [   28],          r,        0x0 */
            uint32_t reg_gpio_29_i                  :  1; /* [   29],          r,        0x0 */
            uint32_t reg_gpio_30_i                  :  1; /* [   30],          r,        0x0 */
            uint32_t reg_gpio_31_i                  :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL31;

    /* 0x184 : GPIO_CFGCTL32 */
    union {
        struct {
            uint32_t reg_gpio_32_i                  :  1; /* [    0],          r,        0x0 */
            uint32_t reg_gpio_33_i                  :  1; /* [    1],          r,        0x0 */
            uint32_t reg_gpio_34_i                  :  1; /* [    2],          r,        0x0 */
            uint32_t reg_gpio_35_i                  :  1; /* [    3],          r,        0x0 */
            uint32_t reg_gpio_36_i                  :  1; /* [    4],          r,        0x0 */
            uint32_t reg_gpio_37_i                  :  1; /* [    5],          r,        0x0 */
            uint32_t reg_gpio_38_i                  :  1; /* [    6],          r,        0x0 */
            uint32_t reg_gpio_39_i                  :  1; /* [    7],          r,        0x0 */
            uint32_t reg_gpio_40_i                  :  1; /* [    8],          r,        0x0 */
            uint32_t reg_gpio_41_i                  :  1; /* [    9],          r,        0x0 */
            uint32_t reg_gpio_42_i                  :  1; /* [   10],          r,        0x0 */
            uint32_t reg_gpio_43_i                  :  1; /* [   11],          r,        0x0 */
            uint32_t gpio_int_stat2                 : 12; /* [23:12],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL32;

    /* 0x188 : GPIO_CFGCTL33 */
    union {
        struct {
            uint32_t reg_gpio_0_o                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_1_o                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_2_o                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_3_o                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_gpio_4_o                   :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_5_o                   :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_6_o                   :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_7_o                   :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_8_o                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_9_o                   :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_10_o                  :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_11_o                  :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio_12_o                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_gpio_13_o                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_gpio_14_o                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_gpio_15_o                  :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_16_o                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_o                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_gpio_18_o                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t reg_gpio_19_o                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t reg_gpio_20_o                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_o                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_22_o                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_23_o                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_24_o                  :  1; /* [   24],        r/w,        0x0 */
            uint32_t reg_gpio_25_o                  :  1; /* [   25],        r/w,        0x0 */
            uint32_t reg_gpio_26_o                  :  1; /* [   26],        r/w,        0x0 */
            uint32_t reg_gpio_27_o                  :  1; /* [   27],        r/w,        0x0 */
            uint32_t reg_gpio_28_o                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_gpio_29_o                  :  1; /* [   29],        r/w,        0x0 */
            uint32_t reg_gpio_30_o                  :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_gpio_31_o                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL33;

    /* 0x18C : GPIO_CFGCTL34 */
    union {
        struct {
            uint32_t reg_gpio_32_oe                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_33_oe                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_34_oe                 :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_35_oe                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_gpio_36_oe                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_37_oe                 :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_38_oe                 :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_39_oe                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_40_oe                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_41_oe                 :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_42_oe                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_43_oe                 :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio_32_o                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_gpio_33_o                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_gpio_34_o                  :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_gpio_35_o                  :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_36_o                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_37_o                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_gpio_38_o                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t reg_gpio_39_o                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t reg_gpio_40_o                  :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_41_o                  :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_42_o                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_43_o                  :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL34;

    /* 0x190 : GPIO_CFGCTL35 */
    union {
        struct {
            uint32_t reg_gpio_0_oe                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_1_oe                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_2_oe                  :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_3_oe                  :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_gpio_4_oe                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_5_oe                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_6_oe                  :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_7_oe                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_8_oe                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_9_oe                  :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_10_oe                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_11_oe                 :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio_12_oe                 :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_gpio_13_oe                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_gpio_14_oe                 :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_gpio_15_oe                 :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_gpio_16_oe                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reg_gpio_17_oe                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_gpio_18_oe                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t reg_gpio_19_oe                 :  1; /* [   19],        r/w,        0x0 */
            uint32_t reg_gpio_20_oe                 :  1; /* [   20],        r/w,        0x0 */
            uint32_t reg_gpio_21_oe                 :  1; /* [   21],        r/w,        0x0 */
            uint32_t reg_gpio_22_oe                 :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_gpio_23_oe                 :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_gpio_24_oe                 :  1; /* [   24],        r/w,        0x0 */
            uint32_t reg_gpio_25_oe                 :  1; /* [   25],        r/w,        0x0 */
            uint32_t reg_gpio_26_oe                 :  1; /* [   26],        r/w,        0x0 */
            uint32_t reg_gpio_27_oe                 :  1; /* [   27],        r/w,        0x0 */
            uint32_t reg_gpio_28_oe                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t reg_gpio_29_oe                 :  1; /* [   29],        r/w,        0x0 */
            uint32_t reg_gpio_30_oe                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t reg_gpio_31_oe                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL35;

    /* 0x194 : GPIO_CFGCTL36 */
    union {
        struct {
            uint32_t reg_gpio_0_set                 :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio_1_set                 :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio_2_set                 :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio_3_set                 :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_gpio_4_set                 :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_gpio_5_set                 :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_gpio_6_set                 :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_gpio_7_set                 :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio_8_set                 :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio_9_set                 :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio_10_set                :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio_11_set                :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio_12_set                :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio_13_set                :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio_14_set                :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio_15_set                :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio_16_set                :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio_17_set                :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio_18_set                :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio_19_set                :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio_20_set                :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio_21_set                :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio_22_set                :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio_23_set                :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio_24_set                :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio_25_set                :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio_26_set                :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio_27_set                :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio_28_set                :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio_29_set                :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio_30_set                :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio_31_set                :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL36;

    /* 0x198 : GPIO_CFGCTL37 */
    union {
        struct {
            uint32_t reg_gpio_0_clr                 :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio_1_clr                 :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio_2_clr                 :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio_3_clr                 :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_gpio_4_clr                 :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_gpio_5_clr                 :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_gpio_6_clr                 :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_gpio_7_clr                 :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio_8_clr                 :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio_9_clr                 :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio_10_clr                :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio_11_clr                :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio_12_clr                :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio_13_clr                :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio_14_clr                :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio_15_clr                :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio_16_clr                :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio_17_clr                :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio_18_clr                :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio_19_clr                :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio_20_clr                :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio_21_clr                :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio_22_clr                :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio_23_clr                :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio_24_clr                :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio_25_clr                :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio_26_clr                :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio_27_clr                :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio_28_clr                :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio_29_clr                :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio_30_clr                :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio_31_clr                :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL37;

    /* 0x19C : GPIO_CFGCTL38 */
    union {
        struct {
            uint32_t reg_gpio_32_set                :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio_33_set                :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio_34_set                :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio_35_set                :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_gpio_36_set                :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_gpio_37_set                :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_gpio_38_set                :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_gpio_39_set                :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio_40_set                :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio_41_set                :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio_42_set                :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio_43_set                :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio_32_clr                :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio_33_clr                :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio_34_clr                :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio_35_clr                :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio_36_clr                :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio_37_clr                :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio_38_clr                :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio_39_clr                :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio_40_clr                :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio_41_clr                :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio_42_clr                :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio_43_clr                :  1; /* [   23],        w1p,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_CFGCTL38;

    /* 0x1A0 : GPIO_INT_MASK1 */
    union {
        struct {
            uint32_t reg_gpio0_int_mask             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reg_gpio1_int_mask             :  1; /* [    1],        r/w,        0x1 */
            uint32_t reg_gpio2_int_mask             :  1; /* [    2],        r/w,        0x1 */
            uint32_t reg_gpio3_int_mask             :  1; /* [    3],        r/w,        0x1 */
            uint32_t reg_gpio4_int_mask             :  1; /* [    4],        r/w,        0x1 */
            uint32_t reg_gpio5_int_mask             :  1; /* [    5],        r/w,        0x1 */
            uint32_t reg_gpio6_int_mask             :  1; /* [    6],        r/w,        0x1 */
            uint32_t reg_gpio7_int_mask             :  1; /* [    7],        r/w,        0x1 */
            uint32_t reg_gpio8_int_mask             :  1; /* [    8],        r/w,        0x1 */
            uint32_t reg_gpio9_int_mask             :  1; /* [    9],        r/w,        0x1 */
            uint32_t reg_gpio10_int_mask            :  1; /* [   10],        r/w,        0x1 */
            uint32_t reg_gpio11_int_mask            :  1; /* [   11],        r/w,        0x1 */
            uint32_t reg_gpio12_int_mask            :  1; /* [   12],        r/w,        0x1 */
            uint32_t reg_gpio13_int_mask            :  1; /* [   13],        r/w,        0x1 */
            uint32_t reg_gpio14_int_mask            :  1; /* [   14],        r/w,        0x1 */
            uint32_t reg_gpio15_int_mask            :  1; /* [   15],        r/w,        0x1 */
            uint32_t reg_gpio16_int_mask            :  1; /* [   16],        r/w,        0x1 */
            uint32_t reg_gpio17_int_mask            :  1; /* [   17],        r/w,        0x1 */
            uint32_t reg_gpio18_int_mask            :  1; /* [   18],        r/w,        0x1 */
            uint32_t reg_gpio19_int_mask            :  1; /* [   19],        r/w,        0x1 */
            uint32_t reg_gpio20_int_mask            :  1; /* [   20],        r/w,        0x1 */
            uint32_t reg_gpio21_int_mask            :  1; /* [   21],        r/w,        0x1 */
            uint32_t reg_gpio22_int_mask            :  1; /* [   22],        r/w,        0x1 */
            uint32_t reg_gpio23_int_mask            :  1; /* [   23],        r/w,        0x1 */
            uint32_t reg_gpio24_int_mask            :  1; /* [   24],        r/w,        0x1 */
            uint32_t reg_gpio25_int_mask            :  1; /* [   25],        r/w,        0x1 */
            uint32_t reg_gpio26_int_mask            :  1; /* [   26],        r/w,        0x1 */
            uint32_t reg_gpio27_int_mask            :  1; /* [   27],        r/w,        0x1 */
            uint32_t reg_gpio28_int_mask            :  1; /* [   28],        r/w,        0x1 */
            uint32_t reg_gpio29_int_mask            :  1; /* [   29],        r/w,        0x1 */
            uint32_t reg_gpio30_int_mask            :  1; /* [   30],        r/w,        0x1 */
            uint32_t reg_gpio31_int_mask            :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MASK1;

    /* 0x1A4 : GPIO_INT_MASK2 */
    union {
        struct {
            uint32_t reg_gpio_32_mask               :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio_33_mask               :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio_34_mask               :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio_35_mask               :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_gpio_36_mask               :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio_37_mask               :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio_38_mask               :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio_39_mask               :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio_40_mask               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio_41_mask               :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio_42_mask               :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio_43_mask               :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MASK2;

    /* 0x1A8 : GPIO_INT_STAT1 */
    union {
        struct {
            uint32_t gpio_int_stat1                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_STAT1;

    /* 0x1AC : GPIO_INT_MASK3 */
    union {
        struct {
            uint32_t reg_gpio32_int_mask            :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_gpio33_int_mask            :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_gpio34_int_mask            :  1; /* [    2],        r/w,        0x0 */
            uint32_t reg_gpio35_int_mask            :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_gpio36_int_mask            :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_gpio37_int_mask            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reg_gpio38_int_mask            :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_gpio39_int_mask            :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_gpio40_int_mask            :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_gpio41_int_mask            :  1; /* [    9],        r/w,        0x0 */
            uint32_t reg_gpio42_int_mask            :  1; /* [   10],        r/w,        0x0 */
            uint32_t reg_gpio43_int_mask            :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_gpio32_int_clr             :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio33_int_clr             :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio34_int_clr             :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio35_int_clr             :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio36_int_clr             :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio37_int_clr             :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio38_int_clr             :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio39_int_clr             :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio40_int_clr             :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio41_int_clr             :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio42_int_clr             :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio43_int_clr             :  1; /* [   23],        w1p,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MASK3;

    /* 0x1B0 : GPIO_INT_CLR1 */
    union {
        struct {
            uint32_t reg_gpio0_int_clr              :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_gpio1_int_clr              :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_gpio2_int_clr              :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_gpio3_int_clr              :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_gpio4_int_clr              :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_gpio5_int_clr              :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_gpio6_int_clr              :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_gpio7_int_clr              :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_gpio8_int_clr              :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_gpio9_int_clr              :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_gpio10_int_clr             :  1; /* [   10],        w1p,        0x0 */
            uint32_t reg_gpio11_int_clr             :  1; /* [   11],        w1p,        0x0 */
            uint32_t reg_gpio12_int_clr             :  1; /* [   12],        w1p,        0x0 */
            uint32_t reg_gpio13_int_clr             :  1; /* [   13],        w1p,        0x0 */
            uint32_t reg_gpio14_int_clr             :  1; /* [   14],        w1p,        0x0 */
            uint32_t reg_gpio15_int_clr             :  1; /* [   15],        w1p,        0x0 */
            uint32_t reg_gpio16_int_clr             :  1; /* [   16],        w1p,        0x0 */
            uint32_t reg_gpio17_int_clr             :  1; /* [   17],        w1p,        0x0 */
            uint32_t reg_gpio18_int_clr             :  1; /* [   18],        w1p,        0x0 */
            uint32_t reg_gpio19_int_clr             :  1; /* [   19],        w1p,        0x0 */
            uint32_t reg_gpio20_int_clr             :  1; /* [   20],        w1p,        0x0 */
            uint32_t reg_gpio21_int_clr             :  1; /* [   21],        w1p,        0x0 */
            uint32_t reg_gpio22_int_clr             :  1; /* [   22],        w1p,        0x0 */
            uint32_t reg_gpio23_int_clr             :  1; /* [   23],        w1p,        0x0 */
            uint32_t reg_gpio24_int_clr             :  1; /* [   24],        w1p,        0x0 */
            uint32_t reg_gpio25_int_clr             :  1; /* [   25],        w1p,        0x0 */
            uint32_t reg_gpio26_int_clr             :  1; /* [   26],        w1p,        0x0 */
            uint32_t reg_gpio27_int_clr             :  1; /* [   27],        w1p,        0x0 */
            uint32_t reg_gpio28_int_clr             :  1; /* [   28],        w1p,        0x0 */
            uint32_t reg_gpio29_int_clr             :  1; /* [   29],        w1p,        0x0 */
            uint32_t reg_gpio30_int_clr             :  1; /* [   30],        w1p,        0x0 */
            uint32_t reg_gpio31_int_clr             :  1; /* [   31],        w1p,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_CLR1;

    /* 0x1B4 : GPIO_INT_MODE_SET1 */
    union {
        struct {
            uint32_t reg_gpio0_int_mode_set         :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio1_int_mode_set         :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio2_int_mode_set         :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio3_int_mode_set         :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_gpio4_int_mode_set         :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_gpio5_int_mode_set         :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_gpio6_int_mode_set         :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_gpio7_int_mode_set         :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET1;

    /* 0x1B8 : GPIO_INT_MODE_SET2 */
    union {
        struct {
            uint32_t reg_gpio8_int_mode_set         :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio9_int_mode_set         :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio10_int_mode_set        :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio11_int_mode_set        :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_gpio12_int_mode_set        :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_gpio13_int_mode_set        :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_gpio14_int_mode_set        :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_gpio15_int_mode_set        :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET2;

    /* 0x1BC : GPIO_INT_MODE_SET3 */
    union {
        struct {
            uint32_t reg_gpio16_int_mode_set        :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio17_int_mode_set        :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio18_int_mode_set        :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio19_int_mode_set        :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_gpio20_int_mode_set        :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_gpio21_int_mode_set        :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_gpio22_int_mode_set        :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_gpio23_int_mode_set        :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET3;

    /* 0x1C0 : GPIO_INT_MODE_SET4 */
    union {
        struct {
            uint32_t reg_gpio24_int_mode_set        :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio25_int_mode_set        :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio26_int_mode_set        :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio27_int_mode_set        :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_gpio28_int_mode_set        :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_gpio29_int_mode_set        :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_gpio30_int_mode_set        :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_gpio31_int_mode_set        :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET4;

    /* 0x1C4 : GPIO_INT_MODE_SET5 */
    union {
        struct {
            uint32_t reg_gpio32_int_mode_set        :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio33_int_mode_set        :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio34_int_mode_set        :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio35_int_mode_set        :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_gpio36_int_mode_set        :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_gpio37_int_mode_set        :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reg_gpio38_int_mode_set        :  4; /* [27:24],        r/w,        0x0 */
            uint32_t reg_gpio39_int_mode_set        :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET5;

    /* 0x1C8 : GPIO_INT_MODE_SET6 */
    union {
        struct {
            uint32_t reg_gpio40_int_mode_set        :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_gpio41_int_mode_set        :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_gpio42_int_mode_set        :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_gpio43_int_mode_set        :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } GPIO_INT_MODE_SET6;

    /* 0x1cc  reserved */
    uint8_t RESERVED0x1cc[88];

    /* 0x224 : led_driver */
    union {
        struct {
            uint32_t led_din_reg                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t led_din_sel                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t led_din_polarity_sel           :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t leddrv_ibias                   :  3; /* [ 6: 4],        r/w,        0x4 */
            uint32_t reserved_7_27                  : 21; /* [27: 7],       rsvd,        0x0 */
            uint32_t leddrv_out_en                  :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t pu_leddrv                      :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } led_driver;

    /* 0x228 : usb_xcvr */
    union {
        struct {
            uint32_t usb_ldo_vfb                    :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t pu_usb_ldo                     :  1; /* [    3],        r/w,        0x0 */
            uint32_t usb_rout_nmos                  :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t usb_rout_pmos                  :  3; /* [10: 8],        r/w,        0x3 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t usb_oeb_sel                    :  1; /* [   12],         rw,        0x0 */
            uint32_t usb_oeb_reg                    :  1; /* [   13],         rw,        0x1 */
            uint32_t usb_oeb                        :  1; /* [   14],          r,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t usb_data_convert               :  1; /* [   16],         rw,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t usb_enum                       :  1; /* [   20],        r/w,        0x0 */
            uint32_t usb_spd                        :  1; /* [   21],        r/w,        0x1 */
            uint32_t usb_sus                        :  1; /* [   22],        r/w,        0x0 */
            uint32_t pu_usb                         :  1; /* [   23],        r/w,        0x0 */
            uint32_t usb_bd                         :  1; /* [   24],          r,        0x0 */
            uint32_t usb_vim                        :  1; /* [   25],          r,        0x0 */
            uint32_t usb_vip                        :  1; /* [   26],          r,        0x0 */
            uint32_t usb_rcv                        :  1; /* [   27],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } usb_xcvr;

    /* 0x22C : usb_xcvr_config */
    union {
        struct {
            uint32_t usb_v_hys_m                    :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t usb_v_hys_p                    :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t usb_bd_vth                     :  3; /* [ 6: 4],        r/w,        0x1 */
            uint32_t reg_usb_use_xcvr               :  1; /* [    7],        r/w,        0x1 */
            uint32_t usb_str_drv                    :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reg_usb_use_ctrl               :  1; /* [   11],        r/w,        0x1 */
            uint32_t usb_res_pullup_tune            :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t usb_slewrate_m_fall            :  3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t usb_slewrate_m_rise            :  3; /* [22:20],        r/w,        0x4 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t usb_slewrate_p_fall            :  3; /* [26:24],        r/w,        0x3 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t usb_slewrate_p_rise            :  3; /* [30:28],        r/w,        0x4 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } usb_xcvr_config;

    /* 0x230  reserved */
    uint8_t RESERVED0x230[208];

    /* 0x300 : rc32m_ctrl0 */
    union {
        struct {
            uint32_t rc32m_cal_done                 :  1; /* [    0],          r,        0x0 */
            uint32_t rc32m_rdy                      :  1; /* [    1],          r,        0x0 */
            uint32_t rc32m_cal_inprogress           :  1; /* [    2],          r,        0x0 */
            uint32_t rc32m_cal_div                  :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32m_cal_precharge            :  1; /* [    5],          r,        0x0 */
            uint32_t rc32m_dig_code_fr_cal          :  8; /* [13: 6],          r,        0x0 */
            uint32_t reserved_14_16                 :  3; /* [16:14],       rsvd,        0x0 */
            uint32_t rc32m_allow_cal                :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t rc32m_ext_code_en              :  1; /* [   19],        r/w,        0x1 */
            uint32_t rc32m_cal_en                   :  1; /* [   20],        r/w,        0x0 */
            uint32_t rc32m_pd                       :  1; /* [   21],        r/w,        0x0 */
            uint32_t reserved_22_29                 :  8; /* [29:22],       rsvd,        0x0 */
            uint32_t rc32m_rc32k_cal_cgen           :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rc32m_ctrl0;

    /* 0x304 : rc32m_ctrl1 */
    union {
        struct {
            uint32_t rc32m_test_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t rc32m_code_fr_ext2             :  8; /* [15: 8],        r/w,       0x60 */
            uint32_t rc32m_ext_code_sel             :  1; /* [   16],        r/w,        0x0 */
            uint32_t rc32m_ext_code_sel_cgen        :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_23                 :  6; /* [23:18],       rsvd,        0x0 */
            uint32_t rc32m_reserved                 :  8; /* [31:24],        r/w,        0xf */
        }BF;
        uint32_t WORD;
    } rc32m_ctrl1;

    /* 0x308 : rc32k_ctrl2 */
    union {
        struct {
            uint32_t rc32k_dig_cal_done             :  1; /* [    0],          r,        0x1 */
            uint32_t rc32k_rdy                      :  1; /* [    1],          r,        0x1 */
            uint32_t rc32k_dig_cal_inprogress       :  1; /* [    2],          r,        0x0 */
            uint32_t rc32k_dig_cal_div              :  2; /* [ 4: 3],        r/w,        0x3 */
            uint32_t rc32k_dig_cal_precharge        :  1; /* [    5],          r,        0x0 */
            uint32_t rc32k_dig_cap_code_fr_cal      :  4; /* [ 9: 6],          r,        0x8 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t rc32k_dig_vref_dly             :  2; /* [17:16],        r/w,        0x0 */
            uint32_t rc32k_allow_cal                :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } rc32k_ctrl2;

    /* 0x30c  reserved */
    uint8_t RESERVED0x30c[292];

    /* 0x430 : sdh_cfg0 */
    union {
        struct {
            uint32_t reg_sdh_clk_div                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t reg_sdh_clk_sel                :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_sdh_clk_en                 :  1; /* [   13],        r/w,        0x1 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sdh_cfg0;

    /* 0x434 : smih_0_cfg0 */
    union {
        struct {
            uint32_t cr_smih_0_cd_n                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_smih_0_wp                   :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_15                  : 14; /* [15: 2],       rsvd,        0x0 */
            uint32_t st_smih_0_led_on               :  1; /* [   16],          r,        0x0 */
            uint32_t st_smih_0_rst_n                :  1; /* [   17],          r,        0x1 */
            uint32_t st_smih_0_wkup_on              :  1; /* [   18],          r,        0x0 */
            uint32_t st_smih_0_od_pp                :  1; /* [   19],          r,        0x0 */
            uint32_t st_smih_0_drive_strength       :  2; /* [21:20],          r,        0x1 */
            uint32_t st_smih_0_vol_sel              :  3; /* [24:22],          r,        0x0 */
            uint32_t st_smih_0_vol_on               :  1; /* [   25],          r,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } smih_0_cfg0;

    /* 0x438 : smih_0_io_dly_0 */
    union {
        struct {
            uint32_t cr_smih_0_clk_i_inv            :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_smih_0_clk_o_inv            :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_smih_0_clk_i_dly            :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t cr_smih_0_clk_o_dly            :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t cr_smih_0_cmd_i_dly            :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t cr_smih_0_cmd_o_dly            :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t cr_smih_0_cmd_e_dly            :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t cr_smih_0_d_0_i_dly            :  2; /* [17:16],        r/w,        0x0 */
            uint32_t cr_smih_0_d_1_i_dly            :  2; /* [19:18],        r/w,        0x0 */
            uint32_t cr_smih_0_d_2_i_dly            :  2; /* [21:20],        r/w,        0x0 */
            uint32_t cr_smih_0_d_3_i_dly            :  2; /* [23:22],        r/w,        0x0 */
            uint32_t cr_smih_0_d_4_i_dly            :  2; /* [25:24],        r/w,        0x0 */
            uint32_t cr_smih_0_d_5_i_dly            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t cr_smih_0_d_6_i_dly            :  2; /* [29:28],        r/w,        0x0 */
            uint32_t cr_smih_0_d_7_i_dly            :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } smih_0_io_dly_0;

    /* 0x43C : smih_0_io_dly_1 */
    union {
        struct {
            uint32_t cr_smih_0_d_0_o_dly            :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t cr_smih_0_d_1_o_dly            :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t cr_smih_0_d_2_o_dly            :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t cr_smih_0_d_3_o_dly            :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t cr_smih_0_d_4_o_dly            :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t cr_smih_0_d_5_o_dly            :  2; /* [11:10],        r/w,        0x0 */
            uint32_t cr_smih_0_d_6_o_dly            :  2; /* [13:12],        r/w,        0x0 */
            uint32_t cr_smih_0_d_7_o_dly            :  2; /* [15:14],        r/w,        0x0 */
            uint32_t cr_smih_0_d_0_e_dly            :  2; /* [17:16],        r/w,        0x0 */
            uint32_t cr_smih_0_d_1_e_dly            :  2; /* [19:18],        r/w,        0x0 */
            uint32_t cr_smih_0_d_2_e_dly            :  2; /* [21:20],        r/w,        0x0 */
            uint32_t cr_smih_0_d_3_e_dly            :  2; /* [23:22],        r/w,        0x0 */
            uint32_t cr_smih_0_d_4_e_dly            :  2; /* [25:24],        r/w,        0x0 */
            uint32_t cr_smih_0_d_5_e_dly            :  2; /* [27:26],        r/w,        0x0 */
            uint32_t cr_smih_0_d_6_e_dly            :  2; /* [29:28],        r/w,        0x0 */
            uint32_t cr_smih_0_d_7_e_dly            :  2; /* [31:30],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } smih_0_io_dly_1;

    /* 0x440  reserved */
    uint8_t RESERVED0x440[204];

    /* 0x50C : gpadc_reg_cmd */
    union {
        struct {
            uint32_t gpadc_global_en                :  1; /* [    0],        r/w,        0x0 */
            uint32_t gpadc_conv_start               :  1; /* [    1],        r/w,        0x0 */
            uint32_t gpadc_soft_rst                 :  1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc_neg_sel                  :  5; /* [ 7: 3],        r/w,        0xf */
            uint32_t gpadc_pos_sel                  :  5; /* [12: 8],        r/w,        0xf */
            uint32_t gpadc_neg_gnd                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t gpadc_micbias_en               :  1; /* [   14],        r/w,        0x0 */
            uint32_t gpadc_micpga_en                :  1; /* [   15],        r/w,        0x0 */
            uint32_t gpadc_byp_micboost             :  1; /* [   16],        r/w,        0x0 */
            uint32_t gpadc_rcal_en                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_dwa_en                   :  1; /* [   18],        r/w,        0x0 */
            uint32_t gpadc_mic2_diff                :  1; /* [   19],        r/w,        0x0 */
            uint32_t gpadc_mic1_diff                :  1; /* [   20],        r/w,        0x0 */
            uint32_t gpadc_mic_pga2_gain            :  2; /* [22:21],        r/w,        0x0 */
            uint32_t gpadc_micboost_32db_en         :  1; /* [   23],        r/w,        0x0 */
            uint32_t reserved_24_26                 :  3; /* [26:24],       rsvd,        0x0 */
            uint32_t gpadc_chip_sen_pu              :  1; /* [   27],        r/w,        0x0 */
            uint32_t gpadc_sen_sel                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t gpadc_sen_test_en              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_cmd;

    /* 0x510 : gpadc_reg_config1 */
    union {
        struct {
            uint32_t reserved_0                     :  1; /* [    0],       rsvd,        0x0 */
            uint32_t gpadc_cont_conv_en             :  1; /* [    1],        r/w,        0x1 */
            uint32_t gpadc_res_sel                  :  3; /* [ 4: 2],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t gpadc_vcm_sel_en               :  1; /* [    8],        r/w,        0x0 */
            uint32_t gpadc_vcm_hyst_sel             :  1; /* [    9],        r/w,        0x0 */
            uint32_t gpadc_lowv_det_en              :  1; /* [   10],        r/w,        0x0 */
            uint32_t gpadc_pwm_trg_en               :  1; /* [   11],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_dly              :  4; /* [15:12],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_dly_en           :  1; /* [   16],        r/w,        0x0 */
            uint32_t gpadc_clk_ana_inv              :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_clk_div_ratio            :  3; /* [20:18],        r/w,        0x3 */
            uint32_t gpadc_scan_length              :  4; /* [24:21],        r/w,        0x0 */
            uint32_t gpadc_scan_en                  :  1; /* [   25],        r/w,        0x0 */
            uint32_t gpadc_dither_en                :  1; /* [   26],        r/w,        0x0 */
            uint32_t gpadc_v11_sel                  :  2; /* [28:27],        r/w,        0x0 */
            uint32_t gpadc_v18_sel                  :  2; /* [30:29],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_config1;

    /* 0x514 : gpadc_reg_config2 */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t gpadc_diff_mode                :  1; /* [    2],        r/w,        0x0 */
            uint32_t gpadc_vref_sel                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t gpadc_vbat_en                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t gpadc_tsext_sel                :  1; /* [    5],        r/w,        0x0 */
            uint32_t gpadc_ts_en                    :  1; /* [    6],        r/w,        0x0 */
            uint32_t gpadc_pga_vcm                  :  2; /* [ 8: 7],        r/w,        0x2 */
            uint32_t gpadc_pga_os_cal               :  4; /* [12: 9],        r/w,        0x8 */
            uint32_t gpadc_pga_en                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t gpadc_pga_vcmi_en              :  1; /* [   14],        r/w,        0x0 */
            uint32_t gpadc_chop_mode                :  2; /* [16:15],        r/w,        0x3 */
            uint32_t gpadc_bias_sel                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t gpadc_test_en                  :  1; /* [   18],        r/w,        0x0 */
            uint32_t gpadc_test_sel                 :  3; /* [21:19],        r/w,        0x0 */
            uint32_t gpadc_pga2_gain                :  3; /* [24:22],        r/w,        0x0 */
            uint32_t gpadc_pga1_gain                :  3; /* [27:25],        r/w,        0x0 */
            uint32_t gpadc_dly_sel                  :  3; /* [30:28],        r/w,        0x0 */
            uint32_t gpadc_tsvbe_low                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_config2;

    /* 0x518 : adc converation sequence 1 */
    union {
        struct {
            uint32_t gpadc_scan_pos_0               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_pos_1               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_pos_2               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_pos_3               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_pos_4               :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_pos_5               :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_pos1;

    /* 0x51C : adc converation sequence 2 */
    union {
        struct {
            uint32_t gpadc_scan_pos_6               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_pos_7               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_pos_8               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_pos_9               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_pos_10              :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_pos_11              :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_pos2;

    /* 0x520 : adc converation sequence 3 */
    union {
        struct {
            uint32_t gpadc_scan_neg_0               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_neg_1               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_neg_2               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_neg_3               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_neg_4               :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_neg_5               :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_neg1;

    /* 0x524 : adc converation sequence 4 */
    union {
        struct {
            uint32_t gpadc_scan_neg_6               :  5; /* [ 4: 0],        r/w,        0xf */
            uint32_t gpadc_scan_neg_7               :  5; /* [ 9: 5],        r/w,        0xf */
            uint32_t gpadc_scan_neg_8               :  5; /* [14:10],        r/w,        0xf */
            uint32_t gpadc_scan_neg_9               :  5; /* [19:15],        r/w,        0xf */
            uint32_t gpadc_scan_neg_10              :  5; /* [24:20],        r/w,        0xf */
            uint32_t gpadc_scan_neg_11              :  5; /* [29:25],        r/w,        0xf */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_scn_neg2;

    /* 0x528 : gpadc_reg_status */
    union {
        struct {
            uint32_t gpadc_data_rdy                 :  1; /* [    0],          r,        0x0 */
            uint32_t reserved_1_15                  : 15; /* [15: 1],       rsvd,        0x0 */
            uint32_t gpadc_reserved                 : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_status;

    /* 0x52C : gpadc_reg_isr */
    union {
        struct {
            uint32_t gpadc_neg_satur                :  1; /* [    0],          r,        0x0 */
            uint32_t gpadc_pos_satur                :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t gpadc_neg_satur_clr            :  1; /* [    4],        r/w,        0x0 */
            uint32_t gpadc_pos_satur_clr            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t gpadc_neg_satur_mask           :  1; /* [    8],        r/w,        0x0 */
            uint32_t gpadc_pos_satur_mask           :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_isr;

    /* 0x530 : gpadc_reg_result */
    union {
        struct {
            uint32_t gpadc_data_out                 : 26; /* [25: 0],          r,  0x1ef0000 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_result;

    /* 0x534 : gpadc_reg_raw_result */
    union {
        struct {
            uint32_t gpadc_raw_data                 : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_raw_result;

    /* 0x538 : gpadc_reg_define */
    union {
        struct {
            uint32_t gpadc_os_cal_data              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpadc_reg_define;

    /* 0x53c  reserved */
    uint8_t RESERVED0x53c[204];

    /* 0x608 : gpdac_ctrl */
    union {
        struct {
            uint32_t gpdaca_rstn_ana                :  1; /* [    0],        r/w,        0x1 */
            uint32_t gpdacb_rstn_ana                :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_6                   :  5; /* [ 6: 2],       rsvd,        0x0 */
            uint32_t gpdac_test_en                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t gpdac_ref_sel                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t gpdac_test_sel                 :  3; /* [11: 9],        r/w,        0x0 */
            uint32_t reserved_12_23                 : 12; /* [23:12],       rsvd,        0x0 */
            uint32_t gpdac_reserved                 :  8; /* [31:24],        r/w,        0xf */
        }BF;
        uint32_t WORD;
    } gpdac_ctrl;

    /* 0x60C : gpdac_actrl */
    union {
        struct {
            uint32_t gpdac_a_en                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t gpdac_ioa_en                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_17                  : 16; /* [17: 2],       rsvd,        0x0 */
            uint32_t gpdac_a_rng                    :  2; /* [19:18],        r/w,        0x3 */
            uint32_t gpdac_a_outmux                 :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpdac_actrl;

    /* 0x610 : gpdac_bctrl */
    union {
        struct {
            uint32_t gpdac_b_en                     :  1; /* [    0],        r/w,        0x0 */
            uint32_t gpdac_iob_en                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_17                  : 16; /* [17: 2],       rsvd,        0x0 */
            uint32_t gpdac_b_rng                    :  2; /* [19:18],        r/w,        0x3 */
            uint32_t gpdac_b_outmux                 :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpdac_bctrl;

    /* 0x614 : gpdac_data */
    union {
        struct {
            uint32_t gpdac_b_data                   : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t gpdac_a_data                   : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } gpdac_data;

    /* 0x618  reserved */
    uint8_t RESERVED0x618[2280];

    /* 0xF00 : tzc_glb_ctrl_0 */
    union {
        struct {
            uint32_t tzc_glb_swrst_s00_lock         :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_swrst_s01_lock         :  1; /* [    1],          r,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t tzc_glb_swrst_s30_lock         :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t tzc_glb_ctrl_pwron_rst_lock    :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_ctrl_cpu_reset_lock    :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_ctrl_sys_reset_lock    :  1; /* [   14],          r,        0x0 */
            uint32_t reserved_15_24                 : 10; /* [24:15],       rsvd,        0x0 */
            uint32_t tzc_glb_misc_lock              :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_sram_lock              :  1; /* [   26],          r,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t tzc_glb_bmx_lock               :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_dbg_lock               :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_mbist_lock             :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_clk_lock               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_0;

    /* 0xF04 : tzc_glb_ctrl_1 */
    union {
        struct {
            uint32_t tzc_glb_swrst_s20_lock         :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_swrst_s21_lock         :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_swrst_s22_lock         :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_swrst_s23_lock         :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_swrst_s24_lock         :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_swrst_s25_lock         :  1; /* [    5],          r,        0x0 */
            uint32_t tzc_glb_swrst_s26_lock         :  1; /* [    6],          r,        0x0 */
            uint32_t tzc_glb_swrst_s27_lock         :  1; /* [    7],          r,        0x0 */
            uint32_t tzc_glb_swrst_s28_lock         :  1; /* [    8],          r,        0x0 */
            uint32_t tzc_glb_swrst_s29_lock         :  1; /* [    9],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2a_lock         :  1; /* [   10],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2b_lock         :  1; /* [   11],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2c_lock         :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2d_lock         :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2e_lock         :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_swrst_s2f_lock         :  1; /* [   15],          r,        0x0 */
            uint32_t tzc_glb_swrst_s10_lock         :  1; /* [   16],          r,        0x0 */
            uint32_t tzc_glb_swrst_s11_lock         :  1; /* [   17],          r,        0x0 */
            uint32_t tzc_glb_swrst_s12_lock         :  1; /* [   18],          r,        0x0 */
            uint32_t tzc_glb_swrst_s13_lock         :  1; /* [   19],          r,        0x0 */
            uint32_t tzc_glb_swrst_s14_lock         :  1; /* [   20],          r,        0x0 */
            uint32_t tzc_glb_swrst_s15_lock         :  1; /* [   21],          r,        0x0 */
            uint32_t tzc_glb_swrst_s16_lock         :  1; /* [   22],          r,        0x0 */
            uint32_t tzc_glb_swrst_s17_lock         :  1; /* [   23],          r,        0x0 */
            uint32_t tzc_glb_swrst_s18_lock         :  1; /* [   24],          r,        0x0 */
            uint32_t tzc_glb_swrst_s19_lock         :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1a_lock         :  1; /* [   26],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1b_lock         :  1; /* [   27],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1c_lock         :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1d_lock         :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1e_lock         :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_swrst_s1f_lock         :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_1;

    /* 0xF08 : tzc_glb_ctrl_2 */
    union {
        struct {
            uint32_t tzc_glb_gpio_0_lock            :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_gpio_1_lock            :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_gpio_2_lock            :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_gpio_3_lock            :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_gpio_4_lock            :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_gpio_5_lock            :  1; /* [    5],          r,        0x0 */
            uint32_t tzc_glb_gpio_6_lock            :  1; /* [    6],          r,        0x0 */
            uint32_t tzc_glb_gpio_7_lock            :  1; /* [    7],          r,        0x0 */
            uint32_t tzc_glb_gpio_8_lock            :  1; /* [    8],          r,        0x0 */
            uint32_t tzc_glb_gpio_9_lock            :  1; /* [    9],          r,        0x0 */
            uint32_t tzc_glb_gpio_10_lock           :  1; /* [   10],          r,        0x0 */
            uint32_t tzc_glb_gpio_11_lock           :  1; /* [   11],          r,        0x0 */
            uint32_t tzc_glb_gpio_12_lock           :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_gpio_13_lock           :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_gpio_14_lock           :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_gpio_15_lock           :  1; /* [   15],          r,        0x0 */
            uint32_t tzc_glb_gpio_16_lock           :  1; /* [   16],          r,        0x0 */
            uint32_t tzc_glb_gpio_17_lock           :  1; /* [   17],          r,        0x0 */
            uint32_t tzc_glb_gpio_18_lock           :  1; /* [   18],          r,        0x0 */
            uint32_t tzc_glb_gpio_19_lock           :  1; /* [   19],          r,        0x0 */
            uint32_t tzc_glb_gpio_20_lock           :  1; /* [   20],          r,        0x0 */
            uint32_t tzc_glb_gpio_21_lock           :  1; /* [   21],          r,        0x0 */
            uint32_t tzc_glb_gpio_22_lock           :  1; /* [   22],          r,        0x0 */
            uint32_t tzc_glb_gpio_23_lock           :  1; /* [   23],          r,        0x0 */
            uint32_t tzc_glb_gpio_24_lock           :  1; /* [   24],          r,        0x0 */
            uint32_t tzc_glb_gpio_25_lock           :  1; /* [   25],          r,        0x0 */
            uint32_t tzc_glb_gpio_26_lock           :  1; /* [   26],          r,        0x0 */
            uint32_t tzc_glb_gpio_27_lock           :  1; /* [   27],          r,        0x0 */
            uint32_t tzc_glb_gpio_28_lock           :  1; /* [   28],          r,        0x0 */
            uint32_t tzc_glb_gpio_29_lock           :  1; /* [   29],          r,        0x0 */
            uint32_t tzc_glb_gpio_30_lock           :  1; /* [   30],          r,        0x0 */
            uint32_t tzc_glb_gpio_31_lock           :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_2;

    /* 0xF0C : tzc_glb_ctrl_3 */
    union {
        struct {
            uint32_t tzc_glb_gpio_32_lock           :  1; /* [    0],          r,        0x0 */
            uint32_t tzc_glb_gpio_33_lock           :  1; /* [    1],          r,        0x0 */
            uint32_t tzc_glb_gpio_34_lock           :  1; /* [    2],          r,        0x0 */
            uint32_t tzc_glb_gpio_35_lock           :  1; /* [    3],          r,        0x0 */
            uint32_t tzc_glb_gpio_36_lock           :  1; /* [    4],          r,        0x0 */
            uint32_t tzc_glb_gpio_37_lock           :  1; /* [    5],          r,        0x0 */
            uint32_t tzc_glb_gpio_38_lock           :  1; /* [    6],          r,        0x0 */
            uint32_t tzc_glb_gpio_39_lock           :  1; /* [    7],          r,        0x0 */
            uint32_t tzc_glb_gpio_40_lock           :  1; /* [    8],          r,        0x0 */
            uint32_t tzc_glb_gpio_41_lock           :  1; /* [    9],          r,        0x0 */
            uint32_t tzc_glb_gpio_42_lock           :  1; /* [   10],          r,        0x0 */
            uint32_t tzc_glb_gpio_43_lock           :  1; /* [   11],          r,        0x0 */
            uint32_t tzc_glb_gpio_44_lock           :  1; /* [   12],          r,        0x0 */
            uint32_t tzc_glb_gpio_45_lock           :  1; /* [   13],          r,        0x0 */
            uint32_t tzc_glb_gpio_46_lock           :  1; /* [   14],          r,        0x0 */
            uint32_t tzc_glb_gpio_47_lock           :  1; /* [   15],          r,        0x0 */
            uint32_t tzc_glb_gpio_48_lock           :  1; /* [   16],          r,        0x0 */
            uint32_t tzc_glb_gpio_49_lock           :  1; /* [   17],          r,        0x0 */
            uint32_t tzc_glb_gpio_50_lock           :  1; /* [   18],          r,        0x0 */
            uint32_t tzc_glb_gpio_51_lock           :  1; /* [   19],          r,        0x0 */
            uint32_t tzc_glb_gpio_52_lock           :  1; /* [   20],          r,        0x0 */
            uint32_t tzc_glb_gpio_53_lock           :  1; /* [   21],          r,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } tzc_glb_ctrl_3;

};

typedef volatile struct glb_reg glb_reg_t;


#endif  /* __GLB_REG_H__ */
