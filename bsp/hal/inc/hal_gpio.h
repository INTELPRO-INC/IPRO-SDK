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

#ifndef __HAL_GPIO__H__
#define __HAL_GPIO__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "drv_list.h"
#include "board_config.h"

enum gpio_pin_type {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_16,
    GPIO_PIN_17,
    GPIO_PIN_18,
    GPIO_PIN_19,
    GPIO_PIN_20,
    GPIO_PIN_21,
    GPIO_PIN_22,
    GPIO_PIN_23,
    GPIO_PIN_24,
    GPIO_PIN_25,
    GPIO_PIN_26,
    GPIO_PIN_27,
    GPIO_PIN_28,
    GPIO_PIN_29,
    GPIO_PIN_30,
    GPIO_PIN_31,
    GPIO_PIN_MAX,
};

#define GPIO_OUTPUT_MODE                   0
#define GPIO_OUTPUT_PP_MODE                1
#define GPIO_OUTPUT_PD_MODE                2
#define GPIO_INPUT_MODE                    3
#define GPIO_INPUT_PP_MODE                 4
#define GPIO_INPUT_PD_MODE                 5
#define GPIO_ASYNC_RISING_TRIGER_INT_MODE  6
#define GPIO_ASYNC_FALLING_TRIGER_INT_MODE 7
#define GPIO_ASYNC_HIGH_LEVEL_INT_MODE     8
#define GPIO_ASYNC_LOW_LEVEL_INT_MODE      9
#define GPIO_SYNC_RISING_TRIGER_INT_MODE   10
#define GPIO_SYNC_FALLING_TRIGER_INT_MODE  11
#define GPIO_SYNC_HIGH_LEVEL_INT_MODE      12
#define GPIO_SYNC_LOW_LEVEL_INT_MODE       13
#define GPIO_HZ_MODE                       14

void gpio_set_mode(uint32_t pin, uint32_t mode);
void gpio_write(uint32_t pin, uint32_t value);
void gpio_toggle(uint32_t pin);
int gpio_read(uint32_t pin);
void gpio_attach_irq(uint32_t pin, void (*cbfun)(uint32_t pin));
void gpio_irq_enable(uint32_t pin, uint8_t enabled);

#ifdef __cplusplus
}
#endif
#endif