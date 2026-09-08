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
 * @file hal_lp.h
 * @brief Low Power HAL for IPRO7
 *
 * This module provides low power mode support including:
 * - RTC timer wakeup
 * - GPIO wakeup
 * - CPU state save/restore
 * - Virtual time tracking
 * - RC32K calibration
 * - Flash XIP recovery
 * - PSRAM restore
 */

#ifndef __HAL_LP_H__
#define __HAL_LP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <generated/autoconf.h>

/* HBN RAM layout definitions (chip level management) */
#include "hal_hbnram_mgnt.h"

/*============================ DEFINES ====================================*/

/* Memory addresses for LP - use centralized HBN RAM management */
#define HAL_LP_HBN_RAM_BASE         HAL_HBNRAM_LP_FW_ADDR
#define HAL_LP_PARAM_ADDR           HAL_HBNRAM_LP_PARAM_ADDR

/* Structure offsets for assembly access (must match hal_lp_param_t) */
#define HAL_LP_PARAM_SRAM_CFG_OFFSET    4   /**< Offset of sram_cfg in hal_lp_param_t */
#define HAL_LP_SRAM_CFG_REG             0x3000008c  /**< SRAM config register */

/* Wakeup reasons (bitmask) */
#define HAL_LP_WAKEUP_UNKNOWN       0
#define HAL_LP_WAKEUP_RTC           (1 << 0)
#define HAL_LP_WAKEUP_GPIO          (1 << 1)
#define HAL_LP_WAKEUP_ACOMP         (1 << 2)
#define HAL_LP_WAKEUP_BLE           (1 << 3)

/*
 * Time conversion macros (32.768kHz RTC clock)
 * Optimized: multiply + shift only, no division
 *
 * US_TO_RTC: us * 32768 / 1000000 ≈ (us * 67) >> 11  (0.16% error)
 * MS_TO_RTC: ms * 32768 / 1000 ≈ (ms * 33554) >> 10  (0.01% error)
 * RTC_TO_US: cnt * 1000000 / 32768 = (cnt * 1000000) >> 15  (exact, 32768=2^15)
 * RTC_TO_MS: cnt * 1000 / 32768 = (cnt * 1000) >> 15  (exact)
 */
#define HAL_LP_US_TO_RTC_CNT(us)    (((uint64_t)(us) * 67) >> 11)
#define HAL_LP_MS_TO_RTC_CNT(ms)    (((uint64_t)(ms) * 33554) >> 10)
#define HAL_LP_RTC_CNT_TO_US(cnt)   (((uint64_t)(cnt) * 1000000) >> 15)
#define HAL_LP_RTC_CNT_TO_MS(cnt)   (((uint64_t)(cnt) * 1000) >> 15)

/* PDS wakeup timing limits */
#define HAL_LP_PDS_WAKEUP_MIN_US    1220
#define HAL_LP_PDS_WAKEUP_DELAY_US  300

/* Runtime timing policy.
 *
 * The macros above are the driver defaults. Applications that have measured a
 * board/product-specific PDS wake path can opt in at runtime; callers that do
 * nothing keep the original HAL behaviour.
 */
#define HAL_LP_TIMING_POLICY_NONE                  (0U)
#define HAL_LP_TIMING_POLICY_EXACT_US_TO_RTC       (1U << 0)
#define HAL_LP_TIMING_POLICY_PDS_WAKE_DELAY_VALID  (1U << 1)

void hal_lp_set_timing_policy(uint32_t policy, uint32_t pds_wakeup_delay_us);
void hal_lp_get_timing_policy(uint32_t *policy, uint32_t *pds_wakeup_delay_us);
uint32_t hal_lp_us_to_rtc_cnt(uint64_t us);
uint32_t hal_lp_get_pds_wakeup_delay_us(void);

/* Maximum GPIO pins for wakeup */
#define HAL_LP_MAX_WAKEUP_GPIO      32

/* Saved registers configuration for LP restore */
#define HAL_LP_SAVED_REGS_MAX       16  /**< Maximum saved registers (extendable) */
#define HAL_LP_SAVED_REGS_COUNT     5   /**< Actual number of registers to save */

/* Register addresses to save/restore (indexed 0 to HAL_LP_SAVED_REGS_COUNT-1) */
#define HAL_LP_SAVED_REG_ADDR_0     0x30000128
#define HAL_LP_SAVED_REG_ADDR_1     0x30000090
#define HAL_LP_SAVED_REG_ADDR_2     0x300000c8
#define HAL_LP_SAVED_REG_ADDR_3     0x300000cc
#define HAL_LP_SAVED_REG_ADDR_4     0x3000008c  /**< SRAM config */

/*============================ TYPES ======================================*/

/**
 * @brief LP callback function type
 */
typedef int (*hal_lp_callback_t)(void *arg);

/**
 * @brief GPIO wakeup trigger mode
 */
typedef enum {
    HAL_LP_GPIO_TRIG_FALLING_EDGE = 0,  /**< Falling edge trigger */
    HAL_LP_GPIO_TRIG_RISING_EDGE,       /**< Rising edge trigger */
    HAL_LP_GPIO_TRIG_LOW_LEVEL,         /**< Low level trigger */
    HAL_LP_GPIO_TRIG_HIGH_LEVEL,        /**< High level trigger */
    HAL_LP_GPIO_TRIG_BOTH_EDGE,         /**< Both edge trigger */
    HAL_LP_GPIO_TRIG_NONE = 0xF,        /**< Disable trigger */
} hal_lp_gpio_trig_t;

/**
 * @brief GPIO pull resistor configuration
 */
typedef enum {
    HAL_LP_GPIO_RES_NONE = 0,           /**< No pull resistor */
    HAL_LP_GPIO_RES_PULL_UP,            /**< Pull up resistor */
    HAL_LP_GPIO_RES_PULL_DOWN,          /**< Pull down resistor */
} hal_lp_gpio_res_t;

/**
 * @brief GPIO wakeup configuration for a single pin
 */
typedef struct {
    uint8_t pin;                        /**< GPIO pin number */
    uint8_t enable;                     /**< Enable wakeup on this pin */
    hal_lp_gpio_trig_t trig_mode;       /**< Trigger mode */
    hal_lp_gpio_res_t res_mode;         /**< Pull resistor mode */
} hal_lp_gpio_pin_cfg_t;

/**
 * @brief GPIO wakeup configuration
 */
typedef struct {
    uint32_t wakeup_mask;               /**< Bitmask of GPIOs enabled for wakeup */
    hal_lp_gpio_trig_t default_trig;    /**< Default trigger mode for all pins */
    hal_lp_gpio_res_t default_res;      /**< Default pull resistor */
    hal_lp_gpio_pin_cfg_t *pin_cfg;     /**< Per-pin configuration (optional) */
    uint8_t pin_cfg_count;              /**< Number of per-pin configs */
} hal_lp_gpio_cfg_t;

/**
 * @brief Low power enter configuration
 */
typedef struct {
    /* RTC wakeup */
    uint8_t rtc_wakeup_en;              /**< Enable RTC timer wakeup */
    uint64_t rtc_sleep_us;              /**< RTC sleep time in microseconds */

    /* GPIO wakeup */
    uint8_t gpio_wakeup_en;             /**< Enable GPIO wakeup */
    hal_lp_gpio_cfg_t *gpio_cfg;        /**< GPIO wakeup configuration */

    /* Power options */
    uint8_t flash_power_down;           /**< Power down flash during sleep */
    uint8_t psram_retention;            /**< Enable PSRAM retention (hybrid sleep) */
    uint8_t ocram_retention;            /**< Enable OCRAM retention */

    /* Callbacks */
    hal_lp_callback_t pre_sleep_cb;     /**< Called before entering sleep */
    void *pre_sleep_arg;                /**< Argument for pre_sleep_cb */
    hal_lp_callback_t post_wakeup_cb;   /**< Called after waking up */
    void *post_wakeup_arg;              /**< Argument for post_wakeup_cb */
} hal_lp_cfg_t;

/**
 * @brief Low power result/status
 */
typedef struct {
    int32_t wakeup_reason;              /**< Wakeup reason bitmask */
    uint32_t wakeup_gpio_bits;          /**< GPIO bits that triggered wakeup */
    uint64_t sleep_duration_us;         /**< Actual sleep duration in us */
} hal_lp_result_t;

/**
 * @brief Internal LP parameter structure (stored in HBN RAM)
 *
 * NOTE: sram_cfg MUST be at offset 4 (immediately after pattern).
 * This is accessed by assembly code in hal_lp_entry.S during early wakeup.
 */
typedef struct {
    uint32_t pattern;                   /**< Magic pattern 0xAA5555AA (offset 0) */
    uint32_t sram_cfg;                  /**< SRAM config 0x3000008c (offset 4, restored in asm) */

    /* CPU state */
    uint32_t cpu_regs[32];              /**< Saved CPU registers (x0-x31) */
    uint32_t csr_mtvt;                  /**< CSR MTVT (0x307) - vector table base */
    uint32_t csr_mtvt2;                 /**< CSR MTVT2 (0x7EC) - non-vector entry */
    uint32_t csr_mtvec;                 /**< CSR MTVEC (0x305) - exception entry */
    uint32_t csr_mscratch;              /**< CSR MSCRATCH (0x340) - interrupt stack ptr */
    uint32_t wakeup_flag;               /**< Set to 1 after wakeup */
    uint32_t app_entry;                 /**< Entry point after wakeup */

    /* RTC/Time */
    uint64_t rtc_before_sleep_cnt;      /**< RTC counter before sleep (survives HBN RAM) */
    uint8_t rtc_wakeup_en;              /**< RTC wakeup enabled */

    /* Wakeup info */
    int32_t wakeup_reason;              /**< Wakeup reason */
    uint32_t wake_gpio_bits;            /**< GPIO bits that woke up */
    uint32_t wake_counts;               /**< Wakeup counts */   

    /* RC32K calibration */
    int32_t rc32k_fr_ext;               /**< RC32K frequency trim code */
    uint8_t rc32k_clock_ready;          /**< RC32K calibration ready */
    int32_t rtc32k_error_ppm;           /**< RC32K error in ppm */

    /* Flash recovery */
    void *flash_cfg;                    /**< Flash configuration pointer */
    uint32_t flash_jdec_id;             /**< Flash JEDEC ID */
    uint32_t flash_offset;              /**< Flash image offset */
    uint8_t flash_clk;                  /**< Flash clock setting */
    uint8_t flash_clk_div;              /**< Flash clock divider */
    uint8_t flash_pin_cfg;              /**< Flash GPIO pin configuration */
    uint8_t do_xip_recovery;            /**< XIP recovery needed */

    /* PSRAM (always present for stable struct layout across configs) */
    uint16_t psram_dqs_cfg;             /**< PSRAM DQS configuration (0 = no PSRAM) */

    /* Saved registers for LP restore */
    uint32_t saved_regs[HAL_LP_SAVED_REGS_MAX];  /**< Saved register values */

    /* System clock state — symmetric save/restore so wakeup can match
     * pre-sleep clock (XTAL-only path skips PLL/DLL power-on entirely). */
    uint8_t sys_clk_type;               /**< GLB_SYS_CLK_Type before sleep */
    uint8_t xtal_type;                  /**< GLB_DLL_XTAL_Type before sleep */
} hal_lp_param_t;

/* Global LP parameter pointer.
 * HBN RAM at 0x20090xxx is in the uncacheable address range (0x2xxxxxxx)
 * on IPRO7, so reads/writes bypass D-Cache automatically. */
#define hal_lp_param    ((volatile hal_lp_param_t *)HAL_LP_PARAM_ADDR)

/*============================ API ========================================*/

/**
 * @brief Initialize low power module
 * @return 0 on success, negative on error
 */
int hal_lp_init(void);

/**
 * @brief Enter low power mode
 * @param cfg Low power configuration
 * @param result Output: wakeup result (can be NULL)
 * @return Wakeup reason on success, negative on error
 */
int hal_lp_enter(hal_lp_cfg_t *cfg, hal_lp_result_t *result);

/**
 * @brief Enter HBN_0 sleep, wake via cold boot (true no-OCRAM-retention)
 *
 * HBN_0 powers off everything except HBN domain (4KB HBN RAM + RTC). On wake
 * bootrom does a full cold boot: no wake callback runs, FW re-enters via
 * _start. All state the app wants to carry across must be in HBN RAM before
 * this call.
 *
 * This function populates hal_hbnram_lp_wake_ctx (magic, wake_counter+1,
 * wake_reason) AND snapshots boot2 params from 0x11010000 into HBN RAM so
 * that Boot2_Param_Save() can fall back to the snapshot after cold boot
 * (PT table / flash cfg survive the OCRAM power cycle).
 *
 * After cold boot, read state via:
 *  - hal_lp_is_fastboot_wakeup() — true on first call after wake
 *  - hal_lp_get_wake_context()   — wake_counter/reason/rtc
 *
 * @param cfg Low power config (only rtc_wakeup_en/gpio_wakeup_en/rtc_sleep_us
 *            are used; retention flags are ignored — HBN_0 has none)
 * @return Negative on error. On success NEVER RETURNS.
 */
int hal_lp_enter_fastboot(hal_lp_cfg_t *cfg);

/**
 * @brief Check whether current boot is an LP fastboot wakeup
 *
 * Reads BOOT_FLAG_LP_FASTBOOT in HBN sysinfo. Safe to call from main() early.
 * Clears the flag on first call to prevent stale detection on next cold boot.
 *
 * @return true if this boot resumed from hal_lp_enter_fastboot(), false otherwise
 */
bool hal_lp_is_fastboot_wakeup(void);

/**
 * @brief Get pointer to LP wake context (fastboot path)
 *
 * Valid immediately after cold boot when hal_lp_is_fastboot_wakeup() == true.
 * Returns a pointer to the volatile HBN RAM region; contents include
 * wake_reason, rtc_cnt_low/high, wake_counter.
 *
 * @return Pointer to wake context, never NULL (HBN RAM is always mapped)
 */
const volatile hal_hbnram_lp_wake_ctx_t *hal_lp_get_wake_context(void);

/**
 * @brief Configure GPIO wakeup
 * @param cfg GPIO configuration
 * @return 0 on success, negative on error
 */
int hal_lp_gpio_wakeup_cfg(hal_lp_gpio_cfg_t *cfg);

/**
 * @brief Get wakeup reason from last sleep
 * @return Wakeup reason bitmask
 */
int hal_lp_get_wakeup_reason(void);

/**
 * @brief Get GPIO state that caused wakeup
 * @return GPIO bits that triggered wakeup
 */
uint32_t hal_lp_get_wakeup_gpio(void);

/*============================ Wakeup Hooks ===============================*/

/**
 * @brief Wakeup hook callback type
 *
 * Called after LP wakeup, before returning to normal operation.
 * Use this to reinitialize peripherals, ECLIC, or other modules
 * that lose state during sleep.
 *
 * @param wakeup_reason Bitmask of wakeup reasons (HAL_LP_WAKEUP_*)
 * @param arg User argument passed during registration
 * @return 0 on success, negative on error
 */
typedef int (*hal_lp_wakeup_hook_t)(int wakeup_reason, void *arg);

/**
 * @brief Register a wakeup hook
 *
 * The hook is called on every LP wakeup, after system restore but
 * before returning to the caller. Multiple hooks can be registered.
 *
 * @param hook Callback function
 * @param arg User argument passed to callback
 * @return 0 on success, negative on error (e.g., max hooks reached)
 */
int hal_lp_register_wakeup_hook(hal_lp_wakeup_hook_t hook, void *arg);

/**
 * @brief Unregister a wakeup hook
 *
 * @param hook Callback function to remove
 * @return 0 on success, -1 if not found
 */
int hal_lp_unregister_wakeup_hook(hal_lp_wakeup_hook_t hook);

/*============================ Virtual Time ===============================*/

/**
 * @brief Get virtual time in microseconds
 *
 * Virtual time is maintained across sleep cycles, providing
 * a continuous timestamp even when the CPU is sleeping.
 *
 * @return Virtual time in microseconds
 */
uint64_t hal_lp_get_virtual_us(void);

/**
 * @brief Get virtual time in milliseconds
 * @return Virtual time in milliseconds
 */
uint64_t hal_lp_get_virtual_ms(void);

/*============================ RC32K Calibration ==========================*/

/**
 * @brief Select RTC clock source
 * @param use_xtal32k true to use XTAL 32K, false to use RC32K
 */
void hal_lp_rtc_clk_sel(bool use_xtal32k);

/**
 * @brief Coarse adjust RC32K clock
 * @param expect_time Expected time in us
 * @param actual_time Actual measured time in us
 * @return 0 on success, negative on error
 */
int hal_lp_rc32k_coarse_adj(uint32_t expect_time, uint32_t actual_time);

/**
 * @brief Set RC32K clock ready flag
 * @param ready 1 = ready, 0 = not ready
 */
void hal_lp_set_rc32k_ready(uint8_t ready);

/**
 * @brief Get RC32K clock ready flag
 * @return 1 if ready, 0 if not ready
 */
uint8_t hal_lp_get_rc32k_ready(void);

/*============================ Flash XIP Recovery =========================*/

/**
 * @brief Perform flash XIP recovery after wakeup
 *
 * This function restores flash XIP mode after exiting
 * low power mode. Called automatically by hal_lp_enter().
 */
void hal_lp_xip_recovery(void);

/*============================ PSRAM ======================================*/

/**
 * @brief Get PSRAM DQS config for LP save
 *
 * Weak default returns 0 (no PSRAM). Overridden by hal_psram.c
 * when CONFIG_USE_PSRAM is enabled.
 */
uint16_t hal_lp_get_psram_dqs_cfg(void);

/**
 * @brief Restore PSRAM after wakeup
 *
 * Called automatically by hal_lp_enter() if psram_dqs_cfg != 0.
 * Weak default is no-op. Overridden by hal_psram.c when PSRAM enabled.
 */
void hal_lp_psram_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_LP_H__ */
