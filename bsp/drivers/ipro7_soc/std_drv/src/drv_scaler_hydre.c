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
#include <drv_scaler_hydre.h>
#include <scaler_hydre_reg.h>

#define W_BLANK_SOC_SUGGESTED_VAL (8) //(128)

#define SCALER_OUT_WIDTH_TH (1280)

static const int32_t MAX_RESOLUTION_BW = 14;
static const int32_t STEP_PRECISION = MAX_RESOLUTION_BW;

typedef struct {
  int32_t width_i; // r/w, shadow, 14'b = (MAX_RESOLUTION_BW)'b, range: 2 ~ max (must be even)
  int32_t height_i; // r/w, shadow, 14'b = (MAX_RESOLUTION_BW)'b, range: 2 ~ max (must be even)
  int32_t width_o; // r/w, shadow, 14'b = (MAX_RESOLUTION_BW)'b, range: 2 ~ max (must be even)
  int32_t height_o; // r/w, shadow, 14'b = (MAX_RESOLUTION_BW)'b, range: 2 ~ max (must be even)
  int32_t x_ini; // r/w, shadow, 28'b = (MAX_RESOLUTION_BW + STEP_PRECISION)'b, range: all
  int32_t y_ini; // r/w, shadow, 28'b = (MAX_RESOLUTION_BW + STEP_PRECISION)'b, range: all
  int32_t x_step; // r/w, shadow, 22'b = (ceil(log2(MAX_DOWN_SCALE_RATIO) + STEP_PRECISION)'b, range: 1 ~ max
  int32_t y_step; // r/w, shadow, 22'b = (ceil(log2(MAX_DOWN_SCALE_RATIO) + STEP_PRECISION)'b, range: 1 ~ max
}SCALER_REG_CFG_t;

static void get_scaler_cfg(SCALER_CONFIG_S info, SCALER_REG_CFG_t *reg_cfg)
{
    reg_cfg->width_i = info.input.in_width;
    reg_cfg->height_i = info.input.in_height;
    reg_cfg->width_o = info.output.width;
    reg_cfg->height_o = info.output.height;

    if (info.output.width >= info.input.width)
    {
      // horizontal up scaling
      reg_cfg->x_ini = 0;
      reg_cfg->x_step = (((info.input.width - 1) << STEP_PRECISION) + ((info.output.width - 1) >> 1)) / (info.output.width - 1);
    }
    else
    {
      // horizontal down scaling
      reg_cfg->x_ini = (((info.input.width << STEP_PRECISION) + (info.output.width >> 1)) / info.output.width - (1 << STEP_PRECISION)) >> 1;
      reg_cfg->x_step = ((info.input.width << STEP_PRECISION) + (info.output.width >> 1)) / info.output.width;
    }

    if (info.output.height >= info.input.height)
    {
      // vertical up scaling
      reg_cfg->y_ini = 0;
      reg_cfg->y_step = (((info.input.height - 1) << STEP_PRECISION) + ((info.output.height - 1) >> 1)) / (info.output.height - 1);
    }
    else
    {
      // vertical down scaling
      reg_cfg->y_ini = (((info.input.height << STEP_PRECISION) + (info.output.height >> 1)) / info.output.height - (1 << STEP_PRECISION)) >> 1;
      reg_cfg->y_step = ((info.input.height << STEP_PRECISION) + (info.output.height >> 1)) / info.output.height;
    }

    reg_cfg->x_ini += (info.input.start_x << STEP_PRECISION);
    reg_cfg->y_ini += (info.input.start_y << STEP_PRECISION);
}

void SCALER_HYDRE_SetSizeCfg(SCALER_HYDRE_TYPE_T scaler, SCALER_CONFIG_S size_cfg)
{
    SCALER_REG_CFG_t reg;
    get_scaler_cfg(size_cfg, &reg);

    uint32_t tmpVal;

    if (SCALER_HYDRE_TYPE_A == scaler)
    {
        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_0_OFFSET);
        tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_F0_EN);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_WIDTH_O, reg.width_o);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_HEIGHT_O, reg.height_o);
        if (reg.width_o > SCALER_OUT_WIDTH_TH)
            tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_F0_BP_OLINE_MODE);
        else
            tmpVal = reg_clr_bit(tmpVal, SCALER_HYDRE_CR_F0_BP_OLINE_MODE);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_0_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_WBLANK_O, W_BLANK_SOC_SUGGESTED_VAL);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_1_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_2_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_Y_INI, reg.y_ini);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_2_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_3_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_X_INI, reg.x_ini);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_3_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_4_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_Y_STEP, reg.y_step);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_4_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_5_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_X_STEP, reg.x_step);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_5_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_7_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_BP_OLINE_ACT_CNT, (reg.width_o>>1));
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_7_OFFSET);
    }
    else
    {
        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_0_OFFSET);
        tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_F1_EN);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_WIDTH_O, reg.width_o);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_HEIGHT_O, reg.height_o);
        if (reg.width_o > SCALER_OUT_WIDTH_TH)
            tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_F1_BP_OLINE_MODE);
        else
            tmpVal = reg_clr_bit(tmpVal, SCALER_HYDRE_CR_F1_BP_OLINE_MODE);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_0_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_1_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_WBLANK_O, W_BLANK_SOC_SUGGESTED_VAL);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_1_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_2_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_Y_INI, reg.y_ini);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_2_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_3_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_X_INI, reg.x_ini);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_3_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_4_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_Y_STEP, reg.y_step);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_4_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_5_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_X_STEP, reg.x_step);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_5_OFFSET);

        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_7_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_BP_OLINE_ACT_CNT, (reg.width_o>>1));
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_7_OFFSET);
    }

    // Config ready and set enable
    tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_MODULE_EN);
    tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_WIDTH_I, reg.width_i);
    tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_HEIGHT_I, reg.height_i);
    writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_CTRL_OFFSET);
}

void SCALER_HYDRE_SetXYCoef(SCALER_HYDRE_TYPE_T scaler, SCALER_COEF_S coef_cfg)
{
    uint32_t tmpVal;

    if (SCALER_HYDRE_TYPE_A == scaler)
    {
        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_6_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_Y_COEFF_RATIO, coef_cfg.y_coeff_ratio);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_X_COEFF_RATIO, coef_cfg.x_coeff_ratio);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_Y_COEFF_SET, coef_cfg.y_coeff_set);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F0_X_COEFF_SET, coef_cfg.x_coeff_set);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F0_6_OFFSET);
    }
    else
    {
        tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_6_OFFSET);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_Y_COEFF_RATIO, coef_cfg.y_coeff_ratio);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_X_COEFF_RATIO, coef_cfg.x_coeff_ratio);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_Y_COEFF_SET, coef_cfg.y_coeff_set);
        tmpVal = reg_set_bits(tmpVal, SCALER_HYDRE_CR_F1_X_COEFF_SET, coef_cfg.x_coeff_set);
        writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_F1_6_OFFSET);
    }
}

void SCALER_HYDRE_SetShdEn(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_SHD_OFFSET);
    tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_SW_SHD);
    writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_SHD_OFFSET);
}

void SCALER_HYDRE_CheckStatus(void)
{
    uint32_t tmpVal;

    tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_SHD_OFFSET);

    if (reg_is_bit_set(tmpVal, SCALER_HYDRE_ST_Y_LBUF_OVF)) {
        tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_Y_SRAM_OVF_CLR);
        LOG_D("Scaler hydre : Y line buffer overflow!\r\n");
    }

    if (reg_is_bit_set(tmpVal, SCALER_HYDRE_ST_C_LBUF_OVF)) {
        tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_C_SRAM_OVF_CLR);
        LOG_D("Scaler hydre : C line buffer overflow!\r\n");
    }

    writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_SHD_OFFSET);
}

void SCALER_HYDRE_ErrIntEn(uint8_t enable)
{
    uint32_t tmpVal;
    tmpVal = readl(SCALERDA_BASE + SCALER_HYDRE_SC_HY_CTRL_OFFSET);

    if (enable)
    {
        tmpVal = reg_set_bit(tmpVal, SCALER_HYDRE_CR_ERROR_INT_EN);
    }
    else
    {
        tmpVal = reg_clr_bit(tmpVal, SCALER_HYDRE_CR_ERROR_INT_EN);
    }

    writel(tmpVal, SCALERDA_BASE + SCALER_HYDRE_SC_HY_CTRL_OFFSET);
}
