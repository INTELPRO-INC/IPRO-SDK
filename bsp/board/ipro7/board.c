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

#include "board_config.h"
#include "drv_aon.h"
#include "drv_clock.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_pwm.h"
#include "drv_timer.h"
#include "generated/autoconf.h"
#include "hal_clock.h"
#include "hal_gpio.h"
#include "hal_hbnram_mgnt.h"
#include "hal_mtimer.h"
#include "hal_pm.h"
#include "hal_uart.h"
#include "platform_common.h"
#include "drv_pds.h"

static void board_pin_mux_init(void) {
    /* Release the PDS pad latch before touching any pin mux.
     *
     * PDS entry sets PDS_CR_PDS_GPIO_KEEP_EN, which freezes every pad in the
     * state it had going to sleep so nothing glitches while the core is off.
     * Nothing on the IPRO7 path ever cleared it, so on the boot that follows a
     * PDS wake the pads stay frozen and any driver that configures a GPIO and
     * then waits on it hangs - observed as pdm_mic_demo stopping right after
     * "PDM configured", inside pdm_buttons_init().
     *
     * Harmless on a cold boot: the bit is already clear. */
    PDS_Disable_GPIO_Keep();

    /*
     * Undo the PDS wake configuration as well.
     *
     * PDS_GPIO_IE_SET, the per-group trigger in PDS_GPIO_INT_SET and
     * PDS_GPIO_INT_MASK all live in the always-on domain, so they survive the
     * reset that ends a PDS cycle and are still armed on the boot that
     * follows. That leaves the pads owned by PDS rather than by GLB, and
     * whatever configures those pins next hangs - the same failure the keep
     * bit caused, from a different register. Mask everything and drop the
     * input enables; an application that wants a PDS wake re-arms it through
     * pm_pds_gpio_wakeup_set().
     */
    writel(0xFFFFFFFFu, PDS_BASE + PDS_GPIO_INT_MASK_OFFSET);
    writel(0x00000000u, PDS_BASE + PDS_GPIO_IE_SET_OFFSET);
    PDS_Set_All_GPIO_IntClear();

    /*
     * Clear every GLB per-pin interrupt latch as well.
     *
     * The event that ends a PDS cycle leaves its pin latched, and the latch
     * survives into the next boot. gpio_attach_irq() finishes by enabling the
     * shared GPIO_INT0 line, so the pending interrupt is taken immediately -
     * before the driver has a callback for that pin, so nothing acknowledges
     * it and it is taken again, and again. The board looks hung; it is
     * actually servicing an interrupt storm. Observed as pdm_mic_demo printing
     * "init: attach_irq" and nothing after it.
     */
    for (int pin = 0; pin <= 41; pin++) {
        GLB_GPIO_IntClear(pin, 1);
        GLB_GPIO_IntClear(pin, 0);
    }

    BOARD_PINMUX_INIT();
    BOARD_UART_ROUTING_INIT();

#if BOARD_PINMUX_COUNT > 0
    /* SD PWR EN - set initial output level (skip for SLT) */
    gpio_write(GLB_GPIO_PIN_15, 0);
#endif

#if defined(CONFIG_ESP_HOSTED)
    /* esp-hosted slave EN (CONFIG_ESP_HOSTED_RESET_PIN, default GPIO14):
     * drive HIGH at POR so the slave doesn't stay in reset before the
     * application calls esp_hosted_init_from_config(). Without this, the
     * pin defaults to input-with-pull-down and pulls the ESP32 EN line
     * low, keeping the slave held in reset. */
    gpio_set_mode(CONFIG_ESP_HOSTED_RESET_PIN, GPIO_OUTPUT_MODE);
    gpio_write(CONFIG_ESP_HOSTED_RESET_PIN, 1);
#endif

    /* Re-apply GLB_PARM CCI/JTAG bits that brom normally programs in
     * ipro_bootrom_cci_jtag_coexsit + GLB_JTAG_Sig_Swap_Set.  Under
     * CONFIG_SUPPORT_PURE_DCDC the cold-init HBN bounce wakes through
     * pure_dcdc_wakeup_stub and skips the brom flow entirely, so these
     * bits are left at POR defaults and JTAG is dead.  Idempotent on
     * classic boot (brom already set the same values). */
#if defined(CONFIG_IPRO_CCI_NATIVE_0_1_2_7) && (CONFIG_IPRO_CCI_NATIVE_0_1_2_7 != 0)
    static GLB_GPIO_Cfg_Type cci_pinmux_table[] = {
        { GLB_GPIO_PIN_0,  15, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },
        { GLB_GPIO_PIN_1,  15, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },
        { GLB_GPIO_PIN_2,  15, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },
        { GLB_GPIO_PIN_7,  15, GPIO_MODE_AF, GPIO_PULL_UP, 0, 1 },
    };
    for (uint32_t i = 0; i < sizeof(cci_pinmux_table) / sizeof(cci_pinmux_table[0]); i++) {
        GLB_GPIO_Init(&cci_pinmux_table[i]);
    }
    GLB_CCI_Use_Jtag_Pin(0);
    GLB_CCI_Use_IO_0_1_2_7(1);
#else
    GLB_CCI_Use_Jtag_Pin(1);
    GLB_JTAG_Sig_Swap_Set(0);
#endif
}

#if XTAL_TYPE != EXTERNAL_XTAL_32M
static void internal_rc32m_init(void) {
    uint32_t tmpVal;
    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_clr_bit(tmpVal, AON_XTAL_CAPCODE_EXTRA_AON);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 0);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_CAPCODE_IN_AON, 0);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_RDY_SEL_AON, 0);
    writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);

    tmpVal = readl(AON_BASE + AON_TSEN_OFFSET);
    tmpVal = reg_set_bits(tmpVal, AON_XTAL_RDY_INT_SEL_AON, 0);
    writel(tmpVal, AON_BASE + AON_TSEN_OFFSET);

    for (uint32_t i = 0; i < 20000; i++) {
        tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
        tmpVal = reg_set_bit(tmpVal, AON_XTAL_EXT_SEL_AON);
        writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);
        tmpVal = readl(AON_BASE + AON_XTAL_CFG_OFFSET);
        tmpVal = reg_clr_bit(tmpVal, AON_XTAL_EXT_SEL_AON);
        writel(tmpVal, AON_BASE + AON_XTAL_CFG_OFFSET);
        if (reg_is_bit_set(readl(GLB_BASE + GLB_CLK_CFG0_OFFSET), GLB_CHIP_RDY))
            break;
    }
}
#endif

void system_clock_init(void) {
    BOARD_SYSTEM_CLOCK_INIT();
    BOARD_MTIMER_CLK_INIT();
    BOARD_32K_CLOCK_INIT();
    BOARD_XCLK_INIT();
}

void peripheral_clock_init(void) {
    /* Use generated clock gating configuration */
    BOARD_CLOCK_GATING_INIT();

    /* Use generated peripheral clock configuration */
    BOARD_PERIPHERAL_CLOCK_INIT();
}

static void board_clock_init(void) {
#if defined(CONFIG_LINK_FLASH)
    system_clock_init();
#endif
    peripheral_clock_init();
}

static void board_em_init(void) {
    // Set EM (Exchange Memory) size based on CONFIG_EM_SIZE from proj.conf
    // This must match the linker configuration for proper memory allocation
    // Available OCRAM = 256KB - CONFIG_EM_SIZE
#if defined(CONFIG_EM_SIZE)
#if (CONFIG_EM_SIZE == 0)
    GLB_Set_EM_Sel(GLB_EM_0KB);
#elif (CONFIG_EM_SIZE == 16)
    GLB_Set_EM_Sel(GLB_EM_16KB);
#elif (CONFIG_EM_SIZE == 32)
    GLB_Set_EM_Sel(GLB_EM_32KB);
#elif (CONFIG_EM_SIZE == 48)
    GLB_Set_EM_Sel(GLB_EM_48KB);
#elif (CONFIG_EM_SIZE == 64)
    GLB_Set_EM_Sel(GLB_EM_64KB);
#else
#error "Invalid CONFIG_EM_SIZE value. Supported: 0, 16, 32, 48, 64"
#endif
#else
    // Default to 0KB if CONFIG_EM_SIZE not defined (backward compatibility)
    GLB_Set_EM_Sel(GLB_EM_0KB);
#endif
}

void plt_show_cg_info(void) {
    MSG("Build:%s,%s\r\n", __TIME__, __DATE__);
#if 0
    MSG("#####################################################\r\n");
    MSG("root clock:%luM\r\n", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_ROOT) / 1000000); /*root clock before f_div*/

    MSG("fclk clock:%luM\r\n", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK) / 1000000); /*after f_div,this is system core clock*/
    MSG("bclk clock:%luM\r\n", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_BCLK) / 1000000);
    MSG("xclk clock:%luM\r\n", Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000000);

    MSG("uart clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_UART0) / 1000000);
    MSG("spi clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_SPI0) / 1000000);
    MSG("i2c clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2C0) / 1000000);
    MSG("adc clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_GPADC) / 1000000);
    MSG("dac clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_GPDAC) / 1000000);
    MSG("i2s clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_I2S) / 1000000);
    MSG("pwm clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_PWM) / 1000000);
    MSG("cam clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_CAM) / 1000000);
    MSG("timer0 clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER0) / 1000000);
    MSG("timer1 clock:%luM\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_TIMER1) / 1000000);
    MSG("UART SIG7-SIG0 :%08lx, SIG11-SIG8: %08lx\r\n", readl(GLB_BASE + GLB_UART_SIG_SEL_0_OFFSET), readl(GLB_BASE + GLB_UART_SIG_SEL_1_OFFSET));
    MSG("#####################################################\r\n");
#endif
}

/* DCDC14 vout code. 0xb = 1.4V, 0x0F = 1.5V, 0x1B = 1.8V.
 * Override at build time to A/B compare different VDD14 targets. */
#ifndef BOARD_DCDC14_VOUT_SEL
#  if defined(CONFIG_BOARD_VDD14_1V4)
#    define BOARD_DCDC14_VOUT_SEL 0x0B  /* 1.4V — verified pure DCDC ok */
#  elif defined(CONFIG_BOARD_VDD14_1V5)
#    define BOARD_DCDC14_VOUT_SEL 0x0F  /* 1.5V */
#  else
#    define BOARD_DCDC14_VOUT_SEL 0x1b  /* 1.8V default */
#  endif
#endif

/* Override ldo14_vout_sel_aon while LDO14 is disabled (or for active
 * LDO+DCDC drive). 3-bit field (0..7). Empirical: divider in LDO14_TOP
 * leaks current even with pu_ldo14_aon=0, so a lower value saves
 * current. */
#ifndef BOARD_LDO14_VOUT_SEL
#  if defined(CONFIG_BOARD_VDD14_1V4)
#    define BOARD_LDO14_VOUT_SEL 0x2
#  elif defined(CONFIG_BOARD_VDD14_1V5)
#    define BOARD_LDO14_VOUT_SEL 0x4
#  else
#    define BOARD_LDO14_VOUT_SEL 0x7
#  endif
#endif


static void board_power_init(void) {
#if defined(CONFIG_NO_RTOS)
    /* Boot2 (CONFIG_NO_RTOS=y): leave VDD14 rail untouched. POR HW startup
     * already brings DCDC ready by the time boot2 runs; bounce-wake takes
     * the wakeup-stub path which bypasses boot2 entirely. FW's
     * __pure_dcdc_early_init owns the LDO14→DCDC switch + bond option
     * + HBN bounce on cold POR. */
    return;
#elif defined(CONFIG_SUPPORT_PURE_DCDC)
    /* Topology switch (LDO14 → DCDC) is done in startup_ipro7.S
     * (__pure_dcdc_early_init) before any LMA→VMA copy.
     * Marker: HBN_MISC[11] (vdd14_bond_option_enb_aon), persists in AON
     * across all resets — naturally takes a warm-skip path on subsequent
     * boots without an HBN bounce.
     *
     * Only the runtime DCDC vout target is programmed here — vout setting
     * is fine in this (post-LMA) stage and is idempotent across boots. */
    AON_Set_DCDC14_Top_0(BOARD_DCDC14_VOUT_SEL, 0x3);
#elif defined(CONFIG_LDO_SUPPORT)
    HBN_Power_On_LDO14();
    HBN_Power_Off_DCDC();
    AON_Set_LDO14_Vout_Sel(BOARD_LDO14_VOUT_SEL);
#else
    /* Classic DCDC mode (board_power_init runs every boot):
     *   0. Init: enable both DCDC and LDO14 so VDD14 rail is always driven.
     *   1. Enable VDD14 bond option.
     *   2. Re-assert DCDC on; wait dcdc_rdy.
     *   3. Disable LDO14.
     *   4. Program DCDC14 output voltage.
     *   5. Program LDO14 vout_sel (lowers residual divider leakage). */
    HBN_Power_On_DCDC();
    HBN_Power_On_LDO14();

    HBN_Set_VDD14_Bond_Option(1);

    HBN_Power_On_DCDC();
    AON_Wait_DCDC_Ready(0);

    HBN_Power_Off_LDO14();

    AON_Set_DCDC14_Top_0(BOARD_DCDC14_VOUT_SEL, 0x3);
    AON_Set_LDO14_Vout_Sel(BOARD_LDO14_VOUT_SEL);
#endif
}

void board_init(void) {
    board_power_init();
    board_em_init();
    board_clock_init();
    board_pin_mux_init();
}

int board_get_debug_uart_index(void) { return UART0_INDEX; }

/* Used for boot2 iap */
void system_mtimer_clock_init(void) {
    mtimer_clear_time();
    GLB_Set_MTimer_CLK(
        1, GLB_MTIMER_CLK_XCLK,
        Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1);
}
