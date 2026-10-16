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
 * @file rtc_agora_stream.h
 * @brief Internal: Agora data stream message reassembly + AI state parsing
 *
 * Internal header for rtc_agora module. Not part of the public API.
 * Currently the implementation lives in app_agora.c; this header
 * declares the internal interfaces for future extraction.
 */

#ifndef RTC_AGORA_STREAM_H
#define RTC_AGORA_STREAM_H

#include <stdint.h>
#include "app_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start the stream message processing task */
int rtc_agora_start_msg_task(void);

/**
 * @brief Set callback for AI state changes.
 * @param cb Callback invoked when AI transitions between states
 */
void rtc_agora_set_ai_state_cb(rtc_ai_state_cb_t cb);

#ifdef __cplusplus
}
#endif

#endif /* RTC_AGORA_STREAM_H */
