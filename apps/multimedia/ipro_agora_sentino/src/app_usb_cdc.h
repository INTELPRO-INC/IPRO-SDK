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

#ifndef APP_USB_CDC_H
#define APP_USB_CDC_H

#include <stdint.h>

/**
 * Initialize USB CDC ACM virtual serial port.
 * Device appears as /dev/cu.usbmodem* on macOS.
 */
void app_usb_cdc_init(void);

/**
 * Write data to USB CDC ACM TX.
 * @return number of bytes written, or -1 on error.
 */
int app_usb_cdc_write(const char *data, uint32_t len);

/**
 * Printf-style write to USB CDC ACM.
 * @return number of bytes written, or -1 on error.
 */
int app_usb_cdc_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif /* APP_USB_CDC_H */
