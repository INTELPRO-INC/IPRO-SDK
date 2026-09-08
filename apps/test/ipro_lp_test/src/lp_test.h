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
 * @file lp_test.h
 * @brief Low Power Test — PDS Level 31 (No BLE)
 */

#ifndef __LP_TEST_H__
#define __LP_TEST_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize LP test module
 *
 * Registers wakeup hook for UART/SysTick reinit.
 */
void lp_test_init(void);

/**
 * @brief Enter PDS Level 31 sleep
 * @param sleep_ms Sleep duration in milliseconds
 */
void lp_test_sleep(uint32_t sleep_ms);

/**
 * @brief Enable/disable automatic PDS in idle hook
 */
void lp_test_auto_enable(bool enable);

/**
 * @brief Check if auto PDS is enabled
 */
bool lp_test_auto_is_enabled(void);

/**
 * @brief Get sleep statistics
 */
void lp_test_get_stats(uint32_t *total_sleep_ms,
                       uint32_t *sleep_count,
                       uint32_t *last_sleep_ms,
                       int32_t *last_wakeup_reason);

/**
 * @brief Reset statistics
 */
void lp_test_reset_stats(void);

#endif /* __LP_TEST_H__ */
