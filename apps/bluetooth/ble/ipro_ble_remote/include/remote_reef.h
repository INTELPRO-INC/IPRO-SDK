/**
 * @file remote_reef.h
 * @brief BLE remote on the IPRO7AI Reef: glue between the services and the board
 *
 * - LED service       -> WS2812 colour LED (colour from Reef service 0xFF81)
 * - Button service    <- SW1/BOOT, SW3, SW4, SW5 with press / release /
 *                        long press / double click; the key id goes out on
 *                        0xFF82. A short BOOT press takes a photo.
 * - Environment       <- on-chip temperature (the Reef has no humidity sensor:
 *                        humidity reads 0)
 * - PIR image service <- OV5640 JPEG captures; "PIR" motion is camera frame
 *                        differencing, armed while PIR notifications are on
 * - Reef service      -> tones, microphone level, board info
 *
 * Copyright (c) 2026 INTELPRO
 */

#ifndef REMOTE_REEF_H
#define REMOTE_REEF_H

#include <stdbool.h>
#include <stdint.h>

#include "remote_services.h"

/** @brief Board bring-up, services' handlers, capture hook. */
int remote_reef_init(void);

/** @brief The LED service changed the LED state. */
void remote_reef_led(const led_state_t *state);

/** @brief Link up / down (down: motion detection and the meter stop). */
void remote_reef_connection(bool connected);

/** @brief PIR status notifications on / off: camera motion detection follows. */
void remote_reef_pir_notify(bool on);

/** @brief Chip temperature in 0.01 C (25.00 C until the first reading). */
int16_t remote_reef_temperature(void);

#endif /* REMOTE_REEF_H */
