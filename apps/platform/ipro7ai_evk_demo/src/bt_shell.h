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
 * @file bt_shell.h
 * @brief Drive every peripheral over BLE by bridging to the existing shell.
 *
 * Rather than a characteristic per peripheral, this exposes the shell itself.
 * The LCD, camera, speaker, microphone, ADC, SD card and video player already
 * have commands, so one bridge reaches all of them - and anything added later
 * is exposed with no extra work.
 *
 * The UUIDs are Nordic's UART Service, so ordinary phone apps talk to it
 * without anything custom:
 *
 *   service 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   RX      6E400002-...  write a command line here
 *   TX      6E400003-...  subscribe for the output
 *
 * Commands run on a worker task, never on the host's RX thread: shell commands
 * block for seconds at a time (`spk tone`, `vid play`) and stalling that thread
 * stops HCI events being processed.
 */
#ifndef BT_SHELL_H
#define BT_SHELL_H

#include <stdbool.h>
#include <stdint.h>

/** @brief Register the service and start the worker. Call after bt_enable(). */
int  bt_shell_init(void);

/** @brief True once a client has subscribed to the output characteristic. */
bool bt_shell_subscribed(void);

uint32_t bt_shell_commands_run(void);

#endif /* BT_SHELL_H */
