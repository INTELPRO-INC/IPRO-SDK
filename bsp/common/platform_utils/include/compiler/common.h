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

#ifndef __COMPILER_COMMON_H
#define __COMPILER_COMMON_H

#include "system_soc.h"
#include <stdio.h>

//#define _REG_WRITE_DEBUG_   /* enable to output write register info*/
#define DRV_WORD_FROM_BYTES(p)   ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | (p[0]))

#define DRV_WORD_TO_BYTES(p, val)   \
    {                              \
        p[0] = val & 0xff;         \
        p[1] = (val >> 8) & 0xff;  \
        p[2] = (val >> 16) & 0xff; \
        p[3] = (val >> 24) & 0xff; \
    }

#define getreg8(a)                         (*(volatile uint8_t *)(uintptr_t)(a))
#define putreg8(v, a)                      (*(volatile uint8_t *)(uintptr_t)(a) = (v))
#define getreg16(a)                        (*(volatile uint16_t *)(uintptr_t)(a))
#define putreg16(v, a)                     (*(volatile uint16_t *)(uintptr_t)(a) = (v))
#define getreg32(a)                        (*(volatile uint32_t *)(uintptr_t)(a))
#define putreg32(v, a)                     (*(volatile uint32_t *)(uintptr_t)(a) = (v))

#define readb(c)                           (*(volatile uint8_t *)(uintptr_t)(c))
#define readw(c)                           (*(volatile uint16_t *)(uintptr_t)(c))
#define readl(c)                           (*(volatile uint32_t *)(uintptr_t)(c))
#define writeb(v, c)                       (*(volatile uint8_t *)(uintptr_t)(c) = (v))
#define writew(v, c)                       (*(volatile uint16_t *)(uintptr_t)(c) = (v))
#if !defined(_REG_WRITE_DEBUG_)
#define writel(v, c)                       (*(volatile uint32_t *)(uintptr_t)(c) = (v))
#else
extern uint32_t writel_dump(uint32_t v, uint32_t *c);
#define writel(v, c)                       writel_dump((uint32_t )(v), (uint32_t *)(c))
#endif

#define reg_set_bit(val, bitname)              ((val) | (1U << bitname##_POS))
#define reg_clr_bit(val, bitname)              ((val)&bitname##_UMSK)
#define reg_get_bits(val, bitname)             (((val)&bitname##_MSK) >> bitname##_POS)
#define reg_set_bits(val, bitname, bitval)     (((val)&bitname##_UMSK) | ((uint32_t)(bitval) << bitname##_POS))
#define reg_is_bit_set(val, bitname)           (((val) & (1U << (bitname##_POS))) != 0)

#define DRV_DUMMY_ACT           \
    {                          \
        __ASM volatile("nop"); \
        __ASM volatile("nop"); \
        __ASM volatile("nop"); \
        __ASM volatile("nop"); \
    }

/* Std driver attribute macro*/
#define ATTR_STRINGIFY(x)                   #x
#define ATTR_TOSTRING(x)                    ATTR_STRINGIFY(x)
#define ATTR_UNI_SYMBOL                     __FILE__ ATTR_TOSTRING(__LINE__)
#define ATTR_EALIGN(x)                      __attribute__((aligned(x)))
#define ATTR_USED                           __attribute__((__used__))

#define ATTR_CLOCK_SECTION                  __attribute__((section(".sclock_rlt_code." ATTR_UNI_SYMBOL)))
#define ATTR_CLOCK_CONST_SECTION            __attribute__((section(".sclock_rlt_const." ATTR_UNI_SYMBOL)))
#define ATTR_TCM_SECTION                    __attribute__((section(".tcm_code." ATTR_UNI_SYMBOL)))
#define ATTR_TCM_CONST_SECTION              __attribute__((section(".tcm_const." ATTR_UNI_SYMBOL)))
#define ATTR_DTCM_SECTION                   __attribute__((section(".tcm_data")))
#define ATTR_SYSRAM_SECTION                 __attribute__((section(".system_ram")))

/*
 * PSRAM section macros - conditional based on CONFIG_USE_PSRAM
 * When PSRAM is available: place in dedicated PSRAM sections
 * When PSRAM is NOT available: place in normal OCRAM sections (cleared by startup)
 */
#if defined(CONFIG_USE_PSRAM) && (CONFIG_USE_PSRAM == 1)
#define ATTR_PSRAM_CODE_SECTION             __attribute__((section(".psram_code." ATTR_UNI_SYMBOL)))
#define ATTR_PSRAM_SECTION                  __attribute__((section(".psram_data")))
#define ATTR_PSRAM_BSS_SECTION              __attribute__((section(".psram_bss")))
#define ATTR_PSRAM_NOINIT_SECTION           __attribute__((section(".psram_noinit")))
#else
/* No PSRAM: map to normal sections that are properly initialized by startup code */
#define ATTR_PSRAM_CODE_SECTION             /* empty - use default .text */
#define ATTR_PSRAM_SECTION                  /* empty - use default .data */
#define ATTR_PSRAM_BSS_SECTION              /* empty - use default .bss */
#define ATTR_PSRAM_NOINIT_SECTION           __attribute__((section(".noinit_data")))
#endif

/* OCRAM section - explicitly place data in fast OCRAM when PSRAM is default */
/* Use for frequently accessed small data that needs fast OCRAM access */
#define ATTR_OCRAM_SECTION                  __attribute__((section(".ocram_data")))
#define ATTR_OCRAM_BSS_SECTION              __attribute__((section(".ocram_bss")))

/* NOINIT section - data preserved across resets, not initialized by startup */
#define ATTR_NOINIT_DATA_SECTION            __attribute__((section(".noinit_data")))

#define ATTR_NOCACHE_RAM_SECTION            __attribute__((section(".nocache_ram")))
#define ATTR_NOCACHE_NOINIT_RAM_SECTION     __attribute__((section(".nocache_noinit_ram")))
#define ATTR_HBN_RAM_SECTION                __attribute__((section(".hbn_ram_code." ATTR_UNI_SYMBOL)))
#define ATTR_HBN_RAM_CONST_SECTION          __attribute__((section(".hbn_ram_data." ATTR_UNI_SYMBOL)))
#define ATTR_FALLTHROUGH()                  __attribute__((fallthrough))

/* OCRAM No-Retention section - NOT preserved during low power mode */
/* Use for temporary buffers that don't need LP retention (e.g., DMA, frame buffers) */
#define ATTR_OCRAM_NORET                    __attribute__((section(".ocram_noret")))
#define ATTR_OCRAM_NORET_NOINIT             __attribute__((section(".ocram_noret_noinit")))

#endif // __COMPILER_COMMON_H
