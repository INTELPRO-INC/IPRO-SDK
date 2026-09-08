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

#ifndef _PLATFORM_COMMON_H_
#define _PLATFORM_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "soc_base.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#define MSG(a, ...)     printf(a, ##__VA_ARGS__)
#define MSG_DBG(a, ...) printf(a, ##__VA_ARGS__)
#define MSG_WAR(a, ...) printf(a, ##__VA_ARGS__)
#define MSG_ERR(a, ...) printf(a, ##__VA_ARGS__)
#define DRV_CASE_FAIL_FUNC          \
    {                         \
        MSG("case fail\r\n"); \
    }
#define DRV_CASE_SUCCESS_FUNC          \
    {                            \
        MSG("case success\r\n"); \
    }

/* compatible with old version */
#ifndef DBG_TAG
#define DBG_TAG "DEBUG"
#endif

/*
 * The color for terminal (foreground)
 * BLACK    30
 * RED      31
 * GREEN    32
 * YELLOW   33
 * BLUE     34
 * PURPLE   35
 * CYAN     36
 * WHITE    37
 */
#define _DBG_COLOR(n) printf("\033[" #n "m")
#define _DBG_LOG_HDR(lvl_name, color_n) \
    printf("\033[" #color_n "m[" lvl_name "/" DBG_TAG "] ")
#define _DBG_LOG_X_END \
    printf("\033[0m\n")

#define dbg_log_line(lvl, color_n, fmt, ...)      \
    do {                                          \
        _DBG_LOG_HDR(lvl, color_n);               \
        printf(fmt, ##__VA_ARGS__); \
        _DBG_LOG_X_END;                           \
    } while (0)

#define LOG_D(fmt, ...) dbg_log_line("D", 0, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) dbg_log_line("I", 35, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) dbg_log_line("W", 33, fmt, ##__VA_ARGS__)
#define LOG_E(fmt, ...) dbg_log_line("E", 31, fmt, ##__VA_ARGS__)
#define LOG_RAW(...)    printf(__VA_ARGS__)

void platform_init(uint32_t baudrate);
void platform_print_set(uint8_t disable);
uint8_t platform_print_get(void);
uint32_t platform_get_log(uint8_t *data, uint32_t maxlen);
void platform_deinit(void);
void platform_init_time(void);
int platform_get_random(uint8_t *data,uint32_t len);

uint64_t platform_get_time_ms(void);
uint64_t platform_get_time_us(void);
void platform_delay_ms(uint32_t ms);
void platform_delay_us(uint32_t us);
void platform_putchar(const char c);
uint32_t platform_get_time_us_since_mark(void);
void platform_mark_time(void);

#ifdef __cplusplus
}
#endif

#endif // _PLATFORM_COMMON_H_
