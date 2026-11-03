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
 * NSPE PSA Crypto client: the RPC helpers of psa_crypto_client_priv.h,
 * library init, key management and random generation. The SPE runs
 * every operation (psa_ipro/crypto_rpc.h, PSA_HANDLE_CRYPTO).
 */
#include <string.h>

#include "psa_client_priv.h"
#include "psa_crypto_client_priv.h"

/* out_vec[1] per id: none, a capacity, an exact request length */
#define OUT_NONE  0u
#define OUT_CAP   1u
#define OUT_EXACT 2u

static const uint8_t crypto_out[CRYPTO_FN_LAST + 1] = {
    [CRYPTO_FN_KEY_GET_ATTRIBUTES] = OUT_EXACT,
    [CRYPTO_FN_KEY_EXPORT]         = OUT_CAP,
    [CRYPTO_FN_KEY_EXPORT_PUBLIC]  = OUT_CAP,
    [CRYPTO_FN_HASH_COMPUTE]       = OUT_CAP,
    [CRYPTO_FN_HASH_FINISH]        = OUT_CAP,
    [CRYPTO_FN_MAC_COMPUTE]        = OUT_CAP,
    [CRYPTO_FN_MAC_SIGN_FINISH]    = OUT_CAP,
    [CRYPTO_FN_CIPHER_ENCRYPT]     = OUT_CAP,
    [CRYPTO_FN_CIPHER_DECRYPT]     = OUT_CAP,
    [CRYPTO_FN_CIPHER_GENERATE_IV] = OUT_CAP,
    [CRYPTO_FN_CIPHER_UPDATE]      = OUT_CAP,
    [CRYPTO_FN_CIPHER_FINISH]      = OUT_CAP,
    [CRYPTO_FN_AEAD_ENCRYPT]       = OUT_CAP,
    [CRYPTO_FN_AEAD_DECRYPT]       = OUT_CAP,
    [CRYPTO_FN_AEAD_GENERATE_NONCE] = OUT_CAP,
    [CRYPTO_FN_AEAD_UPDATE]        = OUT_CAP,
    [CRYPTO_FN_AEAD_FINISH]        = OUT_EXACT,
    [CRYPTO_FN_AEAD_VERIFY]        = OUT_CAP,
    [CRYPTO_FN_SIGN_HASH]          = OUT_CAP,
    [CRYPTO_FN_SIGN_MESSAGE]       = OUT_CAP,
    [CRYPTO_FN_ASYM_ENCRYPT]       = OUT_CAP,
    [CRYPTO_FN_ASYM_DECRYPT]       = OUT_CAP,
    [CRYPTO_FN_KD_OUTPUT_BYTES]    = OUT_EXACT,
    [CRYPTO_FN_RAW_KEY_AGREEMENT]  = OUT_CAP,
    [CRYPTO_FN_GENERATE_RANDOM]    = OUT_EXACT,
    [CRYPTO_FN_PAKE_OUTPUT]        = OUT_CAP,
};

struct psa_crypto_req psa_client_crypto_req(uint32_t key, uint32_t alg,
                                            uint32_t h)
{
    struct psa_crypto_req r;

    memset(&r, 0, sizeof(r));
    r.key_id = key;
    r.alg = alg;
    r.op_handle = h;
    return r;
}

/*
 * A capacity output is clamped to what the SPE can bounce next to
 * the rsp. *ran = 1 when the SPE service handled the call: success,
 * or out_vec[0] len rewritten to 0 (SPM out contract).
 */
static psa_status_t crypto_rpc(int32_t fn, const struct psa_crypto_req *req,
                               const psa_invec *d, size_t n, void *o,
                               size_t o_size, size_t *o_len,
                               struct psa_crypto_rsp *rsp, int *ran)
{
    psa_invec in[PSA_MAX_IOVEC];
    psa_outvec out[2];
    struct psa_crypto_rsp r;
    uint8_t kind = crypto_out[fn];
    psa_status_t st;

    memset(&r, 0, sizeof(r));
    in[0].base = req;
    in[0].len = sizeof(*req);
    for (size_t i = 0; i < n; i++) {
        in[i + 1u] = d[i];
    }
    out[0].base = &r;
    out[0].len = sizeof(r);
    out[1].base = o;
    out[1].len = kind == OUT_CAP ? psa_client_cap(o_size, sizeof(r))
                                 : o_size;
    st = psa_client_call(PSA_HANDLE_CRYPTO, fn, in, n + 1u, out,
                         kind != OUT_NONE ? 2u : 1u);
    *ran = st == PSA_SUCCESS || out[0].len != sizeof(r);
    if (st != PSA_SUCCESS) {
        memset(&r, 0, sizeof(r));
        out[1].len = 0;
    }
    if (o_len != NULL) {
        *o_len = kind != OUT_NONE ? out[1].len : 0;
    }
    if (rsp != NULL) {
        *rsp = r;
    }
    return st;
}

psa_status_t psa_client_crypto_call(int32_t fn,
                                    const struct psa_crypto_req *req,
                                    const psa_invec *d, size_t n, void *o,
                                    size_t o_size, size_t *o_len,
                                    struct psa_crypto_rsp *rsp)
{
    int ran;

    return crypto_rpc(fn, req, d, n, o, o_size, o_len, rsp, &ran);
}

psa_status_t psa_client_crypto_setup(int32_t fn, uint32_t key,
                                     uint32_t alg, psa_client_handle_t *h)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, alg, *h);
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    st = psa_client_crypto_call(fn, &r, NULL, 0, NULL, 0, NULL, &rsp);
    if (st == PSA_SUCCESS) {
        *h = rsp.op_handle;
    }
    return st;
}

psa_status_t psa_client_crypto_op_in(int32_t fn, uint32_t h, const void *p,
                                     size_t n)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, h);
    psa_invec d = { p, n };

    return psa_client_crypto_call(fn, &r, &d, 1, NULL, 0, NULL, NULL);
}

psa_status_t psa_client_crypto_end(int32_t fn, psa_client_handle_t *h,
                                   const psa_invec *d, size_t n, void *o,
                                   size_t o_size, size_t *o_len)
{
    struct psa_crypto_req r = psa_client_crypto_req(0, 0, *h);

    return psa_client_crypto_end_req(fn, h, &r, d, n, o, o_size, o_len,
                                     NULL);
}

psa_status_t psa_client_crypto_end_req(int32_t fn, psa_client_handle_t *h,
                                       const struct psa_crypto_req *r,
                                       const psa_invec *d, size_t n,
                                       void *o, size_t o_size,
                                       size_t *o_len,
                                       struct psa_crypto_rsp *rsp)
{
    int ran;
    psa_status_t st = crypto_rpc(fn, r, d, n, o, o_size, o_len, rsp, &ran);

    if (ran) {
        *h = 0;
    }
    return st;
}

/* A key-creating id: req.attr from attributes, *key = rsp.key_id */
static psa_status_t crypto_new_key(int32_t fn, uint32_t src,
                                   const psa_key_attributes_t *attributes,
                                   const psa_invec *d, size_t n,
                                   mbedtls_svc_key_id_t *key)
{
    struct psa_crypto_req r = psa_client_crypto_req(src, 0, 0);
    struct psa_crypto_rsp rsp;
    psa_status_t st;

    psa_crypto_attr_pack(&r.attr, attributes);
    st = psa_client_crypto_call(fn, &r, d, n, NULL, 0, NULL, &rsp);
    *key = rsp.key_id;
    return st;
}

/* An id on one key, no data vector, output o (NULL: none) */
static psa_status_t crypto_key_op(int32_t fn, mbedtls_svc_key_id_t key,
                                  void *o, size_t o_size, size_t *o_len)
{
    struct psa_crypto_req r = psa_client_crypto_req(key, 0, 0);

    return psa_client_crypto_call(fn, &r, NULL, 0, o, o_size, o_len, NULL);
}

psa_status_t psa_crypto_init(void)
{
    return crypto_key_op(CRYPTO_FN_INIT, 0, NULL, 0, NULL);
}

psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data, size_t data_length,
                            mbedtls_svc_key_id_t *key)
{
    psa_invec d = { data, data_length };

    return crypto_new_key(CRYPTO_FN_KEY_IMPORT, 0, attributes, &d, 1, key);
}

psa_status_t psa_generate_key(const psa_key_attributes_t *attributes,
                              mbedtls_svc_key_id_t *key)
{
    return crypto_new_key(CRYPTO_FN_KEY_GENERATE, 0, attributes, NULL, 0,
                          key);
}

psa_status_t psa_copy_key(mbedtls_svc_key_id_t source_key,
                          const psa_key_attributes_t *attributes,
                          mbedtls_svc_key_id_t *target_key)
{
    return crypto_new_key(CRYPTO_FN_KEY_COPY, source_key, attributes, NULL,
                          0, target_key);
}

/* Key id 0: PSA_SUCCESS without a call (PSA Crypto 1.0) */
psa_status_t psa_destroy_key(mbedtls_svc_key_id_t key)
{
    if (key == 0) {
        return PSA_SUCCESS;
    }
    return crypto_key_op(CRYPTO_FN_KEY_DESTROY, key, NULL, 0, NULL);
}

psa_status_t psa_purge_key(mbedtls_svc_key_id_t key)
{
    return crypto_key_op(CRYPTO_FN_KEY_PURGE, key, NULL, 0, NULL);
}

/* The SPE writes the public fields; a failure resets attributes */
psa_status_t psa_get_key_attributes(mbedtls_svc_key_id_t key,
                                    psa_key_attributes_t *attributes)
{
    struct psa_crypto_attr w;
    psa_status_t st = crypto_key_op(CRYPTO_FN_KEY_GET_ATTRIBUTES, key, &w,
                                    sizeof(w), NULL);

    psa_reset_key_attributes(attributes);
    if (st == PSA_SUCCESS) {
        psa_crypto_attr_unpack(attributes, &w);
    }
    return st;
}

/* Zeroes *attributes; client attributes hold no domain parameters */
void psa_reset_key_attributes(psa_key_attributes_t *attributes)
{
    memset(attributes, 0, sizeof(*attributes));
}

#if PSA_IPRO_DOMAIN_PARAMS
/* data_length 0 only; other domain parameters: NOT_SUPPORTED */
psa_status_t psa_set_key_domain_parameters(psa_key_attributes_t *attributes,
                                           psa_key_type_t type,
                                           const uint8_t *data,
                                           size_t data_length)
{
    (void)data;
    if (data_length != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    attributes->domain_parameters = NULL;
    attributes->domain_parameters_size = 0;
    attributes->core.type = type;
    return PSA_SUCCESS;
}

psa_status_t psa_get_key_domain_parameters(
    const psa_key_attributes_t *attributes, uint8_t *data, size_t data_size,
    size_t *data_length)
{
    (void)attributes;
    (void)data;
    (void)data_size;
    *data_length = 0;
    return PSA_SUCCESS;
}
#endif

psa_status_t psa_export_key(mbedtls_svc_key_id_t key, uint8_t *data,
                            size_t data_size, size_t *data_length)
{
    return crypto_key_op(CRYPTO_FN_KEY_EXPORT, key, data, data_size,
                         data_length);
}

psa_status_t psa_export_public_key(mbedtls_svc_key_id_t key, uint8_t *data,
                                   size_t data_size, size_t *data_length)
{
    return crypto_key_op(CRYPTO_FN_KEY_EXPORT_PUBLIC, key, data, data_size,
                         data_length);
}

/* One call: output_size above the bounce limit is refused */
psa_status_t psa_generate_random(uint8_t *output, size_t output_size)
{
    return crypto_key_op(CRYPTO_FN_GENERATE_RANDOM, 0, output, output_size,
                         NULL);
}
