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

#ifndef  __M154_REG_H__
#define  __M154_REG_H__

#include "ipro6.h"

/* 0x0 : m154_glb_cfg */
#define M154_GLB_CFG_OFFSET                                     (0x0)
#define M154_CLK_EN                                             M154_CLK_EN
#define M154_CLK_EN_POS                                         (0U)
#define M154_CLK_EN_LEN                                         (1U)
#define M154_CLK_EN_MSK                                         (((1ULL<<M154_CLK_EN_LEN)-1)<<M154_CLK_EN_POS)
#define M154_CLK_EN_UMSK                                        (~(((1ULL<<M154_CLK_EN_LEN)-1)<<M154_CLK_EN_POS))
#define M154_BEACON_MODE_EN                                     M154_BEACON_MODE_EN
#define M154_BEACON_MODE_EN_POS                                 (1U)
#define M154_BEACON_MODE_EN_LEN                                 (1U)
#define M154_BEACON_MODE_EN_MSK                                 (((1ULL<<M154_BEACON_MODE_EN_LEN)-1)<<M154_BEACON_MODE_EN_POS)
#define M154_BEACON_MODE_EN_UMSK                                (~(((1ULL<<M154_BEACON_MODE_EN_LEN)-1)<<M154_BEACON_MODE_EN_POS))
#define M154_ZBEN_PROTECT                                       M154_ZBEN_PROTECT
#define M154_ZBEN_PROTECT_POS                                   (2U)
#define M154_ZBEN_PROTECT_LEN                                   (1U)
#define M154_ZBEN_PROTECT_MSK                                   (((1ULL<<M154_ZBEN_PROTECT_LEN)-1)<<M154_ZBEN_PROTECT_POS)
#define M154_ZBEN_PROTECT_UMSK                                  (~(((1ULL<<M154_ZBEN_PROTECT_LEN)-1)<<M154_ZBEN_PROTECT_POS))
#define M154_CLK_XTAL_FREQ                                      M154_CLK_XTAL_FREQ
#define M154_CLK_XTAL_FREQ_POS                                  (4U)
#define M154_CLK_XTAL_FREQ_LEN                                  (6U)
#define M154_CLK_XTAL_FREQ_MSK                                  (((1ULL<<M154_CLK_XTAL_FREQ_LEN)-1)<<M154_CLK_XTAL_FREQ_POS)
#define M154_CLK_XTAL_FREQ_UMSK                                 (~(((1ULL<<M154_CLK_XTAL_FREQ_LEN)-1)<<M154_CLK_XTAL_FREQ_POS))
#define M154_STD_VERSION                                        M154_STD_VERSION
#define M154_STD_VERSION_POS                                    (12U)
#define M154_STD_VERSION_LEN                                    (2U)
#define M154_STD_VERSION_MSK                                    (((1ULL<<M154_STD_VERSION_LEN)-1)<<M154_STD_VERSION_POS)
#define M154_STD_VERSION_UMSK                                   (~(((1ULL<<M154_STD_VERSION_LEN)-1)<<M154_STD_VERSION_POS))
#define M154_RF_CENTER_FREQ_REG                                 M154_RF_CENTER_FREQ_REG
#define M154_RF_CENTER_FREQ_REG_POS                             (16U)
#define M154_RF_CENTER_FREQ_REG_LEN                             (12U)
#define M154_RF_CENTER_FREQ_REG_MSK                             (((1ULL<<M154_RF_CENTER_FREQ_REG_LEN)-1)<<M154_RF_CENTER_FREQ_REG_POS)
#define M154_RF_CENTER_FREQ_REG_UMSK                            (~(((1ULL<<M154_RF_CENTER_FREQ_REG_LEN)-1)<<M154_RF_CENTER_FREQ_REG_POS))

/* 0x4 : m154_data_cfg */
#define M154_DATA_CFG_OFFSET                                    (0x4)
#define M154_RX_SYM_BIT_REVERSE_EN                              M154_RX_SYM_BIT_REVERSE_EN
#define M154_RX_SYM_BIT_REVERSE_EN_POS                          (0U)
#define M154_RX_SYM_BIT_REVERSE_EN_LEN                          (1U)
#define M154_RX_SYM_BIT_REVERSE_EN_MSK                          (((1ULL<<M154_RX_SYM_BIT_REVERSE_EN_LEN)-1)<<M154_RX_SYM_BIT_REVERSE_EN_POS)
#define M154_RX_SYM_BIT_REVERSE_EN_UMSK                         (~(((1ULL<<M154_RX_SYM_BIT_REVERSE_EN_LEN)-1)<<M154_RX_SYM_BIT_REVERSE_EN_POS))
#define M154_TX_SYM_BIT_REVERSE_EN                              M154_TX_SYM_BIT_REVERSE_EN
#define M154_TX_SYM_BIT_REVERSE_EN_POS                          (1U)
#define M154_TX_SYM_BIT_REVERSE_EN_LEN                          (1U)
#define M154_TX_SYM_BIT_REVERSE_EN_MSK                          (((1ULL<<M154_TX_SYM_BIT_REVERSE_EN_LEN)-1)<<M154_TX_SYM_BIT_REVERSE_EN_POS)
#define M154_TX_SYM_BIT_REVERSE_EN_UMSK                         (~(((1ULL<<M154_TX_SYM_BIT_REVERSE_EN_LEN)-1)<<M154_TX_SYM_BIT_REVERSE_EN_POS))
#define M154_RX_CRC_FAULT_EN                                    M154_RX_CRC_FAULT_EN
#define M154_RX_CRC_FAULT_EN_POS                                (2U)
#define M154_RX_CRC_FAULT_EN_LEN                                (1U)
#define M154_RX_CRC_FAULT_EN_MSK                                (((1ULL<<M154_RX_CRC_FAULT_EN_LEN)-1)<<M154_RX_CRC_FAULT_EN_POS)
#define M154_RX_CRC_FAULT_EN_UMSK                               (~(((1ULL<<M154_RX_CRC_FAULT_EN_LEN)-1)<<M154_RX_CRC_FAULT_EN_POS))
#define M154_TX_CRC_FAULT_EN                                    M154_TX_CRC_FAULT_EN
#define M154_TX_CRC_FAULT_EN_POS                                (3U)
#define M154_TX_CRC_FAULT_EN_LEN                                (1U)
#define M154_TX_CRC_FAULT_EN_MSK                                (((1ULL<<M154_TX_CRC_FAULT_EN_LEN)-1)<<M154_TX_CRC_FAULT_EN_POS)
#define M154_TX_CRC_FAULT_EN_UMSK                               (~(((1ULL<<M154_TX_CRC_FAULT_EN_LEN)-1)<<M154_TX_CRC_FAULT_EN_POS))
#define M154_RX_DATA_FAULT_EN                                   M154_RX_DATA_FAULT_EN
#define M154_RX_DATA_FAULT_EN_POS                               (4U)
#define M154_RX_DATA_FAULT_EN_LEN                               (1U)
#define M154_RX_DATA_FAULT_EN_MSK                               (((1ULL<<M154_RX_DATA_FAULT_EN_LEN)-1)<<M154_RX_DATA_FAULT_EN_POS)
#define M154_RX_DATA_FAULT_EN_UMSK                              (~(((1ULL<<M154_RX_DATA_FAULT_EN_LEN)-1)<<M154_RX_DATA_FAULT_EN_POS))
#define M154_TX_DATA_FAULT_EN                                   M154_TX_DATA_FAULT_EN
#define M154_TX_DATA_FAULT_EN_POS                               (5U)
#define M154_TX_DATA_FAULT_EN_LEN                               (1U)
#define M154_TX_DATA_FAULT_EN_MSK                               (((1ULL<<M154_TX_DATA_FAULT_EN_LEN)-1)<<M154_TX_DATA_FAULT_EN_POS)
#define M154_TX_DATA_FAULT_EN_UMSK                              (~(((1ULL<<M154_TX_DATA_FAULT_EN_LEN)-1)<<M154_TX_DATA_FAULT_EN_POS))

/* 0x8 : m154_swrst */
#define M154_SWRST_OFFSET                                       (0x8)
#define M154_SOFT_RESET_REGS_CMD                                M154_SOFT_RESET_REGS_CMD
#define M154_SOFT_RESET_REGS_CMD_POS                            (0U)
#define M154_SOFT_RESET_REGS_CMD_LEN                            (1U)
#define M154_SOFT_RESET_REGS_CMD_MSK                            (((1ULL<<M154_SOFT_RESET_REGS_CMD_LEN)-1)<<M154_SOFT_RESET_REGS_CMD_POS)
#define M154_SOFT_RESET_REGS_CMD_UMSK                           (~(((1ULL<<M154_SOFT_RESET_REGS_CMD_LEN)-1)<<M154_SOFT_RESET_REGS_CMD_POS))
#define M154_SOFT_RESET_TCON_CMD                                M154_SOFT_RESET_TCON_CMD
#define M154_SOFT_RESET_TCON_CMD_POS                            (1U)
#define M154_SOFT_RESET_TCON_CMD_LEN                            (1U)
#define M154_SOFT_RESET_TCON_CMD_MSK                            (((1ULL<<M154_SOFT_RESET_TCON_CMD_LEN)-1)<<M154_SOFT_RESET_TCON_CMD_POS)
#define M154_SOFT_RESET_TCON_CMD_UMSK                           (~(((1ULL<<M154_SOFT_RESET_TCON_CMD_LEN)-1)<<M154_SOFT_RESET_TCON_CMD_POS))
#define M154_SOFT_RESET_RX_CMD                                  M154_SOFT_RESET_RX_CMD
#define M154_SOFT_RESET_RX_CMD_POS                              (2U)
#define M154_SOFT_RESET_RX_CMD_LEN                              (1U)
#define M154_SOFT_RESET_RX_CMD_MSK                              (((1ULL<<M154_SOFT_RESET_RX_CMD_LEN)-1)<<M154_SOFT_RESET_RX_CMD_POS)
#define M154_SOFT_RESET_RX_CMD_UMSK                             (~(((1ULL<<M154_SOFT_RESET_RX_CMD_LEN)-1)<<M154_SOFT_RESET_RX_CMD_POS))
#define M154_SOFT_RESET_TX_CMD                                  M154_SOFT_RESET_TX_CMD
#define M154_SOFT_RESET_TX_CMD_POS                              (3U)
#define M154_SOFT_RESET_TX_CMD_LEN                              (1U)
#define M154_SOFT_RESET_TX_CMD_MSK                              (((1ULL<<M154_SOFT_RESET_TX_CMD_LEN)-1)<<M154_SOFT_RESET_TX_CMD_POS)
#define M154_SOFT_RESET_TX_CMD_UMSK                             (~(((1ULL<<M154_SOFT_RESET_TX_CMD_LEN)-1)<<M154_SOFT_RESET_TX_CMD_POS))
#define M154_SOFT_RESET_CSMA_CA_CMD                             M154_SOFT_RESET_CSMA_CA_CMD
#define M154_SOFT_RESET_CSMA_CA_CMD_POS                         (4U)
#define M154_SOFT_RESET_CSMA_CA_CMD_LEN                         (1U)
#define M154_SOFT_RESET_CSMA_CA_CMD_MSK                         (((1ULL<<M154_SOFT_RESET_CSMA_CA_CMD_LEN)-1)<<M154_SOFT_RESET_CSMA_CA_CMD_POS)
#define M154_SOFT_RESET_CSMA_CA_CMD_UMSK                        (~(((1ULL<<M154_SOFT_RESET_CSMA_CA_CMD_LEN)-1)<<M154_SOFT_RESET_CSMA_CA_CMD_POS))
#define M154_SOFT_RESET_BIST_CMD                                M154_SOFT_RESET_BIST_CMD
#define M154_SOFT_RESET_BIST_CMD_POS                            (5U)
#define M154_SOFT_RESET_BIST_CMD_LEN                            (1U)
#define M154_SOFT_RESET_BIST_CMD_MSK                            (((1ULL<<M154_SOFT_RESET_BIST_CMD_LEN)-1)<<M154_SOFT_RESET_BIST_CMD_POS)
#define M154_SOFT_RESET_BIST_CMD_UMSK                           (~(((1ULL<<M154_SOFT_RESET_BIST_CMD_LEN)-1)<<M154_SOFT_RESET_BIST_CMD_POS))
#define M154_RX_CRC_SW_CLR                                      M154_RX_CRC_SW_CLR
#define M154_RX_CRC_SW_CLR_POS                                  (6U)
#define M154_RX_CRC_SW_CLR_LEN                                  (1U)
#define M154_RX_CRC_SW_CLR_MSK                                  (((1ULL<<M154_RX_CRC_SW_CLR_LEN)-1)<<M154_RX_CRC_SW_CLR_POS)
#define M154_RX_CRC_SW_CLR_UMSK                                 (~(((1ULL<<M154_RX_CRC_SW_CLR_LEN)-1)<<M154_RX_CRC_SW_CLR_POS))
#define M154_TX_CRC_SW_CLR                                      M154_TX_CRC_SW_CLR
#define M154_TX_CRC_SW_CLR_POS                                  (7U)
#define M154_TX_CRC_SW_CLR_LEN                                  (1U)
#define M154_TX_CRC_SW_CLR_MSK                                  (((1ULL<<M154_TX_CRC_SW_CLR_LEN)-1)<<M154_TX_CRC_SW_CLR_POS)
#define M154_TX_CRC_SW_CLR_UMSK                                 (~(((1ULL<<M154_TX_CRC_SW_CLR_LEN)-1)<<M154_TX_CRC_SW_CLR_POS))

/* 0xC : m154_dbg_sel */
#define M154_DBG_SEL_OFFSET                                     (0xC)
#define M154_DEBUG_OUT_SEL                                      M154_DEBUG_OUT_SEL
#define M154_DEBUG_OUT_SEL_POS                                  (0U)
#define M154_DEBUG_OUT_SEL_LEN                                  (4U)
#define M154_DEBUG_OUT_SEL_MSK                                  (((1ULL<<M154_DEBUG_OUT_SEL_LEN)-1)<<M154_DEBUG_OUT_SEL_POS)
#define M154_DEBUG_OUT_SEL_UMSK                                 (~(((1ULL<<M154_DEBUG_OUT_SEL_LEN)-1)<<M154_DEBUG_OUT_SEL_POS))

/* 0x10 : m154_dbg_int_en */
#define M154_DBG_INT_EN_OFFSET                                  (0x10)
#define M154_DEBUG_INT_RAW_EN                                   M154_DEBUG_INT_RAW_EN
#define M154_DEBUG_INT_RAW_EN_POS                               (0U)
#define M154_DEBUG_INT_RAW_EN_LEN                               (32U)
#define M154_DEBUG_INT_RAW_EN_MSK                               (((1ULL<<M154_DEBUG_INT_RAW_EN_LEN)-1)<<M154_DEBUG_INT_RAW_EN_POS)
#define M154_DEBUG_INT_RAW_EN_UMSK                              (~(((1ULL<<M154_DEBUG_INT_RAW_EN_LEN)-1)<<M154_DEBUG_INT_RAW_EN_POS))

/* 0x14 : m154_bist_ctrl */
#define M154_BIST_CTRL_OFFSET                                   (0x14)
#define M154_BIST_PATTERN_CMD                                   M154_BIST_PATTERN_CMD
#define M154_BIST_PATTERN_CMD_POS                               (0U)
#define M154_BIST_PATTERN_CMD_LEN                               (1U)
#define M154_BIST_PATTERN_CMD_MSK                               (((1ULL<<M154_BIST_PATTERN_CMD_LEN)-1)<<M154_BIST_PATTERN_CMD_POS)
#define M154_BIST_PATTERN_CMD_UMSK                              (~(((1ULL<<M154_BIST_PATTERN_CMD_LEN)-1)<<M154_BIST_PATTERN_CMD_POS))
#define M154_BIST_PATTERN_BP_BOUND_EN                           M154_BIST_PATTERN_BP_BOUND_EN
#define M154_BIST_PATTERN_BP_BOUND_EN_POS                       (1U)
#define M154_BIST_PATTERN_BP_BOUND_EN_LEN                       (1U)
#define M154_BIST_PATTERN_BP_BOUND_EN_MSK                       (((1ULL<<M154_BIST_PATTERN_BP_BOUND_EN_LEN)-1)<<M154_BIST_PATTERN_BP_BOUND_EN_POS)
#define M154_BIST_PATTERN_BP_BOUND_EN_UMSK                      (~(((1ULL<<M154_BIST_PATTERN_BP_BOUND_EN_LEN)-1)<<M154_BIST_PATTERN_BP_BOUND_EN_POS))
#define M154_BIST_PATTERN_AR_SEL                                M154_BIST_PATTERN_AR_SEL
#define M154_BIST_PATTERN_AR_SEL_POS                            (2U)
#define M154_BIST_PATTERN_AR_SEL_LEN                            (1U)
#define M154_BIST_PATTERN_AR_SEL_MSK                            (((1ULL<<M154_BIST_PATTERN_AR_SEL_LEN)-1)<<M154_BIST_PATTERN_AR_SEL_POS)
#define M154_BIST_PATTERN_AR_SEL_UMSK                           (~(((1ULL<<M154_BIST_PATTERN_AR_SEL_LEN)-1)<<M154_BIST_PATTERN_AR_SEL_POS))
#define M154_BIST_AUTO_ACK_EN                                   M154_BIST_AUTO_ACK_EN
#define M154_BIST_AUTO_ACK_EN_POS                               (3U)
#define M154_BIST_AUTO_ACK_EN_LEN                               (1U)
#define M154_BIST_AUTO_ACK_EN_MSK                               (((1ULL<<M154_BIST_AUTO_ACK_EN_LEN)-1)<<M154_BIST_AUTO_ACK_EN_POS)
#define M154_BIST_AUTO_ACK_EN_UMSK                              (~(((1ULL<<M154_BIST_AUTO_ACK_EN_LEN)-1)<<M154_BIST_AUTO_ACK_EN_POS))
#define M154_BIST_PERIODIC_BEACON_EN                            M154_BIST_PERIODIC_BEACON_EN
#define M154_BIST_PERIODIC_BEACON_EN_POS                        (4U)
#define M154_BIST_PERIODIC_BEACON_EN_LEN                        (1U)
#define M154_BIST_PERIODIC_BEACON_EN_MSK                        (((1ULL<<M154_BIST_PERIODIC_BEACON_EN_LEN)-1)<<M154_BIST_PERIODIC_BEACON_EN_POS)
#define M154_BIST_PERIODIC_BEACON_EN_UMSK                       (~(((1ULL<<M154_BIST_PERIODIC_BEACON_EN_LEN)-1)<<M154_BIST_PERIODIC_BEACON_EN_POS))
#define M154_BIST_PERIODIC_BEACON_SEL                           M154_BIST_PERIODIC_BEACON_SEL
#define M154_BIST_PERIODIC_BEACON_SEL_POS                       (5U)
#define M154_BIST_PERIODIC_BEACON_SEL_LEN                       (1U)
#define M154_BIST_PERIODIC_BEACON_SEL_MSK                       (((1ULL<<M154_BIST_PERIODIC_BEACON_SEL_LEN)-1)<<M154_BIST_PERIODIC_BEACON_SEL_POS)
#define M154_BIST_PERIODIC_BEACON_SEL_UMSK                      (~(((1ULL<<M154_BIST_PERIODIC_BEACON_SEL_LEN)-1)<<M154_BIST_PERIODIC_BEACON_SEL_POS))
#define M154_BIST_EXT_DUT_EN                                    M154_BIST_EXT_DUT_EN
#define M154_BIST_EXT_DUT_EN_POS                                (6U)
#define M154_BIST_EXT_DUT_EN_LEN                                (1U)
#define M154_BIST_EXT_DUT_EN_MSK                                (((1ULL<<M154_BIST_EXT_DUT_EN_LEN)-1)<<M154_BIST_EXT_DUT_EN_POS)
#define M154_BIST_EXT_DUT_EN_UMSK                               (~(((1ULL<<M154_BIST_EXT_DUT_EN_LEN)-1)<<M154_BIST_EXT_DUT_EN_POS))
#define M154_BIST_EN                                            M154_BIST_EN
#define M154_BIST_EN_POS                                        (7U)
#define M154_BIST_EN_LEN                                        (1U)
#define M154_BIST_EN_MSK                                        (((1ULL<<M154_BIST_EN_LEN)-1)<<M154_BIST_EN_POS)
#define M154_BIST_EN_UMSK                                       (~(((1ULL<<M154_BIST_EN_LEN)-1)<<M154_BIST_EN_POS))
#define M154_BIST_PERIODIC_BEACON_ORDER                         M154_BIST_PERIODIC_BEACON_ORDER
#define M154_BIST_PERIODIC_BEACON_ORDER_POS                     (8U)
#define M154_BIST_PERIODIC_BEACON_ORDER_LEN                     (4U)
#define M154_BIST_PERIODIC_BEACON_ORDER_MSK                     (((1ULL<<M154_BIST_PERIODIC_BEACON_ORDER_LEN)-1)<<M154_BIST_PERIODIC_BEACON_ORDER_POS)
#define M154_BIST_PERIODIC_BEACON_ORDER_UMSK                    (~(((1ULL<<M154_BIST_PERIODIC_BEACON_ORDER_LEN)-1)<<M154_BIST_PERIODIC_BEACON_ORDER_POS))
#define M154_BIST_PATTERN_SEL                                   M154_BIST_PATTERN_SEL
#define M154_BIST_PATTERN_SEL_POS                               (12U)
#define M154_BIST_PATTERN_SEL_LEN                               (4U)
#define M154_BIST_PATTERN_SEL_MSK                               (((1ULL<<M154_BIST_PATTERN_SEL_LEN)-1)<<M154_BIST_PATTERN_SEL_POS)
#define M154_BIST_PATTERN_SEL_UMSK                              (~(((1ULL<<M154_BIST_PATTERN_SEL_LEN)-1)<<M154_BIST_PATTERN_SEL_POS))
#define M154_BIST_DOING                                         M154_BIST_DOING
#define M154_BIST_DOING_POS                                     (16U)
#define M154_BIST_DOING_LEN                                     (1U)
#define M154_BIST_DOING_MSK                                     (((1ULL<<M154_BIST_DOING_LEN)-1)<<M154_BIST_DOING_POS)
#define M154_BIST_DOING_UMSK                                    (~(((1ULL<<M154_BIST_DOING_LEN)-1)<<M154_BIST_DOING_POS))
#define M154_BIST_AUTO_ACK_DOING                                M154_BIST_AUTO_ACK_DOING
#define M154_BIST_AUTO_ACK_DOING_POS                            (17U)
#define M154_BIST_AUTO_ACK_DOING_LEN                            (1U)
#define M154_BIST_AUTO_ACK_DOING_MSK                            (((1ULL<<M154_BIST_AUTO_ACK_DOING_LEN)-1)<<M154_BIST_AUTO_ACK_DOING_POS)
#define M154_BIST_AUTO_ACK_DOING_UMSK                           (~(((1ULL<<M154_BIST_AUTO_ACK_DOING_LEN)-1)<<M154_BIST_AUTO_ACK_DOING_POS))

/* 0x18 : m154_bist_dly */
#define M154_BIST_DLY_OFFSET                                    (0x18)
#define M154_BIST_PATTERN_TRIG_TIME                             M154_BIST_PATTERN_TRIG_TIME
#define M154_BIST_PATTERN_TRIG_TIME_POS                         (0U)
#define M154_BIST_PATTERN_TRIG_TIME_LEN                         (32U)
#define M154_BIST_PATTERN_TRIG_TIME_MSK                         (((1ULL<<M154_BIST_PATTERN_TRIG_TIME_LEN)-1)<<M154_BIST_PATTERN_TRIG_TIME_POS)
#define M154_BIST_PATTERN_TRIG_TIME_UMSK                        (~(((1ULL<<M154_BIST_PATTERN_TRIG_TIME_LEN)-1)<<M154_BIST_PATTERN_TRIG_TIME_POS))

/* 0x20 : m154_int_en */
#define M154_INT_EN_OFFSET                                      (0x20)
#define M154_ALARM_00_INT_EN                                    M154_ALARM_00_INT_EN
#define M154_ALARM_00_INT_EN_POS                                (0U)
#define M154_ALARM_00_INT_EN_LEN                                (1U)
#define M154_ALARM_00_INT_EN_MSK                                (((1ULL<<M154_ALARM_00_INT_EN_LEN)-1)<<M154_ALARM_00_INT_EN_POS)
#define M154_ALARM_00_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_00_INT_EN_LEN)-1)<<M154_ALARM_00_INT_EN_POS))
#define M154_ALARM_01_INT_EN                                    M154_ALARM_01_INT_EN
#define M154_ALARM_01_INT_EN_POS                                (1U)
#define M154_ALARM_01_INT_EN_LEN                                (1U)
#define M154_ALARM_01_INT_EN_MSK                                (((1ULL<<M154_ALARM_01_INT_EN_LEN)-1)<<M154_ALARM_01_INT_EN_POS)
#define M154_ALARM_01_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_01_INT_EN_LEN)-1)<<M154_ALARM_01_INT_EN_POS))
#define M154_ALARM_02_INT_EN                                    M154_ALARM_02_INT_EN
#define M154_ALARM_02_INT_EN_POS                                (2U)
#define M154_ALARM_02_INT_EN_LEN                                (1U)
#define M154_ALARM_02_INT_EN_MSK                                (((1ULL<<M154_ALARM_02_INT_EN_LEN)-1)<<M154_ALARM_02_INT_EN_POS)
#define M154_ALARM_02_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_02_INT_EN_LEN)-1)<<M154_ALARM_02_INT_EN_POS))
#define M154_ALARM_03_INT_EN                                    M154_ALARM_03_INT_EN
#define M154_ALARM_03_INT_EN_POS                                (3U)
#define M154_ALARM_03_INT_EN_LEN                                (1U)
#define M154_ALARM_03_INT_EN_MSK                                (((1ULL<<M154_ALARM_03_INT_EN_LEN)-1)<<M154_ALARM_03_INT_EN_POS)
#define M154_ALARM_03_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_03_INT_EN_LEN)-1)<<M154_ALARM_03_INT_EN_POS))
#define M154_ALARM_04_INT_EN                                    M154_ALARM_04_INT_EN
#define M154_ALARM_04_INT_EN_POS                                (4U)
#define M154_ALARM_04_INT_EN_LEN                                (1U)
#define M154_ALARM_04_INT_EN_MSK                                (((1ULL<<M154_ALARM_04_INT_EN_LEN)-1)<<M154_ALARM_04_INT_EN_POS)
#define M154_ALARM_04_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_04_INT_EN_LEN)-1)<<M154_ALARM_04_INT_EN_POS))
#define M154_ALARM_05_INT_EN                                    M154_ALARM_05_INT_EN
#define M154_ALARM_05_INT_EN_POS                                (5U)
#define M154_ALARM_05_INT_EN_LEN                                (1U)
#define M154_ALARM_05_INT_EN_MSK                                (((1ULL<<M154_ALARM_05_INT_EN_LEN)-1)<<M154_ALARM_05_INT_EN_POS)
#define M154_ALARM_05_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_05_INT_EN_LEN)-1)<<M154_ALARM_05_INT_EN_POS))
#define M154_ALARM_06_INT_EN                                    M154_ALARM_06_INT_EN
#define M154_ALARM_06_INT_EN_POS                                (6U)
#define M154_ALARM_06_INT_EN_LEN                                (1U)
#define M154_ALARM_06_INT_EN_MSK                                (((1ULL<<M154_ALARM_06_INT_EN_LEN)-1)<<M154_ALARM_06_INT_EN_POS)
#define M154_ALARM_06_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_06_INT_EN_LEN)-1)<<M154_ALARM_06_INT_EN_POS))
#define M154_ALARM_07_INT_EN                                    M154_ALARM_07_INT_EN
#define M154_ALARM_07_INT_EN_POS                                (7U)
#define M154_ALARM_07_INT_EN_LEN                                (1U)
#define M154_ALARM_07_INT_EN_MSK                                (((1ULL<<M154_ALARM_07_INT_EN_LEN)-1)<<M154_ALARM_07_INT_EN_POS)
#define M154_ALARM_07_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_07_INT_EN_LEN)-1)<<M154_ALARM_07_INT_EN_POS))
#define M154_ALARM_08_INT_EN                                    M154_ALARM_08_INT_EN
#define M154_ALARM_08_INT_EN_POS                                (8U)
#define M154_ALARM_08_INT_EN_LEN                                (1U)
#define M154_ALARM_08_INT_EN_MSK                                (((1ULL<<M154_ALARM_08_INT_EN_LEN)-1)<<M154_ALARM_08_INT_EN_POS)
#define M154_ALARM_08_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_08_INT_EN_LEN)-1)<<M154_ALARM_08_INT_EN_POS))
#define M154_ALARM_09_INT_EN                                    M154_ALARM_09_INT_EN
#define M154_ALARM_09_INT_EN_POS                                (9U)
#define M154_ALARM_09_INT_EN_LEN                                (1U)
#define M154_ALARM_09_INT_EN_MSK                                (((1ULL<<M154_ALARM_09_INT_EN_LEN)-1)<<M154_ALARM_09_INT_EN_POS)
#define M154_ALARM_09_INT_EN_UMSK                               (~(((1ULL<<M154_ALARM_09_INT_EN_LEN)-1)<<M154_ALARM_09_INT_EN_POS))
#define M154_RX_END_FRAK_INT_EN                                 M154_RX_END_FRAK_INT_EN
#define M154_RX_END_FRAK_INT_EN_POS                             (10U)
#define M154_RX_END_FRAK_INT_EN_LEN                             (1U)
#define M154_RX_END_FRAK_INT_EN_MSK                             (((1ULL<<M154_RX_END_FRAK_INT_EN_LEN)-1)<<M154_RX_END_FRAK_INT_EN_POS)
#define M154_RX_END_FRAK_INT_EN_UMSK                            (~(((1ULL<<M154_RX_END_FRAK_INT_EN_LEN)-1)<<M154_RX_END_FRAK_INT_EN_POS))
#define M154_RX_END_REQ_ENH_ACK_INT_EN                          M154_RX_END_REQ_ENH_ACK_INT_EN
#define M154_RX_END_REQ_ENH_ACK_INT_EN_POS                      (11U)
#define M154_RX_END_REQ_ENH_ACK_INT_EN_LEN                      (1U)
#define M154_RX_END_REQ_ENH_ACK_INT_EN_MSK                      (((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_EN_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_EN_POS)
#define M154_RX_END_REQ_ENH_ACK_INT_EN_UMSK                     (~(((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_EN_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_EN_POS))
#define M154_RX_END_EXT_INT_EN                                  M154_RX_END_EXT_INT_EN
#define M154_RX_END_EXT_INT_EN_POS                              (12U)
#define M154_RX_END_EXT_INT_EN_LEN                              (1U)
#define M154_RX_END_EXT_INT_EN_MSK                              (((1ULL<<M154_RX_END_EXT_INT_EN_LEN)-1)<<M154_RX_END_EXT_INT_EN_POS)
#define M154_RX_END_EXT_INT_EN_UMSK                             (~(((1ULL<<M154_RX_END_EXT_INT_EN_LEN)-1)<<M154_RX_END_EXT_INT_EN_POS))
#define M154_RX_END_MPP_INT_EN                                  M154_RX_END_MPP_INT_EN
#define M154_RX_END_MPP_INT_EN_POS                              (13U)
#define M154_RX_END_MPP_INT_EN_LEN                              (1U)
#define M154_RX_END_MPP_INT_EN_MSK                              (((1ULL<<M154_RX_END_MPP_INT_EN_LEN)-1)<<M154_RX_END_MPP_INT_EN_POS)
#define M154_RX_END_MPP_INT_EN_UMSK                             (~(((1ULL<<M154_RX_END_MPP_INT_EN_LEN)-1)<<M154_RX_END_MPP_INT_EN_POS))
#define M154_RX_NBR_SEARCH_FAIL_INT_EN                          M154_RX_NBR_SEARCH_FAIL_INT_EN
#define M154_RX_NBR_SEARCH_FAIL_INT_EN_POS                      (14U)
#define M154_RX_NBR_SEARCH_FAIL_INT_EN_LEN                      (1U)
#define M154_RX_NBR_SEARCH_FAIL_INT_EN_MSK                      (((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_EN_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_EN_POS)
#define M154_RX_NBR_SEARCH_FAIL_INT_EN_UMSK                     (~(((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_EN_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_EN_POS))
#define M154_CS_ALERT_INT_EN                                    M154_CS_ALERT_INT_EN
#define M154_CS_ALERT_INT_EN_POS                                (15U)
#define M154_CS_ALERT_INT_EN_LEN                                (1U)
#define M154_CS_ALERT_INT_EN_MSK                                (((1ULL<<M154_CS_ALERT_INT_EN_LEN)-1)<<M154_CS_ALERT_INT_EN_POS)
#define M154_CS_ALERT_INT_EN_UMSK                               (~(((1ULL<<M154_CS_ALERT_INT_EN_LEN)-1)<<M154_CS_ALERT_INT_EN_POS))
#define M154_RX_START_INT_EN                                    M154_RX_START_INT_EN
#define M154_RX_START_INT_EN_POS                                (16U)
#define M154_RX_START_INT_EN_LEN                                (1U)
#define M154_RX_START_INT_EN_MSK                                (((1ULL<<M154_RX_START_INT_EN_LEN)-1)<<M154_RX_START_INT_EN_POS)
#define M154_RX_START_INT_EN_UMSK                               (~(((1ULL<<M154_RX_START_INT_EN_LEN)-1)<<M154_RX_START_INT_EN_POS))
#define M154_RX_MHR_INT_EN                                      M154_RX_MHR_INT_EN
#define M154_RX_MHR_INT_EN_POS                                  (17U)
#define M154_RX_MHR_INT_EN_LEN                                  (1U)
#define M154_RX_MHR_INT_EN_MSK                                  (((1ULL<<M154_RX_MHR_INT_EN_LEN)-1)<<M154_RX_MHR_INT_EN_POS)
#define M154_RX_MHR_INT_EN_UMSK                                 (~(((1ULL<<M154_RX_MHR_INT_EN_LEN)-1)<<M154_RX_MHR_INT_EN_POS))
#define M154_RX_MHR_SECURITY_INT_EN                             M154_RX_MHR_SECURITY_INT_EN
#define M154_RX_MHR_SECURITY_INT_EN_POS                         (18U)
#define M154_RX_MHR_SECURITY_INT_EN_LEN                         (1U)
#define M154_RX_MHR_SECURITY_INT_EN_MSK                         (((1ULL<<M154_RX_MHR_SECURITY_INT_EN_LEN)-1)<<M154_RX_MHR_SECURITY_INT_EN_POS)
#define M154_RX_MHR_SECURITY_INT_EN_UMSK                        (~(((1ULL<<M154_RX_MHR_SECURITY_INT_EN_LEN)-1)<<M154_RX_MHR_SECURITY_INT_EN_POS))
#define M154_RX_FILTER_FAIL_INT_EN                              M154_RX_FILTER_FAIL_INT_EN
#define M154_RX_FILTER_FAIL_INT_EN_POS                          (19U)
#define M154_RX_FILTER_FAIL_INT_EN_LEN                          (1U)
#define M154_RX_FILTER_FAIL_INT_EN_MSK                          (((1ULL<<M154_RX_FILTER_FAIL_INT_EN_LEN)-1)<<M154_RX_FILTER_FAIL_INT_EN_POS)
#define M154_RX_FILTER_FAIL_INT_EN_UMSK                         (~(((1ULL<<M154_RX_FILTER_FAIL_INT_EN_LEN)-1)<<M154_RX_FILTER_FAIL_INT_EN_POS))
#define M154_RX_END_BEACON_INT_EN                               M154_RX_END_BEACON_INT_EN
#define M154_RX_END_BEACON_INT_EN_POS                           (20U)
#define M154_RX_END_BEACON_INT_EN_LEN                           (1U)
#define M154_RX_END_BEACON_INT_EN_MSK                           (((1ULL<<M154_RX_END_BEACON_INT_EN_LEN)-1)<<M154_RX_END_BEACON_INT_EN_POS)
#define M154_RX_END_BEACON_INT_EN_UMSK                          (~(((1ULL<<M154_RX_END_BEACON_INT_EN_LEN)-1)<<M154_RX_END_BEACON_INT_EN_POS))
#define M154_RX_END_DATA_INT_EN                                 M154_RX_END_DATA_INT_EN
#define M154_RX_END_DATA_INT_EN_POS                             (21U)
#define M154_RX_END_DATA_INT_EN_LEN                             (1U)
#define M154_RX_END_DATA_INT_EN_MSK                             (((1ULL<<M154_RX_END_DATA_INT_EN_LEN)-1)<<M154_RX_END_DATA_INT_EN_POS)
#define M154_RX_END_DATA_INT_EN_UMSK                            (~(((1ULL<<M154_RX_END_DATA_INT_EN_LEN)-1)<<M154_RX_END_DATA_INT_EN_POS))
#define M154_RX_END_ACK_INT_EN                                  M154_RX_END_ACK_INT_EN
#define M154_RX_END_ACK_INT_EN_POS                              (22U)
#define M154_RX_END_ACK_INT_EN_LEN                              (1U)
#define M154_RX_END_ACK_INT_EN_MSK                              (((1ULL<<M154_RX_END_ACK_INT_EN_LEN)-1)<<M154_RX_END_ACK_INT_EN_POS)
#define M154_RX_END_ACK_INT_EN_UMSK                             (~(((1ULL<<M154_RX_END_ACK_INT_EN_LEN)-1)<<M154_RX_END_ACK_INT_EN_POS))
#define M154_RX_END_CMD_INT_EN                                  M154_RX_END_CMD_INT_EN
#define M154_RX_END_CMD_INT_EN_POS                              (23U)
#define M154_RX_END_CMD_INT_EN_LEN                              (1U)
#define M154_RX_END_CMD_INT_EN_MSK                              (((1ULL<<M154_RX_END_CMD_INT_EN_LEN)-1)<<M154_RX_END_CMD_INT_EN_POS)
#define M154_RX_END_CMD_INT_EN_UMSK                             (~(((1ULL<<M154_RX_END_CMD_INT_EN_LEN)-1)<<M154_RX_END_CMD_INT_EN_POS))
#define M154_RX_END_ACK_OTHERS_INT_EN                           M154_RX_END_ACK_OTHERS_INT_EN
#define M154_RX_END_ACK_OTHERS_INT_EN_POS                       (24U)
#define M154_RX_END_ACK_OTHERS_INT_EN_LEN                       (1U)
#define M154_RX_END_ACK_OTHERS_INT_EN_MSK                       (((1ULL<<M154_RX_END_ACK_OTHERS_INT_EN_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_EN_POS)
#define M154_RX_END_ACK_OTHERS_INT_EN_UMSK                      (~(((1ULL<<M154_RX_END_ACK_OTHERS_INT_EN_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_EN_POS))
#define M154_CSMA_CA_SUCCESS_INT_EN                             M154_CSMA_CA_SUCCESS_INT_EN
#define M154_CSMA_CA_SUCCESS_INT_EN_POS                         (25U)
#define M154_CSMA_CA_SUCCESS_INT_EN_LEN                         (1U)
#define M154_CSMA_CA_SUCCESS_INT_EN_MSK                         (((1ULL<<M154_CSMA_CA_SUCCESS_INT_EN_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_EN_POS)
#define M154_CSMA_CA_SUCCESS_INT_EN_UMSK                        (~(((1ULL<<M154_CSMA_CA_SUCCESS_INT_EN_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_EN_POS))
#define M154_CSMA_CA_FAILURE_INT_EN                             M154_CSMA_CA_FAILURE_INT_EN
#define M154_CSMA_CA_FAILURE_INT_EN_POS                         (26U)
#define M154_CSMA_CA_FAILURE_INT_EN_LEN                         (1U)
#define M154_CSMA_CA_FAILURE_INT_EN_MSK                         (((1ULL<<M154_CSMA_CA_FAILURE_INT_EN_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_EN_POS)
#define M154_CSMA_CA_FAILURE_INT_EN_UMSK                        (~(((1ULL<<M154_CSMA_CA_FAILURE_INT_EN_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_EN_POS))
#define M154_CSMA_CA_TIMEOUT_INT_EN                             M154_CSMA_CA_TIMEOUT_INT_EN
#define M154_CSMA_CA_TIMEOUT_INT_EN_POS                         (27U)
#define M154_CSMA_CA_TIMEOUT_INT_EN_LEN                         (1U)
#define M154_CSMA_CA_TIMEOUT_INT_EN_MSK                         (((1ULL<<M154_CSMA_CA_TIMEOUT_INT_EN_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_EN_POS)
#define M154_CSMA_CA_TIMEOUT_INT_EN_UMSK                        (~(((1ULL<<M154_CSMA_CA_TIMEOUT_INT_EN_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_EN_POS))
#define M154_TX_DONE_INT_EN                                     M154_TX_DONE_INT_EN
#define M154_TX_DONE_INT_EN_POS                                 (28U)
#define M154_TX_DONE_INT_EN_LEN                                 (1U)
#define M154_TX_DONE_INT_EN_MSK                                 (((1ULL<<M154_TX_DONE_INT_EN_LEN)-1)<<M154_TX_DONE_INT_EN_POS)
#define M154_TX_DONE_INT_EN_UMSK                                (~(((1ULL<<M154_TX_DONE_INT_EN_LEN)-1)<<M154_TX_DONE_INT_EN_POS))
#define M154_TX_AUTO_ACK_DONE_INT_EN                            M154_TX_AUTO_ACK_DONE_INT_EN
#define M154_TX_AUTO_ACK_DONE_INT_EN_POS                        (29U)
#define M154_TX_AUTO_ACK_DONE_INT_EN_LEN                        (1U)
#define M154_TX_AUTO_ACK_DONE_INT_EN_MSK                        (((1ULL<<M154_TX_AUTO_ACK_DONE_INT_EN_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_EN_POS)
#define M154_TX_AUTO_ACK_DONE_INT_EN_UMSK                       (~(((1ULL<<M154_TX_AUTO_ACK_DONE_INT_EN_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_EN_POS))
#define M154_RX_ABORT_INT_EN                                    M154_RX_ABORT_INT_EN
#define M154_RX_ABORT_INT_EN_POS                                (30U)
#define M154_RX_ABORT_INT_EN_LEN                                (1U)
#define M154_RX_ABORT_INT_EN_MSK                                (((1ULL<<M154_RX_ABORT_INT_EN_LEN)-1)<<M154_RX_ABORT_INT_EN_POS)
#define M154_RX_ABORT_INT_EN_UMSK                               (~(((1ULL<<M154_RX_ABORT_INT_EN_LEN)-1)<<M154_RX_ABORT_INT_EN_POS))
#define M154_TX_ABORT_INT_EN                                    M154_TX_ABORT_INT_EN
#define M154_TX_ABORT_INT_EN_POS                                (31U)
#define M154_TX_ABORT_INT_EN_LEN                                (1U)
#define M154_TX_ABORT_INT_EN_MSK                                (((1ULL<<M154_TX_ABORT_INT_EN_LEN)-1)<<M154_TX_ABORT_INT_EN_POS)
#define M154_TX_ABORT_INT_EN_UMSK                               (~(((1ULL<<M154_TX_ABORT_INT_EN_LEN)-1)<<M154_TX_ABORT_INT_EN_POS))

/* 0x24 : m154_int_clr */
#define M154_INT_CLR_OFFSET                                     (0x24)
#define M154_ALARM_00_INT_CLR                                   M154_ALARM_00_INT_CLR
#define M154_ALARM_00_INT_CLR_POS                               (0U)
#define M154_ALARM_00_INT_CLR_LEN                               (1U)
#define M154_ALARM_00_INT_CLR_MSK                               (((1ULL<<M154_ALARM_00_INT_CLR_LEN)-1)<<M154_ALARM_00_INT_CLR_POS)
#define M154_ALARM_00_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_00_INT_CLR_LEN)-1)<<M154_ALARM_00_INT_CLR_POS))
#define M154_ALARM_01_INT_CLR                                   M154_ALARM_01_INT_CLR
#define M154_ALARM_01_INT_CLR_POS                               (1U)
#define M154_ALARM_01_INT_CLR_LEN                               (1U)
#define M154_ALARM_01_INT_CLR_MSK                               (((1ULL<<M154_ALARM_01_INT_CLR_LEN)-1)<<M154_ALARM_01_INT_CLR_POS)
#define M154_ALARM_01_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_01_INT_CLR_LEN)-1)<<M154_ALARM_01_INT_CLR_POS))
#define M154_ALARM_02_INT_CLR                                   M154_ALARM_02_INT_CLR
#define M154_ALARM_02_INT_CLR_POS                               (2U)
#define M154_ALARM_02_INT_CLR_LEN                               (1U)
#define M154_ALARM_02_INT_CLR_MSK                               (((1ULL<<M154_ALARM_02_INT_CLR_LEN)-1)<<M154_ALARM_02_INT_CLR_POS)
#define M154_ALARM_02_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_02_INT_CLR_LEN)-1)<<M154_ALARM_02_INT_CLR_POS))
#define M154_ALARM_03_INT_CLR                                   M154_ALARM_03_INT_CLR
#define M154_ALARM_03_INT_CLR_POS                               (3U)
#define M154_ALARM_03_INT_CLR_LEN                               (1U)
#define M154_ALARM_03_INT_CLR_MSK                               (((1ULL<<M154_ALARM_03_INT_CLR_LEN)-1)<<M154_ALARM_03_INT_CLR_POS)
#define M154_ALARM_03_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_03_INT_CLR_LEN)-1)<<M154_ALARM_03_INT_CLR_POS))
#define M154_ALARM_04_INT_CLR                                   M154_ALARM_04_INT_CLR
#define M154_ALARM_04_INT_CLR_POS                               (4U)
#define M154_ALARM_04_INT_CLR_LEN                               (1U)
#define M154_ALARM_04_INT_CLR_MSK                               (((1ULL<<M154_ALARM_04_INT_CLR_LEN)-1)<<M154_ALARM_04_INT_CLR_POS)
#define M154_ALARM_04_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_04_INT_CLR_LEN)-1)<<M154_ALARM_04_INT_CLR_POS))
#define M154_ALARM_05_INT_CLR                                   M154_ALARM_05_INT_CLR
#define M154_ALARM_05_INT_CLR_POS                               (5U)
#define M154_ALARM_05_INT_CLR_LEN                               (1U)
#define M154_ALARM_05_INT_CLR_MSK                               (((1ULL<<M154_ALARM_05_INT_CLR_LEN)-1)<<M154_ALARM_05_INT_CLR_POS)
#define M154_ALARM_05_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_05_INT_CLR_LEN)-1)<<M154_ALARM_05_INT_CLR_POS))
#define M154_ALARM_06_INT_CLR                                   M154_ALARM_06_INT_CLR
#define M154_ALARM_06_INT_CLR_POS                               (6U)
#define M154_ALARM_06_INT_CLR_LEN                               (1U)
#define M154_ALARM_06_INT_CLR_MSK                               (((1ULL<<M154_ALARM_06_INT_CLR_LEN)-1)<<M154_ALARM_06_INT_CLR_POS)
#define M154_ALARM_06_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_06_INT_CLR_LEN)-1)<<M154_ALARM_06_INT_CLR_POS))
#define M154_ALARM_07_INT_CLR                                   M154_ALARM_07_INT_CLR
#define M154_ALARM_07_INT_CLR_POS                               (7U)
#define M154_ALARM_07_INT_CLR_LEN                               (1U)
#define M154_ALARM_07_INT_CLR_MSK                               (((1ULL<<M154_ALARM_07_INT_CLR_LEN)-1)<<M154_ALARM_07_INT_CLR_POS)
#define M154_ALARM_07_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_07_INT_CLR_LEN)-1)<<M154_ALARM_07_INT_CLR_POS))
#define M154_ALARM_08_INT_CLR                                   M154_ALARM_08_INT_CLR
#define M154_ALARM_08_INT_CLR_POS                               (8U)
#define M154_ALARM_08_INT_CLR_LEN                               (1U)
#define M154_ALARM_08_INT_CLR_MSK                               (((1ULL<<M154_ALARM_08_INT_CLR_LEN)-1)<<M154_ALARM_08_INT_CLR_POS)
#define M154_ALARM_08_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_08_INT_CLR_LEN)-1)<<M154_ALARM_08_INT_CLR_POS))
#define M154_ALARM_09_INT_CLR                                   M154_ALARM_09_INT_CLR
#define M154_ALARM_09_INT_CLR_POS                               (9U)
#define M154_ALARM_09_INT_CLR_LEN                               (1U)
#define M154_ALARM_09_INT_CLR_MSK                               (((1ULL<<M154_ALARM_09_INT_CLR_LEN)-1)<<M154_ALARM_09_INT_CLR_POS)
#define M154_ALARM_09_INT_CLR_UMSK                              (~(((1ULL<<M154_ALARM_09_INT_CLR_LEN)-1)<<M154_ALARM_09_INT_CLR_POS))
#define M154_RX_END_FRAK_INT_CLR                                M154_RX_END_FRAK_INT_CLR
#define M154_RX_END_FRAK_INT_CLR_POS                            (10U)
#define M154_RX_END_FRAK_INT_CLR_LEN                            (1U)
#define M154_RX_END_FRAK_INT_CLR_MSK                            (((1ULL<<M154_RX_END_FRAK_INT_CLR_LEN)-1)<<M154_RX_END_FRAK_INT_CLR_POS)
#define M154_RX_END_FRAK_INT_CLR_UMSK                           (~(((1ULL<<M154_RX_END_FRAK_INT_CLR_LEN)-1)<<M154_RX_END_FRAK_INT_CLR_POS))
#define M154_RX_END_REQ_ENH_ACK_INT_CLR                         M154_RX_END_REQ_ENH_ACK_INT_CLR
#define M154_RX_END_REQ_ENH_ACK_INT_CLR_POS                     (11U)
#define M154_RX_END_REQ_ENH_ACK_INT_CLR_LEN                     (1U)
#define M154_RX_END_REQ_ENH_ACK_INT_CLR_MSK                     (((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_CLR_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_CLR_POS)
#define M154_RX_END_REQ_ENH_ACK_INT_CLR_UMSK                    (~(((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_CLR_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_CLR_POS))
#define M154_RX_END_EXT_INT_CLR                                 M154_RX_END_EXT_INT_CLR
#define M154_RX_END_EXT_INT_CLR_POS                             (12U)
#define M154_RX_END_EXT_INT_CLR_LEN                             (1U)
#define M154_RX_END_EXT_INT_CLR_MSK                             (((1ULL<<M154_RX_END_EXT_INT_CLR_LEN)-1)<<M154_RX_END_EXT_INT_CLR_POS)
#define M154_RX_END_EXT_INT_CLR_UMSK                            (~(((1ULL<<M154_RX_END_EXT_INT_CLR_LEN)-1)<<M154_RX_END_EXT_INT_CLR_POS))
#define M154_RX_END_MPP_INT_CLR                                 M154_RX_END_MPP_INT_CLR
#define M154_RX_END_MPP_INT_CLR_POS                             (13U)
#define M154_RX_END_MPP_INT_CLR_LEN                             (1U)
#define M154_RX_END_MPP_INT_CLR_MSK                             (((1ULL<<M154_RX_END_MPP_INT_CLR_LEN)-1)<<M154_RX_END_MPP_INT_CLR_POS)
#define M154_RX_END_MPP_INT_CLR_UMSK                            (~(((1ULL<<M154_RX_END_MPP_INT_CLR_LEN)-1)<<M154_RX_END_MPP_INT_CLR_POS))
#define M154_RX_NBR_SEARCH_FAIL_INT_CLR                         M154_RX_NBR_SEARCH_FAIL_INT_CLR
#define M154_RX_NBR_SEARCH_FAIL_INT_CLR_POS                     (14U)
#define M154_RX_NBR_SEARCH_FAIL_INT_CLR_LEN                     (1U)
#define M154_RX_NBR_SEARCH_FAIL_INT_CLR_MSK                     (((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_CLR_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_CLR_POS)
#define M154_RX_NBR_SEARCH_FAIL_INT_CLR_UMSK                    (~(((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_CLR_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_CLR_POS))
#define M154_CS_ALERT_INT_CLR                                   M154_CS_ALERT_INT_CLR
#define M154_CS_ALERT_INT_CLR_POS                               (15U)
#define M154_CS_ALERT_INT_CLR_LEN                               (1U)
#define M154_CS_ALERT_INT_CLR_MSK                               (((1ULL<<M154_CS_ALERT_INT_CLR_LEN)-1)<<M154_CS_ALERT_INT_CLR_POS)
#define M154_CS_ALERT_INT_CLR_UMSK                              (~(((1ULL<<M154_CS_ALERT_INT_CLR_LEN)-1)<<M154_CS_ALERT_INT_CLR_POS))
#define M154_RX_START_INT_CLR                                   M154_RX_START_INT_CLR
#define M154_RX_START_INT_CLR_POS                               (16U)
#define M154_RX_START_INT_CLR_LEN                               (1U)
#define M154_RX_START_INT_CLR_MSK                               (((1ULL<<M154_RX_START_INT_CLR_LEN)-1)<<M154_RX_START_INT_CLR_POS)
#define M154_RX_START_INT_CLR_UMSK                              (~(((1ULL<<M154_RX_START_INT_CLR_LEN)-1)<<M154_RX_START_INT_CLR_POS))
#define M154_RX_MHR_INT_CLR                                     M154_RX_MHR_INT_CLR
#define M154_RX_MHR_INT_CLR_POS                                 (17U)
#define M154_RX_MHR_INT_CLR_LEN                                 (1U)
#define M154_RX_MHR_INT_CLR_MSK                                 (((1ULL<<M154_RX_MHR_INT_CLR_LEN)-1)<<M154_RX_MHR_INT_CLR_POS)
#define M154_RX_MHR_INT_CLR_UMSK                                (~(((1ULL<<M154_RX_MHR_INT_CLR_LEN)-1)<<M154_RX_MHR_INT_CLR_POS))
#define M154_RX_MHR_SECURITY_INT_CLR                            M154_RX_MHR_SECURITY_INT_CLR
#define M154_RX_MHR_SECURITY_INT_CLR_POS                        (18U)
#define M154_RX_MHR_SECURITY_INT_CLR_LEN                        (1U)
#define M154_RX_MHR_SECURITY_INT_CLR_MSK                        (((1ULL<<M154_RX_MHR_SECURITY_INT_CLR_LEN)-1)<<M154_RX_MHR_SECURITY_INT_CLR_POS)
#define M154_RX_MHR_SECURITY_INT_CLR_UMSK                       (~(((1ULL<<M154_RX_MHR_SECURITY_INT_CLR_LEN)-1)<<M154_RX_MHR_SECURITY_INT_CLR_POS))
#define M154_RX_FILTER_FAIL_INT_CLR                             M154_RX_FILTER_FAIL_INT_CLR
#define M154_RX_FILTER_FAIL_INT_CLR_POS                         (19U)
#define M154_RX_FILTER_FAIL_INT_CLR_LEN                         (1U)
#define M154_RX_FILTER_FAIL_INT_CLR_MSK                         (((1ULL<<M154_RX_FILTER_FAIL_INT_CLR_LEN)-1)<<M154_RX_FILTER_FAIL_INT_CLR_POS)
#define M154_RX_FILTER_FAIL_INT_CLR_UMSK                        (~(((1ULL<<M154_RX_FILTER_FAIL_INT_CLR_LEN)-1)<<M154_RX_FILTER_FAIL_INT_CLR_POS))
#define M154_RX_END_BEACON_INT_CLR                              M154_RX_END_BEACON_INT_CLR
#define M154_RX_END_BEACON_INT_CLR_POS                          (20U)
#define M154_RX_END_BEACON_INT_CLR_LEN                          (1U)
#define M154_RX_END_BEACON_INT_CLR_MSK                          (((1ULL<<M154_RX_END_BEACON_INT_CLR_LEN)-1)<<M154_RX_END_BEACON_INT_CLR_POS)
#define M154_RX_END_BEACON_INT_CLR_UMSK                         (~(((1ULL<<M154_RX_END_BEACON_INT_CLR_LEN)-1)<<M154_RX_END_BEACON_INT_CLR_POS))
#define M154_RX_END_DATA_INT_CLR                                M154_RX_END_DATA_INT_CLR
#define M154_RX_END_DATA_INT_CLR_POS                            (21U)
#define M154_RX_END_DATA_INT_CLR_LEN                            (1U)
#define M154_RX_END_DATA_INT_CLR_MSK                            (((1ULL<<M154_RX_END_DATA_INT_CLR_LEN)-1)<<M154_RX_END_DATA_INT_CLR_POS)
#define M154_RX_END_DATA_INT_CLR_UMSK                           (~(((1ULL<<M154_RX_END_DATA_INT_CLR_LEN)-1)<<M154_RX_END_DATA_INT_CLR_POS))
#define M154_RX_END_ACK_INT_CLR                                 M154_RX_END_ACK_INT_CLR
#define M154_RX_END_ACK_INT_CLR_POS                             (22U)
#define M154_RX_END_ACK_INT_CLR_LEN                             (1U)
#define M154_RX_END_ACK_INT_CLR_MSK                             (((1ULL<<M154_RX_END_ACK_INT_CLR_LEN)-1)<<M154_RX_END_ACK_INT_CLR_POS)
#define M154_RX_END_ACK_INT_CLR_UMSK                            (~(((1ULL<<M154_RX_END_ACK_INT_CLR_LEN)-1)<<M154_RX_END_ACK_INT_CLR_POS))
#define M154_RX_END_CMD_INT_CLR                                 M154_RX_END_CMD_INT_CLR
#define M154_RX_END_CMD_INT_CLR_POS                             (23U)
#define M154_RX_END_CMD_INT_CLR_LEN                             (1U)
#define M154_RX_END_CMD_INT_CLR_MSK                             (((1ULL<<M154_RX_END_CMD_INT_CLR_LEN)-1)<<M154_RX_END_CMD_INT_CLR_POS)
#define M154_RX_END_CMD_INT_CLR_UMSK                            (~(((1ULL<<M154_RX_END_CMD_INT_CLR_LEN)-1)<<M154_RX_END_CMD_INT_CLR_POS))
#define M154_RX_END_ACK_OTHERS_INT_CLR                          M154_RX_END_ACK_OTHERS_INT_CLR
#define M154_RX_END_ACK_OTHERS_INT_CLR_POS                      (24U)
#define M154_RX_END_ACK_OTHERS_INT_CLR_LEN                      (1U)
#define M154_RX_END_ACK_OTHERS_INT_CLR_MSK                      (((1ULL<<M154_RX_END_ACK_OTHERS_INT_CLR_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_CLR_POS)
#define M154_RX_END_ACK_OTHERS_INT_CLR_UMSK                     (~(((1ULL<<M154_RX_END_ACK_OTHERS_INT_CLR_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_CLR_POS))
#define M154_CSMA_CA_SUCCESS_INT_CLR                            M154_CSMA_CA_SUCCESS_INT_CLR
#define M154_CSMA_CA_SUCCESS_INT_CLR_POS                        (25U)
#define M154_CSMA_CA_SUCCESS_INT_CLR_LEN                        (1U)
#define M154_CSMA_CA_SUCCESS_INT_CLR_MSK                        (((1ULL<<M154_CSMA_CA_SUCCESS_INT_CLR_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_CLR_POS)
#define M154_CSMA_CA_SUCCESS_INT_CLR_UMSK                       (~(((1ULL<<M154_CSMA_CA_SUCCESS_INT_CLR_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_CLR_POS))
#define M154_CSMA_CA_FAILURE_INT_CLR                            M154_CSMA_CA_FAILURE_INT_CLR
#define M154_CSMA_CA_FAILURE_INT_CLR_POS                        (26U)
#define M154_CSMA_CA_FAILURE_INT_CLR_LEN                        (1U)
#define M154_CSMA_CA_FAILURE_INT_CLR_MSK                        (((1ULL<<M154_CSMA_CA_FAILURE_INT_CLR_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_CLR_POS)
#define M154_CSMA_CA_FAILURE_INT_CLR_UMSK                       (~(((1ULL<<M154_CSMA_CA_FAILURE_INT_CLR_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_CLR_POS))
#define M154_CSMA_CA_TIMEOUT_INT_CLR                            M154_CSMA_CA_TIMEOUT_INT_CLR
#define M154_CSMA_CA_TIMEOUT_INT_CLR_POS                        (27U)
#define M154_CSMA_CA_TIMEOUT_INT_CLR_LEN                        (1U)
#define M154_CSMA_CA_TIMEOUT_INT_CLR_MSK                        (((1ULL<<M154_CSMA_CA_TIMEOUT_INT_CLR_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_CLR_POS)
#define M154_CSMA_CA_TIMEOUT_INT_CLR_UMSK                       (~(((1ULL<<M154_CSMA_CA_TIMEOUT_INT_CLR_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_CLR_POS))
#define M154_TX_DONE_INT_CLR                                    M154_TX_DONE_INT_CLR
#define M154_TX_DONE_INT_CLR_POS                                (28U)
#define M154_TX_DONE_INT_CLR_LEN                                (1U)
#define M154_TX_DONE_INT_CLR_MSK                                (((1ULL<<M154_TX_DONE_INT_CLR_LEN)-1)<<M154_TX_DONE_INT_CLR_POS)
#define M154_TX_DONE_INT_CLR_UMSK                               (~(((1ULL<<M154_TX_DONE_INT_CLR_LEN)-1)<<M154_TX_DONE_INT_CLR_POS))
#define M154_TX_AUTO_ACK_DONE_INT_CLR                           M154_TX_AUTO_ACK_DONE_INT_CLR
#define M154_TX_AUTO_ACK_DONE_INT_CLR_POS                       (29U)
#define M154_TX_AUTO_ACK_DONE_INT_CLR_LEN                       (1U)
#define M154_TX_AUTO_ACK_DONE_INT_CLR_MSK                       (((1ULL<<M154_TX_AUTO_ACK_DONE_INT_CLR_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_CLR_POS)
#define M154_TX_AUTO_ACK_DONE_INT_CLR_UMSK                      (~(((1ULL<<M154_TX_AUTO_ACK_DONE_INT_CLR_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_CLR_POS))
#define M154_RX_ABORT_INT_CLR                                   M154_RX_ABORT_INT_CLR
#define M154_RX_ABORT_INT_CLR_POS                               (30U)
#define M154_RX_ABORT_INT_CLR_LEN                               (1U)
#define M154_RX_ABORT_INT_CLR_MSK                               (((1ULL<<M154_RX_ABORT_INT_CLR_LEN)-1)<<M154_RX_ABORT_INT_CLR_POS)
#define M154_RX_ABORT_INT_CLR_UMSK                              (~(((1ULL<<M154_RX_ABORT_INT_CLR_LEN)-1)<<M154_RX_ABORT_INT_CLR_POS))
#define M154_TX_ABORT_INT_CLR                                   M154_TX_ABORT_INT_CLR
#define M154_TX_ABORT_INT_CLR_POS                               (31U)
#define M154_TX_ABORT_INT_CLR_LEN                               (1U)
#define M154_TX_ABORT_INT_CLR_MSK                               (((1ULL<<M154_TX_ABORT_INT_CLR_LEN)-1)<<M154_TX_ABORT_INT_CLR_POS)
#define M154_TX_ABORT_INT_CLR_UMSK                              (~(((1ULL<<M154_TX_ABORT_INT_CLR_LEN)-1)<<M154_TX_ABORT_INT_CLR_POS))

/* 0x28 : m154_int_mask */
#define M154_INT_MASK_OFFSET                                    (0x28)
#define M154_ALARM_00_INT_MASK                                  M154_ALARM_00_INT_MASK
#define M154_ALARM_00_INT_MASK_POS                              (0U)
#define M154_ALARM_00_INT_MASK_LEN                              (1U)
#define M154_ALARM_00_INT_MASK_MSK                              (((1ULL<<M154_ALARM_00_INT_MASK_LEN)-1)<<M154_ALARM_00_INT_MASK_POS)
#define M154_ALARM_00_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_00_INT_MASK_LEN)-1)<<M154_ALARM_00_INT_MASK_POS))
#define M154_ALARM_01_INT_MASK                                  M154_ALARM_01_INT_MASK
#define M154_ALARM_01_INT_MASK_POS                              (1U)
#define M154_ALARM_01_INT_MASK_LEN                              (1U)
#define M154_ALARM_01_INT_MASK_MSK                              (((1ULL<<M154_ALARM_01_INT_MASK_LEN)-1)<<M154_ALARM_01_INT_MASK_POS)
#define M154_ALARM_01_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_01_INT_MASK_LEN)-1)<<M154_ALARM_01_INT_MASK_POS))
#define M154_ALARM_02_INT_MASK                                  M154_ALARM_02_INT_MASK
#define M154_ALARM_02_INT_MASK_POS                              (2U)
#define M154_ALARM_02_INT_MASK_LEN                              (1U)
#define M154_ALARM_02_INT_MASK_MSK                              (((1ULL<<M154_ALARM_02_INT_MASK_LEN)-1)<<M154_ALARM_02_INT_MASK_POS)
#define M154_ALARM_02_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_02_INT_MASK_LEN)-1)<<M154_ALARM_02_INT_MASK_POS))
#define M154_ALARM_03_INT_MASK                                  M154_ALARM_03_INT_MASK
#define M154_ALARM_03_INT_MASK_POS                              (3U)
#define M154_ALARM_03_INT_MASK_LEN                              (1U)
#define M154_ALARM_03_INT_MASK_MSK                              (((1ULL<<M154_ALARM_03_INT_MASK_LEN)-1)<<M154_ALARM_03_INT_MASK_POS)
#define M154_ALARM_03_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_03_INT_MASK_LEN)-1)<<M154_ALARM_03_INT_MASK_POS))
#define M154_ALARM_04_INT_MASK                                  M154_ALARM_04_INT_MASK
#define M154_ALARM_04_INT_MASK_POS                              (4U)
#define M154_ALARM_04_INT_MASK_LEN                              (1U)
#define M154_ALARM_04_INT_MASK_MSK                              (((1ULL<<M154_ALARM_04_INT_MASK_LEN)-1)<<M154_ALARM_04_INT_MASK_POS)
#define M154_ALARM_04_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_04_INT_MASK_LEN)-1)<<M154_ALARM_04_INT_MASK_POS))
#define M154_ALARM_05_INT_MASK                                  M154_ALARM_05_INT_MASK
#define M154_ALARM_05_INT_MASK_POS                              (5U)
#define M154_ALARM_05_INT_MASK_LEN                              (1U)
#define M154_ALARM_05_INT_MASK_MSK                              (((1ULL<<M154_ALARM_05_INT_MASK_LEN)-1)<<M154_ALARM_05_INT_MASK_POS)
#define M154_ALARM_05_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_05_INT_MASK_LEN)-1)<<M154_ALARM_05_INT_MASK_POS))
#define M154_ALARM_06_INT_MASK                                  M154_ALARM_06_INT_MASK
#define M154_ALARM_06_INT_MASK_POS                              (6U)
#define M154_ALARM_06_INT_MASK_LEN                              (1U)
#define M154_ALARM_06_INT_MASK_MSK                              (((1ULL<<M154_ALARM_06_INT_MASK_LEN)-1)<<M154_ALARM_06_INT_MASK_POS)
#define M154_ALARM_06_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_06_INT_MASK_LEN)-1)<<M154_ALARM_06_INT_MASK_POS))
#define M154_ALARM_07_INT_MASK                                  M154_ALARM_07_INT_MASK
#define M154_ALARM_07_INT_MASK_POS                              (7U)
#define M154_ALARM_07_INT_MASK_LEN                              (1U)
#define M154_ALARM_07_INT_MASK_MSK                              (((1ULL<<M154_ALARM_07_INT_MASK_LEN)-1)<<M154_ALARM_07_INT_MASK_POS)
#define M154_ALARM_07_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_07_INT_MASK_LEN)-1)<<M154_ALARM_07_INT_MASK_POS))
#define M154_ALARM_08_INT_MASK                                  M154_ALARM_08_INT_MASK
#define M154_ALARM_08_INT_MASK_POS                              (8U)
#define M154_ALARM_08_INT_MASK_LEN                              (1U)
#define M154_ALARM_08_INT_MASK_MSK                              (((1ULL<<M154_ALARM_08_INT_MASK_LEN)-1)<<M154_ALARM_08_INT_MASK_POS)
#define M154_ALARM_08_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_08_INT_MASK_LEN)-1)<<M154_ALARM_08_INT_MASK_POS))
#define M154_ALARM_09_INT_MASK                                  M154_ALARM_09_INT_MASK
#define M154_ALARM_09_INT_MASK_POS                              (9U)
#define M154_ALARM_09_INT_MASK_LEN                              (1U)
#define M154_ALARM_09_INT_MASK_MSK                              (((1ULL<<M154_ALARM_09_INT_MASK_LEN)-1)<<M154_ALARM_09_INT_MASK_POS)
#define M154_ALARM_09_INT_MASK_UMSK                             (~(((1ULL<<M154_ALARM_09_INT_MASK_LEN)-1)<<M154_ALARM_09_INT_MASK_POS))
#define M154_RX_END_FRAK_INT_MASK                               M154_RX_END_FRAK_INT_MASK
#define M154_RX_END_FRAK_INT_MASK_POS                           (10U)
#define M154_RX_END_FRAK_INT_MASK_LEN                           (1U)
#define M154_RX_END_FRAK_INT_MASK_MSK                           (((1ULL<<M154_RX_END_FRAK_INT_MASK_LEN)-1)<<M154_RX_END_FRAK_INT_MASK_POS)
#define M154_RX_END_FRAK_INT_MASK_UMSK                          (~(((1ULL<<M154_RX_END_FRAK_INT_MASK_LEN)-1)<<M154_RX_END_FRAK_INT_MASK_POS))
#define M154_RX_END_REQ_ENH_ACK_INT_MASK                        M154_RX_END_REQ_ENH_ACK_INT_MASK
#define M154_RX_END_REQ_ENH_ACK_INT_MASK_POS                    (11U)
#define M154_RX_END_REQ_ENH_ACK_INT_MASK_LEN                    (1U)
#define M154_RX_END_REQ_ENH_ACK_INT_MASK_MSK                    (((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_MASK_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_MASK_POS)
#define M154_RX_END_REQ_ENH_ACK_INT_MASK_UMSK                   (~(((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_MASK_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_MASK_POS))
#define M154_RX_END_EXT_INT_MASK                                M154_RX_END_EXT_INT_MASK
#define M154_RX_END_EXT_INT_MASK_POS                            (12U)
#define M154_RX_END_EXT_INT_MASK_LEN                            (1U)
#define M154_RX_END_EXT_INT_MASK_MSK                            (((1ULL<<M154_RX_END_EXT_INT_MASK_LEN)-1)<<M154_RX_END_EXT_INT_MASK_POS)
#define M154_RX_END_EXT_INT_MASK_UMSK                           (~(((1ULL<<M154_RX_END_EXT_INT_MASK_LEN)-1)<<M154_RX_END_EXT_INT_MASK_POS))
#define M154_RX_END_MPP_INT_MASK                                M154_RX_END_MPP_INT_MASK
#define M154_RX_END_MPP_INT_MASK_POS                            (13U)
#define M154_RX_END_MPP_INT_MASK_LEN                            (1U)
#define M154_RX_END_MPP_INT_MASK_MSK                            (((1ULL<<M154_RX_END_MPP_INT_MASK_LEN)-1)<<M154_RX_END_MPP_INT_MASK_POS)
#define M154_RX_END_MPP_INT_MASK_UMSK                           (~(((1ULL<<M154_RX_END_MPP_INT_MASK_LEN)-1)<<M154_RX_END_MPP_INT_MASK_POS))
#define M154_RX_NBR_SEARCH_FAIL_INT_MASK                        M154_RX_NBR_SEARCH_FAIL_INT_MASK
#define M154_RX_NBR_SEARCH_FAIL_INT_MASK_POS                    (14U)
#define M154_RX_NBR_SEARCH_FAIL_INT_MASK_LEN                    (1U)
#define M154_RX_NBR_SEARCH_FAIL_INT_MASK_MSK                    (((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_MASK_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_MASK_POS)
#define M154_RX_NBR_SEARCH_FAIL_INT_MASK_UMSK                   (~(((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_MASK_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_MASK_POS))
#define M154_CS_ALERT_INT_MASK                                  M154_CS_ALERT_INT_MASK
#define M154_CS_ALERT_INT_MASK_POS                              (15U)
#define M154_CS_ALERT_INT_MASK_LEN                              (1U)
#define M154_CS_ALERT_INT_MASK_MSK                              (((1ULL<<M154_CS_ALERT_INT_MASK_LEN)-1)<<M154_CS_ALERT_INT_MASK_POS)
#define M154_CS_ALERT_INT_MASK_UMSK                             (~(((1ULL<<M154_CS_ALERT_INT_MASK_LEN)-1)<<M154_CS_ALERT_INT_MASK_POS))
#define M154_RX_START_INT_MASK                                  M154_RX_START_INT_MASK
#define M154_RX_START_INT_MASK_POS                              (16U)
#define M154_RX_START_INT_MASK_LEN                              (1U)
#define M154_RX_START_INT_MASK_MSK                              (((1ULL<<M154_RX_START_INT_MASK_LEN)-1)<<M154_RX_START_INT_MASK_POS)
#define M154_RX_START_INT_MASK_UMSK                             (~(((1ULL<<M154_RX_START_INT_MASK_LEN)-1)<<M154_RX_START_INT_MASK_POS))
#define M154_RX_MHR_INT_MASK                                    M154_RX_MHR_INT_MASK
#define M154_RX_MHR_INT_MASK_POS                                (17U)
#define M154_RX_MHR_INT_MASK_LEN                                (1U)
#define M154_RX_MHR_INT_MASK_MSK                                (((1ULL<<M154_RX_MHR_INT_MASK_LEN)-1)<<M154_RX_MHR_INT_MASK_POS)
#define M154_RX_MHR_INT_MASK_UMSK                               (~(((1ULL<<M154_RX_MHR_INT_MASK_LEN)-1)<<M154_RX_MHR_INT_MASK_POS))
#define M154_RX_MHR_SECURITY_INT_MASK                           M154_RX_MHR_SECURITY_INT_MASK
#define M154_RX_MHR_SECURITY_INT_MASK_POS                       (18U)
#define M154_RX_MHR_SECURITY_INT_MASK_LEN                       (1U)
#define M154_RX_MHR_SECURITY_INT_MASK_MSK                       (((1ULL<<M154_RX_MHR_SECURITY_INT_MASK_LEN)-1)<<M154_RX_MHR_SECURITY_INT_MASK_POS)
#define M154_RX_MHR_SECURITY_INT_MASK_UMSK                      (~(((1ULL<<M154_RX_MHR_SECURITY_INT_MASK_LEN)-1)<<M154_RX_MHR_SECURITY_INT_MASK_POS))
#define M154_RX_FILTER_FAIL_INT_MASK                            M154_RX_FILTER_FAIL_INT_MASK
#define M154_RX_FILTER_FAIL_INT_MASK_POS                        (19U)
#define M154_RX_FILTER_FAIL_INT_MASK_LEN                        (1U)
#define M154_RX_FILTER_FAIL_INT_MASK_MSK                        (((1ULL<<M154_RX_FILTER_FAIL_INT_MASK_LEN)-1)<<M154_RX_FILTER_FAIL_INT_MASK_POS)
#define M154_RX_FILTER_FAIL_INT_MASK_UMSK                       (~(((1ULL<<M154_RX_FILTER_FAIL_INT_MASK_LEN)-1)<<M154_RX_FILTER_FAIL_INT_MASK_POS))
#define M154_RX_END_BEACON_INT_MASK                             M154_RX_END_BEACON_INT_MASK
#define M154_RX_END_BEACON_INT_MASK_POS                         (20U)
#define M154_RX_END_BEACON_INT_MASK_LEN                         (1U)
#define M154_RX_END_BEACON_INT_MASK_MSK                         (((1ULL<<M154_RX_END_BEACON_INT_MASK_LEN)-1)<<M154_RX_END_BEACON_INT_MASK_POS)
#define M154_RX_END_BEACON_INT_MASK_UMSK                        (~(((1ULL<<M154_RX_END_BEACON_INT_MASK_LEN)-1)<<M154_RX_END_BEACON_INT_MASK_POS))
#define M154_RX_END_DATA_INT_MASK                               M154_RX_END_DATA_INT_MASK
#define M154_RX_END_DATA_INT_MASK_POS                           (21U)
#define M154_RX_END_DATA_INT_MASK_LEN                           (1U)
#define M154_RX_END_DATA_INT_MASK_MSK                           (((1ULL<<M154_RX_END_DATA_INT_MASK_LEN)-1)<<M154_RX_END_DATA_INT_MASK_POS)
#define M154_RX_END_DATA_INT_MASK_UMSK                          (~(((1ULL<<M154_RX_END_DATA_INT_MASK_LEN)-1)<<M154_RX_END_DATA_INT_MASK_POS))
#define M154_RX_END_ACK_INT_MASK                                M154_RX_END_ACK_INT_MASK
#define M154_RX_END_ACK_INT_MASK_POS                            (22U)
#define M154_RX_END_ACK_INT_MASK_LEN                            (1U)
#define M154_RX_END_ACK_INT_MASK_MSK                            (((1ULL<<M154_RX_END_ACK_INT_MASK_LEN)-1)<<M154_RX_END_ACK_INT_MASK_POS)
#define M154_RX_END_ACK_INT_MASK_UMSK                           (~(((1ULL<<M154_RX_END_ACK_INT_MASK_LEN)-1)<<M154_RX_END_ACK_INT_MASK_POS))
#define M154_RX_END_CMD_INT_MASK                                M154_RX_END_CMD_INT_MASK
#define M154_RX_END_CMD_INT_MASK_POS                            (23U)
#define M154_RX_END_CMD_INT_MASK_LEN                            (1U)
#define M154_RX_END_CMD_INT_MASK_MSK                            (((1ULL<<M154_RX_END_CMD_INT_MASK_LEN)-1)<<M154_RX_END_CMD_INT_MASK_POS)
#define M154_RX_END_CMD_INT_MASK_UMSK                           (~(((1ULL<<M154_RX_END_CMD_INT_MASK_LEN)-1)<<M154_RX_END_CMD_INT_MASK_POS))
#define M154_RX_END_ACK_OTHERS_INT_MASK                         M154_RX_END_ACK_OTHERS_INT_MASK
#define M154_RX_END_ACK_OTHERS_INT_MASK_POS                     (24U)
#define M154_RX_END_ACK_OTHERS_INT_MASK_LEN                     (1U)
#define M154_RX_END_ACK_OTHERS_INT_MASK_MSK                     (((1ULL<<M154_RX_END_ACK_OTHERS_INT_MASK_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_MASK_POS)
#define M154_RX_END_ACK_OTHERS_INT_MASK_UMSK                    (~(((1ULL<<M154_RX_END_ACK_OTHERS_INT_MASK_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_MASK_POS))
#define M154_CSMA_CA_SUCCESS_INT_MASK                           M154_CSMA_CA_SUCCESS_INT_MASK
#define M154_CSMA_CA_SUCCESS_INT_MASK_POS                       (25U)
#define M154_CSMA_CA_SUCCESS_INT_MASK_LEN                       (1U)
#define M154_CSMA_CA_SUCCESS_INT_MASK_MSK                       (((1ULL<<M154_CSMA_CA_SUCCESS_INT_MASK_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_MASK_POS)
#define M154_CSMA_CA_SUCCESS_INT_MASK_UMSK                      (~(((1ULL<<M154_CSMA_CA_SUCCESS_INT_MASK_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_MASK_POS))
#define M154_CSMA_CA_FAILURE_INT_MASK                           M154_CSMA_CA_FAILURE_INT_MASK
#define M154_CSMA_CA_FAILURE_INT_MASK_POS                       (26U)
#define M154_CSMA_CA_FAILURE_INT_MASK_LEN                       (1U)
#define M154_CSMA_CA_FAILURE_INT_MASK_MSK                       (((1ULL<<M154_CSMA_CA_FAILURE_INT_MASK_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_MASK_POS)
#define M154_CSMA_CA_FAILURE_INT_MASK_UMSK                      (~(((1ULL<<M154_CSMA_CA_FAILURE_INT_MASK_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_MASK_POS))
#define M154_CSMA_CA_TIMEOUT_INT_MASK                           M154_CSMA_CA_TIMEOUT_INT_MASK
#define M154_CSMA_CA_TIMEOUT_INT_MASK_POS                       (27U)
#define M154_CSMA_CA_TIMEOUT_INT_MASK_LEN                       (1U)
#define M154_CSMA_CA_TIMEOUT_INT_MASK_MSK                       (((1ULL<<M154_CSMA_CA_TIMEOUT_INT_MASK_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_MASK_POS)
#define M154_CSMA_CA_TIMEOUT_INT_MASK_UMSK                      (~(((1ULL<<M154_CSMA_CA_TIMEOUT_INT_MASK_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_MASK_POS))
#define M154_TX_DONE_INT_MASK                                   M154_TX_DONE_INT_MASK
#define M154_TX_DONE_INT_MASK_POS                               (28U)
#define M154_TX_DONE_INT_MASK_LEN                               (1U)
#define M154_TX_DONE_INT_MASK_MSK                               (((1ULL<<M154_TX_DONE_INT_MASK_LEN)-1)<<M154_TX_DONE_INT_MASK_POS)
#define M154_TX_DONE_INT_MASK_UMSK                              (~(((1ULL<<M154_TX_DONE_INT_MASK_LEN)-1)<<M154_TX_DONE_INT_MASK_POS))
#define M154_TX_AUTO_ACK_DONE_INT_MASK                          M154_TX_AUTO_ACK_DONE_INT_MASK
#define M154_TX_AUTO_ACK_DONE_INT_MASK_POS                      (29U)
#define M154_TX_AUTO_ACK_DONE_INT_MASK_LEN                      (1U)
#define M154_TX_AUTO_ACK_DONE_INT_MASK_MSK                      (((1ULL<<M154_TX_AUTO_ACK_DONE_INT_MASK_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_MASK_POS)
#define M154_TX_AUTO_ACK_DONE_INT_MASK_UMSK                     (~(((1ULL<<M154_TX_AUTO_ACK_DONE_INT_MASK_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_MASK_POS))
#define M154_RX_ABORT_INT_MASK                                  M154_RX_ABORT_INT_MASK
#define M154_RX_ABORT_INT_MASK_POS                              (30U)
#define M154_RX_ABORT_INT_MASK_LEN                              (1U)
#define M154_RX_ABORT_INT_MASK_MSK                              (((1ULL<<M154_RX_ABORT_INT_MASK_LEN)-1)<<M154_RX_ABORT_INT_MASK_POS)
#define M154_RX_ABORT_INT_MASK_UMSK                             (~(((1ULL<<M154_RX_ABORT_INT_MASK_LEN)-1)<<M154_RX_ABORT_INT_MASK_POS))
#define M154_TX_ABORT_INT_MASK                                  M154_TX_ABORT_INT_MASK
#define M154_TX_ABORT_INT_MASK_POS                              (31U)
#define M154_TX_ABORT_INT_MASK_LEN                              (1U)
#define M154_TX_ABORT_INT_MASK_MSK                              (((1ULL<<M154_TX_ABORT_INT_MASK_LEN)-1)<<M154_TX_ABORT_INT_MASK_POS)
#define M154_TX_ABORT_INT_MASK_UMSK                             (~(((1ULL<<M154_TX_ABORT_INT_MASK_LEN)-1)<<M154_TX_ABORT_INT_MASK_POS))

/* 0x2C : m154_int_raw */
#define M154_INT_RAW_OFFSET                                     (0x2C)
#define M154_ALARM_00_INT_RAW                                   M154_ALARM_00_INT_RAW
#define M154_ALARM_00_INT_RAW_POS                               (0U)
#define M154_ALARM_00_INT_RAW_LEN                               (1U)
#define M154_ALARM_00_INT_RAW_MSK                               (((1ULL<<M154_ALARM_00_INT_RAW_LEN)-1)<<M154_ALARM_00_INT_RAW_POS)
#define M154_ALARM_00_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_00_INT_RAW_LEN)-1)<<M154_ALARM_00_INT_RAW_POS))
#define M154_ALARM_01_INT_RAW                                   M154_ALARM_01_INT_RAW
#define M154_ALARM_01_INT_RAW_POS                               (1U)
#define M154_ALARM_01_INT_RAW_LEN                               (1U)
#define M154_ALARM_01_INT_RAW_MSK                               (((1ULL<<M154_ALARM_01_INT_RAW_LEN)-1)<<M154_ALARM_01_INT_RAW_POS)
#define M154_ALARM_01_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_01_INT_RAW_LEN)-1)<<M154_ALARM_01_INT_RAW_POS))
#define M154_ALARM_02_INT_RAW                                   M154_ALARM_02_INT_RAW
#define M154_ALARM_02_INT_RAW_POS                               (2U)
#define M154_ALARM_02_INT_RAW_LEN                               (1U)
#define M154_ALARM_02_INT_RAW_MSK                               (((1ULL<<M154_ALARM_02_INT_RAW_LEN)-1)<<M154_ALARM_02_INT_RAW_POS)
#define M154_ALARM_02_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_02_INT_RAW_LEN)-1)<<M154_ALARM_02_INT_RAW_POS))
#define M154_ALARM_03_INT_RAW                                   M154_ALARM_03_INT_RAW
#define M154_ALARM_03_INT_RAW_POS                               (3U)
#define M154_ALARM_03_INT_RAW_LEN                               (1U)
#define M154_ALARM_03_INT_RAW_MSK                               (((1ULL<<M154_ALARM_03_INT_RAW_LEN)-1)<<M154_ALARM_03_INT_RAW_POS)
#define M154_ALARM_03_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_03_INT_RAW_LEN)-1)<<M154_ALARM_03_INT_RAW_POS))
#define M154_ALARM_04_INT_RAW                                   M154_ALARM_04_INT_RAW
#define M154_ALARM_04_INT_RAW_POS                               (4U)
#define M154_ALARM_04_INT_RAW_LEN                               (1U)
#define M154_ALARM_04_INT_RAW_MSK                               (((1ULL<<M154_ALARM_04_INT_RAW_LEN)-1)<<M154_ALARM_04_INT_RAW_POS)
#define M154_ALARM_04_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_04_INT_RAW_LEN)-1)<<M154_ALARM_04_INT_RAW_POS))
#define M154_ALARM_05_INT_RAW                                   M154_ALARM_05_INT_RAW
#define M154_ALARM_05_INT_RAW_POS                               (5U)
#define M154_ALARM_05_INT_RAW_LEN                               (1U)
#define M154_ALARM_05_INT_RAW_MSK                               (((1ULL<<M154_ALARM_05_INT_RAW_LEN)-1)<<M154_ALARM_05_INT_RAW_POS)
#define M154_ALARM_05_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_05_INT_RAW_LEN)-1)<<M154_ALARM_05_INT_RAW_POS))
#define M154_ALARM_06_INT_RAW                                   M154_ALARM_06_INT_RAW
#define M154_ALARM_06_INT_RAW_POS                               (6U)
#define M154_ALARM_06_INT_RAW_LEN                               (1U)
#define M154_ALARM_06_INT_RAW_MSK                               (((1ULL<<M154_ALARM_06_INT_RAW_LEN)-1)<<M154_ALARM_06_INT_RAW_POS)
#define M154_ALARM_06_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_06_INT_RAW_LEN)-1)<<M154_ALARM_06_INT_RAW_POS))
#define M154_ALARM_07_INT_RAW                                   M154_ALARM_07_INT_RAW
#define M154_ALARM_07_INT_RAW_POS                               (7U)
#define M154_ALARM_07_INT_RAW_LEN                               (1U)
#define M154_ALARM_07_INT_RAW_MSK                               (((1ULL<<M154_ALARM_07_INT_RAW_LEN)-1)<<M154_ALARM_07_INT_RAW_POS)
#define M154_ALARM_07_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_07_INT_RAW_LEN)-1)<<M154_ALARM_07_INT_RAW_POS))
#define M154_ALARM_08_INT_RAW                                   M154_ALARM_08_INT_RAW
#define M154_ALARM_08_INT_RAW_POS                               (8U)
#define M154_ALARM_08_INT_RAW_LEN                               (1U)
#define M154_ALARM_08_INT_RAW_MSK                               (((1ULL<<M154_ALARM_08_INT_RAW_LEN)-1)<<M154_ALARM_08_INT_RAW_POS)
#define M154_ALARM_08_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_08_INT_RAW_LEN)-1)<<M154_ALARM_08_INT_RAW_POS))
#define M154_ALARM_09_INT_RAW                                   M154_ALARM_09_INT_RAW
#define M154_ALARM_09_INT_RAW_POS                               (9U)
#define M154_ALARM_09_INT_RAW_LEN                               (1U)
#define M154_ALARM_09_INT_RAW_MSK                               (((1ULL<<M154_ALARM_09_INT_RAW_LEN)-1)<<M154_ALARM_09_INT_RAW_POS)
#define M154_ALARM_09_INT_RAW_UMSK                              (~(((1ULL<<M154_ALARM_09_INT_RAW_LEN)-1)<<M154_ALARM_09_INT_RAW_POS))
#define M154_RX_END_FRAK_INT_RAW                                M154_RX_END_FRAK_INT_RAW
#define M154_RX_END_FRAK_INT_RAW_POS                            (10U)
#define M154_RX_END_FRAK_INT_RAW_LEN                            (1U)
#define M154_RX_END_FRAK_INT_RAW_MSK                            (((1ULL<<M154_RX_END_FRAK_INT_RAW_LEN)-1)<<M154_RX_END_FRAK_INT_RAW_POS)
#define M154_RX_END_FRAK_INT_RAW_UMSK                           (~(((1ULL<<M154_RX_END_FRAK_INT_RAW_LEN)-1)<<M154_RX_END_FRAK_INT_RAW_POS))
#define M154_RX_END_REQ_ENH_ACK_INT_RAW                         M154_RX_END_REQ_ENH_ACK_INT_RAW
#define M154_RX_END_REQ_ENH_ACK_INT_RAW_POS                     (11U)
#define M154_RX_END_REQ_ENH_ACK_INT_RAW_LEN                     (1U)
#define M154_RX_END_REQ_ENH_ACK_INT_RAW_MSK                     (((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_RAW_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_RAW_POS)
#define M154_RX_END_REQ_ENH_ACK_INT_RAW_UMSK                    (~(((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_RAW_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_RAW_POS))
#define M154_RX_END_EXT_INT_RAW                                 M154_RX_END_EXT_INT_RAW
#define M154_RX_END_EXT_INT_RAW_POS                             (12U)
#define M154_RX_END_EXT_INT_RAW_LEN                             (1U)
#define M154_RX_END_EXT_INT_RAW_MSK                             (((1ULL<<M154_RX_END_EXT_INT_RAW_LEN)-1)<<M154_RX_END_EXT_INT_RAW_POS)
#define M154_RX_END_EXT_INT_RAW_UMSK                            (~(((1ULL<<M154_RX_END_EXT_INT_RAW_LEN)-1)<<M154_RX_END_EXT_INT_RAW_POS))
#define M154_RX_END_MPP_INT_RAW                                 M154_RX_END_MPP_INT_RAW
#define M154_RX_END_MPP_INT_RAW_POS                             (13U)
#define M154_RX_END_MPP_INT_RAW_LEN                             (1U)
#define M154_RX_END_MPP_INT_RAW_MSK                             (((1ULL<<M154_RX_END_MPP_INT_RAW_LEN)-1)<<M154_RX_END_MPP_INT_RAW_POS)
#define M154_RX_END_MPP_INT_RAW_UMSK                            (~(((1ULL<<M154_RX_END_MPP_INT_RAW_LEN)-1)<<M154_RX_END_MPP_INT_RAW_POS))
#define M154_RX_NBR_SEARCH_FAIL_INT_RAW                         M154_RX_NBR_SEARCH_FAIL_INT_RAW
#define M154_RX_NBR_SEARCH_FAIL_INT_RAW_POS                     (14U)
#define M154_RX_NBR_SEARCH_FAIL_INT_RAW_LEN                     (1U)
#define M154_RX_NBR_SEARCH_FAIL_INT_RAW_MSK                     (((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_RAW_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_RAW_POS)
#define M154_RX_NBR_SEARCH_FAIL_INT_RAW_UMSK                    (~(((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_RAW_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_RAW_POS))
#define M154_CS_ALERT_INT_RAW                                   M154_CS_ALERT_INT_RAW
#define M154_CS_ALERT_INT_RAW_POS                               (15U)
#define M154_CS_ALERT_INT_RAW_LEN                               (1U)
#define M154_CS_ALERT_INT_RAW_MSK                               (((1ULL<<M154_CS_ALERT_INT_RAW_LEN)-1)<<M154_CS_ALERT_INT_RAW_POS)
#define M154_CS_ALERT_INT_RAW_UMSK                              (~(((1ULL<<M154_CS_ALERT_INT_RAW_LEN)-1)<<M154_CS_ALERT_INT_RAW_POS))
#define M154_RX_START_INT_RAW                                   M154_RX_START_INT_RAW
#define M154_RX_START_INT_RAW_POS                               (16U)
#define M154_RX_START_INT_RAW_LEN                               (1U)
#define M154_RX_START_INT_RAW_MSK                               (((1ULL<<M154_RX_START_INT_RAW_LEN)-1)<<M154_RX_START_INT_RAW_POS)
#define M154_RX_START_INT_RAW_UMSK                              (~(((1ULL<<M154_RX_START_INT_RAW_LEN)-1)<<M154_RX_START_INT_RAW_POS))
#define M154_RX_MHR_INT_RAW                                     M154_RX_MHR_INT_RAW
#define M154_RX_MHR_INT_RAW_POS                                 (17U)
#define M154_RX_MHR_INT_RAW_LEN                                 (1U)
#define M154_RX_MHR_INT_RAW_MSK                                 (((1ULL<<M154_RX_MHR_INT_RAW_LEN)-1)<<M154_RX_MHR_INT_RAW_POS)
#define M154_RX_MHR_INT_RAW_UMSK                                (~(((1ULL<<M154_RX_MHR_INT_RAW_LEN)-1)<<M154_RX_MHR_INT_RAW_POS))
#define M154_RX_MHR_SECURITY_INT_RAW                            M154_RX_MHR_SECURITY_INT_RAW
#define M154_RX_MHR_SECURITY_INT_RAW_POS                        (18U)
#define M154_RX_MHR_SECURITY_INT_RAW_LEN                        (1U)
#define M154_RX_MHR_SECURITY_INT_RAW_MSK                        (((1ULL<<M154_RX_MHR_SECURITY_INT_RAW_LEN)-1)<<M154_RX_MHR_SECURITY_INT_RAW_POS)
#define M154_RX_MHR_SECURITY_INT_RAW_UMSK                       (~(((1ULL<<M154_RX_MHR_SECURITY_INT_RAW_LEN)-1)<<M154_RX_MHR_SECURITY_INT_RAW_POS))
#define M154_RX_FILTER_FAIL_INT_RAW                             M154_RX_FILTER_FAIL_INT_RAW
#define M154_RX_FILTER_FAIL_INT_RAW_POS                         (19U)
#define M154_RX_FILTER_FAIL_INT_RAW_LEN                         (1U)
#define M154_RX_FILTER_FAIL_INT_RAW_MSK                         (((1ULL<<M154_RX_FILTER_FAIL_INT_RAW_LEN)-1)<<M154_RX_FILTER_FAIL_INT_RAW_POS)
#define M154_RX_FILTER_FAIL_INT_RAW_UMSK                        (~(((1ULL<<M154_RX_FILTER_FAIL_INT_RAW_LEN)-1)<<M154_RX_FILTER_FAIL_INT_RAW_POS))
#define M154_RX_END_BEACON_INT_RAW                              M154_RX_END_BEACON_INT_RAW
#define M154_RX_END_BEACON_INT_RAW_POS                          (20U)
#define M154_RX_END_BEACON_INT_RAW_LEN                          (1U)
#define M154_RX_END_BEACON_INT_RAW_MSK                          (((1ULL<<M154_RX_END_BEACON_INT_RAW_LEN)-1)<<M154_RX_END_BEACON_INT_RAW_POS)
#define M154_RX_END_BEACON_INT_RAW_UMSK                         (~(((1ULL<<M154_RX_END_BEACON_INT_RAW_LEN)-1)<<M154_RX_END_BEACON_INT_RAW_POS))
#define M154_RX_END_DATA_INT_RAW                                M154_RX_END_DATA_INT_RAW
#define M154_RX_END_DATA_INT_RAW_POS                            (21U)
#define M154_RX_END_DATA_INT_RAW_LEN                            (1U)
#define M154_RX_END_DATA_INT_RAW_MSK                            (((1ULL<<M154_RX_END_DATA_INT_RAW_LEN)-1)<<M154_RX_END_DATA_INT_RAW_POS)
#define M154_RX_END_DATA_INT_RAW_UMSK                           (~(((1ULL<<M154_RX_END_DATA_INT_RAW_LEN)-1)<<M154_RX_END_DATA_INT_RAW_POS))
#define M154_RX_END_ACK_INT_RAW                                 M154_RX_END_ACK_INT_RAW
#define M154_RX_END_ACK_INT_RAW_POS                             (22U)
#define M154_RX_END_ACK_INT_RAW_LEN                             (1U)
#define M154_RX_END_ACK_INT_RAW_MSK                             (((1ULL<<M154_RX_END_ACK_INT_RAW_LEN)-1)<<M154_RX_END_ACK_INT_RAW_POS)
#define M154_RX_END_ACK_INT_RAW_UMSK                            (~(((1ULL<<M154_RX_END_ACK_INT_RAW_LEN)-1)<<M154_RX_END_ACK_INT_RAW_POS))
#define M154_RX_END_CMD_INT_RAW                                 M154_RX_END_CMD_INT_RAW
#define M154_RX_END_CMD_INT_RAW_POS                             (23U)
#define M154_RX_END_CMD_INT_RAW_LEN                             (1U)
#define M154_RX_END_CMD_INT_RAW_MSK                             (((1ULL<<M154_RX_END_CMD_INT_RAW_LEN)-1)<<M154_RX_END_CMD_INT_RAW_POS)
#define M154_RX_END_CMD_INT_RAW_UMSK                            (~(((1ULL<<M154_RX_END_CMD_INT_RAW_LEN)-1)<<M154_RX_END_CMD_INT_RAW_POS))
#define M154_RX_END_ACK_OTHERS_INT_RAW                          M154_RX_END_ACK_OTHERS_INT_RAW
#define M154_RX_END_ACK_OTHERS_INT_RAW_POS                      (24U)
#define M154_RX_END_ACK_OTHERS_INT_RAW_LEN                      (1U)
#define M154_RX_END_ACK_OTHERS_INT_RAW_MSK                      (((1ULL<<M154_RX_END_ACK_OTHERS_INT_RAW_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_RAW_POS)
#define M154_RX_END_ACK_OTHERS_INT_RAW_UMSK                     (~(((1ULL<<M154_RX_END_ACK_OTHERS_INT_RAW_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_RAW_POS))
#define M154_CSMA_CA_SUCCESS_INT_RAW                            M154_CSMA_CA_SUCCESS_INT_RAW
#define M154_CSMA_CA_SUCCESS_INT_RAW_POS                        (25U)
#define M154_CSMA_CA_SUCCESS_INT_RAW_LEN                        (1U)
#define M154_CSMA_CA_SUCCESS_INT_RAW_MSK                        (((1ULL<<M154_CSMA_CA_SUCCESS_INT_RAW_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_RAW_POS)
#define M154_CSMA_CA_SUCCESS_INT_RAW_UMSK                       (~(((1ULL<<M154_CSMA_CA_SUCCESS_INT_RAW_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_RAW_POS))
#define M154_CSMA_CA_FAILURE_INT_RAW                            M154_CSMA_CA_FAILURE_INT_RAW
#define M154_CSMA_CA_FAILURE_INT_RAW_POS                        (26U)
#define M154_CSMA_CA_FAILURE_INT_RAW_LEN                        (1U)
#define M154_CSMA_CA_FAILURE_INT_RAW_MSK                        (((1ULL<<M154_CSMA_CA_FAILURE_INT_RAW_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_RAW_POS)
#define M154_CSMA_CA_FAILURE_INT_RAW_UMSK                       (~(((1ULL<<M154_CSMA_CA_FAILURE_INT_RAW_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_RAW_POS))
#define M154_CSMA_CA_TIMEOUT_INT_RAW                            M154_CSMA_CA_TIMEOUT_INT_RAW
#define M154_CSMA_CA_TIMEOUT_INT_RAW_POS                        (27U)
#define M154_CSMA_CA_TIMEOUT_INT_RAW_LEN                        (1U)
#define M154_CSMA_CA_TIMEOUT_INT_RAW_MSK                        (((1ULL<<M154_CSMA_CA_TIMEOUT_INT_RAW_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_RAW_POS)
#define M154_CSMA_CA_TIMEOUT_INT_RAW_UMSK                       (~(((1ULL<<M154_CSMA_CA_TIMEOUT_INT_RAW_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_RAW_POS))
#define M154_TX_DONE_INT_RAW                                    M154_TX_DONE_INT_RAW
#define M154_TX_DONE_INT_RAW_POS                                (28U)
#define M154_TX_DONE_INT_RAW_LEN                                (1U)
#define M154_TX_DONE_INT_RAW_MSK                                (((1ULL<<M154_TX_DONE_INT_RAW_LEN)-1)<<M154_TX_DONE_INT_RAW_POS)
#define M154_TX_DONE_INT_RAW_UMSK                               (~(((1ULL<<M154_TX_DONE_INT_RAW_LEN)-1)<<M154_TX_DONE_INT_RAW_POS))
#define M154_TX_AUTO_ACK_DONE_INT_RAW                           M154_TX_AUTO_ACK_DONE_INT_RAW
#define M154_TX_AUTO_ACK_DONE_INT_RAW_POS                       (29U)
#define M154_TX_AUTO_ACK_DONE_INT_RAW_LEN                       (1U)
#define M154_TX_AUTO_ACK_DONE_INT_RAW_MSK                       (((1ULL<<M154_TX_AUTO_ACK_DONE_INT_RAW_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_RAW_POS)
#define M154_TX_AUTO_ACK_DONE_INT_RAW_UMSK                      (~(((1ULL<<M154_TX_AUTO_ACK_DONE_INT_RAW_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_RAW_POS))
#define M154_RX_ABORT_INT_RAW                                   M154_RX_ABORT_INT_RAW
#define M154_RX_ABORT_INT_RAW_POS                               (30U)
#define M154_RX_ABORT_INT_RAW_LEN                               (1U)
#define M154_RX_ABORT_INT_RAW_MSK                               (((1ULL<<M154_RX_ABORT_INT_RAW_LEN)-1)<<M154_RX_ABORT_INT_RAW_POS)
#define M154_RX_ABORT_INT_RAW_UMSK                              (~(((1ULL<<M154_RX_ABORT_INT_RAW_LEN)-1)<<M154_RX_ABORT_INT_RAW_POS))
#define M154_TX_ABORT_INT_RAW                                   M154_TX_ABORT_INT_RAW
#define M154_TX_ABORT_INT_RAW_POS                               (31U)
#define M154_TX_ABORT_INT_RAW_LEN                               (1U)
#define M154_TX_ABORT_INT_RAW_MSK                               (((1ULL<<M154_TX_ABORT_INT_RAW_LEN)-1)<<M154_TX_ABORT_INT_RAW_POS)
#define M154_TX_ABORT_INT_RAW_UMSK                              (~(((1ULL<<M154_TX_ABORT_INT_RAW_LEN)-1)<<M154_TX_ABORT_INT_RAW_POS))

/* 0x30 : m154_int */
#define M154_INT_OFFSET                                         (0x30)
#define M154_ALARM_00_INT                                       M154_ALARM_00_INT
#define M154_ALARM_00_INT_POS                                   (0U)
#define M154_ALARM_00_INT_LEN                                   (1U)
#define M154_ALARM_00_INT_MSK                                   (((1ULL<<M154_ALARM_00_INT_LEN)-1)<<M154_ALARM_00_INT_POS)
#define M154_ALARM_00_INT_UMSK                                  (~(((1ULL<<M154_ALARM_00_INT_LEN)-1)<<M154_ALARM_00_INT_POS))
#define M154_ALARM_01_INT                                       M154_ALARM_01_INT
#define M154_ALARM_01_INT_POS                                   (1U)
#define M154_ALARM_01_INT_LEN                                   (1U)
#define M154_ALARM_01_INT_MSK                                   (((1ULL<<M154_ALARM_01_INT_LEN)-1)<<M154_ALARM_01_INT_POS)
#define M154_ALARM_01_INT_UMSK                                  (~(((1ULL<<M154_ALARM_01_INT_LEN)-1)<<M154_ALARM_01_INT_POS))
#define M154_ALARM_02_INT                                       M154_ALARM_02_INT
#define M154_ALARM_02_INT_POS                                   (2U)
#define M154_ALARM_02_INT_LEN                                   (1U)
#define M154_ALARM_02_INT_MSK                                   (((1ULL<<M154_ALARM_02_INT_LEN)-1)<<M154_ALARM_02_INT_POS)
#define M154_ALARM_02_INT_UMSK                                  (~(((1ULL<<M154_ALARM_02_INT_LEN)-1)<<M154_ALARM_02_INT_POS))
#define M154_ALARM_03_INT                                       M154_ALARM_03_INT
#define M154_ALARM_03_INT_POS                                   (3U)
#define M154_ALARM_03_INT_LEN                                   (1U)
#define M154_ALARM_03_INT_MSK                                   (((1ULL<<M154_ALARM_03_INT_LEN)-1)<<M154_ALARM_03_INT_POS)
#define M154_ALARM_03_INT_UMSK                                  (~(((1ULL<<M154_ALARM_03_INT_LEN)-1)<<M154_ALARM_03_INT_POS))
#define M154_ALARM_04_INT                                       M154_ALARM_04_INT
#define M154_ALARM_04_INT_POS                                   (4U)
#define M154_ALARM_04_INT_LEN                                   (1U)
#define M154_ALARM_04_INT_MSK                                   (((1ULL<<M154_ALARM_04_INT_LEN)-1)<<M154_ALARM_04_INT_POS)
#define M154_ALARM_04_INT_UMSK                                  (~(((1ULL<<M154_ALARM_04_INT_LEN)-1)<<M154_ALARM_04_INT_POS))
#define M154_ALARM_05_INT                                       M154_ALARM_05_INT
#define M154_ALARM_05_INT_POS                                   (5U)
#define M154_ALARM_05_INT_LEN                                   (1U)
#define M154_ALARM_05_INT_MSK                                   (((1ULL<<M154_ALARM_05_INT_LEN)-1)<<M154_ALARM_05_INT_POS)
#define M154_ALARM_05_INT_UMSK                                  (~(((1ULL<<M154_ALARM_05_INT_LEN)-1)<<M154_ALARM_05_INT_POS))
#define M154_ALARM_06_INT                                       M154_ALARM_06_INT
#define M154_ALARM_06_INT_POS                                   (6U)
#define M154_ALARM_06_INT_LEN                                   (1U)
#define M154_ALARM_06_INT_MSK                                   (((1ULL<<M154_ALARM_06_INT_LEN)-1)<<M154_ALARM_06_INT_POS)
#define M154_ALARM_06_INT_UMSK                                  (~(((1ULL<<M154_ALARM_06_INT_LEN)-1)<<M154_ALARM_06_INT_POS))
#define M154_ALARM_07_INT                                       M154_ALARM_07_INT
#define M154_ALARM_07_INT_POS                                   (7U)
#define M154_ALARM_07_INT_LEN                                   (1U)
#define M154_ALARM_07_INT_MSK                                   (((1ULL<<M154_ALARM_07_INT_LEN)-1)<<M154_ALARM_07_INT_POS)
#define M154_ALARM_07_INT_UMSK                                  (~(((1ULL<<M154_ALARM_07_INT_LEN)-1)<<M154_ALARM_07_INT_POS))
#define M154_ALARM_08_INT                                       M154_ALARM_08_INT
#define M154_ALARM_08_INT_POS                                   (8U)
#define M154_ALARM_08_INT_LEN                                   (1U)
#define M154_ALARM_08_INT_MSK                                   (((1ULL<<M154_ALARM_08_INT_LEN)-1)<<M154_ALARM_08_INT_POS)
#define M154_ALARM_08_INT_UMSK                                  (~(((1ULL<<M154_ALARM_08_INT_LEN)-1)<<M154_ALARM_08_INT_POS))
#define M154_ALARM_09_INT                                       M154_ALARM_09_INT
#define M154_ALARM_09_INT_POS                                   (9U)
#define M154_ALARM_09_INT_LEN                                   (1U)
#define M154_ALARM_09_INT_MSK                                   (((1ULL<<M154_ALARM_09_INT_LEN)-1)<<M154_ALARM_09_INT_POS)
#define M154_ALARM_09_INT_UMSK                                  (~(((1ULL<<M154_ALARM_09_INT_LEN)-1)<<M154_ALARM_09_INT_POS))
#define M154_RX_END_FRAK_INT                                    M154_RX_END_FRAK_INT
#define M154_RX_END_FRAK_INT_POS                                (10U)
#define M154_RX_END_FRAK_INT_LEN                                (1U)
#define M154_RX_END_FRAK_INT_MSK                                (((1ULL<<M154_RX_END_FRAK_INT_LEN)-1)<<M154_RX_END_FRAK_INT_POS)
#define M154_RX_END_FRAK_INT_UMSK                               (~(((1ULL<<M154_RX_END_FRAK_INT_LEN)-1)<<M154_RX_END_FRAK_INT_POS))
#define M154_RX_END_REQ_ENH_ACK_INT                             M154_RX_END_REQ_ENH_ACK_INT
#define M154_RX_END_REQ_ENH_ACK_INT_POS                         (11U)
#define M154_RX_END_REQ_ENH_ACK_INT_LEN                         (1U)
#define M154_RX_END_REQ_ENH_ACK_INT_MSK                         (((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_POS)
#define M154_RX_END_REQ_ENH_ACK_INT_UMSK                        (~(((1ULL<<M154_RX_END_REQ_ENH_ACK_INT_LEN)-1)<<M154_RX_END_REQ_ENH_ACK_INT_POS))
#define M154_RX_END_EXT_INT                                     M154_RX_END_EXT_INT
#define M154_RX_END_EXT_INT_POS                                 (12U)
#define M154_RX_END_EXT_INT_LEN                                 (1U)
#define M154_RX_END_EXT_INT_MSK                                 (((1ULL<<M154_RX_END_EXT_INT_LEN)-1)<<M154_RX_END_EXT_INT_POS)
#define M154_RX_END_EXT_INT_UMSK                                (~(((1ULL<<M154_RX_END_EXT_INT_LEN)-1)<<M154_RX_END_EXT_INT_POS))
#define M154_RX_END_MPP_INT                                     M154_RX_END_MPP_INT
#define M154_RX_END_MPP_INT_POS                                 (13U)
#define M154_RX_END_MPP_INT_LEN                                 (1U)
#define M154_RX_END_MPP_INT_MSK                                 (((1ULL<<M154_RX_END_MPP_INT_LEN)-1)<<M154_RX_END_MPP_INT_POS)
#define M154_RX_END_MPP_INT_UMSK                                (~(((1ULL<<M154_RX_END_MPP_INT_LEN)-1)<<M154_RX_END_MPP_INT_POS))
#define M154_RX_NBR_SEARCH_FAIL_INT                             M154_RX_NBR_SEARCH_FAIL_INT
#define M154_RX_NBR_SEARCH_FAIL_INT_POS                         (14U)
#define M154_RX_NBR_SEARCH_FAIL_INT_LEN                         (1U)
#define M154_RX_NBR_SEARCH_FAIL_INT_MSK                         (((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_POS)
#define M154_RX_NBR_SEARCH_FAIL_INT_UMSK                        (~(((1ULL<<M154_RX_NBR_SEARCH_FAIL_INT_LEN)-1)<<M154_RX_NBR_SEARCH_FAIL_INT_POS))
#define M154_CS_ALERT_INT                                       M154_CS_ALERT_INT
#define M154_CS_ALERT_INT_POS                                   (15U)
#define M154_CS_ALERT_INT_LEN                                   (1U)
#define M154_CS_ALERT_INT_MSK                                   (((1ULL<<M154_CS_ALERT_INT_LEN)-1)<<M154_CS_ALERT_INT_POS)
#define M154_CS_ALERT_INT_UMSK                                  (~(((1ULL<<M154_CS_ALERT_INT_LEN)-1)<<M154_CS_ALERT_INT_POS))
#define M154_RX_START_INT                                       M154_RX_START_INT
#define M154_RX_START_INT_POS                                   (16U)
#define M154_RX_START_INT_LEN                                   (1U)
#define M154_RX_START_INT_MSK                                   (((1ULL<<M154_RX_START_INT_LEN)-1)<<M154_RX_START_INT_POS)
#define M154_RX_START_INT_UMSK                                  (~(((1ULL<<M154_RX_START_INT_LEN)-1)<<M154_RX_START_INT_POS))
#define M154_RX_MHR_INT                                         M154_RX_MHR_INT
#define M154_RX_MHR_INT_POS                                     (17U)
#define M154_RX_MHR_INT_LEN                                     (1U)
#define M154_RX_MHR_INT_MSK                                     (((1ULL<<M154_RX_MHR_INT_LEN)-1)<<M154_RX_MHR_INT_POS)
#define M154_RX_MHR_INT_UMSK                                    (~(((1ULL<<M154_RX_MHR_INT_LEN)-1)<<M154_RX_MHR_INT_POS))
#define M154_RX_MHR_SECURITY_INT                                M154_RX_MHR_SECURITY_INT
#define M154_RX_MHR_SECURITY_INT_POS                            (18U)
#define M154_RX_MHR_SECURITY_INT_LEN                            (1U)
#define M154_RX_MHR_SECURITY_INT_MSK                            (((1ULL<<M154_RX_MHR_SECURITY_INT_LEN)-1)<<M154_RX_MHR_SECURITY_INT_POS)
#define M154_RX_MHR_SECURITY_INT_UMSK                           (~(((1ULL<<M154_RX_MHR_SECURITY_INT_LEN)-1)<<M154_RX_MHR_SECURITY_INT_POS))
#define M154_RX_FILTER_FAIL_INT                                 M154_RX_FILTER_FAIL_INT
#define M154_RX_FILTER_FAIL_INT_POS                             (19U)
#define M154_RX_FILTER_FAIL_INT_LEN                             (1U)
#define M154_RX_FILTER_FAIL_INT_MSK                             (((1ULL<<M154_RX_FILTER_FAIL_INT_LEN)-1)<<M154_RX_FILTER_FAIL_INT_POS)
#define M154_RX_FILTER_FAIL_INT_UMSK                            (~(((1ULL<<M154_RX_FILTER_FAIL_INT_LEN)-1)<<M154_RX_FILTER_FAIL_INT_POS))
#define M154_RX_END_BEACON_INT                                  M154_RX_END_BEACON_INT
#define M154_RX_END_BEACON_INT_POS                              (20U)
#define M154_RX_END_BEACON_INT_LEN                              (1U)
#define M154_RX_END_BEACON_INT_MSK                              (((1ULL<<M154_RX_END_BEACON_INT_LEN)-1)<<M154_RX_END_BEACON_INT_POS)
#define M154_RX_END_BEACON_INT_UMSK                             (~(((1ULL<<M154_RX_END_BEACON_INT_LEN)-1)<<M154_RX_END_BEACON_INT_POS))
#define M154_RX_END_DATA_INT                                    M154_RX_END_DATA_INT
#define M154_RX_END_DATA_INT_POS                                (21U)
#define M154_RX_END_DATA_INT_LEN                                (1U)
#define M154_RX_END_DATA_INT_MSK                                (((1ULL<<M154_RX_END_DATA_INT_LEN)-1)<<M154_RX_END_DATA_INT_POS)
#define M154_RX_END_DATA_INT_UMSK                               (~(((1ULL<<M154_RX_END_DATA_INT_LEN)-1)<<M154_RX_END_DATA_INT_POS))
#define M154_RX_END_ACK_INT                                     M154_RX_END_ACK_INT
#define M154_RX_END_ACK_INT_POS                                 (22U)
#define M154_RX_END_ACK_INT_LEN                                 (1U)
#define M154_RX_END_ACK_INT_MSK                                 (((1ULL<<M154_RX_END_ACK_INT_LEN)-1)<<M154_RX_END_ACK_INT_POS)
#define M154_RX_END_ACK_INT_UMSK                                (~(((1ULL<<M154_RX_END_ACK_INT_LEN)-1)<<M154_RX_END_ACK_INT_POS))
#define M154_RX_END_CMD_INT                                     M154_RX_END_CMD_INT
#define M154_RX_END_CMD_INT_POS                                 (23U)
#define M154_RX_END_CMD_INT_LEN                                 (1U)
#define M154_RX_END_CMD_INT_MSK                                 (((1ULL<<M154_RX_END_CMD_INT_LEN)-1)<<M154_RX_END_CMD_INT_POS)
#define M154_RX_END_CMD_INT_UMSK                                (~(((1ULL<<M154_RX_END_CMD_INT_LEN)-1)<<M154_RX_END_CMD_INT_POS))
#define M154_RX_END_ACK_OTHERS_INT                              M154_RX_END_ACK_OTHERS_INT
#define M154_RX_END_ACK_OTHERS_INT_POS                          (24U)
#define M154_RX_END_ACK_OTHERS_INT_LEN                          (1U)
#define M154_RX_END_ACK_OTHERS_INT_MSK                          (((1ULL<<M154_RX_END_ACK_OTHERS_INT_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_POS)
#define M154_RX_END_ACK_OTHERS_INT_UMSK                         (~(((1ULL<<M154_RX_END_ACK_OTHERS_INT_LEN)-1)<<M154_RX_END_ACK_OTHERS_INT_POS))
#define M154_CSMA_CA_SUCCESS_INT                                M154_CSMA_CA_SUCCESS_INT
#define M154_CSMA_CA_SUCCESS_INT_POS                            (25U)
#define M154_CSMA_CA_SUCCESS_INT_LEN                            (1U)
#define M154_CSMA_CA_SUCCESS_INT_MSK                            (((1ULL<<M154_CSMA_CA_SUCCESS_INT_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_POS)
#define M154_CSMA_CA_SUCCESS_INT_UMSK                           (~(((1ULL<<M154_CSMA_CA_SUCCESS_INT_LEN)-1)<<M154_CSMA_CA_SUCCESS_INT_POS))
#define M154_CSMA_CA_FAILURE_INT                                M154_CSMA_CA_FAILURE_INT
#define M154_CSMA_CA_FAILURE_INT_POS                            (26U)
#define M154_CSMA_CA_FAILURE_INT_LEN                            (1U)
#define M154_CSMA_CA_FAILURE_INT_MSK                            (((1ULL<<M154_CSMA_CA_FAILURE_INT_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_POS)
#define M154_CSMA_CA_FAILURE_INT_UMSK                           (~(((1ULL<<M154_CSMA_CA_FAILURE_INT_LEN)-1)<<M154_CSMA_CA_FAILURE_INT_POS))
#define M154_CSMA_CA_TIMEOUT_INT                                M154_CSMA_CA_TIMEOUT_INT
#define M154_CSMA_CA_TIMEOUT_INT_POS                            (27U)
#define M154_CSMA_CA_TIMEOUT_INT_LEN                            (1U)
#define M154_CSMA_CA_TIMEOUT_INT_MSK                            (((1ULL<<M154_CSMA_CA_TIMEOUT_INT_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_POS)
#define M154_CSMA_CA_TIMEOUT_INT_UMSK                           (~(((1ULL<<M154_CSMA_CA_TIMEOUT_INT_LEN)-1)<<M154_CSMA_CA_TIMEOUT_INT_POS))
#define M154_TX_DONE_INT                                        M154_TX_DONE_INT
#define M154_TX_DONE_INT_POS                                    (28U)
#define M154_TX_DONE_INT_LEN                                    (1U)
#define M154_TX_DONE_INT_MSK                                    (((1ULL<<M154_TX_DONE_INT_LEN)-1)<<M154_TX_DONE_INT_POS)
#define M154_TX_DONE_INT_UMSK                                   (~(((1ULL<<M154_TX_DONE_INT_LEN)-1)<<M154_TX_DONE_INT_POS))
#define M154_TX_AUTO_ACK_DONE_INT                               M154_TX_AUTO_ACK_DONE_INT
#define M154_TX_AUTO_ACK_DONE_INT_POS                           (29U)
#define M154_TX_AUTO_ACK_DONE_INT_LEN                           (1U)
#define M154_TX_AUTO_ACK_DONE_INT_MSK                           (((1ULL<<M154_TX_AUTO_ACK_DONE_INT_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_POS)
#define M154_TX_AUTO_ACK_DONE_INT_UMSK                          (~(((1ULL<<M154_TX_AUTO_ACK_DONE_INT_LEN)-1)<<M154_TX_AUTO_ACK_DONE_INT_POS))
#define M154_RX_ABORT_INT                                       M154_RX_ABORT_INT
#define M154_RX_ABORT_INT_POS                                   (30U)
#define M154_RX_ABORT_INT_LEN                                   (1U)
#define M154_RX_ABORT_INT_MSK                                   (((1ULL<<M154_RX_ABORT_INT_LEN)-1)<<M154_RX_ABORT_INT_POS)
#define M154_RX_ABORT_INT_UMSK                                  (~(((1ULL<<M154_RX_ABORT_INT_LEN)-1)<<M154_RX_ABORT_INT_POS))
#define M154_TX_ABORT_INT                                       M154_TX_ABORT_INT
#define M154_TX_ABORT_INT_POS                                   (31U)
#define M154_TX_ABORT_INT_LEN                                   (1U)
#define M154_TX_ABORT_INT_MSK                                   (((1ULL<<M154_TX_ABORT_INT_LEN)-1)<<M154_TX_ABORT_INT_POS)
#define M154_TX_ABORT_INT_UMSK                                  (~(((1ULL<<M154_TX_ABORT_INT_LEN)-1)<<M154_TX_ABORT_INT_POS))

/* 0x34 : m154_gci_ctrl0 */
#define M154_GCI_CTRL0_OFFSET                                   (0x34)
#define M154_GCI_CTRL_FW_SEL                                    M154_GCI_CTRL_FW_SEL
#define M154_GCI_CTRL_FW_SEL_POS                                (0U)
#define M154_GCI_CTRL_FW_SEL_LEN                                (4U)
#define M154_GCI_CTRL_FW_SEL_MSK                                (((1ULL<<M154_GCI_CTRL_FW_SEL_LEN)-1)<<M154_GCI_CTRL_FW_SEL_POS)
#define M154_GCI_CTRL_FW_SEL_UMSK                               (~(((1ULL<<M154_GCI_CTRL_FW_SEL_LEN)-1)<<M154_GCI_CTRL_FW_SEL_POS))
#define M154_GCI_CTRL_FW                                        M154_GCI_CTRL_FW
#define M154_GCI_CTRL_FW_POS                                    (8U)
#define M154_GCI_CTRL_FW_LEN                                    (4U)
#define M154_GCI_CTRL_FW_MSK                                    (((1ULL<<M154_GCI_CTRL_FW_LEN)-1)<<M154_GCI_CTRL_FW_POS)
#define M154_GCI_CTRL_FW_UMSK                                   (~(((1ULL<<M154_GCI_CTRL_FW_LEN)-1)<<M154_GCI_CTRL_FW_POS))
#define M154_GCI_CTRL_HW                                        M154_GCI_CTRL_HW
#define M154_GCI_CTRL_HW_POS                                    (16U)
#define M154_GCI_CTRL_HW_LEN                                    (4U)
#define M154_GCI_CTRL_HW_MSK                                    (((1ULL<<M154_GCI_CTRL_HW_LEN)-1)<<M154_GCI_CTRL_HW_POS)
#define M154_GCI_CTRL_HW_UMSK                                   (~(((1ULL<<M154_GCI_CTRL_HW_LEN)-1)<<M154_GCI_CTRL_HW_POS))
#define M154_GCI_CTRL_SYNC                                      M154_GCI_CTRL_SYNC
#define M154_GCI_CTRL_SYNC_POS                                  (24U)
#define M154_GCI_CTRL_SYNC_LEN                                  (4U)
#define M154_GCI_CTRL_SYNC_MSK                                  (((1ULL<<M154_GCI_CTRL_SYNC_LEN)-1)<<M154_GCI_CTRL_SYNC_POS)
#define M154_GCI_CTRL_SYNC_UMSK                                 (~(((1ULL<<M154_GCI_CTRL_SYNC_LEN)-1)<<M154_GCI_CTRL_SYNC_POS))
#define M154_GCI_INT_SYNC                                       M154_GCI_INT_SYNC
#define M154_GCI_INT_SYNC_POS                                   (28U)
#define M154_GCI_INT_SYNC_LEN                                   (1U)
#define M154_GCI_INT_SYNC_MSK                                   (((1ULL<<M154_GCI_INT_SYNC_LEN)-1)<<M154_GCI_INT_SYNC_POS)
#define M154_GCI_INT_SYNC_UMSK                                  (~(((1ULL<<M154_GCI_INT_SYNC_LEN)-1)<<M154_GCI_INT_SYNC_POS))

/* 0x38 : m154_gci_ctrl1 */
#define M154_GCI_CTRL1_OFFSET                                   (0x38)
#define M154_GCI_TX_CONFX_CANCEL_RX_EN                          M154_GCI_TX_CONFX_CANCEL_RX_EN
#define M154_GCI_TX_CONFX_CANCEL_RX_EN_POS                      (0U)
#define M154_GCI_TX_CONFX_CANCEL_RX_EN_LEN                      (1U)
#define M154_GCI_TX_CONFX_CANCEL_RX_EN_MSK                      (((1ULL<<M154_GCI_TX_CONFX_CANCEL_RX_EN_LEN)-1)<<M154_GCI_TX_CONFX_CANCEL_RX_EN_POS)
#define M154_GCI_TX_CONFX_CANCEL_RX_EN_UMSK                     (~(((1ULL<<M154_GCI_TX_CONFX_CANCEL_RX_EN_LEN)-1)<<M154_GCI_TX_CONFX_CANCEL_RX_EN_POS))
#define M154_GCI_TX_CONFX_CANCEL_TX_EN                          M154_GCI_TX_CONFX_CANCEL_TX_EN
#define M154_GCI_TX_CONFX_CANCEL_TX_EN_POS                      (1U)
#define M154_GCI_TX_CONFX_CANCEL_TX_EN_LEN                      (1U)
#define M154_GCI_TX_CONFX_CANCEL_TX_EN_MSK                      (((1ULL<<M154_GCI_TX_CONFX_CANCEL_TX_EN_LEN)-1)<<M154_GCI_TX_CONFX_CANCEL_TX_EN_POS)
#define M154_GCI_TX_CONFX_CANCEL_TX_EN_UMSK                     (~(((1ULL<<M154_GCI_TX_CONFX_CANCEL_TX_EN_LEN)-1)<<M154_GCI_TX_CONFX_CANCEL_TX_EN_POS))
#define M154_GCI_TX_CONFX_INT_DUAL_EN                           M154_GCI_TX_CONFX_INT_DUAL_EN
#define M154_GCI_TX_CONFX_INT_DUAL_EN_POS                       (2U)
#define M154_GCI_TX_CONFX_INT_DUAL_EN_LEN                       (1U)
#define M154_GCI_TX_CONFX_INT_DUAL_EN_MSK                       (((1ULL<<M154_GCI_TX_CONFX_INT_DUAL_EN_LEN)-1)<<M154_GCI_TX_CONFX_INT_DUAL_EN_POS)
#define M154_GCI_TX_CONFX_INT_DUAL_EN_UMSK                      (~(((1ULL<<M154_GCI_TX_CONFX_INT_DUAL_EN_LEN)-1)<<M154_GCI_TX_CONFX_INT_DUAL_EN_POS))
#define M154_GCI_INT_DUAL_EN                                    M154_GCI_INT_DUAL_EN
#define M154_GCI_INT_DUAL_EN_POS                                (3U)
#define M154_GCI_INT_DUAL_EN_LEN                                (1U)
#define M154_GCI_INT_DUAL_EN_MSK                                (((1ULL<<M154_GCI_INT_DUAL_EN_LEN)-1)<<M154_GCI_INT_DUAL_EN_POS)
#define M154_GCI_INT_DUAL_EN_UMSK                               (~(((1ULL<<M154_GCI_INT_DUAL_EN_LEN)-1)<<M154_GCI_INT_DUAL_EN_POS))

/* 0x40 : m154_sym_cnt */
#define M154_SYM_CNT_OFFSET                                     (0x40)
#define M154_SYM_CNT                                            M154_SYM_CNT
#define M154_SYM_CNT_POS                                        (0U)
#define M154_SYM_CNT_LEN                                        (32U)
#define M154_SYM_CNT_MSK                                        (((1ULL<<M154_SYM_CNT_LEN)-1)<<M154_SYM_CNT_POS)
#define M154_SYM_CNT_UMSK                                       (~(((1ULL<<M154_SYM_CNT_LEN)-1)<<M154_SYM_CNT_POS))

/* 0x44 : m154_sym_cnt_frc */
#define M154_SYM_CNT_FRC_OFFSET                                 (0x44)
#define M154_SYM_CNT_FRC                                        M154_SYM_CNT_FRC
#define M154_SYM_CNT_FRC_POS                                    (0U)
#define M154_SYM_CNT_FRC_LEN                                    (4U)
#define M154_SYM_CNT_FRC_MSK                                    (((1ULL<<M154_SYM_CNT_FRC_LEN)-1)<<M154_SYM_CNT_FRC_POS)
#define M154_SYM_CNT_FRC_UMSK                                   (~(((1ULL<<M154_SYM_CNT_FRC_LEN)-1)<<M154_SYM_CNT_FRC_POS))
#define M154_SYM_CNT_RX_BEACON_P1_FLAG                          M154_SYM_CNT_RX_BEACON_P1_FLAG
#define M154_SYM_CNT_RX_BEACON_P1_FLAG_POS                      (8U)
#define M154_SYM_CNT_RX_BEACON_P1_FLAG_LEN                      (4U)
#define M154_SYM_CNT_RX_BEACON_P1_FLAG_MSK                      (((1ULL<<M154_SYM_CNT_RX_BEACON_P1_FLAG_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P1_FLAG_POS)
#define M154_SYM_CNT_RX_BEACON_P1_FLAG_UMSK                     (~(((1ULL<<M154_SYM_CNT_RX_BEACON_P1_FLAG_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P1_FLAG_POS))
#define M154_SYM_CNT_RX_BEACON_P2_FLAG                          M154_SYM_CNT_RX_BEACON_P2_FLAG
#define M154_SYM_CNT_RX_BEACON_P2_FLAG_POS                      (12U)
#define M154_SYM_CNT_RX_BEACON_P2_FLAG_LEN                      (4U)
#define M154_SYM_CNT_RX_BEACON_P2_FLAG_MSK                      (((1ULL<<M154_SYM_CNT_RX_BEACON_P2_FLAG_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P2_FLAG_POS)
#define M154_SYM_CNT_RX_BEACON_P2_FLAG_UMSK                     (~(((1ULL<<M154_SYM_CNT_RX_BEACON_P2_FLAG_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P2_FLAG_POS))

/* 0x48 : m154_sym_cnt_rx_beacon_p1 */
#define M154_SYM_CNT_RX_BEACON_P1_OFFSET                        (0x48)
#define M154_SYM_CNT_RX_BEACON_P1                               M154_SYM_CNT_RX_BEACON_P1
#define M154_SYM_CNT_RX_BEACON_P1_POS                           (0U)
#define M154_SYM_CNT_RX_BEACON_P1_LEN                           (32U)
#define M154_SYM_CNT_RX_BEACON_P1_MSK                           (((1ULL<<M154_SYM_CNT_RX_BEACON_P1_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P1_POS)
#define M154_SYM_CNT_RX_BEACON_P1_UMSK                          (~(((1ULL<<M154_SYM_CNT_RX_BEACON_P1_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P1_POS))

/* 0x4C : m154_sym_cnt_rx_beacon_p2 */
#define M154_SYM_CNT_RX_BEACON_P2_OFFSET                        (0x4C)
#define M154_SYM_CNT_RX_BEACON_P2                               M154_SYM_CNT_RX_BEACON_P2
#define M154_SYM_CNT_RX_BEACON_P2_POS                           (0U)
#define M154_SYM_CNT_RX_BEACON_P2_LEN                           (32U)
#define M154_SYM_CNT_RX_BEACON_P2_MSK                           (((1ULL<<M154_SYM_CNT_RX_BEACON_P2_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P2_POS)
#define M154_SYM_CNT_RX_BEACON_P2_UMSK                          (~(((1ULL<<M154_SYM_CNT_RX_BEACON_P2_LEN)-1)<<M154_SYM_CNT_RX_BEACON_P2_POS))

/* 0x50 : m154_sym_cnt_rx_start */
#define M154_SYM_CNT_RX_START_OFFSET                            (0x50)
#define M154_SYM_CNT_RX_START                                   M154_SYM_CNT_RX_START
#define M154_SYM_CNT_RX_START_POS                               (0U)
#define M154_SYM_CNT_RX_START_LEN                               (32U)
#define M154_SYM_CNT_RX_START_MSK                               (((1ULL<<M154_SYM_CNT_RX_START_LEN)-1)<<M154_SYM_CNT_RX_START_POS)
#define M154_SYM_CNT_RX_START_UMSK                              (~(((1ULL<<M154_SYM_CNT_RX_START_LEN)-1)<<M154_SYM_CNT_RX_START_POS))

/* 0x54 : m154_sym_cnt_rx_end */
#define M154_SYM_CNT_RX_END_OFFSET                              (0x54)
#define M154_SYM_CNT_RX_END                                     M154_SYM_CNT_RX_END
#define M154_SYM_CNT_RX_END_POS                                 (0U)
#define M154_SYM_CNT_RX_END_LEN                                 (32U)
#define M154_SYM_CNT_RX_END_MSK                                 (((1ULL<<M154_SYM_CNT_RX_END_LEN)-1)<<M154_SYM_CNT_RX_END_POS)
#define M154_SYM_CNT_RX_END_UMSK                                (~(((1ULL<<M154_SYM_CNT_RX_END_LEN)-1)<<M154_SYM_CNT_RX_END_POS))

/* 0x58 : m154_sym_cnt_csma_ca_start */
#define M154_SYM_CNT_CSMA_CA_START_OFFSET                       (0x58)
#define M154_SYM_CNT_CSMA_CA_START                              M154_SYM_CNT_CSMA_CA_START
#define M154_SYM_CNT_CSMA_CA_START_POS                          (0U)
#define M154_SYM_CNT_CSMA_CA_START_LEN                          (32U)
#define M154_SYM_CNT_CSMA_CA_START_MSK                          (((1ULL<<M154_SYM_CNT_CSMA_CA_START_LEN)-1)<<M154_SYM_CNT_CSMA_CA_START_POS)
#define M154_SYM_CNT_CSMA_CA_START_UMSK                         (~(((1ULL<<M154_SYM_CNT_CSMA_CA_START_LEN)-1)<<M154_SYM_CNT_CSMA_CA_START_POS))

/* 0x5C : m154_sym_cnt_tx_beacon */
#define M154_SYM_CNT_TX_BEACON_OFFSET                           (0x5C)
#define M154_SYM_CNT_TX_BEACON                                  M154_SYM_CNT_TX_BEACON
#define M154_SYM_CNT_TX_BEACON_POS                              (0U)
#define M154_SYM_CNT_TX_BEACON_LEN                              (32U)
#define M154_SYM_CNT_TX_BEACON_MSK                              (((1ULL<<M154_SYM_CNT_TX_BEACON_LEN)-1)<<M154_SYM_CNT_TX_BEACON_POS)
#define M154_SYM_CNT_TX_BEACON_UMSK                             (~(((1ULL<<M154_SYM_CNT_TX_BEACON_LEN)-1)<<M154_SYM_CNT_TX_BEACON_POS))

/* 0x60 : m154_sym_cnt_tx_end */
#define M154_SYM_CNT_TX_END_OFFSET                              (0x60)
#define M154_SYM_CNT_TX_END                                     M154_SYM_CNT_TX_END
#define M154_SYM_CNT_TX_END_POS                                 (0U)
#define M154_SYM_CNT_TX_END_LEN                                 (32U)
#define M154_SYM_CNT_TX_END_MSK                                 (((1ULL<<M154_SYM_CNT_TX_END_LEN)-1)<<M154_SYM_CNT_TX_END_POS)
#define M154_SYM_CNT_TX_END_UMSK                                (~(((1ULL<<M154_SYM_CNT_TX_END_LEN)-1)<<M154_SYM_CNT_TX_END_POS))

/* 0x64 : m154_alarm_00_target */
#define M154_ALARM_00_TARGET_OFFSET                             (0x64)
#define M154_ALARM_00_TARGET                                    M154_ALARM_00_TARGET
#define M154_ALARM_00_TARGET_POS                                (0U)
#define M154_ALARM_00_TARGET_LEN                                (32U)
#define M154_ALARM_00_TARGET_MSK                                (((1ULL<<M154_ALARM_00_TARGET_LEN)-1)<<M154_ALARM_00_TARGET_POS)
#define M154_ALARM_00_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_00_TARGET_LEN)-1)<<M154_ALARM_00_TARGET_POS))

/* 0x68 : m154_alarm_01_target */
#define M154_ALARM_01_TARGET_OFFSET                             (0x68)
#define M154_ALARM_01_TARGET                                    M154_ALARM_01_TARGET
#define M154_ALARM_01_TARGET_POS                                (0U)
#define M154_ALARM_01_TARGET_LEN                                (32U)
#define M154_ALARM_01_TARGET_MSK                                (((1ULL<<M154_ALARM_01_TARGET_LEN)-1)<<M154_ALARM_01_TARGET_POS)
#define M154_ALARM_01_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_01_TARGET_LEN)-1)<<M154_ALARM_01_TARGET_POS))

/* 0x6C : m154_alarm_02_target */
#define M154_ALARM_02_TARGET_OFFSET                             (0x6C)
#define M154_ALARM_02_TARGET                                    M154_ALARM_02_TARGET
#define M154_ALARM_02_TARGET_POS                                (0U)
#define M154_ALARM_02_TARGET_LEN                                (32U)
#define M154_ALARM_02_TARGET_MSK                                (((1ULL<<M154_ALARM_02_TARGET_LEN)-1)<<M154_ALARM_02_TARGET_POS)
#define M154_ALARM_02_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_02_TARGET_LEN)-1)<<M154_ALARM_02_TARGET_POS))

/* 0x70 : m154_alarm_03_target */
#define M154_ALARM_03_TARGET_OFFSET                             (0x70)
#define M154_ALARM_03_TARGET                                    M154_ALARM_03_TARGET
#define M154_ALARM_03_TARGET_POS                                (0U)
#define M154_ALARM_03_TARGET_LEN                                (32U)
#define M154_ALARM_03_TARGET_MSK                                (((1ULL<<M154_ALARM_03_TARGET_LEN)-1)<<M154_ALARM_03_TARGET_POS)
#define M154_ALARM_03_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_03_TARGET_LEN)-1)<<M154_ALARM_03_TARGET_POS))

/* 0x74 : m154_alarm_04_target */
#define M154_ALARM_04_TARGET_OFFSET                             (0x74)
#define M154_ALARM_04_TARGET                                    M154_ALARM_04_TARGET
#define M154_ALARM_04_TARGET_POS                                (0U)
#define M154_ALARM_04_TARGET_LEN                                (32U)
#define M154_ALARM_04_TARGET_MSK                                (((1ULL<<M154_ALARM_04_TARGET_LEN)-1)<<M154_ALARM_04_TARGET_POS)
#define M154_ALARM_04_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_04_TARGET_LEN)-1)<<M154_ALARM_04_TARGET_POS))

/* 0x78 : m154_alarm_05_target */
#define M154_ALARM_05_TARGET_OFFSET                             (0x78)
#define M154_ALARM_05_TARGET                                    M154_ALARM_05_TARGET
#define M154_ALARM_05_TARGET_POS                                (0U)
#define M154_ALARM_05_TARGET_LEN                                (32U)
#define M154_ALARM_05_TARGET_MSK                                (((1ULL<<M154_ALARM_05_TARGET_LEN)-1)<<M154_ALARM_05_TARGET_POS)
#define M154_ALARM_05_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_05_TARGET_LEN)-1)<<M154_ALARM_05_TARGET_POS))

/* 0x7C : m154_alarm_06_target */
#define M154_ALARM_06_TARGET_OFFSET                             (0x7C)
#define M154_ALARM_06_TARGET                                    M154_ALARM_06_TARGET
#define M154_ALARM_06_TARGET_POS                                (0U)
#define M154_ALARM_06_TARGET_LEN                                (32U)
#define M154_ALARM_06_TARGET_MSK                                (((1ULL<<M154_ALARM_06_TARGET_LEN)-1)<<M154_ALARM_06_TARGET_POS)
#define M154_ALARM_06_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_06_TARGET_LEN)-1)<<M154_ALARM_06_TARGET_POS))

/* 0x80 : m154_alarm_07_target */
#define M154_ALARM_07_TARGET_OFFSET                             (0x80)
#define M154_ALARM_07_TARGET                                    M154_ALARM_07_TARGET
#define M154_ALARM_07_TARGET_POS                                (0U)
#define M154_ALARM_07_TARGET_LEN                                (32U)
#define M154_ALARM_07_TARGET_MSK                                (((1ULL<<M154_ALARM_07_TARGET_LEN)-1)<<M154_ALARM_07_TARGET_POS)
#define M154_ALARM_07_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_07_TARGET_LEN)-1)<<M154_ALARM_07_TARGET_POS))

/* 0x84 : m154_alarm_08_target */
#define M154_ALARM_08_TARGET_OFFSET                             (0x84)
#define M154_ALARM_08_TARGET                                    M154_ALARM_08_TARGET
#define M154_ALARM_08_TARGET_POS                                (0U)
#define M154_ALARM_08_TARGET_LEN                                (32U)
#define M154_ALARM_08_TARGET_MSK                                (((1ULL<<M154_ALARM_08_TARGET_LEN)-1)<<M154_ALARM_08_TARGET_POS)
#define M154_ALARM_08_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_08_TARGET_LEN)-1)<<M154_ALARM_08_TARGET_POS))

/* 0x88 : m154_alarm_09_target */
#define M154_ALARM_09_TARGET_OFFSET                             (0x88)
#define M154_ALARM_09_TARGET                                    M154_ALARM_09_TARGET
#define M154_ALARM_09_TARGET_POS                                (0U)
#define M154_ALARM_09_TARGET_LEN                                (32U)
#define M154_ALARM_09_TARGET_MSK                                (((1ULL<<M154_ALARM_09_TARGET_LEN)-1)<<M154_ALARM_09_TARGET_POS)
#define M154_ALARM_09_TARGET_UMSK                               (~(((1ULL<<M154_ALARM_09_TARGET_LEN)-1)<<M154_ALARM_09_TARGET_POS))

/* 0x90 : m154_csma_0 */
#define M154_CSMA_0_OFFSET                                      (0x90)
#define M154_CSMA_CA_EN                                         M154_CSMA_CA_EN
#define M154_CSMA_CA_EN_POS                                     (0U)
#define M154_CSMA_CA_EN_LEN                                     (1U)
#define M154_CSMA_CA_EN_MSK                                     (((1ULL<<M154_CSMA_CA_EN_LEN)-1)<<M154_CSMA_CA_EN_POS)
#define M154_CSMA_CA_EN_UMSK                                    (~(((1ULL<<M154_CSMA_CA_EN_LEN)-1)<<M154_CSMA_CA_EN_POS))
#define M154_BLE_MODE                                           M154_BLE_MODE
#define M154_BLE_MODE_POS                                       (1U)
#define M154_BLE_MODE_LEN                                       (1U)
#define M154_BLE_MODE_MSK                                       (((1ULL<<M154_BLE_MODE_LEN)-1)<<M154_BLE_MODE_POS)
#define M154_BLE_MODE_UMSK                                      (~(((1ULL<<M154_BLE_MODE_LEN)-1)<<M154_BLE_MODE_POS))
#define M154_CSMA_CA_EB_MODE                                    M154_CSMA_CA_EB_MODE
#define M154_CSMA_CA_EB_MODE_POS                                (2U)
#define M154_CSMA_CA_EB_MODE_LEN                                (1U)
#define M154_CSMA_CA_EB_MODE_MSK                                (((1ULL<<M154_CSMA_CA_EB_MODE_LEN)-1)<<M154_CSMA_CA_EB_MODE_POS)
#define M154_CSMA_CA_EB_MODE_UMSK                               (~(((1ULL<<M154_CSMA_CA_EB_MODE_LEN)-1)<<M154_CSMA_CA_EB_MODE_POS))
#define M154_CSMA_CA_ECR_MODE                                   M154_CSMA_CA_ECR_MODE
#define M154_CSMA_CA_ECR_MODE_POS                               (3U)
#define M154_CSMA_CA_ECR_MODE_LEN                               (1U)
#define M154_CSMA_CA_ECR_MODE_MSK                               (((1ULL<<M154_CSMA_CA_ECR_MODE_LEN)-1)<<M154_CSMA_CA_ECR_MODE_POS)
#define M154_CSMA_CA_ECR_MODE_UMSK                              (~(((1ULL<<M154_CSMA_CA_ECR_MODE_LEN)-1)<<M154_CSMA_CA_ECR_MODE_POS))
#define M154_CSMA_CA_SLOT_EN                                    M154_CSMA_CA_SLOT_EN
#define M154_CSMA_CA_SLOT_EN_POS                                (4U)
#define M154_CSMA_CA_SLOT_EN_LEN                                (1U)
#define M154_CSMA_CA_SLOT_EN_MSK                                (((1ULL<<M154_CSMA_CA_SLOT_EN_LEN)-1)<<M154_CSMA_CA_SLOT_EN_POS)
#define M154_CSMA_CA_SLOT_EN_UMSK                               (~(((1ULL<<M154_CSMA_CA_SLOT_EN_LEN)-1)<<M154_CSMA_CA_SLOT_EN_POS))
#define M154_CSMA_CA_TIMEOUT_CHECK_OPTION                       M154_CSMA_CA_TIMEOUT_CHECK_OPTION
#define M154_CSMA_CA_TIMEOUT_CHECK_OPTION_POS                   (5U)
#define M154_CSMA_CA_TIMEOUT_CHECK_OPTION_LEN                   (1U)
#define M154_CSMA_CA_TIMEOUT_CHECK_OPTION_MSK                   (((1ULL<<M154_CSMA_CA_TIMEOUT_CHECK_OPTION_LEN)-1)<<M154_CSMA_CA_TIMEOUT_CHECK_OPTION_POS)
#define M154_CSMA_CA_TIMEOUT_CHECK_OPTION_UMSK                  (~(((1ULL<<M154_CSMA_CA_TIMEOUT_CHECK_OPTION_LEN)-1)<<M154_CSMA_CA_TIMEOUT_CHECK_OPTION_POS))
#define M154_CSMA_CA_SW_MODE                                    M154_CSMA_CA_SW_MODE
#define M154_CSMA_CA_SW_MODE_POS                                (6U)
#define M154_CSMA_CA_SW_MODE_LEN                                (1U)
#define M154_CSMA_CA_SW_MODE_MSK                                (((1ULL<<M154_CSMA_CA_SW_MODE_LEN)-1)<<M154_CSMA_CA_SW_MODE_POS)
#define M154_CSMA_CA_SW_MODE_UMSK                               (~(((1ULL<<M154_CSMA_CA_SW_MODE_LEN)-1)<<M154_CSMA_CA_SW_MODE_POS))
#define M154_CW0_SEL                                            M154_CW0_SEL
#define M154_CW0_SEL_POS                                        (8U)
#define M154_CW0_SEL_LEN                                        (2U)
#define M154_CW0_SEL_MSK                                        (((1ULL<<M154_CW0_SEL_LEN)-1)<<M154_CW0_SEL_POS)
#define M154_CW0_SEL_UMSK                                       (~(((1ULL<<M154_CW0_SEL_LEN)-1)<<M154_CW0_SEL_POS))
#define M154_CSMA_CA_CCA_EN_OFFSET                              M154_CSMA_CA_CCA_EN_OFFSET
#define M154_CSMA_CA_CCA_EN_OFFSET_POS                          (12U)
#define M154_CSMA_CA_CCA_EN_OFFSET_LEN                          (4U)
#define M154_CSMA_CA_CCA_EN_OFFSET_MSK                          (((1ULL<<M154_CSMA_CA_CCA_EN_OFFSET_LEN)-1)<<M154_CSMA_CA_CCA_EN_OFFSET_POS)
#define M154_CSMA_CA_CCA_EN_OFFSET_UMSK                         (~(((1ULL<<M154_CSMA_CA_CCA_EN_OFFSET_LEN)-1)<<M154_CSMA_CA_CCA_EN_OFFSET_POS))
#define M154_CSMA_CA_TRANSACTION_TIME                           M154_CSMA_CA_TRANSACTION_TIME
#define M154_CSMA_CA_TRANSACTION_TIME_POS                       (16U)
#define M154_CSMA_CA_TRANSACTION_TIME_LEN                       (9U)
#define M154_CSMA_CA_TRANSACTION_TIME_MSK                       (((1ULL<<M154_CSMA_CA_TRANSACTION_TIME_LEN)-1)<<M154_CSMA_CA_TRANSACTION_TIME_POS)
#define M154_CSMA_CA_TRANSACTION_TIME_UMSK                      (~(((1ULL<<M154_CSMA_CA_TRANSACTION_TIME_LEN)-1)<<M154_CSMA_CA_TRANSACTION_TIME_POS))
#define M154_CSMA_CA_CCA_TIMEOUT_SEL                            M154_CSMA_CA_CCA_TIMEOUT_SEL
#define M154_CSMA_CA_CCA_TIMEOUT_SEL_POS                        (28U)
#define M154_CSMA_CA_CCA_TIMEOUT_SEL_LEN                        (1U)
#define M154_CSMA_CA_CCA_TIMEOUT_SEL_MSK                        (((1ULL<<M154_CSMA_CA_CCA_TIMEOUT_SEL_LEN)-1)<<M154_CSMA_CA_CCA_TIMEOUT_SEL_POS)
#define M154_CSMA_CA_CCA_TIMEOUT_SEL_UMSK                       (~(((1ULL<<M154_CSMA_CA_CCA_TIMEOUT_SEL_LEN)-1)<<M154_CSMA_CA_CCA_TIMEOUT_SEL_POS))
#define M154_BACKOFF_RX_DIS                                     M154_BACKOFF_RX_DIS
#define M154_BACKOFF_RX_DIS_POS                                 (29U)
#define M154_BACKOFF_RX_DIS_LEN                                 (1U)
#define M154_BACKOFF_RX_DIS_MSK                                 (((1ULL<<M154_BACKOFF_RX_DIS_LEN)-1)<<M154_BACKOFF_RX_DIS_POS)
#define M154_BACKOFF_RX_DIS_UMSK                                (~(((1ULL<<M154_BACKOFF_RX_DIS_LEN)-1)<<M154_BACKOFF_RX_DIS_POS))

/* 0x94 : m154_csma_1 */
#define M154_CSMA_1_OFFSET                                      (0x94)
#define M154_MANUAL_BE                                          M154_MANUAL_BE
#define M154_MANUAL_BE_POS                                      (0U)
#define M154_MANUAL_BE_LEN                                      (4U)
#define M154_MANUAL_BE_MSK                                      (((1ULL<<M154_MANUAL_BE_LEN)-1)<<M154_MANUAL_BE_POS)
#define M154_MANUAL_BE_UMSK                                     (~(((1ULL<<M154_MANUAL_BE_LEN)-1)<<M154_MANUAL_BE_POS))
#define M154_MANUAL_BE_SEL                                      M154_MANUAL_BE_SEL
#define M154_MANUAL_BE_SEL_POS                                  (4U)
#define M154_MANUAL_BE_SEL_LEN                                  (1U)
#define M154_MANUAL_BE_SEL_MSK                                  (((1ULL<<M154_MANUAL_BE_SEL_LEN)-1)<<M154_MANUAL_BE_SEL_POS)
#define M154_MANUAL_BE_SEL_UMSK                                 (~(((1ULL<<M154_MANUAL_BE_SEL_LEN)-1)<<M154_MANUAL_BE_SEL_POS))
#define M154_MINUS_BE                                           M154_MINUS_BE
#define M154_MINUS_BE_POS                                       (8U)
#define M154_MINUS_BE_LEN                                       (3U)
#define M154_MINUS_BE_MSK                                       (((1ULL<<M154_MINUS_BE_LEN)-1)<<M154_MINUS_BE_POS)
#define M154_MINUS_BE_UMSK                                      (~(((1ULL<<M154_MINUS_BE_LEN)-1)<<M154_MINUS_BE_POS))
#define M154_PLUS_BE                                            M154_PLUS_BE
#define M154_PLUS_BE_POS                                        (12U)
#define M154_PLUS_BE_LEN                                        (3U)
#define M154_PLUS_BE_MSK                                        (((1ULL<<M154_PLUS_BE_LEN)-1)<<M154_PLUS_BE_POS)
#define M154_PLUS_BE_UMSK                                       (~(((1ULL<<M154_PLUS_BE_LEN)-1)<<M154_PLUS_BE_POS))
#define M154_MIN_BE                                             M154_MIN_BE
#define M154_MIN_BE_POS                                         (16U)
#define M154_MIN_BE_LEN                                         (4U)
#define M154_MIN_BE_MSK                                         (((1ULL<<M154_MIN_BE_LEN)-1)<<M154_MIN_BE_POS)
#define M154_MIN_BE_UMSK                                        (~(((1ULL<<M154_MIN_BE_LEN)-1)<<M154_MIN_BE_POS))
#define M154_MAX_BE                                             M154_MAX_BE
#define M154_MAX_BE_POS                                         (20U)
#define M154_MAX_BE_LEN                                         (4U)
#define M154_MAX_BE_MSK                                         (((1ULL<<M154_MAX_BE_LEN)-1)<<M154_MAX_BE_POS)
#define M154_MAX_BE_UMSK                                        (~(((1ULL<<M154_MAX_BE_LEN)-1)<<M154_MAX_BE_POS))
#define M154_MAX_CSMA_BACKOFFS                                  M154_MAX_CSMA_BACKOFFS
#define M154_MAX_CSMA_BACKOFFS_POS                              (24U)
#define M154_MAX_CSMA_BACKOFFS_LEN                              (3U)
#define M154_MAX_CSMA_BACKOFFS_MSK                              (((1ULL<<M154_MAX_CSMA_BACKOFFS_LEN)-1)<<M154_MAX_CSMA_BACKOFFS_POS)
#define M154_MAX_CSMA_BACKOFFS_UMSK                             (~(((1ULL<<M154_MAX_CSMA_BACKOFFS_LEN)-1)<<M154_MAX_CSMA_BACKOFFS_POS))
#define M154_CSMA_CA_AVG_PKT_LEN                                M154_CSMA_CA_AVG_PKT_LEN
#define M154_CSMA_CA_AVG_PKT_LEN_POS                            (28U)
#define M154_CSMA_CA_AVG_PKT_LEN_LEN                            (4U)
#define M154_CSMA_CA_AVG_PKT_LEN_MSK                            (((1ULL<<M154_CSMA_CA_AVG_PKT_LEN_LEN)-1)<<M154_CSMA_CA_AVG_PKT_LEN_POS)
#define M154_CSMA_CA_AVG_PKT_LEN_UMSK                           (~(((1ULL<<M154_CSMA_CA_AVG_PKT_LEN_LEN)-1)<<M154_CSMA_CA_AVG_PKT_LEN_POS))

/* 0x98 : m154_csma_2 */
#define M154_CSMA_2_OFFSET                                      (0x98)
#define M154_CSMA_CA_CAP_END                                    M154_CSMA_CA_CAP_END
#define M154_CSMA_CA_CAP_END_POS                                (0U)
#define M154_CSMA_CA_CAP_END_LEN                                (32U)
#define M154_CSMA_CA_CAP_END_MSK                                (((1ULL<<M154_CSMA_CA_CAP_END_LEN)-1)<<M154_CSMA_CA_CAP_END_POS)
#define M154_CSMA_CA_CAP_END_UMSK                               (~(((1ULL<<M154_CSMA_CA_CAP_END_LEN)-1)<<M154_CSMA_CA_CAP_END_POS))

/* 0x9C : m154_csma_3 */
#define M154_CSMA_3_OFFSET                                      (0x9C)
#define M154_CSMA_CA_BLE_PERIOD_END                             M154_CSMA_CA_BLE_PERIOD_END
#define M154_CSMA_CA_BLE_PERIOD_END_POS                         (0U)
#define M154_CSMA_CA_BLE_PERIOD_END_LEN                         (32U)
#define M154_CSMA_CA_BLE_PERIOD_END_MSK                         (((1ULL<<M154_CSMA_CA_BLE_PERIOD_END_LEN)-1)<<M154_CSMA_CA_BLE_PERIOD_END_POS)
#define M154_CSMA_CA_BLE_PERIOD_END_UMSK                        (~(((1ULL<<M154_CSMA_CA_BLE_PERIOD_END_LEN)-1)<<M154_CSMA_CA_BLE_PERIOD_END_POS))

/* 0xA0 : m154_csma_4 */
#define M154_CSMA_4_OFFSET                                      (0xA0)
#define M154_CSMA_CA_FSM_CURR                                   M154_CSMA_CA_FSM_CURR
#define M154_CSMA_CA_FSM_CURR_POS                               (0U)
#define M154_CSMA_CA_FSM_CURR_LEN                               (4U)
#define M154_CSMA_CA_FSM_CURR_MSK                               (((1ULL<<M154_CSMA_CA_FSM_CURR_LEN)-1)<<M154_CSMA_CA_FSM_CURR_POS)
#define M154_CSMA_CA_FSM_CURR_UMSK                              (~(((1ULL<<M154_CSMA_CA_FSM_CURR_LEN)-1)<<M154_CSMA_CA_FSM_CURR_POS))
#define M154_CSMA_CA_BE_CURR                                    M154_CSMA_CA_BE_CURR
#define M154_CSMA_CA_BE_CURR_POS                                (8U)
#define M154_CSMA_CA_BE_CURR_LEN                                (4U)
#define M154_CSMA_CA_BE_CURR_MSK                                (((1ULL<<M154_CSMA_CA_BE_CURR_LEN)-1)<<M154_CSMA_CA_BE_CURR_POS)
#define M154_CSMA_CA_BE_CURR_UMSK                               (~(((1ULL<<M154_CSMA_CA_BE_CURR_LEN)-1)<<M154_CSMA_CA_BE_CURR_POS))

/* 0xB0 : m154_tx_0 */
#define M154_TX_0_OFFSET                                        (0xB0)
#define M154_TX_SN_FOR_RX_ACK_JUDGE                             M154_TX_SN_FOR_RX_ACK_JUDGE
#define M154_TX_SN_FOR_RX_ACK_JUDGE_POS                         (0U)
#define M154_TX_SN_FOR_RX_ACK_JUDGE_LEN                         (8U)
#define M154_TX_SN_FOR_RX_ACK_JUDGE_MSK                         (((1ULL<<M154_TX_SN_FOR_RX_ACK_JUDGE_LEN)-1)<<M154_TX_SN_FOR_RX_ACK_JUDGE_POS)
#define M154_TX_SN_FOR_RX_ACK_JUDGE_UMSK                        (~(((1ULL<<M154_TX_SN_FOR_RX_ACK_JUDGE_LEN)-1)<<M154_TX_SN_FOR_RX_ACK_JUDGE_POS))
#define M154_TX_MPDU_AES_DDT_PHR_FLAG                           M154_TX_MPDU_AES_DDT_PHR_FLAG
#define M154_TX_MPDU_AES_DDT_PHR_FLAG_POS                       (8U)
#define M154_TX_MPDU_AES_DDT_PHR_FLAG_LEN                       (1U)
#define M154_TX_MPDU_AES_DDT_PHR_FLAG_MSK                       (((1ULL<<M154_TX_MPDU_AES_DDT_PHR_FLAG_LEN)-1)<<M154_TX_MPDU_AES_DDT_PHR_FLAG_POS)
#define M154_TX_MPDU_AES_DDT_PHR_FLAG_UMSK                      (~(((1ULL<<M154_TX_MPDU_AES_DDT_PHR_FLAG_LEN)-1)<<M154_TX_MPDU_AES_DDT_PHR_FLAG_POS))
#define M154_TX_MPDU_AES_DDT_DONE_FLAG                          M154_TX_MPDU_AES_DDT_DONE_FLAG
#define M154_TX_MPDU_AES_DDT_DONE_FLAG_POS                      (9U)
#define M154_TX_MPDU_AES_DDT_DONE_FLAG_LEN                      (1U)
#define M154_TX_MPDU_AES_DDT_DONE_FLAG_MSK                      (((1ULL<<M154_TX_MPDU_AES_DDT_DONE_FLAG_LEN)-1)<<M154_TX_MPDU_AES_DDT_DONE_FLAG_POS)
#define M154_TX_MPDU_AES_DDT_DONE_FLAG_UMSK                     (~(((1ULL<<M154_TX_MPDU_AES_DDT_DONE_FLAG_LEN)-1)<<M154_TX_MPDU_AES_DDT_DONE_FLAG_POS))
#define M154_TX_DOING                                           M154_TX_DOING
#define M154_TX_DOING_POS                                       (10U)
#define M154_TX_DOING_LEN                                       (1U)
#define M154_TX_DOING_MSK                                       (((1ULL<<M154_TX_DOING_LEN)-1)<<M154_TX_DOING_POS)
#define M154_TX_DOING_UMSK                                      (~(((1ULL<<M154_TX_DOING_LEN)-1)<<M154_TX_DOING_POS))
#define M154_TX_AUTO_ACK_DOING                                  M154_TX_AUTO_ACK_DOING
#define M154_TX_AUTO_ACK_DOING_POS                              (11U)
#define M154_TX_AUTO_ACK_DOING_LEN                              (1U)
#define M154_TX_AUTO_ACK_DOING_MSK                              (((1ULL<<M154_TX_AUTO_ACK_DOING_LEN)-1)<<M154_TX_AUTO_ACK_DOING_POS)
#define M154_TX_AUTO_ACK_DOING_UMSK                             (~(((1ULL<<M154_TX_AUTO_ACK_DOING_LEN)-1)<<M154_TX_AUTO_ACK_DOING_POS))
#define M154_TX_MPDU_SYM_CNT_ACC                                M154_TX_MPDU_SYM_CNT_ACC
#define M154_TX_MPDU_SYM_CNT_ACC_POS                            (16U)
#define M154_TX_MPDU_SYM_CNT_ACC_LEN                            (9U)
#define M154_TX_MPDU_SYM_CNT_ACC_MSK                            (((1ULL<<M154_TX_MPDU_SYM_CNT_ACC_LEN)-1)<<M154_TX_MPDU_SYM_CNT_ACC_POS)
#define M154_TX_MPDU_SYM_CNT_ACC_UMSK                           (~(((1ULL<<M154_TX_MPDU_SYM_CNT_ACC_LEN)-1)<<M154_TX_MPDU_SYM_CNT_ACC_POS))

/* 0xB4 : m154_tx_1 */
#define M154_TX_1_OFFSET                                        (0xB4)
#define M154_TX_TRIG_CMD                                        M154_TX_TRIG_CMD
#define M154_TX_TRIG_CMD_POS                                    (0U)
#define M154_TX_TRIG_CMD_LEN                                    (1U)
#define M154_TX_TRIG_CMD_MSK                                    (((1ULL<<M154_TX_TRIG_CMD_LEN)-1)<<M154_TX_TRIG_CMD_POS)
#define M154_TX_TRIG_CMD_UMSK                                   (~(((1ULL<<M154_TX_TRIG_CMD_LEN)-1)<<M154_TX_TRIG_CMD_POS))
#define M154_TX_MPDU_SRC_SEL                                    M154_TX_MPDU_SRC_SEL
#define M154_TX_MPDU_SRC_SEL_POS                                (1U)
#define M154_TX_MPDU_SRC_SEL_LEN                                (1U)
#define M154_TX_MPDU_SRC_SEL_MSK                                (((1ULL<<M154_TX_MPDU_SRC_SEL_LEN)-1)<<M154_TX_MPDU_SRC_SEL_POS)
#define M154_TX_MPDU_SRC_SEL_UMSK                               (~(((1ULL<<M154_TX_MPDU_SRC_SEL_LEN)-1)<<M154_TX_MPDU_SRC_SEL_POS))
#define M154_TX_TRIG_TIME_SEL                                   M154_TX_TRIG_TIME_SEL
#define M154_TX_TRIG_TIME_SEL_POS                               (2U)
#define M154_TX_TRIG_TIME_SEL_LEN                               (1U)
#define M154_TX_TRIG_TIME_SEL_MSK                               (((1ULL<<M154_TX_TRIG_TIME_SEL_LEN)-1)<<M154_TX_TRIG_TIME_SEL_POS)
#define M154_TX_TRIG_TIME_SEL_UMSK                              (~(((1ULL<<M154_TX_TRIG_TIME_SEL_LEN)-1)<<M154_TX_TRIG_TIME_SEL_POS))
#define M154_TX_MPDU_AES_DDT_WAIT_SEL                           M154_TX_MPDU_AES_DDT_WAIT_SEL
#define M154_TX_MPDU_AES_DDT_WAIT_SEL_POS                       (3U)
#define M154_TX_MPDU_AES_DDT_WAIT_SEL_LEN                       (1U)
#define M154_TX_MPDU_AES_DDT_WAIT_SEL_MSK                       (((1ULL<<M154_TX_MPDU_AES_DDT_WAIT_SEL_LEN)-1)<<M154_TX_MPDU_AES_DDT_WAIT_SEL_POS)
#define M154_TX_MPDU_AES_DDT_WAIT_SEL_UMSK                      (~(((1ULL<<M154_TX_MPDU_AES_DDT_WAIT_SEL_LEN)-1)<<M154_TX_MPDU_AES_DDT_WAIT_SEL_POS))
#define M154_TX_AUTO_ACK_EN                                     M154_TX_AUTO_ACK_EN
#define M154_TX_AUTO_ACK_EN_POS                                 (4U)
#define M154_TX_AUTO_ACK_EN_LEN                                 (1U)
#define M154_TX_AUTO_ACK_EN_MSK                                 (((1ULL<<M154_TX_AUTO_ACK_EN_LEN)-1)<<M154_TX_AUTO_ACK_EN_POS)
#define M154_TX_AUTO_ACK_EN_UMSK                                (~(((1ULL<<M154_TX_AUTO_ACK_EN_LEN)-1)<<M154_TX_AUTO_ACK_EN_POS))
#define M154_TX_AUTO_ACK_FP_BIT                                 M154_TX_AUTO_ACK_FP_BIT
#define M154_TX_AUTO_ACK_FP_BIT_POS                             (5U)
#define M154_TX_AUTO_ACK_FP_BIT_LEN                             (1U)
#define M154_TX_AUTO_ACK_FP_BIT_MSK                             (((1ULL<<M154_TX_AUTO_ACK_FP_BIT_LEN)-1)<<M154_TX_AUTO_ACK_FP_BIT_POS)
#define M154_TX_AUTO_ACK_FP_BIT_UMSK                            (~(((1ULL<<M154_TX_AUTO_ACK_FP_BIT_LEN)-1)<<M154_TX_AUTO_ACK_FP_BIT_POS))
#define M154_TX_AUTO_ACK_FP_FROM_SW                             M154_TX_AUTO_ACK_FP_FROM_SW
#define M154_TX_AUTO_ACK_FP_FROM_SW_POS                         (6U)
#define M154_TX_AUTO_ACK_FP_FROM_SW_LEN                         (1U)
#define M154_TX_AUTO_ACK_FP_FROM_SW_MSK                         (((1ULL<<M154_TX_AUTO_ACK_FP_FROM_SW_LEN)-1)<<M154_TX_AUTO_ACK_FP_FROM_SW_POS)
#define M154_TX_AUTO_ACK_FP_FROM_SW_UMSK                        (~(((1ULL<<M154_TX_AUTO_ACK_FP_FROM_SW_LEN)-1)<<M154_TX_AUTO_ACK_FP_FROM_SW_POS))
#define M154_TX_INFINITE_EN                                     M154_TX_INFINITE_EN
#define M154_TX_INFINITE_EN_POS                                 (7U)
#define M154_TX_INFINITE_EN_LEN                                 (1U)
#define M154_TX_INFINITE_EN_MSK                                 (((1ULL<<M154_TX_INFINITE_EN_LEN)-1)<<M154_TX_INFINITE_EN_POS)
#define M154_TX_INFINITE_EN_UMSK                                (~(((1ULL<<M154_TX_INFINITE_EN_LEN)-1)<<M154_TX_INFINITE_EN_POS))
#define M154_TX_START_OFFSET                                    M154_TX_START_OFFSET
#define M154_TX_START_OFFSET_POS                                (8U)
#define M154_TX_START_OFFSET_LEN                                (8U)
#define M154_TX_START_OFFSET_MSK                                (((1ULL<<M154_TX_START_OFFSET_LEN)-1)<<M154_TX_START_OFFSET_POS)
#define M154_TX_START_OFFSET_UMSK                               (~(((1ULL<<M154_TX_START_OFFSET_LEN)-1)<<M154_TX_START_OFFSET_POS))
#define M154_TX_POSTPONE_BY_RX_DOING_EN                         M154_TX_POSTPONE_BY_RX_DOING_EN
#define M154_TX_POSTPONE_BY_RX_DOING_EN_POS                     (16U)
#define M154_TX_POSTPONE_BY_RX_DOING_EN_LEN                     (1U)
#define M154_TX_POSTPONE_BY_RX_DOING_EN_MSK                     (((1ULL<<M154_TX_POSTPONE_BY_RX_DOING_EN_LEN)-1)<<M154_TX_POSTPONE_BY_RX_DOING_EN_POS)
#define M154_TX_POSTPONE_BY_RX_DOING_EN_UMSK                    (~(((1ULL<<M154_TX_POSTPONE_BY_RX_DOING_EN_LEN)-1)<<M154_TX_POSTPONE_BY_RX_DOING_EN_POS))

/* 0xB8 : m154_tx_2 */
#define M154_TX_2_OFFSET                                        (0xB8)
#define M154_TX_TRIG_TIME                                       M154_TX_TRIG_TIME
#define M154_TX_TRIG_TIME_POS                                   (0U)
#define M154_TX_TRIG_TIME_LEN                                   (32U)
#define M154_TX_TRIG_TIME_MSK                                   (((1ULL<<M154_TX_TRIG_TIME_LEN)-1)<<M154_TX_TRIG_TIME_POS)
#define M154_TX_TRIG_TIME_UMSK                                  (~(((1ULL<<M154_TX_TRIG_TIME_LEN)-1)<<M154_TX_TRIG_TIME_POS))

/* 0xBC : m154_tx_3 */
#define M154_TX_3_OFFSET                                        (0xBC)
#define M154_TX_FSM_CURR                                        M154_TX_FSM_CURR
#define M154_TX_FSM_CURR_POS                                    (0U)
#define M154_TX_FSM_CURR_LEN                                    (11U)
#define M154_TX_FSM_CURR_MSK                                    (((1ULL<<M154_TX_FSM_CURR_LEN)-1)<<M154_TX_FSM_CURR_POS)
#define M154_TX_FSM_CURR_UMSK                                   (~(((1ULL<<M154_TX_FSM_CURR_LEN)-1)<<M154_TX_FSM_CURR_POS))
#define M154_RX_AFTER_TX_EN                                     M154_RX_AFTER_TX_EN
#define M154_RX_AFTER_TX_EN_POS                                 (16U)
#define M154_RX_AFTER_TX_EN_LEN                                 (1U)
#define M154_RX_AFTER_TX_EN_MSK                                 (((1ULL<<M154_RX_AFTER_TX_EN_LEN)-1)<<M154_RX_AFTER_TX_EN_POS)
#define M154_RX_AFTER_TX_EN_UMSK                                (~(((1ULL<<M154_RX_AFTER_TX_EN_LEN)-1)<<M154_RX_AFTER_TX_EN_POS))
#define M154_TX_RX_TRANSITION_TIME                              M154_TX_RX_TRANSITION_TIME
#define M154_TX_RX_TRANSITION_TIME_POS                          (24U)
#define M154_TX_RX_TRANSITION_TIME_LEN                          (8U)
#define M154_TX_RX_TRANSITION_TIME_MSK                          (((1ULL<<M154_TX_RX_TRANSITION_TIME_LEN)-1)<<M154_TX_RX_TRANSITION_TIME_POS)
#define M154_TX_RX_TRANSITION_TIME_UMSK                         (~(((1ULL<<M154_TX_RX_TRANSITION_TIME_LEN)-1)<<M154_TX_RX_TRANSITION_TIME_POS))

/* 0xC0 : m154_rx_00 */
#define M154_RX_00_OFFSET                                       (0xC0)
#define M154_RX_TX_TRANSITION_TIME                              M154_RX_TX_TRANSITION_TIME
#define M154_RX_TX_TRANSITION_TIME_POS                          (0U)
#define M154_RX_TX_TRANSITION_TIME_LEN                          (8U)
#define M154_RX_TX_TRANSITION_TIME_MSK                          (((1ULL<<M154_RX_TX_TRANSITION_TIME_LEN)-1)<<M154_RX_TX_TRANSITION_TIME_POS)
#define M154_RX_TX_TRANSITION_TIME_UMSK                         (~(((1ULL<<M154_RX_TX_TRANSITION_TIME_LEN)-1)<<M154_RX_TX_TRANSITION_TIME_POS))
#define M154_RX_MPDU_AES_DDT_TRIG_EN                            M154_RX_MPDU_AES_DDT_TRIG_EN
#define M154_RX_MPDU_AES_DDT_TRIG_EN_POS                        (8U)
#define M154_RX_MPDU_AES_DDT_TRIG_EN_LEN                        (1U)
#define M154_RX_MPDU_AES_DDT_TRIG_EN_MSK                        (((1ULL<<M154_RX_MPDU_AES_DDT_TRIG_EN_LEN)-1)<<M154_RX_MPDU_AES_DDT_TRIG_EN_POS)
#define M154_RX_MPDU_AES_DDT_TRIG_EN_UMSK                       (~(((1ULL<<M154_RX_MPDU_AES_DDT_TRIG_EN_LEN)-1)<<M154_RX_MPDU_AES_DDT_TRIG_EN_POS))
#define M154_RX_MPDU_AES_DDT_TRIG                               M154_RX_MPDU_AES_DDT_TRIG
#define M154_RX_MPDU_AES_DDT_TRIG_POS                           (12U)
#define M154_RX_MPDU_AES_DDT_TRIG_LEN                           (1U)
#define M154_RX_MPDU_AES_DDT_TRIG_MSK                           (((1ULL<<M154_RX_MPDU_AES_DDT_TRIG_LEN)-1)<<M154_RX_MPDU_AES_DDT_TRIG_POS)
#define M154_RX_MPDU_AES_DDT_TRIG_UMSK                          (~(((1ULL<<M154_RX_MPDU_AES_DDT_TRIG_LEN)-1)<<M154_RX_MPDU_AES_DDT_TRIG_POS))
#define M154_RX_DOING                                           M154_RX_DOING
#define M154_RX_DOING_POS                                       (16U)
#define M154_RX_DOING_LEN                                       (1U)
#define M154_RX_DOING_MSK                                       (((1ULL<<M154_RX_DOING_LEN)-1)<<M154_RX_DOING_POS)
#define M154_RX_DOING_UMSK                                      (~(((1ULL<<M154_RX_DOING_LEN)-1)<<M154_RX_DOING_POS))
#define M154_RX_MPDU_SYM_CNT_ACC                                M154_RX_MPDU_SYM_CNT_ACC
#define M154_RX_MPDU_SYM_CNT_ACC_POS                            (20U)
#define M154_RX_MPDU_SYM_CNT_ACC_LEN                            (9U)
#define M154_RX_MPDU_SYM_CNT_ACC_MSK                            (((1ULL<<M154_RX_MPDU_SYM_CNT_ACC_LEN)-1)<<M154_RX_MPDU_SYM_CNT_ACC_POS)
#define M154_RX_MPDU_SYM_CNT_ACC_UMSK                           (~(((1ULL<<M154_RX_MPDU_SYM_CNT_ACC_LEN)-1)<<M154_RX_MPDU_SYM_CNT_ACC_POS))

/* 0xC4 : m154_rx_01 */
#define M154_RX_01_OFFSET                                       (0xC4)
#define M154_RX_CRC_FAIL                                        M154_RX_CRC_FAIL
#define M154_RX_CRC_FAIL_POS                                    (0U)
#define M154_RX_CRC_FAIL_LEN                                    (1U)
#define M154_RX_CRC_FAIL_MSK                                    (((1ULL<<M154_RX_CRC_FAIL_LEN)-1)<<M154_RX_CRC_FAIL_POS)
#define M154_RX_CRC_FAIL_UMSK                                   (~(((1ULL<<M154_RX_CRC_FAIL_LEN)-1)<<M154_RX_CRC_FAIL_POS))
#define M154_RX_FRAME_TYPE_FAIL                                 M154_RX_FRAME_TYPE_FAIL
#define M154_RX_FRAME_TYPE_FAIL_POS                             (1U)
#define M154_RX_FRAME_TYPE_FAIL_LEN                             (1U)
#define M154_RX_FRAME_TYPE_FAIL_MSK                             (((1ULL<<M154_RX_FRAME_TYPE_FAIL_LEN)-1)<<M154_RX_FRAME_TYPE_FAIL_POS)
#define M154_RX_FRAME_TYPE_FAIL_UMSK                            (~(((1ULL<<M154_RX_FRAME_TYPE_FAIL_LEN)-1)<<M154_RX_FRAME_TYPE_FAIL_POS))
#define M154_RX_FRAME_VERSION_FAIL                              M154_RX_FRAME_VERSION_FAIL
#define M154_RX_FRAME_VERSION_FAIL_POS                          (2U)
#define M154_RX_FRAME_VERSION_FAIL_LEN                          (1U)
#define M154_RX_FRAME_VERSION_FAIL_MSK                          (((1ULL<<M154_RX_FRAME_VERSION_FAIL_LEN)-1)<<M154_RX_FRAME_VERSION_FAIL_POS)
#define M154_RX_FRAME_VERSION_FAIL_UMSK                         (~(((1ULL<<M154_RX_FRAME_VERSION_FAIL_LEN)-1)<<M154_RX_FRAME_VERSION_FAIL_POS))
#define M154_RX_DST_ADDR_MODE_FAIL                              M154_RX_DST_ADDR_MODE_FAIL
#define M154_RX_DST_ADDR_MODE_FAIL_POS                          (3U)
#define M154_RX_DST_ADDR_MODE_FAIL_LEN                          (1U)
#define M154_RX_DST_ADDR_MODE_FAIL_MSK                          (((1ULL<<M154_RX_DST_ADDR_MODE_FAIL_LEN)-1)<<M154_RX_DST_ADDR_MODE_FAIL_POS)
#define M154_RX_DST_ADDR_MODE_FAIL_UMSK                         (~(((1ULL<<M154_RX_DST_ADDR_MODE_FAIL_LEN)-1)<<M154_RX_DST_ADDR_MODE_FAIL_POS))
#define M154_RX_SRC_ADDR_MODE_FAIL                              M154_RX_SRC_ADDR_MODE_FAIL
#define M154_RX_SRC_ADDR_MODE_FAIL_POS                          (4U)
#define M154_RX_SRC_ADDR_MODE_FAIL_LEN                          (1U)
#define M154_RX_SRC_ADDR_MODE_FAIL_MSK                          (((1ULL<<M154_RX_SRC_ADDR_MODE_FAIL_LEN)-1)<<M154_RX_SRC_ADDR_MODE_FAIL_POS)
#define M154_RX_SRC_ADDR_MODE_FAIL_UMSK                         (~(((1ULL<<M154_RX_SRC_ADDR_MODE_FAIL_LEN)-1)<<M154_RX_SRC_ADDR_MODE_FAIL_POS))
#define M154_RX_DST_PAN_ID_FAIL                                 M154_RX_DST_PAN_ID_FAIL
#define M154_RX_DST_PAN_ID_FAIL_POS                             (5U)
#define M154_RX_DST_PAN_ID_FAIL_LEN                             (1U)
#define M154_RX_DST_PAN_ID_FAIL_MSK                             (((1ULL<<M154_RX_DST_PAN_ID_FAIL_LEN)-1)<<M154_RX_DST_PAN_ID_FAIL_POS)
#define M154_RX_DST_PAN_ID_FAIL_UMSK                            (~(((1ULL<<M154_RX_DST_PAN_ID_FAIL_LEN)-1)<<M154_RX_DST_PAN_ID_FAIL_POS))
#define M154_RX_DST_ADDR_FAIL                                   M154_RX_DST_ADDR_FAIL
#define M154_RX_DST_ADDR_FAIL_POS                               (6U)
#define M154_RX_DST_ADDR_FAIL_LEN                               (1U)
#define M154_RX_DST_ADDR_FAIL_MSK                               (((1ULL<<M154_RX_DST_ADDR_FAIL_LEN)-1)<<M154_RX_DST_ADDR_FAIL_POS)
#define M154_RX_DST_ADDR_FAIL_UMSK                              (~(((1ULL<<M154_RX_DST_ADDR_FAIL_LEN)-1)<<M154_RX_DST_ADDR_FAIL_POS))
#define M154_RX_SRC_PAN_ID_FAIL                                 M154_RX_SRC_PAN_ID_FAIL
#define M154_RX_SRC_PAN_ID_FAIL_POS                             (7U)
#define M154_RX_SRC_PAN_ID_FAIL_LEN                             (1U)
#define M154_RX_SRC_PAN_ID_FAIL_MSK                             (((1ULL<<M154_RX_SRC_PAN_ID_FAIL_LEN)-1)<<M154_RX_SRC_PAN_ID_FAIL_POS)
#define M154_RX_SRC_PAN_ID_FAIL_UMSK                            (~(((1ULL<<M154_RX_SRC_PAN_ID_FAIL_LEN)-1)<<M154_RX_SRC_PAN_ID_FAIL_POS))
#define M154_RX_BCN_PAN_ID_FAIL                                 M154_RX_BCN_PAN_ID_FAIL
#define M154_RX_BCN_PAN_ID_FAIL_POS                             (8U)
#define M154_RX_BCN_PAN_ID_FAIL_LEN                             (1U)
#define M154_RX_BCN_PAN_ID_FAIL_MSK                             (((1ULL<<M154_RX_BCN_PAN_ID_FAIL_LEN)-1)<<M154_RX_BCN_PAN_ID_FAIL_POS)
#define M154_RX_BCN_PAN_ID_FAIL_UMSK                            (~(((1ULL<<M154_RX_BCN_PAN_ID_FAIL_LEN)-1)<<M154_RX_BCN_PAN_ID_FAIL_POS))
#define M154_STACK1_FAIL                                        M154_STACK1_FAIL
#define M154_STACK1_FAIL_POS                                    (9U)
#define M154_STACK1_FAIL_LEN                                    (1U)
#define M154_STACK1_FAIL_MSK                                    (((1ULL<<M154_STACK1_FAIL_LEN)-1)<<M154_STACK1_FAIL_POS)
#define M154_STACK1_FAIL_UMSK                                   (~(((1ULL<<M154_STACK1_FAIL_LEN)-1)<<M154_STACK1_FAIL_POS))
#define M154_STACK2_FAIL                                        M154_STACK2_FAIL
#define M154_STACK2_FAIL_POS                                    (10U)
#define M154_STACK2_FAIL_LEN                                    (1U)
#define M154_STACK2_FAIL_MSK                                    (((1ULL<<M154_STACK2_FAIL_LEN)-1)<<M154_STACK2_FAIL_POS)
#define M154_STACK2_FAIL_UMSK                                   (~(((1ULL<<M154_STACK2_FAIL_LEN)-1)<<M154_STACK2_FAIL_POS))
#define M154_RX_PHR_ERR_UV                                      M154_RX_PHR_ERR_UV
#define M154_RX_PHR_ERR_UV_POS                                  (16U)
#define M154_RX_PHR_ERR_UV_LEN                                  (1U)
#define M154_RX_PHR_ERR_UV_MSK                                  (((1ULL<<M154_RX_PHR_ERR_UV_LEN)-1)<<M154_RX_PHR_ERR_UV_POS)
#define M154_RX_PHR_ERR_UV_UMSK                                 (~(((1ULL<<M154_RX_PHR_ERR_UV_LEN)-1)<<M154_RX_PHR_ERR_UV_POS))
#define M154_RX_PHR_ERR_UV_CLR                                  M154_RX_PHR_ERR_UV_CLR
#define M154_RX_PHR_ERR_UV_CLR_POS                              (17U)
#define M154_RX_PHR_ERR_UV_CLR_LEN                              (1U)
#define M154_RX_PHR_ERR_UV_CLR_MSK                              (((1ULL<<M154_RX_PHR_ERR_UV_CLR_LEN)-1)<<M154_RX_PHR_ERR_UV_CLR_POS)
#define M154_RX_PHR_ERR_UV_CLR_UMSK                             (~(((1ULL<<M154_RX_PHR_ERR_UV_CLR_LEN)-1)<<M154_RX_PHR_ERR_UV_CLR_POS))
#define M154_RX_PHR_ERR_FILTER_FAIL_EN                          M154_RX_PHR_ERR_FILTER_FAIL_EN
#define M154_RX_PHR_ERR_FILTER_FAIL_EN_POS                      (24U)
#define M154_RX_PHR_ERR_FILTER_FAIL_EN_LEN                      (1U)
#define M154_RX_PHR_ERR_FILTER_FAIL_EN_MSK                      (((1ULL<<M154_RX_PHR_ERR_FILTER_FAIL_EN_LEN)-1)<<M154_RX_PHR_ERR_FILTER_FAIL_EN_POS)
#define M154_RX_PHR_ERR_FILTER_FAIL_EN_UMSK                     (~(((1ULL<<M154_RX_PHR_ERR_FILTER_FAIL_EN_LEN)-1)<<M154_RX_PHR_ERR_FILTER_FAIL_EN_POS))
#define M154_RX_INT_CLR_BY_START_EN                             M154_RX_INT_CLR_BY_START_EN
#define M154_RX_INT_CLR_BY_START_EN_POS                         (28U)
#define M154_RX_INT_CLR_BY_START_EN_LEN                         (1U)
#define M154_RX_INT_CLR_BY_START_EN_MSK                         (((1ULL<<M154_RX_INT_CLR_BY_START_EN_LEN)-1)<<M154_RX_INT_CLR_BY_START_EN_POS)
#define M154_RX_INT_CLR_BY_START_EN_UMSK                        (~(((1ULL<<M154_RX_INT_CLR_BY_START_EN_LEN)-1)<<M154_RX_INT_CLR_BY_START_EN_POS))

/* 0xC8 : m154_rx_02 */
#define M154_RX_02_OFFSET                                       (0xC8)
#define M154_RX_PAN_COORDINATOR_1ST                             M154_RX_PAN_COORDINATOR_1ST
#define M154_RX_PAN_COORDINATOR_1ST_POS                         (0U)
#define M154_RX_PAN_COORDINATOR_1ST_LEN                         (1U)
#define M154_RX_PAN_COORDINATOR_1ST_MSK                         (((1ULL<<M154_RX_PAN_COORDINATOR_1ST_LEN)-1)<<M154_RX_PAN_COORDINATOR_1ST_POS)
#define M154_RX_PAN_COORDINATOR_1ST_UMSK                        (~(((1ULL<<M154_RX_PAN_COORDINATOR_1ST_LEN)-1)<<M154_RX_PAN_COORDINATOR_1ST_POS))
#define M154_RX_PAN_COORDINATOR_2ND                             M154_RX_PAN_COORDINATOR_2ND
#define M154_RX_PAN_COORDINATOR_2ND_POS                         (4U)
#define M154_RX_PAN_COORDINATOR_2ND_LEN                         (1U)
#define M154_RX_PAN_COORDINATOR_2ND_MSK                         (((1ULL<<M154_RX_PAN_COORDINATOR_2ND_LEN)-1)<<M154_RX_PAN_COORDINATOR_2ND_POS)
#define M154_RX_PAN_COORDINATOR_2ND_UMSK                        (~(((1ULL<<M154_RX_PAN_COORDINATOR_2ND_LEN)-1)<<M154_RX_PAN_COORDINATOR_2ND_POS))
#define M154_RX_STACK_1ST_EN                                    M154_RX_STACK_1ST_EN
#define M154_RX_STACK_1ST_EN_POS                                (8U)
#define M154_RX_STACK_1ST_EN_LEN                                (1U)
#define M154_RX_STACK_1ST_EN_MSK                                (((1ULL<<M154_RX_STACK_1ST_EN_LEN)-1)<<M154_RX_STACK_1ST_EN_POS)
#define M154_RX_STACK_1ST_EN_UMSK                               (~(((1ULL<<M154_RX_STACK_1ST_EN_LEN)-1)<<M154_RX_STACK_1ST_EN_POS))
#define M154_RX_STACK_2ND_EN                                    M154_RX_STACK_2ND_EN
#define M154_RX_STACK_2ND_EN_POS                                (12U)
#define M154_RX_STACK_2ND_EN_LEN                                (1U)
#define M154_RX_STACK_2ND_EN_MSK                                (((1ULL<<M154_RX_STACK_2ND_EN_LEN)-1)<<M154_RX_STACK_2ND_EN_POS)
#define M154_RX_STACK_2ND_EN_UMSK                               (~(((1ULL<<M154_RX_STACK_2ND_EN_LEN)-1)<<M154_RX_STACK_2ND_EN_POS))
#define M154_RX_CH_SCAN_MODE_EN                                 M154_RX_CH_SCAN_MODE_EN
#define M154_RX_CH_SCAN_MODE_EN_POS                             (13U)
#define M154_RX_CH_SCAN_MODE_EN_LEN                             (1U)
#define M154_RX_CH_SCAN_MODE_EN_MSK                             (((1ULL<<M154_RX_CH_SCAN_MODE_EN_LEN)-1)<<M154_RX_CH_SCAN_MODE_EN_POS)
#define M154_RX_CH_SCAN_MODE_EN_UMSK                            (~(((1ULL<<M154_RX_CH_SCAN_MODE_EN_LEN)-1)<<M154_RX_CH_SCAN_MODE_EN_POS))
#define M154_RX_CH_SCAN_MODE                                    M154_RX_CH_SCAN_MODE
#define M154_RX_CH_SCAN_MODE_POS                                (14U)
#define M154_RX_CH_SCAN_MODE_LEN                                (2U)
#define M154_RX_CH_SCAN_MODE_MSK                                (((1ULL<<M154_RX_CH_SCAN_MODE_LEN)-1)<<M154_RX_CH_SCAN_MODE_POS)
#define M154_RX_CH_SCAN_MODE_UMSK                               (~(((1ULL<<M154_RX_CH_SCAN_MODE_LEN)-1)<<M154_RX_CH_SCAN_MODE_POS))
#define M154_RX_PROMISCUOUS_MODE                                M154_RX_PROMISCUOUS_MODE
#define M154_RX_PROMISCUOUS_MODE_POS                            (16U)
#define M154_RX_PROMISCUOUS_MODE_LEN                            (1U)
#define M154_RX_PROMISCUOUS_MODE_MSK                            (((1ULL<<M154_RX_PROMISCUOUS_MODE_LEN)-1)<<M154_RX_PROMISCUOUS_MODE_POS)
#define M154_RX_PROMISCUOUS_MODE_UMSK                           (~(((1ULL<<M154_RX_PROMISCUOUS_MODE_LEN)-1)<<M154_RX_PROMISCUOUS_MODE_POS))
#define M154_RX_FILTER_BYPASS_SEL                               M154_RX_FILTER_BYPASS_SEL
#define M154_RX_FILTER_BYPASS_SEL_POS                           (17U)
#define M154_RX_FILTER_BYPASS_SEL_LEN                           (1U)
#define M154_RX_FILTER_BYPASS_SEL_MSK                           (((1ULL<<M154_RX_FILTER_BYPASS_SEL_LEN)-1)<<M154_RX_FILTER_BYPASS_SEL_POS)
#define M154_RX_FILTER_BYPASS_SEL_UMSK                          (~(((1ULL<<M154_RX_FILTER_BYPASS_SEL_LEN)-1)<<M154_RX_FILTER_BYPASS_SEL_POS))
#define M154_GROUP_RX_MODE                                      M154_GROUP_RX_MODE
#define M154_GROUP_RX_MODE_POS                                  (19U)
#define M154_GROUP_RX_MODE_LEN                                  (1U)
#define M154_GROUP_RX_MODE_MSK                                  (((1ULL<<M154_GROUP_RX_MODE_LEN)-1)<<M154_GROUP_RX_MODE_POS)
#define M154_GROUP_RX_MODE_UMSK                                 (~(((1ULL<<M154_GROUP_RX_MODE_LEN)-1)<<M154_GROUP_RX_MODE_POS))
#define M154_RX_BP_BOUND_OFFSET_FRC                             M154_RX_BP_BOUND_OFFSET_FRC
#define M154_RX_BP_BOUND_OFFSET_FRC_POS                         (20U)
#define M154_RX_BP_BOUND_OFFSET_FRC_LEN                         (4U)
#define M154_RX_BP_BOUND_OFFSET_FRC_MSK                         (((1ULL<<M154_RX_BP_BOUND_OFFSET_FRC_LEN)-1)<<M154_RX_BP_BOUND_OFFSET_FRC_POS)
#define M154_RX_BP_BOUND_OFFSET_FRC_UMSK                        (~(((1ULL<<M154_RX_BP_BOUND_OFFSET_FRC_LEN)-1)<<M154_RX_BP_BOUND_OFFSET_FRC_POS))
#define M154_RX_BP_BOUND_OFFSET                                 M154_RX_BP_BOUND_OFFSET
#define M154_RX_BP_BOUND_OFFSET_POS                             (24U)
#define M154_RX_BP_BOUND_OFFSET_LEN                             (4U)
#define M154_RX_BP_BOUND_OFFSET_MSK                             (((1ULL<<M154_RX_BP_BOUND_OFFSET_LEN)-1)<<M154_RX_BP_BOUND_OFFSET_POS)
#define M154_RX_BP_BOUND_OFFSET_UMSK                            (~(((1ULL<<M154_RX_BP_BOUND_OFFSET_LEN)-1)<<M154_RX_BP_BOUND_OFFSET_POS))
#define M154_RX_BP_BOUND_BEACON_EN                              M154_RX_BP_BOUND_BEACON_EN
#define M154_RX_BP_BOUND_BEACON_EN_POS                          (28U)
#define M154_RX_BP_BOUND_BEACON_EN_LEN                          (1U)
#define M154_RX_BP_BOUND_BEACON_EN_MSK                          (((1ULL<<M154_RX_BP_BOUND_BEACON_EN_LEN)-1)<<M154_RX_BP_BOUND_BEACON_EN_POS)
#define M154_RX_BP_BOUND_BEACON_EN_UMSK                         (~(((1ULL<<M154_RX_BP_BOUND_BEACON_EN_LEN)-1)<<M154_RX_BP_BOUND_BEACON_EN_POS))
#define M154_RX_BP_BOUND_DATA_EN                                M154_RX_BP_BOUND_DATA_EN
#define M154_RX_BP_BOUND_DATA_EN_POS                            (29U)
#define M154_RX_BP_BOUND_DATA_EN_LEN                            (1U)
#define M154_RX_BP_BOUND_DATA_EN_MSK                            (((1ULL<<M154_RX_BP_BOUND_DATA_EN_LEN)-1)<<M154_RX_BP_BOUND_DATA_EN_POS)
#define M154_RX_BP_BOUND_DATA_EN_UMSK                           (~(((1ULL<<M154_RX_BP_BOUND_DATA_EN_LEN)-1)<<M154_RX_BP_BOUND_DATA_EN_POS))
#define M154_RX_BP_BOUND_CMD_EN                                 M154_RX_BP_BOUND_CMD_EN
#define M154_RX_BP_BOUND_CMD_EN_POS                             (30U)
#define M154_RX_BP_BOUND_CMD_EN_LEN                             (1U)
#define M154_RX_BP_BOUND_CMD_EN_MSK                             (((1ULL<<M154_RX_BP_BOUND_CMD_EN_LEN)-1)<<M154_RX_BP_BOUND_CMD_EN_POS)
#define M154_RX_BP_BOUND_CMD_EN_UMSK                            (~(((1ULL<<M154_RX_BP_BOUND_CMD_EN_LEN)-1)<<M154_RX_BP_BOUND_CMD_EN_POS))
#define M154_RX_IMPLICIT_BROADCAST                              M154_RX_IMPLICIT_BROADCAST
#define M154_RX_IMPLICIT_BROADCAST_POS                          (31U)
#define M154_RX_IMPLICIT_BROADCAST_LEN                          (1U)
#define M154_RX_IMPLICIT_BROADCAST_MSK                          (((1ULL<<M154_RX_IMPLICIT_BROADCAST_LEN)-1)<<M154_RX_IMPLICIT_BROADCAST_POS)
#define M154_RX_IMPLICIT_BROADCAST_UMSK                         (~(((1ULL<<M154_RX_IMPLICIT_BROADCAST_LEN)-1)<<M154_RX_IMPLICIT_BROADCAST_POS))

/* 0xCC : m154_rx_03 */
#define M154_RX_03_OFFSET                                       (0xCC)
#define M154_RX_ADDR_SHORT_1ST                                  M154_RX_ADDR_SHORT_1ST
#define M154_RX_ADDR_SHORT_1ST_POS                              (0U)
#define M154_RX_ADDR_SHORT_1ST_LEN                              (16U)
#define M154_RX_ADDR_SHORT_1ST_MSK                              (((1ULL<<M154_RX_ADDR_SHORT_1ST_LEN)-1)<<M154_RX_ADDR_SHORT_1ST_POS)
#define M154_RX_ADDR_SHORT_1ST_UMSK                             (~(((1ULL<<M154_RX_ADDR_SHORT_1ST_LEN)-1)<<M154_RX_ADDR_SHORT_1ST_POS))
#define M154_RX_PAN_ID_1ST                                      M154_RX_PAN_ID_1ST
#define M154_RX_PAN_ID_1ST_POS                                  (16U)
#define M154_RX_PAN_ID_1ST_LEN                                  (16U)
#define M154_RX_PAN_ID_1ST_MSK                                  (((1ULL<<M154_RX_PAN_ID_1ST_LEN)-1)<<M154_RX_PAN_ID_1ST_POS)
#define M154_RX_PAN_ID_1ST_UMSK                                 (~(((1ULL<<M154_RX_PAN_ID_1ST_LEN)-1)<<M154_RX_PAN_ID_1ST_POS))

/* 0xD0 : m154_rx_04 */
#define M154_RX_04_OFFSET                                       (0xD0)
#define M154_RX_ADDR_EXTENDED_1ST_31_00                         M154_RX_ADDR_EXTENDED_1ST_31_00
#define M154_RX_ADDR_EXTENDED_1ST_31_00_POS                     (0U)
#define M154_RX_ADDR_EXTENDED_1ST_31_00_LEN                     (32U)
#define M154_RX_ADDR_EXTENDED_1ST_31_00_MSK                     (((1ULL<<M154_RX_ADDR_EXTENDED_1ST_31_00_LEN)-1)<<M154_RX_ADDR_EXTENDED_1ST_31_00_POS)
#define M154_RX_ADDR_EXTENDED_1ST_31_00_UMSK                    (~(((1ULL<<M154_RX_ADDR_EXTENDED_1ST_31_00_LEN)-1)<<M154_RX_ADDR_EXTENDED_1ST_31_00_POS))

/* 0xD4 : m154_rx_05 */
#define M154_RX_05_OFFSET                                       (0xD4)
#define M154_RX_ADDR_EXTENDED_1ST_63_32                         M154_RX_ADDR_EXTENDED_1ST_63_32
#define M154_RX_ADDR_EXTENDED_1ST_63_32_POS                     (0U)
#define M154_RX_ADDR_EXTENDED_1ST_63_32_LEN                     (32U)
#define M154_RX_ADDR_EXTENDED_1ST_63_32_MSK                     (((1ULL<<M154_RX_ADDR_EXTENDED_1ST_63_32_LEN)-1)<<M154_RX_ADDR_EXTENDED_1ST_63_32_POS)
#define M154_RX_ADDR_EXTENDED_1ST_63_32_UMSK                    (~(((1ULL<<M154_RX_ADDR_EXTENDED_1ST_63_32_LEN)-1)<<M154_RX_ADDR_EXTENDED_1ST_63_32_POS))

/* 0xD8 : m154_rx_06 */
#define M154_RX_06_OFFSET                                       (0xD8)
#define M154_RX_ADDR_SHORT_2ND                                  M154_RX_ADDR_SHORT_2ND
#define M154_RX_ADDR_SHORT_2ND_POS                              (0U)
#define M154_RX_ADDR_SHORT_2ND_LEN                              (16U)
#define M154_RX_ADDR_SHORT_2ND_MSK                              (((1ULL<<M154_RX_ADDR_SHORT_2ND_LEN)-1)<<M154_RX_ADDR_SHORT_2ND_POS)
#define M154_RX_ADDR_SHORT_2ND_UMSK                             (~(((1ULL<<M154_RX_ADDR_SHORT_2ND_LEN)-1)<<M154_RX_ADDR_SHORT_2ND_POS))
#define M154_RX_PAN_ID_2ND                                      M154_RX_PAN_ID_2ND
#define M154_RX_PAN_ID_2ND_POS                                  (16U)
#define M154_RX_PAN_ID_2ND_LEN                                  (16U)
#define M154_RX_PAN_ID_2ND_MSK                                  (((1ULL<<M154_RX_PAN_ID_2ND_LEN)-1)<<M154_RX_PAN_ID_2ND_POS)
#define M154_RX_PAN_ID_2ND_UMSK                                 (~(((1ULL<<M154_RX_PAN_ID_2ND_LEN)-1)<<M154_RX_PAN_ID_2ND_POS))

/* 0xDC : m154_rx_07 */
#define M154_RX_07_OFFSET                                       (0xDC)
#define M154_RX_ADDR_EXTENDED_2ND_31_00                         M154_RX_ADDR_EXTENDED_2ND_31_00
#define M154_RX_ADDR_EXTENDED_2ND_31_00_POS                     (0U)
#define M154_RX_ADDR_EXTENDED_2ND_31_00_LEN                     (32U)
#define M154_RX_ADDR_EXTENDED_2ND_31_00_MSK                     (((1ULL<<M154_RX_ADDR_EXTENDED_2ND_31_00_LEN)-1)<<M154_RX_ADDR_EXTENDED_2ND_31_00_POS)
#define M154_RX_ADDR_EXTENDED_2ND_31_00_UMSK                    (~(((1ULL<<M154_RX_ADDR_EXTENDED_2ND_31_00_LEN)-1)<<M154_RX_ADDR_EXTENDED_2ND_31_00_POS))

/* 0xE0 : m154_rx_08 */
#define M154_RX_08_OFFSET                                       (0xE0)
#define M154_RX_ADDR_EXTENDED_2ND_63_32                         M154_RX_ADDR_EXTENDED_2ND_63_32
#define M154_RX_ADDR_EXTENDED_2ND_63_32_POS                     (0U)
#define M154_RX_ADDR_EXTENDED_2ND_63_32_LEN                     (32U)
#define M154_RX_ADDR_EXTENDED_2ND_63_32_MSK                     (((1ULL<<M154_RX_ADDR_EXTENDED_2ND_63_32_LEN)-1)<<M154_RX_ADDR_EXTENDED_2ND_63_32_POS)
#define M154_RX_ADDR_EXTENDED_2ND_63_32_UMSK                    (~(((1ULL<<M154_RX_ADDR_EXTENDED_2ND_63_32_LEN)-1)<<M154_RX_ADDR_EXTENDED_2ND_63_32_POS))

/* 0xE4 : m154_rx_09 */
#define M154_RX_09_OFFSET                                       (0xE4)
#define M154_RX_OFF_TRIG_TIME_EN                                M154_RX_OFF_TRIG_TIME_EN
#define M154_RX_OFF_TRIG_TIME_EN_POS                            (0U)
#define M154_RX_OFF_TRIG_TIME_EN_LEN                            (1U)
#define M154_RX_OFF_TRIG_TIME_EN_MSK                            (((1ULL<<M154_RX_OFF_TRIG_TIME_EN_LEN)-1)<<M154_RX_OFF_TRIG_TIME_EN_POS)
#define M154_RX_OFF_TRIG_TIME_EN_UMSK                           (~(((1ULL<<M154_RX_OFF_TRIG_TIME_EN_LEN)-1)<<M154_RX_OFF_TRIG_TIME_EN_POS))
#define M154_RX_OFF_TRIG_BY_END_EN                              M154_RX_OFF_TRIG_BY_END_EN
#define M154_RX_OFF_TRIG_BY_END_EN_POS                          (5U)
#define M154_RX_OFF_TRIG_BY_END_EN_LEN                          (1U)
#define M154_RX_OFF_TRIG_BY_END_EN_MSK                          (((1ULL<<M154_RX_OFF_TRIG_BY_END_EN_LEN)-1)<<M154_RX_OFF_TRIG_BY_END_EN_POS)
#define M154_RX_OFF_TRIG_BY_END_EN_UMSK                         (~(((1ULL<<M154_RX_OFF_TRIG_BY_END_EN_LEN)-1)<<M154_RX_OFF_TRIG_BY_END_EN_POS))
#define M154_RX_OFF_TRIG_BY_END_SEL                             M154_RX_OFF_TRIG_BY_END_SEL
#define M154_RX_OFF_TRIG_BY_END_SEL_POS                         (6U)
#define M154_RX_OFF_TRIG_BY_END_SEL_LEN                         (2U)
#define M154_RX_OFF_TRIG_BY_END_SEL_MSK                         (((1ULL<<M154_RX_OFF_TRIG_BY_END_SEL_LEN)-1)<<M154_RX_OFF_TRIG_BY_END_SEL_POS)
#define M154_RX_OFF_TRIG_BY_END_SEL_UMSK                        (~(((1ULL<<M154_RX_OFF_TRIG_BY_END_SEL_LEN)-1)<<M154_RX_OFF_TRIG_BY_END_SEL_POS))
#define M154_RX_OFF_CMD                                         M154_RX_OFF_CMD
#define M154_RX_OFF_CMD_POS                                     (8U)
#define M154_RX_OFF_CMD_LEN                                     (1U)
#define M154_RX_OFF_CMD_MSK                                     (((1ULL<<M154_RX_OFF_CMD_LEN)-1)<<M154_RX_OFF_CMD_POS)
#define M154_RX_OFF_CMD_UMSK                                    (~(((1ULL<<M154_RX_OFF_CMD_LEN)-1)<<M154_RX_OFF_CMD_POS))
#define M154_RX_ON_CMD                                          M154_RX_ON_CMD
#define M154_RX_ON_CMD_POS                                      (12U)
#define M154_RX_ON_CMD_LEN                                      (1U)
#define M154_RX_ON_CMD_MSK                                      (((1ULL<<M154_RX_ON_CMD_LEN)-1)<<M154_RX_ON_CMD_POS)
#define M154_RX_ON_CMD_UMSK                                     (~(((1ULL<<M154_RX_ON_CMD_LEN)-1)<<M154_RX_ON_CMD_POS))
#define M154_RX_START_MAC_RX                                    M154_RX_START_MAC_RX
#define M154_RX_START_MAC_RX_POS                                (16U)
#define M154_RX_START_MAC_RX_LEN                                (1U)
#define M154_RX_START_MAC_RX_MSK                                (((1ULL<<M154_RX_START_MAC_RX_LEN)-1)<<M154_RX_START_MAC_RX_POS)
#define M154_RX_START_MAC_RX_UMSK                               (~(((1ULL<<M154_RX_START_MAC_RX_LEN)-1)<<M154_RX_START_MAC_RX_POS))
#define M154_RX_PHY_EN                                          M154_RX_PHY_EN
#define M154_RX_PHY_EN_POS                                      (17U)
#define M154_RX_PHY_EN_LEN                                      (1U)
#define M154_RX_PHY_EN_MSK                                      (((1ULL<<M154_RX_PHY_EN_LEN)-1)<<M154_RX_PHY_EN_POS)
#define M154_RX_PHY_EN_UMSK                                     (~(((1ULL<<M154_RX_PHY_EN_LEN)-1)<<M154_RX_PHY_EN_POS))
#define M154_SFD_FOUND                                          M154_SFD_FOUND
#define M154_SFD_FOUND_POS                                      (18U)
#define M154_SFD_FOUND_LEN                                      (1U)
#define M154_SFD_FOUND_MSK                                      (((1ULL<<M154_SFD_FOUND_LEN)-1)<<M154_SFD_FOUND_POS)
#define M154_SFD_FOUND_UMSK                                     (~(((1ULL<<M154_SFD_FOUND_LEN)-1)<<M154_SFD_FOUND_POS))
#define M154_RX_PPDU_SYM_START                                  M154_RX_PPDU_SYM_START
#define M154_RX_PPDU_SYM_START_POS                              (19U)
#define M154_RX_PPDU_SYM_START_LEN                              (1U)
#define M154_RX_PPDU_SYM_START_MSK                              (((1ULL<<M154_RX_PPDU_SYM_START_LEN)-1)<<M154_RX_PPDU_SYM_START_POS)
#define M154_RX_PPDU_SYM_START_UMSK                             (~(((1ULL<<M154_RX_PPDU_SYM_START_LEN)-1)<<M154_RX_PPDU_SYM_START_POS))
#define M154_CSMA_CA_RX_ST                                      M154_CSMA_CA_RX_ST
#define M154_CSMA_CA_RX_ST_POS                                  (20U)
#define M154_CSMA_CA_RX_ST_LEN                                  (1U)
#define M154_CSMA_CA_RX_ST_MSK                                  (((1ULL<<M154_CSMA_CA_RX_ST_LEN)-1)<<M154_CSMA_CA_RX_ST_POS)
#define M154_CSMA_CA_RX_ST_UMSK                                 (~(((1ULL<<M154_CSMA_CA_RX_ST_LEN)-1)<<M154_CSMA_CA_RX_ST_POS))
#define M154_RX_START_OFFSET                                    M154_RX_START_OFFSET
#define M154_RX_START_OFFSET_POS                                (24U)
#define M154_RX_START_OFFSET_LEN                                (8U)
#define M154_RX_START_OFFSET_MSK                                (((1ULL<<M154_RX_START_OFFSET_LEN)-1)<<M154_RX_START_OFFSET_POS)
#define M154_RX_START_OFFSET_UMSK                               (~(((1ULL<<M154_RX_START_OFFSET_LEN)-1)<<M154_RX_START_OFFSET_POS))

/* 0xE8 : m154_rx_10 */
#define M154_RX_10_OFFSET                                       (0xE8)
#define M154_RX_OFF_TRIG_TIME                                   M154_RX_OFF_TRIG_TIME
#define M154_RX_OFF_TRIG_TIME_POS                               (0U)
#define M154_RX_OFF_TRIG_TIME_LEN                               (32U)
#define M154_RX_OFF_TRIG_TIME_MSK                               (((1ULL<<M154_RX_OFF_TRIG_TIME_LEN)-1)<<M154_RX_OFF_TRIG_TIME_POS)
#define M154_RX_OFF_TRIG_TIME_UMSK                              (~(((1ULL<<M154_RX_OFF_TRIG_TIME_LEN)-1)<<M154_RX_OFF_TRIG_TIME_POS))

/* 0xEC : m154_rx_11 */
#define M154_RX_11_OFFSET                                       (0xEC)
#define M154_RX_FRAME_COUNTER                                   M154_RX_FRAME_COUNTER
#define M154_RX_FRAME_COUNTER_POS                               (0U)
#define M154_RX_FRAME_COUNTER_LEN                               (32U)
#define M154_RX_FRAME_COUNTER_MSK                               (((1ULL<<M154_RX_FRAME_COUNTER_LEN)-1)<<M154_RX_FRAME_COUNTER_POS)
#define M154_RX_FRAME_COUNTER_UMSK                              (~(((1ULL<<M154_RX_FRAME_COUNTER_LEN)-1)<<M154_RX_FRAME_COUNTER_POS))

/* 0xF0 : m154_rx_12 */
#define M154_RX_12_OFFSET                                       (0xF0)
#define M154_RX_AUX_SEC_HEADER_OFFSET                           M154_RX_AUX_SEC_HEADER_OFFSET
#define M154_RX_AUX_SEC_HEADER_OFFSET_POS                       (0U)
#define M154_RX_AUX_SEC_HEADER_OFFSET_LEN                       (5U)
#define M154_RX_AUX_SEC_HEADER_OFFSET_MSK                       (((1ULL<<M154_RX_AUX_SEC_HEADER_OFFSET_LEN)-1)<<M154_RX_AUX_SEC_HEADER_OFFSET_POS)
#define M154_RX_AUX_SEC_HEADER_OFFSET_UMSK                      (~(((1ULL<<M154_RX_AUX_SEC_HEADER_OFFSET_LEN)-1)<<M154_RX_AUX_SEC_HEADER_OFFSET_POS))
#define M154_RX_AUX_SEC_HEADER_LENGTH                           M154_RX_AUX_SEC_HEADER_LENGTH
#define M154_RX_AUX_SEC_HEADER_LENGTH_POS                       (8U)
#define M154_RX_AUX_SEC_HEADER_LENGTH_LEN                       (4U)
#define M154_RX_AUX_SEC_HEADER_LENGTH_MSK                       (((1ULL<<M154_RX_AUX_SEC_HEADER_LENGTH_LEN)-1)<<M154_RX_AUX_SEC_HEADER_LENGTH_POS)
#define M154_RX_AUX_SEC_HEADER_LENGTH_UMSK                      (~(((1ULL<<M154_RX_AUX_SEC_HEADER_LENGTH_LEN)-1)<<M154_RX_AUX_SEC_HEADER_LENGTH_POS))
#define M154_RX_SECURITY_LEVEL                                  M154_RX_SECURITY_LEVEL
#define M154_RX_SECURITY_LEVEL_POS                              (12U)
#define M154_RX_SECURITY_LEVEL_LEN                              (3U)
#define M154_RX_SECURITY_LEVEL_MSK                              (((1ULL<<M154_RX_SECURITY_LEVEL_LEN)-1)<<M154_RX_SECURITY_LEVEL_POS)
#define M154_RX_SECURITY_LEVEL_UMSK                             (~(((1ULL<<M154_RX_SECURITY_LEVEL_LEN)-1)<<M154_RX_SECURITY_LEVEL_POS))
#define M154_RX_MIC_LENGTH                                      M154_RX_MIC_LENGTH
#define M154_RX_MIC_LENGTH_POS                                  (16U)
#define M154_RX_MIC_LENGTH_LEN                                  (5U)
#define M154_RX_MIC_LENGTH_MSK                                  (((1ULL<<M154_RX_MIC_LENGTH_LEN)-1)<<M154_RX_MIC_LENGTH_POS)
#define M154_RX_MIC_LENGTH_UMSK                                 (~(((1ULL<<M154_RX_MIC_LENGTH_LEN)-1)<<M154_RX_MIC_LENGTH_POS))
#define M154_RX_A_LENGTH                                        M154_RX_A_LENGTH
#define M154_RX_A_LENGTH_POS                                    (24U)
#define M154_RX_A_LENGTH_LEN                                    (8U)
#define M154_RX_A_LENGTH_MSK                                    (((1ULL<<M154_RX_A_LENGTH_LEN)-1)<<M154_RX_A_LENGTH_POS)
#define M154_RX_A_LENGTH_UMSK                                   (~(((1ULL<<M154_RX_A_LENGTH_LEN)-1)<<M154_RX_A_LENGTH_POS))

/* 0xF4 : m154_rx_13 */
#define M154_RX_13_OFFSET                                       (0xF4)
#define M154_RX_MAC_PAYLOAD_OFFSET                              M154_RX_MAC_PAYLOAD_OFFSET
#define M154_RX_MAC_PAYLOAD_OFFSET_POS                          (0U)
#define M154_RX_MAC_PAYLOAD_OFFSET_LEN                          (8U)
#define M154_RX_MAC_PAYLOAD_OFFSET_MSK                          (((1ULL<<M154_RX_MAC_PAYLOAD_OFFSET_LEN)-1)<<M154_RX_MAC_PAYLOAD_OFFSET_POS)
#define M154_RX_MAC_PAYLOAD_OFFSET_UMSK                         (~(((1ULL<<M154_RX_MAC_PAYLOAD_OFFSET_LEN)-1)<<M154_RX_MAC_PAYLOAD_OFFSET_POS))
#define M154_RX_BEACON_GTS_OFFSET                               M154_RX_BEACON_GTS_OFFSET
#define M154_RX_BEACON_GTS_OFFSET_POS                           (8U)
#define M154_RX_BEACON_GTS_OFFSET_LEN                           (6U)
#define M154_RX_BEACON_GTS_OFFSET_MSK                           (((1ULL<<M154_RX_BEACON_GTS_OFFSET_LEN)-1)<<M154_RX_BEACON_GTS_OFFSET_POS)
#define M154_RX_BEACON_GTS_OFFSET_UMSK                          (~(((1ULL<<M154_RX_BEACON_GTS_OFFSET_LEN)-1)<<M154_RX_BEACON_GTS_OFFSET_POS))
#define M154_RX_BEACON_PEND_ADDR_OFFSET                         M154_RX_BEACON_PEND_ADDR_OFFSET
#define M154_RX_BEACON_PEND_ADDR_OFFSET_POS                     (16U)
#define M154_RX_BEACON_PEND_ADDR_OFFSET_LEN                     (6U)
#define M154_RX_BEACON_PEND_ADDR_OFFSET_MSK                     (((1ULL<<M154_RX_BEACON_PEND_ADDR_OFFSET_LEN)-1)<<M154_RX_BEACON_PEND_ADDR_OFFSET_POS)
#define M154_RX_BEACON_PEND_ADDR_OFFSET_UMSK                    (~(((1ULL<<M154_RX_BEACON_PEND_ADDR_OFFSET_LEN)-1)<<M154_RX_BEACON_PEND_ADDR_OFFSET_POS))
#define M154_RX_BEACON_PAYLOAD_OFFSET                           M154_RX_BEACON_PAYLOAD_OFFSET
#define M154_RX_BEACON_PAYLOAD_OFFSET_POS                       (24U)
#define M154_RX_BEACON_PAYLOAD_OFFSET_LEN                       (8U)
#define M154_RX_BEACON_PAYLOAD_OFFSET_MSK                       (((1ULL<<M154_RX_BEACON_PAYLOAD_OFFSET_LEN)-1)<<M154_RX_BEACON_PAYLOAD_OFFSET_POS)
#define M154_RX_BEACON_PAYLOAD_OFFSET_UMSK                      (~(((1ULL<<M154_RX_BEACON_PAYLOAD_OFFSET_LEN)-1)<<M154_RX_BEACON_PAYLOAD_OFFSET_POS))

/* 0xF8 : m154_rx_14 */
#define M154_RX_14_OFFSET                                       (0xF8)
#define M154_RX_CMD_ID_OFFSET                                   M154_RX_CMD_ID_OFFSET
#define M154_RX_CMD_ID_OFFSET_POS                               (16U)
#define M154_RX_CMD_ID_OFFSET_LEN                               (8U)
#define M154_RX_CMD_ID_OFFSET_MSK                               (((1ULL<<M154_RX_CMD_ID_OFFSET_LEN)-1)<<M154_RX_CMD_ID_OFFSET_POS)
#define M154_RX_CMD_ID_OFFSET_UMSK                              (~(((1ULL<<M154_RX_CMD_ID_OFFSET_LEN)-1)<<M154_RX_CMD_ID_OFFSET_POS))
#define M154_RX_CMD_ID                                          M154_RX_CMD_ID
#define M154_RX_CMD_ID_POS                                      (24U)
#define M154_RX_CMD_ID_LEN                                      (8U)
#define M154_RX_CMD_ID_MSK                                      (((1ULL<<M154_RX_CMD_ID_LEN)-1)<<M154_RX_CMD_ID_POS)
#define M154_RX_CMD_ID_UMSK                                     (~(((1ULL<<M154_RX_CMD_ID_LEN)-1)<<M154_RX_CMD_ID_POS))

/* 0xFC : m154_rx_15 */
#define M154_RX_15_OFFSET                                       (0xFC)
#define M154_RX_AUTO_RESTART_WAIT_TIME                          M154_RX_AUTO_RESTART_WAIT_TIME
#define M154_RX_AUTO_RESTART_WAIT_TIME_POS                      (0U)
#define M154_RX_AUTO_RESTART_WAIT_TIME_LEN                      (16U)
#define M154_RX_AUTO_RESTART_WAIT_TIME_MSK                      (((1ULL<<M154_RX_AUTO_RESTART_WAIT_TIME_LEN)-1)<<M154_RX_AUTO_RESTART_WAIT_TIME_POS)
#define M154_RX_AUTO_RESTART_WAIT_TIME_UMSK                     (~(((1ULL<<M154_RX_AUTO_RESTART_WAIT_TIME_LEN)-1)<<M154_RX_AUTO_RESTART_WAIT_TIME_POS))
#define M154_RX_ACK_WAIT_TIME_US                                M154_RX_ACK_WAIT_TIME_US
#define M154_RX_ACK_WAIT_TIME_US_POS                            (16U)
#define M154_RX_ACK_WAIT_TIME_US_LEN                            (14U)
#define M154_RX_ACK_WAIT_TIME_US_MSK                            (((1ULL<<M154_RX_ACK_WAIT_TIME_US_LEN)-1)<<M154_RX_ACK_WAIT_TIME_US_POS)
#define M154_RX_ACK_WAIT_TIME_US_UMSK                           (~(((1ULL<<M154_RX_ACK_WAIT_TIME_US_LEN)-1)<<M154_RX_ACK_WAIT_TIME_US_POS))

/* 0x100 : m154_rx_16 */
#define M154_RX_16_OFFSET                                       (0x100)
#define M154_TOTAL_FRAME_WAIT_TIME_US                           M154_TOTAL_FRAME_WAIT_TIME_US
#define M154_TOTAL_FRAME_WAIT_TIME_US_POS                       (0U)
#define M154_TOTAL_FRAME_WAIT_TIME_US_LEN                       (32U)
#define M154_TOTAL_FRAME_WAIT_TIME_US_MSK                       (((1ULL<<M154_TOTAL_FRAME_WAIT_TIME_US_LEN)-1)<<M154_TOTAL_FRAME_WAIT_TIME_US_POS)
#define M154_TOTAL_FRAME_WAIT_TIME_US_UMSK                      (~(((1ULL<<M154_TOTAL_FRAME_WAIT_TIME_US_LEN)-1)<<M154_TOTAL_FRAME_WAIT_TIME_US_POS))

/* 0x104 : m154_rx_17 */
#define M154_RX_17_OFFSET                                       (0x104)
#define M154_RX_ON_AFTER_RX_ACK                                 M154_RX_ON_AFTER_RX_ACK
#define M154_RX_ON_AFTER_RX_ACK_POS                             (0U)
#define M154_RX_ON_AFTER_RX_ACK_LEN                             (1U)
#define M154_RX_ON_AFTER_RX_ACK_MSK                             (((1ULL<<M154_RX_ON_AFTER_RX_ACK_LEN)-1)<<M154_RX_ON_AFTER_RX_ACK_POS)
#define M154_RX_ON_AFTER_RX_ACK_UMSK                            (~(((1ULL<<M154_RX_ON_AFTER_RX_ACK_LEN)-1)<<M154_RX_ON_AFTER_RX_ACK_POS))

/* 0x108 : m154_rsvd_01 */
#define M154_RSVD_01_OFFSET                                     (0x108)

/* 0x10C : m154_capt_frm_00 */
#define M154_CAPT_FRM_00_OFFSET                                 (0x10C)
#define M154_CAPT_FRAME_EN                                      M154_CAPT_FRAME_EN
#define M154_CAPT_FRAME_EN_POS                                  (0U)
#define M154_CAPT_FRAME_EN_LEN                                  (1U)
#define M154_CAPT_FRAME_EN_MSK                                  (((1ULL<<M154_CAPT_FRAME_EN_LEN)-1)<<M154_CAPT_FRAME_EN_POS)
#define M154_CAPT_FRAME_EN_UMSK                                 (~(((1ULL<<M154_CAPT_FRAME_EN_LEN)-1)<<M154_CAPT_FRAME_EN_POS))
#define M154_CAPT_FRAME_TX_RXN                                  M154_CAPT_FRAME_TX_RXN
#define M154_CAPT_FRAME_TX_RXN_POS                              (1U)
#define M154_CAPT_FRAME_TX_RXN_LEN                              (1U)
#define M154_CAPT_FRAME_TX_RXN_MSK                              (((1ULL<<M154_CAPT_FRAME_TX_RXN_LEN)-1)<<M154_CAPT_FRAME_TX_RXN_POS)
#define M154_CAPT_FRAME_TX_RXN_UMSK                             (~(((1ULL<<M154_CAPT_FRAME_TX_RXN_LEN)-1)<<M154_CAPT_FRAME_TX_RXN_POS))
#define M154_CAPT_FRAME_RETRY_CNT                               M154_CAPT_FRAME_RETRY_CNT
#define M154_CAPT_FRAME_RETRY_CNT_POS                           (2U)
#define M154_CAPT_FRAME_RETRY_CNT_LEN                           (6U)
#define M154_CAPT_FRAME_RETRY_CNT_MSK                           (((1ULL<<M154_CAPT_FRAME_RETRY_CNT_LEN)-1)<<M154_CAPT_FRAME_RETRY_CNT_POS)
#define M154_CAPT_FRAME_RETRY_CNT_UMSK                          (~(((1ULL<<M154_CAPT_FRAME_RETRY_CNT_LEN)-1)<<M154_CAPT_FRAME_RETRY_CNT_POS))
#define M154_CAPT_FRAME_PHR                                     M154_CAPT_FRAME_PHR
#define M154_CAPT_FRAME_PHR_POS                                 (8U)
#define M154_CAPT_FRAME_PHR_LEN                                 (8U)
#define M154_CAPT_FRAME_PHR_MSK                                 (((1ULL<<M154_CAPT_FRAME_PHR_LEN)-1)<<M154_CAPT_FRAME_PHR_POS)
#define M154_CAPT_FRAME_PHR_UMSK                                (~(((1ULL<<M154_CAPT_FRAME_PHR_LEN)-1)<<M154_CAPT_FRAME_PHR_POS))
#define M154_CAPT_FRAME_MHR                                     M154_CAPT_FRAME_MHR
#define M154_CAPT_FRAME_MHR_POS                                 (16U)
#define M154_CAPT_FRAME_MHR_LEN                                 (16U)
#define M154_CAPT_FRAME_MHR_MSK                                 (((1ULL<<M154_CAPT_FRAME_MHR_LEN)-1)<<M154_CAPT_FRAME_MHR_POS)
#define M154_CAPT_FRAME_MHR_UMSK                                (~(((1ULL<<M154_CAPT_FRAME_MHR_LEN)-1)<<M154_CAPT_FRAME_MHR_POS))

/* 0x110 : m154_sw_profile */
#define M154_SW_PROFILE_OFFSET                                  (0x110)
#define M154_SW_PROFILE                                         M154_SW_PROFILE
#define M154_SW_PROFILE_POS                                     (0U)
#define M154_SW_PROFILE_LEN                                     (32U)
#define M154_SW_PROFILE_MSK                                     (((1ULL<<M154_SW_PROFILE_LEN)-1)<<M154_SW_PROFILE_POS)
#define M154_SW_PROFILE_UMSK                                    (~(((1ULL<<M154_SW_PROFILE_LEN)-1)<<M154_SW_PROFILE_POS))

/* 0x180 : m154_coex_priority */
#define M154_COEX_PRIORITY_OFFSET                               (0x180)
#define M154_RX_IDLE_PRIORITY                                   M154_RX_IDLE_PRIORITY
#define M154_RX_IDLE_PRIORITY_POS                               (0U)
#define M154_RX_IDLE_PRIORITY_LEN                               (4U)
#define M154_RX_IDLE_PRIORITY_MSK                               (((1ULL<<M154_RX_IDLE_PRIORITY_LEN)-1)<<M154_RX_IDLE_PRIORITY_POS)
#define M154_RX_IDLE_PRIORITY_UMSK                              (~(((1ULL<<M154_RX_IDLE_PRIORITY_LEN)-1)<<M154_RX_IDLE_PRIORITY_POS))
#define M154_RX_SFD_PRIORITY                                    M154_RX_SFD_PRIORITY
#define M154_RX_SFD_PRIORITY_POS                                (4U)
#define M154_RX_SFD_PRIORITY_LEN                                (4U)
#define M154_RX_SFD_PRIORITY_MSK                                (((1ULL<<M154_RX_SFD_PRIORITY_LEN)-1)<<M154_RX_SFD_PRIORITY_POS)
#define M154_RX_SFD_PRIORITY_UMSK                               (~(((1ULL<<M154_RX_SFD_PRIORITY_LEN)-1)<<M154_RX_SFD_PRIORITY_POS))
#define M154_RX_UNICAST_PRIORITY                                M154_RX_UNICAST_PRIORITY
#define M154_RX_UNICAST_PRIORITY_POS                            (8U)
#define M154_RX_UNICAST_PRIORITY_LEN                            (4U)
#define M154_RX_UNICAST_PRIORITY_MSK                            (((1ULL<<M154_RX_UNICAST_PRIORITY_LEN)-1)<<M154_RX_UNICAST_PRIORITY_POS)
#define M154_RX_UNICAST_PRIORITY_UMSK                           (~(((1ULL<<M154_RX_UNICAST_PRIORITY_LEN)-1)<<M154_RX_UNICAST_PRIORITY_POS))
#define M154_TX_PRIORITY                                        M154_TX_PRIORITY
#define M154_TX_PRIORITY_POS                                    (12U)
#define M154_TX_PRIORITY_LEN                                    (4U)
#define M154_TX_PRIORITY_MSK                                    (((1ULL<<M154_TX_PRIORITY_LEN)-1)<<M154_TX_PRIORITY_POS)
#define M154_TX_PRIORITY_UMSK                                   (~(((1ULL<<M154_TX_PRIORITY_LEN)-1)<<M154_TX_PRIORITY_POS))
#define M154_RX_ON_AFTER_RX_ABORT_EN                            M154_RX_ON_AFTER_RX_ABORT_EN
#define M154_RX_ON_AFTER_RX_ABORT_EN_POS                        (16U)
#define M154_RX_ON_AFTER_RX_ABORT_EN_LEN                        (1U)
#define M154_RX_ON_AFTER_RX_ABORT_EN_MSK                        (((1ULL<<M154_RX_ON_AFTER_RX_ABORT_EN_LEN)-1)<<M154_RX_ON_AFTER_RX_ABORT_EN_POS)
#define M154_RX_ON_AFTER_RX_ABORT_EN_UMSK                       (~(((1ULL<<M154_RX_ON_AFTER_RX_ABORT_EN_LEN)-1)<<M154_RX_ON_AFTER_RX_ABORT_EN_POS))
#define M154_RX_ABORT_INT_STATUS                                M154_RX_ABORT_INT_STATUS
#define M154_RX_ABORT_INT_STATUS_POS                            (28U)
#define M154_RX_ABORT_INT_STATUS_LEN                            (1U)
#define M154_RX_ABORT_INT_STATUS_MSK                            (((1ULL<<M154_RX_ABORT_INT_STATUS_LEN)-1)<<M154_RX_ABORT_INT_STATUS_POS)
#define M154_RX_ABORT_INT_STATUS_UMSK                           (~(((1ULL<<M154_RX_ABORT_INT_STATUS_LEN)-1)<<M154_RX_ABORT_INT_STATUS_POS))
#define M154_TX_ABORT_INT_STATUS                                M154_TX_ABORT_INT_STATUS
#define M154_TX_ABORT_INT_STATUS_POS                            (29U)
#define M154_TX_ABORT_INT_STATUS_LEN                            (1U)
#define M154_TX_ABORT_INT_STATUS_MSK                            (((1ULL<<M154_TX_ABORT_INT_STATUS_LEN)-1)<<M154_TX_ABORT_INT_STATUS_POS)
#define M154_TX_ABORT_INT_STATUS_UMSK                           (~(((1ULL<<M154_TX_ABORT_INT_STATUS_LEN)-1)<<M154_TX_ABORT_INT_STATUS_POS))
#define M154_RX_ABORT_INT_DUAL_EN                               M154_RX_ABORT_INT_DUAL_EN
#define M154_RX_ABORT_INT_DUAL_EN_POS                           (30U)
#define M154_RX_ABORT_INT_DUAL_EN_LEN                           (1U)
#define M154_RX_ABORT_INT_DUAL_EN_MSK                           (((1ULL<<M154_RX_ABORT_INT_DUAL_EN_LEN)-1)<<M154_RX_ABORT_INT_DUAL_EN_POS)
#define M154_RX_ABORT_INT_DUAL_EN_UMSK                          (~(((1ULL<<M154_RX_ABORT_INT_DUAL_EN_LEN)-1)<<M154_RX_ABORT_INT_DUAL_EN_POS))
#define M154_TX_ABORT_INT_DUAL_EN                               M154_TX_ABORT_INT_DUAL_EN
#define M154_TX_ABORT_INT_DUAL_EN_POS                           (31U)
#define M154_TX_ABORT_INT_DUAL_EN_LEN                           (1U)
#define M154_TX_ABORT_INT_DUAL_EN_MSK                           (((1ULL<<M154_TX_ABORT_INT_DUAL_EN_LEN)-1)<<M154_TX_ABORT_INT_DUAL_EN_POS)
#define M154_TX_ABORT_INT_DUAL_EN_UMSK                          (~(((1ULL<<M154_TX_ABORT_INT_DUAL_EN_LEN)-1)<<M154_TX_ABORT_INT_DUAL_EN_POS))

/* 0x200 : m154_tx_mpdu_00 */
#define M154_TX_MPDU_00_OFFSET                                  (0x200)
#define M154_TX_MPDU_000                                        M154_TX_MPDU_000
#define M154_TX_MPDU_000_POS                                    (0U)
#define M154_TX_MPDU_000_LEN                                    (8U)
#define M154_TX_MPDU_000_MSK                                    (((1ULL<<M154_TX_MPDU_000_LEN)-1)<<M154_TX_MPDU_000_POS)
#define M154_TX_MPDU_000_UMSK                                   (~(((1ULL<<M154_TX_MPDU_000_LEN)-1)<<M154_TX_MPDU_000_POS))
#define M154_TX_MPDU_001                                        M154_TX_MPDU_001
#define M154_TX_MPDU_001_POS                                    (8U)
#define M154_TX_MPDU_001_LEN                                    (8U)
#define M154_TX_MPDU_001_MSK                                    (((1ULL<<M154_TX_MPDU_001_LEN)-1)<<M154_TX_MPDU_001_POS)
#define M154_TX_MPDU_001_UMSK                                   (~(((1ULL<<M154_TX_MPDU_001_LEN)-1)<<M154_TX_MPDU_001_POS))
#define M154_TX_MPDU_002                                        M154_TX_MPDU_002
#define M154_TX_MPDU_002_POS                                    (16U)
#define M154_TX_MPDU_002_LEN                                    (8U)
#define M154_TX_MPDU_002_MSK                                    (((1ULL<<M154_TX_MPDU_002_LEN)-1)<<M154_TX_MPDU_002_POS)
#define M154_TX_MPDU_002_UMSK                                   (~(((1ULL<<M154_TX_MPDU_002_LEN)-1)<<M154_TX_MPDU_002_POS))
#define M154_TX_MPDU_003                                        M154_TX_MPDU_003
#define M154_TX_MPDU_003_POS                                    (24U)
#define M154_TX_MPDU_003_LEN                                    (8U)
#define M154_TX_MPDU_003_MSK                                    (((1ULL<<M154_TX_MPDU_003_LEN)-1)<<M154_TX_MPDU_003_POS)
#define M154_TX_MPDU_003_UMSK                                   (~(((1ULL<<M154_TX_MPDU_003_LEN)-1)<<M154_TX_MPDU_003_POS))

/* 0x204 : m154_tx_mpdu_01 */
#define M154_TX_MPDU_01_OFFSET                                  (0x204)
#define M154_TX_MPDU_004                                        M154_TX_MPDU_004
#define M154_TX_MPDU_004_POS                                    (0U)
#define M154_TX_MPDU_004_LEN                                    (8U)
#define M154_TX_MPDU_004_MSK                                    (((1ULL<<M154_TX_MPDU_004_LEN)-1)<<M154_TX_MPDU_004_POS)
#define M154_TX_MPDU_004_UMSK                                   (~(((1ULL<<M154_TX_MPDU_004_LEN)-1)<<M154_TX_MPDU_004_POS))
#define M154_TX_MPDU_005                                        M154_TX_MPDU_005
#define M154_TX_MPDU_005_POS                                    (8U)
#define M154_TX_MPDU_005_LEN                                    (8U)
#define M154_TX_MPDU_005_MSK                                    (((1ULL<<M154_TX_MPDU_005_LEN)-1)<<M154_TX_MPDU_005_POS)
#define M154_TX_MPDU_005_UMSK                                   (~(((1ULL<<M154_TX_MPDU_005_LEN)-1)<<M154_TX_MPDU_005_POS))
#define M154_TX_MPDU_006                                        M154_TX_MPDU_006
#define M154_TX_MPDU_006_POS                                    (16U)
#define M154_TX_MPDU_006_LEN                                    (8U)
#define M154_TX_MPDU_006_MSK                                    (((1ULL<<M154_TX_MPDU_006_LEN)-1)<<M154_TX_MPDU_006_POS)
#define M154_TX_MPDU_006_UMSK                                   (~(((1ULL<<M154_TX_MPDU_006_LEN)-1)<<M154_TX_MPDU_006_POS))
#define M154_TX_MPDU_007                                        M154_TX_MPDU_007
#define M154_TX_MPDU_007_POS                                    (24U)
#define M154_TX_MPDU_007_LEN                                    (8U)
#define M154_TX_MPDU_007_MSK                                    (((1ULL<<M154_TX_MPDU_007_LEN)-1)<<M154_TX_MPDU_007_POS)
#define M154_TX_MPDU_007_UMSK                                   (~(((1ULL<<M154_TX_MPDU_007_LEN)-1)<<M154_TX_MPDU_007_POS))

/* 0x208 : m154_tx_mpdu_02 */
#define M154_TX_MPDU_02_OFFSET                                  (0x208)
#define M154_TX_MPDU_008                                        M154_TX_MPDU_008
#define M154_TX_MPDU_008_POS                                    (0U)
#define M154_TX_MPDU_008_LEN                                    (8U)
#define M154_TX_MPDU_008_MSK                                    (((1ULL<<M154_TX_MPDU_008_LEN)-1)<<M154_TX_MPDU_008_POS)
#define M154_TX_MPDU_008_UMSK                                   (~(((1ULL<<M154_TX_MPDU_008_LEN)-1)<<M154_TX_MPDU_008_POS))
#define M154_TX_MPDU_009                                        M154_TX_MPDU_009
#define M154_TX_MPDU_009_POS                                    (8U)
#define M154_TX_MPDU_009_LEN                                    (8U)
#define M154_TX_MPDU_009_MSK                                    (((1ULL<<M154_TX_MPDU_009_LEN)-1)<<M154_TX_MPDU_009_POS)
#define M154_TX_MPDU_009_UMSK                                   (~(((1ULL<<M154_TX_MPDU_009_LEN)-1)<<M154_TX_MPDU_009_POS))
#define M154_TX_MPDU_010                                        M154_TX_MPDU_010
#define M154_TX_MPDU_010_POS                                    (16U)
#define M154_TX_MPDU_010_LEN                                    (8U)
#define M154_TX_MPDU_010_MSK                                    (((1ULL<<M154_TX_MPDU_010_LEN)-1)<<M154_TX_MPDU_010_POS)
#define M154_TX_MPDU_010_UMSK                                   (~(((1ULL<<M154_TX_MPDU_010_LEN)-1)<<M154_TX_MPDU_010_POS))
#define M154_TX_MPDU_011                                        M154_TX_MPDU_011
#define M154_TX_MPDU_011_POS                                    (24U)
#define M154_TX_MPDU_011_LEN                                    (8U)
#define M154_TX_MPDU_011_MSK                                    (((1ULL<<M154_TX_MPDU_011_LEN)-1)<<M154_TX_MPDU_011_POS)
#define M154_TX_MPDU_011_UMSK                                   (~(((1ULL<<M154_TX_MPDU_011_LEN)-1)<<M154_TX_MPDU_011_POS))

/* 0x20C : m154_tx_mpdu_03 */
#define M154_TX_MPDU_03_OFFSET                                  (0x20C)
#define M154_TX_MPDU_012                                        M154_TX_MPDU_012
#define M154_TX_MPDU_012_POS                                    (0U)
#define M154_TX_MPDU_012_LEN                                    (8U)
#define M154_TX_MPDU_012_MSK                                    (((1ULL<<M154_TX_MPDU_012_LEN)-1)<<M154_TX_MPDU_012_POS)
#define M154_TX_MPDU_012_UMSK                                   (~(((1ULL<<M154_TX_MPDU_012_LEN)-1)<<M154_TX_MPDU_012_POS))
#define M154_TX_MPDU_013                                        M154_TX_MPDU_013
#define M154_TX_MPDU_013_POS                                    (8U)
#define M154_TX_MPDU_013_LEN                                    (8U)
#define M154_TX_MPDU_013_MSK                                    (((1ULL<<M154_TX_MPDU_013_LEN)-1)<<M154_TX_MPDU_013_POS)
#define M154_TX_MPDU_013_UMSK                                   (~(((1ULL<<M154_TX_MPDU_013_LEN)-1)<<M154_TX_MPDU_013_POS))
#define M154_TX_MPDU_014                                        M154_TX_MPDU_014
#define M154_TX_MPDU_014_POS                                    (16U)
#define M154_TX_MPDU_014_LEN                                    (8U)
#define M154_TX_MPDU_014_MSK                                    (((1ULL<<M154_TX_MPDU_014_LEN)-1)<<M154_TX_MPDU_014_POS)
#define M154_TX_MPDU_014_UMSK                                   (~(((1ULL<<M154_TX_MPDU_014_LEN)-1)<<M154_TX_MPDU_014_POS))
#define M154_TX_MPDU_015                                        M154_TX_MPDU_015
#define M154_TX_MPDU_015_POS                                    (24U)
#define M154_TX_MPDU_015_LEN                                    (8U)
#define M154_TX_MPDU_015_MSK                                    (((1ULL<<M154_TX_MPDU_015_LEN)-1)<<M154_TX_MPDU_015_POS)
#define M154_TX_MPDU_015_UMSK                                   (~(((1ULL<<M154_TX_MPDU_015_LEN)-1)<<M154_TX_MPDU_015_POS))

/* 0x210 : m154_tx_mpdu_04 */
#define M154_TX_MPDU_04_OFFSET                                  (0x210)
#define M154_TX_MPDU_016                                        M154_TX_MPDU_016
#define M154_TX_MPDU_016_POS                                    (0U)
#define M154_TX_MPDU_016_LEN                                    (8U)
#define M154_TX_MPDU_016_MSK                                    (((1ULL<<M154_TX_MPDU_016_LEN)-1)<<M154_TX_MPDU_016_POS)
#define M154_TX_MPDU_016_UMSK                                   (~(((1ULL<<M154_TX_MPDU_016_LEN)-1)<<M154_TX_MPDU_016_POS))
#define M154_TX_MPDU_017                                        M154_TX_MPDU_017
#define M154_TX_MPDU_017_POS                                    (8U)
#define M154_TX_MPDU_017_LEN                                    (8U)
#define M154_TX_MPDU_017_MSK                                    (((1ULL<<M154_TX_MPDU_017_LEN)-1)<<M154_TX_MPDU_017_POS)
#define M154_TX_MPDU_017_UMSK                                   (~(((1ULL<<M154_TX_MPDU_017_LEN)-1)<<M154_TX_MPDU_017_POS))
#define M154_TX_MPDU_018                                        M154_TX_MPDU_018
#define M154_TX_MPDU_018_POS                                    (16U)
#define M154_TX_MPDU_018_LEN                                    (8U)
#define M154_TX_MPDU_018_MSK                                    (((1ULL<<M154_TX_MPDU_018_LEN)-1)<<M154_TX_MPDU_018_POS)
#define M154_TX_MPDU_018_UMSK                                   (~(((1ULL<<M154_TX_MPDU_018_LEN)-1)<<M154_TX_MPDU_018_POS))
#define M154_TX_MPDU_019                                        M154_TX_MPDU_019
#define M154_TX_MPDU_019_POS                                    (24U)
#define M154_TX_MPDU_019_LEN                                    (8U)
#define M154_TX_MPDU_019_MSK                                    (((1ULL<<M154_TX_MPDU_019_LEN)-1)<<M154_TX_MPDU_019_POS)
#define M154_TX_MPDU_019_UMSK                                   (~(((1ULL<<M154_TX_MPDU_019_LEN)-1)<<M154_TX_MPDU_019_POS))

/* 0x214 : m154_tx_mpdu_05 */
#define M154_TX_MPDU_05_OFFSET                                  (0x214)
#define M154_TX_MPDU_020                                        M154_TX_MPDU_020
#define M154_TX_MPDU_020_POS                                    (0U)
#define M154_TX_MPDU_020_LEN                                    (8U)
#define M154_TX_MPDU_020_MSK                                    (((1ULL<<M154_TX_MPDU_020_LEN)-1)<<M154_TX_MPDU_020_POS)
#define M154_TX_MPDU_020_UMSK                                   (~(((1ULL<<M154_TX_MPDU_020_LEN)-1)<<M154_TX_MPDU_020_POS))
#define M154_TX_MPDU_021                                        M154_TX_MPDU_021
#define M154_TX_MPDU_021_POS                                    (8U)
#define M154_TX_MPDU_021_LEN                                    (8U)
#define M154_TX_MPDU_021_MSK                                    (((1ULL<<M154_TX_MPDU_021_LEN)-1)<<M154_TX_MPDU_021_POS)
#define M154_TX_MPDU_021_UMSK                                   (~(((1ULL<<M154_TX_MPDU_021_LEN)-1)<<M154_TX_MPDU_021_POS))
#define M154_TX_MPDU_022                                        M154_TX_MPDU_022
#define M154_TX_MPDU_022_POS                                    (16U)
#define M154_TX_MPDU_022_LEN                                    (8U)
#define M154_TX_MPDU_022_MSK                                    (((1ULL<<M154_TX_MPDU_022_LEN)-1)<<M154_TX_MPDU_022_POS)
#define M154_TX_MPDU_022_UMSK                                   (~(((1ULL<<M154_TX_MPDU_022_LEN)-1)<<M154_TX_MPDU_022_POS))
#define M154_TX_MPDU_023                                        M154_TX_MPDU_023
#define M154_TX_MPDU_023_POS                                    (24U)
#define M154_TX_MPDU_023_LEN                                    (8U)
#define M154_TX_MPDU_023_MSK                                    (((1ULL<<M154_TX_MPDU_023_LEN)-1)<<M154_TX_MPDU_023_POS)
#define M154_TX_MPDU_023_UMSK                                   (~(((1ULL<<M154_TX_MPDU_023_LEN)-1)<<M154_TX_MPDU_023_POS))

/* 0x218 : m154_tx_mpdu_06 */
#define M154_TX_MPDU_06_OFFSET                                  (0x218)
#define M154_TX_MPDU_024                                        M154_TX_MPDU_024
#define M154_TX_MPDU_024_POS                                    (0U)
#define M154_TX_MPDU_024_LEN                                    (8U)
#define M154_TX_MPDU_024_MSK                                    (((1ULL<<M154_TX_MPDU_024_LEN)-1)<<M154_TX_MPDU_024_POS)
#define M154_TX_MPDU_024_UMSK                                   (~(((1ULL<<M154_TX_MPDU_024_LEN)-1)<<M154_TX_MPDU_024_POS))
#define M154_TX_MPDU_025                                        M154_TX_MPDU_025
#define M154_TX_MPDU_025_POS                                    (8U)
#define M154_TX_MPDU_025_LEN                                    (8U)
#define M154_TX_MPDU_025_MSK                                    (((1ULL<<M154_TX_MPDU_025_LEN)-1)<<M154_TX_MPDU_025_POS)
#define M154_TX_MPDU_025_UMSK                                   (~(((1ULL<<M154_TX_MPDU_025_LEN)-1)<<M154_TX_MPDU_025_POS))
#define M154_TX_MPDU_026                                        M154_TX_MPDU_026
#define M154_TX_MPDU_026_POS                                    (16U)
#define M154_TX_MPDU_026_LEN                                    (8U)
#define M154_TX_MPDU_026_MSK                                    (((1ULL<<M154_TX_MPDU_026_LEN)-1)<<M154_TX_MPDU_026_POS)
#define M154_TX_MPDU_026_UMSK                                   (~(((1ULL<<M154_TX_MPDU_026_LEN)-1)<<M154_TX_MPDU_026_POS))
#define M154_TX_MPDU_027                                        M154_TX_MPDU_027
#define M154_TX_MPDU_027_POS                                    (24U)
#define M154_TX_MPDU_027_LEN                                    (8U)
#define M154_TX_MPDU_027_MSK                                    (((1ULL<<M154_TX_MPDU_027_LEN)-1)<<M154_TX_MPDU_027_POS)
#define M154_TX_MPDU_027_UMSK                                   (~(((1ULL<<M154_TX_MPDU_027_LEN)-1)<<M154_TX_MPDU_027_POS))

/* 0x21C : m154_tx_mpdu_07 */
#define M154_TX_MPDU_07_OFFSET                                  (0x21C)
#define M154_TX_MPDU_028                                        M154_TX_MPDU_028
#define M154_TX_MPDU_028_POS                                    (0U)
#define M154_TX_MPDU_028_LEN                                    (8U)
#define M154_TX_MPDU_028_MSK                                    (((1ULL<<M154_TX_MPDU_028_LEN)-1)<<M154_TX_MPDU_028_POS)
#define M154_TX_MPDU_028_UMSK                                   (~(((1ULL<<M154_TX_MPDU_028_LEN)-1)<<M154_TX_MPDU_028_POS))
#define M154_TX_MPDU_029                                        M154_TX_MPDU_029
#define M154_TX_MPDU_029_POS                                    (8U)
#define M154_TX_MPDU_029_LEN                                    (8U)
#define M154_TX_MPDU_029_MSK                                    (((1ULL<<M154_TX_MPDU_029_LEN)-1)<<M154_TX_MPDU_029_POS)
#define M154_TX_MPDU_029_UMSK                                   (~(((1ULL<<M154_TX_MPDU_029_LEN)-1)<<M154_TX_MPDU_029_POS))
#define M154_TX_MPDU_030                                        M154_TX_MPDU_030
#define M154_TX_MPDU_030_POS                                    (16U)
#define M154_TX_MPDU_030_LEN                                    (8U)
#define M154_TX_MPDU_030_MSK                                    (((1ULL<<M154_TX_MPDU_030_LEN)-1)<<M154_TX_MPDU_030_POS)
#define M154_TX_MPDU_030_UMSK                                   (~(((1ULL<<M154_TX_MPDU_030_LEN)-1)<<M154_TX_MPDU_030_POS))
#define M154_TX_MPDU_031                                        M154_TX_MPDU_031
#define M154_TX_MPDU_031_POS                                    (24U)
#define M154_TX_MPDU_031_LEN                                    (8U)
#define M154_TX_MPDU_031_MSK                                    (((1ULL<<M154_TX_MPDU_031_LEN)-1)<<M154_TX_MPDU_031_POS)
#define M154_TX_MPDU_031_UMSK                                   (~(((1ULL<<M154_TX_MPDU_031_LEN)-1)<<M154_TX_MPDU_031_POS))

/* 0x220 : m154_tx_mpdu_08 */
#define M154_TX_MPDU_08_OFFSET                                  (0x220)
#define M154_TX_MPDU_032                                        M154_TX_MPDU_032
#define M154_TX_MPDU_032_POS                                    (0U)
#define M154_TX_MPDU_032_LEN                                    (8U)
#define M154_TX_MPDU_032_MSK                                    (((1ULL<<M154_TX_MPDU_032_LEN)-1)<<M154_TX_MPDU_032_POS)
#define M154_TX_MPDU_032_UMSK                                   (~(((1ULL<<M154_TX_MPDU_032_LEN)-1)<<M154_TX_MPDU_032_POS))
#define M154_TX_MPDU_033                                        M154_TX_MPDU_033
#define M154_TX_MPDU_033_POS                                    (8U)
#define M154_TX_MPDU_033_LEN                                    (8U)
#define M154_TX_MPDU_033_MSK                                    (((1ULL<<M154_TX_MPDU_033_LEN)-1)<<M154_TX_MPDU_033_POS)
#define M154_TX_MPDU_033_UMSK                                   (~(((1ULL<<M154_TX_MPDU_033_LEN)-1)<<M154_TX_MPDU_033_POS))
#define M154_TX_MPDU_034                                        M154_TX_MPDU_034
#define M154_TX_MPDU_034_POS                                    (16U)
#define M154_TX_MPDU_034_LEN                                    (8U)
#define M154_TX_MPDU_034_MSK                                    (((1ULL<<M154_TX_MPDU_034_LEN)-1)<<M154_TX_MPDU_034_POS)
#define M154_TX_MPDU_034_UMSK                                   (~(((1ULL<<M154_TX_MPDU_034_LEN)-1)<<M154_TX_MPDU_034_POS))
#define M154_TX_MPDU_035                                        M154_TX_MPDU_035
#define M154_TX_MPDU_035_POS                                    (24U)
#define M154_TX_MPDU_035_LEN                                    (8U)
#define M154_TX_MPDU_035_MSK                                    (((1ULL<<M154_TX_MPDU_035_LEN)-1)<<M154_TX_MPDU_035_POS)
#define M154_TX_MPDU_035_UMSK                                   (~(((1ULL<<M154_TX_MPDU_035_LEN)-1)<<M154_TX_MPDU_035_POS))

/* 0x224 : m154_tx_mpdu_09 */
#define M154_TX_MPDU_09_OFFSET                                  (0x224)
#define M154_TX_MPDU_036                                        M154_TX_MPDU_036
#define M154_TX_MPDU_036_POS                                    (0U)
#define M154_TX_MPDU_036_LEN                                    (8U)
#define M154_TX_MPDU_036_MSK                                    (((1ULL<<M154_TX_MPDU_036_LEN)-1)<<M154_TX_MPDU_036_POS)
#define M154_TX_MPDU_036_UMSK                                   (~(((1ULL<<M154_TX_MPDU_036_LEN)-1)<<M154_TX_MPDU_036_POS))
#define M154_TX_MPDU_037                                        M154_TX_MPDU_037
#define M154_TX_MPDU_037_POS                                    (8U)
#define M154_TX_MPDU_037_LEN                                    (8U)
#define M154_TX_MPDU_037_MSK                                    (((1ULL<<M154_TX_MPDU_037_LEN)-1)<<M154_TX_MPDU_037_POS)
#define M154_TX_MPDU_037_UMSK                                   (~(((1ULL<<M154_TX_MPDU_037_LEN)-1)<<M154_TX_MPDU_037_POS))
#define M154_TX_MPDU_038                                        M154_TX_MPDU_038
#define M154_TX_MPDU_038_POS                                    (16U)
#define M154_TX_MPDU_038_LEN                                    (8U)
#define M154_TX_MPDU_038_MSK                                    (((1ULL<<M154_TX_MPDU_038_LEN)-1)<<M154_TX_MPDU_038_POS)
#define M154_TX_MPDU_038_UMSK                                   (~(((1ULL<<M154_TX_MPDU_038_LEN)-1)<<M154_TX_MPDU_038_POS))
#define M154_TX_MPDU_039                                        M154_TX_MPDU_039
#define M154_TX_MPDU_039_POS                                    (24U)
#define M154_TX_MPDU_039_LEN                                    (8U)
#define M154_TX_MPDU_039_MSK                                    (((1ULL<<M154_TX_MPDU_039_LEN)-1)<<M154_TX_MPDU_039_POS)
#define M154_TX_MPDU_039_UMSK                                   (~(((1ULL<<M154_TX_MPDU_039_LEN)-1)<<M154_TX_MPDU_039_POS))

/* 0x228 : m154_tx_mpdu_10 */
#define M154_TX_MPDU_10_OFFSET                                  (0x228)
#define M154_TX_MPDU_040                                        M154_TX_MPDU_040
#define M154_TX_MPDU_040_POS                                    (0U)
#define M154_TX_MPDU_040_LEN                                    (8U)
#define M154_TX_MPDU_040_MSK                                    (((1ULL<<M154_TX_MPDU_040_LEN)-1)<<M154_TX_MPDU_040_POS)
#define M154_TX_MPDU_040_UMSK                                   (~(((1ULL<<M154_TX_MPDU_040_LEN)-1)<<M154_TX_MPDU_040_POS))
#define M154_TX_MPDU_041                                        M154_TX_MPDU_041
#define M154_TX_MPDU_041_POS                                    (8U)
#define M154_TX_MPDU_041_LEN                                    (8U)
#define M154_TX_MPDU_041_MSK                                    (((1ULL<<M154_TX_MPDU_041_LEN)-1)<<M154_TX_MPDU_041_POS)
#define M154_TX_MPDU_041_UMSK                                   (~(((1ULL<<M154_TX_MPDU_041_LEN)-1)<<M154_TX_MPDU_041_POS))
#define M154_TX_MPDU_042                                        M154_TX_MPDU_042
#define M154_TX_MPDU_042_POS                                    (16U)
#define M154_TX_MPDU_042_LEN                                    (8U)
#define M154_TX_MPDU_042_MSK                                    (((1ULL<<M154_TX_MPDU_042_LEN)-1)<<M154_TX_MPDU_042_POS)
#define M154_TX_MPDU_042_UMSK                                   (~(((1ULL<<M154_TX_MPDU_042_LEN)-1)<<M154_TX_MPDU_042_POS))
#define M154_TX_MPDU_043                                        M154_TX_MPDU_043
#define M154_TX_MPDU_043_POS                                    (24U)
#define M154_TX_MPDU_043_LEN                                    (8U)
#define M154_TX_MPDU_043_MSK                                    (((1ULL<<M154_TX_MPDU_043_LEN)-1)<<M154_TX_MPDU_043_POS)
#define M154_TX_MPDU_043_UMSK                                   (~(((1ULL<<M154_TX_MPDU_043_LEN)-1)<<M154_TX_MPDU_043_POS))

/* 0x22C : m154_tx_mpdu_11 */
#define M154_TX_MPDU_11_OFFSET                                  (0x22C)
#define M154_TX_MPDU_044                                        M154_TX_MPDU_044
#define M154_TX_MPDU_044_POS                                    (0U)
#define M154_TX_MPDU_044_LEN                                    (8U)
#define M154_TX_MPDU_044_MSK                                    (((1ULL<<M154_TX_MPDU_044_LEN)-1)<<M154_TX_MPDU_044_POS)
#define M154_TX_MPDU_044_UMSK                                   (~(((1ULL<<M154_TX_MPDU_044_LEN)-1)<<M154_TX_MPDU_044_POS))
#define M154_TX_MPDU_045                                        M154_TX_MPDU_045
#define M154_TX_MPDU_045_POS                                    (8U)
#define M154_TX_MPDU_045_LEN                                    (8U)
#define M154_TX_MPDU_045_MSK                                    (((1ULL<<M154_TX_MPDU_045_LEN)-1)<<M154_TX_MPDU_045_POS)
#define M154_TX_MPDU_045_UMSK                                   (~(((1ULL<<M154_TX_MPDU_045_LEN)-1)<<M154_TX_MPDU_045_POS))
#define M154_TX_MPDU_046                                        M154_TX_MPDU_046
#define M154_TX_MPDU_046_POS                                    (16U)
#define M154_TX_MPDU_046_LEN                                    (8U)
#define M154_TX_MPDU_046_MSK                                    (((1ULL<<M154_TX_MPDU_046_LEN)-1)<<M154_TX_MPDU_046_POS)
#define M154_TX_MPDU_046_UMSK                                   (~(((1ULL<<M154_TX_MPDU_046_LEN)-1)<<M154_TX_MPDU_046_POS))
#define M154_TX_MPDU_047                                        M154_TX_MPDU_047
#define M154_TX_MPDU_047_POS                                    (24U)
#define M154_TX_MPDU_047_LEN                                    (8U)
#define M154_TX_MPDU_047_MSK                                    (((1ULL<<M154_TX_MPDU_047_LEN)-1)<<M154_TX_MPDU_047_POS)
#define M154_TX_MPDU_047_UMSK                                   (~(((1ULL<<M154_TX_MPDU_047_LEN)-1)<<M154_TX_MPDU_047_POS))

/* 0x230 : m154_tx_mpdu_12 */
#define M154_TX_MPDU_12_OFFSET                                  (0x230)
#define M154_TX_MPDU_048                                        M154_TX_MPDU_048
#define M154_TX_MPDU_048_POS                                    (0U)
#define M154_TX_MPDU_048_LEN                                    (8U)
#define M154_TX_MPDU_048_MSK                                    (((1ULL<<M154_TX_MPDU_048_LEN)-1)<<M154_TX_MPDU_048_POS)
#define M154_TX_MPDU_048_UMSK                                   (~(((1ULL<<M154_TX_MPDU_048_LEN)-1)<<M154_TX_MPDU_048_POS))
#define M154_TX_MPDU_049                                        M154_TX_MPDU_049
#define M154_TX_MPDU_049_POS                                    (8U)
#define M154_TX_MPDU_049_LEN                                    (8U)
#define M154_TX_MPDU_049_MSK                                    (((1ULL<<M154_TX_MPDU_049_LEN)-1)<<M154_TX_MPDU_049_POS)
#define M154_TX_MPDU_049_UMSK                                   (~(((1ULL<<M154_TX_MPDU_049_LEN)-1)<<M154_TX_MPDU_049_POS))
#define M154_TX_MPDU_050                                        M154_TX_MPDU_050
#define M154_TX_MPDU_050_POS                                    (16U)
#define M154_TX_MPDU_050_LEN                                    (8U)
#define M154_TX_MPDU_050_MSK                                    (((1ULL<<M154_TX_MPDU_050_LEN)-1)<<M154_TX_MPDU_050_POS)
#define M154_TX_MPDU_050_UMSK                                   (~(((1ULL<<M154_TX_MPDU_050_LEN)-1)<<M154_TX_MPDU_050_POS))
#define M154_TX_MPDU_051                                        M154_TX_MPDU_051
#define M154_TX_MPDU_051_POS                                    (24U)
#define M154_TX_MPDU_051_LEN                                    (8U)
#define M154_TX_MPDU_051_MSK                                    (((1ULL<<M154_TX_MPDU_051_LEN)-1)<<M154_TX_MPDU_051_POS)
#define M154_TX_MPDU_051_UMSK                                   (~(((1ULL<<M154_TX_MPDU_051_LEN)-1)<<M154_TX_MPDU_051_POS))

/* 0x234 : m154_tx_mpdu_13 */
#define M154_TX_MPDU_13_OFFSET                                  (0x234)
#define M154_TX_MPDU_052                                        M154_TX_MPDU_052
#define M154_TX_MPDU_052_POS                                    (0U)
#define M154_TX_MPDU_052_LEN                                    (8U)
#define M154_TX_MPDU_052_MSK                                    (((1ULL<<M154_TX_MPDU_052_LEN)-1)<<M154_TX_MPDU_052_POS)
#define M154_TX_MPDU_052_UMSK                                   (~(((1ULL<<M154_TX_MPDU_052_LEN)-1)<<M154_TX_MPDU_052_POS))
#define M154_TX_MPDU_053                                        M154_TX_MPDU_053
#define M154_TX_MPDU_053_POS                                    (8U)
#define M154_TX_MPDU_053_LEN                                    (8U)
#define M154_TX_MPDU_053_MSK                                    (((1ULL<<M154_TX_MPDU_053_LEN)-1)<<M154_TX_MPDU_053_POS)
#define M154_TX_MPDU_053_UMSK                                   (~(((1ULL<<M154_TX_MPDU_053_LEN)-1)<<M154_TX_MPDU_053_POS))
#define M154_TX_MPDU_054                                        M154_TX_MPDU_054
#define M154_TX_MPDU_054_POS                                    (16U)
#define M154_TX_MPDU_054_LEN                                    (8U)
#define M154_TX_MPDU_054_MSK                                    (((1ULL<<M154_TX_MPDU_054_LEN)-1)<<M154_TX_MPDU_054_POS)
#define M154_TX_MPDU_054_UMSK                                   (~(((1ULL<<M154_TX_MPDU_054_LEN)-1)<<M154_TX_MPDU_054_POS))
#define M154_TX_MPDU_055                                        M154_TX_MPDU_055
#define M154_TX_MPDU_055_POS                                    (24U)
#define M154_TX_MPDU_055_LEN                                    (8U)
#define M154_TX_MPDU_055_MSK                                    (((1ULL<<M154_TX_MPDU_055_LEN)-1)<<M154_TX_MPDU_055_POS)
#define M154_TX_MPDU_055_UMSK                                   (~(((1ULL<<M154_TX_MPDU_055_LEN)-1)<<M154_TX_MPDU_055_POS))

/* 0x238 : m154_tx_mpdu_14 */
#define M154_TX_MPDU_14_OFFSET                                  (0x238)
#define M154_TX_MPDU_056                                        M154_TX_MPDU_056
#define M154_TX_MPDU_056_POS                                    (0U)
#define M154_TX_MPDU_056_LEN                                    (8U)
#define M154_TX_MPDU_056_MSK                                    (((1ULL<<M154_TX_MPDU_056_LEN)-1)<<M154_TX_MPDU_056_POS)
#define M154_TX_MPDU_056_UMSK                                   (~(((1ULL<<M154_TX_MPDU_056_LEN)-1)<<M154_TX_MPDU_056_POS))
#define M154_TX_MPDU_057                                        M154_TX_MPDU_057
#define M154_TX_MPDU_057_POS                                    (8U)
#define M154_TX_MPDU_057_LEN                                    (8U)
#define M154_TX_MPDU_057_MSK                                    (((1ULL<<M154_TX_MPDU_057_LEN)-1)<<M154_TX_MPDU_057_POS)
#define M154_TX_MPDU_057_UMSK                                   (~(((1ULL<<M154_TX_MPDU_057_LEN)-1)<<M154_TX_MPDU_057_POS))
#define M154_TX_MPDU_058                                        M154_TX_MPDU_058
#define M154_TX_MPDU_058_POS                                    (16U)
#define M154_TX_MPDU_058_LEN                                    (8U)
#define M154_TX_MPDU_058_MSK                                    (((1ULL<<M154_TX_MPDU_058_LEN)-1)<<M154_TX_MPDU_058_POS)
#define M154_TX_MPDU_058_UMSK                                   (~(((1ULL<<M154_TX_MPDU_058_LEN)-1)<<M154_TX_MPDU_058_POS))
#define M154_TX_MPDU_059                                        M154_TX_MPDU_059
#define M154_TX_MPDU_059_POS                                    (24U)
#define M154_TX_MPDU_059_LEN                                    (8U)
#define M154_TX_MPDU_059_MSK                                    (((1ULL<<M154_TX_MPDU_059_LEN)-1)<<M154_TX_MPDU_059_POS)
#define M154_TX_MPDU_059_UMSK                                   (~(((1ULL<<M154_TX_MPDU_059_LEN)-1)<<M154_TX_MPDU_059_POS))

/* 0x23C : m154_tx_mpdu_15 */
#define M154_TX_MPDU_15_OFFSET                                  (0x23C)
#define M154_TX_MPDU_060                                        M154_TX_MPDU_060
#define M154_TX_MPDU_060_POS                                    (0U)
#define M154_TX_MPDU_060_LEN                                    (8U)
#define M154_TX_MPDU_060_MSK                                    (((1ULL<<M154_TX_MPDU_060_LEN)-1)<<M154_TX_MPDU_060_POS)
#define M154_TX_MPDU_060_UMSK                                   (~(((1ULL<<M154_TX_MPDU_060_LEN)-1)<<M154_TX_MPDU_060_POS))
#define M154_TX_MPDU_061                                        M154_TX_MPDU_061
#define M154_TX_MPDU_061_POS                                    (8U)
#define M154_TX_MPDU_061_LEN                                    (8U)
#define M154_TX_MPDU_061_MSK                                    (((1ULL<<M154_TX_MPDU_061_LEN)-1)<<M154_TX_MPDU_061_POS)
#define M154_TX_MPDU_061_UMSK                                   (~(((1ULL<<M154_TX_MPDU_061_LEN)-1)<<M154_TX_MPDU_061_POS))
#define M154_TX_MPDU_062                                        M154_TX_MPDU_062
#define M154_TX_MPDU_062_POS                                    (16U)
#define M154_TX_MPDU_062_LEN                                    (8U)
#define M154_TX_MPDU_062_MSK                                    (((1ULL<<M154_TX_MPDU_062_LEN)-1)<<M154_TX_MPDU_062_POS)
#define M154_TX_MPDU_062_UMSK                                   (~(((1ULL<<M154_TX_MPDU_062_LEN)-1)<<M154_TX_MPDU_062_POS))
#define M154_TX_MPDU_063                                        M154_TX_MPDU_063
#define M154_TX_MPDU_063_POS                                    (24U)
#define M154_TX_MPDU_063_LEN                                    (8U)
#define M154_TX_MPDU_063_MSK                                    (((1ULL<<M154_TX_MPDU_063_LEN)-1)<<M154_TX_MPDU_063_POS)
#define M154_TX_MPDU_063_UMSK                                   (~(((1ULL<<M154_TX_MPDU_063_LEN)-1)<<M154_TX_MPDU_063_POS))

/* 0x240 : m154_tx_mpdu_16 */
#define M154_TX_MPDU_16_OFFSET                                  (0x240)
#define M154_TX_MPDU_064                                        M154_TX_MPDU_064
#define M154_TX_MPDU_064_POS                                    (0U)
#define M154_TX_MPDU_064_LEN                                    (8U)
#define M154_TX_MPDU_064_MSK                                    (((1ULL<<M154_TX_MPDU_064_LEN)-1)<<M154_TX_MPDU_064_POS)
#define M154_TX_MPDU_064_UMSK                                   (~(((1ULL<<M154_TX_MPDU_064_LEN)-1)<<M154_TX_MPDU_064_POS))
#define M154_TX_MPDU_065                                        M154_TX_MPDU_065
#define M154_TX_MPDU_065_POS                                    (8U)
#define M154_TX_MPDU_065_LEN                                    (8U)
#define M154_TX_MPDU_065_MSK                                    (((1ULL<<M154_TX_MPDU_065_LEN)-1)<<M154_TX_MPDU_065_POS)
#define M154_TX_MPDU_065_UMSK                                   (~(((1ULL<<M154_TX_MPDU_065_LEN)-1)<<M154_TX_MPDU_065_POS))
#define M154_TX_MPDU_066                                        M154_TX_MPDU_066
#define M154_TX_MPDU_066_POS                                    (16U)
#define M154_TX_MPDU_066_LEN                                    (8U)
#define M154_TX_MPDU_066_MSK                                    (((1ULL<<M154_TX_MPDU_066_LEN)-1)<<M154_TX_MPDU_066_POS)
#define M154_TX_MPDU_066_UMSK                                   (~(((1ULL<<M154_TX_MPDU_066_LEN)-1)<<M154_TX_MPDU_066_POS))
#define M154_TX_MPDU_067                                        M154_TX_MPDU_067
#define M154_TX_MPDU_067_POS                                    (24U)
#define M154_TX_MPDU_067_LEN                                    (8U)
#define M154_TX_MPDU_067_MSK                                    (((1ULL<<M154_TX_MPDU_067_LEN)-1)<<M154_TX_MPDU_067_POS)
#define M154_TX_MPDU_067_UMSK                                   (~(((1ULL<<M154_TX_MPDU_067_LEN)-1)<<M154_TX_MPDU_067_POS))

/* 0x244 : m154_tx_mpdu_17 */
#define M154_TX_MPDU_17_OFFSET                                  (0x244)
#define M154_TX_MPDU_068                                        M154_TX_MPDU_068
#define M154_TX_MPDU_068_POS                                    (0U)
#define M154_TX_MPDU_068_LEN                                    (8U)
#define M154_TX_MPDU_068_MSK                                    (((1ULL<<M154_TX_MPDU_068_LEN)-1)<<M154_TX_MPDU_068_POS)
#define M154_TX_MPDU_068_UMSK                                   (~(((1ULL<<M154_TX_MPDU_068_LEN)-1)<<M154_TX_MPDU_068_POS))
#define M154_TX_MPDU_069                                        M154_TX_MPDU_069
#define M154_TX_MPDU_069_POS                                    (8U)
#define M154_TX_MPDU_069_LEN                                    (8U)
#define M154_TX_MPDU_069_MSK                                    (((1ULL<<M154_TX_MPDU_069_LEN)-1)<<M154_TX_MPDU_069_POS)
#define M154_TX_MPDU_069_UMSK                                   (~(((1ULL<<M154_TX_MPDU_069_LEN)-1)<<M154_TX_MPDU_069_POS))
#define M154_TX_MPDU_070                                        M154_TX_MPDU_070
#define M154_TX_MPDU_070_POS                                    (16U)
#define M154_TX_MPDU_070_LEN                                    (8U)
#define M154_TX_MPDU_070_MSK                                    (((1ULL<<M154_TX_MPDU_070_LEN)-1)<<M154_TX_MPDU_070_POS)
#define M154_TX_MPDU_070_UMSK                                   (~(((1ULL<<M154_TX_MPDU_070_LEN)-1)<<M154_TX_MPDU_070_POS))
#define M154_TX_MPDU_071                                        M154_TX_MPDU_071
#define M154_TX_MPDU_071_POS                                    (24U)
#define M154_TX_MPDU_071_LEN                                    (8U)
#define M154_TX_MPDU_071_MSK                                    (((1ULL<<M154_TX_MPDU_071_LEN)-1)<<M154_TX_MPDU_071_POS)
#define M154_TX_MPDU_071_UMSK                                   (~(((1ULL<<M154_TX_MPDU_071_LEN)-1)<<M154_TX_MPDU_071_POS))

/* 0x248 : m154_tx_mpdu_18 */
#define M154_TX_MPDU_18_OFFSET                                  (0x248)
#define M154_TX_MPDU_072                                        M154_TX_MPDU_072
#define M154_TX_MPDU_072_POS                                    (0U)
#define M154_TX_MPDU_072_LEN                                    (8U)
#define M154_TX_MPDU_072_MSK                                    (((1ULL<<M154_TX_MPDU_072_LEN)-1)<<M154_TX_MPDU_072_POS)
#define M154_TX_MPDU_072_UMSK                                   (~(((1ULL<<M154_TX_MPDU_072_LEN)-1)<<M154_TX_MPDU_072_POS))
#define M154_TX_MPDU_073                                        M154_TX_MPDU_073
#define M154_TX_MPDU_073_POS                                    (8U)
#define M154_TX_MPDU_073_LEN                                    (8U)
#define M154_TX_MPDU_073_MSK                                    (((1ULL<<M154_TX_MPDU_073_LEN)-1)<<M154_TX_MPDU_073_POS)
#define M154_TX_MPDU_073_UMSK                                   (~(((1ULL<<M154_TX_MPDU_073_LEN)-1)<<M154_TX_MPDU_073_POS))
#define M154_TX_MPDU_074                                        M154_TX_MPDU_074
#define M154_TX_MPDU_074_POS                                    (16U)
#define M154_TX_MPDU_074_LEN                                    (8U)
#define M154_TX_MPDU_074_MSK                                    (((1ULL<<M154_TX_MPDU_074_LEN)-1)<<M154_TX_MPDU_074_POS)
#define M154_TX_MPDU_074_UMSK                                   (~(((1ULL<<M154_TX_MPDU_074_LEN)-1)<<M154_TX_MPDU_074_POS))
#define M154_TX_MPDU_075                                        M154_TX_MPDU_075
#define M154_TX_MPDU_075_POS                                    (24U)
#define M154_TX_MPDU_075_LEN                                    (8U)
#define M154_TX_MPDU_075_MSK                                    (((1ULL<<M154_TX_MPDU_075_LEN)-1)<<M154_TX_MPDU_075_POS)
#define M154_TX_MPDU_075_UMSK                                   (~(((1ULL<<M154_TX_MPDU_075_LEN)-1)<<M154_TX_MPDU_075_POS))

/* 0x24C : m154_tx_mpdu_19 */
#define M154_TX_MPDU_19_OFFSET                                  (0x24C)
#define M154_TX_MPDU_076                                        M154_TX_MPDU_076
#define M154_TX_MPDU_076_POS                                    (0U)
#define M154_TX_MPDU_076_LEN                                    (8U)
#define M154_TX_MPDU_076_MSK                                    (((1ULL<<M154_TX_MPDU_076_LEN)-1)<<M154_TX_MPDU_076_POS)
#define M154_TX_MPDU_076_UMSK                                   (~(((1ULL<<M154_TX_MPDU_076_LEN)-1)<<M154_TX_MPDU_076_POS))
#define M154_TX_MPDU_077                                        M154_TX_MPDU_077
#define M154_TX_MPDU_077_POS                                    (8U)
#define M154_TX_MPDU_077_LEN                                    (8U)
#define M154_TX_MPDU_077_MSK                                    (((1ULL<<M154_TX_MPDU_077_LEN)-1)<<M154_TX_MPDU_077_POS)
#define M154_TX_MPDU_077_UMSK                                   (~(((1ULL<<M154_TX_MPDU_077_LEN)-1)<<M154_TX_MPDU_077_POS))
#define M154_TX_MPDU_078                                        M154_TX_MPDU_078
#define M154_TX_MPDU_078_POS                                    (16U)
#define M154_TX_MPDU_078_LEN                                    (8U)
#define M154_TX_MPDU_078_MSK                                    (((1ULL<<M154_TX_MPDU_078_LEN)-1)<<M154_TX_MPDU_078_POS)
#define M154_TX_MPDU_078_UMSK                                   (~(((1ULL<<M154_TX_MPDU_078_LEN)-1)<<M154_TX_MPDU_078_POS))
#define M154_TX_MPDU_079                                        M154_TX_MPDU_079
#define M154_TX_MPDU_079_POS                                    (24U)
#define M154_TX_MPDU_079_LEN                                    (8U)
#define M154_TX_MPDU_079_MSK                                    (((1ULL<<M154_TX_MPDU_079_LEN)-1)<<M154_TX_MPDU_079_POS)
#define M154_TX_MPDU_079_UMSK                                   (~(((1ULL<<M154_TX_MPDU_079_LEN)-1)<<M154_TX_MPDU_079_POS))

/* 0x250 : m154_tx_mpdu_20 */
#define M154_TX_MPDU_20_OFFSET                                  (0x250)
#define M154_TX_MPDU_080                                        M154_TX_MPDU_080
#define M154_TX_MPDU_080_POS                                    (0U)
#define M154_TX_MPDU_080_LEN                                    (8U)
#define M154_TX_MPDU_080_MSK                                    (((1ULL<<M154_TX_MPDU_080_LEN)-1)<<M154_TX_MPDU_080_POS)
#define M154_TX_MPDU_080_UMSK                                   (~(((1ULL<<M154_TX_MPDU_080_LEN)-1)<<M154_TX_MPDU_080_POS))
#define M154_TX_MPDU_081                                        M154_TX_MPDU_081
#define M154_TX_MPDU_081_POS                                    (8U)
#define M154_TX_MPDU_081_LEN                                    (8U)
#define M154_TX_MPDU_081_MSK                                    (((1ULL<<M154_TX_MPDU_081_LEN)-1)<<M154_TX_MPDU_081_POS)
#define M154_TX_MPDU_081_UMSK                                   (~(((1ULL<<M154_TX_MPDU_081_LEN)-1)<<M154_TX_MPDU_081_POS))
#define M154_TX_MPDU_082                                        M154_TX_MPDU_082
#define M154_TX_MPDU_082_POS                                    (16U)
#define M154_TX_MPDU_082_LEN                                    (8U)
#define M154_TX_MPDU_082_MSK                                    (((1ULL<<M154_TX_MPDU_082_LEN)-1)<<M154_TX_MPDU_082_POS)
#define M154_TX_MPDU_082_UMSK                                   (~(((1ULL<<M154_TX_MPDU_082_LEN)-1)<<M154_TX_MPDU_082_POS))
#define M154_TX_MPDU_083                                        M154_TX_MPDU_083
#define M154_TX_MPDU_083_POS                                    (24U)
#define M154_TX_MPDU_083_LEN                                    (8U)
#define M154_TX_MPDU_083_MSK                                    (((1ULL<<M154_TX_MPDU_083_LEN)-1)<<M154_TX_MPDU_083_POS)
#define M154_TX_MPDU_083_UMSK                                   (~(((1ULL<<M154_TX_MPDU_083_LEN)-1)<<M154_TX_MPDU_083_POS))

/* 0x254 : m154_tx_mpdu_21 */
#define M154_TX_MPDU_21_OFFSET                                  (0x254)
#define M154_TX_MPDU_084                                        M154_TX_MPDU_084
#define M154_TX_MPDU_084_POS                                    (0U)
#define M154_TX_MPDU_084_LEN                                    (8U)
#define M154_TX_MPDU_084_MSK                                    (((1ULL<<M154_TX_MPDU_084_LEN)-1)<<M154_TX_MPDU_084_POS)
#define M154_TX_MPDU_084_UMSK                                   (~(((1ULL<<M154_TX_MPDU_084_LEN)-1)<<M154_TX_MPDU_084_POS))
#define M154_TX_MPDU_085                                        M154_TX_MPDU_085
#define M154_TX_MPDU_085_POS                                    (8U)
#define M154_TX_MPDU_085_LEN                                    (8U)
#define M154_TX_MPDU_085_MSK                                    (((1ULL<<M154_TX_MPDU_085_LEN)-1)<<M154_TX_MPDU_085_POS)
#define M154_TX_MPDU_085_UMSK                                   (~(((1ULL<<M154_TX_MPDU_085_LEN)-1)<<M154_TX_MPDU_085_POS))
#define M154_TX_MPDU_086                                        M154_TX_MPDU_086
#define M154_TX_MPDU_086_POS                                    (16U)
#define M154_TX_MPDU_086_LEN                                    (8U)
#define M154_TX_MPDU_086_MSK                                    (((1ULL<<M154_TX_MPDU_086_LEN)-1)<<M154_TX_MPDU_086_POS)
#define M154_TX_MPDU_086_UMSK                                   (~(((1ULL<<M154_TX_MPDU_086_LEN)-1)<<M154_TX_MPDU_086_POS))
#define M154_TX_MPDU_087                                        M154_TX_MPDU_087
#define M154_TX_MPDU_087_POS                                    (24U)
#define M154_TX_MPDU_087_LEN                                    (8U)
#define M154_TX_MPDU_087_MSK                                    (((1ULL<<M154_TX_MPDU_087_LEN)-1)<<M154_TX_MPDU_087_POS)
#define M154_TX_MPDU_087_UMSK                                   (~(((1ULL<<M154_TX_MPDU_087_LEN)-1)<<M154_TX_MPDU_087_POS))

/* 0x258 : m154_tx_mpdu_22 */
#define M154_TX_MPDU_22_OFFSET                                  (0x258)
#define M154_TX_MPDU_088                                        M154_TX_MPDU_088
#define M154_TX_MPDU_088_POS                                    (0U)
#define M154_TX_MPDU_088_LEN                                    (8U)
#define M154_TX_MPDU_088_MSK                                    (((1ULL<<M154_TX_MPDU_088_LEN)-1)<<M154_TX_MPDU_088_POS)
#define M154_TX_MPDU_088_UMSK                                   (~(((1ULL<<M154_TX_MPDU_088_LEN)-1)<<M154_TX_MPDU_088_POS))
#define M154_TX_MPDU_089                                        M154_TX_MPDU_089
#define M154_TX_MPDU_089_POS                                    (8U)
#define M154_TX_MPDU_089_LEN                                    (8U)
#define M154_TX_MPDU_089_MSK                                    (((1ULL<<M154_TX_MPDU_089_LEN)-1)<<M154_TX_MPDU_089_POS)
#define M154_TX_MPDU_089_UMSK                                   (~(((1ULL<<M154_TX_MPDU_089_LEN)-1)<<M154_TX_MPDU_089_POS))
#define M154_TX_MPDU_090                                        M154_TX_MPDU_090
#define M154_TX_MPDU_090_POS                                    (16U)
#define M154_TX_MPDU_090_LEN                                    (8U)
#define M154_TX_MPDU_090_MSK                                    (((1ULL<<M154_TX_MPDU_090_LEN)-1)<<M154_TX_MPDU_090_POS)
#define M154_TX_MPDU_090_UMSK                                   (~(((1ULL<<M154_TX_MPDU_090_LEN)-1)<<M154_TX_MPDU_090_POS))
#define M154_TX_MPDU_091                                        M154_TX_MPDU_091
#define M154_TX_MPDU_091_POS                                    (24U)
#define M154_TX_MPDU_091_LEN                                    (8U)
#define M154_TX_MPDU_091_MSK                                    (((1ULL<<M154_TX_MPDU_091_LEN)-1)<<M154_TX_MPDU_091_POS)
#define M154_TX_MPDU_091_UMSK                                   (~(((1ULL<<M154_TX_MPDU_091_LEN)-1)<<M154_TX_MPDU_091_POS))

/* 0x25C : m154_tx_mpdu_23 */
#define M154_TX_MPDU_23_OFFSET                                  (0x25C)
#define M154_TX_MPDU_092                                        M154_TX_MPDU_092
#define M154_TX_MPDU_092_POS                                    (0U)
#define M154_TX_MPDU_092_LEN                                    (8U)
#define M154_TX_MPDU_092_MSK                                    (((1ULL<<M154_TX_MPDU_092_LEN)-1)<<M154_TX_MPDU_092_POS)
#define M154_TX_MPDU_092_UMSK                                   (~(((1ULL<<M154_TX_MPDU_092_LEN)-1)<<M154_TX_MPDU_092_POS))
#define M154_TX_MPDU_093                                        M154_TX_MPDU_093
#define M154_TX_MPDU_093_POS                                    (8U)
#define M154_TX_MPDU_093_LEN                                    (8U)
#define M154_TX_MPDU_093_MSK                                    (((1ULL<<M154_TX_MPDU_093_LEN)-1)<<M154_TX_MPDU_093_POS)
#define M154_TX_MPDU_093_UMSK                                   (~(((1ULL<<M154_TX_MPDU_093_LEN)-1)<<M154_TX_MPDU_093_POS))
#define M154_TX_MPDU_094                                        M154_TX_MPDU_094
#define M154_TX_MPDU_094_POS                                    (16U)
#define M154_TX_MPDU_094_LEN                                    (8U)
#define M154_TX_MPDU_094_MSK                                    (((1ULL<<M154_TX_MPDU_094_LEN)-1)<<M154_TX_MPDU_094_POS)
#define M154_TX_MPDU_094_UMSK                                   (~(((1ULL<<M154_TX_MPDU_094_LEN)-1)<<M154_TX_MPDU_094_POS))
#define M154_TX_MPDU_095                                        M154_TX_MPDU_095
#define M154_TX_MPDU_095_POS                                    (24U)
#define M154_TX_MPDU_095_LEN                                    (8U)
#define M154_TX_MPDU_095_MSK                                    (((1ULL<<M154_TX_MPDU_095_LEN)-1)<<M154_TX_MPDU_095_POS)
#define M154_TX_MPDU_095_UMSK                                   (~(((1ULL<<M154_TX_MPDU_095_LEN)-1)<<M154_TX_MPDU_095_POS))

/* 0x260 : m154_tx_mpdu_24 */
#define M154_TX_MPDU_24_OFFSET                                  (0x260)
#define M154_TX_MPDU_096                                        M154_TX_MPDU_096
#define M154_TX_MPDU_096_POS                                    (0U)
#define M154_TX_MPDU_096_LEN                                    (8U)
#define M154_TX_MPDU_096_MSK                                    (((1ULL<<M154_TX_MPDU_096_LEN)-1)<<M154_TX_MPDU_096_POS)
#define M154_TX_MPDU_096_UMSK                                   (~(((1ULL<<M154_TX_MPDU_096_LEN)-1)<<M154_TX_MPDU_096_POS))
#define M154_TX_MPDU_097                                        M154_TX_MPDU_097
#define M154_TX_MPDU_097_POS                                    (8U)
#define M154_TX_MPDU_097_LEN                                    (8U)
#define M154_TX_MPDU_097_MSK                                    (((1ULL<<M154_TX_MPDU_097_LEN)-1)<<M154_TX_MPDU_097_POS)
#define M154_TX_MPDU_097_UMSK                                   (~(((1ULL<<M154_TX_MPDU_097_LEN)-1)<<M154_TX_MPDU_097_POS))
#define M154_TX_MPDU_098                                        M154_TX_MPDU_098
#define M154_TX_MPDU_098_POS                                    (16U)
#define M154_TX_MPDU_098_LEN                                    (8U)
#define M154_TX_MPDU_098_MSK                                    (((1ULL<<M154_TX_MPDU_098_LEN)-1)<<M154_TX_MPDU_098_POS)
#define M154_TX_MPDU_098_UMSK                                   (~(((1ULL<<M154_TX_MPDU_098_LEN)-1)<<M154_TX_MPDU_098_POS))
#define M154_TX_MPDU_099                                        M154_TX_MPDU_099
#define M154_TX_MPDU_099_POS                                    (24U)
#define M154_TX_MPDU_099_LEN                                    (8U)
#define M154_TX_MPDU_099_MSK                                    (((1ULL<<M154_TX_MPDU_099_LEN)-1)<<M154_TX_MPDU_099_POS)
#define M154_TX_MPDU_099_UMSK                                   (~(((1ULL<<M154_TX_MPDU_099_LEN)-1)<<M154_TX_MPDU_099_POS))

/* 0x264 : m154_tx_mpdu_25 */
#define M154_TX_MPDU_25_OFFSET                                  (0x264)
#define M154_TX_MPDU_100                                        M154_TX_MPDU_100
#define M154_TX_MPDU_100_POS                                    (0U)
#define M154_TX_MPDU_100_LEN                                    (8U)
#define M154_TX_MPDU_100_MSK                                    (((1ULL<<M154_TX_MPDU_100_LEN)-1)<<M154_TX_MPDU_100_POS)
#define M154_TX_MPDU_100_UMSK                                   (~(((1ULL<<M154_TX_MPDU_100_LEN)-1)<<M154_TX_MPDU_100_POS))
#define M154_TX_MPDU_101                                        M154_TX_MPDU_101
#define M154_TX_MPDU_101_POS                                    (8U)
#define M154_TX_MPDU_101_LEN                                    (8U)
#define M154_TX_MPDU_101_MSK                                    (((1ULL<<M154_TX_MPDU_101_LEN)-1)<<M154_TX_MPDU_101_POS)
#define M154_TX_MPDU_101_UMSK                                   (~(((1ULL<<M154_TX_MPDU_101_LEN)-1)<<M154_TX_MPDU_101_POS))
#define M154_TX_MPDU_102                                        M154_TX_MPDU_102
#define M154_TX_MPDU_102_POS                                    (16U)
#define M154_TX_MPDU_102_LEN                                    (8U)
#define M154_TX_MPDU_102_MSK                                    (((1ULL<<M154_TX_MPDU_102_LEN)-1)<<M154_TX_MPDU_102_POS)
#define M154_TX_MPDU_102_UMSK                                   (~(((1ULL<<M154_TX_MPDU_102_LEN)-1)<<M154_TX_MPDU_102_POS))
#define M154_TX_MPDU_103                                        M154_TX_MPDU_103
#define M154_TX_MPDU_103_POS                                    (24U)
#define M154_TX_MPDU_103_LEN                                    (8U)
#define M154_TX_MPDU_103_MSK                                    (((1ULL<<M154_TX_MPDU_103_LEN)-1)<<M154_TX_MPDU_103_POS)
#define M154_TX_MPDU_103_UMSK                                   (~(((1ULL<<M154_TX_MPDU_103_LEN)-1)<<M154_TX_MPDU_103_POS))

/* 0x268 : m154_tx_mpdu_26 */
#define M154_TX_MPDU_26_OFFSET                                  (0x268)
#define M154_TX_MPDU_104                                        M154_TX_MPDU_104
#define M154_TX_MPDU_104_POS                                    (0U)
#define M154_TX_MPDU_104_LEN                                    (8U)
#define M154_TX_MPDU_104_MSK                                    (((1ULL<<M154_TX_MPDU_104_LEN)-1)<<M154_TX_MPDU_104_POS)
#define M154_TX_MPDU_104_UMSK                                   (~(((1ULL<<M154_TX_MPDU_104_LEN)-1)<<M154_TX_MPDU_104_POS))
#define M154_TX_MPDU_105                                        M154_TX_MPDU_105
#define M154_TX_MPDU_105_POS                                    (8U)
#define M154_TX_MPDU_105_LEN                                    (8U)
#define M154_TX_MPDU_105_MSK                                    (((1ULL<<M154_TX_MPDU_105_LEN)-1)<<M154_TX_MPDU_105_POS)
#define M154_TX_MPDU_105_UMSK                                   (~(((1ULL<<M154_TX_MPDU_105_LEN)-1)<<M154_TX_MPDU_105_POS))
#define M154_TX_MPDU_106                                        M154_TX_MPDU_106
#define M154_TX_MPDU_106_POS                                    (16U)
#define M154_TX_MPDU_106_LEN                                    (8U)
#define M154_TX_MPDU_106_MSK                                    (((1ULL<<M154_TX_MPDU_106_LEN)-1)<<M154_TX_MPDU_106_POS)
#define M154_TX_MPDU_106_UMSK                                   (~(((1ULL<<M154_TX_MPDU_106_LEN)-1)<<M154_TX_MPDU_106_POS))
#define M154_TX_MPDU_107                                        M154_TX_MPDU_107
#define M154_TX_MPDU_107_POS                                    (24U)
#define M154_TX_MPDU_107_LEN                                    (8U)
#define M154_TX_MPDU_107_MSK                                    (((1ULL<<M154_TX_MPDU_107_LEN)-1)<<M154_TX_MPDU_107_POS)
#define M154_TX_MPDU_107_UMSK                                   (~(((1ULL<<M154_TX_MPDU_107_LEN)-1)<<M154_TX_MPDU_107_POS))

/* 0x26C : m154_tx_mpdu_27 */
#define M154_TX_MPDU_27_OFFSET                                  (0x26C)
#define M154_TX_MPDU_108                                        M154_TX_MPDU_108
#define M154_TX_MPDU_108_POS                                    (0U)
#define M154_TX_MPDU_108_LEN                                    (8U)
#define M154_TX_MPDU_108_MSK                                    (((1ULL<<M154_TX_MPDU_108_LEN)-1)<<M154_TX_MPDU_108_POS)
#define M154_TX_MPDU_108_UMSK                                   (~(((1ULL<<M154_TX_MPDU_108_LEN)-1)<<M154_TX_MPDU_108_POS))
#define M154_TX_MPDU_109                                        M154_TX_MPDU_109
#define M154_TX_MPDU_109_POS                                    (8U)
#define M154_TX_MPDU_109_LEN                                    (8U)
#define M154_TX_MPDU_109_MSK                                    (((1ULL<<M154_TX_MPDU_109_LEN)-1)<<M154_TX_MPDU_109_POS)
#define M154_TX_MPDU_109_UMSK                                   (~(((1ULL<<M154_TX_MPDU_109_LEN)-1)<<M154_TX_MPDU_109_POS))
#define M154_TX_MPDU_110                                        M154_TX_MPDU_110
#define M154_TX_MPDU_110_POS                                    (16U)
#define M154_TX_MPDU_110_LEN                                    (8U)
#define M154_TX_MPDU_110_MSK                                    (((1ULL<<M154_TX_MPDU_110_LEN)-1)<<M154_TX_MPDU_110_POS)
#define M154_TX_MPDU_110_UMSK                                   (~(((1ULL<<M154_TX_MPDU_110_LEN)-1)<<M154_TX_MPDU_110_POS))
#define M154_TX_MPDU_111                                        M154_TX_MPDU_111
#define M154_TX_MPDU_111_POS                                    (24U)
#define M154_TX_MPDU_111_LEN                                    (8U)
#define M154_TX_MPDU_111_MSK                                    (((1ULL<<M154_TX_MPDU_111_LEN)-1)<<M154_TX_MPDU_111_POS)
#define M154_TX_MPDU_111_UMSK                                   (~(((1ULL<<M154_TX_MPDU_111_LEN)-1)<<M154_TX_MPDU_111_POS))

/* 0x270 : m154_tx_mpdu_28 */
#define M154_TX_MPDU_28_OFFSET                                  (0x270)
#define M154_TX_MPDU_112                                        M154_TX_MPDU_112
#define M154_TX_MPDU_112_POS                                    (0U)
#define M154_TX_MPDU_112_LEN                                    (8U)
#define M154_TX_MPDU_112_MSK                                    (((1ULL<<M154_TX_MPDU_112_LEN)-1)<<M154_TX_MPDU_112_POS)
#define M154_TX_MPDU_112_UMSK                                   (~(((1ULL<<M154_TX_MPDU_112_LEN)-1)<<M154_TX_MPDU_112_POS))
#define M154_TX_MPDU_113                                        M154_TX_MPDU_113
#define M154_TX_MPDU_113_POS                                    (8U)
#define M154_TX_MPDU_113_LEN                                    (8U)
#define M154_TX_MPDU_113_MSK                                    (((1ULL<<M154_TX_MPDU_113_LEN)-1)<<M154_TX_MPDU_113_POS)
#define M154_TX_MPDU_113_UMSK                                   (~(((1ULL<<M154_TX_MPDU_113_LEN)-1)<<M154_TX_MPDU_113_POS))
#define M154_TX_MPDU_114                                        M154_TX_MPDU_114
#define M154_TX_MPDU_114_POS                                    (16U)
#define M154_TX_MPDU_114_LEN                                    (8U)
#define M154_TX_MPDU_114_MSK                                    (((1ULL<<M154_TX_MPDU_114_LEN)-1)<<M154_TX_MPDU_114_POS)
#define M154_TX_MPDU_114_UMSK                                   (~(((1ULL<<M154_TX_MPDU_114_LEN)-1)<<M154_TX_MPDU_114_POS))
#define M154_TX_MPDU_115                                        M154_TX_MPDU_115
#define M154_TX_MPDU_115_POS                                    (24U)
#define M154_TX_MPDU_115_LEN                                    (8U)
#define M154_TX_MPDU_115_MSK                                    (((1ULL<<M154_TX_MPDU_115_LEN)-1)<<M154_TX_MPDU_115_POS)
#define M154_TX_MPDU_115_UMSK                                   (~(((1ULL<<M154_TX_MPDU_115_LEN)-1)<<M154_TX_MPDU_115_POS))

/* 0x274 : m154_tx_mpdu_29 */
#define M154_TX_MPDU_29_OFFSET                                  (0x274)
#define M154_TX_MPDU_116                                        M154_TX_MPDU_116
#define M154_TX_MPDU_116_POS                                    (0U)
#define M154_TX_MPDU_116_LEN                                    (8U)
#define M154_TX_MPDU_116_MSK                                    (((1ULL<<M154_TX_MPDU_116_LEN)-1)<<M154_TX_MPDU_116_POS)
#define M154_TX_MPDU_116_UMSK                                   (~(((1ULL<<M154_TX_MPDU_116_LEN)-1)<<M154_TX_MPDU_116_POS))
#define M154_TX_MPDU_117                                        M154_TX_MPDU_117
#define M154_TX_MPDU_117_POS                                    (8U)
#define M154_TX_MPDU_117_LEN                                    (8U)
#define M154_TX_MPDU_117_MSK                                    (((1ULL<<M154_TX_MPDU_117_LEN)-1)<<M154_TX_MPDU_117_POS)
#define M154_TX_MPDU_117_UMSK                                   (~(((1ULL<<M154_TX_MPDU_117_LEN)-1)<<M154_TX_MPDU_117_POS))
#define M154_TX_MPDU_118                                        M154_TX_MPDU_118
#define M154_TX_MPDU_118_POS                                    (16U)
#define M154_TX_MPDU_118_LEN                                    (8U)
#define M154_TX_MPDU_118_MSK                                    (((1ULL<<M154_TX_MPDU_118_LEN)-1)<<M154_TX_MPDU_118_POS)
#define M154_TX_MPDU_118_UMSK                                   (~(((1ULL<<M154_TX_MPDU_118_LEN)-1)<<M154_TX_MPDU_118_POS))
#define M154_TX_MPDU_119                                        M154_TX_MPDU_119
#define M154_TX_MPDU_119_POS                                    (24U)
#define M154_TX_MPDU_119_LEN                                    (8U)
#define M154_TX_MPDU_119_MSK                                    (((1ULL<<M154_TX_MPDU_119_LEN)-1)<<M154_TX_MPDU_119_POS)
#define M154_TX_MPDU_119_UMSK                                   (~(((1ULL<<M154_TX_MPDU_119_LEN)-1)<<M154_TX_MPDU_119_POS))

/* 0x278 : m154_tx_mpdu_30 */
#define M154_TX_MPDU_30_OFFSET                                  (0x278)
#define M154_TX_MPDU_120                                        M154_TX_MPDU_120
#define M154_TX_MPDU_120_POS                                    (0U)
#define M154_TX_MPDU_120_LEN                                    (8U)
#define M154_TX_MPDU_120_MSK                                    (((1ULL<<M154_TX_MPDU_120_LEN)-1)<<M154_TX_MPDU_120_POS)
#define M154_TX_MPDU_120_UMSK                                   (~(((1ULL<<M154_TX_MPDU_120_LEN)-1)<<M154_TX_MPDU_120_POS))
#define M154_TX_MPDU_121                                        M154_TX_MPDU_121
#define M154_TX_MPDU_121_POS                                    (8U)
#define M154_TX_MPDU_121_LEN                                    (8U)
#define M154_TX_MPDU_121_MSK                                    (((1ULL<<M154_TX_MPDU_121_LEN)-1)<<M154_TX_MPDU_121_POS)
#define M154_TX_MPDU_121_UMSK                                   (~(((1ULL<<M154_TX_MPDU_121_LEN)-1)<<M154_TX_MPDU_121_POS))
#define M154_TX_MPDU_122                                        M154_TX_MPDU_122
#define M154_TX_MPDU_122_POS                                    (16U)
#define M154_TX_MPDU_122_LEN                                    (8U)
#define M154_TX_MPDU_122_MSK                                    (((1ULL<<M154_TX_MPDU_122_LEN)-1)<<M154_TX_MPDU_122_POS)
#define M154_TX_MPDU_122_UMSK                                   (~(((1ULL<<M154_TX_MPDU_122_LEN)-1)<<M154_TX_MPDU_122_POS))
#define M154_TX_MPDU_123                                        M154_TX_MPDU_123
#define M154_TX_MPDU_123_POS                                    (24U)
#define M154_TX_MPDU_123_LEN                                    (8U)
#define M154_TX_MPDU_123_MSK                                    (((1ULL<<M154_TX_MPDU_123_LEN)-1)<<M154_TX_MPDU_123_POS)
#define M154_TX_MPDU_123_UMSK                                   (~(((1ULL<<M154_TX_MPDU_123_LEN)-1)<<M154_TX_MPDU_123_POS))

/* 0x27C : m154_tx_mpdu_31 */
#define M154_TX_MPDU_31_OFFSET                                  (0x27C)
#define M154_TX_MPDU_124                                        M154_TX_MPDU_124
#define M154_TX_MPDU_124_POS                                    (0U)
#define M154_TX_MPDU_124_LEN                                    (8U)
#define M154_TX_MPDU_124_MSK                                    (((1ULL<<M154_TX_MPDU_124_LEN)-1)<<M154_TX_MPDU_124_POS)
#define M154_TX_MPDU_124_UMSK                                   (~(((1ULL<<M154_TX_MPDU_124_LEN)-1)<<M154_TX_MPDU_124_POS))
#define M154_TX_MPDU_FCS_07_00                                  M154_TX_MPDU_FCS_07_00
#define M154_TX_MPDU_FCS_07_00_POS                              (8U)
#define M154_TX_MPDU_FCS_07_00_LEN                              (8U)
#define M154_TX_MPDU_FCS_07_00_MSK                              (((1ULL<<M154_TX_MPDU_FCS_07_00_LEN)-1)<<M154_TX_MPDU_FCS_07_00_POS)
#define M154_TX_MPDU_FCS_07_00_UMSK                             (~(((1ULL<<M154_TX_MPDU_FCS_07_00_LEN)-1)<<M154_TX_MPDU_FCS_07_00_POS))
#define M154_TX_MPDU_FCS_15_08                                  M154_TX_MPDU_FCS_15_08
#define M154_TX_MPDU_FCS_15_08_POS                              (16U)
#define M154_TX_MPDU_FCS_15_08_LEN                              (8U)
#define M154_TX_MPDU_FCS_15_08_MSK                              (((1ULL<<M154_TX_MPDU_FCS_15_08_LEN)-1)<<M154_TX_MPDU_FCS_15_08_POS)
#define M154_TX_MPDU_FCS_15_08_UMSK                             (~(((1ULL<<M154_TX_MPDU_FCS_15_08_LEN)-1)<<M154_TX_MPDU_FCS_15_08_POS))
#define M154_TX_PHR                                             M154_TX_PHR
#define M154_TX_PHR_POS                                         (24U)
#define M154_TX_PHR_LEN                                         (8U)
#define M154_TX_PHR_MSK                                         (((1ULL<<M154_TX_PHR_LEN)-1)<<M154_TX_PHR_POS)
#define M154_TX_PHR_UMSK                                        (~(((1ULL<<M154_TX_PHR_LEN)-1)<<M154_TX_PHR_POS))

/* 0x280 : m154_tx_ppdu_ctrl */
#define M154_TX_PPDU_CTRL_OFFSET                                (0x280)
#define M154_TX_SFD                                             M154_TX_SFD
#define M154_TX_SFD_POS                                         (0U)
#define M154_TX_SFD_LEN                                         (8U)
#define M154_TX_SFD_MSK                                         (((1ULL<<M154_TX_SFD_LEN)-1)<<M154_TX_SFD_POS)
#define M154_TX_SFD_UMSK                                        (~(((1ULL<<M154_TX_SFD_LEN)-1)<<M154_TX_SFD_POS))
#define M154_TX_MODE                                            M154_TX_MODE
#define M154_TX_MODE_POS                                        (8U)
#define M154_TX_MODE_LEN                                        (2U)
#define M154_TX_MODE_MSK                                        (((1ULL<<M154_TX_MODE_LEN)-1)<<M154_TX_MODE_POS)
#define M154_TX_MODE_UMSK                                       (~(((1ULL<<M154_TX_MODE_LEN)-1)<<M154_TX_MODE_POS))

/* 0x400 : m154_rx_mpdu_00 */
#define M154_RX_MPDU_00_OFFSET                                  (0x400)
#define M154_RX_MPDU_000                                        M154_RX_MPDU_000
#define M154_RX_MPDU_000_POS                                    (0U)
#define M154_RX_MPDU_000_LEN                                    (8U)
#define M154_RX_MPDU_000_MSK                                    (((1ULL<<M154_RX_MPDU_000_LEN)-1)<<M154_RX_MPDU_000_POS)
#define M154_RX_MPDU_000_UMSK                                   (~(((1ULL<<M154_RX_MPDU_000_LEN)-1)<<M154_RX_MPDU_000_POS))
#define M154_RX_MPDU_001                                        M154_RX_MPDU_001
#define M154_RX_MPDU_001_POS                                    (8U)
#define M154_RX_MPDU_001_LEN                                    (8U)
#define M154_RX_MPDU_001_MSK                                    (((1ULL<<M154_RX_MPDU_001_LEN)-1)<<M154_RX_MPDU_001_POS)
#define M154_RX_MPDU_001_UMSK                                   (~(((1ULL<<M154_RX_MPDU_001_LEN)-1)<<M154_RX_MPDU_001_POS))
#define M154_RX_MPDU_002                                        M154_RX_MPDU_002
#define M154_RX_MPDU_002_POS                                    (16U)
#define M154_RX_MPDU_002_LEN                                    (8U)
#define M154_RX_MPDU_002_MSK                                    (((1ULL<<M154_RX_MPDU_002_LEN)-1)<<M154_RX_MPDU_002_POS)
#define M154_RX_MPDU_002_UMSK                                   (~(((1ULL<<M154_RX_MPDU_002_LEN)-1)<<M154_RX_MPDU_002_POS))
#define M154_RX_MPDU_003                                        M154_RX_MPDU_003
#define M154_RX_MPDU_003_POS                                    (24U)
#define M154_RX_MPDU_003_LEN                                    (8U)
#define M154_RX_MPDU_003_MSK                                    (((1ULL<<M154_RX_MPDU_003_LEN)-1)<<M154_RX_MPDU_003_POS)
#define M154_RX_MPDU_003_UMSK                                   (~(((1ULL<<M154_RX_MPDU_003_LEN)-1)<<M154_RX_MPDU_003_POS))

/* 0x404 : m154_rx_mpdu_01 */
#define M154_RX_MPDU_01_OFFSET                                  (0x404)
#define M154_RX_MPDU_004                                        M154_RX_MPDU_004
#define M154_RX_MPDU_004_POS                                    (0U)
#define M154_RX_MPDU_004_LEN                                    (8U)
#define M154_RX_MPDU_004_MSK                                    (((1ULL<<M154_RX_MPDU_004_LEN)-1)<<M154_RX_MPDU_004_POS)
#define M154_RX_MPDU_004_UMSK                                   (~(((1ULL<<M154_RX_MPDU_004_LEN)-1)<<M154_RX_MPDU_004_POS))
#define M154_RX_MPDU_005                                        M154_RX_MPDU_005
#define M154_RX_MPDU_005_POS                                    (8U)
#define M154_RX_MPDU_005_LEN                                    (8U)
#define M154_RX_MPDU_005_MSK                                    (((1ULL<<M154_RX_MPDU_005_LEN)-1)<<M154_RX_MPDU_005_POS)
#define M154_RX_MPDU_005_UMSK                                   (~(((1ULL<<M154_RX_MPDU_005_LEN)-1)<<M154_RX_MPDU_005_POS))
#define M154_RX_MPDU_006                                        M154_RX_MPDU_006
#define M154_RX_MPDU_006_POS                                    (16U)
#define M154_RX_MPDU_006_LEN                                    (8U)
#define M154_RX_MPDU_006_MSK                                    (((1ULL<<M154_RX_MPDU_006_LEN)-1)<<M154_RX_MPDU_006_POS)
#define M154_RX_MPDU_006_UMSK                                   (~(((1ULL<<M154_RX_MPDU_006_LEN)-1)<<M154_RX_MPDU_006_POS))
#define M154_RX_MPDU_007                                        M154_RX_MPDU_007
#define M154_RX_MPDU_007_POS                                    (24U)
#define M154_RX_MPDU_007_LEN                                    (8U)
#define M154_RX_MPDU_007_MSK                                    (((1ULL<<M154_RX_MPDU_007_LEN)-1)<<M154_RX_MPDU_007_POS)
#define M154_RX_MPDU_007_UMSK                                   (~(((1ULL<<M154_RX_MPDU_007_LEN)-1)<<M154_RX_MPDU_007_POS))

/* 0x408 : m154_rx_mpdu_02 */
#define M154_RX_MPDU_02_OFFSET                                  (0x408)
#define M154_RX_MPDU_008                                        M154_RX_MPDU_008
#define M154_RX_MPDU_008_POS                                    (0U)
#define M154_RX_MPDU_008_LEN                                    (8U)
#define M154_RX_MPDU_008_MSK                                    (((1ULL<<M154_RX_MPDU_008_LEN)-1)<<M154_RX_MPDU_008_POS)
#define M154_RX_MPDU_008_UMSK                                   (~(((1ULL<<M154_RX_MPDU_008_LEN)-1)<<M154_RX_MPDU_008_POS))
#define M154_RX_MPDU_009                                        M154_RX_MPDU_009
#define M154_RX_MPDU_009_POS                                    (8U)
#define M154_RX_MPDU_009_LEN                                    (8U)
#define M154_RX_MPDU_009_MSK                                    (((1ULL<<M154_RX_MPDU_009_LEN)-1)<<M154_RX_MPDU_009_POS)
#define M154_RX_MPDU_009_UMSK                                   (~(((1ULL<<M154_RX_MPDU_009_LEN)-1)<<M154_RX_MPDU_009_POS))
#define M154_RX_MPDU_010                                        M154_RX_MPDU_010
#define M154_RX_MPDU_010_POS                                    (16U)
#define M154_RX_MPDU_010_LEN                                    (8U)
#define M154_RX_MPDU_010_MSK                                    (((1ULL<<M154_RX_MPDU_010_LEN)-1)<<M154_RX_MPDU_010_POS)
#define M154_RX_MPDU_010_UMSK                                   (~(((1ULL<<M154_RX_MPDU_010_LEN)-1)<<M154_RX_MPDU_010_POS))
#define M154_RX_MPDU_011                                        M154_RX_MPDU_011
#define M154_RX_MPDU_011_POS                                    (24U)
#define M154_RX_MPDU_011_LEN                                    (8U)
#define M154_RX_MPDU_011_MSK                                    (((1ULL<<M154_RX_MPDU_011_LEN)-1)<<M154_RX_MPDU_011_POS)
#define M154_RX_MPDU_011_UMSK                                   (~(((1ULL<<M154_RX_MPDU_011_LEN)-1)<<M154_RX_MPDU_011_POS))

/* 0x40C : m154_rx_mpdu_03 */
#define M154_RX_MPDU_03_OFFSET                                  (0x40C)
#define M154_RX_MPDU_012                                        M154_RX_MPDU_012
#define M154_RX_MPDU_012_POS                                    (0U)
#define M154_RX_MPDU_012_LEN                                    (8U)
#define M154_RX_MPDU_012_MSK                                    (((1ULL<<M154_RX_MPDU_012_LEN)-1)<<M154_RX_MPDU_012_POS)
#define M154_RX_MPDU_012_UMSK                                   (~(((1ULL<<M154_RX_MPDU_012_LEN)-1)<<M154_RX_MPDU_012_POS))
#define M154_RX_MPDU_013                                        M154_RX_MPDU_013
#define M154_RX_MPDU_013_POS                                    (8U)
#define M154_RX_MPDU_013_LEN                                    (8U)
#define M154_RX_MPDU_013_MSK                                    (((1ULL<<M154_RX_MPDU_013_LEN)-1)<<M154_RX_MPDU_013_POS)
#define M154_RX_MPDU_013_UMSK                                   (~(((1ULL<<M154_RX_MPDU_013_LEN)-1)<<M154_RX_MPDU_013_POS))
#define M154_RX_MPDU_014                                        M154_RX_MPDU_014
#define M154_RX_MPDU_014_POS                                    (16U)
#define M154_RX_MPDU_014_LEN                                    (8U)
#define M154_RX_MPDU_014_MSK                                    (((1ULL<<M154_RX_MPDU_014_LEN)-1)<<M154_RX_MPDU_014_POS)
#define M154_RX_MPDU_014_UMSK                                   (~(((1ULL<<M154_RX_MPDU_014_LEN)-1)<<M154_RX_MPDU_014_POS))
#define M154_RX_MPDU_015                                        M154_RX_MPDU_015
#define M154_RX_MPDU_015_POS                                    (24U)
#define M154_RX_MPDU_015_LEN                                    (8U)
#define M154_RX_MPDU_015_MSK                                    (((1ULL<<M154_RX_MPDU_015_LEN)-1)<<M154_RX_MPDU_015_POS)
#define M154_RX_MPDU_015_UMSK                                   (~(((1ULL<<M154_RX_MPDU_015_LEN)-1)<<M154_RX_MPDU_015_POS))

/* 0x410 : m154_rx_mpdu_04 */
#define M154_RX_MPDU_04_OFFSET                                  (0x410)
#define M154_RX_MPDU_016                                        M154_RX_MPDU_016
#define M154_RX_MPDU_016_POS                                    (0U)
#define M154_RX_MPDU_016_LEN                                    (8U)
#define M154_RX_MPDU_016_MSK                                    (((1ULL<<M154_RX_MPDU_016_LEN)-1)<<M154_RX_MPDU_016_POS)
#define M154_RX_MPDU_016_UMSK                                   (~(((1ULL<<M154_RX_MPDU_016_LEN)-1)<<M154_RX_MPDU_016_POS))
#define M154_RX_MPDU_017                                        M154_RX_MPDU_017
#define M154_RX_MPDU_017_POS                                    (8U)
#define M154_RX_MPDU_017_LEN                                    (8U)
#define M154_RX_MPDU_017_MSK                                    (((1ULL<<M154_RX_MPDU_017_LEN)-1)<<M154_RX_MPDU_017_POS)
#define M154_RX_MPDU_017_UMSK                                   (~(((1ULL<<M154_RX_MPDU_017_LEN)-1)<<M154_RX_MPDU_017_POS))
#define M154_RX_MPDU_018                                        M154_RX_MPDU_018
#define M154_RX_MPDU_018_POS                                    (16U)
#define M154_RX_MPDU_018_LEN                                    (8U)
#define M154_RX_MPDU_018_MSK                                    (((1ULL<<M154_RX_MPDU_018_LEN)-1)<<M154_RX_MPDU_018_POS)
#define M154_RX_MPDU_018_UMSK                                   (~(((1ULL<<M154_RX_MPDU_018_LEN)-1)<<M154_RX_MPDU_018_POS))
#define M154_RX_MPDU_019                                        M154_RX_MPDU_019
#define M154_RX_MPDU_019_POS                                    (24U)
#define M154_RX_MPDU_019_LEN                                    (8U)
#define M154_RX_MPDU_019_MSK                                    (((1ULL<<M154_RX_MPDU_019_LEN)-1)<<M154_RX_MPDU_019_POS)
#define M154_RX_MPDU_019_UMSK                                   (~(((1ULL<<M154_RX_MPDU_019_LEN)-1)<<M154_RX_MPDU_019_POS))

/* 0x414 : m154_rx_mpdu_05 */
#define M154_RX_MPDU_05_OFFSET                                  (0x414)
#define M154_RX_MPDU_020                                        M154_RX_MPDU_020
#define M154_RX_MPDU_020_POS                                    (0U)
#define M154_RX_MPDU_020_LEN                                    (8U)
#define M154_RX_MPDU_020_MSK                                    (((1ULL<<M154_RX_MPDU_020_LEN)-1)<<M154_RX_MPDU_020_POS)
#define M154_RX_MPDU_020_UMSK                                   (~(((1ULL<<M154_RX_MPDU_020_LEN)-1)<<M154_RX_MPDU_020_POS))
#define M154_RX_MPDU_021                                        M154_RX_MPDU_021
#define M154_RX_MPDU_021_POS                                    (8U)
#define M154_RX_MPDU_021_LEN                                    (8U)
#define M154_RX_MPDU_021_MSK                                    (((1ULL<<M154_RX_MPDU_021_LEN)-1)<<M154_RX_MPDU_021_POS)
#define M154_RX_MPDU_021_UMSK                                   (~(((1ULL<<M154_RX_MPDU_021_LEN)-1)<<M154_RX_MPDU_021_POS))
#define M154_RX_MPDU_022                                        M154_RX_MPDU_022
#define M154_RX_MPDU_022_POS                                    (16U)
#define M154_RX_MPDU_022_LEN                                    (8U)
#define M154_RX_MPDU_022_MSK                                    (((1ULL<<M154_RX_MPDU_022_LEN)-1)<<M154_RX_MPDU_022_POS)
#define M154_RX_MPDU_022_UMSK                                   (~(((1ULL<<M154_RX_MPDU_022_LEN)-1)<<M154_RX_MPDU_022_POS))
#define M154_RX_MPDU_023                                        M154_RX_MPDU_023
#define M154_RX_MPDU_023_POS                                    (24U)
#define M154_RX_MPDU_023_LEN                                    (8U)
#define M154_RX_MPDU_023_MSK                                    (((1ULL<<M154_RX_MPDU_023_LEN)-1)<<M154_RX_MPDU_023_POS)
#define M154_RX_MPDU_023_UMSK                                   (~(((1ULL<<M154_RX_MPDU_023_LEN)-1)<<M154_RX_MPDU_023_POS))

/* 0x418 : m154_rx_mpdu_06 */
#define M154_RX_MPDU_06_OFFSET                                  (0x418)
#define M154_RX_MPDU_024                                        M154_RX_MPDU_024
#define M154_RX_MPDU_024_POS                                    (0U)
#define M154_RX_MPDU_024_LEN                                    (8U)
#define M154_RX_MPDU_024_MSK                                    (((1ULL<<M154_RX_MPDU_024_LEN)-1)<<M154_RX_MPDU_024_POS)
#define M154_RX_MPDU_024_UMSK                                   (~(((1ULL<<M154_RX_MPDU_024_LEN)-1)<<M154_RX_MPDU_024_POS))
#define M154_RX_MPDU_025                                        M154_RX_MPDU_025
#define M154_RX_MPDU_025_POS                                    (8U)
#define M154_RX_MPDU_025_LEN                                    (8U)
#define M154_RX_MPDU_025_MSK                                    (((1ULL<<M154_RX_MPDU_025_LEN)-1)<<M154_RX_MPDU_025_POS)
#define M154_RX_MPDU_025_UMSK                                   (~(((1ULL<<M154_RX_MPDU_025_LEN)-1)<<M154_RX_MPDU_025_POS))
#define M154_RX_MPDU_026                                        M154_RX_MPDU_026
#define M154_RX_MPDU_026_POS                                    (16U)
#define M154_RX_MPDU_026_LEN                                    (8U)
#define M154_RX_MPDU_026_MSK                                    (((1ULL<<M154_RX_MPDU_026_LEN)-1)<<M154_RX_MPDU_026_POS)
#define M154_RX_MPDU_026_UMSK                                   (~(((1ULL<<M154_RX_MPDU_026_LEN)-1)<<M154_RX_MPDU_026_POS))
#define M154_RX_MPDU_027                                        M154_RX_MPDU_027
#define M154_RX_MPDU_027_POS                                    (24U)
#define M154_RX_MPDU_027_LEN                                    (8U)
#define M154_RX_MPDU_027_MSK                                    (((1ULL<<M154_RX_MPDU_027_LEN)-1)<<M154_RX_MPDU_027_POS)
#define M154_RX_MPDU_027_UMSK                                   (~(((1ULL<<M154_RX_MPDU_027_LEN)-1)<<M154_RX_MPDU_027_POS))

/* 0x41C : m154_rx_mpdu_07 */
#define M154_RX_MPDU_07_OFFSET                                  (0x41C)
#define M154_RX_MPDU_028                                        M154_RX_MPDU_028
#define M154_RX_MPDU_028_POS                                    (0U)
#define M154_RX_MPDU_028_LEN                                    (8U)
#define M154_RX_MPDU_028_MSK                                    (((1ULL<<M154_RX_MPDU_028_LEN)-1)<<M154_RX_MPDU_028_POS)
#define M154_RX_MPDU_028_UMSK                                   (~(((1ULL<<M154_RX_MPDU_028_LEN)-1)<<M154_RX_MPDU_028_POS))
#define M154_RX_MPDU_029                                        M154_RX_MPDU_029
#define M154_RX_MPDU_029_POS                                    (8U)
#define M154_RX_MPDU_029_LEN                                    (8U)
#define M154_RX_MPDU_029_MSK                                    (((1ULL<<M154_RX_MPDU_029_LEN)-1)<<M154_RX_MPDU_029_POS)
#define M154_RX_MPDU_029_UMSK                                   (~(((1ULL<<M154_RX_MPDU_029_LEN)-1)<<M154_RX_MPDU_029_POS))
#define M154_RX_MPDU_030                                        M154_RX_MPDU_030
#define M154_RX_MPDU_030_POS                                    (16U)
#define M154_RX_MPDU_030_LEN                                    (8U)
#define M154_RX_MPDU_030_MSK                                    (((1ULL<<M154_RX_MPDU_030_LEN)-1)<<M154_RX_MPDU_030_POS)
#define M154_RX_MPDU_030_UMSK                                   (~(((1ULL<<M154_RX_MPDU_030_LEN)-1)<<M154_RX_MPDU_030_POS))
#define M154_RX_MPDU_031                                        M154_RX_MPDU_031
#define M154_RX_MPDU_031_POS                                    (24U)
#define M154_RX_MPDU_031_LEN                                    (8U)
#define M154_RX_MPDU_031_MSK                                    (((1ULL<<M154_RX_MPDU_031_LEN)-1)<<M154_RX_MPDU_031_POS)
#define M154_RX_MPDU_031_UMSK                                   (~(((1ULL<<M154_RX_MPDU_031_LEN)-1)<<M154_RX_MPDU_031_POS))

/* 0x420 : m154_rx_mpdu_08 */
#define M154_RX_MPDU_08_OFFSET                                  (0x420)
#define M154_RX_MPDU_032                                        M154_RX_MPDU_032
#define M154_RX_MPDU_032_POS                                    (0U)
#define M154_RX_MPDU_032_LEN                                    (8U)
#define M154_RX_MPDU_032_MSK                                    (((1ULL<<M154_RX_MPDU_032_LEN)-1)<<M154_RX_MPDU_032_POS)
#define M154_RX_MPDU_032_UMSK                                   (~(((1ULL<<M154_RX_MPDU_032_LEN)-1)<<M154_RX_MPDU_032_POS))
#define M154_RX_MPDU_033                                        M154_RX_MPDU_033
#define M154_RX_MPDU_033_POS                                    (8U)
#define M154_RX_MPDU_033_LEN                                    (8U)
#define M154_RX_MPDU_033_MSK                                    (((1ULL<<M154_RX_MPDU_033_LEN)-1)<<M154_RX_MPDU_033_POS)
#define M154_RX_MPDU_033_UMSK                                   (~(((1ULL<<M154_RX_MPDU_033_LEN)-1)<<M154_RX_MPDU_033_POS))
#define M154_RX_MPDU_034                                        M154_RX_MPDU_034
#define M154_RX_MPDU_034_POS                                    (16U)
#define M154_RX_MPDU_034_LEN                                    (8U)
#define M154_RX_MPDU_034_MSK                                    (((1ULL<<M154_RX_MPDU_034_LEN)-1)<<M154_RX_MPDU_034_POS)
#define M154_RX_MPDU_034_UMSK                                   (~(((1ULL<<M154_RX_MPDU_034_LEN)-1)<<M154_RX_MPDU_034_POS))
#define M154_RX_MPDU_035                                        M154_RX_MPDU_035
#define M154_RX_MPDU_035_POS                                    (24U)
#define M154_RX_MPDU_035_LEN                                    (8U)
#define M154_RX_MPDU_035_MSK                                    (((1ULL<<M154_RX_MPDU_035_LEN)-1)<<M154_RX_MPDU_035_POS)
#define M154_RX_MPDU_035_UMSK                                   (~(((1ULL<<M154_RX_MPDU_035_LEN)-1)<<M154_RX_MPDU_035_POS))

/* 0x424 : m154_rx_mpdu_09 */
#define M154_RX_MPDU_09_OFFSET                                  (0x424)
#define M154_RX_MPDU_036                                        M154_RX_MPDU_036
#define M154_RX_MPDU_036_POS                                    (0U)
#define M154_RX_MPDU_036_LEN                                    (8U)
#define M154_RX_MPDU_036_MSK                                    (((1ULL<<M154_RX_MPDU_036_LEN)-1)<<M154_RX_MPDU_036_POS)
#define M154_RX_MPDU_036_UMSK                                   (~(((1ULL<<M154_RX_MPDU_036_LEN)-1)<<M154_RX_MPDU_036_POS))
#define M154_RX_MPDU_037                                        M154_RX_MPDU_037
#define M154_RX_MPDU_037_POS                                    (8U)
#define M154_RX_MPDU_037_LEN                                    (8U)
#define M154_RX_MPDU_037_MSK                                    (((1ULL<<M154_RX_MPDU_037_LEN)-1)<<M154_RX_MPDU_037_POS)
#define M154_RX_MPDU_037_UMSK                                   (~(((1ULL<<M154_RX_MPDU_037_LEN)-1)<<M154_RX_MPDU_037_POS))
#define M154_RX_MPDU_038                                        M154_RX_MPDU_038
#define M154_RX_MPDU_038_POS                                    (16U)
#define M154_RX_MPDU_038_LEN                                    (8U)
#define M154_RX_MPDU_038_MSK                                    (((1ULL<<M154_RX_MPDU_038_LEN)-1)<<M154_RX_MPDU_038_POS)
#define M154_RX_MPDU_038_UMSK                                   (~(((1ULL<<M154_RX_MPDU_038_LEN)-1)<<M154_RX_MPDU_038_POS))
#define M154_RX_MPDU_039                                        M154_RX_MPDU_039
#define M154_RX_MPDU_039_POS                                    (24U)
#define M154_RX_MPDU_039_LEN                                    (8U)
#define M154_RX_MPDU_039_MSK                                    (((1ULL<<M154_RX_MPDU_039_LEN)-1)<<M154_RX_MPDU_039_POS)
#define M154_RX_MPDU_039_UMSK                                   (~(((1ULL<<M154_RX_MPDU_039_LEN)-1)<<M154_RX_MPDU_039_POS))

/* 0x428 : m154_rx_mpdu_10 */
#define M154_RX_MPDU_10_OFFSET                                  (0x428)
#define M154_RX_MPDU_040                                        M154_RX_MPDU_040
#define M154_RX_MPDU_040_POS                                    (0U)
#define M154_RX_MPDU_040_LEN                                    (8U)
#define M154_RX_MPDU_040_MSK                                    (((1ULL<<M154_RX_MPDU_040_LEN)-1)<<M154_RX_MPDU_040_POS)
#define M154_RX_MPDU_040_UMSK                                   (~(((1ULL<<M154_RX_MPDU_040_LEN)-1)<<M154_RX_MPDU_040_POS))
#define M154_RX_MPDU_041                                        M154_RX_MPDU_041
#define M154_RX_MPDU_041_POS                                    (8U)
#define M154_RX_MPDU_041_LEN                                    (8U)
#define M154_RX_MPDU_041_MSK                                    (((1ULL<<M154_RX_MPDU_041_LEN)-1)<<M154_RX_MPDU_041_POS)
#define M154_RX_MPDU_041_UMSK                                   (~(((1ULL<<M154_RX_MPDU_041_LEN)-1)<<M154_RX_MPDU_041_POS))
#define M154_RX_MPDU_042                                        M154_RX_MPDU_042
#define M154_RX_MPDU_042_POS                                    (16U)
#define M154_RX_MPDU_042_LEN                                    (8U)
#define M154_RX_MPDU_042_MSK                                    (((1ULL<<M154_RX_MPDU_042_LEN)-1)<<M154_RX_MPDU_042_POS)
#define M154_RX_MPDU_042_UMSK                                   (~(((1ULL<<M154_RX_MPDU_042_LEN)-1)<<M154_RX_MPDU_042_POS))
#define M154_RX_MPDU_043                                        M154_RX_MPDU_043
#define M154_RX_MPDU_043_POS                                    (24U)
#define M154_RX_MPDU_043_LEN                                    (8U)
#define M154_RX_MPDU_043_MSK                                    (((1ULL<<M154_RX_MPDU_043_LEN)-1)<<M154_RX_MPDU_043_POS)
#define M154_RX_MPDU_043_UMSK                                   (~(((1ULL<<M154_RX_MPDU_043_LEN)-1)<<M154_RX_MPDU_043_POS))

/* 0x42C : m154_rx_mpdu_11 */
#define M154_RX_MPDU_11_OFFSET                                  (0x42C)
#define M154_RX_MPDU_044                                        M154_RX_MPDU_044
#define M154_RX_MPDU_044_POS                                    (0U)
#define M154_RX_MPDU_044_LEN                                    (8U)
#define M154_RX_MPDU_044_MSK                                    (((1ULL<<M154_RX_MPDU_044_LEN)-1)<<M154_RX_MPDU_044_POS)
#define M154_RX_MPDU_044_UMSK                                   (~(((1ULL<<M154_RX_MPDU_044_LEN)-1)<<M154_RX_MPDU_044_POS))
#define M154_RX_MPDU_045                                        M154_RX_MPDU_045
#define M154_RX_MPDU_045_POS                                    (8U)
#define M154_RX_MPDU_045_LEN                                    (8U)
#define M154_RX_MPDU_045_MSK                                    (((1ULL<<M154_RX_MPDU_045_LEN)-1)<<M154_RX_MPDU_045_POS)
#define M154_RX_MPDU_045_UMSK                                   (~(((1ULL<<M154_RX_MPDU_045_LEN)-1)<<M154_RX_MPDU_045_POS))
#define M154_RX_MPDU_046                                        M154_RX_MPDU_046
#define M154_RX_MPDU_046_POS                                    (16U)
#define M154_RX_MPDU_046_LEN                                    (8U)
#define M154_RX_MPDU_046_MSK                                    (((1ULL<<M154_RX_MPDU_046_LEN)-1)<<M154_RX_MPDU_046_POS)
#define M154_RX_MPDU_046_UMSK                                   (~(((1ULL<<M154_RX_MPDU_046_LEN)-1)<<M154_RX_MPDU_046_POS))
#define M154_RX_MPDU_047                                        M154_RX_MPDU_047
#define M154_RX_MPDU_047_POS                                    (24U)
#define M154_RX_MPDU_047_LEN                                    (8U)
#define M154_RX_MPDU_047_MSK                                    (((1ULL<<M154_RX_MPDU_047_LEN)-1)<<M154_RX_MPDU_047_POS)
#define M154_RX_MPDU_047_UMSK                                   (~(((1ULL<<M154_RX_MPDU_047_LEN)-1)<<M154_RX_MPDU_047_POS))

/* 0x430 : m154_rx_mpdu_12 */
#define M154_RX_MPDU_12_OFFSET                                  (0x430)
#define M154_RX_MPDU_048                                        M154_RX_MPDU_048
#define M154_RX_MPDU_048_POS                                    (0U)
#define M154_RX_MPDU_048_LEN                                    (8U)
#define M154_RX_MPDU_048_MSK                                    (((1ULL<<M154_RX_MPDU_048_LEN)-1)<<M154_RX_MPDU_048_POS)
#define M154_RX_MPDU_048_UMSK                                   (~(((1ULL<<M154_RX_MPDU_048_LEN)-1)<<M154_RX_MPDU_048_POS))
#define M154_RX_MPDU_049                                        M154_RX_MPDU_049
#define M154_RX_MPDU_049_POS                                    (8U)
#define M154_RX_MPDU_049_LEN                                    (8U)
#define M154_RX_MPDU_049_MSK                                    (((1ULL<<M154_RX_MPDU_049_LEN)-1)<<M154_RX_MPDU_049_POS)
#define M154_RX_MPDU_049_UMSK                                   (~(((1ULL<<M154_RX_MPDU_049_LEN)-1)<<M154_RX_MPDU_049_POS))
#define M154_RX_MPDU_050                                        M154_RX_MPDU_050
#define M154_RX_MPDU_050_POS                                    (16U)
#define M154_RX_MPDU_050_LEN                                    (8U)
#define M154_RX_MPDU_050_MSK                                    (((1ULL<<M154_RX_MPDU_050_LEN)-1)<<M154_RX_MPDU_050_POS)
#define M154_RX_MPDU_050_UMSK                                   (~(((1ULL<<M154_RX_MPDU_050_LEN)-1)<<M154_RX_MPDU_050_POS))
#define M154_RX_MPDU_051                                        M154_RX_MPDU_051
#define M154_RX_MPDU_051_POS                                    (24U)
#define M154_RX_MPDU_051_LEN                                    (8U)
#define M154_RX_MPDU_051_MSK                                    (((1ULL<<M154_RX_MPDU_051_LEN)-1)<<M154_RX_MPDU_051_POS)
#define M154_RX_MPDU_051_UMSK                                   (~(((1ULL<<M154_RX_MPDU_051_LEN)-1)<<M154_RX_MPDU_051_POS))

/* 0x434 : m154_rx_mpdu_13 */
#define M154_RX_MPDU_13_OFFSET                                  (0x434)
#define M154_RX_MPDU_052                                        M154_RX_MPDU_052
#define M154_RX_MPDU_052_POS                                    (0U)
#define M154_RX_MPDU_052_LEN                                    (8U)
#define M154_RX_MPDU_052_MSK                                    (((1ULL<<M154_RX_MPDU_052_LEN)-1)<<M154_RX_MPDU_052_POS)
#define M154_RX_MPDU_052_UMSK                                   (~(((1ULL<<M154_RX_MPDU_052_LEN)-1)<<M154_RX_MPDU_052_POS))
#define M154_RX_MPDU_053                                        M154_RX_MPDU_053
#define M154_RX_MPDU_053_POS                                    (8U)
#define M154_RX_MPDU_053_LEN                                    (8U)
#define M154_RX_MPDU_053_MSK                                    (((1ULL<<M154_RX_MPDU_053_LEN)-1)<<M154_RX_MPDU_053_POS)
#define M154_RX_MPDU_053_UMSK                                   (~(((1ULL<<M154_RX_MPDU_053_LEN)-1)<<M154_RX_MPDU_053_POS))
#define M154_RX_MPDU_054                                        M154_RX_MPDU_054
#define M154_RX_MPDU_054_POS                                    (16U)
#define M154_RX_MPDU_054_LEN                                    (8U)
#define M154_RX_MPDU_054_MSK                                    (((1ULL<<M154_RX_MPDU_054_LEN)-1)<<M154_RX_MPDU_054_POS)
#define M154_RX_MPDU_054_UMSK                                   (~(((1ULL<<M154_RX_MPDU_054_LEN)-1)<<M154_RX_MPDU_054_POS))
#define M154_RX_MPDU_055                                        M154_RX_MPDU_055
#define M154_RX_MPDU_055_POS                                    (24U)
#define M154_RX_MPDU_055_LEN                                    (8U)
#define M154_RX_MPDU_055_MSK                                    (((1ULL<<M154_RX_MPDU_055_LEN)-1)<<M154_RX_MPDU_055_POS)
#define M154_RX_MPDU_055_UMSK                                   (~(((1ULL<<M154_RX_MPDU_055_LEN)-1)<<M154_RX_MPDU_055_POS))

/* 0x438 : m154_rx_mpdu_14 */
#define M154_RX_MPDU_14_OFFSET                                  (0x438)
#define M154_RX_MPDU_056                                        M154_RX_MPDU_056
#define M154_RX_MPDU_056_POS                                    (0U)
#define M154_RX_MPDU_056_LEN                                    (8U)
#define M154_RX_MPDU_056_MSK                                    (((1ULL<<M154_RX_MPDU_056_LEN)-1)<<M154_RX_MPDU_056_POS)
#define M154_RX_MPDU_056_UMSK                                   (~(((1ULL<<M154_RX_MPDU_056_LEN)-1)<<M154_RX_MPDU_056_POS))
#define M154_RX_MPDU_057                                        M154_RX_MPDU_057
#define M154_RX_MPDU_057_POS                                    (8U)
#define M154_RX_MPDU_057_LEN                                    (8U)
#define M154_RX_MPDU_057_MSK                                    (((1ULL<<M154_RX_MPDU_057_LEN)-1)<<M154_RX_MPDU_057_POS)
#define M154_RX_MPDU_057_UMSK                                   (~(((1ULL<<M154_RX_MPDU_057_LEN)-1)<<M154_RX_MPDU_057_POS))
#define M154_RX_MPDU_058                                        M154_RX_MPDU_058
#define M154_RX_MPDU_058_POS                                    (16U)
#define M154_RX_MPDU_058_LEN                                    (8U)
#define M154_RX_MPDU_058_MSK                                    (((1ULL<<M154_RX_MPDU_058_LEN)-1)<<M154_RX_MPDU_058_POS)
#define M154_RX_MPDU_058_UMSK                                   (~(((1ULL<<M154_RX_MPDU_058_LEN)-1)<<M154_RX_MPDU_058_POS))
#define M154_RX_MPDU_059                                        M154_RX_MPDU_059
#define M154_RX_MPDU_059_POS                                    (24U)
#define M154_RX_MPDU_059_LEN                                    (8U)
#define M154_RX_MPDU_059_MSK                                    (((1ULL<<M154_RX_MPDU_059_LEN)-1)<<M154_RX_MPDU_059_POS)
#define M154_RX_MPDU_059_UMSK                                   (~(((1ULL<<M154_RX_MPDU_059_LEN)-1)<<M154_RX_MPDU_059_POS))

/* 0x43C : m154_rx_mpdu_15 */
#define M154_RX_MPDU_15_OFFSET                                  (0x43C)
#define M154_RX_MPDU_060                                        M154_RX_MPDU_060
#define M154_RX_MPDU_060_POS                                    (0U)
#define M154_RX_MPDU_060_LEN                                    (8U)
#define M154_RX_MPDU_060_MSK                                    (((1ULL<<M154_RX_MPDU_060_LEN)-1)<<M154_RX_MPDU_060_POS)
#define M154_RX_MPDU_060_UMSK                                   (~(((1ULL<<M154_RX_MPDU_060_LEN)-1)<<M154_RX_MPDU_060_POS))
#define M154_RX_MPDU_061                                        M154_RX_MPDU_061
#define M154_RX_MPDU_061_POS                                    (8U)
#define M154_RX_MPDU_061_LEN                                    (8U)
#define M154_RX_MPDU_061_MSK                                    (((1ULL<<M154_RX_MPDU_061_LEN)-1)<<M154_RX_MPDU_061_POS)
#define M154_RX_MPDU_061_UMSK                                   (~(((1ULL<<M154_RX_MPDU_061_LEN)-1)<<M154_RX_MPDU_061_POS))
#define M154_RX_MPDU_062                                        M154_RX_MPDU_062
#define M154_RX_MPDU_062_POS                                    (16U)
#define M154_RX_MPDU_062_LEN                                    (8U)
#define M154_RX_MPDU_062_MSK                                    (((1ULL<<M154_RX_MPDU_062_LEN)-1)<<M154_RX_MPDU_062_POS)
#define M154_RX_MPDU_062_UMSK                                   (~(((1ULL<<M154_RX_MPDU_062_LEN)-1)<<M154_RX_MPDU_062_POS))
#define M154_RX_MPDU_063                                        M154_RX_MPDU_063
#define M154_RX_MPDU_063_POS                                    (24U)
#define M154_RX_MPDU_063_LEN                                    (8U)
#define M154_RX_MPDU_063_MSK                                    (((1ULL<<M154_RX_MPDU_063_LEN)-1)<<M154_RX_MPDU_063_POS)
#define M154_RX_MPDU_063_UMSK                                   (~(((1ULL<<M154_RX_MPDU_063_LEN)-1)<<M154_RX_MPDU_063_POS))

/* 0x440 : m154_rx_mpdu_16 */
#define M154_RX_MPDU_16_OFFSET                                  (0x440)
#define M154_RX_MPDU_064                                        M154_RX_MPDU_064
#define M154_RX_MPDU_064_POS                                    (0U)
#define M154_RX_MPDU_064_LEN                                    (8U)
#define M154_RX_MPDU_064_MSK                                    (((1ULL<<M154_RX_MPDU_064_LEN)-1)<<M154_RX_MPDU_064_POS)
#define M154_RX_MPDU_064_UMSK                                   (~(((1ULL<<M154_RX_MPDU_064_LEN)-1)<<M154_RX_MPDU_064_POS))
#define M154_RX_MPDU_065                                        M154_RX_MPDU_065
#define M154_RX_MPDU_065_POS                                    (8U)
#define M154_RX_MPDU_065_LEN                                    (8U)
#define M154_RX_MPDU_065_MSK                                    (((1ULL<<M154_RX_MPDU_065_LEN)-1)<<M154_RX_MPDU_065_POS)
#define M154_RX_MPDU_065_UMSK                                   (~(((1ULL<<M154_RX_MPDU_065_LEN)-1)<<M154_RX_MPDU_065_POS))
#define M154_RX_MPDU_066                                        M154_RX_MPDU_066
#define M154_RX_MPDU_066_POS                                    (16U)
#define M154_RX_MPDU_066_LEN                                    (8U)
#define M154_RX_MPDU_066_MSK                                    (((1ULL<<M154_RX_MPDU_066_LEN)-1)<<M154_RX_MPDU_066_POS)
#define M154_RX_MPDU_066_UMSK                                   (~(((1ULL<<M154_RX_MPDU_066_LEN)-1)<<M154_RX_MPDU_066_POS))
#define M154_RX_MPDU_067                                        M154_RX_MPDU_067
#define M154_RX_MPDU_067_POS                                    (24U)
#define M154_RX_MPDU_067_LEN                                    (8U)
#define M154_RX_MPDU_067_MSK                                    (((1ULL<<M154_RX_MPDU_067_LEN)-1)<<M154_RX_MPDU_067_POS)
#define M154_RX_MPDU_067_UMSK                                   (~(((1ULL<<M154_RX_MPDU_067_LEN)-1)<<M154_RX_MPDU_067_POS))

/* 0x444 : m154_rx_mpdu_17 */
#define M154_RX_MPDU_17_OFFSET                                  (0x444)
#define M154_RX_MPDU_068                                        M154_RX_MPDU_068
#define M154_RX_MPDU_068_POS                                    (0U)
#define M154_RX_MPDU_068_LEN                                    (8U)
#define M154_RX_MPDU_068_MSK                                    (((1ULL<<M154_RX_MPDU_068_LEN)-1)<<M154_RX_MPDU_068_POS)
#define M154_RX_MPDU_068_UMSK                                   (~(((1ULL<<M154_RX_MPDU_068_LEN)-1)<<M154_RX_MPDU_068_POS))
#define M154_RX_MPDU_069                                        M154_RX_MPDU_069
#define M154_RX_MPDU_069_POS                                    (8U)
#define M154_RX_MPDU_069_LEN                                    (8U)
#define M154_RX_MPDU_069_MSK                                    (((1ULL<<M154_RX_MPDU_069_LEN)-1)<<M154_RX_MPDU_069_POS)
#define M154_RX_MPDU_069_UMSK                                   (~(((1ULL<<M154_RX_MPDU_069_LEN)-1)<<M154_RX_MPDU_069_POS))
#define M154_RX_MPDU_070                                        M154_RX_MPDU_070
#define M154_RX_MPDU_070_POS                                    (16U)
#define M154_RX_MPDU_070_LEN                                    (8U)
#define M154_RX_MPDU_070_MSK                                    (((1ULL<<M154_RX_MPDU_070_LEN)-1)<<M154_RX_MPDU_070_POS)
#define M154_RX_MPDU_070_UMSK                                   (~(((1ULL<<M154_RX_MPDU_070_LEN)-1)<<M154_RX_MPDU_070_POS))
#define M154_RX_MPDU_071                                        M154_RX_MPDU_071
#define M154_RX_MPDU_071_POS                                    (24U)
#define M154_RX_MPDU_071_LEN                                    (8U)
#define M154_RX_MPDU_071_MSK                                    (((1ULL<<M154_RX_MPDU_071_LEN)-1)<<M154_RX_MPDU_071_POS)
#define M154_RX_MPDU_071_UMSK                                   (~(((1ULL<<M154_RX_MPDU_071_LEN)-1)<<M154_RX_MPDU_071_POS))

/* 0x448 : m154_rx_mpdu_18 */
#define M154_RX_MPDU_18_OFFSET                                  (0x448)
#define M154_RX_MPDU_072                                        M154_RX_MPDU_072
#define M154_RX_MPDU_072_POS                                    (0U)
#define M154_RX_MPDU_072_LEN                                    (8U)
#define M154_RX_MPDU_072_MSK                                    (((1ULL<<M154_RX_MPDU_072_LEN)-1)<<M154_RX_MPDU_072_POS)
#define M154_RX_MPDU_072_UMSK                                   (~(((1ULL<<M154_RX_MPDU_072_LEN)-1)<<M154_RX_MPDU_072_POS))
#define M154_RX_MPDU_073                                        M154_RX_MPDU_073
#define M154_RX_MPDU_073_POS                                    (8U)
#define M154_RX_MPDU_073_LEN                                    (8U)
#define M154_RX_MPDU_073_MSK                                    (((1ULL<<M154_RX_MPDU_073_LEN)-1)<<M154_RX_MPDU_073_POS)
#define M154_RX_MPDU_073_UMSK                                   (~(((1ULL<<M154_RX_MPDU_073_LEN)-1)<<M154_RX_MPDU_073_POS))
#define M154_RX_MPDU_074                                        M154_RX_MPDU_074
#define M154_RX_MPDU_074_POS                                    (16U)
#define M154_RX_MPDU_074_LEN                                    (8U)
#define M154_RX_MPDU_074_MSK                                    (((1ULL<<M154_RX_MPDU_074_LEN)-1)<<M154_RX_MPDU_074_POS)
#define M154_RX_MPDU_074_UMSK                                   (~(((1ULL<<M154_RX_MPDU_074_LEN)-1)<<M154_RX_MPDU_074_POS))
#define M154_RX_MPDU_075                                        M154_RX_MPDU_075
#define M154_RX_MPDU_075_POS                                    (24U)
#define M154_RX_MPDU_075_LEN                                    (8U)
#define M154_RX_MPDU_075_MSK                                    (((1ULL<<M154_RX_MPDU_075_LEN)-1)<<M154_RX_MPDU_075_POS)
#define M154_RX_MPDU_075_UMSK                                   (~(((1ULL<<M154_RX_MPDU_075_LEN)-1)<<M154_RX_MPDU_075_POS))

/* 0x44C : m154_rx_mpdu_19 */
#define M154_RX_MPDU_19_OFFSET                                  (0x44C)
#define M154_RX_MPDU_076                                        M154_RX_MPDU_076
#define M154_RX_MPDU_076_POS                                    (0U)
#define M154_RX_MPDU_076_LEN                                    (8U)
#define M154_RX_MPDU_076_MSK                                    (((1ULL<<M154_RX_MPDU_076_LEN)-1)<<M154_RX_MPDU_076_POS)
#define M154_RX_MPDU_076_UMSK                                   (~(((1ULL<<M154_RX_MPDU_076_LEN)-1)<<M154_RX_MPDU_076_POS))
#define M154_RX_MPDU_077                                        M154_RX_MPDU_077
#define M154_RX_MPDU_077_POS                                    (8U)
#define M154_RX_MPDU_077_LEN                                    (8U)
#define M154_RX_MPDU_077_MSK                                    (((1ULL<<M154_RX_MPDU_077_LEN)-1)<<M154_RX_MPDU_077_POS)
#define M154_RX_MPDU_077_UMSK                                   (~(((1ULL<<M154_RX_MPDU_077_LEN)-1)<<M154_RX_MPDU_077_POS))
#define M154_RX_MPDU_078                                        M154_RX_MPDU_078
#define M154_RX_MPDU_078_POS                                    (16U)
#define M154_RX_MPDU_078_LEN                                    (8U)
#define M154_RX_MPDU_078_MSK                                    (((1ULL<<M154_RX_MPDU_078_LEN)-1)<<M154_RX_MPDU_078_POS)
#define M154_RX_MPDU_078_UMSK                                   (~(((1ULL<<M154_RX_MPDU_078_LEN)-1)<<M154_RX_MPDU_078_POS))
#define M154_RX_MPDU_079                                        M154_RX_MPDU_079
#define M154_RX_MPDU_079_POS                                    (24U)
#define M154_RX_MPDU_079_LEN                                    (8U)
#define M154_RX_MPDU_079_MSK                                    (((1ULL<<M154_RX_MPDU_079_LEN)-1)<<M154_RX_MPDU_079_POS)
#define M154_RX_MPDU_079_UMSK                                   (~(((1ULL<<M154_RX_MPDU_079_LEN)-1)<<M154_RX_MPDU_079_POS))

/* 0x450 : m154_rx_mpdu_20 */
#define M154_RX_MPDU_20_OFFSET                                  (0x450)
#define M154_RX_MPDU_080                                        M154_RX_MPDU_080
#define M154_RX_MPDU_080_POS                                    (0U)
#define M154_RX_MPDU_080_LEN                                    (8U)
#define M154_RX_MPDU_080_MSK                                    (((1ULL<<M154_RX_MPDU_080_LEN)-1)<<M154_RX_MPDU_080_POS)
#define M154_RX_MPDU_080_UMSK                                   (~(((1ULL<<M154_RX_MPDU_080_LEN)-1)<<M154_RX_MPDU_080_POS))
#define M154_RX_MPDU_081                                        M154_RX_MPDU_081
#define M154_RX_MPDU_081_POS                                    (8U)
#define M154_RX_MPDU_081_LEN                                    (8U)
#define M154_RX_MPDU_081_MSK                                    (((1ULL<<M154_RX_MPDU_081_LEN)-1)<<M154_RX_MPDU_081_POS)
#define M154_RX_MPDU_081_UMSK                                   (~(((1ULL<<M154_RX_MPDU_081_LEN)-1)<<M154_RX_MPDU_081_POS))
#define M154_RX_MPDU_082                                        M154_RX_MPDU_082
#define M154_RX_MPDU_082_POS                                    (16U)
#define M154_RX_MPDU_082_LEN                                    (8U)
#define M154_RX_MPDU_082_MSK                                    (((1ULL<<M154_RX_MPDU_082_LEN)-1)<<M154_RX_MPDU_082_POS)
#define M154_RX_MPDU_082_UMSK                                   (~(((1ULL<<M154_RX_MPDU_082_LEN)-1)<<M154_RX_MPDU_082_POS))
#define M154_RX_MPDU_083                                        M154_RX_MPDU_083
#define M154_RX_MPDU_083_POS                                    (24U)
#define M154_RX_MPDU_083_LEN                                    (8U)
#define M154_RX_MPDU_083_MSK                                    (((1ULL<<M154_RX_MPDU_083_LEN)-1)<<M154_RX_MPDU_083_POS)
#define M154_RX_MPDU_083_UMSK                                   (~(((1ULL<<M154_RX_MPDU_083_LEN)-1)<<M154_RX_MPDU_083_POS))

/* 0x454 : m154_rx_mpdu_21 */
#define M154_RX_MPDU_21_OFFSET                                  (0x454)
#define M154_RX_MPDU_084                                        M154_RX_MPDU_084
#define M154_RX_MPDU_084_POS                                    (0U)
#define M154_RX_MPDU_084_LEN                                    (8U)
#define M154_RX_MPDU_084_MSK                                    (((1ULL<<M154_RX_MPDU_084_LEN)-1)<<M154_RX_MPDU_084_POS)
#define M154_RX_MPDU_084_UMSK                                   (~(((1ULL<<M154_RX_MPDU_084_LEN)-1)<<M154_RX_MPDU_084_POS))
#define M154_RX_MPDU_085                                        M154_RX_MPDU_085
#define M154_RX_MPDU_085_POS                                    (8U)
#define M154_RX_MPDU_085_LEN                                    (8U)
#define M154_RX_MPDU_085_MSK                                    (((1ULL<<M154_RX_MPDU_085_LEN)-1)<<M154_RX_MPDU_085_POS)
#define M154_RX_MPDU_085_UMSK                                   (~(((1ULL<<M154_RX_MPDU_085_LEN)-1)<<M154_RX_MPDU_085_POS))
#define M154_RX_MPDU_086                                        M154_RX_MPDU_086
#define M154_RX_MPDU_086_POS                                    (16U)
#define M154_RX_MPDU_086_LEN                                    (8U)
#define M154_RX_MPDU_086_MSK                                    (((1ULL<<M154_RX_MPDU_086_LEN)-1)<<M154_RX_MPDU_086_POS)
#define M154_RX_MPDU_086_UMSK                                   (~(((1ULL<<M154_RX_MPDU_086_LEN)-1)<<M154_RX_MPDU_086_POS))
#define M154_RX_MPDU_087                                        M154_RX_MPDU_087
#define M154_RX_MPDU_087_POS                                    (24U)
#define M154_RX_MPDU_087_LEN                                    (8U)
#define M154_RX_MPDU_087_MSK                                    (((1ULL<<M154_RX_MPDU_087_LEN)-1)<<M154_RX_MPDU_087_POS)
#define M154_RX_MPDU_087_UMSK                                   (~(((1ULL<<M154_RX_MPDU_087_LEN)-1)<<M154_RX_MPDU_087_POS))

/* 0x458 : m154_rx_mpdu_22 */
#define M154_RX_MPDU_22_OFFSET                                  (0x458)
#define M154_RX_MPDU_088                                        M154_RX_MPDU_088
#define M154_RX_MPDU_088_POS                                    (0U)
#define M154_RX_MPDU_088_LEN                                    (8U)
#define M154_RX_MPDU_088_MSK                                    (((1ULL<<M154_RX_MPDU_088_LEN)-1)<<M154_RX_MPDU_088_POS)
#define M154_RX_MPDU_088_UMSK                                   (~(((1ULL<<M154_RX_MPDU_088_LEN)-1)<<M154_RX_MPDU_088_POS))
#define M154_RX_MPDU_089                                        M154_RX_MPDU_089
#define M154_RX_MPDU_089_POS                                    (8U)
#define M154_RX_MPDU_089_LEN                                    (8U)
#define M154_RX_MPDU_089_MSK                                    (((1ULL<<M154_RX_MPDU_089_LEN)-1)<<M154_RX_MPDU_089_POS)
#define M154_RX_MPDU_089_UMSK                                   (~(((1ULL<<M154_RX_MPDU_089_LEN)-1)<<M154_RX_MPDU_089_POS))
#define M154_RX_MPDU_090                                        M154_RX_MPDU_090
#define M154_RX_MPDU_090_POS                                    (16U)
#define M154_RX_MPDU_090_LEN                                    (8U)
#define M154_RX_MPDU_090_MSK                                    (((1ULL<<M154_RX_MPDU_090_LEN)-1)<<M154_RX_MPDU_090_POS)
#define M154_RX_MPDU_090_UMSK                                   (~(((1ULL<<M154_RX_MPDU_090_LEN)-1)<<M154_RX_MPDU_090_POS))
#define M154_RX_MPDU_091                                        M154_RX_MPDU_091
#define M154_RX_MPDU_091_POS                                    (24U)
#define M154_RX_MPDU_091_LEN                                    (8U)
#define M154_RX_MPDU_091_MSK                                    (((1ULL<<M154_RX_MPDU_091_LEN)-1)<<M154_RX_MPDU_091_POS)
#define M154_RX_MPDU_091_UMSK                                   (~(((1ULL<<M154_RX_MPDU_091_LEN)-1)<<M154_RX_MPDU_091_POS))

/* 0x45C : m154_rx_mpdu_23 */
#define M154_RX_MPDU_23_OFFSET                                  (0x45C)
#define M154_RX_MPDU_092                                        M154_RX_MPDU_092
#define M154_RX_MPDU_092_POS                                    (0U)
#define M154_RX_MPDU_092_LEN                                    (8U)
#define M154_RX_MPDU_092_MSK                                    (((1ULL<<M154_RX_MPDU_092_LEN)-1)<<M154_RX_MPDU_092_POS)
#define M154_RX_MPDU_092_UMSK                                   (~(((1ULL<<M154_RX_MPDU_092_LEN)-1)<<M154_RX_MPDU_092_POS))
#define M154_RX_MPDU_093                                        M154_RX_MPDU_093
#define M154_RX_MPDU_093_POS                                    (8U)
#define M154_RX_MPDU_093_LEN                                    (8U)
#define M154_RX_MPDU_093_MSK                                    (((1ULL<<M154_RX_MPDU_093_LEN)-1)<<M154_RX_MPDU_093_POS)
#define M154_RX_MPDU_093_UMSK                                   (~(((1ULL<<M154_RX_MPDU_093_LEN)-1)<<M154_RX_MPDU_093_POS))
#define M154_RX_MPDU_094                                        M154_RX_MPDU_094
#define M154_RX_MPDU_094_POS                                    (16U)
#define M154_RX_MPDU_094_LEN                                    (8U)
#define M154_RX_MPDU_094_MSK                                    (((1ULL<<M154_RX_MPDU_094_LEN)-1)<<M154_RX_MPDU_094_POS)
#define M154_RX_MPDU_094_UMSK                                   (~(((1ULL<<M154_RX_MPDU_094_LEN)-1)<<M154_RX_MPDU_094_POS))
#define M154_RX_MPDU_095                                        M154_RX_MPDU_095
#define M154_RX_MPDU_095_POS                                    (24U)
#define M154_RX_MPDU_095_LEN                                    (8U)
#define M154_RX_MPDU_095_MSK                                    (((1ULL<<M154_RX_MPDU_095_LEN)-1)<<M154_RX_MPDU_095_POS)
#define M154_RX_MPDU_095_UMSK                                   (~(((1ULL<<M154_RX_MPDU_095_LEN)-1)<<M154_RX_MPDU_095_POS))

/* 0x460 : m154_rx_mpdu_24 */
#define M154_RX_MPDU_24_OFFSET                                  (0x460)
#define M154_RX_MPDU_096                                        M154_RX_MPDU_096
#define M154_RX_MPDU_096_POS                                    (0U)
#define M154_RX_MPDU_096_LEN                                    (8U)
#define M154_RX_MPDU_096_MSK                                    (((1ULL<<M154_RX_MPDU_096_LEN)-1)<<M154_RX_MPDU_096_POS)
#define M154_RX_MPDU_096_UMSK                                   (~(((1ULL<<M154_RX_MPDU_096_LEN)-1)<<M154_RX_MPDU_096_POS))
#define M154_RX_MPDU_097                                        M154_RX_MPDU_097
#define M154_RX_MPDU_097_POS                                    (8U)
#define M154_RX_MPDU_097_LEN                                    (8U)
#define M154_RX_MPDU_097_MSK                                    (((1ULL<<M154_RX_MPDU_097_LEN)-1)<<M154_RX_MPDU_097_POS)
#define M154_RX_MPDU_097_UMSK                                   (~(((1ULL<<M154_RX_MPDU_097_LEN)-1)<<M154_RX_MPDU_097_POS))
#define M154_RX_MPDU_098                                        M154_RX_MPDU_098
#define M154_RX_MPDU_098_POS                                    (16U)
#define M154_RX_MPDU_098_LEN                                    (8U)
#define M154_RX_MPDU_098_MSK                                    (((1ULL<<M154_RX_MPDU_098_LEN)-1)<<M154_RX_MPDU_098_POS)
#define M154_RX_MPDU_098_UMSK                                   (~(((1ULL<<M154_RX_MPDU_098_LEN)-1)<<M154_RX_MPDU_098_POS))
#define M154_RX_MPDU_099                                        M154_RX_MPDU_099
#define M154_RX_MPDU_099_POS                                    (24U)
#define M154_RX_MPDU_099_LEN                                    (8U)
#define M154_RX_MPDU_099_MSK                                    (((1ULL<<M154_RX_MPDU_099_LEN)-1)<<M154_RX_MPDU_099_POS)
#define M154_RX_MPDU_099_UMSK                                   (~(((1ULL<<M154_RX_MPDU_099_LEN)-1)<<M154_RX_MPDU_099_POS))

/* 0x464 : m154_rx_mpdu_25 */
#define M154_RX_MPDU_25_OFFSET                                  (0x464)
#define M154_RX_MPDU_100                                        M154_RX_MPDU_100
#define M154_RX_MPDU_100_POS                                    (0U)
#define M154_RX_MPDU_100_LEN                                    (8U)
#define M154_RX_MPDU_100_MSK                                    (((1ULL<<M154_RX_MPDU_100_LEN)-1)<<M154_RX_MPDU_100_POS)
#define M154_RX_MPDU_100_UMSK                                   (~(((1ULL<<M154_RX_MPDU_100_LEN)-1)<<M154_RX_MPDU_100_POS))
#define M154_RX_MPDU_101                                        M154_RX_MPDU_101
#define M154_RX_MPDU_101_POS                                    (8U)
#define M154_RX_MPDU_101_LEN                                    (8U)
#define M154_RX_MPDU_101_MSK                                    (((1ULL<<M154_RX_MPDU_101_LEN)-1)<<M154_RX_MPDU_101_POS)
#define M154_RX_MPDU_101_UMSK                                   (~(((1ULL<<M154_RX_MPDU_101_LEN)-1)<<M154_RX_MPDU_101_POS))
#define M154_RX_MPDU_102                                        M154_RX_MPDU_102
#define M154_RX_MPDU_102_POS                                    (16U)
#define M154_RX_MPDU_102_LEN                                    (8U)
#define M154_RX_MPDU_102_MSK                                    (((1ULL<<M154_RX_MPDU_102_LEN)-1)<<M154_RX_MPDU_102_POS)
#define M154_RX_MPDU_102_UMSK                                   (~(((1ULL<<M154_RX_MPDU_102_LEN)-1)<<M154_RX_MPDU_102_POS))
#define M154_RX_MPDU_103                                        M154_RX_MPDU_103
#define M154_RX_MPDU_103_POS                                    (24U)
#define M154_RX_MPDU_103_LEN                                    (8U)
#define M154_RX_MPDU_103_MSK                                    (((1ULL<<M154_RX_MPDU_103_LEN)-1)<<M154_RX_MPDU_103_POS)
#define M154_RX_MPDU_103_UMSK                                   (~(((1ULL<<M154_RX_MPDU_103_LEN)-1)<<M154_RX_MPDU_103_POS))

/* 0x468 : m154_rx_mpdu_26 */
#define M154_RX_MPDU_26_OFFSET                                  (0x468)
#define M154_RX_MPDU_104                                        M154_RX_MPDU_104
#define M154_RX_MPDU_104_POS                                    (0U)
#define M154_RX_MPDU_104_LEN                                    (8U)
#define M154_RX_MPDU_104_MSK                                    (((1ULL<<M154_RX_MPDU_104_LEN)-1)<<M154_RX_MPDU_104_POS)
#define M154_RX_MPDU_104_UMSK                                   (~(((1ULL<<M154_RX_MPDU_104_LEN)-1)<<M154_RX_MPDU_104_POS))
#define M154_RX_MPDU_105                                        M154_RX_MPDU_105
#define M154_RX_MPDU_105_POS                                    (8U)
#define M154_RX_MPDU_105_LEN                                    (8U)
#define M154_RX_MPDU_105_MSK                                    (((1ULL<<M154_RX_MPDU_105_LEN)-1)<<M154_RX_MPDU_105_POS)
#define M154_RX_MPDU_105_UMSK                                   (~(((1ULL<<M154_RX_MPDU_105_LEN)-1)<<M154_RX_MPDU_105_POS))
#define M154_RX_MPDU_106                                        M154_RX_MPDU_106
#define M154_RX_MPDU_106_POS                                    (16U)
#define M154_RX_MPDU_106_LEN                                    (8U)
#define M154_RX_MPDU_106_MSK                                    (((1ULL<<M154_RX_MPDU_106_LEN)-1)<<M154_RX_MPDU_106_POS)
#define M154_RX_MPDU_106_UMSK                                   (~(((1ULL<<M154_RX_MPDU_106_LEN)-1)<<M154_RX_MPDU_106_POS))
#define M154_RX_MPDU_107                                        M154_RX_MPDU_107
#define M154_RX_MPDU_107_POS                                    (24U)
#define M154_RX_MPDU_107_LEN                                    (8U)
#define M154_RX_MPDU_107_MSK                                    (((1ULL<<M154_RX_MPDU_107_LEN)-1)<<M154_RX_MPDU_107_POS)
#define M154_RX_MPDU_107_UMSK                                   (~(((1ULL<<M154_RX_MPDU_107_LEN)-1)<<M154_RX_MPDU_107_POS))

/* 0x46C : m154_rx_mpdu_27 */
#define M154_RX_MPDU_27_OFFSET                                  (0x46C)
#define M154_RX_MPDU_108                                        M154_RX_MPDU_108
#define M154_RX_MPDU_108_POS                                    (0U)
#define M154_RX_MPDU_108_LEN                                    (8U)
#define M154_RX_MPDU_108_MSK                                    (((1ULL<<M154_RX_MPDU_108_LEN)-1)<<M154_RX_MPDU_108_POS)
#define M154_RX_MPDU_108_UMSK                                   (~(((1ULL<<M154_RX_MPDU_108_LEN)-1)<<M154_RX_MPDU_108_POS))
#define M154_RX_MPDU_109                                        M154_RX_MPDU_109
#define M154_RX_MPDU_109_POS                                    (8U)
#define M154_RX_MPDU_109_LEN                                    (8U)
#define M154_RX_MPDU_109_MSK                                    (((1ULL<<M154_RX_MPDU_109_LEN)-1)<<M154_RX_MPDU_109_POS)
#define M154_RX_MPDU_109_UMSK                                   (~(((1ULL<<M154_RX_MPDU_109_LEN)-1)<<M154_RX_MPDU_109_POS))
#define M154_RX_MPDU_110                                        M154_RX_MPDU_110
#define M154_RX_MPDU_110_POS                                    (16U)
#define M154_RX_MPDU_110_LEN                                    (8U)
#define M154_RX_MPDU_110_MSK                                    (((1ULL<<M154_RX_MPDU_110_LEN)-1)<<M154_RX_MPDU_110_POS)
#define M154_RX_MPDU_110_UMSK                                   (~(((1ULL<<M154_RX_MPDU_110_LEN)-1)<<M154_RX_MPDU_110_POS))
#define M154_RX_MPDU_111                                        M154_RX_MPDU_111
#define M154_RX_MPDU_111_POS                                    (24U)
#define M154_RX_MPDU_111_LEN                                    (8U)
#define M154_RX_MPDU_111_MSK                                    (((1ULL<<M154_RX_MPDU_111_LEN)-1)<<M154_RX_MPDU_111_POS)
#define M154_RX_MPDU_111_UMSK                                   (~(((1ULL<<M154_RX_MPDU_111_LEN)-1)<<M154_RX_MPDU_111_POS))

/* 0x470 : m154_rx_mpdu_28 */
#define M154_RX_MPDU_28_OFFSET                                  (0x470)
#define M154_RX_MPDU_112                                        M154_RX_MPDU_112
#define M154_RX_MPDU_112_POS                                    (0U)
#define M154_RX_MPDU_112_LEN                                    (8U)
#define M154_RX_MPDU_112_MSK                                    (((1ULL<<M154_RX_MPDU_112_LEN)-1)<<M154_RX_MPDU_112_POS)
#define M154_RX_MPDU_112_UMSK                                   (~(((1ULL<<M154_RX_MPDU_112_LEN)-1)<<M154_RX_MPDU_112_POS))
#define M154_RX_MPDU_113                                        M154_RX_MPDU_113
#define M154_RX_MPDU_113_POS                                    (8U)
#define M154_RX_MPDU_113_LEN                                    (8U)
#define M154_RX_MPDU_113_MSK                                    (((1ULL<<M154_RX_MPDU_113_LEN)-1)<<M154_RX_MPDU_113_POS)
#define M154_RX_MPDU_113_UMSK                                   (~(((1ULL<<M154_RX_MPDU_113_LEN)-1)<<M154_RX_MPDU_113_POS))
#define M154_RX_MPDU_114                                        M154_RX_MPDU_114
#define M154_RX_MPDU_114_POS                                    (16U)
#define M154_RX_MPDU_114_LEN                                    (8U)
#define M154_RX_MPDU_114_MSK                                    (((1ULL<<M154_RX_MPDU_114_LEN)-1)<<M154_RX_MPDU_114_POS)
#define M154_RX_MPDU_114_UMSK                                   (~(((1ULL<<M154_RX_MPDU_114_LEN)-1)<<M154_RX_MPDU_114_POS))
#define M154_RX_MPDU_115                                        M154_RX_MPDU_115
#define M154_RX_MPDU_115_POS                                    (24U)
#define M154_RX_MPDU_115_LEN                                    (8U)
#define M154_RX_MPDU_115_MSK                                    (((1ULL<<M154_RX_MPDU_115_LEN)-1)<<M154_RX_MPDU_115_POS)
#define M154_RX_MPDU_115_UMSK                                   (~(((1ULL<<M154_RX_MPDU_115_LEN)-1)<<M154_RX_MPDU_115_POS))

/* 0x474 : m154_rx_mpdu_29 */
#define M154_RX_MPDU_29_OFFSET                                  (0x474)
#define M154_RX_MPDU_116                                        M154_RX_MPDU_116
#define M154_RX_MPDU_116_POS                                    (0U)
#define M154_RX_MPDU_116_LEN                                    (8U)
#define M154_RX_MPDU_116_MSK                                    (((1ULL<<M154_RX_MPDU_116_LEN)-1)<<M154_RX_MPDU_116_POS)
#define M154_RX_MPDU_116_UMSK                                   (~(((1ULL<<M154_RX_MPDU_116_LEN)-1)<<M154_RX_MPDU_116_POS))
#define M154_RX_MPDU_117                                        M154_RX_MPDU_117
#define M154_RX_MPDU_117_POS                                    (8U)
#define M154_RX_MPDU_117_LEN                                    (8U)
#define M154_RX_MPDU_117_MSK                                    (((1ULL<<M154_RX_MPDU_117_LEN)-1)<<M154_RX_MPDU_117_POS)
#define M154_RX_MPDU_117_UMSK                                   (~(((1ULL<<M154_RX_MPDU_117_LEN)-1)<<M154_RX_MPDU_117_POS))
#define M154_RX_MPDU_118                                        M154_RX_MPDU_118
#define M154_RX_MPDU_118_POS                                    (16U)
#define M154_RX_MPDU_118_LEN                                    (8U)
#define M154_RX_MPDU_118_MSK                                    (((1ULL<<M154_RX_MPDU_118_LEN)-1)<<M154_RX_MPDU_118_POS)
#define M154_RX_MPDU_118_UMSK                                   (~(((1ULL<<M154_RX_MPDU_118_LEN)-1)<<M154_RX_MPDU_118_POS))
#define M154_RX_MPDU_119                                        M154_RX_MPDU_119
#define M154_RX_MPDU_119_POS                                    (24U)
#define M154_RX_MPDU_119_LEN                                    (8U)
#define M154_RX_MPDU_119_MSK                                    (((1ULL<<M154_RX_MPDU_119_LEN)-1)<<M154_RX_MPDU_119_POS)
#define M154_RX_MPDU_119_UMSK                                   (~(((1ULL<<M154_RX_MPDU_119_LEN)-1)<<M154_RX_MPDU_119_POS))

/* 0x478 : m154_rx_mpdu_30 */
#define M154_RX_MPDU_30_OFFSET                                  (0x478)
#define M154_RX_MPDU_120                                        M154_RX_MPDU_120
#define M154_RX_MPDU_120_POS                                    (0U)
#define M154_RX_MPDU_120_LEN                                    (8U)
#define M154_RX_MPDU_120_MSK                                    (((1ULL<<M154_RX_MPDU_120_LEN)-1)<<M154_RX_MPDU_120_POS)
#define M154_RX_MPDU_120_UMSK                                   (~(((1ULL<<M154_RX_MPDU_120_LEN)-1)<<M154_RX_MPDU_120_POS))
#define M154_RX_MPDU_121                                        M154_RX_MPDU_121
#define M154_RX_MPDU_121_POS                                    (8U)
#define M154_RX_MPDU_121_LEN                                    (8U)
#define M154_RX_MPDU_121_MSK                                    (((1ULL<<M154_RX_MPDU_121_LEN)-1)<<M154_RX_MPDU_121_POS)
#define M154_RX_MPDU_121_UMSK                                   (~(((1ULL<<M154_RX_MPDU_121_LEN)-1)<<M154_RX_MPDU_121_POS))
#define M154_RX_MPDU_122                                        M154_RX_MPDU_122
#define M154_RX_MPDU_122_POS                                    (16U)
#define M154_RX_MPDU_122_LEN                                    (8U)
#define M154_RX_MPDU_122_MSK                                    (((1ULL<<M154_RX_MPDU_122_LEN)-1)<<M154_RX_MPDU_122_POS)
#define M154_RX_MPDU_122_UMSK                                   (~(((1ULL<<M154_RX_MPDU_122_LEN)-1)<<M154_RX_MPDU_122_POS))
#define M154_RX_MPDU_123                                        M154_RX_MPDU_123
#define M154_RX_MPDU_123_POS                                    (24U)
#define M154_RX_MPDU_123_LEN                                    (8U)
#define M154_RX_MPDU_123_MSK                                    (((1ULL<<M154_RX_MPDU_123_LEN)-1)<<M154_RX_MPDU_123_POS)
#define M154_RX_MPDU_123_UMSK                                   (~(((1ULL<<M154_RX_MPDU_123_LEN)-1)<<M154_RX_MPDU_123_POS))

/* 0x47C : m154_rx_mpdu_31 */
#define M154_RX_MPDU_31_OFFSET                                  (0x47C)
#define M154_RX_MPDU_124                                        M154_RX_MPDU_124
#define M154_RX_MPDU_124_POS                                    (0U)
#define M154_RX_MPDU_124_LEN                                    (8U)
#define M154_RX_MPDU_124_MSK                                    (((1ULL<<M154_RX_MPDU_124_LEN)-1)<<M154_RX_MPDU_124_POS)
#define M154_RX_MPDU_124_UMSK                                   (~(((1ULL<<M154_RX_MPDU_124_LEN)-1)<<M154_RX_MPDU_124_POS))
#define M154_RX_MPDU_FCS_07_00                                  M154_RX_MPDU_FCS_07_00
#define M154_RX_MPDU_FCS_07_00_POS                              (8U)
#define M154_RX_MPDU_FCS_07_00_LEN                              (8U)
#define M154_RX_MPDU_FCS_07_00_MSK                              (((1ULL<<M154_RX_MPDU_FCS_07_00_LEN)-1)<<M154_RX_MPDU_FCS_07_00_POS)
#define M154_RX_MPDU_FCS_07_00_UMSK                             (~(((1ULL<<M154_RX_MPDU_FCS_07_00_LEN)-1)<<M154_RX_MPDU_FCS_07_00_POS))
#define M154_RX_MPDU_FCS_15_08                                  M154_RX_MPDU_FCS_15_08
#define M154_RX_MPDU_FCS_15_08_POS                              (16U)
#define M154_RX_MPDU_FCS_15_08_LEN                              (8U)
#define M154_RX_MPDU_FCS_15_08_MSK                              (((1ULL<<M154_RX_MPDU_FCS_15_08_LEN)-1)<<M154_RX_MPDU_FCS_15_08_POS)
#define M154_RX_MPDU_FCS_15_08_UMSK                             (~(((1ULL<<M154_RX_MPDU_FCS_15_08_LEN)-1)<<M154_RX_MPDU_FCS_15_08_POS))
#define M154_RX_PHR                                             M154_RX_PHR
#define M154_RX_PHR_POS                                         (24U)
#define M154_RX_PHR_LEN                                         (8U)
#define M154_RX_PHR_MSK                                         (((1ULL<<M154_RX_PHR_LEN)-1)<<M154_RX_PHR_POS)
#define M154_RX_PHR_UMSK                                        (~(((1ULL<<M154_RX_PHR_LEN)-1)<<M154_RX_PHR_POS))

/* 0x480 : m154_rx_ppdu_status */
#define M154_RX_PPDU_STATUS_OFFSET                              (0x480)
#define M154_RX_MODE                                            M154_RX_MODE
#define M154_RX_MODE_POS                                        (0U)
#define M154_RX_MODE_LEN                                        (2U)
#define M154_RX_MODE_MSK                                        (((1ULL<<M154_RX_MODE_LEN)-1)<<M154_RX_MODE_POS)
#define M154_RX_MODE_UMSK                                       (~(((1ULL<<M154_RX_MODE_LEN)-1)<<M154_RX_MODE_POS))

/* 0x504 : m154_nbr_ctrl_0 */
#define M154_NBR_CTRL_0_OFFSET                                  (0x504)
#define M154_EFP_EN                                             M154_EFP_EN
#define M154_EFP_EN_POS                                         (0U)
#define M154_EFP_EN_LEN                                         (2U)
#define M154_EFP_EN_MSK                                         (((1ULL<<M154_EFP_EN_LEN)-1)<<M154_EFP_EN_POS)
#define M154_EFP_EN_UMSK                                        (~(((1ULL<<M154_EFP_EN_LEN)-1)<<M154_EFP_EN_POS))
#define M154_NBR_RESULT_FP_VALID                                M154_NBR_RESULT_FP_VALID
#define M154_NBR_RESULT_FP_VALID_POS                            (2U)
#define M154_NBR_RESULT_FP_VALID_LEN                            (1U)
#define M154_NBR_RESULT_FP_VALID_MSK                            (((1ULL<<M154_NBR_RESULT_FP_VALID_LEN)-1)<<M154_NBR_RESULT_FP_VALID_POS)
#define M154_NBR_RESULT_FP_VALID_UMSK                           (~(((1ULL<<M154_NBR_RESULT_FP_VALID_LEN)-1)<<M154_NBR_RESULT_FP_VALID_POS))
#define M154_NBR_RESULT_FP                                      M154_NBR_RESULT_FP
#define M154_NBR_RESULT_FP_POS                                  (3U)
#define M154_NBR_RESULT_FP_LEN                                  (1U)
#define M154_NBR_RESULT_FP_MSK                                  (((1ULL<<M154_NBR_RESULT_FP_LEN)-1)<<M154_NBR_RESULT_FP_POS)
#define M154_NBR_RESULT_FP_UMSK                                 (~(((1ULL<<M154_NBR_RESULT_FP_LEN)-1)<<M154_NBR_RESULT_FP_POS))
#define M154_NBR_RESULT_FP_IDX                                  M154_NBR_RESULT_FP_IDX
#define M154_NBR_RESULT_FP_IDX_POS                              (4U)
#define M154_NBR_RESULT_FP_IDX_LEN                              (7U)
#define M154_NBR_RESULT_FP_IDX_MSK                              (((1ULL<<M154_NBR_RESULT_FP_IDX_LEN)-1)<<M154_NBR_RESULT_FP_IDX_POS)
#define M154_NBR_RESULT_FP_IDX_UMSK                             (~(((1ULL<<M154_NBR_RESULT_FP_IDX_LEN)-1)<<M154_NBR_RESULT_FP_IDX_POS))
#define M154_NBR_SEARCH_DONE                                    M154_NBR_SEARCH_DONE
#define M154_NBR_SEARCH_DONE_POS                                (11U)
#define M154_NBR_SEARCH_DONE_LEN                                (1U)
#define M154_NBR_SEARCH_DONE_MSK                                (((1ULL<<M154_NBR_SEARCH_DONE_LEN)-1)<<M154_NBR_SEARCH_DONE_POS)
#define M154_NBR_SEARCH_DONE_UMSK                               (~(((1ULL<<M154_NBR_SEARCH_DONE_LEN)-1)<<M154_NBR_SEARCH_DONE_POS))
#define M154_RX_TX_TRANSITION_TIME_CNT                          M154_RX_TX_TRANSITION_TIME_CNT
#define M154_RX_TX_TRANSITION_TIME_CNT_POS                      (24U)
#define M154_RX_TX_TRANSITION_TIME_CNT_LEN                      (8U)
#define M154_RX_TX_TRANSITION_TIME_CNT_MSK                      (((1ULL<<M154_RX_TX_TRANSITION_TIME_CNT_LEN)-1)<<M154_RX_TX_TRANSITION_TIME_CNT_POS)
#define M154_RX_TX_TRANSITION_TIME_CNT_UMSK                     (~(((1ULL<<M154_RX_TX_TRANSITION_TIME_CNT_LEN)-1)<<M154_RX_TX_TRANSITION_TIME_CNT_POS))

/* 0x600 : m154_nbr_valid_31_00 */
#define M154_NBR_VALID_31_00_OFFSET                             (0x600)
#define M154_NBR_VALID_31_00                                    M154_NBR_VALID_31_00
#define M154_NBR_VALID_31_00_POS                                (0U)
#define M154_NBR_VALID_31_00_LEN                                (32U)
#define M154_NBR_VALID_31_00_MSK                                (((1ULL<<M154_NBR_VALID_31_00_LEN)-1)<<M154_NBR_VALID_31_00_POS)
#define M154_NBR_VALID_31_00_UMSK                               (~(((1ULL<<M154_NBR_VALID_31_00_LEN)-1)<<M154_NBR_VALID_31_00_POS))

/* 0x604 : m154_nbr_valid_63_32 */
#define M154_NBR_VALID_63_32_OFFSET                             (0x604)
#define M154_NBR_VALID_63_32                                    M154_NBR_VALID_63_32
#define M154_NBR_VALID_63_32_POS                                (0U)
#define M154_NBR_VALID_63_32_LEN                                (32U)
#define M154_NBR_VALID_63_32_MSK                                (((1ULL<<M154_NBR_VALID_63_32_LEN)-1)<<M154_NBR_VALID_63_32_POS)
#define M154_NBR_VALID_63_32_UMSK                               (~(((1ULL<<M154_NBR_VALID_63_32_LEN)-1)<<M154_NBR_VALID_63_32_POS))

/* 0x608 : m154_nbr_valid_95_64 */
#define M154_NBR_VALID_95_64_OFFSET                             (0x608)
#define M154_NBR_VALID_95_64                                    M154_NBR_VALID_95_64
#define M154_NBR_VALID_95_64_POS                                (0U)
#define M154_NBR_VALID_95_64_LEN                                (32U)
#define M154_NBR_VALID_95_64_MSK                                (((1ULL<<M154_NBR_VALID_95_64_LEN)-1)<<M154_NBR_VALID_95_64_POS)
#define M154_NBR_VALID_95_64_UMSK                               (~(((1ULL<<M154_NBR_VALID_95_64_LEN)-1)<<M154_NBR_VALID_95_64_POS))

/* 0x60C : m154_nbr_valid_127_96 */
#define M154_NBR_VALID_127_96_OFFSET                            (0x60C)
#define M154_NBR_VALID_127_96                                   M154_NBR_VALID_127_96
#define M154_NBR_VALID_127_96_POS                               (0U)
#define M154_NBR_VALID_127_96_LEN                               (32U)
#define M154_NBR_VALID_127_96_MSK                               (((1ULL<<M154_NBR_VALID_127_96_LEN)-1)<<M154_NBR_VALID_127_96_POS)
#define M154_NBR_VALID_127_96_UMSK                              (~(((1ULL<<M154_NBR_VALID_127_96_LEN)-1)<<M154_NBR_VALID_127_96_POS))

/* 0x610 : m154_nbr_ext_addr_mode_31_00 */
#define M154_NBR_EXT_ADDR_MODE_31_00_OFFSET                     (0x610)
#define M154_NBR_EXT_ADDR_MODE_31_00                            M154_NBR_EXT_ADDR_MODE_31_00
#define M154_NBR_EXT_ADDR_MODE_31_00_POS                        (0U)
#define M154_NBR_EXT_ADDR_MODE_31_00_LEN                        (32U)
#define M154_NBR_EXT_ADDR_MODE_31_00_MSK                        (((1ULL<<M154_NBR_EXT_ADDR_MODE_31_00_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_31_00_POS)
#define M154_NBR_EXT_ADDR_MODE_31_00_UMSK                       (~(((1ULL<<M154_NBR_EXT_ADDR_MODE_31_00_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_31_00_POS))

/* 0x614 : m154_nbr_ext_addr_mode_63_32 */
#define M154_NBR_EXT_ADDR_MODE_63_32_OFFSET                     (0x614)
#define M154_NBR_EXT_ADDR_MODE_63_32                            M154_NBR_EXT_ADDR_MODE_63_32
#define M154_NBR_EXT_ADDR_MODE_63_32_POS                        (0U)
#define M154_NBR_EXT_ADDR_MODE_63_32_LEN                        (32U)
#define M154_NBR_EXT_ADDR_MODE_63_32_MSK                        (((1ULL<<M154_NBR_EXT_ADDR_MODE_63_32_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_63_32_POS)
#define M154_NBR_EXT_ADDR_MODE_63_32_UMSK                       (~(((1ULL<<M154_NBR_EXT_ADDR_MODE_63_32_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_63_32_POS))

/* 0x618 : m154_nbr_ext_addr_mode_95_64 */
#define M154_NBR_EXT_ADDR_MODE_95_64_OFFSET                     (0x618)
#define M154_NBR_EXT_ADDR_MODE_95_64                            M154_NBR_EXT_ADDR_MODE_95_64
#define M154_NBR_EXT_ADDR_MODE_95_64_POS                        (0U)
#define M154_NBR_EXT_ADDR_MODE_95_64_LEN                        (32U)
#define M154_NBR_EXT_ADDR_MODE_95_64_MSK                        (((1ULL<<M154_NBR_EXT_ADDR_MODE_95_64_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_95_64_POS)
#define M154_NBR_EXT_ADDR_MODE_95_64_UMSK                       (~(((1ULL<<M154_NBR_EXT_ADDR_MODE_95_64_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_95_64_POS))

/* 0x61C : m154_nbr_ext_addr_mode_127_96 */
#define M154_NBR_EXT_ADDR_MODE_127_96_OFFSET                    (0x61C)
#define M154_NBR_EXT_ADDR_MODE_127_96                           M154_NBR_EXT_ADDR_MODE_127_96
#define M154_NBR_EXT_ADDR_MODE_127_96_POS                       (0U)
#define M154_NBR_EXT_ADDR_MODE_127_96_LEN                       (32U)
#define M154_NBR_EXT_ADDR_MODE_127_96_MSK                       (((1ULL<<M154_NBR_EXT_ADDR_MODE_127_96_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_127_96_POS)
#define M154_NBR_EXT_ADDR_MODE_127_96_UMSK                      (~(((1ULL<<M154_NBR_EXT_ADDR_MODE_127_96_LEN)-1)<<M154_NBR_EXT_ADDR_MODE_127_96_POS))

/* 0x620 : m154_nbr_fp_31_00 */
#define M154_NBR_FP_31_00_OFFSET                                (0x620)
#define M154_NBR_FP_31_00                                       M154_NBR_FP_31_00
#define M154_NBR_FP_31_00_POS                                   (0U)
#define M154_NBR_FP_31_00_LEN                                   (32U)
#define M154_NBR_FP_31_00_MSK                                   (((1ULL<<M154_NBR_FP_31_00_LEN)-1)<<M154_NBR_FP_31_00_POS)
#define M154_NBR_FP_31_00_UMSK                                  (~(((1ULL<<M154_NBR_FP_31_00_LEN)-1)<<M154_NBR_FP_31_00_POS))

/* 0x624 : m154_nbr_fp_63_32 */
#define M154_NBR_FP_63_32_OFFSET                                (0x624)
#define M154_NBR_FP_63_32                                       M154_NBR_FP_63_32
#define M154_NBR_FP_63_32_POS                                   (0U)
#define M154_NBR_FP_63_32_LEN                                   (32U)
#define M154_NBR_FP_63_32_MSK                                   (((1ULL<<M154_NBR_FP_63_32_LEN)-1)<<M154_NBR_FP_63_32_POS)
#define M154_NBR_FP_63_32_UMSK                                  (~(((1ULL<<M154_NBR_FP_63_32_LEN)-1)<<M154_NBR_FP_63_32_POS))

/* 0x628 : m154_nbr_fp_95_64 */
#define M154_NBR_FP_95_64_OFFSET                                (0x628)
#define M154_NBR_FP_95_64                                       M154_NBR_FP_95_64
#define M154_NBR_FP_95_64_POS                                   (0U)
#define M154_NBR_FP_95_64_LEN                                   (32U)
#define M154_NBR_FP_95_64_MSK                                   (((1ULL<<M154_NBR_FP_95_64_LEN)-1)<<M154_NBR_FP_95_64_POS)
#define M154_NBR_FP_95_64_UMSK                                  (~(((1ULL<<M154_NBR_FP_95_64_LEN)-1)<<M154_NBR_FP_95_64_POS))

/* 0x62C : m154_nbr_fp_127_96 */
#define M154_NBR_FP_127_96_OFFSET                               (0x62C)
#define M154_NBR_FP_127_96                                      M154_NBR_FP_127_96
#define M154_NBR_FP_127_96_POS                                  (0U)
#define M154_NBR_FP_127_96_LEN                                  (32U)
#define M154_NBR_FP_127_96_MSK                                  (((1ULL<<M154_NBR_FP_127_96_LEN)-1)<<M154_NBR_FP_127_96_POS)
#define M154_NBR_FP_127_96_UMSK                                 (~(((1ULL<<M154_NBR_FP_127_96_LEN)-1)<<M154_NBR_FP_127_96_POS))

/* 0x630 : m154_nbr_addr_01_00 */
#define M154_NBR_ADDR_01_00_OFFSET                              (0x630)
#define M154_NBR_ADDR_00                                        M154_NBR_ADDR_00
#define M154_NBR_ADDR_00_POS                                    (0U)
#define M154_NBR_ADDR_00_LEN                                    (16U)
#define M154_NBR_ADDR_00_MSK                                    (((1ULL<<M154_NBR_ADDR_00_LEN)-1)<<M154_NBR_ADDR_00_POS)
#define M154_NBR_ADDR_00_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_00_LEN)-1)<<M154_NBR_ADDR_00_POS))
#define M154_NBR_ADDR_01                                        M154_NBR_ADDR_01
#define M154_NBR_ADDR_01_POS                                    (16U)
#define M154_NBR_ADDR_01_LEN                                    (16U)
#define M154_NBR_ADDR_01_MSK                                    (((1ULL<<M154_NBR_ADDR_01_LEN)-1)<<M154_NBR_ADDR_01_POS)
#define M154_NBR_ADDR_01_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_01_LEN)-1)<<M154_NBR_ADDR_01_POS))

/* 0x634 : m154_nbr_addr_03_02 */
#define M154_NBR_ADDR_03_02_OFFSET                              (0x634)
#define M154_NBR_ADDR_02                                        M154_NBR_ADDR_02
#define M154_NBR_ADDR_02_POS                                    (0U)
#define M154_NBR_ADDR_02_LEN                                    (16U)
#define M154_NBR_ADDR_02_MSK                                    (((1ULL<<M154_NBR_ADDR_02_LEN)-1)<<M154_NBR_ADDR_02_POS)
#define M154_NBR_ADDR_02_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_02_LEN)-1)<<M154_NBR_ADDR_02_POS))
#define M154_NBR_ADDR_03                                        M154_NBR_ADDR_03
#define M154_NBR_ADDR_03_POS                                    (16U)
#define M154_NBR_ADDR_03_LEN                                    (16U)
#define M154_NBR_ADDR_03_MSK                                    (((1ULL<<M154_NBR_ADDR_03_LEN)-1)<<M154_NBR_ADDR_03_POS)
#define M154_NBR_ADDR_03_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_03_LEN)-1)<<M154_NBR_ADDR_03_POS))

/* 0x638 : m154_nbr_addr_05_04 */
#define M154_NBR_ADDR_05_04_OFFSET                              (0x638)
#define M154_NBR_ADDR_04                                        M154_NBR_ADDR_04
#define M154_NBR_ADDR_04_POS                                    (0U)
#define M154_NBR_ADDR_04_LEN                                    (16U)
#define M154_NBR_ADDR_04_MSK                                    (((1ULL<<M154_NBR_ADDR_04_LEN)-1)<<M154_NBR_ADDR_04_POS)
#define M154_NBR_ADDR_04_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_04_LEN)-1)<<M154_NBR_ADDR_04_POS))
#define M154_NBR_ADDR_05                                        M154_NBR_ADDR_05
#define M154_NBR_ADDR_05_POS                                    (16U)
#define M154_NBR_ADDR_05_LEN                                    (16U)
#define M154_NBR_ADDR_05_MSK                                    (((1ULL<<M154_NBR_ADDR_05_LEN)-1)<<M154_NBR_ADDR_05_POS)
#define M154_NBR_ADDR_05_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_05_LEN)-1)<<M154_NBR_ADDR_05_POS))

/* 0x63C : m154_nbr_addr_07_06 */
#define M154_NBR_ADDR_07_06_OFFSET                              (0x63C)
#define M154_NBR_ADDR_06                                        M154_NBR_ADDR_06
#define M154_NBR_ADDR_06_POS                                    (0U)
#define M154_NBR_ADDR_06_LEN                                    (16U)
#define M154_NBR_ADDR_06_MSK                                    (((1ULL<<M154_NBR_ADDR_06_LEN)-1)<<M154_NBR_ADDR_06_POS)
#define M154_NBR_ADDR_06_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_06_LEN)-1)<<M154_NBR_ADDR_06_POS))
#define M154_NBR_ADDR_07                                        M154_NBR_ADDR_07
#define M154_NBR_ADDR_07_POS                                    (16U)
#define M154_NBR_ADDR_07_LEN                                    (16U)
#define M154_NBR_ADDR_07_MSK                                    (((1ULL<<M154_NBR_ADDR_07_LEN)-1)<<M154_NBR_ADDR_07_POS)
#define M154_NBR_ADDR_07_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_07_LEN)-1)<<M154_NBR_ADDR_07_POS))

/* 0x640 : m154_nbr_addr_09_08 */
#define M154_NBR_ADDR_09_08_OFFSET                              (0x640)
#define M154_NBR_ADDR_08                                        M154_NBR_ADDR_08
#define M154_NBR_ADDR_08_POS                                    (0U)
#define M154_NBR_ADDR_08_LEN                                    (16U)
#define M154_NBR_ADDR_08_MSK                                    (((1ULL<<M154_NBR_ADDR_08_LEN)-1)<<M154_NBR_ADDR_08_POS)
#define M154_NBR_ADDR_08_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_08_LEN)-1)<<M154_NBR_ADDR_08_POS))
#define M154_NBR_ADDR_09                                        M154_NBR_ADDR_09
#define M154_NBR_ADDR_09_POS                                    (16U)
#define M154_NBR_ADDR_09_LEN                                    (16U)
#define M154_NBR_ADDR_09_MSK                                    (((1ULL<<M154_NBR_ADDR_09_LEN)-1)<<M154_NBR_ADDR_09_POS)
#define M154_NBR_ADDR_09_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_09_LEN)-1)<<M154_NBR_ADDR_09_POS))

/* 0x644 : m154_nbr_addr_11_10 */
#define M154_NBR_ADDR_11_10_OFFSET                              (0x644)
#define M154_NBR_ADDR_10                                        M154_NBR_ADDR_10
#define M154_NBR_ADDR_10_POS                                    (0U)
#define M154_NBR_ADDR_10_LEN                                    (16U)
#define M154_NBR_ADDR_10_MSK                                    (((1ULL<<M154_NBR_ADDR_10_LEN)-1)<<M154_NBR_ADDR_10_POS)
#define M154_NBR_ADDR_10_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_10_LEN)-1)<<M154_NBR_ADDR_10_POS))
#define M154_NBR_ADDR_11                                        M154_NBR_ADDR_11
#define M154_NBR_ADDR_11_POS                                    (16U)
#define M154_NBR_ADDR_11_LEN                                    (16U)
#define M154_NBR_ADDR_11_MSK                                    (((1ULL<<M154_NBR_ADDR_11_LEN)-1)<<M154_NBR_ADDR_11_POS)
#define M154_NBR_ADDR_11_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_11_LEN)-1)<<M154_NBR_ADDR_11_POS))

/* 0x648 : m154_nbr_addr_13_12 */
#define M154_NBR_ADDR_13_12_OFFSET                              (0x648)
#define M154_NBR_ADDR_12                                        M154_NBR_ADDR_12
#define M154_NBR_ADDR_12_POS                                    (0U)
#define M154_NBR_ADDR_12_LEN                                    (16U)
#define M154_NBR_ADDR_12_MSK                                    (((1ULL<<M154_NBR_ADDR_12_LEN)-1)<<M154_NBR_ADDR_12_POS)
#define M154_NBR_ADDR_12_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_12_LEN)-1)<<M154_NBR_ADDR_12_POS))
#define M154_NBR_ADDR_13                                        M154_NBR_ADDR_13
#define M154_NBR_ADDR_13_POS                                    (16U)
#define M154_NBR_ADDR_13_LEN                                    (16U)
#define M154_NBR_ADDR_13_MSK                                    (((1ULL<<M154_NBR_ADDR_13_LEN)-1)<<M154_NBR_ADDR_13_POS)
#define M154_NBR_ADDR_13_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_13_LEN)-1)<<M154_NBR_ADDR_13_POS))

/* 0x64C : m154_nbr_addr_15_14 */
#define M154_NBR_ADDR_15_14_OFFSET                              (0x64C)
#define M154_NBR_ADDR_14                                        M154_NBR_ADDR_14
#define M154_NBR_ADDR_14_POS                                    (0U)
#define M154_NBR_ADDR_14_LEN                                    (16U)
#define M154_NBR_ADDR_14_MSK                                    (((1ULL<<M154_NBR_ADDR_14_LEN)-1)<<M154_NBR_ADDR_14_POS)
#define M154_NBR_ADDR_14_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_14_LEN)-1)<<M154_NBR_ADDR_14_POS))
#define M154_NBR_ADDR_15                                        M154_NBR_ADDR_15
#define M154_NBR_ADDR_15_POS                                    (16U)
#define M154_NBR_ADDR_15_LEN                                    (16U)
#define M154_NBR_ADDR_15_MSK                                    (((1ULL<<M154_NBR_ADDR_15_LEN)-1)<<M154_NBR_ADDR_15_POS)
#define M154_NBR_ADDR_15_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_15_LEN)-1)<<M154_NBR_ADDR_15_POS))

/* 0x650 : m154_nbr_addr_17_16 */
#define M154_NBR_ADDR_17_16_OFFSET                              (0x650)
#define M154_NBR_ADDR_16                                        M154_NBR_ADDR_16
#define M154_NBR_ADDR_16_POS                                    (0U)
#define M154_NBR_ADDR_16_LEN                                    (16U)
#define M154_NBR_ADDR_16_MSK                                    (((1ULL<<M154_NBR_ADDR_16_LEN)-1)<<M154_NBR_ADDR_16_POS)
#define M154_NBR_ADDR_16_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_16_LEN)-1)<<M154_NBR_ADDR_16_POS))
#define M154_NBR_ADDR_17                                        M154_NBR_ADDR_17
#define M154_NBR_ADDR_17_POS                                    (16U)
#define M154_NBR_ADDR_17_LEN                                    (16U)
#define M154_NBR_ADDR_17_MSK                                    (((1ULL<<M154_NBR_ADDR_17_LEN)-1)<<M154_NBR_ADDR_17_POS)
#define M154_NBR_ADDR_17_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_17_LEN)-1)<<M154_NBR_ADDR_17_POS))

/* 0x654 : m154_nbr_addr_19_18 */
#define M154_NBR_ADDR_19_18_OFFSET                              (0x654)
#define M154_NBR_ADDR_18                                        M154_NBR_ADDR_18
#define M154_NBR_ADDR_18_POS                                    (0U)
#define M154_NBR_ADDR_18_LEN                                    (16U)
#define M154_NBR_ADDR_18_MSK                                    (((1ULL<<M154_NBR_ADDR_18_LEN)-1)<<M154_NBR_ADDR_18_POS)
#define M154_NBR_ADDR_18_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_18_LEN)-1)<<M154_NBR_ADDR_18_POS))
#define M154_NBR_ADDR_19                                        M154_NBR_ADDR_19
#define M154_NBR_ADDR_19_POS                                    (16U)
#define M154_NBR_ADDR_19_LEN                                    (16U)
#define M154_NBR_ADDR_19_MSK                                    (((1ULL<<M154_NBR_ADDR_19_LEN)-1)<<M154_NBR_ADDR_19_POS)
#define M154_NBR_ADDR_19_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_19_LEN)-1)<<M154_NBR_ADDR_19_POS))

/* 0x658 : m154_nbr_addr_21_20 */
#define M154_NBR_ADDR_21_20_OFFSET                              (0x658)
#define M154_NBR_ADDR_20                                        M154_NBR_ADDR_20
#define M154_NBR_ADDR_20_POS                                    (0U)
#define M154_NBR_ADDR_20_LEN                                    (16U)
#define M154_NBR_ADDR_20_MSK                                    (((1ULL<<M154_NBR_ADDR_20_LEN)-1)<<M154_NBR_ADDR_20_POS)
#define M154_NBR_ADDR_20_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_20_LEN)-1)<<M154_NBR_ADDR_20_POS))
#define M154_NBR_ADDR_21                                        M154_NBR_ADDR_21
#define M154_NBR_ADDR_21_POS                                    (16U)
#define M154_NBR_ADDR_21_LEN                                    (16U)
#define M154_NBR_ADDR_21_MSK                                    (((1ULL<<M154_NBR_ADDR_21_LEN)-1)<<M154_NBR_ADDR_21_POS)
#define M154_NBR_ADDR_21_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_21_LEN)-1)<<M154_NBR_ADDR_21_POS))

/* 0x65C : m154_nbr_addr_23_22 */
#define M154_NBR_ADDR_23_22_OFFSET                              (0x65C)
#define M154_NBR_ADDR_22                                        M154_NBR_ADDR_22
#define M154_NBR_ADDR_22_POS                                    (0U)
#define M154_NBR_ADDR_22_LEN                                    (16U)
#define M154_NBR_ADDR_22_MSK                                    (((1ULL<<M154_NBR_ADDR_22_LEN)-1)<<M154_NBR_ADDR_22_POS)
#define M154_NBR_ADDR_22_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_22_LEN)-1)<<M154_NBR_ADDR_22_POS))
#define M154_NBR_ADDR_23                                        M154_NBR_ADDR_23
#define M154_NBR_ADDR_23_POS                                    (16U)
#define M154_NBR_ADDR_23_LEN                                    (16U)
#define M154_NBR_ADDR_23_MSK                                    (((1ULL<<M154_NBR_ADDR_23_LEN)-1)<<M154_NBR_ADDR_23_POS)
#define M154_NBR_ADDR_23_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_23_LEN)-1)<<M154_NBR_ADDR_23_POS))

/* 0x660 : m154_nbr_addr_25_24 */
#define M154_NBR_ADDR_25_24_OFFSET                              (0x660)
#define M154_NBR_ADDR_24                                        M154_NBR_ADDR_24
#define M154_NBR_ADDR_24_POS                                    (0U)
#define M154_NBR_ADDR_24_LEN                                    (16U)
#define M154_NBR_ADDR_24_MSK                                    (((1ULL<<M154_NBR_ADDR_24_LEN)-1)<<M154_NBR_ADDR_24_POS)
#define M154_NBR_ADDR_24_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_24_LEN)-1)<<M154_NBR_ADDR_24_POS))
#define M154_NBR_ADDR_25                                        M154_NBR_ADDR_25
#define M154_NBR_ADDR_25_POS                                    (16U)
#define M154_NBR_ADDR_25_LEN                                    (16U)
#define M154_NBR_ADDR_25_MSK                                    (((1ULL<<M154_NBR_ADDR_25_LEN)-1)<<M154_NBR_ADDR_25_POS)
#define M154_NBR_ADDR_25_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_25_LEN)-1)<<M154_NBR_ADDR_25_POS))

/* 0x664 : m154_nbr_addr_27_26 */
#define M154_NBR_ADDR_27_26_OFFSET                              (0x664)
#define M154_NBR_ADDR_26                                        M154_NBR_ADDR_26
#define M154_NBR_ADDR_26_POS                                    (0U)
#define M154_NBR_ADDR_26_LEN                                    (16U)
#define M154_NBR_ADDR_26_MSK                                    (((1ULL<<M154_NBR_ADDR_26_LEN)-1)<<M154_NBR_ADDR_26_POS)
#define M154_NBR_ADDR_26_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_26_LEN)-1)<<M154_NBR_ADDR_26_POS))
#define M154_NBR_ADDR_27                                        M154_NBR_ADDR_27
#define M154_NBR_ADDR_27_POS                                    (16U)
#define M154_NBR_ADDR_27_LEN                                    (16U)
#define M154_NBR_ADDR_27_MSK                                    (((1ULL<<M154_NBR_ADDR_27_LEN)-1)<<M154_NBR_ADDR_27_POS)
#define M154_NBR_ADDR_27_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_27_LEN)-1)<<M154_NBR_ADDR_27_POS))

/* 0x668 : m154_nbr_addr_29_28 */
#define M154_NBR_ADDR_29_28_OFFSET                              (0x668)
#define M154_NBR_ADDR_28                                        M154_NBR_ADDR_28
#define M154_NBR_ADDR_28_POS                                    (0U)
#define M154_NBR_ADDR_28_LEN                                    (16U)
#define M154_NBR_ADDR_28_MSK                                    (((1ULL<<M154_NBR_ADDR_28_LEN)-1)<<M154_NBR_ADDR_28_POS)
#define M154_NBR_ADDR_28_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_28_LEN)-1)<<M154_NBR_ADDR_28_POS))
#define M154_NBR_ADDR_29                                        M154_NBR_ADDR_29
#define M154_NBR_ADDR_29_POS                                    (16U)
#define M154_NBR_ADDR_29_LEN                                    (16U)
#define M154_NBR_ADDR_29_MSK                                    (((1ULL<<M154_NBR_ADDR_29_LEN)-1)<<M154_NBR_ADDR_29_POS)
#define M154_NBR_ADDR_29_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_29_LEN)-1)<<M154_NBR_ADDR_29_POS))

/* 0x66C : m154_nbr_addr_31_30 */
#define M154_NBR_ADDR_31_30_OFFSET                              (0x66C)
#define M154_NBR_ADDR_30                                        M154_NBR_ADDR_30
#define M154_NBR_ADDR_30_POS                                    (0U)
#define M154_NBR_ADDR_30_LEN                                    (16U)
#define M154_NBR_ADDR_30_MSK                                    (((1ULL<<M154_NBR_ADDR_30_LEN)-1)<<M154_NBR_ADDR_30_POS)
#define M154_NBR_ADDR_30_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_30_LEN)-1)<<M154_NBR_ADDR_30_POS))
#define M154_NBR_ADDR_31                                        M154_NBR_ADDR_31
#define M154_NBR_ADDR_31_POS                                    (16U)
#define M154_NBR_ADDR_31_LEN                                    (16U)
#define M154_NBR_ADDR_31_MSK                                    (((1ULL<<M154_NBR_ADDR_31_LEN)-1)<<M154_NBR_ADDR_31_POS)
#define M154_NBR_ADDR_31_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_31_LEN)-1)<<M154_NBR_ADDR_31_POS))

/* 0x670 : m154_nbr_addr_33_32 */
#define M154_NBR_ADDR_33_32_OFFSET                              (0x670)
#define M154_NBR_ADDR_32                                        M154_NBR_ADDR_32
#define M154_NBR_ADDR_32_POS                                    (0U)
#define M154_NBR_ADDR_32_LEN                                    (16U)
#define M154_NBR_ADDR_32_MSK                                    (((1ULL<<M154_NBR_ADDR_32_LEN)-1)<<M154_NBR_ADDR_32_POS)
#define M154_NBR_ADDR_32_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_32_LEN)-1)<<M154_NBR_ADDR_32_POS))
#define M154_NBR_ADDR_33                                        M154_NBR_ADDR_33
#define M154_NBR_ADDR_33_POS                                    (16U)
#define M154_NBR_ADDR_33_LEN                                    (16U)
#define M154_NBR_ADDR_33_MSK                                    (((1ULL<<M154_NBR_ADDR_33_LEN)-1)<<M154_NBR_ADDR_33_POS)
#define M154_NBR_ADDR_33_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_33_LEN)-1)<<M154_NBR_ADDR_33_POS))

/* 0x674 : m154_nbr_addr_35_34 */
#define M154_NBR_ADDR_35_34_OFFSET                              (0x674)
#define M154_NBR_ADDR_34                                        M154_NBR_ADDR_34
#define M154_NBR_ADDR_34_POS                                    (0U)
#define M154_NBR_ADDR_34_LEN                                    (16U)
#define M154_NBR_ADDR_34_MSK                                    (((1ULL<<M154_NBR_ADDR_34_LEN)-1)<<M154_NBR_ADDR_34_POS)
#define M154_NBR_ADDR_34_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_34_LEN)-1)<<M154_NBR_ADDR_34_POS))
#define M154_NBR_ADDR_35                                        M154_NBR_ADDR_35
#define M154_NBR_ADDR_35_POS                                    (16U)
#define M154_NBR_ADDR_35_LEN                                    (16U)
#define M154_NBR_ADDR_35_MSK                                    (((1ULL<<M154_NBR_ADDR_35_LEN)-1)<<M154_NBR_ADDR_35_POS)
#define M154_NBR_ADDR_35_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_35_LEN)-1)<<M154_NBR_ADDR_35_POS))

/* 0x678 : m154_nbr_addr_37_36 */
#define M154_NBR_ADDR_37_36_OFFSET                              (0x678)
#define M154_NBR_ADDR_36                                        M154_NBR_ADDR_36
#define M154_NBR_ADDR_36_POS                                    (0U)
#define M154_NBR_ADDR_36_LEN                                    (16U)
#define M154_NBR_ADDR_36_MSK                                    (((1ULL<<M154_NBR_ADDR_36_LEN)-1)<<M154_NBR_ADDR_36_POS)
#define M154_NBR_ADDR_36_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_36_LEN)-1)<<M154_NBR_ADDR_36_POS))
#define M154_NBR_ADDR_37                                        M154_NBR_ADDR_37
#define M154_NBR_ADDR_37_POS                                    (16U)
#define M154_NBR_ADDR_37_LEN                                    (16U)
#define M154_NBR_ADDR_37_MSK                                    (((1ULL<<M154_NBR_ADDR_37_LEN)-1)<<M154_NBR_ADDR_37_POS)
#define M154_NBR_ADDR_37_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_37_LEN)-1)<<M154_NBR_ADDR_37_POS))

/* 0x67C : m154_nbr_addr_39_38 */
#define M154_NBR_ADDR_39_38_OFFSET                              (0x67C)
#define M154_NBR_ADDR_38                                        M154_NBR_ADDR_38
#define M154_NBR_ADDR_38_POS                                    (0U)
#define M154_NBR_ADDR_38_LEN                                    (16U)
#define M154_NBR_ADDR_38_MSK                                    (((1ULL<<M154_NBR_ADDR_38_LEN)-1)<<M154_NBR_ADDR_38_POS)
#define M154_NBR_ADDR_38_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_38_LEN)-1)<<M154_NBR_ADDR_38_POS))
#define M154_NBR_ADDR_39                                        M154_NBR_ADDR_39
#define M154_NBR_ADDR_39_POS                                    (16U)
#define M154_NBR_ADDR_39_LEN                                    (16U)
#define M154_NBR_ADDR_39_MSK                                    (((1ULL<<M154_NBR_ADDR_39_LEN)-1)<<M154_NBR_ADDR_39_POS)
#define M154_NBR_ADDR_39_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_39_LEN)-1)<<M154_NBR_ADDR_39_POS))

/* 0x680 : m154_nbr_addr_41_40 */
#define M154_NBR_ADDR_41_40_OFFSET                              (0x680)
#define M154_NBR_ADDR_40                                        M154_NBR_ADDR_40
#define M154_NBR_ADDR_40_POS                                    (0U)
#define M154_NBR_ADDR_40_LEN                                    (16U)
#define M154_NBR_ADDR_40_MSK                                    (((1ULL<<M154_NBR_ADDR_40_LEN)-1)<<M154_NBR_ADDR_40_POS)
#define M154_NBR_ADDR_40_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_40_LEN)-1)<<M154_NBR_ADDR_40_POS))
#define M154_NBR_ADDR_41                                        M154_NBR_ADDR_41
#define M154_NBR_ADDR_41_POS                                    (16U)
#define M154_NBR_ADDR_41_LEN                                    (16U)
#define M154_NBR_ADDR_41_MSK                                    (((1ULL<<M154_NBR_ADDR_41_LEN)-1)<<M154_NBR_ADDR_41_POS)
#define M154_NBR_ADDR_41_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_41_LEN)-1)<<M154_NBR_ADDR_41_POS))

/* 0x684 : m154_nbr_addr_43_42 */
#define M154_NBR_ADDR_43_42_OFFSET                              (0x684)
#define M154_NBR_ADDR_42                                        M154_NBR_ADDR_42
#define M154_NBR_ADDR_42_POS                                    (0U)
#define M154_NBR_ADDR_42_LEN                                    (16U)
#define M154_NBR_ADDR_42_MSK                                    (((1ULL<<M154_NBR_ADDR_42_LEN)-1)<<M154_NBR_ADDR_42_POS)
#define M154_NBR_ADDR_42_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_42_LEN)-1)<<M154_NBR_ADDR_42_POS))
#define M154_NBR_ADDR_43                                        M154_NBR_ADDR_43
#define M154_NBR_ADDR_43_POS                                    (16U)
#define M154_NBR_ADDR_43_LEN                                    (16U)
#define M154_NBR_ADDR_43_MSK                                    (((1ULL<<M154_NBR_ADDR_43_LEN)-1)<<M154_NBR_ADDR_43_POS)
#define M154_NBR_ADDR_43_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_43_LEN)-1)<<M154_NBR_ADDR_43_POS))

/* 0x688 : m154_nbr_addr_45_44 */
#define M154_NBR_ADDR_45_44_OFFSET                              (0x688)
#define M154_NBR_ADDR_44                                        M154_NBR_ADDR_44
#define M154_NBR_ADDR_44_POS                                    (0U)
#define M154_NBR_ADDR_44_LEN                                    (16U)
#define M154_NBR_ADDR_44_MSK                                    (((1ULL<<M154_NBR_ADDR_44_LEN)-1)<<M154_NBR_ADDR_44_POS)
#define M154_NBR_ADDR_44_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_44_LEN)-1)<<M154_NBR_ADDR_44_POS))
#define M154_NBR_ADDR_45                                        M154_NBR_ADDR_45
#define M154_NBR_ADDR_45_POS                                    (16U)
#define M154_NBR_ADDR_45_LEN                                    (16U)
#define M154_NBR_ADDR_45_MSK                                    (((1ULL<<M154_NBR_ADDR_45_LEN)-1)<<M154_NBR_ADDR_45_POS)
#define M154_NBR_ADDR_45_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_45_LEN)-1)<<M154_NBR_ADDR_45_POS))

/* 0x68C : m154_nbr_addr_47_46 */
#define M154_NBR_ADDR_47_46_OFFSET                              (0x68C)
#define M154_NBR_ADDR_46                                        M154_NBR_ADDR_46
#define M154_NBR_ADDR_46_POS                                    (0U)
#define M154_NBR_ADDR_46_LEN                                    (16U)
#define M154_NBR_ADDR_46_MSK                                    (((1ULL<<M154_NBR_ADDR_46_LEN)-1)<<M154_NBR_ADDR_46_POS)
#define M154_NBR_ADDR_46_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_46_LEN)-1)<<M154_NBR_ADDR_46_POS))
#define M154_NBR_ADDR_47                                        M154_NBR_ADDR_47
#define M154_NBR_ADDR_47_POS                                    (16U)
#define M154_NBR_ADDR_47_LEN                                    (16U)
#define M154_NBR_ADDR_47_MSK                                    (((1ULL<<M154_NBR_ADDR_47_LEN)-1)<<M154_NBR_ADDR_47_POS)
#define M154_NBR_ADDR_47_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_47_LEN)-1)<<M154_NBR_ADDR_47_POS))

/* 0x690 : m154_nbr_addr_49_48 */
#define M154_NBR_ADDR_49_48_OFFSET                              (0x690)
#define M154_NBR_ADDR_48                                        M154_NBR_ADDR_48
#define M154_NBR_ADDR_48_POS                                    (0U)
#define M154_NBR_ADDR_48_LEN                                    (16U)
#define M154_NBR_ADDR_48_MSK                                    (((1ULL<<M154_NBR_ADDR_48_LEN)-1)<<M154_NBR_ADDR_48_POS)
#define M154_NBR_ADDR_48_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_48_LEN)-1)<<M154_NBR_ADDR_48_POS))
#define M154_NBR_ADDR_49                                        M154_NBR_ADDR_49
#define M154_NBR_ADDR_49_POS                                    (16U)
#define M154_NBR_ADDR_49_LEN                                    (16U)
#define M154_NBR_ADDR_49_MSK                                    (((1ULL<<M154_NBR_ADDR_49_LEN)-1)<<M154_NBR_ADDR_49_POS)
#define M154_NBR_ADDR_49_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_49_LEN)-1)<<M154_NBR_ADDR_49_POS))

/* 0x694 : m154_nbr_addr_51_50 */
#define M154_NBR_ADDR_51_50_OFFSET                              (0x694)
#define M154_NBR_ADDR_50                                        M154_NBR_ADDR_50
#define M154_NBR_ADDR_50_POS                                    (0U)
#define M154_NBR_ADDR_50_LEN                                    (16U)
#define M154_NBR_ADDR_50_MSK                                    (((1ULL<<M154_NBR_ADDR_50_LEN)-1)<<M154_NBR_ADDR_50_POS)
#define M154_NBR_ADDR_50_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_50_LEN)-1)<<M154_NBR_ADDR_50_POS))
#define M154_NBR_ADDR_51                                        M154_NBR_ADDR_51
#define M154_NBR_ADDR_51_POS                                    (16U)
#define M154_NBR_ADDR_51_LEN                                    (16U)
#define M154_NBR_ADDR_51_MSK                                    (((1ULL<<M154_NBR_ADDR_51_LEN)-1)<<M154_NBR_ADDR_51_POS)
#define M154_NBR_ADDR_51_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_51_LEN)-1)<<M154_NBR_ADDR_51_POS))

/* 0x698 : m154_nbr_addr_53_52 */
#define M154_NBR_ADDR_53_52_OFFSET                              (0x698)
#define M154_NBR_ADDR_52                                        M154_NBR_ADDR_52
#define M154_NBR_ADDR_52_POS                                    (0U)
#define M154_NBR_ADDR_52_LEN                                    (16U)
#define M154_NBR_ADDR_52_MSK                                    (((1ULL<<M154_NBR_ADDR_52_LEN)-1)<<M154_NBR_ADDR_52_POS)
#define M154_NBR_ADDR_52_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_52_LEN)-1)<<M154_NBR_ADDR_52_POS))
#define M154_NBR_ADDR_53                                        M154_NBR_ADDR_53
#define M154_NBR_ADDR_53_POS                                    (16U)
#define M154_NBR_ADDR_53_LEN                                    (16U)
#define M154_NBR_ADDR_53_MSK                                    (((1ULL<<M154_NBR_ADDR_53_LEN)-1)<<M154_NBR_ADDR_53_POS)
#define M154_NBR_ADDR_53_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_53_LEN)-1)<<M154_NBR_ADDR_53_POS))

/* 0x69C : m154_nbr_addr_55_54 */
#define M154_NBR_ADDR_55_54_OFFSET                              (0x69C)
#define M154_NBR_ADDR_54                                        M154_NBR_ADDR_54
#define M154_NBR_ADDR_54_POS                                    (0U)
#define M154_NBR_ADDR_54_LEN                                    (16U)
#define M154_NBR_ADDR_54_MSK                                    (((1ULL<<M154_NBR_ADDR_54_LEN)-1)<<M154_NBR_ADDR_54_POS)
#define M154_NBR_ADDR_54_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_54_LEN)-1)<<M154_NBR_ADDR_54_POS))
#define M154_NBR_ADDR_55                                        M154_NBR_ADDR_55
#define M154_NBR_ADDR_55_POS                                    (16U)
#define M154_NBR_ADDR_55_LEN                                    (16U)
#define M154_NBR_ADDR_55_MSK                                    (((1ULL<<M154_NBR_ADDR_55_LEN)-1)<<M154_NBR_ADDR_55_POS)
#define M154_NBR_ADDR_55_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_55_LEN)-1)<<M154_NBR_ADDR_55_POS))

/* 0x6A0 : m154_nbr_addr_57_56 */
#define M154_NBR_ADDR_57_56_OFFSET                              (0x6A0)
#define M154_NBR_ADDR_56                                        M154_NBR_ADDR_56
#define M154_NBR_ADDR_56_POS                                    (0U)
#define M154_NBR_ADDR_56_LEN                                    (16U)
#define M154_NBR_ADDR_56_MSK                                    (((1ULL<<M154_NBR_ADDR_56_LEN)-1)<<M154_NBR_ADDR_56_POS)
#define M154_NBR_ADDR_56_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_56_LEN)-1)<<M154_NBR_ADDR_56_POS))
#define M154_NBR_ADDR_57                                        M154_NBR_ADDR_57
#define M154_NBR_ADDR_57_POS                                    (16U)
#define M154_NBR_ADDR_57_LEN                                    (16U)
#define M154_NBR_ADDR_57_MSK                                    (((1ULL<<M154_NBR_ADDR_57_LEN)-1)<<M154_NBR_ADDR_57_POS)
#define M154_NBR_ADDR_57_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_57_LEN)-1)<<M154_NBR_ADDR_57_POS))

/* 0x6A4 : m154_nbr_addr_59_58 */
#define M154_NBR_ADDR_59_58_OFFSET                              (0x6A4)
#define M154_NBR_ADDR_58                                        M154_NBR_ADDR_58
#define M154_NBR_ADDR_58_POS                                    (0U)
#define M154_NBR_ADDR_58_LEN                                    (16U)
#define M154_NBR_ADDR_58_MSK                                    (((1ULL<<M154_NBR_ADDR_58_LEN)-1)<<M154_NBR_ADDR_58_POS)
#define M154_NBR_ADDR_58_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_58_LEN)-1)<<M154_NBR_ADDR_58_POS))
#define M154_NBR_ADDR_59                                        M154_NBR_ADDR_59
#define M154_NBR_ADDR_59_POS                                    (16U)
#define M154_NBR_ADDR_59_LEN                                    (16U)
#define M154_NBR_ADDR_59_MSK                                    (((1ULL<<M154_NBR_ADDR_59_LEN)-1)<<M154_NBR_ADDR_59_POS)
#define M154_NBR_ADDR_59_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_59_LEN)-1)<<M154_NBR_ADDR_59_POS))

/* 0x6A8 : m154_nbr_addr_61_60 */
#define M154_NBR_ADDR_61_60_OFFSET                              (0x6A8)
#define M154_NBR_ADDR_60                                        M154_NBR_ADDR_60
#define M154_NBR_ADDR_60_POS                                    (0U)
#define M154_NBR_ADDR_60_LEN                                    (16U)
#define M154_NBR_ADDR_60_MSK                                    (((1ULL<<M154_NBR_ADDR_60_LEN)-1)<<M154_NBR_ADDR_60_POS)
#define M154_NBR_ADDR_60_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_60_LEN)-1)<<M154_NBR_ADDR_60_POS))
#define M154_NBR_ADDR_61                                        M154_NBR_ADDR_61
#define M154_NBR_ADDR_61_POS                                    (16U)
#define M154_NBR_ADDR_61_LEN                                    (16U)
#define M154_NBR_ADDR_61_MSK                                    (((1ULL<<M154_NBR_ADDR_61_LEN)-1)<<M154_NBR_ADDR_61_POS)
#define M154_NBR_ADDR_61_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_61_LEN)-1)<<M154_NBR_ADDR_61_POS))

/* 0x6AC : m154_nbr_addr_63_62 */
#define M154_NBR_ADDR_63_62_OFFSET                              (0x6AC)
#define M154_NBR_ADDR_62                                        M154_NBR_ADDR_62
#define M154_NBR_ADDR_62_POS                                    (0U)
#define M154_NBR_ADDR_62_LEN                                    (16U)
#define M154_NBR_ADDR_62_MSK                                    (((1ULL<<M154_NBR_ADDR_62_LEN)-1)<<M154_NBR_ADDR_62_POS)
#define M154_NBR_ADDR_62_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_62_LEN)-1)<<M154_NBR_ADDR_62_POS))
#define M154_NBR_ADDR_63                                        M154_NBR_ADDR_63
#define M154_NBR_ADDR_63_POS                                    (16U)
#define M154_NBR_ADDR_63_LEN                                    (16U)
#define M154_NBR_ADDR_63_MSK                                    (((1ULL<<M154_NBR_ADDR_63_LEN)-1)<<M154_NBR_ADDR_63_POS)
#define M154_NBR_ADDR_63_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_63_LEN)-1)<<M154_NBR_ADDR_63_POS))

/* 0x6B0 : m154_nbr_addr_65_64 */
#define M154_NBR_ADDR_65_64_OFFSET                              (0x6B0)
#define M154_NBR_ADDR_64                                        M154_NBR_ADDR_64
#define M154_NBR_ADDR_64_POS                                    (0U)
#define M154_NBR_ADDR_64_LEN                                    (16U)
#define M154_NBR_ADDR_64_MSK                                    (((1ULL<<M154_NBR_ADDR_64_LEN)-1)<<M154_NBR_ADDR_64_POS)
#define M154_NBR_ADDR_64_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_64_LEN)-1)<<M154_NBR_ADDR_64_POS))
#define M154_NBR_ADDR_65                                        M154_NBR_ADDR_65
#define M154_NBR_ADDR_65_POS                                    (16U)
#define M154_NBR_ADDR_65_LEN                                    (16U)
#define M154_NBR_ADDR_65_MSK                                    (((1ULL<<M154_NBR_ADDR_65_LEN)-1)<<M154_NBR_ADDR_65_POS)
#define M154_NBR_ADDR_65_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_65_LEN)-1)<<M154_NBR_ADDR_65_POS))

/* 0x6B4 : m154_nbr_addr_67_66 */
#define M154_NBR_ADDR_67_66_OFFSET                              (0x6B4)
#define M154_NBR_ADDR_66                                        M154_NBR_ADDR_66
#define M154_NBR_ADDR_66_POS                                    (0U)
#define M154_NBR_ADDR_66_LEN                                    (16U)
#define M154_NBR_ADDR_66_MSK                                    (((1ULL<<M154_NBR_ADDR_66_LEN)-1)<<M154_NBR_ADDR_66_POS)
#define M154_NBR_ADDR_66_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_66_LEN)-1)<<M154_NBR_ADDR_66_POS))
#define M154_NBR_ADDR_67                                        M154_NBR_ADDR_67
#define M154_NBR_ADDR_67_POS                                    (16U)
#define M154_NBR_ADDR_67_LEN                                    (16U)
#define M154_NBR_ADDR_67_MSK                                    (((1ULL<<M154_NBR_ADDR_67_LEN)-1)<<M154_NBR_ADDR_67_POS)
#define M154_NBR_ADDR_67_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_67_LEN)-1)<<M154_NBR_ADDR_67_POS))

/* 0x6B8 : m154_nbr_addr_69_68 */
#define M154_NBR_ADDR_69_68_OFFSET                              (0x6B8)
#define M154_NBR_ADDR_68                                        M154_NBR_ADDR_68
#define M154_NBR_ADDR_68_POS                                    (0U)
#define M154_NBR_ADDR_68_LEN                                    (16U)
#define M154_NBR_ADDR_68_MSK                                    (((1ULL<<M154_NBR_ADDR_68_LEN)-1)<<M154_NBR_ADDR_68_POS)
#define M154_NBR_ADDR_68_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_68_LEN)-1)<<M154_NBR_ADDR_68_POS))
#define M154_NBR_ADDR_69                                        M154_NBR_ADDR_69
#define M154_NBR_ADDR_69_POS                                    (16U)
#define M154_NBR_ADDR_69_LEN                                    (16U)
#define M154_NBR_ADDR_69_MSK                                    (((1ULL<<M154_NBR_ADDR_69_LEN)-1)<<M154_NBR_ADDR_69_POS)
#define M154_NBR_ADDR_69_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_69_LEN)-1)<<M154_NBR_ADDR_69_POS))

/* 0x6BC : m154_nbr_addr_71_70 */
#define M154_NBR_ADDR_71_70_OFFSET                              (0x6BC)
#define M154_NBR_ADDR_70                                        M154_NBR_ADDR_70
#define M154_NBR_ADDR_70_POS                                    (0U)
#define M154_NBR_ADDR_70_LEN                                    (16U)
#define M154_NBR_ADDR_70_MSK                                    (((1ULL<<M154_NBR_ADDR_70_LEN)-1)<<M154_NBR_ADDR_70_POS)
#define M154_NBR_ADDR_70_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_70_LEN)-1)<<M154_NBR_ADDR_70_POS))
#define M154_NBR_ADDR_71                                        M154_NBR_ADDR_71
#define M154_NBR_ADDR_71_POS                                    (16U)
#define M154_NBR_ADDR_71_LEN                                    (16U)
#define M154_NBR_ADDR_71_MSK                                    (((1ULL<<M154_NBR_ADDR_71_LEN)-1)<<M154_NBR_ADDR_71_POS)
#define M154_NBR_ADDR_71_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_71_LEN)-1)<<M154_NBR_ADDR_71_POS))

/* 0x6C0 : m154_nbr_addr_73_72 */
#define M154_NBR_ADDR_73_72_OFFSET                              (0x6C0)
#define M154_NBR_ADDR_72                                        M154_NBR_ADDR_72
#define M154_NBR_ADDR_72_POS                                    (0U)
#define M154_NBR_ADDR_72_LEN                                    (16U)
#define M154_NBR_ADDR_72_MSK                                    (((1ULL<<M154_NBR_ADDR_72_LEN)-1)<<M154_NBR_ADDR_72_POS)
#define M154_NBR_ADDR_72_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_72_LEN)-1)<<M154_NBR_ADDR_72_POS))
#define M154_NBR_ADDR_73                                        M154_NBR_ADDR_73
#define M154_NBR_ADDR_73_POS                                    (16U)
#define M154_NBR_ADDR_73_LEN                                    (16U)
#define M154_NBR_ADDR_73_MSK                                    (((1ULL<<M154_NBR_ADDR_73_LEN)-1)<<M154_NBR_ADDR_73_POS)
#define M154_NBR_ADDR_73_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_73_LEN)-1)<<M154_NBR_ADDR_73_POS))

/* 0x6C4 : m154_nbr_addr_75_74 */
#define M154_NBR_ADDR_75_74_OFFSET                              (0x6C4)
#define M154_NBR_ADDR_74                                        M154_NBR_ADDR_74
#define M154_NBR_ADDR_74_POS                                    (0U)
#define M154_NBR_ADDR_74_LEN                                    (16U)
#define M154_NBR_ADDR_74_MSK                                    (((1ULL<<M154_NBR_ADDR_74_LEN)-1)<<M154_NBR_ADDR_74_POS)
#define M154_NBR_ADDR_74_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_74_LEN)-1)<<M154_NBR_ADDR_74_POS))
#define M154_NBR_ADDR_75                                        M154_NBR_ADDR_75
#define M154_NBR_ADDR_75_POS                                    (16U)
#define M154_NBR_ADDR_75_LEN                                    (16U)
#define M154_NBR_ADDR_75_MSK                                    (((1ULL<<M154_NBR_ADDR_75_LEN)-1)<<M154_NBR_ADDR_75_POS)
#define M154_NBR_ADDR_75_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_75_LEN)-1)<<M154_NBR_ADDR_75_POS))

/* 0x6C8 : m154_nbr_addr_77_76 */
#define M154_NBR_ADDR_77_76_OFFSET                              (0x6C8)
#define M154_NBR_ADDR_76                                        M154_NBR_ADDR_76
#define M154_NBR_ADDR_76_POS                                    (0U)
#define M154_NBR_ADDR_76_LEN                                    (16U)
#define M154_NBR_ADDR_76_MSK                                    (((1ULL<<M154_NBR_ADDR_76_LEN)-1)<<M154_NBR_ADDR_76_POS)
#define M154_NBR_ADDR_76_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_76_LEN)-1)<<M154_NBR_ADDR_76_POS))
#define M154_NBR_ADDR_77                                        M154_NBR_ADDR_77
#define M154_NBR_ADDR_77_POS                                    (16U)
#define M154_NBR_ADDR_77_LEN                                    (16U)
#define M154_NBR_ADDR_77_MSK                                    (((1ULL<<M154_NBR_ADDR_77_LEN)-1)<<M154_NBR_ADDR_77_POS)
#define M154_NBR_ADDR_77_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_77_LEN)-1)<<M154_NBR_ADDR_77_POS))

/* 0x6CC : m154_nbr_addr_79_78 */
#define M154_NBR_ADDR_79_78_OFFSET                              (0x6CC)
#define M154_NBR_ADDR_78                                        M154_NBR_ADDR_78
#define M154_NBR_ADDR_78_POS                                    (0U)
#define M154_NBR_ADDR_78_LEN                                    (16U)
#define M154_NBR_ADDR_78_MSK                                    (((1ULL<<M154_NBR_ADDR_78_LEN)-1)<<M154_NBR_ADDR_78_POS)
#define M154_NBR_ADDR_78_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_78_LEN)-1)<<M154_NBR_ADDR_78_POS))
#define M154_NBR_ADDR_79                                        M154_NBR_ADDR_79
#define M154_NBR_ADDR_79_POS                                    (16U)
#define M154_NBR_ADDR_79_LEN                                    (16U)
#define M154_NBR_ADDR_79_MSK                                    (((1ULL<<M154_NBR_ADDR_79_LEN)-1)<<M154_NBR_ADDR_79_POS)
#define M154_NBR_ADDR_79_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_79_LEN)-1)<<M154_NBR_ADDR_79_POS))

/* 0x6D0 : m154_nbr_addr_81_80 */
#define M154_NBR_ADDR_81_80_OFFSET                              (0x6D0)
#define M154_NBR_ADDR_80                                        M154_NBR_ADDR_80
#define M154_NBR_ADDR_80_POS                                    (0U)
#define M154_NBR_ADDR_80_LEN                                    (16U)
#define M154_NBR_ADDR_80_MSK                                    (((1ULL<<M154_NBR_ADDR_80_LEN)-1)<<M154_NBR_ADDR_80_POS)
#define M154_NBR_ADDR_80_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_80_LEN)-1)<<M154_NBR_ADDR_80_POS))
#define M154_NBR_ADDR_81                                        M154_NBR_ADDR_81
#define M154_NBR_ADDR_81_POS                                    (16U)
#define M154_NBR_ADDR_81_LEN                                    (16U)
#define M154_NBR_ADDR_81_MSK                                    (((1ULL<<M154_NBR_ADDR_81_LEN)-1)<<M154_NBR_ADDR_81_POS)
#define M154_NBR_ADDR_81_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_81_LEN)-1)<<M154_NBR_ADDR_81_POS))

/* 0x6D4 : m154_nbr_addr_83_82 */
#define M154_NBR_ADDR_83_82_OFFSET                              (0x6D4)
#define M154_NBR_ADDR_82                                        M154_NBR_ADDR_82
#define M154_NBR_ADDR_82_POS                                    (0U)
#define M154_NBR_ADDR_82_LEN                                    (16U)
#define M154_NBR_ADDR_82_MSK                                    (((1ULL<<M154_NBR_ADDR_82_LEN)-1)<<M154_NBR_ADDR_82_POS)
#define M154_NBR_ADDR_82_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_82_LEN)-1)<<M154_NBR_ADDR_82_POS))
#define M154_NBR_ADDR_83                                        M154_NBR_ADDR_83
#define M154_NBR_ADDR_83_POS                                    (16U)
#define M154_NBR_ADDR_83_LEN                                    (16U)
#define M154_NBR_ADDR_83_MSK                                    (((1ULL<<M154_NBR_ADDR_83_LEN)-1)<<M154_NBR_ADDR_83_POS)
#define M154_NBR_ADDR_83_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_83_LEN)-1)<<M154_NBR_ADDR_83_POS))

/* 0x6D8 : m154_nbr_addr_85_84 */
#define M154_NBR_ADDR_85_84_OFFSET                              (0x6D8)
#define M154_NBR_ADDR_84                                        M154_NBR_ADDR_84
#define M154_NBR_ADDR_84_POS                                    (0U)
#define M154_NBR_ADDR_84_LEN                                    (16U)
#define M154_NBR_ADDR_84_MSK                                    (((1ULL<<M154_NBR_ADDR_84_LEN)-1)<<M154_NBR_ADDR_84_POS)
#define M154_NBR_ADDR_84_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_84_LEN)-1)<<M154_NBR_ADDR_84_POS))
#define M154_NBR_ADDR_85                                        M154_NBR_ADDR_85
#define M154_NBR_ADDR_85_POS                                    (16U)
#define M154_NBR_ADDR_85_LEN                                    (16U)
#define M154_NBR_ADDR_85_MSK                                    (((1ULL<<M154_NBR_ADDR_85_LEN)-1)<<M154_NBR_ADDR_85_POS)
#define M154_NBR_ADDR_85_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_85_LEN)-1)<<M154_NBR_ADDR_85_POS))

/* 0x6DC : m154_nbr_addr_87_86 */
#define M154_NBR_ADDR_87_86_OFFSET                              (0x6DC)
#define M154_NBR_ADDR_86                                        M154_NBR_ADDR_86
#define M154_NBR_ADDR_86_POS                                    (0U)
#define M154_NBR_ADDR_86_LEN                                    (16U)
#define M154_NBR_ADDR_86_MSK                                    (((1ULL<<M154_NBR_ADDR_86_LEN)-1)<<M154_NBR_ADDR_86_POS)
#define M154_NBR_ADDR_86_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_86_LEN)-1)<<M154_NBR_ADDR_86_POS))
#define M154_NBR_ADDR_87                                        M154_NBR_ADDR_87
#define M154_NBR_ADDR_87_POS                                    (16U)
#define M154_NBR_ADDR_87_LEN                                    (16U)
#define M154_NBR_ADDR_87_MSK                                    (((1ULL<<M154_NBR_ADDR_87_LEN)-1)<<M154_NBR_ADDR_87_POS)
#define M154_NBR_ADDR_87_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_87_LEN)-1)<<M154_NBR_ADDR_87_POS))

/* 0x6E0 : m154_nbr_addr_89_88 */
#define M154_NBR_ADDR_89_88_OFFSET                              (0x6E0)
#define M154_NBR_ADDR_88                                        M154_NBR_ADDR_88
#define M154_NBR_ADDR_88_POS                                    (0U)
#define M154_NBR_ADDR_88_LEN                                    (16U)
#define M154_NBR_ADDR_88_MSK                                    (((1ULL<<M154_NBR_ADDR_88_LEN)-1)<<M154_NBR_ADDR_88_POS)
#define M154_NBR_ADDR_88_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_88_LEN)-1)<<M154_NBR_ADDR_88_POS))
#define M154_NBR_ADDR_89                                        M154_NBR_ADDR_89
#define M154_NBR_ADDR_89_POS                                    (16U)
#define M154_NBR_ADDR_89_LEN                                    (16U)
#define M154_NBR_ADDR_89_MSK                                    (((1ULL<<M154_NBR_ADDR_89_LEN)-1)<<M154_NBR_ADDR_89_POS)
#define M154_NBR_ADDR_89_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_89_LEN)-1)<<M154_NBR_ADDR_89_POS))

/* 0x6E4 : m154_nbr_addr_91_90 */
#define M154_NBR_ADDR_91_90_OFFSET                              (0x6E4)
#define M154_NBR_ADDR_90                                        M154_NBR_ADDR_90
#define M154_NBR_ADDR_90_POS                                    (0U)
#define M154_NBR_ADDR_90_LEN                                    (16U)
#define M154_NBR_ADDR_90_MSK                                    (((1ULL<<M154_NBR_ADDR_90_LEN)-1)<<M154_NBR_ADDR_90_POS)
#define M154_NBR_ADDR_90_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_90_LEN)-1)<<M154_NBR_ADDR_90_POS))
#define M154_NBR_ADDR_91                                        M154_NBR_ADDR_91
#define M154_NBR_ADDR_91_POS                                    (16U)
#define M154_NBR_ADDR_91_LEN                                    (16U)
#define M154_NBR_ADDR_91_MSK                                    (((1ULL<<M154_NBR_ADDR_91_LEN)-1)<<M154_NBR_ADDR_91_POS)
#define M154_NBR_ADDR_91_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_91_LEN)-1)<<M154_NBR_ADDR_91_POS))

/* 0x6E8 : m154_nbr_addr_93_92 */
#define M154_NBR_ADDR_93_92_OFFSET                              (0x6E8)
#define M154_NBR_ADDR_92                                        M154_NBR_ADDR_92
#define M154_NBR_ADDR_92_POS                                    (0U)
#define M154_NBR_ADDR_92_LEN                                    (16U)
#define M154_NBR_ADDR_92_MSK                                    (((1ULL<<M154_NBR_ADDR_92_LEN)-1)<<M154_NBR_ADDR_92_POS)
#define M154_NBR_ADDR_92_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_92_LEN)-1)<<M154_NBR_ADDR_92_POS))
#define M154_NBR_ADDR_93                                        M154_NBR_ADDR_93
#define M154_NBR_ADDR_93_POS                                    (16U)
#define M154_NBR_ADDR_93_LEN                                    (16U)
#define M154_NBR_ADDR_93_MSK                                    (((1ULL<<M154_NBR_ADDR_93_LEN)-1)<<M154_NBR_ADDR_93_POS)
#define M154_NBR_ADDR_93_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_93_LEN)-1)<<M154_NBR_ADDR_93_POS))

/* 0x6EC : m154_nbr_addr_95_94 */
#define M154_NBR_ADDR_95_94_OFFSET                              (0x6EC)
#define M154_NBR_ADDR_94                                        M154_NBR_ADDR_94
#define M154_NBR_ADDR_94_POS                                    (0U)
#define M154_NBR_ADDR_94_LEN                                    (16U)
#define M154_NBR_ADDR_94_MSK                                    (((1ULL<<M154_NBR_ADDR_94_LEN)-1)<<M154_NBR_ADDR_94_POS)
#define M154_NBR_ADDR_94_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_94_LEN)-1)<<M154_NBR_ADDR_94_POS))
#define M154_NBR_ADDR_95                                        M154_NBR_ADDR_95
#define M154_NBR_ADDR_95_POS                                    (16U)
#define M154_NBR_ADDR_95_LEN                                    (16U)
#define M154_NBR_ADDR_95_MSK                                    (((1ULL<<M154_NBR_ADDR_95_LEN)-1)<<M154_NBR_ADDR_95_POS)
#define M154_NBR_ADDR_95_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_95_LEN)-1)<<M154_NBR_ADDR_95_POS))

/* 0x6F0 : m154_nbr_addr_97_96 */
#define M154_NBR_ADDR_97_96_OFFSET                              (0x6F0)
#define M154_NBR_ADDR_96                                        M154_NBR_ADDR_96
#define M154_NBR_ADDR_96_POS                                    (0U)
#define M154_NBR_ADDR_96_LEN                                    (16U)
#define M154_NBR_ADDR_96_MSK                                    (((1ULL<<M154_NBR_ADDR_96_LEN)-1)<<M154_NBR_ADDR_96_POS)
#define M154_NBR_ADDR_96_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_96_LEN)-1)<<M154_NBR_ADDR_96_POS))
#define M154_NBR_ADDR_97                                        M154_NBR_ADDR_97
#define M154_NBR_ADDR_97_POS                                    (16U)
#define M154_NBR_ADDR_97_LEN                                    (16U)
#define M154_NBR_ADDR_97_MSK                                    (((1ULL<<M154_NBR_ADDR_97_LEN)-1)<<M154_NBR_ADDR_97_POS)
#define M154_NBR_ADDR_97_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_97_LEN)-1)<<M154_NBR_ADDR_97_POS))

/* 0x6F4 : m154_nbr_addr_99_98 */
#define M154_NBR_ADDR_99_98_OFFSET                              (0x6F4)
#define M154_NBR_ADDR_98                                        M154_NBR_ADDR_98
#define M154_NBR_ADDR_98_POS                                    (0U)
#define M154_NBR_ADDR_98_LEN                                    (16U)
#define M154_NBR_ADDR_98_MSK                                    (((1ULL<<M154_NBR_ADDR_98_LEN)-1)<<M154_NBR_ADDR_98_POS)
#define M154_NBR_ADDR_98_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_98_LEN)-1)<<M154_NBR_ADDR_98_POS))
#define M154_NBR_ADDR_99                                        M154_NBR_ADDR_99
#define M154_NBR_ADDR_99_POS                                    (16U)
#define M154_NBR_ADDR_99_LEN                                    (16U)
#define M154_NBR_ADDR_99_MSK                                    (((1ULL<<M154_NBR_ADDR_99_LEN)-1)<<M154_NBR_ADDR_99_POS)
#define M154_NBR_ADDR_99_UMSK                                   (~(((1ULL<<M154_NBR_ADDR_99_LEN)-1)<<M154_NBR_ADDR_99_POS))

/* 0x6F8 : m154_nbr_addr_101_100 */
#define M154_NBR_ADDR_101_100_OFFSET                            (0x6F8)
#define M154_NBR_ADDR_100                                       M154_NBR_ADDR_100
#define M154_NBR_ADDR_100_POS                                   (0U)
#define M154_NBR_ADDR_100_LEN                                   (16U)
#define M154_NBR_ADDR_100_MSK                                   (((1ULL<<M154_NBR_ADDR_100_LEN)-1)<<M154_NBR_ADDR_100_POS)
#define M154_NBR_ADDR_100_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_100_LEN)-1)<<M154_NBR_ADDR_100_POS))
#define M154_NBR_ADDR_101                                       M154_NBR_ADDR_101
#define M154_NBR_ADDR_101_POS                                   (16U)
#define M154_NBR_ADDR_101_LEN                                   (16U)
#define M154_NBR_ADDR_101_MSK                                   (((1ULL<<M154_NBR_ADDR_101_LEN)-1)<<M154_NBR_ADDR_101_POS)
#define M154_NBR_ADDR_101_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_101_LEN)-1)<<M154_NBR_ADDR_101_POS))

/* 0x6FC : m154_nbr_addr_103_102 */
#define M154_NBR_ADDR_103_102_OFFSET                            (0x6FC)
#define M154_NBR_ADDR_102                                       M154_NBR_ADDR_102
#define M154_NBR_ADDR_102_POS                                   (0U)
#define M154_NBR_ADDR_102_LEN                                   (16U)
#define M154_NBR_ADDR_102_MSK                                   (((1ULL<<M154_NBR_ADDR_102_LEN)-1)<<M154_NBR_ADDR_102_POS)
#define M154_NBR_ADDR_102_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_102_LEN)-1)<<M154_NBR_ADDR_102_POS))
#define M154_NBR_ADDR_103                                       M154_NBR_ADDR_103
#define M154_NBR_ADDR_103_POS                                   (16U)
#define M154_NBR_ADDR_103_LEN                                   (16U)
#define M154_NBR_ADDR_103_MSK                                   (((1ULL<<M154_NBR_ADDR_103_LEN)-1)<<M154_NBR_ADDR_103_POS)
#define M154_NBR_ADDR_103_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_103_LEN)-1)<<M154_NBR_ADDR_103_POS))

/* 0x700 : m154_nbr_addr_105_104 */
#define M154_NBR_ADDR_105_104_OFFSET                            (0x700)
#define M154_NBR_ADDR_104                                       M154_NBR_ADDR_104
#define M154_NBR_ADDR_104_POS                                   (0U)
#define M154_NBR_ADDR_104_LEN                                   (16U)
#define M154_NBR_ADDR_104_MSK                                   (((1ULL<<M154_NBR_ADDR_104_LEN)-1)<<M154_NBR_ADDR_104_POS)
#define M154_NBR_ADDR_104_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_104_LEN)-1)<<M154_NBR_ADDR_104_POS))
#define M154_NBR_ADDR_105                                       M154_NBR_ADDR_105
#define M154_NBR_ADDR_105_POS                                   (16U)
#define M154_NBR_ADDR_105_LEN                                   (16U)
#define M154_NBR_ADDR_105_MSK                                   (((1ULL<<M154_NBR_ADDR_105_LEN)-1)<<M154_NBR_ADDR_105_POS)
#define M154_NBR_ADDR_105_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_105_LEN)-1)<<M154_NBR_ADDR_105_POS))

/* 0x704 : m154_nbr_addr_107_106 */
#define M154_NBR_ADDR_107_106_OFFSET                            (0x704)
#define M154_NBR_ADDR_106                                       M154_NBR_ADDR_106
#define M154_NBR_ADDR_106_POS                                   (0U)
#define M154_NBR_ADDR_106_LEN                                   (16U)
#define M154_NBR_ADDR_106_MSK                                   (((1ULL<<M154_NBR_ADDR_106_LEN)-1)<<M154_NBR_ADDR_106_POS)
#define M154_NBR_ADDR_106_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_106_LEN)-1)<<M154_NBR_ADDR_106_POS))
#define M154_NBR_ADDR_107                                       M154_NBR_ADDR_107
#define M154_NBR_ADDR_107_POS                                   (16U)
#define M154_NBR_ADDR_107_LEN                                   (16U)
#define M154_NBR_ADDR_107_MSK                                   (((1ULL<<M154_NBR_ADDR_107_LEN)-1)<<M154_NBR_ADDR_107_POS)
#define M154_NBR_ADDR_107_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_107_LEN)-1)<<M154_NBR_ADDR_107_POS))

/* 0x708 : m154_nbr_addr_109_108 */
#define M154_NBR_ADDR_109_108_OFFSET                            (0x708)
#define M154_NBR_ADDR_108                                       M154_NBR_ADDR_108
#define M154_NBR_ADDR_108_POS                                   (0U)
#define M154_NBR_ADDR_108_LEN                                   (16U)
#define M154_NBR_ADDR_108_MSK                                   (((1ULL<<M154_NBR_ADDR_108_LEN)-1)<<M154_NBR_ADDR_108_POS)
#define M154_NBR_ADDR_108_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_108_LEN)-1)<<M154_NBR_ADDR_108_POS))
#define M154_NBR_ADDR_109                                       M154_NBR_ADDR_109
#define M154_NBR_ADDR_109_POS                                   (16U)
#define M154_NBR_ADDR_109_LEN                                   (16U)
#define M154_NBR_ADDR_109_MSK                                   (((1ULL<<M154_NBR_ADDR_109_LEN)-1)<<M154_NBR_ADDR_109_POS)
#define M154_NBR_ADDR_109_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_109_LEN)-1)<<M154_NBR_ADDR_109_POS))

/* 0x70C : m154_nbr_addr_111_110 */
#define M154_NBR_ADDR_111_110_OFFSET                            (0x70C)
#define M154_NBR_ADDR_110                                       M154_NBR_ADDR_110
#define M154_NBR_ADDR_110_POS                                   (0U)
#define M154_NBR_ADDR_110_LEN                                   (16U)
#define M154_NBR_ADDR_110_MSK                                   (((1ULL<<M154_NBR_ADDR_110_LEN)-1)<<M154_NBR_ADDR_110_POS)
#define M154_NBR_ADDR_110_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_110_LEN)-1)<<M154_NBR_ADDR_110_POS))
#define M154_NBR_ADDR_111                                       M154_NBR_ADDR_111
#define M154_NBR_ADDR_111_POS                                   (16U)
#define M154_NBR_ADDR_111_LEN                                   (16U)
#define M154_NBR_ADDR_111_MSK                                   (((1ULL<<M154_NBR_ADDR_111_LEN)-1)<<M154_NBR_ADDR_111_POS)
#define M154_NBR_ADDR_111_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_111_LEN)-1)<<M154_NBR_ADDR_111_POS))

/* 0x710 : m154_nbr_addr_113_112 */
#define M154_NBR_ADDR_113_112_OFFSET                            (0x710)
#define M154_NBR_ADDR_112                                       M154_NBR_ADDR_112
#define M154_NBR_ADDR_112_POS                                   (0U)
#define M154_NBR_ADDR_112_LEN                                   (16U)
#define M154_NBR_ADDR_112_MSK                                   (((1ULL<<M154_NBR_ADDR_112_LEN)-1)<<M154_NBR_ADDR_112_POS)
#define M154_NBR_ADDR_112_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_112_LEN)-1)<<M154_NBR_ADDR_112_POS))
#define M154_NBR_ADDR_113                                       M154_NBR_ADDR_113
#define M154_NBR_ADDR_113_POS                                   (16U)
#define M154_NBR_ADDR_113_LEN                                   (16U)
#define M154_NBR_ADDR_113_MSK                                   (((1ULL<<M154_NBR_ADDR_113_LEN)-1)<<M154_NBR_ADDR_113_POS)
#define M154_NBR_ADDR_113_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_113_LEN)-1)<<M154_NBR_ADDR_113_POS))

/* 0x714 : m154_nbr_addr_115_114 */
#define M154_NBR_ADDR_115_114_OFFSET                            (0x714)
#define M154_NBR_ADDR_114                                       M154_NBR_ADDR_114
#define M154_NBR_ADDR_114_POS                                   (0U)
#define M154_NBR_ADDR_114_LEN                                   (16U)
#define M154_NBR_ADDR_114_MSK                                   (((1ULL<<M154_NBR_ADDR_114_LEN)-1)<<M154_NBR_ADDR_114_POS)
#define M154_NBR_ADDR_114_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_114_LEN)-1)<<M154_NBR_ADDR_114_POS))
#define M154_NBR_ADDR_115                                       M154_NBR_ADDR_115
#define M154_NBR_ADDR_115_POS                                   (16U)
#define M154_NBR_ADDR_115_LEN                                   (16U)
#define M154_NBR_ADDR_115_MSK                                   (((1ULL<<M154_NBR_ADDR_115_LEN)-1)<<M154_NBR_ADDR_115_POS)
#define M154_NBR_ADDR_115_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_115_LEN)-1)<<M154_NBR_ADDR_115_POS))

/* 0x718 : m154_nbr_addr_117_116 */
#define M154_NBR_ADDR_117_116_OFFSET                            (0x718)
#define M154_NBR_ADDR_116                                       M154_NBR_ADDR_116
#define M154_NBR_ADDR_116_POS                                   (0U)
#define M154_NBR_ADDR_116_LEN                                   (16U)
#define M154_NBR_ADDR_116_MSK                                   (((1ULL<<M154_NBR_ADDR_116_LEN)-1)<<M154_NBR_ADDR_116_POS)
#define M154_NBR_ADDR_116_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_116_LEN)-1)<<M154_NBR_ADDR_116_POS))
#define M154_NBR_ADDR_117                                       M154_NBR_ADDR_117
#define M154_NBR_ADDR_117_POS                                   (16U)
#define M154_NBR_ADDR_117_LEN                                   (16U)
#define M154_NBR_ADDR_117_MSK                                   (((1ULL<<M154_NBR_ADDR_117_LEN)-1)<<M154_NBR_ADDR_117_POS)
#define M154_NBR_ADDR_117_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_117_LEN)-1)<<M154_NBR_ADDR_117_POS))

/* 0x71C : m154_nbr_addr_119_118 */
#define M154_NBR_ADDR_119_118_OFFSET                            (0x71C)
#define M154_NBR_ADDR_118                                       M154_NBR_ADDR_118
#define M154_NBR_ADDR_118_POS                                   (0U)
#define M154_NBR_ADDR_118_LEN                                   (16U)
#define M154_NBR_ADDR_118_MSK                                   (((1ULL<<M154_NBR_ADDR_118_LEN)-1)<<M154_NBR_ADDR_118_POS)
#define M154_NBR_ADDR_118_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_118_LEN)-1)<<M154_NBR_ADDR_118_POS))
#define M154_NBR_ADDR_119                                       M154_NBR_ADDR_119
#define M154_NBR_ADDR_119_POS                                   (16U)
#define M154_NBR_ADDR_119_LEN                                   (16U)
#define M154_NBR_ADDR_119_MSK                                   (((1ULL<<M154_NBR_ADDR_119_LEN)-1)<<M154_NBR_ADDR_119_POS)
#define M154_NBR_ADDR_119_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_119_LEN)-1)<<M154_NBR_ADDR_119_POS))

/* 0x720 : m154_nbr_addr_121_120 */
#define M154_NBR_ADDR_121_120_OFFSET                            (0x720)
#define M154_NBR_ADDR_120                                       M154_NBR_ADDR_120
#define M154_NBR_ADDR_120_POS                                   (0U)
#define M154_NBR_ADDR_120_LEN                                   (16U)
#define M154_NBR_ADDR_120_MSK                                   (((1ULL<<M154_NBR_ADDR_120_LEN)-1)<<M154_NBR_ADDR_120_POS)
#define M154_NBR_ADDR_120_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_120_LEN)-1)<<M154_NBR_ADDR_120_POS))
#define M154_NBR_ADDR_121                                       M154_NBR_ADDR_121
#define M154_NBR_ADDR_121_POS                                   (16U)
#define M154_NBR_ADDR_121_LEN                                   (16U)
#define M154_NBR_ADDR_121_MSK                                   (((1ULL<<M154_NBR_ADDR_121_LEN)-1)<<M154_NBR_ADDR_121_POS)
#define M154_NBR_ADDR_121_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_121_LEN)-1)<<M154_NBR_ADDR_121_POS))

/* 0x724 : m154_nbr_addr_123_122 */
#define M154_NBR_ADDR_123_122_OFFSET                            (0x724)
#define M154_NBR_ADDR_122                                       M154_NBR_ADDR_122
#define M154_NBR_ADDR_122_POS                                   (0U)
#define M154_NBR_ADDR_122_LEN                                   (16U)
#define M154_NBR_ADDR_122_MSK                                   (((1ULL<<M154_NBR_ADDR_122_LEN)-1)<<M154_NBR_ADDR_122_POS)
#define M154_NBR_ADDR_122_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_122_LEN)-1)<<M154_NBR_ADDR_122_POS))
#define M154_NBR_ADDR_123                                       M154_NBR_ADDR_123
#define M154_NBR_ADDR_123_POS                                   (16U)
#define M154_NBR_ADDR_123_LEN                                   (16U)
#define M154_NBR_ADDR_123_MSK                                   (((1ULL<<M154_NBR_ADDR_123_LEN)-1)<<M154_NBR_ADDR_123_POS)
#define M154_NBR_ADDR_123_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_123_LEN)-1)<<M154_NBR_ADDR_123_POS))

/* 0x728 : m154_nbr_addr_125_124 */
#define M154_NBR_ADDR_125_124_OFFSET                            (0x728)
#define M154_NBR_ADDR_124                                       M154_NBR_ADDR_124
#define M154_NBR_ADDR_124_POS                                   (0U)
#define M154_NBR_ADDR_124_LEN                                   (16U)
#define M154_NBR_ADDR_124_MSK                                   (((1ULL<<M154_NBR_ADDR_124_LEN)-1)<<M154_NBR_ADDR_124_POS)
#define M154_NBR_ADDR_124_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_124_LEN)-1)<<M154_NBR_ADDR_124_POS))
#define M154_NBR_ADDR_125                                       M154_NBR_ADDR_125
#define M154_NBR_ADDR_125_POS                                   (16U)
#define M154_NBR_ADDR_125_LEN                                   (16U)
#define M154_NBR_ADDR_125_MSK                                   (((1ULL<<M154_NBR_ADDR_125_LEN)-1)<<M154_NBR_ADDR_125_POS)
#define M154_NBR_ADDR_125_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_125_LEN)-1)<<M154_NBR_ADDR_125_POS))

/* 0x72C : m154_nbr_addr_127_126 */
#define M154_NBR_ADDR_127_126_OFFSET                            (0x72C)
#define M154_NBR_ADDR_126                                       M154_NBR_ADDR_126
#define M154_NBR_ADDR_126_POS                                   (0U)
#define M154_NBR_ADDR_126_LEN                                   (16U)
#define M154_NBR_ADDR_126_MSK                                   (((1ULL<<M154_NBR_ADDR_126_LEN)-1)<<M154_NBR_ADDR_126_POS)
#define M154_NBR_ADDR_126_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_126_LEN)-1)<<M154_NBR_ADDR_126_POS))
#define M154_NBR_ADDR_127                                       M154_NBR_ADDR_127
#define M154_NBR_ADDR_127_POS                                   (16U)
#define M154_NBR_ADDR_127_LEN                                   (16U)
#define M154_NBR_ADDR_127_MSK                                   (((1ULL<<M154_NBR_ADDR_127_LEN)-1)<<M154_NBR_ADDR_127_POS)
#define M154_NBR_ADDR_127_UMSK                                  (~(((1ULL<<M154_NBR_ADDR_127_LEN)-1)<<M154_NBR_ADDR_127_POS))

/* 0x770 : m154_lb_mpdu_mismatch_31_00 */
#define M154_LB_MPDU_MISMATCH_31_00_OFFSET                      (0x770)
#define M154_LB_MISMATCH_31_DOWNTO_0                            M154_LB_MISMATCH_31_DOWNTO_0
#define M154_LB_MISMATCH_31_DOWNTO_0_POS                        (0U)
#define M154_LB_MISMATCH_31_DOWNTO_0_LEN                        (32U)
#define M154_LB_MISMATCH_31_DOWNTO_0_MSK                        (((1ULL<<M154_LB_MISMATCH_31_DOWNTO_0_LEN)-1)<<M154_LB_MISMATCH_31_DOWNTO_0_POS)
#define M154_LB_MISMATCH_31_DOWNTO_0_UMSK                       (~(((1ULL<<M154_LB_MISMATCH_31_DOWNTO_0_LEN)-1)<<M154_LB_MISMATCH_31_DOWNTO_0_POS))

/* 0x774 : m154_lb_mpdu_mismatch_63_32 */
#define M154_LB_MPDU_MISMATCH_63_32_OFFSET                      (0x774)
#define M154_LB_MISMATCH_63_DOWNTO_32                           M154_LB_MISMATCH_63_DOWNTO_32
#define M154_LB_MISMATCH_63_DOWNTO_32_POS                       (0U)
#define M154_LB_MISMATCH_63_DOWNTO_32_LEN                       (32U)
#define M154_LB_MISMATCH_63_DOWNTO_32_MSK                       (((1ULL<<M154_LB_MISMATCH_63_DOWNTO_32_LEN)-1)<<M154_LB_MISMATCH_63_DOWNTO_32_POS)
#define M154_LB_MISMATCH_63_DOWNTO_32_UMSK                      (~(((1ULL<<M154_LB_MISMATCH_63_DOWNTO_32_LEN)-1)<<M154_LB_MISMATCH_63_DOWNTO_32_POS))

/* 0x778 : m154_lb_mpdu_mismatch_95_64 */
#define M154_LB_MPDU_MISMATCH_95_64_OFFSET                      (0x778)
#define M154_LB_MISMATCH_95_DOWNTO_64                           M154_LB_MISMATCH_95_DOWNTO_64
#define M154_LB_MISMATCH_95_DOWNTO_64_POS                       (0U)
#define M154_LB_MISMATCH_95_DOWNTO_64_LEN                       (32U)
#define M154_LB_MISMATCH_95_DOWNTO_64_MSK                       (((1ULL<<M154_LB_MISMATCH_95_DOWNTO_64_LEN)-1)<<M154_LB_MISMATCH_95_DOWNTO_64_POS)
#define M154_LB_MISMATCH_95_DOWNTO_64_UMSK                      (~(((1ULL<<M154_LB_MISMATCH_95_DOWNTO_64_LEN)-1)<<M154_LB_MISMATCH_95_DOWNTO_64_POS))

/* 0x77C : m154_lb_mpdu_mismatch_127_96 */
#define M154_LB_MPDU_MISMATCH_127_96_OFFSET                     (0x77C)
#define M154_LB_MISMATCH_127_DOWNTO_96                          M154_LB_MISMATCH_127_DOWNTO_96
#define M154_LB_MISMATCH_127_DOWNTO_96_POS                      (0U)
#define M154_LB_MISMATCH_127_DOWNTO_96_LEN                      (32U)
#define M154_LB_MISMATCH_127_DOWNTO_96_MSK                      (((1ULL<<M154_LB_MISMATCH_127_DOWNTO_96_LEN)-1)<<M154_LB_MISMATCH_127_DOWNTO_96_POS)
#define M154_LB_MISMATCH_127_DOWNTO_96_UMSK                     (~(((1ULL<<M154_LB_MISMATCH_127_DOWNTO_96_LEN)-1)<<M154_LB_MISMATCH_127_DOWNTO_96_POS))

/* 0x780 : m154_lb_mpdu_00 */
#define M154_LB_MPDU_00_OFFSET                                  (0x780)
#define M154_LB_MPDU_000                                        M154_LB_MPDU_000
#define M154_LB_MPDU_000_POS                                    (0U)
#define M154_LB_MPDU_000_LEN                                    (8U)
#define M154_LB_MPDU_000_MSK                                    (((1ULL<<M154_LB_MPDU_000_LEN)-1)<<M154_LB_MPDU_000_POS)
#define M154_LB_MPDU_000_UMSK                                   (~(((1ULL<<M154_LB_MPDU_000_LEN)-1)<<M154_LB_MPDU_000_POS))
#define M154_LB_MPDU_001                                        M154_LB_MPDU_001
#define M154_LB_MPDU_001_POS                                    (8U)
#define M154_LB_MPDU_001_LEN                                    (8U)
#define M154_LB_MPDU_001_MSK                                    (((1ULL<<M154_LB_MPDU_001_LEN)-1)<<M154_LB_MPDU_001_POS)
#define M154_LB_MPDU_001_UMSK                                   (~(((1ULL<<M154_LB_MPDU_001_LEN)-1)<<M154_LB_MPDU_001_POS))
#define M154_LB_MPDU_002                                        M154_LB_MPDU_002
#define M154_LB_MPDU_002_POS                                    (16U)
#define M154_LB_MPDU_002_LEN                                    (8U)
#define M154_LB_MPDU_002_MSK                                    (((1ULL<<M154_LB_MPDU_002_LEN)-1)<<M154_LB_MPDU_002_POS)
#define M154_LB_MPDU_002_UMSK                                   (~(((1ULL<<M154_LB_MPDU_002_LEN)-1)<<M154_LB_MPDU_002_POS))
#define M154_LB_MPDU_003                                        M154_LB_MPDU_003
#define M154_LB_MPDU_003_POS                                    (24U)
#define M154_LB_MPDU_003_LEN                                    (8U)
#define M154_LB_MPDU_003_MSK                                    (((1ULL<<M154_LB_MPDU_003_LEN)-1)<<M154_LB_MPDU_003_POS)
#define M154_LB_MPDU_003_UMSK                                   (~(((1ULL<<M154_LB_MPDU_003_LEN)-1)<<M154_LB_MPDU_003_POS))

/* 0x784 : m154_lb_mpdu_01 */
#define M154_LB_MPDU_01_OFFSET                                  (0x784)
#define M154_LB_MPDU_004                                        M154_LB_MPDU_004
#define M154_LB_MPDU_004_POS                                    (0U)
#define M154_LB_MPDU_004_LEN                                    (8U)
#define M154_LB_MPDU_004_MSK                                    (((1ULL<<M154_LB_MPDU_004_LEN)-1)<<M154_LB_MPDU_004_POS)
#define M154_LB_MPDU_004_UMSK                                   (~(((1ULL<<M154_LB_MPDU_004_LEN)-1)<<M154_LB_MPDU_004_POS))
#define M154_LB_MPDU_005                                        M154_LB_MPDU_005
#define M154_LB_MPDU_005_POS                                    (8U)
#define M154_LB_MPDU_005_LEN                                    (8U)
#define M154_LB_MPDU_005_MSK                                    (((1ULL<<M154_LB_MPDU_005_LEN)-1)<<M154_LB_MPDU_005_POS)
#define M154_LB_MPDU_005_UMSK                                   (~(((1ULL<<M154_LB_MPDU_005_LEN)-1)<<M154_LB_MPDU_005_POS))
#define M154_LB_MPDU_006                                        M154_LB_MPDU_006
#define M154_LB_MPDU_006_POS                                    (16U)
#define M154_LB_MPDU_006_LEN                                    (8U)
#define M154_LB_MPDU_006_MSK                                    (((1ULL<<M154_LB_MPDU_006_LEN)-1)<<M154_LB_MPDU_006_POS)
#define M154_LB_MPDU_006_UMSK                                   (~(((1ULL<<M154_LB_MPDU_006_LEN)-1)<<M154_LB_MPDU_006_POS))
#define M154_LB_MPDU_007                                        M154_LB_MPDU_007
#define M154_LB_MPDU_007_POS                                    (24U)
#define M154_LB_MPDU_007_LEN                                    (8U)
#define M154_LB_MPDU_007_MSK                                    (((1ULL<<M154_LB_MPDU_007_LEN)-1)<<M154_LB_MPDU_007_POS)
#define M154_LB_MPDU_007_UMSK                                   (~(((1ULL<<M154_LB_MPDU_007_LEN)-1)<<M154_LB_MPDU_007_POS))

/* 0x788 : m154_lb_mpdu_02 */
#define M154_LB_MPDU_02_OFFSET                                  (0x788)
#define M154_LB_MPDU_008                                        M154_LB_MPDU_008
#define M154_LB_MPDU_008_POS                                    (0U)
#define M154_LB_MPDU_008_LEN                                    (8U)
#define M154_LB_MPDU_008_MSK                                    (((1ULL<<M154_LB_MPDU_008_LEN)-1)<<M154_LB_MPDU_008_POS)
#define M154_LB_MPDU_008_UMSK                                   (~(((1ULL<<M154_LB_MPDU_008_LEN)-1)<<M154_LB_MPDU_008_POS))
#define M154_LB_MPDU_009                                        M154_LB_MPDU_009
#define M154_LB_MPDU_009_POS                                    (8U)
#define M154_LB_MPDU_009_LEN                                    (8U)
#define M154_LB_MPDU_009_MSK                                    (((1ULL<<M154_LB_MPDU_009_LEN)-1)<<M154_LB_MPDU_009_POS)
#define M154_LB_MPDU_009_UMSK                                   (~(((1ULL<<M154_LB_MPDU_009_LEN)-1)<<M154_LB_MPDU_009_POS))
#define M154_LB_MPDU_010                                        M154_LB_MPDU_010
#define M154_LB_MPDU_010_POS                                    (16U)
#define M154_LB_MPDU_010_LEN                                    (8U)
#define M154_LB_MPDU_010_MSK                                    (((1ULL<<M154_LB_MPDU_010_LEN)-1)<<M154_LB_MPDU_010_POS)
#define M154_LB_MPDU_010_UMSK                                   (~(((1ULL<<M154_LB_MPDU_010_LEN)-1)<<M154_LB_MPDU_010_POS))
#define M154_LB_MPDU_011                                        M154_LB_MPDU_011
#define M154_LB_MPDU_011_POS                                    (24U)
#define M154_LB_MPDU_011_LEN                                    (8U)
#define M154_LB_MPDU_011_MSK                                    (((1ULL<<M154_LB_MPDU_011_LEN)-1)<<M154_LB_MPDU_011_POS)
#define M154_LB_MPDU_011_UMSK                                   (~(((1ULL<<M154_LB_MPDU_011_LEN)-1)<<M154_LB_MPDU_011_POS))

/* 0x78C : m154_lb_mpdu_03 */
#define M154_LB_MPDU_03_OFFSET                                  (0x78C)
#define M154_LB_MPDU_012                                        M154_LB_MPDU_012
#define M154_LB_MPDU_012_POS                                    (0U)
#define M154_LB_MPDU_012_LEN                                    (8U)
#define M154_LB_MPDU_012_MSK                                    (((1ULL<<M154_LB_MPDU_012_LEN)-1)<<M154_LB_MPDU_012_POS)
#define M154_LB_MPDU_012_UMSK                                   (~(((1ULL<<M154_LB_MPDU_012_LEN)-1)<<M154_LB_MPDU_012_POS))
#define M154_LB_MPDU_013                                        M154_LB_MPDU_013
#define M154_LB_MPDU_013_POS                                    (8U)
#define M154_LB_MPDU_013_LEN                                    (8U)
#define M154_LB_MPDU_013_MSK                                    (((1ULL<<M154_LB_MPDU_013_LEN)-1)<<M154_LB_MPDU_013_POS)
#define M154_LB_MPDU_013_UMSK                                   (~(((1ULL<<M154_LB_MPDU_013_LEN)-1)<<M154_LB_MPDU_013_POS))
#define M154_LB_MPDU_014                                        M154_LB_MPDU_014
#define M154_LB_MPDU_014_POS                                    (16U)
#define M154_LB_MPDU_014_LEN                                    (8U)
#define M154_LB_MPDU_014_MSK                                    (((1ULL<<M154_LB_MPDU_014_LEN)-1)<<M154_LB_MPDU_014_POS)
#define M154_LB_MPDU_014_UMSK                                   (~(((1ULL<<M154_LB_MPDU_014_LEN)-1)<<M154_LB_MPDU_014_POS))
#define M154_LB_MPDU_015                                        M154_LB_MPDU_015
#define M154_LB_MPDU_015_POS                                    (24U)
#define M154_LB_MPDU_015_LEN                                    (8U)
#define M154_LB_MPDU_015_MSK                                    (((1ULL<<M154_LB_MPDU_015_LEN)-1)<<M154_LB_MPDU_015_POS)
#define M154_LB_MPDU_015_UMSK                                   (~(((1ULL<<M154_LB_MPDU_015_LEN)-1)<<M154_LB_MPDU_015_POS))

/* 0x790 : m154_lb_mpdu_04 */
#define M154_LB_MPDU_04_OFFSET                                  (0x790)
#define M154_LB_MPDU_016                                        M154_LB_MPDU_016
#define M154_LB_MPDU_016_POS                                    (0U)
#define M154_LB_MPDU_016_LEN                                    (8U)
#define M154_LB_MPDU_016_MSK                                    (((1ULL<<M154_LB_MPDU_016_LEN)-1)<<M154_LB_MPDU_016_POS)
#define M154_LB_MPDU_016_UMSK                                   (~(((1ULL<<M154_LB_MPDU_016_LEN)-1)<<M154_LB_MPDU_016_POS))
#define M154_LB_MPDU_017                                        M154_LB_MPDU_017
#define M154_LB_MPDU_017_POS                                    (8U)
#define M154_LB_MPDU_017_LEN                                    (8U)
#define M154_LB_MPDU_017_MSK                                    (((1ULL<<M154_LB_MPDU_017_LEN)-1)<<M154_LB_MPDU_017_POS)
#define M154_LB_MPDU_017_UMSK                                   (~(((1ULL<<M154_LB_MPDU_017_LEN)-1)<<M154_LB_MPDU_017_POS))
#define M154_LB_MPDU_018                                        M154_LB_MPDU_018
#define M154_LB_MPDU_018_POS                                    (16U)
#define M154_LB_MPDU_018_LEN                                    (8U)
#define M154_LB_MPDU_018_MSK                                    (((1ULL<<M154_LB_MPDU_018_LEN)-1)<<M154_LB_MPDU_018_POS)
#define M154_LB_MPDU_018_UMSK                                   (~(((1ULL<<M154_LB_MPDU_018_LEN)-1)<<M154_LB_MPDU_018_POS))
#define M154_LB_MPDU_019                                        M154_LB_MPDU_019
#define M154_LB_MPDU_019_POS                                    (24U)
#define M154_LB_MPDU_019_LEN                                    (8U)
#define M154_LB_MPDU_019_MSK                                    (((1ULL<<M154_LB_MPDU_019_LEN)-1)<<M154_LB_MPDU_019_POS)
#define M154_LB_MPDU_019_UMSK                                   (~(((1ULL<<M154_LB_MPDU_019_LEN)-1)<<M154_LB_MPDU_019_POS))

/* 0x794 : m154_lb_mpdu_05 */
#define M154_LB_MPDU_05_OFFSET                                  (0x794)
#define M154_LB_MPDU_020                                        M154_LB_MPDU_020
#define M154_LB_MPDU_020_POS                                    (0U)
#define M154_LB_MPDU_020_LEN                                    (8U)
#define M154_LB_MPDU_020_MSK                                    (((1ULL<<M154_LB_MPDU_020_LEN)-1)<<M154_LB_MPDU_020_POS)
#define M154_LB_MPDU_020_UMSK                                   (~(((1ULL<<M154_LB_MPDU_020_LEN)-1)<<M154_LB_MPDU_020_POS))
#define M154_LB_MPDU_021                                        M154_LB_MPDU_021
#define M154_LB_MPDU_021_POS                                    (8U)
#define M154_LB_MPDU_021_LEN                                    (8U)
#define M154_LB_MPDU_021_MSK                                    (((1ULL<<M154_LB_MPDU_021_LEN)-1)<<M154_LB_MPDU_021_POS)
#define M154_LB_MPDU_021_UMSK                                   (~(((1ULL<<M154_LB_MPDU_021_LEN)-1)<<M154_LB_MPDU_021_POS))
#define M154_LB_MPDU_022                                        M154_LB_MPDU_022
#define M154_LB_MPDU_022_POS                                    (16U)
#define M154_LB_MPDU_022_LEN                                    (8U)
#define M154_LB_MPDU_022_MSK                                    (((1ULL<<M154_LB_MPDU_022_LEN)-1)<<M154_LB_MPDU_022_POS)
#define M154_LB_MPDU_022_UMSK                                   (~(((1ULL<<M154_LB_MPDU_022_LEN)-1)<<M154_LB_MPDU_022_POS))
#define M154_LB_MPDU_023                                        M154_LB_MPDU_023
#define M154_LB_MPDU_023_POS                                    (24U)
#define M154_LB_MPDU_023_LEN                                    (8U)
#define M154_LB_MPDU_023_MSK                                    (((1ULL<<M154_LB_MPDU_023_LEN)-1)<<M154_LB_MPDU_023_POS)
#define M154_LB_MPDU_023_UMSK                                   (~(((1ULL<<M154_LB_MPDU_023_LEN)-1)<<M154_LB_MPDU_023_POS))

/* 0x798 : m154_lb_mpdu_06 */
#define M154_LB_MPDU_06_OFFSET                                  (0x798)
#define M154_LB_MPDU_024                                        M154_LB_MPDU_024
#define M154_LB_MPDU_024_POS                                    (0U)
#define M154_LB_MPDU_024_LEN                                    (8U)
#define M154_LB_MPDU_024_MSK                                    (((1ULL<<M154_LB_MPDU_024_LEN)-1)<<M154_LB_MPDU_024_POS)
#define M154_LB_MPDU_024_UMSK                                   (~(((1ULL<<M154_LB_MPDU_024_LEN)-1)<<M154_LB_MPDU_024_POS))
#define M154_LB_MPDU_025                                        M154_LB_MPDU_025
#define M154_LB_MPDU_025_POS                                    (8U)
#define M154_LB_MPDU_025_LEN                                    (8U)
#define M154_LB_MPDU_025_MSK                                    (((1ULL<<M154_LB_MPDU_025_LEN)-1)<<M154_LB_MPDU_025_POS)
#define M154_LB_MPDU_025_UMSK                                   (~(((1ULL<<M154_LB_MPDU_025_LEN)-1)<<M154_LB_MPDU_025_POS))
#define M154_LB_MPDU_026                                        M154_LB_MPDU_026
#define M154_LB_MPDU_026_POS                                    (16U)
#define M154_LB_MPDU_026_LEN                                    (8U)
#define M154_LB_MPDU_026_MSK                                    (((1ULL<<M154_LB_MPDU_026_LEN)-1)<<M154_LB_MPDU_026_POS)
#define M154_LB_MPDU_026_UMSK                                   (~(((1ULL<<M154_LB_MPDU_026_LEN)-1)<<M154_LB_MPDU_026_POS))
#define M154_LB_MPDU_027                                        M154_LB_MPDU_027
#define M154_LB_MPDU_027_POS                                    (24U)
#define M154_LB_MPDU_027_LEN                                    (8U)
#define M154_LB_MPDU_027_MSK                                    (((1ULL<<M154_LB_MPDU_027_LEN)-1)<<M154_LB_MPDU_027_POS)
#define M154_LB_MPDU_027_UMSK                                   (~(((1ULL<<M154_LB_MPDU_027_LEN)-1)<<M154_LB_MPDU_027_POS))

/* 0x79C : m154_lb_mpdu_07 */
#define M154_LB_MPDU_07_OFFSET                                  (0x79C)
#define M154_LB_MPDU_028                                        M154_LB_MPDU_028
#define M154_LB_MPDU_028_POS                                    (0U)
#define M154_LB_MPDU_028_LEN                                    (8U)
#define M154_LB_MPDU_028_MSK                                    (((1ULL<<M154_LB_MPDU_028_LEN)-1)<<M154_LB_MPDU_028_POS)
#define M154_LB_MPDU_028_UMSK                                   (~(((1ULL<<M154_LB_MPDU_028_LEN)-1)<<M154_LB_MPDU_028_POS))
#define M154_LB_MPDU_029                                        M154_LB_MPDU_029
#define M154_LB_MPDU_029_POS                                    (8U)
#define M154_LB_MPDU_029_LEN                                    (8U)
#define M154_LB_MPDU_029_MSK                                    (((1ULL<<M154_LB_MPDU_029_LEN)-1)<<M154_LB_MPDU_029_POS)
#define M154_LB_MPDU_029_UMSK                                   (~(((1ULL<<M154_LB_MPDU_029_LEN)-1)<<M154_LB_MPDU_029_POS))
#define M154_LB_MPDU_030                                        M154_LB_MPDU_030
#define M154_LB_MPDU_030_POS                                    (16U)
#define M154_LB_MPDU_030_LEN                                    (8U)
#define M154_LB_MPDU_030_MSK                                    (((1ULL<<M154_LB_MPDU_030_LEN)-1)<<M154_LB_MPDU_030_POS)
#define M154_LB_MPDU_030_UMSK                                   (~(((1ULL<<M154_LB_MPDU_030_LEN)-1)<<M154_LB_MPDU_030_POS))
#define M154_LB_MPDU_031                                        M154_LB_MPDU_031
#define M154_LB_MPDU_031_POS                                    (24U)
#define M154_LB_MPDU_031_LEN                                    (8U)
#define M154_LB_MPDU_031_MSK                                    (((1ULL<<M154_LB_MPDU_031_LEN)-1)<<M154_LB_MPDU_031_POS)
#define M154_LB_MPDU_031_UMSK                                   (~(((1ULL<<M154_LB_MPDU_031_LEN)-1)<<M154_LB_MPDU_031_POS))

/* 0x7A0 : m154_lb_mpdu_08 */
#define M154_LB_MPDU_08_OFFSET                                  (0x7A0)
#define M154_LB_MPDU_032                                        M154_LB_MPDU_032
#define M154_LB_MPDU_032_POS                                    (0U)
#define M154_LB_MPDU_032_LEN                                    (8U)
#define M154_LB_MPDU_032_MSK                                    (((1ULL<<M154_LB_MPDU_032_LEN)-1)<<M154_LB_MPDU_032_POS)
#define M154_LB_MPDU_032_UMSK                                   (~(((1ULL<<M154_LB_MPDU_032_LEN)-1)<<M154_LB_MPDU_032_POS))
#define M154_LB_MPDU_033                                        M154_LB_MPDU_033
#define M154_LB_MPDU_033_POS                                    (8U)
#define M154_LB_MPDU_033_LEN                                    (8U)
#define M154_LB_MPDU_033_MSK                                    (((1ULL<<M154_LB_MPDU_033_LEN)-1)<<M154_LB_MPDU_033_POS)
#define M154_LB_MPDU_033_UMSK                                   (~(((1ULL<<M154_LB_MPDU_033_LEN)-1)<<M154_LB_MPDU_033_POS))
#define M154_LB_MPDU_034                                        M154_LB_MPDU_034
#define M154_LB_MPDU_034_POS                                    (16U)
#define M154_LB_MPDU_034_LEN                                    (8U)
#define M154_LB_MPDU_034_MSK                                    (((1ULL<<M154_LB_MPDU_034_LEN)-1)<<M154_LB_MPDU_034_POS)
#define M154_LB_MPDU_034_UMSK                                   (~(((1ULL<<M154_LB_MPDU_034_LEN)-1)<<M154_LB_MPDU_034_POS))
#define M154_LB_MPDU_035                                        M154_LB_MPDU_035
#define M154_LB_MPDU_035_POS                                    (24U)
#define M154_LB_MPDU_035_LEN                                    (8U)
#define M154_LB_MPDU_035_MSK                                    (((1ULL<<M154_LB_MPDU_035_LEN)-1)<<M154_LB_MPDU_035_POS)
#define M154_LB_MPDU_035_UMSK                                   (~(((1ULL<<M154_LB_MPDU_035_LEN)-1)<<M154_LB_MPDU_035_POS))

/* 0x7A4 : m154_lb_mpdu_09 */
#define M154_LB_MPDU_09_OFFSET                                  (0x7A4)
#define M154_LB_MPDU_036                                        M154_LB_MPDU_036
#define M154_LB_MPDU_036_POS                                    (0U)
#define M154_LB_MPDU_036_LEN                                    (8U)
#define M154_LB_MPDU_036_MSK                                    (((1ULL<<M154_LB_MPDU_036_LEN)-1)<<M154_LB_MPDU_036_POS)
#define M154_LB_MPDU_036_UMSK                                   (~(((1ULL<<M154_LB_MPDU_036_LEN)-1)<<M154_LB_MPDU_036_POS))
#define M154_LB_MPDU_037                                        M154_LB_MPDU_037
#define M154_LB_MPDU_037_POS                                    (8U)
#define M154_LB_MPDU_037_LEN                                    (8U)
#define M154_LB_MPDU_037_MSK                                    (((1ULL<<M154_LB_MPDU_037_LEN)-1)<<M154_LB_MPDU_037_POS)
#define M154_LB_MPDU_037_UMSK                                   (~(((1ULL<<M154_LB_MPDU_037_LEN)-1)<<M154_LB_MPDU_037_POS))
#define M154_LB_MPDU_038                                        M154_LB_MPDU_038
#define M154_LB_MPDU_038_POS                                    (16U)
#define M154_LB_MPDU_038_LEN                                    (8U)
#define M154_LB_MPDU_038_MSK                                    (((1ULL<<M154_LB_MPDU_038_LEN)-1)<<M154_LB_MPDU_038_POS)
#define M154_LB_MPDU_038_UMSK                                   (~(((1ULL<<M154_LB_MPDU_038_LEN)-1)<<M154_LB_MPDU_038_POS))
#define M154_LB_MPDU_039                                        M154_LB_MPDU_039
#define M154_LB_MPDU_039_POS                                    (24U)
#define M154_LB_MPDU_039_LEN                                    (8U)
#define M154_LB_MPDU_039_MSK                                    (((1ULL<<M154_LB_MPDU_039_LEN)-1)<<M154_LB_MPDU_039_POS)
#define M154_LB_MPDU_039_UMSK                                   (~(((1ULL<<M154_LB_MPDU_039_LEN)-1)<<M154_LB_MPDU_039_POS))

/* 0x7A8 : m154_lb_mpdu_10 */
#define M154_LB_MPDU_10_OFFSET                                  (0x7A8)
#define M154_LB_MPDU_040                                        M154_LB_MPDU_040
#define M154_LB_MPDU_040_POS                                    (0U)
#define M154_LB_MPDU_040_LEN                                    (8U)
#define M154_LB_MPDU_040_MSK                                    (((1ULL<<M154_LB_MPDU_040_LEN)-1)<<M154_LB_MPDU_040_POS)
#define M154_LB_MPDU_040_UMSK                                   (~(((1ULL<<M154_LB_MPDU_040_LEN)-1)<<M154_LB_MPDU_040_POS))
#define M154_LB_MPDU_041                                        M154_LB_MPDU_041
#define M154_LB_MPDU_041_POS                                    (8U)
#define M154_LB_MPDU_041_LEN                                    (8U)
#define M154_LB_MPDU_041_MSK                                    (((1ULL<<M154_LB_MPDU_041_LEN)-1)<<M154_LB_MPDU_041_POS)
#define M154_LB_MPDU_041_UMSK                                   (~(((1ULL<<M154_LB_MPDU_041_LEN)-1)<<M154_LB_MPDU_041_POS))
#define M154_LB_MPDU_042                                        M154_LB_MPDU_042
#define M154_LB_MPDU_042_POS                                    (16U)
#define M154_LB_MPDU_042_LEN                                    (8U)
#define M154_LB_MPDU_042_MSK                                    (((1ULL<<M154_LB_MPDU_042_LEN)-1)<<M154_LB_MPDU_042_POS)
#define M154_LB_MPDU_042_UMSK                                   (~(((1ULL<<M154_LB_MPDU_042_LEN)-1)<<M154_LB_MPDU_042_POS))
#define M154_LB_MPDU_043                                        M154_LB_MPDU_043
#define M154_LB_MPDU_043_POS                                    (24U)
#define M154_LB_MPDU_043_LEN                                    (8U)
#define M154_LB_MPDU_043_MSK                                    (((1ULL<<M154_LB_MPDU_043_LEN)-1)<<M154_LB_MPDU_043_POS)
#define M154_LB_MPDU_043_UMSK                                   (~(((1ULL<<M154_LB_MPDU_043_LEN)-1)<<M154_LB_MPDU_043_POS))

/* 0x7AC : m154_lb_mpdu_11 */
#define M154_LB_MPDU_11_OFFSET                                  (0x7AC)
#define M154_LB_MPDU_044                                        M154_LB_MPDU_044
#define M154_LB_MPDU_044_POS                                    (0U)
#define M154_LB_MPDU_044_LEN                                    (8U)
#define M154_LB_MPDU_044_MSK                                    (((1ULL<<M154_LB_MPDU_044_LEN)-1)<<M154_LB_MPDU_044_POS)
#define M154_LB_MPDU_044_UMSK                                   (~(((1ULL<<M154_LB_MPDU_044_LEN)-1)<<M154_LB_MPDU_044_POS))
#define M154_LB_MPDU_045                                        M154_LB_MPDU_045
#define M154_LB_MPDU_045_POS                                    (8U)
#define M154_LB_MPDU_045_LEN                                    (8U)
#define M154_LB_MPDU_045_MSK                                    (((1ULL<<M154_LB_MPDU_045_LEN)-1)<<M154_LB_MPDU_045_POS)
#define M154_LB_MPDU_045_UMSK                                   (~(((1ULL<<M154_LB_MPDU_045_LEN)-1)<<M154_LB_MPDU_045_POS))
#define M154_LB_MPDU_046                                        M154_LB_MPDU_046
#define M154_LB_MPDU_046_POS                                    (16U)
#define M154_LB_MPDU_046_LEN                                    (8U)
#define M154_LB_MPDU_046_MSK                                    (((1ULL<<M154_LB_MPDU_046_LEN)-1)<<M154_LB_MPDU_046_POS)
#define M154_LB_MPDU_046_UMSK                                   (~(((1ULL<<M154_LB_MPDU_046_LEN)-1)<<M154_LB_MPDU_046_POS))
#define M154_LB_MPDU_047                                        M154_LB_MPDU_047
#define M154_LB_MPDU_047_POS                                    (24U)
#define M154_LB_MPDU_047_LEN                                    (8U)
#define M154_LB_MPDU_047_MSK                                    (((1ULL<<M154_LB_MPDU_047_LEN)-1)<<M154_LB_MPDU_047_POS)
#define M154_LB_MPDU_047_UMSK                                   (~(((1ULL<<M154_LB_MPDU_047_LEN)-1)<<M154_LB_MPDU_047_POS))

/* 0x7B0 : m154_lb_mpdu_12 */
#define M154_LB_MPDU_12_OFFSET                                  (0x7B0)
#define M154_LB_MPDU_048                                        M154_LB_MPDU_048
#define M154_LB_MPDU_048_POS                                    (0U)
#define M154_LB_MPDU_048_LEN                                    (8U)
#define M154_LB_MPDU_048_MSK                                    (((1ULL<<M154_LB_MPDU_048_LEN)-1)<<M154_LB_MPDU_048_POS)
#define M154_LB_MPDU_048_UMSK                                   (~(((1ULL<<M154_LB_MPDU_048_LEN)-1)<<M154_LB_MPDU_048_POS))
#define M154_LB_MPDU_049                                        M154_LB_MPDU_049
#define M154_LB_MPDU_049_POS                                    (8U)
#define M154_LB_MPDU_049_LEN                                    (8U)
#define M154_LB_MPDU_049_MSK                                    (((1ULL<<M154_LB_MPDU_049_LEN)-1)<<M154_LB_MPDU_049_POS)
#define M154_LB_MPDU_049_UMSK                                   (~(((1ULL<<M154_LB_MPDU_049_LEN)-1)<<M154_LB_MPDU_049_POS))
#define M154_LB_MPDU_050                                        M154_LB_MPDU_050
#define M154_LB_MPDU_050_POS                                    (16U)
#define M154_LB_MPDU_050_LEN                                    (8U)
#define M154_LB_MPDU_050_MSK                                    (((1ULL<<M154_LB_MPDU_050_LEN)-1)<<M154_LB_MPDU_050_POS)
#define M154_LB_MPDU_050_UMSK                                   (~(((1ULL<<M154_LB_MPDU_050_LEN)-1)<<M154_LB_MPDU_050_POS))
#define M154_LB_MPDU_051                                        M154_LB_MPDU_051
#define M154_LB_MPDU_051_POS                                    (24U)
#define M154_LB_MPDU_051_LEN                                    (8U)
#define M154_LB_MPDU_051_MSK                                    (((1ULL<<M154_LB_MPDU_051_LEN)-1)<<M154_LB_MPDU_051_POS)
#define M154_LB_MPDU_051_UMSK                                   (~(((1ULL<<M154_LB_MPDU_051_LEN)-1)<<M154_LB_MPDU_051_POS))

/* 0x7B4 : m154_lb_mpdu_13 */
#define M154_LB_MPDU_13_OFFSET                                  (0x7B4)
#define M154_LB_MPDU_052                                        M154_LB_MPDU_052
#define M154_LB_MPDU_052_POS                                    (0U)
#define M154_LB_MPDU_052_LEN                                    (8U)
#define M154_LB_MPDU_052_MSK                                    (((1ULL<<M154_LB_MPDU_052_LEN)-1)<<M154_LB_MPDU_052_POS)
#define M154_LB_MPDU_052_UMSK                                   (~(((1ULL<<M154_LB_MPDU_052_LEN)-1)<<M154_LB_MPDU_052_POS))
#define M154_LB_MPDU_053                                        M154_LB_MPDU_053
#define M154_LB_MPDU_053_POS                                    (8U)
#define M154_LB_MPDU_053_LEN                                    (8U)
#define M154_LB_MPDU_053_MSK                                    (((1ULL<<M154_LB_MPDU_053_LEN)-1)<<M154_LB_MPDU_053_POS)
#define M154_LB_MPDU_053_UMSK                                   (~(((1ULL<<M154_LB_MPDU_053_LEN)-1)<<M154_LB_MPDU_053_POS))
#define M154_LB_MPDU_054                                        M154_LB_MPDU_054
#define M154_LB_MPDU_054_POS                                    (16U)
#define M154_LB_MPDU_054_LEN                                    (8U)
#define M154_LB_MPDU_054_MSK                                    (((1ULL<<M154_LB_MPDU_054_LEN)-1)<<M154_LB_MPDU_054_POS)
#define M154_LB_MPDU_054_UMSK                                   (~(((1ULL<<M154_LB_MPDU_054_LEN)-1)<<M154_LB_MPDU_054_POS))
#define M154_LB_MPDU_055                                        M154_LB_MPDU_055
#define M154_LB_MPDU_055_POS                                    (24U)
#define M154_LB_MPDU_055_LEN                                    (8U)
#define M154_LB_MPDU_055_MSK                                    (((1ULL<<M154_LB_MPDU_055_LEN)-1)<<M154_LB_MPDU_055_POS)
#define M154_LB_MPDU_055_UMSK                                   (~(((1ULL<<M154_LB_MPDU_055_LEN)-1)<<M154_LB_MPDU_055_POS))

/* 0x7B8 : m154_lb_mpdu_14 */
#define M154_LB_MPDU_14_OFFSET                                  (0x7B8)
#define M154_LB_MPDU_056                                        M154_LB_MPDU_056
#define M154_LB_MPDU_056_POS                                    (0U)
#define M154_LB_MPDU_056_LEN                                    (8U)
#define M154_LB_MPDU_056_MSK                                    (((1ULL<<M154_LB_MPDU_056_LEN)-1)<<M154_LB_MPDU_056_POS)
#define M154_LB_MPDU_056_UMSK                                   (~(((1ULL<<M154_LB_MPDU_056_LEN)-1)<<M154_LB_MPDU_056_POS))
#define M154_LB_MPDU_057                                        M154_LB_MPDU_057
#define M154_LB_MPDU_057_POS                                    (8U)
#define M154_LB_MPDU_057_LEN                                    (8U)
#define M154_LB_MPDU_057_MSK                                    (((1ULL<<M154_LB_MPDU_057_LEN)-1)<<M154_LB_MPDU_057_POS)
#define M154_LB_MPDU_057_UMSK                                   (~(((1ULL<<M154_LB_MPDU_057_LEN)-1)<<M154_LB_MPDU_057_POS))
#define M154_LB_MPDU_058                                        M154_LB_MPDU_058
#define M154_LB_MPDU_058_POS                                    (16U)
#define M154_LB_MPDU_058_LEN                                    (8U)
#define M154_LB_MPDU_058_MSK                                    (((1ULL<<M154_LB_MPDU_058_LEN)-1)<<M154_LB_MPDU_058_POS)
#define M154_LB_MPDU_058_UMSK                                   (~(((1ULL<<M154_LB_MPDU_058_LEN)-1)<<M154_LB_MPDU_058_POS))
#define M154_LB_MPDU_059                                        M154_LB_MPDU_059
#define M154_LB_MPDU_059_POS                                    (24U)
#define M154_LB_MPDU_059_LEN                                    (8U)
#define M154_LB_MPDU_059_MSK                                    (((1ULL<<M154_LB_MPDU_059_LEN)-1)<<M154_LB_MPDU_059_POS)
#define M154_LB_MPDU_059_UMSK                                   (~(((1ULL<<M154_LB_MPDU_059_LEN)-1)<<M154_LB_MPDU_059_POS))

/* 0x7BC : m154_lb_mpdu_15 */
#define M154_LB_MPDU_15_OFFSET                                  (0x7BC)
#define M154_LB_MPDU_060                                        M154_LB_MPDU_060
#define M154_LB_MPDU_060_POS                                    (0U)
#define M154_LB_MPDU_060_LEN                                    (8U)
#define M154_LB_MPDU_060_MSK                                    (((1ULL<<M154_LB_MPDU_060_LEN)-1)<<M154_LB_MPDU_060_POS)
#define M154_LB_MPDU_060_UMSK                                   (~(((1ULL<<M154_LB_MPDU_060_LEN)-1)<<M154_LB_MPDU_060_POS))
#define M154_LB_MPDU_061                                        M154_LB_MPDU_061
#define M154_LB_MPDU_061_POS                                    (8U)
#define M154_LB_MPDU_061_LEN                                    (8U)
#define M154_LB_MPDU_061_MSK                                    (((1ULL<<M154_LB_MPDU_061_LEN)-1)<<M154_LB_MPDU_061_POS)
#define M154_LB_MPDU_061_UMSK                                   (~(((1ULL<<M154_LB_MPDU_061_LEN)-1)<<M154_LB_MPDU_061_POS))
#define M154_LB_MPDU_062                                        M154_LB_MPDU_062
#define M154_LB_MPDU_062_POS                                    (16U)
#define M154_LB_MPDU_062_LEN                                    (8U)
#define M154_LB_MPDU_062_MSK                                    (((1ULL<<M154_LB_MPDU_062_LEN)-1)<<M154_LB_MPDU_062_POS)
#define M154_LB_MPDU_062_UMSK                                   (~(((1ULL<<M154_LB_MPDU_062_LEN)-1)<<M154_LB_MPDU_062_POS))
#define M154_LB_MPDU_063                                        M154_LB_MPDU_063
#define M154_LB_MPDU_063_POS                                    (24U)
#define M154_LB_MPDU_063_LEN                                    (8U)
#define M154_LB_MPDU_063_MSK                                    (((1ULL<<M154_LB_MPDU_063_LEN)-1)<<M154_LB_MPDU_063_POS)
#define M154_LB_MPDU_063_UMSK                                   (~(((1ULL<<M154_LB_MPDU_063_LEN)-1)<<M154_LB_MPDU_063_POS))

/* 0x7C0 : m154_lb_mpdu_16 */
#define M154_LB_MPDU_16_OFFSET                                  (0x7C0)
#define M154_LB_MPDU_064                                        M154_LB_MPDU_064
#define M154_LB_MPDU_064_POS                                    (0U)
#define M154_LB_MPDU_064_LEN                                    (8U)
#define M154_LB_MPDU_064_MSK                                    (((1ULL<<M154_LB_MPDU_064_LEN)-1)<<M154_LB_MPDU_064_POS)
#define M154_LB_MPDU_064_UMSK                                   (~(((1ULL<<M154_LB_MPDU_064_LEN)-1)<<M154_LB_MPDU_064_POS))
#define M154_LB_MPDU_065                                        M154_LB_MPDU_065
#define M154_LB_MPDU_065_POS                                    (8U)
#define M154_LB_MPDU_065_LEN                                    (8U)
#define M154_LB_MPDU_065_MSK                                    (((1ULL<<M154_LB_MPDU_065_LEN)-1)<<M154_LB_MPDU_065_POS)
#define M154_LB_MPDU_065_UMSK                                   (~(((1ULL<<M154_LB_MPDU_065_LEN)-1)<<M154_LB_MPDU_065_POS))
#define M154_LB_MPDU_066                                        M154_LB_MPDU_066
#define M154_LB_MPDU_066_POS                                    (16U)
#define M154_LB_MPDU_066_LEN                                    (8U)
#define M154_LB_MPDU_066_MSK                                    (((1ULL<<M154_LB_MPDU_066_LEN)-1)<<M154_LB_MPDU_066_POS)
#define M154_LB_MPDU_066_UMSK                                   (~(((1ULL<<M154_LB_MPDU_066_LEN)-1)<<M154_LB_MPDU_066_POS))
#define M154_LB_MPDU_067                                        M154_LB_MPDU_067
#define M154_LB_MPDU_067_POS                                    (24U)
#define M154_LB_MPDU_067_LEN                                    (8U)
#define M154_LB_MPDU_067_MSK                                    (((1ULL<<M154_LB_MPDU_067_LEN)-1)<<M154_LB_MPDU_067_POS)
#define M154_LB_MPDU_067_UMSK                                   (~(((1ULL<<M154_LB_MPDU_067_LEN)-1)<<M154_LB_MPDU_067_POS))

/* 0x7C4 : m154_lb_mpdu_17 */
#define M154_LB_MPDU_17_OFFSET                                  (0x7C4)
#define M154_LB_MPDU_068                                        M154_LB_MPDU_068
#define M154_LB_MPDU_068_POS                                    (0U)
#define M154_LB_MPDU_068_LEN                                    (8U)
#define M154_LB_MPDU_068_MSK                                    (((1ULL<<M154_LB_MPDU_068_LEN)-1)<<M154_LB_MPDU_068_POS)
#define M154_LB_MPDU_068_UMSK                                   (~(((1ULL<<M154_LB_MPDU_068_LEN)-1)<<M154_LB_MPDU_068_POS))
#define M154_LB_MPDU_069                                        M154_LB_MPDU_069
#define M154_LB_MPDU_069_POS                                    (8U)
#define M154_LB_MPDU_069_LEN                                    (8U)
#define M154_LB_MPDU_069_MSK                                    (((1ULL<<M154_LB_MPDU_069_LEN)-1)<<M154_LB_MPDU_069_POS)
#define M154_LB_MPDU_069_UMSK                                   (~(((1ULL<<M154_LB_MPDU_069_LEN)-1)<<M154_LB_MPDU_069_POS))
#define M154_LB_MPDU_070                                        M154_LB_MPDU_070
#define M154_LB_MPDU_070_POS                                    (16U)
#define M154_LB_MPDU_070_LEN                                    (8U)
#define M154_LB_MPDU_070_MSK                                    (((1ULL<<M154_LB_MPDU_070_LEN)-1)<<M154_LB_MPDU_070_POS)
#define M154_LB_MPDU_070_UMSK                                   (~(((1ULL<<M154_LB_MPDU_070_LEN)-1)<<M154_LB_MPDU_070_POS))
#define M154_LB_MPDU_071                                        M154_LB_MPDU_071
#define M154_LB_MPDU_071_POS                                    (24U)
#define M154_LB_MPDU_071_LEN                                    (8U)
#define M154_LB_MPDU_071_MSK                                    (((1ULL<<M154_LB_MPDU_071_LEN)-1)<<M154_LB_MPDU_071_POS)
#define M154_LB_MPDU_071_UMSK                                   (~(((1ULL<<M154_LB_MPDU_071_LEN)-1)<<M154_LB_MPDU_071_POS))

/* 0x7C8 : m154_lb_mpdu_18 */
#define M154_LB_MPDU_18_OFFSET                                  (0x7C8)
#define M154_LB_MPDU_072                                        M154_LB_MPDU_072
#define M154_LB_MPDU_072_POS                                    (0U)
#define M154_LB_MPDU_072_LEN                                    (8U)
#define M154_LB_MPDU_072_MSK                                    (((1ULL<<M154_LB_MPDU_072_LEN)-1)<<M154_LB_MPDU_072_POS)
#define M154_LB_MPDU_072_UMSK                                   (~(((1ULL<<M154_LB_MPDU_072_LEN)-1)<<M154_LB_MPDU_072_POS))
#define M154_LB_MPDU_073                                        M154_LB_MPDU_073
#define M154_LB_MPDU_073_POS                                    (8U)
#define M154_LB_MPDU_073_LEN                                    (8U)
#define M154_LB_MPDU_073_MSK                                    (((1ULL<<M154_LB_MPDU_073_LEN)-1)<<M154_LB_MPDU_073_POS)
#define M154_LB_MPDU_073_UMSK                                   (~(((1ULL<<M154_LB_MPDU_073_LEN)-1)<<M154_LB_MPDU_073_POS))
#define M154_LB_MPDU_074                                        M154_LB_MPDU_074
#define M154_LB_MPDU_074_POS                                    (16U)
#define M154_LB_MPDU_074_LEN                                    (8U)
#define M154_LB_MPDU_074_MSK                                    (((1ULL<<M154_LB_MPDU_074_LEN)-1)<<M154_LB_MPDU_074_POS)
#define M154_LB_MPDU_074_UMSK                                   (~(((1ULL<<M154_LB_MPDU_074_LEN)-1)<<M154_LB_MPDU_074_POS))
#define M154_LB_MPDU_075                                        M154_LB_MPDU_075
#define M154_LB_MPDU_075_POS                                    (24U)
#define M154_LB_MPDU_075_LEN                                    (8U)
#define M154_LB_MPDU_075_MSK                                    (((1ULL<<M154_LB_MPDU_075_LEN)-1)<<M154_LB_MPDU_075_POS)
#define M154_LB_MPDU_075_UMSK                                   (~(((1ULL<<M154_LB_MPDU_075_LEN)-1)<<M154_LB_MPDU_075_POS))

/* 0x7CC : m154_lb_mpdu_19 */
#define M154_LB_MPDU_19_OFFSET                                  (0x7CC)
#define M154_LB_MPDU_076                                        M154_LB_MPDU_076
#define M154_LB_MPDU_076_POS                                    (0U)
#define M154_LB_MPDU_076_LEN                                    (8U)
#define M154_LB_MPDU_076_MSK                                    (((1ULL<<M154_LB_MPDU_076_LEN)-1)<<M154_LB_MPDU_076_POS)
#define M154_LB_MPDU_076_UMSK                                   (~(((1ULL<<M154_LB_MPDU_076_LEN)-1)<<M154_LB_MPDU_076_POS))
#define M154_LB_MPDU_077                                        M154_LB_MPDU_077
#define M154_LB_MPDU_077_POS                                    (8U)
#define M154_LB_MPDU_077_LEN                                    (8U)
#define M154_LB_MPDU_077_MSK                                    (((1ULL<<M154_LB_MPDU_077_LEN)-1)<<M154_LB_MPDU_077_POS)
#define M154_LB_MPDU_077_UMSK                                   (~(((1ULL<<M154_LB_MPDU_077_LEN)-1)<<M154_LB_MPDU_077_POS))
#define M154_LB_MPDU_078                                        M154_LB_MPDU_078
#define M154_LB_MPDU_078_POS                                    (16U)
#define M154_LB_MPDU_078_LEN                                    (8U)
#define M154_LB_MPDU_078_MSK                                    (((1ULL<<M154_LB_MPDU_078_LEN)-1)<<M154_LB_MPDU_078_POS)
#define M154_LB_MPDU_078_UMSK                                   (~(((1ULL<<M154_LB_MPDU_078_LEN)-1)<<M154_LB_MPDU_078_POS))
#define M154_LB_MPDU_079                                        M154_LB_MPDU_079
#define M154_LB_MPDU_079_POS                                    (24U)
#define M154_LB_MPDU_079_LEN                                    (8U)
#define M154_LB_MPDU_079_MSK                                    (((1ULL<<M154_LB_MPDU_079_LEN)-1)<<M154_LB_MPDU_079_POS)
#define M154_LB_MPDU_079_UMSK                                   (~(((1ULL<<M154_LB_MPDU_079_LEN)-1)<<M154_LB_MPDU_079_POS))

/* 0x7D0 : m154_lb_mpdu_20 */
#define M154_LB_MPDU_20_OFFSET                                  (0x7D0)
#define M154_LB_MPDU_080                                        M154_LB_MPDU_080
#define M154_LB_MPDU_080_POS                                    (0U)
#define M154_LB_MPDU_080_LEN                                    (8U)
#define M154_LB_MPDU_080_MSK                                    (((1ULL<<M154_LB_MPDU_080_LEN)-1)<<M154_LB_MPDU_080_POS)
#define M154_LB_MPDU_080_UMSK                                   (~(((1ULL<<M154_LB_MPDU_080_LEN)-1)<<M154_LB_MPDU_080_POS))
#define M154_LB_MPDU_081                                        M154_LB_MPDU_081
#define M154_LB_MPDU_081_POS                                    (8U)
#define M154_LB_MPDU_081_LEN                                    (8U)
#define M154_LB_MPDU_081_MSK                                    (((1ULL<<M154_LB_MPDU_081_LEN)-1)<<M154_LB_MPDU_081_POS)
#define M154_LB_MPDU_081_UMSK                                   (~(((1ULL<<M154_LB_MPDU_081_LEN)-1)<<M154_LB_MPDU_081_POS))
#define M154_LB_MPDU_082                                        M154_LB_MPDU_082
#define M154_LB_MPDU_082_POS                                    (16U)
#define M154_LB_MPDU_082_LEN                                    (8U)
#define M154_LB_MPDU_082_MSK                                    (((1ULL<<M154_LB_MPDU_082_LEN)-1)<<M154_LB_MPDU_082_POS)
#define M154_LB_MPDU_082_UMSK                                   (~(((1ULL<<M154_LB_MPDU_082_LEN)-1)<<M154_LB_MPDU_082_POS))
#define M154_LB_MPDU_083                                        M154_LB_MPDU_083
#define M154_LB_MPDU_083_POS                                    (24U)
#define M154_LB_MPDU_083_LEN                                    (8U)
#define M154_LB_MPDU_083_MSK                                    (((1ULL<<M154_LB_MPDU_083_LEN)-1)<<M154_LB_MPDU_083_POS)
#define M154_LB_MPDU_083_UMSK                                   (~(((1ULL<<M154_LB_MPDU_083_LEN)-1)<<M154_LB_MPDU_083_POS))

/* 0x7D4 : m154_lb_mpdu_21 */
#define M154_LB_MPDU_21_OFFSET                                  (0x7D4)
#define M154_LB_MPDU_084                                        M154_LB_MPDU_084
#define M154_LB_MPDU_084_POS                                    (0U)
#define M154_LB_MPDU_084_LEN                                    (8U)
#define M154_LB_MPDU_084_MSK                                    (((1ULL<<M154_LB_MPDU_084_LEN)-1)<<M154_LB_MPDU_084_POS)
#define M154_LB_MPDU_084_UMSK                                   (~(((1ULL<<M154_LB_MPDU_084_LEN)-1)<<M154_LB_MPDU_084_POS))
#define M154_LB_MPDU_085                                        M154_LB_MPDU_085
#define M154_LB_MPDU_085_POS                                    (8U)
#define M154_LB_MPDU_085_LEN                                    (8U)
#define M154_LB_MPDU_085_MSK                                    (((1ULL<<M154_LB_MPDU_085_LEN)-1)<<M154_LB_MPDU_085_POS)
#define M154_LB_MPDU_085_UMSK                                   (~(((1ULL<<M154_LB_MPDU_085_LEN)-1)<<M154_LB_MPDU_085_POS))
#define M154_LB_MPDU_086                                        M154_LB_MPDU_086
#define M154_LB_MPDU_086_POS                                    (16U)
#define M154_LB_MPDU_086_LEN                                    (8U)
#define M154_LB_MPDU_086_MSK                                    (((1ULL<<M154_LB_MPDU_086_LEN)-1)<<M154_LB_MPDU_086_POS)
#define M154_LB_MPDU_086_UMSK                                   (~(((1ULL<<M154_LB_MPDU_086_LEN)-1)<<M154_LB_MPDU_086_POS))
#define M154_LB_MPDU_087                                        M154_LB_MPDU_087
#define M154_LB_MPDU_087_POS                                    (24U)
#define M154_LB_MPDU_087_LEN                                    (8U)
#define M154_LB_MPDU_087_MSK                                    (((1ULL<<M154_LB_MPDU_087_LEN)-1)<<M154_LB_MPDU_087_POS)
#define M154_LB_MPDU_087_UMSK                                   (~(((1ULL<<M154_LB_MPDU_087_LEN)-1)<<M154_LB_MPDU_087_POS))

/* 0x7D8 : m154_lb_mpdu_22 */
#define M154_LB_MPDU_22_OFFSET                                  (0x7D8)
#define M154_LB_MPDU_088                                        M154_LB_MPDU_088
#define M154_LB_MPDU_088_POS                                    (0U)
#define M154_LB_MPDU_088_LEN                                    (8U)
#define M154_LB_MPDU_088_MSK                                    (((1ULL<<M154_LB_MPDU_088_LEN)-1)<<M154_LB_MPDU_088_POS)
#define M154_LB_MPDU_088_UMSK                                   (~(((1ULL<<M154_LB_MPDU_088_LEN)-1)<<M154_LB_MPDU_088_POS))
#define M154_LB_MPDU_089                                        M154_LB_MPDU_089
#define M154_LB_MPDU_089_POS                                    (8U)
#define M154_LB_MPDU_089_LEN                                    (8U)
#define M154_LB_MPDU_089_MSK                                    (((1ULL<<M154_LB_MPDU_089_LEN)-1)<<M154_LB_MPDU_089_POS)
#define M154_LB_MPDU_089_UMSK                                   (~(((1ULL<<M154_LB_MPDU_089_LEN)-1)<<M154_LB_MPDU_089_POS))
#define M154_LB_MPDU_090                                        M154_LB_MPDU_090
#define M154_LB_MPDU_090_POS                                    (16U)
#define M154_LB_MPDU_090_LEN                                    (8U)
#define M154_LB_MPDU_090_MSK                                    (((1ULL<<M154_LB_MPDU_090_LEN)-1)<<M154_LB_MPDU_090_POS)
#define M154_LB_MPDU_090_UMSK                                   (~(((1ULL<<M154_LB_MPDU_090_LEN)-1)<<M154_LB_MPDU_090_POS))
#define M154_LB_MPDU_091                                        M154_LB_MPDU_091
#define M154_LB_MPDU_091_POS                                    (24U)
#define M154_LB_MPDU_091_LEN                                    (8U)
#define M154_LB_MPDU_091_MSK                                    (((1ULL<<M154_LB_MPDU_091_LEN)-1)<<M154_LB_MPDU_091_POS)
#define M154_LB_MPDU_091_UMSK                                   (~(((1ULL<<M154_LB_MPDU_091_LEN)-1)<<M154_LB_MPDU_091_POS))

/* 0x7DC : m154_lb_mpdu_23 */
#define M154_LB_MPDU_23_OFFSET                                  (0x7DC)
#define M154_LB_MPDU_092                                        M154_LB_MPDU_092
#define M154_LB_MPDU_092_POS                                    (0U)
#define M154_LB_MPDU_092_LEN                                    (8U)
#define M154_LB_MPDU_092_MSK                                    (((1ULL<<M154_LB_MPDU_092_LEN)-1)<<M154_LB_MPDU_092_POS)
#define M154_LB_MPDU_092_UMSK                                   (~(((1ULL<<M154_LB_MPDU_092_LEN)-1)<<M154_LB_MPDU_092_POS))
#define M154_LB_MPDU_093                                        M154_LB_MPDU_093
#define M154_LB_MPDU_093_POS                                    (8U)
#define M154_LB_MPDU_093_LEN                                    (8U)
#define M154_LB_MPDU_093_MSK                                    (((1ULL<<M154_LB_MPDU_093_LEN)-1)<<M154_LB_MPDU_093_POS)
#define M154_LB_MPDU_093_UMSK                                   (~(((1ULL<<M154_LB_MPDU_093_LEN)-1)<<M154_LB_MPDU_093_POS))
#define M154_LB_MPDU_094                                        M154_LB_MPDU_094
#define M154_LB_MPDU_094_POS                                    (16U)
#define M154_LB_MPDU_094_LEN                                    (8U)
#define M154_LB_MPDU_094_MSK                                    (((1ULL<<M154_LB_MPDU_094_LEN)-1)<<M154_LB_MPDU_094_POS)
#define M154_LB_MPDU_094_UMSK                                   (~(((1ULL<<M154_LB_MPDU_094_LEN)-1)<<M154_LB_MPDU_094_POS))
#define M154_LB_MPDU_095                                        M154_LB_MPDU_095
#define M154_LB_MPDU_095_POS                                    (24U)
#define M154_LB_MPDU_095_LEN                                    (8U)
#define M154_LB_MPDU_095_MSK                                    (((1ULL<<M154_LB_MPDU_095_LEN)-1)<<M154_LB_MPDU_095_POS)
#define M154_LB_MPDU_095_UMSK                                   (~(((1ULL<<M154_LB_MPDU_095_LEN)-1)<<M154_LB_MPDU_095_POS))

/* 0x7E0 : m154_lb_mpdu_24 */
#define M154_LB_MPDU_24_OFFSET                                  (0x7E0)
#define M154_LB_MPDU_096                                        M154_LB_MPDU_096
#define M154_LB_MPDU_096_POS                                    (0U)
#define M154_LB_MPDU_096_LEN                                    (8U)
#define M154_LB_MPDU_096_MSK                                    (((1ULL<<M154_LB_MPDU_096_LEN)-1)<<M154_LB_MPDU_096_POS)
#define M154_LB_MPDU_096_UMSK                                   (~(((1ULL<<M154_LB_MPDU_096_LEN)-1)<<M154_LB_MPDU_096_POS))
#define M154_LB_MPDU_097                                        M154_LB_MPDU_097
#define M154_LB_MPDU_097_POS                                    (8U)
#define M154_LB_MPDU_097_LEN                                    (8U)
#define M154_LB_MPDU_097_MSK                                    (((1ULL<<M154_LB_MPDU_097_LEN)-1)<<M154_LB_MPDU_097_POS)
#define M154_LB_MPDU_097_UMSK                                   (~(((1ULL<<M154_LB_MPDU_097_LEN)-1)<<M154_LB_MPDU_097_POS))
#define M154_LB_MPDU_098                                        M154_LB_MPDU_098
#define M154_LB_MPDU_098_POS                                    (16U)
#define M154_LB_MPDU_098_LEN                                    (8U)
#define M154_LB_MPDU_098_MSK                                    (((1ULL<<M154_LB_MPDU_098_LEN)-1)<<M154_LB_MPDU_098_POS)
#define M154_LB_MPDU_098_UMSK                                   (~(((1ULL<<M154_LB_MPDU_098_LEN)-1)<<M154_LB_MPDU_098_POS))
#define M154_LB_MPDU_099                                        M154_LB_MPDU_099
#define M154_LB_MPDU_099_POS                                    (24U)
#define M154_LB_MPDU_099_LEN                                    (8U)
#define M154_LB_MPDU_099_MSK                                    (((1ULL<<M154_LB_MPDU_099_LEN)-1)<<M154_LB_MPDU_099_POS)
#define M154_LB_MPDU_099_UMSK                                   (~(((1ULL<<M154_LB_MPDU_099_LEN)-1)<<M154_LB_MPDU_099_POS))

/* 0x7E4 : m154_lb_mpdu_25 */
#define M154_LB_MPDU_25_OFFSET                                  (0x7E4)
#define M154_LB_MPDU_100                                        M154_LB_MPDU_100
#define M154_LB_MPDU_100_POS                                    (0U)
#define M154_LB_MPDU_100_LEN                                    (8U)
#define M154_LB_MPDU_100_MSK                                    (((1ULL<<M154_LB_MPDU_100_LEN)-1)<<M154_LB_MPDU_100_POS)
#define M154_LB_MPDU_100_UMSK                                   (~(((1ULL<<M154_LB_MPDU_100_LEN)-1)<<M154_LB_MPDU_100_POS))
#define M154_LB_MPDU_101                                        M154_LB_MPDU_101
#define M154_LB_MPDU_101_POS                                    (8U)
#define M154_LB_MPDU_101_LEN                                    (8U)
#define M154_LB_MPDU_101_MSK                                    (((1ULL<<M154_LB_MPDU_101_LEN)-1)<<M154_LB_MPDU_101_POS)
#define M154_LB_MPDU_101_UMSK                                   (~(((1ULL<<M154_LB_MPDU_101_LEN)-1)<<M154_LB_MPDU_101_POS))
#define M154_LB_MPDU_102                                        M154_LB_MPDU_102
#define M154_LB_MPDU_102_POS                                    (16U)
#define M154_LB_MPDU_102_LEN                                    (8U)
#define M154_LB_MPDU_102_MSK                                    (((1ULL<<M154_LB_MPDU_102_LEN)-1)<<M154_LB_MPDU_102_POS)
#define M154_LB_MPDU_102_UMSK                                   (~(((1ULL<<M154_LB_MPDU_102_LEN)-1)<<M154_LB_MPDU_102_POS))
#define M154_LB_MPDU_103                                        M154_LB_MPDU_103
#define M154_LB_MPDU_103_POS                                    (24U)
#define M154_LB_MPDU_103_LEN                                    (8U)
#define M154_LB_MPDU_103_MSK                                    (((1ULL<<M154_LB_MPDU_103_LEN)-1)<<M154_LB_MPDU_103_POS)
#define M154_LB_MPDU_103_UMSK                                   (~(((1ULL<<M154_LB_MPDU_103_LEN)-1)<<M154_LB_MPDU_103_POS))

/* 0x7E8 : m154_lb_mpdu_26 */
#define M154_LB_MPDU_26_OFFSET                                  (0x7E8)
#define M154_LB_MPDU_104                                        M154_LB_MPDU_104
#define M154_LB_MPDU_104_POS                                    (0U)
#define M154_LB_MPDU_104_LEN                                    (8U)
#define M154_LB_MPDU_104_MSK                                    (((1ULL<<M154_LB_MPDU_104_LEN)-1)<<M154_LB_MPDU_104_POS)
#define M154_LB_MPDU_104_UMSK                                   (~(((1ULL<<M154_LB_MPDU_104_LEN)-1)<<M154_LB_MPDU_104_POS))
#define M154_LB_MPDU_105                                        M154_LB_MPDU_105
#define M154_LB_MPDU_105_POS                                    (8U)
#define M154_LB_MPDU_105_LEN                                    (8U)
#define M154_LB_MPDU_105_MSK                                    (((1ULL<<M154_LB_MPDU_105_LEN)-1)<<M154_LB_MPDU_105_POS)
#define M154_LB_MPDU_105_UMSK                                   (~(((1ULL<<M154_LB_MPDU_105_LEN)-1)<<M154_LB_MPDU_105_POS))
#define M154_LB_MPDU_106                                        M154_LB_MPDU_106
#define M154_LB_MPDU_106_POS                                    (16U)
#define M154_LB_MPDU_106_LEN                                    (8U)
#define M154_LB_MPDU_106_MSK                                    (((1ULL<<M154_LB_MPDU_106_LEN)-1)<<M154_LB_MPDU_106_POS)
#define M154_LB_MPDU_106_UMSK                                   (~(((1ULL<<M154_LB_MPDU_106_LEN)-1)<<M154_LB_MPDU_106_POS))
#define M154_LB_MPDU_107                                        M154_LB_MPDU_107
#define M154_LB_MPDU_107_POS                                    (24U)
#define M154_LB_MPDU_107_LEN                                    (8U)
#define M154_LB_MPDU_107_MSK                                    (((1ULL<<M154_LB_MPDU_107_LEN)-1)<<M154_LB_MPDU_107_POS)
#define M154_LB_MPDU_107_UMSK                                   (~(((1ULL<<M154_LB_MPDU_107_LEN)-1)<<M154_LB_MPDU_107_POS))

/* 0x7EC : m154_lb_mpdu_27 */
#define M154_LB_MPDU_27_OFFSET                                  (0x7EC)
#define M154_LB_MPDU_108                                        M154_LB_MPDU_108
#define M154_LB_MPDU_108_POS                                    (0U)
#define M154_LB_MPDU_108_LEN                                    (8U)
#define M154_LB_MPDU_108_MSK                                    (((1ULL<<M154_LB_MPDU_108_LEN)-1)<<M154_LB_MPDU_108_POS)
#define M154_LB_MPDU_108_UMSK                                   (~(((1ULL<<M154_LB_MPDU_108_LEN)-1)<<M154_LB_MPDU_108_POS))
#define M154_LB_MPDU_109                                        M154_LB_MPDU_109
#define M154_LB_MPDU_109_POS                                    (8U)
#define M154_LB_MPDU_109_LEN                                    (8U)
#define M154_LB_MPDU_109_MSK                                    (((1ULL<<M154_LB_MPDU_109_LEN)-1)<<M154_LB_MPDU_109_POS)
#define M154_LB_MPDU_109_UMSK                                   (~(((1ULL<<M154_LB_MPDU_109_LEN)-1)<<M154_LB_MPDU_109_POS))
#define M154_LB_MPDU_110                                        M154_LB_MPDU_110
#define M154_LB_MPDU_110_POS                                    (16U)
#define M154_LB_MPDU_110_LEN                                    (8U)
#define M154_LB_MPDU_110_MSK                                    (((1ULL<<M154_LB_MPDU_110_LEN)-1)<<M154_LB_MPDU_110_POS)
#define M154_LB_MPDU_110_UMSK                                   (~(((1ULL<<M154_LB_MPDU_110_LEN)-1)<<M154_LB_MPDU_110_POS))
#define M154_LB_MPDU_111                                        M154_LB_MPDU_111
#define M154_LB_MPDU_111_POS                                    (24U)
#define M154_LB_MPDU_111_LEN                                    (8U)
#define M154_LB_MPDU_111_MSK                                    (((1ULL<<M154_LB_MPDU_111_LEN)-1)<<M154_LB_MPDU_111_POS)
#define M154_LB_MPDU_111_UMSK                                   (~(((1ULL<<M154_LB_MPDU_111_LEN)-1)<<M154_LB_MPDU_111_POS))

/* 0x7F0 : m154_lb_mpdu_28 */
#define M154_LB_MPDU_28_OFFSET                                  (0x7F0)
#define M154_LB_MPDU_112                                        M154_LB_MPDU_112
#define M154_LB_MPDU_112_POS                                    (0U)
#define M154_LB_MPDU_112_LEN                                    (8U)
#define M154_LB_MPDU_112_MSK                                    (((1ULL<<M154_LB_MPDU_112_LEN)-1)<<M154_LB_MPDU_112_POS)
#define M154_LB_MPDU_112_UMSK                                   (~(((1ULL<<M154_LB_MPDU_112_LEN)-1)<<M154_LB_MPDU_112_POS))
#define M154_LB_MPDU_113                                        M154_LB_MPDU_113
#define M154_LB_MPDU_113_POS                                    (8U)
#define M154_LB_MPDU_113_LEN                                    (8U)
#define M154_LB_MPDU_113_MSK                                    (((1ULL<<M154_LB_MPDU_113_LEN)-1)<<M154_LB_MPDU_113_POS)
#define M154_LB_MPDU_113_UMSK                                   (~(((1ULL<<M154_LB_MPDU_113_LEN)-1)<<M154_LB_MPDU_113_POS))
#define M154_LB_MPDU_114                                        M154_LB_MPDU_114
#define M154_LB_MPDU_114_POS                                    (16U)
#define M154_LB_MPDU_114_LEN                                    (8U)
#define M154_LB_MPDU_114_MSK                                    (((1ULL<<M154_LB_MPDU_114_LEN)-1)<<M154_LB_MPDU_114_POS)
#define M154_LB_MPDU_114_UMSK                                   (~(((1ULL<<M154_LB_MPDU_114_LEN)-1)<<M154_LB_MPDU_114_POS))
#define M154_LB_MPDU_115                                        M154_LB_MPDU_115
#define M154_LB_MPDU_115_POS                                    (24U)
#define M154_LB_MPDU_115_LEN                                    (8U)
#define M154_LB_MPDU_115_MSK                                    (((1ULL<<M154_LB_MPDU_115_LEN)-1)<<M154_LB_MPDU_115_POS)
#define M154_LB_MPDU_115_UMSK                                   (~(((1ULL<<M154_LB_MPDU_115_LEN)-1)<<M154_LB_MPDU_115_POS))

/* 0x7F4 : m154_lb_mpdu_29 */
#define M154_LB_MPDU_29_OFFSET                                  (0x7F4)
#define M154_LB_MPDU_116                                        M154_LB_MPDU_116
#define M154_LB_MPDU_116_POS                                    (0U)
#define M154_LB_MPDU_116_LEN                                    (8U)
#define M154_LB_MPDU_116_MSK                                    (((1ULL<<M154_LB_MPDU_116_LEN)-1)<<M154_LB_MPDU_116_POS)
#define M154_LB_MPDU_116_UMSK                                   (~(((1ULL<<M154_LB_MPDU_116_LEN)-1)<<M154_LB_MPDU_116_POS))
#define M154_LB_MPDU_117                                        M154_LB_MPDU_117
#define M154_LB_MPDU_117_POS                                    (8U)
#define M154_LB_MPDU_117_LEN                                    (8U)
#define M154_LB_MPDU_117_MSK                                    (((1ULL<<M154_LB_MPDU_117_LEN)-1)<<M154_LB_MPDU_117_POS)
#define M154_LB_MPDU_117_UMSK                                   (~(((1ULL<<M154_LB_MPDU_117_LEN)-1)<<M154_LB_MPDU_117_POS))
#define M154_LB_MPDU_118                                        M154_LB_MPDU_118
#define M154_LB_MPDU_118_POS                                    (16U)
#define M154_LB_MPDU_118_LEN                                    (8U)
#define M154_LB_MPDU_118_MSK                                    (((1ULL<<M154_LB_MPDU_118_LEN)-1)<<M154_LB_MPDU_118_POS)
#define M154_LB_MPDU_118_UMSK                                   (~(((1ULL<<M154_LB_MPDU_118_LEN)-1)<<M154_LB_MPDU_118_POS))
#define M154_LB_MPDU_119                                        M154_LB_MPDU_119
#define M154_LB_MPDU_119_POS                                    (24U)
#define M154_LB_MPDU_119_LEN                                    (8U)
#define M154_LB_MPDU_119_MSK                                    (((1ULL<<M154_LB_MPDU_119_LEN)-1)<<M154_LB_MPDU_119_POS)
#define M154_LB_MPDU_119_UMSK                                   (~(((1ULL<<M154_LB_MPDU_119_LEN)-1)<<M154_LB_MPDU_119_POS))

/* 0x7F8 : m154_lb_mpdu_30 */
#define M154_LB_MPDU_30_OFFSET                                  (0x7F8)
#define M154_LB_MPDU_120                                        M154_LB_MPDU_120
#define M154_LB_MPDU_120_POS                                    (0U)
#define M154_LB_MPDU_120_LEN                                    (8U)
#define M154_LB_MPDU_120_MSK                                    (((1ULL<<M154_LB_MPDU_120_LEN)-1)<<M154_LB_MPDU_120_POS)
#define M154_LB_MPDU_120_UMSK                                   (~(((1ULL<<M154_LB_MPDU_120_LEN)-1)<<M154_LB_MPDU_120_POS))
#define M154_LB_MPDU_121                                        M154_LB_MPDU_121
#define M154_LB_MPDU_121_POS                                    (8U)
#define M154_LB_MPDU_121_LEN                                    (8U)
#define M154_LB_MPDU_121_MSK                                    (((1ULL<<M154_LB_MPDU_121_LEN)-1)<<M154_LB_MPDU_121_POS)
#define M154_LB_MPDU_121_UMSK                                   (~(((1ULL<<M154_LB_MPDU_121_LEN)-1)<<M154_LB_MPDU_121_POS))
#define M154_LB_MPDU_122                                        M154_LB_MPDU_122
#define M154_LB_MPDU_122_POS                                    (16U)
#define M154_LB_MPDU_122_LEN                                    (8U)
#define M154_LB_MPDU_122_MSK                                    (((1ULL<<M154_LB_MPDU_122_LEN)-1)<<M154_LB_MPDU_122_POS)
#define M154_LB_MPDU_122_UMSK                                   (~(((1ULL<<M154_LB_MPDU_122_LEN)-1)<<M154_LB_MPDU_122_POS))
#define M154_LB_MPDU_123                                        M154_LB_MPDU_123
#define M154_LB_MPDU_123_POS                                    (24U)
#define M154_LB_MPDU_123_LEN                                    (8U)
#define M154_LB_MPDU_123_MSK                                    (((1ULL<<M154_LB_MPDU_123_LEN)-1)<<M154_LB_MPDU_123_POS)
#define M154_LB_MPDU_123_UMSK                                   (~(((1ULL<<M154_LB_MPDU_123_LEN)-1)<<M154_LB_MPDU_123_POS))

/* 0x7FC : m154_lb_mpdu_31 */
#define M154_LB_MPDU_31_OFFSET                                  (0x7FC)
#define M154_LB_MPDU_124                                        M154_LB_MPDU_124
#define M154_LB_MPDU_124_POS                                    (0U)
#define M154_LB_MPDU_124_LEN                                    (8U)
#define M154_LB_MPDU_124_MSK                                    (((1ULL<<M154_LB_MPDU_124_LEN)-1)<<M154_LB_MPDU_124_POS)
#define M154_LB_MPDU_124_UMSK                                   (~(((1ULL<<M154_LB_MPDU_124_LEN)-1)<<M154_LB_MPDU_124_POS))
#define M154_LB_MPDU_FCS_07_00                                  M154_LB_MPDU_FCS_07_00
#define M154_LB_MPDU_FCS_07_00_POS                              (8U)
#define M154_LB_MPDU_FCS_07_00_LEN                              (8U)
#define M154_LB_MPDU_FCS_07_00_MSK                              (((1ULL<<M154_LB_MPDU_FCS_07_00_LEN)-1)<<M154_LB_MPDU_FCS_07_00_POS)
#define M154_LB_MPDU_FCS_07_00_UMSK                             (~(((1ULL<<M154_LB_MPDU_FCS_07_00_LEN)-1)<<M154_LB_MPDU_FCS_07_00_POS))
#define M154_LB_MPDU_FCS_15_08                                  M154_LB_MPDU_FCS_15_08
#define M154_LB_MPDU_FCS_15_08_POS                              (16U)
#define M154_LB_MPDU_FCS_15_08_LEN                              (8U)
#define M154_LB_MPDU_FCS_15_08_MSK                              (((1ULL<<M154_LB_MPDU_FCS_15_08_LEN)-1)<<M154_LB_MPDU_FCS_15_08_POS)
#define M154_LB_MPDU_FCS_15_08_UMSK                             (~(((1ULL<<M154_LB_MPDU_FCS_15_08_LEN)-1)<<M154_LB_MPDU_FCS_15_08_POS))
#define M154_LB_PHR                                             M154_LB_PHR
#define M154_LB_PHR_POS                                         (24U)
#define M154_LB_PHR_LEN                                         (8U)
#define M154_LB_PHR_MSK                                         (((1ULL<<M154_LB_PHR_LEN)-1)<<M154_LB_PHR_POS)
#define M154_LB_PHR_UMSK                                        (~(((1ULL<<M154_LB_PHR_LEN)-1)<<M154_LB_PHR_POS))


struct  m154_reg {
    /* 0x0 : m154_glb_cfg */
    union {
        struct {
            uint32_t clk_en                         :  1; /* [    0],        r/w,        0x0 */
            uint32_t beacon_mode_en                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t zbEn_protect                   :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t clk_xtal_freq                  :  6; /* [ 9: 4],        r/w,       0x20 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t std_version                    :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rf_center_freq_reg             : 12; /* [27:16],        r/w,      0x965 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_glb_cfg;

    /* 0x4 : m154_data_cfg */
    union {
        struct {
            uint32_t rx_sym_bit_reverse_en          :  1; /* [    0],        r/w,        0x0 */
            uint32_t tx_sym_bit_reverse_en          :  1; /* [    1],        r/w,        0x0 */
            uint32_t rx_crc_fault_en                :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx_crc_fault_en                :  1; /* [    3],        r/w,        0x0 */
            uint32_t rx_data_fault_en               :  1; /* [    4],        r/w,        0x0 */
            uint32_t tx_data_fault_en               :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_data_cfg;

    /* 0x8 : m154_swrst */
    union {
        struct {
            uint32_t soft_reset_regs_cmd            :  1; /* [    0],        w1c,        0x0 */
            uint32_t soft_reset_tcon_cmd            :  1; /* [    1],        w1c,        0x0 */
            uint32_t soft_reset_rx_cmd              :  1; /* [    2],        w1c,        0x0 */
            uint32_t soft_reset_tx_cmd              :  1; /* [    3],        w1c,        0x0 */
            uint32_t soft_reset_csma_ca_cmd         :  1; /* [    4],        w1c,        0x0 */
            uint32_t soft_reset_bist_cmd            :  1; /* [    5],        w1c,        0x0 */
            uint32_t rx_crc_sw_clr                  :  1; /* [    6],        w1c,        0x0 */
            uint32_t tx_crc_sw_clr                  :  1; /* [    7],        w1c,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_swrst;

    /* 0xC : m154_dbg_sel */
    union {
        struct {
            uint32_t debug_out_sel                  :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_dbg_sel;

    /* 0x10 : m154_dbg_int_en */
    union {
        struct {
            uint32_t debug_int_raw_en               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_dbg_int_en;

    /* 0x14 : m154_bist_ctrl */
    union {
        struct {
            uint32_t bist_pattern_cmd               :  1; /* [    0],        w1c,        0x0 */
            uint32_t bist_pattern_bp_bound_en       :  1; /* [    1],        r/w,        0x1 */
            uint32_t bist_pattern_ar_sel            :  1; /* [    2],        r/w,        0x1 */
            uint32_t bist_auto_ack_en               :  1; /* [    3],        r/w,        0x0 */
            uint32_t bist_periodic_beacon_en        :  1; /* [    4],        r/w,        0x0 */
            uint32_t bist_periodic_beacon_sel       :  1; /* [    5],        r/w,        0x0 */
            uint32_t bist_ext_dut_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t bist_en                        :  1; /* [    7],        r/w,        0x0 */
            uint32_t bist_periodic_beacon_order     :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t bist_pattern_sel               :  4; /* [15:12],        r/w,        0x0 */
            uint32_t bist_doing                     :  1; /* [   16],          r,        0x0 */
            uint32_t bist_auto_ack_doing            :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_bist_ctrl;

    /* 0x18 : m154_bist_dly */
    union {
        struct {
            uint32_t bist_pattern_trig_time         : 32; /* [31: 0],        r/w,      0x100 */
        }BF;
        uint32_t WORD;
    } m154_bist_dly;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[4];

    /* 0x20 : m154_int_en */
    union {
        struct {
            uint32_t alarm_00_int_en                :  1; /* [    0],        r/w,        0x0 */
            uint32_t alarm_01_int_en                :  1; /* [    1],        r/w,        0x0 */
            uint32_t alarm_02_int_en                :  1; /* [    2],        r/w,        0x0 */
            uint32_t alarm_03_int_en                :  1; /* [    3],        r/w,        0x0 */
            uint32_t alarm_04_int_en                :  1; /* [    4],        r/w,        0x0 */
            uint32_t alarm_05_int_en                :  1; /* [    5],        r/w,        0x0 */
            uint32_t alarm_06_int_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t alarm_07_int_en                :  1; /* [    7],        r/w,        0x0 */
            uint32_t alarm_08_int_en                :  1; /* [    8],        r/w,        0x0 */
            uint32_t alarm_09_int_en                :  1; /* [    9],        r/w,        0x0 */
            uint32_t rx_end_frak_int_en             :  1; /* [   10],        r/w,        0x0 */
            uint32_t rx_end_req_enh_ack_int_en      :  1; /* [   11],        r/w,        0x0 */
            uint32_t rx_end_ext_int_en              :  1; /* [   12],        r/w,        0x0 */
            uint32_t rx_end_mpp_int_en              :  1; /* [   13],        r/w,        0x0 */
            uint32_t rx_nbr_search_fail_int_en      :  1; /* [   14],        r/w,        0x0 */
            uint32_t cs_alert_int_en                :  1; /* [   15],        r/w,        0x0 */
            uint32_t rx_start_int_en                :  1; /* [   16],        r/w,        0x0 */
            uint32_t rx_mhr_int_en                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t rx_mhr_security_int_en         :  1; /* [   18],        r/w,        0x0 */
            uint32_t rx_filter_fail_int_en          :  1; /* [   19],        r/w,        0x0 */
            uint32_t rx_end_beacon_int_en           :  1; /* [   20],        r/w,        0x1 */
            uint32_t rx_end_data_int_en             :  1; /* [   21],        r/w,        0x1 */
            uint32_t rx_end_ack_int_en              :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_end_cmd_int_en              :  1; /* [   23],        r/w,        0x1 */
            uint32_t rx_end_ack_others_int_en       :  1; /* [   24],        r/w,        0x0 */
            uint32_t csma_ca_success_int_en         :  1; /* [   25],        r/w,        0x0 */
            uint32_t csma_ca_failure_int_en         :  1; /* [   26],        r/w,        0x1 */
            uint32_t csma_ca_timeout_int_en         :  1; /* [   27],        r/w,        0x0 */
            uint32_t tx_done_int_en                 :  1; /* [   28],        r/w,        0x1 */
            uint32_t tx_auto_ack_done_int_en        :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_abort_int_en                :  1; /* [   30],        r/w,        0x0 */
            uint32_t tx_abort_int_en                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_int_en;

    /* 0x24 : m154_int_clr */
    union {
        struct {
            uint32_t alarm_00_int_clr               :  1; /* [    0],        w1c,        0x0 */
            uint32_t alarm_01_int_clr               :  1; /* [    1],        w1c,        0x0 */
            uint32_t alarm_02_int_clr               :  1; /* [    2],        w1c,        0x0 */
            uint32_t alarm_03_int_clr               :  1; /* [    3],        w1c,        0x0 */
            uint32_t alarm_04_int_clr               :  1; /* [    4],        w1c,        0x0 */
            uint32_t alarm_05_int_clr               :  1; /* [    5],        w1c,        0x0 */
            uint32_t alarm_06_int_clr               :  1; /* [    6],        w1c,        0x0 */
            uint32_t alarm_07_int_clr               :  1; /* [    7],        w1c,        0x0 */
            uint32_t alarm_08_int_clr               :  1; /* [    8],        w1c,        0x0 */
            uint32_t alarm_09_int_clr               :  1; /* [    9],        w1c,        0x0 */
            uint32_t rx_end_frak_int_clr            :  1; /* [   10],        w1c,        0x0 */
            uint32_t rx_end_req_enh_ack_int_clr     :  1; /* [   11],        w1c,        0x0 */
            uint32_t rx_end_ext_int_clr             :  1; /* [   12],        w1c,        0x0 */
            uint32_t rx_end_mpp_int_clr             :  1; /* [   13],        w1c,        0x0 */
            uint32_t rx_nbr_search_fail_int_clr     :  1; /* [   14],        w1c,        0x0 */
            uint32_t cs_alert_int_clr               :  1; /* [   15],        w1c,        0x0 */
            uint32_t rx_start_int_clr               :  1; /* [   16],        w1c,        0x0 */
            uint32_t rx_mhr_int_clr                 :  1; /* [   17],        w1c,        0x0 */
            uint32_t rx_mhr_security_int_clr        :  1; /* [   18],        w1c,        0x0 */
            uint32_t rx_filter_fail_int_clr         :  1; /* [   19],        w1c,        0x0 */
            uint32_t rx_end_beacon_int_clr          :  1; /* [   20],        w1c,        0x0 */
            uint32_t rx_end_data_int_clr            :  1; /* [   21],        w1c,        0x0 */
            uint32_t rx_end_ack_int_clr             :  1; /* [   22],        w1c,        0x0 */
            uint32_t rx_end_cmd_int_clr             :  1; /* [   23],        w1c,        0x0 */
            uint32_t rx_end_ack_others_int_clr      :  1; /* [   24],        w1c,        0x0 */
            uint32_t csma_ca_success_int_clr        :  1; /* [   25],        w1c,        0x0 */
            uint32_t csma_ca_failure_int_clr        :  1; /* [   26],        w1c,        0x0 */
            uint32_t csma_ca_timeout_int_clr        :  1; /* [   27],        w1c,        0x0 */
            uint32_t tx_done_int_clr                :  1; /* [   28],        w1c,        0x0 */
            uint32_t tx_auto_ack_done_int_clr       :  1; /* [   29],        w1c,        0x0 */
            uint32_t rx_abort_int_clr               :  1; /* [   30],        w1c,        0x0 */
            uint32_t tx_abort_int_clr               :  1; /* [   31],        w1c,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_int_clr;

    /* 0x28 : m154_int_mask */
    union {
        struct {
            uint32_t alarm_00_int_mask              :  1; /* [    0],        r/w,        0x0 */
            uint32_t alarm_01_int_mask              :  1; /* [    1],        r/w,        0x0 */
            uint32_t alarm_02_int_mask              :  1; /* [    2],        r/w,        0x0 */
            uint32_t alarm_03_int_mask              :  1; /* [    3],        r/w,        0x0 */
            uint32_t alarm_04_int_mask              :  1; /* [    4],        r/w,        0x0 */
            uint32_t alarm_05_int_mask              :  1; /* [    5],        r/w,        0x0 */
            uint32_t alarm_06_int_mask              :  1; /* [    6],        r/w,        0x0 */
            uint32_t alarm_07_int_mask              :  1; /* [    7],        r/w,        0x0 */
            uint32_t alarm_08_int_mask              :  1; /* [    8],        r/w,        0x0 */
            uint32_t alarm_09_int_mask              :  1; /* [    9],        r/w,        0x0 */
            uint32_t rx_end_frak_int_mask           :  1; /* [   10],        r/w,        0x0 */
            uint32_t rx_end_req_enh_ack_int_mask    :  1; /* [   11],        r/w,        0x0 */
            uint32_t rx_end_ext_int_mask            :  1; /* [   12],        r/w,        0x0 */
            uint32_t rx_end_mpp_int_mask            :  1; /* [   13],        r/w,        0x0 */
            uint32_t rx_nbr_search_fail_int_mask    :  1; /* [   14],        r/w,        0x0 */
            uint32_t cs_alert_int_mask              :  1; /* [   15],        r/w,        0x0 */
            uint32_t rx_start_int_mask              :  1; /* [   16],        r/w,        0x0 */
            uint32_t rx_mhr_int_mask                :  1; /* [   17],        r/w,        0x0 */
            uint32_t rx_mhr_security_int_mask       :  1; /* [   18],        r/w,        0x0 */
            uint32_t rx_filter_fail_int_mask        :  1; /* [   19],        r/w,        0x0 */
            uint32_t rx_end_beacon_int_mask         :  1; /* [   20],        r/w,        0x0 */
            uint32_t rx_end_data_int_mask           :  1; /* [   21],        r/w,        0x0 */
            uint32_t rx_end_ack_int_mask            :  1; /* [   22],        r/w,        0x0 */
            uint32_t rx_end_cmd_int_mask            :  1; /* [   23],        r/w,        0x0 */
            uint32_t rx_end_ack_others_int_mask     :  1; /* [   24],        r/w,        0x0 */
            uint32_t csma_ca_success_int_mask       :  1; /* [   25],        r/w,        0x0 */
            uint32_t csma_ca_failure_int_mask       :  1; /* [   26],        r/w,        0x0 */
            uint32_t csma_ca_timeout_int_mask       :  1; /* [   27],        r/w,        0x0 */
            uint32_t tx_done_int_mask               :  1; /* [   28],        r/w,        0x0 */
            uint32_t tx_auto_ack_done_int_mask      :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_abort_int_mask              :  1; /* [   30],        r/w,        0x0 */
            uint32_t tx_abort_int_mask              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_int_mask;

    /* 0x2C : m154_int_raw */
    union {
        struct {
            uint32_t alarm_00_int_raw               :  1; /* [    0],          r,        0x0 */
            uint32_t alarm_01_int_raw               :  1; /* [    1],          r,        0x0 */
            uint32_t alarm_02_int_raw               :  1; /* [    2],          r,        0x0 */
            uint32_t alarm_03_int_raw               :  1; /* [    3],          r,        0x0 */
            uint32_t alarm_04_int_raw               :  1; /* [    4],          r,        0x0 */
            uint32_t alarm_05_int_raw               :  1; /* [    5],          r,        0x0 */
            uint32_t alarm_06_int_raw               :  1; /* [    6],          r,        0x0 */
            uint32_t alarm_07_int_raw               :  1; /* [    7],          r,        0x0 */
            uint32_t alarm_08_int_raw               :  1; /* [    8],          r,        0x0 */
            uint32_t alarm_09_int_raw               :  1; /* [    9],          r,        0x0 */
            uint32_t rx_end_frak_int_raw            :  1; /* [   10],          r,        0x0 */
            uint32_t rx_end_req_enh_ack_int_raw     :  1; /* [   11],          r,        0x0 */
            uint32_t rx_end_ext_int_raw             :  1; /* [   12],          r,        0x0 */
            uint32_t rx_end_mpp_int_raw             :  1; /* [   13],          r,        0x0 */
            uint32_t rx_nbr_search_fail_int_raw     :  1; /* [   14],          r,        0x0 */
            uint32_t cs_alert_int_raw               :  1; /* [   15],          r,        0x0 */
            uint32_t rx_start_int_raw               :  1; /* [   16],          r,        0x0 */
            uint32_t rx_mhr_int_raw                 :  1; /* [   17],          r,        0x0 */
            uint32_t rx_mhr_security_int_raw        :  1; /* [   18],          r,        0x0 */
            uint32_t rx_filter_fail_int_raw         :  1; /* [   19],          r,        0x0 */
            uint32_t rx_end_beacon_int_raw          :  1; /* [   20],          r,        0x0 */
            uint32_t rx_end_data_int_raw            :  1; /* [   21],          r,        0x0 */
            uint32_t rx_end_ack_int_raw             :  1; /* [   22],          r,        0x0 */
            uint32_t rx_end_cmd_int_raw             :  1; /* [   23],          r,        0x0 */
            uint32_t rx_end_ack_others_int_raw      :  1; /* [   24],          r,        0x0 */
            uint32_t csma_ca_success_int_raw        :  1; /* [   25],          r,        0x0 */
            uint32_t csma_ca_failure_int_raw        :  1; /* [   26],          r,        0x0 */
            uint32_t csma_ca_timeout_int_raw        :  1; /* [   27],          r,        0x0 */
            uint32_t tx_done_int_raw                :  1; /* [   28],          r,        0x0 */
            uint32_t tx_auto_ack_done_int_raw       :  1; /* [   29],          r,        0x0 */
            uint32_t rx_abort_int_raw               :  1; /* [   30],          r,        0x0 */
            uint32_t tx_abort_int_raw               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_int_raw;

    /* 0x30 : m154_int */
    union {
        struct {
            uint32_t alarm_00_int                   :  1; /* [    0],          r,        0x0 */
            uint32_t alarm_01_int                   :  1; /* [    1],          r,        0x0 */
            uint32_t alarm_02_int                   :  1; /* [    2],          r,        0x0 */
            uint32_t alarm_03_int                   :  1; /* [    3],          r,        0x0 */
            uint32_t alarm_04_int                   :  1; /* [    4],          r,        0x0 */
            uint32_t alarm_05_int                   :  1; /* [    5],          r,        0x0 */
            uint32_t alarm_06_int                   :  1; /* [    6],          r,        0x0 */
            uint32_t alarm_07_int                   :  1; /* [    7],          r,        0x0 */
            uint32_t alarm_08_int                   :  1; /* [    8],          r,        0x0 */
            uint32_t alarm_09_int                   :  1; /* [    9],          r,        0x0 */
            uint32_t rx_end_frak_int                :  1; /* [   10],          r,        0x0 */
            uint32_t rx_end_req_enh_ack_int         :  1; /* [   11],          r,        0x0 */
            uint32_t rx_end_ext_int                 :  1; /* [   12],          r,        0x0 */
            uint32_t rx_end_mpp_int                 :  1; /* [   13],          r,        0x0 */
            uint32_t rx_nbr_search_fail_int         :  1; /* [   14],          r,        0x0 */
            uint32_t cs_alert_int                   :  1; /* [   15],          r,        0x0 */
            uint32_t rx_start_int                   :  1; /* [   16],          r,        0x0 */
            uint32_t rx_mhr_int                     :  1; /* [   17],          r,        0x0 */
            uint32_t rx_mhr_security_int            :  1; /* [   18],          r,        0x0 */
            uint32_t rx_filter_fail_int             :  1; /* [   19],          r,        0x0 */
            uint32_t rx_end_beacon_int              :  1; /* [   20],          r,        0x0 */
            uint32_t rx_end_data_int                :  1; /* [   21],          r,        0x0 */
            uint32_t rx_end_ack_int                 :  1; /* [   22],          r,        0x0 */
            uint32_t rx_end_cmd_int                 :  1; /* [   23],          r,        0x0 */
            uint32_t rx_end_ack_others_int          :  1; /* [   24],          r,        0x0 */
            uint32_t csma_ca_success_int            :  1; /* [   25],          r,        0x0 */
            uint32_t csma_ca_failure_int            :  1; /* [   26],          r,        0x0 */
            uint32_t csma_ca_timeout_int            :  1; /* [   27],          r,        0x0 */
            uint32_t tx_done_int                    :  1; /* [   28],          r,        0x0 */
            uint32_t tx_auto_ack_done_int           :  1; /* [   29],          r,        0x0 */
            uint32_t rx_abort_int                   :  1; /* [   30],          r,        0x0 */
            uint32_t tx_abort_int                   :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_int;

    /* 0x34 : m154_gci_ctrl0 */
    union {
        struct {
            uint32_t gci_ctrl_fw_sel                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t gci_ctrl_fw                    :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t gci_ctrl_hw                    :  4; /* [19:16],          r,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t gci_ctrl_sync                  :  4; /* [27:24],          r,        0x0 */
            uint32_t gci_int_sync                   :  1; /* [   28],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_gci_ctrl0;

    /* 0x38 : m154_gci_ctrl1 */
    union {
        struct {
            uint32_t gci_tx_confx_cancel_rx_en      :  1; /* [    0],        r/w,        0x0 */
            uint32_t gci_tx_confx_cancel_tx_en      :  1; /* [    1],        r/w,        0x0 */
            uint32_t gci_tx_confx_int_dual_en       :  1; /* [    2],        r/w,        0x0 */
            uint32_t gci_int_dual_en                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_gci_ctrl1;

    /* 0x3c  reserved */
    uint8_t RESERVED0x3c[4];

    /* 0x40 : m154_sym_cnt */
    union {
        struct {
            uint32_t sym_cnt                        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt;

    /* 0x44 : m154_sym_cnt_frc */
    union {
        struct {
            uint32_t sym_cnt_frc                    :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t sym_cnt_rx_beacon_p1_flag      :  4; /* [11: 8],          r,        0x0 */
            uint32_t sym_cnt_rx_beacon_p2_flag      :  4; /* [15:12],          r,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_frc;

    /* 0x48 : m154_sym_cnt_rx_beacon_p1 */
    union {
        struct {
            uint32_t sym_cnt_rx_beacon_p1           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_rx_beacon_p1;

    /* 0x4C : m154_sym_cnt_rx_beacon_p2 */
    union {
        struct {
            uint32_t sym_cnt_rx_beacon_p2           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_rx_beacon_p2;

    /* 0x50 : m154_sym_cnt_rx_start */
    union {
        struct {
            uint32_t sym_cnt_rx_start               : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_rx_start;

    /* 0x54 : m154_sym_cnt_rx_end */
    union {
        struct {
            uint32_t sym_cnt_rx_end                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_rx_end;

    /* 0x58 : m154_sym_cnt_csma_ca_start */
    union {
        struct {
            uint32_t sym_cnt_csma_ca_start          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_csma_ca_start;

    /* 0x5C : m154_sym_cnt_tx_beacon */
    union {
        struct {
            uint32_t sym_cnt_tx_beacon              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_tx_beacon;

    /* 0x60 : m154_sym_cnt_tx_end */
    union {
        struct {
            uint32_t sym_cnt_tx_end                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sym_cnt_tx_end;

    /* 0x64 : m154_alarm_00_target */
    union {
        struct {
            uint32_t alarm_00_target                : 32; /* [31: 0],        r/w,       0x10 */
        }BF;
        uint32_t WORD;
    } m154_alarm_00_target;

    /* 0x68 : m154_alarm_01_target */
    union {
        struct {
            uint32_t alarm_01_target                : 32; /* [31: 0],        r/w,       0x20 */
        }BF;
        uint32_t WORD;
    } m154_alarm_01_target;

    /* 0x6C : m154_alarm_02_target */
    union {
        struct {
            uint32_t alarm_02_target                : 32; /* [31: 0],        r/w,       0x30 */
        }BF;
        uint32_t WORD;
    } m154_alarm_02_target;

    /* 0x70 : m154_alarm_03_target */
    union {
        struct {
            uint32_t alarm_03_target                : 32; /* [31: 0],        r/w,       0x40 */
        }BF;
        uint32_t WORD;
    } m154_alarm_03_target;

    /* 0x74 : m154_alarm_04_target */
    union {
        struct {
            uint32_t alarm_04_target                : 32; /* [31: 0],        r/w,       0x50 */
        }BF;
        uint32_t WORD;
    } m154_alarm_04_target;

    /* 0x78 : m154_alarm_05_target */
    union {
        struct {
            uint32_t alarm_05_target                : 32; /* [31: 0],        r/w,       0x60 */
        }BF;
        uint32_t WORD;
    } m154_alarm_05_target;

    /* 0x7C : m154_alarm_06_target */
    union {
        struct {
            uint32_t alarm_06_target                : 32; /* [31: 0],        r/w,       0x70 */
        }BF;
        uint32_t WORD;
    } m154_alarm_06_target;

    /* 0x80 : m154_alarm_07_target */
    union {
        struct {
            uint32_t alarm_07_target                : 32; /* [31: 0],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } m154_alarm_07_target;

    /* 0x84 : m154_alarm_08_target */
    union {
        struct {
            uint32_t alarm_08_target                : 32; /* [31: 0],        r/w,       0x90 */
        }BF;
        uint32_t WORD;
    } m154_alarm_08_target;

    /* 0x88 : m154_alarm_09_target */
    union {
        struct {
            uint32_t alarm_09_target                : 32; /* [31: 0],        r/w,       0xa0 */
        }BF;
        uint32_t WORD;
    } m154_alarm_09_target;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[4];

    /* 0x90 : m154_csma_0 */
    union {
        struct {
            uint32_t csma_ca_en                     :  1; /* [    0],        r/w,        0x1 */
            uint32_t ble_mode                       :  1; /* [    1],        r/w,        0x0 */
            uint32_t csma_ca_eb_mode                :  1; /* [    2],        r/w,        0x0 */
            uint32_t csma_ca_ecr_mode               :  1; /* [    3],        r/w,        0x0 */
            uint32_t csma_ca_slot_en                :  1; /* [    4],        r/w,        0x0 */
            uint32_t csma_ca_timeout_check_option   :  1; /* [    5],        r/w,        0x1 */
            uint32_t csma_ca_sw_mode                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t cw0_sel                        :  2; /* [ 9: 8],        r/w,        0x2 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t csma_ca_cca_en_offset          :  4; /* [15:12],        r/w,        0x0 */
            uint32_t csma_ca_transaction_time       :  9; /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t csma_ca_cca_timeout_sel        :  1; /* [   28],        r/w,        0x0 */
            uint32_t backoff_rx_dis                 :  1; /* [   29],        r/w,        0x1 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_csma_0;

    /* 0x94 : m154_csma_1 */
    union {
        struct {
            uint32_t manual_be                      :  4; /* [ 3: 0],        r/w,        0x3 */
            uint32_t manual_be_sel                  :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t minus_be                       :  3; /* [10: 8],        r/w,        0x1 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t plus_be                        :  3; /* [14:12],        r/w,        0x1 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t min_be                         :  4; /* [19:16],        r/w,        0x3 */
            uint32_t max_be                         :  4; /* [23:20],        r/w,        0x5 */
            uint32_t max_csma_backoffs              :  3; /* [26:24],        r/w,        0x4 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t csma_ca_avg_pkt_len            :  4; /* [31:28],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } m154_csma_1;

    /* 0x98 : m154_csma_2 */
    union {
        struct {
            uint32_t csma_ca_cap_end                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_csma_2;

    /* 0x9C : m154_csma_3 */
    union {
        struct {
            uint32_t csma_ca_ble_period_end         : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_csma_3;

    /* 0xA0 : m154_csma_4 */
    union {
        struct {
            uint32_t csma_ca_fsm_curr               :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t csma_ca_be_curr                :  4; /* [11: 8],          r,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_csma_4;

    /* 0xa4  reserved */
    uint8_t RESERVED0xa4[12];

    /* 0xB0 : m154_tx_0 */
    union {
        struct {
            uint32_t tx_sn_for_rx_ack_judge         :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_aes_ddt_phr_flag       :  1; /* [    8],          r,        0x0 */
            uint32_t tx_mpdu_aes_ddt_done_flag      :  1; /* [    9],          r,        0x0 */
            uint32_t tx_doing                       :  1; /* [   10],          r,        0x0 */
            uint32_t tx_auto_ack_doing              :  1; /* [   11],          r,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t tx_mpdu_sym_cnt_acc            :  9; /* [24:16],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_0;

    /* 0xB4 : m154_tx_1 */
    union {
        struct {
            uint32_t tx_trig_cmd                    :  1; /* [    0],        w1c,        0x0 */
            uint32_t tx_mpdu_src_sel                :  1; /* [    1],        r/w,        0x0 */
            uint32_t tx_trig_time_sel               :  1; /* [    2],        r/w,        0x0 */
            uint32_t tx_mpdu_aes_ddt_wait_sel       :  1; /* [    3],        r/w,        0x1 */
            uint32_t tx_auto_ack_en                 :  1; /* [    4],        r/w,        0x1 */
            uint32_t tx_auto_ack_fp_bit             :  1; /* [    5],        r/w,        0x1 */
            uint32_t tx_auto_ack_fp_from_sw         :  1; /* [    6],        r/w,        0x0 */
            uint32_t tx_infinite_en                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t tx_start_offset                :  8; /* [15: 8],        r/w,       0x3c */
            uint32_t tx_postpone_by_rx_doing_en     :  1; /* [   16],        r/w,        0x1 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_1;

    /* 0xB8 : m154_tx_2 */
    union {
        struct {
            uint32_t tx_trig_time                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_2;

    /* 0xBC : m154_tx_3 */
    union {
        struct {
            uint32_t tx_fsm_curr                    : 11; /* [10: 0],          r,        0x1 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rx_after_tx_en                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t tx_rx_transition_time          :  8; /* [31:24],        r/w,       0xc0 */
        }BF;
        uint32_t WORD;
    } m154_tx_3;

    /* 0xC0 : m154_rx_00 */
    union {
        struct {
            uint32_t rx_tx_transition_time          :  8; /* [ 7: 0],        r/w,       0xc0 */
            uint32_t rx_mpdu_aes_ddt_trig_en        :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t rx_mpdu_aes_ddt_trig           :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rx_doing                       :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t rx_mpdu_sym_cnt_acc            :  9; /* [28:20],          r,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_00;

    /* 0xC4 : m154_rx_01 */
    union {
        struct {
            uint32_t rx_crc_fail                    :  1; /* [    0],          r,        0x0 */
            uint32_t rx_frame_type_fail             :  1; /* [    1],          r,        0x0 */
            uint32_t rx_frame_version_fail          :  1; /* [    2],          r,        0x0 */
            uint32_t rx_dst_addr_mode_fail          :  1; /* [    3],          r,        0x0 */
            uint32_t rx_src_addr_mode_fail          :  1; /* [    4],          r,        0x0 */
            uint32_t rx_dst_pan_id_fail             :  1; /* [    5],          r,        0x0 */
            uint32_t rx_dst_addr_fail               :  1; /* [    6],          r,        0x0 */
            uint32_t rx_src_pan_id_fail             :  1; /* [    7],          r,        0x0 */
            uint32_t rx_bcn_pan_id_fail             :  1; /* [    8],          r,        0x0 */
            uint32_t stack1_fail                    :  1; /* [    9],          r,        0x0 */
            uint32_t stack2_fail                    :  1; /* [   10],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t rx_phr_err_uv                  :  1; /* [   16],          r,        0x0 */
            uint32_t rx_phr_err_uv_clr              :  1; /* [   17],        w1c,        0x0 */
            uint32_t reserved_18_23                 :  6; /* [23:18],       rsvd,        0x0 */
            uint32_t rx_phr_err_filter_fail_en      :  1; /* [   24],        r/w,        0x1 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t rx_int_clr_by_start_en         :  1; /* [   28],        r/w,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_01;

    /* 0xC8 : m154_rx_02 */
    union {
        struct {
            uint32_t rx_pan_coordinator_1st         :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t rx_pan_coordinator_2nd         :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rx_stack_1st_en                :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t rx_stack_2nd_en                :  1; /* [   12],        r/w,        0x0 */
            uint32_t rx_ch_scan_mode_en             :  1; /* [   13],        r/w,        0x0 */
            uint32_t rx_ch_scan_mode                :  2; /* [15:14],        r/w,        0x0 */
            uint32_t rx_promiscuous_mode            :  1; /* [   16],        r/w,        0x0 */
            uint32_t rx_filter_bypass_sel           :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t group_rx_mode                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t rx_bp_bound_offset_frc         :  4; /* [23:20],        r/w,        0x1 */
            uint32_t rx_bp_bound_offset             :  4; /* [27:24],        r/w,        0xb */
            uint32_t rx_bp_bound_beacon_en          :  1; /* [   28],        r/w,        0x1 */
            uint32_t rx_bp_bound_data_en            :  1; /* [   29],        r/w,        0x0 */
            uint32_t rx_bp_bound_cmd_en             :  1; /* [   30],        r/w,        0x0 */
            uint32_t rx_implicit_broadcast          :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_02;

    /* 0xCC : m154_rx_03 */
    union {
        struct {
            uint32_t rx_addr_short_1st              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t rx_pan_id_1st                  : 16; /* [31:16],        r/w,     0x4321 */
        }BF;
        uint32_t WORD;
    } m154_rx_03;

    /* 0xD0 : m154_rx_04 */
    union {
        struct {
            uint32_t rx_addr_extended_1st_31_00     : 32; /* [31: 0],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } m154_rx_04;

    /* 0xD4 : m154_rx_05 */
    union {
        struct {
            uint32_t rx_addr_extended_1st_63_32     : 32; /* [31: 0],        r/w, 0xacde4800 */
        }BF;
        uint32_t WORD;
    } m154_rx_05;

    /* 0xD8 : m154_rx_06 */
    union {
        struct {
            uint32_t rx_addr_short_2nd              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t rx_pan_id_2nd                  : 16; /* [31:16],        r/w,     0x4321 */
        }BF;
        uint32_t WORD;
    } m154_rx_06;

    /* 0xDC : m154_rx_07 */
    union {
        struct {
            uint32_t rx_addr_extended_2nd_31_00     : 32; /* [31: 0],        r/w,        0x2 */
        }BF;
        uint32_t WORD;
    } m154_rx_07;

    /* 0xE0 : m154_rx_08 */
    union {
        struct {
            uint32_t rx_addr_extended_2nd_63_32     : 32; /* [31: 0],        r/w, 0xacde4800 */
        }BF;
        uint32_t WORD;
    } m154_rx_08;

    /* 0xE4 : m154_rx_09 */
    union {
        struct {
            uint32_t rx_off_trig_time_en            :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_4                   :  4; /* [ 4: 1],       rsvd,        0x0 */
            uint32_t rx_off_trig_by_end_en          :  1; /* [    5],        r/w,        0x1 */
            uint32_t rx_off_trig_by_end_sel         :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t rx_off_cmd                     :  1; /* [    8],        w1c,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t rx_on_cmd                      :  1; /* [   12],        w1c,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rx_start_mac_rx                :  1; /* [   16],          r,        0x0 */
            uint32_t rx_phy_en                      :  1; /* [   17],          r,        0x0 */
            uint32_t sfd_found                      :  1; /* [   18],          r,        0x0 */
            uint32_t rx_ppdu_sym_start              :  1; /* [   19],          r,        0x0 */
            uint32_t csma_ca_rx_st                  :  1; /* [   20],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rx_start_offset                :  8; /* [31:24],        r/w,       0x8f */
        }BF;
        uint32_t WORD;
    } m154_rx_09;

    /* 0xE8 : m154_rx_10 */
    union {
        struct {
            uint32_t rx_off_trig_time               : 32; /* [31: 0],        r/w,      0x100 */
        }BF;
        uint32_t WORD;
    } m154_rx_10;

    /* 0xEC : m154_rx_11 */
    union {
        struct {
            uint32_t rx_frame_counter               : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_11;

    /* 0xF0 : m154_rx_12 */
    union {
        struct {
            uint32_t rx_aux_sec_header_offset       :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t rx_aux_sec_header_length       :  4; /* [11: 8],          r,        0x0 */
            uint32_t rx_security_level              :  3; /* [14:12],          r,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t rx_mic_length                  :  5; /* [20:16],          r,        0x0 */
            uint32_t reserved_21_23                 :  3; /* [23:21],       rsvd,        0x0 */
            uint32_t rx_a_length                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_12;

    /* 0xF4 : m154_rx_13 */
    union {
        struct {
            uint32_t rx_mac_payload_offset          :  8; /* [ 7: 0],          r,        0x3 */
            uint32_t rx_beacon_gts_offset           :  6; /* [13: 8],          r,        0x5 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rx_beacon_pend_addr_offset     :  6; /* [21:16],          r,        0x6 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t rx_beacon_payload_offset       :  8; /* [31:24],          r,        0x7 */
        }BF;
        uint32_t WORD;
    } m154_rx_13;

    /* 0xF8 : m154_rx_14 */
    union {
        struct {
            uint32_t reserved_0_15                  : 16; /* [15: 0],       rsvd,        0x0 */
            uint32_t rx_cmd_id_offset               :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_cmd_id                      :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_14;

    /* 0xFC : m154_rx_15 */
    union {
        struct {
            uint32_t rx_auto_restart_wait_time      : 16; /* [15: 0],        r/w,       0x2e */
            uint32_t rx_ack_wait_time_us            : 14; /* [29:16],        r/w,      0x360 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_15;

    /* 0x100 : m154_rx_16 */
    union {
        struct {
            uint32_t total_frame_wait_time_us       : 32; /* [31: 0],        r/w, 0xffffffff */
        }BF;
        uint32_t WORD;
    } m154_rx_16;

    /* 0x104 : m154_rx_17 */
    union {
        struct {
            uint32_t rx_on_after_rx_ack             :  1; /* [    0],        r/w,        0x0 */
            uint32_t rsvd_00                        : 31; /* [31: 1],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_17;

    /* 0x108 : m154_rsvd_01 */
    union {
        struct {
            uint32_t rsvd_01                        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rsvd_01;

    /* 0x10C : m154_capt_frm_00 */
    union {
        struct {
            uint32_t capt_frame_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t capt_frame_tx_rxn              :  1; /* [    1],        r/w,        0x0 */
            uint32_t capt_frame_retry_cnt           :  6; /* [ 7: 2],        r/w,        0x0 */
            uint32_t capt_frame_phr                 :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t capt_frame_mhr                 : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_capt_frm_00;

    /* 0x110 : m154_sw_profile */
    union {
        struct {
            uint32_t sw_profile                     : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_sw_profile;

    /* 0x114  reserved */
    uint8_t RESERVED0x114[108];

    /* 0x180 : m154_coex_priority */
    union {
        struct {
            uint32_t rx_idle_priority               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t rx_sfd_priority                :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t rx_unicast_priority            :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t tx_priority                    :  4; /* [15:12],        r/w,        0x0 */
            uint32_t rx_on_after_rx_abort_en        :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_27                 : 11; /* [27:17],       rsvd,        0x0 */
            uint32_t rx_abort_int_status            :  1; /* [   28],          r,        0x0 */
            uint32_t tx_abort_int_status            :  1; /* [   29],          r,        0x0 */
            uint32_t rx_abort_int_dual_en           :  1; /* [   30],        r/w,        0x0 */
            uint32_t tx_abort_int_dual_en           :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_coex_priority;

    /* 0x184  reserved */
    uint8_t RESERVED0x184[124];

    /* 0x200 : m154_tx_mpdu_00 */
    union {
        struct {
            uint32_t tx_mpdu_000                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_001                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_002                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_003                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_00;

    /* 0x204 : m154_tx_mpdu_01 */
    union {
        struct {
            uint32_t tx_mpdu_004                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_005                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_006                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_007                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_01;

    /* 0x208 : m154_tx_mpdu_02 */
    union {
        struct {
            uint32_t tx_mpdu_008                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_009                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_010                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_011                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_02;

    /* 0x20C : m154_tx_mpdu_03 */
    union {
        struct {
            uint32_t tx_mpdu_012                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_013                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_014                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_015                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_03;

    /* 0x210 : m154_tx_mpdu_04 */
    union {
        struct {
            uint32_t tx_mpdu_016                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_017                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_018                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_019                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_04;

    /* 0x214 : m154_tx_mpdu_05 */
    union {
        struct {
            uint32_t tx_mpdu_020                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_021                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_022                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_023                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_05;

    /* 0x218 : m154_tx_mpdu_06 */
    union {
        struct {
            uint32_t tx_mpdu_024                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_025                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_026                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_027                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_06;

    /* 0x21C : m154_tx_mpdu_07 */
    union {
        struct {
            uint32_t tx_mpdu_028                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_029                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_030                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_031                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_07;

    /* 0x220 : m154_tx_mpdu_08 */
    union {
        struct {
            uint32_t tx_mpdu_032                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_033                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_034                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_035                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_08;

    /* 0x224 : m154_tx_mpdu_09 */
    union {
        struct {
            uint32_t tx_mpdu_036                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_037                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_038                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_039                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_09;

    /* 0x228 : m154_tx_mpdu_10 */
    union {
        struct {
            uint32_t tx_mpdu_040                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_041                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_042                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_043                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_10;

    /* 0x22C : m154_tx_mpdu_11 */
    union {
        struct {
            uint32_t tx_mpdu_044                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_045                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_046                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_047                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_11;

    /* 0x230 : m154_tx_mpdu_12 */
    union {
        struct {
            uint32_t tx_mpdu_048                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_049                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_050                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_051                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_12;

    /* 0x234 : m154_tx_mpdu_13 */
    union {
        struct {
            uint32_t tx_mpdu_052                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_053                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_054                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_055                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_13;

    /* 0x238 : m154_tx_mpdu_14 */
    union {
        struct {
            uint32_t tx_mpdu_056                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_057                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_058                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_059                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_14;

    /* 0x23C : m154_tx_mpdu_15 */
    union {
        struct {
            uint32_t tx_mpdu_060                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_061                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_062                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_063                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_15;

    /* 0x240 : m154_tx_mpdu_16 */
    union {
        struct {
            uint32_t tx_mpdu_064                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_065                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_066                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_067                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_16;

    /* 0x244 : m154_tx_mpdu_17 */
    union {
        struct {
            uint32_t tx_mpdu_068                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_069                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_070                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_071                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_17;

    /* 0x248 : m154_tx_mpdu_18 */
    union {
        struct {
            uint32_t tx_mpdu_072                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_073                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_074                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_075                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_18;

    /* 0x24C : m154_tx_mpdu_19 */
    union {
        struct {
            uint32_t tx_mpdu_076                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_077                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_078                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_079                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_19;

    /* 0x250 : m154_tx_mpdu_20 */
    union {
        struct {
            uint32_t tx_mpdu_080                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_081                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_082                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_083                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_20;

    /* 0x254 : m154_tx_mpdu_21 */
    union {
        struct {
            uint32_t tx_mpdu_084                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_085                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_086                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_087                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_21;

    /* 0x258 : m154_tx_mpdu_22 */
    union {
        struct {
            uint32_t tx_mpdu_088                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_089                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_090                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_091                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_22;

    /* 0x25C : m154_tx_mpdu_23 */
    union {
        struct {
            uint32_t tx_mpdu_092                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_093                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_094                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_095                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_23;

    /* 0x260 : m154_tx_mpdu_24 */
    union {
        struct {
            uint32_t tx_mpdu_096                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_097                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_098                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_099                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_24;

    /* 0x264 : m154_tx_mpdu_25 */
    union {
        struct {
            uint32_t tx_mpdu_100                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_101                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_102                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_103                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_25;

    /* 0x268 : m154_tx_mpdu_26 */
    union {
        struct {
            uint32_t tx_mpdu_104                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_105                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_106                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_107                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_26;

    /* 0x26C : m154_tx_mpdu_27 */
    union {
        struct {
            uint32_t tx_mpdu_108                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_109                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_110                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_111                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_27;

    /* 0x270 : m154_tx_mpdu_28 */
    union {
        struct {
            uint32_t tx_mpdu_112                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_113                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_114                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_115                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_28;

    /* 0x274 : m154_tx_mpdu_29 */
    union {
        struct {
            uint32_t tx_mpdu_116                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_117                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_118                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_119                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_29;

    /* 0x278 : m154_tx_mpdu_30 */
    union {
        struct {
            uint32_t tx_mpdu_120                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_121                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_122                    :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_mpdu_123                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_30;

    /* 0x27C : m154_tx_mpdu_31 */
    union {
        struct {
            uint32_t tx_mpdu_124                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t tx_mpdu_fcs_07_00              :  8; /* [15: 8],          r,        0x0 */
            uint32_t tx_mpdu_fcs_15_08              :  8; /* [23:16],          r,        0x0 */
            uint32_t tx_phr                         :  8; /* [31:24],          r,        0x5 */
        }BF;
        uint32_t WORD;
    } m154_tx_mpdu_31;

    /* 0x280 : m154_tx_ppdu_ctrl */
    union {
        struct {
            uint32_t tx_sfd                         :  8; /* [ 7: 0],        r/w,       0xa7 */
            uint32_t tx_mode                        :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_tx_ppdu_ctrl;

    /* 0x284  reserved */
    uint8_t RESERVED0x284[380];

    /* 0x400 : m154_rx_mpdu_00 */
    union {
        struct {
            uint32_t rx_mpdu_000                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_001                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_002                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_003                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_00;

    /* 0x404 : m154_rx_mpdu_01 */
    union {
        struct {
            uint32_t rx_mpdu_004                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_005                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_006                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_007                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_01;

    /* 0x408 : m154_rx_mpdu_02 */
    union {
        struct {
            uint32_t rx_mpdu_008                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_009                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_010                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_011                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_02;

    /* 0x40C : m154_rx_mpdu_03 */
    union {
        struct {
            uint32_t rx_mpdu_012                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_013                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_014                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_015                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_03;

    /* 0x410 : m154_rx_mpdu_04 */
    union {
        struct {
            uint32_t rx_mpdu_016                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_017                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_018                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_019                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_04;

    /* 0x414 : m154_rx_mpdu_05 */
    union {
        struct {
            uint32_t rx_mpdu_020                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_021                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_022                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_023                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_05;

    /* 0x418 : m154_rx_mpdu_06 */
    union {
        struct {
            uint32_t rx_mpdu_024                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_025                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_026                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_027                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_06;

    /* 0x41C : m154_rx_mpdu_07 */
    union {
        struct {
            uint32_t rx_mpdu_028                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_029                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_030                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_031                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_07;

    /* 0x420 : m154_rx_mpdu_08 */
    union {
        struct {
            uint32_t rx_mpdu_032                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_033                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_034                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_035                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_08;

    /* 0x424 : m154_rx_mpdu_09 */
    union {
        struct {
            uint32_t rx_mpdu_036                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_037                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_038                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_039                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_09;

    /* 0x428 : m154_rx_mpdu_10 */
    union {
        struct {
            uint32_t rx_mpdu_040                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_041                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_042                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_043                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_10;

    /* 0x42C : m154_rx_mpdu_11 */
    union {
        struct {
            uint32_t rx_mpdu_044                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_045                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_046                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_047                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_11;

    /* 0x430 : m154_rx_mpdu_12 */
    union {
        struct {
            uint32_t rx_mpdu_048                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_049                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_050                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_051                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_12;

    /* 0x434 : m154_rx_mpdu_13 */
    union {
        struct {
            uint32_t rx_mpdu_052                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_053                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_054                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_055                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_13;

    /* 0x438 : m154_rx_mpdu_14 */
    union {
        struct {
            uint32_t rx_mpdu_056                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_057                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_058                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_059                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_14;

    /* 0x43C : m154_rx_mpdu_15 */
    union {
        struct {
            uint32_t rx_mpdu_060                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_061                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_062                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_063                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_15;

    /* 0x440 : m154_rx_mpdu_16 */
    union {
        struct {
            uint32_t rx_mpdu_064                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_065                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_066                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_067                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_16;

    /* 0x444 : m154_rx_mpdu_17 */
    union {
        struct {
            uint32_t rx_mpdu_068                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_069                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_070                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_071                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_17;

    /* 0x448 : m154_rx_mpdu_18 */
    union {
        struct {
            uint32_t rx_mpdu_072                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_073                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_074                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_075                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_18;

    /* 0x44C : m154_rx_mpdu_19 */
    union {
        struct {
            uint32_t rx_mpdu_076                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_077                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_078                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_079                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_19;

    /* 0x450 : m154_rx_mpdu_20 */
    union {
        struct {
            uint32_t rx_mpdu_080                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_081                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_082                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_083                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_20;

    /* 0x454 : m154_rx_mpdu_21 */
    union {
        struct {
            uint32_t rx_mpdu_084                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_085                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_086                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_087                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_21;

    /* 0x458 : m154_rx_mpdu_22 */
    union {
        struct {
            uint32_t rx_mpdu_088                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_089                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_090                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_091                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_22;

    /* 0x45C : m154_rx_mpdu_23 */
    union {
        struct {
            uint32_t rx_mpdu_092                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_093                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_094                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_095                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_23;

    /* 0x460 : m154_rx_mpdu_24 */
    union {
        struct {
            uint32_t rx_mpdu_096                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_097                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_098                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_099                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_24;

    /* 0x464 : m154_rx_mpdu_25 */
    union {
        struct {
            uint32_t rx_mpdu_100                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_101                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_102                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_103                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_25;

    /* 0x468 : m154_rx_mpdu_26 */
    union {
        struct {
            uint32_t rx_mpdu_104                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_105                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_106                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_107                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_26;

    /* 0x46C : m154_rx_mpdu_27 */
    union {
        struct {
            uint32_t rx_mpdu_108                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_109                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_110                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_111                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_27;

    /* 0x470 : m154_rx_mpdu_28 */
    union {
        struct {
            uint32_t rx_mpdu_112                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_113                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_114                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_115                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_28;

    /* 0x474 : m154_rx_mpdu_29 */
    union {
        struct {
            uint32_t rx_mpdu_116                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_117                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_118                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_119                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_29;

    /* 0x478 : m154_rx_mpdu_30 */
    union {
        struct {
            uint32_t rx_mpdu_120                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_121                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_122                    :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_mpdu_123                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_30;

    /* 0x47C : m154_rx_mpdu_31 */
    union {
        struct {
            uint32_t rx_mpdu_124                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t rx_mpdu_fcs_07_00              :  8; /* [15: 8],          r,        0x0 */
            uint32_t rx_mpdu_fcs_15_08              :  8; /* [23:16],          r,        0x0 */
            uint32_t rx_phr                         :  8; /* [31:24],          r,        0x5 */
        }BF;
        uint32_t WORD;
    } m154_rx_mpdu_31;

    /* 0x480 : m154_rx_ppdu_status */
    union {
        struct {
            uint32_t rx_mode                        :  2; /* [ 1: 0],          r,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_rx_ppdu_status;

    /* 0x484  reserved */
    uint8_t RESERVED0x484[128];

    /* 0x504 : m154_nbr_ctrl_0 */
    union {
        struct {
            uint32_t efp_en                         :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t nbr_result_fp_valid            :  1; /* [    2],          r,        0x0 */
            uint32_t nbr_result_fp                  :  1; /* [    3],          r,        0x0 */
            uint32_t nbr_result_fp_idx              :  7; /* [10: 4],          r,        0x0 */
            uint32_t nbr_search_done                :  1; /* [   11],          r,        0x0 */
            uint32_t reserved_12_23                 : 12; /* [23:12],       rsvd,        0x0 */
            uint32_t rx_tx_transition_time_cnt      :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_ctrl_0;

    /* 0x508  reserved */
    uint8_t RESERVED0x508[248];

    /* 0x600 : m154_nbr_valid_31_00 */
    union {
        struct {
            uint32_t nbr_valid_31_00                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_valid_31_00;

    /* 0x604 : m154_nbr_valid_63_32 */
    union {
        struct {
            uint32_t nbr_valid_63_32                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_valid_63_32;

    /* 0x608 : m154_nbr_valid_95_64 */
    union {
        struct {
            uint32_t nbr_valid_95_64                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_valid_95_64;

    /* 0x60C : m154_nbr_valid_127_96 */
    union {
        struct {
            uint32_t nbr_valid_127_96               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_valid_127_96;

    /* 0x610 : m154_nbr_ext_addr_mode_31_00 */
    union {
        struct {
            uint32_t nbr_ext_addr_mode_31_00        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_ext_addr_mode_31_00;

    /* 0x614 : m154_nbr_ext_addr_mode_63_32 */
    union {
        struct {
            uint32_t nbr_ext_addr_mode_63_32        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_ext_addr_mode_63_32;

    /* 0x618 : m154_nbr_ext_addr_mode_95_64 */
    union {
        struct {
            uint32_t nbr_ext_addr_mode_95_64        : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_ext_addr_mode_95_64;

    /* 0x61C : m154_nbr_ext_addr_mode_127_96 */
    union {
        struct {
            uint32_t nbr_ext_addr_mode_127_96       : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_ext_addr_mode_127_96;

    /* 0x620 : m154_nbr_fp_31_00 */
    union {
        struct {
            uint32_t nbr_fp_31_00                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_fp_31_00;

    /* 0x624 : m154_nbr_fp_63_32 */
    union {
        struct {
            uint32_t nbr_fp_63_32                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_fp_63_32;

    /* 0x628 : m154_nbr_fp_95_64 */
    union {
        struct {
            uint32_t nbr_fp_95_64                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_fp_95_64;

    /* 0x62C : m154_nbr_fp_127_96 */
    union {
        struct {
            uint32_t nbr_fp_127_96                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_fp_127_96;

    /* 0x630 : m154_nbr_addr_01_00 */
    union {
        struct {
            uint32_t nbr_addr_00                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_01                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_01_00;

    /* 0x634 : m154_nbr_addr_03_02 */
    union {
        struct {
            uint32_t nbr_addr_02                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_03                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_03_02;

    /* 0x638 : m154_nbr_addr_05_04 */
    union {
        struct {
            uint32_t nbr_addr_04                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_05                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_05_04;

    /* 0x63C : m154_nbr_addr_07_06 */
    union {
        struct {
            uint32_t nbr_addr_06                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_07                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_07_06;

    /* 0x640 : m154_nbr_addr_09_08 */
    union {
        struct {
            uint32_t nbr_addr_08                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_09                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_09_08;

    /* 0x644 : m154_nbr_addr_11_10 */
    union {
        struct {
            uint32_t nbr_addr_10                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_11                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_11_10;

    /* 0x648 : m154_nbr_addr_13_12 */
    union {
        struct {
            uint32_t nbr_addr_12                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_13                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_13_12;

    /* 0x64C : m154_nbr_addr_15_14 */
    union {
        struct {
            uint32_t nbr_addr_14                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_15                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_15_14;

    /* 0x650 : m154_nbr_addr_17_16 */
    union {
        struct {
            uint32_t nbr_addr_16                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_17                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_17_16;

    /* 0x654 : m154_nbr_addr_19_18 */
    union {
        struct {
            uint32_t nbr_addr_18                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_19                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_19_18;

    /* 0x658 : m154_nbr_addr_21_20 */
    union {
        struct {
            uint32_t nbr_addr_20                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_21                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_21_20;

    /* 0x65C : m154_nbr_addr_23_22 */
    union {
        struct {
            uint32_t nbr_addr_22                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_23                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_23_22;

    /* 0x660 : m154_nbr_addr_25_24 */
    union {
        struct {
            uint32_t nbr_addr_24                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_25                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_25_24;

    /* 0x664 : m154_nbr_addr_27_26 */
    union {
        struct {
            uint32_t nbr_addr_26                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_27                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_27_26;

    /* 0x668 : m154_nbr_addr_29_28 */
    union {
        struct {
            uint32_t nbr_addr_28                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_29                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_29_28;

    /* 0x66C : m154_nbr_addr_31_30 */
    union {
        struct {
            uint32_t nbr_addr_30                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_31                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_31_30;

    /* 0x670 : m154_nbr_addr_33_32 */
    union {
        struct {
            uint32_t nbr_addr_32                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_33                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_33_32;

    /* 0x674 : m154_nbr_addr_35_34 */
    union {
        struct {
            uint32_t nbr_addr_34                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_35                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_35_34;

    /* 0x678 : m154_nbr_addr_37_36 */
    union {
        struct {
            uint32_t nbr_addr_36                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_37                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_37_36;

    /* 0x67C : m154_nbr_addr_39_38 */
    union {
        struct {
            uint32_t nbr_addr_38                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_39                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_39_38;

    /* 0x680 : m154_nbr_addr_41_40 */
    union {
        struct {
            uint32_t nbr_addr_40                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_41                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_41_40;

    /* 0x684 : m154_nbr_addr_43_42 */
    union {
        struct {
            uint32_t nbr_addr_42                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_43                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_43_42;

    /* 0x688 : m154_nbr_addr_45_44 */
    union {
        struct {
            uint32_t nbr_addr_44                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_45                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_45_44;

    /* 0x68C : m154_nbr_addr_47_46 */
    union {
        struct {
            uint32_t nbr_addr_46                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_47                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_47_46;

    /* 0x690 : m154_nbr_addr_49_48 */
    union {
        struct {
            uint32_t nbr_addr_48                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_49                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_49_48;

    /* 0x694 : m154_nbr_addr_51_50 */
    union {
        struct {
            uint32_t nbr_addr_50                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_51                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_51_50;

    /* 0x698 : m154_nbr_addr_53_52 */
    union {
        struct {
            uint32_t nbr_addr_52                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_53                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_53_52;

    /* 0x69C : m154_nbr_addr_55_54 */
    union {
        struct {
            uint32_t nbr_addr_54                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_55                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_55_54;

    /* 0x6A0 : m154_nbr_addr_57_56 */
    union {
        struct {
            uint32_t nbr_addr_56                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_57                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_57_56;

    /* 0x6A4 : m154_nbr_addr_59_58 */
    union {
        struct {
            uint32_t nbr_addr_58                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_59                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_59_58;

    /* 0x6A8 : m154_nbr_addr_61_60 */
    union {
        struct {
            uint32_t nbr_addr_60                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_61                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_61_60;

    /* 0x6AC : m154_nbr_addr_63_62 */
    union {
        struct {
            uint32_t nbr_addr_62                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_63                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_63_62;

    /* 0x6B0 : m154_nbr_addr_65_64 */
    union {
        struct {
            uint32_t nbr_addr_64                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_65                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_65_64;

    /* 0x6B4 : m154_nbr_addr_67_66 */
    union {
        struct {
            uint32_t nbr_addr_66                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_67                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_67_66;

    /* 0x6B8 : m154_nbr_addr_69_68 */
    union {
        struct {
            uint32_t nbr_addr_68                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_69                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_69_68;

    /* 0x6BC : m154_nbr_addr_71_70 */
    union {
        struct {
            uint32_t nbr_addr_70                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_71                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_71_70;

    /* 0x6C0 : m154_nbr_addr_73_72 */
    union {
        struct {
            uint32_t nbr_addr_72                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_73                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_73_72;

    /* 0x6C4 : m154_nbr_addr_75_74 */
    union {
        struct {
            uint32_t nbr_addr_74                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_75                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_75_74;

    /* 0x6C8 : m154_nbr_addr_77_76 */
    union {
        struct {
            uint32_t nbr_addr_76                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_77                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_77_76;

    /* 0x6CC : m154_nbr_addr_79_78 */
    union {
        struct {
            uint32_t nbr_addr_78                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_79                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_79_78;

    /* 0x6D0 : m154_nbr_addr_81_80 */
    union {
        struct {
            uint32_t nbr_addr_80                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_81                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_81_80;

    /* 0x6D4 : m154_nbr_addr_83_82 */
    union {
        struct {
            uint32_t nbr_addr_82                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_83                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_83_82;

    /* 0x6D8 : m154_nbr_addr_85_84 */
    union {
        struct {
            uint32_t nbr_addr_84                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_85                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_85_84;

    /* 0x6DC : m154_nbr_addr_87_86 */
    union {
        struct {
            uint32_t nbr_addr_86                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_87                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_87_86;

    /* 0x6E0 : m154_nbr_addr_89_88 */
    union {
        struct {
            uint32_t nbr_addr_88                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_89                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_89_88;

    /* 0x6E4 : m154_nbr_addr_91_90 */
    union {
        struct {
            uint32_t nbr_addr_90                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_91                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_91_90;

    /* 0x6E8 : m154_nbr_addr_93_92 */
    union {
        struct {
            uint32_t nbr_addr_92                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_93                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_93_92;

    /* 0x6EC : m154_nbr_addr_95_94 */
    union {
        struct {
            uint32_t nbr_addr_94                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_95                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_95_94;

    /* 0x6F0 : m154_nbr_addr_97_96 */
    union {
        struct {
            uint32_t nbr_addr_96                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_97                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_97_96;

    /* 0x6F4 : m154_nbr_addr_99_98 */
    union {
        struct {
            uint32_t nbr_addr_98                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_99                    : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_99_98;

    /* 0x6F8 : m154_nbr_addr_101_100 */
    union {
        struct {
            uint32_t nbr_addr_100                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_101                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_101_100;

    /* 0x6FC : m154_nbr_addr_103_102 */
    union {
        struct {
            uint32_t nbr_addr_102                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_103                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_103_102;

    /* 0x700 : m154_nbr_addr_105_104 */
    union {
        struct {
            uint32_t nbr_addr_104                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_105                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_105_104;

    /* 0x704 : m154_nbr_addr_107_106 */
    union {
        struct {
            uint32_t nbr_addr_106                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_107                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_107_106;

    /* 0x708 : m154_nbr_addr_109_108 */
    union {
        struct {
            uint32_t nbr_addr_108                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_109                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_109_108;

    /* 0x70C : m154_nbr_addr_111_110 */
    union {
        struct {
            uint32_t nbr_addr_110                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_111                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_111_110;

    /* 0x710 : m154_nbr_addr_113_112 */
    union {
        struct {
            uint32_t nbr_addr_112                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_113                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_113_112;

    /* 0x714 : m154_nbr_addr_115_114 */
    union {
        struct {
            uint32_t nbr_addr_114                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_115                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_115_114;

    /* 0x718 : m154_nbr_addr_117_116 */
    union {
        struct {
            uint32_t nbr_addr_116                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_117                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_117_116;

    /* 0x71C : m154_nbr_addr_119_118 */
    union {
        struct {
            uint32_t nbr_addr_118                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_119                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_119_118;

    /* 0x720 : m154_nbr_addr_121_120 */
    union {
        struct {
            uint32_t nbr_addr_120                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_121                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_121_120;

    /* 0x724 : m154_nbr_addr_123_122 */
    union {
        struct {
            uint32_t nbr_addr_122                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_123                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_123_122;

    /* 0x728 : m154_nbr_addr_125_124 */
    union {
        struct {
            uint32_t nbr_addr_124                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_125                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_125_124;

    /* 0x72C : m154_nbr_addr_127_126 */
    union {
        struct {
            uint32_t nbr_addr_126                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t nbr_addr_127                   : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_nbr_addr_127_126;

    /* 0x730  reserved */
    uint8_t RESERVED0x730[64];

    /* 0x770 : m154_lb_mpdu_mismatch_31_00 */
    union {
        struct {
            uint32_t lb_mismatch_31_downto_0        : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_mismatch_31_00;

    /* 0x774 : m154_lb_mpdu_mismatch_63_32 */
    union {
        struct {
            uint32_t lb_mismatch_63_downto_32       : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_mismatch_63_32;

    /* 0x778 : m154_lb_mpdu_mismatch_95_64 */
    union {
        struct {
            uint32_t lb_mismatch_95_downto_64       : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_mismatch_95_64;

    /* 0x77C : m154_lb_mpdu_mismatch_127_96 */
    union {
        struct {
            uint32_t lb_mismatch_127_downto_96      : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_mismatch_127_96;

    /* 0x780 : m154_lb_mpdu_00 */
    union {
        struct {
            uint32_t lb_mpdu_000                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_001                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_002                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_003                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_00;

    /* 0x784 : m154_lb_mpdu_01 */
    union {
        struct {
            uint32_t lb_mpdu_004                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_005                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_006                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_007                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_01;

    /* 0x788 : m154_lb_mpdu_02 */
    union {
        struct {
            uint32_t lb_mpdu_008                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_009                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_010                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_011                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_02;

    /* 0x78C : m154_lb_mpdu_03 */
    union {
        struct {
            uint32_t lb_mpdu_012                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_013                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_014                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_015                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_03;

    /* 0x790 : m154_lb_mpdu_04 */
    union {
        struct {
            uint32_t lb_mpdu_016                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_017                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_018                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_019                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_04;

    /* 0x794 : m154_lb_mpdu_05 */
    union {
        struct {
            uint32_t lb_mpdu_020                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_021                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_022                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_023                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_05;

    /* 0x798 : m154_lb_mpdu_06 */
    union {
        struct {
            uint32_t lb_mpdu_024                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_025                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_026                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_027                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_06;

    /* 0x79C : m154_lb_mpdu_07 */
    union {
        struct {
            uint32_t lb_mpdu_028                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_029                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_030                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_031                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_07;

    /* 0x7A0 : m154_lb_mpdu_08 */
    union {
        struct {
            uint32_t lb_mpdu_032                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_033                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_034                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_035                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_08;

    /* 0x7A4 : m154_lb_mpdu_09 */
    union {
        struct {
            uint32_t lb_mpdu_036                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_037                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_038                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_039                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_09;

    /* 0x7A8 : m154_lb_mpdu_10 */
    union {
        struct {
            uint32_t lb_mpdu_040                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_041                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_042                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_043                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_10;

    /* 0x7AC : m154_lb_mpdu_11 */
    union {
        struct {
            uint32_t lb_mpdu_044                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_045                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_046                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_047                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_11;

    /* 0x7B0 : m154_lb_mpdu_12 */
    union {
        struct {
            uint32_t lb_mpdu_048                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_049                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_050                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_051                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_12;

    /* 0x7B4 : m154_lb_mpdu_13 */
    union {
        struct {
            uint32_t lb_mpdu_052                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_053                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_054                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_055                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_13;

    /* 0x7B8 : m154_lb_mpdu_14 */
    union {
        struct {
            uint32_t lb_mpdu_056                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_057                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_058                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_059                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_14;

    /* 0x7BC : m154_lb_mpdu_15 */
    union {
        struct {
            uint32_t lb_mpdu_060                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_061                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_062                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_063                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_15;

    /* 0x7C0 : m154_lb_mpdu_16 */
    union {
        struct {
            uint32_t lb_mpdu_064                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_065                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_066                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_067                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_16;

    /* 0x7C4 : m154_lb_mpdu_17 */
    union {
        struct {
            uint32_t lb_mpdu_068                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_069                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_070                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_071                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_17;

    /* 0x7C8 : m154_lb_mpdu_18 */
    union {
        struct {
            uint32_t lb_mpdu_072                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_073                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_074                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_075                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_18;

    /* 0x7CC : m154_lb_mpdu_19 */
    union {
        struct {
            uint32_t lb_mpdu_076                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_077                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_078                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_079                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_19;

    /* 0x7D0 : m154_lb_mpdu_20 */
    union {
        struct {
            uint32_t lb_mpdu_080                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_081                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_082                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_083                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_20;

    /* 0x7D4 : m154_lb_mpdu_21 */
    union {
        struct {
            uint32_t lb_mpdu_084                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_085                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_086                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_087                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_21;

    /* 0x7D8 : m154_lb_mpdu_22 */
    union {
        struct {
            uint32_t lb_mpdu_088                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_089                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_090                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_091                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_22;

    /* 0x7DC : m154_lb_mpdu_23 */
    union {
        struct {
            uint32_t lb_mpdu_092                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_093                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_094                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_095                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_23;

    /* 0x7E0 : m154_lb_mpdu_24 */
    union {
        struct {
            uint32_t lb_mpdu_096                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_097                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_098                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_099                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_24;

    /* 0x7E4 : m154_lb_mpdu_25 */
    union {
        struct {
            uint32_t lb_mpdu_100                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_101                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_102                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_103                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_25;

    /* 0x7E8 : m154_lb_mpdu_26 */
    union {
        struct {
            uint32_t lb_mpdu_104                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_105                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_106                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_107                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_26;

    /* 0x7EC : m154_lb_mpdu_27 */
    union {
        struct {
            uint32_t lb_mpdu_108                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_109                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_110                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_111                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_27;

    /* 0x7F0 : m154_lb_mpdu_28 */
    union {
        struct {
            uint32_t lb_mpdu_112                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_113                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_114                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_115                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_28;

    /* 0x7F4 : m154_lb_mpdu_29 */
    union {
        struct {
            uint32_t lb_mpdu_116                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_117                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_118                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_119                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_29;

    /* 0x7F8 : m154_lb_mpdu_30 */
    union {
        struct {
            uint32_t lb_mpdu_120                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_121                    :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_122                    :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_mpdu_123                    :  8; /* [31:24],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_30;

    /* 0x7FC : m154_lb_mpdu_31 */
    union {
        struct {
            uint32_t lb_mpdu_124                    :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t lb_mpdu_fcs_07_00              :  8; /* [15: 8],          r,        0x0 */
            uint32_t lb_mpdu_fcs_15_08              :  8; /* [23:16],          r,        0x0 */
            uint32_t lb_phr                         :  8; /* [31:24],          r,        0x5 */
        }BF;
        uint32_t WORD;
    } m154_lb_mpdu_31;

};

typedef volatile struct m154_reg m154_reg_t;


#endif  /* __M154_REG_H__ */
