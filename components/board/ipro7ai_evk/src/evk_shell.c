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
 * @file evk_shell.c
 * @brief `lcd`, `cam`, `mic`, `spk`, `sd` shell commands for the EVK board library.
 *
 * Opt-in with CONFIG_EVK_SHELL. An app that registers its own `lcd` or `cam`
 * command must leave it off - SHELL_CMD_EXPORT_ALIAS creates a global
 * __fsym_<name> symbol per command, so two `cam`s do not link.
 *
 * `cam start` here only starts the pipeline; an app that renders the preview
 * hooks evk_cam_set_frame_cb() itself.
 */

#include <generated/autoconf.h>

#if defined(CONFIG_SHELL) && defined(CONFIG_EVK_SHELL)

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "evk.h"

/* ---------------------------------------------------------------------------
 * lcd
 * -------------------------------------------------------------------------*/

static int cmd_lcd(int argc, char **argv)
{
    if (argc > 2 && !strcmp(argv[1], "clk")) {
        st7789p3_set_spi_clk((uint32_t)strtoul(argv[2], NULL, 0));
        printf("SPI clock request %lu Hz\n", (unsigned long)st7789p3_get_spi_clk());
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "bl"))  { st7789p3_set_backlight(atoi(argv[2])); return 0; }
    if (argc > 2 && !strcmp(argv[1], "bgr")) { st7789p3_set_bgr(atoi(argv[2])); return 0; }
    if (argc > 2 && !strcmp(argv[1], "inv")) { st7789p3_set_inversion(atoi(argv[2])); return 0; }
    if (argc > 2 && !strcmp(argv[1], "fill")) {
        st7789p3_fill_screen((uint16_t)strtoul(argv[2], NULL, 0));
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "init")) {
        return st7789p3_init(ST7789P3_SEQ_MINIMAL, 1) == ST7789P3_OK ? 0 : -1;
    }
    printf("lcd - ST7789P3 %ux%u, %s, SPI %lu Hz, MADCTL 0x%02X, backlight %d\n"
           "  lcd init | clk <hz> | bl <0|1> | bgr <0|1> | inv <0|1> | fill <rgb565>\n",
           st7789p3_width(), st7789p3_height(), st7789p3_is_ready() ? "ready" : "NOT INIT",
           (unsigned long)st7789p3_get_spi_clk(), st7789p3_get_madctl(),
           st7789p3_get_backlight());
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_lcd, lcd, ST7789P3 panel);

/* ---------------------------------------------------------------------------
 * cam
 * -------------------------------------------------------------------------*/

#if EVK_HAS_CAMERA
static int cmd_cam(int argc, char **argv)
{
    int rc;

    if (argc > 1 && !strcmp(argv[1], "start")) {
        rc = evk_cam_start();
        if (rc != EVK_CAM_OK) printf("cam start failed (%d) - J6 pin 3-7 (VDDIO1) wired?\n", rc);
        return rc;
    }
    if (argc > 1 && !strcmp(argv[1], "stop"))  { evk_cam_stop(); return 0; }
    if (argc > 1 && !strcmp(argv[1], "probe")) { return evk_cam_probe(); }
    if (argc > 2 && !strcmp(argv[1], "fps")) {
        rc = evk_cam_set_fps(atoi(argv[2]));
        if (rc != EVK_CAM_OK) printf("fps must be 1..30\n");
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "rot")) {
        rc = evk_cam_set_rot(atoi(argv[2]));
        if (rc == EVK_CAM_OK) printf("rot %d - takes effect on the next start\n", evk_cam_get_rot());
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "fit")) {
        rc = evk_cam_set_fit(!strcmp(argv[2], "crop") ? EVK_CAM_FIT_CROP : EVK_CAM_FIT_LETTERBOX);
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "npu")) {
        evk_cam_npu_set_enabled(atoi(argv[2]) != 0);
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "rst")) { evk_cam_reset_pin(atoi(argv[2])); return 0; }

    evk_cam_geometry_t g;
    evk_cam_stats_t    st;
    evk_cam_get_geometry(&g);
    evk_cam_get_stats(&st);
    printf("cam - %s -> ISP -> preview + NPU\n"
           "  cam start | stop | probe | fps <1..30> | rot <0..3> | fit <box|crop> | npu <0|1> | rst <0|1>\n"
           "pipeline: %s, %d fps, rot %d, NPU feed %s; preview %ux%u at (%u,%u); ROI %ux%u+%u+%u\n"
           "frames  : ch0 %u, ch1 %u (%u to NPU), errors fifo %u vsync %u hsync %u\n"
           "wiring  : SCL=%d SDA=%d D0-7=%d-%d PCLK=%d VSYNC=%d HSYNC=%d MCLK=%d RST=%d\n",
           evk_cam_sensor_name() ? evk_cam_sensor_name() : EVK_CAM_SENSOR_NAME,
           evk_cam_is_running() ? "running" : "stopped", evk_cam_get_fps(), evk_cam_get_rot(),
           evk_cam_npu_enabled() ? "on" : "off",
           (unsigned)g.img_w, (unsigned)g.img_h, (unsigned)g.ox, (unsigned)g.oy,
           (unsigned)g.roi_w, (unsigned)g.roi_h, (unsigned)g.roi_x, (unsigned)g.roi_y,
           (unsigned)st.frames0, (unsigned)st.frames1, (unsigned)st.pushed,
           (unsigned)st.fifo_err, (unsigned)st.vsync_err, (unsigned)st.hsync_err,
           EVK_CAM_PIN_SCL, EVK_CAM_PIN_SDA, EVK_CAM_PIN_D0, EVK_CAM_PIN_D0 + 7,
           EVK_CAM_PIN_PCLK, EVK_CAM_PIN_VSYNC, EVK_CAM_PIN_HSYNC, EVK_CAM_PIN_MCLK,
           EVK_CAM_PIN_RST);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, camera pipeline);
#endif /* EVK_HAS_CAMERA */

/* ---------------------------------------------------------------------------
 * mic / spk
 * -------------------------------------------------------------------------*/

#if EVK_HAS_AUDIO
static int cmd_mic(int argc, char **argv)
{
    evk_mic_stats_t st;

    if (argc > 2 && !strcmp(argv[1], "gain")) {
        if (evk_mic_set_gain_db(atoi(argv[2])) != EVK_MIC_OK) {
            printf("gain must be 0..%d dB\n", EVK_MIC_DIGITAL_GAIN_DB);
            return -1;
        }
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "level")) {
        /* One second of 20 ms frames: prints peak/rms so a silent or a
         * clipping microphone is one command away. */
        static int16_t frame[EVK_MIC_FRAME_SAMPLES];
        uint32_t secs = argc > 2 ? (uint32_t)atoi(argv[2]) : 1u;
        bool was_on = evk_mic_running();
        if (!was_on && evk_mic_start() != EVK_MIC_OK) {
            printf("mic start failed\n");
            return -1;
        }
        evk_mic_flush();
        for (uint32_t f = 0; f < secs * 50u; f++) {
            uint16_t peak, rms;
            if (!evk_mic_read(frame, 200)) {
                evk_mic_get_stats(&st);
                printf("no frame (AUADC ISR frames so far: %u)\n", (unsigned)st.isr_frames);
                break;
            }
            evk_mic_level(frame, EVK_MIC_FRAME_SAMPLES, &peak, &rms);
            if (f % 5 == 4) {
                int pk = evk_mic_dbfs_x10(peak), rm = evk_mic_dbfs_x10(rms);
                printf("peak %6u (%4d.%d dBFS)  rms %6u (%4d.%d dBFS)\n",
                       peak, pk / 10, abs(pk % 10), rms, rm / 10, abs(rm % 10));
            }
        }
        if (!was_on) evk_mic_stop();
        return 0;
    }
    if (argc > 1 && !strcmp(argv[1], "start")) { return evk_mic_start(); }
    if (argc > 1 && !strcmp(argv[1], "stop"))  { evk_mic_stop(); return 0; }

    evk_mic_get_stats(&st);
    printf("mic - IM69D129 PDM on CLK=GPIO%d DAT=GPIO%d, %s, gain %+d dB, %u Hz real\n"
           "  mic start | stop | level [seconds] | gain <0..%d>\n"
           "frames  : isr %u, delivered %u, dropped %u, discarded %u\n",
           EVK_MIC_PIN_CLK, EVK_MIC_PIN_DAT, evk_mic_running() ? "running" : "stopped",
           evk_mic_get_gain_db(), (unsigned)evk_mic_real_rate(), EVK_MIC_DIGITAL_GAIN_DB,
           (unsigned)st.isr_frames, (unsigned)st.delivered, (unsigned)st.dropped,
           (unsigned)st.discarded);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mic, mic, PDM microphone);

static int cmd_spk(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "tone")) {
        uint32_t hz  = argc > 2 ? (uint32_t)atoi(argv[2]) : 1000u;
        uint32_t ms  = argc > 3 ? (uint32_t)atoi(argv[3]) : 1000u;
        uint32_t amp = argc > 4 ? (uint32_t)atoi(argv[4]) : 30u;
        int rc = evk_spk_tone(hz, ms, amp);
        if (rc != EVK_SPK_OK) printf("tone failed (%d)\n", rc);
        return rc;
    }
    if (argc > 2 && !strcmp(argv[1], "vol")) {
        if (evk_spk_set_volume(atoi(argv[2])) != EVK_SPK_OK) {
            printf("volume must be 0..%d\n", EVK_SPK_VOLUME_MAX);
            return -1;
        }
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "en")) { evk_spk_enable(atoi(argv[2])); return 0; }

    printf("spk - DAC GPIO%d -> TPA2037D1 (EN GPIO%d active %s), %s, volume %d%%\n"
           "  spk tone [hz] [ms] [amp%%] | vol <0..%d> | en <0|1>\n",
           EVK_SPK_PIN_DAC, EVK_SPK_PIN_EN, EVK_SPK_EN_ACTIVE ? "high" : "low",
           evk_spk_is_open() ? "open" : "closed", evk_spk_get_volume(), EVK_SPK_VOLUME_MAX);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk, spk, DAC speaker);
#endif /* EVK_HAS_AUDIO */

/* ---------------------------------------------------------------------------
 * sd
 * -------------------------------------------------------------------------*/

#if EVK_HAS_SDCARD
#include "ff.h"

static void sd_ls(const char *path)
{
    DIR     dir;
    FILINFO fno;
    FRESULT fr = f_opendir(&dir, path);

    if (fr != FR_OK) {
        printf("f_opendir(\"%s\") failed (FRESULT %d)\n", path, (int)fr);
        return;
    }
    for (;;) {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == '\0') {
            break;
        }
        printf("  %s%-32s %10lu\n", (fno.fattrib & AM_DIR) ? "/" : " ",
               fno.fname, (unsigned long)fno.fsize);
    }
    f_closedir(&dir);
}

static int cmd_sd(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "mount"))   { return evk_sd_mount(); }
    if (argc > 1 && !strcmp(argv[1], "unmount")) { return evk_sd_unmount(); }
    if (argc > 1 && !strcmp(argv[1], "ls")) {
        if (!evk_sd_mounted() && evk_sd_mount() != EVK_SD_OK) {
            printf("mount failed\n");
            return -1;
        }
        sd_ls(argc > 2 ? argv[2] : EVK_SD_ROOT);
        return 0;
    }
    if (argc > 2 && !strcmp(argv[1], "power")) { evk_sd_power(atoi(argv[2])); return 0; }

    uint32_t total = 0, freek = 0;
    if (evk_sd_mounted()) {
        evk_sd_space_kb(&total, &freek);
    }
    printf("sd - SDH GPIO27-34, FatFS at sd:/ - card %s, %s, %lu MiB total, %lu MiB free\n"
           "  sd mount | unmount | ls [path] | power <0|1>\n",
           evk_sd_present() ? "seated" : "absent", evk_sd_mounted() ? "mounted" : "not mounted",
           (unsigned long)(total / 1024u), (unsigned long)(freek / 1024u));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sd, sd, micro-SD card);
#endif /* EVK_HAS_SDCARD */

#endif /* CONFIG_SHELL && CONFIG_EVK_SHELL */
