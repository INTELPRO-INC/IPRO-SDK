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

static int cmd_mjdec_test(int argc, char **argv)
{
    extern void mjdec_test(void);
    mjdec_test();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mjdec_test, mjdec_test, jpeg decoder test);


void demo_mjdec_init(void)
{
    #if defined(CONFIG_IPRO6)
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MJPEG);
    #endif
    #if defined(CONFIG_IPRO7)
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    #endif
}



