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

#ifndef  __WDR_CSR_REG_H__
#define  __WDR_CSR_REG_H__

#include "ipro7.h"

/* 0x0 : WDR_0 */
#define WDR_CSR_WDR_0_OFFSET                                    (0x0)
#define WDR_CSR_CR_WDR_EN                                       WDR_CSR_CR_WDR_EN
#define WDR_CSR_CR_WDR_EN_POS                                   (0U)
#define WDR_CSR_CR_WDR_EN_LEN                                   (1U)
#define WDR_CSR_CR_WDR_EN_MSK                                   (((1U<<WDR_CSR_CR_WDR_EN_LEN)-1)<<WDR_CSR_CR_WDR_EN_POS)
#define WDR_CSR_CR_WDR_EN_UMSK                                  (~(((1U<<WDR_CSR_CR_WDR_EN_LEN)-1)<<WDR_CSR_CR_WDR_EN_POS))
#define WDR_CSR_STS_WDR_BUF_IDX                                 WDR_CSR_STS_WDR_BUF_IDX
#define WDR_CSR_STS_WDR_BUF_IDX_POS                             (1U)
#define WDR_CSR_STS_WDR_BUF_IDX_LEN                             (1U)
#define WDR_CSR_STS_WDR_BUF_IDX_MSK                             (((1U<<WDR_CSR_STS_WDR_BUF_IDX_LEN)-1)<<WDR_CSR_STS_WDR_BUF_IDX_POS)
#define WDR_CSR_STS_WDR_BUF_IDX_UMSK                            (~(((1U<<WDR_CSR_STS_WDR_BUF_IDX_LEN)-1)<<WDR_CSR_STS_WDR_BUF_IDX_POS))
#define WDR_CSR_CR_WDR_INT_MASK                                 WDR_CSR_CR_WDR_INT_MASK
#define WDR_CSR_CR_WDR_INT_MASK_POS                             (2U)
#define WDR_CSR_CR_WDR_INT_MASK_LEN                             (1U)
#define WDR_CSR_CR_WDR_INT_MASK_MSK                             (((1U<<WDR_CSR_CR_WDR_INT_MASK_LEN)-1)<<WDR_CSR_CR_WDR_INT_MASK_POS)
#define WDR_CSR_CR_WDR_INT_MASK_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_INT_MASK_LEN)-1)<<WDR_CSR_CR_WDR_INT_MASK_POS))
#define WDR_CSR_STS_WDR_W_CNT                                   WDR_CSR_STS_WDR_W_CNT
#define WDR_CSR_STS_WDR_W_CNT_POS                               (4U)
#define WDR_CSR_STS_WDR_W_CNT_LEN                               (5U)
#define WDR_CSR_STS_WDR_W_CNT_MSK                               (((1U<<WDR_CSR_STS_WDR_W_CNT_LEN)-1)<<WDR_CSR_STS_WDR_W_CNT_POS)
#define WDR_CSR_STS_WDR_W_CNT_UMSK                              (~(((1U<<WDR_CSR_STS_WDR_W_CNT_LEN)-1)<<WDR_CSR_STS_WDR_W_CNT_POS))
#define WDR_CSR_CR_CS_GAIN                                      WDR_CSR_CR_CS_GAIN
#define WDR_CSR_CR_CS_GAIN_POS                                  (16U)
#define WDR_CSR_CR_CS_GAIN_LEN                                  (13U)
#define WDR_CSR_CR_CS_GAIN_MSK                                  (((1U<<WDR_CSR_CR_CS_GAIN_LEN)-1)<<WDR_CSR_CR_CS_GAIN_POS)
#define WDR_CSR_CR_CS_GAIN_UMSK                                 (~(((1U<<WDR_CSR_CR_CS_GAIN_LEN)-1)<<WDR_CSR_CR_CS_GAIN_POS))

/* 0x4 : WDR_CS_0 */
#define WDR_CSR_WDR_CS_0_OFFSET                                 (0x4)
#define WDR_CSR_CR_CS_EN                                        WDR_CSR_CR_CS_EN
#define WDR_CSR_CR_CS_EN_POS                                    (0U)
#define WDR_CSR_CR_CS_EN_LEN                                    (1U)
#define WDR_CSR_CR_CS_EN_MSK                                    (((1U<<WDR_CSR_CR_CS_EN_LEN)-1)<<WDR_CSR_CR_CS_EN_POS)
#define WDR_CSR_CR_CS_EN_UMSK                                   (~(((1U<<WDR_CSR_CR_CS_EN_LEN)-1)<<WDR_CSR_CR_CS_EN_POS))
#define WDR_CSR_CR_CS_WEIGHT                                    WDR_CSR_CR_CS_WEIGHT
#define WDR_CSR_CR_CS_WEIGHT_POS                                (8U)
#define WDR_CSR_CR_CS_WEIGHT_LEN                                (5U)
#define WDR_CSR_CR_CS_WEIGHT_MSK                                (((1U<<WDR_CSR_CR_CS_WEIGHT_LEN)-1)<<WDR_CSR_CR_CS_WEIGHT_POS)
#define WDR_CSR_CR_CS_WEIGHT_UMSK                               (~(((1U<<WDR_CSR_CR_CS_WEIGHT_LEN)-1)<<WDR_CSR_CR_CS_WEIGHT_POS))
#define WDR_CSR_CR_CS_GREY_TH                                   WDR_CSR_CR_CS_GREY_TH
#define WDR_CSR_CR_CS_GREY_TH_POS                               (16U)
#define WDR_CSR_CR_CS_GREY_TH_LEN                               (8U)
#define WDR_CSR_CR_CS_GREY_TH_MSK                               (((1U<<WDR_CSR_CR_CS_GREY_TH_LEN)-1)<<WDR_CSR_CR_CS_GREY_TH_POS)
#define WDR_CSR_CR_CS_GREY_TH_UMSK                              (~(((1U<<WDR_CSR_CR_CS_GREY_TH_LEN)-1)<<WDR_CSR_CR_CS_GREY_TH_POS))

/* 0x8 : WDR_HIST_0 */
#define WDR_CSR_WDR_HIST_0_OFFSET                               (0x8)
#define WDR_CSR_CR_HIST_ROI_EN                                  WDR_CSR_CR_HIST_ROI_EN
#define WDR_CSR_CR_HIST_ROI_EN_POS                              (0U)
#define WDR_CSR_CR_HIST_ROI_EN_LEN                              (8U)
#define WDR_CSR_CR_HIST_ROI_EN_MSK                              (((1U<<WDR_CSR_CR_HIST_ROI_EN_LEN)-1)<<WDR_CSR_CR_HIST_ROI_EN_POS)
#define WDR_CSR_CR_HIST_ROI_EN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_ROI_EN_LEN)-1)<<WDR_CSR_CR_HIST_ROI_EN_POS))

/* 0xC : WDR_HIST_1 */
#define WDR_CSR_WDR_HIST_1_OFFSET                               (0xC)
#define WDR_CSR_CR_HIST_ROI0_WEIGHT                             WDR_CSR_CR_HIST_ROI0_WEIGHT
#define WDR_CSR_CR_HIST_ROI0_WEIGHT_POS                         (0U)
#define WDR_CSR_CR_HIST_ROI0_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI0_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI0_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI0_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI0_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI0_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI0_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI1_WEIGHT                             WDR_CSR_CR_HIST_ROI1_WEIGHT
#define WDR_CSR_CR_HIST_ROI1_WEIGHT_POS                         (4U)
#define WDR_CSR_CR_HIST_ROI1_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI1_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI1_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI1_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI1_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI1_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI1_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI2_WEIGHT                             WDR_CSR_CR_HIST_ROI2_WEIGHT
#define WDR_CSR_CR_HIST_ROI2_WEIGHT_POS                         (8U)
#define WDR_CSR_CR_HIST_ROI2_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI2_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI2_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI2_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI2_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI2_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI2_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI3_WEIGHT                             WDR_CSR_CR_HIST_ROI3_WEIGHT
#define WDR_CSR_CR_HIST_ROI3_WEIGHT_POS                         (12U)
#define WDR_CSR_CR_HIST_ROI3_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI3_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI3_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI3_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI3_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI3_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI3_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI4_WEIGHT                             WDR_CSR_CR_HIST_ROI4_WEIGHT
#define WDR_CSR_CR_HIST_ROI4_WEIGHT_POS                         (16U)
#define WDR_CSR_CR_HIST_ROI4_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI4_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI4_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI4_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI4_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI4_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI4_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI5_WEIGHT                             WDR_CSR_CR_HIST_ROI5_WEIGHT
#define WDR_CSR_CR_HIST_ROI5_WEIGHT_POS                         (20U)
#define WDR_CSR_CR_HIST_ROI5_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI5_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI5_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI5_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI5_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI5_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI5_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI6_WEIGHT                             WDR_CSR_CR_HIST_ROI6_WEIGHT
#define WDR_CSR_CR_HIST_ROI6_WEIGHT_POS                         (24U)
#define WDR_CSR_CR_HIST_ROI6_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI6_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI6_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI6_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI6_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI6_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI6_WEIGHT_POS))
#define WDR_CSR_CR_HIST_ROI7_WEIGHT                             WDR_CSR_CR_HIST_ROI7_WEIGHT
#define WDR_CSR_CR_HIST_ROI7_WEIGHT_POS                         (28U)
#define WDR_CSR_CR_HIST_ROI7_WEIGHT_LEN                         (4U)
#define WDR_CSR_CR_HIST_ROI7_WEIGHT_MSK                         (((1U<<WDR_CSR_CR_HIST_ROI7_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI7_WEIGHT_POS)
#define WDR_CSR_CR_HIST_ROI7_WEIGHT_UMSK                        (~(((1U<<WDR_CSR_CR_HIST_ROI7_WEIGHT_LEN)-1)<<WDR_CSR_CR_HIST_ROI7_WEIGHT_POS))

/* 0x10 : WDR_HIST_2 */
#define WDR_CSR_WDR_HIST_2_OFFSET                               (0x10)
#define WDR_CSR_CR_HIST_X0_MAX                                  WDR_CSR_CR_HIST_X0_MAX
#define WDR_CSR_CR_HIST_X0_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X0_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X0_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X0_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X0_MAX_POS)
#define WDR_CSR_CR_HIST_X0_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X0_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X0_MAX_POS))
#define WDR_CSR_CR_HIST_X0_MIN                                  WDR_CSR_CR_HIST_X0_MIN
#define WDR_CSR_CR_HIST_X0_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X0_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X0_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X0_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X0_MIN_POS)
#define WDR_CSR_CR_HIST_X0_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X0_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X0_MIN_POS))

/* 0x14 : WDR_HIST_3 */
#define WDR_CSR_WDR_HIST_3_OFFSET                               (0x14)
#define WDR_CSR_CR_HIST_Y0_MAX                                  WDR_CSR_CR_HIST_Y0_MAX
#define WDR_CSR_CR_HIST_Y0_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y0_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y0_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y0_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y0_MAX_POS)
#define WDR_CSR_CR_HIST_Y0_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y0_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y0_MAX_POS))
#define WDR_CSR_CR_HIST_Y0_MIN                                  WDR_CSR_CR_HIST_Y0_MIN
#define WDR_CSR_CR_HIST_Y0_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y0_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y0_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y0_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y0_MIN_POS)
#define WDR_CSR_CR_HIST_Y0_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y0_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y0_MIN_POS))

/* 0x18 : WDR_HIST_4 */
#define WDR_CSR_WDR_HIST_4_OFFSET                               (0x18)
#define WDR_CSR_CR_HIST_X1_MAX                                  WDR_CSR_CR_HIST_X1_MAX
#define WDR_CSR_CR_HIST_X1_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X1_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X1_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X1_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X1_MAX_POS)
#define WDR_CSR_CR_HIST_X1_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X1_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X1_MAX_POS))
#define WDR_CSR_CR_HIST_X1_MIN                                  WDR_CSR_CR_HIST_X1_MIN
#define WDR_CSR_CR_HIST_X1_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X1_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X1_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X1_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X1_MIN_POS)
#define WDR_CSR_CR_HIST_X1_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X1_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X1_MIN_POS))

/* 0x1C : WDR_HIST_5 */
#define WDR_CSR_WDR_HIST_5_OFFSET                               (0x1C)
#define WDR_CSR_CR_HIST_Y1_MAX                                  WDR_CSR_CR_HIST_Y1_MAX
#define WDR_CSR_CR_HIST_Y1_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y1_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y1_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y1_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y1_MAX_POS)
#define WDR_CSR_CR_HIST_Y1_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y1_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y1_MAX_POS))
#define WDR_CSR_CR_HIST_Y1_MIN                                  WDR_CSR_CR_HIST_Y1_MIN
#define WDR_CSR_CR_HIST_Y1_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y1_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y1_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y1_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y1_MIN_POS)
#define WDR_CSR_CR_HIST_Y1_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y1_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y1_MIN_POS))

/* 0x20 : WDR_HIST_6 */
#define WDR_CSR_WDR_HIST_6_OFFSET                               (0x20)
#define WDR_CSR_CR_HIST_X2_MAX                                  WDR_CSR_CR_HIST_X2_MAX
#define WDR_CSR_CR_HIST_X2_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X2_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X2_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X2_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X2_MAX_POS)
#define WDR_CSR_CR_HIST_X2_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X2_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X2_MAX_POS))
#define WDR_CSR_CR_HIST_X2_MIN                                  WDR_CSR_CR_HIST_X2_MIN
#define WDR_CSR_CR_HIST_X2_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X2_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X2_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X2_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X2_MIN_POS)
#define WDR_CSR_CR_HIST_X2_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X2_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X2_MIN_POS))

/* 0x24 : WDR_HIST_7 */
#define WDR_CSR_WDR_HIST_7_OFFSET                               (0x24)
#define WDR_CSR_CR_HIST_Y2_MAX                                  WDR_CSR_CR_HIST_Y2_MAX
#define WDR_CSR_CR_HIST_Y2_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y2_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y2_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y2_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y2_MAX_POS)
#define WDR_CSR_CR_HIST_Y2_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y2_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y2_MAX_POS))
#define WDR_CSR_CR_HIST_Y2_MIN                                  WDR_CSR_CR_HIST_Y2_MIN
#define WDR_CSR_CR_HIST_Y2_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y2_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y2_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y2_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y2_MIN_POS)
#define WDR_CSR_CR_HIST_Y2_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y2_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y2_MIN_POS))

/* 0x28 : WDR_HIST_8 */
#define WDR_CSR_WDR_HIST_8_OFFSET                               (0x28)
#define WDR_CSR_CR_HIST_X3_MAX                                  WDR_CSR_CR_HIST_X3_MAX
#define WDR_CSR_CR_HIST_X3_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X3_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X3_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X3_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X3_MAX_POS)
#define WDR_CSR_CR_HIST_X3_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X3_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X3_MAX_POS))
#define WDR_CSR_CR_HIST_X3_MIN                                  WDR_CSR_CR_HIST_X3_MIN
#define WDR_CSR_CR_HIST_X3_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X3_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X3_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X3_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X3_MIN_POS)
#define WDR_CSR_CR_HIST_X3_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X3_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X3_MIN_POS))

/* 0x2C : WDR_HIST_9 */
#define WDR_CSR_WDR_HIST_9_OFFSET                               (0x2C)
#define WDR_CSR_CR_HIST_Y3_MAX                                  WDR_CSR_CR_HIST_Y3_MAX
#define WDR_CSR_CR_HIST_Y3_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y3_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y3_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y3_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y3_MAX_POS)
#define WDR_CSR_CR_HIST_Y3_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y3_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y3_MAX_POS))
#define WDR_CSR_CR_HIST_Y3_MIN                                  WDR_CSR_CR_HIST_Y3_MIN
#define WDR_CSR_CR_HIST_Y3_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y3_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y3_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y3_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y3_MIN_POS)
#define WDR_CSR_CR_HIST_Y3_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y3_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y3_MIN_POS))

/* 0x30 : WDR_HIST_10 */
#define WDR_CSR_WDR_HIST_10_OFFSET                              (0x30)
#define WDR_CSR_CR_HIST_X4_MAX                                  WDR_CSR_CR_HIST_X4_MAX
#define WDR_CSR_CR_HIST_X4_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X4_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X4_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X4_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X4_MAX_POS)
#define WDR_CSR_CR_HIST_X4_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X4_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X4_MAX_POS))
#define WDR_CSR_CR_HIST_X4_MIN                                  WDR_CSR_CR_HIST_X4_MIN
#define WDR_CSR_CR_HIST_X4_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X4_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X4_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X4_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X4_MIN_POS)
#define WDR_CSR_CR_HIST_X4_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X4_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X4_MIN_POS))

/* 0x34 : WDR_HIST_11 */
#define WDR_CSR_WDR_HIST_11_OFFSET                              (0x34)
#define WDR_CSR_CR_HIST_Y4_MAX                                  WDR_CSR_CR_HIST_Y4_MAX
#define WDR_CSR_CR_HIST_Y4_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y4_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y4_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y4_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y4_MAX_POS)
#define WDR_CSR_CR_HIST_Y4_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y4_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y4_MAX_POS))
#define WDR_CSR_CR_HIST_Y4_MIN                                  WDR_CSR_CR_HIST_Y4_MIN
#define WDR_CSR_CR_HIST_Y4_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y4_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y4_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y4_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y4_MIN_POS)
#define WDR_CSR_CR_HIST_Y4_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y4_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y4_MIN_POS))

/* 0x38 : WDR_HIST_12 */
#define WDR_CSR_WDR_HIST_12_OFFSET                              (0x38)
#define WDR_CSR_CR_HIST_X5_MAX                                  WDR_CSR_CR_HIST_X5_MAX
#define WDR_CSR_CR_HIST_X5_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X5_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X5_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X5_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X5_MAX_POS)
#define WDR_CSR_CR_HIST_X5_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X5_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X5_MAX_POS))
#define WDR_CSR_CR_HIST_X5_MIN                                  WDR_CSR_CR_HIST_X5_MIN
#define WDR_CSR_CR_HIST_X5_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X5_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X5_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X5_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X5_MIN_POS)
#define WDR_CSR_CR_HIST_X5_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X5_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X5_MIN_POS))

/* 0x3C : WDR_HIST_13 */
#define WDR_CSR_WDR_HIST_13_OFFSET                              (0x3C)
#define WDR_CSR_CR_HIST_Y5_MAX                                  WDR_CSR_CR_HIST_Y5_MAX
#define WDR_CSR_CR_HIST_Y5_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y5_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y5_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y5_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y5_MAX_POS)
#define WDR_CSR_CR_HIST_Y5_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y5_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y5_MAX_POS))
#define WDR_CSR_CR_HIST_Y5_MIN                                  WDR_CSR_CR_HIST_Y5_MIN
#define WDR_CSR_CR_HIST_Y5_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y5_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y5_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y5_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y5_MIN_POS)
#define WDR_CSR_CR_HIST_Y5_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y5_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y5_MIN_POS))

/* 0x40 : WDR_HIST_14 */
#define WDR_CSR_WDR_HIST_14_OFFSET                              (0x40)
#define WDR_CSR_CR_HIST_X6_MAX                                  WDR_CSR_CR_HIST_X6_MAX
#define WDR_CSR_CR_HIST_X6_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X6_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X6_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X6_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X6_MAX_POS)
#define WDR_CSR_CR_HIST_X6_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X6_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X6_MAX_POS))
#define WDR_CSR_CR_HIST_X6_MIN                                  WDR_CSR_CR_HIST_X6_MIN
#define WDR_CSR_CR_HIST_X6_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X6_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X6_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X6_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X6_MIN_POS)
#define WDR_CSR_CR_HIST_X6_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X6_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X6_MIN_POS))

/* 0x44 : WDR_HIST_15 */
#define WDR_CSR_WDR_HIST_15_OFFSET                              (0x44)
#define WDR_CSR_CR_HIST_Y6_MAX                                  WDR_CSR_CR_HIST_Y6_MAX
#define WDR_CSR_CR_HIST_Y6_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y6_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y6_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y6_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y6_MAX_POS)
#define WDR_CSR_CR_HIST_Y6_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y6_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y6_MAX_POS))
#define WDR_CSR_CR_HIST_Y6_MIN                                  WDR_CSR_CR_HIST_Y6_MIN
#define WDR_CSR_CR_HIST_Y6_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y6_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y6_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y6_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y6_MIN_POS)
#define WDR_CSR_CR_HIST_Y6_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y6_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y6_MIN_POS))

/* 0x48 : WDR_HIST_16 */
#define WDR_CSR_WDR_HIST_16_OFFSET                              (0x48)
#define WDR_CSR_CR_HIST_X7_MAX                                  WDR_CSR_CR_HIST_X7_MAX
#define WDR_CSR_CR_HIST_X7_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_X7_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_X7_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_X7_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X7_MAX_POS)
#define WDR_CSR_CR_HIST_X7_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X7_MAX_LEN)-1)<<WDR_CSR_CR_HIST_X7_MAX_POS))
#define WDR_CSR_CR_HIST_X7_MIN                                  WDR_CSR_CR_HIST_X7_MIN
#define WDR_CSR_CR_HIST_X7_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_X7_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_X7_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_X7_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X7_MIN_POS)
#define WDR_CSR_CR_HIST_X7_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_X7_MIN_LEN)-1)<<WDR_CSR_CR_HIST_X7_MIN_POS))

/* 0x4C : WDR_HIST_17 */
#define WDR_CSR_WDR_HIST_17_OFFSET                              (0x4C)
#define WDR_CSR_CR_HIST_Y7_MAX                                  WDR_CSR_CR_HIST_Y7_MAX
#define WDR_CSR_CR_HIST_Y7_MAX_POS                              (0U)
#define WDR_CSR_CR_HIST_Y7_MAX_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y7_MAX_MSK                              (((1U<<WDR_CSR_CR_HIST_Y7_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y7_MAX_POS)
#define WDR_CSR_CR_HIST_Y7_MAX_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y7_MAX_LEN)-1)<<WDR_CSR_CR_HIST_Y7_MAX_POS))
#define WDR_CSR_CR_HIST_Y7_MIN                                  WDR_CSR_CR_HIST_Y7_MIN
#define WDR_CSR_CR_HIST_Y7_MIN_POS                              (16U)
#define WDR_CSR_CR_HIST_Y7_MIN_LEN                              (14U)
#define WDR_CSR_CR_HIST_Y7_MIN_MSK                              (((1U<<WDR_CSR_CR_HIST_Y7_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y7_MIN_POS)
#define WDR_CSR_CR_HIST_Y7_MIN_UMSK                             (~(((1U<<WDR_CSR_CR_HIST_Y7_MIN_LEN)-1)<<WDR_CSR_CR_HIST_Y7_MIN_POS))

/* 0x60 : WDR_CURVE_MISC */
#define WDR_CSR_WDR_CURVE_MISC_OFFSET                           (0x60)
#define WDR_CSR_CR_CURVE_EN                                     WDR_CSR_CR_CURVE_EN
#define WDR_CSR_CR_CURVE_EN_POS                                 (0U)
#define WDR_CSR_CR_CURVE_EN_LEN                                 (1U)
#define WDR_CSR_CR_CURVE_EN_MSK                                 (((1U<<WDR_CSR_CR_CURVE_EN_LEN)-1)<<WDR_CSR_CR_CURVE_EN_POS)
#define WDR_CSR_CR_CURVE_EN_UMSK                                (~(((1U<<WDR_CSR_CR_CURVE_EN_LEN)-1)<<WDR_CSR_CR_CURVE_EN_POS))
#define WDR_CSR_CR_CURVE_WEIGHT                                 WDR_CSR_CR_CURVE_WEIGHT
#define WDR_CSR_CR_CURVE_WEIGHT_POS                             (8U)
#define WDR_CSR_CR_CURVE_WEIGHT_LEN                             (5U)
#define WDR_CSR_CR_CURVE_WEIGHT_MSK                             (((1U<<WDR_CSR_CR_CURVE_WEIGHT_LEN)-1)<<WDR_CSR_CR_CURVE_WEIGHT_POS)
#define WDR_CSR_CR_CURVE_WEIGHT_UMSK                            (~(((1U<<WDR_CSR_CR_CURVE_WEIGHT_LEN)-1)<<WDR_CSR_CR_CURVE_WEIGHT_POS))

/* 0x64 : WDR_ENH_1 */
#define WDR_CSR_WDR_ENH_1_OFFSET                                (0x64)
#define WDR_CSR_CR_ENH_EN                                       WDR_CSR_CR_ENH_EN
#define WDR_CSR_CR_ENH_EN_POS                                   (0U)
#define WDR_CSR_CR_ENH_EN_LEN                                   (1U)
#define WDR_CSR_CR_ENH_EN_MSK                                   (((1U<<WDR_CSR_CR_ENH_EN_LEN)-1)<<WDR_CSR_CR_ENH_EN_POS)
#define WDR_CSR_CR_ENH_EN_UMSK                                  (~(((1U<<WDR_CSR_CR_ENH_EN_LEN)-1)<<WDR_CSR_CR_ENH_EN_POS))
#define WDR_CSR_CR_ENH_WEIGHT                                   WDR_CSR_CR_ENH_WEIGHT
#define WDR_CSR_CR_ENH_WEIGHT_POS                               (8U)
#define WDR_CSR_CR_ENH_WEIGHT_LEN                               (5U)
#define WDR_CSR_CR_ENH_WEIGHT_MSK                               (((1U<<WDR_CSR_CR_ENH_WEIGHT_LEN)-1)<<WDR_CSR_CR_ENH_WEIGHT_POS)
#define WDR_CSR_CR_ENH_WEIGHT_UMSK                              (~(((1U<<WDR_CSR_CR_ENH_WEIGHT_LEN)-1)<<WDR_CSR_CR_ENH_WEIGHT_POS))
#define WDR_CSR_CR_ENH_Y_OFFSET                                 WDR_CSR_CR_ENH_Y_OFFSET
#define WDR_CSR_CR_ENH_Y_OFFSET_POS                             (16U)
#define WDR_CSR_CR_ENH_Y_OFFSET_LEN                             (12U)
#define WDR_CSR_CR_ENH_Y_OFFSET_MSK                             (((1U<<WDR_CSR_CR_ENH_Y_OFFSET_LEN)-1)<<WDR_CSR_CR_ENH_Y_OFFSET_POS)
#define WDR_CSR_CR_ENH_Y_OFFSET_UMSK                            (~(((1U<<WDR_CSR_CR_ENH_Y_OFFSET_LEN)-1)<<WDR_CSR_CR_ENH_Y_OFFSET_POS))

/* 0xFC : WDR_W1P */
#define WDR_CSR_WDR_W1P_OFFSET                                  (0xFC)
#define WDR_CSR_CR_SW_SHD                                       WDR_CSR_CR_SW_SHD
#define WDR_CSR_CR_SW_SHD_POS                                   (0U)
#define WDR_CSR_CR_SW_SHD_LEN                                   (1U)
#define WDR_CSR_CR_SW_SHD_MSK                                   (((1U<<WDR_CSR_CR_SW_SHD_LEN)-1)<<WDR_CSR_CR_SW_SHD_POS)
#define WDR_CSR_CR_SW_SHD_UMSK                                  (~(((1U<<WDR_CSR_CR_SW_SHD_LEN)-1)<<WDR_CSR_CR_SW_SHD_POS))
#define WDR_CSR_CR_WDR_INT_CLR                                  WDR_CSR_CR_WDR_INT_CLR
#define WDR_CSR_CR_WDR_INT_CLR_POS                              (2U)
#define WDR_CSR_CR_WDR_INT_CLR_LEN                              (1U)
#define WDR_CSR_CR_WDR_INT_CLR_MSK                              (((1U<<WDR_CSR_CR_WDR_INT_CLR_LEN)-1)<<WDR_CSR_CR_WDR_INT_CLR_POS)
#define WDR_CSR_CR_WDR_INT_CLR_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_INT_CLR_LEN)-1)<<WDR_CSR_CR_WDR_INT_CLR_POS))

/* 0x100 : WDR_CURVE_1 */
#define WDR_CSR_WDR_CURVE_1_OFFSET                              (0x100)
#define WDR_CSR_CR_WDR_CURVE_0                                  WDR_CSR_CR_WDR_CURVE_0
#define WDR_CSR_CR_WDR_CURVE_0_POS                              (0U)
#define WDR_CSR_CR_WDR_CURVE_0_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_0_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_0_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_0_POS)
#define WDR_CSR_CR_WDR_CURVE_0_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_0_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_0_POS))
#define WDR_CSR_CR_WDR_CURVE_1                                  WDR_CSR_CR_WDR_CURVE_1
#define WDR_CSR_CR_WDR_CURVE_1_POS                              (16U)
#define WDR_CSR_CR_WDR_CURVE_1_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_1_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_1_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_1_POS)
#define WDR_CSR_CR_WDR_CURVE_1_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_1_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_1_POS))

/* 0x104 : WDR_CURVE_2 */
#define WDR_CSR_WDR_CURVE_2_OFFSET                              (0x104)
#define WDR_CSR_CR_WDR_CURVE_2                                  WDR_CSR_CR_WDR_CURVE_2
#define WDR_CSR_CR_WDR_CURVE_2_POS                              (0U)
#define WDR_CSR_CR_WDR_CURVE_2_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_2_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_2_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_2_POS)
#define WDR_CSR_CR_WDR_CURVE_2_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_2_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_2_POS))
#define WDR_CSR_CR_WDR_CURVE_3                                  WDR_CSR_CR_WDR_CURVE_3
#define WDR_CSR_CR_WDR_CURVE_3_POS                              (16U)
#define WDR_CSR_CR_WDR_CURVE_3_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_3_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_3_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_3_POS)
#define WDR_CSR_CR_WDR_CURVE_3_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_3_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_3_POS))

/* 0x108 : WDR_CURVE_3 */
#define WDR_CSR_WDR_CURVE_3_OFFSET                              (0x108)
#define WDR_CSR_CR_WDR_CURVE_4                                  WDR_CSR_CR_WDR_CURVE_4
#define WDR_CSR_CR_WDR_CURVE_4_POS                              (0U)
#define WDR_CSR_CR_WDR_CURVE_4_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_4_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_4_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_4_POS)
#define WDR_CSR_CR_WDR_CURVE_4_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_4_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_4_POS))
#define WDR_CSR_CR_WDR_CURVE_5                                  WDR_CSR_CR_WDR_CURVE_5
#define WDR_CSR_CR_WDR_CURVE_5_POS                              (16U)
#define WDR_CSR_CR_WDR_CURVE_5_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_5_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_5_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_5_POS)
#define WDR_CSR_CR_WDR_CURVE_5_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_5_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_5_POS))

/* 0x10C : WDR_CURVE_4 */
#define WDR_CSR_WDR_CURVE_4_OFFSET                              (0x10C)
#define WDR_CSR_CR_WDR_CURVE_6                                  WDR_CSR_CR_WDR_CURVE_6
#define WDR_CSR_CR_WDR_CURVE_6_POS                              (0U)
#define WDR_CSR_CR_WDR_CURVE_6_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_6_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_6_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_6_POS)
#define WDR_CSR_CR_WDR_CURVE_6_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_6_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_6_POS))
#define WDR_CSR_CR_WDR_CURVE_7                                  WDR_CSR_CR_WDR_CURVE_7
#define WDR_CSR_CR_WDR_CURVE_7_POS                              (16U)
#define WDR_CSR_CR_WDR_CURVE_7_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_7_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_7_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_7_POS)
#define WDR_CSR_CR_WDR_CURVE_7_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_7_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_7_POS))

/* 0x110 : WDR_CURVE_5 */
#define WDR_CSR_WDR_CURVE_5_OFFSET                              (0x110)
#define WDR_CSR_CR_WDR_CURVE_8                                  WDR_CSR_CR_WDR_CURVE_8
#define WDR_CSR_CR_WDR_CURVE_8_POS                              (0U)
#define WDR_CSR_CR_WDR_CURVE_8_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_8_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_8_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_8_POS)
#define WDR_CSR_CR_WDR_CURVE_8_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_8_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_8_POS))
#define WDR_CSR_CR_WDR_CURVE_9                                  WDR_CSR_CR_WDR_CURVE_9
#define WDR_CSR_CR_WDR_CURVE_9_POS                              (16U)
#define WDR_CSR_CR_WDR_CURVE_9_LEN                              (10U)
#define WDR_CSR_CR_WDR_CURVE_9_MSK                              (((1U<<WDR_CSR_CR_WDR_CURVE_9_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_9_POS)
#define WDR_CSR_CR_WDR_CURVE_9_UMSK                             (~(((1U<<WDR_CSR_CR_WDR_CURVE_9_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_9_POS))

/* 0x114 : WDR_CURVE_6 */
#define WDR_CSR_WDR_CURVE_6_OFFSET                              (0x114)
#define WDR_CSR_CR_WDR_CURVE_10                                 WDR_CSR_CR_WDR_CURVE_10
#define WDR_CSR_CR_WDR_CURVE_10_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_10_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_10_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_10_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_10_POS)
#define WDR_CSR_CR_WDR_CURVE_10_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_10_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_10_POS))
#define WDR_CSR_CR_WDR_CURVE_11                                 WDR_CSR_CR_WDR_CURVE_11
#define WDR_CSR_CR_WDR_CURVE_11_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_11_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_11_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_11_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_11_POS)
#define WDR_CSR_CR_WDR_CURVE_11_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_11_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_11_POS))

/* 0x118 : WDR_CURVE_7 */
#define WDR_CSR_WDR_CURVE_7_OFFSET                              (0x118)
#define WDR_CSR_CR_WDR_CURVE_12                                 WDR_CSR_CR_WDR_CURVE_12
#define WDR_CSR_CR_WDR_CURVE_12_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_12_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_12_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_12_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_12_POS)
#define WDR_CSR_CR_WDR_CURVE_12_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_12_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_12_POS))
#define WDR_CSR_CR_WDR_CURVE_13                                 WDR_CSR_CR_WDR_CURVE_13
#define WDR_CSR_CR_WDR_CURVE_13_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_13_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_13_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_13_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_13_POS)
#define WDR_CSR_CR_WDR_CURVE_13_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_13_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_13_POS))

/* 0x11C : WDR_CURVE_8 */
#define WDR_CSR_WDR_CURVE_8_OFFSET                              (0x11C)
#define WDR_CSR_CR_WDR_CURVE_14                                 WDR_CSR_CR_WDR_CURVE_14
#define WDR_CSR_CR_WDR_CURVE_14_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_14_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_14_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_14_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_14_POS)
#define WDR_CSR_CR_WDR_CURVE_14_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_14_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_14_POS))
#define WDR_CSR_CR_WDR_CURVE_15                                 WDR_CSR_CR_WDR_CURVE_15
#define WDR_CSR_CR_WDR_CURVE_15_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_15_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_15_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_15_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_15_POS)
#define WDR_CSR_CR_WDR_CURVE_15_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_15_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_15_POS))

/* 0x120 : WDR_CURVE_9 */
#define WDR_CSR_WDR_CURVE_9_OFFSET                              (0x120)
#define WDR_CSR_CR_WDR_CURVE_16                                 WDR_CSR_CR_WDR_CURVE_16
#define WDR_CSR_CR_WDR_CURVE_16_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_16_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_16_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_16_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_16_POS)
#define WDR_CSR_CR_WDR_CURVE_16_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_16_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_16_POS))
#define WDR_CSR_CR_WDR_CURVE_17                                 WDR_CSR_CR_WDR_CURVE_17
#define WDR_CSR_CR_WDR_CURVE_17_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_17_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_17_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_17_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_17_POS)
#define WDR_CSR_CR_WDR_CURVE_17_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_17_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_17_POS))

/* 0x124 : WDR_CURVE_10 */
#define WDR_CSR_WDR_CURVE_10_OFFSET                             (0x124)
#define WDR_CSR_CR_WDR_CURVE_18                                 WDR_CSR_CR_WDR_CURVE_18
#define WDR_CSR_CR_WDR_CURVE_18_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_18_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_18_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_18_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_18_POS)
#define WDR_CSR_CR_WDR_CURVE_18_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_18_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_18_POS))
#define WDR_CSR_CR_WDR_CURVE_19                                 WDR_CSR_CR_WDR_CURVE_19
#define WDR_CSR_CR_WDR_CURVE_19_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_19_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_19_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_19_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_19_POS)
#define WDR_CSR_CR_WDR_CURVE_19_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_19_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_19_POS))

/* 0x128 : WDR_CURVE_11 */
#define WDR_CSR_WDR_CURVE_11_OFFSET                             (0x128)
#define WDR_CSR_CR_WDR_CURVE_20                                 WDR_CSR_CR_WDR_CURVE_20
#define WDR_CSR_CR_WDR_CURVE_20_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_20_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_20_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_20_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_20_POS)
#define WDR_CSR_CR_WDR_CURVE_20_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_20_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_20_POS))
#define WDR_CSR_CR_WDR_CURVE_21                                 WDR_CSR_CR_WDR_CURVE_21
#define WDR_CSR_CR_WDR_CURVE_21_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_21_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_21_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_21_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_21_POS)
#define WDR_CSR_CR_WDR_CURVE_21_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_21_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_21_POS))

/* 0x12C : WDR_CURVE_12 */
#define WDR_CSR_WDR_CURVE_12_OFFSET                             (0x12C)
#define WDR_CSR_CR_WDR_CURVE_22                                 WDR_CSR_CR_WDR_CURVE_22
#define WDR_CSR_CR_WDR_CURVE_22_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_22_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_22_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_22_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_22_POS)
#define WDR_CSR_CR_WDR_CURVE_22_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_22_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_22_POS))
#define WDR_CSR_CR_WDR_CURVE_23                                 WDR_CSR_CR_WDR_CURVE_23
#define WDR_CSR_CR_WDR_CURVE_23_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_23_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_23_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_23_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_23_POS)
#define WDR_CSR_CR_WDR_CURVE_23_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_23_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_23_POS))

/* 0x130 : WDR_CURVE_13 */
#define WDR_CSR_WDR_CURVE_13_OFFSET                             (0x130)
#define WDR_CSR_CR_WDR_CURVE_24                                 WDR_CSR_CR_WDR_CURVE_24
#define WDR_CSR_CR_WDR_CURVE_24_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_24_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_24_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_24_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_24_POS)
#define WDR_CSR_CR_WDR_CURVE_24_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_24_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_24_POS))
#define WDR_CSR_CR_WDR_CURVE_25                                 WDR_CSR_CR_WDR_CURVE_25
#define WDR_CSR_CR_WDR_CURVE_25_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_25_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_25_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_25_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_25_POS)
#define WDR_CSR_CR_WDR_CURVE_25_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_25_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_25_POS))

/* 0x134 : WDR_CURVE_14 */
#define WDR_CSR_WDR_CURVE_14_OFFSET                             (0x134)
#define WDR_CSR_CR_WDR_CURVE_26                                 WDR_CSR_CR_WDR_CURVE_26
#define WDR_CSR_CR_WDR_CURVE_26_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_26_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_26_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_26_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_26_POS)
#define WDR_CSR_CR_WDR_CURVE_26_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_26_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_26_POS))
#define WDR_CSR_CR_WDR_CURVE_27                                 WDR_CSR_CR_WDR_CURVE_27
#define WDR_CSR_CR_WDR_CURVE_27_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_27_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_27_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_27_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_27_POS)
#define WDR_CSR_CR_WDR_CURVE_27_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_27_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_27_POS))

/* 0x138 : WDR_CURVE_15 */
#define WDR_CSR_WDR_CURVE_15_OFFSET                             (0x138)
#define WDR_CSR_CR_WDR_CURVE_28                                 WDR_CSR_CR_WDR_CURVE_28
#define WDR_CSR_CR_WDR_CURVE_28_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_28_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_28_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_28_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_28_POS)
#define WDR_CSR_CR_WDR_CURVE_28_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_28_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_28_POS))
#define WDR_CSR_CR_WDR_CURVE_29                                 WDR_CSR_CR_WDR_CURVE_29
#define WDR_CSR_CR_WDR_CURVE_29_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_29_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_29_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_29_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_29_POS)
#define WDR_CSR_CR_WDR_CURVE_29_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_29_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_29_POS))

/* 0x13C : WDR_CURVE_16 */
#define WDR_CSR_WDR_CURVE_16_OFFSET                             (0x13C)
#define WDR_CSR_CR_WDR_CURVE_30                                 WDR_CSR_CR_WDR_CURVE_30
#define WDR_CSR_CR_WDR_CURVE_30_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_30_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_30_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_30_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_30_POS)
#define WDR_CSR_CR_WDR_CURVE_30_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_30_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_30_POS))
#define WDR_CSR_CR_WDR_CURVE_31                                 WDR_CSR_CR_WDR_CURVE_31
#define WDR_CSR_CR_WDR_CURVE_31_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_31_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_31_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_31_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_31_POS)
#define WDR_CSR_CR_WDR_CURVE_31_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_31_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_31_POS))

/* 0x140 : WDR_CURVE_17 */
#define WDR_CSR_WDR_CURVE_17_OFFSET                             (0x140)
#define WDR_CSR_CR_WDR_CURVE_32                                 WDR_CSR_CR_WDR_CURVE_32
#define WDR_CSR_CR_WDR_CURVE_32_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_32_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_32_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_32_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_32_POS)
#define WDR_CSR_CR_WDR_CURVE_32_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_32_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_32_POS))
#define WDR_CSR_CR_WDR_CURVE_33                                 WDR_CSR_CR_WDR_CURVE_33
#define WDR_CSR_CR_WDR_CURVE_33_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_33_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_33_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_33_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_33_POS)
#define WDR_CSR_CR_WDR_CURVE_33_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_33_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_33_POS))

/* 0x144 : WDR_CURVE_18 */
#define WDR_CSR_WDR_CURVE_18_OFFSET                             (0x144)
#define WDR_CSR_CR_WDR_CURVE_34                                 WDR_CSR_CR_WDR_CURVE_34
#define WDR_CSR_CR_WDR_CURVE_34_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_34_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_34_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_34_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_34_POS)
#define WDR_CSR_CR_WDR_CURVE_34_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_34_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_34_POS))
#define WDR_CSR_CR_WDR_CURVE_35                                 WDR_CSR_CR_WDR_CURVE_35
#define WDR_CSR_CR_WDR_CURVE_35_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_35_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_35_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_35_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_35_POS)
#define WDR_CSR_CR_WDR_CURVE_35_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_35_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_35_POS))

/* 0x148 : WDR_CURVE_19 */
#define WDR_CSR_WDR_CURVE_19_OFFSET                             (0x148)
#define WDR_CSR_CR_WDR_CURVE_36                                 WDR_CSR_CR_WDR_CURVE_36
#define WDR_CSR_CR_WDR_CURVE_36_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_36_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_36_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_36_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_36_POS)
#define WDR_CSR_CR_WDR_CURVE_36_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_36_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_36_POS))
#define WDR_CSR_CR_WDR_CURVE_37                                 WDR_CSR_CR_WDR_CURVE_37
#define WDR_CSR_CR_WDR_CURVE_37_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_37_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_37_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_37_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_37_POS)
#define WDR_CSR_CR_WDR_CURVE_37_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_37_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_37_POS))

/* 0x14C : WDR_CURVE_20 */
#define WDR_CSR_WDR_CURVE_20_OFFSET                             (0x14C)
#define WDR_CSR_CR_WDR_CURVE_38                                 WDR_CSR_CR_WDR_CURVE_38
#define WDR_CSR_CR_WDR_CURVE_38_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_38_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_38_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_38_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_38_POS)
#define WDR_CSR_CR_WDR_CURVE_38_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_38_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_38_POS))
#define WDR_CSR_CR_WDR_CURVE_39                                 WDR_CSR_CR_WDR_CURVE_39
#define WDR_CSR_CR_WDR_CURVE_39_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_39_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_39_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_39_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_39_POS)
#define WDR_CSR_CR_WDR_CURVE_39_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_39_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_39_POS))

/* 0x150 : WDR_CURVE_21 */
#define WDR_CSR_WDR_CURVE_21_OFFSET                             (0x150)
#define WDR_CSR_CR_WDR_CURVE_40                                 WDR_CSR_CR_WDR_CURVE_40
#define WDR_CSR_CR_WDR_CURVE_40_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_40_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_40_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_40_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_40_POS)
#define WDR_CSR_CR_WDR_CURVE_40_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_40_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_40_POS))
#define WDR_CSR_CR_WDR_CURVE_41                                 WDR_CSR_CR_WDR_CURVE_41
#define WDR_CSR_CR_WDR_CURVE_41_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_41_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_41_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_41_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_41_POS)
#define WDR_CSR_CR_WDR_CURVE_41_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_41_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_41_POS))

/* 0x154 : WDR_CURVE_22 */
#define WDR_CSR_WDR_CURVE_22_OFFSET                             (0x154)
#define WDR_CSR_CR_WDR_CURVE_42                                 WDR_CSR_CR_WDR_CURVE_42
#define WDR_CSR_CR_WDR_CURVE_42_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_42_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_42_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_42_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_42_POS)
#define WDR_CSR_CR_WDR_CURVE_42_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_42_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_42_POS))
#define WDR_CSR_CR_WDR_CURVE_43                                 WDR_CSR_CR_WDR_CURVE_43
#define WDR_CSR_CR_WDR_CURVE_43_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_43_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_43_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_43_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_43_POS)
#define WDR_CSR_CR_WDR_CURVE_43_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_43_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_43_POS))

/* 0x158 : WDR_CURVE_23 */
#define WDR_CSR_WDR_CURVE_23_OFFSET                             (0x158)
#define WDR_CSR_CR_WDR_CURVE_44                                 WDR_CSR_CR_WDR_CURVE_44
#define WDR_CSR_CR_WDR_CURVE_44_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_44_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_44_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_44_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_44_POS)
#define WDR_CSR_CR_WDR_CURVE_44_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_44_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_44_POS))
#define WDR_CSR_CR_WDR_CURVE_45                                 WDR_CSR_CR_WDR_CURVE_45
#define WDR_CSR_CR_WDR_CURVE_45_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_45_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_45_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_45_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_45_POS)
#define WDR_CSR_CR_WDR_CURVE_45_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_45_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_45_POS))

/* 0x15C : WDR_CURVE_24 */
#define WDR_CSR_WDR_CURVE_24_OFFSET                             (0x15C)
#define WDR_CSR_CR_WDR_CURVE_46                                 WDR_CSR_CR_WDR_CURVE_46
#define WDR_CSR_CR_WDR_CURVE_46_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_46_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_46_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_46_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_46_POS)
#define WDR_CSR_CR_WDR_CURVE_46_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_46_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_46_POS))
#define WDR_CSR_CR_WDR_CURVE_47                                 WDR_CSR_CR_WDR_CURVE_47
#define WDR_CSR_CR_WDR_CURVE_47_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_47_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_47_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_47_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_47_POS)
#define WDR_CSR_CR_WDR_CURVE_47_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_47_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_47_POS))

/* 0x160 : WDR_CURVE_25 */
#define WDR_CSR_WDR_CURVE_25_OFFSET                             (0x160)
#define WDR_CSR_CR_WDR_CURVE_48                                 WDR_CSR_CR_WDR_CURVE_48
#define WDR_CSR_CR_WDR_CURVE_48_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_48_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_48_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_48_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_48_POS)
#define WDR_CSR_CR_WDR_CURVE_48_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_48_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_48_POS))
#define WDR_CSR_CR_WDR_CURVE_49                                 WDR_CSR_CR_WDR_CURVE_49
#define WDR_CSR_CR_WDR_CURVE_49_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_49_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_49_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_49_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_49_POS)
#define WDR_CSR_CR_WDR_CURVE_49_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_49_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_49_POS))

/* 0x164 : WDR_CURVE_26 */
#define WDR_CSR_WDR_CURVE_26_OFFSET                             (0x164)
#define WDR_CSR_CR_WDR_CURVE_50                                 WDR_CSR_CR_WDR_CURVE_50
#define WDR_CSR_CR_WDR_CURVE_50_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_50_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_50_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_50_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_50_POS)
#define WDR_CSR_CR_WDR_CURVE_50_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_50_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_50_POS))
#define WDR_CSR_CR_WDR_CURVE_51                                 WDR_CSR_CR_WDR_CURVE_51
#define WDR_CSR_CR_WDR_CURVE_51_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_51_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_51_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_51_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_51_POS)
#define WDR_CSR_CR_WDR_CURVE_51_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_51_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_51_POS))

/* 0x168 : WDR_CURVE_27 */
#define WDR_CSR_WDR_CURVE_27_OFFSET                             (0x168)
#define WDR_CSR_CR_WDR_CURVE_52                                 WDR_CSR_CR_WDR_CURVE_52
#define WDR_CSR_CR_WDR_CURVE_52_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_52_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_52_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_52_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_52_POS)
#define WDR_CSR_CR_WDR_CURVE_52_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_52_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_52_POS))
#define WDR_CSR_CR_WDR_CURVE_53                                 WDR_CSR_CR_WDR_CURVE_53
#define WDR_CSR_CR_WDR_CURVE_53_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_53_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_53_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_53_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_53_POS)
#define WDR_CSR_CR_WDR_CURVE_53_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_53_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_53_POS))

/* 0x16C : WDR_CURVE_28 */
#define WDR_CSR_WDR_CURVE_28_OFFSET                             (0x16C)
#define WDR_CSR_CR_WDR_CURVE_54                                 WDR_CSR_CR_WDR_CURVE_54
#define WDR_CSR_CR_WDR_CURVE_54_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_54_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_54_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_54_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_54_POS)
#define WDR_CSR_CR_WDR_CURVE_54_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_54_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_54_POS))
#define WDR_CSR_CR_WDR_CURVE_55                                 WDR_CSR_CR_WDR_CURVE_55
#define WDR_CSR_CR_WDR_CURVE_55_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_55_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_55_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_55_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_55_POS)
#define WDR_CSR_CR_WDR_CURVE_55_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_55_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_55_POS))

/* 0x170 : WDR_CURVE_29 */
#define WDR_CSR_WDR_CURVE_29_OFFSET                             (0x170)
#define WDR_CSR_CR_WDR_CURVE_56                                 WDR_CSR_CR_WDR_CURVE_56
#define WDR_CSR_CR_WDR_CURVE_56_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_56_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_56_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_56_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_56_POS)
#define WDR_CSR_CR_WDR_CURVE_56_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_56_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_56_POS))
#define WDR_CSR_CR_WDR_CURVE_57                                 WDR_CSR_CR_WDR_CURVE_57
#define WDR_CSR_CR_WDR_CURVE_57_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_57_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_57_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_57_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_57_POS)
#define WDR_CSR_CR_WDR_CURVE_57_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_57_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_57_POS))

/* 0x174 : WDR_CURVE_30 */
#define WDR_CSR_WDR_CURVE_30_OFFSET                             (0x174)
#define WDR_CSR_CR_WDR_CURVE_58                                 WDR_CSR_CR_WDR_CURVE_58
#define WDR_CSR_CR_WDR_CURVE_58_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_58_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_58_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_58_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_58_POS)
#define WDR_CSR_CR_WDR_CURVE_58_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_58_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_58_POS))
#define WDR_CSR_CR_WDR_CURVE_59                                 WDR_CSR_CR_WDR_CURVE_59
#define WDR_CSR_CR_WDR_CURVE_59_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_59_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_59_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_59_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_59_POS)
#define WDR_CSR_CR_WDR_CURVE_59_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_59_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_59_POS))

/* 0x178 : WDR_CURVE_31 */
#define WDR_CSR_WDR_CURVE_31_OFFSET                             (0x178)
#define WDR_CSR_CR_WDR_CURVE_60                                 WDR_CSR_CR_WDR_CURVE_60
#define WDR_CSR_CR_WDR_CURVE_60_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_60_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_60_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_60_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_60_POS)
#define WDR_CSR_CR_WDR_CURVE_60_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_60_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_60_POS))
#define WDR_CSR_CR_WDR_CURVE_61                                 WDR_CSR_CR_WDR_CURVE_61
#define WDR_CSR_CR_WDR_CURVE_61_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_61_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_61_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_61_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_61_POS)
#define WDR_CSR_CR_WDR_CURVE_61_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_61_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_61_POS))

/* 0x17C : WDR_CURVE_32 */
#define WDR_CSR_WDR_CURVE_32_OFFSET                             (0x17C)
#define WDR_CSR_CR_WDR_CURVE_62                                 WDR_CSR_CR_WDR_CURVE_62
#define WDR_CSR_CR_WDR_CURVE_62_POS                             (0U)
#define WDR_CSR_CR_WDR_CURVE_62_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_62_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_62_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_62_POS)
#define WDR_CSR_CR_WDR_CURVE_62_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_62_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_62_POS))
#define WDR_CSR_CR_WDR_CURVE_63                                 WDR_CSR_CR_WDR_CURVE_63
#define WDR_CSR_CR_WDR_CURVE_63_POS                             (16U)
#define WDR_CSR_CR_WDR_CURVE_63_LEN                             (10U)
#define WDR_CSR_CR_WDR_CURVE_63_MSK                             (((1U<<WDR_CSR_CR_WDR_CURVE_63_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_63_POS)
#define WDR_CSR_CR_WDR_CURVE_63_UMSK                            (~(((1U<<WDR_CSR_CR_WDR_CURVE_63_LEN)-1)<<WDR_CSR_CR_WDR_CURVE_63_POS))

/* 0x188 : WDR_ENH_2 */
#define WDR_CSR_WDR_ENH_2_OFFSET                                (0x188)
#define WDR_CSR_CR_ENH_C_GAIN                                   WDR_CSR_CR_ENH_C_GAIN
#define WDR_CSR_CR_ENH_C_GAIN_POS                               (0U)
#define WDR_CSR_CR_ENH_C_GAIN_LEN                               (15U)
#define WDR_CSR_CR_ENH_C_GAIN_MSK                               (((1U<<WDR_CSR_CR_ENH_C_GAIN_LEN)-1)<<WDR_CSR_CR_ENH_C_GAIN_POS)
#define WDR_CSR_CR_ENH_C_GAIN_UMSK                              (~(((1U<<WDR_CSR_CR_ENH_C_GAIN_LEN)-1)<<WDR_CSR_CR_ENH_C_GAIN_POS))
#define WDR_CSR_CR_ENH_Y_GAIN                                   WDR_CSR_CR_ENH_Y_GAIN
#define WDR_CSR_CR_ENH_Y_GAIN_POS                               (16U)
#define WDR_CSR_CR_ENH_Y_GAIN_LEN                               (15U)
#define WDR_CSR_CR_ENH_Y_GAIN_MSK                               (((1U<<WDR_CSR_CR_ENH_Y_GAIN_LEN)-1)<<WDR_CSR_CR_ENH_Y_GAIN_POS)
#define WDR_CSR_CR_ENH_Y_GAIN_UMSK                              (~(((1U<<WDR_CSR_CR_ENH_Y_GAIN_LEN)-1)<<WDR_CSR_CR_ENH_Y_GAIN_POS))


struct  wdr_csr_reg {
    /* 0x0 : WDR_0 */
    union {
        struct {
            uint32_t cr_wdr_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t sts_wdr_buf_idx                :  1; /* [    1],          r,        0x0 */
            uint32_t cr_wdr_int_mask                :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t sts_wdr_w_cnt                  :  5; /* [ 8: 4],          r,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t cr_cs_gain                     : 13; /* [28:16],        r/w,        0x0 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_0;

    /* 0x4 : WDR_CS_0 */
    union {
        struct {
            uint32_t cr_cs_en                       :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t cr_cs_weight                   :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cr_cs_grey_th                  :  8; /* [23:16],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CS_0;

    /* 0x8 : WDR_HIST_0 */
    union {
        struct {
            uint32_t cr_hist_roi_en                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_0;

    /* 0xC : WDR_HIST_1 */
    union {
        struct {
            uint32_t cr_hist_roi0_weight            :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_hist_roi1_weight            :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_hist_roi2_weight            :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t cr_hist_roi3_weight            :  4; /* [15:12],        r/w,        0x0 */
            uint32_t cr_hist_roi4_weight            :  4; /* [19:16],        r/w,        0x0 */
            uint32_t cr_hist_roi5_weight            :  4; /* [23:20],        r/w,        0x0 */
            uint32_t cr_hist_roi6_weight            :  4; /* [27:24],        r/w,        0x0 */
            uint32_t cr_hist_roi7_weight            :  4; /* [31:28],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_1;

    /* 0x10 : WDR_HIST_2 */
    union {
        struct {
            uint32_t cr_hist_x0_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x0_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_2;

    /* 0x14 : WDR_HIST_3 */
    union {
        struct {
            uint32_t cr_hist_y0_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y0_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_3;

    /* 0x18 : WDR_HIST_4 */
    union {
        struct {
            uint32_t cr_hist_x1_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x1_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_4;

    /* 0x1C : WDR_HIST_5 */
    union {
        struct {
            uint32_t cr_hist_y1_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y1_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_5;

    /* 0x20 : WDR_HIST_6 */
    union {
        struct {
            uint32_t cr_hist_x2_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x2_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_6;

    /* 0x24 : WDR_HIST_7 */
    union {
        struct {
            uint32_t cr_hist_y2_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y2_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_7;

    /* 0x28 : WDR_HIST_8 */
    union {
        struct {
            uint32_t cr_hist_x3_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x3_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_8;

    /* 0x2C : WDR_HIST_9 */
    union {
        struct {
            uint32_t cr_hist_y3_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y3_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_9;

    /* 0x30 : WDR_HIST_10 */
    union {
        struct {
            uint32_t cr_hist_x4_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x4_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_10;

    /* 0x34 : WDR_HIST_11 */
    union {
        struct {
            uint32_t cr_hist_y4_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y4_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_11;

    /* 0x38 : WDR_HIST_12 */
    union {
        struct {
            uint32_t cr_hist_x5_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x5_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_12;

    /* 0x3C : WDR_HIST_13 */
    union {
        struct {
            uint32_t cr_hist_y5_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y5_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_13;

    /* 0x40 : WDR_HIST_14 */
    union {
        struct {
            uint32_t cr_hist_x6_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x6_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_14;

    /* 0x44 : WDR_HIST_15 */
    union {
        struct {
            uint32_t cr_hist_y6_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y6_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_15;

    /* 0x48 : WDR_HIST_16 */
    union {
        struct {
            uint32_t cr_hist_x7_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_x7_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_16;

    /* 0x4C : WDR_HIST_17 */
    union {
        struct {
            uint32_t cr_hist_y7_max                 : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_hist_y7_min                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_HIST_17;

    /* 0x50  reserved */
    uint8_t RESERVED0x50[16];

    /* 0x60 : WDR_CURVE_MISC */
    union {
        struct {
            uint32_t cr_curve_en                    :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t cr_curve_weight                :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_MISC;

    /* 0x64 : WDR_ENH_1 */
    union {
        struct {
            uint32_t cr_enh_en                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t cr_enh_weight                  :  5; /* [12: 8],        r/w,        0x0 */
            uint32_t reserved_13_15                 :  3; /* [15:13],       rsvd,        0x0 */
            uint32_t cr_enh_y_offset                : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_ENH_1;

    /* 0x68  reserved */
    uint8_t RESERVED0x68[148];

    /* 0xFC : WDR_W1P */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1                     :  1; /* [    1],       rsvd,        0x0 */
            uint32_t cr_wdr_int_clr                 :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_31                  : 29; /* [31: 3],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_W1P;

    /* 0x100 : WDR_CURVE_1 */
    union {
        struct {
            uint32_t cr_wdr_curve_0                 : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_1                 : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_1;

    /* 0x104 : WDR_CURVE_2 */
    union {
        struct {
            uint32_t cr_wdr_curve_2                 : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_3                 : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_2;

    /* 0x108 : WDR_CURVE_3 */
    union {
        struct {
            uint32_t cr_wdr_curve_4                 : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_5                 : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_3;

    /* 0x10C : WDR_CURVE_4 */
    union {
        struct {
            uint32_t cr_wdr_curve_6                 : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_7                 : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_4;

    /* 0x110 : WDR_CURVE_5 */
    union {
        struct {
            uint32_t cr_wdr_curve_8                 : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_9                 : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_5;

    /* 0x114 : WDR_CURVE_6 */
    union {
        struct {
            uint32_t cr_wdr_curve_10                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_11                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_6;

    /* 0x118 : WDR_CURVE_7 */
    union {
        struct {
            uint32_t cr_wdr_curve_12                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_13                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_7;

    /* 0x11C : WDR_CURVE_8 */
    union {
        struct {
            uint32_t cr_wdr_curve_14                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_15                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_8;

    /* 0x120 : WDR_CURVE_9 */
    union {
        struct {
            uint32_t cr_wdr_curve_16                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_17                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_9;

    /* 0x124 : WDR_CURVE_10 */
    union {
        struct {
            uint32_t cr_wdr_curve_18                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_19                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_10;

    /* 0x128 : WDR_CURVE_11 */
    union {
        struct {
            uint32_t cr_wdr_curve_20                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_21                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_11;

    /* 0x12C : WDR_CURVE_12 */
    union {
        struct {
            uint32_t cr_wdr_curve_22                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_23                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_12;

    /* 0x130 : WDR_CURVE_13 */
    union {
        struct {
            uint32_t cr_wdr_curve_24                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_25                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_13;

    /* 0x134 : WDR_CURVE_14 */
    union {
        struct {
            uint32_t cr_wdr_curve_26                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_27                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_14;

    /* 0x138 : WDR_CURVE_15 */
    union {
        struct {
            uint32_t cr_wdr_curve_28                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_29                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_15;

    /* 0x13C : WDR_CURVE_16 */
    union {
        struct {
            uint32_t cr_wdr_curve_30                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_31                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_16;

    /* 0x140 : WDR_CURVE_17 */
    union {
        struct {
            uint32_t cr_wdr_curve_32                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_33                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_17;

    /* 0x144 : WDR_CURVE_18 */
    union {
        struct {
            uint32_t cr_wdr_curve_34                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_35                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_18;

    /* 0x148 : WDR_CURVE_19 */
    union {
        struct {
            uint32_t cr_wdr_curve_36                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_37                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_19;

    /* 0x14C : WDR_CURVE_20 */
    union {
        struct {
            uint32_t cr_wdr_curve_38                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_39                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_20;

    /* 0x150 : WDR_CURVE_21 */
    union {
        struct {
            uint32_t cr_wdr_curve_40                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_41                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_21;

    /* 0x154 : WDR_CURVE_22 */
    union {
        struct {
            uint32_t cr_wdr_curve_42                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_43                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_22;

    /* 0x158 : WDR_CURVE_23 */
    union {
        struct {
            uint32_t cr_wdr_curve_44                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_45                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_23;

    /* 0x15C : WDR_CURVE_24 */
    union {
        struct {
            uint32_t cr_wdr_curve_46                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_47                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_24;

    /* 0x160 : WDR_CURVE_25 */
    union {
        struct {
            uint32_t cr_wdr_curve_48                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_49                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_25;

    /* 0x164 : WDR_CURVE_26 */
    union {
        struct {
            uint32_t cr_wdr_curve_50                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_51                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_26;

    /* 0x168 : WDR_CURVE_27 */
    union {
        struct {
            uint32_t cr_wdr_curve_52                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_53                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_27;

    /* 0x16C : WDR_CURVE_28 */
    union {
        struct {
            uint32_t cr_wdr_curve_54                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_55                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_28;

    /* 0x170 : WDR_CURVE_29 */
    union {
        struct {
            uint32_t cr_wdr_curve_56                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_57                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_29;

    /* 0x174 : WDR_CURVE_30 */
    union {
        struct {
            uint32_t cr_wdr_curve_58                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_59                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_30;

    /* 0x178 : WDR_CURVE_31 */
    union {
        struct {
            uint32_t cr_wdr_curve_60                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_61                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_31;

    /* 0x17C : WDR_CURVE_32 */
    union {
        struct {
            uint32_t cr_wdr_curve_62                : 10; /* [ 9: 0],        r/w,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_wdr_curve_63                : 10; /* [25:16],        r/w,        0x0 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_CURVE_32;

    /* 0x180  reserved */
    uint8_t RESERVED0x180[8];

    /* 0x188 : WDR_ENH_2 */
    union {
        struct {
            uint32_t cr_enh_c_gain                  : 15; /* [14: 0],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t cr_enh_y_gain                  : 15; /* [30:16],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } WDR_ENH_2;

};

typedef volatile struct wdr_csr_reg wdr_csr_reg_t;


#endif  /* __WDR_CSR_REG_H__ */
