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
 * Launch hardening (spm_harden.h): S-mode left unreachable and no
 * vectored source before U runs; PSM layout checked before ITS.
 */
#include "spm_harden.h"

#include "psa_ipro/memmap.h"

#define SPM_PSM_ALIGN 0x1000u

int spm_irq_ok(uint8_t ie, uint8_t attr)
{
    if ((attr & SPM_IRQ_ATTR_MODE) != SPM_IRQ_ATTR_MODE) {
        return 0;
    }
    return !(ie & 1u) || !(attr & SPM_IRQ_ATTR_SHV);
}

/* 1 when [a, a + an) and [b, b + bn) share a byte; ends do not wrap */
static int spm_overlap(uint32_t a, uint32_t an, uint32_t b, uint32_t bn)
{
    return an != 0 && bn != 0 && a < b + bn && b < a + an;
}

int spm_psm_layout_check(const struct spm_psm_layout *l)
{
    uint32_t low = 0xFFFFFFFFu;
    uint32_t alias;

    if (l->psm_size == 0 || (l->psm_addr | l->psm_size) % SPM_PSM_ALIGN ||
        l->psm_addr > 0xFFFFFFFFu - l->psm_size) {
        return SPM_PSM_SIZE;
    }
    if (l->psm_addr < l->img_off ||
        l->psm_addr - l->img_off > 0xFFFFFFFFu - PSA_XIP_BASE - l->psm_size) {
        return SPM_PSM_NO_ALIAS;
    }
    alias = PSA_XIP_BASE + (l->psm_addr - l->img_off);
    if (spm_overlap(alias, l->psm_size, PSA_SPE_FLASH_BASE,
                    PSA_NSPE_FLASH_BASE + PSA_NSPE_FLASH_SIZE -
                        PSA_SPE_FLASH_BASE)) {
        return SPM_PSM_FW_WINDOW;
    }
    for (unsigned int i = 0; i < 2; i++) {
        if (l->fw_size[i] == 0) {
            continue;
        }
        if (l->fw_addr[i] > 0xFFFFFFFFu - l->fw_size[i] ||
            spm_overlap(l->psm_addr, l->psm_size, l->fw_addr[i],
                        l->fw_size[i])) {
            return SPM_PSM_FW_ENTRY;
        }
        if (l->fw_addr[i] < low) {
            low = l->fw_addr[i];
        }
    }
    if (low == 0xFFFFFFFFu) {
        return SPM_PSM_NO_ENTRY;
    }
    return l->psm_addr < low ? SPM_PSM_BOOT_AREA : SPM_PSM_OK;
}

const char *spm_psm_reason(int r)
{
    static const char *const why[] = {
        [SPM_PSM_OK] = "ok",
        [SPM_PSM_NO_ENTRY] = "no PSM or FW entry",
        [SPM_PSM_SIZE] = "size or alignment",
        [SPM_PSM_NO_ALIAS] = "no XIP alias",
        [SPM_PSM_FW_WINDOW] = "alias in the SPE/NSPE flash window",
        [SPM_PSM_FW_ENTRY] = "overlaps an FW entry",
        [SPM_PSM_BOOT_AREA] = "below the FW entries",
    };

    if (r < 0 || (unsigned int)r >= sizeof(why) / sizeof(why[0])) {
        return "?";
    }
    return why[r];
}

#ifndef SPM_HOST
#include "drv_sf_ctrl.h"
#include "hal_boot2.h"
#include "spm_log.h"
#include "system_soc.h"

int spm_psm_layout_ok(void)
{
    struct spm_psm_layout l = { 0 };
    int active = 0;
    int r = SPM_PSM_NO_ENTRY;

    if (hal_boot2_partition_addr_active(PSA_ITS_PARTITION, &l.psm_addr,
                                        &l.psm_size) == 0 &&
        hal_boot2_partition_addr("FW", &l.fw_addr[0], &l.fw_addr[1],
                                 &l.fw_size[0], &l.fw_size[1],
                                 &active) == 0) {
        l.img_off = SF_Ctrl_Get_Flash_Image_Offset(0, 0);
        r = spm_psm_layout_check(&l);
    }
    SPM_LOG("PSM layout %08lx+%lx img %08lx: %s",
            (unsigned long)l.psm_addr, (unsigned long)l.psm_size,
            (unsigned long)l.img_off, spm_psm_reason(r));
    return r == SPM_PSM_OK ? 0 : -1;
}

void spm_harden_launch(void)
{
    uint32_t n = ECLIC_GetInfoNum();
    uint32_t moved = 0;
    uint32_t on = 0;

    __RV_CSR_WRITE(CSR_MIDELEG, 0);
    /* an S-mode fetch at stvec faults: SPE code has no PMP entry */
    __RV_CSR_WRITE(CSR_STVEC, (uintptr_t)spm_fatal);
    for (uint32_t i = 0; i < n; i++) {
        uint8_t attr = ECLIC->CTRL[i].INTATTR;
        uint8_t ie = ECLIC->CTRL[i].INTIE;

        if ((attr & SPM_IRQ_ATTR_MODE) != SPM_IRQ_ATTR_MODE) {
            ECLIC->CTRL[i].INTATTR = (uint8_t)(attr | SPM_IRQ_ATTR_MODE);
            attr = ECLIC->CTRL[i].INTATTR;
            moved++;
        }
        on += ie & 1u;
        if (!spm_irq_ok(ie, attr)) {
            SPM_LOG("harden: IRQ %lu attr %02x ie %u refused",
                    (unsigned long)i, attr, ie & 1u);
            spm_fatal();
        }
    }
    SPM_LOG("harden: mideleg %lx stvec %08lx, %lu IRQs, %lu on, "
            "%lu set to M",
            (unsigned long)__RV_CSR_READ(CSR_MIDELEG),
            (unsigned long)__RV_CSR_READ(CSR_STVEC), (unsigned long)n,
            (unsigned long)on, (unsigned long)moved);
}
#endif
