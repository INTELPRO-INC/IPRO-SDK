/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * Non-vectored ECLIC dispatch (RISCV_SOC_HAS_CUSTOM_IRQ_HANDLING).
 * Same algorithm as Zephyr's intc_clic.S: read-and-clear mnxti to obtain
 * the highest pending interrupt (as mtvt + 4*irq), call its _sw_isr_table
 * entry, repeat until mnxti returns 0.
 */

#include <zephyr/arch/cpu.h>
#include <zephyr/linker/sections.h>
#include <zephyr/arch/riscv/csr.h>
#include "intc_ipro_eclic.h"

GTEXT(__soc_handle_irq)
/* ECLIC clears pending on mnxti read; nothing to acknowledge here.
 * In .text (not .exception.other) so zephyr_code_relocate() can move the trap
 * path to RAM (zephyr/cmake/ipro_wifi_hot.cmake). */
SECTION_FUNC(TEXT, __soc_handle_irq)
	ret

GTEXT(__soc_handle_all_irqs)
#ifdef CONFIG_TRACING_ISR
GTEXT(sys_trace_isr_enter)
GTEXT(sys_trace_isr_exit)
#endif

SECTION_FUNC(TEXT, __soc_handle_all_irqs)
	addi sp, sp, -16
	sw ra, 0(sp)

	/* a0 = mtvt + 4*irq of the highest pending interrupt, or 0. Also
	 * clears MSTATUS.MIE for the loop (mnxti csrrci semantics). */
	csrrci a0, CSR_MNXTI, MSTATUS_MIE
	beqz a0, irq_done

irq_loop:
#ifdef CONFIG_TRACING_ISR
	call sys_trace_isr_enter
#endif
	/* (a0 - mtvt) is 4*irq; _sw_isr_table entries are 2 words wide. */
	csrr t0, CSR_MTVT
	sub a0, a0, t0
	la t0, _sw_isr_table
	slli a0, a0, 1
	add t0, t0, a0

	lw a0, 0(t0)      /* ISR argument */
	lw t1, 4(t0)      /* ISR function */
	jalr ra, t1, 0

#ifdef CONFIG_TRACING_ISR
	call sys_trace_isr_exit
#endif

	csrrci a0, CSR_MNXTI, MSTATUS_MIE
	bnez a0, irq_loop

irq_done:
	lw ra, 0(sp)
	addi sp, sp, 16
	ret
