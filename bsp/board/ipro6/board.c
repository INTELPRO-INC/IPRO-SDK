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
 * Uses IPRO7's CPU
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
#include "hal_hbnram_mgnt.h"   /* HAL_HBNRAM_CLK32K_OVR_ADDR */

/* Sleep-clock override word written by `lp clk32k` (see lp_demo.c); the value
 * is duplicated here because the app header is not visible to the BSP. */
#define BOARD_CLK32K_OVR_MAGIC  0xC32B0000u
#define BOARD_CLK32K_OVR_RC     2u
/* Longest XTAL32K start-up the probe waits for, once per cold boot */
#define BOARD_XTAL32K_SETTLE_MS 3000u

#if !defined(CONFIG_FPGA)
/* Retune MCU/BCLK only when they differ from the target, powering
 * XTAL + WIFIPLL first if the WIFIPLL is down. */
static void board_mcu_clock_set(uint32_t mcu_hz, uint32_t bclk_hz,
                                uint8_t mcu_clk, uint8_t bclk_div)
{
    if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) == mcu_hz &&
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) == bclk_hz) {
        return;
    }
    if (!(readl(CCI_BASE + CCI_WIFI_PLL_CFG0_OFFSET) & CCI_PU_WIFIPLL_MSK)) {
        GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    }
    GLB_Simple_Set_MCU_System_CLK(mcu_clk, 0, bclk_div);
}
#endif

static void board_clock_init(void)
{
#if !defined(CONFIG_FPGA)
#if defined(CONFIG_BTDM_CLOCK_320_80_TEST)
    /* Application-scoped discriminator: keep BCLK at 80 MHz while selecting
     * the 320 MHz root. */
    board_mcu_clock_set(320000000u, 80000000u,
                        GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M, 3);
#elif defined(CONFIG_BTDM_CLOCK_320_160_TEST)
    /* Match the BL616D reference board implementation exactly. */
    board_mcu_clock_set(320000000u, 160000000u,
                        GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M, 1);
#elif defined(CONFIG_BTDM_CLOCK_240_120_TEST)
    /* Keep the product MCU root but test the next-higher BCLK rate. */
    board_mcu_clock_set(240000000u, 120000000u,
                        GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M, 1);
#else
    /* The IPRO6 bootheader already establishes the final 240/80 MHz tuple
     * before entering firmware.  Do not retune an already-correct tree: the
     * RW-BLE Core requires its CPU/master clock relationship to stay static,
     * and IPRO6S A0's live EM bridge can latch a bad phase during a redundant
     * root/divider transition.  Keep a fallback for RAM/debug boots whose
     * loader did not apply the production bootheader clock configuration. */
    board_mcu_clock_set(240000000u, 80000000u,
                        GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M, 2);
#endif
#endif

    /* Record the board XTAL type in HBN_RSV3 so HBN_Get_Xtal_Value() works
     * (consumed by rfparam_init to size the RF/modem clock config; without
     * this it reads back 0 and modem init falls back to a wrong xtal). */
    HBN_Set_Xtal_Type(GLB_XTAL_40M);

    /* The MTimer is clocked from XCLK (xclk / toggle_cnt); derive the toggle
     * from the live XCLK so one tick == 1 us. A hard-coded 32 only yields 1 us
     * on a 32 MHz XCLK -- this part runs a 40 MHz XTAL, so 32 gave a 1.25 MHz
     * tick (mtimer_get_time_us 25% fast). */
    CPU_Set_MTimer_Toggle_Cnt(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000000);

    /* The 32 kHz sleep clock is a system-wide setting: it clocks the PDS/HBN
     * sequencer and the RTC, not just the WiFi standby path. Resolve it here,
     * at boot, so a board without the 32.768 kHz crystal can never run with a
     * dead clock selected (that hangs the first code that waits on the RTC).
     *
     * clock_32k_resolve() keeps an already-adopted, still-ticking crystal
     * (this function also re-runs from the low-power wake hook, where probing
     * again would cost the settle on every wake and swap the clock under the
     * DTIM tracker), and otherwise probes once and falls back to RC32K.
     *
     * The build policy is the default; the retained override word written by
     * `lp clk32k xtal|rc` (HBN RAM, survives a soft reset, not a power cycle)
     * wins when present, so the CLI still selects the clock for the next boot. */
    {
        /* Retained across a PDS wake (RAM keeps its contents and the wake path
         * does not re-run the C startup), so the crystal adopted at cold boot
         * is kept on every wake without re-measuring the RTC. */
        static uint8_t s_xtal_adopted;
        /* want_xtal of the last resolve this boot; 0xFF = none yet */
        static uint8_t s_resolved_want = 0xFFu;
        uint32_t ovr = *(volatile uint32_t *)HAL_HBNRAM_CLK32K_OVR_ADDR;
        uint32_t sel;
        int want_xtal;

        if ((ovr & 0xFFFF0000u) == BOARD_CLK32K_OVR_MAGIC) {
            want_xtal = ((ovr & 0xFFFFu) != BOARD_CLK32K_OVR_RC);
        } else {
#ifdef CONFIG_LP_FORCE_RC32K
            want_xtal = 0;              /* crystal-less product: never probe */
#else
            want_xtal = 1;              /* auto: probe, and fall back to RC32K */
#endif
        }
        sel = (readl(HBN_BASE + HBN_GLB_OFFSET) & HBN_F32K_SEL_MSK) >> HBN_F32K_SEL_POS;
        if (s_resolved_want == (uint8_t)want_xtal &&
            sel == (s_xtal_adopted ? HBN_32K_XTAL : HBN_32K_RC)) {
            /* wake path: the source this boot resolved is still selected */
        } else {
            s_xtal_adopted = (uint8_t)clock_32k_resolve(want_xtal,
                                                       BOARD_XTAL32K_SETTLE_MS);
            s_resolved_want = (uint8_t)want_xtal;
        }
    }

    /* Gate every module clock the platform does not run on; each driver
     * ungates its own module from its init. Reset leaves all of CGEN_CFG0 set,
     * so an uninitialised module is clocked for nothing: measured 2026-09-16,
     * gating them is -11.1 mA awake unassociated and -8.3 mA associated.
     *
     * This sits in board_clock_init() because it has to run on BOTH paths: the
     * cold boot and every full-firmware wake (lp_wakeup_hook -> board_init()).
     * A PDS wake restores CGEN_CFG0 to its reset value, so gating only at cold
     * boot loses the saving after the first DTIM cycle. */
    GLB_PER_Clock_Gate_All();
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
 * linker sizes ram_wifi as RAM_WIFI_SIZE(160K) - __EM_SIZE (the configured
 * controller carve when BT is linked). If the HW split and that carve disagree, the top of ram_wifi
 * overlaps the EM region and any DMA into it (e.g. an SDIO TX pbuf in a high
 * pool slot) silently corrupts memory. Derive the split from the actual usable
 * ram_wifi span (_eshram - _sshram) so em_sel always matches the linked size.
 *
 * Called from board_init() entry, before any WRAM consumer. HW-probed
 * 2026-07-10: GLB_SRAM_CFG3 is writable from board_init entry (a plain writel
 * sticks even before clock init) and nothing in the boot path overwrites it;
 * the earlier "only writable from main() after platform_init / must use the
 * ROM path" notes were a misdiagnosis. */
#include "wram_split.h"

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

    if (_sshram == (char *)0 || _eshram == (char *)0) {
        return;
    }

    usable = (uint32_t)(uintptr_t)_eshram - (uint32_t)(uintptr_t)_sshram;

    const uint32_t expected_raw = ipro6_wram_em_mask(usable);
    if (expected_raw == UINT32_MAX) {
        /* Never start WRAM consumers with an unsupported linked split. */
        while (1) { }
    }

    /* Boot ROM normally applies the same split from the image header.  Do not
     * re-latch a live SRAM/EM mux merely to write the value already present;
     * only change routing when the linked layout actually differs. */
    {
        volatile uint32_t *const sram_cfg3 =
            (volatile uint32_t *)(uintptr_t)0x3000060cu;
        /* The ROM GLB API accepts only 0/32/64 KiB. Write the same field
         * directly so 16 KiB remains correct with CONFIG_USE_ROM_DRIVERS. */
        if ((*sram_cfg3 & 0x0fu) != expected_raw)
            *sram_cfg3 = (*sram_cfg3 & ~0x0fu) | expected_raw;
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
