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

#ifndef BR_MANAGEMENT_H
#define BR_MANAGEMENT_H
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
enum response_status { BR_BLE_STATUS_OK, BR_BLE_STATUS_INVALID, BR_BLE_STATUS_BUSY, BR_BLE_STATUS_NOT_READY, BR_BLE_STATUS_STORAGE_ERROR, BR_BLE_STATUS_UNSUPPORTED };
int br_management_init(void);
/* Only authenticated transports may call this. Output capacity >=512. */
size_t br_management_execute(uint8_t opcode, const uint8_t *input, size_t size, uint8_t *response, bool *reboot);
#endif
