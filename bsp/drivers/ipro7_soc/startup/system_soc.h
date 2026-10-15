/*
 * Copyright (c) 2026 IntelPro Technology Co., Ltd.
 * All rights reserved.
 *
 * This software and associated documentation files (the "Software") are
 * proprietary and confidential to IntelPro Technology Co., Ltd..
 *
 * Access to and use of this Software is limited exclusively to authorized 
 * customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
 * valid license or non-disclosure agreement.
 *
 * Unauthorized copying, modification, distribution, or reverse engineering 
 * of this Software, via any medium, is strictly prohibited without 
 * prior written permission from IntelPro Technology Co., Ltd..
 *
 * SPDX-License-Identifier: Proprietary
 */

#ifndef __SYSTEM_SOC_H__
#define __SYSTEM_SOC_H__

#include "ipro7.h"

#define SDK_VER "1.00"
extern uint32_t SystemCoreClock;
extern int32_t ECLIC_Register_IRQ(IRQn_Type IRQn, uint8_t shv,
                                  ECLIC_TRIGGER_Type trig_mode, uint8_t lvl,
                                  uint8_t priority, void *handler);

typedef void (*pFunc)(void);

#define INTLEVEL 1
#define INTPRIORITY 0

/*
 * Interrupt controller seam.
 *
 * The HAL registers its ISRs and enables lines through these macros. On the
 * bare-metal / FreeRTOS SDK they program the ECLIC directly. When the HAL is
 * compiled into an RTOS that owns the interrupt controller (Zephyr, see
 * zephyr/hal_glue/ipro_irq_glue.c) the same calls are routed to the RTOS
 * so its ISR table stays authoritative. The HAL itself never includes an
 * RTOS header: only these prototypes are shared. IPRO_HAL_OS_IRQ_SEAM
 * selects the same routing explicitly (host-side HAL tests, tests/hal/).
 */
#if defined(__ZEPHYR__) || defined(IPRO_HAL_OS_IRQ_SEAM)
void ipro_os_irq_register(uint32_t irq, void (*handler)(void));
void ipro_os_irq_enable(uint32_t irq);
void ipro_os_irq_disable(uint32_t irq);
void ipro_os_irq_pending_clear(uint32_t irq);
void (*ipro_os_irq_handler_get(uint32_t irq))(void);

#define CPU_Interrupt_Enable(irq)            ipro_os_irq_enable(irq)
#define CPU_Interrupt_Disable(irq)           ipro_os_irq_disable(irq)
#define CPU_Interrupt_Pending_Clear(irq)     ipro_os_irq_pending_clear(irq)
#define CPU_Interrupt_Handler_Get(irq)       ipro_os_irq_handler_get(irq)
#define Interrupt_Handler_Register(irq, fun) ipro_os_irq_register(irq, (void (*)(void))(fun))
#else
#define CPU_Interrupt_Enable ECLIC_EnableIRQ
#define CPU_Interrupt_Disable ECLIC_DisableIRQ
#define CPU_Interrupt_Pending_Clear ECLIC_ClearPendingIRQ
#define CPU_Interrupt_Handler_Get ECLIC_GetVector
#define Interrupt_Handler_Register(irq, fun)                                   \
    ECLIC_Register_IRQ(irq, ECLIC_NON_VECTOR_INTERRUPT, ECLIC_LEVEL_TRIGGER,   \
                       INTLEVEL, INTPRIORITY, fun)
#endif

#define L1C_DCACHE_CLEAN_RANGE(addr, size)                                     \
    MFlushDCacheRange((uintptr_t)(addr), (size))
/* Bare invalidate is UNSAFE on non-cache-line-aligned buffers:
 * it discards dirty data from adjacent allocations sharing the
 * same cache line.  Redirect to flush+invalidate for safety. */
#define L1C_DCACHE_INVALID_RANGE(addr, size)                                   \
    MFlushInvalDCacheRange((uintptr_t)(addr), (size))
#define L1C_DCACHE_CLEAN_INVALID_RANGE(addr, size)                             \
    MFlushInvalDCacheRange((uintptr_t)(addr), (size))
#define L1C_DCACHE_CLEAN MFlushDCache
#define L1C_DCACHE_INVALID MInvalDCache
#define L1C_DCACHE_CLEAN_INVALID MFlushInvalDCache

uint32_t __irq_save(void);
void __irq_restore(uint32_t flags);
void Exception_Init(void);
void System_Interrupt_Init(void);
void SystemInit(void);

#endif
