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

#ifndef __HAL_AON_WDT__H__
#define __HAL_AON_WDT__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * AON Watchdog Timer (Always-On WDT)
 *
 * Clocked by 32KHz (RC32K or XTAL32K), survives PDS/HBN sleep modes.
 * Timeout = 2^(mask_bit + 1) / 32768 seconds.
 *
 * Common mask_bit values:
 *   15 ->   2s      18 ->  16s      21 -> 128s
 *   16 ->   4s      19 ->  32s      22 -> 256s
 *   17 ->   8s      20 ->  64s
 *
 * Every step is a doubling, so there are no intermediate values: a "30 s" budget
 * means picking 32 s (mask 19), not configuring 30.
 */

#define AON_WDT_TIMEOUT_2S    15
#define AON_WDT_TIMEOUT_4S    16
#define AON_WDT_TIMEOUT_8S    17
#define AON_WDT_TIMEOUT_16S   18
#define AON_WDT_TIMEOUT_32S   19
#define AON_WDT_TIMEOUT_64S   20

/* Default timeout for system AON WDT */
#define AON_WDT_DEFAULT_TIMEOUT  AON_WDT_TIMEOUT_16S

/**
 * @brief Initialize and start the AON watchdog timer
 * @param mask_bit Timeout mask bit (0-63). Use AON_WDT_TIMEOUT_* defines.
 */
void aon_wdt_init(uint8_t mask_bit);

/**
 * @brief Feed (kick) the AON watchdog timer, resetting its counter
 */
void aon_wdt_feed(void);

/**
 * @brief Disable the AON watchdog timer
 */
void aon_wdt_disable(void);

/**
 * @brief Check if the last reset was caused by AON WDT
 * @return 1 if AON WDT triggered the reset, 0 otherwise
 */
int aon_wdt_get_reset_status(void);

/**
 * @brief Clear the AON WDT reset status flag
 */
void aon_wdt_clear_reset_status(void);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_AON_WDT__H__ */
