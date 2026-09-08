/**
 * @file image_upload_service.h
 * @brief Image Upload Service Header - Receive images from phone via BLE
 *
 * Features:
 * - Receive images from phone in chunks
 * - Flow control with PAUSE/RESUME
 * - Integration with PIR Image Service for retrieval
 *
 * Protocol:
 * - Phone sends START with image size
 * - Device responds with READY and suggested chunk size
 * - Phone sends data chunks with index
 * - Device sends PAUSE/RESUME for flow control
 * - Device sends COMPLETE when all chunks received
 * - Image available via PIR service (0xFF53)
 *
 * Copyright (C) IPRO SDK 2025
 */

#ifndef IMAGE_UPLOAD_SERVICE_H
#define IMAGE_UPLOAD_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declaration */
struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SERVICE UUIDs (16-bit custom UUIDs)
 ****************************************************************************************
 */

/// Image Upload Service UUID
#define IMG_UPLOAD_SVC_UUID16           0xFF70

/// Upload Control Characteristic - Write/Notify (commands and responses)
#define UPLOAD_CTRL_CHAR_UUID16         0xFF71

/// Upload Data Characteristic - Write Without Response (image data chunks)
#define UPLOAD_DATA_CHAR_UUID16         0xFF72

/// Upload Status Characteristic - Read/Notify (progress and state)
#define UPLOAD_STATUS_CHAR_UUID16       0xFF73

/*
 * PROTOCOL DEFINITIONS
 ****************************************************************************************
 */

/// Maximum image size (must match PIR service)
#define UPLOAD_IMAGE_MAX_SIZE           (64 * 1024)

/// Upload control commands (Phone -> Device)
typedef enum {
    UPLOAD_CMD_START = 0x01,            ///< Begin upload, payload: 4 bytes (size LE32)
    UPLOAD_CMD_ABORT = 0x02,            ///< Abort current upload
} upload_cmd_t;

/// Upload control responses (Device -> Phone)
typedef enum {
    UPLOAD_RESP_READY    = 0x10,        ///< Ready to receive, payload: 2 bytes (chunk_size LE16)
    UPLOAD_RESP_PAUSE    = 0x11,        ///< Pause sending (buffer processing)
    UPLOAD_RESP_RESUME   = 0x12,        ///< Resume sending
    UPLOAD_RESP_COMPLETE = 0x13,        ///< Upload complete, image in PIR buffer
    UPLOAD_RESP_ERROR    = 0x1F,        ///< Error occurred, payload: 1 byte (error_code)
} upload_resp_t;

/// Upload error codes
typedef enum {
    UPLOAD_ERR_NONE         = 0x00,     ///< No error
    UPLOAD_ERR_SIZE         = 0x01,     ///< Image size exceeds limit
    UPLOAD_ERR_BUSY         = 0x02,     ///< PIR transfer in progress
    UPLOAD_ERR_CHUNK_IDX    = 0x03,     ///< Wrong chunk index
    UPLOAD_ERR_NO_START     = 0x04,     ///< Data received before START
    UPLOAD_ERR_ALLOC        = 0x05,     ///< Memory allocation failed
} upload_err_t;

/*
 * DATA STRUCTURES
 ****************************************************************************************
 */

/// Upload state
typedef enum {
    UPLOAD_STATE_IDLE = 0,              ///< Ready to receive START command
    UPLOAD_STATE_RECEIVING,             ///< Receiving data chunks
} upload_state_t;

/// Upload status data (Read characteristic format)
typedef struct __attribute__((packed)) {
    uint8_t state;                      ///< upload_state_t
    uint32_t total_size;                ///< Total image size
    uint32_t received_size;             ///< Bytes received so far
    uint16_t chunk_count;               ///< Total chunks expected
    uint16_t chunks_received;           ///< Chunks received so far
} upload_status_t;

/// Upload data chunk format
typedef struct __attribute__((packed)) {
    uint16_t chunk_index;               ///< Chunk index (0-based, LE16)
    uint8_t data[];                     ///< Chunk data
} upload_chunk_t;

/*
 * SERVICE EVENT DEFINITIONS
 ****************************************************************************************
 */

/// Upload service events
typedef enum {
    UPLOAD_EVENT_STARTED = 0,           ///< Upload started
    UPLOAD_EVENT_PROGRESS,              ///< Progress update
    UPLOAD_EVENT_COMPLETE,              ///< Upload complete
    UPLOAD_EVENT_ABORTED,               ///< Upload aborted
    UPLOAD_EVENT_ERROR,                 ///< Error occurred
} upload_event_t;

/// Event data structure
typedef struct {
    upload_event_t event;
    uint8_t conn_id;
    union {
        uint32_t total_size;            ///< For STARTED
        uint32_t received_size;         ///< For PROGRESS
        upload_err_t error;             ///< For ERROR
    } data;
} upload_event_data_t;

/// Event callback function type
typedef void (*upload_callback_t)(const upload_event_data_t *event, void *user_data);

/*
 * SERVICE HANDLE STRUCTURE
 ****************************************************************************************
 */

/// Service handles
typedef struct {
    uint16_t svc_handle;
    uint16_t ctrl_handle;
    uint16_t ctrl_cccd_handle;
    uint16_t data_handle;
    uint16_t status_handle;
    uint16_t status_cccd_handle;

    bool ctrl_notify_enabled;
    bool status_notify_enabled;
} upload_handles_t;

/*
 * API FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize Image Upload service
 *
 * @param callback Event callback function (optional)
 * @param user_data User data for callback
 * @return int Status code (0 on success)
 */
int image_upload_service_init(upload_callback_t callback,
                              void *user_data);

/**
 * @brief Handle BLE connection event
 *
 * @param conn_id Connection ID
 */
void image_upload_on_connect(uint8_t conn_id);

/**
 * @brief Handle BLE disconnection event
 *
 * @param conn_id Connection ID
 */
void image_upload_on_disconnect(uint8_t conn_id);

/**
 * @brief Get current upload status
 *
 * @param status Output status structure
 * @return int Status code (0 on success)
 */
int image_upload_get_status(upload_status_t *status);

/**
 * @brief Get service handles
 *
 * @return const upload_handles_t* Pointer to handles structure
 */
const upload_handles_t *image_upload_get_handles(void);

/**
 * @brief Set connection reference for notifications
 *
 * @param conn BLE connection pointer (NULL to clear)
 */
void image_upload_set_conn(struct bt_conn *conn);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_UPLOAD_SERVICE_H
