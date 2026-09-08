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
 * @file test_framework.c
 * @brief Additional test framework utilities
 */

#include <stdio.h>
#include "unity.h"

/**
 * @brief Custom test assertion with logging
 */
void test_assert_with_log(bool condition, const char *msg, uint32_t line)
{
    if (!condition) {
        printf("Test failed at line %lu: %s\n", (unsigned long)line, msg);
    }
    unity_assert(condition, line, msg);
}

/**
 * @brief Performance measurement helper
 */
uint32_t test_get_timestamp_ms(void)
{
    /* Implementation depends on platform timer */
    return 0; /* Placeholder */
}

/**
 * @brief Memory leak detection helper
 */
void test_check_memory_leaks(void)
{
    /* Implementation depends on heap tracking */
    printf("Memory leak check: Not implemented\n");
}
