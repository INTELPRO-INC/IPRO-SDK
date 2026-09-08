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
 * @file ble_demo.c
 * @brief Basic BLE demonstration for ipro7_demo
 * 
 * This module provides CLI commands for BLE functionality.
 * The actual BLE implementation is in ble_demo_app.c
 * 
 * CLI Commands:
 * - ble_info: Display BLE controller information
 * - ble_version: Display BLE stack version
 * - ble_addr: Display Bluetooth device address
 * - ble_peripheral: Start as BLE peripheral (advertising, auto data transfer)
 * - ble_central: Start as BLE central (scanning, connecting, auto data transfer)
 * - ble_stats: Display connection statistics
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include "ble_lib_api.h"
#include "ble_demo.h"
#include "shell.h"
#include "ipro_log.h"

#define LOG_TAG "BLE_DEMO"

/*
 * EXTERNAL FUNCTIONS
 ****************************************************************************************
 */

extern void app_init(void);

/*
 * TYPES & DATA STRUCTURES
 ****************************************************************************************
 */

static struct {
    bool initialized;
    uint8_t bd_addr[6];  // Bluetooth Device Address
} ble_demo_env;

/*
 * HELPER FUNCTIONS
 ****************************************************************************************
 */

/*
 * INITIALIZATION
 ****************************************************************************************
 */

/**
 * @brief Initialize BLE demo
 * 
 * Initializes the BLE controller.
 * Must be called before using any BLE functionality.
 */
int ble_demo_init(void)
{
    if (ble_demo_env.initialized) {
        IPRO_LOGW(LOG_TAG, "BLE demo already initialized", "");
        return 0;
    }
    
    IPRO_LOGI(LOG_TAG, "Initializing BLE demo...", "");
    
    // Initialize BLE controller
    // Parameters: task_priority
    ble_controller_init(configMAX_PRIORITIES - 3);
    
    IPRO_LOGI(LOG_TAG, "BLE controller initialized successfully", "");
    
    // Initialize BLE application
    app_init();
    
    ble_demo_env.initialized = true;
    
    IPRO_LOGI(LOG_TAG, "BLE demo initialization complete", "");
    
    return 0;
}

/*
 * CLI COMMANDS
 ****************************************************************************************
 */



