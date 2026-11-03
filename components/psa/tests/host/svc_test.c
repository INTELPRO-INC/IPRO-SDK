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
 * Harness of the svc_crypto host test. Every vector gets its own
 * heap copy (ASan sees over-reads and over-writes) and every call is
 * checked against the SPM out contract.
 */
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "hw_poll.h"
#include "svc_crypto.h"
#include "svc_test.h"

static int g_contract_bad;
static int g_calls;
static uint64_t g_rng = 0x9E3779B97F4A7C15ull;

const uint8_t abc_sha256[32] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40,
    0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17,
    0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad,
};
const uint8_t aes_key[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};
const uint8_t aes_pt[16] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
    0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
};
const uint8_t aes_ct[16] = {
    0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
    0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
};

int mbedtls_hardware_poll(void *data, unsigned char *out, size_t len,
                          size_t *olen)
{
    (void)data;
    for (size_t i = 0; i < len; i++) {
        g_rng ^= g_rng << 13;
        g_rng ^= g_rng >> 7;
        g_rng ^= g_rng << 17;
        out[i] = (uint8_t)g_rng;
    }
    *olen = len;
    return 0;
}

void t_check(int cond, const char *what)
{
    check(cond, what);
}

int t_summary(void)
{
    return check_summary();
}

void t_info(const char *fmt, ...)
{
    va_list ap;

    if (getenv("PSA_TEST_VERBOSE") == NULL) {
        return;
    }
    va_start(ap, fmt);
    printf("     ");
    vprintf(fmt, ap);
    printf("\n");
    va_end(ap);
}

int t_contract_bad(void)
{
    return g_contract_bad;
}

int t_calls(void)
{
    return g_calls;
}

/* Out contract: lens 0 on failure; rsp and len <= cap on success */
static void contract(psa_status_t st, const psa_outvec *out,
                     size_t out_len, const size_t *cap)
{
    int ok = 1;

    for (size_t i = 0; i < out_len; i++) {
        if (st != PSA_SUCCESS && out[i].len != 0) {
            ok = 0;
        }
        if (st == PSA_SUCCESS && out[i].len > cap[i]) {
            ok = 0;
        }
    }
    if (st == PSA_SUCCESS && out_len != 0 &&
        out[0].len != sizeof(struct psa_crypto_rsp)) {
        ok = 0;
    }
    g_calls++;
    if (!ok) {
        g_contract_bad++;
        printf("FAIL out contract broken, status %d\n", (int)st);
    }
}

psa_status_t t_raw(int32_t fn, const struct buf *in, size_t in_len,
                   struct buf *out, size_t out_len)
{
    psa_invec iv[PSA_MAX_IOVEC];
    psa_outvec ov[PSA_MAX_IOVEC];
    size_t cap[PSA_MAX_IOVEC];
    psa_status_t st;

    for (size_t i = 0; i < in_len; i++) {
        void *c = malloc(in[i].n ? in[i].n : 1);

        memcpy(c, in[i].p, in[i].n);
        iv[i].base = c;
        iv[i].len = in[i].n;
    }
    for (size_t i = 0; i < out_len; i++) {
        ov[i].base = calloc(1, out[i].n ? out[i].n : 1);
        ov[i].len = out[i].n;
        cap[i] = out[i].n;
    }
    st = svc_crypto_call(fn, iv, in_len, ov, out_len);
    contract(st, ov, out_len, cap);
    for (size_t i = 0; i < out_len; i++) {
        if (st == PSA_SUCCESS) {
            memcpy((void *)out[i].p, ov[i].base, ov[i].len);
        }
        out[i].n = ov[i].len;
        free(ov[i].base);
    }
    for (size_t i = 0; i < in_len; i++) {
        free((void *)iv[i].base);
    }
    return st;
}

psa_status_t t_rpc(int32_t fn, const struct psa_crypto_req *req,
                   const struct buf *data, size_t n, void *o,
                   size_t *olen, struct psa_crypto_rsp *rsp)
{
    struct buf in[PSA_MAX_IOVEC];
    struct buf out[2];
    struct psa_crypto_rsp r;
    psa_status_t st;

    memset(&r, 0, sizeof(r));
    in[0].p = req;
    in[0].n = sizeof(*req);
    for (size_t i = 0; i < n; i++) {
        in[i + 1] = data[i];
    }
    out[0].p = &r;
    out[0].n = sizeof(r);
    if (olen != NULL) {
        out[1].p = o;
        out[1].n = *olen;
    }
    st = t_raw(fn, in, n + 1, out, olen != NULL ? 2 : 1);
    if (olen != NULL) {
        *olen = out[1].n;
    }
    if (st != PSA_SUCCESS) {
        memset(&r, 0, sizeof(r));
    }
    if (rsp != NULL) {
        *rsp = r;
    }
    return st;
}

struct psa_crypto_req t_req(uint32_t key, uint32_t alg, uint32_t h)
{
    struct psa_crypto_req r;

    memset(&r, 0, sizeof(r));
    r.key_id = key;
    r.alg = alg;
    r.op_handle = h;
    return r;
}

psa_status_t t_op0(int32_t fn, uint32_t h)
{
    struct psa_crypto_req r = t_req(0, 0, h);

    return t_rpc(fn, &r, NULL, 0, NULL, NULL, NULL);
}

/* an id with one data vector and no output, on handle h */
psa_status_t t_op1(int32_t fn, uint32_t h)
{
    struct psa_crypto_req r = t_req(0, 0, h);
    struct buf d = { "x", 1 };

    return t_rpc(fn, &r, &d, 1, NULL, NULL, NULL);
}

uint32_t t_hash_setup(void)
{
    struct psa_crypto_req r = t_req(0, PSA_ALG_SHA_256, 0);
    struct psa_crypto_rsp rsp;

    return t_rpc(CRYPTO_FN_HASH_SETUP, &r, NULL, 0, NULL, NULL, &rsp) ==
           PSA_SUCCESS ? rsp.op_handle : 0;
}

/* free operation slots, counted by filling them with hash setups */
size_t t_free_slots(void)
{
    uint32_t h[SLOTS + 1];
    size_t n = 0;

    while (n <= SLOTS && (h[n] = t_hash_setup()) != 0) {
        n++;
    }
    for (size_t i = 0; i < n; i++) {
        t_op0(CRYPTO_FN_HASH_ABORT, h[i]);
    }
    return n;
}

static uint32_t new_key(int32_t fn, psa_key_type_t type, size_t bits,
                        psa_key_usage_t usage, psa_algorithm_t alg,
                        const struct buf *d)
{
    struct psa_crypto_req r = t_req(0, 0, 0);
    struct psa_crypto_rsp rsp;

    r.attr.type = type;
    r.attr.bits = (uint32_t)bits;
    r.attr.usage = usage;
    r.attr.alg = alg;
    return t_rpc(fn, &r, d, d != NULL ? 1 : 0, NULL, NULL, &rsp) ==
           PSA_SUCCESS ? rsp.key_id : 0;
}

uint32_t t_import(psa_key_type_t type, size_t bits, psa_key_usage_t usage,
                  psa_algorithm_t alg, const void *key, size_t len)
{
    struct buf d = { key, len };

    return new_key(CRYPTO_FN_KEY_IMPORT, type, bits, usage, alg, &d);
}

uint32_t t_generate(psa_key_type_t type, size_t bits,
                    psa_key_usage_t usage, psa_algorithm_t alg)
{
    return new_key(CRYPTO_FN_KEY_GENERATE, type, bits, usage, alg, NULL);
}

psa_status_t t_destroy(uint32_t key)
{
    struct psa_crypto_req r = t_req(key, 0, 0);

    return t_rpc(CRYPTO_FN_KEY_DESTROY, &r, NULL, 0, NULL, NULL, NULL);
}
