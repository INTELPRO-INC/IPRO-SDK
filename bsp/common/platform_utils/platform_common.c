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

#include "drv_hbn.h"
#include "hal_aon_wdt.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "hal_mtimer.h"
#include "drv_mmheap.h"
#include "hal_common.h"
#include "ring_buffer.h"
#include "platform_common.h"

#ifndef CONFIG_PLATFORM_UART_BAUDRATE
#define CONFIG_PLATFORM_UART_BAUDRATE 115200
#endif


static uint8_t uart_dbg_disable = 0;

extern void board_init(void);
extern int board_get_debug_uart_index(void);
extern void debug_uart_id_set(uint8_t id);

void platform_init(uint32_t baudrate)
{
    static uint8_t initialized = 0;
    extern void newlibc_init(void);

    hal_system_init();

#if !defined(CONFIG_AON_WDT_ENABLE) && !defined(BOOTROM)
    /* Boot2 may have enabled AON WDT. If FW doesn't manage it, disable here. */
    aon_wdt_disable();
#endif

    platform_init_time();

    if (!uart_dbg_disable) {
        uart_set_baudrate(board_get_debug_uart_index(), CONFIG_PLATFORM_UART_BAUDRATE);
        uart_init(board_get_debug_uart_index(), 0);
        debug_uart_id_set(board_get_debug_uart_index());
    }

    if (!initialized) {
        initialized = 1;
        extern void plt_show_cg_info(void);
        plt_show_cg_info();
    }    
}

void platform_print_set(uint8_t disable)
{
    uart_dbg_disable = disable;
}

uint8_t platform_print_get(void)
{
    return uart_dbg_disable;
}

int platform_get_random(uint8_t *data,uint32_t len)
{
    return 0;
}

uint32_t platform_get_log(uint8_t *data, uint32_t maxlen)
{
    return 0;
}

void platform_init_time(void)
{
    HBN_Enable_RTC_Counter();
}

void platform_deinit_time(void)
{
}

void platform_deinit(void)
{
    if (!uart_dbg_disable) {
        uart_uninit(board_get_debug_uart_index());
    }
}

uint64_t platform_get_time_ms()
{
    return mtimer_get_time_ms();
}

uint64_t platform_get_time_us()
{
    return mtimer_get_time_us();
}

void platform_delay_ms(uint32_t ms)
{
    mtimer_delay_ms(ms);
}
void platform_delay_us(uint32_t us)
{
    mtimer_delay_us(us);
}

#ifdef BOOTROM
static uint64_t mark_time = 0;
void platform_mark_time(void)
{
    mark_time = platform_get_time_us();
}

uint32_t  platform_get_time_us_since_mark(void)
{
    return platform_get_time_us() - mark_time;
}
#endif
