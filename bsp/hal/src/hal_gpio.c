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

#include "drv_glb.h"
#include "drv_gpio.h"
#include "hal_gpio.h"

#define GLB_GPIO_INT0_CLEAR_TIMEOUT (32)

struct gpio_int_cfg_private {
    slist_t list;
    uint32_t pin;
    void (*cbfun)(uint32_t pin);
};

static slist_t gpio_int_head = SLIST_OBJECT_INIT(gpio_int_head);

void gpio_set_mode(uint32_t pin, uint32_t mode)
{
    int int_mode_configured = 0;
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.gpioFun = GPIO_FUN_GPIO;
    gpio_cfg.gpioPin = pin;
    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;

    switch (mode) {
        case GPIO_OUTPUT_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
            break;

        case GPIO_OUTPUT_PP_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;
            break;

        case GPIO_OUTPUT_PD_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_OUTPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;
            break;

        case GPIO_INPUT_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_NONE;
            break;

        case GPIO_INPUT_PP_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_UP;
            break;

        case GPIO_INPUT_PD_MODE:
            gpio_cfg.gpioMode = GPIO_MODE_INPUT;
            gpio_cfg.pullType = GPIO_PULL_DOWN;
            break;
        case GPIO_HZ_MODE:
            GLB_GPIO_Set_HZ(pin);
        default:
            CPU_Interrupt_Disable(GPIO_INT0_IRQn);
            GLB_GPIO_IntMask(pin, MASK);
            int_mode_configured = 1;

            gpio_cfg.gpioMode = GPIO_MODE_INPUT;

            if (mode == GPIO_ASYNC_RISING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
            }

            else if (mode == GPIO_ASYNC_FALLING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            }

            else if (mode == GPIO_ASYNC_HIGH_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
            }

            else if (mode == GPIO_ASYNC_LOW_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
            }

            else if (mode == GPIO_SYNC_RISING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_POS_PULSE);
            }

            else if (mode == GPIO_SYNC_FALLING_TRIGER_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_NEG_PULSE);
            }

            else if (mode == GPIO_SYNC_HIGH_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_DOWN;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_POS_LEVEL);
            }

            else if (mode == GPIO_SYNC_LOW_LEVEL_INT_MODE) {
                gpio_cfg.pullType = GPIO_PULL_UP;
                GLB_Set_GPIO_IntMod(pin, GLB_GPIO_INT_CONTROL_SYNC, GLB_GPIO_INT_TRIG_NEG_LEVEL);
            }

            else {
                /* Unknown mode: undo the quiesce rather than returning with
                 * the shared line switched off for every other pin. */
                CPU_Interrupt_Enable(GPIO_INT0_IRQn);
                return;
            }

            break;
    }

    GLB_GPIO_Init(&gpio_cfg);

    if (int_mode_configured) {
        CPU_Interrupt_Enable(GPIO_INT0_IRQn);
    }
}

void gpio_write(uint32_t pin, uint32_t value)
{
    GLB_GPIO_Write(pin, value);
}

void gpio_toggle(uint32_t pin)
{
    GLB_GPIO_Toggle(pin);
}

int gpio_read(uint32_t pin)
{
    return GLB_GPIO_Read(pin);
}

void gpio_irq_enable(uint32_t pin, uint8_t enabled)
{
    if (enabled) {
        GLB_GPIO_IntMask(pin, UNMASK);
    } else {
        GLB_GPIO_IntMask(pin, MASK);
    }
}

static void GPIO_IRQ(void)
{
    int timeOut = 0;
    slist_t *i;
    slist_for_each(i, &gpio_int_head)
    {
        struct gpio_int_cfg_private *int_cfg = slist_entry(i, struct gpio_int_cfg_private, list);

        if (SET == GLB_Get_GPIO_IntStatus(int_cfg->pin)) {
            if (int_cfg->cbfun)
                int_cfg->cbfun(int_cfg->pin);
            GLB_GPIO_IntClear(int_cfg->pin, SET);
            /* timeout check */
            timeOut = GLB_GPIO_INT0_CLEAR_TIMEOUT;

            do {
                timeOut--;
            } while ((SET == GLB_Get_GPIO_IntStatus(int_cfg->pin)) && timeOut);

            if (!timeOut) {
                //printf("WARNING: Clear GPIO interrupt status fail.\r\n");
            }

            GLB_GPIO_IntClear(int_cfg->pin, RESET);
        }
    }
}

void gpio_attach_irq(uint32_t pin, void (*cbfun)(uint32_t pin))
{
    struct gpio_int_cfg_private *int_cfg = malloc(sizeof(struct gpio_int_cfg_private));
    int_cfg->cbfun = cbfun;
    int_cfg->pin = pin;
    slist_add_tail(&gpio_int_head, &int_cfg->list);
    CPU_Interrupt_Disable(GPIO_INT0_IRQn);
    Interrupt_Handler_Register(GPIO_INT0_IRQn, GPIO_IRQ);
    CPU_Interrupt_Enable(GPIO_INT0_IRQn);
}

