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

#ifndef LP_AON_WDT_H
#define LP_AON_WDT_H

#include <stdint.h>

/* AON watchdog (AON 0x890): F32K-clocked and powered through PDS; a
 * timeout resets the chip. Timeout = 2^(mask_bit + 1) / 32768 s. */
#define LP_AON_WDT_REG       0x3008F890u
#define LP_AON_WDT_EN        (1u << 0)
#define LP_AON_WDT_KICK      (1u << 1)
#define LP_AON_WDT_CLR       (1u << 2)
#define LP_AON_WDT_MASK_POS  8u
#define LP_AON_WDT_MASK_MSK  (0x3Fu << LP_AON_WDT_MASK_POS)

/* Last LP step in HBN RAM (AON, kept over a chip reset): [31:24] tag
 * 'B' in the blob, 'S' re-entering PDS, 'F' at the full wake; [23:0]
 * resleep count. Printed and cleared at cold boot. */
#define LP_AON_STAMP_ADDR    0x200908E8u
#define LP_AON_STAMP         (*(volatile uint32_t *)LP_AON_STAMP_ADDR)
#define LP_AON_STAMP_CNT     0x00FFFFFFu

static inline __attribute__((always_inline)) void lp_aon_stamp(uint32_t tag, uint32_t inc)
{
    uint32_t v = LP_AON_STAMP;

    LP_AON_STAMP = (tag << 24) | ((v + inc) & LP_AON_STAMP_CNT);
}

#ifndef CONFIG_LP_AON_WDT_MASK_BIT
#define CONFIG_LP_AON_WDT_MASK_BIT 17
#endif

#define LP_AON_WDT_MASK_MAX  20u

/* Uses the smallest timeout >= Kconfig above 2 x sleep_us; a sleep
 * too long for the largest timeout leaves it stopped. */
static inline __attribute__((always_inline)) void lp_aon_wdt_start(uint64_t sleep_us)
{
    volatile uint32_t *r = (volatile uint32_t *)LP_AON_WDT_REG;
    uint32_t v = *r & ~(LP_AON_WDT_EN | LP_AON_WDT_KICK);
    uint32_t n = CONFIG_LP_AON_WDT_MASK_BIT;

    while (n <= LP_AON_WDT_MASK_MAX
           && (1000000ull << (n + 1)) / 32768u <= 2u * sleep_us) {
        n++;
    }
    *r = v;
    if (n > LP_AON_WDT_MASK_MAX) {
        return;
    }
    *r = v | LP_AON_WDT_CLR;
    v = (v & ~LP_AON_WDT_MASK_MSK) | (n << LP_AON_WDT_MASK_POS);
    *r = v | LP_AON_WDT_EN;
    *r = v | LP_AON_WDT_EN | LP_AON_WDT_KICK;
}

static inline __attribute__((always_inline)) void lp_aon_wdt_kick(void)
{
    volatile uint32_t *r = (volatile uint32_t *)LP_AON_WDT_REG;
    uint32_t v = *r;

    if (v & LP_AON_WDT_EN) {
        *r = v | LP_AON_WDT_KICK;
    }
}

static inline __attribute__((always_inline)) void lp_aon_wdt_stop(void)
{
    volatile uint32_t *r = (volatile uint32_t *)LP_AON_WDT_REG;

    *r = *r & ~(LP_AON_WDT_EN | LP_AON_WDT_KICK);
}

#endif
