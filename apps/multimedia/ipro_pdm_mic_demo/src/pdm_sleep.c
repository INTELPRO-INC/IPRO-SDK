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
 * @file pdm_sleep.c
 * @brief Manual PDS31 entry with GPIO wake, for the button board.
 *
 * This is deliberately not the BLE-driven automatic PDS path: that one is
 * gated by CONFIG_IPRO_PDM_MIC_AUTO_PDS and stays off. Here the application
 * decides when to sleep and the user decides when to wake, which keeps the
 * connected-mode keepalive problems out of the picture entirely.
 *
 * PDS rather than HBN, because of what can wake each:
 *
 *   HBN wakes only from the ten AON pads (GPIO 8/9/10/11/12/13/14/22/30/31).
 *   The buttons on this board are GPIO5/6/7, none of which is on that list,
 *   so HBN could be entered but never left by pressing a key.
 *
 *   PDS31 resets and clock-gates PD_CORE_MISC_DIG but leaves the GPIO pad
 *   logic powered, and PDS_WAKEUP_SRC_GPIO_IRQ covers GPIO0..41. It also
 *   resumes in place with OCRAM retained, so the recording buffer and the
 *   BLE state survive.
 */

#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "hal_pm.h"
#include "hal_gpio.h"
#include "drv_pds.h"
#include "ipro_log.h"
#include "platform_common.h"
#include "pdm_app.h"
#include "ble_audio.h"

#define TAG "PDM_SLEEP"

/* The three buttons, all pulled up through 20k and shorted to ground when
 * pressed - hence a LOW-level trigger. ASYNC, not SYNC: the SYNC variants are
 * clocked, and PDS stops that clock. */
#define SLEEP_WAKE_PIN_POWER    5U
#define SLEEP_WAKE_PIN_VOL_UP   6U
#define SLEEP_WAKE_PIN_VOL_DOWN 7U

static void pdm_sleep_console_restore(void)
{
    /* PDS31 resets PD_CORE_MISC_DIG, which is where UART0 lives, so the
     * console comes back mute and the board looks hung. Re-running the
     * platform's own console setup is enough. This must not be conditional on
     * CONFIG_SHELL: without it a log-only build wakes up silent too, and that
     * is exactly the case where the silence is hardest to diagnose. */
    platform_init(CONFIG_PLATFORM_UART_BAUDRATE);
}

int pdm_sleep_probe(void)
{
    /* Arm the same pins the real path arms, then report instead of sleeping.
     * The interesting question is whether PDS_GPIO_STAT tracks the pin when it
     * is grounded: if it does not, PDS cannot see the pad at all and no wake
     * routing will help. */
    (void)pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_POWER, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    (void)pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_VOL_UP, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    (void)pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_VOL_DOWN, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    pm_pds_gpio_wakeup_dump();
    return 0;
}

int pdm_sleep_enter(uint32_t seconds)
{
    int rc;

    rc = pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_POWER, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    rc |= pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_VOL_UP, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    rc |= pm_pds_gpio_wakeup_set(SLEEP_WAKE_PIN_VOL_DOWN, PDS_GPIO_INT_ASYNC_LOW_LEVEL);
    if (rc != 0) {
        IPRO_LOGE(TAG, "no PDS interrupt group for the wake pins");
        return -1;
    }

    /* Hand the peer a disconnect while the radio can still transmit one. */
    bool ble_was_up = ble_audio_is_started();
    if (ble_was_up) {
        (void)ble_audio_suspend_for_sleep();
    }

    IPRO_LOGI(TAG, "entering PDS31; press power/vol-up/vol-down to wake%s",
              seconds ? " (or wait for the timer)" : "");
    printf("\r\n");   /* flush the line before the UART domain is reset */

    /* sleep_time is in 32768 Hz RTC ticks; 0 means no timed wake. */
    pm_pds_mode_enter(PM_PDS_LEVEL_31, seconds * 32768u);

    pdm_sleep_console_restore();

    int pin = pm_pds_gpio_wakeup_source();
    if (pin >= 0) {
        IPRO_LOGI(TAG, "woke on GPIO%d", pin);
    } else {
        IPRO_LOGI(TAG, "woke (not a GPIO source - timer or other)");
    }
    pm_pds_gpio_wakeup_clear();

    /*
     * The CPU resumes here with OCRAM intact, but the radio does not come back
     * with it: pm_pds_mode_enter() asks for PM_PDS_RF_POWER_OFF, and PDS31
     * resets PD_CORE_MISC_DIG underneath the controller. Its registers, its
     * calibration and the link are all gone.
     *
     * The host is worse off than that, because its RAM *did* survive: bt_dev
     * still believes it is advertising and still holds whatever conn state it
     * had, so bt_le_adv_start() answers -EALREADY and nothing ever gets back
     * on air. The controller offers no re-init entry point to reconcile the
     * two - ipro_ble_ctlr_controller_init() is a one-shot - so a reset is what
     * is actually available, not a shortcut around one.
     *
     * It also matches what the button means. This is the power key: the device
     * went off and is now coming on. Everything needed to be useful again
     * survives in flash - the bond most of all, which is why settings are
     * loaded before advertising in ble_audio.c - so the phone reconnects
     * without the user being asked to pair. The recording buffer does not
     * survive, and after a deliberate power-off it should not.
     *
     * When BLE was never up there is nothing to reconcile, so those builds
     * keep resuming in place.
     */
    if (ble_was_up) {
        extern void hal_reboot(void);

        IPRO_LOGI(TAG, "radio was powered down - resetting to reconnect");
        /* Let the line reach the console before the UART goes with it. */
        vTaskDelay(pdMS_TO_TICKS(20));
        hal_reboot();
        /* not reached */
    }
    return 0;
}
