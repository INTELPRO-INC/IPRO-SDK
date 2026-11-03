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
 * HUK = eFuse key slot HUK_EFUSE_SLOT, as the SEC_ENG hardware key
 * (never in RAM) or read per PRF call into one wiped stack buffer.
 * huk_lock ends HUK use and may set tzc_sboot_done.
 */
#include <stdio.h>
#include <string.h>

#include "drv_ef_ctrl.h"
#include "hal_sec_aes_v2.h"
#include "huk.h"
#include "spm_aes.h"
#include "mbedtls/platform_util.h"
#include "spm_log.h"

/* tzc_sboot_done = TZC_SEC ROM_TZSRG_CTRL bits 31:28 */
#define HUK_SBOOT_POS    28u
#define HUK_SBOOT_MSK    (0xFu << HUK_SBOOT_POS)
#define HUK_SBOOT_LOCKED 0xFu

#ifdef SPM_HOST
/* Stand-ins defined by the host test. */
uint32_t huk_hw_tzc_read(void);
void huk_hw_tzc_write(uint32_t v);
#else
#include "compiler/common.h"
#include "drv_tzc.h"

_Static_assert(TZC_SEC_TZC_SBOOT_DONE_POS == HUK_SBOOT_POS &&
               TZC_SEC_TZC_SBOOT_DONE_MSK == HUK_SBOOT_MSK,
               "tzc_sboot_done field");

static uint32_t huk_hw_tzc_read(void)
{
    return getreg32(TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}

static void huk_hw_tzc_write(uint32_t v)
{
    putreg32(v, TZC_SEC_BASE + TZC_SEC_TZC_ROM_TZSRG_CTRL_OFFSET);
}
#endif

/* tzc_sboot_done; while non-zero every SEC_ENG hardware key is zero */
static uint32_t huk_hw_sboot_done(void)
{
    return (huk_hw_tzc_read() & HUK_SBOOT_MSK) >> HUK_SBOOT_POS;
}

#ifdef CONFIG_PSA_HUK_IN_RAM
#define HUK_RAM_ALLOWED 1
#else
#define HUK_RAM_ALLOWED 0
#endif
#ifdef CONFIG_PSA_HUK_KAT
#define HUK_KAT 1
#else
#define HUK_KAT 0
#endif
#ifdef CONFIG_PSA_DEV_HUK
#define HUK_DEV_ALLOWED 1
#else
#define HUK_DEV_ALLOWED 0
#endif
#ifdef CONFIG_PSA_HUK_LOCK
#define HUK_LOCK 1
#else
#define HUK_LOCK 0
#endif
#if !HUK_RAM_ALLOWED && CONFIG_PSA_HUK_KEY_SEL == 0
#error "CONFIG_PSA_HUK_KEY_SEL 0 is the zero key: no HUK on the engine path"
#endif

typedef int (*huk_aes_fn)(const uint8_t in[16], uint8_t out[16]);

/* huk_hw_check results */
enum huk_hw {
    HUK_HW_OK,
    HUK_HW_FAIL,        /* KAT mismatch */
    HUK_HW_ERR,         /* engine error */
    HUK_HW_BLANK,       /* zero or all-ones key: not provisioned */
    HUK_HW_LOCKED,      /* tzc_sboot_done set */
};

/* "IPRO7-PSA-DEVHUK": the development HUK of unprovisioned parts */
static const uint8_t huk_dev_key[16] = {
    0x49, 0x50, 0x52, 0x4f, 0x37, 0x2d, 0x50, 0x53,
    0x41, 0x2d, 0x44, 0x45, 0x56, 0x48, 0x55, 0x4b,
};
static const uint8_t huk_zero_key[16];
static const uint8_t huk_ones_key[16] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
/* PRF input of the init checks, KDF counter field 0 */
static const uint8_t huk_probe[16] = "HUK-CHECK";
/* AES-128 of the zero block under the zero key */
static const uint8_t huk_zero_ct[16] = {
    0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
    0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e,
};

/* huk_prf's source; NULL until huk_init picks one */
static huk_aes_fn huk_src;
/* tzc_sboot_done read by huk_init; all ones when not read */
static uint32_t huk_sboot_init = UINT32_MAX;
/* 1 after huk_lock; 2 once the refusal is logged */
static int huk_locked;
/* 1 when the huk_init check block got an engine error */
static int huk_engine_err;

static int huk_soft_aes(const uint8_t key[16], const uint8_t in[16],
                        uint8_t out[16])
{
    mbedtls_aes_context ctx;
    int ret;

    mbedtls_aes_init(&ctx);
    ret = mbedtls_aes_setkey_enc(&ctx, key, 128);
    if (ret == 0) {
        ret = mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, in, out);
    }
    mbedtls_aes_free(&ctx);
    return ret == 0 ? 0 : -1;
}

/*
 * SEC_ENG ECB under the hardware key CONFIG_PSA_HUK_KEY_SEL with
 * SE_AES_SBOOT key source 0
 */
static int huk_hw_aes(const uint8_t in[16], uint8_t out[16])
{
    hal_sec_aes_v2_ctx_t c;
    int ret;

    ret = hal_sec_aes_v2_setkey_hw(&c, HAL_SEC_AES_V2_ECB,
                                   HAL_SEC_AES_V2_ENCRYPT,
                                   CONFIG_PSA_HUK_KEY_SEL, 16);
    if (ret == 0) {
        ret = hal_sec_aes_v2_set_hw_src(&c, 0);
    }
    if (ret == 0) {
        ret = hal_sec_aes_v2_crypt(&c, in, out, 16);
    }
    hal_sec_aes_v2_abort(&c);
    return ret == 0 ? 0 : -1;
}

/*
 * Software AES under eFuse slot HUK_EFUSE_SLOT (words as read, LE);
 * -1 when the slot reads all zero or all ones.
 */
static int huk_ram_aes(const uint8_t in[16], uint8_t out[16])
{
    uint32_t key[4];
    uint32_t any = 0;
    uint32_t all = 0xFFFFFFFFu;
    int ret = -1;

    EF_Ctrl_Read_AES_Key(HUK_EFUSE_SLOT, key, 4);
    for (size_t i = 0; i < 4; i++) {
        any |= key[i];
        all &= key[i];
    }
    if (any != 0 && all != 0xFFFFFFFFu) {
        ret = huk_soft_aes((const uint8_t *)key, in, out);
    }
    mbedtls_platform_zeroize(key, sizeof(key));
    return ret;
}

static int huk_dev_aes(const uint8_t in[16], uint8_t out[16])
{
    return huk_soft_aes(huk_dev_key, in, out);
}

/*
 * The hardware key, from one probe block: LOCKED with tzc_sboot_done
 * set, ERR on an engine error; with HUK_KAT OK when it equals
 * huk_ram_aes, else FAIL; without, BLANK for an all-0 or all-1 key.
 */
static enum huk_hw huk_hw_check(uint32_t sboot_done)
{
    uint8_t hw[16];
    uint8_t ref[16];
    enum huk_hw r = HUK_HW_FAIL;

    if (sboot_done != 0) {
        return HUK_HW_LOCKED;
    }
    if (huk_hw_aes(huk_probe, hw) != 0) {
        r = HUK_HW_ERR;
    } else if (HUK_KAT) {
        r = huk_ram_aes(huk_probe, ref) == 0 && memcmp(hw, ref, 16) == 0
                ? HUK_HW_OK : HUK_HW_FAIL;
    } else if (huk_soft_aes(huk_zero_key, huk_probe, ref) == 0 &&
               memcmp(hw, ref, 16) != 0 &&
               huk_soft_aes(huk_ones_key, huk_probe, ref) == 0 &&
               memcmp(hw, ref, 16) != 0) {
        r = HUK_HW_OK;
    } else {
        r = HUK_HW_BLANK;
    }
    mbedtls_platform_zeroize(hw, sizeof(hw));
    mbedtls_platform_zeroize(ref, sizeof(ref));
    return r;
}

static int huk_ram_ok(void)
{
    uint8_t out[16];
    int ok = huk_ram_aes(huk_probe, out) == 0;

    mbedtls_platform_zeroize(out, sizeof(out));
    return ok;
}

int huk_init(void)
{
    char why[40] = "";
    int blank = 0;

    if (huk_locked) {
        return -1;
    }
    huk_src = NULL;
    huk_engine_err = 0;
    if (!HUK_RAM_ALLOWED || HUK_KAT) {
        uint32_t sboot_done = huk_hw_sboot_done();

        huk_sboot_init = sboot_done;
        switch (huk_hw_check(sboot_done)) {
        case HUK_HW_OK:
            huk_src = huk_hw_aes;
            SPM_LOG("HUK via SEC_ENG KEY_SEL %u",
                    (unsigned)CONFIG_PSA_HUK_KEY_SEL);
            break;
        case HUK_HW_LOCKED:
            snprintf(why, sizeof(why), "sboot_done 0x%lx",
                     (unsigned long)sboot_done);
            break;
        case HUK_HW_BLANK:
            blank = 1;
            break;
        case HUK_HW_ERR:
            huk_engine_err = 1;
            snprintf(why, sizeof(why), "SEC_ENG error");
            break;
        default:
            snprintf(why, sizeof(why), "hardware key check failed");
            break;
        }
    }
    if (huk_src == NULL && HUK_RAM_ALLOWED) {
        if (why[0] != '\0') {
            SPM_LOG("HUK engine unavailable: %s", why);
            why[0] = '\0';
        }
        huk_src = huk_ram_ok() ? huk_ram_aes : NULL;
        blank = huk_src == NULL;
    }
    /* a locked or failed engine: no HUK, no development HUK */
    if (huk_src == NULL && why[0] != '\0') {
        SPM_LOG("HUK unavailable: %s, ITS and attestation disabled", why);
        return -1;
    }
    if (huk_src == NULL && blank) {
        SPM_LOG("HUK not provisioned");
    }
    if (huk_src == NULL && HUK_DEV_ALLOWED) {
        SPM_LOG("development HUK in use");
        huk_src = huk_dev_aes;
    }
    return huk_src != NULL ? 0 : -1;
}

int huk_prf(const uint8_t in[16], uint8_t out[16])
{
    int ret = -1;

    if (huk_locked == 1) {
        SPM_LOG("HUK locked: PRF refused");
        huk_locked = 2;
    } else if (!huk_locked && huk_src != NULL) {
        ret = huk_src(in, out);
    }
    if (ret != 0) {
        memset(out, 0, 16);
    }
    return ret;
}

int huk_lock(void)
{
    uint8_t zero[16] = { 0 };
    uint8_t ct[16];
    uint32_t got;
    int ret;
    int ok;

    huk_locked = huk_locked ? huk_locked : 1;
    if (!HUK_LOCK || huk_sboot_init != 0) {
        return 1;
    }
    huk_sboot_init = UINT32_MAX;
    huk_hw_tzc_write((huk_hw_tzc_read() & ~HUK_SBOOT_MSK) |
                     (HUK_SBOOT_LOCKED << HUK_SBOOT_POS));
    got = huk_hw_sboot_done();
    /* an engine that already failed at init may fail here too */
    ret = huk_hw_aes(zero, ct);
    ok = ret == 0 ? memcmp(ct, huk_zero_ct, 16) == 0 : huk_engine_err;
    mbedtls_platform_zeroize(ct, sizeof(ct));
    if (got != HUK_SBOOT_LOCKED || !ok) {
        SPM_LOG("HUK route still open (sboot_done 0x%lx)",
                (unsigned long)got);
        return -1;
    }
    SPM_LOG("HUK locked (sboot_done 0x%lx)%s", (unsigned long)got,
            ret == 0 ? "" : ", SEC_ENG error");
    return 0;
}

int huk_is_dev(void)
{
    return huk_src == huk_dev_aes;
}
