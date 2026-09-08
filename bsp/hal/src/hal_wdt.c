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

#include "hal_wdt.h"
#include "drv_timer.h"

void wdt_isr(void)
{
    WDT_ClearInt(WDT0_ID);
}

void wdt_init(int int_mode, uint32_t wdt_timeout)
{
    WDT_Disable(WDT0_ID);
    if (wdt_timeout > 0) {
        WDT_SetCompValue(WDT0_ID, wdt_timeout);

        if (int_mode) {
            Interrupt_Handler_Register(TIMER0_WDT_IRQn, wdt_isr);
            WDT_IntMask(WDT0_ID, WDT_INT, UNMASK);
            CPU_Interrupt_Pending_Clear(TIMER0_WDT_IRQn);
            CPU_Interrupt_Enable(TIMER0_WDT_IRQn);
        } else {
            WDT_IntMask(WDT0_ID, WDT_INT, MASK);
            CPU_Interrupt_Pending_Clear(TIMER0_WDT_IRQn);
            CPU_Interrupt_Disable(TIMER0_WDT_IRQn);
        }

        WDT_Enable(WDT0_ID);
    }
}

void wdt_disable(void)
{
    WDT_Disable(WDT0_ID);
}

uint32_t wdt_get_counter(void)
{
    return WDT_GetCounterValue(WDT0_ID);
}

void wdt_feed(void)
{
    WDT_ResetCounterValue(WDT0_ID);
}

void wdt_isWDTReset(void)
{
    WDT_GetResetStatus(WDT0_ID);
}

void wdt_clearWDTReset(void)
{
    WDT_ClearResetStatus(WDT0_ID);
}

void wdt_setTimeout(uint32_t wdt_timeout)
{
    WDT_Disable(WDT0_ID);
    WDT_SetCompValue(WDT0_ID, wdt_timeout);
    WDT_Enable(WDT0_ID);
}

