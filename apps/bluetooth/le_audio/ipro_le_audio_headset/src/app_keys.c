/**
 * @file app_keys.c
 * @brief Board keys -> VCP volume renderer (IPRO7AI Reef)
 *
 *   SW3  volume down   } the pair on the top edge, beside LED1
 *   SW4  volume up     }
 *   SW5  mute toggle     (bottom edge, next to the headphone jack)
 *
 * The keys go through the VCP volume renderer, as a headset's own buttons
 * should: a connected phone gets the new volume state notified and moves its
 * slider, and with nothing connected they still set the local level through
 * the same state callback (vcp_server.c) that applies a phone's writes.
 *
 * Polled every KEY_POLL_MS with a two-read debounce; holding a volume key
 * repeats every KEY_REPEAT_MS after KEY_REPEAT_DELAY_MS. All three keys are
 * active low: SW3/SW5 rely on the internal pull-up (they have no external
 * one), SW4 also has R78 100k. GPIO10/11 are the AUX-IN pins too; this app
 * never samples AUX-IN, so they are plain keys here.
 *
 * Copyright (c) 2026 INTELPRO
 */

#include "app_keys.h"
#include "vcp_server.h"
#include "board_config.h"
#include "ipro_log.h"

#include <FreeRTOS.h>
#include <task.h>
#include <hal_gpio.h>
#include <zephyr/bluetooth/audio/vcp.h>

#define TAG "KEYS"

#define KEY_POLL_MS             20
#define KEY_REPEAT_DELAY_MS     500
#define KEY_REPEAT_MS           200
#define KEY_TASK_STACK_WORDS    768     /* VCP state callback + log line */
#define KEY_TASK_PRIORITY       4

typedef enum {
    KEY_VOL_DOWN,
    KEY_VOL_UP,
    KEY_MUTE,
    KEY_COUNT
} key_id_t;

static const struct {
    uint8_t     pin;
    bool        repeats;
    const char *name;
} s_keys[KEY_COUNT] = {
    [KEY_VOL_DOWN] = { BOARD_KEY_SW3_PIN, true,  "SW3 volume down" },
    [KEY_VOL_UP]   = { BOARD_KEY_SW4_PIN, true,  "SW4 volume up" },
    [KEY_MUTE]     = { BOARD_KEY_SW5_PIN, false, "SW5 mute" },
};

static void key_action(key_id_t k)
{
    int err;

    if (!vcp_server_is_ready()) {
        IPRO_LOGW(TAG, "%s: volume control not up yet", s_keys[k].name);
        return;
    }

    switch (k) {
    case KEY_VOL_DOWN:
        err = bt_vcp_vol_rend_vol_down();
        break;
    case KEY_VOL_UP:
        /* Turning it up while muted should be heard, as on a phone. */
        err = bt_vcp_vol_rend_unmute_vol_up();
        break;
    default:
        err = vcp_server_is_muted() ? bt_vcp_vol_rend_unmute() : bt_vcp_vol_rend_mute();
        break;
    }

    if (err != 0) {
        IPRO_LOGW(TAG, "%s: VCP error %d", s_keys[k].name, err);
    }
}

static void keys_task(void *arg)
{
    bool     last[KEY_COUNT] = { false };    /* previous raw read */
    bool     down[KEY_COUNT] = { false };    /* debounced state */
    uint32_t held_ms[KEY_COUNT] = { 0 };

    (void)arg;

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(KEY_POLL_MS));

        for (int k = 0; k < KEY_COUNT; k++) {
            bool now = gpio_read(s_keys[k].pin) == 0;

            if (now != last[k]) {
                last[k] = now;          /* changed: wait for a second read */
                continue;
            }
            if (now != down[k]) {
                down[k] = now;
                held_ms[k] = 0;
                if (now) {
                    IPRO_LOGI(TAG, "%s", s_keys[k].name);
                    key_action((key_id_t)k);
                }
                continue;
            }
            if (now && s_keys[k].repeats) {
                held_ms[k] += KEY_POLL_MS;
                if (held_ms[k] >= KEY_REPEAT_DELAY_MS) {
                    key_action((key_id_t)k);
                    held_ms[k] -= KEY_REPEAT_MS;
                }
            }
        }
    }
}

int app_keys_init(void)
{
    for (int k = 0; k < KEY_COUNT; k++) {
        gpio_set_mode(s_keys[k].pin, GPIO_INPUT_PP_MODE);
    }

    if (xTaskCreate(keys_task, "keys", KEY_TASK_STACK_WORDS, NULL,
                    KEY_TASK_PRIORITY, NULL) != pdPASS) {
        IPRO_LOGE(TAG, "key task create failed");
        return -1;
    }

    IPRO_LOGI(TAG, "Keys: SW3 vol-, SW4 vol+, SW5 mute (GPIO%d/%d/%d)",
              BOARD_KEY_SW3_PIN, BOARD_KEY_SW4_PIN, BOARD_KEY_SW5_PIN);
    return 0;
}
