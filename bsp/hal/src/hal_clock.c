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

/*
 * IPRO6 32 kHz clock: source selection (XTAL32K vs RC32K) and RC32K coarse trim.
 *
 * The 32 kHz is a system clock -- it drives the RTC and the PDS/HBN sequencer --
 * so choosing and trimming it is board bring-up, not power management:
 * board_init() must resolve the source on EVERY IPRO6 image, and a part whose
 * RC32K trim is not burned in efuse needs the CPU to trim it at cold boot
 * whether or not the image ever sleeps. hal_pm.c is compiled only for
 * CONFIG_HAL_LP builds, which is why this does not live there.
 */
#include <generated/autoconf.h>
#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#endif
#include <stdio.h>
#include "drv_common.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "hal_clock.h"
#include "hal_mtimer.h"        /* mtimer_get_time_us: XCLK us for the probe */
#include "hal_lp.h"            /* HAL_LP_RTC_CNT_TO_US */
#include "lp_fw.h"             /* LP_FW_RC32K_*: the RC32K trim register map */
#include "hbn_reg.h"

static void clock_delay_ms_yield(uint32_t ms)
{
#ifdef CONFIG_FREERTOS
    vTaskDelay(pdMS_TO_TICKS(ms));
#else
    arch_delay_ms(ms);
#endif
}

static uint32_t clock_irq_lock(void)
{
#ifdef CONFIG_FREERTOS
    taskENTER_CRITICAL();
    return 0;
#else
    return __irq_save();
#endif
}

static void clock_irq_unlock(uint32_t flags)
{
#ifdef CONFIG_FREERTOS
    (void)flags;
    taskEXIT_CRITICAL();
#else
    __irq_restore(flags);
#endif
}

/*
 * Probe the 32.768 kHz crystal and, when it is real, make it the 32 kHz
 * source. The existence check IS the safety: there is no ready bit for the
 * crystal, so the only proof it oscillates is the RTC advancing at crystal
 * rate against MTIMER once it is selected. A board without the crystal reads
 * ~0 ticks (or, if the select never landed, RC32K's +4.2%) -- both fall
 * outside the +/-2% window and force the RC32K fallback, so enabling the
 * config on a crystal-less board degrades to today's behavior.
 *
 * Adoption must also clear HBN_IRQ_MODE.reg_en_hw_pu_pd (reset value 1): with
 * the AON hardware pull enabled, the PDS pad takeover loads the crystal dead,
 * the AON sequencer loses its 32 kHz and the timed wake never fires (measured
 * 2026-08-14: XTAL sleep woke 0/2 with the bit set, 2/2 clear; RC unaffected).
 * The vendor HBN entry clears the same bit -- the PDS path never did.
 * The sleep paths in hal_pm.c then respect the selection made here.
 */
int clock_xtal32k_adopt(uint32_t settle_ms)
{
    return clock_xtal32k_adopt_ex(settle_ms, 0);
}

/*
 * Same probe, with the settle either yielding to the scheduler (busy = 0, the
 * original behaviour) or busy-waiting (busy = 1). board_init() runs before the
 * scheduler exists on a cold boot, where vTaskDelay() is illegal.
 */
int clock_xtal32k_adopt_ex(uint32_t settle_ms, int busy)
{
    uint32_t lo, hi, r0, tmpVal;

    /* XTAL32K in/out pads (GPIO16/17) go analog before the cell powers up;
     * the reference LP flow then leaves them alone while the crystal owns
     * the domain. */
    GLB_GPIO_Cfg_Type xcfg = {
        .gpioPin  = 16,
        .gpioFun  = GPIO_FUN_ANALOG,
        .gpioMode = GPIO_MODE_ANALOG,
        .pullType = GPIO_PULL_NONE,
        .drive    = 1,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Init(&xcfg);
    xcfg.gpioPin = 17;
    GLB_GPIO_Init(&xcfg);

    HBN_Power_On_Xtal_32K();
    HBN_32K_Sel(HBN_32K_XTAL);

    /* Adopt after two consecutive in-band 100 ms windows; give up
     * when settle_ms passes without them. */
    uint64_t t_start = mtimer_get_time_us();
    uint32_t good = 0u;
    while (good < 2u) {
        HBN_Get_RTC_Timer_Val(&lo, &hi);
        r0 = lo;
        uint64_t m0 = mtimer_get_time_us();
        if (busy) {
            arch_delay_ms(100u);
        } else {
            clock_delay_ms_yield(100u);
        }
        HBN_Get_RTC_Timer_Val(&lo, &hi);
        uint64_t us = mtimer_get_time_us() - m0;
        uint32_t ticks = lo - r0;
        uint64_t expect = us * 32768ull / 1000000ull;
        uint64_t slack = expect / 50u;                          /* +/-2% */
        if (us != 0u && ticks + slack >= expect && ticks <= expect + slack) {
            good++;
        } else if (mtimer_get_time_us() - t_start >= (uint64_t)settle_ms * 1000u) {
            /* Absent or not oscillating: force RC32K even though the config
             * asked for the crystal, and power the dead cell back down. */
            HBN_32K_Sel(HBN_32K_RC);
            HBN_Power_Off_Xtal_32K();
            return 0;
        } else {
            good = 0u;
        }
    }

    HBN_Hw_Pu_Pd_Cfg(0);   /* vendor guide: "always disable ... in PDS/HBN" */

    /* Reference behavior: the RC is powered off once the crystal owns the
     * domain -- both-on is the one configuration the reference never uses
     * (and it wedged PDS here on 2026-08-08). */
    tmpVal = readl(HBN_BASE + HBN_GLB_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, HBN_PU_RC32K);
    writel(tmpVal, HBN_BASE + HBN_GLB_OFFSET);
    return 1;
}

/*
 * Is the selected 32 kHz actually ticking? Two RTC reads around a short
 * MTIMER-timed busy wait; a dead crystal shows zero ticks. 3 ms is ~98 RTC
 * ticks, enough to tell "stopped" from "running" without the +/-2% rate test
 * the adopt probe does.
 */
int clock_32k_is_alive(void)
{
    uint32_t lo, hi, r0;
    uint64_t m0;

    HBN_Get_RTC_Timer_Val(&lo, &hi);
    r0 = lo;
    m0 = mtimer_get_time_us();
    arch_delay_ms(3u);
    HBN_Get_RTC_Timer_Val(&lo, &hi);
    if (mtimer_get_time_us() - m0 == 0u) {
        return 1;                       /* no time base to judge with */
    }
    return (lo - r0) > 16u;             /* ~98 expected; anything alive clears this */
}

/*
 * Resolve the system 32 kHz source at boot.
 *
 * The 32 kHz is a system-wide clock (PDS/HBN sequencer, RTC), not a WiFi
 * setting, so the choice belongs here rather than at the first low-power arm:
 * a board without the crystal must never be left running on a dead selection.
 * Called from board_init(), which also re-runs from the low-power wake hook:
 *
 *   - crystal already selected AND powered AND ticking -> keep it (this is the
 *     wake path; re-probing would cost the settle on every wake and would swap
 *     the clock under the DTIM tracker),
 *   - crystal selected but NOT ticking -> fall back to RC32K and power the dead
 *     cell down (the crystal-less board that used to hang),
 *   - want_xtal = 0 -> RC32K, crystal never powered,
 *   - otherwise -> run the adopt probe once, which falls back to RC32K by
 *     itself when the crystal is absent.
 *
 * Returns 1 if the crystal ends up driving the 32 kHz, 0 for RC32K.
 */
int clock_32k_resolve(int want_xtal, uint32_t settle_ms)
{
    uint32_t sel = (readl(HBN_BASE + HBN_GLB_OFFSET) & HBN_F32K_SEL_MSK) >> HBN_F32K_SEL_POS;
    uint32_t pu  = readl(HBN_BASE + HBN_XTAL32K_OFFSET) & HBN_PU_XTAL32K_MSK;

    if (sel == HBN_32K_XTAL && pu != 0u && want_xtal && clock_32k_is_alive()) {
        return 1;                       /* adopted crystal, still oscillating */
    }
    if (sel == HBN_32K_XTAL) {          /* selected but dead, or no longer wanted */
        HBN_Keep_On_RC32K();
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
    }
    if (!want_xtal) {
        HBN_32K_Sel(HBN_32K_RC);
        HBN_Power_Off_Xtal_32K();
        return 0;
    }
    return clock_xtal32k_adopt_ex(settle_ms, 1);
}

/*
 * RC32K coarse trim against MTIMER. The vendor coarse_adj port (lp rctrim)
 * measured over 50 ms, where one RTC tick is already 610 ppm -- coarser than
 * the target -- and stepped on a 2000-ppm ladder. 200 ms gives 153 ppm of
 * resolution. The step uses a MEASURED slope: the first round steps on the
 * vendor nominal (64 Hz = ~1950 ppm per LSB), every later round on the
 * secant of the last two points; the best code seen is restored at the end,
 * so a bad step can never leave the part worse than it started (a 78
 * ppm/LSB assumption once drove it 300 -> 949 = -527000 ppm, 2026-09-13).
 * The blob's beacon-TSF trim (lp_fw_rc32k_trim_step) takes it from here,
 * with the slope this measured.
 */
static int32_t clock_rc32k_measure_ppm(uint32_t window_ms)
{
    uint32_t rl, rh;
    uint64_t m0, m1, r0, r1;
    uint32_t flags = clock_irq_lock();
    m0 = mtimer_get_time_us();
    HBN_Get_RTC_Timer_Val(&rl, &rh);
    clock_irq_unlock(flags);
    r0 = ((uint64_t)rh << 32) | rl;
    clock_delay_ms_yield(window_ms);
    flags = clock_irq_lock();
    m1 = mtimer_get_time_us();
    HBN_Get_RTC_Timer_Val(&rl, &rh);
    clock_irq_unlock(flags);
    r1 = ((uint64_t)rh << 32) | rl;
    uint32_t mt_us  = (uint32_t)(m1 - m0);
    uint32_t rtc_us = (uint32_t)HAL_LP_RTC_CNT_TO_US(r1 - r0);
    if (mt_us < window_ms * 500u) {
        return INT32_MIN;                       /* MTIMER not running: no reference */
    }
    return (int32_t)(((int64_t)rtc_us - (int64_t)mt_us) * 1000000 / (int64_t)mt_us);
}

static void clock_rc32k_code_wr(int32_t code)
{
    volatile uint32_t *ctrl = (volatile uint32_t *)LP_FW_RC32K_CTRL0_ADDR;
    uint32_t v = *ctrl;
    *ctrl = (v & ~LP_FW_RC32K_CODE_MSK) | ((uint32_t)code << LP_FW_RC32K_CODE_POS) |
            LP_FW_RC32K_EXT_CODE_EN;
    clock_delay_ms_yield(2u);
}

int clock_rc32k_coarse_cal(uint32_t window_ms, int32_t *residual_ppm, int32_t *ppm_per_lsb)
{
    int32_t slope = LP_FW_RC32K_PPM_PER_LSB_NOM;
    int32_t best_ppm = INT32_MAX, best_code = -1;
    int32_t prev_ppm = 0, prev_code = -1;
    int32_t code = (int32_t)lp_fw_rc32k_code_rd(*(volatile uint32_t *)LP_FW_RC32K_CTRL0_ADDR);
    int it;

    if (window_ms < 50u) window_ms = 50u;
    for (it = 1; it <= 6; it++) {
        int32_t ppm = clock_rc32k_measure_ppm(window_ms);
        if (ppm == INT32_MIN) {
            if (residual_ppm) *residual_ppm = 0;
            if (ppm_per_lsb) *ppm_per_lsb = 0;
            return -1;
        }
        int32_t a = (ppm < 0) ? -ppm : ppm;
        int32_t ba = (best_ppm < 0) ? -best_ppm : best_ppm;
        if (a < ba) {
            best_ppm = ppm; best_code = code;
        }
        if (prev_code >= 0 && code != prev_code) {
            /* ppm falls as the code rises: secant slope, kept only when sane */
            int32_t s = (prev_ppm - ppm) / (code - prev_code);
            if (s >= LP_FW_RC32K_PPM_PER_LSB_MIN && s <= LP_FW_RC32K_PPM_PER_LSB_MAX) {
                slope = s;
            }
        }
        printf("[PM] rc32k cal %d: %ld ppm, code %ld, slope %ld ppm/LSB\r\n",
               it, (long)ppm, (long)code, (long)slope);
        if (a <= LP_FW_RC32K_TRIM_THR_PPM) {
            break;
        }
        prev_ppm = ppm; prev_code = code;
        /* step from the best point when this one is worse */
        int32_t base_ppm = ppm, base_code = code;
        if (a > ba) {
            base_ppm = best_ppm; base_code = best_code;
        }
        int32_t half = slope / 2;
        int32_t step = (base_ppm + (base_ppm < 0 ? -half : half)) / slope;
        if (step == 0) {
            step = (base_ppm < 0) ? -1 : 1;
        }
        if (step > 64)  step = 64;
        if (step < -64) step = -64;
        int32_t next = base_code + step;
        if (next < 0)    next = 0;
        if (next > 1023) next = 1023;
        if (next == code) {
            break;                              /* on the rail */
        }
        code = next;
        clock_rc32k_code_wr(code);
    }
    if (best_code >= 0 && best_code != code) {
        clock_rc32k_code_wr(best_code);            /* never leave it worse than the best point */
        printf("[PM] rc32k cal: restored best code %ld (%ld ppm)\r\n",
               (long)best_code, (long)best_ppm);
    }
    if (residual_ppm) *residual_ppm = best_ppm;
    if (ppm_per_lsb) *ppm_per_lsb = slope;
    return it > 6 ? 6 : it;
}

