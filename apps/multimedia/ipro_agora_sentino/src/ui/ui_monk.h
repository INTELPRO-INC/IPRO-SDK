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
 * @file ui_monk.h
 * @brief Monk character animation controller
 *
 * Animation state machine per AI state:
 *   IDLE:      gentle scale breathing (256->270->256, 3s, ease_in_out)
 *   LISTENING: slight tilt + scale pulse
 *   THINKING:  opacity pulse (255->180->255, 1s)
 *   SPEAKING:  scale bounce (256->290->256, 0.8s, bounce path)
 */

#ifndef UI_MONK_H
#define UI_MONK_H

#include "app_module.h"
#include "lvgl/lvgl.h"

/**
 * @brief Create the monk image widget inside a parent container.
 * @param parent  The animation area container from ui_screen
 */
void ui_monk_create(lv_obj_t *parent);

/**
 * @brief Switch animation to match AI state.
 * Stops current animation and starts the new one.
 */
void ui_monk_set_state(rtc_ai_state_t state);

#endif /* UI_MONK_H */
