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

#ifndef __ISP_COMM_SNS_H__
#define __ISP_COMM_SNS_H__

#include <stdint.h>
#include <stdbool.h>
#include <hal_isp.h>
#undef  DEBUG_MODULE
#define DEBUG_MODULE 0

typedef unsigned int EXPO_TIME;    /* exposure time type */
typedef int GAIN_DB;                            /* gain value in dB */


#define SEC_TO_NS(x)                    ((x) * 1000000000)
#define NS_TO_SEC(x)                    DIV_ROUND(x, 1000000000)

#define FLOAT_TO_EXPO_TIME(x)           (EXPO_TIME)( ( SEC_TO_NS(1)/(x) ) )
#define INT_TO_EXPO_TIME(x)             (EXPO_TIME)(DIV_ROUND(SEC_TO_NS(1),x))
#define EXPO_TIME_TO_INT(x)             (SEC_TO_NS(1) / (int)(x))
#define EXPO_TIME_1_SEC                 1000000000

#define EXPO_TIME_1_15_SEC              INT_TO_EXPO_TIME(15)
#define EXPO_TIME_1_25_SEC              INT_TO_EXPO_TIME(25)//40000000
#define EXPO_TIME_1_50_SEC              INT_TO_EXPO_TIME(50)
#define EXPO_TIME_1_100_SEC             INT_TO_EXPO_TIME(100)//10000000
#define EXPO_TIME_1_30_SEC              INT_TO_EXPO_TIME(30)
#define EXPO_TIME_1_60_SEC              INT_TO_EXPO_TIME(60)
#define EXPO_TIME_1_120_SEC             INT_TO_EXPO_TIME(120)
#define EXPO_TIME_MIN                   1

#define FLOAT_TO_GAIN_DB(x)             (GAIN_DB)((x) * 256)
#define INT_TO_GAIN_DB(x)               (GAIN_DB)((x) << 8)
#define GAIN_DB_TO_INT(x)               ((int)(x) >> 8)

#define GAIN_X_TO_DB(x)                 (log2((float)(x)) * 6)
#define GAIN_DB_FLOAT_TO_INT(x)         ((int)((float)(x) * 256 + 0.5))
#define GAIN_DB_INT_TO_FLOAT(x)         ((float)(x) / 256)
#define DB_TO_GAIN(x)                   (pow(2, GAIN_DB_INT_TO_FLOAT(x)/6))

#define GAIN_6_DB                       1536    /* 6 << 8 */
#define GAIN_0_DB                       0


#define IMG_START_SKIP_NUM        0

typedef struct {
    isp_hal_size_t act_win;
    isp_hal_size_t out_win;
} aaa_stats_cfg_t;


typedef enum
{
    LOG_OFF = 0,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG,
    LOG_ALL,
} LOGTYPE;


/**
 * Pre Set log Level
 */

#ifndef LOG_PRINT
#define LOG_PRINT(debug_module, type, ...) \
    (((debug_module && (5 != LOG_OFF)) || (type <= 5))  \
    ? ((void)printf(__VA_ARGS__)) \
    : (void)0)
#endif

/**
*   Simplified macro to print error message .
*/
#define LOGE(...) ((void)LOG_PRINT(DEBUG_MODULE, LOG_ERROR, __VA_ARGS__))


/**
*  Simplified macro to print warning message .
*/

#define LOGW(...) ((void)LOG_PRINT(DEBUG_MODULE, LOG_WARNING, __VA_ARGS__))


/**
*  Simplified macro to print info log message .
*/

#define LOGI(...) ((void)LOG_PRINT(DEBUG_MODULE, LOG_INFO, __VA_ARGS__))


/**
*  Simplified macro to print debug log message .
*/
#define LOGD(...) ((void)LOG_PRINT(DEBUG_MODULE, LOG_DEBUG, __VA_ARGS__))
#define ABORT() ((void)0)

#define ENUM_VALUE_INVALID  0xFFFF

typedef struct {
    int position;
    int value;
} intpl_point_t;


/**************************** helper functions ***********************************/

#define INIT_OBJ(obj)   \
    do {    \
        memset(&obj, 0, sizeof(obj));   \
    } while (0)

bool is_param_valid(int value, int min, int max);
void clip_param_range(int *value, int min, int max);
intpl_point_t create_intpl_point(int position, int value);
int linear_interpolate(intpl_point_t *point_L, intpl_point_t *point_H, int pos, int *value);
void table_linear_interpolate(int *input_tbl, int* output_tbl, int tbl_num, int input_val, int *output_val);

#endif /* __ISP_COMM_SNS_H__ */
