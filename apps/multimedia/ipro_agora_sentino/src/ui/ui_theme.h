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
 * @file ui_theme.h
 * @brief UI theme constants — colors, sizes, layout
 */

#ifndef UI_THEME_H
#define UI_THEME_H

/* Screen: 240 x 320, portrait */
#define UI_SCR_W         240
#define UI_SCR_H         320

/* Status bar (top) */
#define UI_STATUSBAR_H       32
#define UI_STATUSBAR_BG      0x1A1A2E   /* Dark navy */

/* Animation area (centered) */
#define UI_ANIM_SIZE        160
#define UI_ANIM_Y           (UI_STATUSBAR_H + 4)
#define UI_ANIM_BG          0x333344    /* Dark gray placeholder */

/* AI state label */
#define UI_STATE_Y          (UI_ANIM_Y + UI_ANIM_SIZE + 8)
#define UI_STATE_H           24

/* TTS text area (bottom, fills remaining space) */
#define UI_TTS_Y            (UI_STATE_Y + UI_STATE_H + 4)
#define UI_TTS_H            (UI_SCR_H - UI_TTS_Y - 4)

/* Colors */
#define UI_COLOR_BG          0x0F0F1A   /* Near-black background */
#define UI_COLOR_TEXT        0xE0E0E0   /* Light gray text */
#define UI_COLOR_ACCENT      0x00AAFF   /* Blue accent */

/* AI state colors */
#define UI_COLOR_IDLE        0x666666   /* Gray */
#define UI_COLOR_LISTENING   0x00CC44   /* Green */
#define UI_COLOR_THINKING    0xFFAA00   /* Orange */
#define UI_COLOR_SPEAKING    0x00AAFF   /* Blue */

/* Connection indicator */
#define UI_COLOR_CONNECTED   0x00CC44   /* Green */
#define UI_COLOR_DISCONN     0xFF3333   /* Red */

#endif /* UI_THEME_H */
