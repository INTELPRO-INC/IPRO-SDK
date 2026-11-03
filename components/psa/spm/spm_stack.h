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
 * SPE stack high-water mark: main first fills the unused stack with
 * SPM_STACK_FILL, spm_stack_min_free() later scans it.
 */
#pragma once

#include "psa_ipro/asm.h"

#define SPM_STACK_FILL PSA_U(0xA5A5A5A5)

#ifndef __ASSEMBLER__
#include <stdint.h>

/* Fill [__StackLimit, sp) with SPM_STACK_FILL (spm_entry.S) */
void spm_stack_fill(void);

/* Bytes from base up to the first word that is not SPM_STACK_FILL */
uint32_t spm_stack_min_free(const uint32_t *base, const uint32_t *top);
#endif
