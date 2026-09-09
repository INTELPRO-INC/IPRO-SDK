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



#include "drv_pwm.h"
#include "drv_glb.h"

#define PWMx_Get_Reg_Addr(id)  (PWM_BASE + (id)*0x40)
#define PWM_INT_TIMEOUT_COUNT  (160 * 1000)
#define PWM_STOP_TIMEOUT_COUNT (160 * 1000)

#ifndef IPRO_USE_HAL_DRIVER
static intCallback_Type *PWMIntCbfArra[PWMx_ID_MAX][PWM_INT_ALL] = { { NULL } };
#endif

#ifndef IPRO_USE_HAL_DRIVER
static int PWM_IntHandler(IRQn_Type intPeriph);
#endif

#ifndef IPRO_USE_HAL_DRIVER
static int PWM_IntHandler(IRQn_Type intPeriph)
{
    uint32_t tmpVal = 0;
    uint32_t maskVal = 0;
    uint32_t PWMx;
    uint16_t intIndex;
    PWMx_ID_Type id;

    for (id = PWM0_ID; id < PWMx_ID_MAX; id++) {
        /* Get PWMx start register address */
        PWMx = PWMx_Get_Reg_Addr(id);
        tmpVal = readl(PWMx + PWM_MC0_INT_STS_OFFSET);
        maskVal = readl(PWMx + PWM_MC0_INT_MASK_OFFSET);
        for (intIndex = 0; intIndex < PWM_INT_ALL; intIndex++) {
            if (((1 << intIndex) & tmpVal) && (((1 << intIndex) & maskVal) == 0)) {
                if (intIndex == PWM_INT_REPT) {
                    if (reg_get_bits(readl(PWMx + PWM_MC0_CONFIG0_OFFSET), PWM_STOP_ON_REPT) == 1) {
                        PWMx_Disable(id);
                    }
                }
                writel(1 << intIndex, PWMx + PWM_MC0_INT_CLEAR_OFFSET);
                if (PWMIntCbfArra[id][intIndex] != NULL) {
                    PWMIntCbfArra[id][intIndex]();
                }
            }
        }
    }
    return SUCCESS;
}

#endif

int PWMx_Init(PWMx_ID_Type id, PWMx_CFG_Type *cfg)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = PWM_STOP_TIMEOUT_COUNT;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Ungate pwm clock gate */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_PWM);

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    writel(reg_set_bit(tmpVal, PWM_STOP_EN), PWMx + PWM_MC0_CONFIG0_OFFSET);
    while (!reg_is_bit_set(readl(PWMx + PWM_MC0_CONFIG0_OFFSET), PWM_STS_STOP)) {
        timeoutCnt--;
        if (timeoutCnt == 0) {
            return TIMEOUT;
        }
    }
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_REG_CLK_SEL, cfg->clk);
    tmpVal = reg_set_bits(tmpVal, PWM_STOP_MODE, cfg->stopMode);
    tmpVal = reg_set_bits(tmpVal, PWM_EXT_BREAK_PL, cfg->extPol);
    tmpVal = reg_set_bits(tmpVal, PWM_ADC_TRG_SRC, cfg->adcSrc);
    tmpVal = reg_set_bits(tmpVal, PWM_STOP_ON_REPT, cfg->stpRept);
    tmpVal = reg_set_bits(tmpVal, PWM_CLK_DIV, cfg->clkDiv);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);

    /* Config pwm period and rept count */
    tmpVal = readl(PWMx + PWM_MC0_PERIOD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_PERIOD, cfg->period);
    tmpVal = reg_set_bits(tmpVal, PWM_INT_PERIOD_CNT, cfg->intPulseCnt);
    writel(tmpVal, PWMx + PWM_MC0_PERIOD_OFFSET);

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(PWM_IRQn, PWM_IRQHandler);
#endif

    return SUCCESS;
}

void PWMx_Div_Set(PWMx_ID_Type id, uint16_t div)
{
    uint32_t tmpVal;
    //* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_CLK_DIV, div);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWMx_Period_Set(PWMx_ID_Type id, uint16_t period)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm period and duty */
    tmpVal = readl(PWMx + PWM_MC0_PERIOD_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_PERIOD, period);
    writel(tmpVal, PWMx + PWM_MC0_PERIOD_OFFSET);
}

void PWMx_Period_Get(PWMx_ID_Type id, uint16_t *period)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* get pwm period */
    tmpVal = readl(PWMx + PWM_MC0_PERIOD_OFFSET);
    *period = reg_get_bits(tmpVal, PWM_PERIOD);
}

void PWMx_Enable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm clock to enable pwm */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    writel(reg_clr_bit(tmpVal, PWM_STOP_EN), PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWMx_Disable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm clock to disable pwm */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    writel(reg_set_bit(tmpVal, PWM_STOP_EN), PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_Channelx_Init(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_CHx_CFG_Type *cfg)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PEN, cfg->modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NEN, cfg->modN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PPL, cfg->polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NPL, cfg->polN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PSI, cfg->idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NSI, cfg->idlN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PBS, cfg->brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NBS, cfg->brkN);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PEN, cfg->modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NEN, cfg->modN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PPL, cfg->polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NPL, cfg->polN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PSI, cfg->idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NSI, cfg->idlN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PBS, cfg->brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NBS, cfg->brkN);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PEN, cfg->modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NEN, cfg->modN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PPL, cfg->polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NPL, cfg->polN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PSI, cfg->idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NSI, cfg->idlN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PBS, cfg->brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NBS, cfg->brkN);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PEN, cfg->modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NEN, cfg->modN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PPL, cfg->polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NPL, cfg->polN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PSI, cfg->idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NSI, cfg->idlN);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PBS, cfg->brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NBS, cfg->brkN);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);

    /* Config pwm thresholdL and thresholdH */
    switch (ch) {
        case PWM_CH0:
            tmpVal = readl(PWMx + PWM_MC0_CH0_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREL, cfg->thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREH, cfg->thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH0_THRE_OFFSET);
            break;
        case PWM_CH1:
            tmpVal = readl(PWMx + PWM_MC0_CH1_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREL, cfg->thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREH, cfg->thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH1_THRE_OFFSET);
            break;
        case PWM_CH2:
            tmpVal = readl(PWMx + PWM_MC0_CH2_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREL, cfg->thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREH, cfg->thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH2_THRE_OFFSET);
            break;
        case PWM_CH3:
            tmpVal = readl(PWMx + PWM_MC0_CH3_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREL, cfg->thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREH, cfg->thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH3_THRE_OFFSET);
            break;
        default:
            break;
    }

    /* Config pwm dead time */
    tmpVal = readl(PWMx + PWM_MC0_DEAD_TIME_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_DTG, cfg->dtg);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_DTG, cfg->dtg);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_DTG, cfg->dtg);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_DTG, cfg->dtg);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_DEAD_TIME_OFFSET);
}

void PWM_Channelx_Threshold_Set(PWMx_ID_Type id, PWM_CHx_Type ch, uint16_t thresholdL, uint16_t thresholdH)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    switch (ch) {
        case PWM_CH0:
            tmpVal = readl(PWMx + PWM_MC0_CH0_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREL, thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH0_THRE_OFFSET);
            break;
        case PWM_CH1:
            tmpVal = readl(PWMx + PWM_MC0_CH1_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREL, thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH1_THRE_OFFSET);
            break;
        case PWM_CH2:
            tmpVal = readl(PWMx + PWM_MC0_CH2_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREL, thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH2_THRE_OFFSET);
            break;
        case PWM_CH3:
            tmpVal = readl(PWMx + PWM_MC0_CH3_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREL, thresholdL);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH3_THRE_OFFSET);
            break;
        default:
            break;
    }
}

void PWM_Channelx_ThresholdL_Set(PWMx_ID_Type id, PWM_CHx_Type ch, uint16_t thresholdL)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    switch (ch) {
        case PWM_CH0:
            tmpVal = readl(PWMx + PWM_MC0_CH0_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREL, thresholdL);
            writel(tmpVal, PWMx + PWM_MC0_CH0_THRE_OFFSET);
            break;
        case PWM_CH1:
            tmpVal = readl(PWMx + PWM_MC0_CH1_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREL, thresholdL);
            writel(tmpVal, PWMx + PWM_MC0_CH1_THRE_OFFSET);
            break;
        case PWM_CH2:
            tmpVal = readl(PWMx + PWM_MC0_CH2_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREL, thresholdL);
            writel(tmpVal, PWMx + PWM_MC0_CH2_THRE_OFFSET);
            break;
        case PWM_CH3:
            tmpVal = readl(PWMx + PWM_MC0_CH3_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREL, thresholdL);
            writel(tmpVal, PWMx + PWM_MC0_CH3_THRE_OFFSET);
            break;
        default:
            break;
    }
}

void PWM_Channelx_ThresholdH_Set(PWMx_ID_Type id, PWM_CHx_Type ch, uint16_t thresholdH)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    switch (ch) {
        case PWM_CH0:
            tmpVal = readl(PWMx + PWM_MC0_CH0_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH0_THRE_OFFSET);
            break;
        case PWM_CH1:
            tmpVal = readl(PWMx + PWM_MC0_CH1_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH1_THRE_OFFSET);
            break;
        case PWM_CH2:
            tmpVal = readl(PWMx + PWM_MC0_CH2_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH2_THRE_OFFSET);
            break;
        case PWM_CH3:
            tmpVal = readl(PWMx + PWM_MC0_CH3_THRE_OFFSET);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_THREH, thresholdH);
            writel(tmpVal, PWMx + PWM_MC0_CH3_THRE_OFFSET);
            break;
        default:
            break;
    }
}

void PWM_Channelx_Threshold_Get(PWMx_ID_Type id, PWM_CHx_Type ch, uint16_t *thresholdL, uint16_t *thresholdH)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* get pwm thresholdL and thresholdH */
    switch (ch) {
        case PWM_CH0:
            tmpVal = readl(PWMx + PWM_MC0_CH0_THRE_OFFSET);
            *thresholdL = reg_get_bits(tmpVal, PWM_CH0_THREL);
            *thresholdH = reg_get_bits(tmpVal, PWM_CH0_THREH);
            break;
        case PWM_CH1:
            tmpVal = readl(PWMx + PWM_MC0_CH1_THRE_OFFSET);
            *thresholdL = reg_get_bits(tmpVal, PWM_CH1_THREL);
            *thresholdH = reg_get_bits(tmpVal, PWM_CH1_THREH);
            break;
        case PWM_CH2:
            tmpVal = readl(PWMx + PWM_MC0_CH2_THRE_OFFSET);
            *thresholdL = reg_get_bits(tmpVal, PWM_CH2_THREL);
            *thresholdH = reg_get_bits(tmpVal, PWM_CH2_THREH);
            break;
        case PWM_CH3:
            tmpVal = readl(PWMx + PWM_MC0_CH3_THRE_OFFSET);
            *thresholdL = reg_get_bits(tmpVal, PWM_CH3_THREL);
            *thresholdH = reg_get_bits(tmpVal, PWM_CH3_THREH);
            break;
        default:
            break;
    }
}

void PWM_Channelx_Pwm_Mode_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Mode_Type modP, PWM_Mode_Type modN)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PEN, modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NEN, modN);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PEN, modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NEN, modN);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PEN, modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NEN, modN);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PEN, modP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NEN, modN);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Positive_Pwm_Mode_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Mode_Type mod)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PEN, mod);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PEN, mod);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PEN, mod);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PEN, mod);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Negative_Pwm_Mode_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Mode_Type mod)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NEN, mod);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NEN, mod);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NEN, mod);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NEN, mod);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Polarity_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Polarity_Type polP, PWM_Polarity_Type polN)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PPL, polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NPL, polN);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PPL, polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NPL, polN);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PPL, polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NPL, polN);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PPL, polP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NPL, polN);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Positive_Polarity_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Polarity_Type pol)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PPL, pol);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PPL, pol);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PPL, pol);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PPL, pol);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Negative_Polarity_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Polarity_Type pol)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NPL, pol);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NPL, pol);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NPL, pol);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NPL, pol);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Idle_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Idle_State_Type idlP, PWM_Idle_State_Type idlN)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PSI, idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NSI, idlN);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PSI, idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NSI, idlN);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PSI, idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NSI, idlN);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PSI, idlP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NSI, idlN);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Positive_Idle_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Idle_State_Type idl)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PSI, idl);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PSI, idl);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PSI, idl);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PSI, idl);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Negative_Idle_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Idle_State_Type idl)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NSI, idl);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NSI, idl);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NSI, idl);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NSI, idl);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Break_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Break_State_Type brkP, PWM_Break_State_Type brkN)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PBS, brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NBS, brkN);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PBS, brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NBS, brkN);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PBS, brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NBS, brkN);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PBS, brkP);
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NBS, brkN);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Positive_Break_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Break_State_Type brk)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_PBS, brk);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_PBS, brk);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_PBS, brk);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_PBS, brk);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Negative_Break_State_Set(PWMx_ID_Type id, PWM_CHx_Type ch, PWM_Break_State_Type brk)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_NBS, brk);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_NBS, brk);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_NBS, brk);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_NBS, brk);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Config1_Set(PWMx_ID_Type id, uint32_t cfg)
{
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    writel(cfg, PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Config1_Get(PWMx_ID_Type id, uint32_t *cfg)
{
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Get pwm config1 value */
    *cfg = readl(PWMx + PWM_MC0_CONFIG1_OFFSET);
}

void PWM_Channelx_Dtg_Set(PWMx_ID_Type id, PWM_CHx_Type ch, uint8_t dtg)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config1 */
    tmpVal = readl(PWMx + PWM_MC0_DEAD_TIME_OFFSET);
    switch (ch) {
        case PWM_CH0:
            tmpVal = reg_set_bits(tmpVal, PWM_CH0_DTG, dtg);
            break;
        case PWM_CH1:
            tmpVal = reg_set_bits(tmpVal, PWM_CH1_DTG, dtg);
            break;
        case PWM_CH2:
            tmpVal = reg_set_bits(tmpVal, PWM_CH2_DTG, dtg);
            break;
        case PWM_CH3:
            tmpVal = reg_set_bits(tmpVal, PWM_CH3_DTG, dtg);
            break;
        default:
            break;
    }
    writel(tmpVal, PWMx + PWM_MC0_DEAD_TIME_OFFSET);
}

void PWM_SW_Break_Enable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PWM_SW_BREAK_EN);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_SW_Break_Disable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PWM_SW_BREAK_EN);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_EXT_Break_Enable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bit(tmpVal, PWM_EXT_BREAK_EN);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_EXT_Break_Disable(PWMx_ID_Type id)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, PWM_EXT_BREAK_EN);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_EXT_Break_Polarity_Set(PWMx_ID_Type id, PWM_EXT_Break_Polarity_Type pol)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_EXT_BREAK_PL, pol);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_TrigADC_Source_Set(PWMx_ID_Type id, PWM_TrigADC_Source_Type src)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    /* Config pwm config0 */
    tmpVal = readl(PWMx + PWM_MC0_CONFIG0_OFFSET);
    tmpVal = reg_set_bits(tmpVal, PWM_ADC_TRG_SRC, src);
    writel(tmpVal, PWMx + PWM_MC0_CONFIG0_OFFSET);
}

void PWM_Int_Mask(PWMx_ID_Type id, PWM_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    tmpVal = readl(PWMx + PWM_MC0_INT_MASK_OFFSET);
    if (intType == PWM_INT_ALL) {
        if (intMask == UNMASK) {
            tmpVal &= ~((1U << PWM_INT_ALL) - 1);
        } else {
            tmpVal |= ((1U << PWM_INT_ALL) - 1);
        }
    } else {
        if (intMask == UNMASK) {
            tmpVal &= ~(1U << intType);
        } else {
            tmpVal |= (1U << intType);
        }
    }
    writel(tmpVal, PWMx + PWM_MC0_INT_MASK_OFFSET);
}

void PWM_Int_Clear(PWMx_ID_Type id, PWM_INT_Type intType)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    tmpVal = readl(PWMx + PWM_MC0_INT_CLEAR_OFFSET);

    if (intType == PWM_INT_ALL) {
        tmpVal = ((1U << PWM_INT_ALL) - 1);
    } else {
        tmpVal = (1U << intType);
    }
    writel(tmpVal, PWMx + PWM_MC0_INT_CLEAR_OFFSET);
}

int PWM_Int_Status_Get(PWMx_ID_Type id, PWM_INT_Type intType)
{
    uint32_t tmpVal;
    /* Get PWMx start register address */
    uint32_t PWMx = PWMx_Get_Reg_Addr(id);

    /* Check the parameters */

    tmpVal = readl(PWMx + PWM_MC0_INT_STS_OFFSET);

    if (intType == PWM_INT_ALL) {
        if (tmpVal & ((1 << PWM_CHx_MAX) - 1)) {
            return SET;
        } else {
            return RESET;
        }
    } else {
        if (tmpVal & (1 << intType)) {
            return SET;
        } else {
            return RESET;
        }
    }
}

#ifndef IPRO_USE_HAL_DRIVER
void PWM_Int_Callback_Install(PWMx_ID_Type id, uint32_t intType, intCallback_Type *cbFun)
{
    PWMIntCbfArra[id][intType] = cbFun;
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void PWM_IRQHandler(void)
{
    PWM_IntHandler(PWM_IRQn);
}
#endif

