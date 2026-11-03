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
 * Host build of jdec_rgb.c + the SDK's TJpgDec (same sources, same tjpgdcnf.h).
 * Used by host/make_test_jpegs.py (expected output hashes) and
 * host/run_host_tests.py (comparison with libjpeg):
 *
 *   test_jdec_core <in.jpg> <565|565be|888> <scale 0..3> [out.raw]
 *   test_jdec_core selftest <a decodable.jpg>
 *
 * The first form prints one line: ret, picture and output size, FNV-1a of the
 * output, and whether the block-callback path produced exactly the same
 * pixels. The two paths start from buffers filled with different bytes, so
 * "same" also proves every output pixel was written (the output size is right).
 * selftest checks argument handling and edge cases of the API.
 */
#define JDEC_HOST_TEST
#include "../jdec_rgb.c"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint8_t *frame;
    uint32_t pitch, bpp;
} blocks_t;

static int put_block(void *user, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *px)
{
    blocks_t *b = user;

    for (uint32_t r = 0; r < h; r++) {
        memcpy(b->frame + (y + r) * b->pitch + x * b->bpp, px + r * w * b->bpp, (size_t)w * b->bpp);
    }
    return 1;
}

/* Exactly the file's size, so AddressSanitizer catches a read one byte past the end. */
static uint8_t *read_file(const char *path, uint32_t *len)
{
    FILE    *f = fopen(path, "rb");
    uint8_t *buf;
    long     n;

    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (n < 0) {
        fclose(f);
        return NULL;
    }
    buf = malloc(n > 0 ? (size_t)n : 1u);
    if (buf != NULL && fread(buf, 1, (size_t)n, f) != (size_t)n) {
        free(buf);
        buf = NULL;
    }
    fclose(f);
    *len = (uint32_t)n;
    return buf;
}

static int stop_cb(void *user, uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *px)
{
    (void)user, (void)x, (void)y, (void)w, (void)h, (void)px;
    return 0;
}

#define EXPECT(cond)                                                \
    do {                                                            \
        if (!(cond)) {                                              \
            printf("selftest FAIL line %d: %s\n", __LINE__, #cond); \
            fails++;                                                \
        }                                                           \
    } while (0)

static int selftest(const char *path)
{
    jdec_info_t info, zero;
    uint32_t    len, size;
    uint8_t    *jpg = read_file(path, &len), *out, *cut;
    uint16_t    w, h;
    int         fails = 0;
    uint32_t    sos = 0;

    if (jpg == NULL || jdec_probe(jpg, len, &info) != JDEC_OK) {
        printf("selftest: %s must be a decodable JPEG\n", path);
        free(jpg);
        return 2;
    }
    memset(&zero, 0, sizeof(zero));
    size = jdec_out_size(&info, JDEC_RGB888, 0);
    out  = malloc(size);

    EXPECT(jdec_probe(NULL, len, &info) == JDEC_ERR_ARG);
    EXPECT(jdec_probe(jpg, 0, &info) == JDEC_ERR_ARG);
    EXPECT(jdec_probe(jpg, 3, &info) == JDEC_ERR_NOT_JPEG);
    EXPECT(jdec_decode(jpg, len, JDEC_RGB888, 0, NULL, size, NULL) == JDEC_ERR_ARG);
    EXPECT(jdec_decode(jpg, len, (jdec_fmt_t)3, 0, out, size, NULL) == JDEC_ERR_ARG);
    EXPECT(jdec_decode(jpg, len, JDEC_RGB888, 4, out, size, NULL) == JDEC_ERR_ARG);
    EXPECT(jdec_decode(jpg, len, JDEC_RGB888, 0, out, size - 1u, NULL) == JDEC_ERR_BUF_SMALL);
    EXPECT(jdec_decode(jpg, len, JDEC_RGB888, 0, out, size, NULL) == JDEC_OK);
    EXPECT(jdec_decode_blocks(jpg, len, JDEC_RGB565, 0, NULL, NULL, NULL) == JDEC_ERR_ARG);
    EXPECT(jdec_decode_blocks(jpg, len, JDEC_RGB565, 0, stop_cb, NULL, NULL) == JDEC_ERR_ABORTED);
    jdec_out_dims(&zero, 0, &w, &h);
    EXPECT(w == 0 && h == 0);
    jdec_out_dims(&info, 4, &w, &h);
    EXPECT(w == 0 && h == 0);
    EXPECT(jdec_out_size(&zero, JDEC_RGB888, 0) == 0u);
    EXPECT(jdec_out_size(NULL, JDEC_RGB888, 0) == 0u);
    zero.samp = JDEC_SAMP_444;
    zero.width = zero.height = 65535u;
    EXPECT(jdec_out_size(&zero, JDEC_RGB888, 0) == 0u); /* 65535 x 65535 x 3 > 4 GB */

    /* A file that ends right after an SOS marker with an empty length field. */
    for (uint32_t i = 2; i + 1u < len; i++) {
        if (jpg[i] == 0xFF && jpg[i + 1] == 0xDA) {
            sos = i;
            break;
        }
    }
    EXPECT(sos != 0u);
    cut = malloc(sos + 4u);
    memcpy(cut, jpg, sos);
    memcpy(cut + sos, "\xFF\xDA\x00\x02", 4);
    EXPECT(jdec_probe(cut, sos + 4u, &info) == JDEC_ERR_NOT_JPEG);
    memcpy(cut + sos, "\xFF\xDA\x00\x03", 4);
    EXPECT(jdec_probe(cut, sos + 4u, &info) == JDEC_ERR_TRUNCATED);
    for (uint32_t n = 0; n < len; n++) { /* every prefix: an error, never a crash */
        EXPECT(jdec_probe(jpg, n, &info) != JDEC_OK);
    }
    free(cut);
    free(out);
    free(jpg);
    printf("selftest %s\n", fails ? "FAILED" : "ok");
    return fails ? 1 : 0;
}

int main(int argc, char **argv)
{
    static const char *samp[] = { "444", "422", "420", "gray" };
    jdec_info_t        info;
    jdec_fmt_t         fmt;
    uint32_t           len, size, h = 2166136261u;
    uint8_t           *jpg, *a, *b;
    uint8_t            scale;
    uint16_t           ow, oh;
    blocks_t           bl;
    int                ret, ret2;

    if (argc == 3 && !strcmp(argv[1], "selftest")) {
        return selftest(argv[2]);
    }
    if (argc < 4) {
        fprintf(stderr, "usage: %s <in.jpg> <565|565be|888> <scale> [out.raw] | selftest <a.jpg>\n", argv[0]);
        return 2;
    }
    fmt   = !strcmp(argv[2], "888") ? JDEC_RGB888 : (!strcmp(argv[2], "565be") ? JDEC_RGB565_BE : JDEC_RGB565);
    scale = (uint8_t)atoi(argv[3]);
    jpg   = read_file(argv[1], &len);
    if (jpg == NULL) {
        fprintf(stderr, "cannot read %s\n", argv[1]);
        return 2;
    }
    ret = jdec_probe(jpg, len, &info);
    size = ret == JDEC_OK ? jdec_out_size(&info, fmt, scale) : 0u;
    if (size == 0u) {
        printf("ret=%d err=\"%s\"\n", ret != JDEC_OK ? ret : JDEC_ERR_NOMEM, jdec_strerror(ret != JDEC_OK ? ret : JDEC_ERR_NOMEM));
        free(jpg);
        return 0;
    }
    jdec_out_dims(&info, scale, &ow, &oh);
    a = malloc(size);
    b = malloc(size);
    memset(a, 0x5A, size);
    memset(b, 0xA5, size);
    ret     = jdec_decode(jpg, len, fmt, scale, a, size, &info);
    bl.frame = b;
    bl.bpp   = fmt == JDEC_RGB888 ? 3u : 2u;
    bl.pitch = ow * bl.bpp;
    ret2     = jdec_decode_blocks(jpg, len, fmt, scale, put_block, &bl, NULL);
    for (uint32_t i = 0; i < size; i++) {
        h = (h ^ a[i]) * 16777619u;
    }
    printf("ret=%d err=\"%s\" probe=0 w=%u h=%u samp=%s restart=%u header=%u app=%u std_huff=%u out=%ux%u"
           " fnv=%08x blocks=%s pool=%zu\n",
           ret, jdec_strerror(ret), info.width, info.height, samp[info.samp], info.restart_interval,
           info.header_len, info.app_len, info.std_huffman, ow, oh, h,
           ret != JDEC_OK ? (ret2 == ret ? "n/a" : "DIFFER") : (ret2 == ret && memcmp(a, b, size) == 0 ? "same" : "DIFFER"),
           g_jdec_pool_used);
    if (argc > 4 && ret == JDEC_OK) {
        FILE *f = fopen(argv[4], "wb");

        fwrite(a, 1, size, f);
        fclose(f);
    }
    free(a);
    free(b);
    free(jpg);
    return 0;
}
