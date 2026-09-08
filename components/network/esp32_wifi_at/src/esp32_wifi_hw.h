/*
 * esp32_wifi_hw.h - ESP32 WiFi HW abstraction (UART + GPIO)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP32_WIFI_HW_H__
#define __ESP32_WIFI_HW_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize UART for ESP32 AT communication.
 *
 * Sets up GPIO pinmux, signal routing, and UART peripheral.
 * Does NOT enable RX interrupts (caller must register callback first).
 */
void esp32_hw_uart_init(void);

/**
 * @brief Enable UART RX interrupts.
 *
 * Must be called AFTER at_client_init() registers the UART ISR callback.
 */
void esp32_hw_uart_enable_rx(void);

/**
 * @brief Pulse ESP32 EN (reset) pin if configured.
 *
 * Only functional when CONFIG_ESP32_WIFI_GPIO_EN >= 0.
 * Drives EN low for 100ms then releases.
 */
void esp32_hw_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP32_WIFI_HW_H__ */
