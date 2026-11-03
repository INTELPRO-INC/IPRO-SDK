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

#include <stdio.h>
#include <hal_pwm.h>
#include <drv_clock.h>
#include "shell.h"
#include "FreeRTOS.h"
#include "task.h"

#define PWM_ID   0 //MC0, MC1
enum {
    PWM_TESTCASE_FREQ_1K_DUTY_25 = 0,
    PWM_TESTCASE_FREQ_1K_DUTY_50,
    PWM_TESTCASE_FREQ_1K_DUTY_75,
    PWM_TESTCASE_FREQ_10K_DUTY_25,
    PWM_TESTCASE_FREQ_10K_DUTY_50,
    PWM_TESTCASE_FREQ_10K_DUTY_75,
    PWM_TESTCASE_FREQ_1M_DUTY_25,
    PWM_TESTCASE_FREQ_1M_DUTY_50,
    PWM_TESTCASE_FREQ_1M_DUTY_75,
    PWM_TESTCASE_FREQ_10M_DUTY_25,
    PWM_TESTCASE_FREQ_10M_DUTY_50,
    PWM_TESTCASE_FREQ_10M_DUTY_75,
    PWM_TESTCASE_NUM,
};
static void pwm_test_callback(pwm_device_t *dev, void *arg, uint32_t size, uint32_t event)
{
    printf("PWM%d: interrupt event = %ld\n", dev->ch, event);
}

static int cmd_pwm_test(int argc, char **argv)
{
    pwm_dutycycle_config_t duty_cfg;
    uint32_t pwm_clk = 8000000;
    uint32_t period = 0;
    int testcase = 1;
    if(argc == 2) {
        testcase = atoi(argv[1]);
    }

    printf("PWM Test Start\n");

    /* ====== 初始化 PWM ====== */
    pwm_init(PWM_ID, 0);

    /* ====== 設定 PWM 頻率（以 period 方式） ======
     * 頻率 = PWM_clk / period
     * 假設 PWM_clk = 32MHz
     * period = 32000 → freq = 1kHz
     */
    /* ====== 設定Duty ====== */
    duty_cfg.threshold_low = 0;
    duty_cfg.threshold_high = 0;
    switch((testcase)) {
        case PWM_TESTCASE_FREQ_1K_DUTY_25:
            period = 100;
            //duty_cfg.threshold_high = period * 25 / 100;
            break;
        case PWM_TESTCASE_FREQ_1K_DUTY_50:
            period = pwm_clk/1000;
            duty_cfg.threshold_high = period * 50 / 100;
            break;
        case PWM_TESTCASE_FREQ_1K_DUTY_75:
            period = pwm_clk/1000;
            duty_cfg.threshold_high = period * 75 / 100;
            break;
        case PWM_TESTCASE_FREQ_10K_DUTY_25:
            period = pwm_clk/10000;
            duty_cfg.threshold_high = period * 25 / 100;
            break;
        case PWM_TESTCASE_FREQ_10K_DUTY_50:
            period = pwm_clk/10000;
            duty_cfg.threshold_high = period * 50 / 100;
            break;
        case PWM_TESTCASE_FREQ_10K_DUTY_75:
            period = pwm_clk/10000;
            duty_cfg.threshold_high = period * 75 / 100;
            break;
        case PWM_TESTCASE_FREQ_1M_DUTY_25:
            period = pwm_clk/1000000;
            duty_cfg.threshold_high = period * 25 / 100;
            break;
        case PWM_TESTCASE_FREQ_1M_DUTY_50:
            period = pwm_clk/1000000;
            duty_cfg.threshold_high = period * 50 / 100;
            break;
        case PWM_TESTCASE_FREQ_1M_DUTY_75:
            period = pwm_clk/1000000;
            duty_cfg.threshold_high = period * 75 / 100;
            break;
        case PWM_TESTCASE_FREQ_10M_DUTY_25:
            period = pwm_clk/10000000;
            duty_cfg.threshold_high = period * 25 / 100;
            break;
        case PWM_TESTCASE_FREQ_10M_DUTY_50:
            period = pwm_clk/10000000;
            duty_cfg.threshold_high = period * 50 / 100;
            break;
        case PWM_TESTCASE_FREQ_10M_DUTY_75:
            period = pwm_clk/10000000;
            duty_cfg.threshold_high = period * 75 / 100;
            break;
        default:
            printf("Unknown case\r\n");
            break;
    }
printf("PWM param: period=%lu duty=%u\n",
            period, duty_cfg.threshold_high);
    /*if (period == 0 || duty_cfg.threshold_high > period) {
        printf("PWM param error: period=%lu duty=%u\n",
            period, duty_cfg.threshold_high);
    }*/
    //pwm_ioctl(PWM_ID, PWM_CTRL_FREQUENCE_CONFIG, (void*)period);
    //pwm_ioctl(PWM_ID, PWM_CTRL_DUTYCYCLE_CONFIG, &duty_cfg);

    /* ====== 設定 callback ====== */
    pwm_ioctl(PWM_ID, PWM_CTRL_SET_CALLBACKG, pwm_test_callback);

    /* ====== 每 1 in 週期觸發一次中斷 ====== */
    pwm_ioctl(PWM_ID, PWM_CTRL_IT_PULSE_COUNT_CONFIG, (void*)1);

    /* ====== 啟動 ====== */
    printf("Enable PWM...\n");
    pwm_ioctl(PWM_ID, PWM_CTRL_RESUME, NULL);

    /* ====== 運作 5 秒 ====== */
    /*for (int i = 0; i < 5; i++) {
        printf("PWM running... %d\n", i);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("Close PWM\n");
    pwm_close(PWM_ID);*/
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pwm_test, pwm_test, PWM Test);

static int cmd_pwm_set_duty(int argc, char **argv)
{
    pwm_dutycycle_config_t duty_cfg;
    uint32_t duty = atoi(argv[1]);
    uint16_t period;
    pwm_ioctl(PWM_ID, PWM_CRTL_GET_PERIOD_CONFIG, &period);
    printf("PWM period=%d\n",period);
    duty_cfg.threshold_low = 0;
    duty_cfg.threshold_high = period * duty / 100;

    pwm_ioctl(PWM_ID, PWM_CTRL_DUTYCYCLE_CONFIG, &duty_cfg);

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pwm_set_duty, pwm_set_duty, PWM Set duty);

static int cmd_pwm_set_freq(int argc, char **argv)
{
    uint32_t pwm_clk = 32000000;
    uint32_t period = 0;
    uint32_t freq = atoi(argv[1]);
    period = pwm_clk/freq;
    if (period == 0) {
        printf("PWM param error: period=%lu\n",period);
    }
    pwm_ioctl(PWM_ID, PWM_CTRL_FREQUENCE_CONFIG, (void*)period);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pwm_set_freq, pwm_set_freq, PWM Set freq);

static int cmd_pwm_stop(int argc, char **argv)
{
    int ch = atoi(argv[1]);
    pwm_close(PWM_ID, ch);
    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_pwm_stop, pwm_stop, PWM STOP);
