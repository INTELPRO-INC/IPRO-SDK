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
    status->spi2dvp_sync_int = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_SYNC_INT);
    status->spi2dvp_fs_int   = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_FS_INT);
    status->spi2dvp_fe_int   = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_FE_INT);
    status->spi2dvp_ls_int   = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_LS_INT);
    status->spi2dvp_fifo_int = reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_FIFO_INT);
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
        if (reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_FS_INT) && reg_get_bits(tmpVal, MM_MISC_ST_SPI2DVP_FE_INT))
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
    tmpVal = reg_set_bits(tmpVal, MM_MISC_CR_DVP_AS_FIFO_TH, dvpAsCfg->fifoThreshold);
    writel(tmpVal, MM_MISC_BASE + MM_MISC_DVP_AS_REG0_OFFSET);
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
    if (MM_MISC_SW_RESET_ID_JPG & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_JPG, tmpRst);
    if (MM_MISC_SW_RESET_ID_DVP_AS & id)
        tmpVal = reg_set_bits(tmpVal, MM_MISC_SWRST_DVP_AS, tmpRst);

    writel(tmpVal, MM_MISC_BASE + MM_MISC_RST_0_OFFSET);
}
