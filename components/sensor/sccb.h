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

#ifndef __SCCB_H__
#define __SCCB_H__

#include <generated/autoconf.h>
#include "stdint.h"

/*
 * SCCB (I2C) pin definitions — board-variant selection.
 *
 * EVB: SDA=GPIO36, SCL=GPIO37
 * DVK: SDA=GPIO34, SCL=GPIO35
 */
#if defined(CONFIG_EVB)

#ifdef CONFIG_IPRO7
#define SCCB_SDA_PIN    GLB_GPIO_PIN_36
#define SCCB_SCL_PIN    GLB_GPIO_PIN_37
#endif

#ifdef CONFIG_IPRO6
#define SCCB_SDA_PIN    GLB_GPIO_PIN_19
#define SCCB_SCL_PIN    GLB_GPIO_PIN_18
#endif

#elif defined(CONFIG_DVK)

#define SCCB_SDA_PIN    GLB_GPIO_PIN_34
#define SCCB_SCL_PIN    GLB_GPIO_PIN_35

#elif defined(CONFIG_CLMAX_7AI_EVB)

#define SCCB_SDA_PIN    GLB_GPIO_PIN_34
#define SCCB_SCL_PIN    GLB_GPIO_PIN_35

#elif defined(CONFIG_IPRO7AI_EVK)

/* IPRO7AI EVK: camera SCCB on GPIO36/37. On IPRO7 the I2C0 role is fixed by
 * pin parity - EVEN = SCL, ODD = SDA - so GPIO36 is SCL and GPIO37 is SDA,
 * which is how the board is wired. (The EVB block above has these two labels
 * the other way round; it makes no difference because SCCB_Init() only uses
 * them to build a 2-entry list and muxes both to GPIO_FUN_I2C.) */
#define SCCB_SDA_PIN    GLB_GPIO_PIN_37
#define SCCB_SCL_PIN    GLB_GPIO_PIN_36

#else
#define SCCB_SDA_PIN    GLB_GPIO_PIN_36
#define SCCB_SCL_PIN    GLB_GPIO_PIN_37
#endif

typedef struct {
    uint16_t    addr;
    uint8_t     data;
} REG_16BIT_Type;

int SCCB_Init(void);
int SCCB_Scan(int first, int last);
int SCCB_Read(uint8_t slave_addr, uint8_t reg_addr, uint8_t* rdata, uint8_t rdsize);
int SCCB_Read_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* rdata, uint8_t rdsize);
int SCCB_Write(uint8_t slave_addr, uint8_t reg_addr, uint8_t* data);
int SCCB_Write_Reg16(uint8_t slave_addr, uint16_t reg_addr, uint8_t* data);

#endif
