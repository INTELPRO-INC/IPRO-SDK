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
 * @file cmd_scheduler.h
 * @brief Command Scheduler - Execute shell commands with delays
 *
 * Provides functionality to schedule a list of shell commands
 * with configurable delays between each command execution.
 */

#ifndef CMD_SCHEDULER_H
#define CMD_SCHEDULER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Command entry structure
 *
 * Defines a single command with its delay to next command.
 */
typedef struct {
    const char *cmd;        /**< Command string (e.g., "lp_test 5000") */
    uint32_t delay_ms;      /**< Delay in ms before executing next command */
} cmd_entry_t;

/**
 * @brief Command list terminator
 *
 * Use this to mark the end of a command list.
 */
#define CMD_LIST_END  { NULL, 0 }

/**
 * @brief Initialize the command scheduler
 *
 * Must be called before using other scheduler functions.
 * Creates the default event loop if not already created.
 *
 * @return 0 on success, negative error code on failure
 */
int cmd_scheduler_init(void);

/**
 * @brief Schedule a list of commands for execution
 *
 * Commands are executed sequentially with specified delays.
 * The first command executes after initial_delay_ms.
 *
 * @param cmd_list       Array of commands, terminated with CMD_LIST_END
 * @param initial_delay_ms  Delay before first command (0 = immediate)
 *
 * @return 0 on success, negative error code on failure
 *
 * @example
 * static const cmd_entry_t my_commands[] = {
 *     { "lp_test 3000", 5000 },  // Run lp_test, wait 5s
 *     { "lp_test 2000", 3000 },  // Run lp_test, wait 3s
 *     { "mem_info",     0    },  // Run mem_info, no delay after
 *     CMD_LIST_END
 * };
 * cmd_scheduler_run(my_commands, 1000);  // Start after 1s
 */
int cmd_scheduler_run(const cmd_entry_t *cmd_list, uint32_t initial_delay_ms);

/**
 * @brief Cancel all pending scheduled commands
 *
 * @return 0 on success, negative error code on failure
 */
int cmd_scheduler_cancel(void);

/**
 * @brief Check if scheduler is currently running
 *
 * @return 1 if running, 0 if idle
 */
int cmd_scheduler_is_running(void);

#ifdef __cplusplus
}
#endif

#endif /* CMD_SCHEDULER_H */
