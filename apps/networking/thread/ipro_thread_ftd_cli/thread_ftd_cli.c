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
 * @file thread_ftd_cli.c
 * @brief IPRO Thread FTD CLI/Shell Commands Implementation
 *
 * This file implements CLI/Shell commands for Thread FTD application.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/dataset.h>
#include <openthread/cli.h>

#include "thread_ftd_app.h"
#include "ipro_log.h"
#include "openthread_port.h"

#define LOG_TAG "THREAD_CLI"

// OpenThread CLI buffer
#define OT_CLI_MAX_LINE_LENGTH 256
static char ot_cli_buffer[OT_CLI_MAX_LINE_LENGTH];

/*
 * Command handlers (shared logic)
 */

static int cmd_thread_start_handler(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (thread_ftd_app_start() == 0) {
        printf("Thread network started\r\n");
    } else {
        printf("Failed to start Thread network\r\n");
    }
    return 0;
}

static int cmd_thread_stop_handler(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (thread_ftd_app_stop() == 0) {
        printf("Thread network stopped\r\n");
    } else {
        printf("Failed to stop Thread network\r\n");
    }
    return 0;
}

static int cmd_thread_status_handler(int argc, char **argv)
{
    (void)argc; (void)argv;
    if (thread_ftd_app_is_active()) {
        printf("Thread network status: Active\r\n");
    } else {
        printf("Thread network status: Inactive\r\n");
    }
    return 0;
}

static int cmd_ot_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: ot <openthread_command>\r\n");
        printf("Examples:\r\n");
        printf("  ot state           - Show Thread state\r\n");
        printf("  ot channel         - Show/set channel\r\n");
        printf("  ot panid           - Show/set PAN ID\r\n");
        printf("  ot networkkey      - Show/set network key\r\n");
        printf("  ot ifconfig up     - Bring up network interface\r\n");
        printf("  ot thread start    - Start Thread protocol\r\n");
        printf("  ot dataset init new - Initialize new dataset\r\n");
        printf("  ot dataset commit active - Commit active dataset\r\n");
        printf("  ot help            - Show all OpenThread commands\r\n");
        return 0;
    }

    // Build OpenThread CLI command string
    int offset = 0;
    for (int i = 1; i < argc && offset < OT_CLI_MAX_LINE_LENGTH - 1; i++) {
        int arg_len = strlen(argv[i]);
        if (offset + arg_len + 1 >= OT_CLI_MAX_LINE_LENGTH) {
            break;
        }

        if (i > 1) {
            ot_cli_buffer[offset++] = ' ';
        }

        memcpy(&ot_cli_buffer[offset], argv[i], arg_len);
        offset += arg_len;
    }
    ot_cli_buffer[offset] = '\0';

    // Execute OpenThread CLI command.
    // MUST hold the OT lock because this runs in the shell task, NOT the OT task.
    // Without locking, there's a race: Discover() posts a tasklet that wakes the
    // OT task via xTaskNotifyGive. On a preemptive RTOS, the OT task can preempt
    // us and process the tasklet BEFORE we finish setting internal state (e.g.,
    // DiscoverScanner::mState), causing "error:Abort" on ot discover.
    otrLock();
    otCliInputLine(ot_cli_buffer);
    otrUnlock();
    return 0;
}

static int cmd_thread_info_handler(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("\r\n");
    printf("=================================================\r\n");
    printf("  Thread FTD Information\r\n");
    printf("=================================================\r\n");
    printf("  Device Type: Full Thread Device (FTD)\r\n");
    printf("  Status: %s\r\n", thread_ftd_app_is_active() ? "Active" : "Inactive");
    printf("=================================================\r\n");
    printf("\r\n");
    printf("Use 'ot state' to see Thread role\r\n");
    printf("Use 'ot' command to access full OpenThread CLI\r\n");
    printf("\r\n");
    return 0;
}

/*
 * Shell command registration
 */
#ifdef CONFIG_SHELL
static int cmd_thread_start(int argc, char **argv)
{
    return cmd_thread_start_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_thread_start, thread_start, Start Thread network);

static int cmd_thread_stop(int argc, char **argv)
{
    return cmd_thread_stop_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_thread_stop, thread_stop, Stop Thread network);

static int cmd_thread_status(int argc, char **argv)
{
    return cmd_thread_status_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_thread_status, thread_status, Show Thread network status);

static int cmd_thread_info(int argc, char **argv)
{
    return cmd_thread_info_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_thread_info, thread_info, Show Thread FTD information);

static int cmd_ot(int argc, char **argv)
{
    return cmd_ot_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_ot, ot, OpenThread CLI commands);
#endif
