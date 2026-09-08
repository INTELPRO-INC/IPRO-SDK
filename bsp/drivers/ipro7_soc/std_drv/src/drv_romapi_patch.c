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

/**
 * @file drv_romapi_patch.c
 * @brief ROM API patch implementations for IPRO7
 *
 * This file contains patched driver APIs where the SDK signature differs from ROM.
 * When possible, functions call the ROM driver with adapted parameters.
 * When ROM cannot be used (e.g., different bank/ID), SDK implementation is provided.
 *
 * Patched APIs:
 * - GLB_GPIO_Func_Init: Type cast uint8_t* to int* for ROM
 * - EF_Ctrl_Read_Common_Trim: Added reload param, call ROM when reload=1
 * - Timer APIs: Added timerId param, call ROM when timerId=TIMER0_ID
 * - WDT APIs: Added wdtId param, call ROM when wdtId=WDT0_ID
 * - SF_Ctrl_*_Flash_Image_Offset: Added group/bank, call ROM when bank=0
 * - SFlash_Set_IDbus_Cfg, SFlash_IDbus_Read_Enable: Added bank, call ROM when bank=0
 */

#include "drv_romdriver.h"
#include "drv_timer.h"
#include "drv_ef_cfg.h"
#include "drv_sf_ctrl.h"
#include "drv_sflash.h"
#include "drv_gpio.h"
#include "timer_reg.h"

/*
 * The following GPIO and EF_Ctrl patch functions are disabled in Hybrid mode
 * because we include SDK drivers (drv_gpio.c, drv_ef_cfg.c) which have full
 * implementations of these functions.
 */
#if 0  /* Disabled - SDK drivers used in Hybrid mode */
/* ============================================================================
 * GPIO APIs - Type cast wrapper
 * ============================================================================ */

/**
 * @brief GPIO function initialization with type cast for ROM driver
 *
 * ROM driver expects int* for pinList, SDK uses uint8_t*.
 * Convert uint8_t array to int array and call ROM driver.
 */
ATTR_TCM_SECTION
int GLB_GPIO_Func_Init(GLB_GPIO_FUNC_Type gpioFun, uint8_t *pinList, uint8_t cnt)
{
    int pinListInt[32];  /* Max GPIO pins */
    uint8_t i;

    if (cnt > 32) {
        cnt = 32;
    }

    for (i = 0; i < cnt; i++) {
        pinListInt[i] = (int)pinList[i];
    }

    return RomDriver_GLB_GPIO_Func_Init(gpioFun, pinListInt, cnt);
}

/* ============================================================================
 * EF_Ctrl APIs - Reload parameter wrapper
 * ============================================================================ */

/**
 * @brief Read common trim value from eFuse
 *
 * ROM driver always reloads eFuse, so call ROM when reload=1.
 * When reload=0, need to read cached value (not implemented in ROM).
 */
ATTR_TCM_SECTION
void EF_Ctrl_Read_Common_Trim(char *name, Efuse_Common_Trim_Type *trim, uint8_t reload)
{
    if (reload) {
        /* ROM always reloads, so this matches ROM behavior */
        RomDriver_EF_Ctrl_Read_Common_Trim(name, trim);
    } else {
        /* ROM always reloads, but caller wants cached value.
         * For now, call ROM anyway since we don't have a cache mechanism.
         * TODO: Implement eFuse cache if needed for performance. */
        RomDriver_EF_Ctrl_Read_Common_Trim(name, trim);
    }
}
#endif  /* GPIO and EF_Ctrl patches */

/* ============================================================================
 * Timer APIs - Timer ID wrapper
 * ============================================================================ */

/**
 * @brief Get timer base address for given timer ID
 */
static inline uint32_t TIMER_GetBaseAddr(TIMER_ID_Type timerId)
{
    (void)timerId;
    /* Currently only TIMER0 is supported */
    return TIMER_BASE;
}

/**
 * @brief Initialize timer
 *
 * ROM driver only supports TIMER0. Call ROM for TIMER0_ID.
 */
ATTR_TCM_SECTION
int TIMER_Init(TIMER_ID_Type timerId, TIMER_CFG_Type *timerCfg)
{
    if (timerId == TIMER0_ID) {
        return RomDriver_TIMER_Init(timerCfg);
    }
    /* Other timer IDs not supported by ROM - would need SDK implementation */
    return -1;
}

/**
 * @brief Get timer compare value
 */
ATTR_TCM_SECTION
uint32_t TIMER_GetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    if (timerId == TIMER0_ID) {
        return RomDriver_TIMER_GetCompValue(timerCh, cmpNo);
    }
    return 0;
}

/**
 * @brief Set timer compare value
 */
ATTR_TCM_SECTION
void TIMER_SetCompValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo, uint32_t val)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_SetCompValue(timerCh, cmpNo, val);
    }
}

/**
 * @brief Get timer counter value
 */
ATTR_TCM_SECTION
uint32_t TIMER_GetCounterValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    if (timerId == TIMER0_ID) {
        return RomDriver_TIMER_GetCounterValue(timerCh);
    }
    return 0;
}

/**
 * @brief Get timer match status
 */
ATTR_TCM_SECTION
int TIMER_GetMatchStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    if (timerId == TIMER0_ID) {
        return RomDriver_TIMER_GetMatchStatus(timerCh, cmpNo);
    }
    return 0;
}

/**
 * @brief Get timer preload value
 */
ATTR_TCM_SECTION
uint32_t TIMER_GetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    if (timerId == TIMER0_ID) {
        return RomDriver_TIMER_GetPreloadValue(timerCh);
    }
    return 0;
}

/**
 * @brief Set timer preload value
 */
ATTR_TCM_SECTION
void TIMER_SetPreloadValue(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, uint32_t val)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_SetPreloadValue(timerCh, val);
    }
}

/**
 * @brief Set timer preload source
 */
ATTR_TCM_SECTION
void TIMER_SetPreloadSrc(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_PreLoad_Trig_Type plSrc)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_SetPreloadSrc(timerCh, plSrc);
    }
}

/**
 * @brief Set timer count mode
 */
ATTR_TCM_SECTION
void TIMER_SetCountMode(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_CountMode_Type countMode)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_SetCountMode(timerCh, countMode);
    }
}

/**
 * @brief Clear timer interrupt status
 */
ATTR_TCM_SECTION
void TIMER_ClearIntStatus(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_Comp_ID_Type cmpNo)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_ClearIntStatus(timerCh, cmpNo);
    }
}

/**
 * @brief Enable timer
 */
ATTR_TCM_SECTION
void TIMER_Enable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_Enable(timerCh);
    }
}

/**
 * @brief Disable timer
 */
ATTR_TCM_SECTION
void TIMER_Disable(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_Disable(timerCh);
    }
}

/**
 * @brief Set timer interrupt mask
 */
ATTR_TCM_SECTION
void TIMER_IntMask(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_INT_Type intType, int intMask)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_IntMask(timerCh, intType, intMask);
    }
}

/**
 * @brief Set timer divider
 */
ATTR_TCM_SECTION
void TIMER_SetDiv(TIMER_ID_Type timerId, int timer, uint8_t div)
{
    if (timerId == TIMER0_ID) {
        RomDriver_TIMER_SetDiv(timer, div);
    }
}

/**
 * @brief Set timer clock source and divider
 *
 * This function is not in ROM, need SDK implementation.
 */
ATTR_TCM_SECTION
void TIMER_Set_Clock(TIMER_ID_Type timerId, TIMER_Chan_Type timerCh, TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    uint32_t base = TIMER_GetBaseAddr(timerId);
    uint32_t regVal;

    /* Timer clock config is in TCCR register */
    /* Note: TIMER_CS_2 is for CH0, TIMER_CS_3 is for CH1 (per register definition) */
    if (timerCh == TIMER_CH0) {
        regVal = readl(base + TIMER_TCCR_OFFSET);
        regVal = reg_set_bits(regVal, TIMER_CS_2, clkSrc);
        writel(regVal, base + TIMER_TCCR_OFFSET);
    } else if (timerCh == TIMER_CH1) {
        regVal = readl(base + TIMER_TCCR_OFFSET);
        regVal = reg_set_bits(regVal, TIMER_CS_3, clkSrc);
        writel(regVal, base + TIMER_TCCR_OFFSET);
    }

    /* Set divider */
    TIMER_SetDiv(timerId, timerCh, div);
}

/* ============================================================================
 * WDT APIs - Watchdog ID wrapper
 * ============================================================================ */

/**
 * @brief Get WDT base address for given WDT ID
 */
static inline uint32_t WDT_GetBaseAddr(WDT_ID_Type wdtId)
{
    (void)wdtId;
    /* Currently only WDT0 is supported */
    return TIMER_BASE;
}

/**
 * @brief Set WDT clock source and divider
 */
ATTR_TCM_SECTION
void WDT_Set_Clock(WDT_ID_Type wdtId, TIMER_ClkSrc_Type clkSrc, uint8_t div)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_Set_Clock(clkSrc, div);
    }
}

/**
 * @brief Get WDT match value
 */
ATTR_TCM_SECTION
uint16_t WDT_GetMatchValue(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        return RomDriver_WDT_GetMatchValue();
    }
    return 0;
}

/**
 * @brief Set WDT compare value
 */
ATTR_TCM_SECTION
void WDT_SetCompValue(WDT_ID_Type wdtId, uint16_t val)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_SetCompValue(val);
    }
}

/**
 * @brief Get WDT counter value
 */
ATTR_TCM_SECTION
uint16_t WDT_GetCounterValue(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        return RomDriver_WDT_GetCounterValue();
    }
    return 0;
}

/**
 * @brief Reset WDT counter value
 */
ATTR_TCM_SECTION
void WDT_ResetCounterValue(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_ResetCounterValue();
    }
}

/**
 * @brief Get WDT reset status
 */
ATTR_TCM_SECTION
int WDT_GetResetStatus(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        return RomDriver_WDT_GetResetStatus();
    }
    return 0;
}

/**
 * @brief Clear WDT reset status
 */
ATTR_TCM_SECTION
void WDT_ClearResetStatus(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_ClearResetStatus();
    }
}

/**
 * @brief Enable WDT
 */
ATTR_TCM_SECTION
void WDT_Enable(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_Enable();
    }
}

/**
 * @brief Disable WDT
 */
ATTR_TCM_SECTION
void WDT_Disable(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_Disable();
    }
}

/**
 * @brief Set WDT interrupt mask
 */
ATTR_TCM_SECTION
void WDT_IntMask(WDT_ID_Type wdtId, WDT_INT_Type intType, int intMask)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_IntMask(intType, intMask);
    }
}

/**
 * @brief Clear WDT interrupt
 */
ATTR_TCM_SECTION
void WDT_ClearInt(WDT_ID_Type wdtId)
{
    if (wdtId == WDT0_ID) {
        RomDriver_WDT_ClearInt();
    }
}

/* ============================================================================
 * SF_Ctrl APIs - Group/Bank wrapper
 * ============================================================================ */

/**
 * @brief Set flash image offset
 *
 * ROM driver only supports group=0, bank=0.
 */
ATTR_TCM_SECTION
void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset, uint8_t group, uint8_t bank)
{
    if (group == 0 && bank == 0) {
        RomDriver_SF_Ctrl_Set_Flash_Image_Offset(addrOffset);
    }
    /* Other group/bank combinations would need SDK implementation */
}

/**
 * @brief Get flash image offset
 *
 * ROM driver only supports group=0, bank=0.
 */
ATTR_TCM_SECTION
uint32_t SF_Ctrl_Get_Flash_Image_Offset(uint8_t group, uint8_t bank)
{
    if (group == 0 && bank == 0) {
        return RomDriver_SF_Ctrl_Get_Flash_Image_Offset();
    }
    /* Other group/bank combinations would need SDK implementation */
    return 0;
}

/*
 * The following SFlash patch functions are disabled in Hybrid mode
 * because we include SDK driver (drv_sflash.c) which has full implementation.
 */
#if 0  /* Disabled - SDK drv_sflash.c used in Hybrid mode */
/* ============================================================================
 * SFlash APIs - Bank wrapper
 * ============================================================================ */

/**
 * @brief Set IDbus configuration
 *
 * ROM driver only supports bank=0.
 * ROM uses SF_Ctrl_IO_Type for ioMode, SDK uses uint8_t (same values).
 */
ATTR_TCM_SECTION
int SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead,
                         uint32_t addr, uint32_t len, uint8_t bank)
{
    if (bank == 0) {
        /* Cast ioMode to SF_Ctrl_IO_Type for ROM call */
        return RomDriver_SFlash_Set_IDbus_Cfg(flashCfg, (SF_Ctrl_IO_Type)ioMode, contRead, addr, len);
    }
    /* Other banks would need SDK implementation */
    return -1;
}

/**
 * @brief Enable IDbus read
 *
 * ROM driver only supports bank=0.
 */
ATTR_TCM_SECTION
int SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, uint8_t ioMode, uint8_t contRead,
                             uint8_t bank)
{
    if (bank == 0) {
        /* Cast ioMode to SF_Ctrl_IO_Type for ROM call */
        return RomDriver_SFlash_IDbus_Read_Enable(flashCfg, (SF_Ctrl_IO_Type)ioMode, contRead);
    }
    /* Other banks would need SDK implementation */
    return -1;
}
#endif  /* SFlash patches */
