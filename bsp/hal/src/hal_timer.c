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

#include "hal_timer.h"
#include "drv_glb.h"
#include "drv_timer.h"
#include "drv_clock.h"

void TIMER0_IRQ(void);
void TIMER1_IRQ(void);

static timer_device_t timer_devices[TIMER_MAX_INDEX][TIMER_CH_MAX_INDEX];

int timer_init(int timer_id, int timer_ch, hal_timer_config_t *timerCfg)
{
    timer_device_t *timer_device = &timer_devices[timer_id][timer_ch];

    timer_device->id = timer_id;
    timer_device->ch = timer_ch;
    timer_device->cnt_mode = timerCfg->cnt_mode;
    timer_device->trigger = timerCfg->trigger;
    timer_device->reload = timerCfg->reload;
    timer_device->timeout1 = timerCfg->timeout1;
    timer_device->timeout2 = timerCfg->timeout2;
    timer_device->timeout3 = timerCfg->timeout3;

    uint32_t compare_count1 = 0;
    uint32_t compare_count2 = 0;
    uint32_t compare_count3 = 0;
    uint32_t reload_val = 0;
    uint32_t clkval = 0;
    uint32_t unit = 0;

    /* Disable all interrupt */
    TIMER_IntMask(timer_id, timer_ch, TIMER_INT_ALL, MASK);
    /* Disable timer before config */
    TIMER_Disable(timer_id, timer_ch);

    /* Configure timer count mode: preload or free run */
    TIMER_SetCountMode(timer_id, timer_ch, timer_device->cnt_mode);

    /* Configure timer preload trigger source */
    TIMER_SetPreloadSrc(timer_id, timer_ch, timer_device->trigger);

    if (timer_device->cnt_mode == TIMER_CNT_PRELOAD) {
        TIMER_SetPreloadValue(timer_id, timer_ch, timer_device->reload);
        reload_val = timer_device->reload;
    }
    if (timer_id == TIMER_CH0) {
        clkval = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER0);
    } else {
        clkval = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER1);
    }

    if (clkval % 1000000 == 0) {
        unit = 1000000; //1us
    } else if (clkval % 100000 == 0) {
        unit = 100000; //10us
    } else if (clkval % 10000 == 0) {
        unit = 10000; //100us
    } else if (clkval % 1000 == 0) {
        unit = 1000; //1ms
    } else if (clkval % 100 == 0) {
        unit = 100; //10ms
    } else if (clkval % 10 == 0) {
        unit = 10; //100ms
    } else if (clkval % 1 == 0) {
        unit = 1; //s
    } else {
    }

    compare_count1 = timer_device->timeout1 / (1000000 / unit) * (clkval / unit) + reload_val;
    compare_count2 = timer_device->timeout2 / (1000000 / unit) * (clkval / unit) + reload_val;
    compare_count3 = timer_device->timeout3 / (1000000 / unit) * (clkval / unit) + reload_val;

    /* Configure match compare values */
    if ((compare_count1 < 1) && (compare_count2 < 1) && (compare_count3 < 1)) {
        return -1;
    }

    TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_0, MASK);
    TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_1, MASK);
    TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_2, MASK);
    TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_0, 0xFFFFFFFF);
    TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_1, 0xFFFFFFFF);
    TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_2, 0xFFFFFFFF);

    switch (timer_device->trigger) {
        case TIMER_PRELOAD_TRIGGER_COMP2:
            TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_2, compare_count3 - 2);
            TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_2);
            TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_2, UNMASK);
        case TIMER_PRELOAD_TRIGGER_COMP1:
            TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_1, compare_count2 - 2);
            TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_1);
            TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_1, UNMASK);
        case TIMER_PRELOAD_TRIGGER_COMP0:
            TIMER_SetCompValue(timer_id, timer_ch, TIMER_COMP_ID_0, compare_count1 - 2);
            TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_0);
            TIMER_IntMask(timer_id, timer_ch, TIMER_INT_COMP_0, UNMASK);
            break;
        default:
            break;
    }

    if (timer_id == TIMER0_ID) {
        if (timer_ch == TIMER_CH0_INDEX) {
            CPU_Interrupt_Enable(TIMER0_CH0_IRQn);
            Interrupt_Handler_Register(TIMER0_CH0_IRQn, TIMER0_IRQ);
        }
        if (timer_ch == TIMER_CH1_INDEX) {
            CPU_Interrupt_Enable(TIMER0_CH1_IRQn);
            Interrupt_Handler_Register(TIMER0_CH1_IRQn, TIMER1_IRQ);
        }
    }
    return 0;
}

void timer_set_div(int timer_id, int timer_ch, uint8_t div)
{
    TIMER_SetDiv(timer_id, timer_ch, div);
}

void timer_start(int timer_id, int timer_ch)
{
    TIMER_Enable(timer_id, timer_ch);
}

void timer_stop(int timer_id, int timer_ch)
{
    TIMER_Disable(timer_id, timer_ch);
}

uint32_t timer_get_counter(int timer_id, int timer_ch)
{
#ifdef FPGA
    return TIMER_GetCounterValue(timer_id, timer_ch) >> 5;
#else
    return TIMER_GetCounterValue(timer_id, timer_ch);
#endif
}


void timer_setCallback(int timer_id, int timer_ch, void *fn)
{
    timer_device_t *timer_device = &timer_devices[timer_id][timer_ch];
    timer_device->callback = fn;
}

int timer_control(int timer_id, int timer_ch, int cmd, void *args)
{
    timer_device_t *timer_device = &timer_devices[timer_id][timer_ch];

    switch (cmd) {
    case TIMER_CTRL_SET_INT:
    {
        uint32_t offset = __builtin_ctz((uint32_t)args);

        while (offset < 3) {
            if ((uint32_t)args & (1 << offset)) {

            }
            offset++;
        }

        if (timer_ch == TIMER_CH0_INDEX) {
            CPU_Interrupt_Enable(TIMER0_CH0_IRQn);
        } else if (timer_ch == TIMER_CH1_INDEX) {
            CPU_Interrupt_Enable(TIMER0_CH1_IRQn);
        }

        break;
    }

    case TIMER_CTRL_CLR_INT:
    {
        uint32_t offset = __builtin_ctz((uint32_t)args);

        while (offset < 3) {
            if ((uint32_t)args & (1 << offset)) {
                TIMER_IntMask(timer_id, timer_ch, offset, MASK);
            }
            offset++;
        }
        if (timer_ch == TIMER_CH0) {
            CPU_Interrupt_Disable(TIMER0_CH0_IRQn);
        } else if (timer_ch == TIMER_CH1) {
            CPU_Interrupt_Disable(TIMER0_CH1_IRQn);
        }
        break;
    }

    case TIMER_CTRL_GET_INT:
    {
        uint32_t offset = __builtin_ctz((uint32_t)args);
        uint32_t intstatus = TIMER_GetMatchStatus(timer_id, timer_ch, offset);

        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_0);
        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_1);
        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_2);
        return intstatus;
    }

    case TIMER_CTRL_RESUME:
        TIMER_Enable(timer_id, timer_ch);
        break;

    case TIMER_CTRL_SUSPEND:
        TIMER_Disable(timer_id, timer_ch);
        break;

    case TIMER_CTRL_GET_CONFIG:
        return TIMER_GetCounterValue(timer_id, timer_ch);

    case TIMER_CTRL_SET_CALLBACK:
        timer_device->callback = args;
        break;

    default:
        break;
    }

    return 0;
}

int timer_write(int timer_id, int timer_ch, uint32_t pos, const void *buffer, uint32_t size)
{
    timer_device_t *timer_device = &timer_devices[timer_id][timer_ch];
    timer_timeout_cfg_t *timeout_cfg = (timer_timeout_cfg_t *)buffer;
    uint32_t compare_count = 0;
    uint32_t reload_val = 0;
    uint32_t clkval = 0;
    uint32_t unit = 0;

    if (size % sizeof(timer_timeout_cfg_t)) {
        return -1;
    }
    /* Disable timer before config */
    TIMER_Disable(timer_id, timer_ch);

    if (timer_device->cnt_mode == TIMER_CNT_PRELOAD) {
        reload_val = timer_device->reload;
    }

    if (timer_id == TIMER_CH0) {
        clkval = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER0);
    } else {
        clkval = Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER1);
    }

    if (clkval % 1000000 == 0) {
        unit = 1000000; //1us
    } else if (clkval % 100000 == 0) {
        unit = 100000; //10us
    } else if (clkval % 10000 == 0) {
        unit = 10000; //100us
    } else if (clkval % 1000 == 0) {
        unit = 1000; //1ms
    } else if (clkval % 100 == 0) {
        unit = 100; //10ms
    } else if (clkval % 10 == 0) {
        unit = 10; //100ms
    } else if (clkval % 1 == 0) {
        unit = 1; //s
    } else {
    }

    for (uint32_t i = 0; i < size / sizeof(timer_timeout_cfg_t); i++) {
        compare_count = timeout_cfg->timeout_val / (1000000 / unit) * (clkval / unit) + reload_val;

        if (compare_count < 1) {
            return -1;
        }
        TIMER_SetCompValue(timer_id, timer_ch, timeout_cfg->timeout_id, compare_count - 2);
    }
    TIMER_Enable(timer_id, timer_ch);
    return 0;
}

void timer_isr(int timer_id, int timer_ch)
{
    timer_device_t *timer_device = &timer_devices[timer_id][timer_ch];

    /* Comparator 0 match interrupt */
    if (TIMER_GetMatchStatus(timer_id, timer_ch, TIMER_COMP_ID_0)) {
        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_0);
        if (timer_device->callback)
            timer_device->callback(timer_id, 0, TIMER_EVENT_COMP0);
    }

    /* Comparator 1 match interrupt */
    if (TIMER_GetMatchStatus(timer_id, timer_ch, TIMER_COMP_ID_1)) {
        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_1);
        if (timer_device->callback)
            timer_device->callback(timer_id, 1, TIMER_EVENT_COMP1);
    }

    /* Comparator 2 match interrupt */
    if (TIMER_GetMatchStatus(timer_id, timer_ch, TIMER_COMP_ID_2)) {
        TIMER_ClearIntStatus(timer_id, timer_ch, TIMER_COMP_ID_2);
        if (timer_device->callback)
            timer_device->callback(timer_id, 2, TIMER_EVENT_COMP2);
    }
}

void TIMER0_IRQ(void)
{
    timer_isr(TIMER0_INDEX, TIMER_CH0_INDEX);
}

void TIMER1_IRQ(void)
{
    timer_isr(TIMER0_INDEX, TIMER_CH1_INDEX);
}

