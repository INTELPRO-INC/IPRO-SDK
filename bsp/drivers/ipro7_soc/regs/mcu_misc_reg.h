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

#include "ipro7.h"

/* 0x0 : L1C_CFG0 */
#define MCU_MISC_L1C_CFG0_OFFSET                                (0x0)

/* 0x4 : hit_cnt_lsb */
#define MCU_MISC_HIT_CNT_LSB_OFFSET                             (0x4)

/* 0x8 : hit_cnt_msb */
#define MCU_MISC_HIT_CNT_MSB_OFFSET                             (0x8)

/* 0xC : miss_cnt */
#define MCU_MISC_MISS_CNT_OFFSET                                (0xC)

/* 0x10 : l1c_misc */
#define MCU_MISC_L1C_MISC_OFFSET                                (0x10)

/* 0x40 : mcu_bus_cfg0 */
#define MCU_MISC_MCU_BUS_CFG0_OFFSET                            (0x40)
#define MCU_MISC_CR_ROM_NT_ACCESS                               MCU_MISC_CR_ROM_NT_ACCESS
#define MCU_MISC_CR_ROM_NT_ACCESS_POS                           (0U)
#define MCU_MISC_CR_ROM_NT_ACCESS_LEN                           (3U)
#define MCU_MISC_CR_ROM_NT_ACCESS_MSK                           (((1U<<MCU_MISC_CR_ROM_NT_ACCESS_LEN)-1)<<MCU_MISC_CR_ROM_NT_ACCESS_POS)
#define MCU_MISC_CR_ROM_NT_ACCESS_UMSK                          (~(((1U<<MCU_MISC_CR_ROM_NT_ACCESS_LEN)-1)<<MCU_MISC_CR_ROM_NT_ACCESS_POS))
#define MCU_MISC_ST_ROM_NT_ACCESS                               MCU_MISC_ST_ROM_NT_ACCESS
#define MCU_MISC_ST_ROM_NT_ACCESS_POS                           (4U)
#define MCU_MISC_ST_ROM_NT_ACCESS_LEN                           (3U)
#define MCU_MISC_ST_ROM_NT_ACCESS_MSK                           (((1U<<MCU_MISC_ST_ROM_NT_ACCESS_LEN)-1)<<MCU_MISC_ST_ROM_NT_ACCESS_POS)
#define MCU_MISC_ST_ROM_NT_ACCESS_UMSK                          (~(((1U<<MCU_MISC_ST_ROM_NT_ACCESS_LEN)-1)<<MCU_MISC_ST_ROM_NT_ACCESS_POS))
#define MCU_MISC_CR_TIMEOUT_EN                                  MCU_MISC_CR_TIMEOUT_EN
#define MCU_MISC_CR_TIMEOUT_EN_POS                              (8U)
#define MCU_MISC_CR_TIMEOUT_EN_LEN                              (1U)
#define MCU_MISC_CR_TIMEOUT_EN_MSK                              (((1U<<MCU_MISC_CR_TIMEOUT_EN_LEN)-1)<<MCU_MISC_CR_TIMEOUT_EN_POS)
#define MCU_MISC_CR_TIMEOUT_EN_UMSK                             (~(((1U<<MCU_MISC_CR_TIMEOUT_EN_LEN)-1)<<MCU_MISC_CR_TIMEOUT_EN_POS))
#define MCU_MISC_CR_TIMEOUT_CLR                                 MCU_MISC_CR_TIMEOUT_CLR
#define MCU_MISC_CR_TIMEOUT_CLR_POS                             (9U)
#define MCU_MISC_CR_TIMEOUT_CLR_LEN                             (1U)
#define MCU_MISC_CR_TIMEOUT_CLR_MSK                             (((1U<<MCU_MISC_CR_TIMEOUT_CLR_LEN)-1)<<MCU_MISC_CR_TIMEOUT_CLR_POS)
#define MCU_MISC_CR_TIMEOUT_CLR_UMSK                            (~(((1U<<MCU_MISC_CR_TIMEOUT_CLR_LEN)-1)<<MCU_MISC_CR_TIMEOUT_CLR_POS))
#define MCU_MISC_ST_TIMEOUT_MCU_P0                              MCU_MISC_ST_TIMEOUT_MCU_P0
#define MCU_MISC_ST_TIMEOUT_MCU_P0_POS                          (12U)
#define MCU_MISC_ST_TIMEOUT_MCU_P0_LEN                          (1U)
#define MCU_MISC_ST_TIMEOUT_MCU_P0_MSK                          (((1U<<MCU_MISC_ST_TIMEOUT_MCU_P0_LEN)-1)<<MCU_MISC_ST_TIMEOUT_MCU_P0_POS)
#define MCU_MISC_ST_TIMEOUT_MCU_P0_UMSK                         (~(((1U<<MCU_MISC_ST_TIMEOUT_MCU_P0_LEN)-1)<<MCU_MISC_ST_TIMEOUT_MCU_P0_POS))
#define MCU_MISC_ST_TIMEOUT_MCU_P1                              MCU_MISC_ST_TIMEOUT_MCU_P1
#define MCU_MISC_ST_TIMEOUT_MCU_P1_POS                          (13U)
#define MCU_MISC_ST_TIMEOUT_MCU_P1_LEN                          (1U)
#define MCU_MISC_ST_TIMEOUT_MCU_P1_MSK                          (((1U<<MCU_MISC_ST_TIMEOUT_MCU_P1_LEN)-1)<<MCU_MISC_ST_TIMEOUT_MCU_P1_POS)
#define MCU_MISC_ST_TIMEOUT_MCU_P1_UMSK                         (~(((1U<<MCU_MISC_ST_TIMEOUT_MCU_P1_LEN)-1)<<MCU_MISC_ST_TIMEOUT_MCU_P1_POS))
#define MCU_MISC_CR_ARB_MODE                                    MCU_MISC_CR_ARB_MODE
#define MCU_MISC_CR_ARB_MODE_POS                                (16U)
#define MCU_MISC_CR_ARB_MODE_LEN                                (1U)
#define MCU_MISC_CR_ARB_MODE_MSK                                (((1U<<MCU_MISC_CR_ARB_MODE_LEN)-1)<<MCU_MISC_CR_ARB_MODE_POS)
#define MCU_MISC_CR_ARB_MODE_UMSK                               (~(((1U<<MCU_MISC_CR_ARB_MODE_LEN)-1)<<MCU_MISC_CR_ARB_MODE_POS))

/* 0x44 : mcu_bus_cfg1 */
#define MCU_MISC_MCU_BUS_CFG1_OFFSET                            (0x44)
#define MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU                     MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU
#define MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_POS                 (0U)
#define MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_LEN                 (1U)
#define MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_MSK                 (((1U<<MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_LEN)-1)<<MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_POS)
#define MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_UMSK                (~(((1U<<MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_LEN)-1)<<MCU_MISC_CR_HS2F_RCMD_BP_EN_TOP2MCU_POS))
#define MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP                     MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP
#define MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_POS                 (1U)
#define MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_LEN                 (1U)
#define MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_MSK                 (((1U<<MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_LEN)-1)<<MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_POS)
#define MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_UMSK                (~(((1U<<MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_LEN)-1)<<MCU_MISC_CR_HF2S_RDAT_BP_EN_MCU2TOP_POS))
#define MCU_MISC_CR_HS2F_WTHRE_TOP2MCU                          MCU_MISC_CR_HS2F_WTHRE_TOP2MCU
#define MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_POS                      (4U)
#define MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_LEN                      (2U)
#define MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_MSK                      (((1U<<MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_LEN)-1)<<MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_POS)
#define MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_UMSK                     (~(((1U<<MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_LEN)-1)<<MCU_MISC_CR_HS2F_WTHRE_TOP2MCU_POS))
#define MCU_MISC_CR_HASLI_WTHRE_MCU2SF                          MCU_MISC_CR_HASLI_WTHRE_MCU2SF
#define MCU_MISC_CR_HASLI_WTHRE_MCU2SF_POS                      (6U)
#define MCU_MISC_CR_HASLI_WTHRE_MCU2SF_LEN                      (2U)
#define MCU_MISC_CR_HASLI_WTHRE_MCU2SF_MSK                      (((1U<<MCU_MISC_CR_HASLI_WTHRE_MCU2SF_LEN)-1)<<MCU_MISC_CR_HASLI_WTHRE_MCU2SF_POS)
#define MCU_MISC_CR_HASLI_WTHRE_MCU2SF_UMSK                     (~(((1U<<MCU_MISC_CR_HASLI_WTHRE_MCU2SF_LEN)-1)<<MCU_MISC_CR_HASLI_WTHRE_MCU2SF_POS))
#define MCU_MISC_CR_PROT_REQ_SF                                 MCU_MISC_CR_PROT_REQ_SF
#define MCU_MISC_CR_PROT_REQ_SF_POS                             (16U)
#define MCU_MISC_CR_PROT_REQ_SF_LEN                             (1U)
#define MCU_MISC_CR_PROT_REQ_SF_MSK                             (((1U<<MCU_MISC_CR_PROT_REQ_SF_LEN)-1)<<MCU_MISC_CR_PROT_REQ_SF_POS)
#define MCU_MISC_CR_PROT_REQ_SF_UMSK                            (~(((1U<<MCU_MISC_CR_PROT_REQ_SF_LEN)-1)<<MCU_MISC_CR_PROT_REQ_SF_POS))
#define MCU_MISC_ST_PROT_RDY_SF                                 MCU_MISC_ST_PROT_RDY_SF
#define MCU_MISC_ST_PROT_RDY_SF_POS                             (17U)
#define MCU_MISC_ST_PROT_RDY_SF_LEN                             (1U)
#define MCU_MISC_ST_PROT_RDY_SF_MSK                             (((1U<<MCU_MISC_ST_PROT_RDY_SF_LEN)-1)<<MCU_MISC_ST_PROT_RDY_SF_POS)
#define MCU_MISC_ST_PROT_RDY_SF_UMSK                            (~(((1U<<MCU_MISC_ST_PROT_RDY_SF_LEN)-1)<<MCU_MISC_ST_PROT_RDY_SF_POS))

/* 0x48 : mcu_bus_cfg2 */
#define MCU_MISC_MCU_BUS_CFG2_OFFSET                            (0x48)
#define MCU_MISC_CR_ERR_EN_INFRA_MCU                            MCU_MISC_CR_ERR_EN_INFRA_MCU
#define MCU_MISC_CR_ERR_EN_INFRA_MCU_POS                        (0U)
#define MCU_MISC_CR_ERR_EN_INFRA_MCU_LEN                        (4U)
#define MCU_MISC_CR_ERR_EN_INFRA_MCU_MSK                        (((1U<<MCU_MISC_CR_ERR_EN_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_EN_INFRA_MCU_POS)
#define MCU_MISC_CR_ERR_EN_INFRA_MCU_UMSK                       (~(((1U<<MCU_MISC_CR_ERR_EN_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_EN_INFRA_MCU_POS))
#define MCU_MISC_CR_ERR_CLR_INFRA_MCU                           MCU_MISC_CR_ERR_CLR_INFRA_MCU
#define MCU_MISC_CR_ERR_CLR_INFRA_MCU_POS                       (8U)
#define MCU_MISC_CR_ERR_CLR_INFRA_MCU_LEN                       (1U)
#define MCU_MISC_CR_ERR_CLR_INFRA_MCU_MSK                       (((1U<<MCU_MISC_CR_ERR_CLR_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_CLR_INFRA_MCU_POS)
#define MCU_MISC_CR_ERR_CLR_INFRA_MCU_UMSK                      (~(((1U<<MCU_MISC_CR_ERR_CLR_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_CLR_INFRA_MCU_POS))
#define MCU_MISC_CR_ERR_LAST_INFRA_MCU                          MCU_MISC_CR_ERR_LAST_INFRA_MCU
#define MCU_MISC_CR_ERR_LAST_INFRA_MCU_POS                      (9U)
#define MCU_MISC_CR_ERR_LAST_INFRA_MCU_LEN                      (1U)
#define MCU_MISC_CR_ERR_LAST_INFRA_MCU_MSK                      (((1U<<MCU_MISC_CR_ERR_LAST_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_LAST_INFRA_MCU_POS)
#define MCU_MISC_CR_ERR_LAST_INFRA_MCU_UMSK                     (~(((1U<<MCU_MISC_CR_ERR_LAST_INFRA_MCU_LEN)-1)<<MCU_MISC_CR_ERR_LAST_INFRA_MCU_POS))
#define MCU_MISC_ST_ERR_INFRA_MCU                               MCU_MISC_ST_ERR_INFRA_MCU
#define MCU_MISC_ST_ERR_INFRA_MCU_POS                           (16U)
#define MCU_MISC_ST_ERR_INFRA_MCU_LEN                           (1U)
#define MCU_MISC_ST_ERR_INFRA_MCU_MSK                           (((1U<<MCU_MISC_ST_ERR_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_INFRA_MCU_POS)
#define MCU_MISC_ST_ERR_INFRA_MCU_UMSK                          (~(((1U<<MCU_MISC_ST_ERR_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_INFRA_MCU_POS))
#define MCU_MISC_ST_ERR_WRITE_INFRA_MCU                         MCU_MISC_ST_ERR_WRITE_INFRA_MCU
#define MCU_MISC_ST_ERR_WRITE_INFRA_MCU_POS                     (17U)
#define MCU_MISC_ST_ERR_WRITE_INFRA_MCU_LEN                     (1U)
#define MCU_MISC_ST_ERR_WRITE_INFRA_MCU_MSK                     (((1U<<MCU_MISC_ST_ERR_WRITE_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_WRITE_INFRA_MCU_POS)
#define MCU_MISC_ST_ERR_WRITE_INFRA_MCU_UMSK                    (~(((1U<<MCU_MISC_ST_ERR_WRITE_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_WRITE_INFRA_MCU_POS))
#define MCU_MISC_ST_ERR_SRC_INFRA_MCU                           MCU_MISC_ST_ERR_SRC_INFRA_MCU
#define MCU_MISC_ST_ERR_SRC_INFRA_MCU_POS                       (20U)
#define MCU_MISC_ST_ERR_SRC_INFRA_MCU_LEN                       (4U)
#define MCU_MISC_ST_ERR_SRC_INFRA_MCU_MSK                       (((1U<<MCU_MISC_ST_ERR_SRC_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_SRC_INFRA_MCU_POS)
#define MCU_MISC_ST_ERR_SRC_INFRA_MCU_UMSK                      (~(((1U<<MCU_MISC_ST_ERR_SRC_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_SRC_INFRA_MCU_POS))

/* 0x4C : mcu_bus_cfg3 */
#define MCU_MISC_MCU_BUS_CFG3_OFFSET                            (0x4C)
#define MCU_MISC_ST_ERR_ADDR_INFRA_MCU                          MCU_MISC_ST_ERR_ADDR_INFRA_MCU
#define MCU_MISC_ST_ERR_ADDR_INFRA_MCU_POS                      (0U)
#define MCU_MISC_ST_ERR_ADDR_INFRA_MCU_LEN                      (32U)
#define MCU_MISC_ST_ERR_ADDR_INFRA_MCU_MSK                      (((1U<<MCU_MISC_ST_ERR_ADDR_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_ADDR_INFRA_MCU_POS)
#define MCU_MISC_ST_ERR_ADDR_INFRA_MCU_UMSK                     (~(((1U<<MCU_MISC_ST_ERR_ADDR_INFRA_MCU_LEN)-1)<<MCU_MISC_ST_ERR_ADDR_INFRA_MCU_POS))

/* 0x50 : mcu_bus_cfg4 */
#define MCU_MISC_MCU_BUS_CFG4_OFFSET                            (0x50)

/* 0x80 : cpu_clk_gate */
#define MCU_MISC_CPU_CLK_GATE_OFFSET                            (0x80)
#define MCU_MISC_CR_MCU_CLK_EN                                  MCU_MISC_CR_MCU_CLK_EN
#define MCU_MISC_CR_MCU_CLK_EN_POS                              (8U)
#define MCU_MISC_CR_MCU_CLK_EN_LEN                              (1U)
#define MCU_MISC_CR_MCU_CLK_EN_MSK                              (((1U<<MCU_MISC_CR_MCU_CLK_EN_LEN)-1)<<MCU_MISC_CR_MCU_CLK_EN_POS)
#define MCU_MISC_CR_MCU_CLK_EN_UMSK                             (~(((1U<<MCU_MISC_CR_MCU_CLK_EN_LEN)-1)<<MCU_MISC_CR_MCU_CLK_EN_POS))
#define MCU_MISC_CR_MCU_PERI_CLK_EN                             MCU_MISC_CR_MCU_PERI_CLK_EN
#define MCU_MISC_CR_MCU_PERI_CLK_EN_POS                         (9U)
#define MCU_MISC_CR_MCU_PERI_CLK_EN_LEN                         (1U)
#define MCU_MISC_CR_MCU_PERI_CLK_EN_MSK                         (((1U<<MCU_MISC_CR_MCU_PERI_CLK_EN_LEN)-1)<<MCU_MISC_CR_MCU_PERI_CLK_EN_POS)
#define MCU_MISC_CR_MCU_PERI_CLK_EN_UMSK                        (~(((1U<<MCU_MISC_CR_MCU_PERI_CLK_EN_LEN)-1)<<MCU_MISC_CR_MCU_PERI_CLK_EN_POS))

/* 0x90 : mcu_csr0 */
#define MCU_MISC_MCU_CSR0_OFFSET                                (0x90)
#define MCU_MISC_ST_MCU_HALTED                                  MCU_MISC_ST_MCU_HALTED
#define MCU_MISC_ST_MCU_HALTED_POS                              (0U)
#define MCU_MISC_ST_MCU_HALTED_LEN                              (1U)
#define MCU_MISC_ST_MCU_HALTED_MSK                              (((1U<<MCU_MISC_ST_MCU_HALTED_LEN)-1)<<MCU_MISC_ST_MCU_HALTED_POS)
#define MCU_MISC_ST_MCU_HALTED_UMSK                             (~(((1U<<MCU_MISC_ST_MCU_HALTED_LEN)-1)<<MCU_MISC_ST_MCU_HALTED_POS))
#define MCU_MISC_ST_MCU_DBG_STOPTIME                            MCU_MISC_ST_MCU_DBG_STOPTIME
#define MCU_MISC_ST_MCU_DBG_STOPTIME_POS                        (1U)
#define MCU_MISC_ST_MCU_DBG_STOPTIME_LEN                        (1U)
#define MCU_MISC_ST_MCU_DBG_STOPTIME_MSK                        (((1U<<MCU_MISC_ST_MCU_DBG_STOPTIME_LEN)-1)<<MCU_MISC_ST_MCU_DBG_STOPTIME_POS)
#define MCU_MISC_ST_MCU_DBG_STOPTIME_UMSK                       (~(((1U<<MCU_MISC_ST_MCU_DBG_STOPTIME_LEN)-1)<<MCU_MISC_ST_MCU_DBG_STOPTIME_POS))
#define MCU_MISC_ST_MCU_SLEEP                                   MCU_MISC_ST_MCU_SLEEP
#define MCU_MISC_ST_MCU_SLEEP_POS                               (2U)
#define MCU_MISC_ST_MCU_SLEEP_LEN                               (1U)
#define MCU_MISC_ST_MCU_SLEEP_MSK                               (((1U<<MCU_MISC_ST_MCU_SLEEP_LEN)-1)<<MCU_MISC_ST_MCU_SLEEP_POS)
#define MCU_MISC_ST_MCU_SLEEP_UMSK                              (~(((1U<<MCU_MISC_ST_MCU_SLEEP_LEN)-1)<<MCU_MISC_ST_MCU_SLEEP_POS))

/* 0x200 : cpu_mbist */
#define MCU_MISC_CPU_MBIST_OFFSET                               (0x200)
#define MCU_MISC_CPU_MBIST_MODE                                 MCU_MISC_CPU_MBIST_MODE
#define MCU_MISC_CPU_MBIST_MODE_POS                             (0U)
#define MCU_MISC_CPU_MBIST_MODE_LEN                             (1U)
#define MCU_MISC_CPU_MBIST_MODE_MSK                             (((1U<<MCU_MISC_CPU_MBIST_MODE_LEN)-1)<<MCU_MISC_CPU_MBIST_MODE_POS)
#define MCU_MISC_CPU_MBIST_MODE_UMSK                            (~(((1U<<MCU_MISC_CPU_MBIST_MODE_LEN)-1)<<MCU_MISC_CPU_MBIST_MODE_POS))
#define MCU_MISC_IROM_MBIST_MODE                                MCU_MISC_IROM_MBIST_MODE
#define MCU_MISC_IROM_MBIST_MODE_POS                            (1U)
#define MCU_MISC_IROM_MBIST_MODE_LEN                            (1U)
#define MCU_MISC_IROM_MBIST_MODE_MSK                            (((1U<<MCU_MISC_IROM_MBIST_MODE_LEN)-1)<<MCU_MISC_IROM_MBIST_MODE_POS)
#define MCU_MISC_IROM_MBIST_MODE_UMSK                           (~(((1U<<MCU_MISC_IROM_MBIST_MODE_LEN)-1)<<MCU_MISC_IROM_MBIST_MODE_POS))
#define MCU_MISC_IROM_PGEN                                      MCU_MISC_IROM_PGEN
#define MCU_MISC_IROM_PGEN_POS                                  (4U)
#define MCU_MISC_IROM_PGEN_LEN                                  (1U)
#define MCU_MISC_IROM_PGEN_MSK                                  (((1U<<MCU_MISC_IROM_PGEN_LEN)-1)<<MCU_MISC_IROM_PGEN_POS)
#define MCU_MISC_IROM_PGEN_UMSK                                 (~(((1U<<MCU_MISC_IROM_PGEN_LEN)-1)<<MCU_MISC_IROM_PGEN_POS))
#define MCU_MISC_REG_CPU_MBIST_RST_N                            MCU_MISC_REG_CPU_MBIST_RST_N
#define MCU_MISC_REG_CPU_MBIST_RST_N_POS                        (8U)
#define MCU_MISC_REG_CPU_MBIST_RST_N_LEN                        (1U)
#define MCU_MISC_REG_CPU_MBIST_RST_N_MSK                        (((1U<<MCU_MISC_REG_CPU_MBIST_RST_N_LEN)-1)<<MCU_MISC_REG_CPU_MBIST_RST_N_POS)
#define MCU_MISC_REG_CPU_MBIST_RST_N_UMSK                       (~(((1U<<MCU_MISC_REG_CPU_MBIST_RST_N_LEN)-1)<<MCU_MISC_REG_CPU_MBIST_RST_N_POS))
#define MCU_MISC_CR_MCU_CACHE_RET1                              MCU_MISC_CR_MCU_CACHE_RET1
#define MCU_MISC_CR_MCU_CACHE_RET1_POS                          (28U)
#define MCU_MISC_CR_MCU_CACHE_RET1_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_RET1_MSK                          (((1U<<MCU_MISC_CR_MCU_CACHE_RET1_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET1_POS)
#define MCU_MISC_CR_MCU_CACHE_RET1_UMSK                         (~(((1U<<MCU_MISC_CR_MCU_CACHE_RET1_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET1_POS))
#define MCU_MISC_CR_MCU_CACHE_RET2                              MCU_MISC_CR_MCU_CACHE_RET2
#define MCU_MISC_CR_MCU_CACHE_RET2_POS                          (29U)
#define MCU_MISC_CR_MCU_CACHE_RET2_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_RET2_MSK                          (((1U<<MCU_MISC_CR_MCU_CACHE_RET2_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET2_POS)
#define MCU_MISC_CR_MCU_CACHE_RET2_UMSK                         (~(((1U<<MCU_MISC_CR_MCU_CACHE_RET2_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_RET2_POS))
#define MCU_MISC_CR_MCU_CACHE_PGEN                              MCU_MISC_CR_MCU_CACHE_PGEN
#define MCU_MISC_CR_MCU_CACHE_PGEN_POS                          (30U)
#define MCU_MISC_CR_MCU_CACHE_PGEN_LEN                          (1U)
#define MCU_MISC_CR_MCU_CACHE_PGEN_MSK                          (((1U<<MCU_MISC_CR_MCU_CACHE_PGEN_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_PGEN_POS)
#define MCU_MISC_CR_MCU_CACHE_PGEN_UMSK                         (~(((1U<<MCU_MISC_CR_MCU_CACHE_PGEN_LEN)-1)<<MCU_MISC_CR_MCU_CACHE_PGEN_POS))

/* 0x204 : cpu_mbist2 */
#define MCU_MISC_CPU_MBIST2_OFFSET                              (0x204)
#define MCU_MISC_CR_RAM_BUS_MCU                                 MCU_MISC_CR_RAM_BUS_MCU
#define MCU_MISC_CR_RAM_BUS_MCU_POS                             (0U)
#define MCU_MISC_CR_RAM_BUS_MCU_LEN                             (24U)
#define MCU_MISC_CR_RAM_BUS_MCU_MSK                             (((1U<<MCU_MISC_CR_RAM_BUS_MCU_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_POS)
#define MCU_MISC_CR_RAM_BUS_MCU_UMSK                            (~(((1U<<MCU_MISC_CR_RAM_BUS_MCU_LEN)-1)<<MCU_MISC_CR_RAM_BUS_MCU_POS))
#define MCU_MISC_CR_4S_RAM_CLK_EN                               MCU_MISC_CR_4S_RAM_CLK_EN
#define MCU_MISC_CR_4S_RAM_CLK_EN_POS                           (31U)
#define MCU_MISC_CR_4S_RAM_CLK_EN_LEN                           (1U)
#define MCU_MISC_CR_4S_RAM_CLK_EN_MSK                           (((1U<<MCU_MISC_CR_4S_RAM_CLK_EN_LEN)-1)<<MCU_MISC_CR_4S_RAM_CLK_EN_POS)
#define MCU_MISC_CR_4S_RAM_CLK_EN_UMSK                          (~(((1U<<MCU_MISC_CR_4S_RAM_CLK_EN_LEN)-1)<<MCU_MISC_CR_4S_RAM_CLK_EN_POS))


struct  mcu_misc_reg {
    /* 0x0 : L1C_CFG0 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } L1C_CFG0;

    /* 0x4 : hit_cnt_lsb */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } hit_cnt_lsb;

    /* 0x8 : hit_cnt_msb */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } hit_cnt_msb;

    /* 0xC : miss_cnt */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } miss_cnt;

    /* 0x10 : l1c_misc */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } l1c_misc;

    /* 0x14  reserved */
    uint8_t RESERVED0x14[44];

    /* 0x40 : mcu_bus_cfg0 */
    union {
        struct {
            uint32_t cr_rom_nt_access               :  3; /* [ 2: 0],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t st_rom_nt_access               :  3; /* [ 6: 4],          r,        0x1 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t cr_timeout_en                  :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_timeout_clr                 :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_11                 :  2; /* [11:10],       rsvd,        0x0 */
            uint32_t st_timeout_mcu_p0              :  1; /* [   12],          r,        0x0 */
            uint32_t st_timeout_mcu_p1              :  1; /* [   13],          r,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_arb_mode                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg0;

    /* 0x44 : mcu_bus_cfg1 */
    union {
        struct {
            uint32_t cr_hs2f_rcmd_bp_en_top2mcu     :  1; /* [    0],        r/w,        0x1 */
            uint32_t cr_hf2s_rdat_bp_en_mcu2top     :  1; /* [    1],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t cr_hs2f_wthre_top2mcu          :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t cr_hasli_wthre_mcu2sf          :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
            uint32_t cr_prot_req_sf                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t st_prot_rdy_sf                 :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg1;

    /* 0x48 : mcu_bus_cfg2 */
    union {
        struct {
            uint32_t cr_err_en_infra_mcu            :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t cr_err_clr_infra_mcu           :  1; /* [    8],        r/w,        0x0 */
            uint32_t cr_err_last_infra_mcu          :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t st_err_infra_mcu               :  1; /* [   16],          r,        0x0 */
            uint32_t st_err_write_infra_mcu         :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t st_err_src_infra_mcu           :  4; /* [23:20],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg2;

    /* 0x4C : mcu_bus_cfg3 */
    union {
        struct {
            uint32_t st_err_addr_infra_mcu          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg3;

    /* 0x50 : mcu_bus_cfg4 */
    union {
        struct {
            uint32_t reserved_0_31                  : 32; /* [31: 0],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_bus_cfg4;

    /* 0x54  reserved */
    uint8_t RESERVED0x54[44];

    /* 0x80 : cpu_clk_gate */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t cr_mcu_clk_en                  :  1; /* [    8],        r/w,        0x1 */
            uint32_t cr_mcu_peri_clk_en             :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cpu_clk_gate;

    /* 0x84  reserved */
    uint8_t RESERVED0x84[12];

    /* 0x90 : mcu_csr0 */
    union {
        struct {
            uint32_t st_mcu_halted                  :  1; /* [    0],          r,        0x0 */
            uint32_t st_mcu_dbg_stoptime            :  1; /* [    1],          r,        0x0 */
            uint32_t st_mcu_sleep                   :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mcu_csr0;

    /* 0x94  reserved */
    uint8_t RESERVED0x94[364];

    /* 0x200 : cpu_mbist */
    union {
        struct {
            uint32_t cpu_mbist_mode                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t irom_mbist_mode                :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t irom_pgen                      :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
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
    union {
        struct {
            uint32_t cr_ram_bus_mcu                 : 24; /* [23: 0],        r/w,   0x60c2da */
            uint32_t reserved_24_30                 :  7; /* [30:24],       rsvd,        0x0 */
            uint32_t cr_4s_ram_clk_en               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cpu_mbist2;

};

typedef volatile struct mcu_misc_reg mcu_misc_reg_t;


#endif  /* __MCU_MISC_REG_H__ */
