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
 * SPE Initial Attestation service: unpacks the psa_ipro/attest_rpc.h
 * vectors and builds the token in place in the token out vector (SPE
 * bounce RAM): claims map first, then COSE_Sign1 by the IAK.
 */
#include <string.h>

#include "eat.h"
#include "psa_ipro/attest_rpc.h"
#include "spm_nspe.h"
#include "svc_attest.h"
#include "svc_platform.h"

#define ATTEST_CLIENT_NSPE (-1)
#define ATTEST_PROFILE     "PSA_IOT_PROFILE_1"
#define ATTEST_SPE_VERSION "0.1.0"
#define ATTEST_HASH_DESC   "sha-256"
#define ATTEST_VERSION_MAX 16u

/* vector counts per id */
static const struct {
    uint8_t n_in;
    uint8_t n_out;
} attest_fns[ATTEST_FN_GET_TOKEN_SIZE + 1] = {
    [ATTEST_FN_GET_TOKEN] = { 2, 2 },
    [ATTEST_FN_GET_TOKEN_SIZE] = { 1, 1 },
};

static int attest_sign(const uint8_t hash[32], uint8_t sig[64], void *ctx)
{
    (void)ctx;
    return svc_platform_sign(hash, sig);
}

/* decimal digits of v at s; returns the end */
static char *attest_dec(char *s, uint32_t v)
{
    char d[10];
    size_t n = 0;

    do {
        d[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0);
    while (n != 0) {
        *s++ = d[--n];
    }
    return s;
}

/* "M.m.p" of a 0xMMmmpppp version */
static void attest_version(uint32_t v, char s[ATTEST_VERSION_MAX])
{
    s = attest_dec(s, v >> 24);
    *s++ = '.';
    s = attest_dec(s, (v >> 16) & 0xFFu);
    *s++ = '.';
    s = attest_dec(s, v & 0xFFFFu);
    *s = '\0';
}

/* claims of this boot with nonce (zeros when NULL) */
static psa_status_t attest_claims(struct eat_claims *c,
                                  const uint8_t *nonce, size_t n,
                                  char ver[ATTEST_VERSION_MAX])
{
    const struct svc_platform_id *id = svc_platform_id();

    if (id == NULL) {
        return PSA_ERROR_SERVICE_FAILURE;
    }
    memset(c, 0, sizeof(*c));
    if (nonce != NULL) {
        memcpy(c->nonce, nonce, n);
    }
    c->nonce_len = n;
    c->client_id = ATTEST_CLIENT_NSPE;
    c->lifecycle = id->lifecycle;
    memcpy(c->impl_id, id->impl_id, sizeof(c->impl_id));
    memcpy(c->boot_seed, id->boot_seed, sizeof(c->boot_seed));
    memcpy(c->instance_id, id->instance_id, sizeof(c->instance_id));
    c->profile = ATTEST_PROFILE;
    attest_version(spm_nspe_version, ver);
    c->sw[0] = (struct eat_sw){ "SPE", ATTEST_SPE_VERSION, { 0 },
                                ATTEST_HASH_DESC, id->signer_hex };
    memcpy(c->sw[0].measurement, id->spe_hash, 32);
    c->sw[1] = (struct eat_sw){ "NSPE", ver, { 0 }, ATTEST_HASH_DESC,
                                id->signer_hex };
    memcpy(c->sw[1].measurement, spm_nspe_measurement, 32);
    c->sw_count = 2;
    return PSA_SUCCESS;
}

/* capacity-0 encoder pass: BUFFER_TOO_SMALL is success, else error */
static psa_status_t attest_count_status(int st)
{
    return st == PSA_ERROR_BUFFER_TOO_SMALL ? PSA_SUCCESS :
           st == PSA_SUCCESS ? PSA_ERROR_GENERIC_ERROR : st;
}

/* token size of c: both encoders with capacity 0 only count */
static psa_status_t attest_token_size(const struct eat_claims *c,
                                      size_t *size)
{
    uint8_t none[1];
    size_t plen = 0;
    psa_status_t st;

    *size = 0;
    st = attest_count_status(eat_encode_claims(c, none, 0, &plen));
    if (st == PSA_SUCCESS) {
        st = attest_count_status(eat_cose_sign1(none, plen, attest_sign,
                                                NULL, none, 0, size));
    }
    return st;
}

static int attest_challenge_ok(uint32_t n)
{
    return n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 ||
           n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48 ||
           n == PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64;
}

/* Exact vector counts and req/rsp sizes, a valid challenge size */
static psa_status_t attest_unpack(int32_t type, const psa_invec *in,
                                  size_t in_len, const psa_outvec *out,
                                  size_t out_len, struct attest_req *r)
{
    if (type != ATTEST_FN_GET_TOKEN && type != ATTEST_FN_GET_TOKEN_SIZE) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    if (in_len != attest_fns[type].n_in ||
        out_len != attest_fns[type].n_out ||
        in[0].len != sizeof(*r) ||
        out[0].len != sizeof(struct attest_rsp)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(r, in[0].base, sizeof(*r));
    if (!attest_challenge_ok(r->challenge_size) ||
        (in_len == 2 && in[1].len != r->challenge_size)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return PSA_SUCCESS;
}

/*
 * Claims map at the start of the token vector, signed in place; an
 * empty vector is INVALID_ARGUMENT, a short one BUFFER_TOO_SMALL
 */
static psa_status_t attest_build(const struct eat_claims *c,
                                 psa_outvec *tok, size_t size)
{
    size_t plen = 0;
    size_t tlen = 0;
    psa_status_t st;

    if (tok->len == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    if (size > tok->len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    st = eat_encode_claims(c, tok->base, tok->len, &plen);
    if (st == PSA_SUCCESS) {
        st = eat_cose_sign1(tok->base, plen, attest_sign, NULL, tok->base,
                            tok->len, &tlen);
    }
    return st == PSA_SUCCESS && tlen != size ? PSA_ERROR_GENERIC_ERROR
                                             : st;
}

psa_status_t svc_attest_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out,
                             size_t out_len)
{
    struct eat_claims c;
    char ver[ATTEST_VERSION_MAX];
    struct attest_req r = { 0 };
    struct attest_rsp rsp = { 0 };
    size_t size = 0;
    psa_status_t st = attest_unpack(type, in, in_len, out, out_len, &r);

    if (st == PSA_SUCCESS) {
        st = attest_claims(&c, in_len == 2 ? in[1].base : NULL,
                           r.challenge_size, ver);
    }
    if (st == PSA_SUCCESS) {
        st = attest_token_size(&c, &size);
        rsp.token_size = (uint32_t)size;
    }
    if (st == PSA_SUCCESS && type == ATTEST_FN_GET_TOKEN) {
        st = attest_build(&c, &out[1], size);
    }
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    if (st == PSA_SUCCESS) {
        memcpy(out[0].base, &rsp, sizeof(rsp));
        out[0].len = sizeof(rsp);
    }
    if (st == PSA_SUCCESS && out_len == 2) {
        out[1].len = rsp.token_size;
    }
    return st;
}

void svc_attest_client_reset(void)
{
}
