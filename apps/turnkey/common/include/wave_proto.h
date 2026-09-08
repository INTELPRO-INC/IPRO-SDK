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
 * @file wave_proto.h
 * @brief IMU event-waveform upload protocol (G-TAG -> gateway), shared by
 *        ipro7_fall_neck and ipro7_fall_gateway.
 *
 * One fall event ships one window: WAVE_CHANNELS x WAVE_SAMPLES int16 samples
 * (channel-major), split into WAVE_CHUNKS CoAP CON POSTs on /wave, each
 * carrying a 32-byte header + up to WAVE_CHUNK_BYTES of sample data. This is
 * ALSO the clinical training-data format — the gateway mirrors completed
 * windows to the backend, so the corpus accumulates in its final shape from
 * day one (simulator-generated until the IMU hardware lands).
 */

#ifndef __WAVE_PROTO_H__
#define __WAVE_PROTO_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WAVE_MAGIC        0x57F1
#define WAVE_VER          1

#define WAVE_CHANNELS     6     /* acc x/y/z (mg), gyro x/y/z */
#define WAVE_SAMPLES      128   /* per channel; @50 Hz = 2.56 s window */
#define WAVE_RATE_HZ      50
#define WAVE_BYTES        (WAVE_CHANNELS * WAVE_SAMPLES * 2)   /* 1536 */

#define WAVE_CHUNK_BYTES  512
#define WAVE_CHUNKS       ((WAVE_BYTES + WAVE_CHUNK_BYTES - 1) / WAVE_CHUNK_BYTES)

typedef struct __attribute__((packed)) {
    uint16_t magic;      /* WAVE_MAGIC */
    uint8_t  ver;        /* WAVE_VER */
    uint8_t  idx;        /* chunk index, 0..total-1 */
    uint8_t  total;      /* WAVE_CHUNKS */
    uint8_t  channels;   /* WAVE_CHANNELS */
    uint8_t  bits;       /* 16 */
    uint8_t  flags;      /* bit0: 1 = synthetic (simulator) */
    uint16_t rate_hz;    /* WAVE_RATE_HZ */
    uint16_t samples;    /* WAVE_SAMPLES */
    uint32_t event_ms;   /* device-side fall timestamp (joins to /fall ts) */
    char     id[16];     /* 16-hex device id, NOT NUL-terminated */
} wave_hdr_t;            /* 32 bytes */

#define WAVE_FLAG_SYNTHETIC 0x01

#ifdef __cplusplus
}
#endif

#endif /* __WAVE_PROTO_H__ */
