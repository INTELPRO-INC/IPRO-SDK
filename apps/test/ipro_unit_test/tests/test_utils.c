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
 * @file test_utils.c
 * @brief Unit tests for utility components
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"

/* Example utility functions to test */
static int util_add(int a, int b)
{
    return a + b;
}

static size_t util_strlen_safe(const char *str)
{
    return str ? strlen(str) : 0;
}

/* Test cases */
TEST_CASE(test_addition)
{
    TEST_ASSERT_EQUAL_INT(5, util_add(2, 3));
    TEST_ASSERT_EQUAL_INT(0, util_add(0, 0));
    TEST_ASSERT_EQUAL_INT(-1, util_add(1, -2));
}

TEST_CASE(test_strlen_safe_normal)
{
    TEST_ASSERT_EQUAL_UINT(5, util_strlen_safe("hello"));
    TEST_ASSERT_EQUAL_UINT(0, util_strlen_safe(""));
}

TEST_CASE(test_strlen_safe_null)
{
    TEST_ASSERT_EQUAL_UINT(0, util_strlen_safe(NULL));
}

TEST_CASE(test_string_comparison)
{
    const char *str1 = "test";
    const char *str2 = "test";
    const char *str3 = "fail";
    
    TEST_ASSERT_EQUAL_STRING(str1, str2);
    TEST_ASSERT(strcmp(str1, str3) != 0);
}

TEST_CASE(test_memory_operations)
{
    uint8_t buf1[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t buf2[4] = {0x01, 0x02, 0x03, 0x04};
    
    TEST_ASSERT_EQUAL_MEMORY(buf1, buf2, 4);
}

TEST_CASE(test_null_pointer_check)
{
    void *ptr = NULL;
    TEST_ASSERT_NULL(ptr);
    
    int value = 42;
    ptr = &value;
    TEST_ASSERT_NOT_NULL(ptr);
}

/* Test suite runner */
void test_utils_run(void)
{
    printf("Running utility component tests\n");
    
    RUN_TEST(test_addition);
    RUN_TEST(test_strlen_safe_normal);
    RUN_TEST(test_strlen_safe_null);
    RUN_TEST(test_string_comparison);
    RUN_TEST(test_memory_operations);
    RUN_TEST(test_null_pointer_check);
}
