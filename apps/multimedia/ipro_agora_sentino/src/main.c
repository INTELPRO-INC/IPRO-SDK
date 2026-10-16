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
 * @brief IPRO7 Agora Sentino AI Gateway — entry point
 *
 * Platform init, heap setup, then delegates to the gateway orchestrator.
 * All module logic lives in the ops-based modules; main.c is thin.
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "compiler/common.h"
#include "platform_common.h"
#include "freertos_app_hooks.h"
#include <hal_boot2.h>
#include <hal_sys.h>
#include "drv_ef_ctrl.h"

#include "app_log.h"

#define LOG_TAG "MAIN"

/* Module headers */
#include "app_gateway.h"
#include "network_lte.h"
#include "network_emac.h"
#include "network_wifi_aic.h"
#include "network_wifi_ipro6s.h"
#include "audio_pdm.h"
#include "audio_dac.h"
#include "rtc_agora.h"
#include "cloud_sentino.h"

#include <generated/autoconf.h>

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
extern uint32_t __psram_available;
#endif

void __attribute__((weak)) debug_uart_id_set(uint8_t id) { (void)id; }

/* ========== Shell Commands ========== */
#ifdef CONFIG_SHELL
#include "hal_uart.h"
#include "shell.h"


#ifdef CONFIG_BLUETOOTH_LE_ENABLE
#include "app_ble_prov.h"
#include "app_sentino.h"

static int cmd_ble_prov_start(int argc, char **argv) {
    (void)argc; (void)argv;
    const char *pid, *uuid;
    app_sentino_get_device_info(&pid, &uuid);
    app_ble_prov_init(pid, uuid, NULL);
    app_ble_prov_start();
    printf("BLE provisioning ADV started\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_prov_start, ble_prov_start, Start BLE provisioning ADV);

static int cmd_ble_prov_clear(int argc, char **argv) {
    (void)argc; (void)argv;
    app_ble_prov_clear_bind();
    printf("Bind state cleared. Reboot to enter provisioning mode.\r\n");
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_prov_clear, ble_prov_clear, Clear bind state);

static int cmd_ble_prov_status(int argc, char **argv) {
    (void)argc; (void)argv;
    printf("Bound: %s\r\n", app_ble_prov_is_bound() ? "yes" : "no");
    if (app_ble_prov_is_bound()) {
        printf("  userId: %s\r\n", app_ble_prov_get_user_id());
        printf("  assetId: %s\r\n", app_ble_prov_get_asset_id());
    }
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_ble_prov_status, ble_prov_status, Show bind status);
#endif /* CONFIG_BLUETOOTH_LE_ENABLE */

#endif /* CONFIG_SHELL */

#if defined(CONFIG_BLUETOOTH_LE_ENABLE) && defined(CONFIG_NETWORK_USE_WIFI_IPRO6S)
#include "app_ble_prov.h"

/* BLE provisioning's Wi-Fi half on the IPRO6S fin: the AP list the phone
 * asks for and the credentials it sends back. */
#define PROV_SCAN_TIMEOUT_MS 6000

static int prov_wifi_scan(ble_prov_ap_t *aps, int max)
{
    static network_wifi_ap_t found[45];
    int cap = (max < (int)(sizeof(found) / sizeof(found[0]))) ? max
                                                              : (int)(sizeof(found) / sizeof(found[0]));
    int n = network_wifi_ipro6s_scan(found, cap, PROV_SCAN_TIMEOUT_MS);

    for (int i = 0; i < n; i++) {
        memcpy(aps[i].ssid, found[i].ssid, sizeof(aps[i].ssid));
        aps[i].rssi = found[i].rssi;
        aps[i].secure = found[i].secure;
    }
    return n;
}
#endif

int main(void) {
    platform_init(0);
    ipro_setup_heap();
    hal_boot2_init();

#ifdef CONFIG_IPRO_LOG_ENABLE
    ipro_log_init(NULL);
#endif

    IPRO_LOGI(LOG_TAG, "IPRO7 Agora Sentino AI Gateway");
    IPRO_LOGI(LOG_TAG, "FreeRTOS Kernel: %s", tskKERNEL_VERSION_NUMBER);
    IPRO_LOGI(LOG_TAG, "Reset reason: %s (%d)",
              hal_rst_reason_str(hal_rst_reason_get()), hal_rst_reason_get());

    {
        Efuse_Device_Info_Type dev_info;
        EF_Ctrl_Read_Device_Info(&dev_info);
        IPRO_LOGI(LOG_TAG, "EFUSE: flash_cfg=%d sf_swap=%d psram_cfg=%d pkg_info=%d sf_reverse=%d",
                  dev_info.flash_cfg, dev_info.sf_swap_cfg, dev_info.psram_cfg,
                  dev_info.pkg_info, dev_info.sf_reverse);
    }

#if defined(CONFIG_LINK_FLASH) && defined(CONFIG_USE_PSRAM)
    if (__psram_available) {
        IPRO_LOGI(LOG_TAG, "[PSRAM] Initialized successfully");
    } else {
        IPRO_LOGE(LOG_TAG, "[PSRAM] WARNING: Init failed, running without PSRAM");
    }
#endif

    /* Initialize network stack (before scheduler, required by lwIP) */
#if defined(CONFIG_NETWORK_USE_LTE)
    network_lte_ops.init();
#elif defined(CONFIG_NETWORK_USE_WIFI_AIC)
    network_wifi_aic_ops.init();
#elif defined(CONFIG_NETWORK_USE_WIFI_IPRO6S)
    network_wifi_ipro6s_ops.init();
#else
    network_emac_ops.init();
#endif

#ifdef CONFIG_SHELL
    shell_init_with_task(UART0_INDEX);
#endif

    /* Start the gateway orchestrator */
    gateway_config_t gw_cfg = {
#if defined(CONFIG_NETWORK_USE_LTE)
        .network   = &network_lte_ops,
#elif defined(CONFIG_NETWORK_USE_WIFI_AIC)
        .network   = &network_wifi_aic_ops,
#elif defined(CONFIG_NETWORK_USE_WIFI_IPRO6S)
        .network   = &network_wifi_ipro6s_ops,
#else
        .network   = &network_emac_ops,
#endif
        .audio_in  = &audio_pdm_ops,
        .audio_out = &audio_dac_ops,
        .rtc       = &rtc_agora_ops,
        .cloud     = &cloud_sentino_ops,
    };
#if defined(CONFIG_BLUETOOTH_LE_ENABLE) && defined(CONFIG_NETWORK_USE_WIFI_IPRO6S)
    app_ble_prov_set_wifi_hooks(prov_wifi_scan, network_wifi_ipro6s_set_credentials);
#endif
    app_gateway_start(&gw_cfg);

    vTaskStartScheduler();

    while (1) {
    }
    return 0;
}
