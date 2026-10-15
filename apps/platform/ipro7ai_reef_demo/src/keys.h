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
 * @file keys.h
 * @brief The four readable keys, polled with debounce.
 *
 *   SW1 BOOT  GPIO41  ACTIVE HIGH (R27 20k pull-down, pressed -> VDDIO1)
 *   SW3       GPIO10  active low, internal pull-up   (shares AIN4 / AUX-IN L)
 *   SW4       GPIO27  active low, R78 100k pull-up
 *   SW5       GPIO11  active low, internal pull-up   (shares AIN5 / AUX-IN R)
 *
 * SW2 is the hardware reset and cannot be read. SW3/SW5 sit on the AUX-IN
 * pins, so the poller releases them while the AUX-IN capture owns those pads
 * (keys_set_aux_mode()).
 */
#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    KEY_BOOT = 0,   /* SW1 */
    KEY_SW3,
    KEY_SW4,
    KEY_SW5,
    KEY_COUNT
} key_id_t;

typedef void (*key_cb_t)(key_id_t key, bool pressed, void *ctx);

/** Configure the pads and start the 10 ms poll task. */
int keys_init(void);

/** Register a single press/release callback (called from the poll task). */
void keys_set_callback(key_cb_t cb, void *ctx);

/** Current debounced state. */
bool keys_is_pressed(key_id_t key);

/** Press counter since boot, for `btn status`. */
uint32_t keys_press_count(key_id_t key);

/** While true, GPIO10/11 are left alone (AUX-IN capture owns them). */
void keys_set_aux_mode(bool aux_in_use);

const char *keys_name(key_id_t key);
int keys_pin(key_id_t key);

#endif /* KEYS_H */
