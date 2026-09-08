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

#ifndef  __DVP_TSRC_LITE_REG_H__
#define  __DVP_TSRC_LITE_REG_H__

#include "ipro7.h"

/* 0x0 : config */
#define DVP_TSRC_LITE_CONFIG_OFFSET                             (0x0)
#define DVP_TSRC_LITE_CR_ENABLE                                 DVP_TSRC_LITE_CR_ENABLE
#define DVP_TSRC_LITE_CR_ENABLE_POS                             (0U)
#define DVP_TSRC_LITE_CR_ENABLE_LEN                             (1U)
#define DVP_TSRC_LITE_CR_ENABLE_MSK                             (((1U<<DVP_TSRC_LITE_CR_ENABLE_LEN)-1)<<DVP_TSRC_LITE_CR_ENABLE_POS)
#define DVP_TSRC_LITE_CR_ENABLE_UMSK                            (~(((1U<<DVP_TSRC_LITE_CR_ENABLE_LEN)-1)<<DVP_TSRC_LITE_CR_ENABLE_POS))
#define DVP_TSRC_LITE_CR_AXI_EN                                 DVP_TSRC_LITE_CR_AXI_EN
#define DVP_TSRC_LITE_CR_AXI_EN_POS                             (1U)
#define DVP_TSRC_LITE_CR_AXI_EN_LEN                             (1U)
#define DVP_TSRC_LITE_CR_AXI_EN_MSK                             (((1U<<DVP_TSRC_LITE_CR_AXI_EN_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_EN_POS)
#define DVP_TSRC_LITE_CR_AXI_EN_UMSK                            (~(((1U<<DVP_TSRC_LITE_CR_AXI_EN_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_EN_POS))
#define DVP_TSRC_LITE_CR_MODE_CEA_861                           DVP_TSRC_LITE_CR_MODE_CEA_861
#define DVP_TSRC_LITE_CR_MODE_CEA_861_POS                       (2U)
#define DVP_TSRC_LITE_CR_MODE_CEA_861_LEN                       (1U)
#define DVP_TSRC_LITE_CR_MODE_CEA_861_MSK                       (((1U<<DVP_TSRC_LITE_CR_MODE_CEA_861_LEN)-1)<<DVP_TSRC_LITE_CR_MODE_CEA_861_POS)
#define DVP_TSRC_LITE_CR_MODE_CEA_861_UMSK                      (~(((1U<<DVP_TSRC_LITE_CR_MODE_CEA_861_LEN)-1)<<DVP_TSRC_LITE_CR_MODE_CEA_861_POS))
#define DVP_TSRC_LITE_CR_AXI_PUSH_MODE                          DVP_TSRC_LITE_CR_AXI_PUSH_MODE
#define DVP_TSRC_LITE_CR_AXI_PUSH_MODE_POS                      (3U)
#define DVP_TSRC_LITE_CR_AXI_PUSH_MODE_LEN                      (1U)
#define DVP_TSRC_LITE_CR_AXI_PUSH_MODE_MSK                      (((1U<<DVP_TSRC_LITE_CR_AXI_PUSH_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_PUSH_MODE_POS)
#define DVP_TSRC_LITE_CR_AXI_PUSH_MODE_UMSK                     (~(((1U<<DVP_TSRC_LITE_CR_AXI_PUSH_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_PUSH_MODE_POS))
#define DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE                      DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE
#define DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_POS                  (16U)
#define DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_LEN                  (3U)
#define DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_MSK                  (((1U<<DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_POS)
#define DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_UMSK                 (~(((1U<<DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE_POS))
#define DVP_TSRC_LITE_CR_AXI_B0_SEL                             DVP_TSRC_LITE_CR_AXI_B0_SEL
#define DVP_TSRC_LITE_CR_AXI_B0_SEL_POS                         (20U)
#define DVP_TSRC_LITE_CR_AXI_B0_SEL_LEN                         (2U)
#define DVP_TSRC_LITE_CR_AXI_B0_SEL_MSK                         (((1U<<DVP_TSRC_LITE_CR_AXI_B0_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B0_SEL_POS)
#define DVP_TSRC_LITE_CR_AXI_B0_SEL_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_AXI_B0_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B0_SEL_POS))
#define DVP_TSRC_LITE_CR_AXI_B1_SEL                             DVP_TSRC_LITE_CR_AXI_B1_SEL
#define DVP_TSRC_LITE_CR_AXI_B1_SEL_POS                         (22U)
#define DVP_TSRC_LITE_CR_AXI_B1_SEL_LEN                         (2U)
#define DVP_TSRC_LITE_CR_AXI_B1_SEL_MSK                         (((1U<<DVP_TSRC_LITE_CR_AXI_B1_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B1_SEL_POS)
#define DVP_TSRC_LITE_CR_AXI_B1_SEL_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_AXI_B1_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B1_SEL_POS))
#define DVP_TSRC_LITE_CR_AXI_B2_SEL                             DVP_TSRC_LITE_CR_AXI_B2_SEL
#define DVP_TSRC_LITE_CR_AXI_B2_SEL_POS                         (24U)
#define DVP_TSRC_LITE_CR_AXI_B2_SEL_LEN                         (2U)
#define DVP_TSRC_LITE_CR_AXI_B2_SEL_MSK                         (((1U<<DVP_TSRC_LITE_CR_AXI_B2_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B2_SEL_POS)
#define DVP_TSRC_LITE_CR_AXI_B2_SEL_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_AXI_B2_SEL_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_B2_SEL_POS))

/* 0x4 : frame_size_h */
#define DVP_TSRC_LITE_FRAME_SIZE_H_OFFSET                       (0x4)
#define DVP_TSRC_LITE_CR_TOTAL_H                                DVP_TSRC_LITE_CR_TOTAL_H
#define DVP_TSRC_LITE_CR_TOTAL_H_POS                            (0U)
#define DVP_TSRC_LITE_CR_TOTAL_H_LEN                            (16U)
#define DVP_TSRC_LITE_CR_TOTAL_H_MSK                            (((1U<<DVP_TSRC_LITE_CR_TOTAL_H_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_H_POS)
#define DVP_TSRC_LITE_CR_TOTAL_H_UMSK                           (~(((1U<<DVP_TSRC_LITE_CR_TOTAL_H_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_H_POS))
#define DVP_TSRC_LITE_CR_BLANK_H                                DVP_TSRC_LITE_CR_BLANK_H
#define DVP_TSRC_LITE_CR_BLANK_H_POS                            (16U)
#define DVP_TSRC_LITE_CR_BLANK_H_LEN                            (16U)
#define DVP_TSRC_LITE_CR_BLANK_H_MSK                            (((1U<<DVP_TSRC_LITE_CR_BLANK_H_LEN)-1)<<DVP_TSRC_LITE_CR_BLANK_H_POS)
#define DVP_TSRC_LITE_CR_BLANK_H_UMSK                           (~(((1U<<DVP_TSRC_LITE_CR_BLANK_H_LEN)-1)<<DVP_TSRC_LITE_CR_BLANK_H_POS))

/* 0x8 : frame_size_v */
#define DVP_TSRC_LITE_FRAME_SIZE_V_OFFSET                       (0x8)
#define DVP_TSRC_LITE_CR_TOTAL_V                                DVP_TSRC_LITE_CR_TOTAL_V
#define DVP_TSRC_LITE_CR_TOTAL_V_POS                            (0U)
#define DVP_TSRC_LITE_CR_TOTAL_V_LEN                            (16U)
#define DVP_TSRC_LITE_CR_TOTAL_V_MSK                            (((1U<<DVP_TSRC_LITE_CR_TOTAL_V_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_V_POS)
#define DVP_TSRC_LITE_CR_TOTAL_V_UMSK                           (~(((1U<<DVP_TSRC_LITE_CR_TOTAL_V_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_V_POS))
#define DVP_TSRC_LITE_CR_BLANK_V                                DVP_TSRC_LITE_CR_BLANK_V
#define DVP_TSRC_LITE_CR_BLANK_V_POS                            (16U)
#define DVP_TSRC_LITE_CR_BLANK_V_LEN                            (16U)
#define DVP_TSRC_LITE_CR_BLANK_V_MSK                            (((1U<<DVP_TSRC_LITE_CR_BLANK_V_LEN)-1)<<DVP_TSRC_LITE_CR_BLANK_V_POS)
#define DVP_TSRC_LITE_CR_BLANK_V_UMSK                           (~(((1U<<DVP_TSRC_LITE_CR_BLANK_V_LEN)-1)<<DVP_TSRC_LITE_CR_BLANK_V_POS))

/* 0xC : frame_size_cea_861 */
#define DVP_TSRC_LITE_FRAME_SIZE_CEA_861_OFFSET                 (0xC)
#define DVP_TSRC_LITE_CR_H_DURATION                             DVP_TSRC_LITE_CR_H_DURATION
#define DVP_TSRC_LITE_CR_H_DURATION_POS                         (0U)
#define DVP_TSRC_LITE_CR_H_DURATION_LEN                         (8U)
#define DVP_TSRC_LITE_CR_H_DURATION_MSK                         (((1U<<DVP_TSRC_LITE_CR_H_DURATION_LEN)-1)<<DVP_TSRC_LITE_CR_H_DURATION_POS)
#define DVP_TSRC_LITE_CR_H_DURATION_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_H_DURATION_LEN)-1)<<DVP_TSRC_LITE_CR_H_DURATION_POS))
#define DVP_TSRC_LITE_CR_H_PLACEMENT                            DVP_TSRC_LITE_CR_H_PLACEMENT
#define DVP_TSRC_LITE_CR_H_PLACEMENT_POS                        (8U)
#define DVP_TSRC_LITE_CR_H_PLACEMENT_LEN                        (8U)
#define DVP_TSRC_LITE_CR_H_PLACEMENT_MSK                        (((1U<<DVP_TSRC_LITE_CR_H_PLACEMENT_LEN)-1)<<DVP_TSRC_LITE_CR_H_PLACEMENT_POS)
#define DVP_TSRC_LITE_CR_H_PLACEMENT_UMSK                       (~(((1U<<DVP_TSRC_LITE_CR_H_PLACEMENT_LEN)-1)<<DVP_TSRC_LITE_CR_H_PLACEMENT_POS))
#define DVP_TSRC_LITE_CR_V_DURATION                             DVP_TSRC_LITE_CR_V_DURATION
#define DVP_TSRC_LITE_CR_V_DURATION_POS                         (16U)
#define DVP_TSRC_LITE_CR_V_DURATION_LEN                         (8U)
#define DVP_TSRC_LITE_CR_V_DURATION_MSK                         (((1U<<DVP_TSRC_LITE_CR_V_DURATION_LEN)-1)<<DVP_TSRC_LITE_CR_V_DURATION_POS)
#define DVP_TSRC_LITE_CR_V_DURATION_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_V_DURATION_LEN)-1)<<DVP_TSRC_LITE_CR_V_DURATION_POS))
#define DVP_TSRC_LITE_CR_V_PLACEMENT                            DVP_TSRC_LITE_CR_V_PLACEMENT
#define DVP_TSRC_LITE_CR_V_PLACEMENT_POS                        (24U)
#define DVP_TSRC_LITE_CR_V_PLACEMENT_LEN                        (8U)
#define DVP_TSRC_LITE_CR_V_PLACEMENT_MSK                        (((1U<<DVP_TSRC_LITE_CR_V_PLACEMENT_LEN)-1)<<DVP_TSRC_LITE_CR_V_PLACEMENT_POS)
#define DVP_TSRC_LITE_CR_V_PLACEMENT_UMSK                       (~(((1U<<DVP_TSRC_LITE_CR_V_PLACEMENT_LEN)-1)<<DVP_TSRC_LITE_CR_V_PLACEMENT_POS))

/* 0x10 : pix_data_range */
#define DVP_TSRC_LITE_PIX_DATA_RANGE_OFFSET                     (0x10)
#define DVP_TSRC_LITE_CR_DATA_MIN                               DVP_TSRC_LITE_CR_DATA_MIN
#define DVP_TSRC_LITE_CR_DATA_MIN_POS                           (0U)
#define DVP_TSRC_LITE_CR_DATA_MIN_LEN                           (16U)
#define DVP_TSRC_LITE_CR_DATA_MIN_MSK                           (((1U<<DVP_TSRC_LITE_CR_DATA_MIN_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_MIN_POS)
#define DVP_TSRC_LITE_CR_DATA_MIN_UMSK                          (~(((1U<<DVP_TSRC_LITE_CR_DATA_MIN_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_MIN_POS))
#define DVP_TSRC_LITE_CR_DATA_MAX                               DVP_TSRC_LITE_CR_DATA_MAX
#define DVP_TSRC_LITE_CR_DATA_MAX_POS                           (16U)
#define DVP_TSRC_LITE_CR_DATA_MAX_LEN                           (16U)
#define DVP_TSRC_LITE_CR_DATA_MAX_MSK                           (((1U<<DVP_TSRC_LITE_CR_DATA_MAX_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_MAX_POS)
#define DVP_TSRC_LITE_CR_DATA_MAX_UMSK                          (~(((1U<<DVP_TSRC_LITE_CR_DATA_MAX_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_MAX_POS))

/* 0x14 : pix_data_step */
#define DVP_TSRC_LITE_PIX_DATA_STEP_OFFSET                      (0x14)
#define DVP_TSRC_LITE_CR_DATA_STEP                              DVP_TSRC_LITE_CR_DATA_STEP
#define DVP_TSRC_LITE_CR_DATA_STEP_POS                          (0U)
#define DVP_TSRC_LITE_CR_DATA_STEP_LEN                          (8U)
#define DVP_TSRC_LITE_CR_DATA_STEP_MSK                          (((1U<<DVP_TSRC_LITE_CR_DATA_STEP_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_STEP_POS)
#define DVP_TSRC_LITE_CR_DATA_STEP_UMSK                         (~(((1U<<DVP_TSRC_LITE_CR_DATA_STEP_LEN)-1)<<DVP_TSRC_LITE_CR_DATA_STEP_POS))

/* 0x18 : axi_push_mode */
#define DVP_TSRC_LITE_AXI_PUSH_MODE_OFFSET                      (0x18)
#define DVP_TSRC_LITE_CR_SW_SHD                                 DVP_TSRC_LITE_CR_SW_SHD
#define DVP_TSRC_LITE_CR_SW_SHD_POS                             (0U)
#define DVP_TSRC_LITE_CR_SW_SHD_LEN                             (1U)
#define DVP_TSRC_LITE_CR_SW_SHD_MSK                             (((1U<<DVP_TSRC_LITE_CR_SW_SHD_LEN)-1)<<DVP_TSRC_LITE_CR_SW_SHD_POS)
#define DVP_TSRC_LITE_CR_SW_SHD_UMSK                            (~(((1U<<DVP_TSRC_LITE_CR_SW_SHD_LEN)-1)<<DVP_TSRC_LITE_CR_SW_SHD_POS))
#define DVP_TSRC_LITE_CR_AXI_FRAME_PUSH                         DVP_TSRC_LITE_CR_AXI_FRAME_PUSH
#define DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_POS                     (3U)
#define DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_LEN                     (1U)
#define DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_MSK                     (((1U<<DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_POS)
#define DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_UMSK                    (~(((1U<<DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_FRAME_PUSH_POS))
#define DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR                      DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR
#define DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_POS                  (4U)
#define DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_LEN                  (1U)
#define DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_MSK                  (((1U<<DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_POS)
#define DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_UMSK                 (~(((1U<<DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR_POS))
#define DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT                   DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT
#define DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_POS               (8U)
#define DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_LEN               (5U)
#define DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_MSK               (((1U<<DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_LEN)-1)<<DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_POS)
#define DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_UMSK              (~(((1U<<DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_LEN)-1)<<DVP_TSRC_LITE_ST_AXI_FRAME_REMAIN_CNT_POS))

/* 0x20 : axi2dvp_setting */
#define DVP_TSRC_LITE_AXI2DVP_SETTING_OFFSET                    (0x20)
#define DVP_TSRC_LITE_CR_AXI_XLEN                               DVP_TSRC_LITE_CR_AXI_XLEN
#define DVP_TSRC_LITE_CR_AXI_XLEN_POS                           (0U)
#define DVP_TSRC_LITE_CR_AXI_XLEN_LEN                           (3U)
#define DVP_TSRC_LITE_CR_AXI_XLEN_MSK                           (((1U<<DVP_TSRC_LITE_CR_AXI_XLEN_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_XLEN_POS)
#define DVP_TSRC_LITE_CR_AXI_XLEN_UMSK                          (~(((1U<<DVP_TSRC_LITE_CR_AXI_XLEN_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_XLEN_POS))
#define DVP_TSRC_LITE_CR_QOS_SW_MODE                            DVP_TSRC_LITE_CR_QOS_SW_MODE
#define DVP_TSRC_LITE_CR_QOS_SW_MODE_POS                        (10U)
#define DVP_TSRC_LITE_CR_QOS_SW_MODE_LEN                        (1U)
#define DVP_TSRC_LITE_CR_QOS_SW_MODE_MSK                        (((1U<<DVP_TSRC_LITE_CR_QOS_SW_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_QOS_SW_MODE_POS)
#define DVP_TSRC_LITE_CR_QOS_SW_MODE_UMSK                       (~(((1U<<DVP_TSRC_LITE_CR_QOS_SW_MODE_LEN)-1)<<DVP_TSRC_LITE_CR_QOS_SW_MODE_POS))
#define DVP_TSRC_LITE_CR_QOS_SW                                 DVP_TSRC_LITE_CR_QOS_SW
#define DVP_TSRC_LITE_CR_QOS_SW_POS                             (11U)
#define DVP_TSRC_LITE_CR_QOS_SW_LEN                             (1U)
#define DVP_TSRC_LITE_CR_QOS_SW_MSK                             (((1U<<DVP_TSRC_LITE_CR_QOS_SW_LEN)-1)<<DVP_TSRC_LITE_CR_QOS_SW_POS)
#define DVP_TSRC_LITE_CR_QOS_SW_UMSK                            (~(((1U<<DVP_TSRC_LITE_CR_QOS_SW_LEN)-1)<<DVP_TSRC_LITE_CR_QOS_SW_POS))

/* 0x24 : axi2dvp_start_addr_by */
#define DVP_TSRC_LITE_AXI2DVP_START_ADDR_BY_OFFSET              (0x24)
#define DVP_TSRC_LITE_CR_AXI_START_ADDR_BY                      DVP_TSRC_LITE_CR_AXI_START_ADDR_BY
#define DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_POS                  (0U)
#define DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_LEN                  (32U)
#define DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_MSK                  (((1U<<DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_POS)
#define DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_UMSK                 (~(((1U<<DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_START_ADDR_BY_POS))

/* 0x28 : axi2dvp_frame_info1 */
#define DVP_TSRC_LITE_AXI2DVP_FRAME_INFO1_OFFSET                (0x28)
#define DVP_TSRC_LITE_CR_FRAME_WIDTH_X8                         DVP_TSRC_LITE_CR_FRAME_WIDTH_X8
#define DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_POS                     (0U)
#define DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_LEN                     (14U)
#define DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_MSK                     (((1U<<DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_LEN)-1)<<DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_POS)
#define DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_UMSK                    (~(((1U<<DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_LEN)-1)<<DVP_TSRC_LITE_CR_FRAME_WIDTH_X8_POS))
#define DVP_TSRC_LITE_CR_FRAME_HEIGHT                           DVP_TSRC_LITE_CR_FRAME_HEIGHT
#define DVP_TSRC_LITE_CR_FRAME_HEIGHT_POS                       (16U)
#define DVP_TSRC_LITE_CR_FRAME_HEIGHT_LEN                       (14U)
#define DVP_TSRC_LITE_CR_FRAME_HEIGHT_MSK                       (((1U<<DVP_TSRC_LITE_CR_FRAME_HEIGHT_LEN)-1)<<DVP_TSRC_LITE_CR_FRAME_HEIGHT_POS)
#define DVP_TSRC_LITE_CR_FRAME_HEIGHT_UMSK                      (~(((1U<<DVP_TSRC_LITE_CR_FRAME_HEIGHT_LEN)-1)<<DVP_TSRC_LITE_CR_FRAME_HEIGHT_POS))

/* 0x30 : axi2dvp_swap_addr_by */
#define DVP_TSRC_LITE_AXI2DVP_SWAP_ADDR_BY_OFFSET               (0x30)
#define DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY                       DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY
#define DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_POS                   (0U)
#define DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_LEN                   (32U)
#define DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_MSK                   (((1U<<DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_POS)
#define DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_UMSK                  (~(((1U<<DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_LEN)-1)<<DVP_TSRC_LITE_CR_AXI_SWAP_ADDR_BY_POS))

/* 0x34 : axi2dvp_prefetch */
#define DVP_TSRC_LITE_AXI2DVP_PREFETCH_OFFSET                   (0x34)
#define DVP_TSRC_LITE_CR_PREFETCH_V                             DVP_TSRC_LITE_CR_PREFETCH_V
#define DVP_TSRC_LITE_CR_PREFETCH_V_POS                         (0U)
#define DVP_TSRC_LITE_CR_PREFETCH_V_LEN                         (16U)
#define DVP_TSRC_LITE_CR_PREFETCH_V_MSK                         (((1U<<DVP_TSRC_LITE_CR_PREFETCH_V_LEN)-1)<<DVP_TSRC_LITE_CR_PREFETCH_V_POS)
#define DVP_TSRC_LITE_CR_PREFETCH_V_UMSK                        (~(((1U<<DVP_TSRC_LITE_CR_PREFETCH_V_LEN)-1)<<DVP_TSRC_LITE_CR_PREFETCH_V_POS))

/* 0x38 : axi2dvp_frame_info2 */
#define DVP_TSRC_LITE_AXI2DVP_FRAME_INFO2_OFFSET                (0x38)
#define DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8                        DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8
#define DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_POS                    (0U)
#define DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_LEN                    (14U)
#define DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_MSK                    (((1U<<DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_LEN)-1)<<DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_POS)
#define DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_UMSK                   (~(((1U<<DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_LEN)-1)<<DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8_POS))

/* 0x80 : dtsrc_post_tg0 */
#define DVP_TSRC_LITE_DTSRC_POST_TG0_OFFSET                     (0x80)
#define DVP_TSRC_LITE_CR_TOTAL_HPIXELS                          DVP_TSRC_LITE_CR_TOTAL_HPIXELS
#define DVP_TSRC_LITE_CR_TOTAL_HPIXELS_POS                      (0U)
#define DVP_TSRC_LITE_CR_TOTAL_HPIXELS_LEN                      (16U)
#define DVP_TSRC_LITE_CR_TOTAL_HPIXELS_MSK                      (((1U<<DVP_TSRC_LITE_CR_TOTAL_HPIXELS_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_HPIXELS_POS)
#define DVP_TSRC_LITE_CR_TOTAL_HPIXELS_UMSK                     (~(((1U<<DVP_TSRC_LITE_CR_TOTAL_HPIXELS_LEN)-1)<<DVP_TSRC_LITE_CR_TOTAL_HPIXELS_POS))
#define DVP_TSRC_LITE_CR_TG_EN                                  DVP_TSRC_LITE_CR_TG_EN
#define DVP_TSRC_LITE_CR_TG_EN_POS                              (16U)
#define DVP_TSRC_LITE_CR_TG_EN_LEN                              (1U)
#define DVP_TSRC_LITE_CR_TG_EN_MSK                              (((1U<<DVP_TSRC_LITE_CR_TG_EN_LEN)-1)<<DVP_TSRC_LITE_CR_TG_EN_POS)
#define DVP_TSRC_LITE_CR_TG_EN_UMSK                             (~(((1U<<DVP_TSRC_LITE_CR_TG_EN_LEN)-1)<<DVP_TSRC_LITE_CR_TG_EN_POS))

/* 0x84 : dtsrc_post_tg1 */
#define DVP_TSRC_LITE_DTSRC_POST_TG1_OFFSET                     (0x84)
#define DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH                        DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH
#define DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_POS                    (0U)
#define DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_LEN                    (16U)
#define DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_MSK                    (((1U<<DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_POS)
#define DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_UMSK                   (~(((1U<<DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH_POS))
#define DVP_TSRC_LITE_CR_ACT_START_PIXEL                        DVP_TSRC_LITE_CR_ACT_START_PIXEL
#define DVP_TSRC_LITE_CR_ACT_START_PIXEL_POS                    (16U)
#define DVP_TSRC_LITE_CR_ACT_START_PIXEL_LEN                    (16U)
#define DVP_TSRC_LITE_CR_ACT_START_PIXEL_MSK                    (((1U<<DVP_TSRC_LITE_CR_ACT_START_PIXEL_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_START_PIXEL_POS)
#define DVP_TSRC_LITE_CR_ACT_START_PIXEL_UMSK                   (~(((1U<<DVP_TSRC_LITE_CR_ACT_START_PIXEL_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_START_PIXEL_POS))

/* 0x88 : dtsrc_post_tg2 */
#define DVP_TSRC_LITE_DTSRC_POST_TG2_OFFSET                     (0x88)
#define DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT                        DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT
#define DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_POS                    (0U)
#define DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_LEN                    (16U)
#define DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_MSK                    (((1U<<DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_POS)
#define DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_UMSK                   (~(((1U<<DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT_POS))
#define DVP_TSRC_LITE_CR_ACT_START_LINE                         DVP_TSRC_LITE_CR_ACT_START_LINE
#define DVP_TSRC_LITE_CR_ACT_START_LINE_POS                     (16U)
#define DVP_TSRC_LITE_CR_ACT_START_LINE_LEN                     (16U)
#define DVP_TSRC_LITE_CR_ACT_START_LINE_MSK                     (((1U<<DVP_TSRC_LITE_CR_ACT_START_LINE_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_START_LINE_POS)
#define DVP_TSRC_LITE_CR_ACT_START_LINE_UMSK                    (~(((1U<<DVP_TSRC_LITE_CR_ACT_START_LINE_LEN)-1)<<DVP_TSRC_LITE_CR_ACT_START_LINE_POS))

/* 0x90 : dtsrc_crop0 */
#define DVP_TSRC_LITE_DTSRC_CROP0_OFFSET                        (0x90)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_START                       DVP_TSRC_LITE_REG_HSYNC_ACT_START
#define DVP_TSRC_LITE_REG_HSYNC_ACT_START_POS                   (0U)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_START_LEN                   (14U)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_START_MSK                   (((1U<<DVP_TSRC_LITE_REG_HSYNC_ACT_START_LEN)-1)<<DVP_TSRC_LITE_REG_HSYNC_ACT_START_POS)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_START_UMSK                  (~(((1U<<DVP_TSRC_LITE_REG_HSYNC_ACT_START_LEN)-1)<<DVP_TSRC_LITE_REG_HSYNC_ACT_START_POS))
#define DVP_TSRC_LITE_REG_HSYNC_ACT_END                         DVP_TSRC_LITE_REG_HSYNC_ACT_END
#define DVP_TSRC_LITE_REG_HSYNC_ACT_END_POS                     (16U)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_END_LEN                     (14U)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_END_MSK                     (((1U<<DVP_TSRC_LITE_REG_HSYNC_ACT_END_LEN)-1)<<DVP_TSRC_LITE_REG_HSYNC_ACT_END_POS)
#define DVP_TSRC_LITE_REG_HSYNC_ACT_END_UMSK                    (~(((1U<<DVP_TSRC_LITE_REG_HSYNC_ACT_END_LEN)-1)<<DVP_TSRC_LITE_REG_HSYNC_ACT_END_POS))
#define DVP_TSRC_LITE_REG_CROP_ENABLE                           DVP_TSRC_LITE_REG_CROP_ENABLE
#define DVP_TSRC_LITE_REG_CROP_ENABLE_POS                       (31U)
#define DVP_TSRC_LITE_REG_CROP_ENABLE_LEN                       (1U)
#define DVP_TSRC_LITE_REG_CROP_ENABLE_MSK                       (((1U<<DVP_TSRC_LITE_REG_CROP_ENABLE_LEN)-1)<<DVP_TSRC_LITE_REG_CROP_ENABLE_POS)
#define DVP_TSRC_LITE_REG_CROP_ENABLE_UMSK                      (~(((1U<<DVP_TSRC_LITE_REG_CROP_ENABLE_LEN)-1)<<DVP_TSRC_LITE_REG_CROP_ENABLE_POS))

/* 0x94 : dtsrc_crop1 */
#define DVP_TSRC_LITE_DTSRC_CROP1_OFFSET                        (0x94)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_START                       DVP_TSRC_LITE_REG_VSYNC_ACT_START
#define DVP_TSRC_LITE_REG_VSYNC_ACT_START_POS                   (0U)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_START_LEN                   (14U)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_START_MSK                   (((1U<<DVP_TSRC_LITE_REG_VSYNC_ACT_START_LEN)-1)<<DVP_TSRC_LITE_REG_VSYNC_ACT_START_POS)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_START_UMSK                  (~(((1U<<DVP_TSRC_LITE_REG_VSYNC_ACT_START_LEN)-1)<<DVP_TSRC_LITE_REG_VSYNC_ACT_START_POS))
#define DVP_TSRC_LITE_REG_VSYNC_ACT_END                         DVP_TSRC_LITE_REG_VSYNC_ACT_END
#define DVP_TSRC_LITE_REG_VSYNC_ACT_END_POS                     (16U)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_END_LEN                     (14U)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_END_MSK                     (((1U<<DVP_TSRC_LITE_REG_VSYNC_ACT_END_LEN)-1)<<DVP_TSRC_LITE_REG_VSYNC_ACT_END_POS)
#define DVP_TSRC_LITE_REG_VSYNC_ACT_END_UMSK                    (~(((1U<<DVP_TSRC_LITE_REG_VSYNC_ACT_END_LEN)-1)<<DVP_TSRC_LITE_REG_VSYNC_ACT_END_POS))


struct  dvp_tsrc_lite_reg {
    /* 0x0 : config */
    union {
        struct {
            uint32_t cr_enable                      :  1; /* [    0],        r/w,        0x0 */
            uint32_t cr_axi_en                      :  1; /* [    1],        r/w,        0x0 */
            uint32_t cr_mode_cea_861                :  1; /* [    2],        r/w,        0x0 */
            uint32_t cr_axi_push_mode               :  1; /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15                  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t cr_axi_dvp_data_mode           :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reserved_19                    :  1; /* [   19],       rsvd,        0x0 */
            uint32_t cr_axi_b0_sel                  :  2; /* [21:20],        r/w,        0x0 */
            uint32_t cr_axi_b1_sel                  :  2; /* [23:22],        r/w,        0x1 */
            uint32_t cr_axi_b2_sel                  :  2; /* [25:24],        r/w,        0x2 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } config;

    /* 0x4 : frame_size_h */
    union {
        struct {
            uint32_t cr_total_h                     : 16; /* [15: 0],        r/w,      0x897 */
            uint32_t cr_blank_h                     : 16; /* [31:16],        r/w,      0x117 */
        }BF;
        uint32_t WORD;
    } frame_size_h;

    /* 0x8 : frame_size_v */
    union {
        struct {
            uint32_t cr_total_v                     : 16; /* [15: 0],        r/w,      0x464 */
            uint32_t cr_blank_v                     : 16; /* [31:16],        r/w,       0x2c */
        }BF;
        uint32_t WORD;
    } frame_size_v;

    /* 0xC : frame_size_cea_861 */
    union {
        struct {
            uint32_t cr_h_duration                  :  8; /* [ 7: 0],        r/w,       0x83 */
            uint32_t cr_h_placement                 :  8; /* [15: 8],        r/w,       0x57 */
            uint32_t cr_v_duration                  :  8; /* [23:16],        r/w,        0x8 */
            uint32_t cr_v_placement                 :  8; /* [31:24],        r/w,        0x3 */
        }BF;
        uint32_t WORD;
    } frame_size_cea_861;

    /* 0x10 : pix_data_range */
    union {
        struct {
            uint32_t cr_data_min                    : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_data_max                    : 16; /* [31:16],        r/w,     0xffff */
        }BF;
        uint32_t WORD;
    } pix_data_range;

    /* 0x14 : pix_data_step */
    union {
        struct {
            uint32_t cr_data_step                   :  8; /* [ 7: 0],        r/w,        0x1 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } pix_data_step;

    /* 0x18 : axi_push_mode */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1_2                   :  2; /* [ 2: 1],       rsvd,        0x0 */
            uint32_t cr_axi_frame_push              :  1; /* [    3],        w1p,        0x0 */
            uint32_t cr_axi_drain_err_clr           :  1; /* [    4],        w1p,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t st_axi_frame_remain_cnt        :  5; /* [12: 8],          r,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } axi_push_mode;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[4];

    /* 0x20 : axi2dvp_setting */
    union {
        struct {
            uint32_t cr_axi_xlen                    :  3; /* [ 2: 0],        r/w,        0x3 */
            uint32_t reserved_3_9                   :  7; /* [ 9: 3],       rsvd,        0x0 */
            uint32_t cr_qos_sw_mode                 :  1; /* [   10],        r/w,        0x0 */
            uint32_t cr_qos_sw                      :  1; /* [   11],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_setting;

    /* 0x24 : axi2dvp_start_addr_by */
    union {
        struct {
            uint32_t cr_axi_start_addr_by           : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_start_addr_by;

    /* 0x28 : axi2dvp_frame_info1 */
    union {
        struct {
            uint32_t cr_frame_width_x8              : 14; /* [13: 0],        r/w,      0x120 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_frame_height                : 14; /* [29:16],        r/w,      0x510 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_frame_info1;

    /* 0x2c  reserved */
    uint8_t RESERVED0x2c[4];

    /* 0x30 : axi2dvp_swap_addr_by */
    union {
        struct {
            uint32_t cr_axi_swap_addr_by            : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_swap_addr_by;

    /* 0x34 : axi2dvp_prefetch */
    union {
        struct {
            uint32_t cr_prefetch_v                  : 16; /* [15: 0],        r/w,       0x28 */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_prefetch;

    /* 0x38 : axi2dvp_frame_info2 */
    union {
        struct {
            uint32_t cr_stride_width_x8             : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_31                 : 18; /* [31:14],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } axi2dvp_frame_info2;

    /* 0x3c  reserved */
    uint8_t RESERVED0x3c[68];

    /* 0x80 : dtsrc_post_tg0 */
    union {
        struct {
            uint32_t cr_total_hpixels               : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_tg_en                       :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dtsrc_post_tg0;

    /* 0x84 : dtsrc_post_tg1 */
    union {
        struct {
            uint32_t cr_act_pixel_width             : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_act_start_pixel             : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dtsrc_post_tg1;

    /* 0x88 : dtsrc_post_tg2 */
    union {
        struct {
            uint32_t cr_act_line_height             : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t cr_act_start_line              : 16; /* [31:16],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dtsrc_post_tg2;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[4];

    /* 0x90 : dtsrc_crop0 */
    union {
        struct {
            uint32_t reg_hsync_act_start            : 14; /* [13: 0],        r/w,     0x3fff */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_hsync_act_end              : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30                    :  1; /* [   30],       rsvd,        0x0 */
            uint32_t reg_crop_enable                :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } dtsrc_crop0;

    /* 0x94 : dtsrc_crop1 */
    union {
        struct {
            uint32_t reg_vsync_act_start            : 14; /* [13: 0],        r/w,     0x3fff */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_vsync_act_end              : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dtsrc_crop1;

};

typedef volatile struct dvp_tsrc_lite_reg dvp_tsrc_lite_reg_t;


#endif  /* __DVP_TSRC_LITE_REG_H__ */
