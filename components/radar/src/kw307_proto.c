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

#include "kw307_proto.h"

#include <string.h>

uint16_t kw307_crc16(const uint8_t *buf, size_t len)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)buf[i] << 8;
        for (int b = 0; b < 8; b++) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x8005) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

static size_t build(uint8_t *out, uint8_t sof1, uint8_t cmd,
                    const uint8_t *data, size_t data_len)
{
    if (data_len > KW307_MAX_DATA_LEN) {
        return 0;
    }

    /* The wake bytes share this buffer so the caller cannot accidentally split
     * them from the frame across two writes - guide 1.3 forbids the gap. */
    memset(out, 0x00, KW307_WAKE_BYTES);
    uint8_t *f = out + KW307_WAKE_BYTES;

    f[0] = KW307_SOF0;
    f[1] = sof1;
    f[2] = cmd;
    f[3] = (uint8_t)(data_len >> 8);
    f[4] = (uint8_t)(data_len & 0xFF);
    if (data_len) {
        memcpy(f + KW307_HDR_LEN, data, data_len);
    }

    uint16_t crc = kw307_crc16(f + 2, data_len + 3);
    f[KW307_HDR_LEN + data_len]     = (uint8_t)(crc >> 8);
    f[KW307_HDR_LEN + data_len + 1] = (uint8_t)(crc & 0xFF);

    return KW307_WAKE_BYTES + KW307_MIN_FRAME_LEN + data_len;
}

size_t kw307_build_write(uint8_t *out, uint8_t cmd, const uint8_t *data, size_t data_len)
{
    return build(out, KW307_SOF_WRITE, cmd, data, data_len);
}

size_t kw307_build_read(uint8_t *out, uint8_t cmd)
{
    return build(out, KW307_SOF_READ, cmd, NULL, 0);
}

size_t kw307_build_param(uint8_t *out, uint8_t pid, uint16_t value)
{
    const uint8_t data[3] = { pid, (uint8_t)(value >> 8), (uint8_t)(value & 0xFF) };
    return build(out, KW307_SOF_WRITE, KW307_CMD_PARAM, data, sizeof(data));
}

void kw307_parser_reset(kw307_parser_t *p)
{
    memset(p, 0, sizeof(*p));
}

static void discard(kw307_parser_t *p, uint16_t n)
{
    if (n > p->len) {
        n = p->len;
    }
    p->dropped_bytes += n;
    p->len = (uint16_t)(p->len - n);
    memmove(p->buf, p->buf + n, p->len);
}

/* Index of the first AA 55 / AA 56 pair, or -1 when none is present. */
static int find_sof(const kw307_parser_t *p)
{
    for (uint16_t i = 0; i + 1 < p->len; i++) {
        if (p->buf[i] == KW307_SOF0 &&
            (p->buf[i + 1] == KW307_SOF_WRITE || p->buf[i + 1] == KW307_SOF_READ)) {
            return (int)i;
        }
    }
    return -1;
}

bool kw307_parser_feed(kw307_parser_t *p, const uint8_t *in, size_t len, kw307_pkt_t *out)
{
    if (len) {
        /* Overflow means we are already lost; drop the oldest half rather than
         * stall forever, and let dropped_bytes record it. */
        if (p->len + len > sizeof(p->buf)) {
            discard(p, (uint16_t)(p->len + len - sizeof(p->buf)));
        }
        if (len > sizeof(p->buf)) {
            p->dropped_bytes += len - sizeof(p->buf);
            in += len - sizeof(p->buf);
            len = sizeof(p->buf);
        }
        memcpy(p->buf + p->len, in, len);
        p->len = (uint16_t)(p->len + len);
    }

    for (;;) {
        int start = find_sof(p);
        if (start < 0) {
            /* Keep a trailing 0xAA: its partner may still be in flight. */
            uint16_t keep = (p->len && p->buf[p->len - 1] == KW307_SOF0) ? 1 : 0;
            discard(p, (uint16_t)(p->len - keep));
            return false;
        }
        if (start > 0) {
            discard(p, (uint16_t)start);
        }
        if (p->len < KW307_MIN_FRAME_LEN) {
            return false;
        }

        uint16_t data_len = (uint16_t)((p->buf[3] << 8) | p->buf[4]);
        if (data_len > KW307_MAX_DATA_LEN) {
            discard(p, 1);              /* bogus length: this was not a frame */
            continue;
        }

        uint16_t total = (uint16_t)(KW307_MIN_FRAME_LEN + data_len);
        if (p->len < total) {
            return false;               /* wait for the rest */
        }

        uint16_t got = (uint16_t)((p->buf[total - 2] << 8) | p->buf[total - 1]);
        if (got != kw307_crc16(p->buf + 2, (size_t)data_len + 3)) {
            p->crc_errors++;
            discard(p, 1);
            continue;
        }

        /* Copy the body out before consuming, so the caller's pointer stays
         * valid across the next feed. */
        if (data_len) {
            memcpy(p->payload, p->buf + KW307_HDR_LEN, data_len);
        }
        out->is_read = (p->buf[1] == KW307_SOF_READ);
        out->cmd     = p->buf[2];
        out->len     = data_len;
        out->data    = p->payload;

        p->len = (uint16_t)(p->len - total);
        memmove(p->buf, p->buf + total, p->len);
        return true;
    }
}

static uint16_t be16(const uint8_t *b)
{
    return (uint16_t)((b[0] << 8) | b[1]);
}

static void parse_tracker(const uint8_t *b, kw307_tracker_t *t)
{
    t->distance_raw = be16(b);
    t->angle_tenth = (int16_t)be16(b + 2);
    t->magnitude   = be16(b + 4);
    t->state       = b[6];
    t->counter     = b[7];
}

bool kw307_parse_output(const uint8_t *data, uint16_t len, kw307_frame_t *out)
{
    if (len != KW307_OUTPUT_LEN) {
        return false;
    }
    out->index      = be16(data);
    out->human_flag = data[2];
    parse_tracker(data + 3,  &out->moving[0]);
    parse_tracker(data + 11, &out->moving[1]);
    parse_tracker(data + 19, &out->stationary[0]);
    return true;
}

uint16_t kw307_dist_divisor = KW307_DIST_DIVISOR_DEFAULT;

uint16_t kw307_distance_mm(const kw307_tracker_t *t)
{
    uint16_t d = kw307_dist_divisor ? kw307_dist_divisor : 1;
    return (uint16_t)(t->distance_raw / d);
}

bool kw307_tracker_valid(const kw307_tracker_t *t, bool moving, bool advanced)
{
    if (!advanced) {
        return t->state == KW307_STATE_SIMPLE_VALID;
    }
    return t->state == (moving ? KW307_STATE_ADV_MOVING_VALID : KW307_STATE_ADV_STILL_VALID);
}

const char *kw307_ack_status_str(uint8_t status)
{
    switch (status) {
    case KW307_ACK_OK:        return "OK";
    case KW307_ACK_BAD_VALUE: return "invalid value";
    case KW307_ACK_BAD_CMD:   return "unknown CMD";
    case KW307_ACK_BAD_CRC:   return "CRC error";
    case KW307_ACK_BAD_LEN:   return "invalid length";
    default:                  return "unknown status";
    }
}
