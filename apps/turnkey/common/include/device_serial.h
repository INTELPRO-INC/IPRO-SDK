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
 * @file device_serial.h
 * @brief Per-device unique 8-byte serial for boards without a unique efuse MAC.
 *
 * The dev-board efuse has no unique factory EUI64, so every chip reads the same
 * MAC. This mints a random locally-administered 8-byte value on first boot
 * (hardware TRNG), persists it in easyflash, and returns the same value on
 * every subsequent boot. It is the single source of identity used for:
 *   - the USB CDC serial string (so multiple boards enumerate as distinct
 *     /dev/cu.usbmodem* ports on one host),
 *   - the OpenThread IEEE extended address (so SEDs don't collide on the mesh),
 *   - the CoAP device id.
 */

#ifndef __DEVICE_SERIAL_H__
#define __DEVICE_SERIAL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fill @p out with the device's unique 8-byte serial (locally-administered,
 * unicast — out[0] has the U/L bit set, the multicast bit cleared). Stable
 * across reboots. Safe to call before OpenThread init (uses the HW TRNG +
 * easyflash directly).
 */
void device_serial_get(uint8_t out[8]);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_SERIAL_H__ */
