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

#include "hal_pwm.h"
#include "drv_pwm.h"
#include "drv_glb.h"
#include "drv_clock.h"

static pwm_device_t pwm_devices[PWM_MAX_INDEX] = {
    PWM_CH0_CONFIG,
    PWM_CH1_CONFIG,
    PWM_CH2_CONFIG,
    PWM_CH3_CONFIG,
};
static void PWM_IRQ(void);

int pwm_init(int pwm_id, uint16_t caps)
{
    PWMx_CFG_Type pwm_cfg;
    /*PWM_CHx_CFG_Type pwm_ch_cfg = {
        .modP = PWM_MODE_ENABLE,
        .modN = PWM_MODE_ENABLE,
        .polP = PWM_POL_ACTIVE_HIGH,
        .polN = PWM_POL_ACTIVE_HIGH,
        .idlP = PWM_IDLE_STATE_INACTIVE,
        .idlN = PWM_IDLE_STATE_INACTIVE,
        .brkP = PWM_BREAK_STATE_INACTIVE,
        .brkN = PWM_BREAK_STATE_INACTIVE,
        .thresholdL = 0,
        .thresholdH = 0,
        .dtg = 0,
    };*/

        PWM_CHx_CFG_Type pwm_chx_cfg[PWM_MAX_INDEX] = { {
            .modP = PWM_MODE_DISABLE,
            .modN = PWM_MODE_DISABLE,
            .polP = PWM_POL_ACTIVE_HIGH,
            .polN = PWM_POL_ACTIVE_HIGH,
            .idlP = PWM_IDLE_STATE_INACTIVE,
            .idlN = PWM_IDLE_STATE_ACTIVE,
            .brkP = PWM_BREAK_STATE_INACTIVE,
            .brkN = PWM_BREAK_STATE_ACTIVE,
            .thresholdL = 0,
            .thresholdH = 500,
            .dtg = 0,
        },
        {
            .modP = PWM_MODE_DISABLE,
            .modN = PWM_MODE_DISABLE,
            .polP = PWM_POL_ACTIVE_LOW,
            .polN = PWM_POL_ACTIVE_LOW,
            .idlP = PWM_IDLE_STATE_ACTIVE,
            .idlN = PWM_IDLE_STATE_INACTIVE,
            .brkP = PWM_BREAK_STATE_ACTIVE,
            .brkN = PWM_BREAK_STATE_ACTIVE,
            .thresholdL = 200,
            .thresholdH = 400,
            .dtg = 0,
        },
        {
            .modP = PWM_MODE_DISABLE,
            .modN = PWM_MODE_DISABLE,
            .polP = PWM_POL_ACTIVE_HIGH,
            .polN = PWM_POL_ACTIVE_LOW,
            .idlP = PWM_IDLE_STATE_INACTIVE,
            .idlN = PWM_IDLE_STATE_INACTIVE,
            .brkP = PWM_BREAK_STATE_INACTIVE,
            .brkN = PWM_BREAK_STATE_INACTIVE,
            .thresholdL = 99,
            .thresholdH = 999,
            .dtg = 0,
        },
        {
            .modP = PWM_MODE_DISABLE,
            .modN = PWM_MODE_DISABLE,
            .polP = PWM_POL_ACTIVE_LOW,
            .polN = PWM_POL_ACTIVE_HIGH,
            .idlP = PWM_IDLE_STATE_ACTIVE,
            .idlN = PWM_IDLE_STATE_ACTIVE,
            .brkP = PWM_BREAK_STATE_ACTIVE,
            .brkN = PWM_BREAK_STATE_INACTIVE,
            .thresholdL = 300,
            .thresholdH = 900,
            .dtg = 0,
        },
    };
    CPU_Interrupt_Disable(PWM_IRQn);

    pwm_cfg.clk = PWM_CLK_XCLK;
    pwm_cfg.clkDiv = 32;
    pwm_cfg.period = 1000;
    pwm_cfg.stopMode = PWM_STOP_GRACEFUL;
    pwm_cfg.intPulseCnt = 0;
    pwm_cfg.stpRept = DISABLE;
    pwm_cfg.adcSrc = PWM_TRIGADC_SOURCE_NONE;
    
    PWMx_Init(pwm_id, &pwm_cfg);

    for (PWM_CHx_Type ch = PWM_CH0; ch < PWM_CHx_MAX; ch++) {
        PWM_Channelx_Init(pwm_id, ch, &pwm_chx_cfg[ch]);
    }

    if (pwm_cfg.intPulseCnt > 0) {
        Interrupt_Handler_Register(PWM_IRQn, PWM_IRQ);
        CPU_Interrupt_Enable(PWM_IRQn);
    }
    MSG("pwm clock:%luhz\r\n", Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_PWM));
    return 0;
}
int pwm_close(int pwm_id, int ch)
{
    if(ch > 4)
        PWMx_Disable(pwm_id);
    else {
        printf("stop channel %d\n", ch);
        PWM_Channelx_Pwm_Mode_Set(pwm_id, ch, PWM_MODE_DISABLE, PWM_MODE_DISABLE);
    }
    return 0;
}

int pwm_ioctl(int pwm_id, int cmd, void *args)
{
    pwm_device_t *pwm_device = &pwm_devices[pwm_id];
    pwm_dutycycle_config_t *config = (pwm_dutycycle_config_t *)args;
    switch (cmd) {
        case PWM_CTRL_CONFIG:
            break;
        case PWM_CTRL_RESUME:
            PWMx_Enable(pwm_id);
            for (PWM_CHx_Type ch = PWM_CH0; ch < PWM_CHx_MAX; ch++) {
                if(ch != 1)
                    PWM_Channelx_Pwm_Mode_Set(pwm_id, ch, PWM_MODE_ENABLE, PWM_MODE_ENABLE);
            }
            break;

        case PWM_CTRL_SUSPEND:
            PWMx_Disable(pwm_id);
            break;
        case PWM_CTRL_FREQUENCE_CONFIG:

            if ((uint32_t)args > Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_CLOCK_PWM))
                return -1;
            pwm_device->period = (uint32_t)args;
            PWMx_Period_Set(pwm_device->ch, (uint32_t)args);
            break;
        case PWM_CTRL_DUTYCYCLE_CONFIG:
            PWM_Channelx_ThresholdL_Set(pwm_id, pwm_device->ch, config->threshold_low);
            PWM_Channelx_ThresholdH_Set(pwm_id, pwm_device->ch, config->threshold_high);
            break;
        /*case PWM_CTRL_IT_PULSE_COUNT_CONFIG: {
            PWM_Channelx_Set_IntPeriodCnt(pwm_device->ch, (uint32_t)args);
            if ((uint32_t)args)
                CPU_Interrupt_Enable(PWM_IRQn);
            else
                CPU_Interrupt_Disable(PWM_IRQn);
            break;
        }*/
        case PWM_CTRL_POLARITY_CONFIG:
            PWM_CHx_CFG_Type *pwm_ch_config = (PWM_CHx_CFG_Type *)args;
            PWM_Channelx_Polarity_Set(pwm_id, pwm_device->ch, pwm_ch_config->polP, pwm_ch_config->polN);
            break;
        case PWM_CTRL_SET_CALLBACKG:
            pwm_device->callback = args;
            break;
        case PWM_CRTL_GET_PERIOD_CONFIG:
            PWMx_Period_Get(pwm_id, (uint16_t *)args);
            break;
        default:
            break;
    }

    return 0;
}

static void pwm_isr(void)
{
    uint32_t timeoutCnt = 160 * 1000;
    pwm_device_t *pwm_device;


    for (int i = 0; i < PWM_MAX_INDEX; i++) {
        pwm_device = &pwm_devices[i];
        if (!pwm_device->bActive)
            continue;

        if (PWM_Int_Status_Get(i, PWM_INT_ALL)) {
            PWM_Int_Clear(i, PWM_INT_ALL);

            do {
                timeoutCnt--;

                if (timeoutCnt == 0) {
                    break;
                }
            } while (PWM_Int_Status_Get(pwm_device->ch, PWM_INT_ALL));

            PWM_Int_Clear(i, PWM_INT_ALL);


            if (pwm_device->callback) {
                pwm_device->callback(pwm_device, NULL, 0, PWM_EVENT_COMPLETE);
            }
        }
    }   
}

static void PWM_IRQ(void)
{
    pwm_isr();
}
