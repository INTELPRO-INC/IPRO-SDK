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
 * @file test_runner.c
 * @brief Unit Test Runner Implementation
 */

#include <stdio.h>
#include <string.h>
#include "test_runner.h"
#include "unity.h"

/* Test suite structure */
typedef struct {
    const char *name;
    test_group_t group;
    void (*setup)(void);
    void (*teardown)(void);
    void (*run)(void);
} test_suite_t;

/* Group names */
static const char *group_names[] = {
    [TEST_GROUP_SW]     = "sw",
    [TEST_GROUP_HAL]    = "hal",
    [TEST_GROUP_CRYPTO] = "crypto",
    [TEST_GROUP_BLE]    = "ble",
};

static const char *group_labels[] = {
    [TEST_GROUP_SW]     = "Software",
    [TEST_GROUP_HAL]    = "Hardware (HAL)",
    [TEST_GROUP_CRYPTO] = "HW Crypto",
    [TEST_GROUP_BLE]    = "Bluetooth",
};

/* Test statistics */
static struct {
    uint32_t total_tests;
    uint32_t passed_tests;
    uint32_t failed_tests;
    uint32_t skipped_tests;
} test_stats = {0};

/* ========================================================================= */
/* Forward declarations of test suite run functions                          */
/* ========================================================================= */

/* --- Existing SW tests --- */
#ifdef CONFIG_TEST_UTILS
extern void test_utils_run(void);
#endif
#ifdef CONFIG_TEST_MEMORY
extern void test_memory_run(void);
#endif
#ifdef CONFIG_TEST_RTOS
extern void test_rtos_run(void);
#endif
#ifdef CONFIG_TEST_RINGBUF
extern void test_ringbuf_run(void);
#endif
#ifdef CONFIG_TEST_RBTREE
extern void test_rbtree_run(void);
#endif
#ifdef CONFIG_TEST_SOFTCRC
extern void test_softcrc_run(void);
#endif
#ifdef CONFIG_TEST_GETOPT
extern void test_getopt_run(void);
#endif
#ifdef CONFIG_TEST_EVENT
extern void test_event_run(void);
#endif
#ifdef CONFIG_TEST_BASE64
extern void test_base64_run(void);
#endif
#ifdef CONFIG_TEST_LIST
extern void test_list_run(void);
#endif
#ifdef CONFIG_TEST_HEX
extern void test_hex_run(void);
#endif
#ifdef CONFIG_TEST_SHA256
extern void test_sha256_run(void);
#endif

/* --- New SW tests --- */
#ifdef CONFIG_TEST_RTOS_ADV
extern void test_rtos_adv_run(void);
#endif
#ifdef CONFIG_TEST_MEMP
extern void test_memp_run(void);
#endif
#ifdef CONFIG_TEST_MD5
extern void test_md5_run(void);
#endif
#ifdef CONFIG_TEST_SHA1
extern void test_sha1_run(void);
#endif
#ifdef CONFIG_TEST_HMAC
extern void test_hmac_run(void);
#endif
#ifdef CONFIG_TEST_DNS
extern void test_dns_run(void);
#endif
#ifdef CONFIG_TEST_NOTIFIER
extern void test_notifier_run(void);
#endif
#ifdef CONFIG_TEST_BITMAP
extern void test_bitmap_run(void);
#endif

/* --- HAL / Hardware tests --- */
#ifdef CONFIG_TEST_HAL_GPIO
extern void test_hal_gpio_run(void);
#endif
#ifdef CONFIG_TEST_HAL_TIMER
extern void test_hal_timer_run(void);
#endif
#ifdef CONFIG_TEST_HAL_FLASH
extern void test_hal_flash_run(void);
#endif
#ifdef CONFIG_TEST_HAL_DMA
extern void test_hal_dma_run(void);
#endif
#ifdef CONFIG_TEST_HAL_WDT
extern void test_hal_wdt_run(void);
#endif
#ifdef CONFIG_TEST_HAL_RTC
extern void test_hal_rtc_run(void);
#endif
#ifdef CONFIG_TEST_HAL_ADC
extern void test_hal_adc_run(void);
#endif

/* --- HW Crypto engine tests --- */
#ifdef CONFIG_TEST_SEC_AES
extern void test_sec_aes_run(void);
#endif
#ifdef CONFIG_TEST_SEC_HASH
extern void test_sec_hash_run(void);
#endif
#ifdef CONFIG_TEST_SEC_TRNG
extern void test_sec_trng_run(void);
#endif
#ifdef CONFIG_TEST_SEC_GMAC
extern void test_sec_gmac_run(void);
#endif

/* --- BLE tests --- */
#ifdef CONFIG_TEST_BLE
extern void test_ble_run(void);
#endif

/* ========================================================================= */
/* Test suite registry                                                       */
/* ========================================================================= */
static const test_suite_t test_suites[] = {
    /* ---- Software tests ---- */
#ifdef CONFIG_TEST_UTILS
    { .name = "utils",     .group = TEST_GROUP_SW, .run = test_utils_run },
#endif
#ifdef CONFIG_TEST_MEMORY
    { .name = "memory",    .group = TEST_GROUP_SW, .run = test_memory_run },
#endif
#ifdef CONFIG_TEST_RTOS
    { .name = "rtos",      .group = TEST_GROUP_SW, .run = test_rtos_run },
#endif
#ifdef CONFIG_TEST_RINGBUF
    { .name = "ringbuf",   .group = TEST_GROUP_SW, .run = test_ringbuf_run },
#endif
#ifdef CONFIG_TEST_RBTREE
    { .name = "rbtree",    .group = TEST_GROUP_SW, .run = test_rbtree_run },
#endif
#ifdef CONFIG_TEST_SOFTCRC
    { .name = "softcrc",   .group = TEST_GROUP_SW, .run = test_softcrc_run },
#endif
#ifdef CONFIG_TEST_GETOPT
    { .name = "getopt",    .group = TEST_GROUP_SW, .run = test_getopt_run },
#endif
#ifdef CONFIG_TEST_EVENT
    { .name = "event",     .group = TEST_GROUP_SW, .run = test_event_run },
#endif
#ifdef CONFIG_TEST_BASE64
    { .name = "base64",    .group = TEST_GROUP_SW, .run = test_base64_run },
#endif
#ifdef CONFIG_TEST_LIST
    { .name = "list",      .group = TEST_GROUP_SW, .run = test_list_run },
#endif
#ifdef CONFIG_TEST_HEX
    { .name = "hex",       .group = TEST_GROUP_SW, .run = test_hex_run },
#endif
#ifdef CONFIG_TEST_SHA256
    { .name = "sha256",    .group = TEST_GROUP_SW, .run = test_sha256_run },
#endif
#ifdef CONFIG_TEST_RTOS_ADV
    { .name = "rtos_adv",  .group = TEST_GROUP_SW, .run = test_rtos_adv_run },
#endif
#ifdef CONFIG_TEST_MEMP
    { .name = "memp",      .group = TEST_GROUP_SW, .run = test_memp_run },
#endif
#ifdef CONFIG_TEST_MD5
    { .name = "md5",       .group = TEST_GROUP_SW, .run = test_md5_run },
#endif
#ifdef CONFIG_TEST_SHA1
    { .name = "sha1",      .group = TEST_GROUP_SW, .run = test_sha1_run },
#endif
#ifdef CONFIG_TEST_HMAC
    { .name = "hmac",      .group = TEST_GROUP_SW, .run = test_hmac_run },
#endif
#ifdef CONFIG_TEST_DNS
    { .name = "dns",       .group = TEST_GROUP_SW, .run = test_dns_run },
#endif
#ifdef CONFIG_TEST_NOTIFIER
    { .name = "notifier",  .group = TEST_GROUP_SW, .run = test_notifier_run },
#endif
#ifdef CONFIG_TEST_BITMAP
    { .name = "bitmap",    .group = TEST_GROUP_SW, .run = test_bitmap_run },
#endif

    /* ---- HAL Hardware tests ---- */
#ifdef CONFIG_TEST_HAL_GPIO
    { .name = "hal_gpio",  .group = TEST_GROUP_HAL, .run = test_hal_gpio_run },
#endif
#ifdef CONFIG_TEST_HAL_TIMER
    { .name = "hal_timer", .group = TEST_GROUP_HAL, .run = test_hal_timer_run },
#endif
#ifdef CONFIG_TEST_HAL_FLASH
    { .name = "hal_flash", .group = TEST_GROUP_HAL, .run = test_hal_flash_run },
#endif
#ifdef CONFIG_TEST_HAL_DMA
    { .name = "hal_dma",   .group = TEST_GROUP_HAL, .run = test_hal_dma_run },
#endif
#ifdef CONFIG_TEST_HAL_WDT
    { .name = "hal_wdt",   .group = TEST_GROUP_HAL, .run = test_hal_wdt_run },
#endif
#ifdef CONFIG_TEST_HAL_RTC
    { .name = "hal_rtc",   .group = TEST_GROUP_HAL, .run = test_hal_rtc_run },
#endif
#ifdef CONFIG_TEST_HAL_ADC
    { .name = "hal_adc",   .group = TEST_GROUP_HAL, .run = test_hal_adc_run },
#endif

    /* ---- HW Crypto engine tests ---- */
#ifdef CONFIG_TEST_SEC_AES
    { .name = "sec_aes",   .group = TEST_GROUP_CRYPTO, .run = test_sec_aes_run },
#endif
#ifdef CONFIG_TEST_SEC_HASH
    { .name = "sec_hash",  .group = TEST_GROUP_CRYPTO, .run = test_sec_hash_run },
#endif
#ifdef CONFIG_TEST_SEC_TRNG
    { .name = "sec_trng",  .group = TEST_GROUP_CRYPTO, .run = test_sec_trng_run },
#endif
#ifdef CONFIG_TEST_SEC_GMAC
    { .name = "sec_gmac",  .group = TEST_GROUP_CRYPTO, .run = test_sec_gmac_run },
#endif

    /* ---- BLE tests ---- */
#ifdef CONFIG_TEST_BLE
    { .name = "ble",       .group = TEST_GROUP_BLE, .run = test_ble_run },
#endif
};

#define NUM_TEST_SUITES (sizeof(test_suites) / sizeof(test_suites[0]))

/* ========================================================================= */
/* Helper: parse group name string to enum                                   */
/* ========================================================================= */
static int group_from_name(const char *name)
{
    for (int g = 0; g < TEST_GROUP_MAX; g++) {
        if (strcmp(name, group_names[g]) == 0)
            return g;
    }
    return -1;
}

/* ========================================================================= */
/* Helper: run a single suite, update stats, print result                    */
/* ========================================================================= */
static int run_suite(const test_suite_t *suite, size_t idx, size_t total)
{
    printf("\n[%zu/%zu] %-20s ", idx + 1, total, suite->name);

    if (suite->setup)
        suite->setup();

    UNITY_BEGIN();
    suite->run();
    int suite_result = UNITY_END();

    uint32_t suite_count = Unity.NumberOfTests;
    uint32_t suite_fails = Unity.TestFailures;

    test_stats.total_tests  += suite_count;
    test_stats.passed_tests += Unity.TestPasses;
    test_stats.failed_tests += suite_fails;
    test_stats.skipped_tests += Unity.TestIgnores;

    if (suite_fails == 0 && suite_count > 0) {
        printf("[ PASS %u tests ]\n", suite_count);
    } else if (suite_count == 0) {
        printf("[ - skipped ]\n");
    } else {
        printf("[ FAIL %u/%u ]\n", suite_fails, suite_count);
    }

    if (suite->teardown)
        suite->teardown();

    return suite_result;
}

/* ========================================================================= */
/* Public API                                                                */
/* ========================================================================= */

int test_runner_init(void)
{
    printf("Initializing test runner\n");
    printf("Registered test suites: %d\n", (int)NUM_TEST_SUITES);
    memset(&test_stats, 0, sizeof(test_stats));
    return 0;
}

int test_runner_execute_all(void)
{
    int result = 0;

    printf("\n");
    printf("========================================================\n");
    printf("  IPRO SDK Unit Test Suite (v2.0)\n");
    printf("  Executing All Test Suites\n");
    printf("========================================================\n");

    memset(&test_stats, 0, sizeof(test_stats));

    for (int g = 0; g < TEST_GROUP_MAX; g++) {
        /* Count suites in this group */
        size_t count = 0;
        for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
            if (test_suites[i].group == g) count++;
        }
        if (count == 0) continue;

        printf("\n--- [%s] %s Tests (%zu suites) ---\n",
               group_names[g], group_labels[g], count);

        size_t idx = 0;
        for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
            if (test_suites[i].group != g) continue;

            int sr = run_suite(&test_suites[i], idx, count);
            idx++;

            if (sr != 0) {
                result = -1;
#ifdef CONFIG_TEST_STOP_ON_FAIL
                printf("\n[!] Stopping on first failure (CONFIG_TEST_STOP_ON_FAIL)\n");
                goto done;
#endif
            }
        }
    }

#ifdef CONFIG_TEST_STOP_ON_FAIL
done:
#endif
    /* Print summary */
    printf("\n========================================================\n");
    printf("  TEST SUMMARY REPORT\n");
    printf("========================================================\n");
    uint32_t cases_passed = test_stats.total_tests - test_stats.failed_tests - test_stats.skipped_tests;
    printf("  Total Tests:    %3u\n", test_stats.total_tests);
    printf("  Passed:         %3u\n", cases_passed);
    printf("  Failed:         %3u\n", test_stats.failed_tests);
    printf("  Skipped:        %3u\n", test_stats.skipped_tests);
    printf("--------------------------------------------------------\n");

    if (test_stats.failed_tests == 0 && test_stats.total_tests > 0) {
        printf("  Result:         ALL TESTS PASSED\n");
    } else if (test_stats.total_tests == 0) {
        printf("  Result:         NO TESTS EXECUTED\n");
    } else {
        printf("  Result:         SOME TESTS FAILED\n");
    }

    printf("========================================================\n\n");
    return result;
}

int test_runner_execute_group(const char *group_name)
{
    if (!group_name) return -1;

    int g = group_from_name(group_name);
    if (g < 0) {
        printf("ERROR: Unknown test group '%s'\n", group_name);
        printf("Available groups: sw, hal, crypto, ble\n");
        return -1;
    }

    printf("\n========================================================\n");
    printf("  Running group: [%s] %s\n", group_names[g], group_labels[g]);
    printf("========================================================\n");

    memset(&test_stats, 0, sizeof(test_stats));
    int result = 0;

    /* Count suites in group */
    size_t count = 0;
    for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
        if (test_suites[i].group == g) count++;
    }

    if (count == 0) {
        printf("  No test suites in this group\n");
        return 0;
    }

    size_t idx = 0;
    for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
        if (test_suites[i].group != g) continue;

        int sr = run_suite(&test_suites[i], idx, count);
        idx++;

        if (sr != 0) {
            result = -1;
#ifdef CONFIG_TEST_STOP_ON_FAIL
            break;
#endif
        }
    }

    printf("\n  Group [%s]: %u passed, %u failed, %u skipped / %u total\n",
           group_names[g], test_stats.passed_tests, test_stats.failed_tests,
           test_stats.skipped_tests, test_stats.total_tests);

    return result;
}

int test_runner_execute_suite(const char *suite_name)
{
    if (!suite_name) return -1;

    for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
        if (strcmp(test_suites[i].name, suite_name) == 0) {
            printf("Running test suite: %s [%s]\n",
                   test_suites[i].name, group_names[test_suites[i].group]);

            if (test_suites[i].setup)
                test_suites[i].setup();

            UNITY_BEGIN();
            test_suites[i].run();
            int result = UNITY_END();

            test_stats.total_tests  += Unity.NumberOfTests;
            test_stats.passed_tests += Unity.TestPasses;
            test_stats.failed_tests += Unity.TestFailures;
            test_stats.skipped_tests += Unity.TestIgnores;

            if (test_suites[i].teardown)
                test_suites[i].teardown();

            return result;
        }
    }

    printf("ERROR: Test suite '%s' not found\n", suite_name);
    return -1;
}

void test_runner_list_tests(void)
{
    printf("\n========================================================\n");
    printf("  Available Test Suites (%zu total)\n", NUM_TEST_SUITES);
    printf("========================================================\n");

    for (int g = 0; g < TEST_GROUP_MAX; g++) {
        size_t count = 0;
        for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
            if (test_suites[i].group == g) count++;
        }
        if (count == 0) continue;

        printf("\n  [%s] %s (%zu suites):\n", group_names[g], group_labels[g], count);
        for (size_t i = 0; i < NUM_TEST_SUITES; i++) {
            if (test_suites[i].group == g)
                printf("    - %-20s\n", test_suites[i].name);
        }
    }

    printf("\n--------------------------------------------------------\n");
    printf("  Commands:\n");
    printf("    test_all             Run all test suites\n");
    printf("    test_group <name>    Run group (sw|hal|crypto|ble)\n");
    printf("    test_run <suite>     Run specific suite\n");
    printf("    test_list            Show this list\n");
    printf("========================================================\n");
}

void test_runner_print_summary(void)
{
    printf("\n========================================================\n");
    printf("  DETAILED SUMMARY\n");
    printf("========================================================\n");
    uint32_t cases_passed = test_stats.total_tests - test_stats.failed_tests - test_stats.skipped_tests;
    printf("  Total Tests:         %-3u\n", test_stats.total_tests);
    printf("  Passed:              %-3u\n", cases_passed);
    printf("  Failed:              %-3u\n", test_stats.failed_tests);
    printf("  Skipped:             %-3u\n", test_stats.skipped_tests);
    printf("  Assertions:          %-3u\n", test_stats.passed_tests + test_stats.failed_tests);

    if (test_stats.total_tests > 0) {
        uint32_t pass_rate = (cases_passed * 100) / test_stats.total_tests;
        printf("  Pass Rate:           %3u%%\n", pass_rate);
    }

    printf("--------------------------------------------------------\n");
    if (test_stats.failed_tests == 0 && test_stats.total_tests > 0) {
        printf("  Status:              ALL PASSED\n");
    } else if (test_stats.total_tests == 0) {
        printf("  Status:              NO TESTS\n");
    } else {
        printf("  Status:              FAILURES DETECTED\n");
    }
    printf("========================================================\n");
}

void test_runner_reset(void)
{
    memset(&test_stats, 0, sizeof(test_stats));
    printf("Test statistics reset\n");
}
