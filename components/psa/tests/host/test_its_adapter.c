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
 * Host test of its_mbedtls_adapter.c and of mbedtls persistent keys
 * stored through it (the ABI psa_crypto_storage.c calls).
 */
#include <string.h>

#include "check.h"
#include "its_ram.h"
#include "its_store.h"
#include "hw_poll.h"
#include "psa/crypto.h"
#include "psa/internal_trusted_storage.h"

_Static_assert(sizeof(struct psa_storage_info_t) == 12,
               "psa_storage_info_t {capacity, size, flags}");

static const uint8_t g_aes_key[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};
static uint64_t g_rng = 0x9E3779B97F4A7C15ull;

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

static int contains(const uint8_t *p, size_t n, const uint8_t *q, size_t m)
{
    for (size_t i = 0; i + m <= n; i++) {
        if (memcmp(p + i, q, m) == 0) {
            return 1;
        }
    }
    return 0;
}

static void test_adapter(void)
{
    struct psa_storage_info_t info = { 9, 9, 9 };
    uint8_t out[16];
    size_t n = 99;
    size_t len = 0;

#if defined(MBEDTLS_PSA_ITS_FILE_C)
    check(0, "SPE config without MBEDTLS_PSA_ITS_FILE_C");
#else
    check(1, "SPE config without MBEDTLS_PSA_ITS_FILE_C");
#endif
    check(sizeof(struct psa_storage_info_t) == 12,
          "psa_storage_info_t: 12 bytes");
    check(psa_its_get_info(0x10, &info) == PSA_ERROR_DOES_NOT_EXIST,
          "missing uid: DOES_NOT_EXIST");
    check(psa_its_set(0x10, 5, "hello", 0) == PSA_SUCCESS &&
          its_ram_data("/its/k_0000000000000010", &len) != NULL,
          "psa_its_set writes the KEY namespace file k_");
    check(psa_its_get_info(0x10, &info) == PSA_SUCCESS && info.size == 5 &&
          info.capacity == 5 && info.flags == 0,
          "get_info: capacity 5, size 5, flags 0");
    check(psa_its_get(0x10, 1, 3, out, &n) == PSA_SUCCESS && n == 3 &&
          memcmp(out, "ell", 3) == 0, "get offset 1 len 3");
    n = 99;
    check(psa_its_get(0x10, 6, 1, out, &n) == PSA_ERROR_INVALID_ARGUMENT &&
          n == 0, "offset > size: INVALID_ARGUMENT, length 0");
    check(its_store_set(ITS_NS_NSPE, 0x11, "nspe", 4, 0) == PSA_SUCCESS &&
          psa_its_get_info(0x11, &info) == PSA_ERROR_DOES_NOT_EXIST &&
          psa_its_remove(0x11) == PSA_ERROR_DOES_NOT_EXIST,
          "NSPE object invisible to the adapter");
    check(psa_its_set(0x12, 3, "abc", PSA_STORAGE_FLAG_WRITE_ONCE) ==
          PSA_SUCCESS && psa_its_remove(0x12) == PSA_ERROR_NOT_PERMITTED &&
          psa_its_get_info(0x12, &info) == PSA_SUCCESS &&
          info.flags == PSA_STORAGE_FLAG_WRITE_ONCE, "WRITE_ONCE kept");
    check(psa_its_remove(0x10) == PSA_SUCCESS &&
          psa_its_get_info(0x10, &info) == PSA_ERROR_DOES_NOT_EXIST,
          "remove");
    check(psa_its_get_support() == 0, "psa_its_get_support: 0");
    its_ram_reset();
}

static void key_attr(psa_key_attributes_t *a, mbedtls_svc_key_id_t id)
{
    *a = psa_key_attributes_init();
    psa_set_key_id(a, id);
    psa_set_key_type(a, PSA_KEY_TYPE_AES);
    psa_set_key_bits(a, 128);
    psa_set_key_usage_flags(a, PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(a, PSA_ALG_ECB_NO_PADDING);
}

static void test_persistent_key(void)
{
    const char *name = "/its/k_0000000000000020";
    psa_key_attributes_t a;
    mbedtls_svc_key_id_t got = 0;
    uint8_t out[16];
    size_t n = 0;
    size_t len = 0;
    const uint8_t *f;

    key_attr(&a, 0x20);
    check(psa_import_key(&a, g_aes_key, 16, &got) == PSA_SUCCESS &&
          got == 0x20, "persistent AES key 0x20 imported");
    f = its_ram_data(name, &len);
    check(f != NULL && !contains(f, len, g_aes_key, 16),
          "key file k_..20 holds no raw key bytes");
    check(psa_purge_key(0x20) == PSA_SUCCESS &&
          psa_export_key(0x20, out, 16, &n) == PSA_SUCCESS && n == 16 &&
          memcmp(out, g_aes_key, 16) == 0, "purged key reloads from ITS");
    check(psa_import_key(&a, g_aes_key, 16, &got) ==
          PSA_ERROR_ALREADY_EXISTS, "same id again: ALREADY_EXISTS");
    check(psa_destroy_key(0x20) == PSA_SUCCESS &&
          its_ram_data(name, &len) == NULL, "destroy removes the file");

    its_store_deinit();
    key_attr(&a, 0x21);
    check(psa_import_key(&a, g_aes_key, 16, &got) != PSA_SUCCESS &&
          its_ram_count() == 0, "ITS down: persistent import fails");
}

int main(void)
{
    uint8_t key[ITS_KEY_SIZE] = { 1, 2, 3 };
    uint8_t rnd[ITS_RAND_SIZE] = { 4, 5, 6 };

    check(its_store_init(&its_ram, key, rnd) == PSA_SUCCESS &&
          psa_crypto_init() == PSA_SUCCESS, "its_store and PSA init");
    test_adapter();
    test_persistent_key();
    mbedtls_psa_crypto_free();
    return check_summary();
}
