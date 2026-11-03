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
 * @file jdec_rgb.h
 * @brief JPEG -> RGB565 / RGB888 on IPRO7 with a software decoder (TJpgDec, tjpgd/).
 *
 * The IPRO7 MJPEG hardware block is used for encoding only; IPRO7 has no
 * supported hardware JPEG decoder, so pictures are decoded on the CPU by
 * TJpgDec R0.03 (tjpgd/, the same files as components/3rdparty/tjpgdec).
 * This module wraps it:
 *
 *   jdec_probe(jpg, len, &info);                         can it be decoded, and how big is it?
 *   jdec_out_size(&info, JDEC_RGB565, 0);                bytes needed for the whole picture
 *   jdec_decode(jpg, len, JDEC_RGB565, 0, dst, size, &info);      whole picture into dst
 *   jdec_decode_blocks(jpg, len, JDEC_RGB565_BE, 0, cb, user, &info);  one MCU block at a time
 *
 * Pixels come out in true R, G, B order whatever the decoder was built for
 * (tjpgdcnf.h JD_FORMAT 0 / 1 / 2): the decoder itself writes B, G, R, so its
 * RGB565 build is really BGR565. tjpgd/tjpgdcnf.h selects JD_FORMAT 0 (8 bits
 * per channel); with JD_FORMAT 1 RGB888 output is expanded from 16 bits.
 *
 * Accepted input: baseline (SOF0) Huffman JPEG, grey or YCbCr 4:4:4 / 4:2:2 /
 * 4:2:0, 8-bit quantisation tables, any size. EXIF / ICC / other APPn
 * segments and restart markers are fine; camera MJPEG frames without a DHT
 * segment get the standard Huffman tables. Rejected: progressive, other SOF
 * types, 4:1:1 / 4:4:0, CMYK and RGB-coded files, truncated files.
 */
#ifndef JDEC_RGB_H
#define JDEC_RGB_H

#include <stdint.h>

enum {
    JDEC_OK              = 0,
    JDEC_ERR_ARG         = -1,  /* NULL pointer, zero length, bad format or scale */
    JDEC_ERR_NOT_JPEG    = -2,  /* no SOI, or a broken marker structure */
    JDEC_ERR_TRUNCATED   = -3,  /* a segment runs past the end, or no EOI */
    JDEC_ERR_PROGRESSIVE = -4,  /* SOF2 */
    JDEC_ERR_SOF         = -5,  /* extended / lossless / arithmetic / hierarchical / 12-bit */
    JDEC_ERR_SAMPLING    = -6,  /* not 4:4:4, 4:2:2, 4:2:0 or grey */
    JDEC_ERR_COMPONENTS  = -7,  /* CMYK (4), 2 components, or RGB-coded */
    JDEC_ERR_SIZE        = -8,  /* width or height is zero */
    JDEC_ERR_TABLES      = -9,  /* 16-bit DQT, missing DQT, unusual table selection */
    JDEC_ERR_NOMEM       = -10,
    JDEC_ERR_BUF_SMALL   = -11, /* dst smaller than jdec_out_size() */
    JDEC_ERR_DATA        = -12, /* corrupt entropy-coded data */
    JDEC_ERR_ABORTED     = -13, /* the block callback returned 0 */
};

typedef enum {
    JDEC_SAMP_444 = 0,
    JDEC_SAMP_422,
    JDEC_SAMP_420,
    JDEC_SAMP_GRAY,
} jdec_samp_t;

typedef enum {
    JDEC_RGB565 = 0, /* uint16_t per pixel, R in bits 15..11, CPU byte order (little-endian) */
    JDEC_RGB565_BE,  /* the same value high byte first, as SPI LCD controllers expect */
    JDEC_RGB888,     /* 3 bytes per pixel: R, G, B */
} jdec_fmt_t;

typedef struct {
    uint16_t width, height;    /* picture size from the frame header */
    uint8_t  samp;             /* jdec_samp_t */
    uint8_t  std_huffman;      /* 1: no DHT in the file, standard tables supplied */
    uint16_t restart_interval; /* MCUs between RSTn markers, 0 = none */
    uint32_t header_len;       /* offset of the entropy-coded data */
    uint32_t app_len;          /* bytes of APPn / COM segments (EXIF, ICC, ...) */
} jdec_info_t;

/*
 * Receives one decoded block: w x h pixels in the requested format, rows
 * packed (w pixels per row), to be placed at (x, y) in the output picture
 * (whose size jdec_out_dims() gives; a callback writing into its own buffer
 * should still check x + w and y + h against it). pixels is valid only until
 * the callback returns. Return 1 to continue, 0 to stop decoding
 * (jdec_decode_blocks() then returns JDEC_ERR_ABORTED).
 */
typedef int (*jdec_block_cb)(void *user, uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                             const uint8_t *pixels);

int         jdec_probe(const uint8_t *jpg, uint32_t len, jdec_info_t *info);
/* Output size at scale 0..3 (1/1 .. 1/8); 0 x 0 if info is not from a successful probe. */
void        jdec_out_dims(const jdec_info_t *info, uint8_t scale, uint16_t *w, uint16_t *h);
/* Bytes for the whole picture; 0 if it cannot be decoded or does not fit in 32 bits. */
uint32_t    jdec_out_size(const jdec_info_t *info, jdec_fmt_t fmt, uint8_t scale);
int         jdec_decode(const uint8_t *jpg, uint32_t len, jdec_fmt_t fmt, uint8_t scale, uint8_t *dst,
                        uint32_t dst_size, jdec_info_t *info);
int         jdec_decode_blocks(const uint8_t *jpg, uint32_t len, jdec_fmt_t fmt, uint8_t scale,
                               jdec_block_cb cb, void *user, jdec_info_t *info);
const char *jdec_strerror(int err);

#endif /* JDEC_RGB_H */
