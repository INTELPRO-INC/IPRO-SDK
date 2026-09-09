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
 * @file main.c
 * @brief SD-card video player — GH7007-01 5" SPI panel (320x240 -> 1024x600).
 *
 * File format (.ghv, produced by tools/video_convert.py):
 *   16-byte header: "GHV1" | u16 width | u16 height | u16 fps | u16 flags
 *                   | u32 frame_count      (all little-endian)
 *   then frame_count frames of width*height*2 bytes, already in the panel's
 *   WIRE format (gh_px repack + MSB-first byte order applied offline), so
 *   playback is a pure read -> row-stream loop with zero per-pixel work.
 *
 * Storage: FATFS on the PIO soft SD host (CLK=G22 CMD=G23 D0..D3=G24..27 —
 * no conflict with the LCD on G12..17). Bandwidth budget at 15 fps:
 * 2.25 MB/s read vs ~5-10 MB/s PIO-SDH — comfortable.
 *
 * Shell:
 *   video ls              list *.ghv on the card
 *   video play <file> [1] play (optional trailing 1 = loop)
 *   video stop            stop playback
 *   video stat            playback statistics
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <generated/autoconf.h>

#include <FreeRTOS.h>
#include <task.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include "freertos_app_hooks.h"
#include "usb_console.h"

#include "ff.h"
#include "hal_sdh.h"
#include "drv_glb.h"
#include "drv_gpio.h"

#include "gh7007.h"
#include "gh7007_port.h"
#include "gh7007_patterns.h"

/* fatfs port (components/fs/fatfs/port/fatfs_sdh_sdcard.c) */
extern void fatfs_sdh_driver_register(void);
extern void hal_sd_bus_4bits_enable(void);

#define VID_W  GH_DEF_WIDTH     /* 320 */
#define VID_H  GH_DEF_HEIGHT    /* 240 */
#define VID_FRAME_BYTES ((uint32_t)VID_W * VID_H * 2u)

/* Per-device USB serial — a fixed value is enough to enumerate one board. */
void device_serial_get(uint8_t out[8])
{
    static const uint8_t sn[8] = { 0x67, 0x70, 0x07, 0x00, 0x00, 0x00, 0x00, 0x03 };
    for (int i = 0; i < 8; i++) {
        out[i] = sn[i];
    }
}

/* ---- .ghv header ---- */
typedef struct {
    char     magic[4];          /* "GHV1" */
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    uint16_t flags;
    uint32_t frames;
} __attribute__((packed)) ghv_header_t;

/* ---- player state ---- */
static uint16_t s_frame[VID_W * VID_H] __attribute__((section(".psram_bss")));

static FATFS s_fatfs;
static int   s_mounted;

static volatile int s_stop_req;
static volatile int s_playing;
static int  s_loop;
static char s_path[96];

static volatile uint32_t s_stat_frames;
static volatile uint32_t s_stat_fps;
static volatile uint32_t s_stat_read_ms;
static volatile uint32_t s_stat_blit_ms;

/* Native hardware SDH on G28..G33 (CLK/CMD/D0-D3 per the SoC's SDC alt-func
 * mux; the IPRO7 internal flash does NOT use these pads — they are free).
 * Recipe from the proven A/B reference (ipro_pio_sdh_test/hw_sdcard_cmd.c). */
static void sdh_pinmux_init(void)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_UP,
        .drive    = 2,
        .smtCtrl  = 1,
        .gpioFun  = GPIO_FUN_SDC,
    };
    uint8_t pins[] = {
        GLB_GPIO_PIN_28, GLB_GPIO_PIN_29, GLB_GPIO_PIN_30,
        GLB_GPIO_PIN_31, GLB_GPIO_PIN_32, GLB_GPIO_PIN_33,
    };
    for (int i = 0; i < (int)(sizeof(pins) / sizeof(pins[0])); i++) {
        cfg.gpioPin  = pins[i];
        cfg.pullType = (pins[i] == GLB_GPIO_PIN_30) ?
                       GPIO_PULL_NONE : GPIO_PULL_UP;
        GLB_GPIO_Init(&cfg);
    }

    GLB_Set_SDH_CLK(1, GLB_SDH_CLK_PLL_96M, 1);
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_SDH);
}

static int vid_mount(void)
{
    if (s_mounted) {
        return 0;
    }
    sdh_pinmux_init();
    hal_sd_bus_4bits_enable();
    fatfs_sdh_driver_register();

    FRESULT fr = f_mount(&s_fatfs, "sd:", 1);
    if (fr != FR_OK) {
        printf("[VID] mount failed (%d) — card inserted? wiring G28-33?\r\n", fr);
        return -1;
    }
    s_mounted = 1;
    printf("[VID] SD mounted (HW SDH, 4-bit)\r\n");
    return 0;
}

static void vid_play_task(void *arg)
{
    (void)arg;
    FIL f;
    ghv_header_t hdr;
    UINT br;

    FRESULT fr = f_open(&f, s_path, FA_READ);
    if (fr != FR_OK) {
        printf("[VID] open '%s' failed (%d)\r\n", s_path, fr);
        goto out;
    }

    if (f_read(&f, &hdr, sizeof(hdr), &br) != FR_OK || br != sizeof(hdr) ||
        memcmp(hdr.magic, "GHV1", 4) != 0) {
        printf("[VID] '%s' is not a GHV1 file\r\n", s_path);
        f_close(&f);
        goto out;
    }
    if (hdr.width != VID_W || hdr.height != VID_H) {
        printf("[VID] %ux%u not supported (need %ux%u) — re-run video_convert.py\r\n",
               hdr.width, hdr.height, VID_W, VID_H);
        f_close(&f);
        goto out;
    }
    if (hdr.fps == 0 || hdr.fps > 60) {
        hdr.fps = 15;
    }

    printf("[VID] playing %s: %ux%u @%ufps, %u frames%s\r\n", s_path,
           hdr.width, hdr.height, hdr.fps, (unsigned)hdr.frames,
           s_loop ? " (loop)" : "");

    TickType_t period = pdMS_TO_TICKS(1000u / hdr.fps);
    if (period == 0) {
        period = 1;
    }
    TickType_t last_wake = xTaskGetTickCount();
    uint32_t fps_frames = 0;
    uint32_t fps_t0 = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    while (!s_stop_req) {
        TickType_t t0 = xTaskGetTickCount();
        fr = f_read(&f, s_frame, VID_FRAME_BYTES, &br);
        TickType_t t1 = xTaskGetTickCount();

        if (fr != FR_OK) {
            printf("[VID] read error (%d)\r\n", fr);
            break;
        }
        if (br < VID_FRAME_BYTES) {                    /* end of file */
            if (s_loop && f_lseek(&f, sizeof(ghv_header_t)) == FR_OK) {
                last_wake = xTaskGetTickCount();
                continue;
            }
            break;
        }

        gh7007_blit_start(0, 0, VID_W - 1, VID_H - 1);
        for (int y = 0; y < VID_H; y++) {
            gh7007_blit_push_row_raw(
                (const uint8_t *)&s_frame[(uint32_t)y * VID_W], VID_W);
        }
        gh7007_blit_end();
        TickType_t t2 = xTaskGetTickCount();

        s_stat_read_ms = (uint32_t)((t1 - t0) * portTICK_PERIOD_MS);
        s_stat_blit_ms = (uint32_t)((t2 - t1) * portTICK_PERIOD_MS);
        s_stat_frames++;
        fps_frames++;
        uint32_t now = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        if (now - fps_t0 >= 1000u) {
            s_stat_fps = fps_frames;
            fps_frames = 0;
            fps_t0 = now;
        }
        if ((s_stat_frames % 128) == 1) {
            printf("[VID] frame #%u read=%ums blit=%ums fps=%u\r\n",
                   (unsigned)s_stat_frames, (unsigned)s_stat_read_ms,
                   (unsigned)s_stat_blit_ms, (unsigned)s_stat_fps);
        }

        vTaskDelayUntil(&last_wake, period);
    }

    f_close(&f);
    printf("[VID] stopped after %u frames\r\n", (unsigned)s_stat_frames);
out:
    s_playing = 0;
    vTaskDelete(NULL);
}

/* ---------------------------------------------------------------------------
 * Shell
 * ---------------------------------------------------------------------------*/
#ifdef CONFIG_SHELL
static int cmd_video(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: video ls | play <file> [1=loop] | stop | stat\r\n");
        return 0;
    }

    if (strcmp(argv[1], "ls") == 0) {
        if (vid_mount() != 0) {
            return 0;
        }
        DIR dir;
        FILINFO fi;
        if (f_opendir(&dir, "sd:/") != FR_OK) {
            printf("[VID] opendir failed\r\n");
            return 0;
        }
        int n = 0;
        while (f_readdir(&dir, &fi) == FR_OK && fi.fname[0]) {
            if (!(fi.fattrib & AM_DIR)) {
                printf("  %-32s %10u B\r\n", fi.fname, (unsigned)fi.fsize);
                n++;
            }
        }
        f_closedir(&dir);
        printf("%d file(s)\r\n", n);

    } else if (strcmp(argv[1], "play") == 0 && argc >= 3) {
        if (s_playing) {
            printf("[VID] already playing — `video stop` first\r\n");
            return 0;
        }
        if (vid_mount() != 0) {
            return 0;
        }
        snprintf(s_path, sizeof(s_path), "sd:/%s", argv[2]);
        s_loop = (argc >= 4) ? atoi(argv[3]) : 0;
        s_stop_req = 0;
        s_stat_frames = 0;
        s_playing = 1;
        if (xTaskCreate(vid_play_task, "video", 4096, NULL, 8, NULL) != pdPASS) {
            s_playing = 0;
            printf("[VID] task create failed\r\n");
        }

    } else if (strcmp(argv[1], "stop") == 0) {
        s_stop_req = 1;

    } else if (strcmp(argv[1], "stat") == 0) {
        printf("playing=%d frames=%u fps=%u read=%ums blit=%ums loop=%d\r\n",
               s_playing, (unsigned)s_stat_frames, (unsigned)s_stat_fps,
               (unsigned)s_stat_read_ms, (unsigned)s_stat_blit_ms, s_loop);
    } else {
        printf("usage: video ls | play <file> [1=loop] | stop | stat\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_video, video, SD video player);
#endif

/* ---------------------------------------------------------------------------
 * Boot
 * ---------------------------------------------------------------------------*/
static void boot_task(void *arg)
{
    (void)arg;

    printf("[VID] Initializing GH7007 (320x240 -> 1024x600)...\r\n");
    if (gh7007_init() != 0) {
        printf("[VID] LCD init failed\r\n");
    }
    gh7007_pattern_bars();      /* visual "alive" splash until playback starts */

    vid_mount();                /* eager mount; harmless if no card yet */

    printf("[VID] ready — `video ls`, `video play <file> [1]`\r\n");
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
    usb_console_init();

    printf("\r\n========================================\r\n");
    printf("  IPRO7 SD Video Player — GH7007 5\"\r\n");
    printf("========================================\r\n\r\n");

    xTaskCreate(boot_task, "boot", 4096, NULL, 10, NULL);

    vTaskStartScheduler();

    while (1) {
    }
}
