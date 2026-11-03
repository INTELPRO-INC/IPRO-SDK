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
 * PSA attestation token: the PSA IoT profile 1 claims map and its
 * COSE_Sign1 (tag 18, ES256) envelope. Pure C over tinycbor and the
 * mbedtls SHA-256; the signature comes from a callback.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#define EAT_NONCE_MAX 64u
#define EAT_SW_MAX    3u
#define EAT_SIG_SIZE  64u

/* claim keys (PSA IoT profile 1) */
#define EAT_PROFILE         (-75000)
#define EAT_CLIENT_ID       (-75001)
#define EAT_LIFECYCLE       (-75002)
#define EAT_IMPL_ID         (-75003)
#define EAT_BOOT_SEED       (-75004)
#define EAT_SW_COMPONENTS   (-75006)
#define EAT_NONCE           (-75008)
#define EAT_INSTANCE_ID     (-75009)
#define EAT_VERIFY_URL      (-75010)

/* sw component map keys */
#define EAT_SW_TYPE         1
#define EAT_SW_MEASUREMENT  2
#define EAT_SW_VERSION      4
#define EAT_SW_SIGNER_ID    5
#define EAT_SW_DESC         6

/* NULL strings are left out of the token */
struct eat_sw {
    const char *type;
    const char *version;
    uint8_t measurement[32];
    const char *measurement_desc;
    const char *signer_id_hex;      /* bstr of its 1..64 bytes */
};

struct eat_claims {
    uint8_t nonce[EAT_NONCE_MAX];
    size_t nonce_len;
    int32_t client_id;
    uint32_t lifecycle;
    uint8_t impl_id[32];
    uint8_t boot_seed[32];
    uint8_t instance_id[33];
    const char *profile;
    const char *verification_url;
    struct eat_sw sw[EAT_SW_MAX];
    size_t sw_count;
};

/* sig = raw r || s over the 32-byte hash; PSA_SUCCESS or an error */
typedef int (*eat_sign_fn)(const uint8_t hash[32], uint8_t sig[64],
                           void *ctx);

/*
 * CBOR claims map into out. PSA_SUCCESS with *len = bytes written;
 * PSA_ERROR_BUFFER_TOO_SMALL with *len = bytes needed; else *len 0.
 */
int eat_encode_claims(const struct eat_claims *c, uint8_t *out,
                      size_t cap, size_t *len);

/*
 * COSE_Sign1 of payload into out; payload may overlap out. sign()
 * gets SHA-256 of ["Signature1", h'A10126', h'', payload]. Sizes as
 * eat_encode_claims; on any failure out and payload are intact.
 */
int eat_cose_sign1(const uint8_t *payload, size_t plen, eat_sign_fn sign,
                   void *ctx, uint8_t *out, size_t cap, size_t *len);
