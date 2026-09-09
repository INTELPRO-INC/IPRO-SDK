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
 * IPRO6 Board initialization
 * Uses IPRO6LE's SoC peripherals with IPRO7's CPU
 */
#include "hal_clock.h"
#include "hal_uart.h"
#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_glb_gpio.h"
#include "drv_pwm.h"
#include "drv_timer.h"
#include "drv_clock.h"
#include "drv_hbn.h"
#include "platform_common.h"

static void board_clock_init(void)
{
#if defined(CONFIG_LINK_FLASH)
#if defined(CONFIG_BTDM_CLOCK_320_80_TEST)
    /* Application-scoped discriminator: keep BCLK at 80 MHz while selecting
     * the 320 MHz root. */
    if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) != 320000000u ||
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) != 80000000u) {
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M,
                                      0, 3);
    }
#elif defined(CONFIG_BTDM_CLOCK_320_160_TEST)
    /* Match the BL616D reference board implementation exactly. */
    if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) != 320000000u ||
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) != 160000000u) {
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M,
                                      0, 1);
    }
#elif defined(CONFIG_BTDM_CLOCK_240_120_TEST)
    /* Keep the product MCU root but test the next-higher BCLK rate. */
    if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) != 240000000u ||
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) != 120000000u) {
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M,
                                      0, 1);
    }
#else
    /* The IPRO6 bootheader already establishes the final 240/80 MHz tuple
     * before entering firmware.  Do not retune an already-correct tree: the
     * RW-BLE Core requires its CPU/master clock relationship to stay static,
     * and IPRO6S A0's live EM bridge can latch a bad phase during a redundant
     * root/divider transition.  Keep a fallback for RAM/debug boots whose
     * loader did not apply the production bootheader clock configuration. */
    if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) != 240000000u ||
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) != 80000000u) {
        GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M,
                                      0, 2);
    }
#endif
#endif

    HBN_32K_Sel(HBN_32K_RC);
    HBN_Power_Off_Xtal_32K();

    /* Record the board XTAL type in HBN_RSV3 so HBN_Get_Xtal_Value() works
     * (consumed by rfparam_init to size the RF/modem clock config; without
     * this it reads back 0 and modem init falls back to a wrong xtal). */
    HBN_Set_Xtal_Type(GLB_XTAL_40M);

    /* The MTimer is clocked from XCLK (xclk / toggle_cnt); derive the toggle
     * from the live XCLK so one tick == 1 us. A hard-coded 32 only yields 1 us
     * on a 32 MHz XCLK -- this part runs a 40 MHz XTAL, so 32 gave a 1.25 MHz
     * tick (mtimer_get_time_us 25% fast). */
    CPU_Set_MTimer_Toggle_Cnt(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000000);
}

static void board_pin_mux_init(void)
{
    /* UART0 on GPIO28 (TX) / GPIO29 (RX).
     * Pad function = UART (FUNC_SEL = GPIO_FUN_UART). The UART signal mux then
     * routes each pad's fixed sig position (sig = gpio % 8: GPIO28 -> SIG4,
     * GPIO29 -> SIG5) to the UART0 TXD/RXD signals. */
    uint8_t uart0_pins[2] = { 28, 29 };
    GLB_GPIO_Func_Init(GPIO_FUN_UART, uart0_pins, 2);

    /* Use the swap variant so each UART function stays unique: it relocates
     * the function to the requested sig and restores the old owner, instead of
     * leaving a stale duplicate (default UART0_TXD/RXD sit at SIG2/SIG3). */
    GLB_UART_Fun_Sel_Swap(GLB_UART_SIG_4, GLB_UART_SIG_FUN_UART0_TXD); /* GPIO28 -> UART0_TX */
    GLB_UART_Fun_Sel_Swap(GLB_UART_SIG_5, GLB_UART_SIG_FUN_UART0_RXD); /* GPIO29 -> UART0_RX */
}

/* Program the WRAM/EM split (GLB_SRAM_CFG3[3:0], em_sel) to match the ram_wifi
 * LENGTH the linker reserved. em_sel resets to 0x3 = GLB_WRAM128KB_EM32KB, which
 * carves the top 32 KB of WRAM (0x21070000..0x21078000) off for a BT/EM master
 * and leaves it mis-mapped for the CPU and the WiFi-MAC/SDIO DMA masters. The
 * linker sizes ram_wifi as RAM_WIFI_SIZE(160K) - __EM_SIZE (32K iff BT linked,
 * else EM_SIZE). If the HW split and that carve disagree, the top of ram_wifi
 * overlaps the EM region and any DMA into it (e.g. an SDIO TX pbuf in a high
 * pool slot) silently corrupts memory. Derive the split from the actual usable
 * ram_wifi span (_eshram - _sshram) so em_sel always matches the linked size.
 *
 * Called from board_init() entry, before any WRAM consumer. HW-probed
 * 2026-07-10: GLB_SRAM_CFG3 is writable from board_init entry (a plain writel
 * sticks even before clock init) and nothing in the boot path overwrites it;
 * the earlier "only writable from main() after platform_init / must use the
 * ROM path" notes were a misdiagnosis. GLB_Set_EM_Sel() is kept for the
 * field encoding, not for any write-enable magic. */
void board_wram_em_sel_init(void)
{
    /* WEAK on purpose. boot2 links this object for board_init() /
     * board_get_debug_uart_index() -- ld pulls a whole archive member in for any
     * symbol it needs -- but boot2's linker script has no ram_wifi region and so
     * defines neither of these, which used to break the boot2 link outright.
     * Weak lets it link, and the guard below keeps boot2 from programming a
     * split off a bogus span: it has no WRAM consumer, so doing nothing is
     * correct there. In the FW link both are defined and behaviour is unchanged. */
    extern char _sshram[] __attribute__((weak));   /* linker: ram_wifi usable */
    extern char _eshram[] __attribute__((weak));   /*         [start, end)     */
    uint32_t usable;
    uint8_t em_type;

    if (_sshram == (char *)0 || _eshram == (char *)0) {
        return;
    }

    usable = (uint32_t)(uintptr_t)_eshram - (uint32_t)(uintptr_t)_sshram;

    if (usable >= 160 * 1024)
        em_type = 0;    /* GLB_WRAM160KB_EM0KB  : 160 KB WRAM,  0 KB EM */
    else if (usable >= 128 * 1024)
        em_type = 1;    /* GLB_WRAM128KB_EM32KB : 128 KB WRAM, 32 KB EM */
    else
        em_type = 2;    /* GLB_WRAM96KB_EM64KB  :  96 KB WRAM, 64 KB EM */

    /* Boot ROM normally applies the same split from the image header.  Do not
     * re-latch a live SRAM/EM mux merely to write the value already present;
     * only change routing when the linked layout actually differs. */
    {
        volatile uint32_t *const sram_cfg3 =
            (volatile uint32_t *)(uintptr_t)0x3000060cu;
        const uint32_t expected_raw = (1u << (em_type * 2u)) - 1u;

        if ((*sram_cfg3 & 0x0fu) != expected_raw)
            GLB_Set_EM_Sel(em_type);
    }
}

void board_init(void)
{
    /* Program the WRAM/EM split before ANY WRAM consumer runs (WiFi MAC,
     * lwIP heap/memp in .wifibss). HW-probed 2026-07-10: GLB_SRAM_CFG3 is
     * writable from board_init entry (even a plain writel sticks, before
     * clock init) and nothing in the boot path overwrites it — the earlier
     * "not writable that early / must run from main()" note was a
     * misdiagnosis from the ipro6_if_wifi bring-up. */
    board_wram_em_sel_init();

    board_clock_init();
    board_pin_mux_init();
}

void plt_show_cg_info(void)
{
}

int board_get_debug_uart_index(void)
{
    return UART0_INDEX;
}
