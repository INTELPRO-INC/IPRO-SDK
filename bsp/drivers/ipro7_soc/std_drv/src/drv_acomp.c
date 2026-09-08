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

#include "drv_acomp.h"

void AON_ACOMP_Init(AON_ACOMP_ID_Type acompNo, AON_ACOMP_CFG_Type *cfg)
{
    uint32_t tmpVal = 0;

    if (acompNo == AON_ACOMP0_ID) {
        /* Disable ACOMP first */
        tmpVal = readl(AON_BASE + AON_ACOMP0_CTRL_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AON_ACOMP0_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP0_CTRL_OFFSET);

        /* Set ACOMP config */
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_MUXEN, cfg->muxEn);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_POS_SEL, cfg->posChanSel);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_NEG_SEL, cfg->negChanSel);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_LEVEL_SEL, cfg->levelFactor);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_BIAS_PROG, cfg->biasProg);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_HYST_SELP, cfg->hysteresisPosVolt);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP0_HYST_SELN, cfg->hysteresisNegVolt);

        writel(tmpVal, AON_BASE + AON_ACOMP0_CTRL_OFFSET);
    } else {
        /* Disable ACOMP first */
        tmpVal = readl(AON_BASE + AON_ACOMP1_CTRL_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AON_ACOMP1_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP1_CTRL_OFFSET);

        /* Set ACOMP config */
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_MUXEN, cfg->muxEn);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_POS_SEL, cfg->posChanSel);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_NEG_SEL, cfg->negChanSel);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_LEVEL_SEL, cfg->levelFactor);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_BIAS_PROG, cfg->biasProg);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_HYST_SELP, cfg->hysteresisPosVolt);
        tmpVal = reg_set_bits(tmpVal, AON_ACOMP1_HYST_SELN, cfg->hysteresisNegVolt);

        writel(tmpVal, AON_BASE + AON_ACOMP1_CTRL_OFFSET);
    }
}

void AON_ACOMP_Enable(AON_ACOMP_ID_Type acompNo)
{
    uint32_t tmpVal = 0;

    if (acompNo == AON_ACOMP0_ID) {
        tmpVal = readl(AON_BASE + AON_ACOMP0_CTRL_OFFSET);
        tmpVal = reg_set_bit(tmpVal, AON_ACOMP0_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP0_CTRL_OFFSET);
    } else {
        tmpVal = readl(AON_BASE + AON_ACOMP1_CTRL_OFFSET);
        tmpVal = reg_set_bit(tmpVal, AON_ACOMP1_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP1_CTRL_OFFSET);
    }
}

void AON_ACOMP_Disable(AON_ACOMP_ID_Type acompNo)
{
    uint32_t tmpVal = 0;

    if (acompNo == AON_ACOMP0_ID) {
        tmpVal = readl(AON_BASE + AON_ACOMP0_CTRL_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AON_ACOMP0_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP0_CTRL_OFFSET);
    } else {
        tmpVal = readl(AON_BASE + AON_ACOMP1_CTRL_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AON_ACOMP1_EN);
        writel(tmpVal, AON_BASE + AON_ACOMP1_CTRL_OFFSET);
    }
}


int AON_ACOMP_Get_Result(AON_ACOMP_ID_Type acompNo)
{
    uint32_t tmpVal = 0;

    tmpVal = readl(AON_BASE + AON_ACOMP_CTRL_OFFSET);

    /* Disable ACOMP first */
    if (acompNo == AON_ACOMP0_ID) {
        return reg_is_bit_set(tmpVal, AON_ACOMP0_OUT_RAW);
    } else {
        return reg_is_bit_set(tmpVal, AON_ACOMP1_OUT_RAW);

    }
}

void AON_ACOMP_Interrupt_Clear(uint8_t idx)
{
    uint32_t tmpVal;

    if (idx == 0) {
        /* set clear bit */
        tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
        tmpVal |= (1 << 20);
        writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

        /* unset clear bit */
        tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
        tmpVal &= (~(1 << 20));
        writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    } else {
        /* set clear bit */
        tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
        tmpVal |= (1 << 22);
        writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);

        /* unset clear bit */
        tmpVal = readl(HBN_BASE + HBN_IRQ_CLR_OFFSET);
        tmpVal &= (~(1 << 22));
        writel(tmpVal, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    }

}

void AON_ACOMP_Interrupt_Mask(uint8_t idx, uint8_t neg_edge)
{
    uint32_t tmpVal;
    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    if (idx == 0) {
        tmpVal &= ~(neg_edge << 20);
    } else {
        tmpVal &= ~(neg_edge << 22);
    }
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
}

void AON_ACOMP_Interrupt_UnMask(uint8_t idx, uint8_t neg_edge)
{
    uint32_t tmpVal;

    AON_ACOMP_Interrupt_Clear(idx);
    tmpVal = readl(HBN_BASE + HBN_IRQ_MODE_OFFSET);
    if (idx == 0) {
        tmpVal |= (neg_edge << 20);
    } else {
        tmpVal |= (neg_edge << 22);
    }
    writel(tmpVal, HBN_BASE + HBN_IRQ_MODE_OFFSET);
}

