/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO6S reset entry. Mirrors "Startup Stage 1/2" of the SDK's
 * bsp/drivers/ipro6_soc/startup/GCC/startup_ipro6.S, then hands over to the
 * generic Zephyr RISC-V __start. Everything that needs a stack is done later
 * in soc_prep_hook()/soc_early_init_hook().
 *
 * The boot ROM (or boot2) jumps to flash offset 0 (0x13000000), so this
 * function must be the first thing in the image: init.ld pins ".init.*" to
 * ROM_START.
 */

#include <zephyr/toolchain.h>
#include <zephyr/arch/riscv/csr.h>
#include "ipro6s_csr.h"

GTEXT(__ipro6s_start)
GTEXT(__start)

SECTION_FUNC(init, __ipro6s_start)
	/* Global interrupts off until the kernel is ready. */
	csrc mstatus, MSTATUS_MIE

	/* Only the boot hart runs; any other hart parks in WFI. */
	csrr a0, mhartid
	andi a0, a0, 0xFF
	bnez a0, __ipro6s_amp_wait

	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop

	/* NMI shares the mtvec entry with mcause = 0xFFF (Nuclei MMISC_CTL). */
	li t0, IPRO6S_MMISC_CTL_NMI_CAUSE_FFF
	csrs IPRO6S_CSR_MMISC_CTL, t0

	/* Zc* compressed extensions are not part of the march. */
	li t0, IPRO6S_MMISC_CTL_ZC
	csrc IPRO6S_CSR_MMISC_CTL, t0

	/*
	 * Same as the IPRO7 port: the boot ROM leaves mtvt2 (0x7EC) enabled
	 * and mtvt pointing at its own tables; with MTVT2EN set the core
	 * ignores mtvec for interrupts. Route everything through mtvec.
	 */
	csrw IPRO6S_CSR_MTVT2, zero
	csrw IPRO6S_CSR_MTVT, zero

	/* Unlock OPI: GLB 0x30000070[1] = 0 (same as the SDK startup). */
	lui a5, 0x30000
	lw a4, 112(a5)
	andi a4, a4, -3
	sw a4, 112(a5)

	/* FPU state -> Initial (Zephyr's __initialize sets FS again, harmless). */
	li t0, MSTATUS_FS
	csrc mstatus, t0
	li t0, IPRO6S_MSTATUS_FS_INITIAL
	csrs mstatus, t0

	/* Enable mcycle / minstret counters. */
	csrci IPRO6S_CSR_MCOUNTINHIBIT, 0x5

	/* Branch predictor on (SDK enables it right before main()). */
	li t0, IPRO6S_MMISC_CTL_BPU
	csrs IPRO6S_CSR_MMISC_CTL, t0

	tail __start

__ipro6s_amp_wait:
	wfi
	j __ipro6s_amp_wait
