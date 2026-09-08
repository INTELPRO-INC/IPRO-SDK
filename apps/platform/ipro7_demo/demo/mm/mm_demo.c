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

static int cmd_dvp_sensor_start(int argc, char **argv)
{
    extern void strmctrl_init();
    strmctrl_init();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dvp_sensor_start, dvp_sensor, dvp_sensor);

static int cmd_dvp_sensor_get_next(int argc, char **argv)
{
    extern void strmctrl_get_next(void);
    strmctrl_get_next();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dvp_sensor_get_next, dvp_next, dvp_next);

static int cmd_dvp_sensor_set_zoom(int argc, char **argv)
{
    extern void strmctrl_set_zoom(int demo_id);
    if (argc == 2)
        strmctrl_set_zoom(atoi(argv[1]));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dvp_sensor_set_zoom, dvp_zoom, dvp_zoom);

static int cmd_dvp_sensor_set_frc(int argc, char **argv)
{
    extern void strmctrl_set_frc(int channel, int demo_id);
    if (argc == 3)
        strmctrl_set_frc(atoi(argv[1]), atoi(argv[2]));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_dvp_sensor_set_frc, dvp_frc, dvp frame rate);

#ifdef CONFIG_APP_SPI_SENSOR
static int cmd_spi_sensor_start(int argc, char **argv)
{
    extern void cam_task_entry();
    cam_task_entry();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_spi_sensor_start, spi_sensor, spi_sensor);
#endif

int cmd_spi_cam(int argc, char **argv);
SHELL_CMD_EXPORT_ALIAS(cmd_spi_cam, spi_cam, sensor_read);

void demo_mm_init(void)
{
}

