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

#ifndef  __DVP2AXI_REG_H__
#define  __DVP2AXI_REG_H__

#include "ipro6.h"

/* 0x0 : dvp2axi_configue */
#define DVP2AXI_CONFIGUE_OFFSET                                 (0x0)
#define DVP2AXI_REG_DVP_ENABLE                                  DVP2AXI_REG_DVP_ENABLE
#define DVP2AXI_REG_DVP_ENABLE_POS                              (0U)
#define DVP2AXI_REG_DVP_ENABLE_LEN                              (1U)
#define DVP2AXI_REG_DVP_ENABLE_MSK                              (((1U<<DVP2AXI_REG_DVP_ENABLE_LEN)-1)<<DVP2AXI_REG_DVP_ENABLE_POS)
#define DVP2AXI_REG_DVP_ENABLE_UMSK                             (~(((1U<<DVP2AXI_REG_DVP_ENABLE_LEN)-1)<<DVP2AXI_REG_DVP_ENABLE_POS))
#define DVP2AXI_REG_WRAP_MODE                                   DVP2AXI_REG_WRAP_MODE
#define DVP2AXI_REG_WRAP_MODE_POS                               (1U)
#define DVP2AXI_REG_WRAP_MODE_LEN                               (1U)
#define DVP2AXI_REG_WRAP_MODE_MSK                               (((1U<<DVP2AXI_REG_WRAP_MODE_LEN)-1)<<DVP2AXI_REG_WRAP_MODE_POS)
#define DVP2AXI_REG_WRAP_MODE_UMSK                              (~(((1U<<DVP2AXI_REG_WRAP_MODE_LEN)-1)<<DVP2AXI_REG_WRAP_MODE_POS))
#define DVP2AXI_REG_FRAM_VLD_POL                                DVP2AXI_REG_FRAM_VLD_POL
#define DVP2AXI_REG_FRAM_VLD_POL_POS                            (2U)
#define DVP2AXI_REG_FRAM_VLD_POL_LEN                            (1U)
#define DVP2AXI_REG_FRAM_VLD_POL_MSK                            (((1U<<DVP2AXI_REG_FRAM_VLD_POL_LEN)-1)<<DVP2AXI_REG_FRAM_VLD_POL_POS)
#define DVP2AXI_REG_FRAM_VLD_POL_UMSK                           (~(((1U<<DVP2AXI_REG_FRAM_VLD_POL_LEN)-1)<<DVP2AXI_REG_FRAM_VLD_POL_POS))
#define DVP2AXI_REG_LINE_VLD_POL                                DVP2AXI_REG_LINE_VLD_POL
#define DVP2AXI_REG_LINE_VLD_POL_POS                            (3U)
#define DVP2AXI_REG_LINE_VLD_POL_LEN                            (1U)
#define DVP2AXI_REG_LINE_VLD_POL_MSK                            (((1U<<DVP2AXI_REG_LINE_VLD_POL_LEN)-1)<<DVP2AXI_REG_LINE_VLD_POL_POS)
#define DVP2AXI_REG_LINE_VLD_POL_UMSK                           (~(((1U<<DVP2AXI_REG_LINE_VLD_POL_LEN)-1)<<DVP2AXI_REG_LINE_VLD_POL_POS))
#define DVP2AXI_REG_XLEN                                        DVP2AXI_REG_XLEN
#define DVP2AXI_REG_XLEN_POS                                    (4U)
#define DVP2AXI_REG_XLEN_LEN                                    (3U)
#define DVP2AXI_REG_XLEN_MSK                                    (((1U<<DVP2AXI_REG_XLEN_LEN)-1)<<DVP2AXI_REG_XLEN_POS)
#define DVP2AXI_REG_XLEN_UMSK                                   (~(((1U<<DVP2AXI_REG_XLEN_LEN)-1)<<DVP2AXI_REG_XLEN_POS))
#define DVP2AXI_REG_FLIP_MODE                                   DVP2AXI_REG_FLIP_MODE
#define DVP2AXI_REG_FLIP_MODE_POS                               (7U)
#define DVP2AXI_REG_FLIP_MODE_LEN                               (1U)
#define DVP2AXI_REG_FLIP_MODE_MSK                               (((1U<<DVP2AXI_REG_FLIP_MODE_LEN)-1)<<DVP2AXI_REG_FLIP_MODE_POS)
#define DVP2AXI_REG_FLIP_MODE_UMSK                              (~(((1U<<DVP2AXI_REG_FLIP_MODE_LEN)-1)<<DVP2AXI_REG_FLIP_MODE_POS))
#define DVP2AXI_REG_DVP_MODE                                    DVP2AXI_REG_DVP_MODE
#define DVP2AXI_REG_DVP_MODE_POS                                (8U)
#define DVP2AXI_REG_DVP_MODE_LEN                                (3U)
#define DVP2AXI_REG_DVP_MODE_MSK                                (((1U<<DVP2AXI_REG_DVP_MODE_LEN)-1)<<DVP2AXI_REG_DVP_MODE_POS)
#define DVP2AXI_REG_DVP_MODE_UMSK                               (~(((1U<<DVP2AXI_REG_DVP_MODE_LEN)-1)<<DVP2AXI_REG_DVP_MODE_POS))
#define DVP2AXI_REG_WRAP_ADDR_START                             DVP2AXI_REG_WRAP_ADDR_START
#define DVP2AXI_REG_WRAP_ADDR_START_POS                         (11U)
#define DVP2AXI_REG_WRAP_ADDR_START_LEN                         (1U)
#define DVP2AXI_REG_WRAP_ADDR_START_MSK                         (((1U<<DVP2AXI_REG_WRAP_ADDR_START_LEN)-1)<<DVP2AXI_REG_WRAP_ADDR_START_POS)
#define DVP2AXI_REG_WRAP_ADDR_START_UMSK                        (~(((1U<<DVP2AXI_REG_WRAP_ADDR_START_LEN)-1)<<DVP2AXI_REG_WRAP_ADDR_START_POS))
#define DVP2AXI_REG_DROP_EN                                     DVP2AXI_REG_DROP_EN
#define DVP2AXI_REG_DROP_EN_POS                                 (12U)
#define DVP2AXI_REG_DROP_EN_LEN                                 (1U)
#define DVP2AXI_REG_DROP_EN_MSK                                 (((1U<<DVP2AXI_REG_DROP_EN_LEN)-1)<<DVP2AXI_REG_DROP_EN_POS)
#define DVP2AXI_REG_DROP_EN_UMSK                                (~(((1U<<DVP2AXI_REG_DROP_EN_LEN)-1)<<DVP2AXI_REG_DROP_EN_POS))
#define DVP2AXI_REG_DROP_EVEN                                   DVP2AXI_REG_DROP_EVEN
#define DVP2AXI_REG_DROP_EVEN_POS                               (13U)
#define DVP2AXI_REG_DROP_EVEN_LEN                               (1U)
#define DVP2AXI_REG_DROP_EVEN_MSK                               (((1U<<DVP2AXI_REG_DROP_EVEN_LEN)-1)<<DVP2AXI_REG_DROP_EVEN_POS)
#define DVP2AXI_REG_DROP_EVEN_UMSK                              (~(((1U<<DVP2AXI_REG_DROP_EVEN_LEN)-1)<<DVP2AXI_REG_DROP_EVEN_POS))
#define DVP2AXI_REG_QOS_SW_MODE                                 DVP2AXI_REG_QOS_SW_MODE
#define DVP2AXI_REG_QOS_SW_MODE_POS                             (14U)
#define DVP2AXI_REG_QOS_SW_MODE_LEN                             (1U)
#define DVP2AXI_REG_QOS_SW_MODE_MSK                             (((1U<<DVP2AXI_REG_QOS_SW_MODE_LEN)-1)<<DVP2AXI_REG_QOS_SW_MODE_POS)
#define DVP2AXI_REG_QOS_SW_MODE_UMSK                            (~(((1U<<DVP2AXI_REG_QOS_SW_MODE_LEN)-1)<<DVP2AXI_REG_QOS_SW_MODE_POS))
#define DVP2AXI_REG_QOS_SW                                      DVP2AXI_REG_QOS_SW
#define DVP2AXI_REG_QOS_SW_POS                                  (15U)
#define DVP2AXI_REG_QOS_SW_LEN                                  (1U)
#define DVP2AXI_REG_QOS_SW_MSK                                  (((1U<<DVP2AXI_REG_QOS_SW_LEN)-1)<<DVP2AXI_REG_QOS_SW_POS)
#define DVP2AXI_REG_QOS_SW_UMSK                                 (~(((1U<<DVP2AXI_REG_QOS_SW_LEN)-1)<<DVP2AXI_REG_QOS_SW_POS))
#define DVP2AXI_REG_DVP_DATA_MODE                               DVP2AXI_REG_DVP_DATA_MODE
#define DVP2AXI_REG_DVP_DATA_MODE_POS                           (16U)
#define DVP2AXI_REG_DVP_DATA_MODE_LEN                           (3U)
#define DVP2AXI_REG_DVP_DATA_MODE_MSK                           (((1U<<DVP2AXI_REG_DVP_DATA_MODE_LEN)-1)<<DVP2AXI_REG_DVP_DATA_MODE_POS)
#define DVP2AXI_REG_DVP_DATA_MODE_UMSK                          (~(((1U<<DVP2AXI_REG_DVP_DATA_MODE_LEN)-1)<<DVP2AXI_REG_DVP_DATA_MODE_POS))
#define DVP2AXI_REG_DVP_DATA_BSEL                               DVP2AXI_REG_DVP_DATA_BSEL
#define DVP2AXI_REG_DVP_DATA_BSEL_POS                           (19U)
#define DVP2AXI_REG_DVP_DATA_BSEL_LEN                           (1U)
#define DVP2AXI_REG_DVP_DATA_BSEL_MSK                           (((1U<<DVP2AXI_REG_DVP_DATA_BSEL_LEN)-1)<<DVP2AXI_REG_DVP_DATA_BSEL_POS)
#define DVP2AXI_REG_DVP_DATA_BSEL_UMSK                          (~(((1U<<DVP2AXI_REG_DVP_DATA_BSEL_LEN)-1)<<DVP2AXI_REG_DVP_DATA_BSEL_POS))
#define DVP2AXI_REG_V_SUBSAMPLE_EN                              DVP2AXI_REG_V_SUBSAMPLE_EN
#define DVP2AXI_REG_V_SUBSAMPLE_EN_POS                          (22U)
#define DVP2AXI_REG_V_SUBSAMPLE_EN_LEN                          (1U)
#define DVP2AXI_REG_V_SUBSAMPLE_EN_MSK                          (((1U<<DVP2AXI_REG_V_SUBSAMPLE_EN_LEN)-1)<<DVP2AXI_REG_V_SUBSAMPLE_EN_POS)
#define DVP2AXI_REG_V_SUBSAMPLE_EN_UMSK                         (~(((1U<<DVP2AXI_REG_V_SUBSAMPLE_EN_LEN)-1)<<DVP2AXI_REG_V_SUBSAMPLE_EN_POS))
#define DVP2AXI_REG_V_SUBSAMPLE_POL                             DVP2AXI_REG_V_SUBSAMPLE_POL
#define DVP2AXI_REG_V_SUBSAMPLE_POL_POS                         (23U)
#define DVP2AXI_REG_V_SUBSAMPLE_POL_LEN                         (1U)
#define DVP2AXI_REG_V_SUBSAMPLE_POL_MSK                         (((1U<<DVP2AXI_REG_V_SUBSAMPLE_POL_LEN)-1)<<DVP2AXI_REG_V_SUBSAMPLE_POL_POS)
#define DVP2AXI_REG_V_SUBSAMPLE_POL_UMSK                        (~(((1U<<DVP2AXI_REG_V_SUBSAMPLE_POL_LEN)-1)<<DVP2AXI_REG_V_SUBSAMPLE_POL_POS))
#define DVP2AXI_REG_AXI_ROBUST_DIS                              DVP2AXI_REG_AXI_ROBUST_DIS
#define DVP2AXI_REG_AXI_ROBUST_DIS_POS                          (24U)
#define DVP2AXI_REG_AXI_ROBUST_DIS_LEN                          (1U)
#define DVP2AXI_REG_AXI_ROBUST_DIS_MSK                          (((1U<<DVP2AXI_REG_AXI_ROBUST_DIS_LEN)-1)<<DVP2AXI_REG_AXI_ROBUST_DIS_POS)
#define DVP2AXI_REG_AXI_ROBUST_DIS_UMSK                         (~(((1U<<DVP2AXI_REG_AXI_ROBUST_DIS_LEN)-1)<<DVP2AXI_REG_AXI_ROBUST_DIS_POS))
#define DVP2AXI_REG_W_OSTD_MAX                                  DVP2AXI_REG_W_OSTD_MAX
#define DVP2AXI_REG_W_OSTD_MAX_POS                              (28U)
#define DVP2AXI_REG_W_OSTD_MAX_LEN                              (3U)
#define DVP2AXI_REG_W_OSTD_MAX_MSK                              (((1U<<DVP2AXI_REG_W_OSTD_MAX_LEN)-1)<<DVP2AXI_REG_W_OSTD_MAX_POS)
#define DVP2AXI_REG_W_OSTD_MAX_UMSK                             (~(((1U<<DVP2AXI_REG_W_OSTD_MAX_LEN)-1)<<DVP2AXI_REG_W_OSTD_MAX_POS))

/* 0x4 : dvp2axi_addr_start */
#define DVP2AXI_ADDR_START_OFFSET                               (0x4)
#define DVP2AXI_REG_ADDR_START                                  DVP2AXI_REG_ADDR_START
#define DVP2AXI_REG_ADDR_START_POS                              (0U)
#define DVP2AXI_REG_ADDR_START_LEN                              (32U)
#define DVP2AXI_REG_ADDR_START_MSK                              (((1U<<DVP2AXI_REG_ADDR_START_LEN)-1)<<DVP2AXI_REG_ADDR_START_POS)
#define DVP2AXI_REG_ADDR_START_UMSK                             (~(((1U<<DVP2AXI_REG_ADDR_START_LEN)-1)<<DVP2AXI_REG_ADDR_START_POS))

/* 0x8 : dvp2axi_wrap_mem */
#define DVP2AXI_WRAP_MEM_OFFSET                                 (0x8)
#define DVP2AXI_REG_STRIDE_WIDTH                                DVP2AXI_REG_STRIDE_WIDTH
#define DVP2AXI_REG_STRIDE_WIDTH_POS                            (0U)
#define DVP2AXI_REG_STRIDE_WIDTH_LEN                            (16U)
#define DVP2AXI_REG_STRIDE_WIDTH_MSK                            (((1U<<DVP2AXI_REG_STRIDE_WIDTH_LEN)-1)<<DVP2AXI_REG_STRIDE_WIDTH_POS)
#define DVP2AXI_REG_STRIDE_WIDTH_UMSK                           (~(((1U<<DVP2AXI_REG_STRIDE_WIDTH_LEN)-1)<<DVP2AXI_REG_STRIDE_WIDTH_POS))
#define DVP2AXI_REG_WRAP_LCNT                                   DVP2AXI_REG_WRAP_LCNT
#define DVP2AXI_REG_WRAP_LCNT_POS                               (16U)
#define DVP2AXI_REG_WRAP_LCNT_LEN                               (14U)
#define DVP2AXI_REG_WRAP_LCNT_MSK                               (((1U<<DVP2AXI_REG_WRAP_LCNT_LEN)-1)<<DVP2AXI_REG_WRAP_LCNT_POS)
#define DVP2AXI_REG_WRAP_LCNT_UMSK                              (~(((1U<<DVP2AXI_REG_WRAP_LCNT_LEN)-1)<<DVP2AXI_REG_WRAP_LCNT_POS))

/* 0xC : dvp2axi_status */
#define DVP2AXI_STATUS_OFFSET                                   (0xC)
#define DVP2AXI_REG_FRAME_CNT_TRGR_INT                          DVP2AXI_REG_FRAME_CNT_TRGR_INT
#define DVP2AXI_REG_FRAME_CNT_TRGR_INT_POS                      (0U)
#define DVP2AXI_REG_FRAME_CNT_TRGR_INT_LEN                      (5U)
#define DVP2AXI_REG_FRAME_CNT_TRGR_INT_MSK                      (((1U<<DVP2AXI_REG_FRAME_CNT_TRGR_INT_LEN)-1)<<DVP2AXI_REG_FRAME_CNT_TRGR_INT_POS)
#define DVP2AXI_REG_FRAME_CNT_TRGR_INT_UMSK                     (~(((1U<<DVP2AXI_REG_FRAME_CNT_TRGR_INT_LEN)-1)<<DVP2AXI_REG_FRAME_CNT_TRGR_INT_POS))
#define DVP2AXI_REG_INT_HCNT_EN                                 DVP2AXI_REG_INT_HCNT_EN
#define DVP2AXI_REG_INT_HCNT_EN_POS                             (6U)
#define DVP2AXI_REG_INT_HCNT_EN_LEN                             (1U)
#define DVP2AXI_REG_INT_HCNT_EN_MSK                             (((1U<<DVP2AXI_REG_INT_HCNT_EN_LEN)-1)<<DVP2AXI_REG_INT_HCNT_EN_POS)
#define DVP2AXI_REG_INT_HCNT_EN_UMSK                            (~(((1U<<DVP2AXI_REG_INT_HCNT_EN_LEN)-1)<<DVP2AXI_REG_INT_HCNT_EN_POS))
#define DVP2AXI_REG_INT_VCNT_EN                                 DVP2AXI_REG_INT_VCNT_EN
#define DVP2AXI_REG_INT_VCNT_EN_POS                             (7U)
#define DVP2AXI_REG_INT_VCNT_EN_LEN                             (1U)
#define DVP2AXI_REG_INT_VCNT_EN_MSK                             (((1U<<DVP2AXI_REG_INT_VCNT_EN_LEN)-1)<<DVP2AXI_REG_INT_VCNT_EN_POS)
#define DVP2AXI_REG_INT_VCNT_EN_UMSK                            (~(((1U<<DVP2AXI_REG_INT_VCNT_EN_LEN)-1)<<DVP2AXI_REG_INT_VCNT_EN_POS))
#define DVP2AXI_REG_INT_NORMAL_EN                               DVP2AXI_REG_INT_NORMAL_EN
#define DVP2AXI_REG_INT_NORMAL_EN_POS                           (8U)
#define DVP2AXI_REG_INT_NORMAL_EN_LEN                           (1U)
#define DVP2AXI_REG_INT_NORMAL_EN_MSK                           (((1U<<DVP2AXI_REG_INT_NORMAL_EN_LEN)-1)<<DVP2AXI_REG_INT_NORMAL_EN_POS)
#define DVP2AXI_REG_INT_NORMAL_EN_UMSK                          (~(((1U<<DVP2AXI_REG_INT_NORMAL_EN_LEN)-1)<<DVP2AXI_REG_INT_NORMAL_EN_POS))
#define DVP2AXI_REG_INT_FIFO_EN                                 DVP2AXI_REG_INT_FIFO_EN
#define DVP2AXI_REG_INT_FIFO_EN_POS                             (11U)
#define DVP2AXI_REG_INT_FIFO_EN_LEN                             (1U)
#define DVP2AXI_REG_INT_FIFO_EN_MSK                             (((1U<<DVP2AXI_REG_INT_FIFO_EN_LEN)-1)<<DVP2AXI_REG_INT_FIFO_EN_POS)
#define DVP2AXI_REG_INT_FIFO_EN_UMSK                            (~(((1U<<DVP2AXI_REG_INT_FIFO_EN_LEN)-1)<<DVP2AXI_REG_INT_FIFO_EN_POS))
#define DVP2AXI_STS_NORMAL_INT                                  DVP2AXI_STS_NORMAL_INT
#define DVP2AXI_STS_NORMAL_INT_POS                              (12U)
#define DVP2AXI_STS_NORMAL_INT_LEN                              (1U)
#define DVP2AXI_STS_NORMAL_INT_MSK                              (((1U<<DVP2AXI_STS_NORMAL_INT_LEN)-1)<<DVP2AXI_STS_NORMAL_INT_POS)
#define DVP2AXI_STS_NORMAL_INT_UMSK                             (~(((1U<<DVP2AXI_STS_NORMAL_INT_LEN)-1)<<DVP2AXI_STS_NORMAL_INT_POS))
#define DVP2AXI_STS_FIFO_INT                                    DVP2AXI_STS_FIFO_INT
#define DVP2AXI_STS_FIFO_INT_POS                                (15U)
#define DVP2AXI_STS_FIFO_INT_LEN                                (1U)
#define DVP2AXI_STS_FIFO_INT_MSK                                (((1U<<DVP2AXI_STS_FIFO_INT_LEN)-1)<<DVP2AXI_STS_FIFO_INT_POS)
#define DVP2AXI_STS_FIFO_INT_UMSK                               (~(((1U<<DVP2AXI_STS_FIFO_INT_LEN)-1)<<DVP2AXI_STS_FIFO_INT_POS))
#define DVP2AXI_STS_HCNT_INT                                    DVP2AXI_STS_HCNT_INT
#define DVP2AXI_STS_HCNT_INT_POS                                (21U)
#define DVP2AXI_STS_HCNT_INT_LEN                                (1U)
#define DVP2AXI_STS_HCNT_INT_MSK                                (((1U<<DVP2AXI_STS_HCNT_INT_LEN)-1)<<DVP2AXI_STS_HCNT_INT_POS)
#define DVP2AXI_STS_HCNT_INT_UMSK                               (~(((1U<<DVP2AXI_STS_HCNT_INT_LEN)-1)<<DVP2AXI_STS_HCNT_INT_POS))
#define DVP2AXI_STS_VCNT_INT                                    DVP2AXI_STS_VCNT_INT
#define DVP2AXI_STS_VCNT_INT_POS                                (22U)
#define DVP2AXI_STS_VCNT_INT_LEN                                (1U)
#define DVP2AXI_STS_VCNT_INT_MSK                                (((1U<<DVP2AXI_STS_VCNT_INT_LEN)-1)<<DVP2AXI_STS_VCNT_INT_POS)
#define DVP2AXI_STS_VCNT_INT_UMSK                               (~(((1U<<DVP2AXI_STS_VCNT_INT_LEN)-1)<<DVP2AXI_STS_VCNT_INT_POS))
#define DVP2AXI_ST_IDLE                                         DVP2AXI_ST_IDLE
#define DVP2AXI_ST_IDLE_POS                                     (24U)
#define DVP2AXI_ST_IDLE_LEN                                     (1U)
#define DVP2AXI_ST_IDLE_MSK                                     (((1U<<DVP2AXI_ST_IDLE_LEN)-1)<<DVP2AXI_ST_IDLE_POS)
#define DVP2AXI_ST_IDLE_UMSK                                    (~(((1U<<DVP2AXI_ST_IDLE_LEN)-1)<<DVP2AXI_ST_IDLE_POS))
#define DVP2AXI_ST_FUNC                                         DVP2AXI_ST_FUNC
#define DVP2AXI_ST_FUNC_POS                                     (25U)
#define DVP2AXI_ST_FUNC_LEN                                     (1U)
#define DVP2AXI_ST_FUNC_MSK                                     (((1U<<DVP2AXI_ST_FUNC_LEN)-1)<<DVP2AXI_ST_FUNC_POS)
#define DVP2AXI_ST_FUNC_UMSK                                    (~(((1U<<DVP2AXI_ST_FUNC_LEN)-1)<<DVP2AXI_ST_FUNC_POS))
#define DVP2AXI_ST_LEND                                         DVP2AXI_ST_LEND
#define DVP2AXI_ST_LEND_POS                                     (26U)
#define DVP2AXI_ST_LEND_LEN                                     (1U)
#define DVP2AXI_ST_LEND_MSK                                     (((1U<<DVP2AXI_ST_LEND_LEN)-1)<<DVP2AXI_ST_LEND_POS)
#define DVP2AXI_ST_LEND_UMSK                                    (~(((1U<<DVP2AXI_ST_LEND_LEN)-1)<<DVP2AXI_ST_LEND_POS))
#define DVP2AXI_ST_FLSH                                         DVP2AXI_ST_FLSH
#define DVP2AXI_ST_FLSH_POS                                     (27U)
#define DVP2AXI_ST_FLSH_LEN                                     (1U)
#define DVP2AXI_ST_FLSH_MSK                                     (((1U<<DVP2AXI_ST_FLSH_LEN)-1)<<DVP2AXI_ST_FLSH_POS)
#define DVP2AXI_ST_FLSH_UMSK                                    (~(((1U<<DVP2AXI_ST_FLSH_LEN)-1)<<DVP2AXI_ST_FLSH_POS))
#define DVP2AXI_AXI_IDLE                                        DVP2AXI_AXI_IDLE
#define DVP2AXI_AXI_IDLE_POS                                    (28U)
#define DVP2AXI_AXI_IDLE_LEN                                    (1U)
#define DVP2AXI_AXI_IDLE_MSK                                    (((1U<<DVP2AXI_AXI_IDLE_LEN)-1)<<DVP2AXI_AXI_IDLE_POS)
#define DVP2AXI_AXI_IDLE_UMSK                                   (~(((1U<<DVP2AXI_AXI_IDLE_LEN)-1)<<DVP2AXI_AXI_IDLE_POS))

/* 0x10 : dvp2axi_frame */
#define DVP2AXI_FRAME_OFFSET                                    (0x10)
#define DVP2AXI_REG_FRAME_WIDTH                                 DVP2AXI_REG_FRAME_WIDTH
#define DVP2AXI_REG_FRAME_WIDTH_POS                             (0U)
#define DVP2AXI_REG_FRAME_WIDTH_LEN                             (16U)
#define DVP2AXI_REG_FRAME_WIDTH_MSK                             (((1U<<DVP2AXI_REG_FRAME_WIDTH_LEN)-1)<<DVP2AXI_REG_FRAME_WIDTH_POS)
#define DVP2AXI_REG_FRAME_WIDTH_UMSK                            (~(((1U<<DVP2AXI_REG_FRAME_WIDTH_LEN)-1)<<DVP2AXI_REG_FRAME_WIDTH_POS))
#define DVP2AXI_REG_FRAME_HEIGHT                                DVP2AXI_REG_FRAME_HEIGHT
#define DVP2AXI_REG_FRAME_HEIGHT_POS                            (16U)
#define DVP2AXI_REG_FRAME_HEIGHT_LEN                            (14U)
#define DVP2AXI_REG_FRAME_HEIGHT_MSK                            (((1U<<DVP2AXI_REG_FRAME_HEIGHT_LEN)-1)<<DVP2AXI_REG_FRAME_HEIGHT_POS)
#define DVP2AXI_REG_FRAME_HEIGHT_UMSK                           (~(((1U<<DVP2AXI_REG_FRAME_HEIGHT_LEN)-1)<<DVP2AXI_REG_FRAME_HEIGHT_POS))

/* 0x14 : dvp_w1p */
#define DVP2AXI_DVP_W1P_OFFSET                                  (0x14)
#define DVP2AXI_REG_INT_NORMAL_CLR                              DVP2AXI_REG_INT_NORMAL_CLR
#define DVP2AXI_REG_INT_NORMAL_CLR_POS                          (4U)
#define DVP2AXI_REG_INT_NORMAL_CLR_LEN                          (1U)
#define DVP2AXI_REG_INT_NORMAL_CLR_MSK                          (((1U<<DVP2AXI_REG_INT_NORMAL_CLR_LEN)-1)<<DVP2AXI_REG_INT_NORMAL_CLR_POS)
#define DVP2AXI_REG_INT_NORMAL_CLR_UMSK                         (~(((1U<<DVP2AXI_REG_INT_NORMAL_CLR_LEN)-1)<<DVP2AXI_REG_INT_NORMAL_CLR_POS))
#define DVP2AXI_REG_INT_FIFO_CLR                                DVP2AXI_REG_INT_FIFO_CLR
#define DVP2AXI_REG_INT_FIFO_CLR_POS                            (7U)
#define DVP2AXI_REG_INT_FIFO_CLR_LEN                            (1U)
#define DVP2AXI_REG_INT_FIFO_CLR_MSK                            (((1U<<DVP2AXI_REG_INT_FIFO_CLR_LEN)-1)<<DVP2AXI_REG_INT_FIFO_CLR_POS)
#define DVP2AXI_REG_INT_FIFO_CLR_UMSK                           (~(((1U<<DVP2AXI_REG_INT_FIFO_CLR_LEN)-1)<<DVP2AXI_REG_INT_FIFO_CLR_POS))
#define DVP2AXI_REG_INT_HCNT_CLR                                DVP2AXI_REG_INT_HCNT_CLR
#define DVP2AXI_REG_INT_HCNT_CLR_POS                            (8U)
#define DVP2AXI_REG_INT_HCNT_CLR_LEN                            (1U)
#define DVP2AXI_REG_INT_HCNT_CLR_MSK                            (((1U<<DVP2AXI_REG_INT_HCNT_CLR_LEN)-1)<<DVP2AXI_REG_INT_HCNT_CLR_POS)
#define DVP2AXI_REG_INT_HCNT_CLR_UMSK                           (~(((1U<<DVP2AXI_REG_INT_HCNT_CLR_LEN)-1)<<DVP2AXI_REG_INT_HCNT_CLR_POS))
#define DVP2AXI_REG_INT_VCNT_CLR                                DVP2AXI_REG_INT_VCNT_CLR
#define DVP2AXI_REG_INT_VCNT_CLR_POS                            (9U)
#define DVP2AXI_REG_INT_VCNT_CLR_LEN                            (1U)
#define DVP2AXI_REG_INT_VCNT_CLR_MSK                            (((1U<<DVP2AXI_REG_INT_VCNT_CLR_LEN)-1)<<DVP2AXI_REG_INT_VCNT_CLR_POS)
#define DVP2AXI_REG_INT_VCNT_CLR_UMSK                           (~(((1U<<DVP2AXI_REG_INT_VCNT_CLR_LEN)-1)<<DVP2AXI_REG_INT_VCNT_CLR_POS))
#define DVP2AXI_CR_SW_SHD                                       DVP2AXI_CR_SW_SHD
#define DVP2AXI_CR_SW_SHD_POS                                   (16U)
#define DVP2AXI_CR_SW_SHD_LEN                                   (1U)
#define DVP2AXI_CR_SW_SHD_MSK                                   (((1U<<DVP2AXI_CR_SW_SHD_LEN)-1)<<DVP2AXI_CR_SW_SHD_POS)
#define DVP2AXI_CR_SW_SHD_UMSK                                  (~(((1U<<DVP2AXI_CR_SW_SHD_LEN)-1)<<DVP2AXI_CR_SW_SHD_POS))

/* 0x18 : dvp2axi_frame_vld */
#define DVP2AXI_FRAME_VLD_OFFSET                                (0x18)
#define DVP2AXI_REG_FRAME_N_VLD                                 DVP2AXI_REG_FRAME_N_VLD
#define DVP2AXI_REG_FRAME_N_VLD_POS                             (0U)
#define DVP2AXI_REG_FRAME_N_VLD_LEN                             (32U)
#define DVP2AXI_REG_FRAME_N_VLD_MSK                             (((1U<<DVP2AXI_REG_FRAME_N_VLD_LEN)-1)<<DVP2AXI_REG_FRAME_N_VLD_POS)
#define DVP2AXI_REG_FRAME_N_VLD_UMSK                            (~(((1U<<DVP2AXI_REG_FRAME_N_VLD_LEN)-1)<<DVP2AXI_REG_FRAME_N_VLD_POS))

/* 0x1C : dvp2axi_frame_period */
#define DVP2AXI_FRAME_PERIOD_OFFSET                             (0x1C)
#define DVP2AXI_REG_FRAME_PERIOD                                DVP2AXI_REG_FRAME_PERIOD
#define DVP2AXI_REG_FRAME_PERIOD_POS                            (0U)
#define DVP2AXI_REG_FRAME_PERIOD_LEN                            (5U)
#define DVP2AXI_REG_FRAME_PERIOD_MSK                            (((1U<<DVP2AXI_REG_FRAME_PERIOD_LEN)-1)<<DVP2AXI_REG_FRAME_PERIOD_POS)
#define DVP2AXI_REG_FRAME_PERIOD_UMSK                           (~(((1U<<DVP2AXI_REG_FRAME_PERIOD_LEN)-1)<<DVP2AXI_REG_FRAME_PERIOD_POS))
#define DVP2AXI_STS_FRAME_PERIOD_N                              DVP2AXI_STS_FRAME_PERIOD_N
#define DVP2AXI_STS_FRAME_PERIOD_N_POS                          (8U)
#define DVP2AXI_STS_FRAME_PERIOD_N_LEN                          (5U)
#define DVP2AXI_STS_FRAME_PERIOD_N_MSK                          (((1U<<DVP2AXI_STS_FRAME_PERIOD_N_LEN)-1)<<DVP2AXI_STS_FRAME_PERIOD_N_POS)
#define DVP2AXI_STS_FRAME_PERIOD_N_UMSK                         (~(((1U<<DVP2AXI_STS_FRAME_PERIOD_N_LEN)-1)<<DVP2AXI_STS_FRAME_PERIOD_N_POS))

/* 0x20 : dvp2axi_misc */
#define DVP2AXI_MISC_OFFSET                                     (0x20)
#define DVP2AXI_REG_ALPHA                                       DVP2AXI_REG_ALPHA
#define DVP2AXI_REG_ALPHA_POS                                   (0U)
#define DVP2AXI_REG_ALPHA_LEN                                   (8U)
#define DVP2AXI_REG_ALPHA_MSK                                   (((1U<<DVP2AXI_REG_ALPHA_LEN)-1)<<DVP2AXI_REG_ALPHA_POS)
#define DVP2AXI_REG_ALPHA_UMSK                                  (~(((1U<<DVP2AXI_REG_ALPHA_LEN)-1)<<DVP2AXI_REG_ALPHA_POS))
#define DVP2AXI_REG_FORMAT_565                                  DVP2AXI_REG_FORMAT_565
#define DVP2AXI_REG_FORMAT_565_POS                              (8U)
#define DVP2AXI_REG_FORMAT_565_LEN                              (3U)
#define DVP2AXI_REG_FORMAT_565_MSK                              (((1U<<DVP2AXI_REG_FORMAT_565_LEN)-1)<<DVP2AXI_REG_FORMAT_565_POS)
#define DVP2AXI_REG_FORMAT_565_UMSK                             (~(((1U<<DVP2AXI_REG_FORMAT_565_LEN)-1)<<DVP2AXI_REG_FORMAT_565_POS))
#define DVP2AXI_REG_FIFO_THRE_WARNING                           DVP2AXI_REG_FIFO_THRE_WARNING
#define DVP2AXI_REG_FIFO_THRE_WARNING_POS                       (16U)
#define DVP2AXI_REG_FIFO_THRE_WARNING_LEN                       (2U)
#define DVP2AXI_REG_FIFO_THRE_WARNING_MSK                       (((1U<<DVP2AXI_REG_FIFO_THRE_WARNING_LEN)-1)<<DVP2AXI_REG_FIFO_THRE_WARNING_POS)
#define DVP2AXI_REG_FIFO_THRE_WARNING_UMSK                      (~(((1U<<DVP2AXI_REG_FIFO_THRE_WARNING_LEN)-1)<<DVP2AXI_REG_FIFO_THRE_WARNING_POS))
#define DVP2AXI_STS_SFIFO_WARNING                               DVP2AXI_STS_SFIFO_WARNING
#define DVP2AXI_STS_SFIFO_WARNING_POS                           (19U)
#define DVP2AXI_STS_SFIFO_WARNING_LEN                           (1U)
#define DVP2AXI_STS_SFIFO_WARNING_MSK                           (((1U<<DVP2AXI_STS_SFIFO_WARNING_LEN)-1)<<DVP2AXI_STS_SFIFO_WARNING_POS)
#define DVP2AXI_STS_SFIFO_WARNING_UMSK                          (~(((1U<<DVP2AXI_STS_SFIFO_WARNING_LEN)-1)<<DVP2AXI_STS_SFIFO_WARNING_POS))
#define DVP2AXI_REG_FIFO_THRE_URGENT                            DVP2AXI_REG_FIFO_THRE_URGENT
#define DVP2AXI_REG_FIFO_THRE_URGENT_POS                        (20U)
#define DVP2AXI_REG_FIFO_THRE_URGENT_LEN                        (2U)
#define DVP2AXI_REG_FIFO_THRE_URGENT_MSK                        (((1U<<DVP2AXI_REG_FIFO_THRE_URGENT_LEN)-1)<<DVP2AXI_REG_FIFO_THRE_URGENT_POS)
#define DVP2AXI_REG_FIFO_THRE_URGENT_UMSK                       (~(((1U<<DVP2AXI_REG_FIFO_THRE_URGENT_LEN)-1)<<DVP2AXI_REG_FIFO_THRE_URGENT_POS))
#define DVP2AXI_STS_SFIFO_URGENT                                DVP2AXI_STS_SFIFO_URGENT
#define DVP2AXI_STS_SFIFO_URGENT_POS                            (23U)
#define DVP2AXI_STS_SFIFO_URGENT_LEN                            (1U)
#define DVP2AXI_STS_SFIFO_URGENT_MSK                            (((1U<<DVP2AXI_STS_SFIFO_URGENT_LEN)-1)<<DVP2AXI_STS_SFIFO_URGENT_POS)
#define DVP2AXI_STS_SFIFO_URGENT_UMSK                           (~(((1U<<DVP2AXI_STS_SFIFO_URGENT_LEN)-1)<<DVP2AXI_STS_SFIFO_URGENT_POS))

/* 0x30 : dvp2axi_hsync_crop */
#define DVP2AXI_HSYNC_CROP_OFFSET                               (0x30)
#define DVP2AXI_REG_HSYNC_ACT_END                               DVP2AXI_REG_HSYNC_ACT_END
#define DVP2AXI_REG_HSYNC_ACT_END_POS                           (0U)
#define DVP2AXI_REG_HSYNC_ACT_END_LEN                           (14U)
#define DVP2AXI_REG_HSYNC_ACT_END_MSK                           (((1U<<DVP2AXI_REG_HSYNC_ACT_END_LEN)-1)<<DVP2AXI_REG_HSYNC_ACT_END_POS)
#define DVP2AXI_REG_HSYNC_ACT_END_UMSK                          (~(((1U<<DVP2AXI_REG_HSYNC_ACT_END_LEN)-1)<<DVP2AXI_REG_HSYNC_ACT_END_POS))
#define DVP2AXI_REG_HSYNC_ACT_START                             DVP2AXI_REG_HSYNC_ACT_START
#define DVP2AXI_REG_HSYNC_ACT_START_POS                         (16U)
#define DVP2AXI_REG_HSYNC_ACT_START_LEN                         (14U)
#define DVP2AXI_REG_HSYNC_ACT_START_MSK                         (((1U<<DVP2AXI_REG_HSYNC_ACT_START_LEN)-1)<<DVP2AXI_REG_HSYNC_ACT_START_POS)
#define DVP2AXI_REG_HSYNC_ACT_START_UMSK                        (~(((1U<<DVP2AXI_REG_HSYNC_ACT_START_LEN)-1)<<DVP2AXI_REG_HSYNC_ACT_START_POS))

/* 0x34 : dvp2axi_vsync_crop */
#define DVP2AXI_VSYNC_CROP_OFFSET                               (0x34)
#define DVP2AXI_REG_VSYNC_ACT_END                               DVP2AXI_REG_VSYNC_ACT_END
#define DVP2AXI_REG_VSYNC_ACT_END_POS                           (0U)
#define DVP2AXI_REG_VSYNC_ACT_END_LEN                           (14U)
#define DVP2AXI_REG_VSYNC_ACT_END_MSK                           (((1U<<DVP2AXI_REG_VSYNC_ACT_END_LEN)-1)<<DVP2AXI_REG_VSYNC_ACT_END_POS)
#define DVP2AXI_REG_VSYNC_ACT_END_UMSK                          (~(((1U<<DVP2AXI_REG_VSYNC_ACT_END_LEN)-1)<<DVP2AXI_REG_VSYNC_ACT_END_POS))
#define DVP2AXI_REG_VSYNC_ACT_START                             DVP2AXI_REG_VSYNC_ACT_START
#define DVP2AXI_REG_VSYNC_ACT_START_POS                         (16U)
#define DVP2AXI_REG_VSYNC_ACT_START_LEN                         (14U)
#define DVP2AXI_REG_VSYNC_ACT_START_MSK                         (((1U<<DVP2AXI_REG_VSYNC_ACT_START_LEN)-1)<<DVP2AXI_REG_VSYNC_ACT_START_POS)
#define DVP2AXI_REG_VSYNC_ACT_START_UMSK                        (~(((1U<<DVP2AXI_REG_VSYNC_ACT_START_LEN)-1)<<DVP2AXI_REG_VSYNC_ACT_START_POS))

/* 0x38 : dvp2axi_fram_exm */
#define DVP2AXI_FRAM_EXM_OFFSET                                 (0x38)
#define DVP2AXI_REG_TOTAL_HCNT                                  DVP2AXI_REG_TOTAL_HCNT
#define DVP2AXI_REG_TOTAL_HCNT_POS                              (0U)
#define DVP2AXI_REG_TOTAL_HCNT_LEN                              (14U)
#define DVP2AXI_REG_TOTAL_HCNT_MSK                              (((1U<<DVP2AXI_REG_TOTAL_HCNT_LEN)-1)<<DVP2AXI_REG_TOTAL_HCNT_POS)
#define DVP2AXI_REG_TOTAL_HCNT_UMSK                             (~(((1U<<DVP2AXI_REG_TOTAL_HCNT_LEN)-1)<<DVP2AXI_REG_TOTAL_HCNT_POS))
#define DVP2AXI_REG_TOTAL_VCNT                                  DVP2AXI_REG_TOTAL_VCNT
#define DVP2AXI_REG_TOTAL_VCNT_POS                              (16U)
#define DVP2AXI_REG_TOTAL_VCNT_LEN                              (14U)
#define DVP2AXI_REG_TOTAL_VCNT_MSK                              (((1U<<DVP2AXI_REG_TOTAL_VCNT_LEN)-1)<<DVP2AXI_REG_TOTAL_VCNT_POS)
#define DVP2AXI_REG_TOTAL_VCNT_UMSK                             (~(((1U<<DVP2AXI_REG_TOTAL_VCNT_LEN)-1)<<DVP2AXI_REG_TOTAL_VCNT_POS))

/* 0xFC : dvp_dummy_reg */
#define DVP2AXI_DVP_DUMMY_REG_OFFSET                            (0xFC)


struct  dvp2axi_reg {
    /* 0x0 : dvp2axi_configue */
    union {
        struct {
            uint32_t reg_dvp_enable                 :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_wrap_mode                  :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_fram_vld_pol               :  1; /* [    2],        r/w,        0x1 */
            uint32_t reg_line_vld_pol               :  1; /* [    3],        r/w,        0x1 */
            uint32_t reg_xlen                       :  3; /* [ 6: 4],        r/w,        0x3 */
            uint32_t reg_flip_mode                  :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_dvp_mode                   :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reg_wrap_addr_start            :  1; /* [   11],        r/w,        0x0 */
            uint32_t reg_drop_en                    :  1; /* [   12],        r/w,        0x0 */
            uint32_t reg_drop_even                  :  1; /* [   13],        r/w,        0x0 */
            uint32_t reg_qos_sw_mode                :  1; /* [   14],        r/w,        0x0 */
            uint32_t reg_qos_sw                     :  1; /* [   15],        r/w,        0x0 */
            uint32_t reg_dvp_data_mode              :  3; /* [18:16],        r/w,        0x0 */
            uint32_t reg_dvp_data_bsel              :  1; /* [   19],        r/w,        0x0 */
            uint32_t reserved_20_21                 :  2; /* [21:20],       rsvd,        0x0 */
            uint32_t reg_v_subsample_en             :  1; /* [   22],        r/w,        0x0 */
            uint32_t reg_v_subsample_pol            :  1; /* [   23],        r/w,        0x0 */
            uint32_t reg_axi_robust_dis             :  1; /* [   24],        r/w,        0x0 */
            uint32_t reserved_25_27                 :  3; /* [27:25],       rsvd,        0x0 */
            uint32_t reg_w_ostd_max                 :  3; /* [30:28],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_configue;

    /* 0x4 : dvp2axi_addr_start */
    union {
        struct {
            uint32_t reg_addr_start                 : 32; /* [31: 0],        r/w, 0x80000000L */
        }BF;
        uint32_t WORD;
    } dvp2axi_addr_start;

    /* 0x8 : dvp2axi_wrap_mem */
    union {
        struct {
            uint32_t reg_stride_width               : 16; /* [15: 0],        r/w,        0x0 */
            uint32_t reg_wrap_lcnt                  : 14; /* [29:16],        r/w,      0x510 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_wrap_mem;

    /* 0xC : dvp2axi_status */
    union {
        struct {
            uint32_t reg_frame_cnt_trgr_int         :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5                     :  1; /* [    5],       rsvd,        0x0 */
            uint32_t reg_int_hcnt_en                :  1; /* [    6],        r/w,        0x0 */
            uint32_t reg_int_vcnt_en                :  1; /* [    7],        r/w,        0x0 */
            uint32_t reg_int_normal_en              :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9_10                  :  2; /* [10: 9],       rsvd,        0x0 */
            uint32_t reg_int_fifo_en                :  1; /* [   11],        r/w,        0x1 */
            uint32_t sts_normal_int                 :  1; /* [   12],          r,        0x0 */
            uint32_t reserved_13_14                 :  2; /* [14:13],       rsvd,        0x0 */
            uint32_t sts_fifo_int                   :  1; /* [   15],          r,        0x0 */
            uint32_t reserved_16_20                 :  5; /* [20:16],       rsvd,        0x0 */
            uint32_t sts_hcnt_int                   :  1; /* [   21],          r,        0x0 */
            uint32_t sts_vcnt_int                   :  1; /* [   22],          r,        0x0 */
            uint32_t reserved_23                    :  1; /* [   23],       rsvd,        0x0 */
            uint32_t st_idle                        :  1; /* [   24],          r,        0x1 */
            uint32_t st_func                        :  1; /* [   25],          r,        0x0 */
            uint32_t st_lend                        :  1; /* [   26],          r,        0x0 */
            uint32_t st_flsh                        :  1; /* [   27],          r,        0x0 */
            uint32_t axi_idle                       :  1; /* [   28],          r,        0x1 */
            uint32_t reserved_29_31                 :  3; /* [31:29],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_status;

    /* 0x10 : dvp2axi_frame */
    union {
        struct {
            uint32_t reg_frame_width                : 16; /* [15: 0],        r/w,      0x900 */
            uint32_t reg_frame_height               : 14; /* [29:16],        r/w,      0x510 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_frame;

    /* 0x14 : dvp_w1p */
    union {
        struct {
            uint32_t reserved_0_3                   :  4; /* [ 3: 0],       rsvd,        0x0 */
            uint32_t reg_int_normal_clr             :  1; /* [    4],        w1p,        0x0 */
            uint32_t reserved_5_6                   :  2; /* [ 6: 5],       rsvd,        0x0 */
            uint32_t reg_int_fifo_clr               :  1; /* [    7],        w1p,        0x0 */
            uint32_t reg_int_hcnt_clr               :  1; /* [    8],        w1p,        0x0 */
            uint32_t reg_int_vcnt_clr               :  1; /* [    9],        w1p,        0x0 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t cr_sw_shd                      :  1; /* [   16],        w1p,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_w1p;

    /* 0x18 : dvp2axi_frame_vld */
    union {
        struct {
            uint32_t reg_frame_n_vld                : 32; /* [31: 0],        r/w, 0xffffffffL */
        }BF;
        uint32_t WORD;
    } dvp2axi_frame_vld;

    /* 0x1C : dvp2axi_frame_period */
    union {
        struct {
            uint32_t reg_frame_period               :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t sts_frame_period_n             :  5; /* [12: 8],          r,        0x0 */
            uint32_t reserved_13_31                 : 19; /* [31:13],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_frame_period;

    /* 0x20 : dvp2axi_misc */
    union {
        struct {
            uint32_t reg_alpha                      :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t reg_format_565                 :  3; /* [10: 8],        r/w,        0x0 */
            uint32_t reserved_11_15                 :  5; /* [15:11],       rsvd,        0x0 */
            uint32_t reg_fifo_thre_warning          :  2; /* [17:16],        r/w,        0x0 */
            uint32_t reserved_18                    :  1; /* [   18],       rsvd,        0x0 */
            uint32_t sts_sfifo_warning              :  1; /* [   19],          r,        0x0 */
            uint32_t reg_fifo_thre_urgent           :  2; /* [21:20],        r/w,        0x1 */
            uint32_t reserved_22                    :  1; /* [   22],       rsvd,        0x0 */
            uint32_t sts_sfifo_urgent               :  1; /* [   23],          r,        0x0 */
            uint32_t reserved_24_31                 :  8; /* [31:24],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_misc;

    /* 0x24  reserved */
    uint8_t RESERVED0x24[12];

    /* 0x30 : dvp2axi_hsync_crop */
    union {
        struct {
            uint32_t reg_hsync_act_end              : 14; /* [13: 0],        r/w,     0x3fff */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_hsync_act_start            : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_hsync_crop;

    /* 0x34 : dvp2axi_vsync_crop */
    union {
        struct {
            uint32_t reg_vsync_act_end              : 14; /* [13: 0],        r/w,     0x3fff */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_vsync_act_start            : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_vsync_crop;

    /* 0x38 : dvp2axi_fram_exm */
    union {
        struct {
            uint32_t reg_total_hcnt                 : 14; /* [13: 0],        r/w,     0x3fff */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_total_vcnt                 : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp2axi_fram_exm;

    /* 0x3c  reserved */
    uint8_t RESERVED0x3c[192];

    /* 0xFC : dvp_dummy_reg */
    union {
        struct {
            uint32_t RESERVED_7_0                   :  8; /* [ 7: 0],       rsvd,       0xf0 */
            uint32_t reserved_8_31                  : 24; /* [31: 8],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } dvp_dummy_reg;

};

typedef volatile struct dvp2axi_reg dvp2axi_reg_t;


#endif  /* __DVP2AXI_REG_H__ */
