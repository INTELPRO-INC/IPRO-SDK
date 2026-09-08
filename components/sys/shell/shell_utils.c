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

/*
 * Shell Utility Commands
 * Memory access and system time commands
 */

#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_mtimer.h"

/* Print memory command */
static int shell_pmem(int argc, char **argv)
{
    int i;
    char *pos = NULL;
    char *addr = NULL;
    int nunits = 16;
    int width = 4;

    switch (argc) {
        case 4:
            width = strtol(argv[3], NULL, 0);
            __attribute__ ((fallthrough));
        case 3:
            nunits = strtol(argv[2], NULL, 0);
            nunits = nunits > 0x400 ? 0x400 : nunits;
            __attribute__ ((fallthrough));
        case 2:
            addr = (char *)(uintptr_t)strtoul(argv[1], &pos, 0);
            break;
        default:
            break;
    }

    if (pos == NULL || pos == argv[1]) {
        printf("p <addr> <nunits> <width>\r\n"
               "addr  : address to display\r\n"
               "nunits: number of units to display (default is 16)\r\n"
               "width : width of unit, 1/2/4 (default is 4)\r\n");
        return 0;
    }

    switch (width) {
        case 1:
            for (i = 0; i < nunits; i++) {
                if (i % 16 == 0) {
                    printf("0x%08x:", (unsigned int)(uintptr_t)addr);
                }
                printf(" %02x", *(unsigned char *)addr);
                addr += 1;
                if (i % 16 == 15) {
                    printf("\r\n");
                }
            }
            break;
        case 2:
            for (i = 0; i < nunits; i++) {
                if (i % 8 == 0) {
                    printf("0x%08x:", (unsigned int)(uintptr_t)addr);
                }
                printf(" %04x", *(unsigned short *)addr);
                addr += 2;
                if (i % 8 == 7) {
                    printf("\r\n");
                }
            }
            break;
        default:
            for (i = 0; i < nunits; i++) {
                if (i % 4 == 0) {
                    printf("0x%08x:", (unsigned int)(uintptr_t)addr);
                }
                printf(" %08x", *(unsigned int *)addr);
                addr += 4;
                if (i % 4 == 3) {
                    printf("\r\n");
                }
            }
            break;
    }

    if ((width == 1 && nunits % 16 != 0) ||
        (width == 2 && nunits % 8 != 0) ||
        (width == 4 && nunits % 4 != 0)) {
        printf("\r\n");
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_pmem, p, print memory);

/* Modify memory command */
static int shell_mmem(int argc, char **argv)
{
    void *addr = NULL;
    int width = 4;
    unsigned int value = 0;
    unsigned int old_value;
    unsigned int new_value;

    switch (argc) {
        case 4:
            width = strtol(argv[3], NULL, 0);
            __attribute__ ((fallthrough));
        case 3:
            value = (unsigned int)strtoul(argv[2], NULL, 0);
            __attribute__ ((fallthrough));
        case 2:
            addr = (void *)(uintptr_t)strtoul(argv[1], NULL, 0);
            break;
        default:
            addr = NULL;
            break;
    }

    if (addr == NULL) {
        printf("m <addr> <value> <width>\r\n"
               "addr  : address to modify\r\n"
               "value : new value (default is 0)\r\n"
               "width : width of unit, 1/2/4 (default is 4)\r\n");
        return 0;
    }

    switch (width) {
        case 1:
            old_value = (unsigned int)(*(unsigned char volatile *)addr);
            *(unsigned char volatile *)addr = (unsigned char)value;
            new_value = (unsigned int)(*(unsigned char volatile *)addr);
            break;
        case 2:
            old_value = (unsigned int)(*(unsigned short volatile *)addr);
            *(unsigned short volatile *)addr = (unsigned short)value;
            new_value = (unsigned int)(*(unsigned short volatile *)addr);
            break;
        case 4:
        default:
            old_value = *(unsigned int volatile *)addr;
            *(unsigned int volatile *)addr = (unsigned int)value;
            new_value = *(unsigned int volatile *)addr;
            break;
    }
    printf("value on 0x%x change from 0x%x to 0x%x.\r\n",
           (unsigned int)(uintptr_t)addr, old_value, new_value);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_mmem, m, modify memory);

/* System uptime command */
static int shell_time(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    long long ms;
    long long days;
    long long hours;
    long long minutes;
    long long seconds;

    ms = (long long)mtimer_get_time_ms();
    printf("UP time in ms %llu\r\n", ms);
    seconds = ms / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;
    days = hours / 24;
    printf("UP time: %llu days, %llu hours, %llu minutes, %llu seconds\r\n",
           days,
           hours % 24,
           minutes % 60,
           seconds % 60);

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(shell_time, time, system uptime);
