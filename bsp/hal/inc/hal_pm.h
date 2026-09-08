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

#ifndef __HAL_PM__H__
#define __HAL_PM__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"

/* -----------------------------------------------------------------------
 * Sleep level enumerations
 * ----------------------------------------------------------------------- */

/** PDS (Power-Down Sleep) levels — deepest sleep with SRAM retention.
 *
 * PM_PDS_LEVEL_31 is a generic "deepest level" selector, not a per-chip level
 * number. On IPRO6 it maps to the chip's single PDS level (the pds15-equivalent
 * config, pdsCfgLevel15); on IPRO7 it maps to pdsCfgLevel31. The HAL picks the
 * right config struct internally — callers pass this token on both chips. */
enum pm_pds_sleep_level {
    PM_PDS_LEVEL_31 = 31,  /**< Deepest PDS: CPU/RF/Misc reset + clock-gated */
};

/** HBN (Hibernate) levels — all SRAM lost, only HBN RAM (4 KB) retained. */
enum pm_hbn_sleep_level {
    PM_HBN_LEVEL_0,
    PM_HBN_LEVEL_1,
    PM_HBN_LEVEL_2,
};

/** Wakeup event sources for HBN mode. */
enum pm_event_type {
    PM_HBN_WAKEUP_EVENT_NONE,
    PM_HBN_GPIO9_WAKEUP_EVENT,
    PM_HBN_GPIO10_WAKEUP_EVENT,
    PM_HBN_GPIO11_WAKEUP_EVENT,
    PM_HBN_GPIO12_WAKEUP_EVENT,
    PM_HBN_RTC_WAKEUP_EVENT,
    PM_HBN_BOR_WAKEUP_EVENT,
    PM_HBN_ACOMP0_WAKEUP_EVENT,
    PM_HBN_ACOMP1_WAKEUP_EVENT,
};

/* -----------------------------------------------------------------------
 * PDS configuration
 * ----------------------------------------------------------------------- */

/**
 * @brief PDS application configuration.
 *
 * Passed to pm_pds_enable() to control power-down behaviour.
 * Most fields are set by pm_pds_mode_enter() from compile-time defaults.
 */
typedef struct {
    uint8_t  pdsLevel;       /**< PDS level (PM_PDS_LEVEL_31) */
    uint8_t  turnOffRF;      /**< Turn off RF during sleep */
    uint8_t  powerDownFlash; /**< Power down flash during sleep */
    uint8_t  ocramRetetion;  /**< Enable OCRAM retention (HW default: all banks) */
    uint8_t  turnoffPLL;     /**< Turn off PLL during sleep */
    uint8_t  turnoffDLL;     /**< Turn off DLL during sleep */
    uint8_t  flashContRead;  /**< Enable flash continuous read */
    uint8_t  pdsLdoEn;       /**< Enable PDS LDO voltage control */
    uint8_t  fastRecovery;   /**< Enable fast recovery path */
    uint32_t sleepTime;      /**< Sleep duration in RTC ticks (32.768 kHz) */
    uint32_t *flashCfg;      /**< Flash config pointer (for flash power-down) */
    uint32_t ldoLevel;       /**< LDO output voltage level */
    void (*preCbFun)(void);  /**< Pre-sleep callback (optional) */
    void (*postCbFun)(void); /**< Post-wakeup callback (optional) */
} PM_PDS_CFG_Type;

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

/** Configure and enter PDS sleep (low-level, use pm_pds_mode_enter instead). */
void pm_pds_enable(uint32_t *cfg);

/**
 * @brief Keep the Misc power domain alive across PDS31 sleeps.
 *
 * Default PDS31 power-gates the Misc domain (which includes the M154
 * 802.15.4 baseband). For LP apps that need M154 register state to
 * survive across sleep cycles, call with keep=true before the first
 * PDS entry. Trade-off: a few tens of µA extra leakage during sleep
 * (Misc stays at Vdd, just clock-gated) vs avoiding a full M154
 * re-init on every wake.
 *
 * Clears MiscPwrOff/MiscRst in pdsCtl4 so the domain is neither
 * power-gated nor reset; MiscGateClk remains as configured (clock
 * still gated during sleep).
 */
void pm_pds_set_misc_keep(bool keep);

/**
 * @brief Pointer to the level-31 PDS config the entry path applies.
 *
 * PDS_Enable() rewrites PDS_CTL/CTL4 from this struct on every entry, so this
 * -- not the hardware register -- is what decides what a PDS sleep does.
 * Layout is PDS_DEFAULT_LV_CFG_Type (four uint32_t: CTL, CTL2, CTL3, CTL4).
 *
 * @return pointer to the config, never NULL.
 */
void *pm_pds_get_level31_cfg(void);

/* BLE controller PDS domain policy.
 *
 * Driver/BSP defaults remain unchanged unless the application explicitly calls
 * pm_pds_ble_ctlr_domain_policy_set() with PM_PDS_BLE_CTLR_DOMAIN_POLICY_ENABLE.
 * This keeps connected-BLE PDS qualification policy in the application layer
 * instead of silently changing every user of pm_pds_mode_enter().
 */
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_NONE            (0U)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_ENABLE          (1U << 0)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_RF         (1U << 1)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ         (1U << 2)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_PWR_OFF     (1U << 3)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_MEM_ACTIVE  (1U << 4)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC       (1U << 5)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_SAFE_OCRAM      (1U << 6)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE  (1U << 7)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_CLOCKS     (1U << 8)
#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_GLOBAL_PWR (1U << 9)

#define PM_PDS_BLE_CTLR_DOMAIN_POLICY_CONNECTED_BASELINE \
    (PM_PDS_BLE_CTLR_DOMAIN_POLICY_ENABLE |              \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_RF |             \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ |             \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_PWR_OFF |         \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_MEM_ACTIVE |      \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC |           \
     PM_PDS_BLE_CTLR_DOMAIN_POLICY_SAFE_OCRAM)

void pm_pds_ble_ctlr_domain_policy_set(uint32_t policy);
uint32_t pm_pds_ble_ctlr_domain_policy_get(void);

/**
 * Runtime override of the PDS DCDC11 rail (IPRO6 only; no-op elsewhere).
 *
 * PDS powers LDO11 down (pdsCtl.ldo11Off = 1) and rides DCDC11 instead, so
 * pdsDcdc11Vsel is the knob that actually moves the sleep floor — measured
 * 2026-08-06: sweeping the LDO11 vout selects over their full range does
 * nothing, exactly because that rail is off.
 *
 * Exposed at run time because PDS_Enable() writes the whole pdsCtl word into
 * PDS_CTL at every entry, so poking the register from the console is clobbered
 * before the part ever sleeps — the value has to live in pdsCfgLevel15.
 *
 * @param vsel   PDS_CTL[27:23] DCDC11 voltage select, 5 bits (reset value 0x8).
 * @param enable PDS_CTL[18] pdsDcdc11VselEn — 0 leaves DCDC11 at its run-mode
 *               voltage. Exposed alongside vsel so a null vsel result can be
 *               told apart from an inert control path.
 */
void pm_pds_set_dcdc11_vsel(uint8_t vsel, bool enable);

/** Read back what pm_pds_set_dcdc11_vsel() will apply. Zeroed on non-IPRO6. */
void pm_pds_get_dcdc11_vsel(uint8_t *vsel, uint8_t *enable);

/**
 * @brief Pick the 32 kHz source and hand ownership of it to the caller.
 *
 * Legacy sleep paths keep their RC32K default unless a product explicitly
 * takes ownership here.  Once this is called, the selected source is preserved
 * through PDS entry instead of being silently forced back to RC.  The BLE
 * controller low-power path uses this to prefer XTAL32K, measure the actual
 * error, and fall back to calibrated RC32K only when the crystal is outside
 * budget.  Nothing calls it during boot, so the shipped platform default is
 * unchanged.
 *
 * @param use_xtal 1 = power the crystal up and select XTAL32K, 0 = RC32K.
 * @param settle_ms extra settle delay after powering the crystal, before the
 *        select. The driver's own HBN_Power_On_Xtal_32K() waits only 1.1 ms
 *        despite a "Delay >1s" comment, and selecting a not-yet-oscillating
 *        crystal stops the whole 32 kHz domain — so pass a real delay when
 *        switching to XTAL from cold.
 */
void pm_pds_set_32k_src(uint8_t use_xtal, uint32_t settle_ms);

/**
 * @brief Read the live 32 kHz selection.
 * @param use_xtal  out: 1 if F32K_SEL currently selects XTAL32K.
 * @param forced_rc out: 1 while the sleep paths still force RC32K (i.e.
 *        pm_pds_set_32k_src() has not been called).
 */
void pm_pds_get_32k_src(uint8_t *use_xtal, uint8_t *forced_rc);

/** Enter PDS sleep with RTC timed wakeup. */
void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time);

/**
 * @brief Arm a GPIO as a PDS wakeup source.
 *
 * Unlike HBN, which can only be woken by the ten AON pads, PDS keeps the GPIO
 * pad logic alive and can wake on any of GPIO0..41. The pins are grouped four
 * at a time in hardware (PDS_GPIO_GROUP_SET_*), and the trigger is programmed
 * per group, so pins sharing a group share a trigger.
 *
 * Use an ASYNC trigger: the SYNC variants need the clock that PDS stops.
 *
 * @param pin      GPIO number, 0..41.
 * @param trigger  A PDS_GPIO_INT_* value; ASYNC_LOW_LEVEL suits a button that
 *                 pulls the pin down.
 * @return 0 on success, negative if the pin has no PDS interrupt group.
 *
 * Call before pm_pds_mode_enter(); the arming persists until
 * pm_pds_gpio_wakeup_clear(). Without it PDS arms only the RTC path, which is
 * what every existing caller expects.
 */
int pm_pds_gpio_wakeup_set(uint8_t pin, uint8_t trigger);

/** @brief Disarm every GPIO wakeup source armed by pm_pds_gpio_wakeup_set(). */
void pm_pds_gpio_wakeup_clear(void);

/**
 * @brief Dump the PDS wake configuration and what PDS currently senses.
 *
 * Arms nothing and sleeps nothing; it only reports. Useful for telling "PDS
 * cannot see the pin" from "PDS sees it but the wake is not routed", which the
 * behaviour alone does not distinguish.
 */
void pm_pds_gpio_wakeup_dump(void);

/** @brief Which GPIO caused the last PDS wake, or -1 if it was not a GPIO. */
int pm_pds_gpio_wakeup_source(void);

/**
 * @brief Override the next PDS RTC compare with an absolute AON RTC target.
 *
 * The override is one-shot and is consumed by pm_pds_mode_enter(). Passing
 * zero clears it. This avoids racing two separately-derived relative timers
 * when another always-on hardware block owns the sleep deadline.
 */
void pm_pds_rtc_target_override_set(uint64_t target_ticks);

/**
 * Re-enter PDS31 from the wake path (DTIM loop) without resuming the full
 * firmware. TCM-resident, never returns. @param sleep_time in 32 kHz RTC ticks.
 */
void pm_pds_resleep(uint32_t sleep_time);

/** Enter HBN (hibernate) sleep — all SRAM lost on wakeup. */
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint32_t sleep_time);

/** Re-enter HBN after a wakeup event. */
void pm_hbn_enter_again(bool reset);

/** Register fast-wakeup callback (stored in HBN_RSV1, checked by bootrom). */
void pm_set_wakeup_callback(void (*wakeup_callback)(void));

/** Clear wakeup callback — next reset is a normal cold boot. */
void pm_clear_wakeup_callback(void);

/** Get the HBN wakeup event type after waking from HBN. */
enum pm_event_type pm_get_wakeup_event(void);

/** Initialize Brown-Out Reset detection. */
void pm_bor_init(void);

/** Register HBN OUT0 IRQ handler. */
void pm_hbn_out0_irq_register(void);

/** Register HBN OUT1 IRQ handler. */
void pm_hbn_out1_irq_register(void);

/** Wakeup event callback (weak, override in application). */
void pm_irq_callback(enum pm_event_type event);

#ifdef __cplusplus
}
#endif
#endif
