/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7 reset entry. Mirrors "Startup Stage 1/2" of the SDK's
 * bsp/drivers/ipro7_soc/startup/GCC/startup_ipro7.S, then hands over to the
 * generic Zephyr RISC-V __start. Everything that needs a stack (boot2
 * parameter save, clock boost, cache enable) is done later in
 * soc_prep_hook()/soc_early_init_hook().
 *
 * The boot2 loader jumps to flash offset 0 (0x13000000), so this function
 * must be the first thing in the image: init.ld pins ".init.*" to ROM_START.
 */

#include <zephyr/toolchain.h>
#include <zephyr/arch/riscv/csr.h>
#include "ipro7_csr.h"

GTEXT(__ipro7_start)
GTEXT(__start)

SECTION_FUNC(init, __ipro7_start)
	/* Global interrupts off until the kernel is ready. */
	csrc mstatus, MSTATUS_MIE

	/* Only the boot hart runs; any other hart parks in WFI. */
	csrr a0, mhartid
	andi a0, a0, 0xFF
	bnez a0, __ipro7_amp_wait

	/* Global pointer for gp-relative addressing (RISCV_SOC_HAS_GP_RELATIVE_ADDRESSING). */
	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop

	/* NMI shares the mtvec entry with mcause = 0xFFF (Nuclei MMISC_CTL). */
	li t0, IPRO7_MMISC_CTL_NMI_CAUSE_FFF
	csrs IPRO7_CSR_MMISC_CTL, t0

	/* Zc* compressed extensions are not part of the IPRO7 march. */
	li t0, IPRO7_MMISC_CTL_ZC
	csrc IPRO7_CSR_MMISC_CTL, t0

	/*
	 * The boot ROM runs the Nuclei SDK and leaves mtvt2 (0x7EC) enabled,
	 * pointing non-vectored interrupts at its own irq_entry, and mtvt at
	 * its vector table. With MTVT2EN set the core ignores mtvec for
	 * interrupts, so the first timer IRQ would run ROM code against
	 * Zephyr's stack (observed: illegal instruction at mepc 0xfc8 one
	 * tick after boot). Route everything through mtvec (_isr_wrapper).
	 */
	csrw IPRO7_CSR_MTVT2, zero
	csrw IPRO7_CSR_MTVT, zero

	/* Unlock OPI: GLB 0x30000070[1] = 0 (same as the SDK startup). */
	lui a5, 0x30000
	lw a4, 112(a5)
	andi a4, a4, -3
	sw a4, 112(a5)

#ifdef CONFIG_IPRO7_NPU
	/* NPU power: GLB 0x3000008c[8] = 1 (CONFIG_AI_SUPPORT in the SDK). */
	lw a4, 140(a5)
	ori a4, a4, 0x100
	sw a4, 140(a5)
#endif

	/* FPU state -> Initial (Zephyr's __initialize sets FS again, harmless). */
	li t0, MSTATUS_FS
	csrc mstatus, t0
	li t0, IPRO7_MSTATUS_FS_INITIAL
	csrs mstatus, t0

	/* Enable mcycle / minstret counters. */
	csrci IPRO7_CSR_MCOUNTINHIBIT, 0x5

	/* Branch predictor on (SDK enables it right before main()). */
	li t0, IPRO7_MMISC_CTL_BPU
	csrs IPRO7_CSR_MMISC_CTL, t0

	tail __start

__ipro7_amp_wait:
	wfi
	j __ipro7_amp_wait
