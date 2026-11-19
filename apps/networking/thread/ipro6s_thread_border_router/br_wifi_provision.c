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

#include "br_wifi_provision.h"
#include <FreeRTOS.h>
#include <task.h>
#include <string.h>

enum provision_state { LOCKED, IDLE, RECEIVING, READY, PROCESSING, REBOOT, SEALED };
static enum provision_state sState;
static struct br_wifi_profile sInput;
static unsigned sDigits;
static bool sInvalid;
_Static_assert(sizeof(sInput) == 104, "UART BW01 record size changed");

void br_wifi_provision_open(void)
{
    taskENTER_CRITICAL();
    if (sState == LOCKED) sState = IDLE;
    taskEXIT_CRITICAL();
}

int br_wifi_provision_arm(void)
{
    taskENTER_CRITICAL();
    const bool available = sState == IDLE;
    if (available) {
        br_wifi_profile_wipe(&sInput, sizeof(sInput));
        sDigits = 0;
        sInvalid = false;
        sState = RECEIVING;
    }
    taskEXIT_CRITICAL();
    return available ? 0 : -1;
}

static int nibble(uint8_t data)
{
    if (data >= '0' && data <= '9') return data - '0';
    if (data >= 'a' && data <= 'f') return data - 'a' + 10;
    if (data >= 'A' && data <= 'F') return data - 'A' + 10;
    return -1;
}

int br_wifi_provision_input(uint8_t data)
{
    taskENTER_CRITICAL();
    if (sState != RECEIVING) {
        const bool consumed = sState == READY || sState == PROCESSING;
        taskEXIT_CRITICAL();
        return consumed;
    }
    if (data == 3) {
        br_wifi_profile_wipe(&sInput, sizeof(sInput));
        sState = IDLE;
    } else if (data == '\n') {
        sInvalid |= sDigits != 2 * sizeof(sInput);
        sState = READY;
    } else if (data != '\r') {
        const int value = nibble(data);
        if (value < 0 || sDigits >= 2 * sizeof(sInput)) sInvalid = true;
        if (!sInvalid) {
            uint8_t *bytes = (void *)&sInput;
            bytes[sDigits / 2] = (uint8_t)((bytes[sDigits / 2] << 4) | value);
            ++sDigits;
        }
    }
    taskEXIT_CRITICAL();
    return 1;
}

int br_wifi_provision_take(struct br_wifi_profile *output)
{
    if (!output) return -1;
    br_wifi_profile_wipe(output, sizeof(*output));
    taskENTER_CRITICAL();
    if (sState != READY) {
        taskEXIT_CRITICAL();
        return 0;
    }
    *output = sInput;
    const bool invalid = sInvalid;
    br_wifi_profile_wipe(&sInput, sizeof(sInput));
    sState = PROCESSING;
    taskEXIT_CRITICAL();
    if (invalid || !br_wifi_profile_valid(output, sizeof(*output))) {
        br_wifi_profile_wipe(output, sizeof(*output));
        return -1;
    }
    return 1;
}

void br_wifi_provision_finish(bool saved)
{
    taskENTER_CRITICAL();
    if (sState == PROCESSING) sState = saved ? REBOOT : IDLE;
    taskEXIT_CRITICAL();
}

bool br_wifi_provision_reboot_required(void)
{
    taskENTER_CRITICAL();
    const bool required = sState == REBOOT;
    taskEXIT_CRITICAL();
    return required;
}

int br_wifi_provision_seal(void)
{
    taskENTER_CRITICAL();
    if (sState == IDLE || sState == LOCKED) sState = SEALED;
    const bool sealed = sState == SEALED;
    taskEXIT_CRITICAL();
    return sealed ? 0 : 1;
}
