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

#include <generated/autoconf.h>
#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#endif
#include "compiler/common.h"
#include "drv_common.h"
#include "drv_sflash.h"
#include "drv_glb.h"
#include <stdio.h>
#include "drv_pds.h"
#include "drv_hbn.h"
#include "drv_aon.h"
#include "drv_uart.h"
#include "hal_pm.h"
#include "hal_flash.h"
#include "hal_psram.h"
#include "pds_reg.h"
#include "hbn_reg.h"
#include "system_soc.h"        /* L1C_DCACHE_CLEAN_INVALID */

#ifdef CONFIG_USE_PSRAM
#include "drv_psram.h"
#endif

SPI_Flash_Cfg_Type *flash_cfg;

/* Retained breadcrumbs for validating undocumented PDS domain controls.
 * [0..3] are the four config words passed to the driver; [4..7] are the
 * corresponding hardware registers read back after programming and before
 * WFI.  The btctrl2 PDS probe consumes these only for diagnostics. */
volatile uint32_t pm_pds_last_domain_words[8]
    __attribute__((section(".ocram_bss"), aligned(4)));

/* Last OCRAM retention programming.
 * [0] magic
 * [1] PDS_RAM1 cfg:    RET1[15:0] | RET2[15:0] << 16
 * [2] PDS_RAM2 cfg:    PGEN[15:0]
 * [3] PDS_RAM1 readback after programming
 * [4] PDS_RAM2 readback after programming
 *
 * IPRO7 connected BLE PDS product target is to retain only:
 *   - OCRAM bits 0..3  : 64 KiB software-retained window
 *   - EM bit 7         : 16 KiB BLE EM (128..144 KiB in PDS RAM map)
 * and progressively enable low-power/PGEN only for proven non-retained OCRAM
 * bits.  The current verified connected-PDS baseline keeps all OCRAM RAM state
 * valid across PDS (RET1=0, RET2=0xffff, PGEN=0).  Do not tighten this again
 * until all warm-resume state in the candidate power-gated bank is either moved
 * into retained OCRAM/HBN or explicitly reconstructed after wake. */
volatile uint32_t pm_pds_last_ram_words[5]
    __attribute__((section(".hbn_noinit"), used, aligned(4)));

/* Last RTC-arm transaction in always-on RAM.  CCI can inspect this even when
 * the CPU remains parked in PDS, unlike cacheable OCRAM diagnostics. */
volatile uint32_t pm_pds_hbn_arm_diag[40]
    __attribute__((section(".hbn_noinit"), used, aligned(4)));

/* One-shot absolute HBN RTC target. Access is serialized by the caller's
 * IRQ-disabled PDS entry section. */
static volatile uint64_t s_pm_pds_rtc_target_override;

void pm_pds_rtc_target_override_set(uint64_t target_ticks)
{
    s_pm_pds_rtc_target_override = target_ticks;
}

#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
/* Runtime BLE-controller PDS domain sweep mask.  Low 16 bits are test
 * controls; high bits keep this CCI-tuned diagnostic in .data rather than a
 * fragile zeroed BSS location on the live DUT.
 *
 * bit0  MISC power off
 * bit1  MISC reset
 * bit2  MISC clock gate
 * bit3  MISC isolation
 * bit4  BLE reset
 * bit5  BLE clock gate
 * bit6  BLE isolation
 * bit8  RF power controlled by PDS
 * bit9  BZ reset
 * bit10 BZ clock gate
 * bit11 BZ isolation
 */
volatile uint32_t pm_pds_diag_domain_override = UINT32_C(0x50440000);
volatile uint32_t pm_pds_ble_ctlr_domain_policy =
    PM_PDS_BLE_CTLR_DOMAIN_POLICY_NONE;

#define PM_PDS_DIAG_MISC_PWR_OFF  UINT32_C(1 << 0)
#define PM_PDS_DIAG_MISC_RST      UINT32_C(1 << 1)
#define PM_PDS_DIAG_MISC_GATE     UINT32_C(1 << 2)
#define PM_PDS_DIAG_MISC_ISO      UINT32_C(1 << 3)
#define PM_PDS_DIAG_BLE_RST       UINT32_C(1 << 4)
#define PM_PDS_DIAG_BLE_GATE      UINT32_C(1 << 5)
#define PM_PDS_DIAG_BLE_ISO       UINT32_C(1 << 6)
#define PM_PDS_DIAG_RF_PWR_OFF    UINT32_C(1 << 8)
#define PM_PDS_DIAG_BZ_RST        UINT32_C(1 << 9)
#define PM_PDS_DIAG_BZ_GATE       UINT32_C(1 << 10)
#define PM_PDS_DIAG_BZ_ISO        UINT32_C(1 << 11)

#define PM_PDS_RAM_DIAG_MAGIC UINT32_C(0x52414d31) /* "RAM1" */
#define PM_PDS_OCRAM_RETAIN1_SAFE_MASK UINT32_C(0x0000)
#define PM_PDS_OCRAM_RETAIN2_SAFE_MASK UINT32_C(0xffff)
#define PM_PDS_OCRAM_PGEN_SAFE_MASK    UINT32_C(0x0000)
#endif

/* -----------------------------------------------------------------------
 * PDS power-down defaults
 * ----------------------------------------------------------------------- */
#define PM_PDS_FLASH_POWER_OFF   1
#define PM_PDS_DLL_POWER_OFF     1
#define PM_PDS_PLL_POWER_OFF     1
#define PM_PDS_RF_POWER_OFF      1

/* Diagnostic only — see the two call sites around __WFI() in pm_pds_enable()
 * for what it bisects. TEMPORARILY 1 while chasing the hang where
 * hal_lp_enter() never returns on ipro_ble_lowpower_test; this file is
 * recompiled per app rather than shared as a prebuilt object, so this has no
 * effect on any other app's binary, but flip back to 0 once resolved so it
 * does not linger as a silent default for the next person to touch this
 * file. */
#ifndef PM_PDS_WFI_TRACE
#define PM_PDS_WFI_TRACE 0
#endif

/* DIAGNOSTIC — keep the whole BLE side powered and clocked through PDS31.
 *
 * ⚠️ Costs a lot of sleep current by construction. Do NOT ship with this on;
 * it exists to answer one question and then go back to 0.
 *
 * The question: a connected PDS31 leaves the link permanently deaf, and by
 * now everything measurable on our side has been eliminated -- sub-half-slot
 * timing (anchor err 15-16 us on the event that lands), coarse timebase
 * (credit err -1..1), the event counter checked against the PEER across the
 * sleep (d_ec == want, using CRC-OK-to-CRC-OK sampling), the channel map
 * (channels actually used match the applied map exactly), the whole RF
 * register block, a full RF recalibration run while deaf, and the receive
 * hardware itself (a reconnect restores full reception immediately).
 *
 * What has NOT been tested is this: BZ_BLE (the BLE core) is kept alive
 * today, but BZ -- the PHY/baseband next to it -- is reset AND clock-gated
 * on every PDS entry (BzRst/BzGateClk = 1), and PDS is also allowed to power
 * the RF down (pdsCtlRfSel = 1). So the half that was "kept alive" was only
 * ever half of the radio.
 *
 * With this on, nothing on the BLE side loses power, reset or clock:
 *   BzRst/BzGateClk = 0   PHY/baseband stays out of reset and clocked
 *   BleMemStby      = 0   no memory standby on the BLE core either
 *   pdsCtlRfSel     = 0   PDS does not gate RF power
 *   Bz/BleIsoEn     = 0   neither domain is isolated
 *
 * Binary outcome:
 *   link survives  -> the fault is in BZ/RF state torn down by PDS, and the
 *                     search narrows to what BzRst/BzGateClk destroy that the
 *                     existing restore path does not rebuild
 *   still deaf     -> the BLE side is fully exonerated as a whole, and the
 *                     fault is somewhere PDS touches that is not the radio */
/* VERDICT (measured, both outcomes): keep this 0.
 *
 * 1. It does NOT fix the deafness. With the leak repair in place connected
 *    PDS genuinely engaged (conn sleeps 4x, enter climbing) and the link was
 *    still deaf -- so by the criterion set out above, the BLE side as a whole
 *    is exonerated and the fault is somewhere PDS touches that is not the
 *    radio.
 * 2. It is not a stable base to build on anyway. With the domain kept alive
 *    while everything around it power-cycles, CLKN behaviour is
 *    non-deterministic: PDS entries in one run measured ran=3193 vs slept=50,
 *    ran=50 vs slept=208 (frozen), and ran=32000 vs slept=6565 -- a ten-second
 *    jump inside a two-second window, read from hardware before any reload.
 *    Survival was intermittent (4 of 14) even when it behaved. */
#ifndef IPRO7_PDS31_KEEP_BLE_ALIVE
#define IPRO7_PDS31_KEEP_BLE_ALIVE 0
#endif

#if PM_PDS_WFI_TRACE
/* Shared non-blocking trace primitive for this file. Same discipline as
 * hal_lp.c's LP_TRACE: queue-then-bounded-drain, so "byte never appeared"
 * cannot be confused with "queued but not yet clocked out before a power
 * domain transition." Register offsets duplicated from lp_uart.c. */
static void ATTR_TCM_SECTION pm_pds_trace_char(char c)
{
    volatile uint32_t *tx_cnt = (volatile uint32_t *)(UART0_BASE + 0x84);
    volatile uint32_t *tx_wr = (volatile uint32_t *)(UART0_BASE + 0x88);
    uint32_t spin;

    if ((*tx_cnt & 0x1Fu) != 0u) {
        *tx_wr = (uint32_t)c;
    }
    spin = 200000;
    while ((*tx_cnt & 0x1Fu) != 0x1Fu && --spin) {
        /* drain until FIFO reports fully empty, bounded */
    }
}
#define PM_TRACE(c) pm_pds_trace_char(c)

static void ATTR_TCM_SECTION pm_pds_trace_hex(uint32_t v)
{
    static const char digits[] = "0123456789ABCDEF";
    int shift;

    pm_pds_trace_char('0');
    pm_pds_trace_char('x');
    for (shift = 28; shift >= 0; shift -= 4) {
        pm_pds_trace_char(digits[(v >> shift) & 0xFu]);
    }
}
#define PM_TRACE_HEX(v) pm_pds_trace_hex(v)
#else
#define PM_TRACE(c)
#define PM_TRACE_HEX(v)
#endif
#define PM_PDS_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_0P90V
#define PM_HBN_LDO_LEVEL_DEFAULT HBN_LDO_LEVEL_0P90V

/* -----------------------------------------------------------------------
 * PDS hardware configuration — the deepest (and on IPRO6 the ONLY) PDS level.
 * CPU, RF controller ("Bz"), and misc peripherals are all reset + clock-gated
 * during sleep.
 *
 * Per-chip naming: IPRO6 has a SINGLE PDS level, which is the pds15-equivalent
 * of the reference part — so its config struct is pdsCfgLevel15 and BOTH the
 * first sleep (pm_pds_enable) and the DTIM wake-path re-sleep (pm_pds_resleep)
 * use it. IPRO7 keeps pdsCfgLevel31. The public selector PM_PDS_LEVEL_31 is a
 * generic "deepest level" token (PDS_Default_Level_Config takes the cfg struct,
 * not a level number); on IPRO6 it simply selects this one pds15 config.
 *
 * pdsCtl  — master power/clock/isolation controls
 * pdsCtl2 — force-mode overrides (all 0 = let HW decide)
 * pdsCtl3 — miscellaneous isolation
 * pdsCtl4 — per-domain reset / gate-clock enables:
 *           CPU, RF controller, Misc peripherals
 *
     * OCRAM retention: on the IPRO7 DUTs verified for BLE connected-PDS, the
     *                  practical safe baseline is to leave the platform's RAM
     *                  retention defaults intact.  Do not apply undocumented
     *                  RAM-retention clock bits; previous attempts wedged resume.
     *                  When CONFIG_OCRAM_NORET_SPLIT is explicitly enabled by a
     *                  BLE application, pm_pds_config_ocram_safe_retention()
     *                  records and applies the separately validated PDS_RAM1/2
     *                  words (currently RET1=0, RET2=0xffff, PGEN=0). The lp_fw
     *                  image therefore lives in retained OCRAM, not WRAM.
     * ----------------------------------------------------------------------- */
#if defined(IPRO6)
static ATTR_DTCM_SECTION PDS_DEFAULT_LV_CFG_Type pdsCfgLevel15 = {
    .pdsCtl = {
        .pdsStart        = 1,
        .sleepForever    = 0,
        .xtalForceOff    = 0,
        .saveWiFiState   = 0,
        .ldo11Off        = 1,
        .bgSysOff        = 1,   /* overridden to 0 when PSRAM is used */
        .ctrlGpioIePuPd  = 0,
        .dcdc18Off       = 1,   /* overridden to 0 when PSRAM is used */
        .clkOff          = 1,
        .memStby         = 1,
        .glbRstProtect   = 0,
        .isolation       = 1,
        .waitXtalRdy     = 1,
        .pdsPwrOff       = 1,
        .xtalOff         = 1,
        .socEnbForceOn   = 1,   /* match IPRO7 (same CPU): gates the AON soc-enable
                                 * flag, not the CPU power-gate (cpuPwrOff/cpuRst in
                                 * pdsCtl4 do that). IPRO7 runs PDS fine with this =1. */
        .pdsRstSocEn     = 1,
        .pdsRC32mOn      = 0,
        .pdsDcdc11VselEn = 1,
        .usbpllOff       = 1,
        .aupllOff        = 1,
        .wifipllOff      = 0,   /* wifipll cannot re-lock after power-off */
        .pdsDcdc11Vsel   = 0x8,
        .pdsCtlRfSel     = 3,
        .pdsUseTbttSlp   = 0,
        .pdsGpioIsoMod   = 0,
    },
    .pdsCtl2 = {
        .forceCpuPwrOff  = 0, .forceWbPwrOff   = 0, .forceUsbPwrOff  = 0,
        .forceCpuIso     = 0, .forceWbIso       = 0, .forceUsbIso     = 0,
        .forceCpuPdsRst  = 0, .forceWbPdsRst    = 0, .forceUsbPdsRst  = 0,
        .forceCpuMemStby = 0, .forceWbMemStby   = 0, .forceUsbMemStby = 0,
        .forceCpuGateClk = 0, .forceWbGateClk   = 0, .forceUsbGateClk = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff  = 0, .forceMiscIsoEn  = 0, .forceMiscPdsRst  = 0,
        .forceMiscMemStby = 0, .forceMiscGateClk = 0,
        .cpuIsoEn = 1, .wbIsoEn = 1, .usbIsoEn = 1, .miscIsoEn = 1,
    },
    /* Misc domain takes ALL FOUR controls (PDS_CTL4[27:24] = 0xF).
     *
     * Only pwr_off/reset were set; mem_stby and gate_clk were inherited as 0
     * straight from bl616_lp_fw/pds15.c. That omission is worth 27 uA — measured
     * 2026-08-06 on the EVB at 3.3 V, control interleaved between every reading:
     *
     *   [25:24] pwr_off+reset only (as shipped)   67.04 uA   (68.5 / 66.5 / 66.1)
     *   [26]    + mem_stby                        40.47 uA   -26.57 uA
     *   [27:24] + gate_clk  (this)                40.08 uA   -26.96 uA  (-40%)
     *
     * The saving is entirely the MISC domain: the same sweep put cpuMemStby at
     * +0.15 uA, wbMemStby at 0.00 and usbMemStby at -0.35, all inside the noise,
     * so their bits stay clear — in particular cpuMemStby, the one that could
     * plausibly disturb a CPU-domain resume, is not needed and is not taken.
     *
     * OCRAM survives it: a 4 KB canary in a never-initialised NOLOAD section came
     * back intact (0 of 1024 words bad) across resume-in-place PDS, twice, and
     * also with all four domains' mem_stby set. Retention is governed by
     * PDS_RAM3/OCRAM_RET2, which this does not touch.
     *
     * Precedent: BL602's shipping PDS configs set MiscMemStby/MiscGateClk in
     * every level (e.g. bl602_demo_ble_pds/pds.c); BL616 dropped them, and we
     * inherited the drop. This restores the older reference behaviour for the
     * one domain that measurably matters. */
    .pdsCtl4 = {
        .cpuPwrOff  = 1, .cpuRst  = 1, .cpuMemStby  = 0, .cpuGateClk  = 0,
        .wbPwrOff   = 1, .wbRst   = 1, .wbMemStby   = 0, .wbGateClk   = 0,
        .usbPwrOff  = 1, .usbRst  = 1, .usbMemStby  = 0, .usbGateClk  = 0,
        .miscPwrOff = 1, .miscRst = 1, .miscMemStby = 1, .miscGateClk = 1,
    },
    .pdsCtl5 = {
        /* cpuWfiMask=1 (PDS_CTL5[0] cr_np_wfi_mask): PDS starts on the pdsStart
         * write and does NOT wait for the core's np_wfi. The race that =1 used to
         * lose (WFI returning before PDS's power-down latency elapsed) is closed by
         * neutralizing mtimecmp before WFI in pm_pds_enable(), so the core stays
         * parked in WFI long enough for PDS to power-gate it. NOTE: cpuWfiMask=0
         * (the reference value) was tried and breaks resume-in-place on this DUT. */
        /* cpuWfiMask=1 (PDS_CTL5[0] cr_np_wfi_mask): PDS starts on the pdsStart
         * write and does NOT wait for the core's np_wfi. The race that =1 used to
         * lose (WFI returning before PDS's power-down latency elapsed) is closed by
         * neutralizing mtimecmp before WFI (pm_pds_enable / pm_pds_resleep), so the
         * core stays parked in WFI long enough for PDS to power-gate it. NOTE:
         * cpuWfiMask=0 (the reference value) was tried and breaks resume on this DUT. */
        .cpuWfiMask = 1, .pdsPadOdEn = 0,
        .ctrlUsb33  = 1, .ldo18ioOff = 1, .pdsGpioKeep = 0,
    },
};
#else /* IPRO7 */
static ATTR_DTCM_SECTION PDS_DEFAULT_LV_CFG_Type pdsCfgLevel31 = {
    .pdsCtl = {
        .pdsStart       = 1,
        .sleepForever   = 0,
        .xtalForceOff   = 0,
        .waitRC32mRdy   = 1,
        .avdd14Off      = 1,    /* overridden to 0 when PSRAM is used */
        .bgSysOff       = 1,    /* overridden to 0 when PSRAM is used */
        .glbRstProtect  = 0,
        .puFlash        = 1,
        .clkOff         = 1,
        .memStby        = 1,
        .swPuFlash      = 1,
        .isolation      = 1,
        .waitXtalRdy    = 0,
        .pdsPwrOff      = 1,
        .xtalOff        = 1,
        .socEnbForceOn  = 1,
        .pdsRstSocEn    = 1,
        .pdsRC32mOn     = 0,
        .pdsLdoVselEn   = 1,
        .xtalCntRC32kEn = 0,
        .cpu0WfiMask    = 1,
        .ldo11Off       = 1,
        .pdsCtlRfSel    = 1,    /* PDS controls RF power on/off during sleep */
        .xtalDegEn      = 0,
        .bleWakeupReqEn = 0,
        .swGpioIsoMod   = 0,
        .pdsClkOff      = 0,
        .pdsGpioKeep    = 0x7,
        .pdsGpioIsoMod  = 0,
    },
    .pdsCtl2 = {
        .forceCpuPdsRst  = 0,
        .forceBzPdsRst   = 0,
        .forceCpuGateClk = 0,
        .forceBzGateClk  = 0,
    },
    .pdsCtl3 = {
        .forceMiscPwrOff  = 0,
        .forceMiscPdsRst  = 0,
        .forceMiscGateClk = 0,
        .MiscIsoEn        = 1,
        .BleIsoEn         = 0, /* isolating the core defeats keeping it alive */
        .BzIsoEn          = 0, /* likewise for the PHY/baseband next to it */
    },
    .pdsCtl4 = {
        .cpuRst      = 1,
        .cpuGateClk  = 1,
        .BzRst       = 1,
        .BzGateClk   = 1,
        /* Tried 1/1/1 (reset alongside BZ/CPU, matching arch_main.c's
         * ble_controller_restore_hw() which already exists to reinit+restore
         * BLE state on wake for that configuration): identical result, current
         * probe still pinned at PDS level, never returns. Same outcome as 0/0/0
         * (kept alive) -- the CPU itself never comes back either way. Not the
         * cause; reverted to kept-alive, which at least has a stated purpose
         * (letting the connection resume without a full reinit) even though
         * neither setting affects the actual hang.
         *
         * Original rationale for 0/0/0: with the reset default of 1/1/1 (BLE
         * domain reset like BZ), DEEPSLCNTL came back zero, no wake-up
         * interrupt could ever fire, and the controller stayed flagged as
         * sleeping forever while its clock was demonstrably still running --
         * but that was the CPU successfully waking and finding BLE software
         * state inconsistent, a different failure from the current-pinned
         * total non-wake being chased now. */
        .BlePwrOff   = 0,
        .BleRst      = 0,
        .BleMemStby  = 1,
        .BleGateClk  = 0,
        .MiscPwrOff  = 1,
        .MiscRst     = 1,
        .MiscGateClk = 1,
    },
};
#endif /* IPRO6 / IPRO7 */

/* -----------------------------------------------------------------------
 * Internal helpers
 * ----------------------------------------------------------------------- */

/**
 * @brief Wait for UART TX FIFO to drain before entering sleep.
 */
static void pm_uart_flush(void)
{
    while (UART_GetTxBusBusyStatus(UART0_ID) == SET) {
        /* busy wait */
    }
}

/* -----------------------------------------------------------------------
 * PDS configuration tweaks
 * ----------------------------------------------------------------------- */

/* Raw access to the level-31 config the PDS entry applies.
 *
 * PDS_Enable() writes PDS_CTL and PDS_CTL4 straight out of this struct on
 * every single entry, and PDS_Force_Config() does the same for CTL2/CTL3, so
 * poking the hardware registers between sleeps achieves nothing -- the next
 * sleep overwrites them. Anything that wants to change what a PDS sleep does
 * has to change this.
 *
 * Additive: nothing else calls it, and the struct is unchanged. Exposed for
 * the low-power test app's runtime domain probe, which needs to walk the
 * per-domain matrix inside a single boot rather than one rebuild per cell --
 * this silicon's receiver calibration varies enough between cold starts that
 * comparing across boots is not sound. */
void *pm_pds_get_level31_cfg(void)
{
#if defined(IPRO6)
    /* PM_PDS_LEVEL_31 is the generic "deepest level" token on both chips; on
     * IPRO6 that level is backed by pdsCfgLevel15 (see the per-chip naming note
     * above), and pdsCfgLevel31 is not defined in this build at all. */
    return (void *)&pdsCfgLevel15;
#else
    return (void *)&pdsCfgLevel31;
#endif
}

void pm_pds_ble_ctlr_domain_policy_set(uint32_t policy)
{
#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
    pm_pds_ble_ctlr_domain_policy = policy;
#else
    (void)policy;
#endif
}

uint32_t pm_pds_ble_ctlr_domain_policy_get(void)
{
#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
    return pm_pds_ble_ctlr_domain_policy;
#else
    return PM_PDS_BLE_CTLR_DOMAIN_POLICY_NONE;
#endif
}

#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO) && defined(CONFIG_OCRAM_NORET_SPLIT)
static void ATTR_TCM_SECTION pm_pds_config_ocram_safe_retention(void)
{
    const uint32_t ret1_mask = PM_PDS_OCRAM_RETAIN1_SAFE_MASK;
    const uint32_t ret2_mask = PM_PDS_OCRAM_RETAIN2_SAFE_MASK;
    const uint32_t pgen_mask = PM_PDS_OCRAM_PGEN_SAFE_MASK;
    const uint32_t ram1 = ret1_mask | (ret2_mask << PDS_CR_PDS_RAM_RET2_POS);
    const uint32_t ram2 = pgen_mask << PDS_CR_PDS_RAM_PGEN_POS;

    pm_pds_last_ram_words[0] = PM_PDS_RAM_DIAG_MAGIC;
    pm_pds_last_ram_words[1] = ram1;
    pm_pds_last_ram_words[2] = ram2;

    writel(ram1, PDS_BASE + PDS_RAM1_OFFSET);
    writel(ram2, PDS_BASE + PDS_RAM2_OFFSET);

    pm_pds_last_ram_words[3] = readl(PDS_BASE + PDS_RAM1_OFFSET);
    pm_pds_last_ram_words[4] = readl(PDS_BASE + PDS_RAM2_OFFSET);
    pm_pds_hbn_arm_diag[24] = pm_pds_last_ram_words[0];
    pm_pds_hbn_arm_diag[25] = pm_pds_last_ram_words[1];
    pm_pds_hbn_arm_diag[26] = pm_pds_last_ram_words[2];
    pm_pds_hbn_arm_diag[27] = pm_pds_last_ram_words[3];
    pm_pds_hbn_arm_diag[28] = pm_pds_last_ram_words[4];
}
#endif

static void ATTR_TCM_SECTION pm_pds_clean_hbn_diag(void)
{
#if !defined(IPRO6)
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)pm_pds_hbn_arm_diag,
                           sizeof(pm_pds_hbn_arm_diag));
#if defined(CONFIG_BT_CTLR_IPRO)
    L1C_DCACHE_CLEAN_RANGE((uintptr_t)pm_pds_last_ram_words,
                           sizeof(pm_pds_last_ram_words));
#endif
#endif
}

void pm_pds_set_misc_keep(bool keep)
{
    /* keep=true: Misc power domain stays alive across PDS sleep so M154
     * (802.15.4) register state survives. keep=false restores the HW
     * default of power-gating + reset on sleep entry. MiscGateClk is
     * left untouched — clock gating during sleep is fine, the wake
     * path re-asserts the per-block CLK_EN bits it needs.
     *
     * Note: This is the original BSP semantics. A "BZ-keep / wake-on-
     * M154-frame" PoC was attempted 2026-05-21 to see if a shallower
     * PDS could let the radio listen across sleep — the PDS state
     * machine refused to leave IDLE in every configuration tried
     * (misc_keep alone, misc_keep + bz_keep, with/without rail-off
     * bits). On this DUT family PDS only engages when MISC is
     * actually power-gated; LIGHT PDS is not achievable from the
     * exposed BSP knobs. */
#if defined(IPRO6)
    pdsCfgLevel15.pdsCtl4.miscPwrOff = keep ? 0 : 1;
    pdsCfgLevel15.pdsCtl4.miscRst    = keep ? 0 : 1;
#else
    pdsCfgLevel31.pdsCtl4.MiscPwrOff = keep ? 0 : 1;
    pdsCfgLevel31.pdsCtl4.MiscRst    = keep ? 0 : 1;
#endif
}

void pm_pds_set_dcdc11_vsel(uint8_t vsel, bool enable)
{
    /* IPRO7's pdsCfgLevel31 has no DCDC11 select (it uses pdsLdoVselEn), so this
     * knob is IPRO6-only rather than #if'd per-field. */
#if defined(IPRO6)
    pdsCfgLevel15.pdsCtl.pdsDcdc11Vsel   = vsel & 0x1F;
    pdsCfgLevel15.pdsCtl.pdsDcdc11VselEn = enable ? 1 : 0;
#else
    (void)vsel;
    (void)enable;
#endif
}

void pm_pds_get_dcdc11_vsel(uint8_t *vsel, uint8_t *enable)
{
#if defined(IPRO6)
    if (vsel) {
        *vsel = (uint8_t)pdsCfgLevel15.pdsCtl.pdsDcdc11Vsel;
    }
    if (enable) {
        *enable = (uint8_t)pdsCfgLevel15.pdsCtl.pdsDcdc11VselEn;
    }
#else
    if (vsel) {
        *vsel = 0;
    }
    if (enable) {
        *enable = 0;
    }
#endif
}

#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
static void pm_pds_apply_ble_ctlr_domain_override(PDS_DEFAULT_LV_CFG_Type *cfg)
{
    uint32_t policy = pm_pds_ble_ctlr_domain_policy;
    uint32_t mask = pm_pds_diag_domain_override & UINT32_C(0x0000ffff);

    if ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_ENABLE) == 0U) {
        return;
    }

    /* APP-selected connected-PDS baseline.  The BSP struct above stays at its
     * default reset/power policy; only applications that explicitly opt in get
     * the BLE keep-alive profile and the CCI diagnostic mask layered on top. */
    cfg->pdsCtl.pdsCtlRfSel =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_RF) != 0U) ? 0 : 1;
    cfg->pdsCtl4.BzRst =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ) != 0U) ? 0 : 1;
    cfg->pdsCtl4.BzGateClk =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_BZ) != 0U) ? 0 : 1;
    cfg->pdsCtl3.BzIsoEn = 0;
    cfg->pdsCtl3.BleIsoEn = 0;
    cfg->pdsCtl4.BlePwrOff =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_PWR_OFF) != 0U) ? 1 : 0;
    cfg->pdsCtl4.BleRst = 0;
    cfg->pdsCtl4.BleMemStby =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_MEM_ACTIVE) != 0U) ? 0 : 1;
    cfg->pdsCtl4.BleGateClk = 0;

    /* A retained BLE transaction is not merely retained RAM: DEEPSLCNTL's
     * sleep timer and its hidden radio wake state must cross PDS unchanged.
     * PDS_CTL[16] makes PDS drive the SoC pds_rst line on entry/exit.  On
     * IPRO7AI that edge terminates the BLE native sleep transaction and raises
     * SLPINT even though BLE/BZ power, reset and clocks are all configured to
     * remain live.  Keep the BSP default for cold BLE-power-off PDS, but do
     * not assert pds_rst for the explicit retained-controller policy. */
    if ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_BLE_PWR_OFF) == 0U) {
        cfg->pdsCtl.pdsRstSocEn = 0;
    }

    /* BLE controller retained host/RTOS state currently requires MISC power
     * and reset to stay alive.  Stage-D sweeps must explicitly enable MISC
     * teardown and then prove retained placement before that can ship. */
    if ((policy & (PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC |
                   PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE)) != 0U) {
        cfg->pdsCtl4.MiscPwrOff = 0;
        cfg->pdsCtl4.MiscRst = 0;
    } else {
        cfg->pdsCtl4.MiscPwrOff = 1;
        cfg->pdsCtl4.MiscRst = 1;
    }
    cfg->pdsCtl4.MiscGateClk =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE) != 0U) ? 0 : 1;
    cfg->pdsCtl3.MiscIsoEn =
        ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_MISC_LIVE) != 0U) ? 0 : 1;

    if ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_CLOCKS) != 0U) {
        cfg->pdsCtl.xtalForceOff = 0;
        cfg->pdsCtl.clkOff = 0;
        cfg->pdsCtl.waitXtalRdy = 1;
        cfg->pdsCtl.xtalOff = 0;
        cfg->pdsCtl.pdsRC32mOn = 1;
        cfg->pdsCtl.pdsLdoVselEn = 0;
        cfg->pdsCtl.ldo11Off = 0;
    }
    if ((policy & PM_PDS_BLE_CTLR_DOMAIN_POLICY_KEEP_GLOBAL_PWR) != 0U) {
        cfg->pdsCtl.pdsPwrOff = 0;
    }

    if ((mask & PM_PDS_DIAG_MISC_PWR_OFF) != 0U) {
        cfg->pdsCtl4.MiscPwrOff = 1;
    }
    if ((mask & PM_PDS_DIAG_MISC_RST) != 0U) {
        cfg->pdsCtl4.MiscRst = 1;
    }
    if ((mask & PM_PDS_DIAG_MISC_GATE) != 0U) {
        cfg->pdsCtl4.MiscGateClk = 1;
    }
    if ((mask & PM_PDS_DIAG_MISC_ISO) != 0U) {
        cfg->pdsCtl3.MiscIsoEn = 1;
    }
    if ((mask & PM_PDS_DIAG_BLE_RST) != 0U) {
        cfg->pdsCtl4.BleRst = 1;
    }
    if ((mask & PM_PDS_DIAG_BLE_GATE) != 0U) {
        cfg->pdsCtl4.BleGateClk = 1;
    }
    if ((mask & PM_PDS_DIAG_BLE_ISO) != 0U) {
        cfg->pdsCtl3.BleIsoEn = 1;
    }
    if ((mask & PM_PDS_DIAG_RF_PWR_OFF) != 0U) {
        cfg->pdsCtl.pdsCtlRfSel = 1;
    }
    if ((mask & PM_PDS_DIAG_BZ_RST) != 0U) {
        cfg->pdsCtl4.BzRst = 1;
    }
    if ((mask & PM_PDS_DIAG_BZ_GATE) != 0U) {
        cfg->pdsCtl4.BzGateClk = 1;
    }
    if ((mask & PM_PDS_DIAG_BZ_ISO) != 0U) {
        cfg->pdsCtl3.BzIsoEn = 1;
    }
}
#endif

/* Legacy sleep paths force-select RC32K on entry unless an application takes
 * explicit ownership with pm_pds_set_32k_src().  The controller qualification
 * path does that before rwip_init(): it selects XTAL32K, measures it against
 * XTAL32M, and falls back to calibrated RC32K only when the measured error
 * exceeds the BLE sleep-clock budget.  Do not reintroduce unconditional RC
 * selection in the IPRO7 controller path; current hardware PDS wake validation
 * is based on the selected/qualified 32 kHz source, not a hardcoded RC path. */
static uint8_t s_32k_force_rc = 1;
static uint8_t s_32k_use_xtal = 0;

void pm_pds_set_32k_src(uint8_t use_xtal, uint32_t settle_ms)
{
    if (use_xtal) {
        HBN_Power_On_Xtal_32K();
        if (settle_ms) {
            arch_delay_ms(settle_ms);
        }
        HBN_32K_Sel(HBN_32K_XTAL);
        s_32k_use_xtal = 1;
    } else {
        /* Same operation, different spelling per chip: IPRO6 calls it
         * HBN_Keep_On_RC32K(), IPRO7 HBN_Power_On_RC32K(). Neither header
         * declares the other, so an unguarded call breaks the sibling build —
         * boot2 compiles this file for BOTH chips. */
#if defined(IPRO6)
        HBN_Keep_On_RC32K();
#else
        HBN_Power_On_RC32K();
#endif
        HBN_32K_Sel(HBN_32K_RC);
        s_32k_use_xtal = 0;
    }
    s_32k_force_rc = 0;
}

void pm_pds_get_32k_src(uint8_t *use_xtal, uint8_t *forced_rc)
{
    if (use_xtal) {
        /* HBN_CTL is not the selector — F32K_SEL is HBN_GLB[4:3]. */
        uint32_t sel = reg_get_bits(readl(HBN_BASE + HBN_GLB_OFFSET), HBN_F32K_SEL);
        *use_xtal = (uint8_t)(sel == HBN_32K_XTAL);
    }
    if (forced_rc) {
        *forced_rc = s_32k_force_rc;
    }
}

/* -----------------------------------------------------------------------
 * PDS entry
 * ----------------------------------------------------------------------- */

/**
 * @brief Configure hardware and enter PDS sleep.
 *
 * @param cfg  Pointer to PM_PDS_CFG_Type (cast to uint32_t* for legacy API).
 *
 * Flow:
 *   1. If PSRAM present — put PSRAM into hybrid-sleep, adjust avdd14/bgSys
 *   2. Select PDS level configuration struct
 *   3. Program PDS hardware and issue WFI
 *   4. On wakeup, execution resumes after WFI (if cpuRst=1, bootrom runs first)
 */
void ATTR_TCM_SECTION pm_pds_enable(uint32_t *cfg)
{
    PM_PDS_CFG_Type *p = (PM_PDS_CFG_Type *)cfg;
    PDS_DEFAULT_LV_CFG_Type *pPdsCfg = NULL;
    PDS_DEFAULT_LV_CFG_Type pdsCfgEffective;

#if defined(CONFIG_USE_PSRAM) && !defined(IPRO6)
    /* IPRO6 has no internal PSRAM, and this keep-alive uses registers/fields
     * (PDS_PSRAM_IEOE/O offsets, pdsCtl.avdd14Off) that exist only on chips
     * with an on-die PSRAM controller — so it is excluded from IPRO6 builds.
     *
     * hal_lp_enter() already performs a broad cache clean/invalidate before
     * it starts the PDS entry sequence, but more state is written afterwards:
     * LP parameters, saved CPU context, task stack spill, and possibly
     * controller/host state reached through late callbacks.  Wake-up later
     * invalidates D-cache unconditionally, so any dirty cacheable PSRAM line
     * not written back before PSRAM hybrid-sleep can be lost and reappear as
     * stale controller state after resume.  Do the final write-back at the
     * last point before PSRAM is parked. */
    pm_pds_hbn_arm_diag[34] = UINT32_C(0x50434c4e); /* "PCLN" */
    L1C_DCACHE_CLEAN();
    __RWMB();
    pm_pds_hbn_arm_diag[35] = UINT32_C(0x50434c44); /* "PCLD" */

    /* Put PSRAM into hybrid-sleep and float its I/O pins */
    hal_psram_enter_hybridsleep();
    GLB_Set_PSRAMB_CLK_Sel(DISABLE, GLB_PSRAMB_PLL_480M, 0);
    writel(0xfff << 16, PDS_BASE + PDS_PSRAM_IEOE_OFFSET);
    writel(0xfff << 0,  PDS_BASE + PDS_PSRAM_O_OFFSET);

#endif

    cpu_global_irq_disable();
    PM_TRACE('M');

    switch (p->pdsLevel) {
    case PM_PDS_LEVEL_31:
        /* On IPRO6 this selector maps to the single pds15-equivalent config;
         * IPRO7 keeps pdsCfgLevel31. */
#if defined(IPRO6)
        pPdsCfg = &pdsCfgLevel15;
#else
        pPdsCfg = &pdsCfgLevel31;
#endif
        break;
    default:
        cpu_global_irq_enable();
        return;
    }
    PM_TRACE('N');

    /* Work on a per-entry copy.  BLE connected-PDS and PSRAM self-refresh are
     * application/product policies layered onto this single sleep transaction;
     * they must not mutate the BSP level-31 defaults globally.  Otherwise an
     * app that disables BLE PDS after one cycle would silently keep the prior
     * RF/BZ/MISC settings for every later pm_pds_mode_enter(). */
    memcpy(&pdsCfgEffective, pPdsCfg, sizeof(pdsCfgEffective));
    pPdsCfg = &pdsCfgEffective;

#if defined(CONFIG_USE_PSRAM) && !defined(IPRO6)
    /* PSRAM needs AVDD14 and bandgap to stay on for self-refresh. */
    pPdsCfg->pdsCtl.avdd14Off = 0;
    pPdsCfg->pdsCtl.bgSysOff = 0;
#endif

#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
    pm_pds_apply_ble_ctlr_domain_override(pPdsCfg);
#endif
    /* Raw register content about to be written, in case the config struct
     * itself is somehow different from the known-good standalone case.
     * memcpy rather than a pointer cast: the bitfield structs alias uint32_t
     * for the writel() calls inside PDS_Force_Config()/PDS_Enable() too, but
     * -Werror=strict-aliasing only flags it through an explicit dereference
     * here, not through their own (uint32_t *)cfg parameter types. */
    {
        uint32_t word;

        memcpy(&word, &pPdsCfg->pdsCtl, sizeof(word));
        pm_pds_last_domain_words[0] = word;
        pm_pds_hbn_arm_diag[15] = word;
        PM_TRACE_HEX(word);
        memcpy(&word, &pPdsCfg->pdsCtl2, sizeof(word));
        pm_pds_last_domain_words[1] = word;
        pm_pds_hbn_arm_diag[16] = word;
        PM_TRACE_HEX(word);
        memcpy(&word, &pPdsCfg->pdsCtl3, sizeof(word));
        pm_pds_last_domain_words[2] = word;
        pm_pds_hbn_arm_diag[17] = word;
        PM_TRACE_HEX(word);
        memcpy(&word, &pPdsCfg->pdsCtl4, sizeof(word));
        pm_pds_last_domain_words[3] = word;
        pm_pds_hbn_arm_diag[18] = word;
        PM_TRACE_HEX(word);
    }
    pm_pds_hbn_arm_diag[14] =
#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
        pm_pds_diag_domain_override;
#else
        0U;
#endif
    pm_pds_hbn_arm_diag[29] =
#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO)
        pm_pds_ble_ctlr_domain_policy;
#else
        0U;
#endif
    PM_TRACE_HEX(p->sleepTime);

#if !defined(IPRO6) && defined(CONFIG_BT_CTLR_IPRO) && defined(CONFIG_OCRAM_NORET_SPLIT)
    if (p->ocramRetetion &&
        ((pm_pds_ble_ctlr_domain_policy &
          PM_PDS_BLE_CTLR_DOMAIN_POLICY_SAFE_OCRAM) != 0U)) {
        pm_pds_config_ocram_safe_retention();
    }
#endif
    PDS_Default_Level_Config(pPdsCfg, p->sleepTime);
    /* Index 0 is no longer consumed after the RTC arm sequence and, unlike
     * stage index 8, the LP wake framework does not overwrite it. */
    pm_pds_hbn_arm_diag[0] = readl(PDS_BASE + PDS_CTL_OFFSET);
    pm_pds_last_domain_words[4] = readl(PDS_BASE + PDS_CTL_OFFSET);
    pm_pds_last_domain_words[5] = readl(PDS_BASE + PDS_CTL2_OFFSET);
    pm_pds_last_domain_words[6] = readl(PDS_BASE + PDS_CTL3_OFFSET);
    pm_pds_last_domain_words[7] = readl(PDS_BASE + PDS_CTL4_OFFSET);
    pm_pds_hbn_arm_diag[19] = pm_pds_last_domain_words[4];
    pm_pds_hbn_arm_diag[20] = pm_pds_last_domain_words[5];
    pm_pds_hbn_arm_diag[21] = pm_pds_last_domain_words[6];
    pm_pds_hbn_arm_diag[22] = pm_pds_last_domain_words[7];
    pm_pds_hbn_arm_diag[23] = readl(PDS_BASE + PDS_STAT_OFFSET);
    pm_pds_clean_hbn_diag();
    PM_TRACE('O');

    /*
     * Neutralize the machine-timer (CLINT mtimecmp) before issuing WFI.
     *
     * cpu_global_irq_disable() only clears mstatus.MIE — it does NOT stop the
     * FreeRTOS tick. RISC-V WFI wakes on any pending interrupt (mip & mie)
     * regardless of MIE, so a still-pending MTIP makes __WFI() fall through
     * immediately: the core never enters the WFI low-power state, np_wfi never
     * asserts, and the PDS controller never powers down. Pushing the compare
     * to its maximum keeps MTIP clear so WFI actually stalls and PDS engages.
     * The wakeup itself comes from the separate 32 kHz PDS/HBN RTC compare
     * programmed in pm_pds_mode_enter(); on PDS wake the CPU cold-restarts
     * and startup re-initializes the tick, so no mtimecmp restore is needed here.
     */
    SysTimer_SetCompareValue(0xFFFFFFFFFFFFFFFFULL);

#if PM_PDS_WFI_TRACE
    /* Diagnostic only, default off. Bisects a hang where hal_lp_enter() never
     * returns: this file has no unbounded busy-wait anywhere in the entry
     * path, so __WFI() is the only place the CPU can actually park. 'W'
     * proves execution reached this instruction; 'w' proves WFI fell through
     * without the PDS controller ever powering the core down (a config or
     * pending-interrupt issue, still inside this C function). 'W' with no
     * 'w' and no wake-side trace afterward means the CPU truly parked here
     * and the failure is in the PDS/HBN hardware sequencer or the bootrom
     * wake jump — outside anything software can single-step or print through,
     * since nothing runs on this core again until either wake succeeds or the
     * board is reset.
     *
     * The queue-and-go version of this (no drain) shipped first and produced
     * an ambiguous result: no 'W' printed, which could mean either "never
     * reached this line" or "reached it, but MISC/UART lost power before the
     * shift register finished clocking the byte out" — queuing into the FIFO
     * is not the same as the byte having left the pin. Draining here closes
     * that gap: bounded so it cannot itself hang (same discipline as
     * lp_trace_drain() in lp_test_power.c), and short enough — a handful of
     * bit-times at any UART baud rate in use — to be very unlikely to be
     * itself the difference between a run that hangs and one that does not.
     *
     * Register offsets duplicated from lp_uart.c, which keeps them
     * file-private. */
    {
        volatile uint32_t *tx_cnt = (volatile uint32_t *)(UART0_BASE + 0x84);
        volatile uint32_t *tx_wr = (volatile uint32_t *)(UART0_BASE + 0x88);
        uint32_t spin;

        if ((*tx_cnt & 0x1Fu) != 0u) {
            *tx_wr = (uint32_t)'W';
        }
        spin = 200000;
        while ((*tx_cnt & 0x1Fu) != 0x1Fu && --spin) {
            /* Wait for the FIFO to report empty (0x1F = all slots free). */
        }
    }
#endif

    __WFI();

#if PM_PDS_WFI_TRACE
    {
        volatile uint32_t *tx_cnt = (volatile uint32_t *)(UART0_BASE + 0x84);
        volatile uint32_t *tx_wr = (volatile uint32_t *)(UART0_BASE + 0x88);

        if ((*tx_cnt & 0x1Fu) != 0u) {
            *tx_wr = (uint32_t)'w';
        }
    }
#endif

    cpu_global_irq_enable();
}

/* -----------------------------------------------------------------------
 * PDS mode public API
 * ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
 * PDS GPIO wakeup
 *
 * PDS31 resets and clock-gates PD_CORE_MISC_DIG but leaves the GPIO pad logic
 * powered, so any of GPIO0..41 can wake it - unlike HBN, where only the ten
 * AON pads have a path. The hardware groups the pins four at a time and the
 * trigger is per group, so pins in one group necessarily share a trigger.
 *
 * IPRO7 only. PDS_GPIO_GROUP_SET_Type, PDS_GPIO_Cfg_Type, PDS_Set_GPIO_Pad_Cfg
 * and the PDS_GPIO_* register offsets exist in bsp/drivers/ipro7_soc alone, and
 * the one caller (apps/multimedia/ipro_pdm_mic_demo) is an IPRO7 app, so the
 * whole section compiles out rather than growing IPRO6 stubs. The arm() call in
 * pm_pds_mode_enter() is already inside that function's !IPRO6 region.
 * ----------------------------------------------------------------------- */
#if !defined(IPRO6)
static uint64_t s_pds_gpio_wake_mask;
static uint8_t  s_pds_gpio_wake_trig[PDS_GPIO_GROUP_SET_GPIO40_GPIO43 + 1];
static uint32_t s_pds_gpio_wake_groups;

static int pm_pds_gpio_group(uint8_t pin, PDS_GPIO_GROUP_SET_Type *grp)
{
    if (pin <= 3U)                     { *grp = PDS_GPIO_GROUP_SET_GPIO0_GPIO3;   return 0; }
    if (pin >= 4U  && pin <= 7U)       { *grp = PDS_GPIO_GROUP_SET_GPIO4_GPIO7;   return 0; }
    if (pin >= 8U  && pin <= 9U)       { *grp = PDS_GPIO_GROUP_SET_GPIO8_GPIO9;   return 0; }
    if (pin >= 20U && pin <= 23U)      { *grp = PDS_GPIO_GROUP_SET_GPIO20_GPIO23; return 0; }
    if (pin >= 24U && pin <= 27U)      { *grp = PDS_GPIO_GROUP_SET_GPIO24_GPIO27; return 0; }
    if (pin >= 28U && pin <= 31U)      { *grp = PDS_GPIO_GROUP_SET_GPIO28_GPIO31; return 0; }
    if (pin >= 32U && pin <= 35U)      { *grp = PDS_GPIO_GROUP_SET_GPIO32_GPIO35; return 0; }
    if (pin >= 36U && pin <= 39U)      { *grp = PDS_GPIO_GROUP_SET_GPIO36_GPIO39; return 0; }
    if (pin >= 40U && pin <= 41U)      { *grp = PDS_GPIO_GROUP_SET_GPIO40_GPIO43; return 0; }
    /* GPIO10..19 have no PDS interrupt group in this part. */
    return -1;
}

int pm_pds_gpio_wakeup_set(uint8_t pin, uint8_t trigger)
{
    PDS_GPIO_GROUP_SET_Type grp;
    PDS_GPIO_Cfg_Type pad;

    if (pin > 41U || pm_pds_gpio_group(pin, &grp) != 0) {
        return -1;
    }

    /*
     * During PDS the pads are driven from PDS's own PU/PD/IE registers, not
     * from the ordinary GPIO configuration. Without ie_en the input buffer is
     * off and the pin's level simply is not observed, so the wake never fires
     * however the trigger is programmed - which is exactly what grounding the
     * pin produced: nothing.
     *
     * The pull-up is enabled even when the board already has one. Measured on
     * an EVB whose buttons carry a 20k pull-up: with PDS_GPIO_PU_SET clear,
     * PDS_GPIO_STAT read 0 for those pins both released and grounded - the
     * PDS domain does not see the external pull. Against a LOW-level trigger
     * that reads as "already asserted", so the part woke the instant sleep was
     * armed. Callers that genuinely need a pull-down should get an argument
     * for it rather than have this guess.
     */
    pad.ie_en = 1U;
    pad.oe_en = 0U;
    pad.pu_en = 1U;
    pad.pd_en = 0U;
    if (PDS_Set_GPIO_Pad_Cfg((PDS_GPIO_Type)pin, &pad) != 0) {
        return -1;
    }
    s_pds_gpio_wake_mask |= (uint64_t)1U << pin;
    s_pds_gpio_wake_trig[grp] = trigger;
    s_pds_gpio_wake_groups |= 1U << (uint32_t)grp;
    return 0;
}

void pm_pds_gpio_wakeup_clear(void)
{
    s_pds_gpio_wake_mask = 0U;
    s_pds_gpio_wake_groups = 0U;
}

static void pm_pds_gpio_wakeup_arm(void);

void pm_pds_gpio_wakeup_dump(void)
{
    /* Apply the arming first: mask, trigger and wake-source enables are all
     * programmed by the entry path, so a dump taken before that shows the
     * unarmed state and says nothing about what sleep would see. */
    pm_pds_gpio_wakeup_arm();

    uint32_t stat = readl(PDS_BASE + PDS_GPIO_STAT_OFFSET);
    uint32_t ie   = readl(PDS_BASE + PDS_GPIO_IE_SET_OFFSET);
    uint32_t mask = readl(PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);
    uint32_t trig = readl(PDS_BASE + PDS_GPIO_INT_SET_OFFSET);
    uint32_t pu   = readl(PDS_BASE + PDS_GPIO_PU_SET_OFFSET);

    printf("\r\nPDS_GPIO_STAT     0x%08lx  (pad level as PDS sees it)\r\n",
           (unsigned long)stat);
    printf("PDS_GPIO_IE_SET   0x%08lx  (1 = input buffer on)\r\n",
           (unsigned long)ie);
    printf("PDS_GPIO_INT_MASK 0x%08lx  (1 = masked)\r\n",
           (unsigned long)mask);
    printf("PDS_GPIO_INT_SET  0x%08lx  (4 bits per group of pins)\r\n",
           (unsigned long)trig);
    printf("PDS_GPIO_PU_SET   0x%08lx\r\n", (unsigned long)pu);
    printf("armed mask        0x%08lx\r\n",
           (unsigned long)(uint32_t)s_pds_gpio_wake_mask);
    printf("wake src GPIO_IRQ %d   PDS_IO_INT %d   HBN %d   WDG %d\r\n",
           PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_GPIO_IRQ),
           PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_PDS_IO_INT),
           PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_HBN_IRQ_OUT),
           PDS_Get_Wakeup_Src(PDS_WAKEUP_SRC_WDG_TIMEOUT));
    for (uint8_t pin = 5U; pin <= 7U; pin++) {
        printf("  GPIO%u  pds_level %lu  pds_int %d\r\n", pin,
               (unsigned long)((stat >> pin) & 1U),
               PDS_Get_GPIO_Pad_IntStatus((PDS_GPIO_Type)pin));
    }
}

int pm_pds_gpio_wakeup_source(void)
{
    for (uint8_t pin = 0U; pin <= 41U; pin++) {
        if ((s_pds_gpio_wake_mask & ((uint64_t)1U << pin)) == 0U) {
            continue;
        }
        if (PDS_Get_GPIO_Pad_IntStatus((PDS_GPIO_Type)pin)) {
            return (int)pin;
        }
    }
    return -1;
}

/* Program the armed groups and enable the GPIO wake path. Called from the
 * PDS entry path after every source has been masked. */
static void pm_pds_gpio_wakeup_arm(void)
{
    if (s_pds_gpio_wake_groups == 0U) {
        return;
    }
    for (uint32_t g = 0; g <= (uint32_t)PDS_GPIO_GROUP_SET_GPIO40_GPIO43; g++) {
        if ((s_pds_gpio_wake_groups & (1U << g)) == 0U) {
            continue;
        }
        PDS_Set_GPIO_Pad_IntClr((PDS_GPIO_GROUP_SET_Type)g);
        PDS_Set_GPIO_Pad_IntMode((PDS_GPIO_GROUP_SET_Type)g,
                                 (PDS_GPIO_INT_TRIG_Type)s_pds_gpio_wake_trig[g]);
    }

    /*
     * Clear the GLB GPIO latch for the armed pins first.
     *
     * GLB's per-pin interrupt status stays set until an ISR acknowledges it,
     * and PDS_WAKEUP_SRC_GPIO_IRQ is fed from that line - so a button pressed
     * before `pds` leaves a latch that fires the moment sleep is armed, and
     * the part wakes immediately without anyone touching it.
     */
    for (uint8_t pin = 0U; pin <= 41U; pin++) {
        if ((s_pds_gpio_wake_mask & ((uint64_t)1U << pin)) != 0U) {
            GLB_GPIO_IntClear((int)pin, 1);
            GLB_GPIO_IntClear((int)pin, 0);
        }
    }

    /*
     * Unmask the armed pins. PDS_GPIO_INT_MASK is one bit per GPIO and no
     * driver function touches it, so it keeps its reset value - everything
     * masked. Programming the trigger alone leaves the interrupt permanently
     * suppressed, which is what a correctly configured pad that still never
     * wakes looks like.
     */
    {
        uint32_t mask = readl(PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);
        for (uint8_t pin = 0U; pin <= 41U; pin++) {
            if ((s_pds_gpio_wake_mask & ((uint64_t)1U << pin)) != 0U) {
                mask &= ~(1U << pin);       /* 0 = unmasked */
            }
        }
        writel(mask, PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);
    }

    /*
     * Both GPIO-ish wake sources are enabled.
     *
     * PDS_WAKEUP_SRC_GPIO_IRQ is the GLB gpio_irq line, and PDS31 resets
     * PD_CORE_MISC_DIG - where the GPIO controller lives - so that line is not
     * obviously alive during sleep. PDS_WAKEUP_SRC_PDS_IO_INT is fed by the
     * PDS-domain sensing this function programs. Which one the part actually
     * routes is not documented here, they cost one always-on comparator each,
     * and pm_pds_gpio_wakeup_source() reports the pin either way.
     */
    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_GPIO_IRQ, 1);
    PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_PDS_IO_INT, 1);
}
#endif /* !IPRO6 */


/**
 * @brief Enter PDS sleep mode.
 *
 * @param pds_level  PDS level (currently only PM_PDS_LEVEL_31).
 * @param sleep_time Sleep duration in RTC ticks (32.768 kHz).
 *                   Pass 0 for indefinite sleep (wakeup by GPIO/BLE only).
 *
 * Before entering PDS:
 *   - Configures RTC compare for timed wakeup (if sleep_time > 0)
 *   - Clears all pending PDS/HBN GPIO and interrupt flags
 */
void pm_pds_mode_enter(enum pm_pds_sleep_level pds_level, uint32_t sleep_time)
{
#if !defined(IPRO6)
    /* IPRO7 path — restored from the pre-IPRO6-rewrite implementation (stubbed
     * by 22ffb1cde when the rewrite pulled in IPRO6-only symbols). Deliberate
     * differences from the IPRO6 branch below:
     *   - no HBN_Keep_On_RC32K(): symbol does not exist on IPRO7; RC32K stays
     *     powered by hardware default (validated pre-rewrite flow never set it).
     *   - no HBN_32K_Sel(): on IPRO7 it re-applies the efuse/ROM RC32K trim,
     *     clobbering any runtime hal_rc32k_force_cal() result — far more than
     *     the 500 ppm SCA budget a BLE connected sleep can absorb.
     *   - no PDS_WAKEUP_SRC_PDS_TIMER: the enum value does not exist on IPRO7
     *     (PDS_Enable() ignores pdsSleepCnt; there is no PDS_TIME1 register);
     *     HBN RTC compare via HBN_IRQ_OUT is the only timed-wake source. */
    PM_PDS_CFG_Type cfg = {
        .pdsLevel       = pds_level,
        .turnOffRF      = PM_PDS_RF_POWER_OFF,
        .powerDownFlash = PM_PDS_FLASH_POWER_OFF,
        .ocramRetetion  = 1,
        .turnoffPLL     = PM_PDS_PLL_POWER_OFF,
        .turnoffDLL     = PM_PDS_DLL_POWER_OFF,
        .pdsLdoEn       = PM_PDS_PLL_POWER_OFF,
        .flashContRead  = 0,
        .sleepTime      = sleep_time,
        .fastRecovery   = 0,
        .flashCfg       = NULL,
        .ldoLevel       = PM_PDS_LDO_LEVEL_DEFAULT,
        .preCbFun       = NULL,
        .postCbFun      = NULL,
    };

    uint32_t flash_cfg_len;
    flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
    cfg.flashCfg = (uint32_t *)&flash_cfg;
#pragma GCC diagnostic pop
    PM_TRACE('G');

    /* The PDS/HBN AON sequencer is clocked from the 32 kHz domain; the RTC
     * counter must be running before the compare target is derived from it. */
    if (!s_32k_use_xtal) {
        /* RC32K-only boards must keep the RC oscillator alive across PDS so
         * the HBN RTC compare can wake the core.  Do not call HBN_32K_Sel()
         * here: on IPRO7 it reapplies efuse trim and would discard the
         * runtime RC32K calibration selected by the BLE low-power init. */
        HBN_Power_On_RC32K();
    }
    HBN_Enable_RTC_Counter();

    /* Mask every wakeup source first, then arm only the timed-wake path. A
     * stale enable keeps its always-on detect logic clocked through PDS,
     * risking a spurious early wake and raising the sleep floor. */
    PDS_Wakeup_Src_En_DisableAll();
    HBN_Pin_WakeUp_Mask(0x3FF);
    PM_TRACE('H');

    /* Ack stale wake state before arming the RTC comparator.  In particular,
     * never clear HBN IRQs after HBN_Set_RTC_Timer(): on repeated IPRO7 PDS31
     * entries the RTC clear also drops HBN_CTL compare mode [3:1].  The board
     * then reaches WFI with no live timed-wake source (observed on cycle 4 as
     * HBN_CTL=...01 and enter/wake/restore=1/0/0). */
    HBN_Disable_RTC_Compare();
    PDS_Set_All_GPIO_IntClear();
    PDS_IntClear();
    HBN_Clear_All_IRQ();
    /* After the blanket clear, so the arm is not immediately wiped. */
    pm_pds_gpio_wakeup_arm();
    pm_pds_hbn_arm_diag[0] = readl(HBN_BASE + HBN_CTL_OFFSET);
    pm_pds_hbn_arm_diag[1] = readl(HBN_BASE + HBN_IRQ_STAT_OFFSET);

    /* Configure RTC wakeup */
    if (sleep_time > 0) {
        uint32_t rtc_low, rtc_high;
        uint64_t rtc_now, rtc_target;
        HBN_RTC_INT_Delay_Type rtc_delay = HBN_RTC_INT_DELAY_32T;

        PM_TRACE('I');
        HBN_Get_RTC_Timer_Val(&rtc_low, &rtc_high);
        PM_TRACE('J');
        PM_TRACE_HEX(rtc_high);
        PM_TRACE_HEX(rtc_low);
        rtc_now = ((uint64_t)rtc_high << 32) | rtc_low;
        if (s_pm_pds_rtc_target_override != 0U) {
            const uint64_t rtc_mask = (UINT64_C(1) << 45) - UINT64_C(1);
            const uint64_t override_target =
                s_pm_pds_rtc_target_override & rtc_mask;
            const uint64_t delta = (override_target - rtc_now) & rtc_mask;

            s_pm_pds_rtc_target_override = 0U;
            if ((delta != 0U) && (delta < (UINT64_C(1) << 44))) {
                rtc_target = override_target;
                rtc_delay = HBN_RTC_INT_DELAY_0T;
            } else {
                rtc_target = (rtc_now + sleep_time) & rtc_mask;
            }
        } else {
            rtc_target = rtc_now + sleep_time;
        }
        pm_pds_hbn_arm_diag[2] = rtc_low;
        pm_pds_hbn_arm_diag[3] = rtc_high;
        pm_pds_hbn_arm_diag[4] = (uint32_t)rtc_target;
        pm_pds_hbn_arm_diag[5] = (uint32_t)(rtc_target >> 32);
        PM_TRACE_HEX((uint32_t)(rtc_target >> 32));
        PM_TRACE_HEX((uint32_t)(rtc_target & 0xFFFFFFFFu));

        /* An absolute shared deadline must not inherit the 32T (~1 ms)
         * comparator delay used by the legacy relative-timer path. */
        HBN_Set_RTC_Timer(rtc_delay,
                          rtc_target & 0xFFFFFFFF,
                          (rtc_target >> 32) & 0x1fff, 1);
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT, 1);
        pm_pds_hbn_arm_diag[6] = readl(HBN_BASE + HBN_CTL_OFFSET);
        pm_pds_hbn_arm_diag[7] = readl(PDS_BASE + PDS_INT_OFFSET);
        pm_pds_hbn_arm_diag[8] = UINT32_C(0x50445330); /* "PDS0": armed */
        PM_TRACE('K');
    }

    /* The compare is armed now.  Do not acknowledge HBN RTC here: that can
     * disarm the just-programmed compare on this silicon. */
    PM_TRACE('L');

    pm_pds_enable((uint32_t *)&cfg);
#else
    PM_PDS_CFG_Type cfg = {
        .pdsLevel       = pds_level,
        .turnOffRF      = PM_PDS_RF_POWER_OFF,
        .powerDownFlash = PM_PDS_FLASH_POWER_OFF,
        .ocramRetetion  = 1,
        .turnoffPLL     = PM_PDS_PLL_POWER_OFF,
        .turnoffDLL     = PM_PDS_DLL_POWER_OFF,
        .pdsLdoEn       = PM_PDS_PLL_POWER_OFF,
        .flashContRead  = 0,
        .sleepTime      = sleep_time,
        .fastRecovery   = 0,
        .flashCfg       = NULL,
        .ldoLevel       = PM_PDS_LDO_LEVEL_DEFAULT,
        .preCbFun       = NULL,
        .postCbFun      = NULL,
    };

    uint32_t flash_cfg_len;
    flash_get_cfg((uint8_t **)&flash_cfg, &flash_cfg_len);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress-of-packed-member"
    cfg.flashCfg = (uint32_t *)&flash_cfg;
#pragma GCC diagnostic pop

    /* Power on and select the 32 kHz RC oscillator and start the RTC counter.
     * The PDS/HBN AON sequencer is clocked by this 32 kHz: with it dead the PDS
     * state machine stays frozen in IDLE — pdsStart never latches into PDS_CTL,
     * the sleep timer never counts, and the wake compare never fires. Must run
     * BEFORE reading the RTC value so the compare delta is taken off a live
     * counter. Mirrors the reference rtc_init (RC32K path). */
    HBN_Keep_On_RC32K();
    if (s_32k_force_rc) {
        HBN_32K_Sel(HBN_32K_RC);
    }
    HBN_Enable_RTC_Counter();

    /* Mask EVERY wakeup source first, then enable only the timed-wake path
     * (PDS timer + HBN RTC) below. A stale PDS wakeup source or an unmasked HBN
     * pad-wake keeps its always-on detect logic clocked through PDS, which both
     * risks a spurious early wake and holds a sense/clock domain alive — pulling
     * the sleep floor up. The HBN enter path already does this; the PDS path
     * must too. */
    PDS_Wakeup_Src_En_DisableAll();
    HBN_Pin_WakeUp_Mask((uint8_t)0xFF);

    /* Configure RTC wakeup */
    if (sleep_time > 0) {
        uint32_t rtc_low, rtc_high;
        uint64_t rtc_now, rtc_target;

        /* Ack a stale RTC IRQ BEFORE arming — doing it after is fatal.
         * HBN_Clear_RTC_INT() acks by clearing HBN_CTL[3:1] (HBN_RTC_CTL compare
         * mode), which is the SAME field HBN_Set_RTC_Timer() writes to arm the
         * comparator. The ack used to sit below with the PDS int clears, i.e.
         * after this arm, so it disarmed the compare and
         * PDS_WAKEUP_SRC_HBN_IRQ_OUT could never fire. Measured 2026-08-12:
         * RTC-only wake scored 0/4 even on RC32K. */
        HBN_Clear_RTC_INT();

        HBN_Get_RTC_Timer_Val(&rtc_low, &rtc_high);
        rtc_now = ((uint64_t)rtc_high << 32) | rtc_low;
        rtc_target = rtc_now + sleep_time;

        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_32T,
                          rtc_target & 0xFFFFFFFF,
                          (rtc_target >> 32) & 0x1fff, 1);
        /* Both timed-wake sources. PDS_TIMER (pds_sleep_cnt == 0) is driven by
         * PDS_TIME1, which PDS_Enable() programs from sleepTime. HBN_IRQ_OUT is
         * the HBN RTC compare — a real second source now that the ack above no
         * longer disarms it; measured 2026-08-12 it wakes 4/4 on its own. Both
         * live in PDS_INT[19:10].
         *
         * Neither fires when F32K_SEL = XTAL32K (0/4 each, same build): the HBN
         * RTC keeps perfect time on the crystal but the PDS/AON sequencer never
         * sequences, so PDS is RC32K-only on this part. That is why the select
         * above force-picks RC. */
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_PDS_TIMER, 1);
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT, 1);
    }

    /* Clear all pending wakeup sources. The IPRO6 RTC ack is NOT done here — it
     * would clear HBN_CTL[3:1] and disarm the compare armed above; it runs
     * before the arm instead (see HBN_Clear_RTC_INT() in the block above). */
    PDS_Set_All_GPIO_IntClear();
    PDS_IntClear();
#if !defined(IPRO6)
    HBN_Clear_All_IRQ();
#endif

    pm_pds_enable((uint32_t *)&cfg);
#endif /* !IPRO6 */
}

/**
 * @brief Re-enter PDS from the wake path without resuming the full firmware.
 *
 * The DTIM loop uses this: when lp_fw decides SLEEP, the full firmware's saved
 * resume context (cpu_regs in HBN RAM) must stay untouched while we sleep
 * another beacon interval. Runs entirely from TCM — there is no flash XIP on the
 * wake path yet — and never returns; the next PDS wake re-enters
 * hal_lp_wakeup_entry exactly as the first sleep did.
 *
 * This is the IPRO6 port of the reference lp_fw "pds15" enter (the wake-path
 * re-sleep the reference firmware uses between beacons). It reuses the proven
 * first-sleep PDS mechanism (the pds15 config with cpuWfiMask=1, then
 * PDS_Default_Level_Config + mtimecmp-max + WFI, exactly as pm_pds_enable) plus
 * the few extras the wake context needs, which pm_pds_mode_enter does not do:
 *
 *   THE keystone — AON_Power_On_XTAL(): power the crystal back on and WAIT for it
 *   to be ready. The PDS config uses waitXtalRdy=1, so the PDS sequencer stalls
 *   and ABORTS the power-down (~10 us dip on the rail, then the core resumes past
 *   WFI and spins) if XTAL has not settled when we enter — and on the wake path
 *   the Stage-1 hook runs before any full clock bring-up, so XTAL is NOT ready.
 *   Without this the re-sleep never sleeps; with it the power-down completes and
 *   the wake comes up on a clean crystal. The reference lp_fw does the same wait
 *   in its wake entry. (The first sleep runs from full-fw context where XTAL is
 *   already up, so pm_pds_enable does not need it.)
 *
 *   Other wake-context extras, on top of the proven wake-source arming (RC32K +
 *   RTC counter + HBN RTC compare + PDS timer):
 *     - Ack the latched PDS reset event (PDS_STAT.PDS_CLR_RESET_EVENT) left by the
 *       wake that brought us here.
 *     - Re-arm the bootrom fast-wake vector: the bootrom clears the HBN_RSV0
 *       enter-flag after taking the wake, so it must be re-asserted or the next
 *       PDS wake does not re-enter the HBN stub (RSV1, the address, survives).
 *     - ctrlGpioIePuPd if a GPIO wake is pending; flush D-cache (lp_fw bumped its
 *       DTIM counter in retained RAM with the cache on).
 *
 * Two reference pds15 steps are deliberately omitted on IPRO6:
 *   - AON_Set_LDO11_SOC_Sstart_Delay() — not present in the IPRO6 std_drv (an
 *     LDO11 soft-start latency tweak, not required for PDS to engage).
 *   - the PDS_RAM1[31] retention-clock write — setting it WEDGES resume on this
 *     DUT (OCRAM is retained by the HW default; see pdsCfgLevel15 above).
 *
 * Both the first sleep (pm_pds_enable) and this re-sleep use the SAME IPRO6
 * config, pdsCfgLevel15 — the chip has a single PDS level (the pds15-equivalent
 * of the reference part). The public selector PM_PDS_LEVEL_31 stays as a generic
 * "deepest level" token (PDS_Default_Level_Config takes the cfg struct, not a
 * level number; IPRO7 genuinely uses pdsCfgLevel31), so it is not renamed.
 *
 * Every primitive here is TCM- or ram_tcm-resident (AON_Power_On_XTAL is
 * ATTR_CLOCK_SECTION, which the linker places in ram_tcm), so all are reachable
 * with no flash XIP.
 *
 * @param sleep_time  next sleep duration in 32 kHz RTC ticks.
 */
void ATTR_TCM_SECTION pm_pds_resleep(uint32_t sleep_time)
{
#if !defined(IPRO6)
    /* IPRO7 build stub — the body below uses IPRO6-only symbols (AON_Power_On_XTAL,
     * pdsCfgLevel15, HBN_Keep_On_RC32K). No IPRO7 caller exists: the IPRO7 LP flow
     * (hal_lp_enter) always resumes the full firmware and re-enters sleep from
     * tickless idle rather than re-sleeping on the wake path. Port the
     * pdsCfgLevel31 equivalent if a DTIM-style wake-path re-sleep is ever needed. */
    (void)sleep_time;
#else
    /* (1) Keystone: power on the crystal and WAIT until it is ready. The PDS
     * power-down config uses waitXtalRdy=1 (the proven first-sleep behaviour for
     * a clean wake), so the PDS sequencer stalls/aborts if XTAL is not ready when
     * we enter — and on the wake path the Stage-1 hook runs before any full clock
     * bring-up, so XTAL has NOT settled. The reference lp_fw does the same wait in
     * its wake entry (poll xtal_rdy) before re-sleeping. Without this the re-sleep
     * power-down aborts in ~10 us (the core resumes after WFI and spins); with it,
     * the power-down completes and the wake comes up on a clean XTAL. */
    AON_Power_On_XTAL();

    cpu_global_irq_disable();

    /* Ack the latched PDS reset event (PDS_STAT.PDS_CLR_RESET_EVENT, bit31) so the
     * state machine is not held by the previous wake's event. */
    {
        uint32_t s0 = readl(PDS_BASE + PDS_STAT_OFFSET);
        writel(s0 | (1u << 31), PDS_BASE + PDS_STAT_OFFSET);
        writel(s0 & ~(1u << 31), PDS_BASE + PDS_STAT_OFFSET);
    }

    /* Re-assert the 32 kHz source + RTC counter. The PDS/HBN AON sequencer is
     * clocked by this 32 kHz; if it is not live the PDS state machine never
     * latches the sleep and the core never powers down (see pm_pds_mode_enter).
     * Re-doing it every cycle is idempotent and avoids depending on whether a
     * PDS wake leaves the selection intact. */
    HBN_Keep_On_RC32K();
    if (s_32k_force_rc) {
        HBN_32K_Sel(HBN_32K_RC);
    }
    HBN_Enable_RTC_Counter();

    /* Clear every stale wakeup source first. A leftover wakeup source latched
     * from the wake that brought us here would sit pending in the ECLIC; the
     * core must stay parked in WFI long enough for PDS to power-gate it, and a
     * pending-and-enabled interrupt makes WFI fall through, so the core resumes
     * before the gate completes (it spins at ~15 mA). */
    PDS_Wakeup_Src_En_DisableAll();
    HBN_Pin_WakeUp_Mask((uint8_t)0xFF);

    if (sleep_time > 0) {
        uint32_t rtc_low, rtc_high;
        uint64_t rtc_now, rtc_target;

        /* (2a) Clear the PREVIOUS cycle's latched RTC compare match before arming
         * the new one. The HBN RTC IRQ is latched in HBN_CTL[3:1] (the "delayed
         * RTC IRQ", see HBN_Clear_RTC_INT) and drives HBN_IRQ_OUT; HBN_Set_RTC_Timer
         * only OR-sets the compare-enable, it never clears a prior match. Since the
         * previous DTIM cycle woke us *on* an RTC match, that latch is still set, so
         * the PDS_WAKEUP_SRC_HBN_IRQ_OUT armed below would be satisfied immediately
         * and this re-sleep would wake at once instead of waiting sleep_time — the
         * whole DTIM burst then collapses into a few ms. Inlined (HBN_Clear_RTC_INT
         * is not TCM-resident and we run before XIP restore). */
        writel(readl(HBN_BASE + HBN_CTL_OFFSET) & 0xfffffff1u,
               HBN_BASE + HBN_CTL_OFFSET);

        /* (2b) Arm the SAME wake sources the proven first-sleep path arms: the HBN
         * RTC compare (the actual sleep_time timer on IPRO6 — the PDS internal
         * timer alone wakes almost immediately) routed to PDS via HBN_IRQ_OUT,
         * plus the PDS sleep-timer. */
        HBN_Get_RTC_Timer_Val(&rtc_low, &rtc_high);
        rtc_now = ((uint64_t)rtc_high << 32) | rtc_low;
        rtc_target = rtc_now + sleep_time;

        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_32T,
                          rtc_target & 0xFFFFFFFF,
                          (rtc_target >> 32) & 0x1fff, 1);
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_PDS_TIMER, 1);
        PDS_Wakeup_Src_En(PDS_WAKEUP_SRC_HBN_IRQ_OUT, 1);
    }

    /* Clear pending PDS interrupt status so a stale latch does not make WFI fall
     * through. We do NOT touch the HBN RTC IRQ here: coming out of the wake the
     * core has been cold-reset (mip == 0, verified), so there is nothing pending
     * to clear, and pulsing HBN_IRQ_CLR in this context faults the wake path. */
    PDS_Set_All_GPIO_IntClear();
    PDS_IntClear();

    /* If a GPIO wake is pending/configured, keep the pad IE/PU/PD alive across
     * sleep so the level is sensed — mirrors the reference pds15 enter. */
    if (readl(PDS_BASE + PDS_GPIO_I_SET_OFFSET) != 0) {
        pdsCfgLevel15.pdsCtl.ctrlGpioIePuPd = 1;
    }

    /* Re-arm the bootrom fast-wake vector for THIS re-sleep's wake. The reference
     * pds15 enter does this every cycle (blfw_pds_mode_enter -> set wakeup
     * callback). The first sleep set HBN_RSV1 = &hal_lp_wakeup_entry and
     * HBN_RSV0 = enter-flag; the bootrom consumes (clears) the RSV0 flag when it
     * takes the fast-wake on a PDS wake, so without re-asserting it the wake from
     * THIS sleep finds no flag and cold-boots / jumps nowhere instead of
     * re-entering the HBN stub — which is why the re-sleep powered down (PPK2 saw
     * the ~3 uA dip) but never resumed the loop. RSV1 (the address) survives in
     * the always-on HBN domain, so read it back and re-assert both. Raw writes:
     * pm_set_wakeup_callback() is not TCM-resident. */
    {
        uint32_t vec = readl(HBN_BASE + HBN_RSV1_OFFSET);
        writel(vec, HBN_BASE + HBN_RSV1_OFFSET);
        writel(0x4E424845u, HBN_BASE + HBN_RSV0_OFFSET);   /* HBN_STATUS_ENTER_FLAG */
    }

    /* lp_fw ran with D-cache enabled (hal_lp_main turns it on) and bumped its
     * DTIM counter in retained RAM; the core is about to power down, so write
     * dirty lines back and invalidate before WFI — exactly as hal_lp_enter()
     * does. Without this the counter increment is lost on power-down and the
     * loop never reaches its wake threshold. */
    L1C_DCACHE_CLEAN_INVALID();

    /* Make WFI actually park the core until PDS power-gates it. cpuWfiMask=1
     * starts the power-down on the pdsStart write (below), but the core still has
     * to stay in WFI until the gate completes — if WFI falls through, the core
     * resumes and spins instead of powering off. Two pends can make it fall
     * through from the wake context:
     *   - The PDS/HBN wakeup IRQ that brought us out of the previous sleep is
     *     still latched in the ECLIC (PDS_IntClear only clears the PDS-register
     *     status, not the ECLIC pending bit) — clear those pendings.
     *   - The reset CLINT can leave MTIP pending — push mtimecmp to its max.
     * Then raise the ECLIC level threshold to its max so no remaining enabled
     * interrupt can wake WFI. The PDS wake is a hardware reset (the core is
     * power-gated, cpuPwrOff=1), not an interrupt, so blocking interrupts here
     * does not prevent it. */
    ECLIC_ClearPendingIRQ(PDS_WAKEUP_IRQn);
    ECLIC_ClearPendingIRQ(HBN_OUT0_IRQn);
    ECLIC_ClearPendingIRQ(HBN_OUT1_IRQn);
    ECLIC_SetMth(0xFF);
    SysTimer_SetCompareValue(0xFFFFFFFFFFFFFFFFULL);

    /* Program the PDS hardware (cpuWfiMask=1, same as the first sleep) and issue
     * WFI to park the core while PDS powers it down (the IPRO6 pds15 enter). */
    PDS_Default_Level_Config(&pdsCfgLevel15, sleep_time);

    __WFI();

    /* not reached on real power-down */
    while (1) {
        __asm__ volatile("nop");
    }
#endif /* !IPRO6 */
}

/* -----------------------------------------------------------------------
 * HBN mode public API
 * ----------------------------------------------------------------------- */

/**
 * @brief Enter HBN (Hibernate) sleep mode.
 *
 * HBN is deeper than PDS — all SRAM is lost, only HBN RAM (4 KB) is retained.
 * Wakeup causes a full system reset (cold boot).
 *
 * @param hbn_level  HBN level (0, 1, or 2).
 * @param sleep_time Sleep duration in RTC ticks (32.768 kHz), 0 = indefinite.
 */
void pm_hbn_mode_enter(enum pm_hbn_sleep_level hbn_level, uint32_t sleep_time)
{
#if defined(IPRO6)
    /* IPRO6 HBN is a full power-down, NOT a PDS resume-in-place. On wake the chip
     * cold-boots BROM -> boot2 -> FW; only the HBN/AON registers and the 4 KB HBN
     * RAM survive. So this path deliberately does NOT set the HBN_RSV0 enter-flag:
     * that flag steers the bootrom into the fast-wake path (jump to HBN RAM) and
     * would prevent the clean cold boot. Mirrors the ipro7 / reference HBN flow.
     *
     * Two IPRO6-specific deviations from the generic HBN_Enable() driver path:
     *   - no HBN_RSV0 enter-flag (cold boot, per above);
     *   - the terminal wait before the SW POR spins on a bare cycle loop, NOT
     *     arch_delay_ms(): arch_delay_ms() busy-waits on the SysTimer, whose clock
     *     is gated by the HBN config, so it would hang and the POR never fires. */
    uint32_t tmpVal;
    uint32_t valLow = 0, valHigh = 0;
    uint64_t val;

    cpu_global_irq_disable();

    /* RTC is the only wake source */
    PDS_Wakeup_Src_En_DisableAll();
    HBN_Pin_WakeUp_Mask((uint8_t)0xFF);

    /* clear any latched HBN IRQ that would otherwise abort hibernate */
    writel(0xFFFFFFFFu, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    writel(0x0u, HBN_BASE + HBN_IRQ_CLR_OFFSET);

    /* Guarantee a cold boot on wake: clear any stale PDS/LP enter-flag left in
     * HBN_RSV0 (it lives in the AON domain, which level 0 retains) so the bootrom
     * never takes the fast-wake path. */
    writel(0x0u, HBN_BASE + HBN_RSV0_OFFSET);

    /* Keep RC32K powered so the HBN RTC counter advances through hibernate; without
     * it the compare never matches and the wake never fires. */
    HBN_32K_Sel(HBN_32K_RC);
    HBN_Keep_On_RC32K();

    /* arm RTC compare = now + sleep_time (RTC ticks) */
    HBN_Clear_RTC_Counter();
    if (sleep_time != 0) {
        HBN_Get_RTC_Timer_Val(&valLow, &valHigh);
        val = (uint64_t)valLow + ((uint64_t)valHigh << 32);
        val += sleep_time;
        HBN_Set_RTC_Timer(HBN_RTC_INT_DELAY_0T, (uint32_t)(val & 0xffffffff),
                          (uint32_t)(val >> 32), HBN_RTC_COMP_BIT0_39);
        HBN_Enable_RTC_Counter();
    }

    /* HBN retention LDO @ 1.10 V; the 0.90 V default browns out the always-on
     * domain (RTC counter + wake comparator) so the wake never fires. */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_AON_VOUT_SEL, HBN_LDO_LEVEL_1P10V);
    tmpVal = reg_set_bits(tmpVal, HBN_LDO11_RT_VOUT_SEL, HBN_LDO_LEVEL_1P10V);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* HBN level: L0 keeps the AON core (HBN RAM retained); L1 powers it down
     * (HBN RAM lost). Keep the HBN RTC powered in both so the wake can fire. */
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    if (hbn_level == PM_HBN_LEVEL_0) {
        tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_CORE);
    } else {
        tmpVal = reg_set_bit(tmpVal, HBN_PWRDN_HBN_CORE);
    }
    tmpVal = reg_clr_bit(tmpVal, HBN_PWRDN_HBN_RTC);
    tmpVal = reg_clr_bit(tmpVal, HBN_PWR_ON_OPTION);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    pm_uart_flush();

    /* MCU -> RC32M before arming HBN */
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);

    /* clear HBN IRQ once more, then arm HBN_MODE */
    writel(0xFFFFFFFFu, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    writel(0x0u, HBN_BASE + HBN_IRQ_CLR_OFFSET);
    tmpVal = readl(HBN_BASE + HBN_CTL_OFFSET);
    tmpVal = reg_set_bit(tmpVal, HBN_MODE);
    writel(tmpVal, HBN_BASE + HBN_CTL_OFFSET);

    /* HBN_MODE only arms; the SW power-on reset transitions into HBN (the power
     * controller honours HBN_MODE on the POR, and the RTC cold-boots out of it).
     * Bare cycle-loop settle (NOT arch_delay_ms — see header comment). */
    for (volatile uint32_t i = 0; i < 200000u; i++) {
        __asm volatile("nop");
    }
    GLB_SW_POR_Reset();
    while (1) {
        __asm volatile("nop");
    }
#else
    HBN_APP_CFG_Type cfg = {
        .useXtal32k  = 0,
        .sleepTime   = sleep_time,
        .hw_pu_pd_en = 0,
        .flashCfg    = NULL,
        .hbnLevel    = hbn_level,
        .ldoLevel    = PM_HBN_LDO_LEVEL_DEFAULT,
    };
    uint32_t flash_cfg_len;
    flash_get_cfg((uint8_t **)&cfg.flashCfg, &flash_cfg_len);

    PDS_Wakeup_Src_En_DisableAll();
    HBN_Pin_WakeUp_Mask(0x3FF);

    pm_uart_flush();
    HBN_Mode_Enter(&cfg);   /* never returns; the chip resets on wake */
#endif
}

/* -----------------------------------------------------------------------
 * Wakeup callback management
 * ----------------------------------------------------------------------- */

/**
 * @brief Register a wakeup callback for LP (PDS) mode.
 *
 * Stores the callback address in HBN_RSV1 and sets HBN_STATUS_ENTER_FLAG
 * in HBN_RSV0.  After PDS wakeup with cpuRst=1, bootrom detects the flag
 * and jumps to the callback instead of doing a full cold boot.
 */
void pm_set_wakeup_callback(void (*wakeup_callback)(void))
{
    writel((uint32_t)wakeup_callback, HBN_BASE + HBN_RSV1_OFFSET);
    writel(HBN_STATUS_ENTER_FLAG, HBN_BASE + HBN_RSV0_OFFSET);
}

/**
 * @brief Clear wakeup callback — next reset will be a normal cold boot.
 */
void pm_clear_wakeup_callback(void)
{
    writel(0xFFFFFFFF, HBN_BASE + HBN_RSV1_OFFSET);
    writel(0, HBN_BASE + HBN_RSV0_OFFSET);
}
