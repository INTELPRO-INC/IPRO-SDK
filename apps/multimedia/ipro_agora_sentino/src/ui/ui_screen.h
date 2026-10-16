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
 * @file ui_screen.h
 * @brief Main screen layout — status bar, animation area, AI state, TTS text
 */

#ifndef UI_SCREEN_H
#define UI_SCREEN_H

#include <stdbool.h>
#include "app_module.h"

/**
 * @brief Create the main screen layout.
 * Call after lv_init() and display driver setup, under lv_lock().
 */
void ui_screen_create(void);

/** Update AI state label (color-coded) */
void ui_screen_set_ai_state(rtc_ai_state_t state);

/** Update TTS subtitle text (Chinese/English, auto-wrap) */
void ui_screen_set_tts_text(const char *utf8_text);

/** Update connection status indicators */
void ui_screen_set_mqtt_status(bool connected);
void ui_screen_set_agora_status(bool connected);

/** Update time display in status bar */
void ui_screen_set_time(const char *time_str);

#endif /* UI_SCREEN_H */
