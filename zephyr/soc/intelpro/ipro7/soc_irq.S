/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SoC context save/restore hooks for the Nuclei ECLIC (same fix as the
 * upstream gd32vf103 port): when executing MRET the ECLIC only restores the
 * interrupt level from mcause.MPIL if mcause.INTERRUPT is set, so force it
 * on the frame that is about to be restored.
 */

#include <zephyr/offsets.h>
#include <zephyr/toolchain.h>
#include <zephyr/arch/riscv/irq.h>

#ifdef CONFIG_RISCV_SOC_CONTEXT_SAVE
GTEXT(__soc_save_context)
GTEXT(__soc_restore_context)

SECTION_FUNC(exception.other, __soc_save_context)
	ret

SECTION_FUNC(exception.other, __soc_restore_context)
	addi a0, a0, -__struct_arch_esf_soc_context_OFFSET
	lw t0, __struct_arch_esf_mcause_OFFSET(a0)
	li t1, 1 << RISCV_MCAUSE_IRQ_POS
	or t0, t0, t1
	sw t0, __struct_arch_esf_mcause_OFFSET(a0)
	ret
#endif /* CONFIG_RISCV_SOC_CONTEXT_SAVE */
