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


#ifndef __DRV_GLB_GPIO_H__
#define __DRV_GLB_GPIO_H__

#include "glb_reg.h"
#include "pds_reg.h"
#include "drv_gpio.h"
#include "drv_hbn.h"
#include "drv_sf_ctrl.h"
// #include "drv_sf_cfg.h"
#include "drv_aon.h"
#include "drv_ef_ctrl.h"
#include "drv_pds.h"
#include "drv_common.h"

#define GLB_GPIO_INT_TRIG_SYNC_FALLING_EDGE                          (0)
#define GLB_GPIO_INT_TRIG_SYNC_RISING_EDGE                           (1)
#define GLB_GPIO_INT_TRIG_SYNC_LOW_LEVEL                             (2)
#define GLB_GPIO_INT_TRIG_SYNC_HIGH_LEVEL                            (3)
#define GLB_GPIO_INT_TRIG_SYNC_FALLING_RISING_EDGE                   (4)
#define GLB_GPIO_INT_TRIG_ASYNC_FALLING_EDGE                         (8)
#define GLB_GPIO_INT_TRIG_ASYNC_RISING_EDGE                          (9)
#define GLB_GPIO_INT_TRIG_ASYNC_LOW_LEVEL                            (10)
#define GLB_GPIO_INT_TRIG_ASYNC_HIGH_LEVEL                           (11)

#define GLB_GPIO_FIFO_INT_FER   (0)
#define GLB_GPIO_FIFO_INT_FIFO  (1)
#define GLB_GPIO_FIFO_INT_END   (2)
#define GLB_GPIO_FIFO_INT_ALL   (3)

#define GPIO_FIFO_PHASE_FIRST_HIGH  (0)
#define GPIO_FIFO_PHASE_FIRST_LOW   (1)

#define GPIO_FIFO_IDLE_LOW      (0)
#define GPIO_FIFO_IDLE_HIGH     (1)

#define GPIO_FIFO_LATCH_WRITE       (0)
#define GPIO_FIFO_LATCH_SETCLEAR    (1)

typedef enum {
    GLB_GPIO_INT_TRIG_NEG_PULSE, /*!< GPIO negedge pulse trigger interrupt */
    GLB_GPIO_INT_TRIG_POS_PULSE, /*!< GPIO posedge pulse trigger interrupt */
    GLB_GPIO_INT_TRIG_NEG_LEVEL, /*!< GPIO negedge level trigger interrupt (32k 3T) */
    GLB_GPIO_INT_TRIG_POS_LEVEL, /*!< GPIO posedge level trigger interrupt (32k 3T) */
} GLB_GPIO_INT_TRIG_Type;

typedef enum {
    GLB_GPIO_INT_CONTROL_SYNC,  /*!< GPIO interrupt sync mode */
    GLB_GPIO_INT_CONTROL_ASYNC, /*!< GPIO interrupt async mode */
} GLB_GPIO_INT_CONTROL_Type;

typedef struct
{
    uint8_t gpioPin;       /*!< GPIO pin num */
    uint8_t trig; /*!< GPIO interrupt trig mode */
    uint8_t intMask;        /*!< GPIO interrupt mask config */
} GLB_GPIO_INT_Cfg_Type;

typedef struct
{
    uint8_t code0FirstTime;              /*!< The clock num of code0 first send */
    uint8_t code1FirstTime;              /*!< The clock num of code1 first send */
    uint16_t codeTotalTime;              /*!< The total clock num of code0/1(high + low */
    uint8_t code0Phase; /*!< low or high level of code0 first send */
    uint8_t code1Phase; /*!< low or high level of code1 first send */
    uint8_t idle;        /*!< the I/O idle level */
    uint8_t fifoDmaThreshold;            /*!< FIFO threshold */
    uint8_t fifoDmaEnable;           /*!< Enable or disable DMA of GPIO */
    uint8_t latch;      /*!< Write or set/clr GPIO level */
} GLB_GPIO_FIFO_CFG_Type;

#define IS_GLB_GPIO_INT_TRIG_TYPE(type) (((type) == GLB_GPIO_INT_TRIG_SYNC_FALLING_EDGE) ||         \
                                         ((type) == GLB_GPIO_INT_TRIG_SYNC_RISING_EDGE) ||          \
                                         ((type) == GLB_GPIO_INT_TRIG_SYNC_LOW_LEVEL) ||            \
                                         ((type) == GLB_GPIO_INT_TRIG_SYNC_HIGH_LEVEL) ||           \
                                         ((type) == GLB_GPIO_INT_TRIG_SYNC_FALLING_RISING_EDGE) ||  \
                                         ((type) == GLB_GPIO_INT_TRIG_ASYNC_FALLING_EDGE) ||        \
                                         ((type) == GLB_GPIO_INT_TRIG_ASYNC_RISING_EDGE) ||         \
                                         ((type) == GLB_GPIO_INT_TRIG_ASYNC_LOW_LEVEL) ||           \
                                         ((type) == GLB_GPIO_INT_TRIG_ASYNC_HIGH_LEVEL))

#define IS_GLB_GPIO_FIFO_INT_TYPE(type) (((type) == GLB_GPIO_FIFO_INT_FER) ||  \
                                         ((type) == GLB_GPIO_FIFO_INT_FIFO) || \
                                         ((type) == GLB_GPIO_FIFO_INT_END) ||  \
                                         ((type) == GLB_GPIO_FIFO_INT_ALL))

#define IS_GLB_GPIO_FIFO_PHASE_TYPE(type) (((type) == GPIO_FIFO_PHASE_FIRST_HIGH) || \
                                           ((type) == GPIO_FIFO_PHASE_FIRST_LOW))

#define IS_GLB_GPIO_FIFO_IDLE_TYPE(type) (((type) == GPIO_FIFO_IDLE_LOW) || \
                                           ((type) == GPIO_FIFO_IDLE_HIGH))

#define IS_GLB_GPIO_FIFO_LATCH_TYPE(type) (((type) == GPIO_FIFO_LATCH_WRITE) || \
                                           ((type) == GPIO_FIFO_LATCH_SETCLEAR))

/*----------*/
#ifndef IPRO_USE_HAL_DRIVER
void GPIO_INT0_IRQHandler(void);
#endif
/*----------*/
int GLB_GPIO_Pad_LeadOut_Sts(uint8_t gpioPin);
/*----------*/
int GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg);
int GLB_GPIO_Func_Init(uint8_t gpioFun, uint8_t *pinList, uint8_t cnt);
int GLB_GPIO_Input_Enable(uint8_t gpioPin);
int GLB_Embedded_Flash_Pad_Enable(uint8_t swapIo2Cs);
int GLB_GPIO_Input_Disable(uint8_t gpioPin);
int GLB_GPIO_Output_Enable(uint8_t gpioPin);
int GLB_GPIO_Output_Disable(uint8_t gpioPin);
int GLB_GPIO_Set_HZ(uint8_t gpioPin);
uint8_t GLB_GPIO_Get_Fun(uint8_t gpioPin);
uint32_t GLB_GPIO_Read(uint8_t gpioPin);
int GLB_GPIO_Write(uint8_t gpioPin, uint32_t val);
int GLB_GPIO_Set(uint8_t gpioPin);
int GLB_GPIO_Clr(uint8_t gpioPin);
int GLB_GPIO_Int_Init(GLB_GPIO_INT_Cfg_Type *intCfg);
int GLB_GPIO_IntMask(uint8_t gpioPin, int intMask);
int GLB_Get_GPIO_IntStatus(uint8_t gpioPin);
int GLB_Clr_GPIO_IntStatus(uint8_t gpioPin);
int GLB_GPIO_INT0_IRQHandler_Install(void);
int GLB_GPIO_INT0_Callback_Install(uint8_t gpioPin, intCallback_Type *cbFun);

#ifndef IPRO_USE_HAL_DRIVER
void GPIO_FIFO_IRQHandler(void);
#endif
int GLB_GPIO_Fifo_Callback_Install(uint8_t intType, intCallback_Type *cbFun);
int GLB_GPIO_Fifo_IRQHandler_Install(void);
int GLB_GPIO_Fifo_Init(GLB_GPIO_FIFO_CFG_Type *cfg);
int GLB_GPIO_Fifo_Push(uint16_t *data, uint16_t len);
uint32_t GLB_GPIO_Fifo_GetCount(void);
int GLB_GPIO_Fifo_Clear(void);
int GLB_GPIO_Fifo_IntMask(uint8_t intType, int intMask);
int GLB_GPIO_Fifo_IntClear(uint8_t intType);
int GLB_GPIO_Fifo_GetIntStatus(uint8_t intType);
int GLB_GPIO_Fifo_Enable(void);
int GLB_GPIO_Fifo_Disable(void);
int GLB_Set_GPIO_IntMod(int gpioPin, GLB_GPIO_INT_CONTROL_Type intCtlMod, GLB_GPIO_INT_TRIG_Type intTrgMod);
int GLB_GPIO_Toggle(int gpioPin);
int GLB_GPIO_IntClear(int gpioPin, int intClear);

#endif /* __DRV_GLB_GPIO_H__ */
