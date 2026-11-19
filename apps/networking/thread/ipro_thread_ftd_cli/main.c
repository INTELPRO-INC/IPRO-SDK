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
 * @brief IPRO Thread FTD CLI Application Main Entry Point
 * 
 * This application demonstrates how to use OpenThread as a Full Thread Device (FTD)
 * with CLI interface for Thread network management.
 */

#ifdef CONFIG_NEWLIBC_CONSOLE_GUARD
#include "console_guard.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

// Shell support
#ifdef CONFIG_SHELL
#include "shell.h"
#include "hal_uart.h"
#endif

/* Filesystem */
#if defined(CONFIG_EVFS) && defined(CONFIG_EVFS_POSIX)
#include "evfs.h"
#include "evfs_posix.h"
#endif

// mbedTLS self-test
#include "mbedtls_self_test.h"

#include "thread_ftd_app.h"
#include "ipro_log.h"
#include "freertos_app_hooks.h"

#include "ieee802154.h"

/*
 * Application Entry Point
 */
int main(void)
{
    // Initialize platform
    ipro_platform_init();
#ifdef CONFIG_NEWLIBC_CONSOLE_GUARD
    /* console lines serialized between tasks (radio_trace stream vs CLI) */
    if (!newlibc_console_init()) {
        printf("console guard init failed\r\n");
    }
#endif

#ifdef CONFIG_IEEE802154_ENABLE
    /* Initialize IEEE 802.15.4 driver. PAN ID / short addr / ext addr
     * are overwritten by OpenThread later via otPlatRadioSet*.
     *
     * RF mode MUST be switched to ZB-only BEFORE ieee802154_init() —
     * the RFPHY init inside lmac154 sets up TX *and RX* demodulator
     * paths based on the current bz_mode. If we leave bz_mode in the
     * default BLE-only state, the RX demodulator gets configured for
     * BLE and never recovers any 802.15.4 frame, even after a later
     * rf_set_bz_mode(ZB_ONLY). Symptom: rx_done=0 forever, only TX
     * works. Verified against the working ipro_ieee154_sed_lp
     * pattern (sed_app.c sets bz_mode/offsets before ieee802154_init).
     *
     * auto_ack MUST be true: with a designated initializer any field
     * omitted is zero, which would route ieee802154_hal_configure()
     * through lmac154_disableHwAutoTxAck() and the radio stops
     * sending MAC ACKs entirely.
     */
    extern void rf_set_bz_mode(uint8_t mode);
    extern void rf_set_pwr_offset_zb(int8_t tx_pwr_os[16]);
    extern void rf_optimize_by_channel(uint32_t channel_mhz);
    extern uint8_t rf_set_tx_power_zb(int32_t target_power);

    rf_set_bz_mode(1 /* ZB_ONLY */);
    {
        int8_t zero_zb_offsets[16] = {0};
        rf_set_pwr_offset_zb(zero_zb_offsets);
    }

    ieee802154_config_t ieee154_config = {
        .channel       = CONFIG_THREAD_DEFAULT_CHANNEL,
        .pan_id        = 0xFFFF,    /* OT will overwrite */
        .short_addr    = 0xFFFF,    /* OT will overwrite */
        .tx_power      = 0,         /* dBm */
        .auto_ack      = true,
        /* CARRIER_AND_ENERGY (lmac maps to LMAC154_CCA_MODE_ED_AND_CS,
         * register value 2 — IEEE 802.15.4 Mode 3). Verified
         * 2026-04-30: ED-only mode (IEEE802154_CCA_MODE_ENERGY → reg
         * value 0) reports false channel-busy ~60% on this chip even
         * on a clean channel, blocking Thread attach with constant
         * CSMA failures. ED_AND_CS requires BOTH energy AND carrier
         * to declare busy, which matches what other vendor stacks
         * use as default. Don't change this without re-verifying
         * with `radio_stats` — see memory/ipro7_lmac154_csma_broken. */
        .cca_mode      = IEEE802154_CCA_MODE_CARRIER_AND_ENERGY,
        .cca_threshold = -75,
    };
    ieee802154_error_t err = ieee802154_init(&ieee154_config);
    if (err != IEEE802154_OK && err != IEEE802154_ERROR_ALREADY_INITIALIZED) {
        printf("Driver init failed: %d", err);
    }

    /* After ieee802154_init the RFPHY is up; recompute per-channel TX
     * offsets and apply target TX power so the PA lands on the right
     * bias. Same belt-and-braces pattern as sed_app.c. */
    rf_optimize_by_channel(2405u + 5u * ((uint32_t)CONFIG_THREAD_DEFAULT_CHANNEL - 11u));
    (void)rf_set_tx_power_zb((int32_t)0);
#endif
    // Print banner
    printf("\r\n");
    printf("=================================================\r\n");
    printf("  IPRO Thread FTD CLI Application\r\n");
    printf("=================================================\r\n");
    printf("  Build: %s %s\r\n", __DATE__, __TIME__);
    printf("  OpenThread Full Thread Device with CLI\r\n");
    printf("=================================================\r\n");
    printf("\r\n");

    // Create Thread FTD initialization task (will run after scheduler starts)
    if (thread_ftd_app_init() != 0) {
        printf("[ERROR] Failed to create Thread FTD init task\r\n");
        return -1;
    }

    // Start FreeRTOS scheduler first
    // Thread FTD initialization happens in thread_ftd_init_task after scheduler starts

    printf("[INFO] Starting FreeRTOS scheduler...\r\n");
    vTaskStartScheduler();

    // Should never reach here
    printf("[ERROR] Scheduler failed to start!\r\n");
    return -1;
}
