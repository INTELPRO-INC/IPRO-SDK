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

#ifndef _IPRO_DSA_H
#define _IPRO_DSA_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "platform_common.h"

typedef struct tag_ipro_dsa_crt_cfg
{
    uint32_t *dP;
    uint32_t *dQ;
    uint32_t *qInv;
    uint32_t *p;
    uint32_t *invR_p;
    uint32_t *primeN_p;
    uint32_t *q;
    uint32_t *invR_q;
    uint32_t *primeN_q;
}ipro_dsa_crt_cfg;

int32_t ipro_dsa_init(uint32_t size);
int32_t ipro_dsa_mexp_binary(uint32_t size,uint32_t *a,uint32_t *b,uint32_t *c,uint32_t *r);
int32_t ipro_dsa_mexp_mont(uint32_t size,uint32_t *a,uint32_t *b,uint32_t *c,uint32_t *invR_c,uint32_t *primeN_c,uint32_t *r);
int32_t ipro_dsa_decrypt_crt(uint32_t size,uint32_t *c,ipro_dsa_crt_cfg *crtCfg,uint32_t *d,uint32_t *r);

#endif
