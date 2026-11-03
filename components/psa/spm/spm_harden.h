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
 * Launch hardening: trap delegation and ECLIC state before the first
 * mret to U; the PSM flash layout before ITS mounts.
 */
#pragma once

#include <stdint.h>

/* ECLIC clicintattr: shv bit 0, mode bits 7:6 (3 = M-mode) */
#define SPM_IRQ_ATTR_SHV    0x01u
#define SPM_IRQ_ATTR_MODE   0xC0u

/* 1 when an IRQ with enable ie and clicintattr attr may stay */
int spm_irq_ok(uint8_t ie, uint8_t attr);

/* Flash ranges the PSM check reads from the partition table */
struct spm_psm_layout {
    uint32_t psm_addr;
    uint32_t psm_size;
    /* flash address that boot2 maps to PSA_XIP_BASE */
    uint32_t img_off;
    /* both FW entries; size 0 = no entry */
    uint32_t fw_addr[2];
    uint32_t fw_size[2];
};

enum {
    SPM_PSM_OK = 0,
    SPM_PSM_NO_ENTRY,
    SPM_PSM_SIZE,
    SPM_PSM_NO_ALIAS,
    SPM_PSM_FW_WINDOW,
    SPM_PSM_FW_ENTRY,
    SPM_PSM_BOOT_AREA,
};

/*
 * SPM_PSM_OK when the PSM is 4 KB aligned, its XIP alias lies above
 * PSA_XIP_BASE and outside the SPE and NSPE flash windows, and its
 * flash range misses both FW entries and everything below them.
 */
int spm_psm_layout_check(const struct spm_psm_layout *l);
const char *spm_psm_reason(int r);

/* Partition table check, logs one line; 0 when the PSM may mount */
int spm_psm_layout_ok(void);

/*
 * mideleg = 0, stvec into SPE memory, every ECLIC source M-mode;
 * fatal when an enabled source is vectored.
 */
void spm_harden_launch(void);
