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
 * @file unity.h
 * @brief Lightweight Unity Test Framework Header
 * 
 * Simplified Unity-compatible test framework for embedded systems.
 */

#ifndef UNITY_H
#define UNITY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/* Unity test results structure */
typedef struct {
    uint32_t NumberOfTests;
    uint32_t TestFailures;
    uint32_t TestPasses;
    uint32_t TestIgnores;
    const char *CurrentTestName;
    uint32_t CurrentTestLineNumber;
} Unity_t;

extern Unity_t Unity;

/* Unity macros */
#define UNITY_BEGIN()           unity_begin()
#define UNITY_END()             unity_end()

#define TEST_CASE(name)         void name(void)
#define RUN_TEST(func)          unity_run_test(func, #func, __LINE__)

/* Assertion macros */
#define TEST_ASSERT(condition) \
    unity_assert((condition), __LINE__, "Assertion failed: " #condition)

#define TEST_ASSERT_TRUE(condition) \
    unity_assert((condition), __LINE__, "Expected TRUE")

#define TEST_ASSERT_FALSE(condition) \
    unity_assert(!(condition), __LINE__, "Expected FALSE")

#define TEST_ASSERT_EQUAL(expected, actual) \
    unity_assert_equal_int((expected), (actual), __LINE__, "Values not equal")

#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    unity_assert_equal_int((expected), (actual), __LINE__, "Integers not equal")

#define TEST_ASSERT_EQUAL_UINT(expected, actual) \
    unity_assert_equal_uint((expected), (actual), __LINE__, "Unsigned integers not equal")

#define TEST_ASSERT_EQUAL_PTR(expected, actual) \
    unity_assert_equal_ptr((expected), (actual), __LINE__, "Pointers not equal")

#define TEST_ASSERT_NOT_NULL(ptr) \
    unity_assert((ptr) != NULL, __LINE__, "Pointer is NULL")

#define TEST_ASSERT_NULL(ptr) \
    unity_assert((ptr) == NULL, __LINE__, "Pointer is not NULL")

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    unity_assert_equal_string((expected), (actual), __LINE__, "Strings not equal")

#define TEST_ASSERT_EQUAL_MEMORY(expected, actual, len) \
    unity_assert_equal_memory((expected), (actual), (len), __LINE__, "Memory not equal")

#define TEST_FAIL(msg) \
    unity_fail(__LINE__, msg)

#define TEST_PASS() \
    unity_pass()

#define TEST_IGNORE() \
    unity_ignore(__LINE__, "Test ignored")

#define TEST_IGNORE_MESSAGE(msg) \
    unity_ignore(__LINE__, msg)

/* Unity functions */
void unity_begin(void);
int unity_end(void);
void unity_run_test(void (*test_func)(void), const char *test_name, uint32_t line);

void unity_assert(bool condition, uint32_t line, const char *msg);
void unity_assert_equal_int(int expected, int actual, uint32_t line, const char *msg);
void unity_assert_equal_uint(uint32_t expected, uint32_t actual, uint32_t line, const char *msg);
void unity_assert_equal_ptr(const void *expected, const void *actual, uint32_t line, const char *msg);
void unity_assert_equal_string(const char *expected, const char *actual, uint32_t line, const char *msg);
void unity_assert_equal_memory(const void *expected, const void *actual, uint32_t len, uint32_t line, const char *msg);

void unity_fail(uint32_t line, const char *msg);
void unity_pass(void);
void unity_ignore(uint32_t line, const char *msg);

#ifdef __cplusplus
}
#endif

#endif /* UNITY_H */
