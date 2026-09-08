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
//#include <FreeRTOS_POSIX.h>
//#include <pthread.h>
#include <task.h>
#include <getopt.h>
#include "drv_glb.h"


enum mjpeg_test_numeric_short_options {
    MJPEG_TEST_SET_IN_RING_BUF_HEIGHT  = 1000,
    MJPEG_TEST_TSRC_INIT,
    MJPEG_TEST_TSRC_TRI,
};


static void mjpeg_test_show_usage(void)
{
    printf("mjpeg_test [options]\r\n");
    //printf("--set-in-ring-h             [height]    Set input ring buffer buffer height, need to be 16 aligned\r\n");

    printf("--tsrc-init         dvp_tsrc init\r\n");
    printf("--tsrc-trig         dvp_dvp_tsrc trigger\r\n");

    //printf("-k                            trigger kick signle for kick mode to start encode \r\n");
}


static int mjpeg_test_main_cmd(int argc, char **argv)
{
    extern int tsrc_init(void);
    extern int tsrc_trigger(void);

    //static uint32_t width, height;
    //uint32_t bs_addr, bs_size;
    //uint32_t value = 0;
    int ch;

    optind = 0;

    static struct option mjpeg_test_long_options[] =
    {
        ///"set-in-ring-h",           required_argument, 0,  JPEG_TEST_SET_IN_RING_BUF_HEIGHT},

        {"tsrc-init",               no_argument,       0,  MJPEG_TEST_TSRC_INIT},
        {"tsrc-trig",               no_argument,       0,  MJPEG_TEST_TSRC_TRI},

        //{"k",                       no_argument,       0, 'k'},

        {0, 0, 0, 0}
    };

    while ((ch = getopt_long (argc, argv, "k", mjpeg_test_long_options, NULL)) != -1) {
        switch (ch)
        {

            case 'k':
                //jpeg_test_kick(2);  /*fix using ehc channel 2 for JPEG*/
                break;

            case MJPEG_TEST_SET_IN_RING_BUF_HEIGHT:
                //value = atoi(optarg);
                //jpeg_test_set_in_ring_height(2, value);   /*fix using ehc channel 2 for JPEG*/
                break;
            case MJPEG_TEST_TSRC_INIT:
                tsrc_init();
                break;
            case MJPEG_TEST_TSRC_TRI:
                tsrc_trigger();
                break;


            default:
                mjpeg_test_show_usage();
                goto  MJEPG_TEST_FAIL;
        }
    }

    printf("|mjpeg_test_cli| run mjpeg_test_main_cmd() [OK]\r\n");
    return 0;

MJEPG_TEST_FAIL:
    printf("|mjpeg_test_cli| run mjpeg_test_main_cmd() [ERROR]\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(mjpeg_test_main_cmd, mjpeg_test_cmd, mjpeg encoder test command);


TaskHandle_t gCmdTaskHdl;
void mjpeg_test_cmd_run_task(void *args)
{
    extern void mjpeg_test(void);
    gCmdTaskHdl = xTaskGetCurrentTaskHandle();

    mjpeg_test();
    //pthread_exit(0);
    vTaskDelete(NULL);
    printf("creat mjpeg_test_cmd_run_task ended~~~\r\n");
}

static int cmd_mjpeg_test(int argc, char **argv)
{

    //pthread_attr_t  attr;

    //pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); /*PTHREAD_CREATE_JOINABLE, PTHREAD_CREATE_DETACHED*/
    //pthread_attr_setstacksize(&attr, 1024*2);

    printf("create mjpeg_test_cmd_run_task to run mjpeg_test \r\n");
    //pthread_create(a_pt_tid_task, &attr, &mjpeg_test_cmd_run_task, (void*)0);
    xTaskCreate(mjpeg_test_cmd_run_task,"mjpeg_test_cmd_run_task",1024,NULL,1,NULL);
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_mjpeg_test, mjpeg_test, mjpeg encoder test);




void demo_mjpeg_init(void)
{
    #if defined(CONFIG_IPRO6)
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MJPEG);
    #endif
    #if defined(CONFIG_IPRO7)
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_MM);
    #endif
}



