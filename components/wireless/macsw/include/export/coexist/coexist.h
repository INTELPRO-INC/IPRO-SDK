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
 ****************************************************************************************
 *
 * @file coexist.h
 *
 * @brief This file contains all functions for coexist.
 *
 ****************************************************************************************
 */

#ifndef _COEXIST_H_
#define _COEXIST_H_

#if CONFIG_COEX_WIFI_MODE
enum coexist_event_dpsm_yield_reason {
    COEXIST_DPSM_YIELD_NO_TRAFFIC,
    COEXIST_DPSM_YIELD_NO_BEACON,
    COEXIST_DPSM_YIELD_NO_ENOUGH_TIME,
    COEXIST_DPSM_YIELD_ENTER_PS,
};

int coexist_event_scan_start(void);
int coexist_event_scan_done(void);
int coexist_event_conn_start(void);
int coexist_event_conn_ok(void);
int coexist_event_conn_fail(void);
int coexist_event_disconn_start(void);
int coexist_event_disconn_done(void);
int coexist_event_dpsm_start(void);
int coexist_event_dpsm_yield(int reason);
int coexist_event_dpsm_wnd_rem(void);
int coexist_event_dpsm_stop(void);
int coexist_event_roc_req(void);
int coexist_event_tbtt_update(void);
int coexist_is_on(void);
int coexist_event_init(int coex_mode);
int coexist_event_deinit(void);
#endif

#endif // _COEXIST_H_
