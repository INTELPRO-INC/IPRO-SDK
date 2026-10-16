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
 * @file rtc_agora_audio.c
 * @brief Internal: Agora RTC audio RX/TX queue and task management
 *
 * Provides weak stub implementations. The actual RX/TX task code
 * currently lives in app_agora.c (app_agora_start_rx_task,
 * app_agora_start_tx_task). Once app_agora.c is fully decomposed,
 * the real implementations will move here.
 */

#include "rtc_agora_audio.h"

/* Weak stubs — overridden by the real implementations in app_agora.c */
__attribute__((weak))
int rtc_agora_start_rx_task(void) { return 0; }

__attribute__((weak))
int rtc_agora_start_tx_task(void) { return 0; }
