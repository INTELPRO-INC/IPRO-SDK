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

#ifndef _WL_INTERNAL_H_
#define _WL_INTERNAL_H_
#include "wl_api.h"
#include "wl_config.h"
#include <stdio.h> // For the definition of __FILE__ and __LINE__

// Helper macro for creating log messages with the file name and line number
#define WL_LOG_FORMAT(level, format) "[%s:%d] " format, __FILE__, __LINE__

// Logging macros for various levels
#define WL_LOG_CLI(format, ...) wl_logger(WL_LOG_LEVEL_NONE, format, ##__VA_ARGS__)
#define WL_LOG_CRITICAL(format, ...) wl_logger(WL_LOG_LEVEL_CRITICAL, "[WL] " format, ##__VA_ARGS__)
#define WL_LOG_ERROR(format, ...) wl_logger(WL_LOG_LEVEL_ERROR, "[WL] " format, ##__VA_ARGS__)
#define WL_LOG_WARN(format, ...) wl_logger(WL_LOG_LEVEL_WARN, "[WL][WARN] " format, ##__VA_ARGS__)
#define WL_LOG_INFO(format, ...) wl_logger(WL_LOG_LEVEL_INFO, "[WL][INFO] " format, ##__VA_ARGS__)
#define RF_LOG_CRITICAL(format, ...) wl_logger(WL_LOG_LEVEL_CRITICAL, "[RF] " format, ##__VA_ARGS__)
#define RF_LOG_TRACE(format, ...) wl_logger(WL_LOG_LEVEL_TRACE, "[RF][TRACE] " format, ##__VA_ARGS__)
#define RF_LOG_ERROR(format, ...) wl_logger(WL_LOG_LEVEL_ERROR, "[RF] " format, ##__VA_ARGS__)
#define RF_LOG_WARN(format, ...) wl_logger(WL_LOG_LEVEL_WARN, "[RF][WARN] " format, ##__VA_ARGS__)
#define RF_LOG_INFO(format, ...) wl_logger(WL_LOG_LEVEL_INFO, "[RF][INFO] " format, ##__VA_ARGS__)
#define RF_LOG_DEBUG(format, ...) wl_logger(WL_LOG_LEVEL_DEBUG, "[RF][INFO] " format, ##__VA_ARGS__)

extern struct wl_cfg_t* wl_cfg;
extern uint8_t*         wl_cal;
void wl_logger(wl_log_level_t level, const char *format, ...);

#endif
