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

#ifndef OTA_FAKE_STORE_H
#define OTA_FAKE_STORE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <CommonCrypto/CommonDigest.h>

#include "rnm_ota_core.h"

#define FAKE_FLASH_SIZE  0x200000U
#define FAKE_SLOT_ADDR   0x140000U
#define FAKE_SLOT_MAX    0xB8000U

/* NOR model: erase sets 0xFF, write can only clear bits. */
typedef struct {
    uint8_t flash[FAKE_FLASH_SIZE];
    uint32_t slot_max;
    uint32_t erases;
    uint32_t erase_hi;
    uint32_t writes;
    uint32_t bad_writes;
    uint32_t commits;
    uint32_t fail_write;
} fake_store_t;

static inline fake_store_t *fake_store_new(void)
{
    fake_store_t *f = calloc(1, sizeof(*f));

    memset(f->flash, 0xA5, sizeof(f->flash));
    f->slot_max = FAKE_SLOT_MAX;
    return f;
}

static inline int fs_slot_open(void *ctx, uint32_t *addr, uint32_t *max_len)
{
    fake_store_t *f = ctx;

    *addr = FAKE_SLOT_ADDR;
    *max_len = f->slot_max;
    return 0;
}

static inline int fs_erase(void *ctx, uint32_t addr, uint32_t len)
{
    fake_store_t *f = ctx;

    if (addr % 4096U != 0 || len != 4096U || addr + len > FAKE_FLASH_SIZE) {
        return -1;
    }
    memset(f->flash + addr, 0xFF, len);
    f->erases++;
    if (addr + len > f->erase_hi) {
        f->erase_hi = addr + len;
    }
    return 0;
}

static inline int fs_write(void *ctx, uint32_t addr, const uint8_t *data,
                           uint32_t len)
{
    fake_store_t *f = ctx;

    if (++f->writes == f->fail_write || addr + len > FAKE_FLASH_SIZE) {
        return -1;
    }
    for (uint32_t i = 0; i < len; i++) {
        if ((f->flash[addr + i] & data[i]) != data[i]) {
            f->bad_writes++;
        }
        f->flash[addr + i] &= data[i];
    }
    return 0;
}

static inline int fs_read(void *ctx, uint32_t addr, uint8_t *data,
                          uint32_t len)
{
    fake_store_t *f = ctx;

    memcpy(data, f->flash + addr, len);
    return 0;
}

static inline int fs_sha256(void *ctx, uint32_t addr, uint32_t len,
                            uint8_t out[32])
{
    fake_store_t *f = ctx;

    CC_SHA256(f->flash + addr, len, out);
    return 0;
}

static inline int fs_commit(void *ctx)
{
    ((fake_store_t *)ctx)->commits++;
    return 0;
}

static const rnm_ota_store_t fake_store_ops = {
    fs_slot_open, fs_erase, fs_write, fs_read, fs_sha256, fs_commit,
};

/* 512 B header + pseudo-random payload, header fields as iprocube. */
static inline uint8_t *make_pkg(uint32_t payload_len, const char *hw,
                                uint32_t *total)
{
    uint8_t *p = malloc(512U + payload_len);
    uint32_t x = 0x12345678U;

    memset(p, 0xFF, 512);
    memset(p, 0, 64);
    memcpy(p, "IPRO_OTA_Ver1.0", 16);
    memcpy(p + 16, "XZ  ", 4);
    memcpy(p + 20, &payload_len, 4);
    for (int i = 0; i < 8; i++) {
        p[24 + i] = (uint8_t)(i + 1);
    }
    memcpy(p + 32, hw, strlen(hw));
    memcpy(p + 48, "SDK_V1.0.1", 10);
    for (uint32_t i = 0; i < payload_len; i++) {
        x = x * 1103515245U + 12345U;
        p[512 + i] = (uint8_t)(x >> 16);
    }
    CC_SHA256(p + 512, payload_len, p + 64);
    *total = 512U + payload_len;
    return p;
}

#endif /* OTA_FAKE_STORE_H */
