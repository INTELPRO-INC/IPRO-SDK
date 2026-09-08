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

#ifndef __HAL_PWM__H__
#define __HAL_PWM__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"


enum {
    PWM_CTRL_CONFIG = 0,
    PWM_CTRL_RESUME,
    PWM_CTRL_SUSPEND,
    PWM_CTRL_FREQUENCE_CONFIG,
    PWM_CTRL_DUTYCYCLE_CONFIG,
    PWM_CTRL_POLARITY_CONFIG,
    PWM_CTRL_IT_PULSE_COUNT_CONFIG,
    PWM_CTRL_SET_CALLBACKG,
    PWM_CRTL_GET_PERIOD_CONFIG,
};

enum pwm_index_type {
    PWM_CH0_INDEX,
    PWM_CH1_INDEX,
    PWM_CH2_INDEX,
    PWM_CH3_INDEX,
    PWM_MAX_INDEX,
};

#define pwm_channel_start(dev)                 pwm_ioctl(dev, PWM_CTRL_RESUME, NULL)
#define pwm_channel_stop(dev)                  pwm_ioctl(dev, PWM_CTRL_SUSPEND, NULL)
#define pwm_channel_freq_update(dev, count)    pwm_ioctl(dev, PWM_CTRL_FREQUENCE_CONFIG, (void *)count)
#define pwm_channel_dutycycle_update(dev, cfg) pwm_ioctl(dev, PWM_CTRL_DUTYCYCLE_CONFIG, cfg)
#define pwm_it_pulse_count_update(dev, count)  pwm_ioctl(dev, PWM_CTRL_IT_PULSE_COUNT_CONFIG, (void *)count)

enum pwm_event_type {
    PWM_EVENT_COMPLETE,
};

typedef struct
{
    uint16_t threshold_low;
    uint16_t threshold_high;
} pwm_dutycycle_config_t;

typedef struct pwm_device {
    uint16_t caps;
    uint8_t bActive;
    uint8_t ch;
    uint8_t polarity_invert_mode;
    uint16_t period;
    uint16_t threshold_low;
    uint16_t threshold_high;
    uint16_t it_pulse_count;
    void (*callback)(void *dev, void *args, uint32_t size, uint32_t event);
} pwm_device_t;

pwm_device_t* pwm_getDevice(int index);
int pwm_init(int pwm_id, uint16_t caps);
int pwm_ioctl(int pwm_id, int cmd, void *args);
int pwm_close(int pwm_id, int ch);
#ifdef __cplusplus
}
#endif
#endif
