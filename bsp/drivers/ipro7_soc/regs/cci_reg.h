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

#ifndef  __CCI_REG_H__
#define  __CCI_REG_H__

#include "ipro7.h"

/* 0x0 : cci_cfg */
#define CCI_CFG_OFFSET                                          (0x0)
#define CCI_EN                                                  CCI_EN
#define CCI_EN_POS                                              (0U)
#define CCI_EN_LEN                                              (1U)
#define CCI_EN_MSK                                              (((1U<<CCI_EN_LEN)-1)<<CCI_EN_POS)
#define CCI_EN_UMSK                                             (~(((1U<<CCI_EN_LEN)-1)<<CCI_EN_POS))
#define CCI_SLV_SEL_CCI2                                        CCI_SLV_SEL_CCI2
#define CCI_SLV_SEL_CCI2_POS                                    (1U)
#define CCI_SLV_SEL_CCI2_LEN                                    (1U)
#define CCI_SLV_SEL_CCI2_MSK                                    (((1U<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS)
#define CCI_SLV_SEL_CCI2_UMSK                                   (~(((1U<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS))
#define CCI_MAS_SEL_CCI2                                        CCI_MAS_SEL_CCI2
#define CCI_MAS_SEL_CCI2_POS                                    (2U)
#define CCI_MAS_SEL_CCI2_LEN                                    (1U)
#define CCI_MAS_SEL_CCI2_MSK                                    (((1U<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS)
#define CCI_MAS_SEL_CCI2_UMSK                                   (~(((1U<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS))
#define CCI_MAS_HW_MODE                                         CCI_MAS_HW_MODE
#define CCI_MAS_HW_MODE_POS                                     (3U)
#define CCI_MAS_HW_MODE_LEN                                     (1U)
#define CCI_MAS_HW_MODE_MSK                                     (((1U<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS)
#define CCI_MAS_HW_MODE_UMSK                                    (~(((1U<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS))
#define CCI_REG_M_CCI_SCLK_EN                                   CCI_REG_M_CCI_SCLK_EN
#define CCI_REG_M_CCI_SCLK_EN_POS                               (4U)
#define CCI_REG_M_CCI_SCLK_EN_LEN                               (1U)
#define CCI_REG_M_CCI_SCLK_EN_MSK                               (((1U<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS)
#define CCI_REG_M_CCI_SCLK_EN_UMSK                              (~(((1U<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS))
#define CCI_REG_DIV_M_CCI_SCLK                                  CCI_REG_DIV_M_CCI_SCLK
#define CCI_REG_DIV_M_CCI_SCLK_POS                              (5U)
#define CCI_REG_DIV_M_CCI_SCLK_LEN                              (2U)
#define CCI_REG_DIV_M_CCI_SCLK_MSK                              (((1U<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS)
#define CCI_REG_DIV_M_CCI_SCLK_UMSK                             (~(((1U<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS))
#define CCI_CFG_CCI1_PRE_READ                                   CCI_CFG_CCI1_PRE_READ
#define CCI_CFG_CCI1_PRE_READ_POS                               (7U)
#define CCI_CFG_CCI1_PRE_READ_LEN                               (1U)
#define CCI_CFG_CCI1_PRE_READ_MSK                               (((1U<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS)
#define CCI_CFG_CCI1_PRE_READ_UMSK                              (~(((1U<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS))
#define CCI_REG_SCCI_CLK_INV                                    CCI_REG_SCCI_CLK_INV
#define CCI_REG_SCCI_CLK_INV_POS                                (8U)
#define CCI_REG_SCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_SCCI_CLK_INV_MSK                                (((1U<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS)
#define CCI_REG_SCCI_CLK_INV_UMSK                               (~(((1U<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS))
#define CCI_REG_MCCI_CLK_INV                                    CCI_REG_MCCI_CLK_INV
#define CCI_REG_MCCI_CLK_INV_POS                                (9U)
#define CCI_REG_MCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_MCCI_CLK_INV_MSK                                (((1U<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS)
#define CCI_REG_MCCI_CLK_INV_UMSK                               (~(((1U<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS))

/* 0x4 : cci_addr */
#define CCI_ADDR_OFFSET                                         (0x4)
#define CCI_APB_CCI_ADDR                                        CCI_APB_CCI_ADDR
#define CCI_APB_CCI_ADDR_POS                                    (0U)
#define CCI_APB_CCI_ADDR_LEN                                    (32U)
#define CCI_APB_CCI_ADDR_MSK                                    (((1U<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS)
#define CCI_APB_CCI_ADDR_UMSK                                   (~(((1U<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS))

/* 0x8 : cci_wdata */
#define CCI_WDATA_OFFSET                                        (0x8)
#define CCI_APB_CCI_WDATA                                       CCI_APB_CCI_WDATA
#define CCI_APB_CCI_WDATA_POS                                   (0U)
#define CCI_APB_CCI_WDATA_LEN                                   (32U)
#define CCI_APB_CCI_WDATA_MSK                                   (((1U<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS)
#define CCI_APB_CCI_WDATA_UMSK                                  (~(((1U<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS))

/* 0xC : cci_rdata */
#define CCI_RDATA_OFFSET                                        (0xC)
#define CCI_APB_CCI_RDATA                                       CCI_APB_CCI_RDATA
#define CCI_APB_CCI_RDATA_POS                                   (0U)
#define CCI_APB_CCI_RDATA_LEN                                   (32U)
#define CCI_APB_CCI_RDATA_MSK                                   (((1U<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS)
#define CCI_APB_CCI_RDATA_UMSK                                  (~(((1U<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS))

/* 0x10 : cci_ctl */
#define CCI_CTL_OFFSET                                          (0x10)
#define CCI_WRITE_FLAG                                          CCI_WRITE_FLAG
#define CCI_WRITE_FLAG_POS                                      (0U)
#define CCI_WRITE_FLAG_LEN                                      (1U)
#define CCI_WRITE_FLAG_MSK                                      (((1U<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS)
#define CCI_WRITE_FLAG_UMSK                                     (~(((1U<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS))
#define CCI_READ_FLAG                                           CCI_READ_FLAG
#define CCI_READ_FLAG_POS                                       (1U)
#define CCI_READ_FLAG_LEN                                       (1U)
#define CCI_READ_FLAG_MSK                                       (((1U<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS)
#define CCI_READ_FLAG_UMSK                                      (~(((1U<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS))
#define CCI_AHB_STATE                                           CCI_AHB_STATE
#define CCI_AHB_STATE_POS                                       (2U)
#define CCI_AHB_STATE_LEN                                       (2U)
#define CCI_AHB_STATE_MSK                                       (((1U<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS)
#define CCI_AHB_STATE_UMSK                                      (~(((1U<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS))

/* 0x200 : dll */
#define CCI_DLL_OFFSET                                          (0x200)
#define CCI_TEN_DLL                                             CCI_TEN_DLL
#define CCI_TEN_DLL_POS                                         (2U)
#define CCI_TEN_DLL_LEN                                         (1U)
#define CCI_TEN_DLL_MSK                                         (((1U<<CCI_TEN_DLL_LEN)-1)<<CCI_TEN_DLL_POS)
#define CCI_TEN_DLL_UMSK                                        (~(((1U<<CCI_TEN_DLL_LEN)-1)<<CCI_TEN_DLL_POS))
#define CCI_DLL_VCTRL_SEL                                       CCI_DLL_VCTRL_SEL
#define CCI_DLL_VCTRL_SEL_POS                                   (8U)
#define CCI_DLL_VCTRL_SEL_LEN                                   (3U)
#define CCI_DLL_VCTRL_SEL_MSK                                   (((1U<<CCI_DLL_VCTRL_SEL_LEN)-1)<<CCI_DLL_VCTRL_SEL_POS)
#define CCI_DLL_VCTRL_SEL_UMSK                                  (~(((1U<<CCI_DLL_VCTRL_SEL_LEN)-1)<<CCI_DLL_VCTRL_SEL_POS))
#define CCI_DLL_PRECHG_SEL                                      CCI_DLL_PRECHG_SEL
#define CCI_DLL_PRECHG_SEL_POS                                  (12U)
#define CCI_DLL_PRECHG_SEL_LEN                                  (1U)
#define CCI_DLL_PRECHG_SEL_MSK                                  (((1U<<CCI_DLL_PRECHG_SEL_LEN)-1)<<CCI_DLL_PRECHG_SEL_POS)
#define CCI_DLL_PRECHG_SEL_UMSK                                 (~(((1U<<CCI_DLL_PRECHG_SEL_LEN)-1)<<CCI_DLL_PRECHG_SEL_POS))
#define CCI_DLL_PRECHG_REG                                      CCI_DLL_PRECHG_REG
#define CCI_DLL_PRECHG_REG_POS                                  (13U)
#define CCI_DLL_PRECHG_REG_LEN                                  (1U)
#define CCI_DLL_PRECHG_REG_MSK                                  (((1U<<CCI_DLL_PRECHG_REG_LEN)-1)<<CCI_DLL_PRECHG_REG_POS)
#define CCI_DLL_PRECHG_REG_UMSK                                 (~(((1U<<CCI_DLL_PRECHG_REG_LEN)-1)<<CCI_DLL_PRECHG_REG_POS))
#define CCI_DLL_PRECHG_EN                                       CCI_DLL_PRECHG_EN
#define CCI_DLL_PRECHG_EN_POS                                   (14U)
#define CCI_DLL_PRECHG_EN_LEN                                   (1U)
#define CCI_DLL_PRECHG_EN_MSK                                   (((1U<<CCI_DLL_PRECHG_EN_LEN)-1)<<CCI_DLL_PRECHG_EN_POS)
#define CCI_DLL_PRECHG_EN_UMSK                                  (~(((1U<<CCI_DLL_PRECHG_EN_LEN)-1)<<CCI_DLL_PRECHG_EN_POS))
#define CCI_DLL_VCTRL_FORCE_EN                                  CCI_DLL_VCTRL_FORCE_EN
#define CCI_DLL_VCTRL_FORCE_EN_POS                              (15U)
#define CCI_DLL_VCTRL_FORCE_EN_LEN                              (1U)
#define CCI_DLL_VCTRL_FORCE_EN_MSK                              (((1U<<CCI_DLL_VCTRL_FORCE_EN_LEN)-1)<<CCI_DLL_VCTRL_FORCE_EN_POS)
#define CCI_DLL_VCTRL_FORCE_EN_UMSK                             (~(((1U<<CCI_DLL_VCTRL_FORCE_EN_LEN)-1)<<CCI_DLL_VCTRL_FORCE_EN_POS))
#define CCI_DLL_POSTDIV                                         CCI_DLL_POSTDIV
#define CCI_DLL_POSTDIV_POS                                     (16U)
#define CCI_DLL_POSTDIV_LEN                                     (6U)
#define CCI_DLL_POSTDIV_MSK                                     (((1U<<CCI_DLL_POSTDIV_LEN)-1)<<CCI_DLL_POSTDIV_POS)
#define CCI_DLL_POSTDIV_UMSK                                    (~(((1U<<CCI_DLL_POSTDIV_LEN)-1)<<CCI_DLL_POSTDIV_POS))
#define CCI_DLL_DELAY_SEL                                       CCI_DLL_DELAY_SEL
#define CCI_DLL_DELAY_SEL_POS                                   (24U)
#define CCI_DLL_DELAY_SEL_LEN                                   (2U)
#define CCI_DLL_DELAY_SEL_MSK                                   (((1U<<CCI_DLL_DELAY_SEL_LEN)-1)<<CCI_DLL_DELAY_SEL_POS)
#define CCI_DLL_DELAY_SEL_UMSK                                  (~(((1U<<CCI_DLL_DELAY_SEL_LEN)-1)<<CCI_DLL_DELAY_SEL_POS))
#define CCI_DLL_CP_OP_EN                                        CCI_DLL_CP_OP_EN
#define CCI_DLL_CP_OP_EN_POS                                    (26U)
#define CCI_DLL_CP_OP_EN_LEN                                    (1U)
#define CCI_DLL_CP_OP_EN_MSK                                    (((1U<<CCI_DLL_CP_OP_EN_LEN)-1)<<CCI_DLL_CP_OP_EN_POS)
#define CCI_DLL_CP_OP_EN_UMSK                                   (~(((1U<<CCI_DLL_CP_OP_EN_LEN)-1)<<CCI_DLL_CP_OP_EN_POS))
#define CCI_DLL_CP_HIZ                                          CCI_DLL_CP_HIZ
#define CCI_DLL_CP_HIZ_POS                                      (27U)
#define CCI_DLL_CP_HIZ_LEN                                      (1U)
#define CCI_DLL_CP_HIZ_MSK                                      (((1U<<CCI_DLL_CP_HIZ_LEN)-1)<<CCI_DLL_CP_HIZ_POS)
#define CCI_DLL_CP_HIZ_UMSK                                     (~(((1U<<CCI_DLL_CP_HIZ_LEN)-1)<<CCI_DLL_CP_HIZ_POS))
#define CCI_DLL_REFCLK_SEL                                      CCI_DLL_REFCLK_SEL
#define CCI_DLL_REFCLK_SEL_POS                                  (28U)
#define CCI_DLL_REFCLK_SEL_LEN                                  (1U)
#define CCI_DLL_REFCLK_SEL_MSK                                  (((1U<<CCI_DLL_REFCLK_SEL_LEN)-1)<<CCI_DLL_REFCLK_SEL_POS)
#define CCI_DLL_REFCLK_SEL_UMSK                                 (~(((1U<<CCI_DLL_REFCLK_SEL_LEN)-1)<<CCI_DLL_REFCLK_SEL_POS))
#define CCI_DLL_RESET                                           CCI_DLL_RESET
#define CCI_DLL_RESET_POS                                       (29U)
#define CCI_DLL_RESET_LEN                                       (1U)
#define CCI_DLL_RESET_MSK                                       (((1U<<CCI_DLL_RESET_LEN)-1)<<CCI_DLL_RESET_POS)
#define CCI_DLL_RESET_UMSK                                      (~(((1U<<CCI_DLL_RESET_LEN)-1)<<CCI_DLL_RESET_POS))
#define CCI_PU_DLL                                              CCI_PU_DLL
#define CCI_PU_DLL_POS                                          (30U)
#define CCI_PU_DLL_LEN                                          (1U)
#define CCI_PU_DLL_MSK                                          (((1U<<CCI_PU_DLL_LEN)-1)<<CCI_PU_DLL_POS)
#define CCI_PU_DLL_UMSK                                         (~(((1U<<CCI_PU_DLL_LEN)-1)<<CCI_PU_DLL_POS))
#define CCI_PPU_DLL                                             CCI_PPU_DLL
#define CCI_PPU_DLL_POS                                         (31U)
#define CCI_PPU_DLL_LEN                                         (1U)
#define CCI_PPU_DLL_MSK                                         (((1U<<CCI_PPU_DLL_LEN)-1)<<CCI_PPU_DLL_POS)
#define CCI_PPU_DLL_UMSK                                        (~(((1U<<CCI_PPU_DLL_LEN)-1)<<CCI_PPU_DLL_POS))

/* 0x204 : dll2 */
#define CCI_DLL2_OFFSET                                         (0x204)
#define CCI_DLL_EN_DIV1_RF                                      CCI_DLL_EN_DIV1_RF
#define CCI_DLL_EN_DIV1_RF_POS                                  (0U)
#define CCI_DLL_EN_DIV1_RF_LEN                                  (1U)
#define CCI_DLL_EN_DIV1_RF_MSK                                  (((1U<<CCI_DLL_EN_DIV1_RF_LEN)-1)<<CCI_DLL_EN_DIV1_RF_POS)
#define CCI_DLL_EN_DIV1_RF_UMSK                                 (~(((1U<<CCI_DLL_EN_DIV1_RF_LEN)-1)<<CCI_DLL_EN_DIV1_RF_POS))
#define CCI_DLL_EN_DIV63                                        CCI_DLL_EN_DIV63
#define CCI_DLL_EN_DIV63_POS                                    (1U)
#define CCI_DLL_EN_DIV63_LEN                                    (1U)
#define CCI_DLL_EN_DIV63_MSK                                    (((1U<<CCI_DLL_EN_DIV63_LEN)-1)<<CCI_DLL_EN_DIV63_POS)
#define CCI_DLL_EN_DIV63_UMSK                                   (~(((1U<<CCI_DLL_EN_DIV63_LEN)-1)<<CCI_DLL_EN_DIV63_POS))
#define CCI_DLL_EN_DIV21                                        CCI_DLL_EN_DIV21
#define CCI_DLL_EN_DIV21_POS                                    (2U)
#define CCI_DLL_EN_DIV21_LEN                                    (1U)
#define CCI_DLL_EN_DIV21_MSK                                    (((1U<<CCI_DLL_EN_DIV21_LEN)-1)<<CCI_DLL_EN_DIV21_POS)
#define CCI_DLL_EN_DIV21_UMSK                                   (~(((1U<<CCI_DLL_EN_DIV21_LEN)-1)<<CCI_DLL_EN_DIV21_POS))
#define CCI_DLL_EN_DIV5                                         CCI_DLL_EN_DIV5
#define CCI_DLL_EN_DIV5_POS                                     (3U)
#define CCI_DLL_EN_DIV5_LEN                                     (1U)
#define CCI_DLL_EN_DIV5_MSK                                     (((1U<<CCI_DLL_EN_DIV5_LEN)-1)<<CCI_DLL_EN_DIV5_POS)
#define CCI_DLL_EN_DIV5_UMSK                                    (~(((1U<<CCI_DLL_EN_DIV5_LEN)-1)<<CCI_DLL_EN_DIV5_POS))
#define CCI_DLL_EN_DIV3                                         CCI_DLL_EN_DIV3
#define CCI_DLL_EN_DIV3_POS                                     (4U)
#define CCI_DLL_EN_DIV3_LEN                                     (1U)
#define CCI_DLL_EN_DIV3_MSK                                     (((1U<<CCI_DLL_EN_DIV3_LEN)-1)<<CCI_DLL_EN_DIV3_POS)
#define CCI_DLL_EN_DIV3_UMSK                                    (~(((1U<<CCI_DLL_EN_DIV3_LEN)-1)<<CCI_DLL_EN_DIV3_POS))
#define CCI_DLL_EN_DIV2                                         CCI_DLL_EN_DIV2
#define CCI_DLL_EN_DIV2_POS                                     (5U)
#define CCI_DLL_EN_DIV2_LEN                                     (1U)
#define CCI_DLL_EN_DIV2_MSK                                     (((1U<<CCI_DLL_EN_DIV2_LEN)-1)<<CCI_DLL_EN_DIV2_POS)
#define CCI_DLL_EN_DIV2_UMSK                                    (~(((1U<<CCI_DLL_EN_DIV2_LEN)-1)<<CCI_DLL_EN_DIV2_POS))
#define CCI_DLL_EN_DIV1                                         CCI_DLL_EN_DIV1
#define CCI_DLL_EN_DIV1_POS                                     (6U)
#define CCI_DLL_EN_DIV1_LEN                                     (1U)
#define CCI_DLL_EN_DIV1_MSK                                     (((1U<<CCI_DLL_EN_DIV1_LEN)-1)<<CCI_DLL_EN_DIV1_POS)
#define CCI_DLL_EN_DIV1_UMSK                                    (~(((1U<<CCI_DLL_EN_DIV1_LEN)-1)<<CCI_DLL_EN_DIV1_POS))
#define CCI_DLL_DL_CTRL_DIV1_RF                                 CCI_DLL_DL_CTRL_DIV1_RF
#define CCI_DLL_DL_CTRL_DIV1_RF_POS                             (7U)
#define CCI_DLL_DL_CTRL_DIV1_RF_LEN                             (1U)
#define CCI_DLL_DL_CTRL_DIV1_RF_MSK                             (((1U<<CCI_DLL_DL_CTRL_DIV1_RF_LEN)-1)<<CCI_DLL_DL_CTRL_DIV1_RF_POS)
#define CCI_DLL_DL_CTRL_DIV1_RF_UMSK                            (~(((1U<<CCI_DLL_DL_CTRL_DIV1_RF_LEN)-1)<<CCI_DLL_DL_CTRL_DIV1_RF_POS))
#define CCI_DLL_DL_CTRL_DIV5                                    CCI_DLL_DL_CTRL_DIV5
#define CCI_DLL_DL_CTRL_DIV5_POS                                (8U)
#define CCI_DLL_DL_CTRL_DIV5_LEN                                (1U)
#define CCI_DLL_DL_CTRL_DIV5_MSK                                (((1U<<CCI_DLL_DL_CTRL_DIV5_LEN)-1)<<CCI_DLL_DL_CTRL_DIV5_POS)
#define CCI_DLL_DL_CTRL_DIV5_UMSK                               (~(((1U<<CCI_DLL_DL_CTRL_DIV5_LEN)-1)<<CCI_DLL_DL_CTRL_DIV5_POS))
#define CCI_DLL_DL_CTRL_DIV3                                    CCI_DLL_DL_CTRL_DIV3
#define CCI_DLL_DL_CTRL_DIV3_POS                                (9U)
#define CCI_DLL_DL_CTRL_DIV3_LEN                                (1U)
#define CCI_DLL_DL_CTRL_DIV3_MSK                                (((1U<<CCI_DLL_DL_CTRL_DIV3_LEN)-1)<<CCI_DLL_DL_CTRL_DIV3_POS)
#define CCI_DLL_DL_CTRL_DIV3_UMSK                               (~(((1U<<CCI_DLL_DL_CTRL_DIV3_LEN)-1)<<CCI_DLL_DL_CTRL_DIV3_POS))
#define CCI_DLL_DL_CTRL_DIV2                                    CCI_DLL_DL_CTRL_DIV2
#define CCI_DLL_DL_CTRL_DIV2_POS                                (10U)
#define CCI_DLL_DL_CTRL_DIV2_LEN                                (1U)
#define CCI_DLL_DL_CTRL_DIV2_MSK                                (((1U<<CCI_DLL_DL_CTRL_DIV2_LEN)-1)<<CCI_DLL_DL_CTRL_DIV2_POS)
#define CCI_DLL_DL_CTRL_DIV2_UMSK                               (~(((1U<<CCI_DLL_DL_CTRL_DIV2_LEN)-1)<<CCI_DLL_DL_CTRL_DIV2_POS))
#define CCI_DLL_DL_CTRL_DIV1                                    CCI_DLL_DL_CTRL_DIV1
#define CCI_DLL_DL_CTRL_DIV1_POS                                (11U)
#define CCI_DLL_DL_CTRL_DIV1_LEN                                (1U)
#define CCI_DLL_DL_CTRL_DIV1_MSK                                (((1U<<CCI_DLL_DL_CTRL_DIV1_LEN)-1)<<CCI_DLL_DL_CTRL_DIV1_POS)
#define CCI_DLL_DL_CTRL_DIV1_UMSK                               (~(((1U<<CCI_DLL_DL_CTRL_DIV1_LEN)-1)<<CCI_DLL_DL_CTRL_DIV1_POS))
#define CCI_DLL_POSTDIV_DL_CTRL                                 CCI_DLL_POSTDIV_DL_CTRL
#define CCI_DLL_POSTDIV_DL_CTRL_POS                             (16U)
#define CCI_DLL_POSTDIV_DL_CTRL_LEN                             (1U)
#define CCI_DLL_POSTDIV_DL_CTRL_MSK                             (((1U<<CCI_DLL_POSTDIV_DL_CTRL_LEN)-1)<<CCI_DLL_POSTDIV_DL_CTRL_POS)
#define CCI_DLL_POSTDIV_DL_CTRL_UMSK                            (~(((1U<<CCI_DLL_POSTDIV_DL_CTRL_LEN)-1)<<CCI_DLL_POSTDIV_DL_CTRL_POS))
#define CCI_DLL_POSTDIV_STG2_DIV2                               CCI_DLL_POSTDIV_STG2_DIV2
#define CCI_DLL_POSTDIV_STG2_DIV2_POS                           (17U)
#define CCI_DLL_POSTDIV_STG2_DIV2_LEN                           (1U)
#define CCI_DLL_POSTDIV_STG2_DIV2_MSK                           (((1U<<CCI_DLL_POSTDIV_STG2_DIV2_LEN)-1)<<CCI_DLL_POSTDIV_STG2_DIV2_POS)
#define CCI_DLL_POSTDIV_STG2_DIV2_UMSK                          (~(((1U<<CCI_DLL_POSTDIV_STG2_DIV2_LEN)-1)<<CCI_DLL_POSTDIV_STG2_DIV2_POS))
#define CCI_DLL_POSTDIV_EN                                      CCI_DLL_POSTDIV_EN
#define CCI_DLL_POSTDIV_EN_POS                                  (18U)
#define CCI_DLL_POSTDIV_EN_LEN                                  (1U)
#define CCI_DLL_POSTDIV_EN_MSK                                  (((1U<<CCI_DLL_POSTDIV_EN_LEN)-1)<<CCI_DLL_POSTDIV_EN_POS)
#define CCI_DLL_POSTDIV_EN_UMSK                                 (~(((1U<<CCI_DLL_POSTDIV_EN_LEN)-1)<<CCI_DLL_POSTDIV_EN_POS))
#define CCI_DLL_REF_POLARITY                                    CCI_DLL_REF_POLARITY
#define CCI_DLL_REF_POLARITY_POS                                (19U)
#define CCI_DLL_REF_POLARITY_LEN                                (1U)
#define CCI_DLL_REF_POLARITY_MSK                                (((1U<<CCI_DLL_REF_POLARITY_LEN)-1)<<CCI_DLL_REF_POLARITY_POS)
#define CCI_DLL_REF_POLARITY_UMSK                               (~(((1U<<CCI_DLL_REF_POLARITY_LEN)-1)<<CCI_DLL_REF_POLARITY_POS))
#define CCI_DTEN_DLL_REFBUF                                     CCI_DTEN_DLL_REFBUF
#define CCI_DTEN_DLL_REFBUF_POS                                 (20U)
#define CCI_DTEN_DLL_REFBUF_LEN                                 (1U)
#define CCI_DTEN_DLL_REFBUF_MSK                                 (((1U<<CCI_DTEN_DLL_REFBUF_LEN)-1)<<CCI_DTEN_DLL_REFBUF_POS)
#define CCI_DTEN_DLL_REFBUF_UMSK                                (~(((1U<<CCI_DTEN_DLL_REFBUF_LEN)-1)<<CCI_DTEN_DLL_REFBUF_POS))
#define CCI_DTEN_DLL_POSTDIV                                    CCI_DTEN_DLL_POSTDIV
#define CCI_DTEN_DLL_POSTDIV_POS                                (21U)
#define CCI_DTEN_DLL_POSTDIV_LEN                                (1U)
#define CCI_DTEN_DLL_POSTDIV_MSK                                (((1U<<CCI_DTEN_DLL_POSTDIV_LEN)-1)<<CCI_DTEN_DLL_POSTDIV_POS)
#define CCI_DTEN_DLL_POSTDIV_UMSK                               (~(((1U<<CCI_DTEN_DLL_POSTDIV_LEN)-1)<<CCI_DTEN_DLL_POSTDIV_POS))
#define CCI_DTEN_DLL_CLKDIV63                                   CCI_DTEN_DLL_CLKDIV63
#define CCI_DTEN_DLL_CLKDIV63_POS                               (22U)
#define CCI_DTEN_DLL_CLKDIV63_LEN                               (1U)
#define CCI_DTEN_DLL_CLKDIV63_MSK                               (((1U<<CCI_DTEN_DLL_CLKDIV63_LEN)-1)<<CCI_DTEN_DLL_CLKDIV63_POS)
#define CCI_DTEN_DLL_CLKDIV63_UMSK                              (~(((1U<<CCI_DTEN_DLL_CLKDIV63_LEN)-1)<<CCI_DTEN_DLL_CLKDIV63_POS))
#define CCI_DLL_RESV                                            CCI_DLL_RESV
#define CCI_DLL_RESV_POS                                        (24U)
#define CCI_DLL_RESV_LEN                                        (8U)
#define CCI_DLL_RESV_MSK                                        (((1U<<CCI_DLL_RESV_LEN)-1)<<CCI_DLL_RESV_POS)
#define CCI_DLL_RESV_UMSK                                       (~(((1U<<CCI_DLL_RESV_LEN)-1)<<CCI_DLL_RESV_POS))

/* 0x400 : pu_rst_clkpll */
#define CCI_PU_RST_CLKPLL_OFFSET                                (0x400)
#define CCI_CLKPLL_SDM_RESET                                    CCI_CLKPLL_SDM_RESET
#define CCI_CLKPLL_SDM_RESET_POS                                (0U)
#define CCI_CLKPLL_SDM_RESET_LEN                                (1U)
#define CCI_CLKPLL_SDM_RESET_MSK                                (((1U<<CCI_CLKPLL_SDM_RESET_LEN)-1)<<CCI_CLKPLL_SDM_RESET_POS)
#define CCI_CLKPLL_SDM_RESET_UMSK                               (~(((1U<<CCI_CLKPLL_SDM_RESET_LEN)-1)<<CCI_CLKPLL_SDM_RESET_POS))
#define CCI_CLKPLL_RESET_POSTDIV                                CCI_CLKPLL_RESET_POSTDIV
#define CCI_CLKPLL_RESET_POSTDIV_POS                            (1U)
#define CCI_CLKPLL_RESET_POSTDIV_LEN                            (1U)
#define CCI_CLKPLL_RESET_POSTDIV_MSK                            (((1U<<CCI_CLKPLL_RESET_POSTDIV_LEN)-1)<<CCI_CLKPLL_RESET_POSTDIV_POS)
#define CCI_CLKPLL_RESET_POSTDIV_UMSK                           (~(((1U<<CCI_CLKPLL_RESET_POSTDIV_LEN)-1)<<CCI_CLKPLL_RESET_POSTDIV_POS))
#define CCI_CLKPLL_RESET_FBDV                                   CCI_CLKPLL_RESET_FBDV
#define CCI_CLKPLL_RESET_FBDV_POS                               (2U)
#define CCI_CLKPLL_RESET_FBDV_LEN                               (1U)
#define CCI_CLKPLL_RESET_FBDV_MSK                               (((1U<<CCI_CLKPLL_RESET_FBDV_LEN)-1)<<CCI_CLKPLL_RESET_FBDV_POS)
#define CCI_CLKPLL_RESET_FBDV_UMSK                              (~(((1U<<CCI_CLKPLL_RESET_FBDV_LEN)-1)<<CCI_CLKPLL_RESET_FBDV_POS))
#define CCI_CLKPLL_RESET_REFDIV                                 CCI_CLKPLL_RESET_REFDIV
#define CCI_CLKPLL_RESET_REFDIV_POS                             (3U)
#define CCI_CLKPLL_RESET_REFDIV_LEN                             (1U)
#define CCI_CLKPLL_RESET_REFDIV_MSK                             (((1U<<CCI_CLKPLL_RESET_REFDIV_LEN)-1)<<CCI_CLKPLL_RESET_REFDIV_POS)
#define CCI_CLKPLL_RESET_REFDIV_UMSK                            (~(((1U<<CCI_CLKPLL_RESET_REFDIV_LEN)-1)<<CCI_CLKPLL_RESET_REFDIV_POS))
#define CCI_CLKPLL_PU_POSTDIV                                   CCI_CLKPLL_PU_POSTDIV
#define CCI_CLKPLL_PU_POSTDIV_POS                               (4U)
#define CCI_CLKPLL_PU_POSTDIV_LEN                               (1U)
#define CCI_CLKPLL_PU_POSTDIV_MSK                               (((1U<<CCI_CLKPLL_PU_POSTDIV_LEN)-1)<<CCI_CLKPLL_PU_POSTDIV_POS)
#define CCI_CLKPLL_PU_POSTDIV_UMSK                              (~(((1U<<CCI_CLKPLL_PU_POSTDIV_LEN)-1)<<CCI_CLKPLL_PU_POSTDIV_POS))
#define CCI_CLKPLL_PU_FBDV                                      CCI_CLKPLL_PU_FBDV
#define CCI_CLKPLL_PU_FBDV_POS                                  (5U)
#define CCI_CLKPLL_PU_FBDV_LEN                                  (1U)
#define CCI_CLKPLL_PU_FBDV_MSK                                  (((1U<<CCI_CLKPLL_PU_FBDV_LEN)-1)<<CCI_CLKPLL_PU_FBDV_POS)
#define CCI_CLKPLL_PU_FBDV_UMSK                                 (~(((1U<<CCI_CLKPLL_PU_FBDV_LEN)-1)<<CCI_CLKPLL_PU_FBDV_POS))
#define CCI_CLKPLL_PU_CLAMP_OP                                  CCI_CLKPLL_PU_CLAMP_OP
#define CCI_CLKPLL_PU_CLAMP_OP_POS                              (6U)
#define CCI_CLKPLL_PU_CLAMP_OP_LEN                              (1U)
#define CCI_CLKPLL_PU_CLAMP_OP_MSK                              (((1U<<CCI_CLKPLL_PU_CLAMP_OP_LEN)-1)<<CCI_CLKPLL_PU_CLAMP_OP_POS)
#define CCI_CLKPLL_PU_CLAMP_OP_UMSK                             (~(((1U<<CCI_CLKPLL_PU_CLAMP_OP_LEN)-1)<<CCI_CLKPLL_PU_CLAMP_OP_POS))
#define CCI_CLKPLL_PU_PFD                                       CCI_CLKPLL_PU_PFD
#define CCI_CLKPLL_PU_PFD_POS                                   (7U)
#define CCI_CLKPLL_PU_PFD_LEN                                   (1U)
#define CCI_CLKPLL_PU_PFD_MSK                                   (((1U<<CCI_CLKPLL_PU_PFD_LEN)-1)<<CCI_CLKPLL_PU_PFD_POS)
#define CCI_CLKPLL_PU_PFD_UMSK                                  (~(((1U<<CCI_CLKPLL_PU_PFD_LEN)-1)<<CCI_CLKPLL_PU_PFD_POS))
#define CCI_CLKPLL_PU_CP                                        CCI_CLKPLL_PU_CP
#define CCI_CLKPLL_PU_CP_POS                                    (8U)
#define CCI_CLKPLL_PU_CP_LEN                                    (1U)
#define CCI_CLKPLL_PU_CP_MSK                                    (((1U<<CCI_CLKPLL_PU_CP_LEN)-1)<<CCI_CLKPLL_PU_CP_POS)
#define CCI_CLKPLL_PU_CP_UMSK                                   (~(((1U<<CCI_CLKPLL_PU_CP_LEN)-1)<<CCI_CLKPLL_PU_CP_POS))
#define CCI_PU_CLKPLL_SFREG                                     CCI_PU_CLKPLL_SFREG
#define CCI_PU_CLKPLL_SFREG_POS                                 (9U)
#define CCI_PU_CLKPLL_SFREG_LEN                                 (1U)
#define CCI_PU_CLKPLL_SFREG_MSK                                 (((1U<<CCI_PU_CLKPLL_SFREG_LEN)-1)<<CCI_PU_CLKPLL_SFREG_POS)
#define CCI_PU_CLKPLL_SFREG_UMSK                                (~(((1U<<CCI_PU_CLKPLL_SFREG_LEN)-1)<<CCI_PU_CLKPLL_SFREG_POS))
#define CCI_PU_CLKPLL                                           CCI_PU_CLKPLL
#define CCI_PU_CLKPLL_POS                                       (10U)
#define CCI_PU_CLKPLL_LEN                                       (1U)
#define CCI_PU_CLKPLL_MSK                                       (((1U<<CCI_PU_CLKPLL_LEN)-1)<<CCI_PU_CLKPLL_POS)
#define CCI_PU_CLKPLL_UMSK                                      (~(((1U<<CCI_PU_CLKPLL_LEN)-1)<<CCI_PU_CLKPLL_POS))

/* 0x404 : clkpll_top_ctrl */
#define CCI_CLKPLL_TOP_CTRL_OFFSET                              (0x404)
#define CCI_CLKPLL_POSTDIV                                      CCI_CLKPLL_POSTDIV
#define CCI_CLKPLL_POSTDIV_POS                                  (0U)
#define CCI_CLKPLL_POSTDIV_LEN                                  (7U)
#define CCI_CLKPLL_POSTDIV_MSK                                  (((1U<<CCI_CLKPLL_POSTDIV_LEN)-1)<<CCI_CLKPLL_POSTDIV_POS)
#define CCI_CLKPLL_POSTDIV_UMSK                                 (~(((1U<<CCI_CLKPLL_POSTDIV_LEN)-1)<<CCI_CLKPLL_POSTDIV_POS))
#define CCI_CLKPLL_REFDIV_RATIO                                 CCI_CLKPLL_REFDIV_RATIO
#define CCI_CLKPLL_REFDIV_RATIO_POS                             (8U)
#define CCI_CLKPLL_REFDIV_RATIO_LEN                             (4U)
#define CCI_CLKPLL_REFDIV_RATIO_MSK                             (((1U<<CCI_CLKPLL_REFDIV_RATIO_LEN)-1)<<CCI_CLKPLL_REFDIV_RATIO_POS)
#define CCI_CLKPLL_REFDIV_RATIO_UMSK                            (~(((1U<<CCI_CLKPLL_REFDIV_RATIO_LEN)-1)<<CCI_CLKPLL_REFDIV_RATIO_POS))
#define CCI_CLKPLL_XTAL_RC32M_SEL                               CCI_CLKPLL_XTAL_RC32M_SEL
#define CCI_CLKPLL_XTAL_RC32M_SEL_POS                           (12U)
#define CCI_CLKPLL_XTAL_RC32M_SEL_LEN                           (1U)
#define CCI_CLKPLL_XTAL_RC32M_SEL_MSK                           (((1U<<CCI_CLKPLL_XTAL_RC32M_SEL_LEN)-1)<<CCI_CLKPLL_XTAL_RC32M_SEL_POS)
#define CCI_CLKPLL_XTAL_RC32M_SEL_UMSK                          (~(((1U<<CCI_CLKPLL_XTAL_RC32M_SEL_LEN)-1)<<CCI_CLKPLL_XTAL_RC32M_SEL_POS))
#define CCI_CLKPLL_REFCLK_SEL                                   CCI_CLKPLL_REFCLK_SEL
#define CCI_CLKPLL_REFCLK_SEL_POS                               (16U)
#define CCI_CLKPLL_REFCLK_SEL_LEN                               (1U)
#define CCI_CLKPLL_REFCLK_SEL_MSK                               (((1U<<CCI_CLKPLL_REFCLK_SEL_LEN)-1)<<CCI_CLKPLL_REFCLK_SEL_POS)
#define CCI_CLKPLL_REFCLK_SEL_UMSK                              (~(((1U<<CCI_CLKPLL_REFCLK_SEL_LEN)-1)<<CCI_CLKPLL_REFCLK_SEL_POS))
#define CCI_CLKPLL_VG11_SEL                                     CCI_CLKPLL_VG11_SEL
#define CCI_CLKPLL_VG11_SEL_POS                                 (20U)
#define CCI_CLKPLL_VG11_SEL_LEN                                 (2U)
#define CCI_CLKPLL_VG11_SEL_MSK                                 (((1U<<CCI_CLKPLL_VG11_SEL_LEN)-1)<<CCI_CLKPLL_VG11_SEL_POS)
#define CCI_CLKPLL_VG11_SEL_UMSK                                (~(((1U<<CCI_CLKPLL_VG11_SEL_LEN)-1)<<CCI_CLKPLL_VG11_SEL_POS))
#define CCI_CLKPLL_RESV                                         CCI_CLKPLL_RESV
#define CCI_CLKPLL_RESV_POS                                     (24U)
#define CCI_CLKPLL_RESV_LEN                                     (2U)
#define CCI_CLKPLL_RESV_MSK                                     (((1U<<CCI_CLKPLL_RESV_LEN)-1)<<CCI_CLKPLL_RESV_POS)
#define CCI_CLKPLL_RESV_UMSK                                    (~(((1U<<CCI_CLKPLL_RESV_LEN)-1)<<CCI_CLKPLL_RESV_POS))

/* 0x408 : clkpll_cp */
#define CCI_CLKPLL_CP_OFFSET                                    (0x408)
#define CCI_CLKPLL_SEL_CP_BIAS                                  CCI_CLKPLL_SEL_CP_BIAS
#define CCI_CLKPLL_SEL_CP_BIAS_POS                              (0U)
#define CCI_CLKPLL_SEL_CP_BIAS_LEN                              (1U)
#define CCI_CLKPLL_SEL_CP_BIAS_MSK                              (((1U<<CCI_CLKPLL_SEL_CP_BIAS_LEN)-1)<<CCI_CLKPLL_SEL_CP_BIAS_POS)
#define CCI_CLKPLL_SEL_CP_BIAS_UMSK                             (~(((1U<<CCI_CLKPLL_SEL_CP_BIAS_LEN)-1)<<CCI_CLKPLL_SEL_CP_BIAS_POS))
#define CCI_CLKPLL_ICP_5U                                       CCI_CLKPLL_ICP_5U
#define CCI_CLKPLL_ICP_5U_POS                                   (4U)
#define CCI_CLKPLL_ICP_5U_LEN                                   (2U)
#define CCI_CLKPLL_ICP_5U_MSK                                   (((1U<<CCI_CLKPLL_ICP_5U_LEN)-1)<<CCI_CLKPLL_ICP_5U_POS)
#define CCI_CLKPLL_ICP_5U_UMSK                                  (~(((1U<<CCI_CLKPLL_ICP_5U_LEN)-1)<<CCI_CLKPLL_ICP_5U_POS))
#define CCI_CLKPLL_ICP_1U                                       CCI_CLKPLL_ICP_1U
#define CCI_CLKPLL_ICP_1U_POS                                   (6U)
#define CCI_CLKPLL_ICP_1U_LEN                                   (2U)
#define CCI_CLKPLL_ICP_1U_MSK                                   (((1U<<CCI_CLKPLL_ICP_1U_LEN)-1)<<CCI_CLKPLL_ICP_1U_POS)
#define CCI_CLKPLL_ICP_1U_UMSK                                  (~(((1U<<CCI_CLKPLL_ICP_1U_LEN)-1)<<CCI_CLKPLL_ICP_1U_POS))
#define CCI_CLKPLL_INT_FRAC_SW                                  CCI_CLKPLL_INT_FRAC_SW
#define CCI_CLKPLL_INT_FRAC_SW_POS                              (8U)
#define CCI_CLKPLL_INT_FRAC_SW_LEN                              (1U)
#define CCI_CLKPLL_INT_FRAC_SW_MSK                              (((1U<<CCI_CLKPLL_INT_FRAC_SW_LEN)-1)<<CCI_CLKPLL_INT_FRAC_SW_POS)
#define CCI_CLKPLL_INT_FRAC_SW_UMSK                             (~(((1U<<CCI_CLKPLL_INT_FRAC_SW_LEN)-1)<<CCI_CLKPLL_INT_FRAC_SW_POS))
#define CCI_CLKPLL_CP_STARTUP_EN                                CCI_CLKPLL_CP_STARTUP_EN
#define CCI_CLKPLL_CP_STARTUP_EN_POS                            (9U)
#define CCI_CLKPLL_CP_STARTUP_EN_LEN                            (1U)
#define CCI_CLKPLL_CP_STARTUP_EN_MSK                            (((1U<<CCI_CLKPLL_CP_STARTUP_EN_LEN)-1)<<CCI_CLKPLL_CP_STARTUP_EN_POS)
#define CCI_CLKPLL_CP_STARTUP_EN_UMSK                           (~(((1U<<CCI_CLKPLL_CP_STARTUP_EN_LEN)-1)<<CCI_CLKPLL_CP_STARTUP_EN_POS))
#define CCI_CLKPLL_CP_OPAMP_EN                                  CCI_CLKPLL_CP_OPAMP_EN
#define CCI_CLKPLL_CP_OPAMP_EN_POS                              (10U)
#define CCI_CLKPLL_CP_OPAMP_EN_LEN                              (1U)
#define CCI_CLKPLL_CP_OPAMP_EN_MSK                              (((1U<<CCI_CLKPLL_CP_OPAMP_EN_LEN)-1)<<CCI_CLKPLL_CP_OPAMP_EN_POS)
#define CCI_CLKPLL_CP_OPAMP_EN_UMSK                             (~(((1U<<CCI_CLKPLL_CP_OPAMP_EN_LEN)-1)<<CCI_CLKPLL_CP_OPAMP_EN_POS))

/* 0x40C : clkpll_rz */
#define CCI_CLKPLL_RZ_OFFSET                                    (0x40C)
#define CCI_CLKPLL_C4_EN                                        CCI_CLKPLL_C4_EN
#define CCI_CLKPLL_C4_EN_POS                                    (0U)
#define CCI_CLKPLL_C4_EN_LEN                                    (1U)
#define CCI_CLKPLL_C4_EN_MSK                                    (((1U<<CCI_CLKPLL_C4_EN_LEN)-1)<<CCI_CLKPLL_C4_EN_POS)
#define CCI_CLKPLL_C4_EN_UMSK                                   (~(((1U<<CCI_CLKPLL_C4_EN_LEN)-1)<<CCI_CLKPLL_C4_EN_POS))
#define CCI_CLKPLL_R4                                           CCI_CLKPLL_R4
#define CCI_CLKPLL_R4_POS                                       (4U)
#define CCI_CLKPLL_R4_LEN                                       (2U)
#define CCI_CLKPLL_R4_MSK                                       (((1U<<CCI_CLKPLL_R4_LEN)-1)<<CCI_CLKPLL_R4_POS)
#define CCI_CLKPLL_R4_UMSK                                      (~(((1U<<CCI_CLKPLL_R4_LEN)-1)<<CCI_CLKPLL_R4_POS))
#define CCI_CLKPLL_R4_SHORT                                     CCI_CLKPLL_R4_SHORT
#define CCI_CLKPLL_R4_SHORT_POS                                 (8U)
#define CCI_CLKPLL_R4_SHORT_LEN                                 (1U)
#define CCI_CLKPLL_R4_SHORT_MSK                                 (((1U<<CCI_CLKPLL_R4_SHORT_LEN)-1)<<CCI_CLKPLL_R4_SHORT_POS)
#define CCI_CLKPLL_R4_SHORT_UMSK                                (~(((1U<<CCI_CLKPLL_R4_SHORT_LEN)-1)<<CCI_CLKPLL_R4_SHORT_POS))
#define CCI_CLKPLL_C3                                           CCI_CLKPLL_C3
#define CCI_CLKPLL_C3_POS                                       (12U)
#define CCI_CLKPLL_C3_LEN                                       (2U)
#define CCI_CLKPLL_C3_MSK                                       (((1U<<CCI_CLKPLL_C3_LEN)-1)<<CCI_CLKPLL_C3_POS)
#define CCI_CLKPLL_C3_UMSK                                      (~(((1U<<CCI_CLKPLL_C3_LEN)-1)<<CCI_CLKPLL_C3_POS))
#define CCI_CLKPLL_CZ                                           CCI_CLKPLL_CZ
#define CCI_CLKPLL_CZ_POS                                       (14U)
#define CCI_CLKPLL_CZ_LEN                                       (2U)
#define CCI_CLKPLL_CZ_MSK                                       (((1U<<CCI_CLKPLL_CZ_LEN)-1)<<CCI_CLKPLL_CZ_POS)
#define CCI_CLKPLL_CZ_UMSK                                      (~(((1U<<CCI_CLKPLL_CZ_LEN)-1)<<CCI_CLKPLL_CZ_POS))
#define CCI_CLKPLL_RZ                                           CCI_CLKPLL_RZ
#define CCI_CLKPLL_RZ_POS                                       (16U)
#define CCI_CLKPLL_RZ_LEN                                       (3U)
#define CCI_CLKPLL_RZ_MSK                                       (((1U<<CCI_CLKPLL_RZ_LEN)-1)<<CCI_CLKPLL_RZ_POS)
#define CCI_CLKPLL_RZ_UMSK                                      (~(((1U<<CCI_CLKPLL_RZ_LEN)-1)<<CCI_CLKPLL_RZ_POS))

/* 0x410 : clkpll_fbdv */
#define CCI_CLKPLL_FBDV_OFFSET                                  (0x410)
#define CCI_CLKPLL_SEL_SAMPLE_CLK                               CCI_CLKPLL_SEL_SAMPLE_CLK
#define CCI_CLKPLL_SEL_SAMPLE_CLK_POS                           (0U)
#define CCI_CLKPLL_SEL_SAMPLE_CLK_LEN                           (2U)
#define CCI_CLKPLL_SEL_SAMPLE_CLK_MSK                           (((1U<<CCI_CLKPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_CLKPLL_SEL_SAMPLE_CLK_POS)
#define CCI_CLKPLL_SEL_SAMPLE_CLK_UMSK                          (~(((1U<<CCI_CLKPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_CLKPLL_SEL_SAMPLE_CLK_POS))
#define CCI_CLKPLL_SEL_FB_CLK                                   CCI_CLKPLL_SEL_FB_CLK
#define CCI_CLKPLL_SEL_FB_CLK_POS                               (2U)
#define CCI_CLKPLL_SEL_FB_CLK_LEN                               (2U)
#define CCI_CLKPLL_SEL_FB_CLK_MSK                               (((1U<<CCI_CLKPLL_SEL_FB_CLK_LEN)-1)<<CCI_CLKPLL_SEL_FB_CLK_POS)
#define CCI_CLKPLL_SEL_FB_CLK_UMSK                              (~(((1U<<CCI_CLKPLL_SEL_FB_CLK_LEN)-1)<<CCI_CLKPLL_SEL_FB_CLK_POS))

/* 0x414 : clkpll_vco */
#define CCI_CLKPLL_VCO_OFFSET                                   (0x414)
#define CCI_CLKPLL_VCO_SPEED                                    CCI_CLKPLL_VCO_SPEED
#define CCI_CLKPLL_VCO_SPEED_POS                                (0U)
#define CCI_CLKPLL_VCO_SPEED_LEN                                (3U)
#define CCI_CLKPLL_VCO_SPEED_MSK                                (((1U<<CCI_CLKPLL_VCO_SPEED_LEN)-1)<<CCI_CLKPLL_VCO_SPEED_POS)
#define CCI_CLKPLL_VCO_SPEED_UMSK                               (~(((1U<<CCI_CLKPLL_VCO_SPEED_LEN)-1)<<CCI_CLKPLL_VCO_SPEED_POS))
#define CCI_CLKPLL_SHRTR                                        CCI_CLKPLL_SHRTR
#define CCI_CLKPLL_SHRTR_POS                                    (3U)
#define CCI_CLKPLL_SHRTR_LEN                                    (1U)
#define CCI_CLKPLL_SHRTR_MSK                                    (((1U<<CCI_CLKPLL_SHRTR_LEN)-1)<<CCI_CLKPLL_SHRTR_POS)
#define CCI_CLKPLL_SHRTR_UMSK                                   (~(((1U<<CCI_CLKPLL_SHRTR_LEN)-1)<<CCI_CLKPLL_SHRTR_POS))

/* 0x418 : clkpll_sdm */
#define CCI_CLKPLL_SDM_OFFSET                                   (0x418)
#define CCI_CLKPLL_SDMIN                                        CCI_CLKPLL_SDMIN
#define CCI_CLKPLL_SDMIN_POS                                    (0U)
#define CCI_CLKPLL_SDMIN_LEN                                    (24U)
#define CCI_CLKPLL_SDMIN_MSK                                    (((1U<<CCI_CLKPLL_SDMIN_LEN)-1)<<CCI_CLKPLL_SDMIN_POS)
#define CCI_CLKPLL_SDMIN_UMSK                                   (~(((1U<<CCI_CLKPLL_SDMIN_LEN)-1)<<CCI_CLKPLL_SDMIN_POS))
#define CCI_CLKPLL_DITHER_SEL                                   CCI_CLKPLL_DITHER_SEL
#define CCI_CLKPLL_DITHER_SEL_POS                               (24U)
#define CCI_CLKPLL_DITHER_SEL_LEN                               (2U)
#define CCI_CLKPLL_DITHER_SEL_MSK                               (((1U<<CCI_CLKPLL_DITHER_SEL_LEN)-1)<<CCI_CLKPLL_DITHER_SEL_POS)
#define CCI_CLKPLL_DITHER_SEL_UMSK                              (~(((1U<<CCI_CLKPLL_DITHER_SEL_LEN)-1)<<CCI_CLKPLL_DITHER_SEL_POS))
#define CCI_CLKPLL_SDM_FLAG                                     CCI_CLKPLL_SDM_FLAG
#define CCI_CLKPLL_SDM_FLAG_POS                                 (28U)
#define CCI_CLKPLL_SDM_FLAG_LEN                                 (1U)
#define CCI_CLKPLL_SDM_FLAG_MSK                                 (((1U<<CCI_CLKPLL_SDM_FLAG_LEN)-1)<<CCI_CLKPLL_SDM_FLAG_POS)
#define CCI_CLKPLL_SDM_FLAG_UMSK                                (~(((1U<<CCI_CLKPLL_SDM_FLAG_LEN)-1)<<CCI_CLKPLL_SDM_FLAG_POS))
#define CCI_CLKPLL_SDM_BYPASS                                   CCI_CLKPLL_SDM_BYPASS
#define CCI_CLKPLL_SDM_BYPASS_POS                               (29U)
#define CCI_CLKPLL_SDM_BYPASS_LEN                               (1U)
#define CCI_CLKPLL_SDM_BYPASS_MSK                               (((1U<<CCI_CLKPLL_SDM_BYPASS_LEN)-1)<<CCI_CLKPLL_SDM_BYPASS_POS)
#define CCI_CLKPLL_SDM_BYPASS_UMSK                              (~(((1U<<CCI_CLKPLL_SDM_BYPASS_LEN)-1)<<CCI_CLKPLL_SDM_BYPASS_POS))

/* 0x41C : clkpll_output_en */
#define CCI_CLKPLL_OUTPUT_EN_OFFSET                             (0x41C)
#define CCI_CLKPLL_EN_480M                                      CCI_CLKPLL_EN_480M
#define CCI_CLKPLL_EN_480M_POS                                  (0U)
#define CCI_CLKPLL_EN_480M_LEN                                  (1U)
#define CCI_CLKPLL_EN_480M_MSK                                  (((1U<<CCI_CLKPLL_EN_480M_LEN)-1)<<CCI_CLKPLL_EN_480M_POS)
#define CCI_CLKPLL_EN_480M_UMSK                                 (~(((1U<<CCI_CLKPLL_EN_480M_LEN)-1)<<CCI_CLKPLL_EN_480M_POS))
#define CCI_CLKPLL_EN_240M                                      CCI_CLKPLL_EN_240M
#define CCI_CLKPLL_EN_240M_POS                                  (1U)
#define CCI_CLKPLL_EN_240M_LEN                                  (1U)
#define CCI_CLKPLL_EN_240M_MSK                                  (((1U<<CCI_CLKPLL_EN_240M_LEN)-1)<<CCI_CLKPLL_EN_240M_POS)
#define CCI_CLKPLL_EN_240M_UMSK                                 (~(((1U<<CCI_CLKPLL_EN_240M_LEN)-1)<<CCI_CLKPLL_EN_240M_POS))
#define CCI_CLKPLL_EN_192M                                      CCI_CLKPLL_EN_192M
#define CCI_CLKPLL_EN_192M_POS                                  (2U)
#define CCI_CLKPLL_EN_192M_LEN                                  (1U)
#define CCI_CLKPLL_EN_192M_MSK                                  (((1U<<CCI_CLKPLL_EN_192M_LEN)-1)<<CCI_CLKPLL_EN_192M_POS)
#define CCI_CLKPLL_EN_192M_UMSK                                 (~(((1U<<CCI_CLKPLL_EN_192M_LEN)-1)<<CCI_CLKPLL_EN_192M_POS))
#define CCI_CLKPLL_EN_160M                                      CCI_CLKPLL_EN_160M
#define CCI_CLKPLL_EN_160M_POS                                  (3U)
#define CCI_CLKPLL_EN_160M_LEN                                  (1U)
#define CCI_CLKPLL_EN_160M_MSK                                  (((1U<<CCI_CLKPLL_EN_160M_LEN)-1)<<CCI_CLKPLL_EN_160M_POS)
#define CCI_CLKPLL_EN_160M_UMSK                                 (~(((1U<<CCI_CLKPLL_EN_160M_LEN)-1)<<CCI_CLKPLL_EN_160M_POS))
#define CCI_CLKPLL_EN_120M                                      CCI_CLKPLL_EN_120M
#define CCI_CLKPLL_EN_120M_POS                                  (4U)
#define CCI_CLKPLL_EN_120M_LEN                                  (1U)
#define CCI_CLKPLL_EN_120M_MSK                                  (((1U<<CCI_CLKPLL_EN_120M_LEN)-1)<<CCI_CLKPLL_EN_120M_POS)
#define CCI_CLKPLL_EN_120M_UMSK                                 (~(((1U<<CCI_CLKPLL_EN_120M_LEN)-1)<<CCI_CLKPLL_EN_120M_POS))
#define CCI_CLKPLL_EN_96M                                       CCI_CLKPLL_EN_96M
#define CCI_CLKPLL_EN_96M_POS                                   (5U)
#define CCI_CLKPLL_EN_96M_LEN                                   (1U)
#define CCI_CLKPLL_EN_96M_MSK                                   (((1U<<CCI_CLKPLL_EN_96M_LEN)-1)<<CCI_CLKPLL_EN_96M_POS)
#define CCI_CLKPLL_EN_96M_UMSK                                  (~(((1U<<CCI_CLKPLL_EN_96M_LEN)-1)<<CCI_CLKPLL_EN_96M_POS))
#define CCI_CLKPLL_EN_80M                                       CCI_CLKPLL_EN_80M
#define CCI_CLKPLL_EN_80M_POS                                   (6U)
#define CCI_CLKPLL_EN_80M_LEN                                   (1U)
#define CCI_CLKPLL_EN_80M_MSK                                   (((1U<<CCI_CLKPLL_EN_80M_LEN)-1)<<CCI_CLKPLL_EN_80M_POS)
#define CCI_CLKPLL_EN_80M_UMSK                                  (~(((1U<<CCI_CLKPLL_EN_80M_LEN)-1)<<CCI_CLKPLL_EN_80M_POS))
#define CCI_CLKPLL_EN_48M                                       CCI_CLKPLL_EN_48M
#define CCI_CLKPLL_EN_48M_POS                                   (7U)
#define CCI_CLKPLL_EN_48M_LEN                                   (1U)
#define CCI_CLKPLL_EN_48M_MSK                                   (((1U<<CCI_CLKPLL_EN_48M_LEN)-1)<<CCI_CLKPLL_EN_48M_POS)
#define CCI_CLKPLL_EN_48M_UMSK                                  (~(((1U<<CCI_CLKPLL_EN_48M_LEN)-1)<<CCI_CLKPLL_EN_48M_POS))
#define CCI_CLKPLL_EN_32M                                       CCI_CLKPLL_EN_32M
#define CCI_CLKPLL_EN_32M_POS                                   (8U)
#define CCI_CLKPLL_EN_32M_LEN                                   (1U)
#define CCI_CLKPLL_EN_32M_MSK                                   (((1U<<CCI_CLKPLL_EN_32M_LEN)-1)<<CCI_CLKPLL_EN_32M_POS)
#define CCI_CLKPLL_EN_32M_UMSK                                  (~(((1U<<CCI_CLKPLL_EN_32M_LEN)-1)<<CCI_CLKPLL_EN_32M_POS))
#define CCI_CLKPLL_EN_DIV2_480M                                 CCI_CLKPLL_EN_DIV2_480M
#define CCI_CLKPLL_EN_DIV2_480M_POS                             (9U)
#define CCI_CLKPLL_EN_DIV2_480M_LEN                             (1U)
#define CCI_CLKPLL_EN_DIV2_480M_MSK                             (((1U<<CCI_CLKPLL_EN_DIV2_480M_LEN)-1)<<CCI_CLKPLL_EN_DIV2_480M_POS)
#define CCI_CLKPLL_EN_DIV2_480M_UMSK                            (~(((1U<<CCI_CLKPLL_EN_DIV2_480M_LEN)-1)<<CCI_CLKPLL_EN_DIV2_480M_POS))

/* 0x420 : clkpll_test_enable */
#define CCI_CLKPLL_TEST_ENABLE_OFFSET                           (0x420)
#define CCI_DTEN_CLKPLL_POSTDIV_CLK                             CCI_DTEN_CLKPLL_POSTDIV_CLK
#define CCI_DTEN_CLKPLL_POSTDIV_CLK_POS                         (0U)
#define CCI_DTEN_CLKPLL_POSTDIV_CLK_LEN                         (1U)
#define CCI_DTEN_CLKPLL_POSTDIV_CLK_MSK                         (((1U<<CCI_DTEN_CLKPLL_POSTDIV_CLK_LEN)-1)<<CCI_DTEN_CLKPLL_POSTDIV_CLK_POS)
#define CCI_DTEN_CLKPLL_POSTDIV_CLK_UMSK                        (~(((1U<<CCI_DTEN_CLKPLL_POSTDIV_CLK_LEN)-1)<<CCI_DTEN_CLKPLL_POSTDIV_CLK_POS))
#define CCI_DTEN_CLK96M                                         CCI_DTEN_CLK96M
#define CCI_DTEN_CLK96M_POS                                     (1U)
#define CCI_DTEN_CLK96M_LEN                                     (1U)
#define CCI_DTEN_CLK96M_MSK                                     (((1U<<CCI_DTEN_CLK96M_LEN)-1)<<CCI_DTEN_CLK96M_POS)
#define CCI_DTEN_CLK96M_UMSK                                    (~(((1U<<CCI_DTEN_CLK96M_LEN)-1)<<CCI_DTEN_CLK96M_POS))
#define CCI_DTEN_CLK32M                                         CCI_DTEN_CLK32M
#define CCI_DTEN_CLK32M_POS                                     (2U)
#define CCI_DTEN_CLK32M_LEN                                     (1U)
#define CCI_DTEN_CLK32M_MSK                                     (((1U<<CCI_DTEN_CLK32M_LEN)-1)<<CCI_DTEN_CLK32M_POS)
#define CCI_DTEN_CLK32M_UMSK                                    (~(((1U<<CCI_DTEN_CLK32M_LEN)-1)<<CCI_DTEN_CLK32M_POS))
#define CCI_DTEN_CLKPLL_FSDM                                    CCI_DTEN_CLKPLL_FSDM
#define CCI_DTEN_CLKPLL_FSDM_POS                                (3U)
#define CCI_DTEN_CLKPLL_FSDM_LEN                                (1U)
#define CCI_DTEN_CLKPLL_FSDM_MSK                                (((1U<<CCI_DTEN_CLKPLL_FSDM_LEN)-1)<<CCI_DTEN_CLKPLL_FSDM_POS)
#define CCI_DTEN_CLKPLL_FSDM_UMSK                               (~(((1U<<CCI_DTEN_CLKPLL_FSDM_LEN)-1)<<CCI_DTEN_CLKPLL_FSDM_POS))
#define CCI_DTEN_CLKPLL_FREF                                    CCI_DTEN_CLKPLL_FREF
#define CCI_DTEN_CLKPLL_FREF_POS                                (4U)
#define CCI_DTEN_CLKPLL_FREF_LEN                                (1U)
#define CCI_DTEN_CLKPLL_FREF_MSK                                (((1U<<CCI_DTEN_CLKPLL_FREF_LEN)-1)<<CCI_DTEN_CLKPLL_FREF_POS)
#define CCI_DTEN_CLKPLL_FREF_UMSK                               (~(((1U<<CCI_DTEN_CLKPLL_FREF_LEN)-1)<<CCI_DTEN_CLKPLL_FREF_POS))
#define CCI_DTEN_CLKPLL_FIN                                     CCI_DTEN_CLKPLL_FIN
#define CCI_DTEN_CLKPLL_FIN_POS                                 (5U)
#define CCI_DTEN_CLKPLL_FIN_LEN                                 (1U)
#define CCI_DTEN_CLKPLL_FIN_MSK                                 (((1U<<CCI_DTEN_CLKPLL_FIN_LEN)-1)<<CCI_DTEN_CLKPLL_FIN_POS)
#define CCI_DTEN_CLKPLL_FIN_UMSK                                (~(((1U<<CCI_DTEN_CLKPLL_FIN_LEN)-1)<<CCI_DTEN_CLKPLL_FIN_POS))
#define CCI_TEN_CLKPLL_SFREG                                    CCI_TEN_CLKPLL_SFREG
#define CCI_TEN_CLKPLL_SFREG_POS                                (6U)
#define CCI_TEN_CLKPLL_SFREG_LEN                                (1U)
#define CCI_TEN_CLKPLL_SFREG_MSK                                (((1U<<CCI_TEN_CLKPLL_SFREG_LEN)-1)<<CCI_TEN_CLKPLL_SFREG_POS)
#define CCI_TEN_CLKPLL_SFREG_UMSK                               (~(((1U<<CCI_TEN_CLKPLL_SFREG_LEN)-1)<<CCI_TEN_CLKPLL_SFREG_POS))
#define CCI_TEN_CLKPLL                                          CCI_TEN_CLKPLL
#define CCI_TEN_CLKPLL_POS                                      (7U)
#define CCI_TEN_CLKPLL_LEN                                      (1U)
#define CCI_TEN_CLKPLL_MSK                                      (((1U<<CCI_TEN_CLKPLL_LEN)-1)<<CCI_TEN_CLKPLL_POS)
#define CCI_TEN_CLKPLL_UMSK                                     (~(((1U<<CCI_TEN_CLKPLL_LEN)-1)<<CCI_TEN_CLKPLL_POS))
#define CCI_CLKPLL_DC_TP_OUT_EN                                 CCI_CLKPLL_DC_TP_OUT_EN
#define CCI_CLKPLL_DC_TP_OUT_EN_POS                             (8U)
#define CCI_CLKPLL_DC_TP_OUT_EN_LEN                             (1U)
#define CCI_CLKPLL_DC_TP_OUT_EN_MSK                             (((1U<<CCI_CLKPLL_DC_TP_OUT_EN_LEN)-1)<<CCI_CLKPLL_DC_TP_OUT_EN_POS)
#define CCI_CLKPLL_DC_TP_OUT_EN_UMSK                            (~(((1U<<CCI_CLKPLL_DC_TP_OUT_EN_LEN)-1)<<CCI_CLKPLL_DC_TP_OUT_EN_POS))


struct  cci_reg {
    /* 0x0 : cci_cfg */
    union {
        struct {
            uint32_t cci_en                         :  1; /* [    0],        r/w,        0x1 */
            uint32_t cci_slv_sel_cci2               :  1; /* [    1],        r/w,        0x0 */
            uint32_t cci_mas_sel_cci2               :  1; /* [    2],        r/w,        0x0 */
            uint32_t cci_mas_hw_mode                :  1; /* [    3],        r/w,        0x0 */
            uint32_t reg_m_cci_sclk_en              :  1; /* [    4],        r/w,        0x0 */
            uint32_t reg_div_m_cci_sclk             :  2; /* [ 6: 5],        r/w,        0x1 */
            uint32_t cfg_cci1_pre_read              :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_scci_clk_inv               :  1; /* [    8],        r/w,        0x0 */
            uint32_t reg_mcci_clk_inv               :  1; /* [    9],        r/w,        0x1 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_cfg;

    /* 0x4 : cci_addr */
    union {
        struct {
            uint32_t apb_cci_addr                   : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_addr;

    /* 0x8 : cci_wdata */
    union {
        struct {
            uint32_t apb_cci_wdata                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_wdata;

    /* 0xC : cci_rdata */
    union {
        struct {
            uint32_t apb_cci_rdata                  : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_rdata;

    /* 0x10 : cci_ctl */
    union {
        struct {
            uint32_t cci_write_flag                 :  1; /* [    0],          r,        0x0 */
            uint32_t cci_read_flag                  :  1; /* [    1],          r,        0x0 */
            uint32_t ahb_state                      :  2; /* [ 3: 2],          r,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cci_ctl;

    /* 0x14  reserved */
    uint8_t RESERVED0x14[492];

    /* 0x200 : dll */
    union {
        struct {
            uint32_t reserved_0_1                   :  2; /* [ 1: 0],       rsvd,        0x0 */
            uint32_t ten_dll                        :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t dll_vctrl_sel                  :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t dll_prechg_sel                 :  1; /* [   12],        r/w,        0x1 */
            uint32_t dll_prechg_reg                 :  1; /* [   13],        r/w,        0x1 */
            uint32_t dll_prechg_en                  :  1; /* [   14],        r/w,        0x1 */
            uint32_t dll_vctrl_force_en             :  1; /* [   15],        r/w,        0x0 */
            uint32_t dll_postdiv                    :  6; /* [21:16],        r/w,       0x15 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t dll_delay_sel                  :  2; /* [25:24],        r/w,        0x1 */
            uint32_t dll_cp_op_en                   :  1; /* [   26],        r/w,        0x1 */
            uint32_t dll_cp_hiz                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t dll_refclk_sel                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t dll_reset                      :  1; /* [   29],        r/w,        0x1 */
            uint32_t pu_dll                         :  1; /* [   30],        r/w,        0x0 */
            uint32_t ppu_dll                        :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dll;

    /* 0x204 : dll2 */
    union {
        struct {
            uint32_t dll_en_div1_rf                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t dll_en_div63                   :  1; /* [    1],        r/w,        0x0 */
            uint32_t dll_en_div21                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t dll_en_div5                    :  1; /* [    3],        r/w,        0x0 */
            uint32_t dll_en_div3                    :  1; /* [    4],        r/w,        0x0 */
            uint32_t dll_en_div2                    :  1; /* [    5],        r/w,        0x0 */
            uint32_t dll_en_div1                    :  1; /* [    6],        r/w,        0x1 */
            uint32_t dll_dl_ctrl_div1_rf            :  1; /* [    7],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div5               :  1; /* [    8],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div3               :  1; /* [    9],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div2               :  1; /* [   10],        r/w,        0x0 */
            uint32_t dll_dl_ctrl_div1               :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t dll_postdiv_dl_ctrl            :  1; /* [   16],        r/w,        0x0 */
            uint32_t dll_postdiv_stg2_div2          :  1; /* [   17],        r/w,        0x0 */
            uint32_t dll_postdiv_en                 :  1; /* [   18],        r/w,        0x0 */
            uint32_t dll_ref_polarity               :  1; /* [   19],        r/w,        0x0 */
            uint32_t dten_dll_refbuf                :  1; /* [   20],        r/w,        0x0 */
            uint32_t dten_dll_postdiv               :  1; /* [   21],        r/w,        0x0 */
            uint32_t dten_dll_clkdiv63              :  1; /* [   22],        r/w,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t dll_resv                       :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } dll2;

    /* 0x208  reserved */
    uint8_t RESERVED0x208[504];

    /* 0x400 : pu_rst_clkpll */
    union {
        struct {
            uint32_t clkpll_sdm_reset               :  1; /* [    0],        r/w,        0x0 */
            uint32_t clkpll_reset_postdiv           :  1; /* [    1],        r/w,        0x0 */
            uint32_t clkpll_reset_fbdv              :  1; /* [    2],        r/w,        0x0 */
            uint32_t clkpll_reset_refdiv            :  1; /* [    3],        r/w,        0x0 */
            uint32_t clkpll_pu_postdiv              :  1; /* [    4],        r/w,        0x1 */
            uint32_t clkpll_pu_fbdv                 :  1; /* [    5],        r/w,        0x1 */
            uint32_t clkpll_pu_clamp_op             :  1; /* [    6],        r/w,        0x1 */
            uint32_t clkpll_pu_pfd                  :  1; /* [    7],        r/w,        0x1 */
            uint32_t clkpll_pu_cp                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t pu_clkpll_sfreg                :  1; /* [    9],        r/w,        0x0 */
            uint32_t pu_clkpll                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pu_rst_clkpll;

    /* 0x404 : clkpll_top_ctrl */
    union {
        struct {
            uint32_t clkpll_postdiv                 :  7; /* [ 6: 0],        r/w,       0x14 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t clkpll_refdiv_ratio            :  4; /* [11: 8],        r/w,        0x4 */
            uint32_t clkpll_xtal_rc32m_sel          :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t clkpll_refclk_sel              :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t clkpll_vg11_sel                :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t clkpll_resv                    :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_top_ctrl;

    /* 0x408 : clkpll_cp */
    union {
        struct {
            uint32_t clkpll_sel_cp_bias             :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t clkpll_icp_5u                  :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t clkpll_icp_1u                  :  2; /* [ 7: 6],        r/w,        0x1 */
            uint32_t clkpll_int_frac_sw             :  1; /* [    8],        r/w,        0x1 */
            uint32_t clkpll_cp_startup_en           :  1; /* [    9],        r/w,        0x1 */
            uint32_t clkpll_cp_opamp_en             :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_cp;

    /* 0x40C : clkpll_rz */
    union {
        struct {
            uint32_t clkpll_c4_en                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t clkpll_r4                      :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t clkpll_r4_short                :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t clkpll_c3                      :  2; /* [13:12],        r/w,        0x2 */
            uint32_t clkpll_cz                      :  2; /* [15:14],        r/w,        0x2 */
            uint32_t clkpll_rz                      :  3; /* [18:16],        r/w,        0x5 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_rz;

    /* 0x410 : clkpll_fbdv */
    union {
        struct {
            uint32_t clkpll_sel_sample_clk          :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t clkpll_sel_fb_clk              :  2; /* [ 3: 2],        r/w,        0x1 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_fbdv;

    /* 0x414 : clkpll_vco */
    union {
        struct {
            uint32_t clkpll_vco_speed               :  3; /* [ 2: 0],        r/w,        0x6 */
            uint32_t clkpll_shrtr                   :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31                  : 28; /* [31: 4],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_vco;

    /* 0x418 : clkpll_sdm */
    union {
        struct {
            uint32_t clkpll_sdmin                   : 24; /* [23: 0],        r/w,   0x780000 */
            uint32_t clkpll_dither_sel              :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t clkpll_sdm_flag                :  1; /* [   28],        r/w,        0x1 */
            uint32_t clkpll_sdm_bypass              :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_sdm;

    /* 0x41C : clkpll_output_en */
    union {
        struct {
            uint32_t clkpll_en_480m                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t clkpll_en_240m                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t clkpll_en_192m                 :  1; /* [    2],        r/w,        0x0 */
            uint32_t clkpll_en_160m                 :  1; /* [    3],        r/w,        0x0 */
            uint32_t clkpll_en_120m                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t clkpll_en_96m                  :  1; /* [    5],        r/w,        0x0 */
            uint32_t clkpll_en_80m                  :  1; /* [    6],        r/w,        0x0 */
            uint32_t clkpll_en_48m                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t clkpll_en_32m                  :  1; /* [    8],        r/w,        0x1 */
            uint32_t clkpll_en_div2_480m            :  1; /* [    9],        r/w,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_output_en;

    /* 0x420 : clkpll_test_enable */
    union {
        struct {
            uint32_t dten_clkpll_postdiv_clk        :  1; /* [    0],        r/w,        0x0 */
            uint32_t dten_clk96M                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t dten_clk32M                    :  1; /* [    2],        r/w,        0x0 */
            uint32_t dten_clkpll_fsdm               :  1; /* [    3],        r/w,        0x0 */
            uint32_t dten_clkpll_fref               :  1; /* [    4],        r/w,        0x0 */
            uint32_t dten_clkpll_fin                :  1; /* [    5],        r/w,        0x0 */
            uint32_t ten_clkpll_sfreg               :  1; /* [    6],        r/w,        0x0 */
            uint32_t ten_clkpll                     :  1; /* [    7],        r/w,        0x0 */
            uint32_t clkpll_dc_tp_out_en            :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } clkpll_test_enable;

};

typedef volatile struct cci_reg cci_reg_t;


#endif  /* __CCI_REG_H__ */
