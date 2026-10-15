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
 * @file keys.c
 * @brief Key poller. See keys.h for the wiring.
 */

#include "keys.h"
#include "board_config.h"

#include <FreeRTOS.h>
#include <task.h>

#include "hal_gpio.h"
#include "drv_glb.h"
#include "ipro_log.h"

#define LOG_TAG "KEYS"

#define KEY_POLL_MS        10
#define KEY_DEBOUNCE_POLLS 3     /* 30 ms stable before a change is reported */

typedef struct {
    const char *name;
    uint8_t     pin;
    uint8_t     active_level;
    uint8_t     pull_mode;       /* GPIO_INPUT_*_MODE */
    bool        shares_aux;
} key_desc_t;

static const key_desc_t s_desc[KEY_COUNT] = {
    [KEY_BOOT] = { "BOOT/SW1", BOARD_KEY_BOOT_PIN, 1, GPIO_INPUT_PD_MODE, false },
    [KEY_SW3]  = { "SW3",      BOARD_KEY_SW3_PIN,  0, GPIO_INPUT_PP_MODE, true  },
    [KEY_SW4]  = { "SW4",      BOARD_KEY_SW4_PIN,  0, GPIO_INPUT_PP_MODE, false },
    [KEY_SW5]  = { "SW5",      BOARD_KEY_SW5_PIN,  0, GPIO_INPUT_PP_MODE, true  },
};

static volatile bool     s_pressed[KEY_COUNT];
static volatile uint32_t s_count[KEY_COUNT];
static uint8_t           s_stable[KEY_COUNT];
static volatile bool     s_aux_in_use;
static key_cb_t          s_cb;
static void             *s_cb_ctx;
static TaskHandle_t      s_task;

static void key_pad_input(const key_desc_t *d)
{
    uint8_t pin[1] = { d->pin };

    GLB_GPIO_Func_Init(GPIO_FUN_GPIO, pin, 1);
    gpio_set_mode(d->pin, d->pull_mode);
}

static bool key_raw(const key_desc_t *d)
{
    return gpio_read(d->pin) == d->active_level;
}

static void keys_task(void *arg)
{
    (void)arg;

    for (;;) {
        for (int k = 0; k < KEY_COUNT; k++) {
            const key_desc_t *d = &s_desc[k];
            bool now;

            if (d->shares_aux && s_aux_in_use) {
                continue;
            }
            now = key_raw(d);
            if (now == s_pressed[k]) {
                s_stable[k] = 0;
                continue;
            }
            if (++s_stable[k] < KEY_DEBOUNCE_POLLS) {
                continue;
            }
            s_stable[k]  = 0;
            s_pressed[k] = now;
            if (now) {
                s_count[k]++;
            }
            if (s_cb) {
                s_cb((key_id_t)k, now, s_cb_ctx);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(KEY_POLL_MS));
    }
}

int keys_init(void)
{
    if (s_task) {
        return 0;
    }
    for (int k = 0; k < KEY_COUNT; k++) {
        key_pad_input(&s_desc[k]);
        s_pressed[k] = key_raw(&s_desc[k]);
    }
    if (xTaskCreate(keys_task, "keys", 1024, NULL, 2, &s_task) != pdPASS) {
        IPRO_LOGE(LOG_TAG, "task create failed");
        return -1;
    }
    return 0;
}

void keys_set_callback(key_cb_t cb, void *ctx)
{
    s_cb_ctx = ctx;
    s_cb     = cb;
}

bool keys_is_pressed(key_id_t key)
{
    return (key < KEY_COUNT) ? s_pressed[key] : false;
}

uint32_t keys_press_count(key_id_t key)
{
    return (key < KEY_COUNT) ? s_count[key] : 0u;
}

void keys_set_aux_mode(bool aux_in_use)
{
    s_aux_in_use = aux_in_use;
    if (aux_in_use) {
        return;
    }
    /* The capture left GPIO10/11 as analogue pads; take them back. */
    for (int k = 0; k < KEY_COUNT; k++) {
        if (s_desc[k].shares_aux) {
            key_pad_input(&s_desc[k]);
            s_pressed[k] = key_raw(&s_desc[k]);
        }
    }
}

const char *keys_name(key_id_t key)
{
    return (key < KEY_COUNT) ? s_desc[key].name : "?";
}

int keys_pin(key_id_t key)
{
    return (key < KEY_COUNT) ? s_desc[key].pin : -1;
}
