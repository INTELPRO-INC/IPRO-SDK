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
 * NSPE address checks. Pure C: also built on the host with
 * -DSPM_HOST.
 */
#include "spm.h"

#define SPM_RGN_WRITE 1u
#define SPM_RGN_ENTRY 2u

struct spm_region {
    uint32_t base;
    uint32_t size;
    uint32_t flags;
};

static const struct spm_region spm_nspe_regions[] = {
    { PSA_NSPE_FLASH_BASE,  PSA_NSPE_FLASH_SIZE,  SPM_RGN_ENTRY },
    { PSA_NSPE_OCRAM_BASE,  PSA_NSPE_OCRAM_SIZE,
      SPM_RGN_WRITE | SPM_RGN_ENTRY },
    { PSA_NSPE_OCRAM2_BASE, PSA_NSPE_OCRAM2_SIZE, SPM_RGN_WRITE },
    { PSA_NSPE_PSRAM_BASE,  PSA_NSPE_PSRAM_SIZE,  SPM_RGN_WRITE },
};

#define SPM_REGION_COUNT \
    (sizeof(spm_nspe_regions) / sizeof(spm_nspe_regions[0]))

/* 1 when [base, base + len) is in one region that has all of need */
static int spm_nspe_check(uint32_t base, uint32_t len, uint32_t need)
{
    for (size_t i = 0; i < SPM_REGION_COUNT; i++) {
        const struct spm_region *r = &spm_nspe_regions[i];
        /* base below r->base wraps to a large offset */
        uint32_t off = base - r->base;

        if (off >= r->size) {
            continue;
        }
        return (r->flags & need) == need && len <= r->size - off;
    }
    return 0;
}

int spm_nspe_range_ok(uint32_t base, uint32_t len, int write)
{
    return spm_nspe_check(base, len, write ? SPM_RGN_WRITE : 0u);
}

int spm_nspe_entry_ok(uint32_t entry)
{
    return spm_nspe_check(entry, 4u, SPM_RGN_ENTRY);
}
