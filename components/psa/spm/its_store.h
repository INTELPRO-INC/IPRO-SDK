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
 * ITS object store: AES-128-GCM blobs on an its_backend file store,
 * pure C over the backend ops (host tests use a RAM backend).
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

/* namespaces: mbedtls key files (uid = key id) and ITS RPC uids */
#define ITS_NS_KEY  1u
#define ITS_NS_NSPE 2u

/* PSA_STORAGE_FLAG_* values; set refuses other bits */
#define ITS_FLAG_WRITE_ONCE 0x1u
#define ITS_FLAGS_KNOWN     0x7u

#define ITS_STORE_MAX   2048u
#define ITS_BLOB_MAGIC  0x53544931u

/* its_store_init: AES-128-GCM key, random nonce bytes 8..11 */
#define ITS_KEY_SIZE    16u
#define ITS_RAND_SIZE   4u

/*
 * File name "/its/k_<uid>" (KEY) or "/its/n_<uid>" (NSPE), uid as 16
 * lower-case hex digits, most significant first; 24 bytes with NUL.
 */
#define ITS_NAME_LEN    24u

/*
 * Meta file, 24 bytes: "ITSM" || boot_count (u32 LE) || GCM tag over
 * those 8 bytes as AAD, no plaintext, nonce 0 (u32) || boot_count
 * (u32 LE) || 0 (u32). Object counters start at 1.
 */
#define ITS_META_NAME   "/its/meta"
#define ITS_META_LEN    24u

/*
 * backend return codes; any other non-zero value is an I/O error.
 * CORRUPT: the store holds data that is not a readable ITS file.
 */
#define ITS_BE_OK      0
#define ITS_BE_EIO     (-1)
#define ITS_BE_ENOENT  (-2)
#define ITS_BE_ENOSPC  (-3)
#define ITS_BE_CORRUPT (-4)

/*
 * read: *len is the capacity in, the file size out (larger: EIO).
 * write replaces a file atomically, a failure keeps the old content.
 */
struct its_backend {
    int (*read)(const char *name, void *buf, size_t *len);
    int (*write)(const char *name, const void *buf, size_t len);
    int (*remove)(const char *name);
    /* logs why and leaves an empty store */
    int (*format)(const char *why);
    /* *n = files in /its other than ITS_META_NAME */
    int (*count)(size_t *n);
};

/*
 * File: this header || size bytes of ciphertext (fields LE). nonce =
 * counter || boot_count (u32 LE) || boot_rand; AAD (17 bytes) = uid
 * (u64 LE) || flags || size (u32 LE) || namespace (u8). Tag 16.
 */
struct its_blob_hdr {
    uint32_t magic;         /* ITS_BLOB_MAGIC */
    uint32_t flags;
    uint32_t size;
    uint32_t counter;       /* sets after its_store_init, from 1 */
    uint8_t nonce[12];
    uint8_t tag[16];
};

#define ITS_BLOB_MAX (sizeof(struct its_blob_hdr) + ITS_STORE_MAX)

struct its_info {
    uint32_t size;
    uint32_t flags;
};

/*
 * Mounts under key, rewrites the meta; STORAGE_FAILURE until success.
 * boot_count: meta absent or empty -> 1 (format if /its has objects),
 * tag ok -> + 1; tag failed or a CORRUPT backend call -> format, 1.
 */
psa_status_t its_store_init(const struct its_backend *be,
                            const uint8_t key[ITS_KEY_SIZE],
                            const uint8_t boot_rand[ITS_RAND_SIZE]);

/* Wipes the key; later calls return PSA_ERROR_STORAGE_FAILURE */
void its_store_deinit(void);

/* WRITE_ONCE objects: PSA_ERROR_NOT_PERMITTED */
psa_status_t its_store_set(uint8_t ns, uint64_t uid, const void *data,
                           uint32_t len, uint32_t flags);

/*
 * Copies min(len, size - off) bytes at off into buf (only on
 * PSA_SUCCESS); *n = bytes copied (0 on failure); info may be NULL.
 */
psa_status_t its_store_get(uint8_t ns, uint64_t uid, uint32_t off,
                           void *buf, uint32_t len, uint32_t *n,
                           struct its_info *info);

psa_status_t its_store_info(uint8_t ns, uint64_t uid,
                            struct its_info *info);

/* WRITE_ONCE objects: PSA_ERROR_NOT_PERMITTED */
psa_status_t its_store_remove(uint8_t ns, uint64_t uid);

/* File name of (ns, uid); -1 for an unknown namespace */
int its_store_name(uint8_t ns, uint64_t uid, char name[ITS_NAME_LEN]);
