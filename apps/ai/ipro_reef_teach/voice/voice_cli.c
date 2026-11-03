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
 * @file voice_cli.c
 * @brief `spk` shell command: the same requests the Teach page sends, from UART0.
 */

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

#include "voice.h"
#include "voice_link.h"

static void usage(void)
{
    printf("spk status | list | echo on|off\r\n"
           "spk enroll <seconds 8-30> <name>   read aloud; replaces <name>'s voiceprint\r\n"
           "spk live on|off | stop\r\n"
           "spk del <name> | name <anon k> <name>\r\n"
           "spk thresh <id> <anon>             cosine thresholds\r\n"
           "spk mode normal|stranger           stranger mode (resets the thresholds)\r\n");
}

/* Rest of the command line from argv[i] on, spaces restored (names may have them). */
static const char *rest(int argc, char **argv, int i)
{
    static char buf[VOICE_NAME_MAX + 1];
    size_t n = 0;

    buf[0] = 0;
    for (; i < argc; i++) {
        n += (size_t)snprintf(buf + n, sizeof(buf) - n, "%s%s", n ? " " : "", argv[i]);
        if (n >= sizeof(buf)) {
            break;
        }
    }
    return buf;
}

static int cmd_spk(int argc, char **argv)
{
    int rc = 0;

    if (argc < 2 || !strcmp(argv[1], "status")) {
        voice_print_status();
        voice_link_print();
        return 0;
    }
    if (!strcmp(argv[1], "list")) {
        voice_print_status();
    } else if (!strcmp(argv[1], "echo") && argc > 2) {
        voice_set_echo(!strcmp(argv[2], "on"));
    } else if (!strcmp(argv[1], "enroll") && argc > 3) {
        voice_set_echo(true);
        rc = voice_enroll(rest(argc, argv, 3), (uint32_t)strtoul(argv[2], NULL, 10));
    } else if (!strcmp(argv[1], "live") && argc > 2) {
        voice_set_echo(true);
        rc = voice_live(!strcmp(argv[2], "on"));
    } else if (!strcmp(argv[1], "stop")) {
        rc = voice_stop();
    } else if (!strcmp(argv[1], "del") && argc > 2) {
        rc = voice_delete(rest(argc, argv, 2));
    } else if (!strcmp(argv[1], "name") && argc > 3) {
        rc = voice_name_anon(atoi(argv[2]), rest(argc, argv, 3));
    } else if (!strcmp(argv[1], "mode") && argc > 2 &&
               (!strcmp(argv[2], "stranger") || !strcmp(argv[2], "normal"))) {
        voice_set_echo(true);
        rc = voice_set_mode(!strcmp(argv[2], "stranger"));
    } else if (!strcmp(argv[1], "thresh") && argc > 3) {
        rc = voice_set_thresholds(strtof(argv[2], NULL), strtof(argv[3], NULL));
    } else {
        usage();
        return -1;
    }
    if (rc != 0) {
        printf("refused (bad arguments, or busy)\r\n");
    }
    return rc;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spk, spk, Speaker enrollment and live labelling);

#endif /* CONFIG_SHELL */
