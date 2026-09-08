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

#include "hal_sys.h"
#include "hal_hbnram_mgnt.h"
#include "hal_mtimer.h"
#include "drv_aon.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_pds.h"
#include "drv_sec_eng.h"
#include "drv_timer.h"
#include "drv_ef_cfg.h"
#include "ipro_log.h"
#include <generated/autoconf.h>
#include <stdint.h>

volatile int sys_log_all_enable = true;

/*============================ Reset Reason ===================================*/

/* Detected reset reason stored in OCRAM (set at boot, cleared on power-off) */
static hal_rst_reason_t g_rst_reason = HAL_RST_REASON_UNKNOWN;

/* Reset reason strings */
static const char * const rst_reason_str[] = {
    [HAL_RST_REASON_UNKNOWN]    = "Unknown",
    [HAL_RST_REASON_POWER_ON]   = "Power-On",
    [HAL_RST_REASON_WDT]        = "WDT",
    [HAL_RST_REASON_EXCEPTION]  = "Exception",
    [HAL_RST_REASON_SOFTWARE]   = "Software",
    [HAL_RST_REASON_LP_WAKEUP]  = "LP-Wakeup",
    [HAL_RST_REASON_AON_WDT]    = "AON-WDT",
};

const char *hal_rst_reason_str(hal_rst_reason_t reason)
{
    if (reason <= HAL_RST_REASON_AON_WDT) {
        return rst_reason_str[reason];
    }
    return "Invalid";
}

hal_rst_reason_t hal_rst_reason_get(void)
{
    return g_rst_reason;
}

void hal_rst_reason_set(hal_rst_reason_t reason)
{
    /* Store reason in HBN RAM for next boot to read */
    hal_hbnram_rstinfo->reset_reason = (uint32_t)reason;
}

hal_rst_reason_t hal_rst_reason_detect(void)
{
    hal_rst_reason_t reason = HAL_RST_REASON_UNKNOWN;

    /*
     * Detection hierarchy:
     * 1. Check AON WDT status (HW-sticky bit, preserved across WDT-fire reset)
     * 2. Check magic pattern - if not found, this is cold boot (power-on)
     * 3. Check WDT reset status
     * 4. Check PDS/LP wakeup status
     * 5. Read software-set reason from HBN RAM
     *
     * AON_WDT is checked FIRST because relay/external POR may wipe HBN RAM
     * (magic mismatch → would fall into POWER_ON), but the AON WDT status
     * bit reflects the actual silicon reset source and should be honored.
     */

#if defined(CONFIG_AON_WDT_SUPPORT)
    /* Step 1: AON WDT reset status — used by pure_dcdc wakeup stub as
     * hang safety net (HBN_WDT[11] ST_AON_WDT, HW-sticky).
     * Only SoCs that ship the HBN AON-WDT driver (ipro7, ipro6le) have this
     * sticky bit; ipro6 has no such driver, so skip the check entirely. */
    if (HBN_AON_WDT_Get_Reset_Status()) {
        reason = HAL_RST_REASON_AON_WDT;
        HBN_AON_WDT_Clear_Reset_Status();
        /* (Re)initialize magic so subsequent boots take normal path. */
        hal_hbnram_rstinfo->magic = HAL_HBNRAM_RSTINFO_MAGIC;
        hal_hbnram_rstinfo->reset_reason = HAL_RST_REASON_UNKNOWN;
        goto done;
    }
#endif

    /* Step 2: Check for cold boot (power-on) */
    if (hal_hbnram_rstinfo->magic != HAL_HBNRAM_RSTINFO_MAGIC) {
        /* First boot or power was lost - initialize HBN RAM */
        hal_hbnram_rstinfo->magic = HAL_HBNRAM_RSTINFO_MAGIC;
        hal_hbnram_rstinfo->reset_reason = HAL_RST_REASON_UNKNOWN;
        reason = HAL_RST_REASON_POWER_ON;
        goto done;
    }

    /* Step 3: Check WDT reset status */
    if (WDT_GetResetStatus(WDT0_ID)) {
        reason = HAL_RST_REASON_WDT;
        /* Clear WDT reset status */
        WDT_ClearResetStatus(WDT0_ID);
        goto done;
    }

    /* Step 4: Check low power wakeup */
    if (PDS_Get_IntStatus(PDS_INT_WAKEUP)) {
        reason = HAL_RST_REASON_LP_WAKEUP;
        /* Clear PDS interrupt */
        PDS_IntClear();
        goto done;
    }

    /* Step 4: Read software-set reason from HBN RAM */
    reason = (hal_rst_reason_t)hal_hbnram_rstinfo->reset_reason;
    if (reason == HAL_RST_REASON_UNKNOWN) {
        /* No specific reason set, treat as software reset */
        reason = HAL_RST_REASON_SOFTWARE;
    }

done:
    /* Store detected reason in OCRAM for runtime access */
    g_rst_reason = reason;

    /* Clear HBN RAM reset reason for next cycle */
    hal_hbnram_rstinfo->reset_reason = HAL_RST_REASON_UNKNOWN;

    return reason;
}

/*============================ Legacy API =====================================*/

/* Legacy reset info stored in HBN RAM (for backward compatibility) */
#define sys_rstinfo     (*(volatile int *)HAL_HBNRAM_RSTINFO_ADDR)

/*============================ System Functions ===============================*/

void hal_reboot(void)
{
    __disable_irq();
    /* Set reset reason before reboot */
    hal_rst_reason_set(HAL_RST_REASON_SOFTWARE);
    HBN_Set_User_Boot_Config(0);
    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }
}

void hal_reboot_download(void)
{
    __disable_irq();
    HBN_Set_User_Boot_Config(1);
    GLB_SW_POR_Reset();
    while (1) {
        /*empty dead loop*/
    }
}

void hal_sys_reset(void)
{
    __disable_irq();
    GLB_SW_System_Reset();
    while (1) {
        /*empty dead loop*/
    }
}

void hal_poweroff(void)
{
    /*TODO empty now*/
}

void hal_sys_capcode_update(uint8_t capin, uint8_t capout)
{
    static uint8_t capin_static, capout_static;

    if (255 != capin && 255 != capout) {
        AON_Set_Xtal_CapCode(capin, capout);
        capin_static = capin;
        capout_static = capout;
    } else {
        AON_Set_Xtal_CapCode(capin_static, capout_static);
    }
}

uint8_t hal_sys_capcode_get(void)
{
    return AON_Get_Xtal_CapCode();
}

int hal_sys_rstinfo_get(void)
{
    return sys_rstinfo;
}

int hal_sys_logall_enable(void)
{
    sys_log_all_enable = true;
    return 0;
}

int hal_sys_logall_disable(void)
{
    sys_log_all_enable = false;
    return 0;
}

/* CDET (Clock Detection) measurement of RC32K vs XTAL32M.
 *
 * SEC_ENG SE_CDET_0 block — non-secure alias at 0x40080400.
 * Configured for: golden=XTAL32M, test=RC32K, t_loop_n=32, manual mode.
 * Vendor formula: freq_Hz = 32_MHz * (t_count + 2) / g_count
 */
#ifdef CONFIG_IPRO7
#define CDET_BASE       0x40080000U
#define CDET_CTRL0_OFF  0x400U
#define CDET_CTRL1_OFF  0x404U
#define CDET_CTRL2_OFF  0x408U
#define CDET_CTRL3_OFF  0x40CU

static inline uint32_t cdet_rd(uint32_t off)            { return *(volatile uint32_t *)(uintptr_t)(CDET_BASE + off); }
static inline void     cdet_wr(uint32_t off, uint32_t v){ *(volatile uint32_t *)(uintptr_t)(CDET_BASE + off) = v; }

static int s_cdet_inited = 0;

/* CDET init prerequisites at boot:
 *   1. TZC unlock SE_CDET (TZC_NSEC TZC_SE_CTRL_0 bits 8-9 = 0x3)
 *   2. SE_ENG peripheral clock un-gate
 *   3. Group 0 CDET access request
 *   4. RC32M_RC32K_CAL_CGEN = 1 — shared cal-clock gate that drives CDET
 *      counter logic. Set here so CDET works during hal_rc32k_cal().
 *
 * KNOWN LIMITATION: GLB_RC32M_Cal() (called right after hal_rc32k_cal()
 * in hal_boot2_ipro7.c) clears RC32M_RC32K_CAL_CGEN on exit. Re-arming
 * the gate alone does NOT restore post-boot CDET — even the vendor
 * verbatim sequence (rc32k_cdet_run shell cmd) returns garbage g_count
 * after hal_rc32m_cal(). Stage-3 binary search in hal_rc32k_cal() relies
 * on CDET, but cal still produces ~200 ppm err because mtimer post-
 * verification measures actual ppm regardless of which code stage 3
 * landed on. Post-boot rc32k_cdet_* shell commands are non-functional
 * pending deeper hardware investigation. */
static void cdet_setup_once(void)
{
    if (s_cdet_inited) return;

    /* TZC: open SE_CDET to non-secure (bits 8-9 of TZC_NSEC TZC_SE_CTRL_0) */
    volatile uint32_t *tzc_se_ctrl0 = (volatile uint32_t *)(uintptr_t)0x30007F40U;
    *tzc_se_ctrl0 = (*tzc_se_ctrl0 & ~(0x3U << 8)) | (0x3U << 8);
    arch_delay_us(100);

    hal_clock_enable(HAL_MODULE_SEC_ENG);
    arch_delay_ms(2);

    Sec_Eng_Group0_Request_CDET_Access();
    arch_delay_us(100);

    /* Shared cal-clock gate */
    volatile uint32_t *glb_rc32m_ctrl0 = (volatile uint32_t *)(uintptr_t)(0x40000000U + 0x300U);
    *glb_rc32m_ctrl0 |= (1U << 30);
    arch_delay_us(100);

    s_cdet_inited = 1;
}

/* Run CDET measurement of RC32K vs XTAL32M, mirroring proven flow in
 * hal_sec_trng.c::hal_rosc_measure() — but with timeouts on the busy
 * polls (SDK helpers use uncapped while(1) which hangs at boot when CDET
 * doesn't start).
 *
 * Returns g_count (XTAL32M cycles in t_loop_n RC32K cycles), or 0 on fail.
 */
uint32_t hal_rc32k_cdet_measure_g(void)
{
    uint32_t tmpVal;

    cdet_setup_once();

    /* CTRL_0: clear EN, set MODE=1 (manual mode) */
    tmpVal = cdet_rd(CDET_CTRL0_OFF);
    tmpVal &= ~(1U << 0);
    tmpVal |=  (1U << 12);
    cdet_wr(CDET_CTRL0_OFF, tmpVal);

    /* CTRL_1 / CTRL_2 */
    cdet_wr(CDET_CTRL1_OFF, (3U << 16) | 0xFFFFU);
    cdet_wr(CDET_CTRL2_OFF, (0xFFU << 24) | (3U << 16) | 0x20U);

    /* Gold = XTAL32M (1), Test = RC32K (1) */
    Sec_Eng_CDET_Set_Clk_Sel(1, 1);

    /* Start FSM (rising edge on EN). Vendor script just blind-sleeps 100ms
     * after this — don't poll BUSY (FSM may transition too fast to catch). */
    tmpVal = cdet_rd(CDET_CTRL0_OFF);
    tmpVal |= (1U << 0);   /* EN = 1 */
    cdet_wr(CDET_CTRL0_OFF, tmpVal);

    arch_delay_ms(100);    /* same as vendor script */

    return (cdet_rd(CDET_CTRL3_OFF) >> 16) & 0xFFFFU;
}

int hal_rc32k_measure_cdet_ppm(uint32_t *out_t_count, uint32_t *out_g_count)
{
    cdet_setup_once();

    cdet_wr(CDET_CTRL1_OFF, (3U << 16) | 0xFFFFU);
    arch_delay_ms(2);
    cdet_wr(CDET_CTRL2_OFF, (0xFFU << 24) | (3U << 16) | 0x20U);
    arch_delay_ms(2);
    cdet_wr(CDET_CTRL0_OFF, (1U << 28) | (1U << 24) | (1U << 12));
    arch_delay_ms(2);
    cdet_wr(CDET_CTRL0_OFF, cdet_rd(CDET_CTRL0_OFF) | 0x1U);

    int retry = 200;
    while ((cdet_rd(CDET_CTRL0_OFF) & 0x2U) && retry-- > 0) {
        arch_delay_ms(1);
    }
    if (retry <= 0) return INT32_MAX;

    uint32_t r = cdet_rd(CDET_CTRL3_OFF);
    uint32_t t_count = r & 0xFFFFU;
    uint32_t g_count = (r >> 16) & 0xFFFFU;
    if (out_t_count) *out_t_count = t_count;
    if (out_g_count) *out_g_count = g_count;
    if (g_count == 0) return INT32_MAX;

    /* Vendor formula: freq_Hz = 32M * (t_count + 2) / g_count */
    uint64_t freq_hz = (uint64_t)32000000ULL * (t_count + 2ULL) / g_count;
    int64_t err_ppm = ((int64_t)freq_hz - 32768) * 1000000LL / 32768LL;
    return (int)err_ppm;
}

/* Write the 13-bit RC32K fine code (HBN_RC32K_CTRL1 [12:0]) with explicit
 * SDM bypass control. Preserves sdm_rstb=1, aon_rc32k_resv=0xB.
 *
 * The 13-bit code splits into:
 *   [12:3] = 10-bit R (resistor) code  — coarse frequency tuning
 *   [ 2:0] =  3-bit SDM dither code    — fine, only effective when sdm_bypass=0
 *
 * sdm_bypass=1: SDM disabled (HW default), only [12:3] matters.
 * sdm_bypass=0: SDM enabled, full 13-bit resolution.
 */
static void rc32k_set_code_fr_cal(uint16_t code13, int sdm_bypass)
{
    uint32_t v = 0
        | (0xBU                              << 28)   /* aon_rc32k_resv */
        | ((uint32_t)(sdm_bypass ? 1U : 0U)  << 20)   /* rc32k_sdm_bypass */
        | (0x1U                              << 16)   /* rc32k_sdm_rstb */
        | (uint32_t)(code13 & 0x1FFFU);
    writel(v, HBN_BASE + HBN_RC32K_CTRL1_OFFSET);
}

/* Compute golden g_count for target 32768 Hz.
 *
 * CDET runs t_loop_n=32 cycles of test clock, counts XTAL32M cycles → g_count.
 * Vendor formula: freq_Hz = 32M * (t_count + 2) / g_count
 * For freq = 32768:  g_count = 32M * 34 / 32768 = 33203.125
 * t_count is the configured t_loop_n (=32 → +2 → 34).
 */
#define RC32K_GOLDEN_G_COUNT    33203U

/* Per-stage measurement windows. Quantization noise = 1e6 / (32768 * win_s)
 * ppm per ±1 RTC cycle. Tuned so each stage's noise is well below the step
 * size it's resolving:
 *   cap step  ~= 5000+ ppm →  100 ms window (±300 ppm noise) is plenty
 *   R   step  ~=   36 ppm  →  100 ms window (±300 ppm) — best we can do
 *                              within reasonable cal time; final R is within
 *                              ±2 LSB anyway
 *   SDM step  ~=    5 ppm  →  200 ms window (±150 ppm) — sub-step is below
 *                              noise floor; linear sweep still picks the
 *                              local minimum within the cluster
 *   verify    →  1000 ms (±30 ppm) — meaningful end-of-cal report
 */
#define RC32K_CAP_WIN_US       100000U
#define RC32K_R_WIN_US         100000U
#define RC32K_SDM_WIN_US       200000U
#define RC32K_VERIFY_WIN_US   1000000U

/* Probe a single cap_sel and return measured |ppm error|. Sets cap_sel,
 * waits 2 us for analog settle, measures one window. */
static int rc32k_probe_cap_abs_err(int cap)
{
    HBN_Set_RC32K_CapSel(cap);
    arch_delay_us(2);
    int err = hal_rc32k_measure_error_ppm(RC32K_CAP_WIN_US);
    return err < 0 ? -err : err;
}

/* Hybrid cap_sel search: HW FSM hint + bidirectional walk to plateau.
 *
 * cap_sel|→freq is a monotonic, single-minimum function of cap, so once we
 * know which side is decreasing, walking until |err| starts to grow gives
 * the global minimum within that 16-step domain.
 *
 *   1. Start at hint (HW FSM result, or 8 if FSM unavailable).
 *   2. Probe hint, hint-1, hint+1 → identify direction of decreasing |err|.
 *   3. Walk in that direction until |err| stops decreasing → break, use the
 *      last best.
 *
 * Probe count: 3 (best case, hint already optimal) up to 16 (FSM way off).
 * Typical case for the IPRO7 EVB DUT we tested: hint ∈ {6, 8}, truth=6 →
 * 4-5 probes total. Compared to full 16-cap sweep (always 16 probes), this
 * saves ~1.0-1.3 s of cal time. Compared to old ±1 probe (3 probes always),
 * this fixes the bimodal-cap bug when FSM disagrees with the optimum by ≥ 2.
 *
 * Returns best_cap on success. Always called with caller-managed 32K src.
 */
static int rc32k_cap_walk(int hint)
{
    if (hint < 0 || hint > 15) hint = 8;

    int best_cap = hint;
    int best_abs = rc32k_probe_cap_abs_err(hint);

    int abs_minus = (hint > 0)  ? rc32k_probe_cap_abs_err(hint - 1) : INT32_MAX;
    int abs_plus  = (hint < 15) ? rc32k_probe_cap_abs_err(hint + 1) : INT32_MAX;

    int dir = 0;
    if (abs_minus < best_abs) { best_abs = abs_minus; best_cap = hint - 1; dir = -1; }
    if (abs_plus  < best_abs) { best_abs = abs_plus;  best_cap = hint + 1; dir = +1; }

    /* If no improvement on either side, hint was the optimum. */
    if (dir == 0) {
        HBN_Set_RC32K_CapSel(best_cap);
        arch_delay_us(2);
        return best_cap;
    }

    /* Walk in the chosen direction until |err| stops improving. */
    int next = best_cap + dir;
    while (next >= 0 && next <= 15) {
        int aerr = rc32k_probe_cap_abs_err(next);
        if (aerr >= best_abs) break;
        best_abs = aerr;
        best_cap = next;
        next += dir;
    }

    HBN_Set_RC32K_CapSel(best_cap);
    arch_delay_us(2);
    return best_cap;
}

/* Internal: full cal flow.
 *
 *   start_cap >= 0: skip Stage 1 walk, use start_cap as fixed cap.
 *   start_cap <  0: run Stage 1 (HW FSM hint + bidirectional walk).
 *
 * Returns best_cap (>= 0) on success.
 *
 * Algorithm (post-IRQ-fix, post-RTC-latch-fix, post-bimodal-fix,
 * post-SDM-drop refactor 2026-05-05):
 *   Stage 1:  HW FSM hint + walk to |err| plateau (3-16 probes @ 100 ms).
 *             Replaces the old ±1 cap probe that couldn't bridge bimodal
 *             FSM output (cap ∈ {6, 8}). Walk converges deterministically
 *             on the global minimum because cap|→|err| is unimodal.
 *   Stage 2:  R [12:3] 10-bit binary search at 100 ms (sdm_bypass=1).
 *   Stage 3:  DROPPED — 100-trial byp0-vs-byp1 study showed enabling SDM
 *             hurts stability by 30% (1σ 287 → 447 ppm). SDM=0 is locked,
 *             sdm_bypass=1 always. Saves 1.6 s per cal.
 *   Verify:   1000 ms window for end-of-cal final_err report (±30 ppm
 *             quantization).
 */
static int rc32k_run_cal(int start_cap)
{
    uint64_t t0 = mtimer_get_time_us();
    int f32k_val = HBN_Get_32K_Sel();
    int xtal_powered_on = 0;

    /* RC32K must drive RTC for measurements. */
    if (f32k_val != HBN_32K_RC) {
        HBN_32K_Sel(HBN_32K_RC);
    }
    rc32k_set_code_fr_cal(0x1800, /*sdm_bypass=*/1);
    arch_delay_us(2000);

    /* Stage 1: HW FSM hint then bidirectional walk to global minimum. */
    int hint_cap = -1;
    int best_cap;
    if (start_cap >= 0) {
        best_cap = start_cap;
        HBN_Set_RC32K_CapSel(best_cap);
        arch_delay_us(2);
    } else {
        /* HW FSM hint requires XTAL32K reference, so power it on briefly. */
        if (HBN_Get_32K_Sel() != HBN_32K_XTAL) {
            HBN_Power_On_Xtal_32K();
            HBN_32K_Sel(HBN_32K_XTAL);
            xtal_powered_on = 1;
        }
        hint_cap = GLB_RC32K_Cal();
        if (xtal_powered_on) {
            HBN_32K_Sel(HBN_32K_RC);
            HBN_Power_Off_Xtal_32K();
            xtal_powered_on = 0;
            arch_delay_us(2);
        }
        /* If FSM failed (no XTAL32K), fall through with hint=-1 → walk
         * starts at midpoint cap=8. */
        best_cap = rc32k_cap_walk(hint_cap);
    }

    uint64_t t1 = mtimer_get_time_us();

    /* Stage 3a: binary-search 10-bit R code [12:3] with SDM bypassed.
     * Direction probed at R=0x300 vs R=0x100. With 100 ms window the
     * direction probe has ±300 ppm noise — well below the ~10000 ppm
     * full-range R swing, so dir is reliable. */
    int best_r10 = 0x300;
    int iters_r = 0;
    int up_dir;

    rc32k_set_code_fr_cal((uint16_t)(0x300U << 3), /*sdm_bypass=*/1);
    arch_delay_us(1000);
    int e_def = hal_rc32k_measure_error_ppm(RC32K_R_WIN_US);
    rc32k_set_code_fr_cal((uint16_t)(0x100U << 3), /*sdm_bypass=*/1);
    arch_delay_us(1000);
    int e_low = hal_rc32k_measure_error_ppm(RC32K_R_WIN_US);
    up_dir = (e_low < e_def) ? +1 : -1;
    {
        int lo = 0, hi = 0x3FF;
        int best_abs_mt = INT32_MAX;
        for (iters_r = 0; iters_r < 10 && lo <= hi; iters_r++) {
            int mid = (lo + hi) / 2;
            rc32k_set_code_fr_cal((uint16_t)(mid << 3), /*sdm_bypass=*/1);
            arch_delay_us(500);
            int err = hal_rc32k_measure_error_ppm(RC32K_R_WIN_US);
            int aerr = err < 0 ? -err : err;
            if (aerr < best_abs_mt) { best_abs_mt = aerr; best_r10 = mid; }
            if (err == 0) break;
            if (err > 0) { if (up_dir > 0) hi = mid - 1; else lo = mid + 1; }
            else         { if (up_dir > 0) lo = mid + 1; else hi = mid - 1; }
        }
    }

    uint64_t t2 = mtimer_get_time_us();

    /* Stage 3b SDM linear sweep was DROPPED (2026-05-05) after a 100-trial
     * byp0-vs-byp1 experiment showed sdm_bypass=1 produces 30% better
     * post-cal stability (1σ 287 ppm vs 447 ppm with SDM enabled). The SDM
     * modulator with input=0 is also not a true no-op on this DUT family
     * (+861 ppm bias measured at SDM=0), so even keeping a "neutral" SDM
     * is hostile to stability. Lock SDM=0 + sdm_bypass=1 always. The
     * efuse-side machinery (split rc32k_code_h/l, boot apply auto-detect)
     * stays in place — code [2:0] = 0 → boot uses sdm_bypass=1. */
    int best_sdm = 0;
    int best_sdm_bypass = 1;
    int best_code = (best_r10 << 3) | best_sdm;
    rc32k_set_code_fr_cal((uint16_t)best_code, best_sdm_bypass);
    arch_delay_us(1000);

    uint64_t t3 = mtimer_get_time_us();

    /* Final verification with longest window for the most precise err
     * report. 1000 ms window → ~30 ppm RTC quantization noise. */
    int final_err = hal_rc32k_measure_error_ppm(RC32K_VERIFY_WIN_US);

    uint64_t t4 = mtimer_get_time_us();

    /* Restore caller's 32K source. HBN_32K_Sel() internally re-applies
     * efuse cap_sel (drv_hbn.c HBN_Trim_RC32K), so we re-apply best_cap
     * and best_code after to preserve cal output. */
    HBN_32K_Sel(f32k_val);
    HBN_Set_RC32K_CapSel(best_cap);
    rc32k_set_code_fr_cal((uint16_t)best_code, best_sdm_bypass);
    arch_delay_us(2);

    IPRO_LOGI("SYS",
              "RC32K_CAL src=%s hint=%d cap=%d code=0x%04x r=0x%03x sdm=%d byp=%d "
              "dir=%d iters_r=%d err=%d walk_us=%llu r_us=%llu meas_us=%llu",
              (start_cap < 0) ? "walk" : "fixed",
              (start_cap < 0) ? hint_cap : start_cap,
              best_cap, (unsigned)best_code,
              (unsigned)best_r10, best_sdm, best_sdm_bypass,
              up_dir, iters_r, final_err,
              (unsigned long long)(t1 - t0),
              (unsigned long long)(t2 - t1),
              (unsigned long long)(t4 - t3));

    return best_cap;
}

/* hal_rc32k_cal — boot-time RC32K setup, pure efuse application:
 *
 *   1. cap_sel trim invalid  → skip RC32K setup, return -1.
 *   2. cap_sel valid + code trim valid   → apply both, return.
 *      sdm_bypass auto-selected: 0 if code [2:0] != 0 (SDM was tuned),
 *      else 1 (HW default).
 *   3. cap_sel valid + code trim invalid → apply cap_sel only, return
 *      (code_fr_cal stays at HW default, sdm_bypass=1).
 *
 * No post-tune at boot. Use hal_rc32k_force_cal() (rc32k_force_cal shell
 * cmd or SLT slt_test_rc32k) for runtime calibration.
 */
int hal_rc32k_cal(void)
{
    Efuse_Common_Trim_Type trim;
    EF_Ctrl_Read_Common_Trim("rc32k", &trim, 1);
    int has_trim = trim.en &&
                   trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len);

    if (!has_trim) {
        printf("rc32k: no efuse cap_sel trim, skipping setup\r\n");
        return -1;
    }

    HBN_Set_RC32K_CapSel(trim.value);

    /* Read 13-bit fine code split across two efuse words. en/parity bits
     * are SHARED — both half-trims report the same en/parity values; the
     * parity is over the FULL composed 13-bit code, validated below. */
    Efuse_Common_Trim_Type code_h, code_l;
    EF_Ctrl_Read_Common_Trim("rc32k_code_h", &code_h, 1);
    EF_Ctrl_Read_Common_Trim("rc32k_code_l", &code_l, 0);
    if (code_h.en && code_l.en) {
        uint16_t fine_code = (uint16_t)((code_h.value << 7) | code_l.value);
        if (code_h.parity == EF_Ctrl_Get_Trim_Parity(fine_code, 13)) {
            /* SDM bits [2:0] only take effect with sdm_bypass=0. If the
             * burned trim has any SDM bits set, the SLT explicitly tuned
             * them, so enable SDM. Otherwise keep HW default (bypass=1). */
            int sdm_bypass = ((fine_code & 0x7U) == 0) ? 1 : 0;
            rc32k_set_code_fr_cal(fine_code, sdm_bypass);
            printf("rc32k from efuse, cap_sel=0x%x code=0x%04x byp=%d\r\n",
                   (unsigned int)trim.value, (unsigned int)fine_code, sdm_bypass);
            return (int)trim.value;
        }
    }

    printf("rc32k from efuse, cap_sel=0x%x (no code trim)\r\n",
           (unsigned int)trim.value);
    return (int)trim.value;
}

/* hal_rc32k_force_cal — manually trigger full calibration, regardless of
 * efuse trim state. Stages (mtimer/XTAL32M is the precision reference for
 * R/verify; XTAL32K only briefly powered for HW FSM cap hint):
 *   1.  HW FSM cap hint + bidirectional walk @ 100 ms (~0.85-1.0 s)
 *   2.  R [12:3] 10-bit binary search at 100 ms (sdm_bypass=1, ~1.2 s)
 *   3.  Final verify at 1000 ms (~30 ppm precision report)
 * Applies cap_sel + (R << 3 | 0) with sdm_bypass=1. SDM stage was dropped
 * 2026-05-05 — empirically hurts stability on this DUT family. Returns
 * best_cap. */
int hal_rc32k_force_cal(void)
{
    return rc32k_run_cal(-1);
}
#else
int hal_rc32k_cal(void) { return -1; }
int hal_rc32k_force_cal(void) { return -1; }
#endif

/* Atomically clear MIE in mstatus and return prior MIE bit. RISC-V csrrci
 * encodes a 5-bit immediate; MSTATUS_MIE = 0x8 fits. Use unsigned long so
 * the operand matches XLEN (64 bits on RV64). Pure inline-asm so no
 * header dependency on Nuclei/CMSIS CSR helpers. */
static inline unsigned long rc32k_irq_save_disable(void)
{
    unsigned long prev;
    __asm__ volatile ("csrrci %0, mstatus, 8"
                      : "=r"(prev) :: "memory");
    return prev & 0x8UL;
}

static inline void rc32k_irq_restore(unsigned long saved_mie)
{
    if (saved_mie) {
        __asm__ volatile ("csrsi mstatus, 8" ::: "memory");
    }
}

/* RTC latch propagation:
 *
 * Writing the LATCH bit (HBN_RTC_TIME_H[31]) requests an atomic snapshot
 * of the RTC counter into the readable H/L registers. The snapshot only
 * advances on RC32K clock edges, so up to ~30 us elapses before the new
 * value is visible. The vendor `HBN_Get_RTC_Timer_Async_Val` reads back
 * with zero delay — at typical CPU bus speeds, both the trigger and the
 * read complete within a single 32K period, so the readback returns the
 * PREVIOUS latch value (stale by however long ago the previous trigger
 * happened). The do-while in HBN_Get_RTC_Timer_Val only catches staleness
 * if the second pair manages to land in a fresh window; if both pairs
 * race within the same RC32K period, val1 == val and the loop exits with
 * stale data. Observed effect: same (R, SDM) code measures with ±640 ppm
 * spread on a 1000 ms verify window — vs ±30 ppm theoretical quantization.
 *
 * Local rtc_get_safe() forces a fresh latch by triggering then waiting >
 * 1 RC32K period (40 us > 30.5 us nominal) before reading.
 */
#define RC32K_RTC_LATCH_BIT     (1U << 31)
#define RC32K_RTC_LATCH_DELAY_US 40U

static inline void rtc_get_safe(uint32_t *low, uint32_t *high)
{
    uint32_t v = readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    writel(v | RC32K_RTC_LATCH_BIT, HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    arch_delay_us(RC32K_RTC_LATCH_DELAY_US);
    v = readl(HBN_BASE + HBN_RTC_TIME_H_OFFSET);
    *high = v & HBN_RTC_TIME_LATCH_H_MSK;
    *low  = readl(HBN_BASE + HBN_RTC_TIME_L_OFFSET);
}

/* Measure RC32K precision against MTimer (XTAL32M-derived).
 *
 * Caller must ensure HBN_32K_Sel(HBN_32K_RC) is active so that the RTC
 * counter is driven by RC32K. This function does not change the 32K source.
 *
 * The two snapshot pairs (rtc0, mt0) and (rtc1, mt1) are sampled inside
 * critical sections so an IRQ cannot split the rtc/mtimer reads — that would
 * skew expected vs actual count by the IRQ duration (~tens of us per
 * FreeRTOS tick → hundreds of ppm error on a 50 ms window). IRQs are
 * re-enabled during the window wait itself (both timers are HW counters
 * that keep ticking, and arch_delay_us re-checks mtimer regardless).
 *
 * Returns: signed PPM error of the active 32K source vs nominal 32768 Hz
 *          (positive = source is fast, negative = source is slow). On
 *          measurement failure (zero window), returns INT32_MAX.
 */
#ifdef CONFIG_IPRO7
int hal_rc32k_measure_error_ppm(uint32_t window_us)
{
    uint32_t rtc_l0, rtc_h0, rtc_l1, rtc_h1;
    uint64_t rtc0, rtc1, mt0, mt1;
    int64_t actual, expected, err_ppm;
    uint32_t mie;

    if (window_us < 10000U) {
        window_us = 10000U;
    }

    mie = rc32k_irq_save_disable();
    rtc_get_safe(&rtc_l0, &rtc_h0);
    mt0 = mtimer_get_time_us();
    rc32k_irq_restore(mie);
    rtc0 = ((uint64_t)rtc_h0 << 32) | (uint64_t)rtc_l0;

    arch_delay_us(window_us);

    mie = rc32k_irq_save_disable();
    rtc_get_safe(&rtc_l1, &rtc_h1);
    mt1 = mtimer_get_time_us();
    rc32k_irq_restore(mie);
    rtc1 = ((uint64_t)rtc_h1 << 32) | (uint64_t)rtc_l1;

    if (mt1 <= mt0) {
        return INT32_MAX;
    }

    actual   = (int64_t)(rtc1 - rtc0);
    expected = (int64_t)(((mt1 - mt0) * 32768ULL) / 1000000ULL);

    if (expected == 0) {
        return INT32_MAX;
    }

    err_ppm = ((actual - expected) * 1000000) / expected;
    return (int)err_ppm;
}

/* RC32K SW calibration when XTAL32K is unavailable.
 *
 * Uses CPU MTimer (XTAL32M) as the precision reference and sweeps the 4-bit
 * RC32K cap_sel (0..15) to find the value that minimizes |freq_error|.
 *
 * Reference flow: bl616 hal_pm.c pm_rc32k_auto_cal — same idea (compare RTC
 * delta vs mtimer delta), but adapted to one-shot boot-time cap sweep
 * because IPRO7 cap_sel is coarse (only 16 codes, not a fine recal nudge).
 *
 * Total time budget: 16 * (window_us + settle_us). With defaults ≈ 2.4 sec.
 *
 * Returns: best cap_sel (0..15) on success, -1 on failure.
 */
#ifndef RC32K_SW_CAL_WINDOW_US
#define RC32K_SW_CAL_WINDOW_US     150000U     /* 150 ms per measurement */
#endif
#ifndef RC32K_SW_CAL_SETTLE_US
#define RC32K_SW_CAL_SETTLE_US     2U          /* 2 us after cap change (analog settle) */
#endif

int hal_rc32k_sw_cal(void)
{
    HBN_32K_CLK_Type orig_src;
    int best_cap = 8;
    int best_abs_err = INT32_MAX;

    /* Force RC32K as RTC source so we can measure RC32K against mtimer. */
    orig_src = HBN_Get_32K_Sel();
    if (orig_src != HBN_32K_RC) {
        HBN_32K_Sel(HBN_32K_RC);
    }

    printf("rc32k sw_cal: sweep cap_sel 0..15 (window=%uus)\r\n",
           (unsigned)RC32K_SW_CAL_WINDOW_US);

    for (int cap = 0; cap < 16; cap++) {
        HBN_Set_RC32K_CapSel(cap);
        arch_delay_us(RC32K_SW_CAL_SETTLE_US);

        int err = hal_rc32k_measure_error_ppm(RC32K_SW_CAL_WINDOW_US);
        int aerr = (err < 0) ? -err : err;
        printf("rc32k sw_cal: cap=%2d err=%+d ppm\r\n", cap, err);

        if (aerr < best_abs_err) {
            best_abs_err = aerr;
            best_cap = cap;
        }
    }

    HBN_Set_RC32K_CapSel(best_cap);
    arch_delay_us(RC32K_SW_CAL_SETTLE_US);
    printf("rc32k sw_cal: best cap_sel=%d (err=%+d ppm)\r\n",
           best_cap, best_abs_err);

    /* Restore original 32K source if caller had selected XTAL/DIG. */
    if (orig_src != HBN_32K_RC) {
        HBN_32K_Sel(orig_src);
    }
    return best_cap;
}
#else
int hal_rc32k_measure_error_ppm(uint32_t window_us) { (void)window_us; return INT32_MAX; }
int hal_rc32k_sw_cal(void) { return -1; }
#endif

int hal_rc32m_cal(void)
{
#ifdef CONFIG_IPRO7
    Efuse_Common_Trim_Type trim;
    EF_Ctrl_Read_Common_Trim("rc32m", &trim, 1);

    if (trim.en &&
        trim.parity == EF_Ctrl_Get_Trim_Parity(trim.value, trim.len)) {
        GLB_Trim_RC32M();
        printf("rc32m from efuse, code = 0x%x\r\n", (unsigned int)trim.value);
        return (int)trim.value;
    }

    int code = GLB_RC32M_Cal();
    printf("rc32m cal done, code = 0x%x\r\n", code);
    return code;
#else
    return -1;
#endif
}

/*============================ Clock Gate & Reset =============================*/

/**
 * @brief Map HAL module to chip-specific clock type
 */
static int get_clock_type(hal_module_t module)
{
    switch (module) {
#ifdef CONFIG_IPRO7
        case HAL_MODULE_SEC_ENG: return GLB_AHB_CLOCK_SEC_ENG;
        case HAL_MODULE_USB:     return GLB_AHB_CLOCK_USB;
        case HAL_MODULE_EMAC:    return GLB_AHB_CLOCK_EMAC;
        case HAL_MODULE_PKA:     return GLB_AHB_CLOCK_PKA;
#else
        case HAL_MODULE_SEC_ENG: return GLB_AHB_CLOCK_IP_SEC;
        case HAL_MODULE_USB:     return GLB_AHB_CLOCK_IP_USB11;
        case HAL_MODULE_EMAC:    return GLB_AHB_CLOCK_IP_EMAC;
        case HAL_MODULE_PKA:     return -1;  /* Not available on IPRO6LE */
#endif
        default: return -1;
    }
}

/**
 * @brief Map HAL module to chip-specific reset type
 */
static int get_reset_type(hal_module_t module)
{
    switch (module) {
#if defined(CONFIG_IPRO7)
        case HAL_MODULE_SEC_ENG: return GLB_AHB_MCU_SW_SEC_ENG;
        case HAL_MODULE_USB:     return GLB_AHB_MCU_SW_USB;
        case HAL_MODULE_EMAC:    return GLB_AHB_MCU_SW_EMAC;
        case HAL_MODULE_PKA:     return GLB_AHB_MCU_SW_SEC_ENG;  /* PKA uses SEC_ENG reset on IPRO7 */
#elif defined(CONFIG_IPRO6)
        case HAL_MODULE_SEC_ENG: return GLB_AHB_MCU_SW_SEC_ENG;
        case HAL_MODULE_USB:     return GLB_AHB_MCU_SW_USB;
        case HAL_MODULE_EMAC:    return GLB_AHB_MCU_SW_EMAC;
        case HAL_MODULE_PKA:     return -1;  /* Not available on IPRO6 */
#elif defined(CONFIG_IPRO6LE)
        case HAL_MODULE_SEC_ENG: return GLB_AHB_MCU_SW_SEC_ENG;
        case HAL_MODULE_USB:     return GLB_AHB_MCU_SW_EXT_USB;
        case HAL_MODULE_EMAC:    return GLB_AHB_MCU_SW_EXT_EMAC;
        case HAL_MODULE_PKA:     return -1;  /* Not available on IPRO6LE */
#endif
        default: return -1;
    }
}

int hal_clock_enable(hal_module_t module)
{
    int clk_type = get_clock_type(module);
    if (clk_type < 0) {
        return -1;
    }
    return GLB_PER_Clock_UnGate(clk_type);
}

int hal_clock_disable(hal_module_t module)
{
    int clk_type = get_clock_type(module);
    if (clk_type < 0) {
        return -1;
    }
    return GLB_PER_Clock_Gate(clk_type);
}

int hal_module_reset(hal_module_t module)
{
    int rst_type = get_reset_type(module);
    if (rst_type < 0) {
        return -1;
    }
    return GLB_AHB_MCU_Software_Reset(rst_type);
}
