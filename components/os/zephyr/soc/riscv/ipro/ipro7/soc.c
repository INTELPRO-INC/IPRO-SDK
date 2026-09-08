/* Copyright (c) 2025 IPRO
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file soc.c
 * @brief IPRO7 SOC initialization for Zephyr
 * 
 * This file provides the SOC-level initialization that integrates
 * with Zephyr's SYS_INIT mechanism. It calls the board initialization
 * functions which are implemented in board_config.c
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/arch/riscv/csr.h>
#include <zephyr/irq.h>
#include <stdint.h>

/* Forward declarations for platform initialization functions
 * These are implemented in board_config.c */
extern void board_em_init(void);
extern void board_clock_init(void);
extern void board_pin_mux_init(void);

/**
 * @brief Platform initialization for Zephyr
 * 
 * Initialize essential IPRO7 platform hardware in the correct sequence:
 * 1. board_em_init() - Energy Mode (OCRAM allocation)
 * 2. board_clock_init() - System and peripheral clocks
 * 3. board_pin_mux_init() - GPIO pin multiplexing (CRITICAL for UART!)
 * 
 * This replicates the FreeRTOS reference initialization sequence:
 *   setup_heap() → platform_init() → hal_boot2_init()
 * where platform_init() internally calls board_init() which does:
 *   board_em_init(), board_clock_init(), board_pin_mux_init()
 * 
 * CRITICAL: This must run before any UART/serial device driver initialization.
 */
static int ipro7_platform_init(void)
{
	/* Step 1: Initialize Energy Mode (OCRAM memory layout) */
	board_em_init();
	
	/* Step 2: Initialize system clocks and peripheral clocks */
	board_clock_init();
	
	/* Step 3: Initialize GPIO pin multiplexing 
	 * This configures GPIO20/21 for UART0 TX/RX - CRITICAL! */
	board_pin_mux_init();
	
	return 0;
}

/**
 * @brief Perform basic RISC-V interrupt and core initialization
 */
static int ipro7_riscv_init(void)
{
	/* Enable Machine External Interrupt for PLIC */
	unsigned long mie = csr_read(mie);
	mie |= (1UL << 11);  /* MIE_MEIE bit for machine external interrupt */
	csr_write(mie, mie);
	
	return 0;
}

/* Initialize platform hardware very early - before kernel init 
 * Priority 0 = earliest possible, runs before any other SYS_INIT */
SYS_INIT(ipro7_platform_init, PRE_KERNEL_1, 0);

/* Initialize RISC-V core features after platform init */
SYS_INIT(ipro7_riscv_init, PRE_KERNEL_1, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);

/* ===== Required by Zephyr RISC-V interrupt handling ===== */

int __soc_is_irq(uint32_t mcause)
{
	/* Check if the cause indicates an external interrupt (bit 31 = 1, cause = 11) */
	return (mcause & 0x80000000U) && 
		   ((mcause & 0x7FFFFFFFU) == 11U); /* Machine external interrupt */
}

void __soc_handle_irq(unsigned long mcause)
{
	/* Simple pass-through to default interrupt handling */
	/* The PLIC driver and kernel will handle the actual IRQ routing */
}

/* ===== Required by PLIC driver ===== */

void arch_irq_enable(unsigned int irq)
{
	/* Enable interrupt in RISC-V machine mode */
	unsigned long mie_val;
	
	/* For external interrupts, enable machine external interrupt (bit 11) */
	if (irq >= 16) { /* External interrupts start at 16 */
		mie_val = csr_read(mie);
		mie_val |= (1UL << 11); /* MIE_MEIE bit */
		csr_write(mie, mie_val);
	}
}

void arch_irq_disable(unsigned int irq)
{
	/* Disable interrupt in RISC-V machine mode */
	unsigned long mie_val;
	
	if (irq >= 16) {
		mie_val = csr_read(mie);
		mie_val &= ~(1UL << 11); /* Clear MIE_MEIE bit */
		csr_write(mie, mie_val);
	}
}
