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
 * @file throughput_cli.c
 * @brief Throughput Test CLI/Shell Commands Implementation
 */

#include "throughput_app.h"
#include "ipro_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <generated/autoconf.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#endif

#undef STRINGIFY
#undef likely
#undef unlikely
#include <zephyr/bluetooth/conn.h>

#define LOG_TAG "TP_CLI"

/*
 * COMMAND HANDLERS (shared logic)
 ****************************************************************************************
 */

static int cmd_tp_help_handler(int argc, char **argv)
{
    (void)argc; (void)argv;

    printf("\n");
    printf("=== IPRO BLE Throughput Test Commands ===\n");
    printf("\n");
    printf("  tp slave              - Start as peripheral (slave/advertiser)\n");
    printf("  tp master [addr]      - Start as central (master/scanner)\n");
    printf("                          addr format: XX:XX:XX:XX:XX:XX\n");
    printf("  tp start [size] [sec] [interval_ms] - Start throughput test\n");
    printf("                          size: packet size (20-244, default 244)\n");
    printf("                          sec: duration in seconds (0=infinite, default 10)\n");
    printf("                          interval_ms: TX interval (0=flood, 1-1000ms, default 0)\n");
    printf("  tp stop               - Stop current test or disconnect\n");
    printf("  tp stats              - Show current statistics\n");
    printf("  tp status             - Show connection status\n");
    printf("  tp conn <min> <max>   - Update connection interval (units of 1.25ms)\n");
    printf("                          Example: tp conn 6 6 (7.5ms)\n");
    printf("  tp interval <ms>      - Set TX interval during test (1-1000ms)\n");
    printf("  tp phy <1|2|3>        - Set PHY (1=1M, 2=2M, 3=Coded)\n");
    printf("  tp help               - Show this help\n");
    printf("\n");
    printf("=== Quick Start Guide ===\n");
    printf("1. On Slave board:  tp slave\n");
    printf("2. On Master board: tp master\n");
    printf("3. Wait for connection...\n");
    printf("4. On either board: tp start 200 10\n");
    printf("5. View results:    tp stats\n");
    printf("\n");
    return 0;
}

static int cmd_tp_slave_handler(int argc, char **argv)
{
    (void)argc; (void)argv;

    printf("Starting as slave (peripheral)...\n");

    int ret = throughput_app_start_slave();
    if (ret != 0) {
        printf("ERROR: Failed to start slave mode\n");
    }
    return 0;
}

static int cmd_tp_master_handler(int argc, char **argv)
{
    const char *target_addr = NULL;

    if (argc > 1) {
        target_addr = argv[1];
        printf("Starting as master (central) - target: %s\n", target_addr);
    } else {
        printf("Starting as master (central) - will connect to first IPRO-TP device found\n");
    }

    int ret = throughput_app_start_master(target_addr);
    if (ret != 0) {
        printf("ERROR: Failed to start master mode\n");
    }
    return 0;
}

static int cmd_tp_start_handler(int argc, char **argv)
{
    throughput_test_config_t config = {
        .packet_size = 244,
        .duration_sec = 10,
        .direction = THROUGHPUT_DIR_TX,
        .use_notification = true,
        .interval_ms = 0  /* 0 = flood mode (max throughput) */
    };

    if (argc > 1) {
        config.packet_size = atoi(argv[1]);
        if (config.packet_size < 20) config.packet_size = 20;
        if (config.packet_size > 244) config.packet_size = 244;
    }

    if (argc > 2) {
        config.duration_sec = atoi(argv[2]);
    }

    if (argc > 3) {
        config.interval_ms = atoi(argv[3]);
        if (config.interval_ms < 1) config.interval_ms = 1;
    }

    printf("Starting throughput test: packet_size=%u, duration=%lus, interval=%lums\n",
           (unsigned)config.packet_size, (unsigned long)config.duration_sec,
           (unsigned long)config.interval_ms);

    int ret = throughput_app_start_test(&config);
    if (ret != 0) {
        printf("ERROR: Failed to start test (not connected or not ready)\n");
    }
    return 0;
}

static int cmd_tp_stop_handler(int argc, char **argv)
{
    (void)argc; (void)argv;

    throughput_app_ctx_t *ctx = throughput_app_get_ctx();

    if (ctx->test_running) {
        printf("Stopping throughput test...\n");
        throughput_app_stop_test();
    } else {
        printf("Stopping and disconnecting...\n");
        throughput_app_stop();
    }
    return 0;
}

static int cmd_tp_stats_handler(int argc, char **argv)
{
    (void)argc; (void)argv;

    throughput_app_print_stats();
    return 0;
}

static int cmd_tp_status_handler(int argc, char **argv)
{
    (void)argc; (void)argv;

    throughput_app_ctx_t *ctx = throughput_app_get_ctx();

    printf("\n");
    printf("=== Throughput Test Status ===\n");

    const char *role_str[] = {"Idle", "Slave", "Master"};
    const char *state_str[] = {"Idle", "Advertising", "Scanning", "Connecting",
                               "Connected", "Discovering", "Ready", "Testing"};

    printf("Role: %s\n", (ctx->role < 3) ? role_str[ctx->role] : "Unknown");
    printf("State: %s\n", (ctx->state < 8) ? state_str[ctx->state] : "Unknown");
    printf("Connected: %s\n", ctx->conn != NULL ? "Yes" : "No");
    if (ctx->conn != NULL) {
        printf("Connection Index: %d\n", bt_conn_index(ctx->conn));
    }
    printf("Connection Interval: %.2f ms\n", ctx->conn_interval * 1.25f);
    printf("PHY: %dM\n", ctx->phy == 2 ? 2 : 1);
    printf("Test Running: %s\n", ctx->test_running ? "Yes" : "No");
    printf("==============================\n");
    return 0;
}

static int cmd_tp_conn_handler(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: tp conn <min> <max>\n");
        printf("  min/max: connection interval in units of 1.25ms\n");
        printf("  Example: tp conn 6 6 (7.5ms interval)\n");
        printf("  Example: tp conn 24 24 (30ms interval)\n");
        return -1;
    }

    uint16_t interval_min = atoi(argv[1]);
    uint16_t interval_max = atoi(argv[2]);

    printf("Updating connection interval to %d-%d (%.2f-%.2f ms)\n",
           interval_min, interval_max,
           interval_min * 1.25f, interval_max * 1.25f);

    int ret = throughput_app_update_conn_params(interval_min, interval_max, 0, 100);
    if (ret != 0) {
        printf("ERROR: Failed to update connection parameters\n");
    }
    return 0;
}

static int cmd_tp_interval_handler(int argc, char **argv)
{
    if (argc < 2) {
        throughput_app_ctx_t *ctx = throughput_app_get_ctx();
        printf("Current TX interval: %lu ms\n", (unsigned long)ctx->test_config.interval_ms);
        printf("Usage: tp interval <ms>\n");
        printf("  ms: TX interval in milliseconds (1-1000)\n");
        printf("  Example: tp interval 10 (100 packets/second)\n");
        printf("  Example: tp interval 5  (200 packets/second)\n");
        return 0;
    }

    uint32_t interval_ms = atoi(argv[1]);

    printf("Setting TX interval to %lu ms\n", (unsigned long)interval_ms);

    int ret = throughput_app_set_tx_interval(interval_ms);
    if (ret != 0) {
        printf("ERROR: Failed to set TX interval\n");
    }
    return 0;
}

static int cmd_tp_phy_handler(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: tp phy <1|2|3>\n");
        printf("  1: 1M PHY\n");
        printf("  2: 2M PHY (higher throughput)\n");
        printf("  3: Coded PHY (longer range)\n");
        return -1;
    }

    uint8_t phy = atoi(argv[1]);

    printf("Setting PHY to %d\n", phy);

    int ret = throughput_app_set_phy(phy);
    if (ret != 0) {
        printf("ERROR: Failed to set PHY\n");
    }
    return 0;
}

/*
 * MAIN COMMAND HANDLER (shared logic)
 ****************************************************************************************
 */

static int cmd_tp_handler(int argc, char **argv)
{
    if (argc < 2) {
        return cmd_tp_help_handler(argc, argv);
    }

    const char *subcmd = argv[1];

    /* Shift arguments for subcommand */
    argc--;
    argv++;

    if (strcmp(subcmd, "help") == 0 || strcmp(subcmd, "?") == 0) {
        return cmd_tp_help_handler(argc, argv);
    } else if (strcmp(subcmd, "slave") == 0) {
        return cmd_tp_slave_handler(argc, argv);
    } else if (strcmp(subcmd, "master") == 0) {
        return cmd_tp_master_handler(argc, argv);
    } else if (strcmp(subcmd, "start") == 0) {
        return cmd_tp_start_handler(argc, argv);
    } else if (strcmp(subcmd, "stop") == 0) {
        return cmd_tp_stop_handler(argc, argv);
    } else if (strcmp(subcmd, "stats") == 0) {
        return cmd_tp_stats_handler(argc, argv);
    } else if (strcmp(subcmd, "status") == 0) {
        return cmd_tp_status_handler(argc, argv);
    } else if (strcmp(subcmd, "conn") == 0) {
        return cmd_tp_conn_handler(argc, argv);
    } else if (strcmp(subcmd, "interval") == 0) {
        return cmd_tp_interval_handler(argc, argv);
    } else if (strcmp(subcmd, "phy") == 0) {
        return cmd_tp_phy_handler(argc, argv);
    } else {
        printf("Unknown subcommand: %s\n", subcmd);
        printf("Use 'tp help' for available commands\n");
        return -1;
    }
}

/*
 * SHELL COMMAND REGISTRATION
 ****************************************************************************************
 */

#ifdef CONFIG_SHELL
static int cmd_tp(int argc, char **argv)
{
    return cmd_tp_handler(argc, argv);
}
SHELL_CMD_EXPORT_ALIAS(cmd_tp, tp, BLE Throughput test commands);
#endif
