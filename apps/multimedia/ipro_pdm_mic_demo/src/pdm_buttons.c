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

#include "pdm_app.h"

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include "hal_gpio.h"
#include "hal_lp.h"
#include "ipro_log.h"

#define TAG "PDM_BTN"

#define BTN_POWER_PIN       5U
#define BTN_VOL_UP_PIN      6U
#define BTN_VOL_DOWN_PIN    7U
#define BTN_MASK            ((1U << BTN_POWER_PIN) | \
                             (1U << BTN_VOL_UP_PIN) | \
                             (1U << BTN_VOL_DOWN_PIN))
#define BTN_QUEUE_LEN       8U
#define BTN_TASK_STACK      512U
#define BTN_TASK_PRIO       11U
#define BTN_DEBOUNCE_MS     120U
#define BTN_SETTLE_MS       20U
#define BTN_GAIN_STEP_DB    3

static QueueHandle_t s_button_queue;
static TickType_t s_last_tick[3];

static int button_index(uint32_t pin)
{
    switch (pin) {
    case BTN_POWER_PIN:
        return 0;
    case BTN_VOL_UP_PIN:
        return 1;
    case BTN_VOL_DOWN_PIN:
        return 2;
    default:
        return -1;
    }
}

static void pdm_buttons_config_runtime_gpio(void)
{
    gpio_set_mode(BTN_POWER_PIN, GPIO_ASYNC_FALLING_TRIGER_INT_MODE);
    gpio_set_mode(BTN_VOL_UP_PIN, GPIO_ASYNC_FALLING_TRIGER_INT_MODE);
    gpio_set_mode(BTN_VOL_DOWN_PIN, GPIO_ASYNC_FALLING_TRIGER_INT_MODE);
    gpio_irq_enable(BTN_POWER_PIN, 1);
    gpio_irq_enable(BTN_VOL_UP_PIN, 1);
    gpio_irq_enable(BTN_VOL_DOWN_PIN, 1);
}

static void pdm_button_irq(uint32_t pin)
{
    BaseType_t woken = pdFALSE;

    if (s_button_queue != NULL) {
        (void)xQueueSendFromISR(s_button_queue, &pin, &woken);
    }
    portYIELD_FROM_ISR(woken);
}

static int pdm_button_wakeup_hook(int wakeup_reason, void *arg)
{
    uint32_t bits;
    uint32_t pin;

    (void)arg;
    IPRO_LOGI(TAG, "init: config_runtime_gpio");
    pdm_buttons_config_runtime_gpio();
    IPRO_LOGI(TAG, "init: lp_gpio_wakeup_cfg");

    bits = hal_lp_get_wakeup_gpio() & BTN_MASK;
    if (((uint32_t)wakeup_reason & HAL_LP_WAKEUP_GPIO) != 0U) {
        if (bits == 0U) {
            if (gpio_read(BTN_POWER_PIN) == 0) {
                bits |= (1U << BTN_POWER_PIN);
            }
            if (gpio_read(BTN_VOL_UP_PIN) == 0) {
                bits |= (1U << BTN_VOL_UP_PIN);
            }
            if (gpio_read(BTN_VOL_DOWN_PIN) == 0) {
                bits |= (1U << BTN_VOL_DOWN_PIN);
            }
        }
        for (pin = 0U; pin < 32U; pin++) {
            if ((bits & (1U << pin)) != 0U) {
                (void)xQueueSend(s_button_queue, &pin, 0U);
            }
        }
    }

    return 0;
}

static void pdm_button_handle(uint32_t pin, bool require_low_after_settle)
{
    int idx = button_index(pin);
    TickType_t now;

    if (idx < 0) {
        return;
    }

    if (require_low_after_settle) {
        vTaskDelay(pdMS_TO_TICKS(BTN_SETTLE_MS));
        if (gpio_read(pin) != 0) {
            return;
        }
    }

    now = xTaskGetTickCount();
    if ((now - s_last_tick[idx]) < pdMS_TO_TICKS(BTN_DEBOUNCE_MS)) {
        return;
    }
    s_last_tick[idx] = now;

    switch (pin) {
    case BTN_POWER_PIN: {
        extern int pdm_sleep_enter(uint32_t seconds);

        /*
         * Wait for the button to come back up before sleeping.
         *
         * The wake is armed on a LOW level, so entering PDS while the key is
         * still held means the wake condition is already true and the part
         * comes straight back out. Bounded, so a stuck-low pin cannot park the
         * button task here forever.
         */
        IPRO_LOGI(TAG, "GPIO5 power -> entering PDS31, release the key");
        for (int i = 0; i < 200 && gpio_read(pin) == 0; i++) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        if (gpio_read(pin) == 0) {
            IPRO_LOGW(TAG, "GPIO5 still low after 2 s - not sleeping");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(BTN_DEBOUNCE_MS));
        (void)pdm_sleep_enter(0U);
        break;
    }
    case BTN_VOL_UP_PIN:
        pdm_set_gain(pdm_get_gain() + BTN_GAIN_STEP_DB);
        IPRO_LOGI(TAG, "GPIO6 volume up -> gain %d dB", pdm_get_gain());
        break;
    case BTN_VOL_DOWN_PIN:
        pdm_set_gain(pdm_get_gain() - BTN_GAIN_STEP_DB);
        IPRO_LOGI(TAG, "GPIO7 volume down -> gain %d dB", pdm_get_gain());
        break;
    default:
        break;
    }
}

static void pdm_button_task(void *arg)
{
    uint32_t pin;

    (void)arg;
    for (;;) {
        if (xQueueReceive(s_button_queue, &pin, portMAX_DELAY) == pdTRUE) {
            pdm_button_handle(pin, true);
        }
    }
}

int pdm_buttons_init(void)
{
    static hal_lp_gpio_pin_cfg_t wake_pins[] = {
        {
            .pin = BTN_POWER_PIN,
            .enable = 1,
            .trig_mode = HAL_LP_GPIO_TRIG_LOW_LEVEL,
            .res_mode = HAL_LP_GPIO_RES_PULL_UP,
        },
        {
            .pin = BTN_VOL_UP_PIN,
            .enable = 1,
            .trig_mode = HAL_LP_GPIO_TRIG_LOW_LEVEL,
            .res_mode = HAL_LP_GPIO_RES_PULL_UP,
        },
        {
            .pin = BTN_VOL_DOWN_PIN,
            .enable = 1,
            .trig_mode = HAL_LP_GPIO_TRIG_LOW_LEVEL,
            .res_mode = HAL_LP_GPIO_RES_PULL_UP,
        },
    };
    static hal_lp_gpio_cfg_t wake_cfg = {
        .wakeup_mask = BTN_MASK,
        .default_trig = HAL_LP_GPIO_TRIG_LOW_LEVEL,
        .default_res = HAL_LP_GPIO_RES_PULL_UP,
        .pin_cfg = wake_pins,
        .pin_cfg_count = sizeof(wake_pins) / sizeof(wake_pins[0]),
    };

    s_button_queue = xQueueCreate(BTN_QUEUE_LEN, sizeof(uint32_t));
    if (s_button_queue == NULL) {
        IPRO_LOGE(TAG, "button queue alloc failed");
        return -1;
    }

    IPRO_LOGI(TAG, "init: attach_irq");
    gpio_attach_irq(BTN_POWER_PIN, pdm_button_irq);
    gpio_attach_irq(BTN_VOL_UP_PIN, pdm_button_irq);
    gpio_attach_irq(BTN_VOL_DOWN_PIN, pdm_button_irq);
    pdm_buttons_config_runtime_gpio();

    (void)hal_lp_gpio_wakeup_cfg(&wake_cfg);
    IPRO_LOGI(TAG, "init: register_wakeup_hook");
    (void)hal_lp_register_wakeup_hook(pdm_button_wakeup_hook, NULL);
    IPRO_LOGI(TAG, "init: task");

    if (xTaskCreate(pdm_button_task, "pdm_btn", BTN_TASK_STACK, NULL,
                    BTN_TASK_PRIO, NULL) != pdPASS) {
        IPRO_LOGE(TAG, "button task create failed");
        return -1;
    }

    IPRO_LOGI(TAG, "buttons active-low: GPIO5 power, GPIO6 volume up, GPIO7 volume down");
    return 0;
}
