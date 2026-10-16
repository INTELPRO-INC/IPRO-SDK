/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Symbols the SDK HAL expects from bsp/common/platform_common.c and
 * bsp/board/ipro7/board.c, provided on top of the Zephyr kernel instead.
 *
 * platform_common.c is not compiled for Zephyr: it owns the SDK console
 * (uart_init on the debug UART, printf plumbing) which Zephyr replaces
 * with its console/log subsystems. board.c is not compiled either: its
 * work is split between soc.c (power, EM, clocks) and the device tree
 * (pin mux, peripheral clocks).
 */

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>
#include <zephyr/sys/printk.h>

#include "hal_common.h"
#include "hal_uart.h"
#include "platform_common.h"

/* board.c: everything board_init() did is done by soc_early_init_hook(). */
void board_init(void)
{
}

int board_get_debug_uart_index(void)
{
	return UART0_INDEX;
}

/* bsp/board/ipro7/board.c's answer for the BLE controller: no Zephyr board here fits a 32 kHz
 * crystal, so RC32K (HBN_32K_RC) without the XTAL32K probe. */
int board_get_32k_source(void)
{
	return 0;
}

/* platform_common.c timebase helpers, in kernel terms. */
uint64_t platform_get_time_ms(void)
{
	return k_uptime_get();
}

uint64_t platform_get_time_us(void)
{
	return k_ticks_to_us_floor64(k_uptime_ticks());
}

void platform_delay_ms(uint32_t ms)
{
	k_msleep(ms);
}

void platform_delay_us(uint32_t us)
{
	k_busy_wait(us);
}

void platform_putchar(const char c)
{
	printk("%c", c);
}

int platform_get_random(uint8_t *data, uint32_t len)
{
	sys_rand_get(data, len);
	return 0;
}

void platform_print_set(uint8_t disable)
{
	ARG_UNUSED(disable);
}

uint8_t platform_print_get(void)
{
	return 0;
}

/*
 * drv_pds.c's PDS_Enable() records its register writes into this diagnostic
 * array, normally owned by the LP framework (hal_pm.c) which Zephyr does not
 * compile until its power-management port lands. Provide the storage so the
 * LL layer links unchanged.
 */
volatile uint32_t pm_pds_hbn_arm_diag[40];

#ifdef CONFIG_IPRO_HAL_USE_PSRAM
/*
 * hal_psram.c keeps a small pre-scheduler calibration log at the SDK
 * linker symbol __heap_start (the FreeRTOS heap is unused at that point).
 * Zephyr's linker script has no such symbol and its heap is allocated
 * differently, so give the HAL a dedicated, uninitialised scratch buffer
 * under that name instead. Size = sizeof(psram_early_log_t) in hal_psram.c
 * (magic + len + reserved + 256-byte text), rounded up.
 */
__noinit __aligned(4) char __heap_start[8 + 256];
#endif

/* ---- D-cache maintenance ------------------------------------------------ */
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
/* SoC header first: it defines __DCACHE_PRESENT, without which
 * core_feature_cache.h compiles the flush helpers to nothing. */
#include "ipro_soc_regs.h"
#include "core_feature_cache.h"
#endif

void ipro_hal_cache_clean(uint32_t addr, size_t size)
{
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
	MFlushDCacheRange((uintptr_t)addr, size);
#else
	ARG_UNUSED(addr); ARG_UNUSED(size);
#endif
}

void ipro_hal_cache_invalidate(uint32_t addr, size_t size)
{
#if defined(CONFIG_IPRO7_DCACHE) || defined(CONFIG_IPRO6S_DCACHE)
	MFlushInvalDCacheRange((uintptr_t)addr, size);
#else
	ARG_UNUSED(addr); ARG_UNUSED(size);
#endif
}
