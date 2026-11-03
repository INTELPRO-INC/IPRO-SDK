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
 * eFuse, SEC_ENG hardware-key and TZC fakes of test_huk_prov: OR-only
 * program from the shadow, write locks refuse their slot, read locks
 * hide their slot from the shadow; KEY_SEL 1 per g_ef.engine.
 */
#include <string.h>

#include "drv_ef_ctrl.h"
#include "fake_hukp.h"
#include "hal_sec_aes_v2.h"
#include "huk_prov.h"
#include "spm_aes.h"

#define W_LOCK          31u
#define W_SLOT(n)       (7u + 4u * (n))
#define LOCK_WR(n)      (1u << (19u + (n)))
#define LOCK_RD(n)      (1u << (26u + (n)))
#define SBOOT_MSK       (0xFu << 28)

struct fake_efuse g_ef;

/* key slot of word i, or -1 */
static int slot_of(uint32_t i)
{
    return i >= W_SLOT(0) && i < W_SLOT(6) ? (int)((i - W_SLOT(0)) / 4u)
                                           : -1;
}

/* counts eFuse controller use after the busy program */
static void after_busy(void)
{
    if (g_ef.busy_at != 0 && g_ef.programs >= g_ef.busy_at) {
        g_ef.ctrl_after_busy++;
    }
}

void hukp_ef_load(void)
{
    after_busy();
    uint32_t rd = g_ef.rdlock_at_reset ? g_ef.lock_boot : g_ef.fuse[W_LOCK];

    g_ef.loads++;
    for (uint32_t i = 0; i < FAKE_EF_WORDS; i++) {
        int s = slot_of(i);

        g_ef.shadow[i] = s >= 0 && (rd & LOCK_RD(s)) != 0 ? 0 : g_ef.fuse[i];
    }
}

int hukp_ef_ready(void)
{
    return !g_ef.not_ready;
}

uint32_t hukp_ef_get(uint32_t word)
{
    return g_ef.shadow[word];
}

void hukp_ef_set(uint32_t word, uint32_t v)
{
    after_busy();
    g_ef.shadow[word] = v;
}

/* the program lands; with busy_at the controller never goes idle */
int hukp_ef_program(void)
{
    uint32_t wl = g_ef.fuse[W_LOCK];

    after_busy();
    g_ef.programs++;
    for (uint32_t i = 0; i < FAKE_EF_WORDS; i++) {
        int s = slot_of(i);

        if (s < 0 || (wl & LOCK_WR(s)) == 0) {
            g_ef.fuse[i] |= (g_ef.shadow[i] | g_ef.add[i]) & ~g_ef.drop[i];
        }
    }
    return g_ef.programs == g_ef.busy_at ? -1 : 0;
}

void fake_power_on(void)
{
    g_ef.lock_boot = g_ef.fuse[W_LOCK];
    memcpy(g_ef.key_boot, &g_ef.fuse[W_SLOT(4)], sizeof(g_ef.key_boot));
    hukp_ef_load();
    g_ef.loads = 0;
}

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    hukp_ef_load();
    memcpy(keyData, &g_ef.shadow[W_SLOT(index)], len * 4u);
}

uint32_t huk_hw_tzc_read(void)
{
    return g_ef.tzc;
}

void huk_hw_tzc_write(uint32_t v)
{
    g_ef.tzc = g_ef.tzc_stuck ? (v & ~SBOOT_MSK) | (g_ef.tzc & SBOOT_MSK) : v;
}

void hukp_tzc_close(void)
{
    huk_hw_tzc_write((g_ef.tzc & ~SBOOT_MSK) | SBOOT_MSK);
}

uint32_t hukp_tzc_sboot_done(void)
{
    return (g_ef.tzc & SBOOT_MSK) >> 28;
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

/* KEY_SEL 1 with SBOOT key select 0 and tzc_sboot_done 0, else zero */
int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    static const uint32_t third[4] = {
        0x01234567u, 0x89abcdefu, 0xfedcba98u, 0x76543210u,
    };
    uint32_t key[4] = { 0 };
    mbedtls_aes_context aes;

    g_ef.key_src = ctx->hw_src;

    if (len != 16) {
        return -1;
    }
    if (!ctx->keylen && ctx->hw_sel == 1 && g_ef.key_src == 0 &&
        (g_ef.tzc & SBOOT_MSK) == 0) {
        if (g_ef.engine == FAKE_ENG_LATCH) {
            memcpy(key, g_ef.key_boot, sizeof(key));
        } else if (g_ef.engine == FAKE_ENG_THIRD) {
            memcpy(key, third, sizeof(key));
        } else if (g_ef.engine == FAKE_ENG_LIVE ||
                   (g_ef.fuse[W_LOCK] & LOCK_RD(4)) == 0) {
            memcpy(key, &g_ef.fuse[W_SLOT(4)], sizeof(key));
        }
    }
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const uint8_t *)key, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, in, out);
    mbedtls_aes_free(&aes);
    memset(key, 0, sizeof(key));
    return 0;
}

void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}
