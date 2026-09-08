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

#include <string.h>

#include <drv_isp.h>
#include <isp_reg.h>
#include <gamma_csr_reg.h>
#include <gamma_table_reg.h>

#define SIGN_EXTEND(x, num_bits)    ((((int)(x)) & ((1 << num_bits) - 1)) << (32 - num_bits) >> (32 - num_bits))

#define ISP_BRIGHTNESS_MAX  (127)
#define ISP_BRIGHTNESS_MIN  (-128)

#define ISP_CONTRAST_MAX  (255)
#define ISP_CONTRAST_MIN  (0)

#define ISP_SATURATION_MAX  (255)
#define ISP_SATURATION_MIN  (0)

#define ISP_SOLAR_SEPIA_MAX (255)
#define ISP_SOLAR_SEPIA_MIN (0)

#define ISP_SOLAR_THRED_MAX (255)
#define ISP_SOLAR_THRED_MIN (0)

#define ISP_YUV_ADJUST_DEFAULT_Y_CONTRAST_MUL1   (862)
#define ISP_YUV_ADJUST_DEFAULT_Y_CONTRAST_MUL0   (896)
#define ISP_YUV_ADJUST_DEFAULT_Y_BRIGHTNESS      (0)

#define ISP_YUV_ADJUST_DEFAULT_UV_MAX            (240)
#define ISP_YUV_ADJUST_DEFAULT_UV_MIN            (16)
#define ISP_YUV_ADJUST_DEFAULT_Y_MAX             (235)
#define ISP_YUV_ADJUST_DEFAULT_Y_MIN             (16)

#define ISP_YUV_ADJUST_DEFAULT_UV_STURATION_MUL1 (903)
#define ISP_YUV_ADJUST_DEFAULT_UV_STURATION_MUL0 (896)

#define ISP_CE_MAX (2047)
#define ISP_CE_MIN (-2048)

#define ISP_CE_OFFSET_MAX (4095)
#define ISP_CE_OFFSET_MIN (-4096)

#define ISP_AWB_ROI_START_X_RANGE_MAX       2047
#define ISP_AWB_ROI_START_Y_RANGE_MAX       2047
#define ISP_AWB_ROI_WIDTH_RANGE_MAX         2047
#define ISP_AWB_ROI_WIDTH_RANGE_MIN         32
#define ISP_AWB_ROI_HEIGHT_RANGE_MAX        2047
#define ISP_AWB_ROI_HEIGHT_RANGE_MIN        2

#define ISP_AE_ROI_START_X_RANGE_MAX        2047
#define ISP_AE_ROI_START_Y_RANGE_MAX        2047
#define ISP_AE_ROI_WIDTH_RANGE_MAX          2047
#define ISP_AE_ROI_WIDTH_RANGE_MIN          32
#define ISP_AE_ROI_HEIGHT_RANGE_MAX         2047
#define ISP_AE_ROI_HEIGHT_RANGE_MIN         2

#define ISP_AE_HIST_ROI_MAX 2047
#define ISP_AE_HIST_ROI_MIN 0

#define ISP_AE_GRID_WIDTH_RANGE_MAX     255
#define ISP_AE_GRID_WIDTH_RANGE_MIN     32
#define ISP_AE_GRID_HEIGHT_RANGE_MAX    255
#define ISP_AE_GRID_HEIGHT_RANGE_MIN    2
#define ISP_AE_GRID_NUM_X_RANGE_MAX     12
#define ISP_AE_GRID_NUM_X_RANGE_MIN     1
#define ISP_AE_GRID_NUM_Y_RANGE_MAX     8

#define ISP_AE_SRAM_BANK1      (AE_AVG_BASE)
#define ISP_AE_SRAM_BANK2      (AE_AVG_BASE + (AE_STATS_GRID_NUM * 4 * 2))

#define AE_HIST_SUM_ADDR_Y     (AE_HIST_BASE)
#define AE_HIST_SUM_ADDR_R     (AE_HIST_BASE)
#define AE_HIST_SUM_ADDR_GR    (AE_HIST_BASE + (0x104))
#define AE_HIST_SUM_ADDR_GB    (AE_HIST_BASE + (0x208))
#define AE_HIST_SUM_ADDR_B     (AE_HIST_BASE + (0x30c))

void ISP_Set_Brightness(int value)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(value, ISP_BRIGHTNESS_MIN, ISP_BRIGHTNESS_MAX);

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_YBRIGHTNESS, value);
    writel(tmpVal, ISP_BASE + ISP_YUVC_A_OFFSET);
}

void ISP_Get_Brightness(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    *value = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_YBRIGHTNESS), ISP_YBRIGHTNESS_LEN);
}

void ISP_Set_Contrast(int value)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(value, ISP_CONTRAST_MIN, ISP_CONTRAST_MAX);

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_YCONTRAST, value);
    writel(tmpVal, ISP_BASE + ISP_YUVC_A_OFFSET);
}

void ISP_Get_Contrast(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    *value = reg_get_bits(tmpVal, ISP_YCONTRAST);
}

void ISP_Set_Saturation(const ISP_SATURATION_S *saturation)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(saturation->saturation_cb, ISP_SATURATION_MAX);
    CHECK_PARAM_MAX(saturation->saturation_cr, ISP_SATURATION_MAX);

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_CBSATURATION, saturation->saturation_cb);
    tmpVal = reg_set_bits(tmpVal, ISP_CRSATURATION, saturation->saturation_cr);
    writel(tmpVal, ISP_BASE + ISP_YUVC_A_OFFSET);
}

void ISP_Get_Saturation(ISP_SATURATION_S *saturation)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_A_OFFSET);
    saturation->saturation_cb = reg_get_bits(tmpVal, ISP_CBSATURATION);
    saturation->saturation_cr = reg_get_bits(tmpVal, ISP_CRSATURATION);
}

void ISP_Set_Special_Mode(int value)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(value, 0, 4);

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_SPECIALMODE, value);
    writel(tmpVal, ISP_BASE + ISP_YUVC_B_OFFSET);
}

void ISP_Get_Special_Mode(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    *value = reg_get_bits(tmpVal, ISP_SPECIALMODE);
}

void ISP_Set_LPF_Filting_Mode(int value)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(value, 0, 1);

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_C444TO422FILTERINGMODE, value);
    writel(tmpVal, ISP_BASE + ISP_YUVC_B_OFFSET);
}

void ISP_Get_LPF_Filting_Mode(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    *value = reg_get_bits(tmpVal, ISP_C444TO422FILTERINGMODE);
}

void ISP_Set_Sepia(int SepiaCr, int SepiaCb)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(SepiaCr, ISP_SOLAR_SEPIA_MIN, ISP_SOLAR_SEPIA_MAX);
    CHECK_PARAM_RANGE(SepiaCb, ISP_SOLAR_SEPIA_MIN, ISP_SOLAR_SEPIA_MAX);

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_SEPIACR, SepiaCr);
    tmpVal = reg_set_bits(tmpVal, ISP_SEPIACB, SepiaCb);
    writel(tmpVal, ISP_BASE + ISP_YUVC_B_OFFSET);
}

void ISP_Get_Sepia(int *SepiaCr, int *SepiaCb)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    *SepiaCr = reg_get_bits(tmpVal, ISP_SEPIACR);
    *SepiaCb = reg_get_bits(tmpVal, ISP_SEPIACB);
}

void ISP_Set_Solar_Thred(int value)
{
    uint32_t tmpVal;

    CHECK_PARAM_RANGE(value, ISP_SOLAR_THRED_MIN, ISP_SOLAR_THRED_MAX);

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_SOLARTHRED, value);
    writel(tmpVal, ISP_BASE + ISP_YUVC_B_OFFSET);
}

void ISP_Get_Solar_Thred(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_YUVC_B_OFFSET);
    *value = reg_get_bits(tmpVal, ISP_SOLARTHRED);
}

void ISP_Set_Crop_Cfg(ISP_Crop_Cfg_Type* cropCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CROP_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_CROP_VSYNC_START, cropCfg->vsyncStart);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_CROP_VSYNC_END, cropCfg->vsyncEnd);
    writel(tmpVal, ISP_BASE + ISP_CROP_0_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CROP_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_CROP_HSYNC_START, cropCfg->hsyncStart);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_CROP_HSYNC_END, cropCfg->hsyncEnd);
    writel(tmpVal, ISP_BASE + ISP_CROP_1_OFFSET);
}

void ISP_Get_Crop_Cfg(ISP_Crop_Cfg_Type* cropCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CROP_0_OFFSET);
    cropCfg->vsyncStart = reg_get_bits(tmpVal,ISP_REG_CROP_VSYNC_START);
    cropCfg->vsyncEnd = reg_get_bits(tmpVal,ISP_REG_CROP_VSYNC_END);

    tmpVal = readl(ISP_BASE + ISP_CROP_1_OFFSET);
    cropCfg->hsyncStart = reg_get_bits(tmpVal,ISP_REG_CROP_HSYNC_START);
    cropCfg->hsyncEnd = reg_get_bits(tmpVal,ISP_REG_CROP_HSYNC_END);
}

void ISP_Set_Crop_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CROP_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, ISP_REG_CROP_ENABLE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_REG_CROP_ENABLE);
    }
    writel(tmpVal, ISP_BASE + ISP_CROP_0_OFFSET);

}

void ISP_Get_Crop_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CROP_0_OFFSET);

    if (reg_is_bit_set(tmpVal, ISP_REG_CROP_ENABLE)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_YUV_Adjust_Init(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MUL0,  ISP_YUV_ADJUST_DEFAULT_Y_CONTRAST_MUL0);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MUL1,  ISP_YUV_ADJUST_DEFAULT_Y_CONTRAST_MUL1);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_LUMA,  ISP_YUV_ADJUST_DEFAULT_Y_BRIGHTNESS);
    writel(tmpVal, ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MAX,  ISP_YUV_ADJUST_DEFAULT_UV_MAX);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MIN,  ISP_YUV_ADJUST_DEFAULT_UV_MIN);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MAX,   ISP_YUV_ADJUST_DEFAULT_Y_MAX);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MIN,   ISP_YUV_ADJUST_DEFAULT_Y_MIN);
    writel(tmpVal, ISP_BASE + ISP_ADJA_CTRL_1_OFFSET);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MUL0,  ISP_YUV_ADJUST_DEFAULT_UV_STURATION_MUL0);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MUL1,  ISP_YUV_ADJUST_DEFAULT_UV_STURATION_MUL1);
    writel(tmpVal, ISP_BASE + ISP_ADJA_CTRL_2_OFFSET);

    ISP_Set_YUV_Adjust_State(DISABLE);
}

void ISP_Set_YUV_Adjust_Cfg(ISP_YUV_Adjust_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MUL0,  cfg->y_mul0);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MUL1,  cfg->y_mul1);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_LUMA,  cfg->y_luma);
    writel(tmpVal, ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MAX,  cfg->uv_max);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MIN,  cfg->uv_min);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MAX,   cfg->y_max);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_Y_MIN,   cfg->y_min);
    writel(tmpVal, ISP_BASE + ISP_ADJA_CTRL_1_OFFSET);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MUL0,  cfg->uv_mul0);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_UV_MUL1,  cfg->uv_mul1);
    writel(tmpVal, ISP_BASE + ISP_ADJA_CTRL_2_OFFSET);
}

void ISP_Get_YUV_Adjust_Cfg(ISP_YUV_Adjust_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    cfg->y_mul0 = reg_get_bits(tmpVal, ISP_REG_ADJ_Y_MUL0);
    cfg->y_mul1 = reg_get_bits(tmpVal, ISP_REG_ADJ_Y_MUL1);
    cfg->y_luma = reg_get_bits(tmpVal, ISP_REG_ADJ_Y_LUMA);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_1_OFFSET);
    cfg->uv_max = reg_get_bits(tmpVal, ISP_REG_ADJ_UV_MAX);
    cfg->uv_min = reg_get_bits(tmpVal, ISP_REG_ADJ_UV_MIN);
    cfg->y_max = reg_get_bits(tmpVal, ISP_REG_ADJ_Y_MAX);
    cfg->y_min = reg_get_bits(tmpVal, ISP_REG_ADJ_Y_MIN);
    tmpVal = readl(ISP_BASE + ISP_ADJA_CTRL_2_OFFSET);
    cfg->uv_mul0 = reg_get_bits(tmpVal, ISP_REG_ADJ_UV_MUL0);
    cfg->uv_mul1 = reg_get_bits(tmpVal, ISP_REG_ADJ_UV_MUL1);
}

void ISP_Set_YUV_Adjust_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_REG_ADJ_ADJ_EB, state);
    writel(tmpVal, ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
}

void ISP_Get_YUV_Adjust_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_ADJ_CTRL_0_OFFSET);
    *state = reg_get_bits(tmpVal, ISP_REG_ADJ_ADJ_EB);
}

/*
    should not change, yuv range should change in yuv adjust module
*/

void ISP_Init_Color_Enhancement()
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_00, 0x99);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_01, 0x12D);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_3_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_02, 0x3A);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_10, 0xFA9);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_4_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_11, 0xF57);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_12, 0x100);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_5_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_6_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_20, 0x100);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_21, 0xF29);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_6_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_7_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_22, 0xFD7);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_7_OFFSET);

    ISP_Set_CE_State(DISABLE);
}

void ISP_Set_CE_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_EN, state);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
}

void ISP_Get_CE_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    *state = reg_get_bits(tmpVal, ISP_RG_CE_EN);
}

void ISP_Set_CE_Coef(const ISP_CE_COEF_S *ce)
{
    int i;
    uint32_t tmpVal;

    for (i = 0; i < CE_COEF_NUM; i++) {
        CHECK_PARAM_RANGE(ce->coef[i], ISP_CE_MIN, ISP_CE_MAX);
    }

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_00, ce->coef[ISP_CE_11]);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_01, ce->coef[ISP_CE_12]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_3_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_02, ce->coef[ISP_CE_13]);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_10, ce->coef[ISP_CE_21]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_4_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_11, ce->coef[ISP_CE_22]);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_12, ce->coef[ISP_CE_23]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_5_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_6_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_20, ce->coef[ISP_CE_31]);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_21, ce->coef[ISP_CE_32]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_6_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_7_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_MTX_22, ce->coef[ISP_CE_33]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_7_OFFSET);

}

void ISP_Get_CE_Coef(ISP_CE_COEF_S *ce)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_3_OFFSET);
    ce->coef[ISP_CE_11] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_00), ISP_RG_CE_MTX_00_LEN);
    ce->coef[ISP_CE_12] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_01), ISP_RG_CE_MTX_01_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_4_OFFSET);
    ce->coef[ISP_CE_13] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_02), ISP_RG_CE_MTX_02_LEN);
    ce->coef[ISP_CE_21] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_10), ISP_RG_CE_MTX_10_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_5_OFFSET);
    ce->coef[ISP_CE_22] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_11), ISP_RG_CE_MTX_11_LEN);
    ce->coef[ISP_CE_23] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_12), ISP_RG_CE_MTX_12_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_6_OFFSET);
    ce->coef[ISP_CE_31] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_20), ISP_RG_CE_MTX_20_LEN);
    ce->coef[ISP_CE_32] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_21), ISP_RG_CE_MTX_21_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_7_OFFSET);
    ce->coef[ISP_CE_33] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_MTX_22), ISP_RG_CE_MTX_22_LEN);
}

void ISP_Set_CE_Pre_Offset(const ISP_CE_OFFSET_S *ce)
{
    int i;
    uint32_t tmpVal;

    for (i = 0; i < CE_OFFSET_NUM; i++) {
        CHECK_PARAM_RANGE(ce->offset[i], ISP_CE_OFFSET_MIN, ISP_CE_OFFSET_MAX);
    }

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_PRE_0, ce->offset[ISP_CE_OFFSET_0]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_0_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_PRE_1, ce->offset[ISP_CE_OFFSET_1]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_1_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_PRE_2, ce->offset[ISP_CE_OFFSET_2]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_2_OFFSET);

}

void ISP_Get_CE_Pre_Offset(ISP_CE_OFFSET_S *ce)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    ce->offset[ISP_CE_OFFSET_0] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_PRE_0), ISP_RG_CE_PRE_0_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_1_OFFSET);
    ce->offset[ISP_CE_OFFSET_1] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_PRE_1), ISP_RG_CE_PRE_1_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_2_OFFSET);
    ce->offset[ISP_CE_OFFSET_2] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_PRE_2), ISP_RG_CE_PRE_2_LEN);
}

void ISP_Set_CE_Post_Offset(const ISP_CE_OFFSET_S *ce)
{
    int i;
    uint32_t tmpVal;

    for (i = 0; i < CE_OFFSET_NUM; i++) {
        CHECK_PARAM_RANGE(ce->offset[i], ISP_CE_OFFSET_MIN, ISP_CE_OFFSET_MAX);
    }

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_POS_0, ce->offset[ISP_CE_OFFSET_0]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_0_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_POS_1, ce->offset[ISP_CE_OFFSET_1]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_1_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_RG_CE_POS_2, ce->offset[ISP_CE_OFFSET_2]);
    writel(tmpVal, ISP_BASE + ISP_CE_CONFIG_2_OFFSET);

}

void ISP_Get_CE_Post_Offset(ISP_CE_OFFSET_S *ce)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_0_OFFSET);
    ce->offset[ISP_CE_OFFSET_0] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_POS_0), ISP_RG_CE_POS_0_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_1_OFFSET);
    ce->offset[ISP_CE_OFFSET_1] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_POS_1), ISP_RG_CE_POS_1_LEN);

    tmpVal = readl(ISP_BASE + ISP_CE_CONFIG_2_OFFSET);
    ce->offset[ISP_CE_OFFSET_2] = SIGN_EXTEND(reg_get_bits(tmpVal, ISP_RG_CE_PRE_1), ISP_RG_CE_PRE_1_LEN);
}

void ISP_Set_Gamma_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(GAMMA_BASE + GAMMA_CSR_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, GAMMA_CSR_CR_GAMMA_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, GAMMA_CSR_CR_GAMMA_EN);
    }
    writel(tmpVal, GAMMA_BASE + GAMMA_CSR_0_OFFSET);
}

void ISP_Get_Gamma_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(GAMMA_BASE + GAMMA_CSR_0_OFFSET);

    if (reg_is_bit_set(tmpVal, GAMMA_CSR_CR_GAMMA_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_Set_Gamma_Update(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GAMMA_BASE + GAMMA_CSR_WDR_W1P_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GAMMA_CSR_CR_SRAM_SHADOW_EN);
    tmpVal = reg_set_bit(tmpVal, GAMMA_CSR_CR_SW_SHD);
    writel(tmpVal, GAMMA_BASE + GAMMA_CSR_WDR_W1P_OFFSET);
}

void ISP_Set_Gamma_Coef(const ISP_Gamma_Curve_Type *gamma_curve, ISP_Gamma_Channel chn)
{
    uint32_t tmpVal;
    uint32_t i = 0;
    uint16_t *p;
    uint32_t offset = GAMMA_TABLE_R0_OFFSET;

    switch(chn)
    {
        case ISP_GAMMA_CHANNEL_R:
            offset = GAMMA_TABLE_R0_OFFSET;
            break;
        case ISP_GAMMA_CHANNEL_G:
            offset = GAMMA_TABLE_G0_OFFSET;
            break;
        case ISP_GAMMA_CHANNEL_B:
            offset = GAMMA_TABLE_B0_OFFSET;
            break;
    }

    // total 257 elements, set first 256 elements for 32-bit IO
    p = (uint16_t *)gamma_curve;
    for( i = 0; i < (GAMMA_COEF_NUM>>1); i++)
    {
        tmpVal = 0;
        // tricky assign, three channel bit field are the same
        tmpVal = reg_set_bits(tmpVal, GAMMA_TABLE_CR_GAMMA_TABLE_R_0, *(p++));
        tmpVal = reg_set_bits(tmpVal, GAMMA_TABLE_CR_GAMMA_TABLE_R_1, *(p++));
        writel(tmpVal, GAMMA_BASE + offset + (i << 2));
    }

    // Set the rest one element
    if (ISP_GAMMA_CHANNEL_R == chn)
    {
        tmpVal = readl(GAMMA_BASE + GAMMA_CSR_0_OFFSET);
        tmpVal = reg_set_bits(tmpVal, GAMMA_CSR_CR_R_GAMMA_256, *(p++));
        writel(tmpVal, GAMMA_BASE + GAMMA_CSR_0_OFFSET);
    }
    else
    {
        tmpVal = readl(GAMMA_BASE + GAMMA_CSR__OFFSET);
        if (ISP_GAMMA_CHANNEL_G == chn)
        {
            tmpVal = reg_set_bits(tmpVal, GAMMA_CSR_CR_G_GAMMA_256, *(p++));
        }
        else
        {
            tmpVal = reg_set_bits(tmpVal, GAMMA_CSR_CR_B_GAMMA_256, *(p++));
        }
        writel(tmpVal, GAMMA_BASE + GAMMA_CSR__OFFSET);
    }
}

void ISP_Get_Gamma_Coef(ISP_Gamma_Curve_Type *gamma_curve, ISP_Gamma_Channel chn)
{
    uint32_t tmpVal;
    uint32_t i = 0;
    uint16_t *p;
    uint32_t offset;

    switch(chn)
    {
        case ISP_GAMMA_CHANNEL_R:
            offset = GAMMA_TABLE_R0_OFFSET;
            break;
        case ISP_GAMMA_CHANNEL_G:
            offset = GAMMA_TABLE_R0_OFFSET;
            break;
        case ISP_GAMMA_CHANNEL_B:
            offset = GAMMA_TABLE_R0_OFFSET;
            break;
    }

    p = (uint16_t *)gamma_curve;
    // total 257 elements, get first 256 elements for 32-bit IO
    for( i = 0; i < (GAMMA_COEF_NUM>>1); i++)
    {
        tmpVal = readl(GAMMA_BASE + offset + (i << 2));

        *(p++) = reg_get_bits(tmpVal, GAMMA_TABLE_CR_GAMMA_TABLE_R_0);
        *(p++) = reg_get_bits(tmpVal, GAMMA_TABLE_CR_GAMMA_TABLE_R_1);
    }

    // get the rest one element
    if (ISP_GAMMA_CHANNEL_R == chn)
    {
        tmpVal = readl(GAMMA_BASE + GAMMA_CSR_0_OFFSET);
        *(p++) = reg_get_bits(tmpVal, GAMMA_CSR_CR_R_GAMMA_256);
    }
    else
    {
        tmpVal = readl(GAMMA_BASE + GAMMA_CSR__OFFSET);
        if (ISP_GAMMA_CHANNEL_G == chn)
        {
            *(p++) = reg_get_bits(tmpVal, GAMMA_CSR_CR_G_GAMMA_256);
        }
        else
        {
            *(p++) = reg_get_bits(tmpVal, GAMMA_CSR_CR_B_GAMMA_256);
        }
    }

}

void ISP_Set_AWB3_Stats_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, ISP_AWB3_STAT_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_AWB3_STAT_EN);
    }

    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);
}

void ISP_Get_AWB3_Stats_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);
    if (reg_is_bit_set(tmpVal, ISP_AWB3_STAT_EN)){
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void ISP_Set_AWB3_Stats_Conf(const isp_awb3_stats_conf_t *awb_stats_conf)
{
    uint32_t tmpVal;
    uint32_t count;

    CHECK_PARAM_MAX(awb_stats_conf->stat_x_min, ISP_AWB_ROI_START_X_RANGE_MAX);
    CHECK_PARAM_MAX(awb_stats_conf->stat_x_max, ISP_AWB_ROI_START_X_RANGE_MAX);
    CHECK_PARAM_MAX(awb_stats_conf->stat_y_min, ISP_AWB_ROI_START_Y_RANGE_MAX);
    CHECK_PARAM_MAX(awb_stats_conf->stat_y_max, ISP_AWB_ROI_START_Y_RANGE_MAX);

    // awb window
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_STAT_X_MIN, awb_stats_conf->stat_x_min);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_STAT_X_MAX, awb_stats_conf->stat_x_max);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_STAT_Y_MIN, awb_stats_conf->stat_y_min);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_STAT_Y_MAX, awb_stats_conf->stat_y_max);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_1_OFFSET);

    // weight_ratio
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_WT_RATIO_EN, awb_stats_conf->weight_ratio_en);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_WT_RATIO_GAIN, awb_stats_conf->weight_ratio_gain);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);

    // y coeff
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_26_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_Y_R_COEFF, awb_stats_conf->y_r_coeff);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_26_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_27_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_Y_G_COEFF, awb_stats_conf->y_g_coeff);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_27_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_28_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_Y_B_COEFF, awb_stats_conf->y_b_coeff);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_28_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_29_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_WT_Y_USE_G, awb_stats_conf->weight_y_use_g);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_WT_Y_EN, awb_stats_conf->weight_y_en);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_WT_Y_GAIN, awb_stats_conf->weight_y_gain);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_29_OFFSET);

    count = ((ISP_AWB3_CONFIG_50_OFFSET - ISP_AWB3_CONFIG_30_OFFSET) >> 2) + 1;
    arch_memcpy4((uint32_t*)(ISP_BASE + ISP_AWB3_CONFIG_30_OFFSET), \
        (uint32_t*)&(awb_stats_conf->weight_y[0][0]), count);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_51_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_COUNT_ZERO_WT, awb_stats_conf->count_zero_weight);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_INK_TYPE, awb_stats_conf->ink_type);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_INK_GAIN, awb_stats_conf->ink_gain);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_51_OFFSET);

    // calibration
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_G_DIVISOR,    awb_stats_conf->g_divisor);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_R_RATIO_OFST, awb_stats_conf->r_ratio_ofst);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_B_RATIO_OFST, awb_stats_conf->b_ratio_ofst);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);


    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_R_RATIO_MULT, awb_stats_conf->r_ratio_mult);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_B_RATIO_MULT, awb_stats_conf->b_ratio_mult);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_3_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_RATIO_SUM_MIN, awb_stats_conf->ratio_sum_min);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_RATIO_SUM_MAX, awb_stats_conf->ratio_sum_max);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);

    count = ((ISP_AWB3_CONFIG_25_OFFSET - ISP_AWB3_CONFIG_5_OFFSET) >> 2) + 1;
    arch_memcpy4((uint32_t*)(ISP_BASE + ISP_AWB3_CONFIG_5_OFFSET), \
        (uint32_t*)&(awb_stats_conf->weight_ratio[0][0]), count);
}

void ISP_Get_AWB3_Stats_Conf(isp_awb3_stats_conf_t *awb_stats_conf)
{
    uint32_t tmpVal;
    uint32_t count;

    // awb window
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_0_OFFSET);
    awb_stats_conf->stat_x_min = reg_get_bits(tmpVal, ISP_AWB3_STAT_X_MIN);
    awb_stats_conf->stat_x_max = reg_get_bits(tmpVal, ISP_AWB3_STAT_X_MAX);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_1_OFFSET);
    awb_stats_conf->stat_y_min = reg_get_bits(tmpVal, ISP_AWB3_STAT_Y_MIN);
    awb_stats_conf->stat_y_max = reg_get_bits(tmpVal, ISP_AWB3_STAT_Y_MAX);

    // weight_ratio
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);
    awb_stats_conf->weight_ratio_en = reg_get_bits(tmpVal, ISP_AWB3_WT_RATIO_EN);
    awb_stats_conf->weight_ratio_gain = reg_get_bits(tmpVal, ISP_AWB3_WT_RATIO_GAIN);

    // y coeff
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_26_OFFSET);
    awb_stats_conf->y_r_coeff = reg_get_bits(tmpVal, ISP_AWB3_Y_R_COEFF);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_27_OFFSET);
    awb_stats_conf->y_g_coeff = reg_get_bits(tmpVal, ISP_AWB3_Y_G_COEFF);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_28_OFFSET);
    awb_stats_conf->y_b_coeff = reg_get_bits(tmpVal, ISP_AWB3_Y_B_COEFF);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_29_OFFSET);
    awb_stats_conf->weight_y_use_g = reg_get_bits(tmpVal, ISP_AWB3_WT_Y_USE_G);
    awb_stats_conf->weight_y_en = reg_get_bits(tmpVal, ISP_AWB3_WT_Y_EN);
    awb_stats_conf->weight_y_gain = reg_get_bits(tmpVal, ISP_AWB3_WT_Y_GAIN);

    count = ((ISP_AWB3_CONFIG_50_OFFSET - ISP_AWB3_CONFIG_30_OFFSET) >> 2) + 1;
    arch_memcpy4((uint32_t*)&(awb_stats_conf->weight_y[0][0]), \
        (uint32_t*)(ISP_BASE + ISP_AWB3_CONFIG_30_OFFSET), count);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_51_OFFSET);
    awb_stats_conf->count_zero_weight = reg_get_bits(tmpVal, ISP_AWB3_COUNT_ZERO_WT);
    awb_stats_conf->ink_type = reg_get_bits(tmpVal, ISP_AWB3_INK_TYPE);
    awb_stats_conf->ink_gain = reg_get_bits(tmpVal, ISP_AWB3_INK_GAIN);

    // calibration
    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);
    awb_stats_conf->g_divisor = reg_get_bits(tmpVal, ISP_AWB3_G_DIVISOR);
    awb_stats_conf->r_ratio_ofst = reg_get_bits(tmpVal, ISP_AWB3_R_RATIO_OFST);
    awb_stats_conf->b_ratio_ofst = reg_get_bits(tmpVal, ISP_AWB3_B_RATIO_OFST);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_3_OFFSET);
    awb_stats_conf->r_ratio_mult = reg_get_bits(tmpVal, ISP_AWB3_R_RATIO_MULT);
    awb_stats_conf->b_ratio_mult = reg_get_bits(tmpVal, ISP_AWB3_B_RATIO_MULT);

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_4_OFFSET);
    awb_stats_conf->ratio_sum_min = reg_get_bits(tmpVal, ISP_AWB3_RATIO_SUM_MIN);
    awb_stats_conf->ratio_sum_max = reg_get_bits(tmpVal, ISP_AWB3_RATIO_SUM_MAX);

    count = ((ISP_AWB3_CONFIG_25_OFFSET - ISP_AWB3_CONFIG_5_OFFSET) >> 2) + 1;
    arch_memcpy4((uint32_t*)&(awb_stats_conf->weight_ratio[0][0]), \
        (uint32_t*)(ISP_BASE + ISP_AWB3_CONFIG_5_OFFSET), count);
}

int ISP_Get_AWB3_Sum(isp_awb3_sum_t *sum)
{
    uint32_t tmpVal;
    uint64_t tmpVal64;
    uint32_t buf_idx;
    uint32_t w_cnt_idx_start;
    uint32_t w_cnt_idx_end;

    tmpVal = readl(ISP_BASE + ISP_AWB3_STATUS_0_OFFSET);
    buf_idx = reg_get_bits(tmpVal, ISP_AWB3_BUF_IDX_R);
    w_cnt_idx_start = reg_get_bits(tmpVal, ISP_AWB3_W_CNT_R);

    if (0 == buf_idx) {
        sum->r_sum   = readl(ISP_BASE + ISP_AWB3_STA_1_OFFSET);
        sum->g_sum   = readl(ISP_BASE + ISP_AWB3_STA_3_OFFSET);
        sum->b_sum   = readl(ISP_BASE + ISP_AWB3_STA_5_OFFSET);

        sum->r_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_7_OFFSET);
        sum->g_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_9_OFFSET);
        sum->b_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_11_OFFSET);

        sum->pixel_hex = readl(ISP_BASE + ISP_AWB3_STA_12_OFFSET);

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_13_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_14_OFFSET);
        sum->r_ratio_hex_y = (tmpVal64 << 32) + tmpVal;

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_15_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_16_OFFSET);
        sum->b_ratio_hex_y = (tmpVal64 << 32) + tmpVal;

        sum->weight_hex_y = readl(ISP_BASE + ISP_AWB3_STA_17_OFFSET);
        sum->pixel_hex_y  = readl(ISP_BASE + ISP_AWB3_STA_18_OFFSET);

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_19_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_20_OFFSET);
        sum->r_ratio_grid = (tmpVal64 << 32) + tmpVal;

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_21_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_22_OFFSET);
        sum->b_ratio_grid = (tmpVal64 << 32) + tmpVal;

        sum->weight_grid = readl(ISP_BASE + ISP_AWB3_STA_23_OFFSET);
        sum->pixel_grid  = readl(ISP_BASE + ISP_AWB3_STA_24_OFFSET);
    }
    else {
        sum->r_sum   = readl(ISP_BASE + ISP_AWB3_STA_32_OFFSET);
        sum->g_sum   = readl(ISP_BASE + ISP_AWB3_STA_34_OFFSET);
        sum->b_sum   = readl(ISP_BASE + ISP_AWB3_STA_36_OFFSET);

        sum->r_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_38_OFFSET);
        sum->g_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_40_OFFSET);
        sum->b_sum_hex   = readl(ISP_BASE + ISP_AWB3_STA_42_OFFSET);

        sum->pixel_hex = readl(ISP_BASE + ISP_AWB3_STA_43_OFFSET);

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_44_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_45_OFFSET);
        sum->r_ratio_hex_y = (tmpVal64 << 32) + tmpVal;

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_46_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_47_OFFSET);
        sum->b_ratio_hex_y = (tmpVal64 << 32) + tmpVal;

        sum->weight_hex_y = readl(ISP_BASE + ISP_AWB3_STA_48_OFFSET);
        sum->pixel_hex_y  = readl(ISP_BASE + ISP_AWB3_STA_49_OFFSET);

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_50_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_51_OFFSET);
        sum->r_ratio_grid = (tmpVal64 << 32) + tmpVal;

        tmpVal64 = readl(ISP_BASE + ISP_AWB3_STA_52_OFFSET);
        tmpVal   = readl(ISP_BASE + ISP_AWB3_STA_53_OFFSET);
        sum->b_ratio_grid = (tmpVal64 << 32) + tmpVal;

        sum->weight_grid = readl(ISP_BASE + ISP_AWB3_STA_54_OFFSET);
        sum->pixel_grid  = readl(ISP_BASE + ISP_AWB3_STA_55_OFFSET);
    }

    tmpVal = readl(ISP_BASE + ISP_AWB3_STATUS_0_OFFSET);
    w_cnt_idx_end = reg_get_bits(tmpVal, ISP_AWB3_W_CNT_R);

    if (w_cnt_idx_start != w_cnt_idx_end) {
        LOG_W("w_cnt_idx_start %lu, w_cnt_idx_end %lu \r\n", w_cnt_idx_start, w_cnt_idx_end);
        LOG_W("awb3 statistics overwrite!");
        return -1;
    }

    return 0;
}

int ISP_Get_AWB3_G_Divisor(int *value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);
    *value = reg_get_bits(tmpVal, ISP_AWB3_G_DIVISOR);

    return 0;
}

void ISP_Set_AWB3_G_Divisor(int value)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AWB3_G_DIVISOR, value);
    writel(tmpVal, ISP_BASE + ISP_AWB3_CONFIG_2_OFFSET);
}

void ISP_Set_RGBtoBayer(const BAYER_PATTERN_E pattern)
{
    uint32_t tmpVal;
    uint32_t value;

    if (pattern == BAYER_PATTERN_RG) {
        value = 0;
    } else if (pattern == BAYER_PATTERN_GR) {
        value = 1;
    } else if (pattern == BAYER_PATTERN_GB) {
        value = 2;
    } else { // for BG
        value = 3;
    }

    tmpVal = readl(ISP_BASE + ISP_BAYER2RGB_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_CR_RGB2BAYER_MODE, value);
    writel(tmpVal, ISP_BASE + ISP_BAYER2RGB_0_OFFSET);
}

void ISP_Get_RGBtoBayer(BAYER_PATTERN_E *pattern)
{
    uint32_t tmpVal;
    uint32_t value;

    tmpVal = readl(ISP_BASE + ISP_BAYER2RGB_0_OFFSET);
    value = reg_get_bits(tmpVal, ISP_CR_RGB2BAYER_MODE);

    if (value == 0) {
        *pattern = BAYER_PATTERN_RG;
    } else if (value == 1) {
        *pattern = BAYER_PATTERN_GR;
    } else if (value == 2) {
        *pattern = BAYER_PATTERN_GB;
    } else { // for BG
        *pattern = BAYER_PATTERN_BG;
    }
}

void ISP_Set_AE_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);

    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, ISP_NEW_AE_STA_ENABLE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_NEW_AE_STA_ENABLE);
    }

    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
}

void ISP_Set_AE_Stats_Conf(const isp_ae_stats_conf_t *ae_stats_conf)
{
    uint32_t tmpVal;

    CHECK_PARAM_MAX(ae_stats_conf->roi_start_x, ISP_AE_ROI_START_X_RANGE_MAX);
    CHECK_PARAM_MAX(ae_stats_conf->roi_start_y, ISP_AE_ROI_START_Y_RANGE_MAX);
    CHECK_PARAM_RANGE(ae_stats_conf->roi_width,
        ISP_AE_ROI_WIDTH_RANGE_MIN, ISP_AE_ROI_WIDTH_RANGE_MAX);
    CHECK_PARAM_RANGE(ae_stats_conf->roi_height,
        ISP_AE_ROI_HEIGHT_RANGE_MIN, ISP_AE_ROI_HEIGHT_RANGE_MAX);
    CHECK_PARAM_MIN(ae_stats_conf->grid_width, ISP_AE_GRID_WIDTH_RANGE_MIN);
    CHECK_PARAM_MIN(ae_stats_conf->grid_height, ISP_AE_GRID_HEIGHT_RANGE_MIN);
    CHECK_PARAM_RANGE(ae_stats_conf->grid_num_x,
        ISP_AE_GRID_NUM_X_RANGE_MIN, ISP_AE_GRID_NUM_X_RANGE_MAX);
    CHECK_PARAM_RANGE(ae_stats_conf->grid_num_y,
        ISP_AE_GRID_NUM_Y_RANGE_MIN, ISP_AE_GRID_NUM_Y_RANGE_MAX);

    /* special check */

    /* AE ROI must be in the active image region */
    if ((ae_stats_conf->roi_start_x + ae_stats_conf->roi_width > ae_stats_conf->img_width) ||
        (ae_stats_conf->roi_start_y + ae_stats_conf->roi_height > ae_stats_conf->img_height)) {
        LOG_E("AE ROI must be in the active image region!\r\n");
    }

    /* grid width/height must be even number */
    if ((ae_stats_conf->grid_width & 0x1) & (ae_stats_conf->grid_height & 0x1)) {
        LOG_E("Grid width/height must be even number!\r\n");
    }

    tmpVal = readl(ISP_BASE + ISP_AE_ROI_X_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_ROI_START_X, ae_stats_conf->roi_start_x);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_ROI_WIDTH, ae_stats_conf->roi_width);
    writel(tmpVal, ISP_BASE + ISP_AE_ROI_X_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AE_ROI_Y_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_ROI_START_Y, ae_stats_conf->roi_start_y);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_ROI_HEIGHT, ae_stats_conf->roi_height);
    writel(tmpVal, ISP_BASE + ISP_AE_ROI_Y_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AE_IMAGE_SIZE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_IMAGE_WIDTH, ae_stats_conf->img_width);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_IMAGE_HEIGHT, ae_stats_conf->img_height);
    writel(tmpVal, ISP_BASE + ISP_AE_IMAGE_SIZE_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_GRID_WIDTH, ae_stats_conf->grid_width);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_GRID_HEIGHT, ae_stats_conf->grid_height);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_GRID_X_MAX, ae_stats_conf->grid_num_x);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_GRID_Y_MAX, ae_stats_conf->grid_num_y);
    tmpVal = reg_set_bit(tmpVal, ISP_NEW_AE_STA_ENABLE);
    /* AE occupies 2 bank of memory*/
    tmpVal = reg_set_bit(tmpVal, ISP_AE_SWITCH_BANK_EN);
    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
}

void ISP_Get_AE_Stats_Conf(isp_ae_stats_conf_t *ae_stats_conf)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_ROI_X_OFFSET);
    ae_stats_conf->roi_start_x = reg_get_bits(tmpVal, ISP_AE_ROI_START_X);
    ae_stats_conf->roi_width = reg_get_bits(tmpVal, ISP_AE_ROI_WIDTH);

    tmpVal = readl(ISP_BASE + ISP_AE_ROI_Y_OFFSET);
    ae_stats_conf->roi_start_y = reg_get_bits(tmpVal, ISP_AE_ROI_START_Y);
    ae_stats_conf->roi_height = reg_get_bits(tmpVal, ISP_AE_ROI_HEIGHT);

    tmpVal = readl(ISP_BASE + ISP_AE_IMAGE_SIZE_OFFSET);
    ae_stats_conf->img_width = reg_get_bits(tmpVal, ISP_AE_IMAGE_WIDTH);
    ae_stats_conf->img_height = reg_get_bits(tmpVal, ISP_AE_IMAGE_HEIGHT);

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    ae_stats_conf->grid_width = reg_get_bits(tmpVal, ISP_AE_GRID_WIDTH);
    ae_stats_conf->grid_height = reg_get_bits(tmpVal, ISP_AE_GRID_HEIGHT);
    ae_stats_conf->grid_width = reg_get_bits(tmpVal, ISP_AE_GRID_X_MAX);
    ae_stats_conf->grid_height = reg_get_bits(tmpVal, ISP_AE_GRID_Y_MAX);
}

void ISP_Set_AE_Ysum_Coeff(uint8_t coeff_r, uint8_t coeff_g, uint8_t coeff_b)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_COEFF_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_YSUM_COEFF_R, coeff_r);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_YSUM_COEFF_G, coeff_g);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_YSUM_COEFF_B, coeff_b);
    writel(tmpVal, ISP_BASE + ISP_AE_COEFF_OFFSET);
}

void ISP_Get_AE_Ysum_Coeff(uint8_t *coeff_r, uint8_t *coeff_g, uint8_t *coeff_b)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_COEFF_OFFSET);
    *coeff_r = reg_get_bits(tmpVal, ISP_AE_YSUM_COEFF_R);
    *coeff_g = reg_get_bits(tmpVal, ISP_AE_YSUM_COEFF_G);
    *coeff_b = reg_get_bits(tmpVal, ISP_AE_YSUM_COEFF_B);
}

int ISP_Get_AE_Stats(int grid_num, uint32_t *data)
{
    uint32_t tmpVal;
    uint32_t buf_idx;
    uint32_t buf_addr;
    uint32_t w_cnt_idx_start;
    uint32_t w_cnt_idx_end;

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    buf_idx = reg_get_bits(tmpVal, ISP_BLAE_BUF_IDX_R);
    w_cnt_idx_start = reg_get_bits(tmpVal, ISP_BLAE_W_CNT_R);

    buf_addr = (0 == buf_idx) ? ISP_AE_SRAM_BANK1: ISP_AE_SRAM_BANK2;
    arch_memcpy4(data, (uint32_t*)buf_addr, grid_num*2); // two word per grid

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    w_cnt_idx_end = reg_get_bits(tmpVal, ISP_BLAE_W_CNT_R);
    if (w_cnt_idx_start != w_cnt_idx_end) {
        LOG_W("ae statistics overwrite!");
        return -1;
    }

    return 0;
}

void ISP_Get_AE_Sum(int grid_idx, AE_GRID_DATA_TYPE_E data_type, uint32_t *data)
{
    isp_ae_sum_t *grid_array;
    uint32_t tmpVal;
    uint32_t buf_idx;
    uint32_t buf_addr;

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    buf_idx = reg_get_bits(tmpVal, ISP_BLAE_BUF_IDX_R);

    buf_addr = (0 == buf_idx) ? ISP_AE_SRAM_BANK1: ISP_AE_SRAM_BANK2;
    grid_array = (isp_ae_sum_t*)buf_addr;
    *data = grid_array[grid_idx].y_sum;
}

void ISP_Set_AE_Hist_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, ISP_AE_HIST_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_AE_HIST_EN);
    }
    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
}

int  ISP_Get_AE_Hist_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);

    if (reg_is_bit_set(tmpVal, ISP_AE_HIST_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }

    return 0;
}

void ISP_Set_AE_Hist_Mode(AE_HIST_MODE_TYPE_E mode)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    if (AE_HIST_MODE_Y == mode) {
        tmpVal = reg_set_bit(tmpVal, ISP_AE_HIST_MODE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_AE_HIST_MODE);
    }
    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
}

int  ISP_Get_AE_Hist_Mode(AE_HIST_MODE_TYPE_E *mode)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);

    if (reg_is_bit_set(tmpVal, ISP_AE_HIST_EN)) {
        *mode = reg_get_bits(tmpVal, ISP_AE_HIST_MODE);
    } else {
        *mode = AE_HIST_MODE_Y;
    }

    return 0;
}

void ISP_Set_AE_Hist_ROI_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, ISP_AE_HIST_ROI_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, ISP_AE_HIST_ROI_EN);
    }
    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
}

int  ISP_Get_AE_Hist_ROI_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);

    if (reg_is_bit_set(tmpVal, ISP_AE_HIST_ROI_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }

    return 0;
}

int  ISP_Set_AE_Hist_ROI(uint8_t state, const isp_ae_hist_roi_t *roi)
{
    uint32_t tmpVal;

    if (DISABLE == state) {
        tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, ISP_AE_HIST_ROI_EN);
        writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
        return 0;
    }

    if (NULL == roi) {
        return -1;
    }

    CHECK_PARAM_MAX(roi->ae_hist_x_min, ISP_AE_HIST_ROI_MAX);
    CHECK_PARAM_MAX(roi->ae_hist_x_max, ISP_AE_HIST_ROI_MAX);
    CHECK_PARAM_MAX(roi->ae_hist_y_min, ISP_AE_HIST_ROI_MAX);
    CHECK_PARAM_MAX(roi->ae_hist_y_max, ISP_AE_HIST_ROI_MAX);

    tmpVal = readl(ISP_BASE + ISP_AE2_ROI_X_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_HIST_X_MIN, roi->ae_hist_x_min);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_HIST_X_MAX, roi->ae_hist_x_max);
    writel(tmpVal, ISP_BASE + ISP_AE2_ROI_X_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AE2_ROI_Y_OFFSET);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_HIST_Y_MIN, roi->ae_hist_y_min);
    tmpVal = reg_set_bits(tmpVal, ISP_AE_HIST_Y_MAX, roi->ae_hist_y_max);
    writel(tmpVal, ISP_BASE + ISP_AE2_ROI_Y_OFFSET);

    tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
    tmpVal = reg_set_bit(tmpVal, ISP_AE_HIST_ROI_EN);
    writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);

    return 0;
}

int  ISP_Get_AE_Hist_ROI(uint8_t state, isp_ae_hist_roi_t *roi)
{
    uint32_t tmpVal;

    if (DISABLE == state) {
        tmpVal = readl(ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, ISP_AE_HIST_ROI_EN);
        writel(tmpVal, ISP_BASE + ISP_AE_GRID_SIZE_OFFSET);
        return 0;
    }

    tmpVal = readl(ISP_BASE + ISP_AE2_ROI_X_OFFSET);
    roi->ae_hist_x_min = reg_get_bits(tmpVal, ISP_AE_HIST_X_MIN);
    roi->ae_hist_x_max = reg_get_bits(tmpVal, ISP_AE_HIST_X_MAX);

    tmpVal = readl(ISP_BASE + ISP_AE2_ROI_Y_OFFSET);
    roi->ae_hist_y_min = reg_get_bits(tmpVal, ISP_AE_HIST_Y_MIN);
    roi->ae_hist_y_max = reg_get_bits(tmpVal, ISP_AE_HIST_Y_MAX);

    return 0;
}

int ISP_Get_AE_W_Cnt_R(void)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);

    return reg_get_bits(tmpVal, ISP_BLAE_W_CNT_R);
}

int  ISP_Get_AE_Hist_SUM_Y(uint32_t *y_sum)
{
    uint32_t tmpVal;
    uint32_t w_cnt_idx_start;
    uint32_t w_cnt_idx_end;

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    w_cnt_idx_start = reg_get_bits(tmpVal, ISP_AE_HIST_W_CNT_R);

    arch_memcpy4(y_sum, (uint32_t*)AE_HIST_SUM_ADDR_Y, ISP_AE_HIST_Y_BIN_NUM);

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    w_cnt_idx_end = reg_get_bits(tmpVal, ISP_AE_HIST_W_CNT_R);
    if (w_cnt_idx_start != w_cnt_idx_end) {
        LOG_W("ae hist y sum overwrite!");
        return -1;
    }

    return 0;
}

int  ISP_Get_AE_Hist_SUM_Bayer(uint32_t *r_sum, uint32_t *gr_sum, uint32_t *gb_sum, uint32_t *b_sum)
{
    uint32_t tmpVal;
    uint32_t w_cnt_idx_start;
    uint32_t w_cnt_idx_end;

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    w_cnt_idx_start = reg_get_bits(tmpVal, ISP_AE_HIST_W_CNT_R);

    arch_memcpy4(r_sum,  (uint32_t*)AE_HIST_SUM_ADDR_R,  ISP_AE_HIST_BAYER_BIN_NUM);
    arch_memcpy4(gr_sum, (uint32_t*)AE_HIST_SUM_ADDR_GR, ISP_AE_HIST_BAYER_BIN_NUM);
    arch_memcpy4(gb_sum, (uint32_t*)AE_HIST_SUM_ADDR_GB, ISP_AE_HIST_BAYER_BIN_NUM);
    arch_memcpy4(b_sum,  (uint32_t*)AE_HIST_SUM_ADDR_B,  ISP_AE_HIST_BAYER_BIN_NUM);

    tmpVal = readl(ISP_BASE + ISP_AE_STATUS_OFFSET);
    w_cnt_idx_end = reg_get_bits(tmpVal, ISP_AE_HIST_W_CNT_R);
    if (w_cnt_idx_start != w_cnt_idx_end) {
        LOG_W("ae hist bayer sum overwrite!");
        return -1;
    }

    return 0;
}

void ISP_Set_2A_Interrupt(ISP_2A_INT_ID id, uint8_t enable)
{
    uint32_t tmpVal;
    uint8_t tmpEn;

    tmpEn = (0 == enable)?1:0;

    tmpVal = readl(ISP_BASE + ISP_MISC_0_OFFSET);

    if (ISP_2A_INT_ID_AE_STAT == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AE_INT_MASK, tmpEn);
    if (ISP_2A_INT_ID_AE_HIST == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AE_HIST_INT_MASK, tmpEn);
    if (ISP_2A_INT_ID_AWB_STAT == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AWB3_INT_MASK, tmpEn);

    writel(tmpVal, ISP_BASE + ISP_MISC_0_OFFSET);
}

void ISP_Clr_2A_Interrupt_Status(ISP_2A_INT_ID id)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_CFG_0_OFFSET);

    if (ISP_2A_INT_ID_AE_STAT == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AE_INT_CLR, 1);
    if (ISP_2A_INT_ID_AE_HIST == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AE_HIST_INT_CLR, 1);
    if (ISP_2A_INT_ID_AWB_STAT == id)
        tmpVal = reg_set_bits(tmpVal, ISP_RG_AWB3_INT_CLR, 1);

    writel(tmpVal, ISP_BASE + ISP_CFG_0_OFFSET);
}


void ISP_Set_Debug_Link_Sel(uint8_t id, ISP_DBG_LINK_SEL sel)
{
    uint32_t tmpVal;

    tmpVal = readl(ISP_BASE + ISP_DBG_0_OFFSET);
    if (0 == id)
        tmpVal = reg_set_bits(tmpVal, ISP_CR_INK0_SEL, sel);
    else
        tmpVal = reg_set_bits(tmpVal, ISP_CR_INK1_SEL, sel);

    writel(tmpVal, ISP_BASE + ISP_DBG_0_OFFSET);
}
