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

#ifndef  __OSD_DRAW_REG_H__
#define  __OSD_DRAW_REG_H__

#include "ipro7.h"

/* 0x0 : sd_ctrl_000 */
#define OSD_DRAW_SD_CTRL_000_OFFSET                             (0x0)
#define OSD_DRAW_REG_DRAW_EN_L                                  OSD_DRAW_REG_DRAW_EN_L
#define OSD_DRAW_REG_DRAW_EN_L_POS                              (0U)
#define OSD_DRAW_REG_DRAW_EN_L_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_EN_L_MSK                              (((1U<<OSD_DRAW_REG_DRAW_EN_L_LEN)-1)<<OSD_DRAW_REG_DRAW_EN_L_POS)
#define OSD_DRAW_REG_DRAW_EN_L_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_EN_L_LEN)-1)<<OSD_DRAW_REG_DRAW_EN_L_POS))
#define OSD_DRAW_REG_DRAW_TYPE_L                                OSD_DRAW_REG_DRAW_TYPE_L
#define OSD_DRAW_REG_DRAW_TYPE_L_POS                            (16U)
#define OSD_DRAW_REG_DRAW_TYPE_L_LEN                            (8U)
#define OSD_DRAW_REG_DRAW_TYPE_L_MSK                            (((1U<<OSD_DRAW_REG_DRAW_TYPE_L_LEN)-1)<<OSD_DRAW_REG_DRAW_TYPE_L_POS)
#define OSD_DRAW_REG_DRAW_TYPE_L_UMSK                           (~(((1U<<OSD_DRAW_REG_DRAW_TYPE_L_LEN)-1)<<OSD_DRAW_REG_DRAW_TYPE_L_POS))

/* 0x4 : draw_partial_yuv_config0 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG0_OFFSET                (0x4)
#define OSD_DRAW_REG_DRAW_TH_00                                 OSD_DRAW_REG_DRAW_TH_00
#define OSD_DRAW_REG_DRAW_TH_00_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_00_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_00_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_00_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_00_POS)
#define OSD_DRAW_REG_DRAW_TH_00_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_00_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_00_POS))
#define OSD_DRAW_REG_DRAW_V_00                                  OSD_DRAW_REG_DRAW_V_00
#define OSD_DRAW_REG_DRAW_V_00_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_00_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_00_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_00_LEN)-1)<<OSD_DRAW_REG_DRAW_V_00_POS)
#define OSD_DRAW_REG_DRAW_V_00_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_00_LEN)-1)<<OSD_DRAW_REG_DRAW_V_00_POS))
#define OSD_DRAW_REG_DRAW_U_00                                  OSD_DRAW_REG_DRAW_U_00
#define OSD_DRAW_REG_DRAW_U_00_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_00_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_00_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_00_LEN)-1)<<OSD_DRAW_REG_DRAW_U_00_POS)
#define OSD_DRAW_REG_DRAW_U_00_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_00_LEN)-1)<<OSD_DRAW_REG_DRAW_U_00_POS))
#define OSD_DRAW_REG_DRAW_Y_00                                  OSD_DRAW_REG_DRAW_Y_00
#define OSD_DRAW_REG_DRAW_Y_00_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_00_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_00_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_00_POS)
#define OSD_DRAW_REG_DRAW_Y_00_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_00_POS))

/* 0x8 : draw_partial_x_config0 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG0_OFFSET                  (0x8)
#define OSD_DRAW_REG_DRAW_X_MIN_00                              OSD_DRAW_REG_DRAW_X_MIN_00
#define OSD_DRAW_REG_DRAW_X_MIN_00_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_00_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_00_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_00_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_00_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_00_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_00_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_00_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_00                              OSD_DRAW_REG_DRAW_X_MAX_00
#define OSD_DRAW_REG_DRAW_X_MAX_00_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_00_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_00_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_00_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_00_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_00_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_00_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_00_POS))

/* 0xC : draw_partial_y_config0 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG0_OFFSET                  (0xC)
#define OSD_DRAW_REG_DRAW_Y_MIN_00                              OSD_DRAW_REG_DRAW_Y_MIN_00
#define OSD_DRAW_REG_DRAW_Y_MIN_00_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_00_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_00_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_00_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_00_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_00_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_00                              OSD_DRAW_REG_DRAW_Y_MAX_00
#define OSD_DRAW_REG_DRAW_Y_MAX_00_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_00_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_00_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_00_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_00_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_00_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_00_POS))

/* 0x10 : draw_partial_yuv_config1 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG1_OFFSET                (0x10)
#define OSD_DRAW_REG_DRAW_TH_01                                 OSD_DRAW_REG_DRAW_TH_01
#define OSD_DRAW_REG_DRAW_TH_01_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_01_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_01_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_01_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_01_POS)
#define OSD_DRAW_REG_DRAW_TH_01_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_01_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_01_POS))
#define OSD_DRAW_REG_DRAW_V_01                                  OSD_DRAW_REG_DRAW_V_01
#define OSD_DRAW_REG_DRAW_V_01_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_01_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_01_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_01_LEN)-1)<<OSD_DRAW_REG_DRAW_V_01_POS)
#define OSD_DRAW_REG_DRAW_V_01_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_01_LEN)-1)<<OSD_DRAW_REG_DRAW_V_01_POS))
#define OSD_DRAW_REG_DRAW_U_01                                  OSD_DRAW_REG_DRAW_U_01
#define OSD_DRAW_REG_DRAW_U_01_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_01_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_01_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_01_LEN)-1)<<OSD_DRAW_REG_DRAW_U_01_POS)
#define OSD_DRAW_REG_DRAW_U_01_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_01_LEN)-1)<<OSD_DRAW_REG_DRAW_U_01_POS))
#define OSD_DRAW_REG_DRAW_Y_01                                  OSD_DRAW_REG_DRAW_Y_01
#define OSD_DRAW_REG_DRAW_Y_01_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_01_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_01_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_01_POS)
#define OSD_DRAW_REG_DRAW_Y_01_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_01_POS))

/* 0x14 : draw_partial_x_config1 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG1_OFFSET                  (0x14)
#define OSD_DRAW_REG_DRAW_X_MIN_01                              OSD_DRAW_REG_DRAW_X_MIN_01
#define OSD_DRAW_REG_DRAW_X_MIN_01_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_01_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_01_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_01_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_01_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_01_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_01_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_01_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_01                              OSD_DRAW_REG_DRAW_X_MAX_01
#define OSD_DRAW_REG_DRAW_X_MAX_01_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_01_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_01_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_01_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_01_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_01_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_01_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_01_POS))

/* 0x18 : draw_partial_y_config1 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG1_OFFSET                  (0x18)
#define OSD_DRAW_REG_DRAW_Y_MIN_01                              OSD_DRAW_REG_DRAW_Y_MIN_01
#define OSD_DRAW_REG_DRAW_Y_MIN_01_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_01_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_01_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_01_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_01_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_01_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_01                              OSD_DRAW_REG_DRAW_Y_MAX_01
#define OSD_DRAW_REG_DRAW_Y_MAX_01_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_01_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_01_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_01_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_01_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_01_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_01_POS))

/* 0x1C : draw_partial_yuv_config2 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG2_OFFSET                (0x1C)
#define OSD_DRAW_REG_DRAW_TH_02                                 OSD_DRAW_REG_DRAW_TH_02
#define OSD_DRAW_REG_DRAW_TH_02_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_02_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_02_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_02_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_02_POS)
#define OSD_DRAW_REG_DRAW_TH_02_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_02_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_02_POS))
#define OSD_DRAW_REG_DRAW_V_02                                  OSD_DRAW_REG_DRAW_V_02
#define OSD_DRAW_REG_DRAW_V_02_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_02_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_02_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_02_LEN)-1)<<OSD_DRAW_REG_DRAW_V_02_POS)
#define OSD_DRAW_REG_DRAW_V_02_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_02_LEN)-1)<<OSD_DRAW_REG_DRAW_V_02_POS))
#define OSD_DRAW_REG_DRAW_U_02                                  OSD_DRAW_REG_DRAW_U_02
#define OSD_DRAW_REG_DRAW_U_02_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_02_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_02_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_02_LEN)-1)<<OSD_DRAW_REG_DRAW_U_02_POS)
#define OSD_DRAW_REG_DRAW_U_02_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_02_LEN)-1)<<OSD_DRAW_REG_DRAW_U_02_POS))
#define OSD_DRAW_REG_DRAW_Y_02                                  OSD_DRAW_REG_DRAW_Y_02
#define OSD_DRAW_REG_DRAW_Y_02_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_02_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_02_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_02_POS)
#define OSD_DRAW_REG_DRAW_Y_02_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_02_POS))

/* 0x20 : draw_partial_x_config2 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG2_OFFSET                  (0x20)
#define OSD_DRAW_REG_DRAW_X_MIN_02                              OSD_DRAW_REG_DRAW_X_MIN_02
#define OSD_DRAW_REG_DRAW_X_MIN_02_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_02_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_02_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_02_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_02_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_02_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_02_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_02_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_02                              OSD_DRAW_REG_DRAW_X_MAX_02
#define OSD_DRAW_REG_DRAW_X_MAX_02_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_02_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_02_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_02_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_02_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_02_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_02_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_02_POS))

/* 0x24 : draw_partial_y_config2 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG2_OFFSET                  (0x24)
#define OSD_DRAW_REG_DRAW_Y_MIN_02                              OSD_DRAW_REG_DRAW_Y_MIN_02
#define OSD_DRAW_REG_DRAW_Y_MIN_02_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_02_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_02_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_02_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_02_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_02_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_02                              OSD_DRAW_REG_DRAW_Y_MAX_02
#define OSD_DRAW_REG_DRAW_Y_MAX_02_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_02_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_02_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_02_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_02_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_02_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_02_POS))

/* 0x28 : draw_partial_yuv_config3 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG3_OFFSET                (0x28)
#define OSD_DRAW_REG_DRAW_TH_03                                 OSD_DRAW_REG_DRAW_TH_03
#define OSD_DRAW_REG_DRAW_TH_03_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_03_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_03_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_03_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_03_POS)
#define OSD_DRAW_REG_DRAW_TH_03_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_03_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_03_POS))
#define OSD_DRAW_REG_DRAW_V_03                                  OSD_DRAW_REG_DRAW_V_03
#define OSD_DRAW_REG_DRAW_V_03_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_03_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_03_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_03_LEN)-1)<<OSD_DRAW_REG_DRAW_V_03_POS)
#define OSD_DRAW_REG_DRAW_V_03_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_03_LEN)-1)<<OSD_DRAW_REG_DRAW_V_03_POS))
#define OSD_DRAW_REG_DRAW_U_03                                  OSD_DRAW_REG_DRAW_U_03
#define OSD_DRAW_REG_DRAW_U_03_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_03_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_03_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_03_LEN)-1)<<OSD_DRAW_REG_DRAW_U_03_POS)
#define OSD_DRAW_REG_DRAW_U_03_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_03_LEN)-1)<<OSD_DRAW_REG_DRAW_U_03_POS))
#define OSD_DRAW_REG_DRAW_Y_03                                  OSD_DRAW_REG_DRAW_Y_03
#define OSD_DRAW_REG_DRAW_Y_03_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_03_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_03_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_03_POS)
#define OSD_DRAW_REG_DRAW_Y_03_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_03_POS))

/* 0x2C : draw_partial_x_config3 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG3_OFFSET                  (0x2C)
#define OSD_DRAW_REG_DRAW_X_MIN_03                              OSD_DRAW_REG_DRAW_X_MIN_03
#define OSD_DRAW_REG_DRAW_X_MIN_03_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_03_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_03_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_03_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_03_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_03_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_03_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_03_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_03                              OSD_DRAW_REG_DRAW_X_MAX_03
#define OSD_DRAW_REG_DRAW_X_MAX_03_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_03_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_03_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_03_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_03_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_03_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_03_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_03_POS))

/* 0x30 : draw_partial_y_config3 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG3_OFFSET                  (0x30)
#define OSD_DRAW_REG_DRAW_Y_MIN_03                              OSD_DRAW_REG_DRAW_Y_MIN_03
#define OSD_DRAW_REG_DRAW_Y_MIN_03_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_03_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_03_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_03_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_03_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_03_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_03                              OSD_DRAW_REG_DRAW_Y_MAX_03
#define OSD_DRAW_REG_DRAW_Y_MAX_03_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_03_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_03_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_03_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_03_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_03_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_03_POS))

/* 0x34 : draw_partial_yuv_config4 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG4_OFFSET                (0x34)
#define OSD_DRAW_REG_DRAW_TH_04                                 OSD_DRAW_REG_DRAW_TH_04
#define OSD_DRAW_REG_DRAW_TH_04_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_04_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_04_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_04_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_04_POS)
#define OSD_DRAW_REG_DRAW_TH_04_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_04_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_04_POS))
#define OSD_DRAW_REG_DRAW_V_04                                  OSD_DRAW_REG_DRAW_V_04
#define OSD_DRAW_REG_DRAW_V_04_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_04_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_04_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_04_LEN)-1)<<OSD_DRAW_REG_DRAW_V_04_POS)
#define OSD_DRAW_REG_DRAW_V_04_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_04_LEN)-1)<<OSD_DRAW_REG_DRAW_V_04_POS))
#define OSD_DRAW_REG_DRAW_U_04                                  OSD_DRAW_REG_DRAW_U_04
#define OSD_DRAW_REG_DRAW_U_04_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_04_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_04_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_04_LEN)-1)<<OSD_DRAW_REG_DRAW_U_04_POS)
#define OSD_DRAW_REG_DRAW_U_04_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_04_LEN)-1)<<OSD_DRAW_REG_DRAW_U_04_POS))
#define OSD_DRAW_REG_DRAW_Y_04                                  OSD_DRAW_REG_DRAW_Y_04
#define OSD_DRAW_REG_DRAW_Y_04_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_04_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_04_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_04_POS)
#define OSD_DRAW_REG_DRAW_Y_04_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_04_POS))

/* 0x38 : draw_partial_x_config4 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG4_OFFSET                  (0x38)
#define OSD_DRAW_REG_DRAW_X_MIN_04                              OSD_DRAW_REG_DRAW_X_MIN_04
#define OSD_DRAW_REG_DRAW_X_MIN_04_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_04_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_04_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_04_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_04_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_04_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_04_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_04_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_04                              OSD_DRAW_REG_DRAW_X_MAX_04
#define OSD_DRAW_REG_DRAW_X_MAX_04_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_04_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_04_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_04_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_04_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_04_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_04_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_04_POS))

/* 0x3C : draw_partial_y_config4 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG4_OFFSET                  (0x3C)
#define OSD_DRAW_REG_DRAW_Y_MIN_04                              OSD_DRAW_REG_DRAW_Y_MIN_04
#define OSD_DRAW_REG_DRAW_Y_MIN_04_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_04_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_04_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_04_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_04_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_04_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_04                              OSD_DRAW_REG_DRAW_Y_MAX_04
#define OSD_DRAW_REG_DRAW_Y_MAX_04_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_04_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_04_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_04_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_04_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_04_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_04_POS))

/* 0x40 : draw_partial_yuv_config5 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG5_OFFSET                (0x40)
#define OSD_DRAW_REG_DRAW_TH_05                                 OSD_DRAW_REG_DRAW_TH_05
#define OSD_DRAW_REG_DRAW_TH_05_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_05_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_05_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_05_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_05_POS)
#define OSD_DRAW_REG_DRAW_TH_05_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_05_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_05_POS))
#define OSD_DRAW_REG_DRAW_V_05                                  OSD_DRAW_REG_DRAW_V_05
#define OSD_DRAW_REG_DRAW_V_05_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_05_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_05_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_05_LEN)-1)<<OSD_DRAW_REG_DRAW_V_05_POS)
#define OSD_DRAW_REG_DRAW_V_05_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_05_LEN)-1)<<OSD_DRAW_REG_DRAW_V_05_POS))
#define OSD_DRAW_REG_DRAW_U_05                                  OSD_DRAW_REG_DRAW_U_05
#define OSD_DRAW_REG_DRAW_U_05_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_05_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_05_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_05_LEN)-1)<<OSD_DRAW_REG_DRAW_U_05_POS)
#define OSD_DRAW_REG_DRAW_U_05_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_05_LEN)-1)<<OSD_DRAW_REG_DRAW_U_05_POS))
#define OSD_DRAW_REG_DRAW_Y_05                                  OSD_DRAW_REG_DRAW_Y_05
#define OSD_DRAW_REG_DRAW_Y_05_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_05_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_05_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_05_POS)
#define OSD_DRAW_REG_DRAW_Y_05_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_05_POS))

/* 0x44 : draw_partial_x_config5 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG5_OFFSET                  (0x44)
#define OSD_DRAW_REG_DRAW_X_MIN_05                              OSD_DRAW_REG_DRAW_X_MIN_05
#define OSD_DRAW_REG_DRAW_X_MIN_05_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_05_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_05_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_05_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_05_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_05_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_05_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_05_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_05                              OSD_DRAW_REG_DRAW_X_MAX_05
#define OSD_DRAW_REG_DRAW_X_MAX_05_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_05_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_05_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_05_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_05_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_05_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_05_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_05_POS))

/* 0x48 : draw_partial_y_config5 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG5_OFFSET                  (0x48)
#define OSD_DRAW_REG_DRAW_Y_MIN_05                              OSD_DRAW_REG_DRAW_Y_MIN_05
#define OSD_DRAW_REG_DRAW_Y_MIN_05_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_05_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_05_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_05_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_05_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_05_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_05                              OSD_DRAW_REG_DRAW_Y_MAX_05
#define OSD_DRAW_REG_DRAW_Y_MAX_05_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_05_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_05_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_05_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_05_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_05_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_05_POS))

/* 0x4C : draw_partial_yuv_config6 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG6_OFFSET                (0x4C)
#define OSD_DRAW_REG_DRAW_TH_06                                 OSD_DRAW_REG_DRAW_TH_06
#define OSD_DRAW_REG_DRAW_TH_06_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_06_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_06_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_06_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_06_POS)
#define OSD_DRAW_REG_DRAW_TH_06_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_06_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_06_POS))
#define OSD_DRAW_REG_DRAW_V_06                                  OSD_DRAW_REG_DRAW_V_06
#define OSD_DRAW_REG_DRAW_V_06_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_06_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_06_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_06_LEN)-1)<<OSD_DRAW_REG_DRAW_V_06_POS)
#define OSD_DRAW_REG_DRAW_V_06_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_06_LEN)-1)<<OSD_DRAW_REG_DRAW_V_06_POS))
#define OSD_DRAW_REG_DRAW_U_06                                  OSD_DRAW_REG_DRAW_U_06
#define OSD_DRAW_REG_DRAW_U_06_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_06_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_06_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_06_LEN)-1)<<OSD_DRAW_REG_DRAW_U_06_POS)
#define OSD_DRAW_REG_DRAW_U_06_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_06_LEN)-1)<<OSD_DRAW_REG_DRAW_U_06_POS))
#define OSD_DRAW_REG_DRAW_Y_06                                  OSD_DRAW_REG_DRAW_Y_06
#define OSD_DRAW_REG_DRAW_Y_06_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_06_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_06_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_06_POS)
#define OSD_DRAW_REG_DRAW_Y_06_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_06_POS))

/* 0x50 : draw_partial_x_config6 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG6_OFFSET                  (0x50)
#define OSD_DRAW_REG_DRAW_X_MIN_06                              OSD_DRAW_REG_DRAW_X_MIN_06
#define OSD_DRAW_REG_DRAW_X_MIN_06_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_06_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_06_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_06_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_06_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_06_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_06_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_06_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_06                              OSD_DRAW_REG_DRAW_X_MAX_06
#define OSD_DRAW_REG_DRAW_X_MAX_06_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_06_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_06_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_06_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_06_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_06_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_06_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_06_POS))

/* 0x54 : draw_partial_y_config6 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG6_OFFSET                  (0x54)
#define OSD_DRAW_REG_DRAW_Y_MIN_06                              OSD_DRAW_REG_DRAW_Y_MIN_06
#define OSD_DRAW_REG_DRAW_Y_MIN_06_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_06_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_06_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_06_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_06_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_06_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_06                              OSD_DRAW_REG_DRAW_Y_MAX_06
#define OSD_DRAW_REG_DRAW_Y_MAX_06_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_06_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_06_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_06_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_06_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_06_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_06_POS))

/* 0x58 : draw_partial_yuv_config7 */
#define OSD_DRAW_DRAW_PARTIAL_YUV_CONFIG7_OFFSET                (0x58)
#define OSD_DRAW_REG_DRAW_TH_07                                 OSD_DRAW_REG_DRAW_TH_07
#define OSD_DRAW_REG_DRAW_TH_07_POS                             (0U)
#define OSD_DRAW_REG_DRAW_TH_07_LEN                             (8U)
#define OSD_DRAW_REG_DRAW_TH_07_MSK                             (((1U<<OSD_DRAW_REG_DRAW_TH_07_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_07_POS)
#define OSD_DRAW_REG_DRAW_TH_07_UMSK                            (~(((1U<<OSD_DRAW_REG_DRAW_TH_07_LEN)-1)<<OSD_DRAW_REG_DRAW_TH_07_POS))
#define OSD_DRAW_REG_DRAW_V_07                                  OSD_DRAW_REG_DRAW_V_07
#define OSD_DRAW_REG_DRAW_V_07_POS                              (8U)
#define OSD_DRAW_REG_DRAW_V_07_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_V_07_MSK                              (((1U<<OSD_DRAW_REG_DRAW_V_07_LEN)-1)<<OSD_DRAW_REG_DRAW_V_07_POS)
#define OSD_DRAW_REG_DRAW_V_07_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_V_07_LEN)-1)<<OSD_DRAW_REG_DRAW_V_07_POS))
#define OSD_DRAW_REG_DRAW_U_07                                  OSD_DRAW_REG_DRAW_U_07
#define OSD_DRAW_REG_DRAW_U_07_POS                              (16U)
#define OSD_DRAW_REG_DRAW_U_07_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_U_07_MSK                              (((1U<<OSD_DRAW_REG_DRAW_U_07_LEN)-1)<<OSD_DRAW_REG_DRAW_U_07_POS)
#define OSD_DRAW_REG_DRAW_U_07_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_U_07_LEN)-1)<<OSD_DRAW_REG_DRAW_U_07_POS))
#define OSD_DRAW_REG_DRAW_Y_07                                  OSD_DRAW_REG_DRAW_Y_07
#define OSD_DRAW_REG_DRAW_Y_07_POS                              (24U)
#define OSD_DRAW_REG_DRAW_Y_07_LEN                              (8U)
#define OSD_DRAW_REG_DRAW_Y_07_MSK                              (((1U<<OSD_DRAW_REG_DRAW_Y_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_07_POS)
#define OSD_DRAW_REG_DRAW_Y_07_UMSK                             (~(((1U<<OSD_DRAW_REG_DRAW_Y_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_07_POS))

/* 0x5C : draw_partial_x_config7 */
#define OSD_DRAW_DRAW_PARTIAL_X_CONFIG7_OFFSET                  (0x5C)
#define OSD_DRAW_REG_DRAW_X_MIN_07                              OSD_DRAW_REG_DRAW_X_MIN_07
#define OSD_DRAW_REG_DRAW_X_MIN_07_POS                          (0U)
#define OSD_DRAW_REG_DRAW_X_MIN_07_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MIN_07_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MIN_07_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_07_POS)
#define OSD_DRAW_REG_DRAW_X_MIN_07_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MIN_07_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MIN_07_POS))
#define OSD_DRAW_REG_DRAW_X_MAX_07                              OSD_DRAW_REG_DRAW_X_MAX_07
#define OSD_DRAW_REG_DRAW_X_MAX_07_POS                          (16U)
#define OSD_DRAW_REG_DRAW_X_MAX_07_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_X_MAX_07_MSK                          (((1U<<OSD_DRAW_REG_DRAW_X_MAX_07_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_07_POS)
#define OSD_DRAW_REG_DRAW_X_MAX_07_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_X_MAX_07_LEN)-1)<<OSD_DRAW_REG_DRAW_X_MAX_07_POS))

/* 0x60 : draw_partial_y_config7 */
#define OSD_DRAW_DRAW_PARTIAL_Y_CONFIG7_OFFSET                  (0x60)
#define OSD_DRAW_REG_DRAW_Y_MIN_07                              OSD_DRAW_REG_DRAW_Y_MIN_07
#define OSD_DRAW_REG_DRAW_Y_MIN_07_POS                          (0U)
#define OSD_DRAW_REG_DRAW_Y_MIN_07_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MIN_07_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MIN_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_07_POS)
#define OSD_DRAW_REG_DRAW_Y_MIN_07_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MIN_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MIN_07_POS))
#define OSD_DRAW_REG_DRAW_Y_MAX_07                              OSD_DRAW_REG_DRAW_Y_MAX_07
#define OSD_DRAW_REG_DRAW_Y_MAX_07_POS                          (16U)
#define OSD_DRAW_REG_DRAW_Y_MAX_07_LEN                          (11U)
#define OSD_DRAW_REG_DRAW_Y_MAX_07_MSK                          (((1U<<OSD_DRAW_REG_DRAW_Y_MAX_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_07_POS)
#define OSD_DRAW_REG_DRAW_Y_MAX_07_UMSK                         (~(((1U<<OSD_DRAW_REG_DRAW_Y_MAX_07_LEN)-1)<<OSD_DRAW_REG_DRAW_Y_MAX_07_POS))

/* 0xF0 : draw_sw_shd */
#define OSD_DRAW_DRAW_SW_SHD_OFFSET                             (0xF0)
#define OSD_DRAW_CR_SW_SHD                                      OSD_DRAW_CR_SW_SHD
#define OSD_DRAW_CR_SW_SHD_POS                                  (0U)
#define OSD_DRAW_CR_SW_SHD_LEN                                  (1U)
#define OSD_DRAW_CR_SW_SHD_MSK                                  (((1U<<OSD_DRAW_CR_SW_SHD_LEN)-1)<<OSD_DRAW_CR_SW_SHD_POS)
#define OSD_DRAW_CR_SW_SHD_UMSK                                 (~(((1U<<OSD_DRAW_CR_SW_SHD_LEN)-1)<<OSD_DRAW_CR_SW_SHD_POS))


struct  osd_draw_reg {
    /* 0x0 : sd_ctrl_000 */
    union {
        struct {
            uint32_t reg_draw_en_l                  :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_15                  :  8; /* [15: 8],       rsvd,        0x0 */
            uint32_t reg_draw_type_l                :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_000;

    /* 0x4 : draw_partial_yuv_config0 */
    union {
        struct {
            uint32_t reg_draw_th_00                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_00                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_00                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_00                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config0;

    /* 0x8 : draw_partial_x_config0 */
    union {
        struct {
            uint32_t reg_draw_x_min_00              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_00              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config0;

    /* 0xC : draw_partial_y_config0 */
    union {
        struct {
            uint32_t reg_draw_y_min_00              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_00              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config0;

    /* 0x10 : draw_partial_yuv_config1 */
    union {
        struct {
            uint32_t reg_draw_th_01                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_01                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_01                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_01                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config1;

    /* 0x14 : draw_partial_x_config1 */
    union {
        struct {
            uint32_t reg_draw_x_min_01              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_01              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config1;

    /* 0x18 : draw_partial_y_config1 */
    union {
        struct {
            uint32_t reg_draw_y_min_01              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_01              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config1;

    /* 0x1C : draw_partial_yuv_config2 */
    union {
        struct {
            uint32_t reg_draw_th_02                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_02                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_02                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_02                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config2;

    /* 0x20 : draw_partial_x_config2 */
    union {
        struct {
            uint32_t reg_draw_x_min_02              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_02              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config2;

    /* 0x24 : draw_partial_y_config2 */
    union {
        struct {
            uint32_t reg_draw_y_min_02              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_02              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config2;

    /* 0x28 : draw_partial_yuv_config3 */
    union {
        struct {
            uint32_t reg_draw_th_03                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_03                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_03                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_03                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config3;

    /* 0x2C : draw_partial_x_config3 */
    union {
        struct {
            uint32_t reg_draw_x_min_03              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_03              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config3;

    /* 0x30 : draw_partial_y_config3 */
    union {
        struct {
            uint32_t reg_draw_y_min_03              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_03              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config3;

    /* 0x34 : draw_partial_yuv_config4 */
    union {
        struct {
            uint32_t reg_draw_th_04                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_04                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_04                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_04                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config4;

    /* 0x38 : draw_partial_x_config4 */
    union {
        struct {
            uint32_t reg_draw_x_min_04              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_04              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config4;

    /* 0x3C : draw_partial_y_config4 */
    union {
        struct {
            uint32_t reg_draw_y_min_04              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_04              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config4;

    /* 0x40 : draw_partial_yuv_config5 */
    union {
        struct {
            uint32_t reg_draw_th_05                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_05                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_05                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_05                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config5;

    /* 0x44 : draw_partial_x_config5 */
    union {
        struct {
            uint32_t reg_draw_x_min_05              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_05              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config5;

    /* 0x48 : draw_partial_y_config5 */
    union {
        struct {
            uint32_t reg_draw_y_min_05              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_05              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config5;

    /* 0x4C : draw_partial_yuv_config6 */
    union {
        struct {
            uint32_t reg_draw_th_06                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_06                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_06                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_06                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config6;

    /* 0x50 : draw_partial_x_config6 */
    union {
        struct {
            uint32_t reg_draw_x_min_06              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_06              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config6;

    /* 0x54 : draw_partial_y_config6 */
    union {
        struct {
            uint32_t reg_draw_y_min_06              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_06              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config6;

    /* 0x58 : draw_partial_yuv_config7 */
    union {
        struct {
            uint32_t reg_draw_th_07                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_draw_v_07                  :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t reg_draw_u_07                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reg_draw_y_07                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_yuv_config7;

    /* 0x5C : draw_partial_x_config7 */
    union {
        struct {
            uint32_t reg_draw_x_min_07              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_x_max_07              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_x_config7;

    /* 0x60 : draw_partial_y_config7 */
    union {
        struct {
            uint32_t reg_draw_y_min_07              : 11; /* [10: 0],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_draw_y_max_07              : 11; /* [26:16],        r/w,        0x0 */
            uint32_t reserved_27_31                 :  5; /* [31:27],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_partial_y_config7;

    /* 0x64  reserved */
    uint8_t RESERVED0x64[140];

    /* 0xF0 : draw_sw_shd */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } draw_sw_shd;

};

typedef volatile struct osd_draw_reg osd_draw_reg_t;


#endif  /* __OSD_DRAW_REG_H__ */
