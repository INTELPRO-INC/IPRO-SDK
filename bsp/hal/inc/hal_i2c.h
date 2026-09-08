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

#ifndef __HAL_I2C__H__
#define __HAL_I2C__H__

#ifdef __cplusplus
extern "C"{
#endif

#include "hal_common.h"
#include "board_config.h"

enum i2c_index_type {
#ifdef BSP_USING_I2C0
    I2C0_INDEX,
#endif
#ifdef BSP_USING_I2C1
    I2C1_INDEX,
#endif
    I2C_MAX_INDEX
};

#define I2C_WR      0x0000
#define I2C_RD      0x0001
#define I2C_RW_MASK 0x0001

#define SUB_ADDR_0BYTE 0x0010
#define SUB_ADDR_1BYTE 0x0020
#define SUB_ADDR_2BYTE 0x0040

#define I2C_HW_MODE 0
#define I2C_SW_MODE 1

typedef struct i2c_msg {
    uint8_t slaveaddr;
    uint32_t subaddr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
} i2c_msg_t;

typedef struct i2c_device {
    uint8_t id;
    uint8_t mode;
    uint32_t phase;
} i2c_device_t;

int i2c_init(int i2c_id);
int i2c_transfer(int i2c_id, i2c_msg_t msgs[], uint32_t num);
void i2c_setConfig(i2c_device_t *dev);

#ifdef __cplusplus
}
#endif
#endif