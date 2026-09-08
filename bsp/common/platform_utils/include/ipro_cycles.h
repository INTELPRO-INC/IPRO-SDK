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

/**
 * @file ipro_cycles.h
 * @brief 64-bit RISC-V cycle counter reads.
 *
 * mcycle is 32 bits, which at 192 MHz wraps every 2^32/192e6 = 22.4 seconds.
 * A benchmark that reads it as uint32_t silently reports (true mod 2^32) for
 * anything longer - and the result looks plausible rather than obviously
 * broken, which is the dangerous part. A 69-second run once came back as
 * 312,080,793 cycles, i.e. 0.68 cycles/MAC, "faster" than the theoretical
 * peak of the SIMD path it was being compared against.
 *
 * mcycleh holds the upper 32 bits; the read loop below is the standard
 * hi/lo/hi retry that tolerates a carry landing between the two reads.
 */

#ifndef __IPRO_CYCLES_H__
#define __IPRO_CYCLES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Full 64-bit cycle count. Safe across any run length. */
static inline uint64_t ipro_cycles64(void)
{
    uint32_t hi, lo, hi2;
    do {
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi));
        __asm__ volatile("csrr %0, mcycle"  : "=r"(lo));
        __asm__ volatile("csrr %0, mcycleh" : "=r"(hi2));
    } while (hi != hi2);
    return ((uint64_t)hi << 32) | (uint64_t)lo;
}

/** Milliseconds from a cycle delta, given the core clock in MHz. */
static inline uint32_t ipro_cycles_to_ms(uint64_t cycles, uint32_t mhz)
{
    return (uint32_t)(cycles / ((uint64_t)mhz * 1000ULL));
}

#ifdef __cplusplus
}
#endif

#endif /* __IPRO_CYCLES_H__ */
