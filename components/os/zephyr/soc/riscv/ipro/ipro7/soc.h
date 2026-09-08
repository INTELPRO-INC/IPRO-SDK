/*
 * Copyright (c) 2025 IPRO
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file SoC configuration macros for the IPRO7 processor
 */

#ifndef __RISCV_IPRO7_SOC_H_
#define __RISCV_IPRO7_SOC_H_

#include <../soc_common.h>

/* Include IPRO7 specific RISC-V definitions */
/* We need to be careful about macro conflicts with Zephyr */
#ifndef STRINGIFY
#define STRINGIFY_NOT_DEFINED
#endif

#ifndef MSTATUS_FS_CLEAN
#define MSTATUS_FS_CLEAN_NOT_DEFINED  
#endif

#ifndef MSTATUS_FS_DIRTY
#define MSTATUS_FS_DIRTY_NOT_DEFINED
#endif

#include "riscv_encoding.h"

/* Restore Zephyr definitions if they were redefined */
#ifdef STRINGIFY_NOT_DEFINED
#undef STRINGIFY
#undef STRINGIFY_NOT_DEFINED
#endif

#ifdef MSTATUS_FS_CLEAN_NOT_DEFINED
#undef MSTATUS_FS_CLEAN
#define MSTATUS_FS_CLEAN (2UL << 13)
#undef MSTATUS_FS_CLEAN_NOT_DEFINED
#endif

#ifdef MSTATUS_FS_DIRTY_NOT_DEFINED
#undef MSTATUS_FS_DIRTY 
#define MSTATUS_FS_DIRTY (3UL << 13)
#undef MSTATUS_FS_DIRTY_NOT_DEFINED
#endif

/* Define SOC_MCAUSE_EXP_MASK using IPRO7 MCAUSE_CAUSE definition if not already defined */
#ifndef SOC_MCAUSE_EXP_MASK
#define SOC_MCAUSE_EXP_MASK MCAUSE_CAUSE
#endif

/* UART configuration */
#define UART_NS16550_PORT_0_NAME		CONFIG_UART_NS16550_PORT_0_NAME
#define UART_NS16550_PORT_0_BASE_ADDR		DT_REG_ADDR(DT_NODELABEL(uart0))
#define UART_NS16550_PORT_0_IRQ			DT_IRQN(DT_NODELABEL(uart0))
#define UART_NS16550_PORT_0_CLK_FREQ		DT_PROP(DT_NODELABEL(uart0), clock_frequency)

/* Timer configuration */
#define RISCV_MTIME_BASE			DT_REG_ADDR_BY_IDX(DT_NODELABEL(mtimer), 0)
#define RISCV_MTIMECMP_BASE			DT_REG_ADDR_BY_IDX(DT_NODELABEL(mtimer), 1)

/* PLIC configuration */
#define RISCV_PLIC_BASE_ADDR			DT_REG_ADDR(DT_NODELABEL(plic))
#define RISCV_PLIC_MAX_PRIO			DT_PROP(DT_NODELABEL(plic), riscv_max_priority)
#define RISCV_PLIC_PRIO				(RISCV_PLIC_BASE_ADDR + 0x0)
#define RISCV_PLIC_IRQ_EN			(RISCV_PLIC_BASE_ADDR + 0x2000)
#define RISCV_PLIC_REG				(RISCV_PLIC_BASE_ADDR + 0x200000)

#endif /* _SOC_RISCV_IPRO_IPRO7_SOC_H_ */

