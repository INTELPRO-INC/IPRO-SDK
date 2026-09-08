/**
 * @file pir_image_service.h
 * @brief PIR Motion Detection + Image Transfer Service Header
 * 
 * Features:
 * - PIR sensor GPIO interrupt detection
 * - Motion event notification to phone
 * - Image capture and transfer over BLE
 * - Chunked data transfer protocol for large images
 * 
 * Copyright (C) IPRO SDK 2025
 */

#ifndef PIR_IMAGE_SERVICE_H
#define PIR_IMAGE_SERVICE_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declaration */
struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * CONFIGURATION
 ****************************************************************************************
 */

/// Default PIR sensor GPIO pin (can be overridden in proj.conf)
#ifndef CONFIG_PIR_GPIO_PIN
#define CONFIG_PIR_GPIO_PIN         10
#endif

/// Maximum image chunk size (for buffer allocation)
/// Note: Actual chunk size is dynamically calculated based on negotiated MTU
///       Notification format: [chunk_index(2)] + [chunk_size(2)] + [data(payload)] 
///       Total packet = 4 (header) + payload
///       Safe calculation: payload = MTU - 3 (ATT) - 4 (header) - 4 (margin) = MTU - 11
///       Default MTU 247: 236 bytes payload → 240 bytes total packet
///       MTU 517: 506 bytes payload → 510 bytes total packet (phone compatible)
///       
///       Measured performance: 101.4 KB/s (811 Kbps) for 56KB image transfer
#define MAX_IMAGE_CHUNK_SIZE        512

/// Maximum image size (e.g., 64KB for small JPEG)
#define IMAGE_MAX_SIZE              (64 * 1024)

/// Simulated image size for demo (when no real camera)
#define DEMO_IMAGE_SIZE             (8 * 1024)

/*
 * SERVICE UUIDs (16-bit custom UUIDs)
 ****************************************************************************************
 */

/// PIR Image Service UUID
#define PIR_IMG_SVC_UUID16          0xFF50

/// PIR Status Characteristic - Read/Notify (motion detected)
#define PIR_STATUS_CHAR_UUID16      0xFF51

/// Image Control Characteristic - Write (start capture, request chunk)
#define IMG_CTRL_CHAR_UUID16        0xFF52

/// Image Data Characteristic - Notify (image chunks)
#define IMG_DATA_CHAR_UUID16        0xFF53

/// Image Info Characteristic - Read/Notify (image metadata)
#define IMG_INFO_CHAR_UUID16        0xFF54

/// Phone Image Receiver Characteristic - Write Without Response (for high-speed transfer)
/// Note: Phone app needs to provide this characteristic for Write mode
#define PHONE_IMG_RX_CHAR_UUID16    0xFF55

/*
 * TRANSFER MODES
 ****************************************************************************************
 */

/// Image transfer mode
typedef enum {
    IMG_TRANSFER_NOTIFICATION = 0,  ///< Standard notification (36-40 Kbps, reliable)
    IMG_TRANSFER_WRITE_NO_RESP = 1, ///< Write without response (120-150 Kbps, requires phone support)
} img_transfer_mode_t;

/*
 * DATA STRUCTURES
 ****************************************************************************************
 */

/// PIR motion event types
typedef enum {
    PIR_EVENT_NONE = 0,
    PIR_EVENT_MOTION_DETECTED,      ///< Motion detected (rising edge)
    PIR_EVENT_MOTION_CLEARED,       ///< Motion cleared (falling edge)
} pir_event_t;

/// PIR status data (notification format)
typedef struct __attribute__((packed)) {
    uint8_t motion_detected;        ///< 1=motion, 0=clear
    uint32_t timestamp;             ///< Timestamp in ms
    uint32_t event_count;           ///< Total motion events since boot
} pir_status_t;

/// Image control commands
typedef enum {
    IMG_CTRL_CAPTURE = 0x01,        ///< Trigger image capture
    IMG_CTRL_REQUEST_INFO = 0x02,   ///< Request image info
    IMG_CTRL_REQUEST_CHUNK = 0x03,  ///< Request specific chunk
    IMG_CTRL_ABORT = 0x04,          ///< Abort current transfer
} img_ctrl_cmd_t;

/// Image control packet (write format)
typedef struct __attribute__((packed)) {
    uint8_t command;                ///< img_ctrl_cmd_t
    union {
        uint16_t chunk_index;       ///< For REQUEST_CHUNK
        uint8_t params[3];          ///< Generic parameters
    };
} img_ctrl_packet_t;

/// Image info data (notification format)
typedef struct __attribute__((packed)) {
    uint32_t image_size;            ///< Total image size in bytes
    uint16_t chunk_count;           ///< Total number of chunks
    uint16_t chunk_size;            ///< Size of each chunk (except last)
    uint8_t format;                 ///< Image format (0=JPEG, 1=RAW)
    uint8_t status;                 ///< 0=ready, 1=capturing, 2=error
} img_info_t;

/// Image chunk header (notification format)
typedef struct __attribute__((packed)) {
    uint16_t chunk_index;           ///< Current chunk index (0-based)
    uint16_t chunk_size;            ///< Actual data size in this chunk
    uint8_t data[];                 ///< Chunk data
} img_chunk_header_t;

/// Image transfer state
typedef enum {
    IMG_STATE_IDLE = 0,
    IMG_STATE_CAPTURING,
    IMG_STATE_READY,
    IMG_STATE_TRANSFERRING,
    IMG_STATE_ERROR,
} img_state_t;

/*
 * SERVICE EVENT DEFINITIONS
 ****************************************************************************************
 */

/// PIR Image service events
typedef enum {
    PIR_IMG_EVENT_MOTION_DETECTED = 0,
    PIR_IMG_EVENT_MOTION_CLEARED,
    PIR_IMG_EVENT_CAPTURE_REQUEST,
    PIR_IMG_EVENT_TRANSFER_START,
    PIR_IMG_EVENT_TRANSFER_COMPLETE,
    PIR_IMG_EVENT_TRANSFER_ABORT,
    PIR_IMG_EVENT_NOTIFY_ENABLED,
    PIR_IMG_EVENT_NOTIFY_DISABLED,
} pir_img_event_t;

/// Event data structure
typedef struct {
    pir_img_event_t event;
    uint8_t conn_id;
    union {
        pir_status_t pir;
        img_info_t img_info;
        uint16_t chunk_index;
    } data;
} pir_img_event_data_t;

/// Event callback function type
typedef void (*pir_img_callback_t)(const pir_img_event_data_t *event, void *user_data);

/*
 * SERVICE HANDLE STRUCTURE
 ****************************************************************************************
 */

/// Service handles
typedef struct {
    uint16_t svc_handle;
    uint16_t pir_status_handle;
    uint16_t pir_status_cccd_handle;
    uint16_t img_ctrl_handle;
    uint16_t img_data_handle;
    uint16_t img_data_cccd_handle;
    uint16_t img_info_handle;
    uint16_t img_info_cccd_handle;
    
    bool pir_notify_enabled;
    bool img_data_notify_enabled;
    bool img_info_notify_enabled;
} pir_img_handles_t;

/*
 * API FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize PIR Image service
 * 
 * @param callback Event callback function
 * @param user_data User data for callback
 * @return int Status code (0 on success)
 */
int pir_image_service_init(pir_img_callback_t callback,
                          void *user_data);

/**
 * @brief Deinitialize service
 * 
 * @return int Status code (0 on success)
 */
int pir_image_service_deinit(void);

/**
 * @brief Get service handles
 * 
 * @return const pir_img_handles_t* Pointer to handles structure
 */
const pir_img_handles_t *pir_image_service_get_handles(void);

/**
 * @brief Start PIR sensor monitoring
 * 
 * @param gpio_pin GPIO pin number for PIR sensor
 * @return int Status code (0 on success)
 */
int pir_sensor_start(uint8_t gpio_pin);

/**
 * @brief Stop PIR sensor monitoring
 * 
 * @return int Status code (0 on success)
 */
int pir_sensor_stop(void);

/**
 * @brief Get current PIR status
 * 
 * @param status Output status structure
 * @return int Status code (0 on success)
 */
int pir_get_status(pir_status_t *status);

/**
 * @brief Send PIR motion notification
 * 
 * @param conn_id Connection ID
 * @param status PIR status data
 * @return int Status code (0 on success)
 */
int pir_notify_motion(uint8_t conn_id, const pir_status_t *status);

/**
 * @brief Simulate PIR trigger (for testing without hardware)
 * 
 * @return int Status code (0 on success)
 */
int pir_simulate_trigger(void);

/**
 * @brief Set transfer rate based on BLE connection interval
 * 
 * Adjusts chunk sending interval to match BLE connection parameters
 * for optimal throughput without overwhelming the BLE stack.
 * 
 * @param conn_interval_ms Connection interval in milliseconds
 *                         Typical values: 7.5ms to 4000ms
 *                         - Fast: 7.5-20ms (higher throughput)
 *                         - Balanced: 30-50ms (good balance)
 *                         - Power saving: 100ms+ (slower but efficient)
 * @return int Status code (0 on success)
 * 
 * @note Default is 15ms (2x typical 7.5ms connection interval)
 *       Call after connection established with actual parameters
 */
int pir_image_set_transfer_rate(uint16_t conn_interval_ms);

/**
 * @brief Set image transfer mode
 * 
 * @param mode Transfer mode (notification or write without response)
 * @param phone_char_handle Handle of phone's RX characteristic (only for write mode)
 * @return int Status code (0 on success)
 * 
 * @note For IMG_TRANSFER_WRITE_NO_RESP mode:
 *       - Phone must provide GATT Server with writable characteristic
 *       - Characteristic UUID: PHONE_IMG_RX_CHAR_UUID16 (0xFF55)
 *       - Properties: WRITE_WITHOUT_RESPONSE
 *       - Max length: MAX_IMAGE_CHUNK_SIZE bytes (dynamically sized based on MTU)
 *       - Expected 3-4x speed improvement over notification
 */
int pir_image_set_transfer_mode(img_transfer_mode_t mode, uint16_t phone_char_handle);

/**
 * @brief Set connection ID for auto-transfer
 * 
 * @param conn_id Connection ID (0 to disable auto-transfer)
 * @return int Status code (0 on success)
 */
int pir_image_set_conn_id(uint8_t conn_id);
/*
 * PUBLIC FUNCTIONS - Image Type Selection
 ****************************************************************************************
 */

/**
 * @brief Image types for selection
 */
typedef enum {
    PIR_IMAGE_TYPE_DEFAULT = 0,  ///< Default image (44.5 KB)
    PIR_IMAGE_TYPE_CAT = 1,      ///< Cat image (55 KB)
} pir_image_type_t;

/**
 * @brief Set the image type to capture
 * @param type Image type
 */
void image_set_type(pir_image_type_t type);

/**
 * @brief Get current image type
 * @return Current image type
 */
pir_image_type_t image_get_type(void);

/*
 * PUBLIC FUNCTIONS - Image Capture
 ****************************************************************************************
 */
/**
 * @brief Capture image (or generate demo image)
 * 
 * Uses actual camera if available, otherwise generates demo pattern
 * 
 * @return int Status code (0 on success)
 */
int image_capture(void);

/**
 * @brief Set image data (for external image source)
 * 
 * @param data Image data buffer
 * @param size Image size in bytes
 * @return int Status code (0 on success)
 */
int image_set_data(const uint8_t *data, uint32_t size);

/**
 * @brief Get current image info
 * 
 * @param info Output info structure
 * @return int Status code (0 on success)
 */
int image_get_info(img_info_t *info);

/**
 * @brief Send image info notification
 * 
 * @param conn_id Connection ID
 * @return int Status code (0 on success)
 */
int image_notify_info(uint8_t conn_id);

/**
 * @brief Send image chunk
 * 
 * @param conn_id Connection ID
 * @param chunk_index Chunk index (0-based)
 * @return ipro_ble_status_t Status code, -2 if flow control busy
 */
int image_send_chunk(uint8_t conn_id, uint16_t chunk_index);

/**
 * @brief Start auto-send all chunks
 * 
 * @param conn_id Connection ID
 * @return int Status code (0 on success)
 */
int image_start_transfer(uint8_t conn_id);

/**
 * @brief Abort current image transfer
 * 
 * @return int Status code (0 on success)
 */
int image_abort_transfer(void);

/**
 * @brief Process pending image transfer (call from main loop)
 * 
 * Sends next chunk if ready and not flow controlled
 */
void image_transfer_process(void);

/**
 * @brief Check if image transfer is in progress
 * 
 * @return true if transfer is ongoing
 */
bool image_transfer_in_progress(void);

/**
 * @brief Set connection reference for notifications
 *
 * @param conn BLE connection pointer (NULL to clear)
 */
void pir_image_service_set_conn(struct bt_conn *conn);

#ifdef __cplusplus
}
#endif

#endif // PIR_IMAGE_SERVICE_H
