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
 * KW307 24 GHz presence-sensor UART protocol - portable codec, no HW deps.
 *
 * Reference: KW307 SDK User Guide v2.5.1 (2026-08-06) sections 1-2.
 * Everything on the wire is big-endian. CRC-16/CMS (poly 0x8005, seed 0xFFFF,
 * non-reflected) covers CMD + LEN_H + LEN_L + DATA; the SOF bytes are not
 * covered. Verified against the four complete frames printed in guide 1.8.
 */
#ifndef __KW307_PROTO_H__
#define __KW307_PROTO_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KW307_SOF0            0xAA
#define KW307_SOF_WRITE       0x55
#define KW307_SOF_READ        0x56

#define KW307_CMD_POWER_MODE  0x10   /* 1 byte: 1 = normal, 0 = power down   */
#define KW307_CMD_OUTPUT_MODE 0x11   /* 1 byte: output frame selection       */
#define KW307_CMD_GAIN        0x12   /* 1 byte: index 0..8 = -3..+21 dB      */
#define KW307_CMD_PARAM       0x90   /* [PID][BE u16]                        */
#define KW307_CMD_ACK         0xAC
#define KW307_CMD_OUTPUT      0xF0   /* periodic 27-byte detection frame     */

/* CMD_PARAM PIDs from guide 1.8.1. Wire units are a tenth of the API unit:
 * distance in mm (= cm x10), angle in deci-degrees. */
#define KW307_PID_RANGE_MIN   0x0F
#define KW307_PID_RANGE_MAX   0x10
#define KW307_PID_FOV         0x11

#define KW307_ACK_OK          0x00
#define KW307_ACK_BAD_VALUE   0x01
#define KW307_ACK_BAD_CMD     0x02
#define KW307_ACK_BAD_CRC     0x03
#define KW307_ACK_BAD_LEN     0x04

/* Guide 1.3: the module sleeps between frames. At least five 0x00 bytes must
 * precede every command, with no gap before the frame - so they are emitted
 * from the same buffer, never as a second write. */
#define KW307_WAKE_BYTES      5

#define KW307_HDR_LEN         5      /* SOF0 SOF1 CMD LEN_H LEN_L            */
#define KW307_MIN_FRAME_LEN   7      /* header + CRC16                       */
#define KW307_OUTPUT_LEN      27
#define KW307_MAX_DATA_LEN    64
#define KW307_MAX_FRAME_LEN   (KW307_MIN_FRAME_LEN + KW307_MAX_DATA_LEN)
#define KW307_MAX_TX_LEN      (KW307_WAKE_BYTES + KW307_MAX_FRAME_LEN)

/* Tracker "State" - test for Valid, never for state != 0 (guide 2.1). */
#define KW307_STATE_SIMPLE_VALID      1
#define KW307_STATE_ADV_MOVING_VALID  3
#define KW307_STATE_ADV_STILL_VALID   1

/* MEASURED, and it contradicts the vendor documentation.
 *
 * SDK User Guide 2.1 calls this field "Distance_mm ... (2000mm = 200cm)", but a
 * two-point test on 2026-08-31 (targets at a tape-measured 1.0 m and 3.0 m)
 * showed the module reporting exactly twice the true range. Physically that is
 * the round-trip path length: range = c*tau/2, and the /2 is missing. So the
 * wire value is kept raw as `distance_raw` and the correction is applied in one
 * named place, where it is visible and can be re-checked - never folded silently
 * into the decoder, because a Keywave firmware update could remove it.
 *
 * Set the divisor to 1 to see the wire value unmodified. */
#define KW307_DIST_DIVISOR_DEFAULT  2

extern uint16_t kw307_dist_divisor;

typedef struct {
    uint16_t distance_raw;  /* wire value, uncorrected - see the note above */
    int16_t  angle_tenth;   /* 0.1 degree units, signed. Scale NOT yet verified */
    uint16_t magnitude;
    uint8_t  state;
    uint8_t  counter;       /* internal age; cnt_max defaults to 32 moving / 64 still */
} kw307_tracker_t;

/* True range in mm, i.e. the wire value with the correction applied. Numerically
 * this is also tenths of a centimetre, which is how the shell prints it. */
uint16_t kw307_distance_mm(const kw307_tracker_t *t);

typedef struct {
    uint16_t        index;      /* frame counter, for missed-frame detection */
    uint8_t         human_flag;
    kw307_tracker_t moving[2];
    kw307_tracker_t stationary[1];
} kw307_frame_t;

typedef struct {
    bool     is_read;           /* SOF[1] was 0x56 */
    uint8_t  cmd;
    uint16_t len;
    const uint8_t *data;        /* points into the parser buffer */
} kw307_pkt_t;

/* Streaming framer. Resynchronises on garbage and counts what it discards, so
 * a bad wire shows up as a number instead of as silence. */
typedef struct {
    uint8_t  buf[KW307_MAX_FRAME_LEN * 2];
    uint16_t len;
    uint8_t  payload[KW307_MAX_DATA_LEN];  /* decoded frame body, owned here */
    uint32_t dropped_bytes;
    uint32_t crc_errors;
} kw307_parser_t;

uint16_t kw307_crc16(const uint8_t *buf, size_t len);

/* Build one frame, wake bytes included, into out[] (>= KW307_MAX_TX_LEN).
 * Returns the byte count, or 0 if data_len exceeds KW307_MAX_DATA_LEN. */
size_t kw307_build_write(uint8_t *out, uint8_t cmd, const uint8_t *data, size_t data_len);
size_t kw307_build_read(uint8_t *out, uint8_t cmd);
size_t kw307_build_param(uint8_t *out, uint8_t pid, uint16_t value);

void kw307_parser_reset(kw307_parser_t *p);

/* Push bytes in, take frames out. Call repeatedly with len = 0 to drain the
 * frames still buffered from an earlier feed; returns false when empty. */
bool kw307_parser_feed(kw307_parser_t *p, const uint8_t *in, size_t len, kw307_pkt_t *out);

/* Decode a 27-byte CMD 0xF0 payload. Returns false on a wrong length. */
bool kw307_parse_output(const uint8_t *data, uint16_t len, kw307_frame_t *out);

bool kw307_tracker_valid(const kw307_tracker_t *t, bool moving, bool advanced);
const char *kw307_ack_status_str(uint8_t status);

#endif /* __KW307_PROTO_H__ */
