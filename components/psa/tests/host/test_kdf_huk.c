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
 * kdf.c over huk.c, built per HUK path (Makefile): RAM path on eFuse
 * slot 4, or SEC_ENG hardware key. Both stand-ins hold one test HUK;
 * every label must give the same fixed vectors on both paths.
 */
#include <string.h>

#include "check.h"
#include "drv_ef_ctrl.h"
#include "hal_sec_aes_v2.h"
#include "huk.h"
#include "kdf.h"
#include "spm_aes.h"

/* slot 4 words as read; little-endian bytes = 2b7e1516..09cf4f3c */
static const uint32_t g_huk_words[4] = {
    0x16157e2bu, 0xa6d2ae28u, 0x8815f7abu, 0x3c4fcf09u,
};
static int g_slot_reads;
static int g_hw_runs;

/* openssl aes-128-ecb, key 2b7e151628aed2a6abf7158809cf4f3c */
static const struct {
    const char *label;
    size_t len;
    uint8_t out[32];
} g_vec[] = {
    { KDF_LABEL_ITS, 16, {
        0x2c, 0x6f, 0xe2, 0x5f, 0xca, 0x1a, 0x25, 0x05,
        0x84, 0xdc, 0x2f, 0x32, 0x31, 0xb6, 0x99, 0x8c } },
    { KDF_LABEL_IAK, 32, {
        0x29, 0x86, 0x42, 0x5f, 0xd9, 0x89, 0x5a, 0x11,
        0x50, 0x10, 0x9e, 0x35, 0x48, 0xf6, 0x50, 0xeb,
        0x68, 0xa3, 0xc6, 0x4f, 0x10, 0xf3, 0x52, 0xa3,
        0x42, 0x76, 0x3c, 0xce, 0x0b, 0x18, 0x8a, 0x04 } },
    { KDF_LABEL_IAK2, 32, {
        0x24, 0x52, 0xbf, 0xcd, 0x2f, 0x77, 0xe5, 0x80,
        0xdd, 0xb7, 0x7f, 0x65, 0x01, 0xcb, 0xa9, 0xd6,
        0xb5, 0xd1, 0xde, 0xd2, 0x7c, 0xd1, 0xb4, 0x28,
        0x27, 0x49, 0x1c, 0x0f, 0x42, 0xa1, 0x3e, 0xca } },
    { KDF_LABEL_IAK3, 32, {
        0x91, 0x11, 0x04, 0x1e, 0x72, 0xed, 0xe3, 0xbb,
        0xdf, 0x4f, 0x5a, 0xba, 0x0d, 0x12, 0x57, 0x3f,
        0x17, 0x78, 0x59, 0xda, 0x2b, 0x34, 0xdc, 0xe3,
        0xe2, 0x32, 0x6d, 0x31, 0x90, 0x4c, 0xb3, 0x10 } },
};

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    g_slot_reads++;
    for (uint32_t i = 0; i < len; i++) {
        keyData[i] = index == 4 && i < 4 ? g_huk_words[i] : 0;
    }
}

/* ROM_TZSRG_CTRL with tzc_sboot_done (bits 31:28) 0 */
static uint32_t g_tzc;

uint32_t huk_hw_tzc_read(void)
{
    return g_tzc;
}

void huk_hw_tzc_write(uint32_t v)
{
    g_tzc = v;
}

int hal_sec_aes_v2_setkey_hw(hal_sec_aes_v2_ctx_t *ctx, unsigned int mode,
                             unsigned int dir, unsigned int sel,
                             size_t keylen)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->hw_sel = (uint8_t)sel;
    ctx->hw_keylen = (uint8_t)keylen;
    return mode == HAL_SEC_AES_V2_ECB && dir == HAL_SEC_AES_V2_ENCRYPT &&
           keylen == 16 ? 0 : -1;
}

int hal_sec_aes_v2_set_hw_src(hal_sec_aes_v2_ctx_t *ctx, unsigned int src)
{
    ctx->hw_src = (uint8_t)src;
    return src < 2 ? 0 : -1;
}

/*
 * Software AES under the HW key of KEY_SEL 1 with SE_AES_SBOOT 0:
 * the slot 4 words in little-endian byte order (silicon, part #2).
 */
int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    static const uint8_t zero[16];
    mbedtls_aes_context aes;
    int ok = !ctx->keylen && ctx->hw_sel == 1 && ctx->hw_src == 0 &&
             (g_tzc >> 28) == 0;

    g_hw_runs++;
    if (len != 16) {
        return -1;
    }
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, ok ? (const uint8_t *)g_huk_words : zero,
                           128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, in, out);
    mbedtls_aes_free(&aes);
    return 0;
}

void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

int main(void)
{
    int ok = 1;

    check(huk_init() == 0 && !huk_is_dev(), HUK_PATH ": huk_init, real HUK");
    for (size_t i = 0; i < sizeof(g_vec) / sizeof(g_vec[0]); i++) {
        uint8_t out[32];

        memset(out, 0xA5, sizeof(out));
        ok = ok && kdf_derive(g_vec[i].label, out, g_vec[i].len) == 0 &&
             memcmp(out, g_vec[i].out, g_vec[i].len) == 0;
    }
    check(ok, HUK_PATH ": ITS, IAK, IAK2, IAK3 = the fixed vectors");
#ifdef CONFIG_PSA_HUK_IN_RAM
    check(g_hw_runs == 0 && g_slot_reads > 0,
          HUK_PATH ": slot 4 read, SEC_ENG unused");
#else
    check(g_slot_reads == 0 && g_hw_runs > 0,
          HUK_PATH ": SEC_ENG only, slot 4 never read");
#endif
    {
        uint8_t out[16];

        memset(out, 0xA5, sizeof(out));
        check(huk_lock() >= 0 &&
              kdf_derive(KDF_LABEL_ITS, out, sizeof(out)) == -1 &&
              out[0] == 0 && out[15] == 0,
              HUK_PATH ": after huk_lock kdf_derive -1, out zeroed");
    }
    return check_summary();
}
