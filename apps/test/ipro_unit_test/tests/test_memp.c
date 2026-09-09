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
 * @file test_memp.c
 * @brief Unit tests for utils_memp memory pool
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "utils_memp.h"

#define TEST_NODE_SIZE  32
#define TEST_POOL_CAP   4
#define TEST_ALIGN_REQ  4

TEST_CASE(test_memp_init)
{
    utils_memp_pool_t *pool = NULL;
    int ret = utils_memp_init(&pool, TEST_NODE_SIZE, TEST_POOL_CAP, TEST_ALIGN_REQ);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_NOT_NULL(pool);

    ret = utils_memp_deinit(pool);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_memp_alloc_free_single)
{
    utils_memp_pool_t *pool = NULL;
    int ret = utils_memp_init(&pool, TEST_NODE_SIZE, TEST_POOL_CAP, TEST_ALIGN_REQ);
    TEST_ASSERT_EQUAL_INT(0, ret);

    void *node = utils_memp_malloc(pool);
    TEST_ASSERT_NOT_NULL(node);

    /* Should be able to write to the allocated memory */
    memset(node, 0xAA, TEST_NODE_SIZE);

    ret = utils_memp_free(pool, node);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = utils_memp_deinit(pool);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_memp_alloc_exhaustion)
{
    utils_memp_pool_t *pool = NULL;
    int ret = utils_memp_init(&pool, TEST_NODE_SIZE, TEST_POOL_CAP, TEST_ALIGN_REQ);
    TEST_ASSERT_EQUAL_INT(0, ret);

    void *nodes[TEST_POOL_CAP];

    /* Allocate all nodes in the pool */
    for (int i = 0; i < TEST_POOL_CAP; i++) {
        nodes[i] = utils_memp_malloc(pool);
        TEST_ASSERT_NOT_NULL(nodes[i]);
    }

    /* Next allocation should fail (pool exhausted) */
    void *extra = utils_memp_malloc(pool);
    TEST_ASSERT_NULL(extra);

    /* Free all nodes */
    for (int i = 0; i < TEST_POOL_CAP; i++) {
        ret = utils_memp_free(pool, nodes[i]);
        TEST_ASSERT_EQUAL_INT(0, ret);
    }

    ret = utils_memp_deinit(pool);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_memp_free_and_realloc)
{
    utils_memp_pool_t *pool = NULL;
    int ret = utils_memp_init(&pool, TEST_NODE_SIZE, TEST_POOL_CAP, TEST_ALIGN_REQ);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Allocate one node */
    void *node1 = utils_memp_malloc(pool);
    TEST_ASSERT_NOT_NULL(node1);

    /* Free it */
    ret = utils_memp_free(pool, node1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Re-allocate: should succeed since we freed one */
    void *node2 = utils_memp_malloc(pool);
    TEST_ASSERT_NOT_NULL(node2);

    ret = utils_memp_free(pool, node2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = utils_memp_deinit(pool);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_memp_deinit)
{
    utils_memp_pool_t *pool = NULL;
    int ret = utils_memp_init(&pool, TEST_NODE_SIZE, TEST_POOL_CAP, TEST_ALIGN_REQ);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Allocate and free some nodes before deinit */
    void *n1 = utils_memp_malloc(pool);
    void *n2 = utils_memp_malloc(pool);
    TEST_ASSERT_NOT_NULL(n1);
    TEST_ASSERT_NOT_NULL(n2);
    utils_memp_free(pool, n1);
    utils_memp_free(pool, n2);

    ret = utils_memp_deinit(pool);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

void test_memp_run(void)
{
    printf("Running memory pool tests\n");

    RUN_TEST(test_memp_init);
    RUN_TEST(test_memp_alloc_free_single);
    RUN_TEST(test_memp_alloc_exhaustion);
    RUN_TEST(test_memp_free_and_realloc);
    RUN_TEST(test_memp_deinit);
}
