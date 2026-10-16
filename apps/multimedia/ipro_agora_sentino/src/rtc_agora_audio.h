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
 * @file rtc_agora_audio.h
 * @brief Internal: Agora RTC audio RX/TX queue and task management
 *
 * Internal header for rtc_agora module. Not part of the public API.
 * Currently the implementation lives in app_agora.c; this header
 * declares the internal interfaces for future extraction.
 */

#ifndef RTC_AGORA_AUDIO_H
#define RTC_AGORA_AUDIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Start the audio RX task (queue + Opus decode + callback) */
int rtc_agora_start_rx_task(void);

/** Start the audio TX task (queue + send to Agora SDK) */
int rtc_agora_start_tx_task(void);

#ifdef __cplusplus
}
#endif

#endif /* RTC_AGORA_AUDIO_H */
