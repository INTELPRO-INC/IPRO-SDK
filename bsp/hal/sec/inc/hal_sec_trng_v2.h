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

/* SEC_ENG HAL v2: TRNG. */
#ifndef HAL_SEC_TRNG_V2_H
#define HAL_SEC_TRNG_V2_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TRNG registers of hal_sec_trng_v2_reg_read / _write. */
enum hal_sec_trng_v2_reg {
    HAL_SEC_TRNG_V2_CTRL_0,
    HAL_SEC_TRNG_V2_TEST,
    HAL_SEC_TRNG_V2_CTRL_3,
    HAL_SEC_TRNG_V2_ROSC_C1,
    HAL_SEC_TRNG_V2_PROT_RD,
    HAL_SEC_TRNG_V2_REGS
};

int hal_sec_trng_v2_init(void);

/*
 * Fill buf with TRNG output; HAL_SEC_V2_ERR_HW if the TRNG fails. A
 * failed block (timeout, health-test error, all zero) re-instantiates
 * the TRNG and is retried once.
 */
int hal_sec_trng_v2_read(uint8_t *buf, size_t len);

/* on 0: TRNG off; else on, waiting for the instantiate. */
int hal_sec_trng_v2_enable(int on);
/* One 256-bit generation, no retry; ERR_HW (out zeroed) as read. */
int hal_sec_trng_v2_generate(uint8_t out[32]);
/* r >= HAL_SEC_TRNG_V2_REGS reads 0 and ignores the write. */
uint32_t hal_sec_trng_v2_reg_read(unsigned int r);
void hal_sec_trng_v2_reg_write(unsigned int r, uint32_t v);

#ifdef __cplusplus
}
#endif

#endif
