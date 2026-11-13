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

/* A NOR flash in RAM (a write can only clear bits), SHA-256, and a partition table entry. */
#include <string.h>
#include "hal_flash.h"
#include "hal_boot2.h"
#include "hal_sec_hash.h"
#include "stubs.h"

uint8_t  fake_flash[FAKE_FLASH_SIZE];
unsigned fake_erases;
uint32_t fake_erased_bytes;
int      fake_flash_fail;
PtTable_Entry_Config fake_fw;
int      fake_pt_updates;

void fake_reset(uint8_t fill)
{
    memset(fake_flash, fill, sizeof(fake_flash));
    fake_erases = 0;
    fake_erased_bytes = 0;
    fake_flash_fail = 0;
    fake_pt_updates = 0;
    fake_fw.activeIndex = 0;
    fake_fw.Address[0] = 0x12000;
    fake_fw.maxLen[0] = 0xCE000;
    fake_fw.Address[1] = FAKE_FLASH_BASE;
    fake_fw.maxLen[1] = FAKE_FLASH_SIZE;
}

static int in_range(uint32_t addr, uint32_t len)
{
    return !fake_flash_fail && addr >= FAKE_FLASH_BASE && len <= FAKE_FLASH_SIZE && addr - FAKE_FLASH_BASE <= FAKE_FLASH_SIZE - len;
}

int flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    if (!in_range(addr, len)) return -1;
    memcpy(data, &fake_flash[addr - FAKE_FLASH_BASE], len);
    return SUCCESS;
}

int flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    if (!in_range(addr, len)) return -1;
    for (uint32_t i = 0; i < len; i++) fake_flash[addr - FAKE_FLASH_BASE + i] &= data[i];
    return SUCCESS;
}

int flash_erase(uint32_t startaddr, uint32_t len)
{
    if (!in_range(startaddr, len) || (startaddr & 0xFFFu) || (len & 0xFFFu)) return -1;
    memset(&fake_flash[startaddr - FAKE_FLASH_BASE], 0xFF, len);
    fake_erases++;
    fake_erased_bytes += len;
    return SUCCESS;
}

int hal_boot2_get_active_entries(int type, PtTable_Entry_Config *ptEntry)
{
    if (type != PT_ENTRY_FW_CPU0) return -1;
    *ptEntry = fake_fw;
    return 0;
}

int hal_boot2_update_ptable(PtTable_Entry_Config *ptEntry)
{
    fake_fw.activeIndex = (uint8_t)!ptEntry->activeIndex;
    fake_pt_updates++;
    return 0;
}

/* ---- SHA-256 (FIPS 180-4) ---- */

static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

#define ROR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void sha_block(sec_hash_handle_t *h, const uint8_t *p)
{
    uint32_t w[64], s[8];

    for (int i = 0; i < 16; i++) w[i] = ((uint32_t)p[4 * i] << 24) | ((uint32_t)p[4 * i + 1] << 16) | ((uint32_t)p[4 * i + 2] << 8) | p[4 * i + 3];
    for (int i = 16; i < 64; i++) {
        uint32_t s0 = ROR(w[i - 15], 7) ^ ROR(w[i - 15], 18) ^ (w[i - 15] >> 3);
        uint32_t s1 = ROR(w[i - 2], 17) ^ ROR(w[i - 2], 19) ^ (w[i - 2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    memcpy(s, h->state, sizeof(s));
    for (int i = 0; i < 64; i++) {
        uint32_t t1 = s[7] + (ROR(s[4], 6) ^ ROR(s[4], 11) ^ ROR(s[4], 25)) + ((s[4] & s[5]) ^ (~s[4] & s[6])) + K[i] + w[i];
        uint32_t t2 = (ROR(s[0], 2) ^ ROR(s[0], 13) ^ ROR(s[0], 22)) + ((s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]));
        s[7] = s[6]; s[6] = s[5]; s[5] = s[4]; s[4] = s[3] + t1; s[3] = s[2]; s[2] = s[1]; s[1] = s[0]; s[0] = t1 + t2;
    }
    for (int i = 0; i < 8; i++) h->state[i] += s[i];
}

int sec_hash_init_ex(sec_hash_handle_t *h, sec_hash_mode_t mode, uint8_t type)
{
    static const uint32_t init[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };

    (void)mode; (void)type;
    memcpy(h->state, init, sizeof(init));
    h->bits = 0;
    h->used = 0;
    return 0;
}

int sec_hash_update(sec_hash_handle_t *h, const void *buffer, uint32_t size)
{
    const uint8_t *p = buffer;

    h->bits += (uint64_t)size * 8;
    while (size) {
        uint32_t n = 64 - h->used;
        if (n > size) n = size;
        memcpy(&h->block[h->used], p, n);
        h->used += n; p += n; size -= n;
        if (h->used == 64) { sha_block(h, h->block); h->used = 0; }
    }
    return 0;
}

int sec_hash_finish(sec_hash_handle_t *h, void *buffer)
{
    uint8_t *out = buffer;
    uint64_t bits = h->bits;
    uint8_t  pad = 0x80;

    sec_hash_update(h, &pad, 1);
    pad = 0;
    while (h->used != 56) sec_hash_update(h, &pad, 1);
    for (int i = 7; i >= 0; i--) { uint8_t b = (uint8_t)(bits >> (8 * i)); sec_hash_update(h, &b, 1); }
    for (int i = 0; i < 8; i++) { out[4 * i] = (uint8_t)(h->state[i] >> 24); out[4 * i + 1] = (uint8_t)(h->state[i] >> 16); out[4 * i + 2] = (uint8_t)(h->state[i] >> 8); out[4 * i + 3] = (uint8_t)h->state[i]; }
    return 0;
}

int sec_hash_deinit(sec_hash_handle_t *h) { (void)h; return 0; }

void fake_sha256(const uint8_t *data, uint32_t len, uint8_t out[32])
{
    sec_hash_handle_t h;

    sec_hash_init_ex(&h, SEC_HASH_MODE_DIRECT, SEC_HASH_SHA256);
    sec_hash_update(&h, data, len);
    sec_hash_finish(&h, out);
}
