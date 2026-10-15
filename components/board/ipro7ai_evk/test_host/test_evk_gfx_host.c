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
 * Host tests for the row-compositing gfx helpers against a virtual panel.
 *
 *   sh components/board/ipro7ai_evk/test_host/run.sh
 */
#include <stdio.h>
#include <string.h>

#include "evk_gfx.h"
#include "st7789p3_stub.h"

static int g_fail, g_run;

#define CHECK(cond, ...) do {                                      \
    g_run++;                                                       \
    if (!(cond)) { g_fail++;                                       \
        printf("FAIL %s:%d  ", __func__, __LINE__);                \
        printf(__VA_ARGS__); printf("\n"); }                       \
} while (0)

static int count_color(int x0, int y0, int x1, int y1, uint16_t wire)
{
    int n = 0;
    for (int y = y0; y <= y1; y++)
        for (int x = x0; x <= x1; x++)
            n += stub_gram(x, y) == wire;
    return n;
}

static void test_text_metrics(void)
{
    CHECK(evk_text_width("", 1) == 0, "empty width");
    CHECK(evk_text_width("A", 1) == (FONT_ADVANCE - 1) + 2 * EVK_GFX_PAD, "one glyph: %d",
          evk_text_width("A", 1));
    CHECK(evk_text_width("AB", 2) == (2 * FONT_ADVANCE - 1) * 2 + 2 * EVK_GFX_PAD, "two glyphs x2");
    CHECK(evk_text_height(1) == FONT_H + 2 * EVK_GFX_PAD, "height x1");
    CHECK(evk_text_height(3) == FONT_H * 3 + 2 * EVK_GFX_PAD, "height x3");

    char longs[80];
    memset(longs, 'X', sizeof(longs) - 1);
    longs[sizeof(longs) - 1] = '\0';
    evk_text_t t;
    evk_text_set(&t, longs, 1, ST_WHITE, false, 0, 0);
    CHECK(t.len == EVK_GFX_MAX_CHARS, "clips to %d chars: %d", EVK_GFX_MAX_CHARS, t.len);
    CHECK(evk_text_width(longs, 1) == t.w, "width agrees with the clipped box");
    evk_text_set(&t, NULL, 1, ST_WHITE, false, 0, 0);
    CHECK(t.len == 0, "NULL is empty");
}

static void test_text_row(void)
{
    uint16_t row[64];
    evk_text_t t;
    const uint16_t fg = st7789p3_swap(ST_GREEN), bg = st7789p3_swap(ST_BLACK);

    /* "I" at (10, 5), scale 1, with backing: rows 5..15 get a black box
     * 10..10+w-1, glyph pixels inside it. */
    evk_text_set(&t, "I", 1, ST_GREEN, true, 10, 5);
    for (int y = 0; y < 20; y++) {
        for (int i = 0; i < 64; i++) row[i] = 0x1234;
        evk_text_row(&t, row, 64, y);
        int in_box = (y >= t.y && y < t.y + t.h);
        CHECK(row[9] == 0x1234 && row[10 + t.w] == 0x1234, "row %d: untouched outside the box", y);
        if (!in_box) {
            CHECK(row[10] == 0x1234, "row %d: outside the box nothing drawn", y);
        } else {
            CHECK(row[10] == bg || row[10] == fg, "row %d: inside the box is bg or fg", y);
        }
    }
    /* The glyph row for the vertical bar of "I" (middle rows) has fg in the
     * centre column. */
    int hits = 0;
    for (int y = t.y + EVK_GFX_PAD; y < t.y + EVK_GFX_PAD + FONT_H; y++) {
        for (int i = 0; i < 64; i++) row[i] = 0;
        evk_text_row(&t, row, 64, y);
        for (int x = 0; x < 64; x++) hits += row[x] == fg;
    }
    CHECK(hits > 0, "some glyph pixels drawn (%d)", hits);

    /* Clipping: a box starting at x = -3 must not write before index 0. */
    evk_text_set(&t, "AB", 1, ST_WHITE, true, -3, 0);
    for (int i = 0; i < 64; i++) row[i] = 0;
    evk_text_row(&t, row, 8, 3);                  /* narrow row: clipped right too */
    CHECK(row[8] == 0 && row[9] == 0, "no write past the row width");
}

static void test_rect_row(void)
{
    uint16_t row[32];
    evk_rect_t r = { .x1 = 4, .y1 = 2, .x2 = 20, .y2 = 10, .thickness = 2,
                     .color = st7789p3_swap(ST_RED) };

    for (int i = 0; i < 32; i++) row[i] = 0;
    evk_rect_row(&r, row, 32, 1);
    CHECK(row[4] == 0 && row[20] == 0, "above the box: nothing");

    for (int i = 0; i < 32; i++) row[i] = 0;
    evk_rect_row(&r, row, 32, 2);                 /* top edge: solid */
    int n = 0; for (int x = 4; x <= 20; x++) n += row[x] == r.color;
    CHECK(n == 17 && row[3] == 0 && row[21] == 0, "top edge solid 4..20 (%d)", n);

    for (int i = 0; i < 32; i++) row[i] = 0;
    evk_rect_row(&r, row, 32, 6);                 /* middle: 2 px each side */
    CHECK(row[4] == r.color && row[5] == r.color && row[6] == 0 &&
          row[19] == r.color && row[20] == r.color && row[18] == 0, "sides 2 px thick");

    for (int i = 0; i < 32; i++) row[i] = 0;
    evk_rect_row(&r, row, 32, 9);                 /* within thickness of the bottom */
    n = 0; for (int x = 4; x <= 20; x++) n += row[x] == r.color;
    CHECK(n == 17, "bottom edge (2 rows thick) solid (%d)", n);

    /* Clipping to the row width. */
    r.x2 = 40;
    for (int i = 0; i < 32; i++) row[i] = 0;
    evk_rect_row(&r, row, 32, 6);
    CHECK(row[31] == 0 || row[31] == r.color, "right side clipped");
}

static void test_yuyv(void)
{
    /* Two pixels per word [Y0 U Y1 V]: mid grey both -> RGB565 0x8410. */
    uint32_t src[2] = { 0x80808080u, 0x80FF80FFu };  /* pixels 2,3: Y=128 U=255 V=255 -> reddish/blue */
    uint16_t dst[4];

    evk_gfx_yuyv_row(src, dst, 4, false, false);
    CHECK(dst[0] == st7789p3_swap(ST_RGB565(128, 128, 128)), "grey pixel 0 = %04x", dst[0]);
    CHECK(dst[1] == dst[0], "grey pixel 1 identical");
    CHECK(dst[2] != dst[0], "chroma changes the colour");

    uint16_t mir[4];
    evk_gfx_yuyv_row(src, mir, 4, false, true);
    CHECK(mir[3] == dst[0] && mir[2] == dst[1] && mir[1] == dst[2] && mir[0] == dst[3],
          "mirror reverses the row");

    /* uv_swap exchanges Cb/Cr: for U=V it changes nothing. */
    uint16_t sw[4];
    evk_gfx_yuyv_row(src, sw, 4, true, false);
    CHECK(sw[0] == dst[0] && sw[2] == dst[2], "uv swap with U==V is identical");
}

static void test_direct_draw(void)
{
    stub_reset();
    evk_gfx_draw_bar(0, 30, NULL, NULL, ST_BLUE);
    CHECK(count_color(0, 0, 319, 29, st7789p3_swap(ST_BLUE)) == 320 * 30, "bar fills 320x30");
    CHECK(stub_gram(0, 30) == 0xEEEE, "nothing below the bar");
    CHECK(g_lock_errors == 0 && g_lock_depth == 0, "bar took and released the lock");

    stub_reset();
    evk_gfx_text_at(100, 50, "HI", 2, ST_WHITE, ST_RED);
    int w = evk_text_width("HI", 2), h = evk_text_height(2);
    int red = count_color(100, 50, 100 + w - 1, 50 + h - 1, st7789p3_swap(ST_RED));
    int white = count_color(100, 50, 100 + w - 1, 50 + h - 1, st7789p3_swap(ST_WHITE));
    CHECK(red + white == w * h, "box fully painted (%d + %d of %d)", red, white, w * h);
    CHECK(white > 0, "glyph pixels present");
    CHECK(stub_gram(99, 50) == 0xEEEE && stub_gram(100 + w, 50) == 0xEEEE, "outside untouched");
    CHECK(g_lock_errors == 0, "text_at locked the bus");

    stub_reset();
    evk_gfx_text_at(315, 235, "WIDE TEXT", 2, ST_WHITE, ST_RED);
    CHECK(g_lock_errors == 0, "off-panel text does not break the burst");

    stub_reset();
    evk_gfx_splash("TITLE", "subtitle");
    CHECK(count_color(0, 0, 319, 239, st7789p3_swap(ST_BLACK)) > 320 * 200, "splash clears to black");
    CHECK(count_color(0, 0, 319, 239, st7789p3_swap(ST_WHITE)) > 0, "title drawn");
    CHECK(count_color(0, 0, 319, 239, st7789p3_swap(ST_GRAY)) > 0, "subtitle drawn");
    CHECK(g_lock_errors == 0 && g_lock_depth == 0, "splash lock balanced");
}

int main(void)
{
    stub_reset();
    test_text_metrics();
    test_text_row();
    test_rect_row();
    test_yuyv();
    test_direct_draw();
    printf("evk_gfx: %d checks, %d failed\n", g_run, g_fail);
    return g_fail ? 1 : 0;
}
