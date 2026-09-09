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
 * @file test_rtos_adv.c
 * @brief Advanced FreeRTOS tests (mutex, event groups, stream buffers, timers)
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "unity.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "timers.h"

/* ── Mutex tests ── */

TEST_CASE(test_mutex_create_lock_unlock)
{
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    TEST_ASSERT_NOT_NULL(mutex);

    /* Lock (take) the mutex */
    BaseType_t ret = xSemaphoreTake(mutex, pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);

    /* Unlock (give) the mutex */
    ret = xSemaphoreGive(mutex);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);

    vSemaphoreDelete(mutex);
}

TEST_CASE(test_mutex_recursive)
{
    SemaphoreHandle_t mutex = xSemaphoreCreateRecursiveMutex();
    TEST_ASSERT_NOT_NULL(mutex);

    /* Take recursively twice */
    BaseType_t ret = xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    ret = xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);

    /* Give back twice */
    ret = xSemaphoreGiveRecursive(mutex);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    ret = xSemaphoreGiveRecursive(mutex);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);

    vSemaphoreDelete(mutex);
}

/* ── Event group tests ── */

TEST_CASE(test_event_group_create_set_wait)
{
    EventGroupHandle_t eg = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(eg);

    /* Set bits 0 and 2 */
    EventBits_t bits = xEventGroupSetBits(eg, (1 << 0) | (1 << 2));
    TEST_ASSERT((bits & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2)));

    /* Wait for those bits (should return immediately) */
    bits = xEventGroupWaitBits(eg, (1 << 0) | (1 << 2),
                               pdTRUE,  /* clear on exit */
                               pdTRUE,  /* wait for all */
                               0);
    TEST_ASSERT((bits & ((1 << 0) | (1 << 2))) == ((1 << 0) | (1 << 2)));

    /* After clearing, bits should be gone */
    bits = xEventGroupGetBits(eg);
    TEST_ASSERT_EQUAL_UINT(0, bits & ((1 << 0) | (1 << 2)));

    vEventGroupDelete(eg);
}

TEST_CASE(test_event_group_clear_bits)
{
    EventGroupHandle_t eg = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(eg);

    xEventGroupSetBits(eg, 0xFF);
    EventBits_t bits = xEventGroupClearBits(eg, 0x0F);
    /* ClearBits returns the value BEFORE clearing */
    TEST_ASSERT((bits & 0xFF) == 0xFF);

    /* After clear, lower nibble should be zero */
    bits = xEventGroupGetBits(eg);
    TEST_ASSERT_EQUAL_UINT(0xF0, bits & 0xFF);

    vEventGroupDelete(eg);
}

/* ── Stream buffer tests ── */

TEST_CASE(test_stream_buffer_send_receive)
{
    StreamBufferHandle_t sb = xStreamBufferCreate(64, 1);
    TEST_ASSERT_NOT_NULL(sb);

    const char *msg = "hello stream";
    size_t sent = xStreamBufferSend(sb, msg, strlen(msg), 0);
    TEST_ASSERT_EQUAL_UINT(strlen(msg), sent);

    char recv_buf[64];
    memset(recv_buf, 0, sizeof(recv_buf));
    size_t received = xStreamBufferReceive(sb, recv_buf, sizeof(recv_buf), 0);
    TEST_ASSERT_EQUAL_UINT(strlen(msg), received);
    TEST_ASSERT_EQUAL_STRING(msg, recv_buf);

    vStreamBufferDelete(sb);
}

TEST_CASE(test_stream_buffer_empty_check)
{
    StreamBufferHandle_t sb = xStreamBufferCreate(32, 1);
    TEST_ASSERT_NOT_NULL(sb);

    TEST_ASSERT_TRUE(xStreamBufferIsEmpty(sb));

    uint8_t data = 0xAB;
    xStreamBufferSend(sb, &data, 1, 0);
    TEST_ASSERT_FALSE(xStreamBufferIsEmpty(sb));

    uint8_t out;
    xStreamBufferReceive(sb, &out, 1, 0);
    TEST_ASSERT_EQUAL_INT(0xAB, (int)out);
    TEST_ASSERT_TRUE(xStreamBufferIsEmpty(sb));

    vStreamBufferDelete(sb);
}

/* ── Software timer tests ── */

static volatile uint32_t timer_callback_count = 0;

static void test_timer_callback(TimerHandle_t xTimer)
{
    (void)xTimer;
    timer_callback_count++;
}

TEST_CASE(test_sw_timer_create_start)
{
    timer_callback_count = 0;

    TimerHandle_t timer = xTimerCreate(
        "test_tmr",
        pdMS_TO_TICKS(50),
        pdFALSE,  /* one-shot */
        NULL,
        test_timer_callback
    );
    TEST_ASSERT_NOT_NULL(timer);

    BaseType_t ret = xTimerStart(timer, pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(pdPASS, ret);

    /* Wait for timer to fire */
    vTaskDelay(pdMS_TO_TICKS(150));

    TEST_ASSERT(timer_callback_count >= 1);

    xTimerDelete(timer, pdMS_TO_TICKS(100));
}

TEST_CASE(test_sw_timer_periodic)
{
    timer_callback_count = 0;

    TimerHandle_t timer = xTimerCreate(
        "test_per",
        pdMS_TO_TICKS(50),
        pdTRUE,  /* periodic */
        NULL,
        test_timer_callback
    );
    TEST_ASSERT_NOT_NULL(timer);

    BaseType_t ret = xTimerStart(timer, pdMS_TO_TICKS(100));
    TEST_ASSERT_EQUAL_INT(pdPASS, ret);

    /* Wait for at least 3 ticks */
    vTaskDelay(pdMS_TO_TICKS(200));

    xTimerStop(timer, pdMS_TO_TICKS(100));

    /* Should have fired multiple times */
    TEST_ASSERT(timer_callback_count >= 2);

    xTimerDelete(timer, pdMS_TO_TICKS(100));
}

void test_rtos_adv_run(void)
{
    printf("Running FreeRTOS advanced tests\n");

    RUN_TEST(test_mutex_create_lock_unlock);
    RUN_TEST(test_mutex_recursive);
    RUN_TEST(test_event_group_create_set_wait);
    RUN_TEST(test_event_group_clear_bits);
    RUN_TEST(test_stream_buffer_send_receive);
    RUN_TEST(test_stream_buffer_empty_check);
    RUN_TEST(test_sw_timer_create_start);
    RUN_TEST(test_sw_timer_periodic);
}
