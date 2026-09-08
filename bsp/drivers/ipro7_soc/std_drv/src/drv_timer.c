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

#define TIMER_MAX_MATCH 3

uint32_t TIMER_GetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    return readl(TIMER_BASE + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo));
}

void TIMER_SetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val)
{
    writel(val, TIMER_BASE + TIMER_TMR2_0_OFFSET + 4 * (TIMER_MAX_MATCH * timerCh + cmpNo));
}

uint32_t TIMER_GetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;
    uint32_t tmpAddr;

    /* TO avoid risk of reading, don't read TCVWR directly*/
    /* request for read*/
    tmpAddr = TIMER_BASE + TIMER_TCVWR2_OFFSET + 4 * timerCh;
    writel(1, tmpAddr);

    /* Need wait */
    tmpVal = readl(tmpAddr);
    tmpVal = readl(tmpAddr);
    tmpVal = readl(tmpAddr);

    return tmpVal;
}

int TIMER_GetMatchStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpVal;
    int bitStatus = RESET;

    tmpVal = readl(TIMER_BASE + TIMER_TSR2_OFFSET + 4 * timerCh);

    switch (cmpNo) {
        case TIMER_COMP_ID_0:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR2_0) ? SET : RESET;
            break;

        case TIMER_COMP_ID_1:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR2_1) ? SET : RESET;
            break;

        case TIMER_COMP_ID_2:
            bitStatus = reg_is_bit_set(tmpVal, TIMER_TSR2_2) ? SET : RESET;
            break;

        default:
            break;
    }

    return bitStatus;
}

uint32_t TIMER_GetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    return readl(TIMER_BASE + TIMER_TPLVR2_OFFSET + 4 * timerCh);
}

void TIMER_SetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint32_t val)
{
    writel(val, TIMER_BASE + TIMER_TPLVR2_OFFSET + 4 * timerCh);
}

void TIMER_SetPreloadSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc)
{
    writel(plSrc, TIMER_BASE + TIMER_TPLCR2_OFFSET + 4 * timerCh);
}

void TIMER_SetCountMode(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode)
{
    uint32_t tmpval;

    tmpval = readl(TIMER_BASE + TIMER_TCMR_OFFSET);
    tmpval &= (~(1 << (timerCh + 1)));
    tmpval |= (countMode << (timerCh + 1));

    writel(tmpval, TIMER_BASE + TIMER_TCMR_OFFSET);
}

void TIMER_ClearIntStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;

    tmpAddr = TIMER_BASE + TIMER_TICR2_OFFSET + 4 * timerCh;

    tmpVal = readl(tmpAddr);
    tmpVal |= (1 << cmpNo);

    writel(tmpVal, tmpAddr);
}

void TIMER_SetDiv(TIMER_ID_Type timerId, int timer, uint8_t div)
{
    uint32_t tmpVal;

    /* Configure timer clock division */
    tmpVal = readl(TIMER_BASE + TIMER_TCDR_OFFSET);

    if (timer == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER_TCDR2, div);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER_TCDR3, div);
    }

    writel(tmpVal, TIMER_BASE + TIMER_TCDR_OFFSET);
}

void TIMER_Set_Clock(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    uint32_t tmpVal;

    /* Configure timer clock source */
    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);
    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_2, clkSrc);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_3, clkSrc);
    }
    writel(tmpVal, TIMER_BASE + TIMER_TCCR_OFFSET);

    /* Configure timer clock division */
    TIMER_SetDiv(timerId, timerCh, div);
}

int TIMER_Init(TIMER_ID_Type timerId, TIMER_CFG_Type *timerCfg)
{
    TIMER_Chan_Type timerCh = timerCfg->timerCh;
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);

    if (timerCh == TIMER_CH0) {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_2, timerCfg->clkSrc);
    } else {
        tmpVal = reg_set_bits(tmpVal, TIMER_CS_3, timerCfg->clkSrc);
    }

    writel(tmpVal, TIMER_BASE + TIMER_TCCR_OFFSET);

    TIMER_SetDiv(timerId, timerCh, timerCfg->clockDivision);

    /* Configure timer count mode: preload or free run */
    TIMER_SetCountMode(timerId, timerCh, timerCfg->countMode);

    /* Configure timer preload trigger src */
    TIMER_SetPreloadSrc(timerId, timerCh, timerCfg->plTrigSrc);

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

    return SUCCESS;
}

void TIMER_Enable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCER_OFFSET);
    tmpVal |= (1 << (timerCh + 1));

    writel(tmpVal, TIMER_BASE + TIMER_TCER_OFFSET);
}

void TIMER_Disable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    uint32_t tmpVal;

    tmpVal = readl(TIMER_BASE + TIMER_TCER_OFFSET);
    tmpVal &= (~(1 << (timerCh + 1)));

    writel(tmpVal, TIMER_BASE + TIMER_TCER_OFFSET);
}

void TIMER_IntMask(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_INT_Type intType, int intMask)
{
    uint32_t tmpAddr;
    uint32_t tmpVal;

    tmpAddr = TIMER_BASE + TIMER_TIER2_OFFSET + 4 * timerCh;
    tmpVal = readl(tmpAddr);

    switch (intType) {
        case TIMER_INT_COMP_0:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER2_0), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_0), tmpAddr);
            }

            break;

        case TIMER_INT_COMP_1:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER2_1), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_1), tmpAddr);
            }

            break;

        case TIMER_INT_COMP_2:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER2_2), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_2), tmpAddr);
            }

            break;

        case TIMER_INT_ALL:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                writel(reg_set_bit(tmpVal, TIMER_TIER2_0), tmpAddr);
                writel(reg_set_bit(tmpVal, TIMER_TIER2_1), tmpAddr);
                writel(reg_set_bit(tmpVal, TIMER_TIER2_2), tmpAddr);
            } else {
                /* Disable this interrupt */
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_0), tmpAddr);
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_1), tmpAddr);
                writel(reg_clr_bit(tmpVal, TIMER_TIER2_2), tmpAddr);
            }

            break;

        default:
            break;
    }
}

void WDT_Set_Clock(WDT_ID_Type wdtId, TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    uint32_t tmpVal;

    /* Configure watchdog timer clock source */
    tmpVal = readl(TIMER_BASE + TIMER_TCCR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_CS_WDT, clkSrc);
    writel(tmpVal, TIMER_BASE + TIMER_TCCR_OFFSET);

    /* Configure watchdog timer clock divison */
    tmpVal = readl(TIMER_BASE + TIMER_TCDR_OFFSET);
    tmpVal = reg_set_bits(tmpVal, TIMER_WCDR, div);
    writel(tmpVal, TIMER_BASE + TIMER_TCDR_OFFSET);
}

uint16_t WDT_GetMatchValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WMR_OFFSET);

    return tmpVal;
}

void WDT_SetCompValue(WDT_ID_Type wdtId, uint16_t val)
{
    WDT_ENABLE_ACCESS();

    writel(val, TIMER_BASE + TIMER_WMR_OFFSET);
}

uint16_t WDT_GetCounterValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WVR_OFFSET);

    return tmpVal;
}

void WDT_ResetCounterValue(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WCR_OFFSET);

    writel(reg_set_bit(tmpVal, TIMER_WCR), TIMER_BASE + TIMER_WCR_OFFSET);
}

int WDT_GetResetStatus(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WSR_OFFSET);

    return (reg_is_bit_set(tmpVal, TIMER_WTS)) ? SET : RESET;
}

void WDT_ClearResetStatus(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WSR_OFFSET);

    writel(reg_clr_bit(tmpVal, TIMER_WTS), TIMER_BASE + TIMER_WSR_OFFSET);
}

void WDT_Enable(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WMER_OFFSET);

    writel(reg_set_bit(tmpVal, TIMER_WE), TIMER_BASE + TIMER_WMER_OFFSET);
}

void WDT_Disable(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    tmpVal = readl(TIMER_BASE + TIMER_WMER_OFFSET);

    writel(reg_clr_bit(tmpVal, TIMER_WE), TIMER_BASE + TIMER_WMER_OFFSET);
}

void WDT_ForceClockDivision(WDT_ID_Type wdtId)
{
    uint32_t tmpVal = readl(TIMER_BASE + TIMER_WICR_OFFSET);
    writel(reg_set_bit(tmpVal, TIMER_WICLR), TIMER_BASE + TIMER_WICR_OFFSET);
}

void WDT_IntMask(WDT_ID_Type wdtId, WDT_INT_Type intType, int intMask)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();

    /* Deal with watchdog match/interrupt enable register,
      WRIE:watchdog reset/interrupt enable */
    tmpVal = readl(TIMER_BASE + TIMER_WMER_OFFSET);

    switch (intType) {
        case WDT_INT:
            if (intMask == UNMASK) {
                /* Enable this interrupt */
                /* 0 means generates a watchdog interrupt,
                   a watchdog timer reset is not generated*/
                writel(reg_clr_bit(tmpVal, TIMER_WRIE), TIMER_BASE + TIMER_WMER_OFFSET);
            } else {
                /* Disable this interrupt */
                /* 1 means generates a watchdog timer reset,
                   a watchdog  interrupt is not generated*/
                writel(reg_set_bit(tmpVal, TIMER_WRIE), TIMER_BASE + TIMER_WMER_OFFSET);
            }

            break;

        default:
            break;
    }
}

void WDT_ClearInt(WDT_ID_Type wdtId)
{
    uint32_t tmpVal;

    WDT_ENABLE_ACCESS();
    tmpVal = readl(TIMER_BASE + TIMER_WICR_OFFSET);
    writel(reg_set_bit(tmpVal, TIMER_WICLR), TIMER_BASE + TIMER_WICR_OFFSET);

}

