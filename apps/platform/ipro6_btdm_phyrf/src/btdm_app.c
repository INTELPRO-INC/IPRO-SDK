/**
 * @file btdm_app.c
 * @brief BTDM Application Logic
 *
 * This file contains the BTDM application initialization and
 * helper functions for the demo application.
 *
 * Copyright (c) 2024 IPRO
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <generated/autoconf.h>

#ifdef CONFIG_BTDM_ENABLE

#include "btdm_sdk.h"

/* Application state */
static struct {
    bool initialized;
    bool advertising;
    bool scanning;
    uint8_t adv_idx;
    uint8_t scan_idx;
} btdm_app_state = {0};

/**
 * @brief Get BTDM application state
 */
bool btdm_app_is_initialized(void)
{
    return btdm_app_state.initialized;
}

/**
 * @brief Get advertising state
 */
bool btdm_app_is_advertising(void)
{
    return btdm_app_state.advertising;
}

/**
 * @brief Get scanning state
 */
bool btdm_app_is_scanning(void)
{
    return btdm_app_state.scanning;
}

/**
 * @brief Set advertising state
 */
void btdm_app_set_advertising(bool state, uint8_t idx)
{
    btdm_app_state.advertising = state;
    btdm_app_state.adv_idx = idx;
}

/**
 * @brief Set scanning state
 */
void btdm_app_set_scanning(bool state, uint8_t idx)
{
    btdm_app_state.scanning = state;
    btdm_app_state.scan_idx = idx;
}

/**
 * @brief Initialize BTDM application
 */
int btdm_app_init(void)
{
    if (btdm_app_state.initialized) {
        printf("[BTDM_APP] Already initialized\r\n");
        return 0;
    }

    printf("[BTDM_APP] Initializing...\r\n");

    /* Initialize BTDM stack (re-init after btdm_reset is not an error) */
    int ret = btdm_init(NULL);
    if (ret != 0 && ret != BTDM_ERR_ALREADY_INIT) {
        printf("[BTDM_APP] BTDM stack init failed: %d\r\n", ret);
        return ret;
    }

    btdm_app_state.initialized = true;
    printf("[BTDM_APP] Initialized successfully\r\n");

    return 0;
}

/**
 * @brief Start BLE advertising
 */
int btdm_app_start_advertising(void)
{
    if (!btdm_app_state.initialized) {
        printf("[BTDM_APP] Not initialized\r\n");
        return -1;
    }

    if (btdm_app_state.advertising) {
        printf("[BTDM_APP] Already advertising\r\n");
        return 0;
    }

    printf("[BTDM_APP] Starting advertising...\r\n");

    /* TODO: Implement HCI advertising commands */
    /* This will be implemented when the controller is ported */

    btdm_app_state.advertising = true;
    printf("[BTDM_APP] Advertising started\r\n");

    return 0;
}

/**
 * @brief Stop BLE advertising
 */
int btdm_app_stop_advertising(void)
{
    if (!btdm_app_state.advertising) {
        printf("[BTDM_APP] Not advertising\r\n");
        return 0;
    }

    printf("[BTDM_APP] Stopping advertising...\r\n");

    /* TODO: Implement HCI advertising stop commands */

    btdm_app_state.advertising = false;
    printf("[BTDM_APP] Advertising stopped\r\n");

    return 0;
}

/**
 * @brief Start BLE scanning
 */
int btdm_app_start_scanning(void)
{
    if (!btdm_app_state.initialized) {
        printf("[BTDM_APP] Not initialized\r\n");
        return -1;
    }

    if (btdm_app_state.scanning) {
        printf("[BTDM_APP] Already scanning\r\n");
        return 0;
    }

    printf("[BTDM_APP] Starting scanning...\r\n");

    /* TODO: Implement HCI scanning commands */

    btdm_app_state.scanning = true;
    printf("[BTDM_APP] Scanning started\r\n");

    return 0;
}

/**
 * @brief Stop BLE scanning
 */
int btdm_app_stop_scanning(void)
{
    if (!btdm_app_state.scanning) {
        printf("[BTDM_APP] Not scanning\r\n");
        return 0;
    }

    printf("[BTDM_APP] Stopping scanning...\r\n");

    /* TODO: Implement HCI scanning stop commands */

    btdm_app_state.scanning = false;
    printf("[BTDM_APP] Scanning stopped\r\n");

    return 0;
}

/**
 * @brief Print BTDM status
 */
void btdm_app_print_status(void)
{
    printf("[BTDM_APP] Status:\r\n");
    printf("  Initialized: %s\r\n", btdm_app_state.initialized ? "Yes" : "No");
    printf("  Advertising: %s\r\n", btdm_app_state.advertising ? "Yes" : "No");
    printf("  Scanning:    %s\r\n", btdm_app_state.scanning ? "Yes" : "No");

    if (btdm_app_state.initialized) {
        btdm_print_status();
    }
}

/*
 * ============================================================================
 * Shared-RF glue (dual-CLI app only)
 * ============================================================================
 * Strong overrides of the weak btdm_port hooks: bridge the controller's DTM
 * enter/exit and TX power requests to the wifi rf driver, which owns the
 * shared RF front-end. Controller-only builds keep the weak no-ops.
 */
#ifdef CONFIG_PHY_CLI
void rf_bz_enter(void);
void rf_bz_exit(void);
void rf_pri_input_bz_target_power(int8_t target_pwr_dbm);

void btdm_rf_enter_bz(void)
{
    rf_bz_enter();
}

void btdm_rf_exit_bz(void)
{
    rf_bz_exit();
}

void btdm_rf_set_tx_power_dbm(int8_t power_dbm)
{
    rf_pri_input_bz_target_power(power_dbm);
}
#endif /* CONFIG_PHY_CLI */

#endif /* CONFIG_BTDM_ENABLE */
