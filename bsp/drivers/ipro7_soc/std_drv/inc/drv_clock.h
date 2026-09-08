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

#ifndef __DRV_CLOCK_H__
#define __DRV_CLOCK_H__

#include "cci_reg.h"
#include "glb_reg.h"
#include "drv_hbn.h"
#include "drv_common.h"

typedef enum {
    DRV_SYSTEM_CLOCK_ROOT, /*!< ROOT clock */
    DRV_SYSTEM_CLOCK_FCLK, /*!< Fast clock/CPU clock */
    DRV_SYSTEM_CLOCK_BCLK, /*!< BUS clock */
    DRV_SYSTEM_CLOCK_F32K, /*!< F32K clock */
    DRV_SYSTEM_CLOCK_XCLK, /*!< XCLK:RC32M or XTAL */
    DRV_SYSTEM_CLOCK_XTAL, /*!< XTAL clock */
    DRV_SYSTEM_CLOCK_MAX,  /*!< MAX type of system clock */
} Drv_System_Clock_Type;

typedef enum {
    DRV_PERIPHERAL_CLOCK_UART0,   /*!< UART0 clock */
    DRV_PERIPHERAL_CLOCK_UART1,   /*!< UART1 clock */
    DRV_PERIPHERAL_CLOCK_SPI0,    /*!< SPI0 clock */
    DRV_PERIPHERAL_CLOCK_I2C0,    /*!< I2C0 clock */
    DRV_PERIPHERAL_CLOCK_I2S,     /*!< I2S clock */
    DRV_PERIPHERAL_CLOCK_GPADC,   /*!< GPADC clock */
    DRV_PERIPHERAL_CLOCK_GPDAC,   /*!< GPDAC clock */
    DRV_PERIPHERAL_CLOCK_PWM,     /*!< PWM clock */
    DRV_PERIPHERAL_CLOCK_IR,      /*!< IR clock */
    DRV_PERIPHERAL_CLOCK_FLASH,   /*!< FLASH clock */
    DRV_PERIPHERAL_CLOCK_CAM,     /*!< CAM clock */
    DRV_PERIPHERAL_CLOCK_QDEC,    /*!< QDEC clock */
    DRV_PERIPHERAL_CLOCK_TIMER0,  /*!< TIMER0 clock */
    DRV_PERIPHERAL_CLOCK_TIMER1,  /*!< TIMER1 clock */
    DRV_PERIPHERAL_CLOCK_WDT,     /*!< WDT clock */
    DRV_PERIPHERAL_CLOCK_MAX,
} Drv_Peripheral_Type_Type;

uint32_t Clock_System_Clock_Get(Drv_System_Clock_Type type);
uint32_t Clock_Peripheral_Clock_Get(Drv_Peripheral_Type_Type type);
uint32_t SystemCoreClockGet(void);

#endif /* __DRV_CLOCK_H__ */
