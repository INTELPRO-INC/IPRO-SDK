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
 * @file test_rbtree.c
 * @brief Unit tests for red-black tree (utils_rbtree) component
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "unity.h"
#include "utils_rbtree.h"

/* Simple integer comparison callback */
static int int_cmp_cb(struct rb_tree *self, struct rb_node *a, struct rb_node *b)
{
    (void)self;
    int *val_a = (int *)a->value;
    int *val_b = (int *)b->value;
    
    if (*val_a < *val_b) return -1;
    if (*val_a > *val_b) return 1;
    return 0;
}

/* Test cases */
TEST_CASE(test_rbtree_create_and_size)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_EQUAL_UINT(0, rb_tree_size(tree));
    
    rb_tree_dealloc(tree, rb_tree_node_dealloc_cb);
}

TEST_CASE(test_rbtree_insert_find)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    TEST_ASSERT_NOT_NULL(tree);
    
    /* Insert some values */
    int vals[] = {5, 3, 7, 1, 9};
    for (int i = 0; i < 5; i++) {
        int *val = (int *)pvPortMalloc(sizeof(int));
        *val = vals[i];
        int ret = rb_tree_insert(tree, val);
        TEST_ASSERT_EQUAL_INT(1, ret);
    }
    
    TEST_ASSERT_EQUAL_UINT(5, rb_tree_size(tree));
    
    /* Find values */
    int search_val = 7;
    int *found = (int *)rb_tree_find(tree, &search_val);
    TEST_ASSERT_NOT_NULL(found);
    TEST_ASSERT_EQUAL_INT(7, *found);
    
    /* Find non-existent */
    search_val = 100;
    found = (int *)rb_tree_find(tree, &search_val);
    TEST_ASSERT_NULL(found);
    
    rb_tree_dealloc(tree, rb_tree_node_dealloc_cb);
}

TEST_CASE(test_rbtree_remove)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    
    /* Insert values */
    int vals[] = {5, 3, 7, 1, 9, 4, 6};
    for (int i = 0; i < 7; i++) {
        int *val = (int *)pvPortMalloc(sizeof(int));
        *val = vals[i];
        rb_tree_insert(tree, val);
    }
    
    TEST_ASSERT_EQUAL_UINT(7, rb_tree_size(tree));
    
    /* Remove middle element */
    int remove_val = 5;
    int ret = rb_tree_remove(tree, &remove_val);
    TEST_ASSERT_EQUAL_INT(1, ret);
    TEST_ASSERT_EQUAL_UINT(6, rb_tree_size(tree));
    
    /* Verify it's gone */
    int *found = (int *)rb_tree_find(tree, &remove_val);
    TEST_ASSERT_NULL(found);
    
    rb_tree_dealloc(tree, rb_tree_node_dealloc_cb);
}

TEST_CASE(test_rbtree_iterator)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    
    /* Insert values in random order */
    int vals[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) {
        int *val = (int *)pvPortMalloc(sizeof(int));
        *val = vals[i];
        rb_tree_insert(tree, val);
    }
    
    /* Iterate from first */
    struct rb_iter iter;
    rb_iter_init(&iter);
    
    int prev_val = -1;
    int count = 0;
    for (int *val = (int *)rb_iter_first(&iter, tree); val != NULL; val = (int *)rb_iter_next(&iter)) {
        TEST_ASSERT(*val >= prev_val); /* Should be in sorted order */
        prev_val = *val;
        count++;
    }
    
    TEST_ASSERT_EQUAL_INT(7, count);
    
    rb_tree_dealloc(tree, rb_tree_node_dealloc_cb);
}

TEST_CASE(test_rbtree_duplicate_values)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    
    /* Insert duplicate values */
    int val1 = 5;
    int val2 = 5;
    int ret1 = rb_tree_insert(tree, &val1);
    int ret2 = rb_tree_insert(tree, &val2);
    
    /* Tree may or may not allow duplicates depending on implementation */
    /* Just verify no crash */
    TEST_ASSERT(ret1 == 1 || ret1 == 0);
    TEST_ASSERT(ret2 == 1 || ret2 == 0);
    
    rb_tree_dealloc(tree, NULL);
}

TEST_CASE(test_rbtree_large_dataset)
{
    struct rb_tree *tree = rb_tree_create(int_cmp_cb);
    
    /* Insert 50 values */
    for (int i = 50; i > 0; i--) {
        int *val = (int *)pvPortMalloc(sizeof(int));
        *val = i;
        rb_tree_insert(tree, val);
    }
    
    TEST_ASSERT_EQUAL_UINT(50, rb_tree_size(tree));
    
    /* Verify all can be found */
    for (int i = 1; i <= 50; i++) {
        int *found = (int *)rb_tree_find(tree, &i);
        TEST_ASSERT_NOT_NULL(found);
        TEST_ASSERT_EQUAL_INT(i, *found);
    }
    
    rb_tree_dealloc(tree, rb_tree_node_dealloc_cb);
}

/* Test suite runner */
void test_rbtree_run(void)
{
    printf("Running red-black tree (rbtree) component tests\n");
    
    RUN_TEST(test_rbtree_create_and_size);
    RUN_TEST(test_rbtree_insert_find);
    RUN_TEST(test_rbtree_remove);
    RUN_TEST(test_rbtree_iterator);
    RUN_TEST(test_rbtree_duplicate_values);
    RUN_TEST(test_rbtree_large_dataset);
}
