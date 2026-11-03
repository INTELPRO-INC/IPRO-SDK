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
 * Host lfs_xip_init over a RAM NOR flash shared across fork(): the
 * PSM partition is a window of blocks in it, chosen per boot.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#define LFS_RAM_BLOCK 4096u

/* Maps n blocks of erased flash (0xff); 0 on success */
int lfs_ram_init(size_t n);

/* Flash byte at block b, offset 0 */
uint8_t *lfs_ram_block(size_t b);

/* PSM partition = blocks [first, first + count) of the flash */
void lfs_ram_window(size_t first, size_t count);

/* Fills blocks [first, first + count) with byte v */
void lfs_ram_fill(size_t first, size_t count, uint8_t v);
