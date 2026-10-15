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

#ifndef __HAL_CLOCK__H__
#define __HAL_CLOCK__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

/*XTAL_TYPE*/
#define XTAL_NONE         0
#define EXTERNAL_XTAL_32M 1
#define INTERNAL_RC_32M   2

/*CLOCK_32K_XTAL*/
#define EXTERNAL_XTAL_32K 0
#define INTERNAL_RC_32K   1

/*BSP_ROOT_CLOCK_SOURCE*/
#if XTAL_TYPE != EXTERNAL_XTAL_32M
#define ROOT_CLOCK_SOURCE_XCLK 0
#else
#define ROOT_CLOCK_SOURCE_XCLK 1
#endif
#define ROOT_CLOCK_SOURCE_PLL_57P6M 2
#define ROOT_CLOCK_SOURCE_PLL_96M   3
#define ROOT_CLOCK_SOURCE_PLL_144M  4
/*BSP_XXX_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_32K_CLK 5
#define ROOT_CLOCK_SOURCE_FCLK    6
#define ROOT_CLOCK_SOURCE_BCLK    7
#define ROOT_CLOCK_SOURCE_1K_CLK  8

/*BSP_AUDIO_PLL_CLOCK_SOURCE*/
#define ROOT_CLOCK_SOURCE_AUPLL_12288000_HZ 9
#define ROOT_CLOCK_SOURCE_AUPLL_11289600_HZ 10
#define ROOT_CLOCK_SOURCE_AUPLL_5644800_HZ  11
#define ROOT_CLOCK_SOURCE_AUPLL_24576000_HZ 12
#define ROOT_CLOCK_SOURCE_AUPLL_24000000_HZ 13

void system_clock_init(void);
void peripheral_clock_init(void);
void system_mtimer_clock_init(void);

/*
 * 32 kHz source selection and RC32K trim (hal_clock.c) -- IPRO6 only.
 */
#if defined(IPRO6)
/**
 * Probe the 32.768 kHz crystal and, if it oscillates, adopt it as the 32 kHz
 * source: XTAL32K selected, RC32K powered down, and the AON hardware
 * pull disabled (HBN_IRQ_MODE.reg_en_hw_pu_pd = 0) so the PDS timed wake
 * works on it. Existence is proven by the RTC advancing at crystal rate
 * (+/-2%) against MTIMER — a board without the crystal fails the check and
 * is forced back to RC32K, so calling this on any board is safe.
 *
 * @param settle_ms crystal start-up wait (a watch crystal needs 200 ms-1 s).
 * @return 1 = XTAL32K adopted, 0 = absent/unhealthy, RC32K kept.
 * Blocks ~settle_ms + 100 ms; call from task context, once per boot.
 */
int clock_xtal32k_adopt(uint32_t settle_ms);
int clock_xtal32k_adopt_ex(uint32_t settle_ms, int busy);

/**
 * Resolve the 32 kHz source at boot, and on the low-power wake path that
 * re-runs board_init(): keep an already-adopted, still-ticking crystal (a
 * re-probe would cost the settle on every wake and swap the clock under the
 * DTIM tracker), fall back to RC32K when the selected crystal is dead, and
 * otherwise run the adopt probe once.
 *
 * @param want_xtal 0 = RC32K, the crystal is never powered.
 * @return 1 if XTAL32K ends up driving the 32 kHz, 0 for RC32K.
 */
int clock_32k_resolve(int want_xtal, uint32_t settle_ms);

/** Is the selected 32 kHz actually ticking? RTC vs MTIMER over 3 ms. */
int clock_32k_is_alive(void);

/**
 * Trim the RC32K oscillator against MTIMER (XTAL40M-derived, 1 us) for a
 * part without an RC32K efuse trim: measure the RTC rate over window_ms,
 * step HBN_RC32K_CTRL0.CODE_FR_EXT by round(ppm / slope) on a measured
 * (secant) slope starting from the vendor nominal ~1950 ppm/LSB, and repeat
 * until |ppm| <= 300 or 6 rounds; the best code seen is what stays. Call
 * with RC32K selected, before the first sleep is armed: the tracker's
 * RTC-domain state is stale after it.
 *
 * @param window_ms  measurement window per round (200 ms = 153 ppm resolution).
 * @param residual_ppm  out: the error at the retained code (seed for the tracker).
 * @param ppm_per_lsb   out: the slope measured (for the blob's fine trim).
 * @return rounds used (>= 1), or -1 when the measurement itself failed.
 * Blocks ~rounds x window_ms; task context only.
 */
int clock_rc32k_coarse_cal(uint32_t window_ms, int32_t *residual_ppm, int32_t *ppm_per_lsb);
#endif /* IPRO6 */

#ifdef __cplusplus
}
#endif
#endif
