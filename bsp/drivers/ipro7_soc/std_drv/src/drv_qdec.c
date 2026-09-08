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

#include "drv_qdec.h"

static const uint32_t qdecAddr[QDEC_ID_MAX] = { QDEC0_BASE, QDEC1_BASE, QDEC2_BASE };

void QDEC_Init(QDEC_ID_Type qdecId, QDEC_CFG_Type *qdecCfg)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    /* qdec_ctrl */
    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_PERIOD, qdecCfg->sampleCfg.samplePeriod);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_PERIOD, qdecCfg->reportCfg.reportPeriod);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_EN, qdecCfg->ledCfg.ledEn);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_POL, qdecCfg->ledCfg.ledSwap);
    tmpVal = reg_set_bits(tmpVal, QDEC_DEG_EN, qdecCfg->deglitchCfg.deglitchEn);
    tmpVal = reg_set_bits(tmpVal, QDEC_DEG_CNT, qdecCfg->deglitchCfg.deglitchStrength);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);

    /* qdec_ctrl1 */
    tmpVal = readl(QDECx + QDEC0_CTRL1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_MODE, qdecCfg->sampleCfg.sampleMod);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_MODE, qdecCfg->reportCfg.reportMod);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_PERIOD, qdecCfg->ledCfg.ledPeriod);
    tmpVal = reg_set_bits(tmpVal, QDEC_ACC_MODE, qdecCfg->accMod);
    writel(tmpVal, QDECx + QDEC0_CTRL1_OFFSET);
}

/****************************************************************************/ /**
 * @brief  QDEC deinit
 *
 * @param  qdecId: QDEC ID
 *
 * @return None
 *
*******************************************************************************/
void QDEC_DeInit(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_PERIOD, 10);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_PERIOD, 2);
    tmpVal = reg_set_bits(tmpVal, QDEC_DEG_CNT, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_DEG_EN, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_POL, 1);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_EN, 0);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);

    tmpVal = readl(QDECx + QDEC0_CTRL1_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_LED_PERIOD, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_INPUT_SWAP, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_MODE, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_MODE, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_ACC_MODE, 1);
    writel(tmpVal, QDECx + QDEC0_CTRL1_OFFSET);

    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_EN, 1);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);

    tmpVal = readl(QDECx + QDEC0_INT_EN_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_OVERFLOW_EN, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_DBL_RDY_EN, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_RDY_EN, 0);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_RDY_EN, 1);
    writel(tmpVal, QDECx + QDEC0_INT_EN_OFFSET);

    tmpVal = readl(QDECx + QDEC0_INT_CLR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_OVERFLOW_CLR, 1);
    tmpVal = reg_set_bits(tmpVal, QDEC_DBL_RDY_CLR, 1);
    tmpVal = reg_set_bits(tmpVal, QDEC_SPL_RDY_CLR, 1);
    tmpVal = reg_set_bits(tmpVal, QDEC_RPT_RDY_CLR, 1);
    writel(tmpVal, QDECx + QDEC0_INT_STS_OFFSET);

    tmpVal = readl(QDECx + QDEC0_VALUE_OFFSET);

    /* disable qdec */
    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, QDEC_EN, 0);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);
}

void QDEC_Enable(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, QDEC_EN);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);
}

void QDEC_Disable(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_CTRL0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, QDEC_EN);
    writel(tmpVal, QDECx + QDEC0_CTRL0_OFFSET);
}

void QDEC_SetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType, int intMask)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_INT_EN_OFFSET);

    switch (intType) {
        case QDEC_INT_REPORT:
            tmpVal = reg_set_bits(tmpVal, QDEC_RPT_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_SAMPLE:
            tmpVal = reg_set_bits(tmpVal, QDEC_SPL_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_ERROR:
            tmpVal = reg_set_bits(tmpVal, QDEC_DBL_RDY_EN, (intMask ? 0 : 1));
            break;

        case QDEC_INT_OVERFLOW:
            tmpVal = reg_set_bits(tmpVal, QDEC_OVERFLOW_EN, (intMask ? 0 : 1));
            break;

        default:
            break;
    }

    writel(tmpVal, QDECx + QDEC0_INT_EN_OFFSET);
}

int QDEC_GetIntMask(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_INT_EN_OFFSET);

    switch (intType) {
        case QDEC_INT_REPORT:
            return reg_get_bits(tmpVal, QDEC_RPT_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_SAMPLE:
            return reg_get_bits(tmpVal, QDEC_SPL_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_ERROR:
            return reg_get_bits(tmpVal, QDEC_DBL_RDY_EN) ? UNMASK : MASK;

        case QDEC_INT_OVERFLOW:
            return reg_get_bits(tmpVal, QDEC_OVERFLOW_EN) ? UNMASK : MASK;

        default:
            return UNMASK;
    }
}

int QDEC_Get_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_INT_STS_OFFSET);

    switch (intType) {
        case QDEC_INT_REPORT:
            return reg_get_bits(tmpVal, QDEC_RPT_RDY_STS) ? SET : RESET;

        case QDEC_INT_SAMPLE:
            return reg_get_bits(tmpVal, QDEC_SPL_RDY_STS) ? SET : RESET;

        case QDEC_INT_ERROR:
            return reg_get_bits(tmpVal, QDEC_DBL_RDY_STS) ? SET : RESET;

        case QDEC_INT_OVERFLOW:
            return reg_get_bits(tmpVal, QDEC_OVERFLOW_STS) ? SET : RESET;

        default:
            return RESET;
    }
}

void QDEC_Clr_Int_Status(QDEC_ID_Type qdecId, QDEC_INT_Type intType)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_INT_CLR_OFFSET);

    switch (intType) {
        case QDEC_INT_REPORT:
            tmpVal = reg_set_bit(tmpVal, QDEC_RPT_RDY_CLR);
            break;

        case QDEC_INT_SAMPLE:
            tmpVal = reg_set_bit(tmpVal, QDEC_SPL_RDY_CLR);
            break;

        case QDEC_INT_ERROR:
            tmpVal = reg_set_bit(tmpVal, QDEC_DBL_RDY_CLR);
            break;

        case QDEC_INT_OVERFLOW:
            tmpVal = reg_set_bit(tmpVal, QDEC_OVERFLOW_CLR);
            break;

        default:
            break;
    }

    writel(tmpVal, QDECx + QDEC0_INT_CLR_OFFSET);
}

QDEC_DIRECTION_Type QDEC_Get_Sample_Direction(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_VALUE_OFFSET);

    return (QDEC_DIRECTION_Type)reg_get_bits(tmpVal, QDEC_SPL_VAL);
}

uint8_t QDEC_Get_Err_Cnt(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_VALUE_OFFSET);

    return reg_get_bits(tmpVal, QDEC_ACC2_VAL);
}

uint16_t QDEC_Get_Sample_Val(QDEC_ID_Type qdecId)
{
    uint32_t tmpVal = 0;
    uint32_t QDECx = qdecAddr[qdecId];

    tmpVal = readl(QDECx + QDEC0_VALUE_OFFSET);

    return reg_get_bits(tmpVal, QDEC_ACC1_VAL);
}

