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
 * @file demo_ai.h
 * @brief AI Demo Public API
 *
 * This file provides public APIs for AI inference operations,
 * including external buffer management for continuous inference.
 */

#ifndef __DEMO_AI_H__
#define __DEMO_AI_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize AI demo subsystem
 *
 * This function is called from main.c during initialization
 */
void demo_ai_init(void);

/**
 * @brief Push an external buffer address to the AI inference queue
 *
 * This function allows external modules to push image buffer addresses
 * for AI inference. The buffer must contain RGB888 format image data
 * with dimensions 640x360x3.
 *
 * @param addr Physical address of the RGB888 image buffer
 *
 * @return 0 on success
 * @return -1 if external inference queue is not created (ai_run_ext not started)
 * @return -2 if queue is full (timeout waiting for space)
 *
 * @note This function should only be called after starting continuous mode:
 *       ai_run_ext <model_index> continuous
 *
 * @example
 *   uint32_t buffer_addr = 0x52100000;
 *   int ret = ai_ext_push_buffer(buffer_addr);
 *   if (ret != 0) {
 *       printf("Failed to push buffer: %d\n", ret);
 *   }
 */
int ai_ext_push_buffer(uint32_t addr);

/**
 * @brief Check if external inference task is running
 *
 * @return 1 if external inference task is running
 * @return 0 if task is not running or not started
 */
int ai_ext_is_running(void);

/**
 * @brief Get available space in external buffer queue
 *
 * @return Number of free slots in the queue
 * @return -1 if queue is not created
 */
int ai_ext_get_queue_space(void);

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_AI_H__ */
