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

#ifndef __HAL_KEYSCAN__H__
#define __HAL_KEYSCAN__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

enum {
    KYS_CTRL_SET_INT = 0,
    KYS_CTRL_CLR_INT,
    KYS_CTRL_GET_INT,
    KYS_CTRL_RESUME,
    KYS_CTRL_SUSPEND,
    KYS_CTRL_KEYSCAN_GET_KEYCODE,
    KYS_CTRL_SET_CALLBACK,
};

enum keyscan_index_type {
    KEYSCAN_INDEX,
    KEYSCAN_MAX_INDEX
};

enum col_num_type {
    COL_NUM_1 = 1,
    COL_NUM_2,
    COL_NUM_3,
    COL_NUM_4,
    COL_NUM_5,
    COL_NUM_6,
    COL_NUM_7,
    COL_NUM_8,
    COL_NUM_9,
    COL_NUM_10,
    COL_NUM_11,
    COL_NUM_12,
    COL_NUM_13,
    COL_NUM_14,
    COL_NUM_15,
    COL_NUM_16,
    COL_NUM_17,
    COL_NUM_18,
    COL_NUM_19,
    COL_NUM_20
};

enum row_num_type {
    ROW_NUM_1 = 1,
    ROW_NUM_2,
    ROW_NUM_3,
    ROW_NUM_4,
    ROW_NUM_5,
    ROW_NUM_6,
    ROW_NUM_7,
    ROW_NUM_8,
};

enum keyscan_event_type {
    KEYSCAN_EVENT_TRIG,
    KEYSCAN_EVENT_UNKNOWN
};

typedef struct keyscan_device {
    enum col_num_type col_num;
    enum row_num_type row_num;
    uint8_t deglitch_count;
    void (*callback)(void *dev, void *args, uint32_t size, uint32_t event);
} keyscan_device_t;

#define KEYSCAN_DEV(dev) ((keyscan_device_t *)dev)

int keyscan_register(enum keyscan_index_type index, const char *name);

#ifdef __cplusplus
}
#endif
#endif
