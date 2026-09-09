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

#include "drv_common_s.h"
#include <string.h>
#include "platform_common.h"
#include "drv_glb.h"
#include "drv_dma.h"
#include "drv_i2s.h"
#include "drv_i2c.h"
#include "drv_uart.h"
#include "drv_clock.h"

#ifndef IPRO_USE_ROM_DRIVER
static inline uint8_t ATTR_TCM_SECTION is_mem_in_ram(uint32_t addr, size_t size)
{
    /* IPRO6 all RAM: OCRAM (320K) + WRAM (160K) = 480K total
     * Non-cacheable: 0x21000000 + 480K
     * Cacheable:     0x11000000 + 480K
     */
    if ((((IPRO6_ALLRAM_BASE <= addr) && ((addr + size) <= IPRO6_ALLRAM_END)) ||
         ((IPRO6_ALLRAM_CACHEABLE_BASE <= addr) && ((addr + size) <= IPRO6_ALLRAM_CACHEABLE_END)))) {
        return 1;
    }

    return 0;
}
#endif

#ifndef IPRO_USE_ROM_DRIVER
__WEAK
errno_t ATTR_TCM_SECTION memcpy_s(void *dst, size_t dstSize, const void *src, size_t count)
{
    /* param NULL */
    if((NULL==dst)||(NULL==src)){
        return PARAM_ERROR;
    }

    /* memory overlap */
    if(!((((uint32_t)src + count)<(uint32_t)dst)||((uint32_t)src>((uint32_t)dst + dstSize)))){
        return BUFFER_OVERLAP;
    }

    /* size mismatch */
    if(count > dstSize){
        return SIZE_MISMATCHING;
    }

    /* dst overflow */
    if(!is_mem_in_ram((uint32_t)dst, dstSize)){
        return DST_OVERFLOW;
    }

    memcpy(dst, src, count);

    return COPY_SUCCESS;
}
#endif

