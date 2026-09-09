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
 * @file test_ringbuf.c
 * @brief Unit tests for platform ring buffer utility
 *
 * Note: Shell and platform_utils both define Ring_Buffer_Init with different
 * return types (void vs int). With --allow-multiple-definition, the shell
 * version (void) wins. Don't check Ring_Buffer_Init return value.
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "ring_buffer.h"
#include "audio_pcm_ring.h"

static void dummy_lock(void) {}
static void dummy_unlock(void) {}

TEST_CASE(test_ringbuf_basic_rw)
{
    uint8_t buf[16];
    Ring_Buffer_Type rb;

    Ring_Buffer_Init(&rb, buf, sizeof(buf), dummy_lock, dummy_unlock);
    TEST_ASSERT_EQUAL_UINT(0, Ring_Buffer_Get_Length(&rb));
    TEST_ASSERT_EQUAL_INT(RING_BUFFER_EMPTY, Ring_Buffer_Get_Status(&rb));

    /* Write bytes */
    uint8_t w[5] = {1,2,3,4,5};
    uint32_t written = Ring_Buffer_Write(&rb, w, sizeof(w));
    TEST_ASSERT_EQUAL_UINT(5, written);
    TEST_ASSERT_EQUAL_UINT(5, Ring_Buffer_Get_Length(&rb));
    TEST_ASSERT_EQUAL_INT(RING_BUFFER_PARTIAL, Ring_Buffer_Get_Status(&rb));

    /* Read back */
    uint8_t r[5] = {0};
    uint32_t read = Ring_Buffer_Read(&rb, r, sizeof(r));
    TEST_ASSERT_EQUAL_UINT(5, read);
    TEST_ASSERT_EQUAL_MEMORY(w, r, sizeof(w));
    TEST_ASSERT_EQUAL_UINT(0, Ring_Buffer_Get_Length(&rb));
    TEST_ASSERT_EQUAL_INT(RING_BUFFER_EMPTY, Ring_Buffer_Get_Status(&rb));
}

TEST_CASE(test_ringbuf_wrap_and_peek)
{
    uint8_t buf[8];
    Ring_Buffer_Type rb;
    Ring_Buffer_Init(&rb, buf, sizeof(buf), dummy_lock, dummy_unlock);

    /* Fill almost full */
    uint8_t w1[6] = {10,11,12,13,14,15};
    TEST_ASSERT_EQUAL_UINT(6, Ring_Buffer_Write(&rb, w1, sizeof(w1)));

    /* Read 4 to move readIndex forward */
    uint8_t r1[4] = {0};
    TEST_ASSERT_EQUAL_UINT(4, Ring_Buffer_Read(&rb, r1, sizeof(r1)));
    TEST_ASSERT_EQUAL_UINT(10, r1[0]);

    /* Write more to wrap around */
    uint8_t w2[4] = {21,22,23,24};
    TEST_ASSERT_EQUAL_UINT(4, Ring_Buffer_Write(&rb, w2, sizeof(w2)));

    /* Peek 2 — should see first unread: 14, 15 */
    uint8_t pk[2] = {0};
    TEST_ASSERT_EQUAL_UINT(2, Ring_Buffer_Peek(&rb, pk, sizeof(pk)));
    TEST_ASSERT_EQUAL_UINT(14, pk[0]);
    TEST_ASSERT_EQUAL_UINT(15, pk[1]);

    /* Read remaining 6 bytes: {14, 15, 21, 22, 23, 24} */
    uint8_t r2[6] = {0};
    TEST_ASSERT_EQUAL_UINT(6, Ring_Buffer_Read(&rb, r2, sizeof(r2)));
    TEST_ASSERT_EQUAL_UINT(14, r2[0]);
    TEST_ASSERT_EQUAL_UINT(15, r2[1]);
    TEST_ASSERT_EQUAL_UINT(21, r2[2]);
    TEST_ASSERT_EQUAL_UINT(22, r2[3]);
    TEST_ASSERT_EQUAL_UINT(23, r2[4]);
    TEST_ASSERT_EQUAL_UINT(24, r2[5]);
}

TEST_CASE(test_ringbuf_force_write_overflow)
{
    uint8_t buf[4];
    Ring_Buffer_Type rb;
    Ring_Buffer_Init(&rb, buf, sizeof(buf), dummy_lock, dummy_unlock);

    uint8_t w[6] = {1,2,3,4,5,6};
    /* Force write should overwrite oldest data */
    uint32_t written = Ring_Buffer_Write_Force(&rb, w, sizeof(w));
    TEST_ASSERT_EQUAL_UINT(sizeof(buf), written); /* only size can be held */
    TEST_ASSERT_EQUAL_INT(RING_BUFFER_FULL, Ring_Buffer_Get_Status(&rb));

    uint8_t r[4] = {0};
    Ring_Buffer_Read(&rb, r, sizeof(r));
    /* Expect last 4 elements kept: 3,4,5,6 */
    TEST_ASSERT_EQUAL_UINT(3, r[0]);
    TEST_ASSERT_EQUAL_UINT(4, r[1]);
    TEST_ASSERT_EQUAL_UINT(5, r[2]);
    TEST_ASSERT_EQUAL_UINT(6, r[3]);
}

TEST_CASE(test_audio_pcm_ring_exact_frame_and_wrap)
{
    uint8_t storage[12];
    audio_pcm_ring_t ring;
    const uint8_t first[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    const uint8_t second[8] = { 8, 9, 10, 11, 12, 13, 14, 15 };
    uint8_t out[8] = { 0 };

    TEST_ASSERT_EQUAL_INT(0, audio_pcm_ring_init(&ring, storage,
                                                 sizeof(storage)));
    TEST_ASSERT_TRUE(audio_pcm_ring_write(&ring, first, sizeof(first)));
    TEST_ASSERT_TRUE(audio_pcm_ring_read(&ring, out, 4));
    TEST_ASSERT_EQUAL_MEMORY(first, out, 4);

    /* This write crosses the physical end of storage. */
    TEST_ASSERT_TRUE(audio_pcm_ring_write(&ring, second, sizeof(second)));
    TEST_ASSERT_EQUAL_UINT(12, audio_pcm_ring_available(&ring));

    TEST_ASSERT_TRUE(audio_pcm_ring_read(&ring, out, sizeof(out)));
    TEST_ASSERT_EQUAL_MEMORY(&first[4], out, 4);
    TEST_ASSERT_EQUAL_MEMORY(second, &out[4], 4);
    TEST_ASSERT_TRUE(audio_pcm_ring_read(&ring, out, 4));
    TEST_ASSERT_EQUAL_MEMORY(&second[4], out, 4);
    TEST_ASSERT_EQUAL_UINT(0, audio_pcm_ring_available(&ring));
}

TEST_CASE(test_audio_pcm_ring_rejects_partial_frame_without_corruption)
{
    uint8_t storage[8];
    audio_pcm_ring_t ring;
    const uint8_t first[6] = { 1, 2, 3, 4, 5, 6 };
    const uint8_t rejected[4] = { 9, 9, 9, 9 };
    uint8_t out[6] = { 0 };

    TEST_ASSERT_EQUAL_INT(0, audio_pcm_ring_init(&ring, storage,
                                                 sizeof(storage)));
    TEST_ASSERT_TRUE(audio_pcm_ring_write(&ring, first, sizeof(first)));
    TEST_ASSERT_FALSE(audio_pcm_ring_write(&ring, rejected,
                                           sizeof(rejected)));
    TEST_ASSERT_EQUAL_UINT(6, audio_pcm_ring_available(&ring));
    TEST_ASSERT_FALSE(audio_pcm_ring_read(&ring, out, 7));
    TEST_ASSERT_TRUE(audio_pcm_ring_read(&ring, out, sizeof(out)));
    TEST_ASSERT_EQUAL_MEMORY(first, out, sizeof(first));
}

TEST_CASE(test_audio_pcm_ring_reset_and_invalid_arguments)
{
    uint8_t storage[4];
    uint8_t byte = 0x5a;
    audio_pcm_ring_t ring;

    TEST_ASSERT_TRUE(audio_pcm_ring_init(NULL, storage,
                                        sizeof(storage)) != 0);
    TEST_ASSERT_TRUE(audio_pcm_ring_init(&ring, NULL,
                                        sizeof(storage)) != 0);
    TEST_ASSERT_TRUE(audio_pcm_ring_init(&ring, storage, 0) != 0);
    TEST_ASSERT_EQUAL_INT(0, audio_pcm_ring_init(&ring, storage,
                                                 sizeof(storage)));
    TEST_ASSERT_TRUE(audio_pcm_ring_write(&ring, &byte, 1));
    audio_pcm_ring_reset(&ring);
    TEST_ASSERT_EQUAL_UINT(0, audio_pcm_ring_available(&ring));
    TEST_ASSERT_EQUAL_UINT(sizeof(storage), audio_pcm_ring_free(&ring));
    TEST_ASSERT_FALSE(audio_pcm_ring_write(&ring, NULL, 1));
    TEST_ASSERT_FALSE(audio_pcm_ring_read(&ring, NULL, 1));
}

void test_ringbuf_run(void)
{
    printf("Running ring buffer tests\n");
    RUN_TEST(test_ringbuf_basic_rw);
    RUN_TEST(test_ringbuf_wrap_and_peek);
    RUN_TEST(test_ringbuf_force_write_overflow);
    RUN_TEST(test_audio_pcm_ring_exact_frame_and_wrap);
    RUN_TEST(test_audio_pcm_ring_rejects_partial_frame_without_corruption);
    RUN_TEST(test_audio_pcm_ring_reset_and_invalid_arguments);
}
