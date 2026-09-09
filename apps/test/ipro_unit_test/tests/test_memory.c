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
 * @file test_memory.c
 * @brief Unit tests for memory management
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "FreeRTOS.h"

/* Test cases */
TEST_CASE(test_malloc_free)
{
    void *ptr = pvPortMalloc(100);
    TEST_ASSERT_NOT_NULL(ptr);
    
    memset(ptr, 0xAA, 100);
    
    vPortFree(ptr);
    TEST_PASS();
}

TEST_CASE(test_malloc_alignment)
{
    void *ptr = pvPortMalloc(64);
    TEST_ASSERT_NOT_NULL(ptr);
    
    /* Check alignment (should be at least 4-byte aligned) */
    uintptr_t addr = (uintptr_t)ptr;
    TEST_ASSERT_EQUAL_UINT(0, addr % 4);
    
    vPortFree(ptr);
}

TEST_CASE(test_malloc_sizes)
{
    /* Test various allocation sizes */
    size_t sizes[] = {1, 4, 16, 64, 256, 1024, 4096};
    void *ptrs[7];
    
    for (int i = 0; i < 7; i++) {
        ptrs[i] = pvPortMalloc(sizes[i]);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }
    
    /* Free all */
    for (int i = 0; i < 7; i++) {
        vPortFree(ptrs[i]);
    }
    
    TEST_PASS();
}

TEST_CASE(test_heap_info)
{
    size_t free_before = xPortGetFreeHeapSize();
    
    void *ptr = pvPortMalloc(1000);
    TEST_ASSERT_NOT_NULL(ptr);
    
    size_t free_after = xPortGetFreeHeapSize();
    TEST_ASSERT(free_after < free_before);
    
    vPortFree(ptr);
    
    size_t free_final = xPortGetFreeHeapSize();
    /* Allow some slack for heap fragmentation */
    TEST_ASSERT(free_final >= (free_before - 100));
}

TEST_CASE(test_memset_memcpy)
{
    uint8_t src[32];
    uint8_t dst[32];
    
    /* Fill source with pattern */
    memset(src, 0x55, sizeof(src));
    
    /* Copy to destination */
    memcpy(dst, src, sizeof(src));
    
    /* Verify */
    TEST_ASSERT_EQUAL_MEMORY(src, dst, sizeof(src));
    
    /* Test memset */
    memset(dst, 0xAA, sizeof(dst));
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_UINT(0xAA, dst[i]);
    }
}

/* Test suite runner */
void test_memory_run(void)
{
    printf("Running memory management tests\n");
    printf("Current free heap: %u bytes\n", xPortGetFreeHeapSize());
    
    RUN_TEST(test_malloc_free);
    RUN_TEST(test_malloc_alignment);
    RUN_TEST(test_malloc_sizes);
    RUN_TEST(test_heap_info);
    RUN_TEST(test_memset_memcpy);
    
    printf("Final free heap: %u bytes\n", xPortGetFreeHeapSize());
}
