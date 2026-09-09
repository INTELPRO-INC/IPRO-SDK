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
 * @file main.c
 * @brief IPRO SDK Unit Test Application Main
 *
 * Self-contained unit testing framework for validating SDK components.
 * Supports both automated and interactive test execution via shell commands.
 */

#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

#include "test_runner.h"
#include "platform_common.h"
#include "hal_boot2.h"
#include "freertos_app_hooks.h"

#ifdef CONFIG_TEST_BLE
/* New stack: ipro_ble_controller with the Zephyr 4.4 host. ble_lib_api.h
 * still exists and still declares the controller entry point, but the entry
 * point itself is now ipro_ble_ctlr_controller_init(). */
#include <ble_lib_api.h>
#include <hci_driver_ipro_ble_ctlr.h>
/* ble_lib_api.h still declares the old ble_controller_init(); the new tree's
 * entry point is declared here instead. */
#include <ipro_ble_ctlr_arch.h>
#include <zephyr/bluetooth/bluetooth.h>
#endif

/* Define logging tag */
#define TAG "UNIT_TEST"

/* Task priorities */
#define MAIN_TASK_PRIORITY      (configMAX_PRIORITIES - 1)
#define TEST_RUNNER_PRIORITY    (configMAX_PRIORITIES - 2)

/* Task stack sizes */
#define MAIN_TASK_STACK_SIZE    (4096)
#define TEST_RUNNER_STACK_SIZE  (16384)

/* ========================================================================= */
/* Shell commands                                                            */
/* ========================================================================= */
#ifdef CONFIG_SHELL
static int cmd_run_tests(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    printf("Running all enabled tests...\n");
    return test_runner_execute_all();
}

static int cmd_list_tests(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    test_runner_list_tests();
    return 0;
}

static int cmd_run_suite(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: test_run <suite_name>\n");
        return -1;
    }
    return test_runner_execute_suite(argv[1]);
}

static int cmd_test_group(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: test_group <sw|hal|crypto|ble>\n");
        return -1;
    }
    return test_runner_execute_group(argv[1]);
}

static int cmd_test_status(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    test_runner_print_summary();
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_run_tests, test_all, Run all test suites);
SHELL_CMD_EXPORT_ALIAS(cmd_list_tests, test_list, List available test suites);
SHELL_CMD_EXPORT_ALIAS(cmd_run_suite, test_run, Run specific test suite);
SHELL_CMD_EXPORT_ALIAS(cmd_test_group, test_group, Run test group sw|hal|crypto|ble);
SHELL_CMD_EXPORT_ALIAS(cmd_test_status, test_status, Show test results summary);
#endif /* CONFIG_SHELL */

/**
 * @brief Test runner task
 */
static void test_runner_task(void *pvParameters)
{
    (void)pvParameters;

    printf("Test Runner Task Started\n");
    test_runner_init();

#ifdef CONFIG_TEST_AUTO_RUN
    printf("Auto-running tests (CONFIG_TEST_AUTO_RUN enabled)\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    int result = test_runner_execute_all();
    if (result == 0) {
        printf("All tests PASSED\n");
    } else {
        printf("Some tests FAILED (code: %d)\n", result);
    }

    test_runner_print_summary();
#else
    printf("Interactive mode: Use shell commands to run tests\n");
    printf("  test_list            List available tests\n");
    printf("  test_all             Run all tests\n");
    printf("  test_group <name>    Run group (sw|hal|crypto|ble)\n");
    printf("  test_run <suite>     Run specific test suite\n");
    printf("  test_status          Show results summary\n");
#endif

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/**
 * @brief Main task
 */
static void main_task(void *pvParameters)
{
    (void)pvParameters;

    printf("===========================================\n");
    printf("  IPRO SDK Unit Test Application\n");
    printf("  FreeRTOS %s\n", tskKERNEL_VERSION_NUMBER);
    printf("===========================================\n");

#ifdef CONFIG_SHELL
    shell_init_with_task(UART0_INDEX);
    printf("Shell initialized for interactive testing\n");
#endif

#ifdef CONFIG_TEST_BLE
    /* Controller first, then the HCI driver; test_ble_core.c calls bt_enable
     * itself. Both need a moment to bring their tasks up before the next step
     * touches them - the same sequencing ipro_ble_remote uses. */
    printf("Initializing BLE controller...\n");
    int ble_err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 3);
    if (ble_err) {
        printf("BLE controller init failed: %d\n", ble_err);
    } else {
        vTaskDelay(pdMS_TO_TICKS(50));
        ble_err = hci_driver_ipro_ble_ctlr_init();
        if (ble_err) {
            printf("HCI driver init failed: %d\n", ble_err);
        } else {
            vTaskDelay(pdMS_TO_TICKS(50));
            printf("BLE controller initialized\n");
        }
    }
#endif

    BaseType_t ret = xTaskCreate(
        test_runner_task,
        "test_runner",
        TEST_RUNNER_STACK_SIZE,
        NULL,
        TEST_RUNNER_PRIORITY,
        NULL
    );

    if (ret != pdPASS) {
        printf("ERROR: Failed to create test runner task\n");
    }

    printf("Main task initialization complete\n");
    vTaskDelete(NULL);
}

/**
 * @brief Application entry point
 */
int main(void)
{
    ipro_setup_heap();
    platform_init(0);
    hal_boot2_init();

    BaseType_t ret = xTaskCreate(
        main_task,
        "main",
        MAIN_TASK_STACK_SIZE,
        NULL,
        MAIN_TASK_PRIORITY,
        NULL
    );

    if (ret != pdPASS) {
        printf("Failed to create main task\n");
        return -1;
    }

    vTaskStartScheduler();

    while (1) {
        ;
    }

    return 0;
}
