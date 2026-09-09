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
 * @file ipro_coex_cli.c
 * @brief Console access to the coex arbiter.
 *
 * The reference exports four commands (coex_dump_all / coex_wlan / coex_bt /
 * coex_mode) where coex_mode leaves hidden state that changes what a later
 * coex_wlan does. One command with an explicit mode argument covers the same
 * ground without that trap, and keeps the `coex wifi` / `coex bt` /
 * `coex coexist` spelling the bench scripts already use.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipro_coex.h>
#include "shell.h"

static int coex_mode_arg(int argc, char **argv, int idx)
{
    int mode = (argc > idx) ? atoi(argv[idx]) : IPRO_COEX_FORCE_MODE_PTA_FORCE;

    if (mode < IPRO_COEX_FORCE_MODE_PTA_FORCE ||
        mode > IPRO_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2) {
        return -1;
    }
    return mode;
}

static void coex_usage(void)
{
    /* Split deliberately: printf formats through one shared 512-byte buffer
     * (newlibc/debug.c) and this text is within a few characters of its limit. */
    printf("usage: coex [wifi|bt|wifi_pri|bt_pri|coexist] [mode]\r\n"
           "  (no arg)  dump the decoded PTA and MAC coex state\r\n"
           "  wifi [m]  hand the medium to WiFi (default mode 0: BT muted)\r\n"
           "  bt   [m]  hand the medium to BT   (mode 0 mutes WiFi -- link drops)\r\n");
    printf("  wifi_pri  == coex wifi 1  (arbiter on, WiFi wins)\r\n"
           "  bt_pri    == coex bt   1  (arbiter on, BT wins)\r\n"
           "  coexist   restore the boot default\r\n"
           "  mode m:   0 PTA_FORCE  1 PTI_DEFAULT  2 PTI_PRIORITY  3 PTI_PRIORITY2\r\n");
}

void cmd_coex(int argc, char **argv)
{
    const char *what = (argc > 1) ? argv[1] : NULL;
    int mode;

    if (what == NULL) {
        ipro_coex_dump_all();
        return;
    }

    if (!strcmp(what, "coexist")) {
        ipro_coex_init_default();
        printf("coex: restored the boot default\r\n");
    } else if (!strcmp(what, "wifi") || !strcmp(what, "wifi_pri")) {
        mode = strcmp(what, "wifi_pri") ? coex_mode_arg(argc, argv, 2)
                                        : IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE;
        if (mode < 0) {
            coex_usage();
            return;
        }
        ipro_coex_force_mode(mode);
        ipro_coex_force_wlan();
        printf("coex: WiFi forced, mode %d\r\n", mode);
    } else if (!strcmp(what, "bt") || !strcmp(what, "bt_pri")) {
        mode = strcmp(what, "bt_pri") ? coex_mode_arg(argc, argv, 2)
                                      : IPRO_COEX_FORCE_MODE_PTI_DEFAULT_FORCE;
        if (mode < 0) {
            coex_usage();
            return;
        }
        ipro_coex_force_mode(mode);
        ipro_coex_force_bt();
        printf("coex: BT forced, mode %d\r\n", mode);
    } else {
        coex_usage();
        return;
    }

    ipro_coex_dump_all();
}

SHELL_CMD_EXPORT_ALIAS(cmd_coex, coex, wifi/bt PTA arbiter: wifi|bt|wifi_pri|bt_pri|coexist);

int ipro_coex_cli_init(void)
{
    /* The component is linked whole-archive (see CMakeLists.txt), so unlike
     * the reference nothing has to call this to keep the command alive. Kept
     * so callers ported from bl_iot_sdk still compile. */
    return 0;
}
