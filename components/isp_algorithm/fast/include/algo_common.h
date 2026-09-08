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

#ifndef __ALGO_COMMON_H__
#define __ALGO_COMMON_H__

#include "../../isp_firmware/include/isp_comm_sns.h"

#define DEBUG_ALGO_MODULE 1

typedef enum
{
    ALGO_LOG_AE = 0,
    ALGO_LOG_AWB,
    ALGO_LOG_WDR,
    ALGO_LOG_DEFOG,
    ALGO_LOG_NUM,
} ALGO_LOGBIT;

extern int algo_log_print_level;
extern uint8_t *algo_log_print_bit_ptr;

#ifndef ALGO_LOG_PRINT
#define ALGO_LOG_PRINT(debug_module, type, algo, ...) \
    (((debug_module && (algo_log_print_level != LOG_OFF) && ((uint8_t)algo_log_print_bit_ptr[algo])) && (type <= algo_log_print_level))  \
    ? ((void)printf(__VA_ARGS__)) \
    : (void)0)
#endif

#define AE_ALGO_LOGE(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_ERROR,    ALGO_LOG_AE, __VA_ARGS__))
#define AE_ALGO_LOGW(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_WARNING,  ALGO_LOG_AE, __VA_ARGS__))
#define AE_ALGO_LOGI(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_INFO,     ALGO_LOG_AE, __VA_ARGS__))
#define AE_ALGO_LOGD(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_DEBUG,    ALGO_LOG_AE, __VA_ARGS__))

#define AWB_ALGO_LOGE(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_ERROR,   ALGO_LOG_AWB, __VA_ARGS__))
#define AWB_ALGO_LOGW(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_WARNING, ALGO_LOG_AWB, __VA_ARGS__))
#define AWB_ALGO_LOGI(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_INFO,    ALGO_LOG_AWB, __VA_ARGS__))
#define AWB_ALGO_LOGD(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_DEBUG,   ALGO_LOG_AWB, __VA_ARGS__))

#define WDR_ALGO_LOGE(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_ERROR,   ALGO_LOG_WDR, __VA_ARGS__))
#define WDR_ALGO_LOGW(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_WARNING, ALGO_LOG_WDR, __VA_ARGS__))
#define WDR_ALGO_LOGI(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_INFO,    ALGO_LOG_WDR, __VA_ARGS__))
#define WDR_ALGO_LOGD(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_DEBUG,   ALGO_LOG_WDR, __VA_ARGS__))

#define DEFOG_ALGO_LOGE(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_ERROR,     ALGO_LOG_DEFOG, __VA_ARGS__))
#define DEFOG_ALGO_LOGW(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_WARNING,   ALGO_LOG_DEFOG, __VA_ARGS__))
#define DEFOG_ALGO_LOGI(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_INFO,      ALGO_LOG_DEFOG, __VA_ARGS__))
#define DEFOG_ALGO_LOGD(...) ((void)ALGO_LOG_PRINT(DEBUG_ALGO_MODULE, LOG_DEBUG,     ALGO_LOG_DEFOG, __VA_ARGS__))

int ALGO_DBG_ChgLogPrintLevel(LOGTYPE level);
LOGTYPE ALGO_DBG_GetLogPrintLevel(void);
int ALGO_DBG_ChgLogPrintAlgo(ALGO_LOGBIT algo, Fun_Type enable);
Fun_Type ALGO_DBG_GetLogPrintAlgo(ALGO_LOGBIT algo);

#endif /* __ALGO_COMMON_H__ */


