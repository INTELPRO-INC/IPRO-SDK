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

#pragma once

#include <stdbool.h>
#include <stdint.h>

void pdm_start_recording(int seconds);
void pdm_stop_recording(void);
bool pdm_is_recording(void);

void pdm_start_streaming(void);
void pdm_stop_streaming(void);
bool pdm_is_streaming(void);

void pdm_set_gain(int gain_db);
int pdm_get_gain(void);

const int16_t *pdm_get_buffer(void);
uint32_t pdm_get_recorded_bytes(void);
uint32_t pdm_get_buf_capacity(void);


void pdm_audio_power_set(bool on);
void pdm_audio_power_toggle(void);
bool pdm_audio_power_is_on(void);

int pdm_buttons_init(void);
