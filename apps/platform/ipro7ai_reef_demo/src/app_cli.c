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
 * @file app_cli.c
 * @brief Shell commands for the carrier's non-audio blocks:
 *        reef, led, btn, hp, cam, fin.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "shell.h"
#include "board_config.h"
#include "hal_gpio.h"
#include "drv_glb.h"

#include "rgb_led.h"
#include "keys.h"
#include "audio.h"
#include "sdcard.h"
#include "cam.h"

/* ---------------------------------------------------------------------------
 * reef
 * -------------------------------------------------------------------------*/

static int hp_det_raw(void)
{
    uint8_t pin[1] = { BOARD_HP_DET_PIN };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(BOARD_HP_DET_PIN, GPIO_INPUT_PP_MODE);
    return gpio_read(BOARD_HP_DET_PIN);
}

static void evk_status(void)
{
    uint8_t r, g, b;

    rgb_led_get(&r, &g, &b);
    printf("IPRO7AI Reef (CONFIG_IPRO7AI_REEF)\n");
    printf("  console : UART0 TX=%d RX=%d  (U6 wafer 3/4, DTR->BOOT, RTS->RESET)\n",
           BOARD_UART_TX_PIN, BOARD_UART_RX_PIN);
    printf("  keys    :");
    for (int k = 0; k < KEY_COUNT; k++) {
        printf(" %s=%s", keys_name((key_id_t)k),
               keys_is_pressed((key_id_t)k) ? "DOWN" : "up");
    }
    printf("\n");
    printf("  rgb led : GPIO%d  (%u,%u,%u)\n", BOARD_RGB_LED_PIN, r, g, b);
    printf("  speaker : DAC GPIO%d, amp SD# GPIO%d = %s, job=%s\n",
           BOARD_SPK_DAC_PIN, BOARD_SPK_EN_PIN,
           audio_spk_enabled() ? "ON" : "muted", audio_job_name());
    printf("  hp det  : GPIO%d reads %d (%s)\n", BOARD_HP_DET_PIN, hp_det_raw(),
           hp_det_raw() == BOARD_HP_DET_ACTIVE ? "plug inserted" : "no plug");
    printf("  sd slot : DET GPIO%d=%d (%s), PWR GPIO%d=%s, %s\n",
           BOARD_SD_DET_PIN, sdcard_detect_raw(),
           sdcard_present() ? "card/fin seated" : "empty",
           BOARD_SD_PWR_EN_PIN, sdcard_power_is_on() ? "on" : "off",
           sdcard_is_mounted() ? "mounted" : "not mounted");
    printf("  camera  : FLEX-24 on FPC1, XCLK GPIO%d RST GPIO%d PWDN GPIO%d SCCB %d/%d\n",
           BOARD_CAM_MCLK_PIN, BOARD_CAM_RST_PIN, BOARD_CAM_PWDN_PIN,
           BOARD_CAM_SCL_PIN, BOARD_CAM_SDA_PIN);
    printf("  mics    : PDM CLK=%d DAT=%d | headset AIN%d/AIN%d | aux AIN%d/AIN%d\n",
           BOARD_MIC_CLK_PIN, BOARD_MIC_DAT_PIN, BOARD_HSMIC_POS_CH,
           BOARD_HSMIC_NEG_CH, BOARD_AUX_L_CH, BOARD_AUX_R_CH);
}

static void evk_selftest(void)
{
    static const uint8_t seq[][3] = { {40,0,0}, {0,40,0}, {0,0,40}, {0,0,0} };

    printf("[1/4] RGB LED: red, green, blue\n");
    for (unsigned i = 0; i < sizeof(seq) / sizeof(seq[0]); i++) {
        rgb_led_set(seq[i][0], seq[i][1], seq[i][2]);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    printf("[2/4] speaker: 1 kHz, 300 ms, 20%%\n");
    audio_tone(1000, 300, 20, AUDIO_ROUTE_SPK);
    vTaskDelay(pdMS_TO_TICKS(600));
    audio_stop();
    printf("[3/4] SD slot: %s, supply %s\n",
           sdcard_present() ? "something seated" : "empty",
           sdcard_power_is_on() ? "on" : "off");
    printf("[4/4] camera bus: ");
    if (cam_probe() == 0) {
        printf("%s at 0x%02X\n", cam_sensor_name(), cam_sensor_addr());
    } else {
        printf("no sensor\n");
    }
    cam_park();
    printf("done - keys: press any and run `btn status`\n");
}

static int cmd_reef(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "status") == 0) {
        evk_status();
    } else if (strcmp(argv[1], "selftest") == 0) {
        evk_selftest();
    } else {
        printf("reef status | reef selftest\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_reef, reef, IPRO7AI Reef board status and self-test);

/* ---------------------------------------------------------------------------
 * led
 * -------------------------------------------------------------------------*/

static void led_wheel(uint32_t pos, uint8_t *r, uint8_t *g, uint8_t *b)
{
    pos &= 0xFF;
    if (pos < 85) {
        *r = (uint8_t)(255 - pos * 3); *g = (uint8_t)(pos * 3); *b = 0;
    } else if (pos < 170) {
        pos -= 85;
        *r = 0; *g = (uint8_t)(255 - pos * 3); *b = (uint8_t)(pos * 3);
    } else {
        pos -= 170;
        *r = (uint8_t)(pos * 3); *g = 0; *b = (uint8_t)(255 - pos * 3);
    }
}

static int cmd_led(int argc, char **argv)
{
    if (argc >= 4) {
        int r = atoi(argv[1]), g = atoi(argv[2]), b = atoi(argv[3]);
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            printf("values are 0..255\n");
            return 0;
        }
        if (rgb_led_set((uint8_t)r, (uint8_t)g, (uint8_t)b) == 0) {
            printf("LED = (%d,%d,%d)\n", r, g, b);
        }
    } else if (argc >= 2 && strcmp(argv[1], "off") == 0) {
        rgb_led_off();
        printf("LED off, GPIO%d parked low\n", BOARD_RGB_LED_PIN);
    } else if (argc >= 2 && strcmp(argv[1], "rainbow") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 5u;
        uint32_t steps = sec * 50u;
        for (uint32_t i = 0; i < steps; i++) {
            uint8_t r, g, b;
            led_wheel(i * 3, &r, &g, &b);
            rgb_led_set(r / 4, g / 4, b / 4);   /* quarter brightness */
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        rgb_led_set(0, 0, 0);
    } else if (argc >= 2 && strcmp(argv[1], "status") == 0) {
        uint8_t r, g, b;
        rgb_led_get(&r, &g, &b);
        printf("GPIO%d via SPI0 MOSI, 4 SPI bits per WS2812 bit at 3.2 MHz; "
               "last colour (%u,%u,%u)\n", BOARD_RGB_LED_PIN, r, g, b);
    } else {
        printf("led <r> <g> <b>      set colour (0..255 each)\n"
               "led off              LED off, pad back to GPIO\n"
               "led rainbow [sec]    cycle the hue\n"
               "led status\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_led, led, WS2812 RGB LED on GPIO24);

/* ---------------------------------------------------------------------------
 * btn
 * -------------------------------------------------------------------------*/

static volatile int s_watch_events;

static void btn_watch_cb(key_id_t key, bool pressed, void *ctx)
{
    (void)ctx;
    printf("  %s %s\n", keys_name(key), pressed ? "pressed" : "released");
    s_watch_events++;
}

static int cmd_btn(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "watch") == 0) {
        uint32_t sec = (argc > 2) ? (uint32_t)atoi(argv[2]) : 10u;
        printf("watching keys for %u s (BOOT is active high)...\n", (unsigned)sec);
        s_watch_events = 0;
        keys_set_callback(btn_watch_cb, NULL);
        vTaskDelay(pdMS_TO_TICKS(sec * 1000u));
        keys_set_callback(NULL, NULL);
        printf("%d event%s\n", s_watch_events, s_watch_events == 1 ? "" : "s");
    } else {
        for (int k = 0; k < KEY_COUNT; k++) {
            printf("  %-9s GPIO%-2d %-4s pressed %u time%s\n",
                   keys_name((key_id_t)k), keys_pin((key_id_t)k),
                   keys_is_pressed((key_id_t)k) ? "DOWN" : "up",
                   (unsigned)keys_press_count((key_id_t)k),
                   keys_press_count((key_id_t)k) == 1 ? "" : "s");
        }
        printf("SW2 is the reset key. SW3/SW5 are released while AUX-IN captures.\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_btn, btn, keys SW1(BOOT)/SW3/SW4/SW5);

/* ---------------------------------------------------------------------------
 * hp - headphone jack detect
 * -------------------------------------------------------------------------*/

static int cmd_hp(int argc, char **argv)
{
    (void)argc; (void)argv;
    int v = hp_det_raw();

    printf("GPIO%d (J2 pad 6 = ring1 spring, R50 100k to 3V3) reads %d -> %s\n",
           BOARD_HP_DET_PIN, v, v == BOARD_HP_DET_ACTIVE ? "plug inserted" : "no plug");
    printf("LOW comes through the ring1 earpiece coil. If that ear is silent the\n"
           "6-7 bridge rework is missing (HP_TIP sits on the switch contact, pad 7).\n"
           "Jack is OMTP: CTIA headsets play quietly with a dead mic.\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_hp, hp, headphone jack detect on GPIO26);

/* ---------------------------------------------------------------------------
 * cam
 * -------------------------------------------------------------------------*/

static void cam_usage(void)
{
    printf("cam probe                power sequence + SCCB, through components/sensor\n"
           "cam start | cam stop     YUV422 stream -> ISP ch0 (1/4 size, PSRAM)\n"
           "cam status               frames, fps, sync/fifo errors, mean luma\n"
           "cam snap [cols]          ASCII thumbnail of the latest frame\n"
           "cam bar on|off           sensor colour bars (pipeline check without optics)\n"
           "cam reg <addr> [val]     sensor register (OV2640: 0x1xx = sensor bank)\n"
           "cam order lower|higher   DVP byte order, applied at next start\n"
           "cam sync <v> <h>         vsync/hsync polarity: high|low, next start\n"
           "cam pins [ms]            DVP pad activity: edges/s + duty (50..5000 ms)\n"
           "cam park | cam xclk on|off\n");
}

static int cmd_cam(int argc, char **argv)
{
    if (argc < 2) {
        cam_usage();
        return 0;
    }
    if (strcmp(argv[1], "probe") == 0) {
        if (cam_probe() == 0) {
            uint16_t w, h;
            cam_sensor_size(&w, &h);
            printf("sensor %s at SCCB 0x%02X, %ux%u\n", cam_sensor_name(),
                   cam_sensor_addr(), w, h);
        } else {
            printf("no sensor found\n");
        }
    } else if (strcmp(argv[1], "start") == 0) {
        int rc = cam_start();
        printf(rc == CAM_OK ? "streaming - `cam status` in a second\n"
                            : "start failed (%d)\n", rc);
    } else if (strcmp(argv[1], "stop") == 0) {
        cam_stop();
        printf("stopped\n");
    } else if (strcmp(argv[1], "status") == 0) {
        cam_stats_t st;
        uint16_t w, h, ow, oh;
        int lo, vs, hs;
        cam_get_stats(&st);
        cam_sensor_size(&w, &h);
        cam_out_size(&ow, &oh);
        cam_get_dvp_opts(&lo, &vs, &hs);
        printf("sensor  : %s @0x%02X %ux%u, %s\n", cam_sensor_name() ? cam_sensor_name() : "-",
               cam_sensor_addr(), w, h, cam_is_running() ? "streaming" : "idle");
        printf("ch0     : YUYV %ux%u, frames %u, %u.%u fps, luma mean %u\n", ow, oh,
               (unsigned)st.frames, (unsigned)(st.fps_x10 / 10), (unsigned)(st.fps_x10 % 10),
               (unsigned)st.luma_mean);
        printf("errors  : fifo %u, vsync %u, hsync %u\n", (unsigned)st.fifo_err,
               (unsigned)st.vsync_err, (unsigned)st.hsync_err);
        printf("dvp     : byte order %s, vsync %s, hsync %s\n", lo ? "lower-first" : "higher-first",
               vs ? "high" : "low", hs ? "high" : "low");
        if (cam_is_running() && st.frames == 0) {
            printf("no frames: check XCLK on GPIO22 with a scope, then `cam sync`\n");
        }
    } else if (strcmp(argv[1], "snap") == 0) {
        if (cam_snap((argc > 2) ? (unsigned)atoi(argv[2]) : 64u) != CAM_OK) {
            printf("no frame yet - `cam start` first\n");
        }
    } else if (strcmp(argv[1], "bar") == 0 && argc >= 3) {
        bool on = strcmp(argv[2], "on") == 0;
        printf(cam_colorbar(on) == CAM_OK ? "colour bars %s\n" : "failed (%s)\n",
               on ? "on" : "off");
    } else if (strcmp(argv[1], "reg") == 0 && argc >= 3) {
        int addr = (int)strtol(argv[2], NULL, 0);
        int val;
        if (argc >= 4) {
            val = (int)strtol(argv[3], NULL, 0);
            printf(cam_reg_write(addr, val) == CAM_OK ? "0x%04X <= 0x%02X\n" : "write 0x%04X failed (%d)\n",
                   (unsigned)addr, val);
        } else if (cam_reg_read(addr, &val) == CAM_OK) {
            printf("0x%04X = 0x%02X\n", (unsigned)addr, val);
        } else {
            printf("read 0x%04X failed\n", (unsigned)addr);
        }
    } else if (strcmp(argv[1], "order") == 0 && argc >= 3) {
        cam_set_byte_order(strcmp(argv[2], "lower") == 0);
        printf("byte order set - restart the stream\n");
    } else if (strcmp(argv[1], "sync") == 0 && argc >= 4) {
        cam_set_sync(strcmp(argv[2], "high") == 0, strcmp(argv[3], "high") == 0);
        printf("sync polarity set - restart the stream\n");
    } else if (strcmp(argv[1], "pins") == 0) {
        cam_pins_report((argc > 2) ? (unsigned)atoi(argv[2]) : 500u);
    } else if (strcmp(argv[1], "park") == 0) {
        cam_stop();
        printf("PWDN high, RESETB low, XCLK off\n");
    } else if (strcmp(argv[1], "xclk") == 0 && argc >= 3) {
        int on = strcmp(argv[2], "on") == 0;
        cam_xclk(on);
        printf("XCLK %s\n", on ? "24 MHz on GPIO22" : "off");
    } else {
        cam_usage();
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam, cam, FLEX-24 camera: probe / stream / snap);

/* ---------------------------------------------------------------------------
 * fin - the IPRO6S_FIN Wi-Fi fin in the card slot
 * -------------------------------------------------------------------------*/

static int cmd_fin(int argc, char **argv)
{
    if (argc >= 2 && strcmp(argv[1], "cycle") == 0) {
        sdcard_power_cycle(300);
        printf("slot supply cycled (GPIO%d). Only effective with JP3 on 2-3.\n",
               BOARD_SD_PWR_EN_PIN);
    } else if (argc >= 3 && strcmp(argv[1], "power") == 0) {
        bool on = strcmp(argv[2], "on") == 0;
        sdcard_power(on);
        printf("slot supply %s\n", on ? "on" : "off");
    } else {
        printf("IPRO6S_FIN: SDIO slave in the SD slot, SD_VDD = its 3V3.\n"
               "  seated : %s (DET GPIO%d)\n"
               "  supply : %s (GPIO%d, JP3 2-3 required for control)\n"
               "  host   : SDH_0 on GPIO28-33, see apps/platform/ipro7_sdio_wifi\n"
               "fin cycle | fin power on|off\n",
               sdcard_present() ? "yes" : "no", BOARD_SD_DET_PIN,
               sdcard_power_is_on() ? "on" : "off", BOARD_SD_PWR_EN_PIN);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_fin, fin, IPRO6S Wi-Fi fin slot power);

#endif /* CONFIG_SHELL */
