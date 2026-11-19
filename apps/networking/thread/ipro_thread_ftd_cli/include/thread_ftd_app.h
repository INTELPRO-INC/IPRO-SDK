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
 * @file thread_ftd_app.h
 * @brief IPRO Thread FTD CLI Application Header
 * 
 * This header defines the interface for the Thread FTD application.
 */

#ifndef THREAD_FTD_APP_H
#define THREAD_FTD_APP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the Thread FTD application
 * 
 * This function initializes OpenThread and starts the Thread network interface.
 * 
 * @return 0 on success, negative error code on failure
 */
int thread_ftd_app_init(void);

/**
 * @brief Start the Thread FTD application
 * 
 * This function starts the Thread network and enables routing.
 * 
 * @return 0 on success, negative error code on failure
 */
int thread_ftd_app_start(void);

/**
 * @brief Stop the Thread FTD application
 * 
 * This function stops the Thread network.
 * 
 * @return 0 on success, negative error code on failure
 */
int thread_ftd_app_stop(void);

/**
 * @brief Get the Thread network state
 * 
 * @return true if Thread network is active, false otherwise
 */
bool thread_ftd_app_is_active(void);

/**
 * @brief Process Thread stack events
 * 
 * This function should be called periodically to process OpenThread events.
 */
void thread_ftd_app_process(void);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_FTD_APP_H */
