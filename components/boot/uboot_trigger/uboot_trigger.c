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
 * @file uboot_trigger.c
 * @brief 1200-baud touch handler for Arduino-style u-boot rescue entry
 *
 * Mirrors the HBN magic that boot2 itself writes when it detects a blank
 * FW partition — see apps/platform/ipro7_uboot_boot2/main.c.
 */

#include "uboot_trigger.h"

#include <stdint.h>

#include "hal_sys.h"

/* HBN_RSV3: survives soft reset, consumed by boot2_usb_download_check */
#define HBN_RSV3_ADDR       0x3008F10Cu
/* "USB" magic in [31:8] + 0xFF timeout code = infinite rescue wait */
#define USB_DL_MAGIC_INF    0x555342FFu

/* Baud value the host uses to signal "please enter bootloader". 1200 matches
 * the Arduino convention so existing tooling (arduino-cli, avrdude -ccarduino,
 * PlatformIO, and our own ipro_dl reboot) all Just Work. */
#define TOUCH_BAUD          1200u

static volatile uint32_t s_last_baud;
static volatile bool     s_prev_dtr;

void uboot_trigger_note_baud(uint32_t baud)
{
    s_last_baud = baud;
}

void uboot_trigger_note_dtr(bool dtr)
{
    const bool prev = s_prev_dtr;
    s_prev_dtr = dtr;

    /* Trigger only on the high→low edge when the last-known baud is 1200.
     * Some hosts toggle DTR twice while opening a port, so the prev-is-high
     * guard avoids firing on the initial sequence. */
    if (prev && !dtr && s_last_baud == TOUCH_BAUD) {
        uboot_trigger_force();
    }
}

void uboot_trigger_force(void)
{
    *(volatile uint32_t *)HBN_RSV3_ADDR = USB_DL_MAGIC_INF;
    __asm__ volatile("fence" ::: "memory");
    hal_reboot();
    /* hal_reboot does not return; this loop is here so the noreturn
     * attribute holds even if a future hal_reboot regression returns. */
    for (;;) { }
}

/* ------------------------------------------------------------------
 * Optional automatic CDC hook installation. Apps that already provide
 * their own usbd_cdc_acm_* overrides should leave this config off and
 * call uboot_trigger_note_baud / _dtr from their own handlers.
 * ------------------------------------------------------------------ */
#ifdef CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS

#include "usbd_cdc.h"

void usbd_cdc_acm_set_line_coding(uint8_t intf, struct cdc_line_coding *line_coding)
{
    (void)intf;
    if (line_coding) {
        uboot_trigger_note_baud(line_coding->dwDTERate);
    }
}

void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    (void)intf;
    uboot_trigger_note_dtr(dtr);
}

#endif /* CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS */
