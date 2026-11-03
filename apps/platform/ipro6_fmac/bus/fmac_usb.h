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

#ifndef FMAC_USB_H
#define FMAC_USB_H

#include <stdint.h>

/* Loader and FW: the FW takes the configured device over */
#define FMAC_USB_PID          0x1062u

/* Vendor interface FF/FF/FF, bulk IN 0x81 / OUT 0x02, MPS 512.
 * Powers the PHY off and resets the USB block; fmac_usb_poll() attaches
 * after detach_ms */
void fmac_usb_init(uint16_t pid, uint32_t detach_ms);
/* Attaches when due; a bus reset without SET_CONFIGURATION within 1 s
 * detaches, resets and attaches again */
void fmac_usb_poll(void);
/* Keeps the device the loader left configured: no detach, no controller
 * reset; re-opens the bulk endpoints and arms the OUT endpoint */
void fmac_usb_takeover(uint16_t pid);
/* Next received request, or 0 when none */
int fmac_usb_next(uint8_t **pkt, uint32_t *len);
/* fn runs in IRQ context when an OUT transfer lands or an IN one ends */
void fmac_usb_set_notify(void (*fn)(void));
/* Drops off the bus (PHY off, controller reset): the host sees a
 * disconnect */
void fmac_usb_detach(void);
/* No IN transfer queued */
int fmac_usb_tx_done(void);
/* A free IN buffer, *max bytes (NULL if none): fmac_usb_send() of it
 * copies nothing */
uint8_t *fmac_usb_upld_buf(uint32_t *max);
/* OUT buffers the host may fill at once */
uint32_t fmac_usb_dnld_slots(void);
/* Re-arms the OUT endpoint with a buffer from fmac_usb_next() */
void fmac_usb_release(uint8_t *pkt);
/* Queues one IN transfer of len bytes (<= the FMAC_USB_IN_MAX share of one
 * IN buffer); waits for a free IN buffer. A whole-packet length gets 4 zero
 * bytes appended */
int fmac_usb_send(const void *msg, uint32_t len);
/* 1 when an IN buffer is free */
int fmac_usb_tx_idle(void);
/* Prints event counts and controller registers while a host resets the
 * bus and the device is not configured */
void fmac_usb_diag(const char *tag);

#endif
