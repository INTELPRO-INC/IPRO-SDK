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

#include <dvp_tsrc_lite_reg.h>
#include <drv_tsrc.h>

#define TOTAL_W_MAX_VAL (65534) // make it even

/****************************************************************************//**
 * @brief  TSRC init
 *
 * @param  cfg: TSRC configuration
 *
 * @return SUCCESS
 *
*******************************************************************************/

uint32_t TSRC_Init(TSRC_Cfg_Type *cfg)
{
    uint32_t tmpVal;
    uint32_t bytesPerLine, hTotalCnt, vTotalCnt;

    /* Check the parameters */
    CHECK_PARAM(IS_TSRC_PIXEL_FORMAT_TYPE(cfg->format));
    CHECK_PARAM(IS_TSRC_BURST_TYPE(cfg->burst));
    CHECK_PARAM(cfg->vBlank > 4);

    /* Enable using AXI data */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
    // assume use dvp mode first, if needs to run cea mode, call extra TSRC_CEA861_Init API later
    tmpVal = reg_clr_bit(tmpVal,DVP_TSRC_LITE_CR_MODE_CEA_861);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_DVP_DATA_MODE,cfg->format);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_B0_SEL,cfg->byte0);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_B1_SEL,cfg->byte1);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_B2_SEL,cfg->byte2);
    // assume use axi mode first, if needs to run fake mode, call extra TSRC_Fake_Data_Init API later
    tmpVal = reg_set_bit(tmpVal,DVP_TSRC_LITE_CR_AXI_EN);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_PUSH_MODE,cfg->pushMode);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);

    /* Set burst length */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_SETTING_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_AXI_XLEN,cfg->burst);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_SETTING_OFFSET);

    /* Set horizontal pixel count */
    hTotalCnt = cfg->imgSize.width + cfg->imgSize.hBlank;
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_FRAME_SIZE_H_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_TOTAL_H,hTotalCnt);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_BLANK_H,cfg->imgSize.hBlank);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_FRAME_SIZE_H_OFFSET);

    /* Set vertical pixel count */
    vTotalCnt = cfg->imgSize.height + cfg->imgSize.vBlank;
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_FRAME_SIZE_V_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_TOTAL_V,vTotalCnt);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_BLANK_V,cfg->imgSize.vBlank);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_FRAME_SIZE_V_OFFSET);

    switch(cfg->format)
    {
        case TSRC_PIXEL_YUV422_16BIT:             /*!< YUV422 16-bit mode */
        case TSRC_PIXEL_RGB565_16TO24BIT:         /*!< RGB565 16-bit extend 24-bit mode */
            bytesPerLine = cfg->imgSize.width << 1;
        break;
        case TSRC_PIXEL_RGBA8888_32TO24BIT:       /*!< RGBA8888 32-bit compress 24-bit mode */
            bytesPerLine = cfg->imgSize.width << 2;
        break;
        case TSRC_PIXEL_RGB888_24BIT:             /*!< RGB888 24-bit mode */
            bytesPerLine = cfg->imgSize.width * 3;
        break;
        case TSRC_PIXEL_YUV400_8BIT:              /*!< YUV400 8-bit mode */
            bytesPerLine = cfg->imgSize.width;
        break;
        default:
            bytesPerLine = cfg->imgSize.width << 1;
        break;
    }

    CHECK_PARAM(IS_MULTIPULE_OF_8(bytesPerLine));

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_FRAME_INFO1_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_FRAME_WIDTH_X8,bytesPerLine >> 3);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_FRAME_HEIGHT,cfg->imgSize.height);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_FRAME_INFO1_OFFSET);

    /* Set vertical prefetch start position */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_PREFETCH_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_PREFETCH_V,cfg->prefetch);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_PREFETCH_OFFSET);

    /* Set start address */
    writel(cfg->memStart, DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_START_ADDR_BY_OFFSET);

    return SUCCESS;
}

/****************************************************************************//**
 * @brief  TSRC fake data configuration
 *
 * @param  cfg: TSRC fake data configuration
 *
 * @return None
 *
*******************************************************************************/

void TSRC_Fake_Data_Init(TSRC_Fake_Data_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* Enable using fake gradient data */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
    writel(reg_clr_bit(tmpVal,DVP_TSRC_LITE_CR_AXI_EN), DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);

    /* Set min and max value of fake gradient data */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_PIX_DATA_RANGE_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_DATA_MIN,cfg->minData);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_DATA_MAX,cfg->maxData);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_PIX_DATA_RANGE_OFFSET);

    /* Set step value for eack line of fake gradient data */
    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_PIX_DATA_STEP_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_DATA_STEP,cfg->step);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_PIX_DATA_STEP_OFFSET);
}

// void TSRC_CEA861_Init(TSRC_CEA861_Cfg_Type *cfg)
// {

// }

/****************************************************************************//**
 * @brief  TSRC enable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/

void TSRC_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
    writel(reg_set_bit(tmpVal,DVP_TSRC_LITE_CR_ENABLE), DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
}

/****************************************************************************//**
 * @brief  TSRC disable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/

void TSRC_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
    writel(reg_clr_bit(tmpVal,DVP_TSRC_LITE_CR_ENABLE), DTSRCT_BASE + DVP_TSRC_LITE_CONFIG_OFFSET);
}

/****************************************************************************//**
 * @brief  TSRC clear drain error
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void TSRC_Clear_Drain_Error(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI_PUSH_MODE_OFFSET);
    writel(reg_set_bit(tmpVal,DVP_TSRC_LITE_CR_AXI_DRAIN_ERR_CLR), DTSRCT_BASE + DVP_TSRC_LITE_AXI_PUSH_MODE_OFFSET);
}

// void TSRC_Set_Frame_Size(TSRC_Frame_Size_Cfg_Type *cfg)
// {

// }

// void TSRC_Mem_Cfg(uint32_t memAddr)
// {

// }

// uint32_t TSRC_Check_Status(void)
// {

// }

void TSRC_Push_Mode_Trigger(void)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI_PUSH_MODE_OFFSET);

    tmpVal = reg_set_bit(tmpVal, DVP_TSRC_LITE_CR_AXI_FRAME_PUSH);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_AXI_PUSH_MODE_OFFSET);
}

void TSRC_Set_Stride_Width(uint32_t bytesPerLine)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_MULTIPULE_OF_8(bytesPerLine));

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_FRAME_INFO2_OFFSET);
    tmpVal = reg_set_bits(tmpVal,DVP_TSRC_LITE_CR_STRIDE_WIDTH_X8,bytesPerLine>>3);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_AXI2DVP_FRAME_INFO2_OFFSET);
}

void TSRC_Set_TG_State(uint8_t state)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG0_OFFSET);
    if (ENABLE == state) {
        tmpVal = reg_set_bit(tmpVal, DVP_TSRC_LITE_CR_TG_EN);
    } else {
        tmpVal = reg_clr_bit(tmpVal, DVP_TSRC_LITE_CR_TG_EN);
    }
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG0_OFFSET);
}

void TSRC_Set_TG_Cfg(TSRC_TG_Cfg *cfg)
{
    uint32_t tmpVal;

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_TSRC_LITE_CR_TOTAL_HPIXELS, cfg->totalHPixels);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG0_OFFSET);

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_TSRC_LITE_CR_ACT_PIXEL_WIDTH, cfg->actPixelWidth);
    tmpVal = reg_set_bits(tmpVal, DVP_TSRC_LITE_CR_ACT_START_PIXEL, cfg->actStartPixel);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG1_OFFSET);

    tmpVal = readl(DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, DVP_TSRC_LITE_CR_ACT_LINE_HEIGHT, cfg->actLineHeight);
    tmpVal = reg_set_bits(tmpVal, DVP_TSRC_LITE_CR_ACT_START_LINE, cfg->actStartLine);
    writel(tmpVal, DTSRCT_BASE + DVP_TSRC_LITE_DTSRC_POST_TG2_OFFSET);
}

// void TSRC_Set_Pix_Format(TSRC_Pixel_Format_Type type)
// {

// }
