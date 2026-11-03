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
 * U-mode ecall into the PSA SPE: a7 = function id (abi.h), a0..a5
 * arguments, result in a0. -DSPM_HOST links a host stub instead.
 */
#pragma once

#include <stdint.h>

#include "psa_ipro/abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SPM_HOST
int nspe_ecall(uint32_t fn, uint32_t a0, uint32_t a1, uint32_t a2,
               uint32_t a3, uint32_t a4, uint32_t a5);
#else
/* Registers other than a0 come back unchanged. */
static inline int nspe_ecall(uint32_t fn, uint32_t a0, uint32_t a1,
                             uint32_t a2, uint32_t a3, uint32_t a4,
                             uint32_t a5)
{
    register uint32_t r0 __asm__("a0") = a0;
    register uint32_t r1 __asm__("a1") = a1;
    register uint32_t r2 __asm__("a2") = a2;
    register uint32_t r3 __asm__("a3") = a3;
    register uint32_t r4 __asm__("a4") = a4;
    register uint32_t r5 __asm__("a5") = a5;
    register uint32_t r7 __asm__("a7") = fn;

    __asm__ volatile("ecall"
                     : "+r"(r0)
                     : "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5),
                       "r"(r7)
                     : "memory");
    return (int)r0;
}
#endif

#ifdef __cplusplus
}
#endif
