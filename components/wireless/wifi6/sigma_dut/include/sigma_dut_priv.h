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

#ifndef __SIGMA_DUT_PRIV_H__
#define __SIGMA_DUT_PRIV_H__

#include <stdint.h>
#include <stdarg.h>

#define SIGMA_CMD_MAX_LEN       512
#define SIGMA_RESP_MAX_LEN      1024
#define SIGMA_MAX_PARAMS        32
#define SIGMA_CMD_NAME_LEN      64
#define SIGMA_PARAM_KEY_LEN     64
#define SIGMA_PARAM_VAL_LEN     128

/* Parsed CAPI command */
struct sigma_cmd {
    char name[SIGMA_CMD_NAME_LEN];
    int param_count;
    struct {
        char key[SIGMA_PARAM_KEY_LEN];
        char val[SIGMA_PARAM_VAL_LEN];
    } params[SIGMA_MAX_PARAMS];
};

/* Connection context for a single CAPI session */
struct sigma_conn {
    int sockfd;
    char tx_buf[SIGMA_RESP_MAX_LEN];
};

/* Command handler function signature */
typedef int (*sigma_cmd_handler_t)(struct sigma_conn *conn,
                                   const struct sigma_cmd *cmd);

/* Command dispatch table entry */
struct sigma_cmd_entry {
    const char *name;
    sigma_cmd_handler_t handler;
};

/* --- Parser (sigma_dut_parser.c) --- */

/**
 * Parse a CAPI command line into a sigma_cmd structure.
 *
 * CAPI format: "command_name,key1,val1,key2,val2\n"
 *
 * @param line  null-terminated input line (may contain trailing \r\n)
 * @param cmd   output parsed command
 * @return 0 on success, -1 on parse error
 */
int sigma_parse_cmd(const char *line, struct sigma_cmd *cmd);

/**
 * Look up a parameter value by key (case-insensitive).
 *
 * @return parameter value string, or NULL if not found
 */
const char *sigma_cmd_get_param(const struct sigma_cmd *cmd, const char *key);

/* --- Response helpers (sigma_dut_server.c) --- */

int sigma_resp_complete(struct sigma_conn *conn, const char *extra_fmt, ...);
int sigma_resp_error(struct sigma_conn *conn, const char *extra_fmt, ...);
int sigma_resp_raw(struct sigma_conn *conn, const char *fmt, ...);

/* --- Device commands (sigma_dut_cmd_device.c) --- */

extern const struct sigma_cmd_entry sigma_device_commands[];

/* --- STA commands (sigma_dut_cmd_sta.c) --- */

extern const struct sigma_cmd_entry sigma_sta_commands[];

/* --- HE commands (sigma_dut_cmd_he.c) --- */

extern const struct sigma_cmd_entry sigma_he_commands[];

/* --- Traffic commands (sigma_dut_cmd_traffic.c) --- */

extern const struct sigma_cmd_entry sigma_traffic_commands[];

#endif /* __SIGMA_DUT_PRIV_H__ */
