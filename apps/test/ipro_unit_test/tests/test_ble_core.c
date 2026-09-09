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
 * @file test_ble_core.c
 * @brief Unit tests for BLE stack core functionality
 */

#include <stdio.h>
#include <string.h>
#include "unity.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <hci_driver_ipro_ble_ctlr.h>

#define TAG "TEST_BLE_CORE"

static bool ble_ready = false;

/* Test cases */
TEST_CASE(test_ble_enable)
{
    /* main.c has already brought up the controller and registered the HCI
     * driver - on this stack those are ipro_ble_ctlr_controller_init() and
     * hci_driver_ipro_ble_ctlr_init(), and both must run before bt_enable().
     * Calling them again here would double-initialise. */
    int ret = bt_enable(NULL);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ble_ready = true;
}

TEST_CASE(test_ble_device_name)
{
    if (!ble_ready) {
        TEST_IGNORE_MESSAGE("BLE not ready");
    }

    int ret = bt_set_name("IPRO_TEST");
    TEST_ASSERT_EQUAL_INT(0, ret);

    const char *name = bt_get_name();
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("IPRO_TEST", name);
}

TEST_CASE(test_ble_name_change)
{
    if (!ble_ready) {
        TEST_IGNORE_MESSAGE("BLE not ready");
    }

    int ret;

    ret = bt_set_name("NAME_FIRST");
    TEST_ASSERT_EQUAL_INT(0, ret);

    const char *name = bt_get_name();
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("NAME_FIRST", name);

    /* Change the name */
    ret = bt_set_name("NAME_SECOND");
    TEST_ASSERT_EQUAL_INT(0, ret);

    name = bt_get_name();
    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("NAME_SECOND", name);
}

/* Test suite runner */
void test_ble_run(void)
{
    printf("Running BLE core tests\n");

    /* bt_enable must be first so ble_ready is set for subsequent tests */
    RUN_TEST(test_ble_enable);
    RUN_TEST(test_ble_device_name);
    RUN_TEST(test_ble_name_change);
}
