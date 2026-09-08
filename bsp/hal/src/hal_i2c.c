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

#include "hal_i2c.h"
#include "drv_i2c.h"
#include "drv_glb.h"

static i2c_device_t i2c_devices[I2C_MAX_INDEX] = {
    I2C0_CONFIG,
    I2C1_CONFIG,
};

i2c_device_t* i2c_getDevice(int index)
{
    return &i2c_devices[index];
}

void i2c_setConfig(i2c_device_t *dev)
{
    memcpy(&i2c_devices[dev->id], dev, sizeof(i2c_device_t));
}

int i2c_init(int i2c_id)
{
    i2c_device_t *i2c_device = &i2c_devices[i2c_id];

    if (i2c_device->mode == I2C_HW_MODE) {
        I2C_SetPrd(i2c_device->id, i2c_device->phase);
    }

    return 0;
}

int i2c_transfer(int i2c_id, i2c_msg_t msgs[], uint32_t num)
{
    i2c_msg_t *msg;
    I2C_Transfer_Cfg i2cCfg = { 0 };
    i2c_device_t *i2c_device = &i2c_devices[i2c_id];

    if (i2c_device->mode == I2C_HW_MODE) {
        for (uint32_t i = 0; i < num; i++) {
            msg = &msgs[i];
            i2cCfg.slaveAddr = msg->slaveaddr;
            i2cCfg.slaveAddr10Bit = DISABLE;
            i2cCfg.stopEveryByte = DISABLE;
            i2cCfg.subAddr = msg->subaddr;
            i2cCfg.dataSize = msg->len;
            i2cCfg.data = msg->buf;
            i2cCfg.clk = 400000;

            if (msg->flags & SUB_ADDR_0BYTE) {
                i2cCfg.subAddrSize = 0;
            } else if (msg->flags & SUB_ADDR_1BYTE) {
                i2cCfg.subAddrSize = 1;
            } else if (msg->flags & SUB_ADDR_2BYTE) {
                i2cCfg.subAddrSize = 2;
            }

            if ((msg->flags & I2C_RW_MASK) == I2C_WR) {
                return I2C_MasterSendBlocking(i2c_device->id, &i2cCfg);
            } else if ((msg->flags & I2C_RW_MASK) == I2C_RD) {
                return I2C_MasterReceiveBlocking(i2c_device->id, &i2cCfg);
            }
        }
    } else {
    }

    return 0;
}
