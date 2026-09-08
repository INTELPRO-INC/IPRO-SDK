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
 * pio_sdcard_cmd.c - customer-facing DEMO / evaluation / benchmark shell
 * command for the PIO 4-bit SD host (GPIO22-27: CLK=22 CMD=23 D0-D3=24-27).
 *
 * DUAL-MOUNT: the PIO host lives on the DEV_MMC drive slot (mounted at /piosd)
 * so it can run at the SAME TIME as the hardware SDH (DEV_SD, /sdcard, commands
 * `sdcard` / `format_sd` / `sdh_speed_test`). One card per socket.
 *
 * THREADING: the CLI only PARSES + DISPATCHES to a worker task (priority 1,
 * below the USB console TX task) so (a) console output streams live during a
 * test and (b) `pio_sdcard status` stays responsive while a soak runs. Long
 * operations print a once-per-second progress line and update a shared status
 * struct, so a running test is never mistaken for a hang.
 *
 * Customer commands:
 *   pio_sdcard info                 identify the card (vendor / capacity / bus)
 *   pio_sdcard format               make a fresh exFAT + mount   (ERASES CARD)
 *   pio_sdcard mount                mount an existing filesystem at /piosd
 *   pio_sdcard benchmark [mb]       full seq-write / seq-read / small-file suite + report
 *   pio_sdcard status               is a test running? live progress
 * Advanced / building blocks:
 *   fspeed [kb] | fstress <w|r|x> <idx> [mb] | fstress_batch <n> [mb]
 *   fstress_mix [secs] | dual [mb] | pad <drive> <pull> [smt]
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#include "hal_pio_sdh.h"
#include "hal_mtimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <fatfs.h>
#include <diskio.h>
#include <ff.h>
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
#include "evfs.h"
#include "evfs/fatfs_fs.h"
#include "evfs_posix.h"
#endif

#define PIO_MOUNT_POINT  "/piosd"
#define CHUNK_BYTES      (64u * 1024u)

/* App-local diskio port: PIO host on the DEV_MMC drive slot. */
void fatfs_pio_mmc_driver_register(const hal_pio_sdh_cfg_t *cfg);

static uint8_t s_use_dma = 1;
static uint8_t s_dma_tx_ch = 2;
static uint8_t s_dma_rx_ch = 3;

static void pio_sdcard_default_cfg(hal_pio_sdh_cfg_t *c)
{
    c->pin_clk = 22;
    c->pin_cmd = 23;
    c->pin_d0  = 24;      /* D1=25, D2=26, D3=27 */
    c->sm_cmd  = 0;
    c->sm_dat  = 1;
    c->use_dma = s_use_dma;
    c->dma_tx_ch = s_dma_tx_ch;
    c->dma_rx_ch = s_dma_rx_ch;
    c->pio_clk_div = 0;
    c->pio_clk_sel = 0;
}

/* ---- Shared live status (worker writes, `status` command reads) ---- */
static volatile struct {
    char     phase[28];
    uint32_t done_mb;
    uint32_t total_mb;
    uint32_t cur_kbs;
    bool     running;
} s_status;

/* ---- Progress helper: 1 Hz progress line + shared-status update ---- */
typedef struct {
    const char *label;
    uint32_t    total_mb;
    uint64_t    t0_us;
    TickType_t  last_tick;
} prog_t;

static void prog_begin(prog_t *p, const char *label, uint32_t total_mb)
{
    p->label = label;
    p->total_mb = total_mb;
    p->t0_us = mtimer_get_time_us();
    p->last_tick = xTaskGetTickCount();
    strncpy((char *)s_status.phase, label, sizeof(s_status.phase) - 1);
    s_status.phase[sizeof(s_status.phase) - 1] = '\0';
    s_status.total_mb = total_mb;
    s_status.done_mb = 0;
    s_status.cur_kbs = 0;
    s_status.running = true;
}

/* Call after each chunk. `io_us` is the accumulated PURE read/write I/O time
 * (buffer fill + verify EXCLUDED) so the reported rate is raw SD throughput,
 * comparable to other benchmarks. The 1 Hz throttle uses wall time. */
static void prog_tick(prog_t *p, uint64_t done_bytes, uint64_t io_us)
{
    if (!io_us) io_us = 1;
    uint32_t kbs = (uint32_t)(done_bytes * 1000000ULL / 1024ULL / io_us);
    uint32_t done_mb = (uint32_t)(done_bytes >> 20);
    s_status.done_mb = done_mb;
    s_status.cur_kbs = kbs;
    TickType_t now = xTaskGetTickCount();
    if ((now - p->last_tick) >= pdMS_TO_TICKS(1000)) {
        p->last_tick = now;
        printf("    %-12s %4lu / %4lu MB   %2lu.%02lu MB/s\n",
               p->label, (unsigned long)done_mb, (unsigned long)p->total_mb,
               (unsigned long)(kbs / 1024u), (unsigned long)((kbs % 1024u) * 100u / 1024u));
    }
}

/* Returns KiB/s from pure I/O time. Clears running. */
static uint32_t prog_end(prog_t *p, uint64_t done_bytes, uint64_t io_us)
{
    (void)p;
    if (!io_us) io_us = 1;
    uint32_t kbs = (uint32_t)(done_bytes * 1000000ULL / 1024ULL / io_us);
    s_status.done_mb = (uint32_t)(done_bytes >> 20);
    s_status.cur_kbs = kbs;
    s_status.running = false;
    return kbs;
}

static void kbs_str(uint32_t kbs, char *out, size_t n)
{
    snprintf(out, n, "%lu.%02lu MB/s",
             (unsigned long)(kbs / 1024u), (unsigned long)((kbs % 1024u) * 100u / 1024u));
}

/* ---- SD manufacturer-ID -> vendor name (common IDs) ---- */
static const char *sd_vendor(uint8_t mid)
{
    switch (mid) {
        case 0x01: return "Panasonic";
        case 0x02: return "Toshiba/Kioxia";
        case 0x03: return "SanDisk";
        case 0x1b: return "Samsung";
        case 0x1d: return "ADATA";
        case 0x27: return "Phison";
        case 0x28: return "Lexar";
        case 0x31: return "Silicon Power";
        case 0x41: return "Kingston";
        case 0x74: return "Transcend";
        case 0x76: return "Patriot";
        case 0x82: return "Sony";
        default:   return "unknown";
    }
}

/* ---- EVFS registration + posix mount of the PIO card at /piosd ---- */
static int pio_mount(void)
{
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_USE_FATFS)
    int r = evfs_register_fatfs("piosd", DEV_MMC, false);
    if (r != EVFS_OK && r != EVFS_ERR_BAD_ARG) return -1;
    r = evfs_posix_mount(PIO_MOUNT_POINT, "piosd");
    if (r != 0 && r != -16) return -1;
#endif
    return 0;
}

/* Pace a printed line: the USB-FS console (drop-oldest ring) loses bytes if
 * flooded by a dense burst. Used on summary/header lines (progress lines are
 * already >=1 s apart). ~12 ms/line, only on the once-per-run report text. */
static void bl(void) { vTaskDelay(pdMS_TO_TICKS(12)); }

/* Quick check: is a filesystem usable at <mnt> right now? */
static bool fs_ready(const char *mnt)
{
    char p[56];
    snprintf(p, sizeof(p), "%s/.probe", mnt);
    int fd = open(p, O_CREAT | O_RDWR);
    if (fd < 0) return false;
    close(fd);
    unlink(p);
    return true;
}

/* ==================================================================== */
/*  Heavy operations - run in the worker task                           */
/* ==================================================================== */

static void do_info(void)
{
    hal_pio_sdh_cfg_t cfg;
    hal_pio_sdh_card_t card;
    pio_sdcard_default_cfg(&cfg);
    if (hal_pio_sdh_init(&cfg, &card)) {
        printf("pio_sdh: card not detected - check wiring (CLK=G22 CMD=G23 D0-3=G24-27) + power\n");
        return;
    }
    char pnm[6] = { 0 };
    for (int i = 0; i < 5; i++) pnm[i] = (char)card.cid[3 + i];
    uint32_t mb = card.block_count / 2048u;

    printf("\n--- SD card ---------------------------------------\n");
    printf("  Vendor    : %s (MID 0x%02x)\n", sd_vendor(card.cid[0]), card.cid[0]);
    printf("  Product   : %.5s  rev %u.%u\n", pnm, card.cid[8] >> 4, card.cid[8] & 0xF);
    printf("  Capacity  : %lu.%01lu GB (%lu MB, %lu sectors)\n",
           (unsigned long)(mb / 1024u), (unsigned long)((mb % 1024u) * 10u / 1024u),
           (unsigned long)mb, (unsigned long)card.block_count);
    printf("  Bus       : %u-bit %s, PIO host @ 24 MHz %s\n",
           card.bus_width, card.access_mode ? "SDHC/SDXC" : "SDSC",
           s_use_dma ? "(DMA)" : "(CPU)");
    printf("  Mount     : %s\n", PIO_MOUNT_POINT " (run `format` or `mount`)");
    printf("  Heap free : %lu KB (min-ever %lu KB)\n",
           (unsigned long)(xPortGetFreeHeapSize() / 1024u),
           (unsigned long)(xPortGetMinimumEverFreeHeapSize() / 1024u));
    printf("---------------------------------------------------\n");
}

#if FF_FS_MKFS_PROGRESS_REPORT
static void fmt_progress(int pct) { printf("    format %d%%\n", pct); }
#endif

static void do_format(void)
{
    hal_pio_sdh_cfg_t cfg;
    pio_sdcard_default_cfg(&cfg);
    fatfs_pio_mmc_driver_register(&cfg);
    printf("pio_sdh: formatting exFAT over PIO host (ERASES CARD)...\n");
#if FF_FS_MKFS_PROGRESS_REPORT
    int err = fatfs_format(DEV_MMC, fmt_progress);
#else
    int err = fatfs_format(DEV_MMC, NULL);
#endif
    if (err != FR_OK) {
        printf("pio_sdh: format FAILED (err=%d; 1=disk-io, 13=wrote-but-mount-lost - check wiring/power)\n", err);
        return;
    }
    if (pio_mount() == 0)
        printf("pio_sdh: format OK - exFAT mounted at %s\n", PIO_MOUNT_POINT);
    else
        printf("pio_sdh: format OK but mount failed\n");
}

static void do_mount(void)
{
    hal_pio_sdh_cfg_t cfg;
    pio_sdcard_default_cfg(&cfg);
    fatfs_pio_mmc_driver_register(&cfg);
    (void)fatfs_register();          /* eager f_mount (aos result, not 0=ok) */
    if (pio_mount() == 0 && fs_ready(PIO_MOUNT_POINT))
        printf("pio_sdh: mounted at %s\n", PIO_MOUNT_POINT);
    else
        printf("pio_sdh: mount failed / no filesystem - run `pio_sdcard format` (ERASES)\n");
}

/* Single-file write+read+verify. Returns rates via out params (KiB/s), and
 * whether the read-back verified. Prints live progress. */
static int seq_file(const char *path, uint32_t mb, uint32_t *out_wkbs,
                    uint32_t *out_rkbs, bool *out_verify)
{
    uint32_t chunks = mb * (1024u * 1024u / CHUNK_BYTES);
    if (!chunks) chunks = 1;
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { printf("pio_sdh: no memory\n"); return -1; }

    /* WRITE */
    int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
    if (fd < 0) { printf("pio_sdh: open-for-write failed (%d) - mounted?\n", fd); vPortFree(buf); return -1; }
    prog_t pw; prog_begin(&pw, "write", mb);
    uint32_t done = 0; uint64_t wio = 0;
    for (uint32_t c = 0; c < chunks; c++) {
        uint32_t seed = c * 0x85EBCA77u;
        for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);   /* fill: untimed */
        uint64_t t = mtimer_get_time_us();
        int r = write(fd, buf, CHUNK_BYTES);
        wio += mtimer_get_time_us() - t;
        if (r != (int)CHUNK_BYTES) break;
        done++;
        prog_tick(&pw, (uint64_t)done * CHUNK_BYTES, wio);
    }
    uint64_t tc = mtimer_get_time_us(); close(fd); wio += mtimer_get_time_us() - tc;
    uint32_t wkbs = prog_end(&pw, (uint64_t)done * CHUNK_BYTES, wio);
    if (done != chunks) { printf("pio_sdh: WRITE FAILED at %lu/%lu MB\n",
                                 (unsigned long)(done / 16u), (unsigned long)mb); vPortFree(buf); return -1; }

    /* READ + verify (verify is untimed - reported rate is pure read I/O) */
    fd = open(path, O_RDONLY);
    if (fd < 0) { printf("pio_sdh: re-open failed (%d)\n", fd); vPortFree(buf); return -1; }
    prog_t pr; prog_begin(&pr, "read+verify", mb);
    done = 0; bool ok = true; uint64_t rio = 0;
    for (uint32_t c = 0; c < chunks; c++) {
        uint64_t t = mtimer_get_time_us();
        int r = read(fd, buf, CHUNK_BYTES);
        rio += mtimer_get_time_us() - t;
        if (r != (int)CHUNK_BYTES) { ok = false; break; }
        uint32_t seed = c * 0x85EBCA77u;
        for (uint32_t i = 0; i < CHUNK_BYTES; i++)   /* verify: untimed */
            if (buf[i] != (uint8_t)(seed + i)) { ok = false; break; }
        if (!ok) break;
        done++;
        prog_tick(&pr, (uint64_t)done * CHUNK_BYTES, rio);
    }
    close(fd);
    uint32_t rkbs = prog_end(&pr, (uint64_t)done * CHUNK_BYTES, rio);
    vPortFree(buf);
    *out_wkbs = wkbs; *out_rkbs = rkbs; *out_verify = (ok && done == chunks);
    return 0;
}

/* Map errno from a failed open/write/read to a customer-actionable cause.
 * The two that matter most look identical in raw output but need opposite fixes:
 *   ENOSPC = card really is full / a small old partition -> format
 *   EIO    = a write I/O fault (NOT full) -> power/wiring/wauto/card         */
static const char *fs_fail_cause(int e)
{
    switch (e) {
        case ENOSPC: return "card FULL or a small old partition -> run `format`";
        case EIO:    return "card I/O fault (NOT full) -> check card power/wiring, then `format`";
        case EMFILE: return "too many open files (fd pool)";
        case ENOENT: return "path/volume not found -> `mount` first";
        case 0:      return "short transfer (no errno set)";
        default:     return strerror(e);
    }
}

/* Best-effort free space (MB) on the PIO volume ("mmc:"); -1 if unavailable. */
static long pio_free_mb(void)
{
    DWORD nclst = 0; FATFS *fs = NULL;
    if (f_getfree("mmc:", &nclst, &fs) != FR_OK || !fs) return -1;
    /* free bytes = free clusters x sectors/cluster x 512 (FF_MAX_SS) */
    uint64_t bytes = (uint64_t)nclst * (uint64_t)fs->csize * 512ULL;
    return (long)(bytes / (1024ULL * 1024ULL));
}

/* Many small files under <mnt>: write N x fkb-KiB, read+verify all. files/s. */
static int smallfiles(const char *mnt, uint32_t n, uint32_t fkb, uint32_t *out_wfps,
                      uint32_t *out_rfps, bool *out_verify)
{
    uint8_t *buf = pvPortMalloc(fkb * 1024u);
    if (!buf) { printf("pio_sdh: no memory\n"); return -1; }
    char path[48];

    strncpy((char *)s_status.phase, "small write", sizeof(s_status.phase) - 1);
    s_status.phase[sizeof(s_status.phase) - 1] = '\0';
    s_status.total_mb = (n * fkb) / 1024u;
    s_status.done_mb = 0;
    s_status.running = true;
    uint32_t wn = 0, rn = 0; int wfail = -1, rfail = -1;
    uint64_t wio = 0;                 /* pure open+write+close time (fill excluded) */
    TickType_t wtick = xTaskGetTickCount();
    for (uint32_t f = 0; f < n; f++) {
        snprintf(path, sizeof(path), "%s/sf_%lu.bin", mnt, (unsigned long)f);
        for (uint32_t i = 0; i < fkb * 1024u; i++) buf[i] = (uint8_t)(f + i);   /* fill: untimed */
        uint64_t t = mtimer_get_time_us();
        errno = 0;                                   /* clean read of the cause below */
        int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
        int r = (fd < 0) ? -1 : write(fd, buf, fkb * 1024u);
        int e = errno;                               /* capture before close()/printf clobbers */
        if (fd >= 0) close(fd);
        wio += mtimer_get_time_us() - t;
        if (fd < 0 || r != (int)(fkb * 1024u)) {
            wfail = (int)f;
            long freemb = pio_free_mb();
            printf("    small write FAILED at #%lu (open=%d write=%d errno=%d: %s)\n",
                   (unsigned long)f, fd, r, e, fs_fail_cause(e));
            if (freemb >= 0)
                printf("      volume free: %ld MB  (%lu files x %lu KB written first)\n",
                       freemb, (unsigned long)f, (unsigned long)fkb);
            break;
        }
        wn++;
        s_status.done_mb = (f + 1) * fkb / 1024u;
        if ((xTaskGetTickCount() - wtick) >= pdMS_TO_TICKS(1000)) {
            wtick = xTaskGetTickCount();
            printf("    small write  %lu / %lu files\n", (unsigned long)(f + 1), (unsigned long)n);
        }
    }
    if (!wio) wio = 1;
    uint32_t wfps = wn ? (uint32_t)((uint64_t)wn * 1000000ULL / wio) : 0;

    /* Only read back what was successfully written. */
    strncpy((char *)s_status.phase, "small read", sizeof(s_status.phase) - 1);
    uint64_t rio = 0;
    TickType_t rtick = xTaskGetTickCount();
    for (uint32_t f = 0; f < wn; f++) {
        snprintf(path, sizeof(path), "%s/sf_%lu.bin", mnt, (unsigned long)f);
        uint64_t t = mtimer_get_time_us();
        errno = 0;
        int fd = open(path, O_RDONLY);
        int r = (fd < 0) ? -1 : read(fd, buf, fkb * 1024u);
        int e = errno;
        if (fd >= 0) close(fd);
        rio += mtimer_get_time_us() - t;
        if (fd < 0 || r != (int)(fkb * 1024u)) {
            rfail = (int)f;
            printf("    small read FAILED at #%lu (open=%d read=%d errno=%d: %s)\n",
                   (unsigned long)f, fd, r, e, fs_fail_cause(e));
            if (f == 0 && wfail >= 0)
                printf("      (follows the write fault above - the card, not the filesystem)\n");
            break;
        }
        for (uint32_t i = 0; i < fkb * 1024u; i++)   /* verify: untimed */
            if (buf[i] != (uint8_t)(f + i)) { rfail = (int)f;
                printf("    small VERIFY FAILED at #%lu byte %lu\n",
                       (unsigned long)f, (unsigned long)i); break; }
        if (rfail >= 0) break;
        rn++;
        if ((xTaskGetTickCount() - rtick) >= pdMS_TO_TICKS(1000)) {
            rtick = xTaskGetTickCount();
            printf("    small read   %lu / %lu files\n", (unsigned long)(f + 1), (unsigned long)wn);
        }
    }
    if (!rio) rio = 1;
    uint32_t rfps = rn ? (uint32_t)((uint64_t)rn * 1000000ULL / rio) : 0;

    for (uint32_t f = 0; f < n; f++) {
        snprintf(path, sizeof(path), "%s/sf_%lu.bin", mnt, (unsigned long)f);
        unlink(path);
    }
    bool ok = (wfail < 0 && rfail < 0 && wn == n && rn == n);
    s_status.running = false;
    vPortFree(buf);
    *out_wfps = wfps; *out_rfps = rfps; *out_verify = ok;
    return 0;
}

/* One host's suite result. */
typedef struct { uint32_t wkbs, rkbs, wfps, rfps; bool ok; } suite_t;

/* Run seq (mb) + 128x256KB small-file on one mounted volume. Live progress. */
static void bench_suite(const char *mnt, const char *tag, uint32_t mb, suite_t *out)
{
    char path[56];
    bool seq_ok = false, sf_ok = false;
    out->wkbs = out->rkbs = out->wfps = out->rfps = 0;
    snprintf(path, sizeof(path), "%s/bench.bin", mnt);
    printf(" [%s] Sequential %lu MB:\n", tag, (unsigned long)mb); bl();
    (void)seq_file(path, mb, &out->wkbs, &out->rkbs, &seq_ok);
    unlink(path);   /* always remove bench.bin - even a partial write must not leak space into the small-file stage */
    printf(" [%s] Small files 128 x 256 KB:\n", tag); bl();
    (void)smallfiles(mnt, 128, 256, &out->wfps, &out->rfps, &sf_ok);
    out->ok = (seq_ok && sf_ok);
}

/* Print "  label ... 7.08 MB/s" for one or two columns. */
static void row_kbs(const char *label, uint32_t a, uint32_t b, int two)
{
    char sa[16], sb[16];
    kbs_str(a, sa, sizeof(sa));
    if (two) { kbs_str(b, sb, sizeof(sb));
        printf(" %-18s %11s   %11s\n", label, sa, sb); }
    else     printf(" %-18s %11s\n", label, sa);
    bl();
}

/* Flagship benchmark. dual=0: PIO only + CPU breakdown. dual=1: PIO vs HW SDH
 * side-by-side (needs both `sdcard` and PIO mounted). */
static void do_benchmark(uint32_t mb, int dual)
{
    if (!fs_ready(PIO_MOUNT_POINT)) {
        printf("pio_sdh: no filesystem at %s - run `pio_sdcard format` or `mount` first.\n",
               PIO_MOUNT_POINT);
        return;
    }
    if (dual && !fs_ready("/sdcard")) {
        printf("pio_sdh: dual needs the HW SDH card too - run `sdcard` first (mounts /sdcard).\n");
        return;
    }
    hal_pio_sdh_cfg_t cfg; hal_pio_sdh_card_t card;
    pio_sdcard_default_cfg(&cfg);
    (void)hal_pio_sdh_init(&cfg, &card);
    char pnm[6] = { 0 };
    for (int i = 0; i < 5; i++) pnm[i] = (char)card.cid[3 + i];
    uint32_t cap_mb = card.block_count / 2048u;

    printf("\n============ PIO SD Host Benchmark ============\n"); bl();
    printf(" Card : %s %.5s   %lu.%01lu GB   %u-bit %s\n",
           sd_vendor(card.cid[0]), pnm,
           (unsigned long)(cap_mb / 1024u), (unsigned long)((cap_mb % 1024u) * 10u / 1024u),
           card.bus_width, card.access_mode ? "SDHC/SDXC" : "SDSC"); bl();
    printf(" Mode : %s\n", dual ? "DUAL - PIO /piosd  vs  HW SDH /sdcard" : "PIO /piosd"); bl();
    printf(" Size : %lu MB sequential + 128 x 256 KB small-file\n", (unsigned long)mb); bl();
    long freemb = pio_free_mb();
    if (freemb >= 0) {
        printf(" Free : %ld MB on %s%s\n", freemb, PIO_MOUNT_POINT,
               (freemb < (long)(mb + 32u)) ? "   <- LOW: mounted a small partition? run `format`" : "");
        bl();
    }
    printf("-----------------------------------------------\n"); bl();

    size_t heap_before = xPortGetFreeHeapSize();
    hal_pio_sdh_wr1_diag_get(NULL, NULL, NULL, NULL);  /* clear settle stats */

    suite_t pio, hw;
    bench_suite(PIO_MOUNT_POINT, "PIO", mb, &pio);
    if (dual) {
        bench_suite("/sdcard", "HW", mb, &hw);
    }

    size_t heap_min = xPortGetMinimumEverFreeHeapSize();
    uint32_t peak_use_kb = (heap_before > heap_min) ? (uint32_t)((heap_before - heap_min) / 1024u) : 0;

    printf("-----------------------------------------------\n"); bl();
    if (dual) { printf(" %-18s %11s   %11s\n", "", "PIO", "HW SDH"); bl(); }
    row_kbs("Sequential write", pio.wkbs, hw.wkbs, dual);
    row_kbs("Sequential read",  pio.rkbs, hw.rkbs, dual);
    if (dual) {
        printf(" %-18s %8lu f/s   %8lu f/s\n", "Small-file write",
               (unsigned long)pio.wfps, (unsigned long)hw.wfps); bl();
        printf(" %-18s %8lu f/s   %8lu f/s\n", "Small-file read",
               (unsigned long)pio.rfps, (unsigned long)hw.rfps); bl();
        printf(" %-18s %11s   %11s\n", "Data integrity",
               pio.ok ? "PASS" : "FAIL", hw.ok ? "PASS" : "FAIL"); bl();
    } else {
        printf(" Small-file write .......... %8lu files/s\n", (unsigned long)pio.wfps); bl();
        printf(" Small-file read ........... %8lu files/s\n", (unsigned long)pio.rfps); bl();
        printf(" Data integrity ............ %s\n",
               pio.ok ? "PASS (all verified)" : "FAIL - see above"); bl();
    }
    /* Internal write-path counters: only surfaced when something is off, so a
     * clean run stays readable. (`settle` is a diagnostic knob, default on.) */
    uint32_t w1t = 0, w1p = 0, w1to = 0, w1us = 0;
    hal_pio_sdh_wr1_diag_get(&w1t, &w1p, &w1to, &w1us);
    if (w1to || !hal_pio_sdh_get_wr1_settle()) {
        printf(" NOTE: write-path diagnostics: settle %s, card-not-ready %lu\n",
               hal_pio_sdh_get_wr1_settle() ? "on" : "OFF (diagnostic mode)",
               (unsigned long)w1to); bl();
    }
    printf(" RAM: heap free %lu KB, peak use %lu KB (64 KB I/O buf + FatFS)\n",
           (unsigned long)(xPortGetFreeHeapSize() / 1024u), (unsigned long)peak_use_kb); bl();
    printf("-----------------------------------------------\n"); bl();
    printf(" RESULT: %s\n", (pio.ok && (!dual || hw.ok)) ? "PASS" : "FAIL"); bl();
    printf("===============================================\n");
}

/* ---- Advanced building blocks (unchanged behaviour, with progress) ---- */

static void do_fspeed(uint32_t kb)
{
    uint32_t mb = kb / 1024u; if (!mb) mb = 1;
    uint32_t wkbs = 0, rkbs = 0; bool ok = false;
    char ws[16], rs[16];
    if (seq_file(PIO_MOUNT_POINT "/pio_speed.bin", mb, &wkbs, &rkbs, &ok) == 0) {
        unlink(PIO_MOUNT_POINT "/pio_speed.bin");
        kbs_str(wkbs, ws, sizeof(ws)); kbs_str(rkbs, rs, sizeof(rs));
        printf("pio_sdh: fspeed %lu MB: write %s, read %s, data %s\n",
               (unsigned long)mb, ws, rs, ok ? "verified" : "MISMATCH");
    }
}

static void do_fstress(char mode, uint32_t idx, uint32_t mb)
{
    char path[48];
    snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_str_%lu.bin", (unsigned long)idx);
    if (mode == 'x') { printf("pio_sdh: fstress x idx=%lu rc=%d\n", (unsigned long)idx, unlink(path)); return; }
    uint32_t wkbs = 0, rkbs = 0; bool ok = false;
    char ws[16], rs[16];
    if (seq_file(path, mb, &wkbs, &rkbs, &ok) == 0) {
        kbs_str(wkbs, ws, sizeof(ws)); kbs_str(rkbs, rs, sizeof(rs));
        printf("pio_sdh: fstress idx=%lu %lu MB: write %s, read %s, verify=%s\n",
               (unsigned long)idx, (unsigned long)mb, ws, rs, ok ? "OK" : "BAD");
    }
}

static void do_fstress_batch(uint32_t nfiles, uint32_t mb)
{
    uint32_t chunks = mb * (1024u * 1024u / CHUNK_BYTES);
    if (!chunks) chunks = 1;
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { printf("pio_sdh: fstress_batch no memory\n"); return; }
    char path[48];
    uint32_t wmin = 0xFFFFFFFFu, wmax = 0, rmin = 0xFFFFFFFFu, rmax = 0;
    uint32_t wok = 0, rok = 0, vok = 0;
    int failed = 0;
    printf("pio_sdh: fstress_batch %lu files x %lu MB\n", (unsigned long)nfiles, (unsigned long)mb);
    for (uint32_t f = 0; f < nfiles && !failed; f++) {
        snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_str_%lu.bin", (unsigned long)f);
        int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
        if (fd < 0) { printf("pio_sdh: batch W f=%lu open %d\n", (unsigned long)f, fd); failed = 1; break; }
        char lbl[24]; snprintf(lbl, sizeof(lbl), "W f=%lu", (unsigned long)f);
        prog_t pw; prog_begin(&pw, lbl, mb);
        uint32_t done = 0; uint64_t wio = 0;
        for (uint32_t c = 0; c < chunks; c++) {
            uint32_t seed = f * 0x9E3779B1u + c * 0x85EBCA77u;
            for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);   /* fill: untimed */
            uint64_t t = mtimer_get_time_us();
            int r = write(fd, buf, CHUNK_BYTES);
            wio += mtimer_get_time_us() - t;
            if (r != (int)CHUNK_BYTES) { failed = 1; break; }
            done++; prog_tick(&pw, (uint64_t)done * CHUNK_BYTES, wio);
        }
        uint64_t tc = mtimer_get_time_us(); close(fd); wio += mtimer_get_time_us() - tc;
        uint32_t kbs = prog_end(&pw, (uint64_t)done * CHUNK_BYTES, wio);
        if (failed) { printf("pio_sdh: batch W f=%lu WRITE-FAIL (disk err or ENOSPC)\n", (unsigned long)f); break; }
        wok++; if (kbs < wmin) wmin = kbs; if (kbs > wmax) wmax = kbs;
        printf("  W f=%lu done: %lu.%02lu MB/s\n", (unsigned long)f,
               (unsigned long)(kbs / 1024u), (unsigned long)((kbs % 1024u) * 100u / 1024u));
    }
    if (!failed) {
        for (uint32_t f = 0; f < nfiles; f++) {
            snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_str_%lu.bin", (unsigned long)f);
            int fd = open(path, O_RDONLY);
            if (fd < 0) { printf("pio_sdh: batch R f=%lu open %d\n", (unsigned long)f, fd); continue; }
            char lbl[24]; snprintf(lbl, sizeof(lbl), "R f=%lu", (unsigned long)f);
            prog_t pr; prog_begin(&pr, lbl, mb);
            long bad = -1; uint32_t got = 0; uint64_t rio = 0;
            for (uint32_t c = 0; c < chunks; c++) {
                uint64_t t = mtimer_get_time_us();
                int r = read(fd, buf, CHUNK_BYTES);
                rio += mtimer_get_time_us() - t;
                if (r != (int)CHUNK_BYTES) break;
                got++;
                uint32_t seed = f * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK_BYTES; i++)   /* verify: untimed */
                    if (buf[i] != (uint8_t)(seed + i)) { bad = (long)(c * CHUNK_BYTES + i); break; }
                if (bad >= 0) break;
                prog_tick(&pr, (uint64_t)got * CHUNK_BYTES, rio);
            }
            close(fd);
            uint32_t kbs = prog_end(&pr, (uint64_t)got * CHUNK_BYTES, rio);
            int good = (bad < 0 && got == chunks);
            rok++; if (kbs < rmin) rmin = kbs; if (kbs > rmax) rmax = kbs;
            if (good) vok++;
            printf("  R f=%lu done: %lu.%02lu MB/s verify=%s\n", (unsigned long)f,
                   (unsigned long)(kbs / 1024u), (unsigned long)((kbs % 1024u) * 100u / 1024u),
                   good ? "OK" : "BAD");
        }
    }
    for (uint32_t f = 0; f < nfiles; f++) {
        snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_str_%lu.bin", (unsigned long)f);
        unlink(path);
    }
    vPortFree(buf);
    if (!wok) wmin = 0;
    if (!rok) rmin = 0;
    printf("pio_sdh: fstress_batch DONE: W %lu/%lu [%lu.%02lu..%lu.%02lu], "
           "R %lu/%lu [%lu.%02lu..%lu.%02lu] MB/s, verify %lu/%lu\n",
           (unsigned long)wok, (unsigned long)nfiles,
           (unsigned long)(wmin / 1024u), (unsigned long)((wmin % 1024u) * 100u / 1024u),
           (unsigned long)(wmax / 1024u), (unsigned long)((wmax % 1024u) * 100u / 1024u),
           (unsigned long)rok, (unsigned long)nfiles,
           (unsigned long)(rmin / 1024u), (unsigned long)((rmin % 1024u) * 100u / 1024u),
           (unsigned long)(rmax / 1024u), (unsigned long)((rmax % 1024u) * 100u / 1024u),
           (unsigned long)vok, (unsigned long)nfiles);
}

static void do_fstress_mix(uint32_t secs)
{
    static const uint32_t mix[] = { 1024u, 64u, 16u, 4u, 1u };  /* 64M 4M 1M 256K 64K (in 64K chunks) */
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { printf("pio_sdh: fstress_mix no memory\n"); return; }
    uint32_t fid = 0, cyc = 0, vbad = 0; int failed = 0;
    char path[48];
    uint64_t t0 = mtimer_get_time_us();
    uint64_t budget = (uint64_t)secs * 1000000ULL;
    printf("pio_sdh: fstress_mix START %lus (large+small write/verify/delete soak)\n", (unsigned long)secs);
    strncpy((char *)s_status.phase, "mix soak", sizeof(s_status.phase) - 1);
    s_status.running = true; s_status.total_mb = 0;
    while (!failed && (mtimer_get_time_us() - t0) < budget) {
        for (uint32_t m = 0; m < sizeof(mix) / sizeof(mix[0]) && !failed; m++) {
            uint32_t chunks = mix[m];
            snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_mix_%lu.bin", (unsigned long)(fid & 7u));
            int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
            if (fd < 0) { printf("pio_sdh: mix W open %d\n", fd); failed = 1; break; }
            for (uint32_t c = 0; c < chunks && !failed; c++) {
                uint32_t seed = fid * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);
                if (write(fd, buf, CHUNK_BYTES) != (int)CHUNK_BYTES) failed = 1;
            }
            close(fd);
            if (failed) { printf("pio_sdh: mix WRITE-FAIL fid=%lu\n", (unsigned long)fid); break; }
            fd = open(path, O_RDONLY);
            if (fd < 0) { printf("pio_sdh: mix R open %d\n", fd); failed = 1; break; }
            long bad = -1;
            for (uint32_t c = 0; c < chunks; c++) {
                if (read(fd, buf, CHUNK_BYTES) != (int)CHUNK_BYTES) { bad = 0; break; }
                uint32_t seed = fid * 0x9E3779B1u + c * 0x85EBCA77u;
                for (uint32_t i = 0; i < CHUNK_BYTES; i++)
                    if (buf[i] != (uint8_t)(seed + i)) { bad = (long)(c * CHUNK_BYTES + i); break; }
                if (bad >= 0) break;
            }
            close(fd);
            if (bad >= 0) { vbad++; printf("pio_sdh: mix VERIFY-BAD fid=%lu @%ld\n", (unsigned long)fid, bad); }
            unlink(path); fid++;
        }
        cyc++;
        uint32_t el = (uint32_t)((mtimer_get_time_us() - t0) / 1000000ULL);
        s_status.done_mb = el;   /* seconds elapsed, shown by `status` */
        if ((cyc % 8u) == 0u)
            printf("  mix cyc=%lu  t=%lus  files=%lu  bad=%lu\n",
                   (unsigned long)cyc, (unsigned long)el, (unsigned long)fid, (unsigned long)vbad);
    }
    for (uint32_t k = 0; k < 8u; k++) {
        snprintf(path, sizeof(path), PIO_MOUNT_POINT "/pio_mix_%lu.bin", (unsigned long)k);
        unlink(path);
    }
    s_status.running = false;
    vPortFree(buf);
    uint32_t el = (uint32_t)((mtimer_get_time_us() - t0) / 1000000ULL);
    printf("pio_sdh: fstress_mix DONE cyc=%lu files=%lu elapsed=%lus verify-fail=%lu\n",
           (unsigned long)cyc, (unsigned long)fid, (unsigned long)el, (unsigned long)vbad);
}

/* DUAL-MOUNT: interleave 64 KiB writes across BOTH /sdcard (HW) and /piosd
 * (PIO), then read+verify both. Prereq: `sdcard` AND `pio_sdcard mount`. */
static void do_dual(uint32_t mb)
{
    static const char *paths[2] = { "/sdcard/dual_hw.bin", PIO_MOUNT_POINT "/dual_pio.bin" };
    static const char *names[2] = { "HW /sdcard", "PIO " PIO_MOUNT_POINT };
    uint32_t chunks = mb * (1024u * 1024u / CHUNK_BYTES);
    if (!chunks) chunks = 1;
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { printf("pio_sdh: dual no memory\n"); return; }

    int fd[2];
    fd[0] = open(paths[0], O_CREAT | O_TRUNC | O_RDWR);
    fd[1] = open(paths[1], O_CREAT | O_TRUNC | O_RDWR);
    if (fd[0] < 0 || fd[1] < 0) {
        printf("pio_sdh: dual open failed (hw=%d pio=%d) - mount BOTH first:\n"
               "         `sdcard` and `pio_sdcard mount`\n", fd[0], fd[1]);
        if (fd[0] >= 0) close(fd[0]); if (fd[1] >= 0) close(fd[1]);
        vPortFree(buf); return;
    }
    printf("pio_sdh: dual %lu MB/host, interleaved: WRITE...\n", (unsigned long)mb);
    prog_t p; prog_begin(&p, "dual write", mb * 2u);   /* both hosts combined */
    uint64_t wio[2] = { 0, 0 }; int failed = -1;
    for (uint32_t c = 0; c < chunks && failed < 0; c++) {
        for (int h = 0; h < 2; h++) {
            uint32_t seed = (uint32_t)h * 0xB5297A4Du + c * 0x85EBCA77u;
            for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);
            uint64_t t = mtimer_get_time_us();
            int r = write(fd[h], buf, CHUNK_BYTES);
            wio[h] += mtimer_get_time_us() - t;
            if (r != (int)CHUNK_BYTES) { failed = h; break; }
        }
        prog_tick(&p, (uint64_t)(c + 1) * CHUNK_BYTES * 2u, wio[0] + wio[1]);
    }
    for (int h = 0; h < 2; h++) close(fd[h]);
    (void)prog_end(&p, (uint64_t)chunks * CHUNK_BYTES * 2u, wio[0] + wio[1]);
    if (failed >= 0) { printf("pio_sdh: dual WRITE-FAIL on %s\n", names[failed]);
                       unlink(paths[0]); unlink(paths[1]); vPortFree(buf); return; }

    fd[0] = open(paths[0], O_RDONLY);
    fd[1] = open(paths[1], O_RDONLY);
    printf("pio_sdh: dual READ+VERIFY...\n");
    prog_t pr; prog_begin(&pr, "dual read", mb * 2u);   /* both hosts combined */
    uint64_t rio[2] = { 0, 0 }; long bad[2] = { -1, -1 }; uint32_t got[2] = { 0, 0 };
    for (uint32_t c = 0; c < chunks; c++) {
        for (int h = 0; h < 2; h++) {
            if (bad[h] >= 0) continue;
            uint64_t t = mtimer_get_time_us();
            int r = read(fd[h], buf, CHUNK_BYTES);
            rio[h] += mtimer_get_time_us() - t;
            if (r != (int)CHUNK_BYTES) { bad[h] = (long)(c * CHUNK_BYTES); continue; }
            got[h]++;
            uint32_t seed = (uint32_t)h * 0xB5297A4Du + c * 0x85EBCA77u;
            for (uint32_t i = 0; i < CHUNK_BYTES; i++)
                if (buf[i] != (uint8_t)(seed + i)) { bad[h] = (long)(c * CHUNK_BYTES + i); break; }
        }
        prog_tick(&pr, (uint64_t)(c + 1) * CHUNK_BYTES * 2u, rio[0] + rio[1]);
    }
    for (int h = 0; h < 2; h++) { close(fd[h]); if (!rio[h]) rio[h] = 1; if (!wio[h]) wio[h] = 1; }
    (void)prog_end(&pr, (uint64_t)chunks * CHUNK_BYTES * 2u, rio[0] + rio[1]);
    uint32_t kib = chunks * (CHUNK_BYTES / 1024u);
    for (int h = 0; h < 2; h++)
        printf("  %s: write %lu.%02lu MB/s, read %lu.%02lu MB/s, verify=%s\n", names[h],
               (unsigned long)(((uint64_t)kib * 1000000ULL / wio[h]) / 1024u),
               (unsigned long)((((uint64_t)kib * 1000000ULL / wio[h]) % 1024u) * 100u / 1024u),
               (unsigned long)(((uint64_t)got[h] * (CHUNK_BYTES / 1024u) * 1000000ULL / rio[h]) / 1024u),
               (unsigned long)((((uint64_t)got[h] * (CHUNK_BYTES / 1024u) * 1000000ULL / rio[h]) % 1024u) * 100u / 1024u),
               (bad[h] < 0 && got[h] == chunks) ? "OK" : "BAD");
    unlink(paths[0]); unlink(paths[1]); vPortFree(buf);
    printf("pio_sdh: dual %s\n",
           (bad[0] < 0 && bad[1] < 0 && got[0] == chunks && got[1] == chunks)
               ? "BOTH HOSTS VERIFIED" : "FAILURES above");
}

/* ==================================================================== */
/*  Concurrent dual-card test - each card on its OWN task, both running */
/*  AT THE SAME TIME. Models the real use case: SDIO WiFi (HW SDH) RX    */
/*  while PIO SD writes the downloaded file. Answers "does the PIO       */
/*  busy-wait starve the other host, and is data safe?"                  */
/* ==================================================================== */

/* Global FS lock for the concurrent test only. FatFS is built FF_FS_REENTRANT=0
 * (not thread-safe), so two tasks in FatFS at once corrupt shared state. This
 * serialises the FS syscalls to keep data safe. NOTE: the REAL use case (SDIO
 * WiFi RX + PIO FatFS write) has only ONE FatFS user - the WiFi side uses
 * network/SDIO buffers, not FatFS - so this lock is a test artifact, not a
 * production requirement. */
static SemaphoreHandle_t s_fs_lock;
static StaticSemaphore_t s_fs_lock_buf;
static void fs_lock(void)   { if (s_fs_lock) xSemaphoreTake(s_fs_lock, portMAX_DELAY); }
static void fs_unlock(void) { if (s_fs_lock) xSemaphoreGive(s_fs_lock); }

/* One card's concurrent worker: write a big file + read-back verify on its
 * own volume, pure-I/O timing. Bulk file (like a WiFi download), no small-file
 * metadata churn. Each FS syscall is serialised via the global FS lock. */
static void conc_run(const char *mnt, const char *tag, uint32_t mb,
                     uint32_t *out_wkbs, uint32_t *out_rkbs, bool *out_ok)
{
    char path[56];
    snprintf(path, sizeof(path), "%s/conc.bin", mnt);
    uint32_t chunks = mb * (1024u * 1024u / CHUNK_BYTES);
    if (!chunks) chunks = 1;
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { *out_wkbs = *out_rkbs = 0; *out_ok = false; return; }

    fs_lock(); int fd = open(path, O_CREAT | O_TRUNC | O_RDWR); fs_unlock();
    uint64_t wio = 0; uint32_t done = 0; TickType_t tk = xTaskGetTickCount();
    if (fd >= 0) {
        for (uint32_t c = 0; c < chunks; c++) {
            uint32_t seed = c * 0x85EBCA77u;
            for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);
            uint64_t t = mtimer_get_time_us();
            fs_lock(); int r = write(fd, buf, CHUNK_BYTES); fs_unlock();
            wio += mtimer_get_time_us() - t;
            if (r != (int)CHUNK_BYTES) break;
            done++;
            if ((xTaskGetTickCount() - tk) >= pdMS_TO_TICKS(1000)) { tk = xTaskGetTickCount();
                printf("    %s W %lu/%lu MB\n", tag, (unsigned long)(done / 16u), (unsigned long)mb); }
        }
        uint64_t tc = mtimer_get_time_us(); fs_lock(); close(fd); fs_unlock();
        wio += mtimer_get_time_us() - tc;
    }
    if (!wio) wio = 1;
    *out_wkbs = (uint32_t)((uint64_t)done * CHUNK_BYTES * 1000000ULL / 1024ULL / wio);
    bool ok = (done == chunks);

    uint64_t rio = 0; done = 0; tk = xTaskGetTickCount();
    fs_lock(); fd = open(path, O_RDONLY); fs_unlock();
    if (fd >= 0) {
        for (uint32_t c = 0; c < chunks; c++) {
            uint64_t t = mtimer_get_time_us();
            fs_lock(); int r = read(fd, buf, CHUNK_BYTES); fs_unlock();
            rio += mtimer_get_time_us() - t;
            if (r != (int)CHUNK_BYTES) { ok = false; break; }
            uint32_t seed = c * 0x85EBCA77u;
            for (uint32_t i = 0; i < CHUNK_BYTES; i++)
                if (buf[i] != (uint8_t)(seed + i)) { ok = false; break; }
            if (!ok) break;
            done++;
            if ((xTaskGetTickCount() - tk) >= pdMS_TO_TICKS(1000)) { tk = xTaskGetTickCount();
                printf("    %s R %lu/%lu MB\n", tag, (unsigned long)(done / 16u), (unsigned long)mb); }
        }
        fs_lock(); close(fd); fs_unlock();
    } else ok = false;
    if (!rio) rio = 1;
    *out_rkbs = (uint32_t)((uint64_t)done * CHUNK_BYTES * 1000000ULL / 1024ULL / rio);
    if (done != chunks) ok = false;
    fs_lock(); unlink(path); fs_unlock();
    vPortFree(buf);
    *out_ok = ok;
}

typedef struct {
    const char *mnt, *tag; uint32_t mb;
    uint32_t wkbs, rkbs; bool ok;
    SemaphoreHandle_t go;
} conc_arg_t;

static conc_arg_t       s_ca[2];
static SemaphoreHandle_t s_conc_fin;
static StaticSemaphore_t s_go_buf[2], s_fin_buf;
static StackType_t       s_conc_stack[2][2048];
static StaticTask_t      s_conc_tcb[2];
static bool              s_conc_created;

static void conc_task(void *pv)
{
    conc_arg_t *a = pv;
    for (;;) {
        xSemaphoreTake(a->go, portMAX_DELAY);
        conc_run(a->mnt, a->tag, a->mb, &a->wkbs, &a->rkbs, &a->ok);
        xSemaphoreGive(s_conc_fin);
    }
}

static void conc_ensure(void)
{
    if (s_conc_created) return;
    s_fs_lock = xSemaphoreCreateMutexStatic(&s_fs_lock_buf);
    s_conc_fin = xSemaphoreCreateCountingStatic(2, 0, &s_fin_buf);
    s_ca[0].mnt = PIO_MOUNT_POINT; s_ca[0].tag = "PIO";
    s_ca[1].mnt = "/sdcard";       s_ca[1].tag = "HW ";
    for (int i = 0; i < 2; i++) {
        s_ca[i].go = xSemaphoreCreateBinaryStatic(&s_go_buf[i]);
        xTaskCreateStatic(conc_task, s_ca[i].tag, 2048, &s_ca[i], 1,
                          s_conc_stack[i], &s_conc_tcb[i]);
    }
    s_conc_created = true;
}

/* Run both cards' big-file suite at the SAME TIME (two tasks), then report
 * each side's concurrent throughput vs the solo baseline + aggregate + CPU. */
static void do_concurrent(uint32_t mb)
{
    if (!fs_ready(PIO_MOUNT_POINT)) {
        printf("pio_sdh: /piosd not mounted - run `pio_sdcard format` or `mount`.\n"); return;
    }
    if (!fs_ready("/sdcard")) {
        printf("pio_sdh: /sdcard not mounted - run `sdcard` first (HW SDH host).\n"); return;
    }
    conc_ensure();
    s_ca[0].mb = s_ca[1].mb = mb;

    printf("\n========= Concurrent Dual-Card (%lu MB/host) =========\n", (unsigned long)mb); bl();
    printf(" PIO /piosd and HW SDH /sdcard write+read AT THE SAME TIME\n"); bl();
    printf(" (models: SDIO WiFi RX on HW SDH while PIO SD stores the file)\n"); bl();
    printf("-----------------------------------------------------\n"); bl();

    uint32_t t0 = (uint32_t)(mtimer_get_time_us() / 1000u);
    strncpy((char *)s_status.phase, "concurrent", sizeof(s_status.phase) - 1);
    s_status.running = true; s_status.total_mb = mb * 2u;

    xSemaphoreGive(s_ca[0].go);          /* launch both as close together as we can */
    xSemaphoreGive(s_ca[1].go);
    xSemaphoreTake(s_conc_fin, portMAX_DELAY);   /* wait for both to finish */
    xSemaphoreTake(s_conc_fin, portMAX_DELAY);

    uint32_t wall = (uint32_t)(mtimer_get_time_us() / 1000u) - t0;
    s_status.running = false;

    char a[16], b[16], c[16], d[16];
    kbs_str(s_ca[0].wkbs, a, sizeof(a)); kbs_str(s_ca[0].rkbs, b, sizeof(b));
    kbs_str(s_ca[1].wkbs, c, sizeof(c)); kbs_str(s_ca[1].rkbs, d, sizeof(d));
    uint32_t agg_w = s_ca[0].wkbs + s_ca[1].wkbs, agg_r = s_ca[0].rkbs + s_ca[1].rkbs;
    char aw[16], ar[16]; kbs_str(agg_w, aw, sizeof(aw)); kbs_str(agg_r, ar, sizeof(ar));

    printf("-----------------------------------------------------\n"); bl();
    printf(" Concurrent throughput (both active at once):\n"); bl();
    printf("   PIO  /piosd : write %-11s read %-11s %s\n", a, b, s_ca[0].ok ? "OK" : "FAIL"); bl();
    printf("   HW   /sdcard: write %-11s read %-11s %s\n", c, d, s_ca[1].ok ? "OK" : "FAIL"); bl();
    printf(" Aggregate    : write %-11s read %s\n", aw, ar); bl();
    printf(" Solo baseline: PIO ~7.0/9.8,  HW ~7.5/10.4 MB/s (write/read)\n"); bl();
    printf(" Wall: %lu.%lus. Both verified => two cards coexist safely (FS-locked).\n",
           (unsigned long)(wall / 1000u), (unsigned long)((wall % 1000u) / 100u)); bl();
    printf(" CPU: both FS drivers busy-wait here, so it stays ~100%% - NOT the\n"); bl();
    printf("   WiFi story. Real SDIO WiFi is IRQ-driven (yields): the CPU idles\n"); bl();
    printf("   during RX. Run WiFi at a HIGHER priority than the PIO write worker\n"); bl();
    printf("   so RX preempts the busy-wait; measure CPU idle in the real system.\n"); bl();
    printf(" RESULT: %s\n", (s_ca[0].ok && s_ca[1].ok) ? "BOTH VERIFIED" : "FAIL - see above"); bl();
    printf("=====================================================\n");
}

/* ==================================================================== */
/*  Audio-headroom test - can the CPU still play music while PIO writes? */
/* ==================================================================== */

/* A HIGH-priority periodic task that burns `pct`% of every 10 ms frame (models
 * MP3 decode + I2S buffer refill), counting frames + deadline misses. It sits
 * ABOVE the PIO write worker (prio 1), so it preempts the PIO busy-wait -
 * audio is never starved. The cost shows up as reduced PIO write throughput
 * (the low-prio writer gets the leftover CPU). */
#define AUDIO_PERIOD_US 10000u
static volatile bool     s_audio_run;
static volatile uint32_t s_audio_frames, s_audio_missed, s_audio_pct;
static StackType_t       s_audio_stack[512];
static StaticTask_t      s_audio_tcb;
static bool              s_audio_created;

static void audio_task(void *pv)
{
    (void)pv;
    for (;;) {
        if (!s_audio_run) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }
        uint64_t start = mtimer_get_time_us();
        uint64_t deadline = start + AUDIO_PERIOD_US;
        uint32_t work = AUDIO_PERIOD_US * s_audio_pct / 100u;
        while (mtimer_get_time_us() - start < work) { __asm__ volatile("nop"); } /* decode */
        s_audio_frames++;
        uint64_t now = mtimer_get_time_us();
        if (now >= deadline) { s_audio_missed++; }   /* frame overran = audio glitch */
        else {
            uint32_t rem_ms = (uint32_t)((deadline - now) / 1000u);
            vTaskDelay(rem_ms ? pdMS_TO_TICKS(rem_ms) : 1);
        }
    }
}

static void audio_ensure(void)
{
    if (s_audio_created) return;
    xTaskCreateStatic(audio_task, "audio", sizeof(s_audio_stack) / sizeof(StackType_t),
                      NULL, 5, s_audio_stack, &s_audio_tcb);   /* prio 5 > PIO worker 1 */
    s_audio_created = true;
}

/* Write `mb` MB paced to `mbps` MB/s (models a steady WiFi feed). Returns the
 * sustained rate (KiB/s) actually achieved and whether every chunk wrote OK. */
static void paced_write(uint32_t mbps, uint32_t mb, const char *tag,
                        uint32_t *out_kbs, bool *out_ok)
{
    static const char *path = PIO_MOUNT_POINT "/audio.bin";
    uint32_t chunks = mb * (1024u * 1024u / CHUNK_BYTES);
    if (!chunks) chunks = 1;
    uint32_t budget_us = (uint32_t)((uint64_t)CHUNK_BYTES * 1000000ULL /
                                    ((uint64_t)mbps * 1024ULL * 1024ULL));
    uint8_t *buf = pvPortMalloc(CHUNK_BYTES);
    if (!buf) { *out_kbs = 0; *out_ok = false; return; }
    int fd = open(path, O_CREAT | O_TRUNC | O_RDWR);
    if (fd < 0) { *out_kbs = 0; *out_ok = false; vPortFree(buf); return; }

    uint32_t t0 = (uint32_t)(mtimer_get_time_us() / 1000u);
    uint32_t done = 0; bool ok = true; TickType_t tk = xTaskGetTickCount();
    for (uint32_t c = 0; c < chunks; c++) {
        uint64_t cs = mtimer_get_time_us();
        uint32_t seed = c * 0x85EBCA77u;
        for (uint32_t i = 0; i < CHUNK_BYTES; i++) buf[i] = (uint8_t)(seed + i);
        if (write(fd, buf, CHUNK_BYTES) != (int)CHUNK_BYTES) { ok = false; break; }
        done++;
        uint64_t used = mtimer_get_time_us() - cs;
        if (used < budget_us) {                       /* pace: idle to the feed rate */
            uint32_t rem_ms = (uint32_t)((budget_us - used) / 1000u);
            vTaskDelay(rem_ms ? pdMS_TO_TICKS(rem_ms) : 1);
        }
        s_status.done_mb = done / 16u;
        if ((xTaskGetTickCount() - tk) >= pdMS_TO_TICKS(1000)) { tk = xTaskGetTickCount();
            printf("    %s %lu/%lu MB\n", tag, (unsigned long)(done / 16u), (unsigned long)mb); }
    }
    close(fd); unlink(path);
    uint32_t wall = (uint32_t)(mtimer_get_time_us() / 1000u) - t0;
    *out_kbs = wall ? (uint32_t)((uint64_t)done * CHUNK_BYTES / 1024ULL * 1000ULL / wall) : 0;
    *out_ok = ok && (done == chunks);
    vPortFree(buf);
}

/* Coexistence test: a paced write (models WiFi download -> PIO SD store) run
 * alongside music playback (high-prio audio load). Shows that the sustained
 * write rate holds, the audio never misses a frame deadline, and - critically -
 * that the wauto write path survives preemption where the CPU-paced path can
 * fail. This is about write robustness under real-time load, not CPU usage. */
static void do_audiotest(uint32_t pct, uint32_t mbps, uint32_t mb)
{
    if (!fs_ready(PIO_MOUNT_POINT)) {
        printf("pio_sdh: /piosd not mounted - run `pio_sdcard format` or `mount`.\n"); return;
    }
    if (pct > 95u) pct = 95u;
    if (mbps < 1u) mbps = 1u;
    audio_ensure();

    printf("\n===== Coexistence: feed + store + real-time work =====\n"); bl();
    printf(" Feed  : %lu MB/s paced write (models WiFi download -> PIO SD)\n", (unsigned long)mbps); bl();
    printf(" Load  : %lu%% duty @ 10 ms, prio 5 (stands in for audio + WiFi + app)\n", (unsigned long)pct); bl();
    printf(" Total : %lu MB written per phase\n", (unsigned long)mb); bl();
    printf("------------------------------------------------------\n"); bl();

    uint32_t b_kbs, c_kbs, w_kbs; bool b_ok, c_ok, w_ok;

    printf(" [1/3] paced feed, no music:\n"); bl();
    s_audio_run = false; hal_pio_sdh_set_wauto(0, NULL);
    paced_write(mbps, mb, "feed", &b_kbs, &b_ok);

    printf(" [2/3] paced feed + music, CPU-paced write:\n"); bl();
    s_audio_pct = pct; s_audio_frames = 0; s_audio_missed = 0; s_audio_run = true;
    vTaskDelay(pdMS_TO_TICKS(50));
    hal_pio_sdh_set_wauto(0, NULL);
    paced_write(mbps, mb, "feed", &c_kbs, &c_ok);
    uint32_t m2 = s_audio_missed;

    printf(" [3/3] paced feed + music, wauto write:\n"); bl();
    s_audio_frames = 0; s_audio_missed = 0;
    hal_pio_sdh_set_wauto(1, NULL);
    paced_write(mbps, mb, "feed", &w_kbs, &w_ok);
    uint32_t m3 = s_audio_missed;
    hal_pio_sdh_set_wauto(0, NULL);
    s_audio_run = false;

    char bs[16], cs[16], ws[16];
    kbs_str(b_kbs, bs, sizeof(bs)); kbs_str(c_kbs, cs, sizeof(cs)); kbs_str(w_kbs, ws, sizeof(ws));

    printf("------------------------------------------------------\n"); bl();
    printf(" Scenario                    sustained   audio    write\n"); bl();
    printf(" feed only ................. %-10s  -        %s\n",
           bs, b_ok ? "OK" : "FAIL"); bl();
    printf(" feed + load (CPU-paced) ... %-10s  %lu miss   %s\n",
           cs, (unsigned long)m2, c_ok ? "OK" : "WRITE-FAIL"); bl();
    printf(" feed + load (wauto) ....... %-10s  %lu miss   %s\n",
           ws, (unsigned long)m3, w_ok ? "OK" : "WRITE-FAIL"); bl();
    printf("------------------------------------------------------\n"); bl();
    if (c_kbs / 1024u + 1u < mbps) {
        uint32_t cap_kbs = (b_kbs > c_kbs) ? b_kbs : c_kbs;   /* best capacity seen */
        uint32_t suggest = cap_kbs * 7u / 10u / 1024u;        /* 70% of capacity, MB/s */
        if (!suggest) suggest = 1u;
        printf(" NOTE: card sustains only ~%s, below the %lu MB/s feed -> the SD\n",
               cs, (unsigned long)mbps); bl();
        printf("   write can't keep up. Use a feed below capacity, e.g.\n"); bl();
        printf("   `pio_sdcard audiotest %lu %lu`, or `format`/fresh card to restore ~7 MB/s.\n",
               (unsigned long)pct, (unsigned long)suggest); bl();
    } else {
        printf(" Music/RT work never glitches (higher prio); the write is what must\n"); bl();
        printf("   be protected. Use `wauto` - it survives preemption; the CPU-paced\n"); bl();
        printf("   path can hit WRITE-FAIL under a heavy real-time load.\n"); bl();
    }
    printf("======================================================\n");
}

/* ==================================================================== */
/*  Worker task + job dispatch                                          */
/* ==================================================================== */

typedef enum {
    OP_INFO, OP_FORMAT, OP_MOUNT, OP_BENCHMARK, OP_FSPEED,
    OP_FSTRESS, OP_FSTRESS_BATCH, OP_FSTRESS_MIX, OP_DUAL, OP_CONCURRENT,
    OP_AUDIOTEST
} pio_op_t;

typedef struct { pio_op_t op; char mode; uint32_t a, b, c; } pio_job_t;

#define JOB_QUEUE_DEPTH    4
#define WORKER_STACK_WORDS 2048

static QueueHandle_t s_job_q;
static StaticQueue_t s_job_q_buf;
static uint8_t       s_job_q_storage[JOB_QUEUE_DEPTH * sizeof(pio_job_t)];
static StackType_t   s_worker_stack[WORKER_STACK_WORDS];
static StaticTask_t  s_worker_tcb;

static void pio_worker_task(void *pv)
{
    (void)pv;
    pio_job_t j;
    for (;;) {
        if (xQueueReceive(s_job_q, &j, portMAX_DELAY) != pdTRUE) continue;
        switch (j.op) {
            case OP_INFO:          do_info();                    break;
            case OP_FORMAT:        do_format();                  break;
            case OP_MOUNT:         do_mount();                   break;
            case OP_BENCHMARK:     do_benchmark(j.a, j.mode == 'd'); break;
            case OP_FSPEED:        do_fspeed(j.a);               break;
            case OP_FSTRESS:       do_fstress(j.mode, j.a, j.b); break;
            case OP_FSTRESS_BATCH: do_fstress_batch(j.a, j.b);   break;
            case OP_FSTRESS_MIX:   do_fstress_mix(j.a);          break;
            case OP_DUAL:          do_dual(j.a);                 break;
            case OP_CONCURRENT:    do_concurrent(j.a);           break;
            case OP_AUDIOTEST:     do_audiotest(j.a, j.c, j.b);  break;
            default:                                             break;
        }
    }
}

static int worker_ensure(void)
{
    if (s_job_q) return 0;
    s_job_q = xQueueCreateStatic(JOB_QUEUE_DEPTH, sizeof(pio_job_t),
                                 s_job_q_storage, &s_job_q_buf);
    if (!s_job_q) return -1;
    xTaskCreateStatic(pio_worker_task, "pio_wrk", WORKER_STACK_WORDS,
                      NULL, 1, s_worker_stack, &s_worker_tcb);
    return 0;
}

static void dispatch(const pio_job_t *j)
{
    if (worker_ensure() != 0) { printf("pio_sdh: worker init failed\n"); return; }
    if (xQueueSend(s_job_q, j, 0) != pdTRUE)
        printf("pio_sdh: BUSY - a test is running (see `pio_sdcard status`); try again after it ends\n");
}

/* ==================================================================== */
/*  Shell command - parse + dispatch (heavy work runs in the worker)    */
/* ==================================================================== */

static void print_usage(void)
{
    printf("PIO SD host (GPIO22-27) - demo / evaluation / benchmark\n");
    printf("  pio_sdcard info                 identify the card\n");
    printf("  pio_sdcard format               fresh exFAT + mount   (ERASES CARD)\n");
    printf("  pio_sdcard mount                mount an existing filesystem\n");
    printf("  pio_sdcard benchmark [mb=128] [dual]   full suite + report\n");
    printf("                                  `dual` = PIO vs HW SDH side-by-side (needs `sdcard`)\n");
    printf("  pio_sdcard status               is a test running? live progress\n");
    printf("  --- advanced ---\n");
    printf("  pio_sdcard fspeed [kb=2048] | fstress <w|r|x> <idx> [mb]\n");
    printf("  pio_sdcard fstress_batch <n> [mb] | fstress_mix [secs]\n");
    printf("  pio_sdcard dual [mb]        interleaved both cards (1 thread)\n");
    printf("  pio_sdcard concurrent [mb]  both cards on SEPARATE tasks at once (needs `sdcard`)\n");
    printf("  pio_sdcard audiotest [music%%=25] [feed_mbps=4] [mb=64]   WiFi-feed + store + music\n");
    printf("  pio_sdcard pad <drive 0-3> <pull 0|1> [smt 0|1]\n");
    printf("  dual-mount: HW SDH -> /sdcard (`sdcard`),  PIO -> %s\n", PIO_MOUNT_POINT);
}

static int cmd_pio_sdcard(int argc, char **argv)
{
    if (argc < 2) { print_usage(); return 0; }

    /* --- inline commands (run in the shell thread; no card/FS heavy work) --- */
    if (strcmp(argv[1], "help") == 0) { print_usage(); return 0; }

    if (strcmp(argv[1], "status") == 0) {
        if (!s_status.running) {
            printf("pio_sdh: idle (no test running)\n");
        } else if (s_status.total_mb) {
            printf("pio_sdh: RUNNING [%s] %lu/%lu MB  %lu.%02lu MB/s\n",
                   (char *)s_status.phase, (unsigned long)s_status.done_mb,
                   (unsigned long)s_status.total_mb,
                   (unsigned long)(s_status.cur_kbs / 1024u),
                   (unsigned long)((s_status.cur_kbs % 1024u) * 100u / 1024u));
        } else {
            printf("pio_sdh: RUNNING [%s] %lus elapsed\n",
                   (char *)s_status.phase, (unsigned long)s_status.done_mb);
        }
        return 0;
    }

    if (strcmp(argv[1], "wauto") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1u;
        uint32_t st = 0;
        hal_pio_sdh_set_wauto(on, &st);
        printf("pio_sdh: write path = %s (last CRC status 0x%lx)\n",
               on ? "SM-autonomous (preemption-tolerant)" : "CPU-paced (default)",
               (unsigned long)st);
        return 0;
    }

    /* Engineering diagnostic (not in `help`): disables the post-write card-ready
     * handshake to A/B the single-block write path. Leave at the default 1. */
    if (strcmp(argv[1], "settle") == 0) {
        uint8_t on = (argc > 2) ? (uint8_t)atoi(argv[2]) : 1u;
        hal_pio_sdh_set_wr1_settle(on);
        printf("pio_sdh: post-write card-ready handshake = %s%s\n",
               on ? "ON" : "OFF",
               on ? " (default)" : " - DIAGNOSTIC ONLY, writes may fail");
        return 0;
    }

    if (strcmp(argv[1], "pad") == 0) {
        if (argc < 4) {
            printf("pio_sdh: usage: pad <drive 0-3> <pull 0|1> [smt 0|1]\n");
            printf("         pull 0 = internal pulls OFF (adapter has external pull-ups)\n");
            return 0;
        }
        uint8_t d = (uint8_t)atoi(argv[2]), p = (uint8_t)atoi(argv[3]);
        uint8_t s = (argc > 4) ? (uint8_t)atoi(argv[4]) : 1u;
        hal_pio_sdh_set_pad_cfg(d, p, s);
        printf("pio_sdh: pads = drive %u, internal pull %s, smt %u (sticky)\n",
               (d > 3u) ? 3u : d, p ? "UP" : "NONE", s ? 1u : 0u);
        return 0;
    }

    /* --- dispatched commands (run in the worker task) --- */
    pio_job_t j = { 0 };
    if (strcmp(argv[1], "info") == 0)              j.op = OP_INFO;
    else if (strcmp(argv[1], "format") == 0)       j.op = OP_FORMAT;
    else if (strcmp(argv[1], "mount") == 0)        j.op = OP_MOUNT;
    else if (strcmp(argv[1], "benchmark") == 0) {  j.op = OP_BENCHMARK; j.a = 128u;
        /* args in any order: "dual" enables PIO-vs-HW mode, a number sets MB */
        for (int k = 2; k < argc; k++) {
            if (strcmp(argv[k], "dual") == 0) j.mode = 'd';
            else j.a = (uint32_t)strtoul(argv[k], NULL, 0);
        } }
    else if (strcmp(argv[1], "fspeed") == 0) {     j.op = OP_FSPEED;
        j.a = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 2048u; }
    else if (strcmp(argv[1], "fstress") == 0) {
        if (argc < 4) { printf("pio_sdh: usage: fstress <w|r|x> <idx> [mb=1024]\n"); return 0; }
        j.op = OP_FSTRESS; j.mode = argv[2][0];
        j.a = (uint32_t)strtoul(argv[3], NULL, 0);
        j.b = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 1024u; }
    else if (strcmp(argv[1], "fstress_batch") == 0) {
        if (argc < 3) { printf("pio_sdh: usage: fstress_batch <nfiles> [mb=64]\n"); return 0; }
        j.op = OP_FSTRESS_BATCH; j.a = (uint32_t)strtoul(argv[2], NULL, 0);
        j.b = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 64u; }
    else if (strcmp(argv[1], "fstress_mix") == 0) { j.op = OP_FSTRESS_MIX;
        j.a = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 3600u; }
    else if (strcmp(argv[1], "dual") == 0) {       j.op = OP_DUAL;
        j.a = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 32u; }
    else if (strcmp(argv[1], "concurrent") == 0) { j.op = OP_CONCURRENT;
        j.a = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 64u; }
    else if (strcmp(argv[1], "audiotest") == 0) { j.op = OP_AUDIOTEST;
        j.a = (argc > 2) ? (uint32_t)strtoul(argv[2], NULL, 0) : 25u;   /* music CPU %% */
        j.c = (argc > 3) ? (uint32_t)strtoul(argv[3], NULL, 0) : 4u;    /* feed rate MB/s */
        j.b = (argc > 4) ? (uint32_t)strtoul(argv[4], NULL, 0) : 64u; } /* total MB */
    else { printf("pio_sdh: unknown '%s' (try `pio_sdcard help`)\n", argv[1]); return 0; }

    dispatch(&j);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_pio_sdcard, pio_sdcard, PIO SD host demo/eval/benchmark);
