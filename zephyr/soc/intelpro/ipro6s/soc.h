/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_H_
#define ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_H_

#include "ipro6s_csr.h"

/* Memory map (bsp/drivers/ipro6_soc/regs/ipro6.h, cacheable aliases). The
 * link-time XIP window is 0x13000000 like IPRO7 (see the SDK linker script
 * ipro6_common.ld.in), not the 0x10000000 alias in ipro6.h. */
#define IPRO6S_FLASH_XIP_BASE        0x13000000UL
#define IPRO6S_OCRAM_BASE            0x11000000UL
#define IPRO6S_OCRAM_SIZE            (320UL * 1024UL)
#define IPRO6S_WRAM_BASE             0x11050000UL
#define IPRO6S_WRAM_SIZE             (160UL * 1024UL)
#define IPRO6S_HBNRAM_BASE           0x20090000UL
#define IPRO6S_HBNRAM_SIZE           (4UL * 1024UL)
#define IPRO6S_NOCACHE_ADDR_OFFSET   0x10000000UL

#define IPRO6S_IRQ_NUM_BASE          19
#define IPRO6S_IRQ_MSIP              3
#define IPRO6S_IRQ_MTIP              7

#ifndef _ASMLANGUAGE
#include <stdint.h>

uintptr_t ipro_iregion_base(void);

static inline uintptr_t ipro_eclic_base(void)
{
	return ipro_iregion_base() + IPRO6S_IREGION_ECLIC_OFS;
}

static inline uintptr_t ipro_systimer_base(void)
{
	return ipro_iregion_base() + IPRO6S_IREGION_TIMER_OFS;
}
#endif /* _ASMLANGUAGE */

#endif /* ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_H_ */
