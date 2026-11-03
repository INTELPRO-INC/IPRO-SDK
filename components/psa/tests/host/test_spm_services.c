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
 * Host test for the PSA call path of spm_services.c (-DSPM_HOST):
 * iovec checks, bounce layout and the out contract. NSPE OCRAM is a
 * host array behind the spm_copy_in/out stubs below.
 */
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "spm_log.h"
#include "huk.h"
#include "spm_services.h"
#include "svc_attest.h"
#include "svc_crypto.h"
#include "svc_fwu.h"
#include "svc_its.h"
#include "svc_platform.h"

#define OCRAM      PSA_NSPE_OCRAM_BASE
#define IN_VEC     (OCRAM + 0x0000u)
#define OUT_VEC    (OCRAM + 0x0100u)
#define IN_BUF     (OCRAM + 0x1000u)
#define OUT_BUF    (OCRAM + 0x4000u)
#define OUT_BUF2   (OCRAM + 0x6000u)
#define TEST_SVC   PSA_HANDLE_PLATFORM
#define MAX_VEC    5u

static uint8_t g_mem[PSA_NSPE_OCRAM_SIZE];
static int g_copy_out_calls;

/* what the test service saw and does */
static int g_calls;
static size_t g_in_len;
static size_t g_out_len;
static size_t g_in_lens[MAX_VEC];
static const uint8_t *g_in_base[MAX_VEC];
static uint8_t *g_out_base[MAX_VEC];
static size_t g_out_cap[MAX_VEC];
static int g_out_zero;
static int g_in_ok;
static size_t g_write_len[MAX_VEC];
static psa_status_t g_status;

/* svc_* init and call, huk and spm_fatal stand-ins */
static psa_status_t g_crypto_init = PSA_SUCCESS;
static psa_status_t g_its_init = PSA_SUCCESS;
static psa_status_t g_platform_init = PSA_SUCCESS;
static int g_huk_init;
static int g_huk_lock = 1;
/* init steps in call order: H huk_init, I ITS, P platform, L lock,
 * F firmware-update staging */
static char g_init_log[16];
static size_t g_init_n;

static void init_log(char tag)
{
    if (g_init_n < sizeof(g_init_log) - 1) {
        g_init_log[g_init_n++] = tag;
    }
}
static int g_crypto_calls;
static int g_its_calls;
static int g_attest_calls;
static int g_platform_calls;
static int g_fwu_calls;
static int g_its_inits;
static int g_platform_inits;
static int g_fatal;
static jmp_buf g_fatal_jmp;

psa_status_t svc_crypto_init(void)
{
    return g_crypto_init;
}

int huk_init(void)
{
    init_log('H');
    return g_huk_init;
}

int huk_lock(void)
{
    init_log('L');
    return g_huk_lock;
}

psa_status_t svc_its_init(void)
{
    init_log('I');
    g_its_inits++;
    return g_its_init;
}

psa_status_t svc_platform_init(void)
{
    init_log('P');
    g_platform_inits++;
    return g_platform_init;
}

void svc_fwu_init(void)
{
    init_log('F');
}

/* client reset hooks log their tag here, in call order */
static char g_reset_log[16];
static size_t g_reset_n;

static void reset_log(char tag)
{
    if (g_reset_n < sizeof(g_reset_log) - 1) {
        g_reset_log[g_reset_n++] = tag;
    }
}

void svc_crypto_client_reset(void)
{
    reset_log('C');
}

void svc_its_client_reset(void)
{
    reset_log('I');
}

void svc_attest_client_reset(void)
{
    reset_log('A');
}

void svc_platform_client_reset(void)
{
    reset_log('P');
}

void svc_fwu_client_reset(void)
{
    reset_log('F');
}

static void hook_a(void)
{
    reset_log('a');
}

static psa_status_t zero_lens(psa_outvec *out, size_t out_len,
                              psa_status_t st)
{
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    return st;
}

psa_status_t svc_attest_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out, size_t out_len)
{
    (void)type;
    (void)in;
    (void)in_len;
    g_attest_calls++;
    return zero_lens(out, out_len, PSA_ERROR_NOT_PERMITTED);
}

psa_status_t svc_platform_call(int32_t type, const psa_invec *in,
                               size_t in_len, psa_outvec *out,
                               size_t out_len)
{
    (void)type;
    (void)in;
    (void)in_len;
    g_platform_calls++;
    return zero_lens(out, out_len, PSA_ERROR_BAD_STATE);
}

psa_status_t svc_fwu_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out, size_t out_len)
{
    (void)type;
    (void)in;
    (void)in_len;
    g_fwu_calls++;
    return zero_lens(out, out_len, PSA_ERROR_STORAGE_FAILURE);
}

psa_status_t svc_crypto_call(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out,
                             size_t out_len)
{
    (void)type;
    (void)in;
    (void)in_len;
    g_crypto_calls++;
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    return PSA_ERROR_GENERIC_ERROR;
}

psa_status_t svc_its_call(int32_t type, const psa_invec *in,
                          size_t in_len, psa_outvec *out, size_t out_len)
{
    (void)type;
    (void)in;
    (void)in_len;
    g_its_calls++;
    for (size_t i = 0; i < out_len; i++) {
        out[i].len = 0;
    }
    return PSA_ERROR_DOES_NOT_EXIST;
}

void spm_fatal(void)
{
    g_fatal++;
    longjmp(g_fatal_jmp, 1);
}

static uint8_t *host(uint32_t addr, uint32_t len)
{
    if (addr < OCRAM || len > PSA_NSPE_OCRAM_SIZE - (addr - OCRAM)) {
        printf("FAIL test reached NSPE address %08x\n", (unsigned)addr);
        exit(1);
    }
    return &g_mem[addr - OCRAM];
}

int spm_copy_in(void *dst, uint32_t nspe_src, uint32_t len)
{
    if (!spm_nspe_range_ok(nspe_src, len, 0)) {
        return -1;
    }
    memcpy(dst, host(nspe_src, len), len);
    return 0;
}

int spm_copy_out(uint32_t nspe_dst, const void *src, uint32_t len)
{
    g_copy_out_calls++;
    if (!spm_nspe_range_ok(nspe_dst, len, 1)) {
        return -1;
    }
    memcpy(host(nspe_dst, len), src, len);
    return 0;
}

static void put_u32(uint32_t addr, uint32_t v)
{
    uint8_t *p = host(addr, 4);

    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get_u32(uint32_t addr)
{
    const uint8_t *p = host(addr, 4);

    return p[0] | (p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void put_vec(uint32_t arr, uint32_t i, uint32_t base, uint32_t len)
{
    put_u32(arr + 8u * i, base);
    put_u32(arr + 8u * i + 4u, len);
}

static uint32_t vec_len(uint32_t arr, uint32_t i)
{
    return get_u32(arr + 8u * i + 4u);
}

/* in data byte at offset off of in vector i */
static uint8_t in_byte(uint32_t i, uint32_t off)
{
    return (uint8_t)(0x10u * (i + 1u) + off * 7u);
}

static psa_status_t test_svc(int32_t type, const psa_invec *in,
                             size_t in_len, psa_outvec *out, size_t out_len)
{
    (void)type;
    g_calls++;
    g_in_len = in_len;
    g_out_len = out_len;
    g_in_ok = 1;
    g_out_zero = 1;
    for (size_t i = 0; i < in_len; i++) {
        g_in_lens[i] = in[i].len;
        g_in_base[i] = in[i].base;
        for (size_t k = 0; k < in[i].len; k++) {
            if (((const uint8_t *)in[i].base)[k] !=
                in_byte((uint32_t)i, (uint32_t)k)) {
                g_in_ok = 0;
            }
        }
    }
    for (size_t i = 0; i < out_len; i++) {
        uint8_t *b = out[i].base;
        size_t n = g_write_len[i] < out[i].len ? g_write_len[i]
                                               : out[i].len;

        g_out_base[i] = b;
        g_out_cap[i] = out[i].len;
        for (size_t k = 0; k < out[i].len; k++) {
            if (b[k] != 0) {
                g_out_zero = 0;
            }
        }
        memset(b, 0x41 + (int)i, n);
        out[i].len = g_write_len[i];
    }
    return g_status;
}

static void fresh(psa_status_t status)
{
    memset(g_mem, 0xEE, sizeof(g_mem));
    memset(g_write_len, 0, sizeof(g_write_len));
    g_copy_out_calls = 0;
    g_calls = 0;
    g_in_len = 99;
    g_out_len = 99;
    g_status = status;
}

static void fill_in(uint32_t i, uint32_t addr, uint32_t len)
{
    uint8_t *p = host(addr, len);

    for (uint32_t k = 0; k < len; k++) {
        p[k] = in_byte(i, k);
    }
    put_vec(IN_VEC, i, addr, len);
}

static int all(uint32_t addr, uint32_t len, uint8_t v)
{
    const uint8_t *p = host(addr, len);

    for (uint32_t k = 0; k < len; k++) {
        if (p[k] != v) {
            return 0;
        }
    }
    return 1;
}

static psa_status_t call(psa_handle_t h, uint32_t in_len, uint32_t out_len)
{
    return spm_psa_call(h, 0, IN_VEC, in_len, OUT_VEC, out_len);
}

static void test_dispatch_and_version(void)
{
    fresh(PSA_SUCCESS);
    check(call(PSA_HANDLE_CRYPTO, 0, 0) == PSA_ERROR_GENERIC_ERROR &&
          g_crypto_calls == 1, "CRYPTO handle runs svc_crypto_call");
    check(call(PSA_HANDLE_ITS, 0, 0) == PSA_ERROR_DOES_NOT_EXIST &&
          g_its_calls == 1, "ITS handle runs svc_its_call");
    check(call(PSA_HANDLE_PLATFORM, 0, 0) == PSA_ERROR_BAD_STATE &&
          g_platform_calls == 1, "PLATFORM handle runs svc_platform_call");
    check(call(PSA_HANDLE_FWU, 0, 0) == PSA_ERROR_STORAGE_FAILURE &&
          g_fwu_calls == 1, "FWU handle runs svc_fwu_call");
    put_vec(OUT_VEC, 0, OUT_BUF, 16);
    check(call(PSA_HANDLE_ATTEST, 0, 1) == PSA_ERROR_NOT_PERMITTED &&
          g_attest_calls == 1, "ATTEST handle runs svc_attest_call");
    check(all(OUT_BUF, 16, 0xEE) && vec_len(OUT_VEC, 0) == 0,
          "service failure: out data untouched, len 0");
    check(spm_psa_version(PSA_HANDLE_ITS) == 1u, "version of ITS is 1");
    check(spm_psa_version(0x40000999u) == PSA_VERSION_NONE &&
          PSA_VERSION_NONE == 0u, "unknown sid: PSA_VERSION_NONE (0)");
}

static void test_refusals(void)
{
    fresh(PSA_SUCCESS);
    check(call(0x40000999, 0, 0) == PSA_ERROR_INVALID_HANDLE &&
          g_calls == 0, "unknown handle: INVALID_HANDLE");
    check(call(TEST_SVC, 5, 0) == PSA_ERROR_INVALID_ARGUMENT &&
          call(TEST_SVC, 0, 5) == PSA_ERROR_INVALID_ARGUMENT &&
          g_calls == 0, "more than 4 vectors: INVALID_ARGUMENT");
    check(spm_psa_call(TEST_SVC, 0, PSA_SPE_RAM_BASE, 1, OUT_VEC, 0) ==
          PSA_ERROR_INVALID_ARGUMENT && g_calls == 0,
          "in_vec array in SPE RAM: INVALID_ARGUMENT");

    fresh(PSA_SUCCESS);
    put_vec(OUT_VEC, 0, PSA_SPE_RAM_BASE, 16);
    g_write_len[0] = 16;
    check(call(TEST_SVC, 0, 1) == PSA_ERROR_INVALID_ARGUMENT &&
          g_calls == 0 && g_copy_out_calls == 0,
          "out base in SPE RAM: INVALID_ARGUMENT, no write");
    put_vec(OUT_VEC, 0, PSA_NSPE_FLASH_BASE, 16);
    check(call(TEST_SVC, 0, 1) == PSA_ERROR_INVALID_ARGUMENT &&
          g_calls == 0 && g_copy_out_calls == 0,
          "out base in NSPE flash: INVALID_ARGUMENT, no write");
}

static void test_empty_sides(void)
{
    fresh(PSA_SUCCESS);
    put_vec(OUT_VEC, 0, OUT_BUF, 8);
    g_write_len[0] = 4;
    check(call(TEST_SVC, 0, 1) == PSA_SUCCESS && g_calls == 1 &&
          g_in_len == 0 && g_out_len == 1,
          "in_len 0: service sees no in vector");
    check(all(OUT_BUF, 4, 0x41) && all(OUT_BUF + 4, 4, 0xEE) &&
          vec_len(OUT_VEC, 0) == 4, "in_len 0: 4 bytes out, len 4");

    fresh(PSA_SUCCESS);
    fill_in(0, IN_BUF, 16);
    check(call(TEST_SVC, 1, 0) == PSA_SUCCESS && g_in_len == 1 &&
          g_out_len == 0 && g_in_ok && g_copy_out_calls == 0,
          "out_len 0: in data seen, nothing written");

    fresh(PSA_SUCCESS);
    put_vec(IN_VEC, 0, 0, 0);
    put_vec(OUT_VEC, 0, 0, 0);
    check(call(TEST_SVC, 1, 1) == PSA_SUCCESS && g_in_lens[0] == 0 &&
          g_out_cap[0] == 0 && vec_len(OUT_VEC, 0) == 0,
          "{NULL, 0} vectors accepted");
}

static void test_limits(void)
{
    fresh(PSA_SUCCESS);
    fill_in(0, IN_BUF, 4095);
    fill_in(1, IN_BUF + 0x1800u, 1);
    check(call(TEST_SVC, 2, 0) == PSA_SUCCESS && g_in_lens[0] == 4095 &&
          g_in_lens[1] == 1 && g_in_ok,
          "in {4095, 1} = 4096: ok, data intact");
    check(((uintptr_t)g_in_base[1] & 7u) == 0 &&
          g_in_base[1] >= g_in_base[0] + 4095,
          "in slices 8-aligned, no overlap");

    fresh(PSA_SUCCESS);
    fill_in(0, IN_BUF, 4095);
    fill_in(1, IN_BUF + 0x1800u, 2);
    check(call(TEST_SVC, 2, 0) == PSA_ERROR_INSUFFICIENT_MEMORY &&
          g_calls == 0, "in total 4097: INSUFFICIENT_MEMORY");

    fresh(PSA_SUCCESS);
    fill_in(0, IN_BUF, 1);
    fill_in(1, IN_BUF + 0x10u, 1);
    fill_in(2, IN_BUF + 0x20u, 1);
    fill_in(3, IN_BUF + 0x30u, 4093);
    check(call(TEST_SVC, 4, 0) == PSA_SUCCESS && g_in_ok &&
          g_in_base[3] == g_in_base[0] + 24,
          "in {1, 1, 1, 4093}: 21 pad bytes fit");

    fresh(PSA_SUCCESS);
    put_vec(OUT_VEC, 0, OUT_BUF, 4095);
    put_vec(OUT_VEC, 1, OUT_BUF2, 1);
    g_write_len[0] = 4095;
    g_write_len[1] = 1;
    check(call(TEST_SVC, 0, 2) == PSA_SUCCESS &&
          all(OUT_BUF, 4095, 0x41) && all(OUT_BUF2, 1, 0x42) &&
          all(OUT_BUF2 + 1, 1, 0xEE),
          "out {4095, 1} = 4096: ok, data copied");
    put_vec(OUT_VEC, 1, OUT_BUF2, 2);
    g_calls = 0;
    check(call(TEST_SVC, 0, 2) == PSA_ERROR_INSUFFICIENT_MEMORY &&
          g_calls == 0, "out total 4097: INSUFFICIENT_MEMORY");
}

static void test_out_contract(void)
{
    fresh(PSA_SUCCESS);
    put_vec(OUT_VEC, 0, OUT_BUF, 4);
    put_vec(OUT_VEC, 1, OUT_BUF, 4);
    g_write_len[0] = 4;
    g_write_len[1] = 2;
    check(call(TEST_SVC, 0, 2) == PSA_SUCCESS &&
          g_out_base[0] != g_out_base[1],
          "duplicate out_vecs: separate bounce slices");
    check(all(OUT_BUF, 2, 0x42) && all(OUT_BUF + 2, 2, 0x41) &&
          vec_len(OUT_VEC, 0) == 4 && vec_len(OUT_VEC, 1) == 2,
          "duplicate out_vecs: copied in order, both lens");

    fresh(PSA_SUCCESS);
    put_vec(OUT_VEC, 0, OUT_BUF, 8);
    g_write_len[0] = 100;
    check(call(TEST_SVC, 0, 1) == PSA_SUCCESS && g_out_zero &&
          all(OUT_BUF, 8, 0x41) && all(OUT_BUF + 8, 1, 0xEE) &&
          vec_len(OUT_VEC, 0) == 8,
          "len above capacity clamped, out bounce zeroed on entry");

    fresh(PSA_ERROR_GENERIC_ERROR);
    put_vec(OUT_VEC, 0, OUT_BUF, 8);
    g_write_len[0] = 0;
    check(call(TEST_SVC, 0, 1) == PSA_ERROR_GENERIC_ERROR &&
          all(OUT_BUF, 8, 0xEE) && vec_len(OUT_VEC, 0) == 0,
          "service failure: caller data untouched, len 0");

    fresh(PSA_ERROR_BUFFER_TOO_SMALL);
    put_vec(OUT_VEC, 0, OUT_BUF, 8);
    g_write_len[0] = 8;
    check(call(TEST_SVC, 0, 1) == PSA_ERROR_BUFFER_TOO_SMALL &&
          all(OUT_BUF, 8, 0xEE) && vec_len(OUT_VEC, 0) == 8,
          "failure with len set: no data out, clamped len back");
}

static void test_client_reset(void)
{
    spm_client_reset();
    check(strcmp(g_reset_log, "CIAPF") == 0,
          "services init registers crypto, ITS, attest, platform, FWU "
          "resets");
    check(spm_register_client_reset(NULL) == -1, "NULL hook refused");
    check(spm_register_client_reset(hook_a) == 0 &&
          spm_register_client_reset(hook_a) == 0 &&
          spm_register_client_reset(hook_a) == 0,
          "5 hooks after init: hooks 6..8 accepted");
    check(spm_register_client_reset(hook_a) == -1,
          "8 hooks registered: the 9th refused");
    memset(g_reset_log, 0, sizeof(g_reset_log));
    g_reset_n = 0;
    spm_client_reset();
    check(strcmp(g_reset_log, "CIAPFaaa") == 0,
          "hooks run once each, in registration order");
}

static void test_crypto_init_fatal(void)
{
    g_crypto_init = PSA_ERROR_HARDWARE_FAILURE;
    if (setjmp(g_fatal_jmp) == 0) {
        spm_services_init();
    }
    check(g_fatal == 1, "crypto init failure is fatal");
    g_crypto_init = PSA_SUCCESS;
}

/* huk_lock after the last derivation; -1 (route open) is fatal */
static void test_huk_lock(void)
{
    g_init_n = 0;
    memset(g_init_log, 0, sizeof(g_init_log));
    g_huk_lock = -1;
    g_fatal = 0;
    if (setjmp(g_fatal_jmp) == 0) {
        spm_services_init();
    }
    check(strcmp(g_init_log, "HIPL") == 0 && g_fatal == 1 &&
          spm_psa_version(PSA_HANDLE_CRYPTO) == PSA_VERSION_NONE &&
          spm_psa_version(PSA_HANDLE_ITS) == PSA_VERSION_NONE,
          "route open: spm_fatal before any service is registered");
    g_huk_lock = 1;
    g_fatal = 0;
    g_its_inits = 0;
    g_platform_inits = 0;
    g_init_n = 0;
    memset(g_init_log, 0, sizeof(g_init_log));
}

int main(void)
{
    test_huk_lock();
    g_huk_init = -1;
    g_its_init = PSA_ERROR_STORAGE_FAILURE;
    g_platform_init = PSA_ERROR_HARDWARE_FAILURE;
    if (setjmp(g_fatal_jmp) == 0) {
        spm_services_init();
    }
    check(strcmp(g_init_log, "HIPLF") == 0,
          "init order: huk_init, ITS, platform, huk_lock, FWU");
    check(g_fatal == 0 && g_its_inits == 1 && g_platform_inits == 1 &&
          spm_psa_version(PSA_HANDLE_ITS) == 1u &&
          spm_psa_version(PSA_HANDLE_ATTEST) == 1u &&
          spm_psa_version(PSA_HANDLE_PLATFORM) == 1u &&
          spm_psa_version(PSA_HANDLE_FWU) == 1u,
          "HUK, ITS, platform init failures not fatal, all registered");
    test_dispatch_and_version();
    spm_register_service(TEST_SVC, 1u, test_svc);
    test_refusals();
    test_empty_sides();
    test_limits();
    test_out_contract();
    test_client_reset();
    test_crypto_init_fatal();
    return check_summary();
}
