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
 * Host test of kdf.c over a software AES-128-ECB huk_prf stub with
 * key 000102..0f; reference blocks come from mbedtls directly.
 */
#include <string.h>

#include "check.h"
#include "huk.h"
#include "kdf.h"
#include "spm_aes.h"

static const uint8_t g_huk[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};
static int g_prf_calls;
static int g_prf_fail_at;
static int g_prf_zero_in;

static int aes_ecb(const uint8_t in[16], uint8_t out[16])
{
    mbedtls_aes_context ctx;
    int ret;

    mbedtls_aes_init(&ctx);
    ret = mbedtls_aes_setkey_enc(&ctx, g_huk, 128);
    if (ret == 0) {
        ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, in, out);
    }
    mbedtls_aes_free(&ctx);
    return ret == 0 ? 0 : -1;
}

/* fails the g_prf_fail_at-th call (1-based) when not 0 */
int huk_prf(const uint8_t in[16], uint8_t out[16])
{
    uint8_t any = 0;

    for (size_t i = 0; i < 16; i++) {
        any |= in[i];
    }
    g_prf_zero_in += any == 0;
    g_prf_calls++;
    if (g_prf_fail_at != 0 && g_prf_calls == g_prf_fail_at) {
        memset(out, 0, 16);
        return -1;
    }
    return aes_ecb(in, out);
}

/* reference block i of label: AES(label padded || i big-endian) */
static void ref_block(const char *label, uint32_t i, uint8_t out[16])
{
    uint8_t in[16] = { 0 };

    memcpy(in, label, strlen(label));
    in[12] = (uint8_t)(i >> 24);
    in[13] = (uint8_t)(i >> 16);
    in[14] = (uint8_t)(i >> 8);
    in[15] = (uint8_t)i;
    aes_ecb(in, out);
}

static int all(const uint8_t *p, size_t n, uint8_t v)
{
    for (size_t i = 0; i < n; i++) {
        if (p[i] != v) {
            return 0;
        }
    }
    return 1;
}

static void test_stub(void)
{
    static const uint8_t pt[16] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
    };
    static const uint8_t ct[16] = {
        0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
        0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a,
    };
    uint8_t out[16];

    check(huk_prf(pt, out) == 0 && memcmp(out, ct, 16) == 0,
          "stub huk_prf = AES-128 key 00..0f (FIPS-197 C.1)");
}

static void test_its(void)
{
    static const uint8_t in1[16] = {
        'I', 'T', 'S', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x01,
    };
    static const uint8_t in2[16] = {
        'I', 'T', 'S', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, 0x00, 0x02,
    };
    uint8_t exp[32];
    uint8_t out[48];
    uint8_t b3[16];

    aes_ecb(in1, exp);
    aes_ecb(in2, exp + 16);
    memset(out, 0xA5, sizeof(out));
    g_prf_calls = 0;
    check(kdf_derive(KDF_LABEL_ITS, out, 32) == 0 &&
          memcmp(out, exp, 32) == 0 && all(out + 32, 16, 0xA5),
          "ITS 32 = AES(\"ITS\"..0 || 00000001) || AES(.. || 00000002)");
    check(g_prf_calls == 2, "ITS 32: two PRF calls");

    memset(out, 0xA5, sizeof(out));
    check(kdf_derive("ITS", out, 20) == 0 && memcmp(out, exp, 20) == 0 &&
          all(out + 20, 28, 0xA5), "ITS 20: prefix, no write past 20");

    ref_block("ITS", 3, b3);
    check(kdf_derive("ITS", out, 48) == 0 && memcmp(out, exp, 32) == 0 &&
          memcmp(out + 32, b3, 16) == 0, "ITS 48: third block counter 3");

    g_prf_calls = 0;
    check(kdf_derive("ITS", out, 0) == 0 && g_prf_calls == 0,
          "out_len 0: success, no PRF call");
}

static void test_labels(void)
{
    uint8_t its[16];
    uint8_t iak[16];
    uint8_t iak2[16];
    uint8_t ref[16];
    uint8_t out[16];

    check(kdf_derive(KDF_LABEL_ITS, its, 16) == 0 &&
          kdf_derive(KDF_LABEL_IAK, iak, 16) == 0 &&
          kdf_derive(KDF_LABEL_IAK2, iak2, 16) == 0 &&
          memcmp(its, iak, 16) != 0 && memcmp(its, iak2, 16) != 0 &&
          memcmp(iak, iak2, 16) != 0, "ITS, IAK, IAK2 differ");
    ref_block("IAK2", 1, ref);
    check(memcmp(iak2, ref, 16) == 0, "IAK2 block 1 matches");
    ref_block("ABCDEFGHIJKL", 1, ref);
    check(kdf_derive("ABCDEFGHIJKL", out, 16) == 0 &&
          memcmp(out, ref, 16) == 0, "12-byte label fills bytes 0..11");
}

static void test_refusals(void)
{
    uint8_t out[32];

    g_prf_calls = 0;
    memset(out, 0xA5, sizeof(out));
    check(kdf_derive("ABCDEFGHIJKLM", out, 32) == -1 &&
          all(out, 32, 0) && g_prf_calls == 0,
          "13-byte label: -1, out zeroed, no PRF call");
    memset(out, 0xA5, sizeof(out));
    check(kdf_derive("", out, 32) == -1 && all(out, 32, 0) &&
          kdf_derive(NULL, out, 32) == -1, "empty or NULL label: -1");
    check(kdf_derive("ITS", NULL, 16) == -1 && g_prf_calls == 0,
          "NULL out with out_len 16: -1");

    memset(out, 0xA5, sizeof(out));
    g_prf_calls = 0;
    g_prf_fail_at = 2;
    check(kdf_derive("ITS", out, 32) == -1 && all(out, 32, 0),
          "PRF failure on block 2: -1, whole out zeroed");
    g_prf_fail_at = 0;
}

/*
 * The HUK provisioning KCV is huk_prf(0^128): no label and block
 * counter kdf.c accepts gives the zero block as PRF input.
 */
static void test_zero_block(void)
{
    static const char *const labels[] = {
        KDF_LABEL_ITS, KDF_LABEL_IAK, KDF_LABEL_IAK2, KDF_LABEL_IAK3,
        "A", "AB", "ABCDEFGHIJKL", "\x01", "\x01\x01\x01\x01\x01\x01",
    };
    static uint8_t out[16 * 300];
    int calls = 0;

    g_prf_zero_in = 0;
    for (size_t i = 0; i < sizeof(labels) / sizeof(labels[0]); i++) {
        g_prf_calls = 0;
        kdf_derive(labels[i], out, sizeof(out));
        calls += g_prf_calls;
    }
    g_prf_calls = 0;
    kdf_derive("", out, 16);
    kdf_derive(NULL, out, 16);
    kdf_derive("ABCDEFGHIJKLM", out, 16);
    check(calls == 9 * 300 && g_prf_zero_in == 0 && g_prf_calls == 0,
          "KDF never feeds the zero block (blocks 1..300, 9 labels)");
}

int main(void)
{
    test_zero_block();
    test_stub();
    test_its();
    test_labels();
    test_refusals();
    return check_summary();
}
