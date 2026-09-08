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

#ifndef  __IPROAI_REG_H__
#define  __IPROAI_REG_H__

#include "ipro7.h"

/* 0x00 : BALI_GENERAL_CFG */
#define IPROAI4_BALI_GENERAL_CFG_OFFSET                           (0x00)
#define IPROAI4_REG_IMGI_UNSGN                                    IPROAI4_REG_IMGI_UNSGN
#define IPROAI4_REG_IMGI_UNSGN_POS                                (0U)
#define IPROAI4_REG_IMGI_UNSGN_LEN                                (1U)
#define IPROAI4_REG_IMGI_UNSGN_MSK                                (((1U<<IPROAI4_REG_IMGI_UNSGN_LEN)-1)<<IPROAI4_REG_IMGI_UNSGN_POS)
#define IPROAI4_REG_IMGI_UNSGN_UMSK                               (~(((1U<<IPROAI4_REG_IMGI_UNSGN_LEN)-1)<<IPROAI4_REG_IMGI_UNSGN_POS))
#define IPROAI4_REG_IMG_IN_MODE                                   IPROAI4_REG_IMG_IN_MODE
#define IPROAI4_REG_IMG_IN_MODE_POS                               (8U)
#define IPROAI4_REG_IMG_IN_MODE_LEN                               (2U)
#define IPROAI4_REG_IMG_IN_MODE_MSK                               (((1U<<IPROAI4_REG_IMG_IN_MODE_LEN)-1)<<IPROAI4_REG_IMG_IN_MODE_POS)
#define IPROAI4_REG_IMG_IN_MODE_UMSK                              (~(((1U<<IPROAI4_REG_IMG_IN_MODE_LEN)-1)<<IPROAI4_REG_IMG_IN_MODE_POS))
#define IPROAI4_AXI_W_IDLE                                        IPROAI4_AXI_W_IDLE
#define IPROAI4_AXI_W_IDLE_POS                                    (30U)
#define IPROAI4_AXI_W_IDLE_LEN                                    (1U)
#define IPROAI4_AXI_W_IDLE_MSK                                    (((1U<<IPROAI4_AXI_W_IDLE_LEN)-1)<<IPROAI4_AXI_W_IDLE_POS)
#define IPROAI4_AXI_W_IDLE_UMSK                                   (~(((1U<<IPROAI4_AXI_W_IDLE_LEN)-1)<<IPROAI4_AXI_W_IDLE_POS))
#define IPROAI4_AXI_R_IDLE                                        IPROAI4_AXI_R_IDLE
#define IPROAI4_AXI_R_IDLE_POS                                    (31U)
#define IPROAI4_AXI_R_IDLE_LEN                                    (1U)
#define IPROAI4_AXI_R_IDLE_MSK                                    (((1U<<IPROAI4_AXI_R_IDLE_LEN)-1)<<IPROAI4_AXI_R_IDLE_POS)
#define IPROAI4_AXI_R_IDLE_UMSK                                   (~(((1U<<IPROAI4_AXI_R_IDLE_LEN)-1)<<IPROAI4_AXI_R_IDLE_POS))

/* 0x04 : IPROAI_INT_CFG */
#define IPROAI4_IPROAI_INT_CFG_OFFSET                               (0x04)
#define IPROAI4_REG_IPROAI_START                                    IPROAI4_REG_IPROAI_START
#define IPROAI4_REG_IPROAI_START_POS                                (0U)
#define IPROAI4_REG_IPROAI_START_LEN                                (1U)
#define IPROAI4_REG_IPROAI_START_MSK                                (((1U<<IPROAI4_REG_IPROAI_START_LEN)-1)<<IPROAI4_REG_IPROAI_START_POS)
#define IPROAI4_REG_IPROAI_START_UMSK                               (~(((1U<<IPROAI4_REG_IPROAI_START_LEN)-1)<<IPROAI4_REG_IPROAI_START_POS))
#define IPROAI4_REG_IPROAI_STOP                                     IPROAI4_REG_IPROAI_STOP
#define IPROAI4_REG_IPROAI_STOP_POS                                 (1U)
#define IPROAI4_REG_IPROAI_STOP_LEN                                 (1U)
#define IPROAI4_REG_IPROAI_STOP_MSK                                 (((1U<<IPROAI4_REG_IPROAI_STOP_LEN)-1)<<IPROAI4_REG_IPROAI_STOP_POS)
#define IPROAI4_REG_IPROAI_STOP_UMSK                                (~(((1U<<IPROAI4_REG_IPROAI_STOP_LEN)-1)<<IPROAI4_REG_IPROAI_STOP_POS))
#define IPROAI4_REG_IPROAI_RESUME                                   IPROAI4_REG_IPROAI_RESUME
#define IPROAI4_REG_IPROAI_RESUME_POS                               (2U)
#define IPROAI4_REG_IPROAI_RESUME_LEN                               (1U)
#define IPROAI4_REG_IPROAI_RESUME_MSK                               (((1U<<IPROAI4_REG_IPROAI_RESUME_LEN)-1)<<IPROAI4_REG_IPROAI_RESUME_POS)
#define IPROAI4_REG_IPROAI_RESUME_UMSK                              (~(((1U<<IPROAI4_REG_IPROAI_RESUME_LEN)-1)<<IPROAI4_REG_IPROAI_RESUME_POS))
#define IPROAI4_REG_INT_CLR                                       IPROAI4_REG_INT_CLR
#define IPROAI4_REG_INT_CLR_POS                                   (8U)
#define IPROAI4_REG_INT_CLR_LEN                                   (1U)
#define IPROAI4_REG_INT_CLR_MSK                                   (((1U<<IPROAI4_REG_INT_CLR_LEN)-1)<<IPROAI4_REG_INT_CLR_POS)
#define IPROAI4_REG_INT_CLR_UMSK                                  (~(((1U<<IPROAI4_REG_INT_CLR_LEN)-1)<<IPROAI4_REG_INT_CLR_POS))
#define IPROAI4_STS_IPROAI_INT                                      IPROAI4_STS_IPROAI_INT
#define IPROAI4_STS_IPROAI_INT_POS                                  (9U)
#define IPROAI4_STS_IPROAI_INT_LEN                                  (1U)
#define IPROAI4_STS_IPROAI_INT_MSK                                  (((1U<<IPROAI4_STS_IPROAI_INT_LEN)-1)<<IPROAI4_STS_IPROAI_INT_POS)
#define IPROAI4_STS_IPROAI_INT_UMSK                                 (~(((1U<<IPROAI4_STS_IPROAI_INT_LEN)-1)<<IPROAI4_STS_IPROAI_INT_POS))

/* 0x08 : IPROAI_Bus */
#define IPROAI4_IPROAI_BUS_OFFSET                                   (0x08)
#define IPROAI4_REG_RCMD_LEN                                      IPROAI4_REG_RCMD_LEN
#define IPROAI4_REG_RCMD_LEN_POS                                  (0U)
#define IPROAI4_REG_RCMD_LEN_LEN                                  (6U)
#define IPROAI4_REG_RCMD_LEN_MSK                                  (((1U<<IPROAI4_REG_RCMD_LEN_LEN)-1)<<IPROAI4_REG_RCMD_LEN_POS)
#define IPROAI4_REG_RCMD_LEN_UMSK                                 (~(((1U<<IPROAI4_REG_RCMD_LEN_LEN)-1)<<IPROAI4_REG_RCMD_LEN_POS))
#define IPROAI4_REG_R_OSTD_LMT                                    IPROAI4_REG_R_OSTD_LMT
#define IPROAI4_REG_R_OSTD_LMT_POS                                (8U)
#define IPROAI4_REG_R_OSTD_LMT_LEN                                (2U)
#define IPROAI4_REG_R_OSTD_LMT_MSK                                (((1U<<IPROAI4_REG_R_OSTD_LMT_LEN)-1)<<IPROAI4_REG_R_OSTD_LMT_POS)
#define IPROAI4_REG_R_OSTD_LMT_UMSK                               (~(((1U<<IPROAI4_REG_R_OSTD_LMT_LEN)-1)<<IPROAI4_REG_R_OSTD_LMT_POS))
#define IPROAI4_REG_WCMD_LEN                                      IPROAI4_REG_WCMD_LEN
#define IPROAI4_REG_WCMD_LEN_POS                                  (16U)
#define IPROAI4_REG_WCMD_LEN_LEN                                  (6U)
#define IPROAI4_REG_WCMD_LEN_MSK                                  (((1U<<IPROAI4_REG_WCMD_LEN_LEN)-1)<<IPROAI4_REG_WCMD_LEN_POS)
#define IPROAI4_REG_WCMD_LEN_UMSK                                 (~(((1U<<IPROAI4_REG_WCMD_LEN_LEN)-1)<<IPROAI4_REG_WCMD_LEN_POS))
#define IPROAI4_REG_W_OSTD_LMT                                    IPROAI4_REG_W_OSTD_LMT
#define IPROAI4_REG_W_OSTD_LMT_POS                                (24U)
#define IPROAI4_REG_W_OSTD_LMT_LEN                                (2U)
#define IPROAI4_REG_W_OSTD_LMT_MSK                                (((1U<<IPROAI4_REG_W_OSTD_LMT_LEN)-1)<<IPROAI4_REG_W_OSTD_LMT_POS)
#define IPROAI4_REG_W_OSTD_LMT_UMSK                               (~(((1U<<IPROAI4_REG_W_OSTD_LMT_LEN)-1)<<IPROAI4_REG_W_OSTD_LMT_POS))

/* 0xC : IPROAI_TFPM_ADDR */
#define IPROAI4_IPROAI_TFPM_ADDR_OFFSET                             (0xC)
#define IPROAI4_REG_TFPM_ADDR_BASE                                IPROAI4_REG_TFPM_ADDR_BASE
#define IPROAI4_REG_TFPM_ADDR_BASE_POS                            (0U)
#define IPROAI4_REG_TFPM_ADDR_BASE_LEN                            (32U)
#define IPROAI4_REG_TFPM_ADDR_BASE_MSK                            (((1U<<IPROAI4_REG_TFPM_ADDR_BASE_LEN)-1)<<IPROAI4_REG_TFPM_ADDR_BASE_POS)
#define IPROAI4_REG_TFPM_ADDR_BASE_UMSK                           (~(((1U<<IPROAI4_REG_TFPM_ADDR_BASE_LEN)-1)<<IPROAI4_REG_TFPM_ADDR_BASE_POS))

/* 0x10 : IPROAI_WEIGHT_ADDR */
#define IPROAI4_IPROAI_WEIGHT_ADDR_OFFSET                           (0x10)
#define IPROAI4_REG_WEIG_ADDR_BASE                                IPROAI4_REG_WEIG_ADDR_BASE
#define IPROAI4_REG_WEIG_ADDR_BASE_POS                            (0U)
#define IPROAI4_REG_WEIG_ADDR_BASE_LEN                            (32U)
#define IPROAI4_REG_WEIG_ADDR_BASE_MSK                            (((1U<<IPROAI4_REG_WEIG_ADDR_BASE_LEN)-1)<<IPROAI4_REG_WEIG_ADDR_BASE_POS)
#define IPROAI4_REG_WEIG_ADDR_BASE_UMSK                           (~(((1U<<IPROAI4_REG_WEIG_ADDR_BASE_LEN)-1)<<IPROAI4_REG_WEIG_ADDR_BASE_POS))

/* 0x14 : IPROAI_BIAS_ADDR */
#define IPROAI4_IPROAI_BIAS_ADDR_OFFSET                             (0x14)
#define IPROAI4_REG_BIAS_ADDR_BASE                                IPROAI4_REG_BIAS_ADDR_BASE
#define IPROAI4_REG_BIAS_ADDR_BASE_POS                            (0U)
#define IPROAI4_REG_BIAS_ADDR_BASE_LEN                            (32U)
#define IPROAI4_REG_BIAS_ADDR_BASE_MSK                            (((1U<<IPROAI4_REG_BIAS_ADDR_BASE_LEN)-1)<<IPROAI4_REG_BIAS_ADDR_BASE_POS)
#define IPROAI4_REG_BIAS_ADDR_BASE_UMSK                           (~(((1U<<IPROAI4_REG_BIAS_ADDR_BASE_LEN)-1)<<IPROAI4_REG_BIAS_ADDR_BASE_POS))

/* 0x18 : IPROAI_INST_ADDR */
#define IPROAI4_IPROAI_INST_ADDR_OFFSET                             (0x18)
#define IPROAI4_REG_INST_ADDR_BASE                                IPROAI4_REG_INST_ADDR_BASE
#define IPROAI4_REG_INST_ADDR_BASE_POS                            (0U)
#define IPROAI4_REG_INST_ADDR_BASE_LEN                            (32U)
#define IPROAI4_REG_INST_ADDR_BASE_MSK                            (((1U<<IPROAI4_REG_INST_ADDR_BASE_LEN)-1)<<IPROAI4_REG_INST_ADDR_BASE_POS)
#define IPROAI4_REG_INST_ADDR_BASE_UMSK                           (~(((1U<<IPROAI4_REG_INST_ADDR_BASE_LEN)-1)<<IPROAI4_REG_INST_ADDR_BASE_POS))

/* 0x1C : APU_DM0_ADDR */
#define IPROAI4_APU_DM0_ADDR_OFFSET                               (0x1C)
#define IPROAI4_REG_IMG_ADDR_BASE                                 IPROAI4_REG_IMG_ADDR_BASE
#define IPROAI4_REG_IMG_ADDR_BASE_POS                             (0U)
#define IPROAI4_REG_IMG_ADDR_BASE_LEN                             (32U)
#define IPROAI4_REG_IMG_ADDR_BASE_MSK                             (((1U<<IPROAI4_REG_IMG_ADDR_BASE_LEN)-1)<<IPROAI4_REG_IMG_ADDR_BASE_POS)
#define IPROAI4_REG_IMG_ADDR_BASE_UMSK                            (~(((1U<<IPROAI4_REG_IMG_ADDR_BASE_LEN)-1)<<IPROAI4_REG_IMG_ADDR_BASE_POS))

/* 0x20 : APU_DM1_ADDR */
#define IPROAI4_APU_DM1_ADDR_OFFSET                               (0x20)
#define IPROAI4_REG_IMG_MEM_SEG                                   IPROAI4_REG_IMG_MEM_SEG
#define IPROAI4_REG_IMG_MEM_SEG_POS                               (0U)
#define IPROAI4_REG_IMG_MEM_SEG_LEN                               (32U)
#define IPROAI4_REG_IMG_MEM_SEG_MSK                               (((1U<<IPROAI4_REG_IMG_MEM_SEG_LEN)-1)<<IPROAI4_REG_IMG_MEM_SEG_POS)
#define IPROAI4_REG_IMG_MEM_SEG_UMSK                              (~(((1U<<IPROAI4_REG_IMG_MEM_SEG_LEN)-1)<<IPROAI4_REG_IMG_MEM_SEG_POS))

/* 0x24 : IPROAI_TF_CFG0 */
#define IPROAI4_IPROAI_TF_CFG0_OFFSET                               (0x24)
#define IPROAI4_REG_TF_PARAM_FROM_MEM                             IPROAI4_REG_TF_PARAM_FROM_MEM
#define IPROAI4_REG_TF_PARAM_FROM_MEM_POS                         (30U)
#define IPROAI4_REG_TF_PARAM_FROM_MEM_LEN                         (1U)
#define IPROAI4_REG_TF_PARAM_FROM_MEM_MSK                         (((1U<<IPROAI4_REG_TF_PARAM_FROM_MEM_LEN)-1)<<IPROAI4_REG_TF_PARAM_FROM_MEM_POS)
#define IPROAI4_REG_TF_PARAM_FROM_MEM_UMSK                        (~(((1U<<IPROAI4_REG_TF_PARAM_FROM_MEM_LEN)-1)<<IPROAI4_REG_TF_PARAM_FROM_MEM_POS))

/* 0x28 : APU_DM2_ADDR */
#define IPROAI4_APU_DM2_ADDR_OFFSET                               (0x28)
#define IPROAI4_REG_IMG_EXT_ADDR                                   IPROAI4_REG_IMG_EXT_ADDR
#define IPROAI4_REG_IMG_EXT_ADDR_POS                               (0U)
#define IPROAI4_REG_IMG_EXT_ADDR_LEN                               (32U)
#define IPROAI4_REG_IMG_EXT_ADDR_MSK                               (((1U<<IPROAI4_REG_IMG_EXT_ADDR_LEN)-1)<<IPROAI4_REG_IMG_EXT_ADDR_POS)
#define IPROAI4_REG_IMG_EXT_ADDR_UMSK                              (~(((1U<<IPROAI4_REG_IMG_EXT_ADDR_LEN)-1)<<IPROAI4_REG_IMG_EXT_ADDR_POS))

/* 0x30 : IPROAI_DVP */
#define IPROAI4_IPROAI_DVP_OFFSET                                   (0x30)
#define IPROAI4_REG_POST_HBLANK_CNT                               IPROAI4_REG_POST_HBLANK_CNT
#define IPROAI4_REG_POST_HBLANK_CNT_POS                           (0U)
#define IPROAI4_REG_POST_HBLANK_CNT_LEN                           (12U)
#define IPROAI4_REG_POST_HBLANK_CNT_MSK                           (((1U<<IPROAI4_REG_POST_HBLANK_CNT_LEN)-1)<<IPROAI4_REG_POST_HBLANK_CNT_POS)
#define IPROAI4_REG_POST_HBLANK_CNT_UMSK                          (~(((1U<<IPROAI4_REG_POST_HBLANK_CNT_LEN)-1)<<IPROAI4_REG_POST_HBLANK_CNT_POS))
#define IPROAI4_REG_DVP_MODE                                      IPROAI4_REG_DVP_MODE
#define IPROAI4_REG_DVP_MODE_POS                                  (15U)
#define IPROAI4_REG_DVP_MODE_LEN                                  (1U)
#define IPROAI4_REG_DVP_MODE_MSK                                  (((1U<<IPROAI4_REG_DVP_MODE_LEN)-1)<<IPROAI4_REG_DVP_MODE_POS)
#define IPROAI4_REG_DVP_MODE_UMSK                                 (~(((1U<<IPROAI4_REG_DVP_MODE_LEN)-1)<<IPROAI4_REG_DVP_MODE_POS))
#define IPROAI4_REG_PRE_HBLANK_CNT                                IPROAI4_REG_PRE_HBLANK_CNT
#define IPROAI4_REG_PRE_HBLANK_CNT_POS                            (16U)
#define IPROAI4_REG_PRE_HBLANK_CNT_LEN                            (4U)
#define IPROAI4_REG_PRE_HBLANK_CNT_MSK                            (((1U<<IPROAI4_REG_PRE_HBLANK_CNT_LEN)-1)<<IPROAI4_REG_PRE_HBLANK_CNT_POS)
#define IPROAI4_REG_PRE_HBLANK_CNT_UMSK                           (~(((1U<<IPROAI4_REG_PRE_HBLANK_CNT_LEN)-1)<<IPROAI4_REG_PRE_HBLANK_CNT_POS))

/* 0x40 : IPROAI_PMC_CFG */
#define IPROAI4_IPROAI_PMC_CFG_OFFSET                               (0x40)
#define IPROAI4_REG_PMC_CLR                                       IPROAI4_REG_PMC_CLR
#define IPROAI4_REG_PMC_CLR_POS                                   (0U)
#define IPROAI4_REG_PMC_CLR_LEN                                   (1U)
#define IPROAI4_REG_PMC_CLR_MSK                                   (((1U<<IPROAI4_REG_PMC_CLR_LEN)-1)<<IPROAI4_REG_PMC_CLR_POS)
#define IPROAI4_REG_PMC_CLR_UMSK                                  (~(((1U<<IPROAI4_REG_PMC_CLR_LEN)-1)<<IPROAI4_REG_PMC_CLR_POS))

/* 0x44 : IPROAI_PMC_STS0 */
#define IPROAI4_IPROAI_PMC_STS0_OFFSET                              (0x44)
#define IPROAI4_STS_WORK_CNT                                      IPROAI4_STS_WORK_CNT
#define IPROAI4_STS_WORK_CNT_POS                                  (0U)
#define IPROAI4_STS_WORK_CNT_LEN                                  (32U)
#define IPROAI4_STS_WORK_CNT_MSK                                  (((1U<<IPROAI4_STS_WORK_CNT_LEN)-1)<<IPROAI4_STS_WORK_CNT_POS)
#define IPROAI4_STS_WORK_CNT_UMSK                                 (~(((1U<<IPROAI4_STS_WORK_CNT_LEN)-1)<<IPROAI4_STS_WORK_CNT_POS))

/* 0x48 : IPROAI_PMC_STS1 */
#define IPROAI4_IPROAI_PMC_STS1_OFFSET                              (0x48)
#define IPROAI4_STS_IDLE_CNT                                      IPROAI4_STS_IDLE_CNT
#define IPROAI4_STS_IDLE_CNT_POS                                  (0U)
#define IPROAI4_STS_IDLE_CNT_LEN                                  (32U)
#define IPROAI4_STS_IDLE_CNT_MSK                                  (((1U<<IPROAI4_STS_IDLE_CNT_LEN)-1)<<IPROAI4_STS_IDLE_CNT_POS)
#define IPROAI4_STS_IDLE_CNT_UMSK                                 (~(((1U<<IPROAI4_STS_IDLE_CNT_LEN)-1)<<IPROAI4_STS_IDLE_CNT_POS))


struct  iproai4_reg {
    /* 0x00 : BALI_GENERAL_CFG */
    union {
        struct {
            uint32_t reg_imgi_unsgn                 :  1; /* [    0],        r/w,        0x1 */
            uint32_t reserved_1_7                   :  7; /* [ 7: 1],       rsvd,        0x0 */
            uint32_t reg_img_in_mode                :  2; /* [ 9: 8],        r/w,        0x0 */
            uint32_t reserved_10_29                 : 20; /* [29:10],       rsvd,        0x0 */
            uint32_t axi_w_idle                     :  1; /* [   30],          r,        0x0 */
            uint32_t axi_r_idle                     :  1; /* [   31],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } BALI_GENERAL_CFG;

    /* 0x04 : IPROAI_INT_CFG */
    union {
        struct {
            uint32_t reg_iproai_start                 :  1; /* [    0],        w1p,        0x0 */
            uint32_t reg_iproai_stop                  :  1; /* [    1],        w1p,        0x0 */
            uint32_t reg_iproai_resume                :  1; /* [    2],        w1p,        0x0 */
            uint32_t reserved_3_7                   :  5; /* [ 7: 3],       rsvd,        0x0 */
            uint32_t reg_int_clr                    :  1; /* [    8],        w1p,        0x0 */
            uint32_t sts_iproai_int                   :  1; /* [    9],          r,        0x0 */
            uint32_t reserved_10_31                 : 22; /* [31:10],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_INT_CFG;

    /* 0x08 : IPROAI_Bus */
    union {
        struct {
            uint32_t reg_rcmd_len                   :  6; /* [ 5: 0],        r/w,       0x1f */
            uint32_t reserved_6_7                   :  2; /* [ 7: 6],       rsvd,        0x0 */
            uint32_t reg_r_ostd_lmt                 :  2; /* [ 9: 8],        r/w,        0x1 */
            uint32_t reserved_10_15                 :  6; /* [15:10],       rsvd,        0x0 */
            uint32_t reg_wcmd_len                   :  6; /* [21:16],        r/w,       0x1f */
            uint32_t reserved_22_23                 :  2; /* [23:22],       rsvd,        0x0 */
            uint32_t reg_w_ostd_lmt                 :  2; /* [25:24],        r/w,        0x1 */
            uint32_t reserved_26_31                 :  6; /* [31:26],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_Bus;

    /* 0xC : IPROAI_TFPM_ADDR */
    union {
        struct {
            uint32_t reg_tfpm_addr_base             : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_TFPM_ADDR;

    /* 0x10 : IPROAI_WEIGHT_ADDR */
    union {
        struct {
            uint32_t reg_weig_addr_base             : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_WEIGHT_ADDR;

    /* 0x14 : IPROAI_BIAS_ADDR */
    union {
        struct {
            uint32_t reg_bias_addr_base             : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_BIAS_ADDR;

    /* 0x18 : IPROAI_INST_ADDR */
    union {
        struct {
            uint32_t reg_inst_addr_base             : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_INST_ADDR;

    /* 0x1C : APU_DM0_ADDR */
    union {
        struct {
            uint32_t reg_img_addr_base              : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } APU_DM0_ADDR;

    /* 0x20 : APU_DM1_ADDR */
    union {
        struct {
            uint32_t reg_img_mem_seg                : 32; /* [31: 0],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } APU_DM1_ADDR;

    /* 0x24 : IPROAI_TF_CFG0 */
    union {
        struct {
            uint32_t reserved_0_29                  : 30; /* [29: 0],       rsvd,        0x0 */
            uint32_t reg_tf_param_from_mem          :  1; /* [   30],        r/w,        0x0 */
            uint32_t reserved_31                    :  1; /* [   31],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_TF_CFG0;

    /* 0x28  reserved */
    uint8_t RESERVED0x28[8];

    /* 0x30 : IPROAI_DVP */
    union {
        struct {
            uint32_t reg_post_hblank_cnt            : 12; /* [11: 0],        r/w,       0x40 */
            uint32_t reserved_12_14                 :  3; /* [14:12],       rsvd,        0x0 */
            uint32_t reg_dvp_mode                   :  1; /* [   15],        r/w,        0x1 */
            uint32_t reg_pre_hblank_cnt             :  4; /* [19:16],        r/w,        0x8 */
            uint32_t reserved_20_31                 : 12; /* [31:20],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_DVP;

    /* 0x34  reserved */
    uint8_t RESERVED0x34[12];

    /* 0x40 : IPROAI_PMC_CFG */
    union {
        struct {
            uint32_t reg_pmc_clr                    :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_PMC_CFG;

    /* 0x44 : IPROAI_PMC_STS0 */
    union {
        struct {
            uint32_t sts_work_cnt                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_PMC_STS0;

    /* 0x48 : IPROAI_PMC_STS1 */
    union {
        struct {
            uint32_t sts_idle_cnt                   : 32; /* [31: 0],          r,        0x0 */
        }BF;
        uint32_t WORD;
    } IPROAI_PMC_STS1;

};

typedef volatile struct iproai4_reg iproai4_reg_t;


#endif  /* __IPROAI_REG_H__ */
