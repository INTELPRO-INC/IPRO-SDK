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
 * Host test of svc_attest.c over svc_platform.c: RPC decoding,
 * challenge sizes, BUFFER_TOO_SMALL, size-pass status, and a token
 * whose RFC 6979 ES256 signature verifies with the IAK and whose
 * claims match.
 */
#include <string.h>

#include "attest_test.h"
#include "cbor.h"
#include "check.h"
#include "eat.h"
#include "spm_sha256.h"
#include "psa/crypto.h"
#include "psa_ipro/attest_rpc.h"
#include "psa_ipro/platform_rpc.h"
#include "svc_attest.h"
#include "svc_platform.h"

#define ECDSA PSA_ALG_ECDSA(PSA_ALG_SHA_256)
#define DET_ECDSA PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256)
#define TOKEN ATTEST_FN_GET_TOKEN
#define SIZE  ATTEST_FN_GET_TOKEN_SIZE
#define BAD   PSA_ERROR_INVALID_ARGUMENT

static psa_status_t call(int32_t fn, const struct iv *in, size_t n_in,
                         struct ov *out, size_t n_out)
{
    return t_raw(svc_attest_call, fn, in, n_in, out, n_out);
}

static psa_status_t token(uint32_t csize, const uint8_t *ch, size_t ch_len,
                          uint8_t *tok, size_t *tcap, uint32_t *size)
{
    struct attest_req r = { csize };
    struct attest_rsp rsp = { 0 };
    struct iv in[2] = { { &r, sizeof(r) }, { ch, ch_len } };
    struct ov out[2] = { { &rsp, sizeof(rsp) }, { tok, *tcap } };
    psa_status_t st = t_raw(svc_attest_call, ATTEST_FN_GET_TOKEN, in, 2,
                            out, 2);

    *tcap = out[1].n;
    *size = rsp.token_size;
    return st;
}

static psa_status_t token_size(uint32_t csize, uint32_t *size)
{
    struct attest_req r = { csize };
    struct attest_rsp rsp = { 0 };
    struct iv in[1] = { { &r, sizeof(r) } };
    struct ov out[1] = { { &rsp, sizeof(rsp) } };
    psa_status_t st = t_raw(svc_attest_call, ATTEST_FN_GET_TOKEN_SIZE, in,
                            1, out, 1);

    *size = rsp.token_size;
    return st;
}

/* vector counts, req and rsp sizes, ids */
static void test_vectors(uint8_t tok[1024])
{
    struct attest_req r = { 32 };
    struct attest_rsp rsp;
    struct iv in[3] = { { &r, 4 }, { g_spe, 32 }, { g_spe, 1 } };
    struct ov out[2] = { { &rsp, 4 }, { tok, 1024 } };
    struct iv in5[1] = { { tok, 5 } };
    struct ov out3[1] = { { &rsp, 3 } };

    check(call(TOKEN, in, 1, out, 2) == BAD &&
          call(TOKEN, in, 3, out, 2) == BAD &&
          call(TOKEN, in, 2, out, 1) == BAD &&
          call(SIZE, in, 2, out, 1) == BAD &&
          call(SIZE, in, 1, out, 2) == BAD, "wrong vector counts: refused");
    check(call(SIZE, in5, 1, out, 1) == BAD &&
          call(SIZE, in, 1, out3, 1) == BAD,
          "req 5 bytes, rsp 3 bytes: refused");
    check(call(0, in, 1, out, 1) == PSA_ERROR_NOT_SUPPORTED &&
          call(3, in, 1, out, 1) == PSA_ERROR_NOT_SUPPORTED,
          "ids 0 and 3: NOT_SUPPORTED");
}

static void test_attest_rpc(void)
{
    static const uint32_t bad[] = { 0, 16, 31, 33, 47, 63, 65, 128 };
    uint8_t tok[1024];
    uint32_t s32 = 0;
    uint32_t s48 = 0;
    uint32_t s64 = 0;
    uint32_t s = 0;
    size_t cap;
    int ok = 1;

    check(token_size(32, &s32) == PSA_SUCCESS &&
          token_size(48, &s48) == PSA_SUCCESS &&
          token_size(64, &s64) == PSA_SUCCESS && s48 == s32 + 16 &&
          s64 == s48 + 16 && s64 <= PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE,
          "token size for 32/48/64: +16 each, within 1 KB");
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        cap = sizeof(tok);
        ok &= token_size(bad[i], &s) == PSA_ERROR_INVALID_ARGUMENT &&
              token(bad[i], g_spe, bad[i], tok, &cap, &s) ==
              PSA_ERROR_INVALID_ARGUMENT;
    }
    check(ok, "challenge sizes other than 32/48/64: INVALID_ARGUMENT");
    cap = sizeof(tok);
    check(token(32, g_spe, 48, tok, &cap, &s) == PSA_ERROR_INVALID_ARGUMENT,
          "challenge vector != req.challenge_size: refused");
    cap = s48 - 1;
    check(token(48, g_spe, 48, tok, &cap, &s) == PSA_ERROR_BUFFER_TOO_SMALL &&
          cap == 0, "capacity one short: BUFFER_TOO_SMALL, lens 0");
    cap = 0;
    check(token(32, g_spe, 32, tok, &cap, &s) == PSA_ERROR_INVALID_ARGUMENT &&
          cap == 0, "capacity 0: INVALID_ARGUMENT, lens 0");
    cap = s48;
    check(token(48, g_spe, 48, tok, &cap, &s) == PSA_SUCCESS && cap == s48 &&
          s == s48, "exact capacity: token, len = rsp.token_size");
    test_vectors(tok);
}

/*
 * A signer id that is not hex makes the claims encoder refuse; both
 * calls return its status and no size.
 */
static void test_size_status(void)
{
    char *hex = ((struct svc_platform_id *)svc_platform_id())->signer_hex;
    char first = hex[0];
    uint8_t tok[1024];
    size_t cap = sizeof(tok);
    uint32_t s = 1;

    hex[0] = 'x';
    check(token_size(32, &s) == PSA_ERROR_INVALID_ARGUMENT && s == 0,
          "size pass, claims refused: INVALID_ARGUMENT, no size");
    check(token(32, g_spe, 32, tok, &cap, &s) ==
          PSA_ERROR_INVALID_ARGUMENT && cap == 0,
          "token, claims refused: INVALID_ARGUMENT, lens 0");
    hex[0] = first;
    check(token_size(32, &s) == PSA_SUCCESS && s != 0,
          "signer id restored: size pass succeeds");
}

/* the value of int key in the map at m */
static int find(const CborValue *m, int64_t key, CborValue *v)
{
    CborValue it;
    int64_t k;

    cbor_value_enter_container(m, &it);
    while (!cbor_value_at_end(&it)) {
        cbor_value_get_int64(&it, &k);
        cbor_value_advance(&it);
        if (k == key) {
            *v = it;
            return 1;
        }
        cbor_value_advance(&it);
    }
    return 0;
}

static int bytes_eq(const CborValue *v, const void *p, size_t n)
{
    uint8_t b[80];
    size_t len = sizeof(b);

    return cbor_value_copy_byte_string(v, b, &len, NULL) == CborNoError &&
           len == n && memcmp(b, p, n) == 0;
}

/*
 * Same challenge, same token byte for byte; its signature is the
 * RFC 6979 signature of the Sig_structure hash by the IAK seed.
 */
static void test_token_det(const uint8_t ch[64], const uint8_t *tok,
                           size_t cap, size_t plen)
{
    psa_key_attributes_t a = psa_key_attributes_init();
    mbedtls_svc_key_id_t k = 0;
    uint8_t again[1024];
    uint8_t ss[1100] = { 0x84, 0x6A, 'S', 'i', 'g', 'n', 'a', 't', 'u', 'r',
                         'e', '1', 0x43, 0xA1, 0x01, 0x26, 0x40 };
    uint8_t h[32];
    uint8_t d[32];
    uint8_t want[64];
    size_t cap2 = sizeof(again);
    size_t n = 0;
    uint32_t s = 0;

    check(token(64, ch, 64, again, &cap2, &s) == PSA_SUCCESS &&
          cap2 == cap && memcmp(again, tok, cap) == 0,
          "same challenge: identical token, signature included");
    memcpy(ss + 17, tok + 7, 3 + plen);
    mbedtls_sha256_ret(ss, 17 + 3 + plen, h, 0);
    t_seed_of("IAK", d);
    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&a, DET_ECDSA);
    check(psa_import_key(&a, d, 32, &k) == PSA_SUCCESS &&
          psa_sign_hash(k, DET_ECDSA, h, 32, want, 64, &n) == PSA_SUCCESS &&
          n == 64 && memcmp(tok + 12 + plen, want, 64) == 0,
          "token signature = RFC 6979 signature of the IAK seed");
    psa_destroy_key(k);
}

static void test_token(void)
{
    const struct svc_platform_id *id = svc_platform_id();
    psa_key_attributes_t a = psa_key_attributes_init();
    mbedtls_svc_key_id_t k = 0;
    uint8_t ch[64];
    uint8_t tok[1024];
    uint8_t pub[65];
    uint8_t ss[1100] = { 0x84, 0x6A, 'S', 'i', 'g', 'n', 'a', 't', 'u', 'r',
                         'e', '1', 0x43, 0xA1, 0x01, 0x26, 0x40 };
    uint8_t h[32];
    size_t cap = sizeof(tok);
    size_t plen;
    uint32_t s;
    char ver[8] = { 0 };
    size_t vlen = sizeof(ver);
    CborParser p;
    CborValue m;
    CborValue v;
    CborValue sw;

    for (int i = 0; i < 64; i++) {
        ch[i] = (uint8_t)(0xA0 ^ i);
    }
    svc_attest_client_reset();
    svc_platform_client_reset();
    check(token(64, ch, 64, tok, &cap, &s) == PSA_SUCCESS && cap == s &&
          memcmp(tok, "\xD2\x84\x43\xA1\x01\x26\xA0\x59", 8) == 0,
          "token: tag 18 COSE_Sign1, 2-byte payload length");
    plen = ((size_t)tok[8] << 8) | tok[9];
    check(cap == 10 + plen + 2 + 64 && tok[10 + plen] == 0x58 &&
          tok[11 + plen] == 0x40, "payload then a 64-byte signature");
    memcpy(ss + 17, tok + 7, 3 + plen);
    mbedtls_sha256_ret(ss, 17 + 3 + plen, h, 0);
    t_plat(PLATFORM_FN_GET_IAK_PUB, pub, 65);
    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&a, ECDSA);
    psa_import_key(&a, pub, 65, &k);
    check(psa_verify_hash(k, ECDSA, h, 32, tok + 12 + plen, 64) ==
          PSA_SUCCESS, "signature verifies with the exported IAK");
    tok[10 + plen / 2] ^= 1;
    memcpy(ss + 17, tok + 7, 3 + plen);
    mbedtls_sha256_ret(ss, 17 + 3 + plen, h, 0);
    check(psa_verify_hash(k, ECDSA, h, 32, tok + 12 + plen, 64) ==
          PSA_ERROR_INVALID_SIGNATURE, "flipped payload byte: invalid");
    tok[10 + plen / 2] ^= 1;
    psa_destroy_key(k);
    test_token_det(ch, tok, cap, plen);

    cbor_parser_init(tok + 10, plen, 0, &p, &m);
    check(find(&m, EAT_NONCE, &v) && bytes_eq(&v, ch, 64) &&
          find(&m, EAT_INSTANCE_ID, &v) &&
          bytes_eq(&v, id->instance_id, 33) &&
          find(&m, EAT_IMPL_ID, &v) && bytes_eq(&v, id->impl_id, 32) &&
          find(&m, EAT_BOOT_SEED, &v) && bytes_eq(&v, id->boot_seed, 32),
          "claims: nonce, instance id, implementation id, boot seed");
    check(find(&m, EAT_SW_COMPONENTS, &v) &&
          cbor_value_enter_container(&v, &sw) == CborNoError &&
          find(&sw, EAT_SW_MEASUREMENT, &v) &&
          bytes_eq(&v, id->spe_hash, 32) &&
          cbor_value_advance(&sw) == CborNoError &&
          find(&sw, EAT_SW_MEASUREMENT, &v) &&
          bytes_eq(&v, spm_nspe_measurement, 32) &&
          find(&sw, EAT_SW_VERSION, &v) &&
          cbor_value_copy_text_string(&v, ver, &vlen, NULL) == CborNoError &&
          strcmp(ver, "1.2.3") == 0,
          "sw: SPE hash, NSPE measurement and version 1.2.3");
}

int main(void)
{
    uint8_t tok[64];
    size_t cap = sizeof(tok);
    uint32_t s = 0;

    for (size_t i = 0; i < sizeof(g_spe); i++) {
        g_spe[i] = (uint8_t)(i * 13u);
    }
    check(psa_crypto_init() == PSA_SUCCESS, "host PSA init");
    check(token(32, g_spe, 32, tok, &cap, &s) ==
          PSA_ERROR_SERVICE_FAILURE && cap == 0,
          "before platform init: SERVICE_FAILURE, lens 0");
    check(t_init() == PSA_SUCCESS, "platform identity");
    test_attest_rpc();
    test_size_status();
    test_token();
    check(g_contract_bad == 0, "out contract held on every call");
    return check_summary();
}
