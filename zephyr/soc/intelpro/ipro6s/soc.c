/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO6S SoC bring-up for Zephyr.
 *
 * Replaces what the SDK does in startup_ipro6.S (stage 3), system_soc.c
 * (SystemInit/_premain_init) and bsp/board/ipro6/board.c (board_init):
 * boot2 hand-off (when a boot2 is present), caches, brown-out reset, efuse
 * trims, XTAL type, the mtimer divider, the MCU clock tree fallback and the
 * WRAM/EM split. Pin muxing and peripheral clock gating come from the
 * device tree through the pinctrl and clock-control drivers.
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
#include "ipro6.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_clock.h"
#include "hal_boot2.h"
#include "hal_sys.h"
#include "boot2_param.h"
#include "core_feature_cache.h"
#include "core_feature_timer.h"

/*
 * IREGION base. The SDK keeps the same value in CpuIRegionBase
 * (system_soc.c) for the NMSIS inline helpers (SysTimer_*, ECLIC_*) that
 * the HAL uses; define it here so those helpers work without linking
 * system_soc.c.
 */
volatile unsigned long CpuIRegionBase = 0xFFFFFFFFUL;
static uintptr_t iregion_base;

uintptr_t ipro_iregion_base(void)
{
	return iregion_base;
}

static inline void ipro6s_discover_iregion(void)
{
	unsigned long info = csr_read(IPRO6S_CSR_MIRGB_INFO);

	iregion_base = (uintptr_t)((info >> 10) << 10);
	CpuIRegionBase = iregion_base;
}

/*
 * soc_prep_hook: runs on the interrupt stack BEFORE .bss is zeroed and
 * .data copied. Only XIP-resident code and .noinit data may be used here.
 */
/* No loop may be turned into a memcpy/memset call here: the RAM-resident
 * copies of those are exactly what this hook installs. */
__attribute__((optimize("no-tree-loop-distribute-patterns")))
void soc_prep_hook(void)
{
#ifdef CONFIG_IPRO_WIFI_HOT_CODE
	/* The relocated hot path (CONFIG_CODE_DATA_RELOCATION) includes libc's
	 * memcpy/memset; z_data_copy() would install it later, but the SDK code
	 * called below already uses them, so install it here first (word loop:
	 * nothing in this function may call memcpy). */
	{
		extern uint32_t __ram_text_reloc_start[], __ram_text_reloc_size[],
			__ram_text_rom_start[];
		const uint32_t *src = __ram_text_rom_start;
		uint32_t *dst = __ram_text_reloc_start;
		uint32_t words = ((uint32_t)__ram_text_reloc_size + 3) / 4;

		while (words--) {
			*dst++ = *src++;
		}
		__asm__ volatile("fence.i" ::: "memory");
	}
#endif
	/*
	 * A boot2-loaded image finds the partition table + flash config at
	 * BOOT2_PARAM_SRC_ADDR (0x11010000, inside our OCRAM); copy them out
	 * before the kernel overwrites that area. A BootROM-direct image
	 * (the border-router layout) has nothing there: Boot2_Param_Save()
	 * validates the magic and leaves g_boot2_param invalid.
	 */
	Boot2_Param_Save();

	/* SDK ATTR_OCRAM_BSS_SECTION data lives outside Zephyr's .bss. */
	{
		extern char __ipro_bss_start[];
		extern char __ipro_bss_end[];

		for (char *p = __ipro_bss_start; p < __ipro_bss_end; p += 4) {
			*(volatile uint32_t *)p = 0;
		}
	}
}

/*
 * WRAM / Exchange Memory split (board_wram_em_sel_init in board.c): one bit
 * per 16 KB of WRAM handed to the BLE controller, from the top. Must be
 * programmed before anything touches WRAM.
 */
static void ipro6s_wram_em_init(void)
{
	volatile uint32_t *const sram_cfg3 = (volatile uint32_t *)0x3000060cu;
	uint32_t mask = (1u << (CONFIG_IPRO6S_EM_SIZE_KB / 16u)) - 1u;

	if ((*sram_cfg3 & 0x0fu) != mask) {
		*sram_cfg3 = (*sram_cfg3 & ~0x0fu) | mask;
	}
}

static void ipro6s_clock_init(void)
{
#ifdef CONFIG_IPRO6S_MCU_CLOCK_FALLBACK
	/* board_clock_init(): the bootheader normally leaves 240/80 MHz. */
	if (Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_CLK) != 240000000u ||
	    Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_MCU_PBCLK) != 80000000u) {
		GLB_Simple_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_240M, 0, 2);
	}
#endif
	/* Record the board XTAL type in HBN_RSV3 (HBN_Get_Xtal_Value users). */
	HBN_Set_Xtal_Type(GLB_XTAL_40M);

	/* mtime = XCLK / toggle_cnt = 1 MHz (SYS_CLOCK_HW_CYCLES_PER_SEC). */
	CPU_Set_MTimer_Toggle_Cnt(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_XCLK) / 1000000u);

	/* System_Post_Init(): RC32K trim. */
	HBN_Trim_RC32K();
}

static void ipro6s_bor_init(void)
{
	HBN_BOR_CFG_Type bor = {
		.enableBor = 1,
		.enableBorInt = 0,
		.borThreshold = 1,
		.enablePorInBor = 1,
	};

	HBN_Set_BOR_Cfg(&bor);
}

static void ipro6s_cache_init(void)
{
	if (ICachePresent()) {
		EnableICache();
	}
	__RWMB();
	__FENCE_I();

#ifdef CONFIG_IPRO6S_DCACHE
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

	ipro6s_cache_init();
	ipro6s_discover_iregion();

	/* SystemInit(): reset reason latch, brown-out reset. */
	hal_rst_reason_detect();
	ipro6s_bor_init();

	/* _premain_init(): efuse-driven trims. */
	hal_boot2_init_efuse();

	/* board_init() */
	ipro6s_wram_em_init();
	ipro6s_clock_init();

	irq_unlock(key);
}

/*
 * soc_interrupt_init(): make sure every ECLIC line the ROM may have left
 * armed is disabled and cleared before the kernel installs its handlers.
 */
void soc_interrupt_init(void)
{
	(void)arch_irq_lock();
	ipro6s_discover_iregion();

	volatile uint8_t *ctl = (volatile uint8_t *)(ipro_eclic_base() + 0x1000);

	for (int irq = 0; irq < CONFIG_NUM_IRQS; irq++) {
		ctl[irq * 4 + 0] = 0; /* clicintip */
		ctl[irq * 4 + 1] = 0; /* clicintie */
	}
}

#ifdef CONFIG_REBOOT
/*
 * sys_reboot(): same as IPRO7 - record SOFTWARE, clear the user boot config
 * and pull the GLB POR-class reset through the ROM driver.
 */
void sys_arch_reboot(int type)
{
	ARG_UNUSED(type);
	(void)irq_lock();
	hal_rst_reason_set(HAL_RST_REASON_SOFTWARE);
	(void)HBN_Set_User_Boot_Config(0);
	(void)GLB_SW_POR_Reset();
	for (;;) {
	}
}
#endif /* CONFIG_REBOOT */

/* Reset into the BootROM UART loader (shell "reboot_dl"). */
FUNC_NORETURN void ipro_soc_reboot_download(void)
{
	(void)irq_lock();
	(void)HBN_Set_User_Boot_Config(1);
	(void)GLB_SW_POR_Reset();
	for (;;) {
	}
}
