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
 * @file sensor_link.h
 * @brief CDC-ACM link to IPRO AI Studio's Sensor Teach page (USB 3896:0721).
 *
 * UTF-8 lines ending in "\n" both ways; a binary payload follows a line that
 * gives its length, and a board-to-host payload is followed by "\n" (two when
 * that would end on a whole USB packet). The board sends only while the host
 * holds DTR. Host to board (see README.md for the replies):
 *
 *   HELLO | SRC <name> | SIM <state> | STREAM ON|OFF | LIVE ON|OFF | TINF
 *   TDEP <bytes> <crc32 hex> + payload     write .inm + SNS1 to mfg, reboot
 *   FRONT <bytes> + int16 window            -> IMG 32768 + the int8 input image
 *   INFER 32768 + int8 image                -> OUT 128 + the NPU's 4x4x8 output
 */
#ifndef SENSOR_LINK_H
#define SENSOR_LINK_H

int  sensor_link_start(void);
void sensor_link_print(void);

#endif /* SENSOR_LINK_H */
