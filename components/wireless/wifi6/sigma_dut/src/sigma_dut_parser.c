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

#include <string.h>
#include <ctype.h>
#include "sigma_dut_priv.h"

static void str_tolower(char *s)
{
    for (; *s; s++)
        *s = tolower((unsigned char)*s);
}

static void str_trim_trailing(char *s)
{
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == '\r' || s[len - 1] == '\n' ||
                       s[len - 1] == ' '))
        s[--len] = '\0';
}

int sigma_parse_cmd(const char *line, struct sigma_cmd *cmd)
{
    char buf[SIGMA_CMD_MAX_LEN];
    char *token, *saveptr;
    int idx = 0;

    memset(cmd, 0, sizeof(*cmd));

    if (!line || !line[0])
        return -1;

    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    str_trim_trailing(buf);

    if (!buf[0])
        return -1;

    /* First token is the command name */
    token = strtok_r(buf, ",", &saveptr);
    if (!token)
        return -1;

    strncpy(cmd->name, token, sizeof(cmd->name) - 1);
    str_tolower(cmd->name);

    /* Remaining tokens are key,value pairs */
    cmd->param_count = 0;
    while (cmd->param_count < SIGMA_MAX_PARAMS) {
        char *key = strtok_r(NULL, ",", &saveptr);
        if (!key)
            break;

        char *val = strtok_r(NULL, ",", &saveptr);
        if (!val)
            val = "";

        strncpy(cmd->params[idx].key, key, SIGMA_PARAM_KEY_LEN - 1);
        str_tolower(cmd->params[idx].key);

        strncpy(cmd->params[idx].val, val, SIGMA_PARAM_VAL_LEN - 1);
        /* values are case-sensitive — don't lowercase */

        idx++;
        cmd->param_count = idx;
    }

    return 0;
}

const char *sigma_cmd_get_param(const struct sigma_cmd *cmd, const char *key)
{
    for (int i = 0; i < cmd->param_count; i++) {
        if (strcasecmp(cmd->params[i].key, key) == 0)
            return cmd->params[i].val;
    }
    return NULL;
}
