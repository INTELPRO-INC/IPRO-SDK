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
 * @file usb_console.h
 * @brief Single-CDC-ACM USB console for the fall turnkey apps.
 *
 * One USB CDC-ACM function that fully replaces the UART debug console:
 *   - printf / puts / putchar (and therefore IPRO_LOG) are redirected to
 *     the CDC IN endpoint via the linker --wrap mechanism. The app's
 *     CMakeLists MUST add:
 *        -Wl,--wrap=printf -Wl,--wrap=puts -Wl,--wrap=putchar
 *   - CDC OUT bytes are fed straight into the SDK shell (CONFIG_SHELL),
 *     so every shell command works over USB.
 *
 * Pairs with components/boot/uboot_trigger (CONFIG_BOOT_UBOOT_TRIGGER_ENABLE
 * + AUTO_CDC_HOOKS): a 1200-baud host touch drops into the boot2 USB
 * rescue downloader, so code can be flashed over the same USB port with no
 * exposed UART.
 *
 * USB identity: VID/PID/serial are overridable per app via compile
 * definitions (USB_CONSOLE_VID / USB_CONSOLE_PID / USB_CONSOLE_PRODUCT_*),
 * so the neck and gateway enumerate as distinct devices.
 */

#ifndef __USB_CONSOLE_H__
#define __USB_CONSOLE_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the USB CDC console: create the TX ring + drain task and
 * bring up the CDC device. Call once, before the scheduler starts (it
 * creates a FreeRTOS task) or early in the first app task. Console output
 * issued before this call (and before the host opens the port) is dropped.
 *
 * @return 0 on success, -1 on failure.
 */
int usb_console_init(void);

/** True once the USB host has configured the device (port enumerated). */
bool usb_console_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_CONSOLE_H__ */
