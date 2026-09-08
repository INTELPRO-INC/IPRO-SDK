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

#ifndef  __SCALER_HYDRE_REG_H__
#define  __SCALER_HYDRE_REG_H__

#include "ipro7.h"

/* 0x0 : sc_hy_ctrl */
#define SCALER_HYDRE_SC_HY_CTRL_OFFSET                          (0x0)
#define SCALER_HYDRE_CR_WIDTH_I                                 SCALER_HYDRE_CR_WIDTH_I
#define SCALER_HYDRE_CR_WIDTH_I_POS                             (0U)
#define SCALER_HYDRE_CR_WIDTH_I_LEN                             (14U)
#define SCALER_HYDRE_CR_WIDTH_I_MSK                             (((1U<<SCALER_HYDRE_CR_WIDTH_I_LEN)-1)<<SCALER_HYDRE_CR_WIDTH_I_POS)
#define SCALER_HYDRE_CR_WIDTH_I_UMSK                            (~(((1U<<SCALER_HYDRE_CR_WIDTH_I_LEN)-1)<<SCALER_HYDRE_CR_WIDTH_I_POS))
#define SCALER_HYDRE_CR_HEIGHT_I                                SCALER_HYDRE_CR_HEIGHT_I
#define SCALER_HYDRE_CR_HEIGHT_I_POS                            (16U)
#define SCALER_HYDRE_CR_HEIGHT_I_LEN                            (14U)
#define SCALER_HYDRE_CR_HEIGHT_I_MSK                            (((1U<<SCALER_HYDRE_CR_HEIGHT_I_LEN)-1)<<SCALER_HYDRE_CR_HEIGHT_I_POS)
#define SCALER_HYDRE_CR_HEIGHT_I_UMSK                           (~(((1U<<SCALER_HYDRE_CR_HEIGHT_I_LEN)-1)<<SCALER_HYDRE_CR_HEIGHT_I_POS))
#define SCALER_HYDRE_CR_ERROR_INT_EN                            SCALER_HYDRE_CR_ERROR_INT_EN
#define SCALER_HYDRE_CR_ERROR_INT_EN_POS                        (30U)
#define SCALER_HYDRE_CR_ERROR_INT_EN_LEN                        (1U)
#define SCALER_HYDRE_CR_ERROR_INT_EN_MSK                        (((1U<<SCALER_HYDRE_CR_ERROR_INT_EN_LEN)-1)<<SCALER_HYDRE_CR_ERROR_INT_EN_POS)
#define SCALER_HYDRE_CR_ERROR_INT_EN_UMSK                       (~(((1U<<SCALER_HYDRE_CR_ERROR_INT_EN_LEN)-1)<<SCALER_HYDRE_CR_ERROR_INT_EN_POS))
#define SCALER_HYDRE_CR_MODULE_EN                               SCALER_HYDRE_CR_MODULE_EN
#define SCALER_HYDRE_CR_MODULE_EN_POS                           (31U)
#define SCALER_HYDRE_CR_MODULE_EN_LEN                           (1U)
#define SCALER_HYDRE_CR_MODULE_EN_MSK                           (((1U<<SCALER_HYDRE_CR_MODULE_EN_LEN)-1)<<SCALER_HYDRE_CR_MODULE_EN_POS)
#define SCALER_HYDRE_CR_MODULE_EN_UMSK                          (~(((1U<<SCALER_HYDRE_CR_MODULE_EN_LEN)-1)<<SCALER_HYDRE_CR_MODULE_EN_POS))

/* 0x20 : sc_hy_f0_0 */
#define SCALER_HYDRE_SC_HY_F0_0_OFFSET                          (0x20)
#define SCALER_HYDRE_CR_F0_WIDTH_O                              SCALER_HYDRE_CR_F0_WIDTH_O
#define SCALER_HYDRE_CR_F0_WIDTH_O_POS                          (0U)
#define SCALER_HYDRE_CR_F0_WIDTH_O_LEN                          (14U)
#define SCALER_HYDRE_CR_F0_WIDTH_O_MSK                          (((1U<<SCALER_HYDRE_CR_F0_WIDTH_O_LEN)-1)<<SCALER_HYDRE_CR_F0_WIDTH_O_POS)
#define SCALER_HYDRE_CR_F0_WIDTH_O_UMSK                         (~(((1U<<SCALER_HYDRE_CR_F0_WIDTH_O_LEN)-1)<<SCALER_HYDRE_CR_F0_WIDTH_O_POS))
#define SCALER_HYDRE_CR_F0_HEIGHT_O                             SCALER_HYDRE_CR_F0_HEIGHT_O
#define SCALER_HYDRE_CR_F0_HEIGHT_O_POS                         (16U)
#define SCALER_HYDRE_CR_F0_HEIGHT_O_LEN                         (14U)
#define SCALER_HYDRE_CR_F0_HEIGHT_O_MSK                         (((1U<<SCALER_HYDRE_CR_F0_HEIGHT_O_LEN)-1)<<SCALER_HYDRE_CR_F0_HEIGHT_O_POS)
#define SCALER_HYDRE_CR_F0_HEIGHT_O_UMSK                        (~(((1U<<SCALER_HYDRE_CR_F0_HEIGHT_O_LEN)-1)<<SCALER_HYDRE_CR_F0_HEIGHT_O_POS))
#define SCALER_HYDRE_CR_F0_BP_OLINE_MODE                        SCALER_HYDRE_CR_F0_BP_OLINE_MODE
#define SCALER_HYDRE_CR_F0_BP_OLINE_MODE_POS                    (30U)
#define SCALER_HYDRE_CR_F0_BP_OLINE_MODE_LEN                    (1U)
#define SCALER_HYDRE_CR_F0_BP_OLINE_MODE_MSK                    (((1U<<SCALER_HYDRE_CR_F0_BP_OLINE_MODE_LEN)-1)<<SCALER_HYDRE_CR_F0_BP_OLINE_MODE_POS)
#define SCALER_HYDRE_CR_F0_BP_OLINE_MODE_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F0_BP_OLINE_MODE_LEN)-1)<<SCALER_HYDRE_CR_F0_BP_OLINE_MODE_POS))
#define SCALER_HYDRE_CR_F0_EN                                   SCALER_HYDRE_CR_F0_EN
#define SCALER_HYDRE_CR_F0_EN_POS                               (31U)
#define SCALER_HYDRE_CR_F0_EN_LEN                               (1U)
#define SCALER_HYDRE_CR_F0_EN_MSK                               (((1U<<SCALER_HYDRE_CR_F0_EN_LEN)-1)<<SCALER_HYDRE_CR_F0_EN_POS)
#define SCALER_HYDRE_CR_F0_EN_UMSK                              (~(((1U<<SCALER_HYDRE_CR_F0_EN_LEN)-1)<<SCALER_HYDRE_CR_F0_EN_POS))

/* 0x24 : sc_hy_f0_1 */
#define SCALER_HYDRE_SC_HY_F0_1_OFFSET                          (0x24)
#define SCALER_HYDRE_CR_F0_WBLANK_O                             SCALER_HYDRE_CR_F0_WBLANK_O
#define SCALER_HYDRE_CR_F0_WBLANK_O_POS                         (0U)
#define SCALER_HYDRE_CR_F0_WBLANK_O_LEN                         (12U)
#define SCALER_HYDRE_CR_F0_WBLANK_O_MSK                         (((1U<<SCALER_HYDRE_CR_F0_WBLANK_O_LEN)-1)<<SCALER_HYDRE_CR_F0_WBLANK_O_POS)
#define SCALER_HYDRE_CR_F0_WBLANK_O_UMSK                        (~(((1U<<SCALER_HYDRE_CR_F0_WBLANK_O_LEN)-1)<<SCALER_HYDRE_CR_F0_WBLANK_O_POS))

/* 0x28 : sc_hy_f0_2 */
#define SCALER_HYDRE_SC_HY_F0_2_OFFSET                          (0x28)
#define SCALER_HYDRE_CR_F0_Y_INI                                SCALER_HYDRE_CR_F0_Y_INI
#define SCALER_HYDRE_CR_F0_Y_INI_POS                            (0U)
#define SCALER_HYDRE_CR_F0_Y_INI_LEN                            (28U)
#define SCALER_HYDRE_CR_F0_Y_INI_MSK                            (((1U<<SCALER_HYDRE_CR_F0_Y_INI_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_INI_POS)
#define SCALER_HYDRE_CR_F0_Y_INI_UMSK                           (~(((1U<<SCALER_HYDRE_CR_F0_Y_INI_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_INI_POS))

/* 0x2c : sc_hy_f0_3 */
#define SCALER_HYDRE_SC_HY_F0_3_OFFSET                          (0x2c)
#define SCALER_HYDRE_CR_F0_X_INI                                SCALER_HYDRE_CR_F0_X_INI
#define SCALER_HYDRE_CR_F0_X_INI_POS                            (0U)
#define SCALER_HYDRE_CR_F0_X_INI_LEN                            (28U)
#define SCALER_HYDRE_CR_F0_X_INI_MSK                            (((1U<<SCALER_HYDRE_CR_F0_X_INI_LEN)-1)<<SCALER_HYDRE_CR_F0_X_INI_POS)
#define SCALER_HYDRE_CR_F0_X_INI_UMSK                           (~(((1U<<SCALER_HYDRE_CR_F0_X_INI_LEN)-1)<<SCALER_HYDRE_CR_F0_X_INI_POS))

/* 0x30 : sc_hy_f0_4 */
#define SCALER_HYDRE_SC_HY_F0_4_OFFSET                          (0x30)
#define SCALER_HYDRE_CR_F0_Y_STEP                               SCALER_HYDRE_CR_F0_Y_STEP
#define SCALER_HYDRE_CR_F0_Y_STEP_POS                           (0U)
#define SCALER_HYDRE_CR_F0_Y_STEP_LEN                           (22U)
#define SCALER_HYDRE_CR_F0_Y_STEP_MSK                           (((1U<<SCALER_HYDRE_CR_F0_Y_STEP_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_STEP_POS)
#define SCALER_HYDRE_CR_F0_Y_STEP_UMSK                          (~(((1U<<SCALER_HYDRE_CR_F0_Y_STEP_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_STEP_POS))

/* 0x34 : sc_hy_f0_5 */
#define SCALER_HYDRE_SC_HY_F0_5_OFFSET                          (0x34)
#define SCALER_HYDRE_CR_F0_X_STEP                               SCALER_HYDRE_CR_F0_X_STEP
#define SCALER_HYDRE_CR_F0_X_STEP_POS                           (0U)
#define SCALER_HYDRE_CR_F0_X_STEP_LEN                           (22U)
#define SCALER_HYDRE_CR_F0_X_STEP_MSK                           (((1U<<SCALER_HYDRE_CR_F0_X_STEP_LEN)-1)<<SCALER_HYDRE_CR_F0_X_STEP_POS)
#define SCALER_HYDRE_CR_F0_X_STEP_UMSK                          (~(((1U<<SCALER_HYDRE_CR_F0_X_STEP_LEN)-1)<<SCALER_HYDRE_CR_F0_X_STEP_POS))

/* 0x38 : sc_hy_f0_6 */
#define SCALER_HYDRE_SC_HY_F0_6_OFFSET                          (0x38)
#define SCALER_HYDRE_CR_F0_Y_COEFF_RATIO                        SCALER_HYDRE_CR_F0_Y_COEFF_RATIO
#define SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_POS                    (0U)
#define SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_LEN                    (4U)
#define SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_MSK                    (((1U<<SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_POS)
#define SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_COEFF_RATIO_POS))
#define SCALER_HYDRE_CR_F0_X_COEFF_RATIO                        SCALER_HYDRE_CR_F0_X_COEFF_RATIO
#define SCALER_HYDRE_CR_F0_X_COEFF_RATIO_POS                    (4U)
#define SCALER_HYDRE_CR_F0_X_COEFF_RATIO_LEN                    (4U)
#define SCALER_HYDRE_CR_F0_X_COEFF_RATIO_MSK                    (((1U<<SCALER_HYDRE_CR_F0_X_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F0_X_COEFF_RATIO_POS)
#define SCALER_HYDRE_CR_F0_X_COEFF_RATIO_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F0_X_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F0_X_COEFF_RATIO_POS))
#define SCALER_HYDRE_CR_F0_Y_COEFF_SET                          SCALER_HYDRE_CR_F0_Y_COEFF_SET
#define SCALER_HYDRE_CR_F0_Y_COEFF_SET_POS                      (8U)
#define SCALER_HYDRE_CR_F0_Y_COEFF_SET_LEN                      (1U)
#define SCALER_HYDRE_CR_F0_Y_COEFF_SET_MSK                      (((1U<<SCALER_HYDRE_CR_F0_Y_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_COEFF_SET_POS)
#define SCALER_HYDRE_CR_F0_Y_COEFF_SET_UMSK                     (~(((1U<<SCALER_HYDRE_CR_F0_Y_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F0_Y_COEFF_SET_POS))
#define SCALER_HYDRE_CR_F0_X_COEFF_SET                          SCALER_HYDRE_CR_F0_X_COEFF_SET
#define SCALER_HYDRE_CR_F0_X_COEFF_SET_POS                      (10U)
#define SCALER_HYDRE_CR_F0_X_COEFF_SET_LEN                      (2U)
#define SCALER_HYDRE_CR_F0_X_COEFF_SET_MSK                      (((1U<<SCALER_HYDRE_CR_F0_X_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F0_X_COEFF_SET_POS)
#define SCALER_HYDRE_CR_F0_X_COEFF_SET_UMSK                     (~(((1U<<SCALER_HYDRE_CR_F0_X_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F0_X_COEFF_SET_POS))

/* 0x3C : sc_hy_f0_7 */
#define SCALER_HYDRE_SC_HY_F0_7_OFFSET                          (0x3C)
#define SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT                     SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT
#define SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_POS                 (0U)
#define SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_LEN                 (12U)
#define SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_MSK                 (((1U<<SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_LEN)-1)<<SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_POS)
#define SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_UMSK                (~(((1U<<SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_LEN)-1)<<SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT_POS))

/* 0x40 : sc_hy_f1_0 */
#define SCALER_HYDRE_SC_HY_F1_0_OFFSET                          (0x40)
#define SCALER_HYDRE_CR_F1_WIDTH_O                              SCALER_HYDRE_CR_F1_WIDTH_O
#define SCALER_HYDRE_CR_F1_WIDTH_O_POS                          (0U)
#define SCALER_HYDRE_CR_F1_WIDTH_O_LEN                          (14U)
#define SCALER_HYDRE_CR_F1_WIDTH_O_MSK                          (((1U<<SCALER_HYDRE_CR_F1_WIDTH_O_LEN)-1)<<SCALER_HYDRE_CR_F1_WIDTH_O_POS)
#define SCALER_HYDRE_CR_F1_WIDTH_O_UMSK                         (~(((1U<<SCALER_HYDRE_CR_F1_WIDTH_O_LEN)-1)<<SCALER_HYDRE_CR_F1_WIDTH_O_POS))
#define SCALER_HYDRE_CR_F1_HEIGHT_O                             SCALER_HYDRE_CR_F1_HEIGHT_O
#define SCALER_HYDRE_CR_F1_HEIGHT_O_POS                         (16U)
#define SCALER_HYDRE_CR_F1_HEIGHT_O_LEN                         (14U)
#define SCALER_HYDRE_CR_F1_HEIGHT_O_MSK                         (((1U<<SCALER_HYDRE_CR_F1_HEIGHT_O_LEN)-1)<<SCALER_HYDRE_CR_F1_HEIGHT_O_POS)
#define SCALER_HYDRE_CR_F1_HEIGHT_O_UMSK                        (~(((1U<<SCALER_HYDRE_CR_F1_HEIGHT_O_LEN)-1)<<SCALER_HYDRE_CR_F1_HEIGHT_O_POS))
#define SCALER_HYDRE_CR_F1_BP_OLINE_MODE                        SCALER_HYDRE_CR_F1_BP_OLINE_MODE
#define SCALER_HYDRE_CR_F1_BP_OLINE_MODE_POS                    (30U)
#define SCALER_HYDRE_CR_F1_BP_OLINE_MODE_LEN                    (1U)
#define SCALER_HYDRE_CR_F1_BP_OLINE_MODE_MSK                    (((1U<<SCALER_HYDRE_CR_F1_BP_OLINE_MODE_LEN)-1)<<SCALER_HYDRE_CR_F1_BP_OLINE_MODE_POS)
#define SCALER_HYDRE_CR_F1_BP_OLINE_MODE_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F1_BP_OLINE_MODE_LEN)-1)<<SCALER_HYDRE_CR_F1_BP_OLINE_MODE_POS))
#define SCALER_HYDRE_CR_F1_EN                                   SCALER_HYDRE_CR_F1_EN
#define SCALER_HYDRE_CR_F1_EN_POS                               (31U)
#define SCALER_HYDRE_CR_F1_EN_LEN                               (1U)
#define SCALER_HYDRE_CR_F1_EN_MSK                               (((1U<<SCALER_HYDRE_CR_F1_EN_LEN)-1)<<SCALER_HYDRE_CR_F1_EN_POS)
#define SCALER_HYDRE_CR_F1_EN_UMSK                              (~(((1U<<SCALER_HYDRE_CR_F1_EN_LEN)-1)<<SCALER_HYDRE_CR_F1_EN_POS))

/* 0x44 : sc_hy_f1_1 */
#define SCALER_HYDRE_SC_HY_F1_1_OFFSET                          (0x44)
#define SCALER_HYDRE_CR_F1_WBLANK_O                             SCALER_HYDRE_CR_F1_WBLANK_O
#define SCALER_HYDRE_CR_F1_WBLANK_O_POS                         (0U)
#define SCALER_HYDRE_CR_F1_WBLANK_O_LEN                         (12U)
#define SCALER_HYDRE_CR_F1_WBLANK_O_MSK                         (((1U<<SCALER_HYDRE_CR_F1_WBLANK_O_LEN)-1)<<SCALER_HYDRE_CR_F1_WBLANK_O_POS)
#define SCALER_HYDRE_CR_F1_WBLANK_O_UMSK                        (~(((1U<<SCALER_HYDRE_CR_F1_WBLANK_O_LEN)-1)<<SCALER_HYDRE_CR_F1_WBLANK_O_POS))

/* 0x48 : sc_hy_f1_2 */
#define SCALER_HYDRE_SC_HY_F1_2_OFFSET                          (0x48)
#define SCALER_HYDRE_CR_F1_Y_INI                                SCALER_HYDRE_CR_F1_Y_INI
#define SCALER_HYDRE_CR_F1_Y_INI_POS                            (0U)
#define SCALER_HYDRE_CR_F1_Y_INI_LEN                            (28U)
#define SCALER_HYDRE_CR_F1_Y_INI_MSK                            (((1U<<SCALER_HYDRE_CR_F1_Y_INI_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_INI_POS)
#define SCALER_HYDRE_CR_F1_Y_INI_UMSK                           (~(((1U<<SCALER_HYDRE_CR_F1_Y_INI_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_INI_POS))

/* 0x4c : sc_hy_f1_3 */
#define SCALER_HYDRE_SC_HY_F1_3_OFFSET                          (0x4c)
#define SCALER_HYDRE_CR_F1_X_INI                                SCALER_HYDRE_CR_F1_X_INI
#define SCALER_HYDRE_CR_F1_X_INI_POS                            (0U)
#define SCALER_HYDRE_CR_F1_X_INI_LEN                            (28U)
#define SCALER_HYDRE_CR_F1_X_INI_MSK                            (((1U<<SCALER_HYDRE_CR_F1_X_INI_LEN)-1)<<SCALER_HYDRE_CR_F1_X_INI_POS)
#define SCALER_HYDRE_CR_F1_X_INI_UMSK                           (~(((1U<<SCALER_HYDRE_CR_F1_X_INI_LEN)-1)<<SCALER_HYDRE_CR_F1_X_INI_POS))

/* 0x50 : sc_hy_f1_4 */
#define SCALER_HYDRE_SC_HY_F1_4_OFFSET                          (0x50)
#define SCALER_HYDRE_CR_F1_Y_STEP                               SCALER_HYDRE_CR_F1_Y_STEP
#define SCALER_HYDRE_CR_F1_Y_STEP_POS                           (0U)
#define SCALER_HYDRE_CR_F1_Y_STEP_LEN                           (22U)
#define SCALER_HYDRE_CR_F1_Y_STEP_MSK                           (((1U<<SCALER_HYDRE_CR_F1_Y_STEP_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_STEP_POS)
#define SCALER_HYDRE_CR_F1_Y_STEP_UMSK                          (~(((1U<<SCALER_HYDRE_CR_F1_Y_STEP_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_STEP_POS))

/* 0x54 : sc_hy_f1_5 */
#define SCALER_HYDRE_SC_HY_F1_5_OFFSET                          (0x54)
#define SCALER_HYDRE_CR_F1_X_STEP                               SCALER_HYDRE_CR_F1_X_STEP
#define SCALER_HYDRE_CR_F1_X_STEP_POS                           (0U)
#define SCALER_HYDRE_CR_F1_X_STEP_LEN                           (22U)
#define SCALER_HYDRE_CR_F1_X_STEP_MSK                           (((1U<<SCALER_HYDRE_CR_F1_X_STEP_LEN)-1)<<SCALER_HYDRE_CR_F1_X_STEP_POS)
#define SCALER_HYDRE_CR_F1_X_STEP_UMSK                          (~(((1U<<SCALER_HYDRE_CR_F1_X_STEP_LEN)-1)<<SCALER_HYDRE_CR_F1_X_STEP_POS))

/* 0x58 : sc_hy_f1_6 */
#define SCALER_HYDRE_SC_HY_F1_6_OFFSET                          (0x58)
#define SCALER_HYDRE_CR_F1_Y_COEFF_RATIO                        SCALER_HYDRE_CR_F1_Y_COEFF_RATIO
#define SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_POS                    (0U)
#define SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_LEN                    (4U)
#define SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_MSK                    (((1U<<SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_POS)
#define SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_COEFF_RATIO_POS))
#define SCALER_HYDRE_CR_F1_X_COEFF_RATIO                        SCALER_HYDRE_CR_F1_X_COEFF_RATIO
#define SCALER_HYDRE_CR_F1_X_COEFF_RATIO_POS                    (4U)
#define SCALER_HYDRE_CR_F1_X_COEFF_RATIO_LEN                    (4U)
#define SCALER_HYDRE_CR_F1_X_COEFF_RATIO_MSK                    (((1U<<SCALER_HYDRE_CR_F1_X_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F1_X_COEFF_RATIO_POS)
#define SCALER_HYDRE_CR_F1_X_COEFF_RATIO_UMSK                   (~(((1U<<SCALER_HYDRE_CR_F1_X_COEFF_RATIO_LEN)-1)<<SCALER_HYDRE_CR_F1_X_COEFF_RATIO_POS))
#define SCALER_HYDRE_CR_F1_Y_COEFF_SET                          SCALER_HYDRE_CR_F1_Y_COEFF_SET
#define SCALER_HYDRE_CR_F1_Y_COEFF_SET_POS                      (8U)
#define SCALER_HYDRE_CR_F1_Y_COEFF_SET_LEN                      (1U)
#define SCALER_HYDRE_CR_F1_Y_COEFF_SET_MSK                      (((1U<<SCALER_HYDRE_CR_F1_Y_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_COEFF_SET_POS)
#define SCALER_HYDRE_CR_F1_Y_COEFF_SET_UMSK                     (~(((1U<<SCALER_HYDRE_CR_F1_Y_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F1_Y_COEFF_SET_POS))
#define SCALER_HYDRE_CR_F1_X_COEFF_SET                          SCALER_HYDRE_CR_F1_X_COEFF_SET
#define SCALER_HYDRE_CR_F1_X_COEFF_SET_POS                      (10U)
#define SCALER_HYDRE_CR_F1_X_COEFF_SET_LEN                      (2U)
#define SCALER_HYDRE_CR_F1_X_COEFF_SET_MSK                      (((1U<<SCALER_HYDRE_CR_F1_X_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F1_X_COEFF_SET_POS)
#define SCALER_HYDRE_CR_F1_X_COEFF_SET_UMSK                     (~(((1U<<SCALER_HYDRE_CR_F1_X_COEFF_SET_LEN)-1)<<SCALER_HYDRE_CR_F1_X_COEFF_SET_POS))

/* 0x5C : sc_hy_f1_7 */
#define SCALER_HYDRE_SC_HY_F1_7_OFFSET                          (0x5C)
#define SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT                     SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT
#define SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_POS                 (0U)
#define SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_LEN                 (12U)
#define SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_MSK                 (((1U<<SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_LEN)-1)<<SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_POS)
#define SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_UMSK                (~(((1U<<SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_LEN)-1)<<SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT_POS))

/* 0xf0 : sc_hy_shd */
#define SCALER_HYDRE_SC_HY_SHD_OFFSET                           (0xf0)
#define SCALER_HYDRE_CR_SW_SHD                                  SCALER_HYDRE_CR_SW_SHD
#define SCALER_HYDRE_CR_SW_SHD_POS                              (0U)
#define SCALER_HYDRE_CR_SW_SHD_LEN                              (1U)
#define SCALER_HYDRE_CR_SW_SHD_MSK                              (((1U<<SCALER_HYDRE_CR_SW_SHD_LEN)-1)<<SCALER_HYDRE_CR_SW_SHD_POS)
#define SCALER_HYDRE_CR_SW_SHD_UMSK                             (~(((1U<<SCALER_HYDRE_CR_SW_SHD_LEN)-1)<<SCALER_HYDRE_CR_SW_SHD_POS))
#define SCALER_HYDRE_CR_Y_SRAM_OVF_CLR                          SCALER_HYDRE_CR_Y_SRAM_OVF_CLR
#define SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_POS                      (1U)
#define SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_LEN                      (1U)
#define SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_MSK                      (((1U<<SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_LEN)-1)<<SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_POS)
#define SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_UMSK                     (~(((1U<<SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_LEN)-1)<<SCALER_HYDRE_CR_Y_SRAM_OVF_CLR_POS))
#define SCALER_HYDRE_CR_C_SRAM_OVF_CLR                          SCALER_HYDRE_CR_C_SRAM_OVF_CLR
#define SCALER_HYDRE_CR_C_SRAM_OVF_CLR_POS                      (2U)
#define SCALER_HYDRE_CR_C_SRAM_OVF_CLR_LEN                      (1U)
#define SCALER_HYDRE_CR_C_SRAM_OVF_CLR_MSK                      (((1U<<SCALER_HYDRE_CR_C_SRAM_OVF_CLR_LEN)-1)<<SCALER_HYDRE_CR_C_SRAM_OVF_CLR_POS)
#define SCALER_HYDRE_CR_C_SRAM_OVF_CLR_UMSK                     (~(((1U<<SCALER_HYDRE_CR_C_SRAM_OVF_CLR_LEN)-1)<<SCALER_HYDRE_CR_C_SRAM_OVF_CLR_POS))
#define SCALER_HYDRE_ST_Y_LBUF_OVF                              SCALER_HYDRE_ST_Y_LBUF_OVF
#define SCALER_HYDRE_ST_Y_LBUF_OVF_POS                          (16U)
#define SCALER_HYDRE_ST_Y_LBUF_OVF_LEN                          (1U)
#define SCALER_HYDRE_ST_Y_LBUF_OVF_MSK                          (((1U<<SCALER_HYDRE_ST_Y_LBUF_OVF_LEN)-1)<<SCALER_HYDRE_ST_Y_LBUF_OVF_POS)
#define SCALER_HYDRE_ST_Y_LBUF_OVF_UMSK                         (~(((1U<<SCALER_HYDRE_ST_Y_LBUF_OVF_LEN)-1)<<SCALER_HYDRE_ST_Y_LBUF_OVF_POS))
#define SCALER_HYDRE_ST_C_LBUF_OVF                              SCALER_HYDRE_ST_C_LBUF_OVF
#define SCALER_HYDRE_ST_C_LBUF_OVF_POS                          (17U)
#define SCALER_HYDRE_ST_C_LBUF_OVF_LEN                          (1U)
#define SCALER_HYDRE_ST_C_LBUF_OVF_MSK                          (((1U<<SCALER_HYDRE_ST_C_LBUF_OVF_LEN)-1)<<SCALER_HYDRE_ST_C_LBUF_OVF_POS)
#define SCALER_HYDRE_ST_C_LBUF_OVF_UMSK                         (~(((1U<<SCALER_HYDRE_ST_C_LBUF_OVF_LEN)-1)<<SCALER_HYDRE_ST_C_LBUF_OVF_POS))


struct  scaler_hydre_reg {
    /* 0x0 : sc_hy_ctrl */
    union {
        struct {
            uint32_t cr_width_i                     : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_height_i                    : 14; /* [29:16],        r/w,        0x0 */
            uint32_t cr_error_int_en                :  1; /* [   30],        r/w,        0x1 */
            uint32_t cr_module_en                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_ctrl;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[28];

    /* 0x20 : sc_hy_f0_0 */
    union {
        struct {
            uint32_t cr_f0_width_o                  : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_f0_height_o                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t cr_f0_bp_oline_mode            :  1; /* [   30],        r/w,        0x0 */
            uint32_t cr_f0_en                       :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_0;

    /* 0x24 : sc_hy_f0_1 */
    union {
        struct {
            uint32_t cr_f0_wblank_o                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_1;

    /* 0x28 : sc_hy_f0_2 */
    union {
        struct {
            uint32_t cr_f0_y_ini                    : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_2;

    /* 0x2c : sc_hy_f0_3 */
    union {
        struct {
            uint32_t cr_f0_x_ini                    : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_3;

    /* 0x30 : sc_hy_f0_4 */
    union {
        struct {
            uint32_t cr_f0_y_step                   : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_4;

    /* 0x34 : sc_hy_f0_5 */
    union {
        struct {
            uint32_t cr_f0_x_step                   : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_5;

    /* 0x38 : sc_hy_f0_6 */
    union {
        struct {
            uint32_t cr_f0_y_coeff_ratio            :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_f0_x_coeff_ratio            :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_f0_y_coeff_set              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_f0_x_coeff_set              :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_6;

    /* 0x3C : sc_hy_f0_7 */
    union {
        struct {
            uint32_t cr_f0_bp_oline_act_cnt         : 12; /* [11: 0],        r/w,       0x10 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f0_7;

    /* 0x40 : sc_hy_f1_0 */
    union {
        struct {
            uint32_t cr_f1_width_o                  : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_f1_height_o                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t cr_f1_bp_oline_mode            :  1; /* [   30],        r/w,        0x0 */
            uint32_t cr_f1_en                       :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_0;

    /* 0x44 : sc_hy_f1_1 */
    union {
        struct {
            uint32_t cr_f1_wblank_o                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_1;

    /* 0x48 : sc_hy_f1_2 */
    union {
        struct {
            uint32_t cr_f1_y_ini                    : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_2;

    /* 0x4c : sc_hy_f1_3 */
    union {
        struct {
            uint32_t cr_f1_x_ini                    : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_3;

    /* 0x50 : sc_hy_f1_4 */
    union {
        struct {
            uint32_t cr_f1_y_step                   : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_4;

    /* 0x54 : sc_hy_f1_5 */
    union {
        struct {
            uint32_t cr_f1_x_step                   : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_5;

    /* 0x58 : sc_hy_f1_6 */
    union {
        struct {
            uint32_t cr_f1_y_coeff_ratio            :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_f1_x_coeff_ratio            :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_f1_y_coeff_set              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_f1_x_coeff_set              :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_6;

    /* 0x5C : sc_hy_f1_7 */
    union {
        struct {
            uint32_t cr_f1_bp_oline_act_cnt         : 12; /* [11: 0],        r/w,       0x10 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_f1_7;

    /* 0x60  reserved */
    uint8_t RESERVED0x60[144];

    /* 0xf0 : sc_hy_shd */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t cr_y_sram_ovf_clr              :  1; /* [    1],        w1p,        0x0 */
            uint32_t cr_c_sram_ovf_clr              :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_15                  : 13; /* [15: 3],       rsvd,        0x0 */
            uint32_t st_y_lbuf_ovf                  :  1; /* [   16],          r,        0x0 */
            uint32_t st_c_lbuf_ovf                  :  1; /* [   17],          r,        0x0 */
            uint32_t reserved_18_31                 : 14; /* [31:18],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sc_hy_shd;

};

typedef volatile struct scaler_hydre_reg scaler_hydre_reg_t;


#endif  /* __SCALER_HYDRE_REG_H__ */
