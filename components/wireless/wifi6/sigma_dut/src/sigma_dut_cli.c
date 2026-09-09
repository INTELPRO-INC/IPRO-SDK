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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include "sigma_dut.h"

static int cmd_sigma_dut(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: sigma_dut start [port] | stop\n");
        return -1;
    }

    if (strcmp(argv[1], "start") == 0) {
        uint16_t port = 0;
        if (argc >= 3)
            port = (uint16_t)atoi(argv[2]);
        return sigma_dut_start(port);
    }

    if (strcmp(argv[1], "stop") == 0)
        return sigma_dut_stop();

    printf("usage: sigma_dut start [port] | stop\n");
    return -1;
}

SHELL_CMD_EXPORT_ALIAS(cmd_sigma_dut, sigma_dut, WFA sigma-dut CAPI agent);
