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
 * @file evk.h
 * @brief IPRO7AI EVK board library - one include, one evk_begin().
 *
 * The EVK sub-board (schematic IPRO7AI_MODULE_SUB_0812A, CONFIG_IPRO7AI_EVK)
 * carries a 2.4" ST7789P3 LCD, a SC101IOT 720p DVP camera, a PDM microphone
 * and a DAC-driven class-D speaker. This library is the M5Stack-style board
 * layer for it: the drivers live here, an app calls evk_begin() and then
 * talks to st7789p3_* / evk_gfx_* (panel), evk_cam_* (camera + ISP + NPU
 * feed), evk_mic_* and evk_spk_* (audio).
 *
 * What is compiled in follows the app's .config:
 *   CONFIG_COMPONENTS_IPRO7AI_EVK_ENABLE  the library, panel + gfx always
 *   CONFIG_EVK_CAMERA                     evk_cam (needs MM_SUPPORT + sensor)
 *   CONFIG_EVK_AUDIO                      evk_mic + evk_spk (needs AUDIO_OUTPUT)
 *   CONFIG_EVK_SDCARD                     evk_sd (needs FatFS + EVFS)
 *   CONFIG_EVK_SHELL                      lcd / cam / mic / spk / sd shell commands
 *
 * Pin map, rails and the board's quirks: bsp/board/ipro7/IPRO7AI_EVK.md and
 * the *_port.h headers next to this one.
 */
#ifndef EVK_H
#define EVK_H

#include <stdint.h>
#include <stdbool.h>

#include <generated/autoconf.h>

#include "st7789p3.h"
#include "evk_gfx.h"
#ifdef CONFIG_EVK_CAMERA
#include "evk_cam.h"
#include "evk_cam_port.h"
#endif
#ifdef CONFIG_EVK_AUDIO
#include "evk_mic.h"
#include "evk_spk.h"
#include "evk_pcm.h"
#endif
#ifdef CONFIG_EVK_SDCARD
#include "evk_sd.h"
#endif

#define EVK_BOARD_NAME   "IPRO7AI EVK"

/** Which blocks this build carries. */
#define EVK_HAS_DISPLAY  1
#ifdef CONFIG_EVK_CAMERA
#define EVK_HAS_CAMERA   1
#else
#define EVK_HAS_CAMERA   0
#endif
#ifdef CONFIG_EVK_AUDIO
#define EVK_HAS_AUDIO    1
#else
#define EVK_HAS_AUDIO    0
#endif
#ifdef CONFIG_EVK_SDCARD
#define EVK_HAS_SDCARD   1
#else
#define EVK_HAS_SDCARD   0
#endif

/**
 * @brief Bring the board to a known state: camera parked in reset, speaker
 *        amplifier off, panel initialised and cleared.
 *
 * Call once from the first task (it sleeps). Idempotent.
 * @return 0, or -1 if the panel did not initialise (the rest still ran).
 */
int  evk_begin(void);

/** @brief True once evk_begin() has run. */
bool evk_ready(void);

#endif /* EVK_H */
