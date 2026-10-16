/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 SoC header for Zephyr. Deliberately free of SDK/NMSIS includes so it
 * can be pulled in by generic Zephyr code; driver adapters include the SDK
 * HAL headers themselves.
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO7_SOC_H_
#define ZEPHYR_SOC_INTELPRO_IPRO7_SOC_H_

#include "ipro7_csr.h"

/* Memory map (bsp/drivers/ipro7_soc/regs/ipro7.h) */
#define IPRO7_FLASH_XIP_BASE        0x13000000UL
#define IPRO7_PSRAM_XIP_BASE        0x14000000UL
#define IPRO7_OCRAM_BASE            0x11010000UL
#define IPRO7_OCRAM_SIZE            (256UL * 1024UL)
#define IPRO7_PSRAM_BASE            0x1E000000UL
#define IPRO7_PSRAM_SIZE            (8UL * 1024UL * 1024UL)
#define IPRO7_HBNRAM_BASE           0x20090000UL
#define IPRO7_HBNRAM_SIZE           (4UL * 1024UL)
/* Non-cacheable alias: 0x1xxxxxxx -> 0x2xxxxxxx */
#define IPRO7_NOCACHE_ADDR_OFFSET   0x10000000UL

/* Interrupt numbering: 19 core-local lines, externals start at 19. */
#define IPRO7_IRQ_NUM_BASE          19
#define IPRO7_IRQ_MSIP              3
#define IPRO7_IRQ_MTIP              7

#ifndef _ASMLANGUAGE
#include <stdint.h>

/** IREGION base discovered at boot from MIRGB_INFO (0 until soc_early_init). */
uintptr_t ipro_iregion_base(void);

/** ECLIC memory-mapped base (IREGION + 0x20000). */
static inline uintptr_t ipro_eclic_base(void)
{
	return ipro_iregion_base() + IPRO7_IREGION_ECLIC_OFS;
}

/** Private systimer base (IREGION + 0x30000). */
static inline uintptr_t ipro_systimer_base(void)
{
	return ipro_iregion_base() + IPRO7_IREGION_TIMER_OFS;
}

#endif /* !_ASMLANGUAGE */

#endif /* ZEPHYR_SOC_INTELPRO_IPRO7_SOC_H_ */
