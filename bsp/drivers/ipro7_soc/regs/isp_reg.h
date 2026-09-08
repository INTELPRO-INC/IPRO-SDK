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

#ifndef  __ISP_REG_H__
#define  __ISP_REG_H__

#include "ipro7.h"

/* 0x400 : YUVC_A */
#define ISP_YUVC_A_OFFSET                                       (0x400)
#define ISP_YBRIGHTNESS                                         ISP_YBRIGHTNESS
#define ISP_YBRIGHTNESS_POS                                     (0U)
#define ISP_YBRIGHTNESS_LEN                                     (8U)
#define ISP_YBRIGHTNESS_MSK                                     (((1U<<ISP_YBRIGHTNESS_LEN)-1)<<ISP_YBRIGHTNESS_POS)
#define ISP_YBRIGHTNESS_UMSK                                    (~(((1U<<ISP_YBRIGHTNESS_LEN)-1)<<ISP_YBRIGHTNESS_POS))
#define ISP_YCONTRAST                                           ISP_YCONTRAST
#define ISP_YCONTRAST_POS                                       (8U)
#define ISP_YCONTRAST_LEN                                       (8U)
#define ISP_YCONTRAST_MSK                                       (((1U<<ISP_YCONTRAST_LEN)-1)<<ISP_YCONTRAST_POS)
#define ISP_YCONTRAST_UMSK                                      (~(((1U<<ISP_YCONTRAST_LEN)-1)<<ISP_YCONTRAST_POS))
#define ISP_CBSATURATION                                        ISP_CBSATURATION
#define ISP_CBSATURATION_POS                                    (16U)
#define ISP_CBSATURATION_LEN                                    (8U)
#define ISP_CBSATURATION_MSK                                    (((1U<<ISP_CBSATURATION_LEN)-1)<<ISP_CBSATURATION_POS)
#define ISP_CBSATURATION_UMSK                                   (~(((1U<<ISP_CBSATURATION_LEN)-1)<<ISP_CBSATURATION_POS))
#define ISP_CRSATURATION                                        ISP_CRSATURATION
#define ISP_CRSATURATION_POS                                    (24U)
#define ISP_CRSATURATION_LEN                                    (8U)
#define ISP_CRSATURATION_MSK                                    (((1U<<ISP_CRSATURATION_LEN)-1)<<ISP_CRSATURATION_POS)
#define ISP_CRSATURATION_UMSK                                   (~(((1U<<ISP_CRSATURATION_LEN)-1)<<ISP_CRSATURATION_POS))

/* 0x404 : YUVC_B */
#define ISP_YUVC_B_OFFSET                                       (0x404)
#define ISP_SPECIALMODE                                         ISP_SPECIALMODE
#define ISP_SPECIALMODE_POS                                     (0U)
#define ISP_SPECIALMODE_LEN                                     (3U)
#define ISP_SPECIALMODE_MSK                                     (((1U<<ISP_SPECIALMODE_LEN)-1)<<ISP_SPECIALMODE_POS)
#define ISP_SPECIALMODE_UMSK                                    (~(((1U<<ISP_SPECIALMODE_LEN)-1)<<ISP_SPECIALMODE_POS))
#define ISP_C444TO422FILTERINGMODE                              ISP_C444TO422FILTERINGMODE
#define ISP_C444TO422FILTERINGMODE_POS                          (4U)
#define ISP_C444TO422FILTERINGMODE_LEN                          (1U)
#define ISP_C444TO422FILTERINGMODE_MSK                          (((1U<<ISP_C444TO422FILTERINGMODE_LEN)-1)<<ISP_C444TO422FILTERINGMODE_POS)
#define ISP_C444TO422FILTERINGMODE_UMSK                         (~(((1U<<ISP_C444TO422FILTERINGMODE_LEN)-1)<<ISP_C444TO422FILTERINGMODE_POS))
#define ISP_SEPIACB                                             ISP_SEPIACB
#define ISP_SEPIACB_POS                                         (8U)
#define ISP_SEPIACB_LEN                                         (8U)
#define ISP_SEPIACB_MSK                                         (((1U<<ISP_SEPIACB_LEN)-1)<<ISP_SEPIACB_POS)
#define ISP_SEPIACB_UMSK                                        (~(((1U<<ISP_SEPIACB_LEN)-1)<<ISP_SEPIACB_POS))
#define ISP_SEPIACR                                             ISP_SEPIACR
#define ISP_SEPIACR_POS                                         (16U)
#define ISP_SEPIACR_LEN                                         (8U)
#define ISP_SEPIACR_MSK                                         (((1U<<ISP_SEPIACR_LEN)-1)<<ISP_SEPIACR_POS)
#define ISP_SEPIACR_UMSK                                        (~(((1U<<ISP_SEPIACR_LEN)-1)<<ISP_SEPIACR_POS))
#define ISP_SOLARTHRED                                          ISP_SOLARTHRED
#define ISP_SOLARTHRED_POS                                      (24U)
#define ISP_SOLARTHRED_LEN                                      (8U)
#define ISP_SOLARTHRED_MSK                                      (((1U<<ISP_SOLARTHRED_LEN)-1)<<ISP_SOLARTHRED_POS)
#define ISP_SOLARTHRED_UMSK                                     (~(((1U<<ISP_SOLARTHRED_LEN)-1)<<ISP_SOLARTHRED_POS))

/* 0x420 : CROP_0 */
#define ISP_CROP_0_OFFSET                                       (0x420)
#define ISP_REG_CROP_VSYNC_START                                ISP_REG_CROP_VSYNC_START
#define ISP_REG_CROP_VSYNC_START_POS                            (0U)
#define ISP_REG_CROP_VSYNC_START_LEN                            (15U)
#define ISP_REG_CROP_VSYNC_START_MSK                            (((1U<<ISP_REG_CROP_VSYNC_START_LEN)-1)<<ISP_REG_CROP_VSYNC_START_POS)
#define ISP_REG_CROP_VSYNC_START_UMSK                           (~(((1U<<ISP_REG_CROP_VSYNC_START_LEN)-1)<<ISP_REG_CROP_VSYNC_START_POS))
#define ISP_REG_CROP_VSYNC_END                                  ISP_REG_CROP_VSYNC_END
#define ISP_REG_CROP_VSYNC_END_POS                              (16U)
#define ISP_REG_CROP_VSYNC_END_LEN                              (15U)
#define ISP_REG_CROP_VSYNC_END_MSK                              (((1U<<ISP_REG_CROP_VSYNC_END_LEN)-1)<<ISP_REG_CROP_VSYNC_END_POS)
#define ISP_REG_CROP_VSYNC_END_UMSK                             (~(((1U<<ISP_REG_CROP_VSYNC_END_LEN)-1)<<ISP_REG_CROP_VSYNC_END_POS))
#define ISP_REG_CROP_ENABLE                                     ISP_REG_CROP_ENABLE
#define ISP_REG_CROP_ENABLE_POS                                 (31U)
#define ISP_REG_CROP_ENABLE_LEN                                 (1U)
#define ISP_REG_CROP_ENABLE_MSK                                 (((1U<<ISP_REG_CROP_ENABLE_LEN)-1)<<ISP_REG_CROP_ENABLE_POS)
#define ISP_REG_CROP_ENABLE_UMSK                                (~(((1U<<ISP_REG_CROP_ENABLE_LEN)-1)<<ISP_REG_CROP_ENABLE_POS))

/* 0x424 : CROP_1 */
#define ISP_CROP_1_OFFSET                                       (0x424)
#define ISP_REG_CROP_HSYNC_START                                ISP_REG_CROP_HSYNC_START
#define ISP_REG_CROP_HSYNC_START_POS                            (0U)
#define ISP_REG_CROP_HSYNC_START_LEN                            (15U)
#define ISP_REG_CROP_HSYNC_START_MSK                            (((1U<<ISP_REG_CROP_HSYNC_START_LEN)-1)<<ISP_REG_CROP_HSYNC_START_POS)
#define ISP_REG_CROP_HSYNC_START_UMSK                           (~(((1U<<ISP_REG_CROP_HSYNC_START_LEN)-1)<<ISP_REG_CROP_HSYNC_START_POS))
#define ISP_REG_CROP_HSYNC_END                                  ISP_REG_CROP_HSYNC_END
#define ISP_REG_CROP_HSYNC_END_POS                              (16U)
#define ISP_REG_CROP_HSYNC_END_LEN                              (15U)
#define ISP_REG_CROP_HSYNC_END_MSK                              (((1U<<ISP_REG_CROP_HSYNC_END_LEN)-1)<<ISP_REG_CROP_HSYNC_END_POS)
#define ISP_REG_CROP_HSYNC_END_UMSK                             (~(((1U<<ISP_REG_CROP_HSYNC_END_LEN)-1)<<ISP_REG_CROP_HSYNC_END_POS))

/* 0x440 : adj_ctrl_0 */
#define ISP_ADJ_CTRL_0_OFFSET                                   (0x440)
#define ISP_REG_ADJ_ADJ_EB                                      ISP_REG_ADJ_ADJ_EB
#define ISP_REG_ADJ_ADJ_EB_POS                                  (0U)
#define ISP_REG_ADJ_ADJ_EB_LEN                                  (1U)
#define ISP_REG_ADJ_ADJ_EB_MSK                                  (((1U<<ISP_REG_ADJ_ADJ_EB_LEN)-1)<<ISP_REG_ADJ_ADJ_EB_POS)
#define ISP_REG_ADJ_ADJ_EB_UMSK                                 (~(((1U<<ISP_REG_ADJ_ADJ_EB_LEN)-1)<<ISP_REG_ADJ_ADJ_EB_POS))
#define ISP_REG_ADJ_Y_LUMA                                      ISP_REG_ADJ_Y_LUMA
#define ISP_REG_ADJ_Y_LUMA_POS                                  (1U)
#define ISP_REG_ADJ_Y_LUMA_LEN                                  (9U)
#define ISP_REG_ADJ_Y_LUMA_MSK                                  (((1U<<ISP_REG_ADJ_Y_LUMA_LEN)-1)<<ISP_REG_ADJ_Y_LUMA_POS)
#define ISP_REG_ADJ_Y_LUMA_UMSK                                 (~(((1U<<ISP_REG_ADJ_Y_LUMA_LEN)-1)<<ISP_REG_ADJ_Y_LUMA_POS))
#define ISP_REG_ADJ_Y_MUL0                                      ISP_REG_ADJ_Y_MUL0
#define ISP_REG_ADJ_Y_MUL0_POS                                  (10U)
#define ISP_REG_ADJ_Y_MUL0_LEN                                  (11U)
#define ISP_REG_ADJ_Y_MUL0_MSK                                  (((1U<<ISP_REG_ADJ_Y_MUL0_LEN)-1)<<ISP_REG_ADJ_Y_MUL0_POS)
#define ISP_REG_ADJ_Y_MUL0_UMSK                                 (~(((1U<<ISP_REG_ADJ_Y_MUL0_LEN)-1)<<ISP_REG_ADJ_Y_MUL0_POS))
#define ISP_REG_ADJ_Y_MUL1                                      ISP_REG_ADJ_Y_MUL1
#define ISP_REG_ADJ_Y_MUL1_POS                                  (21U)
#define ISP_REG_ADJ_Y_MUL1_LEN                                  (11U)
#define ISP_REG_ADJ_Y_MUL1_MSK                                  (((1U<<ISP_REG_ADJ_Y_MUL1_LEN)-1)<<ISP_REG_ADJ_Y_MUL1_POS)
#define ISP_REG_ADJ_Y_MUL1_UMSK                                 (~(((1U<<ISP_REG_ADJ_Y_MUL1_LEN)-1)<<ISP_REG_ADJ_Y_MUL1_POS))

/* 0x444 : adjA_ctrl_1 */
#define ISP_ADJA_CTRL_1_OFFSET                                  (0x444)
#define ISP_REG_ADJ_Y_MIN                                       ISP_REG_ADJ_Y_MIN
#define ISP_REG_ADJ_Y_MIN_POS                                   (0U)
#define ISP_REG_ADJ_Y_MIN_LEN                                   (8U)
#define ISP_REG_ADJ_Y_MIN_MSK                                   (((1U<<ISP_REG_ADJ_Y_MIN_LEN)-1)<<ISP_REG_ADJ_Y_MIN_POS)
#define ISP_REG_ADJ_Y_MIN_UMSK                                  (~(((1U<<ISP_REG_ADJ_Y_MIN_LEN)-1)<<ISP_REG_ADJ_Y_MIN_POS))
#define ISP_REG_ADJ_Y_MAX                                       ISP_REG_ADJ_Y_MAX
#define ISP_REG_ADJ_Y_MAX_POS                                   (8U)
#define ISP_REG_ADJ_Y_MAX_LEN                                   (8U)
#define ISP_REG_ADJ_Y_MAX_MSK                                   (((1U<<ISP_REG_ADJ_Y_MAX_LEN)-1)<<ISP_REG_ADJ_Y_MAX_POS)
#define ISP_REG_ADJ_Y_MAX_UMSK                                  (~(((1U<<ISP_REG_ADJ_Y_MAX_LEN)-1)<<ISP_REG_ADJ_Y_MAX_POS))
#define ISP_REG_ADJ_UV_MIN                                      ISP_REG_ADJ_UV_MIN
#define ISP_REG_ADJ_UV_MIN_POS                                  (16U)
#define ISP_REG_ADJ_UV_MIN_LEN                                  (8U)
#define ISP_REG_ADJ_UV_MIN_MSK                                  (((1U<<ISP_REG_ADJ_UV_MIN_LEN)-1)<<ISP_REG_ADJ_UV_MIN_POS)
#define ISP_REG_ADJ_UV_MIN_UMSK                                 (~(((1U<<ISP_REG_ADJ_UV_MIN_LEN)-1)<<ISP_REG_ADJ_UV_MIN_POS))
#define ISP_REG_ADJ_UV_MAX                                      ISP_REG_ADJ_UV_MAX
#define ISP_REG_ADJ_UV_MAX_POS                                  (24U)
#define ISP_REG_ADJ_UV_MAX_LEN                                  (8U)
#define ISP_REG_ADJ_UV_MAX_MSK                                  (((1U<<ISP_REG_ADJ_UV_MAX_LEN)-1)<<ISP_REG_ADJ_UV_MAX_POS)
#define ISP_REG_ADJ_UV_MAX_UMSK                                 (~(((1U<<ISP_REG_ADJ_UV_MAX_LEN)-1)<<ISP_REG_ADJ_UV_MAX_POS))

/* 0x448 : adjA_ctrl_2 */
#define ISP_ADJA_CTRL_2_OFFSET                                  (0x448)
#define ISP_REG_ADJ_UV_MUL0                                     ISP_REG_ADJ_UV_MUL0
#define ISP_REG_ADJ_UV_MUL0_POS                                 (10U)
#define ISP_REG_ADJ_UV_MUL0_LEN                                 (11U)
#define ISP_REG_ADJ_UV_MUL0_MSK                                 (((1U<<ISP_REG_ADJ_UV_MUL0_LEN)-1)<<ISP_REG_ADJ_UV_MUL0_POS)
#define ISP_REG_ADJ_UV_MUL0_UMSK                                (~(((1U<<ISP_REG_ADJ_UV_MUL0_LEN)-1)<<ISP_REG_ADJ_UV_MUL0_POS))
#define ISP_REG_ADJ_UV_MUL1                                     ISP_REG_ADJ_UV_MUL1
#define ISP_REG_ADJ_UV_MUL1_POS                                 (21U)
#define ISP_REG_ADJ_UV_MUL1_LEN                                 (11U)
#define ISP_REG_ADJ_UV_MUL1_MSK                                 (((1U<<ISP_REG_ADJ_UV_MUL1_LEN)-1)<<ISP_REG_ADJ_UV_MUL1_POS)
#define ISP_REG_ADJ_UV_MUL1_UMSK                                (~(((1U<<ISP_REG_ADJ_UV_MUL1_LEN)-1)<<ISP_REG_ADJ_UV_MUL1_POS))

/* 0x480 : ce_config_0 */
#define ISP_CE_CONFIG_0_OFFSET                                  (0x480)
#define ISP_RG_CE_PRE_0                                         ISP_RG_CE_PRE_0
#define ISP_RG_CE_PRE_0_POS                                     (0U)
#define ISP_RG_CE_PRE_0_LEN                                     (13U)
#define ISP_RG_CE_PRE_0_MSK                                     (((1U<<ISP_RG_CE_PRE_0_LEN)-1)<<ISP_RG_CE_PRE_0_POS)
#define ISP_RG_CE_PRE_0_UMSK                                    (~(((1U<<ISP_RG_CE_PRE_0_LEN)-1)<<ISP_RG_CE_PRE_0_POS))
#define ISP_RG_CE_POS_0                                         ISP_RG_CE_POS_0
#define ISP_RG_CE_POS_0_POS                                     (16U)
#define ISP_RG_CE_POS_0_LEN                                     (13U)
#define ISP_RG_CE_POS_0_MSK                                     (((1U<<ISP_RG_CE_POS_0_LEN)-1)<<ISP_RG_CE_POS_0_POS)
#define ISP_RG_CE_POS_0_UMSK                                    (~(((1U<<ISP_RG_CE_POS_0_LEN)-1)<<ISP_RG_CE_POS_0_POS))
#define ISP_RG_CE_EN                                            ISP_RG_CE_EN
#define ISP_RG_CE_EN_POS                                        (31U)
#define ISP_RG_CE_EN_LEN                                        (1U)
#define ISP_RG_CE_EN_MSK                                        (((1U<<ISP_RG_CE_EN_LEN)-1)<<ISP_RG_CE_EN_POS)
#define ISP_RG_CE_EN_UMSK                                       (~(((1U<<ISP_RG_CE_EN_LEN)-1)<<ISP_RG_CE_EN_POS))

/* 0x484 : ce_config_1 */
#define ISP_CE_CONFIG_1_OFFSET                                  (0x484)
#define ISP_RG_CE_PRE_1                                         ISP_RG_CE_PRE_1
#define ISP_RG_CE_PRE_1_POS                                     (0U)
#define ISP_RG_CE_PRE_1_LEN                                     (13U)
#define ISP_RG_CE_PRE_1_MSK                                     (((1U<<ISP_RG_CE_PRE_1_LEN)-1)<<ISP_RG_CE_PRE_1_POS)
#define ISP_RG_CE_PRE_1_UMSK                                    (~(((1U<<ISP_RG_CE_PRE_1_LEN)-1)<<ISP_RG_CE_PRE_1_POS))
#define ISP_RG_CE_POS_1                                         ISP_RG_CE_POS_1
#define ISP_RG_CE_POS_1_POS                                     (16U)
#define ISP_RG_CE_POS_1_LEN                                     (13U)
#define ISP_RG_CE_POS_1_MSK                                     (((1U<<ISP_RG_CE_POS_1_LEN)-1)<<ISP_RG_CE_POS_1_POS)
#define ISP_RG_CE_POS_1_UMSK                                    (~(((1U<<ISP_RG_CE_POS_1_LEN)-1)<<ISP_RG_CE_POS_1_POS))

/* 0x488 : ce_config_2 */
#define ISP_CE_CONFIG_2_OFFSET                                  (0x488)
#define ISP_RG_CE_PRE_2                                         ISP_RG_CE_PRE_2
#define ISP_RG_CE_PRE_2_POS                                     (0U)
#define ISP_RG_CE_PRE_2_LEN                                     (13U)
#define ISP_RG_CE_PRE_2_MSK                                     (((1U<<ISP_RG_CE_PRE_2_LEN)-1)<<ISP_RG_CE_PRE_2_POS)
#define ISP_RG_CE_PRE_2_UMSK                                    (~(((1U<<ISP_RG_CE_PRE_2_LEN)-1)<<ISP_RG_CE_PRE_2_POS))
#define ISP_RG_CE_POS_2                                         ISP_RG_CE_POS_2
#define ISP_RG_CE_POS_2_POS                                     (16U)
#define ISP_RG_CE_POS_2_LEN                                     (13U)
#define ISP_RG_CE_POS_2_MSK                                     (((1U<<ISP_RG_CE_POS_2_LEN)-1)<<ISP_RG_CE_POS_2_POS)
#define ISP_RG_CE_POS_2_UMSK                                    (~(((1U<<ISP_RG_CE_POS_2_LEN)-1)<<ISP_RG_CE_POS_2_POS))

/* 0x48C : ce_config_3 */
#define ISP_CE_CONFIG_3_OFFSET                                  (0x48C)
#define ISP_RG_CE_MTX_00                                        ISP_RG_CE_MTX_00
#define ISP_RG_CE_MTX_00_POS                                    (0U)
#define ISP_RG_CE_MTX_00_LEN                                    (12U)
#define ISP_RG_CE_MTX_00_MSK                                    (((1U<<ISP_RG_CE_MTX_00_LEN)-1)<<ISP_RG_CE_MTX_00_POS)
#define ISP_RG_CE_MTX_00_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_00_LEN)-1)<<ISP_RG_CE_MTX_00_POS))
#define ISP_RG_CE_MTX_01                                        ISP_RG_CE_MTX_01
#define ISP_RG_CE_MTX_01_POS                                    (16U)
#define ISP_RG_CE_MTX_01_LEN                                    (12U)
#define ISP_RG_CE_MTX_01_MSK                                    (((1U<<ISP_RG_CE_MTX_01_LEN)-1)<<ISP_RG_CE_MTX_01_POS)
#define ISP_RG_CE_MTX_01_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_01_LEN)-1)<<ISP_RG_CE_MTX_01_POS))

/* 0x490 : ce_config_4 */
#define ISP_CE_CONFIG_4_OFFSET                                  (0x490)
#define ISP_RG_CE_MTX_02                                        ISP_RG_CE_MTX_02
#define ISP_RG_CE_MTX_02_POS                                    (0U)
#define ISP_RG_CE_MTX_02_LEN                                    (12U)
#define ISP_RG_CE_MTX_02_MSK                                    (((1U<<ISP_RG_CE_MTX_02_LEN)-1)<<ISP_RG_CE_MTX_02_POS)
#define ISP_RG_CE_MTX_02_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_02_LEN)-1)<<ISP_RG_CE_MTX_02_POS))
#define ISP_RG_CE_MTX_10                                        ISP_RG_CE_MTX_10
#define ISP_RG_CE_MTX_10_POS                                    (16U)
#define ISP_RG_CE_MTX_10_LEN                                    (12U)
#define ISP_RG_CE_MTX_10_MSK                                    (((1U<<ISP_RG_CE_MTX_10_LEN)-1)<<ISP_RG_CE_MTX_10_POS)
#define ISP_RG_CE_MTX_10_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_10_LEN)-1)<<ISP_RG_CE_MTX_10_POS))

/* 0x494 : ce_config_5 */
#define ISP_CE_CONFIG_5_OFFSET                                  (0x494)
#define ISP_RG_CE_MTX_11                                        ISP_RG_CE_MTX_11
#define ISP_RG_CE_MTX_11_POS                                    (0U)
#define ISP_RG_CE_MTX_11_LEN                                    (12U)
#define ISP_RG_CE_MTX_11_MSK                                    (((1U<<ISP_RG_CE_MTX_11_LEN)-1)<<ISP_RG_CE_MTX_11_POS)
#define ISP_RG_CE_MTX_11_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_11_LEN)-1)<<ISP_RG_CE_MTX_11_POS))
#define ISP_RG_CE_MTX_12                                        ISP_RG_CE_MTX_12
#define ISP_RG_CE_MTX_12_POS                                    (16U)
#define ISP_RG_CE_MTX_12_LEN                                    (12U)
#define ISP_RG_CE_MTX_12_MSK                                    (((1U<<ISP_RG_CE_MTX_12_LEN)-1)<<ISP_RG_CE_MTX_12_POS)
#define ISP_RG_CE_MTX_12_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_12_LEN)-1)<<ISP_RG_CE_MTX_12_POS))

/* 0x498 : ce_config_6 */
#define ISP_CE_CONFIG_6_OFFSET                                  (0x498)
#define ISP_RG_CE_MTX_20                                        ISP_RG_CE_MTX_20
#define ISP_RG_CE_MTX_20_POS                                    (0U)
#define ISP_RG_CE_MTX_20_LEN                                    (12U)
#define ISP_RG_CE_MTX_20_MSK                                    (((1U<<ISP_RG_CE_MTX_20_LEN)-1)<<ISP_RG_CE_MTX_20_POS)
#define ISP_RG_CE_MTX_20_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_20_LEN)-1)<<ISP_RG_CE_MTX_20_POS))
#define ISP_RG_CE_MTX_21                                        ISP_RG_CE_MTX_21
#define ISP_RG_CE_MTX_21_POS                                    (16U)
#define ISP_RG_CE_MTX_21_LEN                                    (12U)
#define ISP_RG_CE_MTX_21_MSK                                    (((1U<<ISP_RG_CE_MTX_21_LEN)-1)<<ISP_RG_CE_MTX_21_POS)
#define ISP_RG_CE_MTX_21_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_21_LEN)-1)<<ISP_RG_CE_MTX_21_POS))

/* 0x49C : ce_config_7 */
#define ISP_CE_CONFIG_7_OFFSET                                  (0x49C)
#define ISP_RG_CE_MTX_22                                        ISP_RG_CE_MTX_22
#define ISP_RG_CE_MTX_22_POS                                    (0U)
#define ISP_RG_CE_MTX_22_LEN                                    (12U)
#define ISP_RG_CE_MTX_22_MSK                                    (((1U<<ISP_RG_CE_MTX_22_LEN)-1)<<ISP_RG_CE_MTX_22_POS)
#define ISP_RG_CE_MTX_22_UMSK                                   (~(((1U<<ISP_RG_CE_MTX_22_LEN)-1)<<ISP_RG_CE_MTX_22_POS))

/* 0xA00 : awb3_config_0 */
#define ISP_AWB3_CONFIG_0_OFFSET                                (0xA00)
#define ISP_AWB3_STAT_X_MAX                                     ISP_AWB3_STAT_X_MAX
#define ISP_AWB3_STAT_X_MAX_POS                                 (0U)
#define ISP_AWB3_STAT_X_MAX_LEN                                 (12U)
#define ISP_AWB3_STAT_X_MAX_MSK                                 (((1U<<ISP_AWB3_STAT_X_MAX_LEN)-1)<<ISP_AWB3_STAT_X_MAX_POS)
#define ISP_AWB3_STAT_X_MAX_UMSK                                (~(((1U<<ISP_AWB3_STAT_X_MAX_LEN)-1)<<ISP_AWB3_STAT_X_MAX_POS))
#define ISP_AWB3_STAT_X_MIN                                     ISP_AWB3_STAT_X_MIN
#define ISP_AWB3_STAT_X_MIN_POS                                 (16U)
#define ISP_AWB3_STAT_X_MIN_LEN                                 (12U)
#define ISP_AWB3_STAT_X_MIN_MSK                                 (((1U<<ISP_AWB3_STAT_X_MIN_LEN)-1)<<ISP_AWB3_STAT_X_MIN_POS)
#define ISP_AWB3_STAT_X_MIN_UMSK                                (~(((1U<<ISP_AWB3_STAT_X_MIN_LEN)-1)<<ISP_AWB3_STAT_X_MIN_POS))
#define ISP_AWB3_STAT_EN                                        ISP_AWB3_STAT_EN
#define ISP_AWB3_STAT_EN_POS                                    (31U)
#define ISP_AWB3_STAT_EN_LEN                                    (1U)
#define ISP_AWB3_STAT_EN_MSK                                    (((1U<<ISP_AWB3_STAT_EN_LEN)-1)<<ISP_AWB3_STAT_EN_POS)
#define ISP_AWB3_STAT_EN_UMSK                                   (~(((1U<<ISP_AWB3_STAT_EN_LEN)-1)<<ISP_AWB3_STAT_EN_POS))

/* 0xA04 : awb3_config_1 */
#define ISP_AWB3_CONFIG_1_OFFSET                                (0xA04)
#define ISP_AWB3_STAT_Y_MAX                                     ISP_AWB3_STAT_Y_MAX
#define ISP_AWB3_STAT_Y_MAX_POS                                 (0U)
#define ISP_AWB3_STAT_Y_MAX_LEN                                 (11U)
#define ISP_AWB3_STAT_Y_MAX_MSK                                 (((1U<<ISP_AWB3_STAT_Y_MAX_LEN)-1)<<ISP_AWB3_STAT_Y_MAX_POS)
#define ISP_AWB3_STAT_Y_MAX_UMSK                                (~(((1U<<ISP_AWB3_STAT_Y_MAX_LEN)-1)<<ISP_AWB3_STAT_Y_MAX_POS))
#define ISP_AWB3_STAT_Y_MIN                                     ISP_AWB3_STAT_Y_MIN
#define ISP_AWB3_STAT_Y_MIN_POS                                 (16U)
#define ISP_AWB3_STAT_Y_MIN_LEN                                 (11U)
#define ISP_AWB3_STAT_Y_MIN_MSK                                 (((1U<<ISP_AWB3_STAT_Y_MIN_LEN)-1)<<ISP_AWB3_STAT_Y_MIN_POS)
#define ISP_AWB3_STAT_Y_MIN_UMSK                                (~(((1U<<ISP_AWB3_STAT_Y_MIN_LEN)-1)<<ISP_AWB3_STAT_Y_MIN_POS))

/* 0xA08 : awb3_config_2 */
#define ISP_AWB3_CONFIG_2_OFFSET                                (0xA08)
#define ISP_AWB3_B_RATIO_OFST                                   ISP_AWB3_B_RATIO_OFST
#define ISP_AWB3_B_RATIO_OFST_POS                               (0U)
#define ISP_AWB3_B_RATIO_OFST_LEN                               (12U)
#define ISP_AWB3_B_RATIO_OFST_MSK                               (((1U<<ISP_AWB3_B_RATIO_OFST_LEN)-1)<<ISP_AWB3_B_RATIO_OFST_POS)
#define ISP_AWB3_B_RATIO_OFST_UMSK                              (~(((1U<<ISP_AWB3_B_RATIO_OFST_LEN)-1)<<ISP_AWB3_B_RATIO_OFST_POS))
#define ISP_AWB3_R_RATIO_OFST                                   ISP_AWB3_R_RATIO_OFST
#define ISP_AWB3_R_RATIO_OFST_POS                               (16U)
#define ISP_AWB3_R_RATIO_OFST_LEN                               (12U)
#define ISP_AWB3_R_RATIO_OFST_MSK                               (((1U<<ISP_AWB3_R_RATIO_OFST_LEN)-1)<<ISP_AWB3_R_RATIO_OFST_POS)
#define ISP_AWB3_R_RATIO_OFST_UMSK                              (~(((1U<<ISP_AWB3_R_RATIO_OFST_LEN)-1)<<ISP_AWB3_R_RATIO_OFST_POS))
#define ISP_AWB3_G_DIVISOR                                      ISP_AWB3_G_DIVISOR
#define ISP_AWB3_G_DIVISOR_POS                                  (31U)
#define ISP_AWB3_G_DIVISOR_LEN                                  (1U)
#define ISP_AWB3_G_DIVISOR_MSK                                  (((1U<<ISP_AWB3_G_DIVISOR_LEN)-1)<<ISP_AWB3_G_DIVISOR_POS)
#define ISP_AWB3_G_DIVISOR_UMSK                                 (~(((1U<<ISP_AWB3_G_DIVISOR_LEN)-1)<<ISP_AWB3_G_DIVISOR_POS))

/* 0xA0C : awb3_config_3 */
#define ISP_AWB3_CONFIG_3_OFFSET                                (0xA0C)
#define ISP_AWB3_B_RATIO_MULT                                   ISP_AWB3_B_RATIO_MULT
#define ISP_AWB3_B_RATIO_MULT_POS                               (0U)
#define ISP_AWB3_B_RATIO_MULT_LEN                               (12U)
#define ISP_AWB3_B_RATIO_MULT_MSK                               (((1U<<ISP_AWB3_B_RATIO_MULT_LEN)-1)<<ISP_AWB3_B_RATIO_MULT_POS)
#define ISP_AWB3_B_RATIO_MULT_UMSK                              (~(((1U<<ISP_AWB3_B_RATIO_MULT_LEN)-1)<<ISP_AWB3_B_RATIO_MULT_POS))
#define ISP_AWB3_R_RATIO_MULT                                   ISP_AWB3_R_RATIO_MULT
#define ISP_AWB3_R_RATIO_MULT_POS                               (16U)
#define ISP_AWB3_R_RATIO_MULT_LEN                               (12U)
#define ISP_AWB3_R_RATIO_MULT_MSK                               (((1U<<ISP_AWB3_R_RATIO_MULT_LEN)-1)<<ISP_AWB3_R_RATIO_MULT_POS)
#define ISP_AWB3_R_RATIO_MULT_UMSK                              (~(((1U<<ISP_AWB3_R_RATIO_MULT_LEN)-1)<<ISP_AWB3_R_RATIO_MULT_POS))

/* 0xA10 : awb3_config_4 */
#define ISP_AWB3_CONFIG_4_OFFSET                                (0xA10)
#define ISP_AWB3_RATIO_SUM_MAX                                  ISP_AWB3_RATIO_SUM_MAX
#define ISP_AWB3_RATIO_SUM_MAX_POS                              (0U)
#define ISP_AWB3_RATIO_SUM_MAX_LEN                              (9U)
#define ISP_AWB3_RATIO_SUM_MAX_MSK                              (((1U<<ISP_AWB3_RATIO_SUM_MAX_LEN)-1)<<ISP_AWB3_RATIO_SUM_MAX_POS)
#define ISP_AWB3_RATIO_SUM_MAX_UMSK                             (~(((1U<<ISP_AWB3_RATIO_SUM_MAX_LEN)-1)<<ISP_AWB3_RATIO_SUM_MAX_POS))
#define ISP_AWB3_RATIO_SUM_MIN                                  ISP_AWB3_RATIO_SUM_MIN
#define ISP_AWB3_RATIO_SUM_MIN_POS                              (9U)
#define ISP_AWB3_RATIO_SUM_MIN_LEN                              (9U)
#define ISP_AWB3_RATIO_SUM_MIN_MSK                              (((1U<<ISP_AWB3_RATIO_SUM_MIN_LEN)-1)<<ISP_AWB3_RATIO_SUM_MIN_POS)
#define ISP_AWB3_RATIO_SUM_MIN_UMSK                             (~(((1U<<ISP_AWB3_RATIO_SUM_MIN_LEN)-1)<<ISP_AWB3_RATIO_SUM_MIN_POS))
#define ISP_AWB3_WT_RATIO_GAIN                                  ISP_AWB3_WT_RATIO_GAIN
#define ISP_AWB3_WT_RATIO_GAIN_POS                              (18U)
#define ISP_AWB3_WT_RATIO_GAIN_LEN                              (8U)
#define ISP_AWB3_WT_RATIO_GAIN_MSK                              (((1U<<ISP_AWB3_WT_RATIO_GAIN_LEN)-1)<<ISP_AWB3_WT_RATIO_GAIN_POS)
#define ISP_AWB3_WT_RATIO_GAIN_UMSK                             (~(((1U<<ISP_AWB3_WT_RATIO_GAIN_LEN)-1)<<ISP_AWB3_WT_RATIO_GAIN_POS))
#define ISP_AWB3_WT_RATIO_EN                                    ISP_AWB3_WT_RATIO_EN
#define ISP_AWB3_WT_RATIO_EN_POS                                (26U)
#define ISP_AWB3_WT_RATIO_EN_LEN                                (1U)
#define ISP_AWB3_WT_RATIO_EN_MSK                                (((1U<<ISP_AWB3_WT_RATIO_EN_LEN)-1)<<ISP_AWB3_WT_RATIO_EN_POS)
#define ISP_AWB3_WT_RATIO_EN_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_EN_LEN)-1)<<ISP_AWB3_WT_RATIO_EN_POS))

/* 0xA14 : awb3_config_5 */
#define ISP_AWB3_CONFIG_5_OFFSET                                (0xA14)
#define ISP_AWB3_WT_RATIO_00                                    ISP_AWB3_WT_RATIO_00
#define ISP_AWB3_WT_RATIO_00_POS                                (0U)
#define ISP_AWB3_WT_RATIO_00_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_00_MSK                                (((1U<<ISP_AWB3_WT_RATIO_00_LEN)-1)<<ISP_AWB3_WT_RATIO_00_POS)
#define ISP_AWB3_WT_RATIO_00_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_00_LEN)-1)<<ISP_AWB3_WT_RATIO_00_POS))
#define ISP_AWB3_WT_RATIO_01                                    ISP_AWB3_WT_RATIO_01
#define ISP_AWB3_WT_RATIO_01_POS                                (8U)
#define ISP_AWB3_WT_RATIO_01_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_01_MSK                                (((1U<<ISP_AWB3_WT_RATIO_01_LEN)-1)<<ISP_AWB3_WT_RATIO_01_POS)
#define ISP_AWB3_WT_RATIO_01_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_01_LEN)-1)<<ISP_AWB3_WT_RATIO_01_POS))
#define ISP_AWB3_WT_RATIO_02                                    ISP_AWB3_WT_RATIO_02
#define ISP_AWB3_WT_RATIO_02_POS                                (16U)
#define ISP_AWB3_WT_RATIO_02_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_02_MSK                                (((1U<<ISP_AWB3_WT_RATIO_02_LEN)-1)<<ISP_AWB3_WT_RATIO_02_POS)
#define ISP_AWB3_WT_RATIO_02_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_02_LEN)-1)<<ISP_AWB3_WT_RATIO_02_POS))
#define ISP_AWB3_WT_RATIO_03                                    ISP_AWB3_WT_RATIO_03
#define ISP_AWB3_WT_RATIO_03_POS                                (24U)
#define ISP_AWB3_WT_RATIO_03_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_03_MSK                                (((1U<<ISP_AWB3_WT_RATIO_03_LEN)-1)<<ISP_AWB3_WT_RATIO_03_POS)
#define ISP_AWB3_WT_RATIO_03_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_03_LEN)-1)<<ISP_AWB3_WT_RATIO_03_POS))

/* 0xA18 : awb3_config_6 */
#define ISP_AWB3_CONFIG_6_OFFSET                                (0xA18)
#define ISP_AWB3_WT_RATIO_04                                    ISP_AWB3_WT_RATIO_04
#define ISP_AWB3_WT_RATIO_04_POS                                (0U)
#define ISP_AWB3_WT_RATIO_04_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_04_MSK                                (((1U<<ISP_AWB3_WT_RATIO_04_LEN)-1)<<ISP_AWB3_WT_RATIO_04_POS)
#define ISP_AWB3_WT_RATIO_04_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_04_LEN)-1)<<ISP_AWB3_WT_RATIO_04_POS))
#define ISP_AWB3_WT_RATIO_05                                    ISP_AWB3_WT_RATIO_05
#define ISP_AWB3_WT_RATIO_05_POS                                (8U)
#define ISP_AWB3_WT_RATIO_05_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_05_MSK                                (((1U<<ISP_AWB3_WT_RATIO_05_LEN)-1)<<ISP_AWB3_WT_RATIO_05_POS)
#define ISP_AWB3_WT_RATIO_05_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_05_LEN)-1)<<ISP_AWB3_WT_RATIO_05_POS))
#define ISP_AWB3_WT_RATIO_06                                    ISP_AWB3_WT_RATIO_06
#define ISP_AWB3_WT_RATIO_06_POS                                (16U)
#define ISP_AWB3_WT_RATIO_06_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_06_MSK                                (((1U<<ISP_AWB3_WT_RATIO_06_LEN)-1)<<ISP_AWB3_WT_RATIO_06_POS)
#define ISP_AWB3_WT_RATIO_06_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_06_LEN)-1)<<ISP_AWB3_WT_RATIO_06_POS))
#define ISP_AWB3_WT_RATIO_07                                    ISP_AWB3_WT_RATIO_07
#define ISP_AWB3_WT_RATIO_07_POS                                (24U)
#define ISP_AWB3_WT_RATIO_07_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_07_MSK                                (((1U<<ISP_AWB3_WT_RATIO_07_LEN)-1)<<ISP_AWB3_WT_RATIO_07_POS)
#define ISP_AWB3_WT_RATIO_07_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_07_LEN)-1)<<ISP_AWB3_WT_RATIO_07_POS))

/* 0xA1C : awb3_config_7 */
#define ISP_AWB3_CONFIG_7_OFFSET                                (0xA1C)
#define ISP_AWB3_WT_RATIO_08                                    ISP_AWB3_WT_RATIO_08
#define ISP_AWB3_WT_RATIO_08_POS                                (0U)
#define ISP_AWB3_WT_RATIO_08_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_08_MSK                                (((1U<<ISP_AWB3_WT_RATIO_08_LEN)-1)<<ISP_AWB3_WT_RATIO_08_POS)
#define ISP_AWB3_WT_RATIO_08_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_08_LEN)-1)<<ISP_AWB3_WT_RATIO_08_POS))
#define ISP_AWB3_WT_RATIO_10                                    ISP_AWB3_WT_RATIO_10
#define ISP_AWB3_WT_RATIO_10_POS                                (8U)
#define ISP_AWB3_WT_RATIO_10_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_10_MSK                                (((1U<<ISP_AWB3_WT_RATIO_10_LEN)-1)<<ISP_AWB3_WT_RATIO_10_POS)
#define ISP_AWB3_WT_RATIO_10_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_10_LEN)-1)<<ISP_AWB3_WT_RATIO_10_POS))
#define ISP_AWB3_WT_RATIO_11                                    ISP_AWB3_WT_RATIO_11
#define ISP_AWB3_WT_RATIO_11_POS                                (16U)
#define ISP_AWB3_WT_RATIO_11_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_11_MSK                                (((1U<<ISP_AWB3_WT_RATIO_11_LEN)-1)<<ISP_AWB3_WT_RATIO_11_POS)
#define ISP_AWB3_WT_RATIO_11_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_11_LEN)-1)<<ISP_AWB3_WT_RATIO_11_POS))
#define ISP_AWB3_WT_RATIO_12                                    ISP_AWB3_WT_RATIO_12
#define ISP_AWB3_WT_RATIO_12_POS                                (24U)
#define ISP_AWB3_WT_RATIO_12_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_12_MSK                                (((1U<<ISP_AWB3_WT_RATIO_12_LEN)-1)<<ISP_AWB3_WT_RATIO_12_POS)
#define ISP_AWB3_WT_RATIO_12_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_12_LEN)-1)<<ISP_AWB3_WT_RATIO_12_POS))

/* 0xA20 : awb3_config_8 */
#define ISP_AWB3_CONFIG_8_OFFSET                                (0xA20)
#define ISP_AWB3_WT_RATIO_13                                    ISP_AWB3_WT_RATIO_13
#define ISP_AWB3_WT_RATIO_13_POS                                (0U)
#define ISP_AWB3_WT_RATIO_13_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_13_MSK                                (((1U<<ISP_AWB3_WT_RATIO_13_LEN)-1)<<ISP_AWB3_WT_RATIO_13_POS)
#define ISP_AWB3_WT_RATIO_13_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_13_LEN)-1)<<ISP_AWB3_WT_RATIO_13_POS))
#define ISP_AWB3_WT_RATIO_14                                    ISP_AWB3_WT_RATIO_14
#define ISP_AWB3_WT_RATIO_14_POS                                (8U)
#define ISP_AWB3_WT_RATIO_14_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_14_MSK                                (((1U<<ISP_AWB3_WT_RATIO_14_LEN)-1)<<ISP_AWB3_WT_RATIO_14_POS)
#define ISP_AWB3_WT_RATIO_14_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_14_LEN)-1)<<ISP_AWB3_WT_RATIO_14_POS))
#define ISP_AWB3_WT_RATIO_15                                    ISP_AWB3_WT_RATIO_15
#define ISP_AWB3_WT_RATIO_15_POS                                (16U)
#define ISP_AWB3_WT_RATIO_15_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_15_MSK                                (((1U<<ISP_AWB3_WT_RATIO_15_LEN)-1)<<ISP_AWB3_WT_RATIO_15_POS)
#define ISP_AWB3_WT_RATIO_15_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_15_LEN)-1)<<ISP_AWB3_WT_RATIO_15_POS))
#define ISP_AWB3_WT_RATIO_16                                    ISP_AWB3_WT_RATIO_16
#define ISP_AWB3_WT_RATIO_16_POS                                (24U)
#define ISP_AWB3_WT_RATIO_16_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_16_MSK                                (((1U<<ISP_AWB3_WT_RATIO_16_LEN)-1)<<ISP_AWB3_WT_RATIO_16_POS)
#define ISP_AWB3_WT_RATIO_16_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_16_LEN)-1)<<ISP_AWB3_WT_RATIO_16_POS))

/* 0xA24 : awb3_config_9 */
#define ISP_AWB3_CONFIG_9_OFFSET                                (0xA24)
#define ISP_AWB3_WT_RATIO_17                                    ISP_AWB3_WT_RATIO_17
#define ISP_AWB3_WT_RATIO_17_POS                                (0U)
#define ISP_AWB3_WT_RATIO_17_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_17_MSK                                (((1U<<ISP_AWB3_WT_RATIO_17_LEN)-1)<<ISP_AWB3_WT_RATIO_17_POS)
#define ISP_AWB3_WT_RATIO_17_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_17_LEN)-1)<<ISP_AWB3_WT_RATIO_17_POS))
#define ISP_AWB3_WT_RATIO_18                                    ISP_AWB3_WT_RATIO_18
#define ISP_AWB3_WT_RATIO_18_POS                                (8U)
#define ISP_AWB3_WT_RATIO_18_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_18_MSK                                (((1U<<ISP_AWB3_WT_RATIO_18_LEN)-1)<<ISP_AWB3_WT_RATIO_18_POS)
#define ISP_AWB3_WT_RATIO_18_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_18_LEN)-1)<<ISP_AWB3_WT_RATIO_18_POS))
#define ISP_AWB3_WT_RATIO_20                                    ISP_AWB3_WT_RATIO_20
#define ISP_AWB3_WT_RATIO_20_POS                                (16U)
#define ISP_AWB3_WT_RATIO_20_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_20_MSK                                (((1U<<ISP_AWB3_WT_RATIO_20_LEN)-1)<<ISP_AWB3_WT_RATIO_20_POS)
#define ISP_AWB3_WT_RATIO_20_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_20_LEN)-1)<<ISP_AWB3_WT_RATIO_20_POS))
#define ISP_AWB3_WT_RATIO_21                                    ISP_AWB3_WT_RATIO_21
#define ISP_AWB3_WT_RATIO_21_POS                                (24U)
#define ISP_AWB3_WT_RATIO_21_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_21_MSK                                (((1U<<ISP_AWB3_WT_RATIO_21_LEN)-1)<<ISP_AWB3_WT_RATIO_21_POS)
#define ISP_AWB3_WT_RATIO_21_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_21_LEN)-1)<<ISP_AWB3_WT_RATIO_21_POS))

/* 0xA28 : awb3_config_10 */
#define ISP_AWB3_CONFIG_10_OFFSET                               (0xA28)
#define ISP_AWB3_WT_RATIO_22                                    ISP_AWB3_WT_RATIO_22
#define ISP_AWB3_WT_RATIO_22_POS                                (0U)
#define ISP_AWB3_WT_RATIO_22_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_22_MSK                                (((1U<<ISP_AWB3_WT_RATIO_22_LEN)-1)<<ISP_AWB3_WT_RATIO_22_POS)
#define ISP_AWB3_WT_RATIO_22_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_22_LEN)-1)<<ISP_AWB3_WT_RATIO_22_POS))
#define ISP_AWB3_WT_RATIO_23                                    ISP_AWB3_WT_RATIO_23
#define ISP_AWB3_WT_RATIO_23_POS                                (8U)
#define ISP_AWB3_WT_RATIO_23_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_23_MSK                                (((1U<<ISP_AWB3_WT_RATIO_23_LEN)-1)<<ISP_AWB3_WT_RATIO_23_POS)
#define ISP_AWB3_WT_RATIO_23_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_23_LEN)-1)<<ISP_AWB3_WT_RATIO_23_POS))
#define ISP_AWB3_WT_RATIO_24                                    ISP_AWB3_WT_RATIO_24
#define ISP_AWB3_WT_RATIO_24_POS                                (16U)
#define ISP_AWB3_WT_RATIO_24_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_24_MSK                                (((1U<<ISP_AWB3_WT_RATIO_24_LEN)-1)<<ISP_AWB3_WT_RATIO_24_POS)
#define ISP_AWB3_WT_RATIO_24_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_24_LEN)-1)<<ISP_AWB3_WT_RATIO_24_POS))
#define ISP_AWB3_WT_RATIO_25                                    ISP_AWB3_WT_RATIO_25
#define ISP_AWB3_WT_RATIO_25_POS                                (24U)
#define ISP_AWB3_WT_RATIO_25_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_25_MSK                                (((1U<<ISP_AWB3_WT_RATIO_25_LEN)-1)<<ISP_AWB3_WT_RATIO_25_POS)
#define ISP_AWB3_WT_RATIO_25_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_25_LEN)-1)<<ISP_AWB3_WT_RATIO_25_POS))

/* 0xA2C : awb3_config_11 */
#define ISP_AWB3_CONFIG_11_OFFSET                               (0xA2C)
#define ISP_AWB3_WT_RATIO_26                                    ISP_AWB3_WT_RATIO_26
#define ISP_AWB3_WT_RATIO_26_POS                                (0U)
#define ISP_AWB3_WT_RATIO_26_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_26_MSK                                (((1U<<ISP_AWB3_WT_RATIO_26_LEN)-1)<<ISP_AWB3_WT_RATIO_26_POS)
#define ISP_AWB3_WT_RATIO_26_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_26_LEN)-1)<<ISP_AWB3_WT_RATIO_26_POS))
#define ISP_AWB3_WT_RATIO_27                                    ISP_AWB3_WT_RATIO_27
#define ISP_AWB3_WT_RATIO_27_POS                                (8U)
#define ISP_AWB3_WT_RATIO_27_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_27_MSK                                (((1U<<ISP_AWB3_WT_RATIO_27_LEN)-1)<<ISP_AWB3_WT_RATIO_27_POS)
#define ISP_AWB3_WT_RATIO_27_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_27_LEN)-1)<<ISP_AWB3_WT_RATIO_27_POS))
#define ISP_AWB3_WT_RATIO_28                                    ISP_AWB3_WT_RATIO_28
#define ISP_AWB3_WT_RATIO_28_POS                                (16U)
#define ISP_AWB3_WT_RATIO_28_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_28_MSK                                (((1U<<ISP_AWB3_WT_RATIO_28_LEN)-1)<<ISP_AWB3_WT_RATIO_28_POS)
#define ISP_AWB3_WT_RATIO_28_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_28_LEN)-1)<<ISP_AWB3_WT_RATIO_28_POS))
#define ISP_AWB3_WT_RATIO_30                                    ISP_AWB3_WT_RATIO_30
#define ISP_AWB3_WT_RATIO_30_POS                                (24U)
#define ISP_AWB3_WT_RATIO_30_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_30_MSK                                (((1U<<ISP_AWB3_WT_RATIO_30_LEN)-1)<<ISP_AWB3_WT_RATIO_30_POS)
#define ISP_AWB3_WT_RATIO_30_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_30_LEN)-1)<<ISP_AWB3_WT_RATIO_30_POS))

/* 0xA30 : awb3_config_12 */
#define ISP_AWB3_CONFIG_12_OFFSET                               (0xA30)
#define ISP_AWB3_WT_RATIO_31                                    ISP_AWB3_WT_RATIO_31
#define ISP_AWB3_WT_RATIO_31_POS                                (0U)
#define ISP_AWB3_WT_RATIO_31_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_31_MSK                                (((1U<<ISP_AWB3_WT_RATIO_31_LEN)-1)<<ISP_AWB3_WT_RATIO_31_POS)
#define ISP_AWB3_WT_RATIO_31_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_31_LEN)-1)<<ISP_AWB3_WT_RATIO_31_POS))
#define ISP_AWB3_WT_RATIO_32                                    ISP_AWB3_WT_RATIO_32
#define ISP_AWB3_WT_RATIO_32_POS                                (8U)
#define ISP_AWB3_WT_RATIO_32_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_32_MSK                                (((1U<<ISP_AWB3_WT_RATIO_32_LEN)-1)<<ISP_AWB3_WT_RATIO_32_POS)
#define ISP_AWB3_WT_RATIO_32_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_32_LEN)-1)<<ISP_AWB3_WT_RATIO_32_POS))
#define ISP_AWB3_WT_RATIO_33                                    ISP_AWB3_WT_RATIO_33
#define ISP_AWB3_WT_RATIO_33_POS                                (16U)
#define ISP_AWB3_WT_RATIO_33_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_33_MSK                                (((1U<<ISP_AWB3_WT_RATIO_33_LEN)-1)<<ISP_AWB3_WT_RATIO_33_POS)
#define ISP_AWB3_WT_RATIO_33_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_33_LEN)-1)<<ISP_AWB3_WT_RATIO_33_POS))
#define ISP_AWB3_WT_RATIO_34                                    ISP_AWB3_WT_RATIO_34
#define ISP_AWB3_WT_RATIO_34_POS                                (24U)
#define ISP_AWB3_WT_RATIO_34_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_34_MSK                                (((1U<<ISP_AWB3_WT_RATIO_34_LEN)-1)<<ISP_AWB3_WT_RATIO_34_POS)
#define ISP_AWB3_WT_RATIO_34_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_34_LEN)-1)<<ISP_AWB3_WT_RATIO_34_POS))

/* 0xA34 : awb3_config_13 */
#define ISP_AWB3_CONFIG_13_OFFSET                               (0xA34)
#define ISP_AWB3_WT_RATIO_35                                    ISP_AWB3_WT_RATIO_35
#define ISP_AWB3_WT_RATIO_35_POS                                (0U)
#define ISP_AWB3_WT_RATIO_35_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_35_MSK                                (((1U<<ISP_AWB3_WT_RATIO_35_LEN)-1)<<ISP_AWB3_WT_RATIO_35_POS)
#define ISP_AWB3_WT_RATIO_35_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_35_LEN)-1)<<ISP_AWB3_WT_RATIO_35_POS))
#define ISP_AWB3_WT_RATIO_36                                    ISP_AWB3_WT_RATIO_36
#define ISP_AWB3_WT_RATIO_36_POS                                (8U)
#define ISP_AWB3_WT_RATIO_36_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_36_MSK                                (((1U<<ISP_AWB3_WT_RATIO_36_LEN)-1)<<ISP_AWB3_WT_RATIO_36_POS)
#define ISP_AWB3_WT_RATIO_36_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_36_LEN)-1)<<ISP_AWB3_WT_RATIO_36_POS))
#define ISP_AWB3_WT_RATIO_37                                    ISP_AWB3_WT_RATIO_37
#define ISP_AWB3_WT_RATIO_37_POS                                (16U)
#define ISP_AWB3_WT_RATIO_37_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_37_MSK                                (((1U<<ISP_AWB3_WT_RATIO_37_LEN)-1)<<ISP_AWB3_WT_RATIO_37_POS)
#define ISP_AWB3_WT_RATIO_37_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_37_LEN)-1)<<ISP_AWB3_WT_RATIO_37_POS))
#define ISP_AWB3_WT_RATIO_38                                    ISP_AWB3_WT_RATIO_38
#define ISP_AWB3_WT_RATIO_38_POS                                (24U)
#define ISP_AWB3_WT_RATIO_38_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_38_MSK                                (((1U<<ISP_AWB3_WT_RATIO_38_LEN)-1)<<ISP_AWB3_WT_RATIO_38_POS)
#define ISP_AWB3_WT_RATIO_38_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_38_LEN)-1)<<ISP_AWB3_WT_RATIO_38_POS))

/* 0xA38 : awb3_config_14 */
#define ISP_AWB3_CONFIG_14_OFFSET                               (0xA38)
#define ISP_AWB3_WT_RATIO_40                                    ISP_AWB3_WT_RATIO_40
#define ISP_AWB3_WT_RATIO_40_POS                                (0U)
#define ISP_AWB3_WT_RATIO_40_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_40_MSK                                (((1U<<ISP_AWB3_WT_RATIO_40_LEN)-1)<<ISP_AWB3_WT_RATIO_40_POS)
#define ISP_AWB3_WT_RATIO_40_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_40_LEN)-1)<<ISP_AWB3_WT_RATIO_40_POS))
#define ISP_AWB3_WT_RATIO_41                                    ISP_AWB3_WT_RATIO_41
#define ISP_AWB3_WT_RATIO_41_POS                                (8U)
#define ISP_AWB3_WT_RATIO_41_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_41_MSK                                (((1U<<ISP_AWB3_WT_RATIO_41_LEN)-1)<<ISP_AWB3_WT_RATIO_41_POS)
#define ISP_AWB3_WT_RATIO_41_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_41_LEN)-1)<<ISP_AWB3_WT_RATIO_41_POS))
#define ISP_AWB3_WT_RATIO_42                                    ISP_AWB3_WT_RATIO_42
#define ISP_AWB3_WT_RATIO_42_POS                                (16U)
#define ISP_AWB3_WT_RATIO_42_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_42_MSK                                (((1U<<ISP_AWB3_WT_RATIO_42_LEN)-1)<<ISP_AWB3_WT_RATIO_42_POS)
#define ISP_AWB3_WT_RATIO_42_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_42_LEN)-1)<<ISP_AWB3_WT_RATIO_42_POS))
#define ISP_AWB3_WT_RATIO_43                                    ISP_AWB3_WT_RATIO_43
#define ISP_AWB3_WT_RATIO_43_POS                                (24U)
#define ISP_AWB3_WT_RATIO_43_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_43_MSK                                (((1U<<ISP_AWB3_WT_RATIO_43_LEN)-1)<<ISP_AWB3_WT_RATIO_43_POS)
#define ISP_AWB3_WT_RATIO_43_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_43_LEN)-1)<<ISP_AWB3_WT_RATIO_43_POS))

/* 0xA3C : awb3_config_15 */
#define ISP_AWB3_CONFIG_15_OFFSET                               (0xA3C)
#define ISP_AWB3_WT_RATIO_44                                    ISP_AWB3_WT_RATIO_44
#define ISP_AWB3_WT_RATIO_44_POS                                (0U)
#define ISP_AWB3_WT_RATIO_44_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_44_MSK                                (((1U<<ISP_AWB3_WT_RATIO_44_LEN)-1)<<ISP_AWB3_WT_RATIO_44_POS)
#define ISP_AWB3_WT_RATIO_44_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_44_LEN)-1)<<ISP_AWB3_WT_RATIO_44_POS))
#define ISP_AWB3_WT_RATIO_45                                    ISP_AWB3_WT_RATIO_45
#define ISP_AWB3_WT_RATIO_45_POS                                (8U)
#define ISP_AWB3_WT_RATIO_45_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_45_MSK                                (((1U<<ISP_AWB3_WT_RATIO_45_LEN)-1)<<ISP_AWB3_WT_RATIO_45_POS)
#define ISP_AWB3_WT_RATIO_45_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_45_LEN)-1)<<ISP_AWB3_WT_RATIO_45_POS))
#define ISP_AWB3_WT_RATIO_46                                    ISP_AWB3_WT_RATIO_46
#define ISP_AWB3_WT_RATIO_46_POS                                (16U)
#define ISP_AWB3_WT_RATIO_46_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_46_MSK                                (((1U<<ISP_AWB3_WT_RATIO_46_LEN)-1)<<ISP_AWB3_WT_RATIO_46_POS)
#define ISP_AWB3_WT_RATIO_46_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_46_LEN)-1)<<ISP_AWB3_WT_RATIO_46_POS))
#define ISP_AWB3_WT_RATIO_47                                    ISP_AWB3_WT_RATIO_47
#define ISP_AWB3_WT_RATIO_47_POS                                (24U)
#define ISP_AWB3_WT_RATIO_47_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_47_MSK                                (((1U<<ISP_AWB3_WT_RATIO_47_LEN)-1)<<ISP_AWB3_WT_RATIO_47_POS)
#define ISP_AWB3_WT_RATIO_47_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_47_LEN)-1)<<ISP_AWB3_WT_RATIO_47_POS))

/* 0xA40 : awb3_config_16 */
#define ISP_AWB3_CONFIG_16_OFFSET                               (0xA40)
#define ISP_AWB3_WT_RATIO_48                                    ISP_AWB3_WT_RATIO_48
#define ISP_AWB3_WT_RATIO_48_POS                                (0U)
#define ISP_AWB3_WT_RATIO_48_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_48_MSK                                (((1U<<ISP_AWB3_WT_RATIO_48_LEN)-1)<<ISP_AWB3_WT_RATIO_48_POS)
#define ISP_AWB3_WT_RATIO_48_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_48_LEN)-1)<<ISP_AWB3_WT_RATIO_48_POS))
#define ISP_AWB3_WT_RATIO_50                                    ISP_AWB3_WT_RATIO_50
#define ISP_AWB3_WT_RATIO_50_POS                                (8U)
#define ISP_AWB3_WT_RATIO_50_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_50_MSK                                (((1U<<ISP_AWB3_WT_RATIO_50_LEN)-1)<<ISP_AWB3_WT_RATIO_50_POS)
#define ISP_AWB3_WT_RATIO_50_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_50_LEN)-1)<<ISP_AWB3_WT_RATIO_50_POS))
#define ISP_AWB3_WT_RATIO_51                                    ISP_AWB3_WT_RATIO_51
#define ISP_AWB3_WT_RATIO_51_POS                                (16U)
#define ISP_AWB3_WT_RATIO_51_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_51_MSK                                (((1U<<ISP_AWB3_WT_RATIO_51_LEN)-1)<<ISP_AWB3_WT_RATIO_51_POS)
#define ISP_AWB3_WT_RATIO_51_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_51_LEN)-1)<<ISP_AWB3_WT_RATIO_51_POS))
#define ISP_AWB3_WT_RATIO_52                                    ISP_AWB3_WT_RATIO_52
#define ISP_AWB3_WT_RATIO_52_POS                                (24U)
#define ISP_AWB3_WT_RATIO_52_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_52_MSK                                (((1U<<ISP_AWB3_WT_RATIO_52_LEN)-1)<<ISP_AWB3_WT_RATIO_52_POS)
#define ISP_AWB3_WT_RATIO_52_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_52_LEN)-1)<<ISP_AWB3_WT_RATIO_52_POS))

/* 0xA44 : awb3_config_17 */
#define ISP_AWB3_CONFIG_17_OFFSET                               (0xA44)
#define ISP_AWB3_WT_RATIO_53                                    ISP_AWB3_WT_RATIO_53
#define ISP_AWB3_WT_RATIO_53_POS                                (0U)
#define ISP_AWB3_WT_RATIO_53_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_53_MSK                                (((1U<<ISP_AWB3_WT_RATIO_53_LEN)-1)<<ISP_AWB3_WT_RATIO_53_POS)
#define ISP_AWB3_WT_RATIO_53_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_53_LEN)-1)<<ISP_AWB3_WT_RATIO_53_POS))
#define ISP_AWB3_WT_RATIO_54                                    ISP_AWB3_WT_RATIO_54
#define ISP_AWB3_WT_RATIO_54_POS                                (8U)
#define ISP_AWB3_WT_RATIO_54_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_54_MSK                                (((1U<<ISP_AWB3_WT_RATIO_54_LEN)-1)<<ISP_AWB3_WT_RATIO_54_POS)
#define ISP_AWB3_WT_RATIO_54_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_54_LEN)-1)<<ISP_AWB3_WT_RATIO_54_POS))
#define ISP_AWB3_WT_RATIO_55                                    ISP_AWB3_WT_RATIO_55
#define ISP_AWB3_WT_RATIO_55_POS                                (16U)
#define ISP_AWB3_WT_RATIO_55_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_55_MSK                                (((1U<<ISP_AWB3_WT_RATIO_55_LEN)-1)<<ISP_AWB3_WT_RATIO_55_POS)
#define ISP_AWB3_WT_RATIO_55_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_55_LEN)-1)<<ISP_AWB3_WT_RATIO_55_POS))
#define ISP_AWB3_WT_RATIO_56                                    ISP_AWB3_WT_RATIO_56
#define ISP_AWB3_WT_RATIO_56_POS                                (24U)
#define ISP_AWB3_WT_RATIO_56_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_56_MSK                                (((1U<<ISP_AWB3_WT_RATIO_56_LEN)-1)<<ISP_AWB3_WT_RATIO_56_POS)
#define ISP_AWB3_WT_RATIO_56_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_56_LEN)-1)<<ISP_AWB3_WT_RATIO_56_POS))

/* 0xA48 : awb3_config_18 */
#define ISP_AWB3_CONFIG_18_OFFSET                               (0xA48)
#define ISP_AWB3_WT_RATIO_57                                    ISP_AWB3_WT_RATIO_57
#define ISP_AWB3_WT_RATIO_57_POS                                (0U)
#define ISP_AWB3_WT_RATIO_57_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_57_MSK                                (((1U<<ISP_AWB3_WT_RATIO_57_LEN)-1)<<ISP_AWB3_WT_RATIO_57_POS)
#define ISP_AWB3_WT_RATIO_57_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_57_LEN)-1)<<ISP_AWB3_WT_RATIO_57_POS))
#define ISP_AWB3_WT_RATIO_58                                    ISP_AWB3_WT_RATIO_58
#define ISP_AWB3_WT_RATIO_58_POS                                (8U)
#define ISP_AWB3_WT_RATIO_58_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_58_MSK                                (((1U<<ISP_AWB3_WT_RATIO_58_LEN)-1)<<ISP_AWB3_WT_RATIO_58_POS)
#define ISP_AWB3_WT_RATIO_58_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_58_LEN)-1)<<ISP_AWB3_WT_RATIO_58_POS))
#define ISP_AWB3_WT_RATIO_60                                    ISP_AWB3_WT_RATIO_60
#define ISP_AWB3_WT_RATIO_60_POS                                (16U)
#define ISP_AWB3_WT_RATIO_60_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_60_MSK                                (((1U<<ISP_AWB3_WT_RATIO_60_LEN)-1)<<ISP_AWB3_WT_RATIO_60_POS)
#define ISP_AWB3_WT_RATIO_60_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_60_LEN)-1)<<ISP_AWB3_WT_RATIO_60_POS))
#define ISP_AWB3_WT_RATIO_61                                    ISP_AWB3_WT_RATIO_61
#define ISP_AWB3_WT_RATIO_61_POS                                (24U)
#define ISP_AWB3_WT_RATIO_61_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_61_MSK                                (((1U<<ISP_AWB3_WT_RATIO_61_LEN)-1)<<ISP_AWB3_WT_RATIO_61_POS)
#define ISP_AWB3_WT_RATIO_61_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_61_LEN)-1)<<ISP_AWB3_WT_RATIO_61_POS))

/* 0xA4C : awb3_config_19 */
#define ISP_AWB3_CONFIG_19_OFFSET                               (0xA4C)
#define ISP_AWB3_WT_RATIO_62                                    ISP_AWB3_WT_RATIO_62
#define ISP_AWB3_WT_RATIO_62_POS                                (0U)
#define ISP_AWB3_WT_RATIO_62_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_62_MSK                                (((1U<<ISP_AWB3_WT_RATIO_62_LEN)-1)<<ISP_AWB3_WT_RATIO_62_POS)
#define ISP_AWB3_WT_RATIO_62_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_62_LEN)-1)<<ISP_AWB3_WT_RATIO_62_POS))
#define ISP_AWB3_WT_RATIO_63                                    ISP_AWB3_WT_RATIO_63
#define ISP_AWB3_WT_RATIO_63_POS                                (8U)
#define ISP_AWB3_WT_RATIO_63_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_63_MSK                                (((1U<<ISP_AWB3_WT_RATIO_63_LEN)-1)<<ISP_AWB3_WT_RATIO_63_POS)
#define ISP_AWB3_WT_RATIO_63_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_63_LEN)-1)<<ISP_AWB3_WT_RATIO_63_POS))
#define ISP_AWB3_WT_RATIO_64                                    ISP_AWB3_WT_RATIO_64
#define ISP_AWB3_WT_RATIO_64_POS                                (16U)
#define ISP_AWB3_WT_RATIO_64_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_64_MSK                                (((1U<<ISP_AWB3_WT_RATIO_64_LEN)-1)<<ISP_AWB3_WT_RATIO_64_POS)
#define ISP_AWB3_WT_RATIO_64_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_64_LEN)-1)<<ISP_AWB3_WT_RATIO_64_POS))
#define ISP_AWB3_WT_RATIO_65                                    ISP_AWB3_WT_RATIO_65
#define ISP_AWB3_WT_RATIO_65_POS                                (24U)
#define ISP_AWB3_WT_RATIO_65_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_65_MSK                                (((1U<<ISP_AWB3_WT_RATIO_65_LEN)-1)<<ISP_AWB3_WT_RATIO_65_POS)
#define ISP_AWB3_WT_RATIO_65_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_65_LEN)-1)<<ISP_AWB3_WT_RATIO_65_POS))

/* 0xA50 : awb3_config_20 */
#define ISP_AWB3_CONFIG_20_OFFSET                               (0xA50)
#define ISP_AWB3_WT_RATIO_66                                    ISP_AWB3_WT_RATIO_66
#define ISP_AWB3_WT_RATIO_66_POS                                (0U)
#define ISP_AWB3_WT_RATIO_66_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_66_MSK                                (((1U<<ISP_AWB3_WT_RATIO_66_LEN)-1)<<ISP_AWB3_WT_RATIO_66_POS)
#define ISP_AWB3_WT_RATIO_66_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_66_LEN)-1)<<ISP_AWB3_WT_RATIO_66_POS))
#define ISP_AWB3_WT_RATIO_67                                    ISP_AWB3_WT_RATIO_67
#define ISP_AWB3_WT_RATIO_67_POS                                (8U)
#define ISP_AWB3_WT_RATIO_67_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_67_MSK                                (((1U<<ISP_AWB3_WT_RATIO_67_LEN)-1)<<ISP_AWB3_WT_RATIO_67_POS)
#define ISP_AWB3_WT_RATIO_67_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_67_LEN)-1)<<ISP_AWB3_WT_RATIO_67_POS))
#define ISP_AWB3_WT_RATIO_68                                    ISP_AWB3_WT_RATIO_68
#define ISP_AWB3_WT_RATIO_68_POS                                (16U)
#define ISP_AWB3_WT_RATIO_68_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_68_MSK                                (((1U<<ISP_AWB3_WT_RATIO_68_LEN)-1)<<ISP_AWB3_WT_RATIO_68_POS)
#define ISP_AWB3_WT_RATIO_68_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_68_LEN)-1)<<ISP_AWB3_WT_RATIO_68_POS))
#define ISP_AWB3_WT_RATIO_70                                    ISP_AWB3_WT_RATIO_70
#define ISP_AWB3_WT_RATIO_70_POS                                (24U)
#define ISP_AWB3_WT_RATIO_70_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_70_MSK                                (((1U<<ISP_AWB3_WT_RATIO_70_LEN)-1)<<ISP_AWB3_WT_RATIO_70_POS)
#define ISP_AWB3_WT_RATIO_70_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_70_LEN)-1)<<ISP_AWB3_WT_RATIO_70_POS))

/* 0xA54 : awb3_config_21 */
#define ISP_AWB3_CONFIG_21_OFFSET                               (0xA54)
#define ISP_AWB3_WT_RATIO_71                                    ISP_AWB3_WT_RATIO_71
#define ISP_AWB3_WT_RATIO_71_POS                                (0U)
#define ISP_AWB3_WT_RATIO_71_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_71_MSK                                (((1U<<ISP_AWB3_WT_RATIO_71_LEN)-1)<<ISP_AWB3_WT_RATIO_71_POS)
#define ISP_AWB3_WT_RATIO_71_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_71_LEN)-1)<<ISP_AWB3_WT_RATIO_71_POS))
#define ISP_AWB3_WT_RATIO_72                                    ISP_AWB3_WT_RATIO_72
#define ISP_AWB3_WT_RATIO_72_POS                                (8U)
#define ISP_AWB3_WT_RATIO_72_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_72_MSK                                (((1U<<ISP_AWB3_WT_RATIO_72_LEN)-1)<<ISP_AWB3_WT_RATIO_72_POS)
#define ISP_AWB3_WT_RATIO_72_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_72_LEN)-1)<<ISP_AWB3_WT_RATIO_72_POS))
#define ISP_AWB3_WT_RATIO_73                                    ISP_AWB3_WT_RATIO_73
#define ISP_AWB3_WT_RATIO_73_POS                                (16U)
#define ISP_AWB3_WT_RATIO_73_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_73_MSK                                (((1U<<ISP_AWB3_WT_RATIO_73_LEN)-1)<<ISP_AWB3_WT_RATIO_73_POS)
#define ISP_AWB3_WT_RATIO_73_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_73_LEN)-1)<<ISP_AWB3_WT_RATIO_73_POS))
#define ISP_AWB3_WT_RATIO_74                                    ISP_AWB3_WT_RATIO_74
#define ISP_AWB3_WT_RATIO_74_POS                                (24U)
#define ISP_AWB3_WT_RATIO_74_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_74_MSK                                (((1U<<ISP_AWB3_WT_RATIO_74_LEN)-1)<<ISP_AWB3_WT_RATIO_74_POS)
#define ISP_AWB3_WT_RATIO_74_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_74_LEN)-1)<<ISP_AWB3_WT_RATIO_74_POS))

/* 0xA58 : awb3_config_22 */
#define ISP_AWB3_CONFIG_22_OFFSET                               (0xA58)
#define ISP_AWB3_WT_RATIO_75                                    ISP_AWB3_WT_RATIO_75
#define ISP_AWB3_WT_RATIO_75_POS                                (0U)
#define ISP_AWB3_WT_RATIO_75_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_75_MSK                                (((1U<<ISP_AWB3_WT_RATIO_75_LEN)-1)<<ISP_AWB3_WT_RATIO_75_POS)
#define ISP_AWB3_WT_RATIO_75_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_75_LEN)-1)<<ISP_AWB3_WT_RATIO_75_POS))
#define ISP_AWB3_WT_RATIO_76                                    ISP_AWB3_WT_RATIO_76
#define ISP_AWB3_WT_RATIO_76_POS                                (8U)
#define ISP_AWB3_WT_RATIO_76_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_76_MSK                                (((1U<<ISP_AWB3_WT_RATIO_76_LEN)-1)<<ISP_AWB3_WT_RATIO_76_POS)
#define ISP_AWB3_WT_RATIO_76_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_76_LEN)-1)<<ISP_AWB3_WT_RATIO_76_POS))
#define ISP_AWB3_WT_RATIO_77                                    ISP_AWB3_WT_RATIO_77
#define ISP_AWB3_WT_RATIO_77_POS                                (16U)
#define ISP_AWB3_WT_RATIO_77_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_77_MSK                                (((1U<<ISP_AWB3_WT_RATIO_77_LEN)-1)<<ISP_AWB3_WT_RATIO_77_POS)
#define ISP_AWB3_WT_RATIO_77_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_77_LEN)-1)<<ISP_AWB3_WT_RATIO_77_POS))
#define ISP_AWB3_WT_RATIO_78                                    ISP_AWB3_WT_RATIO_78
#define ISP_AWB3_WT_RATIO_78_POS                                (24U)
#define ISP_AWB3_WT_RATIO_78_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_78_MSK                                (((1U<<ISP_AWB3_WT_RATIO_78_LEN)-1)<<ISP_AWB3_WT_RATIO_78_POS)
#define ISP_AWB3_WT_RATIO_78_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_78_LEN)-1)<<ISP_AWB3_WT_RATIO_78_POS))

/* 0xA5C : awb3_config_23 */
#define ISP_AWB3_CONFIG_23_OFFSET                               (0xA5C)
#define ISP_AWB3_WT_RATIO_80                                    ISP_AWB3_WT_RATIO_80
#define ISP_AWB3_WT_RATIO_80_POS                                (0U)
#define ISP_AWB3_WT_RATIO_80_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_80_MSK                                (((1U<<ISP_AWB3_WT_RATIO_80_LEN)-1)<<ISP_AWB3_WT_RATIO_80_POS)
#define ISP_AWB3_WT_RATIO_80_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_80_LEN)-1)<<ISP_AWB3_WT_RATIO_80_POS))
#define ISP_AWB3_WT_RATIO_81                                    ISP_AWB3_WT_RATIO_81
#define ISP_AWB3_WT_RATIO_81_POS                                (8U)
#define ISP_AWB3_WT_RATIO_81_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_81_MSK                                (((1U<<ISP_AWB3_WT_RATIO_81_LEN)-1)<<ISP_AWB3_WT_RATIO_81_POS)
#define ISP_AWB3_WT_RATIO_81_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_81_LEN)-1)<<ISP_AWB3_WT_RATIO_81_POS))
#define ISP_AWB3_WT_RATIO_82                                    ISP_AWB3_WT_RATIO_82
#define ISP_AWB3_WT_RATIO_82_POS                                (16U)
#define ISP_AWB3_WT_RATIO_82_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_82_MSK                                (((1U<<ISP_AWB3_WT_RATIO_82_LEN)-1)<<ISP_AWB3_WT_RATIO_82_POS)
#define ISP_AWB3_WT_RATIO_82_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_82_LEN)-1)<<ISP_AWB3_WT_RATIO_82_POS))
#define ISP_AWB3_WT_RATIO_83                                    ISP_AWB3_WT_RATIO_83
#define ISP_AWB3_WT_RATIO_83_POS                                (24U)
#define ISP_AWB3_WT_RATIO_83_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_83_MSK                                (((1U<<ISP_AWB3_WT_RATIO_83_LEN)-1)<<ISP_AWB3_WT_RATIO_83_POS)
#define ISP_AWB3_WT_RATIO_83_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_83_LEN)-1)<<ISP_AWB3_WT_RATIO_83_POS))

/* 0xA60 : awb3_config_24 */
#define ISP_AWB3_CONFIG_24_OFFSET                               (0xA60)
#define ISP_AWB3_WT_RATIO_84                                    ISP_AWB3_WT_RATIO_84
#define ISP_AWB3_WT_RATIO_84_POS                                (0U)
#define ISP_AWB3_WT_RATIO_84_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_84_MSK                                (((1U<<ISP_AWB3_WT_RATIO_84_LEN)-1)<<ISP_AWB3_WT_RATIO_84_POS)
#define ISP_AWB3_WT_RATIO_84_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_84_LEN)-1)<<ISP_AWB3_WT_RATIO_84_POS))
#define ISP_AWB3_WT_RATIO_85                                    ISP_AWB3_WT_RATIO_85
#define ISP_AWB3_WT_RATIO_85_POS                                (8U)
#define ISP_AWB3_WT_RATIO_85_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_85_MSK                                (((1U<<ISP_AWB3_WT_RATIO_85_LEN)-1)<<ISP_AWB3_WT_RATIO_85_POS)
#define ISP_AWB3_WT_RATIO_85_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_85_LEN)-1)<<ISP_AWB3_WT_RATIO_85_POS))
#define ISP_AWB3_WT_RATIO_86                                    ISP_AWB3_WT_RATIO_86
#define ISP_AWB3_WT_RATIO_86_POS                                (16U)
#define ISP_AWB3_WT_RATIO_86_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_86_MSK                                (((1U<<ISP_AWB3_WT_RATIO_86_LEN)-1)<<ISP_AWB3_WT_RATIO_86_POS)
#define ISP_AWB3_WT_RATIO_86_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_86_LEN)-1)<<ISP_AWB3_WT_RATIO_86_POS))
#define ISP_AWB3_WT_RATIO_87                                    ISP_AWB3_WT_RATIO_87
#define ISP_AWB3_WT_RATIO_87_POS                                (24U)
#define ISP_AWB3_WT_RATIO_87_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_87_MSK                                (((1U<<ISP_AWB3_WT_RATIO_87_LEN)-1)<<ISP_AWB3_WT_RATIO_87_POS)
#define ISP_AWB3_WT_RATIO_87_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_87_LEN)-1)<<ISP_AWB3_WT_RATIO_87_POS))

/* 0xA64 : awb3_config_25 */
#define ISP_AWB3_CONFIG_25_OFFSET                               (0xA64)
#define ISP_AWB3_WT_RATIO_88                                    ISP_AWB3_WT_RATIO_88
#define ISP_AWB3_WT_RATIO_88_POS                                (0U)
#define ISP_AWB3_WT_RATIO_88_LEN                                (8U)
#define ISP_AWB3_WT_RATIO_88_MSK                                (((1U<<ISP_AWB3_WT_RATIO_88_LEN)-1)<<ISP_AWB3_WT_RATIO_88_POS)
#define ISP_AWB3_WT_RATIO_88_UMSK                               (~(((1U<<ISP_AWB3_WT_RATIO_88_LEN)-1)<<ISP_AWB3_WT_RATIO_88_POS))

/* 0xA68 : awb3_config_26 */
#define ISP_AWB3_CONFIG_26_OFFSET                               (0xA68)
#define ISP_AWB3_Y_R_COEFF                                      ISP_AWB3_Y_R_COEFF
#define ISP_AWB3_Y_R_COEFF_POS                                  (0U)
#define ISP_AWB3_Y_R_COEFF_LEN                                  (12U)
#define ISP_AWB3_Y_R_COEFF_MSK                                  (((1U<<ISP_AWB3_Y_R_COEFF_LEN)-1)<<ISP_AWB3_Y_R_COEFF_POS)
#define ISP_AWB3_Y_R_COEFF_UMSK                                 (~(((1U<<ISP_AWB3_Y_R_COEFF_LEN)-1)<<ISP_AWB3_Y_R_COEFF_POS))

/* 0xA6C : awb3_config_27 */
#define ISP_AWB3_CONFIG_27_OFFSET                               (0xA6C)
#define ISP_AWB3_Y_G_COEFF                                      ISP_AWB3_Y_G_COEFF
#define ISP_AWB3_Y_G_COEFF_POS                                  (0U)
#define ISP_AWB3_Y_G_COEFF_LEN                                  (12U)
#define ISP_AWB3_Y_G_COEFF_MSK                                  (((1U<<ISP_AWB3_Y_G_COEFF_LEN)-1)<<ISP_AWB3_Y_G_COEFF_POS)
#define ISP_AWB3_Y_G_COEFF_UMSK                                 (~(((1U<<ISP_AWB3_Y_G_COEFF_LEN)-1)<<ISP_AWB3_Y_G_COEFF_POS))

/* 0xA70 : awb3_config_28 */
#define ISP_AWB3_CONFIG_28_OFFSET                               (0xA70)
#define ISP_AWB3_Y_B_COEFF                                      ISP_AWB3_Y_B_COEFF
#define ISP_AWB3_Y_B_COEFF_POS                                  (0U)
#define ISP_AWB3_Y_B_COEFF_LEN                                  (12U)
#define ISP_AWB3_Y_B_COEFF_MSK                                  (((1U<<ISP_AWB3_Y_B_COEFF_LEN)-1)<<ISP_AWB3_Y_B_COEFF_POS)
#define ISP_AWB3_Y_B_COEFF_UMSK                                 (~(((1U<<ISP_AWB3_Y_B_COEFF_LEN)-1)<<ISP_AWB3_Y_B_COEFF_POS))

/* 0xA74 : awb3_config_29 */
#define ISP_AWB3_CONFIG_29_OFFSET                               (0xA74)
#define ISP_AWB3_WT_Y_GAIN                                      ISP_AWB3_WT_Y_GAIN
#define ISP_AWB3_WT_Y_GAIN_POS                                  (0U)
#define ISP_AWB3_WT_Y_GAIN_LEN                                  (8U)
#define ISP_AWB3_WT_Y_GAIN_MSK                                  (((1U<<ISP_AWB3_WT_Y_GAIN_LEN)-1)<<ISP_AWB3_WT_Y_GAIN_POS)
#define ISP_AWB3_WT_Y_GAIN_UMSK                                 (~(((1U<<ISP_AWB3_WT_Y_GAIN_LEN)-1)<<ISP_AWB3_WT_Y_GAIN_POS))
#define ISP_AWB3_WT_Y_EN                                        ISP_AWB3_WT_Y_EN
#define ISP_AWB3_WT_Y_EN_POS                                    (8U)
#define ISP_AWB3_WT_Y_EN_LEN                                    (1U)
#define ISP_AWB3_WT_Y_EN_MSK                                    (((1U<<ISP_AWB3_WT_Y_EN_LEN)-1)<<ISP_AWB3_WT_Y_EN_POS)
#define ISP_AWB3_WT_Y_EN_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_EN_LEN)-1)<<ISP_AWB3_WT_Y_EN_POS))
#define ISP_AWB3_WT_Y_USE_G                                     ISP_AWB3_WT_Y_USE_G
#define ISP_AWB3_WT_Y_USE_G_POS                                 (12U)
#define ISP_AWB3_WT_Y_USE_G_LEN                                 (1U)
#define ISP_AWB3_WT_Y_USE_G_MSK                                 (((1U<<ISP_AWB3_WT_Y_USE_G_LEN)-1)<<ISP_AWB3_WT_Y_USE_G_POS)
#define ISP_AWB3_WT_Y_USE_G_UMSK                                (~(((1U<<ISP_AWB3_WT_Y_USE_G_LEN)-1)<<ISP_AWB3_WT_Y_USE_G_POS))

/* 0xA78 : awb3_config_30 */
#define ISP_AWB3_CONFIG_30_OFFSET                               (0xA78)
#define ISP_AWB3_WT_Y_00                                        ISP_AWB3_WT_Y_00
#define ISP_AWB3_WT_Y_00_POS                                    (0U)
#define ISP_AWB3_WT_Y_00_LEN                                    (8U)
#define ISP_AWB3_WT_Y_00_MSK                                    (((1U<<ISP_AWB3_WT_Y_00_LEN)-1)<<ISP_AWB3_WT_Y_00_POS)
#define ISP_AWB3_WT_Y_00_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_00_LEN)-1)<<ISP_AWB3_WT_Y_00_POS))
#define ISP_AWB3_WT_Y_01                                        ISP_AWB3_WT_Y_01
#define ISP_AWB3_WT_Y_01_POS                                    (8U)
#define ISP_AWB3_WT_Y_01_LEN                                    (8U)
#define ISP_AWB3_WT_Y_01_MSK                                    (((1U<<ISP_AWB3_WT_Y_01_LEN)-1)<<ISP_AWB3_WT_Y_01_POS)
#define ISP_AWB3_WT_Y_01_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_01_LEN)-1)<<ISP_AWB3_WT_Y_01_POS))
#define ISP_AWB3_WT_Y_02                                        ISP_AWB3_WT_Y_02
#define ISP_AWB3_WT_Y_02_POS                                    (16U)
#define ISP_AWB3_WT_Y_02_LEN                                    (8U)
#define ISP_AWB3_WT_Y_02_MSK                                    (((1U<<ISP_AWB3_WT_Y_02_LEN)-1)<<ISP_AWB3_WT_Y_02_POS)
#define ISP_AWB3_WT_Y_02_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_02_LEN)-1)<<ISP_AWB3_WT_Y_02_POS))
#define ISP_AWB3_WT_Y_03                                        ISP_AWB3_WT_Y_03
#define ISP_AWB3_WT_Y_03_POS                                    (24U)
#define ISP_AWB3_WT_Y_03_LEN                                    (8U)
#define ISP_AWB3_WT_Y_03_MSK                                    (((1U<<ISP_AWB3_WT_Y_03_LEN)-1)<<ISP_AWB3_WT_Y_03_POS)
#define ISP_AWB3_WT_Y_03_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_03_LEN)-1)<<ISP_AWB3_WT_Y_03_POS))

/* 0xA7C : awb3_config_31 */
#define ISP_AWB3_CONFIG_31_OFFSET                               (0xA7C)
#define ISP_AWB3_WT_Y_04                                        ISP_AWB3_WT_Y_04
#define ISP_AWB3_WT_Y_04_POS                                    (0U)
#define ISP_AWB3_WT_Y_04_LEN                                    (8U)
#define ISP_AWB3_WT_Y_04_MSK                                    (((1U<<ISP_AWB3_WT_Y_04_LEN)-1)<<ISP_AWB3_WT_Y_04_POS)
#define ISP_AWB3_WT_Y_04_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_04_LEN)-1)<<ISP_AWB3_WT_Y_04_POS))
#define ISP_AWB3_WT_Y_05                                        ISP_AWB3_WT_Y_05
#define ISP_AWB3_WT_Y_05_POS                                    (8U)
#define ISP_AWB3_WT_Y_05_LEN                                    (8U)
#define ISP_AWB3_WT_Y_05_MSK                                    (((1U<<ISP_AWB3_WT_Y_05_LEN)-1)<<ISP_AWB3_WT_Y_05_POS)
#define ISP_AWB3_WT_Y_05_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_05_LEN)-1)<<ISP_AWB3_WT_Y_05_POS))
#define ISP_AWB3_WT_Y_06                                        ISP_AWB3_WT_Y_06
#define ISP_AWB3_WT_Y_06_POS                                    (16U)
#define ISP_AWB3_WT_Y_06_LEN                                    (8U)
#define ISP_AWB3_WT_Y_06_MSK                                    (((1U<<ISP_AWB3_WT_Y_06_LEN)-1)<<ISP_AWB3_WT_Y_06_POS)
#define ISP_AWB3_WT_Y_06_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_06_LEN)-1)<<ISP_AWB3_WT_Y_06_POS))
#define ISP_AWB3_WT_Y_07                                        ISP_AWB3_WT_Y_07
#define ISP_AWB3_WT_Y_07_POS                                    (24U)
#define ISP_AWB3_WT_Y_07_LEN                                    (8U)
#define ISP_AWB3_WT_Y_07_MSK                                    (((1U<<ISP_AWB3_WT_Y_07_LEN)-1)<<ISP_AWB3_WT_Y_07_POS)
#define ISP_AWB3_WT_Y_07_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_07_LEN)-1)<<ISP_AWB3_WT_Y_07_POS))

/* 0xA80 : awb3_config_32 */
#define ISP_AWB3_CONFIG_32_OFFSET                               (0xA80)
#define ISP_AWB3_WT_Y_08                                        ISP_AWB3_WT_Y_08
#define ISP_AWB3_WT_Y_08_POS                                    (0U)
#define ISP_AWB3_WT_Y_08_LEN                                    (8U)
#define ISP_AWB3_WT_Y_08_MSK                                    (((1U<<ISP_AWB3_WT_Y_08_LEN)-1)<<ISP_AWB3_WT_Y_08_POS)
#define ISP_AWB3_WT_Y_08_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_08_LEN)-1)<<ISP_AWB3_WT_Y_08_POS))
#define ISP_AWB3_WT_Y_10                                        ISP_AWB3_WT_Y_10
#define ISP_AWB3_WT_Y_10_POS                                    (8U)
#define ISP_AWB3_WT_Y_10_LEN                                    (8U)
#define ISP_AWB3_WT_Y_10_MSK                                    (((1U<<ISP_AWB3_WT_Y_10_LEN)-1)<<ISP_AWB3_WT_Y_10_POS)
#define ISP_AWB3_WT_Y_10_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_10_LEN)-1)<<ISP_AWB3_WT_Y_10_POS))
#define ISP_AWB3_WT_Y_11                                        ISP_AWB3_WT_Y_11
#define ISP_AWB3_WT_Y_11_POS                                    (16U)
#define ISP_AWB3_WT_Y_11_LEN                                    (8U)
#define ISP_AWB3_WT_Y_11_MSK                                    (((1U<<ISP_AWB3_WT_Y_11_LEN)-1)<<ISP_AWB3_WT_Y_11_POS)
#define ISP_AWB3_WT_Y_11_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_11_LEN)-1)<<ISP_AWB3_WT_Y_11_POS))
#define ISP_AWB3_WT_Y_12                                        ISP_AWB3_WT_Y_12
#define ISP_AWB3_WT_Y_12_POS                                    (24U)
#define ISP_AWB3_WT_Y_12_LEN                                    (8U)
#define ISP_AWB3_WT_Y_12_MSK                                    (((1U<<ISP_AWB3_WT_Y_12_LEN)-1)<<ISP_AWB3_WT_Y_12_POS)
#define ISP_AWB3_WT_Y_12_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_12_LEN)-1)<<ISP_AWB3_WT_Y_12_POS))

/* 0xA84 : awb3_config_33 */
#define ISP_AWB3_CONFIG_33_OFFSET                               (0xA84)
#define ISP_AWB3_WT_Y_13                                        ISP_AWB3_WT_Y_13
#define ISP_AWB3_WT_Y_13_POS                                    (0U)
#define ISP_AWB3_WT_Y_13_LEN                                    (8U)
#define ISP_AWB3_WT_Y_13_MSK                                    (((1U<<ISP_AWB3_WT_Y_13_LEN)-1)<<ISP_AWB3_WT_Y_13_POS)
#define ISP_AWB3_WT_Y_13_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_13_LEN)-1)<<ISP_AWB3_WT_Y_13_POS))
#define ISP_AWB3_WT_Y_14                                        ISP_AWB3_WT_Y_14
#define ISP_AWB3_WT_Y_14_POS                                    (8U)
#define ISP_AWB3_WT_Y_14_LEN                                    (8U)
#define ISP_AWB3_WT_Y_14_MSK                                    (((1U<<ISP_AWB3_WT_Y_14_LEN)-1)<<ISP_AWB3_WT_Y_14_POS)
#define ISP_AWB3_WT_Y_14_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_14_LEN)-1)<<ISP_AWB3_WT_Y_14_POS))
#define ISP_AWB3_WT_Y_15                                        ISP_AWB3_WT_Y_15
#define ISP_AWB3_WT_Y_15_POS                                    (16U)
#define ISP_AWB3_WT_Y_15_LEN                                    (8U)
#define ISP_AWB3_WT_Y_15_MSK                                    (((1U<<ISP_AWB3_WT_Y_15_LEN)-1)<<ISP_AWB3_WT_Y_15_POS)
#define ISP_AWB3_WT_Y_15_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_15_LEN)-1)<<ISP_AWB3_WT_Y_15_POS))
#define ISP_AWB3_WT_Y_16                                        ISP_AWB3_WT_Y_16
#define ISP_AWB3_WT_Y_16_POS                                    (24U)
#define ISP_AWB3_WT_Y_16_LEN                                    (8U)
#define ISP_AWB3_WT_Y_16_MSK                                    (((1U<<ISP_AWB3_WT_Y_16_LEN)-1)<<ISP_AWB3_WT_Y_16_POS)
#define ISP_AWB3_WT_Y_16_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_16_LEN)-1)<<ISP_AWB3_WT_Y_16_POS))

/* 0xA88 : awb3_config_34 */
#define ISP_AWB3_CONFIG_34_OFFSET                               (0xA88)
#define ISP_AWB3_WT_Y_17                                        ISP_AWB3_WT_Y_17
#define ISP_AWB3_WT_Y_17_POS                                    (0U)
#define ISP_AWB3_WT_Y_17_LEN                                    (8U)
#define ISP_AWB3_WT_Y_17_MSK                                    (((1U<<ISP_AWB3_WT_Y_17_LEN)-1)<<ISP_AWB3_WT_Y_17_POS)
#define ISP_AWB3_WT_Y_17_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_17_LEN)-1)<<ISP_AWB3_WT_Y_17_POS))
#define ISP_AWB3_WT_Y_18                                        ISP_AWB3_WT_Y_18
#define ISP_AWB3_WT_Y_18_POS                                    (8U)
#define ISP_AWB3_WT_Y_18_LEN                                    (8U)
#define ISP_AWB3_WT_Y_18_MSK                                    (((1U<<ISP_AWB3_WT_Y_18_LEN)-1)<<ISP_AWB3_WT_Y_18_POS)
#define ISP_AWB3_WT_Y_18_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_18_LEN)-1)<<ISP_AWB3_WT_Y_18_POS))
#define ISP_AWB3_WT_Y_20                                        ISP_AWB3_WT_Y_20
#define ISP_AWB3_WT_Y_20_POS                                    (16U)
#define ISP_AWB3_WT_Y_20_LEN                                    (8U)
#define ISP_AWB3_WT_Y_20_MSK                                    (((1U<<ISP_AWB3_WT_Y_20_LEN)-1)<<ISP_AWB3_WT_Y_20_POS)
#define ISP_AWB3_WT_Y_20_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_20_LEN)-1)<<ISP_AWB3_WT_Y_20_POS))
#define ISP_AWB3_WT_Y_21                                        ISP_AWB3_WT_Y_21
#define ISP_AWB3_WT_Y_21_POS                                    (24U)
#define ISP_AWB3_WT_Y_21_LEN                                    (8U)
#define ISP_AWB3_WT_Y_21_MSK                                    (((1U<<ISP_AWB3_WT_Y_21_LEN)-1)<<ISP_AWB3_WT_Y_21_POS)
#define ISP_AWB3_WT_Y_21_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_21_LEN)-1)<<ISP_AWB3_WT_Y_21_POS))

/* 0xA8C : awb3_config_35 */
#define ISP_AWB3_CONFIG_35_OFFSET                               (0xA8C)
#define ISP_AWB3_WT_Y_22                                        ISP_AWB3_WT_Y_22
#define ISP_AWB3_WT_Y_22_POS                                    (0U)
#define ISP_AWB3_WT_Y_22_LEN                                    (8U)
#define ISP_AWB3_WT_Y_22_MSK                                    (((1U<<ISP_AWB3_WT_Y_22_LEN)-1)<<ISP_AWB3_WT_Y_22_POS)
#define ISP_AWB3_WT_Y_22_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_22_LEN)-1)<<ISP_AWB3_WT_Y_22_POS))
#define ISP_AWB3_WT_Y_23                                        ISP_AWB3_WT_Y_23
#define ISP_AWB3_WT_Y_23_POS                                    (8U)
#define ISP_AWB3_WT_Y_23_LEN                                    (8U)
#define ISP_AWB3_WT_Y_23_MSK                                    (((1U<<ISP_AWB3_WT_Y_23_LEN)-1)<<ISP_AWB3_WT_Y_23_POS)
#define ISP_AWB3_WT_Y_23_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_23_LEN)-1)<<ISP_AWB3_WT_Y_23_POS))
#define ISP_AWB3_WT_Y_24                                        ISP_AWB3_WT_Y_24
#define ISP_AWB3_WT_Y_24_POS                                    (16U)
#define ISP_AWB3_WT_Y_24_LEN                                    (8U)
#define ISP_AWB3_WT_Y_24_MSK                                    (((1U<<ISP_AWB3_WT_Y_24_LEN)-1)<<ISP_AWB3_WT_Y_24_POS)
#define ISP_AWB3_WT_Y_24_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_24_LEN)-1)<<ISP_AWB3_WT_Y_24_POS))
#define ISP_AWB3_WT_Y_25                                        ISP_AWB3_WT_Y_25
#define ISP_AWB3_WT_Y_25_POS                                    (24U)
#define ISP_AWB3_WT_Y_25_LEN                                    (8U)
#define ISP_AWB3_WT_Y_25_MSK                                    (((1U<<ISP_AWB3_WT_Y_25_LEN)-1)<<ISP_AWB3_WT_Y_25_POS)
#define ISP_AWB3_WT_Y_25_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_25_LEN)-1)<<ISP_AWB3_WT_Y_25_POS))

/* 0xA90 : awb3_config_36 */
#define ISP_AWB3_CONFIG_36_OFFSET                               (0xA90)
#define ISP_AWB3_WT_Y_26                                        ISP_AWB3_WT_Y_26
#define ISP_AWB3_WT_Y_26_POS                                    (0U)
#define ISP_AWB3_WT_Y_26_LEN                                    (8U)
#define ISP_AWB3_WT_Y_26_MSK                                    (((1U<<ISP_AWB3_WT_Y_26_LEN)-1)<<ISP_AWB3_WT_Y_26_POS)
#define ISP_AWB3_WT_Y_26_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_26_LEN)-1)<<ISP_AWB3_WT_Y_26_POS))
#define ISP_AWB3_WT_Y_27                                        ISP_AWB3_WT_Y_27
#define ISP_AWB3_WT_Y_27_POS                                    (8U)
#define ISP_AWB3_WT_Y_27_LEN                                    (8U)
#define ISP_AWB3_WT_Y_27_MSK                                    (((1U<<ISP_AWB3_WT_Y_27_LEN)-1)<<ISP_AWB3_WT_Y_27_POS)
#define ISP_AWB3_WT_Y_27_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_27_LEN)-1)<<ISP_AWB3_WT_Y_27_POS))
#define ISP_AWB3_WT_Y_28                                        ISP_AWB3_WT_Y_28
#define ISP_AWB3_WT_Y_28_POS                                    (16U)
#define ISP_AWB3_WT_Y_28_LEN                                    (8U)
#define ISP_AWB3_WT_Y_28_MSK                                    (((1U<<ISP_AWB3_WT_Y_28_LEN)-1)<<ISP_AWB3_WT_Y_28_POS)
#define ISP_AWB3_WT_Y_28_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_28_LEN)-1)<<ISP_AWB3_WT_Y_28_POS))
#define ISP_AWB3_WT_Y_30                                        ISP_AWB3_WT_Y_30
#define ISP_AWB3_WT_Y_30_POS                                    (24U)
#define ISP_AWB3_WT_Y_30_LEN                                    (8U)
#define ISP_AWB3_WT_Y_30_MSK                                    (((1U<<ISP_AWB3_WT_Y_30_LEN)-1)<<ISP_AWB3_WT_Y_30_POS)
#define ISP_AWB3_WT_Y_30_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_30_LEN)-1)<<ISP_AWB3_WT_Y_30_POS))

/* 0xA94 : awb3_config_37 */
#define ISP_AWB3_CONFIG_37_OFFSET                               (0xA94)
#define ISP_AWB3_WT_Y_31                                        ISP_AWB3_WT_Y_31
#define ISP_AWB3_WT_Y_31_POS                                    (0U)
#define ISP_AWB3_WT_Y_31_LEN                                    (8U)
#define ISP_AWB3_WT_Y_31_MSK                                    (((1U<<ISP_AWB3_WT_Y_31_LEN)-1)<<ISP_AWB3_WT_Y_31_POS)
#define ISP_AWB3_WT_Y_31_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_31_LEN)-1)<<ISP_AWB3_WT_Y_31_POS))
#define ISP_AWB3_WT_Y_32                                        ISP_AWB3_WT_Y_32
#define ISP_AWB3_WT_Y_32_POS                                    (8U)
#define ISP_AWB3_WT_Y_32_LEN                                    (8U)
#define ISP_AWB3_WT_Y_32_MSK                                    (((1U<<ISP_AWB3_WT_Y_32_LEN)-1)<<ISP_AWB3_WT_Y_32_POS)
#define ISP_AWB3_WT_Y_32_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_32_LEN)-1)<<ISP_AWB3_WT_Y_32_POS))
#define ISP_AWB3_WT_Y_33                                        ISP_AWB3_WT_Y_33
#define ISP_AWB3_WT_Y_33_POS                                    (16U)
#define ISP_AWB3_WT_Y_33_LEN                                    (8U)
#define ISP_AWB3_WT_Y_33_MSK                                    (((1U<<ISP_AWB3_WT_Y_33_LEN)-1)<<ISP_AWB3_WT_Y_33_POS)
#define ISP_AWB3_WT_Y_33_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_33_LEN)-1)<<ISP_AWB3_WT_Y_33_POS))
#define ISP_AWB3_WT_Y_34                                        ISP_AWB3_WT_Y_34
#define ISP_AWB3_WT_Y_34_POS                                    (24U)
#define ISP_AWB3_WT_Y_34_LEN                                    (8U)
#define ISP_AWB3_WT_Y_34_MSK                                    (((1U<<ISP_AWB3_WT_Y_34_LEN)-1)<<ISP_AWB3_WT_Y_34_POS)
#define ISP_AWB3_WT_Y_34_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_34_LEN)-1)<<ISP_AWB3_WT_Y_34_POS))

/* 0xA98 : awb3_config_38 */
#define ISP_AWB3_CONFIG_38_OFFSET                               (0xA98)
#define ISP_AWB3_WT_Y_35                                        ISP_AWB3_WT_Y_35
#define ISP_AWB3_WT_Y_35_POS                                    (0U)
#define ISP_AWB3_WT_Y_35_LEN                                    (8U)
#define ISP_AWB3_WT_Y_35_MSK                                    (((1U<<ISP_AWB3_WT_Y_35_LEN)-1)<<ISP_AWB3_WT_Y_35_POS)
#define ISP_AWB3_WT_Y_35_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_35_LEN)-1)<<ISP_AWB3_WT_Y_35_POS))
#define ISP_AWB3_WT_Y_36                                        ISP_AWB3_WT_Y_36
#define ISP_AWB3_WT_Y_36_POS                                    (8U)
#define ISP_AWB3_WT_Y_36_LEN                                    (8U)
#define ISP_AWB3_WT_Y_36_MSK                                    (((1U<<ISP_AWB3_WT_Y_36_LEN)-1)<<ISP_AWB3_WT_Y_36_POS)
#define ISP_AWB3_WT_Y_36_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_36_LEN)-1)<<ISP_AWB3_WT_Y_36_POS))
#define ISP_AWB3_WT_Y_37                                        ISP_AWB3_WT_Y_37
#define ISP_AWB3_WT_Y_37_POS                                    (16U)
#define ISP_AWB3_WT_Y_37_LEN                                    (8U)
#define ISP_AWB3_WT_Y_37_MSK                                    (((1U<<ISP_AWB3_WT_Y_37_LEN)-1)<<ISP_AWB3_WT_Y_37_POS)
#define ISP_AWB3_WT_Y_37_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_37_LEN)-1)<<ISP_AWB3_WT_Y_37_POS))
#define ISP_AWB3_WT_Y_38                                        ISP_AWB3_WT_Y_38
#define ISP_AWB3_WT_Y_38_POS                                    (24U)
#define ISP_AWB3_WT_Y_38_LEN                                    (8U)
#define ISP_AWB3_WT_Y_38_MSK                                    (((1U<<ISP_AWB3_WT_Y_38_LEN)-1)<<ISP_AWB3_WT_Y_38_POS)
#define ISP_AWB3_WT_Y_38_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_38_LEN)-1)<<ISP_AWB3_WT_Y_38_POS))

/* 0xA9C : awb3_config_39 */
#define ISP_AWB3_CONFIG_39_OFFSET                               (0xA9C)
#define ISP_AWB3_WT_Y_40                                        ISP_AWB3_WT_Y_40
#define ISP_AWB3_WT_Y_40_POS                                    (0U)
#define ISP_AWB3_WT_Y_40_LEN                                    (8U)
#define ISP_AWB3_WT_Y_40_MSK                                    (((1U<<ISP_AWB3_WT_Y_40_LEN)-1)<<ISP_AWB3_WT_Y_40_POS)
#define ISP_AWB3_WT_Y_40_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_40_LEN)-1)<<ISP_AWB3_WT_Y_40_POS))
#define ISP_AWB3_WT_Y_41                                        ISP_AWB3_WT_Y_41
#define ISP_AWB3_WT_Y_41_POS                                    (8U)
#define ISP_AWB3_WT_Y_41_LEN                                    (8U)
#define ISP_AWB3_WT_Y_41_MSK                                    (((1U<<ISP_AWB3_WT_Y_41_LEN)-1)<<ISP_AWB3_WT_Y_41_POS)
#define ISP_AWB3_WT_Y_41_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_41_LEN)-1)<<ISP_AWB3_WT_Y_41_POS))
#define ISP_AWB3_WT_Y_42                                        ISP_AWB3_WT_Y_42
#define ISP_AWB3_WT_Y_42_POS                                    (16U)
#define ISP_AWB3_WT_Y_42_LEN                                    (8U)
#define ISP_AWB3_WT_Y_42_MSK                                    (((1U<<ISP_AWB3_WT_Y_42_LEN)-1)<<ISP_AWB3_WT_Y_42_POS)
#define ISP_AWB3_WT_Y_42_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_42_LEN)-1)<<ISP_AWB3_WT_Y_42_POS))
#define ISP_AWB3_WT_Y_43                                        ISP_AWB3_WT_Y_43
#define ISP_AWB3_WT_Y_43_POS                                    (24U)
#define ISP_AWB3_WT_Y_43_LEN                                    (8U)
#define ISP_AWB3_WT_Y_43_MSK                                    (((1U<<ISP_AWB3_WT_Y_43_LEN)-1)<<ISP_AWB3_WT_Y_43_POS)
#define ISP_AWB3_WT_Y_43_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_43_LEN)-1)<<ISP_AWB3_WT_Y_43_POS))

/* 0xAA0 : awb3_config_40 */
#define ISP_AWB3_CONFIG_40_OFFSET                               (0xAA0)
#define ISP_AWB3_WT_Y_44                                        ISP_AWB3_WT_Y_44
#define ISP_AWB3_WT_Y_44_POS                                    (0U)
#define ISP_AWB3_WT_Y_44_LEN                                    (8U)
#define ISP_AWB3_WT_Y_44_MSK                                    (((1U<<ISP_AWB3_WT_Y_44_LEN)-1)<<ISP_AWB3_WT_Y_44_POS)
#define ISP_AWB3_WT_Y_44_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_44_LEN)-1)<<ISP_AWB3_WT_Y_44_POS))
#define ISP_AWB3_WT_Y_45                                        ISP_AWB3_WT_Y_45
#define ISP_AWB3_WT_Y_45_POS                                    (8U)
#define ISP_AWB3_WT_Y_45_LEN                                    (8U)
#define ISP_AWB3_WT_Y_45_MSK                                    (((1U<<ISP_AWB3_WT_Y_45_LEN)-1)<<ISP_AWB3_WT_Y_45_POS)
#define ISP_AWB3_WT_Y_45_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_45_LEN)-1)<<ISP_AWB3_WT_Y_45_POS))
#define ISP_AWB3_WT_Y_46                                        ISP_AWB3_WT_Y_46
#define ISP_AWB3_WT_Y_46_POS                                    (16U)
#define ISP_AWB3_WT_Y_46_LEN                                    (8U)
#define ISP_AWB3_WT_Y_46_MSK                                    (((1U<<ISP_AWB3_WT_Y_46_LEN)-1)<<ISP_AWB3_WT_Y_46_POS)
#define ISP_AWB3_WT_Y_46_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_46_LEN)-1)<<ISP_AWB3_WT_Y_46_POS))
#define ISP_AWB3_WT_Y_47                                        ISP_AWB3_WT_Y_47
#define ISP_AWB3_WT_Y_47_POS                                    (24U)
#define ISP_AWB3_WT_Y_47_LEN                                    (8U)
#define ISP_AWB3_WT_Y_47_MSK                                    (((1U<<ISP_AWB3_WT_Y_47_LEN)-1)<<ISP_AWB3_WT_Y_47_POS)
#define ISP_AWB3_WT_Y_47_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_47_LEN)-1)<<ISP_AWB3_WT_Y_47_POS))

/* 0xAA4 : awb3_config_41 */
#define ISP_AWB3_CONFIG_41_OFFSET                               (0xAA4)
#define ISP_AWB3_WT_Y_48                                        ISP_AWB3_WT_Y_48
#define ISP_AWB3_WT_Y_48_POS                                    (0U)
#define ISP_AWB3_WT_Y_48_LEN                                    (8U)
#define ISP_AWB3_WT_Y_48_MSK                                    (((1U<<ISP_AWB3_WT_Y_48_LEN)-1)<<ISP_AWB3_WT_Y_48_POS)
#define ISP_AWB3_WT_Y_48_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_48_LEN)-1)<<ISP_AWB3_WT_Y_48_POS))
#define ISP_AWB3_WT_Y_50                                        ISP_AWB3_WT_Y_50
#define ISP_AWB3_WT_Y_50_POS                                    (8U)
#define ISP_AWB3_WT_Y_50_LEN                                    (8U)
#define ISP_AWB3_WT_Y_50_MSK                                    (((1U<<ISP_AWB3_WT_Y_50_LEN)-1)<<ISP_AWB3_WT_Y_50_POS)
#define ISP_AWB3_WT_Y_50_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_50_LEN)-1)<<ISP_AWB3_WT_Y_50_POS))
#define ISP_AWB3_WT_Y_51                                        ISP_AWB3_WT_Y_51
#define ISP_AWB3_WT_Y_51_POS                                    (16U)
#define ISP_AWB3_WT_Y_51_LEN                                    (8U)
#define ISP_AWB3_WT_Y_51_MSK                                    (((1U<<ISP_AWB3_WT_Y_51_LEN)-1)<<ISP_AWB3_WT_Y_51_POS)
#define ISP_AWB3_WT_Y_51_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_51_LEN)-1)<<ISP_AWB3_WT_Y_51_POS))
#define ISP_AWB3_WT_Y_52                                        ISP_AWB3_WT_Y_52
#define ISP_AWB3_WT_Y_52_POS                                    (24U)
#define ISP_AWB3_WT_Y_52_LEN                                    (8U)
#define ISP_AWB3_WT_Y_52_MSK                                    (((1U<<ISP_AWB3_WT_Y_52_LEN)-1)<<ISP_AWB3_WT_Y_52_POS)
#define ISP_AWB3_WT_Y_52_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_52_LEN)-1)<<ISP_AWB3_WT_Y_52_POS))

/* 0xAA8 : awb3_config_42 */
#define ISP_AWB3_CONFIG_42_OFFSET                               (0xAA8)
#define ISP_AWB3_WT_Y_53                                        ISP_AWB3_WT_Y_53
#define ISP_AWB3_WT_Y_53_POS                                    (0U)
#define ISP_AWB3_WT_Y_53_LEN                                    (8U)
#define ISP_AWB3_WT_Y_53_MSK                                    (((1U<<ISP_AWB3_WT_Y_53_LEN)-1)<<ISP_AWB3_WT_Y_53_POS)
#define ISP_AWB3_WT_Y_53_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_53_LEN)-1)<<ISP_AWB3_WT_Y_53_POS))
#define ISP_AWB3_WT_Y_54                                        ISP_AWB3_WT_Y_54
#define ISP_AWB3_WT_Y_54_POS                                    (8U)
#define ISP_AWB3_WT_Y_54_LEN                                    (8U)
#define ISP_AWB3_WT_Y_54_MSK                                    (((1U<<ISP_AWB3_WT_Y_54_LEN)-1)<<ISP_AWB3_WT_Y_54_POS)
#define ISP_AWB3_WT_Y_54_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_54_LEN)-1)<<ISP_AWB3_WT_Y_54_POS))
#define ISP_AWB3_WT_Y_55                                        ISP_AWB3_WT_Y_55
#define ISP_AWB3_WT_Y_55_POS                                    (16U)
#define ISP_AWB3_WT_Y_55_LEN                                    (8U)
#define ISP_AWB3_WT_Y_55_MSK                                    (((1U<<ISP_AWB3_WT_Y_55_LEN)-1)<<ISP_AWB3_WT_Y_55_POS)
#define ISP_AWB3_WT_Y_55_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_55_LEN)-1)<<ISP_AWB3_WT_Y_55_POS))
#define ISP_AWB3_WT_Y_56                                        ISP_AWB3_WT_Y_56
#define ISP_AWB3_WT_Y_56_POS                                    (24U)
#define ISP_AWB3_WT_Y_56_LEN                                    (8U)
#define ISP_AWB3_WT_Y_56_MSK                                    (((1U<<ISP_AWB3_WT_Y_56_LEN)-1)<<ISP_AWB3_WT_Y_56_POS)
#define ISP_AWB3_WT_Y_56_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_56_LEN)-1)<<ISP_AWB3_WT_Y_56_POS))

/* 0xAAC : awb3_config_43 */
#define ISP_AWB3_CONFIG_43_OFFSET                               (0xAAC)
#define ISP_AWB3_WT_Y_57                                        ISP_AWB3_WT_Y_57
#define ISP_AWB3_WT_Y_57_POS                                    (0U)
#define ISP_AWB3_WT_Y_57_LEN                                    (8U)
#define ISP_AWB3_WT_Y_57_MSK                                    (((1U<<ISP_AWB3_WT_Y_57_LEN)-1)<<ISP_AWB3_WT_Y_57_POS)
#define ISP_AWB3_WT_Y_57_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_57_LEN)-1)<<ISP_AWB3_WT_Y_57_POS))
#define ISP_AWB3_WT_Y_58                                        ISP_AWB3_WT_Y_58
#define ISP_AWB3_WT_Y_58_POS                                    (8U)
#define ISP_AWB3_WT_Y_58_LEN                                    (8U)
#define ISP_AWB3_WT_Y_58_MSK                                    (((1U<<ISP_AWB3_WT_Y_58_LEN)-1)<<ISP_AWB3_WT_Y_58_POS)
#define ISP_AWB3_WT_Y_58_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_58_LEN)-1)<<ISP_AWB3_WT_Y_58_POS))
#define ISP_AWB3_WT_Y_60                                        ISP_AWB3_WT_Y_60
#define ISP_AWB3_WT_Y_60_POS                                    (16U)
#define ISP_AWB3_WT_Y_60_LEN                                    (8U)
#define ISP_AWB3_WT_Y_60_MSK                                    (((1U<<ISP_AWB3_WT_Y_60_LEN)-1)<<ISP_AWB3_WT_Y_60_POS)
#define ISP_AWB3_WT_Y_60_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_60_LEN)-1)<<ISP_AWB3_WT_Y_60_POS))
#define ISP_AWB3_WT_Y_61                                        ISP_AWB3_WT_Y_61
#define ISP_AWB3_WT_Y_61_POS                                    (24U)
#define ISP_AWB3_WT_Y_61_LEN                                    (8U)
#define ISP_AWB3_WT_Y_61_MSK                                    (((1U<<ISP_AWB3_WT_Y_61_LEN)-1)<<ISP_AWB3_WT_Y_61_POS)
#define ISP_AWB3_WT_Y_61_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_61_LEN)-1)<<ISP_AWB3_WT_Y_61_POS))

/* 0xAB0 : awb3_config_44 */
#define ISP_AWB3_CONFIG_44_OFFSET                               (0xAB0)
#define ISP_AWB3_WT_Y_62                                        ISP_AWB3_WT_Y_62
#define ISP_AWB3_WT_Y_62_POS                                    (0U)
#define ISP_AWB3_WT_Y_62_LEN                                    (8U)
#define ISP_AWB3_WT_Y_62_MSK                                    (((1U<<ISP_AWB3_WT_Y_62_LEN)-1)<<ISP_AWB3_WT_Y_62_POS)
#define ISP_AWB3_WT_Y_62_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_62_LEN)-1)<<ISP_AWB3_WT_Y_62_POS))
#define ISP_AWB3_WT_Y_63                                        ISP_AWB3_WT_Y_63
#define ISP_AWB3_WT_Y_63_POS                                    (8U)
#define ISP_AWB3_WT_Y_63_LEN                                    (8U)
#define ISP_AWB3_WT_Y_63_MSK                                    (((1U<<ISP_AWB3_WT_Y_63_LEN)-1)<<ISP_AWB3_WT_Y_63_POS)
#define ISP_AWB3_WT_Y_63_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_63_LEN)-1)<<ISP_AWB3_WT_Y_63_POS))
#define ISP_AWB3_WT_Y_64                                        ISP_AWB3_WT_Y_64
#define ISP_AWB3_WT_Y_64_POS                                    (16U)
#define ISP_AWB3_WT_Y_64_LEN                                    (8U)
#define ISP_AWB3_WT_Y_64_MSK                                    (((1U<<ISP_AWB3_WT_Y_64_LEN)-1)<<ISP_AWB3_WT_Y_64_POS)
#define ISP_AWB3_WT_Y_64_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_64_LEN)-1)<<ISP_AWB3_WT_Y_64_POS))
#define ISP_AWB3_WT_Y_65                                        ISP_AWB3_WT_Y_65
#define ISP_AWB3_WT_Y_65_POS                                    (24U)
#define ISP_AWB3_WT_Y_65_LEN                                    (8U)
#define ISP_AWB3_WT_Y_65_MSK                                    (((1U<<ISP_AWB3_WT_Y_65_LEN)-1)<<ISP_AWB3_WT_Y_65_POS)
#define ISP_AWB3_WT_Y_65_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_65_LEN)-1)<<ISP_AWB3_WT_Y_65_POS))

/* 0xAB4 : awb3_config_45 */
#define ISP_AWB3_CONFIG_45_OFFSET                               (0xAB4)
#define ISP_AWB3_WT_Y_66                                        ISP_AWB3_WT_Y_66
#define ISP_AWB3_WT_Y_66_POS                                    (0U)
#define ISP_AWB3_WT_Y_66_LEN                                    (8U)
#define ISP_AWB3_WT_Y_66_MSK                                    (((1U<<ISP_AWB3_WT_Y_66_LEN)-1)<<ISP_AWB3_WT_Y_66_POS)
#define ISP_AWB3_WT_Y_66_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_66_LEN)-1)<<ISP_AWB3_WT_Y_66_POS))
#define ISP_AWB3_WT_Y_67                                        ISP_AWB3_WT_Y_67
#define ISP_AWB3_WT_Y_67_POS                                    (8U)
#define ISP_AWB3_WT_Y_67_LEN                                    (8U)
#define ISP_AWB3_WT_Y_67_MSK                                    (((1U<<ISP_AWB3_WT_Y_67_LEN)-1)<<ISP_AWB3_WT_Y_67_POS)
#define ISP_AWB3_WT_Y_67_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_67_LEN)-1)<<ISP_AWB3_WT_Y_67_POS))
#define ISP_AWB3_WT_Y_68                                        ISP_AWB3_WT_Y_68
#define ISP_AWB3_WT_Y_68_POS                                    (16U)
#define ISP_AWB3_WT_Y_68_LEN                                    (8U)
#define ISP_AWB3_WT_Y_68_MSK                                    (((1U<<ISP_AWB3_WT_Y_68_LEN)-1)<<ISP_AWB3_WT_Y_68_POS)
#define ISP_AWB3_WT_Y_68_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_68_LEN)-1)<<ISP_AWB3_WT_Y_68_POS))
#define ISP_AWB3_WT_Y_70                                        ISP_AWB3_WT_Y_70
#define ISP_AWB3_WT_Y_70_POS                                    (24U)
#define ISP_AWB3_WT_Y_70_LEN                                    (8U)
#define ISP_AWB3_WT_Y_70_MSK                                    (((1U<<ISP_AWB3_WT_Y_70_LEN)-1)<<ISP_AWB3_WT_Y_70_POS)
#define ISP_AWB3_WT_Y_70_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_70_LEN)-1)<<ISP_AWB3_WT_Y_70_POS))

/* 0xAB8 : awb3_config_46 */
#define ISP_AWB3_CONFIG_46_OFFSET                               (0xAB8)
#define ISP_AWB3_WT_Y_71                                        ISP_AWB3_WT_Y_71
#define ISP_AWB3_WT_Y_71_POS                                    (0U)
#define ISP_AWB3_WT_Y_71_LEN                                    (8U)
#define ISP_AWB3_WT_Y_71_MSK                                    (((1U<<ISP_AWB3_WT_Y_71_LEN)-1)<<ISP_AWB3_WT_Y_71_POS)
#define ISP_AWB3_WT_Y_71_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_71_LEN)-1)<<ISP_AWB3_WT_Y_71_POS))
#define ISP_AWB3_WT_Y_72                                        ISP_AWB3_WT_Y_72
#define ISP_AWB3_WT_Y_72_POS                                    (8U)
#define ISP_AWB3_WT_Y_72_LEN                                    (8U)
#define ISP_AWB3_WT_Y_72_MSK                                    (((1U<<ISP_AWB3_WT_Y_72_LEN)-1)<<ISP_AWB3_WT_Y_72_POS)
#define ISP_AWB3_WT_Y_72_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_72_LEN)-1)<<ISP_AWB3_WT_Y_72_POS))
#define ISP_AWB3_WT_Y_73                                        ISP_AWB3_WT_Y_73
#define ISP_AWB3_WT_Y_73_POS                                    (16U)
#define ISP_AWB3_WT_Y_73_LEN                                    (8U)
#define ISP_AWB3_WT_Y_73_MSK                                    (((1U<<ISP_AWB3_WT_Y_73_LEN)-1)<<ISP_AWB3_WT_Y_73_POS)
#define ISP_AWB3_WT_Y_73_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_73_LEN)-1)<<ISP_AWB3_WT_Y_73_POS))
#define ISP_AWB3_WT_Y_74                                        ISP_AWB3_WT_Y_74
#define ISP_AWB3_WT_Y_74_POS                                    (24U)
#define ISP_AWB3_WT_Y_74_LEN                                    (8U)
#define ISP_AWB3_WT_Y_74_MSK                                    (((1U<<ISP_AWB3_WT_Y_74_LEN)-1)<<ISP_AWB3_WT_Y_74_POS)
#define ISP_AWB3_WT_Y_74_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_74_LEN)-1)<<ISP_AWB3_WT_Y_74_POS))

/* 0xABC : awb3_config_47 */
#define ISP_AWB3_CONFIG_47_OFFSET                               (0xABC)
#define ISP_AWB3_WT_Y_75                                        ISP_AWB3_WT_Y_75
#define ISP_AWB3_WT_Y_75_POS                                    (0U)
#define ISP_AWB3_WT_Y_75_LEN                                    (8U)
#define ISP_AWB3_WT_Y_75_MSK                                    (((1U<<ISP_AWB3_WT_Y_75_LEN)-1)<<ISP_AWB3_WT_Y_75_POS)
#define ISP_AWB3_WT_Y_75_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_75_LEN)-1)<<ISP_AWB3_WT_Y_75_POS))
#define ISP_AWB3_WT_Y_76                                        ISP_AWB3_WT_Y_76
#define ISP_AWB3_WT_Y_76_POS                                    (8U)
#define ISP_AWB3_WT_Y_76_LEN                                    (8U)
#define ISP_AWB3_WT_Y_76_MSK                                    (((1U<<ISP_AWB3_WT_Y_76_LEN)-1)<<ISP_AWB3_WT_Y_76_POS)
#define ISP_AWB3_WT_Y_76_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_76_LEN)-1)<<ISP_AWB3_WT_Y_76_POS))
#define ISP_AWB3_WT_Y_77                                        ISP_AWB3_WT_Y_77
#define ISP_AWB3_WT_Y_77_POS                                    (16U)
#define ISP_AWB3_WT_Y_77_LEN                                    (8U)
#define ISP_AWB3_WT_Y_77_MSK                                    (((1U<<ISP_AWB3_WT_Y_77_LEN)-1)<<ISP_AWB3_WT_Y_77_POS)
#define ISP_AWB3_WT_Y_77_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_77_LEN)-1)<<ISP_AWB3_WT_Y_77_POS))
#define ISP_AWB3_WT_Y_78                                        ISP_AWB3_WT_Y_78
#define ISP_AWB3_WT_Y_78_POS                                    (24U)
#define ISP_AWB3_WT_Y_78_LEN                                    (8U)
#define ISP_AWB3_WT_Y_78_MSK                                    (((1U<<ISP_AWB3_WT_Y_78_LEN)-1)<<ISP_AWB3_WT_Y_78_POS)
#define ISP_AWB3_WT_Y_78_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_78_LEN)-1)<<ISP_AWB3_WT_Y_78_POS))

/* 0xAC0 : awb3_config_48 */
#define ISP_AWB3_CONFIG_48_OFFSET                               (0xAC0)
#define ISP_AWB3_WT_Y_80                                        ISP_AWB3_WT_Y_80
#define ISP_AWB3_WT_Y_80_POS                                    (0U)
#define ISP_AWB3_WT_Y_80_LEN                                    (8U)
#define ISP_AWB3_WT_Y_80_MSK                                    (((1U<<ISP_AWB3_WT_Y_80_LEN)-1)<<ISP_AWB3_WT_Y_80_POS)
#define ISP_AWB3_WT_Y_80_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_80_LEN)-1)<<ISP_AWB3_WT_Y_80_POS))
#define ISP_AWB3_WT_Y_81                                        ISP_AWB3_WT_Y_81
#define ISP_AWB3_WT_Y_81_POS                                    (8U)
#define ISP_AWB3_WT_Y_81_LEN                                    (8U)
#define ISP_AWB3_WT_Y_81_MSK                                    (((1U<<ISP_AWB3_WT_Y_81_LEN)-1)<<ISP_AWB3_WT_Y_81_POS)
#define ISP_AWB3_WT_Y_81_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_81_LEN)-1)<<ISP_AWB3_WT_Y_81_POS))
#define ISP_AWB3_WT_Y_82                                        ISP_AWB3_WT_Y_82
#define ISP_AWB3_WT_Y_82_POS                                    (16U)
#define ISP_AWB3_WT_Y_82_LEN                                    (8U)
#define ISP_AWB3_WT_Y_82_MSK                                    (((1U<<ISP_AWB3_WT_Y_82_LEN)-1)<<ISP_AWB3_WT_Y_82_POS)
#define ISP_AWB3_WT_Y_82_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_82_LEN)-1)<<ISP_AWB3_WT_Y_82_POS))
#define ISP_AWB3_WT_Y_83                                        ISP_AWB3_WT_Y_83
#define ISP_AWB3_WT_Y_83_POS                                    (24U)
#define ISP_AWB3_WT_Y_83_LEN                                    (8U)
#define ISP_AWB3_WT_Y_83_MSK                                    (((1U<<ISP_AWB3_WT_Y_83_LEN)-1)<<ISP_AWB3_WT_Y_83_POS)
#define ISP_AWB3_WT_Y_83_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_83_LEN)-1)<<ISP_AWB3_WT_Y_83_POS))

/* 0xAC4 : awb3_config_49 */
#define ISP_AWB3_CONFIG_49_OFFSET                               (0xAC4)
#define ISP_AWB3_WT_Y_84                                        ISP_AWB3_WT_Y_84
#define ISP_AWB3_WT_Y_84_POS                                    (0U)
#define ISP_AWB3_WT_Y_84_LEN                                    (8U)
#define ISP_AWB3_WT_Y_84_MSK                                    (((1U<<ISP_AWB3_WT_Y_84_LEN)-1)<<ISP_AWB3_WT_Y_84_POS)
#define ISP_AWB3_WT_Y_84_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_84_LEN)-1)<<ISP_AWB3_WT_Y_84_POS))
#define ISP_AWB3_WT_Y_85                                        ISP_AWB3_WT_Y_85
#define ISP_AWB3_WT_Y_85_POS                                    (8U)
#define ISP_AWB3_WT_Y_85_LEN                                    (8U)
#define ISP_AWB3_WT_Y_85_MSK                                    (((1U<<ISP_AWB3_WT_Y_85_LEN)-1)<<ISP_AWB3_WT_Y_85_POS)
#define ISP_AWB3_WT_Y_85_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_85_LEN)-1)<<ISP_AWB3_WT_Y_85_POS))
#define ISP_AWB3_WT_Y_86                                        ISP_AWB3_WT_Y_86
#define ISP_AWB3_WT_Y_86_POS                                    (16U)
#define ISP_AWB3_WT_Y_86_LEN                                    (8U)
#define ISP_AWB3_WT_Y_86_MSK                                    (((1U<<ISP_AWB3_WT_Y_86_LEN)-1)<<ISP_AWB3_WT_Y_86_POS)
#define ISP_AWB3_WT_Y_86_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_86_LEN)-1)<<ISP_AWB3_WT_Y_86_POS))
#define ISP_AWB3_WT_Y_87                                        ISP_AWB3_WT_Y_87
#define ISP_AWB3_WT_Y_87_POS                                    (24U)
#define ISP_AWB3_WT_Y_87_LEN                                    (8U)
#define ISP_AWB3_WT_Y_87_MSK                                    (((1U<<ISP_AWB3_WT_Y_87_LEN)-1)<<ISP_AWB3_WT_Y_87_POS)
#define ISP_AWB3_WT_Y_87_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_87_LEN)-1)<<ISP_AWB3_WT_Y_87_POS))

/* 0xAC8 : awb3_config_50 */
#define ISP_AWB3_CONFIG_50_OFFSET                               (0xAC8)
#define ISP_AWB3_WT_Y_88                                        ISP_AWB3_WT_Y_88
#define ISP_AWB3_WT_Y_88_POS                                    (0U)
#define ISP_AWB3_WT_Y_88_LEN                                    (8U)
#define ISP_AWB3_WT_Y_88_MSK                                    (((1U<<ISP_AWB3_WT_Y_88_LEN)-1)<<ISP_AWB3_WT_Y_88_POS)
#define ISP_AWB3_WT_Y_88_UMSK                                   (~(((1U<<ISP_AWB3_WT_Y_88_LEN)-1)<<ISP_AWB3_WT_Y_88_POS))

/* 0xACC : awb3_config_51 */
#define ISP_AWB3_CONFIG_51_OFFSET                               (0xACC)
#define ISP_AWB3_INK_GAIN                                       ISP_AWB3_INK_GAIN
#define ISP_AWB3_INK_GAIN_POS                                   (0U)
#define ISP_AWB3_INK_GAIN_LEN                                   (4U)
#define ISP_AWB3_INK_GAIN_MSK                                   (((1U<<ISP_AWB3_INK_GAIN_LEN)-1)<<ISP_AWB3_INK_GAIN_POS)
#define ISP_AWB3_INK_GAIN_UMSK                                  (~(((1U<<ISP_AWB3_INK_GAIN_LEN)-1)<<ISP_AWB3_INK_GAIN_POS))
#define ISP_AWB3_INK_TYPE                                       ISP_AWB3_INK_TYPE
#define ISP_AWB3_INK_TYPE_POS                                   (4U)
#define ISP_AWB3_INK_TYPE_LEN                                   (2U)
#define ISP_AWB3_INK_TYPE_MSK                                   (((1U<<ISP_AWB3_INK_TYPE_LEN)-1)<<ISP_AWB3_INK_TYPE_POS)
#define ISP_AWB3_INK_TYPE_UMSK                                  (~(((1U<<ISP_AWB3_INK_TYPE_LEN)-1)<<ISP_AWB3_INK_TYPE_POS))
#define ISP_AWB3_COUNT_ZERO_WT                                  ISP_AWB3_COUNT_ZERO_WT
#define ISP_AWB3_COUNT_ZERO_WT_POS                              (8U)
#define ISP_AWB3_COUNT_ZERO_WT_LEN                              (1U)
#define ISP_AWB3_COUNT_ZERO_WT_MSK                              (((1U<<ISP_AWB3_COUNT_ZERO_WT_LEN)-1)<<ISP_AWB3_COUNT_ZERO_WT_POS)
#define ISP_AWB3_COUNT_ZERO_WT_UMSK                             (~(((1U<<ISP_AWB3_COUNT_ZERO_WT_LEN)-1)<<ISP_AWB3_COUNT_ZERO_WT_POS))

/* 0xAD8 : awb3_sta_1 */
#define ISP_AWB3_STA_1_OFFSET                                   (0xAD8)
#define ISP_AWB3_R_SUM0L_R                                      ISP_AWB3_R_SUM0L_R
#define ISP_AWB3_R_SUM0L_R_POS                                  (0U)
#define ISP_AWB3_R_SUM0L_R_LEN                                  (32U)
#define ISP_AWB3_R_SUM0L_R_MSK                                  (((1U<<ISP_AWB3_R_SUM0L_R_LEN)-1)<<ISP_AWB3_R_SUM0L_R_POS)
#define ISP_AWB3_R_SUM0L_R_UMSK                                 (~(((1U<<ISP_AWB3_R_SUM0L_R_LEN)-1)<<ISP_AWB3_R_SUM0L_R_POS))

/* 0xAE0 : awb3_sta_3 */
#define ISP_AWB3_STA_3_OFFSET                                   (0xAE0)
#define ISP_AWB3_G_SUM0L_R                                      ISP_AWB3_G_SUM0L_R
#define ISP_AWB3_G_SUM0L_R_POS                                  (0U)
#define ISP_AWB3_G_SUM0L_R_LEN                                  (32U)
#define ISP_AWB3_G_SUM0L_R_MSK                                  (((1U<<ISP_AWB3_G_SUM0L_R_LEN)-1)<<ISP_AWB3_G_SUM0L_R_POS)
#define ISP_AWB3_G_SUM0L_R_UMSK                                 (~(((1U<<ISP_AWB3_G_SUM0L_R_LEN)-1)<<ISP_AWB3_G_SUM0L_R_POS))

/* 0xAE8 : awb3_sta_5 */
#define ISP_AWB3_STA_5_OFFSET                                   (0xAE8)
#define ISP_AWB3_B_SUM0L_R                                      ISP_AWB3_B_SUM0L_R
#define ISP_AWB3_B_SUM0L_R_POS                                  (0U)
#define ISP_AWB3_B_SUM0L_R_LEN                                  (32U)
#define ISP_AWB3_B_SUM0L_R_MSK                                  (((1U<<ISP_AWB3_B_SUM0L_R_LEN)-1)<<ISP_AWB3_B_SUM0L_R_POS)
#define ISP_AWB3_B_SUM0L_R_UMSK                                 (~(((1U<<ISP_AWB3_B_SUM0L_R_LEN)-1)<<ISP_AWB3_B_SUM0L_R_POS))

/* 0xAF0 : awb3_sta_7 */
#define ISP_AWB3_STA_7_OFFSET                                   (0xAF0)
#define ISP_AWB3_R_SUM_HEX0L_R                                  ISP_AWB3_R_SUM_HEX0L_R
#define ISP_AWB3_R_SUM_HEX0L_R_POS                              (0U)
#define ISP_AWB3_R_SUM_HEX0L_R_LEN                              (32U)
#define ISP_AWB3_R_SUM_HEX0L_R_MSK                              (((1U<<ISP_AWB3_R_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_R_SUM_HEX0L_R_POS)
#define ISP_AWB3_R_SUM_HEX0L_R_UMSK                             (~(((1U<<ISP_AWB3_R_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_R_SUM_HEX0L_R_POS))

/* 0xAF8 : awb3_sta_9 */
#define ISP_AWB3_STA_9_OFFSET                                   (0xAF8)
#define ISP_AWB3_G_SUM_HEX0L_R                                  ISP_AWB3_G_SUM_HEX0L_R
#define ISP_AWB3_G_SUM_HEX0L_R_POS                              (0U)
#define ISP_AWB3_G_SUM_HEX0L_R_LEN                              (32U)
#define ISP_AWB3_G_SUM_HEX0L_R_MSK                              (((1U<<ISP_AWB3_G_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_G_SUM_HEX0L_R_POS)
#define ISP_AWB3_G_SUM_HEX0L_R_UMSK                             (~(((1U<<ISP_AWB3_G_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_G_SUM_HEX0L_R_POS))

/* 0xB00 : awb3_sta_11 */
#define ISP_AWB3_STA_11_OFFSET                                  (0xB00)
#define ISP_AWB3_B_SUM_HEX0L_R                                  ISP_AWB3_B_SUM_HEX0L_R
#define ISP_AWB3_B_SUM_HEX0L_R_POS                              (0U)
#define ISP_AWB3_B_SUM_HEX0L_R_LEN                              (32U)
#define ISP_AWB3_B_SUM_HEX0L_R_MSK                              (((1U<<ISP_AWB3_B_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_B_SUM_HEX0L_R_POS)
#define ISP_AWB3_B_SUM_HEX0L_R_UMSK                             (~(((1U<<ISP_AWB3_B_SUM_HEX0L_R_LEN)-1)<<ISP_AWB3_B_SUM_HEX0L_R_POS))

/* 0xB04 : awb3_sta_12 */
#define ISP_AWB3_STA_12_OFFSET                                  (0xB04)
#define ISP_AWB3_PIXEL_HEX0_R                                   ISP_AWB3_PIXEL_HEX0_R
#define ISP_AWB3_PIXEL_HEX0_R_POS                               (0U)
#define ISP_AWB3_PIXEL_HEX0_R_LEN                               (20U)
#define ISP_AWB3_PIXEL_HEX0_R_MSK                               (((1U<<ISP_AWB3_PIXEL_HEX0_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX0_R_POS)
#define ISP_AWB3_PIXEL_HEX0_R_UMSK                              (~(((1U<<ISP_AWB3_PIXEL_HEX0_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX0_R_POS))

/* 0xB08 : awb3_sta_13 */
#define ISP_AWB3_STA_13_OFFSET                                  (0xB08)
#define ISP_AWB3_R_RATIO_HEX_Y0U_R                              ISP_AWB3_R_RATIO_HEX_Y0U_R
#define ISP_AWB3_R_RATIO_HEX_Y0U_R_POS                          (0U)
#define ISP_AWB3_R_RATIO_HEX_Y0U_R_LEN                          (8U)
#define ISP_AWB3_R_RATIO_HEX_Y0U_R_MSK                          (((1U<<ISP_AWB3_R_RATIO_HEX_Y0U_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y0U_R_POS)
#define ISP_AWB3_R_RATIO_HEX_Y0U_R_UMSK                         (~(((1U<<ISP_AWB3_R_RATIO_HEX_Y0U_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y0U_R_POS))

/* 0xB0C : awb3_sta_14 */
#define ISP_AWB3_STA_14_OFFSET                                  (0xB0C)
#define ISP_AWB3_R_RATIO_HEX_Y0L_R                              ISP_AWB3_R_RATIO_HEX_Y0L_R
#define ISP_AWB3_R_RATIO_HEX_Y0L_R_POS                          (0U)
#define ISP_AWB3_R_RATIO_HEX_Y0L_R_LEN                          (32U)
#define ISP_AWB3_R_RATIO_HEX_Y0L_R_MSK                          (((1U<<ISP_AWB3_R_RATIO_HEX_Y0L_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y0L_R_POS)
#define ISP_AWB3_R_RATIO_HEX_Y0L_R_UMSK                         (~(((1U<<ISP_AWB3_R_RATIO_HEX_Y0L_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y0L_R_POS))

/* 0xB10 : awb3_sta_15 */
#define ISP_AWB3_STA_15_OFFSET                                  (0xB10)
#define ISP_AWB3_B_RATIO_HEX_Y0U_R                              ISP_AWB3_B_RATIO_HEX_Y0U_R
#define ISP_AWB3_B_RATIO_HEX_Y0U_R_POS                          (0U)
#define ISP_AWB3_B_RATIO_HEX_Y0U_R_LEN                          (8U)
#define ISP_AWB3_B_RATIO_HEX_Y0U_R_MSK                          (((1U<<ISP_AWB3_B_RATIO_HEX_Y0U_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y0U_R_POS)
#define ISP_AWB3_B_RATIO_HEX_Y0U_R_UMSK                         (~(((1U<<ISP_AWB3_B_RATIO_HEX_Y0U_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y0U_R_POS))

/* 0xB14 : awb3_sta_16 */
#define ISP_AWB3_STA_16_OFFSET                                  (0xB14)
#define ISP_AWB3_B_RATIO_HEX_Y0L_R                              ISP_AWB3_B_RATIO_HEX_Y0L_R
#define ISP_AWB3_B_RATIO_HEX_Y0L_R_POS                          (0U)
#define ISP_AWB3_B_RATIO_HEX_Y0L_R_LEN                          (32U)
#define ISP_AWB3_B_RATIO_HEX_Y0L_R_MSK                          (((1U<<ISP_AWB3_B_RATIO_HEX_Y0L_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y0L_R_POS)
#define ISP_AWB3_B_RATIO_HEX_Y0L_R_UMSK                         (~(((1U<<ISP_AWB3_B_RATIO_HEX_Y0L_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y0L_R_POS))

/* 0xB18 : awb3_sta_17 */
#define ISP_AWB3_STA_17_OFFSET                                  (0xB18)
#define ISP_AWB3_WT_HEX_Y0_R                                    ISP_AWB3_WT_HEX_Y0_R
#define ISP_AWB3_WT_HEX_Y0_R_POS                                (0U)
#define ISP_AWB3_WT_HEX_Y0_R_LEN                                (28U)
#define ISP_AWB3_WT_HEX_Y0_R_MSK                                (((1U<<ISP_AWB3_WT_HEX_Y0_R_LEN)-1)<<ISP_AWB3_WT_HEX_Y0_R_POS)
#define ISP_AWB3_WT_HEX_Y0_R_UMSK                               (~(((1U<<ISP_AWB3_WT_HEX_Y0_R_LEN)-1)<<ISP_AWB3_WT_HEX_Y0_R_POS))

/* 0xB1C : awb3_sta_18 */
#define ISP_AWB3_STA_18_OFFSET                                  (0xB1C)
#define ISP_AWB3_PIXEL_HEX_Y0_R                                 ISP_AWB3_PIXEL_HEX_Y0_R
#define ISP_AWB3_PIXEL_HEX_Y0_R_POS                             (0U)
#define ISP_AWB3_PIXEL_HEX_Y0_R_LEN                             (20U)
#define ISP_AWB3_PIXEL_HEX_Y0_R_MSK                             (((1U<<ISP_AWB3_PIXEL_HEX_Y0_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX_Y0_R_POS)
#define ISP_AWB3_PIXEL_HEX_Y0_R_UMSK                            (~(((1U<<ISP_AWB3_PIXEL_HEX_Y0_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX_Y0_R_POS))

/* 0xB20 : awb3_sta_19 */
#define ISP_AWB3_STA_19_OFFSET                                  (0xB20)
#define ISP_AWB3_R_RATIO_GRID0U_R                               ISP_AWB3_R_RATIO_GRID0U_R
#define ISP_AWB3_R_RATIO_GRID0U_R_POS                           (0U)
#define ISP_AWB3_R_RATIO_GRID0U_R_LEN                           (8U)
#define ISP_AWB3_R_RATIO_GRID0U_R_MSK                           (((1U<<ISP_AWB3_R_RATIO_GRID0U_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID0U_R_POS)
#define ISP_AWB3_R_RATIO_GRID0U_R_UMSK                          (~(((1U<<ISP_AWB3_R_RATIO_GRID0U_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID0U_R_POS))

/* 0xB24 : awb3_sta_20 */
#define ISP_AWB3_STA_20_OFFSET                                  (0xB24)
#define ISP_AWB3_R_RATIO_GRID0L_R                               ISP_AWB3_R_RATIO_GRID0L_R
#define ISP_AWB3_R_RATIO_GRID0L_R_POS                           (0U)
#define ISP_AWB3_R_RATIO_GRID0L_R_LEN                           (32U)
#define ISP_AWB3_R_RATIO_GRID0L_R_MSK                           (((1U<<ISP_AWB3_R_RATIO_GRID0L_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID0L_R_POS)
#define ISP_AWB3_R_RATIO_GRID0L_R_UMSK                          (~(((1U<<ISP_AWB3_R_RATIO_GRID0L_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID0L_R_POS))

/* 0xB28 : awb3_sta_21 */
#define ISP_AWB3_STA_21_OFFSET                                  (0xB28)
#define ISP_AWB3_B_RATIO_GRID0U_R                               ISP_AWB3_B_RATIO_GRID0U_R
#define ISP_AWB3_B_RATIO_GRID0U_R_POS                           (0U)
#define ISP_AWB3_B_RATIO_GRID0U_R_LEN                           (8U)
#define ISP_AWB3_B_RATIO_GRID0U_R_MSK                           (((1U<<ISP_AWB3_B_RATIO_GRID0U_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID0U_R_POS)
#define ISP_AWB3_B_RATIO_GRID0U_R_UMSK                          (~(((1U<<ISP_AWB3_B_RATIO_GRID0U_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID0U_R_POS))

/* 0xB2C : awb3_sta_22 */
#define ISP_AWB3_STA_22_OFFSET                                  (0xB2C)
#define ISP_AWB3_B_RATIO_GRID0L_R                               ISP_AWB3_B_RATIO_GRID0L_R
#define ISP_AWB3_B_RATIO_GRID0L_R_POS                           (0U)
#define ISP_AWB3_B_RATIO_GRID0L_R_LEN                           (32U)
#define ISP_AWB3_B_RATIO_GRID0L_R_MSK                           (((1U<<ISP_AWB3_B_RATIO_GRID0L_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID0L_R_POS)
#define ISP_AWB3_B_RATIO_GRID0L_R_UMSK                          (~(((1U<<ISP_AWB3_B_RATIO_GRID0L_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID0L_R_POS))

/* 0xB30 : awb3_sta_23 */
#define ISP_AWB3_STA_23_OFFSET                                  (0xB30)
#define ISP_AWB3_WT_GRID0_R                                     ISP_AWB3_WT_GRID0_R
#define ISP_AWB3_WT_GRID0_R_POS                                 (0U)
#define ISP_AWB3_WT_GRID0_R_LEN                                 (28U)
#define ISP_AWB3_WT_GRID0_R_MSK                                 (((1U<<ISP_AWB3_WT_GRID0_R_LEN)-1)<<ISP_AWB3_WT_GRID0_R_POS)
#define ISP_AWB3_WT_GRID0_R_UMSK                                (~(((1U<<ISP_AWB3_WT_GRID0_R_LEN)-1)<<ISP_AWB3_WT_GRID0_R_POS))

/* 0xB34 : awb3_sta_24 */
#define ISP_AWB3_STA_24_OFFSET                                  (0xB34)
#define ISP_AWB3_PIXEL_GRID0_R                                  ISP_AWB3_PIXEL_GRID0_R
#define ISP_AWB3_PIXEL_GRID0_R_POS                              (0U)
#define ISP_AWB3_PIXEL_GRID0_R_LEN                              (20U)
#define ISP_AWB3_PIXEL_GRID0_R_MSK                              (((1U<<ISP_AWB3_PIXEL_GRID0_R_LEN)-1)<<ISP_AWB3_PIXEL_GRID0_R_POS)
#define ISP_AWB3_PIXEL_GRID0_R_UMSK                             (~(((1U<<ISP_AWB3_PIXEL_GRID0_R_LEN)-1)<<ISP_AWB3_PIXEL_GRID0_R_POS))

/* 0xB54 : awb3_sta_32 */
#define ISP_AWB3_STA_32_OFFSET                                  (0xB54)
#define ISP_AWB3_R_SUM1L_R                                      ISP_AWB3_R_SUM1L_R
#define ISP_AWB3_R_SUM1L_R_POS                                  (0U)
#define ISP_AWB3_R_SUM1L_R_LEN                                  (32U)
#define ISP_AWB3_R_SUM1L_R_MSK                                  (((1U<<ISP_AWB3_R_SUM1L_R_LEN)-1)<<ISP_AWB3_R_SUM1L_R_POS)
#define ISP_AWB3_R_SUM1L_R_UMSK                                 (~(((1U<<ISP_AWB3_R_SUM1L_R_LEN)-1)<<ISP_AWB3_R_SUM1L_R_POS))

/* 0xB5C : awb3_sta_34 */
#define ISP_AWB3_STA_34_OFFSET                                  (0xB5C)
#define ISP_AWB3_G_SUM1L_R                                      ISP_AWB3_G_SUM1L_R
#define ISP_AWB3_G_SUM1L_R_POS                                  (0U)
#define ISP_AWB3_G_SUM1L_R_LEN                                  (32U)
#define ISP_AWB3_G_SUM1L_R_MSK                                  (((1U<<ISP_AWB3_G_SUM1L_R_LEN)-1)<<ISP_AWB3_G_SUM1L_R_POS)
#define ISP_AWB3_G_SUM1L_R_UMSK                                 (~(((1U<<ISP_AWB3_G_SUM1L_R_LEN)-1)<<ISP_AWB3_G_SUM1L_R_POS))

/* 0xB64 : awb3_sta_36 */
#define ISP_AWB3_STA_36_OFFSET                                  (0xB64)
#define ISP_AWB3_B_SUM1L_R                                      ISP_AWB3_B_SUM1L_R
#define ISP_AWB3_B_SUM1L_R_POS                                  (0U)
#define ISP_AWB3_B_SUM1L_R_LEN                                  (32U)
#define ISP_AWB3_B_SUM1L_R_MSK                                  (((1U<<ISP_AWB3_B_SUM1L_R_LEN)-1)<<ISP_AWB3_B_SUM1L_R_POS)
#define ISP_AWB3_B_SUM1L_R_UMSK                                 (~(((1U<<ISP_AWB3_B_SUM1L_R_LEN)-1)<<ISP_AWB3_B_SUM1L_R_POS))

/* 0xB6C : awb3_sta_38 */
#define ISP_AWB3_STA_38_OFFSET                                  (0xB6C)
#define ISP_AWB3_R_SUM_HEX1L_R                                  ISP_AWB3_R_SUM_HEX1L_R
#define ISP_AWB3_R_SUM_HEX1L_R_POS                              (0U)
#define ISP_AWB3_R_SUM_HEX1L_R_LEN                              (32U)
#define ISP_AWB3_R_SUM_HEX1L_R_MSK                              (((1U<<ISP_AWB3_R_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_R_SUM_HEX1L_R_POS)
#define ISP_AWB3_R_SUM_HEX1L_R_UMSK                             (~(((1U<<ISP_AWB3_R_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_R_SUM_HEX1L_R_POS))

/* 0xB74 : awb3_sta_40 */
#define ISP_AWB3_STA_40_OFFSET                                  (0xB74)
#define ISP_AWB3_G_SUM_HEX1L_R                                  ISP_AWB3_G_SUM_HEX1L_R
#define ISP_AWB3_G_SUM_HEX1L_R_POS                              (0U)
#define ISP_AWB3_G_SUM_HEX1L_R_LEN                              (32U)
#define ISP_AWB3_G_SUM_HEX1L_R_MSK                              (((1U<<ISP_AWB3_G_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_G_SUM_HEX1L_R_POS)
#define ISP_AWB3_G_SUM_HEX1L_R_UMSK                             (~(((1U<<ISP_AWB3_G_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_G_SUM_HEX1L_R_POS))

/* 0xB7C : awb3_sta_42 */
#define ISP_AWB3_STA_42_OFFSET                                  (0xB7C)
#define ISP_AWB3_B_SUM_HEX1L_R                                  ISP_AWB3_B_SUM_HEX1L_R
#define ISP_AWB3_B_SUM_HEX1L_R_POS                              (0U)
#define ISP_AWB3_B_SUM_HEX1L_R_LEN                              (32U)
#define ISP_AWB3_B_SUM_HEX1L_R_MSK                              (((1U<<ISP_AWB3_B_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_B_SUM_HEX1L_R_POS)
#define ISP_AWB3_B_SUM_HEX1L_R_UMSK                             (~(((1U<<ISP_AWB3_B_SUM_HEX1L_R_LEN)-1)<<ISP_AWB3_B_SUM_HEX1L_R_POS))

/* 0xB80 : awb3_sta_43 */
#define ISP_AWB3_STA_43_OFFSET                                  (0xB80)
#define ISP_AWB3_PIXEL_HEX1_R                                   ISP_AWB3_PIXEL_HEX1_R
#define ISP_AWB3_PIXEL_HEX1_R_POS                               (0U)
#define ISP_AWB3_PIXEL_HEX1_R_LEN                               (20U)
#define ISP_AWB3_PIXEL_HEX1_R_MSK                               (((1U<<ISP_AWB3_PIXEL_HEX1_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX1_R_POS)
#define ISP_AWB3_PIXEL_HEX1_R_UMSK                              (~(((1U<<ISP_AWB3_PIXEL_HEX1_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX1_R_POS))

/* 0xB84 : awb3_sta_44 */
#define ISP_AWB3_STA_44_OFFSET                                  (0xB84)
#define ISP_AWB3_R_RATIO_HEX_Y1U_R                              ISP_AWB3_R_RATIO_HEX_Y1U_R
#define ISP_AWB3_R_RATIO_HEX_Y1U_R_POS                          (0U)
#define ISP_AWB3_R_RATIO_HEX_Y1U_R_LEN                          (8U)
#define ISP_AWB3_R_RATIO_HEX_Y1U_R_MSK                          (((1U<<ISP_AWB3_R_RATIO_HEX_Y1U_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y1U_R_POS)
#define ISP_AWB3_R_RATIO_HEX_Y1U_R_UMSK                         (~(((1U<<ISP_AWB3_R_RATIO_HEX_Y1U_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y1U_R_POS))

/* 0xB88 : awb3_sta_45 */
#define ISP_AWB3_STA_45_OFFSET                                  (0xB88)
#define ISP_AWB3_R_RATIO_HEX_Y1L_R                              ISP_AWB3_R_RATIO_HEX_Y1L_R
#define ISP_AWB3_R_RATIO_HEX_Y1L_R_POS                          (0U)
#define ISP_AWB3_R_RATIO_HEX_Y1L_R_LEN                          (32U)
#define ISP_AWB3_R_RATIO_HEX_Y1L_R_MSK                          (((1U<<ISP_AWB3_R_RATIO_HEX_Y1L_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y1L_R_POS)
#define ISP_AWB3_R_RATIO_HEX_Y1L_R_UMSK                         (~(((1U<<ISP_AWB3_R_RATIO_HEX_Y1L_R_LEN)-1)<<ISP_AWB3_R_RATIO_HEX_Y1L_R_POS))

/* 0xB8C : awb3_sta_46 */
#define ISP_AWB3_STA_46_OFFSET                                  (0xB8C)
#define ISP_AWB3_B_RATIO_HEX_Y1U_R                              ISP_AWB3_B_RATIO_HEX_Y1U_R
#define ISP_AWB3_B_RATIO_HEX_Y1U_R_POS                          (0U)
#define ISP_AWB3_B_RATIO_HEX_Y1U_R_LEN                          (8U)
#define ISP_AWB3_B_RATIO_HEX_Y1U_R_MSK                          (((1U<<ISP_AWB3_B_RATIO_HEX_Y1U_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y1U_R_POS)
#define ISP_AWB3_B_RATIO_HEX_Y1U_R_UMSK                         (~(((1U<<ISP_AWB3_B_RATIO_HEX_Y1U_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y1U_R_POS))

/* 0xB90 : awb3_sta_47 */
#define ISP_AWB3_STA_47_OFFSET                                  (0xB90)
#define ISP_AWB3_B_RATIO_HEX_Y1L_R                              ISP_AWB3_B_RATIO_HEX_Y1L_R
#define ISP_AWB3_B_RATIO_HEX_Y1L_R_POS                          (0U)
#define ISP_AWB3_B_RATIO_HEX_Y1L_R_LEN                          (32U)
#define ISP_AWB3_B_RATIO_HEX_Y1L_R_MSK                          (((1U<<ISP_AWB3_B_RATIO_HEX_Y1L_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y1L_R_POS)
#define ISP_AWB3_B_RATIO_HEX_Y1L_R_UMSK                         (~(((1U<<ISP_AWB3_B_RATIO_HEX_Y1L_R_LEN)-1)<<ISP_AWB3_B_RATIO_HEX_Y1L_R_POS))

/* 0xB94 : awb3_sta_48 */
#define ISP_AWB3_STA_48_OFFSET                                  (0xB94)
#define ISP_AWB3_WT_HEX_Y1_R                                    ISP_AWB3_WT_HEX_Y1_R
#define ISP_AWB3_WT_HEX_Y1_R_POS                                (0U)
#define ISP_AWB3_WT_HEX_Y1_R_LEN                                (28U)
#define ISP_AWB3_WT_HEX_Y1_R_MSK                                (((1U<<ISP_AWB3_WT_HEX_Y1_R_LEN)-1)<<ISP_AWB3_WT_HEX_Y1_R_POS)
#define ISP_AWB3_WT_HEX_Y1_R_UMSK                               (~(((1U<<ISP_AWB3_WT_HEX_Y1_R_LEN)-1)<<ISP_AWB3_WT_HEX_Y1_R_POS))

/* 0xB98 : awb3_sta_49 */
#define ISP_AWB3_STA_49_OFFSET                                  (0xB98)
#define ISP_AWB3_PIXEL_HEX_Y1_R                                 ISP_AWB3_PIXEL_HEX_Y1_R
#define ISP_AWB3_PIXEL_HEX_Y1_R_POS                             (0U)
#define ISP_AWB3_PIXEL_HEX_Y1_R_LEN                             (20U)
#define ISP_AWB3_PIXEL_HEX_Y1_R_MSK                             (((1U<<ISP_AWB3_PIXEL_HEX_Y1_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX_Y1_R_POS)
#define ISP_AWB3_PIXEL_HEX_Y1_R_UMSK                            (~(((1U<<ISP_AWB3_PIXEL_HEX_Y1_R_LEN)-1)<<ISP_AWB3_PIXEL_HEX_Y1_R_POS))

/* 0xB9C : awb3_sta_50 */
#define ISP_AWB3_STA_50_OFFSET                                  (0xB9C)
#define ISP_AWB3_R_RATIO_GRID1U_R                               ISP_AWB3_R_RATIO_GRID1U_R
#define ISP_AWB3_R_RATIO_GRID1U_R_POS                           (0U)
#define ISP_AWB3_R_RATIO_GRID1U_R_LEN                           (8U)
#define ISP_AWB3_R_RATIO_GRID1U_R_MSK                           (((1U<<ISP_AWB3_R_RATIO_GRID1U_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID1U_R_POS)
#define ISP_AWB3_R_RATIO_GRID1U_R_UMSK                          (~(((1U<<ISP_AWB3_R_RATIO_GRID1U_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID1U_R_POS))

/* 0xBA0 : awb3_sta_51 */
#define ISP_AWB3_STA_51_OFFSET                                  (0xBA0)
#define ISP_AWB3_R_RATIO_GRID1L_R                               ISP_AWB3_R_RATIO_GRID1L_R
#define ISP_AWB3_R_RATIO_GRID1L_R_POS                           (0U)
#define ISP_AWB3_R_RATIO_GRID1L_R_LEN                           (32U)
#define ISP_AWB3_R_RATIO_GRID1L_R_MSK                           (((1U<<ISP_AWB3_R_RATIO_GRID1L_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID1L_R_POS)
#define ISP_AWB3_R_RATIO_GRID1L_R_UMSK                          (~(((1U<<ISP_AWB3_R_RATIO_GRID1L_R_LEN)-1)<<ISP_AWB3_R_RATIO_GRID1L_R_POS))

/* 0xBA4 : awb3_sta_52 */
#define ISP_AWB3_STA_52_OFFSET                                  (0xBA4)
#define ISP_AWB3_B_RATIO_GRID1U_R                               ISP_AWB3_B_RATIO_GRID1U_R
#define ISP_AWB3_B_RATIO_GRID1U_R_POS                           (0U)
#define ISP_AWB3_B_RATIO_GRID1U_R_LEN                           (8U)
#define ISP_AWB3_B_RATIO_GRID1U_R_MSK                           (((1U<<ISP_AWB3_B_RATIO_GRID1U_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID1U_R_POS)
#define ISP_AWB3_B_RATIO_GRID1U_R_UMSK                          (~(((1U<<ISP_AWB3_B_RATIO_GRID1U_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID1U_R_POS))

/* 0xBA8 : awb3_sta_53 */
#define ISP_AWB3_STA_53_OFFSET                                  (0xBA8)
#define ISP_AWB3_B_RATIO_GRID1L_R                               ISP_AWB3_B_RATIO_GRID1L_R
#define ISP_AWB3_B_RATIO_GRID1L_R_POS                           (0U)
#define ISP_AWB3_B_RATIO_GRID1L_R_LEN                           (32U)
#define ISP_AWB3_B_RATIO_GRID1L_R_MSK                           (((1U<<ISP_AWB3_B_RATIO_GRID1L_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID1L_R_POS)
#define ISP_AWB3_B_RATIO_GRID1L_R_UMSK                          (~(((1U<<ISP_AWB3_B_RATIO_GRID1L_R_LEN)-1)<<ISP_AWB3_B_RATIO_GRID1L_R_POS))

/* 0xBAC : awb3_sta_54 */
#define ISP_AWB3_STA_54_OFFSET                                  (0xBAC)
#define ISP_AWB3_WT_GRID1_R                                     ISP_AWB3_WT_GRID1_R
#define ISP_AWB3_WT_GRID1_R_POS                                 (0U)
#define ISP_AWB3_WT_GRID1_R_LEN                                 (28U)
#define ISP_AWB3_WT_GRID1_R_MSK                                 (((1U<<ISP_AWB3_WT_GRID1_R_LEN)-1)<<ISP_AWB3_WT_GRID1_R_POS)
#define ISP_AWB3_WT_GRID1_R_UMSK                                (~(((1U<<ISP_AWB3_WT_GRID1_R_LEN)-1)<<ISP_AWB3_WT_GRID1_R_POS))

/* 0xBB0 : awb3_sta_55 */
#define ISP_AWB3_STA_55_OFFSET                                  (0xBB0)
#define ISP_AWB3_PIXEL_GRID1_R                                  ISP_AWB3_PIXEL_GRID1_R
#define ISP_AWB3_PIXEL_GRID1_R_POS                              (0U)
#define ISP_AWB3_PIXEL_GRID1_R_LEN                              (20U)
#define ISP_AWB3_PIXEL_GRID1_R_MSK                              (((1U<<ISP_AWB3_PIXEL_GRID1_R_LEN)-1)<<ISP_AWB3_PIXEL_GRID1_R_POS)
#define ISP_AWB3_PIXEL_GRID1_R_UMSK                             (~(((1U<<ISP_AWB3_PIXEL_GRID1_R_LEN)-1)<<ISP_AWB3_PIXEL_GRID1_R_POS))

/* 0xBCC : awb3_status_0 */
#define ISP_AWB3_STATUS_0_OFFSET                                (0xBCC)
#define ISP_AWB3_W_CNT_R                                        ISP_AWB3_W_CNT_R
#define ISP_AWB3_W_CNT_R_POS                                    (0U)
#define ISP_AWB3_W_CNT_R_LEN                                    (5U)
#define ISP_AWB3_W_CNT_R_MSK                                    (((1U<<ISP_AWB3_W_CNT_R_LEN)-1)<<ISP_AWB3_W_CNT_R_POS)
#define ISP_AWB3_W_CNT_R_UMSK                                   (~(((1U<<ISP_AWB3_W_CNT_R_LEN)-1)<<ISP_AWB3_W_CNT_R_POS))
#define ISP_AWB3_BUF_IDX_R                                      ISP_AWB3_BUF_IDX_R
#define ISP_AWB3_BUF_IDX_R_POS                                  (8U)
#define ISP_AWB3_BUF_IDX_R_LEN                                  (1U)
#define ISP_AWB3_BUF_IDX_R_MSK                                  (((1U<<ISP_AWB3_BUF_IDX_R_LEN)-1)<<ISP_AWB3_BUF_IDX_R_POS)
#define ISP_AWB3_BUF_IDX_R_UMSK                                 (~(((1U<<ISP_AWB3_BUF_IDX_R_LEN)-1)<<ISP_AWB3_BUF_IDX_R_POS))

/* 0xC30 : bayer2rgb_0 */
#define ISP_BAYER2RGB_0_OFFSET                                  (0xC30)
#define ISP_CR_RGB2BAYER_MODE                                   ISP_CR_RGB2BAYER_MODE
#define ISP_CR_RGB2BAYER_MODE_POS                               (0U)
#define ISP_CR_RGB2BAYER_MODE_LEN                               (2U)
#define ISP_CR_RGB2BAYER_MODE_MSK                               (((1U<<ISP_CR_RGB2BAYER_MODE_LEN)-1)<<ISP_CR_RGB2BAYER_MODE_POS)
#define ISP_CR_RGB2BAYER_MODE_UMSK                              (~(((1U<<ISP_CR_RGB2BAYER_MODE_LEN)-1)<<ISP_CR_RGB2BAYER_MODE_POS))

/* 0xC40 : ae_roi_x */
#define ISP_AE_ROI_X_OFFSET                                     (0xC40)
#define ISP_AE_ROI_START_X                                      ISP_AE_ROI_START_X
#define ISP_AE_ROI_START_X_POS                                  (0U)
#define ISP_AE_ROI_START_X_LEN                                  (12U)
#define ISP_AE_ROI_START_X_MSK                                  (((1U<<ISP_AE_ROI_START_X_LEN)-1)<<ISP_AE_ROI_START_X_POS)
#define ISP_AE_ROI_START_X_UMSK                                 (~(((1U<<ISP_AE_ROI_START_X_LEN)-1)<<ISP_AE_ROI_START_X_POS))
#define ISP_AE_ROI_WIDTH                                        ISP_AE_ROI_WIDTH
#define ISP_AE_ROI_WIDTH_POS                                    (16U)
#define ISP_AE_ROI_WIDTH_LEN                                    (12U)
#define ISP_AE_ROI_WIDTH_MSK                                    (((1U<<ISP_AE_ROI_WIDTH_LEN)-1)<<ISP_AE_ROI_WIDTH_POS)
#define ISP_AE_ROI_WIDTH_UMSK                                   (~(((1U<<ISP_AE_ROI_WIDTH_LEN)-1)<<ISP_AE_ROI_WIDTH_POS))

/* 0xC44 : ae_roi_y */
#define ISP_AE_ROI_Y_OFFSET                                     (0xC44)
#define ISP_AE_ROI_START_Y                                      ISP_AE_ROI_START_Y
#define ISP_AE_ROI_START_Y_POS                                  (0U)
#define ISP_AE_ROI_START_Y_LEN                                  (11U)
#define ISP_AE_ROI_START_Y_MSK                                  (((1U<<ISP_AE_ROI_START_Y_LEN)-1)<<ISP_AE_ROI_START_Y_POS)
#define ISP_AE_ROI_START_Y_UMSK                                 (~(((1U<<ISP_AE_ROI_START_Y_LEN)-1)<<ISP_AE_ROI_START_Y_POS))
#define ISP_AE_ROI_HEIGHT                                       ISP_AE_ROI_HEIGHT
#define ISP_AE_ROI_HEIGHT_POS                                   (16U)
#define ISP_AE_ROI_HEIGHT_LEN                                   (11U)
#define ISP_AE_ROI_HEIGHT_MSK                                   (((1U<<ISP_AE_ROI_HEIGHT_LEN)-1)<<ISP_AE_ROI_HEIGHT_POS)
#define ISP_AE_ROI_HEIGHT_UMSK                                  (~(((1U<<ISP_AE_ROI_HEIGHT_LEN)-1)<<ISP_AE_ROI_HEIGHT_POS))

/* 0xC48 : ae_image_size */
#define ISP_AE_IMAGE_SIZE_OFFSET                                (0xC48)
#define ISP_AE_IMAGE_WIDTH                                      ISP_AE_IMAGE_WIDTH
#define ISP_AE_IMAGE_WIDTH_POS                                  (0U)
#define ISP_AE_IMAGE_WIDTH_LEN                                  (12U)
#define ISP_AE_IMAGE_WIDTH_MSK                                  (((1U<<ISP_AE_IMAGE_WIDTH_LEN)-1)<<ISP_AE_IMAGE_WIDTH_POS)
#define ISP_AE_IMAGE_WIDTH_UMSK                                 (~(((1U<<ISP_AE_IMAGE_WIDTH_LEN)-1)<<ISP_AE_IMAGE_WIDTH_POS))
#define ISP_AE_IMAGE_HEIGHT                                     ISP_AE_IMAGE_HEIGHT
#define ISP_AE_IMAGE_HEIGHT_POS                                 (16U)
#define ISP_AE_IMAGE_HEIGHT_LEN                                 (11U)
#define ISP_AE_IMAGE_HEIGHT_MSK                                 (((1U<<ISP_AE_IMAGE_HEIGHT_LEN)-1)<<ISP_AE_IMAGE_HEIGHT_POS)
#define ISP_AE_IMAGE_HEIGHT_UMSK                                (~(((1U<<ISP_AE_IMAGE_HEIGHT_LEN)-1)<<ISP_AE_IMAGE_HEIGHT_POS))

/* 0xC4C : ae_grid_size */
#define ISP_AE_GRID_SIZE_OFFSET                                 (0xC4C)
#define ISP_AE_GRID_WIDTH                                       ISP_AE_GRID_WIDTH
#define ISP_AE_GRID_WIDTH_POS                                   (0U)
#define ISP_AE_GRID_WIDTH_LEN                                   (8U)
#define ISP_AE_GRID_WIDTH_MSK                                   (((1U<<ISP_AE_GRID_WIDTH_LEN)-1)<<ISP_AE_GRID_WIDTH_POS)
#define ISP_AE_GRID_WIDTH_UMSK                                  (~(((1U<<ISP_AE_GRID_WIDTH_LEN)-1)<<ISP_AE_GRID_WIDTH_POS))
#define ISP_AE_GRID_HEIGHT                                      ISP_AE_GRID_HEIGHT
#define ISP_AE_GRID_HEIGHT_POS                                  (8U)
#define ISP_AE_GRID_HEIGHT_LEN                                  (8U)
#define ISP_AE_GRID_HEIGHT_MSK                                  (((1U<<ISP_AE_GRID_HEIGHT_LEN)-1)<<ISP_AE_GRID_HEIGHT_POS)
#define ISP_AE_GRID_HEIGHT_UMSK                                 (~(((1U<<ISP_AE_GRID_HEIGHT_LEN)-1)<<ISP_AE_GRID_HEIGHT_POS))
#define ISP_AE_GRID_Y_MAX                                       ISP_AE_GRID_Y_MAX
#define ISP_AE_GRID_Y_MAX_POS                                   (16U)
#define ISP_AE_GRID_Y_MAX_LEN                                   (4U)
#define ISP_AE_GRID_Y_MAX_MSK                                   (((1U<<ISP_AE_GRID_Y_MAX_LEN)-1)<<ISP_AE_GRID_Y_MAX_POS)
#define ISP_AE_GRID_Y_MAX_UMSK                                  (~(((1U<<ISP_AE_GRID_Y_MAX_LEN)-1)<<ISP_AE_GRID_Y_MAX_POS))
#define ISP_AE_GRID_X_MAX                                       ISP_AE_GRID_X_MAX
#define ISP_AE_GRID_X_MAX_POS                                   (20U)
#define ISP_AE_GRID_X_MAX_LEN                                   (4U)
#define ISP_AE_GRID_X_MAX_MSK                                   (((1U<<ISP_AE_GRID_X_MAX_LEN)-1)<<ISP_AE_GRID_X_MAX_POS)
#define ISP_AE_GRID_X_MAX_UMSK                                  (~(((1U<<ISP_AE_GRID_X_MAX_LEN)-1)<<ISP_AE_GRID_X_MAX_POS))
#define ISP_NEW_AE_STA_ENABLE                                   ISP_NEW_AE_STA_ENABLE
#define ISP_NEW_AE_STA_ENABLE_POS                               (24U)
#define ISP_NEW_AE_STA_ENABLE_LEN                               (1U)
#define ISP_NEW_AE_STA_ENABLE_MSK                               (((1U<<ISP_NEW_AE_STA_ENABLE_LEN)-1)<<ISP_NEW_AE_STA_ENABLE_POS)
#define ISP_NEW_AE_STA_ENABLE_UMSK                              (~(((1U<<ISP_NEW_AE_STA_ENABLE_LEN)-1)<<ISP_NEW_AE_STA_ENABLE_POS))
#define ISP_AE_SWITCH_BANK_EN                                   ISP_AE_SWITCH_BANK_EN
#define ISP_AE_SWITCH_BANK_EN_POS                               (25U)
#define ISP_AE_SWITCH_BANK_EN_LEN                               (1U)
#define ISP_AE_SWITCH_BANK_EN_MSK                               (((1U<<ISP_AE_SWITCH_BANK_EN_LEN)-1)<<ISP_AE_SWITCH_BANK_EN_POS)
#define ISP_AE_SWITCH_BANK_EN_UMSK                              (~(((1U<<ISP_AE_SWITCH_BANK_EN_LEN)-1)<<ISP_AE_SWITCH_BANK_EN_POS))
#define ISP_AE_HIST_EN                                          ISP_AE_HIST_EN
#define ISP_AE_HIST_EN_POS                                      (26U)
#define ISP_AE_HIST_EN_LEN                                      (1U)
#define ISP_AE_HIST_EN_MSK                                      (((1U<<ISP_AE_HIST_EN_LEN)-1)<<ISP_AE_HIST_EN_POS)
#define ISP_AE_HIST_EN_UMSK                                     (~(((1U<<ISP_AE_HIST_EN_LEN)-1)<<ISP_AE_HIST_EN_POS))
#define ISP_AE_HIST_MODE                                        ISP_AE_HIST_MODE
#define ISP_AE_HIST_MODE_POS                                    (27U)
#define ISP_AE_HIST_MODE_LEN                                    (1U)
#define ISP_AE_HIST_MODE_MSK                                    (((1U<<ISP_AE_HIST_MODE_LEN)-1)<<ISP_AE_HIST_MODE_POS)
#define ISP_AE_HIST_MODE_UMSK                                   (~(((1U<<ISP_AE_HIST_MODE_LEN)-1)<<ISP_AE_HIST_MODE_POS))
#define ISP_AE_HIST_ROI_EN                                      ISP_AE_HIST_ROI_EN
#define ISP_AE_HIST_ROI_EN_POS                                  (28U)
#define ISP_AE_HIST_ROI_EN_LEN                                  (1U)
#define ISP_AE_HIST_ROI_EN_MSK                                  (((1U<<ISP_AE_HIST_ROI_EN_LEN)-1)<<ISP_AE_HIST_ROI_EN_POS)
#define ISP_AE_HIST_ROI_EN_UMSK                                 (~(((1U<<ISP_AE_HIST_ROI_EN_LEN)-1)<<ISP_AE_HIST_ROI_EN_POS))

/* 0xC50 : ae_coeff */
#define ISP_AE_COEFF_OFFSET                                     (0xC50)
#define ISP_AE_YSUM_COEFF_R                                     ISP_AE_YSUM_COEFF_R
#define ISP_AE_YSUM_COEFF_R_POS                                 (0U)
#define ISP_AE_YSUM_COEFF_R_LEN                                 (8U)
#define ISP_AE_YSUM_COEFF_R_MSK                                 (((1U<<ISP_AE_YSUM_COEFF_R_LEN)-1)<<ISP_AE_YSUM_COEFF_R_POS)
#define ISP_AE_YSUM_COEFF_R_UMSK                                (~(((1U<<ISP_AE_YSUM_COEFF_R_LEN)-1)<<ISP_AE_YSUM_COEFF_R_POS))
#define ISP_AE_YSUM_COEFF_G                                     ISP_AE_YSUM_COEFF_G
#define ISP_AE_YSUM_COEFF_G_POS                                 (8U)
#define ISP_AE_YSUM_COEFF_G_LEN                                 (8U)
#define ISP_AE_YSUM_COEFF_G_MSK                                 (((1U<<ISP_AE_YSUM_COEFF_G_LEN)-1)<<ISP_AE_YSUM_COEFF_G_POS)
#define ISP_AE_YSUM_COEFF_G_UMSK                                (~(((1U<<ISP_AE_YSUM_COEFF_G_LEN)-1)<<ISP_AE_YSUM_COEFF_G_POS))
#define ISP_AE_YSUM_COEFF_B                                     ISP_AE_YSUM_COEFF_B
#define ISP_AE_YSUM_COEFF_B_POS                                 (16U)
#define ISP_AE_YSUM_COEFF_B_LEN                                 (8U)
#define ISP_AE_YSUM_COEFF_B_MSK                                 (((1U<<ISP_AE_YSUM_COEFF_B_LEN)-1)<<ISP_AE_YSUM_COEFF_B_POS)
#define ISP_AE_YSUM_COEFF_B_UMSK                                (~(((1U<<ISP_AE_YSUM_COEFF_B_LEN)-1)<<ISP_AE_YSUM_COEFF_B_POS))

/* 0xC54 : ae2_roi_x */
#define ISP_AE2_ROI_X_OFFSET                                    (0xC54)
#define ISP_AE_HIST_X_MAX                                       ISP_AE_HIST_X_MAX
#define ISP_AE_HIST_X_MAX_POS                                   (0U)
#define ISP_AE_HIST_X_MAX_LEN                                   (12U)
#define ISP_AE_HIST_X_MAX_MSK                                   (((1U<<ISP_AE_HIST_X_MAX_LEN)-1)<<ISP_AE_HIST_X_MAX_POS)
#define ISP_AE_HIST_X_MAX_UMSK                                  (~(((1U<<ISP_AE_HIST_X_MAX_LEN)-1)<<ISP_AE_HIST_X_MAX_POS))
#define ISP_AE_HIST_X_MIN                                       ISP_AE_HIST_X_MIN
#define ISP_AE_HIST_X_MIN_POS                                   (16U)
#define ISP_AE_HIST_X_MIN_LEN                                   (12U)
#define ISP_AE_HIST_X_MIN_MSK                                   (((1U<<ISP_AE_HIST_X_MIN_LEN)-1)<<ISP_AE_HIST_X_MIN_POS)
#define ISP_AE_HIST_X_MIN_UMSK                                  (~(((1U<<ISP_AE_HIST_X_MIN_LEN)-1)<<ISP_AE_HIST_X_MIN_POS))

/* 0xC58 : ae2_roi_y */
#define ISP_AE2_ROI_Y_OFFSET                                    (0xC58)
#define ISP_AE_HIST_Y_MAX                                       ISP_AE_HIST_Y_MAX
#define ISP_AE_HIST_Y_MAX_POS                                   (0U)
#define ISP_AE_HIST_Y_MAX_LEN                                   (11U)
#define ISP_AE_HIST_Y_MAX_MSK                                   (((1U<<ISP_AE_HIST_Y_MAX_LEN)-1)<<ISP_AE_HIST_Y_MAX_POS)
#define ISP_AE_HIST_Y_MAX_UMSK                                  (~(((1U<<ISP_AE_HIST_Y_MAX_LEN)-1)<<ISP_AE_HIST_Y_MAX_POS))
#define ISP_AE_HIST_Y_MIN                                       ISP_AE_HIST_Y_MIN
#define ISP_AE_HIST_Y_MIN_POS                                   (16U)
#define ISP_AE_HIST_Y_MIN_LEN                                   (11U)
#define ISP_AE_HIST_Y_MIN_MSK                                   (((1U<<ISP_AE_HIST_Y_MIN_LEN)-1)<<ISP_AE_HIST_Y_MIN_POS)
#define ISP_AE_HIST_Y_MIN_UMSK                                  (~(((1U<<ISP_AE_HIST_Y_MIN_LEN)-1)<<ISP_AE_HIST_Y_MIN_POS))

/* 0xC5C : ae_status */
#define ISP_AE_STATUS_OFFSET                                    (0xC5C)
#define ISP_AE_HIST_W_CNT_R                                     ISP_AE_HIST_W_CNT_R
#define ISP_AE_HIST_W_CNT_R_POS                                 (4U)
#define ISP_AE_HIST_W_CNT_R_LEN                                 (5U)
#define ISP_AE_HIST_W_CNT_R_MSK                                 (((1U<<ISP_AE_HIST_W_CNT_R_LEN)-1)<<ISP_AE_HIST_W_CNT_R_POS)
#define ISP_AE_HIST_W_CNT_R_UMSK                                (~(((1U<<ISP_AE_HIST_W_CNT_R_LEN)-1)<<ISP_AE_HIST_W_CNT_R_POS))
#define ISP_BLAE_BUF_IDX_R                                      ISP_BLAE_BUF_IDX_R
#define ISP_BLAE_BUF_IDX_R_POS                                  (16U)
#define ISP_BLAE_BUF_IDX_R_LEN                                  (1U)
#define ISP_BLAE_BUF_IDX_R_MSK                                  (((1U<<ISP_BLAE_BUF_IDX_R_LEN)-1)<<ISP_BLAE_BUF_IDX_R_POS)
#define ISP_BLAE_BUF_IDX_R_UMSK                                 (~(((1U<<ISP_BLAE_BUF_IDX_R_LEN)-1)<<ISP_BLAE_BUF_IDX_R_POS))
#define ISP_BLAE_W_CNT_R                                        ISP_BLAE_W_CNT_R
#define ISP_BLAE_W_CNT_R_POS                                    (20U)
#define ISP_BLAE_W_CNT_R_LEN                                    (5U)
#define ISP_BLAE_W_CNT_R_MSK                                    (((1U<<ISP_BLAE_W_CNT_R_LEN)-1)<<ISP_BLAE_W_CNT_R_POS)
#define ISP_BLAE_W_CNT_R_UMSK                                   (~(((1U<<ISP_BLAE_W_CNT_R_LEN)-1)<<ISP_BLAE_W_CNT_R_POS))

/* 0xC60 : ae_sta_dbg */
#define ISP_AE_STA_DBG_OFFSET                                   (0xC60)
#define ISP_AE_GRID_CNT                                         ISP_AE_GRID_CNT
#define ISP_AE_GRID_CNT_POS                                     (0U)
#define ISP_AE_GRID_CNT_LEN                                     (7U)
#define ISP_AE_GRID_CNT_MSK                                     (((1U<<ISP_AE_GRID_CNT_LEN)-1)<<ISP_AE_GRID_CNT_POS)
#define ISP_AE_GRID_CNT_UMSK                                    (~(((1U<<ISP_AE_GRID_CNT_LEN)-1)<<ISP_AE_GRID_CNT_POS))
#define ISP_AE_STA_DONE                                         ISP_AE_STA_DONE
#define ISP_AE_STA_DONE_POS                                     (7U)
#define ISP_AE_STA_DONE_LEN                                     (1U)
#define ISP_AE_STA_DONE_MSK                                     (((1U<<ISP_AE_STA_DONE_LEN)-1)<<ISP_AE_STA_DONE_POS)
#define ISP_AE_STA_DONE_UMSK                                    (~(((1U<<ISP_AE_STA_DONE_LEN)-1)<<ISP_AE_STA_DONE_POS))
#define ISP_AE_WRITE_DATA_CNT                                   ISP_AE_WRITE_DATA_CNT
#define ISP_AE_WRITE_DATA_CNT_POS                               (8U)
#define ISP_AE_WRITE_DATA_CNT_LEN                               (8U)
#define ISP_AE_WRITE_DATA_CNT_MSK                               (((1U<<ISP_AE_WRITE_DATA_CNT_LEN)-1)<<ISP_AE_WRITE_DATA_CNT_POS)
#define ISP_AE_WRITE_DATA_CNT_UMSK                              (~(((1U<<ISP_AE_WRITE_DATA_CNT_LEN)-1)<<ISP_AE_WRITE_DATA_CNT_POS))
#define ISP_AE_STA_INT_FLAG                                     ISP_AE_STA_INT_FLAG
#define ISP_AE_STA_INT_FLAG_POS                                 (16U)
#define ISP_AE_STA_INT_FLAG_LEN                                 (1U)
#define ISP_AE_STA_INT_FLAG_MSK                                 (((1U<<ISP_AE_STA_INT_FLAG_LEN)-1)<<ISP_AE_STA_INT_FLAG_POS)
#define ISP_AE_STA_INT_FLAG_UMSK                                (~(((1U<<ISP_AE_STA_INT_FLAG_LEN)-1)<<ISP_AE_STA_INT_FLAG_POS))
#define ISP_AE_GRID_CNT_MAX                                     ISP_AE_GRID_CNT_MAX
#define ISP_AE_GRID_CNT_MAX_POS                                 (17U)
#define ISP_AE_GRID_CNT_MAX_LEN                                 (7U)
#define ISP_AE_GRID_CNT_MAX_MSK                                 (((1U<<ISP_AE_GRID_CNT_MAX_LEN)-1)<<ISP_AE_GRID_CNT_MAX_POS)
#define ISP_AE_GRID_CNT_MAX_UMSK                                (~(((1U<<ISP_AE_GRID_CNT_MAX_LEN)-1)<<ISP_AE_GRID_CNT_MAX_POS))
#define ISP_AE_FIFO_FULL                                        ISP_AE_FIFO_FULL
#define ISP_AE_FIFO_FULL_POS                                    (24U)
#define ISP_AE_FIFO_FULL_LEN                                    (1U)
#define ISP_AE_FIFO_FULL_MSK                                    (((1U<<ISP_AE_FIFO_FULL_LEN)-1)<<ISP_AE_FIFO_FULL_POS)
#define ISP_AE_FIFO_FULL_UMSK                                   (~(((1U<<ISP_AE_FIFO_FULL_LEN)-1)<<ISP_AE_FIFO_FULL_POS))
#define ISP_AE_CONTROL_STATUS                                   ISP_AE_CONTROL_STATUS
#define ISP_AE_CONTROL_STATUS_POS                               (25U)
#define ISP_AE_CONTROL_STATUS_LEN                               (4U)
#define ISP_AE_CONTROL_STATUS_MSK                               (((1U<<ISP_AE_CONTROL_STATUS_LEN)-1)<<ISP_AE_CONTROL_STATUS_POS)
#define ISP_AE_CONTROL_STATUS_UMSK                              (~(((1U<<ISP_AE_CONTROL_STATUS_LEN)-1)<<ISP_AE_CONTROL_STATUS_POS))
#define ISP_AE_OPERATION_DONE                                   ISP_AE_OPERATION_DONE
#define ISP_AE_OPERATION_DONE_POS                               (29U)
#define ISP_AE_OPERATION_DONE_LEN                               (1U)
#define ISP_AE_OPERATION_DONE_MSK                               (((1U<<ISP_AE_OPERATION_DONE_LEN)-1)<<ISP_AE_OPERATION_DONE_POS)
#define ISP_AE_OPERATION_DONE_UMSK                              (~(((1U<<ISP_AE_OPERATION_DONE_LEN)-1)<<ISP_AE_OPERATION_DONE_POS))
#define ISP_AE_FIFO_EMPTY                                       ISP_AE_FIFO_EMPTY
#define ISP_AE_FIFO_EMPTY_POS                                   (30U)
#define ISP_AE_FIFO_EMPTY_LEN                                   (1U)
#define ISP_AE_FIFO_EMPTY_MSK                                   (((1U<<ISP_AE_FIFO_EMPTY_LEN)-1)<<ISP_AE_FIFO_EMPTY_POS)
#define ISP_AE_FIFO_EMPTY_UMSK                                  (~(((1U<<ISP_AE_FIFO_EMPTY_LEN)-1)<<ISP_AE_FIFO_EMPTY_POS))

/* 0xF00 : isp_dbg_0 */
#define ISP_DBG_0_OFFSET                                        (0xF00)
#define ISP_CR_INK0_SEL                                         ISP_CR_INK0_SEL
#define ISP_CR_INK0_SEL_POS                                     (0U)
#define ISP_CR_INK0_SEL_LEN                                     (4U)
#define ISP_CR_INK0_SEL_MSK                                     (((1U<<ISP_CR_INK0_SEL_LEN)-1)<<ISP_CR_INK0_SEL_POS)
#define ISP_CR_INK0_SEL_UMSK                                    (~(((1U<<ISP_CR_INK0_SEL_LEN)-1)<<ISP_CR_INK0_SEL_POS))
#define ISP_CR_INK1_SEL                                         ISP_CR_INK1_SEL
#define ISP_CR_INK1_SEL_POS                                     (8U)
#define ISP_CR_INK1_SEL_LEN                                     (4U)
#define ISP_CR_INK1_SEL_MSK                                     (((1U<<ISP_CR_INK1_SEL_LEN)-1)<<ISP_CR_INK1_SEL_POS)
#define ISP_CR_INK1_SEL_UMSK                                    (~(((1U<<ISP_CR_INK1_SEL_LEN)-1)<<ISP_CR_INK1_SEL_POS))

/* 0xFF8 : isp_misc_0 */
#define ISP_MISC_0_OFFSET                                       (0xFF8)
#define ISP_RG_AE_INT_MASK                                      ISP_RG_AE_INT_MASK
#define ISP_RG_AE_INT_MASK_POS                                  (4U)
#define ISP_RG_AE_INT_MASK_LEN                                  (1U)
#define ISP_RG_AE_INT_MASK_MSK                                  (((1U<<ISP_RG_AE_INT_MASK_LEN)-1)<<ISP_RG_AE_INT_MASK_POS)
#define ISP_RG_AE_INT_MASK_UMSK                                 (~(((1U<<ISP_RG_AE_INT_MASK_LEN)-1)<<ISP_RG_AE_INT_MASK_POS))
#define ISP_RG_AE_HIST_INT_MASK                                 ISP_RG_AE_HIST_INT_MASK
#define ISP_RG_AE_HIST_INT_MASK_POS                             (5U)
#define ISP_RG_AE_HIST_INT_MASK_LEN                             (1U)
#define ISP_RG_AE_HIST_INT_MASK_MSK                             (((1U<<ISP_RG_AE_HIST_INT_MASK_LEN)-1)<<ISP_RG_AE_HIST_INT_MASK_POS)
#define ISP_RG_AE_HIST_INT_MASK_UMSK                            (~(((1U<<ISP_RG_AE_HIST_INT_MASK_LEN)-1)<<ISP_RG_AE_HIST_INT_MASK_POS))
#define ISP_RG_AWB3_INT_MASK                                    ISP_RG_AWB3_INT_MASK
#define ISP_RG_AWB3_INT_MASK_POS                                (6U)
#define ISP_RG_AWB3_INT_MASK_LEN                                (1U)
#define ISP_RG_AWB3_INT_MASK_MSK                                (((1U<<ISP_RG_AWB3_INT_MASK_LEN)-1)<<ISP_RG_AWB3_INT_MASK_POS)
#define ISP_RG_AWB3_INT_MASK_UMSK                               (~(((1U<<ISP_RG_AWB3_INT_MASK_LEN)-1)<<ISP_RG_AWB3_INT_MASK_POS))

/* 0xFFC : isp_cfg_0 */
#define ISP_CFG_0_OFFSET                                        (0xFFC)
#define ISP_CR_SW_SHD                                           ISP_CR_SW_SHD
#define ISP_CR_SW_SHD_POS                                       (0U)
#define ISP_CR_SW_SHD_LEN                                       (1U)
#define ISP_CR_SW_SHD_MSK                                       (((1U<<ISP_CR_SW_SHD_LEN)-1)<<ISP_CR_SW_SHD_POS)
#define ISP_CR_SW_SHD_UMSK                                      (~(((1U<<ISP_CR_SW_SHD_LEN)-1)<<ISP_CR_SW_SHD_POS))
#define ISP_RG_AE_INT_CLR                                       ISP_RG_AE_INT_CLR
#define ISP_RG_AE_INT_CLR_POS                                   (4U)
#define ISP_RG_AE_INT_CLR_LEN                                   (1U)
#define ISP_RG_AE_INT_CLR_MSK                                   (((1U<<ISP_RG_AE_INT_CLR_LEN)-1)<<ISP_RG_AE_INT_CLR_POS)
#define ISP_RG_AE_INT_CLR_UMSK                                  (~(((1U<<ISP_RG_AE_INT_CLR_LEN)-1)<<ISP_RG_AE_INT_CLR_POS))
#define ISP_RG_AE_HIST_INT_CLR                                  ISP_RG_AE_HIST_INT_CLR
#define ISP_RG_AE_HIST_INT_CLR_POS                              (5U)
#define ISP_RG_AE_HIST_INT_CLR_LEN                              (1U)
#define ISP_RG_AE_HIST_INT_CLR_MSK                              (((1U<<ISP_RG_AE_HIST_INT_CLR_LEN)-1)<<ISP_RG_AE_HIST_INT_CLR_POS)
#define ISP_RG_AE_HIST_INT_CLR_UMSK                             (~(((1U<<ISP_RG_AE_HIST_INT_CLR_LEN)-1)<<ISP_RG_AE_HIST_INT_CLR_POS))
#define ISP_RG_AWB3_INT_CLR                                     ISP_RG_AWB3_INT_CLR
#define ISP_RG_AWB3_INT_CLR_POS                                 (6U)
#define ISP_RG_AWB3_INT_CLR_LEN                                 (1U)
#define ISP_RG_AWB3_INT_CLR_MSK                                 (((1U<<ISP_RG_AWB3_INT_CLR_LEN)-1)<<ISP_RG_AWB3_INT_CLR_POS)
#define ISP_RG_AWB3_INT_CLR_UMSK                                (~(((1U<<ISP_RG_AWB3_INT_CLR_LEN)-1)<<ISP_RG_AWB3_INT_CLR_POS))


struct  isp_reg {
    /* 0x0  reserved */
    uint8_t RESERVED0x0[1024];

    /* 0x400 : YUVC_A */
    union {
        struct {
            uint32_t Ybrightness                    :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t Ycontrast                      :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t CbSaturation                   :  8; /* [23:16],        r/w,       0x80 */
            uint32_t CrSaturation                   :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } YUVC_A;

    /* 0x404 : YUVC_B */
    union {
        struct {
            uint32_t SpecialMode                    :  3; /* [ 2: 0],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t C444to422Filteringmode         :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t SepiaCb                        :  8; /* [15: 8],        r/w,       0x73 */
            uint32_t SepiaCr                        :  8; /* [23:16],        r/w,       0x89 */
            uint32_t SolarThred                     :  8; /* [31:24],        r/w,       0x80 */
        }BF;
        uint32_t WORD;
    } YUVC_B;

    /* 0x408  reserved */
    uint8_t RESERVED0x408[24];

    /* 0x420 : CROP_0 */
    union {
        struct {
            uint32_t reg_crop_vsync_start           : 15; /* [14: 0],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t reg_crop_vsync_end             : 15; /* [30:16],        r/w,     0x7fff */
            uint32_t reg_crop_enable                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } CROP_0;

    /* 0x424 : CROP_1 */
    union {
        struct {
            uint32_t reg_crop_hsync_start           : 15; /* [14: 0],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t reg_crop_hsync_end             : 15; /* [30:16],        r/w,     0x7fff */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } CROP_1;

    /* 0x428  reserved */
    uint8_t RESERVED0x428[24];

    /* 0x440 : adj_ctrl_0 */
    union {
        struct {
            uint32_t reg_adj_adj_eb                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_adj_y_luma                 :  9; /* [ 9: 1],        r/w,        0x0 */
            uint32_t reg_adj_y_mul0                 : 11; /* [20:10],        r/w,      0x380 */
            uint32_t reg_adj_y_mul1                 : 11; /* [31:21],        r/w,      0x35e */
        }BF;
        uint32_t WORD;
    } adj_ctrl_0;

    /* 0x444 : adjA_ctrl_1 */
    union {
        struct {
            uint32_t reg_adj_y_min                  :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t reg_adj_y_max                  :  8; /* [15: 8],        r/w,       0xeb */
            uint32_t reg_adj_uv_min                 :  8; /* [23:16],        r/w,       0x10 */
            uint32_t reg_adj_uv_max                 :  8; /* [31:24],        r/w,       0xf0 */
        }BF;
        uint32_t WORD;
    } adjA_ctrl_1;

    /* 0x448 : adjA_ctrl_2 */
    union {
        struct {
            uint32_t reserved_0_9                   : 10; /* [ 9: 0],       rsvd,        0x0 */
            uint32_t reg_adj_uv_mul0                : 11; /* [20:10],        r/w,      0x380 */
            uint32_t reg_adj_uv_mul1                : 11; /* [31:21],        r/w,      0x387 */
        }BF;
        uint32_t WORD;
    } adjA_ctrl_2;

    /* 0x44c  reserved */
    uint8_t RESERVED0x44c[52];

    /* 0x480 : ce_config_0 */
    union {
        struct {
            uint32_t rg_ce_pre_0                    : 13; /* [12: 0],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rg_ce_pos_0                    : 13; /* [28:16],        r/w,        0x0 */
            uint32_t reserved_29_30                 :  2; /* [30:29],       rsvd,        0x0 */
            uint32_t rg_ce_en                       :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_0;

    /* 0x484 : ce_config_1 */
    union {
        struct {
            uint32_t rg_ce_pre_1                    : 13; /* [12: 0],        r/w,      0x180 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rg_ce_pos_1                    : 13; /* [28:16],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_1;

    /* 0x488 : ce_config_2 */
    union {
        struct {
            uint32_t rg_ce_pre_2                    : 13; /* [12: 0],        r/w,      0x180 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t rg_ce_pos_2                    : 13; /* [28:16],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_2;

    /* 0x48C : ce_config_3 */
    union {
        struct {
            uint32_t rg_ce_mtx_00                   : 12; /* [11: 0],        r/w,      0x200 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_ce_mtx_01                   : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_3;

    /* 0x490 : ce_config_4 */
    union {
        struct {
            uint32_t rg_ce_mtx_02                   : 12; /* [11: 0],        r/w,      0x2ce */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_ce_mtx_10                   : 12; /* [27:16],        r/w,      0x200 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_4;

    /* 0x494 : ce_config_5 */
    union {
        struct {
            uint32_t rg_ce_mtx_11                   : 12; /* [11: 0],        r/w,      0xf50 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_ce_mtx_12                   : 12; /* [27:16],        r/w,      0xe92 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_5;

    /* 0x498 : ce_config_6 */
    union {
        struct {
            uint32_t rg_ce_mtx_20                   : 12; /* [11: 0],        r/w,      0x38b */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_ce_mtx_21                   : 12; /* [27:16],        r/w,      0x200 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_6;

    /* 0x49C : ce_config_7 */
    union {
        struct {
            uint32_t rg_ce_mtx_22                   : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ce_config_7;

    /* 0x4a0  reserved */
    uint8_t RESERVED0x4a0[1376];

    /* 0xA00 : awb3_config_0 */
    union {
        struct {
            uint32_t awb3_stat_x_max                : 12; /* [11: 0],        r/w,      0x77f */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t awb3_stat_x_min                : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_30                 :  3; /* [30:28],       rsvd,        0x0 */
            uint32_t awb3_stat_en                   :  1; /* [   31],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_0;

    /* 0xA04 : awb3_config_1 */
    union {
        struct {
            uint32_t awb3_stat_y_max                : 11; /* [10: 0],        r/w,      0x437 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t awb3_stat_y_min                : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_1;

    /* 0xA08 : awb3_config_2 */
    union {
        struct {
            uint32_t awb3_b_ratio_ofst              : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t awb3_r_ratio_ofst              : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_30                 :  3; /* [30:28],       rsvd,        0x0 */
            uint32_t awb3_g_divisor                 :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_2;

    /* 0xA0C : awb3_config_3 */
    union {
        struct {
            uint32_t awb3_b_ratio_mult              : 12; /* [11: 0],        r/w,      0x100 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t awb3_r_ratio_mult              : 12; /* [27:16],        r/w,      0x100 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_3;

    /* 0xA10 : awb3_config_4 */
    union {
        struct {
            uint32_t awb3_ratio_sum_max             :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t awb3_ratio_sum_min             :  9; /* [17: 9],        r/w,        0x0 */
            uint32_t awb3_wt_ratio_gain             :  8; /* [25:18],        r/w,        0x0 */
            uint32_t awb3_wt_ratio_en               :  1; /* [   26],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_4;

    /* 0xA14 : awb3_config_5 */
    union {
        struct {
            uint32_t awb3_wt_ratio_00               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_01               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_02               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_03               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_5;

    /* 0xA18 : awb3_config_6 */
    union {
        struct {
            uint32_t awb3_wt_ratio_04               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_05               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_06               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_07               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_6;

    /* 0xA1C : awb3_config_7 */
    union {
        struct {
            uint32_t awb3_wt_ratio_08               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_10               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_11               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_12               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_7;

    /* 0xA20 : awb3_config_8 */
    union {
        struct {
            uint32_t awb3_wt_ratio_13               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_14               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_15               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_16               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_8;

    /* 0xA24 : awb3_config_9 */
    union {
        struct {
            uint32_t awb3_wt_ratio_17               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_18               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_20               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_21               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_9;

    /* 0xA28 : awb3_config_10 */
    union {
        struct {
            uint32_t awb3_wt_ratio_22               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_23               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_24               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_25               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_10;

    /* 0xA2C : awb3_config_11 */
    union {
        struct {
            uint32_t awb3_wt_ratio_26               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_27               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_28               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_30               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_11;

    /* 0xA30 : awb3_config_12 */
    union {
        struct {
            uint32_t awb3_wt_ratio_31               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_32               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_33               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_34               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_12;

    /* 0xA34 : awb3_config_13 */
    union {
        struct {
            uint32_t awb3_wt_ratio_35               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_36               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_37               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_38               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_13;

    /* 0xA38 : awb3_config_14 */
    union {
        struct {
            uint32_t awb3_wt_ratio_40               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_41               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_42               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_43               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_14;

    /* 0xA3C : awb3_config_15 */
    union {
        struct {
            uint32_t awb3_wt_ratio_44               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_45               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_46               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_47               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_15;

    /* 0xA40 : awb3_config_16 */
    union {
        struct {
            uint32_t awb3_wt_ratio_48               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_50               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_51               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_52               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_16;

    /* 0xA44 : awb3_config_17 */
    union {
        struct {
            uint32_t awb3_wt_ratio_53               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_54               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_55               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_56               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_17;

    /* 0xA48 : awb3_config_18 */
    union {
        struct {
            uint32_t awb3_wt_ratio_57               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_58               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_60               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_61               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_18;

    /* 0xA4C : awb3_config_19 */
    union {
        struct {
            uint32_t awb3_wt_ratio_62               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_63               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_64               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_65               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_19;

    /* 0xA50 : awb3_config_20 */
    union {
        struct {
            uint32_t awb3_wt_ratio_66               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_67               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_68               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_70               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_20;

    /* 0xA54 : awb3_config_21 */
    union {
        struct {
            uint32_t awb3_wt_ratio_71               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_72               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_73               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_74               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_21;

    /* 0xA58 : awb3_config_22 */
    union {
        struct {
            uint32_t awb3_wt_ratio_75               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_76               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_77               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_78               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_22;

    /* 0xA5C : awb3_config_23 */
    union {
        struct {
            uint32_t awb3_wt_ratio_80               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_81               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_82               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_83               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_23;

    /* 0xA60 : awb3_config_24 */
    union {
        struct {
            uint32_t awb3_wt_ratio_84               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_85               :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_86               :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_ratio_87               :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_24;

    /* 0xA64 : awb3_config_25 */
    union {
        struct {
            uint32_t awb3_wt_ratio_88               :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_25;

    /* 0xA68 : awb3_config_26 */
    union {
        struct {
            uint32_t awb3_y_r_coeff                 : 12; /* [11: 0],        r/w,       0x4d */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_26;

    /* 0xA6C : awb3_config_27 */
    union {
        struct {
            uint32_t awb3_y_g_coeff                 : 12; /* [11: 0],        r/w,       0x96 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_27;

    /* 0xA70 : awb3_config_28 */
    union {
        struct {
            uint32_t awb3_y_b_coeff                 : 12; /* [11: 0],        r/w,       0x1d */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_28;

    /* 0xA74 : awb3_config_29 */
    union {
        struct {
            uint32_t awb3_wt_y_gain                 :  8; /* [ 7: 0],        r/w,       0x10 */
            uint32_t awb3_wt_y_en                   :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_11                  :  3; /* [11: 9],       rsvd,        0x0 */
            uint32_t awb3_wt_y_use_g                :  1; /* [   12],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_29;

    /* 0xA78 : awb3_config_30 */
    union {
        struct {
            uint32_t awb3_wt_y_00                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_01                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_02                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_03                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_30;

    /* 0xA7C : awb3_config_31 */
    union {
        struct {
            uint32_t awb3_wt_y_04                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_05                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_06                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_07                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_31;

    /* 0xA80 : awb3_config_32 */
    union {
        struct {
            uint32_t awb3_wt_y_08                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_10                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_11                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_12                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_32;

    /* 0xA84 : awb3_config_33 */
    union {
        struct {
            uint32_t awb3_wt_y_13                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_14                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_15                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_16                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_33;

    /* 0xA88 : awb3_config_34 */
    union {
        struct {
            uint32_t awb3_wt_y_17                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_18                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_20                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_21                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_34;

    /* 0xA8C : awb3_config_35 */
    union {
        struct {
            uint32_t awb3_wt_y_22                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_23                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_24                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_25                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_35;

    /* 0xA90 : awb3_config_36 */
    union {
        struct {
            uint32_t awb3_wt_y_26                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_27                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_28                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_30                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_36;

    /* 0xA94 : awb3_config_37 */
    union {
        struct {
            uint32_t awb3_wt_y_31                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_32                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_33                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_34                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_37;

    /* 0xA98 : awb3_config_38 */
    union {
        struct {
            uint32_t awb3_wt_y_35                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_36                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_37                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_38                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_38;

    /* 0xA9C : awb3_config_39 */
    union {
        struct {
            uint32_t awb3_wt_y_40                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_41                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_42                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_43                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_39;

    /* 0xAA0 : awb3_config_40 */
    union {
        struct {
            uint32_t awb3_wt_y_44                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_45                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_46                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_47                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_40;

    /* 0xAA4 : awb3_config_41 */
    union {
        struct {
            uint32_t awb3_wt_y_48                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_50                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_51                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_52                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_41;

    /* 0xAA8 : awb3_config_42 */
    union {
        struct {
            uint32_t awb3_wt_y_53                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_54                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_55                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_56                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_42;

    /* 0xAAC : awb3_config_43 */
    union {
        struct {
            uint32_t awb3_wt_y_57                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_58                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_60                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_61                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_43;

    /* 0xAB0 : awb3_config_44 */
    union {
        struct {
            uint32_t awb3_wt_y_62                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_63                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_64                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_65                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_44;

    /* 0xAB4 : awb3_config_45 */
    union {
        struct {
            uint32_t awb3_wt_y_66                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_67                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_68                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_70                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_45;

    /* 0xAB8 : awb3_config_46 */
    union {
        struct {
            uint32_t awb3_wt_y_71                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_72                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_73                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_74                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_46;

    /* 0xABC : awb3_config_47 */
    union {
        struct {
            uint32_t awb3_wt_y_75                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_76                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_77                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_78                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_47;

    /* 0xAC0 : awb3_config_48 */
    union {
        struct {
            uint32_t awb3_wt_y_80                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_81                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_82                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_83                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_48;

    /* 0xAC4 : awb3_config_49 */
    union {
        struct {
            uint32_t awb3_wt_y_84                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t awb3_wt_y_85                   :  8; /* [15: 8],        r/w,        0x1 */
            uint32_t awb3_wt_y_86                   :  8; /* [23:16],        r/w,        0x1 */
            uint32_t awb3_wt_y_87                   :  8; /* [31:24],        r/w,        0x1 */
        }BF;
        uint32_t WORD;
    } awb3_config_49;

    /* 0xAC8 : awb3_config_50 */
    union {
        struct {
            uint32_t awb3_wt_y_88                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_50;

    /* 0xACC : awb3_config_51 */
    union {
        struct {
            uint32_t awb3_ink_gain                  :  4; /* [ 3: 0],        r/w,        0x1 */
            uint32_t awb3_ink_type                  :  2; /* [ 5: 4],        r/w,        0x0 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t awb3_count_zero_wt             :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_config_51;

    /* 0xad0  reserved */
    uint8_t RESERVED0xad0[8];

    /* 0xAD8 : awb3_sta_1 */
    union {
        struct {
            uint32_t awb3_r_sum0l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_1;

    /* 0xadc  reserved */
    uint8_t RESERVED0xadc[4];

    /* 0xAE0 : awb3_sta_3 */
    union {
        struct {
            uint32_t awb3_g_sum0l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_3;

    /* 0xae4  reserved */
    uint8_t RESERVED0xae4[4];

    /* 0xAE8 : awb3_sta_5 */
    union {
        struct {
            uint32_t awb3_b_sum0l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_5;

    /* 0xaec  reserved */
    uint8_t RESERVED0xaec[4];

    /* 0xAF0 : awb3_sta_7 */
    union {
        struct {
            uint32_t awb3_r_sum_hex0l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_7;

    /* 0xaf4  reserved */
    uint8_t RESERVED0xaf4[4];

    /* 0xAF8 : awb3_sta_9 */
    union {
        struct {
            uint32_t awb3_g_sum_hex0l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_9;

    /* 0xafc  reserved */
    uint8_t RESERVED0xafc[4];

    /* 0xB00 : awb3_sta_11 */
    union {
        struct {
            uint32_t awb3_b_sum_hex0l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_11;

    /* 0xB04 : awb3_sta_12 */
    union {
        struct {
            uint32_t awb3_pixel_hex0_r              : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_12;

    /* 0xB08 : awb3_sta_13 */
    union {
        struct {
            uint32_t awb3_r_ratio_hex_y0u_r         :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_13;

    /* 0xB0C : awb3_sta_14 */
    union {
        struct {
            uint32_t awb3_r_ratio_hex_y0l_r         : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_14;

    /* 0xB10 : awb3_sta_15 */
    union {
        struct {
            uint32_t awb3_b_ratio_hex_y0u_r         :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_15;

    /* 0xB14 : awb3_sta_16 */
    union {
        struct {
            uint32_t awb3_b_ratio_hex_y0l_r         : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_16;

    /* 0xB18 : awb3_sta_17 */
    union {
        struct {
            uint32_t awb3_wt_hex_y0_r               : 28; /* [27: 0],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_17;

    /* 0xB1C : awb3_sta_18 */
    union {
        struct {
            uint32_t awb3_pixel_hex_y0_r            : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_18;

    /* 0xB20 : awb3_sta_19 */
    union {
        struct {
            uint32_t awb3_r_ratio_grid0u_r          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_19;

    /* 0xB24 : awb3_sta_20 */
    union {
        struct {
            uint32_t awb3_r_ratio_grid0l_r          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_20;

    /* 0xB28 : awb3_sta_21 */
    union {
        struct {
            uint32_t awb3_b_ratio_grid0u_r          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_21;

    /* 0xB2C : awb3_sta_22 */
    union {
        struct {
            uint32_t awb3_b_ratio_grid0l_r          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_22;

    /* 0xB30 : awb3_sta_23 */
    union {
        struct {
            uint32_t awb3_wt_grid0_r                : 28; /* [27: 0],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_23;

    /* 0xB34 : awb3_sta_24 */
    union {
        struct {
            uint32_t awb3_pixel_grid0_r             : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_24;

    /* 0xb38  reserved */
    uint8_t RESERVED0xb38[28];

    /* 0xB54 : awb3_sta_32 */
    union {
        struct {
            uint32_t awb3_r_sum1l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_32;

    /* 0xb58  reserved */
    uint8_t RESERVED0xb58[4];

    /* 0xB5C : awb3_sta_34 */
    union {
        struct {
            uint32_t awb3_g_sum1l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_34;

    /* 0xb60  reserved */
    uint8_t RESERVED0xb60[4];

    /* 0xB64 : awb3_sta_36 */
    union {
        struct {
            uint32_t awb3_b_sum1l_r                 : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_36;

    /* 0xb68  reserved */
    uint8_t RESERVED0xb68[4];

    /* 0xB6C : awb3_sta_38 */
    union {
        struct {
            uint32_t awb3_r_sum_hex1l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_38;

    /* 0xb70  reserved */
    uint8_t RESERVED0xb70[4];

    /* 0xB74 : awb3_sta_40 */
    union {
        struct {
            uint32_t awb3_g_sum_hex1l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_40;

    /* 0xb78  reserved */
    uint8_t RESERVED0xb78[4];

    /* 0xB7C : awb3_sta_42 */
    union {
        struct {
            uint32_t awb3_b_sum_hex1l_r             : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_42;

    /* 0xB80 : awb3_sta_43 */
    union {
        struct {
            uint32_t awb3_pixel_hex1_r              : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_43;

    /* 0xB84 : awb3_sta_44 */
    union {
        struct {
            uint32_t awb3_r_ratio_hex_y1u_r         :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_44;

    /* 0xB88 : awb3_sta_45 */
    union {
        struct {
            uint32_t awb3_r_ratio_hex_y1l_r         : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_45;

    /* 0xB8C : awb3_sta_46 */
    union {
        struct {
            uint32_t awb3_b_ratio_hex_y1u_r         :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_46;

    /* 0xB90 : awb3_sta_47 */
    union {
        struct {
            uint32_t awb3_b_ratio_hex_y1l_r         : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_47;

    /* 0xB94 : awb3_sta_48 */
    union {
        struct {
            uint32_t awb3_wt_hex_y1_r               : 28; /* [27: 0],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_48;

    /* 0xB98 : awb3_sta_49 */
    union {
        struct {
            uint32_t awb3_pixel_hex_y1_r            : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_49;

    /* 0xB9C : awb3_sta_50 */
    union {
        struct {
            uint32_t awb3_r_ratio_grid1u_r          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_50;

    /* 0xBA0 : awb3_sta_51 */
    union {
        struct {
            uint32_t awb3_r_ratio_grid1l_r          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_51;

    /* 0xBA4 : awb3_sta_52 */
    union {
        struct {
            uint32_t awb3_b_ratio_grid1u_r          :  8; /* [ 7: 0],          r,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_52;

    /* 0xBA8 : awb3_sta_53 */
    union {
        struct {
            uint32_t awb3_b_ratio_grid1l_r          : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_53;

    /* 0xBAC : awb3_sta_54 */
    union {
        struct {
            uint32_t awb3_wt_grid1_r                : 28; /* [27: 0],          r,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_54;

    /* 0xBB0 : awb3_sta_55 */
    union {
        struct {
            uint32_t awb3_pixel_grid1_r             : 20; /* [19: 0],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_sta_55;

    /* 0xbb4  reserved */
    uint8_t RESERVED0xbb4[24];

    /* 0xBCC : awb3_status_0 */
    union {
        struct {
            uint32_t awb3_w_cnt_r                   :  5; /* [ 4: 0],          r,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t awb3_buf_idx_r                 :  1; /* [    8],          r,        0x0 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } awb3_status_0;

    /* 0xbd0  reserved */
    uint8_t RESERVED0xbd0[96];

    /* 0xC30 : bayer2rgb_0 */
    union {
        struct {
            uint32_t cr_rgb2bayer_mode              :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_31                  : 30; /* [31: 2],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } bayer2rgb_0;

    /* 0xc34  reserved */
    uint8_t RESERVED0xc34[12];

    /* 0xC40 : ae_roi_x */
    union {
        struct {
            uint32_t ae_roi_start_x                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t ae_roi_width                   : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_roi_x;

    /* 0xC44 : ae_roi_y */
    union {
        struct {
            uint32_t ae_roi_start_y                 : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t ae_roi_height                  : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_roi_y;

    /* 0xC48 : ae_image_size */
    union {
        struct {
            uint32_t ae_image_width                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t ae_image_height                : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_image_size;

    /* 0xC4C : ae_grid_size */
    union {
        struct {
            uint32_t ae_grid_width                  :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t ae_grid_height                 :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t ae_grid_y_max                  :  4; /* [19:16],        r/w,        0x0 */
            uint32_t ae_grid_x_max                  :  4; /* [23:20],        r/w,        0x0 */
            uint32_t new_ae_sta_enable              :  1; /* [   24],        r/w,        0x0 */
            uint32_t ae_switch_bank_en              :  1; /* [   25],        r/w,        0x1 */
            uint32_t ae_hist_en                     :  1; /* [   26],        r/w,        0x1 */
            uint32_t ae_hist_mode                   :  1; /* [   27],        r/w,        0x1 */
            uint32_t ae_hist_roi_en                 :  1; /* [   28],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_grid_size;

    /* 0xC50 : ae_coeff */
    union {
        struct {
            uint32_t ae_ysum_coeff_r                :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t ae_ysum_coeff_g                :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t ae_ysum_coeff_b                :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_coeff;

    /* 0xC54 : ae2_roi_x */
    union {
        struct {
            uint32_t ae_hist_x_max                  : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t ae_hist_x_min                  : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae2_roi_x;

    /* 0xC58 : ae2_roi_y */
    union {
        struct {
            uint32_t ae_hist_y_max                  : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t ae_hist_y_min                  : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae2_roi_y;

    /* 0xC5C : ae_status */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t ae_hist_w_cnt_r                :  5; /* [ 8: 4],          r,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t blae_buf_idx_r                 :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_19                 :  3; /* [19:17],       rsvd,        0x0 */
            uint32_t blae_w_cnt_r                   :  5; /* [24:20],          r,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_status;

    /* 0xC60 : ae_sta_dbg */
    union {
        struct {
            uint32_t ae_grid_cnt                    :  7; /* [ 6: 0],          r,        0x0 */
            uint32_t ae_sta_done                    :  1; /* [    7],          r,        0x0 */
            uint32_t ae_write_data_cnt              :  8; /* [15: 8],          r,        0x0 */
            uint32_t ae_sta_int_flag                :  1; /* [   16],          r,        0x0 */
            uint32_t ae_grid_cnt_max                :  7; /* [23:17],          r,        0x0 */
            uint32_t ae_fifo_full                   :  1; /* [   24],          r,        0x0 */
            uint32_t ae_control_status              :  4; /* [28:25],          r,        0x0 */
            uint32_t ae_operation_done              :  1; /* [   29],          r,        0x0 */
            uint32_t ae_fifo_empty                  :  1; /* [   30],          r,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } ae_sta_dbg;

    /* 0xc64  reserved */
    uint8_t RESERVED0xc64[668];

    /* 0xF00 : isp_dbg_0 */
    union {
        struct {
            uint32_t cr_ink0_sel                    :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reserved_4_7                   :  4; /* [ 7: 4],       rsvd,        0x0 */
            uint32_t cr_ink1_sel                    :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } isp_dbg_0;

    /* 0xf04  reserved */
    uint8_t RESERVED0xf04[244];

    /* 0xFF8 : isp_misc_0 */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t rg_ae_int_mask                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t rg_ae_hist_int_mask            :  1; /* [    5],        r/w,        0x0 */
            uint32_t rg_awb3_int_mask               :  1; /* [    6],        r/w,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } isp_misc_0;

    /* 0xFFC : isp_cfg_0 */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1_3                   :  3; /* [ 3: 1],       rsvd,        0x0 */
            uint32_t rg_ae_int_clr                  :  1; /* [    4],        w1p,        0x0 */
            uint32_t rg_ae_hist_int_clr             :  1; /* [    5],        w1p,        0x0 */
            uint32_t rg_awb3_int_clr                :  1; /* [    6],        w1p,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } isp_cfg_0;

};

typedef volatile struct isp_reg isp_reg_t;


#endif  /* __ISP_REG_H__ */
