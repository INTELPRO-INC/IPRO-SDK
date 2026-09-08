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

/*
 * cmsis_compiler.h - minimal CMSIS-Core compiler shim for RISC-V / GCC.
 *
 * DAP.h includes this file for __STATIC_FORCEINLINE, __ASM, __NOP, etc.
 * The real CMSIS-Core version targets Cortex-M; this shim covers only the
 * symbols actually used by DAP.h on a RISC-V/GCC toolchain.
 *
 * Defining __CC_ARM redirects DAP.h's PIN_DELAY_SLOW to the portable C loop
 * (avoids the Thumb ".syntax unified" inline-asm that won't assemble on RV32).
 * __CC_ARM is not tested anywhere else in the CMSIS-DAP source tree.
 */
#ifndef CMSIS_COMPILER_H
#define CMSIS_COMPILER_H

/* Force the portable C loop path in DAP.h's PIN_DELAY_SLOW. */
#ifndef __CC_ARM
#define __CC_ARM
#endif

#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif

#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE static inline __attribute__((always_inline))
#endif

#ifndef __ASM
#define __ASM __asm__
#endif

/* __NOP() is called function-style in PIN_DELAY_FAST. */
#ifndef __NOP
#define __NOP() __asm__ volatile("nop")
#endif

/* __WEAK: used in DAP.c for the default DAP_ProcessVendorCommand. */
#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif

#endif /* CMSIS_COMPILER_H */
