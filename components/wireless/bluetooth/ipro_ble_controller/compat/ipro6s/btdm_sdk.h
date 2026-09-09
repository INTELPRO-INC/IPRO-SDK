/**
 * @file btdm_sdk.h
 * @brief BTDM SDK Public API
 *
 * This is the unified public API for the IPRO6 BTDM (Bluetooth Dual Mode) controller.
 * Currently implements BLE Controller only (HCI interface).
 *
 * Copyright (c) 2024 IPRO
 */

#ifndef BTDM_SDK_H
#define BTDM_SDK_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BTDM Error Codes
 */
#define BTDM_OK                     0
#define BTDM_ERR_INVALID_PARAM      -1
#define BTDM_ERR_NOT_INITIALIZED    -2
#define BTDM_ERR_ALREADY_INIT       -3
#define BTDM_ERR_NO_MEMORY          -4
#define BTDM_ERR_BUSY               -5
#define BTDM_ERR_TIMEOUT            -6
#define BTDM_ERR_HCI_ERROR          -7
#define BTDM_ERR_NOT_SUPPORTED      -8

/*
 * BLE Constants
 */
#define BTDM_BD_ADDR_LEN            6
#define BTDM_MAX_ADV_DATA_LEN       31
#define BTDM_MAX_SCAN_RSP_LEN       31

/*
 * BLE PHY Types
 */
typedef enum {
    BTDM_PHY_1M = 0x01,
    BTDM_PHY_2M = 0x02,
    BTDM_PHY_CODED = 0x03,
} btdm_phy_t;

/*
 * Advertising Types
 */
typedef enum {
    BTDM_ADV_TYPE_IND = 0x00,
    BTDM_ADV_TYPE_DIRECT_IND_HIGH = 0x01,
    BTDM_ADV_TYPE_SCAN_IND = 0x02,
    BTDM_ADV_TYPE_NONCONN_IND = 0x03,
    BTDM_ADV_TYPE_DIRECT_IND_LOW = 0x04,
} btdm_adv_type_t;

/*
 * Scan Types
 */
typedef enum {
    BTDM_SCAN_TYPE_PASSIVE = 0x00,
    BTDM_SCAN_TYPE_ACTIVE = 0x01,
} btdm_scan_type_t;

/*
 * BTDM Configuration
 */
typedef struct {
    uint8_t bd_addr[BTDM_BD_ADDR_LEN];  /**< Bluetooth Device Address */
    int8_t  tx_power_dbm;               /**< Default TX power in dBm */
    uint8_t max_connections;            /**< Maximum ACL connections */
    bool    sleep_enable;               /**< Enable deep sleep */
} btdm_config_t;

/*
 * ============================================================================
 * BTDM Core API
 * ============================================================================
 */

/**
 * @brief Initialize BTDM controller
 *
 * @param config Configuration parameters (NULL for defaults)
 * @return 0 on success, negative error code on failure
 */
int btdm_init(const btdm_config_t *config);

/**
 * @brief Deinitialize BTDM controller
 *
 * @return 0 on success, negative error code on failure
 */
int btdm_deinit(void);

/**
 * @brief Check if BTDM is initialized
 *
 * @return true if initialized
 */
bool btdm_is_initialized(void);

/**
 * @brief Print BTDM controller status
 */
void btdm_print_status(void);

/*
 * ============================================================================
 * HCI API
 * ============================================================================
 */

/**
 * @brief Send HCI Reset command
 *
 * @return 0 on success, negative error code on failure
 */
int btdm_hci_reset(void);

/**
 * @brief Start LE TX test (v4)
 *
 * @param channel BLE channel (0-39)
 * @param length Payload length (0-255)
 * @param payload Payload type (0-7)
 * @param phy PHY type (1=1M, 2=2M, 3=Coded S=8, 4=Coded S=2)
 * @param tx_power TX power in dBm (-127~+20), 0x7E=min, 0x7F=max
 * @return 0 on success, negative error code on failure
 */
int btdm_hci_le_tx_test(uint8_t channel, uint8_t length, uint8_t payload,
                        uint8_t phy, int8_t tx_power);

/**
 * @brief Start LE RX test (v2)
 *
 * @param channel BLE channel (0-39)
 * @param phy PHY type (1=1M, 2=2M, 3=Coded)
 * @param mod_idx Modulation index (0=standard, 1=stable)
 * @return 0 on success, negative error code on failure
 */
int btdm_hci_le_rx_test(uint8_t channel, uint8_t phy, uint8_t mod_idx);

/**
 * @brief End LE test mode
 *
 * @param num_packets Pointer to store number of packets received
 * @return 0 on success, negative error code on failure
 */
int btdm_hci_le_test_end(uint16_t *num_packets);

/*
 * ============================================================================
 * Configuration API
 * ============================================================================
 */

/**
 * @brief Set BD address
 *
 * @param addr 6-byte BD address (LSB first)
 * @return 0 on success, negative error code on failure
 */
int btdm_set_bd_addr(const uint8_t addr[BTDM_BD_ADDR_LEN]);

/**
 * @brief Get BD address
 *
 * @param addr Buffer to store 6-byte BD address (LSB first)
 * @return 0 on success, negative error code on failure
 */
int btdm_get_bd_addr(uint8_t addr[BTDM_BD_ADDR_LEN]);

/**
 * @brief Set TX power
 *
 * @param power_dbm TX power in dBm
 * @return 0 on success, negative error code on failure
 */
int btdm_set_tx_power(int8_t power_dbm);

/**
 * @brief Get current TX power
 *
 * @return TX power in dBm
 */
int8_t btdm_get_tx_power(void);

/*
 * ============================================================================
 * Sleep/Power Management API
 * ============================================================================
 */

/**
 * @brief Enter sleep mode
 *
 * @param sleep_duration_ms Sleep duration in milliseconds
 * @return Actual sleep duration
 */
uint32_t btdm_sleep(uint32_t sleep_duration_ms);

/**
 * @brief Check if sleep is allowed
 *
 * @return true if sleep is allowed
 */
bool btdm_can_sleep(void);

/**
 * @brief Wakeup from sleep
 */
void btdm_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif /* BTDM_SDK_H */
