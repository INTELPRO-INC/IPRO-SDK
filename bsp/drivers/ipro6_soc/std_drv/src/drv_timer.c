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



#include "drv_timer.h"
#include "drv_glb.h"

#define TIMER_MAX_MATCH 3

static const uint32_t timerAddr[TIMER_ID_MAX] = { TIMER_BASE };
#ifndef IPRO_USE_HAL_DRIVER
intCallback_Type *timerIntCbfArra[TIMER_ID_MAX][TIMER_CH_MAX][TIMER_INT_ALL] = { NULL };
intCallback_Type *wdtIntCbfArra[WDT_ID_MAX][WDT_INT_ALL] = { NULL };
#endif

#ifndef IPRO_USE_HAL_DRIVER
static void TIMER_IntHandler(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
#endif

#ifndef IPRO_USE_HAL_DRIVER
void TIMER_IntHandler(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t intId;
    uint32_t tmpVal;
    uint32_t tmpAddr;
    uint32_t TIMERx = timerAddr[timerId];

    intId = readl(TIMERx + TIMER_TSR2_OFFSET + 4 * timerCh);
    tmpAddr = TIMERx + TIMER_TICR2_OFFSET + 4 * timerCh;
    tmpVal = readl(tmpAddr);

    /* Comparator 0 match interrupt */
    if (reg_is_bit_set(intId, TIMER_TSR2_0)) {
        writel(reg_set_bit(tmpVal, TIMER_TSR2_0), tmpAddr);

        if (timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_0] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_0]();
        }
    }

    /* Comparator 1 match interrupt */
    if (reg_is_bit_set(intId, TIMER_TSR2_1)) {
        writel(reg_set_bit(tmpVal, TIMER_TSR2_1), tmpAddr);

        if (timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_1] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_1]();
        }
    }

    /* Comparator 2 match interrupt */
    if (reg_is_bit_set(intId, TIMER_TSR2_2)) {
        writel(reg_set_bit(tmpVal, TIMER_TSR2_2), tmpAddr);

        if (timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_2] != NULL) {
            /* Call the callback function */
            timerIntCbfArra[timerId][timerCh][TIMER_INT_COMP_2]();
        }
    }
}
#endif

uint32_t TIMER_GetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo));
    return tmpVal;
}

void TIMER_SetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val)
{
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    writel(val, TIMERx + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo));
}

void TIMER_CompValueEffectImmediately(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, int enable)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TCMR_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER2_ALIGN, 1 - enable);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER3_ALIGN, 1 - enable);
    }

    writel(tmpVal, TIMERx + TIMER_TCMR_OFFSET);
}

uint32_t TIMER_GetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t tmpAddr;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpAddr = TIMERx + TIMER_TCR2_OFFSET + 4 * timerCh;

    tmpVal = readl(tmpAddr);

    return tmpVal;
}

void TIMER_ResetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TCER_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bit(tmpVal, TIMER_TCR2_CNT_CLR);
        writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, TIMER_TCR2_CNT_CLR);
        writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
    } else {
        tmpVal = reg_set_bit(tmpVal, TIMER_TCR3_CNT_CLR);
        writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, TIMER_TCR3_CNT_CLR);
        writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
    }
}

int TIMER_GetMatchStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpVal;
    int bitStatus = RESET;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TSR2_OFFSET + 4 * timerCh);

    switch (cmpNo) {
        case TIMER_COMP_ID_0:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR3_0) ? SET : RESET;
            break;

        case TIMER_COMP_ID_1:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR3_1) ? SET : RESET;
            break;

        case TIMER_COMP_ID_2:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR3_2) ? SET : RESET;
            break;

        default:
            break;
    }

    return bitStatus;
}

uint32_t TIMER_GetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    return readl(TIMERx + TIMER_TPLVR2_OFFSET + 4 * timerCh);
}

void TIMER_SetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint32_t val)
{
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    writel(val, TIMERx + TIMER_TPLVR2_OFFSET + 4 * timerCh);
}

void TIMER_SetPreloadTrigSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc)
{
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    writel(plSrc, TIMERx + TIMER_TPLCR2_OFFSET + 4 * timerCh);
}

/* Alias for HAL layer compatibility */
void TIMER_SetPreloadSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc)
{
    TIMER_SetPreloadTrigSrc(timerId, timerCh, plSrc);
}

void TIMER_SetCountMode(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode)
{
    uint32_t tmpval;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpval = readl(TIMERx + TIMER_TCMR_OFFSET);
    tmpval &= (~(1 << (timerCh + 1)));
    tmpval |= (countMode << (timerCh + 1));

    writel(tmpval, TIMERx + TIMER_TCMR_OFFSET);
}

void TIMER_ClearIntStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpAddr = TIMERx + TIMER_TICR2_OFFSET + 4 * timerCh;

    tmpVal = readl(tmpAddr);
    tmpVal |= (1 << cmpNo);

    writel(tmpVal, tmpAddr);
}

void TIMER_SetDiv(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint8_t div)
{
}

int TIMER_Init(TIMER_ID_Type timerId, TIMER_CFG_Type *timerCfg)
{
    TIMER_Chan_Type timerCh = timerCfg->timerCh;
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_TIMER);

    /* Configure timer clock source */
    tmpVal = readl(TIMERx + TIMER_TCCR_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_2, timerCfg->clkSrc);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_3, timerCfg->clkSrc);
    }

    writel(tmpVal, TIMERx + TIMER_TCCR_OFFSET);

    /* Configure timer clock division */
    tmpVal = readl(TIMERx + TIMER_TCDR_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER_TCDR2, timerCfg->clockDivision);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER_TCDR3, timerCfg->clockDivision);
    }

    writel(tmpVal, TIMERx + TIMER_TCDR_OFFSET);

    /* Configure timer count mode: preload or free run */
    TIMER_SetCountMode(timerId, timerCh, timerCfg->countMode);

    /* Configure timer preload trigger src */
    TIMER_SetPreloadTrigSrc(timerId, timerCh, timerCfg->plTrigSrc);

    if (timerCfg->countMode == TIMER_COUNT_PRELOAD) {
        /* Configure timer preload value */
        TIMER_SetPreloadValue(timerId, timerCh, timerCfg->preLoadVal);

        /* Configure match compare values */
        if (timerCfg->matchVal0 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0);
        }

        if (timerCfg->matchVal1 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1);
        }

        if (timerCfg->matchVal2 > 1 + timerCfg->preLoadVal) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2);
        }
    } else {
        /* Configure match compare values */
        if (timerCfg->matchVal0 > 1) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_0, timerCfg->matchVal0);
        }

        if (timerCfg->matchVal1 > 1) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_1, timerCfg->matchVal1);
        }

        if (timerCfg->matchVal2 > 1) {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2 - 2);
        } else {
            TIMER_SetCompValue(timerId, timerCh, TIMER_COMP_ID_2, timerCfg->matchVal2);
        }
    }

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(TIMER0_CH0_IRQn, TIMER0_CH0_IRQHandler);
    Interrupt_Handler_Register(TIMER0_CH1_IRQn, TIMER0_CH1_IRQHandler);
#endif

    return SUCCESS;
}

int TIMER_DeInit(TIMER_ID_Type timerId)
{
    if (TIMER0_ID == timerId) {
        GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_TIMER);
    }

    return SUCCESS;
}

void TIMER_Enable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TCER_OFFSET);
    tmpVal |= (1 << (timerCh + 1));

    writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
}

void TIMER_Disable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_TCER_OFFSET);
    tmpVal &= (~(1 << (timerCh + 1)));

    writel(tmpVal, TIMERx + TIMER_TCER_OFFSET);
}

void TIMER_IntMask(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_INT_Type intType, int intMask)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpAddr = TIMERx + TIMER_TIER2_OFFSET + 4 * timerCh;
    tmpVal = readl(tmpAddr);

    switch (intType) {
        case TIMER_INT_COMP_0:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER3_0), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_0), tmpAddr);
            }

            break;

        case TIMER_INT_COMP_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER3_1), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_1), tmpAddr);
            }

            break;

        case TIMER_INT_COMP_2:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER3_2), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_2), tmpAddr);
            }

            break;

        case TIMER_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER3_0), tmpAddr);
                writel(reg_set_bit(tmpVal, TIMER_TIER3_1), tmpAddr);
                writel(reg_set_bit(tmpVal, TIMER_TIER3_2), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_0), tmpAddr);
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_1), tmpAddr);
                writel(reg_clr_bit(tmpVal, TIMER_TIER3_2), tmpAddr);
            }

            break;

        default:
            break;
    }
}

void TIMER_GPIOSetPolarity(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_GPIO_Polarity_Type polarity)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_GPIO_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER2_GPIO_INV, polarity);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER3_GPIO_INV, polarity);
    }

    writel(tmpVal, TIMERx + TIMER_GPIO_OFFSET);
}

void TIMER_CH0_SetMeasurePulseWidth(TIMER_ID_Type timerId, int enable)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_GPIO_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER2_GPIO_EN, enable);
    writel(tmpVal, TIMERx + TIMER_GPIO_OFFSET);
}

uint32_t TIMER_CH0_GetMeasurePulseWidth(TIMER_ID_Type timerId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    do {
        tmpVal = readl(TIMERx + TIMER_GPIO_OFFSET);
    } while (!reg_is_bit_set(tmpVal, TIMER_GPIO_LAT_OK));

    return (readl(TIMERx + TIMER_GPIO_LAT2_OFFSET) - readl(TIMERx + TIMER_GPIO_LAT1_OFFSET));
}

void TIMER_ForceClockDivision(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[timerId];

    /* Check the parameters */

    /* write 0 after write 1 to force clock division */
    tmpVal = readl(TIMERx + TIMER_TCDR_FORCE_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bit(tmpVal, TIMER_TCDR2_FORCE);
    } else {
        tmpVal = reg_set_bit(tmpVal, TIMER_TCDR3_FORCE);
    }

    writel(tmpVal, TIMERx + TIMER_TCDR_FORCE_OFFSET);

    tmpVal = readl(TIMERx + TIMER_TCDR_FORCE_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_clr_bit(tmpVal, TIMER_TCDR2_FORCE);
    } else {
        tmpVal = reg_clr_bit(tmpVal, TIMER_TCDR3_FORCE);
    }

    writel(tmpVal, TIMERx + TIMER_TCDR_FORCE_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void TIMER_Int_Callback_Install(TIMER_ID_Type timerId, TIMER_Chan_Type timerChan, TIMER_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */

    timerIntCbfArra[timerId][timerChan][intType] = cbFun;
}
#endif

void WDT_Set_Clock(WDT_ID_Type wdtId, TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    /* Configure watchdog timer clock source */
    tmpVal = readl(TIMERx + TIMER_TCCR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_CS_WDT, clkSrc);
    writel(tmpVal, TIMERx + TIMER_TCCR_OFFSET);

    /* Configure watchdog timer clock divison */
    tmpVal = readl(TIMERx + TIMER_TCDR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_WCDR, div);
    writel(tmpVal, TIMERx + TIMER_TCDR_OFFSET);
}

uint16_t WDT_GetMatchValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    /* Get watchdog timer match register value */
    tmpVal = readl(TIMERx + TIMER_WMR_OFFSET);
    tmpVal = reg_get_bits(tmpVal, TIMER_WMR);

    return tmpVal;
}

void WDT_SetCompValue(WDT_ID_Type wdtId, uint16_t val)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    /* Set watchdog timer match register value */
    tmpVal = readl(TIMERx + TIMER_WMR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_WMR, val);
    writel(tmpVal, TIMERx + TIMER_WMR_OFFSET);
}

void WDT_CompValueEffectImmediately(WDT_ID_Type wdtId, int enable)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_WMR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_WDT_ALIGN, 1 - enable);
    writel(tmpVal, TIMERx + TIMER_WMR_OFFSET);
}

uint16_t WDT_GetCounterValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    /* Get watchdog timer count register value */
    tmpVal = readl(TIMERx + TIMER_WVR_OFFSET);

    return tmpVal;
}

void WDT_ResetCounterValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    /* Reset watchdog timer count register value */
    WDT_ENABLE_ACCESS(TIMERx);

    tmpVal = readl(TIMERx + TIMER_WCR_OFFSET);

    /* Set watchdog counter reset register bit0 to 1 */
    writel(reg_set_bit(tmpVal, TIMER_WCR), TIMERx + TIMER_WCR_OFFSET);
}

int WDT_GetResetStatus(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    /* Get watchdog status register */
    tmpVal = readl(TIMERx + TIMER_WSR_OFFSET);

    return (reg_is_bit_set(tmpVal, TIMER_WTS)) ? SET : RESET;
}

void WDT_ClearResetStatus(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    tmpVal = readl(TIMERx + TIMER_WSR_OFFSET);

    /* Set watchdog status register */
    writel(reg_clr_bit(tmpVal, TIMER_WTS), TIMERx + TIMER_WSR_OFFSET);
}

void WDT_ClearInt(WDT_ID_Type wdtId)
{
    uint32_t TIMERx = timerAddr[wdtId];
    uint32_t tmpVal = readl(TIMERx + TIMER_WICR_OFFSET);
    writel(reg_set_bit(tmpVal, TIMER_WICLR), TIMERx + TIMER_WICR_OFFSET);
}

void WDT_Enable(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

#ifndef IPRO_USE_HAL_DRIVER
    Interrupt_Handler_Register(TIMER0_WDT_IRQn, TIMER0_WDT_IRQHandler);
#endif

    WDT_ENABLE_ACCESS(TIMERx);

    tmpVal = readl(TIMERx + TIMER_WMER_OFFSET);

    writel(reg_set_bit(tmpVal, TIMER_WE), TIMERx + TIMER_WMER_OFFSET);
}

void WDT_Disable(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    tmpVal = readl(TIMERx + TIMER_WMER_OFFSET);

    writel(reg_clr_bit(tmpVal, TIMER_WE), TIMERx + TIMER_WMER_OFFSET);
}

void WDT_ForceClockDivision(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    /* write 0 after write 1 to force clock division */
    tmpVal = readl(TIMERx + TIMER_TCDR_FORCE_OFFSET);

    tmpVal = reg_set_bit(tmpVal, TIMER_WCDR_FORCE);

    writel(tmpVal, TIMERx + TIMER_TCDR_FORCE_OFFSET);

    tmpVal = readl(TIMERx + TIMER_TCDR_FORCE_OFFSET);

    tmpVal = reg_clr_bit(tmpVal, TIMER_WCDR_FORCE);

    writel(tmpVal, TIMERx + TIMER_TCDR_FORCE_OFFSET);
}

void WDT_IntMask(WDT_ID_Type wdtId, WDT_INT_Type intType, int intMask)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    WDT_ENABLE_ACCESS(TIMERx);

    /* Deal with watchdog match/interrupt enable register,
	  WRIE:watchdog reset/interrupt enable */
    tmpVal = readl(TIMERx + TIMER_WMER_OFFSET);

    switch (intType) {
        case WDT_INT:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                /* 0 means generates a watchdog interrupt,
				   a watchdog timer reset is not generated*/
                writel(reg_clr_bit(tmpVal, TIMER_WRIE), TIMERx + TIMER_WMER_OFFSET);
            } else {
                /* Disable this interrupt */
                /* 1 means generates a watchdog timer reset,
				   a watchdog  interrupt is not generated*/
                writel(reg_set_bit(tmpVal, TIMER_WRIE), TIMERx + TIMER_WMER_OFFSET);
            }

            break;

        default:
            break;
    }
}

#ifndef IPRO_USE_HAL_DRIVER
void WDT_Int_Callback_Install(WDT_ID_Type wdtId, WDT_INT_Type wdtInt, intCallback_Type *cbFun)
{
    /* Check the parameters */

    wdtIntCbfArra[wdtId][wdtInt] = cbFun;
}
#endif

void WDT_GPIOSetPolarity(WDT_ID_Type wdtId, TIMER_GPIO_Polarity_Type polarity)
{
    uint32_t tmpVal;
    uint32_t TIMERx = timerAddr[wdtId];

    /* Check the parameters */

    tmpVal = readl(TIMERx + TIMER_GPIO_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_WDT_GPIO_INV, polarity);
    writel(tmpVal, TIMERx + TIMER_GPIO_OFFSET);
}

#ifndef IPRO_USE_HAL_DRIVER
void TIMER0_CH0_IRQHandler(void)
{
    TIMER_IntHandler(TIMER0_ID, TIMER_CH0);
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void TIMER0_CH1_IRQHandler(void)
{
    TIMER_IntHandler(TIMER0_ID, TIMER_CH1);
}
#endif

#ifndef IPRO_USE_HAL_DRIVER
void TIMER0_WDT_IRQHandler(void)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS(TIMER_BASE);

    tmpVal = readl(TIMER_BASE + TIMER_WICR_OFFSET);
    writel(reg_set_bit(tmpVal, TIMER_WICLR), TIMER_BASE + TIMER_WICR_OFFSET);

    if (wdtIntCbfArra[WDT0_ID][WDT_INT] != NULL) {
        /* Call the callback function */
        wdtIntCbfArra[WDT0_ID][WDT_INT]();
    }
}
#endif

