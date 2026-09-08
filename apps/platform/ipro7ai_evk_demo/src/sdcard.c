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
 * @file sdcard.c
 * @brief micro-SD + FatFS, and the `sd` shell command. See sdcard.h.
 */

#include "sdcard.h"
#include "board_config.h"

#include <string.h>
#include <stdio.h>

#include "hal_gpio.h"
#include "drv_gpio.h"
#include "drv_glb.h"
#include "ff.h"
#include "hal_mtimer.h"
#include "fatfs_diskio_register.h"
#include "ipro_log.h"

#define LOG_TAG "SD"

/* Registered by components/fs/fatfs/port/fatfs_sdh_sdcard.c. */
extern void fatfs_sdh_driver_register(void);

static FATFS s_fatfs;
static bool  s_mounted;
static bool  s_driver_registered;

int sdcard_mount(void)
{
    FRESULT fr;

    if (s_mounted) {
        return SD_OK;
    }
    if (!s_driver_registered) {
        /*
         * Ask for 4-bit before the driver registers.
         *
         * fatfs_sdh_sdcard.c defaults sdio_bus_width to 1-bit and nothing was
         * calling the setter, so the card ran at a quarter of its bandwidth
         * even though it reports "4-line supported[YES]" during init - the
         * mount log then says "Current is 1-line mode" and that is easy to
         * read past.
         *
         * It is not just slow, it is out of headroom: 1-bit at 25 MHz tops out
         * near 3.1 MB/s and sustained video playback measured 2.87 MB/s, so
         * any pause the card takes for its own housekeeping overran the
         * driver's read timeout and the transfer failed outright. All four
         * data lines are wired on this board (D0=29 D1=28 D2=33 D3=32).
         *
         * SDH_SDCardInit() still falls back to 1-bit on its own if the card
         * turns out not to support it, so this is a request and not an
         * assumption.
         */
        hal_sd_bus_4bits_enable();
        fatfs_sdh_driver_register();
        s_driver_registered = true;
    }

    /* Mount immediately (opt = 1) so a missing or unformatted card fails here
     * rather than on the first file operation. */
    fr = f_mount(&s_fatfs, "sd:", 1);
    if (fr != FR_OK) {
        IPRO_LOGE(LOG_TAG, "f_mount failed (FRESULT %d)", (int)fr);
        return SD_ERR_MOUNT;
    }
    s_mounted = true;
    return SD_OK;
}

int sdcard_unmount(void)
{
    if (!s_mounted) {
        return SD_OK;
    }
    f_mount(NULL, "sd:", 0);
    s_mounted = false;
    return SD_OK;
}

bool sdcard_is_mounted(void) { return s_mounted; }

int sdcard_detect_raw(void)
{
    uint8_t pin[1] = { BOARD_SD_DET_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_DET_PIN, GPIO_INPUT_PP_MODE);
    return gpio_read(BOARD_SD_DET_PIN);
}

void sdcard_power(int on)
{
    uint8_t pin[1] = { BOARD_SD_PWR_EN_PIN };
    int level = on ? BOARD_SD_PWR_ON_LEVEL : !BOARD_SD_PWR_ON_LEVEL;

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_SD_PWR_EN_PIN, GPIO_OUTPUT_MODE);
    gpio_write(BOARD_SD_PWR_EN_PIN, (uint32_t)level);
}

int sdcard_space_kb(uint32_t *total_kb, uint32_t *free_kb)
{
    FATFS   *fs;
    DWORD    free_clusters;
    FRESULT  fr;

    if (!s_mounted) {
        return SD_ERR_STATE;
    }
    fr = f_getfree("sd:", &free_clusters, &fs);
    if (fr != FR_OK) {
        return SD_ERR_IO;
    }
    /* Sectors are 512 B, so sectors/2 = KiB. */
    if (total_kb) {
        *total_kb = (uint32_t)(((fs->n_fatent - 2) * fs->csize) / 2);
    }
    if (free_kb) {
        *free_kb = (uint32_t)((free_clusters * fs->csize) / 2);
    }
    return SD_OK;
}

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdlib.h>

#define SD_TEST_PATH  "sd:/evk_test.txt"

static void sd_usage(void)
{
    printf(
      "sd - micro-SD on the SDH controller (GPIO%d-%d), FatFS at \"sd:\"\n"
      "  sd status              card detect, mount state, free space\n"
      "  sd mount | sd unmount\n"
      "  sd ls [path]           directory listing (default sd:/)\n"
      "  sd test                write then read back a small file\n"
      "  sd power <on|off>      SD supply gate on GPIO%d\n",
      BOARD_SD_D1_PIN, BOARD_SD_D2_PIN, BOARD_SD_PWR_EN_PIN);
}

static void sd_status(void)
{
    int det = sdcard_detect_raw();

    printf("pins    : CLK=%d CMD=%d D0=%d D1=%d D2=%d D3=%d, DET=%d, PWR_EN=%d\n",
           BOARD_SD_CLK_PIN, BOARD_SD_CMD_PIN, BOARD_SD_D0_PIN,
           BOARD_SD_D1_PIN, BOARD_SD_D2_PIN, BOARD_SD_D3_PIN,
           BOARD_SD_DET_PIN, BOARD_SD_PWR_EN_PIN);
    printf("detect  : GPIO%d reads %d (%s)\n", BOARD_SD_DET_PIN, det,
           det ? "no card, if DET is active-low" : "card seated");
    printf("mounted : %s\n", sdcard_is_mounted() ? "yes" : "no");

    if (sdcard_is_mounted()) {
        uint32_t total = 0, freek = 0;
        if (sdcard_space_kb(&total, &freek) == SD_OK) {
            printf("space   : %u KiB free of %u KiB\n",
                   (unsigned)freek, (unsigned)total);
        }
    }
    printf("note    : the board init already muxed GPIO28-33 to SDC and\n"
           "          un-gated the SDH clock, so `sd mount` is all that is\n"
           "          needed. SD_PWR_EN polarity is unconfirmed - if mounting\n"
           "          fails, try `sd power off` then `sd power on`.\n");
}

static void sd_ls(const char *path)
{
    DIR      dir;
    FILINFO  fno;
    FRESULT  fr;
    int      n = 0;

    fr = f_opendir(&dir, path);
    if (fr != FR_OK) {
        printf("f_opendir(\"%s\") failed (FRESULT %d)\n", path, (int)fr);
        return;
    }
    for (;;) {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0) {
            break;
        }
        printf("  %-24s %s%lu\n", fno.fname,
               (fno.fattrib & AM_DIR) ? "<DIR> " : "",
               (fno.fattrib & AM_DIR) ? 0UL : (unsigned long)fno.fsize);
        n++;
    }
    f_closedir(&dir);
    printf("%d entr%s in %s\n", n, (n == 1) ? "y" : "ies", path);
}

static void sd_test(void)
{
    static const char msg[] = "IPRO7AI EVK sd test\n";
    char     buf[64];
    FIL      f;
    UINT     bw = 0, br = 0;
    FRESULT  fr;

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
        printf("write failed (FRESULT %d, %u of %u bytes)\n",
               (int)fr, (unsigned)bw, (unsigned)(sizeof(msg) - 1));
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
    if (fr != FR_OK) {
        printf("read failed (FRESULT %d)\n", (int)fr);
        return;
    }

    if (br == bw && memcmp(buf, msg, bw) == 0) {
        printf("PASS: wrote and read back %u bytes at %s\n",
               (unsigned)bw, SD_TEST_PATH);
    } else {
        printf("MISMATCH: wrote %u bytes, read %u bytes back\n",
               (unsigned)bw, (unsigned)br);
    }
}

/*
 * Read a file straight through with nothing else running.
 *
 * The point is to separate "the card or the driver cannot sustain this" from
 * "it only breaks while the panel is also being driven". SDH_ReadBlocks polls
 * a status register in a loop with no yield, at the same priority as the blit
 * task, so concurrency is a real suspect - and this command removes it.
 */
static int sd_bench(const char *path)
{
    static uint8_t buf[8192];
    FIL fp;
    FRESULT fr;
    UINT got;
    uint64_t t0, dt;
    uint32_t total = 0;

    fr = f_open(&fp, path, FA_READ);
    if (fr != FR_OK) {
        printf("f_open(\"%s\") failed (FRESULT %d)\n", path, (int)fr);
        return -1;
    }

    t0 = mtimer_get_time_us();
    for (;;) {
        fr = f_read(&fp, buf, sizeof(buf), &got);
        if (fr != FR_OK) {
            printf("f_read failed at offset %u (FRESULT %d)\n",
                   (unsigned)total, (int)fr);
            break;
        }
        if (got == 0) {
            break;
        }
        total += got;
    }
    dt = mtimer_get_time_us() - t0;
    f_close(&fp);

    printf("read %u bytes in %u ms", (unsigned)total, (unsigned)(dt / 1000u));
    if (dt) {
        printf(" -> %u KB/s", (unsigned)((uint64_t)total * 1000000u / dt / 1024u));
    }
    printf("\n%s\n", (fr == FR_OK)
           ? "no error - the card sustains this on its own, so a failure "
             "during playback is about what else is running"
           : "failed with nothing else running - this is the card or the "
             "driver, not the player");
    return 0;
}

int cmd_sd(int argc, char **argv)
{
    const char *sub;

    if (argc < 2) {
        sd_usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "status") == 0) {
        sd_status();
    } else if (strcmp(sub, "mount") == 0) {
        if (sdcard_mount() == SD_OK) {
            printf("mounted at sd:\n");
            sd_status();
        } else {
            printf("mount failed - check the card is seated and formatted\n");
        }
    } else if (strcmp(sub, "unmount") == 0) {
        sdcard_unmount();
        printf("unmounted\n");
    } else if (strcmp(sub, "ls") == 0) {
        if (!sdcard_is_mounted()) {
            printf("not mounted - run `sd mount` first\n");
        } else {
            sd_ls((argc > 2) ? argv[2] : "sd:/");
        }
    } else if (strcmp(sub, "test") == 0) {
        sd_test();
    } else if (strcmp(sub, "bench") == 0) {
        sd_bench((argc > 2) ? argv[2] : "sd:/3.ipv");
    } else if (strcmp(sub, "power") == 0) {
        if (argc < 3) {
            printf("usage: sd power <on|off>\n");
        } else {
            int on = (strcmp(argv[2], "on") == 0);
            sdcard_power(on);
            printf("SD supply %s (GPIO%d driven %d)\n", on ? "on" : "off",
                   BOARD_SD_PWR_EN_PIN,
                   on ? BOARD_SD_PWR_ON_LEVEL : !BOARD_SD_PWR_ON_LEVEL);
        }
    } else {
        sd_usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_sd, sd, micro-SD card test);

#endif /* CONFIG_SHELL */
