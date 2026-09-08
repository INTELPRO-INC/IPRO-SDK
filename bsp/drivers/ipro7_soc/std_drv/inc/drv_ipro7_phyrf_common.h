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

#ifndef __DRV_IPRO7_PHYRF_COMMON_H__
#define __DRV_IPRO7_PHYRF_COMMON_H__

#include "ipro7.h"
#include "misc.h"

#ifndef __NOP
#define __NOP() __ASM volatile("nop") /* This implementation generates debug information */
#endif
#ifndef __WFI
#define __WFI() __ASM volatile("wfi") /* This implementation generates debug information */
#endif
#ifndef __WFE
#define __WFE() __ASM volatile("wfe") /* This implementation generates debug information */
#endif
#ifndef __SEV
#define __SEV() __ASM volatile("sev") /* This implementation generates debug information */
#endif
#ifndef __set_MSP
#define __set_MSP(msp) __ASM volatile("add sp, x0, %0" ::"r"(msp))
#endif

// __attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
// {
//     //return __builtin_bswap32(value);
//     uint32_t res = 0;

//     res = (value << 24) | (value >> 24);
//     res &= 0xFF0000FF; /* only for sure */
//     res |= ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000);

//     return res;
// }

// __attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
// {
//     return __builtin_bswap16(value);
// }


/**
  \brief   Enable IRQ Interrupts
  \details Enables IRQ interrupts by setting the IE-bit in the PSR.
           Can only be executed in Privileged modes.
__ALWAYS_STATIC_INLINE void __enable_irq(void)
{
    __ASM volatile("csrs mstatus, 8");
}
 */

/**
  \brief   Disable IRQ Interrupts
  \details Disables IRQ interrupts by clearing the IE-bit in the PSR.
  Can only be executed in Privileged modes.
__ALWAYS_STATIC_INLINE void __disable_irq(void)
{
    __ASM volatile("csrc mstatus, 8");
}
 */

/** @defgroup  COMMON_Public_Constants
 *  @{
 */

/** @defgroup DRIVER_INT_PERIPH
 *  @{
 */
#define IS_INT_PERIPH(INT_PERIPH) ((INT_PERIPH) < IRQn_LAST)

/*@} end of group DRIVER_INT_PERIPH */

/** @defgroup DRIVER_INT_MASK
 *  @{
 */
#define IS_DRV_MASK_TYPE(type) (((type) == MASK) || ((type) == UNMASK))

/*@} end of group COMMON_Public_Constants */

/*@} end of group DRIVER_Public_Macro */
#define IPRO7_MemCpy      arch_memcpy
#define IPRO7_MemSet      arch_memset
#define IPRO7_MemCmp      arch_memcmp
#define IPRO7_MemCpy4     arch_memcpy4
#define IPRO7_MemCpy_Fast arch_memcpy_fast
#define IPRO7_MemSet4     arch_memset4

#define arch_delay_us IPRO7_Delay_US
#define arch_delay_ms IPRO7_Delay_MS

void IPRO7_Delay_US(uint32_t cnt);
void IPRO7_Delay_MS(uint32_t cnt);
#endif
