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
 * Operation contexts of the SEC_ENG PSA accelerator driver (ipro_sec).
 * Included by psa/crypto_driver_contexts_{primitives,composites}.h
 * of the 3.6 and TF-PSA trees; plain data, copied to clone.
 */
#ifndef IPRO_SEC_DRIVER_CONTEXTS_H
#define IPRO_SEC_DRIVER_CONTEXTS_H

#include <stdint.h>

#include "hal_sec_aes_v2.h"
#include "hal_sec_gmac_v2.h"
#include "hal_sec_hash_v2.h"

typedef hal_sec_hash_v2_ctx_t ipro_sec_hash_operation_t;

/*
 * ECB, CBC: buf holds blen bytes of a partial input block.
 * CTR: buf holds a keystream block of which blen bytes are used.
 */
typedef struct {
    hal_sec_aes_v2_ctx_t aes;
    uint8_t buf[16];
    uint8_t blen;
    uint8_t mode;
    uint8_t iv_length;
    uint8_t reserved;
} ipro_sec_cipher_operation_t;

/*
 * AES-GCM. ctr runs CTR32 from inc32(J0); ks holds a keystream block
 * of which ks_used bytes are used; ek_j0 = E(K, J0).
 */
typedef struct {
    hal_sec_aes_v2_ctx_t ctr;
    hal_sec_ghash_v2_ctx_t ghash;
    uint8_t ek_j0[16];
    uint8_t ks[16];
    uint64_t ad_len;
    uint64_t text_len;
    uint8_t ks_used;
    uint8_t tag_length;
    uint8_t is_encrypt;
    uint8_t body_started;
} ipro_sec_aead_operation_t;

#endif
