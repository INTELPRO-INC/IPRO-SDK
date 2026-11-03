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
 * One mbedtls PSA call per crypto RPC id, grouped by family. Vectors,
 * keys and slots are checked by svc_crypto.c before these run.
 */
#include <string.h>

#include "svc_crypto_priv.h"

/* Copy out the public fields only, as struct psa_crypto_attr */
static psa_status_t svc_key_attributes(struct svc_call *c)
{
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;
    struct psa_crypto_attr pub;
    psa_status_t st;

    if (c->out_cap != sizeof(pub)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    st = psa_get_key_attributes(c->req.key_id, &a);
    if (st == PSA_SUCCESS) {
        psa_crypto_attr_pack(&pub, &a);
        memcpy(c->out, &pub, sizeof(pub));
        c->rsp.length = sizeof(pub);
    }
    psa_reset_key_attributes(&a);
    return st;
}

static psa_status_t svc_key(int32_t fn, struct svc_call *c)
{
    switch (fn) {
    case CRYPTO_FN_INIT:
        return psa_crypto_init();
    case CRYPTO_FN_KEY_IMPORT:
        return psa_import_key(&c->attr, c->in[0], c->in_len[0],
                              &c->rsp.key_id);
    case CRYPTO_FN_KEY_DESTROY:
        return psa_destroy_key(c->req.key_id);
    case CRYPTO_FN_KEY_GET_ATTRIBUTES:
        return svc_key_attributes(c);
    case CRYPTO_FN_KEY_EXPORT:
        return psa_export_key(c->req.key_id, c->out, c->out_cap,
                              &c->rsp.length);
    case CRYPTO_FN_KEY_EXPORT_PUBLIC:
        return psa_export_public_key(c->req.key_id, c->out, c->out_cap,
                                     &c->rsp.length);
    case CRYPTO_FN_KEY_GENERATE:
        return psa_generate_key(&c->attr, &c->rsp.key_id);
    case CRYPTO_FN_KEY_COPY:
        return psa_copy_key(c->req.key_id, &c->attr, &c->rsp.key_id);
    case CRYPTO_FN_KEY_PURGE:
        return psa_purge_key(c->req.key_id);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

static psa_status_t svc_hash(int32_t fn, struct svc_call *c)
{
    psa_hash_operation_t *op = c->op != NULL ? &c->op->u.hash : NULL;

    switch (fn) {
    case CRYPTO_FN_HASH_COMPUTE:
        return psa_hash_compute(c->req.alg, c->in[0], c->in_len[0],
                                c->out, c->out_cap, &c->rsp.length);
    case CRYPTO_FN_HASH_COMPARE:
        return psa_hash_compare(c->req.alg, c->in[0], c->in_len[0],
                                c->in[1], c->in_len[1]);
    case CRYPTO_FN_HASH_SETUP:
        return psa_hash_setup(&c->new_op->u.hash, c->req.alg);
    case CRYPTO_FN_HASH_UPDATE:
        return psa_hash_update(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_HASH_FINISH:
        return psa_hash_finish(op, c->out, c->out_cap, &c->rsp.length);
    case CRYPTO_FN_HASH_VERIFY:
        return psa_hash_verify(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_HASH_ABORT:
        return psa_hash_abort(op);
    case CRYPTO_FN_HASH_CLONE:
        return psa_hash_clone(op, &c->new_op->u.hash);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

static psa_status_t svc_mac(int32_t fn, struct svc_call *c)
{
    psa_mac_operation_t *op = c->op != NULL ? &c->op->u.mac : NULL;

    switch (fn) {
    case CRYPTO_FN_MAC_COMPUTE:
        return psa_mac_compute(c->req.key_id, c->req.alg, c->in[0],
                               c->in_len[0], c->out, c->out_cap,
                               &c->rsp.length);
    case CRYPTO_FN_MAC_VERIFY:
        return psa_mac_verify(c->req.key_id, c->req.alg, c->in[0],
                              c->in_len[0], c->in[1], c->in_len[1]);
    case CRYPTO_FN_MAC_SIGN_SETUP:
        return psa_mac_sign_setup(&c->new_op->u.mac, c->req.key_id,
                                  c->req.alg);
    case CRYPTO_FN_MAC_VERIFY_SETUP:
        return psa_mac_verify_setup(&c->new_op->u.mac, c->req.key_id,
                                    c->req.alg);
    case CRYPTO_FN_MAC_UPDATE:
        return psa_mac_update(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_MAC_SIGN_FINISH:
        return psa_mac_sign_finish(op, c->out, c->out_cap, &c->rsp.length);
    case CRYPTO_FN_MAC_VERIFY_FINISH:
        return psa_mac_verify_finish(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_MAC_ABORT:
        return psa_mac_abort(op);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

static psa_status_t svc_cipher(int32_t fn, struct svc_call *c)
{
    psa_cipher_operation_t *op = c->op != NULL ? &c->op->u.cipher : NULL;

    switch (fn) {
    case CRYPTO_FN_CIPHER_ENCRYPT:
        return psa_cipher_encrypt(c->req.key_id, c->req.alg, c->in[0],
                                  c->in_len[0], c->out, c->out_cap,
                                  &c->rsp.length);
    case CRYPTO_FN_CIPHER_DECRYPT:
        return psa_cipher_decrypt(c->req.key_id, c->req.alg, c->in[0],
                                  c->in_len[0], c->out, c->out_cap,
                                  &c->rsp.length);
    case CRYPTO_FN_CIPHER_ENCRYPT_SETUP:
        return psa_cipher_encrypt_setup(&c->new_op->u.cipher,
                                        c->req.key_id, c->req.alg);
    case CRYPTO_FN_CIPHER_DECRYPT_SETUP:
        return psa_cipher_decrypt_setup(&c->new_op->u.cipher,
                                        c->req.key_id, c->req.alg);
    case CRYPTO_FN_CIPHER_GENERATE_IV:
        return psa_cipher_generate_iv(op, c->out, c->out_cap,
                                      &c->rsp.length);
    case CRYPTO_FN_CIPHER_SET_IV:
        return psa_cipher_set_iv(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_CIPHER_UPDATE:
        return psa_cipher_update(op, c->in[0], c->in_len[0], c->out,
                                 c->out_cap, &c->rsp.length);
    case CRYPTO_FN_CIPHER_FINISH:
        return psa_cipher_finish(op, c->out, c->out_cap, &c->rsp.length);
    case CRYPTO_FN_CIPHER_ABORT:
        return psa_cipher_abort(op);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

#if PSA_IPRO_AEAD_MULTI
static psa_status_t svc_aead_lengths(psa_aead_operation_t *op,
                                     struct svc_call *c)
{
    struct psa_crypto_aead_lengths l;

    if (c->in_len[0] != sizeof(l)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&l, c->in[0], sizeof(l));
    return psa_aead_set_lengths(op, l.ad_length, l.plaintext_length);
}

/* out: req.aux ciphertext bytes, the unused ones zeroed, then the tag */
static psa_status_t svc_aead_finish(psa_aead_operation_t *op,
                                    struct svc_call *c)
{
    size_t ct_cap = c->req.aux;
    size_t ct_len = 0;
    size_t tag_len = 0;
    psa_status_t st;

    if (ct_cap > c->out_cap) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    st = psa_aead_finish(op, c->out, ct_cap, &ct_len, c->out + ct_cap,
                         c->out_cap - ct_cap, &tag_len);
    if (st == PSA_SUCCESS) {
        memset(c->out + ct_len, 0, ct_cap - ct_len);
        c->rsp.length = ct_cap + tag_len;
        c->rsp.length2 = ct_len;
    }
    return st;
}

static psa_status_t svc_aead_multi(int32_t fn, struct svc_call *c)
{
    psa_aead_operation_t *op = c->op != NULL ? &c->op->u.aead : NULL;

    switch (fn) {
    case CRYPTO_FN_AEAD_ENCRYPT_SETUP:
        return psa_aead_encrypt_setup(&c->new_op->u.aead, c->req.key_id,
                                      c->req.alg);
    case CRYPTO_FN_AEAD_DECRYPT_SETUP:
        return psa_aead_decrypt_setup(&c->new_op->u.aead, c->req.key_id,
                                      c->req.alg);
    case CRYPTO_FN_AEAD_GENERATE_NONCE:
        return psa_aead_generate_nonce(op, c->out, c->out_cap,
                                       &c->rsp.length);
    case CRYPTO_FN_AEAD_SET_NONCE:
        return psa_aead_set_nonce(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_AEAD_SET_LENGTHS:
        return svc_aead_lengths(op, c);
    case CRYPTO_FN_AEAD_UPDATE_AD:
        return psa_aead_update_ad(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_AEAD_UPDATE:
        return psa_aead_update(op, c->in[0], c->in_len[0], c->out,
                               c->out_cap, &c->rsp.length);
    case CRYPTO_FN_AEAD_FINISH:
        return svc_aead_finish(op, c);
    case CRYPTO_FN_AEAD_VERIFY:
        return psa_aead_verify(op, c->out, c->out_cap, &c->rsp.length,
                               c->in[0], c->in_len[0]);
    case CRYPTO_FN_AEAD_ABORT:
        return psa_aead_abort(op);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif

static psa_status_t svc_aead(int32_t fn, struct svc_call *c)
{
    switch (fn) {
    case CRYPTO_FN_AEAD_ENCRYPT:
        return psa_aead_encrypt(c->req.key_id, c->req.alg, c->in[0],
                                c->in_len[0], c->in[1], c->in_len[1],
                                c->in[2], c->in_len[2], c->out,
                                c->out_cap, &c->rsp.length);
    case CRYPTO_FN_AEAD_DECRYPT:
        return psa_aead_decrypt(c->req.key_id, c->req.alg, c->in[0],
                                c->in_len[0], c->in[1], c->in_len[1],
                                c->in[2], c->in_len[2], c->out,
                                c->out_cap, &c->rsp.length);
    default:
#if PSA_IPRO_AEAD_MULTI
        return svc_aead_multi(fn, c);
#else
        return PSA_ERROR_NOT_SUPPORTED;
#endif
    }
}

static psa_status_t svc_sign(int32_t fn, struct svc_call *c)
{
    switch (fn) {
    case CRYPTO_FN_SIGN_HASH:
        return psa_sign_hash(c->req.key_id, c->req.alg, c->in[0],
                             c->in_len[0], c->out, c->out_cap,
                             &c->rsp.length);
    case CRYPTO_FN_VERIFY_HASH:
        return psa_verify_hash(c->req.key_id, c->req.alg, c->in[0],
                               c->in_len[0], c->in[1], c->in_len[1]);
    case CRYPTO_FN_SIGN_MESSAGE:
        return psa_sign_message(c->req.key_id, c->req.alg, c->in[0],
                                c->in_len[0], c->out, c->out_cap,
                                &c->rsp.length);
    case CRYPTO_FN_VERIFY_MESSAGE:
        return psa_verify_message(c->req.key_id, c->req.alg, c->in[0],
                                  c->in_len[0], c->in[1], c->in_len[1]);
    case CRYPTO_FN_ASYM_ENCRYPT:
        return psa_asymmetric_encrypt(c->req.key_id, c->req.alg, c->in[0],
                                      c->in_len[0], c->in[1],
                                      c->in_len[1], c->out, c->out_cap,
                                      &c->rsp.length);
    case CRYPTO_FN_ASYM_DECRYPT:
        return psa_asymmetric_decrypt(c->req.key_id, c->req.alg, c->in[0],
                                      c->in_len[0], c->in[1],
                                      c->in_len[1], c->out, c->out_cap,
                                      &c->rsp.length);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

#if PSA_IPRO_KD_INTEGER
static psa_status_t svc_kd_integer(psa_key_derivation_operation_t *op,
                                   psa_key_derivation_step_t step,
                                   struct svc_call *c)
{
    uint64_t v;

    if (c->in_len[0] != sizeof(v)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&v, c->in[0], sizeof(v));
    return psa_key_derivation_input_integer(op, step, v);
}
#endif

static psa_status_t svc_kd(int32_t fn, struct svc_call *c)
{
    psa_key_derivation_operation_t *op = c->op != NULL ? &c->op->u.kd
                                                       : NULL;
    psa_key_derivation_step_t step = (psa_key_derivation_step_t)c->req.step;
    psa_status_t st;

    switch (fn) {
    case CRYPTO_FN_KD_SETUP:
        return psa_key_derivation_setup(&c->new_op->u.kd, c->req.alg);
    case CRYPTO_FN_KD_GET_CAPACITY:
        return psa_key_derivation_get_capacity(op, &c->rsp.length);
    case CRYPTO_FN_KD_SET_CAPACITY:
        return psa_key_derivation_set_capacity(op, c->req.aux);
    case CRYPTO_FN_KD_INPUT_BYTES:
        return psa_key_derivation_input_bytes(op, step, c->in[0],
                                              c->in_len[0]);
    case CRYPTO_FN_KD_INPUT_KEY:
        return psa_key_derivation_input_key(op, step, c->req.key_id);
    case CRYPTO_FN_KD_KEY_AGREEMENT:
        return psa_key_derivation_key_agreement(op, step, c->req.key_id,
                                                c->in[0], c->in_len[0]);
    case CRYPTO_FN_KD_OUTPUT_BYTES:
        st = psa_key_derivation_output_bytes(op, c->out, c->out_cap);
        c->rsp.length = st == PSA_SUCCESS ? c->out_cap : 0;
        return st;
    case CRYPTO_FN_KD_OUTPUT_KEY:
        return psa_key_derivation_output_key(&c->attr, op,
                                             &c->rsp.key_id);
    case CRYPTO_FN_KD_ABORT:
        return psa_key_derivation_abort(op);
    case CRYPTO_FN_RAW_KEY_AGREEMENT:
        return psa_raw_key_agreement(c->req.alg, c->req.key_id, c->in[0],
                                     c->in_len[0], c->out, c->out_cap,
                                     &c->rsp.length);
#if PSA_IPRO_KD_INTEGER
    case CRYPTO_FN_KD_INPUT_INTEGER:
        return svc_kd_integer(op, step, c);
#endif
#if PSA_IPRO_API_1_2
    case CRYPTO_FN_KEY_AGREEMENT:
        return psa_key_agreement(c->req.key_id, c->in[0], c->in_len[0],
                                 c->req.alg, &c->attr, &c->rsp.key_id);
#endif
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}

static psa_status_t svc_random(int32_t fn, struct svc_call *c)
{
    psa_status_t st;

    if (fn != CRYPTO_FN_GENERATE_RANDOM) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    st = psa_generate_random(c->out, c->out_cap);
    c->rsp.length = st == PSA_SUCCESS ? c->out_cap : 0;
    return st;
}

#if PSA_IPRO_API_1_2
static psa_status_t svc_pake_setup(struct svc_call *c)
{
    psa_pake_cipher_suite_t cs = psa_pake_cipher_suite_init();
    struct psa_crypto_pake_suite w;

    if (c->in_len[0] != sizeof(w)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(&w, c->in[0], sizeof(w));
    psa_pake_cs_set_algorithm(&cs, w.alg);
    psa_pake_cs_set_primitive(&cs, w.primitive);
    psa_pake_cs_set_key_confirmation(&cs, w.key_confirmation);
    return psa_pake_setup(&c->new_op->u.pake, c->req.key_id, &cs);
}

static psa_status_t svc_pake(int32_t fn, struct svc_call *c)
{
    psa_pake_operation_t *op = c->op != NULL ? &c->op->u.pake : NULL;
    psa_pake_step_t step = (psa_pake_step_t)c->req.step;

    if (c->req.step > UINT8_MAX || c->req.aux > UINT8_MAX) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    switch (fn) {
    case CRYPTO_FN_PAKE_SETUP:
        return svc_pake_setup(c);
    case CRYPTO_FN_PAKE_SET_ROLE:
        return psa_pake_set_role(op, (psa_pake_role_t)c->req.aux);
    case CRYPTO_FN_PAKE_SET_USER:
        return psa_pake_set_user(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_PAKE_SET_PEER:
        return psa_pake_set_peer(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_PAKE_SET_CONTEXT:
        return psa_pake_set_context(op, c->in[0], c->in_len[0]);
    case CRYPTO_FN_PAKE_OUTPUT:
        return psa_pake_output(op, step, c->out, c->out_cap,
                               &c->rsp.length);
    case CRYPTO_FN_PAKE_INPUT:
        return psa_pake_input(op, step, c->in[0], c->in_len[0]);
    case CRYPTO_FN_PAKE_GET_SHARED_KEY:
        return psa_pake_get_shared_key(op, &c->attr, &c->rsp.key_id);
    case CRYPTO_FN_PAKE_ABORT:
        return psa_pake_abort(op);
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
#endif

psa_status_t svc_family_call(uint32_t family, int32_t fn,
                             struct svc_call *c)
{
    switch (family) {
    case S_KEY:
        return svc_key(fn, c);
    case S_HASH:
        return svc_hash(fn, c);
    case S_MAC:
        return svc_mac(fn, c);
    case S_CIPHER:
        return svc_cipher(fn, c);
    case S_AEAD:
        return svc_aead(fn, c);
    case S_SIGN:
        return svc_sign(fn, c);
    case S_KD:
        return svc_kd(fn, c);
    case S_RANDOM:
        return svc_random(fn, c);
#if PSA_IPRO_API_1_2
    case S_PAKE:
        return svc_pake(fn, c);
#endif
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }
}
