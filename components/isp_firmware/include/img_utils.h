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

#ifndef __IMG_UTILITY_H__
#define __IMG_UTILITY_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <FreeRTOS.h>
#include <task.h>

#if !defined(MIN)
#define MIN(x, y) ((x) > (y) ? (y) : (x))
#endif

#if !defined(MAX)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

#define DIV_ROUND(divident, divider)    (((divident)+((divider)>>1)) / (divider))

typedef struct {
    TickType_t tick;
    uint32_t   pmc_cntr;
    uint32_t   pmxev_cntr;
} img_perf_t;

int32_t round_off(int32_t data, int32_t round_bit);
int32_t alpha_blend(int32_t data_0, int32_t data_1, int32_t alpha_0, int32_t alpha_precision);
int32_t bilinear_filter(int32_t data_y0_x0, int32_t data_y0_x1, int32_t data_y1_x0, int32_t data_y1_x1, int32_t x_position, int32_t x_precision, int32_t y_position, int32_t y_precision);
int32_t clip(int32_t min, int32_t max, int32_t data);
int32_t soft_clip(int32_t min, int32_t max, int32_t data, int32_t slope, int32_t slope_precision);
bool    within(int32_t min, int32_t max, int32_t data);
int     even_division(int dividend, int divisor);
#if 0//joie
void img_perf_enter();
void img_perf_get(char* tag, img_perf_t *perf);
void img_perf_printer(char* tag, img_perf_t *start, img_perf_t *end);
void img_perf_exit();

int utils_get_resolution_param(char *optarg, uint32_t *width, uint32_t *height);
#endif
#endif
