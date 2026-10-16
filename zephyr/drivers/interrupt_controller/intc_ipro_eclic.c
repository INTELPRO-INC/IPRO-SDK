/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 (Nuclei N300) ECLIC driver.
 *
 * Functionally the LEGACY_CLIC_MEMORYMAP_ACCESS path of Zephyr's generic
 * drivers/interrupt_controller/intc_clic.c, with one difference that made a
 * separate driver necessary: the register base is discovered at run time
 * from the MIRGB_INFO CSR (soc.c) instead of coming from a device-tree
 * "reg" property, because the IPRO7 does not document a fixed IREGION
 * address and its ROM/SDK also resolve it dynamically.
 *
 * Non-vectored mode is used for every interrupt: mtvec = _isr_wrapper|3,
 * and __soc_handle_all_irqs (intc_ipro_eclic.S) drains mnxti.
 */

#define DT_DRV_COMPAT intelpro_ipro_eclic

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/arch/riscv/csr.h>
#include <zephyr/drivers/interrupt_controller/riscv_clic.h>
#include <zephyr/sys/util.h>
#include <soc.h>
#include "intc_ipro_eclic.h"

struct ipro_eclic_data {
	mem_addr_t base;
	uint8_t nlbits;
	uint8_t intctlbits;
	uint16_t numint;
};

static struct ipro_eclic_data eclic;

static inline void eclic_write8(uint32_t ofs, uint8_t v)
{
	sys_write8(v, eclic.base + ofs);
}

static inline uint8_t eclic_read8(uint32_t ofs)
{
	return sys_read8(eclic.base + ofs);
}

void riscv_clic_irq_enable(uint32_t irq)
{
	eclic_write8(ECLIC_INTIE_OFS(irq), 1);
}

void riscv_clic_irq_disable(uint32_t irq)
{
	eclic_write8(ECLIC_INTIE_OFS(irq), 0);
}

int riscv_clic_irq_is_enabled(uint32_t irq)
{
	return eclic_read8(ECLIC_INTIE_OFS(irq)) & 1;
}

void riscv_clic_irq_priority_set(uint32_t irq, uint32_t pri, uint32_t flags)
{
	/*
	 * clicintctl[i] = level bits (nlbits, MSB first) then priority bits,
	 * unimplemented low bits read as 1. Same encoding as intc_clic.c:
	 * a larger Zephyr "priority" is a higher ECLIC level and preempts.
	 */
	const uint8_t max_level = BIT_MASK(eclic.nlbits);
	const uint8_t max_prio = BIT_MASK(eclic.intctlbits - eclic.nlbits);
	uint8_t intctl = (MIN(pri, max_prio) << (8U - eclic.intctlbits)) |
			 (MIN(pri, max_level) << (8U - eclic.nlbits)) |
			 BIT_MASK(8U - eclic.intctlbits);

	eclic_write8(ECLIC_INTCTL_OFS(irq), intctl);

	/* Machine mode, non-vectored, trigger from the IRQ flags (0 = level). */
	eclic_write8(ECLIC_INTATTR_OFS(irq),
		     ECLIC_INTATTR_MODE_M | ((flags & BIT_MASK(2)) << ECLIC_INTATTR_TRIG_POS));
}

void riscv_clic_irq_vector_set(uint32_t irq)
{
	/* Selective hardware vectoring is not used on this port. */
	ARG_UNUSED(irq);
}

void riscv_clic_irq_set_pending(uint32_t irq)
{
	eclic_write8(ECLIC_INTIP_OFS(irq), 1);
}

void riscv_clic_irq_pending_clear(uint32_t irq)
{
	eclic_write8(ECLIC_INTIP_OFS(irq), 0);
}

static int ipro_eclic_init(void)
{
	uint32_t info;

	eclic.base = ipro_eclic_base();
	__ASSERT(eclic.base != 0, "IREGION base not discovered before ECLIC init");

	info = sys_read32(eclic.base + ECLIC_INFO_OFS);
	eclic.numint = info & ECLIC_INFO_NUMINT_MASK;
	eclic.intctlbits = (info & ECLIC_INFO_INTCTLBITS_MASK) >> ECLIC_INFO_INTCTLBITS_POS;

	eclic.nlbits = CONFIG_INTC_IPRO_ECLIC_NLBITS;
	if (eclic.nlbits == 0 || eclic.nlbits > eclic.intctlbits) {
		eclic.nlbits = eclic.intctlbits;
	}

	/* Threshold 0: nothing masked by level. */
	eclic_write8(ECLIC_MTH_OFS, 0);

	/* Number of level bits (cliccfg.nlbits). */
	uint8_t cfg = eclic_read8(ECLIC_CFG_OFS);

	cfg = (cfg & ~ECLIC_CFG_NLBITS_MASK) | (eclic.nlbits << ECLIC_CFG_NLBITS_POS);
	eclic_write8(ECLIC_CFG_OFS, cfg);

	/* Every line disabled, not pending, level triggered, M-mode. */
	for (uint32_t irq = 0; irq < CONFIG_NUM_IRQS; irq++) {
		sys_write32(0, eclic.base + ECLIC_INT_OFS(irq));
	}

	/*
	 * mnxti returns mtvt | 4*irq (the ECLIC ORs the 4-byte-entry offset
	 * into the table base, which is why Nuclei asks for a 512-byte aligned
	 * table with 83 lines). __soc_handle_all_irqs subtracts mtvt again and
	 * indexes Zephyr's 8-byte _sw_isr_table itself, so the cleanest base is
	 * 0: the arithmetic is then exact for any table alignment and nothing
	 * the boot ROM left in mtvt can leak in.
	 */
	csr_write(CSR_MTVT, 0);

	return 0;
}

/* Priority 0: every driver that calls irq_enable() at PRE_KERNEL_1 (GPIO,
 * UART, ...) must find the controller configured. */
SYS_INIT(ipro_eclic_init, PRE_KERNEL_1, 0);
