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
 * @file st7789p3_cli.c
 * @brief `lcd` shell command for ST7789P3 bring-up.
 */

#include "st7789p3.h"
#include "st7789p3_patterns.h"

#ifdef CONFIG_SHELL

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

static bool require_ready(void)
{
    if (!st7789p3_is_ready()) {
        printf("panel not initialised - run `lcd init` first\n");
        return false;
    }
    return true;
}

/* Named colour, 0xRRGGBB, or a raw 0xRGB565 value. */
static bool parse_color(const char *s, uint16_t *out)
{
    static const struct { const char *name; uint16_t v; } tbl[] = {
        { "black", ST_BLACK }, { "white", ST_WHITE }, { "red", ST_RED },
        { "green", ST_GREEN }, { "blue", ST_BLUE },   { "yellow", ST_YELLOW },
        { "cyan", ST_CYAN },   { "magenta", ST_MAGENTA }, { "gray", ST_GRAY },
        { "grey", ST_GRAY },
    };

    for (unsigned i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++) {
        if (strcmp(s, tbl[i].name) == 0) {
            *out = tbl[i].v;
            return true;
        }
    }
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        unsigned long v = strtoul(s, NULL, 16);
        if (v > 0xFFFFuL) {
            *out = ST_RGB565((uint8_t)((v >> 16) & 0xFFu),
                             (uint8_t)((v >> 8) & 0xFFu),
                             (uint8_t)(v & 0xFFu));
        } else {
            *out = (uint16_t)v;
        }
        return true;
    }
    return false;
}

static bool parse_onoff(const char *s, int *out)
{
    if (strcmp(s, "on") == 0 || strcmp(s, "1") == 0) {
        *out = 1;
        return true;
    }
    if (strcmp(s, "off") == 0 || strcmp(s, "0") == 0) {
        *out = 0;
        return true;
    }
    return false;
}

static void usage(void)
{
    printf(
      "lcd - ST7789P3 240x320 SPI LCD (LCM-T2D4BP-084)\n"
      "  lcd status                    show current state\n"
      "  lcd init [minimal|tuned] [noclear]\n"
      "                                reset + init sequence (+ clear black)\n"
      "  lcd id                        read RDDID/RDID1-3/RDDST over SDA\n"
      "  lcd clk <hz>                  set SCLK (<=16M uses XCLK, above uses BCLK)\n"
      "  lcd framing <window|byte>     CS window per register (default) or per byte\n"
      "  lcd bl <on|off>               backlight enable pin (GPIO12)\n"
      "  lcd dir <0..3> [mirror]       0/2 portrait 240x320, 1/3 landscape 320x240\n"
      "  lcd bgr <on|off>              MADCTL colour order (fix swapped red/blue)\n"
      "  lcd inv <on|off>              INVON/INVOFF (IPS normally-black needs on)\n"
      "  lcd sleep <in|out>            SLPIN/SLPOUT\n"
      "  lcd disp <on|off>             DISPON/DISPOFF\n"
      "  lcd gamma default             restore datasheet default E0h/E1h curves\n"
      "  lcd fill <colour>             solid fill\n"
      "  lcd box <x0> <y0> <x1> <y1> <colour>\n"
      "  lcd bars | rgbtest | checker [sq] | grad | gray | frame [colour] | ruler\n"
      "  lcd bench                     time one full-screen fill\n"
      "  lcd demo                      cycle a few patterns\n"
      "  lcd cmd <hex> [p0 p1 ...]     raw register write\n"
      "  lcd read <hex> <nbytes>       raw register read (1..8 bytes)\n"
      "  lcd pin <gpio> [0|1]          drive/read a pin (breaks the bus; re-init)\n"
      "  colours: black white red green blue yellow cyan magenta gray,\n"
      "           0xRRGGBB, or a raw 0xRGB565 value\n");
}

static void cmd_status(void)
{
    printf("panel   : ST7789P3 %ux%u RGB565, %s\n",
           (unsigned)st7789p3_width(), (unsigned)st7789p3_height(),
           st7789p3_is_ready() ? "initialised" : "NOT initialised");
    printf("sequence: %s\n",
           (st7789p3_get_seq() == ST7789P3_SEQ_TUNED) ? "tuned" : "minimal");
    printf("spi     : SPI0 mode 0, sclk %u Hz, framing %s\n",
           (unsigned)st7789p3_get_spi_clk(),
           (st7789p3_get_framing() == ST7789P3_FRAME_BYTE) ? "per-byte"
                                                           : "per-window");
    printf("pins    : SCLK=%d MOSI=%d CS=%d DC=%d RST=%d BL=%d\n",
           ST_PIN_SCLK, ST_PIN_MOSI, ST_PIN_CS, ST_PIN_DC, ST_PIN_RST,
           ST_PIN_BL);
    printf("madctl  : 0x%02X (dir=%u %s), inversion %s, backlight %s\n",
           st7789p3_get_madctl(), (unsigned)st7789p3_get_dir(),
           st7789p3_get_bgr() ? "BGR" : "RGB",
           st7789p3_get_inversion() ? "on" : "off",
           st7789p3_get_backlight() ? "on" : "off");
}

static void cmd_init(int argc, char **argv)
{
    int seq   = ST7789P3_SEQ_MINIMAL;
    int clear = 1;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "tuned") == 0) {
            seq = ST7789P3_SEQ_TUNED;
        } else if (strcmp(argv[i], "minimal") == 0) {
            seq = ST7789P3_SEQ_MINIMAL;
        } else if (strcmp(argv[i], "noclear") == 0) {
            clear = 0;
        } else {
            printf("unknown init option '%s'\n", argv[i]);
            return;
        }
    }

    if (st7789p3_init(seq, clear) != ST7789P3_OK) {
        printf("init failed\n");
        return;
    }
    cmd_status();
}

static void cmd_id(void)
{
    uint8_t buf[8];

    /* RDDID (04h) returns 3 bytes: manufacturer, module/driver version,
     * module/driver ID. RDID1/2/3 (DAh/DBh/DCh) return one byte each and are
     * loaded from the module's NVM, so a blank OTP legitimately reads 00. */
    if (st7789p3_read_reg(0x04, buf, 3) == ST7789P3_OK) {
        printf("RDDID  (04h): %02X %02X %02X\n", buf[0], buf[1], buf[2]);
    }
    if (st7789p3_read_reg(0x09, buf, 4) == ST7789P3_OK) {
        printf("RDDST  (09h): %02X %02X %02X %02X\n",
               buf[0], buf[1], buf[2], buf[3]);
    }
    if (st7789p3_read_reg(0x0A, buf, 1) == ST7789P3_OK) {
        printf("RDDPM  (0Ah): %02X   (bit7 booster, bit4 sleep-out, "
               "bit2 display-on)\n", buf[0]);
    }
    if (st7789p3_read_reg(0xDA, buf, 1) == ST7789P3_OK) {
        printf("RDID1  (DAh): %02X\n", buf[0]);
    }
    if (st7789p3_read_reg(0xDB, buf, 1) == ST7789P3_OK) {
        printf("RDID2  (DBh): %02X\n", buf[0]);
    }
    if (st7789p3_read_reg(0xDC, buf, 1) == ST7789P3_OK) {
        printf("RDID3  (DCh): %02X\n", buf[0]);
    }
    printf("note: all-00 or all-FF means the chip is not driving SDA back -\n"
           "      check that nothing else is holding GPIO%d, then re-run "
           "`lcd init`.\n", ST_PIN_MOSI);
}

static void cmd_box(int argc, char **argv)
{
    uint16_t c;

    if (argc < 7) {
        printf("usage: lcd box <x0> <y0> <x1> <y1> <colour>\n");
        return;
    }
    if (!parse_color(argv[6], &c)) {
        printf("bad colour '%s'\n", argv[6]);
        return;
    }
    st7789p3_fill_rect((uint16_t)atoi(argv[2]), (uint16_t)atoi(argv[3]),
                       (uint16_t)atoi(argv[4]), (uint16_t)atoi(argv[5]), c);
}

static void cmd_raw(int argc, char **argv)
{
    uint8_t args[16];
    uint32_t n = 0;

    if (argc < 3) {
        printf("usage: lcd cmd <hex> [p0 p1 ...]\n");
        return;
    }
    for (int i = 3; i < argc && n < sizeof(args); i++) {
        args[n++] = (uint8_t)strtoul(argv[i], NULL, 16);
    }
    st7789p3_write_reg((uint8_t)strtoul(argv[2], NULL, 16), args, n);
    printf("wrote %s with %u param(s)\n", argv[2], (unsigned)n);
}

static void cmd_read(int argc, char **argv)
{
    uint8_t buf[8];
    uint32_t n;

    if (argc < 4) {
        printf("usage: lcd read <hex> <nbytes 1..8>\n");
        return;
    }
    n = (uint32_t)atoi(argv[3]);
    if (st7789p3_read_reg((uint8_t)strtoul(argv[2], NULL, 16), buf, n)
        != ST7789P3_OK) {
        printf("read failed (nbytes must be 1..8)\n");
        return;
    }
    printf("%s ->", argv[2]);
    for (uint32_t i = 0; i < n; i++) {
        printf(" %02X", buf[i]);
    }
    printf("\n");
}

static void cmd_pin(int argc, char **argv)
{
    uint8_t pin;

    if (argc < 3) {
        printf("usage: lcd pin <gpio> [0|1]\n");
        return;
    }
    pin = (uint8_t)atoi(argv[2]);
    if (argc >= 4) {
        st7789p3_pin_out(pin, atoi(argv[3]));
        printf("GPIO%u driven %s (bus released - re-run `lcd init`)\n",
               (unsigned)pin, atoi(argv[3]) ? "HIGH" : "LOW");
    } else {
        printf("GPIO%u reads %d (bus released - re-run `lcd init`)\n",
               (unsigned)pin, st7789p3_pin_in(pin));
    }
}

static void cmd_bench(void)
{
    TickType_t t0, t1;
    uint32_t px = (uint32_t)st7789p3_width() * st7789p3_height();
    uint32_t ms;

    t0 = xTaskGetTickCount();
    st7789p3_fill_screen(ST_BLUE);
    t1 = xTaskGetTickCount();
    ms = (uint32_t)((t1 - t0) * portTICK_PERIOD_MS);

    printf("full-screen fill: %u px, %u ms", (unsigned)px, (unsigned)ms);
    if (ms) {
        printf("  (~%u fps, ~%u kB/s)", (unsigned)(1000u / ms),
               (unsigned)((px * 2u) / ms));
    }
    printf("\n");
}

static void cmd_demo(void)
{
    printf("bars...\n");     st7789p3_pattern_bars();
    vTaskDelay(pdMS_TO_TICKS(1500));
    printf("rgbtest...\n");  st7789p3_pattern_rgbtest();
    vTaskDelay(pdMS_TO_TICKS(1500));
    printf("gradient...\n"); st7789p3_pattern_gradient();
    vTaskDelay(pdMS_TO_TICKS(1500));
    printf("checker...\n");  st7789p3_pattern_checker(16);
    vTaskDelay(pdMS_TO_TICKS(1500));
    printf("frame...\n");    st7789p3_pattern_frame(ST_WHITE);
    printf("demo done\n");
}

int cmd_lcd(int argc, char **argv)
{
    const char *sub;
    int on;

    if (argc < 2) {
        usage();
        return 0;
    }
    sub = argv[1];

    if (strcmp(sub, "help") == 0) {
        usage();
    } else if (strcmp(sub, "status") == 0) {
        cmd_status();
    } else if (strcmp(sub, "init") == 0) {
        cmd_init(argc, argv);
    } else if (strcmp(sub, "id") == 0) {
        cmd_id();
    } else if (strcmp(sub, "clk") == 0) {
        if (argc < 3) {
            printf("sclk = %u Hz\n", (unsigned)st7789p3_get_spi_clk());
        } else {
            st7789p3_set_spi_clk((uint32_t)strtoul(argv[2], NULL, 0));
            printf("sclk requested %u Hz (re-run `lcd init` to be safe)\n",
                   (unsigned)st7789p3_get_spi_clk());
        }
    } else if (strcmp(sub, "framing") == 0) {
        if (argc < 3) {
            printf("framing = %s\n",
                   st7789p3_get_framing() == ST7789P3_FRAME_BYTE ? "byte"
                                                                 : "window");
        } else if (strcmp(argv[2], "byte") == 0) {
            st7789p3_set_framing(ST7789P3_FRAME_BYTE);
            printf("framing = byte\n");
        } else if (strcmp(argv[2], "window") == 0) {
            st7789p3_set_framing(ST7789P3_FRAME_WINDOW);
            printf("framing = window\n");
        } else {
            printf("usage: lcd framing <window|byte>\n");
        }
    } else if (strcmp(sub, "bl") == 0) {
        if (argc < 3 || !parse_onoff(argv[2], &on)) {
            printf("usage: lcd bl <on|off>\n");
        } else {
            st7789p3_set_backlight(on);
            printf("backlight %s\n", on ? "on" : "off");
        }
    } else if (strcmp(sub, "dir") == 0) {
        if (argc < 3) {
            printf("usage: lcd dir <0..3> [mirror]\n");
        } else if (!require_ready()) {
            /* message already printed */
        } else if (st7789p3_set_dir((uint8_t)atoi(argv[2]),
                                    (uint8_t)(argc > 3 ? atoi(argv[3]) : 0))
                   != ST7789P3_OK) {
            printf("dir must be 0..3\n");
        } else {
            printf("dir=%u -> %ux%u, madctl=0x%02X\n",
                   (unsigned)st7789p3_get_dir(), (unsigned)st7789p3_width(),
                   (unsigned)st7789p3_height(), st7789p3_get_madctl());
        }
    } else if (strcmp(sub, "bgr") == 0) {
        if (argc < 3 || !parse_onoff(argv[2], &on)) {
            printf("usage: lcd bgr <on|off>\n");
        } else if (require_ready()) {
            st7789p3_set_bgr(on);
            printf("colour order = %s, madctl=0x%02X\n", on ? "BGR" : "RGB",
                   st7789p3_get_madctl());
        }
    } else if (strcmp(sub, "inv") == 0) {
        if (argc < 3 || !parse_onoff(argv[2], &on)) {
            printf("usage: lcd inv <on|off>\n");
        } else if (require_ready()) {
            st7789p3_set_inversion(on);
            printf("inversion %s\n", on ? "on" : "off");
        }
    } else if (strcmp(sub, "sleep") == 0) {
        if (argc < 3) {
            printf("usage: lcd sleep <in|out>\n");
        } else if (require_ready()) {
            int in = (strcmp(argv[2], "in") == 0);
            st7789p3_set_sleep(in);
            printf("sleep %s\n", in ? "in" : "out");
        }
    } else if (strcmp(sub, "disp") == 0) {
        if (argc < 3 || !parse_onoff(argv[2], &on)) {
            printf("usage: lcd disp <on|off>\n");
        } else if (require_ready()) {
            st7789p3_set_display(on);
            printf("display %s\n", on ? "on" : "off");
        }
    } else if (strcmp(sub, "gamma") == 0) {
        if (argc < 3 || strcmp(argv[2], "default") != 0) {
            printf("usage: lcd gamma default\n");
        } else if (require_ready()) {
            st7789p3_gamma_default();
            printf("restored datasheet default gamma (E0h/E1h)\n");
        }
    } else if (strcmp(sub, "fill") == 0) {
        uint16_t c;
        if (argc < 3 || !parse_color(argv[2], &c)) {
            printf("usage: lcd fill <colour>\n");
        } else if (require_ready()) {
            st7789p3_fill_screen(c);
        }
    } else if (strcmp(sub, "box") == 0) {
        if (require_ready()) {
            cmd_box(argc, argv);
        }
    } else if (strcmp(sub, "bars") == 0) {
        if (require_ready()) { st7789p3_pattern_bars(); }
    } else if (strcmp(sub, "rgbtest") == 0) {
        if (require_ready()) {
            st7789p3_pattern_rgbtest();
            printf("bands top to bottom: RED GREEN BLUE WHITE BLACK\n"
                   "if the top band is blue and the third is red, run "
                   "`lcd bgr on`\n");
        }
    } else if (strcmp(sub, "checker") == 0) {
        if (require_ready()) {
            st7789p3_pattern_checker((uint16_t)(argc > 2 ? atoi(argv[2]) : 16));
        }
    } else if (strcmp(sub, "grad") == 0) {
        if (require_ready()) { st7789p3_pattern_gradient(); }
    } else if (strcmp(sub, "gray") == 0 || strcmp(sub, "grey") == 0) {
        if (require_ready()) { st7789p3_pattern_grayramp(); }
    } else if (strcmp(sub, "frame") == 0) {
        uint16_t c = ST_WHITE;
        if (argc > 2 && !parse_color(argv[2], &c)) {
            printf("bad colour '%s'\n", argv[2]);
        } else if (require_ready()) {
            st7789p3_pattern_frame(c);
        }
    } else if (strcmp(sub, "ruler") == 0) {
        if (require_ready()) { st7789p3_pattern_ruler(); }
    } else if (strcmp(sub, "bench") == 0) {
        if (require_ready()) { cmd_bench(); }
    } else if (strcmp(sub, "demo") == 0) {
        if (require_ready()) { cmd_demo(); }
    } else if (strcmp(sub, "cmd") == 0) {
        cmd_raw(argc, argv);
    } else if (strcmp(sub, "read") == 0) {
        cmd_read(argc, argv);
    } else if (strcmp(sub, "pin") == 0) {
        cmd_pin(argc, argv);
    } else {
        usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lcd, lcd, ST7789P3 SPI LCD test);

#endif /* CONFIG_SHELL */
