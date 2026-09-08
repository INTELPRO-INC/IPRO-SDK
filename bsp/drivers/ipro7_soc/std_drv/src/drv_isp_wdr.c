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

#include <drv_isp_wdr.h>
#include <wdr_csr_reg.h>
#include <wdr_hist_sts_reg.h>

#define ISP_WDR_ROI_WEIGHT_MAX                      (15)
#define ISP_WDR_CURVE_WEIGHT_MAX                    (16)
#define ISP_WDR_POST_ENHANCE_WEIGHT_MAX             (16)
#define ISP_WDR_POST_ENHANCE_Y_OFFSET_MAX           ((1 << 12) - 1)
#define ISP_WDR_POST_ENHANCE_Y_GAIN_MAX             ((1 << 15) - 1)
#define ISP_WDR_POST_ENHANCE_C_GAIN_MAX             ((1 << 15) - 1)


void ISP_WDR_Get_WDR_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);

    if (reg_is_bit_set(tmpVal, WDR_CSR_CR_WDR_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_WDR_Set_WDR_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, WDR_CSR_CR_WDR_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, WDR_CSR_CR_WDR_EN);
    }
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_0_OFFSET);
}

void ISP_WDR_Set_CS_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, WDR_CSR_CR_CS_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, WDR_CSR_CR_CS_EN);
    }
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);
}

void ISP_WDR_Get_CS_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);

    if (reg_is_bit_set(tmpVal, WDR_CSR_CR_CS_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_WDR_Set_CS_Config(const ISP_Chroma_Suppress_Type *chromaSupprCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_CS_GAIN, chromaSupprCfg->gain);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_0_OFFSET);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_CS_WEIGHT, chromaSupprCfg->weight);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_CS_GREY_TH, chromaSupprCfg->grey_thr);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);
}

void ISP_WDR_Get_CS_Config(ISP_Chroma_Suppress_Type *chromaSupprCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);
    chromaSupprCfg->gain = reg_get_bits(tmpVal, WDR_CSR_CR_CS_GAIN);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CS_0_OFFSET);
    chromaSupprCfg->weight = reg_get_bits(tmpVal, WDR_CSR_CR_CS_WEIGHT);
    chromaSupprCfg->grey_thr = reg_get_bits(tmpVal, WDR_CSR_CR_CS_GREY_TH);
}

void ISP_WDR_Set_ROI_State(int roi_id, uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_0_OFFSET);
    if (ENABLE == state) {
        tmpVal |= (1 << roi_id);
    } else {
        tmpVal &= ~(1 << roi_id);
    }

    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_0_OFFSET);
}

void ISP_WDR_Get_ROI_State(int roi_id, uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_0_OFFSET);
    if (tmpVal & (1 << roi_id))
        *state = ENABLE;
    else
        *state = DISABLE;
}

void ISP_WDR_Set_ROI_Position(int roi_id, int left_x, int right_x,
                                int top_y, int bottom_y)
{
    uint32_t tmpVal;

    switch(roi_id)
    {
        case 0:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_2_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X0_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X0_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_2_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_3_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y0_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y0_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_3_OFFSET);
            break;
        case 1:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_4_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X1_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X1_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_4_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_5_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y1_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y1_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_5_OFFSET);
            break;
        case 2:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_6_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X2_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X2_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_6_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_7_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y2_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y2_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_7_OFFSET);
            break;
        case 3:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_8_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X3_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X3_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_8_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_9_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y3_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y3_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_9_OFFSET);
            break;
        case 4:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_10_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X4_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X4_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_10_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_11_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y4_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y4_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_11_OFFSET);
            break;
        case 5:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_12_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X5_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X5_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_12_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_13_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y5_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y5_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_13_OFFSET);
            break;
        case 6:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_14_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X6_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X6_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_14_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_15_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y6_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y6_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_15_OFFSET);
            break;
        case 7:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_16_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X7_MIN, left_x);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_X7_MAX, right_x);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_16_OFFSET);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_17_OFFSET);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y7_MIN, top_y);
            tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_Y7_MAX, bottom_y);
            writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_17_OFFSET);
            break;
    }
}

void ISP_WDR_Get_ROI_Position(int roi_id, int *left_x, int *right_x,
                                int *top_y, int *bottom_y)
{
    uint32_t tmpVal;

    switch(roi_id)
    {
        case 0:
            tmpVal  = readl(WDR_BASE + WDR_CSR_WDR_HIST_2_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X0_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X0_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_3_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y0_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y0_MAX);
            break;
        case 1:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_4_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X1_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X1_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_5_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y1_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y1_MAX);
            break;
        case 2:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_6_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X2_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X2_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_7_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y2_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y2_MAX);
            break;
        case 3:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_8_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X3_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X3_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_9_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y3_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y3_MAX);
            break;
        case 4:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_10_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X4_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X4_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_11_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y4_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y4_MAX);
            break;
        case 5:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_12_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X5_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X5_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_13_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y5_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y5_MAX);
            break;
        case 6:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_14_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X6_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X6_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_15_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y6_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y6_MAX);
            break;
        case 7:
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_16_OFFSET);
            *left_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X7_MIN);
            *right_x = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_X7_MAX);
            tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_17_OFFSET);
            *top_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y7_MIN);
            *bottom_y = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_Y7_MAX);
            break;
    }
}

void ISP_WDR_Set_ROI_Weight(int roi_id, int weight)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(weight, ISP_WDR_ROI_WEIGHT_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_1_OFFSET);
    switch(roi_id)
    {
        case 0:
        case 1:
            if (roi_id & 0x01) {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI1_WEIGHT, weight);
            } else {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI0_WEIGHT, weight);
            }
            break;
        case 2:
        case 3:
            if (roi_id & 0x01) {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI3_WEIGHT, weight);
            } else {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI2_WEIGHT, weight);
            }
            break;
        case 4:
        case 5:
            if (roi_id & 0x01) {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI5_WEIGHT, weight);
            } else {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI4_WEIGHT, weight);
            }
            break;
        case 6:
        case 7:
            if (roi_id & 0x01) {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI7_WEIGHT, weight);
            } else {
                tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_HIST_ROI6_WEIGHT, weight);
            }
            break;
    }
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_HIST_1_OFFSET);
}

void ISP_WDR_Get_ROI_Weight(int roi_id, int *weight)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(*weight, ISP_WDR_ROI_WEIGHT_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_HIST_1_OFFSET);

    switch(roi_id)
    {
        case 0:
        case 1:
            if (roi_id & 0x01) {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI1_WEIGHT);
            } else {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI0_WEIGHT);
            }
            break;
        case 2:
        case 3:
            if (roi_id & 0x01) {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI3_WEIGHT);
            } else {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI2_WEIGHT);
            }
            break;
        case 4:
        case 5:
            if (roi_id & 0x01) {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI5_WEIGHT);
            } else {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI4_WEIGHT);
            }
            break;
        case 6:
        case 7:
            if (roi_id & 0x01) {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI7_WEIGHT);
            } else {
                *weight = reg_get_bits(tmpVal, WDR_CSR_CR_HIST_ROI6_WEIGHT);
            }
            break;
    }
}


int ISP_WDR_Get_Hist_Data(ISP_WDR_Hist_Data_Type *hist)
{
    uint32_t tmpVal;
    uint32_t buf_idx;
    uint32_t buf_addr;
    uint32_t w_cnt_idx_start;
    uint32_t w_cnt_idx_end;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);
    buf_idx = reg_get_bits(tmpVal, WDR_CSR_STS_WDR_BUF_IDX);
    w_cnt_idx_start = reg_get_bits(tmpVal, WDR_CSR_STS_WDR_W_CNT);

    if (0 == buf_idx) {
        buf_addr = WDR_BASE + WDR_HIST_STS_REG_HIST0_0_OFFSET;
    } else {
        buf_addr = WDR_BASE + WDR_HIST_STS_REG_HIST1_0_OFFSET;
    }

    /* copy all the histogram bin except for the last one */
    arch_memcpy4(&hist->hist_bin[0], (uint32_t*)buf_addr, 65);

    w_cnt_idx_end = reg_get_bits(tmpVal, WDR_CSR_STS_WDR_W_CNT);
    if (w_cnt_idx_start != w_cnt_idx_end) {
        LOG_W("wdr statistics overwrite!");
        return -1;
    }

    return 0;
}

void ISP_WDR_Set_WDR_Curve_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, WDR_CSR_CR_CURVE_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, WDR_CSR_CR_CURVE_EN);
    }
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);
}

void ISP_WDR_Get_WDR_Curve_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);

    if (reg_is_bit_set(tmpVal, WDR_CSR_CR_CURVE_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_WDR_Set_WDR_Curve(const ISP_WDR_Curve_Type *wdr_curve)
{
    arch_memcpy4((uint32_t*)(WDR_BASE + WDR_CSR_WDR_CURVE_1_OFFSET), (uint32_t*)&wdr_curve->curve_coef[0], ISP_WDR_CURVE_COEF_NUM / 2);
}

void ISP_WDR_Get_WDR_Curve(ISP_WDR_Curve_Type *wdr_curve)
{
    arch_memcpy4((uint32_t*)&wdr_curve->curve_coef[0], (uint32_t*)(WDR_BASE + WDR_CSR_WDR_CURVE_1_OFFSET), ISP_WDR_CURVE_COEF_NUM / 2);
}

void ISP_WDR_Set_WDR_Curve_Weight(int weight)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(weight, ISP_WDR_CURVE_WEIGHT_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_CURVE_WEIGHT, weight);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);
}

void ISP_WDR_Get_WDR_Curve_Weight(int *weight)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_CURVE_MISC_OFFSET);
    *weight = reg_get_bits(tmpVal, WDR_CSR_CR_CURVE_WEIGHT);
}

void ISP_WDR_Get_Post_Enhance_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);

    if (reg_is_bit_set(tmpVal, WDR_CSR_CR_ENH_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_WDR_Set_Post_Enhance_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, WDR_CSR_CR_ENH_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, WDR_CSR_CR_ENH_EN);
    }
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
}

void ISP_WDR_Set_Post_Enhance_Weight(int weight)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(weight, ISP_WDR_POST_ENHANCE_WEIGHT_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_ENH_WEIGHT, weight);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
}

void ISP_WDR_Get_Post_Enhance_Weight(int *weight)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
    *weight = reg_get_bits(tmpVal, WDR_CSR_CR_ENH_WEIGHT);
}

void ISP_WDR_Set_Post_Enhance_Y_Offset(int y_offset)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(y_offset, ISP_WDR_POST_ENHANCE_Y_OFFSET_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_ENH_Y_OFFSET, y_offset);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
}

void ISP_WDR_Get_Post_Enhance_Y_Offset(int *y_offset)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_1_OFFSET);
    *y_offset = reg_get_bits(tmpVal, WDR_CSR_CR_ENH_Y_OFFSET);
}

void ISP_WDR_Set_Post_Enhance_Luma_Gain(int gain)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(gain, ISP_WDR_POST_ENHANCE_Y_GAIN_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_ENH_Y_GAIN, gain);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
}

void ISP_WDR_Get_Post_Enhance_Luma_Gain(int *gain)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
    *gain = reg_get_bits(tmpVal, WDR_CSR_CR_ENH_Y_GAIN);
}

void ISP_WDR_Set_Post_Enhance_Chroma_Gain(int gain)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(gain, ISP_WDR_POST_ENHANCE_C_GAIN_MAX);

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_ENH_C_GAIN, gain);
    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
}

void ISP_WDR_Get_Post_Enhance_Chroma_Gain(int *gain)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_ENH_2_OFFSET);
    *gain = reg_get_bits(tmpVal, WDR_CSR_CR_ENH_C_GAIN);
}

void ISP_WDR_Set_Interrupt(uint8_t enable)
{
    uint32_t tmpVal;
    uint8_t tmpEn;

    tmpEn = (0 == enable)?1:0;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_0_OFFSET);

    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_WDR_INT_MASK, tmpEn);

    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_0_OFFSET);
}

void ISP_WDR_Clr_Interrupt_Status(void)
{
    uint32_t tmpVal;

    tmpVal = readl(WDR_BASE + WDR_CSR_WDR_W1P_OFFSET);

    tmpVal = reg_set_bits(tmpVal, WDR_CSR_CR_WDR_INT_CLR, 1);

    writel(tmpVal, WDR_BASE + WDR_CSR_WDR_W1P_OFFSET);
}
