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
 * Shared by svc_crypto.c (RPC checks, operation slots, key owners)
 * and svc_crypto_family.c (the mbedtls PSA calls).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa_ipro/crypto_lib.h"
#include "psa/crypto.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/crypto_rpc.h"

/* RPC families; HASH..KD, AEAD and PAKE are also op slot kinds */
enum svc_family {
    S_NONE = 0,
    S_HASH,
    S_MAC,
    S_CIPHER,
    S_KD,
    S_KEY,
    S_AEAD,
    S_SIGN,
    S_RANDOM,
    S_PAKE,
};

/* kind 0 = free; else the family of the operation */
struct svc_op {
    uint32_t kind;
    union {
        psa_hash_operation_t hash;
        psa_mac_operation_t mac;
        psa_cipher_operation_t cipher;
        psa_key_derivation_operation_t kd;
#if PSA_IPRO_AEAD_MULTI
        psa_aead_operation_t aead;
#endif
#if PSA_IPRO_API_1_2
        psa_pake_operation_t pake;
#endif
    } u;
};

struct svc_call {
    struct psa_crypto_req req;
    psa_key_attributes_t attr;  /* req.attr unpacked (A_IN ids) */
    const uint8_t *in[PSA_MAX_IOVEC - 1];
    size_t in_len[PSA_MAX_IOVEC - 1];
    uint8_t *out;
    size_t out_cap;
    struct svc_op *op;          /* slot of req.op_handle */
    struct svc_op *new_op;      /* slot of rsp.op_handle */
    int idle;                   /* OP_IDLE with handle 0: no call */
    struct psa_crypto_rsp rsp;
};

/* The mbedtls PSA call of id fn, which belongs to family */
psa_status_t svc_family_call(uint32_t family, int32_t fn,
                             struct svc_call *c);
