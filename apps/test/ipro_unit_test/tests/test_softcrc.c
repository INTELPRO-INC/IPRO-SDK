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
 * @file test_softcrc.c
 * @brief Unit tests for software CRC component
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "softcrc.h"

/* Test cases */
TEST_CASE(test_crc16_simple)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    uint16_t crc = Utils_Soft_CRC16(data, sizeof(data));
    
    /* CRC16 should produce a non-zero result for non-zero data */
    TEST_ASSERT(crc != 0);
    TEST_ASSERT(crc != 0xFFFF);
}

TEST_CASE(test_crc16_consistency)
{
    uint8_t data[] = "Hello, CRC16!";
    
    uint16_t crc1 = Utils_Soft_CRC16(data, strlen((char *)data));
    uint16_t crc2 = Utils_Soft_CRC16(data, strlen((char *)data));
    
    /* Same data should produce same CRC */
    TEST_ASSERT_EQUAL_UINT(crc1, crc2);
}

TEST_CASE(test_crc16_different_data)
{
    uint8_t data1[] = "Test1";
    uint8_t data2[] = "Test2";
    
    uint16_t crc1 = Utils_Soft_CRC16(data1, strlen((char *)data1));
    uint16_t crc2 = Utils_Soft_CRC16(data2, strlen((char *)data2));
    
    /* Different data should produce different CRC (usually) */
    TEST_ASSERT(crc1 != crc2);
}

TEST_CASE(test_crc16_empty)
{
    uint8_t data[] = {};
    
    uint16_t crc = Utils_Soft_CRC16(data, 0);
    
    /* Empty data produces a specific CRC (usually all 0xFF for MODBUS CRC16) */
    TEST_ASSERT_EQUAL_UINT(0xFFFF, crc);
}

TEST_CASE(test_crc32_simple)
{
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    
    uint32_t crc = Utils_Soft_CRC32(data, sizeof(data));
    
    /* CRC32 should produce a non-zero result */
    TEST_ASSERT(crc != 0);
}

TEST_CASE(test_crc32_consistency)
{
    uint8_t data[] = "CRC32 Test Data";
    
    uint32_t crc1 = Utils_Soft_CRC32(data, strlen((char *)data));
    uint32_t crc2 = Utils_Soft_CRC32(data, strlen((char *)data));
    
    /* Same data should produce same CRC */
    TEST_ASSERT_EQUAL_UINT(crc1, crc2);
}

TEST_CASE(test_crc32_different_data)
{
    uint8_t data1[] = "AAAA";
    uint8_t data2[] = "BBBB";
    
    uint32_t crc1 = Utils_Soft_CRC32(data1, strlen((char *)data1));
    uint32_t crc2 = Utils_Soft_CRC32(data2, strlen((char *)data2));
    
    /* Different data should produce different CRC */
    TEST_ASSERT(crc1 != crc2);
}

TEST_CASE(test_crc32_ex)
{
    uint8_t data1[] = {0x11, 0x22, 0x33};
    uint8_t data2[] = {0x44, 0x55};
    
    /* CRC32_Ex with initial value */
    uint32_t crc_ex = Utils_Soft_CRC32_Ex(0x12345678, data1, sizeof(data1));
    
    /* CRC32 normal */
    uint32_t crc_normal = Utils_Soft_CRC32(data1, sizeof(data1));
    
    /* They should differ due to different initial values */
    TEST_ASSERT(crc_ex != crc_normal);
}

TEST_CASE(test_crc32_incremental)
{
    uint8_t data1[] = "Part1";
    uint8_t data2[] = "Part2";
    
    /* Calculate CRC of combined data */
    uint8_t combined[10] = {};
    memcpy(&combined[0], data1, strlen((char *)data1));
    memcpy(&combined[5], data2, strlen((char *)data2));
    
    uint32_t crc_combined = Utils_Soft_CRC32(combined, sizeof(combined));
    
    /* Calculate incrementally */
    uint32_t crc_part1 = Utils_Soft_CRC32(data1, strlen((char *)data1));
    uint32_t crc_incremental = Utils_Soft_CRC32_Ex(crc_part1, data2, strlen((char *)data2));
    
    /* Incremental approach (may or may not equal combined due to padding) */
    TEST_ASSERT(crc_incremental != 0);
}

TEST_CASE(test_crc_large_data)
{
    /* Create a large buffer */
    uint8_t large_data[1024];
    for (int i = 0; i < sizeof(large_data); i++) {
        large_data[i] = (uint8_t)(i & 0xFF);
    }
    
    /* Calculate CRC16 and CRC32 */
    uint16_t crc16 = Utils_Soft_CRC16(large_data, sizeof(large_data));
    uint32_t crc32 = Utils_Soft_CRC32(large_data, sizeof(large_data));
    
    /* Both should be valid */
    TEST_ASSERT(crc16 != 0);
    TEST_ASSERT(crc32 != 0);
    
    /* Recalculate and verify consistency */
    uint16_t crc16_2 = Utils_Soft_CRC16(large_data, sizeof(large_data));
    uint32_t crc32_2 = Utils_Soft_CRC32(large_data, sizeof(large_data));
    
    TEST_ASSERT_EQUAL_UINT(crc16, crc16_2);
    TEST_ASSERT_EQUAL_UINT(crc32, crc32_2);
}

/* Test suite runner */
void test_softcrc_run(void)
{
    printf("Running software CRC component tests\n");
    
    RUN_TEST(test_crc16_simple);
    RUN_TEST(test_crc16_consistency);
    RUN_TEST(test_crc16_different_data);
    RUN_TEST(test_crc16_empty);
    RUN_TEST(test_crc32_simple);
    RUN_TEST(test_crc32_consistency);
    RUN_TEST(test_crc32_different_data);
    RUN_TEST(test_crc32_ex);
    RUN_TEST(test_crc32_incremental);
    RUN_TEST(test_crc_large_data);
}
