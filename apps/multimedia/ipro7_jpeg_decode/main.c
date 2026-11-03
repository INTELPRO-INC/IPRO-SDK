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
 * @brief JPEG -> RGB565 / RGB888 sample for IPRO7, driven from the shell.
 *
 *   jdec list                                  built-in test images
 *   jdec info <n|rx>                           what jdec_probe() reports
 *   jdec mem                                   stack and heap used by a few decodes
 *   jdec run <n|all> [565|565be|888] [scale] [blocks]
 *                                              decode, check, time; scale 0..3 = 1/1 .. 1/8;
 *                                              "blocks" uses the per-block callback instead of
 *                                              a whole-picture buffer
 *   jdec dump <n|rx> [565|888] [scale]         decode and print the pixels as base64
 *                                              (host/jdec_host.py turns them into a PNG)
 *   jdec rx <bytes> / jdec put <base64> / jdec go [565|565be|888] [scale]
 *                                              receive a JPEG over the console and decode it
 *
 * Checks for the built-in images: the output hash must equal the one of the
 * same decoder built on the PC (bit-exact), and the RGB mean of every 16x16
 * block (64x64 above 640 pixels wide) must stay within MAX_DIFF of libjpeg.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>

#include "freertos_app_hooks.h"
#include "shell.h"
#include "drv_clock.h"
#include "hal_mtimer.h"

#include "jdec_rgb.h"
#include "test_images.h"

#define MAX_DIFF 10                 /* block-mean tolerance against libjpeg (TJpgDec: up to 8) */
#define RX_MAX   (1024u * 1024u)
#define B64_LINE 96u                /* raw bytes per base64 output line */

static uint8_t *s_rx;
static uint32_t s_rx_len, s_rx_cap;

static const char *const k_samp[] = { "4:4:4", "4:2:2", "4:2:0", "grey" };
static const char *const k_fmt[]  = { "565", "565be", "888" };

/* ------------------------------------------------------------------ */

static uint32_t fnv1a(uint32_t h, const uint8_t *p, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        h = (h ^ p[i]) * 16777619u;
    }
    return h;
}

static uint32_t bpp(jdec_fmt_t fmt)
{
    return fmt == JDEC_RGB888 ? 3u : 2u;
}

/* [565|565be|888] [scale] [blocks] from argv[first] on; returns -1 on a bad word. */
static int parse_opts(int argc, char **argv, int first, jdec_fmt_t *fmt, uint8_t *scale, bool *blocks)
{
    *fmt    = JDEC_RGB565;
    *scale  = 0;
    *blocks = false;
    for (int i = first; i < argc; i++) {
        if (!strcmp(argv[i], "565")) {
            *fmt = JDEC_RGB565;
        } else if (!strcmp(argv[i], "565be")) {
            *fmt = JDEC_RGB565_BE;
        } else if (!strcmp(argv[i], "888")) {
            *fmt = JDEC_RGB888;
        } else if (!strcmp(argv[i], "blocks")) {
            *blocks = true;
        } else if (argv[i][0] >= '0' && argv[i][0] <= '3' && argv[i][1] == '\0') {
            *scale = (uint8_t)(argv[i][0] - '0');
        } else {
            return -1;
        }
    }
    return 0;
}

static const test_image_t *image_arg(const char *s)
{
    char    *end;
    unsigned n = (unsigned)strtoul(s, &end, 10);

    if (*end == '\0' && n < g_test_image_count) {
        return &g_test_images[n];
    }
    for (unsigned i = 0; i < g_test_image_count; i++) {
        if (!strcmp(s, g_test_images[i].name)) {
            return &g_test_images[i];
        }
    }
    return NULL;
}

/* Built-in image or the one received with rx / put. */
static int source_arg(const char *s, const uint8_t **jpg, uint32_t *len, const test_image_t **t)
{
    *t = NULL;
    if (!strcmp(s, "rx")) {
        if (s_rx == NULL || s_rx_len == 0u) {
            printf("nothing received (jdec rx / put)\r\n");
            return -1;
        }
        *jpg = s_rx;
        *len = s_rx_len;
        return 0;
    }
    *t = image_arg(s);
    if (*t == NULL) {
        printf("no image %s (jdec list)\r\n", s);
        return -1;
    }
    *jpg = (*t)->jpg;
    *len = (*t)->len;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Block mode: what an LCD driver would do with each block            */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t *frame; /* here only to prove the result equals whole-picture mode */
    uint32_t pitch, bpp, blocks;
    uint16_t ow, oh; /* jdec_out_dims() */
} blocks_t;

static int put_block(void *user, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *px)
{
    blocks_t *b = user;

    if ((uint32_t)x + w > b->ow || (uint32_t)y + h > b->oh) {
        return 0; /* never write outside the destination, whatever the decoder says */
    }
    for (uint32_t r = 0; r < h; r++) {
        memcpy(b->frame + (y + r) * b->pitch + x * b->bpp, px + r * w * b->bpp, (size_t)w * b->bpp);
    }
    b->blocks++;
    return 1;
}

/* ------------------------------------------------------------------ */
/* Check against the libjpeg block means                              */
/* ------------------------------------------------------------------ */

static void px_rgb(const uint8_t *p, jdec_fmt_t fmt, int *r, int *g, int *b)
{
    uint16_t v;

    if (fmt == JDEC_RGB888) {
        *r = p[0];
        *g = p[1];
        *b = p[2];
        return;
    }
    v  = fmt == JDEC_RGB565 ? (uint16_t)(p[0] | (p[1] << 8)) : (uint16_t)((p[0] << 8) | p[1]);
    *r = ((v >> 11) << 3) | (v >> 13);
    *g = (((v >> 5) & 0x3F) << 2) | ((v >> 9) & 0x03);
    *b = ((v & 0x1F) << 3) | ((v >> 2) & 0x07);
}

static int block_mean_diff(const test_image_t *t, const uint8_t *rgb, uint16_t width, jdec_fmt_t fmt)
{
    uint32_t area = (uint32_t)t->blk * t->blk;
    int      worst = 0;

    for (uint32_t by = 0; by < t->bh; by++) {
        for (uint32_t bx = 0; bx < t->bw; bx++) {
            uint32_t sum[3] = { 0, 0, 0 };

            for (uint32_t y = by * t->blk; y < (by + 1u) * t->blk; y++) {
                const uint8_t *row = rgb + (y * width + bx * t->blk) * bpp(fmt);

                for (uint32_t x = 0; x < t->blk; x++) {
                    int r, g, b;

                    px_rgb(row + x * bpp(fmt), fmt, &r, &g, &b);
                    sum[0] += (uint32_t)r;
                    sum[1] += (uint32_t)g;
                    sum[2] += (uint32_t)b;
                }
            }
            for (uint32_t k = 0; k < 3; k++) {
                int d = abs((int)((sum[k] + area / 2u) / area) - t->ref[(by * t->bw + bx) * 3u + k]);

                worst = d > worst ? d : worst;
            }
        }
    }
    return worst;
}

/* ------------------------------------------------------------------ */

static int run_one(unsigned idx, const test_image_t *t, const uint8_t *jpg, uint32_t len, jdec_fmt_t fmt,
                   uint8_t scale, bool blocks)
{
    const char *name = t != NULL ? t->name : "rx";
    jdec_info_t info;
    uint16_t    ow, oh;
    uint32_t    size, hash, want = 0;
    uint8_t    *frame;
    uint64_t    t0;
    uint32_t    us, tenths;
    blocks_t    bl;
    int         ret, diff = -1;
    bool        pass;

    ret = jdec_probe(jpg, len, &info);
    if (t != NULL && t->expect != JDEC_OK) {
        pass = ret == t->expect;
        printf("[%2u] %-16s rejected: %d %s -> %s\r\n", idx, name, ret, jdec_strerror(ret), pass ? "PASS" : "FAIL");
        return pass ? 0 : -1;
    }
    if (ret != JDEC_OK) {
        printf("[%2u] %-16s %d %s -> FAIL\r\n", idx, name, ret, jdec_strerror(ret));
        return -1;
    }
    jdec_out_dims(&info, scale, &ow, &oh);
    size  = jdec_out_size(&info, fmt, scale);
    frame = size != 0u ? pvPortMalloc(size) : NULL;
    if (frame == NULL) {
        printf("[%2u] %-16s %ux%u: no memory for the output -> FAIL\r\n", idx, name, ow, oh);
        return -1;
    }
    t0 = mtimer_get_time_us();
    if (blocks) {
        bl.frame  = frame;
        bl.bpp    = bpp(fmt);
        bl.pitch  = (uint32_t)ow * bl.bpp;
        bl.blocks = 0;
        bl.ow     = ow;
        bl.oh     = oh;
        ret = jdec_decode_blocks(jpg, len, fmt, scale, put_block, &bl, NULL);
    } else {
        ret = jdec_decode(jpg, len, fmt, scale, frame, size, NULL);
    }
    us = (uint32_t)(mtimer_get_time_us() - t0);
    if (ret != JDEC_OK) {
        printf("[%2u] %-16s decode error %d %s -> FAIL\r\n", idx, name, ret, jdec_strerror(ret));
        vPortFree(frame);
        return -1;
    }
    hash = fnv1a(2166136261u, frame, size);
    if (t != NULL && scale == 0u) {
        want = fmt == JDEC_RGB565 ? t->fnv565 : (fmt == JDEC_RGB888 ? t->fnv888 : 0u);
        diff = t->ref != NULL ? block_mean_diff(t, frame, ow, fmt) : -1;
    }
    vPortFree(frame);
    pass = (want == 0u || hash == want) && diff <= MAX_DIFF;
    tenths = (uint32_t)((uint64_t)us * 10000u / ((uint32_t)info.width * info.height)); /* 0.1 ms per Mpix */
    printf("[%2u] %-16s %4ux%-4u %-5s -> %4ux%-4u %-5s %7lu us %4lu.%lu ms/Mpix  fnv %08lx%s", idx, name,
           info.width, info.height, k_samp[info.samp], ow, oh, k_fmt[fmt], (unsigned long)us,
           (unsigned long)(tenths / 10u), (unsigned long)(tenths % 10u), (unsigned long)hash,
           want == 0u ? "" : (hash == want ? " =host" : " !=host"));
    if (diff >= 0) {
        printf("  libjpeg diff %d", diff);
    }
    if (blocks) {
        printf("  %lu blocks", (unsigned long)bl.blocks);
    }
    printf(" -> %s\r\n", pass ? "PASS" : "FAIL");
    return pass ? 0 : -1;
}

static int cmd_run(int argc, char **argv)
{
    jdec_fmt_t fmt;
    uint8_t    scale;
    bool       blocks;
    int        fails = 0;

    if (argc < 3 || parse_opts(argc, argv, 3, &fmt, &scale, &blocks)) {
        printf("jdec run <n|name|all> [565|565be|888] [scale 0..3] [blocks]\r\n");
        return -1;
    }
    if (!strcmp(argv[2], "all")) {
        for (unsigned i = 0; i < g_test_image_count; i++) {
            const test_image_t *t = &g_test_images[i];

            fails += run_one(i, t, t->jpg, t->len, fmt, scale, blocks) != 0;
        }
        printf("%u images, %d failed -> %s\r\n", g_test_image_count, fails, fails ? "FAIL" : "ALL PASS");
        return fails ? -1 : 0;
    }
    {
        const test_image_t *t = image_arg(argv[2]);

        if (t == NULL) {
            printf("no image %s (jdec list)\r\n", argv[2]);
            return -1;
        }
        return run_one((unsigned)(t - g_test_images), t, t->jpg, t->len, fmt, scale, blocks);
    }
}

/* ------------------------------------------------------------------ */
/* Base64 in / out                                                    */
/* ------------------------------------------------------------------ */

static const char k_b64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void b64_line(const uint8_t *p, uint32_t n)
{
    char     line[B64_LINE / 3u * 4u + 4u];
    uint32_t o = 0;

    for (uint32_t i = 0; i < n; i += 3) {
        uint32_t v = (uint32_t)p[i] << 16;

        v |= i + 1 < n ? (uint32_t)p[i + 1] << 8 : 0u;
        v |= i + 2 < n ? p[i + 2] : 0u;
        line[o++] = k_b64[(v >> 18) & 63];
        line[o++] = k_b64[(v >> 12) & 63];
        line[o++] = i + 1 < n ? k_b64[(v >> 6) & 63] : '=';
        line[o++] = i + 2 < n ? k_b64[v & 63] : '=';
    }
    line[o] = '\0';
    printf("%s\r\n", line);
}

static int b64_val(char c)
{
    const char *p = c ? strchr(k_b64, c) : NULL;

    return p != NULL ? (int)(p - k_b64) : -1;
}

static int cmd_dump(int argc, char **argv)
{
    const test_image_t *t;
    const uint8_t      *jpg;
    uint32_t            len, size;
    jdec_info_t         info;
    jdec_fmt_t          fmt;
    uint8_t             scale;
    uint16_t            ow, oh;
    uint8_t            *frame;
    bool                blocks;
    int                 ret;

    if (argc < 3 || parse_opts(argc, argv, 3, &fmt, &scale, &blocks) || fmt == JDEC_RGB565_BE) {
        printf("jdec dump <n|name|rx> [565|888] [scale 0..3]\r\n");
        return -1;
    }
    if (source_arg(argv[2], &jpg, &len, &t)) {
        return -1;
    }
    ret = jdec_probe(jpg, len, &info);
    if (ret != JDEC_OK) {
        printf("error %d: %s\r\n", ret, jdec_strerror(ret));
        return -1;
    }
    size  = jdec_out_size(&info, fmt, scale);
    frame = size != 0u ? pvPortMalloc(size) : NULL;
    if (frame == NULL) {
        printf("error: no memory for the output (%ux%u)\r\n", info.width, info.height);
        return -1;
    }
    ret = jdec_decode(jpg, len, fmt, scale, frame, size, NULL);
    if (ret != JDEC_OK) {
        printf("error %d: %s\r\n", ret, jdec_strerror(ret));
        vPortFree(frame);
        return -1;
    }
    jdec_out_dims(&info, scale, &ow, &oh);
    printf("-----BEGIN JDEC fmt=%s w=%u h=%u bytes=%lu-----\r\n", fmt == JDEC_RGB888 ? "rgb888" : "rgb565le", ow,
           oh, (unsigned long)size);
    for (uint32_t i = 0; i < size; i += B64_LINE) {
        b64_line(frame + i, size - i < B64_LINE ? size - i : B64_LINE);
    }
    printf("-----END JDEC fnv=%08lx-----\r\n", (unsigned long)fnv1a(2166136261u, frame, size));
    vPortFree(frame);
    return 0;
}

static int cmd_rx(int argc, char **argv)
{
    uint32_t n = argc > 2 ? (uint32_t)strtoul(argv[2], NULL, 0) : 0u;

    if (n == 0u || n > RX_MAX) {
        printf("jdec rx <bytes>  (1 .. %u)\r\n", RX_MAX);
        return -1;
    }
    vPortFree(s_rx);
    s_rx     = pvPortMalloc(n);
    s_rx_cap = s_rx != NULL ? n : 0u;
    s_rx_len = 0;
    printf("rx %lu\r\n", (unsigned long)s_rx_cap);
    return s_rx != NULL ? 0 : -1;
}

static int cmd_put(int argc, char **argv)
{
    const char *s    = argc > 2 ? argv[2] : "";
    uint32_t    acc  = 0;
    int         bits = 0;

    for (; *s && *s != '='; s++) {
        int v = b64_val(*s);

        if (v < 0) {
            printf("put: bad base64\r\n");
            return -1;
        }
        acc = (acc << 6) | (uint32_t)v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            if (s_rx_len >= s_rx_cap) {
                printf("put: overflow\r\n");
                return -1;
            }
            s_rx[s_rx_len++] = (uint8_t)(acc >> bits);
        }
    }
    printf("put %lu/%lu\r\n", (unsigned long)s_rx_len, (unsigned long)s_rx_cap);
    return 0;
}

static int cmd_info(int argc, char **argv)
{
    const test_image_t *t;
    const uint8_t      *jpg;
    uint32_t            len;
    jdec_info_t         in;
    int                 ret;

    if (argc < 3 || source_arg(argv[2], &jpg, &len, &t)) {
        printf("jdec info <n|name|rx>\r\n");
        return -1;
    }
    ret = jdec_probe(jpg, len, &in);
    printf("%s: %lu B, probe %d (%s)\r\n", t != NULL ? t->name : "rx", (unsigned long)len, ret, jdec_strerror(ret));
    if (ret == JDEC_OK) {
        printf("  %ux%u %s, header %lu B (APPn/COM %lu B), restart %u, %s Huffman tables;"
               " RGB565 needs %lu B, RGB888 %lu B\r\n",
               in.width, in.height, k_samp[in.samp], (unsigned long)in.header_len, (unsigned long)in.app_len,
               in.restart_interval, in.std_huffman ? "standard (none in file)" : "file's",
               (unsigned long)jdec_out_size(&in, JDEC_RGB565, 0), (unsigned long)jdec_out_size(&in, JDEC_RGB888, 0));
    }
    return 0;
}

/* Stack and heap around a few decodes, for sizing the task that will decode. */
static void cmd_mem(void)
{
    UBaseType_t before = uxTaskGetStackHighWaterMark(NULL);
    size_t      heap   = xPortGetFreeHeapSize();

    run_one(0, &g_test_images[0], g_test_images[0].jpg, g_test_images[0].len, JDEC_RGB888, 0, false);
    run_one(2, &g_test_images[2], g_test_images[2].jpg, g_test_images[2].len, JDEC_RGB565, 0, true);
    run_one(6, &g_test_images[6], g_test_images[6].jpg, g_test_images[6].len, JDEC_RGB565, 3, false);
    printf("stack high-water: %lu B unused before, %lu B after (this task's stack: %lu B)\r\n",
           (unsigned long)before * sizeof(StackType_t),
           (unsigned long)uxTaskGetStackHighWaterMark(NULL) * sizeof(StackType_t),
           (unsigned long)SHELL_EXEC_THREAD_STACK_SIZE * sizeof(StackType_t));
    printf("heap free: %lu B before, %lu B after\r\n", (unsigned long)heap, (unsigned long)xPortGetFreeHeapSize());
}

static void cmd_list(void)
{
    for (unsigned i = 0; i < g_test_image_count; i++) {
        const test_image_t *t = &g_test_images[i];

        printf("[%2u] %-16s %7lu B  %-40s %s\r\n", i, t->name, (unsigned long)t->len,
               t->expect == JDEC_OK ? "decodes" : jdec_strerror(t->expect), t->what);
    }
}

static int cmd_jdec(int argc, char **argv)
{
    const char *sub = argc > 1 ? argv[1] : "";
    jdec_fmt_t  fmt;
    uint8_t     scale;
    bool        blocks;

    if (!strcmp(sub, "list")) {
        cmd_list();
        return 0;
    }
    if (!strcmp(sub, "info")) {
        return cmd_info(argc, argv);
    }
    if (!strcmp(sub, "mem")) {
        cmd_mem();
        return 0;
    }
    if (!strcmp(sub, "run")) {
        return cmd_run(argc, argv);
    }
    if (!strcmp(sub, "dump")) {
        return cmd_dump(argc, argv);
    }
    if (!strcmp(sub, "rx")) {
        return cmd_rx(argc, argv);
    }
    if (!strcmp(sub, "put")) {
        return cmd_put(argc, argv);
    }
    if (!strcmp(sub, "go")) {
        if (s_rx == NULL || s_rx_len == 0u || parse_opts(argc, argv, 2, &fmt, &scale, &blocks)) {
            printf("jdec go [565|565be|888] [scale 0..3] [blocks]  (after rx / put)\r\n");
            return -1;
        }
        return run_one(0, NULL, s_rx, s_rx_len, fmt, scale, blocks);
    }
    printf("jdec list | info <n|rx> | mem | run <n|all> [565|565be|888] [scale] [blocks]\r\n"
           "     dump <n|rx> [565|888] [scale] | rx <bytes> | put <base64> | go [fmt] [scale]\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_jdec, jdec, JPEG decode to RGB565 / RGB888);

int main(void)
{
    ipro_platform_init();
    printf("\r\nipro7_jpeg_decode: JPEG -> RGB565 / RGB888, CPU %lu MHz, type `jdec`\r\n",
           (unsigned long)(Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_FCLK) / 1000000u));
    vTaskStartScheduler();
    while (1) {
    }
}
