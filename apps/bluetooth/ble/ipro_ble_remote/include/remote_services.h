/**
 * @file remote_services.h
 * @brief BLE Remote Control Services Header
 * 
 * Defines GATT services for mobile phone interaction:
 * - LED Control Service
 * - Button Service
 * - Environment Service
 * - UART Service
 * 
 * Copyright (C) IPRO SDK 2025
 */

#ifndef REMOTE_SERVICES_H
#define REMOTE_SERVICES_H

#include <stdint.h>
#include <stdbool.h>

/* Forward declaration */
struct bt_conn;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SERVICE UUIDs (16-bit custom UUIDs in reserved range 0xFF00-0xFFFF)
 ****************************************************************************************
 */

// LED Control Service
#define LED_SVC_UUID16              0xFF10
#define LED_STATE_CHAR_UUID16       0xFF11  // Read/Write - On/Off
#define LED_BRIGHT_CHAR_UUID16      0xFF12  // Read/Write - Brightness 0-100
#define LED_MODE_CHAR_UUID16        0xFF13  // Read/Write - Mode (solid/blink)

// Button Service
#define BTN_SVC_UUID16              0xFF20
#define BTN_STATE_CHAR_UUID16       0xFF21  // Read/Notify - Button state
#define BTN_COUNT_CHAR_UUID16       0xFF22  // Read - Press count

// Environment Service
#define ENV_SVC_UUID16              0xFF30
#define ENV_TEMP_CHAR_UUID16        0xFF31  // Read/Notify - Temperature
#define ENV_HUMID_CHAR_UUID16       0xFF32  // Read/Notify - Humidity

// UART Service
#define UART_SVC_UUID16             0xFF40
#define UART_TX_CHAR_UUID16         0xFF41  // Notify - Device to phone
#define UART_RX_CHAR_UUID16         0xFF42  // Write - Phone to device

/*
 * LED SERVICE DEFINITIONS
 ****************************************************************************************
 */

/// LED modes
typedef enum {
    LED_MODE_OFF = 0,
    LED_MODE_ON,
    LED_MODE_BLINK_SLOW,
    LED_MODE_BLINK_FAST,
} led_mode_t;

/// LED state structure
typedef struct {
    bool on;                ///< LED on/off state
    uint8_t brightness;     ///< Brightness 0-100
    led_mode_t mode;        ///< LED mode
} led_state_t;

/*
 * BUTTON SERVICE DEFINITIONS
 ****************************************************************************************
 */

/// Button events
typedef enum {
    BTN_EVENT_RELEASED = 0,
    BTN_EVENT_PRESSED,
    BTN_EVENT_LONG_PRESS,
    BTN_EVENT_DOUBLE_CLICK,
} btn_event_t;

/// Button state structure
typedef struct {
    btn_event_t event;      ///< Last button event
    uint32_t press_count;   ///< Total press count
} btn_state_t;

/*
 * ENVIRONMENT SERVICE DEFINITIONS
 ****************************************************************************************
 */

/// Environment data structure
typedef struct {
    int16_t temperature;    ///< Temperature in 0.01°C units (e.g., 2500 = 25.00°C)
    uint8_t humidity;       ///< Humidity 0-100%
} env_data_t;

/*
 * UART SERVICE DEFINITIONS
 ****************************************************************************************
 */

#define UART_MAX_DATA_LEN   244  ///< Maximum UART data length

/*
 * SERVICE EVENT DEFINITIONS
 ****************************************************************************************
 */

/// Remote service events
typedef enum {
    // LED events
    REMOTE_EVENT_LED_STATE_CHANGED = 0,
    REMOTE_EVENT_LED_BRIGHT_CHANGED,
    REMOTE_EVENT_LED_MODE_CHANGED,
    
    // Button events
    REMOTE_EVENT_BTN_NOTIFY_ENABLED,
    REMOTE_EVENT_BTN_NOTIFY_DISABLED,
    
    // Environment events
    REMOTE_EVENT_ENV_TEMP_NOTIFY_ENABLED,
    REMOTE_EVENT_ENV_TEMP_NOTIFY_DISABLED,
    REMOTE_EVENT_ENV_HUMID_NOTIFY_ENABLED,
    REMOTE_EVENT_ENV_HUMID_NOTIFY_DISABLED,
    
    // UART events
    REMOTE_EVENT_UART_TX_ENABLED,
    REMOTE_EVENT_UART_TX_DISABLED,
    REMOTE_EVENT_UART_RX_DATA,
} remote_event_t;

/// Remote service event data
typedef struct {
    remote_event_t event;
    uint8_t conn_id;
    union {
        led_state_t led;
        btn_state_t btn;
        env_data_t env;
        struct {
            const uint8_t *data;
            uint16_t length;
        } uart;
    } data;
} remote_event_data_t;

/// Event callback function type
typedef void (*remote_event_callback_t)(const remote_event_data_t *event, void *user_data);

/*
 * SERVICE HANDLE STRUCTURE
 ****************************************************************************************
 */

/// Service handles
typedef struct {
    // LED Service handles
    uint16_t led_svc_handle;
    uint16_t led_state_handle;
    uint16_t led_bright_handle;
    uint16_t led_mode_handle;
    
    // Button Service handles
    uint16_t btn_svc_handle;
    uint16_t btn_state_handle;
    uint16_t btn_state_cccd_handle;
    uint16_t btn_count_handle;
    
    // Environment Service handles
    uint16_t env_svc_handle;
    uint16_t env_temp_handle;
    uint16_t env_temp_cccd_handle;
    uint16_t env_humid_handle;
    uint16_t env_humid_cccd_handle;
    
    // UART Service handles
    uint16_t uart_svc_handle;
    uint16_t uart_tx_handle;
    uint16_t uart_tx_cccd_handle;
    uint16_t uart_rx_handle;
    
    // Notification states
    bool btn_notify_enabled;
    bool temp_notify_enabled;
    bool humid_notify_enabled;
    bool uart_notify_enabled;
} remote_svc_handles_t;

/*
 * API FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize all remote control services
 * 
 * @param callback Event callback function
 * @param user_data User data for callback
 * @return int Status code (0 on success)
 */
int remote_services_init(remote_event_callback_t callback,
                        void *user_data);

/**
 * @brief Deinitialize services
 * 
 * @return int Status code (0 on success)
 */
int remote_services_deinit(void);

/**
 * @brief Get service handles
 * 
 * @return const remote_svc_handles_t* Pointer to handles structure
 */
const remote_svc_handles_t *remote_services_get_handles(void);

/**
 * @brief Get current LED state
 * 
 * @param state Output LED state
 * @return int Status code (0 on success)
 */
int remote_get_led_state(led_state_t *state);

/**
 * @brief Set LED state (for local control)
 * 
 * @param state New LED state
 * @return int Status code (0 on success)
 */
int remote_set_led_state(const led_state_t *state);

/**
 * @brief Send button event notification
 * 
 * @param conn BT connection
 * @param event Button event
 * @return int Status code (0 on success)
 */
int remote_notify_button(struct bt_conn *conn, btn_event_t event);

/**
 * @brief Send environment data notification
 * 
 * @param conn BT connection
 * @param data Environment data
 * @return int Status code (0 on success)
 */
int remote_notify_environment(struct bt_conn *conn, const env_data_t *data);

/**
 * @brief Send UART data notification
 * 
 * @param conn BT connection
 * @param data Data to send
 * @param length Data length
 * @return int Status code (0 on success)
 */
int remote_uart_send(struct bt_conn *conn, const uint8_t *data, uint16_t length);

/**
 * @brief Send UART string notification
 * 
 * @param conn BT connection
 * @param str String to send
 * @return int Status code (0 on success)
 */
int remote_uart_send_string(struct bt_conn *conn, const char *str);

#ifdef __cplusplus
}
#endif

#endif // REMOTE_SERVICES_H
