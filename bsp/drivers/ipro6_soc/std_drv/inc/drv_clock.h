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

#include "glb_reg.h"
#include "mcu_misc_reg.h"
#include "pds_reg.h"
#include "drv_glb.h"
#include "drv_hbn.h"
#include "drv_common.h"

/* IPRO6 compatibility stubs for unsupported peripherals */
#ifdef IPRO6
#include "ipro6_compat.h"
#endif

typedef enum {
    DRV_SYSTEM_CLOCK_MCU_ROOT_CLK, /*!< MCU root clock */
    DRV_SYSTEM_CLOCK_MCU_CLK,      /*!< MCU Fast clock/CPU clock */
    DRV_SYSTEM_CLOCK_MCU_BCLK,     /*!< MCU BUS clock */
    DRV_SYSTEM_CLOCK_MCU_PBCLK,    /*!< MCU peri BUS clock */
    DRV_SYSTEM_CLOCK_F32K,         /*!< F32K clock */
    DRV_SYSTEM_CLOCK_XCLK,         /*!< XCLK:RC32M or XTAL */
    DRV_SYSTEM_CLOCK_XTAL,         /*!< XTAL clock */
    DRV_SYSTEM_CLOCK_MAX,          /*!< MAX type of system clock */
} DRV_SYSTEM_CLOCK_Type;


typedef struct
{
    uint32_t magic;                              /*!< Clock config magic */
    uint32_t systemClock[DRV_SYSTEM_CLOCK_MAX];   /*!< System lock value */
    uint32_t peripheralClock[IPRO_AHB_SLAVE1_MAX]; /*!< Pewripherals clock value */
    uint32_t i2sClock;                           /*!< I2S clock */
} Clock_Cfg_Type;

typedef enum {
    CLOCK_AUPLL_DIV1,
    CLOCK_AUPLL_DIV2,
    CLOCK_AUPLL_DIV2P5,
    CLOCK_AUPLL_DIV3,
    CLOCK_AUPLL_DIV4,
    CLOCK_AUPLL_DIV5,
    CLOCK_AUPLL_DIV6,
    CLOCK_AUPLL_DIV10,
    CLOCK_AUPLL_DIV15,
} CLOCK_AUPLL_Type;

typedef enum {
    DRV_PERIPHERAL_CLOCK_UART0,       /*!< UART0 clock */
    DRV_PERIPHERAL_CLOCK_UART1,       /*!< UART1 clock */
    DRV_PERIPHERAL_CLOCK_UART2,       /*!< UART2 clock */
    DRV_PERIPHERAL_CLOCK_SPI,         /*!< SPI clock */
    DRV_PERIPHERAL_CLOCK_DBI,         /*!< DBI clock */
    DRV_PERIPHERAL_CLOCK_EMI,         /*!< EMI clock */
    DRV_PERIPHERAL_CLOCK_ISP,         /*!< ISP clock */
    DRV_PERIPHERAL_CLOCK_I2C0,        /*!< I2C0 clock */
    DRV_PERIPHERAL_CLOCK_I2C1,        /*!< I2C1 clock */
    DRV_PERIPHERAL_CLOCK_PSRAMB,      /*!< PSRAMB clock */
    DRV_PERIPHERAL_CLOCK_FLASH,       /*!< FLASH clock */
    DRV_PERIPHERAL_CLOCK_I2S,         /*!< I2S clock */
    DRV_PERIPHERAL_CLOCK_IR,          /*!< IR clock */
    DRV_PERIPHERAL_CLOCK_ADC,         /*!< ADC clock */
    DRV_PERIPHERAL_CLOCK_GPADC,       /*!< GPADC clock */
    DRV_PERIPHERAL_CLOCK_GPDAC,       /*!< GPDAC clock */
    DRV_PERIPHERAL_CLOCK_CAM,         /*!< CAM clock */
    DRV_PERIPHERAL_CLOCK_PKA,         /*!< PKA clock */
    DRV_PERIPHERAL_CLOCK_MAX,
    DRV_PERIPHERAL_CLOCK_TIMER0,
    DRV_PERIPHERAL_CLOCK_TIMER1,
} DRV_PERIPHERAL_Type;

#define IS_DRV_SYSTEM_CLOCK_TYPE(type) (((type) == DRV_SYSTEM_CLOCK_MCU_ROOT_CLK) || \
                                       ((type) == DRV_SYSTEM_CLOCK_MCU_CLK) ||      \
                                       ((type) == DRV_SYSTEM_CLOCK_MCU_BCLK) ||     \
                                       ((type) == DRV_SYSTEM_CLOCK_MCU_PBCLK) ||    \
                                       ((type) == DRV_SYSTEM_CLOCK_DSP_ROOT_CLK) || \
                                       ((type) == DRV_SYSTEM_CLOCK_DSP_CLK) ||      \
                                       ((type) == DRV_SYSTEM_CLOCK_DSP_BCLK) ||     \
                                       ((type) == DRV_SYSTEM_CLOCK_DSP_PBCLK) ||    \
                                       ((type) == DRV_SYSTEM_CLOCK_LP_CLK) ||       \
                                       ((type) == DRV_SYSTEM_CLOCK_F32K) ||         \
                                       ((type) == DRV_SYSTEM_CLOCK_XCLK) ||         \
                                       ((type) == DRV_SYSTEM_CLOCK_XTAL) ||         \
                                       ((type) == DRV_SYSTEM_CLOCK_MAX))

#define SYS_CLOCK_CFG_MAGIC (0x12345678)
#define SYS_CLOCK_CFG_ADDR  (0x20010000 + 4 * 1024 - 512)

/*----------*/
uint32_t Clock_System_Clock_Get(DRV_SYSTEM_CLOCK_Type type);
uint32_t Clock_Peripheral_Clock_Get(DRV_PERIPHERAL_Type type);
uint32_t Clock_Peripheral_Clock_Get_Uart(void);
uint32_t Clock_Peripheral_Clock_Get_Flash(void);
uint32_t Clock_Audio_ADC_Clock_Get(void);
uint32_t Clock_Audio_DAC_Clock_Get(void);
/*----------*/
uint32_t SystemCoreClockGet(void);
/*----------*/
void CPU_Set_MTimer_Toggle_Cnt(uint16_t cnt);
uint16_t CPU_Get_MTimer_Toggle_Cnt(void);
uint32_t CPU_Get_MTimer_Clock(void);
uint64_t CPU_Get_MTimer_Counter(void);
uint64_t CPU_Get_CPU_Cycle(void);
uint64_t CPU_Get_MTimer_US(void);
uint64_t CPU_Get_MTimer_MS(void);
int CPU_MTimer_Delay_US(uint32_t cnt);
int CPU_MTimer_Delay_MS(uint32_t cnt);
uint32_t Clock_Get_EMI_Clk(void);
uint32_t Clock_Get_ISP_Clk(void);
uint32_t Clock_Get_BLAI_Clk(void);
uint32_t Clock_Get_Display_Clk(void);
uint32_t Clock_Get_PSRAMB_Clk(void);
uint32_t Clock_Get_PSRAMB_Clk(void);
uint32_t Clock_Get_H264_Clk(void);
/*----------*/

#endif /* __DRV_CLOCK_H__ */
