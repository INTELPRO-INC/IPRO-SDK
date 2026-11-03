/*
 * Entry points of the U-mode FreeRTOS port. No CSR access: the SPE
 * saves and restores the whole context as struct psa_nspe_frame and
 * NSPE_ECALL_IRQ_RETURN resumes one.
 */
#include "psa_ipro/abi.h"

#define NSPE_IRQ_NONE (-1)

/*
 * vIRQ entry from the SPE: a0 = vIRQ id, a1 = sp = frame of the
 * interrupted task, virq_mask = 1. Runs nspe_trap_c on the boot stack
 * (_nspe_stack_top); its {frame, irq} in a0/a1 go to IRQ_RETURN.
 */
    .section .text.nspe_trap_entry, "ax", @progbits
    .balign 4
    .globl nspe_trap_entry
    .type nspe_trap_entry, @function
nspe_trap_entry:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, _nspe_stack_top
    call nspe_trap_c
    li a7, NSPE_ECALL_IRQ_RETURN
    ecall
1:
    j 1b
    .size nspe_trap_entry, . - nspe_trap_entry

/* a0 = frame of the first task; the SPE unmasks vIRQs */
    .section .text.nspe_start_first_task, "ax", @progbits
    .balign 4
    .globl nspe_start_first_task
    .type nspe_start_first_task, @function
nspe_start_first_task:
    li a1, NSPE_IRQ_NONE
    li a7, NSPE_ECALL_IRQ_RETURN
    ecall
1:
    j 1b
    .size nspe_start_first_task, . - nspe_start_first_task
