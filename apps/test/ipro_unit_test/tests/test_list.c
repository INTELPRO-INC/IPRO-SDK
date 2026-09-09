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
 * @file test_list.c
 * @brief Unit tests for utils_list (singly-linked, doubly-linked, and pool-based lists)
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "utils_list.h"

/* Test node structure embedding list header */
typedef struct {
    struct utils_list_hdr hdr;
    int value;
} test_node_t;

/* Doubly-linked list test node */
typedef struct {
    utils_dlist_t node;
    int value;
} test_dnode_t;

/* ── Single-linked list (utils_list) tests ── */

TEST_CASE(test_list_init_empty)
{
    struct utils_list list;
    utils_list_init(&list);
    TEST_ASSERT_TRUE(utils_list_is_empty(&list));
    TEST_ASSERT_EQUAL_UINT(0, utils_list_cnt(&list));
    TEST_ASSERT_NULL(utils_list_pick(&list));
}

TEST_CASE(test_list_push_back_and_pop)
{
    struct utils_list list;
    test_node_t a = {.value = 1};
    test_node_t b = {.value = 2};
    test_node_t c = {.value = 3};

    utils_list_init(&list);
    utils_list_push_back(&list, &a.hdr);
    utils_list_push_back(&list, &b.hdr);
    utils_list_push_back(&list, &c.hdr);

    TEST_ASSERT_EQUAL_UINT(3, utils_list_cnt(&list));

    /* Pop front should return in FIFO order */
    test_node_t *n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(1, n->value);
    n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(2, n->value);
    n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(3, n->value);

    TEST_ASSERT_TRUE(utils_list_is_empty(&list));
    TEST_ASSERT_NULL(utils_list_pop_front(&list));
}

TEST_CASE(test_list_push_front)
{
    struct utils_list list;
    test_node_t a = {.value = 10};
    test_node_t b = {.value = 20};

    utils_list_init(&list);
    utils_list_push_front(&list, &a.hdr);
    utils_list_push_front(&list, &b.hdr);

    /* b was pushed last to front, should come out first */
    test_node_t *n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(20, n->value);
    n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(10, n->value);
}

TEST_CASE(test_list_find_and_extract)
{
    struct utils_list list;
    test_node_t a = {.value = 1};
    test_node_t b = {.value = 2};
    test_node_t c = {.value = 3};
    test_node_t d = {.value = 99};

    utils_list_init(&list);
    utils_list_push_back(&list, &a.hdr);
    utils_list_push_back(&list, &b.hdr);
    utils_list_push_back(&list, &c.hdr);

    /* Find existing element */
    TEST_ASSERT_TRUE(utils_list_find(&list, &b.hdr));
    /* Find non-existing element */
    TEST_ASSERT_FALSE(utils_list_find(&list, &d.hdr));

    /* Extract middle element */
    utils_list_extract(&list, &b.hdr);
    TEST_ASSERT_EQUAL_UINT(2, utils_list_cnt(&list));
    TEST_ASSERT_FALSE(utils_list_find(&list, &b.hdr));

    /* Remaining: a, c */
    test_node_t *n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(1, n->value);
    n = (test_node_t *)utils_list_pop_front(&list);
    TEST_ASSERT_EQUAL_INT(3, n->value);
}

TEST_CASE(test_list_concat)
{
    struct utils_list list1, list2;
    test_node_t a = {.value = 1};
    test_node_t b = {.value = 2};
    test_node_t c = {.value = 3};
    test_node_t d = {.value = 4};

    utils_list_init(&list1);
    utils_list_init(&list2);

    utils_list_push_back(&list1, &a.hdr);
    utils_list_push_back(&list1, &b.hdr);
    utils_list_push_back(&list2, &c.hdr);
    utils_list_push_back(&list2, &d.hdr);

    utils_list_concat(&list1, &list2);

    TEST_ASSERT_EQUAL_UINT(4, utils_list_cnt(&list1));
    TEST_ASSERT_TRUE(utils_list_is_empty(&list2));

    /* Should be a, b, c, d in order */
    test_node_t *n;
    n = (test_node_t *)utils_list_pop_front(&list1);
    TEST_ASSERT_EQUAL_INT(1, n->value);
    n = (test_node_t *)utils_list_pop_front(&list1);
    TEST_ASSERT_EQUAL_INT(2, n->value);
    n = (test_node_t *)utils_list_pop_front(&list1);
    TEST_ASSERT_EQUAL_INT(3, n->value);
    n = (test_node_t *)utils_list_pop_front(&list1);
    TEST_ASSERT_EQUAL_INT(4, n->value);
}

/* ── Doubly-linked list (utils_dlist) tests ── */

TEST_CASE(test_dlist_add_and_iterate)
{
    utils_dlist_t head;
    test_dnode_t a = {.value = 10};
    test_dnode_t b = {.value = 20};
    test_dnode_t c = {.value = 30};

    INIT_UTILS_DLIST_HEAD(&head);
    TEST_ASSERT_TRUE(utils_dlist_empty(&head));

    utils_dlist_add_tail(&a.node, &head);
    utils_dlist_add_tail(&b.node, &head);
    utils_dlist_add_tail(&c.node, &head);

    TEST_ASSERT_FALSE(utils_dlist_empty(&head));
    TEST_ASSERT_EQUAL_INT(3, utils_dlist_entry_number(&head));

    /* Iterate and check order */
    int expected[] = {10, 20, 30};
    int idx = 0;
    utils_dlist_t *pos;
    utils_dlist_for_each(pos, &head) {
        test_dnode_t *entry = utils_dlist_entry(pos, test_dnode_t, node);
        TEST_ASSERT_EQUAL_INT(expected[idx++], entry->value);
    }
    TEST_ASSERT_EQUAL_INT(3, idx);
}

TEST_CASE(test_dlist_del)
{
    utils_dlist_t head;
    test_dnode_t a = {.value = 1};
    test_dnode_t b = {.value = 2};
    test_dnode_t c = {.value = 3};

    INIT_UTILS_DLIST_HEAD(&head);
    utils_dlist_add_tail(&a.node, &head);
    utils_dlist_add_tail(&b.node, &head);
    utils_dlist_add_tail(&c.node, &head);

    /* Remove middle element */
    utils_dlist_del(&b.node);
    TEST_ASSERT_EQUAL_INT(2, utils_dlist_entry_number(&head));

    /* Check remaining: a, c */
    test_dnode_t *first = utils_dlist_first_entry(&head, test_dnode_t, node);
    TEST_ASSERT_EQUAL_INT(1, first->value);
}

TEST_CASE(test_dlist_add_front)
{
    utils_dlist_t head;
    test_dnode_t a = {.value = 100};
    test_dnode_t b = {.value = 200};

    INIT_UTILS_DLIST_HEAD(&head);
    utils_dlist_add(&a.node, &head);  /* add to front */
    utils_dlist_add(&b.node, &head);  /* add to front again */

    /* b should be first (most recently added to front) */
    test_dnode_t *first = utils_dlist_first_entry(&head, test_dnode_t, node);
    TEST_ASSERT_EQUAL_INT(200, first->value);
}

void test_list_run(void)
{
    printf("Running linked list tests\n");

    RUN_TEST(test_list_init_empty);
    RUN_TEST(test_list_push_back_and_pop);
    RUN_TEST(test_list_push_front);
    RUN_TEST(test_list_find_and_extract);
    RUN_TEST(test_list_concat);
    RUN_TEST(test_dlist_add_and_iterate);
    RUN_TEST(test_dlist_del);
    RUN_TEST(test_dlist_add_front);
}
