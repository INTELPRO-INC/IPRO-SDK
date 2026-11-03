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
 * svc_crypto known-answer cases per family: hash, cipher, MAC, AEAD,
 * ECDSA, key derivation and agreement, all through the RPC.
 */
#include <string.h>

#include "psa_ipro/crypto_lib.h"
#include "svc_test.h"

void test_hash(void)
{
    struct psa_crypto_req r = t_req(0, PSA_ALG_SHA_256, 0);
    struct psa_crypto_rsp rsp;
    struct buf abc = { "abc", 3 };
    struct buf a = { "a", 1 };
    struct buf bc = { "bc", 2 };
    struct buf both[2] = { { "abc", 3 }, { abc_sha256, 32 } };
    struct buf hv = { abc_sha256, 32 };
    uint8_t h[32];
    size_t n = 32;
    uint32_t h1;
    uint32_t h2;

    t_check(t_op0(CRYPTO_FN_INIT, 0) == PSA_SUCCESS, "INIT again: SUCCESS");
    t_check(t_rpc(CRYPTO_FN_HASH_COMPUTE, &r, &abc, 1, h, &n, &rsp) ==
            PSA_SUCCESS && n == 32 && memcmp(h, abc_sha256, 32) == 0,
            "HASH_COMPUTE SHA-256(\"abc\")");
    n = 31;
    t_check(t_rpc(CRYPTO_FN_HASH_COMPUTE, &r, &abc, 1, h, &n, &rsp) ==
            PSA_ERROR_BUFFER_TOO_SMALL && n == 0,
            "HASH_COMPUTE 31-byte buffer: BUFFER_TOO_SMALL, len 0");
    t_check(t_rpc(CRYPTO_FN_HASH_COMPARE, &r, both, 2, NULL, NULL, NULL) ==
            PSA_SUCCESS, "HASH_COMPARE match");
    both[0].n = 2;
    t_check(t_rpc(CRYPTO_FN_HASH_COMPARE, &r, both, 2, NULL, NULL, NULL) ==
            PSA_ERROR_INVALID_SIGNATURE, "HASH_COMPARE mismatch");

    h1 = t_hash_setup();
    r.op_handle = h1;
    t_check(h1 >= 1 && h1 <= SLOTS &&
            t_rpc(CRYPTO_FN_HASH_UPDATE, &r, &a, 1, NULL, NULL, NULL) ==
            PSA_SUCCESS, "HASH_SETUP handle in 1..8, UPDATE");
    t_check(t_rpc(CRYPTO_FN_HASH_CLONE, &r, NULL, 0, NULL, NULL, &rsp) ==
            PSA_SUCCESS && rsp.op_handle != 0 && rsp.op_handle != h1,
            "HASH_CLONE returns a second handle");
    h2 = rsp.op_handle;
    t_rpc(CRYPTO_FN_HASH_UPDATE, &r, &bc, 1, NULL, NULL, NULL);
    n = 32;
    memset(h, 0, sizeof(h));
    t_check(t_rpc(CRYPTO_FN_HASH_FINISH, &r, NULL, 0, h, &n, &rsp) ==
            PSA_SUCCESS && n == 32 && rsp.length == 32 &&
            memcmp(h, abc_sha256, 32) == 0, "multipart SHA-256 FINISH");
    r.op_handle = h2;
    t_rpc(CRYPTO_FN_HASH_UPDATE, &r, &bc, 1, NULL, NULL, NULL);
    t_check(t_rpc(CRYPTO_FN_HASH_VERIFY, &r, &hv, 1, NULL, NULL, NULL) ==
            PSA_SUCCESS, "clone continues independently, VERIFY");
    t_check(t_free_slots() == SLOTS, "FINISH and VERIFY freed both slots");
    r.op_handle = h1;
    t_check(t_rpc(CRYPTO_FN_HASH_UPDATE, &r, &bc, 1, NULL, NULL, NULL) ==
            PSA_ERROR_INVALID_HANDLE, "ended handle: INVALID_HANDLE");
}

void test_cipher(void)
{
    const psa_key_usage_t ed = PSA_KEY_USAGE_ENCRYPT |
                               PSA_KEY_USAGE_DECRYPT;
    uint32_t ecb = t_import(PSA_KEY_TYPE_AES, 128, ed | PSA_KEY_USAGE_EXPORT |
                            PSA_KEY_USAGE_COPY, PSA_ALG_ECB_NO_PADDING,
                            aes_key, 16);
    uint32_t ctr = t_import(PSA_KEY_TYPE_AES, 128, ed, PSA_ALG_CTR,
                            aes_key, 16);
    struct psa_crypto_req r = t_req(ecb, PSA_ALG_ECB_NO_PADDING, 0);
    struct psa_crypto_rsp rsp;
    struct buf pt = { aes_pt, 16 };
    struct buf ct = { aes_ct, 16 };
    uint8_t msg[20];
    uint8_t enc[40];
    uint8_t dec[40];
    uint8_t iv[16];
    size_t n = 16;
    size_t e = 0;
    size_t d = 0;
    struct buf b;

    t_check(ecb != 0 && ctr != 0 && ecb != ctr, "KEY_IMPORT two AES keys");
    t_check(t_rpc(CRYPTO_FN_CIPHER_ENCRYPT, &r, &pt, 1, enc, &n, NULL) ==
            PSA_SUCCESS && n == 16 && memcmp(enc, aes_ct, 16) == 0,
            "CIPHER_ENCRYPT AES-128 ECB FIPS-197");
    n = 16;
    t_check(t_rpc(CRYPTO_FN_CIPHER_DECRYPT, &r, &ct, 1, dec, &n, NULL) ==
            PSA_SUCCESS && n == 16 && memcmp(dec, aes_pt, 16) == 0,
            "CIPHER_DECRYPT AES-128 ECB FIPS-197");
    n = 16;
    t_check(t_rpc(CRYPTO_FN_KEY_EXPORT, &r, NULL, 0, dec, &n, NULL) ==
            PSA_SUCCESS && n == 16 && memcmp(dec, aes_key, 16) == 0,
            "KEY_EXPORT");
    r.attr.usage = (PSA_KEY_USAGE_ENCRYPT);
    r.attr.alg = (PSA_ALG_ECB_NO_PADDING);
    t_check(t_rpc(CRYPTO_FN_KEY_COPY, &r, NULL, 0, NULL, NULL, &rsp) ==
            PSA_SUCCESS && rsp.key_id != 0 && rsp.key_id != ecb,
            "KEY_COPY gives a new key");
    r = t_req(rsp.key_id, PSA_ALG_ECB_NO_PADDING, 0);
    n = 16;
    t_check(t_rpc(CRYPTO_FN_CIPHER_ENCRYPT, &r, &pt, 1, enc, &n, NULL) ==
            PSA_SUCCESS && memcmp(enc, aes_ct, 16) == 0 &&
            t_destroy(rsp.key_id) == PSA_SUCCESS, "copy usable, destroyed");

    for (size_t i = 0; i < sizeof(msg); i++) {
        msg[i] = (uint8_t)(i * 13u);
    }
    r = t_req(ctr, PSA_ALG_CTR, 0);
    t_rpc(CRYPTO_FN_CIPHER_ENCRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    b.p = msg;
    b.n = sizeof(msg);
    n = 40;
    t_check(t_rpc(CRYPTO_FN_CIPHER_UPDATE, &r, &b, 1, enc, &n, NULL) ==
            PSA_ERROR_BAD_STATE && t_free_slots() == SLOTS - 1,
            "UPDATE before the IV: BAD_STATE, slot kept");
    t_check(t_op0(CRYPTO_FN_CIPHER_ABORT, r.op_handle) == PSA_SUCCESS &&
            t_free_slots() == SLOTS, "CIPHER_ABORT frees it");

    r = t_req(ctr, PSA_ALG_CTR, 0);
    t_rpc(CRYPTO_FN_CIPHER_ENCRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    n = 16;
    t_check(t_rpc(CRYPTO_FN_CIPHER_GENERATE_IV, &r, NULL, 0, iv, &n,
                  NULL) == PSA_SUCCESS && n == 16,
            "CIPHER_GENERATE_IV 16 bytes");
    n = 40;
    t_rpc(CRYPTO_FN_CIPHER_UPDATE, &r, &b, 1, enc, &n, NULL);
    e = n;
    n = 40 - e;
    t_check(t_rpc(CRYPTO_FN_CIPHER_FINISH, &r, NULL, 0, enc + e, &n,
                  NULL) == PSA_SUCCESS && e + n == sizeof(msg),
            "CTR encrypt 20 bytes");
    e += n;

    r = t_req(ctr, PSA_ALG_CTR, 0);
    t_rpc(CRYPTO_FN_CIPHER_DECRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    b.p = iv;
    b.n = 16;
    t_rpc(CRYPTO_FN_CIPHER_SET_IV, &r, &b, 1, NULL, NULL, NULL);
    b.p = enc;
    b.n = e;
    n = 40;
    t_rpc(CRYPTO_FN_CIPHER_UPDATE, &r, &b, 1, dec, &n, NULL);
    d = n;
    n = 40 - d;
    t_check(t_rpc(CRYPTO_FN_CIPHER_FINISH, &r, NULL, 0, dec + d, &n,
                  NULL) == PSA_SUCCESS && d + n == sizeof(msg) &&
            memcmp(dec, msg, sizeof(msg)) == 0 && t_free_slots() == SLOTS,
            "CTR decrypt round trip, slots free");
    t_check(t_destroy(ecb) == PSA_SUCCESS && t_destroy(ctr) == PSA_SUCCESS,
            "KEY_DESTROY");
    r = t_req(ecb, PSA_ALG_ECB_NO_PADDING, 0);
    n = 16;
    t_check(t_rpc(CRYPTO_FN_CIPHER_ENCRYPT, &r, &pt, 1, enc, &n, NULL) ==
            PSA_ERROR_INVALID_HANDLE, "destroyed key: INVALID_HANDLE");
}

void test_mac(void)
{
    static const uint8_t key[20] = {
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    };
    static const uint8_t tag[32] = {
        0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53, 0x5c, 0xa8, 0xaf,
        0xce, 0xaf, 0x0b, 0xf1, 0x2b, 0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83,
        0x3d, 0xa7, 0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7,
    };
    const psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
    uint32_t k = t_import(PSA_KEY_TYPE_HMAC, 160,
                          PSA_KEY_USAGE_SIGN_MESSAGE |
                          PSA_KEY_USAGE_VERIFY_MESSAGE, alg, key, 20);
    struct psa_crypto_req r = t_req(k, alg, 0);
    struct psa_crypto_rsp rsp;
    struct buf msg = { "Hi There", 8 };
    struct buf both[2] = { { "Hi There", 8 }, { tag, 32 } };
    struct buf t = { tag, 32 };
    uint8_t m[32];
    size_t n = 32;

    t_check(k != 0 && t_rpc(CRYPTO_FN_MAC_COMPUTE, &r, &msg, 1, m, &n,
                            NULL) == PSA_SUCCESS &&
            n == 32 && memcmp(m, tag, 32) == 0,
            "MAC_COMPUTE HMAC-SHA-256 RFC 4231 case 1");
    t_check(t_rpc(CRYPTO_FN_MAC_VERIFY, &r, both, 2, NULL, NULL, NULL) ==
            PSA_SUCCESS, "MAC_VERIFY");
    t_rpc(CRYPTO_FN_MAC_SIGN_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    t_rpc(CRYPTO_FN_MAC_UPDATE, &r, &msg, 1, NULL, NULL, NULL);
    n = 32;
    memset(m, 0, sizeof(m));
    t_check(t_rpc(CRYPTO_FN_MAC_SIGN_FINISH, &r, NULL, 0, m, &n, NULL) ==
            PSA_SUCCESS && n == 32 && memcmp(m, tag, 32) == 0,
            "multipart MAC sign");
    r.op_handle = 0;
    t_rpc(CRYPTO_FN_MAC_VERIFY_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    msg.n = 7;
    t_rpc(CRYPTO_FN_MAC_UPDATE, &r, &msg, 1, NULL, NULL, NULL);
    t_check(t_rpc(CRYPTO_FN_MAC_VERIFY_FINISH, &r, &t, 1, NULL, NULL,
                  NULL) == PSA_ERROR_INVALID_SIGNATURE &&
            t_free_slots() == SLOTS,
            "MAC_VERIFY_FINISH mismatch: INVALID_SIGNATURE, slot freed");
    r.op_handle = 0;
    t_rpc(CRYPTO_FN_MAC_SIGN_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    t_check(t_op0(CRYPTO_FN_MAC_ABORT, rsp.op_handle) == PSA_SUCCESS &&
            t_free_slots() == SLOTS, "MAC_ABORT");
    t_destroy(k);
}

#if PSA_IPRO_AEAD_MULTI
/* GCM through the multipart ids; ref = one-shot ciphertext || tag */
static void aead_multi(uint32_t k, const uint8_t ref[32])
{
    struct psa_crypto_req r = t_req(k, PSA_ALG_GCM, 0);
    struct psa_crypto_rsp rsp;
    uint8_t nonce[12] = { 1, 2, 3 };
    uint8_t ct[32];
    uint8_t pt[16];
    size_t n;
    size_t c;
    struct buf b = { nonce, 12 };
    struct buf hdr = { "hdr", 3 };

    t_rpc(CRYPTO_FN_AEAD_ENCRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    t_rpc(CRYPTO_FN_AEAD_SET_NONCE, &r, &b, 1, NULL, NULL, NULL);
    t_rpc(CRYPTO_FN_AEAD_UPDATE_AD, &r, &hdr, 1, NULL, NULL, NULL);
    b.p = aes_pt;
    b.n = 16;
    c = 16;
    t_rpc(CRYPTO_FN_AEAD_UPDATE, &r, &b, 1, ct, &c, NULL);
    r.aux = 16 - (uint32_t)c;
    n = r.aux + 16u;
    t_check(t_rpc(CRYPTO_FN_AEAD_FINISH, &r, NULL, 0, ct + c, &n, &rsp) ==
            PSA_SUCCESS && c + rsp.length2 == 16 && n == r.aux + 16u &&
            memcmp(ct, ref, 16) == 0 && memcmp(ct + 16, ref + 16, 16) == 0 &&
            t_free_slots() == SLOTS,
            "AEAD multipart encrypt == one-shot, slot freed");

    r = t_req(k, PSA_ALG_GCM, 0);
    t_rpc(CRYPTO_FN_AEAD_DECRYPT_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    b.p = nonce;
    b.n = 12;
    t_rpc(CRYPTO_FN_AEAD_SET_NONCE, &r, &b, 1, NULL, NULL, NULL);
    t_rpc(CRYPTO_FN_AEAD_UPDATE_AD, &r, &hdr, 1, NULL, NULL, NULL);
    b.p = ref;
    b.n = 16;
    c = 16;
    t_rpc(CRYPTO_FN_AEAD_UPDATE, &r, &b, 1, pt, &c, NULL);
    b.p = ref + 16;
    n = 16 - c;
    t_check(t_rpc(CRYPTO_FN_AEAD_VERIFY, &r, &b, 1, pt + c, &n, NULL) ==
            PSA_SUCCESS && c + n == 16 && memcmp(pt, aes_pt, 16) == 0 &&
            t_free_slots() == SLOTS,
            "AEAD multipart decrypt + VERIFY round trip, slot freed");
}
#endif

void test_aead(void)
{
    uint32_t k = t_import(PSA_KEY_TYPE_AES, 128, PSA_KEY_USAGE_ENCRYPT |
                          PSA_KEY_USAGE_DECRYPT, PSA_ALG_GCM, aes_key, 16);
    struct psa_crypto_req r = t_req(k, PSA_ALG_GCM, 0);
    uint8_t nonce[12] = { 1, 2, 3 };
    uint8_t ct[48];
    uint8_t pt[48];
    size_t n = sizeof(ct);
    struct buf in[3] = { { nonce, 12 }, { "hdr", 3 }, { aes_pt, 16 } };

    t_check(k != 0 && t_rpc(CRYPTO_FN_AEAD_ENCRYPT, &r, in, 3, ct, &n,
                            NULL) == PSA_SUCCESS && n == 32,
            "AEAD_ENCRYPT GCM: 16 + 16 tag");
    in[2].p = ct;
    in[2].n = n;
    n = sizeof(pt);
    t_check(t_rpc(CRYPTO_FN_AEAD_DECRYPT, &r, in, 3, pt, &n, NULL) ==
            PSA_SUCCESS && n == 16 && memcmp(pt, aes_pt, 16) == 0,
            "AEAD_DECRYPT round trip");
    ct[31] ^= 1;
    n = sizeof(pt);
    t_check(t_rpc(CRYPTO_FN_AEAD_DECRYPT, &r, in, 3, pt, &n, NULL) ==
            PSA_ERROR_INVALID_SIGNATURE && n == 0,
            "AEAD_DECRYPT bad tag: INVALID_SIGNATURE, len 0");
#if PSA_IPRO_AEAD_MULTI
    ct[31] ^= 1;
    aead_multi(k, ct);
#endif
    t_destroy(k);
}

void test_ecc(void)
{
    const psa_algorithm_t alg = PSA_ALG_ECDSA(PSA_ALG_SHA_256);
    const psa_key_usage_t u = PSA_KEY_USAGE_SIGN_HASH |
                              PSA_KEY_USAGE_VERIFY_HASH;
    static const size_t bits[3] = { 256, 384, 521 };
    struct psa_crypto_req r;
    uint8_t pub[133];
    uint8_t sig[132];
    size_t n;
    size_t s;
    struct buf hv[2] = { { abc_sha256, 32 }, { sig, 0 } };
    struct buf mv[2] = { { "abc", 3 }, { sig, 0 } };

    for (size_t i = 0; i < 3; i++) {
        uint32_t k = t_generate(ECC_P_PAIR, bits[i], u, alg);
        size_t len = (bits[i] + 7u) / 8u;
        int ok = k != 0;

        r = t_req(k, alg, 0);
        n = sizeof(pub);
        ok &= t_rpc(CRYPTO_FN_KEY_EXPORT_PUBLIC, &r, NULL, 0, pub, &n,
                    NULL) == PSA_SUCCESS && n == 1 + 2 * len &&
              pub[0] == 4;
        s = sizeof(sig);
        ok &= t_rpc(CRYPTO_FN_SIGN_HASH, &r, hv, 1, sig, &s, NULL) ==
              PSA_SUCCESS && s == 2 * len;
        hv[1].n = s;
        ok &= t_rpc(CRYPTO_FN_VERIFY_HASH, &r, hv, 2, NULL, NULL, NULL) ==
              PSA_SUCCESS;
        sig[0] ^= 1;
        ok &= t_rpc(CRYPTO_FN_VERIFY_HASH, &r, hv, 2, NULL, NULL, NULL) ==
              PSA_ERROR_INVALID_SIGNATURE;
        s = sizeof(sig);
        ok &= t_rpc(CRYPTO_FN_SIGN_MESSAGE, &r, mv, 1, sig, &s, NULL) ==
              PSA_SUCCESS;
        mv[1].n = s;
        ok &= t_rpc(CRYPTO_FN_VERIFY_MESSAGE, &r, mv, 2, NULL, NULL,
                    NULL) == PSA_SUCCESS;
        ok &= t_destroy(k) == PSA_SUCCESS;
        t_info("P-%zu: key %08x", bits[i], (unsigned)k);
        t_check(ok, "ECDSA: generate, export public, sign/verify hash "
                    "and message");
    }
}

static const uint8_t ikm[22] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
};
static const uint8_t salt[13] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
    0x0b, 0x0c,
};
static const uint8_t info[10] = {
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
};
static const uint8_t okm[42] = {
    0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43, 0x4f,
    0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a,
    0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf, 0x34,
    0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18, 0x58, 0x65,
};

/* HKDF-SHA-256 of RFC 5869 case 1, secret as bytes or as key k */
static int hkdf_case1(uint32_t k, uint8_t *out)
{
    struct psa_crypto_req r = t_req(0, PSA_ALG_HKDF(PSA_ALG_SHA_256), 0);
    struct psa_crypto_rsp rsp;
    struct buf s = { salt, 13 };
    struct buf x = { ikm, 22 };
    struct buf in = { info, 10 };
    size_t n = 42;
    int ok;

    ok = t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp) ==
         PSA_SUCCESS;
    r = t_req(0, 0, rsp.op_handle);
    r.step = PSA_KEY_DERIVATION_INPUT_SALT;
    ok &= t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &s, 1, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    r.step = PSA_KEY_DERIVATION_INPUT_SECRET;
    if (k != 0) {
        r.key_id = k;
        ok &= t_rpc(CRYPTO_FN_KD_INPUT_KEY, &r, NULL, 0, NULL, NULL,
                    NULL) == PSA_SUCCESS;
        r.key_id = 0;
    } else {
        ok &= t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &x, 1, NULL, NULL,
                    NULL) == PSA_SUCCESS;
    }
    r.step = PSA_KEY_DERIVATION_INPUT_INFO;
    ok &= t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &in, 1, NULL, NULL, NULL) ==
          PSA_SUCCESS;
    r.step = 0;
    ok &= t_rpc(CRYPTO_FN_KD_OUTPUT_BYTES, &r, NULL, 0, out, &n, NULL) ==
          PSA_SUCCESS && n == 42;
    ok &= t_rpc(CRYPTO_FN_KD_GET_CAPACITY, &r, NULL, 0, NULL, NULL,
                &rsp) == PSA_SUCCESS && rsp.length == 255u * 32u - 42u;
    r.aux = 10;
    ok &= t_rpc(CRYPTO_FN_KD_SET_CAPACITY, &r, NULL, 0, NULL, NULL,
                NULL) == PSA_SUCCESS;
    r.aux = 0;
    n = 11;
    ok &= t_rpc(CRYPTO_FN_KD_OUTPUT_BYTES, &r, NULL, 0, out + 42, &n,
                NULL) == PSA_ERROR_INSUFFICIENT_DATA && n == 0;
    ok &= t_op0(CRYPTO_FN_KD_ABORT, r.op_handle) == PSA_SUCCESS;
    return ok;
}

#if PSA_IPRO_KD_INTEGER
/* PBKDF2-HMAC-SHA256("password", "salt", 1), RFC 7914 section 11 */
static void kd_pbkdf2(void)
{
    static const uint8_t dk[32] = {
        0x12, 0x0f, 0xb6, 0xcf, 0xfc, 0xf8, 0xb3, 0x2c, 0x43, 0xe7, 0x22,
        0x52, 0x56, 0xc4, 0xf8, 0x37, 0xa8, 0x65, 0x48, 0xc9, 0x2c, 0xcc,
        0x35, 0x48, 0x08, 0x05, 0x98, 0x7c, 0xb7, 0x0b, 0xe1, 0x7b,
    };
    const psa_algorithm_t alg = PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_256);
    struct psa_crypto_req r = t_req(0, alg, 0);
    struct psa_crypto_rsp rsp;
    uint64_t cost = 1;
    uint32_t short_cost = 1;
    struct buf v = { &short_cost, sizeof(short_cost) };
    struct buf salt = { "salt", 4 };
    struct buf pw = { "password", 8 };
    uint8_t out[32];
    size_t n = sizeof(out);

    t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r.op_handle = rsp.op_handle;
    r.step = PSA_KEY_DERIVATION_INPUT_COST;
    t_check(t_rpc(CRYPTO_FN_KD_INPUT_INTEGER, &r, &v, 1, NULL, NULL,
                  NULL) == PSA_ERROR_INVALID_ARGUMENT,
            "KD_INPUT_INTEGER 4-byte value: INVALID_ARGUMENT");
    v.p = &cost;
    v.n = sizeof(cost);
    t_check(t_rpc(CRYPTO_FN_KD_INPUT_INTEGER, &r, &v, 1, NULL, NULL,
                  NULL) == PSA_SUCCESS, "KD_INPUT_INTEGER cost 1");
    r.step = PSA_KEY_DERIVATION_INPUT_SALT;
    t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &salt, 1, NULL, NULL, NULL);
    r.step = PSA_KEY_DERIVATION_INPUT_PASSWORD;
    t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &pw, 1, NULL, NULL, NULL);
    r.step = 0;
    t_check(t_rpc(CRYPTO_FN_KD_OUTPUT_BYTES, &r, NULL, 0, out, &n, NULL) ==
            PSA_SUCCESS && n == 32 && memcmp(out, dk, 32) == 0,
            "PBKDF2-HMAC-SHA256 known answer");
    t_check(t_op0(CRYPTO_FN_KD_ABORT, r.op_handle) == PSA_SUCCESS &&
            t_free_slots() == SLOTS, "KD_ABORT frees the PBKDF2 op");
}
#endif

void test_kd(void)
{
    const psa_algorithm_t ka = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,
                                   PSA_ALG_HKDF(PSA_ALG_SHA_256));
    uint32_t sec = t_import(PSA_KEY_TYPE_DERIVE, 176, PSA_KEY_USAGE_DERIVE,
                            PSA_ALG_HKDF(PSA_ALG_SHA_256), ikm, 22);
    uint32_t a = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_DERIVE,
                            PSA_ALG_ECDH);
    uint32_t b = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_DERIVE,
                            PSA_ALG_ECDH);
    uint32_t c = t_generate(ECC_P_PAIR, 256, PSA_KEY_USAGE_DERIVE, ka);
    struct psa_crypto_req r = t_req(a, 0, 0);
    struct psa_crypto_rsp rsp;
    uint8_t out[64];
    uint8_t pa[65];
    uint8_t pb[65];
    uint8_t za[32];
    uint8_t zb[32];
    size_t n;
    struct buf peer;

    t_check(hkdf_case1(0, out) && memcmp(out, okm, 42) == 0,
            "HKDF-SHA-256 RFC 5869 case 1, capacity, INSUFFICIENT_DATA");
    t_check(sec != 0 && hkdf_case1(sec, out) && memcmp(out, okm, 42) == 0,
            "same with the secret as KD_INPUT_KEY");

    n = 65;
    t_rpc(CRYPTO_FN_KEY_EXPORT_PUBLIC, &r, NULL, 0, pa, &n, NULL);
    r.key_id = b;
    n = 65;
    t_rpc(CRYPTO_FN_KEY_EXPORT_PUBLIC, &r, NULL, 0, pb, &n, NULL);
    r = t_req(a, PSA_ALG_ECDH, 0);
    peer.p = pb;
    peer.n = 65;
    n = 32;
    t_check(t_rpc(CRYPTO_FN_RAW_KEY_AGREEMENT, &r, &peer, 1, za, &n,
                  NULL) == PSA_SUCCESS && n == 32,
            "RAW_KEY_AGREEMENT a x B");
    r.key_id = b;
    peer.p = pa;
    n = 32;
    t_check(t_rpc(CRYPTO_FN_RAW_KEY_AGREEMENT, &r, &peer, 1, zb, &n,
                  NULL) == PSA_SUCCESS && memcmp(za, zb, 32) == 0,
            "b x A is the same");

    r = t_req(0, ka, 0);
    t_rpc(CRYPTO_FN_KD_SETUP, &r, NULL, 0, NULL, NULL, &rsp);
    r = t_req(c, 0, rsp.op_handle);
    r.step = 0x10000u | PSA_KEY_DERIVATION_INPUT_SECRET;
    t_check(t_rpc(CRYPTO_FN_KD_KEY_AGREEMENT, &r, &peer, 1, NULL, NULL,
                  NULL) == PSA_ERROR_INVALID_ARGUMENT,
            "step > 0xFFFF: INVALID_ARGUMENT");
    r.step = PSA_KEY_DERIVATION_INPUT_SECRET;
    t_check(t_rpc(CRYPTO_FN_KD_KEY_AGREEMENT, &r, &peer, 1, NULL, NULL,
                  NULL) == PSA_SUCCESS, "KD_KEY_AGREEMENT");
    r = t_req(0, 0, r.op_handle);
    r.step = PSA_KEY_DERIVATION_INPUT_INFO;
    peer.p = info;
    peer.n = 10;
    t_rpc(CRYPTO_FN_KD_INPUT_BYTES, &r, &peer, 1, NULL, NULL, NULL);
    r.step = 0;
    r.attr.type = (PSA_KEY_TYPE_AES);
    r.attr.bits = (128);
    r.attr.usage = (PSA_KEY_USAGE_ENCRYPT);
    r.attr.alg = (PSA_ALG_ECB_NO_PADDING);
    t_check(t_rpc(CRYPTO_FN_KD_OUTPUT_KEY, &r, NULL, 0, NULL, NULL,
                  &rsp) == PSA_SUCCESS && rsp.key_id != 0,
            "KD_OUTPUT_KEY AES-128");
    t_op0(CRYPTO_FN_KD_ABORT, r.op_handle);
    r = t_req(rsp.key_id, PSA_ALG_ECB_NO_PADDING, 0);
    peer.p = aes_pt;
    peer.n = 16;
    n = 16;
    t_check(t_rpc(CRYPTO_FN_CIPHER_ENCRYPT, &r, &peer, 1, out, &n,
                  NULL) == PSA_SUCCESS &&
            t_destroy(rsp.key_id) == PSA_SUCCESS,
            "derived key is the client's: usable, destroyed");
    t_destroy(sec);
    t_destroy(a);
    t_destroy(b);
    t_destroy(c);
    t_check(t_free_slots() == SLOTS, "KD slots free");
#if PSA_IPRO_KD_INTEGER
    kd_pbkdf2();
#endif
}
