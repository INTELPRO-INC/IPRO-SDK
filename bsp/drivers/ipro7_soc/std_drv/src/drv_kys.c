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

#include "drv_kys.h"

int KYS_Init(KYS_CFG_Type *kysCfg)
{
    uint32_t tmpVal;

    tmpVal = readl(KYS_BASE + KYS_KS_CTRL_OFFSET);
    /* Set col and row */
    tmpVal = reg_set_bits(tmpVal, KYS_COL_NUM, kysCfg->col - 1);
    tmpVal = reg_set_bits(tmpVal, KYS_ROW_NUM, kysCfg->row - 1);

    /* Set idle duration between column scans */
    tmpVal = reg_set_bits(tmpVal, KYS_RC_EXT, kysCfg->idleDuration);

    /* Enable or disable ghost key event detection */
    tmpVal = reg_set_bits(tmpVal, KYS_GHOST_EN, kysCfg->ghostEn);

    /* Enable or disable deglitch function */
    tmpVal = reg_set_bits(tmpVal, KYS_DEG_EN, kysCfg->deglitchEn);

    /* Set deglitch count */
    tmpVal = reg_set_bits(tmpVal, KYS_DEG_CNT, kysCfg->deglitchCnt);

    /* Write back */
    writel(tmpVal, KYS_BASE + KYS_KS_CTRL_OFFSET);

    return SUCCESS;
}

int KYS_Enable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(KYS_BASE + KYS_KS_CTRL_OFFSET);
    writel(reg_set_bit(tmpVal, KYS_KS_EN), KYS_BASE + KYS_KS_CTRL_OFFSET);

    return SUCCESS;
}

int KYS_Disable(void)
{
    uint32_t tmpVal;

    tmpVal = readl(KYS_BASE + KYS_KS_CTRL_OFFSET);
    writel(reg_clr_bit(tmpVal, KYS_KS_EN), KYS_BASE + KYS_KS_CTRL_OFFSET);

    return SUCCESS;
}

int KYS_IntMask(int intMask)
{
    if (MASK == intMask) {
        writel(0, KYS_BASE + KYS_KS_INT_EN_OFFSET);
    } else {
        writel(1, KYS_BASE + KYS_KS_INT_EN_OFFSET);
    }

    return SUCCESS;
}

int KYS_IntClear(void)
{
    writel(0xf, KYS_BASE + KYS_KEYCODE_CLR_OFFSET);

    return SUCCESS;
}

uint8_t KYS_GetIntStatus(void)
{
    return readl(KYS_BASE + KYS_KS_INT_STS_OFFSET) & 0xf;
}

uint8_t KYS_GetKeycode(KYS_Keycode_Type keycode, uint8_t *col, uint8_t *row)
{
    uint32_t tmpVal;
    uint8_t keyValue;

    /* Get keycode value */
    keyValue = readl(KYS_BASE + KYS_KEYCODE_VALUE_OFFSET);
    if (keycode < KYS_KEYCODE_ALL)
        keyValue = (keyValue >> (8 * keycode)) & 0xff;

    /* Get total row number of keyboard */
    tmpVal = readl(KYS_BASE + KYS_KS_CTRL_OFFSET);
    tmpVal = reg_get_bits(tmpVal, KYS_ROW_NUM);

    /* Calculate col and row of the key */
    if (col)
        *col = keyValue / (tmpVal + 1);
    if (row)
        *row = keyValue % (tmpVal + 1);

    return keyValue;
}

