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

#include <stdint.h>
#include "ipro6.h"

#define __STARTUP_CLEAR_BSS 1

/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __itcm_load_addr;
extern uint32_t __dtcm_load_addr;
extern uint32_t __system_ram_load_addr;
extern uint32_t __ram_load_addr;

extern uint32_t __text_code_start__;
extern uint32_t __text_code_end__;
extern uint32_t __tcm_code_start__;
extern uint32_t __tcm_code_end__;
extern uint32_t __tcm_data_start__;
extern uint32_t __tcm_data_end__;
extern uint32_t __system_ram_data_start__;
extern uint32_t __system_ram_data_end__;
extern uint32_t __ram_data_start__;
extern uint32_t __ram_data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __noinit_data_start__;
extern uint32_t __noinit_data_end__;

extern uint32_t __StackTop;
extern uint32_t __StackLimit;
extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

//extern uint32_t __copy_table_start__;
//extern uint32_t __copy_table_end__;
//extern uint32_t __zero_table_start__;
//extern uint32_t __zero_table_end__;

void start_load(void)
{
#if 0
    uint32_t *pSrc, *pDest;
    uint32_t *pTable __attribute__((unused));

    /* Copy ITCM code */
    pSrc = &__itcm_load_addr;
    pDest = &__tcm_code_start__;

    for (; pDest < &__tcm_code_end__;) {
        *pDest++ = *pSrc++;
    }

    /* Copy DTCM code */
    pSrc = &__dtcm_load_addr;
    pDest = &__tcm_data_start__;

    for (; pDest < &__tcm_data_end__;) {
        *pDest++ = *pSrc++;
    }

    /* BF Add system RAM data copy */
    pSrc = &__system_ram_load_addr;
    pDest = &__system_ram_data_start__;

    for (; pDest < &__system_ram_data_end__;) {
        *pDest++ = *pSrc++;
    }

    /* BF Add OCARAM data copy */
    pSrc = &__ram_load_addr;
    pDest = &__ram_data_start__;

    for (; pDest < &__ram_data_end__;) {
        *pDest++ = *pSrc++;
    }

#ifdef __STARTUP_CLEAR_BSS
    /*  Single BSS section scheme.
     *
     *  The BSS section is specified by following symbols
     *    __bss_start__: start of the BSS section.
     *    __bss_end__: end of the BSS section.
     *
     *  Both addresses must be aligned to 4 bytes boundary.
     */
    pDest = &__bss_start__;

    for (; pDest < &__bss_end__;) {
        *pDest++ = 0ul;
    }

#endif
#endif
}
