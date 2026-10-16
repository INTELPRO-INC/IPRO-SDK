/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_CONTEXT_H_
#define ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_CONTEXT_H_

#ifdef CONFIG_RISCV_SOC_CONTEXT_SAVE

/* No extra state: the hook only patches mcause (see soc_irq.S). */
#define SOC_ESF_MEMBERS
#define SOC_ESF_INIT

#endif /* CONFIG_RISCV_SOC_CONTEXT_SAVE */

#endif /* ZEPHYR_SOC_INTELPRO_IPRO6S_SOC_CONTEXT_H_ */
