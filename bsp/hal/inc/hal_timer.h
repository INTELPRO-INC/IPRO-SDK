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

#ifndef __HAL_TIMER__H__
#define __HAL_TIMER__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

enum timer_ch_index_type {
    TIMER_CH0_INDEX,
    TIMER_CH1_INDEX,
    TIMER_CH_MAX_INDEX
};

enum timer_index_type {
    TIMER0_INDEX,
    TIMER1_INDEX,
    TIMER_MAX_INDEX
};

enum {
    TIMER_CTRL_SET_INT = 0,
    TIMER_CTRL_CLR_INT,
    TIMER_CTRL_GET_INT,
    TIMER_CTRL_CONFIG,
    TIMER_CTRL_RESUME,
    TIMER_CTRL_SUSPEND,
    TIMER_CTRL_GET_CONFIG,
    TIMER_CTRL_SET_CALLBACK,
};

enum timer_preload_trigger_type {
    TIMER_PRELOAD_TRIGGER_NONE,
    TIMER_PRELOAD_TRIGGER_COMP0,
    TIMER_PRELOAD_TRIGGER_COMP1,
    TIMER_PRELOAD_TRIGGER_COMP2,
};

enum timer_cnt_mode_type {
    TIMER_CNT_PRELOAD,
    TIMER_CNT_FREERUN,
};

enum timer_compare_id_type {
    TIMER_COMPARE_ID_0,
    TIMER_COMPARE_ID_1,
    TIMER_COMPARE_ID_2,
};

enum timer_it_type {
    TIMER_COMP0_IT = 1 << 0,
    TIMER_COMP1_IT = 1 << 1,
    TIMER_COMP2_IT = 1 << 2,
};

enum timer_event_type {
    TIMER_EVENT_COMP0,
    TIMER_EVENT_COMP1,
    TIMER_EVENT_COMP2,
    TIMER_EVENT_UNKNOWN
};

typedef struct timer_timeout_cfg {
    enum timer_compare_id_type timeout_id;
    uint32_t timeout_val;
} timer_timeout_cfg_t;

typedef struct timer_device {
    uint8_t id;
    uint8_t ch;
    enum timer_cnt_mode_type cnt_mode;
    enum timer_preload_trigger_type trigger;
    uint32_t reload;
    uint32_t timeout1;
    uint32_t timeout2;
    uint32_t timeout3;
    void (*callback)(int timer_id, int timer_ch, uint32_t event);
} timer_device_t;

typedef struct hal_timer_config {
    uint8_t  cnt_mode;
    uint8_t  trigger;
    uint32_t reload;
    uint32_t timeout1;
    uint32_t timeout2;
    uint32_t timeout3;    
} hal_timer_config_t;

int timer_init(int timer_id, int timer_ch, hal_timer_config_t *timerCfg);
void timer_set_div(int timer_id, int timer_ch, uint8_t div);
void timer_setCallback(int timer_id, int timer_ch, void *fn);
uint32_t timer_get_counter(int timer_id, int timer_ch);
void timer_start(int timer_id, int timer_ch);
void timer_stop(int timer_id, int timer_ch);
#ifdef __cplusplus
}
#endif
#endif