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
 * @file ipro_log_example.c
 * @brief IPRO Log System Usage Example
 * 
 * This file demonstrates how to use the IPRO logging system.
 */

#include "ipro_log.h"
#include "FreeRTOS.h"
#include "task.h"

#define LOG_TAG "LOG_EXAMPLE"

/**
 * @brief Example function showing different log levels
 */
void log_levels_example(void)
{
    IPRO_LOGI(LOG_TAG, "=== Log Levels Example ===");
    
    // Different log levels
    IPRO_LOGE(LOG_TAG, "This is an ERROR message (critical issues)");
    IPRO_LOGW(LOG_TAG, "This is a WARNING message (potential problems)");
    IPRO_LOGI(LOG_TAG, "This is an INFO message (general information)");
    IPRO_LOGD(LOG_TAG, "This is a DEBUG message (debugging info)");
    IPRO_LOGV(LOG_TAG, "This is a VERBOSE message (detailed debugging)");
    
    // Log with variables
    int error_code = 42;
    const char* status = "active";
    IPRO_LOGI(LOG_TAG, "System status: %s, error code: %d", status, error_code);
}

/**
 * @brief Example function showing buffer dumping
 */
void buffer_dump_example(void)
{
    IPRO_LOGI(LOG_TAG, "=== Buffer Dump Example ===");
    
    // Sample data buffer
    uint8_t test_data[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\0'
    };
    
    // Hex dump
    IPRO_LOGI(LOG_TAG, "Hex dump of test data:");
    IPRO_LOG_BUFFER_HEX(LOG_TAG, test_data, sizeof(test_data));
    
    // Hex + ASCII dump
    IPRO_LOGI(LOG_TAG, "Hex + ASCII dump of test data:");
    IPRO_LOG_BUFFER_HEXDUMP(LOG_TAG, test_data, sizeof(test_data));
}

/**
 * @brief Example function showing performance measurement
 */
void performance_measurement_example(void)
{
    IPRO_LOGI(LOG_TAG, "=== Performance Measurement Example ===");
    
    // Measure a simple operation
    IPRO_LOG_PERF_START(LOG_TAG, task_delay);
    vTaskDelay(pdMS_TO_TICKS(100));  // 100ms delay
    IPRO_LOG_PERF_END(LOG_TAG, task_delay);
    
    // Measure a computation
    IPRO_LOG_PERF_START(LOG_TAG, computation);
    volatile uint32_t sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    IPRO_LOG_PERF_END(LOG_TAG, computation);
    
    IPRO_LOGI(LOG_TAG, "Computation result: %lu", sum);
}

/**
 * @brief Example function showing runtime log level control
 */
void runtime_control_example(void)
{
    IPRO_LOGI(LOG_TAG, "=== Runtime Control Example ===");
    
    // Show current log level
    ipro_log_level_t current_level = ipro_log_get_level();
    IPRO_LOGI(LOG_TAG, "Current log level: %d", current_level);
    
    // Test all log levels
    IPRO_LOGI(LOG_TAG, "Testing all log levels:");
    IPRO_LOGE(LOG_TAG, "ERROR level test");
    IPRO_LOGW(LOG_TAG, "WARN level test");
    IPRO_LOGI(LOG_TAG, "INFO level test");
    IPRO_LOGD(LOG_TAG, "DEBUG level test");
    IPRO_LOGV(LOG_TAG, "VERBOSE level test");
    
    // Change log level to INFO only
    IPRO_LOGI(LOG_TAG, "Changing log level to INFO only...");
    ipro_log_set_level(IPRO_LOG_LEVEL_INFO);
    
    // Test again - DEBUG and VERBOSE should not appear
    IPRO_LOGI(LOG_TAG, "Testing after level change:");
    IPRO_LOGE(LOG_TAG, "ERROR level test (should appear)");
    IPRO_LOGW(LOG_TAG, "WARN level test (should appear)");
    IPRO_LOGI(LOG_TAG, "INFO level test (should appear)");
    IPRO_LOGD(LOG_TAG, "DEBUG level test (should NOT appear)");
    IPRO_LOGV(LOG_TAG, "VERBOSE level test (should NOT appear)");
    
    // Restore original level
    ipro_log_set_level(current_level);
    IPRO_LOGI(LOG_TAG, "Log level restored to: %d", current_level);
}

/**
 * @brief Example function showing conditional logging
 */
void conditional_logging_example(void)
{
    IPRO_LOGI(LOG_TAG, "=== Conditional Logging Example ===");
    
    // Check if debug level is enabled before expensive operations
    if (ipro_log_level_enabled(IPRO_LOG_LEVEL_DEBUG)) {
        // This expensive operation only runs if debug logging is enabled
        volatile uint32_t expensive_result = 0;
        for (int i = 0; i < 100000; i++) {
            expensive_result += i * i;
        }
        IPRO_LOGD(LOG_TAG, "Expensive debug operation result: %lu", expensive_result);
    } else {
        IPRO_LOGI(LOG_TAG, "Debug logging disabled, skipping expensive operation");
    }
}

/**
 * @brief Main example task
 */
void log_example_task(void *pvParameters)
{
    (void)pvParameters;
    
    // Initialize log system with custom configuration
    ipro_log_config_t config = IPRO_LOG_DEFAULT_CONFIG();
    config.global_level = IPRO_LOG_LEVEL_VERBOSE;  // Enable all log levels
    config.enable_colors = true;
    config.enable_timestamp = true;
    config.enable_thread_id = true;
    
    if (!ipro_log_init(&config)) {
        printf("Failed to initialize log system!\n");
        vTaskDelete(NULL);
        return;
    }
    
    IPRO_LOGI(LOG_TAG, "IPRO Log System Example Started");
    IPRO_LOGI(LOG_TAG, "=====================================");
    
    while (1) {
        // Run different examples
        log_levels_example();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        buffer_dump_example();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        performance_measurement_example();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        runtime_control_example();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        conditional_logging_example();
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        IPRO_LOGI(LOG_TAG, "=== Example cycle completed, waiting 5 seconds ===");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
