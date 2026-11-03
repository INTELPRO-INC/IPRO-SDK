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
 * NSPE context: the saved U-mode frame, the trap path from U and
 * the return path to U, launch and reset of the NSPE image.
 */
#pragma once

#include <stdint.h>

#include "psa_ipro/abi.h"

/* struct psa_nspe_frame x[] index of register xN is N - 1 */
#define SPM_X_SP 1u
#define SPM_X_A0 9u
#define SPM_X_A1 10u
#define SPM_X_A7 16u

/* Live U-mode context, in SPE RAM; filled by spm_entry.S. */
extern struct psa_nspe_frame spm_uframe;

/* SHA-256 of the NSPE image, measured at every launch. */
extern uint8_t spm_nspe_measurement[32];

/* psa_nspe_header.version of the launched image, 0xMMmmpppp */
extern uint32_t spm_nspe_version;

/* Trap taken from U; f is &spm_uframe. */
void spm_trap_c(uint32_t mcause, struct psa_nspe_frame *f)
    __attribute__((noreturn));

/* Trap taken from M; regs is the SAVE_CONTEXT slot layout. */
void spm_mtrap_c(uint32_t mcause, uint32_t *regs);

/* Deliver the lowest pending vIRQ when unmasked, then resume U. */
void spm_return_to_nspe(struct psa_nspe_frame *f)
    __attribute__((noreturn));

/* Load f, MPP=U, MPIE=1, FS=Initial, mscratch=SPE stack top, mret. */
void spm_mret_to_u(struct psa_nspe_frame *f) __attribute__((noreturn));

/* Drop NSPE IRQ and service client state, launch the image again. */
void spm_nspe_reset(void) __attribute__((noreturn));
