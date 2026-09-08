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

#include <generated/autoconf.h>
#include <stdio.h>
#include "sccb.h"
#include "drv_glb.h"

#include "hal_i2c.h"

#define I2C_INDEX (I2C0_INDEX)

#if !defined(CONFIG_FPGA)
static void I2C_GPIO_Init(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpio_pins[2];
    int i;

    gpio_pins[0] = SCCB_SDA_PIN;
    gpio_pins[1] = SCCB_SCL_PIN;

    cfg.gpioMode=GPIO_MODE_AF;
    cfg.pullType=GPIO_PULL_UP;
    cfg.drive=0;
    cfg.smtCtrl=1;

    if (I2C0_INDEX == I2C_INDEX)
    {
#ifdef CONFIG_IPRO7
        cfg.gpioFun=GPIO_FUN_I2C;
#endif
#ifdef CONFIG_IPRO6
        cfg.gpioFun=GPIO_FUN_I2C0;
#endif
    }
    else
    {
        cfg.gpioFun=GPIO_FUN_I2C1;
    }

    for (i = 0; i < 2; i++) {
        cfg.gpioPin = gpio_pins[i];
        GLB_GPIO_Init(&cfg);
    }
}
#endif

int SCCB_Init(void)
{
#if defined(CONFIG_FPGA)
#else
    I2C_GPIO_Init();
#endif
    i2c_init(I2C_INDEX);
    return 0;
}

int SCCB_Scan(int first, int last)
{
    printf("-------------------scan_i2c_bus...--------------------\r\n");
    int i, j;
    int ret;
    uint8_t tmp;
    uint8_t last_acked = 0;

    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");

    for (i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for(j = 0; j < 16; j++) {
            /* Skip unwanted addresses */
            if (i+j < first || i+j > last) {
                printf("   ");
                continue;
            }

            ret = SCCB_Read(i + j, 0x0, &tmp, 1);
            if (ret == 0) {
                printf("%02x ", i+j);
                last_acked = i + j;
            } else {
                printf("-- ");
            }
        }
        printf("\r\n");
    }

    if (0 == last_acked) {
        return 0;
    }
    return last_acked;
}

int SCCB_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* rdata, uint8_t rdsize)
{
    static i2c_msg_t msg;

    msg.slaveaddr = slave_addr,
    msg.flags = SUB_ADDR_1BYTE | I2C_RD,
    msg.subaddr = reg_addr;
    msg.buf = rdata;
    msg.len = rdsize;

    return i2c_transfer(I2C_INDEX, &msg, 1);
}

int SCCB_Read_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* rdata, uint8_t rdsize)
{
    return 0;
}

int SCCB_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t* data)
{
    static i2c_msg_t msg;

    msg.slaveaddr = slave_addr,
    msg.flags = SUB_ADDR_1BYTE | I2C_WR,
    msg.subaddr = reg_addr;
    msg.buf = data;
    msg.len = 1;

    return i2c_transfer(I2C_INDEX, &msg, 1);
}


int SCCB_Write_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* data)
{
    return 0;
}

