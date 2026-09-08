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

#ifndef __HAL_SYS__H__
#define __HAL_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "drv_common.h"

/*============================ Reset Reason ===================================*/

/**
 * @brief System reset reason codes
 *
 * Detection hierarchy (checked in order):
 *   1. Power-off boot: HBN RAM magic pattern not found
 *   2. WDT reboot: WDT_GetResetStatus() returns 1
 *   3. Low power wakeup: PDS wakeup interrupt detected
 *   4. Software/Exception: Read from HBN RAM (set before reboot)
 */
typedef enum {
    HAL_RST_REASON_UNKNOWN      = 0,    /**< Unknown or uninitialized */
    HAL_RST_REASON_POWER_ON     = 1,    /**< Power-off boot (cold boot) */
    HAL_RST_REASON_WDT          = 2,    /**< Watchdog timer reboot */
    HAL_RST_REASON_EXCEPTION    = 3,    /**< CPU exception reboot */
    HAL_RST_REASON_SOFTWARE     = 4,    /**< Software reboot (hal_reboot) */
    HAL_RST_REASON_LP_WAKEUP    = 5,    /**< Low power wakeup (PDS) */
    HAL_RST_REASON_AON_WDT      = 6,    /**< AON watchdog timeout (HBN-domain wakeup recovery) */
} hal_rst_reason_t;

/**
 * @brief Get reset reason string
 * @param reason Reset reason code
 * @return String representation of reset reason
 */
const char *hal_rst_reason_str(hal_rst_reason_t reason);

/**
 * @brief Get the detected reset reason
 *
 * This returns the reset reason detected during boot.
 * The detection is performed in SystemInit and stored in OCRAM.
 *
 * @return Reset reason code
 */
hal_rst_reason_t hal_rst_reason_get(void);

/**
 * @brief Set reset reason before reboot
 *
 * This function stores the reset reason in HBN RAM before
 * triggering a reboot. Called internally by hal_reboot() and
 * exception handler (if enabled).
 *
 * @param reason Reset reason to set
 */
void hal_rst_reason_set(hal_rst_reason_t reason);

/**
 * @brief Detect and initialize reset reason at boot
 *
 * This function is called during SystemInit to:
 *   1. Check HBN RAM magic for cold boot detection
 *   2. Check WDT reset status
 *   3. Check PDS wakeup status
 *   4. Read software-set reason from HBN RAM
 *   5. Clear HBN RAM reset reason and interrupts
 *
 * @return Detected reset reason
 */
hal_rst_reason_t hal_rst_reason_detect(void);

/*============================ Legacy API (deprecated) ========================*/

/* Legacy enum values for backward compatibility */
enum {
    SYSTEM_RST_POR      = 0,
    SYSTEM_RST_BOR      = 1,
    SYSTEM_RST_WDT      = 2,
    SYSTEM_RST_HBN      = 3,
    SYSTEM_RST_SOFTWARE = 4,
};

/*============================ System Functions ===============================*/

void hal_reboot(void);
void hal_reboot_download(void);
void hal_sys_reset(void);
void hal_poweroff(void);
int hal_rc32k_cal(void);
int hal_rc32k_force_cal(void);
int hal_rc32k_sw_cal(void);
int hal_rc32k_measure_error_ppm(uint32_t window_us);
int hal_rc32k_measure_cdet_ppm(uint32_t *out_t_count, uint32_t *out_g_count);
int hal_rc32m_cal(void);

/*============================ Clock Gate & Reset =============================*/

/**
 * @brief Peripheral module identifiers for clock gating and reset
 */
typedef enum {
    HAL_MODULE_SEC_ENG = 0,     /**< Security Engine (AES/SHA/TRNG/PKA) */
    HAL_MODULE_USB,             /**< USB controller */
    HAL_MODULE_EMAC,            /**< Ethernet MAC */
    HAL_MODULE_PKA,             /**< Public Key Accelerator */
    HAL_MODULE_MAX
} hal_module_t;

/**
 * @brief Enable clock for a peripheral module
 * @param module Module identifier
 * @return 0 on success, negative on error
 */
int hal_clock_enable(hal_module_t module);

/**
 * @brief Disable clock for a peripheral module
 * @param module Module identifier
 * @return 0 on success, negative on error
 */
int hal_clock_disable(hal_module_t module);

/**
 * @brief Software reset a peripheral module
 * @param module Module identifier
 * @return 0 on success, negative on error
 */
int hal_module_reset(hal_module_t module);

#ifdef __cplusplus
}
#endif

#endif // __HAL_SYS__H__
