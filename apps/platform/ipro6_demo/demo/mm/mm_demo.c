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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include <FreeRTOS.h>
#include <task.h>

static int cmd_sensor_start(int argc, char **argv)
{
    extern void strmctrl_init(int use_dvp);
    if (argc == 2)
        strmctrl_init(atoi(argv[1]));
    else
    {
        printf("sensor start option\r\n");
        printf("sensor_start use_dvp\r\n");
        printf("use_dvp: 0 for spi_sensor, 1 for dvp_sensor\r\n");
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sensor_start, sensor_start, sensor start stream);

static int cmd_sensor_get_next(int argc, char **argv)
{
    extern void strmctrl_get_next(void);
    strmctrl_get_next();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sensor_get_next, sensor_next, sensor get next mjpg);

static int cmd_sensor_set_frc(int argc, char **argv)
{
    extern void strmctrl_set_frc(int demo_id);
    if (argc == 2)
        strmctrl_set_frc(atoi(argv[1]));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_sensor_set_frc, sensor_frc, sensor frame rate);

void demo_mm_init(void)
{

}

