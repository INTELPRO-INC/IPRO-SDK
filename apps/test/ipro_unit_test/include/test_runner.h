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
 * @file test_runner.h
 * @brief Unit Test Runner Interface
 *
 * Provides the main interface for executing and managing unit tests.
 * Supports test groups for organizing tests by category.
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test group categories
 */
typedef enum {
    TEST_GROUP_SW = 0,      /**< Software tests (no HW dependency) */
    TEST_GROUP_HAL,         /**< HAL driver tests (GPIO, Timer, Flash, etc.) */
    TEST_GROUP_CRYPTO,      /**< HW security engine tests (AES, SHA, TRNG) */
    TEST_GROUP_BLE,         /**< BLE stack tests */
    TEST_GROUP_MAX
} test_group_t;

/**
 * @brief Initialize the test runner
 *
 * Must be called before executing any tests.
 *
 * @return 0 on success, negative on failure
 */
int test_runner_init(void);

/**
 * @brief Execute all registered test suites
 *
 * @return 0 if all tests passed, negative if any test failed
 */
int test_runner_execute_all(void);

/**
 * @brief Execute a specific test suite by name
 *
 * @param suite_name Name of the test suite to execute
 * @return 0 if tests passed, negative if tests failed or suite not found
 */
int test_runner_execute_suite(const char *suite_name);

/**
 * @brief Execute all test suites in a specific group
 *
 * @param group_name Group name: "sw", "hal", "crypto", "ble"
 * @return 0 if all tests passed, negative if any failed
 */
int test_runner_execute_group(const char *group_name);

/**
 * @brief List all available test suites (grouped by category)
 */
void test_runner_list_tests(void);

/**
 * @brief Print test execution summary
 */
void test_runner_print_summary(void);

/**
 * @brief Reset test statistics
 */
void test_runner_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_RUNNER_H */
