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

#include "img_utils.h"

int32_t round_off(int32_t data, int32_t round_bit)
{
    if (round_bit == 1) {
      return data >> round_bit;
    }
    else {
      return (data + (1 << (round_bit - 1))) >> round_bit;
    }
}

int32_t alpha_blend(int32_t data_0, int32_t data_1, int32_t alpha_0, int32_t alpha_prec)
{
    return round_off(alpha_0 * (data_0 - data_1) + (data_1 << alpha_prec), alpha_prec);
}

int32_t bilinear_filter(int32_t data_y0_x0, int32_t data_y0_x1, int32_t data_y1_x0, int32_t data_y1_x1, int32_t x_position, int32_t x_precision, int32_t y_position, int32_t y_precision)
{
  int32_t data_y0 = alpha_blend(data_y0_x1, data_y0_x0, x_position, x_precision);
  int32_t data_y1 = alpha_blend(data_y1_x1, data_y1_x0, x_position, x_precision);

  return alpha_blend(data_y1, data_y0, y_position, y_precision);
}

int32_t clip(int32_t min, int32_t max, int32_t data)
{
    if (data < min) {
      return min;
    }
    else if (data > max) {
      return max;
    }
    else {
      return data;
    }
}

int32_t soft_clip(int32_t min, int32_t max, int32_t data, int32_t slope, int32_t slope_precision)
{
    int32_t data_blend;

    if (data < min) {
        data_blend = min;
    }
    else if (data > max) {
        data_blend = max;
    }
    else {
        data_blend = data;
    }

    return alpha_blend(data, data_blend, slope, slope_precision);
}

bool within(int32_t min, int32_t max, int32_t data)
{
    if (data >= min && data <= max) {
        return true;
    }
    else {
        return false;
    }
}

int even_division(int dividend, int divisor)
{
    int tmp = dividend/divisor;
    return (tmp - tmp % 2);
}
#if 0 //joie
void img_perf_enter()
{
    vTaskEnterCritical();
    //PMC_Reset_All();
    //PMC_Select(0);
    printf("================ENTER IMG PERF==================\r\n");
}

void img_perf_get(char* tag, img_perf_t *perf)
{
    perf->tick       = xTaskGetTickCount();
    //perf->pmc_cntr   = __get_PMCCNTR();
    //perf->pmxev_cntr = __get_PMXEVCNTR();
    printf("[%s] tick: %lu, pmc_cntr: %d, pmxev_cntr: %d\r\n",
        tag, (long unsigned int)perf->tick, (int)perf->pmc_cntr, (int)perf->pmxev_cntr);/*abner test*/
}

void img_perf_printer(char* tag, img_perf_t *start, img_perf_t *end)
{
    TickType_t elapse_time;
    uint32_t pmc_num, pmxev_num;
    float mcps, mips;

    elapse_time = end->tick - start->tick;
    pmc_num     = end->pmc_cntr - start->pmc_cntr;
    pmxev_num   = end->pmxev_cntr - start->pmxev_cntr;

    mcps = (float)pmc_num/elapse_time/1000;
    mips = (float)pmxev_num/elapse_time/1000;

    printf("[%s]: pmc_num: %d,  pmxev_num: %d\r\n",
        tag, (int)pmc_num, (int)pmxev_num); /*abner test*/
    printf("[%s]: TICK: %lu,  MCPS: %.2f, MIPS: %.2f\r\n",
        tag, (long unsigned int)elapse_time, mcps, mips);/*abner test*/
}

void img_perf_exit()
{
    printf("================EXIT IMG PERF==================\r\n");
    //PMC_Reset_All();
    vTaskExitCritical();
}

// Supported argument
typedef enum
{
    DEFAULT_ISP_SIZE_1080P,
    DEFAULT_ISP_SIZE_1080p,
    DEFAULT_ISP_SIZE_720P,
    DEFAULT_ISP_SIZE_720p,
    DEFAULT_ISP_SIZE_SVGA,
    DEFAULT_ISP_SIZE_svga,
    DEFAULT_ISP_SIZE_VGA,
    DEFAULT_ISP_SIZE_vga,
    DEFAULT_ISP_SIZE_MAX,
}DEFAULT_ISP_SIZE;

static char def_img_size[DEFAULT_ISP_SIZE_MAX][16] =
{
    "1080P",
    "1080p",
    "720P",
    "720p",
    "SVGA",
    "svga",
    "VGA",
    "vga",
};

int utils_get_resolution_param(char *optarg, uint32_t *width, uint32_t *height)
{
    if ((strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_1080P]) != 0) || (strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_1080p]) != 0)) {
        *width = 1920;
        *height = 1080;
    } else if ((strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_720P]) != 0) || (strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_720p]) != 0)) {
        *width = 1280;
        *height = 720;
    } else if ((strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_SVGA]) != 0) || (strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_svga]) != 0)) {
        *width = 800;
        *height = 600;
    } else if ((strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_VGA]) != 0) || (strstr(optarg, def_img_size[DEFAULT_ISP_SIZE_vga]) != 0)) {
        *width = 640;
        *height = 480;
    } else if (sscanf(optarg, "%ux%u", (unsigned int*)width, (unsigned int*)height) == 2) {
    } else {
        return -1;
    }

    return 0;
}
#endif 
