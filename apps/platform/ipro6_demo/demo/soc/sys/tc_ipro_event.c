/*
 * SPDX-FileCopyrightText: 2025 IPRO Technology Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO Event Loop Test Cases
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/autoconf.h>

#ifdef CONFIG_IPRO_EVENT

#include "ipro_event.h"
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"

/* Define test event base */
IPRO_EVENT_DEFINE_BASE(TEST_EVENT);

/* Test event IDs */
enum {
    TEST_EVENT_HELLO = 0,
    TEST_EVENT_DATA,
    TEST_EVENT_TIMER_TICK,
};

/* Test data structure */
typedef struct {
    int value;
    char message[32];
} test_event_data_t;

/* Timer handle for periodic test */
static ipro_event_timer_t s_test_timer = NULL;
static int s_timer_count = 0;

/* Event handler for TEST_EVENT_HELLO */
static void test_hello_handler(const ipro_event_t *event, void *user_data)
{
    printf("[EVENT] Hello event received! user_data=%p\r\n", user_data);
}

/* Event handler for TEST_EVENT_DATA */
static void test_data_handler(const ipro_event_t *event, void *user_data)
{
    if (event->event_data && event->event_data_size >= sizeof(test_event_data_t)) {
        test_event_data_t *data = (test_event_data_t *)event->event_data;
        printf("[EVENT] Data event: value=%d, message='%s'\r\n",
               data->value, data->message);
    } else {
        printf("[EVENT] Data event with no data\r\n");
    }
}

/* Timer callback */
static void test_timer_callback(void *arg)
{
    s_timer_count++;
    printf("[TIMER] Tick #%d (arg=%p)\r\n", s_timer_count, arg);

    /* Stop after 5 ticks */
    if (s_timer_count >= 5 && s_test_timer) {
        printf("[TIMER] Auto-stopping after 5 ticks\r\n");
        ipro_event_timer_stop(s_test_timer);
    }
}

/* Delayed action callback */
static void test_delayed_action(void *arg)
{
    uint32_t delay_ms = (uint32_t)(uintptr_t)arg;
    printf("[DELAYED] Action executed after %lu ms delay\r\n", (unsigned long)delay_ms);
}

/* Shell command: event_init - Initialize event loop */
static int cmd_event_init(int argc, char **argv)
{
    int ret = ipro_event_loop_create_default();
    if (ret == IPRO_OK) {
        printf("Event loop created successfully\r\n");

        /* Register handlers */
        ret = ipro_event_handler_register(TEST_EVENT, TEST_EVENT_HELLO,
                                          test_hello_handler, (void *)0x1234);
        if (ret == IPRO_OK) {
            printf("Registered HELLO handler\r\n");
        }

        ret = ipro_event_handler_register(TEST_EVENT, TEST_EVENT_DATA,
                                          test_data_handler, NULL);
        if (ret == IPRO_OK) {
            printf("Registered DATA handler\r\n");
        }
    } else if (ret == IPRO_ERR_INVALID_STATE) {
        printf("Event loop already exists\r\n");
    } else {
        printf("Failed to create event loop: %d\r\n", ret);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_init, event_init, "Initialize IPRO event loop and register handlers");

/* Shell command: event_post - Post a hello event */
static int cmd_event_post(int argc, char **argv)
{
    int ret = ipro_event_post(TEST_EVENT, TEST_EVENT_HELLO, NULL, 0, 100);
    if (ret == IPRO_OK) {
        printf("Posted HELLO event\r\n");
    } else {
        printf("Failed to post event: %d\r\n", ret);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_post, event_post, "Post a hello event to the event loop");

/* Shell command: event_data - Post an event with data */
static int cmd_event_data(int argc, char **argv)
{
    test_event_data_t data = {
        .value = 42,
    };

    if (argc > 1) {
        data.value = atoi(argv[1]);
    }
    if (argc > 2) {
        strncpy(data.message, argv[2], sizeof(data.message) - 1);
        data.message[sizeof(data.message) - 1] = '\0';
    } else {
        strcpy(data.message, "Hello IPRO!");
    }

    int ret = ipro_event_post(TEST_EVENT, TEST_EVENT_DATA,
                              &data, sizeof(data), 100);
    if (ret == IPRO_OK) {
        printf("Posted DATA event: value=%d, message='%s'\r\n",
               data.value, data.message);
    } else {
        printf("Failed to post event: %d\r\n", ret);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_data, event_data, "Post data event [value] [message]");

/* Shell command: event_delay - Post a delayed action */
static int cmd_event_delay(int argc, char **argv)
{
    uint32_t delay_ms = 1000;
    if (argc > 1) {
        delay_ms = atoi(argv[1]);
    }

    int ret = ipro_event_post_delayed(delay_ms, test_delayed_action,
                                       (void *)(uintptr_t)delay_ms);
    if (ret == IPRO_OK) {
        printf("Scheduled delayed action in %lu ms\r\n", (unsigned long)delay_ms);
    } else {
        printf("Failed to schedule delayed action: %d\r\n", ret);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_delay, event_delay, "Schedule delayed action [delay_ms]");

/* Shell command: event_timer - Start/stop periodic timer */
static int cmd_event_timer(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: event_timer <start|stop> [period_ms]\r\n");
        return 0;
    }

    if (strcmp(argv[1], "start") == 0) {
        if (s_test_timer) {
            printf("Timer already exists, stopping first\r\n");
            ipro_event_timer_stop(s_test_timer);
            ipro_event_timer_delete(s_test_timer);
            s_test_timer = NULL;
        }

        uint32_t period_ms = 1000;
        if (argc > 2) {
            period_ms = atoi(argv[2]);
        }

        s_timer_count = 0;
        int ret = ipro_event_timer_create(period_ms, test_timer_callback,
                                           (void *)0xCAFE, &s_test_timer);
        if (ret == IPRO_OK) {
            ret = ipro_event_timer_start(s_test_timer);
            if (ret == IPRO_OK) {
                printf("Timer started with period %lu ms\r\n", (unsigned long)period_ms);
            } else {
                printf("Failed to start timer: %d\r\n", ret);
            }
        } else {
            printf("Failed to create timer: %d\r\n", ret);
        }
    } else if (strcmp(argv[1], "stop") == 0) {
        if (s_test_timer) {
            ipro_event_timer_stop(s_test_timer);
            ipro_event_timer_delete(s_test_timer);
            s_test_timer = NULL;
            printf("Timer stopped and deleted\r\n");
        } else {
            printf("No timer running\r\n");
        }
    } else {
        printf("Unknown command: %s\r\n", argv[1]);
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_timer, event_timer, "Control periodic timer: start [period_ms] | stop");

/* Shell command: event_test - Run full test sequence */
static int cmd_event_test(int argc, char **argv)
{
    printf("\r\n=== IPRO Event Loop Test ===\r\n\r\n");

    /* Step 1: Initialize */
    printf("Step 1: Initialize event loop\r\n");
    cmd_event_init(0, NULL);

    /* Step 2: Post hello event */
    printf("\r\nStep 2: Post hello event\r\n");
    cmd_event_post(0, NULL);
    vTaskDelay(pdMS_TO_TICKS(100)); /* Wait for event processing */

    /* Step 3: Post data event */
    printf("\r\nStep 3: Post data event\r\n");
    char *data_args[] = {"event_data", "123", "TestMessage"};
    cmd_event_data(3, data_args);
    vTaskDelay(pdMS_TO_TICKS(100));

    /* Step 4: Schedule delayed action */
    printf("\r\nStep 4: Schedule delayed action (500ms)\r\n");
    char *delay_args[] = {"event_delay", "500"};
    cmd_event_delay(2, delay_args);
    printf("Waiting for delayed action...\r\n");
    vTaskDelay(pdMS_TO_TICKS(700));

    /* Step 5: Start timer for 3 seconds */
    printf("\r\nStep 5: Start timer (500ms period, will auto-stop after 5 ticks)\r\n");
    char *timer_args[] = {"event_timer", "start", "500"};
    cmd_event_timer(3, timer_args);

    printf("\r\n=== Test sequence initiated ===\r\n");
    printf("Timer will auto-stop after 5 ticks (~2.5s)\r\n\r\n");

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_event_test, event_test, "Run complete IPRO event loop test sequence");

#endif /* CONFIG_IPRO_EVENT */
