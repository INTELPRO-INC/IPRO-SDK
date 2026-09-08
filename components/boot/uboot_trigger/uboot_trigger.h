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
 * @file uboot_trigger.h
 * @brief 1200-baud touch / runtime trigger for re-entering u-boot rescue
 *
 * Host-side pattern (Arduino-style):
 *   1. Host opens the CDC port at 1200 baud
 *   2. Host closes the port (→ DTR falls low)
 *   3. Firmware catches the DTR-low-after-1200-baud sequence and reboots
 *      with HBN_RSV3 set to the boot2 USB download magic
 *   4. Boot2 detects the magic on restart and enters CDC rescue mode
 *
 * This component provides:
 *   - Weak CDC callback overrides (install automatically when
 *     CONFIG_BOOT_UBOOT_TRIGGER_AUTO_CDC_HOOKS is set)
 *   - Plain C entry points apps can call from their own CDC handlers
 *   - A manual `uboot_trigger_force()` for non-CDC triggers (shell cmd,
 *     crash handler, factory-reset button, etc.)
 *
 * Depends on: hal_reboot() and HBN_RSV3 register at 0x3008F10C (ipro7).
 */
#ifndef UBOOT_TRIGGER_H
#define UBOOT_TRIGGER_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Note the latest CDC line-coding baud rate. Call from your app's
 * usbd_cdc_acm_set_line_coding() callback if you already override it.
 */
void uboot_trigger_note_baud(uint32_t baud);

/**
 * Note the latest CDC DTR state. If DTR falls low after the last baud
 * rate was 1200, this function will trigger `uboot_trigger_force()`
 * and will not return (device reboots). Call from your app's
 * usbd_cdc_acm_set_dtr() callback if you already override it.
 */
void uboot_trigger_note_dtr(bool dtr);

/**
 * Unconditionally arm the boot2 USB rescue magic and soft-reset the
 * MCU. Does not return. Safe to call from any context (writes a single
 * 32-bit HBN register before invoking hal_reboot()).
 *
 * Intended for shell commands, crash recovery paths, or custom triggers
 * beyond 1200-baud touch.
 */
void uboot_trigger_force(void) __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif /* UBOOT_TRIGGER_H */
