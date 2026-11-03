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
 * @file reef_usb.c
 * @brief cherryusb's by-name callbacks, handed to the Teach that runs.
 *
 * Each Teach app defines usbd_event_handler (and the serial ones
 * usbd_cdc_acm_set_dtr); built into this firmware (REEF_TEACH_COMBINED) they
 * are renamed <mode>_usbd_..., and these forward to the one this boot started.
 */
#include <stdbool.h>
#include <stdint.h>

#include "reef_mode.h"

void image_usbd_event_handler(uint8_t event);
void voice_usbd_event_handler(uint8_t event);
void sensor_usbd_event_handler(uint8_t event);
void voice_usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr);
void sensor_usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr);

void usbd_event_handler(uint8_t event)
{
    switch (reef_mode_current()) {
    case REEF_MODE_VOICE:
        voice_usbd_event_handler(event);
        break;
    case REEF_MODE_SENSOR:
        sensor_usbd_event_handler(event);
        break;
    default:
        image_usbd_event_handler(event);
        break;
    }
}

/* the image Teach's serial port does not use DTR (cherryusb's default did nothing) */
void usbd_cdc_acm_set_dtr(uint8_t intf, bool dtr)
{
    switch (reef_mode_current()) {
    case REEF_MODE_VOICE:
        voice_usbd_cdc_acm_set_dtr(intf, dtr);
        break;
    case REEF_MODE_SENSOR:
        sensor_usbd_cdc_acm_set_dtr(intf, dtr);
        break;
    default:
        break;
    }
}
