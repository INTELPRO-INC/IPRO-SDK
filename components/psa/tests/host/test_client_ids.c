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
 * Every PSA Crypto RPC id the client sends, in crypto_rpc.h order:
 * vector counts, the req fields of the id (others 0) and the data
 * vectors in natural argument order.
 */
#include <stdio.h>
#include <string.h>

#include "client_stub.h"
#include "psa_crypto_client_priv.h"

#define KEY  0x1001u
#define ALG  PSA_ALG_SHA_256
#define HV   5u
#define STEP PSA_KEY_DERIVATION_INPUT_SECRET
#define CAPY 77u

/* req fields an id carries */
#define K 0x01u     /* key_id */
#define A 0x02u     /* alg */
#define H 0x04u     /* op_handle */
#define S 0x08u     /* step */
#define X 0x10u     /* aux */
#define T 0x20u     /* attr */

/* out_vec[1]: none, obuf, the attributes struct */
#define O_NONE 0u
#define O_BUF  1u
#define O_ATTR 2u

struct want {
    int32_t fn;
    uint8_t n;          /* data vectors after the req */
    uint8_t out;
    uint8_t f;
    const char *name;
};

static const struct want wants[] = {
    { CRYPTO_FN_INIT, 0, O_NONE, 0, "INIT" },
    { CRYPTO_FN_KEY_IMPORT, 1, O_NONE, T, "KEY_IMPORT" },
    { CRYPTO_FN_KEY_DESTROY, 0, O_NONE, K, "KEY_DESTROY" },
    { CRYPTO_FN_KEY_GET_ATTRIBUTES, 0, O_ATTR, K, "KEY_GET_ATTRIBUTES" },
    { CRYPTO_FN_KEY_EXPORT, 0, O_BUF, K, "KEY_EXPORT" },
    { CRYPTO_FN_KEY_EXPORT_PUBLIC, 0, O_BUF, K, "KEY_EXPORT_PUBLIC" },
    { CRYPTO_FN_KEY_GENERATE, 0, O_NONE, T, "KEY_GENERATE" },
    { CRYPTO_FN_KEY_COPY, 0, O_NONE, K | T, "KEY_COPY" },
    { CRYPTO_FN_KEY_PURGE, 0, O_NONE, K, "KEY_PURGE" },
    { CRYPTO_FN_HASH_COMPUTE, 1, O_BUF, A, "HASH_COMPUTE" },
    { CRYPTO_FN_HASH_COMPARE, 2, O_NONE, A, "HASH_COMPARE" },
    { CRYPTO_FN_HASH_SETUP, 0, O_NONE, A, "HASH_SETUP" },
    { CRYPTO_FN_HASH_UPDATE, 1, O_NONE, H, "HASH_UPDATE" },
    { CRYPTO_FN_HASH_FINISH, 0, O_BUF, H, "HASH_FINISH" },
    { CRYPTO_FN_HASH_VERIFY, 1, O_NONE, H, "HASH_VERIFY" },
    { CRYPTO_FN_HASH_ABORT, 0, O_NONE, H, "HASH_ABORT" },
    { CRYPTO_FN_HASH_CLONE, 0, O_NONE, H, "HASH_CLONE" },
    { CRYPTO_FN_MAC_COMPUTE, 1, O_BUF, K | A, "MAC_COMPUTE" },
    { CRYPTO_FN_MAC_VERIFY, 2, O_NONE, K | A, "MAC_VERIFY" },
    { CRYPTO_FN_MAC_SIGN_SETUP, 0, O_NONE, K | A, "MAC_SIGN_SETUP" },
    { CRYPTO_FN_MAC_VERIFY_SETUP, 0, O_NONE, K | A, "MAC_VERIFY_SETUP" },
    { CRYPTO_FN_MAC_UPDATE, 1, O_NONE, H, "MAC_UPDATE" },
    { CRYPTO_FN_MAC_SIGN_FINISH, 0, O_BUF, H, "MAC_SIGN_FINISH" },
    { CRYPTO_FN_MAC_VERIFY_FINISH, 1, O_NONE, H, "MAC_VERIFY_FINISH" },
    { CRYPTO_FN_MAC_ABORT, 0, O_NONE, H, "MAC_ABORT" },
    { CRYPTO_FN_CIPHER_ENCRYPT, 1, O_BUF, K | A, "CIPHER_ENCRYPT" },
    { CRYPTO_FN_CIPHER_DECRYPT, 1, O_BUF, K | A, "CIPHER_DECRYPT" },
    { CRYPTO_FN_CIPHER_ENCRYPT_SETUP, 0, O_NONE, K | A,
      "CIPHER_ENCRYPT_SETUP" },
    { CRYPTO_FN_CIPHER_DECRYPT_SETUP, 0, O_NONE, K | A,
      "CIPHER_DECRYPT_SETUP" },
    { CRYPTO_FN_CIPHER_GENERATE_IV, 0, O_BUF, H, "CIPHER_GENERATE_IV" },
    { CRYPTO_FN_CIPHER_SET_IV, 1, O_NONE, H, "CIPHER_SET_IV" },
    { CRYPTO_FN_CIPHER_UPDATE, 1, O_BUF, H, "CIPHER_UPDATE" },
    { CRYPTO_FN_CIPHER_FINISH, 0, O_BUF, H, "CIPHER_FINISH" },
    { CRYPTO_FN_CIPHER_ABORT, 0, O_NONE, H, "CIPHER_ABORT" },
    { CRYPTO_FN_AEAD_ENCRYPT, 3, O_BUF, K | A, "AEAD_ENCRYPT" },
    { CRYPTO_FN_AEAD_DECRYPT, 3, O_BUF, K | A, "AEAD_DECRYPT" },
    { CRYPTO_FN_SIGN_HASH, 1, O_BUF, K | A, "SIGN_HASH" },
    { CRYPTO_FN_VERIFY_HASH, 2, O_NONE, K | A, "VERIFY_HASH" },
    { CRYPTO_FN_SIGN_MESSAGE, 1, O_BUF, K | A, "SIGN_MESSAGE" },
    { CRYPTO_FN_VERIFY_MESSAGE, 2, O_NONE, K | A, "VERIFY_MESSAGE" },
    { CRYPTO_FN_ASYM_ENCRYPT, 2, O_BUF, K | A, "ASYM_ENCRYPT" },
    { CRYPTO_FN_ASYM_DECRYPT, 2, O_BUF, K | A, "ASYM_DECRYPT" },
    { CRYPTO_FN_KD_SETUP, 0, O_NONE, A, "KD_SETUP" },
    { CRYPTO_FN_KD_GET_CAPACITY, 0, O_NONE, H, "KD_GET_CAPACITY" },
    { CRYPTO_FN_KD_SET_CAPACITY, 0, O_NONE, H | X, "KD_SET_CAPACITY" },
    { CRYPTO_FN_KD_INPUT_BYTES, 1, O_NONE, H | S, "KD_INPUT_BYTES" },
    { CRYPTO_FN_KD_INPUT_KEY, 0, O_NONE, H | S | K, "KD_INPUT_KEY" },
    { CRYPTO_FN_KD_KEY_AGREEMENT, 1, O_NONE, H | S | K,
      "KD_KEY_AGREEMENT" },
    { CRYPTO_FN_KD_OUTPUT_BYTES, 0, O_BUF, H, "KD_OUTPUT_BYTES" },
    { CRYPTO_FN_KD_OUTPUT_KEY, 0, O_NONE, H | T, "KD_OUTPUT_KEY" },
    { CRYPTO_FN_KD_ABORT, 0, O_NONE, H, "KD_ABORT" },
    { CRYPTO_FN_RAW_KEY_AGREEMENT, 1, O_BUF, K | A, "RAW_KEY_AGREEMENT" },
    { CRYPTO_FN_GENERATE_RANDOM, 0, O_BUF, 0, "GENERATE_RANDOM" },
};

#define N_WANTS (sizeof(wants) / sizeof(wants[0]))

static const uint8_t d1[11] = "d1";
static const uint8_t d2[12] = "d2";
static const uint8_t d3[13] = "d3";
static uint8_t obuf[40];
static psa_key_attributes_t g_attr;
static psa_key_attributes_t g_got;

/* Ops with handle HV (in use) or 0 (fresh), per family */
static psa_hash_operation_t hash_op(uint32_t h)
{
    psa_hash_operation_t op = PSA_HASH_OPERATION_INIT;

    PSA_CLIENT_H(&op) = h;
    return op;
}

static psa_mac_operation_t mac_op(uint32_t h)
{
    psa_mac_operation_t op = PSA_MAC_OPERATION_INIT;

    PSA_CLIENT_H(&op) = h;
    return op;
}

static psa_cipher_operation_t cipher_op(uint32_t h)
{
    psa_cipher_operation_t op = PSA_CIPHER_OPERATION_INIT;

    PSA_CLIENT_H(&op) = h;
    return op;
}

/* a KD op on SPE handle h */
static psa_key_derivation_operation_t kd_op(uint32_t h)
{
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;

    PSA_CLIENT_KD_H(&op) = h;
    return op;
}

/* One API call per row of wants[], same order */
static void call_all(void)
{
    psa_hash_operation_t h = hash_op(HV), h0 = hash_op(0);
    psa_mac_operation_t m = mac_op(HV), m0 = mac_op(0);
    psa_cipher_operation_t c = cipher_op(HV), c0 = cipher_op(0);
    psa_key_derivation_operation_t kd = kd_op(HV), kd0 = kd_op(0);
    mbedtls_svc_key_id_t k;
    size_t n;
    size_t cap;

    psa_crypto_init();
    psa_import_key(&g_attr, d1, sizeof(d1), &k);
    psa_destroy_key(KEY);
    psa_get_key_attributes(KEY, &g_got);
    psa_export_key(KEY, obuf, sizeof(obuf), &n);
    psa_export_public_key(KEY, obuf, sizeof(obuf), &n);
    psa_generate_key(&g_attr, &k);
    psa_copy_key(KEY, &g_attr, &k);
    psa_purge_key(KEY);
    psa_hash_compute(ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_hash_compare(ALG, d1, sizeof(d1), d2, sizeof(d2));
    psa_hash_setup(&h0, ALG);
    psa_hash_update(&h, d1, sizeof(d1));
    psa_hash_finish(&h, obuf, sizeof(obuf), &n);
    h = hash_op(HV);
    psa_hash_verify(&h, d1, sizeof(d1));
    h = hash_op(HV);
    psa_hash_abort(&h);
    h = hash_op(HV);
    h0 = hash_op(0);
    psa_hash_clone(&h, &h0);
    psa_mac_compute(KEY, ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_mac_verify(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2));
    psa_mac_sign_setup(&m0, KEY, ALG);
    m0 = mac_op(0);
    psa_mac_verify_setup(&m0, KEY, ALG);
    psa_mac_update(&m, d1, sizeof(d1));
    psa_mac_sign_finish(&m, obuf, sizeof(obuf), &n);
    m = mac_op(HV);
    psa_mac_verify_finish(&m, d1, sizeof(d1));
    m = mac_op(HV);
    psa_mac_abort(&m);
    psa_cipher_encrypt(KEY, ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_cipher_decrypt(KEY, ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_cipher_encrypt_setup(&c0, KEY, ALG);
    c0 = cipher_op(0);
    psa_cipher_decrypt_setup(&c0, KEY, ALG);
    psa_cipher_generate_iv(&c, obuf, sizeof(obuf), &n);
    psa_cipher_set_iv(&c, d1, sizeof(d1));
    psa_cipher_update(&c, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_cipher_finish(&c, obuf, sizeof(obuf), &n);
    c = cipher_op(HV);
    psa_cipher_abort(&c);
    psa_aead_encrypt(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2), d3,
                     sizeof(d3), obuf, sizeof(obuf), &n);
    psa_aead_decrypt(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2), d3,
                     sizeof(d3), obuf, sizeof(obuf), &n);
    psa_sign_hash(KEY, ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_verify_hash(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2));
    psa_sign_message(KEY, ALG, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_verify_message(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2));
    psa_asymmetric_encrypt(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2), obuf,
                           sizeof(obuf), &n);
    psa_asymmetric_decrypt(KEY, ALG, d1, sizeof(d1), d2, sizeof(d2), obuf,
                           sizeof(obuf), &n);
    psa_key_derivation_setup(&kd0, ALG);
    psa_key_derivation_get_capacity(&kd, &cap);
    psa_key_derivation_set_capacity(&kd, CAPY);
    psa_key_derivation_input_bytes(&kd, STEP, d1, sizeof(d1));
    psa_key_derivation_input_key(&kd, STEP, KEY);
    psa_key_derivation_key_agreement(&kd, STEP, KEY, d1, sizeof(d1));
    psa_key_derivation_output_bytes(&kd, obuf, sizeof(obuf));
    psa_key_derivation_output_key(&g_attr, &kd, &k);
    psa_key_derivation_abort(&kd);
    psa_raw_key_agreement(ALG, KEY, d1, sizeof(d1), obuf, sizeof(obuf), &n);
    psa_generate_random(obuf, sizeof(obuf));
}

static int row_ok(const struct call *c, const struct want *w)
{
    static const uint8_t *const dv[3] = { d1, d2, d3 };
    static const size_t dl[3] = { sizeof(d1), sizeof(d2), sizeof(d3) };
    struct psa_crypto_req q = req_of(c);
    struct psa_crypto_attr wa;
    int ok;

    ok = c->handle == PSA_HANDLE_CRYPTO && c->type == w->fn &&
         c->in_len == 1u + w->n && c->in[0].len == REQ &&
         c->out_len == (w->out != O_NONE ? 2u : 1u) && c->out[0].len == RSP;
    ok = ok && q.key_id == ((w->f & K) ? KEY : 0) &&
         q.alg == ((w->f & A) ? ALG : 0) &&
         q.op_handle == ((w->f & H) ? HV : 0) &&
         q.step == ((w->f & S) ? STEP : 0) &&
         q.aux == ((w->f & X) ? CAPY : 0) && q.aux2 == 0;
    psa_crypto_attr_pack(&wa, &g_attr);
    ok = ok && ((w->f & T) ? memcmp(&q.attr, &wa, sizeof(q.attr)) == 0
                           : all_zero(&q.attr, sizeof(q.attr)));
    for (size_t i = 0; ok && i < w->n; i++) {
        ok = c->in[i + 1].base == dv[i] && c->in[i + 1].len == dl[i];
    }
    if (ok && w->out == O_BUF) {
        ok = c->out[1].base == obuf && c->out[1].len == sizeof(obuf);
    }
    if (ok && w->out == O_ATTR) {
        ok = c->out[1].len == sizeof(struct psa_crypto_attr);
    }
    return ok;
}

void test_client_ids(void)
{
    struct psa_crypto_rsp rsp = { 0, 0, 0, 0 };
    char what[64];

    memset(&g_attr, 0, sizeof(g_attr));
    psa_set_key_type(&g_attr, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&g_attr, 256);
    psa_set_key_usage_flags(&g_attr, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&g_attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    reply(PSA_SUCCESS, &rsp, sizeof(rsp), 0);
    call_all();
    t_check(g_n == N_WANTS, "ids: one psa_call per 2.28 client id");
    t_check(N_WANTS == CRYPTO_FN_GENERATE_RANDOM - 10u,
            "ids: table covers ids 1..63 minus 10 AEAD multipart");
    for (size_t i = 0; i < N_WANTS && i < g_n; i++) {
        snprintf(what, sizeof(what), "ids: %s packing", wants[i].name);
        t_check(row_ok(&g_log[i], &wants[i]), what);
    }
}
