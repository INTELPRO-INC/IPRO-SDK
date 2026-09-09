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

#ifndef __DRV_COMMON_H_
#define __DRV_COMMON_H_

/**
 * IPRO6 driver common header
 * Based on IPRO7's version since IPRO6 uses IPRO7's CPU
 */

#include "misc.h"
#include "platform_common.h"
#include "system_soc.h"

/* IPRO6LE compatibility: ARCH_Mem* maps to arch_mem* from misc.h */
#define ARCH_MemCpy4     arch_memcpy4
#define ARCH_MemCpy      arch_memcpy
#define ARCH_MemCpy_Fast arch_memcpy_fast
#define ARCH_MemSet      arch_memset
#define ARCH_MemSet4     arch_memset4

#ifndef __NOP
#define __NOP() __ASM volatile("nop")
#endif
#ifndef __WFI
#define __WFI() __ASM volatile("wfi")
#endif
#ifndef __WFE
#define __WFE() __ASM volatile("wfe")
#endif
#ifndef __SEV
#define __SEV() __ASM volatile("sev")
#endif
#ifndef __set_MSP
#define __set_MSP(msp) __ASM volatile("add sp, x0, %0" ::"r"(msp))
#endif

void arch_delay_us(uint32_t cnt);
void arch_delay_ms(uint32_t cnt);

#endif // __DRV_COMMON_H_
