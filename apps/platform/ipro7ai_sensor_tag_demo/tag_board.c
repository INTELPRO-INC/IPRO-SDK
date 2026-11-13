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

/*
 * LEDs, button and buzzer of the IPRO7AI Sensor Tag board. See tag_board.h for the pins.
 */
#include "tag_board.h"

#include "drv_glb.h"
#include "drv_gpio.h"
#include "drv_pwm.h"

/* The LEDs sit between 3V3 and the pin, the button between the pin and ground. */
#define LED_ON_LEVEL        0
#define LED_OFF_LEVEL       1
#define BUTTON_DOWN_LEVEL   0

/* GPIO2 is channel 2 of PWM0 (pin number modulo 5). */
#define BUZZER_PWM_ID       PWM0_ID
#define BUZZER_PWM_CH       PWM_CH2

static bool s_buzzer_on;

static void led_pin(uint8_t pin)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = pin,
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_OUTPUT,
        .pullType = GPIO_PULL_NONE,
        .drive    = 0,
        .smtCtrl  = 0,
    };
    GLB_GPIO_Init(&cfg);
    GLB_GPIO_OUTPUT_Enable(pin);
    GLB_GPIO_Write(pin, LED_OFF_LEVEL);
}

static void button_pin(uint8_t pin)
{
    GLB_GPIO_Cfg_Type cfg = {
        .gpioPin  = pin,
        .gpioFun  = GPIO_FUN_GPIO,
        .gpioMode = GPIO_MODE_INPUT,
        .pullType = GPIO_PULL_UP,
        .drive    = 0,
        .smtCtrl  = 1,
    };
    GLB_GPIO_Init(&cfg);
}

static void buzzer_pin(void)
{
    GLB_GPIO_Cfg_Type pad = {
        .gpioPin  = TAG_BUZZER_GPIO,
        .gpioFun  = GPIO_FUN_PWM,
        .gpioMode = GPIO_MODE_AF,
        .pullType = GPIO_PULL_DOWN,     /* low while the PWM is idle */
        .drive    = 1,
        .smtCtrl  = 0,
    };
    /* 32 MHz crystal clock / 32 = 1 MHz; a period of 1000 counts is 1 kHz. */
    PWMx_CFG_Type group = {
        .clk         = PWM_CLK_XCLK,
        .clkDiv      = 32,
        .period      = 1000,
        .stopMode    = PWM_STOP_GRACEFUL,
        .intPulseCnt = 0,
        .stpRept     = DISABLE,
        .adcSrc      = PWM_TRIGADC_SOURCE_NONE,
    };
    /* Half duty is the loudest for a passive buzzer; off until asked. */
    PWM_CHx_CFG_Type channel = {
        .modP = PWM_MODE_DISABLE,
        .modN = PWM_MODE_DISABLE,
        .polP = PWM_POL_ACTIVE_HIGH,
        .polN = PWM_POL_ACTIVE_HIGH,
        .idlP = PWM_IDLE_STATE_INACTIVE,
        .idlN = PWM_IDLE_STATE_INACTIVE,
        .brkP = PWM_BREAK_STATE_INACTIVE,
        .brkN = PWM_BREAK_STATE_INACTIVE,
        .thresholdL = 0,
        .thresholdH = 500,
        .dtg = 0,
    };

    GLB_GPIO_Init(&pad);
    PWMx_Init(BUZZER_PWM_ID, &group);
    PWM_Channelx_Init(BUZZER_PWM_ID, BUZZER_PWM_CH, &channel);
    PWMx_Enable(BUZZER_PWM_ID);
}

void tag_board_init(void)
{
    led_pin(TAG_LED_RED_GPIO);
    led_pin(TAG_LED_GREEN_GPIO);
    button_pin(TAG_BUTTON_GPIO);
    buzzer_pin();
}

void tag_led_red(bool on)
{
    GLB_GPIO_Write(TAG_LED_RED_GPIO, on ? LED_ON_LEVEL : LED_OFF_LEVEL);
}

void tag_led_green(bool on)
{
    GLB_GPIO_Write(TAG_LED_GREEN_GPIO, on ? LED_ON_LEVEL : LED_OFF_LEVEL);
}

bool tag_button_pressed(void)
{
    return GLB_GPIO_Read(TAG_BUTTON_GPIO) == BUTTON_DOWN_LEVEL;
}

void tag_buzzer(bool on)
{
    if (on == s_buzzer_on) {
        return;
    }
    PWM_Channelx_Pwm_Mode_Set(BUZZER_PWM_ID, BUZZER_PWM_CH,
                              on ? PWM_MODE_ENABLE : PWM_MODE_DISABLE,
                              on ? PWM_MODE_ENABLE : PWM_MODE_DISABLE);
    s_buzzer_on = on;
}
