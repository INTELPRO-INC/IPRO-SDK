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

#ifndef _IPRO_ECDSA_H
#define _IPRO_ECDSA_H

#include "ipro_ecdsa_port.h"
#include "platform_common.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define IPRO_ECDSA_OK             0
#define IPRO_ECDSA_ERROR          -1

int32_t ipro_ecdsa_init(uint8_t id);

int32_t ipro_ecdsa_verify(uint8_t id,const uint32_t *hash, uint32_t hashLen,const uint32_t *pkX,const uint32_t *pkY, const uint32_t *r, const uint32_t *s);
int32_t ipro_ecdsa_deinit(uint8_t id);
int32_t ipro_ecdh_get_public_key( uint8_t id,const uint32_t *pkX,const uint32_t *pkY,const uint32_t *private_key,const uint32_t *pRx,const uint32_t *pRy);
int32_t ipro_ecc_get_random_value(uint32_t *randomData,uint32_t *maxRef,uint32_t size);
int32_t ipro_ecdsa_get_private_key( uint8_t id,uint32_t *private_key);
int32_t ipro_ecdsa_sign( uint8_t id,const uint32_t *private_key,const uint32_t *random_k,const uint32_t *hash,uint32_t *r,uint32_t *s);

#endif
