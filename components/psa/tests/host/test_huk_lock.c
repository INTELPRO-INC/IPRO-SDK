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
 * Host test of huk_lock: the tzc_sboot_done write, the zero-key proof
 * and the PRF refusal after it. Built with and without
 * CONFIG_PSA_HUK_LOCK (Makefile); each scene runs in a forked child.
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "check.h"
#include "drv_ef_ctrl.h"
#include "hal_sec_aes_v2.h"
#include "huk.h"
#include "kdf.h"
#include "spm_aes.h"

#define SBOOT_POS   28u
#define SBOOT_MSK   (0xFu << SBOOT_POS)
/* ROM_TZSRG_CTRL bits outside tzc_sboot_done */
#define OTHER_BITS  0x0A5C3E71u

/* engine behaviour of a scene */
enum {
    ENG_OK, ENG_STUCK, ENG_BIT3, ENG_NO_ZERO, ENG_ERR_LOCKED, ENG_BLANK,
    ENG_ERR,
};

struct scene {
    const char *what;
    uint32_t sboot0;    /* tzc_sboot_done at huk_init */
    uint8_t eng;
    int init_ret;       /* huk_init */
    int lock_ret;       /* huk_lock with CONFIG_PSA_HUK_LOCK */
    const char *line;   /* huk_lock log line, NULL for none */
};

static const uint8_t g_huk[16] = {
    0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
    0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c,
};
static uint32_t g_tzc;
static uint8_t g_eng;
static int g_writes;
static int g_hw_runs;
static uint8_t g_last_sel;

void EF_Ctrl_Read_AES_Key(uint8_t index, uint32_t *keyData, uint32_t len)
{
    (void)index;
    memset(keyData, 0, len * 4u);
}

uint32_t huk_hw_tzc_read(void)
{
    return g_tzc;
}

/* ENG_STUCK: the field keeps its value; ENG_BIT3: bit 31 stays 0 */
void huk_hw_tzc_write(uint32_t v)
{
    g_writes++;
    g_tzc = g_eng == ENG_STUCK ? (v & ~SBOOT_MSK) | (g_tzc & SBOOT_MSK)
            : g_eng == ENG_BIT3 ? v & ~(0x8u << SBOOT_POS) : v;
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
 * KEY_SEL 1 is g_huk while SE_AES_SBOOT and tzc_sboot_done are 0,
 * else the zero key (ENG_NO_ZERO: g_huk regardless of the field).
 */
int hal_sec_aes_v2_crypt(hal_sec_aes_v2_ctx_t *ctx, const uint8_t *in,
                         uint8_t *out, size_t len)
{
    static const uint8_t zero[16];
    int locked = (g_tzc & SBOOT_MSK) != 0;
    int live = !ctx->keylen && ctx->hw_sel == 1 && ctx->hw_src == 0 &&
               g_eng != ENG_BLANK && (!locked || g_eng == ENG_NO_ZERO);
    mbedtls_aes_context aes;

    g_hw_runs++;
    g_last_sel = ctx->hw_sel;
    if (len != 16 || g_eng == ENG_ERR ||
        (locked && g_eng == ENG_ERR_LOCKED)) {
        return -1;
    }
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, live ? g_huk : zero, 128);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, in, out);
    mbedtls_aes_free(&aes);
    return 0;
}

void hal_sec_aes_v2_abort(hal_sec_aes_v2_ctx_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

static int g_fail;

static void want(int cond, const char *what)
{
    if (!cond) {
        fprintf(stderr, "  child FAIL %s\n", what);
        g_fail++;
    }
}

/* fn() with stdout in log */
static int logged(int (*fn)(void), char *log, size_t n)
{
    FILE *tmp = tmpfile();
    int saved;
    int ret;
    size_t got;

    fflush(stdout);
    saved = dup(1);
    dup2(fileno(tmp), 1);
    ret = fn();
    fflush(stdout);
    dup2(saved, 1);
    close(saved);
    rewind(tmp);
    got = fread(log, 1, n - 1, tmp);
    log[got] = '\0';
    fclose(tmp);
    return ret;
}

static int count_of(const char *log, const char *s)
{
    int n = 0;

    for (const char *p = strstr(log, s); p != NULL; p = strstr(p + 1, s)) {
        n++;
    }
    return n;
}

static int prf_twice(void)
{
    uint8_t in[16] = "PRF-AFTER-LOCK";
    uint8_t out[16];
    uint8_t zero[16] = { 0 };
    int r1;
    int r2;

    memset(out, 0xA5, sizeof(out));
    r1 = huk_prf(in, out);
    want(memcmp(out, zero, 16) == 0, "refused huk_prf zeroes out");
    memset(out, 0xA5, sizeof(out));
    r2 = huk_prf(in, out);
    want(memcmp(out, zero, 16) == 0, "second refused huk_prf zeroes out");
    return r1 == -1 && r2 == -1 ? 0 : -1;
}

static int kdf_after(void)
{
    uint8_t out[16];
    uint8_t zero[16] = { 0 };

    memset(out, 0xA5, sizeof(out));
    return kdf_derive(KDF_LABEL_ITS, out, sizeof(out)) == -1 &&
           memcmp(out, zero, 16) == 0 ? 0 : -1;
}

/* the child of one scene: exit status = failed conditions */
static void run_scene(const struct scene *s)
{
    char log[512];
    uint8_t key[16];
    uint32_t before;
    int ret;
    int runs;

    g_eng = s->eng;
    g_tzc = OTHER_BITS | (s->sboot0 << SBOOT_POS);
    before = g_tzc;
    want(logged(huk_init, log, sizeof(log)) == s->init_ret, "huk_init");
    want(s->init_ret != 0 ||
         count_of(log, "development HUK") + count_of(log, "SEC_ENG") == 1,
         "huk_init: engine or blank-key development HUK");
    want(s->init_ret == 0 ||
         count_of(log, ", ITS and attestation disabled\r\n") == 1,
         "huk_init: HUK unavailable line");
    want(kdf_derive(KDF_LABEL_ITS, key, sizeof(key)) == s->init_ret,
         "kdf_derive before the lock");
    runs = g_hw_runs;
    ret = logged(huk_lock, log, sizeof(log));
#ifdef CONFIG_PSA_HUK_LOCK
    want(ret == s->lock_ret, "huk_lock return");
    want(s->line == NULL ? count_of(log, "[SPE] HUK") == 0
                         : count_of(log, s->line) == 1, "huk_lock log line");
    if (s->sboot0 == 0) {
        want(g_writes == 1 && (g_tzc & ~SBOOT_MSK) == OTHER_BITS,
             "one RMW write, other bits kept");
        want(g_hw_runs == runs + 1 && g_last_sel == 1,
             "one KEY_SEL 1 block after the write");
    } else {
        want(g_writes == 0 && g_tzc == before && g_hw_runs == runs,
             "field already set: no write, no engine block");
    }
#else
    want(ret == 1 && count_of(log, "[SPE] HUK") == 0, "lock off: 1, no log");
    want(g_writes == 0 && g_tzc == before && g_hw_runs == runs,
         "lock off: no write, no engine block");
#endif
    runs = g_hw_runs;
    want(logged(prf_twice, log, sizeof(log)) == 0, "huk_prf -1 twice");
    want(count_of(log, "[SPE] HUK locked: PRF refused\r\n") == 1,
         "refusal logged once");
    want(kdf_after() == 0, "kdf_derive -1 after the lock, out zeroed");
    want(huk_init() == -1 && prf_twice() == 0,
         "huk_init after the lock: -1, PRF still refused");
    want(g_hw_runs == runs, "no engine block after the lock");
    _exit(g_fail);
}

int main(void)
{
    static const struct scene scenes[] = {
        { "sboot_done 0, route closes", 0, ENG_OK, 0, 0,
          "[SPE] HUK locked (sboot_done 0xf)\r\n" },
        { "sboot_done 0, field write ignored", 0, ENG_STUCK, 0, -1,
          "[SPE] HUK route still open (sboot_done 0x0)\r\n" },
        { "sboot_done 0x7 read back, zero key", 0, ENG_BIT3, 0, -1,
          "[SPE] HUK route still open (sboot_done 0x7)\r\n" },
        { "sboot_done 0xf read back, key still live", 0, ENG_NO_ZERO, 0, -1,
          "[SPE] HUK route still open (sboot_done 0xf)\r\n" },
        { "sboot_done 0xf read back, engine error", 0, ENG_ERR_LOCKED, 0, -1,
          "[SPE] HUK route still open (sboot_done 0xf)\r\n" },
        { "sboot_done 0, blank key, development HUK", 0, ENG_BLANK, 0, 0,
          "[SPE] HUK locked (sboot_done 0xf)\r\n" },
        { "engine error at init: no HUK, lock", 0, ENG_ERR, -1, 0,
          "[SPE] HUK locked (sboot_done 0xf), SEC_ENG error\r\n" },
        { "sboot_done 0xf at init: no HUK, lock skipped", 0xF, ENG_OK, -1,
          1, NULL },
        { "sboot_done 0x2 at init: no HUK, lock skipped", 0x2, ENG_OK, -1,
          1, NULL },
    };
    uint8_t ct[16];
    static const uint8_t zero[16];
    static const uint8_t zero_ct[16] = {
        0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
        0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e,
    };
    mbedtls_aes_context ctx;

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, zero, 128);
    mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, zero, ct);
    mbedtls_aes_free(&ctx);
    check(memcmp(ct, zero_ct, 16) == 0,
          "AES-128 zero key, zero block = 66e94bd4..ca342b2e");
    for (size_t i = 0; i < sizeof(scenes) / sizeof(scenes[0]); i++) {
        char what[96];
        int status = -1;
        pid_t pid;

        fflush(stdout);
        pid = fork();
        if (pid == 0) {
            run_scene(&scenes[i]);
        }
        waitpid(pid, &status, 0);
        snprintf(what, sizeof(what), "%s: %s", LOCK_CFG, scenes[i].what);
        check(WIFEXITED(status) && WEXITSTATUS(status) == 0, what);
    }
    return check_summary();
}
