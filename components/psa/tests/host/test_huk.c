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
 * Host test of huk.c with eFuse, SEC_ENG and TZC stand-ins, built once
 * per HUK_CFG (1..6, see Makefile) with that config's expected table.
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "check.h"
#include "drv_ef_ctrl.h"
#include "hal_sec_aes_v2.h"
#include "huk.h"
#include "spm_aes.h"

enum { SLOT_GOOD, SLOT_ZERO, SLOT_ONES };
enum { HW_SLOT, HW_OTHER, HW_ZERO, HW_ERR, HW_ONES };
enum { SRC_NONE, SRC_HW, SRC_RAM, SRC_DEV };

/* "HUK engine unavailable: <why>" lines: the RAM path follows */
#define LOG_HW   0x01u  /* hardware key check failed */
#define LOG_NP   0x02u  /* "HUK not provisioned" */
#define LOG_DEV  0x04u  /* "development HUK in use" */
#define LOG_VIA  0x08u  /* "HUK via SEC_ENG KEY_SEL 2" */
#define LOG_UNAV 0x10u  /* sboot_done 0xf */
#define LOG_ERR  0x20u  /* SEC_ENG error */
/* "HUK unavailable: <why>, ITS and attestation disabled" lines */
#define ABS_HW   0x40u  /* hardware key check failed */
#define ABS_LK   0x80u  /* sboot_done 0xf */
#define ABS_ERR  0x100u /* SEC_ENG error */
#define LOG_TWICE 0x8000u

struct scene {
    uint8_t slot;
    uint8_t hw;
    uint8_t sboot_done;
};

struct expect {
    uint8_t src;
    uint16_t logs;
};

#define N_SCENES 10u
static const struct scene g_scenes[N_SCENES] = {
    { SLOT_GOOD, HW_SLOT, 0 }, { SLOT_GOOD, HW_OTHER, 0 },
    { SLOT_ZERO, HW_OTHER, 0 }, { SLOT_ONES, HW_OTHER, 0 },
    { SLOT_GOOD, HW_ERR, 0 }, { SLOT_ZERO, HW_ZERO, 0 },
    { SLOT_GOOD, HW_ZERO, 0 }, { SLOT_GOOD, HW_SLOT, 0xF },
    { SLOT_ZERO, HW_OTHER, 0xF }, { SLOT_ZERO, HW_ONES, 0 },
};

#define R  SRC_RAM
#define H  SRC_HW
#define D  SRC_DEV
#define N  SRC_NONE
#define NPD (LOG_NP | LOG_DEV)
#if HUK_CFG == 1        /* IN_RAM, DEV */
#define CFG_NAME "IN_RAM DEV"
static const struct expect g_exp[N_SCENES] = {
    { R, 0 }, { R, 0 }, { D, NPD }, { D, NPD }, { R, 0 }, { D, NPD },
    { R, 0 }, { R, 0 }, { D, NPD }, { D, NPD },
};
#elif HUK_CFG == 2      /* IN_RAM */
#define CFG_NAME "IN_RAM"
static const struct expect g_exp[N_SCENES] = {
    { R, 0 }, { R, 0 }, { N, LOG_NP }, { N, LOG_NP }, { R, 0 },
    { N, LOG_NP }, { R, 0 }, { R, 0 }, { N, LOG_NP }, { N, LOG_NP },
};
#elif HUK_CFG == 3 || HUK_CFG == 6  /* IN_RAM, KAT, DEV */
#if HUK_CFG == 6
#define CFG_NAME "IN_RAM KAT DEV slot 2"
#else
#define CFG_NAME "IN_RAM KAT DEV"
#endif
static const struct expect g_exp[N_SCENES] = {
    { H, LOG_VIA }, { R, LOG_HW }, { D, LOG_HW | NPD }, { D, LOG_HW | NPD },
    { R, LOG_ERR }, { D, LOG_HW | NPD }, { R, LOG_HW }, { R, LOG_UNAV },
    { D, LOG_UNAV | NPD }, { D, LOG_HW | NPD },
};
#elif HUK_CFG == 4      /* DEV */
#define CFG_NAME "DEV"
static const struct expect g_exp[N_SCENES] = {
    { H, LOG_VIA }, { H, LOG_VIA }, { H, LOG_VIA }, { H, LOG_VIA },
    { N, ABS_ERR }, { D, NPD }, { D, NPD }, { N, ABS_LK }, { N, ABS_LK },
    { D, NPD },
};
#elif HUK_CFG == 5      /* KAT */
#define CFG_NAME "KAT"
static const struct expect g_exp[N_SCENES] = {
    { H, LOG_VIA }, { N, ABS_HW }, { N, ABS_HW }, { N, ABS_HW },
    { N, ABS_ERR }, { N, ABS_HW }, { N, ABS_HW }, { N, ABS_LK },
    { N, ABS_LK }, { N, ABS_HW },
};
#else
#error "HUK_CFG 1..6"
#endif

/* "IPRO7-PSA-DEVHUK" */
static const uint8_t g_dev_key[16] = {
    0x49, 0x50, 0x52, 0x4f, 0x37, 0x2d, 0x50, 0x53,
    0x41, 0x2d, 0x44, 0x45, 0x56, 0x48, 0x55, 0x4b,
};
static const uint8_t g_other_key[16] = {
    0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
    0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
};
static uint32_t g_slot[4];
static int g_hw;
static int g_slot_reads;
static int g_hw_runs;
static uint8_t g_last_sel;
/* SE_AES_SBOOT key source and tzc_sboot_done of the stand-ins */
static uint8_t g_key_src;
static uint32_t g_sboot_done;
static int g_sboot_reads;

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    g_slot_reads++;
    for (uint32_t i = 0; i < len; i++) {
        keyData[i] = index == HUK_EFUSE_SLOT ? g_slot[i] : 0x13579bdfu;
    }
}

/* ROM_TZSRG_CTRL: tzc_sboot_done in bits 31:28 */
uint32_t huk_hw_tzc_read(void)
{
    g_sboot_reads++;
    return g_sboot_done << 28 | 0x00123456u;
}

void huk_hw_tzc_write(uint32_t v)
{
    g_sboot_done = v >> 28;
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

static void aes(const uint8_t key[16], const uint8_t in[16], uint8_t out[16])
{
    mbedtls_aes_context ctx;

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, key, 128);
    mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, in, out);
    mbedtls_aes_free(&ctx);
}

/* hardware key of KEY_SEL 2 per g_hw; other KEY_SELs: all 0x77 */
static void hw_key_of(uint8_t sel, uint8_t key[16])
{
    memset(key, 0, 16);
    if (sel != 2) {
        memset(key, 0x77, 16);
    } else if (g_hw == HW_SLOT) {
        memcpy(key, g_slot, 16);
    } else if (g_hw == HW_OTHER) {
        memcpy(key, g_other_key, 16);
    } else if (g_hw == HW_ONES) {
        memset(key, 0xff, 16);
    }
}

/* hw_key_of, or all zero unless SE_AES_SBOOT and sboot_done are 0 */
static void hw_key(uint8_t sel, uint8_t key[16])
{
    hw_key_of(sel, key);
    if (g_key_src != 0 || g_sboot_done != 0) {
        memset(key, 0, 16);
    }
}

int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    uint8_t key[16];

    g_key_src = ctx->hw_src;

    g_hw_runs++;
    g_last_sel = ctx->hw_sel;
    if (g_hw == HW_ERR || !!ctx->keylen || len != 16) {
        return -1;
    }
    hw_key(ctx->hw_sel, key);
    aes(key, in, out);
    return 0;
}

void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

/* huk_init with stdout in log (the SPM_LOG lines) */
static int init_logged(char *log, size_t n)
{
    FILE *tmp = tmpfile();
    int saved;
    int ret;
    size_t got;

    fflush(stdout);
    saved = dup(1);
    dup2(fileno(tmp), 1);
    ret = huk_init();
    fflush(stdout);
    dup2(saved, 1);
    close(saved);
    rewind(tmp);
    got = fread(log, 1, n - 1, tmp);
    log[got] = '\0';
    fclose(tmp);
    return ret;
}

/* the log bits of log; LOG_TWICE when a line occurs twice */
static unsigned logs_of(const char *log)
{
    static const char *const line[] = {
        "[SPE] HUK engine unavailable: hardware key check failed\r\n",
        "[SPE] HUK not provisioned\r\n",
        "[SPE] development HUK in use\r\n",
        "[SPE] HUK via SEC_ENG KEY_SEL 2\r\n",
        "[SPE] HUK engine unavailable: sboot_done 0xf\r\n",
        "[SPE] HUK engine unavailable: SEC_ENG error\r\n",
        "[SPE] HUK unavailable: hardware key check failed, ITS and "
        "attestation disabled\r\n",
        "[SPE] HUK unavailable: sboot_done 0xf, ITS and attestation "
        "disabled\r\n",
        "[SPE] HUK unavailable: SEC_ENG error, ITS and attestation "
        "disabled\r\n",
    };
    unsigned bits = 0;

    for (size_t i = 0; i < sizeof(line) / sizeof(line[0]); i++) {
        const char *p = strstr(log, line[i]);

        if (p != NULL) {
            bits |= 1u << i;
            if (strstr(p + 1, line[i]) != NULL) {
                bits |= LOG_TWICE;
            }
        }
    }
    return bits;
}

static void set_slot(uint8_t slot)
{
    static const uint32_t good[4] = {
        0x03020100u, 0x07060504u, 0x0b0a0908u, 0x0f0e0d0cu,
    };

    for (size_t i = 0; i < 4; i++) {
        g_slot[i] = slot == SLOT_GOOD ? good[i]
                    : slot == SLOT_ZERO ? 0u : 0xFFFFFFFFu;
    }
}

/* 1 when huk_prf runs on source src */
static int runs_on(uint8_t src)
{
    static const uint8_t in[16] = "KDF-INPUT-BLOCK";
    uint8_t out[16];
    uint8_t ref[16];
    uint8_t key[16];
    int runs = g_hw_runs;
    int ret;

    g_key_src = 1;
    ret = huk_prf(in, out);
    if (src == SRC_NONE) {
        memset(ref, 0, 16);
        return ret == -1 && memcmp(out, ref, 16) == 0 && !huk_is_dev();
    }
    hw_key_of(2, key);
    aes(src == SRC_RAM ? (const uint8_t *)g_slot
        : src == SRC_DEV ? g_dev_key : key, in, ref);
    return ret == 0 && memcmp(out, ref, 16) == 0 &&
           (g_hw_runs != runs) == (src == SRC_HW) &&
           (src != SRC_HW || g_last_sel == 2) &&
           huk_is_dev() == (src == SRC_DEV);
}

int main(void)
{
    uint8_t out[16];
    char log[512];
    char what[96];

    memset(out, 0xA5, sizeof(out));
    check(huk_prf((const uint8_t *)"0123456789abcdef", out) == -1 &&
          out[0] == 0 && out[15] == 0 && !huk_is_dev(),
          "huk_prf before huk_init: -1, out zeroed");
    for (size_t i = 0; i < N_SCENES; i++) {
        const struct expect *e = &g_exp[i];
        int ret;

        set_slot(g_scenes[i].slot);
        g_hw = g_scenes[i].hw;
        g_sboot_done = g_scenes[i].sboot_done;
        g_key_src = 1;
        g_slot_reads = 0;
        g_sboot_reads = 0;
        ret = init_logged(log, sizeof(log));
        snprintf(what, sizeof(what), "%s, scene %zu: source %u, logs %x",
                 CFG_NAME, i + 1, e->src, e->logs);
        check(ret == (e->src == SRC_NONE ? -1 : 0) && runs_on(e->src) &&
              logs_of(log) == e->logs, what);
#if HUK_CFG == 4
        check(g_slot_reads == 0, "hardware key only: slot 4 never read");
#endif
#if HUK_CFG == 1 || HUK_CFG == 2
        check(g_sboot_reads == 0, "RAM path: tzc_sboot_done not read");
#endif
    }
    return check_summary();
}
