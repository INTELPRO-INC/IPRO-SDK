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
 * @file clock_boost.c
 * @brief Early-boot clock bring-up — XIP-resident, called from startup
 *        assembly BEFORE LMA->VMA copy.
 *
 * Targets HCLK = PLL 192M, BCLK = HCLK/2 = 96M so the LMA copy inside
 * __init_common runs at 6x the default XCLK speed.
 *
 * Two callers:
 *   - Normal cold POR:  boot2 already powered PLL/DLL and may have set
 *                       root clock. We early-out if PLL is already on
 *                       (avoid unnecessary PLL re-lock).
 *   - Fastboot wake:    boot2 was skipped by the HBN cold-boot path
 *                       (commit 9f6dc5c7762e); PLL/DLL are off, so we
 *                       must do the full bring-up.
 *
 * Implementation: dispatches via ROM API (RomDriver_GLB_Set_System_CLK),
 * which is the authoritative vendor PLL/DLL bring-up sequence baked
 * into bootrom. Since the ROM lives at fixed addresses in flash and
 * uses no user-firmware globals, it's safe to call before LMA->VMA
 * copy and BSS clear are done.
 *
 * SECTION: this file is placed in .text.init so the function runs
 * directly from XIP flash, no relocation needed.
 */

#include <stdint.h>
#include "drv_glb.h"            /* GLB_DLL_XTAL_Type, GLB_SYS_CLK_Type */
#include "drv_romdriver.h"      /* RomDriver_GLB_Set_System_CLK */

#define ATTR_TEXT_INIT __attribute__((section(".text.init")))

#define GLB_CLK_CFG1            (*(volatile uint32_t *)(uintptr_t)0x30000004U)
#define GLB_REG_PLL_EN_BIT      (1U << 0)

void ATTR_TEXT_INIT clock_boost_init(void)
{
    /* Fast-path: boot2 already brought PLL up and we just need it to
     * be selected. RomDriver_GLB_Set_System_CLK still does the right
     * thing (it power-cycles PLL/DLL each call, then re-locks), but
     * the re-lock costs ~10-15 µs of cold-boot time on the normal
     * cold-POR path.  Skip it when CLK_CFG1[0] REG_PLL_EN is already
     * set — that bit only gets set when the PLL is up. */
    if (GLB_CLK_CFG1 & GLB_REG_PLL_EN_BIT) {
        /* PLL already running.  In principle we should still set
         * the root clock + dividers if boot2 didn't, but boot2 in
         * practice leaves the system on RC32M/XTAL via XCLK; the
         * later board_init() will switch to PLL192M when called
         * from main().  Leave the existing behaviour for the
         * non-fastboot path to avoid regressing boot speed. */
        return;
    }

    /* Fastboot wake (PLL/DLL not powered): full bring-up via ROM API.
     * This is the same call BOARD_SYSTEM_CLOCK_INIT() makes later, but
     * via ROM so it's reachable before LMA->VMA copy. The ROM function
     * uses no user-firmware data — only registers + its own stack. */
    RomDriver_GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_PLL192M);

    /* GLB_Set_System_CLK_Ex internally calls GLB_Set_System_CLK_Div(0,1)
     * for the PLL192M case, so HCLK_DIV=0 and BCLK_DIV=1 are already
     * set by the call above.  No follow-up needed. */
}
