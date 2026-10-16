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
 * @file rtc_agora_stream.c
 * @brief Internal: Agora data stream message reassembly + AI state parsing
 *
 * Provides weak stub implementations. The actual stream message
 * processing and AI state parsing currently lives in app_agora.c.
 * Once app_agora.c is fully decomposed, the real implementations
 * will move here.
 */

#include "rtc_agora_stream.h"

__attribute__((weak))
int rtc_agora_start_msg_task(void) { return 0; }

__attribute__((weak))
void rtc_agora_set_ai_state_cb(rtc_ai_state_cb_t cb) { (void)cb; }
