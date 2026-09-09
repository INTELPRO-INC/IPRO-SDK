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
 * Host-side tests for the KW307 codec - runs on the dev machine, no board.
 *
 *   cc -std=c11 -Wall -Wextra -Werror -o /tmp/kw307_test \
 *      test_kw307_proto_host.c kw307_proto.c && /tmp/kw307_test
 *
 * The vectors are the complete frames printed in the KW307 SDK User Guide
 * v2.5.1 section 1.8, so a pass means our framing matches the vendor's, not
 * merely that we agree with ourselves.
 */
#include <stdio.h>
#include <string.h>

#include "kw307_proto.h"

static int g_fail;
static int g_run;

#define CHECK(cond, ...) do {                                      \
    g_run++;                                                       \
    if (!(cond)) { g_fail++;                                       \
        printf("FAIL %s:%d  ", __func__, __LINE__);                \
        printf(__VA_ARGS__); printf("\n"); }                       \
} while (0)

static void check_bytes(const char *what, const uint8_t *got, size_t got_len,
                        const uint8_t *exp, size_t exp_len)
{
    g_run++;
    if (got_len == exp_len && memcmp(got, exp, exp_len) == 0) {
        return;
    }
    g_fail++;
    printf("FAIL %s\n  got:", what);
    for (size_t i = 0; i < got_len; i++) printf(" %02X", got[i]);
    printf("\n  exp:");
    for (size_t i = 0; i < exp_len; i++) printf(" %02X", exp[i]);
    printf("\n");
}

/* ---- guide 1.6 / 1.8: CRC over CMD+LEN+DATA ---------------------------- */
static void test_crc_vectors(void)
{
    const uint8_t a[] = { 0x10, 0x00, 0x01, 0x01 };
    const uint8_t b[] = { 0x12, 0x00, 0x01, 0x04 };
    const uint8_t c[] = { 0x12, 0x00, 0x00 };
    const uint8_t d[] = { 0xAC, 0x00, 0x02, 0x12, 0x00 };

    CHECK(kw307_crc16(a, sizeof(a)) == 0x4624, "power mode crc");
    CHECK(kw307_crc16(b, sizeof(b)) == 0xEE39, "gain crc");
    CHECK(kw307_crc16(c, sizeof(c)) == 0x8F6B, "read request crc");
    CHECK(kw307_crc16(d, sizeof(d)) == 0xF90B, "ack crc");
    CHECK(kw307_crc16(NULL, 0) == 0xFFFF, "empty crc is the seed");
}

/* ---- guide 1.8.1: complete frames, wake bytes included ------------------ */
static void test_build_frames(void)
{
    uint8_t out[KW307_MAX_TX_LEN];
    size_t n;

    const uint8_t gain[] = { 0x00, 0x00, 0x00, 0x00, 0x00,
                             0xAA, 0x55, 0x12, 0x00, 0x01, 0x04, 0xEE, 0x39 };
    const uint8_t v = 0x04;
    n = kw307_build_write(out, KW307_CMD_GAIN, &v, 1);
    check_bytes("set gain +9dB", out, n, gain, sizeof(gain));

    const uint8_t rd[] = { 0x00, 0x00, 0x00, 0x00, 0x00,
                           0xAA, 0x56, 0x12, 0x00, 0x00, 0x8F, 0x6B };
    n = kw307_build_read(out, KW307_CMD_GAIN);
    check_bytes("read gain request", out, n, rd, sizeof(rd));

    /* min 500 mm, max 3000 mm, FOV 900 (= 90.0 deg) */
    const uint8_t mn[] = { 0xAA, 0x55, 0x90, 0x00, 0x03, 0x0F, 0x01, 0xF4 };
    n = kw307_build_param(out, KW307_PID_RANGE_MIN, 500);
    check_bytes("range min", out + KW307_WAKE_BYTES, 8, mn, sizeof(mn));

    const uint8_t mx[] = { 0xAA, 0x55, 0x90, 0x00, 0x03, 0x10, 0x0B, 0xB8 };
    n = kw307_build_param(out, KW307_PID_RANGE_MAX, 3000);
    check_bytes("range max", out + KW307_WAKE_BYTES, 8, mx, sizeof(mx));

    const uint8_t fov[] = { 0xAA, 0x55, 0x90, 0x00, 0x03, 0x11, 0x03, 0x84 };
    n = kw307_build_param(out, KW307_PID_FOV, 900);
    check_bytes("fov 90 deg", out + KW307_WAKE_BYTES, 8, fov, sizeof(fov));

    CHECK(kw307_build_write(out, KW307_CMD_GAIN, NULL, KW307_MAX_DATA_LEN + 1) == 0,
          "oversized payload is refused");
    (void)n;
}

static size_t frame_only(uint8_t *dst, const uint8_t *tx, size_t n)
{
    memcpy(dst, tx + KW307_WAKE_BYTES, n - KW307_WAKE_BYTES);
    return n - KW307_WAKE_BYTES;
}

/* ---- the framer -------------------------------------------------------- */
static void test_parser_basic(void)
{
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN];
    const uint8_t v = 0x04;
    size_t n = frame_only(wire, tx, kw307_build_write(tx, KW307_CMD_GAIN, &v, 1));

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);

    CHECK(kw307_parser_feed(&p, wire, n, &pkt), "frame decodes");
    CHECK(pkt.cmd == KW307_CMD_GAIN && pkt.len == 1 && pkt.data[0] == 0x04, "fields");
    CHECK(!pkt.is_read, "write direction");
    CHECK(!kw307_parser_feed(&p, NULL, 0, &pkt), "no second frame");
}

static void test_parser_byte_at_a_time(void)
{
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN];
    const uint8_t v = 0x04;
    size_t n = frame_only(wire, tx, kw307_build_write(tx, KW307_CMD_GAIN, &v, 1));

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);

    int decoded = 0;
    for (size_t i = 0; i < n; i++) {
        if (kw307_parser_feed(&p, &wire[i], 1, &pkt)) decoded++;
    }
    CHECK(decoded == 1, "a byte-at-a-time frame decodes exactly once, got %d", decoded);
}

static void test_parser_resync(void)
{
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN];
    size_t n = frame_only(wire, tx, kw307_build_read(tx, KW307_CMD_GAIN));

    /* Leading junk, including a lone 0xAA that is not a real SOF. */
    uint8_t stream[KW307_MAX_FRAME_LEN + 5] = { 0x00, 0x00, 0x00, 0xFF, 0xAA };
    memcpy(stream + 5, wire, n);

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);

    CHECK(kw307_parser_feed(&p, stream, n + 5, &pkt), "resyncs past junk");
    CHECK(p.dropped_bytes == 5, "counted 5 dropped, got %u", p.dropped_bytes);
    CHECK(pkt.is_read, "read direction preserved");
}

static void test_parser_wake_bytes_are_harmless(void)
{
    uint8_t tx[KW307_MAX_TX_LEN];
    const uint8_t v = 0x04;
    size_t n = kw307_build_write(tx, KW307_CMD_GAIN, &v, 1);

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);
    CHECK(kw307_parser_feed(&p, tx, n, &pkt), "leading wake bytes do not desync");
}

static void test_parser_bad_crc(void)
{
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN * 2];
    const uint8_t v = 0x04, w = 0x05;
    size_t n1 = frame_only(wire, tx, kw307_build_write(tx, KW307_CMD_GAIN, &v, 1));
    wire[n1 - 1] ^= 0xFF;                                   /* corrupt the CRC */
    size_t n2 = frame_only(wire + n1, tx, kw307_build_write(tx, KW307_CMD_GAIN, &w, 1));

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);

    CHECK(kw307_parser_feed(&p, wire, n1 + n2, &pkt), "recovers after a bad CRC");
    CHECK(p.crc_errors == 1, "one CRC error counted, got %u", p.crc_errors);
    CHECK(pkt.data[0] == 0x05, "the good frame is the one returned");
}

static void test_parser_absurd_length(void)
{
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN * 2];
    uint8_t junk[] = { 0xAA, 0x55, 0xF0, 0xFF, 0xFF };
    memcpy(wire, junk, sizeof(junk));
    const uint8_t v = 0x04;
    size_t n = frame_only(wire + sizeof(junk), tx,
                          kw307_build_write(tx, KW307_CMD_GAIN, &v, 1));

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_parser_reset(&p);
    CHECK(kw307_parser_feed(&p, wire, sizeof(junk) + n, &pkt),
          "a bogus LEN does not stall the stream");
}

/* ---- guide 2 / 2.1: the 27-byte detection frame ------------------------- */
static void test_output_frame(void)
{
    uint8_t body[KW307_OUTPUT_LEN] = { 0 };
    body[0] = 0x12; body[1] = 0x34;                          /* frame index    */
    body[2] = 1;                                             /* human flag     */
    /* moving[0]: 2000 mm, +35.0 deg, mag 512, Valid, counter 20 */
    const uint8_t m0[8] = { 0x07, 0xD0, 0x01, 0x5E, 0x02, 0x00, 0x01, 0x14 };
    memcpy(body + 3, m0, 8);
    /* stationary[0]: 1500 mm, -35.0 deg, mag 100, Valid, counter 9 */
    const uint8_t s0[8] = { 0x05, 0xDC, 0xFE, 0xA2, 0x00, 0x64, 0x01, 0x09 };
    memcpy(body + 19, s0, 8);

    kw307_frame_t f;
    CHECK(kw307_parse_output(body, KW307_OUTPUT_LEN, &f), "27-byte frame decodes");
    CHECK(!kw307_parse_output(body, 26, &f), "26 bytes is rejected");

    CHECK(f.index == 0x1234, "index");
    CHECK(f.human_flag == 1, "human flag");
    /* The wire value is kept raw; the measured /2 correction is applied by
     * kw307_distance_mm() and nowhere else. */
    CHECK(f.moving[0].distance_raw == 2000, "raw wire distance");
    CHECK(kw307_distance_mm(&f.moving[0]) == 1000, "corrected range is half the wire value");
    kw307_dist_divisor = 1;
    CHECK(kw307_distance_mm(&f.moving[0]) == 2000, "divisor 1 returns the wire value");
    kw307_dist_divisor = KW307_DIST_DIVISOR_DEFAULT;
    CHECK(f.moving[0].angle_tenth == 350, "angle");
    CHECK(f.moving[0].magnitude == 512, "magnitude");
    CHECK(f.moving[0].counter == 20, "counter");
    CHECK(f.stationary[0].angle_tenth == -350, "negative angle is signed, got %d",
          f.stationary[0].angle_tenth);
    CHECK(f.moving[1].state == 0, "unused slot is Empty");
}

static void test_valid_semantics(void)
{
    /* Guide 2.1: test for Valid, not for state != 0. The moving slot's Valid
     * value differs between the simple and advanced reporting forms. */
    kw307_tracker_t simple = { 1000, 0, 100, 1, 5 };
    kw307_tracker_t adv    = { 1000, 0, 100, 3, 5 };
    kw307_tracker_t hidden = { 1500, 0, 200, 2, 9 };
    CHECK(kw307_distance_mm(&simple) == 500, "accessor applies the divisor");

    CHECK(kw307_tracker_valid(&simple, true, false), "simple moving Valid = 1");
    CHECK(!kw307_tracker_valid(&adv, true, false), "state 3 is not Valid in simple form");
    CHECK(kw307_tracker_valid(&adv, true, true), "advanced moving Valid = 3");
    CHECK(!kw307_tracker_valid(&simple, true, true), "advanced moving 1 is Checking");
    CHECK(kw307_tracker_valid(&simple, false, true), "stationary Valid = 1 in both forms");
    CHECK(!kw307_tracker_valid(&hidden, false, true), "Hidden stationary is not Valid");
}

static void test_output_through_the_framer(void)
{
    uint8_t body[KW307_OUTPUT_LEN];
    for (int i = 0; i < KW307_OUTPUT_LEN; i++) body[i] = (uint8_t)i;

    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN];
    size_t n = frame_only(wire, tx,
                          kw307_build_write(tx, KW307_CMD_OUTPUT, body, sizeof(body)));

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_frame_t f;
    kw307_parser_reset(&p);

    CHECK(kw307_parser_feed(&p, wire, n, &pkt), "output frame survives the framer");
    CHECK(pkt.cmd == KW307_CMD_OUTPUT && pkt.len == KW307_OUTPUT_LEN, "cmd and len");
    CHECK(kw307_parse_output(pkt.data, pkt.len, &f), "and decodes");
    CHECK(memcmp(pkt.data, body, KW307_OUTPUT_LEN) == 0, "payload is byte-exact");
}

static void test_back_to_back_frames(void)
{
    /* At 20 Hz one read often holds several frames; all of them must come out. */
    uint8_t tx[KW307_MAX_TX_LEN], wire[KW307_MAX_FRAME_LEN * 4];
    uint8_t body[KW307_OUTPUT_LEN] = { 0 };
    size_t off = 0;

    for (int i = 0; i < 3; i++) {
        body[1] = (uint8_t)i;
        size_t n = kw307_build_write(tx, KW307_CMD_OUTPUT, body, sizeof(body));
        off += frame_only(wire + off, tx, n);
    }

    kw307_parser_t p;
    kw307_pkt_t pkt;
    kw307_frame_t f;
    kw307_parser_reset(&p);

    int seen = 0;
    bool more = kw307_parser_feed(&p, wire, off, &pkt);
    while (more) {
        if (kw307_parse_output(pkt.data, pkt.len, &f) && f.index == (uint16_t)seen) seen++;
        more = kw307_parser_feed(&p, NULL, 0, &pkt);
    }
    CHECK(seen == 3, "three queued frames all drain in order, got %d", seen);
    CHECK(p.crc_errors == 0 && p.dropped_bytes == 0, "and nothing was discarded");
}

int main(void)
{
    test_crc_vectors();
    test_build_frames();
    test_parser_basic();
    test_parser_byte_at_a_time();
    test_parser_resync();
    test_parser_wake_bytes_are_harmless();
    test_parser_bad_crc();
    test_parser_absurd_length();
    test_output_frame();
    test_valid_semantics();
    test_output_through_the_framer();
    test_back_to_back_frames();

    printf("%s: %d checks, %d failed\n", g_fail ? "FAIL" : "PASS", g_run, g_fail);
    return g_fail ? 1 : 0;
}
