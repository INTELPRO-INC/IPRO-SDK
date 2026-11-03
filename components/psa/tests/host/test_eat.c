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
 * Host test of eat.c: claims map content, COSE_Sign1 layout, an
 * RFC 6979 ES256 signature checked with mbedtls over an independently
 * built Sig_structure, tampering and the BUFFER_TOO_SMALL sizes.
 */
#include <string.h>

/*
 * the checks read the ECDSA context fields (private in mbedtls 3.6);
 * TF-PSA-Crypto declares the ECDSA module under mbedtls/private
 */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include "cbor.h"
#include "check.h"
#include "eat.h"
#include "hw_poll.h"
#include "psa_ipro/crypto_lib.h"
#if PSA_IPRO_LEGACY_PRIVATE
#include "mbedtls/private/ecdsa.h"
#else
#include "mbedtls/ecdsa.h"
#endif
#include "spm_sha256.h"

#ifndef MBEDTLS_PRIVATE
#define MBEDTLS_PRIVATE(member) member
#endif

#define TOK_CAP 1024u

static const uint8_t g_protected[3] = { 0xA1, 0x01, 0x26 };
static const char g_signer_hex[] = "00112233445566778899aabbccddeeff"
                                   "0123456789ABCDEF0123456789abcdef";
static uint64_t g_rng = 0x9E3779B97F4A7C15ull;
static mbedtls_ecdsa_context g_key;
static int g_sign_calls;

static int rng(void *p, unsigned char *out, size_t n)
{
    (void)p;
    for (size_t i = 0; i < n; i++) {
        g_rng ^= g_rng << 13;
        g_rng ^= g_rng >> 7;
        g_rng ^= g_rng << 17;
        out[i] = (uint8_t)g_rng;
    }
    return 0;
}

/* entropy of the PSA core that mbedtls 3.6 md.c pulls in */
int mbedtls_hardware_poll(void *data, unsigned char *out, size_t len,
                          size_t *olen)
{
    *olen = len;
    return rng(data, out, len);
}

static int ecdsa_sign(const uint8_t hash[32], uint8_t sig[64], void *ctx)
{
    mbedtls_ecdsa_context *k = ctx;
    mbedtls_mpi r;
    mbedtls_mpi s;
    int ret;

    g_sign_calls++;
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    ret = mbedtls_ecdsa_sign_det_ext(&k->MBEDTLS_PRIVATE(grp), &r, &s,
                                     &k->MBEDTLS_PRIVATE(d), hash, 32,
                                     MBEDTLS_MD_SHA256, rng, NULL);
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&r, sig, 32);
    }
    if (ret == 0) {
        ret = mbedtls_mpi_write_binary(&s, sig + 32, 32);
    }
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    return ret == 0 ? PSA_SUCCESS : PSA_ERROR_GENERIC_ERROR;
}

/* deterministic stand-in: sig = hash || hash */
static int copy_sign(const uint8_t hash[32], uint8_t sig[64], void *ctx)
{
    (void)ctx;
    g_sign_calls++;
    memcpy(sig, hash, 32);
    memcpy(sig + 32, hash, 32);
    return PSA_SUCCESS;
}

static int fail_sign(const uint8_t hash[32], uint8_t sig[64], void *ctx)
{
    (void)hash;
    (void)ctx;
    memset(sig, 0, 64);
    return PSA_ERROR_HARDWARE_FAILURE;
}

static void fill(uint8_t *p, size_t n, uint8_t seed)
{
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)(seed + 3u * i);
    }
}

static struct eat_claims claims(void)
{
    struct eat_claims c;

    memset(&c, 0, sizeof(c));
    fill(c.nonce, 48, 0x10);
    c.nonce_len = 48;
    c.client_id = -1;
    c.lifecycle = 0x3000;
    fill(c.impl_id, 32, 0x20);
    fill(c.boot_seed, 32, 0x30);
    c.instance_id[0] = 0x01;
    fill(c.instance_id + 1, 32, 0x40);
    c.profile = "PSA_IOT_PROFILE_1";
    c.verification_url = "https://verifier.example";
    c.sw[0] = (struct eat_sw){ "SPE", "0.1.0", { 0 }, "sha-256",
                               g_signer_hex };
    fill(c.sw[0].measurement, 32, 0x50);
    c.sw[1] = (struct eat_sw){ "NSPE", "1.2.3", { 0 }, "sha-256",
                               g_signer_hex };
    fill(c.sw[1].measurement, 32, 0x60);
    c.sw_count = 2;
    return c;
}

/* copy of the next bstr into buf; 0 when it is not one or too long */
static size_t bstr(CborValue *it, uint8_t *buf, size_t cap)
{
    size_t n = cap;

    if (!cbor_value_is_byte_string(it) ||
        cbor_value_copy_byte_string(it, buf, &n, it) != CborNoError) {
        return 0;
    }
    return n;
}

static int64_t int_at(CborValue *it)
{
    int64_t v = 0;

    cbor_value_get_int64(it, &v);
    cbor_value_advance(it);
    return v;
}

/* sw component map: bit k set for key k; signer id bytes checked */
static unsigned sw_keys(CborValue *map, int *signer_ok)
{
    CborValue m;
    uint8_t b[64];
    unsigned keys = 0;

    cbor_value_enter_container(map, &m);
    while (!cbor_value_at_end(&m)) {
        int64_t k = int_at(&m);

        keys |= 1u << (k & 15);
        if (k == EAT_SW_SIGNER_ID) {
            *signer_ok = bstr(&m, b, sizeof(b)) == 32 && b[0] == 0x00 &&
                         b[15] == 0xff && b[16] == 0x01 && b[23] == 0xef &&
                         b[31] == 0xef;
        } else {
            cbor_value_advance(&m);
        }
    }
    cbor_value_leave_container(map, &m);
    return keys;
}

static void test_claims(void)
{
    struct eat_claims c = claims();
    uint8_t buf[TOK_CAP];
    uint8_t b[64];
    size_t len = 0;
    size_t n_map = 0;
    size_t n_keys = 0;
    size_t n_sw = 0;
    unsigned sw0 = 0;
    int nonce_ok = 0;
    int signer_ok = 0;
    int64_t client = 0;
    CborParser p;
    CborValue it;
    CborValue m;

    check(eat_encode_claims(&c, buf, sizeof(buf), &len) == PSA_SUCCESS &&
          len > 300, "claims encode");
    check(cbor_parser_init(buf, len, 0, &p, &it) == CborNoError &&
          cbor_value_is_map(&it) &&
          cbor_value_get_map_length(&it, &n_map) == CborNoError,
          "claims: one definite-length map");
    cbor_value_enter_container(&it, &m);
    while (!cbor_value_at_end(&m)) {
        int64_t k = int_at(&m);

        n_keys++;
        if (k == EAT_NONCE) {
            nonce_ok = bstr(&m, b, sizeof(b)) == 48 &&
                       memcmp(b, c.nonce, 48) == 0;
        } else if (k == EAT_CLIENT_ID) {
            client = int_at(&m);
        } else if (k == EAT_SW_COMPONENTS && cbor_value_is_array(&m)) {
            CborValue a;

            cbor_value_get_array_length(&m, &n_sw);
            cbor_value_enter_container(&m, &a);
            sw0 = sw_keys(&a, &signer_ok);
            while (!cbor_value_at_end(&a)) {
                cbor_value_advance(&a);
            }
            cbor_value_leave_container(&m, &a);
        } else {
            cbor_value_advance(&m);
        }
    }
    check(n_map == 9 && n_keys == 9, "claims map has the 9 claim keys");
    check(nonce_ok, "nonce bytes equal");
    check(client == -1, "client id -1");
    check(n_sw == 2, "sw components: array of 2");
    check(sw0 == ((1u << 1) | (1u << 2) | (1u << 4) | (1u << 5) |
                  (1u << 6)) && signer_ok,
          "sw map keys 1 2 4 5 6, signer id hex decoded");
}

static void test_claims_edges(void)
{
    struct eat_claims c = claims();
    uint8_t buf[TOK_CAP];
    size_t len = 0;
    size_t need = 0;
    size_t n_map = 0;
    CborParser p;
    CborValue it;

    eat_encode_claims(&c, buf, sizeof(buf), &need);
    check(eat_encode_claims(&c, buf, need - 1, &len) ==
          PSA_ERROR_BUFFER_TOO_SMALL && len == need,
          "claims: cap one short -> BUFFER_TOO_SMALL, len = required");
    check(eat_encode_claims(&c, buf, need, &len) == PSA_SUCCESS &&
          len == need, "claims: exact cap fits");

    c.profile = NULL;
    c.verification_url = NULL;
    c.sw[0] = (struct eat_sw){ NULL, NULL, { 1 }, NULL, NULL };
    c.sw_count = 1;
    check(eat_encode_claims(&c, buf, sizeof(buf), &len) == PSA_SUCCESS &&
          cbor_parser_init(buf, len, 0, &p, &it) == CborNoError &&
          cbor_value_get_map_length(&it, &n_map) == CborNoError &&
          n_map == 7, "NULL profile and url left out: 7 keys");

    c = claims();
    c.nonce_len = EAT_NONCE_MAX + 1;
    check(eat_encode_claims(&c, buf, sizeof(buf), &len) ==
          PSA_ERROR_INVALID_ARGUMENT && len == 0, "nonce of 65: refused");
    c = claims();
    c.sw_count = EAT_SW_MAX + 1;
    check(eat_encode_claims(&c, buf, sizeof(buf), &len) ==
          PSA_ERROR_INVALID_ARGUMENT && len == 0, "4 sw components: refused");
    c = claims();
    c.sw[1].signer_id_hex = "abc";
    check(eat_encode_claims(&c, buf, sizeof(buf), &len) ==
          PSA_ERROR_INVALID_ARGUMENT, "odd-length signer hex: refused");
    c.sw[1].signer_id_hex = "zz";
    check(eat_encode_claims(&c, buf, sizeof(buf), &len) ==
          PSA_ERROR_INVALID_ARGUMENT, "non-hex signer id: refused");
}

/* SHA-256 of the Sig_structure, encoded here with tinycbor */
static void sig_hash(const uint8_t *prot, size_t plen_prot,
                     const uint8_t *payload, size_t plen, uint8_t h[32])
{
    static uint8_t ss[TOK_CAP + 64];
    CborEncoder e;
    CborEncoder a;

    cbor_encoder_init(&e, ss, sizeof(ss), 0);
    cbor_encoder_create_array(&e, &a, 4);
    cbor_encode_text_stringz(&a, "Signature1");
    cbor_encode_byte_string(&a, prot, plen_prot);
    cbor_encode_byte_string(&a, NULL, 0);
    cbor_encode_byte_string(&a, payload, plen);
    cbor_encoder_close_container(&e, &a);
    mbedtls_sha256_ret(ss, cbor_encoder_get_buffer_size(&e, ss), h, 0);
}

/* 1 when tok is 18([h'A10126', {}, payload, sig]) and sig verifies */
static int cose_verify(const uint8_t *tok, size_t len, uint8_t *payload,
                       size_t *plen)
{
    uint8_t prot[8];
    uint8_t sig[80];
    uint8_t h[32];
    size_t n_arr = 0;
    size_t n_unprot = 1;
    size_t n_prot;
    size_t n_sig;
    CborTag tag = 0;
    CborParser p;
    CborValue it;
    CborValue a;
    mbedtls_mpi r;
    mbedtls_mpi s;
    int ok;

    if (cbor_parser_init(tok, len, 0, &p, &it) != CborNoError ||
        !cbor_value_is_tag(&it) || cbor_value_get_tag(&it, &tag) ||
        tag != 18 || cbor_value_advance_fixed(&it) ||
        !cbor_value_is_array(&it) ||
        cbor_value_get_array_length(&it, &n_arr) || n_arr != 4) {
        return 0;
    }
    cbor_value_enter_container(&it, &a);
    n_prot = bstr(&a, prot, sizeof(prot));
    if (!cbor_value_is_map(&a) ||
        cbor_value_get_map_length(&a, &n_unprot) || n_unprot != 0) {
        return 0;
    }
    cbor_value_advance(&a);
    *plen = bstr(&a, payload, TOK_CAP);
    n_sig = bstr(&a, sig, sizeof(sig));
    if (n_prot != 3 || memcmp(prot, g_protected, 3) != 0 || n_sig != 64 ||
        !cbor_value_at_end(&a) || cbor_value_leave_container(&it, &a) ||
        !cbor_value_at_end(&it)) {
        return 0;
    }
    sig_hash(prot, n_prot, payload, *plen, h);
    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);
    mbedtls_mpi_read_binary(&r, sig, 32);
    mbedtls_mpi_read_binary(&s, sig + 32, 32);
    ok = mbedtls_ecdsa_verify(&g_key.MBEDTLS_PRIVATE(grp), h, 32,
                              &g_key.MBEDTLS_PRIVATE(Q), &r, &s) == 0;
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);
    return ok;
}

static void test_cose(void)
{
    struct eat_claims c = claims();
    static uint8_t pl[TOK_CAP];
    static uint8_t tok[TOK_CAP];
    static uint8_t got[TOK_CAP];
    size_t plen = 0;
    size_t tlen = 0;
    size_t glen = 0;

    eat_encode_claims(&c, pl, sizeof(pl), &plen);
    check(eat_cose_sign1(pl, plen, ecdsa_sign, &g_key, tok, sizeof(tok),
                         &tlen) == PSA_SUCCESS &&
          tlen == 1 + 1 + 4 + 1 + 3 + plen + 2 + 64,
          "COSE_Sign1 built, size = envelope + payload + sig");
    check(tok[0] == 0xD2 && tok[1] == 0x84, "tag 18, array of 4");
    check(cose_verify(tok, tlen, got, &glen) && glen == plen &&
          memcmp(got, pl, plen) == 0,
          "tag 18 parsed; ES256 over the Sig_structure verifies");

    {
        static uint8_t tok2[TOK_CAP];
        size_t tlen2 = 0;

        check(eat_cose_sign1(pl, plen, ecdsa_sign, &g_key, tok2,
                             sizeof(tok2), &tlen2) == PSA_SUCCESS &&
              tlen2 == tlen && memcmp(tok2, tok, tlen) == 0,
              "same claims, RFC 6979 signer: identical token");
    }

    tok[tlen - 64 - 2 - plen / 2] ^= 0x01;
    check(!cose_verify(tok, tlen, got, &glen),
          "one payload byte flipped: verify fails");
    tok[tlen - 64 - 2 - plen / 2] ^= 0x01;
    tok[tlen - 1] ^= 0x80;
    check(!cose_verify(tok, tlen, got, &glen),
          "one signature byte flipped: verify fails");
}

static void test_cose_edges(void)
{
    struct eat_claims c = claims();
    static uint8_t pl[TOK_CAP];
    static uint8_t a[TOK_CAP];
    static uint8_t b[TOK_CAP];
    size_t plen = 0;
    size_t alen = 0;
    size_t blen = 0;
    int calls;

    eat_encode_claims(&c, pl, sizeof(pl), &plen);
    eat_cose_sign1(pl, plen, copy_sign, NULL, a, sizeof(a), &alen);
    memset(b, 0xEE, sizeof(b));
    calls = g_sign_calls;
    check(eat_cose_sign1(pl, plen, copy_sign, NULL, b, alen - 1, &blen) ==
          PSA_ERROR_BUFFER_TOO_SMALL && blen == alen,
          "COSE: cap one short -> BUFFER_TOO_SMALL, len = required");
    check(g_sign_calls == calls && b[0] == 0xEE && b[alen - 2] == 0xEE,
          "COSE too small: no signature, out untouched");

    memcpy(b, pl, plen);
    check(eat_cose_sign1(b, plen, copy_sign, NULL, b, alen, &blen) ==
          PSA_SUCCESS && blen == alen && memcmp(a, b, alen) == 0,
          "payload at the start of out: same token in place");

    check(eat_cose_sign1(pl, plen, fail_sign, NULL, b, sizeof(b), &blen) ==
          PSA_ERROR_HARDWARE_FAILURE && blen == 0,
          "signer error returned, len 0");
    check(eat_cose_sign1(pl, 0, copy_sign, NULL, b, sizeof(b), &blen) ==
          PSA_SUCCESS && blen == 1 + 1 + 4 + 1 + 1 + 2 + 64,
          "empty payload: 1-byte bstr head");
}

int main(void)
{
    mbedtls_ecdsa_init(&g_key);
    check(mbedtls_ecdsa_genkey(&g_key, MBEDTLS_ECP_DP_SECP256R1, rng,
                               NULL) == 0, "host P-256 key");
    test_claims();
    test_claims_edges();
    test_cose();
    test_cose_edges();
    mbedtls_ecdsa_free(&g_key);
    return check_summary();
}
