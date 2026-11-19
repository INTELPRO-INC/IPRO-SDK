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
 * @file thread_ftd_app.c
 * @brief IPRO Thread FTD Application Implementation
 * 
 * This file implements the Thread Full Thread Device application logic.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/thread_ftd.h>
#include <openthread/tasklet.h>
#include <openthread/platform/logging.h>
#include <openthread/cli.h>
#include <openthread/dataset.h>
#include <openthread/joiner.h>
#include <openthread/ip6.h>
#include <openthread/link.h>

#define OT_FREERTOS_ENABLE 1
#include "openthread_port.h"
#include "thread_ftd_app.h"
#include "thread_default_dataset.h"
#include "ipro_log.h"

#define LOG_TAG "THREAD_APP"

// Forward declarations
static void thread_ftd_init_task(void *pvParameters);

// Static allocations for init task
static StaticTask_t xInitTaskBuffer;
static StackType_t xInitStack[2048];

// OpenThread instance
static otInstance *s_ot_instance = NULL;
static bool s_thread_active = false;

/**
 * @brief OpenThread state change callback
 */
static void ot_state_changed_callback(otChangedFlags aFlags, void *aContext)
{
    otInstance *instance = (otInstance *)aContext;
    
    if (aFlags & OT_CHANGED_THREAD_ROLE) {
        otDeviceRole role = otThreadGetDeviceRole(instance);
        const char *role_str = "Unknown";
        
        switch (role) {
            case OT_DEVICE_ROLE_DISABLED:
                role_str = "Disabled";
                s_thread_active = false;
                break;
            case OT_DEVICE_ROLE_DETACHED:
                role_str = "Detached";
                s_thread_active = false;
                break;
            case OT_DEVICE_ROLE_CHILD:
                role_str = "Child";
                s_thread_active = true;
                break;
            case OT_DEVICE_ROLE_ROUTER:
                role_str = "Router";
                s_thread_active = true;
                break;
            case OT_DEVICE_ROLE_LEADER:
                role_str = "Leader";
                s_thread_active = true;
                break;
            default:
                break;
        }
        
        IPRO_LOGI(LOG_TAG, "Thread role changed: %s", role_str);
    }
    
    if (aFlags & OT_CHANGED_THREAD_NETDATA) {
        IPRO_LOGD(LOG_TAG, "Thread network data changed");
    }
}

/**
 * @brief Initialize OpenThread instance (runs in separate task after scheduler starts)
 */
static void thread_ftd_init_task(void *pvParameters)
{
    (void)pvParameters;
    
    IPRO_LOGI(LOG_TAG, "Initializing Thread FTD application...");
    
    // Start OpenThread stack (this creates the task and initializes everything)
    otrStart();
    
    // Get OpenThread instance (should be available after otrStart())
    s_ot_instance = otrGetInstance();
    if (s_ot_instance == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to get OpenThread instance");
        vTaskDelete(NULL);
        return;
    }
    
    IPRO_LOGI(LOG_TAG, "Got OpenThread instance: %p", s_ot_instance);
    
    // Register state change callback
    otSetStateChangedCallback(s_ot_instance, ot_state_changed_callback, s_ot_instance);
    
    IPRO_LOGI(LOG_TAG, "Thread FTD application initialized successfully");

#ifdef CONFIG_THREAD_AUTO_START
    /* Apply factory dataset (if not already commissioned) and bring up
     * the Thread interface. With CONFIG_THREAD_AUTO_START=y a freshly
     * flashed device joins the demo network without any CLI input. */
    if (thread_ftd_app_start() != 0) {
        IPRO_LOGE(LOG_TAG, "Auto-start failed");
    }
#endif

    printf("\r\nThread FTD CLI is ready. Type 'ot help' for OpenThread commands.\r\n\r\n");

    // Task done, delete itself
    vTaskDelete(NULL);
}

/**
 * @brief Create initialization task (called from main before scheduler starts)
 */
int thread_ftd_app_init(void)
{
    // Create a one-shot task to initialize OpenThread after scheduler starts
    TaskHandle_t initTask = xTaskCreateStatic(
        thread_ftd_init_task,
        "ot_init",
        2048,
        NULL,
        15,  // High priority to initialize quickly
        xInitStack,
        &xInitTaskBuffer
    );
    
    if (initTask == NULL) {
        IPRO_LOGE(LOG_TAG, "Failed to create init task");
        return -1;
    }
    
    return 0;
}


/**
 * @brief Start Thread network
 */
int thread_ftd_app_start(void)
{
    if (s_ot_instance == NULL) {
        IPRO_LOGE(LOG_TAG, "OpenThread instance not initialized");
        return -1;
    }
    
    /* Apply the compiled-in factory dataset if no dataset is committed.
     * Lets a freshly-flashed device auto-attach to the demo Thread
     * network without any provisioning step. */
    if (thread_apply_default_dataset(s_ot_instance) != 0) {
        IPRO_LOGE(LOG_TAG, "Failed to apply default Thread dataset");
        /* fall through — user may set dataset via CLI */
    }

    IPRO_LOGI(LOG_TAG, "Starting Thread network...");

#if defined(CONFIG_OPENTHREAD_MAC_CSL_RECEIVER_ENABLE) && defined(CONFIG_THREAD_CSL_PERIOD_US) && \
    CONFIG_THREAD_CSL_PERIOD_US > 0
    /* CSL is RAM-only in OT: set it before the child link is restored, so a
     * reset SSED re-synchronizes as a CSL receiver (Kconfig help). */
#if defined(CONFIG_THREAD_CSL_TIMEOUT_S) && CONFIG_THREAD_CSL_TIMEOUT_S > 0
    if (otLinkSetCslTimeout(s_ot_instance, CONFIG_THREAD_CSL_TIMEOUT_S) != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to set CSL timeout %u s", (unsigned)CONFIG_THREAD_CSL_TIMEOUT_S);
    }
#endif
    if (otLinkSetCslPeriod(s_ot_instance, CONFIG_THREAD_CSL_PERIOD_US) != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to set CSL period %u us", (unsigned)CONFIG_THREAD_CSL_PERIOD_US);
    }
#endif

    // Enable Thread interface
    otError error = otIp6SetEnabled(s_ot_instance, true);
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to enable IPv6 interface: %d", error);
        return -1;
    }
    
    // Start Thread protocol
    error = otThreadSetEnabled(s_ot_instance, true);
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to enable Thread: %d", error);
        return -1;
    }
    
    IPRO_LOGI(LOG_TAG, "Thread network started successfully");
    
    return 0;
}

/**
 * @brief Stop Thread network
 */
int thread_ftd_app_stop(void)
{
    if (s_ot_instance == NULL) {
        IPRO_LOGE(LOG_TAG, "OpenThread instance not initialized");
        return -1;
    }
    
    IPRO_LOGI(LOG_TAG, "Stopping Thread network...");
    
    // Disable Thread protocol
    otError error = otThreadSetEnabled(s_ot_instance, false);
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to disable Thread: %d", error);
        return -1;
    }
    
    // Disable Thread interface
    error = otIp6SetEnabled(s_ot_instance, false);
    if (error != OT_ERROR_NONE) {
        IPRO_LOGE(LOG_TAG, "Failed to disable IPv6 interface: %d", error);
        return -1;
    }
    
    s_thread_active = false;
    
    IPRO_LOGI(LOG_TAG, "Thread network stopped successfully");
    
    return 0;
}

/**
 * @brief Check if Thread network is active
 */
bool thread_ftd_app_is_active(void)
{
    return s_thread_active;
}

/**
 * @brief Process OpenThread stack events
 */
void thread_ftd_app_process(void)
{
    if (s_ot_instance != NULL) {
        // Process OpenThread tasklets
        otTaskletsProcess(s_ot_instance);
    }
}
