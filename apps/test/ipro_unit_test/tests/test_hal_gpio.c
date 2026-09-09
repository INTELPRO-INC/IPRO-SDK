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
 * @file test_hal_gpio.c
 * @brief Unit tests for HAL GPIO functionality
 *
 * Requires loopback wiring: GPIO_PIN_4 (output) -> GPIO_PIN_5 (input).
 * Tests auto-skip if loopback is not detected.
 */

#include <stdio.h>
#include <stdbool.h>
#include "unity.h"
#include "hal_gpio.h"

#define TEST_GPIO_OUT  GPIO_PIN_4
#define TEST_GPIO_IN   GPIO_PIN_5

static bool hw_available = false;

TEST_CASE(test_gpio_loopback_detect)
{
    /* Configure output pin and input pin */
    gpio_set_mode(TEST_GPIO_OUT, GPIO_OUTPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_PP_MODE);

    /* Verify both HIGH and LOW pass through — prevents false positive
     * from internal pull-ups when pins aren't actually wired together */
    gpio_write(TEST_GPIO_OUT, 1);
    for (volatile int i = 0; i < 1000; i++) {}
    int val_high = gpio_read(TEST_GPIO_IN);

    gpio_write(TEST_GPIO_OUT, 0);
    for (volatile int i = 0; i < 1000; i++) {}
    int val_low = gpio_read(TEST_GPIO_IN);

    if (val_high == 1 && val_low == 0) {
        hw_available = true;
    }

    if (!hw_available) {
        TEST_IGNORE_MESSAGE("GPIO loopback not available (PIN4->PIN5)");
        return;
    }

    TEST_PASS();
}

TEST_CASE(test_gpio_output_high)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("GPIO loopback not available"); return; }

    gpio_set_mode(TEST_GPIO_OUT, GPIO_OUTPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_PP_MODE);

    gpio_write(TEST_GPIO_OUT, 1);
    for (volatile int i = 0; i < 1000; i++) {}

    TEST_ASSERT_EQUAL_INT(1, gpio_read(TEST_GPIO_IN));
}

TEST_CASE(test_gpio_output_low)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("GPIO loopback not available"); return; }

    gpio_set_mode(TEST_GPIO_OUT, GPIO_OUTPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_PP_MODE);

    gpio_write(TEST_GPIO_OUT, 0);
    for (volatile int i = 0; i < 1000; i++) {}

    TEST_ASSERT_EQUAL_INT(0, gpio_read(TEST_GPIO_IN));
}

TEST_CASE(test_gpio_loopback_verify)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("GPIO loopback not available"); return; }

    gpio_set_mode(TEST_GPIO_OUT, GPIO_OUTPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_PP_MODE);

    /* Toggle through HIGH -> LOW -> HIGH and verify each state */
    gpio_write(TEST_GPIO_OUT, 1);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(1, gpio_read(TEST_GPIO_IN));

    gpio_write(TEST_GPIO_OUT, 0);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(0, gpio_read(TEST_GPIO_IN));

    gpio_write(TEST_GPIO_OUT, 1);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(1, gpio_read(TEST_GPIO_IN));
}

TEST_CASE(test_gpio_toggle)
{
    if (!hw_available) { TEST_IGNORE_MESSAGE("GPIO loopback not available"); return; }

    gpio_set_mode(TEST_GPIO_OUT, GPIO_OUTPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_PP_MODE);

    /* Set known state LOW */
    gpio_write(TEST_GPIO_OUT, 0);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(0, gpio_read(TEST_GPIO_IN));

    /* Toggle should go HIGH */
    gpio_toggle(TEST_GPIO_OUT);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(1, gpio_read(TEST_GPIO_IN));

    /* Toggle again should go LOW */
    gpio_toggle(TEST_GPIO_OUT);
    for (volatile int i = 0; i < 1000; i++) {}
    TEST_ASSERT_EQUAL_INT(0, gpio_read(TEST_GPIO_IN));
}

/* Test suite runner */
void test_hal_gpio_run(void)
{
    printf("Running HAL GPIO tests\n");
    printf("Loopback: GPIO_PIN_4 (out) -> GPIO_PIN_5 (in)\n");

    RUN_TEST(test_gpio_loopback_detect);
    RUN_TEST(test_gpio_output_high);
    RUN_TEST(test_gpio_output_low);
    RUN_TEST(test_gpio_loopback_verify);
    RUN_TEST(test_gpio_toggle);

    /* Cleanup: set pins to input mode (high-Z) */
    gpio_set_mode(TEST_GPIO_OUT, GPIO_INPUT_MODE);
    gpio_set_mode(TEST_GPIO_IN, GPIO_INPUT_MODE);
}
