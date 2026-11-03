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
 * SPE heap: first-fit allocator over a static arena in SPE RAM,
 * the mbedtls calloc/free of the PSA Crypto core.
 */
#pragma once

#include <stddef.h>

#define SPM_HEAP_SIZE (16u * 1024u)

/* 8-byte aligned block, NULL when no free block fits */
void *spm_malloc(size_t n);

/* spm_malloc of n * size zeroed bytes; NULL on overflow */
void *spm_calloc(size_t n, size_t size);

/* Wipe and release; NULL, foreign or freed pointers are ignored */
void spm_free(void *p);

/* Bytes in use and the high-water mark, block headers included */
void spm_heap_stats(size_t *used, size_t *peak);
