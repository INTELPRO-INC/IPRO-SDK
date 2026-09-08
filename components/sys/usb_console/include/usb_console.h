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

/*
 * USB CDC-ACM debug console (shared, ipro6 + ipro7).
 *
 * When CONFIG_DEBUG_CONSOLE_USB is set, firmware printf/log output is mirrored
 * to a USB CDC-ACM IN endpoint and FinSH shell input is additionally sourced
 * from the matching OUT endpoint -- the UART console keeps working unchanged
 * (mirror mode), so early-boot and the flashtool console path stay intact.
 *
 * The output sink is wired into the libc retarget (components/libc/newlibc/
 * debug.c): every printf/puts/putchar funnels through usb_console_write().
 * The input source is the shell's public shell_exe_cmd(), driven from the CDC
 * OUT callback inside the implementation.
 *
 * When the config is unset, all entry points are no-ops so callers compile
 * cleanly with no #ifdef at the call site.
 */
#ifndef USB_CONSOLE_H
#define USB_CONSOLE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Bring up the CDC-ACM console: ring + drain task + USB device.
 * Call once after the scheduler is running (heap + FreeRTOS up) and after the
 * USB clock is available (BROM leaves WIFIPLL up at boot). Returns 0 on
 * success, <0 on failure. No-op returning 0 when the feature is disabled.
 */
int usb_console_init(void);

/* attach 模式(CONFIG_DEBUG_CONSOLE_USB_ATTACH):console 掛進 app 的複合
 * USB 裝置。app 先 usbd_desc_register 含第二組 ACM 的描述元,呼叫這支加
 * interfaces/endpoints,最後自己 usbd_initialize;事件轉呼 notify。 */
int  usb_console_attach(void);
void usb_console_notify_event(uint8_t event);

/*
 * Queue text for the console IN endpoint. Never blocks -- on ring overflow the
 * oldest bytes are dropped so the firmware is never stalled by a closed host
 * port. Safe from task or ISR context. Returns bytes accepted (0 before init
 * or when disabled). LF is expanded to CRLF internally.
 */
uint32_t usb_console_write(const void *buf, uint32_t len);

/* True once the host has set the CDC configuration (port open-able). */
bool usb_console_is_connected(void);

#ifdef __cplusplus
}
#endif

#endif /* USB_CONSOLE_H */
