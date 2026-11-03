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
 * Host test of the its_store mount: /its/meta boot count, reformat on
 * a key change or a missing meta with objects, object key binding and
 * failures during mount.
 */
#include <stdlib.h>
#include <string.h>

#include "check.h"
#include "its_ram.h"
#include "its_store.h"
#include "spm_gcm.h"
#include "mbedtls/platform.h"

#define NS ITS_NS_NSPE

static uint8_t g_key[ITS_KEY_SIZE];
static uint8_t g_key2[ITS_KEY_SIZE];
static const uint8_t g_rand[ITS_RAND_SIZE] = { 0xa1, 0xb2, 0xc3, 0xd4 };
static uint8_t g_data[64];
static uint8_t g_out[64];

static const char *name_of(uint8_t ns, uint64_t uid)
{
    static char name[ITS_NAME_LEN];

    its_store_name(ns, uid, name);
    return name;
}

static int reads_back(uint64_t uid, const void *d, uint32_t len)
{
    uint32_t n = 0;

    return its_store_get(NS, uid, 0, g_out, sizeof(g_out), &n, NULL) ==
           PSA_SUCCESS && n == len && memcmp(g_out, d, len) == 0;
}

/* The meta file of boot count b under key, built with mbedtls */
static void meta_make(const uint8_t *key, uint32_t b, uint8_t m[24])
{
    uint8_t nonce[12] = { 0 };
    mbedtls_gcm_context gcm;

    memcpy(m, "ITSM", 4);
    for (size_t i = 0; i < 4; i++) {
        m[4 + i] = (uint8_t)(b >> (8 * i));
        nonce[4 + i] = m[4 + i];
    }
    mbedtls_gcm_init(&gcm);
    mbedtls_gcm_setkey(&gcm, MBEDTLS_CIPHER_ID_AES, key, 128);
    mbedtls_gcm_crypt_and_tag(&gcm, MBEDTLS_GCM_ENCRYPT, 0, nonce, 12, m,
                              8, NULL, NULL, 16, m + 8);
    mbedtls_gcm_free(&gcm);
}

/* 1 when /its/meta is the valid meta file of b under key */
static int meta_is(const uint8_t *key, uint32_t b)
{
    uint8_t m[24];
    size_t len = 0;
    const uint8_t *f = its_ram_data(ITS_META_NAME, &len);

    meta_make(key, b, m);
    return f != NULL && len == sizeof(m) && memcmp(f, m, len) == 0;
}

static void test_boot_count(void)
{
    static const uint8_t tag1[16] = {
        0x57, 0xd8, 0xb2, 0x81, 0xac, 0x30, 0xbf, 0xb1,
        0x96, 0xc0, 0xdf, 0xb9, 0xea, 0x5d, 0xac, 0xfa,
    };
    uint8_t m[24];
    struct its_blob_hdr h;
    size_t len = 0;

    its_ram_reset();
    meta_make(g_key, 1, m);
    check(memcmp(m + 8, tag1, 16) == 0, "meta tag KAT, boot 1, key 00..0f");
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          meta_is(g_key, 1) && its_ram_count() == 1 &&
          its_ram_formats() == 0, "first mount: meta created, boot 1");
    check(its_store_set(NS, 0x10, g_data, 8, 0) == PSA_SUCCESS &&
          its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          meta_is(g_key, 2) && its_store_init(&its_ram, g_key, g_rand) ==
          PSA_SUCCESS && meta_is(g_key, 3) && its_ram_formats() == 0 &&
          reads_back(0x10, g_data, 8), "each mount: boot + 1, objects kept");
    check(its_store_set(NS, 0x11, g_data, 8, 0) == PSA_SUCCESS,
          "set on boot 3");
    memcpy(&h, its_ram_data(name_of(NS, 0x11), &len), sizeof(h));
    check(h.counter == 1 && memcmp(h.nonce, "\1\0\0\0\3\0\0\0", 8) == 0 &&
          memcmp(h.nonce + 8, g_rand, 4) == 0,
          "nonce = counter 1 || boot 3 || boot_rand");
}

static void test_key_change(void)
{
    uint8_t m[24];

    check(its_store_init(&its_ram, g_key2, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key2, 1) &&
          strcmp(its_ram_format_why(), "key changed") == 0,
          "other key: reformat (key changed), meta boot 1");
    check(its_store_get(NS, 0x10, 0, g_out, 8, &(uint32_t){ 0 }, NULL) ==
          PSA_ERROR_DOES_NOT_EXIST && its_store_set(NS, 0x10, g_data, 4,
          0) == PSA_SUCCESS && reads_back(0x10, g_data, 4),
          "other key: old objects gone, new ones work");

    its_ram_data(ITS_META_NAME, &(size_t){ 0 })[10] ^= 1;
    check(its_store_init(&its_ram, g_key2, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 2 && its_ram_count() == 1,
          "meta tag flipped: reformat");
    meta_make(g_key2, 5, m);
    memcpy(m, "ITSX", 4);
    its_ram.write(ITS_META_NAME, m, sizeof(m));
    check(its_store_init(&its_ram, g_key2, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 3, "meta bad magic: reformat");
    its_ram_set_len(ITS_META_NAME, 23);
    check(its_store_init(&its_ram, g_key2, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 4 && meta_is(g_key2, 1),
          "meta 23 bytes: reformat");
    meta_make(g_key2, UINT32_MAX, m);
    its_ram.write(ITS_META_NAME, m, sizeof(m));
    check(its_store_init(&its_ram, g_key2, g_rand) ==
          PSA_ERROR_INSUFFICIENT_STORAGE && its_ram_formats() == 4 &&
          its_store_set(NS, 1, "x", 1, 0) == PSA_ERROR_STORAGE_FAILURE,
          "boot count at UINT32_MAX: refused, store down");
}

static void *no_calloc(size_t n, size_t size)
{
    (void)n;
    (void)size;
    return NULL;
}

static void test_mount_failures(void)
{
    int writes;

    its_ram_reset();
    its_store_init(&its_ram, g_key, g_rand);
    its_store_set(NS, 0x20, g_data, 8, 0);
    its_ram_fail_read(ITS_BE_EIO);
    check(its_store_init(&its_ram, g_key, g_rand) ==
          PSA_ERROR_STORAGE_FAILURE && its_ram_formats() == 0 &&
          its_store_info(NS, 0x20, &(struct its_info){ 0 }) ==
          PSA_ERROR_STORAGE_FAILURE && meta_is(g_key, 1),
          "meta read I/O error: down, no format, files kept");
    its_ram_fail_write(ITS_BE_EIO);
    check(its_store_init(&its_ram, g_key, g_rand) ==
          PSA_ERROR_STORAGE_FAILURE && meta_is(g_key, 1),
          "meta write error: down, meta unchanged");
    its_ram_fail_format(ITS_BE_EIO);
    check(its_store_init(&its_ram, g_key2, g_rand) ==
          PSA_ERROR_STORAGE_FAILURE && its_ram_count() == 2,
          "format error: down, files unchanged");
    writes = its_ram_writes();
    mbedtls_platform_set_calloc_free(no_calloc, free);
    check(its_store_init(&its_ram, g_key, g_rand) ==
          PSA_ERROR_INSUFFICIENT_MEMORY && its_ram_formats() == 1 &&
          its_ram_writes() == writes, "no heap: down, no format, no write");
    mbedtls_platform_set_calloc_free(calloc, free);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          reads_back(0x20, g_data, 8) && meta_is(g_key, 2),
          "next mount: objects intact, boot 2");
}

static void test_no_heap(void)
{
    uint32_t n = 3;

    its_store_set(NS, 0x70, g_data, 32, 0);
    mbedtls_platform_set_calloc_free(no_calloc, free);
    check(its_store_get(NS, 0x70, 0, g_out, 32, &n, NULL) ==
          PSA_ERROR_INSUFFICIENT_MEMORY && n == 0 &&
          its_store_set(NS, 0x70, g_data + 1, 32, 0) ==
          PSA_ERROR_INSUFFICIENT_MEMORY,
          "no heap for GCM: INSUFFICIENT_MEMORY, not STORAGE_FAILURE");
    mbedtls_platform_set_calloc_free(calloc, free);
    check(reads_back(0x70, g_data, 32), "heap back: object intact");
    its_store_remove(NS, 0x70);
}

/* One tampered object stays refused; the others and the mount work */
static void test_one_bad_object(void)
{
    int formats = its_ram_formats();

    its_store_set(NS, 0x31, g_data, 16, 0);
    its_store_set(NS, 0x32, g_data + 1, 16, 0);
    its_ram_data(name_of(NS, 0x31), &(size_t){ 0 })[50] ^= 1;
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == formats, "mount with a bad object: no format");
    check(its_store_get(NS, 0x31, 0, g_out, 16, &(uint32_t){ 0 }, NULL) ==
          PSA_ERROR_STORAGE_FAILURE && its_store_set(NS, 0x31, g_data, 4,
          0) == PSA_ERROR_STORAGE_FAILURE && its_store_remove(NS, 0x31) ==
          PSA_ERROR_STORAGE_FAILURE &&
          its_ram_data(name_of(NS, 0x31), &(size_t){ 0 }) != NULL,
          "bad object: get, set, remove refused, file kept");
    check(reads_back(0x32, g_data + 1, 16) &&
          its_store_set(NS, 0x32, g_data, 8, 0) == PSA_SUCCESS &&
          reads_back(0x32, g_data, 8) &&
          its_store_remove(NS, 0x32) == PSA_SUCCESS,
          "other object: get, set, remove work");
}

/* A blob of key A copied into a store mounted under key B */
static void test_key_binding(void)
{
    static uint8_t blob[ITS_BLOB_MAX];
    size_t len = 0;
    const uint8_t *f;

    check(its_store_set(NS, 0x60, g_data, 64, 0) == PSA_SUCCESS &&
          its_store_init(&its_ram, g_key, g_data) == PSA_SUCCESS &&
          reads_back(0x60, g_data, 64),
          "same key, other boot_rand: readable");
    f = its_ram_data(name_of(NS, 0x60), &len);
    memcpy(blob, f, len);
    check(its_store_init(&its_ram, g_key2, g_rand) == PSA_SUCCESS &&
          its_ram_data(name_of(NS, 0x60), &(size_t){ 0 }) == NULL &&
          its_ram.write(name_of(NS, 0x60), blob, len) == ITS_BE_OK &&
          its_store_get(NS, 0x60, 0, g_out, 64, &(uint32_t){ 0 }, NULL) ==
          PSA_ERROR_STORAGE_FAILURE, "blob of another key: STORAGE_FAILURE");
    its_store_deinit();
}

/* A store holding only the meta file of boot 1 under g_key */
static void store_fresh(void)
{
    its_ram_reset();
    its_store_init(&its_ram, g_key, g_rand);
}

/*
 * Meta absent or empty: boot 1 when /its holds no other file, else a
 * reformat; files outside /its do not count.
 */
static void test_meta_missing(void)
{
    struct its_info info;

    its_ram_reset();
    its_ram.write("/other", "x", 1);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          meta_is(g_key, 1) && its_ram_formats() == 0 &&
          its_ram_data("/other", &(size_t){ 0 }) != NULL,
          "no meta, no object: boot 1, no format, /other kept");

    store_fresh();
    its_store_set(NS, 0x40, g_data, 8, 0);
    its_ram.remove(ITS_META_NAME);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1) &&
          its_store_info(NS, 0x40, &info) == PSA_ERROR_DOES_NOT_EXIST &&
          strcmp(its_ram_format_why(), "meta missing with objects") == 0,
          "no meta, one object: reformat, object gone, boot 1");

    store_fresh();
    its_store_set(ITS_NS_KEY, 0x41, g_data, 8, 0);
    its_ram_set_len(ITS_META_NAME, 0);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1), "empty meta, one key object: reformat");

    store_fresh();
    its_ram.write("/its/x", "x", 1);
    its_ram.remove(ITS_META_NAME);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1), "no meta, another /its file: reformat");

    store_fresh();
    its_ram_set_len(ITS_META_NAME, 0);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 0 && meta_is(g_key, 1),
          "empty meta, no object: boot 1, no format");

    store_fresh();
    its_store_set(NS, 0x42, g_data, 8, 0);
    its_ram.remove(ITS_META_NAME);
    its_ram_fail_count(ITS_BE_EIO);
    check(its_store_init(&its_ram, g_key, g_rand) ==
          PSA_ERROR_STORAGE_FAILURE && its_ram_formats() == 0 &&
          its_ram_count() == 1 && its_store_info(NS, 0x42, &info) ==
          PSA_ERROR_STORAGE_FAILURE, "no meta, count error: down, no format");
    its_store_deinit();
}

/*
 * CORRUPT (store content that is not a readable ITS) from the meta
 * read, the count or the meta write: reformat, boot 1; I/O errors
 * keep the store down (test_mount_failures).
 */
static void test_store_unreadable(void)
{
    store_fresh();
    its_store_set(NS, 0x50, g_data, 8, 0);
    its_ram_fail_read(ITS_BE_CORRUPT);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1) &&
          strcmp(its_ram_format_why(), "store unreadable") == 0 &&
          its_store_set(NS, 0x51, g_data, 4, 0) == PSA_SUCCESS,
          "meta read CORRUPT: reformat (store unreadable), boot 1, works");

    store_fresh();
    its_store_set(NS, 0x52, g_data, 8, 0);
    its_ram.remove(ITS_META_NAME);
    its_ram_fail_count(ITS_BE_CORRUPT);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1) &&
          strcmp(its_ram_format_why(), "store unreadable") == 0,
          "no meta, count CORRUPT: reformat, boot 1");

    store_fresh();
    its_store_set(NS, 0x53, g_data, 8, 0);
    its_ram_fail_write(ITS_BE_CORRUPT);
    check(its_store_init(&its_ram, g_key, g_rand) == PSA_SUCCESS &&
          its_ram_formats() == 1 && its_ram_count() == 1 &&
          meta_is(g_key, 1) &&
          strcmp(its_ram_format_why(), "store unreadable") == 0 &&
          reads_back(0x53, g_data, 8) == 0,
          "meta write CORRUPT: reformat, meta boot 1 written");

    store_fresh();
    its_store_set(NS, 0x54, g_data, 8, 0);
    its_ram_fail_read(ITS_BE_CORRUPT);
    its_ram_fail_format(ITS_BE_EIO);
    check(its_store_init(&its_ram, g_key, g_rand) ==
          PSA_ERROR_STORAGE_FAILURE && its_ram_count() == 2 &&
          its_store_info(NS, 0x54, &(struct its_info){ 0 }) ==
          PSA_ERROR_STORAGE_FAILURE,
          "meta read CORRUPT, format fails: down, files unchanged");
    its_store_deinit();
}

int main(void)
{
    for (size_t i = 0; i < sizeof(g_key); i++) {
        g_key[i] = (uint8_t)i;
        g_key2[i] = (uint8_t)(i ^ 0x80u);
    }
    for (size_t i = 0; i < sizeof(g_data); i++) {
        g_data[i] = (uint8_t)(i * 5u + 1u);
    }
    test_boot_count();
    test_key_change();
    test_mount_failures();
    test_no_heap();
    test_one_bad_object();
    test_key_binding();
    test_meta_missing();
    test_store_unreadable();
    return check_summary();
}
