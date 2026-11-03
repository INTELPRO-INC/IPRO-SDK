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
 * HUK provisioning: read-only checks, plan and digest, host
 * confirmation, TRNG draw, slot 4 burn, KCV, slot 4 locks, wipe.
 * Sequence and lines: apps/security/ipro7_psa_spe/README.md.
 */
#include <stdio.h>
#include <string.h>

#include "huk.h"
#include "huk_prov.h"
#include "spm_aes.h"
#include "mbedtls/platform_util.h"
#include "spm_sha256.h"

#if defined(CONFIG_PSA_HUK_IN_RAM) || defined(CONFIG_PSA_DEV_HUK) || \
    defined(CONFIG_PSA_HUK_KAT) || !defined(CONFIG_PSA_HUK_LOCK) || \
    CONFIG_PSA_HUK_KEY_SEL != 1
#error "HUK provisioning needs KEY_SEL 1 + HUK_LOCK, no IN_RAM/DEV/KAT"
#endif
#if HUK_EFUSE_SLOT != 4
#error "HUK provisioning burns slot 4 (XIP AES-128 parts)"
#endif

/* eFuse R0 words (byte offset / 4) */
#define EF_WORDS        32u
#define EF_W_CFG0       0u      /* 0x00 ef_cfg_0 */
#define EF_W_CFG1       1u      /* 0x04 */
#define EF_W_SVN        19u     /* 0x4C anti-rollback counter */
#define EF_W_LOCK       31u     /* 0x7C locks, lifecycle [3:0] */
#define EF_W_SLOT(n)    (7u + 4u * (n))     /* key slot n, 4 words */
#define LOCK_WR(n)      (1u << (19u + (n)))
#define LOCK_RD(n)      (1u << (26u + (n)))
#define LIFECYCLE_MSK   0xFu
#define HUK_SLOT        4u
/* 0x7C bits 23 (slot 4 write lock) and 30 (slot 4 read lock) */
#define PLAN_LOCKS      (LOCK_WR(HUK_SLOT) | LOCK_RD(HUK_SLOT))

/* ef_cfg_0 and word 0x04 fields */
#define CFG0_AES_MODE(w)    ((w) & 0x3u)
#define CFG0_SIGN_MODE(w)   (((w) >> 2) & 0x3u)
#define CFG0_CPU0_ENC       (1u << 7)
#define CFG0_KEY_ENC_EN     (1u << 17)
#define CFG1_KEEP_DBG       (1u << 23)
#define CFG1_UARTBOOT_DIS   (1u << 25)

/* SEC_ENG TRNG fields */
#define TRNG_HT_ERROR       (1u << 4)
#define TEST_RAW_BITS       0x7u        /* test_en, cp_test_en, cp_bypass */
#define TEST_HT_DIS         (1u << 3)
#define TEST_ALARM_N(t)     (((t) >> 4) & 0xFFu)
#define CTRL3_CUT_MSK       0x03FFFFFFu /* cp_ratio, ht_rct_c, ht_apt_c */
#define CTRL3_CP_RATIO      3u
#define CTRL3_RCT           26u
#define CTRL3_APT           655u
#define CTRL3_CUTOFFS       (CTRL3_CP_RATIO | (CTRL3_RCT << 8) | \
                             (CTRL3_APT << 16))
#define ROSC_C1_ASSESSED    0x0F00u     /* oscillators 4-7 (FiGaRO) */
#define ROSC_C1_PROBE       0xFF00u
#define PROT_SE_DBG_DIS     (1u << 31)
#define SW_SE_DBG_DIS       (1u << 22)
#define TRNG_SENTINEL       0xDEADBEEFu

#define BCLK_MHZ            96u
#define CONFIRM_MS          60000u
#define CONFIRM_MAX         47u

/* AES-128 of the zero block under the zero key */
static const uint8_t hukp_zero_ct[16] = {
    0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
    0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e,
};
static const uint8_t hukp_zero_block[16];

/* everything that may hold a key; wiped at the end of every run */
static struct {
    uint32_t snap[EF_WORDS];    /* eFuse at the checks, slot 2 included */
    uint32_t now[EF_WORDS];     /* eFuse before the burn, after locks */
    uint32_t g1[8];
    uint32_t g2[8];
    uint32_t g3[8];
    uint32_t huk[4];
    uint32_t rb[4];
    uint8_t kcv_sw[16];
    uint8_t kcv_hw[16];
} hukp_s;

static void hukp_hex(char *out, const uint8_t *b, size_t n)
{
    static const char digits[] = "0123456789abcdef";

    for (size_t i = 0; i < n; i++) {
        out[2 * i] = digits[b[i] >> 4];
        out[2 * i + 1] = digits[b[i] & 0xFu];
    }
    out[2 * n] = '\0';
}

static uint32_t hukp_ct_diff(const void *a, const void *b, size_t n)
{
    const uint8_t *x = a;
    const uint8_t *y = b;
    uint32_t d = 0;

    for (size_t i = 0; i < n; i++) {
        d |= (uint32_t)(x[i] ^ y[i]);
    }
    return d;
}

static void hukp_ef_read_all(uint32_t w[EF_WORDS])
{
    hukp_ef_load();
    for (uint32_t i = 0; i < EF_WORDS; i++) {
        w[i] = hukp_ef_get(i);
    }
}

static int hukp_slot_word(uint32_t i, uint32_t slot)
{
    return i >= EF_W_SLOT(slot) && i < EF_W_SLOT(slot) + 4u;
}

static int hukp_slot_zero(const uint32_t w[EF_WORDS], uint32_t slot)
{
    uint32_t any = 0;

    for (uint32_t i = 0; i < 4; i++) {
        any |= w[EF_W_SLOT(slot) + i];
    }
    return any == 0;
}

/* digest flag of a key slot: 0 blank, 1 set, 2 locked */
static uint8_t hukp_slot_flag(const uint32_t w[EF_WORDS], uint32_t slot)
{
    if ((w[EF_W_LOCK] & (LOCK_WR(slot) | LOCK_RD(slot))) != 0) {
        return 2;
    }
    return hukp_slot_zero(w, slot) ? 0 : 1;
}

static void hukp_le32(uint8_t b[4], uint32_t v)
{
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
    b[2] = (uint8_t)(v >> 16);
    b[3] = (uint8_t)(v >> 24);
}

static int hukp_image_sha(uint8_t out[32])
{
    size_t len = 0;
    const uint8_t *img = hukp_image(&len);

    return mbedtls_sha256_ret(img, len, out, 0);
}

/*
 * SHA-256("HUKPROV1" || chip id || words 0x00-0x7C, slots 2 and 4 as
 * one flag byte each || slot 4 word, word count, lock mask || image
 * SHA-256), first 8 bytes.
 */
static int hukp_digest(const uint8_t chip[8], const uint32_t w[EF_WORDS],
                       const uint8_t img[32], uint8_t dig[8])
{
    static const char tag[] = "HUKPROV1";
    mbedtls_sha256_context sha;
    uint8_t b[32];
    int ret;

    mbedtls_sha256_init(&sha);
    ret = mbedtls_sha256_starts_ret(&sha, 0);
    ret = ret != 0 ? ret : mbedtls_sha256_update_ret(&sha,
                                     (const uint8_t *)tag, 8);
    ret = ret != 0 ? ret : mbedtls_sha256_update_ret(&sha, chip, 8);
    for (uint32_t i = 0; ret == 0 && i < EF_WORDS; i++) {
        uint32_t slot = hukp_slot_word(i, 2) ? 2u :
                        hukp_slot_word(i, HUK_SLOT) ? HUK_SLOT : 0u;

        if (slot == 0) {
            hukp_le32(b, w[i]);
            ret = mbedtls_sha256_update_ret(&sha, b, 4);
        } else if (i == EF_W_SLOT(slot)) {
            b[0] = hukp_slot_flag(w, slot);
            ret = mbedtls_sha256_update_ret(&sha, b, 1);
        }
    }
    hukp_le32(b, EF_W_SLOT(HUK_SLOT));
    hukp_le32(b + 4, 4u);
    hukp_le32(b + 8, PLAN_LOCKS);
    ret = ret != 0 ? ret : mbedtls_sha256_update_ret(&sha, b, 12);
    ret = ret != 0 ? ret : mbedtls_sha256_update_ret(&sha, img, 32);
    ret = ret != 0 ? ret : mbedtls_sha256_finish_ret(&sha, b);
    mbedtls_sha256_free(&sha);
    memcpy(dig, b, 8);
    return ret;
}

static enum hukp_result hukp_refuse(const char *why)
{
    printf("[HUKP] refused: %s\r\n", why);
    return HUKP_REFUSED;
}

/* engine block of the zero input after huk_init; -1 without HUK */
static int hukp_engine_kcv(uint8_t kcv[16])
{
    if (hukp_tzc_sboot_done() != 0 || huk_init() != 0) {
        memset(kcv, 0, 16);
        return -1;
    }
    return huk_prf(hukp_zero_block, kcv);
}

static const char *hukp_trng_setup(uint32_t *alarm_n);

/*
 * slot 4 already locked: TRNG set-up and lock, engine KCV, route
 * closed, nothing burned
 */
static enum hukp_result hukp_provisioned(void)
{
    char h[33] = "unavailable";
    uint32_t alarm_n = 0;
    const char *why = hukp_trng_setup(&alarm_n);

    printf("[HUKP] trng set-up %s: rosc_c1 0x%05lx se_dbg_dis %lu "
           "ctrl_3 0x%08lx alarm_n %lu\r\n", why != NULL ? why : "ok",
           (unsigned long)hukp_reg_read(HUKP_TRNG_ROSC_C1),
           (unsigned long)(hukp_reg_read(HUKP_SE_PROT_RD) >> 31),
           (unsigned long)hukp_reg_read(HUKP_TRNG_CTRL_3),
           (unsigned long)TEST_ALARM_N(hukp_reg_read(HUKP_TRNG_TEST)));
    if (hukp_engine_kcv(hukp_s.kcv_hw) == 0) {
        hukp_hex(h, hukp_s.kcv_hw, 16);
    }
    (void)huk_lock();
    printf("[HUKP] refused: slot4 provisioned, engine kcv %s\r\n", h);
    return HUKP_REFUSED;
}

/* R10-R13: engine off, cutoffs, source, software lock; NULL when ok */
static const char *hukp_trng_setup(uint32_t *alarm_n)
{
    uint32_t t;

    (void)hukp_trng_enable(0);
    if ((hukp_reg_read(HUKP_TRNG_TEST) & TEST_RAW_BITS) != 0) {
        return "trng test register";
    }
    hukp_reg_write(HUKP_TRNG_CTRL_3,
                   (hukp_reg_read(HUKP_TRNG_CTRL_3) & ~CTRL3_CUT_MSK) |
                   CTRL3_CUTOFFS);
    hukp_reg_write(HUKP_TRNG_ROSC_C1, ROSC_C1_ASSESSED);
    if (hukp_reg_read(HUKP_TRNG_ROSC_C1) != ROSC_C1_ASSESSED) {
        return "trng source";
    }
    hukp_reg_write(HUKP_EF_SW_CFG_0,
                   hukp_reg_read(HUKP_EF_SW_CFG_0) | SW_SE_DBG_DIS);
    hukp_reg_write(HUKP_TRNG_ROSC_C1, ROSC_C1_PROBE);
    if ((hukp_reg_read(HUKP_SE_PROT_RD) & PROT_SE_DBG_DIS) == 0 ||
        hukp_reg_read(HUKP_TRNG_ROSC_C1) != ROSC_C1_ASSESSED) {
        return "trng lock";
    }
    t = hukp_reg_read(HUKP_TRNG_TEST);
    if ((t & TEST_RAW_BITS) != 0) {
        return "trng test register";
    }
    if ((t & TEST_HT_DIS) != 0 ||
        (hukp_reg_read(HUKP_TRNG_CTRL_3) & CTRL3_CUT_MSK) != CTRL3_CUTOFFS) {
        return "trng cutoffs";
    }
    *alarm_n = TEST_ALARM_N(t);
    return NULL;
}

/* R15: 1 when the first non-empty line within 60 s equals want */
static int hukp_confirmed(const char *want)
{
    char line[CONFIRM_MAX + 1];
    size_t n = 0;
    int over = 0;

    for (uint32_t ms = 0; ms < CONFIRM_MS; ms++) {
        int c;

        while ((c = hukp_uart_getc()) >= 0) {
            if (c != '\r' && c != '\n') {
                over |= n == CONFIRM_MAX;
                if (n < CONFIRM_MAX) {
                    line[n++] = (char)c;
                }
                continue;
            }
            if (n == 0 && !over) {
                continue;
            }
            line[n] = '\0';
            return !over && strcmp(line, want) == 0;
        }
        hukp_delay_ms(1);
    }
    return 0;
}

/* one generation with the R18 checks */
static const char *hukp_generate(uint32_t out[8])
{
    uint32_t bad = 0;

    if (hukp_trng_generate(out) != 0) {
        return "trng error";
    }
    for (uint32_t i = 0; i < 8; i++) {
        bad |= out[i] == TRNG_SENTINEL;
    }
    if (bad || (hukp_reg_read(HUKP_TRNG_CTRL_0) & TRNG_HT_ERROR) != 0) {
        return "trng health test";
    }
    return NULL;
}

static uint32_t hukp_weight(const uint32_t w[4])
{
    uint32_t n = 0;

    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t v = w[i]; v != 0; v &= v - 1u) {
            n++;
        }
    }
    return n;
}

/* D, R16-R19: HUK = G2 words 0-3 of a second instantiate */
static const char *hukp_draw(void)
{
    static const uint32_t ones[4] = {
        0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
    };
    const uint32_t *slot2 = &hukp_s.snap[EF_W_SLOT(2)];
    const char *why;
    uint32_t same = 0;
    uint32_t weight;

    if (hukp_trng_enable(1) != 0) {
        return "trng error";
    }
    if ((why = hukp_generate(hukp_s.g1)) != NULL) {
        return why;
    }
    (void)hukp_trng_enable(0);
    if (hukp_trng_enable(1) != 0) {
        return "trng error";
    }
    if ((why = hukp_generate(hukp_s.g2)) != NULL) {
        return why;
    }
    for (uint32_t i = 0; i < 8; i++) {
        same |= hukp_s.g1[i] == hukp_s.g2[i];
    }
    if (same) {
        return "trng stuck";
    }
    memcpy(hukp_s.huk, hukp_s.g2, sizeof(hukp_s.huk));
    mbedtls_platform_zeroize(hukp_s.g1, sizeof(hukp_s.g1));
    mbedtls_platform_zeroize(hukp_s.g2, sizeof(hukp_s.g2));
    why = hukp_generate(hukp_s.g3);
    mbedtls_platform_zeroize(hukp_s.g3, sizeof(hukp_s.g3));
    if (why != NULL) {
        return why;
    }
    weight = hukp_weight(hukp_s.huk);
    if (weight == 0 || hukp_ct_diff(hukp_s.huk, ones, 16) == 0 ||
        ((hukp_s.snap[EF_W_LOCK] & LOCK_RD(2)) == 0 &&
         hukp_ct_diff(hukp_s.huk, slot2, 16) == 0) ||
        weight < 32u || weight > 96u) {
        return "trng value rejected";
    }
    printf("[HUKP] trng ok: 2 instantiates, ht_error 0, sentinel 0\r\n");
    return NULL;
}

static int hukp_soft_kcv(const uint32_t key[4], uint8_t out[16])
{
    mbedtls_aes_context ctx;
    int ret;

    mbedtls_aes_init(&ctx);
    ret = mbedtls_aes_setkey_enc(&ctx, (const uint8_t *)key, 128);
    if (ret == 0) {
        ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT,
                                    hukp_zero_block, out);
    }
    mbedtls_aes_free(&ctx);
    return ret;
}

/*
 * end of a run that burned: route closed; shadow slot 4 cleared unless
 * the controller stayed busy (busy != 0)
 */
static enum hukp_result hukp_burn_end(const char *fail, int busy)
{
    int lock = huk_lock();

    for (uint32_t i = 0; !busy && i < 4; i++) {
        hukp_ef_set(EF_W_SLOT(HUK_SLOT) + i, 0);
    }
    if (fail == NULL && lock != 0) {
        fail = "route open";
    }
    if (fail != NULL) {
        printf("[HUKP] FAIL: %s\r\n", fail);
        return HUKP_FAIL;
    }
    printf("[HUKP] DONE, reset the part\r\n");
    return HUKP_DONE;
}

/* F, G: value, readback, KCV, engine, locks, readback, engine */
static enum hukp_result hukp_burn(void)
{
    uint32_t missing = 0;
    uint32_t extra = 0;
    uint32_t lock;
    uint32_t other = 0;
    uint32_t rb_any = 0;
    int engine;
    int engine_ok;
    char h[33];

    hukp_ef_load();
    for (uint32_t i = 0; i < 4; i++) {
        hukp_ef_set(EF_W_SLOT(HUK_SLOT) + i, hukp_s.huk[i]);
    }
    if (hukp_ef_program() != 0) {
        return hukp_burn_end("program timeout", 1);
    }
    hukp_ef_load();
    for (uint32_t i = 0; i < 4; i++) {
        hukp_s.rb[i] = hukp_ef_get(EF_W_SLOT(HUK_SLOT) + i);
        missing |= hukp_s.huk[i] & ~hukp_s.rb[i];
        extra |= hukp_s.rb[i] & ~hukp_s.huk[i];
    }
    if (missing != 0 || extra != 0) {
        return hukp_burn_end(missing == 0 ? "bits extra" :
                             extra == 0 ? "bits missing" :
                             "bits missing and extra", 0);
    }
    printf("[HUKP] slot4 programmed, readback match\r\n");
    if (hukp_soft_kcv(hukp_s.huk, hukp_s.kcv_sw) != 0) {
        return hukp_burn_end("software aes", 0);
    }
    hukp_hex(h, hukp_s.kcv_sw, 16);
    printf("[HUKP] kcv %s\r\n", h);
    engine = hukp_engine_kcv(hukp_s.kcv_hw);
    engine_ok = engine == 0 &&
                hukp_ct_diff(hukp_s.kcv_hw, hukp_s.kcv_sw, 16) == 0;
    /* zero-key block or no HUK at init: engine pending reset */
    if (!engine_ok && engine == 0 &&
        memcmp(hukp_s.kcv_hw, hukp_zero_ct, 16) != 0) {
        return hukp_burn_end("engine third value", 0);
    }
    printf("[HUKP] engine %s\r\n", engine_ok ? "ok" : "pending reset");

    hukp_ef_load();
    lock = hukp_ef_get(EF_W_LOCK);
    hukp_ef_set(EF_W_LOCK, lock | PLAN_LOCKS);
    if (hukp_ef_program() != 0) {
        return hukp_burn_end("program timeout", 1);
    }
    hukp_ef_read_all(hukp_s.now);
    if (hukp_s.now[EF_W_LOCK] != (hukp_s.snap[EF_W_LOCK] | PLAN_LOCKS)) {
        return hukp_burn_end("lock word", 0);
    }
    for (uint32_t i = 0; i < EF_W_LOCK; i++) {
        if (!hukp_slot_word(i, HUK_SLOT)) {
            other |= hukp_s.now[i] ^ hukp_s.snap[i];
        }
    }
    if (other != 0) {
        return hukp_burn_end("other word changed", 0);
    }
    for (uint32_t i = 0; i < 4; i++) {
        hukp_s.rb[i] = hukp_s.now[EF_W_SLOT(HUK_SLOT) + i];
        rb_any |= hukp_s.rb[i];
    }
    if (rb_any != 0 && hukp_ct_diff(hukp_s.rb, hukp_s.huk, 16) != 0) {
        return hukp_burn_end("slot4 readback", 0);
    }
    printf("[HUKP] 7c %08lx -> %08lx, other words unchanged\r\n",
           (unsigned long)hukp_s.snap[EF_W_LOCK],
           (unsigned long)hukp_s.now[EF_W_LOCK]);
    printf("[HUKP] slot4 cpu read %s\r\n",
           rb_any == 0 ? "zero" : "pending reset");
    if (engine_ok &&
        (huk_prf(hukp_zero_block, hukp_s.kcv_hw) != 0 ||
         hukp_ct_diff(hukp_s.kcv_hw, hukp_s.kcv_sw, 16) != 0)) {
        return hukp_burn_end("engine after locks", 0);
    }
    return hukp_burn_end(NULL, 0);
}

/* R2-R15: checks, plan, digest, confirmation; *go = 1 to draw */
static enum hukp_result hukp_check(int *go)
{
    const uint32_t *w = hukp_s.snap;
    uint8_t chip[8];
    uint8_t img[32];
    uint8_t dig[8];
    char chip_h[17];
    char img_h[17];
    char dig_h[17];
    char want[CONFIRM_MAX + 1];
    const char *why;
    uint32_t alarm_n = 0;
    uint32_t tzc;
    uint32_t bclk;
    int chip_ok;
    uint8_t chip_any = 0;

    *go = 0;
    if (!hukp_ef_ready()) {
        return hukp_refuse("efuse not ready");
    }
    hukp_ef_read_all(hukp_s.snap);
    chip_ok = hukp_chip_id(chip) == 0;
    for (uint32_t i = 0; i < 8; i++) {
        chip_any |= chip[i];
    }
    if (hukp_image_sha(img) != 0) {
        return hukp_refuse("image hash");
    }
    tzc = hukp_tzc_sboot_done();
    bclk = hukp_bclk_mhz();
    hukp_hex(chip_h, chip, 8);
    hukp_hex(img_h, img, 8);
    printf("[HUKP] chip %s image %s\r\n", chip_h, img_h);
    printf("[HUKP] efuse 00=%08lx 04=%08lx 4c=%08lx 7c=%08lx bit17=%u\r\n",
           (unsigned long)w[EF_W_CFG0], (unsigned long)w[EF_W_CFG1],
           (unsigned long)w[EF_W_SVN], (unsigned long)w[EF_W_LOCK],
           (unsigned)((w[EF_W_CFG0] & CFG0_KEY_ENC_EN) != 0));
    printf("[HUKP] state tzc_sboot_done 0x%lx bclk %lu MHz rosc_c1 0x%05lx"
           " trng_test 0x%03lx ctrl_3 0x%08lx\r\n", (unsigned long)tzc,
           (unsigned long)bclk,
           (unsigned long)hukp_reg_read(HUKP_TRNG_ROSC_C1),
           (unsigned long)hukp_reg_read(HUKP_TRNG_TEST),
           (unsigned long)hukp_reg_read(HUKP_TRNG_CTRL_3));
    if ((w[EF_W_LOCK] & PLAN_LOCKS) != 0) {
        return hukp_provisioned();
    }
    if (!chip_ok || chip_any == 0) {
        return hukp_refuse("chip id");
    }
    if (!hukp_slot_zero(w, HUK_SLOT)) {
        return hukp_refuse("slot4 not blank");
    }
    if (!hukp_slot_zero(w, 5) ||
        (w[EF_W_LOCK] & (LOCK_WR(5) | LOCK_RD(5))) != 0) {
        return hukp_refuse("slot5 not blank");
    }
    if (CFG0_SIGN_MODE(w[EF_W_CFG0]) != 1u ||
        CFG0_AES_MODE(w[EF_W_CFG0]) != 1u ||
        (w[EF_W_CFG0] & CFG0_CPU0_ENC) == 0 ||
        (w[EF_W_CFG0] & CFG0_KEY_ENC_EN) != 0) {
        return hukp_refuse("ef_cfg_0");
    }
    if ((w[EF_W_CFG1] & CFG1_KEEP_DBG) == 0 ||
        (w[EF_W_CFG1] & CFG1_UARTBOOT_DIS) != 0) {
        return hukp_refuse("word 0x04");
    }
    if (tzc != 0) {
        return hukp_refuse("tzc_sboot_done");
    }
    if (w[EF_W_SVN] != 0 || (w[EF_W_LOCK] & LIFECYCLE_MSK) != 0) {
        return hukp_refuse("counter or lifecycle");
    }
    if (bclk != BCLK_MHZ) {
        return hukp_refuse("bclk");
    }
    if ((why = hukp_trng_setup(&alarm_n)) != NULL) {
        return hukp_refuse(why);
    }
    if (hukp_digest(chip, w, img, dig) != 0) {
        return hukp_refuse("digest");
    }
    hukp_hex(dig_h, dig, 8);
    printf("[HUKP] slot4 blank unlocked, slot5 blank, slot2 %s\r\n",
           (w[EF_W_LOCK] & LOCK_RD(2)) != 0 ? "locked" :
           hukp_slot_zero(w, 2) ? "blank" : "set readable");
    printf("[HUKP] trng rosc_c1=0x%04x locked, rct=%u apt=%u cp=%u "
           "alarm_n=%lu, bclk %u MHz\r\n", (unsigned)ROSC_C1_ASSESSED,
           (unsigned)CTRL3_RCT, (unsigned)CTRL3_APT,
           (unsigned)CTRL3_CP_RATIO, (unsigned long)alarm_n,
           (unsigned)BCLK_MHZ);
    printf("[HUKP] plan: slot4 <- 128 TRNG bits; 7c |= 0x%08lx "
           "(bits 23, 30)\r\n", (unsigned long)PLAN_LOCKS);
    printf("[HUKP] digest %s; send \"HUKPROV %s %s\" within 60 s\r\n",
           dig_h, chip_h, dig_h);
    snprintf(want, sizeof(want), "HUKPROV %s %s", chip_h, dig_h);
    if (!hukp_confirmed(want)) {
        return hukp_refuse("no confirmation");
    }
    *go = 1;
    return HUKP_REFUSED;
}

enum hukp_result huk_prov_run(void)
{
    enum hukp_result r;
    const char *why;
    int go;

    r = hukp_check(&go);
    if (go) {
        hukp_irq_off();
        why = hukp_draw();
        if (why == NULL) {
            hukp_ef_read_all(hukp_s.now);
            if (hukp_ct_diff(hukp_s.now, hukp_s.snap,
                             sizeof(hukp_s.now)) != 0) {
                why = "efuse changed";
            }
        }
        r = why != NULL ? hukp_refuse(why) : hukp_burn();
    }
    if (hukp_tzc_sboot_done() == 0) {
        hukp_tzc_close();
        printf("[HUKP] tzc_sboot_done 0x0 -> 0x%lx\r\n",
               (unsigned long)hukp_tzc_sboot_done());
    }
    mbedtls_platform_zeroize(&hukp_s, sizeof(hukp_s));
    return r;
}

#ifdef SPM_HOST
const void *huk_prov_state(size_t *len)
{
    *len = sizeof(hukp_s);
    return &hukp_s;
}
#else
void huk_prov_main(void)
{
    (void)huk_prov_run();
    for (;;) {
        __asm__ volatile("wfi");
    }
}
#endif
