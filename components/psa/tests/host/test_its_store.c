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
 * Host test of its_store.c on the RAM backend: blob format, PSA ITS
 * semantics, tamper detection and namespace separation.
 */
#include <string.h>

#include "check.h"
#include "its_ram.h"
#include "its_store.h"
#include "spm_gcm.h"

#define NS ITS_NS_NSPE
#define HDR sizeof(struct its_blob_hdr)

static uint8_t g_key[ITS_KEY_SIZE];
static const uint8_t g_rand[ITS_RAND_SIZE] = { 0x10, 0x32, 0x54, 0x76 };
static uint8_t g_data[ITS_STORE_MAX + 1];
static uint8_t g_out[ITS_STORE_MAX + 16];

static const char *name_of(uint8_t ns, uint64_t uid)
{
    static char name[ITS_NAME_LEN];

    its_store_name(ns, uid, name);
    return name;
}

static uint8_t *file_of(uint8_t ns, uint64_t uid, size_t *len)
{
    return its_ram_data(name_of(ns, uid), len);
}

/* XORs byte at of the file of (ns, uid); 0 when the file is absent */
static int poke(uint8_t ns, uint64_t uid, size_t at, uint8_t x)
{
    size_t len = 0;
    uint8_t *f = file_of(ns, uid, &len);

    if (f == NULL || at >= len) {
        return 0;
    }
    f[at] ^= x;
    return 1;
}

/* Header of the stored file of (ns, uid); zeroed when absent */
static struct its_blob_hdr hdr_of(uint8_t ns, uint64_t uid)
{
    struct its_blob_hdr h = { 0 };
    size_t len = 0;
    const uint8_t *f = file_of(ns, uid, &len);

    if (f != NULL && len >= HDR) {
        memcpy(&h, f, HDR);
    }
    return h;
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

static psa_status_t get(uint8_t ns, uint64_t uid, uint32_t off,
                        uint32_t len, uint32_t *n)
{
    memset(g_out, 0xA5, sizeof(g_out));
    return its_store_get(ns, uid, off, g_out, len, n, NULL);
}

static int reads_back(uint8_t ns, uint64_t uid, const void *d, uint32_t len)
{
    uint32_t n = 0;

    return get(ns, uid, 0, sizeof(g_out), &n) == PSA_SUCCESS &&
           n == len && memcmp(g_out, d, len) == 0;
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

/*
 * 1 when uid (100 bytes, set here) with file byte at XOR x reads as
 * STORAGE_FAILURE with *n 0 and the buffer untouched.
 */
static int tampered(uint64_t uid, size_t at, uint8_t x)
{
    uint32_t n = 7;

    return its_store_set(NS, uid, g_data, 100, 0) == PSA_SUCCESS &&
           reads_back(NS, uid, g_data, 100) && poke(NS, uid, at, x) &&
           get(NS, uid, 0, 100, &n) == PSA_ERROR_STORAGE_FAILURE &&
           n == 0 && all(g_out, sizeof(g_out), 0xA5);
}

static void test_uninit(void)
{
    uint32_t n = 1;

    check(its_store_set(NS, 1, "x", 1, 0) == PSA_ERROR_STORAGE_FAILURE &&
          get(NS, 1, 0, 1, &n) == PSA_ERROR_STORAGE_FAILURE && n == 0,
          "before init: STORAGE_FAILURE");
    check(its_store_init(NULL, g_key, g_rand) ==
          PSA_ERROR_INVALID_ARGUMENT, "init without backend refused");
}

static void test_round_trip(void)
{
    struct its_info info = { 9, 9 };
    struct its_blob_hdr h;
    size_t len = 0;
    uint8_t *f;
    uint32_t n = 0;

    check(its_store_set(NS, 0x1234, g_data, 100, 0) == PSA_SUCCESS &&
          reads_back(NS, 0x1234, g_data, 100), "set / get round trip");
    f = its_ram_data("/its/n_0000000000001234", &len);
    check(f != NULL && len == HDR + 100, "file /its/n_<16 hex>, hdr+100");
    h = hdr_of(NS, 0x1234);
    check(h.magic == ITS_BLOB_MAGIC && h.size == 100 && h.flags == 0 &&
          h.counter == 1, "header: magic, size, flags, counter 1");
    check(f != NULL && !contains(f, len, g_data + 10, 16),
          "no 16-byte plaintext run in the file");
    check(get(NS, 0x1234, 10, 20, &n) == PSA_SUCCESS && n == 20 &&
          memcmp(g_out, g_data + 10, 20) == 0 && all(g_out + 20, 8, 0xA5),
          "get offset 10 len 20");
    check(get(NS, 0x1234, 90, 20, &n) == PSA_SUCCESS && n == 10 &&
          memcmp(g_out, g_data + 90, 10) == 0, "get past the end: 10");
    check(get(NS, 0x1234, 100, 20, &n) == PSA_SUCCESS && n == 0,
          "get at offset == size: 0 bytes");
    n = 5;
    check(get(NS, 0x1234, 101, 20, &n) == PSA_ERROR_INVALID_ARGUMENT &&
          n == 0 && all(g_out, sizeof(g_out), 0xA5),
          "offset > size: INVALID_ARGUMENT, buffer untouched");
    check(its_store_info(NS, 0x1234, &info) == PSA_SUCCESS &&
          info.size == 100 && info.flags == 0, "info: size 100, flags 0");
    check(its_store_get(NS, 0x1234, 0, g_out, 4, &n, &info) ==
          PSA_SUCCESS && n == 4 && info.size == 100, "get fills info");

    check(its_store_set(NS, 0x1234, g_data + 1, 50, 0) == PSA_SUCCESS &&
          reads_back(NS, 0x1234, g_data + 1, 50), "overwrite");
    h = hdr_of(NS, 0x1234);
    check(h.counter == 2 && h.nonce[0] == 2, "overwrite: counter 2");

    check(its_store_set(NS, 0x99, NULL, 0, 0) == PSA_SUCCESS &&
          reads_back(NS, 0x99, "", 0), "empty object");
    check(its_store_remove(NS, 0x1234) == PSA_SUCCESS &&
          its_store_remove(NS, 0x99) == PSA_SUCCESS &&
          get(NS, 0x1234, 0, 1, &n) == PSA_ERROR_DOES_NOT_EXIST &&
          its_store_info(NS, 0x1234, &info) == PSA_ERROR_DOES_NOT_EXIST &&
          its_store_remove(NS, 0x1234) == PSA_ERROR_DOES_NOT_EXIST &&
          its_ram_count() == 1, "remove; then DOES_NOT_EXIST, meta left");
}

/*
 * First set after the first mount (boot_count 1), key 00..0f, rand
 * 10325476: KEY uid 0x0102030405060708, WRITE_ONCE, 16 bytes.
 */
static void test_known_answer(void)
{
    static const uint8_t exp[HDR + 16] = {
        0x31, 0x49, 0x54, 0x53, 0x01, 0x00, 0x00, 0x00, /* magic flags */
        0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, /* size ctr */
        0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, /* nonce */
        0x10, 0x32, 0x54, 0x76,
        0x85, 0x08, 0x53, 0x34, 0xbb, 0xca, 0x29, 0x00, /* tag */
        0x87, 0xc9, 0xf6, 0xa4, 0xe7, 0xb5, 0x4b, 0xab,
        0x55, 0xe0, 0x84, 0x5e, 0x2c, 0xd6, 0x6d, 0xd6, /* ciphertext */
        0x78, 0x71, 0xe8, 0xea, 0x61, 0x45, 0xc2, 0xf3,
    };
    static const uint8_t nonce[12] = {
        1, 0, 0, 0, 1, 0, 0, 0, 0x10, 0x32, 0x54, 0x76,
    };
    static const uint8_t aad[17] = {
        8, 7, 6, 5, 4, 3, 2, 1, 1, 0, 0, 0, 16, 0, 0, 0, ITS_NS_KEY,
    };
    uint8_t ct[16];
    uint8_t tag[16];
    mbedtls_gcm_context gcm;
    size_t len = 0;
    uint8_t *f;

    mbedtls_gcm_init(&gcm);
    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, g_key, 128);
    mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, 16, nonce, 12,
                              aad, sizeof(aad),
                              (const uint8_t *)"ITS known answer", ct, 16,
                              tag);
    mbedtls_gcm_free(&gcm);
    check(memcmp(tag, exp + 28, 16) == 0 && memcmp(ct, exp + HDR, 16) == 0,
          "pinned KAT bytes = mbedtls GCM of the documented layout");

    its_ram_reset();
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_store_set(ITS_NS_KEY, 0x0102030405060708ull,
                        "ITS known answer", 16, ITS_FLAG_WRITE_ONCE) ==
          PSA_SUCCESS, "KAT set");
    f = its_ram_data("/its/k_0102030405060708", &len);
    check(f != NULL && len == sizeof(exp) && memcmp(f, exp, len) == 0,
          "blob = pinned bytes (nonce ctr || boot 1 || rand)");
    its_ram_reset();
}

static void test_write_once(void)
{
    struct its_info info;

    check(its_store_set(NS, 0x77, g_data, 16, ITS_FLAG_WRITE_ONCE) ==
          PSA_SUCCESS && its_store_info(NS, 0x77, &info) == PSA_SUCCESS &&
          info.flags == ITS_FLAG_WRITE_ONCE, "WRITE_ONCE set, info flag");
    check(its_store_set(NS, 0x77, g_data + 1, 16, 0) ==
          PSA_ERROR_NOT_PERMITTED &&
          its_store_remove(NS, 0x77) == PSA_ERROR_NOT_PERMITTED &&
          reads_back(NS, 0x77, g_data, 16),
          "WRITE_ONCE: set and remove NOT_PERMITTED, data intact");
    check(its_store_set(NS, 0x78, g_data, 8, 0x2) == PSA_SUCCESS &&
          its_store_set(NS, 0x79, g_data, 8, 0x8) ==
          PSA_ERROR_NOT_SUPPORTED, "flag 0x2 kept, unknown 0x8 refused");
}

static void test_tamper(void)
{
    char from[ITS_NAME_LEN];
    char to[ITS_NAME_LEN];
    size_t len = 0;

    check(tampered(0x51, HDR + 5, 0x01),
          "flipped ciphertext: STORAGE_FAILURE, buffer untouched");
    check(tampered(0x52, offsetof(struct its_blob_hdr, tag), 0x80),
          "flipped tag: STORAGE_FAILURE");
    check(tampered(0x53, offsetof(struct its_blob_hdr, nonce) + 6, 0x01),
          "flipped nonce: STORAGE_FAILURE");
    check(tampered(0x54, offsetof(struct its_blob_hdr, size), 0x01),
          "header size + 1: STORAGE_FAILURE");
    check(tampered(0x55, 0, 0x01), "bad magic: STORAGE_FAILURE");
    check(tampered(0x56, offsetof(struct its_blob_hdr, counter), 0x01),
          "counter != nonce prefix: STORAGE_FAILURE");
    check(its_store_set(NS, 0x57, g_data, 100, 0) == PSA_SUCCESS &&
          its_ram_set_len(name_of(NS, 0x57), HDR + 101) == 0 &&
          get(NS, 0x57, 0, 100, &(uint32_t){ 0 }) ==
          PSA_ERROR_STORAGE_FAILURE, "trailing byte: STORAGE_FAILURE");
    for (uint64_t uid = 0x51; uid <= 0x57; uid++) {
        poke(NS, uid, 0, 0);
    }
    check(its_store_set(NS, 0x51, g_data, 4, 0) ==
          PSA_ERROR_STORAGE_FAILURE && its_store_remove(NS, 0x51) ==
          PSA_ERROR_STORAGE_FAILURE && file_of(NS, 0x51, &len) != NULL,
          "tampered object: set and remove refused, file kept");

    check(its_store_set(NS, 0x58, g_data, 16, ITS_FLAG_WRITE_ONCE) ==
          PSA_SUCCESS &&
          poke(NS, 0x58, offsetof(struct its_blob_hdr, flags), 0x01) &&
          its_store_remove(NS, 0x58) == PSA_ERROR_STORAGE_FAILURE &&
          its_store_set(NS, 0x58, g_data, 1, 0) ==
          PSA_ERROR_STORAGE_FAILURE && file_of(NS, 0x58, &len) != NULL,
          "WRITE_ONCE bit cleared in file: refused, file kept");

    check(its_store_set(NS, 0x59, g_data, 16, 0) == PSA_SUCCESS &&
          its_ram_set_len(name_of(NS, 0x59), HDR + 15) == 0 &&
          its_store_info(NS, 0x59, NULL) == PSA_ERROR_INVALID_ARGUMENT,
          "info NULL: INVALID_ARGUMENT");
    check(reads_back(NS, 0x59, g_data, 15) == 0 &&
          its_store_get(NS, 0x59, 0, g_out, 16, &(uint32_t){ 0 }, NULL) ==
          PSA_ERROR_STORAGE_FAILURE, "truncated file: STORAGE_FAILURE");

    check(its_store_set(NS, 0x5a, g_data, 16, 0) == PSA_SUCCESS &&
          its_ram_set_len(name_of(NS, 0x5a), 0) == 0 &&
          its_store_info(NS, 0x5a, &(struct its_info){ 0 }) ==
          PSA_ERROR_DOES_NOT_EXIST && its_store_remove(NS, 0x5a) ==
          PSA_ERROR_DOES_NOT_EXIST && file_of(NS, 0x5a, &len) == NULL,
          "empty file: DOES_NOT_EXIST, remove deletes it");
    check(its_store_set(NS, 0x5a, g_data, 16, 0) == PSA_SUCCESS &&
          its_ram_set_len(name_of(NS, 0x5a), 0) == 0 &&
          its_store_set(NS, 0x5a, g_data, 8, 0) == PSA_SUCCESS &&
          reads_back(NS, 0x5a, g_data, 8), "empty file: set replaces it");
    its_ram_reset();

    its_store_name(NS, 0x01, from);
    its_store_name(NS, 0x02, to);
    check(its_store_set(NS, 0x01, g_data, 16, 0) == PSA_SUCCESS &&
          its_ram_rename(from, to) == 0 &&
          its_store_get(NS, 0x02, 0, g_out, 16, &(uint32_t){ 0 }, NULL) ==
          PSA_ERROR_STORAGE_FAILURE, "file renamed to uid 2: uid in AAD");
}

static void test_namespaces(void)
{
    char k[ITS_NAME_LEN];
    char nn[ITS_NAME_LEN];

    its_ram_reset();
    check(its_store_set(ITS_NS_KEY, 7, "key-object", 10, 0) ==
          PSA_SUCCESS && its_store_set(ITS_NS_NSPE, 7, "nspe", 4, 0) ==
          PSA_SUCCESS && its_ram_count() == 2, "same uid in KEY and NSPE");
    check(reads_back(ITS_NS_KEY, 7, "key-object", 10) &&
          reads_back(ITS_NS_NSPE, 7, "nspe", 4), "each reads its own");
    check(its_store_name(ITS_NS_KEY, 7, k) == 0 &&
          its_store_name(ITS_NS_NSPE, 7, nn) == 0 &&
          strcmp(k, "/its/k_0000000000000007") == 0 &&
          strcmp(nn, "/its/n_0000000000000007") == 0, "k_ / n_ names");
    check(its_store_remove(ITS_NS_NSPE, 7) == PSA_SUCCESS &&
          reads_back(ITS_NS_KEY, 7, "key-object", 10) &&
          its_store_remove(ITS_NS_NSPE, 7) == PSA_ERROR_DOES_NOT_EXIST,
          "NSPE remove leaves the KEY object");
    check(its_ram_rename(k, nn) == 0 &&
          its_store_get(ITS_NS_NSPE, 7, 0, g_out, 16, &(uint32_t){ 0 },
                        NULL) == PSA_ERROR_STORAGE_FAILURE,
          "KEY file renamed to n_: namespace in AAD");
    check(its_store_set(0, 1, "x", 1, 0) == PSA_ERROR_INVALID_ARGUMENT &&
          its_store_set(3, 1, "x", 1, 0) == PSA_ERROR_INVALID_ARGUMENT &&
          its_store_name(3, 1, k) == -1, "unknown namespace refused");
    its_ram_reset();
}

static void test_limits(void)
{
    uint32_t n = 0;

    check(its_store_set(NS, 0x2048, g_data, ITS_STORE_MAX, 0) ==
          PSA_SUCCESS && get(NS, 0x2048, 0, ITS_STORE_MAX, &n) ==
          PSA_SUCCESS && n == ITS_STORE_MAX &&
          memcmp(g_out, g_data, ITS_STORE_MAX) == 0, "2048 bytes ok");
    check(its_store_set(NS, 0x2049, g_data, ITS_STORE_MAX + 1, 0) ==
          PSA_ERROR_INVALID_ARGUMENT && file_of(NS, 0x2049, &(size_t){ 0 })
          == NULL, "2049 bytes: INVALID_ARGUMENT, no file");
    check(its_store_set(NS, 0, "x", 1, 0) == PSA_ERROR_INVALID_ARGUMENT &&
          get(NS, 0, 0, 1, &n) == PSA_ERROR_INVALID_ARGUMENT,
          "uid 0: INVALID_ARGUMENT");
    check(its_store_set(NS, 5, NULL, 1, 0) == PSA_ERROR_INVALID_ARGUMENT &&
          its_store_get(NS, 0x2048, 0, NULL, 1, &n, NULL) ==
          PSA_ERROR_INVALID_ARGUMENT, "NULL buffer with len > 0");

    its_ram_fail_write(ITS_BE_EIO);
    check(its_store_set(NS, 0x2048, g_data + 1, 8, 0) ==
          PSA_ERROR_STORAGE_FAILURE &&
          reads_back(NS, 0x2048, g_data, ITS_STORE_MAX),
          "write I/O error: STORAGE_FAILURE, old object kept");
    its_ram_fail_write(ITS_BE_ENOSPC);
    check(its_store_set(NS, 0x2050, g_data, 8, 0) ==
          PSA_ERROR_INSUFFICIENT_STORAGE, "full: INSUFFICIENT_STORAGE");
}

int main(void)
{
    for (size_t i = 0; i < sizeof(g_key); i++) {
        g_key[i] = (uint8_t)i;
    }
    for (size_t i = 0; i < sizeof(g_data); i++) {
        g_data[i] = (uint8_t)(i * 7u + 3u);
    }
    test_uninit();
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS, "init");
    test_round_trip();
    test_known_answer();
    its_store_init(&its_ram, g_key, g_rand);
    test_write_once();
    test_tamper();
    test_namespaces();
    test_limits();
    its_store_deinit();
    check(its_store_info(NS, 0x2048, &(struct its_info){ 0 }) ==
          PSA_ERROR_STORAGE_FAILURE, "after deinit: STORAGE_FAILURE");
    return check_summary();
}
