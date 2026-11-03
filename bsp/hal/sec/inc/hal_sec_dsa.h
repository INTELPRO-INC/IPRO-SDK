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

#ifndef __HAL_SEC_DSA__H__
#define __HAL_SEC_DSA__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"

typedef struct sec_dsa_crt_cfg_tag {
    uint32_t *dP;
    uint32_t *dQ;
    uint32_t *qInv;
    uint32_t *p;
    uint32_t *invR_p;
    uint32_t *primeN_p;
    uint32_t *q;
    uint32_t *invR_q;
    uint32_t *primeN_q;
} sec_dsa_crt_cfg_t;

typedef struct
{
    uint32_t size;
    uint32_t crtSize;
    uint32_t *n;
    uint32_t *e;
    uint32_t *d;
    sec_dsa_crt_cfg_t crtCfg;
} sec_dsa_handle_t;

int sec_dsa_init(sec_dsa_handle_t *handle, uint32_t size);
int sec_dsa_mexp_binary(uint32_t size, const uint32_t *a, const uint32_t *b, const uint32_t *c, uint32_t *r);
int sec_dsa_mexp_mont(uint32_t size, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *invR_c, uint32_t *primeN_c, uint32_t *r);
int sec_dsa_decrypt_crt(uint32_t size, uint32_t *c, sec_dsa_crt_cfg_t *crtCfg, uint32_t *d, uint32_t *r);
int sec_dsa_sign(sec_dsa_handle_t *handle, const uint32_t *hash, uint32_t hashLenInWord, uint32_t *s);
int sec_dsa_verify(sec_dsa_handle_t *handle, const uint32_t *hash, uint32_t hashLenInWord, const uint32_t *s);
#ifdef __cplusplus
}
#endif
#endif