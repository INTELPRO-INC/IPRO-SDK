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

#ifndef  __MCU_MISC_REG_H__
#define  __MCU_MISC_REG_H__

#include "ipro6.h"

/* 0x0 : mcu_bus_cfg0 */
#define MCU_MISC_MCU_BUS_CFG0_OFFSET                            (0x0)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN                       MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_POS                   (0U)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_LEN                   (1U)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_MSK                   (((1ULL<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_LEN)-1)<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_POS)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_UMSK                  (~(((1ULL<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_LEN)-1)<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_EN_POS))
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR                      MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_POS                  (1U)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_LEN                  (1U)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_MSK                  (((1ULL<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_LEN)-1)<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_POS)
#define MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_UMSK                 (~(((1ULL<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_LEN)-1)<<MCU_MISC_REG_INFRA_MCU_TIMEOUT_CLR_POS))
#define MCU_MISC_STS_INFRA_MCU_TIMEOUT                          MCU_MISC_STS_INFRA_MCU_TIMEOUT
#define MCU_MISC_STS_INFRA_MCU_TIMEOUT_POS                      (16U)
#define MCU_MISC_STS_INFRA_MCU_TIMEOUT_LEN                      (1U)
#define MCU_MISC_STS_INFRA_MCU_TIMEOUT_MSK                      (((1ULL<<MCU_MISC_STS_INFRA_MCU_TIMEOUT_LEN)-1)<<MCU_MISC_STS_INFRA_MCU_TIMEOUT_POS)
#define MCU_MISC_STS_INFRA_MCU_TIMEOUT_UMSK                     (~(((1ULL<<MCU_MISC_STS_INFRA_MCU_TIMEOUT_LEN)-1)<<MCU_MISC_STS_INFRA_MCU_TIMEOUT_POS))

/* 0x4 : mcu_bus_cfg1 */
#define MCU_MISC_MCU_BUS_CFG1_OFFSET                            (0x4)
#define MCU_MISC_REG_MCU_D_AWQOS                                MCU_MISC_REG_MCU_D_AWQOS
#define MCU_MISC_REG_MCU_D_AWQOS_POS                            (0U)
#define MCU_MISC_REG_MCU_D_AWQOS_LEN                            (1U)
#define MCU_MISC_REG_MCU_D_AWQOS_MSK                            (((1ULL<<MCU_MISC_REG_MCU_D_AWQOS_LEN)-1)<<MCU_MISC_REG_MCU_D_AWQOS_POS)
#define MCU_MISC_REG_MCU_D_AWQOS_UMSK                           (~(((1ULL<<MCU_MISC_REG_MCU_D_AWQOS_LEN)-1)<<MCU_MISC_REG_MCU_D_AWQOS_POS))
#define MCU_MISC_REG_MCU_D_ARQOS                                MCU_MISC_REG_MCU_D_ARQOS
#define MCU_MISC_REG_MCU_D_ARQOS_POS                            (1U)
#define MCU_MISC_REG_MCU_D_ARQOS_LEN                            (1U)
#define MCU_MISC_REG_MCU_D_ARQOS_MSK                            (((1ULL<<MCU_MISC_REG_MCU_D_ARQOS_LEN)-1)<<MCU_MISC_REG_MCU_D_ARQOS_POS)
#define MCU_MISC_REG_MCU_D_ARQOS_UMSK                           (~(((1ULL<<MCU_MISC_REG_MCU_D_ARQOS_LEN)-1)<<MCU_MISC_REG_MCU_D_ARQOS_POS))
#define MCU_MISC_REG_MCU_I_ARQOS                                MCU_MISC_REG_MCU_I_ARQOS
#define MCU_MISC_REG_MCU_I_ARQOS_POS                            (3U)
#define MCU_MISC_REG_MCU_I_ARQOS_LEN                            (1U)
#define MCU_MISC_REG_MCU_I_ARQOS_MSK                            (((1ULL<<MCU_MISC_REG_MCU_I_ARQOS_LEN)-1)<<MCU_MISC_REG_MCU_I_ARQOS_POS)
#define MCU_MISC_REG_MCU_I_ARQOS_UMSK                           (~(((1ULL<<MCU_MISC_REG_MCU_I_ARQOS_LEN)-1)<<MCU_MISC_REG_MCU_I_ARQOS_POS))
#define MCU_MISC_REG_ARB_MODE                                   MCU_MISC_REG_ARB_MODE
#define MCU_MISC_REG_ARB_MODE_POS                               (8U)
#define MCU_MISC_REG_ARB_MODE_LEN                               (1U)
#define MCU_MISC_REG_ARB_MODE_MSK                               (((1ULL<<MCU_MISC_REG_ARB_MODE_LEN)-1)<<MCU_MISC_REG_ARB_MODE_POS)
#define MCU_MISC_REG_ARB_MODE_UMSK                              (~(((1ULL<<MCU_MISC_REG_ARB_MODE_LEN)-1)<<MCU_MISC_REG_ARB_MODE_POS))

/* 0x40 : mcu_bus_err_csr_1 */
#define MCU_MISC_MCU_BUS_ERR_CSR_1_OFFSET                       (0x40)
#define MCU_MISC_REG_MCU_BERR_EN                                MCU_MISC_REG_MCU_BERR_EN
#define MCU_MISC_REG_MCU_BERR_EN_POS                            (0U)
#define MCU_MISC_REG_MCU_BERR_EN_LEN                            (3U)
#define MCU_MISC_REG_MCU_BERR_EN_MSK                            (((1ULL<<MCU_MISC_REG_MCU_BERR_EN_LEN)-1)<<MCU_MISC_REG_MCU_BERR_EN_POS)
#define MCU_MISC_REG_MCU_BERR_EN_UMSK                           (~(((1ULL<<MCU_MISC_REG_MCU_BERR_EN_LEN)-1)<<MCU_MISC_REG_MCU_BERR_EN_POS))
#define MCU_MISC_REG_MCU_BERR_CLR                               MCU_MISC_REG_MCU_BERR_CLR
#define MCU_MISC_REG_MCU_BERR_CLR_POS                           (4U)
#define MCU_MISC_REG_MCU_BERR_CLR_LEN                           (1U)
#define MCU_MISC_REG_MCU_BERR_CLR_MSK                           (((1ULL<<MCU_MISC_REG_MCU_BERR_CLR_LEN)-1)<<MCU_MISC_REG_MCU_BERR_CLR_POS)
#define MCU_MISC_REG_MCU_BERR_CLR_UMSK                          (~(((1ULL<<MCU_MISC_REG_MCU_BERR_CLR_LEN)-1)<<MCU_MISC_REG_MCU_BERR_CLR_POS))
#define MCU_MISC_REG_MCU_BERR_LAST                              MCU_MISC_REG_MCU_BERR_LAST
#define MCU_MISC_REG_MCU_BERR_LAST_POS                          (5U)
#define MCU_MISC_REG_MCU_BERR_LAST_LEN                          (1U)
#define MCU_MISC_REG_MCU_BERR_LAST_MSK                          (((1ULL<<MCU_MISC_REG_MCU_BERR_LAST_LEN)-1)<<MCU_MISC_REG_MCU_BERR_LAST_POS)
#define MCU_MISC_REG_MCU_BERR_LAST_UMSK                         (~(((1ULL<<MCU_MISC_REG_MCU_BERR_LAST_LEN)-1)<<MCU_MISC_REG_MCU_BERR_LAST_POS))
#define MCU_MISC_STS_MCU_BERR                                   MCU_MISC_STS_MCU_BERR
#define MCU_MISC_STS_MCU_BERR_POS                               (6U)
#define MCU_MISC_STS_MCU_BERR_LEN                               (1U)
#define MCU_MISC_STS_MCU_BERR_MSK                               (((1ULL<<MCU_MISC_STS_MCU_BERR_LEN)-1)<<MCU_MISC_STS_MCU_BERR_POS)
#define MCU_MISC_STS_MCU_BERR_UMSK                              (~(((1ULL<<MCU_MISC_STS_MCU_BERR_LEN)-1)<<MCU_MISC_STS_MCU_BERR_POS))
#define MCU_MISC_STS_MCU_BERR_WRITE                             MCU_MISC_STS_MCU_BERR_WRITE
#define MCU_MISC_STS_MCU_BERR_WRITE_POS                         (7U)
#define MCU_MISC_STS_MCU_BERR_WRITE_LEN                         (1U)
#define MCU_MISC_STS_MCU_BERR_WRITE_MSK                         (((1ULL<<MCU_MISC_STS_MCU_BERR_WRITE_LEN)-1)<<MCU_MISC_STS_MCU_BERR_WRITE_POS)
#define MCU_MISC_STS_MCU_BERR_WRITE_UMSK                        (~(((1ULL<<MCU_MISC_STS_MCU_BERR_WRITE_LEN)-1)<<MCU_MISC_STS_MCU_BERR_WRITE_POS))
#define MCU_MISC_STS_MCU_BERR_SRC                               MCU_MISC_STS_MCU_BERR_SRC
#define MCU_MISC_STS_MCU_BERR_SRC_POS                           (16U)
#define MCU_MISC_STS_MCU_BERR_SRC_LEN                           (3U)
#define MCU_MISC_STS_MCU_BERR_SRC_MSK                           (((1ULL<<MCU_MISC_STS_MCU_BERR_SRC_LEN)-1)<<MCU_MISC_STS_MCU_BERR_SRC_POS)
#define MCU_MISC_STS_MCU_BERR_SRC_UMSK                          (~(((1ULL<<MCU_MISC_STS_MCU_BERR_SRC_LEN)-1)<<MCU_MISC_STS_MCU_BERR_SRC_POS))
#define MCU_MISC_STS_MCU_BERR_ID                                MCU_MISC_STS_MCU_BERR_ID
#define MCU_MISC_STS_MCU_BERR_ID_POS                            (20U)
#define MCU_MISC_STS_MCU_BERR_ID_LEN                            (2U)
#define MCU_MISC_STS_MCU_BERR_ID_MSK                            (((1ULL<<MCU_MISC_STS_MCU_BERR_ID_LEN)-1)<<MCU_MISC_STS_MCU_BERR_ID_POS)
#define MCU_MISC_STS_MCU_BERR_ID_UMSK                           (~(((1ULL<<MCU_MISC_STS_MCU_BERR_ID_LEN)-1)<<MCU_MISC_STS_MCU_BERR_ID_POS))

/* 0x100 : main_mcu_cfg1 */
#define MCU_MISC_MAIN_MCU_CFG1_OFFSET                           (0x100)
#define MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP                       MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP
#define MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_POS                   (0U)
#define MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_LEN                   (1U)
#define MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_MSK                   (((1ULL<<MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_LEN)-1)<<MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_POS)
#define MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_UMSK                  (~(((1ULL<<MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_LEN)-1)<<MCU_MISC_CR_DBG_OVERRIDE_DM_SLEEP_POS))
#define MCU_MISC_CR_FORCE_MCU_WFI                               MCU_MISC_CR_FORCE_MCU_WFI
#define MCU_MISC_CR_FORCE_MCU_WFI_POS                           (1U)
#define MCU_MISC_CR_FORCE_MCU_WFI_LEN                           (1U)
#define MCU_MISC_CR_FORCE_MCU_WFI_MSK                           (((1ULL<<MCU_MISC_CR_FORCE_MCU_WFI_LEN)-1)<<MCU_MISC_CR_FORCE_MCU_WFI_POS)
#define MCU_MISC_CR_FORCE_MCU_WFI_UMSK                          (~(((1ULL<<MCU_MISC_CR_FORCE_MCU_WFI_LEN)-1)<<MCU_MISC_CR_FORCE_MCU_WFI_POS))
#define MCU_MISC_STS_MAIN_WFI                                   MCU_MISC_STS_MAIN_WFI
#define MCU_MISC_STS_MAIN_WFI_POS                               (6U)
#define MCU_MISC_STS_MAIN_WFI_LEN                               (1U)
#define MCU_MISC_STS_MAIN_WFI_MSK                               (((1ULL<<MCU_MISC_STS_MAIN_WFI_LEN)-1)<<MCU_MISC_STS_MAIN_WFI_POS)
#define MCU_MISC_STS_MAIN_WFI_UMSK                              (~(((1ULL<<MCU_MISC_STS_MAIN_WFI_LEN)-1)<<MCU_MISC_STS_MAIN_WFI_POS))
#define MCU_MISC_STS_MAIN_SLEEP_VALUE                           MCU_MISC_STS_MAIN_SLEEP_VALUE
#define MCU_MISC_STS_MAIN_SLEEP_VALUE_POS                       (10U)
#define MCU_MISC_STS_MAIN_SLEEP_VALUE_LEN                       (1U)
#define MCU_MISC_STS_MAIN_SLEEP_VALUE_MSK                       (((1ULL<<MCU_MISC_STS_MAIN_SLEEP_VALUE_LEN)-1)<<MCU_MISC_STS_MAIN_SLEEP_VALUE_POS)
#define MCU_MISC_STS_MAIN_SLEEP_VALUE_UMSK                      (~(((1ULL<<MCU_MISC_STS_MAIN_SLEEP_VALUE_LEN)-1)<<MCU_MISC_STS_MAIN_SLEEP_VALUE_POS))
#define MCU_MISC_STS_MAIN_HALTED                                MCU_MISC_STS_MAIN_HALTED
#define MCU_MISC_STS_MAIN_HALTED_POS                            (16U)
#define MCU_MISC_STS_MAIN_HALTED_LEN                            (1U)
#define MCU_MISC_STS_MAIN_HALTED_MSK                            (((1ULL<<MCU_MISC_STS_MAIN_HALTED_LEN)-1)<<MCU_MISC_STS_MAIN_HALTED_POS)
#define MCU_MISC_STS_MAIN_HALTED_UMSK                           (~(((1ULL<<MCU_MISC_STS_MAIN_HALTED_LEN)-1)<<MCU_MISC_STS_MAIN_HALTED_POS))
#define MCU_MISC_STS_MAIN_DBG_STOPTIME                          MCU_MISC_STS_MAIN_DBG_STOPTIME
#define MCU_MISC_STS_MAIN_DBG_STOPTIME_POS                      (17U)
#define MCU_MISC_STS_MAIN_DBG_STOPTIME_LEN                      (1U)
#define MCU_MISC_STS_MAIN_DBG_STOPTIME_MSK                      (((1ULL<<MCU_MISC_STS_MAIN_DBG_STOPTIME_LEN)-1)<<MCU_MISC_STS_MAIN_DBG_STOPTIME_POS)
#define MCU_MISC_STS_MAIN_DBG_STOPTIME_UMSK                     (~(((1ULL<<MCU_MISC_STS_MAIN_DBG_STOPTIME_LEN)-1)<<MCU_MISC_STS_MAIN_DBG_STOPTIME_POS))

/* 0x104 : main_mcu_cfg2 */
#define MCU_MISC_MAIN_MCU_CFG2_OFFSET                           (0x104)
#define MCU_MISC_CR_MTIME_TOGGLE_CNT                            MCU_MISC_CR_MTIME_TOGGLE_CNT
#define MCU_MISC_CR_MTIME_TOGGLE_CNT_POS                        (0U)
#define MCU_MISC_CR_MTIME_TOGGLE_CNT_LEN                        (16U)
#define MCU_MISC_CR_MTIME_TOGGLE_CNT_MSK                        (((1ULL<<MCU_MISC_CR_MTIME_TOGGLE_CNT_LEN)-1)<<MCU_MISC_CR_MTIME_TOGGLE_CNT_POS)
#define MCU_MISC_CR_MTIME_TOGGLE_CNT_UMSK                       (~(((1ULL<<MCU_MISC_CR_MTIME_TOGGLE_CNT_LEN)-1)<<MCU_MISC_CR_MTIME_TOGGLE_CNT_POS))
#define MCU_MISC_CR_DBG_TOGGLE_CNT                              MCU_MISC_CR_DBG_TOGGLE_CNT
#define MCU_MISC_CR_DBG_TOGGLE_CNT_POS                          (16U)
#define MCU_MISC_CR_DBG_TOGGLE_CNT_LEN                          (16U)
#define MCU_MISC_CR_DBG_TOGGLE_CNT_MSK                          (((1ULL<<MCU_MISC_CR_DBG_TOGGLE_CNT_LEN)-1)<<MCU_MISC_CR_DBG_TOGGLE_CNT_POS)
#define MCU_MISC_CR_DBG_TOGGLE_CNT_UMSK                         (~(((1ULL<<MCU_MISC_CR_DBG_TOGGLE_CNT_LEN)-1)<<MCU_MISC_CR_DBG_TOGGLE_CNT_POS))

/* 0x110 : sts_mmcu_pipe_0 */
#define MCU_MISC_STS_MMCU_PIPE_0_OFFSET                         (0x110)
#define MCU_MISC_STS_MCU0_TRACE_IEXCEPTION                      MCU_MISC_STS_MCU0_TRACE_IEXCEPTION
#define MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_POS                  (0U)
#define MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_LEN                  (1U)
#define MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_MSK                  (((1ULL<<MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_POS)
#define MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_UMSK                 (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_IEXCEPTION_POS))
#define MCU_MISC_STS_MCU0_TRACE_INTERRUPT                       MCU_MISC_STS_MCU0_TRACE_INTERRUPT
#define MCU_MISC_STS_MCU0_TRACE_INTERRUPT_POS                   (1U)
#define MCU_MISC_STS_MCU0_TRACE_INTERRUPT_LEN                   (1U)
#define MCU_MISC_STS_MCU0_TRACE_INTERRUPT_MSK                   (((1ULL<<MCU_MISC_STS_MCU0_TRACE_INTERRUPT_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_INTERRUPT_POS)
#define MCU_MISC_STS_MCU0_TRACE_INTERRUPT_UMSK                  (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_INTERRUPT_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_INTERRUPT_POS))
#define MCU_MISC_STS_MCU0_TRACE_PRIV                            MCU_MISC_STS_MCU0_TRACE_PRIV
#define MCU_MISC_STS_MCU0_TRACE_PRIV_POS                        (2U)
#define MCU_MISC_STS_MCU0_TRACE_PRIV_LEN                        (2U)
#define MCU_MISC_STS_MCU0_TRACE_PRIV_MSK                        (((1ULL<<MCU_MISC_STS_MCU0_TRACE_PRIV_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_PRIV_POS)
#define MCU_MISC_STS_MCU0_TRACE_PRIV_UMSK                       (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_PRIV_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_PRIV_POS))
#define MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN                       MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN
#define MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_POS                   (4U)
#define MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_LEN                   (1U)
#define MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_MSK                   (((1ULL<<MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_POS)
#define MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_UMSK                  (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_BJP_TAKEN_POS))
#define MCU_MISC_STS_MCU0_TRACE_DMODE                           MCU_MISC_STS_MCU0_TRACE_DMODE
#define MCU_MISC_STS_MCU0_TRACE_DMODE_POS                       (5U)
#define MCU_MISC_STS_MCU0_TRACE_DMODE_LEN                       (1U)
#define MCU_MISC_STS_MCU0_TRACE_DMODE_MSK                       (((1ULL<<MCU_MISC_STS_MCU0_TRACE_DMODE_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_DMODE_POS)
#define MCU_MISC_STS_MCU0_TRACE_DMODE_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_DMODE_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_DMODE_POS))
#define MCU_MISC_STS_MCU0_TRACE_CMT_ENA                         MCU_MISC_STS_MCU0_TRACE_CMT_ENA
#define MCU_MISC_STS_MCU0_TRACE_CMT_ENA_POS                     (6U)
#define MCU_MISC_STS_MCU0_TRACE_CMT_ENA_LEN                     (1U)
#define MCU_MISC_STS_MCU0_TRACE_CMT_ENA_MSK                     (((1ULL<<MCU_MISC_STS_MCU0_TRACE_CMT_ENA_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_CMT_ENA_POS)
#define MCU_MISC_STS_MCU0_TRACE_CMT_ENA_UMSK                    (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_CMT_ENA_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_CMT_ENA_POS))
#define MCU_MISC_STS_MCU1_TRACE_IEXCEPTION                      MCU_MISC_STS_MCU1_TRACE_IEXCEPTION
#define MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_POS                  (16U)
#define MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_LEN                  (1U)
#define MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_MSK                  (((1ULL<<MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_POS)
#define MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_UMSK                 (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_IEXCEPTION_POS))
#define MCU_MISC_STS_MCU1_TRACE_INTERRUPT                       MCU_MISC_STS_MCU1_TRACE_INTERRUPT
#define MCU_MISC_STS_MCU1_TRACE_INTERRUPT_POS                   (17U)
#define MCU_MISC_STS_MCU1_TRACE_INTERRUPT_LEN                   (1U)
#define MCU_MISC_STS_MCU1_TRACE_INTERRUPT_MSK                   (((1ULL<<MCU_MISC_STS_MCU1_TRACE_INTERRUPT_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_INTERRUPT_POS)
#define MCU_MISC_STS_MCU1_TRACE_INTERRUPT_UMSK                  (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_INTERRUPT_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_INTERRUPT_POS))
#define MCU_MISC_STS_MCU1_TRACE_PRIV                            MCU_MISC_STS_MCU1_TRACE_PRIV
#define MCU_MISC_STS_MCU1_TRACE_PRIV_POS                        (18U)
#define MCU_MISC_STS_MCU1_TRACE_PRIV_LEN                        (2U)
#define MCU_MISC_STS_MCU1_TRACE_PRIV_MSK                        (((1ULL<<MCU_MISC_STS_MCU1_TRACE_PRIV_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_PRIV_POS)
#define MCU_MISC_STS_MCU1_TRACE_PRIV_UMSK                       (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_PRIV_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_PRIV_POS))
#define MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN                       MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN
#define MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_POS                   (20U)
#define MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_LEN                   (1U)
#define MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_MSK                   (((1ULL<<MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_POS)
#define MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_UMSK                  (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_BJP_TAKEN_POS))
#define MCU_MISC_STS_MCU1_TRACE_DMODE                           MCU_MISC_STS_MCU1_TRACE_DMODE
#define MCU_MISC_STS_MCU1_TRACE_DMODE_POS                       (21U)
#define MCU_MISC_STS_MCU1_TRACE_DMODE_LEN                       (1U)
#define MCU_MISC_STS_MCU1_TRACE_DMODE_MSK                       (((1ULL<<MCU_MISC_STS_MCU1_TRACE_DMODE_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_DMODE_POS)
#define MCU_MISC_STS_MCU1_TRACE_DMODE_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_DMODE_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_DMODE_POS))
#define MCU_MISC_STS_MCU1_TRACE_CMT_ENA                         MCU_MISC_STS_MCU1_TRACE_CMT_ENA
#define MCU_MISC_STS_MCU1_TRACE_CMT_ENA_POS                     (22U)
#define MCU_MISC_STS_MCU1_TRACE_CMT_ENA_LEN                     (1U)
#define MCU_MISC_STS_MCU1_TRACE_CMT_ENA_MSK                     (((1ULL<<MCU_MISC_STS_MCU1_TRACE_CMT_ENA_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_CMT_ENA_POS)
#define MCU_MISC_STS_MCU1_TRACE_CMT_ENA_UMSK                    (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_CMT_ENA_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_CMT_ENA_POS))

/* 0x114 : sts_mmcu_pipe_1 */
#define MCU_MISC_STS_MMCU_PIPE_1_OFFSET                         (0x114)
#define MCU_MISC_STS_MCU0_TRACE_CAUSE                           MCU_MISC_STS_MCU0_TRACE_CAUSE
#define MCU_MISC_STS_MCU0_TRACE_CAUSE_POS                       (0U)
#define MCU_MISC_STS_MCU0_TRACE_CAUSE_LEN                       (32U)
#define MCU_MISC_STS_MCU0_TRACE_CAUSE_MSK                       (((1ULL<<MCU_MISC_STS_MCU0_TRACE_CAUSE_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_CAUSE_POS)
#define MCU_MISC_STS_MCU0_TRACE_CAUSE_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_CAUSE_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_CAUSE_POS))

/* 0x118 : sts_mmcu_pipe_2 */
#define MCU_MISC_STS_MMCU_PIPE_2_OFFSET                         (0x118)
#define MCU_MISC_STS_MCU0_TRACE_TVAL                            MCU_MISC_STS_MCU0_TRACE_TVAL
#define MCU_MISC_STS_MCU0_TRACE_TVAL_POS                        (0U)
#define MCU_MISC_STS_MCU0_TRACE_TVAL_LEN                        (32U)
#define MCU_MISC_STS_MCU0_TRACE_TVAL_MSK                        (((1ULL<<MCU_MISC_STS_MCU0_TRACE_TVAL_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_TVAL_POS)
#define MCU_MISC_STS_MCU0_TRACE_TVAL_UMSK                       (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_TVAL_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_TVAL_POS))

/* 0x11C : sts_mmcu_pipe_3 */
#define MCU_MISC_STS_MMCU_PIPE_3_OFFSET                         (0x11C)
#define MCU_MISC_STS_MCU0_TRACE_IADDR                           MCU_MISC_STS_MCU0_TRACE_IADDR
#define MCU_MISC_STS_MCU0_TRACE_IADDR_POS                       (0U)
#define MCU_MISC_STS_MCU0_TRACE_IADDR_LEN                       (32U)
#define MCU_MISC_STS_MCU0_TRACE_IADDR_MSK                       (((1ULL<<MCU_MISC_STS_MCU0_TRACE_IADDR_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_IADDR_POS)
#define MCU_MISC_STS_MCU0_TRACE_IADDR_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_IADDR_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_IADDR_POS))

/* 0x120 : sts_mmcu_pipe_4 */
#define MCU_MISC_STS_MMCU_PIPE_4_OFFSET                         (0x120)
#define MCU_MISC_STS_MCU0_TRACE_INSTR                           MCU_MISC_STS_MCU0_TRACE_INSTR
#define MCU_MISC_STS_MCU0_TRACE_INSTR_POS                       (0U)
#define MCU_MISC_STS_MCU0_TRACE_INSTR_LEN                       (32U)
#define MCU_MISC_STS_MCU0_TRACE_INSTR_MSK                       (((1ULL<<MCU_MISC_STS_MCU0_TRACE_INSTR_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_INSTR_POS)
#define MCU_MISC_STS_MCU0_TRACE_INSTR_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU0_TRACE_INSTR_LEN)-1)<<MCU_MISC_STS_MCU0_TRACE_INSTR_POS))

/* 0x124 : sts_mmcu_pipe_5 */
#define MCU_MISC_STS_MMCU_PIPE_5_OFFSET                         (0x124)
#define MCU_MISC_STS_MCU1_TRACE_CAUSE                           MCU_MISC_STS_MCU1_TRACE_CAUSE
#define MCU_MISC_STS_MCU1_TRACE_CAUSE_POS                       (0U)
#define MCU_MISC_STS_MCU1_TRACE_CAUSE_LEN                       (32U)
#define MCU_MISC_STS_MCU1_TRACE_CAUSE_MSK                       (((1ULL<<MCU_MISC_STS_MCU1_TRACE_CAUSE_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_CAUSE_POS)
#define MCU_MISC_STS_MCU1_TRACE_CAUSE_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_CAUSE_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_CAUSE_POS))

/* 0x128 : sts_mmcu_pipe_6 */
#define MCU_MISC_STS_MMCU_PIPE_6_OFFSET                         (0x128)
#define MCU_MISC_STS_MCU1_TRACE_TVAL                            MCU_MISC_STS_MCU1_TRACE_TVAL
#define MCU_MISC_STS_MCU1_TRACE_TVAL_POS                        (0U)
#define MCU_MISC_STS_MCU1_TRACE_TVAL_LEN                        (32U)
#define MCU_MISC_STS_MCU1_TRACE_TVAL_MSK                        (((1ULL<<MCU_MISC_STS_MCU1_TRACE_TVAL_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_TVAL_POS)
#define MCU_MISC_STS_MCU1_TRACE_TVAL_UMSK                       (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_TVAL_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_TVAL_POS))

/* 0x12C : sts_mmcu_pipe_7 */
#define MCU_MISC_STS_MMCU_PIPE_7_OFFSET                         (0x12C)
#define MCU_MISC_STS_MCU1_TRACE_IADDR                           MCU_MISC_STS_MCU1_TRACE_IADDR
#define MCU_MISC_STS_MCU1_TRACE_IADDR_POS                       (0U)
#define MCU_MISC_STS_MCU1_TRACE_IADDR_LEN                       (32U)
#define MCU_MISC_STS_MCU1_TRACE_IADDR_MSK                       (((1ULL<<MCU_MISC_STS_MCU1_TRACE_IADDR_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_IADDR_POS)
#define MCU_MISC_STS_MCU1_TRACE_IADDR_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_IADDR_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_IADDR_POS))

/* 0x130 : sts_mmcu_pipe_8 */
#define MCU_MISC_STS_MMCU_PIPE_8_OFFSET                         (0x130)
#define MCU_MISC_STS_MCU1_TRACE_INSTR                           MCU_MISC_STS_MCU1_TRACE_INSTR
#define MCU_MISC_STS_MCU1_TRACE_INSTR_POS                       (0U)
#define MCU_MISC_STS_MCU1_TRACE_INSTR_LEN                       (32U)
#define MCU_MISC_STS_MCU1_TRACE_INSTR_MSK                       (((1ULL<<MCU_MISC_STS_MCU1_TRACE_INSTR_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_INSTR_POS)
#define MCU_MISC_STS_MCU1_TRACE_INSTR_UMSK                      (~(((1ULL<<MCU_MISC_STS_MCU1_TRACE_INSTR_LEN)-1)<<MCU_MISC_STS_MCU1_TRACE_INSTR_POS))

/* 0x200 : cpu_mbist */
#define MCU_MISC_CPU_MBIST_OFFSET                               (0x200)
#define MCU_MISC_CPU_MBIST_MODE                                 MCU_MISC_CPU_MBIST_MODE
#define MCU_MISC_CPU_MBIST_MODE_POS                             (0U)
#define MCU_MISC_CPU_MBIST_MODE_LEN                             (1U)
#define MCU_MISC_CPU_MBIST_MODE_MSK                             (((1ULL<<MCU_MISC_CPU_MBIST_MODE_LEN)-1)<<MCU_MISC_CPU_MBIST_MODE_POS)
#define MCU_MISC_CPU_MBIST_MODE_UMSK                            (~(((1ULL<<MCU_MISC_CPU_MBIST_MODE_LEN)-1)<<MCU_MISC_CPU_MBIST_MODE_POS))
#define MCU_MISC_IROM_MBIST_MODE                                MCU_MISC_IROM_MBIST_MODE
#define MCU_MISC_IROM_MBIST_MODE_POS                            (1U)
#define MCU_MISC_IROM_MBIST_MODE_LEN                            (1U)
#define MCU_MISC_IROM_MBIST_MODE_MSK                            (((1ULL<<MCU_MISC_IROM_MBIST_MODE_LEN)-1)<<MCU_MISC_IROM_MBIST_MODE_POS)
#define MCU_MISC_IROM_MBIST_MODE_UMSK                           (~(((1ULL<<MCU_MISC_IROM_MBIST_MODE_LEN)-1)<<MCU_MISC_IROM_MBIST_MODE_POS))
#define MCU_MISC_REG_CPU_MBIST_RST_N                            MCU_MISC_REG_CPU_MBIST_RST_N
#define MCU_MISC_REG_CPU_MBIST_RST_N_POS                        (8U)
#define MCU_MISC_REG_CPU_MBIST_RST_N_LEN                        (1U)
#define MCU_MISC_REG_CPU_MBIST_RST_N_MSK                        (((1ULL<<MCU_MISC_REG_CPU_MBIST_RST_N_LEN)-1)<<MCU_MISC_REG_CPU_MBIST_RST_N_POS)
#define MCU_MISC_REG_CPU_MBIST_RST_N_UMSK                       (~(((1ULL<<MCU_MISC_REG_CPU_MBIST_RST_N_LEN)-1)<<MCU_MISC_REG_CPU_MBIST_RST_N_POS))
#define MCU_MISC_CR_MCU_CACHE_RET1                              MCU_MISC_CR_MCU_CACHE_RET1
#define MCU_MISC_CR_MCU_CACHE_RET1_POS                          (28U)
#define MCU_MISC_CR_MCU_CACHE_RET1_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_RET1_MSK                          (((1ULL<<MCU_MISC_CR_MCU_CACHE_RET1_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET1_POS)
#define MCU_MISC_CR_MCU_CACHE_RET1_UMSK                         (~(((1ULL<<MCU_MISC_CR_MCU_CACHE_RET1_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET1_POS))
#define MCU_MISC_CR_MCU_CACHE_RET2                              MCU_MISC_CR_MCU_CACHE_RET2
#define MCU_MISC_CR_MCU_CACHE_RET2_POS                          (29U)
#define MCU_MISC_CR_MCU_CACHE_RET2_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_RET2_MSK                          (((1ULL<<MCU_MISC_CR_MCU_CACHE_RET2_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET2_POS)
#define MCU_MISC_CR_MCU_CACHE_RET2_UMSK                         (~(((1ULL<<MCU_MISC_CR_MCU_CACHE_RET2_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET2_POS))
#define MCU_MISC_CR_MCU_CACHE_PGEN                              MCU_MISC_CR_MCU_CACHE_PGEN
#define MCU_MISC_CR_MCU_CACHE_PGEN_POS                          (30U)
#define MCU_MISC_CR_MCU_CACHE_PGEN_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_PGEN_MSK                          (((1ULL<<MCU_MISC_CR_MCU_CACHE_PGEN_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_PGEN_POS)
#define MCU_MISC_CR_MCU_CACHE_PGEN_UMSK                         (~(((1ULL<<MCU_MISC_CR_MCU_CACHE_PGEN_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_PGEN_POS))

/* 0x204 : cpu_mbist2 */
#define MCU_MISC_CPU_MBIST2_OFFSET                              (0x204)
#define MCU_MISC_CR_RAM_BUS_MCU_CACHE                           MCU_MISC_CR_RAM_BUS_MCU_CACHE
#define MCU_MISC_CR_RAM_BUS_MCU_CACHE_POS                       (0U)
#define MCU_MISC_CR_RAM_BUS_MCU_CACHE_LEN                       (15U)
#define MCU_MISC_CR_RAM_BUS_MCU_CACHE_MSK                       (((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_CACHE_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_CACHE_POS)
#define MCU_MISC_CR_RAM_BUS_MCU_CACHE_UMSK                      (~(((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_CACHE_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_CACHE_POS))
#define MCU_MISC_CR_RAM_BUS_MCU                                 MCU_MISC_CR_RAM_BUS_MCU
#define MCU_MISC_CR_RAM_BUS_MCU_POS                             (0U)
#define MCU_MISC_CR_RAM_BUS_MCU_LEN                             (24U)
#define MCU_MISC_CR_RAM_BUS_MCU_MSK                             (((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_POS)
#define MCU_MISC_CR_RAM_BUS_MCU_UMSK                            (~(((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_POS))
#define MCU_MISC_CR_RAM_BUS_MCU_ROM                             MCU_MISC_CR_RAM_BUS_MCU_ROM
#define MCU_MISC_CR_RAM_BUS_MCU_ROM_POS                         (16U)
#define MCU_MISC_CR_RAM_BUS_MCU_ROM_LEN                         (15U)
#define MCU_MISC_CR_RAM_BUS_MCU_ROM_MSK                         (((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_ROM_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_ROM_POS)
#define MCU_MISC_CR_RAM_BUS_MCU_ROM_UMSK                        (~(((1ULL<<MCU_MISC_CR_RAM_BUS_MCU_ROM_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_ROM_POS))
#define MCU_MISC_CR_4S_RAM_CLK_EN                               MCU_MISC_CR_4S_RAM_CLK_EN
#define MCU_MISC_CR_4S_RAM_CLK_EN_POS                           (31U)
#define MCU_MISC_CR_4S_RAM_CLK_EN_LEN                           (1U)
#define MCU_MISC_CR_4S_RAM_CLK_EN_MSK                           (((1ULL<<MCU_MISC_CR_4S_RAM_CLK_EN_LEN)-1)<<MCU_MISC_CR_4S_RAM_CLK_EN_POS)
#define MCU_MISC_CR_4S_RAM_CLK_EN_UMSK                          (~(((1ULL<<MCU_MISC_CR_4S_RAM_CLK_EN_LEN)-1)<<MCU_MISC_CR_4S_RAM_CLK_EN_POS))


struct  mcu_misc_reg {
    /* 0x0 : mcu_bus_cfg0 */
    union {
        struct {
            uint32_t reg_infra_mcu_timeout_en       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_infra_mcu_timeout_clr      :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_15                  : 14; /* [15: 2],       rsvd,        0x0 */
            uint32_t sts_infra_mcu_timeout          :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg0;

    /* 0x4 : mcu_bus_cfg1 */
    union {
        struct {
            uint32_t reg_mcu_d_awqos                :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_mcu_d_arqos                :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t reg_mcu_i_arqos                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t reg_arb_mode                   :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg1;

    /* 0x8  reserved */
    uint8_t RESERVED0x8[56];

    /* 0x40 : mcu_bus_err_csr_1 */
    union {
        struct {
            uint32_t reg_mcu_berr_en                :  3; /* [ 2: 0],        r/w,        0x7 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t reg_mcu_berr_clr               :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_mcu_berr_last              :  1; /* [    5],        r/w,        0x0 */
            uint32_t sts_mcu_berr                   :  1; /* [    6],          r,        0x0 */
            uint32_t sts_mcu_berr_write             :  1; /* [    7],          r,        0x0 */
            uint32_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
            uint32_t sts_mcu_berr_src               :  3; /* [18:16],          r,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t sts_mcu_berr_id                :  2; /* [21:20],          r,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_err_csr_1;

    /* 0x44  reserved */
    uint8_t RESERVED0x44[188];

    /* 0x100 : main_mcu_cfg1 */
    union {
        struct {
            uint32_t cr_dbg_override_dm_sleep       :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_force_mcu_wfi               :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_5                   :  4; /* [ 5: 2],       rsvd,        0x0 */
            uint32_t sts_main_wfi                   :  1; /* [    6],          r,        0x0 */
            uint32_t reserved_7_9                   :  3; /* [ 9: 7],       rsvd,        0x0 */
            uint32_t sts_main_sleep_value           :  1; /* [   10],          r,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t sts_main_halted                :  1; /* [   16],          r,        0x0 */
            uint32_t sts_main_dbg_stoptime          :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } main_mcu_cfg1;

    /* 0x104 : main_mcu_cfg2 */
    union {
        struct {
            uint32_t cr_mtime_toggle_cnt            : 16; /* [15: 0],        r/w,      0x100 */
            uint32_t cr_dbg_toggle_cnt              : 16; /* [31:16],        r/w,      0x100 */
        }BF;
        uint32_t WORD;
    } main_mcu_cfg2;

    /* 0x108  reserved */
    uint8_t RESERVED0x108[8];

    /* 0x110 : sts_mmcu_pipe_0 */
    union {
        struct {
            uint32_t sts_mcu0_trace_iexception      :  1; /* [    0],          r,        0x0 */
            uint32_t sts_mcu0_trace_interrupt       :  1; /* [    1],          r,        0x0 */
            uint32_t sts_mcu0_trace_priv            :  2; /* [ 3: 2],          r,        0x0 */
            uint32_t sts_mcu0_trace_bjp_taken       :  1; /* [    4],          r,        0x0 */
            uint32_t sts_mcu0_trace_dmode           :  1; /* [    5],          r,        0x0 */
            uint32_t sts_mcu0_trace_cmt_ena         :  1; /* [    6],          r,        0x0 */
            uint32_t reserved_7_15                  :  9; /* [15: 7],       rsvd,        0x0 */
            uint32_t sts_mcu1_trace_iexception      :  1; /* [   16],          r,        0x0 */
            uint32_t sts_mcu1_trace_interrupt       :  1; /* [   17],          r,        0x0 */
            uint32_t sts_mcu1_trace_priv            :  2; /* [19:18],          r,        0x0 */
            uint32_t sts_mcu1_trace_bjp_taken       :  1; /* [   20],          r,        0x0 */
            uint32_t sts_mcu1_trace_dmode           :  1; /* [   21],          r,        0x0 */
            uint32_t sts_mcu1_trace_cmt_ena         :  1; /* [   22],          r,        0x0 */
            uint32_t reserved_23_31                 :  9; /* [31:23],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_0;

    /* 0x114 : sts_mmcu_pipe_1 */
    union {
        struct {
            uint32_t sts_mcu0_trace_cause           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_1;

    /* 0x118 : sts_mmcu_pipe_2 */
    union {
        struct {
            uint32_t sts_mcu0_trace_tval            : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_2;

    /* 0x11C : sts_mmcu_pipe_3 */
    union {
        struct {
            uint32_t sts_mcu0_trace_iaddr           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_3;

    /* 0x120 : sts_mmcu_pipe_4 */
    union {
        struct {
            uint32_t sts_mcu0_trace_instr           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_4;

    /* 0x124 : sts_mmcu_pipe_5 */
    union {
        struct {
            uint32_t sts_mcu1_trace_cause           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_5;

    /* 0x128 : sts_mmcu_pipe_6 */
    union {
        struct {
            uint32_t sts_mcu1_trace_tval            : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_6;

    /* 0x12C : sts_mmcu_pipe_7 */
    union {
        struct {
            uint32_t sts_mcu1_trace_iaddr           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_7;

    /* 0x130 : sts_mmcu_pipe_8 */
    union {
        struct {
            uint32_t sts_mcu1_trace_instr           : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } sts_mmcu_pipe_8;

    /* 0x134  reserved */
    uint8_t RESERVED0x134[204];

    /* 0x200 : cpu_mbist */
    union {
        struct {
            uint32_t cpu_mbist_mode                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t irom_mbist_mode                :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t reg_cpu_mbist_rst_n            :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_27                  : 19; /* [27: 9],       rsvd,        0x0 */
            uint32_t cr_mcu_cache_ret1              :  1; /* [   28],        r/w,        0x0 */
            uint32_t cr_mcu_cache_ret2              :  1; /* [   29],        r/w,        0x1 */
            uint32_t cr_mcu_cache_pgen              :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpu_mbist;

    /* 0x204 : cpu_mbist2 */
    /* alternate names (use *_MSK macros): cr_ram_bus_mcu([23:0]) */
    union {
        struct {
            uint32_t cr_ram_bus_mcu_cache           : 15; /* [14: 0],        r/w,     0x1520 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t cr_ram_bus_mcu_rom             : 15; /* [30:16],        r/w,     0x1520 */
            uint32_t cr_4s_ram_clk_en               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cpu_mbist2;

};

typedef volatile struct mcu_misc_reg mcu_misc_reg_t;


#endif  /* __MCU_MISC_REG_H__ */
