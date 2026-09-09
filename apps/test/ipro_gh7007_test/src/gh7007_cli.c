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
 * @file gh7007_cli.c
 * @brief Shell commands for GH7007-01 SPI LCD bring-up:
 *
 *   lcd init                 reset + run vendor init + clear black
 *   lcd fill <color|0xRGB>   solid fill (named or 0xRRGGBB)
 *   lcd bars                 8 vertical colour bars
 *   lcd checker [sq]         checkerboard (square size, default 16)
 *   lcd grad                 R/G/B/gray gradient bands
 *   lcd frame [color]        border + crosshair (edge/scaler reach check)
 *   lcd box x0 y0 x1 y1 <color>   fill a rectangle (window test)
 *   lcd dims <w> <h>         override GRAM input-plane size (default 320x240)
 *   lcd cmd <hex> [p0 p1..]  raw command + parameter bytes (debug)
 *   lcd pin <gpio> [0|1]     drive/read a pin for multimeter wire checks
 */

#include "gh7007.h"
#include "gh7007_patterns.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

static bool s_inited;

static bool require_init(void)
{
    if (!s_inited) {
        printf("LCD not initialised — run `lcd init` first.\n");
        return false;
    }
    return true;
}

/* Named colour -> RGB565, or parse 0xRRGGBB / 0xRGB565. Returns true on hit. */
static bool parse_color(const char *s, uint16_t *out)
{
    static const struct { const char *name; uint16_t v; } tbl[] = {
        { "black", GH_BLACK }, { "white", GH_WHITE }, { "red", GH_RED },
        { "green", GH_GREEN }, { "blue", GH_BLUE },   { "yellow", GH_YELLOW },
        { "cyan", GH_CYAN },   { "magenta", GH_MAGENTA }, { "gray", GH_GRAY },
        { "grey", GH_GRAY },
    };
    for (unsigned i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++) {
        if (strcmp(s, tbl[i].name) == 0) {
            *out = tbl[i].v;
            return true;
        }
    }
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        unsigned long v = strtoul(s, NULL, 16);
        if (v > 0xFFFF) {
            /* treat as 0xRRGGBB -> RGB565 */
            uint8_t r = (uint8_t)((v >> 16) & 0xFF);
            uint8_t g = (uint8_t)((v >> 8) & 0xFF);
            uint8_t b = (uint8_t)(v & 0xFF);
            *out = GH_RGB565(r, g, b);
        } else {
            *out = (uint16_t)v;        /* raw RGB565 */
        }
        return true;
    }
    return false;
}

static void cmd_fill(const char *arg)
{
    if (!require_init()) {
        return;
    }
    uint16_t c;
    if (!arg || !parse_color(arg, &c)) {
        printf("usage: lcd fill <black|white|red|green|blue|yellow|cyan|"
               "magenta|gray | 0xRRGGBB | 0xRGB565>\n");
        return;
    }
    gh7007_fill_screen(c);
    printf("filled 0x%04X (%ux%u plane)\n", c, gh7007_width(), gh7007_height());
}

static void cmd_video(int argc, char *argv[])
{
    if (!require_init()) {
        return;
    }
    uint16_t c;
    if (argc < 3 || !parse_color(argv[2], &c)) {
        printf("usage: lcd video <color> [frames]  (continuous frame stream)\n");
        return;
    }
    int frames = (argc > 3) ? atoi(argv[3]) : 120;
    printf("streaming %d frames of 0x%04X (watch the panel) ...\n", frames, c);
    gh7007_stream_color(c, frames);
    printf("done\n");
}

static void cmd_box(int argc, char *argv[])
{
    if (!require_init()) {
        return;
    }
    if (argc < 7) {
        printf("usage: lcd box <x0> <y0> <x1> <y1> <color>\n");
        return;
    }
    uint16_t x0 = (uint16_t)strtoul(argv[2], NULL, 0);
    uint16_t y0 = (uint16_t)strtoul(argv[3], NULL, 0);
    uint16_t x1 = (uint16_t)strtoul(argv[4], NULL, 0);
    uint16_t y1 = (uint16_t)strtoul(argv[5], NULL, 0);
    uint16_t c;
    if (!parse_color(argv[6], &c)) {
        printf("bad color '%s'\n", argv[6]);
        return;
    }
    gh7007_fill_rect(x0, y0, x1, y1, c);
    printf("box (%u,%u)-(%u,%u) = 0x%04X\n", x0, y0, x1, y1, c);
}

static void cmd_dims(int argc, char *argv[])
{
    if (argc >= 4) {
        gh7007_set_dims((uint16_t)strtoul(argv[2], NULL, 0),
                        (uint16_t)strtoul(argv[3], NULL, 0));
    }
    printf("input plane = %ux%u (scaler -> 1024x600 panel)\n",
           gh7007_width(), gh7007_height());
}

static void cmd_clk(int argc, char *argv[])
{
    if (argc >= 3) {
        /* accept "1000000", "1M", "8m", "500k" */
        char *end = NULL;
        unsigned long hz = strtoul(argv[2], &end, 0);
        if (end && (*end == 'M' || *end == 'm')) hz *= 1000000ul;
        else if (end && (*end == 'K' || *end == 'k')) hz *= 1000ul;
        gh7007_set_spi_clk((uint32_t)hz);
    }
    printf("SPI clock = %lu Hz  (re-run `lcd init` to redo the sequence at this speed)\n",
           (unsigned long)gh7007_get_spi_clk());
}

static void cmd_raw(int argc, char *argv[])
{
    if (!require_init()) {
        return;
    }
    if (argc < 3) {
        printf("usage: lcd cmd <hex cmd> [p0 p1 ...]\n");
        return;
    }
    uint8_t cmd = (uint8_t)strtoul(argv[2], NULL, 16);
    int n = argc - 3;
    uint8_t params[16];
    if (n > 16) {
        n = 16;
    }
    for (int i = 0; i < n; i++) {
        params[i] = (uint8_t)strtoul(argv[3 + i], NULL, 16);
    }
    /* one CS window: cmd (DCX=0) + params (DCX=1) — userguide register format */
    gh7007_write_reg(cmd, params, (uint32_t)(n > 0 ? n : 0));
    printf("cmd 0x%02X + %d param(s) sent\n", cmd, n > 0 ? n : 0);
}

static void cmd_pin(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: lcd pin <gpio> [0|1]   (omit level to read)\n"
               "  CS=18 DC=15 RST=14 SCLK=19 MOSI/SDA=16\n");
        return;
    }
    uint8_t pin = (uint8_t)strtoul(argv[2], NULL, 0);
    if (argc >= 4) {
        int lvl = (argv[3][0] == '1');
        gh7007_pin_out(pin, lvl);
        printf("GPIO%u driven %s\n", pin, lvl ? "HIGH" : "LOW");
    } else {
        printf("GPIO%u reads %d\n", pin, gh7007_pin_in(pin));
    }
}

static void usage(void)
{
    printf("usage: lcd <init|fill|bars|checker|grad|frame|box|dims|cmd|pin>\n"
           "  init               reset + vendor init + clear black\n"
           "  fill <color>       solid fill (named / 0xRRGGBB / 0xRGB565)\n"
           "  bars               8 vertical colour bars\n"
           "  checker [sq]       checkerboard (square px, default 16)\n"
           "  grad               R/G/B/gray gradient bands\n"
           "  image [frames]     real detailed picture via LCD_ShowPicture path\n"
           "  frame [color]      border + crosshair (edge reach check)\n"
           "  box x0 y0 x1 y1 c  fill a rectangle\n"
           "  dims <w> <h>       set GRAM input plane (default 320x240)\n"
           "  clk <hz|1M|500k>   set SPI clock (lower for flywire/clips), then re-init\n"
           "  cmd <hex> [p..]    raw command + params (debug)\n"
           "  pin <gpio> [0|1]   drive/read a pin (wire check)\n");
}

static int cmd_lcd(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        return 0;
    }
    const char *sub = argv[1];

    if (strcmp(sub, "init") == 0) {
        /* `lcd init [dspa04] [nc]` — dspa04 = alternative vendor sequence
         * (different GSRAM timing); nc = no trailing clear (pure register
         * sequence, zero 2C/3C video traffic). */
        int variant = 0, clear = 1, post11 = 0;
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "dspa04") == 0) {
                variant = 1;
            } else if (strcmp(argv[i], "v800") == 0) {
                variant = 2;    /* OTP-OK RGB 800x480 reference config */
            } else if (strcmp(argv[i], "hyb") == 0) {
                variant = 3;    /* SPI base + v800 glass blocks (800x480 glass) */
            } else if (strcmp(argv[i], "nc") == 0) {
                clear = 0;
            } else if (strcmp(argv[i], "post11") == 0) {
                post11 = 1;
            }
        }
        if (post11) {
            gh7007_init_post11(variant);
        } else {
            gh7007_init_select(variant, clear);
        }
        s_inited = true;
        printf("init sequence SENT (plane %ux%u -> 1024x600). Write-only bus: "
               "this only means the SoC clocked it out, NOT that the panel got "
               "it. Verify by display (flashlight ok) or VDD current jump. "
               "Try `lcd bars`.\n",
               gh7007_width(), gh7007_height());
    } else if (strcmp(sub, "dcsetup") == 0) {
        if (argc > 2) {
            gh7007_set_dc_setup((uint32_t)strtoul(argv[2], NULL, 10));
        }
        printf("dc setup = %lu us (spacing of DC-park / CS-fall / DC-drop at "
               "each command window)\n", (unsigned long)gh7007_get_dc_setup());
    } else if (strcmp(sub, "watch") == 0) {
        /* revert-latency stopwatch: prints elapsed seconds so the user can
         * note exactly when the display falls back to the power-on stripes,
         * and whether the interval is regular (periodic self-reset). */
        int secs = (argc > 2) ? atoi(argv[2]) : 30;
        printf("watch: note the number shown when the display reverts...\n");
        for (int s = 1; s <= secs; s++) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            printf("  t = %d s\n", s);
        }
        printf("watch done\n");
    } else if (strcmp(sub, "rsthiz") == 0) {
        /* release the RST line: G17 -> input w/ weak pull-up, SoC stops
         * actively driving it. If reverts change/stop, something on OUR side
         * was yanking RST; if unchanged, the chip resets itself internally. */
        int en = (argc > 2) ? atoi(argv[2]) : 1;
        gh7007_rst_release(en);
        printf(en ? "RST released to input + weak pull-up\n"
                  : "RST driven high again\n");
    } else if (strcmp(sub, "aggr") == 0) {
        int which = 3, secs = 5;
        if (argc > 2) {
            which = (argv[2][0] == 's') ? 1 : (argv[2][0] == 'd') ? 2 : 3;
        }
        if (argc > 3) {
            secs = atoi(argv[3]);
        }
        printf("aggressor: toggling %s for %ds with CS HIGH (chip must ignore "
               "this per DS p.46) — WATCH THE PANEL: does it revert to the "
               "power-on stripes?\n",
               which == 1 ? "SCLK" : which == 2 ? "MOSI" : "SCLK+MOSI", secs);
        gh7007_aggressor(which, secs);
        printf("aggressor done (bus pinmux restored, pad drive reset to 1)\n");
    } else if (strcmp(sub, "drive") == 0) {
        int v = 3, a = 0;
        if (argc > 2) {
            v = atoi(argv[2]);
        }
        if (argc > 3) {
            a = atoi(argv[3]);
        }
        gh7007_set_drive(v, a);
        printf("pad drive set: RST/CS/DC=%d, SCLK/MOSI=%d (0=weakest 3=strongest; "
               "re-apply after `lcd aggr`/`lcd bus`)\n", v & 3, a & 3);
    } else if (strcmp(sub, "paranoid") == 0) {
        if (argc > 2) {
            gh7007_set_paranoid(atoi(argv[2]));
        }
        printf("paranoid = %d (re-mux + reprogram SPI before every transaction)\n",
               gh7007_get_paranoid());
    } else if (strcmp(sub, "invtest") == 0) {
        /* Count-vs-time deafness isolation: pure-register init, then toggle
         * INVON/INVOFF from firmware — no human typing gaps. Watch the panel:
         *   blinks ~6x fast then more after the pause = interface stays alive
         *     when driven promptly -> TIME-based lock (chip goes deaf after idle)
         *   blinks once then never = hard count-lock after first command
         *   phase-2 blinks dead only = deafness kicks in between 3s and 10s idle */
        gh7007_init_select(0, 0);
        s_inited = true;
        printf("phase 1: 6 toggles at 500ms pace...\n");
        for (int i = 0; i < 6; i++) {
            gh7007_write_reg(0x21, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
            gh7007_write_reg(0x20, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
        printf("phase 2: 10s idle, then 4 toggles at 1s pace...\n");
        vTaskDelay(pdMS_TO_TICKS(10000));
        for (int i = 0; i < 4; i++) {
            gh7007_write_reg(0x21, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            gh7007_write_reg(0x20, NULL, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        printf("invtest done — report: how many blinks in phase 1 and phase 2?\n");
    } else if (strcmp(sub, "patch") == 0) {
        if (argc == 3 && strcmp(argv[2], "clear") == 0) {
            gh7007_patch_clear();
            printf("patches cleared\n");
        } else if (argc >= 5) {
            uint8_t pg = (uint8_t)strtoul(argv[2], NULL, 16);
            uint8_t rg = (uint8_t)strtoul(argv[3], NULL, 16);
            uint8_t vl = (uint8_t)strtoul(argv[4], NULL, 16);
            if (gh7007_patch_add(pg, rg, vl) < 0) {
                printf("patch table full (16)\n");
            } else {
                printf("patch p%02X r%02X = %02X (applied on every `lcd init`)\n",
                       pg, rg, vl);
            }
        } else {
            uint8_t lst[16][3];
            int n = gh7007_patch_list(lst, 16);
            printf("%d patch(es):\n", n);
            for (int i = 0; i < n; i++) {
                printf("  p%02X r%02X = %02X\n", lst[i][0], lst[i][1], lst[i][2]);
            }
            printf("usage: lcd patch <page> <reg> <val> (hex) | lcd patch clear\n");
        }
    } else if (strcmp(sub, "map") == 0) {
        if (argc == 3 && strcmp(argv[2], "off") == 0) {
            gh7007_set_map(0, 0, 0, 0, 0, 0);
            printf("map off — compile-time pins + HW SPI restored\n");
        } else if (argc >= 7) {
            uint8_t rst = (uint8_t)atoi(argv[2]);
            uint8_t cs  = (uint8_t)atoi(argv[3]);
            uint8_t scl = (uint8_t)atoi(argv[4]);
            uint8_t sda = (uint8_t)atoi(argv[5]);
            uint8_t dc  = (uint8_t)atoi(argv[6]);
            gh7007_set_map(1, rst, cs, scl, sda, dc);
            s_inited = false;
            printf("map ON (bit-bang): RST=G%u CS=G%u SCL=G%u SDA=G%u DC=G%u — "
                   "now `lcd sleeptest` then `lcd init nc`\n",
                   rst, cs, scl, sda, dc);
        } else {
            uint8_t m[5];
            int on = gh7007_get_map(m);
            printf("map %s: RST=G%u CS=G%u SCL=G%u SDA=G%u DC=G%u\n"
                   "usage: lcd map <rst> <cs> <scl> <sda> <dc> | lcd map off\n",
                   on ? "ON(bitbang)" : "off(default)", m[0], m[1], m[2], m[3], m[4]);
        }
    } else if (strcmp(sub, "sleeptest") == 0) {
        /* BINARY command-delivery test. SLPIN must stop the panel drive
         * entirely -> the stripes VANISH into a uniform field. "Stripes
         * gone yes/no" is unambiguous, unlike invert/brightness effects on a
         * garbage image. No init needed — works even on an OTP-auto-booted
         * module. */
        printf("baseline 5s: confirm stripes are visible now...\n");
        vTaskDelay(pdMS_TO_TICKS(5000));
        printf(">>> sending SLPIN (0x10)\n");
        gh7007_write_reg(0x10, NULL, 0);
        printf("Q1 (10s): did the stripes COMPLETELY VANISH into a uniform "
               "field?\n");
        vTaskDelay(pdMS_TO_TICKS(10000));
        printf(">>> sending SLPOUT (0x11) + DISPON (0x29)\n");
        gh7007_write_reg(0x11, NULL, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
        gh7007_write_reg(0x29, NULL, 0);
        printf("Q2 (10s): did the stripes COME BACK?\n");
        vTaskDelay(pdMS_TO_TICKS(10000));
        printf("sleeptest done — report Q1/Q2 as yes/no\n");
    } else if (strcmp(sub, "i2cscan") == 0) {
        uint8_t found[8];
        printf("scanning I2C on SCL=G%d SDA=G%d (bit-bang, ~50kHz)...\n",
               15, 16);
        int n = gh7007_i2c_scan(found, 8);
        if (n == 0) {
            printf("no ACK on 0x03..0x77 — module does not answer I2C on these "
                   "wires (CMD_SEL likely strapped to SPI, or no pull-ups)\n");
        } else {
            for (int i = 0; i < n; i++) {
                printf("ACK at 0x%02X\n", found[i]);
            }
            printf("=> module IS I2C-strapped (CMD_SEL=0). Command path must "
                   "use I2C, not SPI.\n");
        }
    } else if (strcmp(sub, "bus") == 0) {
        if (argc > 2) {
            gh7007_set_bus_3w(strcmp(argv[2], "3w") == 0);
        }
        printf("bus = %s  (re-run `lcd init`; 3w is bit-banged — init takes ~10s, "
               "a full fill tens of seconds)\n",
               gh7007_get_bus_3w() ? "3-wire (9-bit inline D/CX)" : "4-wire (DCX pin)");
    } else if (strcmp(sub, "mode") == 0) {
        if (argc > 2) {
            gh7007_set_spi_mode(atoi(argv[2]));
        }
        printf("SPI mode = %d  (re-run `lcd init` to redo the sequence in this mode)\n",
               gh7007_get_spi_mode());
    } else if (strcmp(sub, "atomic") == 0) {
        if (argc > 2) {
            gh7007_set_atomic(atoi(argv[2]));
        }
        printf("atomic = %d (scheduler suspended across each row transfer)\n",
               gh7007_get_atomic());
    } else if (strcmp(sub, "rot") == 0) {
        if (argc >= 5) {
            gh7007_set_rot((uint8_t)atoi(argv[2]), (uint8_t)atoi(argv[3]),
                           (uint8_t)atoi(argv[4]));
        }
        uint8_t ro[3];
        gh7007_get_rot(ro);
        printf("rot: R5=%u B6=%u G5=%u (pre-rotate-right amounts; tune with "
               "`lcd grad` until all bands ramp smoothly)\n", ro[0], ro[1], ro[2]);
    } else if (strcmp(sub, "bitrev") == 0) {
        if (argc > 2) {
            gh7007_set_bitrev(atoi(argv[2]));
        }
        printf("bitrev = %d (pre-reverse channel bits, chip is LSB-first)\n",
               gh7007_get_bitrev());
    } else if (strcmp(sub, "swapgb") == 0) {
        if (argc > 2) {
            gh7007_set_swap_gb(atoi(argv[2]));
        }
        printf("swapgb = %d (repack pixels for the chip's R5-B5-G6 layout)\n",
               gh7007_get_swap_gb());
    } else if (strcmp(sub, "wr8") == 0) {
        if (argc > 2) {
            gh7007_set_wr8(atoi(argv[2]));
        }
        printf("wr8 = %d (vendor per-byte CS-window framing)\n", gh7007_get_wr8());
    } else if (strcmp(sub, "fill3") == 0) {
        if (require_init()) {
            uint16_t c = GH_RED;
            if (argc > 2) {
                parse_color(argv[2], &c);
            }
            gh7007_fill_vendor(c);
            printf("fill3 (vendor DMA framing: 2A/2B, 2C own window, frame in one "
                   "window) sent\n");
        }
    } else if (strcmp(sub, "box2") == 0) {
        if (require_init()) {
            uint16_t c = GH_RED;
            if (argc > 2) {
                parse_color(argv[2], &c);
            }
            /* centered 100x60 box via DBI windowing */
            gh7007_box_windowed(110, 90, 209, 149, c);
            printf("box2: windowed 100x60 box at center (110,90)-(209,149). "
                   "Landed centered = partial refresh OK; top-left/absent = "
                   "full-frame only.\n");
        }
    } else if (strcmp(sub, "fill2") == 0) {
        if (require_init()) {
            uint16_t c = GH_RED;
            if (argc > 2) {
                parse_color(argv[2], &c);
            }
            gh7007_fill_dbi(c);
            printf("fill2 (standard DBI: CASET/RASET + single-window RAMWR) sent\n");
        }
    } else if (strcmp(sub, "fill") == 0) {
        cmd_fill(argc > 2 ? argv[2] : NULL);
    } else if (strcmp(sub, "video") == 0) {
        cmd_video(argc, argv);
    } else if (strcmp(sub, "bitprobe") == 0) {
        if (require_init()) {
            gh7007_set_bitrev(0);
            gh7007_pattern_bitprobe();
            printf("bitprobe (bitrev forced OFF): 3 bands = R(5), G(6), B(5) "
                   "segments, left=our MSB. Report each band's brightness "
                   "order left-to-right (e.g. R: 3rd brightest, then 1st...)\n");
        }
    } else if (strcmp(sub, "demo") == 0) {
        /* one-key acceptance: init + full pattern tour */
        gh7007_init_select(0, 1);
        s_inited = true;
        printf("demo: bars...\n");
        gh7007_pattern_bars();
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("demo: gradient...\n");
        gh7007_pattern_gradient();
        vTaskDelay(pdMS_TO_TICKS(2000));
        printf("demo: fills...\n");
        gh7007_fill_screen(GH_RED);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gh7007_fill_screen(GH_GREEN);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gh7007_fill_screen(GH_BLUE);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gh7007_fill_screen(GH_WHITE);
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("demo: frame...\n");
        gh7007_pattern_frame(GH_RED);
        printf("demo done\n");
    } else if (strcmp(sub, "ruler") == 0) {
        if (require_init()) {
            gh7007_pattern_ruler();
            printf("ruler: WHITE lines at x=0/100/200/300/400/500, RED at "
                   "50/150/.../450; left ticks every 50 rows (WHITE=0/100/200/300, "
                   "RED=50/150/250). Photograph and read the rightmost/lowest "
                   "visible marker.\n");
        }
    } else if (strcmp(sub, "bars") == 0) {
        if (require_init()) {
            gh7007_pattern_bars();
            printf("colour bars\n");
        }
    } else if (strcmp(sub, "checker") == 0) {
        if (require_init()) {
            gh7007_pattern_checker(argc > 2 ? (uint16_t)atoi(argv[2]) : 16);
            printf("checkerboard\n");
        }
    } else if (strcmp(sub, "grad") == 0) {
        if (require_init()) {
            gh7007_pattern_gradient();
            printf("gradient\n");
        }
    } else if (strcmp(sub, "image") == 0) {
        if (require_init()) {
            int frames = (argc > 2) ? atoi(argv[2]) : 1;
            printf("streaming %d image frame(s) via LCD_ShowPicture path "
                   "(0x2C/0x3C) ...\n", frames);
            gh7007_pattern_image(frames);
            printf("done\n");
        }
    } else if (strcmp(sub, "frame") == 0) {
        if (require_init()) {
            uint16_t c = GH_WHITE;
            if (argc > 2) {
                parse_color(argv[2], &c);
            }
            gh7007_pattern_frame(c);
            printf("frame + crosshair\n");
        }
    } else if (strcmp(sub, "box") == 0) {
        cmd_box(argc, argv);
    } else if (strcmp(sub, "dims") == 0) {
        cmd_dims(argc, argv);
    } else if (strcmp(sub, "clk") == 0) {
        cmd_clk(argc, argv);
    } else if (strcmp(sub, "cmd") == 0) {
        cmd_raw(argc, argv);
    } else if (strcmp(sub, "pin") == 0) {
        cmd_pin(argc, argv);
    } else {
        usage();
    }
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_lcd, lcd, GH7007 SPI LCD test);

#endif /* CONFIG_SHELL */
