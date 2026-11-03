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
 * Host test of svc_platform.c: identity init, IAK seed retry and
 * fatal, lifecycle mapping, the IAK as an SPE key, and the platform
 * RPC decoding under the SPM out contract.
 */
#include <string.h>

#include "attest_test.h"
#include "check.h"
#include "psa_ipro/crypto_lib.h"
#include "spm_sha256.h"
#include "psa/crypto.h"
#include "psa_ipro/platform_rpc.h"
#include "svc_platform.h"

#define ECDSA PSA_ALG_ECDSA(PSA_ALG_SHA_256)
#define DET_ECDSA PSA_ALG_DETERMINISTIC_ECDSA(PSA_ALG_SHA_256)
/* mbedtls 2.28 adds SIGN_MESSAGE to a SIGN_HASH usage */
#define SIGN_ONLY (PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_SIGN_MESSAGE)
/* first of the 32 volatile key ids of the PSA core */
#if PSA_IPRO_CRYPTO_LIB >= 3
#define VOLATILE_MIN PSA_KEY_ID_VENDOR_MIN
#else
#define VOLATILE_MIN (PSA_KEY_ID_VENDOR_MAX - 31u)
#endif

static void test_init(void)
{
    uint8_t pub[65];
    uint8_t buf[8];
    uint8_t hash[32] = { 1 };
    uint8_t sig[64];

    check(t_plat(PLATFORM_FN_GET_IAK_PUB, pub, 65) ==
          PSA_ERROR_SERVICE_FAILURE &&
          svc_platform_sign(hash, sig) == PSA_ERROR_SERVICE_FAILURE,
          "before init: no IAK pub, no signature");
    g_kdf_fail = 1;
    check(t_init() == PSA_ERROR_HARDWARE_FAILURE &&
          svc_platform_id() == NULL && strcmp(g_labels, "IAK ") == 0,
          "KDF failure: no identity");
    g_kdf_fail = 0;
    check(t_plat(PLATFORM_FN_GET_UID, buf, 8) == PSA_SUCCESS &&
          buf[0] == 0xC0 && buf[7] == 0xC7,
          "no identity: UID still served (parity mismatch logged)");
    g_bad_seeds = 7;
    check(t_init() == PSA_ERROR_GENERIC_ERROR && g_fatal == 1 &&
          strcmp(g_labels, "IAK IAK2 IAK3 ") == 0,
          "3 seeds out of range: IAK, IAK2, IAK3 tried, fatal");
    g_bad_seeds = 1;
    check(t_init() == PSA_SUCCESS && svc_platform_id() != NULL &&
          strcmp(g_labels, "IAK IAK2 ") == 0,
          "IAK out of range: IAK2 used");
    g_bad_seeds = 0;
}

static void test_identity(void)
{
    static const uint32_t raw_lc[] = { 0, 1, 7, 15, 3 };
    static const uint32_t psa_lc[] = { 0x1000, 0x2000, 0x3000, 0x6000, 0 };
    const struct svc_platform_id *id;
    uint8_t seed[32];
    uint8_t h[32];
    uint32_t lc = 0;
    int ok = 1;

    for (size_t i = 0; i < 5; i++) {
        g_lc_raw = raw_lc[i];
        ok &= t_init() == PSA_SUCCESS &&
              t_plat(PLATFORM_FN_GET_LIFECYCLE, &lc, 4) == PSA_SUCCESS &&
              lc == psa_lc[i];
    }
    check(ok, "lifecycle 0/1/7/15/3 -> 0x1000/2000/3000/6000/0");
    g_dev = 1;
    check(t_init() == PSA_SUCCESS &&
          t_plat(PLATFORM_FN_GET_LIFECYCLE, &lc, 4) == PSA_SUCCESS && lc == 0,
          "development HUK: lifecycle UNKNOWN");
    g_dev = 0;
    memcpy(seed, svc_platform_id()->boot_seed, 32);
    t_init();
    id = svc_platform_id();
    check(memcmp(seed, id->boot_seed, 32) != 0, "boot seed per init");
    mbedtls_sha256_ret(g_spe, sizeof(g_spe), h, 0);
    check(memcmp(h, id->spe_hash, 32) == 0, "SPE hash over the image");
    check(strncmp(id->signer_hex, "1111111122222222", 16) == 0 &&
          strlen(id->signer_hex) == 64, "signer id: PK hash in hex");
}

static void test_iak(void)
{
    const struct svc_platform_id *id = svc_platform_id();
    psa_key_attributes_t a = psa_key_attributes_init();
    psa_key_attributes_t b = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_svc_key_id_t k;
    uint8_t d[32];
    uint8_t pub[65];
    uint8_t want[65];
    uint8_t h[32];
    size_t n = 0;
    int found = 0;

    t_seed_of("IAK", d);
    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_import_key(&a, d, 32, &k);
    psa_export_public_key(k, want, 65, &n);
    psa_destroy_key(k);
    check(t_plat(PLATFORM_FN_GET_IAK_PUB, pub, 65) == PSA_SUCCESS &&
          memcmp(pub, want, 65) == 0 && pub[0] == 0x04,
          "IAK pub: SEC1 point of the IAK seed scalar");
    mbedtls_sha256_ret(pub, 65, h, 0);
    check(id->instance_id[0] == 0x01 &&
          memcmp(id->instance_id + 1, h, 32) == 0,
          "instance id = 0x01 || SHA-256(pub)");
    for (uint32_t i = VOLATILE_MIN; i <= VOLATILE_MIN + 31u; i++) {
        uint8_t p[65];

        if (psa_export_public_key(i, p, 65, &n) == PSA_SUCCESS &&
            memcmp(p, pub, 65) == 0 &&
            psa_get_key_attributes(i, &b) == PSA_SUCCESS) {
            found = i > PSA_KEY_ID_USER_MAX &&
                    psa_get_key_usage_flags(&b) == SIGN_ONLY &&
                    psa_get_key_algorithm(&b) == DET_ECDSA &&
                    PSA_KEY_LIFETIME_IS_VOLATILE(psa_get_key_lifetime(&b)) &&
                    psa_export_key(i, d, 32, &n) == PSA_ERROR_NOT_PERMITTED;
            psa_reset_key_attributes(&b);
        }
    }
    check(found, "IAK: volatile, sign only, outside the user range");
}

/*
 * svc_platform_sign = RFC 6979 ECDSA under the IAK seed scalar: one
 * hash, one signature, equal to a PSA deterministic signature by an
 * independent key; plain ECDSA verify accepts it.
 */
static void test_iak_sign(void)
{
    psa_key_attributes_t a = psa_key_attributes_init();
    mbedtls_svc_key_id_t k = 0;
    uint8_t d[32];
    uint8_t pub[65];
    uint8_t hash[32];
    uint8_t s1[64];
    uint8_t s2[64];
    uint8_t want[64];
    size_t n = 0;

    for (size_t i = 0; i < sizeof(hash); i++) {
        hash[i] = (uint8_t)(0x30 + i);
    }
    check(svc_platform_sign(hash, s1) == PSA_SUCCESS &&
          svc_platform_sign(hash, s2) == PSA_SUCCESS &&
          memcmp(s1, s2, 64) == 0, "IAK sign: same hash, same signature");
    t_seed_of("IAK", d);
    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&a, DET_ECDSA);
    check(psa_import_key(&a, d, 32, &k) == PSA_SUCCESS &&
          psa_sign_hash(k, DET_ECDSA, hash, 32, want, 64, &n) ==
          PSA_SUCCESS && n == 64 && memcmp(s1, want, 64) == 0,
          "IAK sign = RFC 6979 signature of the seed scalar");
    psa_destroy_key(k);
    psa_reset_key_attributes(&a);
    t_plat(PLATFORM_FN_GET_IAK_PUB, pub, 65);
    psa_set_key_type(&a, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_usage_flags(&a, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&a, ECDSA);
    k = 0;
    check(psa_import_key(&a, pub, 65, &k) == PSA_SUCCESS &&
          psa_verify_hash(k, ECDSA, hash, 32, s1, 64) == PSA_SUCCESS,
          "IAK signature verifies as plain ECDSA with the IAK pub");
    hash[0] ^= 1;
    check(psa_verify_hash(k, ECDSA, hash, 32, s1, 64) ==
          PSA_ERROR_INVALID_SIGNATURE, "other hash: invalid signature");
    psa_destroy_key(k);
}

static void test_platform_rpc(void)
{
    static const size_t sz[] = { 0, 8, 4, 65, 32, 64 };
    uint8_t buf[80];
    int ok = 1;

    for (int32_t t = 1; t <= 5; t++) {
        ok &= t_plat(t, buf, sz[t] - 1) == PSA_ERROR_INVALID_ARGUMENT &&
              t_plat(t, buf, sz[t] + 1) == PSA_ERROR_INVALID_ARGUMENT;
    }
    check(ok, "every fn: out size +-1 INVALID_ARGUMENT");
    check(t_plat(0, buf, 8) == PSA_ERROR_NOT_SUPPORTED &&
          t_plat(6, buf, 8) == PSA_ERROR_NOT_SUPPORTED &&
          t_plat(-1, buf, 8) == PSA_ERROR_NOT_SUPPORTED,
          "ids 0, 6, -1: NOT_SUPPORTED");
    {
        struct iv in[1] = { { buf, 4 } };
        struct ov out[2] = { { buf, 8 }, { buf, 8 } };

        check(t_raw(svc_platform_call, PLATFORM_FN_GET_UID, in, 1, out, 1) ==
              PSA_ERROR_INVALID_ARGUMENT &&
              t_raw(svc_platform_call, PLATFORM_FN_GET_UID, NULL, 0, out, 2) ==
              PSA_ERROR_INVALID_ARGUMENT &&
              t_raw(svc_platform_call, PLATFORM_FN_GET_UID, NULL, 0, out, 0) ==
              PSA_ERROR_INVALID_ARGUMENT, "an in vector, 0 or 2 out: refused");
    }
    memset(spm_nspe_measurement, 0x5A, 32);
    check(t_plat(PLATFORM_FN_GET_NSPE_MEASUREMENT, buf, 32) == PSA_SUCCESS &&
          memcmp(buf, spm_nspe_measurement, 32) == 0, "NSPE measurement");
    check(t_plat(PLATFORM_FN_GET_STATS, buf, 64) == PSA_ERROR_BAD_STATE,
          "stats without a bound control block: BAD_STATE");
    g_bound = 1;
    g_ctrl.stat_traps = 11;
    g_ctrl.stat_irq = 33;
    check(t_plat(PLATFORM_FN_GET_STATS, buf, 64) == PSA_SUCCESS &&
          memcmp(buf, (const void *)&g_ctrl, 64) == 0,
          "stats: copy of the control block");
}

int main(void)
{
    for (size_t i = 0; i < sizeof(g_spe); i++) {
        g_spe[i] = (uint8_t)(i * 13u);
    }
    check(psa_crypto_init() == PSA_SUCCESS, "host PSA init");
    test_init();
    test_identity();
    test_iak();
    test_iak_sign();
    test_platform_rpc();
    check(g_contract_bad == 0, "out contract held on every call");
    return check_summary();
}
