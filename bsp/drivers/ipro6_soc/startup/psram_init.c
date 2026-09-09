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
 * PSRAM Early Initialization
 * 
 * This file provides PSRAM initialization called during system startup
 */

#include <stdint.h>
#include "hal_boot2.h"
#include "system_soc.h"

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)

/* External symbols from linker script */
extern uint32_t _text_psram_lma;
extern uint32_t _text_psram_vma;
extern uint32_t _etext_psram;
extern uint32_t _data_psram_lma;
extern uint32_t _data_psram_vma;
extern uint32_t _edata_psram;
extern uint32_t _bss_psram_start;
extern uint32_t _bss_psram_end;
extern uint32_t __heap_psram_size;

/* Global flag to indicate PSRAM availability */
uint32_t __psram_available = 0;

/**
 * @brief Initialize PSRAM and load PSRAM sections
 * 
 * This function is called from _premain_init() after SysTimer initialization.
 * 
 * Steps:
 * 1. Calls hardware PSRAM initialization (hal_boot2_psram)
 * 2. If successful, loads .text_psram section from Flash to PSRAM (CODE)
 * 3. Loads .data_psram section from Flash to PSRAM (DATA)
 * 4. Clears .bss_psram section
 * 5. Flushes D-Cache and invalidates I-Cache for code section
 * 6. Sets __psram_available flag
 * 
 * If initialization fails, sets __psram_available to 0 and clears heap size
 * to prevent system from using PSRAM.
 */
void psram_init_and_load(void)
{
    uint32_t ret;
    uint32_t *src, *dst, *end;
    uint32_t code_start, code_size;

    /* Call PSRAM hardware initialization */
    ret = hal_boot2_psram();

    if (ret == 0) {
        /* PSRAM initialization failed */
        __psram_available = 0;
        __heap_psram_size = 0;
        return;
    }

    /* PSRAM initialization successful - load sections */

    /* Load .text_psram section (LMA -> VMA) - CODE MUST BE LOADED FIRST */
    src = &_text_psram_lma;
    dst = &_text_psram_vma;
    end = &_etext_psram;

    /* Save code section info for cache operations */
    code_start = (uint32_t)&_text_psram_vma;
    code_size = (uint32_t)end - (uint32_t)dst;

    /* Only copy if LMA != VMA and valid range */
    if (src != dst && dst < end) {
        while (dst < end) {
            *dst++ = *src++;
        }
    }

    /* Load .data_psram section (LMA -> VMA) */
    src = &_data_psram_lma;
    dst = &_data_psram_vma;
    end = &_edata_psram;

    /* Only copy if LMA != VMA and valid range */
    if (src != dst && dst < end) {
        while (dst < end) {
            *dst++ = *src++;
        }
    }

    /* Clear .bss_psram section */
    dst = &_bss_psram_start;
    end = &_bss_psram_end;

    while (dst < end) {
        *dst++ = 0;
    }

    /*
     * Cache synchronization for code section:
     * 1. Clean D-Cache to ensure data is written to PSRAM
     * 2. Invalidate I-Cache to ensure CPU fetches new code from PSRAM
     * 3. Execute fence.i to synchronize instruction stream
     */
    if (code_size > 0) {
        /* Clean D-Cache - write cached data to PSRAM */
        L1C_DCACHE_CLEAN_RANGE(code_start, code_size);

        /* Invalidate I-Cache - force re-fetch from PSRAM */
        MInvalICache();

        /* Fence.i - synchronize instruction stream */
        __FENCE_I();
    }

    /* Set PSRAM available flag */
    __psram_available = 1;
}

#endif /* CONFIG_LINK_FLASH && CONFIG_USE_PSRAM */
