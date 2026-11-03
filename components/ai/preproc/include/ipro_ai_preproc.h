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
 * @file ipro_ai_preproc.h
 * @brief Shared preprocessing: samples, audio and images -> NPU input bytes.
 *
 * What the three Teach tools did each in their own way, with one owner:
 *
 *  - quantise   float features -> the bytes an int8 input reads
 *               (q = round(x / scale) + zp, then the tensor's encoding)
 *  - voice      log-mel features (fbank.c, 16 kHz) quantised into a packed
 *               one-channel input: the speaker trunk's front end
 *  - sensor     sensor_front.h (moved here unchanged): windows of int16
 *               samples -> spectrogram or resampled wave, normalised, int8
 *  - image      crop, resize (nearest or bilinear, integer arithmetic),
 *               pixel format -> RGB/BGR/gray, packed into the NPU's pixel
 *               pitch. On the Reef the ISP does this in hardware for the
 *               camera; this path is for sources without it (a decoded
 *               JPEG, an SPI camera, a frame from the host).
 *
 * Every function has a host twin (tools/edge_ai/preproc_ref.py) and stored
 * golden vectors (components/ai/preproc/test_host); the comparison rules are
 * in docs/edge_ai/08_test_plan.md.
 *
 * Pure C: no allocation, no RTOS; callers own every buffer.
 */
#ifndef IPRO_AI_PREPROC_H
#define IPRO_AI_PREPROC_H

#include <stdint.h>

#include "ipro_ai.h"                   /* ipro_ai_enc_t, IPRO_AI_E_* */

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------------------------------------------------- quantise */

/**
 * @brief out[i] = clamp(round(x[i] / scale) + zp) in the tensor's encoding.
 *
 * Rounding is lrintf (to nearest, ties to even) of x * (1 / scale), the
 * arithmetic the voice trunk has always used. OFFSET_BINARY clamps to
 * -128..127 and stores q ^ 0x80; UNSIGNED clamps to 0..255.
 */
int ipro_ai_quantize(const float *x, uint32_t n, float scale, int32_t zp, ipro_ai_enc_t enc,
                     uint8_t *out);

/* ---------------------------------------------------------------- voice */

/**
 * @brief The speaker trunk's input: log-mel of 16 kHz PCM, quantised.
 *
 * @param fbank     an initialised FBankContext (fbank_init), opaque here
 * @param pcm       n_samples of 16 kHz mono int16
 * @param frames    rows the model takes (n_samples / 160 - 1 at most)
 * @param feat      scratch, FBANK_SEQ_LEN x FBANK_N_MELS floats
 * @param out       frames x FBANK_N_MELS bytes, row-major, packed
 * @return IPRO_AI_OK, or IPRO_AI_E_ARG when fbank produced fewer frames
 */
int ipro_ai_voice_front(const void *fbank, const int16_t *pcm, uint32_t n_samples, uint32_t frames,
                        float in_scale, int32_t in_zp, float *feat, uint8_t *out);

/* ---------------------------------------------------------------- image */

typedef enum {
    IPRO_AI_PIX_GRAY8 = 0,
    IPRO_AI_PIX_RGB565,        /* little-endian 16-bit words, R in bits 15..11 */
    IPRO_AI_PIX_RGB888,        /* bytes R, G, B */
    IPRO_AI_PIX_BGR888,        /* bytes B, G, R (TJpgDec's JD_FORMAT 0 on this SDK) */
    IPRO_AI_PIX_ARGB8888,      /* 32-bit words 0xAARRGGBB, little-endian: bytes B, G, R, A */
    IPRO_AI_PIX_YUYV,          /* Y0 U Y1 V, BT.601 full range */
} ipro_ai_pixfmt_t;

typedef struct {
    const uint8_t   *data;
    uint32_t         width, height;
    uint32_t         stride;   /* bytes per row; 0: width x bytes per pixel */
    ipro_ai_pixfmt_t format;
} ipro_ai_image_t;

typedef struct {
    uint32_t x, y, w, h;       /* inside the image */
} ipro_ai_rect_t;

typedef enum { IPRO_AI_RESIZE_NEAREST = 0, IPRO_AI_RESIZE_BILINEAR } ipro_ai_resize_t;
typedef enum { IPRO_AI_ORDER_RGB = 0, IPRO_AI_ORDER_BGR, IPRO_AI_ORDER_GRAY } ipro_ai_order_t;

typedef struct {
    uint32_t         width, height;
    uint32_t         pitch;    /* bytes per pixel in the tensor: 1 (gray), 3 or 4 */
    ipro_ai_order_t  order;
    ipro_ai_resize_t resize;
    ipro_ai_enc_t    encoding; /* OFFSET_BINARY: byte = pixel ("u8 - 128" as int8);
                                * UNSIGNED: byte = pixel as well, the NPU reads it unsigned */
} ipro_ai_image_spec_t;

/**
 * @brief Crop, resize and pack an image into an NPU input.
 *
 * Bilinear sampling uses pixel centres (src = (dst + 0.5) * scale - 0.5,
 * clamped at the edges) in Q16 coordinates and Q8 weights, so the host twin
 * reproduces it byte for byte. Gray is (77 R + 150 G + 29 B + 128) >> 8,
 * YUYV -> RGB is BT.601 full range in Q8. Padding bytes (pitch 4) are 0x80
 * for OFFSET_BINARY (zero) and 0 for UNSIGNED.
 *
 * @param crop  NULL for the whole image
 * @param out   spec->width x spec->height x spec->pitch bytes
 */
int ipro_ai_image_to_tensor(const ipro_ai_image_t *src, const ipro_ai_rect_t *crop,
                            const ipro_ai_image_spec_t *spec, uint8_t *out, uint32_t out_bytes);

#ifdef __cplusplus
}
#endif

#endif /* IPRO_AI_PREPROC_H */
