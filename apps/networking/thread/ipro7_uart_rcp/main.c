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

#include "FreeRTOS.h"
#include "task.h"
#include "freertos_app_hooks.h"
#include "ieee802154.h"
#include "openthread_port.h"
#ifdef CONFIG_NEWLIBC_CONSOLE_GUARD
#include "console_guard.h"
#endif

static void rcp_start_task(void *arg)
{
    (void)arg;
    extern void rf_set_bz_mode(uint8_t mode);
    extern void rf_set_pwr_offset_zb(int8_t offsets[16]);
    extern void rf_optimize_by_channel(uint32_t channel_mhz);
    extern uint8_t rf_set_tx_power_zb(int32_t power);
    int8_t offsets[16] = {0};
    rf_set_bz_mode(1); /* Configure the demodulator for 802.15.4 before init. */
    rf_set_pwr_offset_zb(offsets);
    ieee802154_config_t radio_config = {
        .channel = 15,
        .pan_id = 0xffff,
        .short_addr = 0xffff,
        .tx_power = 0,
        .auto_ack = true,
        .cca_mode = IEEE802154_CCA_MODE_CARRIER_AND_ENERGY,
        .cca_threshold = -75,
    };

    printf("\r\nIPRO7 OpenThread UART RCP: UART%d TX=GPIO%d RX=GPIO%d @ %d\r\n",
           CONFIG_THREAD_RCP_UART_PORT, CONFIG_THREAD_RCP_UART_TX_PIN,
           CONFIG_THREAD_RCP_UART_RX_PIN, CONFIG_THREAD_RCP_UART_BAUD);
    if (ieee802154_init(&radio_config) != IEEE802154_OK) {
        printf("RCP radio initialization failed\r\n");
        vTaskDelete(NULL);
        return;
    }

    rf_optimize_by_channel(2425);
    (void)rf_set_tx_power_zb(0);
    otrStart();
    vTaskDelete(NULL);
}

int main(void)
{
    ipro_platform_init();
#ifdef CONFIG_NEWLIBC_CONSOLE_GUARD
    /* console lines serialized between tasks (radio_trace stream vs shell) */
    if (!newlibc_console_init()) {
        printf("console guard init failed\r\n");
    }
#endif
    if (xTaskCreate(rcp_start_task, "rcp_start", 1024, NULL, 5, NULL) != pdPASS) {
        return -1;
    }
    vTaskStartScheduler();
    return -1;
}
