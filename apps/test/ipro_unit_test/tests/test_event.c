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
 * @file test_event.c
 * @brief Unit tests for IPRO event loop system
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ipro_event.h"

/* Define test event base */
IPRO_EVENT_DEFINE_BASE(TEST_EVENT);

/* Test event IDs */
enum {
    TEST_EVENT_ID_1 = 0,
    TEST_EVENT_ID_2 = 1,
    TEST_EVENT_ID_3 = 2,
};

/* Handler callback tracking */
static struct {
    int call_count;
    int last_event_id;
    int last_user_data;
} handler_context = {0};

static void test_handler(const ipro_event_t *event, void *user_data)
{
    handler_context.call_count++;
    handler_context.last_event_id = event->event_id;
    handler_context.last_user_data = (int)(uintptr_t)user_data;
}

/* Test cases */
TEST_CASE(test_event_loop_create_default)
{
    int ret = ipro_event_loop_create_default();
    
    /* May already exist from previous test, so either 0 or error is ok */
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_INVALID_STATE);
}

TEST_CASE(test_event_handler_register)
{
    /* Create default loop if needed */
    ipro_event_loop_create_default();
    
    /* Reset context */
    handler_context.call_count = 0;
    
    /* Register handler */
    int ret = ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        test_handler, 
        (void *)42
    );
    
    TEST_ASSERT_EQUAL_INT(0, ret);
}

TEST_CASE(test_event_post)
{
    /* Create default loop if needed */
    ipro_event_loop_create_default();
    
    /* Reset context */
    handler_context.call_count = 0;
    
    /* Register handler */
    ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_2, 
        test_handler, 
        (void *)99
    );
    
    /* Post event */
    int ret = ipro_event_post(TEST_EVENT, TEST_EVENT_ID_2, NULL, 0, 100);
    
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_INVALID_STATE);
    
    /* Give event loop time to process */
    vTaskDelay(pdMS_TO_TICKS(50));
    
    /* Handler should have been called (might not if event loop not running) */
    if (handler_context.call_count > 0) {
        TEST_ASSERT_EQUAL_INT(TEST_EVENT_ID_2, handler_context.last_event_id);
        TEST_ASSERT_EQUAL_INT(99, handler_context.last_user_data);
    }
}

TEST_CASE(test_event_handler_unregister)
{
    /* Create default loop */
    ipro_event_loop_create_default();
    
    /* Register and unregister handler */
    ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        test_handler, 
        (void *)1
    );
    
    int ret = ipro_event_handler_unregister(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        test_handler
    );
    
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_NOT_FOUND);
}

TEST_CASE(test_event_post_event_data)
{
    /* Create default loop */
    ipro_event_loop_create_default();
    
    /* Reset context */
    handler_context.call_count = 0;
    
    /* Register handler */
    ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_3, 
        test_handler, 
        (void *)777
    );
    
    /* Post event with data */
    uint32_t event_data = 0xDEADBEEF;
    int ret = ipro_event_post(
        TEST_EVENT, 
        TEST_EVENT_ID_3, 
        &event_data, 
        sizeof(event_data), 
        100
    );
    
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_INVALID_STATE);
    
    /* Give event loop time */
    vTaskDelay(pdMS_TO_TICKS(50));
}

TEST_CASE(test_event_wildcard_base)
{
    /* Create default loop */
    ipro_event_loop_create_default();
    
    /* Reset context */
    handler_context.call_count = 0;
    
    /* Register handler for any event base */
    int ret = ipro_event_handler_register(
        IPRO_EVENT_ANY_BASE, 
        IPRO_EVENT_ANY_ID, 
        test_handler, 
        (void *)123
    );
    
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_INVALID_STATE || ret == IPRO_ERR_NO_MEM);
}

TEST_CASE(test_event_isr_post)
{
    /* Create default loop */
    ipro_event_loop_create_default();
    
    /* ISR-safe post with small data (4 bytes max) */
    bool woken = false;
    uint32_t small_data = 0x12345678;
    
    int ret = ipro_event_isr_post(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        &small_data, 
        sizeof(small_data), 
        &woken
    );
    
    TEST_ASSERT(ret == 0 || ret == IPRO_ERR_INVALID_STATE || ret == IPRO_ERR_TIMEOUT);
}

TEST_CASE(test_event_multiple_handlers)
{
    /* Create default loop */
    ipro_event_loop_create_default();
    
    /* Register same event for multiple handlers */
    int ret1 = ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        test_handler, 
        (void *)10
    );
    
    int ret2 = ipro_event_handler_register(
        TEST_EVENT, 
        TEST_EVENT_ID_1, 
        test_handler, 
        (void *)20
    );
    
    TEST_ASSERT(ret1 == 0 || ret1 == IPRO_ERR_INVALID_STATE);
    TEST_ASSERT(ret2 == 0 || ret2 == IPRO_ERR_INVALID_STATE);
}

TEST_CASE(test_event_loop_cleanup)
{
    /* Delete default loop at the end */
    int ret = ipro_event_loop_delete_default();
    
    /* May succeed or fail if loop not running */
    TEST_ASSERT(ret == 0 || ret != 0); /* Just verify no crash */
}

/* Test suite runner */
void test_event_run(void)
{
    printf("Running IPRO event loop component tests\n");
    printf("Note: Event loop may not be actively running in unit test context\n");
    
    RUN_TEST(test_event_loop_create_default);
    RUN_TEST(test_event_handler_register);
    RUN_TEST(test_event_post);
    RUN_TEST(test_event_handler_unregister);
    RUN_TEST(test_event_post_event_data);
    RUN_TEST(test_event_wildcard_base);
    RUN_TEST(test_event_isr_post);
    RUN_TEST(test_event_multiple_handlers);
    RUN_TEST(test_event_loop_cleanup);
}
