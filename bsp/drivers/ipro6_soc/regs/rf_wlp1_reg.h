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

#ifndef  __RF_WLP1_REG_H__
#define  __RF_WLP1_REG_H__

#include "ipro6.h"

/* 0x400 : wlp1_cfg0 */
#define RF_WLP1_WLP1_CFG0_OFFSET                                (0x400)
#define RF_WLP1_WL1_SLV_QOS                                     RF_WLP1_WL1_SLV_QOS
#define RF_WLP1_WL1_SLV_QOS_POS                                 (0U)
#define RF_WLP1_WL1_SLV_QOS_LEN                                 (1U)
#define RF_WLP1_WL1_SLV_QOS_MSK                                 (((1ULL<<RF_WLP1_WL1_SLV_QOS_LEN)-1)<<RF_WLP1_WL1_SLV_QOS_POS)
#define RF_WLP1_WL1_SLV_QOS_UMSK                                (~(((1ULL<<RF_WLP1_WL1_SLV_QOS_LEN)-1)<<RF_WLP1_WL1_SLV_QOS_POS))
#define RF_WLP1_M154M_HQOS                                      RF_WLP1_M154M_HQOS
#define RF_WLP1_M154M_HQOS_POS                                  (1U)
#define RF_WLP1_M154M_HQOS_LEN                                  (1U)
#define RF_WLP1_M154M_HQOS_MSK                                  (((1ULL<<RF_WLP1_M154M_HQOS_LEN)-1)<<RF_WLP1_M154M_HQOS_POS)
#define RF_WLP1_M154M_HQOS_UMSK                                 (~(((1ULL<<RF_WLP1_M154M_HQOS_LEN)-1)<<RF_WLP1_M154M_HQOS_POS))
#define RF_WLP1_PLDMA_HQOS                                      RF_WLP1_PLDMA_HQOS
#define RF_WLP1_PLDMA_HQOS_POS                                  (2U)
#define RF_WLP1_PLDMA_HQOS_LEN                                  (1U)
#define RF_WLP1_PLDMA_HQOS_MSK                                  (((1ULL<<RF_WLP1_PLDMA_HQOS_LEN)-1)<<RF_WLP1_PLDMA_HQOS_POS)
#define RF_WLP1_PLDMA_HQOS_UMSK                                 (~(((1ULL<<RF_WLP1_PLDMA_HQOS_LEN)-1)<<RF_WLP1_PLDMA_HQOS_POS))
#define RF_WLP1_WIFI_CSI_HQOS                                   RF_WLP1_WIFI_CSI_HQOS
#define RF_WLP1_WIFI_CSI_HQOS_POS                               (3U)
#define RF_WLP1_WIFI_CSI_HQOS_LEN                               (1U)
#define RF_WLP1_WIFI_CSI_HQOS_MSK                               (((1ULL<<RF_WLP1_WIFI_CSI_HQOS_LEN)-1)<<RF_WLP1_WIFI_CSI_HQOS_POS)
#define RF_WLP1_WIFI_CSI_HQOS_UMSK                              (~(((1ULL<<RF_WLP1_WIFI_CSI_HQOS_LEN)-1)<<RF_WLP1_WIFI_CSI_HQOS_POS))
#define RF_WLP1_ARB_MODE_WL1_GP                                 RF_WLP1_ARB_MODE_WL1_GP
#define RF_WLP1_ARB_MODE_WL1_GP_POS                             (4U)
#define RF_WLP1_ARB_MODE_WL1_GP_LEN                             (1U)
#define RF_WLP1_ARB_MODE_WL1_GP_MSK                             (((1ULL<<RF_WLP1_ARB_MODE_WL1_GP_LEN)-1)<<RF_WLP1_ARB_MODE_WL1_GP_POS)
#define RF_WLP1_ARB_MODE_WL1_GP_UMSK                            (~(((1ULL<<RF_WLP1_ARB_MODE_WL1_GP_LEN)-1)<<RF_WLP1_ARB_MODE_WL1_GP_POS))
#define RF_WLP1_BMX_GATING_DIS_SCAN                             RF_WLP1_BMX_GATING_DIS_SCAN
#define RF_WLP1_BMX_GATING_DIS_SCAN_POS                         (5U)
#define RF_WLP1_BMX_GATING_DIS_SCAN_LEN                         (1U)
#define RF_WLP1_BMX_GATING_DIS_SCAN_MSK                         (((1ULL<<RF_WLP1_BMX_GATING_DIS_SCAN_LEN)-1)<<RF_WLP1_BMX_GATING_DIS_SCAN_POS)
#define RF_WLP1_BMX_GATING_DIS_SCAN_UMSK                        (~(((1ULL<<RF_WLP1_BMX_GATING_DIS_SCAN_LEN)-1)<<RF_WLP1_BMX_GATING_DIS_SCAN_POS))
#define RF_WLP1_M154_RX_IGNORE                                  RF_WLP1_M154_RX_IGNORE
#define RF_WLP1_M154_RX_IGNORE_POS                              (6U)
#define RF_WLP1_M154_RX_IGNORE_LEN                              (1U)
#define RF_WLP1_M154_RX_IGNORE_MSK                              (((1ULL<<RF_WLP1_M154_RX_IGNORE_LEN)-1)<<RF_WLP1_M154_RX_IGNORE_POS)
#define RF_WLP1_M154_RX_IGNORE_UMSK                             (~(((1ULL<<RF_WLP1_M154_RX_IGNORE_LEN)-1)<<RF_WLP1_M154_RX_IGNORE_POS))
#define RF_WLP1_FORCE_RAM_CLK_EN                                RF_WLP1_FORCE_RAM_CLK_EN
#define RF_WLP1_FORCE_RAM_CLK_EN_POS                            (7U)
#define RF_WLP1_FORCE_RAM_CLK_EN_LEN                            (1U)
#define RF_WLP1_FORCE_RAM_CLK_EN_MSK                            (((1ULL<<RF_WLP1_FORCE_RAM_CLK_EN_LEN)-1)<<RF_WLP1_FORCE_RAM_CLK_EN_POS)
#define RF_WLP1_FORCE_RAM_CLK_EN_UMSK                           (~(((1ULL<<RF_WLP1_FORCE_RAM_CLK_EN_LEN)-1)<<RF_WLP1_FORCE_RAM_CLK_EN_POS))
#define RF_WLP1_CGEN_S3_M154                                    RF_WLP1_CGEN_S3_M154
#define RF_WLP1_CGEN_S3_M154_POS                                (8U)
#define RF_WLP1_CGEN_S3_M154_LEN                                (1U)
#define RF_WLP1_CGEN_S3_M154_MSK                                (((1ULL<<RF_WLP1_CGEN_S3_M154_LEN)-1)<<RF_WLP1_CGEN_S3_M154_POS)
#define RF_WLP1_CGEN_S3_M154_UMSK                               (~(((1ULL<<RF_WLP1_CGEN_S3_M154_LEN)-1)<<RF_WLP1_CGEN_S3_M154_POS))
#define RF_WLP1_CGEN_S3_BT_BLE                                  RF_WLP1_CGEN_S3_BT_BLE
#define RF_WLP1_CGEN_S3_BT_BLE_POS                              (9U)
#define RF_WLP1_CGEN_S3_BT_BLE_LEN                              (1U)
#define RF_WLP1_CGEN_S3_BT_BLE_MSK                              (((1ULL<<RF_WLP1_CGEN_S3_BT_BLE_LEN)-1)<<RF_WLP1_CGEN_S3_BT_BLE_POS)
#define RF_WLP1_CGEN_S3_BT_BLE_UMSK                             (~(((1ULL<<RF_WLP1_CGEN_S3_BT_BLE_LEN)-1)<<RF_WLP1_CGEN_S3_BT_BLE_POS))
#define RF_WLP1_CGEN_S1_RF_TOP                                  RF_WLP1_CGEN_S1_RF_TOP
#define RF_WLP1_CGEN_S1_RF_TOP_POS                              (10U)
#define RF_WLP1_CGEN_S1_RF_TOP_LEN                              (1U)
#define RF_WLP1_CGEN_S1_RF_TOP_MSK                              (((1ULL<<RF_WLP1_CGEN_S1_RF_TOP_LEN)-1)<<RF_WLP1_CGEN_S1_RF_TOP_POS)
#define RF_WLP1_CGEN_S1_RF_TOP_UMSK                             (~(((1ULL<<RF_WLP1_CGEN_S1_RF_TOP_LEN)-1)<<RF_WLP1_CGEN_S1_RF_TOP_POS))
#define RF_WLP1_CGEN_M_WIFI_PHY                                 RF_WLP1_CGEN_M_WIFI_PHY
#define RF_WLP1_CGEN_M_WIFI_PHY_POS                             (11U)
#define RF_WLP1_CGEN_M_WIFI_PHY_LEN                             (1U)
#define RF_WLP1_CGEN_M_WIFI_PHY_MSK                             (((1ULL<<RF_WLP1_CGEN_M_WIFI_PHY_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_PHY_POS)
#define RF_WLP1_CGEN_M_WIFI_PHY_UMSK                            (~(((1ULL<<RF_WLP1_CGEN_M_WIFI_PHY_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_PHY_POS))
#define RF_WLP1_CGEN_M_WIFI_MAC_PHY                             RF_WLP1_CGEN_M_WIFI_MAC_PHY
#define RF_WLP1_CGEN_M_WIFI_MAC_PHY_POS                         (12U)
#define RF_WLP1_CGEN_M_WIFI_MAC_PHY_LEN                         (1U)
#define RF_WLP1_CGEN_M_WIFI_MAC_PHY_MSK                         (((1ULL<<RF_WLP1_CGEN_M_WIFI_MAC_PHY_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_MAC_PHY_POS)
#define RF_WLP1_CGEN_M_WIFI_MAC_PHY_UMSK                        (~(((1ULL<<RF_WLP1_CGEN_M_WIFI_MAC_PHY_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_MAC_PHY_POS))
#define RF_WLP1_CGEN_M_WIFI_PLATFORM                            RF_WLP1_CGEN_M_WIFI_PLATFORM
#define RF_WLP1_CGEN_M_WIFI_PLATFORM_POS                        (13U)
#define RF_WLP1_CGEN_M_WIFI_PLATFORM_LEN                        (1U)
#define RF_WLP1_CGEN_M_WIFI_PLATFORM_MSK                        (((1ULL<<RF_WLP1_CGEN_M_WIFI_PLATFORM_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_PLATFORM_POS)
#define RF_WLP1_CGEN_M_WIFI_PLATFORM_UMSK                       (~(((1ULL<<RF_WLP1_CGEN_M_WIFI_PLATFORM_LEN)-1)<<RF_WLP1_CGEN_M_WIFI_PLATFORM_POS))
#define RF_WLP1_CLK_PHY_EN                                      RF_WLP1_CLK_PHY_EN
#define RF_WLP1_CLK_PHY_EN_POS                                  (14U)
#define RF_WLP1_CLK_PHY_EN_LEN                                  (1U)
#define RF_WLP1_CLK_PHY_EN_MSK                                  (((1ULL<<RF_WLP1_CLK_PHY_EN_LEN)-1)<<RF_WLP1_CLK_PHY_EN_POS)
#define RF_WLP1_CLK_PHY_EN_UMSK                                 (~(((1ULL<<RF_WLP1_CLK_PHY_EN_LEN)-1)<<RF_WLP1_CLK_PHY_EN_POS))
#define RF_WLP1_M154_ZBEN                                       RF_WLP1_M154_ZBEN
#define RF_WLP1_M154_ZBEN_POS                                   (15U)
#define RF_WLP1_M154_ZBEN_LEN                                   (1U)
#define RF_WLP1_M154_ZBEN_MSK                                   (((1ULL<<RF_WLP1_M154_ZBEN_LEN)-1)<<RF_WLP1_M154_ZBEN_POS)
#define RF_WLP1_M154_ZBEN_UMSK                                  (~(((1ULL<<RF_WLP1_M154_ZBEN_LEN)-1)<<RF_WLP1_M154_ZBEN_POS))
#define RF_WLP1_BLE_CLK_SEL                                     RF_WLP1_BLE_CLK_SEL
#define RF_WLP1_BLE_CLK_SEL_POS                                 (16U)
#define RF_WLP1_BLE_CLK_SEL_LEN                                 (6U)
#define RF_WLP1_BLE_CLK_SEL_MSK                                 (((1ULL<<RF_WLP1_BLE_CLK_SEL_LEN)-1)<<RF_WLP1_BLE_CLK_SEL_POS)
#define RF_WLP1_BLE_CLK_SEL_UMSK                                (~(((1ULL<<RF_WLP1_BLE_CLK_SEL_LEN)-1)<<RF_WLP1_BLE_CLK_SEL_POS))
#define RF_WLP1_CGEN_480M_SOC                                   RF_WLP1_CGEN_480M_SOC
#define RF_WLP1_CGEN_480M_SOC_POS                               (22U)
#define RF_WLP1_CGEN_480M_SOC_LEN                               (1U)
#define RF_WLP1_CGEN_480M_SOC_MSK                               (((1ULL<<RF_WLP1_CGEN_480M_SOC_LEN)-1)<<RF_WLP1_CGEN_480M_SOC_POS)
#define RF_WLP1_CGEN_480M_SOC_UMSK                              (~(((1ULL<<RF_WLP1_CGEN_480M_SOC_LEN)-1)<<RF_WLP1_CGEN_480M_SOC_POS))
#define RF_WLP1_BLE_EN                                          RF_WLP1_BLE_EN
#define RF_WLP1_BLE_EN_POS                                      (23U)
#define RF_WLP1_BLE_EN_LEN                                      (1U)
#define RF_WLP1_BLE_EN_MSK                                      (((1ULL<<RF_WLP1_BLE_EN_LEN)-1)<<RF_WLP1_BLE_EN_POS)
#define RF_WLP1_BLE_EN_UMSK                                     (~(((1ULL<<RF_WLP1_BLE_EN_LEN)-1)<<RF_WLP1_BLE_EN_POS))
#define RF_WLP1_WIFI_MAC_CORE_DIV                               RF_WLP1_WIFI_MAC_CORE_DIV
#define RF_WLP1_WIFI_MAC_CORE_DIV_POS                           (24U)
#define RF_WLP1_WIFI_MAC_CORE_DIV_LEN                           (4U)
#define RF_WLP1_WIFI_MAC_CORE_DIV_MSK                           (((1ULL<<RF_WLP1_WIFI_MAC_CORE_DIV_LEN)-1)<<RF_WLP1_WIFI_MAC_CORE_DIV_POS)
#define RF_WLP1_WIFI_MAC_CORE_DIV_UMSK                          (~(((1ULL<<RF_WLP1_WIFI_MAC_CORE_DIV_LEN)-1)<<RF_WLP1_WIFI_MAC_CORE_DIV_POS))
#define RF_WLP1_WIFI_MAC_WT_DIV                                 RF_WLP1_WIFI_MAC_WT_DIV
#define RF_WLP1_WIFI_MAC_WT_DIV_POS                             (28U)
#define RF_WLP1_WIFI_MAC_WT_DIV_LEN                             (4U)
#define RF_WLP1_WIFI_MAC_WT_DIV_MSK                             (((1ULL<<RF_WLP1_WIFI_MAC_WT_DIV_LEN)-1)<<RF_WLP1_WIFI_MAC_WT_DIV_POS)
#define RF_WLP1_WIFI_MAC_WT_DIV_UMSK                            (~(((1ULL<<RF_WLP1_WIFI_MAC_WT_DIV_LEN)-1)<<RF_WLP1_WIFI_MAC_WT_DIV_POS))

/* 0x404 : wifi_ftm_ctrl_0 */
#define RF_WLP1_WIFI_FTM_CTRL_0_OFFSET                          (0x404)
#define RF_WLP1_TODTIMESTAMPLO                                  RF_WLP1_TODTIMESTAMPLO
#define RF_WLP1_TODTIMESTAMPLO_POS                              (0U)
#define RF_WLP1_TODTIMESTAMPLO_LEN                              (9U)
#define RF_WLP1_TODTIMESTAMPLO_MSK                              (((1ULL<<RF_WLP1_TODTIMESTAMPLO_LEN)-1)<<RF_WLP1_TODTIMESTAMPLO_POS)
#define RF_WLP1_TODTIMESTAMPLO_UMSK                             (~(((1ULL<<RF_WLP1_TODTIMESTAMPLO_LEN)-1)<<RF_WLP1_TODTIMESTAMPLO_POS))
#define RF_WLP1_FTMTOACLOCKFREQMHZ                              RF_WLP1_FTMTOACLOCKFREQMHZ
#define RF_WLP1_FTMTOACLOCKFREQMHZ_POS                          (24U)
#define RF_WLP1_FTMTOACLOCKFREQMHZ_LEN                          (8U)
#define RF_WLP1_FTMTOACLOCKFREQMHZ_MSK                          (((1ULL<<RF_WLP1_FTMTOACLOCKFREQMHZ_LEN)-1)<<RF_WLP1_FTMTOACLOCKFREQMHZ_POS)
#define RF_WLP1_FTMTOACLOCKFREQMHZ_UMSK                         (~(((1ULL<<RF_WLP1_FTMTOACLOCKFREQMHZ_LEN)-1)<<RF_WLP1_FTMTOACLOCKFREQMHZ_POS))

/* 0x408 : wifi_ftm_ctrl_1 */
#define RF_WLP1_WIFI_FTM_CTRL_1_OFFSET                          (0x408)
#define RF_WLP1_TODTIMESTAMPHI                                  RF_WLP1_TODTIMESTAMPHI
#define RF_WLP1_TODTIMESTAMPHI_POS                              (0U)
#define RF_WLP1_TODTIMESTAMPHI_LEN                              (32U)
#define RF_WLP1_TODTIMESTAMPHI_MSK                              (((1ULL<<RF_WLP1_TODTIMESTAMPHI_LEN)-1)<<RF_WLP1_TODTIMESTAMPHI_POS)
#define RF_WLP1_TODTIMESTAMPHI_UMSK                             (~(((1ULL<<RF_WLP1_TODTIMESTAMPHI_LEN)-1)<<RF_WLP1_TODTIMESTAMPHI_POS))

/* 0x40C : wifi_ftm_ctrl_2 */
#define RF_WLP1_WIFI_FTM_CTRL_2_OFFSET                          (0x40C)
#define RF_WLP1_FTMTOATIMESTAMPLO                               RF_WLP1_FTMTOATIMESTAMPLO
#define RF_WLP1_FTMTOATIMESTAMPLO_POS                           (0U)
#define RF_WLP1_FTMTOATIMESTAMPLO_LEN                           (9U)
#define RF_WLP1_FTMTOATIMESTAMPLO_MSK                           (((1ULL<<RF_WLP1_FTMTOATIMESTAMPLO_LEN)-1)<<RF_WLP1_FTMTOATIMESTAMPLO_POS)
#define RF_WLP1_FTMTOATIMESTAMPLO_UMSK                          (~(((1ULL<<RF_WLP1_FTMTOATIMESTAMPLO_LEN)-1)<<RF_WLP1_FTMTOATIMESTAMPLO_POS))

/* 0x410 : wifi_ftm_ctrl_3 */
#define RF_WLP1_WIFI_FTM_CTRL_3_OFFSET                          (0x410)
#define RF_WLP1_FTMTOATIMESTAMPHI                               RF_WLP1_FTMTOATIMESTAMPHI
#define RF_WLP1_FTMTOATIMESTAMPHI_POS                           (0U)
#define RF_WLP1_FTMTOATIMESTAMPHI_LEN                           (32U)
#define RF_WLP1_FTMTOATIMESTAMPHI_MSK                           (((1ULL<<RF_WLP1_FTMTOATIMESTAMPHI_LEN)-1)<<RF_WLP1_FTMTOATIMESTAMPHI_POS)
#define RF_WLP1_FTMTOATIMESTAMPHI_UMSK                          (~(((1ULL<<RF_WLP1_FTMTOATIMESTAMPHI_LEN)-1)<<RF_WLP1_FTMTOATIMESTAMPHI_POS))

/* 0x414 : wifi_ftm_ctrl_4 */
#define RF_WLP1_WIFI_FTM_CTRL_4_OFFSET                          (0x414)
#define RF_WLP1_TOATIMESTAMPLO                                  RF_WLP1_TOATIMESTAMPLO
#define RF_WLP1_TOATIMESTAMPLO_POS                              (0U)
#define RF_WLP1_TOATIMESTAMPLO_LEN                              (9U)
#define RF_WLP1_TOATIMESTAMPLO_MSK                              (((1ULL<<RF_WLP1_TOATIMESTAMPLO_LEN)-1)<<RF_WLP1_TOATIMESTAMPLO_POS)
#define RF_WLP1_TOATIMESTAMPLO_UMSK                             (~(((1ULL<<RF_WLP1_TOATIMESTAMPLO_LEN)-1)<<RF_WLP1_TOATIMESTAMPLO_POS))

/* 0x418 : wifi_ftm_ctrl_5 */
#define RF_WLP1_WIFI_FTM_CTRL_5_OFFSET                          (0x418)
#define RF_WLP1_TOATIMESTAMPHI                                  RF_WLP1_TOATIMESTAMPHI
#define RF_WLP1_TOATIMESTAMPHI_POS                              (0U)
#define RF_WLP1_TOATIMESTAMPHI_LEN                              (32U)
#define RF_WLP1_TOATIMESTAMPHI_MSK                              (((1ULL<<RF_WLP1_TOATIMESTAMPHI_LEN)-1)<<RF_WLP1_TOATIMESTAMPHI_POS)
#define RF_WLP1_TOATIMESTAMPHI_UMSK                             (~(((1ULL<<RF_WLP1_TOATIMESTAMPHI_LEN)-1)<<RF_WLP1_TOATIMESTAMPHI_POS))

/* 0x41C : wlp1_cfg1 */
#define RF_WLP1_WLP1_CFG1_OFFSET                                (0x41C)
#define RF_WLP1_WLP1_RSVD                                       RF_WLP1_WLP1_RSVD
#define RF_WLP1_WLP1_RSVD_POS                                   (0U)
#define RF_WLP1_WLP1_RSVD_LEN                                   (8U)
#define RF_WLP1_WLP1_RSVD_MSK                                   (((1ULL<<RF_WLP1_WLP1_RSVD_LEN)-1)<<RF_WLP1_WLP1_RSVD_POS)
#define RF_WLP1_WLP1_RSVD_UMSK                                  (~(((1ULL<<RF_WLP1_WLP1_RSVD_LEN)-1)<<RF_WLP1_WLP1_RSVD_POS))
#define RF_WLP1_CFG_WIFI_CLK_SEL                                RF_WLP1_CFG_WIFI_CLK_SEL
#define RF_WLP1_CFG_WIFI_CLK_SEL_POS                            (15U)
#define RF_WLP1_CFG_WIFI_CLK_SEL_LEN                            (2U)
#define RF_WLP1_CFG_WIFI_CLK_SEL_MSK                            (((1ULL<<RF_WLP1_CFG_WIFI_CLK_SEL_LEN)-1)<<RF_WLP1_CFG_WIFI_CLK_SEL_POS)
#define RF_WLP1_CFG_WIFI_CLK_SEL_UMSK                           (~(((1ULL<<RF_WLP1_CFG_WIFI_CLK_SEL_LEN)-1)<<RF_WLP1_CFG_WIFI_CLK_SEL_POS))
#define RF_WLP1_CFG_MAC_CLK_SEL                                 RF_WLP1_CFG_MAC_CLK_SEL
#define RF_WLP1_CFG_MAC_CLK_SEL_POS                             (17U)
#define RF_WLP1_CFG_MAC_CLK_SEL_LEN                             (1U)
#define RF_WLP1_CFG_MAC_CLK_SEL_MSK                             (((1ULL<<RF_WLP1_CFG_MAC_CLK_SEL_LEN)-1)<<RF_WLP1_CFG_MAC_CLK_SEL_POS)
#define RF_WLP1_CFG_MAC_CLK_SEL_UMSK                            (~(((1ULL<<RF_WLP1_CFG_MAC_CLK_SEL_LEN)-1)<<RF_WLP1_CFG_MAC_CLK_SEL_POS))
#define RF_WLP1_PLDMA_BMX_HQOS                                  RF_WLP1_PLDMA_BMX_HQOS
#define RF_WLP1_PLDMA_BMX_HQOS_POS                              (30U)
#define RF_WLP1_PLDMA_BMX_HQOS_LEN                              (1U)
#define RF_WLP1_PLDMA_BMX_HQOS_MSK                              (((1ULL<<RF_WLP1_PLDMA_BMX_HQOS_LEN)-1)<<RF_WLP1_PLDMA_BMX_HQOS_POS)
#define RF_WLP1_PLDMA_BMX_HQOS_UMSK                             (~(((1ULL<<RF_WLP1_PLDMA_BMX_HQOS_LEN)-1)<<RF_WLP1_PLDMA_BMX_HQOS_POS))
#define RF_WLP1_PLDMA_X2H_R_SGL                                 RF_WLP1_PLDMA_X2H_R_SGL
#define RF_WLP1_PLDMA_X2H_R_SGL_POS                             (31U)
#define RF_WLP1_PLDMA_X2H_R_SGL_LEN                             (1U)
#define RF_WLP1_PLDMA_X2H_R_SGL_MSK                             (((1ULL<<RF_WLP1_PLDMA_X2H_R_SGL_LEN)-1)<<RF_WLP1_PLDMA_X2H_R_SGL_POS)
#define RF_WLP1_PLDMA_X2H_R_SGL_UMSK                            (~(((1ULL<<RF_WLP1_PLDMA_X2H_R_SGL_LEN)-1)<<RF_WLP1_PLDMA_X2H_R_SGL_POS))

/* 0x480 : wifi_mbist_0 */
#define RF_WLP1_WIFI_MBIST_0_OFFSET                             (0x480)
#define RF_WLP1_WIFI_MBIST_MODE                                 RF_WLP1_WIFI_MBIST_MODE
#define RF_WLP1_WIFI_MBIST_MODE_POS                             (0U)
#define RF_WLP1_WIFI_MBIST_MODE_LEN                             (1U)
#define RF_WLP1_WIFI_MBIST_MODE_MSK                             (((1ULL<<RF_WLP1_WIFI_MBIST_MODE_LEN)-1)<<RF_WLP1_WIFI_MBIST_MODE_POS)
#define RF_WLP1_WIFI_MBIST_MODE_UMSK                            (~(((1ULL<<RF_WLP1_WIFI_MBIST_MODE_LEN)-1)<<RF_WLP1_WIFI_MBIST_MODE_POS))
#define RF_WLP1_REG_WIFI_MBIST_RST_N                            RF_WLP1_REG_WIFI_MBIST_RST_N
#define RF_WLP1_REG_WIFI_MBIST_RST_N_POS                        (1U)
#define RF_WLP1_REG_WIFI_MBIST_RST_N_LEN                        (1U)
#define RF_WLP1_REG_WIFI_MBIST_RST_N_MSK                        (((1ULL<<RF_WLP1_REG_WIFI_MBIST_RST_N_LEN)-1)<<RF_WLP1_REG_WIFI_MBIST_RST_N_POS)
#define RF_WLP1_REG_WIFI_MBIST_RST_N_UMSK                       (~(((1ULL<<RF_WLP1_REG_WIFI_MBIST_RST_N_LEN)-1)<<RF_WLP1_REG_WIFI_MBIST_RST_N_POS))
#define RF_WLP1_WIFI_MBIST_DONE                                 RF_WLP1_WIFI_MBIST_DONE
#define RF_WLP1_WIFI_MBIST_DONE_POS                             (8U)
#define RF_WLP1_WIFI_MBIST_DONE_LEN                             (19U)
#define RF_WLP1_WIFI_MBIST_DONE_MSK                             (((1ULL<<RF_WLP1_WIFI_MBIST_DONE_LEN)-1)<<RF_WLP1_WIFI_MBIST_DONE_POS)
#define RF_WLP1_WIFI_MBIST_DONE_UMSK                            (~(((1ULL<<RF_WLP1_WIFI_MBIST_DONE_LEN)-1)<<RF_WLP1_WIFI_MBIST_DONE_POS))

/* 0x484 : wifi_mbist_1 */
#define RF_WLP1_WIFI_MBIST_1_OFFSET                             (0x484)
#define RF_WLP1_WIFI_MBIST_FAIL                                 RF_WLP1_WIFI_MBIST_FAIL
#define RF_WLP1_WIFI_MBIST_FAIL_POS                             (8U)
#define RF_WLP1_WIFI_MBIST_FAIL_LEN                             (19U)
#define RF_WLP1_WIFI_MBIST_FAIL_MSK                             (((1ULL<<RF_WLP1_WIFI_MBIST_FAIL_LEN)-1)<<RF_WLP1_WIFI_MBIST_FAIL_POS)
#define RF_WLP1_WIFI_MBIST_FAIL_UMSK                            (~(((1ULL<<RF_WLP1_WIFI_MBIST_FAIL_LEN)-1)<<RF_WLP1_WIFI_MBIST_FAIL_POS))


struct  rf_wlp1_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[1024];

    /* 0x400 : wlp1_cfg0 */
    union {
        struct {
            uint32_t wl1_slv_qos                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t m154m_hqos                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t pldma_hqos                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t wifi_csi_hqos                  :  1; /* [    3],        r/w,        0x0 */
            uint32_t arb_mode_wl1_gp                :  1; /* [    4],        r/w,        0x0 */
            uint32_t bmx_gating_dis_scan            :  1; /* [    5],        r/w,        0x1 */
            uint32_t m154_rx_ignore                 :  1; /* [    6],        r/w,        0x0 */
            uint32_t force_ram_clk_en               :  1; /* [    7],        r/w,        0x0 */
            uint32_t cgen_s3_m154                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t cgen_s3_bt_ble                 :  1; /* [    9],        r/w,        0x1 */
            uint32_t cgen_s1_rf_top                 :  1; /* [   10],        r/w,        0x1 */
            uint32_t cgen_m_wifi_phy                :  1; /* [   11],        r/w,        0x1 */
            uint32_t cgen_m_wifi_mac_phy            :  1; /* [   12],        r/w,        0x1 */
            uint32_t cgen_m_wifi_platform           :  1; /* [   13],        r/w,        0x1 */
            uint32_t clk_phy_en                     :  1; /* [   14],        r/w,        0x1 */
            uint32_t m154_zbEn                      :  1; /* [   15],        r/w,        0x1 */
            uint32_t ble_clk_sel                    :  6; /* [21:16],        r/w,       0x10 */
            uint32_t cgen_480m_soc                  :  1; /* [   22],        r/w,        0x1 */
            uint32_t ble_en                         :  1; /* [   23],        r/w,        0x1 */
            uint32_t wifi_mac_core_div              :  4; /* [27:24],        r/w,        0x1 */
            uint32_t wifi_mac_wt_div                :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } wlp1_cfg0;

    /* 0x404 : wifi_ftm_ctrl_0 */
    union {
        struct {
            uint32_t ToDTimestampLo                 :  9; /* [ 8: 0],          r,        0x0 */
            uint32_t reserved_9_23                  : 15; /* [23: 9],       rsvd,        0x0 */
            uint32_t FtmToAClockFreqMHz             :  8; /* [31:24],        r/w,       0x3c */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_0;

    /* 0x408 : wifi_ftm_ctrl_1 */
    union {
        struct {
            uint32_t ToDTimestampHi                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_1;

    /* 0x40C : wifi_ftm_ctrl_2 */
    union {
        struct {
            uint32_t FtmToATimestampLo              :  9; /* [ 8: 0],          r,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_2;

    /* 0x410 : wifi_ftm_ctrl_3 */
    union {
        struct {
            uint32_t FtmToATimestampHi              : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_3;

    /* 0x414 : wifi_ftm_ctrl_4 */
    union {
        struct {
            uint32_t ToATimestampLo                 :  9; /* [ 8: 0],          r,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_4;

    /* 0x418 : wifi_ftm_ctrl_5 */
    union {
        struct {
            uint32_t ToATimestampHi                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_ftm_ctrl_5;

    /* 0x41C : wlp1_cfg1 */
    union {
        struct {
            uint32_t wlp1_rsvd                      :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_14                  :  7; /* [14: 8],       rsvd,        0x0 */
            uint32_t cfg_wifi_clk_sel               :  2; /* [16:15],        r/w,        0x0 */
            uint32_t cfg_mac_clk_sel                :  1; /* [   17],        r/w,        0x0 */
            uint32_t reserved_18_29                 : 12; /* [29:18],       rsvd,        0x0 */
            uint32_t pldma_bmx_hqos                 :  1; /* [   30],        r/w,        0x0 */
            uint32_t pldma_x2h_r_sgl                :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } wlp1_cfg1;

    /* 0x420  reserved */
    uint8_t RESERVED0x420[96];

    /* 0x480 : wifi_mbist_0 */
    union {
        struct {
            uint32_t wifi_mbist_mode                :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_wifi_mbist_rst_n           :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t wifi_mbist_done                : 19; /* [26: 8],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_mbist_0;

    /* 0x484 : wifi_mbist_1 */
    union {
        struct {
            uint32_t reserved_0_7                   :  8; /* [ 7: 0],       rsvd,        0x0 */
            uint32_t wifi_mbist_fail                : 19; /* [26: 8],          r,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } wifi_mbist_1;

};

typedef volatile struct rf_wlp1_reg rf_wlp1_reg_t;


#endif  /* __RF_WLP1_REG_H__ */
