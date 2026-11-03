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
 * SPE stack high-water mark, see spm_stack.h.
 */
#include "spm_stack.h"

uint32_t spm_stack_min_free(const uint32_t *base, const uint32_t *top)
{
    const uint32_t *p = base;

    while (p < top && *p == SPM_STACK_FILL) {
        p++;
    }
    return (uint32_t)((uintptr_t)p - (uintptr_t)base);
}
