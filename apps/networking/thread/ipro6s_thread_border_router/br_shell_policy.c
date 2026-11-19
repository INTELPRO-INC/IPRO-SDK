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

#include <generated/autoconf.h>
#ifdef CONFIG_SHELL
#include <stddef.h>
#include <string.h>

/* A Matter BR has one commissioning owner. Fence external shell writers from
 * boot, before any command can queue asynchronous WPA/IPC work. A plain SDK BR
 * retains its bring-up CLI. This is NOT a fence for direct firmware API calls.
 * Reset commands preserve the existing boot-journal recovery/DL workflow. */
int shell_command_allowed(int argc, char *argv[])
{
#ifdef IPRO_MATTER_PACKAGE_ACTIVE
    if (argc < 1 || !argv || !argv[0]) return 0;
    if (argc == 3 && strcmp(argv[0], "ot") == 0) {
        return argv[1] && argv[2] &&
            ((strcmp(argv[1], "nat64") == 0 &&
              (strcmp(argv[2], "state") == 0 || strcmp(argv[2], "counters") == 0 ||
               strcmp(argv[2], "mappings") == 0)) ||
             (strcmp(argv[1], "br") == 0 && strcmp(argv[2], "nat64prefix") == 0));
    }
    if (argc == 4 && strcmp(argv[0], "ot") == 0) {
        return argv[1] && argv[2] && argv[3] &&
               strcmp(argv[1], "br") == 0 && strcmp(argv[2], "pd") == 0 &&
               (strcmp(argv[3], "state") == 0 || strcmp(argv[3], "omrprefix") == 0);
    }
    if (argc == 2 && strcmp(argv[0], "ot") == 0) {
        static const char *const queries[] = {
            "state", "version", "ipaddr", "rloc16", "extaddr", "channel", "panid", "networkname"
        };
        if (!argv[1]) return 0;
        for (size_t i = 0; i < sizeof(queries) / sizeof(queries[0]); ++i)
            if (strcmp(argv[1], queries[i]) == 0) return 1;
        return 0;
    }
    if (argc != 1) return 0;
    static const char *const commands[] = {
        "help", "reboot", "reboot_dl", "wifi_state", "wifi_sta_rssi", "wifi_sta_channel",
        "wifi_sta_mac_get", "br_dua_stats", "br_mdns_stats", "br_mcast_stats",
        "br_packet_trace", "br_cli_timing", "br_wifi_provision", "rcp_status"
    };
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i)
        if (strcmp(argv[0], commands[i]) == 0) return 1;
    return 0;
#else
    (void)argc;
    (void)argv;
    return 1;
#endif
}
#endif
