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
 * @file cmd_scheduler_cli.c
 * @brief Command Scheduler CLI Commands
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_scheduler.h"
#include "shell.h"

/****************************************/
/*    PREDEFINED COMMAND LISTS          */
/****************************************/

/* Example: LP test sequence */
static const cmd_entry_t mm_ai_test_sequence[] = {
    { "sdcard", 2000 },   /* Sleep 3s, wait 5s after wakeup */
    { "trng_verify", 4000 },   /* Sleep 2s, wait 4s after wakeup */
    { "dsp_all", 3000 },   /* Sleep 1s, wait 3s after wakeup */
    CMD_LIST_END
};

/* Example: System info sequence */
static const cmd_entry_t sysinfo_sequence[] = {
    { "help",         2000 },
    { "free",         2000 },
    { "ps",           2000 },
    CMD_LIST_END
};

/****************************************/
/*    CLI COMMANDS                      */
/****************************************/

/**
 * @brief Run predefined command sequence
 *
 * Usage: cmd_seq <name> [initial_delay_ms]
 *   name: lp_test, sysinfo
 */
static int cmd_seq_func(int argc, char **argv)
{
    uint32_t delay = 1000;  /* Default 1s initial delay */
    const cmd_entry_t *seq = NULL;

    if (argc < 2) {
        printf("Usage: cmd_seq <name> [initial_delay_ms]\r\n");
        printf("  Available sequences:\r\n");
        printf("    mm_ai_test  - MM+AI test sequence\r\n");
        printf("    sysinfo  - System info commands\r\n");
        return -1;
    }

    if (strcmp(argv[1], "mm_ai_test") == 0) {
        seq = mm_ai_test_sequence;
    } else if (strcmp(argv[1], "sysinfo") == 0) {
        seq = sysinfo_sequence;
    } else {
        printf("Unknown sequence: %s\r\n", argv[1]);
        return -1;
    }

    if (argc >= 3) {
        delay = strtoul(argv[2], NULL, 10);
    }

    return cmd_scheduler_run(seq, delay);
}
SHELL_CMD_EXPORT_ALIAS(cmd_seq_func, cmd_seq, Run predefined command sequence);

/**
 * @brief Cancel running command sequence
 *
 * Usage: cmd_cancel
 */
static int cmd_cancel_func(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return cmd_scheduler_cancel();
}
SHELL_CMD_EXPORT_ALIAS(cmd_cancel_func, cmd_cancel, Cancel running command sequence);

/**
 * @brief Check scheduler status
 *
 * Usage: cmd_status
 */
static int cmd_status_func(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("Command scheduler: %s\r\n",
           cmd_scheduler_is_running() ? "RUNNING" : "IDLE");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_status_func, cmd_status, Check command scheduler status);
