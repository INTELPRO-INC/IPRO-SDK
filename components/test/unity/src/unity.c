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
 * @file unity.c
 * @brief Lightweight Unity Test Framework Implementation
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"

/* Global Unity state */
Unity_t Unity = {0};

/* Static helper for printing test results */
static void print_test_result(bool passed, const char *msg, uint32_t line)
{
    if (passed) {
        printf("  [PASS] %s\n", Unity.CurrentTestName);
    } else {
        printf("  [FAIL] %s:%lu - %s\n", Unity.CurrentTestName, (unsigned long)line, msg ? msg : "");
    }
}

void unity_begin(void)
{
    Unity.NumberOfTests = 0;
    Unity.TestFailures = 0;
    Unity.TestPasses = 0;
    Unity.TestIgnores = 0;
    Unity.CurrentTestName = NULL;
    Unity.CurrentTestLineNumber = 0;
}

int unity_end(void)
{
    printf("\n-----------------------\n");
    printf("Tests: %lu  Failures: %lu  Ignored: %lu\n", 
           (unsigned long)Unity.NumberOfTests, (unsigned long)Unity.TestFailures, (unsigned long)Unity.TestIgnores);
    
    return (Unity.TestFailures == 0) ? 0 : -1;
}

void unity_run_test(void (*test_func)(void), const char *test_name, uint32_t line)
{
    Unity.NumberOfTests++;
    Unity.CurrentTestName = test_name;
    Unity.CurrentTestLineNumber = line;
    
    /* Run the test */
    test_func();
}

void unity_assert(bool condition, uint32_t line, const char *msg)
{
    if (condition) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
        print_test_result(false, msg, line);
    }
}

void unity_assert_equal_int(int expected, int actual, uint32_t line, const char *msg)
{
    if (expected == actual) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
         printf("  [FAIL] %s:%lu - %s (Expected: %d, Actual: %d)\n", 
             Unity.CurrentTestName, (unsigned long)line, msg, expected, actual);
    }
}

void unity_assert_equal_uint(uint32_t expected, uint32_t actual, uint32_t line, const char *msg)
{
    if (expected == actual) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
         printf("  [FAIL] %s:%lu - %s (Expected: %lu, Actual: %lu)\n", 
             Unity.CurrentTestName, (unsigned long)line, msg, (unsigned long)expected, (unsigned long)actual);
    }
}

void unity_assert_equal_ptr(const void *expected, const void *actual, uint32_t line, const char *msg)
{
    if (expected == actual) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
         printf("  [FAIL] %s:%lu - %s (Expected: %p, Actual: %p)\n", 
             Unity.CurrentTestName, (unsigned long)line, msg, expected, actual);
    }
}

void unity_assert_equal_string(const char *expected, const char *actual, uint32_t line, const char *msg)
{
    if (expected == NULL && actual == NULL) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else if (expected == NULL || actual == NULL) {
        Unity.TestFailures++;
        print_test_result(false, "One string is NULL", line);
    } else if (strcmp(expected, actual) == 0) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
        printf("  [FAIL] %s:%lu - %s\n", Unity.CurrentTestName, (unsigned long)line, msg);
        printf("    Expected: \"%s\"\n", expected);
        printf("    Actual:   \"%s\"\n", actual);
    }
}

void unity_assert_equal_memory(const void *expected, const void *actual, uint32_t len, uint32_t line, const char *msg)
{
    if (memcmp(expected, actual, len) == 0) {
        Unity.TestPasses++;
        print_test_result(true, msg, line);
    } else {
        Unity.TestFailures++;
        print_test_result(false, msg, line);
    }
}

void unity_fail(uint32_t line, const char *msg)
{
    Unity.TestFailures++;
    print_test_result(false, msg, line);
}

void unity_pass(void)
{
    Unity.TestPasses++;
    print_test_result(true, "Test passed", Unity.CurrentTestLineNumber);
}

void unity_ignore(uint32_t line, const char *msg)
{
    Unity.TestIgnores++;
    printf("  [IGNORE] %s:%lu - %s\n", Unity.CurrentTestName, (unsigned long)line, msg);
}
