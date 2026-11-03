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
 * Host test of svc_crypto.c on the mbedtls PSA core built with the
 * SPE config: RPC checks, operation slots, key ownership and the
 * client reset; family vectors are in test_svc_crypto_family.c.
 */
#include <string.h>

#include "its_ram.h"
#include "its_store.h"
#include "psa_ipro/crypto_lib.h"
#include "spm_heap.h"
#include "svc_crypto.h"
#include "svc_test.h"

static void test_vectors(void)
{
    uint8_t rq[sizeof(struct psa_crypto_req)];
    struct psa_crypto_req r = t_req(0, PSA_ALG_SHA_256, 0);
    struct psa_crypto_rsp rsp;
    uint8_t h[32];
    struct buf in[3] = { { rq, sizeof(rq) }, { "abc", 3 }, { "x", 1 } };
    struct buf out[3] = { { &rsp, sizeof(rsp) }, { h, 32 }, { h, 1 } };
    static const int32_t bad_ids[3] = { 0, CRYPTO_FN_LAST + 1, -1 };
    static const size_t counts[5][2] = {
        { 1, 2 }, { 3, 2 }, { 2, 1 }, { 2, 3 }, { 2, 0 },
    };
    int bad = 0;

    memcpy(rq, &r, sizeof(r));
    for (size_t i = 0; i < 3; i++) {
        out[0].n = sizeof(rsp);
        out[1].n = 32;
        bad |= t_raw(bad_ids[i], in, 2, out, 2) != PSA_ERROR_NOT_SUPPORTED;
    }
    t_check(!bad, "ids 0, CRYPTO_FN_LAST + 1, -1: NOT_SUPPORTED");

    bad = 0;
    for (size_t i = 0; i < 5; i++) {
        out[0].n = sizeof(rsp);
        out[1].n = 32;
        bad |= t_raw(CRYPTO_FN_HASH_COMPUTE, in, counts[i][0], out,
                     counts[i][1]) != PSA_ERROR_INVALID_ARGUMENT;
    }
    t_check(!bad, "vector count other than the id's: INVALID_ARGUMENT");

    bad = 0;
    in[0].n = sizeof(rq) - 1;
    out[0].n = sizeof(rsp);
    out[1].n = 32;
    bad |= t_raw(CRYPTO_FN_HASH_COMPUTE, in, 2, out, 2) !=
           PSA_ERROR_INVALID_ARGUMENT;
    in[0].n = sizeof(rq);
    out[0].n = sizeof(rsp) - 1;
    out[1].n = 32;
    bad |= t_raw(CRYPTO_FN_HASH_COMPUTE, in, 2, out, 2) !=
           PSA_ERROR_INVALID_ARGUMENT;
    out[0].n = sizeof(rsp) + 1;
    out[1].n = 32;
    bad |= t_raw(CRYPTO_FN_HASH_COMPUTE, in, 2, out, 2) !=
           PSA_ERROR_INVALID_ARGUMENT;
    t_check(!bad, "req or rsp vector of the wrong size: INVALID_ARGUMENT");

    out[0].n = sizeof(rsp);
    out[1].n = 32;
    t_check(t_raw(CRYPTO_FN_HASH_COMPUTE, in, 2, out, 2) == PSA_SUCCESS &&
            out[1].n == 32 && rsp.length == 32 &&
            memcmp(h, abc_sha256, 32) == 0, "raw SHA-256(\"abc\")");
#if !PSA_IPRO_AEAD_MULTI
    t_check(t_raw(CRYPTO_FN_AEAD_UPDATE, in, 2, out, 2) ==
            PSA_ERROR_NOT_SUPPORTED, "multipart AEAD: NOT_SUPPORTED");
#endif
#if !PSA_IPRO_KD_INTEGER
    t_check(t_raw(CRYPTO_FN_KD_INPUT_INTEGER, in, 2, out, 2) ==
            PSA_ERROR_NOT_SUPPORTED, "KD_INPUT_INTEGER: NOT_SUPPORTED");
#endif
#if !PSA_IPRO_API_1_2
    for (int32_t fn = CRYPTO_FN_KEY_AGREEMENT; fn <= CRYPTO_FN_LAST;
         fn++) {
        t_check(t_raw(fn, in, 2, out, 2) == PSA_ERROR_NOT_SUPPORTED,
                "API 1.2 id: NOT_SUPPORTED");
    }
#endif
}

static void test_reserved(void)
{
    struct psa_crypto_req r = t_req(0, PSA_ALG_SHA_256, 0);
    struct psa_crypto_rsp rsp;
    struct buf abc = { "abc", 3 };
    uint8_t h[32];
    size_t n = 32;
    int bad = 0;

    r.aux2 = 1;
    bad |= t_rpc(CRYPTO_FN_HASH_COMPUTE, &r, &abc, 1, h, &n, NULL) !=
           PSA_ERROR_INVALID_ARGUMENT;
    r.aux2 = 0;
    r.step = 1;
    n = 32;
    bad |= t_rpc(CRYPTO_FN_HASH_COMPUTE, &r, &abc, 1, h, &n, NULL) !=
           PSA_ERROR_INVALID_ARGUMENT;
    r.step = 0;
    r.aux = 1;
    n = 32;
    bad |= t_rpc(CRYPTO_FN_HASH_COMPUTE, &r, &abc, 1, h, &n, NULL) !=
           PSA_ERROR_INVALID_ARGUMENT;
    t_check(!bad, "aux2, unused step, unused aux != 0: INVALID_ARGUMENT");

    r = t_req(0, PSA_ALG_HKDF(PSA_ALG_SHA_256), 0);
    t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r = t_req(0, 0, rsp.op_handle);
    r.step = PSA_KEY_DERIVATION_INPUT_SALT;
    r.aux = 5;
    bad = t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &abc, 1, NULL, NULL, NULL) !=
          PSA_ERROR_INVALID_ARGUMENT;
    r.step = 1;
    r.aux = 10;
    bad |= t_rpc(CRYPTO_FN_KD_SET_CAPACITY, &r, NULL, 0, NULL, NULL,
                 NULL) != PSA_ERROR_INVALID_ARGUMENT;
    r.step = PSA_KEY_DERIVATION_INPUT_SALT;
    r.aux = 0;
    bad |= t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &abc, 1, NULL, NULL,
                 NULL) != PSA_SUCCESS;
    t_check(!bad && t_op0(CRYPTO_FN_KD_ABORT, r.op_handle) == PSA_SUCCESS,
            "KD: aux on INPUT_BYTES, step on SET_CAPACITY refused");
}

static void test_slots(void)
{
    struct psa_crypto_req r = t_req(0, PSA_ALG_SHA_256, 0);
    struct buf a = { "a", 1 };
    struct buf bad = { abc_sha256, 31 };
    uint32_t h[SLOTS];
    uint8_t d[32];
    size_t n = 31;
    int ok = 1;

    t_check(t_op1(CRYPTO_FN_HASH_UPDATE, 0) == PSA_ERROR_BAD_STATE,
            "handle 0: BAD_STATE");
    t_check(t_op1(CRYPTO_FN_HASH_UPDATE, SLOTS + 1) ==
            PSA_ERROR_INVALID_HANDLE &&
            t_op0(CRYPTO_FN_HASH_ABORT, 0xFFFFFFFFu) ==
            PSA_ERROR_INVALID_HANDLE, "handle > 8: INVALID_HANDLE");
    t_check(t_op0(CRYPTO_FN_HASH_ABORT, 0) == PSA_SUCCESS &&
            t_op0(CRYPTO_FN_KD_ABORT, 0) == PSA_SUCCESS,
            "ABORT of handle 0: SUCCESS");
    r.op_handle = 3;
    t_check(t_rpc(CRYPTO_FN_HASH_SETUP, &r, NULL, 0, NULL, NULL, NULL) ==
            PSA_ERROR_BAD_STATE, "SETUP with a handle set: BAD_STATE");

    for (size_t i = 0; i < SLOTS; i++) {
        h[i] = t_hash_setup();
        ok &= h[i] != 0;
    }
    t_check(ok && t_hash_setup() == 0, "8 slots, the 9th SETUP fails");
    r.op_handle = 0;
    t_check(t_rpc(CRYPTO_FN_HASH_SETUP, &r, NULL, 0, NULL, NULL, NULL) ==
            PSA_ERROR_INSUFFICIENT_MEMORY, "full: INSUFFICIENT_MEMORY");
    t_check(t_op1(CRYPTO_FN_MAC_UPDATE, h[0]) == PSA_ERROR_INVALID_HANDLE &&
            t_op0(CRYPTO_FN_KD_ABORT, h[0]) == PSA_ERROR_INVALID_HANDLE,
            "hash handle in a MAC or KD call: INVALID_HANDLE");
    for (size_t i = 0; i < SLOTS; i++) {
        t_op0(CRYPTO_FN_HASH_ABORT, h[i]);
    }
    t_check(t_free_slots() == SLOTS, "ABORT frees");

    r.op_handle = t_hash_setup();
    t_rpc(CRYPTO_FN_HASH_UPDATE, &r, &a, 1, NULL, NULL, NULL);
    t_check(t_rpc(CRYPTO_FN_HASH_FINISH, &r, NULL, 0, d, &n, NULL) ==
            PSA_ERROR_BUFFER_TOO_SMALL && t_free_slots() == SLOTS,
            "FINISH failure frees the slot");
    r.op_handle = t_hash_setup();
    t_check(t_rpc(CRYPTO_FN_HASH_VERIFY, &r, &bad, 1, NULL, NULL, NULL) !=
            PSA_SUCCESS && t_free_slots() == SLOTS,
            "VERIFY failure frees the slot");
    r.op_handle = 0;
    r.alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
    t_check(t_rpc(CRYPTO_FN_HASH_SETUP, &r, NULL, 0, NULL, NULL, NULL) !=
            PSA_SUCCESS && t_free_slots() == SLOTS,
            "SETUP failure frees the slot");
}

static void test_keys(void)
{
    struct psa_crypto_req r = t_req(0, 0, 0);
    struct psa_crypto_rsp rsp;
    struct psa_crypto_attr got;
    uint8_t rnd[32] = { 0 };
    uint8_t out[64];
    size_t n = 32;
    size_t flen = 0;
    uint32_t k;
    struct buf d[2] = { { aes_key, 16 }, { NULL, 0 } };

    t_check(t_rpc(CRYPTO_FN_GENERATE_RANDOM, &r, NULL, 0, rnd, &n, &rsp) ==
            PSA_SUCCESS && n == 32 && rsp.length == 32 &&
            memcmp(rnd, (uint8_t[32]){ 0 }, 32) != 0,
            "GENERATE_RANDOM 32");
    n = 0;
    t_check(t_rpc(CRYPTO_FN_GENERATE_RANDOM, &r, NULL, 0, rnd, &n, NULL) ==
            PSA_SUCCESS && n == 0, "GENERATE_RANDOM 0");

    k = t_import(PSA_KEY_TYPE_AES, 128, PSA_KEY_USAGE_ENCRYPT,
                 PSA_ALG_ECB_NO_PADDING, aes_key, 16);
    r.key_id = k;
    n = sizeof(got);
    memset(&got, 0xEE, sizeof(got));
    t_check(t_rpc(CRYPTO_FN_KEY_GET_ATTRIBUTES, &r, NULL, 0, &got, &n,
                  NULL) == PSA_SUCCESS && n == sizeof(got) &&
            got.type == PSA_KEY_TYPE_AES && got.bits == 128 &&
            got.id == k && got.lifetime == PSA_KEY_LIFETIME_VOLATILE &&
            got.usage == PSA_KEY_USAGE_ENCRYPT &&
            got.alg == PSA_ALG_ECB_NO_PADDING && got.alg2 == 0,
            "KEY_GET_ATTRIBUTES");
    n = sizeof(got) - 1;
    t_check(t_rpc(CRYPTO_FN_KEY_GET_ATTRIBUTES, &r, NULL, 0, &got, &n,
                  NULL) == PSA_ERROR_INVALID_ARGUMENT,
            "attributes buffer size checked");
    t_check(t_rpc(CRYPTO_FN_KEY_PURGE, &r, NULL, 0, NULL, NULL, NULL) ==
            PSA_SUCCESS && t_destroy(k) == PSA_SUCCESS,
            "KEY_PURGE volatile");

    r = t_req(0, 0, 0);
    t_attr_id(&r.attr, 0x10u);
    r.attr.type = (PSA_KEY_TYPE_AES);
    r.attr.bits = (128);
    r.attr.usage = (PSA_KEY_USAGE_ENCRYPT);
    t_check(t_rpc(CRYPTO_FN_KEY_IMPORT, &r, d, 1, NULL, NULL, &rsp) ==
            PSA_SUCCESS && rsp.key_id == 0x10u &&
            its_ram_data("/its/k_0000000000000010", &flen) != NULL &&
            t_destroy(0x10u) == PSA_SUCCESS &&
            its_ram_data("/its/k_0000000000000010", &flen) == NULL,
            "persistent import in ITS k_ file, destroy removes it");

    r = t_req(0, 0, 0);
    r.attr.type = (PSA_KEY_TYPE_AES);
    r.attr.bits = (128);
    r.attr.usage = (PSA_KEY_USAGE_ENCRYPT);
    r.attr.id = 0x11u;
    t_check(t_rpc(CRYPTO_FN_KEY_IMPORT, &r, d, 1, NULL, NULL, &rsp) ==
            PSA_SUCCESS && rsp.key_id != 0x11u &&
            its_ram_data("/its/k_0000000000000011", &flen) == NULL &&
            t_destroy(rsp.key_id) == PSA_SUCCESS,
            "attr.id with a volatile lifetime: a volatile key");

    k = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_ENCRYPT |
                   PSA_KEY_USAGE_DECRYPT, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    r = t_req(k, PSA_ALG_ECDSA(PSA_ALG_SHA_256), 0);
    d[0].p = abc_sha256;
    d[0].n = 32;
    n = sizeof(out);
    t_check(t_rpc(CRYPTO_FN_ASYM_ENCRYPT, &r, d, 2, out, &n, NULL) !=
            PSA_SUCCESS && n == 0, "ASYM_ENCRYPT, ECC key: refused");
    n = sizeof(out);
    t_check(t_rpc(CRYPTO_FN_ASYM_DECRYPT, &r, d, 2, out, &n, NULL) !=
            PSA_SUCCESS && n == 0, "ASYM_DECRYPT, ECC key: refused");
    t_destroy(k);
}

/* every id that reads req.key_id: count of data inputs, output */
static const struct {
    int32_t fn;
    uint8_t n_in;
    uint8_t out;
} k_in_ids[] = {
    { CRYPTO_FN_KEY_DESTROY, 0, 0 },
    { CRYPTO_FN_KEY_GET_ATTRIBUTES, 0, 1 },
    { CRYPTO_FN_KEY_EXPORT, 0, 1 },
    { CRYPTO_FN_KEY_EXPORT_PUBLIC, 0, 1 },
    { CRYPTO_FN_KEY_COPY, 0, 0 },
    { CRYPTO_FN_KEY_PURGE, 0, 0 },
    { CRYPTO_FN_MAC_COMPUTE, 1, 1 },
    { CRYPTO_FN_MAC_VERIFY, 2, 0 },
    { CRYPTO_FN_MAC_SIGN_SETUP, 0, 0 },
    { CRYPTO_FN_MAC_VERIFY_SETUP, 0, 0 },
    { CRYPTO_FN_CIPHER_ENCRYPT, 1, 1 },
    { CRYPTO_FN_CIPHER_DECRYPT, 1, 1 },
    { CRYPTO_FN_CIPHER_ENCRYPT_SETUP, 0, 0 },
    { CRYPTO_FN_CIPHER_DECRYPT_SETUP, 0, 0 },
    { CRYPTO_FN_AEAD_ENCRYPT, 3, 1 },
    { CRYPTO_FN_AEAD_DECRYPT, 3, 1 },
    { CRYPTO_FN_SIGN_HASH, 1, 1 },
    { CRYPTO_FN_VERIFY_HASH, 2, 0 },
    { CRYPTO_FN_SIGN_MESSAGE, 1, 1 },
    { CRYPTO_FN_VERIFY_MESSAGE, 2, 0 },
    { CRYPTO_FN_ASYM_ENCRYPT, 2, 1 },
    { CRYPTO_FN_ASYM_DECRYPT, 2, 1 },
    { CRYPTO_FN_KD_INPUT_KEY, 0, 0 },
    { CRYPTO_FN_KD_KEY_AGREEMENT, 1, 0 },
    { CRYPTO_FN_RAW_KEY_AGREEMENT, 1, 1 },
};

/*
 * The key exists, so INVALID_HANDLE can only come from the ownership
 * check (mbedtls answers policy or type misuse with other codes).
 */
static int refused_everywhere(uint32_t key, uint32_t kd)
{
    static const uint8_t zero[64];
    struct buf in[3] = { { zero, 16 }, { zero, 16 }, { zero, 16 } };
    uint8_t out[64];
    int refused = 0;

    for (size_t i = 0; i < sizeof(k_in_ids) / sizeof(k_in_ids[0]); i++) {
        struct psa_crypto_req r = t_req(key, PSA_ALG_SHA_256, 0);
        size_t n = sizeof(out);
        int32_t fn = k_in_ids[i].fn;
        psa_status_t st;

        if (fn == CRYPTO_FN_KD_INPUT_KEY ||
            fn == CRYPTO_FN_KD_KEY_AGREEMENT) {
            r.op_handle = kd;
            r.step = PSA_KEY_DERIVATION_INPUT_SECRET;
        }
        st = t_rpc(fn, &r, in, k_in_ids[i].n_in, out,
                   k_in_ids[i].out ? &n : NULL, NULL);
        if (st == PSA_ERROR_INVALID_HANDLE) {
            refused++;
        } else {
            t_info("id %d with SPE key: status %d", (int)fn, (int)st);
        }
    }
    return refused == (int)(sizeof(k_in_ids) / sizeof(k_in_ids[0]));
}

static void test_ownership(void)
{
    psa_key_attributes_t at = psa_key_attributes_init();
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;
    struct psa_crypto_req r = t_req(0, PSA_ALG_HKDF(PSA_ALG_SHA_256), 0);
    struct psa_crypto_rsp rsp;
    mbedtls_svc_key_id_t spe;
    mbedtls_svc_key_id_t again;
    uint8_t out[16];
    size_t n;
    uint32_t k;

    psa_set_key_type(&at, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&at, 128);
    psa_set_key_usage_flags(&at, PSA_KEY_USAGE_ENCRYPT |
                            PSA_KEY_USAGE_EXPORT | PSA_KEY_USAGE_COPY);
    psa_set_key_algorithm(&at, PSA_ALG_ECB_NO_PADDING);
    t_check(psa_import_key(&at, aes_key, 16, &spe) == PSA_SUCCESS,
            "SPE imports its own key");
    t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    t_check(refused_everywhere(spe, rsp.op_handle),
            "SPE key: all 25 key-reading ids INVALID_HANDLE");
    t_check(t_op0(CRYPTO_FN_KD_ABORT, rsp.op_handle) == PSA_SUCCESS &&
            t_free_slots() == SLOTS, "refusals left no slot behind");
    t_check(psa_cipher_encrypt(spe, PSA_ALG_ECB_NO_PADDING, aes_pt, 16, out,
                               16, &n) == PSA_SUCCESS &&
            memcmp(out, aes_ct, 16) == 0 && psa_destroy_key(spe) ==
            PSA_SUCCESS, "SPE key still works inside the SPE");
    t_check(t_destroy(0) == PSA_ERROR_INVALID_HANDLE &&
            t_destroy(0x40000000u) == PSA_ERROR_INVALID_HANDLE,
            "key 0 and vendor-range ids: INVALID_HANDLE");

    k = t_import(PSA_KEY_TYPE_AES, 128, PSA_KEY_USAGE_ENCRYPT,
                 PSA_ALG_ECB_NO_PADDING, aes_key, 16);
    t_destroy(k);
    t_check(psa_import_key(&at, aes_key, 16, &again) == PSA_SUCCESS &&
            again == k && refused_everywhere(again, 0) &&
            psa_destroy_key(again) == PSA_SUCCESS,
            "SPE key on a destroyed client key's id: refused");

    k = t_import(PSA_KEY_TYPE_AES, 128, PSA_KEY_USAGE_ENCRYPT,
                 PSA_ALG_ECB_NO_PADDING, aes_key, 16);
    psa_destroy_key(k);
    t_check(t_destroy(k) != PSA_SUCCESS &&
            psa_import_key(&at, aes_key, 16, &again) == PSA_SUCCESS &&
            again == k && refused_everywhere(again, 0) &&
            psa_get_key_attributes(again, &a) == PSA_SUCCESS &&
            psa_destroy_key(again) == PSA_SUCCESS,
            "failed KEY_DESTROY of a gone key drops its id");
    psa_reset_key_attributes(&a);
}

static void test_reset(size_t base)
{
    psa_key_attributes_t at = psa_key_attributes_init();
    psa_key_attributes_t a = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t spe;
    uint32_t h = t_hash_setup();
    uint32_t k = t_import(PSA_KEY_TYPE_AES, 128, PSA_KEY_USAGE_ENCRYPT,
                          PSA_ALG_CTR, aes_key, 16);
    struct psa_crypto_req r = t_req(k, PSA_ALG_CTR, 0);
    struct psa_crypto_rsp rsp;
    size_t used;
    size_t peak;

    psa_set_key_type(&at, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&at, 128);
    psa_set_key_usage_flags(&at, PSA_KEY_USAGE_ENCRYPT);
    psa_import_key(&at, aes_key, 16, &spe);
    t_rpc(CRYPTO_FN_CIPHER_ENCRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    t_check(h != 0 && k != 0 && rsp.op_handle != 0 &&
            t_free_slots() == SLOTS - 2, "before reset: 2 slots, a key");

    svc_crypto_client_reset();
    t_check(t_free_slots() == SLOTS, "client reset frees every slot");
    t_check(t_op1(CRYPTO_FN_HASH_UPDATE, h) == PSA_ERROR_INVALID_HANDLE &&
            t_op0(CRYPTO_FN_CIPHER_ABORT, rsp.op_handle) ==
            PSA_ERROR_INVALID_HANDLE, "old handles refused");
    t_check(psa_get_key_attributes(k, &a) == PSA_ERROR_INVALID_HANDLE &&
            t_destroy(k) == PSA_ERROR_INVALID_HANDLE,
            "client key destroyed and refused");
    psa_reset_key_attributes(&a);
    t_check(psa_get_key_attributes(spe, &a) == PSA_SUCCESS &&
            psa_destroy_key(spe) == PSA_SUCCESS, "SPE key kept");
    psa_reset_key_attributes(&a);
    spm_heap_stats(&used, &peak);
    t_check(used == base, "heap back to its post-init use");
}

int main(void)
{
    static const uint8_t its_key[ITS_KEY_SIZE] = { 1 };
    static const uint8_t its_rand[ITS_RAND_SIZE] = { 2 };
    size_t base;
    size_t used;
    size_t peak;

    t_check(its_store_init(&its_ram, its_key, its_rand) == PSA_SUCCESS &&
            svc_crypto_init() == PSA_SUCCESS, "its_store, svc_crypto init");
    spm_heap_stats(&base, &peak);
    test_vectors();
    test_reserved();
    test_hash();
    test_slots();
    test_cipher();
    test_mac();
    test_aead();
    test_ecc();
    test_kd();
    test_api12();
    test_keys();
    test_ownership();
    test_reset(base);
    spm_heap_stats(&used, &peak);
    t_info("heap: %zu B after init, peak %zu B of %u", base, peak,
           SPM_HEAP_SIZE);
    t_info("%d calls", t_calls());
    t_check(used == base, "every key destroyed: heap back to its init use");
    t_check(t_free_slots() == SLOTS, "every operation slot free");
    t_check(t_contract_bad() == 0, "out contract held on every call");
    return t_summary();
}
