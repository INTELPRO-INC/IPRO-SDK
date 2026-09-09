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
 * @file cli.h
 * @brief CLI compatibility header for ipro6_rf
 *
 * This header provides compatibility with legacy CLI code by defining
 * the minimal structures and macros needed by cli_app.c
 */

#ifndef _CLI_COMPAT_H_
#define _CLI_COMPAT_H_

#include <stdint.h>

/* CLI command structure (legacy compatibility) */
struct cli_command {
    const char *name;
    const char *help;
    void (*function)(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv);
};

/*
 * STATIC_CLI_CMD_ATTRIBUTE - legacy attribute for static CLI commands
 * When using shell system, this just places commands in a named section
 * that won't be used (shell uses FSymTab instead)
 */
#define STATIC_CLI_CMD_ATTRIBUTE __attribute__((used, section(".cli_cmds")))

/* Legacy CLI registration - no-op when using shell */
static inline int aos_cli_register_commands(const struct cli_command *cmds, int num)
{
    (void)cmds;
    (void)num;
    return 0;
}

#endif /* _CLI_COMPAT_H_ */
