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
#include "shell.h"
#include <FreeRTOS.h>
#include <task.h>
#include "drv_glb.h"


static int cmd_isp_tsrc(int argc, char **argv)
{
    extern void isp_tsrc_test(void);
    isp_tsrc_test();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_isp_tsrc, isp_tsrc, isp tsrc);

static int cmd_isp_dvp(int argc, char **argv)
{
    extern void isp_dvp_test(void);
    isp_dvp_test();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_isp_dvp, isp_dvp, isp dvp);

static int cmd_isp_spi(int argc, char **argv)
{
    extern void isp_spi_test(void);
    isp_spi_test();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_isp_spi, isp_spi, isp spi);

static int cmd_cam_dump_raw(int argc, char **argv)
{
    extern void cam_dump_raw_test(int in_src);
    cam_dump_raw_test(atoi(argv[1]));
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_cam_dump_raw, cam_dump_raw, cam dump raw);

void demo_isp_init(void)
{
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
}



