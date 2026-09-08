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
 * @file hal_psram.h
 * @brief PSRAM Hardware Abstraction Layer
 *
 * This header provides PSRAM initialization and calibration APIs.
 */

#ifndef __HAL_PSRAM_H__
#define __HAL_PSRAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <generated/autoconf.h>

#define HAL_PSRAM_ID_WINBOND_8MB    0x0c86
#define HAL_PSRAM_DQS_TABLE_SIZE    16

#ifdef CONFIG_USE_PSRAM
/**
 * @brief Initialize PSRAM GPIO pins
 */
void hal_psram_gpio_init(void);

/**
 * @brief Get DQS delay value from calibration table
 *
 * @param index Table index (0-15)
 * @return uint16_t DQS delay value
 */
uint16_t hal_psram_get_dqs_value(uint8_t index);

/**
 * @brief Get the actual DQS configuration after calibration
 *
 * @return uint16_t DQS config (left_flag << 4 | right_flag)
 */
uint16_t hal_psram_get_dqs_cfg(void);

/**
 * @brief Set the DQS configuration (for boot2 PSRAM-only path)
 *
 * @param cfg DQS config value (left_flag << 4 | right_flag)
 */
void hal_psram_set_dqs_cfg(uint16_t cfg);

/**
 * @brief Initialize Winbond PSRAM with DQS delay
 *
 * @param burst_len Burst length configuration
 * @param is_fixLatency Fixed latency enable
 * @param latency Latency value
 * @param dqs_delay DQS delay value
 * @return uint16_t PSRAM ID read from device
 */
uint16_t hal_psram_winbond_init_dqs(int8_t burst_len, uint8_t is_fixLatency,
                                    uint8_t latency, uint16_t dqs_delay);

void hal_psram_enter_hybridsleep(void);
void hal_psram_enter_deepsleep(void);
void hal_psram_exit_hybridsleep(void);

/**
 * @brief Perform PSRAM read/write check
 *
 * @return uint8_t SUCCESS or ERROR
 */
uint8_t hal_psram_rw_check(void);

/**
 * @brief Perform x8 PSRAM calibration
 *
 * @param psram_dqs_cfg DQS configuration from efuse (pass in from hal_boot2_get_psram_dqs_cfg())
 * @param psram_dqs_win_num Output: DQS window number
 * @param new_dqs_cfg Output: New DQS configuration (for efuse write if needed)
 * @return uint16_t PSRAM ID on success, ERROR on failure
 */
uint16_t hal_psram_x8_calibration(uint16_t psram_dqs_cfg, int32_t *psram_dqs_win_num, uint16_t *new_dqs_cfg);

/**
 * @brief Main PSRAM initialization entry point
 *
 * @param psram_dqs_cfg DQS configuration from efuse
 * @return uint32_t PSRAM ID on success, ERROR on failure
 */
uint32_t hal_psram_init(uint16_t psram_dqs_cfg);

/**
 * @brief Print PSRAM calibration log saved during early boot
 *
 * This function prints the calibration log that was saved to heap buffer
 * before UART was initialized. Should be called after UART init.
 */
void hal_psram_print_calibration_log(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PSRAM_H__ */
