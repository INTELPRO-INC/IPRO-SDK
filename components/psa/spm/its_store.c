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
 * ITS objects as AES-128-GCM blobs on an its_backend file store. One
 * static blob buffer holds a file; plaintext exists only in it or in
 * the caller's buffers and the buffer is wiped after every call.
 */
#include <string.h>

#include "its_store.h"
#include "spm_gcm.h"
#include "mbedtls/platform_util.h"

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "its_blob_hdr is stored in host byte order: little-endian only"
#endif

#define ITS_HDR       sizeof(struct its_blob_hdr)
#define ITS_AAD_LEN   17u
#define ITS_TAG_LEN   16u
#define ITS_NONCE_LEN 12u

_Static_assert(sizeof(struct its_blob_hdr) == 44, "its_blob_hdr");

/* file name tag of each namespace; 0 = unknown namespace */
static const char its_ns_tag[] = {
    [ITS_NS_KEY] = 'k',
    [ITS_NS_NSPE] = 'n',
};

static const uint8_t its_meta_magic[4] = { 'I', 'T', 'S', 'M' };

static const struct its_backend *its_be;
static uint8_t its_key[ITS_KEY_SIZE];
/* nonce bytes 4..11: boot_count (u32 LE) || boot_rand */
static uint8_t its_nonce_hi[8];
static uint32_t its_ctr;
static mbedtls_gcm_context its_gcm_ctx;
static uint8_t its_blob[ITS_BLOB_MAX] __attribute__((aligned(8)));

static int its_ns_ok(uint8_t ns)
{
    return ns < sizeof(its_ns_tag) && its_ns_tag[ns] != 0;
}

static void its_put_le32(uint8_t *p, uint32_t v)
{
    for (size_t i = 0; i < 4; i++) {
        p[i] = (uint8_t)(v >> (8 * i));
    }
}

static uint32_t its_get_le32(const uint8_t *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 |
           (uint32_t)p[3] << 24;
}

static void its_wipe_blob(void)
{
    mbedtls_platform_zeroize(its_blob, sizeof(its_blob));
}

int its_store_name(uint8_t ns, uint64_t uid, char name[ITS_NAME_LEN])
{
    static const char hex[] = "0123456789abcdef";

    if (!its_ns_ok(ns)) {
        return -1;
    }
    memcpy(name, "/its/?_", 7);
    name[5] = its_ns_tag[ns];
    for (size_t i = 0; i < 16; i++) {
        name[7 + i] = hex[(uid >> (60 - 4 * i)) & 0xFu];
    }
    name[ITS_NAME_LEN - 1] = '\0';
    return 0;
}

/* Argument checks shared by every call */
static psa_status_t its_check(uint8_t ns, uint64_t uid)
{
    if (its_be == NULL) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    return its_ns_ok(ns) && uid != 0 ? PSA_SUCCESS
                                     : PSA_ERROR_INVALID_ARGUMENT;
}

/*
 * GCM of len bytes in to out under its_key. Encrypt writes tag,
 * decrypt checks it (STORAGE_FAILURE). No heap: INSUFFICIENT_MEMORY.
 */
static psa_status_t its_gcm(int mode, const uint8_t nonce[ITS_NONCE_LEN],
                            const uint8_t *aad, size_t aad_len,
                            size_t len, const uint8_t *in, uint8_t *out,
                            uint8_t tag[ITS_TAG_LEN])
{
    psa_status_t st = PSA_ERROR_INSUFFICIENT_MEMORY;
    int ret;

    mbedtls_gcm_init(&its_gcm_ctx);
    ret = mbedtls_gcm_setkey(&its_gcm_ctx, MBEDTLS_CIPHER_ID_AES, its_key, 128);
    if (ret == 0 && mode == MBEDTLS_GCM_ENCRYPT) {
        ret = mbedtls_gcm_crypt_and_tag(&its_gcm_ctx, MBEDTLS_GCM_ENCRYPT, len,
                                        nonce, ITS_NONCE_LEN, aad, aad_len,
                                        in, out, ITS_TAG_LEN, tag);
        st = ret == 0 ? PSA_SUCCESS : PSA_ERROR_GENERIC_ERROR;
    } else if (ret == 0) {
        ret = mbedtls_gcm_auth_decrypt(&its_gcm_ctx, len, nonce, ITS_NONCE_LEN,
                                       aad, aad_len, tag, ITS_TAG_LEN, in,
                                       out);
        st = ret == 0 ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
    }
    mbedtls_gcm_free(&its_gcm_ctx);
    return st;
}

/* Object GCM: AAD of (ns, uid, h), nonce and tag of h */
static psa_status_t its_gcm_obj(int mode, uint8_t ns, uint64_t uid,
                                struct its_blob_hdr *h, const uint8_t *in,
                                uint8_t *out)
{
    uint8_t aad[ITS_AAD_LEN];

    its_put_le32(aad, (uint32_t)uid);
    its_put_le32(aad + 4, (uint32_t)(uid >> 32));
    its_put_le32(aad + 8, h->flags);
    its_put_le32(aad + 12, h->size);
    aad[16] = ns;
    return its_gcm(mode, h->nonce, aad, sizeof(aad), h->size, in, out,
                   h->tag);
}

/*
 * Reads and authenticates the object into its_blob: header in *h,
 * plaintext at its_blob[0..h->size). An empty file is no object.
 */
static psa_status_t its_load(uint8_t ns, uint64_t uid,
                             struct its_blob_hdr *h)
{
    char name[ITS_NAME_LEN];
    size_t n = sizeof(its_blob);
    int rc;

    its_store_name(ns, uid, name);
    rc = its_be->read(name, its_blob, &n);
    if (rc == ITS_BE_ENOENT || (rc == ITS_BE_OK && n == 0)) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    if (rc != ITS_BE_OK || n < ITS_HDR) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    memcpy(h, its_blob, ITS_HDR);
    if (h->magic != ITS_BLOB_MAGIC || h->size != n - ITS_HDR ||
        (h->flags & ~ITS_FLAGS_KNOWN) != 0 ||
        h->counter != its_get_le32(h->nonce)) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    /* plaintext to its_blob[0], ITS_HDR bytes before the input */
    return its_gcm_obj(MBEDTLS_GCM_DECRYPT, ns, uid, h, its_blob + ITS_HDR,
                       its_blob);
}

/* its_load status; NOT_PERMITTED for a WRITE_ONCE object */
static psa_status_t its_writable(uint8_t ns, uint64_t uid)
{
    struct its_blob_hdr h;
    psa_status_t st = its_load(ns, uid, &h);

    its_wipe_blob();
    if (st == PSA_SUCCESS && (h.flags & ITS_FLAG_WRITE_ONCE) != 0) {
        return PSA_ERROR_NOT_PERMITTED;
    }
    return st;
}

/* Meta of boot count b in m; the tag m[8..23] written or checked */
static psa_status_t its_meta_tag(int mode, uint8_t m[ITS_META_LEN],
                                 uint32_t b)
{
    uint8_t nonce[ITS_NONCE_LEN] = { 0 };

    memcpy(m, its_meta_magic, sizeof(its_meta_magic));
    its_put_le32(m + 4, b);
    its_put_le32(nonce + 4, b);
    return its_gcm(mode, nonce, m, 8, 0, m, m + 8, m + 8);
}

/* Formats the store (why is logged); boot count 1 on success */
static psa_status_t its_reformat(const struct its_backend *be,
                                 const char *why, uint32_t *boot)
{
    *boot = 1;
    return be->format(why) == ITS_BE_OK ? PSA_SUCCESS
                                        : PSA_ERROR_STORAGE_FAILURE;
}

/*
 * Boot count of this mount: the meta count + 1, or 1 for no meta.
 * Formats when the meta fails its check, when it is missing and /its
 * holds other files, or when the read or count returns CORRUPT.
 */
static psa_status_t its_meta_mount(const struct its_backend *be,
                                   uint32_t *boot)
{
    size_t n = sizeof(its_blob);
    size_t files = 0;
    int rc = be->read(ITS_META_NAME, its_blob, &n);
    uint32_t b = its_get_le32(its_blob + 4);
    const char *why = "key changed";
    psa_status_t st = PSA_ERROR_STORAGE_FAILURE;

    if (rc == ITS_BE_ENOENT || (rc == ITS_BE_OK && n == 0)) {
        rc = be->count(&files);
        b = 0;
        st = files == 0 ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
        why = "meta missing with objects";
    } else if (rc == ITS_BE_OK && n == ITS_META_LEN &&
               memcmp(its_blob, its_meta_magic,
                      sizeof(its_meta_magic)) == 0) {
        st = its_meta_tag(MBEDTLS_GCM_DECRYPT, its_blob, b);
    }
    if (rc == ITS_BE_CORRUPT) {
        st = PSA_ERROR_STORAGE_FAILURE;
        why = "store unreadable";
    } else if (rc != ITS_BE_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }
    if (st == PSA_SUCCESS) {
        *boot = b + 1u;
        return b == UINT32_MAX ? PSA_ERROR_INSUFFICIENT_STORAGE : st;
    }
    if (st != PSA_ERROR_STORAGE_FAILURE) {
        return st;
    }
    return its_reformat(be, why, boot);
}

/* Writes the meta of *boot; on CORRUPT formats and writes boot 1 */
static psa_status_t its_meta_write(const struct its_backend *be,
                                   uint32_t *boot)
{
    uint8_t m[ITS_META_LEN];
    psa_status_t st = its_meta_tag(MBEDTLS_GCM_ENCRYPT, m, *boot);
    int rc = ITS_BE_OK;

    if (st == PSA_SUCCESS) {
        rc = be->write(ITS_META_NAME, m, sizeof(m));
    }
    if (rc == ITS_BE_CORRUPT) {
        st = its_reformat(be, "store unreadable", boot);
        if (st == PSA_SUCCESS) {
            st = its_meta_tag(MBEDTLS_GCM_ENCRYPT, m, *boot);
        }
        if (st == PSA_SUCCESS) {
            rc = be->write(ITS_META_NAME, m, sizeof(m));
        }
    }
    if (st == PSA_SUCCESS && rc != ITS_BE_OK) {
        st = PSA_ERROR_STORAGE_FAILURE;
    }
    return st;
}

psa_status_t its_store_init(const struct its_backend *be,
                            const uint8_t key[ITS_KEY_SIZE],
                            const uint8_t boot_rand[ITS_RAND_SIZE])
{
    uint32_t boot = 0;
    psa_status_t st;

    its_store_deinit();
    if (be == NULL || be->read == NULL || be->write == NULL ||
        be->remove == NULL || be->format == NULL || be->count == NULL ||
        key == NULL || boot_rand == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    memcpy(its_key, key, sizeof(its_key));
    st = its_meta_mount(be, &boot);
    its_wipe_blob();
    if (st == PSA_SUCCESS) {
        st = its_meta_write(be, &boot);
    }
    if (st != PSA_SUCCESS) {
        its_store_deinit();
        return st;
    }
    its_put_le32(its_nonce_hi, boot);
    memcpy(its_nonce_hi + 4, boot_rand, ITS_RAND_SIZE);
    its_be = be;
    return PSA_SUCCESS;
}

void its_store_deinit(void)
{
    its_be = NULL;
    its_ctr = 0;
    mbedtls_platform_zeroize(its_key, sizeof(its_key));
    mbedtls_platform_zeroize(its_nonce_hi, sizeof(its_nonce_hi));
    its_wipe_blob();
}

psa_status_t its_store_set(uint8_t ns, uint64_t uid, const void *data,
                           uint32_t len, uint32_t flags)
{
    struct its_blob_hdr h = { ITS_BLOB_MAGIC, flags, len, 0, { 0 }, { 0 } };
    char name[ITS_NAME_LEN];
    psa_status_t st = its_check(ns, uid);
    int rc;

    if (st == PSA_SUCCESS && (len > ITS_STORE_MAX ||
                              (data == NULL && len != 0))) {
        st = PSA_ERROR_INVALID_ARGUMENT;
    }
    if (st == PSA_SUCCESS && (flags & ~ITS_FLAGS_KNOWN) != 0) {
        st = PSA_ERROR_NOT_SUPPORTED;
    }
    if (st == PSA_SUCCESS && its_ctr == UINT32_MAX) {
        st = PSA_ERROR_INSUFFICIENT_STORAGE;
    }
    if (st == PSA_SUCCESS) {
        st = its_writable(ns, uid);
    }
    if (st == PSA_ERROR_DOES_NOT_EXIST) {
        st = PSA_SUCCESS;
    }
    if (st != PSA_SUCCESS) {
        return st;
    }
    h.counter = ++its_ctr;
    its_put_le32(h.nonce, h.counter);
    memcpy(h.nonce + 4, its_nonce_hi, sizeof(its_nonce_hi));
    st = its_gcm_obj(MBEDTLS_GCM_ENCRYPT, ns, uid, &h,
                     len != 0 ? data : its_blob + ITS_HDR,
                     its_blob + ITS_HDR);
    if (st != PSA_SUCCESS) {
        its_wipe_blob();
        return st;
    }
    memcpy(its_blob, &h, ITS_HDR);
    its_store_name(ns, uid, name);
    rc = its_be->write(name, its_blob, ITS_HDR + len);
    its_wipe_blob();
    if (rc == ITS_BE_ENOSPC) {
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    }
    return rc == ITS_BE_OK ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
}

psa_status_t its_store_get(uint8_t ns, uint64_t uid, uint32_t off,
                           void *buf, uint32_t len, uint32_t *n,
                           struct its_info *info)
{
    struct its_blob_hdr h;
    psa_status_t st = its_check(ns, uid);

    *n = 0;
    if (st == PSA_SUCCESS && buf == NULL && len != 0) {
        st = PSA_ERROR_INVALID_ARGUMENT;
    }
    if (st == PSA_SUCCESS) {
        st = its_load(ns, uid, &h);
    }
    if (st == PSA_SUCCESS && off > h.size) {
        st = PSA_ERROR_INVALID_ARGUMENT;
    }
    if (st == PSA_SUCCESS) {
        *n = h.size - off < len ? h.size - off : len;
        if (*n != 0) {
            memcpy(buf, its_blob + off, *n);
        }
        if (info != NULL) {
            info->size = h.size;
            info->flags = h.flags;
        }
    }
    its_wipe_blob();
    return st;
}

psa_status_t its_store_info(uint8_t ns, uint64_t uid,
                            struct its_info *info)
{
    struct its_blob_hdr h;
    psa_status_t st = its_check(ns, uid);

    if (st == PSA_SUCCESS && info == NULL) {
        st = PSA_ERROR_INVALID_ARGUMENT;
    }
    if (st == PSA_SUCCESS) {
        st = its_load(ns, uid, &h);
    }
    if (st == PSA_SUCCESS) {
        info->size = h.size;
        info->flags = h.flags;
    }
    its_wipe_blob();
    return st;
}

/* An absent object: DOES_NOT_EXIST; an empty file is deleted too */
psa_status_t its_store_remove(uint8_t ns, uint64_t uid)
{
    char name[ITS_NAME_LEN];
    psa_status_t st = its_check(ns, uid);
    int rc;

    if (st == PSA_SUCCESS) {
        st = its_writable(ns, uid);
    }
    if (st != PSA_SUCCESS && st != PSA_ERROR_DOES_NOT_EXIST) {
        return st;
    }
    its_store_name(ns, uid, name);
    rc = its_be->remove(name);
    if (st != PSA_SUCCESS || rc == ITS_BE_ENOENT) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }
    return rc == ITS_BE_OK ? PSA_SUCCESS : PSA_ERROR_STORAGE_FAILURE;
}
