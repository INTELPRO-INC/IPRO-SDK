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
 * @file uvc_teach_link.h
 * @brief CDC-ACM beside the UVC stream (CONFIG_UVC_TEACH): IPRO AI Studio
 *        deploys a Teach model over the same USB cable it watches the camera on.
 *
 * Host -> device, one 16-byte header, little-endian, then the payload:
 *
 *   "TDEP" | u32 size | u32 crc32 | u32 0     payload = .inm + label block
 *   "TINF" | 0 | 0 | 0                        no payload
 *
 * Device -> host, one text line each:
 *
 *   TDEP OK <size> <crc32>        written to mfg+0x1000; the board reboots
 *   TDEP ERR <reason>
 *   TINF <model bytes> <classes> <label>,<label>,...
 *   TINF BUSY                     the model is still loading: ask again
 *
 * The payload is staged in PSRAM by the USB ISR, checked, and only then
 * written: a bad or short transfer never touches the model in flash.
 */
#ifndef UVC_TEACH_LINK_H
#define UVC_TEACH_LINK_H

#include <stdint.h>

/* CDC interfaces 2/3 after the video function's 0/1; endpoints clear of the
 * video's 0x81. uvc.c builds the descriptor from these. */
#define UVC_TEACH_LINK_FIRST_INTF 0x02
#define UVC_TEACH_LINK_INT_EP     0x83
#define UVC_TEACH_LINK_OUT_EP     0x02
#define UVC_TEACH_LINK_IN_EP      0x84
#define UVC_TEACH_LINK_MPS        64      /* full speed bulk */

/** @brief Add the CDC interfaces and endpoints. Call between the video
 *         function's registration and usbd_initialize(). */
void uvc_teach_link_add(void);

/** @brief Start the deploy task; needs the scheduler. */
int uvc_teach_link_start(void);

/** @brief Print the link's receive state (for `teach link`). */
void uvc_teach_link_print(void);

/** @brief cherryusb device events, forwarded by uvc.c's usbd_event_handler. */
void uvc_teach_link_event(uint8_t event);

#endif /* UVC_TEACH_LINK_H */
