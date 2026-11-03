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
 * @file jdec_rgb.c
 * @brief JPEG -> RGB565 / RGB888 with TJpgDec, see jdec_rgb.h.
 *
 * jdec_probe() walks the markers once and turns everything TJpgDec cannot
 * handle into a specific error before any memory is allocated. Decoding feeds
 * TJpgDec from memory; when the file has no DHT (camera MJPEG frames) the
 * standard tables are inserted right after SOI, so tables defined later in
 * the file still win. The output callback converts each block from the
 * decoder's native pixel format (B, G, R order) to the requested one.
 *
 * Build with -DJDEC_HOST_TEST to run the same code on a PC (host/test_jdec_core.c).
 */
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "tjpgd.h"
#include "jdec_rgb.h"

#ifdef JDEC_HOST_TEST
#include <stdlib.h>
#define JDEC_MALLOC(n) malloc(n)
#define JDEC_FREE(p)   free(p)
#else
#include <FreeRTOS.h>
#define JDEC_MALLOC(n) pvPortMalloc(n)
#define JDEC_FREE(p)   vPortFree(p)
#endif

/*
 * TJpgDec work area. Typical use is about 3.5 KB (9.7 KB with JD_FASTDECODE 2);
 * a valid file at the limits (4 quantisation tables, the standard Huffman tables
 * plus a full set of its own) needs about 5.4 KB (16 KB). A file that keeps
 * redefining tables can still run out: the decoder then reports JDEC_ERR_NOMEM.
 */
#ifndef JDEC_POOL_SIZE
#if JD_FASTDECODE == 2
#define JDEC_POOL_SIZE 16384u
#else
#define JDEC_POOL_SIZE 8192u
#endif
#endif

#ifdef JDEC_HOST_TEST
size_t g_jdec_pool_used; /* reported by host/test_jdec_core.c */
#endif

/* ------------------------------------------------------------------ */
/* Header check                                                       */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t     nc;
    uint8_t     id[3], hs[3], vs[3], tq[3], sel[3];
    uint8_t     qt;       /* bit n: 8-bit DQT n loaded */
    uint8_t     ht[2];    /* [class] bit n: DHT id n loaded */
    uint32_t    eoi_end;  /* offset just past EOI */
    jdec_info_t info;
} jdec_parse_t;

static uint16_t be16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static int parse_dqt(const uint8_t *s, uint32_t n, jdec_parse_t *p)
{
    for (uint32_t k = 0; k < n; k += 65u) {
        if (s[k] >> 4) {
            return JDEC_ERR_TABLES; /* 16-bit table */
        }
        if (n - k < 65u || (s[k] & 0x0F) > 3) {
            return JDEC_ERR_NOT_JPEG;
        }
        p->qt |= (uint8_t)(1u << (s[k] & 0x0F));
    }
    return JDEC_OK;
}

/* The decoder keeps DC/AC tables 0 and 1 only, and DC symbols up to 11. */
static int parse_dht(const uint8_t *s, uint32_t n, jdec_parse_t *p)
{
    uint32_t k = 0;

    while (k < n) {
        uint32_t cnt = 0;

        if (n - k < 17u) {
            return JDEC_ERR_NOT_JPEG;
        }
        if (s[k] & 0xEE) {
            return JDEC_ERR_TABLES;
        }
        for (uint32_t i = 1; i <= 16u; i++) {
            cnt += s[k + i];
        }
        if (n - k < 17u + cnt) {
            return JDEC_ERR_NOT_JPEG;
        }
        if ((s[k] >> 4) == 0u) { /* DC table */
            for (uint32_t i = 0; i < cnt; i++) {
                if (s[k + 17 + i] > 11u) {
                    return JDEC_ERR_TABLES;
                }
            }
        }
        p->ht[s[k] >> 4] |= (uint8_t)(1u << (s[k] & 0x0F));
        k += 17u + cnt;
    }
    return JDEC_OK;
}

static int parse_sof(const uint8_t *s, uint32_t n, jdec_parse_t *p)
{
    if (p->nc != 0u) {
        return JDEC_ERR_NOT_JPEG; /* second frame header */
    }
    if (n < 6u) {
        return JDEC_ERR_NOT_JPEG;
    }
    if (s[0] != 8u) {
        return JDEC_ERR_SOF; /* 12-bit samples */
    }
    p->info.height = be16(s + 1);
    p->info.width  = be16(s + 3);
    p->nc          = s[5];
    if (p->nc != 1u && p->nc != 3u) {
        return JDEC_ERR_COMPONENTS;
    }
    if (n < 6u + 3u * p->nc) {
        return JDEC_ERR_NOT_JPEG;
    }
    for (uint8_t c = 0; c < p->nc; c++) {
        p->id[c] = s[6 + 3 * c];
        p->hs[c] = s[7 + 3 * c] >> 4;
        p->vs[c] = s[7 + 3 * c] & 0x0F;
        p->tq[c] = s[8 + 3 * c];
    }
    return JDEC_OK;
}

/* n >= 1 here: parse() rejects empty segments. */
static int parse_sos(const uint8_t *s, uint32_t n, jdec_parse_t *p)
{
    if (p->nc == 0u) {
        return JDEC_ERR_NOT_JPEG; /* SOS before SOF */
    }
    if (s[0] != p->nc) {
        return JDEC_ERR_SOF; /* one component per scan */
    }
    if (n < 4u + 2u * p->nc) {
        return JDEC_ERR_NOT_JPEG;
    }
    for (uint8_t k = 0; k < p->nc; k++) {
        uint8_t c = 0;

        while (c < p->nc && p->id[c] != s[1 + 2 * k]) {
            c++;
        }
        if (c == p->nc) {
            return JDEC_ERR_NOT_JPEG;
        }
        p->sel[c] = s[2 + 2 * k];
    }
    return JDEC_OK;
}

/* What TJpgDec R0.03 accepts, checked up front so the reason can be reported. */
static int check_frame(jdec_parse_t *p)
{
    jdec_info_t *in = &p->info;
    uint8_t      need_dc = 1u, need_ac = 1u;

    if (in->width == 0u || in->height == 0u) {
        return JDEC_ERR_SIZE;
    }
    if (p->nc == 3u) {
        if (p->id[0] == 'R' && p->id[1] == 'G' && p->id[2] == 'B') {
            return JDEC_ERR_COMPONENTS;
        }
        if (p->hs[1] != 1u || p->vs[1] != 1u || p->hs[2] != 1u || p->vs[2] != 1u) {
            return JDEC_ERR_SAMPLING;
        }
        if (p->hs[0] == 1u && p->vs[0] == 1u) {
            in->samp = JDEC_SAMP_444;
        } else if (p->hs[0] == 2u && p->vs[0] == 1u) {
            in->samp = JDEC_SAMP_422;
        } else if (p->hs[0] == 2u && p->vs[0] == 2u) {
            in->samp = JDEC_SAMP_420;
        } else {
            return JDEC_ERR_SAMPLING;
        }
        /* The decoder takes Huffman tables 0 for Y and 1 for Cb/Cr, whatever SOS says. */
        if (p->sel[0] != 0x00 || p->sel[1] != 0x11 || p->sel[2] != 0x11) {
            return JDEC_ERR_TABLES;
        }
        need_dc = need_ac = 3u;
    } else {
        if (p->hs[0] != 1u || p->vs[0] != 1u) {
            return JDEC_ERR_SAMPLING; /* grey must be 1x1 */
        }
        if (p->sel[0] != 0x00) {
            return JDEC_ERR_TABLES;
        }
        in->samp = JDEC_SAMP_GRAY;
    }
    for (uint8_t c = 0; c < p->nc; c++) {
        if (p->tq[c] > 3u || !(p->qt & (1u << p->tq[c]))) {
            return JDEC_ERR_TABLES;
        }
    }
    in->std_huffman = (p->ht[0] & need_dc) != need_dc || (p->ht[1] & need_ac) != need_ac;
    return JDEC_OK;
}

/* Entropy-coded data up to EOI: stuffed 0xFF00, RSTn and fill bytes are skipped. */
static int find_eoi(const uint8_t *j, uint32_t len, jdec_parse_t *p)
{
    uint32_t i = p->info.header_len;

    for (;;) {
        const uint8_t *f = i < len ? memchr(j + i, 0xFF, len - i) : NULL;
        uint8_t        b;

        if (f == NULL) {
            return JDEC_ERR_TRUNCATED;
        }
        i = (uint32_t)(f - j);
        if (i + 1u >= len) {
            return JDEC_ERR_TRUNCATED;
        }
        b = j[i + 1];
        if (b == 0x00 || (b >= 0xD0 && b <= 0xD7)) {
            i += 2u;
        } else if (b == 0xFF) {
            i += 1u;
        } else if (b == 0xD9) {
            p->eoi_end = i + 2u;
            return JDEC_OK;
        } else {
            return JDEC_ERR_SOF; /* another scan follows: multi-scan file */
        }
    }
}

static int parse(const uint8_t *j, uint32_t len, jdec_parse_t *p)
{
    uint32_t i = 2;

    memset(p, 0, sizeof(*p));
    if (j == NULL || len == 0u) {
        return JDEC_ERR_ARG;
    }
    if (len < 4u || j[0] != 0xFF || j[1] != 0xD8) {
        return JDEC_ERR_NOT_JPEG;
    }
    /* The decoder reads every header item as marker + length, so fill bytes,
     * markers without a length field and empty segments are rejected here
     * rather than misread there. */
    for (;;) {
        uint8_t        m;
        uint32_t       n;
        const uint8_t *s;
        int            ret = JDEC_OK;

        if (i + 4u > len) {
            return JDEC_ERR_TRUNCATED;
        }
        if (j[i] != 0xFF) {
            return JDEC_ERR_NOT_JPEG;
        }
        m = j[i + 1];
        if (m == 0xD9) {
            return JDEC_ERR_TRUNCATED; /* EOI before any scan */
        }
        if (m == 0xFF || m == 0xD8 || m == 0x01 || m == 0x00 || (m >= 0xD0 && m <= 0xD7)) {
            return JDEC_ERR_NOT_JPEG;
        }
        n = be16(j + i + 2);
        if (n <= 2u) {
            return JDEC_ERR_NOT_JPEG;
        }
        if (i + 2u + n > len) {
            return JDEC_ERR_TRUNCATED;
        }
        s = j + i + 4;
        n -= 2u;
        if ((m == 0xC0 || m == 0xC4 || m == 0xDB || m == 0xDD || m == 0xDA) && n > JD_SZBUF) {
            return JDEC_ERR_TABLES; /* the decoder loads these segments whole into JD_SZBUF */
        }

        switch (m) {
        case 0xDB:
            ret = parse_dqt(s, n, p);
            break;
        case 0xC4:
            ret = parse_dht(s, n, p);
            break;
        case 0xDD:
            ret = n >= 2u ? JDEC_OK : JDEC_ERR_NOT_JPEG;
            p->info.restart_interval = n >= 2u ? be16(s) : 0u;
            break;
        case 0xC0:
            ret = parse_sof(s, n, p);
            break;
        case 0xC2:
            return JDEC_ERR_PROGRESSIVE;
        case 0xC1: case 0xC3: case 0xC5: case 0xC6: case 0xC7:
        case 0xC9: case 0xCA: case 0xCB: case 0xCD: case 0xCE: case 0xCF:
            return JDEC_ERR_SOF;
        case 0xDA:
            ret = parse_sos(s, n, p);
            if (ret != JDEC_OK) {
                return ret;
            }
            p->info.header_len = i + 4u + n;
            ret = check_frame(p);
            return ret != JDEC_OK ? ret : find_eoi(j, len, p);
        default:
            if ((m >= 0xE0 && m <= 0xEF) || m == 0xFE) {
                p->info.app_len += n + 4u; /* APPn, COM: skipped by the decoder */
            }
            break;
        }
        if (ret != JDEC_OK) {
            return ret;
        }
        i += 4u + n;
    }
}

int jdec_probe(const uint8_t *jpg, uint32_t len, jdec_info_t *info)
{
    jdec_parse_t p;
    int          ret = parse(jpg, len, &p);

    if (info != NULL) {
        *info = p.info;
    }
    return ret;
}

/* Output size: TJpgDec scales every MCU on its own and drops what rounds to zero. */
static uint16_t scaled(uint16_t n, uint16_t mcu, uint8_t s)
{
    uint32_t last;

    if (n == 0u) {
        return 0;
    }
    last = (uint32_t)(n - 1u) / mcu * mcu;
    return (uint16_t)((last >> s) + ((n - last) >> s));
}

/* 0 x 0 for a bad scale or an info that did not come from a successful probe. */
void jdec_out_dims(const jdec_info_t *info, uint8_t scale, uint16_t *w, uint16_t *h)
{
    uint16_t mx, my;

    *w = *h = 0;
    if (info == NULL || scale > 3u || info->samp > JDEC_SAMP_GRAY) {
        return;
    }
    mx = info->samp == JDEC_SAMP_422 || info->samp == JDEC_SAMP_420 ? 16u : 8u;
    my = info->samp == JDEC_SAMP_420 ? 16u : 8u;
    *w = scaled(info->width, mx, scale);
    *h = scaled(info->height, my, scale);
}

static uint32_t bytes_per_pixel(jdec_fmt_t fmt)
{
    return fmt == JDEC_RGB888 ? 3u : 2u;
}

/* 0 when the picture cannot be decoded or the size does not fit in 32 bits. */
uint32_t jdec_out_size(const jdec_info_t *info, jdec_fmt_t fmt, uint8_t scale)
{
    uint16_t w, h;
    uint64_t n;

    jdec_out_dims(info, scale, &w, &h);
    n = (uint64_t)w * h * bytes_per_pixel(fmt);
    return n > 0xFFFFFFFFu ? 0u : (uint32_t)n;
}

/* ------------------------------------------------------------------ */
/* Decoding                                                           */
/* ------------------------------------------------------------------ */

/* DHT with the four standard tables (ITU-T T.81 Annex K.3): DC0, AC0, DC1, AC1. */
static const uint8_t k_std_dht[] = {
    0xFF, 0xC4, 0x01, 0xA2,
    0x00, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7D,
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xA1, 0x08, 0x23, 0x42, 0xB1, 0xC1, 0x15, 0x52, 0xD1, 0xF0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0A, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
    0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3, 0xC4, 0xC5,
    0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xE1, 0xE2,
    0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA,
    0x01, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
    0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xA1, 0xB1, 0xC1, 0x09, 0x23, 0x33, 0x52, 0xF0,
    0x15, 0x62, 0x72, 0xD1, 0x0A, 0x16, 0x24, 0x34, 0xE1, 0x25, 0xF1, 0x17, 0x18, 0x19, 0x1A, 0x26,
    0x27, 0x28, 0x29, 0x2A, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4A, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6A, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0xA2, 0xA3, 0xA4, 0xA5,
    0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xC2, 0xC3,
    0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
    0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8,
    0xF9, 0xFA,
};

typedef struct {
    jdec_fmt_t    fmt;
    uint8_t      *dst;     /* whole-picture mode */
    uint32_t      pitch;
    uint16_t      out_w, out_h;
    jdec_block_cb cb;      /* block mode */
    void         *user;
} jdec_out_t;

/* Lives in the heap block in front of the decoder's work area. */
typedef struct {
    const uint8_t *src[3]; /* the stream as up to three pieces read back to back */
    uint32_t       srclen[3];
    uint8_t        nsrc, cur;
    uint32_t       pos;
    jdec_out_t     out;
    uint8_t        blk[16 * 16 * 3];
} jdec_ctx_t;

static size_t in_func(JDEC *jd, uint8_t *buf, size_t n)
{
    jdec_ctx_t *c    = jd->device;
    size_t      done = 0;

    while (done < n && c->cur < c->nsrc) {
        size_t k = c->srclen[c->cur] - c->pos;

        k = k < n - done ? k : n - done;
        if (buf != NULL) {
            memcpy(buf + done, c->src[c->cur] + c->pos, k);
        }
        done += k;
        c->pos += (uint32_t)k;
        if (c->pos == c->srclen[c->cur]) {
            c->cur++;
            c->pos = 0;
        }
    }
    return done;
}

static inline void emit(uint8_t **d, jdec_fmt_t fmt, uint16_t v565, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t *p = *d;

    if (fmt == JDEC_RGB888) {
        p[0] = r;
        p[1] = g;
        p[2] = b;
        *d   = p + 3;
    } else if (fmt == JDEC_RGB565) {
        p[0] = (uint8_t)v565;
        p[1] = (uint8_t)(v565 >> 8);
        *d   = p + 2;
    } else {
        p[0] = (uint8_t)(v565 >> 8);
        p[1] = (uint8_t)v565;
        *d   = p + 2;
    }
}

/* n pixels of the decoder's native format -> fmt. */
static void convert(const void *bitmap, uint32_t n, jdec_fmt_t fmt, uint8_t *d)
{
#if JD_FORMAT == 1
    const uint16_t *s = bitmap; /* BGR565: B in bits 15..11, R in bits 4..0 */

    for (uint32_t i = 0; i < n; i++) {
        uint16_t v  = s[i];
        uint8_t  r5 = v & 0x1Fu, g6 = (v >> 5) & 0x3Fu, b5 = v >> 11;

        emit(&d, fmt, (uint16_t)((r5 << 11) | (g6 << 5) | b5), (uint8_t)((r5 << 3) | (r5 >> 2)),
             (uint8_t)((g6 << 2) | (g6 >> 4)), (uint8_t)((b5 << 3) | (b5 >> 2)));
    }
#elif JD_FORMAT == 0
    const uint8_t *s = bitmap; /* B, G, R */

    for (uint32_t i = 0; i < n; i++, s += 3) {
        emit(&d, fmt, (uint16_t)(((s[2] & 0xF8) << 8) | ((s[1] & 0xFC) << 3) | (s[0] >> 3)), s[2], s[1], s[0]);
    }
#else
    const uint8_t *s = bitmap; /* grey */

    for (uint32_t i = 0; i < n; i++) {
        emit(&d, fmt, (uint16_t)(((s[i] & 0xF8) << 8) | ((s[i] & 0xFC) << 3) | (s[i] >> 3)), s[i], s[i], s[i]);
    }
#endif
}

#define NATIVE_BPP (JD_FORMAT == 0 ? 3u : (JD_FORMAT == 1 ? 2u : 1u))

static int out_func(JDEC *jd, void *bitmap, JRECT *r)
{
    jdec_ctx_t *c = jd->device;
    uint32_t    w = (uint32_t)r->right - r->left + 1u;
    uint32_t    h = (uint32_t)r->bottom - r->top + 1u;

    const jdec_out_t *o = &c->out;

    if (o->cb != NULL) {
        convert(bitmap, w * h, o->fmt, c->blk);
        return o->cb(o->user, r->left, r->top, (uint16_t)w, (uint16_t)h, c->blk);
    }
    if (r->right >= o->out_w || r->bottom >= o->out_h) {
        return 0; /* cannot happen with jdec_out_size(); stop rather than overrun */
    }
    for (uint32_t y = 0; y < h; y++) {
        convert((const uint8_t *)bitmap + y * w * NATIVE_BPP, w, o->fmt,
                o->dst + (r->top + y) * o->pitch + r->left * bytes_per_pixel(o->fmt));
    }
    return 1;
}

static int map_result(JRESULT r)
{
    switch (r) {
    case JDR_OK:   return JDEC_OK;
    case JDR_INTR: return JDEC_ERR_ABORTED;
    case JDR_INP:  return JDEC_ERR_TRUNCATED;
    case JDR_MEM1: return JDEC_ERR_NOMEM;
    case JDR_MEM2: return JDEC_ERR_TABLES; /* a table segment above JD_SZBUF bytes */
    case JDR_PAR:  return JDEC_ERR_ARG;
    case JDR_FMT1: return JDEC_ERR_DATA;
    default:       return JDEC_ERR_SOF;
    }
}

static bool bad_args(jdec_fmt_t fmt, uint8_t scale)
{
    return (unsigned)fmt > JDEC_RGB888 || scale > 3u || (scale != 0u && !JD_USE_SCALE);
}

static int run(const uint8_t *jpg, const jdec_parse_t *p, const jdec_out_t *out, uint8_t scale)
{
    jdec_ctx_t *c = JDEC_MALLOC(sizeof(*c) + JDEC_POOL_SIZE);
    JDEC        jd;
    JRESULT     r;

    if (c == NULL) {
        return JDEC_ERR_NOMEM;
    }
    c->out = *out;
    c->cur = 0;
    c->pos = 0;
    if (p->info.std_huffman) { /* SOI, standard DHT, rest of the file */
        c->src[0]    = jpg;
        c->srclen[0] = 2;
        c->src[1]    = k_std_dht;
        c->srclen[1] = sizeof(k_std_dht);
        c->src[2]    = jpg + 2;
        c->srclen[2] = p->eoi_end - 2u;
        c->nsrc      = 3;
    } else {
        c->src[0]    = jpg;
        c->srclen[0] = p->eoi_end;
        c->nsrc      = 1;
    }
    r = jd_prepare(&jd, in_func, (uint8_t *)(c + 1), JDEC_POOL_SIZE, c);
#ifdef JDEC_HOST_TEST
    g_jdec_pool_used = JDEC_POOL_SIZE - jd.sz_pool;
#endif
    if (r == JDR_OK) {
        r = jd_decomp(&jd, out_func, scale);
    }
    JDEC_FREE(c);
    return map_result(r);
}

int jdec_decode(const uint8_t *jpg, uint32_t len, jdec_fmt_t fmt, uint8_t scale, uint8_t *dst,
                uint32_t dst_size, jdec_info_t *info)
{
    jdec_parse_t p;
    jdec_out_t   o;
    int          ret;

    if (dst == NULL || bad_args(fmt, scale)) {
        return JDEC_ERR_ARG;
    }
    ret = parse(jpg, len, &p);
    if (info != NULL) {
        *info = p.info;
    }
    if (ret != JDEC_OK) {
        return ret;
    }
    memset(&o, 0, sizeof(o));
    o.fmt = fmt;
    o.dst = dst;
    jdec_out_dims(&p.info, scale, &o.out_w, &o.out_h);
    o.pitch = (uint32_t)o.out_w * bytes_per_pixel(fmt);
    if ((uint64_t)o.pitch * o.out_h > dst_size) {
        return JDEC_ERR_BUF_SMALL;
    }
    return run(jpg, &p, &o, scale);
}

int jdec_decode_blocks(const uint8_t *jpg, uint32_t len, jdec_fmt_t fmt, uint8_t scale, jdec_block_cb cb,
                       void *user, jdec_info_t *info)
{
    jdec_parse_t p;
    jdec_out_t   o;
    int          ret;

    if (cb == NULL || bad_args(fmt, scale)) {
        return JDEC_ERR_ARG;
    }
    ret = parse(jpg, len, &p);
    if (info != NULL) {
        *info = p.info;
    }
    if (ret != JDEC_OK) {
        return ret;
    }
    memset(&o, 0, sizeof(o));
    o.fmt  = fmt;
    o.cb   = cb;
    o.user = user;
    return run(jpg, &p, &o, scale);
}

const char *jdec_strerror(int err)
{
    switch (err) {
    case JDEC_OK:              return "ok";
    case JDEC_ERR_ARG:         return "bad argument";
    case JDEC_ERR_NOT_JPEG:    return "not a JPEG / broken marker structure";
    case JDEC_ERR_TRUNCATED:   return "truncated (segment past the end or no EOI)";
    case JDEC_ERR_PROGRESSIVE: return "progressive JPEG not supported";
    case JDEC_ERR_SOF:         return "not baseline (extended/lossless/arithmetic/12-bit/multi-scan)";
    case JDEC_ERR_SAMPLING:    return "chroma sampling not 4:4:4, 4:2:2, 4:2:0 or grey";
    case JDEC_ERR_COMPONENTS:  return "CMYK / RGB-coded / 2-component JPEG not supported";
    case JDEC_ERR_SIZE:        return "width or height is zero";
    case JDEC_ERR_TABLES:      return "quantisation / Huffman tables not supported";
    case JDEC_ERR_NOMEM:       return "out of memory";
    case JDEC_ERR_BUF_SMALL:   return "output buffer too small";
    case JDEC_ERR_DATA:        return "corrupt image data";
    case JDEC_ERR_ABORTED:     return "stopped by the block callback";
    default:                   return "unknown error";
    }
}
