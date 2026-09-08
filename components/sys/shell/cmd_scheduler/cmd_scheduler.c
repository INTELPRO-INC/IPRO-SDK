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
 * @file cmd_scheduler.c
 * @brief Command Scheduler Implementation
 */

#include <stdio.h>
#include <string.h>
#include "cmd_scheduler.h"
#include "ipro_event.h"
#include "compiler/common.h"

/* Shell exec function (internal, not in header) */
extern int shell_exec(char *cmd, uint32_t length);

/****************************************/
/*    PRIVATE DATA                      */
/****************************************/

/* Current command list being executed */
static const cmd_entry_t *g_cmd_list = NULL;
static int g_cmd_index = 0;
static volatile int g_running = 0;

/* Command buffer for shell_exec (it modifies the buffer) */
#define CMD_BUF_SIZE 128
static char ATTR_PSRAM_SECTION g_cmd_buf[CMD_BUF_SIZE];

/****************************************/
/*    PRIVATE FUNCTIONS                 */
/****************************************/

/**
 * @brief Execute current command and schedule next
 */
static void cmd_scheduler_action(void *arg)
{
    (void)arg;

    if (!g_running || g_cmd_list == NULL) {
        g_running = 0;
        return;
    }

    const cmd_entry_t *entry = &g_cmd_list[g_cmd_index];

    /* Check for end of list */
    if (entry->cmd == NULL) {
        printf("[CMD_SCHED] Completed all commands\r\n");
        g_running = 0;
        g_cmd_list = NULL;
        g_cmd_index = 0;
        return;
    }

    /* Copy command to buffer (shell_exec modifies it) */
    size_t len = strlen(entry->cmd);
    if (len >= CMD_BUF_SIZE) {
        len = CMD_BUF_SIZE - 1;
    }
    memcpy(g_cmd_buf, entry->cmd, len);
    g_cmd_buf[len] = '\0';

    /* Execute command */
    printf("[CMD_SCHED] [%d] Executing: %s\r\n", g_cmd_index, g_cmd_buf);
    shell_exec(g_cmd_buf, len);

    /* Move to next command */
    g_cmd_index++;

    /* Schedule next command if there's a delay and more commands */
    if (g_cmd_list[g_cmd_index].cmd != NULL) {
        uint32_t delay = entry->delay_ms;
        if (delay > 0) {
            printf("[CMD_SCHED] Next command in %lu ms\r\n", (unsigned long)delay);
            ipro_event_post_delayed(delay, cmd_scheduler_action, NULL);
        } else {
            /* No delay, execute immediately */
            cmd_scheduler_action(NULL);
        }
    } else {
        /* No more commands */
        printf("[CMD_SCHED] Completed all commands\r\n");
        g_running = 0;
        g_cmd_list = NULL;
        g_cmd_index = 0;
    }
}

/****************************************/
/*    PUBLIC API                        */
/****************************************/

int cmd_scheduler_init(void)
{
    /* Create default event loop if not exists */
    if (ipro_event_loop_get_default() == NULL) {
        int ret = ipro_event_loop_create_default();
        if (ret != IPRO_OK) {
            printf("[CMD_SCHED] Failed to create event loop: %d\r\n", ret);
            return ret;
        }
    }

    printf("[CMD_SCHED] Initialized\r\n");
    return 0;
}

int cmd_scheduler_run(const cmd_entry_t *cmd_list, uint32_t initial_delay_ms)
{
    if (cmd_list == NULL) {
        return IPRO_ERR_INVALID_ARG;
    }

    if (g_running) {
        printf("[CMD_SCHED] Already running, cancel first\r\n");
        return IPRO_ERR_INVALID_STATE;
    }

    /* Count commands */
    int count = 0;
    while (cmd_list[count].cmd != NULL) {
        count++;
    }

    if (count == 0) {
        printf("[CMD_SCHED] Empty command list\r\n");
        return IPRO_ERR_INVALID_ARG;
    }

    printf("[CMD_SCHED] Scheduling %d commands, first in %lu ms\r\n",
           count, (unsigned long)initial_delay_ms);

    /* Store command list */
    g_cmd_list = cmd_list;
    g_cmd_index = 0;
    g_running = 1;

    /* Schedule first command */
    if (initial_delay_ms > 0) {
        return ipro_event_post_delayed(initial_delay_ms, cmd_scheduler_action, NULL);
    } else {
        /* Execute immediately via event loop */
        return ipro_event_schedule_call(cmd_scheduler_action, NULL);
    }
}

int cmd_scheduler_cancel(void)
{
    if (!g_running) {
        return 0;
    }

    printf("[CMD_SCHED] Cancelling scheduled commands\r\n");

    /* Cancel pending delayed action */
    ipro_event_cancel_delayed(cmd_scheduler_action, NULL);

    g_running = 0;
    g_cmd_list = NULL;
    g_cmd_index = 0;

    return 0;
}

int cmd_scheduler_is_running(void)
{
    return g_running;
}
