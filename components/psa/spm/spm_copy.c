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
 * Copies across the SPE/NSPE boundary: range check, then memcpy.
 */
#include <string.h>

#include "spm.h"

int spm_copy_in(void *dst, uint32_t nspe_src, uint32_t len)
{
    if (!spm_nspe_range_ok(nspe_src, len, 0)) {
        return -1;
    }
    memcpy(dst, (const void *)(uintptr_t)nspe_src, len);
    return 0;
}

int spm_copy_out(uint32_t nspe_dst, const void *src, uint32_t len)
{
    if (!spm_nspe_range_ok(nspe_dst, len, 1)) {
        return -1;
    }
    memcpy((void *)(uintptr_t)nspe_dst, src, len);
    return 0;
}
