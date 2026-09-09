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



#include "drv_dac.h"

#define GPIP_CLK_SET_DUMMY_WAIT \
    {                           \
        __NOP();                \
        __NOP();                \
        __NOP();                \
        __NOP();                \
        __NOP();                \
        __NOP();                \
        __NOP();                \
        __NOP();                \
    }

void GLB_DAC_Init(GLB_DAC_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set DAC config */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_REF_SEL, cfg->refSel);

    if (ENABLE == cfg->resetChanA) {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPDACA_RSTN_ANA);
        tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    if (ENABLE == cfg->resetChanB) {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPDACB_RSTN_ANA);
        tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    /* Clear reset */
    tmpVal = reg_set_bit(tmpVal, GLB_GPDACA_RSTN_ANA);
    tmpVal = reg_set_bit(tmpVal, GLB_GPDACB_RSTN_ANA);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);
}

void GLB_DAC_Set_ChanA_Config(GLB_DAC_Chan_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set channel A config */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_A_OUTMUX, cfg->outMux);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_IOA_EN, cfg->outputEn);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_A_EN, cfg->chanEn);

    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG1_OFFSET);
}

void GLB_DAC_Set_ChanB_Config(GLB_DAC_Chan_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* Check the parameters */

    /* Set channel A config */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_B_OUTMUX, cfg->outMux);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_IOB_EN, cfg->outputEn);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_B_EN, cfg->chanEn);

    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG2_OFFSET);
}

void GPIP_Set_DAC_ChanB_SRC_SEL(GPIP_DAC_ChanB_SRC_Type src)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_CH_B_SEL, src);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_Set_DAC_ChanA_SRC_SEL(GPIP_DAC_ChanA_SRC_Type src)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_CH_A_SEL, src);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_DAC_ChanB_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPDAC_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_DAC_ChanB_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GPIP_GPDAC_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_DAC_ChanA_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPDAC_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_DAC_ChanA_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GPIP_GPDAC_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
}

void GPIP_Set_DAC_DMA_TX_FORMAT_SEL(GPIP_DAC_DMA_TX_FORMAT_Type fmt)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_DMA_FORMAT, fmt);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
}

void GPIP_Set_DAC_DMA_TX_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bit(tmpVal, GPIP_GPDAC_DMA_TX_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
}

void GPIP_Set_DAC_DMA_TX_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, GPIP_GPDAC_DMA_TX_EN);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
}

void GPIP_DAC_DMA_WriteData(uint32_t data)
{
    writel(data, GPIP_BASE + GPIP_GPDAC_DMA_WDATA_OFFSET);
}

int GLB_GPIP_DAC_Init(GLB_GPIP_DAC_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* AON Set DAC config */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_REF_SEL, cfg->refSel);

    if (ENABLE == cfg->resetChanA) {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPDACA_RSTN_ANA);
        tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    if (ENABLE == cfg->resetChanB) {
        tmpVal = reg_clr_bit(tmpVal, GLB_GPDACB_RSTN_ANA);
        tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }

    /* AON Clear reset */
    tmpVal = reg_set_bit(tmpVal, GLB_GPDACA_RSTN_ANA);
    tmpVal = reg_set_bit(tmpVal, GLB_GPDACB_RSTN_ANA);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG0_OFFSET);

    /* GPIP Set DAC config */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_MODE, cfg->div);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);

    /* GPIP Set DMA config */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_DMA_TX_EN, cfg->dmaEn);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_DMA_FORMAT, cfg->dmaFmt);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_DMA_CONFIG_OFFSET);

    return SUCCESS;
}

void GLB_GPIP_DAC_Set_ChanA_Config(GLB_GPIP_DAC_ChanA_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* GPIP select source */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_CH_A_SEL, cfg->src);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);

    /* GPIP enable or disable channel */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_EN, cfg->chanEn);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);

    /* AON enable or disable channel */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_IOA_EN, cfg->outputEn);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_A_EN, cfg->chanCovtEn);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG1_OFFSET);
}

void GLB_GPIP_DAC_Set_ChanB_Config(GLB_GPIP_DAC_ChanB_Cfg_Type *cfg)
{
    uint32_t tmpVal;

    /* GPIP select source */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_CH_B_SEL, cfg->src);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);

    /* GPIP enable or disable channel */
    tmpVal = readl(GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GPIP_GPDAC_EN, cfg->chanEn);
    writel(tmpVal, GPIP_BASE + GPIP_GPDAC_CONFIG_OFFSET);

    /* AON enable or disable channel */
    tmpVal = readl(GLB_BASE + GLB_DAC_CFG2_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_IOB_EN, cfg->outputEn);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_B_EN, cfg->chanCovtEn);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG2_OFFSET);
}

void GLB_DAC_Set_ChanA_Value(uint16_t val)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DAC_CFG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_A_DATA, val);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG3_OFFSET);
}

void GLB_DAC_Set_ChanB_Value(uint16_t val)
{
    uint32_t tmpVal;

    tmpVal = readl(GLB_BASE + GLB_DAC_CFG3_OFFSET);
    tmpVal = reg_set_bits(tmpVal, GLB_GPDAC_B_DATA, val);
    tmpVal = writel(tmpVal, GLB_BASE + GLB_DAC_CFG3_OFFSET);
}

