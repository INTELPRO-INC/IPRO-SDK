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

#ifndef  __DBI_REG_H__
#define  __DBI_REG_H__

#include "ipro6.h"

/* 0x0 : dbi_config */
#define DBI_CONFIG_OFFSET                                       (0x0)
#define DBI_CR_DBI_EN                                           DBI_CR_DBI_EN
#define DBI_CR_DBI_EN_POS                                       (0U)
#define DBI_CR_DBI_EN_LEN                                       (1U)
#define DBI_CR_DBI_EN_MSK                                       (((1ULL<<DBI_CR_DBI_EN_LEN)-1)<<DBI_CR_DBI_EN_POS)
#define DBI_CR_DBI_EN_UMSK                                      (~(((1ULL<<DBI_CR_DBI_EN_LEN)-1)<<DBI_CR_DBI_EN_POS))
#define DBI_CR_DBI_SEL                                          DBI_CR_DBI_SEL
#define DBI_CR_DBI_SEL_POS                                      (1U)
#define DBI_CR_DBI_SEL_LEN                                      (2U)
#define DBI_CR_DBI_SEL_MSK                                      (((1ULL<<DBI_CR_DBI_SEL_LEN)-1)<<DBI_CR_DBI_SEL_POS)
#define DBI_CR_DBI_SEL_UMSK                                     (~(((1ULL<<DBI_CR_DBI_SEL_LEN)-1)<<DBI_CR_DBI_SEL_POS))
#define DBI_CR_DBI_CMD_EN                                       DBI_CR_DBI_CMD_EN
#define DBI_CR_DBI_CMD_EN_POS                                   (4U)
#define DBI_CR_DBI_CMD_EN_LEN                                   (1U)
#define DBI_CR_DBI_CMD_EN_MSK                                   (((1ULL<<DBI_CR_DBI_CMD_EN_LEN)-1)<<DBI_CR_DBI_CMD_EN_POS)
#define DBI_CR_DBI_CMD_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_CMD_EN_LEN)-1)<<DBI_CR_DBI_CMD_EN_POS))
#define DBI_CR_DBI_DAT_EN                                       DBI_CR_DBI_DAT_EN
#define DBI_CR_DBI_DAT_EN_POS                                   (5U)
#define DBI_CR_DBI_DAT_EN_LEN                                   (1U)
#define DBI_CR_DBI_DAT_EN_MSK                                   (((1ULL<<DBI_CR_DBI_DAT_EN_LEN)-1)<<DBI_CR_DBI_DAT_EN_POS)
#define DBI_CR_DBI_DAT_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_DAT_EN_LEN)-1)<<DBI_CR_DBI_DAT_EN_POS))
#define DBI_CR_DBI_DAT_WR                                       DBI_CR_DBI_DAT_WR
#define DBI_CR_DBI_DAT_WR_POS                                   (6U)
#define DBI_CR_DBI_DAT_WR_LEN                                   (1U)
#define DBI_CR_DBI_DAT_WR_MSK                                   (((1ULL<<DBI_CR_DBI_DAT_WR_LEN)-1)<<DBI_CR_DBI_DAT_WR_POS)
#define DBI_CR_DBI_DAT_WR_UMSK                                  (~(((1ULL<<DBI_CR_DBI_DAT_WR_LEN)-1)<<DBI_CR_DBI_DAT_WR_POS))
#define DBI_CR_DBI_DAT_TP                                       DBI_CR_DBI_DAT_TP
#define DBI_CR_DBI_DAT_TP_POS                                   (7U)
#define DBI_CR_DBI_DAT_TP_LEN                                   (1U)
#define DBI_CR_DBI_DAT_TP_MSK                                   (((1ULL<<DBI_CR_DBI_DAT_TP_LEN)-1)<<DBI_CR_DBI_DAT_TP_POS)
#define DBI_CR_DBI_DAT_TP_UMSK                                  (~(((1ULL<<DBI_CR_DBI_DAT_TP_LEN)-1)<<DBI_CR_DBI_DAT_TP_POS))
#define DBI_CR_DBI_DAT_BC                                       DBI_CR_DBI_DAT_BC
#define DBI_CR_DBI_DAT_BC_POS                                   (8U)
#define DBI_CR_DBI_DAT_BC_LEN                                   (8U)
#define DBI_CR_DBI_DAT_BC_MSK                                   (((1ULL<<DBI_CR_DBI_DAT_BC_LEN)-1)<<DBI_CR_DBI_DAT_BC_POS)
#define DBI_CR_DBI_DAT_BC_UMSK                                  (~(((1ULL<<DBI_CR_DBI_DAT_BC_LEN)-1)<<DBI_CR_DBI_DAT_BC_POS))
#define DBI_CR_DBI_SCL_POL                                      DBI_CR_DBI_SCL_POL
#define DBI_CR_DBI_SCL_POL_POS                                  (16U)
#define DBI_CR_DBI_SCL_POL_LEN                                  (1U)
#define DBI_CR_DBI_SCL_POL_MSK                                  (((1ULL<<DBI_CR_DBI_SCL_POL_LEN)-1)<<DBI_CR_DBI_SCL_POL_POS)
#define DBI_CR_DBI_SCL_POL_UMSK                                 (~(((1ULL<<DBI_CR_DBI_SCL_POL_LEN)-1)<<DBI_CR_DBI_SCL_POL_POS))
#define DBI_CR_DBI_SCL_PH                                       DBI_CR_DBI_SCL_PH
#define DBI_CR_DBI_SCL_PH_POS                                   (17U)
#define DBI_CR_DBI_SCL_PH_LEN                                   (1U)
#define DBI_CR_DBI_SCL_PH_MSK                                   (((1ULL<<DBI_CR_DBI_SCL_PH_LEN)-1)<<DBI_CR_DBI_SCL_PH_POS)
#define DBI_CR_DBI_SCL_PH_UMSK                                  (~(((1ULL<<DBI_CR_DBI_SCL_PH_LEN)-1)<<DBI_CR_DBI_SCL_PH_POS))
#define DBI_CR_DBI_CONT_EN                                      DBI_CR_DBI_CONT_EN
#define DBI_CR_DBI_CONT_EN_POS                                  (18U)
#define DBI_CR_DBI_CONT_EN_LEN                                  (1U)
#define DBI_CR_DBI_CONT_EN_MSK                                  (((1ULL<<DBI_CR_DBI_CONT_EN_LEN)-1)<<DBI_CR_DBI_CONT_EN_POS)
#define DBI_CR_DBI_CONT_EN_UMSK                                 (~(((1ULL<<DBI_CR_DBI_CONT_EN_LEN)-1)<<DBI_CR_DBI_CONT_EN_POS))
#define DBI_CR_DBI_DMY_EN                                       DBI_CR_DBI_DMY_EN
#define DBI_CR_DBI_DMY_EN_POS                                   (19U)
#define DBI_CR_DBI_DMY_EN_LEN                                   (1U)
#define DBI_CR_DBI_DMY_EN_MSK                                   (((1ULL<<DBI_CR_DBI_DMY_EN_LEN)-1)<<DBI_CR_DBI_DMY_EN_POS)
#define DBI_CR_DBI_DMY_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_DMY_EN_LEN)-1)<<DBI_CR_DBI_DMY_EN_POS))
#define DBI_CR_DBI_DMY_CNT                                      DBI_CR_DBI_DMY_CNT
#define DBI_CR_DBI_DMY_CNT_POS                                  (20U)
#define DBI_CR_DBI_DMY_CNT_LEN                                  (4U)
#define DBI_CR_DBI_DMY_CNT_MSK                                  (((1ULL<<DBI_CR_DBI_DMY_CNT_LEN)-1)<<DBI_CR_DBI_DMY_CNT_POS)
#define DBI_CR_DBI_DMY_CNT_UMSK                                 (~(((1ULL<<DBI_CR_DBI_DMY_CNT_LEN)-1)<<DBI_CR_DBI_DMY_CNT_POS))
#define DBI_CR_DBI_CS_STRETCH                                   DBI_CR_DBI_CS_STRETCH
#define DBI_CR_DBI_CS_STRETCH_POS                               (24U)
#define DBI_CR_DBI_CS_STRETCH_LEN                               (1U)
#define DBI_CR_DBI_CS_STRETCH_MSK                               (((1ULL<<DBI_CR_DBI_CS_STRETCH_LEN)-1)<<DBI_CR_DBI_CS_STRETCH_POS)
#define DBI_CR_DBI_CS_STRETCH_UMSK                              (~(((1ULL<<DBI_CR_DBI_CS_STRETCH_LEN)-1)<<DBI_CR_DBI_CS_STRETCH_POS))
#define DBI_STS_DBI_BUS_BUSY                                    DBI_STS_DBI_BUS_BUSY
#define DBI_STS_DBI_BUS_BUSY_POS                                (31U)
#define DBI_STS_DBI_BUS_BUSY_LEN                                (1U)
#define DBI_STS_DBI_BUS_BUSY_MSK                                (((1ULL<<DBI_STS_DBI_BUS_BUSY_LEN)-1)<<DBI_STS_DBI_BUS_BUSY_POS)
#define DBI_STS_DBI_BUS_BUSY_UMSK                               (~(((1ULL<<DBI_STS_DBI_BUS_BUSY_LEN)-1)<<DBI_STS_DBI_BUS_BUSY_POS))

/* 0x4 : qspi_config */
#define DBI_QSPI_CONFIG_OFFSET                                  (0x4)
#define DBI_CR_QSPI_CMD_4B                                      DBI_CR_QSPI_CMD_4B
#define DBI_CR_QSPI_CMD_4B_POS                                  (0U)
#define DBI_CR_QSPI_CMD_4B_LEN                                  (1U)
#define DBI_CR_QSPI_CMD_4B_MSK                                  (((1ULL<<DBI_CR_QSPI_CMD_4B_LEN)-1)<<DBI_CR_QSPI_CMD_4B_POS)
#define DBI_CR_QSPI_CMD_4B_UMSK                                 (~(((1ULL<<DBI_CR_QSPI_CMD_4B_LEN)-1)<<DBI_CR_QSPI_CMD_4B_POS))
#define DBI_CR_QSPI_ADR_4B                                      DBI_CR_QSPI_ADR_4B
#define DBI_CR_QSPI_ADR_4B_POS                                  (1U)
#define DBI_CR_QSPI_ADR_4B_LEN                                  (1U)
#define DBI_CR_QSPI_ADR_4B_MSK                                  (((1ULL<<DBI_CR_QSPI_ADR_4B_LEN)-1)<<DBI_CR_QSPI_ADR_4B_POS)
#define DBI_CR_QSPI_ADR_4B_UMSK                                 (~(((1ULL<<DBI_CR_QSPI_ADR_4B_LEN)-1)<<DBI_CR_QSPI_ADR_4B_POS))
#define DBI_CR_QSPI_DAT_4B                                      DBI_CR_QSPI_DAT_4B
#define DBI_CR_QSPI_DAT_4B_POS                                  (2U)
#define DBI_CR_QSPI_DAT_4B_LEN                                  (1U)
#define DBI_CR_QSPI_DAT_4B_MSK                                  (((1ULL<<DBI_CR_QSPI_DAT_4B_LEN)-1)<<DBI_CR_QSPI_DAT_4B_POS)
#define DBI_CR_QSPI_DAT_4B_UMSK                                 (~(((1ULL<<DBI_CR_QSPI_DAT_4B_LEN)-1)<<DBI_CR_QSPI_DAT_4B_POS))
#define DBI_CR_QSPI_ADR_BC                                      DBI_CR_QSPI_ADR_BC
#define DBI_CR_QSPI_ADR_BC_POS                                  (4U)
#define DBI_CR_QSPI_ADR_BC_LEN                                  (2U)
#define DBI_CR_QSPI_ADR_BC_MSK                                  (((1ULL<<DBI_CR_QSPI_ADR_BC_LEN)-1)<<DBI_CR_QSPI_ADR_BC_POS)
#define DBI_CR_QSPI_ADR_BC_UMSK                                 (~(((1ULL<<DBI_CR_QSPI_ADR_BC_LEN)-1)<<DBI_CR_QSPI_ADR_BC_POS))

/* 0x8 : dbi_pix_cnt */
#define DBI_PIX_CNT_OFFSET                                      (0x8)
#define DBI_CR_DBI_PIX_CNT                                      DBI_CR_DBI_PIX_CNT
#define DBI_CR_DBI_PIX_CNT_POS                                  (0U)
#define DBI_CR_DBI_PIX_CNT_LEN                                  (24U)
#define DBI_CR_DBI_PIX_CNT_MSK                                  (((1ULL<<DBI_CR_DBI_PIX_CNT_LEN)-1)<<DBI_CR_DBI_PIX_CNT_POS)
#define DBI_CR_DBI_PIX_CNT_UMSK                                 (~(((1ULL<<DBI_CR_DBI_PIX_CNT_LEN)-1)<<DBI_CR_DBI_PIX_CNT_POS))
#define DBI_CR_DBI_PIX_FORMAT                                   DBI_CR_DBI_PIX_FORMAT
#define DBI_CR_DBI_PIX_FORMAT_POS                               (31U)
#define DBI_CR_DBI_PIX_FORMAT_LEN                               (1U)
#define DBI_CR_DBI_PIX_FORMAT_MSK                               (((1ULL<<DBI_CR_DBI_PIX_FORMAT_LEN)-1)<<DBI_CR_DBI_PIX_FORMAT_POS)
#define DBI_CR_DBI_PIX_FORMAT_UMSK                              (~(((1ULL<<DBI_CR_DBI_PIX_FORMAT_LEN)-1)<<DBI_CR_DBI_PIX_FORMAT_POS))

/* 0xC : dbi_prd */
#define DBI_PRD_OFFSET                                          (0xC)
#define DBI_CR_DBI_PRD_S                                        DBI_CR_DBI_PRD_S
#define DBI_CR_DBI_PRD_S_POS                                    (0U)
#define DBI_CR_DBI_PRD_S_LEN                                    (8U)
#define DBI_CR_DBI_PRD_S_MSK                                    (((1ULL<<DBI_CR_DBI_PRD_S_LEN)-1)<<DBI_CR_DBI_PRD_S_POS)
#define DBI_CR_DBI_PRD_S_UMSK                                   (~(((1ULL<<DBI_CR_DBI_PRD_S_LEN)-1)<<DBI_CR_DBI_PRD_S_POS))
#define DBI_CR_DBI_PRD_I                                        DBI_CR_DBI_PRD_I
#define DBI_CR_DBI_PRD_I_POS                                    (8U)
#define DBI_CR_DBI_PRD_I_LEN                                    (8U)
#define DBI_CR_DBI_PRD_I_MSK                                    (((1ULL<<DBI_CR_DBI_PRD_I_LEN)-1)<<DBI_CR_DBI_PRD_I_POS)
#define DBI_CR_DBI_PRD_I_UMSK                                   (~(((1ULL<<DBI_CR_DBI_PRD_I_LEN)-1)<<DBI_CR_DBI_PRD_I_POS))
#define DBI_CR_DBI_PRD_D_PH_0                                   DBI_CR_DBI_PRD_D_PH_0
#define DBI_CR_DBI_PRD_D_PH_0_POS                               (16U)
#define DBI_CR_DBI_PRD_D_PH_0_LEN                               (8U)
#define DBI_CR_DBI_PRD_D_PH_0_MSK                               (((1ULL<<DBI_CR_DBI_PRD_D_PH_0_LEN)-1)<<DBI_CR_DBI_PRD_D_PH_0_POS)
#define DBI_CR_DBI_PRD_D_PH_0_UMSK                              (~(((1ULL<<DBI_CR_DBI_PRD_D_PH_0_LEN)-1)<<DBI_CR_DBI_PRD_D_PH_0_POS))
#define DBI_CR_DBI_PRD_D_PH_1                                   DBI_CR_DBI_PRD_D_PH_1
#define DBI_CR_DBI_PRD_D_PH_1_POS                               (24U)
#define DBI_CR_DBI_PRD_D_PH_1_LEN                               (8U)
#define DBI_CR_DBI_PRD_D_PH_1_MSK                               (((1ULL<<DBI_CR_DBI_PRD_D_PH_1_LEN)-1)<<DBI_CR_DBI_PRD_D_PH_1_POS)
#define DBI_CR_DBI_PRD_D_PH_1_UMSK                              (~(((1ULL<<DBI_CR_DBI_PRD_D_PH_1_LEN)-1)<<DBI_CR_DBI_PRD_D_PH_1_POS))

/* 0x10 : dbi_cmd */
#define DBI_CMD_OFFSET                                          (0x10)
#define DBI_CR_DBI_CMD                                          DBI_CR_DBI_CMD
#define DBI_CR_DBI_CMD_POS                                      (0U)
#define DBI_CR_DBI_CMD_LEN                                      (8U)
#define DBI_CR_DBI_CMD_MSK                                      (((1ULL<<DBI_CR_DBI_CMD_LEN)-1)<<DBI_CR_DBI_CMD_POS)
#define DBI_CR_DBI_CMD_UMSK                                     (~(((1ULL<<DBI_CR_DBI_CMD_LEN)-1)<<DBI_CR_DBI_CMD_POS))

/* 0x14 : dbi_qspi_adr */
#define DBI_QSPI_ADR_OFFSET                                     (0x14)
#define DBI_CR_QSPI_ADR                                         DBI_CR_QSPI_ADR
#define DBI_CR_QSPI_ADR_POS                                     (0U)
#define DBI_CR_QSPI_ADR_LEN                                     (32U)
#define DBI_CR_QSPI_ADR_MSK                                     (((1ULL<<DBI_CR_QSPI_ADR_LEN)-1)<<DBI_CR_QSPI_ADR_POS)
#define DBI_CR_QSPI_ADR_UMSK                                    (~(((1ULL<<DBI_CR_QSPI_ADR_LEN)-1)<<DBI_CR_QSPI_ADR_POS))

/* 0x18 : dbi_rdata_0 */
#define DBI_RDATA_0_OFFSET                                      (0x18)
#define DBI_STS_DBI_RDATA_0                                     DBI_STS_DBI_RDATA_0
#define DBI_STS_DBI_RDATA_0_POS                                 (0U)
#define DBI_STS_DBI_RDATA_0_LEN                                 (32U)
#define DBI_STS_DBI_RDATA_0_MSK                                 (((1ULL<<DBI_STS_DBI_RDATA_0_LEN)-1)<<DBI_STS_DBI_RDATA_0_POS)
#define DBI_STS_DBI_RDATA_0_UMSK                                (~(((1ULL<<DBI_STS_DBI_RDATA_0_LEN)-1)<<DBI_STS_DBI_RDATA_0_POS))

/* 0x1C : dbi_rdata_1 */
#define DBI_RDATA_1_OFFSET                                      (0x1C)
#define DBI_STS_DBI_RDATA_1                                     DBI_STS_DBI_RDATA_1
#define DBI_STS_DBI_RDATA_1_POS                                 (0U)
#define DBI_STS_DBI_RDATA_1_LEN                                 (32U)
#define DBI_STS_DBI_RDATA_1_MSK                                 (((1ULL<<DBI_STS_DBI_RDATA_1_LEN)-1)<<DBI_STS_DBI_RDATA_1_POS)
#define DBI_STS_DBI_RDATA_1_UMSK                                (~(((1ULL<<DBI_STS_DBI_RDATA_1_LEN)-1)<<DBI_STS_DBI_RDATA_1_POS))

/* 0x30 : dbi_int_sts */
#define DBI_INT_STS_OFFSET                                      (0x30)
#define DBI_END_INT                                             DBI_END_INT
#define DBI_END_INT_POS                                         (0U)
#define DBI_END_INT_LEN                                         (1U)
#define DBI_END_INT_MSK                                         (((1ULL<<DBI_END_INT_LEN)-1)<<DBI_END_INT_POS)
#define DBI_END_INT_UMSK                                        (~(((1ULL<<DBI_END_INT_LEN)-1)<<DBI_END_INT_POS))
#define DBI_TXF_INT                                             DBI_TXF_INT
#define DBI_TXF_INT_POS                                         (1U)
#define DBI_TXF_INT_LEN                                         (1U)
#define DBI_TXF_INT_MSK                                         (((1ULL<<DBI_TXF_INT_LEN)-1)<<DBI_TXF_INT_POS)
#define DBI_TXF_INT_UMSK                                        (~(((1ULL<<DBI_TXF_INT_LEN)-1)<<DBI_TXF_INT_POS))
#define DBI_FER_INT                                             DBI_FER_INT
#define DBI_FER_INT_POS                                         (2U)
#define DBI_FER_INT_LEN                                         (1U)
#define DBI_FER_INT_MSK                                         (((1ULL<<DBI_FER_INT_LEN)-1)<<DBI_FER_INT_POS)
#define DBI_FER_INT_UMSK                                        (~(((1ULL<<DBI_FER_INT_LEN)-1)<<DBI_FER_INT_POS))
#define DBI_CR_DBI_END_MASK                                     DBI_CR_DBI_END_MASK
#define DBI_CR_DBI_END_MASK_POS                                 (8U)
#define DBI_CR_DBI_END_MASK_LEN                                 (1U)
#define DBI_CR_DBI_END_MASK_MSK                                 (((1ULL<<DBI_CR_DBI_END_MASK_LEN)-1)<<DBI_CR_DBI_END_MASK_POS)
#define DBI_CR_DBI_END_MASK_UMSK                                (~(((1ULL<<DBI_CR_DBI_END_MASK_LEN)-1)<<DBI_CR_DBI_END_MASK_POS))
#define DBI_CR_DBI_TXF_MASK                                     DBI_CR_DBI_TXF_MASK
#define DBI_CR_DBI_TXF_MASK_POS                                 (9U)
#define DBI_CR_DBI_TXF_MASK_LEN                                 (1U)
#define DBI_CR_DBI_TXF_MASK_MSK                                 (((1ULL<<DBI_CR_DBI_TXF_MASK_LEN)-1)<<DBI_CR_DBI_TXF_MASK_POS)
#define DBI_CR_DBI_TXF_MASK_UMSK                                (~(((1ULL<<DBI_CR_DBI_TXF_MASK_LEN)-1)<<DBI_CR_DBI_TXF_MASK_POS))
#define DBI_CR_DBI_FER_MASK                                     DBI_CR_DBI_FER_MASK
#define DBI_CR_DBI_FER_MASK_POS                                 (10U)
#define DBI_CR_DBI_FER_MASK_LEN                                 (1U)
#define DBI_CR_DBI_FER_MASK_MSK                                 (((1ULL<<DBI_CR_DBI_FER_MASK_LEN)-1)<<DBI_CR_DBI_FER_MASK_POS)
#define DBI_CR_DBI_FER_MASK_UMSK                                (~(((1ULL<<DBI_CR_DBI_FER_MASK_LEN)-1)<<DBI_CR_DBI_FER_MASK_POS))
#define DBI_CR_DBI_END_CLR                                      DBI_CR_DBI_END_CLR
#define DBI_CR_DBI_END_CLR_POS                                  (16U)
#define DBI_CR_DBI_END_CLR_LEN                                  (1U)
#define DBI_CR_DBI_END_CLR_MSK                                  (((1ULL<<DBI_CR_DBI_END_CLR_LEN)-1)<<DBI_CR_DBI_END_CLR_POS)
#define DBI_CR_DBI_END_CLR_UMSK                                 (~(((1ULL<<DBI_CR_DBI_END_CLR_LEN)-1)<<DBI_CR_DBI_END_CLR_POS))
#define DBI_CR_DBI_END_EN                                       DBI_CR_DBI_END_EN
#define DBI_CR_DBI_END_EN_POS                                   (24U)
#define DBI_CR_DBI_END_EN_LEN                                   (1U)
#define DBI_CR_DBI_END_EN_MSK                                   (((1ULL<<DBI_CR_DBI_END_EN_LEN)-1)<<DBI_CR_DBI_END_EN_POS)
#define DBI_CR_DBI_END_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_END_EN_LEN)-1)<<DBI_CR_DBI_END_EN_POS))
#define DBI_CR_DBI_TXF_EN                                       DBI_CR_DBI_TXF_EN
#define DBI_CR_DBI_TXF_EN_POS                                   (25U)
#define DBI_CR_DBI_TXF_EN_LEN                                   (1U)
#define DBI_CR_DBI_TXF_EN_MSK                                   (((1ULL<<DBI_CR_DBI_TXF_EN_LEN)-1)<<DBI_CR_DBI_TXF_EN_POS)
#define DBI_CR_DBI_TXF_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_TXF_EN_LEN)-1)<<DBI_CR_DBI_TXF_EN_POS))
#define DBI_CR_DBI_FER_EN                                       DBI_CR_DBI_FER_EN
#define DBI_CR_DBI_FER_EN_POS                                   (26U)
#define DBI_CR_DBI_FER_EN_LEN                                   (1U)
#define DBI_CR_DBI_FER_EN_MSK                                   (((1ULL<<DBI_CR_DBI_FER_EN_LEN)-1)<<DBI_CR_DBI_FER_EN_POS)
#define DBI_CR_DBI_FER_EN_UMSK                                  (~(((1ULL<<DBI_CR_DBI_FER_EN_LEN)-1)<<DBI_CR_DBI_FER_EN_POS))

/* 0x60 : dbi_yuv_rgb_config_0 */
#define DBI_YUV_RGB_CONFIG_0_OFFSET                             (0x60)
#define DBI_CR_Y2R_PRE_0                                        DBI_CR_Y2R_PRE_0
#define DBI_CR_Y2R_PRE_0_POS                                    (0U)
#define DBI_CR_Y2R_PRE_0_LEN                                    (9U)
#define DBI_CR_Y2R_PRE_0_MSK                                    (((1ULL<<DBI_CR_Y2R_PRE_0_LEN)-1)<<DBI_CR_Y2R_PRE_0_POS)
#define DBI_CR_Y2R_PRE_0_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_PRE_0_LEN)-1)<<DBI_CR_Y2R_PRE_0_POS))
#define DBI_CR_Y2R_PRE_1                                        DBI_CR_Y2R_PRE_1
#define DBI_CR_Y2R_PRE_1_POS                                    (10U)
#define DBI_CR_Y2R_PRE_1_LEN                                    (9U)
#define DBI_CR_Y2R_PRE_1_MSK                                    (((1ULL<<DBI_CR_Y2R_PRE_1_LEN)-1)<<DBI_CR_Y2R_PRE_1_POS)
#define DBI_CR_Y2R_PRE_1_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_PRE_1_LEN)-1)<<DBI_CR_Y2R_PRE_1_POS))
#define DBI_CR_Y2R_PRE_2                                        DBI_CR_Y2R_PRE_2
#define DBI_CR_Y2R_PRE_2_POS                                    (20U)
#define DBI_CR_Y2R_PRE_2_LEN                                    (9U)
#define DBI_CR_Y2R_PRE_2_MSK                                    (((1ULL<<DBI_CR_Y2R_PRE_2_LEN)-1)<<DBI_CR_Y2R_PRE_2_POS)
#define DBI_CR_Y2R_PRE_2_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_PRE_2_LEN)-1)<<DBI_CR_Y2R_PRE_2_POS))
#define DBI_CR_Y2R_EN                                           DBI_CR_Y2R_EN
#define DBI_CR_Y2R_EN_POS                                       (31U)
#define DBI_CR_Y2R_EN_LEN                                       (1U)
#define DBI_CR_Y2R_EN_MSK                                       (((1ULL<<DBI_CR_Y2R_EN_LEN)-1)<<DBI_CR_Y2R_EN_POS)
#define DBI_CR_Y2R_EN_UMSK                                      (~(((1ULL<<DBI_CR_Y2R_EN_LEN)-1)<<DBI_CR_Y2R_EN_POS))

/* 0x64 : dbi_yuv_rgb_config_1 */
#define DBI_YUV_RGB_CONFIG_1_OFFSET                             (0x64)
#define DBI_CR_Y2R_POS_0                                        DBI_CR_Y2R_POS_0
#define DBI_CR_Y2R_POS_0_POS                                    (0U)
#define DBI_CR_Y2R_POS_0_LEN                                    (9U)
#define DBI_CR_Y2R_POS_0_MSK                                    (((1ULL<<DBI_CR_Y2R_POS_0_LEN)-1)<<DBI_CR_Y2R_POS_0_POS)
#define DBI_CR_Y2R_POS_0_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_POS_0_LEN)-1)<<DBI_CR_Y2R_POS_0_POS))
#define DBI_CR_Y2R_POS_1                                        DBI_CR_Y2R_POS_1
#define DBI_CR_Y2R_POS_1_POS                                    (10U)
#define DBI_CR_Y2R_POS_1_LEN                                    (9U)
#define DBI_CR_Y2R_POS_1_MSK                                    (((1ULL<<DBI_CR_Y2R_POS_1_LEN)-1)<<DBI_CR_Y2R_POS_1_POS)
#define DBI_CR_Y2R_POS_1_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_POS_1_LEN)-1)<<DBI_CR_Y2R_POS_1_POS))
#define DBI_CR_Y2R_POS_2                                        DBI_CR_Y2R_POS_2
#define DBI_CR_Y2R_POS_2_POS                                    (20U)
#define DBI_CR_Y2R_POS_2_LEN                                    (9U)
#define DBI_CR_Y2R_POS_2_MSK                                    (((1ULL<<DBI_CR_Y2R_POS_2_LEN)-1)<<DBI_CR_Y2R_POS_2_POS)
#define DBI_CR_Y2R_POS_2_UMSK                                   (~(((1ULL<<DBI_CR_Y2R_POS_2_LEN)-1)<<DBI_CR_Y2R_POS_2_POS))

/* 0x68 : dbi_yuv_rgb_config_2 */
#define DBI_YUV_RGB_CONFIG_2_OFFSET                             (0x68)
#define DBI_CR_Y2R_MTX_00                                       DBI_CR_Y2R_MTX_00
#define DBI_CR_Y2R_MTX_00_POS                                   (0U)
#define DBI_CR_Y2R_MTX_00_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_00_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_00_LEN)-1)<<DBI_CR_Y2R_MTX_00_POS)
#define DBI_CR_Y2R_MTX_00_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_00_LEN)-1)<<DBI_CR_Y2R_MTX_00_POS))
#define DBI_CR_Y2R_MTX_01                                       DBI_CR_Y2R_MTX_01
#define DBI_CR_Y2R_MTX_01_POS                                   (12U)
#define DBI_CR_Y2R_MTX_01_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_01_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_01_LEN)-1)<<DBI_CR_Y2R_MTX_01_POS)
#define DBI_CR_Y2R_MTX_01_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_01_LEN)-1)<<DBI_CR_Y2R_MTX_01_POS))
#define DBI_CR_Y2R_MTX_02_L                                     DBI_CR_Y2R_MTX_02_L
#define DBI_CR_Y2R_MTX_02_L_POS                                 (24U)
#define DBI_CR_Y2R_MTX_02_L_LEN                                 (8U)
#define DBI_CR_Y2R_MTX_02_L_MSK                                 (((1ULL<<DBI_CR_Y2R_MTX_02_L_LEN)-1)<<DBI_CR_Y2R_MTX_02_L_POS)
#define DBI_CR_Y2R_MTX_02_L_UMSK                                (~(((1ULL<<DBI_CR_Y2R_MTX_02_L_LEN)-1)<<DBI_CR_Y2R_MTX_02_L_POS))

/* 0x6C : dbi_yuv_rgb_config_3 */
#define DBI_YUV_RGB_CONFIG_3_OFFSET                             (0x6C)
#define DBI_CR_Y2R_MTX_02_U                                     DBI_CR_Y2R_MTX_02_U
#define DBI_CR_Y2R_MTX_02_U_POS                                 (0U)
#define DBI_CR_Y2R_MTX_02_U_LEN                                 (4U)
#define DBI_CR_Y2R_MTX_02_U_MSK                                 (((1ULL<<DBI_CR_Y2R_MTX_02_U_LEN)-1)<<DBI_CR_Y2R_MTX_02_U_POS)
#define DBI_CR_Y2R_MTX_02_U_UMSK                                (~(((1ULL<<DBI_CR_Y2R_MTX_02_U_LEN)-1)<<DBI_CR_Y2R_MTX_02_U_POS))
#define DBI_CR_Y2R_MTX_10                                       DBI_CR_Y2R_MTX_10
#define DBI_CR_Y2R_MTX_10_POS                                   (4U)
#define DBI_CR_Y2R_MTX_10_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_10_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_10_LEN)-1)<<DBI_CR_Y2R_MTX_10_POS)
#define DBI_CR_Y2R_MTX_10_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_10_LEN)-1)<<DBI_CR_Y2R_MTX_10_POS))
#define DBI_CR_Y2R_MTX_11                                       DBI_CR_Y2R_MTX_11
#define DBI_CR_Y2R_MTX_11_POS                                   (16U)
#define DBI_CR_Y2R_MTX_11_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_11_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_11_LEN)-1)<<DBI_CR_Y2R_MTX_11_POS)
#define DBI_CR_Y2R_MTX_11_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_11_LEN)-1)<<DBI_CR_Y2R_MTX_11_POS))
#define DBI_CR_Y2R_MTX_12_L                                     DBI_CR_Y2R_MTX_12_L
#define DBI_CR_Y2R_MTX_12_L_POS                                 (28U)
#define DBI_CR_Y2R_MTX_12_L_LEN                                 (4U)
#define DBI_CR_Y2R_MTX_12_L_MSK                                 (((1ULL<<DBI_CR_Y2R_MTX_12_L_LEN)-1)<<DBI_CR_Y2R_MTX_12_L_POS)
#define DBI_CR_Y2R_MTX_12_L_UMSK                                (~(((1ULL<<DBI_CR_Y2R_MTX_12_L_LEN)-1)<<DBI_CR_Y2R_MTX_12_L_POS))

/* 0x70 : dbi_yuv_rgb_config_4 */
#define DBI_YUV_RGB_CONFIG_4_OFFSET                             (0x70)
#define DBI_CR_Y2R_MTX_12_U                                     DBI_CR_Y2R_MTX_12_U
#define DBI_CR_Y2R_MTX_12_U_POS                                 (0U)
#define DBI_CR_Y2R_MTX_12_U_LEN                                 (8U)
#define DBI_CR_Y2R_MTX_12_U_MSK                                 (((1ULL<<DBI_CR_Y2R_MTX_12_U_LEN)-1)<<DBI_CR_Y2R_MTX_12_U_POS)
#define DBI_CR_Y2R_MTX_12_U_UMSK                                (~(((1ULL<<DBI_CR_Y2R_MTX_12_U_LEN)-1)<<DBI_CR_Y2R_MTX_12_U_POS))
#define DBI_CR_Y2R_MTX_20                                       DBI_CR_Y2R_MTX_20
#define DBI_CR_Y2R_MTX_20_POS                                   (8U)
#define DBI_CR_Y2R_MTX_20_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_20_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_20_LEN)-1)<<DBI_CR_Y2R_MTX_20_POS)
#define DBI_CR_Y2R_MTX_20_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_20_LEN)-1)<<DBI_CR_Y2R_MTX_20_POS))
#define DBI_CR_Y2R_MTX_21                                       DBI_CR_Y2R_MTX_21
#define DBI_CR_Y2R_MTX_21_POS                                   (20U)
#define DBI_CR_Y2R_MTX_21_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_21_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_21_LEN)-1)<<DBI_CR_Y2R_MTX_21_POS)
#define DBI_CR_Y2R_MTX_21_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_21_LEN)-1)<<DBI_CR_Y2R_MTX_21_POS))

/* 0x74 : dbi_yuv_rgb_config_5 */
#define DBI_YUV_RGB_CONFIG_5_OFFSET                             (0x74)
#define DBI_CR_Y2R_MTX_22                                       DBI_CR_Y2R_MTX_22
#define DBI_CR_Y2R_MTX_22_POS                                   (0U)
#define DBI_CR_Y2R_MTX_22_LEN                                   (12U)
#define DBI_CR_Y2R_MTX_22_MSK                                   (((1ULL<<DBI_CR_Y2R_MTX_22_LEN)-1)<<DBI_CR_Y2R_MTX_22_POS)
#define DBI_CR_Y2R_MTX_22_UMSK                                  (~(((1ULL<<DBI_CR_Y2R_MTX_22_LEN)-1)<<DBI_CR_Y2R_MTX_22_POS))

/* 0x80 : dbi_fifo_config_0 */
#define DBI_FIFO_CONFIG_0_OFFSET                                (0x80)
#define DBI_DMA_TX_EN                                           DBI_DMA_TX_EN
#define DBI_DMA_TX_EN_POS                                       (0U)
#define DBI_DMA_TX_EN_LEN                                       (1U)
#define DBI_DMA_TX_EN_MSK                                       (((1ULL<<DBI_DMA_TX_EN_LEN)-1)<<DBI_DMA_TX_EN_POS)
#define DBI_DMA_TX_EN_UMSK                                      (~(((1ULL<<DBI_DMA_TX_EN_LEN)-1)<<DBI_DMA_TX_EN_POS))
#define DBI_TX_FIFO_CLR                                         DBI_TX_FIFO_CLR
#define DBI_TX_FIFO_CLR_POS                                     (2U)
#define DBI_TX_FIFO_CLR_LEN                                     (1U)
#define DBI_TX_FIFO_CLR_MSK                                     (((1ULL<<DBI_TX_FIFO_CLR_LEN)-1)<<DBI_TX_FIFO_CLR_POS)
#define DBI_TX_FIFO_CLR_UMSK                                    (~(((1ULL<<DBI_TX_FIFO_CLR_LEN)-1)<<DBI_TX_FIFO_CLR_POS))
#define DBI_TX_FIFO_OVERFLOW                                    DBI_TX_FIFO_OVERFLOW
#define DBI_TX_FIFO_OVERFLOW_POS                                (4U)
#define DBI_TX_FIFO_OVERFLOW_LEN                                (1U)
#define DBI_TX_FIFO_OVERFLOW_MSK                                (((1ULL<<DBI_TX_FIFO_OVERFLOW_LEN)-1)<<DBI_TX_FIFO_OVERFLOW_POS)
#define DBI_TX_FIFO_OVERFLOW_UMSK                               (~(((1ULL<<DBI_TX_FIFO_OVERFLOW_LEN)-1)<<DBI_TX_FIFO_OVERFLOW_POS))
#define DBI_TX_FIFO_UNDERFLOW                                   DBI_TX_FIFO_UNDERFLOW
#define DBI_TX_FIFO_UNDERFLOW_POS                               (5U)
#define DBI_TX_FIFO_UNDERFLOW_LEN                               (1U)
#define DBI_TX_FIFO_UNDERFLOW_MSK                               (((1ULL<<DBI_TX_FIFO_UNDERFLOW_LEN)-1)<<DBI_TX_FIFO_UNDERFLOW_POS)
#define DBI_TX_FIFO_UNDERFLOW_UMSK                              (~(((1ULL<<DBI_TX_FIFO_UNDERFLOW_LEN)-1)<<DBI_TX_FIFO_UNDERFLOW_POS))
#define DBI_FIFO_YUV_MODE                                       DBI_FIFO_YUV_MODE
#define DBI_FIFO_YUV_MODE_POS                                   (28U)
#define DBI_FIFO_YUV_MODE_LEN                                   (1U)
#define DBI_FIFO_YUV_MODE_MSK                                   (((1ULL<<DBI_FIFO_YUV_MODE_LEN)-1)<<DBI_FIFO_YUV_MODE_POS)
#define DBI_FIFO_YUV_MODE_UMSK                                  (~(((1ULL<<DBI_FIFO_YUV_MODE_LEN)-1)<<DBI_FIFO_YUV_MODE_POS))
#define DBI_FIFO_FORMAT                                         DBI_FIFO_FORMAT
#define DBI_FIFO_FORMAT_POS                                     (29U)
#define DBI_FIFO_FORMAT_LEN                                     (3U)
#define DBI_FIFO_FORMAT_MSK                                     (((1ULL<<DBI_FIFO_FORMAT_LEN)-1)<<DBI_FIFO_FORMAT_POS)
#define DBI_FIFO_FORMAT_UMSK                                    (~(((1ULL<<DBI_FIFO_FORMAT_LEN)-1)<<DBI_FIFO_FORMAT_POS))

/* 0x84 : dbi_fifo_config_1 */
#define DBI_FIFO_CONFIG_1_OFFSET                                (0x84)
#define DBI_TX_FIFO_CNT                                         DBI_TX_FIFO_CNT
#define DBI_TX_FIFO_CNT_POS                                     (0U)
#define DBI_TX_FIFO_CNT_LEN                                     (4U)
#define DBI_TX_FIFO_CNT_MSK                                     (((1ULL<<DBI_TX_FIFO_CNT_LEN)-1)<<DBI_TX_FIFO_CNT_POS)
#define DBI_TX_FIFO_CNT_UMSK                                    (~(((1ULL<<DBI_TX_FIFO_CNT_LEN)-1)<<DBI_TX_FIFO_CNT_POS))
#define DBI_TX_FIFO_TH                                          DBI_TX_FIFO_TH
#define DBI_TX_FIFO_TH_POS                                      (16U)
#define DBI_TX_FIFO_TH_LEN                                      (3U)
#define DBI_TX_FIFO_TH_MSK                                      (((1ULL<<DBI_TX_FIFO_TH_LEN)-1)<<DBI_TX_FIFO_TH_POS)
#define DBI_TX_FIFO_TH_UMSK                                     (~(((1ULL<<DBI_TX_FIFO_TH_LEN)-1)<<DBI_TX_FIFO_TH_POS))

/* 0x88 : dbi_fifo_wdata */
#define DBI_FIFO_WDATA_OFFSET                                   (0x88)
#define DBI_FIFO_WDATA                                          DBI_FIFO_WDATA
#define DBI_FIFO_WDATA_POS                                      (0U)
#define DBI_FIFO_WDATA_LEN                                      (32U)
#define DBI_FIFO_WDATA_MSK                                      (((1ULL<<DBI_FIFO_WDATA_LEN)-1)<<DBI_FIFO_WDATA_POS)
#define DBI_FIFO_WDATA_UMSK                                     (~(((1ULL<<DBI_FIFO_WDATA_LEN)-1)<<DBI_FIFO_WDATA_POS))

/* 0xFC : dbi_dummy */
#define DBI_DUMMY_OFFSET                                        (0xFC)
#define DBI_DUMMY                                               DBI_DUMMY
#define DBI_DUMMY_POS                                           (0U)
#define DBI_DUMMY_LEN                                           (8U)
#define DBI_DUMMY_MSK                                           (((1ULL<<DBI_DUMMY_LEN)-1)<<DBI_DUMMY_POS)
#define DBI_DUMMY_UMSK                                          (~(((1ULL<<DBI_DUMMY_LEN)-1)<<DBI_DUMMY_POS))


struct  dbi_reg {
    /* 0x0 : dbi_config */
    union {
        struct {
            uint32_t cr_dbi_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_dbi_sel                     :  2; /* [ 2: 1],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t cr_dbi_cmd_en                  :  1; /* [    4],        r/w,        0x1 */
            uint32_t cr_dbi_dat_en                  :  1; /* [    5],        r/w,        0x1 */
            uint32_t cr_dbi_dat_wr                  :  1; /* [    6],        r/w,        0x1 */
            uint32_t cr_dbi_dat_tp                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t cr_dbi_dat_bc                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t cr_dbi_scl_pol                 :  1; /* [   16],        r/w,        0x1 */
            uint32_t cr_dbi_scl_ph                  :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_dbi_cont_en                 :  1; /* [   18],        r/w,        0x1 */
            uint32_t cr_dbi_dmy_en                  :  1; /* [   19],        r/w,        0x0 */
            uint32_t cr_dbi_dmy_cnt                 :  4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_dbi_cs_stretch              :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_30                 :  6; /* [30:25],       rsvd,        0x0 */
            uint32_t sts_dbi_bus_busy               :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_config;

    /* 0x4 : qspi_config */
    union {
        struct {
            uint32_t cr_qspi_cmd_4b                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_qspi_adr_4b                 :  1; /* [    1],        r/w,        0x1 */
            uint32_t cr_qspi_dat_4b                 :  1; /* [    2],        r/w,        0x1 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t cr_qspi_adr_bc                 :  2; /* [ 5: 4],        r/w,        0x2 */
            uint32_t reserved_6_31                  : 26; /* [31: 6],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } qspi_config;

    /* 0x8 : dbi_pix_cnt */
    union {
        struct {
            uint32_t cr_dbi_pix_cnt                 : 24; /* [23: 0],        r/w,        0x0 */
            uint32_t reserved_24_30                 :  7; /* [30:24],       rsvd,        0x0 */
            uint32_t cr_dbi_pix_format              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_pix_cnt;

    /* 0xC : dbi_prd */
    union {
        struct {
            uint32_t cr_dbi_prd_s                   :  8; /* [ 7: 0],        r/w,        0xf */
            uint32_t cr_dbi_prd_i                   :  8; /* [15: 8],        r/w,        0xf */
            uint32_t cr_dbi_prd_d_ph_0              :  8; /* [23:16],        r/w,        0xf */
            uint32_t cr_dbi_prd_d_ph_1              :  8; /* [31:24],        r/w,        0xf */
        }BF;
        uint32_t WORD;
    } dbi_prd;

    /* 0x10 : dbi_cmd */
    union {
        struct {
            uint32_t cr_dbi_cmd                     :  8; /* [ 7: 0],        r/w,       0x2c */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_cmd;

    /* 0x14 : dbi_qspi_adr */
    union {
        struct {
            uint32_t cr_qspi_adr                    : 32; /* [31: 0],        r/w,     0x2c00 */
        }BF;
        uint32_t WORD;
    } dbi_qspi_adr;

    /* 0x18 : dbi_rdata_0 */
    union {
        struct {
            uint32_t sts_dbi_rdata_0                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_rdata_0;

    /* 0x1C : dbi_rdata_1 */
    union {
        struct {
            uint32_t sts_dbi_rdata_1                : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_rdata_1;

    /* 0x20  reserved */
    uint8_t RESERVED0x20[16];

    /* 0x30 : dbi_int_sts */
    union {
        struct {
            uint32_t dbi_end_int                    :  1; /* [    0],          r,        0x0 */
            uint32_t dbi_txf_int                    :  1; /* [    1],          r,        0x1 */
            uint32_t dbi_fer_int                    :  1; /* [    2],          r,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t cr_dbi_end_mask                :  1; /* [    8],        r/w,        0x1 */
            uint32_t cr_dbi_txf_mask                :  1; /* [    9],        r/w,        0x1 */
            uint32_t cr_dbi_fer_mask                :  1; /* [   10],        r/w,        0x1 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t cr_dbi_end_clr                 :  1; /* [   16],        w1c,        0x0 */
            uint32_t rsvd_17                        :  1; /* [   17],       rsvd,        0x0 */
            uint32_t rsvd_18                        :  1; /* [   18],       rsvd,        0x0 */
            uint32_t reserved_19_23                 :  5; /* [23:19],       rsvd,        0x0 */
            uint32_t cr_dbi_end_en                  :  1; /* [   24],        r/w,        0x1 */
            uint32_t cr_dbi_txf_en                  :  1; /* [   25],        r/w,        0x1 */
            uint32_t cr_dbi_fer_en                  :  1; /* [   26],        r/w,        0x1 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_int_sts;

    /* 0x34  reserved */
    uint8_t RESERVED0x34[44];

    /* 0x60 : dbi_yuv_rgb_config_0 */
    union {
        struct {
            uint32_t cr_y2r_pre_0                   :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_y2r_pre_1                   :  9; /* [18:10],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t cr_y2r_pre_2                   :  9; /* [28:20],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t cr_y2r_en                      :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_0;

    /* 0x64 : dbi_yuv_rgb_config_1 */
    union {
        struct {
            uint32_t cr_y2r_pos_0                   :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_y2r_pos_1                   :  9; /* [18:10],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t cr_y2r_pos_2                   :  9; /* [28:20],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_1;

    /* 0x68 : dbi_yuv_rgb_config_2 */
    union {
        struct {
            uint32_t cr_y2r_mtx_00                  : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_01                  : 12; /* [23:12],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_02_l                :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_2;

    /* 0x6C : dbi_yuv_rgb_config_3 */
    union {
        struct {
            uint32_t cr_y2r_mtx_02_u                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_10                  : 12; /* [15: 4],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_11                  : 12; /* [27:16],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_12_l                :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_3;

    /* 0x70 : dbi_yuv_rgb_config_4 */
    union {
        struct {
            uint32_t cr_y2r_mtx_12_u                :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_20                  : 12; /* [19: 8],        r/w,        0x0 */
            uint32_t cr_y2r_mtx_21                  : 12; /* [31:20],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_4;

    /* 0x74 : dbi_yuv_rgb_config_5 */
    union {
        struct {
            uint32_t cr_y2r_mtx_22                  : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_yuv_rgb_config_5;

    /* 0x78  reserved */
    uint8_t RESERVED0x78[8];

    /* 0x80 : dbi_fifo_config_0 */
    union {
        struct {
            uint32_t dbi_dma_tx_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t tx_fifo_clr                    :  1; /* [    2],        w1c,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t tx_fifo_overflow               :  1; /* [    4],          r,        0x0 */
            uint32_t tx_fifo_underflow              :  1; /* [    5],          r,        0x0 */
            uint32_t reserved_6_27                  : 22; /* [27: 6],       rsvd,        0x0 */
            uint32_t fifo_yuv_mode                  :  1; /* [   28],        r/w,        0x0 */
            uint32_t fifo_format                    :  3; /* [31:29],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_fifo_config_0;

    /* 0x84 : dbi_fifo_config_1 */
    union {
        struct {
            uint32_t tx_fifo_cnt                    :  4; /* [ 3: 0],          r,        0x8 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t tx_fifo_th                     :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_fifo_config_1;

    /* 0x88 : dbi_fifo_wdata */
    union {
        struct {
            uint32_t dbi_fifo_wdata                 : 32; /* [31: 0],          w,          x */
        }BF;
        uint32_t WORD;
    } dbi_fifo_wdata;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[112];

    /* 0xFC : dbi_dummy */
    union {
        struct {
            uint32_t dbi_dummy                      :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dbi_dummy;

};

typedef volatile struct dbi_reg dbi_reg_t;


#endif  /* __DBI_REG_H__ */
