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

#ifndef __HAL_SEC_ECDSA__H__
#define __HAL_SEC_ECDSA__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_common.h"

typedef enum {
    ECP_SECP256R1 = 0,
    ECP_SECP256K1 = 1,
    ECP_TYPE_MAX = 2,
} sec_ecp_type;

typedef struct
{
    sec_ecp_type ecpId;
    uint32_t *privateKey;
    uint32_t *publicKeyx;
    uint32_t *publicKeyy;
} sec_ecdsa_handle_t;

typedef struct
{
    sec_ecp_type ecpId;
} sec_ecdh_handle_t;

int sec_ecdsa_init(sec_ecdsa_handle_t *handle, sec_ecp_type id);
int sec_ecdsa_deinit(sec_ecdsa_handle_t *handle);
int sec_ecdsa_sign(sec_ecdsa_handle_t *handle, const uint32_t *random_k, const uint32_t *hash, uint32_t hashLenInWord, uint32_t *r, uint32_t *s);
int sec_ecdsa_verify(sec_ecdsa_handle_t *handle, const uint32_t *hash, uint32_t hashLen, const uint32_t *r, const uint32_t *s);
int sec_ecdsa_get_private_key(sec_ecdsa_handle_t *handle, uint32_t *private_key);
int sec_ecdsa_get_public_key(sec_ecdsa_handle_t *handle, const uint32_t *private_key, const uint32_t *pRx, const uint32_t *pRy);

int sec_ecdh_init(sec_ecdh_handle_t *handle, sec_ecp_type id);
int sec_ecdh_deinit(sec_ecdh_handle_t *handle);
int sec_ecdh_get_encrypt_key(sec_ecdh_handle_t *handle, const uint32_t *pkX, const uint32_t *pkY, const uint32_t *private_key, const uint32_t *pRx, const uint32_t *pRy);
int sec_ecdh_get_public_key(sec_ecdh_handle_t *handle, const uint32_t *private_key, const uint32_t *pRx, const uint32_t *pRy);
int sec_ecc_get_random_value(uint32_t *randomData, uint32_t *maxRef, uint32_t size);
int sec_eng_trng_enable(void);
void sec_eng_trng_disable(void);
int sec_eng_trng_read(uint8_t data[32]);
#ifdef __cplusplus
}
#endif
#endif