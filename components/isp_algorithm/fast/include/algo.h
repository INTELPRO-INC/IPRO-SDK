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

#ifndef __ALGO_H__
#define __ALGO_H__

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h>

//#include "utility.h"
#include "../../isp_firmware/include/isp_comm_sns.h"
#include "../../isp_firmware/include/img_utils.h"

typedef enum {
    LOG_ALGO_WB = 0,
    LOG_ALGO_FAST_AE,
    LOG_ALGO_NUM,
} log_alog_t;

typedef enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
} log_level_t;


#define PRINTF_EN 0
#define LOG_ALG_MODE    LOG_ALGO_FAST_AE
#define LOG_LEVEL       LOG_LEVEL_NONE//LOG_LEVEL_DEBUG
typedef unsigned int EXPO_TIME;    /* exposure time type */
typedef int GAIN_DB; /* gain value in dB */
typedef int COLOR_TEMP;                   /* ct value in K */

#define aprintf(algo, level, args...) \
    (((LOG_ALG_MODE == algo) && (LOG_LEVEL >= level)) \
    ? ((void)printf(args)) \
    : (void)0)

typedef struct {
    int start_cnt;
    int calc_idx;
    int luma_target;
    int base_ev;
} ae_fast_param_t;

int show_wb_version(void);
int show_fast_ae_version(void);
void fast_awb_init(int img_width, int img_height);
void calc_wb_gain(int *rgain, int *bgain);
void fast_calc_ae_expo_gain(int luma_avg, int *ae_total_gain, int *ae_expo_level);
void fast_ae_init(ae_fast_param_t* param);
int fast_ae_stat_luma_avg(void);
void get_awb_level(int level, int *rgain, int *bgain);
void set_awb_level(int level, int rgain, int bgain);
void fast_ae_get_internal_cfg(int *low_flag);
int fast_ae_get_low_base(void);
void fast_awb_reset(void);

#endif //__ALGO_H__
