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

#include "ipro6.h"

/* 0x0 : cci_cfg */
#define CCI_CFG_OFFSET                                          (0x0)
#define CCI_EN                                                  CCI_EN
#define CCI_EN_POS                                              (0U)
#define CCI_EN_LEN                                              (1U)
#define CCI_EN_MSK                                              (((1ULL<<CCI_EN_LEN)-1)<<CCI_EN_POS)
#define CCI_EN_UMSK                                             (~(((1ULL<<CCI_EN_LEN)-1)<<CCI_EN_POS))
#define CCI_SLV_SEL_CCI2                                        CCI_SLV_SEL_CCI2
#define CCI_SLV_SEL_CCI2_POS                                    (1U)
#define CCI_SLV_SEL_CCI2_LEN                                    (1U)
#define CCI_SLV_SEL_CCI2_MSK                                    (((1ULL<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS)
#define CCI_SLV_SEL_CCI2_UMSK                                   (~(((1ULL<<CCI_SLV_SEL_CCI2_LEN)-1)<<CCI_SLV_SEL_CCI2_POS))
#define CCI_MAS_SEL_CCI2                                        CCI_MAS_SEL_CCI2
#define CCI_MAS_SEL_CCI2_POS                                    (2U)
#define CCI_MAS_SEL_CCI2_LEN                                    (1U)
#define CCI_MAS_SEL_CCI2_MSK                                    (((1ULL<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS)
#define CCI_MAS_SEL_CCI2_UMSK                                   (~(((1ULL<<CCI_MAS_SEL_CCI2_LEN)-1)<<CCI_MAS_SEL_CCI2_POS))
#define CCI_MAS_HW_MODE                                         CCI_MAS_HW_MODE
#define CCI_MAS_HW_MODE_POS                                     (3U)
#define CCI_MAS_HW_MODE_LEN                                     (1U)
#define CCI_MAS_HW_MODE_MSK                                     (((1ULL<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS)
#define CCI_MAS_HW_MODE_UMSK                                    (~(((1ULL<<CCI_MAS_HW_MODE_LEN)-1)<<CCI_MAS_HW_MODE_POS))
#define CCI_REG_M_CCI_SCLK_EN                                   CCI_REG_M_CCI_SCLK_EN
#define CCI_REG_M_CCI_SCLK_EN_POS                               (4U)
#define CCI_REG_M_CCI_SCLK_EN_LEN                               (1U)
#define CCI_REG_M_CCI_SCLK_EN_MSK                               (((1ULL<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS)
#define CCI_REG_M_CCI_SCLK_EN_UMSK                              (~(((1ULL<<CCI_REG_M_CCI_SCLK_EN_LEN)-1)<<CCI_REG_M_CCI_SCLK_EN_POS))
#define CCI_REG_DIV_M_CCI_SCLK                                  CCI_REG_DIV_M_CCI_SCLK
#define CCI_REG_DIV_M_CCI_SCLK_POS                              (5U)
#define CCI_REG_DIV_M_CCI_SCLK_LEN                              (2U)
#define CCI_REG_DIV_M_CCI_SCLK_MSK                              (((1ULL<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS)
#define CCI_REG_DIV_M_CCI_SCLK_UMSK                             (~(((1ULL<<CCI_REG_DIV_M_CCI_SCLK_LEN)-1)<<CCI_REG_DIV_M_CCI_SCLK_POS))
#define CCI_CFG_CCI1_PRE_READ                                   CCI_CFG_CCI1_PRE_READ
#define CCI_CFG_CCI1_PRE_READ_POS                               (7U)
#define CCI_CFG_CCI1_PRE_READ_LEN                               (1U)
#define CCI_CFG_CCI1_PRE_READ_MSK                               (((1ULL<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS)
#define CCI_CFG_CCI1_PRE_READ_UMSK                              (~(((1ULL<<CCI_CFG_CCI1_PRE_READ_LEN)-1)<<CCI_CFG_CCI1_PRE_READ_POS))
#define CCI_REG_SCCI_CLK_INV                                    CCI_REG_SCCI_CLK_INV
#define CCI_REG_SCCI_CLK_INV_POS                                (8U)
#define CCI_REG_SCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_SCCI_CLK_INV_MSK                                (((1ULL<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS)
#define CCI_REG_SCCI_CLK_INV_UMSK                               (~(((1ULL<<CCI_REG_SCCI_CLK_INV_LEN)-1)<<CCI_REG_SCCI_CLK_INV_POS))
#define CCI_REG_MCCI_CLK_INV                                    CCI_REG_MCCI_CLK_INV
#define CCI_REG_MCCI_CLK_INV_POS                                (9U)
#define CCI_REG_MCCI_CLK_INV_LEN                                (1U)
#define CCI_REG_MCCI_CLK_INV_MSK                                (((1ULL<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS)
#define CCI_REG_MCCI_CLK_INV_UMSK                               (~(((1ULL<<CCI_REG_MCCI_CLK_INV_LEN)-1)<<CCI_REG_MCCI_CLK_INV_POS))
#define CCI_CFG_MCCI_DLY_R                                      CCI_CFG_MCCI_DLY_R
#define CCI_CFG_MCCI_DLY_R_POS                                  (16U)
#define CCI_CFG_MCCI_DLY_R_LEN                                  (1U)
#define CCI_CFG_MCCI_DLY_R_MSK                                  (((1ULL<<CCI_CFG_MCCI_DLY_R_LEN)-1)<<CCI_CFG_MCCI_DLY_R_POS)
#define CCI_CFG_MCCI_DLY_R_UMSK                                 (~(((1ULL<<CCI_CFG_MCCI_DLY_R_LEN)-1)<<CCI_CFG_MCCI_DLY_R_POS))

/* 0x4 : cci_addr */
#define CCI_ADDR_OFFSET                                         (0x4)
#define CCI_APB_CCI_ADDR                                        CCI_APB_CCI_ADDR
#define CCI_APB_CCI_ADDR_POS                                    (0U)
#define CCI_APB_CCI_ADDR_LEN                                    (32U)
#define CCI_APB_CCI_ADDR_MSK                                    (((1ULL<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS)
#define CCI_APB_CCI_ADDR_UMSK                                   (~(((1ULL<<CCI_APB_CCI_ADDR_LEN)-1)<<CCI_APB_CCI_ADDR_POS))

/* 0x8 : cci_wdata */
#define CCI_WDATA_OFFSET                                        (0x8)
#define CCI_APB_CCI_WDATA                                       CCI_APB_CCI_WDATA
#define CCI_APB_CCI_WDATA_POS                                   (0U)
#define CCI_APB_CCI_WDATA_LEN                                   (32U)
#define CCI_APB_CCI_WDATA_MSK                                   (((1ULL<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS)
#define CCI_APB_CCI_WDATA_UMSK                                  (~(((1ULL<<CCI_APB_CCI_WDATA_LEN)-1)<<CCI_APB_CCI_WDATA_POS))

/* 0xC : cci_rdata */
#define CCI_RDATA_OFFSET                                        (0xC)
#define CCI_APB_CCI_RDATA                                       CCI_APB_CCI_RDATA
#define CCI_APB_CCI_RDATA_POS                                   (0U)
#define CCI_APB_CCI_RDATA_LEN                                   (32U)
#define CCI_APB_CCI_RDATA_MSK                                   (((1ULL<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS)
#define CCI_APB_CCI_RDATA_UMSK                                  (~(((1ULL<<CCI_APB_CCI_RDATA_LEN)-1)<<CCI_APB_CCI_RDATA_POS))

/* 0x10 : cci_ctl */
#define CCI_CTL_OFFSET                                          (0x10)
#define CCI_WRITE_FLAG                                          CCI_WRITE_FLAG
#define CCI_WRITE_FLAG_POS                                      (0U)
#define CCI_WRITE_FLAG_LEN                                      (1U)
#define CCI_WRITE_FLAG_MSK                                      (((1ULL<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS)
#define CCI_WRITE_FLAG_UMSK                                     (~(((1ULL<<CCI_WRITE_FLAG_LEN)-1)<<CCI_WRITE_FLAG_POS))
#define CCI_READ_FLAG                                           CCI_READ_FLAG
#define CCI_READ_FLAG_POS                                       (1U)
#define CCI_READ_FLAG_LEN                                       (1U)
#define CCI_READ_FLAG_MSK                                       (((1ULL<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS)
#define CCI_READ_FLAG_UMSK                                      (~(((1ULL<<CCI_READ_FLAG_LEN)-1)<<CCI_READ_FLAG_POS))
#define CCI_AHB_STATE                                           CCI_AHB_STATE
#define CCI_AHB_STATE_POS                                       (2U)
#define CCI_AHB_STATE_LEN                                       (2U)
#define CCI_AHB_STATE_MSK                                       (((1ULL<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS)
#define CCI_AHB_STATE_UMSK                                      (~(((1ULL<<CCI_AHB_STATE_LEN)-1)<<CCI_AHB_STATE_POS))

/* 0x810 : wifi_pll_cfg0 */
#define CCI_WIFI_PLL_CFG0_OFFSET                                (0x810)
#define CCI_WIFIPLL_SDM_RSTB                                    CCI_WIFIPLL_SDM_RSTB
#define CCI_WIFIPLL_SDM_RSTB_POS                                (0U)
#define CCI_WIFIPLL_SDM_RSTB_LEN                                (1U)
#define CCI_WIFIPLL_SDM_RSTB_MSK                                (((1ULL<<CCI_WIFIPLL_SDM_RSTB_LEN)-1)<<CCI_WIFIPLL_SDM_RSTB_POS)
#define CCI_WIFIPLL_SDM_RSTB_UMSK                               (~(((1ULL<<CCI_WIFIPLL_SDM_RSTB_LEN)-1)<<CCI_WIFIPLL_SDM_RSTB_POS))
#define CCI_WIFIPLL_POSTDIV_RSTB                                CCI_WIFIPLL_POSTDIV_RSTB
#define CCI_WIFIPLL_POSTDIV_RSTB_POS                            (1U)
#define CCI_WIFIPLL_POSTDIV_RSTB_LEN                            (1U)
#define CCI_WIFIPLL_POSTDIV_RSTB_MSK                            (((1ULL<<CCI_WIFIPLL_POSTDIV_RSTB_LEN)-1)<<CCI_WIFIPLL_POSTDIV_RSTB_POS)
#define CCI_WIFIPLL_POSTDIV_RSTB_UMSK                           (~(((1ULL<<CCI_WIFIPLL_POSTDIV_RSTB_LEN)-1)<<CCI_WIFIPLL_POSTDIV_RSTB_POS))
#define CCI_WIFIPLL_FBDV_RSTB                                   CCI_WIFIPLL_FBDV_RSTB
#define CCI_WIFIPLL_FBDV_RSTB_POS                               (2U)
#define CCI_WIFIPLL_FBDV_RSTB_LEN                               (1U)
#define CCI_WIFIPLL_FBDV_RSTB_MSK                               (((1ULL<<CCI_WIFIPLL_FBDV_RSTB_LEN)-1)<<CCI_WIFIPLL_FBDV_RSTB_POS)
#define CCI_WIFIPLL_FBDV_RSTB_UMSK                              (~(((1ULL<<CCI_WIFIPLL_FBDV_RSTB_LEN)-1)<<CCI_WIFIPLL_FBDV_RSTB_POS))
#define CCI_WIFIPLL_REFDIV_RSTB                                 CCI_WIFIPLL_REFDIV_RSTB
#define CCI_WIFIPLL_REFDIV_RSTB_POS                             (3U)
#define CCI_WIFIPLL_REFDIV_RSTB_LEN                             (1U)
#define CCI_WIFIPLL_REFDIV_RSTB_MSK                             (((1ULL<<CCI_WIFIPLL_REFDIV_RSTB_LEN)-1)<<CCI_WIFIPLL_REFDIV_RSTB_POS)
#define CCI_WIFIPLL_REFDIV_RSTB_UMSK                            (~(((1ULL<<CCI_WIFIPLL_REFDIV_RSTB_LEN)-1)<<CCI_WIFIPLL_REFDIV_RSTB_POS))
#define CCI_PU_WIFIPLL_CLKTREE                                  CCI_PU_WIFIPLL_CLKTREE
#define CCI_PU_WIFIPLL_CLKTREE_POS                              (4U)
#define CCI_PU_WIFIPLL_CLKTREE_LEN                              (1U)
#define CCI_PU_WIFIPLL_CLKTREE_MSK                              (((1ULL<<CCI_PU_WIFIPLL_CLKTREE_LEN)-1)<<CCI_PU_WIFIPLL_CLKTREE_POS)
#define CCI_PU_WIFIPLL_CLKTREE_UMSK                             (~(((1ULL<<CCI_PU_WIFIPLL_CLKTREE_LEN)-1)<<CCI_PU_WIFIPLL_CLKTREE_POS))
#define CCI_PU_WIFIPLL_POSTDIV                                  CCI_PU_WIFIPLL_POSTDIV
#define CCI_PU_WIFIPLL_POSTDIV_POS                              (5U)
#define CCI_PU_WIFIPLL_POSTDIV_LEN                              (1U)
#define CCI_PU_WIFIPLL_POSTDIV_MSK                              (((1ULL<<CCI_PU_WIFIPLL_POSTDIV_LEN)-1)<<CCI_PU_WIFIPLL_POSTDIV_POS)
#define CCI_PU_WIFIPLL_POSTDIV_UMSK                             (~(((1ULL<<CCI_PU_WIFIPLL_POSTDIV_LEN)-1)<<CCI_PU_WIFIPLL_POSTDIV_POS))
#define CCI_PU_WIFIPLL_FBDV                                     CCI_PU_WIFIPLL_FBDV
#define CCI_PU_WIFIPLL_FBDV_POS                                 (6U)
#define CCI_PU_WIFIPLL_FBDV_LEN                                 (1U)
#define CCI_PU_WIFIPLL_FBDV_MSK                                 (((1ULL<<CCI_PU_WIFIPLL_FBDV_LEN)-1)<<CCI_PU_WIFIPLL_FBDV_POS)
#define CCI_PU_WIFIPLL_FBDV_UMSK                                (~(((1ULL<<CCI_PU_WIFIPLL_FBDV_LEN)-1)<<CCI_PU_WIFIPLL_FBDV_POS))
#define CCI_PU_WIFIPLL_CLAMP_OP                                 CCI_PU_WIFIPLL_CLAMP_OP
#define CCI_PU_WIFIPLL_CLAMP_OP_POS                             (7U)
#define CCI_PU_WIFIPLL_CLAMP_OP_LEN                             (1U)
#define CCI_PU_WIFIPLL_CLAMP_OP_MSK                             (((1ULL<<CCI_PU_WIFIPLL_CLAMP_OP_LEN)-1)<<CCI_PU_WIFIPLL_CLAMP_OP_POS)
#define CCI_PU_WIFIPLL_CLAMP_OP_UMSK                            (~(((1ULL<<CCI_PU_WIFIPLL_CLAMP_OP_LEN)-1)<<CCI_PU_WIFIPLL_CLAMP_OP_POS))
#define CCI_PU_WIFIPLL_PFD                                      CCI_PU_WIFIPLL_PFD
#define CCI_PU_WIFIPLL_PFD_POS                                  (8U)
#define CCI_PU_WIFIPLL_PFD_LEN                                  (1U)
#define CCI_PU_WIFIPLL_PFD_MSK                                  (((1ULL<<CCI_PU_WIFIPLL_PFD_LEN)-1)<<CCI_PU_WIFIPLL_PFD_POS)
#define CCI_PU_WIFIPLL_PFD_UMSK                                 (~(((1ULL<<CCI_PU_WIFIPLL_PFD_LEN)-1)<<CCI_PU_WIFIPLL_PFD_POS))
#define CCI_PU_WIFIPLL_CP                                       CCI_PU_WIFIPLL_CP
#define CCI_PU_WIFIPLL_CP_POS                                   (9U)
#define CCI_PU_WIFIPLL_CP_LEN                                   (1U)
#define CCI_PU_WIFIPLL_CP_MSK                                   (((1ULL<<CCI_PU_WIFIPLL_CP_LEN)-1)<<CCI_PU_WIFIPLL_CP_POS)
#define CCI_PU_WIFIPLL_CP_UMSK                                  (~(((1ULL<<CCI_PU_WIFIPLL_CP_LEN)-1)<<CCI_PU_WIFIPLL_CP_POS))
#define CCI_PU_WIFIPLL_SFREG                                    CCI_PU_WIFIPLL_SFREG
#define CCI_PU_WIFIPLL_SFREG_POS                                (10U)
#define CCI_PU_WIFIPLL_SFREG_LEN                                (1U)
#define CCI_PU_WIFIPLL_SFREG_MSK                                (((1ULL<<CCI_PU_WIFIPLL_SFREG_LEN)-1)<<CCI_PU_WIFIPLL_SFREG_POS)
#define CCI_PU_WIFIPLL_SFREG_UMSK                               (~(((1ULL<<CCI_PU_WIFIPLL_SFREG_LEN)-1)<<CCI_PU_WIFIPLL_SFREG_POS))
#define CCI_PU_WIFIPLL                                          CCI_PU_WIFIPLL
#define CCI_PU_WIFIPLL_POS                                      (11U)
#define CCI_PU_WIFIPLL_LEN                                      (1U)
#define CCI_PU_WIFIPLL_MSK                                      (((1ULL<<CCI_PU_WIFIPLL_LEN)-1)<<CCI_PU_WIFIPLL_POS)
#define CCI_PU_WIFIPLL_UMSK                                     (~(((1ULL<<CCI_PU_WIFIPLL_LEN)-1)<<CCI_PU_WIFIPLL_POS))

/* 0x814 : wifi_pll_cfg1 */
#define CCI_WIFI_PLL_CFG1_OFFSET                                (0x814)
#define CCI_WIFIPLL_POSTDIV                                     CCI_WIFIPLL_POSTDIV
#define CCI_WIFIPLL_POSTDIV_POS                                 (0U)
#define CCI_WIFIPLL_POSTDIV_LEN                                 (7U)
#define CCI_WIFIPLL_POSTDIV_MSK                                 (((1ULL<<CCI_WIFIPLL_POSTDIV_LEN)-1)<<CCI_WIFIPLL_POSTDIV_POS)
#define CCI_WIFIPLL_POSTDIV_UMSK                                (~(((1ULL<<CCI_WIFIPLL_POSTDIV_LEN)-1)<<CCI_WIFIPLL_POSTDIV_POS))
#define CCI_WIFIPLL_REFDIV_RATIO                                CCI_WIFIPLL_REFDIV_RATIO
#define CCI_WIFIPLL_REFDIV_RATIO_POS                            (8U)
#define CCI_WIFIPLL_REFDIV_RATIO_LEN                            (4U)
#define CCI_WIFIPLL_REFDIV_RATIO_MSK                            (((1ULL<<CCI_WIFIPLL_REFDIV_RATIO_LEN)-1)<<CCI_WIFIPLL_REFDIV_RATIO_POS)
#define CCI_WIFIPLL_REFDIV_RATIO_UMSK                           (~(((1ULL<<CCI_WIFIPLL_REFDIV_RATIO_LEN)-1)<<CCI_WIFIPLL_REFDIV_RATIO_POS))
#define CCI_WIFIPLL_REFCLK_SEL                                  CCI_WIFIPLL_REFCLK_SEL
#define CCI_WIFIPLL_REFCLK_SEL_POS                              (16U)
#define CCI_WIFIPLL_REFCLK_SEL_LEN                              (2U)
#define CCI_WIFIPLL_REFCLK_SEL_MSK                              (((1ULL<<CCI_WIFIPLL_REFCLK_SEL_LEN)-1)<<CCI_WIFIPLL_REFCLK_SEL_POS)
#define CCI_WIFIPLL_REFCLK_SEL_UMSK                             (~(((1ULL<<CCI_WIFIPLL_REFCLK_SEL_LEN)-1)<<CCI_WIFIPLL_REFCLK_SEL_POS))
#define CCI_WIFIPLL_VG11_SEL                                    CCI_WIFIPLL_VG11_SEL
#define CCI_WIFIPLL_VG11_SEL_POS                                (20U)
#define CCI_WIFIPLL_VG11_SEL_LEN                                (2U)
#define CCI_WIFIPLL_VG11_SEL_MSK                                (((1ULL<<CCI_WIFIPLL_VG11_SEL_LEN)-1)<<CCI_WIFIPLL_VG11_SEL_POS)
#define CCI_WIFIPLL_VG11_SEL_UMSK                               (~(((1ULL<<CCI_WIFIPLL_VG11_SEL_LEN)-1)<<CCI_WIFIPLL_VG11_SEL_POS))
#define CCI_WIFIPLL_VG13_SEL                                    CCI_WIFIPLL_VG13_SEL
#define CCI_WIFIPLL_VG13_SEL_POS                                (24U)
#define CCI_WIFIPLL_VG13_SEL_LEN                                (2U)
#define CCI_WIFIPLL_VG13_SEL_MSK                                (((1ULL<<CCI_WIFIPLL_VG13_SEL_LEN)-1)<<CCI_WIFIPLL_VG13_SEL_POS)
#define CCI_WIFIPLL_VG13_SEL_UMSK                               (~(((1ULL<<CCI_WIFIPLL_VG13_SEL_LEN)-1)<<CCI_WIFIPLL_VG13_SEL_POS))

/* 0x818 : wifi_pll_cfg2 */
#define CCI_WIFI_PLL_CFG2_OFFSET                                (0x818)
#define CCI_WIFIPLL_SEL_CP_BIAS                                 CCI_WIFIPLL_SEL_CP_BIAS
#define CCI_WIFIPLL_SEL_CP_BIAS_POS                             (0U)
#define CCI_WIFIPLL_SEL_CP_BIAS_LEN                             (1U)
#define CCI_WIFIPLL_SEL_CP_BIAS_MSK                             (((1ULL<<CCI_WIFIPLL_SEL_CP_BIAS_LEN)-1)<<CCI_WIFIPLL_SEL_CP_BIAS_POS)
#define CCI_WIFIPLL_SEL_CP_BIAS_UMSK                            (~(((1ULL<<CCI_WIFIPLL_SEL_CP_BIAS_LEN)-1)<<CCI_WIFIPLL_SEL_CP_BIAS_POS))
#define CCI_WIFIPLL_ICP_5U                                      CCI_WIFIPLL_ICP_5U
#define CCI_WIFIPLL_ICP_5U_POS                                  (4U)
#define CCI_WIFIPLL_ICP_5U_LEN                                  (2U)
#define CCI_WIFIPLL_ICP_5U_MSK                                  (((1ULL<<CCI_WIFIPLL_ICP_5U_LEN)-1)<<CCI_WIFIPLL_ICP_5U_POS)
#define CCI_WIFIPLL_ICP_5U_UMSK                                 (~(((1ULL<<CCI_WIFIPLL_ICP_5U_LEN)-1)<<CCI_WIFIPLL_ICP_5U_POS))
#define CCI_WIFIPLL_ICP_1U                                      CCI_WIFIPLL_ICP_1U
#define CCI_WIFIPLL_ICP_1U_POS                                  (6U)
#define CCI_WIFIPLL_ICP_1U_LEN                                  (2U)
#define CCI_WIFIPLL_ICP_1U_MSK                                  (((1ULL<<CCI_WIFIPLL_ICP_1U_LEN)-1)<<CCI_WIFIPLL_ICP_1U_POS)
#define CCI_WIFIPLL_ICP_1U_UMSK                                 (~(((1ULL<<CCI_WIFIPLL_ICP_1U_LEN)-1)<<CCI_WIFIPLL_ICP_1U_POS))
#define CCI_WIFIPLL_INT_FRAC_SW                                 CCI_WIFIPLL_INT_FRAC_SW
#define CCI_WIFIPLL_INT_FRAC_SW_POS                             (8U)
#define CCI_WIFIPLL_INT_FRAC_SW_LEN                             (1U)
#define CCI_WIFIPLL_INT_FRAC_SW_MSK                             (((1ULL<<CCI_WIFIPLL_INT_FRAC_SW_LEN)-1)<<CCI_WIFIPLL_INT_FRAC_SW_POS)
#define CCI_WIFIPLL_INT_FRAC_SW_UMSK                            (~(((1ULL<<CCI_WIFIPLL_INT_FRAC_SW_LEN)-1)<<CCI_WIFIPLL_INT_FRAC_SW_POS))
#define CCI_WIFIPLL_CP_STARTUP_EN                               CCI_WIFIPLL_CP_STARTUP_EN
#define CCI_WIFIPLL_CP_STARTUP_EN_POS                           (9U)
#define CCI_WIFIPLL_CP_STARTUP_EN_LEN                           (1U)
#define CCI_WIFIPLL_CP_STARTUP_EN_MSK                           (((1ULL<<CCI_WIFIPLL_CP_STARTUP_EN_LEN)-1)<<CCI_WIFIPLL_CP_STARTUP_EN_POS)
#define CCI_WIFIPLL_CP_STARTUP_EN_UMSK                          (~(((1ULL<<CCI_WIFIPLL_CP_STARTUP_EN_LEN)-1)<<CCI_WIFIPLL_CP_STARTUP_EN_POS))
#define CCI_WIFIPLL_CP_OPAMP_EN                                 CCI_WIFIPLL_CP_OPAMP_EN
#define CCI_WIFIPLL_CP_OPAMP_EN_POS                             (10U)
#define CCI_WIFIPLL_CP_OPAMP_EN_LEN                             (1U)
#define CCI_WIFIPLL_CP_OPAMP_EN_MSK                             (((1ULL<<CCI_WIFIPLL_CP_OPAMP_EN_LEN)-1)<<CCI_WIFIPLL_CP_OPAMP_EN_POS)
#define CCI_WIFIPLL_CP_OPAMP_EN_UMSK                            (~(((1ULL<<CCI_WIFIPLL_CP_OPAMP_EN_LEN)-1)<<CCI_WIFIPLL_CP_OPAMP_EN_POS))

/* 0x81C : wifi_pll_cfg3 */
#define CCI_WIFI_PLL_CFG3_OFFSET                                (0x81C)
#define CCI_WIFIPLL_C4_EN                                       CCI_WIFIPLL_C4_EN
#define CCI_WIFIPLL_C4_EN_POS                                   (0U)
#define CCI_WIFIPLL_C4_EN_LEN                                   (1U)
#define CCI_WIFIPLL_C4_EN_MSK                                   (((1ULL<<CCI_WIFIPLL_C4_EN_LEN)-1)<<CCI_WIFIPLL_C4_EN_POS)
#define CCI_WIFIPLL_C4_EN_UMSK                                  (~(((1ULL<<CCI_WIFIPLL_C4_EN_LEN)-1)<<CCI_WIFIPLL_C4_EN_POS))
#define CCI_WIFIPLL_R4                                          CCI_WIFIPLL_R4
#define CCI_WIFIPLL_R4_POS                                      (4U)
#define CCI_WIFIPLL_R4_LEN                                      (2U)
#define CCI_WIFIPLL_R4_MSK                                      (((1ULL<<CCI_WIFIPLL_R4_LEN)-1)<<CCI_WIFIPLL_R4_POS)
#define CCI_WIFIPLL_R4_UMSK                                     (~(((1ULL<<CCI_WIFIPLL_R4_LEN)-1)<<CCI_WIFIPLL_R4_POS))
#define CCI_WIFIPLL_R4_SHORT                                    CCI_WIFIPLL_R4_SHORT
#define CCI_WIFIPLL_R4_SHORT_POS                                (8U)
#define CCI_WIFIPLL_R4_SHORT_LEN                                (1U)
#define CCI_WIFIPLL_R4_SHORT_MSK                                (((1ULL<<CCI_WIFIPLL_R4_SHORT_LEN)-1)<<CCI_WIFIPLL_R4_SHORT_POS)
#define CCI_WIFIPLL_R4_SHORT_UMSK                               (~(((1ULL<<CCI_WIFIPLL_R4_SHORT_LEN)-1)<<CCI_WIFIPLL_R4_SHORT_POS))
#define CCI_WIFIPLL_C3                                          CCI_WIFIPLL_C3
#define CCI_WIFIPLL_C3_POS                                      (12U)
#define CCI_WIFIPLL_C3_LEN                                      (2U)
#define CCI_WIFIPLL_C3_MSK                                      (((1ULL<<CCI_WIFIPLL_C3_LEN)-1)<<CCI_WIFIPLL_C3_POS)
#define CCI_WIFIPLL_C3_UMSK                                     (~(((1ULL<<CCI_WIFIPLL_C3_LEN)-1)<<CCI_WIFIPLL_C3_POS))
#define CCI_WIFIPLL_CZ                                          CCI_WIFIPLL_CZ
#define CCI_WIFIPLL_CZ_POS                                      (14U)
#define CCI_WIFIPLL_CZ_LEN                                      (2U)
#define CCI_WIFIPLL_CZ_MSK                                      (((1ULL<<CCI_WIFIPLL_CZ_LEN)-1)<<CCI_WIFIPLL_CZ_POS)
#define CCI_WIFIPLL_CZ_UMSK                                     (~(((1ULL<<CCI_WIFIPLL_CZ_LEN)-1)<<CCI_WIFIPLL_CZ_POS))
#define CCI_WIFIPLL_RZ                                          CCI_WIFIPLL_RZ
#define CCI_WIFIPLL_RZ_POS                                      (16U)
#define CCI_WIFIPLL_RZ_LEN                                      (3U)
#define CCI_WIFIPLL_RZ_MSK                                      (((1ULL<<CCI_WIFIPLL_RZ_LEN)-1)<<CCI_WIFIPLL_RZ_POS)
#define CCI_WIFIPLL_RZ_UMSK                                     (~(((1ULL<<CCI_WIFIPLL_RZ_LEN)-1)<<CCI_WIFIPLL_RZ_POS))

/* 0x820 : wifi_pll_cfg4 */
#define CCI_WIFI_PLL_CFG4_OFFSET                                (0x820)
#define CCI_WIFIPLL_SEL_SAMPLE_CLK                              CCI_WIFIPLL_SEL_SAMPLE_CLK
#define CCI_WIFIPLL_SEL_SAMPLE_CLK_POS                          (0U)
#define CCI_WIFIPLL_SEL_SAMPLE_CLK_LEN                          (2U)
#define CCI_WIFIPLL_SEL_SAMPLE_CLK_MSK                          (((1ULL<<CCI_WIFIPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_WIFIPLL_SEL_SAMPLE_CLK_POS)
#define CCI_WIFIPLL_SEL_SAMPLE_CLK_UMSK                         (~(((1ULL<<CCI_WIFIPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_WIFIPLL_SEL_SAMPLE_CLK_POS))
#define CCI_WIFIPLL_SEL_FB_CLK                                  CCI_WIFIPLL_SEL_FB_CLK
#define CCI_WIFIPLL_SEL_FB_CLK_POS                              (4U)
#define CCI_WIFIPLL_SEL_FB_CLK_LEN                              (2U)
#define CCI_WIFIPLL_SEL_FB_CLK_MSK                              (((1ULL<<CCI_WIFIPLL_SEL_FB_CLK_LEN)-1)<<CCI_WIFIPLL_SEL_FB_CLK_POS)
#define CCI_WIFIPLL_SEL_FB_CLK_UMSK                             (~(((1ULL<<CCI_WIFIPLL_SEL_FB_CLK_LEN)-1)<<CCI_WIFIPLL_SEL_FB_CLK_POS))
#define CCI_WIFIPLL_SDMCLK_SEL                                  CCI_WIFIPLL_SDMCLK_SEL
#define CCI_WIFIPLL_SDMCLK_SEL_POS                              (8U)
#define CCI_WIFIPLL_SDMCLK_SEL_LEN                              (1U)
#define CCI_WIFIPLL_SDMCLK_SEL_MSK                              (((1ULL<<CCI_WIFIPLL_SDMCLK_SEL_LEN)-1)<<CCI_WIFIPLL_SDMCLK_SEL_POS)
#define CCI_WIFIPLL_SDMCLK_SEL_UMSK                             (~(((1ULL<<CCI_WIFIPLL_SDMCLK_SEL_LEN)-1)<<CCI_WIFIPLL_SDMCLK_SEL_POS))

/* 0x824 : wifi_pll_cfg5 */
#define CCI_WIFI_PLL_CFG5_OFFSET                                (0x824)
#define CCI_WIFIPLL_VCO_SPEED                                   CCI_WIFIPLL_VCO_SPEED
#define CCI_WIFIPLL_VCO_SPEED_POS                               (0U)
#define CCI_WIFIPLL_VCO_SPEED_LEN                               (3U)
#define CCI_WIFIPLL_VCO_SPEED_MSK                               (((1ULL<<CCI_WIFIPLL_VCO_SPEED_LEN)-1)<<CCI_WIFIPLL_VCO_SPEED_POS)
#define CCI_WIFIPLL_VCO_SPEED_UMSK                              (~(((1ULL<<CCI_WIFIPLL_VCO_SPEED_LEN)-1)<<CCI_WIFIPLL_VCO_SPEED_POS))
#define CCI_WIFIPLL_VCO_DIV3_EN                                 CCI_WIFIPLL_VCO_DIV3_EN
#define CCI_WIFIPLL_VCO_DIV3_EN_POS                             (3U)
#define CCI_WIFIPLL_VCO_DIV3_EN_LEN                             (1U)
#define CCI_WIFIPLL_VCO_DIV3_EN_MSK                             (((1ULL<<CCI_WIFIPLL_VCO_DIV3_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV3_EN_POS)
#define CCI_WIFIPLL_VCO_DIV3_EN_UMSK                            (~(((1ULL<<CCI_WIFIPLL_VCO_DIV3_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV3_EN_POS))
#define CCI_WIFIPLL_VCO_DIV2_EN                                 CCI_WIFIPLL_VCO_DIV2_EN
#define CCI_WIFIPLL_VCO_DIV2_EN_POS                             (4U)
#define CCI_WIFIPLL_VCO_DIV2_EN_LEN                             (1U)
#define CCI_WIFIPLL_VCO_DIV2_EN_MSK                             (((1ULL<<CCI_WIFIPLL_VCO_DIV2_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV2_EN_POS)
#define CCI_WIFIPLL_VCO_DIV2_EN_UMSK                            (~(((1ULL<<CCI_WIFIPLL_VCO_DIV2_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV2_EN_POS))
#define CCI_WIFIPLL_VCO_DIV1_EN                                 CCI_WIFIPLL_VCO_DIV1_EN
#define CCI_WIFIPLL_VCO_DIV1_EN_POS                             (5U)
#define CCI_WIFIPLL_VCO_DIV1_EN_LEN                             (1U)
#define CCI_WIFIPLL_VCO_DIV1_EN_MSK                             (((1ULL<<CCI_WIFIPLL_VCO_DIV1_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV1_EN_POS)
#define CCI_WIFIPLL_VCO_DIV1_EN_UMSK                            (~(((1ULL<<CCI_WIFIPLL_VCO_DIV1_EN_LEN)-1)<<CCI_WIFIPLL_VCO_DIV1_EN_POS))

/* 0x828 : wifi_pll_cfg6 */
#define CCI_WIFI_PLL_CFG6_OFFSET                                (0x828)
#define CCI_WIFIPLL_SDMIN                                       CCI_WIFIPLL_SDMIN
#define CCI_WIFIPLL_SDMIN_POS                                   (0U)
#define CCI_WIFIPLL_SDMIN_LEN                                   (26U)
#define CCI_WIFIPLL_SDMIN_MSK                                   (((1ULL<<CCI_WIFIPLL_SDMIN_LEN)-1)<<CCI_WIFIPLL_SDMIN_POS)
#define CCI_WIFIPLL_SDMIN_UMSK                                  (~(((1ULL<<CCI_WIFIPLL_SDMIN_LEN)-1)<<CCI_WIFIPLL_SDMIN_POS))
#define CCI_WIFIPLL_SDM_BYPASS                                  CCI_WIFIPLL_SDM_BYPASS
#define CCI_WIFIPLL_SDM_BYPASS_POS                              (26U)
#define CCI_WIFIPLL_SDM_BYPASS_LEN                              (1U)
#define CCI_WIFIPLL_SDM_BYPASS_MSK                              (((1ULL<<CCI_WIFIPLL_SDM_BYPASS_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_POS)
#define CCI_WIFIPLL_SDM_BYPASS_UMSK                             (~(((1ULL<<CCI_WIFIPLL_SDM_BYPASS_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_POS))
#define CCI_WIFIPLL_SDM_BYPASS_HW                               CCI_WIFIPLL_SDM_BYPASS_HW
#define CCI_WIFIPLL_SDM_BYPASS_HW_POS                           (30U)
#define CCI_WIFIPLL_SDM_BYPASS_HW_LEN                           (1U)
#define CCI_WIFIPLL_SDM_BYPASS_HW_MSK                           (((1ULL<<CCI_WIFIPLL_SDM_BYPASS_HW_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_HW_POS)
#define CCI_WIFIPLL_SDM_BYPASS_HW_UMSK                          (~(((1ULL<<CCI_WIFIPLL_SDM_BYPASS_HW_LEN)-1)<<CCI_WIFIPLL_SDM_BYPASS_HW_POS))
#define CCI_WIFIPLL_SDM_CTRL_HW                                 CCI_WIFIPLL_SDM_CTRL_HW
#define CCI_WIFIPLL_SDM_CTRL_HW_POS                             (31U)
#define CCI_WIFIPLL_SDM_CTRL_HW_LEN                             (1U)
#define CCI_WIFIPLL_SDM_CTRL_HW_MSK                             (((1ULL<<CCI_WIFIPLL_SDM_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_SDM_CTRL_HW_POS)
#define CCI_WIFIPLL_SDM_CTRL_HW_UMSK                            (~(((1ULL<<CCI_WIFIPLL_SDM_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_SDM_CTRL_HW_POS))

/* 0x82C : wifi_pll_cfg7 */
#define CCI_WIFI_PLL_CFG7_OFFSET                                (0x82C)
#define CCI_WIFIPLL_SDM_ORDER_SEL                               CCI_WIFIPLL_SDM_ORDER_SEL
#define CCI_WIFIPLL_SDM_ORDER_SEL_POS                           (0U)
#define CCI_WIFIPLL_SDM_ORDER_SEL_LEN                           (2U)
#define CCI_WIFIPLL_SDM_ORDER_SEL_MSK                           (((1ULL<<CCI_WIFIPLL_SDM_ORDER_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_ORDER_SEL_POS)
#define CCI_WIFIPLL_SDM_ORDER_SEL_UMSK                          (~(((1ULL<<CCI_WIFIPLL_SDM_ORDER_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_ORDER_SEL_POS))
#define CCI_WIFIPLL_SDM_NOI_PRBS_SEL                            CCI_WIFIPLL_SDM_NOI_PRBS_SEL
#define CCI_WIFIPLL_SDM_NOI_PRBS_SEL_POS                        (4U)
#define CCI_WIFIPLL_SDM_NOI_PRBS_SEL_LEN                        (2U)
#define CCI_WIFIPLL_SDM_NOI_PRBS_SEL_MSK                        (((1ULL<<CCI_WIFIPLL_SDM_NOI_PRBS_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_PRBS_SEL_POS)
#define CCI_WIFIPLL_SDM_NOI_PRBS_SEL_UMSK                       (~(((1ULL<<CCI_WIFIPLL_SDM_NOI_PRBS_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_PRBS_SEL_POS))
#define CCI_WIFIPLL_SDM_NOI_PRBS_EN                             CCI_WIFIPLL_SDM_NOI_PRBS_EN
#define CCI_WIFIPLL_SDM_NOI_PRBS_EN_POS                         (8U)
#define CCI_WIFIPLL_SDM_NOI_PRBS_EN_LEN                         (1U)
#define CCI_WIFIPLL_SDM_NOI_PRBS_EN_MSK                         (((1ULL<<CCI_WIFIPLL_SDM_NOI_PRBS_EN_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_PRBS_EN_POS)
#define CCI_WIFIPLL_SDM_NOI_PRBS_EN_UMSK                        (~(((1ULL<<CCI_WIFIPLL_SDM_NOI_PRBS_EN_LEN)-1)<<CCI_WIFIPLL_SDM_NOI_PRBS_EN_POS))
#define CCI_WIFIPLL_SDM_SIG_PRBS_SEL                            CCI_WIFIPLL_SDM_SIG_PRBS_SEL
#define CCI_WIFIPLL_SDM_SIG_PRBS_SEL_POS                        (12U)
#define CCI_WIFIPLL_SDM_SIG_PRBS_SEL_LEN                        (2U)
#define CCI_WIFIPLL_SDM_SIG_PRBS_SEL_MSK                        (((1ULL<<CCI_WIFIPLL_SDM_SIG_PRBS_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_PRBS_SEL_POS)
#define CCI_WIFIPLL_SDM_SIG_PRBS_SEL_UMSK                       (~(((1ULL<<CCI_WIFIPLL_SDM_SIG_PRBS_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_PRBS_SEL_POS))
#define CCI_WIFIPLL_SDM_SIG_DITH_SEL                            CCI_WIFIPLL_SDM_SIG_DITH_SEL
#define CCI_WIFIPLL_SDM_SIG_DITH_SEL_POS                        (16U)
#define CCI_WIFIPLL_SDM_SIG_DITH_SEL_LEN                        (2U)
#define CCI_WIFIPLL_SDM_SIG_DITH_SEL_MSK                        (((1ULL<<CCI_WIFIPLL_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_DITH_SEL_POS)
#define CCI_WIFIPLL_SDM_SIG_DITH_SEL_UMSK                       (~(((1ULL<<CCI_WIFIPLL_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_WIFIPLL_SDM_SIG_DITH_SEL_POS))

/* 0x830 : wifi_pll_cfg8 */
#define CCI_WIFI_PLL_CFG8_OFFSET                                (0x830)
#define CCI_WIFIPLL_EN_RF_DIV3                                  CCI_WIFIPLL_EN_RF_DIV3
#define CCI_WIFIPLL_EN_RF_DIV3_POS                              (0U)
#define CCI_WIFIPLL_EN_RF_DIV3_LEN                              (1U)
#define CCI_WIFIPLL_EN_RF_DIV3_MSK                              (((1ULL<<CCI_WIFIPLL_EN_RF_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_POS)
#define CCI_WIFIPLL_EN_RF_DIV3_UMSK                             (~(((1ULL<<CCI_WIFIPLL_EN_RF_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_POS))
#define CCI_WIFIPLL_EN_RF_DIV6                                  CCI_WIFIPLL_EN_RF_DIV6
#define CCI_WIFIPLL_EN_RF_DIV6_POS                              (1U)
#define CCI_WIFIPLL_EN_RF_DIV6_LEN                              (1U)
#define CCI_WIFIPLL_EN_RF_DIV6_MSK                              (((1ULL<<CCI_WIFIPLL_EN_RF_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV6_POS)
#define CCI_WIFIPLL_EN_RF_DIV6_UMSK                             (~(((1ULL<<CCI_WIFIPLL_EN_RF_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV6_POS))
#define CCI_WIFIPLL_SEL_DIV3_DIV6                               CCI_WIFIPLL_SEL_DIV3_DIV6
#define CCI_WIFIPLL_SEL_DIV3_DIV6_POS                           (2U)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_LEN                           (1U)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_MSK                           (((1ULL<<CCI_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<CCI_WIFIPLL_SEL_DIV3_DIV6_POS)
#define CCI_WIFIPLL_SEL_DIV3_DIV6_UMSK                          (~(((1ULL<<CCI_WIFIPLL_SEL_DIV3_DIV6_LEN)-1)<<CCI_WIFIPLL_SEL_DIV3_DIV6_POS))
#define CCI_WIFIPLL_SEL_DIV6_DIV12                              CCI_WIFIPLL_SEL_DIV6_DIV12
#define CCI_WIFIPLL_SEL_DIV6_DIV12_POS                          (3U)
#define CCI_WIFIPLL_SEL_DIV6_DIV12_LEN                          (1U)
#define CCI_WIFIPLL_SEL_DIV6_DIV12_MSK                          (((1ULL<<CCI_WIFIPLL_SEL_DIV6_DIV12_LEN)-1)<<CCI_WIFIPLL_SEL_DIV6_DIV12_POS)
#define CCI_WIFIPLL_SEL_DIV6_DIV12_UMSK                         (~(((1ULL<<CCI_WIFIPLL_SEL_DIV6_DIV12_LEN)-1)<<CCI_WIFIPLL_SEL_DIV6_DIV12_POS))
#define CCI_WIFIPLL_EN_DIV3                                     CCI_WIFIPLL_EN_DIV3
#define CCI_WIFIPLL_EN_DIV3_POS                                 (4U)
#define CCI_WIFIPLL_EN_DIV3_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV3_MSK                                 (((1ULL<<CCI_WIFIPLL_EN_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_DIV3_POS)
#define CCI_WIFIPLL_EN_DIV3_UMSK                                (~(((1ULL<<CCI_WIFIPLL_EN_DIV3_LEN)-1)<<CCI_WIFIPLL_EN_DIV3_POS))
#define CCI_WIFIPLL_EN_DIV4                                     CCI_WIFIPLL_EN_DIV4
#define CCI_WIFIPLL_EN_DIV4_POS                                 (5U)
#define CCI_WIFIPLL_EN_DIV4_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV4_MSK                                 (((1ULL<<CCI_WIFIPLL_EN_DIV4_LEN)-1)<<CCI_WIFIPLL_EN_DIV4_POS)
#define CCI_WIFIPLL_EN_DIV4_UMSK                                (~(((1ULL<<CCI_WIFIPLL_EN_DIV4_LEN)-1)<<CCI_WIFIPLL_EN_DIV4_POS))
#define CCI_WIFIPLL_EN_DIV5                                     CCI_WIFIPLL_EN_DIV5
#define CCI_WIFIPLL_EN_DIV5_POS                                 (6U)
#define CCI_WIFIPLL_EN_DIV5_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV5_MSK                                 (((1ULL<<CCI_WIFIPLL_EN_DIV5_LEN)-1)<<CCI_WIFIPLL_EN_DIV5_POS)
#define CCI_WIFIPLL_EN_DIV5_UMSK                                (~(((1ULL<<CCI_WIFIPLL_EN_DIV5_LEN)-1)<<CCI_WIFIPLL_EN_DIV5_POS))
#define CCI_WIFIPLL_EN_DIV6                                     CCI_WIFIPLL_EN_DIV6
#define CCI_WIFIPLL_EN_DIV6_POS                                 (7U)
#define CCI_WIFIPLL_EN_DIV6_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV6_MSK                                 (((1ULL<<CCI_WIFIPLL_EN_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_DIV6_POS)
#define CCI_WIFIPLL_EN_DIV6_UMSK                                (~(((1ULL<<CCI_WIFIPLL_EN_DIV6_LEN)-1)<<CCI_WIFIPLL_EN_DIV6_POS))
#define CCI_WIFIPLL_EN_DIV8                                     CCI_WIFIPLL_EN_DIV8
#define CCI_WIFIPLL_EN_DIV8_POS                                 (8U)
#define CCI_WIFIPLL_EN_DIV8_LEN                                 (1U)
#define CCI_WIFIPLL_EN_DIV8_MSK                                 (((1ULL<<CCI_WIFIPLL_EN_DIV8_LEN)-1)<<CCI_WIFIPLL_EN_DIV8_POS)
#define CCI_WIFIPLL_EN_DIV8_UMSK                                (~(((1ULL<<CCI_WIFIPLL_EN_DIV8_LEN)-1)<<CCI_WIFIPLL_EN_DIV8_POS))
#define CCI_WIFIPLL_EN_DIV10                                    CCI_WIFIPLL_EN_DIV10
#define CCI_WIFIPLL_EN_DIV10_POS                                (9U)
#define CCI_WIFIPLL_EN_DIV10_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV10_MSK                                (((1ULL<<CCI_WIFIPLL_EN_DIV10_LEN)-1)<<CCI_WIFIPLL_EN_DIV10_POS)
#define CCI_WIFIPLL_EN_DIV10_UMSK                               (~(((1ULL<<CCI_WIFIPLL_EN_DIV10_LEN)-1)<<CCI_WIFIPLL_EN_DIV10_POS))
#define CCI_WIFIPLL_EN_DIV12                                    CCI_WIFIPLL_EN_DIV12
#define CCI_WIFIPLL_EN_DIV12_POS                                (10U)
#define CCI_WIFIPLL_EN_DIV12_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV12_MSK                                (((1ULL<<CCI_WIFIPLL_EN_DIV12_LEN)-1)<<CCI_WIFIPLL_EN_DIV12_POS)
#define CCI_WIFIPLL_EN_DIV12_UMSK                               (~(((1ULL<<CCI_WIFIPLL_EN_DIV12_LEN)-1)<<CCI_WIFIPLL_EN_DIV12_POS))
#define CCI_WIFIPLL_EN_DIV20                                    CCI_WIFIPLL_EN_DIV20
#define CCI_WIFIPLL_EN_DIV20_POS                                (11U)
#define CCI_WIFIPLL_EN_DIV20_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV20_MSK                                (((1ULL<<CCI_WIFIPLL_EN_DIV20_LEN)-1)<<CCI_WIFIPLL_EN_DIV20_POS)
#define CCI_WIFIPLL_EN_DIV20_UMSK                               (~(((1ULL<<CCI_WIFIPLL_EN_DIV20_LEN)-1)<<CCI_WIFIPLL_EN_DIV20_POS))
#define CCI_WIFIPLL_EN_DIV30                                    CCI_WIFIPLL_EN_DIV30
#define CCI_WIFIPLL_EN_DIV30_POS                                (12U)
#define CCI_WIFIPLL_EN_DIV30_LEN                                (1U)
#define CCI_WIFIPLL_EN_DIV30_MSK                                (((1ULL<<CCI_WIFIPLL_EN_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_DIV30_POS)
#define CCI_WIFIPLL_EN_DIV30_UMSK                               (~(((1ULL<<CCI_WIFIPLL_EN_DIV30_LEN)-1)<<CCI_WIFIPLL_EN_DIV30_POS))
#define CCI_WIFIPLL_EN_RF_DIV3_HW                               CCI_WIFIPLL_EN_RF_DIV3_HW
#define CCI_WIFIPLL_EN_RF_DIV3_HW_POS                           (20U)
#define CCI_WIFIPLL_EN_RF_DIV3_HW_LEN                           (1U)
#define CCI_WIFIPLL_EN_RF_DIV3_HW_MSK                           (((1ULL<<CCI_WIFIPLL_EN_RF_DIV3_HW_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_HW_POS)
#define CCI_WIFIPLL_EN_RF_DIV3_HW_UMSK                          (~(((1ULL<<CCI_WIFIPLL_EN_RF_DIV3_HW_LEN)-1)<<CCI_WIFIPLL_EN_RF_DIV3_HW_POS))
#define CCI_WIFIPLL_EN_CTRL_HW                                  CCI_WIFIPLL_EN_CTRL_HW
#define CCI_WIFIPLL_EN_CTRL_HW_POS                              (31U)
#define CCI_WIFIPLL_EN_CTRL_HW_LEN                              (1U)
#define CCI_WIFIPLL_EN_CTRL_HW_MSK                              (((1ULL<<CCI_WIFIPLL_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_EN_CTRL_HW_POS)
#define CCI_WIFIPLL_EN_CTRL_HW_UMSK                             (~(((1ULL<<CCI_WIFIPLL_EN_CTRL_HW_LEN)-1)<<CCI_WIFIPLL_EN_CTRL_HW_POS))

/* 0x834 : wifi_pll_cfg9 */
#define CCI_WIFI_PLL_CFG9_OFFSET                                (0x834)
#define CCI_WIFIPLL_DC_TP_OUT_EN                                CCI_WIFIPLL_DC_TP_OUT_EN
#define CCI_WIFIPLL_DC_TP_OUT_EN_POS                            (0U)
#define CCI_WIFIPLL_DC_TP_OUT_EN_LEN                            (1U)
#define CCI_WIFIPLL_DC_TP_OUT_EN_MSK                            (((1ULL<<CCI_WIFIPLL_DC_TP_OUT_EN_LEN)-1)<<CCI_WIFIPLL_DC_TP_OUT_EN_POS)
#define CCI_WIFIPLL_DC_TP_OUT_EN_UMSK                           (~(((1ULL<<CCI_WIFIPLL_DC_TP_OUT_EN_LEN)-1)<<CCI_WIFIPLL_DC_TP_OUT_EN_POS))
#define CCI_TEN_WIFIPLL                                         CCI_TEN_WIFIPLL
#define CCI_TEN_WIFIPLL_POS                                     (1U)
#define CCI_TEN_WIFIPLL_LEN                                     (1U)
#define CCI_TEN_WIFIPLL_MSK                                     (((1ULL<<CCI_TEN_WIFIPLL_LEN)-1)<<CCI_TEN_WIFIPLL_POS)
#define CCI_TEN_WIFIPLL_UMSK                                    (~(((1ULL<<CCI_TEN_WIFIPLL_LEN)-1)<<CCI_TEN_WIFIPLL_POS))
#define CCI_TEN_WIFIPLL_SFREG                                   CCI_TEN_WIFIPLL_SFREG
#define CCI_TEN_WIFIPLL_SFREG_POS                               (2U)
#define CCI_TEN_WIFIPLL_SFREG_LEN                               (1U)
#define CCI_TEN_WIFIPLL_SFREG_MSK                               (((1ULL<<CCI_TEN_WIFIPLL_SFREG_LEN)-1)<<CCI_TEN_WIFIPLL_SFREG_POS)
#define CCI_TEN_WIFIPLL_SFREG_UMSK                              (~(((1ULL<<CCI_TEN_WIFIPLL_SFREG_LEN)-1)<<CCI_TEN_WIFIPLL_SFREG_POS))
#define CCI_DTEN_WIFIPLL_FIN                                    CCI_DTEN_WIFIPLL_FIN
#define CCI_DTEN_WIFIPLL_FIN_POS                                (4U)
#define CCI_DTEN_WIFIPLL_FIN_LEN                                (1U)
#define CCI_DTEN_WIFIPLL_FIN_MSK                                (((1ULL<<CCI_DTEN_WIFIPLL_FIN_LEN)-1)<<CCI_DTEN_WIFIPLL_FIN_POS)
#define CCI_DTEN_WIFIPLL_FIN_UMSK                               (~(((1ULL<<CCI_DTEN_WIFIPLL_FIN_LEN)-1)<<CCI_DTEN_WIFIPLL_FIN_POS))
#define CCI_DTEN_WIFIPLL_FREF                                   CCI_DTEN_WIFIPLL_FREF
#define CCI_DTEN_WIFIPLL_FREF_POS                               (5U)
#define CCI_DTEN_WIFIPLL_FREF_LEN                               (1U)
#define CCI_DTEN_WIFIPLL_FREF_MSK                               (((1ULL<<CCI_DTEN_WIFIPLL_FREF_LEN)-1)<<CCI_DTEN_WIFIPLL_FREF_POS)
#define CCI_DTEN_WIFIPLL_FREF_UMSK                              (~(((1ULL<<CCI_DTEN_WIFIPLL_FREF_LEN)-1)<<CCI_DTEN_WIFIPLL_FREF_POS))
#define CCI_DTEN_WIFIPLL_FSDM                                   CCI_DTEN_WIFIPLL_FSDM
#define CCI_DTEN_WIFIPLL_FSDM_POS                               (6U)
#define CCI_DTEN_WIFIPLL_FSDM_LEN                               (1U)
#define CCI_DTEN_WIFIPLL_FSDM_MSK                               (((1ULL<<CCI_DTEN_WIFIPLL_FSDM_LEN)-1)<<CCI_DTEN_WIFIPLL_FSDM_POS)
#define CCI_DTEN_WIFIPLL_FSDM_UMSK                              (~(((1ULL<<CCI_DTEN_WIFIPLL_FSDM_LEN)-1)<<CCI_DTEN_WIFIPLL_FSDM_POS))
#define CCI_DTEN_WIFIPLL_DIV30                                  CCI_DTEN_WIFIPLL_DIV30
#define CCI_DTEN_WIFIPLL_DIV30_POS                              (7U)
#define CCI_DTEN_WIFIPLL_DIV30_LEN                              (1U)
#define CCI_DTEN_WIFIPLL_DIV30_MSK                              (((1ULL<<CCI_DTEN_WIFIPLL_DIV30_LEN)-1)<<CCI_DTEN_WIFIPLL_DIV30_POS)
#define CCI_DTEN_WIFIPLL_DIV30_UMSK                             (~(((1ULL<<CCI_DTEN_WIFIPLL_DIV30_LEN)-1)<<CCI_DTEN_WIFIPLL_DIV30_POS))
#define CCI_DTEN_WIFIPLL_DIV10                                  CCI_DTEN_WIFIPLL_DIV10
#define CCI_DTEN_WIFIPLL_DIV10_POS                              (8U)
#define CCI_DTEN_WIFIPLL_DIV10_LEN                              (1U)
#define CCI_DTEN_WIFIPLL_DIV10_MSK                              (((1ULL<<CCI_DTEN_WIFIPLL_DIV10_LEN)-1)<<CCI_DTEN_WIFIPLL_DIV10_POS)
#define CCI_DTEN_WIFIPLL_DIV10_UMSK                             (~(((1ULL<<CCI_DTEN_WIFIPLL_DIV10_LEN)-1)<<CCI_DTEN_WIFIPLL_DIV10_POS))
#define CCI_DTEN_WIFIPLL_POSTDIV_CLK                            CCI_DTEN_WIFIPLL_POSTDIV_CLK
#define CCI_DTEN_WIFIPLL_POSTDIV_CLK_POS                        (9U)
#define CCI_DTEN_WIFIPLL_POSTDIV_CLK_LEN                        (1U)
#define CCI_DTEN_WIFIPLL_POSTDIV_CLK_MSK                        (((1ULL<<CCI_DTEN_WIFIPLL_POSTDIV_CLK_LEN)-1)<<CCI_DTEN_WIFIPLL_POSTDIV_CLK_POS)
#define CCI_DTEN_WIFIPLL_POSTDIV_CLK_UMSK                       (~(((1ULL<<CCI_DTEN_WIFIPLL_POSTDIV_CLK_LEN)-1)<<CCI_DTEN_WIFIPLL_POSTDIV_CLK_POS))
#define CCI_DTEN_USBPLL_PCLK                                    CCI_DTEN_USBPLL_PCLK
#define CCI_DTEN_USBPLL_PCLK_POS                                (10U)
#define CCI_DTEN_USBPLL_PCLK_LEN                                (1U)
#define CCI_DTEN_USBPLL_PCLK_MSK                                (((1ULL<<CCI_DTEN_USBPLL_PCLK_LEN)-1)<<CCI_DTEN_USBPLL_PCLK_POS)
#define CCI_DTEN_USBPLL_PCLK_UMSK                               (~(((1ULL<<CCI_DTEN_USBPLL_PCLK_LEN)-1)<<CCI_DTEN_USBPLL_PCLK_POS))
#define CCI_DTEN_USBPLL_CLKOUT                                  CCI_DTEN_USBPLL_CLKOUT
#define CCI_DTEN_USBPLL_CLKOUT_POS                              (11U)
#define CCI_DTEN_USBPLL_CLKOUT_LEN                              (1U)
#define CCI_DTEN_USBPLL_CLKOUT_MSK                              (((1ULL<<CCI_DTEN_USBPLL_CLKOUT_LEN)-1)<<CCI_DTEN_USBPLL_CLKOUT_POS)
#define CCI_DTEN_USBPLL_CLKOUT_UMSK                             (~(((1ULL<<CCI_DTEN_USBPLL_CLKOUT_LEN)-1)<<CCI_DTEN_USBPLL_CLKOUT_POS))
#define CCI_DTEN_SSCDIV_PCLK                                    CCI_DTEN_SSCDIV_PCLK
#define CCI_DTEN_SSCDIV_PCLK_POS                                (12U)
#define CCI_DTEN_SSCDIV_PCLK_LEN                                (1U)
#define CCI_DTEN_SSCDIV_PCLK_MSK                                (((1ULL<<CCI_DTEN_SSCDIV_PCLK_LEN)-1)<<CCI_DTEN_SSCDIV_PCLK_POS)
#define CCI_DTEN_SSCDIV_PCLK_UMSK                               (~(((1ULL<<CCI_DTEN_SSCDIV_PCLK_LEN)-1)<<CCI_DTEN_SSCDIV_PCLK_POS))
#define CCI_DTEN_SSCDIV_CLKOUT                                  CCI_DTEN_SSCDIV_CLKOUT
#define CCI_DTEN_SSCDIV_CLKOUT_POS                              (13U)
#define CCI_DTEN_SSCDIV_CLKOUT_LEN                              (1U)
#define CCI_DTEN_SSCDIV_CLKOUT_MSK                              (((1ULL<<CCI_DTEN_SSCDIV_CLKOUT_LEN)-1)<<CCI_DTEN_SSCDIV_CLKOUT_POS)
#define CCI_DTEN_SSCDIV_CLKOUT_UMSK                             (~(((1ULL<<CCI_DTEN_SSCDIV_CLKOUT_LEN)-1)<<CCI_DTEN_SSCDIV_CLKOUT_POS))
#define CCI_DTEST_PULLDOWN                                      CCI_DTEST_PULLDOWN
#define CCI_DTEST_PULLDOWN_POS                                  (14U)
#define CCI_DTEST_PULLDOWN_LEN                                  (1U)
#define CCI_DTEST_PULLDOWN_MSK                                  (((1ULL<<CCI_DTEST_PULLDOWN_LEN)-1)<<CCI_DTEST_PULLDOWN_POS)
#define CCI_DTEST_PULLDOWN_UMSK                                 (~(((1ULL<<CCI_DTEST_PULLDOWN_LEN)-1)<<CCI_DTEST_PULLDOWN_POS))

/* 0x838 : wifi_pll_cfg10 */
#define CCI_WIFI_PLL_CFG10_OFFSET                               (0x838)
#define CCI_USBPLL_SDMIN                                        CCI_USBPLL_SDMIN
#define CCI_USBPLL_SDMIN_POS                                    (0U)
#define CCI_USBPLL_SDMIN_LEN                                    (19U)
#define CCI_USBPLL_SDMIN_MSK                                    (((1ULL<<CCI_USBPLL_SDMIN_LEN)-1)<<CCI_USBPLL_SDMIN_POS)
#define CCI_USBPLL_SDMIN_UMSK                                   (~(((1ULL<<CCI_USBPLL_SDMIN_LEN)-1)<<CCI_USBPLL_SDMIN_POS))
#define CCI_USBPLL_SDM_BYPASS                                   CCI_USBPLL_SDM_BYPASS
#define CCI_USBPLL_SDM_BYPASS_POS                               (20U)
#define CCI_USBPLL_SDM_BYPASS_LEN                               (1U)
#define CCI_USBPLL_SDM_BYPASS_MSK                               (((1ULL<<CCI_USBPLL_SDM_BYPASS_LEN)-1)<<CCI_USBPLL_SDM_BYPASS_POS)
#define CCI_USBPLL_SDM_BYPASS_UMSK                              (~(((1ULL<<CCI_USBPLL_SDM_BYPASS_LEN)-1)<<CCI_USBPLL_SDM_BYPASS_POS))
#define CCI_USBPLL_SDM_ORDER_SEL                                CCI_USBPLL_SDM_ORDER_SEL
#define CCI_USBPLL_SDM_ORDER_SEL_POS                            (21U)
#define CCI_USBPLL_SDM_ORDER_SEL_LEN                            (1U)
#define CCI_USBPLL_SDM_ORDER_SEL_MSK                            (((1ULL<<CCI_USBPLL_SDM_ORDER_SEL_LEN)-1)<<CCI_USBPLL_SDM_ORDER_SEL_POS)
#define CCI_USBPLL_SDM_ORDER_SEL_UMSK                           (~(((1ULL<<CCI_USBPLL_SDM_ORDER_SEL_LEN)-1)<<CCI_USBPLL_SDM_ORDER_SEL_POS))
#define CCI_USBPLL_SDM_SIG_DITH_SEL                             CCI_USBPLL_SDM_SIG_DITH_SEL
#define CCI_USBPLL_SDM_SIG_DITH_SEL_POS                         (22U)
#define CCI_USBPLL_SDM_SIG_DITH_SEL_LEN                         (2U)
#define CCI_USBPLL_SDM_SIG_DITH_SEL_MSK                         (((1ULL<<CCI_USBPLL_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_USBPLL_SDM_SIG_DITH_SEL_POS)
#define CCI_USBPLL_SDM_SIG_DITH_SEL_UMSK                        (~(((1ULL<<CCI_USBPLL_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_USBPLL_SDM_SIG_DITH_SEL_POS))
#define CCI_USBPLL_DIV2_EN                                      CCI_USBPLL_DIV2_EN
#define CCI_USBPLL_DIV2_EN_POS                                  (24U)
#define CCI_USBPLL_DIV2_EN_LEN                                  (1U)
#define CCI_USBPLL_DIV2_EN_MSK                                  (((1ULL<<CCI_USBPLL_DIV2_EN_LEN)-1)<<CCI_USBPLL_DIV2_EN_POS)
#define CCI_USBPLL_DIV2_EN_UMSK                                 (~(((1ULL<<CCI_USBPLL_DIV2_EN_LEN)-1)<<CCI_USBPLL_DIV2_EN_POS))
#define CCI_USBPLL_CLKOUT_EN                                    CCI_USBPLL_CLKOUT_EN
#define CCI_USBPLL_CLKOUT_EN_POS                                (25U)
#define CCI_USBPLL_CLKOUT_EN_LEN                                (1U)
#define CCI_USBPLL_CLKOUT_EN_MSK                                (((1ULL<<CCI_USBPLL_CLKOUT_EN_LEN)-1)<<CCI_USBPLL_CLKOUT_EN_POS)
#define CCI_USBPLL_CLKOUT_EN_UMSK                               (~(((1ULL<<CCI_USBPLL_CLKOUT_EN_LEN)-1)<<CCI_USBPLL_CLKOUT_EN_POS))
#define CCI_USBPLL_SEL_SAMPLE_CLK                               CCI_USBPLL_SEL_SAMPLE_CLK
#define CCI_USBPLL_SEL_SAMPLE_CLK_POS                           (26U)
#define CCI_USBPLL_SEL_SAMPLE_CLK_LEN                           (2U)
#define CCI_USBPLL_SEL_SAMPLE_CLK_MSK                           (((1ULL<<CCI_USBPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_USBPLL_SEL_SAMPLE_CLK_POS)
#define CCI_USBPLL_SEL_SAMPLE_CLK_UMSK                          (~(((1ULL<<CCI_USBPLL_SEL_SAMPLE_CLK_LEN)-1)<<CCI_USBPLL_SEL_SAMPLE_CLK_POS))
#define CCI_USBPLL_RSTB                                         CCI_USBPLL_RSTB
#define CCI_USBPLL_RSTB_POS                                     (28U)
#define CCI_USBPLL_RSTB_LEN                                     (1U)
#define CCI_USBPLL_RSTB_MSK                                     (((1ULL<<CCI_USBPLL_RSTB_LEN)-1)<<CCI_USBPLL_RSTB_POS)
#define CCI_USBPLL_RSTB_UMSK                                    (~(((1ULL<<CCI_USBPLL_RSTB_LEN)-1)<<CCI_USBPLL_RSTB_POS))
#define CCI_PU_USBPLL_MMDIV                                     CCI_PU_USBPLL_MMDIV
#define CCI_PU_USBPLL_MMDIV_POS                                 (29U)
#define CCI_PU_USBPLL_MMDIV_LEN                                 (1U)
#define CCI_PU_USBPLL_MMDIV_MSK                                 (((1ULL<<CCI_PU_USBPLL_MMDIV_LEN)-1)<<CCI_PU_USBPLL_MMDIV_POS)
#define CCI_PU_USBPLL_MMDIV_UMSK                                (~(((1ULL<<CCI_PU_USBPLL_MMDIV_LEN)-1)<<CCI_PU_USBPLL_MMDIV_POS))

/* 0x83C : wifi_pll_cfg11 */
#define CCI_WIFI_PLL_CFG11_OFFSET                               (0x83C)
#define CCI_USBPLL_SSC_CNT                                      CCI_USBPLL_SSC_CNT
#define CCI_USBPLL_SSC_CNT_POS                                  (0U)
#define CCI_USBPLL_SSC_CNT_LEN                                  (9U)
#define CCI_USBPLL_SSC_CNT_MSK                                  (((1ULL<<CCI_USBPLL_SSC_CNT_LEN)-1)<<CCI_USBPLL_SSC_CNT_POS)
#define CCI_USBPLL_SSC_CNT_UMSK                                 (~(((1ULL<<CCI_USBPLL_SSC_CNT_LEN)-1)<<CCI_USBPLL_SSC_CNT_POS))
#define CCI_USBPLL_SSC_START                                    CCI_USBPLL_SSC_START
#define CCI_USBPLL_SSC_START_POS                                (9U)
#define CCI_USBPLL_SSC_START_LEN                                (1U)
#define CCI_USBPLL_SSC_START_MSK                                (((1ULL<<CCI_USBPLL_SSC_START_LEN)-1)<<CCI_USBPLL_SSC_START_POS)
#define CCI_USBPLL_SSC_START_UMSK                               (~(((1ULL<<CCI_USBPLL_SSC_START_LEN)-1)<<CCI_USBPLL_SSC_START_POS))
#define CCI_USBPLL_SSC_START_GATE_EN                            CCI_USBPLL_SSC_START_GATE_EN
#define CCI_USBPLL_SSC_START_GATE_EN_POS                        (10U)
#define CCI_USBPLL_SSC_START_GATE_EN_LEN                        (1U)
#define CCI_USBPLL_SSC_START_GATE_EN_MSK                        (((1ULL<<CCI_USBPLL_SSC_START_GATE_EN_LEN)-1)<<CCI_USBPLL_SSC_START_GATE_EN_POS)
#define CCI_USBPLL_SSC_START_GATE_EN_UMSK                       (~(((1ULL<<CCI_USBPLL_SSC_START_GATE_EN_LEN)-1)<<CCI_USBPLL_SSC_START_GATE_EN_POS))
#define CCI_USBPLL_SSC_GAIN                                     CCI_USBPLL_SSC_GAIN
#define CCI_USBPLL_SSC_GAIN_POS                                 (12U)
#define CCI_USBPLL_SSC_GAIN_LEN                                 (3U)
#define CCI_USBPLL_SSC_GAIN_MSK                                 (((1ULL<<CCI_USBPLL_SSC_GAIN_LEN)-1)<<CCI_USBPLL_SSC_GAIN_POS)
#define CCI_USBPLL_SSC_GAIN_UMSK                                (~(((1ULL<<CCI_USBPLL_SSC_GAIN_LEN)-1)<<CCI_USBPLL_SSC_GAIN_POS))
#define CCI_USBPLL_SSC_EN                                       CCI_USBPLL_SSC_EN
#define CCI_USBPLL_SSC_EN_POS                                   (16U)
#define CCI_USBPLL_SSC_EN_LEN                                   (1U)
#define CCI_USBPLL_SSC_EN_MSK                                   (((1ULL<<CCI_USBPLL_SSC_EN_LEN)-1)<<CCI_USBPLL_SSC_EN_POS)
#define CCI_USBPLL_SSC_EN_UMSK                                  (~(((1ULL<<CCI_USBPLL_SSC_EN_LEN)-1)<<CCI_USBPLL_SSC_EN_POS))

/* 0x840 : wifi_pll_cfg12 */
#define CCI_WIFI_PLL_CFG12_OFFSET                               (0x840)
#define CCI_SSCDIV_SDMIN                                        CCI_SSCDIV_SDMIN
#define CCI_SSCDIV_SDMIN_POS                                    (0U)
#define CCI_SSCDIV_SDMIN_LEN                                    (19U)
#define CCI_SSCDIV_SDMIN_MSK                                    (((1ULL<<CCI_SSCDIV_SDMIN_LEN)-1)<<CCI_SSCDIV_SDMIN_POS)
#define CCI_SSCDIV_SDMIN_UMSK                                   (~(((1ULL<<CCI_SSCDIV_SDMIN_LEN)-1)<<CCI_SSCDIV_SDMIN_POS))
#define CCI_SSCDIV_SDM_BYPASS                                   CCI_SSCDIV_SDM_BYPASS
#define CCI_SSCDIV_SDM_BYPASS_POS                               (20U)
#define CCI_SSCDIV_SDM_BYPASS_LEN                               (1U)
#define CCI_SSCDIV_SDM_BYPASS_MSK                               (((1ULL<<CCI_SSCDIV_SDM_BYPASS_LEN)-1)<<CCI_SSCDIV_SDM_BYPASS_POS)
#define CCI_SSCDIV_SDM_BYPASS_UMSK                              (~(((1ULL<<CCI_SSCDIV_SDM_BYPASS_LEN)-1)<<CCI_SSCDIV_SDM_BYPASS_POS))
#define CCI_SSCDIV_SDM_ORDER_SEL                                CCI_SSCDIV_SDM_ORDER_SEL
#define CCI_SSCDIV_SDM_ORDER_SEL_POS                            (21U)
#define CCI_SSCDIV_SDM_ORDER_SEL_LEN                            (1U)
#define CCI_SSCDIV_SDM_ORDER_SEL_MSK                            (((1ULL<<CCI_SSCDIV_SDM_ORDER_SEL_LEN)-1)<<CCI_SSCDIV_SDM_ORDER_SEL_POS)
#define CCI_SSCDIV_SDM_ORDER_SEL_UMSK                           (~(((1ULL<<CCI_SSCDIV_SDM_ORDER_SEL_LEN)-1)<<CCI_SSCDIV_SDM_ORDER_SEL_POS))
#define CCI_SSCDIV_SDM_SIG_DITH_SEL                             CCI_SSCDIV_SDM_SIG_DITH_SEL
#define CCI_SSCDIV_SDM_SIG_DITH_SEL_POS                         (22U)
#define CCI_SSCDIV_SDM_SIG_DITH_SEL_LEN                         (2U)
#define CCI_SSCDIV_SDM_SIG_DITH_SEL_MSK                         (((1ULL<<CCI_SSCDIV_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_SSCDIV_SDM_SIG_DITH_SEL_POS)
#define CCI_SSCDIV_SDM_SIG_DITH_SEL_UMSK                        (~(((1ULL<<CCI_SSCDIV_SDM_SIG_DITH_SEL_LEN)-1)<<CCI_SSCDIV_SDM_SIG_DITH_SEL_POS))
#define CCI_SSCDIV_DIV2_EN                                      CCI_SSCDIV_DIV2_EN
#define CCI_SSCDIV_DIV2_EN_POS                                  (24U)
#define CCI_SSCDIV_DIV2_EN_LEN                                  (1U)
#define CCI_SSCDIV_DIV2_EN_MSK                                  (((1ULL<<CCI_SSCDIV_DIV2_EN_LEN)-1)<<CCI_SSCDIV_DIV2_EN_POS)
#define CCI_SSCDIV_DIV2_EN_UMSK                                 (~(((1ULL<<CCI_SSCDIV_DIV2_EN_LEN)-1)<<CCI_SSCDIV_DIV2_EN_POS))
#define CCI_SSCDIV_CLKOUT_EN                                    CCI_SSCDIV_CLKOUT_EN
#define CCI_SSCDIV_CLKOUT_EN_POS                                (25U)
#define CCI_SSCDIV_CLKOUT_EN_LEN                                (1U)
#define CCI_SSCDIV_CLKOUT_EN_MSK                                (((1ULL<<CCI_SSCDIV_CLKOUT_EN_LEN)-1)<<CCI_SSCDIV_CLKOUT_EN_POS)
#define CCI_SSCDIV_CLKOUT_EN_UMSK                               (~(((1ULL<<CCI_SSCDIV_CLKOUT_EN_LEN)-1)<<CCI_SSCDIV_CLKOUT_EN_POS))
#define CCI_SSCDIV_SEL_SAMPLE_CLK                               CCI_SSCDIV_SEL_SAMPLE_CLK
#define CCI_SSCDIV_SEL_SAMPLE_CLK_POS                           (26U)
#define CCI_SSCDIV_SEL_SAMPLE_CLK_LEN                           (2U)
#define CCI_SSCDIV_SEL_SAMPLE_CLK_MSK                           (((1ULL<<CCI_SSCDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_SSCDIV_SEL_SAMPLE_CLK_POS)
#define CCI_SSCDIV_SEL_SAMPLE_CLK_UMSK                          (~(((1ULL<<CCI_SSCDIV_SEL_SAMPLE_CLK_LEN)-1)<<CCI_SSCDIV_SEL_SAMPLE_CLK_POS))
#define CCI_SSCDIV_RSTB                                         CCI_SSCDIV_RSTB
#define CCI_SSCDIV_RSTB_POS                                     (28U)
#define CCI_SSCDIV_RSTB_LEN                                     (1U)
#define CCI_SSCDIV_RSTB_MSK                                     (((1ULL<<CCI_SSCDIV_RSTB_LEN)-1)<<CCI_SSCDIV_RSTB_POS)
#define CCI_SSCDIV_RSTB_UMSK                                    (~(((1ULL<<CCI_SSCDIV_RSTB_LEN)-1)<<CCI_SSCDIV_RSTB_POS))
#define CCI_PU_SSCDIV_MMDIV                                     CCI_PU_SSCDIV_MMDIV
#define CCI_PU_SSCDIV_MMDIV_POS                                 (29U)
#define CCI_PU_SSCDIV_MMDIV_LEN                                 (1U)
#define CCI_PU_SSCDIV_MMDIV_MSK                                 (((1ULL<<CCI_PU_SSCDIV_MMDIV_LEN)-1)<<CCI_PU_SSCDIV_MMDIV_POS)
#define CCI_PU_SSCDIV_MMDIV_UMSK                                (~(((1ULL<<CCI_PU_SSCDIV_MMDIV_LEN)-1)<<CCI_PU_SSCDIV_MMDIV_POS))

/* 0x844 : wifi_pll_cfg13 */
#define CCI_WIFI_PLL_CFG13_OFFSET                               (0x844)
#define CCI_SSCDIV_SSC_CNT                                      CCI_SSCDIV_SSC_CNT
#define CCI_SSCDIV_SSC_CNT_POS                                  (0U)
#define CCI_SSCDIV_SSC_CNT_LEN                                  (9U)
#define CCI_SSCDIV_SSC_CNT_MSK                                  (((1ULL<<CCI_SSCDIV_SSC_CNT_LEN)-1)<<CCI_SSCDIV_SSC_CNT_POS)
#define CCI_SSCDIV_SSC_CNT_UMSK                                 (~(((1ULL<<CCI_SSCDIV_SSC_CNT_LEN)-1)<<CCI_SSCDIV_SSC_CNT_POS))
#define CCI_SSCDIV_SSC_START                                    CCI_SSCDIV_SSC_START
#define CCI_SSCDIV_SSC_START_POS                                (9U)
#define CCI_SSCDIV_SSC_START_LEN                                (1U)
#define CCI_SSCDIV_SSC_START_MSK                                (((1ULL<<CCI_SSCDIV_SSC_START_LEN)-1)<<CCI_SSCDIV_SSC_START_POS)
#define CCI_SSCDIV_SSC_START_UMSK                               (~(((1ULL<<CCI_SSCDIV_SSC_START_LEN)-1)<<CCI_SSCDIV_SSC_START_POS))
#define CCI_SSCDIV_SSC_START_GATE_EN                            CCI_SSCDIV_SSC_START_GATE_EN
#define CCI_SSCDIV_SSC_START_GATE_EN_POS                        (10U)
#define CCI_SSCDIV_SSC_START_GATE_EN_LEN                        (1U)
#define CCI_SSCDIV_SSC_START_GATE_EN_MSK                        (((1ULL<<CCI_SSCDIV_SSC_START_GATE_EN_LEN)-1)<<CCI_SSCDIV_SSC_START_GATE_EN_POS)
#define CCI_SSCDIV_SSC_START_GATE_EN_UMSK                       (~(((1ULL<<CCI_SSCDIV_SSC_START_GATE_EN_LEN)-1)<<CCI_SSCDIV_SSC_START_GATE_EN_POS))
#define CCI_SSCDIV_SSC_GAIN                                     CCI_SSCDIV_SSC_GAIN
#define CCI_SSCDIV_SSC_GAIN_POS                                 (12U)
#define CCI_SSCDIV_SSC_GAIN_LEN                                 (3U)
#define CCI_SSCDIV_SSC_GAIN_MSK                                 (((1ULL<<CCI_SSCDIV_SSC_GAIN_LEN)-1)<<CCI_SSCDIV_SSC_GAIN_POS)
#define CCI_SSCDIV_SSC_GAIN_UMSK                                (~(((1ULL<<CCI_SSCDIV_SSC_GAIN_LEN)-1)<<CCI_SSCDIV_SSC_GAIN_POS))
#define CCI_SSCDIV_SSC_EN                                       CCI_SSCDIV_SSC_EN
#define CCI_SSCDIV_SSC_EN_POS                                   (16U)
#define CCI_SSCDIV_SSC_EN_LEN                                   (1U)
#define CCI_SSCDIV_SSC_EN_MSK                                   (((1ULL<<CCI_SSCDIV_SSC_EN_LEN)-1)<<CCI_SSCDIV_SSC_EN_POS)
#define CCI_SSCDIV_SSC_EN_UMSK                                  (~(((1ULL<<CCI_SSCDIV_SSC_EN_LEN)-1)<<CCI_SSCDIV_SSC_EN_POS))

/* 0x848 : wifi_pll_cfg14 */
#define CCI_WIFI_PLL_CFG14_OFFSET                               (0x848)
#define CCI_WIFIPLL_RESV                                        CCI_WIFIPLL_RESV
#define CCI_WIFIPLL_RESV_POS                                    (0U)
#define CCI_WIFIPLL_RESV_LEN                                    (16U)
#define CCI_WIFIPLL_RESV_MSK                                    (((1ULL<<CCI_WIFIPLL_RESV_LEN)-1)<<CCI_WIFIPLL_RESV_POS)
#define CCI_WIFIPLL_RESV_UMSK                                   (~(((1ULL<<CCI_WIFIPLL_RESV_LEN)-1)<<CCI_WIFIPLL_RESV_POS))
#define CCI_SSCDIV_DL_CTRL                                      CCI_SSCDIV_DL_CTRL
#define CCI_SSCDIV_DL_CTRL_POS                                  (16U)
#define CCI_SSCDIV_DL_CTRL_LEN                                  (1U)
#define CCI_SSCDIV_DL_CTRL_MSK                                  (((1ULL<<CCI_SSCDIV_DL_CTRL_LEN)-1)<<CCI_SSCDIV_DL_CTRL_POS)
#define CCI_SSCDIV_DL_CTRL_UMSK                                 (~(((1ULL<<CCI_SSCDIV_DL_CTRL_LEN)-1)<<CCI_SSCDIV_DL_CTRL_POS))
#define CCI_USBPLL_DL_CTRL                                      CCI_USBPLL_DL_CTRL
#define CCI_USBPLL_DL_CTRL_POS                                  (17U)
#define CCI_USBPLL_DL_CTRL_LEN                                  (1U)
#define CCI_USBPLL_DL_CTRL_MSK                                  (((1ULL<<CCI_USBPLL_DL_CTRL_LEN)-1)<<CCI_USBPLL_DL_CTRL_POS)
#define CCI_USBPLL_DL_CTRL_UMSK                                 (~(((1ULL<<CCI_USBPLL_DL_CTRL_LEN)-1)<<CCI_USBPLL_DL_CTRL_POS))
#define CCI_WIFIPLL_DL_CTRL_30                                  CCI_WIFIPLL_DL_CTRL_30
#define CCI_WIFIPLL_DL_CTRL_30_POS                              (18U)
#define CCI_WIFIPLL_DL_CTRL_30_LEN                              (1U)
#define CCI_WIFIPLL_DL_CTRL_30_MSK                              (((1ULL<<CCI_WIFIPLL_DL_CTRL_30_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_30_POS)
#define CCI_WIFIPLL_DL_CTRL_30_UMSK                             (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_30_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_30_POS))
#define CCI_WIFIPLL_DL_CTRL_20                                  CCI_WIFIPLL_DL_CTRL_20
#define CCI_WIFIPLL_DL_CTRL_20_POS                              (19U)
#define CCI_WIFIPLL_DL_CTRL_20_LEN                              (1U)
#define CCI_WIFIPLL_DL_CTRL_20_MSK                              (((1ULL<<CCI_WIFIPLL_DL_CTRL_20_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_20_POS)
#define CCI_WIFIPLL_DL_CTRL_20_UMSK                             (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_20_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_20_POS))
#define CCI_WIFIPLL_DL_CTRL_12                                  CCI_WIFIPLL_DL_CTRL_12
#define CCI_WIFIPLL_DL_CTRL_12_POS                              (20U)
#define CCI_WIFIPLL_DL_CTRL_12_LEN                              (1U)
#define CCI_WIFIPLL_DL_CTRL_12_MSK                              (((1ULL<<CCI_WIFIPLL_DL_CTRL_12_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_12_POS)
#define CCI_WIFIPLL_DL_CTRL_12_UMSK                             (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_12_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_12_POS))
#define CCI_WIFIPLL_DL_CTRL_10                                  CCI_WIFIPLL_DL_CTRL_10
#define CCI_WIFIPLL_DL_CTRL_10_POS                              (21U)
#define CCI_WIFIPLL_DL_CTRL_10_LEN                              (1U)
#define CCI_WIFIPLL_DL_CTRL_10_MSK                              (((1ULL<<CCI_WIFIPLL_DL_CTRL_10_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_10_POS)
#define CCI_WIFIPLL_DL_CTRL_10_UMSK                             (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_10_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_10_POS))
#define CCI_WIFIPLL_DL_CTRL_8                                   CCI_WIFIPLL_DL_CTRL_8
#define CCI_WIFIPLL_DL_CTRL_8_POS                               (22U)
#define CCI_WIFIPLL_DL_CTRL_8_LEN                               (1U)
#define CCI_WIFIPLL_DL_CTRL_8_MSK                               (((1ULL<<CCI_WIFIPLL_DL_CTRL_8_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_8_POS)
#define CCI_WIFIPLL_DL_CTRL_8_UMSK                              (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_8_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_8_POS))
#define CCI_WIFIPLL_DL_CTRL_6_RF                                CCI_WIFIPLL_DL_CTRL_6_RF
#define CCI_WIFIPLL_DL_CTRL_6_RF_POS                            (23U)
#define CCI_WIFIPLL_DL_CTRL_6_RF_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_6_RF_MSK                            (((1ULL<<CCI_WIFIPLL_DL_CTRL_6_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_6_RF_POS)
#define CCI_WIFIPLL_DL_CTRL_6_RF_UMSK                           (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_6_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_6_RF_POS))
#define CCI_WIFIPLL_DL_CTRL_6                                   CCI_WIFIPLL_DL_CTRL_6
#define CCI_WIFIPLL_DL_CTRL_6_POS                               (24U)
#define CCI_WIFIPLL_DL_CTRL_6_LEN                               (1U)
#define CCI_WIFIPLL_DL_CTRL_6_MSK                               (((1ULL<<CCI_WIFIPLL_DL_CTRL_6_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_6_POS)
#define CCI_WIFIPLL_DL_CTRL_6_UMSK                              (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_6_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_6_POS))
#define CCI_WIFIPLL_DL_CTRL_5                                   CCI_WIFIPLL_DL_CTRL_5
#define CCI_WIFIPLL_DL_CTRL_5_POS                               (25U)
#define CCI_WIFIPLL_DL_CTRL_5_LEN                               (1U)
#define CCI_WIFIPLL_DL_CTRL_5_MSK                               (((1ULL<<CCI_WIFIPLL_DL_CTRL_5_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_5_POS)
#define CCI_WIFIPLL_DL_CTRL_5_UMSK                              (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_5_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_5_POS))
#define CCI_WIFIPLL_DL_CTRL_4                                   CCI_WIFIPLL_DL_CTRL_4
#define CCI_WIFIPLL_DL_CTRL_4_POS                               (26U)
#define CCI_WIFIPLL_DL_CTRL_4_LEN                               (1U)
#define CCI_WIFIPLL_DL_CTRL_4_MSK                               (((1ULL<<CCI_WIFIPLL_DL_CTRL_4_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_4_POS)
#define CCI_WIFIPLL_DL_CTRL_4_UMSK                              (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_4_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_4_POS))
#define CCI_WIFIPLL_DL_CTRL_3_RF                                CCI_WIFIPLL_DL_CTRL_3_RF
#define CCI_WIFIPLL_DL_CTRL_3_RF_POS                            (27U)
#define CCI_WIFIPLL_DL_CTRL_3_RF_LEN                            (1U)
#define CCI_WIFIPLL_DL_CTRL_3_RF_MSK                            (((1ULL<<CCI_WIFIPLL_DL_CTRL_3_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_3_RF_POS)
#define CCI_WIFIPLL_DL_CTRL_3_RF_UMSK                           (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_3_RF_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_3_RF_POS))
#define CCI_WIFIPLL_DL_CTRL_3                                   CCI_WIFIPLL_DL_CTRL_3
#define CCI_WIFIPLL_DL_CTRL_3_POS                               (28U)
#define CCI_WIFIPLL_DL_CTRL_3_LEN                               (1U)
#define CCI_WIFIPLL_DL_CTRL_3_MSK                               (((1ULL<<CCI_WIFIPLL_DL_CTRL_3_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_3_POS)
#define CCI_WIFIPLL_DL_CTRL_3_UMSK                              (~(((1ULL<<CCI_WIFIPLL_DL_CTRL_3_LEN)-1)<<CCI_WIFIPLL_DL_CTRL_3_POS))


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
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cfg_mcci_dly_r                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
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
    uint8_t RESERVED0x14[2044];

    /* 0x810 : wifi_pll_cfg0 */
    union {
        struct {
            uint32_t wifipll_sdm_rstb               :  1; /* [    0],        r/w,        0x1 */
            uint32_t wifipll_postdiv_rstb           :  1; /* [    1],        r/w,        0x1 */
            uint32_t wifipll_fbdv_rstb              :  1; /* [    2],        r/w,        0x1 */
            uint32_t wifipll_refdiv_rstb            :  1; /* [    3],        r/w,        0x1 */
            uint32_t pu_wifipll_clktree             :  1; /* [    4],        r/w,        0x1 */
            uint32_t pu_wifipll_postdiv             :  1; /* [    5],        r/w,        0x1 */
            uint32_t pu_wifipll_fbdv                :  1; /* [    6],        r/w,        0x1 */
            uint32_t pu_wifipll_clamp_op            :  1; /* [    7],        r/w,        0x1 */
            uint32_t pu_wifipll_pfd                 :  1; /* [    8],        r/w,        0x1 */
            uint32_t pu_wifipll_cp                  :  1; /* [    9],        r/w,        0x1 */
            uint32_t pu_wifipll_sfreg               :  1; /* [   10],        r/w,        0x0 */
            uint32_t pu_wifipll                     :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg0;

    /* 0x814 : wifi_pll_cfg1 */
    union {
        struct {
            uint32_t wifipll_postdiv                :  7; /* [ 6: 0],        r/w,       0x14 */
            uint32_t reserved_7                     :  1; /* [    7],       rsvd,        0x0 */
            uint32_t wifipll_refdiv_ratio           :  4; /* [11: 8],        r/w,        0x2 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t wifipll_refclk_sel             :  2; /* [17:16],        r/w,        0x1 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t wifipll_vg11_sel               :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t wifipll_vg13_sel               :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg1;

    /* 0x818 : wifi_pll_cfg2 */
    union {
        struct {
            uint32_t wifipll_sel_cp_bias            :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t wifipll_icp_5u                 :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t wifipll_icp_1u                 :  2; /* [ 7: 6],        r/w,        0x0 */
            uint32_t wifipll_int_frac_sw            :  1; /* [    8],        r/w,        0x0 */
            uint32_t wifipll_cp_startup_en          :  1; /* [    9],        r/w,        0x1 */
            uint32_t wifipll_cp_opamp_en            :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg2;

    /* 0x81C : wifi_pll_cfg3 */
    union {
        struct {
            uint32_t wifipll_c4_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t wifipll_r4                     :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t wifipll_r4_short               :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t wifipll_c3                     :  2; /* [13:12],        r/w,        0x2 */
            uint32_t wifipll_cz                     :  2; /* [15:14],        r/w,        0x1 */
            uint32_t wifipll_rz                     :  3; /* [18:16],        r/w,        0x3 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg3;

    /* 0x820 : wifi_pll_cfg4 */
    union {
        struct {
            uint32_t wifipll_sel_sample_clk         :  2; /* [ 1: 0],        r/w,        0x1 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t wifipll_sel_fb_clk             :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t wifipll_sdmclk_sel             :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg4;

    /* 0x824 : wifi_pll_cfg5 */
    union {
        struct {
            uint32_t wifipll_vco_speed              :  3; /* [ 2: 0],        r/w,        0x5 */
            uint32_t wifipll_vco_div3_en            :  1; /* [    3],        r/w,        0x1 */
            uint32_t wifipll_vco_div2_en            :  1; /* [    4],        r/w,        0x1 */
            uint32_t wifipll_vco_div1_en            :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg5;

    /* 0x828 : wifi_pll_cfg6 */
    union {
        struct {
            uint32_t wifipll_sdmin                  : 26; /* [25: 0],        r/w,  0x1800000 */
            uint32_t wifipll_sdm_bypass             :  1; /* [   26],        r/w,        0x1 */
            uint32_t reserved_27_29                 :  3; /* [29:27],       rsvd,        0x0 */
            uint32_t wifipll_sdm_bypass_hw          :  1; /* [   30],          r,        0x0 */
            uint32_t wifipll_sdm_ctrl_hw            :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg6;

    /* 0x82C : wifi_pll_cfg7 */
    union {
        struct {
            uint32_t wifipll_sdm_order_sel          :  2; /* [ 1: 0],        r/w,        0x2 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t wifipll_sdm_noi_prbs_sel       :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t wifipll_sdm_noi_prbs_en        :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t wifipll_sdm_sig_prbs_sel       :  2; /* [13:12],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t wifipll_sdm_sig_dith_sel       :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg7;

    /* 0x830 : wifi_pll_cfg8 */
    union {
        struct {
            uint32_t wifipll_en_rf_div3             :  1; /* [    0],        r/w,        0x0 */
            uint32_t wifipll_en_rf_div6             :  1; /* [    1],        r/w,        0x0 */
            uint32_t wifipll_sel_div3_div6          :  1; /* [    2],        r/w,        0x0 */
            uint32_t wifipll_sel_div6_div12         :  1; /* [    3],        r/w,        0x0 */
            uint32_t wifipll_en_div3                :  1; /* [    4],        r/w,        0x0 */
            uint32_t wifipll_en_div4                :  1; /* [    5],        r/w,        0x0 */
            uint32_t wifipll_en_div5                :  1; /* [    6],        r/w,        0x0 */
            uint32_t wifipll_en_div6                :  1; /* [    7],        r/w,        0x0 */
            uint32_t wifipll_en_div8                :  1; /* [    8],        r/w,        0x0 */
            uint32_t wifipll_en_div10               :  1; /* [    9],        r/w,        0x0 */
            uint32_t wifipll_en_div12               :  1; /* [   10],        r/w,        0x0 */
            uint32_t wifipll_en_div20               :  1; /* [   11],        r/w,        0x0 */
            uint32_t wifipll_en_div30               :  1; /* [   12],        r/w,        0x1 */
            uint32_t reserved_13_19                 :  7; /* [19:13],       rsvd,        0x0 */
            uint32_t wifipll_en_rf_div3_hw          :  1; /* [   20],          r,        0x1 */
            uint32_t reserved_21_30                 : 10; /* [30:21],       rsvd,        0x0 */
            uint32_t wifipll_en_ctrl_hw             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg8;

    /* 0x834 : wifi_pll_cfg9 */
    union {
        struct {
            uint32_t wifipll_dc_tp_out_en           :  1; /* [    0],        r/w,        0x0 */
            uint32_t ten_wifipll                    :  1; /* [    1],        r/w,        0x0 */
            uint32_t ten_wifipll_sfreg              :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t dten_wifipll_fin               :  1; /* [    4],        r/w,        0x0 */
            uint32_t dten_wifipll_fref              :  1; /* [    5],        r/w,        0x0 */
            uint32_t dten_wifipll_fsdm              :  1; /* [    6],        r/w,        0x0 */
            uint32_t dten_wifipll_div30             :  1; /* [    7],        r/w,        0x0 */
            uint32_t dten_wifipll_div10             :  1; /* [    8],        r/w,        0x0 */
            uint32_t dten_wifipll_postdiv_clk       :  1; /* [    9],        r/w,        0x0 */
            uint32_t dten_usbpll_pclk               :  1; /* [   10],        r/w,        0x0 */
            uint32_t dten_usbpll_clkout             :  1; /* [   11],        r/w,        0x0 */
            uint32_t dten_sscdiv_pclk               :  1; /* [   12],        r/w,        0x0 */
            uint32_t dten_sscdiv_clkout             :  1; /* [   13],        r/w,        0x0 */
            uint32_t dtest_pulldown                 :  1; /* [   14],        r/w,        0x1 */
            uint32_t reserved_15_31                 : 17; /* [31:15],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg9;

    /* 0x838 : wifi_pll_cfg10 */
    union {
        struct {
            uint32_t usbpll_sdmin                   : 19; /* [18: 0],        r/w,    0x28000 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t usbpll_sdm_bypass              :  1; /* [   20],        r/w,        0x0 */
            uint32_t usbpll_sdm_order_sel           :  1; /* [   21],        r/w,        0x1 */
            uint32_t usbpll_sdm_sig_dith_sel        :  2; /* [23:22],        r/w,        0x0 */
            uint32_t usbpll_div2_en                 :  1; /* [   24],        r/w,        0x1 */
            uint32_t usbpll_clkout_en               :  1; /* [   25],        r/w,        0x1 */
            uint32_t usbpll_sel_sample_clk          :  2; /* [27:26],        r/w,        0x1 */
            uint32_t usbpll_rstb                    :  1; /* [   28],        r/w,        0x1 */
            uint32_t pu_usbpll_mmdiv                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg10;

    /* 0x83C : wifi_pll_cfg11 */
    union {
        struct {
            uint32_t usbpll_ssc_cnt                 :  9; /* [ 8: 0],        r/w,       0xf0 */
            uint32_t usbpll_ssc_start               :  1; /* [    9],        r/w,        0x1 */
            uint32_t usbpll_ssc_start_gate_en       :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t usbpll_ssc_gain                :  3; /* [14:12],        r/w,        0x3 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t usbpll_ssc_en                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg11;

    /* 0x840 : wifi_pll_cfg12 */
    union {
        struct {
            uint32_t sscdiv_sdmin                   : 19; /* [18: 0],        r/w,    0x28000 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t sscdiv_sdm_bypass              :  1; /* [   20],        r/w,        0x0 */
            uint32_t sscdiv_sdm_order_sel           :  1; /* [   21],        r/w,        0x1 */
            uint32_t sscdiv_sdm_sig_dith_sel        :  2; /* [23:22],        r/w,        0x0 */
            uint32_t sscdiv_div2_en                 :  1; /* [   24],        r/w,        0x1 */
            uint32_t sscdiv_clkout_en               :  1; /* [   25],        r/w,        0x1 */
            uint32_t sscdiv_sel_sample_clk          :  2; /* [27:26],        r/w,        0x1 */
            uint32_t sscdiv_rstb                    :  1; /* [   28],        r/w,        0x1 */
            uint32_t pu_sscdiv_mmdiv                :  1; /* [   29],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg12;

    /* 0x844 : wifi_pll_cfg13 */
    union {
        struct {
            uint32_t sscdiv_ssc_cnt                 :  9; /* [ 8: 0],        r/w,      0x1e0 */
            uint32_t sscdiv_ssc_start               :  1; /* [    9],        r/w,        0x1 */
            uint32_t sscdiv_ssc_start_gate_en       :  1; /* [   10],        r/w,        0x0 */
            uint32_t reserved_11                    :  1; /* [   11],       rsvd,        0x0 */
            uint32_t sscdiv_ssc_gain                :  3; /* [14:12],        r/w,        0x2 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t sscdiv_ssc_en                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg13;

    /* 0x848 : wifi_pll_cfg14 */
    union {
        struct {
            uint32_t wifipll_resv                   : 16; /* [15: 0],        r/w,     0xff00 */
            uint32_t sscdiv_dl_ctrl                 :  1; /* [   16],        r/w,        0x0 */
            uint32_t usbpll_dl_ctrl                 :  1; /* [   17],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_30             :  1; /* [   18],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_20             :  1; /* [   19],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_12             :  1; /* [   20],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_10             :  1; /* [   21],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_8              :  1; /* [   22],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_6_rf           :  1; /* [   23],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_6              :  1; /* [   24],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_5              :  1; /* [   25],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_4              :  1; /* [   26],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_3_rf           :  1; /* [   27],        r/w,        0x0 */
            uint32_t wifipll_dl_ctrl_3              :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_pll_cfg14;

};

typedef volatile struct cci_reg cci_reg_t;


#endif  /* __CCI_REG_H__ */
