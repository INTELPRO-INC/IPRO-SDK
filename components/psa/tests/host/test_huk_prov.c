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
 * Host test of huk_prov.c over huk.c (KEY_SEL 1, HUK_LOCK) and the
 * stub/fake_*.c models. Each scene runs in a forked child; the boot B
 * scenes start from the eFuse array a DONE scene left behind.
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "check.h"
#include "fake_hukp.h"
#include "huk.h"
#include "huk_prov.h"
#include "spm_aes.h"
#include "spm_sha256.h"

#define W_CFG0          0u
#define W_CFG1          1u
#define W_SVN           19u
#define W_LOCK          31u
#define W_SLOT(n)       (7u + 4u * (n))
#define LOCK_WR(n)      (1u << (19u + (n)))
#define LOCK_RD(n)      (1u << (26u + (n)))
#define PLAN_LOCKS      0x40800000u
#define SBOOT_MSK       (0xFu << 28)
#define G1              0
#define G2              1
#define G3              2

static const uint8_t g_chip[8] = {
    0xc2, 0x74, 0x15, 0x00, 0x00, 0x2a, 0x11, 0x07,
};
/* slot 2 of the model part */
static const uint32_t g_slot2[4] = {
    0x0badf00du, 0x5eed1234u, 0x89abcdefu, 0x13579bdfu,
};

static char g_log[16384];
static char g_rx[160];
static FILE *g_state;
static int g_fail;

static void want(int cond, const char *what)
{
    if (!cond) {
        fprintf(stderr, "  child FAIL %s\n", what);
        g_fail++;
    }
}

/* the sec-verify chip at S8a: S1-S8 burned, slot 4 and 5 blank */
static void part_ready(void)
{
    memset(&g_ef, 0, sizeof(g_ef));
    memset(&g_misc, 0, sizeof(g_misc));
    g_ef.fuse[W_CFG0] = 0x000000B5u;
    g_ef.fuse[W_CFG1] = 0x00800000u;
    g_ef.fuse[2] = 0x1234abcdu;
    g_ef.fuse[5] = 0x001574c2u;
    g_ef.fuse[6] = 0x07112a00u;
    g_ef.fuse[20] = 0x00a50000u;
    g_ef.fuse[22] = 0x00000042u;
    for (uint32_t i = 0; i < 8; i++) {
        g_ef.fuse[W_SLOT(0) + i] = 0xd35f21bbu ^ (0x01010101u * (i + 1));
    }
    memcpy(&g_ef.fuse[W_SLOT(2)], g_slot2, sizeof(g_slot2));
    g_ef.fuse[W_LOCK] = 0x00180000u;
    g_ef.key_src = 1;
    fake_power_on();
    fake_trng_reset(1);
    g_misc.bclk_mhz = 96;
    memcpy(g_misc.chip, g_chip, 8);
    for (size_t i = 0; i < sizeof(g_misc.image); i++) {
        g_misc.image[i] = (uint8_t)(i * 7 + 3);
    }
}

static void hex(char *out, const uint8_t *b, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        snprintf(out + 2 * i, 3, "%02x", b[i]);
    }
}

static void le32(uint8_t b[4], uint32_t v)
{
    b[0] = (uint8_t)v;
    b[1] = (uint8_t)(v >> 8);
    b[2] = (uint8_t)(v >> 16);
    b[3] = (uint8_t)(v >> 24);
}

static uint8_t slot_flag(uint32_t slot)
{
    uint32_t any = 0;

    if ((g_ef.fuse[W_LOCK] & (LOCK_WR(slot) | LOCK_RD(slot))) != 0) {
        return 2;
    }
    for (uint32_t i = 0; i < 4; i++) {
        any |= g_ef.fuse[W_SLOT(slot) + i];
    }
    return any != 0;
}

/* the digest of the design, section 4, from the model state */
static void ref_digest(char out[17])
{
    mbedtls_sha256_context sha;
    uint8_t img[32];
    uint8_t b[32];

    mbedtls_sha256_ret(g_misc.image, sizeof(g_misc.image), img, 0);
    mbedtls_sha256_init(&sha);
    mbedtls_sha256_starts_ret(&sha, 0);
    mbedtls_sha256_update_ret(&sha, (const uint8_t *)"HUKPROV1", 8);
    mbedtls_sha256_update_ret(&sha, g_misc.chip, 8);
    for (uint32_t i = 0; i < 32; i++) {
        if (i == W_SLOT(2) || i == W_SLOT(4)) {
            b[0] = slot_flag(i == W_SLOT(2) ? 2 : 4);
            mbedtls_sha256_update_ret(&sha, b, 1);
        } else if ((i < W_SLOT(2) || i >= W_SLOT(2) + 4) &&
                   (i < W_SLOT(4) || i >= W_SLOT(4) + 4)) {
            le32(b, g_ef.fuse[i]);
            mbedtls_sha256_update_ret(&sha, b, 4);
        }
    }
    le32(b, W_SLOT(4));
    le32(b + 4, 4);
    le32(b + 8, PLAN_LOCKS);
    mbedtls_sha256_update_ret(&sha, b, 12);
    mbedtls_sha256_update_ret(&sha, img, 32);
    mbedtls_sha256_finish_ret(&sha, b);
    mbedtls_sha256_free(&sha);
    hex(out, b, 8);
}

/* g_rx = the confirmation the host tool sends for the model state */
static void confirm(void)
{
    char chip[17];
    char dig[17];

    hex(chip, g_misc.chip, 8);
    ref_digest(dig);
    snprintf(g_rx, sizeof(g_rx), "HUKPROV %s %s\r\n", chip, dig);
    g_misc.rx = g_rx;
}

static void kcv_of(const uint32_t key[4], char out[33])
{
    static const uint8_t zero[16];
    mbedtls_aes_context ctx;
    uint8_t ct[16];

    mbedtls_aes_init(&ctx);
    mbedtls_aes_setkey_enc(&ctx, (const uint8_t *)key, 128);
    mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, zero, ct);
    mbedtls_aes_free(&ctx);
    hex(out, ct, 16);
}

static enum hukp_result run(void)
{
    FILE *tmp = tmpfile();
    enum hukp_result r;
    int saved;
    size_t got;

    fflush(stdout);
    saved = dup(1);
    dup2(fileno(tmp), 1);
    r = huk_prov_run();
    fflush(stdout);
    dup2(saved, 1);
    close(saved);
    rewind(tmp);
    got = fread(g_log, 1, sizeof(g_log) - 1, tmp);
    g_log[got] = '\0';
    fclose(tmp);
    return r;
}

static int count(const char *s)
{
    int n = 0;

    for (const char *p = strstr(g_log, s); p != NULL; p = strstr(p + 1, s)) {
        n++;
    }
    return n;
}

static int has(const char *line)
{
    char l[160];

    snprintf(l, sizeof(l), "%s\r\n", line);
    return count(l) == 1;
}

/* the log ends with line a, then line b when b is not NULL */
static int ends_with(const char *a, const char *b)
{
    char tail[200];
    size_t n;
    size_t ln = strlen(g_log);

    snprintf(tail, sizeof(tail), "%s\r\n%s%s", a, b != NULL ? b : "",
             b != NULL ? "\r\n" : "");
    n = strlen(tail);
    return ln >= n && strcmp(g_log + ln - n, tail) == 0;
}

/* lines appear once each and in this order */
static int in_order(const char *const *lines, size_t n)
{
    const char *at = g_log;

    for (size_t i = 0; i < n; i++) {
        const char *p = strstr(at, lines[i]);

        if (p == NULL || count(lines[i]) != 1) {
            fprintf(stderr, "  missing or out of order: %s\n", lines[i]);
            return 0;
        }
        at = p + strlen(lines[i]);
    }
    return 1;
}

static const char *log_value(const char *tag, char *out, size_t n)
{
    const char *p = strstr(g_log, tag);
    size_t i = 0;

    out[0] = '\0';
    if (p == NULL) {
        return out;
    }
    for (p += strlen(tag); i + 1 < n && *p != ';' && *p != '\r' &&
         *p != ' ' && *p != '\0'; p++) {
        out[i++] = *p;
    }
    out[i] = '\0';
    return out;
}

static int find(const char *hay, size_t hn, const char *needle, size_t nn)
{
    for (size_t i = 0; i + nn <= hn; i++) {
        if (memcmp(hay + i, needle, nn) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * 1 when a 4-byte window of v (LE or BE words) is in the log, raw or
 * as hex; all-0 and all-1 windows are skipped
 */
static int leaks(const uint32_t *v, size_t words)
{
    size_t ln = strlen(g_log);
    uint8_t b[2][32];
    size_t n = words * 4;

    for (size_t i = 0; i < words; i++) {
        le32(&b[0][4 * i], v[i]);
        for (size_t k = 0; k < 4; k++) {
            b[1][4 * i + k] = b[0][4 * i + 3 - k];
        }
    }
    for (size_t o = 0; o < 2; o++) {
        for (size_t i = 0; i + 4 <= n; i++) {
            char h[9];
            char u[9];
            uint32_t w;

            memcpy(&w, &b[o][i], 4);
            if (w == 0 || w == 0xFFFFFFFFu) {
                continue;
            }
            hex(h, &b[o][i], 4);
            for (size_t k = 0; k < 8; k++) {
                u[k] = (char)(h[k] >= 'a' ? h[k] - 32 : h[k]);
            }
            u[8] = '\0';
            if (find(g_log, ln, (const char *)&b[o][i], 4) ||
                strstr(g_log, h) != NULL || strstr(g_log, u) != NULL) {
                return 1;
            }
        }
    }
    return 0;
}

static int state_wiped(void)
{
    size_t n = 0;
    const uint8_t *p = huk_prov_state(&n);
    uint8_t any = 0;

    for (size_t i = 0; i < n; i++) {
        any |= p[i];
    }
    return any == 0;
}

static int shadow_slot4_zero(void)
{
    uint32_t any = 0;

    for (uint32_t i = 0; i < 4; i++) {
        any |= g_ef.shadow[W_SLOT(4) + i];
    }
    return any == 0;
}

/* no secret on the console, every buffer and shadow slot 4 wiped */
static void want_no_export(void)
{
    uint32_t blank = 0;

    for (uint32_t i = 0; i < 4; i++) {
        blank |= g_ef.fuse[W_SLOT(4) + i];
    }
    want(!leaks(g_trng.out[G1], 8), "console has no G1 window");
    want(!leaks(g_trng.out[G2], 8), "console has no G2 window");
    want(!leaks(g_trng.out[G3], 8), "console has no G3 window");
    want(!leaks(g_slot2, 4), "console has no slot 2 window");
    want(state_wiped(), "module state wiped");
    want(shadow_slot4_zero() || (blank != 0 && g_ef.programs == 0),
         "shadow slot 4 zero (a set slot the run found is left)");
}

static int scene(const char *what, void (*fn)(void))
{
    int status = -1;
    pid_t pid;

    fflush(stdout);
    pid = fork();
    if (pid == 0) {
        g_fail = 0;
        fn();
        _exit(g_fail);
    }
    waitpid(pid, &status, 0);
    check(WIFEXITED(status) && WEXITSTATUS(status) == 0, what);
    return status;
}

/* 1 timeout: the read-only check, nothing burned */
static void s_check_only(void)
{
    static const char *const lines[] = {
        "[HUKP] chip c2741500002a1107 image ",
        "[HUKP] efuse 00=000000b5 04=00800000 4c=00000000 7c=00180000 "
        "bit17=0",
        "[HUKP] state tzc_sboot_done 0x0 bclk 96 MHz rosc_c1 0x0f000 "
        "trng_test 0x000 ctrl_3 0x037a4203",
        "[HUKP] slot4 blank unlocked, slot5 blank, slot2 set readable",
        "[HUKP] trng rosc_c1=0x0f00 locked, rct=26 apt=655 cp=3 "
        "alarm_n=0, bclk 96 MHz",
        "[HUKP] plan: slot4 <- 128 TRNG bits; 7c |= 0x40800000 "
        "(bits 23, 30)",
        "[HUKP] refused: no confirmation",
        "[HUKP] tzc_sboot_done 0x0 -> 0xf",
    };
    char dig[17];
    char got[40];

    part_ready();
    ref_digest(dig);
    want(run() == HUKP_REFUSED, "refused");
    want(in_order(lines, sizeof(lines) / sizeof(lines[0])), "check lines");
    want(strcmp(log_value("[HUKP] digest ", got, sizeof(got)), dig) == 0,
         "printed digest = design digest");
    want(g_ef.programs == 0 && g_trng.gens == 0 &&
         g_trng.instantiates == 0, "no program, no draw");
    want(g_misc.ms >= 60000, "waited 60 s");
    want((g_ef.tzc & SBOOT_MSK) == SBOOT_MSK, "route closed at the end");
    want(g_trng.rosc_c1 == 0x0F00 && g_trng.locked &&
         (g_trng.ctrl3 & 0x03FFFFFFu) == (3u | 26u << 8 | 655u << 16),
         "rosc_c1 0x0f00, se_dbg_dis, cutoffs 26/655/3");
    want(memcmp(g_trng.out[G1], g_trng.dout, 32) != 0, "no generation");
    want_no_export();
}

/* 1 confirmation: value and locks burned, DONE */
static void s_done(void)
{
    static const char *head[] = {
        "[HUKP] trng ok: 2 instantiates, ht_error 0, sentinel 0",
        "[HUKP] slot4 programmed, readback match",
    };
    static const char *tail[] = {
        "[SPE] HUK via SEC_ENG KEY_SEL 1",
        "[HUKP] engine ok",
        "[HUKP] 7c 00180000 -> 40980000, other words unchanged",
        "[HUKP] slot4 cpu read zero",
        "[SPE] HUK locked (sboot_done 0xf)",
        "[HUKP] DONE, reset the part",
    };
    uint32_t before[32];
    uint32_t other = 0;
    char kcv[33];
    char line[64];

    part_ready();
    confirm();
    memcpy(before, g_ef.fuse, sizeof(before));
    kcv_of(g_trng.out[G2], kcv);
    snprintf(line, sizeof(line), "[HUKP] kcv %s", kcv);
    want(run() == HUKP_DONE, "DONE");
    want(in_order(head, 2) && has(line) && in_order(tail, 6),
         "result block");
    want(strstr(g_log, line) > strstr(g_log, head[1]) &&
         strstr(g_log, line) < strstr(g_log, tail[1]), "kcv line place");
    want(memcmp(&g_ef.fuse[W_SLOT(4)], g_trng.out[G2], 16) == 0,
         "slot 4 = G2 words 0-3");
    want(g_ef.fuse[W_LOCK] == (before[W_LOCK] | PLAN_LOCKS) &&
         (g_ef.fuse[W_LOCK] & (1u << 13)) == 0, "7c bits 23 + 30 only");
    for (uint32_t i = 0; i < W_LOCK; i++) {
        if (i < W_SLOT(4) || i >= W_SLOT(4) + 4) {
            other |= g_ef.fuse[i] ^ before[i];
        }
    }
    want(other == 0, "every other word unchanged");
    want(g_ef.programs == 2, "two programs");
    want(g_trng.instantiates == 2 && g_trng.gens == 3,
         "2 instantiates, G1 G2 G3");
    want(g_misc.irq_off, "interrupts off for the draw");
    want((g_ef.tzc & SBOOT_MSK) == SBOOT_MSK, "route closed");
    want(memcmp(g_trng.dout, g_trng.out[G2], 32) != 0, "dout != G2");
    want_no_export();
    if (g_state != NULL) {
        fwrite(g_ef.fuse, sizeof(g_ef.fuse), 1, g_state);
        fflush(g_state);
    }
}

/* boot B: TRNG set-up and lock, engine KCV, route closed */
static const char *const g_boot_b_lines[] = {
    "[HUKP] trng set-up ok: rosc_c1 0x00f00 se_dbg_dis 1 ctrl_3 0x028f1a03 alarm_n 0",
    "[SPE] HUK via SEC_ENG KEY_SEL 1",
    "[SPE] HUK locked (sboot_done 0xf)",
};

/* 8 boot B on the eFuse array of the last DONE scene */
static void s_boot_b(void)
{
    char kcv[33];
    char line[96];

    part_ready();
    rewind(g_state);
    want(fread(g_ef.fuse, sizeof(g_ef.fuse), 1, g_state) == 1,
         "DONE state read");
    fake_power_on();
    want(slot_flag(4) == 2, "slot 4 locked");
    kcv_of(&g_ef.fuse[W_SLOT(4)], kcv);
    snprintf(line, sizeof(line),
             "[HUKP] refused: slot4 provisioned, engine kcv %s", kcv);
    want(run() == HUKP_REFUSED && has(line), "refused, same KCV");
    want(in_order(g_boot_b_lines, 3) && ends_with(line, NULL),
         "TRNG locked before the engine KCV; refusal last");
    want(has("[SPE] HUK locked (sboot_done 0xf)"), "route closed");
    want(count("digest") == 0 && g_ef.programs == 0 && g_trng.gens == 0,
         "no digest, no program, no draw");
    want(!leaks(&g_ef.fuse[W_SLOT(4)], 4), "no slot 4 window");
    want(state_wiped(), "module state wiped");
}

/* the part #2 state: slots 2 + 4 locked, counter 2 */
static void part2(void)
{
    static const uint32_t key[4] = {
        0x16157e2bu, 0xa6d2ae28u, 0x8815f7abu, 0x3c4fcf09u,
    };

    part_ready();
    memcpy(&g_ef.fuse[W_SLOT(4)], key, sizeof(key));
    g_ef.fuse[W_SVN] = 0x3;
    g_ef.fuse[W_LOCK] = 0x50B80000u;
    fake_power_on();
}

static void s_part2(void)
{
    char kcv[33];
    char line[96];

    part2();
    kcv_of(&g_ef.fuse[W_SLOT(4)], kcv);
    snprintf(line, sizeof(line),
             "[HUKP] refused: slot4 provisioned, engine kcv %s", kcv);
    want(run() == HUKP_REFUSED && has(line), "part #2: refused, KCV");
    want(in_order(g_boot_b_lines, 3) && ends_with(line, NULL),
         "TRNG locked before the engine KCV; refusal last");
    want(g_trng.locked && g_trng.rosc_c1 == 0x0F00, "TRNG locked");
    want(has("[HUKP] efuse 00=000000b5 04=00800000 4c=00000003 "
             "7c=50b80000 bit17=0"), "part #2 efuse line");
    want(g_ef.programs == 0 && g_trng.gens == 0, "nothing burned");
    want(!leaks(&g_ef.fuse[W_SLOT(4)], 4), "no slot 4 window");
}

static void s_part2_closed(void)
{
    part2();
    g_ef.tzc = SBOOT_MSK;
    want(run() == HUKP_REFUSED &&
         has("[HUKP] refused: slot4 provisioned, engine kcv unavailable"),
         "route closed: no KCV");
    want(g_ef.programs == 0, "nothing burned");
}

/* E2: hardware key from the power-on autoload, lock acts at once */
static void s_engine_latch(void)
{
    part_ready();
    confirm();
    g_ef.engine = FAKE_ENG_LATCH;
    want(run() == HUKP_DONE, "DONE");
    want(has("[HUKP] engine pending reset") &&
         has("[HUKP] slot4 cpu read zero") &&
         has("[SPE] HUK locked (sboot_done 0xf)"), "pending reset lines");
    want(g_ef.programs == 2 && g_ef.fuse[W_LOCK] == 0x40980000u,
         "locks still programmed");
    want_no_export();
    if (g_state != NULL) {
        fwrite(g_ef.fuse, sizeof(g_ef.fuse), 1, g_state);
        fflush(g_state);
    }
}

/* E2: read lock acts after a reset only */
static void s_rdlock_late(void)
{
    part_ready();
    confirm();
    g_ef.rdlock_at_reset = 1;
    want(run() == HUKP_DONE, "DONE");
    want(has("[HUKP] engine ok") &&
         has("[HUKP] slot4 cpu read pending reset"), "pending reset line");
    want_no_export();
}

/* blank lines before the confirmation are skipped */
static void s_blank_lines(void)
{
    static char rx[200];

    part_ready();
    confirm();
    snprintf(rx, sizeof(rx), "\r\n\n\r%s", g_rx);
    g_misc.rx = rx;
    want(run() == HUKP_DONE, "DONE after blank lines");
}

/* 4 weight 32 and 96 accepted */
static void s_weight_32(void)
{
    static const uint32_t v[4] = { 0xFFFFFFFFu, 0, 0, 0 };

    part_ready();
    confirm();
    memcpy(g_trng.out[G2], v, sizeof(v));
    want(run() == HUKP_DONE, "weight 32: DONE");
}

static void s_weight_96(void)
{
    static const uint32_t v[4] = { 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0 };

    part_ready();
    confirm();
    memcpy(g_trng.out[G2], v, sizeof(v));
    want(run() == HUKP_DONE, "weight 96: DONE");
}

/* slot 2 read-locked: plan names it, R16 cannot compare */
static void s_slot2_locked(void)
{
    part_ready();
    g_ef.fuse[W_LOCK] |= LOCK_WR(2) | LOCK_RD(2);
    fake_power_on();
    confirm();
    want(run() == HUKP_DONE && has("[HUKP] slot4 blank unlocked, slot5 "
                                   "blank, slot2 locked"), "DONE");
}

/* 2 digest: state only, not the TRNG; slot values out of it */
static void s_digest(void)
{
    char a[40];
    char b[40];
    char r[17];

    part_ready();
    run();
    log_value("[HUKP] digest ", a, sizeof(a));
    part_ready();
    fake_trng_reset(77);
    run();
    log_value("[HUKP] digest ", b, sizeof(b));
    ref_digest(r);
    want(strlen(a) == 16 && strcmp(a, b) == 0 && strcmp(a, r) == 0,
         "same state, other TRNG script: same digest");

    part_ready();
    g_ef.fuse[2] ^= 0x100;
    fake_power_on();
    run();
    ref_digest(r);
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), r) == 0 &&
         strcmp(a, b) != 0, "word 0x08 changes the digest");

    part_ready();
    g_misc.image[9] ^= 1;
    run();
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), a) != 0,
         "image changes the digest");

    part_ready();
    g_misc.chip[0] ^= 1;
    run();
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), a) != 0,
         "chip id changes the digest");

    part_ready();
    g_ef.fuse[W_SLOT(2) + 1] ^= 0x8000;
    fake_power_on();
    run();
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), a) == 0,
         "slot 2 value is not in the digest");

    part_ready();
    memset(&g_ef.fuse[W_SLOT(2)], 0, 16);
    fake_power_on();
    run();
    ref_digest(r);
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), r) == 0 &&
         strcmp(a, b) != 0 && has("[HUKP] slot4 blank unlocked, slot5 "
                                  "blank, slot2 blank"),
         "slot 2 flag blank changes the digest");

    part_ready();
    g_ef.fuse[W_LOCK] |= LOCK_RD(2);
    fake_power_on();
    run();
    ref_digest(r);
    want(strcmp(log_value("[HUKP] digest ", b, sizeof(b)), r) == 0 &&
         strcmp(a, b) != 0, "slot 2 flag locked changes the digest");
}

/* one refusal: setup, line, digest printed or not, draw or not */
struct refusal {
    const char *what;
    const char *why;
    void (*mod)(void);
    int plan;           /* 1: digest printed (confirmation asked) */
    int draw;           /* 1: refused after the draw */
};

static const struct refusal *g_ref;

static void m_not_ready(void) { g_ef.not_ready = 1; }
static void m_slot4(void) { g_ef.fuse[W_SLOT(4) + 2] = 0x10; fake_power_on(); }
static void m_slot5(void) { g_ef.fuse[W_SLOT(5)] = 1; fake_power_on(); }
static void m_slot5_rd(void)
{
    g_ef.fuse[W_LOCK] |= LOCK_RD(5);
    fake_power_on();
}
static void m_sign0(void) { g_ef.fuse[W_CFG0] = 0xB1; fake_power_on(); }
static void m_aes2(void) { g_ef.fuse[W_CFG0] = 0xB6; fake_power_on(); }
static void m_cpu0(void) { g_ef.fuse[W_CFG0] = 0x35; fake_power_on(); }
static void m_bit17(void) { g_ef.fuse[W_CFG0] = 0x200B5; fake_power_on(); }
static void m_keepdbg(void) { g_ef.fuse[W_CFG1] = 0; fake_power_on(); }
static void m_uartdis(void)
{
    g_ef.fuse[W_CFG1] = 0x02800000u;
    fake_power_on();
}
static void m_tzc(void) { g_ef.tzc = 0x2u << 28; }
static void m_svn(void) { g_ef.fuse[W_SVN] = 1; fake_power_on(); }
static void m_lifecycle(void) { g_ef.fuse[W_LOCK] |= 1; fake_power_on(); }
static void m_bclk(void) { g_misc.bclk_mhz = 80; }
static void m_efuse_lock(void) { g_trng.efuse_locked = 1; }
static void m_bypass(void) { g_trng.test = 0x4; }
static void m_test_en(void) { g_trng.test = 0x1; }
static void m_lock_ignored(void) { g_trng.lock_ignored = 1; }
static void m_ctrl3_ro(void) { g_trng.ctrl3_ro = 1; }
static void m_ht_dis(void) { g_trng.test = 0x8; }
static void m_timeout(void) { g_misc.rx = NULL; }
static void m_no_eol(void) { g_rx[strlen(g_rx) - 2] = '\0'; }
static void m_wrong_chip(void) { g_rx[8] = g_rx[8] == '0' ? '1' : '0'; }
static void m_wrong_dig(void) { g_rx[30] = g_rx[30] == '0' ? '1' : '0'; }
static void m_lower(void) { memcpy(g_rx, "hukprov", 7); }
static void m_noise(void)
{
    static char rx[200];

    snprintf(rx, sizeof(rx), "hello\r\n%s", g_rx);
    g_misc.rx = rx;
}
static void m_long(void)
{
    static char rx[200];

    snprintf(rx, sizeof(rx), "%.*s%s", 50,
             "HUKPROVHUKPROVHUKPROVHUKPROVHUKPROVHUKPROVHUKPROVHUKPROV",
             g_rx);
    g_misc.rx = rx;
}
static void m_prefix(void)
{
    size_t n = strlen(g_rx);

    memmove(&g_rx[n - 3], &g_rx[n - 2], 3);
}
static void m_prefix_chip(void) { strcpy(&g_rx[24], "\r\n"); }
static void m_chip_bad(void) { g_misc.chip_bad = 1; }
static void m_chip_zero(void) { memset(g_misc.chip, 0, 8); }
static void m_ht_g1(void) { g_trng.ht_error[G1] = 1; }
static void m_ht_g2(void) { g_trng.ht_error[G2] = 1; }
static void m_sent_g1(void) { g_trng.out[G1][3] = 0xDEADBEEFu; }
static void m_sent_g2(void) { g_trng.out[G2][6] = 0xDEADBEEFu; }
static void m_stuck(void) { g_trng.out[G2][5] = g_trng.out[G1][5]; }
static void m_same(void) { memcpy(g_trng.out[G2], g_trng.out[G1], 32); }
static void m_zero(void) { memset(g_trng.out[G2], 0, 16); }
static void m_ones(void) { memset(g_trng.out[G2], 0xFF, 16); }
static void m_w31(void)
{
    static const uint32_t v[4] = { 0x7FFFFFFFu, 0, 0, 0 };

    memcpy(g_trng.out[G2], v, 16);
}
static void m_w97(void)
{
    static const uint32_t v[4] = {
        0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0x1u,
    };

    memcpy(g_trng.out[G2], v, 16);
}
static void m_eq_slot2(void) { memcpy(g_trng.out[G2], g_slot2, 16); }
static void m_en_timeout(void) { g_trng.en_timeout = 1; }
static void m_g3_fail(void) { g_trng.fail_gen[G3] = 1; }
static void poke(int n)
{
    if (n == G2) {
        g_ef.fuse[2] ^= 1;
    }
}
static void m_changed(void) { g_trng.on_gen = poke; }

static const struct refusal g_refusals[] = {
    { "R2 eFuse not ready", "efuse not ready", m_not_ready, 0, 0 },
    { "R3 slot 4 set, unlocked", "slot4 not blank", m_slot4, 0, 0 },
    { "R4 slot 5 set", "slot5 not blank", m_slot5, 0, 0 },
    { "R4 slot 5 read lock", "slot5 not blank", m_slot5_rd, 0, 0 },
    { "R5 sign_mode 0", "ef_cfg_0", m_sign0, 0, 0 },
    { "R5 sf_aes_mode 2", "ef_cfg_0", m_aes2, 0, 0 },
    { "R5 cpu0_enc_en 0", "ef_cfg_0", m_cpu0, 0, 0 },
    { "R5 bit 17", "ef_cfg_0", m_bit17, 0, 0 },
    { "R6 keep_dbg_port_closed 0", "word 0x04", m_keepdbg, 0, 0 },
    { "R6 uartboot_disable 1", "word 0x04", m_uartdis, 0, 0 },
    { "R7 tzc_sboot_done 2", "tzc_sboot_done", m_tzc, 0, 0 },
    { "R8 counter 1", "counter or lifecycle", m_svn, 0, 0 },
    { "R8 lifecycle 1", "counter or lifecycle", m_lifecycle, 0, 0 },
    { "R14 bclk 80 MHz", "bclk", m_bclk, 0, 0 },
    { "R10 eFuse se_dbg_dis", "trng source", m_efuse_lock, 0, 0 },
    { "R11 cp_bypass", "trng test register", m_bypass, 0, 0 },
    { "R11 test_en", "trng test register", m_test_en, 0, 0 },
    { "R12 software lock ignored", "trng lock", m_lock_ignored, 0, 0 },
    { "R13 ctrl_3 read-only", "trng cutoffs", m_ctrl3_ro, 0, 0 },
    { "R13 ht_dis", "trng cutoffs", m_ht_dis, 0, 0 },
    { "R15 timeout", "no confirmation", m_timeout, 1, 0 },
    { "R15 no line end", "no confirmation", m_no_eol, 1, 0 },
    { "R15 wrong chip id", "no confirmation", m_wrong_chip, 1, 0 },
    { "R15 wrong digest", "no confirmation", m_wrong_dig, 1, 0 },
    { "R15 lower case", "no confirmation", m_lower, 1, 0 },
    { "R15 other line first", "no confirmation", m_noise, 1, 0 },
    { "R15 line = expected minus its last char", "no confirmation",
      m_prefix, 1, 0 },
    { "R15 line = HUKPROV <chip> only", "no confirmation", m_prefix_chip,
      1, 0 },
    { "chip id parity error", "chip id", m_chip_bad, 0, 0 },
    { "chip id all zero", "chip id", m_chip_zero, 0, 0 },
    { "R15 overlong line", "no confirmation", m_long, 1, 0 },
    { "R18 ht_error after G1", "trng health test", m_ht_g1, 1, 1 },
    { "R18 ht_error after G2", "trng health test", m_ht_g2, 1, 1 },
    { "R18 sentinel in G1", "trng health test", m_sent_g1, 1, 1 },
    { "R18 sentinel in G2", "trng health test", m_sent_g2, 1, 1 },
    { "R19 one word G1 == G2", "trng stuck", m_stuck, 1, 1 },
    { "R19 G1 == G2", "trng stuck", m_same, 1, 1 },
    { "R16 all zero", "trng value rejected", m_zero, 1, 1 },
    { "R16 all ones", "trng value rejected", m_ones, 1, 1 },
    { "R16 HUK == slot 2", "trng value rejected", m_eq_slot2, 1, 1 },
    { "R17 weight 31", "trng value rejected", m_w31, 1, 1 },
    { "R17 weight 97", "trng value rejected", m_w97, 1, 1 },
    { "instantiate timeout", "trng error", m_en_timeout, 1, 0 },
    { "G3 failure", "trng error", m_g3_fail, 1, 1 },
    { "eFuse changed during the draw", "efuse changed", m_changed, 1, 1 },
};

static void s_refusal(void)
{
    char line[96];

    part_ready();
    confirm();
    g_ref->mod();
    snprintf(line, sizeof(line), "[HUKP] refused: %s", g_ref->why);
    want(run() == HUKP_REFUSED, "refused");
    want(has(line), "refusal line names the check");
    want(count("[HUKP] refused") == 1, "one refusal line");
    want(ends_with(line, g_ref->mod == m_tzc ? NULL : "[HUKP] tzc_sboot_done 0x0 -> 0xf"),
         "refusal, then the route close line, end the log");
    want((g_ef.tzc & SBOOT_MSK) != 0, "route closed at the end");
    want(g_ef.programs == 0, "program count 0");
    want((count("[HUKP] digest ") == 1) == g_ref->plan, "digest printed");
    want((g_trng.gens > 0) == g_ref->draw, "draw as expected");
    want(count("[SPE] HUK") == 0, "no HUK engine use");
    want_no_export();
}

/* 5, 6 burn faults and engine results after the confirmation */
struct fault {
    const char *what;
    void (*mod)(void);
    enum hukp_result res;
    const char *line;
    int programs;
    int locked;         /* huk_lock closed the route */
    const char *route;  /* last line after the FAIL line, or NULL */
};

static const struct fault *g_fault;

static uint32_t low_bit(uint32_t v, int set)
{
    for (uint32_t b = 0; b < 32; b++) {
        if (((v >> b) & 1u) == (uint32_t)set) {
            return 1u << b;
        }
    }
    return 0;
}

static void f_missing(void)
{
    g_ef.drop[W_SLOT(4) + 1] = low_bit(g_trng.out[G2][1], 1);
}
static void f_extra(void)
{
    g_ef.add[W_SLOT(4) + 2] = low_bit(g_trng.out[G2][2], 0);
}
static void f_both(void)
{
    f_missing();
    f_extra();
}
static void f_lock(void) { g_ef.drop[W_LOCK] = LOCK_RD(4); }
static void f_other(void) { g_ef.add[2] = low_bit(g_ef.fuse[2], 0); }
static void f_third(void) { g_ef.engine = FAKE_ENG_THIRD; }
static void f_lost(void) { g_ef.engine = FAKE_ENG_LOST; }
static void f_stuck(void) { g_ef.tzc_stuck = 1; }
static void f_bit13(void) { g_ef.add[W_LOCK] = 1u << 13; }
static void f_slot5(void) { g_ef.add[W_SLOT(5)] = 0x10; }
static void f_slot5_last(void) { g_ef.add[W_SLOT(5) + 3] = 0x80000000u; }

static const struct fault g_faults[] = {
    { "F bits missing", f_missing, HUKP_FAIL, "[HUKP] FAIL: bits missing",
      1, 0, "[HUKP] tzc_sboot_done 0x0 -> 0xf" },
    { "F bits extra", f_extra, HUKP_FAIL, "[HUKP] FAIL: bits extra", 1, 0,
      "[HUKP] tzc_sboot_done 0x0 -> 0xf" },
    { "F bits missing and extra", f_both, HUKP_FAIL,
      "[HUKP] FAIL: bits missing and extra", 1, 0, "[HUKP] tzc_sboot_done 0x0 -> 0xf" },
    { "G.4 lock word wrong", f_lock, HUKP_FAIL, "[HUKP] FAIL: lock word",
      2, 1, NULL },
    { "G.4 extra 0x7C bit 13", f_bit13, HUKP_FAIL, "[HUKP] FAIL: lock word",
      2, 1, NULL },
    { "G.4 another word changes", f_other, HUKP_FAIL,
      "[HUKP] FAIL: other word changed", 2, 1, NULL },
    { "G.4 slot 5 word 0 changes", f_slot5, HUKP_FAIL,
      "[HUKP] FAIL: other word changed", 2, 1, NULL },
    { "G.4 slot 5 word 3 changes", f_slot5_last, HUKP_FAIL,
      "[HUKP] FAIL: other word changed", 2, 1, NULL },
    { "G.2 engine third value", f_third, HUKP_FAIL,
      "[HUKP] FAIL: engine third value", 1, 1, NULL },
    { "G.5 engine lost after locks", f_lost, HUKP_FAIL,
      "[HUKP] FAIL: engine after locks", 2, 1, NULL },
    { "G.6 route stays open", f_stuck, HUKP_FAIL, "[HUKP] FAIL: route open",
      2, 0, "[HUKP] tzc_sboot_done 0x0 -> 0x0" },
};

static void s_fault(void)
{
    uint32_t lock0;

    part_ready();
    confirm();
    lock0 = g_ef.fuse[W_LOCK];
    g_fault->mod();
    want(run() == g_fault->res, "result");
    want(has(g_fault->line), "FAIL line");
    want(count("[HUKP] DONE") == 0, "no DONE");
    want(g_ef.programs == g_fault->programs, "program count");
    want(g_fault->programs != 1 || g_ef.fuse[W_LOCK] == lock0,
         "no lock program");
    want(count("[SPE] HUK locked (sboot_done 0xf)") == g_fault->locked,
         "route close line");
    want(ends_with(g_fault->line, g_fault->route),
         "FAIL line, then the route line if huk_lock did not run");
    want_no_export();
}

/* R3: one of the two slot 4 lock bits is enough for boot B */
static void lockbit(uint32_t bit, int set)
{
    static const uint32_t key[4] = {
        0x16157e2bu, 0xa6d2ae28u, 0x8815f7abu, 0x3c4fcf09u,
    };
    char kcv[33] = "unavailable";
    char line[96];

    part_ready();
    if (set) {
        memcpy(&g_ef.fuse[W_SLOT(4)], key, sizeof(key));
        kcv_of(key, kcv);
    }
    g_ef.fuse[W_LOCK] |= bit;
    fake_power_on();
    confirm();
    snprintf(line, sizeof(line),
             "[HUKP] refused: slot4 provisioned, engine kcv %s", kcv);
    want(run() == HUKP_REFUSED && has(line), "boot B line");
    want(count("digest") == 0 && g_ef.programs == 0 && g_trng.gens == 0,
         "no digest, no program, no draw");
}

static void s_wr_only_blank(void) { lockbit(LOCK_WR(4), 0); }
static void s_rd_only_blank(void) { lockbit(LOCK_RD(4), 0); }
static void s_wr_only_set(void) { lockbit(LOCK_WR(4), 1); }

/* part #2 with a chip id parity error: boot B comes first */
static void s_part2_chip_bad(void)
{
    part2();
    g_misc.chip_bad = 1;
    want(run() == HUKP_REFUSED && count("slot4 provisioned, engine kcv "
                                        "unavailable") == 0 &&
         count("slot4 provisioned, engine kcv ") == 1, "boot B, KCV");
}

/* I1: the controller stays busy after program n */
static void busy(int n)
{
    char line[64];

    part_ready();
    confirm();
    g_ef.busy_at = n;
    want(run() == HUKP_FAIL && has("[HUKP] FAIL: program timeout"),
         "FAIL: program timeout");
    want(g_ef.programs == n && g_ef.ctrl_after_busy == 0,
         "no load, set or program after the busy program");
    want(count("[HUKP] DONE") == 0 && count("readback match") == n - 1,
         "stops at the busy program");
    snprintf(line, sizeof(line), "%s", n == 1 ?
             "[HUKP] tzc_sboot_done 0x0 -> 0xf" :
             "[SPE] HUK locked (sboot_done 0xf)");
    want(has(line) && (g_ef.tzc & SBOOT_MSK) == SBOOT_MSK, "route closed");
    want(!leaks(g_trng.out[G2], 8) && !leaks(g_trng.out[G1], 8),
         "console has no G1 / G2 window");
    want(state_wiped(), "module state wiped");
}

static void s_busy_value(void) { busy(1); }
static void s_busy_locks(void) { busy(2); }

#define N_OF(a) (sizeof(a) / sizeof((a)[0]))

int main(void)
{
    char what[96];

    scene("1 check only: plan, digest, timeout, nothing burned",
          s_check_only);
    g_state = tmpfile();
    scene("1 confirmed: slot 4 + 0x7C bits 23/30, KCV, DONE", s_done);
    scene("8 boot B after DONE: refused, same KCV", s_boot_b);
    rewind(g_state);
    scene("6 engine latched at autoload: pending reset, DONE",
          s_engine_latch);
    scene("8 boot B after pending reset: refused, KCV", s_boot_b);
    fclose(g_state);
    g_state = NULL;
    scene("8 part #2 state: refused, engine KCV", s_part2);
    scene("8 part #2 state, route closed: KCV unavailable", s_part2_closed);
    scene("6 read lock after reset: cpu read pending reset", s_rdlock_late);
    scene("3 R3 write lock only, slot 4 blank: boot B", s_wr_only_blank);
    scene("3 R3 read lock only, slot 4 blank: boot B", s_rd_only_blank);
    scene("3 R3 write lock only, slot 4 set: boot B, KCV", s_wr_only_set);
    scene("8 part #2 state, chip id parity error: boot B first",
          s_part2_chip_bad);
    scene("I1 busy after the slot 4 program: FAIL, no reload",
          s_busy_value);
    scene("I1 busy after the lock program: FAIL, no reload", s_busy_locks);
    scene("15 blank lines before the confirmation", s_blank_lines);
    scene("4 weight 32 accepted", s_weight_32);
    scene("4 weight 96 accepted", s_weight_96);
    scene("16 slot 2 read-locked: plan line, DONE", s_slot2_locked);
    scene("2 digest over state, image, chip; slot values out",
          s_digest);
    for (size_t i = 0; i < N_OF(g_refusals); i++) {
        g_ref = &g_refusals[i];
        snprintf(what, sizeof(what), "3/4 refusal %s", g_ref->what);
        scene(what, s_refusal);
    }
    for (size_t i = 0; i < N_OF(g_faults); i++) {
        g_fault = &g_faults[i];
        snprintf(what, sizeof(what), "5/6 %s", g_fault->what);
        scene(what, s_fault);
    }
    return check_summary();
}
