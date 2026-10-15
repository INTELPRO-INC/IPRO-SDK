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
 * @file llm_simd.h
 * @brief SMAQA (four int8 products added to an int32, the IPRO7 DSP extension) and aligned word
 *        loads, with portable C versions off IPRO7 so the host tests compute the board's numbers.
 */
#ifndef LLM_SIMD_H
#define LLM_SIMD_H

#include <stdint.h>
#include <string.h>

#include "llm_internal.h"

#if defined(__riscv_xxldsp)
#include "ipro7.h"              /* defines __DSP_PRESENT */
#include "core_feature_dsp.h"   /* __RV_SMAQA, __RV_PKBB16 */
#define LLM_OCRAM __attribute__((section(".ocram_bss")))
#define SMAQA(acc, a, b)  ((int32_t)__RV_SMAQA((acc), (a), (b)))
#define PKBB16(hi, lo)    ((uint32_t)__RV_PKBB16((hi), (lo)))
#else
#define LLM_OCRAM
static inline int32_t SMAQA(int32_t acc, uint32_t a, uint32_t b)
{
    for (int i = 0; i < 4; i++) {
        acc += (int32_t)(int8_t)(a >> (8 * i)) * (int32_t)(int8_t)(b >> (8 * i));
    }
    return acc;
}
#define PKBB16(hi, lo)    ((uint32_t)(hi) << 16 | (uint32_t)(lo))
#endif

/* 32-bit / 16-bit loads of packed indices, int8 activations and norms. All
 * are naturally aligned (tensor data is 16-aligned in the blob, rows are whole
 * words, llm_act_t.q is aligned); on IPRO7 a plain lw / lhu, as k_cq.c,
 * instead of a byte-wise memcpy. */
#if defined(__riscv_xxldsp)
typedef uint32_t __attribute__((may_alias)) u32a_t;
typedef uint16_t __attribute__((may_alias)) u16a_t;
static inline uint32_t rd32(const void *p)
{
    return *(const u32a_t *)p;
}
static inline uint16_t rd16(const void *p)
{
    return *(const u16a_t *)p;
}
#else
static inline uint32_t rd32(const void *p)
{
    uint32_t v;
    memcpy(&v, p, sizeof v);
    return v;
}
static inline uint16_t rd16(const void *p)
{
    return llm_rd16(p);
}
#endif

#endif /* LLM_SIMD_H */
