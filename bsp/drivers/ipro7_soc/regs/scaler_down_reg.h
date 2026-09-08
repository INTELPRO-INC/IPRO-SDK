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

#ifndef  __SCALER_DOWN_REG_H__
#define  __SCALER_DOWN_REG_H__

#include "ipro7.h"

/* 0x0 : sd_ctrl_000 */
#define SCALER_DOWN_SD_CTRL_000_OFFSET                          (0x0)
#define SCALER_DOWN_CR_WIDTH_I                                  SCALER_DOWN_CR_WIDTH_I
#define SCALER_DOWN_CR_WIDTH_I_POS                              (0U)
#define SCALER_DOWN_CR_WIDTH_I_LEN                              (14U)
#define SCALER_DOWN_CR_WIDTH_I_MSK                              (((1U<<SCALER_DOWN_CR_WIDTH_I_LEN)-1)<<SCALER_DOWN_CR_WIDTH_I_POS)
#define SCALER_DOWN_CR_WIDTH_I_UMSK                             (~(((1U<<SCALER_DOWN_CR_WIDTH_I_LEN)-1)<<SCALER_DOWN_CR_WIDTH_I_POS))
#define SCALER_DOWN_CR_HEIGHT_I                                 SCALER_DOWN_CR_HEIGHT_I
#define SCALER_DOWN_CR_HEIGHT_I_POS                             (16U)
#define SCALER_DOWN_CR_HEIGHT_I_LEN                             (14U)
#define SCALER_DOWN_CR_HEIGHT_I_MSK                             (((1U<<SCALER_DOWN_CR_HEIGHT_I_LEN)-1)<<SCALER_DOWN_CR_HEIGHT_I_POS)
#define SCALER_DOWN_CR_HEIGHT_I_UMSK                            (~(((1U<<SCALER_DOWN_CR_HEIGHT_I_LEN)-1)<<SCALER_DOWN_CR_HEIGHT_I_POS))
#define SCALER_DOWN_CR_DVP_O_RST_EN                             SCALER_DOWN_CR_DVP_O_RST_EN
#define SCALER_DOWN_CR_DVP_O_RST_EN_POS                         (30U)
#define SCALER_DOWN_CR_DVP_O_RST_EN_LEN                         (1U)
#define SCALER_DOWN_CR_DVP_O_RST_EN_MSK                         (((1U<<SCALER_DOWN_CR_DVP_O_RST_EN_LEN)-1)<<SCALER_DOWN_CR_DVP_O_RST_EN_POS)
#define SCALER_DOWN_CR_DVP_O_RST_EN_UMSK                        (~(((1U<<SCALER_DOWN_CR_DVP_O_RST_EN_LEN)-1)<<SCALER_DOWN_CR_DVP_O_RST_EN_POS))
#define SCALER_DOWN_CR_MODULE_EN                                SCALER_DOWN_CR_MODULE_EN
#define SCALER_DOWN_CR_MODULE_EN_POS                            (31U)
#define SCALER_DOWN_CR_MODULE_EN_LEN                            (1U)
#define SCALER_DOWN_CR_MODULE_EN_MSK                            (((1U<<SCALER_DOWN_CR_MODULE_EN_LEN)-1)<<SCALER_DOWN_CR_MODULE_EN_POS)
#define SCALER_DOWN_CR_MODULE_EN_UMSK                           (~(((1U<<SCALER_DOWN_CR_MODULE_EN_LEN)-1)<<SCALER_DOWN_CR_MODULE_EN_POS))

/* 0x4 : sd_ctrl_004 */
#define SCALER_DOWN_SD_CTRL_004_OFFSET                          (0x4)
#define SCALER_DOWN_CR_WIDTH_O                                  SCALER_DOWN_CR_WIDTH_O
#define SCALER_DOWN_CR_WIDTH_O_POS                              (0U)
#define SCALER_DOWN_CR_WIDTH_O_LEN                              (14U)
#define SCALER_DOWN_CR_WIDTH_O_MSK                              (((1U<<SCALER_DOWN_CR_WIDTH_O_LEN)-1)<<SCALER_DOWN_CR_WIDTH_O_POS)
#define SCALER_DOWN_CR_WIDTH_O_UMSK                             (~(((1U<<SCALER_DOWN_CR_WIDTH_O_LEN)-1)<<SCALER_DOWN_CR_WIDTH_O_POS))
#define SCALER_DOWN_CR_HEIGHT_O                                 SCALER_DOWN_CR_HEIGHT_O
#define SCALER_DOWN_CR_HEIGHT_O_POS                             (16U)
#define SCALER_DOWN_CR_HEIGHT_O_LEN                             (14U)
#define SCALER_DOWN_CR_HEIGHT_O_MSK                             (((1U<<SCALER_DOWN_CR_HEIGHT_O_LEN)-1)<<SCALER_DOWN_CR_HEIGHT_O_POS)
#define SCALER_DOWN_CR_HEIGHT_O_UMSK                            (~(((1U<<SCALER_DOWN_CR_HEIGHT_O_LEN)-1)<<SCALER_DOWN_CR_HEIGHT_O_POS))

/* 0x8 : sd_ctrl_008 */
#define SCALER_DOWN_SD_CTRL_008_OFFSET                          (0x8)
#define SCALER_DOWN_CR_WBLANK_O                                 SCALER_DOWN_CR_WBLANK_O
#define SCALER_DOWN_CR_WBLANK_O_POS                             (0U)
#define SCALER_DOWN_CR_WBLANK_O_LEN                             (12U)
#define SCALER_DOWN_CR_WBLANK_O_MSK                             (((1U<<SCALER_DOWN_CR_WBLANK_O_LEN)-1)<<SCALER_DOWN_CR_WBLANK_O_POS)
#define SCALER_DOWN_CR_WBLANK_O_UMSK                            (~(((1U<<SCALER_DOWN_CR_WBLANK_O_LEN)-1)<<SCALER_DOWN_CR_WBLANK_O_POS))
#define SCALER_DOWN_CR_HBLANK_O                                 SCALER_DOWN_CR_HBLANK_O
#define SCALER_DOWN_CR_HBLANK_O_POS                             (16U)
#define SCALER_DOWN_CR_HBLANK_O_LEN                             (12U)
#define SCALER_DOWN_CR_HBLANK_O_MSK                             (((1U<<SCALER_DOWN_CR_HBLANK_O_LEN)-1)<<SCALER_DOWN_CR_HBLANK_O_POS)
#define SCALER_DOWN_CR_HBLANK_O_UMSK                            (~(((1U<<SCALER_DOWN_CR_HBLANK_O_LEN)-1)<<SCALER_DOWN_CR_HBLANK_O_POS))

/* 0xC : sd_ctrl_00c */
#define SCALER_DOWN_SD_CTRL_00C_OFFSET                          (0xC)
#define SCALER_DOWN_CR_Y_INI                                    SCALER_DOWN_CR_Y_INI
#define SCALER_DOWN_CR_Y_INI_POS                                (0U)
#define SCALER_DOWN_CR_Y_INI_LEN                                (28U)
#define SCALER_DOWN_CR_Y_INI_MSK                                (((1U<<SCALER_DOWN_CR_Y_INI_LEN)-1)<<SCALER_DOWN_CR_Y_INI_POS)
#define SCALER_DOWN_CR_Y_INI_UMSK                               (~(((1U<<SCALER_DOWN_CR_Y_INI_LEN)-1)<<SCALER_DOWN_CR_Y_INI_POS))

/* 0x10 : sd_ctrl_010 */
#define SCALER_DOWN_SD_CTRL_010_OFFSET                          (0x10)
#define SCALER_DOWN_CR_X_INI                                    SCALER_DOWN_CR_X_INI
#define SCALER_DOWN_CR_X_INI_POS                                (0U)
#define SCALER_DOWN_CR_X_INI_LEN                                (28U)
#define SCALER_DOWN_CR_X_INI_MSK                                (((1U<<SCALER_DOWN_CR_X_INI_LEN)-1)<<SCALER_DOWN_CR_X_INI_POS)
#define SCALER_DOWN_CR_X_INI_UMSK                               (~(((1U<<SCALER_DOWN_CR_X_INI_LEN)-1)<<SCALER_DOWN_CR_X_INI_POS))

/* 0x14 : sd_ctrl_014 */
#define SCALER_DOWN_SD_CTRL_014_OFFSET                          (0x14)
#define SCALER_DOWN_CR_Y_STEP                                   SCALER_DOWN_CR_Y_STEP
#define SCALER_DOWN_CR_Y_STEP_POS                               (0U)
#define SCALER_DOWN_CR_Y_STEP_LEN                               (22U)
#define SCALER_DOWN_CR_Y_STEP_MSK                               (((1U<<SCALER_DOWN_CR_Y_STEP_LEN)-1)<<SCALER_DOWN_CR_Y_STEP_POS)
#define SCALER_DOWN_CR_Y_STEP_UMSK                              (~(((1U<<SCALER_DOWN_CR_Y_STEP_LEN)-1)<<SCALER_DOWN_CR_Y_STEP_POS))

/* 0x18 : sd_ctrl_018 */
#define SCALER_DOWN_SD_CTRL_018_OFFSET                          (0x18)
#define SCALER_DOWN_CR_X_STEP                                   SCALER_DOWN_CR_X_STEP
#define SCALER_DOWN_CR_X_STEP_POS                               (0U)
#define SCALER_DOWN_CR_X_STEP_LEN                               (22U)
#define SCALER_DOWN_CR_X_STEP_MSK                               (((1U<<SCALER_DOWN_CR_X_STEP_LEN)-1)<<SCALER_DOWN_CR_X_STEP_POS)
#define SCALER_DOWN_CR_X_STEP_UMSK                              (~(((1U<<SCALER_DOWN_CR_X_STEP_LEN)-1)<<SCALER_DOWN_CR_X_STEP_POS))

/* 0x1C : sd_ctrl_01C */
#define SCALER_DOWN_SD_CTRL_01C_OFFSET                          (0x1C)
#define SCALER_DOWN_CR_Y_COEFF_RATIO                            SCALER_DOWN_CR_Y_COEFF_RATIO
#define SCALER_DOWN_CR_Y_COEFF_RATIO_POS                        (0U)
#define SCALER_DOWN_CR_Y_COEFF_RATIO_LEN                        (4U)
#define SCALER_DOWN_CR_Y_COEFF_RATIO_MSK                        (((1U<<SCALER_DOWN_CR_Y_COEFF_RATIO_LEN)-1)<<SCALER_DOWN_CR_Y_COEFF_RATIO_POS)
#define SCALER_DOWN_CR_Y_COEFF_RATIO_UMSK                       (~(((1U<<SCALER_DOWN_CR_Y_COEFF_RATIO_LEN)-1)<<SCALER_DOWN_CR_Y_COEFF_RATIO_POS))
#define SCALER_DOWN_CR_X_COEFF_RATIO                            SCALER_DOWN_CR_X_COEFF_RATIO
#define SCALER_DOWN_CR_X_COEFF_RATIO_POS                        (4U)
#define SCALER_DOWN_CR_X_COEFF_RATIO_LEN                        (4U)
#define SCALER_DOWN_CR_X_COEFF_RATIO_MSK                        (((1U<<SCALER_DOWN_CR_X_COEFF_RATIO_LEN)-1)<<SCALER_DOWN_CR_X_COEFF_RATIO_POS)
#define SCALER_DOWN_CR_X_COEFF_RATIO_UMSK                       (~(((1U<<SCALER_DOWN_CR_X_COEFF_RATIO_LEN)-1)<<SCALER_DOWN_CR_X_COEFF_RATIO_POS))
#define SCALER_DOWN_CR_Y_COEFF_SET                              SCALER_DOWN_CR_Y_COEFF_SET
#define SCALER_DOWN_CR_Y_COEFF_SET_POS                          (8U)
#define SCALER_DOWN_CR_Y_COEFF_SET_LEN                          (1U)
#define SCALER_DOWN_CR_Y_COEFF_SET_MSK                          (((1U<<SCALER_DOWN_CR_Y_COEFF_SET_LEN)-1)<<SCALER_DOWN_CR_Y_COEFF_SET_POS)
#define SCALER_DOWN_CR_Y_COEFF_SET_UMSK                         (~(((1U<<SCALER_DOWN_CR_Y_COEFF_SET_LEN)-1)<<SCALER_DOWN_CR_Y_COEFF_SET_POS))
#define SCALER_DOWN_CR_X_COEFF_SET                              SCALER_DOWN_CR_X_COEFF_SET
#define SCALER_DOWN_CR_X_COEFF_SET_POS                          (10U)
#define SCALER_DOWN_CR_X_COEFF_SET_LEN                          (2U)
#define SCALER_DOWN_CR_X_COEFF_SET_MSK                          (((1U<<SCALER_DOWN_CR_X_COEFF_SET_LEN)-1)<<SCALER_DOWN_CR_X_COEFF_SET_POS)
#define SCALER_DOWN_CR_X_COEFF_SET_UMSK                         (~(((1U<<SCALER_DOWN_CR_X_COEFF_SET_LEN)-1)<<SCALER_DOWN_CR_X_COEFF_SET_POS))

/* 0x20 : sd_ctrl_020 */
#define SCALER_DOWN_SD_CTRL_020_OFFSET                          (0x20)
#define SCALER_DOWN_CR_SW_SHD                                   SCALER_DOWN_CR_SW_SHD
#define SCALER_DOWN_CR_SW_SHD_POS                               (0U)
#define SCALER_DOWN_CR_SW_SHD_LEN                               (1U)
#define SCALER_DOWN_CR_SW_SHD_MSK                               (((1U<<SCALER_DOWN_CR_SW_SHD_LEN)-1)<<SCALER_DOWN_CR_SW_SHD_POS)
#define SCALER_DOWN_CR_SW_SHD_UMSK                              (~(((1U<<SCALER_DOWN_CR_SW_SHD_LEN)-1)<<SCALER_DOWN_CR_SW_SHD_POS))


struct  scaler_down_reg {
    /* 0x0 : sd_ctrl_000 */
    union {
        struct {
            uint32_t cr_width_i                     : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_height_i                    : 14; /* [29:16],        r/w,        0x0 */
            uint32_t cr_dvp_o_rst_en                :  1; /* [   30],        r/w,        0x1 */
            uint32_t cr_module_en                   :  1; /* [   31],        r/w,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_000;

    /* 0x4 : sd_ctrl_004 */
    union {
        struct {
            uint32_t cr_width_o                     : 14; /* [13: 0],        r/w,        0x0 */
            uint32_t reserved_14_15                 :  2; /* [15:14],       rsvd,        0x0 */
            uint32_t cr_height_o                    : 14; /* [29:16],        r/w,        0x0 */
            uint32_t reserved_30_31                 :  2; /* [31:30],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_004;

    /* 0x8 : sd_ctrl_008 */
    union {
        struct {
            uint32_t cr_wblank_o                    : 12; /* [11: 0],        r/w,        0x0 */
            uint32_t reserved_12_15                 :  4; /* [15:12],       rsvd,        0x0 */
            uint32_t cr_hblank_o                    : 12; /* [27:16],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_008;

    /* 0xC : sd_ctrl_00c */
    union {
        struct {
            uint32_t cr_y_ini                       : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_00c;

    /* 0x10 : sd_ctrl_010 */
    union {
        struct {
            uint32_t cr_x_ini                       : 28; /* [27: 0],        r/w,        0x0 */
            uint32_t reserved_28_31                 :  4; /* [31:28],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_010;

    /* 0x14 : sd_ctrl_014 */
    union {
        struct {
            uint32_t cr_y_step                      : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_014;

    /* 0x18 : sd_ctrl_018 */
    union {
        struct {
            uint32_t cr_x_step                      : 22; /* [21: 0],        r/w,        0x0 */
            uint32_t reserved_22_31                 : 10; /* [31:22],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_018;

    /* 0x1C : sd_ctrl_01C */
    union {
        struct {
            uint32_t cr_y_coeff_ratio               :  4; /* [ 3: 0],        r/w,        0x0 */
            uint32_t cr_x_coeff_ratio               :  4; /* [ 7: 4],        r/w,        0x0 */
            uint32_t cr_y_coeff_set                 :  1; /* [    8],        r/w,        0x0 */
            uint32_t reserved_9                     :  1; /* [    9],       rsvd,        0x0 */
            uint32_t cr_x_coeff_set                 :  2; /* [11:10],        r/w,        0x0 */
            uint32_t reserved_12_31                 : 20; /* [31:12],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_01C;

    /* 0x20 : sd_ctrl_020 */
    union {
        struct {
            uint32_t cr_sw_shd                      :  1; /* [    0],        w1p,        0x0 */
            uint32_t reserved_1_31                  : 31; /* [31: 1],       rsvd,        0x0 */
        }BF;
        uint32_t WORD;
    } sd_ctrl_020;

};

typedef volatile struct scaler_down_reg scaler_down_reg_t;


#endif  /* __SCALER_DOWN_REG_H__ */
