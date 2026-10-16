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
 * @file app_audio_route.h
 * @brief Audio routing layer — connects input, output, and RTC modules
 *
 * Handles upsample (16kHz->32kHz), silence detection, and half-duplex
 * TX gating.  Modules communicate only through this layer.
 */

#ifndef APP_AUDIO_ROUTE_H
#define APP_AUDIO_ROUTE_H

#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Configuration for the audio route layer */
typedef struct {
    const audio_input_ops_t  *input;   /* microphone / capture */
    const audio_output_ops_t *output;  /* speaker / DAC */
    const rtc_ops_t          *rtc;     /* RTC channel (Agora, etc.) */
} audio_route_cfg_t;

/**
 * @brief Initialize audio routing with the given module ops.
 *
 * Registers internal callbacks:
 *  - RTC RX -> upsample -> output feed
 *  - Input capture -> silence gate + half-duplex -> RTC send
 *  - AI state -> TX mute control
 */
app_err_t audio_route_init(const audio_route_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* APP_AUDIO_ROUTE_H */
