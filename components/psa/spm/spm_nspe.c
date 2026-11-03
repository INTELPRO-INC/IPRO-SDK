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
 * NSPE launch: validate the image header, measure the image with the
 * SEC_ENG SHA-256, program the PMP, log the SPE stack high-water mark
 * and enter U-mode.
 */
#include <string.h>

#include "hal_sec_hash_v2.h"
#include "spm.h"
#include "spm_harden.h"
#include "spm_log.h"
#include "spm_nspe.h"
#include "spm_periph.h"
#include "spm_pmp.h"
#include "spm_stack.h"
#include "spm_virq.h"
#include "system_soc.h"

#define SPM_HASH_CHUNK 256u
/* mcounteren: CY, TM and IR readable from U */
#define SPM_MCOUNTEREN 0x7u
/*
 * ccm_suen (CCM Mechanism 4.2.4, Table 4.8) = 0: every U-mode CCM
 * operation raises an illegal instruction exception.
 */
#define SPM_CCM_SUEN   0x0u

/* SPE stack bounds of ipro7_psa_spe.ld */
extern uint32_t __StackLimit[];
extern uint32_t __StackTop[];

uint8_t spm_nspe_measurement[32];
uint32_t spm_nspe_version;

static hal_sec_hash_v2_ctx_t spm_hash;

static int spm_header_ok(const struct psa_nspe_header *h)
{
    uint32_t entry_off = h->entry - PSA_NSPE_FLASH_BASE;

    return h->magic == NSPE_HDR_MAGIC &&
           h->hdr_size == sizeof(*h) &&
           h->image_size >= sizeof(*h) &&
           h->image_size <= PSA_NSPE_FLASH_SIZE &&
           entry_off >= sizeof(*h) && entry_off < h->image_size &&
           h->stack_top > PSA_NSPE_OCRAM_BASE &&
           h->stack_top <= PSA_NSPE_OCRAM_BASE + PSA_NSPE_OCRAM_SIZE &&
           (h->stack_top & 15u) == 0;
}

/* SHA-256 of [PSA_NSPE_FLASH_BASE, + size) via an SPE RAM chunk */
static int spm_measure(uint32_t size)
{
    uint8_t chunk[SPM_HASH_CHUNK] __attribute__((aligned(32)));
    size_t len = 0;
    int ret = 0;

    if (hal_sec_hash_v2_setup(&spm_hash, HAL_SEC_HASH_V2_SHA256) != 0) {
        return -1;
    }
    for (uint32_t off = 0; off < size && ret == 0; off += SPM_HASH_CHUNK) {
        uint32_t n = size - off;

        if (n > SPM_HASH_CHUNK) {
            n = SPM_HASH_CHUNK;
        }
        memcpy(chunk, (const void *)(uintptr_t)(PSA_NSPE_FLASH_BASE + off),
               n);
        ret = hal_sec_hash_v2_update(&spm_hash, chunk, n);
    }
    if (ret == 0 &&
        (hal_sec_hash_v2_finish(&spm_hash, spm_nspe_measurement,
                                sizeof(spm_nspe_measurement), &len) != 0 ||
         len != sizeof(spm_nspe_measurement))) {
        ret = -1;
    }
    hal_sec_hash_v2_abort(&spm_hash);
    return ret;
}

void spm_launch_nspe(void)
{
    struct psa_nspe_header h;
    const uint8_t *m = spm_nspe_measurement;

    spm_periph_reset();
    memcpy(&h, (const void *)(uintptr_t)PSA_NSPE_FLASH_BASE, sizeof(h));
    if (!spm_header_ok(&h)) {
        SPM_LOG("no valid NSPE header at %08lx",
                (unsigned long)PSA_NSPE_FLASH_BASE);
        spm_fatal();
    }
    if (spm_measure(h.image_size) != 0) {
        SPM_LOG("NSPE measurement failed");
        spm_fatal();
    }
    spm_nspe_version = h.version;
    SPM_LOG("NSPE v%08lx size %lu entry %08lx sha256 %02x%02x%02x%02x..",
            (unsigned long)h.version, (unsigned long)h.image_size,
            (unsigned long)h.entry, m[0], m[1], m[2], m[3]);

    spm_pmp_apply();
    spm_harden_launch();
    __RV_CSR_WRITE(CSR_MCOUNTEREN, SPM_MCOUNTEREN);
    __RV_CSR_WRITE(CSR_CCM_SUEN, SPM_CCM_SUEN);
    /*
     * medeleg = 0: every exception from U, ecall included, traps to
     * the SPE in M-mode.
     */
    __RV_CSR_WRITE(CSR_MEDELEG, 0);

    memset(&spm_uframe, 0, sizeof(spm_uframe));
    spm_uframe.epc = h.entry;
    spm_uframe.x[SPM_X_SP] = h.stack_top;
    SPM_LOG("stack min free %lu B of %lu",
            (unsigned long)spm_stack_min_free(__StackLimit, __StackTop),
            (unsigned long)((uintptr_t)__StackTop -
                            (uintptr_t)__StackLimit));
    spm_mret_to_u(&spm_uframe);
}

void spm_nspe_reset(void)
{
    spm_virq_reset();
    spm_client_reset();
    SPM_LOG("NSPE reset");
    spm_launch_nspe();
}
