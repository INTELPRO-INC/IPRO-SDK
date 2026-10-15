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
 * @file ble_demo.c
 * @brief Basic BLE demonstration for ipro7_demo
 *
 * Brings up the IPRO BLE controller and the Bluetooth host, and exports the
 * CLI commands. The peripheral / central logic is in ble_demo_app.c.
 *
 * CLI Commands:
 * - ble_peripheral: Start as BLE peripheral (advertising as "IPRO7_DEMO")
 * - ble_central: Scan, and auto-connect to an "IPRO7_DEMO" peripheral
 * - ble_list: List the devices found by the last scan
 * - ble_connect <n>: Connect to device <n> of ble_list
 * - ble_disconnect: Drop the current connection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <FreeRTOS.h>
#include <task.h>

#include <ipro_ble_controller.h>
#include <hci_driver_ipro_ble_ctlr.h>

#include "ble_demo.h"
#include "shell.h"
#include "ipro_log.h"

#define LOG_TAG "BLE_DEMO"

/*
 * EXTERNAL FUNCTIONS
 ****************************************************************************************
 */

extern int app_init(void);

/*
 * TYPES & DATA STRUCTURES
 ****************************************************************************************
 */

static struct {
    bool initialized;
} ble_demo_env;

/*
 * INITIALIZATION
 ****************************************************************************************
 */

/**
 * @brief Initialize BLE demo
 *
 * Starts the controller, attaches the host to it over the HCI driver and
 * enables the host. Blocks until the host is up, so call it from a task once
 * the scheduler is running.
 */
int ble_demo_init(void)
{
    int err;

    if (ble_demo_env.initialized) {
        IPRO_LOGW(LOG_TAG, "BLE demo already initialized", "");
        return 0;
    }

    IPRO_LOGI(LOG_TAG, "Initializing BLE demo...", "");

    // Controller first, then the HCI driver the host talks to it through
    err = ipro_ble_ctlr_controller_init(configMAX_PRIORITIES - 1);
    if (err) {
        IPRO_LOGE(LOG_TAG, "Controller init failed: %d", err);
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    err = hci_driver_ipro_ble_ctlr_init();
    if (err) {
        IPRO_LOGE(LOG_TAG, "HCI driver init failed: %d", err);
        return err;
    }
    vTaskDelay(pdMS_TO_TICKS(50));

    IPRO_LOGI(LOG_TAG, "BLE controller initialized successfully", "");

    // Enable the host and register the application callbacks
    err = app_init();
    if (err) {
        return err;
    }

    ble_demo_env.initialized = true;

    IPRO_LOGI(LOG_TAG, "BLE demo initialization complete", "");

    return 0;
}

/*
 * CLI COMMANDS
 ****************************************************************************************
 */

static int ble_peripheral_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return ble_demo_start_peripheral();
}
SHELL_CMD_EXPORT_ALIAS(ble_peripheral_cmd, ble_peripheral, Start BLE advertising as IPRO7_DEMO);

static int ble_central_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ble_central_scan();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(ble_central_cmd, ble_central, Scan for BLE devices and connect to IPRO7_DEMO);

static int ble_list_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    ble_central_list_devices();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(ble_list_cmd, ble_list, List devices found by the last scan);

static int ble_connect_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: ble_connect <index from ble_list>\r\n");
        return -1;
    }
    return ble_central_connect((uint8_t)strtoul(argv[1], NULL, 10));
}
SHELL_CMD_EXPORT_ALIAS(ble_connect_cmd, ble_connect, Connect to a scanned device by index);

static int ble_disconnect_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return ble_central_disconnect();
}
SHELL_CMD_EXPORT_ALIAS(ble_disconnect_cmd, ble_disconnect, Disconnect the current BLE link);
