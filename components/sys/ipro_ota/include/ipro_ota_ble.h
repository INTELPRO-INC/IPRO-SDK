/**
 * @file ota_service.h
 * @brief BLE OTA (Over-The-Air) Firmware Update Service
 * 
 * Provides secure firmware update capability over BLE connection.
 * Uses dual-partition boot mechanism with automatic rollback support.
 * 
 * Features:
 * - Firmware data transfer via BLE characteristics
 * - CRC32 integrity verification
 * - Partition management (active/backup switching)
 * - Progress reporting
 * - Automatic reboot after successful update
 * 
 * Copyright (C) IPRO SDK 2025
 */

#ifndef OTA_SERVICE_H
#define OTA_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declaration */
struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * OTA SERVICE UUIDs (Custom 16-bit UUIDs)
 ****************************************************************************************
 */

#define OTA_SVC_UUID16              0xFF60      ///< OTA Service UUID
#define OTA_CTRL_CHAR_UUID16        0xFF61      ///< Control characteristic (Write)
#define OTA_DATA_CHAR_UUID16        0xFF62      ///< Data characteristic (Write Without Response)
#define OTA_STATUS_CHAR_UUID16      0xFF63      ///< Status characteristic (Read/Notify)

/*
 * OTA CONTROL COMMANDS
 ****************************************************************************************
 */

/// OTA Control Commands (written to OTA_CTRL characteristic)
typedef enum {
    OTA_CMD_START = 0x01,           ///< Start OTA session (payload: firmware size)
    OTA_CMD_END = 0x02,             ///< End OTA session (payload: CRC32)
    OTA_CMD_ABORT = 0x03,           ///< Abort current OTA session
    OTA_CMD_VERIFY = 0x04,          ///< Verify firmware integrity
    OTA_CMD_REBOOT = 0x05,          ///< Reboot to new firmware
} ota_cmd_t;

/*
 * OTA STATUS CODES
 ****************************************************************************************
 */

/// OTA Status Codes (notified via OTA_STATUS characteristic)
typedef enum {
    OTA_STATUS_IDLE = 0x00,         ///< No active OTA session
    OTA_STATUS_RECEIVING = 0x01,    ///< Receiving firmware data
    OTA_STATUS_VERIFYING = 0x02,    ///< Verifying firmware
    OTA_STATUS_READY = 0x03,        ///< Ready to reboot
    OTA_STATUS_COMPLETE = 0x04,     ///< OTA completed successfully
    OTA_STATUS_RETRANSMIT_REQ = 0x10, ///< Requesting retransmission of missing packets
    OTA_STATUS_ERROR = 0xFF,        ///< Error occurred
} ota_status_t;

/// OTA Error Codes (sub-status when OTA_STATUS_ERROR)
typedef enum {
    OTA_ERR_NONE = 0x00,            ///< No error
    OTA_ERR_INVALID_SIZE = 0x01,    ///< Invalid firmware size
    OTA_ERR_WRITE_FAILED = 0x02,    ///< Flash write failed
    OTA_ERR_ERASE_FAILED = 0x03,    ///< Flash erase failed
    OTA_ERR_CRC_FAILED = 0x04,      ///< CRC verification failed
    OTA_ERR_NO_SPACE = 0x05,        ///< Insufficient partition space
    OTA_ERR_BUSY = 0x06,            ///< OTA already in progress
    OTA_ERR_INVALID_STATE = 0x07,   ///< Invalid state for command
    OTA_ERR_PARTITION = 0x08,       ///< Partition table error
    OTA_ERR_INCOMPLETE = 0x09,      ///< Incomplete transfer (missing packets)
    OTA_ERR_VERIFY_FAILED = 0x0A,   ///< Firmware verification failed
    OTA_ERR_UPDATE_FAILED = 0x0B,   ///< Partition update failed
} ota_error_t;

/*
 * OTA STATUS NOTIFICATION FORMAT
 ****************************************************************************************
 */

/// OTA Status notification structure (5 bytes)
typedef struct __attribute__((packed)) {
    uint8_t status;                 ///< OTA status (ota_status_t)
    uint8_t error;                  ///< Error code (ota_error_t)
    uint8_t progress;               ///< Progress percentage (0-100)
    uint16_t reserved;              ///< Reserved for future use
} ota_status_notif_t;

/*
 * OTA CONTROL COMMAND FORMATS
 ****************************************************************************************
 */

/// OTA START command payload (5 bytes)
typedef struct __attribute__((packed)) {
    uint8_t cmd;                    ///< OTA_CMD_START
    uint32_t firmware_size;         ///< Total firmware size in bytes
} ota_start_cmd_t;

/// OTA END command payload (5 bytes)
typedef struct __attribute__((packed)) {
    uint8_t cmd;                    ///< OTA_CMD_END
    uint32_t crc32;                 ///< Expected CRC32 checksum
} ota_end_cmd_t;

/*
 * OTA SERVICE CONFIGURATION
 ****************************************************************************************
 */

/// Maximum firmware size (adjust based on partition size)
#define OTA_MAX_FIRMWARE_SIZE       (800 * 1024)    ///< 800KB max firmware size

/// OTA packet size (BLE MTU - 3 bytes overhead)
#define OTA_DATA_PACKET_SIZE        244             ///< Max data per packet (MTU=247)

/*
 * OTA SERVICE CALLBACKS
 ****************************************************************************************
 */

/// OTA event callback function type
typedef void (*ota_event_cb_t)(ota_status_t status, ota_error_t error, uint8_t progress);

/*
 * OTA SERVICE API
 ****************************************************************************************
 */

/**
 * @brief Initialize OTA service
 * 
 * Initializes OTA context and prepares for service registration.
 * 
 * @param event_cb Optional callback for OTA status events (can be NULL)
 * @return 0 on success, negative error code otherwise
 */
int ota_service_init(ota_event_cb_t event_cb);

/**
 * @brief Register OTA GATT service
 * 
 * Registers OTA GATT service with BLE stack.
 * Must be called after BLE initialization.
 * 
 * @return 0 on success, negative error code otherwise
 */
int ota_service_register_zephyr(void);

/**
 * @brief Get current OTA status
 * 
 * @param status Output pointer for current status
 * @param error Output pointer for error code
 * @param progress Output pointer for progress percentage
 * @return 0 on success, negative error code otherwise
 */
int ota_service_get_status(ota_status_t *status, ota_error_t *error, uint8_t *progress);

/**
 * @brief Abort current OTA session
 * 
 * Cleans up any ongoing OTA transfer and resets state.
 * 
 * @return 0 on success, negative error code otherwise
 */
int ota_service_abort(void);

/**
 * @brief Notify connected device of status change
 * 
 * Sends OTA status notification to connected BLE device.
 * 
 * @param conidx Connection index
 * @return 0 on success, negative error code otherwise
 */
int ota_service_notify_status(uint8_t conidx);

/**
 * @brief Update OTA service with new MTU size
 * @param mtu New MTU value (negotiated)
 * @note Should be called when MTU changes to dynamically adjust packet size
 */
void ota_service_update_mtu(uint16_t mtu);

/**
 * @brief Set connection reference for notifications
 *
 * @param conn BLE connection pointer (NULL to clear)
 */
void ota_service_set_conn(struct bt_conn *conn);

/*
 * INTERNAL STRUCTURES (exposed for testing/debugging)
 ****************************************************************************************
 */

/// OTA session context
typedef struct {
    bool active;                    ///< OTA session active flag
    uint32_t firmware_size;         ///< Total firmware size
    uint32_t bytes_received;        ///< Bytes received so far
    uint32_t flash_offset;          ///< Current flash write offset
    uint32_t backup_partition_addr; ///< Backup partition flash address
    uint32_t backup_partition_size; ///< Backup partition size
    uint32_t calculated_crc32;      ///< Calculated CRC32 of received data
    uint32_t expected_crc32;        ///< Expected CRC32 from END command
    ota_status_t status;            ///< Current OTA status
    ota_error_t error;              ///< Current error code
    uint8_t progress;               ///< Progress percentage (0-100)
    uint8_t conidx;                 ///< Connected device index
    ota_event_cb_t event_cb;        ///< Event callback
} ota_context_t;

/**
 * @brief Get OTA context (for debugging)
 * 
 * @return Pointer to internal OTA context
 */
const ota_context_t *ota_get_context(void);

#ifdef __cplusplus
}
#endif

#endif /* OTA_SERVICE_H */
