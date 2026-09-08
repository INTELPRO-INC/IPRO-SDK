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
 * @file device_pskd.h
 * @brief Per-device unique Thread Joiner PSKd (OOB commissioning credential).
 *
 * Each device mints a random 9-char Base32-Thread PSKd on first boot from the
 * HW TRNG and persists it in easyflash (key "dev_pskd"). It is STABLE across
 * reboots AND across OpenThread factory reset (it is not an OT settings key, so
 * `otInstanceFactoryReset` does not wipe it) — the PSKd is a permanent device
 * identity, printed on the device's QR label.
 *
 * Onboarding flow: the device QR encodes (eui64, PSKd); a nurse scans it with
 * the mobile app (same LAN as the gateway); the app POSTs the pair to the
 * gateway's /api/commission, which opens a per-device commissioning window;
 * the device joins with this PSKd via `neck join`.
 *
 * Mirrors device_serial.h (same easyflash + TRNG + resolve-once-cache pattern).
 */
#ifndef DEVICE_PSKD_H
#define DEVICE_PSKD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEVICE_PSKD_GEN_LEN 9   /* length of an auto-minted PSKd (chars)      */
#define DEVICE_PSKD_MAX     32  /* Thread spec max PSKd length                */
#define DEVICE_PSKD_BUFSZ   (DEVICE_PSKD_MAX + 1) /* recommended caller buffer */

/* Fill @p out (out_len >= DEVICE_PSKD_BUFSZ) with the device's PSKd, NUL-
 * terminated. Mints + persists a random one on the first call if the device is
 * not yet provisioned. Safe to call before OpenThread init. */
void device_pskd_get(char *out, size_t out_len);

/* Override the stored PSKd (factory provisioning). Validates length (6..32) and
 * the Base32-Thread charset (0-9, A-Z excluding I,O,Q,Z). Returns 0 on success,
 * -1 on invalid input or flash error. Updates the in-RAM cache too. */
int device_pskd_set(const char *pskd);

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_PSKD_H */
