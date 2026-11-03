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

/* SEC_ENG HAL v2 internals: register map, I/O seam, job runner. */
#ifndef HAL_SEC_V2_PRIV_H
#define HAL_SEC_V2_PRIV_H

#include <stddef.h>
#include <stdint.h>
#include "hal_sec_common_v2.h"

#ifdef CONFIG_SEC_ENG_HAL_V2_CHUNK
#define SEC_V2_CHUNK CONFIG_SEC_ENG_HAL_V2_CHUNK
#else
#define SEC_V2_CHUNK 256
#endif

/* Largest job on data the engine reads in place (sec_v2_direct). */
#define SEC_V2_DIRECT_MAX 16384u

#if SEC_V2_CHUNK % 128 != 0 || SEC_V2_CHUNK < 128 || SEC_V2_CHUNK > 4096
#error "SEC_ENG_HAL_V2_CHUNK must be a multiple of 128 in [128, 4096]"
#endif

#define SEC_V2_SHA_CTRL    0x000u
#define SEC_V2_SHA_LINK    0x050u
#define SEC_V2_AES_CTRL    0x100u
#define SEC_V2_AES_KEY_SEL0 0x140u
#define SEC_V2_AES_KEY_SEL1 0x144u
#define SEC_V2_AES_ENDIAN  0x148u
#define SEC_V2_AES_SBOOT   0x14Cu
#define SEC_V2_AES_LINK    0x150u
#define SEC_V2_TRNG_CTRL   0x200u
#define SEC_V2_GMAC_CTRL   0x500u
#define SEC_V2_GMAC_LINK   0x504u
#define SEC_V2_PROT(b)     (0x0FCu + 0x100u * (b))
#define SEC_V2_PROT_RD     0xF00u

#define SEC_V2_BUSY        (1u << 0)
#define SEC_V2_TRIG        (1u << 1)
#define SEC_V2_LINK_MODE   (1u << 15)
#define SEC_V2_TRNG_HT_ERR (1u << 4)

/* Busy-wait bound in register reads (about 30 ms at 192 MHz). */
#define SEC_V2_SPIN        (1u << 20)

uint32_t sec_v2_rd(uint32_t off);
void sec_v2_wr(uint32_t off, uint32_t v);

/* Engine (DMA) address of p. */
uint32_t sec_v2_dma(const void *p);

/*
 * D-cache write-back and invalidate of the lines of [p, p + n); a
 * no-op without a D-cache.
 */
void sec_v2_flush(const void *p, size_t n);

/* 1 when the engine may read the n bytes at p in place. */
int sec_v2_direct(const void *p, size_t n);

/*
 * Bytes from p, at most n, that the engine may write in place: whole
 * D-cache lines only, so no other data shares a line with the output.
 */
size_t sec_v2_direct_out(const void *p, size_t n);

int sec_v2_lock(unsigned int block);
void sec_v2_unlock(unsigned int block);

/*
 * One LINK job: wait idle, set the link address, set/clear ctrl bits,
 * trigger, wait idle. Timeout -> hal_sec_v2_recover().
 */
int sec_v2_job(uint32_t ctrl, uint32_t link, uint32_t set, uint32_t clr,
               uint32_t cfg_dma);

#endif
