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


#ifndef __DRV_TIMER_H__
#define __DRV_TIMER_H__

#include "timer_reg.h"
#include "drv_common.h"

typedef enum {
    TIMER0_ID,    /*!< TIMER0 port define */
    TIMER_ID_MAX, /*!< TIMER MAX ID define */
} TIMER_ID_Type;

typedef enum {
    WDT0_ID,    /*!< Watchdog0 port define */
    WDT_ID_MAX, /*!< Watchdog MAX ID define */
} WDT_ID_Type;

typedef enum {
    TIMER_CH0,    /*!< TIMER channel 0 port define */
    TIMER_CH1,    /*!< TIMER channel 1 port define */
    TIMER_CH_MAX, /*!<  */
} TIMER_Chan_Type;

typedef enum {
    TIMER_CLKSRC_FCLK, /*!< TIMER clock source :System CLK */
    TIMER_CLKSRC_32K,  /*!< TIMER clock source :32K CLK */
    TIMER_CLKSRC_1K,   /*!< TIMER clock source :1K CLK,Only for Timer not for Watchdog */
    TIMER_CLKSRC_XTAL, /*!< TIMER clock source :XTAL CLK */
    TIMER_CLKSRC_GPIO, /*!< TIMER clock source :GPIO */
    TIMER_CLKSRC_NO,   /*!< TIMER clock source :No CLK */
} TIMER_ClkSrc_Type;

typedef enum {
    TIMER_COMP_ID_0, /*!< TIMER match compare ID 0 define */
    TIMER_COMP_ID_1, /*!< TIMER match compare ID 1 define */
    TIMER_COMP_ID_2, /*!< TIMER match compare ID 2 define */
} TIMER_Comp_ID_Type;

typedef enum {
    TIMER_PRELOAD_TRIG_NONE,  /*!< TIMER no preload source, just free run */
    TIMER_PRELOAD_TRIG_COMP0, /*!< TIMER count register preload triggered by comparator 0 */
    TIMER_PRELOAD_TRIG_COMP1, /*!< TIMER count register preload triggered by comparator 1 */
    TIMER_PRELOAD_TRIG_COMP2, /*!< TIMER count register preload triggered by comparator 2 */
} TIMER_PreLoad_Trig_Type;

typedef enum {
    TIMER_COUNT_PRELOAD, /*!< TIMER count register preload from comparator register */
    TIMER_COUNT_FREERUN, /*!< TIMER count register free run */
} TIMER_CountMode_Type;

typedef enum {
    TIMER_GPIO_POS, /*!< TIMER gpio plarity posedge */
    TIMER_GPIO_NEG, /*!< TIMER gpio plarity negedge */
} TIMER_GPIO_Polarity_Type;

typedef enum {
    TIMER_INT_COMP_0, /*!< Comparator 0 match cause interrupt */
    TIMER_INT_COMP_1, /*!< Comparator 1 match cause interrupt */
    TIMER_INT_COMP_2, /*!< Comparator 2 match cause interrupt */
    TIMER_INT_ALL,    /*!<  */
} TIMER_INT_Type;

typedef enum {
    WDT_INT,     /*!< Comparator 0 match cause interrupt */
    WDT_INT_ALL, /*!<  */
} WDT_INT_Type;

typedef struct
{
    TIMER_Chan_Type timerCh;           /*!< Timer channel */
    TIMER_ClkSrc_Type clkSrc;          /*!< Timer clock source */
    TIMER_PreLoad_Trig_Type plTrigSrc; /*!< Timer count register preload trigger source slelect */
    TIMER_CountMode_Type countMode;    /*!< Timer count mode */
    uint8_t clockDivision;             /*!< Timer clock divison value */
    uint32_t matchVal0;                /*!< Timer match 0 value 0 */
    uint32_t matchVal1;                /*!< Timer match 1 value 0 */
    uint32_t matchVal2;                /*!< Timer match 2 value 0 */
    uint32_t preLoadVal;               /*!< Timer preload value */
} TIMER_CFG_Type;

#define IS_TIMER_ID_TYPE(type) (((type) == TIMER0_ID) || \
                                ((type) == TIMER_ID_MAX))

#define IS_WDT_ID_TYPE(type) (((type) == WDT0_ID) || \
                              ((type) == WDT_ID_MAX))

#define IS_TIMER_CHAN_TYPE(type) (((type) == TIMER_CH0) || \
                                  ((type) == TIMER_CH1) || \
                                  ((type) == TIMER_CH_MAX))

#define IS_TIMER_CLKSRC_TYPE(type) (((type) == TIMER_CLKSRC_FCLK) || \
                                    ((type) == TIMER_CLKSRC_32K) ||  \
                                    ((type) == TIMER_CLKSRC_1K) ||   \
                                    ((type) == TIMER_CLKSRC_XTAL) || \
                                    ((type) == TIMER_CLKSRC_GPIO) || \
                                    ((type) == TIMER_CLKSRC_NO))

#define IS_TIMER_COMP_ID_TYPE(type) (((type) == TIMER_COMP_ID_0) || \
                                     ((type) == TIMER_COMP_ID_1) || \
                                     ((type) == TIMER_COMP_ID_2))

#define IS_TIMER_PRELOAD_TRIG_TYPE(type) (((type) == TIMER_PRELOAD_TRIG_NONE) ||  \
                                          ((type) == TIMER_PRELOAD_TRIG_COMP0) || \
                                          ((type) == TIMER_PRELOAD_TRIG_COMP1) || \
                                          ((type) == TIMER_PRELOAD_TRIG_COMP2))

#define IS_TIMER_COUNTMODE_TYPE(type) (((type) == TIMER_COUNT_PRELOAD) || \
                                       ((type) == TIMER_COUNT_FREERUN))

#define IS_TIMER_GPIO_POLARITY_TYPE(type) (((type) == TIMER_GPIO_POS) || \
                                           ((type) == TIMER_GPIO_NEG))

#define IS_TIMER_INT_TYPE(type) (((type) == TIMER_INT_COMP_0) || \
                                 ((type) == TIMER_INT_COMP_1) || \
                                 ((type) == TIMER_INT_COMP_2) || \
                                 ((type) == TIMER_INT_ALL))

#define IS_WDT_INT_TYPE(type) (((type) == WDT_INT) || \
                               ((type) == WDT_INT_ALL))

#define WDT_ENABLE_ACCESS(baseAddr)                                                                                \
    {                                                                                                              \
        writel(reg_set_bits(readl(baseAddr + TIMER_WFAR_OFFSET), TIMER_WFAR, 0xBABA), baseAddr + TIMER_WFAR_OFFSET); \
        writel(reg_set_bits(readl(baseAddr + TIMER_WSAR_OFFSET), TIMER_WSAR, 0xEB10), baseAddr + TIMER_WSAR_OFFSET); \
    }

#ifndef IPRO_USE_HAL_DRIVER
void TIMER0_CH0_IRQHandler(void);
void TIMER0_CH1_IRQHandler(void);
void TIMER0_WDT_IRQHandler(void);
void TIMER1_CH0_IRQHandler(void);
void TIMER1_CH1_IRQHandler(void);
//void TIMER1_WDT_IRQHandler(void);
#endif
int TIMER_Init(TIMER_ID_Type timerId, TIMER_CFG_Type *timerCfg);
int TIMER_DeInit(TIMER_ID_Type timerId);
uint32_t TIMER_GetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo);
void TIMER_SetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val);
void TIMER_CompValueEffectImmediately(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, int enable);
uint32_t TIMER_GetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
void TIMER_ResetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
int TIMER_GetMatchStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo);
uint32_t TIMER_GetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
void TIMER_SetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint32_t val);
void TIMER_SetPreloadSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc);
void TIMER_SetCountMode(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode);
void TIMER_ClearIntStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo);
void TIMER_Enable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
void TIMER_Disable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
void TIMER_IntMask(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_INT_Type intType, int intMask);
void TIMER_GPIOSetPolarity(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_GPIO_Polarity_Type polarity);
void TIMER_CH0_SetMeasurePulseWidth(TIMER_ID_Type timerId, int enable);
uint32_t TIMER_CH0_GetMeasurePulseWidth(TIMER_ID_Type timerId);
void TIMER_ForceClockDivision(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh);
void TIMER_SetPreloadTrigSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc);
void TIMER_Int_Callback_Install(TIMER_ID_Type timerId, TIMER_Chan_Type timerChan, TIMER_INT_Type intType, intCallback_Type *cbFun);
void TIMER_SetDiv(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint8_t div);

void WDT_Set_Clock(WDT_ID_Type wdtId, TIMER_ClkSrc_Type clkSrc, uint8_t div);
uint16_t WDT_GetMatchValue(WDT_ID_Type wdtId);
void WDT_SetCompValue(WDT_ID_Type wdtId, uint16_t val);
void WDT_CompValueEffectImmediately(WDT_ID_Type wdtId, int enable);
uint16_t WDT_GetCounterValue(WDT_ID_Type wdtId);
void WDT_ResetCounterValue(WDT_ID_Type wdtId);
int WDT_GetResetStatus(WDT_ID_Type wdtId);
void WDT_ClearResetStatus(WDT_ID_Type wdtId);
void WDT_Enable(WDT_ID_Type wdtId);
void WDT_Disable(WDT_ID_Type wdtId);
void WDT_IntMask(WDT_ID_Type wdtId, WDT_INT_Type intType, int intMask);
void WDT_ForceClockDivision(WDT_ID_Type wdtId);
void WDT_Int_Callback_Install(WDT_ID_Type wdtId, WDT_INT_Type wdtInt, intCallback_Type *cbFun);
void WDT_GPIOSetPolarity(WDT_ID_Type wdtId, TIMER_GPIO_Polarity_Type polarity);
void WDT_ClearInt(WDT_ID_Type wdtId);

#endif /* __DRV_TIMER_H__ */
