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

/**
 * @file sd_cli.c
 * @brief `sd` shell command: mount, list, write/read test, supply gate.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "board_config.h"
#include "ff.h"
#include "sdcard.h"
#include "hal_mtimer.h"

#define SD_TEST_PATH  "sd:/reef_test.txt"
#define SD_BENCH_PATH "sd:/reef_bench.bin"
#define SD_BENCH_CHUNK     4096u
#define SD_BENCH_DEF_KIB   1024u
#define SD_BENCH_MAX_KIB   8192u

static uint8_t s_bench_buf[SD_BENCH_CHUNK] __attribute__((aligned(64)));

static void sd_usage(void)
{
    printf("sd status              detect, mount state, free space\n"
           "sd mount | sd unmount\n"
           "sd ls [path]           directory listing (default sd:/)\n"
           "sd test                write then read back a small file\n"
           "sd bench [kib]         write, read back and verify kib KiB (default %u)\n"
           "sd power on|off|cycle  SD_VDD gate on GPIO%d (JP3 2-3)\n",
           SD_BENCH_DEF_KIB, BOARD_SD_PWR_EN_PIN);
}

static void sd_status(void)
{
    printf("pins    : CLK=%d CMD=%d D0=%d D1=%d D2=%d D3=%d, DET=%d, PWR_EN=%d\n",
           BOARD_SD_CLK_PIN, BOARD_SD_CMD_PIN, BOARD_SD_D0_PIN, BOARD_SD_D1_PIN,
           BOARD_SD_D2_PIN, BOARD_SD_D3_PIN, BOARD_SD_DET_PIN, BOARD_SD_PWR_EN_PIN);
    printf("detect  : GPIO%d reads %d (%s)\n", BOARD_SD_DET_PIN, sdcard_detect_raw(),
           sdcard_present() ? "card or fin seated" : "slot empty");
    printf("supply  : %s (GPIO%d=%d)\n", sdcard_power_is_on() ? "on" : "off",
           BOARD_SD_PWR_EN_PIN, sdcard_power_is_on() ? BOARD_SD_PWR_ON_LEVEL : !BOARD_SD_PWR_ON_LEVEL);
    printf("mounted : %s\n", sdcard_is_mounted() ? "yes" : "no");
    if (sdcard_is_mounted()) {
        uint32_t total = 0, freek = 0;
        if (sdcard_space_kb(&total, &freek) == SD_OK) {
            printf("space   : %u KiB free of %u KiB\n", (unsigned)freek, (unsigned)total);
        }
    }
}

static void sd_ls(const char *path)
{
    DIR      dir;
    FILINFO  fno;
    int      n = 0;

    if (f_opendir(&dir, path) != FR_OK) {
        printf("f_opendir(\"%s\") failed\n", path);
        return;
    }
    for (;;) {
        if (f_readdir(&dir, &fno) != FR_OK || fno.fname[0] == 0) {
            break;
        }
        printf("  %-24s %s%lu\n", fno.fname, (fno.fattrib & AM_DIR) ? "<DIR> " : "",
               (fno.fattrib & AM_DIR) ? 0UL : (unsigned long)fno.fsize);
        n++;
    }
    f_closedir(&dir);
    printf("%d entr%s in %s\n", n, (n == 1) ? "y" : "ies", path);
}

static void sd_test(void)
{
    static const char msg[] = "IPRO7AI Reef sd test\n";
    char    buf[64];
    FIL     f;
    UINT    bw = 0, br = 0;
    FRESULT fr;

    if (!sdcard_is_mounted()) {
        printf("not mounted - run `sd mount` first\n");
        return;
    }
    fr = f_open(&f, SD_TEST_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("open for write failed (FRESULT %d)\n", (int)fr);
        return;
    }
    fr = f_write(&f, msg, sizeof(msg) - 1, &bw);
    f_close(&f);
    if (fr != FR_OK || bw != sizeof(msg) - 1) {
        printf("write failed (FRESULT %d, %u bytes)\n", (int)fr, (unsigned)bw);
        return;
    }
    fr = f_open(&f, SD_TEST_PATH, FA_READ);
    if (fr != FR_OK) {
        printf("open for read failed (FRESULT %d)\n", (int)fr);
        return;
    }
    memset(buf, 0, sizeof(buf));
    fr = f_read(&f, buf, sizeof(buf) - 1, &br);
    f_close(&f);
    if (fr == FR_OK && br == bw && memcmp(buf, msg, bw) == 0) {
        printf("PASS: wrote and read back %u bytes at %s\n", (unsigned)bw, SD_TEST_PATH);
    } else {
        printf("FAIL: FRESULT %d, wrote %u, read %u\n", (int)fr, (unsigned)bw, (unsigned)br);
    }
}

/* Position-dependent pattern: a sector written to or read from the wrong
 * place, or a stuck / swapped data line, shows up as a mismatch. */
static inline uint8_t bench_byte(uint32_t off)
{
    return (uint8_t)(((off * 2654435761u) >> 24) ^ (off >> 9));
}

static unsigned kib_per_s(uint32_t kib, uint32_t us)
{
    return us ? (unsigned)((uint64_t)kib * 1000000u / us) : 0u;
}

static void sd_bench(uint32_t kib)
{
    FIL f;
    FRESULT fr;
    UINT n;
    uint32_t total = kib * 1024u, off, t0, t_wr, t_rd, bad = 0, first_bad = 0;

    if (!sdcard_is_mounted()) {
        printf("not mounted - run `sd mount` first\n");
        return;
    }
    fr = f_open(&f, SD_BENCH_PATH, FA_CREATE_ALWAYS | FA_WRITE);
    if (fr != FR_OK) {
        printf("open for write failed (FRESULT %d)\n", (int)fr);
        return;
    }
    t0 = (uint32_t)mtimer_get_time_us();
    for (off = 0; off < total; off += SD_BENCH_CHUNK) {
        for (uint32_t i = 0; i < SD_BENCH_CHUNK; i++) {
            s_bench_buf[i] = bench_byte(off + i);
        }
        fr = f_write(&f, s_bench_buf, SD_BENCH_CHUNK, &n);
        if (fr != FR_OK || n != SD_BENCH_CHUNK) {
            printf("write failed at %u (FRESULT %d)\n", (unsigned)off, (int)fr);
            f_close(&f);
            return;
        }
    }
    fr = f_close(&f);
    t_wr = (uint32_t)mtimer_get_time_us() - t0;
    if (fr != FR_OK) {
        printf("close after write failed (FRESULT %d)\n", (int)fr);
        return;
    }

    fr = f_open(&f, SD_BENCH_PATH, FA_READ);
    if (fr != FR_OK) {
        printf("open for read failed (FRESULT %d)\n", (int)fr);
        return;
    }
    t0 = (uint32_t)mtimer_get_time_us();
    for (off = 0; off < total; off += SD_BENCH_CHUNK) {
        fr = f_read(&f, s_bench_buf, SD_BENCH_CHUNK, &n);
        if (fr != FR_OK || n != SD_BENCH_CHUNK) {
            printf("read failed at %u (FRESULT %d)\n", (unsigned)off, (int)fr);
            f_close(&f);
            return;
        }
        for (uint32_t i = 0; i < SD_BENCH_CHUNK; i++) {
            if (s_bench_buf[i] != bench_byte(off + i)) {
                if (bad++ == 0) {
                    first_bad = off + i;
                }
            }
        }
    }
    t_rd = (uint32_t)mtimer_get_time_us() - t0;
    f_close(&f);
    f_unlink(SD_BENCH_PATH);

    printf("write %u KiB in %u ms (%u KiB/s), read %u KiB in %u ms (%u KiB/s)\n",
           (unsigned)kib, (unsigned)(t_wr / 1000u), kib_per_s(kib, t_wr),
           (unsigned)kib, (unsigned)(t_rd / 1000u), kib_per_s(kib, t_rd));
    if (bad) {
        printf("FAIL: %u bytes differ, first at offset 0x%x\n", (unsigned)bad, (unsigned)first_bad);
    } else {
        printf("PASS: %u KiB verified\n", (unsigned)kib);
    }
}

static int cmd_sd(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "status") == 0) {
        sd_status();
    } else if (strcmp(argv[1], "mount") == 0) {
        if (sdcard_mount() == SD_OK) {
            printf("mounted at sd:\n");
            sd_status();
        } else {
            printf("mount failed - card seated and formatted? try `sd power cycle`\n");
        }
    } else if (strcmp(argv[1], "unmount") == 0) {
        sdcard_unmount();
        printf("unmounted\n");
    } else if (strcmp(argv[1], "ls") == 0) {
        if (!sdcard_is_mounted()) {
            printf("not mounted - run `sd mount` first\n");
        } else {
            sd_ls((argc > 2) ? argv[2] : "sd:/");
        }
    } else if (strcmp(argv[1], "test") == 0) {
        sd_test();
    } else if (strcmp(argv[1], "bench") == 0) {
        long kib = (argc > 2) ? strtol(argv[2], NULL, 0) : (long)SD_BENCH_DEF_KIB;
        if (kib < 4 || kib > (long)SD_BENCH_MAX_KIB) {
            printf("4..%u KiB\n", SD_BENCH_MAX_KIB);
        } else {
            sd_bench((uint32_t)kib);
        }
    } else if (strcmp(argv[1], "power") == 0 && argc >= 3) {
        if (strcmp(argv[2], "cycle") == 0) {
            sdcard_power_cycle(300);
            printf("cycled\n");
        } else {
            bool on = strcmp(argv[2], "on") == 0;
            sdcard_power(on);
            printf("SD supply %s\n", on ? "on" : "off");
        }
    } else {
        sd_usage();
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sd, sd, SD slot on the SDH controller);

#endif /* CONFIG_SHELL */
