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

/**
 * @file ipro_log_config.h
 * @brief IPRO SDK Logging System Configuration
 * 
 * This file contains compile-time configuration for the IPRO logging system.
 * Configuration values are typically set through Kconfig.
 * 
 * @author IPRO SDK Team
 * @version 1.0
 */

#ifndef IPRO_LOG_CONFIG_H
#define IPRO_LOG_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default configuration values
 * These can be overridden by Kconfig or compile-time defines
 */

// Default log level (if not configured)
#ifndef CONFIG_LOG_DEFAULT_LEVEL
#define CONFIG_LOG_DEFAULT_LEVEL 3  // INFO level (matches IPRO_LOG_LEVEL_INFO)
#endif

// Color support
#ifndef CONFIG_LOG_COLORS
#define CONFIG_LOG_COLORS 1
#endif

// Timestamp support
#ifndef CONFIG_LOG_TIMESTAMP
#define CONFIG_LOG_TIMESTAMP 1
#endif

// Thread ID support
#ifndef CONFIG_LOG_THREAD_ID
#define CONFIG_LOG_THREAD_ID 1
#endif

// Buffer size
#ifndef CONFIG_LOG_BUFFER_SIZE
#define CONFIG_LOG_BUFFER_SIZE 512
#endif

// Mutex support
#ifndef CONFIG_LOG_USE_MUTEX
#define CONFIG_LOG_USE_MUTEX 1
#endif

// FreeRTOS support detection
#if defined(configUSE_PREEMPTION) || defined(CONFIG_FREERTOS_SUPPORT)
#define CONFIG_FREERTOS 1
#endif

// Output type configuration
#ifndef CONFIG_LOG_OUTPUT_UART
#ifndef CONFIG_LOG_OUTPUT_RTT
#ifndef CONFIG_LOG_OUTPUT_FILE
#define CONFIG_LOG_OUTPUT_UART 1  // Default to UART
#endif
#endif
#endif

// Performance measurement support
#ifndef CONFIG_LOG_PERF_MEASUREMENT
#define CONFIG_LOG_PERF_MEASUREMENT 1
#endif

// Early logging support (before log system init)
#ifndef CONFIG_LOG_EARLY_ENABLE
#define CONFIG_LOG_EARLY_ENABLE 1
#endif

// Maximum tag length
#ifndef CONFIG_LOG_MAX_TAG_LENGTH
#define CONFIG_LOG_MAX_TAG_LENGTH 16
#endif

// Log format string maximum length
#ifndef CONFIG_LOG_MAX_FORMAT_LENGTH
#define CONFIG_LOG_MAX_FORMAT_LENGTH 256
#endif

/*
 * Feature enable/disable based on configuration
 */
#if CONFIG_LOG_DEFAULT_LEVEL > 0
#define IPRO_LOG_SYSTEM_ENABLE 1
#else
#define IPRO_LOG_SYSTEM_ENABLE 0
#endif

// Validate configuration
#if CONFIG_LOG_BUFFER_SIZE < 64
#error "Log buffer size must be at least 64 bytes"
#endif

#if CONFIG_LOG_BUFFER_SIZE > 4096
#warning "Large log buffer size may impact memory usage"
#endif

#ifdef __cplusplus
}
#endif

#endif /* IPRO_LOG_CONFIG_H */
