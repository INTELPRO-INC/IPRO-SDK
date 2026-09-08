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

#ifndef  __EF_DATA_0_REG_H__
#define  __EF_DATA_0_REG_H__

#include "ipro7.h"

/* 0x0 : ef_cfg_0 */
#define EF_DATA_0_EF_CFG_0_OFFSET                               (0x0)
#define EF_DATA_0_EF_SF_AES_MODE                                EF_DATA_0_EF_SF_AES_MODE
#define EF_DATA_0_EF_SF_AES_MODE_POS                            (0U)
#define EF_DATA_0_EF_SF_AES_MODE_LEN                            (2U)
#define EF_DATA_0_EF_SF_AES_MODE_MSK                            (((1U<<EF_DATA_0_EF_SF_AES_MODE_LEN)-1)<<EF_DATA_0_EF_SF_AES_MODE_POS)
#define EF_DATA_0_EF_SF_AES_MODE_UMSK                           (~(((1U<<EF_DATA_0_EF_SF_AES_MODE_LEN)-1)<<EF_DATA_0_EF_SF_AES_MODE_POS))
#define EF_DATA_0_EF_SBOOT_SIGN_MODE                            EF_DATA_0_EF_SBOOT_SIGN_MODE
#define EF_DATA_0_EF_SBOOT_SIGN_MODE_POS                        (2U)
#define EF_DATA_0_EF_SBOOT_SIGN_MODE_LEN                        (2U)
#define EF_DATA_0_EF_SBOOT_SIGN_MODE_MSK                        (((1U<<EF_DATA_0_EF_SBOOT_SIGN_MODE_LEN)-1)<<EF_DATA_0_EF_SBOOT_SIGN_MODE_POS)
#define EF_DATA_0_EF_SBOOT_SIGN_MODE_UMSK                       (~(((1U<<EF_DATA_0_EF_SBOOT_SIGN_MODE_LEN)-1)<<EF_DATA_0_EF_SBOOT_SIGN_MODE_POS))
#define EF_DATA_0_EF_SBOOT_EN                                   EF_DATA_0_EF_SBOOT_EN
#define EF_DATA_0_EF_SBOOT_EN_POS                               (4U)
#define EF_DATA_0_EF_SBOOT_EN_LEN                               (2U)
#define EF_DATA_0_EF_SBOOT_EN_MSK                               (((1U<<EF_DATA_0_EF_SBOOT_EN_LEN)-1)<<EF_DATA_0_EF_SBOOT_EN_POS)
#define EF_DATA_0_EF_SBOOT_EN_UMSK                              (~(((1U<<EF_DATA_0_EF_SBOOT_EN_LEN)-1)<<EF_DATA_0_EF_SBOOT_EN_POS))
#define EF_DATA_0_EF_CPU1_ENC_EN                                EF_DATA_0_EF_CPU1_ENC_EN
#define EF_DATA_0_EF_CPU1_ENC_EN_POS                            (6U)
#define EF_DATA_0_EF_CPU1_ENC_EN_LEN                            (1U)
#define EF_DATA_0_EF_CPU1_ENC_EN_MSK                            (((1U<<EF_DATA_0_EF_CPU1_ENC_EN_LEN)-1)<<EF_DATA_0_EF_CPU1_ENC_EN_POS)
#define EF_DATA_0_EF_CPU1_ENC_EN_UMSK                           (~(((1U<<EF_DATA_0_EF_CPU1_ENC_EN_LEN)-1)<<EF_DATA_0_EF_CPU1_ENC_EN_POS))
#define EF_DATA_0_EF_CPU0_ENC_EN                                EF_DATA_0_EF_CPU0_ENC_EN
#define EF_DATA_0_EF_CPU0_ENC_EN_POS                            (7U)
#define EF_DATA_0_EF_CPU0_ENC_EN_LEN                            (1U)
#define EF_DATA_0_EF_CPU0_ENC_EN_MSK                            (((1U<<EF_DATA_0_EF_CPU0_ENC_EN_LEN)-1)<<EF_DATA_0_EF_CPU0_ENC_EN_POS)
#define EF_DATA_0_EF_CPU0_ENC_EN_UMSK                           (~(((1U<<EF_DATA_0_EF_CPU0_ENC_EN_LEN)-1)<<EF_DATA_0_EF_CPU0_ENC_EN_POS))
#define EF_DATA_0_EF_BOOT_SEL                                   EF_DATA_0_EF_BOOT_SEL
#define EF_DATA_0_EF_BOOT_SEL_POS                               (8U)
#define EF_DATA_0_EF_BOOT_SEL_LEN                               (4U)
#define EF_DATA_0_EF_BOOT_SEL_MSK                               (((1U<<EF_DATA_0_EF_BOOT_SEL_LEN)-1)<<EF_DATA_0_EF_BOOT_SEL_POS)
#define EF_DATA_0_EF_BOOT_SEL_UMSK                              (~(((1U<<EF_DATA_0_EF_BOOT_SEL_LEN)-1)<<EF_DATA_0_EF_BOOT_SEL_POS))
#define EF_DATA_0_EF_SF_KEY_0_SEL                               EF_DATA_0_EF_SF_KEY_0_SEL
#define EF_DATA_0_EF_SF_KEY_0_SEL_POS                           (12U)
#define EF_DATA_0_EF_SF_KEY_0_SEL_LEN                           (2U)
#define EF_DATA_0_EF_SF_KEY_0_SEL_MSK                           (((1U<<EF_DATA_0_EF_SF_KEY_0_SEL_LEN)-1)<<EF_DATA_0_EF_SF_KEY_0_SEL_POS)
#define EF_DATA_0_EF_SF_KEY_0_SEL_UMSK                          (~(((1U<<EF_DATA_0_EF_SF_KEY_0_SEL_LEN)-1)<<EF_DATA_0_EF_SF_KEY_0_SEL_POS))
#define EF_DATA_0_EF_SDU_DIS                                    EF_DATA_0_EF_SDU_DIS
#define EF_DATA_0_EF_SDU_DIS_POS                                (14U)
#define EF_DATA_0_EF_SDU_DIS_LEN                                (1U)
#define EF_DATA_0_EF_SDU_DIS_MSK                                (((1U<<EF_DATA_0_EF_SDU_DIS_LEN)-1)<<EF_DATA_0_EF_SDU_DIS_POS)
#define EF_DATA_0_EF_SDU_DIS_UMSK                               (~(((1U<<EF_DATA_0_EF_SDU_DIS_LEN)-1)<<EF_DATA_0_EF_SDU_DIS_POS))
#define EF_DATA_0_EF_BLE_DIS                                    EF_DATA_0_EF_BLE_DIS
#define EF_DATA_0_EF_BLE_DIS_POS                                (15U)
#define EF_DATA_0_EF_BLE_DIS_LEN                                (1U)
#define EF_DATA_0_EF_BLE_DIS_MSK                                (((1U<<EF_DATA_0_EF_BLE_DIS_LEN)-1)<<EF_DATA_0_EF_BLE_DIS_POS)
#define EF_DATA_0_EF_BLE_DIS_UMSK                               (~(((1U<<EF_DATA_0_EF_BLE_DIS_LEN)-1)<<EF_DATA_0_EF_BLE_DIS_POS))
#define EF_DATA_0_EF_WIFI_DIS                                   EF_DATA_0_EF_WIFI_DIS
#define EF_DATA_0_EF_WIFI_DIS_POS                               (16U)
#define EF_DATA_0_EF_WIFI_DIS_LEN                               (1U)
#define EF_DATA_0_EF_WIFI_DIS_MSK                               (((1U<<EF_DATA_0_EF_WIFI_DIS_LEN)-1)<<EF_DATA_0_EF_WIFI_DIS_POS)
#define EF_DATA_0_EF_WIFI_DIS_UMSK                              (~(((1U<<EF_DATA_0_EF_WIFI_DIS_LEN)-1)<<EF_DATA_0_EF_WIFI_DIS_POS))
#define EF_DATA_0_EF_0_KEY_ENC_EN                               EF_DATA_0_EF_0_KEY_ENC_EN
#define EF_DATA_0_EF_0_KEY_ENC_EN_POS                           (17U)
#define EF_DATA_0_EF_0_KEY_ENC_EN_LEN                           (1U)
#define EF_DATA_0_EF_0_KEY_ENC_EN_MSK                           (((1U<<EF_DATA_0_EF_0_KEY_ENC_EN_LEN)-1)<<EF_DATA_0_EF_0_KEY_ENC_EN_POS)
#define EF_DATA_0_EF_0_KEY_ENC_EN_UMSK                          (~(((1U<<EF_DATA_0_EF_0_KEY_ENC_EN_LEN)-1)<<EF_DATA_0_EF_0_KEY_ENC_EN_POS))
#define EF_DATA_0_EF_CAM_DIS                                    EF_DATA_0_EF_CAM_DIS
#define EF_DATA_0_EF_CAM_DIS_POS                                (18U)
#define EF_DATA_0_EF_CAM_DIS_LEN                                (1U)
#define EF_DATA_0_EF_CAM_DIS_MSK                                (((1U<<EF_DATA_0_EF_CAM_DIS_LEN)-1)<<EF_DATA_0_EF_CAM_DIS_POS)
#define EF_DATA_0_EF_CAM_DIS_UMSK                               (~(((1U<<EF_DATA_0_EF_CAM_DIS_LEN)-1)<<EF_DATA_0_EF_CAM_DIS_POS))
#define EF_DATA_0_EF_M154_DIS                                   EF_DATA_0_EF_M154_DIS
#define EF_DATA_0_EF_M154_DIS_POS                               (19U)
#define EF_DATA_0_EF_M154_DIS_LEN                               (1U)
#define EF_DATA_0_EF_M154_DIS_MSK                               (((1U<<EF_DATA_0_EF_M154_DIS_LEN)-1)<<EF_DATA_0_EF_M154_DIS_POS)
#define EF_DATA_0_EF_M154_DIS_UMSK                              (~(((1U<<EF_DATA_0_EF_M154_DIS_LEN)-1)<<EF_DATA_0_EF_M154_DIS_POS))
#define EF_DATA_0_EF_CPU1_DIS                                   EF_DATA_0_EF_CPU1_DIS
#define EF_DATA_0_EF_CPU1_DIS_POS                               (20U)
#define EF_DATA_0_EF_CPU1_DIS_LEN                               (1U)
#define EF_DATA_0_EF_CPU1_DIS_MSK                               (((1U<<EF_DATA_0_EF_CPU1_DIS_LEN)-1)<<EF_DATA_0_EF_CPU1_DIS_POS)
#define EF_DATA_0_EF_CPU1_DIS_UMSK                              (~(((1U<<EF_DATA_0_EF_CPU1_DIS_LEN)-1)<<EF_DATA_0_EF_CPU1_DIS_POS))
#define EF_DATA_0_EF_CPU_RST_DBG_DIS                            EF_DATA_0_EF_CPU_RST_DBG_DIS
#define EF_DATA_0_EF_CPU_RST_DBG_DIS_POS                        (21U)
#define EF_DATA_0_EF_CPU_RST_DBG_DIS_LEN                        (1U)
#define EF_DATA_0_EF_CPU_RST_DBG_DIS_MSK                        (((1U<<EF_DATA_0_EF_CPU_RST_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_CPU_RST_DBG_DIS_POS)
#define EF_DATA_0_EF_CPU_RST_DBG_DIS_UMSK                       (~(((1U<<EF_DATA_0_EF_CPU_RST_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_CPU_RST_DBG_DIS_POS))
#define EF_DATA_0_EF_SE_DBG_DIS                                 EF_DATA_0_EF_SE_DBG_DIS
#define EF_DATA_0_EF_SE_DBG_DIS_POS                             (22U)
#define EF_DATA_0_EF_SE_DBG_DIS_LEN                             (1U)
#define EF_DATA_0_EF_SE_DBG_DIS_MSK                             (((1U<<EF_DATA_0_EF_SE_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_SE_DBG_DIS_POS)
#define EF_DATA_0_EF_SE_DBG_DIS_UMSK                            (~(((1U<<EF_DATA_0_EF_SE_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_SE_DBG_DIS_POS))
#define EF_DATA_0_EF_EFUSE_DBG_DIS                              EF_DATA_0_EF_EFUSE_DBG_DIS
#define EF_DATA_0_EF_EFUSE_DBG_DIS_POS                          (23U)
#define EF_DATA_0_EF_EFUSE_DBG_DIS_LEN                          (1U)
#define EF_DATA_0_EF_EFUSE_DBG_DIS_MSK                          (((1U<<EF_DATA_0_EF_EFUSE_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_EFUSE_DBG_DIS_POS)
#define EF_DATA_0_EF_EFUSE_DBG_DIS_UMSK                         (~(((1U<<EF_DATA_0_EF_EFUSE_DBG_DIS_LEN)-1)<<EF_DATA_0_EF_EFUSE_DBG_DIS_POS))
#define EF_DATA_0_EF_DBG_JTAG_1_DIS                             EF_DATA_0_EF_DBG_JTAG_1_DIS
#define EF_DATA_0_EF_DBG_JTAG_1_DIS_POS                         (24U)
#define EF_DATA_0_EF_DBG_JTAG_1_DIS_LEN                         (2U)
#define EF_DATA_0_EF_DBG_JTAG_1_DIS_MSK                         (((1U<<EF_DATA_0_EF_DBG_JTAG_1_DIS_LEN)-1)<<EF_DATA_0_EF_DBG_JTAG_1_DIS_POS)
#define EF_DATA_0_EF_DBG_JTAG_1_DIS_UMSK                        (~(((1U<<EF_DATA_0_EF_DBG_JTAG_1_DIS_LEN)-1)<<EF_DATA_0_EF_DBG_JTAG_1_DIS_POS))
#define EF_DATA_0_EF_DBG_JTAG_0_DIS                             EF_DATA_0_EF_DBG_JTAG_0_DIS
#define EF_DATA_0_EF_DBG_JTAG_0_DIS_POS                         (26U)
#define EF_DATA_0_EF_DBG_JTAG_0_DIS_LEN                         (2U)
#define EF_DATA_0_EF_DBG_JTAG_0_DIS_MSK                         (((1U<<EF_DATA_0_EF_DBG_JTAG_0_DIS_LEN)-1)<<EF_DATA_0_EF_DBG_JTAG_0_DIS_POS)
#define EF_DATA_0_EF_DBG_JTAG_0_DIS_UMSK                        (~(((1U<<EF_DATA_0_EF_DBG_JTAG_0_DIS_LEN)-1)<<EF_DATA_0_EF_DBG_JTAG_0_DIS_POS))
#define EF_DATA_0_EF_DBG_MODE                                   EF_DATA_0_EF_DBG_MODE
#define EF_DATA_0_EF_DBG_MODE_POS                               (28U)
#define EF_DATA_0_EF_DBG_MODE_LEN                               (4U)
#define EF_DATA_0_EF_DBG_MODE_MSK                               (((1U<<EF_DATA_0_EF_DBG_MODE_LEN)-1)<<EF_DATA_0_EF_DBG_MODE_POS)
#define EF_DATA_0_EF_DBG_MODE_UMSK                              (~(((1U<<EF_DATA_0_EF_DBG_MODE_LEN)-1)<<EF_DATA_0_EF_DBG_MODE_POS))

/* 0x4 : ef_sw_usage_0 */
#define EF_DATA_0_EF_SW_USAGE_0_OFFSET                          (0x4)
#define EF_DATA_0_EF_SW_USAGE_0                                 EF_DATA_0_EF_SW_USAGE_0
#define EF_DATA_0_EF_SW_USAGE_0_POS                             (0U)
#define EF_DATA_0_EF_SW_USAGE_0_LEN                             (32U)
#define EF_DATA_0_EF_SW_USAGE_0_MSK                             (((1U<<EF_DATA_0_EF_SW_USAGE_0_LEN)-1)<<EF_DATA_0_EF_SW_USAGE_0_POS)
#define EF_DATA_0_EF_SW_USAGE_0_UMSK                            (~(((1U<<EF_DATA_0_EF_SW_USAGE_0_LEN)-1)<<EF_DATA_0_EF_SW_USAGE_0_POS))

/* 0x8 : ef_sw_usage_1 */
#define EF_DATA_0_EF_SW_USAGE_1_OFFSET                          (0x8)
#define EF_DATA_0_EF_SW_USAGE_1                                 EF_DATA_0_EF_SW_USAGE_1
#define EF_DATA_0_EF_SW_USAGE_1_POS                             (0U)
#define EF_DATA_0_EF_SW_USAGE_1_LEN                             (32U)
#define EF_DATA_0_EF_SW_USAGE_1_MSK                             (((1U<<EF_DATA_0_EF_SW_USAGE_1_LEN)-1)<<EF_DATA_0_EF_SW_USAGE_1_POS)
#define EF_DATA_0_EF_SW_USAGE_1_UMSK                            (~(((1U<<EF_DATA_0_EF_SW_USAGE_1_LEN)-1)<<EF_DATA_0_EF_SW_USAGE_1_POS))

/* 0xC : ef_ana_trim_0 */
#define EF_DATA_0_EF_ANA_TRIM_0_OFFSET                          (0xC)
#define EF_DATA_0_EF_ANA_TRIM_0                                 EF_DATA_0_EF_ANA_TRIM_0
#define EF_DATA_0_EF_ANA_TRIM_0_POS                             (0U)
#define EF_DATA_0_EF_ANA_TRIM_0_LEN                             (32U)
#define EF_DATA_0_EF_ANA_TRIM_0_MSK                             (((1U<<EF_DATA_0_EF_ANA_TRIM_0_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_0_POS)
#define EF_DATA_0_EF_ANA_TRIM_0_UMSK                            (~(((1U<<EF_DATA_0_EF_ANA_TRIM_0_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_0_POS))

/* 0x10 : ef_ana_trim_1 */
#define EF_DATA_0_EF_ANA_TRIM_1_OFFSET                          (0x10)
#define EF_DATA_0_EF_ANA_TRIM_1                                 EF_DATA_0_EF_ANA_TRIM_1
#define EF_DATA_0_EF_ANA_TRIM_1_POS                             (0U)
#define EF_DATA_0_EF_ANA_TRIM_1_LEN                             (32U)
#define EF_DATA_0_EF_ANA_TRIM_1_MSK                             (((1U<<EF_DATA_0_EF_ANA_TRIM_1_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_1_POS)
#define EF_DATA_0_EF_ANA_TRIM_1_UMSK                            (~(((1U<<EF_DATA_0_EF_ANA_TRIM_1_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_1_POS))

/* 0x14 : ef_wifi_mac_low */
#define EF_DATA_0_EF_WIFI_MAC_LOW_OFFSET                        (0x14)
#define EF_DATA_0_EF_WIFI_MAC_LOW                               EF_DATA_0_EF_WIFI_MAC_LOW
#define EF_DATA_0_EF_WIFI_MAC_LOW_POS                           (0U)
#define EF_DATA_0_EF_WIFI_MAC_LOW_LEN                           (32U)
#define EF_DATA_0_EF_WIFI_MAC_LOW_MSK                           (((1U<<EF_DATA_0_EF_WIFI_MAC_LOW_LEN)-1)<<EF_DATA_0_EF_WIFI_MAC_LOW_POS)
#define EF_DATA_0_EF_WIFI_MAC_LOW_UMSK                          (~(((1U<<EF_DATA_0_EF_WIFI_MAC_LOW_LEN)-1)<<EF_DATA_0_EF_WIFI_MAC_LOW_POS))

/* 0x18 : ef_wifi_mac_high */
#define EF_DATA_0_EF_WIFI_MAC_HIGH_OFFSET                       (0x18)
#define EF_DATA_0_EF_WIFI_MAC_HIGH                              EF_DATA_0_EF_WIFI_MAC_HIGH
#define EF_DATA_0_EF_WIFI_MAC_HIGH_POS                          (0U)
#define EF_DATA_0_EF_WIFI_MAC_HIGH_LEN                          (32U)
#define EF_DATA_0_EF_WIFI_MAC_HIGH_MSK                          (((1U<<EF_DATA_0_EF_WIFI_MAC_HIGH_LEN)-1)<<EF_DATA_0_EF_WIFI_MAC_HIGH_POS)
#define EF_DATA_0_EF_WIFI_MAC_HIGH_UMSK                         (~(((1U<<EF_DATA_0_EF_WIFI_MAC_HIGH_LEN)-1)<<EF_DATA_0_EF_WIFI_MAC_HIGH_POS))

/* 0x1C : ef_key_slot_0_w0 */
#define EF_DATA_0_EF_KEY_SLOT_0_W0_OFFSET                       (0x1C)
#define EF_DATA_0_EF_KEY_SLOT_0_W0                              EF_DATA_0_EF_KEY_SLOT_0_W0
#define EF_DATA_0_EF_KEY_SLOT_0_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_0_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_0_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_0_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_0_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_0_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W0_POS))

/* 0x20 : ef_key_slot_0_w1 */
#define EF_DATA_0_EF_KEY_SLOT_0_W1_OFFSET                       (0x20)
#define EF_DATA_0_EF_KEY_SLOT_0_W1                              EF_DATA_0_EF_KEY_SLOT_0_W1
#define EF_DATA_0_EF_KEY_SLOT_0_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_0_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_0_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_0_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_0_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_0_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W1_POS))

/* 0x24 : ef_key_slot_0_w2 */
#define EF_DATA_0_EF_KEY_SLOT_0_W2_OFFSET                       (0x24)
#define EF_DATA_0_EF_KEY_SLOT_0_W2                              EF_DATA_0_EF_KEY_SLOT_0_W2
#define EF_DATA_0_EF_KEY_SLOT_0_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_0_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_0_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_0_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_0_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_0_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W2_POS))

/* 0x28 : ef_key_slot_0_w3 */
#define EF_DATA_0_EF_KEY_SLOT_0_W3_OFFSET                       (0x28)
#define EF_DATA_0_EF_KEY_SLOT_0_W3                              EF_DATA_0_EF_KEY_SLOT_0_W3
#define EF_DATA_0_EF_KEY_SLOT_0_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_0_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_0_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_0_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_0_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_0_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_0_W3_POS))

/* 0x2C : ef_key_slot_1_w0 */
#define EF_DATA_0_EF_KEY_SLOT_1_W0_OFFSET                       (0x2C)
#define EF_DATA_0_EF_KEY_SLOT_1_W0                              EF_DATA_0_EF_KEY_SLOT_1_W0
#define EF_DATA_0_EF_KEY_SLOT_1_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_1_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_1_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_1_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_1_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_1_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W0_POS))

/* 0x30 : ef_key_slot_1_w1 */
#define EF_DATA_0_EF_KEY_SLOT_1_W1_OFFSET                       (0x30)
#define EF_DATA_0_EF_KEY_SLOT_1_W1                              EF_DATA_0_EF_KEY_SLOT_1_W1
#define EF_DATA_0_EF_KEY_SLOT_1_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_1_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_1_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_1_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_1_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_1_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W1_POS))

/* 0x34 : ef_key_slot_1_w2 */
#define EF_DATA_0_EF_KEY_SLOT_1_W2_OFFSET                       (0x34)
#define EF_DATA_0_EF_KEY_SLOT_1_W2                              EF_DATA_0_EF_KEY_SLOT_1_W2
#define EF_DATA_0_EF_KEY_SLOT_1_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_1_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_1_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_1_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_1_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_1_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W2_POS))

/* 0x38 : ef_key_slot_1_w3 */
#define EF_DATA_0_EF_KEY_SLOT_1_W3_OFFSET                       (0x38)
#define EF_DATA_0_EF_KEY_SLOT_1_W3                              EF_DATA_0_EF_KEY_SLOT_1_W3
#define EF_DATA_0_EF_KEY_SLOT_1_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_1_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_1_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_1_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_1_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_1_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_1_W3_POS))

/* 0x3C : ef_key_slot_2_w0 */
#define EF_DATA_0_EF_KEY_SLOT_2_W0_OFFSET                       (0x3C)
#define EF_DATA_0_EF_KEY_SLOT_2_W0                              EF_DATA_0_EF_KEY_SLOT_2_W0
#define EF_DATA_0_EF_KEY_SLOT_2_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_2_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_2_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_2_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_2_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_2_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W0_POS))

/* 0x40 : ef_key_slot_2_w1 */
#define EF_DATA_0_EF_KEY_SLOT_2_W1_OFFSET                       (0x40)
#define EF_DATA_0_EF_KEY_SLOT_2_W1                              EF_DATA_0_EF_KEY_SLOT_2_W1
#define EF_DATA_0_EF_KEY_SLOT_2_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_2_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_2_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_2_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_2_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_2_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W1_POS))

/* 0x44 : ef_key_slot_2_w2 */
#define EF_DATA_0_EF_KEY_SLOT_2_W2_OFFSET                       (0x44)
#define EF_DATA_0_EF_KEY_SLOT_2_W2                              EF_DATA_0_EF_KEY_SLOT_2_W2
#define EF_DATA_0_EF_KEY_SLOT_2_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_2_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_2_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_2_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_2_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_2_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W2_POS))

/* 0x48 : ef_key_slot_2_w3 */
#define EF_DATA_0_EF_KEY_SLOT_2_W3_OFFSET                       (0x48)
#define EF_DATA_0_EF_KEY_SLOT_2_W3                              EF_DATA_0_EF_KEY_SLOT_2_W3
#define EF_DATA_0_EF_KEY_SLOT_2_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_2_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_2_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_2_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_2_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_2_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_2_W3_POS))

/* 0x4C : ef_key_slot_3_w0 */
#define EF_DATA_0_EF_KEY_SLOT_3_W0_OFFSET                       (0x4C)
#define EF_DATA_0_EF_KEY_SLOT_3_W0                              EF_DATA_0_EF_KEY_SLOT_3_W0
#define EF_DATA_0_EF_KEY_SLOT_3_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_3_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_3_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_3_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_3_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_3_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W0_POS))

/* 0x50 : ef_key_slot_3_w1 */
#define EF_DATA_0_EF_KEY_SLOT_3_W1_OFFSET                       (0x50)
#define EF_DATA_0_EF_KEY_SLOT_3_W1                              EF_DATA_0_EF_KEY_SLOT_3_W1
#define EF_DATA_0_EF_KEY_SLOT_3_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_3_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_3_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_3_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_3_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_3_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W1_POS))

/* 0x54 : ef_key_slot_3_w2 */
#define EF_DATA_0_EF_KEY_SLOT_3_W2_OFFSET                       (0x54)
#define EF_DATA_0_EF_KEY_SLOT_3_W2                              EF_DATA_0_EF_KEY_SLOT_3_W2
#define EF_DATA_0_EF_KEY_SLOT_3_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_3_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_3_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_3_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_3_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_3_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W2_POS))

/* 0x58 : ef_key_slot_3_w3 */
#define EF_DATA_0_EF_KEY_SLOT_3_W3_OFFSET                       (0x58)
#define EF_DATA_0_EF_KEY_SLOT_3_W3                              EF_DATA_0_EF_KEY_SLOT_3_W3
#define EF_DATA_0_EF_KEY_SLOT_3_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_3_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_3_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_3_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_3_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_3_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_3_W3_POS))

/* 0x5C : ef_key_slot_4_w0 */
#define EF_DATA_0_EF_KEY_SLOT_4_W0_OFFSET                       (0x5C)
#define EF_DATA_0_EF_KEY_SLOT_4_W0                              EF_DATA_0_EF_KEY_SLOT_4_W0
#define EF_DATA_0_EF_KEY_SLOT_4_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_4_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_4_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_4_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_4_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_4_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W0_POS))

/* 0x60 : ef_key_slot_4_w1 */
#define EF_DATA_0_EF_KEY_SLOT_4_W1_OFFSET                       (0x60)
#define EF_DATA_0_EF_KEY_SLOT_4_W1                              EF_DATA_0_EF_KEY_SLOT_4_W1
#define EF_DATA_0_EF_KEY_SLOT_4_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_4_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_4_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_4_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_4_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_4_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W1_POS))

/* 0x64 : ef_key_slot_4_w2 */
#define EF_DATA_0_EF_KEY_SLOT_4_W2_OFFSET                       (0x64)
#define EF_DATA_0_EF_KEY_SLOT_4_W2                              EF_DATA_0_EF_KEY_SLOT_4_W2
#define EF_DATA_0_EF_KEY_SLOT_4_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_4_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_4_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_4_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_4_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_4_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W2_POS))

/* 0x68 : ef_key_slot_4_w3 */
#define EF_DATA_0_EF_KEY_SLOT_4_W3_OFFSET                       (0x68)
#define EF_DATA_0_EF_KEY_SLOT_4_W3                              EF_DATA_0_EF_KEY_SLOT_4_W3
#define EF_DATA_0_EF_KEY_SLOT_4_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_4_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_4_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_4_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_4_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_4_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_4_W3_POS))

/* 0x6C : ef_key_slot_5_w0 */
#define EF_DATA_0_EF_KEY_SLOT_5_W0_OFFSET                       (0x6C)
#define EF_DATA_0_EF_KEY_SLOT_5_W0                              EF_DATA_0_EF_KEY_SLOT_5_W0
#define EF_DATA_0_EF_KEY_SLOT_5_W0_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_5_W0_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_5_W0_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_5_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W0_POS)
#define EF_DATA_0_EF_KEY_SLOT_5_W0_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_5_W0_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W0_POS))

/* 0x70 : ef_key_slot_5_w1 */
#define EF_DATA_0_EF_KEY_SLOT_5_W1_OFFSET                       (0x70)
#define EF_DATA_0_EF_KEY_SLOT_5_W1                              EF_DATA_0_EF_KEY_SLOT_5_W1
#define EF_DATA_0_EF_KEY_SLOT_5_W1_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_5_W1_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_5_W1_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_5_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W1_POS)
#define EF_DATA_0_EF_KEY_SLOT_5_W1_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_5_W1_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W1_POS))

/* 0x74 : ef_key_slot_5_w2 */
#define EF_DATA_0_EF_KEY_SLOT_5_W2_OFFSET                       (0x74)
#define EF_DATA_0_EF_KEY_SLOT_5_W2                              EF_DATA_0_EF_KEY_SLOT_5_W2
#define EF_DATA_0_EF_KEY_SLOT_5_W2_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_5_W2_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_5_W2_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_5_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W2_POS)
#define EF_DATA_0_EF_KEY_SLOT_5_W2_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_5_W2_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W2_POS))

/* 0x78 : ef_key_slot_5_w3 */
#define EF_DATA_0_EF_KEY_SLOT_5_W3_OFFSET                       (0x78)
#define EF_DATA_0_EF_KEY_SLOT_5_W3                              EF_DATA_0_EF_KEY_SLOT_5_W3
#define EF_DATA_0_EF_KEY_SLOT_5_W3_POS                          (0U)
#define EF_DATA_0_EF_KEY_SLOT_5_W3_LEN                          (32U)
#define EF_DATA_0_EF_KEY_SLOT_5_W3_MSK                          (((1U<<EF_DATA_0_EF_KEY_SLOT_5_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W3_POS)
#define EF_DATA_0_EF_KEY_SLOT_5_W3_UMSK                         (~(((1U<<EF_DATA_0_EF_KEY_SLOT_5_W3_LEN)-1)<<EF_DATA_0_EF_KEY_SLOT_5_W3_POS))

/* 0x7C : ef_data_0_lock */
#define EF_DATA_0_LOCK_OFFSET                                   (0x7C)
#define EF_DATA_0_EF_SEC_LIFECYCLE                              EF_DATA_0_EF_SEC_LIFECYCLE
#define EF_DATA_0_EF_SEC_LIFECYCLE_POS                          (0U)
#define EF_DATA_0_EF_SEC_LIFECYCLE_LEN                          (4U)
#define EF_DATA_0_EF_SEC_LIFECYCLE_MSK                          (((1U<<EF_DATA_0_EF_SEC_LIFECYCLE_LEN)-1)<<EF_DATA_0_EF_SEC_LIFECYCLE_POS)
#define EF_DATA_0_EF_SEC_LIFECYCLE_UMSK                         (~(((1U<<EF_DATA_0_EF_SEC_LIFECYCLE_LEN)-1)<<EF_DATA_0_EF_SEC_LIFECYCLE_POS))
#define EF_DATA_0_EF_ANA_TRIM_2                                 EF_DATA_0_EF_ANA_TRIM_2
#define EF_DATA_0_EF_ANA_TRIM_2_POS                             (4U)
#define EF_DATA_0_EF_ANA_TRIM_2_LEN                             (9U)
#define EF_DATA_0_EF_ANA_TRIM_2_MSK                             (((1U<<EF_DATA_0_EF_ANA_TRIM_2_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_2_POS)
#define EF_DATA_0_EF_ANA_TRIM_2_UMSK                            (~(((1U<<EF_DATA_0_EF_ANA_TRIM_2_LEN)-1)<<EF_DATA_0_EF_ANA_TRIM_2_POS))
#define EF_DATA_0_WR_LOCK_TRIM_1                                EF_DATA_0_WR_LOCK_TRIM_1
#define EF_DATA_0_WR_LOCK_TRIM_1_POS                            (13U)
#define EF_DATA_0_WR_LOCK_TRIM_1_LEN                            (1U)
#define EF_DATA_0_WR_LOCK_TRIM_1_MSK                            (((1U<<EF_DATA_0_WR_LOCK_TRIM_1_LEN)-1)<<EF_DATA_0_WR_LOCK_TRIM_1_POS)
#define EF_DATA_0_WR_LOCK_TRIM_1_UMSK                           (~(((1U<<EF_DATA_0_WR_LOCK_TRIM_1_LEN)-1)<<EF_DATA_0_WR_LOCK_TRIM_1_POS))
#define EF_DATA_0_WR_LOCK_TRIM_0                                EF_DATA_0_WR_LOCK_TRIM_0
#define EF_DATA_0_WR_LOCK_TRIM_0_POS                            (14U)
#define EF_DATA_0_WR_LOCK_TRIM_0_LEN                            (1U)
#define EF_DATA_0_WR_LOCK_TRIM_0_MSK                            (((1U<<EF_DATA_0_WR_LOCK_TRIM_0_LEN)-1)<<EF_DATA_0_WR_LOCK_TRIM_0_POS)
#define EF_DATA_0_WR_LOCK_TRIM_0_UMSK                           (~(((1U<<EF_DATA_0_WR_LOCK_TRIM_0_LEN)-1)<<EF_DATA_0_WR_LOCK_TRIM_0_POS))
#define EF_DATA_0_WR_LOCK_BOOT_MODE                             EF_DATA_0_WR_LOCK_BOOT_MODE
#define EF_DATA_0_WR_LOCK_BOOT_MODE_POS                         (15U)
#define EF_DATA_0_WR_LOCK_BOOT_MODE_LEN                         (1U)
#define EF_DATA_0_WR_LOCK_BOOT_MODE_MSK                         (((1U<<EF_DATA_0_WR_LOCK_BOOT_MODE_LEN)-1)<<EF_DATA_0_WR_LOCK_BOOT_MODE_POS)
#define EF_DATA_0_WR_LOCK_BOOT_MODE_UMSK                        (~(((1U<<EF_DATA_0_WR_LOCK_BOOT_MODE_LEN)-1)<<EF_DATA_0_WR_LOCK_BOOT_MODE_POS))
#define EF_DATA_0_WR_LOCK_SW_USAGE_1                            EF_DATA_0_WR_LOCK_SW_USAGE_1
#define EF_DATA_0_WR_LOCK_SW_USAGE_1_POS                        (16U)
#define EF_DATA_0_WR_LOCK_SW_USAGE_1_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_SW_USAGE_1_MSK                        (((1U<<EF_DATA_0_WR_LOCK_SW_USAGE_1_LEN)-1)<<EF_DATA_0_WR_LOCK_SW_USAGE_1_POS)
#define EF_DATA_0_WR_LOCK_SW_USAGE_1_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_SW_USAGE_1_LEN)-1)<<EF_DATA_0_WR_LOCK_SW_USAGE_1_POS))
#define EF_DATA_0_WR_LOCK_SW_USAGE_0                            EF_DATA_0_WR_LOCK_SW_USAGE_0
#define EF_DATA_0_WR_LOCK_SW_USAGE_0_POS                        (17U)
#define EF_DATA_0_WR_LOCK_SW_USAGE_0_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_SW_USAGE_0_MSK                        (((1U<<EF_DATA_0_WR_LOCK_SW_USAGE_0_LEN)-1)<<EF_DATA_0_WR_LOCK_SW_USAGE_0_POS)
#define EF_DATA_0_WR_LOCK_SW_USAGE_0_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_SW_USAGE_0_LEN)-1)<<EF_DATA_0_WR_LOCK_SW_USAGE_0_POS))
#define EF_DATA_0_WR_LOCK_WIFI_MAC                              EF_DATA_0_WR_LOCK_WIFI_MAC
#define EF_DATA_0_WR_LOCK_WIFI_MAC_POS                          (18U)
#define EF_DATA_0_WR_LOCK_WIFI_MAC_LEN                          (1U)
#define EF_DATA_0_WR_LOCK_WIFI_MAC_MSK                          (((1U<<EF_DATA_0_WR_LOCK_WIFI_MAC_LEN)-1)<<EF_DATA_0_WR_LOCK_WIFI_MAC_POS)
#define EF_DATA_0_WR_LOCK_WIFI_MAC_UMSK                         (~(((1U<<EF_DATA_0_WR_LOCK_WIFI_MAC_LEN)-1)<<EF_DATA_0_WR_LOCK_WIFI_MAC_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_0                            EF_DATA_0_WR_LOCK_KEY_SLOT_0
#define EF_DATA_0_WR_LOCK_KEY_SLOT_0_POS                        (19U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_0_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_0_MSK                        (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_0_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_0_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_0_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_0_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_0_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_1                            EF_DATA_0_WR_LOCK_KEY_SLOT_1
#define EF_DATA_0_WR_LOCK_KEY_SLOT_1_POS                        (20U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_1_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_1_MSK                        (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_1_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_1_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_1_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_1_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_1_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_2                            EF_DATA_0_WR_LOCK_KEY_SLOT_2
#define EF_DATA_0_WR_LOCK_KEY_SLOT_2_POS                        (21U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_2_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_2_MSK                        (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_2_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_2_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_2_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_2_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_2_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_L                          EF_DATA_0_WR_LOCK_KEY_SLOT_3_L
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_POS                      (22U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_LEN                      (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_MSK                      (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_UMSK                     (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_L_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_4                            EF_DATA_0_WR_LOCK_KEY_SLOT_4
#define EF_DATA_0_WR_LOCK_KEY_SLOT_4_POS                        (23U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_4_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_4_MSK                        (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_4_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_4_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_4_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_4_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_4_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_5                            EF_DATA_0_WR_LOCK_KEY_SLOT_5
#define EF_DATA_0_WR_LOCK_KEY_SLOT_5_POS                        (24U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_5_LEN                        (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_5_MSK                        (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_5_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_5_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_5_UMSK                       (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_5_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_5_POS))
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_H                          EF_DATA_0_WR_LOCK_KEY_SLOT_3_H
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_POS                      (25U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_LEN                      (1U)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_MSK                      (((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_POS)
#define EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_UMSK                     (~(((1U<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_LEN)-1)<<EF_DATA_0_WR_LOCK_KEY_SLOT_3_H_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_0                            EF_DATA_0_RD_LOCK_KEY_SLOT_0
#define EF_DATA_0_RD_LOCK_KEY_SLOT_0_POS                        (26U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_0_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_0_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_0_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_0_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_0_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_0_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_0_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_1                            EF_DATA_0_RD_LOCK_KEY_SLOT_1
#define EF_DATA_0_RD_LOCK_KEY_SLOT_1_POS                        (27U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_1_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_1_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_1_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_1_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_1_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_1_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_1_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_2                            EF_DATA_0_RD_LOCK_KEY_SLOT_2
#define EF_DATA_0_RD_LOCK_KEY_SLOT_2_POS                        (28U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_2_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_2_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_2_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_2_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_2_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_2_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_2_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_3                            EF_DATA_0_RD_LOCK_KEY_SLOT_3
#define EF_DATA_0_RD_LOCK_KEY_SLOT_3_POS                        (29U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_3_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_3_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_3_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_3_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_3_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_3_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_3_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_4                            EF_DATA_0_RD_LOCK_KEY_SLOT_4
#define EF_DATA_0_RD_LOCK_KEY_SLOT_4_POS                        (30U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_4_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_4_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_4_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_4_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_4_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_4_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_4_POS))
#define EF_DATA_0_RD_LOCK_KEY_SLOT_5                            EF_DATA_0_RD_LOCK_KEY_SLOT_5
#define EF_DATA_0_RD_LOCK_KEY_SLOT_5_POS                        (31U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_5_LEN                        (1U)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_5_MSK                        (((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_5_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_5_POS)
#define EF_DATA_0_RD_LOCK_KEY_SLOT_5_UMSK                       (~(((1U<<EF_DATA_0_RD_LOCK_KEY_SLOT_5_LEN)-1)<<EF_DATA_0_RD_LOCK_KEY_SLOT_5_POS))


struct  ef_data_0_reg {
    /* 0x0 : ef_cfg_0 */
    union {
        struct {
            uint32_t ef_sf_aes_mode                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t ef_sboot_sign_mode             :  2; /* [ 3: 2],        r/w,        0x0 */
            uint32_t ef_sboot_en                    :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t ef_cpu1_enc_en                 :  1; /* [    6],        r/w,        0x0 */
            uint32_t ef_cpu0_enc_en                 :  1; /* [    7],        r/w,        0x0 */
            uint32_t ef_boot_sel                    :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t ef_sf_key_0_sel                :  2; /* [13:12],        r/w,        0x0 */
            uint32_t ef_sdu_dis                     :  1; /* [   14],        r/w,        0x0 */
            uint32_t ef_ble_dis                     :  1; /* [   15],        r/w,        0x0 */
            uint32_t ef_wifi_dis                    :  1; /* [   16],        r/w,        0x0 */
            uint32_t ef_0_key_enc_en                :  1; /* [   17],        r/w,        0x0 */
            uint32_t ef_cam_dis                     :  1; /* [   18],        r/w,        0x0 */
            uint32_t ef_m154_dis                    :  1; /* [   19],        r/w,        0x0 */
            uint32_t ef_cpu1_dis                    :  1; /* [   20],        r/w,        0x0 */
            uint32_t ef_cpu_rst_dbg_dis             :  1; /* [   21],        r/w,        0x0 */
            uint32_t ef_se_dbg_dis                  :  1; /* [   22],        r/w,        0x0 */
            uint32_t ef_efuse_dbg_dis               :  1; /* [   23],        r/w,        0x0 */
            uint32_t ef_dbg_jtag_1_dis              :  2; /* [25:24],        r/w,        0x0 */
            uint32_t ef_dbg_jtag_0_dis              :  2; /* [27:26],        r/w,        0x0 */
            uint32_t ef_dbg_mode                    :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_cfg_0;

    /* 0x4 : ef_sw_usage_0 */
    union {
        struct {
            uint32_t ef_sw_usage_0                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_sw_usage_0;

    /* 0x8 : ef_sw_usage_1 */
    union {
        struct {
            uint32_t ef_sw_usage_1                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_sw_usage_1;

    /* 0xC : ef_ana_trim_0 */
    union {
        struct {
            uint32_t ef_ana_trim_0                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_ana_trim_0;

    /* 0x10 : ef_ana_trim_1 */
    union {
        struct {
            uint32_t ef_ana_trim_1                  : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_ana_trim_1;

    /* 0x14 : ef_wifi_mac_low */
    union {
        struct {
            uint32_t ef_wifi_mac_low                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_wifi_mac_low;

    /* 0x18 : ef_wifi_mac_high */
    union {
        struct {
            uint32_t ef_wifi_mac_high               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_wifi_mac_high;

    /* 0x1C : ef_key_slot_0_w0 */
    union {
        struct {
            uint32_t ef_key_slot_0_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_0_w0;

    /* 0x20 : ef_key_slot_0_w1 */
    union {
        struct {
            uint32_t ef_key_slot_0_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_0_w1;

    /* 0x24 : ef_key_slot_0_w2 */
    union {
        struct {
            uint32_t ef_key_slot_0_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_0_w2;

    /* 0x28 : ef_key_slot_0_w3 */
    union {
        struct {
            uint32_t ef_key_slot_0_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_0_w3;

    /* 0x2C : ef_key_slot_1_w0 */
    union {
        struct {
            uint32_t ef_key_slot_1_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_1_w0;

    /* 0x30 : ef_key_slot_1_w1 */
    union {
        struct {
            uint32_t ef_key_slot_1_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_1_w1;

    /* 0x34 : ef_key_slot_1_w2 */
    union {
        struct {
            uint32_t ef_key_slot_1_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_1_w2;

    /* 0x38 : ef_key_slot_1_w3 */
    union {
        struct {
            uint32_t ef_key_slot_1_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_1_w3;

    /* 0x3C : ef_key_slot_2_w0 */
    union {
        struct {
            uint32_t ef_key_slot_2_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_2_w0;

    /* 0x40 : ef_key_slot_2_w1 */
    union {
        struct {
            uint32_t ef_key_slot_2_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_2_w1;

    /* 0x44 : ef_key_slot_2_w2 */
    union {
        struct {
            uint32_t ef_key_slot_2_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_2_w2;

    /* 0x48 : ef_key_slot_2_w3 */
    union {
        struct {
            uint32_t ef_key_slot_2_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_2_w3;

    /* 0x4C : ef_key_slot_3_w0 */
    union {
        struct {
            uint32_t ef_key_slot_3_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_3_w0;

    /* 0x50 : ef_key_slot_3_w1 */
    union {
        struct {
            uint32_t ef_key_slot_3_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_3_w1;

    /* 0x54 : ef_key_slot_3_w2 */
    union {
        struct {
            uint32_t ef_key_slot_3_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_3_w2;

    /* 0x58 : ef_key_slot_3_w3 */
    union {
        struct {
            uint32_t ef_key_slot_3_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_3_w3;

    /* 0x5C : ef_key_slot_4_w0 */
    union {
        struct {
            uint32_t ef_key_slot_4_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_4_w0;

    /* 0x60 : ef_key_slot_4_w1 */
    union {
        struct {
            uint32_t ef_key_slot_4_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_4_w1;

    /* 0x64 : ef_key_slot_4_w2 */
    union {
        struct {
            uint32_t ef_key_slot_4_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_4_w2;

    /* 0x68 : ef_key_slot_4_w3 */
    union {
        struct {
            uint32_t ef_key_slot_4_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_4_w3;

    /* 0x6C : ef_key_slot_5_w0 */
    union {
        struct {
            uint32_t ef_key_slot_5_w0               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_5_w0;

    /* 0x70 : ef_key_slot_5_w1 */
    union {
        struct {
            uint32_t ef_key_slot_5_w1               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_5_w1;

    /* 0x74 : ef_key_slot_5_w2 */
    union {
        struct {
            uint32_t ef_key_slot_5_w2               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_5_w2;

    /* 0x78 : ef_key_slot_5_w3 */
    union {
        struct {
            uint32_t ef_key_slot_5_w3               : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_key_slot_5_w3;

    /* 0x7C : ef_data_0_lock */
    union {
        struct {
            uint32_t ef_sec_lifecycle               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t ef_ana_trim_2                  :  9; /* [12: 4],        r/w,        0x0 */
            uint32_t wr_lock_trim_1                 :  1; /* [   13],        r/w,        0x0 */
            uint32_t wr_lock_trim_0                 :  1; /* [   14],        r/w,        0x0 */
            uint32_t wr_lock_boot_mode              :  1; /* [   15],        r/w,        0x0 */
            uint32_t wr_lock_sw_usage_1             :  1; /* [   16],        r/w,        0x0 */
            uint32_t wr_lock_sw_usage_0             :  1; /* [   17],        r/w,        0x0 */
            uint32_t wr_lock_wifi_mac               :  1; /* [   18],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_0             :  1; /* [   19],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_1             :  1; /* [   20],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_2             :  1; /* [   21],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_3_l           :  1; /* [   22],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_4             :  1; /* [   23],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_5             :  1; /* [   24],        r/w,        0x0 */
            uint32_t wr_lock_key_slot_3_h           :  1; /* [   25],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_0             :  1; /* [   26],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_1             :  1; /* [   27],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_2             :  1; /* [   28],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_3             :  1; /* [   29],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_4             :  1; /* [   30],        r/w,        0x0 */
            uint32_t rd_lock_key_slot_5             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ef_data_0_lock;

};

typedef volatile struct ef_data_0_reg ef_data_0_reg_t;


#endif  /* __EF_DATA_0_REG_H__ */
