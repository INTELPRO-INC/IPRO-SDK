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
 * @file ipro_log.h
 * @brief IPRO SDK Logging System
 * 
 * This file provides a comprehensive logging system for the IPRO SDK.
 * Features:
 * - Multiple log levels (ERROR, WARN, INFO, DEBUG, VERBOSE)
 * - Color output support
 * - Timestamp support
 * - Module tagging
 * - Runtime log level control
 * - Thread-safe operation
 * 
 * @author IPRO SDK Team
 * @version 1.0
 */

#ifndef IPRO_LOG_H
#define IPRO_LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "ipro_log_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log levels enumeration
 */
typedef enum {
    IPRO_LOG_LEVEL_NONE = 0,    ///< No logging
    IPRO_LOG_LEVEL_ERROR,       ///< Error conditions
    IPRO_LOG_LEVEL_WARN,        ///< Warning conditions
    IPRO_LOG_LEVEL_INFO,        ///< Informational messages
    IPRO_LOG_LEVEL_DEBUG,       ///< Debug-level messages
    IPRO_LOG_LEVEL_VERBOSE,     ///< Verbose debug messages
    IPRO_LOG_LEVEL_MAX
} ipro_log_level_t;

/**
 * @brief Log output destination
 */
typedef enum {
    IPRO_LOG_OUTPUT_UART = 0,   ///< UART output (default)
    IPRO_LOG_OUTPUT_RTT,        ///< RTT output
    IPRO_LOG_OUTPUT_FILE,       ///< File output
    IPRO_LOG_OUTPUT_CUSTOM      ///< Custom output function
} ipro_log_output_t;

/**
 * @brief Log configuration structure
 */
typedef struct {
    ipro_log_level_t global_level;     ///< Global log level
    ipro_log_output_t output;           ///< Output destination
    bool enable_colors;                 ///< Enable color output
    bool enable_timestamp;              ///< Enable timestamp
    bool enable_thread_id;              ///< Enable thread ID
    bool enable_module_tag;             ///< Enable module tag
    uint32_t buffer_size;               ///< Log buffer size
} ipro_log_config_t;

/**
 * @brief Custom log output function type
 * @param level Log level
 * @param tag Module tag
 * @param format Format string
 * @param args Variable arguments
 */
typedef void (*ipro_log_output_func_t)(ipro_log_level_t level, const char *tag, 
                                        const char *format, va_list args);

/*
 * Color codes for terminal output
 */
#if CONFIG_LOG_COLORS
#define IPRO_LOG_COLOR_BLACK   "30"
#define IPRO_LOG_COLOR_RED     "31"
#define IPRO_LOG_COLOR_GREEN   "32"
#define IPRO_LOG_COLOR_BROWN   "33"
#define IPRO_LOG_COLOR_BLUE    "34"
#define IPRO_LOG_COLOR_PURPLE  "35"
#define IPRO_LOG_COLOR_CYAN    "36"
#define IPRO_LOG_COLOR_WHITE   "37"

#define IPRO_LOG_COLOR_FORMAT(COLOR, FORMAT)  "\033[0;" COLOR "m" FORMAT "\033[0m"
#define IPRO_LOG_COLOR_E       IPRO_LOG_COLOR_FORMAT(IPRO_LOG_COLOR_RED, "E")
#define IPRO_LOG_COLOR_W       IPRO_LOG_COLOR_FORMAT(IPRO_LOG_COLOR_BROWN, "W")
#define IPRO_LOG_COLOR_I       IPRO_LOG_COLOR_FORMAT(IPRO_LOG_COLOR_GREEN, "I")
#define IPRO_LOG_COLOR_D       IPRO_LOG_COLOR_FORMAT(IPRO_LOG_COLOR_CYAN, "D")
#define IPRO_LOG_COLOR_V       IPRO_LOG_COLOR_FORMAT(IPRO_LOG_COLOR_WHITE, "V")
#else
#define IPRO_LOG_COLOR_E       "E"
#define IPRO_LOG_COLOR_W       "W"
#define IPRO_LOG_COLOR_I       "I"
#define IPRO_LOG_COLOR_D       "D"
#define IPRO_LOG_COLOR_V       "V"
#endif

/*
 * Core logging functions
 */

/**
 * @brief Initialize the logging system
 * @param config Log configuration (NULL for default)
 * @return true if successful, false otherwise
 */
bool ipro_log_init(const ipro_log_config_t *config);

/**
 * @brief Deinitialize the logging system
 */
void ipro_log_deinit(void);

/**
 * @brief Set global log level
 * @param level New log level
 */
void ipro_log_set_level(ipro_log_level_t level);

/**
 * @brief Get current global log level
 * @return Current log level
 */
ipro_log_level_t ipro_log_get_level(void);

/**
 * @brief Set custom output function
 * @param func Custom output function
 */
void ipro_log_set_output_func(ipro_log_output_func_t func);

/**
 * @brief Core log function
 * @param level Log level
 * @param tag Module tag
 * @param format Format string
 * @param ... Variable arguments
 */
void ipro_log_write(ipro_log_level_t level, const char *tag, const char *format, ...);

/**
 * @brief Core log function with va_list
 * @param level Log level
 * @param tag Module tag
 * @param format Format string
 * @param args Variable arguments list
 */
void ipro_log_writev(ipro_log_level_t level, const char *tag, const char *format, va_list args);

/**
 * @brief Check if log level is enabled
 * @param level Log level to check
 * @return true if enabled, false otherwise
 */
bool ipro_log_level_enabled(ipro_log_level_t level);

/*
 * Convenience macros for different log levels
 */
#define IPRO_LOG_LEVEL_LOCAL CONFIG_LOG_DEFAULT_LEVEL

#define IPRO_LOG_FORMAT(letter, format) letter " (%u) %s: " format
#define IPRO_LOG_SYSTEM_TIME_FORMAT(letter, format) letter " (%u) %s: " format

// Early return macros for performance
#define IPRO_EARLY_LOGE(tag, format, ...) do { \
    if ((1) <= CONFIG_LOG_DEFAULT_LEVEL) { \
        ipro_log_write(IPRO_LOG_LEVEL_ERROR, tag, format, ##__VA_ARGS__); \
    } } while(0)

#define IPRO_EARLY_LOGW(tag, format, ...) do { \
    if ((2) <= CONFIG_LOG_DEFAULT_LEVEL) { \
        ipro_log_write(IPRO_LOG_LEVEL_WARN, tag, format, ##__VA_ARGS__); \
    } } while(0)

#define IPRO_EARLY_LOGI(tag, format, ...) do { \
    if ((3) <= CONFIG_LOG_DEFAULT_LEVEL) { \
        ipro_log_write(IPRO_LOG_LEVEL_INFO, tag, format, ##__VA_ARGS__); \
    } } while(0)

#define IPRO_EARLY_LOGD(tag, format, ...) do { \
    if ((4) <= CONFIG_LOG_DEFAULT_LEVEL) { \
        ipro_log_write(IPRO_LOG_LEVEL_DEBUG, tag, format, ##__VA_ARGS__); \
    } } while(0)

#define IPRO_EARLY_LOGV(tag, format, ...) do { \
    if ((5) <= CONFIG_LOG_DEFAULT_LEVEL) { \
        ipro_log_write(IPRO_LOG_LEVEL_VERBOSE, tag, format, ##__VA_ARGS__); \
    } } while(0)

// Main logging macros
#define IPRO_LOGE(tag, format, ...) IPRO_EARLY_LOGE(tag, format, ##__VA_ARGS__)
#define IPRO_LOGW(tag, format, ...) IPRO_EARLY_LOGW(tag, format, ##__VA_ARGS__)
#define IPRO_LOGI(tag, format, ...) IPRO_EARLY_LOGI(tag, format, ##__VA_ARGS__)
#define IPRO_LOGD(tag, format, ...) IPRO_EARLY_LOGD(tag, format, ##__VA_ARGS__)
#define IPRO_LOGV(tag, format, ...) IPRO_EARLY_LOGV(tag, format, ##__VA_ARGS__)

// Conditional compilation macros based on log level
#if CONFIG_LOG_DEFAULT_LEVEL >= 1  // IPRO_LOG_LEVEL_ERROR
#define IPRO_LOG_ERROR_ENABLED 1
#else
#define IPRO_LOG_ERROR_ENABLED 0
#endif

#if CONFIG_LOG_DEFAULT_LEVEL >= 2  // IPRO_LOG_LEVEL_WARN
#define IPRO_LOG_WARN_ENABLED 1
#else
#define IPRO_LOG_WARN_ENABLED 0
#endif

#if CONFIG_LOG_DEFAULT_LEVEL >= 3  // IPRO_LOG_LEVEL_INFO
#define IPRO_LOG_INFO_ENABLED 1
#else
#define IPRO_LOG_INFO_ENABLED 0
#endif

#if CONFIG_LOG_DEFAULT_LEVEL >= 4  // IPRO_LOG_LEVEL_DEBUG
#define IPRO_LOG_DEBUG_ENABLED 1
#else
#define IPRO_LOG_DEBUG_ENABLED 0
#endif

#if CONFIG_LOG_DEFAULT_LEVEL >= 5  // IPRO_LOG_LEVEL_VERBOSE
#define IPRO_LOG_VERBOSE_ENABLED 1
#else
#define IPRO_LOG_VERBOSE_ENABLED 0
#endif

/*
 * Helper macros for buffer dumping
 */

/**
 * @brief Dump buffer in hex format
 * @param tag Module tag
 * @param buffer Buffer to dump
 * @param length Buffer length
 * @param level Log level
 */
void ipro_log_buffer_hex_internal(const char *tag, const void *buffer, 
                                  uint16_t length, ipro_log_level_t level);

/**
 * @brief Dump buffer in hex and ASCII format
 * @param tag Module tag
 * @param buffer Buffer to dump
 * @param length Buffer length
 * @param level Log level
 */
void ipro_log_buffer_hexdump_internal(const char *tag, const void *buffer, 
                                      uint16_t length, ipro_log_level_t level);

// Buffer dump macros
#define IPRO_LOG_BUFFER_HEX_LEVEL(tag, buffer, length, level) \
    do { \
        if (ipro_log_level_enabled(level)) \
            ipro_log_buffer_hex_internal(tag, buffer, length, level); \
    } while(0)

#define IPRO_LOG_BUFFER_HEXDUMP_LEVEL(tag, buffer, length, level) \
    do { \
        if (ipro_log_level_enabled(level)) \
            ipro_log_buffer_hexdump_internal(tag, buffer, length, level); \
    } while(0)

// Convenience macros for buffer dumping
#define IPRO_LOG_BUFFER_HEX(tag, buffer, length) \
    IPRO_LOG_BUFFER_HEX_LEVEL(tag, buffer, length, IPRO_LOG_LEVEL_INFO)

#define IPRO_LOG_BUFFER_HEXDUMP(tag, buffer, length) \
    IPRO_LOG_BUFFER_HEXDUMP_LEVEL(tag, buffer, length, IPRO_LOG_LEVEL_INFO)

/*
 * Performance measurement macros
 */
#if IPRO_LOG_DEBUG_ENABLED
#define IPRO_LOG_PERF_START(tag, name) \
    uint32_t _perf_start_##name = xTaskGetTickCount(); \
    IPRO_LOGD(tag, "PERF_START: %s", #name)

#define IPRO_LOG_PERF_END(tag, name) \
    uint32_t _perf_end_##name = xTaskGetTickCount(); \
    IPRO_LOGD(tag, "PERF_END: %s took %u ms", #name, \
             (_perf_end_##name - _perf_start_##name) * portTICK_PERIOD_MS)
#else
#define IPRO_LOG_PERF_START(tag, name)
#define IPRO_LOG_PERF_END(tag, name)
#endif

/*
 * Default configuration
 */
#define IPRO_LOG_DEFAULT_CONFIG() { \
    .global_level = CONFIG_LOG_DEFAULT_LEVEL, \
    .output = IPRO_LOG_OUTPUT_UART, \
    .enable_colors = CONFIG_LOG_COLORS, \
    .enable_timestamp = CONFIG_LOG_TIMESTAMP, \
    .enable_thread_id = true, \
    .enable_module_tag = true, \
    .buffer_size = 256 \
}

/*
 * Zephyr Logging API Compatibility
 * Only LOG_MODULE_REGISTER is defined here.
 * LOG_DBG/LOG_ERR/LOG_WRN/LOG_INF are provided by bt_log.h
 * to avoid redefinition errors.
 */
#include <errno.h>

#ifndef LOG_MODULE_REGISTER
#define LOG_MODULE_REGISTER(name, ...) /* no-op */
#endif

#ifndef LOG_MODULE_DECLARE
#define LOG_MODULE_DECLARE(name, ...) /* no-op */
#endif

#ifndef LOG_HEXDUMP_DBG
#define LOG_HEXDUMP_DBG(data, len, str) /* no-op */
#endif

/* Parameter validation macro (Zephyr sys/check.h) */
#ifndef CHECKIF
#define CHECKIF(expr) if (expr)
#endif

/* Compile-time assertion */
#ifndef BUILD_ASSERT
#define BUILD_ASSERT(expr, ...) _Static_assert(expr, "" __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* IPRO_LOG_H */
