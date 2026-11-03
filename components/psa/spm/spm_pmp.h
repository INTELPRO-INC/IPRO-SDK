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
 * PMP view of the U-mode NSPE: NAPOT entries 0..5, L=0, so M-mode
 * (the SPE) is never restricted by them. TEE parts hold the same
 * entries in sPMP 0..5.
 */
#pragma once

#include <stdint.h>

/* pmpaddr of a naturally aligned power-of-two region (size >= 8) */
uint32_t spm_pmp_napot(uint32_t base, uint32_t size);

/* sPMP cfg byte of a PMP cfg byte: A | X | W | R, bits 7:5 clear */
uint8_t spm_spmp_cfg(uint8_t pmpcfg);

/*
 * PMP 0..5 = NSPE memory, then spm_periph windows; 6..7 off. With
 * mcfg_info.TEE: the same in sPMP 0..5, smpuswitch0 = 0x3F. Fence.
 */
void spm_pmp_apply(void);
