/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 SoC bring-up for Zephyr.
 *
 * This replaces, in order, what the SDK does in startup_ipro7.S (stage 3),
 * system_soc.c (SystemInit/_premain_init) and bsp/board/ipro7/board.c
 * (board_init): boot2 hand-off, clock boost, caches, power rail, Exchange
 * Memory, system/mtimer/32K clocks and the PDS/GPIO latch clean-up that
 * every IPRO7 boot needs. Pin muxing and peripheral clock gating are NOT
 * done here - they come from the device tree through the pinctrl and
 * clock-control drivers.
 *
 * Everything below calls into the SDK HAL/LL layer (bsp/); nothing here
 * touches registers the SDK does not already own.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/irq.h>
#include <zephyr/arch/riscv/csr.h>
#include <zephyr/platform/hooks.h>
#include <soc.h>

/* SDK LL / HAL */
#include "ipro7.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_aon.h"
#include "drv_pds.h"
#include "drv_clock.h"
#include "hal_boot2.h"
#include "hal_sys.h"
#include "boot2_param.h"
#include "hal_hbnram_mgnt.h"
#include <string.h>
#include "core_feature_cache.h"
#include <zephyr/sys/reboot.h>

/* From bsp/drivers/ipro7_soc/startup/clock_boost.c (XIP resident). */
extern void clock_boost_init(void);

/*
 * IREGION base. The SDK keeps the same value in CpuIRegionBase (system_soc.c)
 * for the NMSIS inline helpers (SysTimer_*, ECLIC_*) that the HAL uses; we
 * define that symbol here so those helpers work without linking
 * system_soc.c, whose SystemInit()/_premain_init() would fight the kernel
 * for mtvec.
 */
volatile unsigned long CpuIRegionBase = 0xFFFFFFFFUL;
static uintptr_t iregion_base;

uintptr_t ipro_iregion_base(void)
{
	return iregion_base;
}

static inline void ipro7_discover_iregion(void)
{
	unsigned long info = csr_read(IPRO7_CSR_MIRGB_INFO);

	iregion_base = (uintptr_t)((info >> 10) << 10);
	CpuIRegionBase = iregion_base;
}

#ifdef CONFIG_IPRO_HAL_USE_PSRAM
/* section_copy.c (not compiled for Zephyr) owns these in the SDK. */
__attribute__((section(".ocram_bss"))) volatile uint16_t g_psram_early_dqs_cfg;
extern uint32_t psram_early_hw_init(void);

#define GLB_RSV3          0x3000007CUL
#define PSRAM_READY_MAGIC 0x50535249U /* "IPSR": boot2 already trained PSRAM */

/*
 * PSRAM controller bring-up + SDK ATTR_PSRAM_* section loading. Runs from
 * soc_prep_hook (XIP code, no .data/.bss yet) exactly where
 * early_section_copy() does it in the SDK.
 */
static void ipro7_psram_prep(void)
{
	extern char __ipro_psram_data_load_start[], __ipro_psram_data_start[],
		__ipro_psram_data_end[], __ipro_psram_bss_start[], __ipro_psram_bss_end[];

	if (*(volatile uint32_t *)GLB_RSV3 == PSRAM_READY_MAGIC) {
		*(volatile uint32_t *)GLB_RSV3 = 0; /* consume */
	} else {
		uint32_t dqs = psram_early_hw_init();

		g_psram_early_dqs_cfg = (uint16_t)(dqs & 0xFF);
		if (dqs == 0) {
			for (;;) {
				/* SDK behaviour: PSRAM training failed, halt. */
			}
		}
	}

	for (char *s = __ipro_psram_data_load_start, *d = __ipro_psram_data_start;
	     d < __ipro_psram_data_end; s += 4, d += 4) {
		*(volatile uint32_t *)d = *(volatile uint32_t *)s;
	}
	for (char *d = __ipro_psram_bss_start; d < __ipro_psram_bss_end; d += 4) {
		*(volatile uint32_t *)d = 0;
	}
}
#endif /* CONFIG_IPRO_HAL_USE_PSRAM */

/*
 * soc_prep_hook: runs on the interrupt stack BEFORE .bss is zeroed and
 * .data copied (see arch/riscv/core/prep_c.c). Only XIP-resident code
 * and .noinit data may be used here.
 */
void soc_prep_hook(void)
{
	/*
	 * boot2 leaves the partition table + flash config at the start of
	 * OCRAM (0x21010000). Copy them out (into .noinit) before the kernel
	 * overwrites that area with .data/.bss.
	 *
	 * Also mirror the raw bytes into the HBN RAM snapshot slot the SDK's
	 * LP framework uses for fast-boot wakes (HAL_HBNRAM_BOOT2_ADDR, "BOT2"
	 * + 640 B): a chained image - the application MCUboot jumps to - finds
	 * OCRAM already reused by the bootloader, and Boot2_Param_Save() falls
	 * back to that slot. Only written when OCRAM holds a valid table, so a
	 * later image never sees a stale copy over a fresh hand-off.
	 */
	{
		const uint8_t *src = (const uint8_t *)BOOT2_PARAM_SRC_ADDR;
		uint32_t magic;

		memcpy(&magic, src + 8, sizeof(magic));
		if (magic == IPRO_PT_MAGIC_CODE) {
			volatile uint32_t *hdr = (volatile uint32_t *)HAL_HBNRAM_BOOT2_ADDR;
			volatile uint8_t *dst = (volatile uint8_t *)(HAL_HBNRAM_BOOT2_ADDR + 4);

			for (uint32_t i = 0; i < HAL_HBNRAM_BOOT2_DATA_SIZE; i++) {
				dst[i] = src[i];
			}
			hdr[0] = HAL_HBNRAM_BOOT2_MAGIC;
		}
	}
	Boot2_Param_Save();

	/* SDK ATTR_OCRAM_BSS_SECTION data lives outside Zephyr's .bss. */
	{
		extern char __ipro_bss_start[];
		extern char __ipro_bss_end[];

		for (char *p = __ipro_bss_start; p < __ipro_bss_end; p += 4) {
			*(volatile uint32_t *)p = 0;
		}
	}

#ifdef CONFIG_IPRO7_CLOCK_BOOST_AT_RESET
	/* HCLK -> PLL 192 MHz for the section copy (no-op if boot2 did it). */
	clock_boost_init();
#endif

#ifdef CONFIG_IPRO_HAL_USE_PSRAM
	ipro7_psram_prep();
#endif
}

static void ipro7_power_init(void)
{
	/*
	 * Classic DCDC mode, identical to board_power_init() in board.c:
	 * keep VDD14 driven by both regulators, enable the bond option, wait
	 * for DCDC ready, then retire LDO14 and program the DCDC target.
	 */
	HBN_Power_On_DCDC();
	HBN_Power_On_LDO14();
	HBN_Set_VDD14_Bond_Option(1);
	HBN_Power_On_DCDC();
	AON_Wait_DCDC_Ready(0);
	HBN_Power_Off_LDO14();
	AON_Set_DCDC14_Top_0(CONFIG_IPRO7_DCDC14_VOUT_SEL, 0x3);
	AON_Set_LDO14_Vout_Sel(CONFIG_IPRO7_LDO14_VOUT_SEL);
}

static void ipro7_em_init(void)
{
	/* Exchange Memory for the BLE controller, carved from the OCRAM top. */
	GLB_EM_Type em;

	switch (CONFIG_IPRO7_EM_SIZE_KB) {
	case 0:
		em = GLB_EM_0KB;
		break;
	case 16:
		em = GLB_EM_16KB;
		break;
	case 32:
		em = GLB_EM_32KB;
		break;
	case 48:
		em = GLB_EM_48KB;
		break;
	case 64:
		em = GLB_EM_64KB;
		break;
	default:
		em = GLB_EM_0KB;
		break;
	}
	GLB_Set_EM_Sel(em);
}

static bool ipro7_sys_clock_at_target(void)
{
	uint32_t cfg0 = *(volatile uint32_t *)(GLB_BASE + GLB_CLK_CFG0_OFFSET);
	uint32_t root = (cfg0 >> GLB_HBN_ROOT_CLK_SEL_POS) &
			((1U << GLB_HBN_ROOT_CLK_SEL_LEN) - 1);
	uint32_t pll = (cfg0 >> GLB_REG_PLL_SEL_POS) & ((1U << GLB_REG_PLL_SEL_LEN) - 1);

	/* Target is always a DLL/PLL output: root must be DLL (2 or 3). */
	return (root == 2 || root == 3) &&
	       pll == (uint32_t)(GLB_SYS_CLK_PLL192M - GLB_SYS_CLK_DLL25P6M);
}

static void ipro7_clock_init(void)
{
	/*
	 * BOARD_SYSTEM_CLOCK_INIT(): PLL 192 MHz from the 32 MHz crystal,
	 * HCLK = 192 MHz, BCLK = 96 MHz. GLB_Set_System_CLK() briefly parks
	 * the root on XTAL which stalls OCRAM, so skip it if boot2 already
	 * left the chip at the target (same guard as the SDK board header).
	 */
	if (!ipro7_sys_clock_at_target()) {
		GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_PLL192M);
	} else {
		/*
		 * boot2 left the root on PLL192M, but which PLL/DLL branch
		 * outputs it enabled depends on the boot2 build; the SDK's full
		 * GLB_Set_System_CLK() turns every branch on. Do the same here
		 * so peripherals that select a PLL branch (CAM MCLK PLL48M,
		 * SDH PLL96M, USB 48M) never clock a dead source.
		 */
		GLB_Enable_PLL_All_Clks();
		GLB_Enable_DLL_All_Clks();
	}
	GLB_Set_System_CLK_Div(0, 1);

	/* BOARD_MTIMER_CLK_INIT(): mtimer = XCLK / 16 = 2 MHz (4 MHz count). */
	GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_XCLK,
			   Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000 / 1000 / 2 - 1);

	/* BOARD_32K_CLOCK_INIT(): RC32K, external 32 kHz crystal off. */
	HBN_Power_Off_Xtal_32K();
	HBN_32K_Sel(HBN_32K_RC);

	/* BOARD_XCLK_INIT(): XCLK from the 32 MHz crystal. */
	HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);

	/* System_Post_Init(): UART signal mux to a known state, RC32K trim. */
	*(volatile uint32_t *)(GLB_BASE + GLB_UART_SIG_SEL_0_OFFSET) = 0xffffffffU;
	HBN_Trim_RC32K();

	/* RTC counter for the 32 kHz timebase used by the LP framework. */
	HBN_Enable_RTC_Counter();
}

static void ipro7_pad_latch_release(void)
{
	/*
	 * Undo whatever a previous PDS cycle left armed (see the long comment
	 * in board.c: pad keep, PDS wake sources and latched GLB GPIO
	 * interrupts all survive the reset that ends a PDS sleep).
	 */
	PDS_Disable_GPIO_Keep();
	*(volatile uint32_t *)(PDS_BASE + PDS_GPIO_INT_MASK_OFFSET) = 0xFFFFFFFFu;
	*(volatile uint32_t *)(PDS_BASE + PDS_GPIO_IE_SET_OFFSET) = 0x00000000u;
	PDS_Set_All_GPIO_IntClear();

	for (int pin = 0; pin <= 41; pin++) {
		GLB_GPIO_IntClear(pin, 1);
		GLB_GPIO_IntClear(pin, 0);
	}
}

static void ipro7_bor_init(void)
{
	HBN_BOR_CFG_Type bor = {
		.enableBor = 1,
		.enableBorInt = 0,
		.borThreshold = 1,
		.enablePorInBor = 1,
	};

	HBN_Set_BOR_Cfg(&bor);
}

/*
 * The RF crystal trim every RF init loads (ipro7_rf.h). The RF and 802.15.4
 * cores ship prebuilt, so they ask for it at run time and this image's
 * CONFIG_IPRO7_XTAL_CAPCODE applies; 0 = none (the RF init loads 85).
 */
uint8_t ipro7_rf_xtal_capcode(void);
uint8_t ipro7_rf_xtal_capcode(void)
{
#if defined(CONFIG_IPRO7_XTAL_CAPCODE) && (CONFIG_IPRO7_XTAL_CAPCODE > 0)
	return CONFIG_IPRO7_XTAL_CAPCODE;
#else
	return 0;
#endif
}

#if defined(CONFIG_IPRO7_XTAL_CAPCODE) && (CONFIG_IPRO7_XTAL_CAPCODE > 0)
/* Capcode IN before the trim, for the boot log. */
static uint8_t ipro7_xtal_capcode_boot;

/*
 * Board RF crystal trim (SOC_IPRO7_XTAL_CAPCODE, mirrored to the SDK's
 * CONFIG_IPRO7_XTAL_CAPCODE), as the SDK's board_init() applies it. Every RF
 * init (ipro7_rf_private.c rf_update_or, the 802.15.4 HAL) loads it again.
 */
static void ipro7_xtal_capcode_init(void)
{
	ipro7_xtal_capcode_boot = AON_Get_Xtal_CapCode();
	(void)AON_Set_Xtal_CapCode(CONFIG_IPRO7_XTAL_CAPCODE, CONFIG_IPRO7_XTAL_CAPCODE);
}

/*
 * The trim runs before the console exists: report it once printk works.
 * APPLICATION level, because without EARLY_CONSOLE the console hooks printk
 * only at POST_KERNEL (CONSOLE_INIT_PRIORITY).
 */
static int ipro7_xtal_capcode_report(void)
{
	printk("[XTAL] capcode %u -> %u\n", (unsigned int)ipro7_xtal_capcode_boot,
	       (unsigned int)CONFIG_IPRO7_XTAL_CAPCODE);
	return 0;
}

SYS_INIT(ipro7_xtal_capcode_report, APPLICATION, 0);
#endif

static void ipro7_cache_init(void)
{
	if (ICachePresent()) {
		EnableICache();
	}
	__RWMB();
	__FENCE_I();

#ifdef CONFIG_IPRO7_DCACHE
	if (DCachePresent()) {
		EnableDCache();
	}
#endif
	__RWMB();
	__FENCE_I();
}

/*
 * soc_early_init_hook: first thing in z_cstart() after the C runtime is
 * ready, before any device init level. Interrupts are still off.
 */
void soc_early_init_hook(void)
{
	unsigned int key = irq_lock();

	ipro7_cache_init();
	ipro7_discover_iregion();

	/* SystemInit(): brown-out reset on every platform. */
	ipro7_bor_init();

	/* _premain_init(): efuse-driven trims and the reset reason latch. */
	hal_boot2_init_efuse();
	hal_rst_reason_detect();

#if defined(CONFIG_IPRO_HAL_USE_PSRAM)
	{
		extern void hal_psram_set_dqs_cfg(uint16_t cfg);
		extern unsigned int hal_boot2_get_psram_dqs_cfg(void);
		extern uint32_t __psram_available;
		extern volatile uint16_t g_psram_early_dqs_cfg;
		uint16_t dqs = g_psram_early_dqs_cfg;

		if (dqs == 0) {
			dqs = (uint16_t)hal_boot2_get_psram_dqs_cfg();
		}
		hal_psram_set_dqs_cfg(dqs);
		__psram_available = 1;
	}
#endif

	/*
	 * Boot2 arms the always-on watchdog (~16 s, port_wdt_start()) before
	 * it jumps to the application so a firmware that never comes up is
	 * recovered into USB/BootROM download; the SDK's platform_init()
	 * disarms it once the application runs. Do the same here, before any
	 * device init: an application that wants the AON WDT re-arms it through
	 * the watchdog driver. (Found on ipro7ai_evk 2026-09-21: hello_world
	 * reset by itself ~16 s after boot2, HBN event SW_RST.)
	 */
	(void)HBN_AON_WDT_Disable();

	/* board_init(): power rail, EM, clocks, PDS latch clean-up, crystal trim. */
	ipro7_power_init();
	ipro7_em_init();
	ipro7_clock_init();
	ipro7_pad_latch_release();
#if defined(CONFIG_IPRO7_XTAL_CAPCODE) && (CONFIG_IPRO7_XTAL_CAPCODE > 0)
	ipro7_xtal_capcode_init();
#endif

	irq_unlock(key);
}

/*
 * soc_interrupt_init(): Zephyr's generic version clears mie/mip. On an
 * ECLIC core mie is not the enable path; instead make sure every ECLIC
 * line the ROM/boot2 may have left armed is disabled and cleared before
 * the kernel installs its handlers (this is what SystemInit() does).
 */
void soc_interrupt_init(void)
{
	(void)arch_irq_lock();
	ipro7_discover_iregion();

	volatile uint8_t *ctl = (volatile uint8_t *)(ipro_eclic_base() + 0x1000);

	for (int irq = 0; irq < CONFIG_NUM_IRQS; irq++) {
		ctl[irq * 4 + 0] = 0; /* clicintip */
		ctl[irq * 4 + 1] = 0; /* clicintie */
	}
}

#ifdef CONFIG_REBOOT
/*
 * sys_reboot(): record the reason in HBN RAM so the next boot's
 * hal_rst_reason_detect() (and the hwinfo driver) report SOFTWARE, then
 * pull the GLB POR-class software reset - the same one the SDK's
 * hal_reboot() uses. Both SYS_REBOOT_WARM and SYS_REBOOT_COLD take it:
 * the GLB "system reset" (GLB_SW_System_Reset, SYS_RESET + CPU_RESET bits)
 * never came back on the ipro7_evb (no ROM banner, 2026-09-20), so there
 * is no lighter reset to offer. HBN RAM (.hbn_noinit) survives; OCRAM does
 * not, because boot2 runs from it. The LL routine is OCRAM-resident.
 */
void sys_arch_reboot(int type)
{
	ARG_UNUSED(type);
	(void)irq_lock();
	hal_rst_reason_set(HAL_RST_REASON_SOFTWARE);
	HBN_Set_User_Boot_Config(0);
	(void)GLB_SW_POR_Reset();
	for (;;) {
	}
}
#endif /* CONFIG_REBOOT */

/*
 * Reset into the BootROM's UART download mode (the SDK's hal_reboot_download,
 * shell "reboot_dl"): the user-boot-config bit in HBN tells the ROM to stay
 * in the loader instead of running boot2, and the IntelProCube tool can then
 * flash without touching the BOOT pin.
 */
FUNC_NORETURN void ipro_soc_reboot_download(void)
{
	(void)irq_lock();
	HBN_Set_User_Boot_Config(1);
	(void)GLB_SW_POR_Reset();
	for (;;) {
	}
}

#ifdef CONFIG_IPRO7_IDLE_NO_WFI
/* Diagnostic idle: never execute wfi, just re-enable interrupts and spin. */
void arch_cpu_idle(void)
{
	irq_unlock(MSTATUS_IEN);
}

void arch_cpu_atomic_idle(unsigned int key)
{
	irq_unlock(key);
}
#endif /* CONFIG_IPRO7_IDLE_NO_WFI */

