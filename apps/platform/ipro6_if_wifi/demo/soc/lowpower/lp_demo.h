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
 * @file lp_demo.h
 * @brief IPRO6 Low Power demo — PDS Level 31 + DTIM wake (No BLE, No PSRAM)
 *
 * Ported into ipro6_demo from the standalone ipro_lp_test app. Gated by
 * CONFIG_DEMO_LOWPOWER; provides the `lp` shell command and the PDS/HBN app
 * flow (staged lp_fw Stage-1 image + DTIM re-sleep simulation).
 */

#ifndef __LP_DEMO_H__
#define __LP_DEMO_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Bring up the LP demo app flow.
 *
 * Initializes the hal_lp framework, stages the embedded lp_fw Stage-1 image to
 * its retained-OCRAM run address, registers the UART/SysTick wakeup hook, and
 * prints the boot-time HBN-RAM retention probe. Call once from main() after the
 * heap is up.
 */
void lp_demo_app_init(void);

/**
 * @brief Idle-hook entry — enters PDS when auto sleep is enabled.
 *
 * Call from vApplicationIdleHook(). No-op unless `lp auto on` was issued.
 */
void lp_demo_idle(void);

/**
 * @brief Enter PDS Level 31 sleep (DTIM simulation)
 * @param sleep_ms Sleep duration per DTIM cycle, in milliseconds
 * @param cnt      DTIM re-sleep cycles before waking the full firmware
 *                 (0 => LP_FW_SIM_DTIM_CYCLES default)
 */
void lp_demo_sleep(uint32_t sleep_ms, uint32_t cnt);

/**
 * @brief Enable/disable automatic PDS in the idle hook
 */
void lp_demo_auto_enable(bool enable);

/**
 * @brief Check if auto PDS is enabled
 */
bool lp_demo_auto_is_enabled(void);

/**
 * @brief Get sleep statistics
 */
void lp_demo_get_stats(uint32_t *total_sleep_ms,
                       uint32_t *sleep_count,
                       uint32_t *last_sleep_ms,
                       int32_t *last_wakeup_reason);

/**
 * @brief Reset statistics
 */
void lp_demo_reset_stats(void);

#endif /* __LP_DEMO_H__ */
