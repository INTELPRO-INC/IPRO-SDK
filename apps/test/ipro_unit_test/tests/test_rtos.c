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
 * @file test_rtos.c
 * @brief Unit tests for FreeRTOS functionality
 */

#include <stdio.h>
#include "unity.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Test task handle */
static TaskHandle_t test_task_handle = NULL;
static volatile bool test_task_ran = false;

/* Test task function */
static void test_task_function(void *pvParameters)
{
    (void)pvParameters;
    test_task_ran = true;
    vTaskDelete(NULL);
}

/* Test cases */
TEST_CASE(test_task_create_delete)
{
    test_task_ran = false;
    
    BaseType_t ret = xTaskCreate(
        test_task_function,
        "test_task",
        1024,
        NULL,
        tskIDLE_PRIORITY + 1,
        &test_task_handle
    );
    
    TEST_ASSERT_EQUAL_INT(pdPASS, ret);
    TEST_ASSERT_NOT_NULL(test_task_handle);
    
    /* Give task time to run */
    vTaskDelay(pdMS_TO_TICKS(100));
    
    TEST_ASSERT_TRUE(test_task_ran);
}

TEST_CASE(test_semaphore_binary)
{
    SemaphoreHandle_t sem = xSemaphoreCreateBinary();
    TEST_ASSERT_NOT_NULL(sem);
    
    /* Should not be able to take before giving */
    BaseType_t ret = xSemaphoreTake(sem, 0);
    TEST_ASSERT_EQUAL_INT(pdFALSE, ret);
    
    /* Give semaphore */
    ret = xSemaphoreGive(sem);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    
    /* Now should be able to take */
    ret = xSemaphoreTake(sem, 0);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    
    vSemaphoreDelete(sem);
}

TEST_CASE(test_semaphore_counting)
{
    SemaphoreHandle_t sem = xSemaphoreCreateCounting(5, 0);
    TEST_ASSERT_NOT_NULL(sem);
    
    /* Give 3 times */
    for (int i = 0; i < 3; i++) {
        BaseType_t ret = xSemaphoreGive(sem);
        TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    }
    
    /* Take 3 times */
    for (int i = 0; i < 3; i++) {
        BaseType_t ret = xSemaphoreTake(sem, 0);
        TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    }
    
    /* Should fail now */
    BaseType_t ret = xSemaphoreTake(sem, 0);
    TEST_ASSERT_EQUAL_INT(pdFALSE, ret);
    
    vSemaphoreDelete(sem);
}

TEST_CASE(test_queue)
{
    QueueHandle_t queue = xQueueCreate(5, sizeof(uint32_t));
    TEST_ASSERT_NOT_NULL(queue);
    
    /* Send some values */
    uint32_t send_val = 0x12345678;
    BaseType_t ret = xQueueSend(queue, &send_val, 0);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    
    /* Receive and verify */
    uint32_t recv_val = 0;
    ret = xQueueReceive(queue, &recv_val, 0);
    TEST_ASSERT_EQUAL_INT(pdTRUE, ret);
    TEST_ASSERT_EQUAL_UINT(send_val, recv_val);
    
    vQueueDelete(queue);
}

TEST_CASE(test_delay)
{
    TickType_t start = xTaskGetTickCount();
    
    vTaskDelay(pdMS_TO_TICKS(100));
    
    TickType_t end = xTaskGetTickCount();
    TickType_t elapsed = end - start;
    
    /* Should be approximately 100ms (allow ±10ms tolerance) */
    TEST_ASSERT(elapsed >= pdMS_TO_TICKS(90));
    TEST_ASSERT(elapsed <= pdMS_TO_TICKS(110));
}

TEST_CASE(test_critical_section)
{
    /* Enter critical section */
    taskENTER_CRITICAL();
    
    /* Do some work (interrupts disabled) */
    volatile int count = 0;
    for (int i = 0; i < 1000; i++) {
        count++;
    }
    
    /* Exit critical section */
    taskEXIT_CRITICAL();
    
    TEST_ASSERT_EQUAL_INT(1000, count);
}

/* Test suite runner */
void test_rtos_run(void)
{
    printf("Running FreeRTOS tests\n");
    printf("FreeRTOS version: %s\n", tskKERNEL_VERSION_NUMBER);
    
    RUN_TEST(test_task_create_delete);
    RUN_TEST(test_semaphore_binary);
    RUN_TEST(test_semaphore_counting);
    RUN_TEST(test_queue);
    RUN_TEST(test_delay);
    RUN_TEST(test_critical_section);
}
