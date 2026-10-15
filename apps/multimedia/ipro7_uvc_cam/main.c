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
 * @file main.c
 * @brief IPRO7 UVC Camera — standalone USB webcam.
 *
 * Brings up the DVP sensor (SC101IOT, 720p native) -> ISP -> hardware MJPEG
 * encoder -> USB UVC isochronous stream. The device enumerates as a plain UVC
 * webcam; once the host opens the stream it runs continuously.
 *
 * Deliberately minimal: no PIR motion detection, no AI/NPU, no low-power
 * sleep, no filesystem.
 */

#include <stdint.h>
#include <stdio.h>

#include <generated/autoconf.h>

#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

#include <FreeRTOS.h>
#include <task.h>

#include "hal_boot2.h"
#include "hal_sys.h"
#include "platform_common.h"
#include "freertos_app_hooks.h"
#include "compiler/common.h"

#include "uvc_isp.h"

/* USB UVC bring-up (uvc.c) */
extern void uvc_init(void);

/****************************************/
/*    FREERTOS HOOKS (OVERRIDES)        */
/****************************************/

/*
 * Place the FreeRTOS timer-service task stack in PSRAM. OCRAM is tight under
 * this app's linker script (ipro7_uvc_cam.ld), and the default weak hook
 * would draw from it.
 */
#define UVC_TIMER_TASK_STACK_DEPTH  (configMINIMAL_STACK_SIZE)

void __attribute__((weak)) vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                                          StackType_t **ppxTimerTaskStackBuffer,
                                                          uint32_t *pulTimerTaskStackSize)
{
    static StaticTask_t xTimerTaskTCB;
    static ATTR_PSRAM_SECTION StackType_t uxTimerTaskStack[UVC_TIMER_TASK_STACK_DEPTH];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = UVC_TIMER_TASK_STACK_DEPTH;
}

/****************************************/
/*    INIT TASK                         */
/****************************************/

#define UVC_INIT_STACK_SIZE  (512)

static void uvc_init_task(void *arg)
{
    (void)arg;

#ifdef CONFIG_SHELL
    shell_init_with_task(UART0_INDEX);
#endif

    printf("[UVC] Bringing up USB UVC webcam (720p MJPEG)...\r\n");

    /* USB device + UVC class first, then sensor / ISP / MJPEG stream.
     * Preview size from uvc_isp.h: QVGA on the EVB (the 640x384 model leaves
     * too little PSRAM for a VGA ch0 ping-pong; detection stays 640x384),
     * 720p on the Reef + OV5640 build (no AI), 640x360 on the Reef
     * rock-paper-scissors build (CONFIG_UVC_RPS, uvc_rps.c). */
    uvc_init();
    uvc_isp_set_param(UVC_RES_MODE, true);
    uvc_isp_init();

    printf("[UVC] Streaming pipeline started\r\n");

    /* Streaming now runs in the ISP/MJPEG task; free this init stack. */
    vTaskDelete(NULL);
    for (;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

/****************************************/
/*    MAIN ENTRY                        */
/****************************************/

int main(void)
{
    platform_init(0);

    printf("[SYS] Reset reason: %s (%d)\r\n",
           hal_rst_reason_str(hal_rst_reason_get()),
           hal_rst_reason_get());

    hal_boot2_init();

    ipro_setup_heap();

    printf("[UVC] Starting init task...\r\n");
    xTaskCreate(uvc_init_task, "uvc_init", UVC_INIT_STACK_SIZE, NULL, 10, NULL);

    printf("[OS] Starting OS Scheduler...\r\n");
    vTaskStartScheduler();

    return 0;
}
