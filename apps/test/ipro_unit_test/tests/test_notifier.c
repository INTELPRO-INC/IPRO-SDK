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
 * @file test_notifier.c
 * @brief Unit tests for utils_notifier chain
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_notifier.h"
#include "utils_list.h"

/* Callback tracking state */
static volatile int notifier_call_count = 0;
static volatile int notifier_last_env = 0;

static int test_notifier_cb(void *cb_arg, void *env)
{
    (void)cb_arg;
    notifier_call_count++;
    if (env) {
        notifier_last_env = *(int *)env;
    }
    return 0;
}

/* Second callback for multi-register test */
static volatile int notifier_cb2_count = 0;

static int test_notifier_cb2(void *cb_arg, void *env)
{
    (void)cb_arg;
    (void)env;
    notifier_cb2_count++;
    return 0;
}

TEST_CASE(test_notifier_chain_init)
{
    ntf_list_t list;
    int ret = utils_notifier_chain_init(&list);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_notifier_register_and_call)
{
    ntf_list_t list;
    utils_notifier_t notifier;

    notifier_call_count = 0;
    notifier_last_env = 0;

    utils_notifier_chain_init(&list);

    memset(&notifier, 0, sizeof(notifier));
    notifier.cb = test_notifier_cb;
    notifier.cb_arg = NULL;
    notifier.priority = 0;

    int ret = utils_notifier_chain_register(&list, &notifier);
    TEST_ASSERT_EQUAL_INT(0, ret);

    int env_val = 42;
    ret = utils_notifier_chain_call(&list, &env_val);
    TEST_ASSERT_EQUAL_INT(0, ret);

    TEST_ASSERT_EQUAL_INT(1, notifier_call_count);
    TEST_ASSERT_EQUAL_INT(42, notifier_last_env);
}

TEST_CASE(test_notifier_multiple_callbacks)
{
    ntf_list_t list;
    utils_notifier_t n1, n2;

    notifier_call_count = 0;
    notifier_cb2_count = 0;

    utils_notifier_chain_init(&list);

    memset(&n1, 0, sizeof(n1));
    n1.cb = test_notifier_cb;
    n1.cb_arg = NULL;
    n1.priority = 10;

    memset(&n2, 0, sizeof(n2));
    n2.cb = test_notifier_cb2;
    n2.cb_arg = NULL;
    n2.priority = 20;

    utils_notifier_chain_register(&list, &n1);
    utils_notifier_chain_register(&list, &n2);

    int ret = utils_notifier_chain_call(&list, NULL);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Both callbacks should have been called */
    TEST_ASSERT_EQUAL_INT(1, notifier_call_count);
    TEST_ASSERT_EQUAL_INT(1, notifier_cb2_count);
}

TEST_CASE(test_notifier_call_empty_chain)
{
    ntf_list_t list;
    utils_notifier_chain_init(&list);

    /* Calling an empty chain should not crash */
    int ret = utils_notifier_chain_call(&list, NULL);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_notifier_run(void)
{
    printf("Running notifier chain tests\n");

    RUN_TEST(test_notifier_chain_init);
    RUN_TEST(test_notifier_register_and_call);
    RUN_TEST(test_notifier_multiple_callbacks);
    RUN_TEST(test_notifier_call_empty_chain);
}
