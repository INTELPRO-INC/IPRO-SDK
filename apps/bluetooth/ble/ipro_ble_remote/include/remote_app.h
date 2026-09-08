/**
 * @file remote_app.h
 * @brief BLE Remote Control Application Header
 * 
 * Copyright (C) IPRO SDK 2025
 */

#ifndef REMOTE_APP_H
#define REMOTE_APP_H

#include <stdint.h>
#include <stdbool.h>
#include "remote_services.h"
#include "pir_image_service.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * APPLICATION STATES
 ****************************************************************************************
 */

typedef enum {
    REMOTE_STATE_IDLE = 0,
    REMOTE_STATE_INITIALIZING,
    REMOTE_STATE_READY,
    REMOTE_STATE_ADVERTISING,
    REMOTE_STATE_CONNECTED,
    REMOTE_STATE_ERROR,
} remote_app_state_t;

/*
 * APPLICATION CONTEXT
 ****************************************************************************************
 */

typedef struct {
    remote_app_state_t state;
    uint8_t conn_id;
    bool connected;
    
    // Simulated sensor data
    int16_t sim_temperature;
    uint8_t sim_humidity;
    bool env_notify_running;
    
    // Button simulation
    uint32_t btn_press_count;
} remote_app_context_t;

/*
 * API FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Initialize the remote control application
 * 
 * @return int 0 on success, negative on error
 */
int remote_app_init(void);

/**
 * @brief Start BLE advertising
 * 
 * @return int 0 on success, negative on error
 */
int remote_app_start_advertising(void);

/**
 * @brief Stop BLE advertising
 * 
 * @return int 0 on success, negative on error
 */
int remote_app_stop_advertising(void);

/**
 * @brief Get application context
 * 
 * @return const remote_app_context_t* Pointer to context
 */
const remote_app_context_t *remote_app_get_context(void);

/**
 * @brief Simulate button press event
 * 
 * @param event Button event type
 * @return int 0 on success, negative on error
 */
int remote_app_simulate_button(btn_event_t event);

/**
 * @brief Update simulated environment data
 * 
 * @param temperature Temperature in 0.01°C units
 * @param humidity Humidity percentage (0-100)
 * @return int 0 on success, negative on error
 */
int remote_app_update_environment(int16_t temperature, uint8_t humidity);

/**
 * @brief Send UART message to connected phone
 * 
 * @param message Message string
 * @return int 0 on success, negative on error
 */
int remote_app_send_uart(const char *message);

/**
 * @brief Start periodic environment notification
 * 
 * @param interval_ms Notification interval in milliseconds
 * @return int 0 on success, negative on error
 */
int remote_app_start_env_notify(uint32_t interval_ms);

/**
 * @brief Stop periodic environment notification
 * 
 * @return int 0 on success, negative on error
 */
int remote_app_stop_env_notify(void);

/**
 * @brief Set LED state (called from phone or local)
 * 
 * @param on LED on/off state
 * @param brightness Brightness 0-100
 * @return int 0 on success, negative on error
 */
int remote_app_set_led(bool on, uint8_t brightness);

/**
 * @brief Print application status
 */
void remote_app_print_status(void);

/**
 * @brief Process pending notifications (call from main loop)
 */
void remote_app_process_env_notify(void);

/*
 * PIR + IMAGE FUNCTIONS
 ****************************************************************************************
 */

/**
 * @brief Start PIR sensor monitoring
 * 
 * @param gpio_pin GPIO pin number (default CONFIG_PIR_GPIO_PIN)
 * @return int 0 on success
 */
int remote_app_start_pir(uint8_t gpio_pin);

/**
 * @brief Stop PIR sensor monitoring
 * 
 * @return int 0 on success
 */
int remote_app_stop_pir(void);

/**
 * @brief Simulate PIR trigger (for testing)
 * 
 * @return int 0 on success
 */
int remote_app_pir_trigger(void);

/**
 * @brief Capture image (demo or actual camera)
 * 
 * @return int 0 on success
 */
int remote_app_capture_image(void);

/**
 * @brief Start image transfer to phone
 * 
 * @return int 0 on success
 */
int remote_app_start_image_transfer(void);

/**
 * @brief Get PIR/Image service status
 */
void remote_app_print_pir_status(void);

#ifdef __cplusplus
}
#endif

#endif // REMOTE_APP_H
