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
 * PSA IoT profile 1 claims map over tinycbor; the COSE_Sign1 ES256
 * envelope and its Sig_structure hash are laid out byte by byte
 * around the payload.
 */
#include <string.h>

#include "cbor.h"
#include "eat.h"
#include "spm_sha256.h"

#define EAT_SIGNER_MAX 64u
#define EAT_CLAIMS_MIN 7u

/* tag 18, array(4), bstr(h'A10126' = {1: -7}), {} */
static const uint8_t eat_cose_head[] = {
    0xD2, 0x84, 0x43, 0xA1, 0x01, 0x26, 0xA0,
};
/* array(4), "Signature1", bstr(h'A10126'), h'' */
static const uint8_t eat_sig_head[] = {
    0x84, 0x6A, 'S', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', '1',
    0x43, 0xA1, 0x01, 0x26, 0x40,
};
/* bstr head of the 64-byte signature */
static const uint8_t eat_sig_bstr[] = { 0x58, 0x40 };

static int eat_nibble(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

/* hex string bytes; 0 when empty, odd, over 64 bytes or not hex */
static size_t eat_unhex(const char *hex, uint8_t out[EAT_SIGNER_MAX])
{
    size_t n = strlen(hex);

    if (n == 0 || (n & 1u) != 0 || n / 2u > EAT_SIGNER_MAX) {
        return 0;
    }
    for (size_t i = 0; i < n; i += 2) {
        int hi = eat_nibble(hex[i]);
        int lo = eat_nibble(hex[i + 1]);

        if (hi < 0 || lo < 0) {
            return 0;
        }
        out[i / 2u] = (uint8_t)((hi << 4) | lo);
    }
    return n / 2u;
}

static int eat_claims_ok(const struct eat_claims *c)
{
    uint8_t sid[EAT_SIGNER_MAX];

    if (c->nonce_len > EAT_NONCE_MAX || c->sw_count > EAT_SW_MAX) {
        return 0;
    }
    for (size_t i = 0; i < c->sw_count; i++) {
        const char *hex = c->sw[i].signer_id_hex;

        if (hex != NULL && eat_unhex(hex, sid) == 0) {
            return 0;
        }
    }
    return 1;
}

static int eat_text(CborEncoder *m, int key, const char *s)
{
    int err;

    if (s == NULL) {
        return CborNoError;
    }
    err = cbor_encode_int(m, key);
    return err | cbor_encode_text_stringz(m, s);
}

static int eat_bytes(CborEncoder *m, int key, const uint8_t *p, size_t n)
{
    int err = cbor_encode_int(m, key);

    return err | cbor_encode_byte_string(m, p, n);
}

static int eat_sw(CborEncoder *a, const struct eat_sw *sw)
{
    uint8_t sid[EAT_SIGNER_MAX];
    size_t keys = 1u + (sw->type != NULL) + (sw->version != NULL) +
                  (sw->signer_id_hex != NULL) +
                  (sw->measurement_desc != NULL);
    CborEncoder m;
    int err = cbor_encoder_create_map(a, &m, keys);

    err |= eat_text(&m, EAT_SW_TYPE, sw->type);
    err |= eat_bytes(&m, EAT_SW_MEASUREMENT, sw->measurement,
                     sizeof(sw->measurement));
    err |= eat_text(&m, EAT_SW_VERSION, sw->version);
    if (sw->signer_id_hex != NULL) {
        err |= eat_bytes(&m, EAT_SW_SIGNER_ID, sid,
                         eat_unhex(sw->signer_id_hex, sid));
    }
    err |= eat_text(&m, EAT_SW_DESC, sw->measurement_desc);
    return err | cbor_encoder_close_container(a, &m);
}

int eat_encode_claims(const struct eat_claims *c, uint8_t *out,
                      size_t cap, size_t *len)
{
    size_t keys = EAT_CLAIMS_MIN + (c->profile != NULL) +
                  (c->verification_url != NULL);
    CborEncoder e;
    CborEncoder m;
    CborEncoder a;
    int err;

    *len = 0;
    if (!eat_claims_ok(c)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    cbor_encoder_init(&e, out, cap, 0);
    err = cbor_encoder_create_map(&e, &m, keys);
    err |= eat_text(&m, EAT_PROFILE, c->profile);
    err |= cbor_encode_int(&m, EAT_CLIENT_ID);
    err |= cbor_encode_int(&m, c->client_id);
    err |= cbor_encode_int(&m, EAT_LIFECYCLE);
    err |= cbor_encode_uint(&m, c->lifecycle);
    err |= eat_bytes(&m, EAT_IMPL_ID, c->impl_id, sizeof(c->impl_id));
    err |= eat_bytes(&m, EAT_BOOT_SEED, c->boot_seed, sizeof(c->boot_seed));
    err |= cbor_encode_int(&m, EAT_SW_COMPONENTS);
    err |= cbor_encoder_create_array(&m, &a, c->sw_count);
    for (size_t i = 0; i < c->sw_count; i++) {
        err |= eat_sw(&a, &c->sw[i]);
    }
    err |= cbor_encoder_close_container(&m, &a);
    err |= eat_bytes(&m, EAT_NONCE, c->nonce, c->nonce_len);
    err |= eat_bytes(&m, EAT_INSTANCE_ID, c->instance_id,
                     sizeof(c->instance_id));
    err |= eat_text(&m, EAT_VERIFY_URL, c->verification_url);
    err |= cbor_encoder_close_container(&e, &m);

    if (err == CborErrorOutOfMemory) {
        *len = cap + cbor_encoder_get_extra_bytes_needed(&e);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    if (err != CborNoError) {
        return PSA_ERROR_GENERIC_ERROR;
    }
    *len = cbor_encoder_get_buffer_size(&e, out);
    return PSA_SUCCESS;
}

/* CBOR bstr head of n bytes; 0 above 0xFFFF */
static size_t eat_bstr_head(uint8_t h[3], size_t n)
{
    if (n < 24u) {
        h[0] = (uint8_t)(0x40u | n);
        return 1;
    }
    if (n <= 0xFFu) {
        h[0] = 0x58;
        h[1] = (uint8_t)n;
        return 2;
    }
    if (n <= 0xFFFFu) {
        h[0] = 0x59;
        h[1] = (uint8_t)(n >> 8);
        h[2] = (uint8_t)n;
        return 3;
    }
    return 0;
}

/* SHA-256 of the Sig_structure: fixed head, payload head, payload */
static int eat_sig_hash(const uint8_t *ph, size_t phlen,
                        const uint8_t *payload, size_t plen,
                        uint8_t hash[32])
{
    mbedtls_sha256_context sha;
    int ret;

    mbedtls_sha256_init(&sha);
    ret = mbedtls_sha256_starts_ret(&sha, 0);
    if (ret == 0) {
        ret = mbedtls_sha256_update_ret(&sha, eat_sig_head,
                                        sizeof(eat_sig_head));
    }
    if (ret == 0) {
        ret = mbedtls_sha256_update_ret(&sha, ph, phlen);
    }
    if (ret == 0 && plen != 0) {
        ret = mbedtls_sha256_update_ret(&sha, payload, plen);
    }
    if (ret == 0) {
        ret = mbedtls_sha256_finish_ret(&sha, hash);
    }
    mbedtls_sha256_free(&sha);
    return ret == 0 ? PSA_SUCCESS : PSA_ERROR_GENERIC_ERROR;
}

int eat_cose_sign1(const uint8_t *payload, size_t plen, eat_sign_fn sign,
                   void *ctx, uint8_t *out, size_t cap, size_t *len)
{
    uint8_t ph[3];
    uint8_t hash[32];
    uint8_t sig[EAT_SIG_SIZE];
    size_t phlen = eat_bstr_head(ph, plen);
    size_t pre = sizeof(eat_cose_head) + phlen;
    size_t need = pre + plen + sizeof(eat_sig_bstr) + sizeof(sig);
    int st;

    *len = 0;
    if (phlen == 0 || sign == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (cap < need) {
        *len = need;
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    st = eat_sig_hash(ph, phlen, payload, plen, hash);
    if (st == PSA_SUCCESS) {
        st = sign(hash, sig, ctx);
    }
    if (st != PSA_SUCCESS) {
        return st;
    }
    if (plen != 0) {
        memmove(out + pre, payload, plen);
    }
    memcpy(out, eat_cose_head, sizeof(eat_cose_head));
    memcpy(out + sizeof(eat_cose_head), ph, phlen);
    memcpy(out + pre + plen, eat_sig_bstr, sizeof(eat_sig_bstr));
    memcpy(out + pre + plen + sizeof(eat_sig_bstr), sig, sizeof(sig));
    *len = need;
    return PSA_SUCCESS;
}
