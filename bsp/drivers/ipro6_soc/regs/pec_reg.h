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


#ifndef  __PEC_REG_H__
#define  __PEC_REG_H__

#include "ipro6.h"

/* 0x0 : PEC control register */
#define PEC_CTRL_OFFSET                                         (0x0)
#define PEC_CR_SM_EN                                            PEC_CR_SM_EN
#define PEC_CR_SM_EN_POS                                        (0U)
#define PEC_CR_SM_EN_LEN                                        (4U)
#define PEC_CR_SM_EN_MSK                                        (((1U<<PEC_CR_SM_EN_LEN)-1)<<PEC_CR_SM_EN_POS)
#define PEC_CR_SM_EN_UMSK                                       (~(((1U<<PEC_CR_SM_EN_LEN)-1)<<PEC_CR_SM_EN_POS))
#define PEC_CR_SM_RESET                                         PEC_CR_SM_RESET
#define PEC_CR_SM_RESET_POS                                     (4U)
#define PEC_CR_SM_RESET_LEN                                     (4U)
#define PEC_CR_SM_RESET_MSK                                     (((1U<<PEC_CR_SM_RESET_LEN)-1)<<PEC_CR_SM_RESET_POS)
#define PEC_CR_SM_RESET_UMSK                                    (~(((1U<<PEC_CR_SM_RESET_LEN)-1)<<PEC_CR_SM_RESET_POS))
#define PEC_CR_CLKDIV_RESET                                     PEC_CR_CLKDIV_RESET
#define PEC_CR_CLKDIV_RESET_POS                                 (8U)
#define PEC_CR_CLKDIV_RESET_LEN                                 (4U)
#define PEC_CR_CLKDIV_RESET_MSK                                 (((1U<<PEC_CR_CLKDIV_RESET_LEN)-1)<<PEC_CR_CLKDIV_RESET_POS)
#define PEC_CR_CLKDIV_RESET_UMSK                                (~(((1U<<PEC_CR_CLKDIV_RESET_LEN)-1)<<PEC_CR_CLKDIV_RESET_POS))
#define PEC_CR_SM0_PC_ORIGIN                                    PEC_CR_SM0_PC_ORIGIN
#define PEC_CR_SM0_PC_ORIGIN_POS                                (12U)
#define PEC_CR_SM0_PC_ORIGIN_LEN                                (5U)
#define PEC_CR_SM0_PC_ORIGIN_MSK                                (((1U<<PEC_CR_SM0_PC_ORIGIN_LEN)-1)<<PEC_CR_SM0_PC_ORIGIN_POS)
#define PEC_CR_SM0_PC_ORIGIN_UMSK                               (~(((1U<<PEC_CR_SM0_PC_ORIGIN_LEN)-1)<<PEC_CR_SM0_PC_ORIGIN_POS))
#define PEC_CR_SM1_PC_ORIGIN                                    PEC_CR_SM1_PC_ORIGIN
#define PEC_CR_SM1_PC_ORIGIN_POS                                (17U)
#define PEC_CR_SM1_PC_ORIGIN_LEN                                (5U)
#define PEC_CR_SM1_PC_ORIGIN_MSK                                (((1U<<PEC_CR_SM1_PC_ORIGIN_LEN)-1)<<PEC_CR_SM1_PC_ORIGIN_POS)
#define PEC_CR_SM1_PC_ORIGIN_UMSK                               (~(((1U<<PEC_CR_SM1_PC_ORIGIN_LEN)-1)<<PEC_CR_SM1_PC_ORIGIN_POS))
#define PEC_CR_SM2_PC_ORIGIN                                    PEC_CR_SM2_PC_ORIGIN
#define PEC_CR_SM2_PC_ORIGIN_POS                                (22U)
#define PEC_CR_SM2_PC_ORIGIN_LEN                                (5U)
#define PEC_CR_SM2_PC_ORIGIN_MSK                                (((1U<<PEC_CR_SM2_PC_ORIGIN_LEN)-1)<<PEC_CR_SM2_PC_ORIGIN_POS)
#define PEC_CR_SM2_PC_ORIGIN_UMSK                               (~(((1U<<PEC_CR_SM2_PC_ORIGIN_LEN)-1)<<PEC_CR_SM2_PC_ORIGIN_POS))
#define PEC_CR_SM3_PC_ORIGIN                                    PEC_CR_SM3_PC_ORIGIN
#define PEC_CR_SM3_PC_ORIGIN_POS                                (27U)
#define PEC_CR_SM3_PC_ORIGIN_LEN                                (5U)
#define PEC_CR_SM3_PC_ORIGIN_MSK                                (((1U<<PEC_CR_SM3_PC_ORIGIN_LEN)-1)<<PEC_CR_SM3_PC_ORIGIN_POS)
#define PEC_CR_SM3_PC_ORIGIN_UMSK                               (~(((1U<<PEC_CR_SM3_PC_ORIGIN_LEN)-1)<<PEC_CR_SM3_PC_ORIGIN_POS))

/* 0x4 : FIFO status register */
#define PEC_FSTAT_OFFSET                                        (0x4)
#define PEC_ST_RXFULL                                           PEC_ST_RXFULL
#define PEC_ST_RXFULL_POS                                       (0U)
#define PEC_ST_RXFULL_LEN                                       (4U)
#define PEC_ST_RXFULL_MSK                                       (((1U<<PEC_ST_RXFULL_LEN)-1)<<PEC_ST_RXFULL_POS)
#define PEC_ST_RXFULL_UMSK                                      (~(((1U<<PEC_ST_RXFULL_LEN)-1)<<PEC_ST_RXFULL_POS))
#define PEC_ST_RXEMPTY                                          PEC_ST_RXEMPTY
#define PEC_ST_RXEMPTY_POS                                      (8U)
#define PEC_ST_RXEMPTY_LEN                                      (4U)
#define PEC_ST_RXEMPTY_MSK                                      (((1U<<PEC_ST_RXEMPTY_LEN)-1)<<PEC_ST_RXEMPTY_POS)
#define PEC_ST_RXEMPTY_UMSK                                     (~(((1U<<PEC_ST_RXEMPTY_LEN)-1)<<PEC_ST_RXEMPTY_POS))
#define PEC_ST_TXFULL                                           PEC_ST_TXFULL
#define PEC_ST_TXFULL_POS                                       (16U)
#define PEC_ST_TXFULL_LEN                                       (4U)
#define PEC_ST_TXFULL_MSK                                       (((1U<<PEC_ST_TXFULL_LEN)-1)<<PEC_ST_TXFULL_POS)
#define PEC_ST_TXFULL_UMSK                                      (~(((1U<<PEC_ST_TXFULL_LEN)-1)<<PEC_ST_TXFULL_POS))
#define PEC_ST_TXEMPTY                                          PEC_ST_TXEMPTY
#define PEC_ST_TXEMPTY_POS                                      (24U)
#define PEC_ST_TXEMPTY_LEN                                      (4U)
#define PEC_ST_TXEMPTY_MSK                                      (((1U<<PEC_ST_TXEMPTY_LEN)-1)<<PEC_ST_TXEMPTY_POS)
#define PEC_ST_TXEMPTY_UMSK                                     (~(((1U<<PEC_ST_TXEMPTY_LEN)-1)<<PEC_ST_TXEMPTY_POS))

/* 0x8 : FIFO debug register */
#define PEC_FDEBUG_OFFSET                                       (0x8)
#define PEC_CR_RXSTALL_CLR                                      PEC_CR_RXSTALL_CLR
#define PEC_CR_RXSTALL_CLR_POS                                  (0U)
#define PEC_CR_RXSTALL_CLR_LEN                                  (4U)
#define PEC_CR_RXSTALL_CLR_MSK                                  (((1U<<PEC_CR_RXSTALL_CLR_LEN)-1)<<PEC_CR_RXSTALL_CLR_POS)
#define PEC_CR_RXSTALL_CLR_UMSK                                 (~(((1U<<PEC_CR_RXSTALL_CLR_LEN)-1)<<PEC_CR_RXSTALL_CLR_POS))
#define PEC_ST_RXSTALL                                          PEC_ST_RXSTALL
#define PEC_ST_RXSTALL_POS                                      (4U)
#define PEC_ST_RXSTALL_LEN                                      (4U)
#define PEC_ST_RXSTALL_MSK                                      (((1U<<PEC_ST_RXSTALL_LEN)-1)<<PEC_ST_RXSTALL_POS)
#define PEC_ST_RXSTALL_UMSK                                     (~(((1U<<PEC_ST_RXSTALL_LEN)-1)<<PEC_ST_RXSTALL_POS))
#define PEC_CR_RXUNDER_CLR                                      PEC_CR_RXUNDER_CLR
#define PEC_CR_RXUNDER_CLR_POS                                  (8U)
#define PEC_CR_RXUNDER_CLR_LEN                                  (4U)
#define PEC_CR_RXUNDER_CLR_MSK                                  (((1U<<PEC_CR_RXUNDER_CLR_LEN)-1)<<PEC_CR_RXUNDER_CLR_POS)
#define PEC_CR_RXUNDER_CLR_UMSK                                 (~(((1U<<PEC_CR_RXUNDER_CLR_LEN)-1)<<PEC_CR_RXUNDER_CLR_POS))
#define PEC_ST_RXUNDER                                          PEC_ST_RXUNDER
#define PEC_ST_RXUNDER_POS                                      (12U)
#define PEC_ST_RXUNDER_LEN                                      (4U)
#define PEC_ST_RXUNDER_MSK                                      (((1U<<PEC_ST_RXUNDER_LEN)-1)<<PEC_ST_RXUNDER_POS)
#define PEC_ST_RXUNDER_UMSK                                     (~(((1U<<PEC_ST_RXUNDER_LEN)-1)<<PEC_ST_RXUNDER_POS))
#define PEC_CR_TXOVER_CLR                                       PEC_CR_TXOVER_CLR
#define PEC_CR_TXOVER_CLR_POS                                   (16U)
#define PEC_CR_TXOVER_CLR_LEN                                   (4U)
#define PEC_CR_TXOVER_CLR_MSK                                   (((1U<<PEC_CR_TXOVER_CLR_LEN)-1)<<PEC_CR_TXOVER_CLR_POS)
#define PEC_CR_TXOVER_CLR_UMSK                                  (~(((1U<<PEC_CR_TXOVER_CLR_LEN)-1)<<PEC_CR_TXOVER_CLR_POS))
#define PEC_ST_TXOVER                                           PEC_ST_TXOVER
#define PEC_ST_TXOVER_POS                                       (20U)
#define PEC_ST_TXOVER_LEN                                       (4U)
#define PEC_ST_TXOVER_MSK                                       (((1U<<PEC_ST_TXOVER_LEN)-1)<<PEC_ST_TXOVER_POS)
#define PEC_ST_TXOVER_UMSK                                      (~(((1U<<PEC_ST_TXOVER_LEN)-1)<<PEC_ST_TXOVER_POS))
#define PEC_CR_TXSTALL_CLR                                      PEC_CR_TXSTALL_CLR
#define PEC_CR_TXSTALL_CLR_POS                                  (24U)
#define PEC_CR_TXSTALL_CLR_LEN                                  (4U)
#define PEC_CR_TXSTALL_CLR_MSK                                  (((1U<<PEC_CR_TXSTALL_CLR_LEN)-1)<<PEC_CR_TXSTALL_CLR_POS)
#define PEC_CR_TXSTALL_CLR_UMSK                                 (~(((1U<<PEC_CR_TXSTALL_CLR_LEN)-1)<<PEC_CR_TXSTALL_CLR_POS))
#define PEC_ST_TXSTALL                                          PEC_ST_TXSTALL
#define PEC_ST_TXSTALL_POS                                      (28U)
#define PEC_ST_TXSTALL_LEN                                      (4U)
#define PEC_ST_TXSTALL_MSK                                      (((1U<<PEC_ST_TXSTALL_LEN)-1)<<PEC_ST_TXSTALL_POS)
#define PEC_ST_TXSTALL_UMSK                                     (~(((1U<<PEC_ST_TXSTALL_LEN)-1)<<PEC_ST_TXSTALL_POS))

/* 0xC : FIFO levels */
#define PEC_FLEVEL_OFFSET                                       (0xC)
#define PEC_ST_SM0_TXLEVEL                                      PEC_ST_SM0_TXLEVEL
#define PEC_ST_SM0_TXLEVEL_POS                                  (0U)
#define PEC_ST_SM0_TXLEVEL_LEN                                  (4U)
#define PEC_ST_SM0_TXLEVEL_MSK                                  (((1U<<PEC_ST_SM0_TXLEVEL_LEN)-1)<<PEC_ST_SM0_TXLEVEL_POS)
#define PEC_ST_SM0_TXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM0_TXLEVEL_LEN)-1)<<PEC_ST_SM0_TXLEVEL_POS))
#define PEC_ST_SM0_RXLEVEL                                      PEC_ST_SM0_RXLEVEL
#define PEC_ST_SM0_RXLEVEL_POS                                  (4U)
#define PEC_ST_SM0_RXLEVEL_LEN                                  (4U)
#define PEC_ST_SM0_RXLEVEL_MSK                                  (((1U<<PEC_ST_SM0_RXLEVEL_LEN)-1)<<PEC_ST_SM0_RXLEVEL_POS)
#define PEC_ST_SM0_RXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM0_RXLEVEL_LEN)-1)<<PEC_ST_SM0_RXLEVEL_POS))
#define PEC_ST_SM1_TXLEVEL                                      PEC_ST_SM1_TXLEVEL
#define PEC_ST_SM1_TXLEVEL_POS                                  (8U)
#define PEC_ST_SM1_TXLEVEL_LEN                                  (4U)
#define PEC_ST_SM1_TXLEVEL_MSK                                  (((1U<<PEC_ST_SM1_TXLEVEL_LEN)-1)<<PEC_ST_SM1_TXLEVEL_POS)
#define PEC_ST_SM1_TXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM1_TXLEVEL_LEN)-1)<<PEC_ST_SM1_TXLEVEL_POS))
#define PEC_ST_SM1_RXLEVEL                                      PEC_ST_SM1_RXLEVEL
#define PEC_ST_SM1_RXLEVEL_POS                                  (12U)
#define PEC_ST_SM1_RXLEVEL_LEN                                  (4U)
#define PEC_ST_SM1_RXLEVEL_MSK                                  (((1U<<PEC_ST_SM1_RXLEVEL_LEN)-1)<<PEC_ST_SM1_RXLEVEL_POS)
#define PEC_ST_SM1_RXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM1_RXLEVEL_LEN)-1)<<PEC_ST_SM1_RXLEVEL_POS))
#define PEC_ST_SM2_TXLEVEL                                      PEC_ST_SM2_TXLEVEL
#define PEC_ST_SM2_TXLEVEL_POS                                  (16U)
#define PEC_ST_SM2_TXLEVEL_LEN                                  (4U)
#define PEC_ST_SM2_TXLEVEL_MSK                                  (((1U<<PEC_ST_SM2_TXLEVEL_LEN)-1)<<PEC_ST_SM2_TXLEVEL_POS)
#define PEC_ST_SM2_TXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM2_TXLEVEL_LEN)-1)<<PEC_ST_SM2_TXLEVEL_POS))
#define PEC_ST_SM2_RXLEVEL                                      PEC_ST_SM2_RXLEVEL
#define PEC_ST_SM2_RXLEVEL_POS                                  (20U)
#define PEC_ST_SM2_RXLEVEL_LEN                                  (4U)
#define PEC_ST_SM2_RXLEVEL_MSK                                  (((1U<<PEC_ST_SM2_RXLEVEL_LEN)-1)<<PEC_ST_SM2_RXLEVEL_POS)
#define PEC_ST_SM2_RXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM2_RXLEVEL_LEN)-1)<<PEC_ST_SM2_RXLEVEL_POS))
#define PEC_ST_SM3_TXLEVEL                                      PEC_ST_SM3_TXLEVEL
#define PEC_ST_SM3_TXLEVEL_POS                                  (24U)
#define PEC_ST_SM3_TXLEVEL_LEN                                  (4U)
#define PEC_ST_SM3_TXLEVEL_MSK                                  (((1U<<PEC_ST_SM3_TXLEVEL_LEN)-1)<<PEC_ST_SM3_TXLEVEL_POS)
#define PEC_ST_SM3_TXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM3_TXLEVEL_LEN)-1)<<PEC_ST_SM3_TXLEVEL_POS))
#define PEC_ST_SM3_RXLEVEL                                      PEC_ST_SM3_RXLEVEL
#define PEC_ST_SM3_RXLEVEL_POS                                  (28U)
#define PEC_ST_SM3_RXLEVEL_LEN                                  (4U)
#define PEC_ST_SM3_RXLEVEL_MSK                                  (((1U<<PEC_ST_SM3_RXLEVEL_LEN)-1)<<PEC_ST_SM3_RXLEVEL_POS)
#define PEC_ST_SM3_RXLEVEL_UMSK                                 (~(((1U<<PEC_ST_SM3_RXLEVEL_LEN)-1)<<PEC_ST_SM3_RXLEVEL_POS))

/* 0x10 : TXF0 */
#define PEC_TXF0_OFFSET                                         (0x10)
#define PEC_SM0_TXF_WDATA                                       PEC_SM0_TXF_WDATA
#define PEC_SM0_TXF_WDATA_POS                                   (0U)
#define PEC_SM0_TXF_WDATA_LEN                                   (32U)
#define PEC_SM0_TXF_WDATA_MSK                                   (((1U<<PEC_SM0_TXF_WDATA_LEN)-1)<<PEC_SM0_TXF_WDATA_POS)
#define PEC_SM0_TXF_WDATA_UMSK                                  (~(((1U<<PEC_SM0_TXF_WDATA_LEN)-1)<<PEC_SM0_TXF_WDATA_POS))

/* 0x14 : TXF1 */
#define PEC_TXF1_OFFSET                                         (0x14)
#define PEC_SM1_TXF_WDATA                                       PEC_SM1_TXF_WDATA
#define PEC_SM1_TXF_WDATA_POS                                   (0U)
#define PEC_SM1_TXF_WDATA_LEN                                   (32U)
#define PEC_SM1_TXF_WDATA_MSK                                   (((1U<<PEC_SM1_TXF_WDATA_LEN)-1)<<PEC_SM1_TXF_WDATA_POS)
#define PEC_SM1_TXF_WDATA_UMSK                                  (~(((1U<<PEC_SM1_TXF_WDATA_LEN)-1)<<PEC_SM1_TXF_WDATA_POS))

/* 0x18 : TXF2 */
#define PEC_TXF2_OFFSET                                         (0x18)
#define PEC_SM2_TXF_WDATA                                       PEC_SM2_TXF_WDATA
#define PEC_SM2_TXF_WDATA_POS                                   (0U)
#define PEC_SM2_TXF_WDATA_LEN                                   (32U)
#define PEC_SM2_TXF_WDATA_MSK                                   (((1U<<PEC_SM2_TXF_WDATA_LEN)-1)<<PEC_SM2_TXF_WDATA_POS)
#define PEC_SM2_TXF_WDATA_UMSK                                  (~(((1U<<PEC_SM2_TXF_WDATA_LEN)-1)<<PEC_SM2_TXF_WDATA_POS))

/* 0x1C : TXF3 */
#define PEC_TXF3_OFFSET                                         (0x1C)
#define PEC_SM3_TXF_WDATA                                       PEC_SM3_TXF_WDATA
#define PEC_SM3_TXF_WDATA_POS                                   (0U)
#define PEC_SM3_TXF_WDATA_LEN                                   (32U)
#define PEC_SM3_TXF_WDATA_MSK                                   (((1U<<PEC_SM3_TXF_WDATA_LEN)-1)<<PEC_SM3_TXF_WDATA_POS)
#define PEC_SM3_TXF_WDATA_UMSK                                  (~(((1U<<PEC_SM3_TXF_WDATA_LEN)-1)<<PEC_SM3_TXF_WDATA_POS))

/* 0x20 : RXF0 */
#define PEC_RXF0_OFFSET                                         (0x20)
#define PEC_SM0_RXF_RDATA                                       PEC_SM0_RXF_RDATA
#define PEC_SM0_RXF_RDATA_POS                                   (0U)
#define PEC_SM0_RXF_RDATA_LEN                                   (32U)
#define PEC_SM0_RXF_RDATA_MSK                                   (((1U<<PEC_SM0_RXF_RDATA_LEN)-1)<<PEC_SM0_RXF_RDATA_POS)
#define PEC_SM0_RXF_RDATA_UMSK                                  (~(((1U<<PEC_SM0_RXF_RDATA_LEN)-1)<<PEC_SM0_RXF_RDATA_POS))

/* 0x24 : RXF1 */
#define PEC_RXF1_OFFSET                                         (0x24)
#define PEC_SM1_RXF_RDATA                                       PEC_SM1_RXF_RDATA
#define PEC_SM1_RXF_RDATA_POS                                   (0U)
#define PEC_SM1_RXF_RDATA_LEN                                   (32U)
#define PEC_SM1_RXF_RDATA_MSK                                   (((1U<<PEC_SM1_RXF_RDATA_LEN)-1)<<PEC_SM1_RXF_RDATA_POS)
#define PEC_SM1_RXF_RDATA_UMSK                                  (~(((1U<<PEC_SM1_RXF_RDATA_LEN)-1)<<PEC_SM1_RXF_RDATA_POS))

/* 0x28 : RXF2 */
#define PEC_RXF2_OFFSET                                         (0x28)
#define PEC_SM2_RXF_RDATA                                       PEC_SM2_RXF_RDATA
#define PEC_SM2_RXF_RDATA_POS                                   (0U)
#define PEC_SM2_RXF_RDATA_LEN                                   (32U)
#define PEC_SM2_RXF_RDATA_MSK                                   (((1U<<PEC_SM2_RXF_RDATA_LEN)-1)<<PEC_SM2_RXF_RDATA_POS)
#define PEC_SM2_RXF_RDATA_UMSK                                  (~(((1U<<PEC_SM2_RXF_RDATA_LEN)-1)<<PEC_SM2_RXF_RDATA_POS))

/* 0x2C : RXF3 */
#define PEC_RXF3_OFFSET                                         (0x2C)
#define PEC_SM3_RXF_RDATA                                       PEC_SM3_RXF_RDATA
#define PEC_SM3_RXF_RDATA_POS                                   (0U)
#define PEC_SM3_RXF_RDATA_LEN                                   (32U)
#define PEC_SM3_RXF_RDATA_MSK                                   (((1U<<PEC_SM3_RXF_RDATA_LEN)-1)<<PEC_SM3_RXF_RDATA_POS)
#define PEC_SM3_RXF_RDATA_UMSK                                  (~(((1U<<PEC_SM3_RXF_RDATA_LEN)-1)<<PEC_SM3_RXF_RDATA_POS))

/* 0x30 : IRQ */
#define PEC_IRQ_OFFSET                                          (0x30)
#define PEC_CR_IRQ                                              PEC_CR_IRQ
#define PEC_CR_IRQ_POS                                          (0U)
#define PEC_CR_IRQ_LEN                                          (8U)
#define PEC_CR_IRQ_MSK                                          (((1U<<PEC_CR_IRQ_LEN)-1)<<PEC_CR_IRQ_POS)
#define PEC_CR_IRQ_UMSK                                         (~(((1U<<PEC_CR_IRQ_LEN)-1)<<PEC_CR_IRQ_POS))

/* 0x34 : IRQ_FORCE */
#define PEC_IRQ_FORCE_OFFSET                                    (0x34)
#define PEC_CR_IRQ_FORCE                                        PEC_CR_IRQ_FORCE
#define PEC_CR_IRQ_FORCE_POS                                    (0U)
#define PEC_CR_IRQ_FORCE_LEN                                    (8U)
#define PEC_CR_IRQ_FORCE_MSK                                    (((1U<<PEC_CR_IRQ_FORCE_LEN)-1)<<PEC_CR_IRQ_FORCE_POS)
#define PEC_CR_IRQ_FORCE_UMSK                                   (~(((1U<<PEC_CR_IRQ_FORCE_LEN)-1)<<PEC_CR_IRQ_FORCE_POS))

/* 0x38 : INPUT_SYNC_BYPASS */
#define PEC_INPUT_SYNC_BYPASS_OFFSET                            (0x38)
#define PEC_CR_IN_SYNC_BYP                                      PEC_CR_IN_SYNC_BYP
#define PEC_CR_IN_SYNC_BYP_POS                                  (0U)
#define PEC_CR_IN_SYNC_BYP_LEN                                  (32U)
#define PEC_CR_IN_SYNC_BYP_MSK                                  (((1U<<PEC_CR_IN_SYNC_BYP_LEN)-1)<<PEC_CR_IN_SYNC_BYP_POS)
#define PEC_CR_IN_SYNC_BYP_UMSK                                 (~(((1U<<PEC_CR_IN_SYNC_BYP_LEN)-1)<<PEC_CR_IN_SYNC_BYP_POS))

/* 0x3C : DBG_PADOUT */
#define PEC_DBG_PADOUT_OFFSET                                   (0x3C)
#define PEC_ST_DBG_PADOUT                                       PEC_ST_DBG_PADOUT
#define PEC_ST_DBG_PADOUT_POS                                   (0U)
#define PEC_ST_DBG_PADOUT_LEN                                   (32U)
#define PEC_ST_DBG_PADOUT_MSK                                   (((1U<<PEC_ST_DBG_PADOUT_LEN)-1)<<PEC_ST_DBG_PADOUT_POS)
#define PEC_ST_DBG_PADOUT_UMSK                                  (~(((1U<<PEC_ST_DBG_PADOUT_LEN)-1)<<PEC_ST_DBG_PADOUT_POS))

/* 0x40 : DBG_PADOE */
#define PEC_DBG_PADOE_OFFSET                                    (0x40)
#define PEC_ST_DBG_PADOE                                        PEC_ST_DBG_PADOE
#define PEC_ST_DBG_PADOE_POS                                    (0U)
#define PEC_ST_DBG_PADOE_LEN                                    (32U)
#define PEC_ST_DBG_PADOE_MSK                                    (((1U<<PEC_ST_DBG_PADOE_LEN)-1)<<PEC_ST_DBG_PADOE_POS)
#define PEC_ST_DBG_PADOE_UMSK                                   (~(((1U<<PEC_ST_DBG_PADOE_LEN)-1)<<PEC_ST_DBG_PADOE_POS))

/* 0x44 : The PEC hardware has some free parameters that may vary between chip products. */
#define PEC_DBG_CFGINFO_OFFSET                                  (0x44)
#define PEC_ST_FIFO_DEPTH                                       PEC_ST_FIFO_DEPTH
#define PEC_ST_FIFO_DEPTH_POS                                   (0U)
#define PEC_ST_FIFO_DEPTH_LEN                                   (6U)
#define PEC_ST_FIFO_DEPTH_MSK                                   (((1U<<PEC_ST_FIFO_DEPTH_LEN)-1)<<PEC_ST_FIFO_DEPTH_POS)
#define PEC_ST_FIFO_DEPTH_UMSK                                  (~(((1U<<PEC_ST_FIFO_DEPTH_LEN)-1)<<PEC_ST_FIFO_DEPTH_POS))
#define PEC_ST_SM_COUNT                                         PEC_ST_SM_COUNT
#define PEC_ST_SM_COUNT_POS                                     (8U)
#define PEC_ST_SM_COUNT_LEN                                     (4U)
#define PEC_ST_SM_COUNT_MSK                                     (((1U<<PEC_ST_SM_COUNT_LEN)-1)<<PEC_ST_SM_COUNT_POS)
#define PEC_ST_SM_COUNT_UMSK                                    (~(((1U<<PEC_ST_SM_COUNT_LEN)-1)<<PEC_ST_SM_COUNT_POS))
#define PEC_ST_IMEM_SIZE                                        PEC_ST_IMEM_SIZE
#define PEC_ST_IMEM_SIZE_POS                                    (16U)
#define PEC_ST_IMEM_SIZE_LEN                                    (6U)
#define PEC_ST_IMEM_SIZE_MSK                                    (((1U<<PEC_ST_IMEM_SIZE_LEN)-1)<<PEC_ST_IMEM_SIZE_POS)
#define PEC_ST_IMEM_SIZE_UMSK                                   (~(((1U<<PEC_ST_IMEM_SIZE_LEN)-1)<<PEC_ST_IMEM_SIZE_POS))

/* 0x48 : INSTR_MEM0 */
#define PEC_INSTR_MEM0_OFFSET                                   (0x48)
#define PEC_CR_INSTR_MEM0                                       PEC_CR_INSTR_MEM0
#define PEC_CR_INSTR_MEM0_POS                                   (0U)
#define PEC_CR_INSTR_MEM0_LEN                                   (16U)
#define PEC_CR_INSTR_MEM0_MSK                                   (((1U<<PEC_CR_INSTR_MEM0_LEN)-1)<<PEC_CR_INSTR_MEM0_POS)
#define PEC_CR_INSTR_MEM0_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM0_LEN)-1)<<PEC_CR_INSTR_MEM0_POS))

/* 0x4C : INSTR_MEM1 */
#define PEC_INSTR_MEM1_OFFSET                                   (0x4C)
#define PEC_CR_INSTR_MEM1                                       PEC_CR_INSTR_MEM1
#define PEC_CR_INSTR_MEM1_POS                                   (0U)
#define PEC_CR_INSTR_MEM1_LEN                                   (16U)
#define PEC_CR_INSTR_MEM1_MSK                                   (((1U<<PEC_CR_INSTR_MEM1_LEN)-1)<<PEC_CR_INSTR_MEM1_POS)
#define PEC_CR_INSTR_MEM1_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM1_LEN)-1)<<PEC_CR_INSTR_MEM1_POS))

/* 0x50 : INSTR_MEM2 */
#define PEC_INSTR_MEM2_OFFSET                                   (0x50)
#define PEC_CR_INSTR_MEM2                                       PEC_CR_INSTR_MEM2
#define PEC_CR_INSTR_MEM2_POS                                   (0U)
#define PEC_CR_INSTR_MEM2_LEN                                   (16U)
#define PEC_CR_INSTR_MEM2_MSK                                   (((1U<<PEC_CR_INSTR_MEM2_LEN)-1)<<PEC_CR_INSTR_MEM2_POS)
#define PEC_CR_INSTR_MEM2_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM2_LEN)-1)<<PEC_CR_INSTR_MEM2_POS))

/* 0x54 : INSTR_MEM3 */
#define PEC_INSTR_MEM3_OFFSET                                   (0x54)
#define PEC_CR_INSTR_MEM3                                       PEC_CR_INSTR_MEM3
#define PEC_CR_INSTR_MEM3_POS                                   (0U)
#define PEC_CR_INSTR_MEM3_LEN                                   (16U)
#define PEC_CR_INSTR_MEM3_MSK                                   (((1U<<PEC_CR_INSTR_MEM3_LEN)-1)<<PEC_CR_INSTR_MEM3_POS)
#define PEC_CR_INSTR_MEM3_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM3_LEN)-1)<<PEC_CR_INSTR_MEM3_POS))

/* 0x58 : INSTR_MEM4 */
#define PEC_INSTR_MEM4_OFFSET                                   (0x58)
#define PEC_CR_INSTR_MEM4                                       PEC_CR_INSTR_MEM4
#define PEC_CR_INSTR_MEM4_POS                                   (0U)
#define PEC_CR_INSTR_MEM4_LEN                                   (16U)
#define PEC_CR_INSTR_MEM4_MSK                                   (((1U<<PEC_CR_INSTR_MEM4_LEN)-1)<<PEC_CR_INSTR_MEM4_POS)
#define PEC_CR_INSTR_MEM4_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM4_LEN)-1)<<PEC_CR_INSTR_MEM4_POS))

/* 0x5C : INSTR_MEM5 */
#define PEC_INSTR_MEM5_OFFSET                                   (0x5C)
#define PEC_CR_INSTR_MEM5                                       PEC_CR_INSTR_MEM5
#define PEC_CR_INSTR_MEM5_POS                                   (0U)
#define PEC_CR_INSTR_MEM5_LEN                                   (16U)
#define PEC_CR_INSTR_MEM5_MSK                                   (((1U<<PEC_CR_INSTR_MEM5_LEN)-1)<<PEC_CR_INSTR_MEM5_POS)
#define PEC_CR_INSTR_MEM5_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM5_LEN)-1)<<PEC_CR_INSTR_MEM5_POS))

/* 0x60 : INSTR_MEM6 */
#define PEC_INSTR_MEM6_OFFSET                                   (0x60)
#define PEC_CR_INSTR_MEM6                                       PEC_CR_INSTR_MEM6
#define PEC_CR_INSTR_MEM6_POS                                   (0U)
#define PEC_CR_INSTR_MEM6_LEN                                   (16U)
#define PEC_CR_INSTR_MEM6_MSK                                   (((1U<<PEC_CR_INSTR_MEM6_LEN)-1)<<PEC_CR_INSTR_MEM6_POS)
#define PEC_CR_INSTR_MEM6_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM6_LEN)-1)<<PEC_CR_INSTR_MEM6_POS))

/* 0x64 : INSTR_MEM7 */
#define PEC_INSTR_MEM7_OFFSET                                   (0x64)
#define PEC_CR_INSTR_MEM7                                       PEC_CR_INSTR_MEM7
#define PEC_CR_INSTR_MEM7_POS                                   (0U)
#define PEC_CR_INSTR_MEM7_LEN                                   (16U)
#define PEC_CR_INSTR_MEM7_MSK                                   (((1U<<PEC_CR_INSTR_MEM7_LEN)-1)<<PEC_CR_INSTR_MEM7_POS)
#define PEC_CR_INSTR_MEM7_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM7_LEN)-1)<<PEC_CR_INSTR_MEM7_POS))

/* 0x68 : INSTR_MEM8 */
#define PEC_INSTR_MEM8_OFFSET                                   (0x68)
#define PEC_CR_INSTR_MEM8                                       PEC_CR_INSTR_MEM8
#define PEC_CR_INSTR_MEM8_POS                                   (0U)
#define PEC_CR_INSTR_MEM8_LEN                                   (16U)
#define PEC_CR_INSTR_MEM8_MSK                                   (((1U<<PEC_CR_INSTR_MEM8_LEN)-1)<<PEC_CR_INSTR_MEM8_POS)
#define PEC_CR_INSTR_MEM8_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM8_LEN)-1)<<PEC_CR_INSTR_MEM8_POS))

/* 0x6C : INSTR_MEM9 */
#define PEC_INSTR_MEM9_OFFSET                                   (0x6C)
#define PEC_CR_INSTR_MEM9                                       PEC_CR_INSTR_MEM9
#define PEC_CR_INSTR_MEM9_POS                                   (0U)
#define PEC_CR_INSTR_MEM9_LEN                                   (16U)
#define PEC_CR_INSTR_MEM9_MSK                                   (((1U<<PEC_CR_INSTR_MEM9_LEN)-1)<<PEC_CR_INSTR_MEM9_POS)
#define PEC_CR_INSTR_MEM9_UMSK                                  (~(((1U<<PEC_CR_INSTR_MEM9_LEN)-1)<<PEC_CR_INSTR_MEM9_POS))

/* 0x70 : INSTR_MEM10 */
#define PEC_INSTR_MEM10_OFFSET                                  (0x70)
#define PEC_CR_INSTR_MEM10                                      PEC_CR_INSTR_MEM10
#define PEC_CR_INSTR_MEM10_POS                                  (0U)
#define PEC_CR_INSTR_MEM10_LEN                                  (16U)
#define PEC_CR_INSTR_MEM10_MSK                                  (((1U<<PEC_CR_INSTR_MEM10_LEN)-1)<<PEC_CR_INSTR_MEM10_POS)
#define PEC_CR_INSTR_MEM10_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM10_LEN)-1)<<PEC_CR_INSTR_MEM10_POS))

/* 0x74 : INSTR_MEM11 */
#define PEC_INSTR_MEM11_OFFSET                                  (0x74)
#define PEC_CR_INSTR_MEM11                                      PEC_CR_INSTR_MEM11
#define PEC_CR_INSTR_MEM11_POS                                  (0U)
#define PEC_CR_INSTR_MEM11_LEN                                  (16U)
#define PEC_CR_INSTR_MEM11_MSK                                  (((1U<<PEC_CR_INSTR_MEM11_LEN)-1)<<PEC_CR_INSTR_MEM11_POS)
#define PEC_CR_INSTR_MEM11_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM11_LEN)-1)<<PEC_CR_INSTR_MEM11_POS))

/* 0x78 : INSTR_MEM12 */
#define PEC_INSTR_MEM12_OFFSET                                  (0x78)
#define PEC_CR_INSTR_MEM12                                      PEC_CR_INSTR_MEM12
#define PEC_CR_INSTR_MEM12_POS                                  (0U)
#define PEC_CR_INSTR_MEM12_LEN                                  (16U)
#define PEC_CR_INSTR_MEM12_MSK                                  (((1U<<PEC_CR_INSTR_MEM12_LEN)-1)<<PEC_CR_INSTR_MEM12_POS)
#define PEC_CR_INSTR_MEM12_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM12_LEN)-1)<<PEC_CR_INSTR_MEM12_POS))

/* 0x7C : INSTR_MEM13 */
#define PEC_INSTR_MEM13_OFFSET                                  (0x7C)
#define PEC_CR_INSTR_MEM13                                      PEC_CR_INSTR_MEM13
#define PEC_CR_INSTR_MEM13_POS                                  (0U)
#define PEC_CR_INSTR_MEM13_LEN                                  (16U)
#define PEC_CR_INSTR_MEM13_MSK                                  (((1U<<PEC_CR_INSTR_MEM13_LEN)-1)<<PEC_CR_INSTR_MEM13_POS)
#define PEC_CR_INSTR_MEM13_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM13_LEN)-1)<<PEC_CR_INSTR_MEM13_POS))

/* 0x80 : INSTR_MEM14 */
#define PEC_INSTR_MEM14_OFFSET                                  (0x80)
#define PEC_CR_INSTR_MEM14                                      PEC_CR_INSTR_MEM14
#define PEC_CR_INSTR_MEM14_POS                                  (0U)
#define PEC_CR_INSTR_MEM14_LEN                                  (16U)
#define PEC_CR_INSTR_MEM14_MSK                                  (((1U<<PEC_CR_INSTR_MEM14_LEN)-1)<<PEC_CR_INSTR_MEM14_POS)
#define PEC_CR_INSTR_MEM14_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM14_LEN)-1)<<PEC_CR_INSTR_MEM14_POS))

/* 0x84 : INSTR_MEM15 */
#define PEC_INSTR_MEM15_OFFSET                                  (0x84)
#define PEC_CR_INSTR_MEM15                                      PEC_CR_INSTR_MEM15
#define PEC_CR_INSTR_MEM15_POS                                  (0U)
#define PEC_CR_INSTR_MEM15_LEN                                  (16U)
#define PEC_CR_INSTR_MEM15_MSK                                  (((1U<<PEC_CR_INSTR_MEM15_LEN)-1)<<PEC_CR_INSTR_MEM15_POS)
#define PEC_CR_INSTR_MEM15_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM15_LEN)-1)<<PEC_CR_INSTR_MEM15_POS))

/* 0x88 : INSTR_MEM16 */
#define PEC_INSTR_MEM16_OFFSET                                  (0x88)
#define PEC_CR_INSTR_MEM16                                      PEC_CR_INSTR_MEM16
#define PEC_CR_INSTR_MEM16_POS                                  (0U)
#define PEC_CR_INSTR_MEM16_LEN                                  (16U)
#define PEC_CR_INSTR_MEM16_MSK                                  (((1U<<PEC_CR_INSTR_MEM16_LEN)-1)<<PEC_CR_INSTR_MEM16_POS)
#define PEC_CR_INSTR_MEM16_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM16_LEN)-1)<<PEC_CR_INSTR_MEM16_POS))

/* 0x8C : INSTR_MEM17 */
#define PEC_INSTR_MEM17_OFFSET                                  (0x8C)
#define PEC_CR_INSTR_MEM17                                      PEC_CR_INSTR_MEM17
#define PEC_CR_INSTR_MEM17_POS                                  (0U)
#define PEC_CR_INSTR_MEM17_LEN                                  (16U)
#define PEC_CR_INSTR_MEM17_MSK                                  (((1U<<PEC_CR_INSTR_MEM17_LEN)-1)<<PEC_CR_INSTR_MEM17_POS)
#define PEC_CR_INSTR_MEM17_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM17_LEN)-1)<<PEC_CR_INSTR_MEM17_POS))

/* 0x90 : INSTR_MEM18 */
#define PEC_INSTR_MEM18_OFFSET                                  (0x90)
#define PEC_CR_INSTR_MEM18                                      PEC_CR_INSTR_MEM18
#define PEC_CR_INSTR_MEM18_POS                                  (0U)
#define PEC_CR_INSTR_MEM18_LEN                                  (16U)
#define PEC_CR_INSTR_MEM18_MSK                                  (((1U<<PEC_CR_INSTR_MEM18_LEN)-1)<<PEC_CR_INSTR_MEM18_POS)
#define PEC_CR_INSTR_MEM18_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM18_LEN)-1)<<PEC_CR_INSTR_MEM18_POS))

/* 0x94 : INSTR_MEM19 */
#define PEC_INSTR_MEM19_OFFSET                                  (0x94)
#define PEC_CR_INSTR_MEM19                                      PEC_CR_INSTR_MEM19
#define PEC_CR_INSTR_MEM19_POS                                  (0U)
#define PEC_CR_INSTR_MEM19_LEN                                  (16U)
#define PEC_CR_INSTR_MEM19_MSK                                  (((1U<<PEC_CR_INSTR_MEM19_LEN)-1)<<PEC_CR_INSTR_MEM19_POS)
#define PEC_CR_INSTR_MEM19_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM19_LEN)-1)<<PEC_CR_INSTR_MEM19_POS))

/* 0x98 : INSTR_MEM20 */
#define PEC_INSTR_MEM20_OFFSET                                  (0x98)
#define PEC_CR_INSTR_MEM20                                      PEC_CR_INSTR_MEM20
#define PEC_CR_INSTR_MEM20_POS                                  (0U)
#define PEC_CR_INSTR_MEM20_LEN                                  (16U)
#define PEC_CR_INSTR_MEM20_MSK                                  (((1U<<PEC_CR_INSTR_MEM20_LEN)-1)<<PEC_CR_INSTR_MEM20_POS)
#define PEC_CR_INSTR_MEM20_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM20_LEN)-1)<<PEC_CR_INSTR_MEM20_POS))

/* 0x9C : INSTR_MEM21 */
#define PEC_INSTR_MEM21_OFFSET                                  (0x9C)
#define PEC_CR_INSTR_MEM21                                      PEC_CR_INSTR_MEM21
#define PEC_CR_INSTR_MEM21_POS                                  (0U)
#define PEC_CR_INSTR_MEM21_LEN                                  (16U)
#define PEC_CR_INSTR_MEM21_MSK                                  (((1U<<PEC_CR_INSTR_MEM21_LEN)-1)<<PEC_CR_INSTR_MEM21_POS)
#define PEC_CR_INSTR_MEM21_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM21_LEN)-1)<<PEC_CR_INSTR_MEM21_POS))

/* 0xA0 : INSTR_MEM22 */
#define PEC_INSTR_MEM22_OFFSET                                  (0xA0)
#define PEC_CR_INSTR_MEM22                                      PEC_CR_INSTR_MEM22
#define PEC_CR_INSTR_MEM22_POS                                  (0U)
#define PEC_CR_INSTR_MEM22_LEN                                  (16U)
#define PEC_CR_INSTR_MEM22_MSK                                  (((1U<<PEC_CR_INSTR_MEM22_LEN)-1)<<PEC_CR_INSTR_MEM22_POS)
#define PEC_CR_INSTR_MEM22_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM22_LEN)-1)<<PEC_CR_INSTR_MEM22_POS))

/* 0xA4 : INSTR_MEM23 */
#define PEC_INSTR_MEM23_OFFSET                                  (0xA4)
#define PEC_CR_INSTR_MEM23                                      PEC_CR_INSTR_MEM23
#define PEC_CR_INSTR_MEM23_POS                                  (0U)
#define PEC_CR_INSTR_MEM23_LEN                                  (16U)
#define PEC_CR_INSTR_MEM23_MSK                                  (((1U<<PEC_CR_INSTR_MEM23_LEN)-1)<<PEC_CR_INSTR_MEM23_POS)
#define PEC_CR_INSTR_MEM23_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM23_LEN)-1)<<PEC_CR_INSTR_MEM23_POS))

/* 0xA8 : INSTR_MEM24 */
#define PEC_INSTR_MEM24_OFFSET                                  (0xA8)
#define PEC_CR_INSTR_MEM24                                      PEC_CR_INSTR_MEM24
#define PEC_CR_INSTR_MEM24_POS                                  (0U)
#define PEC_CR_INSTR_MEM24_LEN                                  (16U)
#define PEC_CR_INSTR_MEM24_MSK                                  (((1U<<PEC_CR_INSTR_MEM24_LEN)-1)<<PEC_CR_INSTR_MEM24_POS)
#define PEC_CR_INSTR_MEM24_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM24_LEN)-1)<<PEC_CR_INSTR_MEM24_POS))

/* 0xAC : INSTR_MEM25 */
#define PEC_INSTR_MEM25_OFFSET                                  (0xAC)
#define PEC_CR_INSTR_MEM25                                      PEC_CR_INSTR_MEM25
#define PEC_CR_INSTR_MEM25_POS                                  (0U)
#define PEC_CR_INSTR_MEM25_LEN                                  (16U)
#define PEC_CR_INSTR_MEM25_MSK                                  (((1U<<PEC_CR_INSTR_MEM25_LEN)-1)<<PEC_CR_INSTR_MEM25_POS)
#define PEC_CR_INSTR_MEM25_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM25_LEN)-1)<<PEC_CR_INSTR_MEM25_POS))

/* 0xB0 : INSTR_MEM26 */
#define PEC_INSTR_MEM26_OFFSET                                  (0xB0)
#define PEC_CR_INSTR_MEM26                                      PEC_CR_INSTR_MEM26
#define PEC_CR_INSTR_MEM26_POS                                  (0U)
#define PEC_CR_INSTR_MEM26_LEN                                  (16U)
#define PEC_CR_INSTR_MEM26_MSK                                  (((1U<<PEC_CR_INSTR_MEM26_LEN)-1)<<PEC_CR_INSTR_MEM26_POS)
#define PEC_CR_INSTR_MEM26_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM26_LEN)-1)<<PEC_CR_INSTR_MEM26_POS))

/* 0xB4 : INSTR_MEM27 */
#define PEC_INSTR_MEM27_OFFSET                                  (0xB4)
#define PEC_CR_INSTR_MEM27                                      PEC_CR_INSTR_MEM27
#define PEC_CR_INSTR_MEM27_POS                                  (0U)
#define PEC_CR_INSTR_MEM27_LEN                                  (16U)
#define PEC_CR_INSTR_MEM27_MSK                                  (((1U<<PEC_CR_INSTR_MEM27_LEN)-1)<<PEC_CR_INSTR_MEM27_POS)
#define PEC_CR_INSTR_MEM27_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM27_LEN)-1)<<PEC_CR_INSTR_MEM27_POS))

/* 0xB8 : INSTR_MEM28 */
#define PEC_INSTR_MEM28_OFFSET                                  (0xB8)
#define PEC_CR_INSTR_MEM28                                      PEC_CR_INSTR_MEM28
#define PEC_CR_INSTR_MEM28_POS                                  (0U)
#define PEC_CR_INSTR_MEM28_LEN                                  (16U)
#define PEC_CR_INSTR_MEM28_MSK                                  (((1U<<PEC_CR_INSTR_MEM28_LEN)-1)<<PEC_CR_INSTR_MEM28_POS)
#define PEC_CR_INSTR_MEM28_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM28_LEN)-1)<<PEC_CR_INSTR_MEM28_POS))

/* 0xBC : INSTR_MEM29 */
#define PEC_INSTR_MEM29_OFFSET                                  (0xBC)
#define PEC_CR_INSTR_MEM29                                      PEC_CR_INSTR_MEM29
#define PEC_CR_INSTR_MEM29_POS                                  (0U)
#define PEC_CR_INSTR_MEM29_LEN                                  (16U)
#define PEC_CR_INSTR_MEM29_MSK                                  (((1U<<PEC_CR_INSTR_MEM29_LEN)-1)<<PEC_CR_INSTR_MEM29_POS)
#define PEC_CR_INSTR_MEM29_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM29_LEN)-1)<<PEC_CR_INSTR_MEM29_POS))

/* 0xC0 : INSTR_MEM30 */
#define PEC_INSTR_MEM30_OFFSET                                  (0xC0)
#define PEC_CR_INSTR_MEM30                                      PEC_CR_INSTR_MEM30
#define PEC_CR_INSTR_MEM30_POS                                  (0U)
#define PEC_CR_INSTR_MEM30_LEN                                  (16U)
#define PEC_CR_INSTR_MEM30_MSK                                  (((1U<<PEC_CR_INSTR_MEM30_LEN)-1)<<PEC_CR_INSTR_MEM30_POS)
#define PEC_CR_INSTR_MEM30_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM30_LEN)-1)<<PEC_CR_INSTR_MEM30_POS))

/* 0xC4 : INSTR_MEM31 */
#define PEC_INSTR_MEM31_OFFSET                                  (0xC4)
#define PEC_CR_INSTR_MEM31                                      PEC_CR_INSTR_MEM31
#define PEC_CR_INSTR_MEM31_POS                                  (0U)
#define PEC_CR_INSTR_MEM31_LEN                                  (16U)
#define PEC_CR_INSTR_MEM31_MSK                                  (((1U<<PEC_CR_INSTR_MEM31_LEN)-1)<<PEC_CR_INSTR_MEM31_POS)
#define PEC_CR_INSTR_MEM31_UMSK                                 (~(((1U<<PEC_CR_INSTR_MEM31_LEN)-1)<<PEC_CR_INSTR_MEM31_POS))

/* 0xC8 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
#define PEC_SM0_CLKDIV_OFFSET                                   (0xC8)
#define PEC_CR_SM0_FRAC                                         PEC_CR_SM0_FRAC
#define PEC_CR_SM0_FRAC_POS                                     (8U)
#define PEC_CR_SM0_FRAC_LEN                                     (8U)
#define PEC_CR_SM0_FRAC_MSK                                     (((1U<<PEC_CR_SM0_FRAC_LEN)-1)<<PEC_CR_SM0_FRAC_POS)
#define PEC_CR_SM0_FRAC_UMSK                                    (~(((1U<<PEC_CR_SM0_FRAC_LEN)-1)<<PEC_CR_SM0_FRAC_POS))
#define PEC_CR_SM0_INT                                          PEC_CR_SM0_INT
#define PEC_CR_SM0_INT_POS                                      (16U)
#define PEC_CR_SM0_INT_LEN                                      (16U)
#define PEC_CR_SM0_INT_MSK                                      (((1U<<PEC_CR_SM0_INT_LEN)-1)<<PEC_CR_SM0_INT_POS)
#define PEC_CR_SM0_INT_UMSK                                     (~(((1U<<PEC_CR_SM0_INT_LEN)-1)<<PEC_CR_SM0_INT_POS))

/* 0xCC : Execution/behavioural settings for state machine N */
#define PEC_SM0_EXECCTRL_OFFSET                                 (0xCC)
#define PEC_CR_SM0_STATUS_N                                     PEC_CR_SM0_STATUS_N
#define PEC_CR_SM0_STATUS_N_POS                                 (0U)
#define PEC_CR_SM0_STATUS_N_LEN                                 (4U)
#define PEC_CR_SM0_STATUS_N_MSK                                 (((1U<<PEC_CR_SM0_STATUS_N_LEN)-1)<<PEC_CR_SM0_STATUS_N_POS)
#define PEC_CR_SM0_STATUS_N_UMSK                                (~(((1U<<PEC_CR_SM0_STATUS_N_LEN)-1)<<PEC_CR_SM0_STATUS_N_POS))
#define PEC_CR_SM0_STATUS_SEL                                   PEC_CR_SM0_STATUS_SEL
#define PEC_CR_SM0_STATUS_SEL_POS                               (4U)
#define PEC_CR_SM0_STATUS_SEL_LEN                               (1U)
#define PEC_CR_SM0_STATUS_SEL_MSK                               (((1U<<PEC_CR_SM0_STATUS_SEL_LEN)-1)<<PEC_CR_SM0_STATUS_SEL_POS)
#define PEC_CR_SM0_STATUS_SEL_UMSK                              (~(((1U<<PEC_CR_SM0_STATUS_SEL_LEN)-1)<<PEC_CR_SM0_STATUS_SEL_POS))
#define PEC_CR_SM0_WRAP_BOTTOM                                  PEC_CR_SM0_WRAP_BOTTOM
#define PEC_CR_SM0_WRAP_BOTTOM_POS                              (7U)
#define PEC_CR_SM0_WRAP_BOTTOM_LEN                              (5U)
#define PEC_CR_SM0_WRAP_BOTTOM_MSK                              (((1U<<PEC_CR_SM0_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM0_WRAP_BOTTOM_POS)
#define PEC_CR_SM0_WRAP_BOTTOM_UMSK                             (~(((1U<<PEC_CR_SM0_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM0_WRAP_BOTTOM_POS))
#define PEC_CR_SM0_WRAP_TOP                                     PEC_CR_SM0_WRAP_TOP
#define PEC_CR_SM0_WRAP_TOP_POS                                 (12U)
#define PEC_CR_SM0_WRAP_TOP_LEN                                 (5U)
#define PEC_CR_SM0_WRAP_TOP_MSK                                 (((1U<<PEC_CR_SM0_WRAP_TOP_LEN)-1)<<PEC_CR_SM0_WRAP_TOP_POS)
#define PEC_CR_SM0_WRAP_TOP_UMSK                                (~(((1U<<PEC_CR_SM0_WRAP_TOP_LEN)-1)<<PEC_CR_SM0_WRAP_TOP_POS))
#define PEC_CR_SM0_JMP_PIN                                      PEC_CR_SM0_JMP_PIN
#define PEC_CR_SM0_JMP_PIN_POS                                  (24U)
#define PEC_CR_SM0_JMP_PIN_LEN                                  (5U)
#define PEC_CR_SM0_JMP_PIN_MSK                                  (((1U<<PEC_CR_SM0_JMP_PIN_LEN)-1)<<PEC_CR_SM0_JMP_PIN_POS)
#define PEC_CR_SM0_JMP_PIN_UMSK                                 (~(((1U<<PEC_CR_SM0_JMP_PIN_LEN)-1)<<PEC_CR_SM0_JMP_PIN_POS))
#define PEC_CR_SM0_SIDE_PINDIR                                  PEC_CR_SM0_SIDE_PINDIR
#define PEC_CR_SM0_SIDE_PINDIR_POS                              (29U)
#define PEC_CR_SM0_SIDE_PINDIR_LEN                              (1U)
#define PEC_CR_SM0_SIDE_PINDIR_MSK                              (((1U<<PEC_CR_SM0_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM0_SIDE_PINDIR_POS)
#define PEC_CR_SM0_SIDE_PINDIR_UMSK                             (~(((1U<<PEC_CR_SM0_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM0_SIDE_PINDIR_POS))
#define PEC_CR_SM0_SIDE_EN                                      PEC_CR_SM0_SIDE_EN
#define PEC_CR_SM0_SIDE_EN_POS                                  (30U)
#define PEC_CR_SM0_SIDE_EN_LEN                                  (1U)
#define PEC_CR_SM0_SIDE_EN_MSK                                  (((1U<<PEC_CR_SM0_SIDE_EN_LEN)-1)<<PEC_CR_SM0_SIDE_EN_POS)
#define PEC_CR_SM0_SIDE_EN_UMSK                                 (~(((1U<<PEC_CR_SM0_SIDE_EN_LEN)-1)<<PEC_CR_SM0_SIDE_EN_POS))
#define PEC_ST_SM0_EXEC_STALLED                                 PEC_ST_SM0_EXEC_STALLED
#define PEC_ST_SM0_EXEC_STALLED_POS                             (31U)
#define PEC_ST_SM0_EXEC_STALLED_LEN                             (1U)
#define PEC_ST_SM0_EXEC_STALLED_MSK                             (((1U<<PEC_ST_SM0_EXEC_STALLED_LEN)-1)<<PEC_ST_SM0_EXEC_STALLED_POS)
#define PEC_ST_SM0_EXEC_STALLED_UMSK                            (~(((1U<<PEC_ST_SM0_EXEC_STALLED_LEN)-1)<<PEC_ST_SM0_EXEC_STALLED_POS))

/* 0xD0 : Control behaviour of the input/output shift registers for state machine N */
#define PEC_SM0_SHIFTCTRL_OFFSET                                (0xD0)
#define PEC_CR_SM0_TXF_THR                                      PEC_CR_SM0_TXF_THR
#define PEC_CR_SM0_TXF_THR_POS                                  (0U)
#define PEC_CR_SM0_TXF_THR_LEN                                  (3U)
#define PEC_CR_SM0_TXF_THR_MSK                                  (((1U<<PEC_CR_SM0_TXF_THR_LEN)-1)<<PEC_CR_SM0_TXF_THR_POS)
#define PEC_CR_SM0_TXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM0_TXF_THR_LEN)-1)<<PEC_CR_SM0_TXF_THR_POS))
#define PEC_ST_SM0_TXF_RDY                                      PEC_ST_SM0_TXF_RDY
#define PEC_ST_SM0_TXF_RDY_POS                                  (7U)
#define PEC_ST_SM0_TXF_RDY_LEN                                  (1U)
#define PEC_ST_SM0_TXF_RDY_MSK                                  (((1U<<PEC_ST_SM0_TXF_RDY_LEN)-1)<<PEC_ST_SM0_TXF_RDY_POS)
#define PEC_ST_SM0_TXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM0_TXF_RDY_LEN)-1)<<PEC_ST_SM0_TXF_RDY_POS))
#define PEC_CR_SM0_RXF_THR                                      PEC_CR_SM0_RXF_THR
#define PEC_CR_SM0_RXF_THR_POS                                  (8U)
#define PEC_CR_SM0_RXF_THR_LEN                                  (3U)
#define PEC_CR_SM0_RXF_THR_MSK                                  (((1U<<PEC_CR_SM0_RXF_THR_LEN)-1)<<PEC_CR_SM0_RXF_THR_POS)
#define PEC_CR_SM0_RXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM0_RXF_THR_LEN)-1)<<PEC_CR_SM0_RXF_THR_POS))
#define PEC_ST_SM0_RXF_RDY                                      PEC_ST_SM0_RXF_RDY
#define PEC_ST_SM0_RXF_RDY_POS                                  (15U)
#define PEC_ST_SM0_RXF_RDY_LEN                                  (1U)
#define PEC_ST_SM0_RXF_RDY_MSK                                  (((1U<<PEC_ST_SM0_RXF_RDY_LEN)-1)<<PEC_ST_SM0_RXF_RDY_POS)
#define PEC_ST_SM0_RXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM0_RXF_RDY_LEN)-1)<<PEC_ST_SM0_RXF_RDY_POS))
#define PEC_CR_SM0_AUTOPUSH                                     PEC_CR_SM0_AUTOPUSH
#define PEC_CR_SM0_AUTOPUSH_POS                                 (16U)
#define PEC_CR_SM0_AUTOPUSH_LEN                                 (1U)
#define PEC_CR_SM0_AUTOPUSH_MSK                                 (((1U<<PEC_CR_SM0_AUTOPUSH_LEN)-1)<<PEC_CR_SM0_AUTOPUSH_POS)
#define PEC_CR_SM0_AUTOPUSH_UMSK                                (~(((1U<<PEC_CR_SM0_AUTOPUSH_LEN)-1)<<PEC_CR_SM0_AUTOPUSH_POS))
#define PEC_CR_SM0_AUTOPULL                                     PEC_CR_SM0_AUTOPULL
#define PEC_CR_SM0_AUTOPULL_POS                                 (17U)
#define PEC_CR_SM0_AUTOPULL_LEN                                 (1U)
#define PEC_CR_SM0_AUTOPULL_MSK                                 (((1U<<PEC_CR_SM0_AUTOPULL_LEN)-1)<<PEC_CR_SM0_AUTOPULL_POS)
#define PEC_CR_SM0_AUTOPULL_UMSK                                (~(((1U<<PEC_CR_SM0_AUTOPULL_LEN)-1)<<PEC_CR_SM0_AUTOPULL_POS))
#define PEC_CR_SM0_IN_SHIFTDIR                                  PEC_CR_SM0_IN_SHIFTDIR
#define PEC_CR_SM0_IN_SHIFTDIR_POS                              (18U)
#define PEC_CR_SM0_IN_SHIFTDIR_LEN                              (1U)
#define PEC_CR_SM0_IN_SHIFTDIR_MSK                              (((1U<<PEC_CR_SM0_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM0_IN_SHIFTDIR_POS)
#define PEC_CR_SM0_IN_SHIFTDIR_UMSK                             (~(((1U<<PEC_CR_SM0_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM0_IN_SHIFTDIR_POS))
#define PEC_CR_SM0_OUT_SHIFTDIR                                 PEC_CR_SM0_OUT_SHIFTDIR
#define PEC_CR_SM0_OUT_SHIFTDIR_POS                             (19U)
#define PEC_CR_SM0_OUT_SHIFTDIR_LEN                             (1U)
#define PEC_CR_SM0_OUT_SHIFTDIR_MSK                             (((1U<<PEC_CR_SM0_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM0_OUT_SHIFTDIR_POS)
#define PEC_CR_SM0_OUT_SHIFTDIR_UMSK                            (~(((1U<<PEC_CR_SM0_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM0_OUT_SHIFTDIR_POS))
#define PEC_CR_SM0_PUSH_THRESH                                  PEC_CR_SM0_PUSH_THRESH
#define PEC_CR_SM0_PUSH_THRESH_POS                              (20U)
#define PEC_CR_SM0_PUSH_THRESH_LEN                              (5U)
#define PEC_CR_SM0_PUSH_THRESH_MSK                              (((1U<<PEC_CR_SM0_PUSH_THRESH_LEN)-1)<<PEC_CR_SM0_PUSH_THRESH_POS)
#define PEC_CR_SM0_PUSH_THRESH_UMSK                             (~(((1U<<PEC_CR_SM0_PUSH_THRESH_LEN)-1)<<PEC_CR_SM0_PUSH_THRESH_POS))
#define PEC_CR_SM0_PULL_THRESH                                  PEC_CR_SM0_PULL_THRESH
#define PEC_CR_SM0_PULL_THRESH_POS                              (25U)
#define PEC_CR_SM0_PULL_THRESH_LEN                              (5U)
#define PEC_CR_SM0_PULL_THRESH_MSK                              (((1U<<PEC_CR_SM0_PULL_THRESH_LEN)-1)<<PEC_CR_SM0_PULL_THRESH_POS)
#define PEC_CR_SM0_PULL_THRESH_UMSK                             (~(((1U<<PEC_CR_SM0_PULL_THRESH_LEN)-1)<<PEC_CR_SM0_PULL_THRESH_POS))
#define PEC_CR_SM0_TXF_CLR                                      PEC_CR_SM0_TXF_CLR
#define PEC_CR_SM0_TXF_CLR_POS                                  (30U)
#define PEC_CR_SM0_TXF_CLR_LEN                                  (1U)
#define PEC_CR_SM0_TXF_CLR_MSK                                  (((1U<<PEC_CR_SM0_TXF_CLR_LEN)-1)<<PEC_CR_SM0_TXF_CLR_POS)
#define PEC_CR_SM0_TXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM0_TXF_CLR_LEN)-1)<<PEC_CR_SM0_TXF_CLR_POS))
#define PEC_CR_SM0_RXF_CLR                                      PEC_CR_SM0_RXF_CLR
#define PEC_CR_SM0_RXF_CLR_POS                                  (31U)
#define PEC_CR_SM0_RXF_CLR_LEN                                  (1U)
#define PEC_CR_SM0_RXF_CLR_MSK                                  (((1U<<PEC_CR_SM0_RXF_CLR_LEN)-1)<<PEC_CR_SM0_RXF_CLR_POS)
#define PEC_CR_SM0_RXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM0_RXF_CLR_LEN)-1)<<PEC_CR_SM0_RXF_CLR_POS))

/* 0xD4 : SM0_ADDR */
#define PEC_SM0_ADDR_OFFSET                                     (0xD4)
#define PEC_ST_SM0_ADDR                                         PEC_ST_SM0_ADDR
#define PEC_ST_SM0_ADDR_POS                                     (0U)
#define PEC_ST_SM0_ADDR_LEN                                     (5U)
#define PEC_ST_SM0_ADDR_MSK                                     (((1U<<PEC_ST_SM0_ADDR_LEN)-1)<<PEC_ST_SM0_ADDR_POS)
#define PEC_ST_SM0_ADDR_UMSK                                    (~(((1U<<PEC_ST_SM0_ADDR_LEN)-1)<<PEC_ST_SM0_ADDR_POS))

/* 0xD8 : SM0_INSTR */
#define PEC_SM0_INSTR_OFFSET                                    (0xD8)
#define PEC_CR_SM0_INSTR                                        PEC_CR_SM0_INSTR
#define PEC_CR_SM0_INSTR_POS                                    (0U)
#define PEC_CR_SM0_INSTR_LEN                                    (16U)
#define PEC_CR_SM0_INSTR_MSK                                    (((1U<<PEC_CR_SM0_INSTR_LEN)-1)<<PEC_CR_SM0_INSTR_POS)
#define PEC_CR_SM0_INSTR_UMSK                                   (~(((1U<<PEC_CR_SM0_INSTR_LEN)-1)<<PEC_CR_SM0_INSTR_POS))
#define PEC_ST_SM0_INSTR                                        PEC_ST_SM0_INSTR
#define PEC_ST_SM0_INSTR_POS                                    (16U)
#define PEC_ST_SM0_INSTR_LEN                                    (16U)
#define PEC_ST_SM0_INSTR_MSK                                    (((1U<<PEC_ST_SM0_INSTR_LEN)-1)<<PEC_ST_SM0_INSTR_POS)
#define PEC_ST_SM0_INSTR_UMSK                                   (~(((1U<<PEC_ST_SM0_INSTR_LEN)-1)<<PEC_ST_SM0_INSTR_POS))

/* 0xDC : State machine pin control */
#define PEC_SM0_PINCTRL_OFFSET                                  (0xDC)
#define PEC_CR_SM0_OUT_BASE                                     PEC_CR_SM0_OUT_BASE
#define PEC_CR_SM0_OUT_BASE_POS                                 (0U)
#define PEC_CR_SM0_OUT_BASE_LEN                                 (5U)
#define PEC_CR_SM0_OUT_BASE_MSK                                 (((1U<<PEC_CR_SM0_OUT_BASE_LEN)-1)<<PEC_CR_SM0_OUT_BASE_POS)
#define PEC_CR_SM0_OUT_BASE_UMSK                                (~(((1U<<PEC_CR_SM0_OUT_BASE_LEN)-1)<<PEC_CR_SM0_OUT_BASE_POS))
#define PEC_CR_SM0_SET_BASE                                     PEC_CR_SM0_SET_BASE
#define PEC_CR_SM0_SET_BASE_POS                                 (5U)
#define PEC_CR_SM0_SET_BASE_LEN                                 (5U)
#define PEC_CR_SM0_SET_BASE_MSK                                 (((1U<<PEC_CR_SM0_SET_BASE_LEN)-1)<<PEC_CR_SM0_SET_BASE_POS)
#define PEC_CR_SM0_SET_BASE_UMSK                                (~(((1U<<PEC_CR_SM0_SET_BASE_LEN)-1)<<PEC_CR_SM0_SET_BASE_POS))
#define PEC_CR_SM0_SIDESET_BASE                                 PEC_CR_SM0_SIDESET_BASE
#define PEC_CR_SM0_SIDESET_BASE_POS                             (10U)
#define PEC_CR_SM0_SIDESET_BASE_LEN                             (5U)
#define PEC_CR_SM0_SIDESET_BASE_MSK                             (((1U<<PEC_CR_SM0_SIDESET_BASE_LEN)-1)<<PEC_CR_SM0_SIDESET_BASE_POS)
#define PEC_CR_SM0_SIDESET_BASE_UMSK                            (~(((1U<<PEC_CR_SM0_SIDESET_BASE_LEN)-1)<<PEC_CR_SM0_SIDESET_BASE_POS))
#define PEC_CR_SM0_IN_BASE                                      PEC_CR_SM0_IN_BASE
#define PEC_CR_SM0_IN_BASE_POS                                  (15U)
#define PEC_CR_SM0_IN_BASE_LEN                                  (5U)
#define PEC_CR_SM0_IN_BASE_MSK                                  (((1U<<PEC_CR_SM0_IN_BASE_LEN)-1)<<PEC_CR_SM0_IN_BASE_POS)
#define PEC_CR_SM0_IN_BASE_UMSK                                 (~(((1U<<PEC_CR_SM0_IN_BASE_LEN)-1)<<PEC_CR_SM0_IN_BASE_POS))
#define PEC_CR_SM0_OUT_COUNT                                    PEC_CR_SM0_OUT_COUNT
#define PEC_CR_SM0_OUT_COUNT_POS                                (20U)
#define PEC_CR_SM0_OUT_COUNT_LEN                                (6U)
#define PEC_CR_SM0_OUT_COUNT_MSK                                (((1U<<PEC_CR_SM0_OUT_COUNT_LEN)-1)<<PEC_CR_SM0_OUT_COUNT_POS)
#define PEC_CR_SM0_OUT_COUNT_UMSK                               (~(((1U<<PEC_CR_SM0_OUT_COUNT_LEN)-1)<<PEC_CR_SM0_OUT_COUNT_POS))
#define PEC_CR_SM0_SET_COUNT                                    PEC_CR_SM0_SET_COUNT
#define PEC_CR_SM0_SET_COUNT_POS                                (26U)
#define PEC_CR_SM0_SET_COUNT_LEN                                (3U)
#define PEC_CR_SM0_SET_COUNT_MSK                                (((1U<<PEC_CR_SM0_SET_COUNT_LEN)-1)<<PEC_CR_SM0_SET_COUNT_POS)
#define PEC_CR_SM0_SET_COUNT_UMSK                               (~(((1U<<PEC_CR_SM0_SET_COUNT_LEN)-1)<<PEC_CR_SM0_SET_COUNT_POS))
#define PEC_CR_SM0_SIDESET_COUNT                                PEC_CR_SM0_SIDESET_COUNT
#define PEC_CR_SM0_SIDESET_COUNT_POS                            (29U)
#define PEC_CR_SM0_SIDESET_COUNT_LEN                            (3U)
#define PEC_CR_SM0_SIDESET_COUNT_MSK                            (((1U<<PEC_CR_SM0_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM0_SIDESET_COUNT_POS)
#define PEC_CR_SM0_SIDESET_COUNT_UMSK                           (~(((1U<<PEC_CR_SM0_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM0_SIDESET_COUNT_POS))

/* 0xE0 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
#define PEC_SM1_CLKDIV_OFFSET                                   (0xE0)
#define PEC_CR_SM1_FRAC                                         PEC_CR_SM1_FRAC
#define PEC_CR_SM1_FRAC_POS                                     (8U)
#define PEC_CR_SM1_FRAC_LEN                                     (8U)
#define PEC_CR_SM1_FRAC_MSK                                     (((1U<<PEC_CR_SM1_FRAC_LEN)-1)<<PEC_CR_SM1_FRAC_POS)
#define PEC_CR_SM1_FRAC_UMSK                                    (~(((1U<<PEC_CR_SM1_FRAC_LEN)-1)<<PEC_CR_SM1_FRAC_POS))
#define PEC_CR_SM1_INT                                          PEC_CR_SM1_INT
#define PEC_CR_SM1_INT_POS                                      (16U)
#define PEC_CR_SM1_INT_LEN                                      (16U)
#define PEC_CR_SM1_INT_MSK                                      (((1U<<PEC_CR_SM1_INT_LEN)-1)<<PEC_CR_SM1_INT_POS)
#define PEC_CR_SM1_INT_UMSK                                     (~(((1U<<PEC_CR_SM1_INT_LEN)-1)<<PEC_CR_SM1_INT_POS))

/* 0xE4 : Execution/behavioural settings for state machine N */
#define PEC_SM1_EXECCTRL_OFFSET                                 (0xE4)
#define PEC_CR_SM1_STATUS_N                                     PEC_CR_SM1_STATUS_N
#define PEC_CR_SM1_STATUS_N_POS                                 (0U)
#define PEC_CR_SM1_STATUS_N_LEN                                 (4U)
#define PEC_CR_SM1_STATUS_N_MSK                                 (((1U<<PEC_CR_SM1_STATUS_N_LEN)-1)<<PEC_CR_SM1_STATUS_N_POS)
#define PEC_CR_SM1_STATUS_N_UMSK                                (~(((1U<<PEC_CR_SM1_STATUS_N_LEN)-1)<<PEC_CR_SM1_STATUS_N_POS))
#define PEC_CR_SM1_STATUS_SEL                                   PEC_CR_SM1_STATUS_SEL
#define PEC_CR_SM1_STATUS_SEL_POS                               (4U)
#define PEC_CR_SM1_STATUS_SEL_LEN                               (1U)
#define PEC_CR_SM1_STATUS_SEL_MSK                               (((1U<<PEC_CR_SM1_STATUS_SEL_LEN)-1)<<PEC_CR_SM1_STATUS_SEL_POS)
#define PEC_CR_SM1_STATUS_SEL_UMSK                              (~(((1U<<PEC_CR_SM1_STATUS_SEL_LEN)-1)<<PEC_CR_SM1_STATUS_SEL_POS))
#define PEC_CR_SM1_WRAP_BOTTOM                                  PEC_CR_SM1_WRAP_BOTTOM
#define PEC_CR_SM1_WRAP_BOTTOM_POS                              (7U)
#define PEC_CR_SM1_WRAP_BOTTOM_LEN                              (5U)
#define PEC_CR_SM1_WRAP_BOTTOM_MSK                              (((1U<<PEC_CR_SM1_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM1_WRAP_BOTTOM_POS)
#define PEC_CR_SM1_WRAP_BOTTOM_UMSK                             (~(((1U<<PEC_CR_SM1_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM1_WRAP_BOTTOM_POS))
#define PEC_CR_SM1_WRAP_TOP                                     PEC_CR_SM1_WRAP_TOP
#define PEC_CR_SM1_WRAP_TOP_POS                                 (12U)
#define PEC_CR_SM1_WRAP_TOP_LEN                                 (5U)
#define PEC_CR_SM1_WRAP_TOP_MSK                                 (((1U<<PEC_CR_SM1_WRAP_TOP_LEN)-1)<<PEC_CR_SM1_WRAP_TOP_POS)
#define PEC_CR_SM1_WRAP_TOP_UMSK                                (~(((1U<<PEC_CR_SM1_WRAP_TOP_LEN)-1)<<PEC_CR_SM1_WRAP_TOP_POS))
#define PEC_CR_SM1_JMP_PIN                                      PEC_CR_SM1_JMP_PIN
#define PEC_CR_SM1_JMP_PIN_POS                                  (24U)
#define PEC_CR_SM1_JMP_PIN_LEN                                  (5U)
#define PEC_CR_SM1_JMP_PIN_MSK                                  (((1U<<PEC_CR_SM1_JMP_PIN_LEN)-1)<<PEC_CR_SM1_JMP_PIN_POS)
#define PEC_CR_SM1_JMP_PIN_UMSK                                 (~(((1U<<PEC_CR_SM1_JMP_PIN_LEN)-1)<<PEC_CR_SM1_JMP_PIN_POS))
#define PEC_CR_SM1_SIDE_PINDIR                                  PEC_CR_SM1_SIDE_PINDIR
#define PEC_CR_SM1_SIDE_PINDIR_POS                              (29U)
#define PEC_CR_SM1_SIDE_PINDIR_LEN                              (1U)
#define PEC_CR_SM1_SIDE_PINDIR_MSK                              (((1U<<PEC_CR_SM1_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM1_SIDE_PINDIR_POS)
#define PEC_CR_SM1_SIDE_PINDIR_UMSK                             (~(((1U<<PEC_CR_SM1_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM1_SIDE_PINDIR_POS))
#define PEC_CR_SM1_SIDE_EN                                      PEC_CR_SM1_SIDE_EN
#define PEC_CR_SM1_SIDE_EN_POS                                  (30U)
#define PEC_CR_SM1_SIDE_EN_LEN                                  (1U)
#define PEC_CR_SM1_SIDE_EN_MSK                                  (((1U<<PEC_CR_SM1_SIDE_EN_LEN)-1)<<PEC_CR_SM1_SIDE_EN_POS)
#define PEC_CR_SM1_SIDE_EN_UMSK                                 (~(((1U<<PEC_CR_SM1_SIDE_EN_LEN)-1)<<PEC_CR_SM1_SIDE_EN_POS))
#define PEC_ST_SM1_EXEC_STALLED                                 PEC_ST_SM1_EXEC_STALLED
#define PEC_ST_SM1_EXEC_STALLED_POS                             (31U)
#define PEC_ST_SM1_EXEC_STALLED_LEN                             (1U)
#define PEC_ST_SM1_EXEC_STALLED_MSK                             (((1U<<PEC_ST_SM1_EXEC_STALLED_LEN)-1)<<PEC_ST_SM1_EXEC_STALLED_POS)
#define PEC_ST_SM1_EXEC_STALLED_UMSK                            (~(((1U<<PEC_ST_SM1_EXEC_STALLED_LEN)-1)<<PEC_ST_SM1_EXEC_STALLED_POS))

/* 0xE8 : Control behaviour of the input/output shift registers for state machine N */
#define PEC_SM1_SHIFTCTRL_OFFSET                                (0xE8)
#define PEC_CR_SM1_TXF_THR                                      PEC_CR_SM1_TXF_THR
#define PEC_CR_SM1_TXF_THR_POS                                  (0U)
#define PEC_CR_SM1_TXF_THR_LEN                                  (3U)
#define PEC_CR_SM1_TXF_THR_MSK                                  (((1U<<PEC_CR_SM1_TXF_THR_LEN)-1)<<PEC_CR_SM1_TXF_THR_POS)
#define PEC_CR_SM1_TXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM1_TXF_THR_LEN)-1)<<PEC_CR_SM1_TXF_THR_POS))
#define PEC_ST_SM1_TXF_RDY                                      PEC_ST_SM1_TXF_RDY
#define PEC_ST_SM1_TXF_RDY_POS                                  (7U)
#define PEC_ST_SM1_TXF_RDY_LEN                                  (1U)
#define PEC_ST_SM1_TXF_RDY_MSK                                  (((1U<<PEC_ST_SM1_TXF_RDY_LEN)-1)<<PEC_ST_SM1_TXF_RDY_POS)
#define PEC_ST_SM1_TXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM1_TXF_RDY_LEN)-1)<<PEC_ST_SM1_TXF_RDY_POS))
#define PEC_CR_SM1_RXF_THR                                      PEC_CR_SM1_RXF_THR
#define PEC_CR_SM1_RXF_THR_POS                                  (8U)
#define PEC_CR_SM1_RXF_THR_LEN                                  (3U)
#define PEC_CR_SM1_RXF_THR_MSK                                  (((1U<<PEC_CR_SM1_RXF_THR_LEN)-1)<<PEC_CR_SM1_RXF_THR_POS)
#define PEC_CR_SM1_RXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM1_RXF_THR_LEN)-1)<<PEC_CR_SM1_RXF_THR_POS))
#define PEC_ST_SM1_RXF_RDY                                      PEC_ST_SM1_RXF_RDY
#define PEC_ST_SM1_RXF_RDY_POS                                  (15U)
#define PEC_ST_SM1_RXF_RDY_LEN                                  (1U)
#define PEC_ST_SM1_RXF_RDY_MSK                                  (((1U<<PEC_ST_SM1_RXF_RDY_LEN)-1)<<PEC_ST_SM1_RXF_RDY_POS)
#define PEC_ST_SM1_RXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM1_RXF_RDY_LEN)-1)<<PEC_ST_SM1_RXF_RDY_POS))
#define PEC_CR_SM1_AUTOPUSH                                     PEC_CR_SM1_AUTOPUSH
#define PEC_CR_SM1_AUTOPUSH_POS                                 (16U)
#define PEC_CR_SM1_AUTOPUSH_LEN                                 (1U)
#define PEC_CR_SM1_AUTOPUSH_MSK                                 (((1U<<PEC_CR_SM1_AUTOPUSH_LEN)-1)<<PEC_CR_SM1_AUTOPUSH_POS)
#define PEC_CR_SM1_AUTOPUSH_UMSK                                (~(((1U<<PEC_CR_SM1_AUTOPUSH_LEN)-1)<<PEC_CR_SM1_AUTOPUSH_POS))
#define PEC_CR_SM1_AUTOPULL                                     PEC_CR_SM1_AUTOPULL
#define PEC_CR_SM1_AUTOPULL_POS                                 (17U)
#define PEC_CR_SM1_AUTOPULL_LEN                                 (1U)
#define PEC_CR_SM1_AUTOPULL_MSK                                 (((1U<<PEC_CR_SM1_AUTOPULL_LEN)-1)<<PEC_CR_SM1_AUTOPULL_POS)
#define PEC_CR_SM1_AUTOPULL_UMSK                                (~(((1U<<PEC_CR_SM1_AUTOPULL_LEN)-1)<<PEC_CR_SM1_AUTOPULL_POS))
#define PEC_CR_SM1_IN_SHIFTDIR                                  PEC_CR_SM1_IN_SHIFTDIR
#define PEC_CR_SM1_IN_SHIFTDIR_POS                              (18U)
#define PEC_CR_SM1_IN_SHIFTDIR_LEN                              (1U)
#define PEC_CR_SM1_IN_SHIFTDIR_MSK                              (((1U<<PEC_CR_SM1_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM1_IN_SHIFTDIR_POS)
#define PEC_CR_SM1_IN_SHIFTDIR_UMSK                             (~(((1U<<PEC_CR_SM1_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM1_IN_SHIFTDIR_POS))
#define PEC_CR_SM1_OUT_SHIFTDIR                                 PEC_CR_SM1_OUT_SHIFTDIR
#define PEC_CR_SM1_OUT_SHIFTDIR_POS                             (19U)
#define PEC_CR_SM1_OUT_SHIFTDIR_LEN                             (1U)
#define PEC_CR_SM1_OUT_SHIFTDIR_MSK                             (((1U<<PEC_CR_SM1_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM1_OUT_SHIFTDIR_POS)
#define PEC_CR_SM1_OUT_SHIFTDIR_UMSK                            (~(((1U<<PEC_CR_SM1_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM1_OUT_SHIFTDIR_POS))
#define PEC_CR_SM1_PUSH_THRESH                                  PEC_CR_SM1_PUSH_THRESH
#define PEC_CR_SM1_PUSH_THRESH_POS                              (20U)
#define PEC_CR_SM1_PUSH_THRESH_LEN                              (5U)
#define PEC_CR_SM1_PUSH_THRESH_MSK                              (((1U<<PEC_CR_SM1_PUSH_THRESH_LEN)-1)<<PEC_CR_SM1_PUSH_THRESH_POS)
#define PEC_CR_SM1_PUSH_THRESH_UMSK                             (~(((1U<<PEC_CR_SM1_PUSH_THRESH_LEN)-1)<<PEC_CR_SM1_PUSH_THRESH_POS))
#define PEC_CR_SM1_PULL_THRESH                                  PEC_CR_SM1_PULL_THRESH
#define PEC_CR_SM1_PULL_THRESH_POS                              (25U)
#define PEC_CR_SM1_PULL_THRESH_LEN                              (5U)
#define PEC_CR_SM1_PULL_THRESH_MSK                              (((1U<<PEC_CR_SM1_PULL_THRESH_LEN)-1)<<PEC_CR_SM1_PULL_THRESH_POS)
#define PEC_CR_SM1_PULL_THRESH_UMSK                             (~(((1U<<PEC_CR_SM1_PULL_THRESH_LEN)-1)<<PEC_CR_SM1_PULL_THRESH_POS))
#define PEC_CR_SM1_TXF_CLR                                      PEC_CR_SM1_TXF_CLR
#define PEC_CR_SM1_TXF_CLR_POS                                  (30U)
#define PEC_CR_SM1_TXF_CLR_LEN                                  (1U)
#define PEC_CR_SM1_TXF_CLR_MSK                                  (((1U<<PEC_CR_SM1_TXF_CLR_LEN)-1)<<PEC_CR_SM1_TXF_CLR_POS)
#define PEC_CR_SM1_TXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM1_TXF_CLR_LEN)-1)<<PEC_CR_SM1_TXF_CLR_POS))
#define PEC_CR_SM1_RXF_CLR                                      PEC_CR_SM1_RXF_CLR
#define PEC_CR_SM1_RXF_CLR_POS                                  (31U)
#define PEC_CR_SM1_RXF_CLR_LEN                                  (1U)
#define PEC_CR_SM1_RXF_CLR_MSK                                  (((1U<<PEC_CR_SM1_RXF_CLR_LEN)-1)<<PEC_CR_SM1_RXF_CLR_POS)
#define PEC_CR_SM1_RXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM1_RXF_CLR_LEN)-1)<<PEC_CR_SM1_RXF_CLR_POS))

/* 0xEC : SM1_ADDR */
#define PEC_SM1_ADDR_OFFSET                                     (0xEC)
#define PEC_ST_SM1_ADDR                                         PEC_ST_SM1_ADDR
#define PEC_ST_SM1_ADDR_POS                                     (0U)
#define PEC_ST_SM1_ADDR_LEN                                     (5U)
#define PEC_ST_SM1_ADDR_MSK                                     (((1U<<PEC_ST_SM1_ADDR_LEN)-1)<<PEC_ST_SM1_ADDR_POS)
#define PEC_ST_SM1_ADDR_UMSK                                    (~(((1U<<PEC_ST_SM1_ADDR_LEN)-1)<<PEC_ST_SM1_ADDR_POS))

/* 0xF0 : SM1_INSTR */
#define PEC_SM1_INSTR_OFFSET                                    (0xF0)
#define PEC_CR_SM1_INSTR                                        PEC_CR_SM1_INSTR
#define PEC_CR_SM1_INSTR_POS                                    (0U)
#define PEC_CR_SM1_INSTR_LEN                                    (16U)
#define PEC_CR_SM1_INSTR_MSK                                    (((1U<<PEC_CR_SM1_INSTR_LEN)-1)<<PEC_CR_SM1_INSTR_POS)
#define PEC_CR_SM1_INSTR_UMSK                                   (~(((1U<<PEC_CR_SM1_INSTR_LEN)-1)<<PEC_CR_SM1_INSTR_POS))
#define PEC_ST_SM1_INSTR                                        PEC_ST_SM1_INSTR
#define PEC_ST_SM1_INSTR_POS                                    (16U)
#define PEC_ST_SM1_INSTR_LEN                                    (16U)
#define PEC_ST_SM1_INSTR_MSK                                    (((1U<<PEC_ST_SM1_INSTR_LEN)-1)<<PEC_ST_SM1_INSTR_POS)
#define PEC_ST_SM1_INSTR_UMSK                                   (~(((1U<<PEC_ST_SM1_INSTR_LEN)-1)<<PEC_ST_SM1_INSTR_POS))

/* 0xF4 : State machine pin control */
#define PEC_SM1_PINCTRL_OFFSET                                  (0xF4)
#define PEC_CR_SM1_OUT_BASE                                     PEC_CR_SM1_OUT_BASE
#define PEC_CR_SM1_OUT_BASE_POS                                 (0U)
#define PEC_CR_SM1_OUT_BASE_LEN                                 (5U)
#define PEC_CR_SM1_OUT_BASE_MSK                                 (((1U<<PEC_CR_SM1_OUT_BASE_LEN)-1)<<PEC_CR_SM1_OUT_BASE_POS)
#define PEC_CR_SM1_OUT_BASE_UMSK                                (~(((1U<<PEC_CR_SM1_OUT_BASE_LEN)-1)<<PEC_CR_SM1_OUT_BASE_POS))
#define PEC_CR_SM1_SET_BASE                                     PEC_CR_SM1_SET_BASE
#define PEC_CR_SM1_SET_BASE_POS                                 (5U)
#define PEC_CR_SM1_SET_BASE_LEN                                 (5U)
#define PEC_CR_SM1_SET_BASE_MSK                                 (((1U<<PEC_CR_SM1_SET_BASE_LEN)-1)<<PEC_CR_SM1_SET_BASE_POS)
#define PEC_CR_SM1_SET_BASE_UMSK                                (~(((1U<<PEC_CR_SM1_SET_BASE_LEN)-1)<<PEC_CR_SM1_SET_BASE_POS))
#define PEC_CR_SM1_SIDESET_BASE                                 PEC_CR_SM1_SIDESET_BASE
#define PEC_CR_SM1_SIDESET_BASE_POS                             (10U)
#define PEC_CR_SM1_SIDESET_BASE_LEN                             (5U)
#define PEC_CR_SM1_SIDESET_BASE_MSK                             (((1U<<PEC_CR_SM1_SIDESET_BASE_LEN)-1)<<PEC_CR_SM1_SIDESET_BASE_POS)
#define PEC_CR_SM1_SIDESET_BASE_UMSK                            (~(((1U<<PEC_CR_SM1_SIDESET_BASE_LEN)-1)<<PEC_CR_SM1_SIDESET_BASE_POS))
#define PEC_CR_SM1_IN_BASE                                      PEC_CR_SM1_IN_BASE
#define PEC_CR_SM1_IN_BASE_POS                                  (15U)
#define PEC_CR_SM1_IN_BASE_LEN                                  (5U)
#define PEC_CR_SM1_IN_BASE_MSK                                  (((1U<<PEC_CR_SM1_IN_BASE_LEN)-1)<<PEC_CR_SM1_IN_BASE_POS)
#define PEC_CR_SM1_IN_BASE_UMSK                                 (~(((1U<<PEC_CR_SM1_IN_BASE_LEN)-1)<<PEC_CR_SM1_IN_BASE_POS))
#define PEC_CR_SM1_OUT_COUNT                                    PEC_CR_SM1_OUT_COUNT
#define PEC_CR_SM1_OUT_COUNT_POS                                (20U)
#define PEC_CR_SM1_OUT_COUNT_LEN                                (6U)
#define PEC_CR_SM1_OUT_COUNT_MSK                                (((1U<<PEC_CR_SM1_OUT_COUNT_LEN)-1)<<PEC_CR_SM1_OUT_COUNT_POS)
#define PEC_CR_SM1_OUT_COUNT_UMSK                               (~(((1U<<PEC_CR_SM1_OUT_COUNT_LEN)-1)<<PEC_CR_SM1_OUT_COUNT_POS))
#define PEC_CR_SM1_SET_COUNT                                    PEC_CR_SM1_SET_COUNT
#define PEC_CR_SM1_SET_COUNT_POS                                (26U)
#define PEC_CR_SM1_SET_COUNT_LEN                                (3U)
#define PEC_CR_SM1_SET_COUNT_MSK                                (((1U<<PEC_CR_SM1_SET_COUNT_LEN)-1)<<PEC_CR_SM1_SET_COUNT_POS)
#define PEC_CR_SM1_SET_COUNT_UMSK                               (~(((1U<<PEC_CR_SM1_SET_COUNT_LEN)-1)<<PEC_CR_SM1_SET_COUNT_POS))
#define PEC_CR_SM1_SIDESET_COUNT                                PEC_CR_SM1_SIDESET_COUNT
#define PEC_CR_SM1_SIDESET_COUNT_POS                            (29U)
#define PEC_CR_SM1_SIDESET_COUNT_LEN                            (3U)
#define PEC_CR_SM1_SIDESET_COUNT_MSK                            (((1U<<PEC_CR_SM1_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM1_SIDESET_COUNT_POS)
#define PEC_CR_SM1_SIDESET_COUNT_UMSK                           (~(((1U<<PEC_CR_SM1_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM1_SIDESET_COUNT_POS))

/* 0xF8 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
#define PEC_SM2_CLKDIV_OFFSET                                   (0xF8)
#define PEC_CR_SM2_FRAC                                         PEC_CR_SM2_FRAC
#define PEC_CR_SM2_FRAC_POS                                     (8U)
#define PEC_CR_SM2_FRAC_LEN                                     (8U)
#define PEC_CR_SM2_FRAC_MSK                                     (((1U<<PEC_CR_SM2_FRAC_LEN)-1)<<PEC_CR_SM2_FRAC_POS)
#define PEC_CR_SM2_FRAC_UMSK                                    (~(((1U<<PEC_CR_SM2_FRAC_LEN)-1)<<PEC_CR_SM2_FRAC_POS))
#define PEC_CR_SM2_INT                                          PEC_CR_SM2_INT
#define PEC_CR_SM2_INT_POS                                      (16U)
#define PEC_CR_SM2_INT_LEN                                      (16U)
#define PEC_CR_SM2_INT_MSK                                      (((1U<<PEC_CR_SM2_INT_LEN)-1)<<PEC_CR_SM2_INT_POS)
#define PEC_CR_SM2_INT_UMSK                                     (~(((1U<<PEC_CR_SM2_INT_LEN)-1)<<PEC_CR_SM2_INT_POS))

/* 0xFC : Execution/behavioural settings for state machine N */
#define PEC_SM2_EXECCTRL_OFFSET                                 (0xFC)
#define PEC_CR_SM2_STATUS_N                                     PEC_CR_SM2_STATUS_N
#define PEC_CR_SM2_STATUS_N_POS                                 (0U)
#define PEC_CR_SM2_STATUS_N_LEN                                 (4U)
#define PEC_CR_SM2_STATUS_N_MSK                                 (((1U<<PEC_CR_SM2_STATUS_N_LEN)-1)<<PEC_CR_SM2_STATUS_N_POS)
#define PEC_CR_SM2_STATUS_N_UMSK                                (~(((1U<<PEC_CR_SM2_STATUS_N_LEN)-1)<<PEC_CR_SM2_STATUS_N_POS))
#define PEC_CR_SM2_STATUS_SEL                                   PEC_CR_SM2_STATUS_SEL
#define PEC_CR_SM2_STATUS_SEL_POS                               (4U)
#define PEC_CR_SM2_STATUS_SEL_LEN                               (1U)
#define PEC_CR_SM2_STATUS_SEL_MSK                               (((1U<<PEC_CR_SM2_STATUS_SEL_LEN)-1)<<PEC_CR_SM2_STATUS_SEL_POS)
#define PEC_CR_SM2_STATUS_SEL_UMSK                              (~(((1U<<PEC_CR_SM2_STATUS_SEL_LEN)-1)<<PEC_CR_SM2_STATUS_SEL_POS))
#define PEC_CR_SM2_WRAP_BOTTOM                                  PEC_CR_SM2_WRAP_BOTTOM
#define PEC_CR_SM2_WRAP_BOTTOM_POS                              (7U)
#define PEC_CR_SM2_WRAP_BOTTOM_LEN                              (5U)
#define PEC_CR_SM2_WRAP_BOTTOM_MSK                              (((1U<<PEC_CR_SM2_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM2_WRAP_BOTTOM_POS)
#define PEC_CR_SM2_WRAP_BOTTOM_UMSK                             (~(((1U<<PEC_CR_SM2_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM2_WRAP_BOTTOM_POS))
#define PEC_CR_SM2_WRAP_TOP                                     PEC_CR_SM2_WRAP_TOP
#define PEC_CR_SM2_WRAP_TOP_POS                                 (12U)
#define PEC_CR_SM2_WRAP_TOP_LEN                                 (5U)
#define PEC_CR_SM2_WRAP_TOP_MSK                                 (((1U<<PEC_CR_SM2_WRAP_TOP_LEN)-1)<<PEC_CR_SM2_WRAP_TOP_POS)
#define PEC_CR_SM2_WRAP_TOP_UMSK                                (~(((1U<<PEC_CR_SM2_WRAP_TOP_LEN)-1)<<PEC_CR_SM2_WRAP_TOP_POS))
#define PEC_CR_SM2_JMP_PIN                                      PEC_CR_SM2_JMP_PIN
#define PEC_CR_SM2_JMP_PIN_POS                                  (24U)
#define PEC_CR_SM2_JMP_PIN_LEN                                  (5U)
#define PEC_CR_SM2_JMP_PIN_MSK                                  (((1U<<PEC_CR_SM2_JMP_PIN_LEN)-1)<<PEC_CR_SM2_JMP_PIN_POS)
#define PEC_CR_SM2_JMP_PIN_UMSK                                 (~(((1U<<PEC_CR_SM2_JMP_PIN_LEN)-1)<<PEC_CR_SM2_JMP_PIN_POS))
#define PEC_CR_SM2_SIDE_PINDIR                                  PEC_CR_SM2_SIDE_PINDIR
#define PEC_CR_SM2_SIDE_PINDIR_POS                              (29U)
#define PEC_CR_SM2_SIDE_PINDIR_LEN                              (1U)
#define PEC_CR_SM2_SIDE_PINDIR_MSK                              (((1U<<PEC_CR_SM2_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM2_SIDE_PINDIR_POS)
#define PEC_CR_SM2_SIDE_PINDIR_UMSK                             (~(((1U<<PEC_CR_SM2_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM2_SIDE_PINDIR_POS))
#define PEC_CR_SM2_SIDE_EN                                      PEC_CR_SM2_SIDE_EN
#define PEC_CR_SM2_SIDE_EN_POS                                  (30U)
#define PEC_CR_SM2_SIDE_EN_LEN                                  (1U)
#define PEC_CR_SM2_SIDE_EN_MSK                                  (((1U<<PEC_CR_SM2_SIDE_EN_LEN)-1)<<PEC_CR_SM2_SIDE_EN_POS)
#define PEC_CR_SM2_SIDE_EN_UMSK                                 (~(((1U<<PEC_CR_SM2_SIDE_EN_LEN)-1)<<PEC_CR_SM2_SIDE_EN_POS))
#define PEC_ST_SM2_EXEC_STALLED                                 PEC_ST_SM2_EXEC_STALLED
#define PEC_ST_SM2_EXEC_STALLED_POS                             (31U)
#define PEC_ST_SM2_EXEC_STALLED_LEN                             (1U)
#define PEC_ST_SM2_EXEC_STALLED_MSK                             (((1U<<PEC_ST_SM2_EXEC_STALLED_LEN)-1)<<PEC_ST_SM2_EXEC_STALLED_POS)
#define PEC_ST_SM2_EXEC_STALLED_UMSK                            (~(((1U<<PEC_ST_SM2_EXEC_STALLED_LEN)-1)<<PEC_ST_SM2_EXEC_STALLED_POS))

/* 0x100 : Control behaviour of the input/output shift registers for state machine N */
#define PEC_SM2_SHIFTCTRL_OFFSET                                (0x100)
#define PEC_CR_SM2_TXF_THR                                      PEC_CR_SM2_TXF_THR
#define PEC_CR_SM2_TXF_THR_POS                                  (0U)
#define PEC_CR_SM2_TXF_THR_LEN                                  (3U)
#define PEC_CR_SM2_TXF_THR_MSK                                  (((1U<<PEC_CR_SM2_TXF_THR_LEN)-1)<<PEC_CR_SM2_TXF_THR_POS)
#define PEC_CR_SM2_TXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM2_TXF_THR_LEN)-1)<<PEC_CR_SM2_TXF_THR_POS))
#define PEC_ST_SM2_TXF_RDY                                      PEC_ST_SM2_TXF_RDY
#define PEC_ST_SM2_TXF_RDY_POS                                  (7U)
#define PEC_ST_SM2_TXF_RDY_LEN                                  (1U)
#define PEC_ST_SM2_TXF_RDY_MSK                                  (((1U<<PEC_ST_SM2_TXF_RDY_LEN)-1)<<PEC_ST_SM2_TXF_RDY_POS)
#define PEC_ST_SM2_TXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM2_TXF_RDY_LEN)-1)<<PEC_ST_SM2_TXF_RDY_POS))
#define PEC_CR_SM2_RXF_THR                                      PEC_CR_SM2_RXF_THR
#define PEC_CR_SM2_RXF_THR_POS                                  (8U)
#define PEC_CR_SM2_RXF_THR_LEN                                  (3U)
#define PEC_CR_SM2_RXF_THR_MSK                                  (((1U<<PEC_CR_SM2_RXF_THR_LEN)-1)<<PEC_CR_SM2_RXF_THR_POS)
#define PEC_CR_SM2_RXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM2_RXF_THR_LEN)-1)<<PEC_CR_SM2_RXF_THR_POS))
#define PEC_ST_SM2_RXF_RDY                                      PEC_ST_SM2_RXF_RDY
#define PEC_ST_SM2_RXF_RDY_POS                                  (15U)
#define PEC_ST_SM2_RXF_RDY_LEN                                  (1U)
#define PEC_ST_SM2_RXF_RDY_MSK                                  (((1U<<PEC_ST_SM2_RXF_RDY_LEN)-1)<<PEC_ST_SM2_RXF_RDY_POS)
#define PEC_ST_SM2_RXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM2_RXF_RDY_LEN)-1)<<PEC_ST_SM2_RXF_RDY_POS))
#define PEC_CR_SM2_AUTOPUSH                                     PEC_CR_SM2_AUTOPUSH
#define PEC_CR_SM2_AUTOPUSH_POS                                 (16U)
#define PEC_CR_SM2_AUTOPUSH_LEN                                 (1U)
#define PEC_CR_SM2_AUTOPUSH_MSK                                 (((1U<<PEC_CR_SM2_AUTOPUSH_LEN)-1)<<PEC_CR_SM2_AUTOPUSH_POS)
#define PEC_CR_SM2_AUTOPUSH_UMSK                                (~(((1U<<PEC_CR_SM2_AUTOPUSH_LEN)-1)<<PEC_CR_SM2_AUTOPUSH_POS))
#define PEC_CR_SM2_AUTOPULL                                     PEC_CR_SM2_AUTOPULL
#define PEC_CR_SM2_AUTOPULL_POS                                 (17U)
#define PEC_CR_SM2_AUTOPULL_LEN                                 (1U)
#define PEC_CR_SM2_AUTOPULL_MSK                                 (((1U<<PEC_CR_SM2_AUTOPULL_LEN)-1)<<PEC_CR_SM2_AUTOPULL_POS)
#define PEC_CR_SM2_AUTOPULL_UMSK                                (~(((1U<<PEC_CR_SM2_AUTOPULL_LEN)-1)<<PEC_CR_SM2_AUTOPULL_POS))
#define PEC_CR_SM2_IN_SHIFTDIR                                  PEC_CR_SM2_IN_SHIFTDIR
#define PEC_CR_SM2_IN_SHIFTDIR_POS                              (18U)
#define PEC_CR_SM2_IN_SHIFTDIR_LEN                              (1U)
#define PEC_CR_SM2_IN_SHIFTDIR_MSK                              (((1U<<PEC_CR_SM2_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM2_IN_SHIFTDIR_POS)
#define PEC_CR_SM2_IN_SHIFTDIR_UMSK                             (~(((1U<<PEC_CR_SM2_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM2_IN_SHIFTDIR_POS))
#define PEC_CR_SM2_OUT_SHIFTDIR                                 PEC_CR_SM2_OUT_SHIFTDIR
#define PEC_CR_SM2_OUT_SHIFTDIR_POS                             (19U)
#define PEC_CR_SM2_OUT_SHIFTDIR_LEN                             (1U)
#define PEC_CR_SM2_OUT_SHIFTDIR_MSK                             (((1U<<PEC_CR_SM2_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM2_OUT_SHIFTDIR_POS)
#define PEC_CR_SM2_OUT_SHIFTDIR_UMSK                            (~(((1U<<PEC_CR_SM2_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM2_OUT_SHIFTDIR_POS))
#define PEC_CR_SM2_PUSH_THRESH                                  PEC_CR_SM2_PUSH_THRESH
#define PEC_CR_SM2_PUSH_THRESH_POS                              (20U)
#define PEC_CR_SM2_PUSH_THRESH_LEN                              (5U)
#define PEC_CR_SM2_PUSH_THRESH_MSK                              (((1U<<PEC_CR_SM2_PUSH_THRESH_LEN)-1)<<PEC_CR_SM2_PUSH_THRESH_POS)
#define PEC_CR_SM2_PUSH_THRESH_UMSK                             (~(((1U<<PEC_CR_SM2_PUSH_THRESH_LEN)-1)<<PEC_CR_SM2_PUSH_THRESH_POS))
#define PEC_CR_SM2_PULL_THRESH                                  PEC_CR_SM2_PULL_THRESH
#define PEC_CR_SM2_PULL_THRESH_POS                              (25U)
#define PEC_CR_SM2_PULL_THRESH_LEN                              (5U)
#define PEC_CR_SM2_PULL_THRESH_MSK                              (((1U<<PEC_CR_SM2_PULL_THRESH_LEN)-1)<<PEC_CR_SM2_PULL_THRESH_POS)
#define PEC_CR_SM2_PULL_THRESH_UMSK                             (~(((1U<<PEC_CR_SM2_PULL_THRESH_LEN)-1)<<PEC_CR_SM2_PULL_THRESH_POS))
#define PEC_CR_SM2_TXF_CLR                                      PEC_CR_SM2_TXF_CLR
#define PEC_CR_SM2_TXF_CLR_POS                                  (30U)
#define PEC_CR_SM2_TXF_CLR_LEN                                  (1U)
#define PEC_CR_SM2_TXF_CLR_MSK                                  (((1U<<PEC_CR_SM2_TXF_CLR_LEN)-1)<<PEC_CR_SM2_TXF_CLR_POS)
#define PEC_CR_SM2_TXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM2_TXF_CLR_LEN)-1)<<PEC_CR_SM2_TXF_CLR_POS))
#define PEC_CR_SM2_RXF_CLR                                      PEC_CR_SM2_RXF_CLR
#define PEC_CR_SM2_RXF_CLR_POS                                  (31U)
#define PEC_CR_SM2_RXF_CLR_LEN                                  (1U)
#define PEC_CR_SM2_RXF_CLR_MSK                                  (((1U<<PEC_CR_SM2_RXF_CLR_LEN)-1)<<PEC_CR_SM2_RXF_CLR_POS)
#define PEC_CR_SM2_RXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM2_RXF_CLR_LEN)-1)<<PEC_CR_SM2_RXF_CLR_POS))

/* 0x104 : SM2_ADDR */
#define PEC_SM2_ADDR_OFFSET                                     (0x104)
#define PEC_ST_SM2_ADDR                                         PEC_ST_SM2_ADDR
#define PEC_ST_SM2_ADDR_POS                                     (0U)
#define PEC_ST_SM2_ADDR_LEN                                     (5U)
#define PEC_ST_SM2_ADDR_MSK                                     (((1U<<PEC_ST_SM2_ADDR_LEN)-1)<<PEC_ST_SM2_ADDR_POS)
#define PEC_ST_SM2_ADDR_UMSK                                    (~(((1U<<PEC_ST_SM2_ADDR_LEN)-1)<<PEC_ST_SM2_ADDR_POS))

/* 0x108 : SM2_INSTR */
#define PEC_SM2_INSTR_OFFSET                                    (0x108)
#define PEC_CR_SM2_INSTR                                        PEC_CR_SM2_INSTR
#define PEC_CR_SM2_INSTR_POS                                    (0U)
#define PEC_CR_SM2_INSTR_LEN                                    (16U)
#define PEC_CR_SM2_INSTR_MSK                                    (((1U<<PEC_CR_SM2_INSTR_LEN)-1)<<PEC_CR_SM2_INSTR_POS)
#define PEC_CR_SM2_INSTR_UMSK                                   (~(((1U<<PEC_CR_SM2_INSTR_LEN)-1)<<PEC_CR_SM2_INSTR_POS))
#define PEC_ST_SM2_INSTR                                        PEC_ST_SM2_INSTR
#define PEC_ST_SM2_INSTR_POS                                    (16U)
#define PEC_ST_SM2_INSTR_LEN                                    (16U)
#define PEC_ST_SM2_INSTR_MSK                                    (((1U<<PEC_ST_SM2_INSTR_LEN)-1)<<PEC_ST_SM2_INSTR_POS)
#define PEC_ST_SM2_INSTR_UMSK                                   (~(((1U<<PEC_ST_SM2_INSTR_LEN)-1)<<PEC_ST_SM2_INSTR_POS))

/* 0x10C : State machine pin control */
#define PEC_SM2_PINCTRL_OFFSET                                  (0x10C)
#define PEC_CR_SM2_OUT_BASE                                     PEC_CR_SM2_OUT_BASE
#define PEC_CR_SM2_OUT_BASE_POS                                 (0U)
#define PEC_CR_SM2_OUT_BASE_LEN                                 (5U)
#define PEC_CR_SM2_OUT_BASE_MSK                                 (((1U<<PEC_CR_SM2_OUT_BASE_LEN)-1)<<PEC_CR_SM2_OUT_BASE_POS)
#define PEC_CR_SM2_OUT_BASE_UMSK                                (~(((1U<<PEC_CR_SM2_OUT_BASE_LEN)-1)<<PEC_CR_SM2_OUT_BASE_POS))
#define PEC_CR_SM2_SET_BASE                                     PEC_CR_SM2_SET_BASE
#define PEC_CR_SM2_SET_BASE_POS                                 (5U)
#define PEC_CR_SM2_SET_BASE_LEN                                 (5U)
#define PEC_CR_SM2_SET_BASE_MSK                                 (((1U<<PEC_CR_SM2_SET_BASE_LEN)-1)<<PEC_CR_SM2_SET_BASE_POS)
#define PEC_CR_SM2_SET_BASE_UMSK                                (~(((1U<<PEC_CR_SM2_SET_BASE_LEN)-1)<<PEC_CR_SM2_SET_BASE_POS))
#define PEC_CR_SM2_SIDESET_BASE                                 PEC_CR_SM2_SIDESET_BASE
#define PEC_CR_SM2_SIDESET_BASE_POS                             (10U)
#define PEC_CR_SM2_SIDESET_BASE_LEN                             (5U)
#define PEC_CR_SM2_SIDESET_BASE_MSK                             (((1U<<PEC_CR_SM2_SIDESET_BASE_LEN)-1)<<PEC_CR_SM2_SIDESET_BASE_POS)
#define PEC_CR_SM2_SIDESET_BASE_UMSK                            (~(((1U<<PEC_CR_SM2_SIDESET_BASE_LEN)-1)<<PEC_CR_SM2_SIDESET_BASE_POS))
#define PEC_CR_SM2_IN_BASE                                      PEC_CR_SM2_IN_BASE
#define PEC_CR_SM2_IN_BASE_POS                                  (15U)
#define PEC_CR_SM2_IN_BASE_LEN                                  (5U)
#define PEC_CR_SM2_IN_BASE_MSK                                  (((1U<<PEC_CR_SM2_IN_BASE_LEN)-1)<<PEC_CR_SM2_IN_BASE_POS)
#define PEC_CR_SM2_IN_BASE_UMSK                                 (~(((1U<<PEC_CR_SM2_IN_BASE_LEN)-1)<<PEC_CR_SM2_IN_BASE_POS))
#define PEC_CR_SM2_OUT_COUNT                                    PEC_CR_SM2_OUT_COUNT
#define PEC_CR_SM2_OUT_COUNT_POS                                (20U)
#define PEC_CR_SM2_OUT_COUNT_LEN                                (6U)
#define PEC_CR_SM2_OUT_COUNT_MSK                                (((1U<<PEC_CR_SM2_OUT_COUNT_LEN)-1)<<PEC_CR_SM2_OUT_COUNT_POS)
#define PEC_CR_SM2_OUT_COUNT_UMSK                               (~(((1U<<PEC_CR_SM2_OUT_COUNT_LEN)-1)<<PEC_CR_SM2_OUT_COUNT_POS))
#define PEC_CR_SM2_SET_COUNT                                    PEC_CR_SM2_SET_COUNT
#define PEC_CR_SM2_SET_COUNT_POS                                (26U)
#define PEC_CR_SM2_SET_COUNT_LEN                                (3U)
#define PEC_CR_SM2_SET_COUNT_MSK                                (((1U<<PEC_CR_SM2_SET_COUNT_LEN)-1)<<PEC_CR_SM2_SET_COUNT_POS)
#define PEC_CR_SM2_SET_COUNT_UMSK                               (~(((1U<<PEC_CR_SM2_SET_COUNT_LEN)-1)<<PEC_CR_SM2_SET_COUNT_POS))
#define PEC_CR_SM2_SIDESET_COUNT                                PEC_CR_SM2_SIDESET_COUNT
#define PEC_CR_SM2_SIDESET_COUNT_POS                            (29U)
#define PEC_CR_SM2_SIDESET_COUNT_LEN                            (3U)
#define PEC_CR_SM2_SIDESET_COUNT_MSK                            (((1U<<PEC_CR_SM2_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM2_SIDESET_COUNT_POS)
#define PEC_CR_SM2_SIDESET_COUNT_UMSK                           (~(((1U<<PEC_CR_SM2_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM2_SIDESET_COUNT_POS))

/* 0x110 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
#define PEC_SM3_CLKDIV_OFFSET                                   (0x110)
#define PEC_CR_SM3_FRAC                                         PEC_CR_SM3_FRAC
#define PEC_CR_SM3_FRAC_POS                                     (8U)
#define PEC_CR_SM3_FRAC_LEN                                     (8U)
#define PEC_CR_SM3_FRAC_MSK                                     (((1U<<PEC_CR_SM3_FRAC_LEN)-1)<<PEC_CR_SM3_FRAC_POS)
#define PEC_CR_SM3_FRAC_UMSK                                    (~(((1U<<PEC_CR_SM3_FRAC_LEN)-1)<<PEC_CR_SM3_FRAC_POS))
#define PEC_CR_SM3_INT                                          PEC_CR_SM3_INT
#define PEC_CR_SM3_INT_POS                                      (16U)
#define PEC_CR_SM3_INT_LEN                                      (16U)
#define PEC_CR_SM3_INT_MSK                                      (((1U<<PEC_CR_SM3_INT_LEN)-1)<<PEC_CR_SM3_INT_POS)
#define PEC_CR_SM3_INT_UMSK                                     (~(((1U<<PEC_CR_SM3_INT_LEN)-1)<<PEC_CR_SM3_INT_POS))

/* 0x114 : Execution/behavioural settings for state machine N */
#define PEC_SM3_EXECCTRL_OFFSET                                 (0x114)
#define PEC_CR_SM3_STATUS_N                                     PEC_CR_SM3_STATUS_N
#define PEC_CR_SM3_STATUS_N_POS                                 (0U)
#define PEC_CR_SM3_STATUS_N_LEN                                 (4U)
#define PEC_CR_SM3_STATUS_N_MSK                                 (((1U<<PEC_CR_SM3_STATUS_N_LEN)-1)<<PEC_CR_SM3_STATUS_N_POS)
#define PEC_CR_SM3_STATUS_N_UMSK                                (~(((1U<<PEC_CR_SM3_STATUS_N_LEN)-1)<<PEC_CR_SM3_STATUS_N_POS))
#define PEC_CR_SM3_STATUS_SEL                                   PEC_CR_SM3_STATUS_SEL
#define PEC_CR_SM3_STATUS_SEL_POS                               (4U)
#define PEC_CR_SM3_STATUS_SEL_LEN                               (1U)
#define PEC_CR_SM3_STATUS_SEL_MSK                               (((1U<<PEC_CR_SM3_STATUS_SEL_LEN)-1)<<PEC_CR_SM3_STATUS_SEL_POS)
#define PEC_CR_SM3_STATUS_SEL_UMSK                              (~(((1U<<PEC_CR_SM3_STATUS_SEL_LEN)-1)<<PEC_CR_SM3_STATUS_SEL_POS))
#define PEC_CR_SM3_WRAP_BOTTOM                                  PEC_CR_SM3_WRAP_BOTTOM
#define PEC_CR_SM3_WRAP_BOTTOM_POS                              (7U)
#define PEC_CR_SM3_WRAP_BOTTOM_LEN                              (5U)
#define PEC_CR_SM3_WRAP_BOTTOM_MSK                              (((1U<<PEC_CR_SM3_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM3_WRAP_BOTTOM_POS)
#define PEC_CR_SM3_WRAP_BOTTOM_UMSK                             (~(((1U<<PEC_CR_SM3_WRAP_BOTTOM_LEN)-1)<<PEC_CR_SM3_WRAP_BOTTOM_POS))
#define PEC_CR_SM3_WRAP_TOP                                     PEC_CR_SM3_WRAP_TOP
#define PEC_CR_SM3_WRAP_TOP_POS                                 (12U)
#define PEC_CR_SM3_WRAP_TOP_LEN                                 (5U)
#define PEC_CR_SM3_WRAP_TOP_MSK                                 (((1U<<PEC_CR_SM3_WRAP_TOP_LEN)-1)<<PEC_CR_SM3_WRAP_TOP_POS)
#define PEC_CR_SM3_WRAP_TOP_UMSK                                (~(((1U<<PEC_CR_SM3_WRAP_TOP_LEN)-1)<<PEC_CR_SM3_WRAP_TOP_POS))
#define PEC_CR_SM3_JMP_PIN                                      PEC_CR_SM3_JMP_PIN
#define PEC_CR_SM3_JMP_PIN_POS                                  (24U)
#define PEC_CR_SM3_JMP_PIN_LEN                                  (5U)
#define PEC_CR_SM3_JMP_PIN_MSK                                  (((1U<<PEC_CR_SM3_JMP_PIN_LEN)-1)<<PEC_CR_SM3_JMP_PIN_POS)
#define PEC_CR_SM3_JMP_PIN_UMSK                                 (~(((1U<<PEC_CR_SM3_JMP_PIN_LEN)-1)<<PEC_CR_SM3_JMP_PIN_POS))
#define PEC_CR_SM3_SIDE_PINDIR                                  PEC_CR_SM3_SIDE_PINDIR
#define PEC_CR_SM3_SIDE_PINDIR_POS                              (29U)
#define PEC_CR_SM3_SIDE_PINDIR_LEN                              (1U)
#define PEC_CR_SM3_SIDE_PINDIR_MSK                              (((1U<<PEC_CR_SM3_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM3_SIDE_PINDIR_POS)
#define PEC_CR_SM3_SIDE_PINDIR_UMSK                             (~(((1U<<PEC_CR_SM3_SIDE_PINDIR_LEN)-1)<<PEC_CR_SM3_SIDE_PINDIR_POS))
#define PEC_CR_SM3_SIDE_EN                                      PEC_CR_SM3_SIDE_EN
#define PEC_CR_SM3_SIDE_EN_POS                                  (30U)
#define PEC_CR_SM3_SIDE_EN_LEN                                  (1U)
#define PEC_CR_SM3_SIDE_EN_MSK                                  (((1U<<PEC_CR_SM3_SIDE_EN_LEN)-1)<<PEC_CR_SM3_SIDE_EN_POS)
#define PEC_CR_SM3_SIDE_EN_UMSK                                 (~(((1U<<PEC_CR_SM3_SIDE_EN_LEN)-1)<<PEC_CR_SM3_SIDE_EN_POS))
#define PEC_ST_SM3_EXEC_STALLED                                 PEC_ST_SM3_EXEC_STALLED
#define PEC_ST_SM3_EXEC_STALLED_POS                             (31U)
#define PEC_ST_SM3_EXEC_STALLED_LEN                             (1U)
#define PEC_ST_SM3_EXEC_STALLED_MSK                             (((1U<<PEC_ST_SM3_EXEC_STALLED_LEN)-1)<<PEC_ST_SM3_EXEC_STALLED_POS)
#define PEC_ST_SM3_EXEC_STALLED_UMSK                            (~(((1U<<PEC_ST_SM3_EXEC_STALLED_LEN)-1)<<PEC_ST_SM3_EXEC_STALLED_POS))

/* 0x118 : Control behaviour of the input/output shift registers for state machine N */
#define PEC_SM3_SHIFTCTRL_OFFSET                                (0x118)
#define PEC_CR_SM3_TXF_THR                                      PEC_CR_SM3_TXF_THR
#define PEC_CR_SM3_TXF_THR_POS                                  (0U)
#define PEC_CR_SM3_TXF_THR_LEN                                  (3U)
#define PEC_CR_SM3_TXF_THR_MSK                                  (((1U<<PEC_CR_SM3_TXF_THR_LEN)-1)<<PEC_CR_SM3_TXF_THR_POS)
#define PEC_CR_SM3_TXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM3_TXF_THR_LEN)-1)<<PEC_CR_SM3_TXF_THR_POS))
#define PEC_ST_SM3_TXF_RDY                                      PEC_ST_SM3_TXF_RDY
#define PEC_ST_SM3_TXF_RDY_POS                                  (7U)
#define PEC_ST_SM3_TXF_RDY_LEN                                  (1U)
#define PEC_ST_SM3_TXF_RDY_MSK                                  (((1U<<PEC_ST_SM3_TXF_RDY_LEN)-1)<<PEC_ST_SM3_TXF_RDY_POS)
#define PEC_ST_SM3_TXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM3_TXF_RDY_LEN)-1)<<PEC_ST_SM3_TXF_RDY_POS))
#define PEC_CR_SM3_RXF_THR                                      PEC_CR_SM3_RXF_THR
#define PEC_CR_SM3_RXF_THR_POS                                  (8U)
#define PEC_CR_SM3_RXF_THR_LEN                                  (3U)
#define PEC_CR_SM3_RXF_THR_MSK                                  (((1U<<PEC_CR_SM3_RXF_THR_LEN)-1)<<PEC_CR_SM3_RXF_THR_POS)
#define PEC_CR_SM3_RXF_THR_UMSK                                 (~(((1U<<PEC_CR_SM3_RXF_THR_LEN)-1)<<PEC_CR_SM3_RXF_THR_POS))
#define PEC_ST_SM3_RXF_RDY                                      PEC_ST_SM3_RXF_RDY
#define PEC_ST_SM3_RXF_RDY_POS                                  (15U)
#define PEC_ST_SM3_RXF_RDY_LEN                                  (1U)
#define PEC_ST_SM3_RXF_RDY_MSK                                  (((1U<<PEC_ST_SM3_RXF_RDY_LEN)-1)<<PEC_ST_SM3_RXF_RDY_POS)
#define PEC_ST_SM3_RXF_RDY_UMSK                                 (~(((1U<<PEC_ST_SM3_RXF_RDY_LEN)-1)<<PEC_ST_SM3_RXF_RDY_POS))
#define PEC_CR_SM3_AUTOPUSH                                     PEC_CR_SM3_AUTOPUSH
#define PEC_CR_SM3_AUTOPUSH_POS                                 (16U)
#define PEC_CR_SM3_AUTOPUSH_LEN                                 (1U)
#define PEC_CR_SM3_AUTOPUSH_MSK                                 (((1U<<PEC_CR_SM3_AUTOPUSH_LEN)-1)<<PEC_CR_SM3_AUTOPUSH_POS)
#define PEC_CR_SM3_AUTOPUSH_UMSK                                (~(((1U<<PEC_CR_SM3_AUTOPUSH_LEN)-1)<<PEC_CR_SM3_AUTOPUSH_POS))
#define PEC_CR_SM3_AUTOPULL                                     PEC_CR_SM3_AUTOPULL
#define PEC_CR_SM3_AUTOPULL_POS                                 (17U)
#define PEC_CR_SM3_AUTOPULL_LEN                                 (1U)
#define PEC_CR_SM3_AUTOPULL_MSK                                 (((1U<<PEC_CR_SM3_AUTOPULL_LEN)-1)<<PEC_CR_SM3_AUTOPULL_POS)
#define PEC_CR_SM3_AUTOPULL_UMSK                                (~(((1U<<PEC_CR_SM3_AUTOPULL_LEN)-1)<<PEC_CR_SM3_AUTOPULL_POS))
#define PEC_CR_SM3_IN_SHIFTDIR                                  PEC_CR_SM3_IN_SHIFTDIR
#define PEC_CR_SM3_IN_SHIFTDIR_POS                              (18U)
#define PEC_CR_SM3_IN_SHIFTDIR_LEN                              (1U)
#define PEC_CR_SM3_IN_SHIFTDIR_MSK                              (((1U<<PEC_CR_SM3_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM3_IN_SHIFTDIR_POS)
#define PEC_CR_SM3_IN_SHIFTDIR_UMSK                             (~(((1U<<PEC_CR_SM3_IN_SHIFTDIR_LEN)-1)<<PEC_CR_SM3_IN_SHIFTDIR_POS))
#define PEC_CR_SM3_OUT_SHIFTDIR                                 PEC_CR_SM3_OUT_SHIFTDIR
#define PEC_CR_SM3_OUT_SHIFTDIR_POS                             (19U)
#define PEC_CR_SM3_OUT_SHIFTDIR_LEN                             (1U)
#define PEC_CR_SM3_OUT_SHIFTDIR_MSK                             (((1U<<PEC_CR_SM3_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM3_OUT_SHIFTDIR_POS)
#define PEC_CR_SM3_OUT_SHIFTDIR_UMSK                            (~(((1U<<PEC_CR_SM3_OUT_SHIFTDIR_LEN)-1)<<PEC_CR_SM3_OUT_SHIFTDIR_POS))
#define PEC_CR_SM3_PUSH_THRESH                                  PEC_CR_SM3_PUSH_THRESH
#define PEC_CR_SM3_PUSH_THRESH_POS                              (20U)
#define PEC_CR_SM3_PUSH_THRESH_LEN                              (5U)
#define PEC_CR_SM3_PUSH_THRESH_MSK                              (((1U<<PEC_CR_SM3_PUSH_THRESH_LEN)-1)<<PEC_CR_SM3_PUSH_THRESH_POS)
#define PEC_CR_SM3_PUSH_THRESH_UMSK                             (~(((1U<<PEC_CR_SM3_PUSH_THRESH_LEN)-1)<<PEC_CR_SM3_PUSH_THRESH_POS))
#define PEC_CR_SM3_PULL_THRESH                                  PEC_CR_SM3_PULL_THRESH
#define PEC_CR_SM3_PULL_THRESH_POS                              (25U)
#define PEC_CR_SM3_PULL_THRESH_LEN                              (5U)
#define PEC_CR_SM3_PULL_THRESH_MSK                              (((1U<<PEC_CR_SM3_PULL_THRESH_LEN)-1)<<PEC_CR_SM3_PULL_THRESH_POS)
#define PEC_CR_SM3_PULL_THRESH_UMSK                             (~(((1U<<PEC_CR_SM3_PULL_THRESH_LEN)-1)<<PEC_CR_SM3_PULL_THRESH_POS))
#define PEC_CR_SM3_TXF_CLR                                      PEC_CR_SM3_TXF_CLR
#define PEC_CR_SM3_TXF_CLR_POS                                  (30U)
#define PEC_CR_SM3_TXF_CLR_LEN                                  (1U)
#define PEC_CR_SM3_TXF_CLR_MSK                                  (((1U<<PEC_CR_SM3_TXF_CLR_LEN)-1)<<PEC_CR_SM3_TXF_CLR_POS)
#define PEC_CR_SM3_TXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM3_TXF_CLR_LEN)-1)<<PEC_CR_SM3_TXF_CLR_POS))
#define PEC_CR_SM3_RXF_CLR                                      PEC_CR_SM3_RXF_CLR
#define PEC_CR_SM3_RXF_CLR_POS                                  (31U)
#define PEC_CR_SM3_RXF_CLR_LEN                                  (1U)
#define PEC_CR_SM3_RXF_CLR_MSK                                  (((1U<<PEC_CR_SM3_RXF_CLR_LEN)-1)<<PEC_CR_SM3_RXF_CLR_POS)
#define PEC_CR_SM3_RXF_CLR_UMSK                                 (~(((1U<<PEC_CR_SM3_RXF_CLR_LEN)-1)<<PEC_CR_SM3_RXF_CLR_POS))

/* 0x11C : SM3_ADDR */
#define PEC_SM3_ADDR_OFFSET                                     (0x11C)
#define PEC_ST_SM3_ADDR                                         PEC_ST_SM3_ADDR
#define PEC_ST_SM3_ADDR_POS                                     (0U)
#define PEC_ST_SM3_ADDR_LEN                                     (5U)
#define PEC_ST_SM3_ADDR_MSK                                     (((1U<<PEC_ST_SM3_ADDR_LEN)-1)<<PEC_ST_SM3_ADDR_POS)
#define PEC_ST_SM3_ADDR_UMSK                                    (~(((1U<<PEC_ST_SM3_ADDR_LEN)-1)<<PEC_ST_SM3_ADDR_POS))

/* 0x120 : SM3_INSTR */
#define PEC_SM3_INSTR_OFFSET                                    (0x120)
#define PEC_CR_SM3_INSTR                                        PEC_CR_SM3_INSTR
#define PEC_CR_SM3_INSTR_POS                                    (0U)
#define PEC_CR_SM3_INSTR_LEN                                    (16U)
#define PEC_CR_SM3_INSTR_MSK                                    (((1U<<PEC_CR_SM3_INSTR_LEN)-1)<<PEC_CR_SM3_INSTR_POS)
#define PEC_CR_SM3_INSTR_UMSK                                   (~(((1U<<PEC_CR_SM3_INSTR_LEN)-1)<<PEC_CR_SM3_INSTR_POS))
#define PEC_ST_SM3_INSTR                                        PEC_ST_SM3_INSTR
#define PEC_ST_SM3_INSTR_POS                                    (16U)
#define PEC_ST_SM3_INSTR_LEN                                    (16U)
#define PEC_ST_SM3_INSTR_MSK                                    (((1U<<PEC_ST_SM3_INSTR_LEN)-1)<<PEC_ST_SM3_INSTR_POS)
#define PEC_ST_SM3_INSTR_UMSK                                   (~(((1U<<PEC_ST_SM3_INSTR_LEN)-1)<<PEC_ST_SM3_INSTR_POS))

/* 0x124 : State machine pin control */
#define PEC_SM3_PINCTRL_OFFSET                                  (0x124)
#define PEC_CR_SM3_OUT_BASE                                     PEC_CR_SM3_OUT_BASE
#define PEC_CR_SM3_OUT_BASE_POS                                 (0U)
#define PEC_CR_SM3_OUT_BASE_LEN                                 (5U)
#define PEC_CR_SM3_OUT_BASE_MSK                                 (((1U<<PEC_CR_SM3_OUT_BASE_LEN)-1)<<PEC_CR_SM3_OUT_BASE_POS)
#define PEC_CR_SM3_OUT_BASE_UMSK                                (~(((1U<<PEC_CR_SM3_OUT_BASE_LEN)-1)<<PEC_CR_SM3_OUT_BASE_POS))
#define PEC_CR_SM3_SET_BASE                                     PEC_CR_SM3_SET_BASE
#define PEC_CR_SM3_SET_BASE_POS                                 (5U)
#define PEC_CR_SM3_SET_BASE_LEN                                 (5U)
#define PEC_CR_SM3_SET_BASE_MSK                                 (((1U<<PEC_CR_SM3_SET_BASE_LEN)-1)<<PEC_CR_SM3_SET_BASE_POS)
#define PEC_CR_SM3_SET_BASE_UMSK                                (~(((1U<<PEC_CR_SM3_SET_BASE_LEN)-1)<<PEC_CR_SM3_SET_BASE_POS))
#define PEC_CR_SM3_SIDESET_BASE                                 PEC_CR_SM3_SIDESET_BASE
#define PEC_CR_SM3_SIDESET_BASE_POS                             (10U)
#define PEC_CR_SM3_SIDESET_BASE_LEN                             (5U)
#define PEC_CR_SM3_SIDESET_BASE_MSK                             (((1U<<PEC_CR_SM3_SIDESET_BASE_LEN)-1)<<PEC_CR_SM3_SIDESET_BASE_POS)
#define PEC_CR_SM3_SIDESET_BASE_UMSK                            (~(((1U<<PEC_CR_SM3_SIDESET_BASE_LEN)-1)<<PEC_CR_SM3_SIDESET_BASE_POS))
#define PEC_CR_SM3_IN_BASE                                      PEC_CR_SM3_IN_BASE
#define PEC_CR_SM3_IN_BASE_POS                                  (15U)
#define PEC_CR_SM3_IN_BASE_LEN                                  (5U)
#define PEC_CR_SM3_IN_BASE_MSK                                  (((1U<<PEC_CR_SM3_IN_BASE_LEN)-1)<<PEC_CR_SM3_IN_BASE_POS)
#define PEC_CR_SM3_IN_BASE_UMSK                                 (~(((1U<<PEC_CR_SM3_IN_BASE_LEN)-1)<<PEC_CR_SM3_IN_BASE_POS))
#define PEC_CR_SM3_OUT_COUNT                                    PEC_CR_SM3_OUT_COUNT
#define PEC_CR_SM3_OUT_COUNT_POS                                (20U)
#define PEC_CR_SM3_OUT_COUNT_LEN                                (6U)
#define PEC_CR_SM3_OUT_COUNT_MSK                                (((1U<<PEC_CR_SM3_OUT_COUNT_LEN)-1)<<PEC_CR_SM3_OUT_COUNT_POS)
#define PEC_CR_SM3_OUT_COUNT_UMSK                               (~(((1U<<PEC_CR_SM3_OUT_COUNT_LEN)-1)<<PEC_CR_SM3_OUT_COUNT_POS))
#define PEC_CR_SM3_SET_COUNT                                    PEC_CR_SM3_SET_COUNT
#define PEC_CR_SM3_SET_COUNT_POS                                (26U)
#define PEC_CR_SM3_SET_COUNT_LEN                                (3U)
#define PEC_CR_SM3_SET_COUNT_MSK                                (((1U<<PEC_CR_SM3_SET_COUNT_LEN)-1)<<PEC_CR_SM3_SET_COUNT_POS)
#define PEC_CR_SM3_SET_COUNT_UMSK                               (~(((1U<<PEC_CR_SM3_SET_COUNT_LEN)-1)<<PEC_CR_SM3_SET_COUNT_POS))
#define PEC_CR_SM3_SIDESET_COUNT                                PEC_CR_SM3_SIDESET_COUNT
#define PEC_CR_SM3_SIDESET_COUNT_POS                            (29U)
#define PEC_CR_SM3_SIDESET_COUNT_LEN                            (3U)
#define PEC_CR_SM3_SIDESET_COUNT_MSK                            (((1U<<PEC_CR_SM3_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM3_SIDESET_COUNT_POS)
#define PEC_CR_SM3_SIDESET_COUNT_UMSK                           (~(((1U<<PEC_CR_SM3_SIDESET_COUNT_LEN)-1)<<PEC_CR_SM3_SIDESET_COUNT_POS))

/* 0x128 : Raw Interrupts */
#define PEC_INTR_OFFSET                                         (0x128)
#define PEC_ST_INTR                                             PEC_ST_INTR
#define PEC_ST_INTR_POS                                         (0U)
#define PEC_ST_INTR_LEN                                         (16U)
#define PEC_ST_INTR_MSK                                         (((1U<<PEC_ST_INTR_LEN)-1)<<PEC_ST_INTR_POS)
#define PEC_ST_INTR_UMSK                                        (~(((1U<<PEC_ST_INTR_LEN)-1)<<PEC_ST_INTR_POS))

/* 0x12C : Interrupt Enable for irq0 */
#define PEC_IRQ0_INTE_OFFSET                                    (0x12C)
#define PEC_CR_IRQ0_INTE                                        PEC_CR_IRQ0_INTE
#define PEC_CR_IRQ0_INTE_POS                                    (0U)
#define PEC_CR_IRQ0_INTE_LEN                                    (12U)
#define PEC_CR_IRQ0_INTE_MSK                                    (((1U<<PEC_CR_IRQ0_INTE_LEN)-1)<<PEC_CR_IRQ0_INTE_POS)
#define PEC_CR_IRQ0_INTE_UMSK                                   (~(((1U<<PEC_CR_IRQ0_INTE_LEN)-1)<<PEC_CR_IRQ0_INTE_POS))

/* 0x130 : Interrupt Force for irq0 */
#define PEC_IRQ0_INTF_OFFSET                                    (0x130)
#define PEC_CR_IRQ0_INTF                                        PEC_CR_IRQ0_INTF
#define PEC_CR_IRQ0_INTF_POS                                    (0U)
#define PEC_CR_IRQ0_INTF_LEN                                    (12U)
#define PEC_CR_IRQ0_INTF_MSK                                    (((1U<<PEC_CR_IRQ0_INTF_LEN)-1)<<PEC_CR_IRQ0_INTF_POS)
#define PEC_CR_IRQ0_INTF_UMSK                                   (~(((1U<<PEC_CR_IRQ0_INTF_LEN)-1)<<PEC_CR_IRQ0_INTF_POS))

/* 0x134 : Interrupt status after masking & forcing for irq0 */
#define PEC_IRQ0_INTS_OFFSET                                    (0x134)
#define PEC_ST_IRQ0_INTS                                        PEC_ST_IRQ0_INTS
#define PEC_ST_IRQ0_INTS_POS                                    (0U)
#define PEC_ST_IRQ0_INTS_LEN                                    (12U)
#define PEC_ST_IRQ0_INTS_MSK                                    (((1U<<PEC_ST_IRQ0_INTS_LEN)-1)<<PEC_ST_IRQ0_INTS_POS)
#define PEC_ST_IRQ0_INTS_UMSK                                   (~(((1U<<PEC_ST_IRQ0_INTS_LEN)-1)<<PEC_ST_IRQ0_INTS_POS))

/* 0x138 : Interrupt Enable for irq1 */
#define PEC_IRQ1_INTE_OFFSET                                    (0x138)
#define PEC_CR_IRQ1_INTE                                        PEC_CR_IRQ1_INTE
#define PEC_CR_IRQ1_INTE_POS                                    (0U)
#define PEC_CR_IRQ1_INTE_LEN                                    (12U)
#define PEC_CR_IRQ1_INTE_MSK                                    (((1U<<PEC_CR_IRQ1_INTE_LEN)-1)<<PEC_CR_IRQ1_INTE_POS)
#define PEC_CR_IRQ1_INTE_UMSK                                   (~(((1U<<PEC_CR_IRQ1_INTE_LEN)-1)<<PEC_CR_IRQ1_INTE_POS))

/* 0x13C : Interrupt Force for irq1 */
#define PEC_IRQ1_INTF_OFFSET                                    (0x13C)
#define PEC_CR_IRQ1_INTF                                        PEC_CR_IRQ1_INTF
#define PEC_CR_IRQ1_INTF_POS                                    (0U)
#define PEC_CR_IRQ1_INTF_LEN                                    (12U)
#define PEC_CR_IRQ1_INTF_MSK                                    (((1U<<PEC_CR_IRQ1_INTF_LEN)-1)<<PEC_CR_IRQ1_INTF_POS)
#define PEC_CR_IRQ1_INTF_UMSK                                   (~(((1U<<PEC_CR_IRQ1_INTF_LEN)-1)<<PEC_CR_IRQ1_INTF_POS))

/* 0x140 : Interrupt status after masking & forcing for irq1 */
#define PEC_IRQ1_INTS_OFFSET                                    (0x140)
#define PEC_ST_IRQ1_INTS                                        PEC_ST_IRQ1_INTS
#define PEC_ST_IRQ1_INTS_POS                                    (0U)
#define PEC_ST_IRQ1_INTS_LEN                                    (12U)
#define PEC_ST_IRQ1_INTS_MSK                                    (((1U<<PEC_ST_IRQ1_INTS_LEN)-1)<<PEC_ST_IRQ1_INTS_POS)
#define PEC_ST_IRQ1_INTS_UMSK                                   (~(((1U<<PEC_ST_IRQ1_INTS_LEN)-1)<<PEC_ST_IRQ1_INTS_POS))

/* 0x144 : PAD_O_INV */
#define PEC_PAD_O_INV_OFFSET                                    (0x144)
#define PEC_CR_PAD_O_INV                                        PEC_CR_PAD_O_INV
#define PEC_CR_PAD_O_INV_POS                                    (0U)
#define PEC_CR_PAD_O_INV_LEN                                    (32U)
#define PEC_CR_PAD_O_INV_MSK                                    (((1U<<PEC_CR_PAD_O_INV_LEN)-1)<<PEC_CR_PAD_O_INV_POS)
#define PEC_CR_PAD_O_INV_UMSK                                   (~(((1U<<PEC_CR_PAD_O_INV_LEN)-1)<<PEC_CR_PAD_O_INV_POS))

/* 0x148 : PAD_OE_INV */
#define PEC_PAD_OE_INV_OFFSET                                   (0x148)
#define PEC_CR_PAD_OE_INV                                       PEC_CR_PAD_OE_INV
#define PEC_CR_PAD_OE_INV_POS                                   (0U)
#define PEC_CR_PAD_OE_INV_LEN                                   (32U)
#define PEC_CR_PAD_OE_INV_MSK                                   (((1U<<PEC_CR_PAD_OE_INV_LEN)-1)<<PEC_CR_PAD_OE_INV_POS)
#define PEC_CR_PAD_OE_INV_UMSK                                  (~(((1U<<PEC_CR_PAD_OE_INV_LEN)-1)<<PEC_CR_PAD_OE_INV_POS))

/* 0x14C : PAD_I_INV */
#define PEC_PAD_I_INV_OFFSET                                    (0x14C)
#define PEC_CR_PAD_I_INV                                        PEC_CR_PAD_I_INV
#define PEC_CR_PAD_I_INV_POS                                    (0U)
#define PEC_CR_PAD_I_INV_LEN                                    (32U)
#define PEC_CR_PAD_I_INV_MSK                                    (((1U<<PEC_CR_PAD_I_INV_LEN)-1)<<PEC_CR_PAD_I_INV_POS)
#define PEC_CR_PAD_I_INV_UMSK                                   (~(((1U<<PEC_CR_PAD_I_INV_LEN)-1)<<PEC_CR_PAD_I_INV_POS))

/* 0x150 : PAD_O_SET */
#define PEC_PAD_O_SET_OFFSET                                    (0x150)
#define PEC_CR_PAD_O_SET                                        PEC_CR_PAD_O_SET
#define PEC_CR_PAD_O_SET_POS                                    (0U)
#define PEC_CR_PAD_O_SET_LEN                                    (32U)
#define PEC_CR_PAD_O_SET_MSK                                    (((1U<<PEC_CR_PAD_O_SET_LEN)-1)<<PEC_CR_PAD_O_SET_POS)
#define PEC_CR_PAD_O_SET_UMSK                                   (~(((1U<<PEC_CR_PAD_O_SET_LEN)-1)<<PEC_CR_PAD_O_SET_POS))

/* 0x154 : PAD_OE_SET */
#define PEC_PAD_OE_SET_OFFSET                                   (0x154)
#define PEC_CR_PAD_OE_SET                                       PEC_CR_PAD_OE_SET
#define PEC_CR_PAD_OE_SET_POS                                   (0U)
#define PEC_CR_PAD_OE_SET_LEN                                   (32U)
#define PEC_CR_PAD_OE_SET_MSK                                   (((1U<<PEC_CR_PAD_OE_SET_LEN)-1)<<PEC_CR_PAD_OE_SET_POS)
#define PEC_CR_PAD_OE_SET_UMSK                                  (~(((1U<<PEC_CR_PAD_OE_SET_LEN)-1)<<PEC_CR_PAD_OE_SET_POS))

/* 0x158 : DMA_CONFIG */
#define PEC_DMA_CONFIG_OFFSET                                   (0x158)
#define PEC_CR_SM0_DMA_TX_EN                                    PEC_CR_SM0_DMA_TX_EN
#define PEC_CR_SM0_DMA_TX_EN_POS                                (0U)
#define PEC_CR_SM0_DMA_TX_EN_LEN                                (1U)
#define PEC_CR_SM0_DMA_TX_EN_MSK                                (((1U<<PEC_CR_SM0_DMA_TX_EN_LEN)-1)<<PEC_CR_SM0_DMA_TX_EN_POS)
#define PEC_CR_SM0_DMA_TX_EN_UMSK                               (~(((1U<<PEC_CR_SM0_DMA_TX_EN_LEN)-1)<<PEC_CR_SM0_DMA_TX_EN_POS))
#define PEC_CR_SM0_DMA_RX_EN                                    PEC_CR_SM0_DMA_RX_EN
#define PEC_CR_SM0_DMA_RX_EN_POS                                (1U)
#define PEC_CR_SM0_DMA_RX_EN_LEN                                (1U)
#define PEC_CR_SM0_DMA_RX_EN_MSK                                (((1U<<PEC_CR_SM0_DMA_RX_EN_LEN)-1)<<PEC_CR_SM0_DMA_RX_EN_POS)
#define PEC_CR_SM0_DMA_RX_EN_UMSK                               (~(((1U<<PEC_CR_SM0_DMA_RX_EN_LEN)-1)<<PEC_CR_SM0_DMA_RX_EN_POS))
#define PEC_CR_SM1_DMA_TX_EN                                    PEC_CR_SM1_DMA_TX_EN
#define PEC_CR_SM1_DMA_TX_EN_POS                                (2U)
#define PEC_CR_SM1_DMA_TX_EN_LEN                                (1U)
#define PEC_CR_SM1_DMA_TX_EN_MSK                                (((1U<<PEC_CR_SM1_DMA_TX_EN_LEN)-1)<<PEC_CR_SM1_DMA_TX_EN_POS)
#define PEC_CR_SM1_DMA_TX_EN_UMSK                               (~(((1U<<PEC_CR_SM1_DMA_TX_EN_LEN)-1)<<PEC_CR_SM1_DMA_TX_EN_POS))
#define PEC_CR_SM1_DMA_RX_EN                                    PEC_CR_SM1_DMA_RX_EN
#define PEC_CR_SM1_DMA_RX_EN_POS                                (3U)
#define PEC_CR_SM1_DMA_RX_EN_LEN                                (1U)
#define PEC_CR_SM1_DMA_RX_EN_MSK                                (((1U<<PEC_CR_SM1_DMA_RX_EN_LEN)-1)<<PEC_CR_SM1_DMA_RX_EN_POS)
#define PEC_CR_SM1_DMA_RX_EN_UMSK                               (~(((1U<<PEC_CR_SM1_DMA_RX_EN_LEN)-1)<<PEC_CR_SM1_DMA_RX_EN_POS))
#define PEC_CR_SM2_DMA_TX_EN                                    PEC_CR_SM2_DMA_TX_EN
#define PEC_CR_SM2_DMA_TX_EN_POS                                (4U)
#define PEC_CR_SM2_DMA_TX_EN_LEN                                (1U)
#define PEC_CR_SM2_DMA_TX_EN_MSK                                (((1U<<PEC_CR_SM2_DMA_TX_EN_LEN)-1)<<PEC_CR_SM2_DMA_TX_EN_POS)
#define PEC_CR_SM2_DMA_TX_EN_UMSK                               (~(((1U<<PEC_CR_SM2_DMA_TX_EN_LEN)-1)<<PEC_CR_SM2_DMA_TX_EN_POS))
#define PEC_CR_SM2_DMA_RX_EN                                    PEC_CR_SM2_DMA_RX_EN
#define PEC_CR_SM2_DMA_RX_EN_POS                                (5U)
#define PEC_CR_SM2_DMA_RX_EN_LEN                                (1U)
#define PEC_CR_SM2_DMA_RX_EN_MSK                                (((1U<<PEC_CR_SM2_DMA_RX_EN_LEN)-1)<<PEC_CR_SM2_DMA_RX_EN_POS)
#define PEC_CR_SM2_DMA_RX_EN_UMSK                               (~(((1U<<PEC_CR_SM2_DMA_RX_EN_LEN)-1)<<PEC_CR_SM2_DMA_RX_EN_POS))
#define PEC_CR_SM3_DMA_TX_EN                                    PEC_CR_SM3_DMA_TX_EN
#define PEC_CR_SM3_DMA_TX_EN_POS                                (6U)
#define PEC_CR_SM3_DMA_TX_EN_LEN                                (1U)
#define PEC_CR_SM3_DMA_TX_EN_MSK                                (((1U<<PEC_CR_SM3_DMA_TX_EN_LEN)-1)<<PEC_CR_SM3_DMA_TX_EN_POS)
#define PEC_CR_SM3_DMA_TX_EN_UMSK                               (~(((1U<<PEC_CR_SM3_DMA_TX_EN_LEN)-1)<<PEC_CR_SM3_DMA_TX_EN_POS))
#define PEC_CR_SM3_DMA_RX_EN                                    PEC_CR_SM3_DMA_RX_EN
#define PEC_CR_SM3_DMA_RX_EN_POS                                (7U)
#define PEC_CR_SM3_DMA_RX_EN_LEN                                (1U)
#define PEC_CR_SM3_DMA_RX_EN_MSK                                (((1U<<PEC_CR_SM3_DMA_RX_EN_LEN)-1)<<PEC_CR_SM3_DMA_RX_EN_POS)
#define PEC_CR_SM3_DMA_RX_EN_UMSK                               (~(((1U<<PEC_CR_SM3_DMA_RX_EN_LEN)-1)<<PEC_CR_SM3_DMA_RX_EN_POS))

struct  pec_reg {
    /* 0x0 : PEC control register */
    union {
        struct {
            uint32_t cr_sm_en                       :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_sm_reset                    :  4; /* [ 7: 4],        w1c,        0x0 */
            uint32_t cr_clkdiv_reset                :  4; /* [11: 8],        w1c,        0x0 */
            uint32_t cr_sm0_pc_origin               :  5; /* [16:12],        r/w,        0x0 */
            uint32_t cr_sm1_pc_origin               :  5; /* [21:17],        r/w,        0x0 */
            uint32_t cr_sm2_pc_origin               :  5; /* [26:22],        r/w,        0x0 */
            uint32_t cr_sm3_pc_origin               :  5; /* [31:27],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } CTRL;

    /* 0x4 : FIFO status register */
    union {
        struct {
            uint32_t st_rxfull                      :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t st_rxempty                     :  4; /* [11: 8],          r,        0xf */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t st_txfull                      :  4; /* [19:16],          r,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t st_txempty                     :  4; /* [27:24],          r,        0xf */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } FSTAT;

    /* 0x8 : FIFO debug register */
    union {
        struct {
            uint32_t cr_rxstall_clr                 :  4; /* [ 3: 0],        w1c,        0x0 */
            uint32_t st_rxstall                     :  4; /* [ 7: 4],          r,        0x0 */
            uint32_t cr_rxunder_clr                 :  4; /* [11: 8],        w1c,        0x0 */
            uint32_t st_rxunder                     :  4; /* [15:12],          r,        0x0 */
            uint32_t cr_txover_clr                  :  4; /* [19:16],        w1c,        0x0 */
            uint32_t st_txover                      :  4; /* [23:20],          r,        0x0 */
            uint32_t cr_txstall_clr                 :  4; /* [27:24],        w1c,        0x0 */
            uint32_t st_txstall                     :  4; /* [31:28],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } FDEBUG;

    /* 0xC : FIFO levels */
    union {
        struct {
            uint32_t st_sm0_txlevel                 :  4; /* [ 3: 0],          r,        0x0 */
            uint32_t st_sm0_rxlevel                 :  4; /* [ 7: 4],          r,        0x0 */
            uint32_t st_sm1_txlevel                 :  4; /* [11: 8],          r,        0x0 */
            uint32_t st_sm1_rxlevel                 :  4; /* [15:12],          r,        0x0 */
            uint32_t st_sm2_txlevel                 :  4; /* [19:16],          r,        0x0 */
            uint32_t st_sm2_rxlevel                 :  4; /* [23:20],          r,        0x0 */
            uint32_t st_sm3_txlevel                 :  4; /* [27:24],          r,        0x0 */
            uint32_t st_sm3_rxlevel                 :  4; /* [31:28],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } FLEVEL;

    /* 0x10 : TXF0 */
    union {
        struct {
            uint32_t sm0_txf_wdata                  : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } TXF0;

    /* 0x14 : TXF1 */
    union {
        struct {
            uint32_t sm1_txf_wdata                  : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } TXF1;

    /* 0x18 : TXF2 */
    union {
        struct {
            uint32_t sm2_txf_wdata                  : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } TXF2;

    /* 0x1C : TXF3 */
    union {
        struct {
            uint32_t sm3_txf_wdata                  : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } TXF3;

    /* 0x20 : RXF0 */
    union {
        struct {
            uint32_t sm0_rxf_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } RXF0;

    /* 0x24 : RXF1 */
    union {
        struct {
            uint32_t sm1_rxf_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } RXF1;

    /* 0x28 : RXF2 */
    union {
        struct {
            uint32_t sm2_rxf_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } RXF2;

    /* 0x2C : RXF3 */
    union {
        struct {
            uint32_t sm3_rxf_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } RXF3;

    /* 0x30 : IRQ */
    union {
        struct {
            uint32_t cr_irq                         :  8; /* [ 7: 0],        w1c,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ;

    /* 0x34 : IRQ_FORCE */
    union {
        struct {
            uint32_t cr_irq_force                   :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ_FORCE;

    /* 0x38 : INPUT_SYNC_BYPASS */
    union {
        struct {
            uint32_t cr_in_sync_byp                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } INPUT_SYNC_BYPASS;

    /* 0x3C : DBG_PADOUT */
    union {
        struct {
            uint32_t st_dbg_padout                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_PADOUT;

    /* 0x40 : DBG_PADOE */
    union {
        struct {
            uint32_t st_dbg_padoe                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_PADOE;

    /* 0x44 : The PEC hardware has some free parameters that may vary between chip products. */
    union {
        struct {
            uint32_t st_fifo_depth                  :  6; /* [ 5: 0],          r,        0x8 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t st_sm_count                    :  4; /* [11: 8],          r,        0x4 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t st_imem_size                   :  6; /* [21:16],          r,       0x20 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DBG_CFGINFO;

    /* 0x48 : INSTR_MEM0 */
    union {
        struct {
            uint32_t cr_instr_mem0                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM0;

    /* 0x4C : INSTR_MEM1 */
    union {
        struct {
            uint32_t cr_instr_mem1                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM1;

    /* 0x50 : INSTR_MEM2 */
    union {
        struct {
            uint32_t cr_instr_mem2                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM2;

    /* 0x54 : INSTR_MEM3 */
    union {
        struct {
            uint32_t cr_instr_mem3                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM3;

    /* 0x58 : INSTR_MEM4 */
    union {
        struct {
            uint32_t cr_instr_mem4                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM4;

    /* 0x5C : INSTR_MEM5 */
    union {
        struct {
            uint32_t cr_instr_mem5                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM5;

    /* 0x60 : INSTR_MEM6 */
    union {
        struct {
            uint32_t cr_instr_mem6                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM6;

    /* 0x64 : INSTR_MEM7 */
    union {
        struct {
            uint32_t cr_instr_mem7                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM7;

    /* 0x68 : INSTR_MEM8 */
    union {
        struct {
            uint32_t cr_instr_mem8                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM8;

    /* 0x6C : INSTR_MEM9 */
    union {
        struct {
            uint32_t cr_instr_mem9                  : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM9;

    /* 0x70 : INSTR_MEM10 */
    union {
        struct {
            uint32_t cr_instr_mem10                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM10;

    /* 0x74 : INSTR_MEM11 */
    union {
        struct {
            uint32_t cr_instr_mem11                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM11;

    /* 0x78 : INSTR_MEM12 */
    union {
        struct {
            uint32_t cr_instr_mem12                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM12;

    /* 0x7C : INSTR_MEM13 */
    union {
        struct {
            uint32_t cr_instr_mem13                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM13;

    /* 0x80 : INSTR_MEM14 */
    union {
        struct {
            uint32_t cr_instr_mem14                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM14;

    /* 0x84 : INSTR_MEM15 */
    union {
        struct {
            uint32_t cr_instr_mem15                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM15;

    /* 0x88 : INSTR_MEM16 */
    union {
        struct {
            uint32_t cr_instr_mem16                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM16;

    /* 0x8C : INSTR_MEM17 */
    union {
        struct {
            uint32_t cr_instr_mem17                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM17;

    /* 0x90 : INSTR_MEM18 */
    union {
        struct {
            uint32_t cr_instr_mem18                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM18;

    /* 0x94 : INSTR_MEM19 */
    union {
        struct {
            uint32_t cr_instr_mem19                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM19;

    /* 0x98 : INSTR_MEM20 */
    union {
        struct {
            uint32_t cr_instr_mem20                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM20;

    /* 0x9C : INSTR_MEM21 */
    union {
        struct {
            uint32_t cr_instr_mem21                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM21;

    /* 0xA0 : INSTR_MEM22 */
    union {
        struct {
            uint32_t cr_instr_mem22                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM22;

    /* 0xA4 : INSTR_MEM23 */
    union {
        struct {
            uint32_t cr_instr_mem23                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM23;

    /* 0xA8 : INSTR_MEM24 */
    union {
        struct {
            uint32_t cr_instr_mem24                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM24;

    /* 0xAC : INSTR_MEM25 */
    union {
        struct {
            uint32_t cr_instr_mem25                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM25;

    /* 0xB0 : INSTR_MEM26 */
    union {
        struct {
            uint32_t cr_instr_mem26                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM26;

    /* 0xB4 : INSTR_MEM27 */
    union {
        struct {
            uint32_t cr_instr_mem27                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM27;

    /* 0xB8 : INSTR_MEM28 */
    union {
        struct {
            uint32_t cr_instr_mem28                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM28;

    /* 0xBC : INSTR_MEM29 */
    union {
        struct {
            uint32_t cr_instr_mem29                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM29;

    /* 0xC0 : INSTR_MEM30 */
    union {
        struct {
            uint32_t cr_instr_mem30                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM30;

    /* 0xC4 : INSTR_MEM31 */
    union {
        struct {
            uint32_t cr_instr_mem31                 : 16; /* [15: 0],          w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INSTR_MEM31;

    /* 0xC8 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_sm0_frac                    :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t cr_sm0_int                     : 16; /* [31:16],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } SM0_CLKDIV;

    /* 0xCC : Execution/behavioural settings for state machine N */
    union {
        struct {
            uint32_t cr_sm0_status_n                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_sm0_status_sel              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t cr_sm0_wrap_bottom             :  5; /* [11: 7],        r/w,        0x0 */
            uint32_t cr_sm0_wrap_top                :  5; /* [16:12],        r/w,       0x1f */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t cr_sm0_jmp_pin                 :  5; /* [28:24],        r/w,        0x0 */
            uint32_t cr_sm0_side_pindir             :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_sm0_side_en                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t st_sm0_exec_stalled            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM0_EXECCTRL;

    /* 0xD0 : Control behaviour of the input/output shift registers for state machine N */
    union {
        struct {
            uint32_t cr_sm0_txf_thr                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_6                   :  4; /* [ 6: 3],       rsvd,        0x0 */
            uint32_t st_sm0_txf_rdy                 :  1; /* [    7],          r,        0x1 */
            uint32_t cr_sm0_rxf_thr                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_14                 :  4; /* [14:11],       rsvd,        0x0 */
            uint32_t st_sm0_rxf_rdy                 :  1; /* [   15],          r,        0x0 */
            uint32_t cr_sm0_autopush                :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_sm0_autopull                :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_sm0_in_shiftdir             :  1; /* [   18],        r/w,        0x1 */
            uint32_t cr_sm0_out_shiftdir            :  1; /* [   19],        r/w,        0x1 */
            uint32_t cr_sm0_push_thresh             :  5; /* [24:20],        r/w,        0x0 */
            uint32_t cr_sm0_pull_thresh             :  5; /* [29:25],        r/w,        0x0 */
            uint32_t cr_sm0_txf_clr                 :  1; /* [   30],        w1c,        0x0 */
            uint32_t cr_sm0_rxf_clr                 :  1; /* [   31],        w1c,        0x0 */
        }BF;
        uint32_t WORD;
    } SM0_SHIFTCTRL;

    /* 0xD4 : SM0_ADDR */
    union {
        struct {
            uint32_t st_sm0_addr                    :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } SM0_ADDR;

    /* 0xD8 : SM0_INSTR */
    union {
        struct {
            uint32_t cr_sm0_instr                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t st_sm0_instr                   : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM0_INSTR;

    /* 0xDC : State machine pin control */
    union {
        struct {
            uint32_t cr_sm0_out_base                :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t cr_sm0_set_base                :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t cr_sm0_sideset_base            :  5; /* [14:10],        r/w,        0x0 */
            uint32_t cr_sm0_in_base                 :  5; /* [19:15],        r/w,        0x0 */
            uint32_t cr_sm0_out_count               :  6; /* [25:20],        r/w,        0x0 */
            uint32_t cr_sm0_set_count               :  3; /* [28:26],        r/w,        0x5 */
            uint32_t cr_sm0_sideset_count           :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } SM0_PINCTRL;

    /* 0xE0 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_sm1_frac                    :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t cr_sm1_int                     : 16; /* [31:16],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } SM1_CLKDIV;

    /* 0xE4 : Execution/behavioural settings for state machine N */
    union {
        struct {
            uint32_t cr_sm1_status_n                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_sm1_status_sel              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t cr_sm1_wrap_bottom             :  5; /* [11: 7],        r/w,        0x0 */
            uint32_t cr_sm1_wrap_top                :  5; /* [16:12],        r/w,       0x1f */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t cr_sm1_jmp_pin                 :  5; /* [28:24],        r/w,        0x0 */
            uint32_t cr_sm1_side_pindir             :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_sm1_side_en                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t st_sm1_exec_stalled            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM1_EXECCTRL;

    /* 0xE8 : Control behaviour of the input/output shift registers for state machine N */
    union {
        struct {
            uint32_t cr_sm1_txf_thr                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_6                   :  4; /* [ 6: 3],       rsvd,        0x0 */
            uint32_t st_sm1_txf_rdy                 :  1; /* [    7],          r,        0x1 */
            uint32_t cr_sm1_rxf_thr                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_14                 :  4; /* [14:11],       rsvd,        0x0 */
            uint32_t st_sm1_rxf_rdy                 :  1; /* [   15],          r,        0x0 */
            uint32_t cr_sm1_autopush                :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_sm1_autopull                :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_sm1_in_shiftdir             :  1; /* [   18],        r/w,        0x1 */
            uint32_t cr_sm1_out_shiftdir            :  1; /* [   19],        r/w,        0x1 */
            uint32_t cr_sm1_push_thresh             :  5; /* [24:20],        r/w,        0x0 */
            uint32_t cr_sm1_pull_thresh             :  5; /* [29:25],        r/w,        0x0 */
            uint32_t cr_sm1_txf_clr                 :  1; /* [   30],        w1c,        0x0 */
            uint32_t cr_sm1_rxf_clr                 :  1; /* [   31],        w1c,        0x0 */
        }BF;
        uint32_t WORD;
    } SM1_SHIFTCTRL;

    /* 0xEC : SM1_ADDR */
    union {
        struct {
            uint32_t st_sm1_addr                    :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } SM1_ADDR;

    /* 0xF0 : SM1_INSTR */
    union {
        struct {
            uint32_t cr_sm1_instr                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t st_sm1_instr                   : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM1_INSTR;

    /* 0xF4 : State machine pin control */
    union {
        struct {
            uint32_t cr_sm1_out_base                :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t cr_sm1_set_base                :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t cr_sm1_sideset_base            :  5; /* [14:10],        r/w,        0x0 */
            uint32_t cr_sm1_in_base                 :  5; /* [19:15],        r/w,        0x0 */
            uint32_t cr_sm1_out_count               :  6; /* [25:20],        r/w,        0x0 */
            uint32_t cr_sm1_set_count               :  3; /* [28:26],        r/w,        0x5 */
            uint32_t cr_sm1_sideset_count           :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } SM1_PINCTRL;

    /* 0xF8 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_sm2_frac                    :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t cr_sm2_int                     : 16; /* [31:16],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } SM2_CLKDIV;

    /* 0xFC : Execution/behavioural settings for state machine N */
    union {
        struct {
            uint32_t cr_sm2_status_n                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_sm2_status_sel              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t cr_sm2_wrap_bottom             :  5; /* [11: 7],        r/w,        0x0 */
            uint32_t cr_sm2_wrap_top                :  5; /* [16:12],        r/w,       0x1f */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t cr_sm2_jmp_pin                 :  5; /* [28:24],        r/w,        0x0 */
            uint32_t cr_sm2_side_pindir             :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_sm2_side_en                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t st_sm2_exec_stalled            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM2_EXECCTRL;

    /* 0x100 : Control behaviour of the input/output shift registers for state machine N */
    union {
        struct {
            uint32_t cr_sm2_txf_thr                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_6                   :  4; /* [ 6: 3],       rsvd,        0x0 */
            uint32_t st_sm2_txf_rdy                 :  1; /* [    7],          r,        0x1 */
            uint32_t cr_sm2_rxf_thr                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_14                 :  4; /* [14:11],       rsvd,        0x0 */
            uint32_t st_sm2_rxf_rdy                 :  1; /* [   15],          r,        0x0 */
            uint32_t cr_sm2_autopush                :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_sm2_autopull                :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_sm2_in_shiftdir             :  1; /* [   18],        r/w,        0x1 */
            uint32_t cr_sm2_out_shiftdir            :  1; /* [   19],        r/w,        0x1 */
            uint32_t cr_sm2_push_thresh             :  5; /* [24:20],        r/w,        0x0 */
            uint32_t cr_sm2_pull_thresh             :  5; /* [29:25],        r/w,        0x0 */
            uint32_t cr_sm2_txf_clr                 :  1; /* [   30],        w1c,        0x0 */
            uint32_t cr_sm2_rxf_clr                 :  1; /* [   31],        w1c,        0x0 */
        }BF;
        uint32_t WORD;
    } SM2_SHIFTCTRL;

    /* 0x104 : SM2_ADDR */
    union {
        struct {
            uint32_t st_sm2_addr                    :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } SM2_ADDR;

    /* 0x108 : SM2_INSTR */
    union {
        struct {
            uint32_t cr_sm2_instr                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t st_sm2_instr                   : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM2_INSTR;

    /* 0x10C : State machine pin control */
    union {
        struct {
            uint32_t cr_sm2_out_base                :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t cr_sm2_set_base                :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t cr_sm2_sideset_base            :  5; /* [14:10],        r/w,        0x0 */
            uint32_t cr_sm2_in_base                 :  5; /* [19:15],        r/w,        0x0 */
            uint32_t cr_sm2_out_count               :  6; /* [25:20],        r/w,        0x0 */
            uint32_t cr_sm2_set_count               :  3; /* [28:26],        r/w,        0x5 */
            uint32_t cr_sm2_sideset_count           :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } SM2_PINCTRL;

    /* 0x110 : Clock divider register for state machine N. Frequency = clock freq / (CLKDIV_INT + CLKDIV_FRAC / 256) */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_sm3_frac                    :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t cr_sm3_int                     : 16; /* [31:16],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } SM3_CLKDIV;

    /* 0x114 : Execution/behavioural settings for state machine N */
    union {
        struct {
            uint32_t cr_sm3_status_n                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_sm3_status_sel              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t cr_sm3_wrap_bottom             :  5; /* [11: 7],        r/w,        0x0 */
            uint32_t cr_sm3_wrap_top                :  5; /* [16:12],        r/w,       0x1f */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t cr_sm3_jmp_pin                 :  5; /* [28:24],        r/w,        0x0 */
            uint32_t cr_sm3_side_pindir             :  1; /* [   29],        r/w,        0x0 */
            uint32_t cr_sm3_side_en                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t st_sm3_exec_stalled            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM3_EXECCTRL;

    /* 0x118 : Control behaviour of the input/output shift registers for state machine N */
    union {
        struct {
            uint32_t cr_sm3_txf_thr                 :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3_6                   :  4; /* [ 6: 3],       rsvd,        0x0 */
            uint32_t st_sm3_txf_rdy                 :  1; /* [    7],          r,        0x1 */
            uint32_t cr_sm3_rxf_thr                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_14                 :  4; /* [14:11],       rsvd,        0x0 */
            uint32_t st_sm3_rxf_rdy                 :  1; /* [   15],          r,        0x0 */
            uint32_t cr_sm3_autopush                :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_sm3_autopull                :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_sm3_in_shiftdir             :  1; /* [   18],        r/w,        0x1 */
            uint32_t cr_sm3_out_shiftdir            :  1; /* [   19],        r/w,        0x1 */
            uint32_t cr_sm3_push_thresh             :  5; /* [24:20],        r/w,        0x0 */
            uint32_t cr_sm3_pull_thresh             :  5; /* [29:25],        r/w,        0x0 */
            uint32_t cr_sm3_txf_clr                 :  1; /* [   30],        w1c,        0x0 */
            uint32_t cr_sm3_rxf_clr                 :  1; /* [   31],        w1c,        0x0 */
        }BF;
        uint32_t WORD;
    } SM3_SHIFTCTRL;

    /* 0x11C : SM3_ADDR */
    union {
        struct {
            uint32_t st_sm3_addr                    :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } SM3_ADDR;

    /* 0x120 : SM3_INSTR */
    union {
        struct {
            uint32_t cr_sm3_instr                   : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t st_sm3_instr                   : 16; /* [31:16],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } SM3_INSTR;

    /* 0x124 : State machine pin control */
    union {
        struct {
            uint32_t cr_sm3_out_base                :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t cr_sm3_set_base                :  5; /* [ 9: 5],        r/w,        0x0 */
            uint32_t cr_sm3_sideset_base            :  5; /* [14:10],        r/w,        0x0 */
            uint32_t cr_sm3_in_base                 :  5; /* [19:15],        r/w,        0x0 */
            uint32_t cr_sm3_out_count               :  6; /* [25:20],        r/w,        0x0 */
            uint32_t cr_sm3_set_count               :  3; /* [28:26],        r/w,        0x5 */
            uint32_t cr_sm3_sideset_count           :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } SM3_PINCTRL;

    /* 0x128 : Raw Interrupts */
    union {
        struct {
            uint32_t st_intr                        : 16; /* [15: 0],          r,       0xf0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } INTR;

    /* 0x12C : Interrupt Enable for irq0 */
    union {
        struct {
            uint32_t cr_irq0_inte                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ0_INTE;

    /* 0x130 : Interrupt Force for irq0 */
    union {
        struct {
            uint32_t cr_irq0_intf                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ0_INTF;

    /* 0x134 : Interrupt status after masking & forcing for irq0 */
    union {
        struct {
            uint32_t st_irq0_ints                   : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ0_INTS;

    /* 0x138 : Interrupt Enable for irq1 */
    union {
        struct {
            uint32_t cr_irq1_inte                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ1_INTE;

    /* 0x13C : Interrupt Force for irq1 */
    union {
        struct {
            uint32_t cr_irq1_intf                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ1_INTF;

    /* 0x140 : Interrupt status after masking & forcing for irq1 */
    union {
        struct {
            uint32_t st_irq1_ints                   : 12; /* [11: 0],          r,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IRQ1_INTS;

    /* 0x144 : PAD_O_INV */
    union {
        struct {
            uint32_t cr_pad_o_inv                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PAD_O_INV;

    /* 0x148 : PAD_OE_INV */
    union {
        struct {
            uint32_t cr_pad_oe_inv                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PAD_OE_INV;

    /* 0x14C : PAD_I_INV */
    union {
        struct {
            uint32_t cr_pad_i_inv                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PAD_I_INV;

    /* 0x150 : PAD_O_SET */
    union {
        struct {
            uint32_t cr_pad_o_set                   : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } PAD_O_SET;

    /* 0x154 : PAD_OE_SET */
    union {
        struct {
            uint32_t cr_pad_oe_set                  : 32; /* [31: 0],          w,        0x0 */
        }BF;
        uint32_t WORD;
    } PAD_OE_SET;

    /* 0x158 : DMA_CONFIG */
    union {
        struct {
            uint32_t cr_sm0_dma_tx_en               :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_sm0_dma_rx_en               :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_sm1_dma_tx_en               :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_sm1_dma_rx_en               :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_sm2_dma_tx_en               :  1; /* [    4],        r/w,        0x0 */
            uint32_t cr_sm2_dma_rx_en               :  1; /* [    5],        r/w,        0x0 */
            uint32_t cr_sm3_dma_tx_en               :  1; /* [    6],        r/w,        0x0 */
            uint32_t cr_sm3_dma_rx_en               :  1; /* [    7],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } DMA_CONFIG;

};

typedef volatile struct pec_reg pec_reg_t;

#endif  /* __PEC_REG_H__ */
