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

#ifndef  __OSD_BLEND_REG_H__
#define  __OSD_BLEND_REG_H__

#include "ipro7.h"

/* 0x0 : obnd_layer_xconfig */
#define OSD_BLEND_OBND_LAYER_XCONFIG_OFFSET                     (0x0)
#define OSD_BLEND_OBND_X_MIN                                    OSD_BLEND_OBND_X_MIN
#define OSD_BLEND_OBND_X_MIN_POS                                (0U)
#define OSD_BLEND_OBND_X_MIN_LEN                                (12U)
#define OSD_BLEND_OBND_X_MIN_MSK                                (((1U<<OSD_BLEND_OBND_X_MIN_LEN)-1)<<OSD_BLEND_OBND_X_MIN_POS)
#define OSD_BLEND_OBND_X_MIN_UMSK                               (~(((1U<<OSD_BLEND_OBND_X_MIN_LEN)-1)<<OSD_BLEND_OBND_X_MIN_POS))
#define OSD_BLEND_OBND_X_MAX                                    OSD_BLEND_OBND_X_MAX
#define OSD_BLEND_OBND_X_MAX_POS                                (16U)
#define OSD_BLEND_OBND_X_MAX_LEN                                (12U)
#define OSD_BLEND_OBND_X_MAX_MSK                                (((1U<<OSD_BLEND_OBND_X_MAX_LEN)-1)<<OSD_BLEND_OBND_X_MAX_POS)
#define OSD_BLEND_OBND_X_MAX_UMSK                               (~(((1U<<OSD_BLEND_OBND_X_MAX_LEN)-1)<<OSD_BLEND_OBND_X_MAX_POS))

/* 0x4 : obnd_layer_yconfig */
#define OSD_BLEND_OBND_LAYER_YCONFIG_OFFSET                     (0x4)
#define OSD_BLEND_OBND_Y_MIN                                    OSD_BLEND_OBND_Y_MIN
#define OSD_BLEND_OBND_Y_MIN_POS                                (0U)
#define OSD_BLEND_OBND_Y_MIN_LEN                                (12U)
#define OSD_BLEND_OBND_Y_MIN_MSK                                (((1U<<OSD_BLEND_OBND_Y_MIN_LEN)-1)<<OSD_BLEND_OBND_Y_MIN_POS)
#define OSD_BLEND_OBND_Y_MIN_UMSK                               (~(((1U<<OSD_BLEND_OBND_Y_MIN_LEN)-1)<<OSD_BLEND_OBND_Y_MIN_POS))
#define OSD_BLEND_OBND_Y_MAX                                    OSD_BLEND_OBND_Y_MAX
#define OSD_BLEND_OBND_Y_MAX_POS                                (16U)
#define OSD_BLEND_OBND_Y_MAX_LEN                                (12U)
#define OSD_BLEND_OBND_Y_MAX_MSK                                (((1U<<OSD_BLEND_OBND_Y_MAX_LEN)-1)<<OSD_BLEND_OBND_Y_MAX_POS)
#define OSD_BLEND_OBND_Y_MAX_UMSK                               (~(((1U<<OSD_BLEND_OBND_Y_MAX_LEN)-1)<<OSD_BLEND_OBND_Y_MAX_POS))

/* 0x8 : obnd_mem_config0 */
#define OSD_BLEND_OBND_MEM_CONFIG0_OFFSET                       (0x8)
#define OSD_BLEND_OBND_FORCE_SH                                 OSD_BLEND_OBND_FORCE_SH
#define OSD_BLEND_OBND_FORCE_SH_POS                             (0U)
#define OSD_BLEND_OBND_FORCE_SH_LEN                             (1U)
#define OSD_BLEND_OBND_FORCE_SH_MSK                             (((1U<<OSD_BLEND_OBND_FORCE_SH_LEN)-1)<<OSD_BLEND_OBND_FORCE_SH_POS)
#define OSD_BLEND_OBND_FORCE_SH_UMSK                            (~(((1U<<OSD_BLEND_OBND_FORCE_SH_LEN)-1)<<OSD_BLEND_OBND_FORCE_SH_POS))
#define OSD_BLEND_REG_OSD_BUS_STRT_W                            OSD_BLEND_REG_OSD_BUS_STRT_W
#define OSD_BLEND_REG_OSD_BUS_STRT_W_POS                        (1U)
#define OSD_BLEND_REG_OSD_BUS_STRT_W_LEN                        (1U)
#define OSD_BLEND_REG_OSD_BUS_STRT_W_MSK                        (((1U<<OSD_BLEND_REG_OSD_BUS_STRT_W_LEN)-1)<<OSD_BLEND_REG_OSD_BUS_STRT_W_POS)
#define OSD_BLEND_REG_OSD_BUS_STRT_W_UMSK                       (~(((1U<<OSD_BLEND_REG_OSD_BUS_STRT_W_LEN)-1)<<OSD_BLEND_REG_OSD_BUS_STRT_W_POS))
#define OSD_BLEND_REG_OSD_SEOF_INT_CLR_W                        OSD_BLEND_REG_OSD_SEOF_INT_CLR_W
#define OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_POS                    (2U)
#define OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_LEN                    (1U)
#define OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_MSK                    (((1U<<OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_POS)
#define OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_UMSK                   (~(((1U<<OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_CLR_W_POS))
#define OSD_BLEND_STS_OSD_CMD_IDLE_R                            OSD_BLEND_STS_OSD_CMD_IDLE_R
#define OSD_BLEND_STS_OSD_CMD_IDLE_R_POS                        (16U)
#define OSD_BLEND_STS_OSD_CMD_IDLE_R_LEN                        (1U)
#define OSD_BLEND_STS_OSD_CMD_IDLE_R_MSK                        (((1U<<OSD_BLEND_STS_OSD_CMD_IDLE_R_LEN)-1)<<OSD_BLEND_STS_OSD_CMD_IDLE_R_POS)
#define OSD_BLEND_STS_OSD_CMD_IDLE_R_UMSK                       (~(((1U<<OSD_BLEND_STS_OSD_CMD_IDLE_R_LEN)-1)<<OSD_BLEND_STS_OSD_CMD_IDLE_R_POS))
#define OSD_BLEND_STS_OSD_AXI_IDLE_R                            OSD_BLEND_STS_OSD_AXI_IDLE_R
#define OSD_BLEND_STS_OSD_AXI_IDLE_R_POS                        (17U)
#define OSD_BLEND_STS_OSD_AXI_IDLE_R_LEN                        (1U)
#define OSD_BLEND_STS_OSD_AXI_IDLE_R_MSK                        (((1U<<OSD_BLEND_STS_OSD_AXI_IDLE_R_LEN)-1)<<OSD_BLEND_STS_OSD_AXI_IDLE_R_POS)
#define OSD_BLEND_STS_OSD_AXI_IDLE_R_UMSK                       (~(((1U<<OSD_BLEND_STS_OSD_AXI_IDLE_R_LEN)-1)<<OSD_BLEND_STS_OSD_AXI_IDLE_R_POS))
#define OSD_BLEND_STS_OSD_DRAIN_INT_R                           OSD_BLEND_STS_OSD_DRAIN_INT_R
#define OSD_BLEND_STS_OSD_DRAIN_INT_R_POS                       (18U)
#define OSD_BLEND_STS_OSD_DRAIN_INT_R_LEN                       (1U)
#define OSD_BLEND_STS_OSD_DRAIN_INT_R_MSK                       (((1U<<OSD_BLEND_STS_OSD_DRAIN_INT_R_LEN)-1)<<OSD_BLEND_STS_OSD_DRAIN_INT_R_POS)
#define OSD_BLEND_STS_OSD_DRAIN_INT_R_UMSK                      (~(((1U<<OSD_BLEND_STS_OSD_DRAIN_INT_R_LEN)-1)<<OSD_BLEND_STS_OSD_DRAIN_INT_R_POS))
#define OSD_BLEND_STS_OSD_SEOF_INT_R                            OSD_BLEND_STS_OSD_SEOF_INT_R
#define OSD_BLEND_STS_OSD_SEOF_INT_R_POS                        (19U)
#define OSD_BLEND_STS_OSD_SEOF_INT_R_LEN                        (1U)
#define OSD_BLEND_STS_OSD_SEOF_INT_R_MSK                        (((1U<<OSD_BLEND_STS_OSD_SEOF_INT_R_LEN)-1)<<OSD_BLEND_STS_OSD_SEOF_INT_R_POS)
#define OSD_BLEND_STS_OSD_SEOF_INT_R_UMSK                       (~(((1U<<OSD_BLEND_STS_OSD_SEOF_INT_R_LEN)-1)<<OSD_BLEND_STS_OSD_SEOF_INT_R_POS))

/* 0x10 : osd_misc */
#define OSD_BLEND_OSD_MISC_OFFSET                               (0x10)
#define OSD_BLEND_OBND_LAYER_EN                                 OSD_BLEND_OBND_LAYER_EN
#define OSD_BLEND_OBND_LAYER_EN_POS                             (0U)
#define OSD_BLEND_OBND_LAYER_EN_LEN                             (1U)
#define OSD_BLEND_OBND_LAYER_EN_MSK                             (((1U<<OSD_BLEND_OBND_LAYER_EN_LEN)-1)<<OSD_BLEND_OBND_LAYER_EN_POS)
#define OSD_BLEND_OBND_LAYER_EN_UMSK                            (~(((1U<<OSD_BLEND_OBND_LAYER_EN_LEN)-1)<<OSD_BLEND_OBND_LAYER_EN_POS))
#define OSD_BLEND_REG_QOS_SW_MODE_W                             OSD_BLEND_REG_QOS_SW_MODE_W
#define OSD_BLEND_REG_QOS_SW_MODE_W_POS                         (1U)
#define OSD_BLEND_REG_QOS_SW_MODE_W_LEN                         (1U)
#define OSD_BLEND_REG_QOS_SW_MODE_W_MSK                         (((1U<<OSD_BLEND_REG_QOS_SW_MODE_W_LEN)-1)<<OSD_BLEND_REG_QOS_SW_MODE_W_POS)
#define OSD_BLEND_REG_QOS_SW_MODE_W_UMSK                        (~(((1U<<OSD_BLEND_REG_QOS_SW_MODE_W_LEN)-1)<<OSD_BLEND_REG_QOS_SW_MODE_W_POS))
#define OSD_BLEND_REG_QOS_SW_W                                  OSD_BLEND_REG_QOS_SW_W
#define OSD_BLEND_REG_QOS_SW_W_POS                              (2U)
#define OSD_BLEND_REG_QOS_SW_W_LEN                              (1U)
#define OSD_BLEND_REG_QOS_SW_W_MSK                              (((1U<<OSD_BLEND_REG_QOS_SW_W_LEN)-1)<<OSD_BLEND_REG_QOS_SW_W_POS)
#define OSD_BLEND_REG_QOS_SW_W_UMSK                             (~(((1U<<OSD_BLEND_REG_QOS_SW_W_LEN)-1)<<OSD_BLEND_REG_QOS_SW_W_POS))
#define OSD_BLEND_REG_OSD_OSTD_MAX_W                            OSD_BLEND_REG_OSD_OSTD_MAX_W
#define OSD_BLEND_REG_OSD_OSTD_MAX_W_POS                        (4U)
#define OSD_BLEND_REG_OSD_OSTD_MAX_W_LEN                        (2U)
#define OSD_BLEND_REG_OSD_OSTD_MAX_W_MSK                        (((1U<<OSD_BLEND_REG_OSD_OSTD_MAX_W_LEN)-1)<<OSD_BLEND_REG_OSD_OSTD_MAX_W_POS)
#define OSD_BLEND_REG_OSD_OSTD_MAX_W_UMSK                       (~(((1U<<OSD_BLEND_REG_OSD_OSTD_MAX_W_LEN)-1)<<OSD_BLEND_REG_OSD_OSTD_MAX_W_POS))
#define OSD_BLEND_REG_XLEN_W                                    OSD_BLEND_REG_XLEN_W
#define OSD_BLEND_REG_XLEN_W_POS                                (8U)
#define OSD_BLEND_REG_XLEN_W_LEN                                (3U)
#define OSD_BLEND_REG_XLEN_W_MSK                                (((1U<<OSD_BLEND_REG_XLEN_W_LEN)-1)<<OSD_BLEND_REG_XLEN_W_POS)
#define OSD_BLEND_REG_XLEN_W_UMSK                               (~(((1U<<OSD_BLEND_REG_XLEN_W_LEN)-1)<<OSD_BLEND_REG_XLEN_W_POS))
#define OSD_BLEND_REG_OSD_SEOF_INT_MASK_W                       OSD_BLEND_REG_OSD_SEOF_INT_MASK_W
#define OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_POS                   (17U)
#define OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_LEN                   (1U)
#define OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_MSK                   (((1U<<OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_POS)
#define OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_UMSK                  (~(((1U<<OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_MASK_W_POS))
#define OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W                       OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W
#define OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_POS                   (18U)
#define OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_LEN                   (1U)
#define OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_MSK                   (((1U<<OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_POS)
#define OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_UMSK                  (~(((1U<<OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_LEN)-1)<<OSD_BLEND_REG_OSD_SEOF_INT_EDGE_W_POS))

/* 0x14 : obnd_layer_config0 */
#define OSD_BLEND_OBND_LAYER_CONFIG0_OFFSET                     (0x14)
#define OSD_BLEND_OBND_COLOR_FORMAT                             OSD_BLEND_OBND_COLOR_FORMAT
#define OSD_BLEND_OBND_COLOR_FORMAT_POS                         (0U)
#define OSD_BLEND_OBND_COLOR_FORMAT_LEN                         (5U)
#define OSD_BLEND_OBND_COLOR_FORMAT_MSK                         (((1U<<OSD_BLEND_OBND_COLOR_FORMAT_LEN)-1)<<OSD_BLEND_OBND_COLOR_FORMAT_POS)
#define OSD_BLEND_OBND_COLOR_FORMAT_UMSK                        (~(((1U<<OSD_BLEND_OBND_COLOR_FORMAT_LEN)-1)<<OSD_BLEND_OBND_COLOR_FORMAT_POS))
#define OSD_BLEND_OBND_ORDER_A                                  OSD_BLEND_OBND_ORDER_A
#define OSD_BLEND_OBND_ORDER_A_POS                              (8U)
#define OSD_BLEND_OBND_ORDER_A_LEN                              (2U)
#define OSD_BLEND_OBND_ORDER_A_MSK                              (((1U<<OSD_BLEND_OBND_ORDER_A_LEN)-1)<<OSD_BLEND_OBND_ORDER_A_POS)
#define OSD_BLEND_OBND_ORDER_A_UMSK                             (~(((1U<<OSD_BLEND_OBND_ORDER_A_LEN)-1)<<OSD_BLEND_OBND_ORDER_A_POS))
#define OSD_BLEND_OBND_ORDER_RV                                 OSD_BLEND_OBND_ORDER_RV
#define OSD_BLEND_OBND_ORDER_RV_POS                             (10U)
#define OSD_BLEND_OBND_ORDER_RV_LEN                             (2U)
#define OSD_BLEND_OBND_ORDER_RV_MSK                             (((1U<<OSD_BLEND_OBND_ORDER_RV_LEN)-1)<<OSD_BLEND_OBND_ORDER_RV_POS)
#define OSD_BLEND_OBND_ORDER_RV_UMSK                            (~(((1U<<OSD_BLEND_OBND_ORDER_RV_LEN)-1)<<OSD_BLEND_OBND_ORDER_RV_POS))
#define OSD_BLEND_OBND_ORDER_GY                                 OSD_BLEND_OBND_ORDER_GY
#define OSD_BLEND_OBND_ORDER_GY_POS                             (12U)
#define OSD_BLEND_OBND_ORDER_GY_LEN                             (2U)
#define OSD_BLEND_OBND_ORDER_GY_MSK                             (((1U<<OSD_BLEND_OBND_ORDER_GY_LEN)-1)<<OSD_BLEND_OBND_ORDER_GY_POS)
#define OSD_BLEND_OBND_ORDER_GY_UMSK                            (~(((1U<<OSD_BLEND_OBND_ORDER_GY_LEN)-1)<<OSD_BLEND_OBND_ORDER_GY_POS))
#define OSD_BLEND_OBND_ORDER_BU                                 OSD_BLEND_OBND_ORDER_BU
#define OSD_BLEND_OBND_ORDER_BU_POS                             (14U)
#define OSD_BLEND_OBND_ORDER_BU_LEN                             (2U)
#define OSD_BLEND_OBND_ORDER_BU_MSK                             (((1U<<OSD_BLEND_OBND_ORDER_BU_LEN)-1)<<OSD_BLEND_OBND_ORDER_BU_POS)
#define OSD_BLEND_OBND_ORDER_BU_UMSK                            (~(((1U<<OSD_BLEND_OBND_ORDER_BU_LEN)-1)<<OSD_BLEND_OBND_ORDER_BU_POS))
#define OSD_BLEND_OBND_GLOBAL_A_EN                              OSD_BLEND_OBND_GLOBAL_A_EN
#define OSD_BLEND_OBND_GLOBAL_A_EN_POS                          (16U)
#define OSD_BLEND_OBND_GLOBAL_A_EN_LEN                          (1U)
#define OSD_BLEND_OBND_GLOBAL_A_EN_MSK                          (((1U<<OSD_BLEND_OBND_GLOBAL_A_EN_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_A_EN_POS)
#define OSD_BLEND_OBND_GLOBAL_A_EN_UMSK                         (~(((1U<<OSD_BLEND_OBND_GLOBAL_A_EN_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_A_EN_POS))
#define OSD_BLEND_OBND_GLOBAL_A                                 OSD_BLEND_OBND_GLOBAL_A
#define OSD_BLEND_OBND_GLOBAL_A_POS                             (24U)
#define OSD_BLEND_OBND_GLOBAL_A_LEN                             (8U)
#define OSD_BLEND_OBND_GLOBAL_A_MSK                             (((1U<<OSD_BLEND_OBND_GLOBAL_A_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_A_POS)
#define OSD_BLEND_OBND_GLOBAL_A_UMSK                            (~(((1U<<OSD_BLEND_OBND_GLOBAL_A_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_A_POS))

/* 0x18 : obnd_layer_config1 */
#define OSD_BLEND_OBND_LAYER_CONFIG1_OFFSET                     (0x18)
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN                          OSD_BLEND_OBND_GLOBAL_COLOR_EN
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN_POS                      (0U)
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN_LEN                      (1U)
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN_MSK                      (((1U<<OSD_BLEND_OBND_GLOBAL_COLOR_EN_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_COLOR_EN_POS)
#define OSD_BLEND_OBND_GLOBAL_COLOR_EN_UMSK                     (~(((1U<<OSD_BLEND_OBND_GLOBAL_COLOR_EN_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_COLOR_EN_POS))
#define OSD_BLEND_OBND_GLOBAL_RV                                OSD_BLEND_OBND_GLOBAL_RV
#define OSD_BLEND_OBND_GLOBAL_RV_POS                            (8U)
#define OSD_BLEND_OBND_GLOBAL_RV_LEN                            (8U)
#define OSD_BLEND_OBND_GLOBAL_RV_MSK                            (((1U<<OSD_BLEND_OBND_GLOBAL_RV_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_RV_POS)
#define OSD_BLEND_OBND_GLOBAL_RV_UMSK                           (~(((1U<<OSD_BLEND_OBND_GLOBAL_RV_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_RV_POS))
#define OSD_BLEND_OBND_GLOBAL_GY                                OSD_BLEND_OBND_GLOBAL_GY
#define OSD_BLEND_OBND_GLOBAL_GY_POS                            (16U)
#define OSD_BLEND_OBND_GLOBAL_GY_LEN                            (8U)
#define OSD_BLEND_OBND_GLOBAL_GY_MSK                            (((1U<<OSD_BLEND_OBND_GLOBAL_GY_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_GY_POS)
#define OSD_BLEND_OBND_GLOBAL_GY_UMSK                           (~(((1U<<OSD_BLEND_OBND_GLOBAL_GY_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_GY_POS))
#define OSD_BLEND_OBND_GLOBAL_BU                                OSD_BLEND_OBND_GLOBAL_BU
#define OSD_BLEND_OBND_GLOBAL_BU_POS                            (24U)
#define OSD_BLEND_OBND_GLOBAL_BU_LEN                            (8U)
#define OSD_BLEND_OBND_GLOBAL_BU_MSK                            (((1U<<OSD_BLEND_OBND_GLOBAL_BU_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_BU_POS)
#define OSD_BLEND_OBND_GLOBAL_BU_UMSK                           (~(((1U<<OSD_BLEND_OBND_GLOBAL_BU_LEN)-1)<<OSD_BLEND_OBND_GLOBAL_BU_POS))

/* 0x1C : obnd_layer_config2 */
#define OSD_BLEND_OBND_LAYER_CONFIG2_OFFSET                     (0x1C)
#define OSD_BLEND_OBND_KEY_PALETTE_EN                           OSD_BLEND_OBND_KEY_PALETTE_EN
#define OSD_BLEND_OBND_KEY_PALETTE_EN_POS                       (0U)
#define OSD_BLEND_OBND_KEY_PALETTE_EN_LEN                       (1U)
#define OSD_BLEND_OBND_KEY_PALETTE_EN_MSK                       (((1U<<OSD_BLEND_OBND_KEY_PALETTE_EN_LEN)-1)<<OSD_BLEND_OBND_KEY_PALETTE_EN_POS)
#define OSD_BLEND_OBND_KEY_PALETTE_EN_UMSK                      (~(((1U<<OSD_BLEND_OBND_KEY_PALETTE_EN_LEN)-1)<<OSD_BLEND_OBND_KEY_PALETTE_EN_POS))
#define OSD_BLEND_OBND_KEY_INDEX_INV                            OSD_BLEND_OBND_KEY_INDEX_INV
#define OSD_BLEND_OBND_KEY_INDEX_INV_POS                        (1U)
#define OSD_BLEND_OBND_KEY_INDEX_INV_LEN                        (1U)
#define OSD_BLEND_OBND_KEY_INDEX_INV_MSK                        (((1U<<OSD_BLEND_OBND_KEY_INDEX_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_INV_POS)
#define OSD_BLEND_OBND_KEY_INDEX_INV_UMSK                       (~(((1U<<OSD_BLEND_OBND_KEY_INDEX_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_INV_POS))
#define OSD_BLEND_OBND_KEY_INDEX_MIN                            OSD_BLEND_OBND_KEY_INDEX_MIN
#define OSD_BLEND_OBND_KEY_INDEX_MIN_POS                        (8U)
#define OSD_BLEND_OBND_KEY_INDEX_MIN_LEN                        (8U)
#define OSD_BLEND_OBND_KEY_INDEX_MIN_MSK                        (((1U<<OSD_BLEND_OBND_KEY_INDEX_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_MIN_POS)
#define OSD_BLEND_OBND_KEY_INDEX_MIN_UMSK                       (~(((1U<<OSD_BLEND_OBND_KEY_INDEX_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_MIN_POS))
#define OSD_BLEND_OBND_KEY_INDEX_MAX                            OSD_BLEND_OBND_KEY_INDEX_MAX
#define OSD_BLEND_OBND_KEY_INDEX_MAX_POS                        (16U)
#define OSD_BLEND_OBND_KEY_INDEX_MAX_LEN                        (8U)
#define OSD_BLEND_OBND_KEY_INDEX_MAX_MSK                        (((1U<<OSD_BLEND_OBND_KEY_INDEX_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_MAX_POS)
#define OSD_BLEND_OBND_KEY_INDEX_MAX_UMSK                       (~(((1U<<OSD_BLEND_OBND_KEY_INDEX_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_INDEX_MAX_POS))
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX                        OSD_BLEND_OBND_KEY_REPLACE_INDEX
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_POS                    (24U)
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_LEN                    (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_MSK                    (((1U<<OSD_BLEND_OBND_KEY_REPLACE_INDEX_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_INDEX_POS)
#define OSD_BLEND_OBND_KEY_REPLACE_INDEX_UMSK                   (~(((1U<<OSD_BLEND_OBND_KEY_REPLACE_INDEX_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_INDEX_POS))

/* 0x20 : obnd_layer_config3 */
#define OSD_BLEND_OBND_LAYER_CONFIG3_OFFSET                     (0x20)
#define OSD_BLEND_OBND_KEY_A_MIN                                OSD_BLEND_OBND_KEY_A_MIN
#define OSD_BLEND_OBND_KEY_A_MIN_POS                            (0U)
#define OSD_BLEND_OBND_KEY_A_MIN_LEN                            (8U)
#define OSD_BLEND_OBND_KEY_A_MIN_MSK                            (((1U<<OSD_BLEND_OBND_KEY_A_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_A_MIN_POS)
#define OSD_BLEND_OBND_KEY_A_MIN_UMSK                           (~(((1U<<OSD_BLEND_OBND_KEY_A_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_A_MIN_POS))
#define OSD_BLEND_OBND_KEY_A_MAX                                OSD_BLEND_OBND_KEY_A_MAX
#define OSD_BLEND_OBND_KEY_A_MAX_POS                            (8U)
#define OSD_BLEND_OBND_KEY_A_MAX_LEN                            (8U)
#define OSD_BLEND_OBND_KEY_A_MAX_MSK                            (((1U<<OSD_BLEND_OBND_KEY_A_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_A_MAX_POS)
#define OSD_BLEND_OBND_KEY_A_MAX_UMSK                           (~(((1U<<OSD_BLEND_OBND_KEY_A_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_A_MAX_POS))
#define OSD_BLEND_OBND_KEY_RV_MIN                               OSD_BLEND_OBND_KEY_RV_MIN
#define OSD_BLEND_OBND_KEY_RV_MIN_POS                           (16U)
#define OSD_BLEND_OBND_KEY_RV_MIN_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_RV_MIN_MSK                           (((1U<<OSD_BLEND_OBND_KEY_RV_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_MIN_POS)
#define OSD_BLEND_OBND_KEY_RV_MIN_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_RV_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_MIN_POS))
#define OSD_BLEND_OBND_KEY_RV_MAX                               OSD_BLEND_OBND_KEY_RV_MAX
#define OSD_BLEND_OBND_KEY_RV_MAX_POS                           (24U)
#define OSD_BLEND_OBND_KEY_RV_MAX_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_RV_MAX_MSK                           (((1U<<OSD_BLEND_OBND_KEY_RV_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_MAX_POS)
#define OSD_BLEND_OBND_KEY_RV_MAX_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_RV_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_MAX_POS))

/* 0x24 : obnd_layer_config4 */
#define OSD_BLEND_OBND_LAYER_CONFIG4_OFFSET                     (0x24)
#define OSD_BLEND_OBND_KEY_GY_MIN                               OSD_BLEND_OBND_KEY_GY_MIN
#define OSD_BLEND_OBND_KEY_GY_MIN_POS                           (0U)
#define OSD_BLEND_OBND_KEY_GY_MIN_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_GY_MIN_MSK                           (((1U<<OSD_BLEND_OBND_KEY_GY_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_MIN_POS)
#define OSD_BLEND_OBND_KEY_GY_MIN_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_GY_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_MIN_POS))
#define OSD_BLEND_OBND_KEY_GY_MAX                               OSD_BLEND_OBND_KEY_GY_MAX
#define OSD_BLEND_OBND_KEY_GY_MAX_POS                           (8U)
#define OSD_BLEND_OBND_KEY_GY_MAX_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_GY_MAX_MSK                           (((1U<<OSD_BLEND_OBND_KEY_GY_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_MAX_POS)
#define OSD_BLEND_OBND_KEY_GY_MAX_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_GY_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_MAX_POS))
#define OSD_BLEND_OBND_KEY_BU_MIN                               OSD_BLEND_OBND_KEY_BU_MIN
#define OSD_BLEND_OBND_KEY_BU_MIN_POS                           (16U)
#define OSD_BLEND_OBND_KEY_BU_MIN_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_BU_MIN_MSK                           (((1U<<OSD_BLEND_OBND_KEY_BU_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_MIN_POS)
#define OSD_BLEND_OBND_KEY_BU_MIN_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_BU_MIN_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_MIN_POS))
#define OSD_BLEND_OBND_KEY_BU_MAX                               OSD_BLEND_OBND_KEY_BU_MAX
#define OSD_BLEND_OBND_KEY_BU_MAX_POS                           (24U)
#define OSD_BLEND_OBND_KEY_BU_MAX_LEN                           (8U)
#define OSD_BLEND_OBND_KEY_BU_MAX_MSK                           (((1U<<OSD_BLEND_OBND_KEY_BU_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_MAX_POS)
#define OSD_BLEND_OBND_KEY_BU_MAX_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_BU_MAX_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_MAX_POS))

/* 0x28 : obnd_layer_config5 */
#define OSD_BLEND_OBND_LAYER_CONFIG5_OFFSET                     (0x28)
#define OSD_BLEND_OBND_KEY_REPLACE_A                            OSD_BLEND_OBND_KEY_REPLACE_A
#define OSD_BLEND_OBND_KEY_REPLACE_A_POS                        (0U)
#define OSD_BLEND_OBND_KEY_REPLACE_A_LEN                        (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_A_MSK                        (((1U<<OSD_BLEND_OBND_KEY_REPLACE_A_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_A_POS)
#define OSD_BLEND_OBND_KEY_REPLACE_A_UMSK                       (~(((1U<<OSD_BLEND_OBND_KEY_REPLACE_A_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_A_POS))
#define OSD_BLEND_OBND_KEY_REPLACE_RV                           OSD_BLEND_OBND_KEY_REPLACE_RV
#define OSD_BLEND_OBND_KEY_REPLACE_RV_POS                       (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_RV_LEN                       (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_RV_MSK                       (((1U<<OSD_BLEND_OBND_KEY_REPLACE_RV_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_RV_POS)
#define OSD_BLEND_OBND_KEY_REPLACE_RV_UMSK                      (~(((1U<<OSD_BLEND_OBND_KEY_REPLACE_RV_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_RV_POS))
#define OSD_BLEND_OBND_KEY_REPLACE_GY                           OSD_BLEND_OBND_KEY_REPLACE_GY
#define OSD_BLEND_OBND_KEY_REPLACE_GY_POS                       (16U)
#define OSD_BLEND_OBND_KEY_REPLACE_GY_LEN                       (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_GY_MSK                       (((1U<<OSD_BLEND_OBND_KEY_REPLACE_GY_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_GY_POS)
#define OSD_BLEND_OBND_KEY_REPLACE_GY_UMSK                      (~(((1U<<OSD_BLEND_OBND_KEY_REPLACE_GY_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_GY_POS))
#define OSD_BLEND_OBND_KEY_REPLACE_BU                           OSD_BLEND_OBND_KEY_REPLACE_BU
#define OSD_BLEND_OBND_KEY_REPLACE_BU_POS                       (24U)
#define OSD_BLEND_OBND_KEY_REPLACE_BU_LEN                       (8U)
#define OSD_BLEND_OBND_KEY_REPLACE_BU_MSK                       (((1U<<OSD_BLEND_OBND_KEY_REPLACE_BU_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_BU_POS)
#define OSD_BLEND_OBND_KEY_REPLACE_BU_UMSK                      (~(((1U<<OSD_BLEND_OBND_KEY_REPLACE_BU_LEN)-1)<<OSD_BLEND_OBND_KEY_REPLACE_BU_POS))

/* 0x2C : obnd_layer_config6 */
#define OSD_BLEND_OBND_LAYER_CONFIG6_OFFSET                     (0x2C)
#define OSD_BLEND_OBND_KEY_COLOR_EN                             OSD_BLEND_OBND_KEY_COLOR_EN
#define OSD_BLEND_OBND_KEY_COLOR_EN_POS                         (0U)
#define OSD_BLEND_OBND_KEY_COLOR_EN_LEN                         (1U)
#define OSD_BLEND_OBND_KEY_COLOR_EN_MSK                         (((1U<<OSD_BLEND_OBND_KEY_COLOR_EN_LEN)-1)<<OSD_BLEND_OBND_KEY_COLOR_EN_POS)
#define OSD_BLEND_OBND_KEY_COLOR_EN_UMSK                        (~(((1U<<OSD_BLEND_OBND_KEY_COLOR_EN_LEN)-1)<<OSD_BLEND_OBND_KEY_COLOR_EN_POS))
#define OSD_BLEND_OBND_KEY_A_INV                                OSD_BLEND_OBND_KEY_A_INV
#define OSD_BLEND_OBND_KEY_A_INV_POS                            (1U)
#define OSD_BLEND_OBND_KEY_A_INV_LEN                            (1U)
#define OSD_BLEND_OBND_KEY_A_INV_MSK                            (((1U<<OSD_BLEND_OBND_KEY_A_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_A_INV_POS)
#define OSD_BLEND_OBND_KEY_A_INV_UMSK                           (~(((1U<<OSD_BLEND_OBND_KEY_A_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_A_INV_POS))
#define OSD_BLEND_OBND_KEY_RV_INV                               OSD_BLEND_OBND_KEY_RV_INV
#define OSD_BLEND_OBND_KEY_RV_INV_POS                           (2U)
#define OSD_BLEND_OBND_KEY_RV_INV_LEN                           (1U)
#define OSD_BLEND_OBND_KEY_RV_INV_MSK                           (((1U<<OSD_BLEND_OBND_KEY_RV_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_INV_POS)
#define OSD_BLEND_OBND_KEY_RV_INV_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_RV_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_RV_INV_POS))
#define OSD_BLEND_OBND_KEY_GY_INV                               OSD_BLEND_OBND_KEY_GY_INV
#define OSD_BLEND_OBND_KEY_GY_INV_POS                           (3U)
#define OSD_BLEND_OBND_KEY_GY_INV_LEN                           (1U)
#define OSD_BLEND_OBND_KEY_GY_INV_MSK                           (((1U<<OSD_BLEND_OBND_KEY_GY_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_INV_POS)
#define OSD_BLEND_OBND_KEY_GY_INV_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_GY_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_GY_INV_POS))
#define OSD_BLEND_OBND_KEY_BU_INV                               OSD_BLEND_OBND_KEY_BU_INV
#define OSD_BLEND_OBND_KEY_BU_INV_POS                           (4U)
#define OSD_BLEND_OBND_KEY_BU_INV_LEN                           (1U)
#define OSD_BLEND_OBND_KEY_BU_INV_MSK                           (((1U<<OSD_BLEND_OBND_KEY_BU_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_INV_POS)
#define OSD_BLEND_OBND_KEY_BU_INV_UMSK                          (~(((1U<<OSD_BLEND_OBND_KEY_BU_INV_LEN)-1)<<OSD_BLEND_OBND_KEY_BU_INV_POS))

/* 0x34 : obnd_layer_config8 */
#define OSD_BLEND_OBND_LAYER_CONFIG8_OFFSET                     (0x34)
#define OSD_BLEND_OBND_ALPHA_0                                  OSD_BLEND_OBND_ALPHA_0
#define OSD_BLEND_OBND_ALPHA_0_POS                              (0U)
#define OSD_BLEND_OBND_ALPHA_0_LEN                              (8U)
#define OSD_BLEND_OBND_ALPHA_0_MSK                              (((1U<<OSD_BLEND_OBND_ALPHA_0_LEN)-1)<<OSD_BLEND_OBND_ALPHA_0_POS)
#define OSD_BLEND_OBND_ALPHA_0_UMSK                             (~(((1U<<OSD_BLEND_OBND_ALPHA_0_LEN)-1)<<OSD_BLEND_OBND_ALPHA_0_POS))
#define OSD_BLEND_OBND_ALPHA_1                                  OSD_BLEND_OBND_ALPHA_1
#define OSD_BLEND_OBND_ALPHA_1_POS                              (8U)
#define OSD_BLEND_OBND_ALPHA_1_LEN                              (8U)
#define OSD_BLEND_OBND_ALPHA_1_MSK                              (((1U<<OSD_BLEND_OBND_ALPHA_1_LEN)-1)<<OSD_BLEND_OBND_ALPHA_1_POS)
#define OSD_BLEND_OBND_ALPHA_1_UMSK                             (~(((1U<<OSD_BLEND_OBND_ALPHA_1_LEN)-1)<<OSD_BLEND_OBND_ALPHA_1_POS))

/* 0x40 : obnd_error */
#define OSD_BLEND_OBND_ERROR_OFFSET                             (0x40)
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W                         OSD_BLEND_REG_RFIFO_DRAIN_CLR_W
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_POS                     (0U)
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_LEN                     (1U)
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_MSK                     (((1U<<OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_LEN)-1)<<OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_POS)
#define OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_UMSK                    (~(((1U<<OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_LEN)-1)<<OSD_BLEND_REG_RFIFO_DRAIN_CLR_W_POS))
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W                        OSD_BLEND_REG_RFIFO_DRAIN_MASK_W
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_POS                    (1U)
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_LEN                    (1U)
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_MSK                    (((1U<<OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_LEN)-1)<<OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_POS)
#define OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_UMSK                   (~(((1U<<OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_LEN)-1)<<OSD_BLEND_REG_RFIFO_DRAIN_MASK_W_POS))
#define OSD_BLEND_STS_RFIFO_DRAIN_R                             OSD_BLEND_STS_RFIFO_DRAIN_R
#define OSD_BLEND_STS_RFIFO_DRAIN_R_POS                         (16U)
#define OSD_BLEND_STS_RFIFO_DRAIN_R_LEN                         (1U)
#define OSD_BLEND_STS_RFIFO_DRAIN_R_MSK                         (((1U<<OSD_BLEND_STS_RFIFO_DRAIN_R_LEN)-1)<<OSD_BLEND_STS_RFIFO_DRAIN_R_POS)
#define OSD_BLEND_STS_RFIFO_DRAIN_R_UMSK                        (~(((1U<<OSD_BLEND_STS_RFIFO_DRAIN_R_LEN)-1)<<OSD_BLEND_STS_RFIFO_DRAIN_R_POS))

/* 0x44 : obnd_sh */
#define OSD_BLEND_OBND_SH_OFFSET                                (0x44)
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT                        OSD_BLEND_OBND_LAYER_MEM_REQ_CNT
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_POS                    (0U)
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_LEN                    (32U)
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_MSK                    (((1U<<OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_LEN)-1)<<OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_POS)
#define OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_UMSK                   (~(((1U<<OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_LEN)-1)<<OSD_BLEND_OBND_LAYER_MEM_REQ_CNT_POS))

/* 0x60 : obnd_mem_config1 */
#define OSD_BLEND_OBND_MEM_CONFIG1_OFFSET                       (0x60)
#define OSD_BLEND_REG_ADDR_START                                OSD_BLEND_REG_ADDR_START
#define OSD_BLEND_REG_ADDR_START_POS                            (0U)
#define OSD_BLEND_REG_ADDR_START_LEN                            (32U)
#define OSD_BLEND_REG_ADDR_START_MSK                            (((1U<<OSD_BLEND_REG_ADDR_START_LEN)-1)<<OSD_BLEND_REG_ADDR_START_POS)
#define OSD_BLEND_REG_ADDR_START_UMSK                           (~(((1U<<OSD_BLEND_REG_ADDR_START_LEN)-1)<<OSD_BLEND_REG_ADDR_START_POS))

/* 0x64 : obnd_mem_config2 */
#define OSD_BLEND_OBND_MEM_CONFIG2_OFFSET                       (0x64)
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8                       OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_POS                   (0U)
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_LEN                   (14U)
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_MSK                   (((1U<<OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_LEN)-1)<<OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_POS)
#define OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_UMSK                  (~(((1U<<OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_LEN)-1)<<OSD_BLEND_REG_FRAME_WIDTH_BYTE_X8_POS))
#define OSD_BLEND_REG_STRIDE_BYTE_X8                            OSD_BLEND_REG_STRIDE_BYTE_X8
#define OSD_BLEND_REG_STRIDE_BYTE_X8_POS                        (16U)
#define OSD_BLEND_REG_STRIDE_BYTE_X8_LEN                        (14U)
#define OSD_BLEND_REG_STRIDE_BYTE_X8_MSK                        (((1U<<OSD_BLEND_REG_STRIDE_BYTE_X8_LEN)-1)<<OSD_BLEND_REG_STRIDE_BYTE_X8_POS)
#define OSD_BLEND_REG_STRIDE_BYTE_X8_UMSK                       (~(((1U<<OSD_BLEND_REG_STRIDE_BYTE_X8_LEN)-1)<<OSD_BLEND_REG_STRIDE_BYTE_X8_POS))

/* 0x68 : obnd_mem_config3 */
#define OSD_BLEND_OBND_MEM_CONFIG3_OFFSET                       (0x68)
#define OSD_BLEND_REG_FRAME_HEIGHT                              OSD_BLEND_REG_FRAME_HEIGHT
#define OSD_BLEND_REG_FRAME_HEIGHT_POS                          (0U)
#define OSD_BLEND_REG_FRAME_HEIGHT_LEN                          (14U)
#define OSD_BLEND_REG_FRAME_HEIGHT_MSK                          (((1U<<OSD_BLEND_REG_FRAME_HEIGHT_LEN)-1)<<OSD_BLEND_REG_FRAME_HEIGHT_POS)
#define OSD_BLEND_REG_FRAME_HEIGHT_UMSK                         (~(((1U<<OSD_BLEND_REG_FRAME_HEIGHT_LEN)-1)<<OSD_BLEND_REG_FRAME_HEIGHT_POS))
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT                         OSD_BLEND_REG_LINE_HEAD_FIX_BIT
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_POS                     (16U)
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_LEN                     (6U)
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_MSK                     (((1U<<OSD_BLEND_REG_LINE_HEAD_FIX_BIT_LEN)-1)<<OSD_BLEND_REG_LINE_HEAD_FIX_BIT_POS)
#define OSD_BLEND_REG_LINE_HEAD_FIX_BIT_UMSK                    (~(((1U<<OSD_BLEND_REG_LINE_HEAD_FIX_BIT_LEN)-1)<<OSD_BLEND_REG_LINE_HEAD_FIX_BIT_POS))
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT                         OSD_BLEND_REG_LINE_TAIL_FIX_BIT
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_POS                     (24U)
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_LEN                     (6U)
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_MSK                     (((1U<<OSD_BLEND_REG_LINE_TAIL_FIX_BIT_LEN)-1)<<OSD_BLEND_REG_LINE_TAIL_FIX_BIT_POS)
#define OSD_BLEND_REG_LINE_TAIL_FIX_BIT_UMSK                    (~(((1U<<OSD_BLEND_REG_LINE_TAIL_FIX_BIT_LEN)-1)<<OSD_BLEND_REG_LINE_TAIL_FIX_BIT_POS))

/* 0x400 : PALE_SRAM_00 */
#define OSD_BLEND_PALE_SRAM_00_OFFSET                           (0x400)
#define OSD_BLEND_PALETTE_WORD_00                               OSD_BLEND_PALETTE_WORD_00
#define OSD_BLEND_PALETTE_WORD_00_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_00_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_00_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_00_LEN)-1)<<OSD_BLEND_PALETTE_WORD_00_POS)
#define OSD_BLEND_PALETTE_WORD_00_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_00_LEN)-1)<<OSD_BLEND_PALETTE_WORD_00_POS))

/* 0x404 : PALE_SRAM_01 */
#define OSD_BLEND_PALE_SRAM_01_OFFSET                           (0x404)
#define OSD_BLEND_PALETTE_WORD_01                               OSD_BLEND_PALETTE_WORD_01
#define OSD_BLEND_PALETTE_WORD_01_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_01_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_01_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_01_LEN)-1)<<OSD_BLEND_PALETTE_WORD_01_POS)
#define OSD_BLEND_PALETTE_WORD_01_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_01_LEN)-1)<<OSD_BLEND_PALETTE_WORD_01_POS))

/* 0x408 : PALE_SRAM_02 */
#define OSD_BLEND_PALE_SRAM_02_OFFSET                           (0x408)
#define OSD_BLEND_PALETTE_WORD_02                               OSD_BLEND_PALETTE_WORD_02
#define OSD_BLEND_PALETTE_WORD_02_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_02_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_02_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_02_LEN)-1)<<OSD_BLEND_PALETTE_WORD_02_POS)
#define OSD_BLEND_PALETTE_WORD_02_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_02_LEN)-1)<<OSD_BLEND_PALETTE_WORD_02_POS))

/* 0x40C : PALE_SRAM_03 */
#define OSD_BLEND_PALE_SRAM_03_OFFSET                           (0x40C)
#define OSD_BLEND_PALETTE_WORD_03                               OSD_BLEND_PALETTE_WORD_03
#define OSD_BLEND_PALETTE_WORD_03_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_03_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_03_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_03_LEN)-1)<<OSD_BLEND_PALETTE_WORD_03_POS)
#define OSD_BLEND_PALETTE_WORD_03_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_03_LEN)-1)<<OSD_BLEND_PALETTE_WORD_03_POS))

/* 0x410 : PALE_SRAM_04 */
#define OSD_BLEND_PALE_SRAM_04_OFFSET                           (0x410)
#define OSD_BLEND_PALETTE_WORD_04                               OSD_BLEND_PALETTE_WORD_04
#define OSD_BLEND_PALETTE_WORD_04_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_04_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_04_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_04_LEN)-1)<<OSD_BLEND_PALETTE_WORD_04_POS)
#define OSD_BLEND_PALETTE_WORD_04_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_04_LEN)-1)<<OSD_BLEND_PALETTE_WORD_04_POS))

/* 0x414 : PALE_SRAM_05 */
#define OSD_BLEND_PALE_SRAM_05_OFFSET                           (0x414)
#define OSD_BLEND_PALETTE_WORD_05                               OSD_BLEND_PALETTE_WORD_05
#define OSD_BLEND_PALETTE_WORD_05_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_05_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_05_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_05_LEN)-1)<<OSD_BLEND_PALETTE_WORD_05_POS)
#define OSD_BLEND_PALETTE_WORD_05_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_05_LEN)-1)<<OSD_BLEND_PALETTE_WORD_05_POS))

/* 0x418 : PALE_SRAM_06 */
#define OSD_BLEND_PALE_SRAM_06_OFFSET                           (0x418)
#define OSD_BLEND_PALETTE_WORD_06                               OSD_BLEND_PALETTE_WORD_06
#define OSD_BLEND_PALETTE_WORD_06_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_06_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_06_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_06_LEN)-1)<<OSD_BLEND_PALETTE_WORD_06_POS)
#define OSD_BLEND_PALETTE_WORD_06_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_06_LEN)-1)<<OSD_BLEND_PALETTE_WORD_06_POS))

/* 0x41C : PALE_SRAM_07 */
#define OSD_BLEND_PALE_SRAM_07_OFFSET                           (0x41C)
#define OSD_BLEND_PALETTE_WORD_07                               OSD_BLEND_PALETTE_WORD_07
#define OSD_BLEND_PALETTE_WORD_07_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_07_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_07_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_07_LEN)-1)<<OSD_BLEND_PALETTE_WORD_07_POS)
#define OSD_BLEND_PALETTE_WORD_07_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_07_LEN)-1)<<OSD_BLEND_PALETTE_WORD_07_POS))

/* 0x420 : PALE_SRAM_08 */
#define OSD_BLEND_PALE_SRAM_08_OFFSET                           (0x420)
#define OSD_BLEND_PALETTE_WORD_08                               OSD_BLEND_PALETTE_WORD_08
#define OSD_BLEND_PALETTE_WORD_08_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_08_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_08_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_08_LEN)-1)<<OSD_BLEND_PALETTE_WORD_08_POS)
#define OSD_BLEND_PALETTE_WORD_08_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_08_LEN)-1)<<OSD_BLEND_PALETTE_WORD_08_POS))

/* 0x424 : PALE_SRAM_09 */
#define OSD_BLEND_PALE_SRAM_09_OFFSET                           (0x424)
#define OSD_BLEND_PALETTE_WORD_09                               OSD_BLEND_PALETTE_WORD_09
#define OSD_BLEND_PALETTE_WORD_09_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_09_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_09_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_09_LEN)-1)<<OSD_BLEND_PALETTE_WORD_09_POS)
#define OSD_BLEND_PALETTE_WORD_09_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_09_LEN)-1)<<OSD_BLEND_PALETTE_WORD_09_POS))

/* 0x428 : PALE_SRAM_0A */
#define OSD_BLEND_PALE_SRAM_0A_OFFSET                           (0x428)
#define OSD_BLEND_PALETTE_WORD_0A                               OSD_BLEND_PALETTE_WORD_0A
#define OSD_BLEND_PALETTE_WORD_0A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0A_POS)
#define OSD_BLEND_PALETTE_WORD_0A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0A_POS))

/* 0x42C : PALE_SRAM_0B */
#define OSD_BLEND_PALE_SRAM_0B_OFFSET                           (0x42C)
#define OSD_BLEND_PALETTE_WORD_0B                               OSD_BLEND_PALETTE_WORD_0B
#define OSD_BLEND_PALETTE_WORD_0B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0B_POS)
#define OSD_BLEND_PALETTE_WORD_0B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0B_POS))

/* 0x430 : PALE_SRAM_0C */
#define OSD_BLEND_PALE_SRAM_0C_OFFSET                           (0x430)
#define OSD_BLEND_PALETTE_WORD_0C                               OSD_BLEND_PALETTE_WORD_0C
#define OSD_BLEND_PALETTE_WORD_0C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0C_POS)
#define OSD_BLEND_PALETTE_WORD_0C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0C_POS))

/* 0x434 : PALE_SRAM_0D */
#define OSD_BLEND_PALE_SRAM_0D_OFFSET                           (0x434)
#define OSD_BLEND_PALETTE_WORD_0D                               OSD_BLEND_PALETTE_WORD_0D
#define OSD_BLEND_PALETTE_WORD_0D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0D_POS)
#define OSD_BLEND_PALETTE_WORD_0D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0D_POS))

/* 0x438 : PALE_SRAM_0E */
#define OSD_BLEND_PALE_SRAM_0E_OFFSET                           (0x438)
#define OSD_BLEND_PALETTE_WORD_0E                               OSD_BLEND_PALETTE_WORD_0E
#define OSD_BLEND_PALETTE_WORD_0E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0E_POS)
#define OSD_BLEND_PALETTE_WORD_0E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0E_POS))

/* 0x43C : PALE_SRAM_0F */
#define OSD_BLEND_PALE_SRAM_0F_OFFSET                           (0x43C)
#define OSD_BLEND_PALETTE_WORD_0F                               OSD_BLEND_PALETTE_WORD_0F
#define OSD_BLEND_PALETTE_WORD_0F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_0F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_0F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_0F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0F_POS)
#define OSD_BLEND_PALETTE_WORD_0F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_0F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_0F_POS))

/* 0x440 : PALE_SRAM_10 */
#define OSD_BLEND_PALE_SRAM_10_OFFSET                           (0x440)
#define OSD_BLEND_PALETTE_WORD_10                               OSD_BLEND_PALETTE_WORD_10
#define OSD_BLEND_PALETTE_WORD_10_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_10_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_10_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_10_LEN)-1)<<OSD_BLEND_PALETTE_WORD_10_POS)
#define OSD_BLEND_PALETTE_WORD_10_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_10_LEN)-1)<<OSD_BLEND_PALETTE_WORD_10_POS))

/* 0x444 : PALE_SRAM_11 */
#define OSD_BLEND_PALE_SRAM_11_OFFSET                           (0x444)
#define OSD_BLEND_PALETTE_WORD_11                               OSD_BLEND_PALETTE_WORD_11
#define OSD_BLEND_PALETTE_WORD_11_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_11_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_11_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_11_LEN)-1)<<OSD_BLEND_PALETTE_WORD_11_POS)
#define OSD_BLEND_PALETTE_WORD_11_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_11_LEN)-1)<<OSD_BLEND_PALETTE_WORD_11_POS))

/* 0x448 : PALE_SRAM_12 */
#define OSD_BLEND_PALE_SRAM_12_OFFSET                           (0x448)
#define OSD_BLEND_PALETTE_WORD_12                               OSD_BLEND_PALETTE_WORD_12
#define OSD_BLEND_PALETTE_WORD_12_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_12_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_12_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_12_LEN)-1)<<OSD_BLEND_PALETTE_WORD_12_POS)
#define OSD_BLEND_PALETTE_WORD_12_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_12_LEN)-1)<<OSD_BLEND_PALETTE_WORD_12_POS))

/* 0x44C : PALE_SRAM_13 */
#define OSD_BLEND_PALE_SRAM_13_OFFSET                           (0x44C)
#define OSD_BLEND_PALETTE_WORD_13                               OSD_BLEND_PALETTE_WORD_13
#define OSD_BLEND_PALETTE_WORD_13_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_13_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_13_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_13_LEN)-1)<<OSD_BLEND_PALETTE_WORD_13_POS)
#define OSD_BLEND_PALETTE_WORD_13_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_13_LEN)-1)<<OSD_BLEND_PALETTE_WORD_13_POS))

/* 0x450 : PALE_SRAM_14 */
#define OSD_BLEND_PALE_SRAM_14_OFFSET                           (0x450)
#define OSD_BLEND_PALETTE_WORD_14                               OSD_BLEND_PALETTE_WORD_14
#define OSD_BLEND_PALETTE_WORD_14_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_14_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_14_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_14_LEN)-1)<<OSD_BLEND_PALETTE_WORD_14_POS)
#define OSD_BLEND_PALETTE_WORD_14_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_14_LEN)-1)<<OSD_BLEND_PALETTE_WORD_14_POS))

/* 0x454 : PALE_SRAM_15 */
#define OSD_BLEND_PALE_SRAM_15_OFFSET                           (0x454)
#define OSD_BLEND_PALETTE_WORD_15                               OSD_BLEND_PALETTE_WORD_15
#define OSD_BLEND_PALETTE_WORD_15_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_15_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_15_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_15_LEN)-1)<<OSD_BLEND_PALETTE_WORD_15_POS)
#define OSD_BLEND_PALETTE_WORD_15_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_15_LEN)-1)<<OSD_BLEND_PALETTE_WORD_15_POS))

/* 0x458 : PALE_SRAM_16 */
#define OSD_BLEND_PALE_SRAM_16_OFFSET                           (0x458)
#define OSD_BLEND_PALETTE_WORD_16                               OSD_BLEND_PALETTE_WORD_16
#define OSD_BLEND_PALETTE_WORD_16_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_16_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_16_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_16_LEN)-1)<<OSD_BLEND_PALETTE_WORD_16_POS)
#define OSD_BLEND_PALETTE_WORD_16_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_16_LEN)-1)<<OSD_BLEND_PALETTE_WORD_16_POS))

/* 0x45C : PALE_SRAM_17 */
#define OSD_BLEND_PALE_SRAM_17_OFFSET                           (0x45C)
#define OSD_BLEND_PALETTE_WORD_17                               OSD_BLEND_PALETTE_WORD_17
#define OSD_BLEND_PALETTE_WORD_17_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_17_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_17_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_17_LEN)-1)<<OSD_BLEND_PALETTE_WORD_17_POS)
#define OSD_BLEND_PALETTE_WORD_17_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_17_LEN)-1)<<OSD_BLEND_PALETTE_WORD_17_POS))

/* 0x460 : PALE_SRAM_18 */
#define OSD_BLEND_PALE_SRAM_18_OFFSET                           (0x460)
#define OSD_BLEND_PALETTE_WORD_18                               OSD_BLEND_PALETTE_WORD_18
#define OSD_BLEND_PALETTE_WORD_18_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_18_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_18_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_18_LEN)-1)<<OSD_BLEND_PALETTE_WORD_18_POS)
#define OSD_BLEND_PALETTE_WORD_18_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_18_LEN)-1)<<OSD_BLEND_PALETTE_WORD_18_POS))

/* 0x464 : PALE_SRAM_19 */
#define OSD_BLEND_PALE_SRAM_19_OFFSET                           (0x464)
#define OSD_BLEND_PALETTE_WORD_19                               OSD_BLEND_PALETTE_WORD_19
#define OSD_BLEND_PALETTE_WORD_19_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_19_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_19_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_19_LEN)-1)<<OSD_BLEND_PALETTE_WORD_19_POS)
#define OSD_BLEND_PALETTE_WORD_19_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_19_LEN)-1)<<OSD_BLEND_PALETTE_WORD_19_POS))

/* 0x468 : PALE_SRAM_1A */
#define OSD_BLEND_PALE_SRAM_1A_OFFSET                           (0x468)
#define OSD_BLEND_PALETTE_WORD_1A                               OSD_BLEND_PALETTE_WORD_1A
#define OSD_BLEND_PALETTE_WORD_1A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1A_POS)
#define OSD_BLEND_PALETTE_WORD_1A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1A_POS))

/* 0x46C : PALE_SRAM_1B */
#define OSD_BLEND_PALE_SRAM_1B_OFFSET                           (0x46C)
#define OSD_BLEND_PALETTE_WORD_1B                               OSD_BLEND_PALETTE_WORD_1B
#define OSD_BLEND_PALETTE_WORD_1B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1B_POS)
#define OSD_BLEND_PALETTE_WORD_1B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1B_POS))

/* 0x470 : PALE_SRAM_1C */
#define OSD_BLEND_PALE_SRAM_1C_OFFSET                           (0x470)
#define OSD_BLEND_PALETTE_WORD_1C                               OSD_BLEND_PALETTE_WORD_1C
#define OSD_BLEND_PALETTE_WORD_1C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1C_POS)
#define OSD_BLEND_PALETTE_WORD_1C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1C_POS))

/* 0x474 : PALE_SRAM_1D */
#define OSD_BLEND_PALE_SRAM_1D_OFFSET                           (0x474)
#define OSD_BLEND_PALETTE_WORD_1D                               OSD_BLEND_PALETTE_WORD_1D
#define OSD_BLEND_PALETTE_WORD_1D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1D_POS)
#define OSD_BLEND_PALETTE_WORD_1D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1D_POS))

/* 0x478 : PALE_SRAM_1E */
#define OSD_BLEND_PALE_SRAM_1E_OFFSET                           (0x478)
#define OSD_BLEND_PALETTE_WORD_1E                               OSD_BLEND_PALETTE_WORD_1E
#define OSD_BLEND_PALETTE_WORD_1E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1E_POS)
#define OSD_BLEND_PALETTE_WORD_1E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1E_POS))

/* 0x47C : PALE_SRAM_1F */
#define OSD_BLEND_PALE_SRAM_1F_OFFSET                           (0x47C)
#define OSD_BLEND_PALETTE_WORD_1F                               OSD_BLEND_PALETTE_WORD_1F
#define OSD_BLEND_PALETTE_WORD_1F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_1F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_1F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_1F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1F_POS)
#define OSD_BLEND_PALETTE_WORD_1F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_1F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_1F_POS))

/* 0x480 : PALE_SRAM_20 */
#define OSD_BLEND_PALE_SRAM_20_OFFSET                           (0x480)
#define OSD_BLEND_PALETTE_WORD_20                               OSD_BLEND_PALETTE_WORD_20
#define OSD_BLEND_PALETTE_WORD_20_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_20_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_20_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_20_LEN)-1)<<OSD_BLEND_PALETTE_WORD_20_POS)
#define OSD_BLEND_PALETTE_WORD_20_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_20_LEN)-1)<<OSD_BLEND_PALETTE_WORD_20_POS))

/* 0x484 : PALE_SRAM_21 */
#define OSD_BLEND_PALE_SRAM_21_OFFSET                           (0x484)
#define OSD_BLEND_PALETTE_WORD_21                               OSD_BLEND_PALETTE_WORD_21
#define OSD_BLEND_PALETTE_WORD_21_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_21_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_21_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_21_LEN)-1)<<OSD_BLEND_PALETTE_WORD_21_POS)
#define OSD_BLEND_PALETTE_WORD_21_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_21_LEN)-1)<<OSD_BLEND_PALETTE_WORD_21_POS))

/* 0x488 : PALE_SRAM_22 */
#define OSD_BLEND_PALE_SRAM_22_OFFSET                           (0x488)
#define OSD_BLEND_PALETTE_WORD_22                               OSD_BLEND_PALETTE_WORD_22
#define OSD_BLEND_PALETTE_WORD_22_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_22_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_22_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_22_LEN)-1)<<OSD_BLEND_PALETTE_WORD_22_POS)
#define OSD_BLEND_PALETTE_WORD_22_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_22_LEN)-1)<<OSD_BLEND_PALETTE_WORD_22_POS))

/* 0x48C : PALE_SRAM_23 */
#define OSD_BLEND_PALE_SRAM_23_OFFSET                           (0x48C)
#define OSD_BLEND_PALETTE_WORD_23                               OSD_BLEND_PALETTE_WORD_23
#define OSD_BLEND_PALETTE_WORD_23_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_23_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_23_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_23_LEN)-1)<<OSD_BLEND_PALETTE_WORD_23_POS)
#define OSD_BLEND_PALETTE_WORD_23_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_23_LEN)-1)<<OSD_BLEND_PALETTE_WORD_23_POS))

/* 0x490 : PALE_SRAM_24 */
#define OSD_BLEND_PALE_SRAM_24_OFFSET                           (0x490)
#define OSD_BLEND_PALETTE_WORD_24                               OSD_BLEND_PALETTE_WORD_24
#define OSD_BLEND_PALETTE_WORD_24_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_24_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_24_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_24_LEN)-1)<<OSD_BLEND_PALETTE_WORD_24_POS)
#define OSD_BLEND_PALETTE_WORD_24_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_24_LEN)-1)<<OSD_BLEND_PALETTE_WORD_24_POS))

/* 0x494 : PALE_SRAM_25 */
#define OSD_BLEND_PALE_SRAM_25_OFFSET                           (0x494)
#define OSD_BLEND_PALETTE_WORD_25                               OSD_BLEND_PALETTE_WORD_25
#define OSD_BLEND_PALETTE_WORD_25_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_25_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_25_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_25_LEN)-1)<<OSD_BLEND_PALETTE_WORD_25_POS)
#define OSD_BLEND_PALETTE_WORD_25_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_25_LEN)-1)<<OSD_BLEND_PALETTE_WORD_25_POS))

/* 0x498 : PALE_SRAM_26 */
#define OSD_BLEND_PALE_SRAM_26_OFFSET                           (0x498)
#define OSD_BLEND_PALETTE_WORD_26                               OSD_BLEND_PALETTE_WORD_26
#define OSD_BLEND_PALETTE_WORD_26_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_26_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_26_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_26_LEN)-1)<<OSD_BLEND_PALETTE_WORD_26_POS)
#define OSD_BLEND_PALETTE_WORD_26_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_26_LEN)-1)<<OSD_BLEND_PALETTE_WORD_26_POS))

/* 0x49C : PALE_SRAM_27 */
#define OSD_BLEND_PALE_SRAM_27_OFFSET                           (0x49C)
#define OSD_BLEND_PALETTE_WORD_27                               OSD_BLEND_PALETTE_WORD_27
#define OSD_BLEND_PALETTE_WORD_27_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_27_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_27_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_27_LEN)-1)<<OSD_BLEND_PALETTE_WORD_27_POS)
#define OSD_BLEND_PALETTE_WORD_27_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_27_LEN)-1)<<OSD_BLEND_PALETTE_WORD_27_POS))

/* 0x4A0 : PALE_SRAM_28 */
#define OSD_BLEND_PALE_SRAM_28_OFFSET                           (0x4A0)
#define OSD_BLEND_PALETTE_WORD_28                               OSD_BLEND_PALETTE_WORD_28
#define OSD_BLEND_PALETTE_WORD_28_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_28_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_28_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_28_LEN)-1)<<OSD_BLEND_PALETTE_WORD_28_POS)
#define OSD_BLEND_PALETTE_WORD_28_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_28_LEN)-1)<<OSD_BLEND_PALETTE_WORD_28_POS))

/* 0x4A4 : PALE_SRAM_29 */
#define OSD_BLEND_PALE_SRAM_29_OFFSET                           (0x4A4)
#define OSD_BLEND_PALETTE_WORD_29                               OSD_BLEND_PALETTE_WORD_29
#define OSD_BLEND_PALETTE_WORD_29_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_29_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_29_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_29_LEN)-1)<<OSD_BLEND_PALETTE_WORD_29_POS)
#define OSD_BLEND_PALETTE_WORD_29_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_29_LEN)-1)<<OSD_BLEND_PALETTE_WORD_29_POS))

/* 0x4A8 : PALE_SRAM_2A */
#define OSD_BLEND_PALE_SRAM_2A_OFFSET                           (0x4A8)
#define OSD_BLEND_PALETTE_WORD_2A                               OSD_BLEND_PALETTE_WORD_2A
#define OSD_BLEND_PALETTE_WORD_2A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2A_POS)
#define OSD_BLEND_PALETTE_WORD_2A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2A_POS))

/* 0x4AC : PALE_SRAM_2B */
#define OSD_BLEND_PALE_SRAM_2B_OFFSET                           (0x4AC)
#define OSD_BLEND_PALETTE_WORD_2B                               OSD_BLEND_PALETTE_WORD_2B
#define OSD_BLEND_PALETTE_WORD_2B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2B_POS)
#define OSD_BLEND_PALETTE_WORD_2B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2B_POS))

/* 0x4B0 : PALE_SRAM_2C */
#define OSD_BLEND_PALE_SRAM_2C_OFFSET                           (0x4B0)
#define OSD_BLEND_PALETTE_WORD_2C                               OSD_BLEND_PALETTE_WORD_2C
#define OSD_BLEND_PALETTE_WORD_2C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2C_POS)
#define OSD_BLEND_PALETTE_WORD_2C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2C_POS))

/* 0x4B4 : PALE_SRAM_2D */
#define OSD_BLEND_PALE_SRAM_2D_OFFSET                           (0x4B4)
#define OSD_BLEND_PALETTE_WORD_2D                               OSD_BLEND_PALETTE_WORD_2D
#define OSD_BLEND_PALETTE_WORD_2D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2D_POS)
#define OSD_BLEND_PALETTE_WORD_2D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2D_POS))

/* 0x4B8 : PALE_SRAM_2E */
#define OSD_BLEND_PALE_SRAM_2E_OFFSET                           (0x4B8)
#define OSD_BLEND_PALETTE_WORD_2E                               OSD_BLEND_PALETTE_WORD_2E
#define OSD_BLEND_PALETTE_WORD_2E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2E_POS)
#define OSD_BLEND_PALETTE_WORD_2E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2E_POS))

/* 0x4BC : PALE_SRAM_2F */
#define OSD_BLEND_PALE_SRAM_2F_OFFSET                           (0x4BC)
#define OSD_BLEND_PALETTE_WORD_2F                               OSD_BLEND_PALETTE_WORD_2F
#define OSD_BLEND_PALETTE_WORD_2F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_2F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_2F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_2F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2F_POS)
#define OSD_BLEND_PALETTE_WORD_2F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_2F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_2F_POS))

/* 0x4C0 : PALE_SRAM_30 */
#define OSD_BLEND_PALE_SRAM_30_OFFSET                           (0x4C0)
#define OSD_BLEND_PALETTE_WORD_30                               OSD_BLEND_PALETTE_WORD_30
#define OSD_BLEND_PALETTE_WORD_30_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_30_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_30_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_30_LEN)-1)<<OSD_BLEND_PALETTE_WORD_30_POS)
#define OSD_BLEND_PALETTE_WORD_30_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_30_LEN)-1)<<OSD_BLEND_PALETTE_WORD_30_POS))

/* 0x4C4 : PALE_SRAM_31 */
#define OSD_BLEND_PALE_SRAM_31_OFFSET                           (0x4C4)
#define OSD_BLEND_PALETTE_WORD_31                               OSD_BLEND_PALETTE_WORD_31
#define OSD_BLEND_PALETTE_WORD_31_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_31_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_31_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_31_LEN)-1)<<OSD_BLEND_PALETTE_WORD_31_POS)
#define OSD_BLEND_PALETTE_WORD_31_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_31_LEN)-1)<<OSD_BLEND_PALETTE_WORD_31_POS))

/* 0x4C8 : PALE_SRAM_32 */
#define OSD_BLEND_PALE_SRAM_32_OFFSET                           (0x4C8)
#define OSD_BLEND_PALETTE_WORD_32                               OSD_BLEND_PALETTE_WORD_32
#define OSD_BLEND_PALETTE_WORD_32_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_32_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_32_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_32_LEN)-1)<<OSD_BLEND_PALETTE_WORD_32_POS)
#define OSD_BLEND_PALETTE_WORD_32_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_32_LEN)-1)<<OSD_BLEND_PALETTE_WORD_32_POS))

/* 0x4CC : PALE_SRAM_33 */
#define OSD_BLEND_PALE_SRAM_33_OFFSET                           (0x4CC)
#define OSD_BLEND_PALETTE_WORD_33                               OSD_BLEND_PALETTE_WORD_33
#define OSD_BLEND_PALETTE_WORD_33_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_33_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_33_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_33_LEN)-1)<<OSD_BLEND_PALETTE_WORD_33_POS)
#define OSD_BLEND_PALETTE_WORD_33_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_33_LEN)-1)<<OSD_BLEND_PALETTE_WORD_33_POS))

/* 0x4D0 : PALE_SRAM_34 */
#define OSD_BLEND_PALE_SRAM_34_OFFSET                           (0x4D0)
#define OSD_BLEND_PALETTE_WORD_34                               OSD_BLEND_PALETTE_WORD_34
#define OSD_BLEND_PALETTE_WORD_34_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_34_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_34_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_34_LEN)-1)<<OSD_BLEND_PALETTE_WORD_34_POS)
#define OSD_BLEND_PALETTE_WORD_34_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_34_LEN)-1)<<OSD_BLEND_PALETTE_WORD_34_POS))

/* 0x4D4 : PALE_SRAM_35 */
#define OSD_BLEND_PALE_SRAM_35_OFFSET                           (0x4D4)
#define OSD_BLEND_PALETTE_WORD_35                               OSD_BLEND_PALETTE_WORD_35
#define OSD_BLEND_PALETTE_WORD_35_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_35_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_35_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_35_LEN)-1)<<OSD_BLEND_PALETTE_WORD_35_POS)
#define OSD_BLEND_PALETTE_WORD_35_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_35_LEN)-1)<<OSD_BLEND_PALETTE_WORD_35_POS))

/* 0x4D8 : PALE_SRAM_36 */
#define OSD_BLEND_PALE_SRAM_36_OFFSET                           (0x4D8)
#define OSD_BLEND_PALETTE_WORD_36                               OSD_BLEND_PALETTE_WORD_36
#define OSD_BLEND_PALETTE_WORD_36_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_36_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_36_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_36_LEN)-1)<<OSD_BLEND_PALETTE_WORD_36_POS)
#define OSD_BLEND_PALETTE_WORD_36_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_36_LEN)-1)<<OSD_BLEND_PALETTE_WORD_36_POS))

/* 0x4DC : PALE_SRAM_37 */
#define OSD_BLEND_PALE_SRAM_37_OFFSET                           (0x4DC)
#define OSD_BLEND_PALETTE_WORD_37                               OSD_BLEND_PALETTE_WORD_37
#define OSD_BLEND_PALETTE_WORD_37_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_37_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_37_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_37_LEN)-1)<<OSD_BLEND_PALETTE_WORD_37_POS)
#define OSD_BLEND_PALETTE_WORD_37_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_37_LEN)-1)<<OSD_BLEND_PALETTE_WORD_37_POS))

/* 0x4E0 : PALE_SRAM_38 */
#define OSD_BLEND_PALE_SRAM_38_OFFSET                           (0x4E0)
#define OSD_BLEND_PALETTE_WORD_38                               OSD_BLEND_PALETTE_WORD_38
#define OSD_BLEND_PALETTE_WORD_38_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_38_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_38_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_38_LEN)-1)<<OSD_BLEND_PALETTE_WORD_38_POS)
#define OSD_BLEND_PALETTE_WORD_38_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_38_LEN)-1)<<OSD_BLEND_PALETTE_WORD_38_POS))

/* 0x4E4 : PALE_SRAM_39 */
#define OSD_BLEND_PALE_SRAM_39_OFFSET                           (0x4E4)
#define OSD_BLEND_PALETTE_WORD_39                               OSD_BLEND_PALETTE_WORD_39
#define OSD_BLEND_PALETTE_WORD_39_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_39_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_39_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_39_LEN)-1)<<OSD_BLEND_PALETTE_WORD_39_POS)
#define OSD_BLEND_PALETTE_WORD_39_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_39_LEN)-1)<<OSD_BLEND_PALETTE_WORD_39_POS))

/* 0x4E8 : PALE_SRAM_3A */
#define OSD_BLEND_PALE_SRAM_3A_OFFSET                           (0x4E8)
#define OSD_BLEND_PALETTE_WORD_3A                               OSD_BLEND_PALETTE_WORD_3A
#define OSD_BLEND_PALETTE_WORD_3A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3A_POS)
#define OSD_BLEND_PALETTE_WORD_3A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3A_POS))

/* 0x4EC : PALE_SRAM_3B */
#define OSD_BLEND_PALE_SRAM_3B_OFFSET                           (0x4EC)
#define OSD_BLEND_PALETTE_WORD_3B                               OSD_BLEND_PALETTE_WORD_3B
#define OSD_BLEND_PALETTE_WORD_3B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3B_POS)
#define OSD_BLEND_PALETTE_WORD_3B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3B_POS))

/* 0x4F0 : PALE_SRAM_3C */
#define OSD_BLEND_PALE_SRAM_3C_OFFSET                           (0x4F0)
#define OSD_BLEND_PALETTE_WORD_3C                               OSD_BLEND_PALETTE_WORD_3C
#define OSD_BLEND_PALETTE_WORD_3C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3C_POS)
#define OSD_BLEND_PALETTE_WORD_3C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3C_POS))

/* 0x4F4 : PALE_SRAM_3D */
#define OSD_BLEND_PALE_SRAM_3D_OFFSET                           (0x4F4)
#define OSD_BLEND_PALETTE_WORD_3D                               OSD_BLEND_PALETTE_WORD_3D
#define OSD_BLEND_PALETTE_WORD_3D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3D_POS)
#define OSD_BLEND_PALETTE_WORD_3D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3D_POS))

/* 0x4F8 : PALE_SRAM_3E */
#define OSD_BLEND_PALE_SRAM_3E_OFFSET                           (0x4F8)
#define OSD_BLEND_PALETTE_WORD_3E                               OSD_BLEND_PALETTE_WORD_3E
#define OSD_BLEND_PALETTE_WORD_3E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3E_POS)
#define OSD_BLEND_PALETTE_WORD_3E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3E_POS))

/* 0x4FC : PALE_SRAM_3F */
#define OSD_BLEND_PALE_SRAM_3F_OFFSET                           (0x4FC)
#define OSD_BLEND_PALETTE_WORD_3F                               OSD_BLEND_PALETTE_WORD_3F
#define OSD_BLEND_PALETTE_WORD_3F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_3F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_3F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_3F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3F_POS)
#define OSD_BLEND_PALETTE_WORD_3F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_3F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_3F_POS))

/* 0x500 : PALE_SRAM_40 */
#define OSD_BLEND_PALE_SRAM_40_OFFSET                           (0x500)
#define OSD_BLEND_PALETTE_WORD_40                               OSD_BLEND_PALETTE_WORD_40
#define OSD_BLEND_PALETTE_WORD_40_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_40_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_40_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_40_LEN)-1)<<OSD_BLEND_PALETTE_WORD_40_POS)
#define OSD_BLEND_PALETTE_WORD_40_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_40_LEN)-1)<<OSD_BLEND_PALETTE_WORD_40_POS))

/* 0x504 : PALE_SRAM_41 */
#define OSD_BLEND_PALE_SRAM_41_OFFSET                           (0x504)
#define OSD_BLEND_PALETTE_WORD_41                               OSD_BLEND_PALETTE_WORD_41
#define OSD_BLEND_PALETTE_WORD_41_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_41_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_41_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_41_LEN)-1)<<OSD_BLEND_PALETTE_WORD_41_POS)
#define OSD_BLEND_PALETTE_WORD_41_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_41_LEN)-1)<<OSD_BLEND_PALETTE_WORD_41_POS))

/* 0x508 : PALE_SRAM_42 */
#define OSD_BLEND_PALE_SRAM_42_OFFSET                           (0x508)
#define OSD_BLEND_PALETTE_WORD_42                               OSD_BLEND_PALETTE_WORD_42
#define OSD_BLEND_PALETTE_WORD_42_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_42_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_42_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_42_LEN)-1)<<OSD_BLEND_PALETTE_WORD_42_POS)
#define OSD_BLEND_PALETTE_WORD_42_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_42_LEN)-1)<<OSD_BLEND_PALETTE_WORD_42_POS))

/* 0x50C : PALE_SRAM_43 */
#define OSD_BLEND_PALE_SRAM_43_OFFSET                           (0x50C)
#define OSD_BLEND_PALETTE_WORD_43                               OSD_BLEND_PALETTE_WORD_43
#define OSD_BLEND_PALETTE_WORD_43_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_43_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_43_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_43_LEN)-1)<<OSD_BLEND_PALETTE_WORD_43_POS)
#define OSD_BLEND_PALETTE_WORD_43_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_43_LEN)-1)<<OSD_BLEND_PALETTE_WORD_43_POS))

/* 0x510 : PALE_SRAM_44 */
#define OSD_BLEND_PALE_SRAM_44_OFFSET                           (0x510)
#define OSD_BLEND_PALETTE_WORD_44                               OSD_BLEND_PALETTE_WORD_44
#define OSD_BLEND_PALETTE_WORD_44_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_44_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_44_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_44_LEN)-1)<<OSD_BLEND_PALETTE_WORD_44_POS)
#define OSD_BLEND_PALETTE_WORD_44_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_44_LEN)-1)<<OSD_BLEND_PALETTE_WORD_44_POS))

/* 0x514 : PALE_SRAM_45 */
#define OSD_BLEND_PALE_SRAM_45_OFFSET                           (0x514)
#define OSD_BLEND_PALETTE_WORD_45                               OSD_BLEND_PALETTE_WORD_45
#define OSD_BLEND_PALETTE_WORD_45_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_45_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_45_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_45_LEN)-1)<<OSD_BLEND_PALETTE_WORD_45_POS)
#define OSD_BLEND_PALETTE_WORD_45_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_45_LEN)-1)<<OSD_BLEND_PALETTE_WORD_45_POS))

/* 0x518 : PALE_SRAM_46 */
#define OSD_BLEND_PALE_SRAM_46_OFFSET                           (0x518)
#define OSD_BLEND_PALETTE_WORD_46                               OSD_BLEND_PALETTE_WORD_46
#define OSD_BLEND_PALETTE_WORD_46_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_46_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_46_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_46_LEN)-1)<<OSD_BLEND_PALETTE_WORD_46_POS)
#define OSD_BLEND_PALETTE_WORD_46_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_46_LEN)-1)<<OSD_BLEND_PALETTE_WORD_46_POS))

/* 0x51C : PALE_SRAM_47 */
#define OSD_BLEND_PALE_SRAM_47_OFFSET                           (0x51C)
#define OSD_BLEND_PALETTE_WORD_47                               OSD_BLEND_PALETTE_WORD_47
#define OSD_BLEND_PALETTE_WORD_47_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_47_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_47_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_47_LEN)-1)<<OSD_BLEND_PALETTE_WORD_47_POS)
#define OSD_BLEND_PALETTE_WORD_47_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_47_LEN)-1)<<OSD_BLEND_PALETTE_WORD_47_POS))

/* 0x520 : PALE_SRAM_48 */
#define OSD_BLEND_PALE_SRAM_48_OFFSET                           (0x520)
#define OSD_BLEND_PALETTE_WORD_48                               OSD_BLEND_PALETTE_WORD_48
#define OSD_BLEND_PALETTE_WORD_48_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_48_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_48_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_48_LEN)-1)<<OSD_BLEND_PALETTE_WORD_48_POS)
#define OSD_BLEND_PALETTE_WORD_48_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_48_LEN)-1)<<OSD_BLEND_PALETTE_WORD_48_POS))

/* 0x524 : PALE_SRAM_49 */
#define OSD_BLEND_PALE_SRAM_49_OFFSET                           (0x524)
#define OSD_BLEND_PALETTE_WORD_49                               OSD_BLEND_PALETTE_WORD_49
#define OSD_BLEND_PALETTE_WORD_49_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_49_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_49_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_49_LEN)-1)<<OSD_BLEND_PALETTE_WORD_49_POS)
#define OSD_BLEND_PALETTE_WORD_49_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_49_LEN)-1)<<OSD_BLEND_PALETTE_WORD_49_POS))

/* 0x528 : PALE_SRAM_4A */
#define OSD_BLEND_PALE_SRAM_4A_OFFSET                           (0x528)
#define OSD_BLEND_PALETTE_WORD_4A                               OSD_BLEND_PALETTE_WORD_4A
#define OSD_BLEND_PALETTE_WORD_4A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4A_POS)
#define OSD_BLEND_PALETTE_WORD_4A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4A_POS))

/* 0x52C : PALE_SRAM_4B */
#define OSD_BLEND_PALE_SRAM_4B_OFFSET                           (0x52C)
#define OSD_BLEND_PALETTE_WORD_4B                               OSD_BLEND_PALETTE_WORD_4B
#define OSD_BLEND_PALETTE_WORD_4B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4B_POS)
#define OSD_BLEND_PALETTE_WORD_4B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4B_POS))

/* 0x530 : PALE_SRAM_4C */
#define OSD_BLEND_PALE_SRAM_4C_OFFSET                           (0x530)
#define OSD_BLEND_PALETTE_WORD_4C                               OSD_BLEND_PALETTE_WORD_4C
#define OSD_BLEND_PALETTE_WORD_4C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4C_POS)
#define OSD_BLEND_PALETTE_WORD_4C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4C_POS))

/* 0x534 : PALE_SRAM_4D */
#define OSD_BLEND_PALE_SRAM_4D_OFFSET                           (0x534)
#define OSD_BLEND_PALETTE_WORD_4D                               OSD_BLEND_PALETTE_WORD_4D
#define OSD_BLEND_PALETTE_WORD_4D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4D_POS)
#define OSD_BLEND_PALETTE_WORD_4D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4D_POS))

/* 0x538 : PALE_SRAM_4E */
#define OSD_BLEND_PALE_SRAM_4E_OFFSET                           (0x538)
#define OSD_BLEND_PALETTE_WORD_4E                               OSD_BLEND_PALETTE_WORD_4E
#define OSD_BLEND_PALETTE_WORD_4E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4E_POS)
#define OSD_BLEND_PALETTE_WORD_4E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4E_POS))

/* 0x53C : PALE_SRAM_4F */
#define OSD_BLEND_PALE_SRAM_4F_OFFSET                           (0x53C)
#define OSD_BLEND_PALETTE_WORD_4F                               OSD_BLEND_PALETTE_WORD_4F
#define OSD_BLEND_PALETTE_WORD_4F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_4F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_4F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_4F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4F_POS)
#define OSD_BLEND_PALETTE_WORD_4F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_4F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_4F_POS))

/* 0x540 : PALE_SRAM_50 */
#define OSD_BLEND_PALE_SRAM_50_OFFSET                           (0x540)
#define OSD_BLEND_PALETTE_WORD_50                               OSD_BLEND_PALETTE_WORD_50
#define OSD_BLEND_PALETTE_WORD_50_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_50_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_50_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_50_LEN)-1)<<OSD_BLEND_PALETTE_WORD_50_POS)
#define OSD_BLEND_PALETTE_WORD_50_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_50_LEN)-1)<<OSD_BLEND_PALETTE_WORD_50_POS))

/* 0x544 : PALE_SRAM_51 */
#define OSD_BLEND_PALE_SRAM_51_OFFSET                           (0x544)
#define OSD_BLEND_PALETTE_WORD_51                               OSD_BLEND_PALETTE_WORD_51
#define OSD_BLEND_PALETTE_WORD_51_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_51_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_51_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_51_LEN)-1)<<OSD_BLEND_PALETTE_WORD_51_POS)
#define OSD_BLEND_PALETTE_WORD_51_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_51_LEN)-1)<<OSD_BLEND_PALETTE_WORD_51_POS))

/* 0x548 : PALE_SRAM_52 */
#define OSD_BLEND_PALE_SRAM_52_OFFSET                           (0x548)
#define OSD_BLEND_PALETTE_WORD_52                               OSD_BLEND_PALETTE_WORD_52
#define OSD_BLEND_PALETTE_WORD_52_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_52_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_52_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_52_LEN)-1)<<OSD_BLEND_PALETTE_WORD_52_POS)
#define OSD_BLEND_PALETTE_WORD_52_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_52_LEN)-1)<<OSD_BLEND_PALETTE_WORD_52_POS))

/* 0x54C : PALE_SRAM_53 */
#define OSD_BLEND_PALE_SRAM_53_OFFSET                           (0x54C)
#define OSD_BLEND_PALETTE_WORD_53                               OSD_BLEND_PALETTE_WORD_53
#define OSD_BLEND_PALETTE_WORD_53_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_53_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_53_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_53_LEN)-1)<<OSD_BLEND_PALETTE_WORD_53_POS)
#define OSD_BLEND_PALETTE_WORD_53_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_53_LEN)-1)<<OSD_BLEND_PALETTE_WORD_53_POS))

/* 0x550 : PALE_SRAM_54 */
#define OSD_BLEND_PALE_SRAM_54_OFFSET                           (0x550)
#define OSD_BLEND_PALETTE_WORD_54                               OSD_BLEND_PALETTE_WORD_54
#define OSD_BLEND_PALETTE_WORD_54_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_54_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_54_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_54_LEN)-1)<<OSD_BLEND_PALETTE_WORD_54_POS)
#define OSD_BLEND_PALETTE_WORD_54_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_54_LEN)-1)<<OSD_BLEND_PALETTE_WORD_54_POS))

/* 0x554 : PALE_SRAM_55 */
#define OSD_BLEND_PALE_SRAM_55_OFFSET                           (0x554)
#define OSD_BLEND_PALETTE_WORD_55                               OSD_BLEND_PALETTE_WORD_55
#define OSD_BLEND_PALETTE_WORD_55_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_55_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_55_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_55_LEN)-1)<<OSD_BLEND_PALETTE_WORD_55_POS)
#define OSD_BLEND_PALETTE_WORD_55_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_55_LEN)-1)<<OSD_BLEND_PALETTE_WORD_55_POS))

/* 0x558 : PALE_SRAM_56 */
#define OSD_BLEND_PALE_SRAM_56_OFFSET                           (0x558)
#define OSD_BLEND_PALETTE_WORD_56                               OSD_BLEND_PALETTE_WORD_56
#define OSD_BLEND_PALETTE_WORD_56_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_56_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_56_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_56_LEN)-1)<<OSD_BLEND_PALETTE_WORD_56_POS)
#define OSD_BLEND_PALETTE_WORD_56_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_56_LEN)-1)<<OSD_BLEND_PALETTE_WORD_56_POS))

/* 0x55C : PALE_SRAM_57 */
#define OSD_BLEND_PALE_SRAM_57_OFFSET                           (0x55C)
#define OSD_BLEND_PALETTE_WORD_57                               OSD_BLEND_PALETTE_WORD_57
#define OSD_BLEND_PALETTE_WORD_57_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_57_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_57_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_57_LEN)-1)<<OSD_BLEND_PALETTE_WORD_57_POS)
#define OSD_BLEND_PALETTE_WORD_57_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_57_LEN)-1)<<OSD_BLEND_PALETTE_WORD_57_POS))

/* 0x560 : PALE_SRAM_58 */
#define OSD_BLEND_PALE_SRAM_58_OFFSET                           (0x560)
#define OSD_BLEND_PALETTE_WORD_58                               OSD_BLEND_PALETTE_WORD_58
#define OSD_BLEND_PALETTE_WORD_58_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_58_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_58_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_58_LEN)-1)<<OSD_BLEND_PALETTE_WORD_58_POS)
#define OSD_BLEND_PALETTE_WORD_58_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_58_LEN)-1)<<OSD_BLEND_PALETTE_WORD_58_POS))

/* 0x564 : PALE_SRAM_59 */
#define OSD_BLEND_PALE_SRAM_59_OFFSET                           (0x564)
#define OSD_BLEND_PALETTE_WORD_59                               OSD_BLEND_PALETTE_WORD_59
#define OSD_BLEND_PALETTE_WORD_59_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_59_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_59_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_59_LEN)-1)<<OSD_BLEND_PALETTE_WORD_59_POS)
#define OSD_BLEND_PALETTE_WORD_59_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_59_LEN)-1)<<OSD_BLEND_PALETTE_WORD_59_POS))

/* 0x568 : PALE_SRAM_5A */
#define OSD_BLEND_PALE_SRAM_5A_OFFSET                           (0x568)
#define OSD_BLEND_PALETTE_WORD_5A                               OSD_BLEND_PALETTE_WORD_5A
#define OSD_BLEND_PALETTE_WORD_5A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5A_POS)
#define OSD_BLEND_PALETTE_WORD_5A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5A_POS))

/* 0x56C : PALE_SRAM_5B */
#define OSD_BLEND_PALE_SRAM_5B_OFFSET                           (0x56C)
#define OSD_BLEND_PALETTE_WORD_5B                               OSD_BLEND_PALETTE_WORD_5B
#define OSD_BLEND_PALETTE_WORD_5B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5B_POS)
#define OSD_BLEND_PALETTE_WORD_5B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5B_POS))

/* 0x570 : PALE_SRAM_5C */
#define OSD_BLEND_PALE_SRAM_5C_OFFSET                           (0x570)
#define OSD_BLEND_PALETTE_WORD_5C                               OSD_BLEND_PALETTE_WORD_5C
#define OSD_BLEND_PALETTE_WORD_5C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5C_POS)
#define OSD_BLEND_PALETTE_WORD_5C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5C_POS))

/* 0x574 : PALE_SRAM_5D */
#define OSD_BLEND_PALE_SRAM_5D_OFFSET                           (0x574)
#define OSD_BLEND_PALETTE_WORD_5D                               OSD_BLEND_PALETTE_WORD_5D
#define OSD_BLEND_PALETTE_WORD_5D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5D_POS)
#define OSD_BLEND_PALETTE_WORD_5D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5D_POS))

/* 0x578 : PALE_SRAM_5E */
#define OSD_BLEND_PALE_SRAM_5E_OFFSET                           (0x578)
#define OSD_BLEND_PALETTE_WORD_5E                               OSD_BLEND_PALETTE_WORD_5E
#define OSD_BLEND_PALETTE_WORD_5E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5E_POS)
#define OSD_BLEND_PALETTE_WORD_5E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5E_POS))

/* 0x57C : PALE_SRAM_5F */
#define OSD_BLEND_PALE_SRAM_5F_OFFSET                           (0x57C)
#define OSD_BLEND_PALETTE_WORD_5F                               OSD_BLEND_PALETTE_WORD_5F
#define OSD_BLEND_PALETTE_WORD_5F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_5F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_5F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_5F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5F_POS)
#define OSD_BLEND_PALETTE_WORD_5F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_5F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_5F_POS))

/* 0x580 : PALE_SRAM_60 */
#define OSD_BLEND_PALE_SRAM_60_OFFSET                           (0x580)
#define OSD_BLEND_PALETTE_WORD_60                               OSD_BLEND_PALETTE_WORD_60
#define OSD_BLEND_PALETTE_WORD_60_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_60_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_60_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_60_LEN)-1)<<OSD_BLEND_PALETTE_WORD_60_POS)
#define OSD_BLEND_PALETTE_WORD_60_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_60_LEN)-1)<<OSD_BLEND_PALETTE_WORD_60_POS))

/* 0x584 : PALE_SRAM_61 */
#define OSD_BLEND_PALE_SRAM_61_OFFSET                           (0x584)
#define OSD_BLEND_PALETTE_WORD_61                               OSD_BLEND_PALETTE_WORD_61
#define OSD_BLEND_PALETTE_WORD_61_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_61_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_61_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_61_LEN)-1)<<OSD_BLEND_PALETTE_WORD_61_POS)
#define OSD_BLEND_PALETTE_WORD_61_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_61_LEN)-1)<<OSD_BLEND_PALETTE_WORD_61_POS))

/* 0x588 : PALE_SRAM_62 */
#define OSD_BLEND_PALE_SRAM_62_OFFSET                           (0x588)
#define OSD_BLEND_PALETTE_WORD_62                               OSD_BLEND_PALETTE_WORD_62
#define OSD_BLEND_PALETTE_WORD_62_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_62_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_62_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_62_LEN)-1)<<OSD_BLEND_PALETTE_WORD_62_POS)
#define OSD_BLEND_PALETTE_WORD_62_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_62_LEN)-1)<<OSD_BLEND_PALETTE_WORD_62_POS))

/* 0x58C : PALE_SRAM_63 */
#define OSD_BLEND_PALE_SRAM_63_OFFSET                           (0x58C)
#define OSD_BLEND_PALETTE_WORD_63                               OSD_BLEND_PALETTE_WORD_63
#define OSD_BLEND_PALETTE_WORD_63_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_63_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_63_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_63_LEN)-1)<<OSD_BLEND_PALETTE_WORD_63_POS)
#define OSD_BLEND_PALETTE_WORD_63_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_63_LEN)-1)<<OSD_BLEND_PALETTE_WORD_63_POS))

/* 0x590 : PALE_SRAM_64 */
#define OSD_BLEND_PALE_SRAM_64_OFFSET                           (0x590)
#define OSD_BLEND_PALETTE_WORD_64                               OSD_BLEND_PALETTE_WORD_64
#define OSD_BLEND_PALETTE_WORD_64_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_64_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_64_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_64_LEN)-1)<<OSD_BLEND_PALETTE_WORD_64_POS)
#define OSD_BLEND_PALETTE_WORD_64_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_64_LEN)-1)<<OSD_BLEND_PALETTE_WORD_64_POS))

/* 0x594 : PALE_SRAM_65 */
#define OSD_BLEND_PALE_SRAM_65_OFFSET                           (0x594)
#define OSD_BLEND_PALETTE_WORD_65                               OSD_BLEND_PALETTE_WORD_65
#define OSD_BLEND_PALETTE_WORD_65_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_65_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_65_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_65_LEN)-1)<<OSD_BLEND_PALETTE_WORD_65_POS)
#define OSD_BLEND_PALETTE_WORD_65_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_65_LEN)-1)<<OSD_BLEND_PALETTE_WORD_65_POS))

/* 0x598 : PALE_SRAM_66 */
#define OSD_BLEND_PALE_SRAM_66_OFFSET                           (0x598)
#define OSD_BLEND_PALETTE_WORD_66                               OSD_BLEND_PALETTE_WORD_66
#define OSD_BLEND_PALETTE_WORD_66_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_66_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_66_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_66_LEN)-1)<<OSD_BLEND_PALETTE_WORD_66_POS)
#define OSD_BLEND_PALETTE_WORD_66_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_66_LEN)-1)<<OSD_BLEND_PALETTE_WORD_66_POS))

/* 0x59C : PALE_SRAM_67 */
#define OSD_BLEND_PALE_SRAM_67_OFFSET                           (0x59C)
#define OSD_BLEND_PALETTE_WORD_67                               OSD_BLEND_PALETTE_WORD_67
#define OSD_BLEND_PALETTE_WORD_67_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_67_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_67_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_67_LEN)-1)<<OSD_BLEND_PALETTE_WORD_67_POS)
#define OSD_BLEND_PALETTE_WORD_67_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_67_LEN)-1)<<OSD_BLEND_PALETTE_WORD_67_POS))

/* 0x5A0 : PALE_SRAM_68 */
#define OSD_BLEND_PALE_SRAM_68_OFFSET                           (0x5A0)
#define OSD_BLEND_PALETTE_WORD_68                               OSD_BLEND_PALETTE_WORD_68
#define OSD_BLEND_PALETTE_WORD_68_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_68_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_68_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_68_LEN)-1)<<OSD_BLEND_PALETTE_WORD_68_POS)
#define OSD_BLEND_PALETTE_WORD_68_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_68_LEN)-1)<<OSD_BLEND_PALETTE_WORD_68_POS))

/* 0x5A4 : PALE_SRAM_69 */
#define OSD_BLEND_PALE_SRAM_69_OFFSET                           (0x5A4)
#define OSD_BLEND_PALETTE_WORD_69                               OSD_BLEND_PALETTE_WORD_69
#define OSD_BLEND_PALETTE_WORD_69_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_69_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_69_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_69_LEN)-1)<<OSD_BLEND_PALETTE_WORD_69_POS)
#define OSD_BLEND_PALETTE_WORD_69_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_69_LEN)-1)<<OSD_BLEND_PALETTE_WORD_69_POS))

/* 0x5A8 : PALE_SRAM_6A */
#define OSD_BLEND_PALE_SRAM_6A_OFFSET                           (0x5A8)
#define OSD_BLEND_PALETTE_WORD_6A                               OSD_BLEND_PALETTE_WORD_6A
#define OSD_BLEND_PALETTE_WORD_6A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6A_POS)
#define OSD_BLEND_PALETTE_WORD_6A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6A_POS))

/* 0x5AC : PALE_SRAM_6B */
#define OSD_BLEND_PALE_SRAM_6B_OFFSET                           (0x5AC)
#define OSD_BLEND_PALETTE_WORD_6B                               OSD_BLEND_PALETTE_WORD_6B
#define OSD_BLEND_PALETTE_WORD_6B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6B_POS)
#define OSD_BLEND_PALETTE_WORD_6B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6B_POS))

/* 0x5B0 : PALE_SRAM_6C */
#define OSD_BLEND_PALE_SRAM_6C_OFFSET                           (0x5B0)
#define OSD_BLEND_PALETTE_WORD_6C                               OSD_BLEND_PALETTE_WORD_6C
#define OSD_BLEND_PALETTE_WORD_6C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6C_POS)
#define OSD_BLEND_PALETTE_WORD_6C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6C_POS))

/* 0x5B4 : PALE_SRAM_6D */
#define OSD_BLEND_PALE_SRAM_6D_OFFSET                           (0x5B4)
#define OSD_BLEND_PALETTE_WORD_6D                               OSD_BLEND_PALETTE_WORD_6D
#define OSD_BLEND_PALETTE_WORD_6D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6D_POS)
#define OSD_BLEND_PALETTE_WORD_6D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6D_POS))

/* 0x5B8 : PALE_SRAM_6E */
#define OSD_BLEND_PALE_SRAM_6E_OFFSET                           (0x5B8)
#define OSD_BLEND_PALETTE_WORD_6E                               OSD_BLEND_PALETTE_WORD_6E
#define OSD_BLEND_PALETTE_WORD_6E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6E_POS)
#define OSD_BLEND_PALETTE_WORD_6E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6E_POS))

/* 0x5BC : PALE_SRAM_6F */
#define OSD_BLEND_PALE_SRAM_6F_OFFSET                           (0x5BC)
#define OSD_BLEND_PALETTE_WORD_6F                               OSD_BLEND_PALETTE_WORD_6F
#define OSD_BLEND_PALETTE_WORD_6F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_6F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_6F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_6F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6F_POS)
#define OSD_BLEND_PALETTE_WORD_6F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_6F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_6F_POS))

/* 0x5C0 : PALE_SRAM_70 */
#define OSD_BLEND_PALE_SRAM_70_OFFSET                           (0x5C0)
#define OSD_BLEND_PALETTE_WORD_70                               OSD_BLEND_PALETTE_WORD_70
#define OSD_BLEND_PALETTE_WORD_70_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_70_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_70_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_70_LEN)-1)<<OSD_BLEND_PALETTE_WORD_70_POS)
#define OSD_BLEND_PALETTE_WORD_70_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_70_LEN)-1)<<OSD_BLEND_PALETTE_WORD_70_POS))

/* 0x5C4 : PALE_SRAM_71 */
#define OSD_BLEND_PALE_SRAM_71_OFFSET                           (0x5C4)
#define OSD_BLEND_PALETTE_WORD_71                               OSD_BLEND_PALETTE_WORD_71
#define OSD_BLEND_PALETTE_WORD_71_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_71_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_71_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_71_LEN)-1)<<OSD_BLEND_PALETTE_WORD_71_POS)
#define OSD_BLEND_PALETTE_WORD_71_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_71_LEN)-1)<<OSD_BLEND_PALETTE_WORD_71_POS))

/* 0x5C8 : PALE_SRAM_72 */
#define OSD_BLEND_PALE_SRAM_72_OFFSET                           (0x5C8)
#define OSD_BLEND_PALETTE_WORD_72                               OSD_BLEND_PALETTE_WORD_72
#define OSD_BLEND_PALETTE_WORD_72_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_72_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_72_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_72_LEN)-1)<<OSD_BLEND_PALETTE_WORD_72_POS)
#define OSD_BLEND_PALETTE_WORD_72_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_72_LEN)-1)<<OSD_BLEND_PALETTE_WORD_72_POS))

/* 0x5CC : PALE_SRAM_73 */
#define OSD_BLEND_PALE_SRAM_73_OFFSET                           (0x5CC)
#define OSD_BLEND_PALETTE_WORD_73                               OSD_BLEND_PALETTE_WORD_73
#define OSD_BLEND_PALETTE_WORD_73_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_73_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_73_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_73_LEN)-1)<<OSD_BLEND_PALETTE_WORD_73_POS)
#define OSD_BLEND_PALETTE_WORD_73_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_73_LEN)-1)<<OSD_BLEND_PALETTE_WORD_73_POS))

/* 0x5D0 : PALE_SRAM_74 */
#define OSD_BLEND_PALE_SRAM_74_OFFSET                           (0x5D0)
#define OSD_BLEND_PALETTE_WORD_74                               OSD_BLEND_PALETTE_WORD_74
#define OSD_BLEND_PALETTE_WORD_74_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_74_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_74_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_74_LEN)-1)<<OSD_BLEND_PALETTE_WORD_74_POS)
#define OSD_BLEND_PALETTE_WORD_74_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_74_LEN)-1)<<OSD_BLEND_PALETTE_WORD_74_POS))

/* 0x5D4 : PALE_SRAM_75 */
#define OSD_BLEND_PALE_SRAM_75_OFFSET                           (0x5D4)
#define OSD_BLEND_PALETTE_WORD_75                               OSD_BLEND_PALETTE_WORD_75
#define OSD_BLEND_PALETTE_WORD_75_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_75_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_75_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_75_LEN)-1)<<OSD_BLEND_PALETTE_WORD_75_POS)
#define OSD_BLEND_PALETTE_WORD_75_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_75_LEN)-1)<<OSD_BLEND_PALETTE_WORD_75_POS))

/* 0x5D8 : PALE_SRAM_76 */
#define OSD_BLEND_PALE_SRAM_76_OFFSET                           (0x5D8)
#define OSD_BLEND_PALETTE_WORD_76                               OSD_BLEND_PALETTE_WORD_76
#define OSD_BLEND_PALETTE_WORD_76_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_76_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_76_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_76_LEN)-1)<<OSD_BLEND_PALETTE_WORD_76_POS)
#define OSD_BLEND_PALETTE_WORD_76_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_76_LEN)-1)<<OSD_BLEND_PALETTE_WORD_76_POS))

/* 0x5DC : PALE_SRAM_77 */
#define OSD_BLEND_PALE_SRAM_77_OFFSET                           (0x5DC)
#define OSD_BLEND_PALETTE_WORD_77                               OSD_BLEND_PALETTE_WORD_77
#define OSD_BLEND_PALETTE_WORD_77_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_77_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_77_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_77_LEN)-1)<<OSD_BLEND_PALETTE_WORD_77_POS)
#define OSD_BLEND_PALETTE_WORD_77_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_77_LEN)-1)<<OSD_BLEND_PALETTE_WORD_77_POS))

/* 0x5E0 : PALE_SRAM_78 */
#define OSD_BLEND_PALE_SRAM_78_OFFSET                           (0x5E0)
#define OSD_BLEND_PALETTE_WORD_78                               OSD_BLEND_PALETTE_WORD_78
#define OSD_BLEND_PALETTE_WORD_78_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_78_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_78_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_78_LEN)-1)<<OSD_BLEND_PALETTE_WORD_78_POS)
#define OSD_BLEND_PALETTE_WORD_78_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_78_LEN)-1)<<OSD_BLEND_PALETTE_WORD_78_POS))

/* 0x5E4 : PALE_SRAM_79 */
#define OSD_BLEND_PALE_SRAM_79_OFFSET                           (0x5E4)
#define OSD_BLEND_PALETTE_WORD_79                               OSD_BLEND_PALETTE_WORD_79
#define OSD_BLEND_PALETTE_WORD_79_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_79_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_79_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_79_LEN)-1)<<OSD_BLEND_PALETTE_WORD_79_POS)
#define OSD_BLEND_PALETTE_WORD_79_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_79_LEN)-1)<<OSD_BLEND_PALETTE_WORD_79_POS))

/* 0x5E8 : PALE_SRAM_7A */
#define OSD_BLEND_PALE_SRAM_7A_OFFSET                           (0x5E8)
#define OSD_BLEND_PALETTE_WORD_7A                               OSD_BLEND_PALETTE_WORD_7A
#define OSD_BLEND_PALETTE_WORD_7A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7A_POS)
#define OSD_BLEND_PALETTE_WORD_7A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7A_POS))

/* 0x5EC : PALE_SRAM_7B */
#define OSD_BLEND_PALE_SRAM_7B_OFFSET                           (0x5EC)
#define OSD_BLEND_PALETTE_WORD_7B                               OSD_BLEND_PALETTE_WORD_7B
#define OSD_BLEND_PALETTE_WORD_7B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7B_POS)
#define OSD_BLEND_PALETTE_WORD_7B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7B_POS))

/* 0x5F0 : PALE_SRAM_7C */
#define OSD_BLEND_PALE_SRAM_7C_OFFSET                           (0x5F0)
#define OSD_BLEND_PALETTE_WORD_7C                               OSD_BLEND_PALETTE_WORD_7C
#define OSD_BLEND_PALETTE_WORD_7C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7C_POS)
#define OSD_BLEND_PALETTE_WORD_7C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7C_POS))

/* 0x5F4 : PALE_SRAM_7D */
#define OSD_BLEND_PALE_SRAM_7D_OFFSET                           (0x5F4)
#define OSD_BLEND_PALETTE_WORD_7D                               OSD_BLEND_PALETTE_WORD_7D
#define OSD_BLEND_PALETTE_WORD_7D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7D_POS)
#define OSD_BLEND_PALETTE_WORD_7D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7D_POS))

/* 0x5F8 : PALE_SRAM_7E */
#define OSD_BLEND_PALE_SRAM_7E_OFFSET                           (0x5F8)
#define OSD_BLEND_PALETTE_WORD_7E                               OSD_BLEND_PALETTE_WORD_7E
#define OSD_BLEND_PALETTE_WORD_7E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7E_POS)
#define OSD_BLEND_PALETTE_WORD_7E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7E_POS))

/* 0x5FC : PALE_SRAM_7F */
#define OSD_BLEND_PALE_SRAM_7F_OFFSET                           (0x5FC)
#define OSD_BLEND_PALETTE_WORD_7F                               OSD_BLEND_PALETTE_WORD_7F
#define OSD_BLEND_PALETTE_WORD_7F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_7F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_7F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_7F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7F_POS)
#define OSD_BLEND_PALETTE_WORD_7F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_7F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_7F_POS))

/* 0x600 : PALE_SRAM_80 */
#define OSD_BLEND_PALE_SRAM_80_OFFSET                           (0x600)
#define OSD_BLEND_PALETTE_WORD_80                               OSD_BLEND_PALETTE_WORD_80
#define OSD_BLEND_PALETTE_WORD_80_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_80_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_80_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_80_LEN)-1)<<OSD_BLEND_PALETTE_WORD_80_POS)
#define OSD_BLEND_PALETTE_WORD_80_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_80_LEN)-1)<<OSD_BLEND_PALETTE_WORD_80_POS))

/* 0x604 : PALE_SRAM_81 */
#define OSD_BLEND_PALE_SRAM_81_OFFSET                           (0x604)
#define OSD_BLEND_PALETTE_WORD_81                               OSD_BLEND_PALETTE_WORD_81
#define OSD_BLEND_PALETTE_WORD_81_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_81_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_81_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_81_LEN)-1)<<OSD_BLEND_PALETTE_WORD_81_POS)
#define OSD_BLEND_PALETTE_WORD_81_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_81_LEN)-1)<<OSD_BLEND_PALETTE_WORD_81_POS))

/* 0x608 : PALE_SRAM_82 */
#define OSD_BLEND_PALE_SRAM_82_OFFSET                           (0x608)
#define OSD_BLEND_PALETTE_WORD_82                               OSD_BLEND_PALETTE_WORD_82
#define OSD_BLEND_PALETTE_WORD_82_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_82_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_82_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_82_LEN)-1)<<OSD_BLEND_PALETTE_WORD_82_POS)
#define OSD_BLEND_PALETTE_WORD_82_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_82_LEN)-1)<<OSD_BLEND_PALETTE_WORD_82_POS))

/* 0x60C : PALE_SRAM_83 */
#define OSD_BLEND_PALE_SRAM_83_OFFSET                           (0x60C)
#define OSD_BLEND_PALETTE_WORD_83                               OSD_BLEND_PALETTE_WORD_83
#define OSD_BLEND_PALETTE_WORD_83_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_83_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_83_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_83_LEN)-1)<<OSD_BLEND_PALETTE_WORD_83_POS)
#define OSD_BLEND_PALETTE_WORD_83_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_83_LEN)-1)<<OSD_BLEND_PALETTE_WORD_83_POS))

/* 0x610 : PALE_SRAM_84 */
#define OSD_BLEND_PALE_SRAM_84_OFFSET                           (0x610)
#define OSD_BLEND_PALETTE_WORD_84                               OSD_BLEND_PALETTE_WORD_84
#define OSD_BLEND_PALETTE_WORD_84_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_84_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_84_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_84_LEN)-1)<<OSD_BLEND_PALETTE_WORD_84_POS)
#define OSD_BLEND_PALETTE_WORD_84_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_84_LEN)-1)<<OSD_BLEND_PALETTE_WORD_84_POS))

/* 0x614 : PALE_SRAM_85 */
#define OSD_BLEND_PALE_SRAM_85_OFFSET                           (0x614)
#define OSD_BLEND_PALETTE_WORD_85                               OSD_BLEND_PALETTE_WORD_85
#define OSD_BLEND_PALETTE_WORD_85_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_85_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_85_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_85_LEN)-1)<<OSD_BLEND_PALETTE_WORD_85_POS)
#define OSD_BLEND_PALETTE_WORD_85_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_85_LEN)-1)<<OSD_BLEND_PALETTE_WORD_85_POS))

/* 0x618 : PALE_SRAM_86 */
#define OSD_BLEND_PALE_SRAM_86_OFFSET                           (0x618)
#define OSD_BLEND_PALETTE_WORD_86                               OSD_BLEND_PALETTE_WORD_86
#define OSD_BLEND_PALETTE_WORD_86_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_86_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_86_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_86_LEN)-1)<<OSD_BLEND_PALETTE_WORD_86_POS)
#define OSD_BLEND_PALETTE_WORD_86_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_86_LEN)-1)<<OSD_BLEND_PALETTE_WORD_86_POS))

/* 0x61C : PALE_SRAM_87 */
#define OSD_BLEND_PALE_SRAM_87_OFFSET                           (0x61C)
#define OSD_BLEND_PALETTE_WORD_87                               OSD_BLEND_PALETTE_WORD_87
#define OSD_BLEND_PALETTE_WORD_87_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_87_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_87_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_87_LEN)-1)<<OSD_BLEND_PALETTE_WORD_87_POS)
#define OSD_BLEND_PALETTE_WORD_87_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_87_LEN)-1)<<OSD_BLEND_PALETTE_WORD_87_POS))

/* 0x620 : PALE_SRAM_88 */
#define OSD_BLEND_PALE_SRAM_88_OFFSET                           (0x620)
#define OSD_BLEND_PALETTE_WORD_88                               OSD_BLEND_PALETTE_WORD_88
#define OSD_BLEND_PALETTE_WORD_88_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_88_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_88_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_88_LEN)-1)<<OSD_BLEND_PALETTE_WORD_88_POS)
#define OSD_BLEND_PALETTE_WORD_88_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_88_LEN)-1)<<OSD_BLEND_PALETTE_WORD_88_POS))

/* 0x624 : PALE_SRAM_89 */
#define OSD_BLEND_PALE_SRAM_89_OFFSET                           (0x624)
#define OSD_BLEND_PALETTE_WORD_89                               OSD_BLEND_PALETTE_WORD_89
#define OSD_BLEND_PALETTE_WORD_89_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_89_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_89_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_89_LEN)-1)<<OSD_BLEND_PALETTE_WORD_89_POS)
#define OSD_BLEND_PALETTE_WORD_89_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_89_LEN)-1)<<OSD_BLEND_PALETTE_WORD_89_POS))

/* 0x628 : PALE_SRAM_8A */
#define OSD_BLEND_PALE_SRAM_8A_OFFSET                           (0x628)
#define OSD_BLEND_PALETTE_WORD_8A                               OSD_BLEND_PALETTE_WORD_8A
#define OSD_BLEND_PALETTE_WORD_8A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8A_POS)
#define OSD_BLEND_PALETTE_WORD_8A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8A_POS))

/* 0x62C : PALE_SRAM_8B */
#define OSD_BLEND_PALE_SRAM_8B_OFFSET                           (0x62C)
#define OSD_BLEND_PALETTE_WORD_8B                               OSD_BLEND_PALETTE_WORD_8B
#define OSD_BLEND_PALETTE_WORD_8B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8B_POS)
#define OSD_BLEND_PALETTE_WORD_8B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8B_POS))

/* 0x630 : PALE_SRAM_8C */
#define OSD_BLEND_PALE_SRAM_8C_OFFSET                           (0x630)
#define OSD_BLEND_PALETTE_WORD_8C                               OSD_BLEND_PALETTE_WORD_8C
#define OSD_BLEND_PALETTE_WORD_8C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8C_POS)
#define OSD_BLEND_PALETTE_WORD_8C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8C_POS))

/* 0x634 : PALE_SRAM_8D */
#define OSD_BLEND_PALE_SRAM_8D_OFFSET                           (0x634)
#define OSD_BLEND_PALETTE_WORD_8D                               OSD_BLEND_PALETTE_WORD_8D
#define OSD_BLEND_PALETTE_WORD_8D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8D_POS)
#define OSD_BLEND_PALETTE_WORD_8D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8D_POS))

/* 0x638 : PALE_SRAM_8E */
#define OSD_BLEND_PALE_SRAM_8E_OFFSET                           (0x638)
#define OSD_BLEND_PALETTE_WORD_8E                               OSD_BLEND_PALETTE_WORD_8E
#define OSD_BLEND_PALETTE_WORD_8E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8E_POS)
#define OSD_BLEND_PALETTE_WORD_8E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8E_POS))

/* 0x63C : PALE_SRAM_8F */
#define OSD_BLEND_PALE_SRAM_8F_OFFSET                           (0x63C)
#define OSD_BLEND_PALETTE_WORD_8F                               OSD_BLEND_PALETTE_WORD_8F
#define OSD_BLEND_PALETTE_WORD_8F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_8F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_8F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_8F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8F_POS)
#define OSD_BLEND_PALETTE_WORD_8F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_8F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_8F_POS))

/* 0x640 : PALE_SRAM_90 */
#define OSD_BLEND_PALE_SRAM_90_OFFSET                           (0x640)
#define OSD_BLEND_PALETTE_WORD_90                               OSD_BLEND_PALETTE_WORD_90
#define OSD_BLEND_PALETTE_WORD_90_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_90_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_90_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_90_LEN)-1)<<OSD_BLEND_PALETTE_WORD_90_POS)
#define OSD_BLEND_PALETTE_WORD_90_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_90_LEN)-1)<<OSD_BLEND_PALETTE_WORD_90_POS))

/* 0x644 : PALE_SRAM_91 */
#define OSD_BLEND_PALE_SRAM_91_OFFSET                           (0x644)
#define OSD_BLEND_PALETTE_WORD_91                               OSD_BLEND_PALETTE_WORD_91
#define OSD_BLEND_PALETTE_WORD_91_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_91_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_91_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_91_LEN)-1)<<OSD_BLEND_PALETTE_WORD_91_POS)
#define OSD_BLEND_PALETTE_WORD_91_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_91_LEN)-1)<<OSD_BLEND_PALETTE_WORD_91_POS))

/* 0x648 : PALE_SRAM_92 */
#define OSD_BLEND_PALE_SRAM_92_OFFSET                           (0x648)
#define OSD_BLEND_PALETTE_WORD_92                               OSD_BLEND_PALETTE_WORD_92
#define OSD_BLEND_PALETTE_WORD_92_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_92_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_92_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_92_LEN)-1)<<OSD_BLEND_PALETTE_WORD_92_POS)
#define OSD_BLEND_PALETTE_WORD_92_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_92_LEN)-1)<<OSD_BLEND_PALETTE_WORD_92_POS))

/* 0x64C : PALE_SRAM_93 */
#define OSD_BLEND_PALE_SRAM_93_OFFSET                           (0x64C)
#define OSD_BLEND_PALETTE_WORD_93                               OSD_BLEND_PALETTE_WORD_93
#define OSD_BLEND_PALETTE_WORD_93_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_93_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_93_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_93_LEN)-1)<<OSD_BLEND_PALETTE_WORD_93_POS)
#define OSD_BLEND_PALETTE_WORD_93_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_93_LEN)-1)<<OSD_BLEND_PALETTE_WORD_93_POS))

/* 0x650 : PALE_SRAM_94 */
#define OSD_BLEND_PALE_SRAM_94_OFFSET                           (0x650)
#define OSD_BLEND_PALETTE_WORD_94                               OSD_BLEND_PALETTE_WORD_94
#define OSD_BLEND_PALETTE_WORD_94_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_94_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_94_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_94_LEN)-1)<<OSD_BLEND_PALETTE_WORD_94_POS)
#define OSD_BLEND_PALETTE_WORD_94_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_94_LEN)-1)<<OSD_BLEND_PALETTE_WORD_94_POS))

/* 0x654 : PALE_SRAM_95 */
#define OSD_BLEND_PALE_SRAM_95_OFFSET                           (0x654)
#define OSD_BLEND_PALETTE_WORD_95                               OSD_BLEND_PALETTE_WORD_95
#define OSD_BLEND_PALETTE_WORD_95_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_95_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_95_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_95_LEN)-1)<<OSD_BLEND_PALETTE_WORD_95_POS)
#define OSD_BLEND_PALETTE_WORD_95_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_95_LEN)-1)<<OSD_BLEND_PALETTE_WORD_95_POS))

/* 0x658 : PALE_SRAM_96 */
#define OSD_BLEND_PALE_SRAM_96_OFFSET                           (0x658)
#define OSD_BLEND_PALETTE_WORD_96                               OSD_BLEND_PALETTE_WORD_96
#define OSD_BLEND_PALETTE_WORD_96_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_96_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_96_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_96_LEN)-1)<<OSD_BLEND_PALETTE_WORD_96_POS)
#define OSD_BLEND_PALETTE_WORD_96_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_96_LEN)-1)<<OSD_BLEND_PALETTE_WORD_96_POS))

/* 0x65C : PALE_SRAM_97 */
#define OSD_BLEND_PALE_SRAM_97_OFFSET                           (0x65C)
#define OSD_BLEND_PALETTE_WORD_97                               OSD_BLEND_PALETTE_WORD_97
#define OSD_BLEND_PALETTE_WORD_97_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_97_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_97_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_97_LEN)-1)<<OSD_BLEND_PALETTE_WORD_97_POS)
#define OSD_BLEND_PALETTE_WORD_97_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_97_LEN)-1)<<OSD_BLEND_PALETTE_WORD_97_POS))

/* 0x660 : PALE_SRAM_98 */
#define OSD_BLEND_PALE_SRAM_98_OFFSET                           (0x660)
#define OSD_BLEND_PALETTE_WORD_98                               OSD_BLEND_PALETTE_WORD_98
#define OSD_BLEND_PALETTE_WORD_98_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_98_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_98_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_98_LEN)-1)<<OSD_BLEND_PALETTE_WORD_98_POS)
#define OSD_BLEND_PALETTE_WORD_98_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_98_LEN)-1)<<OSD_BLEND_PALETTE_WORD_98_POS))

/* 0x664 : PALE_SRAM_99 */
#define OSD_BLEND_PALE_SRAM_99_OFFSET                           (0x664)
#define OSD_BLEND_PALETTE_WORD_99                               OSD_BLEND_PALETTE_WORD_99
#define OSD_BLEND_PALETTE_WORD_99_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_99_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_99_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_99_LEN)-1)<<OSD_BLEND_PALETTE_WORD_99_POS)
#define OSD_BLEND_PALETTE_WORD_99_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_99_LEN)-1)<<OSD_BLEND_PALETTE_WORD_99_POS))

/* 0x668 : PALE_SRAM_9A */
#define OSD_BLEND_PALE_SRAM_9A_OFFSET                           (0x668)
#define OSD_BLEND_PALETTE_WORD_9A                               OSD_BLEND_PALETTE_WORD_9A
#define OSD_BLEND_PALETTE_WORD_9A_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9A_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9A_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9A_POS)
#define OSD_BLEND_PALETTE_WORD_9A_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9A_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9A_POS))

/* 0x66C : PALE_SRAM_9B */
#define OSD_BLEND_PALE_SRAM_9B_OFFSET                           (0x66C)
#define OSD_BLEND_PALETTE_WORD_9B                               OSD_BLEND_PALETTE_WORD_9B
#define OSD_BLEND_PALETTE_WORD_9B_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9B_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9B_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9B_POS)
#define OSD_BLEND_PALETTE_WORD_9B_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9B_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9B_POS))

/* 0x670 : PALE_SRAM_9C */
#define OSD_BLEND_PALE_SRAM_9C_OFFSET                           (0x670)
#define OSD_BLEND_PALETTE_WORD_9C                               OSD_BLEND_PALETTE_WORD_9C
#define OSD_BLEND_PALETTE_WORD_9C_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9C_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9C_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9C_POS)
#define OSD_BLEND_PALETTE_WORD_9C_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9C_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9C_POS))

/* 0x674 : PALE_SRAM_9D */
#define OSD_BLEND_PALE_SRAM_9D_OFFSET                           (0x674)
#define OSD_BLEND_PALETTE_WORD_9D                               OSD_BLEND_PALETTE_WORD_9D
#define OSD_BLEND_PALETTE_WORD_9D_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9D_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9D_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9D_POS)
#define OSD_BLEND_PALETTE_WORD_9D_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9D_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9D_POS))

/* 0x678 : PALE_SRAM_9E */
#define OSD_BLEND_PALE_SRAM_9E_OFFSET                           (0x678)
#define OSD_BLEND_PALETTE_WORD_9E                               OSD_BLEND_PALETTE_WORD_9E
#define OSD_BLEND_PALETTE_WORD_9E_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9E_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9E_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9E_POS)
#define OSD_BLEND_PALETTE_WORD_9E_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9E_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9E_POS))

/* 0x67C : PALE_SRAM_9F */
#define OSD_BLEND_PALE_SRAM_9F_OFFSET                           (0x67C)
#define OSD_BLEND_PALETTE_WORD_9F                               OSD_BLEND_PALETTE_WORD_9F
#define OSD_BLEND_PALETTE_WORD_9F_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_9F_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_9F_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_9F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9F_POS)
#define OSD_BLEND_PALETTE_WORD_9F_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_9F_LEN)-1)<<OSD_BLEND_PALETTE_WORD_9F_POS))

/* 0x680 : PALE_SRAM_A0 */
#define OSD_BLEND_PALE_SRAM_A0_OFFSET                           (0x680)
#define OSD_BLEND_PALETTE_WORD_A0                               OSD_BLEND_PALETTE_WORD_A0
#define OSD_BLEND_PALETTE_WORD_A0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A0_POS)
#define OSD_BLEND_PALETTE_WORD_A0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A0_POS))

/* 0x684 : PALE_SRAM_A1 */
#define OSD_BLEND_PALE_SRAM_A1_OFFSET                           (0x684)
#define OSD_BLEND_PALETTE_WORD_A1                               OSD_BLEND_PALETTE_WORD_A1
#define OSD_BLEND_PALETTE_WORD_A1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A1_POS)
#define OSD_BLEND_PALETTE_WORD_A1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A1_POS))

/* 0x688 : PALE_SRAM_A2 */
#define OSD_BLEND_PALE_SRAM_A2_OFFSET                           (0x688)
#define OSD_BLEND_PALETTE_WORD_A2                               OSD_BLEND_PALETTE_WORD_A2
#define OSD_BLEND_PALETTE_WORD_A2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A2_POS)
#define OSD_BLEND_PALETTE_WORD_A2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A2_POS))

/* 0x68C : PALE_SRAM_A3 */
#define OSD_BLEND_PALE_SRAM_A3_OFFSET                           (0x68C)
#define OSD_BLEND_PALETTE_WORD_A3                               OSD_BLEND_PALETTE_WORD_A3
#define OSD_BLEND_PALETTE_WORD_A3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A3_POS)
#define OSD_BLEND_PALETTE_WORD_A3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A3_POS))

/* 0x690 : PALE_SRAM_A4 */
#define OSD_BLEND_PALE_SRAM_A4_OFFSET                           (0x690)
#define OSD_BLEND_PALETTE_WORD_A4                               OSD_BLEND_PALETTE_WORD_A4
#define OSD_BLEND_PALETTE_WORD_A4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A4_POS)
#define OSD_BLEND_PALETTE_WORD_A4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A4_POS))

/* 0x694 : PALE_SRAM_A5 */
#define OSD_BLEND_PALE_SRAM_A5_OFFSET                           (0x694)
#define OSD_BLEND_PALETTE_WORD_A5                               OSD_BLEND_PALETTE_WORD_A5
#define OSD_BLEND_PALETTE_WORD_A5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A5_POS)
#define OSD_BLEND_PALETTE_WORD_A5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A5_POS))

/* 0x698 : PALE_SRAM_A6 */
#define OSD_BLEND_PALE_SRAM_A6_OFFSET                           (0x698)
#define OSD_BLEND_PALETTE_WORD_A6                               OSD_BLEND_PALETTE_WORD_A6
#define OSD_BLEND_PALETTE_WORD_A6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A6_POS)
#define OSD_BLEND_PALETTE_WORD_A6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A6_POS))

/* 0x69C : PALE_SRAM_A7 */
#define OSD_BLEND_PALE_SRAM_A7_OFFSET                           (0x69C)
#define OSD_BLEND_PALETTE_WORD_A7                               OSD_BLEND_PALETTE_WORD_A7
#define OSD_BLEND_PALETTE_WORD_A7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A7_POS)
#define OSD_BLEND_PALETTE_WORD_A7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A7_POS))

/* 0x6A0 : PALE_SRAM_A8 */
#define OSD_BLEND_PALE_SRAM_A8_OFFSET                           (0x6A0)
#define OSD_BLEND_PALETTE_WORD_A8                               OSD_BLEND_PALETTE_WORD_A8
#define OSD_BLEND_PALETTE_WORD_A8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A8_POS)
#define OSD_BLEND_PALETTE_WORD_A8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A8_POS))

/* 0x6A4 : PALE_SRAM_A9 */
#define OSD_BLEND_PALE_SRAM_A9_OFFSET                           (0x6A4)
#define OSD_BLEND_PALETTE_WORD_A9                               OSD_BLEND_PALETTE_WORD_A9
#define OSD_BLEND_PALETTE_WORD_A9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_A9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_A9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_A9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A9_POS)
#define OSD_BLEND_PALETTE_WORD_A9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_A9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_A9_POS))

/* 0x6A8 : PALE_SRAM_AA */
#define OSD_BLEND_PALE_SRAM_AA_OFFSET                           (0x6A8)
#define OSD_BLEND_PALETTE_WORD_AA                               OSD_BLEND_PALETTE_WORD_AA
#define OSD_BLEND_PALETTE_WORD_AA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AA_POS)
#define OSD_BLEND_PALETTE_WORD_AA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AA_POS))

/* 0x6AC : PALE_SRAM_AB */
#define OSD_BLEND_PALE_SRAM_AB_OFFSET                           (0x6AC)
#define OSD_BLEND_PALETTE_WORD_AB                               OSD_BLEND_PALETTE_WORD_AB
#define OSD_BLEND_PALETTE_WORD_AB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AB_POS)
#define OSD_BLEND_PALETTE_WORD_AB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AB_POS))

/* 0x6B0 : PALE_SRAM_AC */
#define OSD_BLEND_PALE_SRAM_AC_OFFSET                           (0x6B0)
#define OSD_BLEND_PALETTE_WORD_AC                               OSD_BLEND_PALETTE_WORD_AC
#define OSD_BLEND_PALETTE_WORD_AC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AC_POS)
#define OSD_BLEND_PALETTE_WORD_AC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AC_POS))

/* 0x6B4 : PALE_SRAM_AD */
#define OSD_BLEND_PALE_SRAM_AD_OFFSET                           (0x6B4)
#define OSD_BLEND_PALETTE_WORD_AD                               OSD_BLEND_PALETTE_WORD_AD
#define OSD_BLEND_PALETTE_WORD_AD_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AD_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AD_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AD_POS)
#define OSD_BLEND_PALETTE_WORD_AD_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AD_POS))

/* 0x6B8 : PALE_SRAM_AE */
#define OSD_BLEND_PALE_SRAM_AE_OFFSET                           (0x6B8)
#define OSD_BLEND_PALETTE_WORD_AE                               OSD_BLEND_PALETTE_WORD_AE
#define OSD_BLEND_PALETTE_WORD_AE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AE_POS)
#define OSD_BLEND_PALETTE_WORD_AE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AE_POS))

/* 0x6BC : PALE_SRAM_AF */
#define OSD_BLEND_PALE_SRAM_AF_OFFSET                           (0x6BC)
#define OSD_BLEND_PALETTE_WORD_AF                               OSD_BLEND_PALETTE_WORD_AF
#define OSD_BLEND_PALETTE_WORD_AF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_AF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_AF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_AF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AF_POS)
#define OSD_BLEND_PALETTE_WORD_AF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_AF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_AF_POS))

/* 0x6C0 : PALE_SRAM_B0 */
#define OSD_BLEND_PALE_SRAM_B0_OFFSET                           (0x6C0)
#define OSD_BLEND_PALETTE_WORD_B0                               OSD_BLEND_PALETTE_WORD_B0
#define OSD_BLEND_PALETTE_WORD_B0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B0_POS)
#define OSD_BLEND_PALETTE_WORD_B0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B0_POS))

/* 0x6C4 : PALE_SRAM_B1 */
#define OSD_BLEND_PALE_SRAM_B1_OFFSET                           (0x6C4)
#define OSD_BLEND_PALETTE_WORD_B1                               OSD_BLEND_PALETTE_WORD_B1
#define OSD_BLEND_PALETTE_WORD_B1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B1_POS)
#define OSD_BLEND_PALETTE_WORD_B1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B1_POS))

/* 0x6C8 : PALE_SRAM_B2 */
#define OSD_BLEND_PALE_SRAM_B2_OFFSET                           (0x6C8)
#define OSD_BLEND_PALETTE_WORD_B2                               OSD_BLEND_PALETTE_WORD_B2
#define OSD_BLEND_PALETTE_WORD_B2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B2_POS)
#define OSD_BLEND_PALETTE_WORD_B2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B2_POS))

/* 0x6CC : PALE_SRAM_B3 */
#define OSD_BLEND_PALE_SRAM_B3_OFFSET                           (0x6CC)
#define OSD_BLEND_PALETTE_WORD_B3                               OSD_BLEND_PALETTE_WORD_B3
#define OSD_BLEND_PALETTE_WORD_B3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B3_POS)
#define OSD_BLEND_PALETTE_WORD_B3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B3_POS))

/* 0x6D0 : PALE_SRAM_B4 */
#define OSD_BLEND_PALE_SRAM_B4_OFFSET                           (0x6D0)
#define OSD_BLEND_PALETTE_WORD_B4                               OSD_BLEND_PALETTE_WORD_B4
#define OSD_BLEND_PALETTE_WORD_B4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B4_POS)
#define OSD_BLEND_PALETTE_WORD_B4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B4_POS))

/* 0x6D4 : PALE_SRAM_B5 */
#define OSD_BLEND_PALE_SRAM_B5_OFFSET                           (0x6D4)
#define OSD_BLEND_PALETTE_WORD_B5                               OSD_BLEND_PALETTE_WORD_B5
#define OSD_BLEND_PALETTE_WORD_B5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B5_POS)
#define OSD_BLEND_PALETTE_WORD_B5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B5_POS))

/* 0x6D8 : PALE_SRAM_B6 */
#define OSD_BLEND_PALE_SRAM_B6_OFFSET                           (0x6D8)
#define OSD_BLEND_PALETTE_WORD_B6                               OSD_BLEND_PALETTE_WORD_B6
#define OSD_BLEND_PALETTE_WORD_B6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B6_POS)
#define OSD_BLEND_PALETTE_WORD_B6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B6_POS))

/* 0x6DC : PALE_SRAM_B7 */
#define OSD_BLEND_PALE_SRAM_B7_OFFSET                           (0x6DC)
#define OSD_BLEND_PALETTE_WORD_B7                               OSD_BLEND_PALETTE_WORD_B7
#define OSD_BLEND_PALETTE_WORD_B7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B7_POS)
#define OSD_BLEND_PALETTE_WORD_B7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B7_POS))

/* 0x6E0 : PALE_SRAM_B8 */
#define OSD_BLEND_PALE_SRAM_B8_OFFSET                           (0x6E0)
#define OSD_BLEND_PALETTE_WORD_B8                               OSD_BLEND_PALETTE_WORD_B8
#define OSD_BLEND_PALETTE_WORD_B8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B8_POS)
#define OSD_BLEND_PALETTE_WORD_B8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B8_POS))

/* 0x6E4 : PALE_SRAM_B9 */
#define OSD_BLEND_PALE_SRAM_B9_OFFSET                           (0x6E4)
#define OSD_BLEND_PALETTE_WORD_B9                               OSD_BLEND_PALETTE_WORD_B9
#define OSD_BLEND_PALETTE_WORD_B9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_B9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_B9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_B9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B9_POS)
#define OSD_BLEND_PALETTE_WORD_B9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_B9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_B9_POS))

/* 0x6E8 : PALE_SRAM_BA */
#define OSD_BLEND_PALE_SRAM_BA_OFFSET                           (0x6E8)
#define OSD_BLEND_PALETTE_WORD_BA                               OSD_BLEND_PALETTE_WORD_BA
#define OSD_BLEND_PALETTE_WORD_BA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BA_POS)
#define OSD_BLEND_PALETTE_WORD_BA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BA_POS))

/* 0x6EC : PALE_SRAM_BB */
#define OSD_BLEND_PALE_SRAM_BB_OFFSET                           (0x6EC)
#define OSD_BLEND_PALETTE_WORD_BB                               OSD_BLEND_PALETTE_WORD_BB
#define OSD_BLEND_PALETTE_WORD_BB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BB_POS)
#define OSD_BLEND_PALETTE_WORD_BB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BB_POS))

/* 0x6F0 : PALE_SRAM_BC */
#define OSD_BLEND_PALE_SRAM_BC_OFFSET                           (0x6F0)
#define OSD_BLEND_PALETTE_WORD_BC                               OSD_BLEND_PALETTE_WORD_BC
#define OSD_BLEND_PALETTE_WORD_BC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BC_POS)
#define OSD_BLEND_PALETTE_WORD_BC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BC_POS))

/* 0x6F4 : PALE_SRAM_BD */
#define OSD_BLEND_PALE_SRAM_BD_OFFSET                           (0x6F4)
#define OSD_BLEND_PALETTE_WORD_BD                               OSD_BLEND_PALETTE_WORD_BD
#define OSD_BLEND_PALETTE_WORD_BD_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BD_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BD_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BD_POS)
#define OSD_BLEND_PALETTE_WORD_BD_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BD_POS))

/* 0x6F8 : PALE_SRAM_BE */
#define OSD_BLEND_PALE_SRAM_BE_OFFSET                           (0x6F8)
#define OSD_BLEND_PALETTE_WORD_BE                               OSD_BLEND_PALETTE_WORD_BE
#define OSD_BLEND_PALETTE_WORD_BE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BE_POS)
#define OSD_BLEND_PALETTE_WORD_BE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BE_POS))

/* 0x6FC : PALE_SRAM_BF */
#define OSD_BLEND_PALE_SRAM_BF_OFFSET                           (0x6FC)
#define OSD_BLEND_PALETTE_WORD_BF                               OSD_BLEND_PALETTE_WORD_BF
#define OSD_BLEND_PALETTE_WORD_BF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_BF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_BF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_BF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BF_POS)
#define OSD_BLEND_PALETTE_WORD_BF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_BF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_BF_POS))

/* 0x700 : PALE_SRAM_C0 */
#define OSD_BLEND_PALE_SRAM_C0_OFFSET                           (0x700)
#define OSD_BLEND_PALETTE_WORD_C0                               OSD_BLEND_PALETTE_WORD_C0
#define OSD_BLEND_PALETTE_WORD_C0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C0_POS)
#define OSD_BLEND_PALETTE_WORD_C0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C0_POS))

/* 0x704 : PALE_SRAM_C1 */
#define OSD_BLEND_PALE_SRAM_C1_OFFSET                           (0x704)
#define OSD_BLEND_PALETTE_WORD_C1                               OSD_BLEND_PALETTE_WORD_C1
#define OSD_BLEND_PALETTE_WORD_C1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C1_POS)
#define OSD_BLEND_PALETTE_WORD_C1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C1_POS))

/* 0x708 : PALE_SRAM_C2 */
#define OSD_BLEND_PALE_SRAM_C2_OFFSET                           (0x708)
#define OSD_BLEND_PALETTE_WORD_C2                               OSD_BLEND_PALETTE_WORD_C2
#define OSD_BLEND_PALETTE_WORD_C2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C2_POS)
#define OSD_BLEND_PALETTE_WORD_C2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C2_POS))

/* 0x70C : PALE_SRAM_C3 */
#define OSD_BLEND_PALE_SRAM_C3_OFFSET                           (0x70C)
#define OSD_BLEND_PALETTE_WORD_C3                               OSD_BLEND_PALETTE_WORD_C3
#define OSD_BLEND_PALETTE_WORD_C3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C3_POS)
#define OSD_BLEND_PALETTE_WORD_C3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C3_POS))

/* 0x710 : PALE_SRAM_C4 */
#define OSD_BLEND_PALE_SRAM_C4_OFFSET                           (0x710)
#define OSD_BLEND_PALETTE_WORD_C4                               OSD_BLEND_PALETTE_WORD_C4
#define OSD_BLEND_PALETTE_WORD_C4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C4_POS)
#define OSD_BLEND_PALETTE_WORD_C4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C4_POS))

/* 0x714 : PALE_SRAM_C5 */
#define OSD_BLEND_PALE_SRAM_C5_OFFSET                           (0x714)
#define OSD_BLEND_PALETTE_WORD_C5                               OSD_BLEND_PALETTE_WORD_C5
#define OSD_BLEND_PALETTE_WORD_C5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C5_POS)
#define OSD_BLEND_PALETTE_WORD_C5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C5_POS))

/* 0x718 : PALE_SRAM_C6 */
#define OSD_BLEND_PALE_SRAM_C6_OFFSET                           (0x718)
#define OSD_BLEND_PALETTE_WORD_C6                               OSD_BLEND_PALETTE_WORD_C6
#define OSD_BLEND_PALETTE_WORD_C6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C6_POS)
#define OSD_BLEND_PALETTE_WORD_C6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C6_POS))

/* 0x71C : PALE_SRAM_C7 */
#define OSD_BLEND_PALE_SRAM_C7_OFFSET                           (0x71C)
#define OSD_BLEND_PALETTE_WORD_C7                               OSD_BLEND_PALETTE_WORD_C7
#define OSD_BLEND_PALETTE_WORD_C7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C7_POS)
#define OSD_BLEND_PALETTE_WORD_C7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C7_POS))

/* 0x720 : PALE_SRAM_C8 */
#define OSD_BLEND_PALE_SRAM_C8_OFFSET                           (0x720)
#define OSD_BLEND_PALETTE_WORD_C8                               OSD_BLEND_PALETTE_WORD_C8
#define OSD_BLEND_PALETTE_WORD_C8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C8_POS)
#define OSD_BLEND_PALETTE_WORD_C8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C8_POS))

/* 0x724 : PALE_SRAM_C9 */
#define OSD_BLEND_PALE_SRAM_C9_OFFSET                           (0x724)
#define OSD_BLEND_PALETTE_WORD_C9                               OSD_BLEND_PALETTE_WORD_C9
#define OSD_BLEND_PALETTE_WORD_C9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_C9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_C9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_C9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C9_POS)
#define OSD_BLEND_PALETTE_WORD_C9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_C9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_C9_POS))

/* 0x728 : PALE_SRAM_CA */
#define OSD_BLEND_PALE_SRAM_CA_OFFSET                           (0x728)
#define OSD_BLEND_PALETTE_WORD_CA                               OSD_BLEND_PALETTE_WORD_CA
#define OSD_BLEND_PALETTE_WORD_CA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CA_POS)
#define OSD_BLEND_PALETTE_WORD_CA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CA_POS))

/* 0x72C : PALE_SRAM_CB */
#define OSD_BLEND_PALE_SRAM_CB_OFFSET                           (0x72C)
#define OSD_BLEND_PALETTE_WORD_CB                               OSD_BLEND_PALETTE_WORD_CB
#define OSD_BLEND_PALETTE_WORD_CB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CB_POS)
#define OSD_BLEND_PALETTE_WORD_CB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CB_POS))

/* 0x730 : PALE_SRAM_CC */
#define OSD_BLEND_PALE_SRAM_CC_OFFSET                           (0x730)
#define OSD_BLEND_PALETTE_WORD_CC                               OSD_BLEND_PALETTE_WORD_CC
#define OSD_BLEND_PALETTE_WORD_CC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CC_POS)
#define OSD_BLEND_PALETTE_WORD_CC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CC_POS))

/* 0x734 : PALE_SRAM_CD */
#define OSD_BLEND_PALE_SRAM_CD_OFFSET                           (0x734)
#define OSD_BLEND_PALETTE_WORD_CD                               OSD_BLEND_PALETTE_WORD_CD
#define OSD_BLEND_PALETTE_WORD_CD_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CD_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CD_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CD_POS)
#define OSD_BLEND_PALETTE_WORD_CD_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CD_POS))

/* 0x738 : PALE_SRAM_CE */
#define OSD_BLEND_PALE_SRAM_CE_OFFSET                           (0x738)
#define OSD_BLEND_PALETTE_WORD_CE                               OSD_BLEND_PALETTE_WORD_CE
#define OSD_BLEND_PALETTE_WORD_CE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CE_POS)
#define OSD_BLEND_PALETTE_WORD_CE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CE_POS))

/* 0x73C : PALE_SRAM_CF */
#define OSD_BLEND_PALE_SRAM_CF_OFFSET                           (0x73C)
#define OSD_BLEND_PALETTE_WORD_CF                               OSD_BLEND_PALETTE_WORD_CF
#define OSD_BLEND_PALETTE_WORD_CF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_CF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_CF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_CF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CF_POS)
#define OSD_BLEND_PALETTE_WORD_CF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_CF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_CF_POS))

/* 0x740 : PALE_SRAM_D0 */
#define OSD_BLEND_PALE_SRAM_D0_OFFSET                           (0x740)
#define OSD_BLEND_PALETTE_WORD_D0                               OSD_BLEND_PALETTE_WORD_D0
#define OSD_BLEND_PALETTE_WORD_D0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D0_POS)
#define OSD_BLEND_PALETTE_WORD_D0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D0_POS))

/* 0x744 : PALE_SRAM_D1 */
#define OSD_BLEND_PALE_SRAM_D1_OFFSET                           (0x744)
#define OSD_BLEND_PALETTE_WORD_D1                               OSD_BLEND_PALETTE_WORD_D1
#define OSD_BLEND_PALETTE_WORD_D1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D1_POS)
#define OSD_BLEND_PALETTE_WORD_D1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D1_POS))

/* 0x748 : PALE_SRAM_D2 */
#define OSD_BLEND_PALE_SRAM_D2_OFFSET                           (0x748)
#define OSD_BLEND_PALETTE_WORD_D2                               OSD_BLEND_PALETTE_WORD_D2
#define OSD_BLEND_PALETTE_WORD_D2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D2_POS)
#define OSD_BLEND_PALETTE_WORD_D2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D2_POS))

/* 0x74C : PALE_SRAM_D3 */
#define OSD_BLEND_PALE_SRAM_D3_OFFSET                           (0x74C)
#define OSD_BLEND_PALETTE_WORD_D3                               OSD_BLEND_PALETTE_WORD_D3
#define OSD_BLEND_PALETTE_WORD_D3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D3_POS)
#define OSD_BLEND_PALETTE_WORD_D3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D3_POS))

/* 0x750 : PALE_SRAM_D4 */
#define OSD_BLEND_PALE_SRAM_D4_OFFSET                           (0x750)
#define OSD_BLEND_PALETTE_WORD_D4                               OSD_BLEND_PALETTE_WORD_D4
#define OSD_BLEND_PALETTE_WORD_D4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D4_POS)
#define OSD_BLEND_PALETTE_WORD_D4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D4_POS))

/* 0x754 : PALE_SRAM_D5 */
#define OSD_BLEND_PALE_SRAM_D5_OFFSET                           (0x754)
#define OSD_BLEND_PALETTE_WORD_D5                               OSD_BLEND_PALETTE_WORD_D5
#define OSD_BLEND_PALETTE_WORD_D5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D5_POS)
#define OSD_BLEND_PALETTE_WORD_D5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D5_POS))

/* 0x758 : PALE_SRAM_D6 */
#define OSD_BLEND_PALE_SRAM_D6_OFFSET                           (0x758)
#define OSD_BLEND_PALETTE_WORD_D6                               OSD_BLEND_PALETTE_WORD_D6
#define OSD_BLEND_PALETTE_WORD_D6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D6_POS)
#define OSD_BLEND_PALETTE_WORD_D6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D6_POS))

/* 0x75C : PALE_SRAM_D7 */
#define OSD_BLEND_PALE_SRAM_D7_OFFSET                           (0x75C)
#define OSD_BLEND_PALETTE_WORD_D7                               OSD_BLEND_PALETTE_WORD_D7
#define OSD_BLEND_PALETTE_WORD_D7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D7_POS)
#define OSD_BLEND_PALETTE_WORD_D7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D7_POS))

/* 0x760 : PALE_SRAM_D8 */
#define OSD_BLEND_PALE_SRAM_D8_OFFSET                           (0x760)
#define OSD_BLEND_PALETTE_WORD_D8                               OSD_BLEND_PALETTE_WORD_D8
#define OSD_BLEND_PALETTE_WORD_D8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D8_POS)
#define OSD_BLEND_PALETTE_WORD_D8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D8_POS))

/* 0x764 : PALE_SRAM_D9 */
#define OSD_BLEND_PALE_SRAM_D9_OFFSET                           (0x764)
#define OSD_BLEND_PALETTE_WORD_D9                               OSD_BLEND_PALETTE_WORD_D9
#define OSD_BLEND_PALETTE_WORD_D9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_D9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_D9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_D9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D9_POS)
#define OSD_BLEND_PALETTE_WORD_D9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_D9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_D9_POS))

/* 0x768 : PALE_SRAM_DA */
#define OSD_BLEND_PALE_SRAM_DA_OFFSET                           (0x768)
#define OSD_BLEND_PALETTE_WORD_DA                               OSD_BLEND_PALETTE_WORD_DA
#define OSD_BLEND_PALETTE_WORD_DA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DA_POS)
#define OSD_BLEND_PALETTE_WORD_DA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DA_POS))

/* 0x76C : PALE_SRAM_DB */
#define OSD_BLEND_PALE_SRAM_DB_OFFSET                           (0x76C)
#define OSD_BLEND_PALETTE_WORD_DB                               OSD_BLEND_PALETTE_WORD_DB
#define OSD_BLEND_PALETTE_WORD_DB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DB_POS)
#define OSD_BLEND_PALETTE_WORD_DB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DB_POS))

/* 0x770 : PALE_SRAM_DC */
#define OSD_BLEND_PALE_SRAM_DC_OFFSET                           (0x770)
#define OSD_BLEND_PALETTE_WORD_DC                               OSD_BLEND_PALETTE_WORD_DC
#define OSD_BLEND_PALETTE_WORD_DC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DC_POS)
#define OSD_BLEND_PALETTE_WORD_DC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DC_POS))

/* 0x774 : PALE_SRAM_DD */
#define OSD_BLEND_PALE_SRAM_DD_OFFSET                           (0x774)
#define OSD_BLEND_PALETTE_WORD_DD                               OSD_BLEND_PALETTE_WORD_DD
#define OSD_BLEND_PALETTE_WORD_DD_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DD_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DD_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DD_POS)
#define OSD_BLEND_PALETTE_WORD_DD_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DD_POS))

/* 0x778 : PALE_SRAM_DE */
#define OSD_BLEND_PALE_SRAM_DE_OFFSET                           (0x778)
#define OSD_BLEND_PALETTE_WORD_DE                               OSD_BLEND_PALETTE_WORD_DE
#define OSD_BLEND_PALETTE_WORD_DE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DE_POS)
#define OSD_BLEND_PALETTE_WORD_DE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DE_POS))

/* 0x77C : PALE_SRAM_DF */
#define OSD_BLEND_PALE_SRAM_DF_OFFSET                           (0x77C)
#define OSD_BLEND_PALETTE_WORD_DF                               OSD_BLEND_PALETTE_WORD_DF
#define OSD_BLEND_PALETTE_WORD_DF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_DF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_DF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_DF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DF_POS)
#define OSD_BLEND_PALETTE_WORD_DF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_DF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_DF_POS))

/* 0x780 : PALE_SRAM_E0 */
#define OSD_BLEND_PALE_SRAM_E0_OFFSET                           (0x780)
#define OSD_BLEND_PALETTE_WORD_E0                               OSD_BLEND_PALETTE_WORD_E0
#define OSD_BLEND_PALETTE_WORD_E0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E0_POS)
#define OSD_BLEND_PALETTE_WORD_E0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E0_POS))

/* 0x784 : PALE_SRAM_E1 */
#define OSD_BLEND_PALE_SRAM_E1_OFFSET                           (0x784)
#define OSD_BLEND_PALETTE_WORD_E1                               OSD_BLEND_PALETTE_WORD_E1
#define OSD_BLEND_PALETTE_WORD_E1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E1_POS)
#define OSD_BLEND_PALETTE_WORD_E1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E1_POS))

/* 0x788 : PALE_SRAM_E2 */
#define OSD_BLEND_PALE_SRAM_E2_OFFSET                           (0x788)
#define OSD_BLEND_PALETTE_WORD_E2                               OSD_BLEND_PALETTE_WORD_E2
#define OSD_BLEND_PALETTE_WORD_E2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E2_POS)
#define OSD_BLEND_PALETTE_WORD_E2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E2_POS))

/* 0x78C : PALE_SRAM_E3 */
#define OSD_BLEND_PALE_SRAM_E3_OFFSET                           (0x78C)
#define OSD_BLEND_PALETTE_WORD_E3                               OSD_BLEND_PALETTE_WORD_E3
#define OSD_BLEND_PALETTE_WORD_E3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E3_POS)
#define OSD_BLEND_PALETTE_WORD_E3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E3_POS))

/* 0x790 : PALE_SRAM_E4 */
#define OSD_BLEND_PALE_SRAM_E4_OFFSET                           (0x790)
#define OSD_BLEND_PALETTE_WORD_E4                               OSD_BLEND_PALETTE_WORD_E4
#define OSD_BLEND_PALETTE_WORD_E4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E4_POS)
#define OSD_BLEND_PALETTE_WORD_E4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E4_POS))

/* 0x794 : PALE_SRAM_E5 */
#define OSD_BLEND_PALE_SRAM_E5_OFFSET                           (0x794)
#define OSD_BLEND_PALETTE_WORD_E5                               OSD_BLEND_PALETTE_WORD_E5
#define OSD_BLEND_PALETTE_WORD_E5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E5_POS)
#define OSD_BLEND_PALETTE_WORD_E5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E5_POS))

/* 0x798 : PALE_SRAM_E6 */
#define OSD_BLEND_PALE_SRAM_E6_OFFSET                           (0x798)
#define OSD_BLEND_PALETTE_WORD_E6                               OSD_BLEND_PALETTE_WORD_E6
#define OSD_BLEND_PALETTE_WORD_E6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E6_POS)
#define OSD_BLEND_PALETTE_WORD_E6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E6_POS))

/* 0x79C : PALE_SRAM_E7 */
#define OSD_BLEND_PALE_SRAM_E7_OFFSET                           (0x79C)
#define OSD_BLEND_PALETTE_WORD_E7                               OSD_BLEND_PALETTE_WORD_E7
#define OSD_BLEND_PALETTE_WORD_E7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E7_POS)
#define OSD_BLEND_PALETTE_WORD_E7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E7_POS))

/* 0x7A0 : PALE_SRAM_E8 */
#define OSD_BLEND_PALE_SRAM_E8_OFFSET                           (0x7A0)
#define OSD_BLEND_PALETTE_WORD_E8                               OSD_BLEND_PALETTE_WORD_E8
#define OSD_BLEND_PALETTE_WORD_E8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E8_POS)
#define OSD_BLEND_PALETTE_WORD_E8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E8_POS))

/* 0x7A4 : PALE_SRAM_E9 */
#define OSD_BLEND_PALE_SRAM_E9_OFFSET                           (0x7A4)
#define OSD_BLEND_PALETTE_WORD_E9                               OSD_BLEND_PALETTE_WORD_E9
#define OSD_BLEND_PALETTE_WORD_E9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_E9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_E9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_E9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E9_POS)
#define OSD_BLEND_PALETTE_WORD_E9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_E9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_E9_POS))

/* 0x7A8 : PALE_SRAM_EA */
#define OSD_BLEND_PALE_SRAM_EA_OFFSET                           (0x7A8)
#define OSD_BLEND_PALETTE_WORD_EA                               OSD_BLEND_PALETTE_WORD_EA
#define OSD_BLEND_PALETTE_WORD_EA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_EA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_EA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_EA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EA_POS)
#define OSD_BLEND_PALETTE_WORD_EA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_EA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EA_POS))

/* 0x7AC : PALE_SRAM_EB */
#define OSD_BLEND_PALE_SRAM_EB_OFFSET                           (0x7AC)
#define OSD_BLEND_PALETTE_WORD_EB                               OSD_BLEND_PALETTE_WORD_EB
#define OSD_BLEND_PALETTE_WORD_EB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_EB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_EB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_EB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EB_POS)
#define OSD_BLEND_PALETTE_WORD_EB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_EB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EB_POS))

/* 0x7B0 : PALE_SRAM_EC */
#define OSD_BLEND_PALE_SRAM_EC_OFFSET                           (0x7B0)
#define OSD_BLEND_PALETTE_WORD_EC                               OSD_BLEND_PALETTE_WORD_EC
#define OSD_BLEND_PALETTE_WORD_EC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_EC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_EC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_EC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EC_POS)
#define OSD_BLEND_PALETTE_WORD_EC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_EC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EC_POS))

/* 0x7B4 : PALE_SRAM_ED */
#define OSD_BLEND_PALE_SRAM_ED_OFFSET                           (0x7B4)
#define OSD_BLEND_PALETTE_WORD_ED                               OSD_BLEND_PALETTE_WORD_ED
#define OSD_BLEND_PALETTE_WORD_ED_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_ED_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_ED_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_ED_LEN)-1)<<OSD_BLEND_PALETTE_WORD_ED_POS)
#define OSD_BLEND_PALETTE_WORD_ED_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_ED_LEN)-1)<<OSD_BLEND_PALETTE_WORD_ED_POS))

/* 0x7B8 : PALE_SRAM_EE */
#define OSD_BLEND_PALE_SRAM_EE_OFFSET                           (0x7B8)
#define OSD_BLEND_PALETTE_WORD_EE                               OSD_BLEND_PALETTE_WORD_EE
#define OSD_BLEND_PALETTE_WORD_EE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_EE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_EE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_EE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EE_POS)
#define OSD_BLEND_PALETTE_WORD_EE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_EE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EE_POS))

/* 0x7BC : PALE_SRAM_EF */
#define OSD_BLEND_PALE_SRAM_EF_OFFSET                           (0x7BC)
#define OSD_BLEND_PALETTE_WORD_EF                               OSD_BLEND_PALETTE_WORD_EF
#define OSD_BLEND_PALETTE_WORD_EF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_EF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_EF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_EF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EF_POS)
#define OSD_BLEND_PALETTE_WORD_EF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_EF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_EF_POS))

/* 0x7C0 : PALE_SRAM_F0 */
#define OSD_BLEND_PALE_SRAM_F0_OFFSET                           (0x7C0)
#define OSD_BLEND_PALETTE_WORD_F0                               OSD_BLEND_PALETTE_WORD_F0
#define OSD_BLEND_PALETTE_WORD_F0_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F0_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F0_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F0_POS)
#define OSD_BLEND_PALETTE_WORD_F0_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F0_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F0_POS))

/* 0x7C4 : PALE_SRAM_F1 */
#define OSD_BLEND_PALE_SRAM_F1_OFFSET                           (0x7C4)
#define OSD_BLEND_PALETTE_WORD_F1                               OSD_BLEND_PALETTE_WORD_F1
#define OSD_BLEND_PALETTE_WORD_F1_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F1_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F1_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F1_POS)
#define OSD_BLEND_PALETTE_WORD_F1_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F1_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F1_POS))

/* 0x7C8 : PALE_SRAM_F2 */
#define OSD_BLEND_PALE_SRAM_F2_OFFSET                           (0x7C8)
#define OSD_BLEND_PALETTE_WORD_F2                               OSD_BLEND_PALETTE_WORD_F2
#define OSD_BLEND_PALETTE_WORD_F2_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F2_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F2_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F2_POS)
#define OSD_BLEND_PALETTE_WORD_F2_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F2_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F2_POS))

/* 0x7CC : PALE_SRAM_F3 */
#define OSD_BLEND_PALE_SRAM_F3_OFFSET                           (0x7CC)
#define OSD_BLEND_PALETTE_WORD_F3                               OSD_BLEND_PALETTE_WORD_F3
#define OSD_BLEND_PALETTE_WORD_F3_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F3_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F3_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F3_POS)
#define OSD_BLEND_PALETTE_WORD_F3_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F3_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F3_POS))

/* 0x7D0 : PALE_SRAM_F4 */
#define OSD_BLEND_PALE_SRAM_F4_OFFSET                           (0x7D0)
#define OSD_BLEND_PALETTE_WORD_F4                               OSD_BLEND_PALETTE_WORD_F4
#define OSD_BLEND_PALETTE_WORD_F4_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F4_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F4_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F4_POS)
#define OSD_BLEND_PALETTE_WORD_F4_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F4_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F4_POS))

/* 0x7D4 : PALE_SRAM_F5 */
#define OSD_BLEND_PALE_SRAM_F5_OFFSET                           (0x7D4)
#define OSD_BLEND_PALETTE_WORD_F5                               OSD_BLEND_PALETTE_WORD_F5
#define OSD_BLEND_PALETTE_WORD_F5_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F5_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F5_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F5_POS)
#define OSD_BLEND_PALETTE_WORD_F5_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F5_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F5_POS))

/* 0x7D8 : PALE_SRAM_F6 */
#define OSD_BLEND_PALE_SRAM_F6_OFFSET                           (0x7D8)
#define OSD_BLEND_PALETTE_WORD_F6                               OSD_BLEND_PALETTE_WORD_F6
#define OSD_BLEND_PALETTE_WORD_F6_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F6_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F6_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F6_POS)
#define OSD_BLEND_PALETTE_WORD_F6_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F6_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F6_POS))

/* 0x7DC : PALE_SRAM_F7 */
#define OSD_BLEND_PALE_SRAM_F7_OFFSET                           (0x7DC)
#define OSD_BLEND_PALETTE_WORD_F7                               OSD_BLEND_PALETTE_WORD_F7
#define OSD_BLEND_PALETTE_WORD_F7_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F7_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F7_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F7_POS)
#define OSD_BLEND_PALETTE_WORD_F7_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F7_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F7_POS))

/* 0x7E0 : PALE_SRAM_F8 */
#define OSD_BLEND_PALE_SRAM_F8_OFFSET                           (0x7E0)
#define OSD_BLEND_PALETTE_WORD_F8                               OSD_BLEND_PALETTE_WORD_F8
#define OSD_BLEND_PALETTE_WORD_F8_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F8_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F8_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F8_POS)
#define OSD_BLEND_PALETTE_WORD_F8_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F8_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F8_POS))

/* 0x7E4 : PALE_SRAM_F9 */
#define OSD_BLEND_PALE_SRAM_F9_OFFSET                           (0x7E4)
#define OSD_BLEND_PALETTE_WORD_F9                               OSD_BLEND_PALETTE_WORD_F9
#define OSD_BLEND_PALETTE_WORD_F9_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_F9_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_F9_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_F9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F9_POS)
#define OSD_BLEND_PALETTE_WORD_F9_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_F9_LEN)-1)<<OSD_BLEND_PALETTE_WORD_F9_POS))

/* 0x7E8 : PALE_SRAM_FA */
#define OSD_BLEND_PALE_SRAM_FA_OFFSET                           (0x7E8)
#define OSD_BLEND_PALETTE_WORD_FA                               OSD_BLEND_PALETTE_WORD_FA
#define OSD_BLEND_PALETTE_WORD_FA_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FA_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FA_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FA_POS)
#define OSD_BLEND_PALETTE_WORD_FA_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FA_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FA_POS))

/* 0x7EC : PALE_SRAM_FB */
#define OSD_BLEND_PALE_SRAM_FB_OFFSET                           (0x7EC)
#define OSD_BLEND_PALETTE_WORD_FB                               OSD_BLEND_PALETTE_WORD_FB
#define OSD_BLEND_PALETTE_WORD_FB_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FB_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FB_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FB_POS)
#define OSD_BLEND_PALETTE_WORD_FB_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FB_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FB_POS))

/* 0x7F0 : PALE_SRAM_FC */
#define OSD_BLEND_PALE_SRAM_FC_OFFSET                           (0x7F0)
#define OSD_BLEND_PALETTE_WORD_FC                               OSD_BLEND_PALETTE_WORD_FC
#define OSD_BLEND_PALETTE_WORD_FC_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FC_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FC_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FC_POS)
#define OSD_BLEND_PALETTE_WORD_FC_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FC_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FC_POS))

/* 0x7F4 : PALE_SRAM_FD */
#define OSD_BLEND_PALE_SRAM_FD_OFFSET                           (0x7F4)
#define OSD_BLEND_PALETTE_WORD_FD                               OSD_BLEND_PALETTE_WORD_FD
#define OSD_BLEND_PALETTE_WORD_FD_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FD_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FD_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FD_POS)
#define OSD_BLEND_PALETTE_WORD_FD_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FD_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FD_POS))

/* 0x7F8 : PALE_SRAM_FE */
#define OSD_BLEND_PALE_SRAM_FE_OFFSET                           (0x7F8)
#define OSD_BLEND_PALETTE_WORD_FE                               OSD_BLEND_PALETTE_WORD_FE
#define OSD_BLEND_PALETTE_WORD_FE_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FE_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FE_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FE_POS)
#define OSD_BLEND_PALETTE_WORD_FE_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FE_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FE_POS))

/* 0x7FC : PALE_SRAM_FF */
#define OSD_BLEND_PALE_SRAM_FF_OFFSET                           (0x7FC)
#define OSD_BLEND_PALETTE_WORD_FF                               OSD_BLEND_PALETTE_WORD_FF
#define OSD_BLEND_PALETTE_WORD_FF_POS                           (0U)
#define OSD_BLEND_PALETTE_WORD_FF_LEN                           (32U)
#define OSD_BLEND_PALETTE_WORD_FF_MSK                           (((1U<<OSD_BLEND_PALETTE_WORD_FF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FF_POS)
#define OSD_BLEND_PALETTE_WORD_FF_UMSK                          (~(((1U<<OSD_BLEND_PALETTE_WORD_FF_LEN)-1)<<OSD_BLEND_PALETTE_WORD_FF_POS))


struct  osd_blend_reg {
    /* 0x0 : obnd_layer_xconfig */
    union {
        struct {
            uint32_t obnd_x_min                     : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t obnd_x_max                     : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_xconfig;

    /* 0x4 : obnd_layer_yconfig */
    union {
        struct {
            uint32_t obnd_y_min                     : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t obnd_y_max                     : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_yconfig;

    /* 0x8 : obnd_mem_config0 */
    union {
        struct {
            uint32_t obnd_force_sh                  :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_osd_bus_strt_w             :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_osd_seof_int_clr_w         :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_15                  : 13; /* [15: 3],       rsvd,        0x0 */
            uint32_t sts_osd_cmd_idle_r             :  1; /* [   16],          r,        0x0 */
            uint32_t sts_osd_axi_idle_r             :  1; /* [   17],          r,        0x0 */
            uint32_t sts_osd_drain_int_r            :  1; /* [   18],          r,        0x0 */
            uint32_t sts_osd_seof_int_r             :  1; /* [   19],          r,        0x0 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_mem_config0;

    /* 0xc  reserved */
    uint8_t RESERVED0xc[4];

    /* 0x10 : osd_misc */
    union {
        struct {
            uint32_t obnd_layer_en                  :  1; /* [    0],        r/w,        0x0 */
            uint32_t reg_qos_sw_mode_w              :  1; /* [    1],        r/w,        0x0 */
            uint32_t reg_qos_sw_w                   :  1; /* [    2],        r/w,        0x0 */
            uint32_t reserved_3                     :  1; /* [    3],       rsvd,        0x0 */
            uint32_t reg_osd_ostd_max_w             :  2; /* [ 5: 4],        r/w,        0x1 */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_xlen_w                     :  3; /* [10: 8],        r/w,        0x4 */
            uint32_t reserved_11_16                 :  6; /* [16:11],       rsvd,        0x0 */
            uint32_t reg_osd_seof_int_mask_w        :  1; /* [   17],        r/w,        0x0 */
            uint32_t reg_osd_seof_int_edge_w        :  1; /* [   18],        r/w,        0x0 */
            uint32_t reserved_19_31                 : 13; /* [31:19],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } osd_misc;

    /* 0x14 : obnd_layer_config0 */
    union {
        struct {
            uint32_t obnd_color_format              :  5; /* [ 4: 0],        r/w,        0x0 */
            uint32_t reserved_5_7                   :  3; /* [ 7: 5],       rsvd,        0x0 */
            uint32_t obnd_order_a                   :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t obnd_order_rv                  :  2; /* [11:10],        r/w,        0x0 */
            uint32_t obnd_order_gy                  :  2; /* [13:12],        r/w,        0x0 */
            uint32_t obnd_order_bu                  :  2; /* [15:14],        r/w,        0x0 */
            uint32_t obnd_global_a_en               :  1; /* [   16],        r/w,        0x0 */
            uint32_t reserved_17_23                 :  7; /* [23:17],       rsvd,        0x0 */
            uint32_t obnd_global_a                  :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config0;

    /* 0x18 : obnd_layer_config1 */
    union {
        struct {
            uint32_t obnd_global_color_en           :  1; /* [    0],        r/w,        0x0 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t obnd_global_rv                 :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t obnd_global_gy                 :  8; /* [23:16],        r/w,        0x0 */
            uint32_t obnd_global_bu                 :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config1;

    /* 0x1C : obnd_layer_config2 */
    union {
        struct {
            uint32_t obnd_key_palette_en            :  1; /* [    0],        r/w,        0x0 */
            uint32_t obnd_key_index_inv             :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_7                   :  6; /* [ 7: 2],       rsvd,        0x0 */
            uint32_t obnd_key_index_min             :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t obnd_key_index_max             :  8; /* [23:16],        r/w,        0x0 */
            uint32_t obnd_key_replace_index         :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config2;

    /* 0x20 : obnd_layer_config3 */
    union {
        struct {
            uint32_t obnd_key_a_min                 :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t obnd_key_a_max                 :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t obnd_key_rv_min                :  8; /* [23:16],        r/w,        0x0 */
            uint32_t obnd_key_rv_max                :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config3;

    /* 0x24 : obnd_layer_config4 */
    union {
        struct {
            uint32_t obnd_key_gy_min                :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t obnd_key_gy_max                :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t obnd_key_bu_min                :  8; /* [23:16],        r/w,        0x0 */
            uint32_t obnd_key_bu_max                :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config4;

    /* 0x28 : obnd_layer_config5 */
    union {
        struct {
            uint32_t obnd_key_replace_a             :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t obnd_key_replace_rv            :  8; /* [15: 8],        r/w,        0x0 */
            uint32_t obnd_key_replace_gy            :  8; /* [23:16],        r/w,        0x0 */
            uint32_t obnd_key_replace_bu            :  8; /* [31:24],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config5;

    /* 0x2C : obnd_layer_config6 */
    union {
        struct {
            uint32_t obnd_key_color_en              :  1; /* [    0],        r/w,        0x0 */
            uint32_t obnd_key_a_inv                 :  1; /* [    1],        r/w,        0x0 */
            uint32_t obnd_key_rv_inv                :  1; /* [    2],        r/w,        0x0 */
            uint32_t obnd_key_gy_inv                :  1; /* [    3],        r/w,        0x0 */
            uint32_t obnd_key_bu_inv                :  1; /* [    4],        r/w,        0x0 */
            uint32_t reserved_5_31                  : 27; /* [31: 5],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config6;

    /* 0x30  reserved */
    uint8_t RESERVED0x30[4];

    /* 0x34 : obnd_layer_config8 */
    union {
        struct {
            uint32_t obnd_alpha_0                   :  8; /* [ 7: 0],        r/w,        0x0 */
            uint32_t obnd_alpha_1                   :  8; /* [15: 8],        r/w,       0xff */
            uint32_t reserved_16_31                 : 16; /* [31:16],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_layer_config8;

    /* 0x38  reserved */
    uint8_t RESERVED0x38[8];

    /* 0x40 : obnd_error */
    union {
        struct {
            uint32_t reg_rfifo_drain_clr_w          :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_rfifo_drain_mask_w         :  1; /* [    1],        r/w,        0x0 */
            uint32_t reserved_2_15                  : 14; /* [15: 2],       rsvd,        0x0 */
            uint32_t sts_rfifo_drain_r              :  1; /* [   16],          r,        0x0 */
            uint32_t reserved_17_31                 : 15; /* [31:17],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_error;

    /* 0x44 : obnd_sh */
    union {
        struct {
            uint32_t obnd_layer_mem_req_cnt         : 32; /* [31: 0],        r/w,     0x1000 */
        }BF;
        uint32_t WORD;
    } obnd_sh;

    /* 0x48  reserved */
    uint8_t RESERVED0x48[24];

    /* 0x60 : obnd_mem_config1 */
    union {
        struct {
            uint32_t reg_addr_start                 : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_mem_config1;

    /* 0x64 : obnd_mem_config2 */
    union {
        struct {
            uint32_t reg_frame_width_byte_x8        : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_stride_byte_x8             : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_mem_config2;

    /* 0x68 : obnd_mem_config3 */
    union {
        struct {
            uint32_t reg_frame_height               : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t reg_line_head_fix_bit          :  6; /* [21:16],        r/w,        0x0 */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_line_tail_fix_bit          :  6; /* [29:24],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } obnd_mem_config3;

    /* 0x6c  reserved */
    uint8_t RESERVED0x6c[916];

    /* 0x400 : PALE_SRAM_00 */
    union {
        struct {
            uint32_t palette_word_00                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_00;

    /* 0x404 : PALE_SRAM_01 */
    union {
        struct {
            uint32_t palette_word_01                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_01;

    /* 0x408 : PALE_SRAM_02 */
    union {
        struct {
            uint32_t palette_word_02                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_02;

    /* 0x40C : PALE_SRAM_03 */
    union {
        struct {
            uint32_t palette_word_03                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_03;

    /* 0x410 : PALE_SRAM_04 */
    union {
        struct {
            uint32_t palette_word_04                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_04;

    /* 0x414 : PALE_SRAM_05 */
    union {
        struct {
            uint32_t palette_word_05                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_05;

    /* 0x418 : PALE_SRAM_06 */
    union {
        struct {
            uint32_t palette_word_06                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_06;

    /* 0x41C : PALE_SRAM_07 */
    union {
        struct {
            uint32_t palette_word_07                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_07;

    /* 0x420 : PALE_SRAM_08 */
    union {
        struct {
            uint32_t palette_word_08                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_08;

    /* 0x424 : PALE_SRAM_09 */
    union {
        struct {
            uint32_t palette_word_09                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_09;

    /* 0x428 : PALE_SRAM_0A */
    union {
        struct {
            uint32_t palette_word_0A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0A;

    /* 0x42C : PALE_SRAM_0B */
    union {
        struct {
            uint32_t palette_word_0B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0B;

    /* 0x430 : PALE_SRAM_0C */
    union {
        struct {
            uint32_t palette_word_0C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0C;

    /* 0x434 : PALE_SRAM_0D */
    union {
        struct {
            uint32_t palette_word_0D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0D;

    /* 0x438 : PALE_SRAM_0E */
    union {
        struct {
            uint32_t palette_word_0E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0E;

    /* 0x43C : PALE_SRAM_0F */
    union {
        struct {
            uint32_t palette_word_0F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_0F;

    /* 0x440 : PALE_SRAM_10 */
    union {
        struct {
            uint32_t palette_word_10                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_10;

    /* 0x444 : PALE_SRAM_11 */
    union {
        struct {
            uint32_t palette_word_11                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_11;

    /* 0x448 : PALE_SRAM_12 */
    union {
        struct {
            uint32_t palette_word_12                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_12;

    /* 0x44C : PALE_SRAM_13 */
    union {
        struct {
            uint32_t palette_word_13                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_13;

    /* 0x450 : PALE_SRAM_14 */
    union {
        struct {
            uint32_t palette_word_14                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_14;

    /* 0x454 : PALE_SRAM_15 */
    union {
        struct {
            uint32_t palette_word_15                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_15;

    /* 0x458 : PALE_SRAM_16 */
    union {
        struct {
            uint32_t palette_word_16                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_16;

    /* 0x45C : PALE_SRAM_17 */
    union {
        struct {
            uint32_t palette_word_17                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_17;

    /* 0x460 : PALE_SRAM_18 */
    union {
        struct {
            uint32_t palette_word_18                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_18;

    /* 0x464 : PALE_SRAM_19 */
    union {
        struct {
            uint32_t palette_word_19                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_19;

    /* 0x468 : PALE_SRAM_1A */
    union {
        struct {
            uint32_t palette_word_1A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1A;

    /* 0x46C : PALE_SRAM_1B */
    union {
        struct {
            uint32_t palette_word_1B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1B;

    /* 0x470 : PALE_SRAM_1C */
    union {
        struct {
            uint32_t palette_word_1C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1C;

    /* 0x474 : PALE_SRAM_1D */
    union {
        struct {
            uint32_t palette_word_1D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1D;

    /* 0x478 : PALE_SRAM_1E */
    union {
        struct {
            uint32_t palette_word_1E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1E;

    /* 0x47C : PALE_SRAM_1F */
    union {
        struct {
            uint32_t palette_word_1F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_1F;

    /* 0x480 : PALE_SRAM_20 */
    union {
        struct {
            uint32_t palette_word_20                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_20;

    /* 0x484 : PALE_SRAM_21 */
    union {
        struct {
            uint32_t palette_word_21                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_21;

    /* 0x488 : PALE_SRAM_22 */
    union {
        struct {
            uint32_t palette_word_22                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_22;

    /* 0x48C : PALE_SRAM_23 */
    union {
        struct {
            uint32_t palette_word_23                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_23;

    /* 0x490 : PALE_SRAM_24 */
    union {
        struct {
            uint32_t palette_word_24                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_24;

    /* 0x494 : PALE_SRAM_25 */
    union {
        struct {
            uint32_t palette_word_25                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_25;

    /* 0x498 : PALE_SRAM_26 */
    union {
        struct {
            uint32_t palette_word_26                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_26;

    /* 0x49C : PALE_SRAM_27 */
    union {
        struct {
            uint32_t palette_word_27                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_27;

    /* 0x4A0 : PALE_SRAM_28 */
    union {
        struct {
            uint32_t palette_word_28                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_28;

    /* 0x4A4 : PALE_SRAM_29 */
    union {
        struct {
            uint32_t palette_word_29                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_29;

    /* 0x4A8 : PALE_SRAM_2A */
    union {
        struct {
            uint32_t palette_word_2A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2A;

    /* 0x4AC : PALE_SRAM_2B */
    union {
        struct {
            uint32_t palette_word_2B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2B;

    /* 0x4B0 : PALE_SRAM_2C */
    union {
        struct {
            uint32_t palette_word_2C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2C;

    /* 0x4B4 : PALE_SRAM_2D */
    union {
        struct {
            uint32_t palette_word_2D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2D;

    /* 0x4B8 : PALE_SRAM_2E */
    union {
        struct {
            uint32_t palette_word_2E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2E;

    /* 0x4BC : PALE_SRAM_2F */
    union {
        struct {
            uint32_t palette_word_2F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_2F;

    /* 0x4C0 : PALE_SRAM_30 */
    union {
        struct {
            uint32_t palette_word_30                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_30;

    /* 0x4C4 : PALE_SRAM_31 */
    union {
        struct {
            uint32_t palette_word_31                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_31;

    /* 0x4C8 : PALE_SRAM_32 */
    union {
        struct {
            uint32_t palette_word_32                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_32;

    /* 0x4CC : PALE_SRAM_33 */
    union {
        struct {
            uint32_t palette_word_33                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_33;

    /* 0x4D0 : PALE_SRAM_34 */
    union {
        struct {
            uint32_t palette_word_34                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_34;

    /* 0x4D4 : PALE_SRAM_35 */
    union {
        struct {
            uint32_t palette_word_35                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_35;

    /* 0x4D8 : PALE_SRAM_36 */
    union {
        struct {
            uint32_t palette_word_36                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_36;

    /* 0x4DC : PALE_SRAM_37 */
    union {
        struct {
            uint32_t palette_word_37                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_37;

    /* 0x4E0 : PALE_SRAM_38 */
    union {
        struct {
            uint32_t palette_word_38                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_38;

    /* 0x4E4 : PALE_SRAM_39 */
    union {
        struct {
            uint32_t palette_word_39                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_39;

    /* 0x4E8 : PALE_SRAM_3A */
    union {
        struct {
            uint32_t palette_word_3A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3A;

    /* 0x4EC : PALE_SRAM_3B */
    union {
        struct {
            uint32_t palette_word_3B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3B;

    /* 0x4F0 : PALE_SRAM_3C */
    union {
        struct {
            uint32_t palette_word_3C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3C;

    /* 0x4F4 : PALE_SRAM_3D */
    union {
        struct {
            uint32_t palette_word_3D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3D;

    /* 0x4F8 : PALE_SRAM_3E */
    union {
        struct {
            uint32_t palette_word_3E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3E;

    /* 0x4FC : PALE_SRAM_3F */
    union {
        struct {
            uint32_t palette_word_3F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_3F;

    /* 0x500 : PALE_SRAM_40 */
    union {
        struct {
            uint32_t palette_word_40                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_40;

    /* 0x504 : PALE_SRAM_41 */
    union {
        struct {
            uint32_t palette_word_41                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_41;

    /* 0x508 : PALE_SRAM_42 */
    union {
        struct {
            uint32_t palette_word_42                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_42;

    /* 0x50C : PALE_SRAM_43 */
    union {
        struct {
            uint32_t palette_word_43                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_43;

    /* 0x510 : PALE_SRAM_44 */
    union {
        struct {
            uint32_t palette_word_44                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_44;

    /* 0x514 : PALE_SRAM_45 */
    union {
        struct {
            uint32_t palette_word_45                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_45;

    /* 0x518 : PALE_SRAM_46 */
    union {
        struct {
            uint32_t palette_word_46                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_46;

    /* 0x51C : PALE_SRAM_47 */
    union {
        struct {
            uint32_t palette_word_47                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_47;

    /* 0x520 : PALE_SRAM_48 */
    union {
        struct {
            uint32_t palette_word_48                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_48;

    /* 0x524 : PALE_SRAM_49 */
    union {
        struct {
            uint32_t palette_word_49                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_49;

    /* 0x528 : PALE_SRAM_4A */
    union {
        struct {
            uint32_t palette_word_4A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4A;

    /* 0x52C : PALE_SRAM_4B */
    union {
        struct {
            uint32_t palette_word_4B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4B;

    /* 0x530 : PALE_SRAM_4C */
    union {
        struct {
            uint32_t palette_word_4C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4C;

    /* 0x534 : PALE_SRAM_4D */
    union {
        struct {
            uint32_t palette_word_4D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4D;

    /* 0x538 : PALE_SRAM_4E */
    union {
        struct {
            uint32_t palette_word_4E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4E;

    /* 0x53C : PALE_SRAM_4F */
    union {
        struct {
            uint32_t palette_word_4F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_4F;

    /* 0x540 : PALE_SRAM_50 */
    union {
        struct {
            uint32_t palette_word_50                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_50;

    /* 0x544 : PALE_SRAM_51 */
    union {
        struct {
            uint32_t palette_word_51                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_51;

    /* 0x548 : PALE_SRAM_52 */
    union {
        struct {
            uint32_t palette_word_52                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_52;

    /* 0x54C : PALE_SRAM_53 */
    union {
        struct {
            uint32_t palette_word_53                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_53;

    /* 0x550 : PALE_SRAM_54 */
    union {
        struct {
            uint32_t palette_word_54                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_54;

    /* 0x554 : PALE_SRAM_55 */
    union {
        struct {
            uint32_t palette_word_55                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_55;

    /* 0x558 : PALE_SRAM_56 */
    union {
        struct {
            uint32_t palette_word_56                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_56;

    /* 0x55C : PALE_SRAM_57 */
    union {
        struct {
            uint32_t palette_word_57                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_57;

    /* 0x560 : PALE_SRAM_58 */
    union {
        struct {
            uint32_t palette_word_58                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_58;

    /* 0x564 : PALE_SRAM_59 */
    union {
        struct {
            uint32_t palette_word_59                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_59;

    /* 0x568 : PALE_SRAM_5A */
    union {
        struct {
            uint32_t palette_word_5A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5A;

    /* 0x56C : PALE_SRAM_5B */
    union {
        struct {
            uint32_t palette_word_5B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5B;

    /* 0x570 : PALE_SRAM_5C */
    union {
        struct {
            uint32_t palette_word_5C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5C;

    /* 0x574 : PALE_SRAM_5D */
    union {
        struct {
            uint32_t palette_word_5D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5D;

    /* 0x578 : PALE_SRAM_5E */
    union {
        struct {
            uint32_t palette_word_5E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5E;

    /* 0x57C : PALE_SRAM_5F */
    union {
        struct {
            uint32_t palette_word_5F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_5F;

    /* 0x580 : PALE_SRAM_60 */
    union {
        struct {
            uint32_t palette_word_60                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_60;

    /* 0x584 : PALE_SRAM_61 */
    union {
        struct {
            uint32_t palette_word_61                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_61;

    /* 0x588 : PALE_SRAM_62 */
    union {
        struct {
            uint32_t palette_word_62                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_62;

    /* 0x58C : PALE_SRAM_63 */
    union {
        struct {
            uint32_t palette_word_63                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_63;

    /* 0x590 : PALE_SRAM_64 */
    union {
        struct {
            uint32_t palette_word_64                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_64;

    /* 0x594 : PALE_SRAM_65 */
    union {
        struct {
            uint32_t palette_word_65                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_65;

    /* 0x598 : PALE_SRAM_66 */
    union {
        struct {
            uint32_t palette_word_66                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_66;

    /* 0x59C : PALE_SRAM_67 */
    union {
        struct {
            uint32_t palette_word_67                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_67;

    /* 0x5A0 : PALE_SRAM_68 */
    union {
        struct {
            uint32_t palette_word_68                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_68;

    /* 0x5A4 : PALE_SRAM_69 */
    union {
        struct {
            uint32_t palette_word_69                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_69;

    /* 0x5A8 : PALE_SRAM_6A */
    union {
        struct {
            uint32_t palette_word_6A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6A;

    /* 0x5AC : PALE_SRAM_6B */
    union {
        struct {
            uint32_t palette_word_6B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6B;

    /* 0x5B0 : PALE_SRAM_6C */
    union {
        struct {
            uint32_t palette_word_6C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6C;

    /* 0x5B4 : PALE_SRAM_6D */
    union {
        struct {
            uint32_t palette_word_6D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6D;

    /* 0x5B8 : PALE_SRAM_6E */
    union {
        struct {
            uint32_t palette_word_6E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6E;

    /* 0x5BC : PALE_SRAM_6F */
    union {
        struct {
            uint32_t palette_word_6F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_6F;

    /* 0x5C0 : PALE_SRAM_70 */
    union {
        struct {
            uint32_t palette_word_70                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_70;

    /* 0x5C4 : PALE_SRAM_71 */
    union {
        struct {
            uint32_t palette_word_71                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_71;

    /* 0x5C8 : PALE_SRAM_72 */
    union {
        struct {
            uint32_t palette_word_72                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_72;

    /* 0x5CC : PALE_SRAM_73 */
    union {
        struct {
            uint32_t palette_word_73                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_73;

    /* 0x5D0 : PALE_SRAM_74 */
    union {
        struct {
            uint32_t palette_word_74                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_74;

    /* 0x5D4 : PALE_SRAM_75 */
    union {
        struct {
            uint32_t palette_word_75                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_75;

    /* 0x5D8 : PALE_SRAM_76 */
    union {
        struct {
            uint32_t palette_word_76                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_76;

    /* 0x5DC : PALE_SRAM_77 */
    union {
        struct {
            uint32_t palette_word_77                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_77;

    /* 0x5E0 : PALE_SRAM_78 */
    union {
        struct {
            uint32_t palette_word_78                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_78;

    /* 0x5E4 : PALE_SRAM_79 */
    union {
        struct {
            uint32_t palette_word_79                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_79;

    /* 0x5E8 : PALE_SRAM_7A */
    union {
        struct {
            uint32_t palette_word_7A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7A;

    /* 0x5EC : PALE_SRAM_7B */
    union {
        struct {
            uint32_t palette_word_7B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7B;

    /* 0x5F0 : PALE_SRAM_7C */
    union {
        struct {
            uint32_t palette_word_7C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7C;

    /* 0x5F4 : PALE_SRAM_7D */
    union {
        struct {
            uint32_t palette_word_7D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7D;

    /* 0x5F8 : PALE_SRAM_7E */
    union {
        struct {
            uint32_t palette_word_7E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7E;

    /* 0x5FC : PALE_SRAM_7F */
    union {
        struct {
            uint32_t palette_word_7F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_7F;

    /* 0x600 : PALE_SRAM_80 */
    union {
        struct {
            uint32_t palette_word_80                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_80;

    /* 0x604 : PALE_SRAM_81 */
    union {
        struct {
            uint32_t palette_word_81                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_81;

    /* 0x608 : PALE_SRAM_82 */
    union {
        struct {
            uint32_t palette_word_82                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_82;

    /* 0x60C : PALE_SRAM_83 */
    union {
        struct {
            uint32_t palette_word_83                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_83;

    /* 0x610 : PALE_SRAM_84 */
    union {
        struct {
            uint32_t palette_word_84                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_84;

    /* 0x614 : PALE_SRAM_85 */
    union {
        struct {
            uint32_t palette_word_85                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_85;

    /* 0x618 : PALE_SRAM_86 */
    union {
        struct {
            uint32_t palette_word_86                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_86;

    /* 0x61C : PALE_SRAM_87 */
    union {
        struct {
            uint32_t palette_word_87                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_87;

    /* 0x620 : PALE_SRAM_88 */
    union {
        struct {
            uint32_t palette_word_88                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_88;

    /* 0x624 : PALE_SRAM_89 */
    union {
        struct {
            uint32_t palette_word_89                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_89;

    /* 0x628 : PALE_SRAM_8A */
    union {
        struct {
            uint32_t palette_word_8A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8A;

    /* 0x62C : PALE_SRAM_8B */
    union {
        struct {
            uint32_t palette_word_8B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8B;

    /* 0x630 : PALE_SRAM_8C */
    union {
        struct {
            uint32_t palette_word_8C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8C;

    /* 0x634 : PALE_SRAM_8D */
    union {
        struct {
            uint32_t palette_word_8D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8D;

    /* 0x638 : PALE_SRAM_8E */
    union {
        struct {
            uint32_t palette_word_8E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8E;

    /* 0x63C : PALE_SRAM_8F */
    union {
        struct {
            uint32_t palette_word_8F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_8F;

    /* 0x640 : PALE_SRAM_90 */
    union {
        struct {
            uint32_t palette_word_90                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_90;

    /* 0x644 : PALE_SRAM_91 */
    union {
        struct {
            uint32_t palette_word_91                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_91;

    /* 0x648 : PALE_SRAM_92 */
    union {
        struct {
            uint32_t palette_word_92                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_92;

    /* 0x64C : PALE_SRAM_93 */
    union {
        struct {
            uint32_t palette_word_93                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_93;

    /* 0x650 : PALE_SRAM_94 */
    union {
        struct {
            uint32_t palette_word_94                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_94;

    /* 0x654 : PALE_SRAM_95 */
    union {
        struct {
            uint32_t palette_word_95                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_95;

    /* 0x658 : PALE_SRAM_96 */
    union {
        struct {
            uint32_t palette_word_96                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_96;

    /* 0x65C : PALE_SRAM_97 */
    union {
        struct {
            uint32_t palette_word_97                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_97;

    /* 0x660 : PALE_SRAM_98 */
    union {
        struct {
            uint32_t palette_word_98                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_98;

    /* 0x664 : PALE_SRAM_99 */
    union {
        struct {
            uint32_t palette_word_99                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_99;

    /* 0x668 : PALE_SRAM_9A */
    union {
        struct {
            uint32_t palette_word_9A                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9A;

    /* 0x66C : PALE_SRAM_9B */
    union {
        struct {
            uint32_t palette_word_9B                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9B;

    /* 0x670 : PALE_SRAM_9C */
    union {
        struct {
            uint32_t palette_word_9C                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9C;

    /* 0x674 : PALE_SRAM_9D */
    union {
        struct {
            uint32_t palette_word_9D                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9D;

    /* 0x678 : PALE_SRAM_9E */
    union {
        struct {
            uint32_t palette_word_9E                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9E;

    /* 0x67C : PALE_SRAM_9F */
    union {
        struct {
            uint32_t palette_word_9F                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_9F;

    /* 0x680 : PALE_SRAM_A0 */
    union {
        struct {
            uint32_t palette_word_A0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A0;

    /* 0x684 : PALE_SRAM_A1 */
    union {
        struct {
            uint32_t palette_word_A1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A1;

    /* 0x688 : PALE_SRAM_A2 */
    union {
        struct {
            uint32_t palette_word_A2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A2;

    /* 0x68C : PALE_SRAM_A3 */
    union {
        struct {
            uint32_t palette_word_A3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A3;

    /* 0x690 : PALE_SRAM_A4 */
    union {
        struct {
            uint32_t palette_word_A4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A4;

    /* 0x694 : PALE_SRAM_A5 */
    union {
        struct {
            uint32_t palette_word_A5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A5;

    /* 0x698 : PALE_SRAM_A6 */
    union {
        struct {
            uint32_t palette_word_A6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A6;

    /* 0x69C : PALE_SRAM_A7 */
    union {
        struct {
            uint32_t palette_word_A7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A7;

    /* 0x6A0 : PALE_SRAM_A8 */
    union {
        struct {
            uint32_t palette_word_A8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A8;

    /* 0x6A4 : PALE_SRAM_A9 */
    union {
        struct {
            uint32_t palette_word_A9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_A9;

    /* 0x6A8 : PALE_SRAM_AA */
    union {
        struct {
            uint32_t palette_word_AA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AA;

    /* 0x6AC : PALE_SRAM_AB */
    union {
        struct {
            uint32_t palette_word_AB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AB;

    /* 0x6B0 : PALE_SRAM_AC */
    union {
        struct {
            uint32_t palette_word_AC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AC;

    /* 0x6B4 : PALE_SRAM_AD */
    union {
        struct {
            uint32_t palette_word_AD                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AD;

    /* 0x6B8 : PALE_SRAM_AE */
    union {
        struct {
            uint32_t palette_word_AE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AE;

    /* 0x6BC : PALE_SRAM_AF */
    union {
        struct {
            uint32_t palette_word_AF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_AF;

    /* 0x6C0 : PALE_SRAM_B0 */
    union {
        struct {
            uint32_t palette_word_B0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B0;

    /* 0x6C4 : PALE_SRAM_B1 */
    union {
        struct {
            uint32_t palette_word_B1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B1;

    /* 0x6C8 : PALE_SRAM_B2 */
    union {
        struct {
            uint32_t palette_word_B2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B2;

    /* 0x6CC : PALE_SRAM_B3 */
    union {
        struct {
            uint32_t palette_word_B3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B3;

    /* 0x6D0 : PALE_SRAM_B4 */
    union {
        struct {
            uint32_t palette_word_B4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B4;

    /* 0x6D4 : PALE_SRAM_B5 */
    union {
        struct {
            uint32_t palette_word_B5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B5;

    /* 0x6D8 : PALE_SRAM_B6 */
    union {
        struct {
            uint32_t palette_word_B6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B6;

    /* 0x6DC : PALE_SRAM_B7 */
    union {
        struct {
            uint32_t palette_word_B7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B7;

    /* 0x6E0 : PALE_SRAM_B8 */
    union {
        struct {
            uint32_t palette_word_B8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B8;

    /* 0x6E4 : PALE_SRAM_B9 */
    union {
        struct {
            uint32_t palette_word_B9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_B9;

    /* 0x6E8 : PALE_SRAM_BA */
    union {
        struct {
            uint32_t palette_word_BA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BA;

    /* 0x6EC : PALE_SRAM_BB */
    union {
        struct {
            uint32_t palette_word_BB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BB;

    /* 0x6F0 : PALE_SRAM_BC */
    union {
        struct {
            uint32_t palette_word_BC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BC;

    /* 0x6F4 : PALE_SRAM_BD */
    union {
        struct {
            uint32_t palette_word_BD                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BD;

    /* 0x6F8 : PALE_SRAM_BE */
    union {
        struct {
            uint32_t palette_word_BE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BE;

    /* 0x6FC : PALE_SRAM_BF */
    union {
        struct {
            uint32_t palette_word_BF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_BF;

    /* 0x700 : PALE_SRAM_C0 */
    union {
        struct {
            uint32_t palette_word_C0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C0;

    /* 0x704 : PALE_SRAM_C1 */
    union {
        struct {
            uint32_t palette_word_C1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C1;

    /* 0x708 : PALE_SRAM_C2 */
    union {
        struct {
            uint32_t palette_word_C2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C2;

    /* 0x70C : PALE_SRAM_C3 */
    union {
        struct {
            uint32_t palette_word_C3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C3;

    /* 0x710 : PALE_SRAM_C4 */
    union {
        struct {
            uint32_t palette_word_C4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C4;

    /* 0x714 : PALE_SRAM_C5 */
    union {
        struct {
            uint32_t palette_word_C5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C5;

    /* 0x718 : PALE_SRAM_C6 */
    union {
        struct {
            uint32_t palette_word_C6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C6;

    /* 0x71C : PALE_SRAM_C7 */
    union {
        struct {
            uint32_t palette_word_C7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C7;

    /* 0x720 : PALE_SRAM_C8 */
    union {
        struct {
            uint32_t palette_word_C8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C8;

    /* 0x724 : PALE_SRAM_C9 */
    union {
        struct {
            uint32_t palette_word_C9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_C9;

    /* 0x728 : PALE_SRAM_CA */
    union {
        struct {
            uint32_t palette_word_CA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CA;

    /* 0x72C : PALE_SRAM_CB */
    union {
        struct {
            uint32_t palette_word_CB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CB;

    /* 0x730 : PALE_SRAM_CC */
    union {
        struct {
            uint32_t palette_word_CC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CC;

    /* 0x734 : PALE_SRAM_CD */
    union {
        struct {
            uint32_t palette_word_CD                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CD;

    /* 0x738 : PALE_SRAM_CE */
    union {
        struct {
            uint32_t palette_word_CE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CE;

    /* 0x73C : PALE_SRAM_CF */
    union {
        struct {
            uint32_t palette_word_CF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_CF;

    /* 0x740 : PALE_SRAM_D0 */
    union {
        struct {
            uint32_t palette_word_D0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D0;

    /* 0x744 : PALE_SRAM_D1 */
    union {
        struct {
            uint32_t palette_word_D1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D1;

    /* 0x748 : PALE_SRAM_D2 */
    union {
        struct {
            uint32_t palette_word_D2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D2;

    /* 0x74C : PALE_SRAM_D3 */
    union {
        struct {
            uint32_t palette_word_D3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D3;

    /* 0x750 : PALE_SRAM_D4 */
    union {
        struct {
            uint32_t palette_word_D4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D4;

    /* 0x754 : PALE_SRAM_D5 */
    union {
        struct {
            uint32_t palette_word_D5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D5;

    /* 0x758 : PALE_SRAM_D6 */
    union {
        struct {
            uint32_t palette_word_D6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D6;

    /* 0x75C : PALE_SRAM_D7 */
    union {
        struct {
            uint32_t palette_word_D7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D7;

    /* 0x760 : PALE_SRAM_D8 */
    union {
        struct {
            uint32_t palette_word_D8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D8;

    /* 0x764 : PALE_SRAM_D9 */
    union {
        struct {
            uint32_t palette_word_D9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_D9;

    /* 0x768 : PALE_SRAM_DA */
    union {
        struct {
            uint32_t palette_word_DA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DA;

    /* 0x76C : PALE_SRAM_DB */
    union {
        struct {
            uint32_t palette_word_DB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DB;

    /* 0x770 : PALE_SRAM_DC */
    union {
        struct {
            uint32_t palette_word_DC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DC;

    /* 0x774 : PALE_SRAM_DD */
    union {
        struct {
            uint32_t palette_word_DD                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DD;

    /* 0x778 : PALE_SRAM_DE */
    union {
        struct {
            uint32_t palette_word_DE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DE;

    /* 0x77C : PALE_SRAM_DF */
    union {
        struct {
            uint32_t palette_word_DF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_DF;

    /* 0x780 : PALE_SRAM_E0 */
    union {
        struct {
            uint32_t palette_word_E0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E0;

    /* 0x784 : PALE_SRAM_E1 */
    union {
        struct {
            uint32_t palette_word_E1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E1;

    /* 0x788 : PALE_SRAM_E2 */
    union {
        struct {
            uint32_t palette_word_E2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E2;

    /* 0x78C : PALE_SRAM_E3 */
    union {
        struct {
            uint32_t palette_word_E3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E3;

    /* 0x790 : PALE_SRAM_E4 */
    union {
        struct {
            uint32_t palette_word_E4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E4;

    /* 0x794 : PALE_SRAM_E5 */
    union {
        struct {
            uint32_t palette_word_E5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E5;

    /* 0x798 : PALE_SRAM_E6 */
    union {
        struct {
            uint32_t palette_word_E6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E6;

    /* 0x79C : PALE_SRAM_E7 */
    union {
        struct {
            uint32_t palette_word_E7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E7;

    /* 0x7A0 : PALE_SRAM_E8 */
    union {
        struct {
            uint32_t palette_word_E8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E8;

    /* 0x7A4 : PALE_SRAM_E9 */
    union {
        struct {
            uint32_t palette_word_E9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_E9;

    /* 0x7A8 : PALE_SRAM_EA */
    union {
        struct {
            uint32_t palette_word_EA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_EA;

    /* 0x7AC : PALE_SRAM_EB */
    union {
        struct {
            uint32_t palette_word_EB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_EB;

    /* 0x7B0 : PALE_SRAM_EC */
    union {
        struct {
            uint32_t palette_word_EC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_EC;

    /* 0x7B4 : PALE_SRAM_ED */
    union {
        struct {
            uint32_t palette_word_ED                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_ED;

    /* 0x7B8 : PALE_SRAM_EE */
    union {
        struct {
            uint32_t palette_word_EE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_EE;

    /* 0x7BC : PALE_SRAM_EF */
    union {
        struct {
            uint32_t palette_word_EF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_EF;

    /* 0x7C0 : PALE_SRAM_F0 */
    union {
        struct {
            uint32_t palette_word_F0                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F0;

    /* 0x7C4 : PALE_SRAM_F1 */
    union {
        struct {
            uint32_t palette_word_F1                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F1;

    /* 0x7C8 : PALE_SRAM_F2 */
    union {
        struct {
            uint32_t palette_word_F2                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F2;

    /* 0x7CC : PALE_SRAM_F3 */
    union {
        struct {
            uint32_t palette_word_F3                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F3;

    /* 0x7D0 : PALE_SRAM_F4 */
    union {
        struct {
            uint32_t palette_word_F4                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F4;

    /* 0x7D4 : PALE_SRAM_F5 */
    union {
        struct {
            uint32_t palette_word_F5                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F5;

    /* 0x7D8 : PALE_SRAM_F6 */
    union {
        struct {
            uint32_t palette_word_F6                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F6;

    /* 0x7DC : PALE_SRAM_F7 */
    union {
        struct {
            uint32_t palette_word_F7                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F7;

    /* 0x7E0 : PALE_SRAM_F8 */
    union {
        struct {
            uint32_t palette_word_F8                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F8;

    /* 0x7E4 : PALE_SRAM_F9 */
    union {
        struct {
            uint32_t palette_word_F9                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_F9;

    /* 0x7E8 : PALE_SRAM_FA */
    union {
        struct {
            uint32_t palette_word_FA                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FA;

    /* 0x7EC : PALE_SRAM_FB */
    union {
        struct {
            uint32_t palette_word_FB                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FB;

    /* 0x7F0 : PALE_SRAM_FC */
    union {
        struct {
            uint32_t palette_word_FC                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FC;

    /* 0x7F4 : PALE_SRAM_FD */
    union {
        struct {
            uint32_t palette_word_FD                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FD;

    /* 0x7F8 : PALE_SRAM_FE */
    union {
        struct {
            uint32_t palette_word_FE                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FE;

    /* 0x7FC : PALE_SRAM_FF */
    union {
        struct {
            uint32_t palette_word_FF                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } PALE_SRAM_FF;

};

typedef volatile struct osd_blend_reg osd_blend_reg_t;


#endif  /* __OSD_BLEND_REG_H__ */
