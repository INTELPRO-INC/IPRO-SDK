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

#include <stdint.h>
#include <drv_mm_misc.h>
#include <mm_misc_reg.h>

#define DVP_AS_FIFO_MAX (1024)

void MM_MISC_Set_SNSR_Reset(uint8_t value)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_0_OFFSET);

    if (0 == value) {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_CR_SNSR_RESET);
    } else {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_CR_SNSR_RESET);
    }

    writel(tmpVal, MM_MISC_BASE + MM_MISC_0_OFFSET);
}

void MM_MISC_Set_SNSR_PowerDown(uint8_t value)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_0_OFFSET);

    if (0 == value) {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_CR_SNSR_PWDN);
    } else {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_CR_SNSR_PWDN);
    }

    writel(tmpVal, MM_MISC_BASE + MM_MISC_0_OFFSET);
}

void MM_MISC_Set_PCLK_Force_On(uint16_t value)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_FORCE_PCLK_ON, value);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_0_OFFSET);
}


void MM_MISC_Set_Mux_Shadow_Update(uint32_t id)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_1_OFFSET);

    if (id & MM_MISC_DVP_MUX_SH_ISP)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_ISP);
    if (id & MM_MISC_DVP_MUX_SH_SCALER)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_SCALER);
    if (id & MM_MISC_DVP_MUX_SH_OSD_BLEND)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_OSD_BLEND);
    if (id & MM_MISC_DVP_MUX_SH_OSD_DRAW)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_OSD_DRAW);
    if (id & MM_MISC_DVP_MUX_SH_422TO444)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_422TO444);
    if (id & MM_MISC_DVP_MUX_SH_422TO420)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_422TO420);
    if (id & MM_MISC_DVP_MUX_SH_TRA)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_TRA);
    if (id & MM_MISC_DVP_MUX_SH_444TO422)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_444TO422);
    if (id & MM_MISC_DVP_MUX_SH_D2XA)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_D2XA);
    if (id & MM_MISC_DVP_MUX_SH_D2XB)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_D2XB);
    if (id & MM_MISC_DVP_MUX_SH_D2XC)
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_DVP_MUX_SH_D2XC);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_1_OFFSET);
}

void MM_MISC_Set_SPI2DVP_Select(MM_MISC_SPI2DVP_SEL sel)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_SPI2DVP_SEL, sel);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_2_OFFSET);
}

void MM_MISC_Set_DVP_AS_SRAM_Mode(MM_MISC_SHARED_SRAM_MODE mode)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_SRAM_MODE, mode);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_2_OFFSET);
}

void MM_MISC_Set_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT port, MM_MISC_DVP2AXI_SRC src)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);

    switch (port)
    {
    case MM_MISC_DVP2AXI_PORT_A:
        tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_D2XA_IN_SEL, src);
        break;
    case MM_MISC_DVP2AXI_PORT_B:
        tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_D2XB_IN_SEL, src);
        break;
    case MM_MISC_DVP2AXI_PORT_C:
        tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_D2XC_IN_SEL, src);
        break;
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);
}

void MM_MISC_Get_DVP2AXI_Src(MM_MISC_DVP2AXI_PORT port, MM_MISC_DVP2AXI_SRC *src)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);
    switch (port)
    {
        case MM_MISC_DVP2AXI_PORT_A:
            *src = reg_get_bits(tmpVal, MM_MISC_REG_D2XA_IN_SEL);
            break;
        case MM_MISC_DVP2AXI_PORT_B:
            *src = reg_get_bits(tmpVal, MM_MISC_REG_D2XB_IN_SEL);
            break;
        case MM_MISC_DVP2AXI_PORT_C:
            *src = reg_get_bits(tmpVal, MM_MISC_REG_D2XC_IN_SEL);
            break;
    }
}

void MM_MISC_Set_YUV422_In_Mux(MM_MISC_YUV422_IN_MUX_ID id, MM_MISC_YUV422_IN_MUX_SEL sel)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_YUV422_MUX_REG0_OFFSET);

    switch (id)
    {
        case MM_MISC_YUV422_IN_MUX_ID_ISP:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_ISP_IN_SEL, sel);
            break;
        case MM_MISC_YUV422_IN_MUX_ID_SCALER:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_SCHY_IN_SEL, sel);
            break;
        case MM_MISC_YUV422_IN_MUX_ID_OSD_BLEND:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_OSD_BLEND_IN_SEL, sel);
            break;
        case MM_MISC_YUV422_IN_MUX_ID_OSD_DRAW:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_OSD_DRAW_IN_SEL, sel);
            break;
        case MM_MISC_YUV422_IN_MUX_ID_YUV422TO444:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_422TO444_IN_SEL, sel);
            break;
        case MM_MISC_YUV422_IN_MUX_ID_YUV422TO420:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_422TO420_IN_SEL, sel);
            break;
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_YUV422_MUX_REG0_OFFSET);
}

void MM_MISC_Set_YUV444_In_Mux(MM_MISC_YUV444_IN_MUX_ID id, MM_MISC_YUV444_IN_MUX_SEL sel)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_YUV444_MUX_REG0_OFFSET);

    switch (id)
    {
        case MM_MISC_YUV444_IN_MUX_ID_YUVTORGB:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_TRA_IN_SEL, sel);
            break;
        case MM_MISC_YUV444_IN_MUX_ID_YUV444TO422:
            tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_YUV444TO422_IN_SEL, sel);
            break;
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_YUV444_MUX_REG0_OFFSET);
}

void MM_MISC_Set_Crop_Cfg(MM_MISC_Crop_Cfg_Type* cropCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_CROPA_VSYNC_START, cropCfg->vsyncStart);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_CROPA_VSYNC_END, cropCfg->vsyncEnd);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_CROPA_HSYNC_START, cropCfg->hsyncStart);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_CROPA_HSYNC_END, cropCfg->hsyncEnd);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_CROPA_REG_1_OFFSET);
}

void MM_MISC_Get_Crop_Cfg(MM_MISC_Crop_Cfg_Type* cropCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);
    cropCfg->vsyncStart = reg_get_bits(tmpVal,MM_MISC_REG_CROPA_VSYNC_START);
    cropCfg->vsyncEnd = reg_get_bits(tmpVal,MM_MISC_REG_CROPA_VSYNC_END);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_1_OFFSET);
    cropCfg->hsyncStart = reg_get_bits(tmpVal,MM_MISC_REG_CROPA_HSYNC_START);
    cropCfg->hsyncEnd = reg_get_bits(tmpVal,MM_MISC_REG_CROPA_HSYNC_END);
}

void MM_MISC_Set_Crop_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_REG_CROPA_ENABLE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_REG_CROPA_ENABLE);
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);

}

void MM_MISC_Get_Crop_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CROPA_REG_0_OFFSET);

    if (reg_is_bit_set(tmpVal, MM_MISC_REG_CROPA_ENABLE)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void MM_MISC_YUV2RGB_Init(MM_MISC_YUV2RGB_Cfg_Type* yuv2rgbCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_0,yuv2rgbCfg->preOffset0);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_POS_0,yuv2rgbCfg->postOffset0);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_1_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_1,yuv2rgbCfg->preOffset1);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_POS_1,yuv2rgbCfg->postOffset1);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_1_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_2_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_2,yuv2rgbCfg->preOffset2);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_POS_2,yuv2rgbCfg->postOffset2);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_2_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_3_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_00,yuv2rgbCfg->matrix00);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_01,yuv2rgbCfg->matrix01);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_3_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_4_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_02,yuv2rgbCfg->matrix02);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_10,yuv2rgbCfg->matrix10);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_4_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_5_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_11,yuv2rgbCfg->matrix11);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_12,yuv2rgbCfg->matrix12);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_5_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_6_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_20,yuv2rgbCfg->matrix20);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_21,yuv2rgbCfg->matrix21);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_6_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_7_OFFSET);
    tmpVal = reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_22,yuv2rgbCfg->matrix22);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_Y2RA_REG_7_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);
    writel(reg_set_bits(tmpVal,MM_MISC_RG_Y2RA_EN,yuv2rgbCfg->enable), MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);
}

void MM_MISC_Get_YUV2RGB_Cfg(MM_MISC_YUV2RGB_Cfg_Type* yuv2rgbCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);
    yuv2rgbCfg->preOffset0 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_0);
    yuv2rgbCfg->postOffset0 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_POS_0);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_1_OFFSET);
    yuv2rgbCfg->preOffset1 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_1);
    yuv2rgbCfg->postOffset1 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_POS_1);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_2_OFFSET);
    yuv2rgbCfg->preOffset2 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_PRE_2);
    yuv2rgbCfg->postOffset2 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_POS_2);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_3_OFFSET);
    yuv2rgbCfg->matrix00 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_00);
    yuv2rgbCfg->matrix01 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_01);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_4_OFFSET);
    yuv2rgbCfg->matrix02 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_02);
    yuv2rgbCfg->matrix10 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_10);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_5_OFFSET);
    yuv2rgbCfg->matrix11 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_11);
    yuv2rgbCfg->matrix12 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_12);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_6_OFFSET);
    yuv2rgbCfg->matrix20 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_20);
    yuv2rgbCfg->matrix21 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_21);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_7_OFFSET);
    yuv2rgbCfg->matrix22 = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_MTX_22);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_Y2RA_REG_0_OFFSET);
    yuv2rgbCfg->enable = reg_get_bits(tmpVal,MM_MISC_RG_Y2RA_EN);
}

void MM_MISC_SPI2DVP_Init(MM_MISC_SPI2DVP_Cfg cfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SPI2DVP_DATA_FORMAT, cfg.dataFormat);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SPI2DVP_DATA_ORDER, cfg.dataOrder);

    if (MM_MISC_SPI2DVP_MODE_0 == cfg.spiMode)
    {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_POL);
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_PH);
    }
    else if (MM_MISC_SPI2DVP_MODE_1 == cfg.spiMode)
    {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_POL);
        tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_PH);
    }
    else if (MM_MISC_SPI2DVP_MODE_2 == cfg.spiMode)
    {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_POL);
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_PH);
    }
    else
    {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_POL);
        tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_SCLK_PH);
    }

    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SPI2DVP_ENDIAN, cfg.dataEndian);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
}

void MM_MISC_Set_SPI2DVP_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_ENABLE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_ENABLE);
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
}

void MM_MISC_Get_SPI2DVP_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);

    if (reg_is_bit_set(tmpVal, MM_MISC_RG_SPI2DVP_ENABLE)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void MM_MISC_SPI2DVP_INT_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, MM_MISC_RG_SPI2DVP_INT_EN);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
}

void MM_MISC_SPI2DVP_INT_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, MM_MISC_RG_SPI2DVP_INT_EN);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
}

void MM_MISC_Get_SPI2DVP_INT_Status(MM_MISC_SPI2DVP_INT_Status *status)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
    status->spi2dvp_sync_int = reg_get_bits(tmpVal, MM_MISC_SPI2DVP_SYNC_INT);
    status->spi2dvp_fs_int   = reg_get_bits(tmpVal, MM_MISC_SPI2DVP_FS_INT);
    status->spi2dvp_fe_int   = reg_get_bits(tmpVal, MM_MISC_SPI2DVP_FE_INT);
    status->spi2dvp_ls_int   = reg_get_bits(tmpVal, MM_MISC_SPI2DVP_LS_INT);
    status->spi2dvp_fifo_int = reg_get_bits(tmpVal, MM_MISC_SPI2DVP_FIFO_INT);
}

void MM_MISC_Get_SPI2DVP_Status(MM_MISC_SPI2DVP_Status *status)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_STATUS_0_OFFSET);
    status->spi2dvp_pkt_id    = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_PKT_ID);
    status->spi2dvp_data_id   = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_DATA_ID);
    status->spi2dvp_data_size = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_DATA_SIZE);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_STATUS_1_OFFSET);
    status->spi2dvp_img_w     = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_IMG_W);
    status->spi2dvp_img_h     = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_IMG_H);
}

void MM_MISC_SPI2DVP_WaitReady(void)
{
    uint32_t tmpVal;
    while (1) {
        tmpVal = readl(MM_MISC_BASE + MM_MISC_SPI2DVP_CTRL_OFFSET);
        if (reg_get_bits(tmpVal, MM_MISC_SPI2DVP_FS_INT) && reg_get_bits(tmpVal, MM_MISC_SPI2DVP_FE_INT))
            break;
    }
}

void MM_MISC_S2P_Init(MM_MISC_S2P_Cfg_Type* s2pCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_S2P_HS_INV, s2pCfg->hSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_S2P_VS_INV, s2pCfg->vSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_S2P_DA_ORDER, s2pCfg->dataOrder);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
}

void MM_MISC_Get_S2P_Cfg(MM_MISC_S2P_Cfg_Type* s2pCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    s2pCfg->hSyncLevel = reg_get_bits(tmpVal,MM_MISC_CR_DVP_S2P_HS_INV);
    s2pCfg->vSyncLevel = reg_get_bits(tmpVal,MM_MISC_CR_DVP_S2P_VS_INV);
    s2pCfg->dataOrder = reg_get_bits(tmpVal,MM_MISC_CR_DVP_S2P_DA_ORDER);
}

void MM_MISC_Set_S2P_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_CR_DVP_S2P_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_CR_DVP_S2P_EN);
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
}

void MM_MISC_Get_S2P_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    if (reg_is_bit_set(tmpVal, MM_MISC_CR_DVP_S2P_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void MM_MISC_Set_DVP_AS_TG_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_CR_DVP_AS_TG_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_CR_DVP_AS_TG_EN);
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
}

void MM_MISC_Set_DVP_AS_TG_Cfg(MM_MISC_DVP_AS_TG_Cfg* dvpAsTgCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_TOTAL_HPIXELS, dvpAsTgCfg->totalHPixels);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_ACT_START_LINE, dvpAsTgCfg->actStartLine);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG2_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_ACT_PIXEL_WIDTH, dvpAsTgCfg->actPixelWidth);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_ACT_START_PIXEL, dvpAsTgCfg->actStartPixel);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG1_OFFSET);

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_ACT_LINE_HEIGHT, dvpAsTgCfg->actLineHeight);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG3_OFFSET);
}

void MM_MISC_Set_DVP_AS_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, MM_MISC_CR_DVP_AS_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, MM_MISC_CR_DVP_AS_EN);
    }
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
}

void MM_MISC_Get_DVP_AS_State(uint8_t *state)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    if (reg_is_bit_set(tmpVal, MM_MISC_CR_DVP_AS_EN)) {
        *state = ENABLE;
    } else {
        *state = DISABLE;
    }
}

void MM_MISC_DVP_AS_Set_Cfg(MM_MISC_DVP_AS_Cfg* dvpAsCfg)
{
    uint32_t tmpVal;

    if (dvpAsCfg->fifoThreshold >= DVP_AS_FIFO_MAX)
    {
        printf("[WARN] dvp as threshold over 1024, lower BCLK or higher PCLK\r\n");
    }

    tmpVal = readl(MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_VS_INV, dvpAsCfg->vSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_HS_INV, dvpAsCfg->hSyncLevel);
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_FIFO_TH, dvpAsCfg->fifoThreshold);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
}

#define MM_MISC_SEOF_SET_SRC_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_SRC, selection);\
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);}

void MM_MISC_Set_SEOF_Src(MM_MISC_SEOF_E seof, MM_MISC_SEOF_INTR_SRC selection)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_SET_SRC_MACRO(0)
    else  MM_MISC_SEOF_SET_SRC_MACRO(1)
}

#define MM_MISC_SEOF_GET_SRC_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    *selection = reg_get_bits(tmpVal, MM_MISC_RG_SEOF##name##_SRC);}

void MM_MISC_Get_SEOF_Src(MM_MISC_SEOF_E seof, MM_MISC_SEOF_INTR_SRC *selection)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_GET_SRC_MACRO(0)
    else  MM_MISC_SEOF_GET_SRC_MACRO(1)
}

#define MM_MISC_SEOF_SET_TYPE_EN_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_TYPE_EN, type.byte);\
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);}

void MM_MISC_Set_SEOF_TypeEn(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E type)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_SET_TYPE_EN_MACRO(0)
    else  MM_MISC_SEOF_SET_TYPE_EN_MACRO(1)
}

#define MM_MISC_SEOF_GET_TYPE_EN_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    type->byte = reg_get_bits(tmpVal, MM_MISC_RG_SEOF##name##_TYPE_EN);}

void MM_MISC_Get_SEOF_TypeEn(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E *type)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_GET_TYPE_EN_MACRO(0)
    else  MM_MISC_SEOF_GET_TYPE_EN_MACRO(1)
}

#define MM_MISC_SEOF_SET_LINECNT_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_LCNT, lineCnt);\
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);}

void MM_MISC_Set_SEOF_LineCnt(MM_MISC_SEOF_E seof, uint16_t lineCnt)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_SET_LINECNT_MACRO(0)
    else  MM_MISC_SEOF_SET_LINECNT_MACRO(1)
}

#define MM_MISC_SEOF_GET_TYPE_STATUS_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF##name##_CFG_OFFSET);\
    type->byte = reg_get_bits(tmpVal, MM_MISC_ST_SEOF##name##_TYPE);}

void MM_MISC_Get_SEOF_TypeStatus(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E *type)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_GET_TYPE_STATUS_MACRO(0)
    else  MM_MISC_SEOF_GET_TYPE_STATUS_MACRO(1)
}

#define MM_MISC_SEOF_CLR_TYPE_STATUS_MACRO(name) \
    {tmpVal = readl(MM_MISC_BASE + MM_MISC_SEOF_CLR_OFFSET);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_TYPE0_CLR, type.BF.SoF);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_TYPE1_CLR, type.BF.EoF);\
    tmpVal = reg_set_bits(tmpVal, MM_MISC_RG_SEOF##name##_TYPE2_CLR, type.BF.lineCnt);\
    writel(tmpVal, MM_MISC_BASE + MM_MISC_SEOF_CLR_OFFSET);}

void MM_MISC_Clr_SEOF_TypeStatus(MM_MISC_SEOF_E seof, MM_MISC_SEOF_TYPE_E type)
{
    uint32_t tmpVal;

    if (seof == MM_MISC_SEOF0) MM_MISC_SEOF_CLR_TYPE_STATUS_MACRO(0)
    else  MM_MISC_SEOF_CLR_TYPE_STATUS_MACRO(1)
}

void MM_MISC_Set_SW_Reset(uint32_t id, uint8_t aRst)
{
    uint32_t tmpVal;
    uint8_t tmpRst;

    tmpRst = (0 == aRst)?0:1;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_RST_0_OFFSET);

    if (MM_MISC_SW_RESET_ID_MM_MISC & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_MM_MISC, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP2AXI_A & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_D2XA, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP2AXI_B & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_D2XB, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP2AXI_C & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_D2XC, tmpRst);
    if (MM_MISC_SW_RESET_ID_SCALER & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_SCHY, tmpRst);
    if (MM_MISC_SW_RESET_ID_OSD_DRAW & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_OSD_DRAW, tmpRst);
    if (MM_MISC_SW_RESET_ID_OSD_BLEND & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_OSD_BLEND, tmpRst);
    if (MM_MISC_SW_RESET_ID_JPG & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_JPG, tmpRst);
    if (MM_MISC_SW_RESET_ID_IPROAI & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_IPROAI, tmpRst);
    if (MM_MISC_SW_RESET_ID_ISP & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_ISP, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP_TSRC & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_DTSRC, tmpRst);
    if (MM_MISC_SW_RESET_ID_SPI2DVP & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_SPI2DVP, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP_AS & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_DVP_AS, tmpRst);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_RST_0_OFFSET);
}


void MM_MISC_Set_Clock_Gen(uint32_t id, uint8_t enable)
{
    uint32_t tmpVal;
    uint8_t tmpEn;

    tmpEn = (0 == enable)?0:1;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_CLK_0_OFFSET);

    if (MM_MISC_CLK_GEN_ID_SCALER & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_SCHY, tmpEn);
    if (MM_MISC_CLK_GEN_ID_OSD_DRAW & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_OSD_DRAW, tmpEn);
    if (MM_MISC_CLK_GEN_ID_OSD_BLEND & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_OSD_BLEND, tmpEn);
    if (MM_MISC_CLK_GEN_ID_ISP & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_ISP, tmpEn);
    if (MM_MISC_CLK_GEN_ID_JPG & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_JPG, tmpEn);
    if (MM_MISC_CLK_GEN_ID_DVP2AXI_A & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_D2XA, tmpEn);
    if (MM_MISC_CLK_GEN_ID_DVP2AXI_B & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_D2XB, tmpEn);
    if (MM_MISC_CLK_GEN_ID_DVP2AXI_C & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_D2XC, tmpEn);
    if (MM_MISC_CLK_GEN_ID_DVP_TSRC & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CGEN_DTSRC, tmpEn);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_CLK_0_OFFSET);
}

void MM_MISC_Set_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL src, MM_MISC_DVP2AXI_PORT port)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);
    if (MM_MISC_JPG_SRC_SEL_Y == src)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_MJ_Y_SEL_D2X, port);
    else
        tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_MJ_C_SEL_D2X, port);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);
}

void MM_MISC_Get_JPG_Src_Sel(MM_MISC_JPG_SRC_SEL src, MM_MISC_DVP2AXI_PORT *port)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_D2X_MUX_REG0_OFFSET);
    if (MM_MISC_JPG_SRC_SEL_Y == src)
        *port = reg_get_bits(tmpVal, MM_MISC_CR_MJ_Y_SEL_D2X);
    else
        *port = reg_get_bits(tmpVal, MM_MISC_CR_MJ_C_SEL_D2X);
}

void MM_MISC_Set_IPROAIPRO_Rcmd_Mode(MM_MISC_IPROAIPRO_MODE mode)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_IPROAI_LMTR_RD_OFFSET);

    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_IPROAI_PRO_RCMD_MODE, mode);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_IPROAI_LMTR_RD_OFFSET);
}

void MM_MISC_Set_IPROAIPRO_Rcmd_MasterEn(uint8_t dvp2axi_a, uint8_t dvp2axi_b, uint8_t dvp2axi_c)
{
    uint32_t tmpVal;
    uint32_t en = 0;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_IPROAI_LMTR_RD_OFFSET);
    if (dvp2axi_a)
        en |= 0x1;
    if (dvp2axi_b)
        en |= 0x2;
    if (dvp2axi_c)
        en |= 0x4;

    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_IPROAI_PRO_RD_MSTR_EN, en);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_IPROAI_LMTR_RD_OFFSET);
}

void MM_MISC_Set_IPROAIPRO_Wcmd_Mode(MM_MISC_IPROAIPRO_MODE mode)
{
    uint32_t tmpVal;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_IPROAI_LMTR_WR_OFFSET);

    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_IPROAI_PRO_WCMD_MODE, mode);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_IPROAI_LMTR_WR_OFFSET);
}

void MM_MISC_Set_IPROAIPRO_Wcmd_MasterEn(uint8_t dvp2axi_a, uint8_t dvp2axi_b, uint8_t dvp2axi_c)
{
    uint32_t tmpVal;
    uint32_t en = 0;

    tmpVal = readl(MM_MISC_BASE + MM_MISC_IPROAI_LMTR_WR_OFFSET);
    if (dvp2axi_a)
        en |= 0x1;
    if (dvp2axi_b)
        en |= 0x2;
    if (dvp2axi_c)
        en |= 0x4;

    tmpVal = reg_set_bits(tmpVal, MM_MISC_REG_IPROAI_PRO_WR_MSTR_EN, en);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_IPROAI_LMTR_WR_OFFSET);
}


