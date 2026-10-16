/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Watchdog facade over the SDK HAL (hal_wdt.c) and LL (drv_timer.c). Pure
 * SDK side: hal_wdt.h's wdt_feed()/wdt_disable() share their names with
 * Zephyr's watchdog API.
 *
 * Reset mode uses hal_wdt.c's wdt_init() unchanged. Interrupt mode needs a
 * callback the HAL's fixed wdt_isr() cannot provide, so it is armed here
 * with the same LL sequence and the ISR registered through the SDK
 * interrupt seam (system_soc.h -> Zephyr dynamic IRQ).
 */

#include <ipro_zephyr/ipro_hal_glue.h>

#include "hal_wdt.h"
#include "drv_timer.h"
#include "ipro_soc_regs.h"

static void (*wdt_user_isr)(void);

static void ipro_wdt_irq(void)
{
#ifdef CONFIG_SOC_SERIES_IPRO6S
	/* The IPRO6 LL's WDT_ClearInt() skips the WFAR/WSAR unlock every other
	 * WDT_* function does, so its WICR write is dropped and the level IRQ
	 * storms (153k callbacks in 8 s on the EVB). Unlock first. */
	WDT_ENABLE_ACCESS(TIMER_BASE);
#endif
	WDT_ClearInt(WDT0_ID);
	if (wdt_user_isr != NULL) {
		wdt_user_isr();
	}
}

void ipro_hal_wdt_set_isr(void (*isr)(void))
{
	wdt_user_isr = isr;
}

void ipro_hal_wdt_start(bool irq_mode, uint32_t timeout_ms)
{
	if (!irq_mode) {
		wdt_init(0, timeout_ms);
		return;
	}

	WDT_Disable(WDT0_ID);
	WDT_SetCompValue(WDT0_ID, (uint16_t)timeout_ms);
	Interrupt_Handler_Register(TIMER0_WDT_IRQn, ipro_wdt_irq);
	WDT_IntMask(WDT0_ID, WDT_INT, UNMASK);
	CPU_Interrupt_Pending_Clear(TIMER0_WDT_IRQn);
	CPU_Interrupt_Enable(TIMER0_WDT_IRQn);
	WDT_Enable(WDT0_ID);
}

void ipro_hal_wdt_stop(void)
{
	wdt_disable();
}

void ipro_hal_wdt_feed(void)
{
	wdt_feed();
}
