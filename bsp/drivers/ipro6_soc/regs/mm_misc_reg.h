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

#include "ipro6.h"

/* 0x0 : mm_misc_0 */
#define MM_MISC_0_OFFSET                                        (0x0)
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
#define MM_MISC_ST_SPI2DVP_SYNC_INT                             MM_MISC_ST_SPI2DVP_SYNC_INT
#define MM_MISC_ST_SPI2DVP_SYNC_INT_POS                         (17U)
#define MM_MISC_ST_SPI2DVP_SYNC_INT_LEN                         (1U)
#define MM_MISC_ST_SPI2DVP_SYNC_INT_MSK                         (((1U<<MM_MISC_ST_SPI2DVP_SYNC_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_SYNC_INT_POS)
#define MM_MISC_ST_SPI2DVP_SYNC_INT_UMSK                        (~(((1U<<MM_MISC_ST_SPI2DVP_SYNC_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_SYNC_INT_POS))
#define MM_MISC_ST_SPI2DVP_FS_INT                               MM_MISC_ST_SPI2DVP_FS_INT
#define MM_MISC_ST_SPI2DVP_FS_INT_POS                           (18U)
#define MM_MISC_ST_SPI2DVP_FS_INT_LEN                           (1U)
#define MM_MISC_ST_SPI2DVP_FS_INT_MSK                           (((1U<<MM_MISC_ST_SPI2DVP_FS_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FS_INT_POS)
#define MM_MISC_ST_SPI2DVP_FS_INT_UMSK                          (~(((1U<<MM_MISC_ST_SPI2DVP_FS_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FS_INT_POS))
#define MM_MISC_ST_SPI2DVP_FE_INT                               MM_MISC_ST_SPI2DVP_FE_INT
#define MM_MISC_ST_SPI2DVP_FE_INT_POS                           (19U)
#define MM_MISC_ST_SPI2DVP_FE_INT_LEN                           (1U)
#define MM_MISC_ST_SPI2DVP_FE_INT_MSK                           (((1U<<MM_MISC_ST_SPI2DVP_FE_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FE_INT_POS)
#define MM_MISC_ST_SPI2DVP_FE_INT_UMSK                          (~(((1U<<MM_MISC_ST_SPI2DVP_FE_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FE_INT_POS))
#define MM_MISC_ST_SPI2DVP_LS_INT                               MM_MISC_ST_SPI2DVP_LS_INT
#define MM_MISC_ST_SPI2DVP_LS_INT_POS                           (20U)
#define MM_MISC_ST_SPI2DVP_LS_INT_LEN                           (1U)
#define MM_MISC_ST_SPI2DVP_LS_INT_MSK                           (((1U<<MM_MISC_ST_SPI2DVP_LS_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_LS_INT_POS)
#define MM_MISC_ST_SPI2DVP_LS_INT_UMSK                          (~(((1U<<MM_MISC_ST_SPI2DVP_LS_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_LS_INT_POS))
#define MM_MISC_ST_SPI2DVP_FIFO_INT                             MM_MISC_ST_SPI2DVP_FIFO_INT
#define MM_MISC_ST_SPI2DVP_FIFO_INT_POS                         (21U)
#define MM_MISC_ST_SPI2DVP_FIFO_INT_LEN                         (1U)
#define MM_MISC_ST_SPI2DVP_FIFO_INT_MSK                         (((1U<<MM_MISC_ST_SPI2DVP_FIFO_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FIFO_INT_POS)
#define MM_MISC_ST_SPI2DVP_FIFO_INT_UMSK                        (~(((1U<<MM_MISC_ST_SPI2DVP_FIFO_INT_LEN)-1)<<MM_MISC_ST_SPI2DVP_FIFO_INT_POS))
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
#define MM_MISC_CR_DVP_AS_ROBUST_EN                             MM_MISC_CR_DVP_AS_ROBUST_EN
#define MM_MISC_CR_DVP_AS_ROBUST_EN_POS                         (1U)
#define MM_MISC_CR_DVP_AS_ROBUST_EN_LEN                         (1U)
#define MM_MISC_CR_DVP_AS_ROBUST_EN_MSK                         (((1U<<MM_MISC_CR_DVP_AS_ROBUST_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_ROBUST_EN_POS)
#define MM_MISC_CR_DVP_AS_ROBUST_EN_UMSK                        (~(((1U<<MM_MISC_CR_DVP_AS_ROBUST_EN_LEN)-1)<<MM_MISC_CR_DVP_AS_ROBUST_EN_POS))
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
#define MM_MISC_SWRST_JPG                                       MM_MISC_SWRST_JPG
#define MM_MISC_SWRST_JPG_POS                                   (8U)
#define MM_MISC_SWRST_JPG_LEN                                   (1U)
#define MM_MISC_SWRST_JPG_MSK                                   (((1U<<MM_MISC_SWRST_JPG_LEN)-1)<<MM_MISC_SWRST_JPG_POS)
#define MM_MISC_SWRST_JPG_UMSK                                  (~(((1U<<MM_MISC_SWRST_JPG_LEN)-1)<<MM_MISC_SWRST_JPG_POS))
#define MM_MISC_SWRST_DVP_AS                                    MM_MISC_SWRST_DVP_AS
#define MM_MISC_SWRST_DVP_AS_POS                                (13U)
#define MM_MISC_SWRST_DVP_AS_LEN                                (1U)
#define MM_MISC_SWRST_DVP_AS_MSK                                (((1U<<MM_MISC_SWRST_DVP_AS_LEN)-1)<<MM_MISC_SWRST_DVP_AS_POS)
#define MM_MISC_SWRST_DVP_AS_UMSK                               (~(((1U<<MM_MISC_SWRST_DVP_AS_LEN)-1)<<MM_MISC_SWRST_DVP_AS_POS))


struct  mm_misc_reg {
    /* 0x0 : mm_misc_0 */
    union {
        struct {
            uint32_t reserved_0_29                  : 30; /* [29: 0],       rsvd,        0x0 */
            uint32_t cr_snsr_pwdn                   :  1; /* [   30],        r/w,        0x0 */
            uint32_t cr_snsr_reset                  :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_0;

    /* 0x4  reserved */
    uint8_t RESERVED0x4[92];

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
            uint32_t st_spi2dvp_sync_int            :  1; /* [   17],          r,        0x0 */
            uint32_t st_spi2dvp_fs_int              :  1; /* [   18],          r,        0x0 */
            uint32_t st_spi2dvp_fe_int              :  1; /* [   19],          r,        0x0 */
            uint32_t st_spi2dvp_ls_int              :  1; /* [   20],          r,        0x0 */
            uint32_t st_spi2dvp_fifo_int            :  1; /* [   21],          r,        0x0 */
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
            uint32_t cr_dvp_as_robust_en            :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2                     :  1; /* [    2],       rsvd,        0x0 */
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

    /* 0x80  reserved */
    uint8_t RESERVED0x80[128];

    /* 0x100 : mm_misc_rst_0 */
    union {
        struct {
            uint32_t swrst_mm_misc                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t swrst_d2xA                     :  1; /* [    1],        r/w,        0x0 */
            uint32_t swrst_d2xB                     :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t swrst_jpg                      :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_12                  :  4; /* [12: 9],       rsvd,        0x0 */
            uint32_t swrst_dvp_as                   :  1; /* [   13],        r/w,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } mm_misc_rst_0;

};

typedef volatile struct mm_misc_reg mm_misc_reg_t;


#endif  /* __MM_MISC_REG_H__ */
