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
 * @file ipro_log.c
 * @brief IPRO SDK Logging System Implementation
 * 
 * @author IPRO SDK Team
 * @version 1.0
 */

#include "ipro_log.h"
#include <string.h>
#include <time.h>

#ifdef CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

// Global log configuration
static ipro_log_config_t g_log_config = IPRO_LOG_DEFAULT_CONFIG();
static bool g_log_initialized = false;
static ipro_log_output_func_t g_custom_output_func = NULL;

#ifdef CONFIG_FREERTOS
static SemaphoreHandle_t g_log_mutex = NULL;
/* Static backing store pinned to .ocram_bss. The log mutex is taken on
 * EVERY log line, so on LP builds with CONFIG_OCRAM_NORET_SPLIT it must
 * live in the HW-retained 64 KB OCRAM region — a dynamic (heap) mutex
 * lands in the power-gated ocram_noret region and a retention bit-flip
 * in its uxItemSize trips configASSERT in xQueueSemaphoreTake
 * (queue.c:1670) after enough PDS31 cycles. On non-split builds
 * .ocram_bss is just regular OCRAM, so this is harmless. */
static __attribute__((section(".ocram_bss"))) StaticSemaphore_t g_log_mutex_buf;
#endif

// Internal functions
static void ipro_log_default_output(ipro_log_level_t level, const char *tag, 
                                   const char *format, va_list args);
static const char* ipro_log_level_to_string(ipro_log_level_t level);
static const char* ipro_log_level_to_color_string(ipro_log_level_t level);
static uint32_t ipro_log_get_timestamp(void);
static uint32_t ipro_log_get_thread_id(void);

/**
 * @brief Initialize the logging system
 */
bool ipro_log_init(const ipro_log_config_t *config)
{
    if (g_log_initialized) {
        return true;
    }

    // Use provided config or default
    if (config != NULL) {
        memcpy(&g_log_config, config, sizeof(ipro_log_config_t));
    }

#ifdef CONFIG_FREERTOS
    // Create mutex for thread safety. Static allocation so the backing
    // store can be pinned to retained OCRAM (survives PDS31) — see decl.
    g_log_mutex = xSemaphoreCreateMutexStatic(&g_log_mutex_buf);
    if (g_log_mutex == NULL) {
        return false;
    }
#endif

    g_log_initialized = true;
    
    // Log system initialization message
    IPRO_LOGI("LOG", "IPRO Log system initialized (level: %s, colors: %s, timestamp: %s)",
              ipro_log_level_to_string(g_log_config.global_level),
              g_log_config.enable_colors ? "ON" : "OFF",
              g_log_config.enable_timestamp ? "ON" : "OFF");

    return true;
}

/**
 * @brief Deinitialize the logging system
 */
void ipro_log_deinit(void)
{
    if (!g_log_initialized) {
        return;
    }

    IPRO_LOGI("LOG", "IPRO Log system deinitialized");

#ifdef CONFIG_FREERTOS
    if (g_log_mutex != NULL) {
        vSemaphoreDelete(g_log_mutex);
        g_log_mutex = NULL;
    }
#endif

    g_log_initialized = false;
    g_custom_output_func = NULL;
}

/**
 * @brief Set global log level
 */
void ipro_log_set_level(ipro_log_level_t level)
{
    if (level < IPRO_LOG_LEVEL_NONE || level >= IPRO_LOG_LEVEL_MAX) {
        return;
    }
    
    g_log_config.global_level = level;
    IPRO_LOGI("LOG", "Log level changed to: %s", ipro_log_level_to_string(level));
}

/**
 * @brief Get current global log level
 */
ipro_log_level_t ipro_log_get_level(void)
{
    return g_log_config.global_level;
}

/**
 * @brief Set custom output function
 */
void ipro_log_set_output_func(ipro_log_output_func_t func)
{
    g_custom_output_func = func;
}

/**
 * @brief Check if log level is enabled
 */
bool ipro_log_level_enabled(ipro_log_level_t level)
{
    return (g_log_initialized && level <= g_log_config.global_level);
}

/**
 * @brief Core log function
 */
void ipro_log_write(ipro_log_level_t level, const char *tag, const char *format, ...)
{
    if (!ipro_log_level_enabled(level)) {
        return;
    }

    va_list args;
    va_start(args, format);
    ipro_log_writev(level, tag, format, args);
    va_end(args);
}

/**
 * @brief Core log function with va_list
 */
void ipro_log_writev(ipro_log_level_t level, const char *tag, const char *format, va_list args)
{
    if (!ipro_log_level_enabled(level)) {
        return;
    }

#ifdef CONFIG_FREERTOS
    /* Thread safety. When the scheduler is suspended (vApplicationSleep
     * tickless-idle entry, hal_lp_register_wakeup_hook callback, etc.)
     * no other task can run, so skip the mutex — taking it with timeout
     * trips queue.c:1675 (configASSERT in xQueueGenericReceive: blocking
     * with scheduler suspended). xTaskGetSchedulerState() also reports
     * NOT_STARTED before vTaskStartScheduler so logs from very early
     * boot work too. */
    /* Nor from interrupt context.  xSemaphoreTake() with a timeout inside an
     * ISR blocks the *interrupted* task (pxCurrentTCB is whatever was running,
     * typically IDLE) on the mutex's event list and yields from the handler,
     * which wedges the scheduler - the console dies mid-line.  It only bites
     * when the ISR happens to interrupt a task that already holds the mutex,
     * so it hides for a long time.  Printing unlocked is the correct trade
     * here: interleaved output beats a dead system. */
    bool need_mutex = (g_log_mutex != NULL) &&
                      (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) &&
                      (xPortIsInsideInterrupt() == pdFALSE);
    if (need_mutex) {
        if (xSemaphoreTake(g_log_mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
            return; // Skip log if cannot acquire mutex
        }
    }
#endif

    // Use custom output function if available
    if (g_custom_output_func != NULL) {
        g_custom_output_func(level, tag, format, args);
    } else {
        ipro_log_default_output(level, tag, format, args);
    }

#ifdef CONFIG_FREERTOS
    if (need_mutex) {
        xSemaphoreGive(g_log_mutex);
    }
#endif
}

/**
 * @brief Default log output function
 */
static void ipro_log_default_output(ipro_log_level_t level, const char *tag, 
                                   const char *format, va_list args)
{
    char log_buffer[512];
    int offset = 0;
    
    // Build log prefix
    if (g_log_config.enable_colors) {
        offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset,
                          "%s", ipro_log_level_to_color_string(level));
    } else {
        offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset,
                          "%s", ipro_log_level_to_string(level));
    }

    // Add timestamp
    if (g_log_config.enable_timestamp) {
        uint32_t timestamp = ipro_log_get_timestamp();
        offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset,
                          " (%lu)", timestamp);
    }

    // Add thread ID
    if (g_log_config.enable_thread_id) {
        uint32_t thread_id = ipro_log_get_thread_id();
        offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset,
                          " [%lu]", thread_id);
    }

    // Add module tag
    if (g_log_config.enable_module_tag && tag != NULL) {
        offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset,
                          " %s:", tag);
    }

    // Add separator
    offset += snprintf(log_buffer + offset, sizeof(log_buffer) - offset, " ");

    // Add user message
    offset += vsnprintf(log_buffer + offset, sizeof(log_buffer) - offset, format, args);

    // Add newline if not present
    if (offset > 0 && log_buffer[offset - 1] != '\n') {
        if (offset < sizeof(log_buffer) - 1) {
            log_buffer[offset++] = '\n';
            log_buffer[offset] = '\0';
        }
    }

    // Output based on configuration
    switch (g_log_config.output) {
        case IPRO_LOG_OUTPUT_UART:
        default:
            printf("%s", log_buffer);
            break;
        
        case IPRO_LOG_OUTPUT_RTT:
            // TODO: Implement RTT output
            printf("%s", log_buffer);
            break;
            
        case IPRO_LOG_OUTPUT_FILE:
            // TODO: Implement file output
            printf("%s", log_buffer);
            break;
    }
}

/**
 * @brief Convert log level to string
 */
static const char* ipro_log_level_to_string(ipro_log_level_t level)
{
    switch (level) {
        case IPRO_LOG_LEVEL_ERROR:   return "E";
        case IPRO_LOG_LEVEL_WARN:    return "W";
        case IPRO_LOG_LEVEL_INFO:    return "I";
        case IPRO_LOG_LEVEL_DEBUG:   return "D";
        case IPRO_LOG_LEVEL_VERBOSE: return "V";
        default:                     return "?";
    }
}

/**
 * @brief Convert log level to colored string
 */
static const char* ipro_log_level_to_color_string(ipro_log_level_t level)
{
    switch (level) {
        case IPRO_LOG_LEVEL_ERROR:   return IPRO_LOG_COLOR_E;
        case IPRO_LOG_LEVEL_WARN:    return IPRO_LOG_COLOR_W;
        case IPRO_LOG_LEVEL_INFO:    return IPRO_LOG_COLOR_I;
        case IPRO_LOG_LEVEL_DEBUG:   return IPRO_LOG_COLOR_D;
        case IPRO_LOG_LEVEL_VERBOSE: return IPRO_LOG_COLOR_V;
        default:                     return "?";
    }
}

/**
 * @brief Get current timestamp
 */
static uint32_t ipro_log_get_timestamp(void)
{
#ifdef CONFIG_FREERTOS
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
#else
    return 0; // TODO: Implement non-FreeRTOS timestamp
#endif
}

/**
 * @brief Get current thread ID
 */
static uint32_t ipro_log_get_thread_id(void)
{
#ifdef CONFIG_FREERTOS
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    return (uint32_t)task;
#else
    return 0;
#endif
}

/**
 * @brief Dump buffer in hex format
 */
void ipro_log_buffer_hex_internal(const char *tag, const void *buffer, 
                                  uint16_t length, ipro_log_level_t level)
{
    if (!ipro_log_level_enabled(level) || buffer == NULL || length == 0) {
        return;
    }

    const uint8_t *data = (const uint8_t *)buffer;
    char hex_buffer[128];
    int offset = 0;

    ipro_log_write(level, tag, "Buffer dump (%u bytes):", length);

    for (uint16_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            if (i > 0) {
                // Print previous line
                ipro_log_write(level, tag, "%s", hex_buffer);
            }
            // Start new line
            offset = snprintf(hex_buffer, sizeof(hex_buffer), "%04x: ", i);
        }

        offset += snprintf(hex_buffer + offset, sizeof(hex_buffer) - offset,
                          "%02x ", data[i]);

        // Add space every 8 bytes for readability
        if ((i + 1) % 8 == 0 && (i + 1) % 16 != 0) {
            offset += snprintf(hex_buffer + offset, sizeof(hex_buffer) - offset, " ");
        }
    }

    // Print last line if needed
    if (length > 0) {
        ipro_log_write(level, tag, "%s", hex_buffer);
    }
}

/**
 * @brief Dump buffer in hex and ASCII format
 */
void ipro_log_buffer_hexdump_internal(const char *tag, const void *buffer, 
                                      uint16_t length, ipro_log_level_t level)
{
    if (!ipro_log_level_enabled(level) || buffer == NULL || length == 0) {
        return;
    }

    const uint8_t *data = (const uint8_t *)buffer;
    char hex_buffer[128];
    char ascii_buffer[17];
    int hex_offset = 0;
    int ascii_offset = 0;

    ipro_log_write(level, tag, "Buffer hexdump (%u bytes):", length);

    for (uint16_t i = 0; i < length; i++) {
        if (i % 16 == 0) {
            if (i > 0) {
                // Print previous line
                ipro_log_write(level, tag, "%s  |%s|", hex_buffer, ascii_buffer);
            }
            // Start new line
            hex_offset = snprintf(hex_buffer, sizeof(hex_buffer), "%04x: ", i);
            ascii_offset = 0;
            memset(ascii_buffer, 0, sizeof(ascii_buffer));
        }

        // Add hex representation
        hex_offset += snprintf(hex_buffer + hex_offset, sizeof(hex_buffer) - hex_offset,
                              "%02x ", data[i]);

        // Add ASCII representation
        ascii_buffer[ascii_offset++] = (data[i] >= 32 && data[i] < 127) ? data[i] : '.';

        // Add space every 8 bytes for readability
        if ((i + 1) % 8 == 0 && (i + 1) % 16 != 0) {
            hex_offset += snprintf(hex_buffer + hex_offset, sizeof(hex_buffer) - hex_offset, " ");
        }
    }

    // Print last line if needed
    if (length > 0) {
        // Pad hex output if last line is incomplete
        while (hex_offset < 50) {
            hex_buffer[hex_offset++] = ' ';
        }
        hex_buffer[hex_offset] = '\0';
        
        ipro_log_write(level, tag, "%s  |%s|", hex_buffer, ascii_buffer);
    }
}
