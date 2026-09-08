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

#ifndef  __MM_MISC_REG_H__
#define  __MM_MISC_REG_H__

#include "ipro7.h"

/* 0x0 : mm_misc_0 */
#define MM_MISC_0_OFFSET                                        (0x0)
#define MM_MISC_REG_FORCE_PCLK_ON                               MM_MISC_REG_FORCE_PCLK_ON
#define MM_MISC_REG_FORCE_PCLK_ON_POS                           (0U)
#define MM_MISC_REG_FORCE_PCLK_ON_LEN                           (16U)
#define MM_MISC_REG_FORCE_PCLK_ON_MSK                           (((1U<<MM_MISC_REG_FORCE_PCLK_ON_LEN)-1)<<MM_MISC_REG_FORCE_PCLK_ON_POS)
#define MM_MISC_REG_FORCE_PCLK_ON_UMSK                          (~(((1U<<MM_MISC_REG_FORCE_PCLK_ON_LEN)-1)<<MM_MISC_REG_FORCE_PCLK_ON_POS))
#define MM_MISC_CR_SNSR_PWDN                                    MM_MISC_CR_SNSR_PWDN
#define MM_MISC_CR_SNSR_PWDN_POS                                (30U)
#define MM_MISC_CR_SNSR_PWDN_LEN                                (1U)
#define MM_MISC_CR_SNSR_PWDN_MSK                                (((1U<<MM_MISC_CR_SNSR_PWDN_LEN)-1)<<MM_MISC_CR_SNSR_PWDN_POS)
#define MM_MISC_CR_SNSR_PWDN_UMSK                               (~(((1U<<MM_MISC_CR_SNSR_PWDN_LEN)-1)<<MM_MISC_CR_SNSR_PWDN_POS))
#define MM_MISC_CR_SNSR_RESET                                   MM_MISC_CR_SNSR_RESET
#define MM_MISC_CR_SNSR_RESET_POS                               (31U)
#define MM_MISC_CR_SNSR_RESET_LEN                               (1U)
#define MM_MISC_CR_SNSR_RESET_MSK                               (((1U<<MM_MISC_CR_SNSR_RESET_LEN)-1)<<MM_MISC_CR_SNSR_RESET_POS)
#define MM_MISC_CR_SNSR_RESET_UMSK                              (~(((1U<<MM_MISC_CR_SNSR_RESET_LEN)-1)<<MM_MISC_CR_SNSR_RESET_POS))

/* 0x4 : mm_misc_1 */
#define MM_MISC_1_OFFSET                                        (0x4)
#define MM_MISC_REG_DVP_MUX_SH_ISP                              MM_MISC_REG_DVP_MUX_SH_ISP
#define MM_MISC_REG_DVP_MUX_SH_ISP_POS                          (0U)
#define MM_MISC_REG_DVP_MUX_SH_ISP_LEN                          (1U)
#define MM_MISC_REG_DVP_MUX_SH_ISP_MSK                          (((1U<<MM_MISC_REG_DVP_MUX_SH_ISP_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_ISP_POS)
#define MM_MISC_REG_DVP_MUX_SH_ISP_UMSK                         (~(((1U<<MM_MISC_REG_DVP_MUX_SH_ISP_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_ISP_POS))
#define MM_MISC_REG_DVP_MUX_SH_SCALER                           MM_MISC_REG_DVP_MUX_SH_SCALER
#define MM_MISC_REG_DVP_MUX_SH_SCALER_POS                       (1U)
#define MM_MISC_REG_DVP_MUX_SH_SCALER_LEN                       (1U)
#define MM_MISC_REG_DVP_MUX_SH_SCALER_MSK                       (((1U<<MM_MISC_REG_DVP_MUX_SH_SCALER_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_SCALER_POS)
#define MM_MISC_REG_DVP_MUX_SH_SCALER_UMSK                      (~(((1U<<MM_MISC_REG_DVP_MUX_SH_SCALER_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_SCALER_POS))
#define MM_MISC_REG_DVP_MUX_SH_OSD_BLEND                        MM_MISC_REG_DVP_MUX_SH_OSD_BLEND
#define MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_POS                    (2U)
#define MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_LEN                    (1U)
#define MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_MSK                    (((1U<<MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_POS)
#define MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_UMSK                   (~(((1U<<MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_OSD_BLEND_POS))
#define MM_MISC_REG_DVP_MUX_SH_OSD_DRAW                         MM_MISC_REG_DVP_MUX_SH_OSD_DRAW
#define MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_POS                     (3U)
#define MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_LEN                     (1U)
#define MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_MSK                     (((1U<<MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_POS)
#define MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_UMSK                    (~(((1U<<MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_OSD_DRAW_POS))
#define MM_MISC_REG_DVP_MUX_SH_422TO444                         MM_MISC_REG_DVP_MUX_SH_422TO444
#define MM_MISC_REG_DVP_MUX_SH_422TO444_POS                     (4U)
#define MM_MISC_REG_DVP_MUX_SH_422TO444_LEN                     (1U)
#define MM_MISC_REG_DVP_MUX_SH_422TO444_MSK                     (((1U<<MM_MISC_REG_DVP_MUX_SH_422TO444_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_422TO444_POS)
#define MM_MISC_REG_DVP_MUX_SH_422TO444_UMSK                    (~(((1U<<MM_MISC_REG_DVP_MUX_SH_422TO444_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_422TO444_POS))
#define MM_MISC_REG_DVP_MUX_SH_422TO420                         MM_MISC_REG_DVP_MUX_SH_422TO420
#define MM_MISC_REG_DVP_MUX_SH_422TO420_POS                     (5U)
#define MM_MISC_REG_DVP_MUX_SH_422TO420_LEN                     (1U)
#define MM_MISC_REG_DVP_MUX_SH_422TO420_MSK                     (((1U<<MM_MISC_REG_DVP_MUX_SH_422TO420_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_422TO420_POS)
#define MM_MISC_REG_DVP_MUX_SH_422TO420_UMSK                    (~(((1U<<MM_MISC_REG_DVP_MUX_SH_422TO420_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_422TO420_POS))
#define MM_MISC_REG_DVP_MUX_SH_TRA                              MM_MISC_REG_DVP_MUX_SH_TRA
#define MM_MISC_REG_DVP_MUX_SH_TRA_POS                          (6U)
#define MM_MISC_REG_DVP_MUX_SH_TRA_LEN                          (1U)
#define MM_MISC_REG_DVP_MUX_SH_TRA_MSK                          (((1U<<MM_MISC_REG_DVP_MUX_SH_TRA_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_TRA_POS)
#define MM_MISC_REG_DVP_MUX_SH_TRA_UMSK                         (~(((1U<<MM_MISC_REG_DVP_MUX_SH_TRA_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_TRA_POS))
#define MM_MISC_REG_DVP_MUX_SH_444TO422                         MM_MISC_REG_DVP_MUX_SH_444TO422
#define MM_MISC_REG_DVP_MUX_SH_444TO422_POS                     (7U)
#define MM_MISC_REG_DVP_MUX_SH_444TO422_LEN                     (1U)
#define MM_MISC_REG_DVP_MUX_SH_444TO422_MSK                     (((1U<<MM_MISC_REG_DVP_MUX_SH_444TO422_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_444TO422_POS)
#define MM_MISC_REG_DVP_MUX_SH_444TO422_UMSK                    (~(((1U<<MM_MISC_REG_DVP_MUX_SH_444TO422_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_444TO422_POS))
#define MM_MISC_REG_DVP_MUX_SH_D2XA                             MM_MISC_REG_DVP_MUX_SH_D2XA
#define MM_MISC_REG_DVP_MUX_SH_D2XA_POS                         (8U)
#define MM_MISC_REG_DVP_MUX_SH_D2XA_LEN                         (1U)
#define MM_MISC_REG_DVP_MUX_SH_D2XA_MSK                         (((1U<<MM_MISC_REG_DVP_MUX_SH_D2XA_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XA_POS)
#define MM_MISC_REG_DVP_MUX_SH_D2XA_UMSK                        (~(((1U<<MM_MISC_REG_DVP_MUX_SH_D2XA_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XA_POS))
#define MM_MISC_REG_DVP_MUX_SH_D2XB                             MM_MISC_REG_DVP_MUX_SH_D2XB
#define MM_MISC_REG_DVP_MUX_SH_D2XB_POS                         (9U)
#define MM_MISC_REG_DVP_MUX_SH_D2XB_LEN                         (1U)
#define MM_MISC_REG_DVP_MUX_SH_D2XB_MSK                         (((1U<<MM_MISC_REG_DVP_MUX_SH_D2XB_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XB_POS)
#define MM_MISC_REG_DVP_MUX_SH_D2XB_UMSK                        (~(((1U<<MM_MISC_REG_DVP_MUX_SH_D2XB_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XB_POS))
#define MM_MISC_REG_DVP_MUX_SH_D2XC                             MM_MISC_REG_DVP_MUX_SH_D2XC
#define MM_MISC_REG_DVP_MUX_SH_D2XC_POS                         (10U)
#define MM_MISC_REG_DVP_MUX_SH_D2XC_LEN                         (1U)
#define MM_MISC_REG_DVP_MUX_SH_D2XC_MSK                         (((1U<<MM_MISC_REG_DVP_MUX_SH_D2XC_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XC_POS)
#define MM_MISC_REG_DVP_MUX_SH_D2XC_UMSK                        (~(((1U<<MM_MISC_REG_DVP_MUX_SH_D2XC_LEN)-1)<<MM_MISC_REG_DVP_MUX_SH_D2XC_POS))

/* 0x8 : mm_misc_2 */
#define MM_MISC_2_OFFSET                                        (0x8)
#define MM_MISC_CR_DVP_AS_SRAM_MODE                             MM_MISC_CR_DVP_AS_SRAM_MODE
#define MM_MISC_CR_DVP_AS_SRAM_MODE_POS                         (0U)
#define MM_MISC_CR_DVP_AS_SRAM_MODE_LEN                         (2U)
#define MM_MISC_CR_DVP_AS_SRAM_MODE_MSK                         (((1U<<MM_MISC_CR_DVP_AS_SRAM_MODE_LEN)-1)<<MM_MISC_CR_DVP_AS_SRAM_MODE_POS)
#define MM_MISC_CR_DVP_AS_SRAM_MODE_UMSK                        (~(((1U<<MM_MISC_CR_DVP_AS_SRAM_MODE_LEN)-1)<<MM_MISC_CR_DVP_AS_SRAM_MODE_POS))
#define MM_MISC_CR_SPI2DVP_SEL                                  MM_MISC_CR_SPI2DVP_SEL
#define MM_MISC_CR_SPI2DVP_SEL_POS                              (4U)
#define MM_MISC_CR_SPI2DVP_SEL_LEN                              (1U)
#define MM_MISC_CR_SPI2DVP_SEL_MSK                              (((1U<<MM_MISC_CR_SPI2DVP_SEL_LEN)-1)<<MM_MISC_CR_SPI2DVP_SEL_POS)
#define MM_MISC_CR_SPI2DVP_SEL_UMSK                             (~(((1U<<MM_MISC_CR_SPI2DVP_SEL_LEN)-1)<<MM_MISC_CR_SPI2DVP_SEL_POS))

/* 0x10 : d2x_mux_reg0 */
#define MM_MISC_D2X_MUX_REG0_OFFSET                             (0x10)
#define MM_MISC_REG_D2XA_IN_SEL                                 MM_MISC_REG_D2XA_IN_SEL
#define MM_MISC_REG_D2XA_IN_SEL_POS                             (0U)
#define MM_MISC_REG_D2XA_IN_SEL_LEN                             (4U)
#define MM_MISC_REG_D2XA_IN_SEL_MSK                             (((1U<<MM_MISC_REG_D2XA_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XA_IN_SEL_POS)
#define MM_MISC_REG_D2XA_IN_SEL_UMSK                            (~(((1U<<MM_MISC_REG_D2XA_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XA_IN_SEL_POS))
#define MM_MISC_REG_D2XB_IN_SEL                                 MM_MISC_REG_D2XB_IN_SEL
#define MM_MISC_REG_D2XB_IN_SEL_POS                             (4U)
#define MM_MISC_REG_D2XB_IN_SEL_LEN                             (4U)
#define MM_MISC_REG_D2XB_IN_SEL_MSK                             (((1U<<MM_MISC_REG_D2XB_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XB_IN_SEL_POS)
#define MM_MISC_REG_D2XB_IN_SEL_UMSK                            (~(((1U<<MM_MISC_REG_D2XB_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XB_IN_SEL_POS))
#define MM_MISC_REG_D2XC_IN_SEL                                 MM_MISC_REG_D2XC_IN_SEL
#define MM_MISC_REG_D2XC_IN_SEL_POS                             (8U)
#define MM_MISC_REG_D2XC_IN_SEL_LEN                             (4U)
#define MM_MISC_REG_D2XC_IN_SEL_MSK                             (((1U<<MM_MISC_REG_D2XC_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XC_IN_SEL_POS)
#define MM_MISC_REG_D2XC_IN_SEL_UMSK                            (~(((1U<<MM_MISC_REG_D2XC_IN_SEL_LEN)-1)<<MM_MISC_REG_D2XC_IN_SEL_POS))
#define MM_MISC_CR_MJ_Y_SEL_D2X                                 MM_MISC_CR_MJ_Y_SEL_D2X
#define MM_MISC_CR_MJ_Y_SEL_D2X_POS                             (24U)
#define MM_MISC_CR_MJ_Y_SEL_D2X_LEN                             (2U)
#define MM_MISC_CR_MJ_Y_SEL_D2X_MSK                             (((1U<<MM_MISC_CR_MJ_Y_SEL_D2X_LEN)-1)<<MM_MISC_CR_MJ_Y_SEL_D2X_POS)
#define MM_MISC_CR_MJ_Y_SEL_D2X_UMSK                            (~(((1U<<MM_MISC_CR_MJ_Y_SEL_D2X_LEN)-1)<<MM_MISC_CR_MJ_Y_SEL_D2X_POS))
#define MM_MISC_CR_MJ_C_SEL_D2X                                 MM_MISC_CR_MJ_C_SEL_D2X
#define MM_MISC_CR_MJ_C_SEL_D2X_POS                             (28U)
#define MM_MISC_CR_MJ_C_SEL_D2X_LEN                             (2U)
#define MM_MISC_CR_MJ_C_SEL_D2X_MSK                             (((1U<<MM_MISC_CR_MJ_C_SEL_D2X_LEN)-1)<<MM_MISC_CR_MJ_C_SEL_D2X_POS)
#define MM_MISC_CR_MJ_C_SEL_D2X_UMSK                            (~(((1U<<MM_MISC_CR_MJ_C_SEL_D2X_LEN)-1)<<MM_MISC_CR_MJ_C_SEL_D2X_POS))

/* 0x14 : yuv422_mux_reg0 */
#define MM_MISC_YUV422_MUX_REG0_OFFSET                          (0x14)
#define MM_MISC_REG_ISP_IN_SEL                                  MM_MISC_REG_ISP_IN_SEL
#define MM_MISC_REG_ISP_IN_SEL_POS                              (0U)
#define MM_MISC_REG_ISP_IN_SEL_LEN                              (4U)
#define MM_MISC_REG_ISP_IN_SEL_MSK                              (((1U<<MM_MISC_REG_ISP_IN_SEL_LEN)-1)<<MM_MISC_REG_ISP_IN_SEL_POS)
#define MM_MISC_REG_ISP_IN_SEL_UMSK                             (~(((1U<<MM_MISC_REG_ISP_IN_SEL_LEN)-1)<<MM_MISC_REG_ISP_IN_SEL_POS))
#define MM_MISC_REG_SCHY_IN_SEL                                 MM_MISC_REG_SCHY_IN_SEL
#define MM_MISC_REG_SCHY_IN_SEL_POS                             (4U)
#define MM_MISC_REG_SCHY_IN_SEL_LEN                             (4U)
#define MM_MISC_REG_SCHY_IN_SEL_MSK                             (((1U<<MM_MISC_REG_SCHY_IN_SEL_LEN)-1)<<MM_MISC_REG_SCHY_IN_SEL_POS)
#define MM_MISC_REG_SCHY_IN_SEL_UMSK                            (~(((1U<<MM_MISC_REG_SCHY_IN_SEL_LEN)-1)<<MM_MISC_REG_SCHY_IN_SEL_POS))
#define MM_MISC_REG_OSD_BLEND_IN_SEL                            MM_MISC_REG_OSD_BLEND_IN_SEL
#define MM_MISC_REG_OSD_BLEND_IN_SEL_POS                        (8U)
#define MM_MISC_REG_OSD_BLEND_IN_SEL_LEN                        (4U)
#define MM_MISC_REG_OSD_BLEND_IN_SEL_MSK                        (((1U<<MM_MISC_REG_OSD_BLEND_IN_SEL_LEN)-1)<<MM_MISC_REG_OSD_BLEND_IN_SEL_POS)
#define MM_MISC_REG_OSD_BLEND_IN_SEL_UMSK                       (~(((1U<<MM_MISC_REG_OSD_BLEND_IN_SEL_LEN)-1)<<MM_MISC_REG_OSD_BLEND_IN_SEL_POS))
#define MM_MISC_REG_OSD_DRAW_IN_SEL                             MM_MISC_REG_OSD_DRAW_IN_SEL
#define MM_MISC_REG_OSD_DRAW_IN_SEL_POS                         (12U)
#define MM_MISC_REG_OSD_DRAW_IN_SEL_LEN                         (4U)
#define MM_MISC_REG_OSD_DRAW_IN_SEL_MSK                         (((1U<<MM_MISC_REG_OSD_DRAW_IN_SEL_LEN)-1)<<MM_MISC_REG_OSD_DRAW_IN_SEL_POS)
#define MM_MISC_REG_OSD_DRAW_IN_SEL_UMSK                        (~(((1U<<MM_MISC_REG_OSD_DRAW_IN_SEL_LEN)-1)<<MM_MISC_REG_OSD_DRAW_IN_SEL_POS))
#define MM_MISC_REG_422TO444_IN_SEL                             MM_MISC_REG_422TO444_IN_SEL
#define MM_MISC_REG_422TO444_IN_SEL_POS                         (16U)
#define MM_MISC_REG_422TO444_IN_SEL_LEN                         (4U)
#define MM_MISC_REG_422TO444_IN_SEL_MSK                         (((1U<<MM_MISC_REG_422TO444_IN_SEL_LEN)-1)<<MM_MISC_REG_422TO444_IN_SEL_POS)
#define MM_MISC_REG_422TO444_IN_SEL_UMSK                        (~(((1U<<MM_MISC_REG_422TO444_IN_SEL_LEN)-1)<<MM_MISC_REG_422TO444_IN_SEL_POS))
#define MM_MISC_REG_422TO420_IN_SEL                             MM_MISC_REG_422TO420_IN_SEL
#define MM_MISC_REG_422TO420_IN_SEL_POS                         (20U)
#define MM_MISC_REG_422TO420_IN_SEL_LEN                         (4U)
#define MM_MISC_REG_422TO420_IN_SEL_MSK                         (((1U<<MM_MISC_REG_422TO420_IN_SEL_LEN)-1)<<MM_MISC_REG_422TO420_IN_SEL_POS)
#define MM_MISC_REG_422TO420_IN_SEL_UMSK                        (~(((1U<<MM_MISC_REG_422TO420_IN_SEL_LEN)-1)<<MM_MISC_REG_422TO420_IN_SEL_POS))

/* 0x18 : yuv444_mux_reg0 */
#define MM_MISC_YUV444_MUX_REG0_OFFSET                          (0x18)
#define MM_MISC_REG_TRA_IN_SEL                                  MM_MISC_REG_TRA_IN_SEL
#define MM_MISC_REG_TRA_IN_SEL_POS                              (0U)
#define MM_MISC_REG_TRA_IN_SEL_LEN                              (2U)
#define MM_MISC_REG_TRA_IN_SEL_MSK                              (((1U<<MM_MISC_REG_TRA_IN_SEL_LEN)-1)<<MM_MISC_REG_TRA_IN_SEL_POS)
#define MM_MISC_REG_TRA_IN_SEL_UMSK                             (~(((1U<<MM_MISC_REG_TRA_IN_SEL_LEN)-1)<<MM_MISC_REG_TRA_IN_SEL_POS))
#define MM_MISC_REG_YUV444TO422_IN_SEL                          MM_MISC_REG_YUV444TO422_IN_SEL
#define MM_MISC_REG_YUV444TO422_IN_SEL_POS                      (3U)
#define MM_MISC_REG_YUV444TO422_IN_SEL_LEN                      (2U)
#define MM_MISC_REG_YUV444TO422_IN_SEL_MSK                      (((1U<<MM_MISC_REG_YUV444TO422_IN_SEL_LEN)-1)<<MM_MISC_REG_YUV444TO422_IN_SEL_POS)
#define MM_MISC_REG_YUV444TO422_IN_SEL_UMSK                     (~(((1U<<MM_MISC_REG_YUV444TO422_IN_SEL_LEN)-1)<<MM_MISC_REG_YUV444TO422_IN_SEL_POS))

/* 0x20 : cropA_reg_0 */
#define MM_MISC_CROPA_REG_0_OFFSET                              (0x20)
#define MM_MISC_REG_CROPA_VSYNC_START                           MM_MISC_REG_CROPA_VSYNC_START
#define MM_MISC_REG_CROPA_VSYNC_START_POS                       (0U)
#define MM_MISC_REG_CROPA_VSYNC_START_LEN                       (15U)
#define MM_MISC_REG_CROPA_VSYNC_START_MSK                       (((1U<<MM_MISC_REG_CROPA_VSYNC_START_LEN)-1)<<MM_MISC_REG_CROPA_VSYNC_START_POS)
#define MM_MISC_REG_CROPA_VSYNC_START_UMSK                      (~(((1U<<MM_MISC_REG_CROPA_VSYNC_START_LEN)-1)<<MM_MISC_REG_CROPA_VSYNC_START_POS))
#define MM_MISC_REG_CROPA_VSYNC_END                             MM_MISC_REG_CROPA_VSYNC_END
#define MM_MISC_REG_CROPA_VSYNC_END_POS                         (16U)
#define MM_MISC_REG_CROPA_VSYNC_END_LEN                         (15U)
#define MM_MISC_REG_CROPA_VSYNC_END_MSK                         (((1U<<MM_MISC_REG_CROPA_VSYNC_END_LEN)-1)<<MM_MISC_REG_CROPA_VSYNC_END_POS)
#define MM_MISC_REG_CROPA_VSYNC_END_UMSK                        (~(((1U<<MM_MISC_REG_CROPA_VSYNC_END_LEN)-1)<<MM_MISC_REG_CROPA_VSYNC_END_POS))
#define MM_MISC_REG_CROPA_ENABLE                                MM_MISC_REG_CROPA_ENABLE
#define MM_MISC_REG_CROPA_ENABLE_POS                            (31U)
#define MM_MISC_REG_CROPA_ENABLE_LEN                            (1U)
#define MM_MISC_REG_CROPA_ENABLE_MSK                            (((1U<<MM_MISC_REG_CROPA_ENABLE_LEN)-1)<<MM_MISC_REG_CROPA_ENABLE_POS)
#define MM_MISC_REG_CROPA_ENABLE_UMSK                           (~(((1U<<MM_MISC_REG_CROPA_ENABLE_LEN)-1)<<MM_MISC_REG_CROPA_ENABLE_POS))

/* 0x24 : cropA_reg_1 */
#define MM_MISC_CROPA_REG_1_OFFSET                              (0x24)
#define MM_MISC_REG_CROPA_HSYNC_START                           MM_MISC_REG_CROPA_HSYNC_START
#define MM_MISC_REG_CROPA_HSYNC_START_POS                       (0U)
#define MM_MISC_REG_CROPA_HSYNC_START_LEN                       (15U)
#define MM_MISC_REG_CROPA_HSYNC_START_MSK                       (((1U<<MM_MISC_REG_CROPA_HSYNC_START_LEN)-1)<<MM_MISC_REG_CROPA_HSYNC_START_POS)
#define MM_MISC_REG_CROPA_HSYNC_START_UMSK                      (~(((1U<<MM_MISC_REG_CROPA_HSYNC_START_LEN)-1)<<MM_MISC_REG_CROPA_HSYNC_START_POS))
#define MM_MISC_REG_CROPA_HSYNC_END                             MM_MISC_REG_CROPA_HSYNC_END
#define MM_MISC_REG_CROPA_HSYNC_END_POS                         (16U)
#define MM_MISC_REG_CROPA_HSYNC_END_LEN                         (15U)
#define MM_MISC_REG_CROPA_HSYNC_END_MSK                         (((1U<<MM_MISC_REG_CROPA_HSYNC_END_LEN)-1)<<MM_MISC_REG_CROPA_HSYNC_END_POS)
#define MM_MISC_REG_CROPA_HSYNC_END_UMSK                        (~(((1U<<MM_MISC_REG_CROPA_HSYNC_END_LEN)-1)<<MM_MISC_REG_CROPA_HSYNC_END_POS))

/* 0x40 : y2rA_reg_0 */
#define MM_MISC_Y2RA_REG_0_OFFSET                               (0x40)
#define MM_MISC_RG_Y2RA_PRE_0                                   MM_MISC_RG_Y2RA_PRE_0
#define MM_MISC_RG_Y2RA_PRE_0_POS                               (0U)
#define MM_MISC_RG_Y2RA_PRE_0_LEN                               (9U)
#define MM_MISC_RG_Y2RA_PRE_0_MSK                               (((1U<<MM_MISC_RG_Y2RA_PRE_0_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_0_POS)
#define MM_MISC_RG_Y2RA_PRE_0_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_PRE_0_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_0_POS))
#define MM_MISC_RG_Y2RA_POS_0                                   MM_MISC_RG_Y2RA_POS_0
#define MM_MISC_RG_Y2RA_POS_0_POS                               (16U)
#define MM_MISC_RG_Y2RA_POS_0_LEN                               (9U)
#define MM_MISC_RG_Y2RA_POS_0_MSK                               (((1U<<MM_MISC_RG_Y2RA_POS_0_LEN)-1)<<MM_MISC_RG_Y2RA_POS_0_POS)
#define MM_MISC_RG_Y2RA_POS_0_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_POS_0_LEN)-1)<<MM_MISC_RG_Y2RA_POS_0_POS))
#define MM_MISC_RG_Y2RA_EN                                      MM_MISC_RG_Y2RA_EN
#define MM_MISC_RG_Y2RA_EN_POS                                  (27U)
#define MM_MISC_RG_Y2RA_EN_LEN                                  (1U)
#define MM_MISC_RG_Y2RA_EN_MSK                                  (((1U<<MM_MISC_RG_Y2RA_EN_LEN)-1)<<MM_MISC_RG_Y2RA_EN_POS)
#define MM_MISC_RG_Y2RA_EN_UMSK                                 (~(((1U<<MM_MISC_RG_Y2RA_EN_LEN)-1)<<MM_MISC_RG_Y2RA_EN_POS))

/* 0x44 : y2rA_reg_1 */
#define MM_MISC_Y2RA_REG_1_OFFSET                               (0x44)
#define MM_MISC_RG_Y2RA_PRE_1                                   MM_MISC_RG_Y2RA_PRE_1
#define MM_MISC_RG_Y2RA_PRE_1_POS                               (0U)
#define MM_MISC_RG_Y2RA_PRE_1_LEN                               (9U)
#define MM_MISC_RG_Y2RA_PRE_1_MSK                               (((1U<<MM_MISC_RG_Y2RA_PRE_1_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_1_POS)
#define MM_MISC_RG_Y2RA_PRE_1_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_PRE_1_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_1_POS))
#define MM_MISC_RG_Y2RA_POS_1                                   MM_MISC_RG_Y2RA_POS_1
#define MM_MISC_RG_Y2RA_POS_1_POS                               (16U)
#define MM_MISC_RG_Y2RA_POS_1_LEN                               (9U)
#define MM_MISC_RG_Y2RA_POS_1_MSK                               (((1U<<MM_MISC_RG_Y2RA_POS_1_LEN)-1)<<MM_MISC_RG_Y2RA_POS_1_POS)
#define MM_MISC_RG_Y2RA_POS_1_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_POS_1_LEN)-1)<<MM_MISC_RG_Y2RA_POS_1_POS))

/* 0x48 : y2rA_reg_2 */
#define MM_MISC_Y2RA_REG_2_OFFSET                               (0x48)
#define MM_MISC_RG_Y2RA_PRE_2                                   MM_MISC_RG_Y2RA_PRE_2
#define MM_MISC_RG_Y2RA_PRE_2_POS                               (0U)
#define MM_MISC_RG_Y2RA_PRE_2_LEN                               (9U)
#define MM_MISC_RG_Y2RA_PRE_2_MSK                               (((1U<<MM_MISC_RG_Y2RA_PRE_2_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_2_POS)
#define MM_MISC_RG_Y2RA_PRE_2_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_PRE_2_LEN)-1)<<MM_MISC_RG_Y2RA_PRE_2_POS))
#define MM_MISC_RG_Y2RA_POS_2                                   MM_MISC_RG_Y2RA_POS_2
#define MM_MISC_RG_Y2RA_POS_2_POS                               (16U)
#define MM_MISC_RG_Y2RA_POS_2_LEN                               (9U)
#define MM_MISC_RG_Y2RA_POS_2_MSK                               (((1U<<MM_MISC_RG_Y2RA_POS_2_LEN)-1)<<MM_MISC_RG_Y2RA_POS_2_POS)
#define MM_MISC_RG_Y2RA_POS_2_UMSK                              (~(((1U<<MM_MISC_RG_Y2RA_POS_2_LEN)-1)<<MM_MISC_RG_Y2RA_POS_2_POS))

/* 0x4C : y2rA_reg_3 */
#define MM_MISC_Y2RA_REG_3_OFFSET                               (0x4C)
#define MM_MISC_RG_Y2RA_MTX_00                                  MM_MISC_RG_Y2RA_MTX_00
#define MM_MISC_RG_Y2RA_MTX_00_POS                              (0U)
#define MM_MISC_RG_Y2RA_MTX_00_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_00_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_00_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_00_POS)
#define MM_MISC_RG_Y2RA_MTX_00_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_00_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_00_POS))
#define MM_MISC_RG_Y2RA_MTX_01                                  MM_MISC_RG_Y2RA_MTX_01
#define MM_MISC_RG_Y2RA_MTX_01_POS                              (16U)
#define MM_MISC_RG_Y2RA_MTX_01_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_01_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_01_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_01_POS)
#define MM_MISC_RG_Y2RA_MTX_01_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_01_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_01_POS))

/* 0x50 : y2rA_reg_4 */
#define MM_MISC_Y2RA_REG_4_OFFSET                               (0x50)
#define MM_MISC_RG_Y2RA_MTX_02                                  MM_MISC_RG_Y2RA_MTX_02
#define MM_MISC_RG_Y2RA_MTX_02_POS                              (0U)
#define MM_MISC_RG_Y2RA_MTX_02_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_02_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_02_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_02_POS)
#define MM_MISC_RG_Y2RA_MTX_02_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_02_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_02_POS))
#define MM_MISC_RG_Y2RA_MTX_10                                  MM_MISC_RG_Y2RA_MTX_10
#define MM_MISC_RG_Y2RA_MTX_10_POS                              (16U)
#define MM_MISC_RG_Y2RA_MTX_10_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_10_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_10_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_10_POS)
#define MM_MISC_RG_Y2RA_MTX_10_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_10_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_10_POS))

/* 0x54 : y2rA_reg_5 */
#define MM_MISC_Y2RA_REG_5_OFFSET                               (0x54)
#define MM_MISC_RG_Y2RA_MTX_11                                  MM_MISC_RG_Y2RA_MTX_11
#define MM_MISC_RG_Y2RA_MTX_11_POS                              (0U)
#define MM_MISC_RG_Y2RA_MTX_11_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_11_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_11_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_11_POS)
#define MM_MISC_RG_Y2RA_MTX_11_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_11_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_11_POS))
#define MM_MISC_RG_Y2RA_MTX_12                                  MM_MISC_RG_Y2RA_MTX_12
#define MM_MISC_RG_Y2RA_MTX_12_POS                              (16U)
#define MM_MISC_RG_Y2RA_MTX_12_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_12_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_12_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_12_POS)
#define MM_MISC_RG_Y2RA_MTX_12_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_12_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_12_POS))

/* 0x58 : y2rA_reg_6 */
#define MM_MISC_Y2RA_REG_6_OFFSET                               (0x58)
#define MM_MISC_RG_Y2RA_MTX_20                                  MM_MISC_RG_Y2RA_MTX_20
#define MM_MISC_RG_Y2RA_MTX_20_POS                              (0U)
#define MM_MISC_RG_Y2RA_MTX_20_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_20_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_20_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_20_POS)
#define MM_MISC_RG_Y2RA_MTX_20_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_20_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_20_POS))
#define MM_MISC_RG_Y2RA_MTX_21                                  MM_MISC_RG_Y2RA_MTX_21
#define MM_MISC_RG_Y2RA_MTX_21_POS                              (16U)
#define MM_MISC_RG_Y2RA_MTX_21_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_21_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_21_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_21_POS)
#define MM_MISC_RG_Y2RA_MTX_21_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_21_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_21_POS))

/* 0x5C : y2rA_reg_7 */
#define MM_MISC_Y2RA_REG_7_OFFSET                               (0x5C)
#define MM_MISC_RG_Y2RA_MTX_22                                  MM_MISC_RG_Y2RA_MTX_22
#define MM_MISC_RG_Y2RA_MTX_22_POS                              (0U)
#define MM_MISC_RG_Y2RA_MTX_22_LEN                              (12U)
#define MM_MISC_RG_Y2RA_MTX_22_MSK                              (((1U<<MM_MISC_RG_Y2RA_MTX_22_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_22_POS)
#define MM_MISC_RG_Y2RA_MTX_22_UMSK                             (~(((1U<<MM_MISC_RG_Y2RA_MTX_22_LEN)-1)<<MM_MISC_RG_Y2RA_MTX_22_POS))

/* 0x60 : spi2dvp_ctrl */
#define MM_MISC_SPI2DVP_CTRL_OFFSET                             (0x60)
#define MM_MISC_RG_SPI2DVP_ENABLE                               MM_MISC_RG_SPI2DVP_ENABLE
#define MM_MISC_RG_SPI2DVP_ENABLE_POS                           (0U)
#define MM_MISC_RG_SPI2DVP_ENABLE_LEN                           (1U)
#define MM_MISC_RG_SPI2DVP_ENABLE_MSK                           (((1U<<MM_MISC_RG_SPI2DVP_ENABLE_LEN)-1)<<MM_MISC_RG_SPI2DVP_ENABLE_POS)
#define MM_MISC_RG_SPI2DVP_ENABLE_UMSK                          (~(((1U<<MM_MISC_RG_SPI2DVP_ENABLE_LEN)-1)<<MM_MISC_RG_SPI2DVP_ENABLE_POS))
#define MM_MISC_RG_SPI2DVP_ENDIAN                               MM_MISC_RG_SPI2DVP_ENDIAN
#define MM_MISC_RG_SPI2DVP_ENDIAN_POS                           (1U)
#define MM_MISC_RG_SPI2DVP_ENDIAN_LEN                           (1U)
#define MM_MISC_RG_SPI2DVP_ENDIAN_MSK                           (((1U<<MM_MISC_RG_SPI2DVP_ENDIAN_LEN)-1)<<MM_MISC_RG_SPI2DVP_ENDIAN_POS)
#define MM_MISC_RG_SPI2DVP_ENDIAN_UMSK                          (~(((1U<<MM_MISC_RG_SPI2DVP_ENDIAN_LEN)-1)<<MM_MISC_RG_SPI2DVP_ENDIAN_POS))
#define MM_MISC_RG_SPI2DVP_SCLK_POL                             MM_MISC_RG_SPI2DVP_SCLK_POL
#define MM_MISC_RG_SPI2DVP_SCLK_POL_POS                         (2U)
#define MM_MISC_RG_SPI2DVP_SCLK_POL_LEN                         (1U)
#define MM_MISC_RG_SPI2DVP_SCLK_POL_MSK                         (((1U<<MM_MISC_RG_SPI2DVP_SCLK_POL_LEN)-1)<<MM_MISC_RG_SPI2DVP_SCLK_POL_POS)
#define MM_MISC_RG_SPI2DVP_SCLK_POL_UMSK                        (~(((1U<<MM_MISC_RG_SPI2DVP_SCLK_POL_LEN)-1)<<MM_MISC_RG_SPI2DVP_SCLK_POL_POS))
#define MM_MISC_RG_SPI2DVP_SCLK_PH                              MM_MISC_RG_SPI2DVP_SCLK_PH
#define MM_MISC_RG_SPI2DVP_SCLK_PH_POS                          (3U)
#define MM_MISC_RG_SPI2DVP_SCLK_PH_LEN                          (1U)
#define MM_MISC_RG_SPI2DVP_SCLK_PH_MSK                          (((1U<<MM_MISC_RG_SPI2DVP_SCLK_PH_LEN)-1)<<MM_MISC_RG_SPI2DVP_SCLK_PH_POS)
#define MM_MISC_RG_SPI2DVP_SCLK_PH_UMSK                         (~(((1U<<MM_MISC_RG_SPI2DVP_SCLK_PH_LEN)-1)<<MM_MISC_RG_SPI2DVP_SCLK_PH_POS))
#define MM_MISC_RG_SPI2DVP_DATA_FORMAT                          MM_MISC_RG_SPI2DVP_DATA_FORMAT
#define MM_MISC_RG_SPI2DVP_DATA_FORMAT_POS                      (4U)
#define MM_MISC_RG_SPI2DVP_DATA_FORMAT_LEN                      (1U)
#define MM_MISC_RG_SPI2DVP_DATA_FORMAT_MSK                      (((1U<<MM_MISC_RG_SPI2DVP_DATA_FORMAT_LEN)-1)<<MM_MISC_RG_SPI2DVP_DATA_FORMAT_POS)
#define MM_MISC_RG_SPI2DVP_DATA_FORMAT_UMSK                     (~(((1U<<MM_MISC_RG_SPI2DVP_DATA_FORMAT_LEN)-1)<<MM_MISC_RG_SPI2DVP_DATA_FORMAT_POS))
#define MM_MISC_RG_SPI2DVP_DATA_ORDER                           MM_MISC_RG_SPI2DVP_DATA_ORDER
#define MM_MISC_RG_SPI2DVP_DATA_ORDER_POS                       (5U)
#define MM_MISC_RG_SPI2DVP_DATA_ORDER_LEN                       (1U)
#define MM_MISC_RG_SPI2DVP_DATA_ORDER_MSK                       (((1U<<MM_MISC_RG_SPI2DVP_DATA_ORDER_LEN)-1)<<MM_MISC_RG_SPI2DVP_DATA_ORDER_POS)
#define MM_MISC_RG_SPI2DVP_DATA_ORDER_UMSK                      (~(((1U<<MM_MISC_RG_SPI2DVP_DATA_ORDER_LEN)-1)<<MM_MISC_RG_SPI2DVP_DATA_ORDER_POS))
#define MM_MISC_RG_SPI2DVP_INT_EN                               MM_MISC_RG_SPI2DVP_INT_EN
#define MM_MISC_RG_SPI2DVP_INT_EN_POS                           (16U)
#define MM_MISC_RG_SPI2DVP_INT_EN_LEN                           (1U)
#define MM_MISC_RG_SPI2DVP_INT_EN_MSK                           (((1U<<MM_MISC_RG_SPI2DVP_INT_EN_LEN)-1)<<MM_MISC_RG_SPI2DVP_INT_EN_POS)
#define MM_MISC_RG_SPI2DVP_INT_EN_UMSK                          (~(((1U<<MM_MISC_RG_SPI2DVP_INT_EN_LEN)-1)<<MM_MISC_RG_SPI2DVP_INT_EN_POS))
#define MM_MISC_SPI2DVP_SYNC_INT                                MM_MISC_SPI2DVP_SYNC_INT
#define MM_MISC_SPI2DVP_SYNC_INT_POS                            (17U)
#define MM_MISC_SPI2DVP_SYNC_INT_LEN                            (1U)
#define MM_MISC_SPI2DVP_SYNC_INT_MSK                            (((1U<<MM_MISC_SPI2DVP_SYNC_INT_LEN)-1)<<MM_MISC_SPI2DVP_SYNC_INT_POS)
#define MM_MISC_SPI2DVP_SYNC_INT_UMSK                           (~(((1U<<MM_MISC_SPI2DVP_SYNC_INT_LEN)-1)<<MM_MISC_SPI2DVP_SYNC_INT_POS))
#define MM_MISC_SPI2DVP_FS_INT                                  MM_MISC_SPI2DVP_FS_INT
#define MM_MISC_SPI2DVP_FS_INT_POS                              (18U)
#define MM_MISC_SPI2DVP_FS_INT_LEN                              (1U)
#define MM_MISC_SPI2DVP_FS_INT_MSK                              (((1U<<MM_MISC_SPI2DVP_FS_INT_LEN)-1)<<MM_MISC_SPI2DVP_FS_INT_POS)
#define MM_MISC_SPI2DVP_FS_INT_UMSK                             (~(((1U<<MM_MISC_SPI2DVP_FS_INT_LEN)-1)<<MM_MISC_SPI2DVP_FS_INT_POS))
#define MM_MISC_SPI2DVP_FE_INT                                  MM_MISC_SPI2DVP_FE_INT
#define MM_MISC_SPI2DVP_FE_INT_POS                              (19U)
#define MM_MISC_SPI2DVP_FE_INT_LEN                              (1U)
#define MM_MISC_SPI2DVP_FE_INT_MSK                              (((1U<<MM_MISC_SPI2DVP_FE_INT_LEN)-1)<<MM_MISC_SPI2DVP_FE_INT_POS)
#define MM_MISC_SPI2DVP_FE_INT_UMSK                             (~(((1U<<MM_MISC_SPI2DVP_FE_INT_LEN)-1)<<MM_MISC_SPI2DVP_FE_INT_POS))
#define MM_MISC_SPI2DVP_LS_INT                                  MM_MISC_SPI2DVP_LS_INT
#define MM_MISC_SPI2DVP_LS_INT_POS                              (20U)
#define MM_MISC_SPI2DVP_LS_INT_LEN                              (1U)
#define MM_MISC_SPI2DVP_LS_INT_MSK                              (((1U<<MM_MISC_SPI2DVP_LS_INT_LEN)-1)<<MM_MISC_SPI2DVP_LS_INT_POS)
#define MM_MISC_SPI2DVP_LS_INT_UMSK                             (~(((1U<<MM_MISC_SPI2DVP_LS_INT_LEN)-1)<<MM_MISC_SPI2DVP_LS_INT_POS))
#define MM_MISC_SPI2DVP_FIFO_INT                                MM_MISC_SPI2DVP_FIFO_INT
#define MM_MISC_SPI2DVP_FIFO_INT_POS                            (21U)
#define MM_MISC_SPI2DVP_FIFO_INT_LEN                            (1U)
#define MM_MISC_SPI2DVP_FIFO_INT_MSK                            (((1U<<MM_MISC_SPI2DVP_FIFO_INT_LEN)-1)<<MM_MISC_SPI2DVP_FIFO_INT_POS)
#define MM_MISC_SPI2DVP_FIFO_INT_UMSK                           (~(((1U<<MM_MISC_SPI2DVP_FIFO_INT_LEN)-1)<<MM_MISC_SPI2DVP_FIFO_INT_POS))
#define MM_MISC_ST_SPI2DVP_BUS_BUSY                             MM_MISC_ST_SPI2DVP_BUS_BUSY
#define MM_MISC_ST_SPI2DVP_BUS_BUSY_POS                         (31U)
#define MM_MISC_ST_SPI2DVP_BUS_BUSY_LEN                         (1U)
#define MM_MISC_ST_SPI2DVP_BUS_BUSY_MSK                         (((1U<<MM_MISC_ST_SPI2DVP_BUS_BUSY_LEN)-1)<<MM_MISC_ST_SPI2DVP_BUS_BUSY_POS)
#define MM_MISC_ST_SPI2DVP_BUS_BUSY_UMSK                        (~(((1U<<MM_MISC_ST_SPI2DVP_BUS_BUSY_LEN)-1)<<MM_MISC_ST_SPI2DVP_BUS_BUSY_POS))

/* 0x64 : spi2dvp_status_0 */
#define MM_MISC_SPI2DVP_STATUS_0_OFFSET                         (0x64)
#define MM_MISC_ST_SPI2DVP_PKT_ID                               MM_MISC_ST_SPI2DVP_PKT_ID
#define MM_MISC_ST_SPI2DVP_PKT_ID_POS                           (0U)
#define MM_MISC_ST_SPI2DVP_PKT_ID_LEN                           (8U)
#define MM_MISC_ST_SPI2DVP_PKT_ID_MSK                           (((1U<<MM_MISC_ST_SPI2DVP_PKT_ID_LEN)-1)<<MM_MISC_ST_SPI2DVP_PKT_ID_POS)
#define MM_MISC_ST_SPI2DVP_PKT_ID_UMSK                          (~(((1U<<MM_MISC_ST_SPI2DVP_PKT_ID_LEN)-1)<<MM_MISC_ST_SPI2DVP_PKT_ID_POS))
#define MM_MISC_ST_SPI2DVP_DATA_ID                              MM_MISC_ST_SPI2DVP_DATA_ID
#define MM_MISC_ST_SPI2DVP_DATA_ID_POS                          (8U)
#define MM_MISC_ST_SPI2DVP_DATA_ID_LEN                          (8U)
#define MM_MISC_ST_SPI2DVP_DATA_ID_MSK                          (((1U<<MM_MISC_ST_SPI2DVP_DATA_ID_LEN)-1)<<MM_MISC_ST_SPI2DVP_DATA_ID_POS)
#define MM_MISC_ST_SPI2DVP_DATA_ID_UMSK                         (~(((1U<<MM_MISC_ST_SPI2DVP_DATA_ID_LEN)-1)<<MM_MISC_ST_SPI2DVP_DATA_ID_POS))
#define MM_MISC_ST_SPI2DVP_DATA_SIZE                            MM_MISC_ST_SPI2DVP_DATA_SIZE
#define MM_MISC_ST_SPI2DVP_DATA_SIZE_POS                        (16U)
#define MM_MISC_ST_SPI2DVP_DATA_SIZE_LEN                        (16U)
#define MM_MISC_ST_SPI2DVP_DATA_SIZE_MSK                        (((1U<<MM_MISC_ST_SPI2DVP_DATA_SIZE_LEN)-1)<<MM_MISC_ST_SPI2DVP_DATA_SIZE_POS)
#define MM_MISC_ST_SPI2DVP_DATA_SIZE_UMSK                       (~(((1U<<MM_MISC_ST_SPI2DVP_DATA_SIZE_LEN)-1)<<MM_MISC_ST_SPI2DVP_DATA_SIZE_POS))

/* 0x68 : spi2dvp_status_1 */
#define MM_MISC_SPI2DVP_STATUS_1_OFFSET                         (0x68)
#define MM_MISC_ST_SPI2DVP_IMG_W                                MM_MISC_ST_SPI2DVP_IMG_W
#define MM_MISC_ST_SPI2DVP_IMG_W_POS                            (0U)
#define MM_MISC_ST_SPI2DVP_IMG_W_LEN                            (16U)
#define MM_MISC_ST_SPI2DVP_IMG_W_MSK                            (((1U<<MM_MISC_ST_SPI2DVP_IMG_W_LEN)-1)<<MM_MISC_ST_SPI2DVP_IMG_W_POS)
#define MM_MISC_ST_SPI2DVP_IMG_W_UMSK                           (~(((1U<<MM_MISC_ST_SPI2DVP_IMG_W_LEN)-1)<<MM_MISC_ST_SPI2DVP_IMG_W_POS))
#define MM_MISC_ST_SPI2DVP_IMG_H                                MM_MISC_ST_SPI2DVP_IMG_H
#define MM_MISC_ST_SPI2DVP_IMG_H_POS                            (16U)
#define MM_MISC_ST_SPI2DVP_IMG_H_LEN                            (16U)
#define MM_MISC_ST_SPI2DVP_IMG_H_MSK                            (((1U<<MM_MISC_ST_SPI2DVP_IMG_H_LEN)-1)<<MM_MISC_ST_SPI2DVP_IMG_H_POS)
#define MM_MISC_ST_SPI2DVP_IMG_H_UMSK                           (~(((1U<<MM_MISC_ST_SPI2DVP_IMG_H_LEN)-1)<<MM_MISC_ST_SPI2DVP_IMG_H_POS))

/* 0x70 : dvp_as_reg0 */
#define MM_MISC_DVP_AS_REG0_OFFSET                              (0x70)
#define MM_MISC_CR_DVP_AS_EN                                    MM_MISC_CR_DVP_AS_EN
#define MM_MISC_CR_DVP_AS_EN_POS                                (0U)
#define MM_MISC_CR_DVP_AS_EN_LEN                                (1U)
#define MM_MISC_CR_DVP_AS_EN_MSK                                (((1U<<MM_MISC_CR_DVP_AS_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_EN_POS)
#define MM_MISC_CR_DVP_AS_EN_UMSK                               (~(((1U<<MM_MISC_CR_DVP_AS_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_EN_POS))
#define MM_MISC_CR_DVP_AS_HS_INV                                MM_MISC_CR_DVP_AS_HS_INV
#define MM_MISC_CR_DVP_AS_HS_INV_POS                            (1U)
#define MM_MISC_CR_DVP_AS_HS_INV_LEN                            (1U)
#define MM_MISC_CR_DVP_AS_HS_INV_MSK                            (((1U<<MM_MISC_CR_DVP_AS_HS_INV_LEN)-1)<<MM_MISC_CR_DVP_AS_HS_INV_POS)
#define MM_MISC_CR_DVP_AS_HS_INV_UMSK                           (~(((1U<<MM_MISC_CR_DVP_AS_HS_INV_LEN)-1)<<MM_MISC_CR_DVP_AS_HS_INV_POS))
#define MM_MISC_CR_DVP_AS_VS_INV                                MM_MISC_CR_DVP_AS_VS_INV
#define MM_MISC_CR_DVP_AS_VS_INV_POS                            (2U)
#define MM_MISC_CR_DVP_AS_VS_INV_LEN                            (1U)
#define MM_MISC_CR_DVP_AS_VS_INV_MSK                            (((1U<<MM_MISC_CR_DVP_AS_VS_INV_LEN)-1)<<MM_MISC_CR_DVP_AS_VS_INV_POS)
#define MM_MISC_CR_DVP_AS_VS_INV_UMSK                           (~(((1U<<MM_MISC_CR_DVP_AS_VS_INV_LEN)-1)<<MM_MISC_CR_DVP_AS_VS_INV_POS))
#define MM_MISC_CR_DVP_AS_TG_EN                                 MM_MISC_CR_DVP_AS_TG_EN
#define MM_MISC_CR_DVP_AS_TG_EN_POS                             (3U)
#define MM_MISC_CR_DVP_AS_TG_EN_LEN                             (1U)
#define MM_MISC_CR_DVP_AS_TG_EN_MSK                             (((1U<<MM_MISC_CR_DVP_AS_TG_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_TG_EN_POS)
#define MM_MISC_CR_DVP_AS_TG_EN_UMSK                            (~(((1U<<MM_MISC_CR_DVP_AS_TG_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_TG_EN_POS))
#define MM_MISC_CR_DVP_AS_FIFO_TH                               MM_MISC_CR_DVP_AS_FIFO_TH
#define MM_MISC_CR_DVP_AS_FIFO_TH_POS                           (4U)
#define MM_MISC_CR_DVP_AS_FIFO_TH_LEN                           (12U)
#define MM_MISC_CR_DVP_AS_FIFO_TH_MSK                           (((1U<<MM_MISC_CR_DVP_AS_FIFO_TH_LEN)-1)<<MM_MISC_CR_DVP_AS_FIFO_TH_POS)
#define MM_MISC_CR_DVP_AS_FIFO_TH_UMSK                          (~(((1U<<MM_MISC_CR_DVP_AS_FIFO_TH_LEN)-1)<<MM_MISC_CR_DVP_AS_FIFO_TH_POS))
#define MM_MISC_CR_DVP_S2P_EN                                   MM_MISC_CR_DVP_S2P_EN
#define MM_MISC_CR_DVP_S2P_EN_POS                               (16U)
#define MM_MISC_CR_DVP_S2P_EN_LEN                               (1U)
#define MM_MISC_CR_DVP_S2P_EN_MSK                               (((1U<<MM_MISC_CR_DVP_S2P_EN_LEN)-1)<<MM_MISC_CR_DVP_S2P_EN_POS)
#define MM_MISC_CR_DVP_S2P_EN_UMSK                              (~(((1U<<MM_MISC_CR_DVP_S2P_EN_LEN)-1)<<MM_MISC_CR_DVP_S2P_EN_POS))
#define MM_MISC_CR_DVP_S2P_VS_INV                               MM_MISC_CR_DVP_S2P_VS_INV
#define MM_MISC_CR_DVP_S2P_VS_INV_POS                           (17U)
#define MM_MISC_CR_DVP_S2P_VS_INV_LEN                           (1U)
#define MM_MISC_CR_DVP_S2P_VS_INV_MSK                           (((1U<<MM_MISC_CR_DVP_S2P_VS_INV_LEN)-1)<<MM_MISC_CR_DVP_S2P_VS_INV_POS)
#define MM_MISC_CR_DVP_S2P_VS_INV_UMSK                          (~(((1U<<MM_MISC_CR_DVP_S2P_VS_INV_LEN)-1)<<MM_MISC_CR_DVP_S2P_VS_INV_POS))
#define MM_MISC_CR_DVP_S2P_HS_INV                               MM_MISC_CR_DVP_S2P_HS_INV
#define MM_MISC_CR_DVP_S2P_HS_INV_POS                           (18U)
#define MM_MISC_CR_DVP_S2P_HS_INV_LEN                           (1U)
#define MM_MISC_CR_DVP_S2P_HS_INV_MSK                           (((1U<<MM_MISC_CR_DVP_S2P_HS_INV_LEN)-1)<<MM_MISC_CR_DVP_S2P_HS_INV_POS)
#define MM_MISC_CR_DVP_S2P_HS_INV_UMSK                          (~(((1U<<MM_MISC_CR_DVP_S2P_HS_INV_LEN)-1)<<MM_MISC_CR_DVP_S2P_HS_INV_POS))
#define MM_MISC_CR_DVP_S2P_DA_ORDER                             MM_MISC_CR_DVP_S2P_DA_ORDER
#define MM_MISC_CR_DVP_S2P_DA_ORDER_POS                         (19U)
#define MM_MISC_CR_DVP_S2P_DA_ORDER_LEN                         (1U)
#define MM_MISC_CR_DVP_S2P_DA_ORDER_MSK                         (((1U<<MM_MISC_CR_DVP_S2P_DA_ORDER_LEN)-1)<<MM_MISC_CR_DVP_S2P_DA_ORDER_POS)
#define MM_MISC_CR_DVP_S2P_DA_ORDER_UMSK                        (~(((1U<<MM_MISC_CR_DVP_S2P_DA_ORDER_LEN)-1)<<MM_MISC_CR_DVP_S2P_DA_ORDER_POS))

/* 0x74 : dvp_as_reg1 */
#define MM_MISC_DVP_AS_REG1_OFFSET                              (0x74)
#define MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH                       MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH
#define MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_POS                   (0U)
#define MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_LEN                   (16U)
#define MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_MSK                   (((1U<<MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_POS)
#define MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_UMSK                  (~(((1U<<MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH_POS))
#define MM_MISC_CR_DVP_AS_ACT_START_PIXEL                       MM_MISC_CR_DVP_AS_ACT_START_PIXEL
#define MM_MISC_CR_DVP_AS_ACT_START_PIXEL_POS                   (16U)
#define MM_MISC_CR_DVP_AS_ACT_START_PIXEL_LEN                   (16U)
#define MM_MISC_CR_DVP_AS_ACT_START_PIXEL_MSK                   (((1U<<MM_MISC_CR_DVP_AS_ACT_START_PIXEL_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_START_PIXEL_POS)
#define MM_MISC_CR_DVP_AS_ACT_START_PIXEL_UMSK                  (~(((1U<<MM_MISC_CR_DVP_AS_ACT_START_PIXEL_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_START_PIXEL_POS))

/* 0x78 : dvp_as_reg2 */
#define MM_MISC_DVP_AS_REG2_OFFSET                              (0x78)
#define MM_MISC_CR_DVP_AS_TOTAL_HPIXELS                         MM_MISC_CR_DVP_AS_TOTAL_HPIXELS
#define MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_POS                     (0U)
#define MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_LEN                     (16U)
#define MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_MSK                     (((1U<<MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_LEN)-1)<<MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_POS)
#define MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_UMSK                    (~(((1U<<MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_LEN)-1)<<MM_MISC_CR_DVP_AS_TOTAL_HPIXELS_POS))
#define MM_MISC_CR_DVP_AS_ACT_START_LINE                        MM_MISC_CR_DVP_AS_ACT_START_LINE
#define MM_MISC_CR_DVP_AS_ACT_START_LINE_POS                    (16U)
#define MM_MISC_CR_DVP_AS_ACT_START_LINE_LEN                    (16U)
#define MM_MISC_CR_DVP_AS_ACT_START_LINE_MSK                    (((1U<<MM_MISC_CR_DVP_AS_ACT_START_LINE_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_START_LINE_POS)
#define MM_MISC_CR_DVP_AS_ACT_START_LINE_UMSK                   (~(((1U<<MM_MISC_CR_DVP_AS_ACT_START_LINE_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_START_LINE_POS))

/* 0x7C : dvp_as_reg3 */
#define MM_MISC_DVP_AS_REG3_OFFSET                              (0x7C)
#define MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT                       MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT
#define MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_POS                   (0U)
#define MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_LEN                   (16U)
#define MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_MSK                   (((1U<<MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_POS)
#define MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_UMSK                  (~(((1U<<MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_LEN)-1)<<MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT_POS))

/* 0x80 : iproai_lmtr_rd */
#define MM_MISC_IPROAI_LMTR_RD_OFFSET                             (0x80)
#define MM_MISC_REG_IPROAI_RCMD_CNT                               MM_MISC_REG_IPROAI_RCMD_CNT
#define MM_MISC_REG_IPROAI_RCMD_CNT_POS                           (0U)
#define MM_MISC_REG_IPROAI_RCMD_CNT_LEN                           (16U)
#define MM_MISC_REG_IPROAI_RCMD_CNT_MSK                           (((1U<<MM_MISC_REG_IPROAI_RCMD_CNT_LEN)-1)<<MM_MISC_REG_IPROAI_RCMD_CNT_POS)
#define MM_MISC_REG_IPROAI_RCMD_CNT_UMSK                          (~(((1U<<MM_MISC_REG_IPROAI_RCMD_CNT_LEN)-1)<<MM_MISC_REG_IPROAI_RCMD_CNT_POS))
#define MM_MISC_REG_IPROAI_PRO_RCMD_MODE                          MM_MISC_REG_IPROAI_PRO_RCMD_MODE
#define MM_MISC_REG_IPROAI_PRO_RCMD_MODE_POS                      (16U)
#define MM_MISC_REG_IPROAI_PRO_RCMD_MODE_LEN                      (2U)
#define MM_MISC_REG_IPROAI_PRO_RCMD_MODE_MSK                      (((1U<<MM_MISC_REG_IPROAI_PRO_RCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_RCMD_MODE_POS)
#define MM_MISC_REG_IPROAI_PRO_RCMD_MODE_UMSK                     (~(((1U<<MM_MISC_REG_IPROAI_PRO_RCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_RCMD_MODE_POS))
#define MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN                         MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN
#define MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_POS                     (20U)
#define MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_LEN                     (3U)
#define MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_MSK                     (((1U<<MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_POS)
#define MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_UMSK                    (~(((1U<<MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN_POS))
#define MM_MISC_REG_IPROAI_RCMD_MODE                              MM_MISC_REG_IPROAI_RCMD_MODE
#define MM_MISC_REG_IPROAI_RCMD_MODE_POS                          (31U)
#define MM_MISC_REG_IPROAI_RCMD_MODE_LEN                          (1U)
#define MM_MISC_REG_IPROAI_RCMD_MODE_MSK                          (((1U<<MM_MISC_REG_IPROAI_RCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_RCMD_MODE_POS)
#define MM_MISC_REG_IPROAI_RCMD_MODE_UMSK                         (~(((1U<<MM_MISC_REG_IPROAI_RCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_RCMD_MODE_POS))

/* 0x84 : iproai_lmtr_wr */
#define MM_MISC_IPROAI_LMTR_WR_OFFSET                             (0x84)
#define MM_MISC_REG_IPROAI_WCMD_CNT                               MM_MISC_REG_IPROAI_WCMD_CNT
#define MM_MISC_REG_IPROAI_WCMD_CNT_POS                           (0U)
#define MM_MISC_REG_IPROAI_WCMD_CNT_LEN                           (16U)
#define MM_MISC_REG_IPROAI_WCMD_CNT_MSK                           (((1U<<MM_MISC_REG_IPROAI_WCMD_CNT_LEN)-1)<<MM_MISC_REG_IPROAI_WCMD_CNT_POS)
#define MM_MISC_REG_IPROAI_WCMD_CNT_UMSK                          (~(((1U<<MM_MISC_REG_IPROAI_WCMD_CNT_LEN)-1)<<MM_MISC_REG_IPROAI_WCMD_CNT_POS))
#define MM_MISC_REG_IPROAI_PRO_WCMD_MODE                          MM_MISC_REG_IPROAI_PRO_WCMD_MODE
#define MM_MISC_REG_IPROAI_PRO_WCMD_MODE_POS                      (16U)
#define MM_MISC_REG_IPROAI_PRO_WCMD_MODE_LEN                      (2U)
#define MM_MISC_REG_IPROAI_PRO_WCMD_MODE_MSK                      (((1U<<MM_MISC_REG_IPROAI_PRO_WCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_WCMD_MODE_POS)
#define MM_MISC_REG_IPROAI_PRO_WCMD_MODE_UMSK                     (~(((1U<<MM_MISC_REG_IPROAI_PRO_WCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_WCMD_MODE_POS))
#define MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN                         MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN
#define MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_POS                     (20U)
#define MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_LEN                     (3U)
#define MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_MSK                     (((1U<<MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_POS)
#define MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_UMSK                    (~(((1U<<MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_LEN)-1)<<MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN_POS))
#define MM_MISC_REG_IPROAI_WCMD_MODE                              MM_MISC_REG_IPROAI_WCMD_MODE
#define MM_MISC_REG_IPROAI_WCMD_MODE_POS                          (31U)
#define MM_MISC_REG_IPROAI_WCMD_MODE_LEN                          (1U)
#define MM_MISC_REG_IPROAI_WCMD_MODE_MSK                          (((1U<<MM_MISC_REG_IPROAI_WCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_WCMD_MODE_POS)
#define MM_MISC_REG_IPROAI_WCMD_MODE_UMSK                         (~(((1U<<MM_MISC_REG_IPROAI_WCMD_MODE_LEN)-1)<<MM_MISC_REG_IPROAI_WCMD_MODE_POS))

/* 0xA0 : seof0_cfg */
#define MM_MISC_SEOF0_CFG_OFFSET                                (0xA0)
#define MM_MISC_RG_SEOF0_LCNT                                   MM_MISC_RG_SEOF0_LCNT
#define MM_MISC_RG_SEOF0_LCNT_POS                               (0U)
#define MM_MISC_RG_SEOF0_LCNT_LEN                               (14U)
#define MM_MISC_RG_SEOF0_LCNT_MSK                               (((1U<<MM_MISC_RG_SEOF0_LCNT_LEN)-1)<<MM_MISC_RG_SEOF0_LCNT_POS)
#define MM_MISC_RG_SEOF0_LCNT_UMSK                              (~(((1U<<MM_MISC_RG_SEOF0_LCNT_LEN)-1)<<MM_MISC_RG_SEOF0_LCNT_POS))
#define MM_MISC_RG_SEOF0_SRC                                    MM_MISC_RG_SEOF0_SRC
#define MM_MISC_RG_SEOF0_SRC_POS                                (16U)
#define MM_MISC_RG_SEOF0_SRC_LEN                                (4U)
#define MM_MISC_RG_SEOF0_SRC_MSK                                (((1U<<MM_MISC_RG_SEOF0_SRC_LEN)-1)<<MM_MISC_RG_SEOF0_SRC_POS)
#define MM_MISC_RG_SEOF0_SRC_UMSK                               (~(((1U<<MM_MISC_RG_SEOF0_SRC_LEN)-1)<<MM_MISC_RG_SEOF0_SRC_POS))
#define MM_MISC_RG_SEOF0_TYPE_EN                                MM_MISC_RG_SEOF0_TYPE_EN
#define MM_MISC_RG_SEOF0_TYPE_EN_POS                            (24U)
#define MM_MISC_RG_SEOF0_TYPE_EN_LEN                            (3U)
#define MM_MISC_RG_SEOF0_TYPE_EN_MSK                            (((1U<<MM_MISC_RG_SEOF0_TYPE_EN_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE_EN_POS)
#define MM_MISC_RG_SEOF0_TYPE_EN_UMSK                           (~(((1U<<MM_MISC_RG_SEOF0_TYPE_EN_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE_EN_POS))
#define MM_MISC_ST_SEOF0_TYPE                                   MM_MISC_ST_SEOF0_TYPE
#define MM_MISC_ST_SEOF0_TYPE_POS                               (28U)
#define MM_MISC_ST_SEOF0_TYPE_LEN                               (3U)
#define MM_MISC_ST_SEOF0_TYPE_MSK                               (((1U<<MM_MISC_ST_SEOF0_TYPE_LEN)-1)<<MM_MISC_ST_SEOF0_TYPE_POS)
#define MM_MISC_ST_SEOF0_TYPE_UMSK                              (~(((1U<<MM_MISC_ST_SEOF0_TYPE_LEN)-1)<<MM_MISC_ST_SEOF0_TYPE_POS))
#define MM_MISC_RG_SEOF0_INT_MASK                               MM_MISC_RG_SEOF0_INT_MASK
#define MM_MISC_RG_SEOF0_INT_MASK_POS                           (31U)
#define MM_MISC_RG_SEOF0_INT_MASK_LEN                           (1U)
#define MM_MISC_RG_SEOF0_INT_MASK_MSK                           (((1U<<MM_MISC_RG_SEOF0_INT_MASK_LEN)-1)<<MM_MISC_RG_SEOF0_INT_MASK_POS)
#define MM_MISC_RG_SEOF0_INT_MASK_UMSK                          (~(((1U<<MM_MISC_RG_SEOF0_INT_MASK_LEN)-1)<<MM_MISC_RG_SEOF0_INT_MASK_POS))

/* 0xA4 : seof1_cfg */
#define MM_MISC_SEOF1_CFG_OFFSET                                (0xA4)
#define MM_MISC_RG_SEOF1_LCNT                                   MM_MISC_RG_SEOF1_LCNT
#define MM_MISC_RG_SEOF1_LCNT_POS                               (0U)
#define MM_MISC_RG_SEOF1_LCNT_LEN                               (14U)
#define MM_MISC_RG_SEOF1_LCNT_MSK                               (((1U<<MM_MISC_RG_SEOF1_LCNT_LEN)-1)<<MM_MISC_RG_SEOF1_LCNT_POS)
#define MM_MISC_RG_SEOF1_LCNT_UMSK                              (~(((1U<<MM_MISC_RG_SEOF1_LCNT_LEN)-1)<<MM_MISC_RG_SEOF1_LCNT_POS))
#define MM_MISC_RG_SEOF1_SRC                                    MM_MISC_RG_SEOF1_SRC
#define MM_MISC_RG_SEOF1_SRC_POS                                (16U)
#define MM_MISC_RG_SEOF1_SRC_LEN                                (4U)
#define MM_MISC_RG_SEOF1_SRC_MSK                                (((1U<<MM_MISC_RG_SEOF1_SRC_LEN)-1)<<MM_MISC_RG_SEOF1_SRC_POS)
#define MM_MISC_RG_SEOF1_SRC_UMSK                               (~(((1U<<MM_MISC_RG_SEOF1_SRC_LEN)-1)<<MM_MISC_RG_SEOF1_SRC_POS))
#define MM_MISC_RG_SEOF1_TYPE_EN                                MM_MISC_RG_SEOF1_TYPE_EN
#define MM_MISC_RG_SEOF1_TYPE_EN_POS                            (24U)
#define MM_MISC_RG_SEOF1_TYPE_EN_LEN                            (3U)
#define MM_MISC_RG_SEOF1_TYPE_EN_MSK                            (((1U<<MM_MISC_RG_SEOF1_TYPE_EN_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE_EN_POS)
#define MM_MISC_RG_SEOF1_TYPE_EN_UMSK                           (~(((1U<<MM_MISC_RG_SEOF1_TYPE_EN_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE_EN_POS))
#define MM_MISC_ST_SEOF1_TYPE                                   MM_MISC_ST_SEOF1_TYPE
#define MM_MISC_ST_SEOF1_TYPE_POS                               (28U)
#define MM_MISC_ST_SEOF1_TYPE_LEN                               (3U)
#define MM_MISC_ST_SEOF1_TYPE_MSK                               (((1U<<MM_MISC_ST_SEOF1_TYPE_LEN)-1)<<MM_MISC_ST_SEOF1_TYPE_POS)
#define MM_MISC_ST_SEOF1_TYPE_UMSK                              (~(((1U<<MM_MISC_ST_SEOF1_TYPE_LEN)-1)<<MM_MISC_ST_SEOF1_TYPE_POS))
#define MM_MISC_RG_SEOF1_INT_MASK                               MM_MISC_RG_SEOF1_INT_MASK
#define MM_MISC_RG_SEOF1_INT_MASK_POS                           (31U)
#define MM_MISC_RG_SEOF1_INT_MASK_LEN                           (1U)
#define MM_MISC_RG_SEOF1_INT_MASK_MSK                           (((1U<<MM_MISC_RG_SEOF1_INT_MASK_LEN)-1)<<MM_MISC_RG_SEOF1_INT_MASK_POS)
#define MM_MISC_RG_SEOF1_INT_MASK_UMSK                          (~(((1U<<MM_MISC_RG_SEOF1_INT_MASK_LEN)-1)<<MM_MISC_RG_SEOF1_INT_MASK_POS))

/* 0xBC : seof_clr */
#define MM_MISC_SEOF_CLR_OFFSET                                 (0xBC)
#define MM_MISC_RG_SEOF0_TYPE0_CLR                              MM_MISC_RG_SEOF0_TYPE0_CLR
#define MM_MISC_RG_SEOF0_TYPE0_CLR_POS                          (0U)
#define MM_MISC_RG_SEOF0_TYPE0_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF0_TYPE0_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF0_TYPE0_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE0_CLR_POS)
#define MM_MISC_RG_SEOF0_TYPE0_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF0_TYPE0_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE0_CLR_POS))
#define MM_MISC_RG_SEOF0_TYPE1_CLR                              MM_MISC_RG_SEOF0_TYPE1_CLR
#define MM_MISC_RG_SEOF0_TYPE1_CLR_POS                          (1U)
#define MM_MISC_RG_SEOF0_TYPE1_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF0_TYPE1_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF0_TYPE1_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE1_CLR_POS)
#define MM_MISC_RG_SEOF0_TYPE1_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF0_TYPE1_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE1_CLR_POS))
#define MM_MISC_RG_SEOF0_TYPE2_CLR                              MM_MISC_RG_SEOF0_TYPE2_CLR
#define MM_MISC_RG_SEOF0_TYPE2_CLR_POS                          (2U)
#define MM_MISC_RG_SEOF0_TYPE2_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF0_TYPE2_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF0_TYPE2_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE2_CLR_POS)
#define MM_MISC_RG_SEOF0_TYPE2_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF0_TYPE2_CLR_LEN)-1)<<MM_MISC_RG_SEOF0_TYPE2_CLR_POS))
#define MM_MISC_RG_SEOF1_TYPE0_CLR                              MM_MISC_RG_SEOF1_TYPE0_CLR
#define MM_MISC_RG_SEOF1_TYPE0_CLR_POS                          (4U)
#define MM_MISC_RG_SEOF1_TYPE0_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF1_TYPE0_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF1_TYPE0_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE0_CLR_POS)
#define MM_MISC_RG_SEOF1_TYPE0_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF1_TYPE0_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE0_CLR_POS))
#define MM_MISC_RG_SEOF1_TYPE1_CLR                              MM_MISC_RG_SEOF1_TYPE1_CLR
#define MM_MISC_RG_SEOF1_TYPE1_CLR_POS                          (5U)
#define MM_MISC_RG_SEOF1_TYPE1_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF1_TYPE1_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF1_TYPE1_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE1_CLR_POS)
#define MM_MISC_RG_SEOF1_TYPE1_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF1_TYPE1_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE1_CLR_POS))
#define MM_MISC_RG_SEOF1_TYPE2_CLR                              MM_MISC_RG_SEOF1_TYPE2_CLR
#define MM_MISC_RG_SEOF1_TYPE2_CLR_POS                          (6U)
#define MM_MISC_RG_SEOF1_TYPE2_CLR_LEN                          (1U)
#define MM_MISC_RG_SEOF1_TYPE2_CLR_MSK                          (((1U<<MM_MISC_RG_SEOF1_TYPE2_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE2_CLR_POS)
#define MM_MISC_RG_SEOF1_TYPE2_CLR_UMSK                         (~(((1U<<MM_MISC_RG_SEOF1_TYPE2_CLR_LEN)-1)<<MM_MISC_RG_SEOF1_TYPE2_CLR_POS))

/* 0x100 : mm_misc_rst_0 */
#define MM_MISC_RST_0_OFFSET                                    (0x100)
#define MM_MISC_SWRST_MM_MISC                                   MM_MISC_SWRST_MM_MISC
#define MM_MISC_SWRST_MM_MISC_POS                               (0U)
#define MM_MISC_SWRST_MM_MISC_LEN                               (1U)
#define MM_MISC_SWRST_MM_MISC_MSK                               (((1U<<MM_MISC_SWRST_MM_MISC_LEN)-1)<<MM_MISC_SWRST_MM_MISC_POS)
#define MM_MISC_SWRST_MM_MISC_UMSK                              (~(((1U<<MM_MISC_SWRST_MM_MISC_LEN)-1)<<MM_MISC_SWRST_MM_MISC_POS))
#define MM_MISC_SWRST_D2XA                                      MM_MISC_SWRST_D2XA
#define MM_MISC_SWRST_D2XA_POS                                  (1U)
#define MM_MISC_SWRST_D2XA_LEN                                  (1U)
#define MM_MISC_SWRST_D2XA_MSK                                  (((1U<<MM_MISC_SWRST_D2XA_LEN)-1)<<MM_MISC_SWRST_D2XA_POS)
#define MM_MISC_SWRST_D2XA_UMSK                                 (~(((1U<<MM_MISC_SWRST_D2XA_LEN)-1)<<MM_MISC_SWRST_D2XA_POS))
#define MM_MISC_SWRST_D2XB                                      MM_MISC_SWRST_D2XB
#define MM_MISC_SWRST_D2XB_POS                                  (2U)
#define MM_MISC_SWRST_D2XB_LEN                                  (1U)
#define MM_MISC_SWRST_D2XB_MSK                                  (((1U<<MM_MISC_SWRST_D2XB_LEN)-1)<<MM_MISC_SWRST_D2XB_POS)
#define MM_MISC_SWRST_D2XB_UMSK                                 (~(((1U<<MM_MISC_SWRST_D2XB_LEN)-1)<<MM_MISC_SWRST_D2XB_POS))
#define MM_MISC_SWRST_D2XC                                      MM_MISC_SWRST_D2XC
#define MM_MISC_SWRST_D2XC_POS                                  (3U)
#define MM_MISC_SWRST_D2XC_LEN                                  (1U)
#define MM_MISC_SWRST_D2XC_MSK                                  (((1U<<MM_MISC_SWRST_D2XC_LEN)-1)<<MM_MISC_SWRST_D2XC_POS)
#define MM_MISC_SWRST_D2XC_UMSK                                 (~(((1U<<MM_MISC_SWRST_D2XC_LEN)-1)<<MM_MISC_SWRST_D2XC_POS))
#define MM_MISC_SWRST_D2XD                                      MM_MISC_SWRST_D2XD
#define MM_MISC_SWRST_D2XD_POS                                  (4U)
#define MM_MISC_SWRST_D2XD_LEN                                  (1U)
#define MM_MISC_SWRST_D2XD_MSK                                  (((1U<<MM_MISC_SWRST_D2XD_LEN)-1)<<MM_MISC_SWRST_D2XD_POS)
#define MM_MISC_SWRST_D2XD_UMSK                                 (~(((1U<<MM_MISC_SWRST_D2XD_LEN)-1)<<MM_MISC_SWRST_D2XD_POS))
#define MM_MISC_SWRST_SCHY                                      MM_MISC_SWRST_SCHY
#define MM_MISC_SWRST_SCHY_POS                                  (5U)
#define MM_MISC_SWRST_SCHY_LEN                                  (1U)
#define MM_MISC_SWRST_SCHY_MSK                                  (((1U<<MM_MISC_SWRST_SCHY_LEN)-1)<<MM_MISC_SWRST_SCHY_POS)
#define MM_MISC_SWRST_SCHY_UMSK                                 (~(((1U<<MM_MISC_SWRST_SCHY_LEN)-1)<<MM_MISC_SWRST_SCHY_POS))
#define MM_MISC_SWRST_OSD_DRAW                                  MM_MISC_SWRST_OSD_DRAW
#define MM_MISC_SWRST_OSD_DRAW_POS                              (6U)
#define MM_MISC_SWRST_OSD_DRAW_LEN                              (1U)
#define MM_MISC_SWRST_OSD_DRAW_MSK                              (((1U<<MM_MISC_SWRST_OSD_DRAW_LEN)-1)<<MM_MISC_SWRST_OSD_DRAW_POS)
#define MM_MISC_SWRST_OSD_DRAW_UMSK                             (~(((1U<<MM_MISC_SWRST_OSD_DRAW_LEN)-1)<<MM_MISC_SWRST_OSD_DRAW_POS))
#define MM_MISC_SWRST_OSD_BLEND                                 MM_MISC_SWRST_OSD_BLEND
#define MM_MISC_SWRST_OSD_BLEND_POS                             (7U)
#define MM_MISC_SWRST_OSD_BLEND_LEN                             (1U)
#define MM_MISC_SWRST_OSD_BLEND_MSK                             (((1U<<MM_MISC_SWRST_OSD_BLEND_LEN)-1)<<MM_MISC_SWRST_OSD_BLEND_POS)
#define MM_MISC_SWRST_OSD_BLEND_UMSK                            (~(((1U<<MM_MISC_SWRST_OSD_BLEND_LEN)-1)<<MM_MISC_SWRST_OSD_BLEND_POS))
#define MM_MISC_SWRST_JPG                                       MM_MISC_SWRST_JPG
#define MM_MISC_SWRST_JPG_POS                                   (8U)
#define MM_MISC_SWRST_JPG_LEN                                   (1U)
#define MM_MISC_SWRST_JPG_MSK                                   (((1U<<MM_MISC_SWRST_JPG_LEN)-1)<<MM_MISC_SWRST_JPG_POS)
#define MM_MISC_SWRST_JPG_UMSK                                  (~(((1U<<MM_MISC_SWRST_JPG_LEN)-1)<<MM_MISC_SWRST_JPG_POS))
#define MM_MISC_SWRST_IPROAI                                      MM_MISC_SWRST_IPROAI
#define MM_MISC_SWRST_IPROAI_POS                                  (9U)
#define MM_MISC_SWRST_IPROAI_LEN                                  (1U)
#define MM_MISC_SWRST_IPROAI_MSK                                  (((1U<<MM_MISC_SWRST_IPROAI_LEN)-1)<<MM_MISC_SWRST_IPROAI_POS)
#define MM_MISC_SWRST_IPROAI_UMSK                                 (~(((1U<<MM_MISC_SWRST_IPROAI_LEN)-1)<<MM_MISC_SWRST_IPROAI_POS))
#define MM_MISC_SWRST_ISP                                       MM_MISC_SWRST_ISP
#define MM_MISC_SWRST_ISP_POS                                   (10U)
#define MM_MISC_SWRST_ISP_LEN                                   (1U)
#define MM_MISC_SWRST_ISP_MSK                                   (((1U<<MM_MISC_SWRST_ISP_LEN)-1)<<MM_MISC_SWRST_ISP_POS)
#define MM_MISC_SWRST_ISP_UMSK                                  (~(((1U<<MM_MISC_SWRST_ISP_LEN)-1)<<MM_MISC_SWRST_ISP_POS))
#define MM_MISC_SWRST_DTSRC                                     MM_MISC_SWRST_DTSRC
#define MM_MISC_SWRST_DTSRC_POS                                 (11U)
#define MM_MISC_SWRST_DTSRC_LEN                                 (1U)
#define MM_MISC_SWRST_DTSRC_MSK                                 (((1U<<MM_MISC_SWRST_DTSRC_LEN)-1)<<MM_MISC_SWRST_DTSRC_POS)
#define MM_MISC_SWRST_DTSRC_UMSK                                (~(((1U<<MM_MISC_SWRST_DTSRC_LEN)-1)<<MM_MISC_SWRST_DTSRC_POS))
#define MM_MISC_SWRST_SPI2DVP                                   MM_MISC_SWRST_SPI2DVP
#define MM_MISC_SWRST_SPI2DVP_POS                               (12U)
#define MM_MISC_SWRST_SPI2DVP_LEN                               (1U)
#define MM_MISC_SWRST_SPI2DVP_MSK                               (((1U<<MM_MISC_SWRST_SPI2DVP_LEN)-1)<<MM_MISC_SWRST_SPI2DVP_POS)
#define MM_MISC_SWRST_SPI2DVP_UMSK                              (~(((1U<<MM_MISC_SWRST_SPI2DVP_LEN)-1)<<MM_MISC_SWRST_SPI2DVP_POS))
#define MM_MISC_SWRST_DVP_AS                                    MM_MISC_SWRST_DVP_AS
#define MM_MISC_SWRST_DVP_AS_POS                                (13U)
#define MM_MISC_SWRST_DVP_AS_LEN                                (1U)
#define MM_MISC_SWRST_DVP_AS_MSK                                (((1U<<MM_MISC_SWRST_DVP_AS_LEN)-1)<<MM_MISC_SWRST_DVP_AS_POS)
#define MM_MISC_SWRST_DVP_AS_UMSK                               (~(((1U<<MM_MISC_SWRST_DVP_AS_LEN)-1)<<MM_MISC_SWRST_DVP_AS_POS))

/* 0x120 : mm_misc_clk_0 */
#define MM_MISC_CLK_0_OFFSET                                    (0x120)
#define MM_MISC_CGEN_SCHY                                       MM_MISC_CGEN_SCHY
#define MM_MISC_CGEN_SCHY_POS                                   (0U)
#define MM_MISC_CGEN_SCHY_LEN                                   (1U)
#define MM_MISC_CGEN_SCHY_MSK                                   (((1U<<MM_MISC_CGEN_SCHY_LEN)-1)<<MM_MISC_CGEN_SCHY_POS)
#define MM_MISC_CGEN_SCHY_UMSK                                  (~(((1U<<MM_MISC_CGEN_SCHY_LEN)-1)<<MM_MISC_CGEN_SCHY_POS))
#define MM_MISC_CGEN_OSD_DRAW                                   MM_MISC_CGEN_OSD_DRAW
#define MM_MISC_CGEN_OSD_DRAW_POS                               (1U)
#define MM_MISC_CGEN_OSD_DRAW_LEN                               (1U)
#define MM_MISC_CGEN_OSD_DRAW_MSK                               (((1U<<MM_MISC_CGEN_OSD_DRAW_LEN)-1)<<MM_MISC_CGEN_OSD_DRAW_POS)
#define MM_MISC_CGEN_OSD_DRAW_UMSK                              (~(((1U<<MM_MISC_CGEN_OSD_DRAW_LEN)-1)<<MM_MISC_CGEN_OSD_DRAW_POS))
#define MM_MISC_CGEN_OSD_BLEND                                  MM_MISC_CGEN_OSD_BLEND
#define MM_MISC_CGEN_OSD_BLEND_POS                              (2U)
#define MM_MISC_CGEN_OSD_BLEND_LEN                              (1U)
#define MM_MISC_CGEN_OSD_BLEND_MSK                              (((1U<<MM_MISC_CGEN_OSD_BLEND_LEN)-1)<<MM_MISC_CGEN_OSD_BLEND_POS)
#define MM_MISC_CGEN_OSD_BLEND_UMSK                             (~(((1U<<MM_MISC_CGEN_OSD_BLEND_LEN)-1)<<MM_MISC_CGEN_OSD_BLEND_POS))
#define MM_MISC_CGEN_ISP                                        MM_MISC_CGEN_ISP
#define MM_MISC_CGEN_ISP_POS                                    (3U)
#define MM_MISC_CGEN_ISP_LEN                                    (1U)
#define MM_MISC_CGEN_ISP_MSK                                    (((1U<<MM_MISC_CGEN_ISP_LEN)-1)<<MM_MISC_CGEN_ISP_POS)
#define MM_MISC_CGEN_ISP_UMSK                                   (~(((1U<<MM_MISC_CGEN_ISP_LEN)-1)<<MM_MISC_CGEN_ISP_POS))
#define MM_MISC_CGEN_JPG                                        MM_MISC_CGEN_JPG
#define MM_MISC_CGEN_JPG_POS                                    (4U)
#define MM_MISC_CGEN_JPG_LEN                                    (1U)
#define MM_MISC_CGEN_JPG_MSK                                    (((1U<<MM_MISC_CGEN_JPG_LEN)-1)<<MM_MISC_CGEN_JPG_POS)
#define MM_MISC_CGEN_JPG_UMSK                                   (~(((1U<<MM_MISC_CGEN_JPG_LEN)-1)<<MM_MISC_CGEN_JPG_POS))
#define MM_MISC_CGEN_D2XA                                       MM_MISC_CGEN_D2XA
#define MM_MISC_CGEN_D2XA_POS                                   (5U)
#define MM_MISC_CGEN_D2XA_LEN                                   (1U)
#define MM_MISC_CGEN_D2XA_MSK                                   (((1U<<MM_MISC_CGEN_D2XA_LEN)-1)<<MM_MISC_CGEN_D2XA_POS)
#define MM_MISC_CGEN_D2XA_UMSK                                  (~(((1U<<MM_MISC_CGEN_D2XA_LEN)-1)<<MM_MISC_CGEN_D2XA_POS))
#define MM_MISC_CGEN_D2XB                                       MM_MISC_CGEN_D2XB
#define MM_MISC_CGEN_D2XB_POS                                   (6U)
#define MM_MISC_CGEN_D2XB_LEN                                   (1U)
#define MM_MISC_CGEN_D2XB_MSK                                   (((1U<<MM_MISC_CGEN_D2XB_LEN)-1)<<MM_MISC_CGEN_D2XB_POS)
#define MM_MISC_CGEN_D2XB_UMSK                                  (~(((1U<<MM_MISC_CGEN_D2XB_LEN)-1)<<MM_MISC_CGEN_D2XB_POS))
#define MM_MISC_CGEN_D2XC                                       MM_MISC_CGEN_D2XC
#define MM_MISC_CGEN_D2XC_POS                                   (7U)
#define MM_MISC_CGEN_D2XC_LEN                                   (1U)
#define MM_MISC_CGEN_D2XC_MSK                                   (((1U<<MM_MISC_CGEN_D2XC_LEN)-1)<<MM_MISC_CGEN_D2XC_POS)
#define MM_MISC_CGEN_D2XC_UMSK                                  (~(((1U<<MM_MISC_CGEN_D2XC_LEN)-1)<<MM_MISC_CGEN_D2XC_POS))
#define MM_MISC_CGEN_DTSRC                                      MM_MISC_CGEN_DTSRC
#define MM_MISC_CGEN_DTSRC_POS                                  (8U)
#define MM_MISC_CGEN_DTSRC_LEN                                  (1U)
#define MM_MISC_CGEN_DTSRC_MSK                                  (((1U<<MM_MISC_CGEN_DTSRC_LEN)-1)<<MM_MISC_CGEN_DTSRC_POS)
#define MM_MISC_CGEN_DTSRC_UMSK                                 (~(((1U<<MM_MISC_CGEN_DTSRC_LEN)-1)<<MM_MISC_CGEN_DTSRC_POS))


struct  mm_misc_reg {
    /* 0x0 : mm_misc_0 */
    union {
        struct {
            uint32_t reg_force_pclk_on              : 16; /* [15: 0],        r/w,     0xffff */
            uint32_t reserved_16_29                 : 14; /* [29:16],       rsvd,        0x0 */
            uint32_t cr_snsr_pwdn                   :  1; /* [   30],        r/w,        0x0 */
            uint32_t cr_snsr_reset                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_0;

    /* 0x4 : mm_misc_1 */
    union {
        struct {
            uint32_t reg_dvp_mux_sh_isp             :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_scaler          :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_osd_blend       :  1; /* [    2],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_osd_draw        :  1; /* [    3],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_422to444        :  1; /* [    4],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_422to420        :  1; /* [    5],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_trA             :  1; /* [    6],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_444to422        :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_d2xA            :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_d2xB            :  1; /* [    9],        w1p,        0x0 */
            uint32_t reg_dvp_mux_sh_d2xC            :  1; /* [   10],        w1p,        0x0 */
            uint32_t reserved_11_31                 : 21; /* [31:11],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_1;

    /* 0x8 : mm_misc_2 */
    union {
        struct {
            uint32_t cr_dvp_as_sram_mode            :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2_3                   :  2; /* [ 3: 2],       rsvd,        0x0 */
            uint32_t cr_spi2dvp_sel                 :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_2;

    /* 0xc  reserved */
    uint8_t RESERVED0xc[4];

    /* 0x10 : d2x_mux_reg0 */
    union {
        struct {
            uint32_t reg_d2xA_in_sel                :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_d2xB_in_sel                :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_d2xC_in_sel                :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reserved_12_23                 : 12; /* [23:12],       rsvd,        0x0 */
            uint32_t cr_mj_y_sel_d2x                :  2; /* [25:24],        r/w,        0x0 */
            uint32_t reserved_26_27                 :  2; /* [27:26],       rsvd,        0x0 */
            uint32_t cr_mj_c_sel_d2x                :  2; /* [29:28],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } d2x_mux_reg0;

    /* 0x14 : yuv422_mux_reg0 */
    union {
        struct {
            uint32_t reg_isp_in_sel                 :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t reg_schy_in_sel                :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t reg_osd_blend_in_sel           :  4; /* [11: 8],        r/w,        0x0 */
            uint32_t reg_osd_draw_in_sel            :  4; /* [15:12],        r/w,        0x0 */
            uint32_t reg_422to444_in_sel            :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reg_422to420_in_sel            :  4; /* [23:20],        r/w,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } yuv422_mux_reg0;

    /* 0x18 : yuv444_mux_reg0 */
    union {
        struct {
            uint32_t reg_trA_in_sel                 :  2; /* [ 1: 0],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
            uint32_t reg_yuv444to422_in_sel         :  2; /* [ 4: 3],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } yuv444_mux_reg0;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[4];

    /* 0x20 : cropA_reg_0 */
    union {
        struct {
            uint32_t reg_cropA_vsync_start          : 15; /* [14: 0],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t reg_cropA_vsync_end            : 15; /* [30:16],        r/w,     0x7fff */
            uint32_t reg_cropA_enable               :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } cropA_reg_0;

    /* 0x24 : cropA_reg_1 */
    union {
        struct {
            uint32_t reg_cropA_hsync_start          : 15; /* [14: 0],        r/w,        0x0 */
            uint32_t reserved_15                    :  1; /* [   15],       rsvd,        0x0 */
            uint32_t reg_cropA_hsync_end            : 15; /* [30:16],        r/w,     0x7fff */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } cropA_reg_1;

    /* 0x28  reserved */
    uint8_t RESERVED0x28[24];

    /* 0x40 : y2rA_reg_0 */
    union {
        struct {
            uint32_t rg_y2rA_pre_0                  :  9; /* [ 8: 0],        r/w,        0x0 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t rg_y2rA_pos_0                  :  9; /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_26                 :  2; /* [26:25],       rsvd,        0x0 */
            uint32_t rg_y2rA_en                     :  1; /* [   27],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_0;

    /* 0x44 : y2rA_reg_1 */
    union {
        struct {
            uint32_t rg_y2rA_pre_1                  :  9; /* [ 8: 0],        r/w,      0x180 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t rg_y2rA_pos_1                  :  9; /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_1;

    /* 0x48 : y2rA_reg_2 */
    union {
        struct {
            uint32_t rg_y2rA_pre_2                  :  9; /* [ 8: 0],        r/w,      0x180 */
            uint32_t reserved_9_15                  :  7; /* [15: 9],       rsvd,        0x0 */
            uint32_t rg_y2rA_pos_2                  :  9; /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31                 :  7; /* [31:25],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_2;

    /* 0x4C : y2rA_reg_3 */
    union {
        struct {
            uint32_t rg_y2rA_mtx_00                 : 12; /* [11: 0],        r/w,      0x200 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_y2rA_mtx_01                 : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_3;

    /* 0x50 : y2rA_reg_4 */
    union {
        struct {
            uint32_t rg_y2rA_mtx_02                 : 12; /* [11: 0],        r/w,      0x2ce */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_y2rA_mtx_10                 : 12; /* [27:16],        r/w,      0x200 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_4;

    /* 0x54 : y2rA_reg_5 */
    union {
        struct {
            uint32_t rg_y2rA_mtx_11                 : 12; /* [11: 0],        r/w,      0xf50 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_y2rA_mtx_12                 : 12; /* [27:16],        r/w,      0xe92 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_5;

    /* 0x58 : y2rA_reg_6 */
    union {
        struct {
            uint32_t rg_y2rA_mtx_20                 : 12; /* [11: 0],        r/w,      0x200 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t rg_y2rA_mtx_21                 : 12; /* [27:16],        r/w,      0x38b */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_6;

    /* 0x5C : y2rA_reg_7 */
    union {
        struct {
            uint32_t rg_y2rA_mtx_22                 : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } y2rA_reg_7;

    /* 0x60 : spi2dvp_ctrl */
    union {
        struct {
            uint32_t rg_spi2dvp_enable              :  1; /* [    0],        r/w,        0x0 */
            uint32_t rg_spi2dvp_endian              :  1; /* [    1],        r/w,        0x0 */
            uint32_t rg_spi2dvp_sclk_pol            :  1; /* [    2],        r/w,        0x0 */
            uint32_t rg_spi2dvp_sclk_ph             :  1; /* [    3],        r/w,        0x0 */
            uint32_t rg_spi2dvp_data_format         :  1; /* [    4],        r/w,        0x0 */
            uint32_t rg_spi2dvp_data_order          :  1; /* [    5],        r/w,        0x0 */
            uint32_t reserved_6_15                  : 10; /* [15: 6],       rsvd,        0x0 */
            uint32_t rg_spi2dvp_int_en              :  1; /* [   16],        r/w,        0x0 */
            uint32_t spi2dvp_sync_int               :  1; /* [   17],          r,        0x0 */
            uint32_t spi2dvp_fs_int                 :  1; /* [   18],          r,        0x0 */
            uint32_t spi2dvp_fe_int                 :  1; /* [   19],          r,        0x0 */
            uint32_t spi2dvp_ls_int                 :  1; /* [   20],          r,        0x0 */
            uint32_t spi2dvp_fifo_int               :  1; /* [   21],          r,        0x0 */
            uint32_t reserved_22_30                 :  9; /* [30:22],       rsvd,        0x0 */
            uint32_t st_spi2dvp_bus_busy            :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } spi2dvp_ctrl;

    /* 0x64 : spi2dvp_status_0 */
    union {
        struct {
            uint32_t st_spi2dvp_pkt_id              :  8; /* [ 7: 0],          r,       0xff */
            uint32_t st_spi2dvp_data_id             :  8; /* [15: 8],          r,       0xff */
            uint32_t st_spi2dvp_data_size           : 16; /* [31:16],          r,     0xffff */
        }BF;
        uint32_t WORD;
    } spi2dvp_status_0;

    /* 0x68 : spi2dvp_status_1 */
    union {
        struct {
            uint32_t st_spi2dvp_img_w               : 16; /* [15: 0],          r,     0xffff */
            uint32_t st_spi2dvp_img_h               : 16; /* [31:16],          r,     0xffff */
        }BF;
        uint32_t WORD;
    } spi2dvp_status_1;

    /* 0x6c  reserved */
    uint8_t RESERVED0x6c[4];

    /* 0x70 : dvp_as_reg0 */
    union {
        struct {
            uint32_t cr_dvp_as_en                   :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_dvp_as_hs_inv               :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_dvp_as_vs_inv               :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_dvp_as_tg_en                :  1; /* [    3],        r/w,        0x0 */
            uint32_t cr_dvp_as_fifo_th              : 12; /* [15: 4],        r/w,        0x0 */
            uint32_t cr_dvp_s2p_en                  :  1; /* [   16],        r/w,        0x0 */
            uint32_t cr_dvp_s2p_vs_inv              :  1; /* [   17],        r/w,        0x0 */
            uint32_t cr_dvp_s2p_hs_inv              :  1; /* [   18],        r/w,        0x0 */
            uint32_t cr_dvp_s2p_da_order            :  1; /* [   19],        r/w,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_as_reg0;

    /* 0x74 : dvp_as_reg1 */
    union {
        struct {
            uint32_t cr_dvp_as_act_pixel_width      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_dvp_as_act_start_pixel      : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_as_reg1;

    /* 0x78 : dvp_as_reg2 */
    union {
        struct {
            uint32_t cr_dvp_as_total_hpixels        : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_dvp_as_act_start_line       : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_as_reg2;

    /* 0x7C : dvp_as_reg3 */
    union {
        struct {
            uint32_t cr_dvp_as_act_line_height      : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_as_reg3;

    /* 0x80 : iproai_lmtr_rd */
    union {
        struct {
            uint32_t reg_iproai_rcmd_cnt              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reg_iproai_pro_rcmd_mode         :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t reg_iproai_pro_rd_mstr_en        :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23_30                 :  8; /* [30:23],       rsvd,        0x0 */
            uint32_t reg_iproai_rcmd_mode             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } iproai_lmtr_rd;

    /* 0x84 : iproai_lmtr_wr */
    union {
        struct {
            uint32_t reg_iproai_wcmd_cnt              : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reg_iproai_pro_wcmd_mode         :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18_19                 :  2; /* [19:18],       rsvd,        0x0 */
            uint32_t reg_iproai_pro_wr_mstr_en        :  3; /* [22:20],        r/w,        0x0 */
            uint32_t reserved_23_30                 :  8; /* [30:23],       rsvd,        0x0 */
            uint32_t reg_iproai_wcmd_mode             :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } iproai_lmtr_wr;

    /* 0x88  reserved */
    uint8_t RESERVED0x88[24];

    /* 0xA0 : seof0_cfg */
    union {
        struct {
            uint32_t rg_seof0_lcnt                  : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rg_seof0_src                   :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t rg_seof0_type_en               :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t st_seof0_type                  :  3; /* [30:28],          r,        0x0 */
            uint32_t rg_seof0_int_mask              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } seof0_cfg;

    /* 0xA4 : seof1_cfg */
    union {
        struct {
            uint32_t rg_seof1_lcnt                  : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t rg_seof1_src                   :  4; /* [19:16],        r/w,        0x0 */
            uint32_t reserved_20_23                 :  4; /* [23:20],       rsvd,        0x0 */
            uint32_t rg_seof1_type_en               :  3; /* [26:24],        r/w,        0x0 */
            uint32_t reserved_27                    :  1; /* [   27],       rsvd,        0x0 */
            uint32_t st_seof1_type                  :  3; /* [30:28],          r,        0x0 */
            uint32_t rg_seof1_int_mask              :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } seof1_cfg;

    /* 0xa8  reserved */
    uint8_t RESERVED0xa8[20];

    /* 0xBC : seof_clr */
    union {
        struct {
            uint32_t rg_seof0_type0_clr             :  1; /* [    0],        w1p,        0x0 */
            uint32_t rg_seof0_type1_clr             :  1; /* [    1],        w1p,        0x0 */
            uint32_t rg_seof0_type2_clr             :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t rg_seof1_type0_clr             :  1; /* [    4],        w1p,        0x0 */
            uint32_t rg_seof1_type1_clr             :  1; /* [    5],        w1p,        0x0 */
            uint32_t rg_seof1_type2_clr             :  1; /* [    6],        w1p,        0x0 */
            uint32_t reserved_7_31                  : 25; /* [31: 7],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } seof_clr;

    /* 0xc0  reserved */
    uint8_t RESERVED0xc0[64];

    /* 0x100 : mm_misc_rst_0 */
    union {
        struct {
            uint32_t swrst_mm_misc                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t swrst_d2xA                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t swrst_d2xB                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t swrst_d2xC                     :  1; /* [    3],        r/w,        0x0 */
            uint32_t swrst_d2xD                     :  1; /* [    4],        r/w,        0x0 */
            uint32_t swrst_schy                     :  1; /* [    5],        r/w,        0x0 */
            uint32_t swrst_osd_draw                 :  1; /* [    6],        r/w,        0x0 */
            uint32_t swrst_osd_blend                :  1; /* [    7],        r/w,        0x0 */
            uint32_t swrst_jpg                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t swrst_blai                     :  1; /* [    9],        r/w,        0x0 */
            uint32_t swrst_isp                      :  1; /* [   10],        r/w,        0x0 */
            uint32_t swrst_dtsrc                    :  1; /* [   11],        r/w,        0x0 */
            uint32_t swrst_spi2dvp                  :  1; /* [   12],        r/w,        0x0 */
            uint32_t swrst_dvp_as                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_rst_0;

    /* 0x104  reserved */
    uint8_t RESERVED0x104[28];

    /* 0x120 : mm_misc_clk_0 */
    union {
        struct {
            uint32_t cgen_schy                      :  1; /* [    0],        r/w,        0x1 */
            uint32_t cgen_osd_draw                  :  1; /* [    1],        r/w,        0x1 */
            uint32_t cgen_osd_blend                 :  1; /* [    2],        r/w,        0x1 */
            uint32_t cgen_isp                       :  1; /* [    3],        r/w,        0x1 */
            uint32_t cgen_jpg                       :  1; /* [    4],        r/w,        0x1 */
            uint32_t cgen_d2xA                      :  1; /* [    5],        r/w,        0x1 */
            uint32_t cgen_d2xB                      :  1; /* [    6],        r/w,        0x1 */
            uint32_t cgen_d2xC                      :  1; /* [    7],        r/w,        0x1 */
            uint32_t cgen_dtsrc                     :  1; /* [    8],        r/w,        0x1 */
            uint32_t reserved_9_31                  : 23; /* [31: 9],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_clk_0;

};

typedef volatile struct mm_misc_reg mm_misc_reg_t;


#endif  /* __MM_MISC_REG_H__ */
