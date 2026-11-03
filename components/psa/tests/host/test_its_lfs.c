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
 * Host test of its_lfs.c and svc_its_init on littlefs over lfs_ram.c:
 * stale or foreign littlefs content in the PSM. Each SPE boot runs in
 * a forked child, the flash is shared.
 */
#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "check.h"
#include "its_store.h"
#include "kdf.h"
#include "lfs_port.h"
#include "lfs_ram.h"
#include "psa/crypto.h"
#include "svc_its.h"

#define FLASH_BLOCKS 96u
#define PSM_BLOCKS   32u
/* 1 MB OTA table PSM 0xC4000; the 1M_boot2 PSM 0xDA000 = block 22 */
#define OTA_PSM      0u
#define BOOT2_PSM    22u
#define UID          0x4011u

/* one SPE boot: svc_its_init, then the op, and the console lines */
struct boot {
    int init;
    int op;
    char log[4096];
};

/* 1: kdf_derive fails, as with no HUK */
static int g_no_huk;
/* 1: the partition table check refuses the PSM */
static int g_bad_layout;

int spm_psm_layout_ok(void)
{
    if (g_bad_layout) {
        printf("[SPE] PSM layout 000da000+20000 img 00015000: "
               "overlaps an FW entry\r\n");
        return -1;
    }
    return 0;
}

int kdf_derive(const char *label, uint8_t *out, size_t out_len)
{
    (void)label;
    memset(out, g_no_huk ? 0 : 0x5c, out_len);
    return g_no_huk ? -1 : 0;
}

psa_status_t psa_generate_random(uint8_t *output, size_t output_size)
{
    static uint8_t n;

    memset(output, ++n, output_size);
    return PSA_SUCCESS;
}

static psa_status_t op_set(void)
{
    return its_store_set(ITS_NS_NSPE, UID, "abc", 3, 0);
}

static psa_status_t op_get(void)
{
    char buf[8] = { 0 };
    uint32_t n = 0;
    psa_status_t st = its_store_get(ITS_NS_NSPE, UID, 0, buf, sizeof(buf),
                                    &n, NULL);

    return st == PSA_SUCCESS && (n != 3 || memcmp(buf, "abc", 3) != 0)
               ? PSA_ERROR_DATA_CORRUPT : st;
}

/* object uid of the fill tests: g_fill_len bytes of (uint8_t)uid */
static uint32_t g_fill_len;
/* op_fill sets uid g_fill_base + 1, + 2, ... */
static uint64_t g_fill_base;

static void fill_data(uint64_t uid, uint8_t *buf)
{
    memset(buf, (uint8_t)uid, g_fill_len);
}

/* sets uid 1, 2, ... until a set fails; prints "FILL <n> <status>" */
static psa_status_t op_fill(void)
{
    static uint8_t buf[ITS_STORE_MAX];
    psa_status_t st = PSA_SUCCESS;
    uint64_t uid = g_fill_base;

    while (st == PSA_SUCCESS && uid < g_fill_base + 100000u) {
        fill_data(++uid, buf);
        st = its_store_set(ITS_NS_NSPE, uid, buf, g_fill_len, 0);
    }
    printf("FILL %lu %d\n", (unsigned long)(uid - 1 - g_fill_base),
           (int)st);
    return st == PSA_ERROR_INSUFFICIENT_STORAGE ? PSA_SUCCESS : st;
}

/*
 * uid 1 and uid t read back, an overwrite of uid 1 succeeds or
 * returns INSUFFICIENT_STORAGE, uid t is removed and set again; t = 2,
 * or uid 1001 (3 B) when 3 B objects followed the fill
 */
static psa_status_t op_full_use(void)
{
    static uint8_t buf[ITS_STORE_MAX];
    static uint8_t want[ITS_STORE_MAX];
    uint64_t uids[2] = { 1, g_fill_base != 0 ? 1001 : 2 };
    uint32_t lens[2] = { g_fill_len, g_fill_base != 0 ? 3 : g_fill_len };
    psa_status_t st = PSA_SUCCESS;
    uint32_t n = 0;

    for (size_t i = 0; i < 2 && st == PSA_SUCCESS; i++) {
        memset(want, (uint8_t)uids[i], lens[i]);
        st = its_store_get(ITS_NS_NSPE, uids[i], 0, buf, lens[i], &n, NULL);
        if (st == PSA_SUCCESS &&
            (n != lens[i] || memcmp(buf, want, n) != 0)) {
            st = PSA_ERROR_DATA_CORRUPT;
        }
    }
    printf("USE get %d", (int)st);
    if (st == PSA_SUCCESS) {
        memset(buf, 1, g_fill_len);
        st = its_store_set(ITS_NS_NSPE, 1, buf, g_fill_len, 0);
        printf(" overwrite %d", (int)st);
        st = st == PSA_ERROR_INSUFFICIENT_STORAGE ? PSA_SUCCESS : st;
    }
    if (st == PSA_SUCCESS) {
        st = its_store_remove(ITS_NS_NSPE, uids[1]);
        printf(" remove %d", (int)st);
    }
    if (st == PSA_SUCCESS) {
        memset(buf, (uint8_t)uids[1], lens[1]);
        st = its_store_set(ITS_NS_NSPE, uids[1], buf, lens[1], 0);
        printf(" set %d", (int)st);
    }
    printf("\n");
    return st;
}

/* SPE boot on the PSM at block first, in a child with stdout in b->log */
static void spe_boot(size_t first, psa_status_t (*op)(void), struct boot *b)
{
    int fd[2];
    size_t got = 0;
    ssize_t n;
    pid_t pid;

    memset(b, 0, sizeof(*b));
    b->init = b->op = 1;
    fflush(stdout);
    if (pipe(fd) != 0) {
        return;
    }
    pid = fork();
    if (pid == 0) {
        psa_status_t st;

        close(fd[0]);
        dup2(fd[1], 1);
        lfs_ram_window(first, PSM_BLOCKS);
        st = svc_its_init();
        printf("RESULT %d %d\n", (int)st,
               (int)(st == PSA_SUCCESS ? op() : 1));
        fflush(stdout);
        _exit(0);
    }
    close(fd[1]);
    while ((n = read(fd[0], b->log + got, sizeof(b->log) - 1 - got)) > 0) {
        got += (size_t)n;
    }
    close(fd[0]);
    waitpid(pid, NULL, 0);
    b->log[got] = '\0';
    if (strstr(b->log, "RESULT ") != NULL) {
        sscanf(strstr(b->log, "RESULT "), "RESULT %d %d", &b->init, &b->op);
    }
}

/* occurrences of s in log */
static int count_of(const char *log, const char *s)
{
    int n = 0;

    for (const char *p = strstr(log, s); p != NULL; p = strstr(p + 1, s)) {
        n++;
    }
    return n;
}

/* raw littlefs of the its_lfs.c geometry on blocks [first, +count) */
static uint8_t g_rd[256];
static uint8_t g_pg[256];
static uint8_t g_fb[256];
static uint32_t g_la[8];
static struct lfs_context g_ctx;
static struct lfs_config g_cfg;
static const struct lfs_file_config g_fcfg = { .buffer = g_fb };

static lfs_t *raw_mount(size_t first, size_t count)
{
    memset(&g_ctx, 0, sizeof(g_ctx));
    memset(&g_cfg, 0, sizeof(g_cfg));
    g_ctx.partition_name = "PSM";
    g_cfg.read_size = g_cfg.prog_size = g_cfg.cache_size = 256;
    g_cfg.block_size = LFS_RAM_BLOCK;
    g_cfg.block_cycles = 500;
    g_cfg.lookahead_size = 32;
    g_cfg.read_buffer = g_rd;
    g_cfg.prog_buffer = g_pg;
    g_cfg.lookahead_buffer = g_la;
    lfs_ram_window(first, count);
    return lfs_xip_init(&g_ctx, &g_cfg);
}

static int raw_write(lfs_t *lfs, const char *name, const void *d, size_t n)
{
    lfs_file_t f;
    int rc = lfs_file_opencfg(lfs, &f, name,
                              LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC,
                              &g_fcfg);

    if (rc >= 0) {
        rc = lfs_file_write(lfs, &f, d, (lfs_size_t)n) == (lfs_ssize_t)n
                 ? 0 : -1;
        rc = lfs_file_close(lfs, &f) < 0 ? -1 : rc;
    }
    return rc;
}

static uint32_t g_lcg;

static uint32_t rnd(void)
{
    g_lcg = g_lcg * 1103515245u + 12345u;
    return (g_lcg >> 16) & 0x7fffu;
}

/* One boot of an ITS-like writer: meta + ops random sets / removes */
static void writer_boot(size_t first, int ops)
{
    static uint8_t buf[2100];
    lfs_t *lfs = raw_mount(first, PSM_BLOCKS);
    int rc = lfs != NULL ? lfs_mkdir(lfs, "/its") : -1;

    if (rc != 0 && rc != LFS_ERR_EXIST) {
        return;
    }
    for (int i = -1; i < ops; i++) {
        char name[32] = "/its/meta";
        uint32_t len = 24;

        if (i >= 0) {
            snprintf(name, sizeof(name), "/its/n_%016x",
                     (unsigned)(1 + rnd() % 40));
            len = rnd() % 600;
            len = rnd() % 8 == 0 ? rnd() % 2092 : len;
            if (rnd() % 4 == 0) {
                lfs_remove(lfs, name);
                continue;
            }
        }
        memset(buf, (int)rnd(), len);
        raw_write(lfs, name, buf, len);
    }
    lfs_unmount(lfs);
}

/*
 * The engine-run history: ITS on the 1M_boot2 PSM, then the OTA table
 * PSM that overlaps its first 10 blocks. Seed 1082: 9 boots.
 */
static void img_stale(void)
{
    g_lcg = 1082;
    for (int s = 0; s < 2; s++) {
        size_t first = s == 0 ? BOOT2_PSM : OTA_PSM;
        int boots = 1 + (int)(rnd() % 12);

        for (int b = 0; b < boots; b++) {
            writer_boot(first, (int)(rnd() % 30));
        }
    }
}

/* A foreign littlefs on the PSM: name of kind dir or file in it */
static void img_foreign(const char *dir, const char *file, size_t len)
{
    static const uint8_t data[3000];
    lfs_t *lfs = raw_mount(BOOT2_PSM, PSM_BLOCKS);

    if (dir != NULL) {
        lfs_mkdir(lfs, "/its");
        lfs_mkdir(lfs, dir);
    }
    if (file != NULL) {
        if (strncmp(file, "/its/", 5) == 0) {
            lfs_mkdir(lfs, "/its");
        }
        raw_write(lfs, file, data, len);
    }
    lfs_unmount(lfs);
}

/* A littlefs formatted for 64 blocks at the PSM start */
static void img_geometry(void)
{
    lfs_t *lfs = raw_mount(BOOT2_PSM, PSM_BLOCKS * 2u);

    lfs_mkdir(lfs, "/data");
    raw_write(lfs, "/data/x", "x", 1);
    lfs_unmount(lfs);
}

/* 1 when a raw mount succeeds and lfs_mkdir("/its") gives CORRUPT */
static int mkdir_corrupt(void)
{
    lfs_t *lfs = raw_mount(BOOT2_PSM, PSM_BLOCKS);
    int rc = lfs != NULL ? lfs_mkdir(lfs, "/its") : 0;

    if (lfs != NULL) {
        lfs_unmount(lfs);
    }
    return rc == LFS_ERR_CORRUPT;
}

/*
 * Boot 1 reformats once with line, ITS works; boots 2 and 3 do not
 * reformat and read the object back.
 */
static void check_recovers(const char *what, const char *line)
{
    struct boot b[3];
    char msg[160];

    spe_boot(BOOT2_PSM, op_set, &b[0]);
    spe_boot(BOOT2_PSM, op_get, &b[1]);
    spe_boot(BOOT2_PSM, op_get, &b[2]);
    snprintf(msg, sizeof(msg), "%s: boot 1 \"%s\" once, ITS set", what,
             line);
    check(b[0].init == PSA_SUCCESS && b[0].op == PSA_SUCCESS &&
          count_of(b[0].log, line) == 1 &&
          count_of(b[0].log, "reformatting") == 1, msg);
    snprintf(msg, sizeof(msg), "%s: boots 2, 3 no reformat, get abc", what);
    check(b[1].init == PSA_SUCCESS && b[1].op == PSA_SUCCESS &&
          b[2].init == PSA_SUCCESS && b[2].op == PSA_SUCCESS &&
          count_of(b[1].log, "reformat") == 0 &&
          count_of(b[2].log, "reformat") == 0, msg);
}

static void test_stale(void)
{
    lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
    img_stale();
    check(mkdir_corrupt(), "stale PSM: mounts, lfs_mkdir(/its) = CORRUPT");
    check_recovers("stale PSM",
                   "[SPE] ITS store unreadable (lfs -84), reformatting");
}

static void test_foreign(void)
{
    static const struct {
        const char *what;
        const char *dir;
        const char *file;
        size_t len;
    } imgs[] = {
        { "foreign /its file", NULL, "/its", 7 },
        { "foreign /its/meta dir", "/its/meta", NULL, 0 },
        { "foreign 3000 B /its/meta", NULL, "/its/meta", 3000 },
    };

    for (size_t i = 0; i < sizeof(imgs) / sizeof(imgs[0]); i++) {
        lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
        img_foreign(imgs[i].dir, imgs[i].file, imgs[i].len);
        check_recovers(imgs[i].what,
                       "[SPE] ITS store unreadable, reformatting");
    }
    lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
    img_geometry();
    check_recovers("littlefs of 64 blocks",
                   "[SPE] ITS store unreadable (lfs -22), reformatting");
}

/* No HUK: the stale PSM stays as it is, nothing is formatted */
static void test_no_huk(void)
{
    static uint8_t before[PSM_BLOCKS * LFS_RAM_BLOCK];
    struct boot b;

    lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
    img_stale();
    memcpy(before, lfs_ram_block(BOOT2_PSM), sizeof(before));
    g_no_huk = 1;
    spe_boot(BOOT2_PSM, op_get, &b);
    g_no_huk = 0;
    check(b.init == PSA_ERROR_HARDWARE_FAILURE &&
          count_of(b.log, "reformat") == 0 &&
          count_of(b.log, "[LFS]") == 0 &&
          memcmp(before, lfs_ram_block(BOOT2_PSM), sizeof(before)) == 0,
          "no HUK on a stale PSM: HARDWARE_FAILURE, no mount, PSM unchanged");
}

/* PSM refused by the layout check: no mount, PSM unchanged */
static void test_bad_layout(void)
{
    static uint8_t before[PSM_BLOCKS * LFS_RAM_BLOCK];
    struct boot b;

    lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
    img_stale();
    memcpy(before, lfs_ram_block(BOOT2_PSM), sizeof(before));
    g_bad_layout = 1;
    spe_boot(BOOT2_PSM, op_get, &b);
    g_bad_layout = 0;
    check(b.init == PSA_ERROR_STORAGE_FAILURE &&
          count_of(b.log, "[SPE] PSM layout") == 1 &&
          count_of(b.log, "[LFS]") == 0 &&
          memcmp(before, lfs_ram_block(BOOT2_PSM), sizeof(before)) == 0,
          "PSM layout refused: STORAGE_FAILURE, no mount, PSM unchanged");
}

/*
 * A store filled to INSUFFICIENT_STORAGE by the NSPE: the next boots
 * still write the meta, read the objects and can overwrite, remove
 * and set one.
 */
static void test_full(void)
{
    /* objects of lens[i], then 3 B objects when tail[i] */
    static const uint32_t lens[] = { ITS_STORE_MAX, 600, 3, ITS_STORE_MAX,
                                     600 };
    static const int tail[] = { 0, 0, 0, 1, 1 };
    char msg[96];
    struct boot b;
    int boots;

    for (size_t i = 0; i < sizeof(lens) / sizeof(lens[0]); i++) {
        g_fill_len = lens[i];
        g_fill_base = 0;
        lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
        spe_boot(BOOT2_PSM, op_fill, &b);
        printf("     %u B: %s", (unsigned)lens[i], strstr(b.log, "FILL "));
        if (tail[i] && b.init == PSA_SUCCESS && b.op == PSA_SUCCESS) {
            g_fill_len = 3;
            g_fill_base = 1000;
            spe_boot(BOOT2_PSM, op_fill, &b);
            printf("     then 3 B: %s", strstr(b.log, "FILL ") ?
                   strstr(b.log, "FILL ") : b.log);
            g_fill_len = lens[i];
        }
        snprintf(msg, sizeof(msg), "%u B objects%s: store filled to "
                 "INSUFFICIENT_STORAGE", (unsigned)lens[i],
                 tail[i] ? " then 3 B" : "");
        check(b.init == PSA_SUCCESS && b.op == PSA_SUCCESS, msg);
        for (boots = 0; boots < 4; boots++) {
            spe_boot(BOOT2_PSM, op_full_use, &b);
            if (b.init != PSA_SUCCESS || b.op != PSA_SUCCESS ||
                count_of(b.log, "reformat") != 0) {
                printf("     boot %d: init %d op %d\n%s", boots + 1, b.init,
                       b.op, b.log);
                break;
            }
        }
        snprintf(msg, sizeof(msg), "%u B objects%s, full store: 4 boots "
                 "init, read, remove, set", (unsigned)lens[i],
                 tail[i] ? " then 3 B" : "");
        check(boots == 4, msg);
    }
}

static void test_controls(void)
{
    struct boot b;

    lfs_ram_fill(0, FLASH_BLOCKS, 0xff);
    spe_boot(BOOT2_PSM, op_set, &b);
    check(b.init == PSA_SUCCESS && b.op == PSA_SUCCESS &&
          count_of(b.log, "[LFS] try to reformat") == 1 &&
          count_of(b.log, "[SPE] ITS") == 0,
          "blank PSM: littlefs formats, no ITS reformat, set");
    spe_boot(BOOT2_PSM, op_get, &b);
    check(b.init == PSA_SUCCESS && b.op == PSA_SUCCESS &&
          count_of(b.log, "reformat") == 0,
          "own ITS: no reformat, object read back");
}

int main(void)
{
    if (lfs_ram_init(FLASH_BLOCKS) != 0) {
        printf("FAIL no RAM flash\n");
        return 1;
    }
    test_controls();
    test_no_huk();
    test_bad_layout();
    test_stale();
    test_foreign();
    test_full();
    return check_summary();
}
