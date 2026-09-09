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


#ifndef __DRV_HBN_H__
#define __DRV_HBN_H__

#include "hbn_reg.h"
#include "drv_aon.h"

/* IPRO6 compatibility stubs for HBN RETRAM naming */
#ifdef IPRO6
#include "ipro6_compat.h"
#endif
#include "drv_l1c.h"
#include "drv_sflash.h"
#include "drv_common.h"

typedef struct
{
    int lowIntEn;  /*!< Low will trigger interrupt */
    int highIntEn; /*!< High will trigger interrupt */
} HBN_PIR_INT_CFG_Type;

typedef enum {
    HBN_PIR_LPF_DIV1, /*!< HBN PIR lpf div 1 */
    HBN_PIR_LPF_DIV2, /*!< HBN PIR lpf div 2 */
} HBN_PIR_LPF_Type;

typedef enum {
    HBN_PIR_HPF_METHOD0, /*!< HBN PIR hpf calc method 0, 1-z^-1 */
    HBN_PIR_HPF_METHOD1, /*!< HBN PIR hpf calc method 1, 1-z^-2 */
    HBN_PIR_HPF_METHOD2, /*!< HBN PIR hpf calc method 2, 1-z^-3 */
} HBN_PIR_HPF_Type;

typedef enum {
    HBN_FLASH_PAD_PULL_NONE, /*!< flash pad pull none when Flash at Deep Power Down Mode */
    HBN_FLASH_PAD_PULL_UP,   /*!< flash pad pull up when Flash at Deep Power Down Mode */
    HBN_FLASH_PAD_PULL_DOWN, /*!< flash pad pull down when Flash at Deep Power Down Mode */
} HBN_FLASH_PAD_PULL_Type;

typedef enum {
    HBN_BOD_THRES_2P05V, /*!< BOD threshold 2.05V */
    HBN_BOD_THRES_2P10V, /*!< BOD threshold 2.10V */
    HBN_BOD_THRES_2P15V, /*!< BOD threshold 2.15V */
    HBN_BOD_THRES_2P20V, /*!< BOD threshold 2.20V */
    HBN_BOD_THRES_2P25V, /*!< BOD threshold 2.25V */
    HBN_BOD_THRES_2P30V, /*!< BOD threshold 2.30V */
    HBN_BOD_THRES_2P35V, /*!< BOD threshold 2.35V */
    HBN_BOD_THRES_2P40V, /*!< BOD threshold 2.40V */
} HBN_BOD_THRES_Type;

typedef enum {
    HBN_BOD_MODE_POR_INDEPENDENT, /*!< POR is independent of BOD */
    HBN_BOD_MODE_POR_RELEVANT,    /*!< POR is relevant to BOD */
} HBN_BOD_MODE_Type;

#define HBN_32K_RC      (0)
#define HBN_32K_XTAL    (1)
#define HBN_32K_DIG     (3)

#define HBN_MCU_XCLK_RC32M      (0)
#define HBN_MCU_XCLK_XTAL       (1)

#define HBN_GPADC_CLK_32M       (0)
#define HBN_GPADC_CLK_F32K      (1)

#define HBN_MCU_ROOT_CLK_XCLK       (0)
#define HBN_MCU_ROOT_CLK_PLL        (1)

#define HBN_UART_CLK_MCU_BCLK       (0)
#define HBN_UART_CLK_MUXPLL_160M    (1)
#define HBN_UART_CLK_XCLK           (2)

#define HBN_RTC_INT_DELAY_32T   (0)
#define HBN_RTC_INT_DELAY_0T    (1)

#define HBN_INT_GPIO16      (0)
#define HBN_INT_GPIO17      (1)
#define HBN_INT_GPIO18      (2)
#define HBN_INT_GPIO19      (3)
#define HBN_INT_RTC         (16)
#define HBN_INT_PIR         (17)
#define HBN_INT_BOD         (18)
#define HBN_INT_ACOMP0      (20)
#define HBN_INT_ACOMP1      (22)

#define HBN_AON_PAD_GPIO16      (0)
#define HBN_AON_PAD_GPIO17      (1)
#define HBN_AON_PAD_GPIO18      (2)
#define HBN_AON_PAD_GPIO19      (3)

typedef struct
{
    uint8_t ctrlEn;   /*!< AON PAD Control by AON HW */
    uint8_t ie;       /*!< Always on PAD IE/SMT (if corresponding  AON GPIO controlled by AON HW) */
    uint8_t oe;       /*!< Always on PAD OE (if corresponding  AON GPIO controlled by AON HW)     */
    uint8_t pullUp;   /*!< Always on PAD PU (if corresponding  AON GPIO controlled by AON HW)     */
    uint8_t pullDown; /*!< Always on PAD PD (if corresponding  AON GPIO controlled by AON HW)     */
} HBN_AON_PAD_CFG_Type;

#define HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE              (0x0)
#define HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE               (0x1)
#define HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL                 (0x2)
#define HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL                (0x3)
#define HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE       (0x4)
#define HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE             (0x8)
#define HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE              (0x9)
#define HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL                (0xA)
#define HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL               (0xB)

typedef enum {
    HBN_OUT0_INT_GPIO16 = 0, /*!< HBN out 0 interrupt type: GPIO16 */
    HBN_OUT0_INT_GPIO17 = 1, /*!< HBN out 0 interrupt type: GPIO17 */
    HBN_OUT0_INT_GPIO18 = 2, /*!< HBN out 0 interrupt type: GPIO18 */
    HBN_OUT0_INT_GPIO19 = 3, /*!< HBN out 0 interrupt type: GPIO19 */
    HBN_OUT0_INT_RTC,        /*!< HBN out 0 interrupt type: RTC */
    HBN_OUT0_INT_MAX,        /*!< MAX */
} HBN_OUT0_INT_Type;

typedef enum {
    HBN_OUT1_INT_PIR,    /*!< HBN out 1 interrupt type: PIR */
    HBN_OUT1_INT_BOD,    /*!< HBN out 1 interrupt type: BOD */
    HBN_OUT1_INT_ACOMP0, /*!< HBN out 1 interrupt type: ACOMP0 */
    HBN_OUT1_INT_ACOMP1, /*!< HBN out 1 interrupt type: ACOMP1 */
    HBN_OUT1_INT_MAX,    /*!< MAX */
} HBN_OUT1_INT_Type;

typedef enum {
    HBN_ACOMP_INT_EDGE_POSEDGE = 1,         /*!< HBN acomp interrupt edge posedge */
    HBN_ACOMP_INT_EDGE_NEGEDGE = 2,         /*!< HBN acomp interrupt edge negedge */
    HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE = 3, /*!< HBN acomp interrupt edge posedge and negedge */
} HBN_ACOMP_INT_EDGE_Type;

#define HBN_LDO_LEVEL_0P70V     (2)
#define HBN_LDO_LEVEL_0P75V     (3)
#define HBN_LDO_LEVEL_0P80V     (4)
#define HBN_LDO_LEVEL_0P85V     (5)
#define HBN_LDO_LEVEL_0P90V     (6)
#define HBN_LDO_LEVEL_0P95V     (7)
#define HBN_LDO_LEVEL_1P00V     (8)
#define HBN_LDO_LEVEL_1P05V     (9)
#define HBN_LDO_LEVEL_1P10V     (10)
#define HBN_LDO_LEVEL_1P15V     (11)
#define HBN_LDO_LEVEL_1P20V     (12)
#define HBN_LDO_LEVEL_1P25V     (13)
#define HBN_LDO_LEVEL_1P30V     (14)
#define HBN_LDO_LEVEL_1P35V     (15)

typedef enum {
    HBN_LEVEL_0, /*!< HBN pd_core */
    HBN_LEVEL_1, /*!< HBN pd_aon_hbncore + pd_core */
} HBN_LEVEL_Type;

typedef struct
{
    uint8_t enableBod;      /*!< Enable BOD or not */
    uint8_t enableBodInt;   /*!< Enable BOD interrupt or not */
    uint8_t bodThreshold;   /*!< BOD threshold */
    uint8_t enablePorInBod; /*!< Enable POR when BOD occure or not */
} HBN_BOD_CFG_Type;

/* IPRO7 compatibility: BOR (Brown-Out Reset) aliases for BOD (Brown-Out Detect) */
typedef struct
{
    uint8_t enableBor;      /*!< Enable BOR or not (maps to enableBod) */
    uint8_t enableBorInt;   /*!< Enable BOR interrupt or not (maps to enableBodInt) */
    uint8_t borThreshold;   /*!< BOR threshold (maps to bodThreshold) */
    uint8_t enablePorInBor; /*!< Enable POR when BOR occure or not (maps to enablePorInBod) */
} HBN_BOR_CFG_Type;

#define HBN_Set_BOR_Cfg(cfg) HBN_Set_BOD_Cfg((HBN_BOD_CFG_Type*)(cfg))

typedef struct
{
    uint8_t useXtal32k;                     /*!< Whether use xtal 32K as 32K clock source,otherwise use rc32k */
    uint32_t sleepTime;                     /*!< HBN sleep time */
    uint8_t gpioWakeupSrc;                  /*!< GPIO Wakeup source */
    uint8_t gpioTrigType; /*!< GPIO Triger type */
    SPI_Flash_Cfg_Type *flashCfg;           /*!< Flash config pointer, used when power down flash */
    HBN_LEVEL_Type hbnLevel;                /*!< HBN level */
    uint8_t ldoLevel;            /*!< LDO level */
    uint8_t dcdcPuSeq;                      /*!< power on dcdc sequence */
} HBN_APP_CFG_Type;

#define IS_HBN_PIR_LPF_TYPE(type) (((type) == HBN_PIR_LPF_DIV1) || \
                                   ((type) == HBN_PIR_LPF_DIV2))

#define IS_HBN_PIR_HPF_TYPE(type) (((type) == HBN_PIR_HPF_METHOD0) || \
                                   ((type) == HBN_PIR_HPF_METHOD1) || \
                                   ((type) == HBN_PIR_HPF_METHOD2))

#define IS_HBN_FLASH_PAD_PULL_TYPE(type) (((type) == HBN_FLASH_PAD_PULL_NONE) || \
                                          ((type) == HBN_FLASH_PAD_PULL_UP) ||   \
                                          ((type) == HBN_FLASH_PAD_PULL_DOWN))

#define IS_HBN_BOD_THRES_TYPE(type) (((type) == HBN_BOD_THRES_2P05V) || \
                                     ((type) == HBN_BOD_THRES_2P10V) || \
                                     ((type) == HBN_BOD_THRES_2P15V) || \
                                     ((type) == HBN_BOD_THRES_2P20V) || \
                                     ((type) == HBN_BOD_THRES_2P25V) || \
                                     ((type) == HBN_BOD_THRES_2P30V) || \
                                     ((type) == HBN_BOD_THRES_2P35V) || \
                                     ((type) == HBN_BOD_THRES_2P40V))

#define IS_HBN_BOD_MODE_TYPE(type) (((type) == HBN_BOD_MODE_POR_INDEPENDENT) || \
                                    ((type) == HBN_BOD_MODE_POR_RELEVANT))

#define IS_HBN_32K_CLK_TYPE(type) (((type) == HBN_32K_RC) ||   \
                                   ((type) == HBN_32K_XTAL) || \
                                   ((type) == HBN_32K_DIG))

#define IS_HBN_MCU_XCLK_TYPE(type) (((type) == HBN_MCU_XCLK_RC32M) || \
                                    ((type) == HBN_MCU_XCLK_XTAL))

#define IS_HBN_GPADC_CLK_TYPE(type) (((type) == HBN_GPADC_CLK_32M) || \
                                     ((type) == HBN_GPADC_CLK_F32K))

#define IS_HBN_MCU_ROOT_CLK_TYPE(type) (((type) == HBN_MCU_ROOT_CLK_XCLK) || \
                                        ((type) == HBN_MCU_ROOT_CLK_PLL))

#define IS_HBN_UART_CLK_TYPE(type) (((type) == HBN_UART_CLK_MCU_BCLK) ||    \
                                    ((type) == HBN_UART_CLK_MUXPLL_160M) || \
                                    ((type) == HBN_UART_CLK_XCLK))

#define IS_HBN_RTC_INT_DELAY_TYPE(type) (((type) == HBN_RTC_INT_DELAY_32T) || \
                                         ((type) == HBN_RTC_INT_DELAY_0T))

#define IS_HBN_INT_TYPE(type) (((type) == HBN_INT_GPIO9) ||  \
                               ((type) == HBN_INT_GPIO10) || \
                               ((type) == HBN_INT_GPIO11) || \
                               ((type) == HBN_INT_GPIO12) || \
                               ((type) == HBN_INT_GPIO13) || \
                               ((type) == HBN_INT_GPIO14) || \
                               ((type) == HBN_INT_GPIO15) || \
                               ((type) == HBN_INT_GPIO40) || \
                               ((type) == HBN_INT_GPIO41) || \
                               ((type) == HBN_INT_RTC) ||    \
                               ((type) == HBN_INT_PIR) ||    \
                               ((type) == HBN_INT_BOD) ||    \
                               ((type) == HBN_INT_ACOMP0) || \
                               ((type) == HBN_INT_ACOMP1))

#define IS_HBN_AON_PAD_TYPE(type) (((type) == HBN_AON_PAD_GPIO16) || \
                                   ((type) == HBN_AON_PAD_GPIO17) || \
                                   ((type) == HBN_AON_PAD_GPIO18) || \
                                   ((type) == HBN_AON_PAD_GPIO19))

#define IS_HBN_ACOMP_INT_EDGE_TYPE(type) (((type) == HBN_ACOMP_INT_EDGE_POSEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_NEGEDGE) || \
                                          ((type) == HBN_ACOMP_INT_EDGE_POSEDGE_NEGEDGE))

#define IS_HBN_GPIO_INT_TRIGGER_TYPE(type) (((type) == HBN_GPIO_INT_TRIGGER_SYNC_FALLING_EDGE) ||        \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_RISING_EDGE) ||         \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_LOW_LEVEL) ||           \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_HIGH_LEVEL) ||          \
                                            ((type) == HBN_GPIO_INT_TRIGGER_SYNC_RISING_FALLING_EDGE) || \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_FALLING_EDGE) ||       \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_RISING_EDGE) ||        \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_LOW_LEVEL) ||          \
                                            ((type) == HBN_GPIO_INT_TRIGGER_ASYNC_HIGH_LEVEL))

#define IS_HBN_OUT0_INT_TYPE(type) (((type) == HBN_OUT0_INT_GPIO9) ||  \
                                    ((type) == HBN_OUT0_INT_GPIO10) || \
                                    ((type) == HBN_OUT0_INT_GPIO11) || \
                                    ((type) == HBN_OUT0_INT_GPIO12) || \
                                    ((type) == HBN_OUT0_INT_GPIO13) || \
                                    ((type) == HBN_OUT0_INT_GPIO14) || \
                                    ((type) == HBN_OUT0_INT_GPIO15) || \
                                    ((type) == HBN_OUT0_INT_GPIO40) || \
                                    ((type) == HBN_OUT0_INT_GPIO41) || \
                                    ((type) == HBN_OUT0_INT_RTC) ||    \
                                    ((type) == HBN_OUT0_INT_MAX))

#define IS_HBN_OUT1_INT_TYPE(type) (((type) == HBN_OUT1_INT_PIR) ||    \
                                    ((type) == HBN_OUT1_INT_BOD) ||    \
                                    ((type) == HBN_OUT1_INT_ACOMP0) || \
                                    ((type) == HBN_OUT1_INT_ACOMP1) || \
                                    ((type) == HBN_OUT1_INT_MAX))

#define IS_HBN_LDO_LEVEL_TYPE(type) (((type) == HBN_LDO_LEVEL_0P70V) || \
                                     ((type) == HBN_LDO_LEVEL_0P75V) || \
                                     ((type) == HBN_LDO_LEVEL_0P80V) || \
                                     ((type) == HBN_LDO_LEVEL_0P85V) || \
                                     ((type) == HBN_LDO_LEVEL_0P90V) || \
                                     ((type) == HBN_LDO_LEVEL_0P95V) || \
                                     ((type) == HBN_LDO_LEVEL_1P00V) || \
                                     ((type) == HBN_LDO_LEVEL_1P05V) || \
                                     ((type) == HBN_LDO_LEVEL_1P10V) || \
                                     ((type) == HBN_LDO_LEVEL_1P15V) || \
                                     ((type) == HBN_LDO_LEVEL_1P20V) || \
                                     ((type) == HBN_LDO_LEVEL_1P25V) || \
                                     ((type) == HBN_LDO_LEVEL_1P30V) || \
                                     ((type) == HBN_LDO_LEVEL_1P35V))

#define IS_HBN_LEVEL_TYPE(type) (((type) == HBN_LEVEL_0) || \
                                 ((type) == HBN_LEVEL_1))

#define HBN_RAM_SIZE               (4 * 1024)
#define HBN_RTC_COMP_BIT0_39       0x01
#define HBN_RTC_COMP_BIT0_23       0x02
#define HBN_RTC_COMP_BIT13_39      0x04
#define HBN_STATUS_ENTER_FLAG      0x4e424845
#define HBN_STATUS_WAKEUP_FLAG     0x4e424857
#define APP_JUMP_ENTER_FLAG        0x50504145
#define APP_JUMP_DONE_FLAG         0x50504157
#define HBN_RELEASE_CORE_FLAG      (0x4)
#define HBN_LDO18IO_POWER_DLY_FLAG (0x52)
#define HBN_XTAL_FLAG_VALUE        (0x8)
#define HBN_FLASH_POWER_DLY_FLAG   (0x6)
#define HBN_RESET_REASON_FLAG      (0x9)
#define HBN_GPIO_KEEP_FLAG         (0x2)
#define PDS_GPIO_KEEP_FLAG         (0x2)

/* 0x108 : HBN_RSV2 */
#define HBN_LDO18IO_POWER_ON_DLY       HBN_LDO18IO_POWER_ON_DLY
#define HBN_LDO18IO_POWER_ON_DLY_POS   (0U)
#define HBN_LDO18IO_POWER_ON_DLY_LEN   (11U)
#define HBN_LDO18IO_POWER_ON_DLY_MSK   (((1U << HBN_LDO18IO_POWER_ON_DLY_LEN) - 1) << HBN_LDO18IO_POWER_ON_DLY_POS)
#define HBN_LDO18IO_POWER_ON_DLY_UMSK  (~(((1U << HBN_LDO18IO_POWER_ON_DLY_LEN) - 1) << HBN_LDO18IO_POWER_ON_DLY_POS))
#define HBN_LDO18IO_POWER_OFF_DLY      HBN_LDO18IO_POWER_OFF_DLY
#define HBN_LDO18IO_POWER_OFF_DLY_POS  (11U)
#define HBN_LDO18IO_POWER_OFF_DLY_LEN  (5U)
#define HBN_LDO18IO_POWER_OFF_DLY_MSK  (((1U << HBN_LDO18IO_POWER_OFF_DLY_LEN) - 1) << HBN_LDO18IO_POWER_OFF_DLY_POS)
#define HBN_LDO18IO_POWER_OFF_DLY_UMSK (~(((1U << HBN_LDO18IO_POWER_OFF_DLY_LEN) - 1) << HBN_LDO18IO_POWER_OFF_DLY_POS))
#define HBN_LDO18IO_POWER_DLY_STS      HBN_LDO18IO_POWER_DLY_STS
#define HBN_LDO18IO_POWER_DLY_STS_POS  (16U)
#define HBN_LDO18IO_POWER_DLY_STS_LEN  (8U)
#define HBN_LDO18IO_POWER_DLY_STS_MSK  (((1U << HBN_LDO18IO_POWER_DLY_STS_LEN) - 1) << HBN_LDO18IO_POWER_DLY_STS_POS)
#define HBN_LDO18IO_POWER_DLY_STS_UMSK (~(((1U << HBN_LDO18IO_POWER_DLY_STS_LEN) - 1) << HBN_LDO18IO_POWER_DLY_STS_POS))
#define HBN_CORE_UNHALT                HBN_CORE_UNHALT
#define HBN_CORE_UNHALT_POS            (25U)
#define HBN_CORE_UNHALT_LEN            (1U)
#define HBN_CORE_UNHALT_MSK            (((1U << HBN_CORE_UNHALT_LEN) - 1) << HBN_CORE_UNHALT_POS)
#define HBN_CORE_UNHALT_UMSK           (~(((1U << HBN_CORE_UNHALT_LEN) - 1) << HBN_CORE_UNHALT_POS))
#define HBN_USER_BOOT_SEL              HBN_USER_BOOT_SEL
#define HBN_USER_BOOT_SEL_POS          (26U)
#define HBN_USER_BOOT_SEL_LEN          (2U)
#define HBN_USER_BOOT_SEL_MSK          (((1U << HBN_USER_BOOT_SEL_LEN) - 1) << HBN_USER_BOOT_SEL_POS)
#define HBN_USER_BOOT_SEL_UMSK         (~(((1U << HBN_USER_BOOT_SEL_LEN) - 1) << HBN_USER_BOOT_SEL_POS))
#define HBN_RELEASE_CORE               HBN_RELEASE_CORE
#define HBN_RELEASE_CORE_POS           (28U)
#define HBN_RELEASE_CORE_LEN           (4U)
#define HBN_RELEASE_CORE_MSK           (((1U << HBN_RELEASE_CORE_LEN) - 1) << HBN_RELEASE_CORE_POS)
#define HBN_RELEASE_CORE_UMSK          (~(((1U << HBN_RELEASE_CORE_LEN) - 1) << HBN_RELEASE_CORE_POS))

/* 0x108 : HBN_RSV3 */
#define HBN_XTAL_TYPE            HBN_XTAL_TYPE
#define HBN_XTAL_TYPE_POS        (0U)
#define HBN_XTAL_TYPE_LEN        (4U)
#define HBN_XTAL_TYPE_MSK        (((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS)
#define HBN_XTAL_TYPE_UMSK       (~(((1U << HBN_XTAL_TYPE_LEN) - 1) << HBN_XTAL_TYPE_POS))
#define HBN_XTAL_STS             HBN_XTAL_STS
#define HBN_XTAL_STS_POS         (4U)
#define HBN_XTAL_STS_LEN         (4U)
#define HBN_XTAL_STS_MSK         (((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS)
#define HBN_XTAL_STS_UMSK        (~(((1U << HBN_XTAL_STS_LEN) - 1) << HBN_XTAL_STS_POS))
#define HBN_FLASH_POWER_DLY      HBN_FLASH_POWER_DLY
#define HBN_FLASH_POWER_DLY_POS  (8U)
#define HBN_FLASH_POWER_DLY_LEN  (8U)
#define HBN_FLASH_POWER_DLY_MSK  (((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS)
#define HBN_FLASH_POWER_DLY_UMSK (~(((1U << HBN_FLASH_POWER_DLY_LEN) - 1) << HBN_FLASH_POWER_DLY_POS))
#define HBN_FLASH_POWER_STS      HBN_FLASH_POWER_STS
#define HBN_FLASH_POWER_STS_POS  (16U)
#define HBN_FLASH_POWER_STS_LEN  (4U)
#define HBN_FLASH_POWER_STS_MSK  (((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS)
#define HBN_FLASH_POWER_STS_UMSK (~(((1U << HBN_FLASH_POWER_STS_LEN) - 1) << HBN_FLASH_POWER_STS_POS))
#define PDS_GPIO_KEEP_PIN        PDS_GPIO_KEEP_PIN
#define PDS_GPIO_KEEP_PIN_POS    (20U)
#define PDS_GPIO_KEEP_PIN_LEN    (4U)
#define PDS_GPIO_KEEP_PIN_MSK    (((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS)
#define PDS_GPIO_KEEP_PIN_UMSK   (~(((1U << PDS_GPIO_KEEP_PIN_LEN) - 1) << PDS_GPIO_KEEP_PIN_POS))
#define HBN_GPIO_KEEP_PIN        HBN_GPIO_KEEP_PIN
#define HBN_GPIO_KEEP_PIN_POS    (24U)
#define HBN_GPIO_KEEP_PIN_LEN    (4U)
#define HBN_GPIO_KEEP_PIN_MSK    (((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS)
#define HBN_GPIO_KEEP_PIN_UMSK   (~(((1U << HBN_GPIO_KEEP_PIN_LEN) - 1) << HBN_GPIO_KEEP_PIN_POS))
#define PDS_GPIO_KEEP_STS        PDS_GPIO_KEEP_STS
#define PDS_GPIO_KEEP_STS_POS    (28U)
#define PDS_GPIO_KEEP_STS_LEN    (2U)
#define PDS_GPIO_KEEP_STS_MSK    (((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS)
#define PDS_GPIO_KEEP_STS_UMSK   (~(((1U << PDS_GPIO_KEEP_STS_LEN) - 1) << PDS_GPIO_KEEP_STS_POS))
#define HBN_GPIO_KEEP_STS        HBN_GPIO_KEEP_STS
#define HBN_GPIO_KEEP_STS_POS    (30U)
#define HBN_GPIO_KEEP_STS_LEN    (2U)
#define HBN_GPIO_KEEP_STS_MSK    (((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS)
#define HBN_GPIO_KEEP_STS_UMSK   (~(((1U << HBN_GPIO_KEEP_STS_LEN) - 1) << HBN_GPIO_KEEP_STS_POS))

/*----------*/
#ifndef IPRO_USE_HAL_DRIVER
void HBN_OUT0_IRQHandler(void);
void HBN_OUT1_IRQHandler(void);
#endif
/*----------*/
void HBN_Mode_Enter(HBN_APP_CFG_Type *cfg);
void HBN_Power_Down_Flash(SPI_Flash_Cfg_Type *flashCfg);
void HBN_Enable(uint32_t aGPIOIeCfg, uint8_t ldoLevel, HBN_LEVEL_Type hbnLevel, uint8_t dcdcPuSeq);
int HBN_Reset(void);
/*----------*/
int HBN_Set_GPADC_CLK_Sel(uint8_t clkSel);
int HBN_PIR_Enable(void);
int HBN_PIR_Disable(void);
int HBN_PIR_INT_Config(HBN_PIR_INT_CFG_Type *pirIntCfg);
int HBN_PIR_LPF_Sel(HBN_PIR_LPF_Type lpf);
int HBN_PIR_HPF_Sel(HBN_PIR_HPF_Type hpf);
int HBN_Set_PIR_Threshold(uint16_t threshold);
uint16_t HBN_Get_PIR_Threshold(void);
int HBN_Set_PIR_Interval(uint16_t interval);
uint16_t HBN_Get_PIR_Interval(void);
/*----------*/
int HBN_Get_BOD_OUT_State(void);
int HBN_Set_BOD_Config(uint8_t enable, HBN_BOD_THRES_Type threshold, HBN_BOD_MODE_Type mode);
/*----------*/
int HBN_Set_Ldo11_Aon_Vout(uint8_t ldoLevel);
int HBN_Set_Ldo11_Rt_Vout(uint8_t ldoLevel);
int HBN_Set_Ldo11_Soc_Vout(uint8_t ldoLevel);
int HBN_Set_Ldo11_All_Vout(uint8_t ldoLevel);
/*----------*/
int HBN_32K_Sel(uint8_t clkType);
int HBN_Set_UART_CLK_Sel(uint8_t clkSel);
uint8_t HBN_Get_MCU_XCLK_Sel(void);
int HBN_Set_MCU_XCLK_Sel(uint8_t xclk);
uint8_t HBN_Get_MCU_Root_CLK_Sel(void);
int HBN_Set_MCU_Root_CLK_Sel(uint8_t rootClk);
/*----------*/
int HBN_Set_HRAM_slp(void);
int HBN_Set_HRAM_Ret(void);
/*----------*/
uint32_t HBN_Get_Status_Flag(void);
int HBN_Set_Status_Flag(uint32_t flag);
uint32_t HBN_Get_Wakeup_Addr(void);
int HBN_Set_Wakeup_Addr(uint32_t addr);
/*----------*/
uint8_t HBN_Get_Core_Unhalt_Config(void);
int HBN_Set_Core_Reboot_Config(uint8_t core, uint8_t hcfg);
uint8_t HBN_Get_User_Boot_Config(void);
int HBN_Set_User_Boot_Config(uint8_t ubCfg);
int HBN_Get_Ldo18io_Power_Delay_Config(uint16_t *pwrOffDly, uint16_t *pwrOnDly);
int HBN_Set_Ldo18io_Power_Delay_Config(uint16_t pwrOffDly, uint16_t pwrOnDly);
/*----------*/
int HBN_Set_Xtal_Type(uint8_t xtalType);
int HBN_Get_Xtal_Type(uint8_t *xtalType);
int HBN_Get_Xtal_Value(uint32_t *xtalVal);
#if 0
int HBN_Set_Flash_Power_Delay(uint8_t flashPwrDly);
#endif
int HBN_Get_Flash_Power_Delay(uint8_t *flashPwrDly);
int HBN_Set_Reset_Reason(uint16_t rstReason);
int HBN_Get_Reset_Reason(uint16_t *rstReason);
int HBN_Set_Gpio_Keep(uint8_t gpioKeep);
int HBN_Get_Gpio_Keep(uint8_t *gpioKeep);
int HBN_Clear_Gpio_Keep(uint8_t gpioKeep);
int HBN_Set_PDS_Gpio_Keep(uint8_t gpioKeep);
int HBN_Get_PDS_Gpio_Keep(uint8_t *gpioKeep);
int HBN_Clear_PDS_Gpio_Keep(uint8_t gpioKeep);

/*----------*/
int HBN_Clear_RTC_Counter(void);
int HBN_Enable_RTC_Counter(void);
int HBN_Set_RTC_Timer(uint8_t delay, uint32_t compValLow, uint32_t compValHigh, uint8_t compMode);
int HBN_Get_RTC_Timer_Val(uint32_t *valLow, uint32_t *valHigh);
int HBN_Recal_RC32K(int32_t expected_counter, int32_t actual_counter);
int HBN_Clear_RTC_IRQ(void);
/*----------*/
int HBN_GPIO_INT_Enable(uint8_t gpioIntTrigType);
int HBN_GPIO_INT_Disable(void);
int HBN_Get_INT_State(uint8_t irqType);
uint8_t HBN_Get_Pin_Wakeup_Mode(void);
int HBN_Clear_IRQ(uint8_t irqType);
int HBN_Hw_Pu_Pd_Cfg(uint8_t enable);
int HBN_Pin_WakeUp_Mask(uint8_t maskVal);
int HBN_Aon_Pad_Ctrl(uint32_t aonPadCtl1, uint32_t aonPadCtl2);
int HBN_Aon_Pad_Cfg(uint8_t aonPadHwCtrlEn, uint8_t aonGpio, HBN_AON_PAD_CFG_Type *aonPadCfg);
int HBN_Aon_Pad_Cfg_Set(uint8_t aonPadHwCtrlEn, uint8_t aonGpio);
/*----------*/
int HBN_Enable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge);
int HBN_Disable_AComp_IRQ(uint8_t acompId, HBN_ACOMP_INT_EDGE_Type edge);
/*----------*/
int HBN_Enable_BOD_IRQ(void);
int HBN_Disable_BOD_IRQ(void);
/*----------*/
int HBN_Out0_Callback_Install(HBN_OUT0_INT_Type intType, intCallback_Type *cbFun);
int HBN_Out1_Callback_Install(HBN_OUT1_INT_Type intType, intCallback_Type *cbFun);
/*----------*/
int HBN_Aon_Pad_WakeUpCfg(int puPdEn, uint8_t trigMode, uint32_t maskVal, int dlyEn, uint8_t dlySec);
/*----------*/
int HBN_Power_On_Xtal_32K(void);
int HBN_Power_Off_Xtal_32K(void);
int HBN_Keep_On_RC32K(void);
int HBN_Power_Off_RC32K(void);
int HBN_Trim_Ldo33VoutTrim(void);
int HBN_Trim_RC32K(void);
int HBN_Set_BOD_Cfg(HBN_BOD_CFG_Type *cfg);
int HBN_Clear_RTC_INT(void);
/*----------*/
void HBN_Get_Reset_Event(uint8_t* event);
void HBN_Clr_Reset_Event(void);
int HBN_Disable_AComp0_IRQ(void);
int HBN_Disable_AComp1_IRQ(void);
int HBN_Enable_AComp0_IRQ(void);
int HBN_Enable_AComp1_IRQ(void);
/*----------*/

#endif /* __DRV_HBN_H__ */
