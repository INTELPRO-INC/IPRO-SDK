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
 * @file test_hal_dma.c
 * @brief Unit tests for HAL DMA memory-to-memory transfers
 *
 * Requires cache flush/invalidate for CPU-DMA coherency.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_dma.h"
#include "FreeRTOS.h"
#include "task.h"

/* Cache management macros */
extern void MFlushDCacheRange(unsigned long addr, unsigned long size);
extern void MInvalDCacheRange(unsigned long addr, unsigned long size);

#define TEST_DMA_ID    DMA0_ID
#define TEST_DMA_CH    DMA0_CH0_INDEX
#define TEST_BUF_SIZE  256  /* bytes */

static bool hw_available = false;
static volatile int dma_transfer_done = 0;

/* Static buffers aligned to cache line (32 bytes) to avoid heap corruption
 * from MInvalDCacheRange invalidating adjacent heap metadata */
static uint8_t __attribute__((aligned(32))) dma_src_buf[TEST_BUF_SIZE];
static uint8_t __attribute__((aligned(32))) dma_dst_buf[TEST_BUF_SIZE];

static void dma_callback(void *dev, uint32_t flag)
{
    if (flag == HAL_DMA_INT_COMPLETE)
        dma_transfer_done = 1;
}

static int dma_do_m2m_transfer(uint32_t src_addr, uint32_t dst_addr, uint32_t size)
{
    dma_transfer_done = 0;

    dma_ioctl(TEST_DMA_ID, TEST_DMA_CH, DMA_CTRL_SET_INT, NULL);

    dma_lli_ctrl_t *lli = NULL;
    int ret = dma_reload(TEST_DMA_ID, TEST_DMA_CH, src_addr, dst_addr, size, &lli);
    if (ret != 0) return ret;

    /* Flush source from CPU cache, invalidate destination */
    MFlushDCacheRange((unsigned long)src_addr, size);
    MInvalDCacheRange((unsigned long)dst_addr, size);

    dma_channel_start(TEST_DMA_ID, TEST_DMA_CH);

    /* Wait for DMA completion via callback */
    int timeout = 100000;
    while (!dma_transfer_done && timeout > 0) {
        timeout--;
    }

    /* Invalidate destination again to see DMA-written data */
    MInvalDCacheRange((unsigned long)dst_addr, size);

    return (timeout > 0) ? 0 : -1;
}

TEST_CASE(test_dma_init)
{
    dma_device_t dma_dev = {
        .id = TEST_DMA_ID,
        .ch = TEST_DMA_CH,
        .transfer_mode  = DMA_LLI_ONCE_MODE,
        .direction      = DMA_MEMORY_TO_MEMORY,
        .src_req        = DMA_REQUEST_NONE,
        .dst_req        = DMA_REQUEST_NONE,
        .src_addr_inc   = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc   = DMA_ADDR_INCREMENT_ENABLE,
        .src_burst_size = DMA_BURST_INCR4,
        .dst_burst_size = DMA_BURST_INCR4,
        .src_width      = DMA_TRANSFER_WIDTH_32BIT,
        .dst_width      = DMA_TRANSFER_WIDTH_32BIT,
        .lli_cfg        = NULL,
        .callback       = dma_callback,
    };

    dma_setConfig(&dma_dev);
    int ret = dma_init(TEST_DMA_ID, TEST_DMA_CH);
    hw_available = (ret == 0);

    if (!hw_available) {
        TEST_IGNORE_MESSAGE("DMA hardware not available");
        return;
    }

    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_dma_m2m_transfer)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("DMA hardware not available"); return; }

    for (int i = 0; i < TEST_BUF_SIZE; i++) {
        dma_src_buf[i] = (uint8_t)(i & 0xFF);
    }
    memset(dma_dst_buf, 0, TEST_BUF_SIZE);

    int ret = dma_do_m2m_transfer((uint32_t)(uintptr_t)dma_src_buf,
                                   (uint32_t)(uintptr_t)dma_dst_buf,
                                   TEST_BUF_SIZE);

    if (ret != 0) {
        TEST_FAIL("DMA transfer timed out");
        return;
    }

    TEST_ASSERT_EQUAL_MEMORY(dma_src_buf, dma_dst_buf, TEST_BUF_SIZE);
}

TEST_CASE(test_dma_m2m_pattern_verify)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("DMA hardware not available"); return; }

    uint32_t *src = (uint32_t *)dma_src_buf;
    uint32_t *dst = (uint32_t *)dma_dst_buf;

    int num_words = TEST_BUF_SIZE / sizeof(uint32_t);
    for (int i = 0; i < num_words; i++) {
        src[i] = 0xDEADBEEF ^ (uint32_t)i;
    }
    memset(dst, 0, TEST_BUF_SIZE);

    int ret = dma_do_m2m_transfer((uint32_t)(uintptr_t)src,
                                   (uint32_t)(uintptr_t)dst,
                                   TEST_BUF_SIZE);

    if (ret != 0) {
        TEST_FAIL("DMA transfer timed out");
        return;
    }

    TEST_ASSERT_EQUAL_MEMORY(src, dst, TEST_BUF_SIZE);
}

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR

/* ---------------------------------------------------------------
 * Allocator tests. Each test releases everything it acquires so
 * tests are order-independent. Uses fake peripheral request IDs
 * that don't conflict with any live peripheral. We intentionally
 * do NOT start a transfer here — this is a pure allocator test.
 * --------------------------------------------------------------- */

TEST_CASE(test_dma_alloc_request_release)
{
    dma_ch_handle_t h = dma_request_channel(DMA_REQUEST_NONE,
                                            DMA_MEMORY_TO_MEMORY,
                                            "unit_test_alloc");
    TEST_ASSERT_NOT_NULL(h);

    int ch = dma_handle_to_ch(h);
    TEST_ASSERT_TRUE(ch >= 0 && ch < DMA_MAX_INDEX);
    TEST_ASSERT_EQUAL_INT(1, dma_channel_is_allocator_owned(DMA0_ID, ch));

    dma_release_channel(h);
    TEST_ASSERT_EQUAL_INT(0, dma_channel_is_allocator_owned(DMA0_ID, ch));
}

TEST_CASE(test_dma_alloc_refcount_same_owner)
{
    dma_ch_handle_t h1 = dma_request_channel(DMA_REQUEST_DAC0,
                                             DMA_MEMORY_TO_PERIPH,
                                             "unit_test_refcount");
    TEST_ASSERT_NOT_NULL(h1);

    dma_ch_handle_t h2 = dma_request_channel(DMA_REQUEST_DAC0,
                                             DMA_MEMORY_TO_PERIPH,
                                             "unit_test_refcount");
    TEST_ASSERT_NOT_NULL(h2);
    TEST_ASSERT_EQUAL_PTR(h1, h2);   /* same handle = same slot */
    TEST_ASSERT_EQUAL_INT(dma_handle_to_ch(h1), dma_handle_to_ch(h2));

    /* First release: slot should remain in use (refcount 2 -> 1) */
    dma_release_channel(h2);
    TEST_ASSERT_EQUAL_INT(1, dma_channel_is_allocator_owned(
                                 DMA0_ID, dma_handle_to_ch(h1)));

    /* Second release: slot now free (refcount 1 -> 0) */
    dma_release_channel(h1);
    TEST_ASSERT_EQUAL_INT(0, dma_channel_is_allocator_owned(
                                 DMA0_ID, dma_handle_to_ch(h1)));
}

TEST_CASE(test_dma_alloc_conflict_different_owner)
{
    dma_ch_handle_t h1 = dma_request_channel(DMA_REQUEST_DAC0,
                                             DMA_MEMORY_TO_PERIPH,
                                             "owner_a");
    TEST_ASSERT_NOT_NULL(h1);

    /* Different owner requesting the same peripheral request line
     * must be rejected. */
    dma_ch_handle_t h2 = dma_request_channel(DMA_REQUEST_DAC0,
                                             DMA_MEMORY_TO_PERIPH,
                                             "owner_b");
    TEST_ASSERT_NULL(h2);

    dma_release_channel(h1);
}

TEST_CASE(test_dma_alloc_exhaust_pool)
{
    /* Request DMA_MAX_INDEX distinct M2M channels. M2M skips the
     * same-req refcount fast path so each call burns a fresh slot. */
    dma_ch_handle_t hs[DMA_MAX_INDEX];
    int acquired = 0;
    for (int i = 0; i < DMA_MAX_INDEX; i++) {
        hs[i] = dma_request_channel(DMA_REQUEST_NONE,
                                    DMA_MEMORY_TO_MEMORY,
                                    "unit_test_exhaust");
        if (hs[i]) acquired++;
        else       break;
    }
    TEST_ASSERT_EQUAL_INT(DMA_MAX_INDEX, acquired);

    /* Pool exhausted: one more request must fail */
    dma_ch_handle_t extra = dma_request_channel(DMA_REQUEST_NONE,
                                                DMA_MEMORY_TO_MEMORY,
                                                "unit_test_exhaust");
    TEST_ASSERT_NULL(extra);

    for (int i = 0; i < acquired; i++) dma_release_channel(hs[i]);
}

TEST_CASE(test_dma_alloc_dump_allocation)
{
    /* Smoke test: dump with an allocation in place + dump when empty. */
    dma_ch_handle_t h = dma_request_channel(DMA_REQUEST_AUD_ADC,
                                            DMA_PERIPH_TO_MEMORY,
                                            "unit_test_dump");
    TEST_ASSERT_NOT_NULL(h);

    dma_dump_allocation();   /* should print one live entry */

    dma_release_channel(h);
    dma_dump_allocation();   /* should print all free */

    TEST_PASS();
}

#endif /* CONFIG_DMA_CHANNEL_ALLOCATOR */

/* Test suite runner */
void test_hal_dma_run(void)
{
    printf("Running HAL DMA tests\n");

    RUN_TEST(test_dma_init);
    RUN_TEST(test_dma_m2m_transfer);
    RUN_TEST(test_dma_m2m_pattern_verify);

#ifdef CONFIG_DMA_CHANNEL_ALLOCATOR
    RUN_TEST(test_dma_alloc_request_release);
    RUN_TEST(test_dma_alloc_refcount_same_owner);
    RUN_TEST(test_dma_alloc_conflict_different_owner);
    RUN_TEST(test_dma_alloc_exhaust_pool);
    RUN_TEST(test_dma_alloc_dump_allocation);
#endif

    if (hw_available) {
        dma_ioctl(TEST_DMA_ID, TEST_DMA_CH, DMA_CTRL_CHANNEL_STOP, NULL);
        dma_ioctl(TEST_DMA_ID, TEST_DMA_CH, DMA_CTRL_CLEAR_INT, NULL);
        dma_close(TEST_DMA_ID, TEST_DMA_CH);
    }
}
