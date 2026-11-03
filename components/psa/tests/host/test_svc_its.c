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
 * Host test of svc_its.c on its_store and the RAM backend: RPC
 * decoding, the SPM out contract and the NSPE namespace.
 */
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "its_lfs.h"
#include "its_ram.h"
#include "its_store.h"
#include "kdf.h"
#include "psa/crypto.h"
#include "psa_ipro/abi.h"
#include "psa_ipro/its_rpc.h"
#include "svc_its.h"

_Static_assert(sizeof(struct psa_storage_info_t) == 12,
               "public psa_storage_info_t {capacity, size, flags}");

struct iv {
    const void *p;
    size_t n;
};

struct ov {
    void *p;
    size_t n;
};

static int g_contract_bad;
static int g_calls;
static uint8_t g_data[ITS_MAX_OBJECT + 1];

/* svc_its_init stand-ins: backend, KDF and RNG, each can fail */
static int g_no_backend;
static int g_kdf_fail;
static psa_status_t g_rng_status = PSA_SUCCESS;
static char g_kdf_label[16];
static int g_mounts;

const struct its_backend *its_lfs_mount(void)
{
    g_mounts++;
    return g_no_backend ? NULL : &its_ram;
}

int kdf_derive(const char *label, uint8_t *out, size_t out_len)
{
    strncpy(g_kdf_label, label, sizeof(g_kdf_label) - 1);
    memset(out, g_kdf_fail ? 0 : 0x5c, out_len);
    return g_kdf_fail ? -1 : 0;
}

psa_status_t psa_generate_random(uint8_t *output, size_t output_size)
{
    memset(output, 0x3a, output_size);
    return g_rng_status;
}

/*
 * svc_its_call on heap copies; out[i].n is the capacity in, the
 * returned len out. Checks the out contract of spm.h.
 */
static psa_status_t raw(int32_t fn, const struct iv *in, size_t in_len,
                        struct ov *out, size_t out_len)
{
    psa_invec iv[PSA_MAX_IOVEC];
    psa_outvec ov[PSA_MAX_IOVEC];
    psa_status_t st;

    for (size_t i = 0; i < in_len; i++) {
        void *p = malloc(in[i].n ? in[i].n : 1);

        memcpy(p, in[i].p, in[i].n);
        iv[i].base = p;
        iv[i].len = in[i].n;
    }
    for (size_t i = 0; i < out_len; i++) {
        ov[i].base = calloc(1, out[i].n ? out[i].n : 1);
        ov[i].len = out[i].n;
    }
    st = svc_its_call(fn, iv, in_len, ov, out_len);
    g_calls++;
    for (size_t i = 0; i < out_len; i++) {
        if (ov[i].len > out[i].n || (st != PSA_SUCCESS && ov[i].len)) {
            g_contract_bad++;
        }
        if (st == PSA_SUCCESS && out[i].p != NULL) {
            memcpy(out[i].p, ov[i].base, ov[i].len);
        }
        out[i].n = ov[i].len;
        free(ov[i].base);
    }
    for (size_t i = 0; i < in_len; i++) {
        free((void *)iv[i].base);
    }
    return st;
}

static struct its_req req(uint64_t uid, uint32_t off, uint32_t flags,
                          uint32_t size)
{
    struct its_req r = { uid, off, flags, size, 0 };

    return r;
}

static psa_status_t set(uint64_t uid, const void *d, uint32_t len,
                        uint32_t flags)
{
    struct its_req r = req(uid, 0, flags, len);
    struct iv in[2] = { { &r, sizeof(r) }, { d, len } };

    return raw(ITS_FN_SET, in, 2, NULL, 0);
}

static psa_status_t get(uint64_t uid, uint32_t off, void *buf,
                        uint32_t len, size_t *n, struct its_rsp *rsp)
{
    struct its_req r = req(uid, off, 0, len);
    struct iv in[1] = { { &r, sizeof(r) } };
    struct ov out[2] = { { rsp, sizeof(*rsp) }, { buf, len } };
    psa_status_t st = raw(ITS_FN_GET, in, 1, out, 2);

    *n = out[1].n;
    return st;
}

static psa_status_t info(uint64_t uid, struct its_rsp *rsp)
{
    struct its_req r = req(uid, 0, 0, 0);
    struct iv in[1] = { { &r, sizeof(r) } };
    struct ov out[1] = { { rsp, sizeof(*rsp) } };

    return raw(ITS_FN_GET_INFO, in, 1, out, 1);
}

static psa_status_t rm(uint64_t uid)
{
    struct its_req r = req(uid, 0, 0, 0);
    struct iv in[1] = { { &r, sizeof(r) } };

    return raw(ITS_FN_REMOVE, in, 1, NULL, 0);
}

static void test_layout(void)
{
    check(sizeof(struct psa_storage_info_t) == 12,
          "public psa_storage_info_t: 12 bytes");
    check(sizeof(struct its_req) == 24 && sizeof(struct its_rsp) == 12,
          "its_req 24 bytes, its_rsp 12 bytes");
    check(PSA_STORAGE_FLAG_WRITE_ONCE == ITS_FLAG_WRITE_ONCE &&
          ITS_MAX_OBJECT == ITS_STORE_MAX, "flag and size limits agree");
}

static void test_decode(void)
{
    static const uint8_t shape[5][2] = {
        { 0, 0 }, { 2, 0 }, { 1, 2 }, { 1, 1 }, { 1, 0 },
    };
    static const int32_t bad_ids[3] = { 0, 5, -1 };
    struct its_req r = req(1, 0, 0, 4);
    uint8_t big[sizeof(struct its_req) + 1] = { 0 };
    struct its_rsp rsp;
    uint8_t buf[8];
    struct iv in[3] = { { &r, sizeof(r) }, { "abcd", 4 }, { "x", 1 } };
    struct ov out[3];
    int bad = 0;

    for (size_t i = 0; i < 3; i++) {
        out[0] = (struct ov){ &rsp, sizeof(rsp) };
        out[1] = (struct ov){ buf, 4 };
        bad |= raw(bad_ids[i], in, 1, out, 2) != PSA_ERROR_NOT_SUPPORTED;
    }
    check(!bad, "ids 0, 5, -1: NOT_SUPPORTED");

    for (int32_t fn = ITS_FN_SET; fn <= ITS_FN_REMOVE; fn++) {
        size_t ni = shape[fn][0];
        size_t no = shape[fn][1];
        const size_t tries[4][2] = {
            { ni + 1, no }, { ni - 1, no }, { ni, no + 1 },
            { ni, no ? no - 1 : 3 },
        };

        for (size_t t = 0; t < 4; t++) {
            out[0] = (struct ov){ &rsp, sizeof(rsp) };
            out[1] = (struct ov){ buf, 4 };
            out[2] = (struct ov){ buf, 4 };
            bad |= raw(fn, in, tries[t][0], out, tries[t][1]) !=
                   PSA_ERROR_INVALID_ARGUMENT;
        }
    }
    check(!bad, "vector counts other than the id's: INVALID_ARGUMENT");

    memcpy(big, &r, sizeof(r));
    in[0] = (struct iv){ big, sizeof(r) - 1 };
    bad |= raw(ITS_FN_REMOVE, in, 1, NULL, 0) != PSA_ERROR_INVALID_ARGUMENT;
    in[0].n = sizeof(r) + 1;
    bad |= raw(ITS_FN_REMOVE, in, 1, NULL, 0) != PSA_ERROR_INVALID_ARGUMENT;
    in[0] = (struct iv){ &r, sizeof(r) };
    out[0] = (struct ov){ &rsp, sizeof(rsp) - 1 };
    bad |= raw(ITS_FN_GET_INFO, in, 1, out, 1) != PSA_ERROR_INVALID_ARGUMENT;
    check(!bad, "req or rsp vector of the wrong size: INVALID_ARGUMENT");
}

static void test_fields(void)
{
    const struct its_req bad_info[4] = {
        { 1, 1, 0, 0, 0 }, { 1, 0, 1, 0, 0 }, { 1, 0, 0, 1, 0 },
        { 1, 0, 0, 0, 1 },
    };
    struct its_req r;
    struct iv in[2] = { { &r, sizeof(r) }, { "abcd", 4 } };
    struct its_rsp rsp;
    struct ov out[2] = { { &rsp, sizeof(rsp) }, { NULL, 4 } };
    int bad = 0;

    for (size_t i = 0; i < 4; i++) {
        r = bad_info[i];
        out[0].n = sizeof(rsp);
        bad |= raw(ITS_FN_GET_INFO, in, 1, out, 1) !=
               PSA_ERROR_INVALID_ARGUMENT;
        bad |= raw(ITS_FN_REMOVE, in, 1, NULL, 0) !=
               PSA_ERROR_INVALID_ARGUMENT;
    }
    check(!bad, "GET_INFO / REMOVE: offset, flags, size, reserved != 0");

    r = req(1, 1, 0, 4);
    bad |= raw(ITS_FN_SET, in, 2, NULL, 0) != PSA_ERROR_INVALID_ARGUMENT;
    r = req(1, 0, 0, 3);
    bad |= raw(ITS_FN_SET, in, 2, NULL, 0) != PSA_ERROR_INVALID_ARGUMENT;
    check(!bad && rm(1) == PSA_ERROR_DOES_NOT_EXIST,
          "SET: offset != 0 or size != data length refused, no object");

    r = req(1, 0, 1, 4);
    out[0].n = sizeof(rsp);
    out[1].n = 4;
    bad |= raw(ITS_FN_GET, in, 1, out, 2) != PSA_ERROR_INVALID_ARGUMENT;
    r = req(1, 0, 0, 3);
    out[0].n = sizeof(rsp);
    out[1].n = 4;
    bad |= raw(ITS_FN_GET, in, 1, out, 2) != PSA_ERROR_INVALID_ARGUMENT;
    check(!bad, "GET: flags != 0 or size != out capacity refused");
}

static void test_calls(void)
{
    struct its_rsp rsp = { 0, 0, 0 };
    uint8_t buf[ITS_MAX_OBJECT];
    size_t n = 0;

    check(set(0x42, g_data, 100, 0) == PSA_SUCCESS &&
          get(0x42, 0, buf, 100, &n, &rsp) == PSA_SUCCESS && n == 100 &&
          memcmp(buf, g_data, 100) == 0 && rsp.size == 100 &&
          rsp.capacity == 100 && rsp.flags == 0, "SET / GET round trip");
    check(get(0x42, 10, buf, 20, &n, &rsp) == PSA_SUCCESS && n == 20 &&
          memcmp(buf, g_data + 10, 20) == 0, "GET offset 10 len 20");
    check(get(0x42, 95, buf, 20, &n, &rsp) == PSA_SUCCESS && n == 5,
          "GET past the end: data out len 5");
    check(get(0x42, 101, buf, 20, &n, &rsp) == PSA_ERROR_INVALID_ARGUMENT,
          "GET offset > size: INVALID_ARGUMENT");
    check(info(0x42, &rsp) == PSA_SUCCESS && rsp.size == 100 &&
          rsp.capacity == 100 && rsp.flags == 0, "GET_INFO");
    check(rm(0x42) == PSA_SUCCESS && info(0x42, &rsp) ==
          PSA_ERROR_DOES_NOT_EXIST && get(0x42, 0, buf, 4, &n, &rsp) ==
          PSA_ERROR_DOES_NOT_EXIST && rm(0x42) == PSA_ERROR_DOES_NOT_EXIST,
          "REMOVE, then DOES_NOT_EXIST");

    check(set(0x43, g_data, 8, PSA_STORAGE_FLAG_WRITE_ONCE) == PSA_SUCCESS &&
          set(0x43, g_data, 8, 0) == PSA_ERROR_NOT_PERMITTED &&
          rm(0x43) == PSA_ERROR_NOT_PERMITTED && info(0x43, &rsp) ==
          PSA_SUCCESS && rsp.flags == PSA_STORAGE_FLAG_WRITE_ONCE,
          "WRITE_ONCE: SET and REMOVE NOT_PERMITTED");
    check(set(0x44, g_data, ITS_MAX_OBJECT, 0) == PSA_SUCCESS &&
          get(0x44, 0, buf, ITS_MAX_OBJECT, &n, &rsp) == PSA_SUCCESS &&
          n == ITS_MAX_OBJECT && memcmp(buf, g_data, n) == 0 &&
          set(0x45, g_data, ITS_MAX_OBJECT + 1, 0) ==
          PSA_ERROR_INVALID_ARGUMENT, "2048 bytes ok, 2049 refused");
    check(set(0, g_data, 1, 0) == PSA_ERROR_INVALID_ARGUMENT,
          "uid 0: INVALID_ARGUMENT");
}

static void test_namespace(void)
{
    struct its_rsp rsp;
    uint8_t buf[8];
    size_t n = 0;
    uint32_t got = 0;

    check(set(0x30, "nspe", 4, 0) == PSA_SUCCESS &&
          its_store_get(ITS_NS_KEY, 0x30, 0, buf, 8, &got, NULL) ==
          PSA_ERROR_DOES_NOT_EXIST, "RPC objects live in ITS_NS_NSPE");
    check(its_store_set(ITS_NS_KEY, 0x31, "secret", 6, 0) == PSA_SUCCESS &&
          get(0x31, 0, buf, 6, &n, &rsp) == PSA_ERROR_DOES_NOT_EXIST &&
          info(0x31, &rsp) == PSA_ERROR_DOES_NOT_EXIST &&
          rm(0x31) == PSA_ERROR_DOES_NOT_EXIST &&
          its_store_get(ITS_NS_KEY, 0x31, 0, buf, 8, &got, NULL) ==
          PSA_SUCCESS && got == 6, "KEY objects unreachable over RPC");
    svc_its_client_reset();
    check(get(0x30, 0, buf, 4, &n, &rsp) == PSA_SUCCESS && n == 4,
          "client reset keeps the objects");
    its_store_deinit();
    check(info(0x30, &rsp) == PSA_ERROR_STORAGE_FAILURE &&
          set(0x32, "x", 1, 0) == PSA_ERROR_STORAGE_FAILURE,
          "ITS down: STORAGE_FAILURE");
}

static void test_init(void)
{
    struct its_rsp rsp;

    g_no_backend = 1;
    check(svc_its_init() == PSA_ERROR_STORAGE_FAILURE &&
          info(0x40, &rsp) == PSA_ERROR_STORAGE_FAILURE,
          "init without backend: STORAGE_FAILURE, store down");
    g_no_backend = 0;
    g_kdf_fail = 1;
    g_mounts = 0;
    check(svc_its_init() == PSA_ERROR_HARDWARE_FAILURE &&
          info(0x40, &rsp) == PSA_ERROR_STORAGE_FAILURE && g_mounts == 0,
          "init, KDF failure (no HUK): HARDWARE_FAILURE, PSM not mounted");
    g_kdf_fail = 0;
    g_rng_status = PSA_ERROR_INSUFFICIENT_ENTROPY;
    check(svc_its_init() == PSA_ERROR_INSUFFICIENT_ENTROPY,
          "init, RNG failure: its status");
    g_rng_status = PSA_SUCCESS;
    check(svc_its_init() == PSA_SUCCESS && strcmp(g_kdf_label, "ITS") == 0 &&
          info(0x40, &rsp) == PSA_ERROR_DOES_NOT_EXIST,
          "init: KDF label ITS, store up");
}

int main(void)
{
    for (size_t i = 0; i < sizeof(g_data); i++) {
        g_data[i] = (uint8_t)(i * 13u + 1u);
    }
    test_init();
    test_layout();
    test_decode();
    test_fields();
    test_calls();
    test_namespace();
    if (getenv("PSA_TEST_VERBOSE") != NULL) {
        printf("     %d calls\n", g_calls);
    }
    check(g_contract_bad == 0, "out contract held on every call");
    return check_summary();
}
