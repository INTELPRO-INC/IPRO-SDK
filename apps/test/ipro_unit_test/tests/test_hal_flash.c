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
 * @file test_hal_flash.c
 * @brief Unit tests for HAL Flash functionality
 *
 * SAFETY: Uses test sector at offset 0x3F0000 (last 4KB of 4MB flash).
 * Always erases the test sector after each test to clean up.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_flash.h"

/* Test sector: last 4KB sector of 4MB flash (safe area) */
#define TEST_FLASH_ADDR   0x3F0000
#define TEST_FLASH_SIZE   4096

static bool hw_available = false;

TEST_CASE(test_flash_jedec_id)
{
    flash_init();

    uint32_t jedec_id = flash_get_jedecid();
    hw_available = (jedec_id != 0);

    if (!hw_available) {
        TEST_IGNORE_MESSAGE("Flash hardware not available (JEDEC ID = 0)");
        return;
    }

    printf("  Flash JEDEC ID: 0x%06X\n", (unsigned int)jedec_id);
    TEST_ASSERT(jedec_id != 0);
}

TEST_CASE(test_flash_size)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Flash hardware not available"); return; }

    uint32_t size = flash_get_size();
    printf("  Flash size: %u bytes\n", (unsigned int)size);
    TEST_ASSERT(size > 0);
}

TEST_CASE(test_flash_erase_write_read)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Flash hardware not available"); return; }

    uint8_t write_buf[256];
    uint8_t read_buf[256];
    int ret;

    /* Fill write buffer with pattern */
    for (int i = 0; i < 256; i++) {
        write_buf[i] = (uint8_t)(i & 0xFF);
    }

    /* Erase the test sector */
    ret = flash_erase(TEST_FLASH_ADDR, TEST_FLASH_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Verify erased area reads 0xFF */
    memset(read_buf, 0, sizeof(read_buf));
    ret = flash_read(TEST_FLASH_ADDR, read_buf, sizeof(read_buf));
    TEST_ASSERT_EQUAL_INT(0, ret);
    for (int i = 0; i < 256; i++) {
        TEST_ASSERT_EQUAL_INT(0xFF, (int)read_buf[i]);
    }

    /* Write pattern */
    ret = flash_write(TEST_FLASH_ADDR, write_buf, sizeof(write_buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Read back and verify */
    memset(read_buf, 0, sizeof(read_buf));
    ret = flash_read(TEST_FLASH_ADDR, read_buf, sizeof(read_buf));
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_MEMORY(write_buf, read_buf, sizeof(write_buf));

    /* Cleanup: erase test sector */
    flash_erase(TEST_FLASH_ADDR, TEST_FLASH_SIZE);
}

TEST_CASE(test_flash_write_pattern_verify)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("Flash hardware not available"); return; }

    uint8_t write_buf[128];
    uint8_t read_buf[128];
    int ret;

    /* Use a different pattern: 0xA5 alternating with 0x5A */
    for (int i = 0; i < 128; i++) {
        write_buf[i] = (i % 2 == 0) ? 0xA5 : 0x5A;
    }

    /* Erase */
    ret = flash_erase(TEST_FLASH_ADDR, TEST_FLASH_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Write at a small offset within the test sector */
    ret = flash_write(TEST_FLASH_ADDR + 256, write_buf, sizeof(write_buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Read back */
    memset(read_buf, 0, sizeof(read_buf));
    ret = flash_read(TEST_FLASH_ADDR + 256, read_buf, sizeof(read_buf));
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Verify pattern */
    for (int i = 0; i < 128; i++) {
        uint8_t expected = (i % 2 == 0) ? 0xA5 : 0x5A;
        TEST_ASSERT_EQUAL_INT((int)expected, (int)read_buf[i]);
    }

    /* Cleanup: erase test sector */
    flash_erase(TEST_FLASH_ADDR, TEST_FLASH_SIZE);
}

/* Test suite runner */
void test_hal_flash_run(void)
{
    printf("Running HAL Flash tests\n");
    printf("Test sector: 0x%06X (last 4KB of 4MB flash)\n", TEST_FLASH_ADDR);

    RUN_TEST(test_flash_jedec_id);
    RUN_TEST(test_flash_size);
    RUN_TEST(test_flash_erase_write_read);
    RUN_TEST(test_flash_write_pattern_verify);

    /* Final cleanup */
    flash_erase(TEST_FLASH_ADDR, TEST_FLASH_SIZE);
}
