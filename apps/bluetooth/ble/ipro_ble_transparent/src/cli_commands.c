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
 * @file cli_commands.c
 * @brief Shell debug commands for BLE transparent module
 */

#include "ble_transparent.h"
#include "uart_bridge.h"
#include "at_command.h"
#include "app_config.h"
#include "ipro_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <generated/autoconf.h>

#ifdef CONFIG_SHELL
#include "shell.h"

static void cmd_trans(char *buf, int len, int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: trans <status|at|mode|send>\n");
        return;
    }

    if (strcmp(argv[1], "status") == 0) {
        const char *state_str[] = { "IDLE", "ADVERTISING", "CONNECTED" };
        const char *mode_str[] = { "COMMAND", "DATA" };

        printf("\n=== BLE Transparent Module ===\n");
        printf("BLE State:   %s\n", state_str[ble_transparent_get_state()]);
        printf("BLE Name:    %s\n", ble_transparent_get_name());
        printf("MTU:         %u\n", ble_transparent_get_mtu());
        printf("Bridge Mode: %s\n", mode_str[uart_bridge_get_mode()]);
        printf("Transport:   %s\n", uart_bridge_get_transport_name());
        printf("UART Baud:   %lu\n", (unsigned long)uart_bridge_get_baudrate());
        printf("Version:     %s\n", APP_VERSION);
        printf("==============================\n\n");

    } else if (strcmp(argv[1], "at") == 0) {
        if (argc < 3) {
            printf("Usage: trans at <command>\n");
            return;
        }
        /* Build the AT command string from remaining args */
        at_command_process(argv[2]);

    } else if (strcmp(argv[1], "mode") == 0) {
        if (argc < 3) {
            printf("Mode: %s\n",
                   uart_bridge_get_mode() == BRIDGE_MODE_DATA ? "DATA" : "COMMAND");
            return;
        }
        int val = atoi(argv[2]);
        uart_bridge_set_mode(val ? BRIDGE_MODE_DATA : BRIDGE_MODE_COMMAND);
        printf("Mode: %s\n", val ? "DATA" : "COMMAND");

    } else if (strcmp(argv[1], "send") == 0) {
        if (argc < 3) {
            printf("Usage: trans send <data>\n");
            return;
        }
        int ret = ble_transparent_send((const uint8_t *)argv[2], strlen(argv[2]));
        printf("Send %s (%d bytes)\n", ret == 0 ? "OK" : "FAILED", (int)strlen(argv[2]));

    } else {
        printf("Unknown subcommand: %s\n", argv[1]);
        printf("Usage: trans <status|at|mode|send>\n");
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_trans, trans, BLE Transparent Module commands);

#endif /* CONFIG_SHELL */

/* Dummy function so main.c can call trans_cli_init() without #ifdef */
void trans_cli_init(void)
{
    /* Shell commands registered via SHELL_CMD_EXPORT_ALIAS at link time */
}
