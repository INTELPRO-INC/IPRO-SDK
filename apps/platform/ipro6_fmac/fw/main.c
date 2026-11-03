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
 * IPRO6S FullMAC firmware. Runs from OCRAM after the FMAC loader's jump,
 * keeps the loader's SDIO or USB session and runs the macsw UMAC/LMAC for a
 * Linux cfg80211 host.
 */
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>

#include "platform_common.h"
#include "ipro_irq.h"
#include "fmac_host.h"

void setup_heap(void);
void wifi_main(void *param);
/* MAC HW ISR of the macsw intc */
void interrupt0_handler(void);

#ifdef CONFIG_IPRO6_FMAC_AP
#define FW_STACK_WORDS 256
#else
#define FW_STACK_WORDS 1536
#endif
#define FW_TASK_PRIO   30

int main(void)
{
    platform_init(0);
    setup_heap();
    if (fmac_host_init())
        printf("[FMACFW] host link init failed\r\n");
    /* Writes the vector slot and enables the line; board_init does not */
    Interrupt_Handler_Register(WIFI_IRQn, interrupt0_handler);
    if (xTaskCreate(wifi_main, "fw", FW_STACK_WORDS, NULL, FW_TASK_PRIO, NULL) != pdPASS)
        printf("[FMACFW] fw task create failed\r\n");
    vTaskStartScheduler();
    return 0;
}
