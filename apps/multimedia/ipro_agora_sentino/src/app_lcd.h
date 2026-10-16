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
 * @file app_lcd.h
 * @brief LCD display module — LVGL-driven ILI9341 SPI status display
 *
 * Provides initialization, pause/resume for SPI bus sharing with audio,
 * and high-level state update APIs for the gateway UI.
 */

#ifndef APP_LCD_H
#define APP_LCD_H

#include <generated/autoconf.h>

#ifdef CONFIG_APP_LCD_ENABLE

#include <stdbool.h>
#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize LCD hardware, LVGL, and display driver.
 *
 * Call early in gateway startup (before audio init).
 * Creates the LVGL handler task and shows initial UI.
 *
 * @return APP_OK on success
 */
app_err_t app_lcd_init(void);

/**
 * @brief Pause/resume LVGL rendering (SPI transfers).
 *
 * Call with pause=true before audio RX DMA operations that
 * share the SPI bus, and pause=false after they complete.
 * Uses atomic reference counting — safe for multiple callers.
 *
 * @param pause  true to pause, false to resume
 */
void app_lcd_pause(bool pause);

/**
 * @brief Update the AI agent state shown on screen.
 * @param state  AI state (idle, listening, thinking, speaking)
 */
void app_lcd_set_ai_state(rtc_ai_state_t state);

/**
 * @brief Update the TTS subtitle text shown on screen.
 * @param text  UTF-8 text to display (copied internally)
 */
void app_lcd_set_tts_text(const char *text);

/**
 * @brief Update the network connection status indicator.
 * @param connected  true if network is up
 */
void app_lcd_set_network_status(bool connected);

/**
 * @brief Update the cloud/MQTT connection status indicator.
 * @param connected  true if cloud is connected
 */
void app_lcd_set_cloud_status(bool connected);

#ifdef __cplusplus
}
#endif

#else /* !CONFIG_APP_LCD_ENABLE */

/* Stub macros when LCD is disabled — zero overhead */
#include "app_module.h"
#define app_lcd_init()                    (APP_OK)
#define app_lcd_pause(p)                  ((void)(p))
#define app_lcd_set_ai_state(s)           ((void)(s))
#define app_lcd_set_tts_text(t)           ((void)(t))
#define app_lcd_set_network_status(c)     ((void)(c))
#define app_lcd_set_cloud_status(c)       ((void)(c))

#endif /* CONFIG_APP_LCD_ENABLE */

#endif /* APP_LCD_H */
